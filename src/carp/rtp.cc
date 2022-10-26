#include "carp/rtp.h"

#include "msgfmt.h"
#include "preload_internal.h"

namespace {

/* convert double array to string (only used for printing in debug log) */
std::string darr2str(double *v, size_t vsz) {
  std::string rv;
  for (size_t i = 0 ; i < vsz ; i++) {
    if (i == 0) {
      rv = std::to_string(v[i]);
    } else {
      rv += "," + std::to_string(v[i]);
    }
  }
  return rv;
}

/* Hash function, from
 * https://stackoverflow.com/questions/8317508/hash-function-for-a-string
 */
#define A 54059   /* a prime */
#define B 76963   /* another prime */
#define C 86969   /* yet another prime */
#define FIRSTH 37 /* also prime */

uint32_t hash_str(const char* data, int slen) {
  uint32_t h = FIRSTH;
  for (int sidx = 0; sidx < slen; sidx++) {
    char s = data[sidx];
    h = (h * A) ^ (s * B);
  }
  return h % C;
}

inline int cube(int x) { return x * x * x; }

inline int square(int x) { return x * x; }

int compute_fanout_better(int world_sz, int* fo_arr) {
  int rv = 0;

  /* Init fanout of the tree for each stage */
  if (world_sz % 4) {
    ABORT("RTP world size must be a multiple of 4");
  }

  /* base setup */
  fo_arr[1] = 1;
  fo_arr[2] = 1;
  fo_arr[3] = world_sz;

  int wsz_remain = world_sz;
  double root = pow(wsz_remain, 1.0 / 3);
  int f1_cand = (int)root;

  while (f1_cand < wsz_remain) {
    if (wsz_remain % f1_cand == 0) {
      fo_arr[1] = f1_cand;
      wsz_remain /= f1_cand;
      break;
    }

    f1_cand++;
  }

  root = pow(wsz_remain, 1.0 / 2);
  int f2_cand = (int)root;

  while (f2_cand < wsz_remain) {
    if (wsz_remain % f2_cand == 0) {
      fo_arr[2] = f2_cand;
      wsz_remain /= f2_cand;
      break;
    }

    f2_cand++;
  }

  fo_arr[3] = world_sz / (fo_arr[1] * fo_arr[2]);
  assert(fo_arr[3] * fo_arr[2] * fo_arr[1] == world_sz);

  if (pctx.my_rank == 0)
    logf(LOG_INFO, "RTP Fanout: %d/%d/%d", fo_arr[1], fo_arr[2], fo_arr[3]);

  return rv;
}

int compute_fanout(const int world_sz, int* fo_arr) {
  int rv = 0;

  /* Init fanout of the tree for each stage */
  if (world_sz % 4) {
    ABORT("RTP world size must be a multiple of 4");
  }

  int wsz_remain = world_sz;

  int fo_tmp = 1;
  while (cube(fo_tmp + 1) <= world_sz) {
    fo_tmp++;
    if (wsz_remain % fo_tmp == 0) {
      fo_arr[3] = fo_tmp;
    }
  }
  assert(fo_arr[3] >= 2);
  wsz_remain = world_sz / fo_arr[3];
  fo_tmp = 1;

  while (square(fo_tmp + 1) <= world_sz) {
    fo_tmp++;
    if (wsz_remain % fo_tmp == 0) {
      fo_arr[2] = fo_tmp;
    }
  }

  fo_arr[1] = world_sz / (fo_arr[2] * fo_arr[3]);

  if (pctx.my_rank == 0)
    logf(LOG_INFO, "RTP Fanout: %d/%d/%d", fo_arr[1], fo_arr[2], fo_arr[3]);

  return rv;
}
}  // namespace

namespace pdlfs {
namespace carp {
RTP::RTP(Carp* carp, const CarpOptions& opts)
    : carp_(carp),
      sh_(nullptr),
      data_buffer_(opts.rtp_pvtcnt),
      round_num_(0),
      my_rank_(opts.my_rank),
      num_ranks_(opts.num_ranks) {
  if (state_.GetState() != RenegState::INIT) {
    logf(LOG_ERRO, "rtp_init: can initialize only in init stage");
    return;
  }

  if (opts.sctx->type != SHUFFLE_XN) {
    logf(LOG_ERRO, "Only 3-hop is supported by the RTP protocol");
    return;
  }

  if (opts.rtp_pvtcnt[1] > kMaxPivots) {
    logf(LOG_ERRO, "pvtcnt_s1 exceeds MAX_PIVOTS");
    return;
  }

  if (opts.rtp_pvtcnt[2] > kMaxPivots) {
    logf(LOG_ERRO, "pvtcnt_s1 exceeds MAX_PIVOTS");
    return;
  }

  if (opts.rtp_pvtcnt[3] > kMaxPivots) {
    logf(LOG_ERRO, "pvtcnt_s1 exceeds MAX_PIVOTS");
    return;
  }

  sh_ = static_cast<xn_ctx_t*>(opts.sctx->rep);

  //  rctx->carp = carp;

  /* Number of pivots generated by each stage */
  pvtcnt_[1] = opts.rtp_pvtcnt[1];
  pvtcnt_[2] = opts.rtp_pvtcnt[2];
  pvtcnt_[3] = opts.rtp_pvtcnt[3];

  if (pctx.my_rank == 0) {
    logf(LOG_INFO, "[rtp_init] pivot_count: %d/%d/%d", pvtcnt_[1], pvtcnt_[2],
         pvtcnt_[3]);
  }

  Status s = InitTopology();

  if (s.ok()) {
    state_.UpdateState(RenegState::READY);
  }
}

Status RTP::InitRound() {
  Status s = Status::OK();

  carp_->mutex_.AssertHeld();
  mutex_.Lock();

  if (state_.GetState() == RenegState::READY) {
    s = BroadcastBegin();
    state_.UpdateState(RenegState::READYBLOCK);
    carp_->UpdateState(MainThreadState::MT_READYBLOCK);
  }

  mutex_.Unlock();

  while (carp_->GetCurState() != MainThreadState::MT_READY) {
    carp_->cv_.Wait();
  }

  return s;
}

Status RTP::HandleMessage(void* buf, unsigned int bufsz, int src,
                          uint32_t type) {
  Status s = Status::OK();
  switch (type) {
    case MSGFMT_RTP_BEGIN:
      s = HandleBegin(buf, bufsz, src);
      break;
    case MSGFMT_RTP_PIVOT:
      s = HandlePivots(buf, bufsz, src);
      break;
    case MSGFMT_RTP_PVT_BCAST:
      s = HandlePivotBroadcast(buf, bufsz, src);
      break;
    default:
      ABORT("rtp_handle_message: unknown msg_type");
      break;
  }

  return s;
}

int RTP::NumRounds() const { return round_num_; }

// --------- PRIVATE METHODS ---------

Status RTP::InitTopology() {
  Status s = Status::OK();

  if (state_.GetState() != RenegState::INIT) {
    logf(LOG_DBUG, "rtp_topology_init: can initialize only in init stage");
    return Status::AssertionFailed("RTP: Not in INIT state");
  }

  int grank, gsz;
  grank = my_rank_;
  gsz = num_ranks_;

  compute_fanout_better(gsz, fanout_);

  /* Init peers for each stage */
  int s1mask = ~(fanout_[1] - 1);
  int s2mask = ~(fanout_[1] * fanout_[2] - 1);
  int s3mask = ~((fanout_[1] * fanout_[2] * fanout_[3]) - 1);

  int s1root = grank & s1mask;
  root_[1] = s1root;

  for (int pidx = 0; pidx < fanout_[1]; pidx++) {
    int jump = 1;
    int peer = s1root + pidx * jump;

    if (peer >= gsz) break;

    peers_[1][pidx] = peer;
  }

  int s2root = grank & s2mask;
  root_[2] = s2root;

  for (int pidx = 0; pidx < fanout_[2]; pidx++) {
    int jump = fanout_[1];
    int peer = s2root + pidx * jump;

    if (peer >= gsz) break;

    peers_[2][pidx] = peer;
  }

  int s3root = grank & s3mask;
  root_[3] = s3root;

  for (int pidx = 0; pidx < fanout_[3]; pidx++) {
    int jump = fanout_[1] * fanout_[2];
    int peer = s3root + pidx * jump;

    if (peer >= gsz) break;

    peers_[3][pidx] = peer;
  }

  return s;
}

Status RTP::BroadcastBegin() {
  Status s = Status::OK();
  mutex_.AssertHeld();

  logf(LOG_DBUG, "broadcast_rtp_begin: at rank %d, to %d", my_rank_,
       root_[3]);

  char buf[256];
  int buflen = msgfmt_encode_rtp_begin(buf, 256, my_rank_, round_num_);
  SendToRank(buf, buflen, root_[3], MSGFMT_RTP_BEGIN);

  return s;
}

Status RTP::SendToRank(const void* buf, int bufsz, int rank, uint32_t type) {
  xn_shuffle_priority_send(sh_, (void*)buf, bufsz, 0/*XXXCDC remove epoch?*/,
                           rank, my_rank_, type);
  return Status::OK();
}

Status RTP::SendToAll(int stage, const void* buf, int bufsz,
                      bool exclude_self, uint32_t type) {
  for (int rank_idx = 0; rank_idx < fanout_[stage]; rank_idx++) {
    int drank = peers_[stage][rank_idx];

    /*  Not enabled by default; my_rank is set to -1 unless overridden */
    if (drank == my_rank_ && exclude_self) continue;

    logf(LOG_DBUG, "send_to_all: sending from %d to %d", my_rank_, drank);

    xn_shuffle_priority_send(sh_, (void*)buf, bufsz, 0/*XXXCDC rm epoch?*/,
                             drank, my_rank_, type);
  }

  return Status::OK();
}

Status RTP::SendToChildren(const void* buf, int bufsz, bool exclude_self,
                           uint32_t type) {
  Status s = Status::OK();

#define AMROOT(x) (my_rank_ == root_[(x)])

  if (AMROOT(3)) {
    SendToAll(3, buf, bufsz, exclude_self, type);
  }
  if (AMROOT(2)) {
    SendToAll(2, buf, bufsz, exclude_self, type);
  }
  if (AMROOT(1)) {
    SendToAll(1, buf, bufsz, exclude_self, type);
  }

#undef AMROOT

  return s;
}

Status RTP::HandleBegin(void* buf, unsigned int bufsz, int src) {
  Status s = Status::OK();

  int srank, msg_round_num;
  msgfmt_decode_rtp_begin(buf, bufsz, &srank, &msg_round_num);

  logf(LOG_DBUG, "Received RTP_BEGIN_%d at Rank %d from %d", msg_round_num,
       my_rank_, src);

  bool activated_now = false;

  carp_->mutex_.Lock();
  mutex_.Lock();

  if (msg_round_num < round_num_) {
    logf(LOG_DBG2,
         "Stale RENEG_BEGIN received at Rank %d, dropping"
         "(theirs: %d, ours: %d)",
         my_rank_, msg_round_num, round_num_);
  } else if (state_.GetState() == RenegState::READY ||
             state_.GetState() == RenegState::READYBLOCK) {
    /* If we're ready for Round R, no way we can receive R+1 first */
    assert(msg_round_num == round_num_);
    carp_->UpdateState(MainThreadState::MT_BLOCK);
    state_.UpdateState(RenegState::PVTSND);
    reneg_bench_.MarkActive();
    activated_now = true;
  } else if (msg_round_num == round_num_ + 1) {
    state_.MarkNextRoundStart(msg_round_num);
  } else if (msg_round_num != round_num_) {
    /* If round_nums are equal, msg is duplicate and DROP */
    ABORT("rtp_handle_rtp_begin: unexpected msg_round_num recvd");
  } else {
    assert(msg_round_num == round_num_);
  }

  mutex_.Unlock();
  carp_->mutex_.Unlock();

  if (activated_now) {
    /* Can reuse the same RTP_BEGIN buf. src_rank is anyway sent separately
     * If we're an S3 root, prioritize Stage 3 sending first, so as to
     * trigger other leaf broadcasts in parallel
     * No need to send it to ourself since we just got it....
     * */
    SendToChildren(buf, bufsz, /* exclude_self */ true, MSGFMT_RTP_BEGIN);

    /* send pivots to s1root now */
    const int stage_idx = 1;

    carp_->mutex_.Lock();

    int pvtcnt = pvtcnt_[stage_idx];

    int pvt_buf_sz = msgfmt_bufsize_rtp_pivots(pvtcnt);
    char pvt_buf[pvt_buf_sz];
    int pvt_buf_len;

    PivotUtils::CalculatePivots(carp_, pvtcnt);
    carp_->LogPivots(pvtcnt);

    pvt_buf_len = msgfmt_encode_rtp_pivots(
        pvt_buf, pvt_buf_sz, round_num_, stage_idx, my_rank_, carp_->my_pivots_,
        carp_->my_pivot_width_, pvtcnt, false);

    carp_->mutex_.Unlock();

    logf(LOG_DBG2, "pvt_calc_local @ R%d: bufsz: %d, bufhash: %u",
         pctx.my_rank, pvt_buf_len, ::hash_str(pvt_buf, pvt_buf_len));

    logf(LOG_DBUG, "sending pivots, count: %d", pvtcnt);

    carp_->LogPrintf("RENEG_RTP_PVT_SEND");

    SendToRank(pvt_buf, pvt_buf_len, root_[1], MSGFMT_RTP_PIVOT);
  }

  return s;
}

Status RTP::HandlePivots(void* buf, unsigned int bufsz, int src) {
  Status s = Status::OK();

  int msg_round_num, stage_num, sender_id, num_pivots;
  double pivot_width;
  double* pivots;

  logf(LOG_DBG2, "rtp_handle_reneg_pivot: bufsz: %u, bufhash, %u", bufsz,
       ::hash_str((char*)buf, bufsz));

  msgfmt_decode_rtp_pivots(buf, bufsz, &msg_round_num, &stage_num, &sender_id,
                           &pivots, &pivot_width, &num_pivots, false);
  /* stage_num refers to the stage the pivots were generated at.
   * merged_pvtcnt must correspond to the next stage
   */
  int merged_pvtcnt =
      (stage_num >= 3) ? (num_ranks_ + 1) : pvtcnt_[stage_num + 1];

  assert(num_pivots <= kMaxPivots);

  logf(LOG_DBUG, "rtp_handle_reneg_pivot: S%d %d pivots from %d", stage_num,
       num_pivots, sender_id);

  if (num_pivots >= 4) {
    logf(LOG_DBG2, "rtp_handle_reneg_pivot: %.2f %.2f %.2f %.2f ...",
         pivots[0], pivots[1], pivots[2], pivots[3]);
  }

  mutex_.Lock();

  int stage_pivot_count = 0;

#define EXPECTED_ITEMS_FOR_STAGE(stage_idx, item_count) \
  (fanout_[(stage_idx)] == (item_count))

  if (msg_round_num != round_num_) {
    stage_pivot_count = data_buffer_.StoreData(
        stage_num, pivots, num_pivots, pivot_width, /* isnextround */ true);

    /* If we're receiving a pivot for a future round, we can never have
     * received all pivots for a future round, as we also expect one pivot
     * from ourselves, which can't be received out of turn
     *
     * This assumption is important, as it enables us to not have to store
     * and replay the handler logic for when all items of a future round/stage
     * have been received
     */
    assert(!EXPECTED_ITEMS_FOR_STAGE(stage_num, stage_pivot_count));
  } else {
    stage_pivot_count = data_buffer_.StoreData(
        stage_num, pivots, num_pivots, pivot_width, /*isnextround */ false);
  }
  mutex_.Unlock();

  logf(LOG_INFO,
       "rtp_handle_reneg_pivot: S%d at Rank %d, item from %d. Total: %d",
       stage_num, my_rank_, src, stage_pivot_count);

  if (EXPECTED_ITEMS_FOR_STAGE(stage_num, stage_pivot_count)) {
    double merged_pivots[merged_pvtcnt];
    double merged_width;

    ComputeAggregatePivots(stage_num, merged_pvtcnt, merged_pivots,
                           merged_width);

    logf(LOG_DBUG, "compute_aggr_pvts: R%d - %s - %.1f (cnt: %d)", my_rank_,
         darr2str(merged_pivots, merged_pvtcnt).c_str(),
         merged_width, merged_pvtcnt);

    logf(LOG_INFO, "rtp_handle_reneg_pivot: S%d at Rank %d, collected",
         stage_num, my_rank_);

    size_t next_buf_sz = msgfmt_bufsize_rtp_pivots(merged_pvtcnt);
    char next_buf[next_buf_sz];

    if (stage_num < STAGES_MAX) {
      logf(LOG_DBUG, "rtp_handle_reneg_pivot: choice 1");

      // char next_buf[2048];

      int next_buf_len = msgfmt_encode_rtp_pivots(
          next_buf, next_buf_sz, msg_round_num, stage_num + 1, my_rank_,
          merged_pivots, merged_width, merged_pvtcnt, /* bcast */ false);

      int new_dest = stage_num == 1 ? root_[2] : root_[3];

      SendToRank(next_buf, next_buf_len, new_dest, MSGFMT_RTP_PIVOT);
    } else {
      /* New pivots need to be broadcast from S3. We send them back to
       * ourself (s3root) so that the broadcast code can be cleanly
       * contained in rtp_handle_pivot_bcast
       */
      logf(LOG_DBUG, "rtp_handle_reneg_pivot: choice 2 @ %d", root_[3]);

      assert(my_rank_ == root_[3]);

      reneg_bench_.MarkPvtBcast();

      /* XXX: num_pivots_ = comm_sz, 2048B IS NOT SUFFICIENT */
      int next_buf_len = msgfmt_encode_rtp_pivots(
          next_buf, next_buf_sz, msg_round_num, stage_num + 1, my_rank_,
          merged_pivots, merged_width, merged_pvtcnt, /* bcast */ true);

      carp_->LogPrintf("RENEG_RTP_PVT_MASS %f",
                       (merged_pvtcnt - 1) * merged_width);

      SendToRank(next_buf, next_buf_len, root_[3], MSGFMT_RTP_PVT_BCAST);
    }  // if
  }    // if

  return s;
}

Status RTP::HandlePivotBroadcast(void* buf, unsigned int bufsz, int src) {
  /* You only expect to receive this message once per-round;
   * TODO: parse the round-num for a FATAL error verification
   * but no intermediate state change necessary here. Process
   * the update and then move to READY
   */
  Status s = Status::OK();

  mutex_.Lock();

  RenegState rstate = state_.GetState();

  if (rstate != RenegState::PVTSND) {
    ABORT("rtp_handle_pivot_bcast: unexpected pivot bcast");
  }

  mutex_.Unlock();

  /* send_to_all here excludes self */
  SendToChildren(buf, bufsz, /* exclude_self */ true, MSGFMT_RTP_PVT_BCAST);

  int round_num, stage_num, sender_id, num_pivots;
  double pivot_width;
  double* pivots;

  msgfmt_decode_rtp_pivots(buf, bufsz, &round_num, &stage_num, &sender_id,
                           &pivots, &pivot_width, &num_pivots, true);

  logf(LOG_DBUG, "rtp_handle_pivot_bcast: received pivots at %d from %d",
       my_rank_, src);

  if (my_rank_ == 0) {
    logf(LOG_DBUG, "rtp_handle_pivot_bcast: pivots @ %d: %s (%.1f)", my_rank_,
         darr2str(pivots, num_pivots).c_str(), pivot_width);

    logf(LOG_INFO, "-- carp round %d completed at rank 0 --", round_num_);
  }

  /* Install pivots, Reset state, and signal back to the main thread */
  bool replay_rtp_begin_flag = false;

  /* If next round has already started, replay its messages from buffers.
   * If not, mark self as READY, and wake up Main Thread
   */
  carp_->mutex_.Lock();
  mutex_.Lock();

  carp_->LogReneg(round_num_);

  logf(LOG_DBG2, "Broadcast pivot count: %d, expected %d", num_pivots,
       num_ranks_ + 1);
  assert(num_pivots == num_ranks_ + 1);

  PivotUtils::UpdatePivots(carp_, pivots, num_pivots);

  data_buffer_.AdvanceRound();
  round_num_++;

  if (state_.GetNextRoundStart()) {
    /* Next round has started, keep main thread sleeping and participate */
    replay_rtp_begin_flag = true;
    state_.UpdateState(RenegState::READYBLOCK);
    carp_->UpdateState(MainThreadState::MT_REMAIN_BLOCKED);
    logf(LOG_DBUG, "[CARP] Rank %d: continuing to round %d", my_rank_,
         round_num_);
  } else {
    /* Next round has not started yet, we're READY */
    state_.UpdateState(RenegState::READY);
    carp_->UpdateState(MainThreadState::MT_READY);
    logf(LOG_DBUG, "[CARP] Rank %d: RTP finished, READY", my_rank_);
  }

  mutex_.Unlock();

  if (!replay_rtp_begin_flag) {
    /* Since no round to replay, wake up Main Thread */
    carp_->cv_.Signal();
  }

  carp_->mutex_.Unlock();

  if (replay_rtp_begin_flag) {
    /* Not supposed to access without lock, but logging statement so ok */
    logf(LOG_DBG2, "Rank %d: continuing to round %d", my_rank_, round_num_);
    /* Send rtp_begin to self, to resume pivot send etc */
    ReplayBegin();
  }

  return s;
}

Status RTP::ReplayBegin() {
  // XXX: commented out as it makes sense, but not tested
  // mutex_.AssertHeld();
  Status s = Status::OK();

  logf(LOG_DBUG, "replay_rtp_begin: at rank %d", my_rank_);

  char buf[256];
  // TODO: round_num is correct?
  int bufsz = msgfmt_encode_rtp_begin(buf, 256, my_rank_, round_num_);
  SendToRank(buf, bufsz, my_rank_, MSGFMT_RTP_BEGIN);

  return s;
}

void RTP::ComputeAggregatePivots(int stage_num, int num_merged,
                                 double* merged_pivots, double& merged_width) {
  std::vector<rb_item_t> rbvec;

  std::vector<double> unified_bins;
  std::vector<float> unified_bin_counts;

  std::vector<double> samples;
  std::vector<int> sample_counts;

  std::vector<double> pivot_widths;

  double sample_width;

  data_buffer_.LoadIntoRbvec(stage_num, rbvec);
  data_buffer_.GetPivotWidths(stage_num, pivot_widths);

  pivot_union(rbvec, unified_bins, unified_bin_counts, pivot_widths,
              fanout_[stage_num]);

  std::vector<double> merged_pivot_vec;

  resample_bins_irregular(unified_bins, unified_bin_counts, merged_pivot_vec,
                          sample_width, num_merged);

  logf(LOG_DBG2, "resampled pivot count: s%d cnt: %zu", stage_num,
       merged_pivot_vec.size());

  merged_width = sample_width;
  std::copy(merged_pivot_vec.begin(), merged_pivot_vec.end(), merged_pivots);
}
}  // namespace carp
}  // namespace pdlfs

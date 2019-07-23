#pragma once

#include <condition_variable>
#include <mutex>
#include "common.h"

#define MSGFMT_MAX_BUFSIZE 255

#define ABORT_FILENAME \
  (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define ABORT(msg) msg_abort(errno, msg, __func__, ABORT_FILENAME, __LINE__)

#include <vector>

// TODO: make this configurable
#define RANGE_BUFSZ 1000
// TODO: Can shorten this by using indirect ptr?
#define RANGE_MAX_PSZ 255
#define RANGE_MAX_OOB_THRESHOLD 8
/* Total  for left + right buffers */
#define RANGE_TOTAL_OOB_THRESHOLD 2 * RANGE_MAX_OOB_THRESHOLD
#define RANGE_NUM_PIVOTS 4

#define RANGE_IS_READY(x) (x->range_state == range_state_t::RS_READY)
#define RANGE_IS_INIT(x) (x->range_state == range_state_t::RS_INIT)

#define RANGE_LEFT_OOB_FULL(x) (x->oob_count_left == RANGE_MAX_OOB_THRESHOLD)
#define RANGE_RIGHT_OOB_FULL(x) (x->oob_count_right == RANGE_MAX_OOB_THRESHOLD)

#define RANGE_OOB_FULL(x) \
  (x->oob_count_left + x->oob_count_right == RANGE_TOTAL_OOB_THRESHOLD)

/* abort with an error message: forward decl */
void msg_abort(int err, const char* msg, const char* func, const char* file,
               int line);

typedef struct particle_mem {
  float indexed_prop;       // property for range query
  char ptr[RANGE_MAX_PSZ];  // other data
} particle_mem_t;

enum class range_state_t { RS_INIT, RS_READY, RS_RENEGO };

enum class buf_type_t { RB_NO_BUF, RB_BUF_LEFT, RB_BUF_RIGHT };

typedef struct range_ctx {
  /* range data structures */
  float negotiated_range_start;
  float negotiated_range_end;

  int ts_writes_received;
  int ts_writes_shuffled;

  range_state_t range_state;
  range_state_t range_state_prev;

  std::vector<float> rank_bins;
  std::vector<float> rank_bin_count;
  int range_min, range_max;

  std::vector<float> rank_bins_ss;
  std::vector<float> rank_bin_count_ss;
  int range_min_ss, range_max_ss;

  std::vector<particle_mem_t> oob_buffer_left;
  /* oob_buffers are preallocated to MAX to avoid resize calls
   * thus we use counters to track actual size */
  int oob_count_left;

  std::vector<particle_mem_t> oob_buffer_right;
  int oob_count_right;

  /* "infinitely" extensible queue for when you don't know what
   * to do with a particle; to be used sparingly for corner cases
   * at some point the rank will come to its senses and flush this
   * queue (read: finish negotiation or flush fixed queues)
   */
  // std::vector<particle_mem_t> contingency_queue;

  std::vector<float> my_pivots;

  // XXX: Use atomic?
  volatile bool snapshot_in_progress;
  std::mutex block_writes_m;
  std::condition_variable block_writes_cv;
} range_ctx_t;

typedef struct preload_ctx preload_ctx_t;

void range_init_negotiation(preload_ctx_t *pctx);

/* get_local_pivots: Take the bins stored in bin_snapshots and OOB buffers
 * and store their pivots in rctx->my_pivots. Return nothing
 * XXX TODO: Also need to snapshot OOB buffers (if only indexable_prob) in
 * addition to bins.
 * @param rctx range_ctx
 * @return None
 * */
void get_local_pivots(range_ctx_t* rctx);
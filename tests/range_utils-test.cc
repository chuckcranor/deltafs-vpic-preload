#include "carp/range_utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <iostream>

#include "carp/carp.h"
#include "pdlfs-common/testharness.h"
#include "pdlfs-common/testutil.h"
#include "carp/range_common.h"

namespace pdlfs {
namespace carp {
class RangeUtilsTest {
 public:
  carp::Carp* carp;
  carp::CarpOptions ro;
  shuffle_ctx_t sh_ctx;

  RangeUtilsTest() {
    ro.rtp_pvtcnt[1] = DEFAULT_PVTCNT;
    ro.rtp_pvtcnt[2] = DEFAULT_PVTCNT;
    ro.rtp_pvtcnt[3] = DEFAULT_PVTCNT;
    ro.oob_sz = DEFAULT_OOBSZ;
    ro.sctx = &sh_ctx;
    ro.sctx->type = SHUFFLE_XN;
    ro.my_rank = 0;
    ro.num_ranks = 512;
    ro.reneg_policy = "InvocationPeriodic";

    carp = new carp::Carp(ro);

    carp->mutex_.Lock();
    carp->UpdateState(MainThreadState::MT_BLOCK);
    carp->mutex_.Unlock();
  }

  void AdvancePastInit() {
    carp->mutex_.Lock();
    carp->UpdateState(MainThreadState::MT_READY);
    carp->UpdateState(MainThreadState::MT_BLOCK);
    carp->mutex_.Unlock();
  }

  void LoadData(const float* oob_data, const int oob_data_sz) {
    carp->oob_buffer_.Reset();

    for (int i = 0; i < oob_data_sz; i++) {
      carp::particle_mem_t p;
      p.indexed_prop = oob_data[i];
      carp->oob_buffer_.Insert(p);
    }
  }

  void LoadData(const int num_ranks, const float range_min,
                const float range_max, const uint64_t* rank_bin_counts,
                const float* rank_bins) {
    carp->UpdateRange({range_min, range_max});
    carp->oob_buffer_.SetRange(range_min, range_max);
    carp->bins_.UpdateFromArrays(num_ranks, rank_bins, rank_bin_counts);
  }

  float WeightedAverage(float a, float b, float frac) {
    return carp::PivotUtils::WeightedAverage(a, b, frac);
  }

  void AssertStrictMonotonicity(Pivots& pivots) {
    size_t sz = pivots.Size();

    for (size_t idx = 1; idx < sz; idx++) {
      ASSERT_GT(pivots[idx], pivots[idx - 1]);
    }
  }

  ~RangeUtilsTest() { delete carp; }
};

TEST(RangeUtilsTest, ParticleCount) {
  ASSERT_EQ(get_particle_count(3, 5, 2), 4);
  ASSERT_EQ(get_particle_count(2, 5, 2), 6);
  ASSERT_EQ(get_particle_count(3, 3, 2), 0);
}

TEST(RangeUtilsTest, WeightedAvg) {
  float a, b, frac_1, frac_2;

  a = 0;
  b = 1;
  frac_1 = 0.3;
  frac_2 = 0.6;

  float wavg_1 = WeightedAverage(a, b, frac_1);
  float wavg_2 = WeightedAverage(a, b, frac_2);
  ASSERT_LE(wavg_1, wavg_2);

  a = 0.473880023;
  b = 0.473880142;
  frac_1 = 0.412109375;
  frac_2 = 0.536865234;

  wavg_1 = WeightedAverage(a, b, frac_1);
  wavg_2 = WeightedAverage(a, b, frac_2);
  ASSERT_LE(wavg_1, wavg_2);
}

TEST(RangeUtilsTest, DeduplicateVector) {
  std::vector<float> v = { 5, 5, 5, 5, 5, 6, 6, 7, 8};
  size_t oldsz = v.size();
  pdlfs::carp::deduplicate_sorted_vector(v);
  size_t newsz = v.size();
  logf(LOG_INFO, "Dedup, old size: %zu, new size: %zu", oldsz, newsz);
  assert(v.size() == 4);
}

TEST(RangeUtilsTest, OrderedBinAddition) {
  float bins[] = {1, 2, 3, 4, 5};
  uint64_t counts1[] = {2, 3, 2, 3};
  uint64_t counts2[] = {1, 2, 3, 4};

  OrderedBins bins1(4), bins2(4);
  bins1.UpdateFromArrays(4, bins, counts1);
  bins2.UpdateFromArrays(4, bins, counts2);

  OrderedBins bins_total = bins1 + bins2;

  ASSERT_EQ(bins1.GetTotalMass(), 10);
  ASSERT_EQ(bins2.GetTotalMass(), 10);
  ASSERT_EQ(bins_total.GetTotalMass(), 20);
}

TEST(RangeUtilsTest, PivotCalc) {
  srand(time(NULL));

  int oob_count = DEFAULT_OOBSZ;

  for (int oob_idx = 0; oob_idx < oob_count; oob_idx++) {
    float rand_val = rand() * 1.0f / RAND_MAX;
    carp::particle_mem_t p;
    p.indexed_prop = rand_val;
    carp->oob_buffer_.Insert(p);
  }

  int num_pivots = 8;
  carp->mutex_.Lock();
  carp->CalculatePivots(num_pivots);
  carp->mutex_.Unlock();

  assert(carp->pivots_.Size() == num_pivots);
  AssertStrictMonotonicity(carp->pivots_);
}

TEST(RangeUtilsTest, PivotCalc2) {
  int oob_count = 12;

  const float data[] = {
      0.183005, 0.261744, 0.379052, 0.130448, 0.400778, 0.327600,
      0.831964, 0.363970, 1.327184, 0.193020, 2.427586, 0.213298,
  };

  const float pivots_ref[] = {0.130447999, 0.186760634, 0.208228499,
                              0.26997599,  0.345785022, 0.377166778,
                              0.508574486, 2.42758608};

  for (int oob_idx = 0; oob_idx < oob_count; oob_idx++) {
    carp::particle_mem_t p;
    p.indexed_prop = data[oob_idx];
    carp->oob_buffer_.Insert(p);
  }

  int num_pivots = 8;
  carp->mutex_.Lock();
  carp->CalculatePivots(num_pivots);
  carp->mutex_.Unlock();

  assert(carp->pivots_.Size() == num_pivots);

  for (int pvt_idx = 0; pvt_idx < num_pivots; pvt_idx++) {
    float pvt = carp->pivots_[pvt_idx];
    float ref = pivots_ref[pvt_idx];

    ASSERT_TRUE(float_eq(pvt, ref));
  }
}

TEST(RangeUtilsTest, PivotCalc3) {
  AdvancePastInit();

  const float oob_data[] = {
      0.530524611, 2.07151246,  0.129153624, 0.317573667, 0.179045826,
      1.58116162,  3.8822875,   0.311510593, 0.475103348, 0.10761077,
      0.264501095, 0.594769895, 1.46151435,  0.779551029, 2.9388082,
      1.78281081,  4.48976421,  0.371605545, 0.250535101, 1.02625966,
      0.250535101, 0.433013767, 3.02271819,  0.179088727, 0.348295808};

  const float rank_bins[] = {0.011929879, 0.203471959, 0.338690162,
                             0.500766993, 0.735446155, 1.01613414,
                             1.4439038,   2.18780971,  4.48976707};

  const uint64_t rank_bin_counts[] = {11, 10, 10, 6, 1, 3, 2, 1};
  const float range_min = 0.011929879, range_max = 4.48976707;
  const int oob_data_sz = 25;
  const int num_ranks = 8;
  const int num_pivots = 8;

  LoadData(oob_data, oob_data_sz);
  LoadData(num_ranks, range_min, range_max, rank_bin_counts, rank_bins);
  carp->mutex_.Lock();
  carp->CalculatePivots(num_pivots);
  carp->mutex_.Unlock();

  assert(carp->pivots_.Size() == num_pivots);
  AssertStrictMonotonicity(carp->pivots_);
}

TEST(RangeUtilsTest, PivotCalc4) {
#include "pivot_calc_4_data.cc"  // NOLINT(bugprone-suspicious-include)
  AdvancePastInit();
  LoadData(oob_data, oob_data_sz);
  LoadData(num_ranks, range_min, range_max, rank_bin_counts, rank_bins);
  carp->mutex_.Lock();
  carp->CalculatePivots(num_pivots);
  carp->mutex_.Unlock();

  assert(carp->pivots_.Size() == num_pivots);
  AssertStrictMonotonicity(carp->pivots_);
}

TEST(RangeUtilsTest, PivotCalc5) {
#include "pivot_calc_5_data.cc"  // NOLINT(bugprone-suspicious-include)
  AdvancePastInit();
  LoadData(oob_data, oob_data_sz);
  LoadData(num_ranks, range_min, range_max, rank_bin_counts, rank_bins);
  carp->mutex_.Lock();
  carp->CalculatePivots(num_pivots);
  carp->mutex_.Unlock();

  assert(carp->pivots_.Size() == num_pivots);
  AssertStrictMonotonicity(carp->pivots_);
}

TEST(RangeUtilsTest, PivotCalc6) {
#include "pivot_calc_6_data.cc"  // NOLINT(bugprone-suspicious-include)
  AdvancePastInit();
  LoadData(oob_data, oob_data_sz);
  LoadData(num_ranks, range_min, range_max, rank_bin_counts, rank_bins);
  carp->mutex_.Lock();
  carp->CalculatePivots(num_pivots);
  carp->mutex_.Unlock();

  assert(carp->pivots_.Size() == num_pivots);
  AssertStrictMonotonicity(carp->pivots_);
}

TEST(RangeUtilsTest, PivotCalc7) {
#include "pivot_calc_7_data.cc"  // NOLINT(bugprone-suspicious-include)
  LoadData(oob_data, oob_data_sz);
  carp->mutex_.Lock();
  carp->CalculatePivots(num_pivots);
  carp->mutex_.Unlock();

  assert(carp->pivots_.Size() == num_pivots);
  AssertStrictMonotonicity(carp->pivots_);
}

TEST(RangeUtilsTest, PivotCalc8) {
#include "pivot_calc_8_data.cc"  // NOLINT(bugprone-suspicious-include)
  AdvancePastInit();
  LoadData(oob_data, oob_data_sz);
  LoadData(num_ranks, range_min, range_max, rank_bin_counts, rank_bins);
  carp->mutex_.Lock();
  carp->CalculatePivots(num_pivots);
  carp->mutex_.Unlock();

  assert(carp->pivots_.Size() == num_pivots);
  AssertStrictMonotonicity(carp->pivots_);
}

#if 0 /* XXX */
// fails with: "range_utils-test.cc:24: failed: 0.04 > 0.04"
// issue is zero-width pivots...  can CalculatePivots had gracefully?
// currently assert_monotonic isn't going to allow it and fail the test.
// comment this out for now, come back and look at it later
TEST(RangeUtilsTest, PivotCalc9) {
#include "pivot_calc_9_data.cc"  // NOLINT(bugprone-suspicious-include)
  AdvancePastInit();
  LoadData(oob_data, oob_data_sz);
  LoadData(num_ranks, range_min, range_max, rank_bin_counts, rank_bins);
  carp->mutex_.Lock();
  carp::PivotUtils::CalculatePivots(carp, num_pivots);
  carp->mutex_.Unlock();
  ::assert_monotonic(carp->my_pivots_, num_pivots);
}
#endif

/* This test may require bumping up the value of
 * pdlfs::kMaxPivots to >= 2048.
 * TODO: think of a better way to handle this
 */
TEST(RangeUtilsTest, PivotCalc10) {
#include "pivot_calc_10_data.cc"  // NOLINT(bugprone-suspicious-include)
  LoadData(oob_data, oob_data_sz);
  int npivots = 2048;
  carp->mutex_.Lock();
  carp->CalculatePivots(num_pivots);
  carp->mutex_.Unlock();

  assert(carp->pivots_.Size() == num_pivots);
  AssertStrictMonotonicity(carp->pivots_);
}

TEST(RangeUtilsTest, PivotCalc11) {
#include "pivot_calc_11_data.cc"  // NOLINT(bugprone-suspicious-include)
  LoadData(oob_data, oob_data_sz);
  carp->mutex_.Lock();
  carp->CalculatePivots(num_pivots);
  carp->mutex_.Unlock();

  assert(carp->pivots_.Size() == num_pivots);
  AssertStrictMonotonicity(carp->pivots_);
}
}  // namespace carp
}  // namespace pdlfs

int main(int argc, char* argv[]) {
  return ::pdlfs::test::RunAllTests(&argc, &argv);
}

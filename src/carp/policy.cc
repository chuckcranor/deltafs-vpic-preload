//
// Created by Ankush J on 3/5/21.
//

#include "policy.h"

#include "carp.h"

namespace pdlfs {
namespace carp {
InvocationPolicy::InvocationPolicy(Carp& carp, const CarpOptions& options)
    : options_(options),
      carp_(carp),
      invoke_intvl_(options_.reneg_intvl),
      epoch_(0),
      num_writes_(0) {}

bool InvocationPolicy::BufferInOob(particle_mem_t& p) {
  // XXX: should ideally be the same as consulting
  // carp.range_min_ and carp.range_max_
  // TODO: remove redundancy
  return carp_.oob_buffer_.OutOfBounds(p.indexed_prop);
}

bool InvocationPolicy::FirstRenegCompleted() {
  return !carp_.mts_mgr_.FirstBlock();
}

void InvocationPolicy::Reset() { carp_.Reset(); }

int InvocationPolicy::ComputeShuffleTarget(particle_mem_t& p, int& rank) {
  auto rank_iter = std::lower_bound(carp_.rank_bins_.begin(),
                                    carp_.rank_bins_.end(), p.indexed_prop);

  rank = rank_iter - carp_.rank_bins_.begin() - 1;
  return 0;
}

bool InvocationPolicy::IsOobFull() { return carp_.oob_buffer_.IsFull(); }

InvocationIntraEpoch::InvocationIntraEpoch(Carp& carp,
                                           const CarpOptions& options)
    : InvocationPolicy(carp, options) {}

bool InvocationIntraEpoch::TriggerReneg() {
  num_writes_++;
  bool intvl_trigger =
      (options_.my_rank == 0) && (num_writes_ % invoke_intvl_ == 0);
  return intvl_trigger || IsOobFull();
}

bool InvocationInterEpoch::TriggerReneg() {
  if ((options_.my_rank == 0) && !reneg_triggered_ &&
      InvocationPolicy::IsOobFull()) {
    reneg_triggered_ = true;
    return true;
  } else if (options_.my_rank != 0 && !reneg_triggered_) {
    reneg_triggered_ = FirstRenegCompleted();
  }

  return false;
}

int InvocationInterEpoch::ComputeShuffleTarget(particle_mem_t& p) {
  int rank;
  InvocationPolicy::ComputeShuffleTarget(p, rank);
  /* dump all unseen particles into the last rank */
  if (rank == -1 and reneg_triggered_) {
    rank = 0;
  } else if (rank == options_.num_ranks and reneg_triggered_) {
    rank = options_.num_ranks - 1;
  }
  return rank;
}
}  // namespace carp
}  // namespace pdlfs

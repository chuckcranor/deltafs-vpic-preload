//
// Created by Ankush J on 3/5/21.
//

#pragma once

#include <stdint.h>
#include <stdio.h>

#include "oob_buffer.h"

namespace pdlfs {
namespace carp {
class Carp;
class CarpOptions;
class InvocationPolicy {
 public:
  InvocationPolicy(Carp& carp, const CarpOptions& options);
  virtual bool TriggerReneg() = 0;
  virtual void AdvanceEpoch() = 0;
  virtual int ComputeShuffleTarget(particle_mem_t& p) = 0;

 protected:
  int ComputeShuffleTarget(particle_mem_t& p, int& rank, int& num_ranks);

  bool IsOobFull();

  uint32_t epoch_;
  uint64_t num_writes_;
  const Carp& carp_;
  const CarpOptions& options_;
};

class InvocationPeriodic : public InvocationPolicy {
 public:
  InvocationPeriodic(Carp& carp, const CarpOptions& options);

  bool TriggerReneg() override;

  void AdvanceEpoch() override {
    epoch_++;
    num_writes_ = 0;
  }

  int ComputeShuffleTarget(particle_mem_t& p) override {
    int rank;
    int num_ranks;
    InvocationPolicy::ComputeShuffleTarget(p, rank, num_ranks);
    return rank;
  }

 private:
  const uint64_t invoke_intvl_;
};

class InvocationPerEpoch : public InvocationPolicy {
 public:
  InvocationPerEpoch(Carp& carp, const CarpOptions& options)
      : InvocationPolicy(carp, options), reneg_triggered_(false) {}

  bool TriggerReneg() override {
    if (!reneg_triggered_ && InvocationPolicy::IsOobFull()) {
      reneg_triggered_ = true;
      return true;
    } else {
      return false;
    }
  }

  void AdvanceEpoch() override {
    epoch_++;
    reneg_triggered_ = false;
  }

  int ComputeShuffleTarget(particle_mem_t& p) override {
    int rank;
    int num_ranks;
    InvocationPolicy::ComputeShuffleTarget(p, rank, num_ranks);
    /* dump all unseen particles into the last rank */
    if (rank == num_ranks) {
      rank = num_ranks - 1;
    }
    return rank;
  }

 private:
  bool reneg_triggered_;
};

class InvocationOnce : public InvocationPerEpoch {
 public:
  InvocationOnce(Carp& carp, const CarpOptions& options)
      : InvocationPerEpoch(carp, options) {}
  void AdvanceEpoch() override { epoch_++; }
};
}  // namespace carp
}  // namespace pdlfs

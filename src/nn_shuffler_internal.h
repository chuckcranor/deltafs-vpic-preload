/*
 * Copyright (c) 2019 Carnegie Mellon University,
 * Copyright (c) 2019 Triad National Security, LLC, as operator of
 *     Los Alamos National Laboratory.
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * with the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of CMU, TRIAD, Los Alamos National Laboratory, LANL, the
 *    U.S. Government, nor the names of its contributors may be used to endorse
 *    or promote products derived from this software without specific prior
 *    written permission.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/utsname.h>
#include <unistd.h>

#include <mercury.h>
#include <mpi.h>
#include <mssg.h>

#include <deltafs/deltafs_api.h>

#include "preload_internal.h"
#include "preload_shuffle.h"

#include "hstg.h"
/*
 * The max allowed size for a single rpc message.
 */
#define MAX_RPC_MESSAGE (524288)

#define RPC_FAILED_FILENAME \
  (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define RPC_FAILED(msg, ret) \
  rpc_failed(hret, msg, __func__, RPC_FAILED_FILENAME, __LINE__)

/*
 * rpc_failed: abort with a mercury rpc error
 */
void rpc_failed(hg_return_t hret, const char* msg, const char* func,
                const char* file, int line);

/*
 * nn_rusage: cpu usage report.
 */
typedef struct nn_rusage {
  unsigned long long sys_micros; /* sys-level cpu time */
  unsigned long long usr_micros; /* usr-level cpu time */

  char tag[16];
} nn_rusage_t;

/*
 * nn_ctx: state for an nn shuffler.
 */
typedef struct nn_ctx {
  char my_addr[100]; /* mercury server uri */
  struct utsname my_uname;
  shuffle_ctx_t* shctx;

  /* total num of hg errors before we abort */
  int hg_errors;
  int hg_nice; /* nice value to be applied to the looper thread */
  int hg_rusage;
  hg_class_t* hg_clz;
  hg_context_t* hg_ctx;
  hg_id_t hg_id;

  /* hg_progress intervals */
  hstg_t hg_intvl;

  /* hg_timeouts (in ms) */
  unsigned int hg_max_interval;
  int hg_timeout;
  int timeout; /* rpc timeout (in secs) */

  int random_flush; /* flush rpc queues in out-of-order */
  int force_sync;   /* avoid async rpc */
  int cache_hlds;   /* cache mercury rpc handles */
  int hash_sig;     /* generate a hash signature for each rpc */

  int paranoid_checks;

  /* MSSG context */
  mssg_t* mssg;

  /* rpc usage */
  nn_rusage_t r[5];

  /* rpc stats */
  unsigned long long total_writes; /* total number of writes shuffled */
  unsigned long long total_msgsz;  /* total rpc msg size */

  /* rpc incoming queue depth */
  hstg_t iq_dep;

} nn_ctx_t;

extern nn_ctx_t nnctx;

typedef struct write_in {
  hg_uint32_t hash_sig; /* hash signature of the entire payload */
  hg_uint32_t sz;       /* msg size */

  hg_int32_t dst;
  hg_int32_t src;
  hg_int32_t epo;
  void* msg;
} write_in_t;

typedef struct write_out {
  hg_int32_t rv; /* ret value of the write operation */
} write_out_t;

typedef struct write_cb {
  int ok; /* non-zero if rpc has completed */
  hg_return_t hret;
} write_cb_t;

typedef struct write_async_cb {
  void* arg1;
  void* arg2;
  int slot; /* cb slot used */
} write_async_cb_t;

typedef struct write_info {
  /* number of writes sent within an rpc msg */
  hg_uint32_t num_writes;
  hg_uint32_t sz; /* msg size */
} write_info_t;

void nn_vector_random_shuffle(int rank, std::vector<int>* vec);
hg_return_t nn_shuffler_write_rpc_handler(hg_handle_t h, write_info_t*);
hg_return_t nn_shuffler_write_rpc_handler_wrapper(hg_handle_t handle);
hg_return_t nn_shuffler_write_async_handler(const struct hg_cb_info* info);
hg_return_t nn_shuffler_write_handler(const struct hg_cb_info* info);

/* get current hg class and context instances */
inline void* nn_shuffler_hg_class(void) { return nnctx.hg_clz; }
inline void* nn_shuffler_hg_ctx(void) { return nnctx.hg_ctx; }

/* encoding decoding procedure */
hg_return_t nn_shuffler_write_out_proc(hg_proc_t proc, void* data);
hg_return_t nn_shuffler_write_in_proc(hg_proc_t proc, void* data);

hg_uint32_t nn_shuffler_maybe_hashsig(const write_in_t* in);

/*
 * nn_shuffler_write_send_async: asynchronously send one or more encoded writes
 * to a remote peer and return immediately without waiting for response.
 *
 * return 0 on success, or EOF on errors.
 */
int nn_shuffler_write_send_async(write_in_t* write_in, int peer_rank,
                                 void* arg1, void* arg2);
/*
 * nn_shuffler_write_send: send one or more encoded writes to a remote peer
 * and wait for its response.
 *
 * return 0 on success, or EOF on errors.
 */
int nn_shuffler_write_send(write_in_t* write_in, int peer_rank);

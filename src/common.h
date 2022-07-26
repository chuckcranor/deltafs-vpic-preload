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
#include <pthread.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include <string>

/*
 * utilities for probing important system configurations.
 */
void check_clockres();
void check_sse42();
void maybe_warn_rlimit(int myrank, int worldsz);
void maybe_warn_numa();

void try_scan_procfs();
void try_scan_sysfs();

/* print VmSize and VmRSS */
void print_meminfo();

/* get the number of cpu cores that we may use */
int my_cpu_cores();

/* get the current time in us. */
uint64_t now_micros();

/* get the current time in us with fast but coarse-grained timestamps. */
uint64_t now_micros_coarse();

/* convert posix timeval to micros */
uint64_t timeval_to_micros(const struct timeval* tv);

/* return the memory size (KB) of the calling process */
long my_maxrss();

#define LOG_ERRO 5
#define LOG_WARN 4
#define LOG_INFO 3
#define LOG_DBUG 2
#define LOG_DBG2 1

/* levels >= LOG_LEVEL are printed */
#define LOG_LEVEL 3

int logf(int lvl, const char* fmt, ...);
int loge(const char* op, const char* path);

/*
 * logging facilities and helpers
 */
#define ABORT_FILENAME \
  (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define ABORT(msg) msg_abort(errno, msg, __func__, ABORT_FILENAME, __LINE__)

/* abort with an error message */
void msg_abort(int err, const char* msg, const char* func, const char* file,
               int line);

inline const char* maybe_getenv(const char* key) {
  const char* env;
  env = getenv(key);
  return env;
}

inline int is_envset(const char* key) {
  const char* env;
  env = maybe_getenv(key);
  if (env == NULL || env[0] == 0) return false;
  return strcmp(env, "0") != 0;
}

inline int getr(int min, int max) {
  return static_cast<int>(
      (static_cast<double>(rand()) / (static_cast<double>(RAND_MAX) + 1)) *
          (max - min + 1) +
      min);
}

inline int pthread_cv_notifyall(pthread_cond_t* cv) {
  int r = pthread_cond_broadcast(cv);
  if (r != 0) {
    ABORT("pthread_cond_broadcast");
  }

  return 0;
}

inline int pthread_cv_timedwait(pthread_cond_t* cv, pthread_mutex_t* mtx,
                                const timespec* due) {
  int r = pthread_cond_timedwait(cv, mtx, due);
  if (r == ETIMEDOUT) {
    return r;
  }
  if (r != 0) {
    ABORT("pthread_cond_timedwait");
  }

  return 0;
}

inline int pthread_cv_wait(pthread_cond_t* cv, pthread_mutex_t* mtx) {
  int r = pthread_cond_wait(cv, mtx);
  if (r != 0) {
    ABORT("pthread_cond_wait");
  }

  return 0;
}

inline int pthread_mtx_lock(pthread_mutex_t* mtx) {
  int r = pthread_mutex_lock(mtx);
  if (r != 0) {
    ABORT("pthread_mutex_lock");
  }

  return 0;
}

inline int pthread_mtx_unlock(pthread_mutex_t* mtx) {
  int r = pthread_mutex_unlock(mtx);
  if (r != 0) {
    ABORT("pthread_mutex_unlock");
  }

  return 0;
}

#define PRELOAD_PRETTY_SIZE_USE_BINARY

/* print a human-readable time duration. */
inline std::string pretty_dura(double us) {
  char tmp[100];
#ifndef NDEBUG
  snprintf(tmp, sizeof(tmp), "%.0f us", us);
#else
  if (us >= 1000000) {
    snprintf(tmp, sizeof(tmp), "%.3f s", us / 1000000.0);
  } else {
    snprintf(tmp, sizeof(tmp), "%.3f ms", us / 1000.0);
  }
#endif
  return tmp;
}

/* print a human-readable integer number. */
inline std::string pretty_num(double num) {
  char tmp[100];
#ifndef NDEBUG
  snprintf(tmp, sizeof(tmp), "%.0f", num);
#else
#if defined(PRELOAD_PRETTY_USE_BINARY)
  if (num >= 1099511627776.0) {
    num /= 1099511627776.0;
    snprintf(tmp, sizeof(tmp), "%.1f Ti", num);
  } else if (num >= 1073741824.0) {
    num /= 1073741824.0;
    snprintf(tmp, sizeof(tmp), "%.1f Gi", num);
  } else if (num >= 1048576.0) {
    num /= 1048576.0;
    snprintf(tmp, sizeof(tmp), "%.1f Mi", num);
  } else if (num >= 1024.0) {
    num /= 1024.0;
    snprintf(tmp, sizeof(tmp), "%.1f Ki", num);
  } else {
    snprintf(tmp, sizeof(tmp), "%.0f", num);
  }
#else
  if (num >= 1000000000000.0) {
    num /= 1000000000000.0;
    snprintf(tmp, sizeof(tmp), "%.1f T", num);
  } else if (num >= 1000000000.0) {
    num /= 1000000000.0;
    snprintf(tmp, sizeof(tmp), "%.1f G", num);
  } else if (num >= 1000000.0) {
    num /= 1000000.0;
    snprintf(tmp, sizeof(tmp), "%.1f M", num);
  } else if (num >= 1000.0) {
    num /= 1000.0;
    snprintf(tmp, sizeof(tmp), "%.1f K", num);
  } else {
    snprintf(tmp, sizeof(tmp), "%.0f", num);
  }
#endif
#endif
  return tmp;
}

/* print a human-readable I/O throughput number. */
inline std::string pretty_tput(double ops, double us) {
  char tmp[100];
  double ops_per_s = ops / us * 1000000;
#ifndef NDEBUG
  snprintf(tmp, sizeof(tmp), "%.0f op/s", ops_per_s);
#else
#if defined(PRELOAD_PRETTY_USE_BINARY)
  if (ops_per_s >= 1099511627776.0) {
    ops_per_s /= 1099511627776.0;
    snprintf(tmp, sizeof(tmp), "%.3f Tiop/s", ops_per_s);
  } else if (ops_per_s >= 1073741824.0) {
    ops_per_s /= 1073741824.0;
    snprintf(tmp, sizeof(tmp), "%.3f Giop/s", ops_per_s);
  } else if (ops_per_s >= 1048576.0) {
    ops_per_s /= 1048576.0;
    snprintf(tmp, sizeof(tmp), "%.3f Miop/s", ops_per_s);
  } else if (ops_per_s >= 1024.0) {
    ops_per_s /= 1024.0;
    snprintf(tmp, sizeof(tmp), "%.3f Kiop/s", ops_per_s);
  } else {
    snprintf(tmp, sizeof(tmp), "%.0f op/s", ops_per_s);
  }
#else
  if (ops_per_s >= 1000000000000.0) {
    ops_per_s /= 1000000000000.0;
    snprintf(tmp, sizeof(tmp), "%.3f Top/s", ops_per_s);
  } else if (ops_per_s >= 1000000000.0) {
    ops_per_s /= 1000000000.0;
    snprintf(tmp, sizeof(tmp), "%.3f Gop/s", ops_per_s);
  } else if (ops_per_s >= 1000000.0) {
    ops_per_s /= 1000000.0;
    snprintf(tmp, sizeof(tmp), "%.3f Mop/s", ops_per_s);
  } else if (ops_per_s >= 1000.0) {
    ops_per_s /= 1000.0;
    snprintf(tmp, sizeof(tmp), "%.3f Kop/s", ops_per_s);
  } else {
    snprintf(tmp, sizeof(tmp), "%.0f op/s", ops_per_s);
  }
#endif
#endif
  return tmp;
}

/* print a human-readable I/O size. */
inline std::string pretty_size(double size) {
  char tmp[100];
#ifndef NDEBUG
  snprintf(tmp, sizeof(tmp), "%.0f bytes", size);
#else
#if defined(PRELOAD_PRETTY_SIZE_USE_BINARY)
  if (size >= 1099511627776.0) {
    size /= 1099511627776.0;
    snprintf(tmp, sizeof(tmp), "%.1f TiB", size);
  } else if (size >= 1073741824.0) {
    size /= 1073741824.0;
    snprintf(tmp, sizeof(tmp), "%.1f GiB", size);
  } else if (size >= 1048576.0) {
    size /= 1048576.0;
    snprintf(tmp, sizeof(tmp), "%.1f MiB", size);
  } else if (size >= 1024.0) {
    size /= 1024.0;
    snprintf(tmp, sizeof(tmp), "%.1f KiB", size);
  } else {
    snprintf(tmp, sizeof(tmp), "%.0f bytes", size);
  }
#else
  if (size >= 1000000000000.0) {
    size /= 1000000000000.0;
    snprintf(tmp, sizeof(tmp), "%.1f TB", size);
  } else if (size >= 1000000000.0) {
    size /= 1000000000.0;
    snprintf(tmp, sizeof(tmp), "%.1f GB", size);
  } else if (size >= 1000000.0) {
    size /= 1000000.0;
    snprintf(tmp, sizeof(tmp), "%.1f MB", size);
  } else if (size >= 1000.0) {
    size /= 1000.0;
    snprintf(tmp, sizeof(tmp), "%.1f KB", size);
  } else {
    snprintf(tmp, sizeof(tmp), "%.0f bytes", size);
  }
#endif
#endif
  return tmp;
}

/* print a human-readable data bandwidth number. */
inline std::string pretty_bw(double bytes, double us) {
  char tmp[100];
  double bytes_per_s = bytes / us * 1000000;
#ifndef NDEBUG
  snprintf(tmp, sizeof(tmp), "%.0f bytes/s", bytes_per_s);
#else
#if defined(PRELOAD_PRETTY_SIZE_USE_BINARY)
  if (bytes_per_s >= 1099511627776.0) {
    bytes_per_s /= 1099511627776.0;
    snprintf(tmp, sizeof(tmp), "%.3f TiB/s", bytes_per_s);
  } else if (bytes_per_s >= 1073741824.0) {
    bytes_per_s /= 1073741824.0;
    snprintf(tmp, sizeof(tmp), "%.3f GiB/s", bytes_per_s);
  } else if (bytes_per_s >= 1048576.0) {
    bytes_per_s /= 1048576.0;
    snprintf(tmp, sizeof(tmp), "%.3f MiB/s", bytes_per_s);
  } else if (bytes_per_s >= 1024.0) {
    bytes_per_s /= 1024.0;
    snprintf(tmp, sizeof(tmp), "%.3f KiB/s", bytes_per_s);
  } else {
    snprintf(tmp, sizeof(tmp), "%.3f bytes/s", bytes_per_s);
  }
#else
  if (bytes_per_s >= 1000000000000.0) {
    bytes_per_s /= 1000000000000.0;
    snprintf(tmp, sizeof(tmp), "%.3f TB/s", bytes_per_s);
  } else if (bytes_per_s >= 1000000000.0) {
    bytes_per_s /= 1000000000.0;
    snprintf(tmp, sizeof(tmp), "%.3f GB/s", bytes_per_s);
  } else if (bytes_per_s >= 1000000.0) {
    bytes_per_s /= 1000000.0;
    snprintf(tmp, sizeof(tmp), "%.3f MB/s", bytes_per_s);
  } else if (bytes_per_s >= 1000.0) {
    bytes_per_s /= 1000.0;
    snprintf(tmp, sizeof(tmp), "%.3f KB/s", bytes_per_s);
  } else {
    snprintf(tmp, sizeof(tmp), "%.3f bytes/s", bytes_per_s);
  }
#endif
#endif
  return tmp;
}

#undef PRELOAD_PRETTY_SIZE_USE_BINARY

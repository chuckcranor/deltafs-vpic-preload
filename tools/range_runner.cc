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

/*
 * preload_runner.cc a simple vpic io skeleton program for emulating
 * vpic workloads that use the file-per-particle io pattern.
 */

/*
 * To run this program, either compile and link this program with the
 * rest preload code, or compile and link it without the preload
 * code but use LD_PRELOAD at runtime to intercept io calls.
 */
#include <dirent.h>
#include <errno.h>
#include <getopt.h>
#include <limits.h>
#include <mpi.h>
#include <signal.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <fstream>
#include <iostream>

#include "workload_generator.h"

/*
 * helper/utility functions, included inline here so we are self-contained
 * in one single source file...
 */
static char* argv0; /* argv[0], program name */
static int myrank = 0;

/*
 * for trace replay
 */
static int trace_epochs[256];
const int kMaxEpochs = 256;
static int trace_epcnt;

/*
 * Range query arguments. Can be moved to take cmdline params
 */
// static float range_bins[] = {1, 2, 3, 3, 3, 4, 6, 8, 6, 4, 3, 3, 3, 2, 1};
// static int num_bins = 15;
static float range_bins[] = {1, 2};
static int num_bins = 2;

static float range_start = 0;
static float range_end = 50;
static rangeutils::WorkloadPattern range_wp =
    rangeutils::WorkloadPattern::WP_RANDOM;

/*
 * vcomplain/complain about something.  if ret is non-zero we exit(ret)
 * after complaining.  if r0only is set, we only print if myrank == 0.
 */
static void vcomplain(int ret, int r0only, const char* format, va_list ap) {
  if (!r0only || myrank == 0) {
    fprintf(stderr, "%s: ", argv0);
    vfprintf(stderr, format, ap);
    fprintf(stderr, "\n");
  }
  if (ret) {
    MPI_Finalize();
    exit(ret);
  }
}

static void complain(int ret, int r0only, const char* format, ...) {
  va_list ap;
  va_start(ap, format);
  vcomplain(ret, r0only, format, ap);
  va_end(ap);
}

/*
 * abort with a fatal message
 */
#define FATAL(msg) fatal(__FILE__, __LINE__, msg)
static void fatal(const char* f, int d, const char* msg) {
  fprintf(stderr, "=== ABORT === ");
  fprintf(stderr, "%s (%s:%d)", msg, f, d);
  fprintf(stderr, "\n");
  abort();
}

/*
 * default values
 */
#define DEF_STEPTIME 1.0    /* secs per vpic timestep */
#define DEF_NSTEPS 15       /* total steps */
#define DEF_NDUMPS 3        /* total epoch dumps */
#define DEF_PARTICLESIZE 40 /* bytes per particle */
#define DEF_NPARTICLES 16   /* total particles per rank */
#define DEF_TIMEOUT 240     /* alarm timeout */

/*
 * gs: shared global data (e.g. from the command line)
 */
static struct gs {
  char pdir[128]; /* particle dirname */
  /* note: MPI rank stored in global "myrank" */
  int size;           /* world size (from MPI) */
  double steptime;    /* computation time per vpic timestep (sec) */
  int p[3];           /* particles on x,y,z dimension */
  int t[3];           /* topology on x,y,z dimension  */
  const char* deckid; /* vpic deck id (vpic app name) */
  const char* deck;   /* vpic deck (run time) */
  int nsteps;         /* total vpic timesteps to execute */
  int ndumps;         /* total dumps to perform */
  int psize;          /* total state per vpic particle (bytes) */
  int nps;            /* number of particles per rank */
  int timeout;        /* alarm timeout */
  const char* trace;
} g;

/*
 * alarm signal handler
 */
static void sigalarm(int foo) {
  fprintf(stderr, "SIGALRM detected (%d)\n", myrank);
  fprintf(stderr, "Alarm clock\n");
  MPI_Finalize();
  exit(1);
}

/*
 * usage
 */
static void usage(const char* msg) {
  /* only have rank 0 print usage error message */
  if (myrank) goto skip_prints;

  if (msg) fprintf(stderr, "%s: %s\n", argv0, msg);
  fprintf(stderr,
          "usage: %s [options] [deck deck_id px py pz tx ty tz "
          "num_dumps num_steps]\n",
          argv0);
  fprintf(stderr, "\noptions:\n");
  fprintf(stderr, "\t-b bytes    bytes for each particle\n");
  fprintf(stderr, "\t-c count    number of particles to simulate per rank\n");
  fprintf(stderr, "\t-d dump     number of frame dumps\n");
  fprintf(stderr, "\t-o output   particle output dir (can be relative)\n");
  fprintf(stderr, "\t-s step     number of steps to perform\n");
  fprintf(stderr, "\t-T time     step time in seconds\n");
  fprintf(stderr, "\t-t sec      timeout (alarm), in seconds\n");

skip_prints:
  MPI_Finalize();
  exit(1);
}

static int compare(const void* a, const void* b) {
  return (*(int*)a - *(int*)b);
}

static void read_tracedir(const char* trace_path) {
  struct dirent* entry;
  DIR* dir = opendir(trace_path);

  if (dir == NULL) {
    fprintf(stderr, "[range-runner] Failed to open tracedir: %s\n", trace_path);
    exit(1);
  }

  trace_epcnt = 0;

  while ((entry = readdir(dir))) {
    const char* dname = entry->d_name;
    size_t dsz = strlen(dname);
    if (dsz < 3) continue;

    if ((strncmp(dname, "T.", 2))) continue;

    long int ts = strtol(&(dname[2]), NULL, 10);
    if (ts == LONG_MAX) continue;

    if (ts > INT_MAX) continue;
    int tsint = ts;

    if (trace_epcnt >= kMaxEpochs) {
      fprintf(stderr, "Too many epochs discovered!\n");
      exit(1);
    }

    trace_epochs[trace_epcnt++] = tsint;
  }

  closedir(dir);

  qsort(trace_epochs, trace_epcnt, sizeof(int), compare);

  if (myrank == 0) {
    fprintf(stdout, "[range-runner] Trace Replay: %d epochs discovered\n",
            trace_epcnt);
  }
}

/* particle data struct
 * energy is 4 bytes; balance_size is psz - 4b
 * struct members must be individually accessed
 * sizeof(struct) != psz because of padding issues
 */
struct pd {
  float energy;
  char* bdata;
};

/*
 * per-rank program state.
 */
static struct ps {
  size_t psz; /* total write size per particle */
  char pname[256];
  pd pdata;
} p;

/*
 * forward prototype decls.
 */
static void run_vpic_app();
static void do_dump();
static void do_dump_mux(int);
static void do_dump_shuffle_skew();
static void do_dump_from_trace(int);

/*
 * main program.
 */
int main(int argc, char* argv[]) {
  char* env;
  int ch;

  argv0 = argv[0];

  /* mpich says we should call this early as possible */
  if (MPI_Init(&argc, &argv) != MPI_SUCCESS) {
    FATAL("!MPI_Init");
  }

  /* we want lines!! */
  setlinebuf(stdout);

  /* setup default to zero/null, except as noted below */
  memset(&g, 0, sizeof(g));
  if (MPI_Comm_rank(MPI_COMM_WORLD, &myrank) != MPI_SUCCESS)
    FATAL("!MPI_Comm_rank");
  if (MPI_Comm_size(MPI_COMM_WORLD, &g.size) != MPI_SUCCESS)
    FATAL("!MPI_Comm_size");

  strcpy(g.pdir, "particle");
  g.p[0] = g.p[1] = g.p[2] = g.t[0] = g.t[1] = g.t[2] = -1;
  g.deckid = g.deck = "unknown";

  g.steptime = DEF_STEPTIME;
  g.nsteps = DEF_NSTEPS;
  g.ndumps = DEF_NDUMPS;
  g.psize = DEF_PARTICLESIZE;
  g.nps = DEF_NPARTICLES;
  g.timeout = DEF_TIMEOUT;
  g.trace = NULL;

  while ((ch = getopt(argc, argv, "b:c:d:i:o:s:T:t:")) != -1) {
    switch (ch) {
      case 'b':
        g.psize = atoi(optarg);
        if (g.psize < 0) usage("bad particle bytes");
        break;
      case 'c':
        g.nps = atoi(optarg);
        if (g.nps < 0) usage("bad num particles per rank");
        break;
      case 'd':
        g.ndumps = atoi(optarg);
        if (g.ndumps < 0) usage("bad num dumps");
        break;
      case 'i':
        g.trace = optarg;
        break;
      case 'o':
        strncpy(g.pdir, optarg, sizeof(g.pdir));
        break;
      case 's':
        g.nsteps = atoi(optarg);
        if (g.nsteps < 0) usage("bad num steps");
        break;
      case 'T':
        g.steptime = atof(optarg);
        if (g.steptime < 0) usage("bad steptime");
        break;
      case 't':
        g.timeout = atoi(optarg);
        if (g.timeout < 0) usage("bad timeout");
        break;
      default:
        usage(NULL);
    }
  }
  argc -= optind;
  argv += optind;

  /* all other args are optional, but are honored
   * when they are set, and override previous settings */
  if (argc > 0) g.deck = argv[0];
  if (argc > 1) g.deckid = argv[1];
  if (argc > 2) g.p[0] = atoi(argv[2]);
  if (argc > 3) g.p[1] = atoi(argv[3]);
  if (argc > 4) g.p[2] = atoi(argv[4]);
  if (argc > 5) g.t[0] = atoi(argv[5]);
  if (argc > 6) g.t[1] = atoi(argv[6]);
  if (argc > 7) g.t[2] = atoi(argv[7]);
  if (argc > 8) g.ndumps = atoi(argv[8]);
  if (argc > 9) g.nsteps = atoi(argv[9]);

  if (g.p[0] != -1 && g.p[1] != -1 && g.p[2] != -1) {
    g.nps = 100LL * g.p[0] * g.p[1] * g.p[2] / g.size;
  }

  /* check env vars */
  env = getenv("PRELOAD_Particle_size");
  if (env && env[0]) {
    g.psize = atoi(env);
    if (g.psize < 0) {
      g.psize = 0;
    }
  }

  if (myrank == 0) {
    printf("== VPIC options:\n");
    printf(" > MPI_rank   = %d\n", myrank);
    printf(" > MPI_size   = %d\n", g.size);
    if (g.trace) {
      printf(" > trace       = %s\n", g.trace);
    }
    printf(" > deck       = %s\n", g.deck);
    printf(" > deckid     = %s\n", g.deckid);
    printf(" > @p         = [ %d x %d x %d ]\n", g.p[0], g.p[1], g.p[2]);
    printf(" > @t         = [ %d x %d x %d ]\n", g.t[0], g.t[1], g.t[2]);
    printf(" > output_dir = %s\n", g.pdir);
    printf(" > time_per_step       = %.3f secs\n", g.steptime);
    printf(" > bytes_per_particle  = %d bytes\n", g.psize);
    if (g.nps > 1000000)
      printf(" > num particles       = %d M per rank\n", g.nps / 1000000);
    else if (g.nps > 1000)
      printf(" > num particles       = %d K per rank\n", g.nps / 1000);
    else
      printf(" > num particles       = %d per rank\n", g.nps);
    printf(" > num_dumps  = %d\n", g.ndumps);
    printf(" > num_steps  = %d\n", g.nsteps);
    printf(" > timeout    = %d secs\n", g.timeout);
    printf("\n");
  }

  signal(SIGALRM, sigalarm);
  alarm(g.timeout);

  if (g.trace) {
    read_tracedir(g.trace);
    if (g.ndumps > trace_epcnt) {
      fprintf(stderr,
              "[range-runner] dumps requested exceed those found in trace\n");
      exit(1);
    }
  }

  if (myrank == 0) printf("== VPIC Starting ...\n");

  memset(&p, 0, sizeof(p));
  p.psz = static_cast<size_t>(g.psize);

  size_t balance_psz = p.psz - sizeof(float);
  if (balance_psz < 0) {
    complain(EXIT_FAILURE, 0, "Particle size needs to be 4 at least.");
  }

  if (balance_psz > 0) {
    p.pdata.bdata = static_cast<char*>(malloc(p.psz - sizeof(float)));
    if (!p.pdata.bdata) complain(EXIT_FAILURE, 0, "malloc pdata failed");
    memset(p.pdata.bdata, 'x', balance_psz);
  } else {
    p.pdata.bdata = NULL;
  }

  run_vpic_app();
  MPI_Barrier(MPI_COMM_WORLD);
  if (myrank == 0) printf("\n== VPIC Done\n");
  free(p.pdata.bdata);

  MPI_Finalize();
  return 0;
}

static void run_vpic_app() {
  int rv = 0;
  if (myrank == 0) {
    rv = mkdir(g.pdir, 0777);
  }
  if (rv != 0) {
    complain(EXIT_FAILURE, 0, "mkdir %s failed errno=%d", g.pdir, errno);
  }
  for (int epoch = 0; epoch < g.ndumps; epoch++) {
    MPI_Barrier(MPI_COMM_WORLD);
    if (myrank == 0) printf("\n== VPIC Epoch %d ...\n", epoch + 1);
    int steps = g.nsteps / g.ndumps; /* vpic timesteps per epoch */
    usleep(int(g.steptime * steps * 1000 * 1000));
    do_dump_mux(epoch);
  }
}

namespace {
const unsigned char base64_table[65] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/* Total #ranks >= 16 million && #particles per rank
 * >= 16 million */
#ifdef PRELOAD_EXASCALE_RUNS
/*
 * Base64 encoding/decoding (RFC1341).
 * ASSUMPTION: PLATFORM IS LITTLE ENDIAN. Lowest memory for least
 * significant bit. Code adapted from Jouni Malinen <j@w1.fi>
 */
void base64_encoding(char* dst, uint64_t input) { /* 8 bits -> 11 bits */
  const unsigned char* in = reinterpret_cast<unsigned char*>(&input);
  *dst++ = base64_table[in[0] >> 2];
  *dst++ = base64_table[((in[0] & 0x03) << 4) | (in[1] >> 4)];
  *dst++ = base64_table[((in[1] & 0x0f) << 2) | (in[2] >> 6)];
  *dst++ = base64_table[in[2] & 0x3f];

  *dst++ = base64_table[in[3] >> 2];
  *dst++ = base64_table[((in[3] & 0x03) << 4) | (in[4] >> 4)];
  *dst++ = base64_table[((in[4] & 0x0f) << 2) | (in[5] >> 6)];
  *dst++ = base64_table[in[5] & 0x3f];

  *dst++ = base64_table[in[6] >> 2];
  *dst++ = base64_table[((in[6] & 0x03) << 4) | (in[7] >> 4)];
  *dst++ = base64_table[(in[7] & 0x0f) << 2];

  *dst = 0;
}
#else
void base64_encoding(char* dst, uint64_t input) { /* 6 bits -> 8 bits */
  const unsigned char* in = reinterpret_cast<unsigned char*>(&input);
  *dst++ = base64_table[in[0] >> 2];
  *dst++ = base64_table[((in[0] & 0x03) << 4) | (in[1] >> 4)];
  *dst++ = base64_table[((in[1] & 0x0f) << 2) | (in[2] >> 6)];
  *dst++ = base64_table[in[2] & 0x3f];

  *dst++ = base64_table[in[3] >> 2];
  *dst++ = base64_table[((in[3] & 0x03) << 4) | (in[4] >> 4)];
  *dst++ = base64_table[((in[4] & 0x0f) << 2) | (in[5] >> 6)];
  *dst++ = base64_table[in[5] & 0x3f];

  *dst = 0;
}
#endif
}  // namespace

static void do_dump_mux(int timestep) {
  // do_dump_shuffle_skew();
  if (g.trace) {
    do_dump_from_trace(timestep);
  } else {
    do_dump();
  }
}

static void do_dump() {
  FILE* file;
  DIR* dir;
  dir = opendir(g.pdir);
  if (!dir) {
    complain(EXIT_FAILURE, 0, "!opendir errno=%d", errno);
  }

  // TODO: change to uint64_t if we expect more than 2B particles
  rangeutils::WorkloadGenerator wg(range_bins, num_bins, range_start, range_end,
                                   g.nps * g.size, range_wp, myrank, g.size);

  const int prefix = snprintf(p.pname, sizeof(p.pname), "%s/", g.pdir);
#ifdef PRELOAD_EXASCALE_RUNS
  uint64_t highbits = (static_cast<uint64_t>(myrank) << 32);
#else
  uint64_t highbits = (static_cast<uint64_t>(myrank) << 24);
#endif

  float p_energy;
  float p_energy_base = 1;

  for (int i = 0; i < g.nps; i++) {
    base64_encoding(p.pname + prefix, (highbits | i));
    file = fopen(p.pname, "a");
    if (!file) complain(EXIT_FAILURE, 0, "!fopen errno=%d", errno);
    int ret = wg.next(p_energy);
    if (ret)
      complain(EXIT_FAILURE, 0,
               "[Ret %d] Ran out of particles earlier than expected", ret);
    p_energy = p_energy_base + myrank;
    p_energy_base += 2;
    // fprintf(stderr, "myrank: %d %.1f\n", myrank, p_energy);
    fwrite(static_cast<void*>(&p_energy), 1, sizeof(float), file);
    fwrite(p.pdata.bdata, 1, p.psz - sizeof(float), file);
    fclose(file);
  }

  closedir(dir);
}

void gen_bins(float* range_bins, int bin_len, int skew_degree) {
  int n, base_factor, skew_factor;

  std::ifstream my_file;
  my_file.open("/users/ankushj/range/build-post/bin/run/wp.txt");

  my_file >> n >> base_factor >> skew_factor;

  int ridx_arr[n];

  for (int i = 0; i < n; i++) {
    my_file >> ridx_arr[i];
  }

  my_file.close();

  // int ridx_arr[] = {36, 20, 21, 25, 33, 14, 24, 60};
  for (int idx = 0; idx < bin_len; idx++) {
    range_bins[idx] = base_factor;
  }

  for (int ridx = 0; ridx < n; ridx++) {
    range_bins[ridx_arr[ridx]] = skew_factor;
  }

  return;
}

static void do_dump_shuffle_skew() {
  FILE* file;
  DIR* dir;
  dir = opendir(g.pdir);
  if (!dir) {
    complain(EXIT_FAILURE, 0, "!opendir errno=%d", errno);
  }

  float range_bins[g.size];
  /* generate relative weights for different ranks */
  gen_bins(range_bins, g.size, 1);

  rangeutils::WorkloadPattern skew_wp =
      rangeutils::WorkloadPattern::WP_SHUFFLE_SKEW;

  int64_t num_queries = (int64_t)g.nps * (int64_t)g.size;
  // TODO: change to uint64_t if we expect more than 2B particles
  rangeutils::WorkloadGenerator wg(range_bins, g.size, range_start, range_end,
                                   num_queries, skew_wp, myrank, g.size);

  const int prefix = snprintf(p.pname, sizeof(p.pname), "%s/", g.pdir);
#ifdef PRELOAD_EXASCALE_RUNS
  uint64_t highbits = (static_cast<uint64_t>(myrank) << 32);
#else
  uint64_t highbits = (static_cast<uint64_t>(myrank) << 24);
#endif

  float p_energy;
  float p_energy_base = 1;

  for (int i = 0; i < g.nps; i++) {
    base64_encoding(p.pname + prefix, (highbits | i));
    file = fopen(p.pname, "a");
    if (!file) complain(EXIT_FAILURE, 0, "!fopen errno=%d", errno);
    int ret = wg.next(p_energy);
    if (ret)
      complain(EXIT_FAILURE, 0,
               "[Ret %d] Ran out of particles earlier than expected", ret);
    // fprintf(stderr, "myrank: %d %.1f\n", myrank, p_energy);
    fwrite(static_cast<void*>(&p_energy), 1, sizeof(float), file);
    fwrite(p.pdata.bdata, 1, p.psz - sizeof(float), file);
    fclose(file);
  }

  closedir(dir);
}

static void do_dump_from_trace(int epoch) {
  FILE* file;
  DIR* dir;
  dir = opendir(g.pdir);
  if (!dir) {
    complain(EXIT_FAILURE, 0, "!opendir errno=%d", errno);
  }

  // TODO: change to uint64_t if we expect more than 2B particles
  // rangeutils::WorkloadGenerator wg(range_bins, num_bins, range_start,
  // range_end, g.nps * g.size, range_wp, myrank, g.size);

  char fpath[255];
  int timestep = trace_epochs[epoch];

  snprintf(fpath, 255, "%s/T.%d/eparticle.%d.%d", g.trace, timestep, timestep,
           myrank);

  if (myrank == 0) {
    fprintf(stdout, "[range-runner] trace: %s\n", fpath);
  }

  FILE* trace_file = fopen(fpath, "r");

  const int prefix = snprintf(p.pname, sizeof(p.pname), "%s/", g.pdir);
#ifdef PRELOAD_EXASCALE_RUNS
  uint64_t highbits = (static_cast<uint64_t>(myrank) << 32);
#else
  uint64_t highbits = (static_cast<uint64_t>(myrank) << 24);
#endif

  float p_energy;
  float p_energy_base = 1;

  for (int i = 0; i < g.nps; i++) {
    size_t bytes_read =
        fread(static_cast<void*>(&p_energy), 1, sizeof(float), trace_file);

    if (bytes_read != sizeof(float)) {
      if (feof(trace_file)) {
        break;
      } else {
        complain(EXIT_FAILURE, 0, "trace fread error");
      }
    }

    base64_encoding(p.pname + prefix, (highbits | i));
    file = fopen(p.pname, "a");
    if (!file) complain(EXIT_FAILURE, 0, "!fopen errno=%d", errno);
    fwrite(static_cast<void*>(&p_energy), 1, sizeof(float), file);
    fwrite(p.pdata.bdata, 1, p.psz - sizeof(float), file);
    fclose(file);
  }

  closedir(dir);
  fclose(trace_file);
}

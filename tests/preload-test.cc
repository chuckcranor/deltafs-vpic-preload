/*
 * Copyright (c) 2017 Carnegie Mellon University.
 *
 * All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file. See the AUTHORS file for names of contributors.
 */

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <mpi.h>

#include "../src/preload_internal.h" // msg_abort
#include "../src/preload.h"

#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>

int main(int argc, char **argv) {
    int rank;
    int r = MPI_Init(&argc, &argv);
    if (r == MPI_SUCCESS) {
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    } else {
        msg_abort("MPI_init");
    }
    const char* rt = getenv("PRELOAD_Deltafs_root");
    if (rt == NULL) {
        msg_abort("no deltafs root");
    } else if (rt[0] == '/') {
        msg_abort("deltafs root must be relative");
    }
    fprintf(stderr, "deltafs_root is %s\n", rt);
    char dname[PATH_MAX];
    snprintf(dname, sizeof(dname), "%s", rt);
    fprintf(stderr, "make deltafs dir %s\n", dname);
    r = (rank == 0) ? mkdir(dname, 0777) : 0;
    if (r != 0) {
        msg_abort("mkdir");
    }

    MPI_Barrier(MPI_COMM_WORLD);

    char fname[PATH_MAX];
    snprintf(fname, sizeof(fname), "%s/xyzxyz", dname);
    fprintf(stderr, "writing into deltafs %s\n", fname);
    FILE* fp = fopen(fname, "a");
    if (fp == NULL) {
        msg_abort("fopen");
    }
    fwrite("1234", 4, 1, fp);
    fwrite("5678", 1, 4, fp);
    fwrite("9", 1, 1, fp);
    fwrite("0", 1, 1, fp);
    fwrite("abcdefghijk", 1, 11, fp);
    fwrite("lmnopqrstuv", 1, 11, fp);
    r = fclose(fp);
    if (r != 0) {
        msg_abort("fclose");
    }

    MPI_Finalize();

    char rname[PATH_MAX];
    const char* lo = getenv("PRELOAD_Local_root");
    if (lo == NULL) {
        msg_abort("no local root");
    }
    fprintf(stderr, "local_root is %s\n", rt);
    snprintf(rname, sizeof(rname), "%s/%s", lo, fname);
    fprintf(stderr, "reading from localfs %s\n", rname);
    int fd = open(rname, O_RDONLY);
    if (fd == -1) {
        msg_abort("open");
    }
    char buf[32];
    ssize_t nr = read(fd, buf, 32);
    if (nr != 32) {
        msg_abort("read");
    }
    int cmp = memcmp(buf, "1234567890abcdefghijklmnopqrstuv", 32);
    if (cmp != 0) {
        msg_abort("data lost");
    }
    close(fd);

    exit(0);
}

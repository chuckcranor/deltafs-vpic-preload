/*
 * Copyright (c) 2017 Carnegie Mellon University.
 * George Amvrosiadis <gamvrosi@cs.cmu.edu>
 *
 * All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file. See the AUTHORS file for names of contributors.
 */

#pragma once

/*
 * user specifies a prefix that we use to redirect to deltafs via
 * the PDLFS_Root env varible.   If not provided, we default to /tmp/pdlfs.
 */
#define DEFAULT_ROOT "/tmp/pdlfs"

/*
 * for simple testing, we redirect ops from watched prefix to this dir.
 * We create it ourselves.
 */
#define REDIRECT_TEST_ROOT "/tmp/pdlfs-test"

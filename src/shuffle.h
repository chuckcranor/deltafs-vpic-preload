/*
 * Copyright (c) 2016-2017 Carnegie Mellon University.
 *
 * All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file. See the AUTHORS file for names of contributors.
 */

#pragma once

/* !!! A list of all environmental variables used by us !!! */

//
//  Env                                Description
// ----------------------------------|----------------------------------
//  SHUFFLE_Mercury_proto              Mercury rpc proto
//  SHUFFLE_Force_rpc                  Send rpc even if addr is local
//  SHUFFLE_Force_sync_rpc             Avoid sending async rpc
//  SHUFFLE_Num_outstanding_rpc        Max num of outstanding rpc allowed
//  SHUFFLE_Subnet                     IP prefix of the subnet we prefer to use
//  SHUFFLE_Min_port                   The min port number we can use
//  SHUFFLE_Max_port                   The max port number we can use
//  SHUFFLE_Virtual_factor             Virtual factor used by nodes in a ch ring
//  SHUFFLE_Buffer_per_queue           Memory allocated for each rpc queue
//  SHUFFLE_Timeout                    RPC timeout
// ----------------------------------|----------------------------------
//

/*
 * The default min.
 */
#define DEFAULT_MIN_PORT 50000

/*
 * The default max.
 */
#define DEFAULT_MAX_PORT 59999

/*
 * Default amount of memory allocated for each rpc queue.
 *
 * This is considered a soft limit. There is also a hard limit
 * set for each rpc message.
 */
#define DEFAULT_BUFFER_PER_QUEUE 4000

/*
 * Default num of outstanding rpc.
 *
 * This is considered a soft limit. There is also a hard limit
 * set at compile time.
 *
 * Ignored if rpc is forced to be sync.
 */
#define DEFAULT_OUTSTANDING_RPC 16

/*
 * Default rpc timeout (in secs).
 *
 * Abort when a rpc fails to complete within this amount of time.
 *
 * A server may not be able to finish rpc in time if its
 * in-memory write buffer is full and the background compaction
 * progress is unable to keep up.
 *
 * Timeout ignored in testing mode.
 */
#define DEFAULT_TIMEOUT 300

/*
 * Default virtual factor.
 *
 * Require a reasonably large number to achieve a more
 * uniform distribution.
 */
#define DEFAULT_VIRTUAL_FACTOR 1024

/*
 * The default subnet.
 *
 * Guaranteed to be wrong in production.
 */
#define DEFAULT_SUBNET "127.0.0.1"

/*
 * If "mercury_proto" is not specified, we set it to the follows.
 *
 * This assumes the mercury linked by us has been
 * built with this specific transport.
 *
 * Use of tcp is subject to high latency.
 */
#define DEFAULT_PROTO "bmi+tcp"

// END

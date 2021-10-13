/*
 **************************************************************************
 * Copyright (c) 2015, 2017, The Linux Foundation. All rights reserved.
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all copies.
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 **************************************************************************
 */

/**
 * @file nss_def.h
 *	NSS definitions
 */

#ifndef __NSS_DEF_H
#define __NSS_DEF_H

/**
 * @addtogroup nss_common_subsystem
 * @{
 */

#define NSS_ETH_NORMAL_FRAME_MTU 1500		/**< MTU of a normal frame.*/
#define NSS_ETH_MINI_JUMBO_FRAME_MTU 1978	/**< MTU of a mini-jumbo frame. */
#define NSS_ETH_FULL_JUMBO_FRAME_MTU 9600	/**< MTU of a full jumbo frame. */

/**
 * Number of ingress or egress VLANS supported in a connection entry.
 */
#define MAX_VLAN_DEPTH 2

/**
 * Number of egress interfaces supported in a multicast connection entry.
 */
#define NSS_MC_IF_MAX 16

/**
 * Real pointer size of the system.
 */
#ifdef __LP64__
typedef uint64_t nss_ptr_t;
#else
typedef uint32_t nss_ptr_t;
#endif

/**
 * @}
 */

#endif /** __NSS_DEF_H */

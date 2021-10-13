/*
 **************************************************************************
 * Copyright (c) 2017-2018, The Linux Foundation. All rights reserved.
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
 * @file nss_ipq807x.h
 *	Architecture dependent parameters.
 */
#ifndef __NSS_IPQ807X_H
#define __NSS_IPQ807X_H

/**
 * @addtogroup nss_arch_macros_ipq807x
 * @{
 */

#define NSS_MAX_NUM_PRI 4		/**< Maximum number of priority queues in NSS. */
#define NSS_HOST_CORES 4		/**< Number of host cores. */
#define NSS_PPE_SUPPORTED		/**< PPE supported flag. */

#define NSS_N2H_RING_COUNT 5		/**< Number of N2H rings. */
#define NSS_H2N_RING_COUNT 11		/**< Number of H2N rings. */
#define NSS_RING_SIZE	128		/**< Ring size. */

#define NSS_IMEM_START	0x38000000	/**< NSS IMEM start address. */
#define NSS_IMEM_SIZE	0x30000		/**< NSS IMEM size per core. */

/**
 * @}
 */

#endif /** __NSS_IPQ807X_H */

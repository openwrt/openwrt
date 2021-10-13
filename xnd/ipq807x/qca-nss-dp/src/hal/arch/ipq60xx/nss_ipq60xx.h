/*
 * Copyright (c) 2020, The Linux Foundation. All rights reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef __NSS_DP_ARCH_H__
#define __NSS_DP_ARCH_H__

#define NSS_DP_HAL_MAX_PORTS		5
#define NSS_DP_HAL_CPU_NUM		4
#define NSS_DP_HAL_START_IFNUM		1
#define NSS_DP_HAL_MAX_MTU_SIZE		9216
#define NSS_DP_HAL_MAX_PACKET_LEN	65535
#define NSS_DP_PREHEADER_SIZE		32

/**
 * nss_dp_hal_gmac_stats
 *	The per-GMAC statistics structure.
 */
struct nss_dp_hal_gmac_stats {
};

#endif /* __NSS_DP_ARCH_H__ */

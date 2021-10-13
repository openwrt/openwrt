/*
 **************************************************************************
 * Copyright (c) 2014-2015, The Linux Foundation. All rights reserved.
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

/*
 * nss_pm.h
 *    NSS PM Driver header file
 */

#ifndef __NSS_PM_H
#define __NSS_PM_H

#include<linux/version.h>

#include <mach/msm_nss_gmac.h>
#include <mach/msm_nss_crypto.h>
#include <mach/msm_bus_board.h>
#include <mach/msm_bus.h>

#include <nss_api_if.h>

/*
 * NSS PM debug macros
 */
#if (NSS_PM_DEBUG_LEVEL < 1)
#define nss_pm_assert(fmt, args...)
#else
#define nss_pm_assert(c) if (!(c)) { BUG_ON(!(c)); }
#endif

#if (NSS_PM_DEBUG_LEVEL < 2)
#define nss_pm_warning(fmt, args...)
#else
#define nss_pm_warning(fmt, args...) printk(KERN_WARNING "nss_pm:"fmt, ##args)
#endif

#if (NSS_PM_DEBUG_LEVEL < 3)
#define nss_pm_info(fmt, args...)
#else
#define nss_pm_info(fmt, args...) printk(KERN_INFO "nss_pm:"fmt, ##args)
#endif

#if (NSS_PM_DEBUG_LEVEL < 4)
#define nss_pm_trace(fmt, args...)
#else
#define nss_pm_trace(fmt, args...) printk(KERN_DEBUG "nss_pm:"fmt, ##args)
#endif

/*
 * Define this to use NETAP driver also request for NSS Fab1 BW on behalf of GMAC driver
 */
#define NSS_PM_NETAP_GMAC_SCALING 1

/*
 * PM Client data structure
 */
typedef struct {
	uint32_t bus_perf_client;
	uint32_t clk_handle;
	uint32_t current_perf_lvl;
	uint32_t auto_scale;
	struct dentry *dentry;
	nss_pm_client_t client_id;
} nss_pm_client_data_t;

/*
 * NSS PM driver context
 */
struct nss_pm_global_ctx {
	struct dentry *pm_dentry;
	bool turbo_support;
	nss_pm_client_data_t nss_pm_client[NSS_PM_MAX_CLIENTS];
};

/*
 * Macro defining Bus vector for GMAC driver
 */
#define GMAC_BW_MBPS(_data_bw, _desc_bw) \
{ \
	.vectors = (struct msm_bus_vectors[]){ \
		{\
			.src = MSM_BUS_MASTER_NSS_GMAC_0, \
			.dst = MSM_BUS_SLAVE_EBI_CH0, \
			.ab = (_data_bw) * 16 * 1000000ULL, \
			.ib = (_data_bw) * 16 * 1000000ULL, \
		}, \
		{ \
			.src =  MSM_BUS_MASTER_NSS_GMAC_0, \
			.dst = MSM_BUS_SLAVE_NSS_TCM, \
			.ab = (_desc_bw) * 8 * 1000000ULL, \
			.ib = (_desc_bw) * 8 * 1000000ULL, \
		}, \
	}, \
	.num_paths = 2, \
}

/*
 *  Macro defining Bus vector for NSS crypto driver
 */
#define CRYPTO_BW_MBPS(_data_bw, _desc_bw) \
{ \
	.vectors = (struct msm_bus_vectors[]){ \
		{\
			.src = MSM_BUS_MASTER_NSS_CRYPTO5_0, \
			.dst = MSM_BUS_SLAVE_EBI_CH0, \
			.ab = 0, \
			.ib = 0, \
		}, \
		{ \
			.src =  MSM_BUS_MASTER_NSS_CRYPTO5_0, \
			.dst = MSM_BUS_SLAVE_NSS_TCM, \
			.ab = (_desc_bw) * 8 * 1000000ULL, \
			.ib = (_desc_bw) * 8 * 1000000ULL, \
		}, \
	}, \
	.num_paths = 2, \
}

/*
 * Macro defining Bus vector for NSS driver
 *
 */
#define NETAP_BW_MBPS(_data_bw, _desc_bw) \
{ \
	.vectors = (struct msm_bus_vectors[]){ \
		{\
			.src = MSM_BUS_MASTER_UBI32_0, \
			.dst = MSM_BUS_SLAVE_EBI_CH0, \
			.ab = (_data_bw) * 16 * 1000000ULL, \
			.ib = (_data_bw) * 16 * 1000000ULL, \
		}, \
		{ \
			.src = MSM_BUS_MASTER_UBI32_0, \
			.dst = MSM_BUS_SLAVE_NSS_TCM, \
			.ab = (_desc_bw) * 8 * 1000000ULL, \
			.ib = (_desc_bw) * 8 * 1000000ULL, \
		}, \
	}, \
	.num_paths = 2, \
}

/*
 * Initialize NSS PM top level structures
 */
void nss_pm_init(void);

/*
 * Sets the turbo support flag globally for all PM clients
 */
void nss_pm_set_turbo(void);

#endif  /** __NSS_PM_H */

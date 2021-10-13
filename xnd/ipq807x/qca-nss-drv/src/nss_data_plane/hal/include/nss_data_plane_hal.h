/*
 * Copyright (c) 2016-2020, The Linux Foundation. All rights reserved.
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

#include "nss_phys_if.h"
#include <nss_dp_api_if.h>

#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 5, 0))
#define NSS_DATA_PLANE_SUPPORTED_FEATURES (NETIF_F_HIGHDMA \
					| NETIF_F_HW_CSUM \
					| NETIF_F_RXCSUM \
					| NETIF_F_SG \
					| NETIF_F_FRAGLIST \
					| (NETIF_F_TSO | NETIF_F_TSO6 | NETIF_F_UFO))
#else
#define NSS_DATA_PLANE_SUPPORTED_FEATURES (NETIF_F_HIGHDMA \
					| NETIF_F_HW_CSUM \
					| NETIF_F_RXCSUM \
					| NETIF_F_SG \
					| NETIF_F_FRAGLIST \
					| (NETIF_F_TSO | NETIF_F_TSO6))
#endif

/*
 * nss_data_plane_param
 */
struct nss_data_plane_param {
	struct nss_dp_data_plane_ctx dpc;	/* data plane ctx base class */
	int if_num;				/* physical interface number */
	struct nss_ctx_instance *nss_ctx;	/* which nss core */
	struct nss_dp_gmac_stats gmac_stats;	/* SoC specific stats for GMAC */
	int notify_open;			/* This data plane interface has been opened or not */
	uint32_t features;			/* skb types supported by this interface */
	uint32_t bypass_nw_process;		/* Do we want to bypass NW processing in NSS for this data plane? */
};

void nss_data_plane_hal_add_dp_ops(struct nss_dp_data_plane_ops *dp_ops);
void nss_data_plane_hal_register(struct nss_ctx_instance *nss_ctx);
void nss_data_plane_hal_unregister(struct nss_ctx_instance *nss_ctx);
void nss_data_plane_hal_set_features(struct nss_dp_data_plane_ctx *dpc);
uint16_t nss_data_plane_hal_get_mtu_sz(uint16_t mtu);
void nss_data_plane_hal_stats_sync(struct nss_data_plane_param *ndpp, struct nss_phys_if_stats *stats);

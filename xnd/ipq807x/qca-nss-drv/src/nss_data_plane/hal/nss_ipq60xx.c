/*
 * Copyright (c) 2018-2020, The Linux Foundation. All rights reserved.
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

#include "nss_core.h"
#include "nss_data_plane_hal.h"

/*
 * nss_data_plane_hal_vsi_assign()
 *	Called by nss-dp to assign vsi of a data plane
 */
static int nss_data_plane_hal_vsi_assign(struct nss_dp_data_plane_ctx *dpc, uint32_t vsi)
{
	struct nss_data_plane_param *dp = (struct nss_data_plane_param *)dpc;

	return nss_phys_if_vsi_assign(dp->nss_ctx, vsi, dp->if_num);
}

/*
 * nss_data_plane_hal_vsi_unassign()
 *	Called by nss-dp to unassign vsi of a data plane
 */
static int nss_data_plane_hal_vsi_unassign(struct nss_dp_data_plane_ctx *dpc, uint32_t vsi)
{
	struct nss_data_plane_param *dp = (struct nss_data_plane_param *)dpc;

	return nss_phys_if_vsi_unassign(dp->nss_ctx, vsi, dp->if_num);
}

/*
 * nss_data_plane_hal_get_stats()
 *	Called by nss-dp to get GMAC stats
 */
static void nss_data_plane_hal_get_stats(struct nss_dp_data_plane_ctx *dpc,
						struct nss_dp_gmac_stats *stats)
{
	/*
	 * EDMA doesn't send extended statistics.
	 */
}

/*
 * nss_data_plane_hal_add_dp_ops()
 */
void nss_data_plane_hal_add_dp_ops(struct nss_dp_data_plane_ops *dp_ops)
{
	dp_ops->vsi_assign = nss_data_plane_hal_vsi_assign;
	dp_ops->vsi_unassign = nss_data_plane_hal_vsi_unassign;
	dp_ops->get_stats = nss_data_plane_hal_get_stats;
}

/*
 * nss_data_plane_hal_register()
 */
void nss_data_plane_hal_register(struct nss_ctx_instance *nss_ctx)
{
	/*
	 * Packets with the ptp service code should be delivered to
	 * PHY driver for timestamping.
	 */
	nss_cmn_register_service_code(nss_ctx, nss_phy_tstamp_rx_buf,
					NSS_PTP_EVENT_SERVICE_CODE, nss_ctx);
}

/*
 * nss_data_plane_hal_unregister()
 */
void nss_data_plane_hal_unregister(struct nss_ctx_instance *nss_ctx)
{
	nss_cmn_unregister_service_code(nss_ctx, nss_phy_tstamp_rx_buf,
					NSS_PTP_EVENT_SERVICE_CODE);
}

/*
 * nss_data_plane_hal_set_features
 */
void nss_data_plane_hal_set_features(struct nss_dp_data_plane_ctx *dpc)
{
	dpc->dev->features |= NSS_DATA_PLANE_SUPPORTED_FEATURES;
	dpc->dev->hw_features |= NSS_DATA_PLANE_SUPPORTED_FEATURES;
	dpc->dev->vlan_features |= NSS_DATA_PLANE_SUPPORTED_FEATURES;
	dpc->dev->wanted_features |= NSS_DATA_PLANE_SUPPORTED_FEATURES;
}

/*
 * nss_data_plane_hal_stats_sync()
 */
void nss_data_plane_hal_stats_sync(struct nss_data_plane_param *ndpp,
					struct nss_phys_if_stats *stats)
{
	/*
	 * EDMA does not pass sync interface stats through phys_if_stats
	 */
}

/*
 * nss_data_plane_hal_get_mtu_sz()
 */
uint16_t nss_data_plane_hal_get_mtu_sz(uint16_t mtu)
{
	/*
	 * Reserve space for preheader
	 */
	return mtu + NSS_DP_PREHEADER_SIZE;
}

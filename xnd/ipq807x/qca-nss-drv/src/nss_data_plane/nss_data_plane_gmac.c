/*
 **************************************************************************
 * Copyright (c) 2014-2020, The Linux Foundation. All rights reserved.
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

#include "nss_data_plane.h"
#include "nss_phys_if.h"
#include "nss_core.h"
#include "nss_tx_rx_common.h"
#include <nss_gmac_api_if.h>

#define NSS_DP_GMAC_SUPPORTED_FEATURES (NETIF_F_HIGHDMA | NETIF_F_HW_CSUM | NETIF_F_RXCSUM | NETIF_F_SG | NETIF_F_FRAGLIST | (NETIF_F_TSO | NETIF_F_TSO6 | NETIF_F_UFO))
#define NSS_DATA_PLANE_GMAC_MAX_INTERFACES 4

static DEFINE_SPINLOCK(nss_data_plane_gmac_stats_lock);

/*
 * nss_data_plane_gmac_param
 *	Holds the information that is going to pass to data plane host as a cookie
 */
struct nss_data_plane_gmac_param {
	int if_num;				/* physical interface number */
	struct net_device *dev;			/* net_device instance of this data plane */
	struct nss_ctx_instance *nss_ctx;	/* which nss core */
	struct nss_gmac_stats gmac_stats;	/* gmac stats */
	int notify_open;			/* This data plane interface has been opened or not */
	uint32_t features;			/* skb types supported by this interface */
	uint32_t bypass_nw_process;		/* Do we want to bypass NW processing in NSS for this data plane? */
} nss_data_plane_gmac_params[NSS_DATA_PLANE_GMAC_MAX_INTERFACES];

/*
 * __nss_data_plane_open()
 *	Called by gmac to notify open to nss-fw
 */
static int __nss_data_plane_open(void *arg, uint32_t tx_desc_ring, uint32_t rx_desc_ring, uint32_t mode)
{
	struct nss_data_plane_gmac_param *dp = (struct nss_data_plane_gmac_param *)arg;

	if (dp->notify_open) {
		return NSS_GMAC_SUCCESS;
	}
	if (nss_phys_if_open(dp->nss_ctx, tx_desc_ring, rx_desc_ring, mode, dp->if_num, dp->bypass_nw_process) == NSS_TX_SUCCESS) {
		dp->notify_open = 1;
		return NSS_GMAC_SUCCESS;
	}
	return NSS_GMAC_FAILURE;
}

/*
 * __nss_data_plane_close()
 *	Called by gmac to notify close to nss-fw
 */
static int __nss_data_plane_close(void *arg)
{
	/*
	 * We don't actually do synopsys gmac close in fw, just return success
	 */
	return NSS_GMAC_SUCCESS;
}

/*
 * __nss_data_plane_link_state()
 *	Called by gmac to notify link state change to nss-fw
 */
static int __nss_data_plane_link_state(void *arg, uint32_t link_state)
{
	struct nss_data_plane_gmac_param *dp = (struct nss_data_plane_gmac_param *)arg;

	return nss_phys_if_link_state(dp->nss_ctx, link_state, dp->if_num);
}

/*
 * __nss_data_plane_mac_addr()
 *	Called by gmac to set mac address
 */
static int __nss_data_plane_mac_addr(void *arg, uint8_t *addr)
{
	struct nss_data_plane_gmac_param *dp = (struct nss_data_plane_gmac_param *)arg;

	return nss_phys_if_mac_addr(dp->nss_ctx, addr, dp->if_num);
}

/*
 * __nss_data_plane_change_mtu()
 *	Called by gmac to change mtu of a gmac
 */
static int __nss_data_plane_change_mtu(void *arg, uint32_t mtu)
{
	struct nss_data_plane_gmac_param *dp = (struct nss_data_plane_gmac_param *)arg;

	/*
	 * MTU size check is already done in nss-gmac driver, just pass to phys_if
	 */
	return nss_phys_if_change_mtu(dp->nss_ctx, mtu, dp->if_num);
}

/*
 * __nss_data_plane_pause_on_off()
 *	Called by gmac to enable/disable pause frames
 */
static int __nss_data_plane_pause_on_off(void *arg, uint32_t pause_on)
{
	struct nss_data_plane_gmac_param *dp = (struct nss_data_plane_gmac_param *)arg;

	return nss_phys_if_pause_on_off(dp->nss_ctx, pause_on, dp->if_num);
}

/*
 * __nss_data_plane_buf()
 *	Called by gmac to pass a sk_buff for xmit
 */
static int __nss_data_plane_buf(void *arg, struct sk_buff *os_buf)
{
	struct nss_data_plane_gmac_param *dp = (struct nss_data_plane_gmac_param *)arg;

	return nss_phys_if_buf(dp->nss_ctx, os_buf, dp->if_num);
}

/*
 * __nss_data_plane_set_features()
 *	Called by gmac to allow data plane to modify the set of features it supports
 */
static void __nss_data_plane_set_features(struct net_device *netdev)
{
	netdev->features |= NSS_DP_GMAC_SUPPORTED_FEATURES;
	netdev->hw_features |= NSS_DP_GMAC_SUPPORTED_FEATURES;
	netdev->vlan_features |= NSS_DP_GMAC_SUPPORTED_FEATURES;
	netdev->wanted_features |= NSS_DP_GMAC_SUPPORTED_FEATURES;
}

/*
 * __nss_data_plane_get_stats()
 */
static void __nss_data_plane_get_stats(void *arg, struct nss_gmac_stats *stats)
{
	struct nss_data_plane_gmac_param *dp = (struct nss_data_plane_gmac_param *)arg;

	spin_lock_bh(&nss_data_plane_gmac_stats_lock);
	memcpy(stats, &dp->gmac_stats, sizeof(*stats));
	spin_unlock_bh(&nss_data_plane_gmac_stats_lock);
}

/*
 * nss offload data plane ops
 */
static struct nss_gmac_data_plane_ops dp_ops = {
	.open		= __nss_data_plane_open,
	.close		= __nss_data_plane_close,
	.link_state	= __nss_data_plane_link_state,
	.mac_addr	= __nss_data_plane_mac_addr,
	.change_mtu	= __nss_data_plane_change_mtu,
	.xmit		= __nss_data_plane_buf,
	.set_features	= __nss_data_plane_set_features,
	.pause_on_off	= __nss_data_plane_pause_on_off,
	.get_stats	= __nss_data_plane_get_stats,
};

/*
 * nss_data_plane_register_to_nss_gmac()
 */
static bool nss_data_plane_register_to_nss_gmac(struct nss_ctx_instance *nss_ctx, int if_num)
{
	struct nss_data_plane_gmac_param *ndpp = &nss_data_plane_gmac_params[if_num];
	struct nss_top_instance *nss_top = nss_ctx->nss_top;
	struct net_device *netdev;
	bool is_open;
	int core;

	netdev = nss_gmac_get_netdev_by_macid(if_num);
	if (!netdev) {
		nss_info("Platform don't have gmac%d enabled, don't bring up nss_phys_if and don't register to nss-gmac", if_num);
		return false;
	}

	is_open = nss_gmac_is_in_open_state(netdev);
	ndpp->dev = netdev;
	ndpp->nss_ctx = nss_ctx;
	ndpp->if_num = if_num;
	ndpp->notify_open = 0;
	ndpp->features = 0;

	/*
	 * Check if NSS NW processing to be bypassed for this GMAC
	 */
	if (nss_skip_nw_process) {
		ndpp->bypass_nw_process = 1;
	} else {
		ndpp->bypass_nw_process = 0;
	}

	if (nss_gmac_override_data_plane(netdev, &dp_ops, ndpp) != NSS_GMAC_SUCCESS) {
		nss_info("Override nss-gmac data plane failed\n");
		return false;
	}

	/*
	 * Setup the receive callback so that data pkts received form NSS-FW will
	 * be redirected to the gmac driver as we are overriding the data plane
	 */
	nss_top->phys_if_handler_id[if_num] = nss_ctx->id;
	nss_phys_if_register_handler(nss_ctx, if_num);

	/*
	 * Packets recieved on physical interface can be exceptioned to HLOS
	 * from any NSS core so we need to register data plane for all
	 */
	for (core = 0; core < nss_top->num_nss; core++) {
		nss_core_register_subsys_dp(&nss_top->nss[core], if_num, nss_gmac_receive, NULL, NULL, netdev, ndpp->features);
	}

	/*
	 * Now we are registered and our side is ready, if the gmac was opened, ask it to start again
	 */
	if (is_open) {
		nss_gmac_start_data_plane(netdev, ndpp);
	}
	return true;
}

/*
 * nss_data_plane_unregister_from_nss_gmac()
 */
static void nss_data_plane_unregister_from_nss_gmac(int if_num)
{
	nss_gmac_restore_data_plane(nss_data_plane_gmac_params[if_num].dev);
	nss_data_plane_gmac_params[if_num].dev = NULL;
	nss_data_plane_gmac_params[if_num].nss_ctx = NULL;
	nss_data_plane_gmac_params[if_num].if_num = 0;
	nss_data_plane_gmac_params[if_num].notify_open = 0;
	nss_data_plane_gmac_params[if_num].bypass_nw_process = 0;
}

/*
 * __nss_data_plane_register()
 */
static void __nss_data_plane_register(struct nss_ctx_instance *nss_ctx)
{
	int i;

	for (i = 0; i < NSS_DATA_PLANE_GMAC_MAX_INTERFACES; i++) {
		if (!nss_data_plane_register_to_nss_gmac(nss_ctx, i)) {
			nss_warning("%px: Register data plane failed for gmac:%d\n", nss_ctx, i);
		} else {
			nss_info("%px: Register data plan to gmac:%d success\n", nss_ctx, i);
		}
	}
}

/*
 * __nss_data_plane_unregister()
 */
static void __nss_data_plane_unregister(void)
{
	int i, core;

	for (core = 0; core < nss_top_main.num_nss; core++) {
		for (i = 0; i < NSS_DATA_PLANE_GMAC_MAX_INTERFACES; i++) {
			if (nss_top_main.nss[core].subsys_dp_register[i].ndev) {
				nss_data_plane_unregister_from_nss_gmac(i);
				nss_core_unregister_subsys_dp(&nss_top_main.nss[core], i);
			}
		}
	}
}

/*
 * __nss_data_plane_stats_sync()
 *	Handle the syncing of gmac data plane stats.
 */
static void __nss_data_plane_stats_sync(struct nss_phys_if_stats *stats, uint16_t interface)
{
	struct nss_gmac_stats *gmac_stats = &nss_data_plane_gmac_params[interface].gmac_stats;

	spin_lock_bh(&nss_data_plane_gmac_stats_lock);
	gmac_stats->rx_bytes += stats->if_stats.rx_bytes;
	gmac_stats->rx_packets += stats->if_stats.rx_packets;
	gmac_stats->rx_errors += stats->estats.rx_errors;
	gmac_stats->rx_receive_errors += stats->estats.rx_receive_errors;
	gmac_stats->rx_descriptor_errors += stats->estats.rx_descriptor_errors;
	gmac_stats->rx_late_collision_errors += stats->estats.rx_late_collision_errors;
	gmac_stats->rx_dribble_bit_errors += stats->estats.rx_dribble_bit_errors;
	gmac_stats->rx_length_errors += stats->estats.rx_length_errors;
	gmac_stats->rx_ip_header_errors += stats->estats.rx_ip_header_errors;
	gmac_stats->rx_ip_payload_errors += stats->estats.rx_ip_payload_errors;
	gmac_stats->rx_no_buffer_errors += stats->estats.rx_no_buffer_errors;
	gmac_stats->rx_transport_csum_bypassed += stats->estats.rx_transport_csum_bypassed;

	gmac_stats->tx_bytes += stats->if_stats.tx_bytes;
	gmac_stats->tx_packets += stats->if_stats.tx_packets;
	gmac_stats->tx_collisions += stats->estats.tx_collisions;
	gmac_stats->tx_errors += stats->estats.tx_errors;
	gmac_stats->tx_jabber_timeout_errors += stats->estats.tx_jabber_timeout_errors;
	gmac_stats->tx_frame_flushed_errors += stats->estats.tx_frame_flushed_errors;
	gmac_stats->tx_loss_of_carrier_errors += stats->estats.tx_loss_of_carrier_errors;
	gmac_stats->tx_no_carrier_errors += stats->estats.tx_no_carrier_errors;
	gmac_stats->tx_late_collision_errors += stats->estats.tx_late_collision_errors;
	gmac_stats->tx_excessive_collision_errors += stats->estats.tx_excessive_collision_errors;
	gmac_stats->tx_excessive_deferral_errors += stats->estats.tx_excessive_deferral_errors;
	gmac_stats->tx_underflow_errors += stats->estats.tx_underflow_errors;
	gmac_stats->tx_ip_header_errors += stats->estats.tx_ip_header_errors;
	gmac_stats->tx_ip_payload_errors += stats->estats.tx_ip_payload_errors;
	gmac_stats->tx_dropped += stats->estats.tx_dropped;

	gmac_stats->hw_errs[0] += stats->estats.hw_errs[0];
	gmac_stats->hw_errs[1] += stats->estats.hw_errs[1];
	gmac_stats->hw_errs[2] += stats->estats.hw_errs[2];
	gmac_stats->hw_errs[3] += stats->estats.hw_errs[3];
	gmac_stats->hw_errs[4] += stats->estats.hw_errs[4];
	gmac_stats->hw_errs[5] += stats->estats.hw_errs[5];
	gmac_stats->hw_errs[6] += stats->estats.hw_errs[6];
	gmac_stats->hw_errs[7] += stats->estats.hw_errs[7];
	gmac_stats->hw_errs[8] += stats->estats.hw_errs[8];
	gmac_stats->hw_errs[9] += stats->estats.hw_errs[9];
	gmac_stats->rx_missed += stats->estats.rx_missed;

	gmac_stats->fifo_overflows += stats->estats.fifo_overflows;
	gmac_stats->rx_scatter_errors += stats->estats.rx_scatter_errors;
	gmac_stats->tx_ts_create_errors += stats->estats.tx_ts_create_errors;
	gmac_stats->gmac_total_ticks += stats->estats.gmac_total_ticks;
	gmac_stats->gmac_worst_case_ticks += stats->estats.gmac_worst_case_ticks;
	gmac_stats->gmac_iterations += stats->estats.gmac_iterations;
	gmac_stats->tx_pause_frames += stats->estats.tx_pause_frames;
	gmac_stats->mmc_rx_overflow_errors += stats->estats.mmc_rx_overflow_errors;
	gmac_stats->mmc_rx_watchdog_timeout_errors += stats->estats.mmc_rx_watchdog_timeout_errors;
	gmac_stats->mmc_rx_crc_errors += stats->estats.mmc_rx_crc_errors;
	gmac_stats->mmc_rx_ip_header_errors += stats->estats.mmc_rx_ip_header_errors;
	gmac_stats->mmc_rx_octets_g += stats->estats.mmc_rx_octets_g;
	gmac_stats->mmc_rx_ucast_frames += stats->estats.mmc_rx_ucast_frames;
	gmac_stats->mmc_rx_bcast_frames += stats->estats.mmc_rx_bcast_frames;
	gmac_stats->mmc_rx_mcast_frames += stats->estats.mmc_rx_mcast_frames;
	gmac_stats->mmc_rx_undersize += stats->estats.mmc_rx_undersize;
	gmac_stats->mmc_rx_oversize += stats->estats.mmc_rx_oversize;
	gmac_stats->mmc_rx_jabber += stats->estats.mmc_rx_jabber;
	gmac_stats->mmc_rx_octets_gb += stats->estats.mmc_rx_octets_gb;
	gmac_stats->mmc_rx_frag_frames_g += stats->estats.mmc_rx_frag_frames_g;
	gmac_stats->mmc_tx_octets_g += stats->estats.mmc_tx_octets_g;
	gmac_stats->mmc_tx_ucast_frames += stats->estats.mmc_tx_ucast_frames;
	gmac_stats->mmc_tx_bcast_frames += stats->estats.mmc_tx_bcast_frames;
	gmac_stats->mmc_tx_mcast_frames += stats->estats.mmc_tx_mcast_frames;
	gmac_stats->mmc_tx_deferred += stats->estats.mmc_tx_deferred;
	gmac_stats->mmc_tx_single_col += stats->estats.mmc_tx_single_col;
	gmac_stats->mmc_tx_multiple_col += stats->estats.mmc_tx_multiple_col;
	gmac_stats->mmc_tx_octets_gb += stats->estats.mmc_tx_octets_gb;

	spin_unlock_bh(&nss_data_plane_gmac_stats_lock);
}

/*
 * __nss_data_plane_get_mtu_sz()
 */
static uint16_t __nss_data_plane_get_mtu_sz(uint16_t max_mtu)
{
	/*
	 * GMACs support 3 Modes
	 * Normal Mode Payloads upto 1522 Bytes ( 1500 + 14 + 4(Vlan) + 4(CRC))
	 * Mini Jumbo Mode Payloads upto 2000 Bytes (1978 + 14 + 4(Vlan) + 4 (CRC))
	 * Full Jumbo Mode payloads upto 9622 Bytes (9600 + 14 + 4(Vlan) + 4 (CRC))
	 */

	/*
	 * The configured MTU value on a gmac interface should be one of these
	 * cases. Finding the Needed MTU size that is required for GMAC to
	 * successfully receive the frame.
	 */
	if (max_mtu <= NSS_GMAC_NORMAL_FRAME_MTU) {
		return NSS_GMAC_NORMAL_FRAME_MTU;
	}
	if (max_mtu <= NSS_GMAC_MINI_JUMBO_FRAME_MTU) {
		return NSS_GMAC_MINI_JUMBO_FRAME_MTU;
	}
	if (max_mtu <= NSS_GMAC_FULL_JUMBO_FRAME_MTU) {
		return NSS_GMAC_FULL_JUMBO_FRAME_MTU;
	}
	return 0;
}

/*
 * nss_data_plane_gmac_ops
 */
struct nss_data_plane_ops nss_data_plane_gmac_ops = {
	.data_plane_register = &__nss_data_plane_register,
	.data_plane_unregister = &__nss_data_plane_unregister,
	.data_plane_stats_sync = &__nss_data_plane_stats_sync,
	.data_plane_get_mtu_sz = &__nss_data_plane_get_mtu_sz,
};

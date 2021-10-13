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

#include "nss_core.h"
#include "nss_data_plane_hal.h"

static DEFINE_SPINLOCK(nss_data_plane_hal_gmac_stats_lock);

/*
 * nss_data_plane_hal_get_stats()
 *	Called by nss-dp to get GMAC stats
 */
static void nss_data_plane_hal_get_stats(struct nss_dp_data_plane_ctx *dpc,
					struct nss_dp_gmac_stats *stats)
{
	struct nss_data_plane_param *dp = (struct nss_data_plane_param *)dpc;

	spin_lock_bh(&nss_data_plane_hal_gmac_stats_lock);
	memcpy(stats, &dp->gmac_stats, sizeof(*stats));
	spin_unlock_bh(&nss_data_plane_hal_gmac_stats_lock);
}

/*
 * nss_data_plane_hal_add_dp_ops()
 */
void nss_data_plane_hal_add_dp_ops(struct nss_dp_data_plane_ops *dp_ops)
{
	dp_ops->get_stats = nss_data_plane_hal_get_stats;
}

/*
 * nss_data_plane_hal_register()
 */
void nss_data_plane_hal_register(struct nss_ctx_instance *nss_ctx)
{
}

/*
 * nss_data_plane_hal_unregister()
 */
void nss_data_plane_hal_unregister(struct nss_ctx_instance *nss_ctx)
{
}

/*
 * nss_data_plane_hal_set_features
 */
void nss_data_plane_hal_set_features(struct nss_dp_data_plane_ctx *dpc)
{
	dpc->dev->features |= NSS_DATA_PLANE_SUPPORTED_FEATURES;
	dpc->dev->hw_features |= NSS_DATA_PLANE_SUPPORTED_FEATURES;
	dpc->dev->wanted_features |= NSS_DATA_PLANE_SUPPORTED_FEATURES;

	/*
	 * Synopsys GMAC does not support checksum offload for QinQ VLANs.
	 * Hence, we do not advertise checksum offload support for VLANs.
	 */
	dpc->dev->vlan_features |= NSS_DATA_PLANE_SUPPORTED_FEATURES &
					(~(NETIF_F_RXCSUM | NETIF_F_HW_CSUM));
}

/*
 * nss_data_plane_hal_stats_sync()
 */
void nss_data_plane_hal_stats_sync(struct nss_data_plane_param *ndpp,
					struct nss_phys_if_stats *stats)
{
	struct nss_dp_hal_gmac_stats *gmac_stats = &ndpp->gmac_stats.stats;

	spin_lock_bh(&nss_data_plane_hal_gmac_stats_lock);

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

	spin_unlock_bh(&nss_data_plane_hal_gmac_stats_lock);
}

/*
 * nss_data_plane_hal_get_mtu_sz()
 */
uint16_t nss_data_plane_hal_get_mtu_sz(uint16_t mtu)
{
	/*
	 * GMACs support 3 Modes
	 * Normal Mode Payloads upto 1522 Bytes ( 1500 + 14 + 4(Vlan) + 4(CRC))
	 * Mini Jumbo Mode Payloads upto 2000 Bytes (1978 + 14 + 4(Vlan) + 4 (CRC))
	 * Full Jumbo Mode payloads upto 9022 Bytes (9000 + 14 + 4(Vlan) + 4 (CRC))
	 */

	/*
	 * The configured MTU value on a GMAC interface should be one of these
	 * cases. Finding the Needed MTU size that is required for GMAC to
	 * successfully receive the frame.
	 */
	if (mtu <= NSS_DP_GMAC_NORMAL_FRAME_MTU) {
	    return NSS_DP_GMAC_NORMAL_FRAME_MTU;
	}
	if (mtu <= NSS_DP_GMAC_MINI_JUMBO_FRAME_MTU) {
	    return NSS_DP_GMAC_MINI_JUMBO_FRAME_MTU;
	}
	if (mtu <= NSS_DP_GMAC_FULL_JUMBO_FRAME_MTU) {
		return NSS_DP_GMAC_FULL_JUMBO_FRAME_MTU;
	}
	return 0;
}

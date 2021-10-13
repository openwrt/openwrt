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

#include <fal/fal_mib.h>
#include <fal/fal_port_ctrl.h>
#include <linux/delay.h>
#include <linux/ethtool.h>
#include <linux/vmalloc.h>
#include <linux/spinlock.h>
#include <linux/io.h>
#include <nss_dp_hal.h>
#include "syn_dev.h"
#include "syn_reg.h"

#define SYN_STAT(m)	offsetof(struct nss_dp_hal_gmac_stats, m)
#define HW_ERR_SIZE	sizeof(uint64_t)

/*
 * Array to store ethtool statistics
 */
struct syn_ethtool_stats {
	uint8_t stat_string[ETH_GSTRING_LEN];
	uint64_t stat_offset;
};

/*
 * Array of strings describing statistics
 */
static const struct syn_ethtool_stats syn_gstrings_stats[] = {
	{"rx_bytes", SYN_STAT(rx_bytes)},
	{"rx_packets", SYN_STAT(rx_packets)},
	{"rx_errors", SYN_STAT(rx_errors)},
	{"rx_receive_errors", SYN_STAT(rx_receive_errors)},
	{"rx_descriptor_errors", SYN_STAT(rx_descriptor_errors)},
	{"rx_late_collision_errors", SYN_STAT(rx_late_collision_errors)},
	{"rx_dribble_bit_errors", SYN_STAT(rx_dribble_bit_errors)},
	{"rx_length_errors", SYN_STAT(rx_length_errors)},
	{"rx_ip_header_errors", SYN_STAT(rx_ip_header_errors)},
	{"rx_ip_payload_errors", SYN_STAT(rx_ip_payload_errors)},
	{"rx_no_buffer_errors", SYN_STAT(rx_no_buffer_errors)},
	{"rx_transport_csum_bypassed", SYN_STAT(rx_transport_csum_bypassed)},
	{"tx_bytes", SYN_STAT(tx_bytes)},
	{"tx_packets", SYN_STAT(tx_packets)},
	{"tx_collisions", SYN_STAT(tx_collisions)},
	{"tx_errors", SYN_STAT(tx_errors)},
	{"tx_jabber_timeout_errors", SYN_STAT(tx_jabber_timeout_errors)},
	{"tx_frame_flushed_errors", SYN_STAT(tx_frame_flushed_errors)},
	{"tx_loss_of_carrier_errors", SYN_STAT(tx_loss_of_carrier_errors)},
	{"tx_no_carrier_errors", SYN_STAT(tx_no_carrier_errors)},
	{"tx_late_collision_errors", SYN_STAT(tx_late_collision_errors)},
	{"tx_excessive_collision_errors", SYN_STAT(tx_excessive_collision_errors)},
	{"tx_excessive_deferral_errors", SYN_STAT(tx_excessive_deferral_errors)},
	{"tx_underflow_errors", SYN_STAT(tx_underflow_errors)},
	{"tx_ip_header_errors", SYN_STAT(tx_ip_header_errors)},
	{"tx_ip_payload_errors", SYN_STAT(tx_ip_payload_errors)},
	{"tx_dropped", SYN_STAT(tx_dropped)},
	{"rx_missed", SYN_STAT(rx_missed)},
	{"fifo_overflows", SYN_STAT(fifo_overflows)},
	{"rx_scatter_errors", SYN_STAT(rx_scatter_errors)},
	{"tx_ts_create_errors", SYN_STAT(tx_ts_create_errors)},
	{"pmt_interrupts", SYN_STAT(hw_errs[0])},
	{"mmc_interrupts", SYN_STAT(hw_errs[0]) + (1 * HW_ERR_SIZE)},
	{"line_interface_interrupts", SYN_STAT(hw_errs[0]) + (2 * HW_ERR_SIZE)},
	{"fatal_bus_error_interrupts", SYN_STAT(hw_errs[0]) + (3 * HW_ERR_SIZE)},
	{"rx_buffer_unavailable_interrupts", SYN_STAT(hw_errs[0]) + (4 * HW_ERR_SIZE)},
	{"rx_process_stopped_interrupts", SYN_STAT(hw_errs[0]) + (5 * HW_ERR_SIZE)},
	{"tx_underflow_interrupts", SYN_STAT(hw_errs[0]) + (6 * HW_ERR_SIZE)},
	{"rx_overflow_interrupts", SYN_STAT(hw_errs[0]) + (7 * HW_ERR_SIZE)},
	{"tx_jabber_timeout_interrutps", SYN_STAT(hw_errs[0]) + (8 * HW_ERR_SIZE)},
	{"tx_process_stopped_interrutps", SYN_STAT(hw_errs[0]) + (9 * HW_ERR_SIZE)},
	{"gmac_total_ticks", SYN_STAT(gmac_total_ticks)},
	{"gmac_worst_case_ticks", SYN_STAT(gmac_worst_case_ticks)},
	{"gmac_iterations", SYN_STAT(gmac_iterations)},
	{"tx_pause_frames", SYN_STAT(tx_pause_frames)},
	{"mmc_rx_overflow_errors", SYN_STAT(mmc_rx_overflow_errors)},
	{"mmc_rx_watchdog_timeout_errors", SYN_STAT(mmc_rx_watchdog_timeout_errors)},
	{"mmc_rx_crc_errors", SYN_STAT(mmc_rx_crc_errors)},
	{"mmc_rx_ip_header_errors", SYN_STAT(mmc_rx_ip_header_errors)},
	{"mmc_rx_octets_g", SYN_STAT(mmc_rx_octets_g)},
	{"mmc_rx_ucast_frames", SYN_STAT(mmc_rx_ucast_frames)},
	{"mmc_rx_bcast_frames", SYN_STAT(mmc_rx_bcast_frames)},
	{"mmc_rx_mcast_frames", SYN_STAT(mmc_rx_mcast_frames)},
	{"mmc_rx_undersize", SYN_STAT(mmc_rx_undersize)},
	{"mmc_rx_oversize", SYN_STAT(mmc_rx_oversize)},
	{"mmc_rx_jabber", SYN_STAT(mmc_rx_jabber)},
	{"mmc_rx_octets_gb", SYN_STAT(mmc_rx_octets_gb)},
	{"mmc_rx_frag_frames_g", SYN_STAT(mmc_rx_frag_frames_g)},
	{"mmc_tx_octets_g", SYN_STAT(mmc_tx_octets_g)},
	{"mmc_tx_ucast_frames", SYN_STAT(mmc_tx_ucast_frames)},
	{"mmc_tx_bcast_frames", SYN_STAT(mmc_tx_bcast_frames)},
	{"mmc_tx_mcast_frames", SYN_STAT(mmc_tx_mcast_frames)},
	{"mmc_tx_deferred", SYN_STAT(mmc_tx_deferred)},
	{"mmc_tx_single_col", SYN_STAT(mmc_tx_single_col)},
	{"mmc_tx_multiple_col", SYN_STAT(mmc_tx_multiple_col)},
	{"mmc_tx_octets_gb", SYN_STAT(mmc_tx_octets_gb)},
};

#define SYN_STATS_LEN		ARRAY_SIZE(syn_gstrings_stats)

/*
 * syn_set_rx_flow_ctrl()
 */
static inline void syn_set_rx_flow_ctrl(struct nss_gmac_hal_dev *nghd)
{
	hal_set_reg_bits(nghd, SYN_MAC_FLOW_CONTROL,
			SYN_MAC_FC_RX_FLOW_CONTROL);
}

/*
 * syn_clear_rx_flow_ctrl()
 */
static inline void syn_clear_rx_flow_ctrl(struct nss_gmac_hal_dev *nghd)
{
	hal_clear_reg_bits(nghd, SYN_MAC_FLOW_CONTROL,
			SYN_MAC_FC_RX_FLOW_CONTROL);

}

/*
 * syn_set_tx_flow_ctrl()
 */
static inline void syn_set_tx_flow_ctrl(struct nss_gmac_hal_dev *nghd)
{
	hal_set_reg_bits(nghd, SYN_MAC_FLOW_CONTROL,
			SYN_MAC_FC_TX_FLOW_CONTROL);
}

/*
 * syn_send_tx_pause_frame()
 */
static inline void syn_send_tx_pause_frame(struct nss_gmac_hal_dev *nghd)
{
	syn_set_tx_flow_ctrl(nghd);
	hal_set_reg_bits(nghd, SYN_MAC_FLOW_CONTROL,
			SYN_MAC_FC_SEND_PAUSE_FRAME);
}

/*
 * syn_clear_tx_flow_ctrl()
 */
static inline void syn_clear_tx_flow_ctrl(struct nss_gmac_hal_dev *nghd)
{
	hal_clear_reg_bits(nghd, SYN_MAC_FLOW_CONTROL,
			SYN_MAC_FC_TX_FLOW_CONTROL);
}

/*
 * syn_rx_enable()
 */
static inline void syn_rx_enable(struct nss_gmac_hal_dev *nghd)
{
	hal_set_reg_bits(nghd, SYN_MAC_CONFIGURATION, SYN_MAC_RX);
	hal_set_reg_bits(nghd, SYN_MAC_FRAME_FILTER, SYN_MAC_FILTER_OFF);
}

/*
 * syn_tx_enable()
 */
static inline void syn_tx_enable(struct nss_gmac_hal_dev *nghd)
{
	hal_set_reg_bits(nghd, SYN_MAC_CONFIGURATION, SYN_MAC_TX);
}

/************Ip checksum offloading APIs*************/

/*
 * syn_enable_rx_chksum_offload()
 *	Enable IPv4 header and IPv4/IPv6 TCP/UDP checksum calculation by GMAC.
 */
static inline void syn_enable_rx_chksum_offload(struct nss_gmac_hal_dev *nghd)
{
	hal_set_reg_bits(nghd,
			      SYN_MAC_CONFIGURATION, SYN_MAC_RX_IPC_OFFLOAD);
}

/*
 * syn_disable_rx_chksum_offload()
 *	Disable the IP checksum offloading in receive path.
 */
static inline void syn_disable_rx_chksum_offload(struct nss_gmac_hal_dev *nghd)
{
	hal_clear_reg_bits(nghd,
				SYN_MAC_CONFIGURATION, SYN_MAC_RX_IPC_OFFLOAD);
}

/*
 * syn_rx_tcpip_chksum_drop_enable()
 *	Instruct the DMA to drop the packets that fail TCP/IP checksum.
 *
 * This is to instruct the receive DMA engine to drop the recevied
 * packet if they fails the tcp/ip checksum in hardware. Valid only when
 * full checksum offloading is enabled(type-2).
 */
static inline void syn_rx_tcpip_chksum_drop_enable(struct nss_gmac_hal_dev *nghd)
{
	hal_clear_reg_bits(nghd,
				SYN_DMA_OPERATION_MODE, SYN_DMA_DISABLE_DROP_TCP_CS);
}

/*******************Ip checksum offloading APIs**********************/

/*
 * syn_ipc_offload_init()
 *	Initialize IPC Checksum offloading.
 */
static inline void syn_ipc_offload_init(struct nss_gmac_hal_dev *nghd)
{
	struct nss_dp_dev *dp_priv;
	dp_priv = netdev_priv(nghd->netdev);

	if (test_bit(__NSS_DP_RXCSUM, &dp_priv->flags)) {
		/*
		 * Enable the offload engine in the receive path
		 */
		syn_enable_rx_chksum_offload(nghd);

		/*
		 * DMA drops the packets if error in encapsulated ethernet
		 * payload.
		 */
		syn_rx_tcpip_chksum_drop_enable(nghd);
		netdev_dbg(nghd->netdev, "%s: enable Rx checksum\n", __func__);
	} else {
		syn_disable_rx_chksum_offload(nghd);
		netdev_dbg(nghd->netdev, "%s: disable Rx checksum\n", __func__);
	}
}

/*
 * syn_disable_mac_interrupt()
 *	Disable all the interrupts.
 */
static inline void syn_disable_mac_interrupt(struct nss_gmac_hal_dev *nghd)
{
	hal_write_reg(nghd->mac_base, SYN_INTERRUPT_MASK, 0xffffffff);
}

/*
 * syn_disable_mmc_tx_interrupt()
 *	Disable the MMC Tx interrupt.
 *
 * The MMC tx interrupts are masked out as per the mask specified.
 */
static inline void syn_disable_mmc_tx_interrupt(struct nss_gmac_hal_dev *nghd,
						uint32_t mask)
{
	hal_set_reg_bits(nghd, SYN_MMC_TX_INTERRUPT_MASK, mask);
}

/*
 * syn_disable_mmc_rx_interrupt()
 *	Disable the MMC Rx interrupt.
 *
 * The MMC rx interrupts are masked out as per the mask specified.
 */
static inline void syn_disable_mmc_rx_interrupt(struct nss_gmac_hal_dev *nghd,
						uint32_t mask)
{
	hal_set_reg_bits(nghd, SYN_MMC_RX_INTERRUPT_MASK, mask);
}

/*
 * syn_disable_mmc_ipc_rx_interrupt()
 *	Disable the MMC ipc rx checksum offload interrupt.
 *
 * The MMC ipc rx checksum offload interrupts are masked out as
 * per the mask specified.
 */
static inline void syn_disable_mmc_ipc_rx_interrupt(struct nss_gmac_hal_dev *nghd,
					   uint32_t mask)
{
	hal_set_reg_bits(nghd, SYN_MMC_IPC_RX_INTR_MASK, mask);
}

/*
 * syn_disable_dma_interrupt()
 *	Disables all DMA interrupts.
 */
void syn_disable_dma_interrupt(struct nss_gmac_hal_dev *nghd)
{
	hal_write_reg(nghd->mac_base, SYN_DMA_INT_ENABLE, SYN_DMA_INT_DISABLE);
}

/*
 * syn_enable_dma_interrupt()
 *	Enables all DMA interrupts.
 */
void syn_enable_dma_interrupt(struct nss_gmac_hal_dev *nghd)
{
	hal_write_reg(nghd->mac_base, SYN_DMA_INT_ENABLE, SYN_DMA_INT_EN);
}

/*
 * syn_disable_interrupt_all()
 *	Disable all the interrupts.
 */
static inline void syn_disable_interrupt_all(struct nss_gmac_hal_dev *nghd)
{
	syn_disable_mac_interrupt(nghd);
	syn_disable_dma_interrupt(nghd);
	syn_disable_mmc_tx_interrupt(nghd, 0xFFFFFFFF);
	syn_disable_mmc_rx_interrupt(nghd, 0xFFFFFFFF);
	syn_disable_mmc_ipc_rx_interrupt(nghd, 0xFFFFFFFF);
}

/*
 * syn_dma_bus_mode_init()
 *	Function to program DMA bus mode register.
 */
static inline void syn_dma_bus_mode_init(struct nss_gmac_hal_dev *nghd)
{
	hal_write_reg(nghd->mac_base, SYN_DMA_BUS_MODE, SYN_DMA_BUS_MODE_VAL);
}

/*
 * syn_clear_dma_status()
 *	Clear all the pending dma interrupts.
 */
void syn_clear_dma_status(struct nss_gmac_hal_dev *nghd)
{
	uint32_t data;

	data = hal_read_reg(nghd->mac_base, SYN_DMA_STATUS);
	hal_write_reg(nghd->mac_base, SYN_DMA_STATUS, data);
}

/*
 * syn_enable_dma_rx()
 *	Enable Rx GMAC operation
 */
void syn_enable_dma_rx(struct nss_gmac_hal_dev *nghd)
{
	uint32_t data;

	data = hal_read_reg(nghd->mac_base, SYN_DMA_OPERATION_MODE);
	data |= SYN_DMA_RX_START;
	hal_write_reg(nghd->mac_base, SYN_DMA_OPERATION_MODE, data);
}

/*
 * syn_disable_dma_rx()
 *	Disable Rx GMAC operation
 */
void syn_disable_dma_rx(struct nss_gmac_hal_dev *nghd)
{
	uint32_t data;

	data = hal_read_reg(nghd->mac_base, SYN_DMA_OPERATION_MODE);
	data &= ~SYN_DMA_RX_START;
	hal_write_reg(nghd->mac_base, SYN_DMA_OPERATION_MODE, data);
}

/*
 * syn_enable_dma_tx()
 *	Enable Rx GMAC operation
 */
void syn_enable_dma_tx(struct nss_gmac_hal_dev *nghd)
{
	uint32_t data;

	data = hal_read_reg(nghd->mac_base, SYN_DMA_OPERATION_MODE);
	data |= SYN_DMA_TX_START;
	hal_write_reg(nghd->mac_base, SYN_DMA_OPERATION_MODE, data);
}

/*
 * syn_disable_dma_tx()
 *	Disable Rx GMAC operation
 */
void syn_disable_dma_tx(struct nss_gmac_hal_dev *nghd)
{
	uint32_t data;

	data = hal_read_reg(nghd->mac_base, SYN_DMA_OPERATION_MODE);
	data &= ~SYN_DMA_TX_START;
	hal_write_reg(nghd->mac_base, SYN_DMA_OPERATION_MODE, data);
}

/*
 * syn_resume_dma_tx
 *	Resumes the DMA Transmission.
 */
void syn_resume_dma_tx(struct nss_gmac_hal_dev *nghd)
{
	hal_write_reg(nghd->mac_base, SYN_DMA_TX_POLL_DEMAND, 0);
}

/*
 * syn_get_rx_missed
 *	Get Rx missed errors
 */
uint32_t syn_get_rx_missed(struct nss_gmac_hal_dev *nghd)
{
	uint32_t missed_frame_buff_overflow;
	missed_frame_buff_overflow = hal_read_reg(nghd->mac_base, SYN_DMA_MISSED_FRAME_AND_BUFF_OVERFLOW_COUNTER);
	return missed_frame_buff_overflow & 0xFFFF;
}

/*
 * syn_get_fifo_overflows
 *	Get FIFO overflows
 */
uint32_t syn_get_fifo_overflows(struct nss_gmac_hal_dev *nghd)
{
	uint32_t missed_frame_buff_overflow;
	missed_frame_buff_overflow = hal_read_reg(nghd->mac_base, SYN_DMA_MISSED_FRAME_AND_BUFF_OVERFLOW_COUNTER);
	return (missed_frame_buff_overflow >> 17) & 0x7ff;
}

/*
 * syn_init_tx_desc_base()
 *	Programs the Dma Tx Base address with the starting address of the descriptor ring or chain.
 */
void syn_init_tx_desc_base(struct nss_gmac_hal_dev *nghd, uint32_t tx_desc_dma)
{
	hal_write_reg(nghd->mac_base, SYN_DMA_TX_DESCRIPTOR_LIST_ADDRESS, tx_desc_dma);
}

/*
 * syn_init_rx_desc_base()
 *	Programs the Dma Rx Base address with the starting address of the descriptor ring or chain.
 */
void syn_init_rx_desc_base(struct nss_gmac_hal_dev *nghd, uint32_t rx_desc_dma)
{
	hal_write_reg(nghd->mac_base, SYN_DMA_RX_DESCRIPTOR_LIST_ADDRESS, rx_desc_dma);
}

/*
 * syn_dma_axi_bus_mode_init()
 *	Function to program DMA AXI bus mode register.
 */
static inline void syn_dma_axi_bus_mode_init(struct nss_gmac_hal_dev *nghd)
{
	hal_write_reg(nghd->mac_base, SYN_DMA_AXI_BUS_MODE,
					SYN_DMA_AXI_BUS_MODE_VAL);
}

/*
 * syn_dma_operation_mode_init()
 *	Function to program DMA Operation Mode register.
 */
static inline void syn_dma_operation_mode_init(struct nss_gmac_hal_dev *nghd)
{
	hal_write_reg(nghd->mac_base, SYN_DMA_OPERATION_MODE, SYN_DMA_OMR);
}

/*
 * syn_broadcast_enable()
 *	Enables Broadcast frames.
 *
 * When enabled Address filtering module passes all incoming broadcast frames.
 */
static inline void syn_broadcast_enable(struct nss_gmac_hal_dev *nghd)
{
	hal_clear_reg_bits(nghd, SYN_MAC_FRAME_FILTER, SYN_MAC_BROADCAST);
}

/*
 * syn_multicast_enable()
 *	Enables Multicast frames.
 *
 * When enabled all multicast frames are passed.
 */
static inline void syn_multicast_enable(struct nss_gmac_hal_dev *nghd)
{
	hal_set_reg_bits(nghd, SYN_MAC_FRAME_FILTER, SYN_MAC_MULTICAST_FILTER);
}

/*
 * syn_promisc_enable()
 *	Enables promiscous mode.
 *
 * When enabled Address filter modules pass all incoming frames
 * regardless of their Destination and source addresses.
 */
static inline void syn_promisc_enable(struct nss_gmac_hal_dev *nghd)
{
	hal_set_reg_bits(nghd, SYN_MAC_FRAME_FILTER, SYN_MAC_FILTER_OFF);
	hal_set_reg_bits(nghd, SYN_MAC_FRAME_FILTER,
				SYN_MAC_PROMISCUOUS_MODE_ON);
}

/*
 * syn_get_stats()
 */
static int syn_get_stats(struct nss_gmac_hal_dev *nghd)
{
	struct nss_dp_dev *dp_priv;
	struct syn_hal_dev *shd;
	struct nss_dp_gmac_stats *stats;

	BUG_ON(nghd == NULL);

	shd = (struct syn_hal_dev *)nghd;
	stats = &(shd->stats);

	dp_priv = netdev_priv(nghd->netdev);
	if (!dp_priv->data_plane_ops)
		return -1;

	dp_priv->data_plane_ops->get_stats(dp_priv->dpc, stats);

	return 0;
}

/*
 * syn_rx_flow_control()
 */
static void syn_rx_flow_control(struct nss_gmac_hal_dev *nghd,
				     bool enabled)
{
	BUG_ON(nghd == NULL);

	if (enabled)
		syn_set_rx_flow_ctrl(nghd);
	else
		syn_clear_rx_flow_ctrl(nghd);
}

/*
 * syn_tx_flow_control()
 */
static void syn_tx_flow_control(struct nss_gmac_hal_dev *nghd,
				     bool enabled)
{
	BUG_ON(nghd == NULL);

	if (enabled)
		syn_set_tx_flow_ctrl(nghd);
	else
		syn_clear_tx_flow_ctrl(nghd);
}

/*
 * syn_get_max_frame_size()
 */
static int32_t syn_get_max_frame_size(struct nss_gmac_hal_dev *nghd)
{
	int ret;
	uint32_t mtu;

	BUG_ON(nghd == NULL);

	ret = fal_port_max_frame_size_get(0, nghd->mac_id, &mtu);

	if (!ret)
		return mtu;

	return ret;
}

/*
 * syn_set_max_frame_size()
 */
static int32_t syn_set_max_frame_size(struct nss_gmac_hal_dev *nghd,
					uint32_t val)
{
	BUG_ON(nghd == NULL);

	return fal_port_max_frame_size_set(0, nghd->mac_id, val);
}

/*
 * syn_set_mac_speed()
 */
static int32_t syn_set_mac_speed(struct nss_gmac_hal_dev *nghd,
				   uint32_t mac_speed)
{
	struct net_device *netdev;
	BUG_ON(nghd == NULL);

	netdev = nghd->netdev;

	netdev_warn(netdev, "API deprecated\n");
	return 0;
}

/*
 * syn_get_mac_speed()
 */
static uint32_t syn_get_mac_speed(struct nss_gmac_hal_dev *nghd)
{
	struct net_device *netdev;
	BUG_ON(nghd == NULL);

	netdev = nghd->netdev;

	netdev_warn(netdev, "API deprecated\n");
	return 0;
}

/*
 * syn_set_duplex_mode()
 */
static void syn_set_duplex_mode(struct nss_gmac_hal_dev *nghd,
				uint8_t duplex_mode)
{
	struct net_device *netdev;
	BUG_ON(nghd == NULL);

	netdev = nghd->netdev;

	netdev_warn(netdev, "API deprecated\n");
}

/*
 * syn_get_duplex_mode()
 */
static uint8_t syn_get_duplex_mode(struct nss_gmac_hal_dev *nghd)
{
	struct net_device *netdev;
	BUG_ON(nghd == NULL);

	netdev = nghd->netdev;

	netdev_warn(netdev, "API deprecated\n");
	return 0;
}

/*
 * syn_get_netdev_stats()
 */
static int syn_get_netdev_stats(struct nss_gmac_hal_dev *nghd,
				struct rtnl_link_stats64 *stats)
{
	struct syn_hal_dev *shd;
	struct nss_dp_hal_gmac_stats *ndo_stats;

	BUG_ON(nghd == NULL);

	shd = (struct syn_hal_dev *)nghd;
	ndo_stats = &(shd->stats.stats);

	/*
	 * Read stats from the registered dataplane.
	 */
	if (syn_get_stats(nghd))
		return -1;

	stats->rx_packets = ndo_stats->rx_packets;
	stats->rx_bytes = ndo_stats->rx_bytes;
	stats->rx_errors = ndo_stats->rx_errors;
	stats->rx_dropped = ndo_stats->rx_errors;
	stats->rx_length_errors = ndo_stats->rx_length_errors;
	stats->rx_over_errors = ndo_stats->mmc_rx_overflow_errors;
	stats->rx_crc_errors = ndo_stats->mmc_rx_crc_errors;
	stats->rx_frame_errors = ndo_stats->rx_dribble_bit_errors;
	stats->rx_fifo_errors = ndo_stats->fifo_overflows;
	stats->rx_missed_errors = ndo_stats->rx_missed;
	stats->collisions = ndo_stats->tx_collisions + ndo_stats->rx_late_collision_errors;
	stats->tx_packets = ndo_stats->tx_packets;
	stats->tx_bytes = ndo_stats->tx_bytes;
	stats->tx_errors = ndo_stats->tx_errors;
	stats->tx_dropped = ndo_stats->tx_dropped;
	stats->tx_carrier_errors = ndo_stats->tx_loss_of_carrier_errors + ndo_stats->tx_no_carrier_errors;
	stats->tx_fifo_errors = ndo_stats->tx_underflow_errors;
	stats->tx_window_errors = ndo_stats->tx_late_collision_errors;

	return 0;
}

/*
 * syn_get_eth_stats()
 */
static int32_t syn_get_eth_stats(struct nss_gmac_hal_dev *nghd,
						uint64_t *data)
{
	struct syn_hal_dev *shd;
	struct nss_dp_gmac_stats *stats;
	uint8_t *p = NULL;
	int i;

	BUG_ON(nghd == NULL);

	shd = (struct syn_hal_dev *)nghd;
	stats = &(shd->stats);

	/*
	 * Read stats from the registered dataplane.
	 */
	if (syn_get_stats(nghd))
		return -1;

	for (i = 0; i < SYN_STATS_LEN; i++) {
		p = ((uint8_t *)(stats) +
				syn_gstrings_stats[i].stat_offset);
		data[i] = *(uint32_t *)p;
	}

	return 0;
}

/*
 * syn_get_strset_count()
 */
static int32_t syn_get_strset_count(struct nss_gmac_hal_dev *nghd,
							int32_t sset)
{
	struct net_device *netdev;
	BUG_ON(nghd == NULL);

	netdev = nghd->netdev;

	switch (sset) {
	case ETH_SS_STATS:
		return SYN_STATS_LEN;
	}

	netdev_dbg(netdev, "%s: Invalid string set\n", __func__);
	return -EPERM;
}

/*
 * syn_get_strings()
 */
static int32_t syn_get_strings(struct nss_gmac_hal_dev *nghd,
					int32_t stringset, uint8_t *data)
{
	struct net_device *netdev;
	int i;

	BUG_ON(nghd == NULL);

	netdev = nghd->netdev;

	switch (stringset) {
	case ETH_SS_STATS:
		for (i = 0; i < SYN_STATS_LEN; i++) {
			memcpy(data, syn_gstrings_stats[i].stat_string,
				ETH_GSTRING_LEN);
			data += ETH_GSTRING_LEN;
		}
		break;

	default:
		netdev_dbg(netdev, "%s: Invalid string set\n", __func__);
		return -EPERM;
	}

	return 0;
}

/*
 * syn_send_pause_frame()
 */
static void syn_send_pause_frame(struct nss_gmac_hal_dev *nghd)
{
	BUG_ON(nghd == NULL);

	syn_send_tx_pause_frame(nghd);
}

/*
 * syn_set_mac_address()
 */
static void syn_set_mac_address(struct nss_gmac_hal_dev *nghd,
							uint8_t *macaddr)
{
	uint32_t data;

	BUG_ON(nghd == NULL);

	if (!macaddr) {
		netdev_warn(nghd->netdev, "macaddr is not valid.\n");
		return;
	}

	data = (macaddr[5] << 8) | macaddr[4] | SYN_MAC_ADDR_HIGH_AE;
	hal_write_reg(nghd->mac_base, SYN_MAC_ADDR0_HIGH, data);
	data = (macaddr[3] << 24) | (macaddr[2] << 16) | (macaddr[1] << 8)
		| macaddr[0];
	hal_write_reg(nghd->mac_base, SYN_MAC_ADDR0_LOW, data);
}

/*
 * syn_get_mac_address()
 */
static void syn_get_mac_address(struct nss_gmac_hal_dev *nghd,
							uint8_t *macaddr)
{
	uint32_t data;

	BUG_ON(nghd == NULL);

	if (!macaddr) {
		netdev_warn(nghd->netdev, "macaddr is not valid.\n");
		return;
	}

	data = hal_read_reg(nghd->mac_base, SYN_MAC_ADDR0_HIGH);
	macaddr[5] = (data >> 8) & 0xff;
	macaddr[4] = (data) & 0xff;

	data = hal_read_reg(nghd->mac_base, SYN_MAC_ADDR0_LOW);
	macaddr[3] = (data >> 24) & 0xff;
	macaddr[2] = (data >> 16) & 0xff;
	macaddr[1] = (data >> 8) & 0xff;
	macaddr[0] = (data) & 0xff;
}

/*
 * syn_dma_init()
 *	Initialize settings for GMAC DMA and AXI bus.
 */
static void syn_dma_init(struct nss_gmac_hal_dev *nghd)
{
	struct net_device *ndev = nghd->netdev;
	struct nss_dp_dev *dp_priv = netdev_priv(ndev);

	/*
	 * Enable SoC specific GMAC clocks.
	 */
	nss_dp_hal_clk_enable(dp_priv);

	/*
	 * Configure DMA registers.
	 */
	syn_dma_bus_mode_init(nghd);
	syn_dma_axi_bus_mode_init(nghd);
	syn_dma_operation_mode_init(nghd);
}

/*
 * syn_init()
 */
static void *syn_init(struct gmac_hal_platform_data *gmacpdata)
{
	struct syn_hal_dev *shd = NULL;
	struct net_device *ndev = NULL;
	struct nss_dp_dev *dp_priv = NULL;
	struct resource *res;

	ndev = gmacpdata->netdev;
	dp_priv = netdev_priv(ndev);

	res = platform_get_resource(dp_priv->pdev, IORESOURCE_MEM, 0);
	if (!res) {
		netdev_dbg(ndev, "Resource get failed.\n");
		return NULL;
	}

	shd = (struct syn_hal_dev *)devm_kzalloc(&dp_priv->pdev->dev,
					sizeof(struct syn_hal_dev),
					GFP_KERNEL);
	if (!shd) {
		netdev_dbg(ndev, "kzalloc failed. Returning...\n");
		return NULL;
	}

	shd->nghd.mac_reg_len = resource_size(res);
	shd->nghd.memres = devm_request_mem_region(&dp_priv->pdev->dev,
								res->start,
								resource_size(res),
								ndev->name);
	if (!shd->nghd.memres) {
		netdev_dbg(ndev, "Request mem region failed. Returning...\n");
		devm_kfree(&dp_priv->pdev->dev, shd);
		return NULL;
	}

	/*
	 * Save netdev context in syn HAL context
	 */
	shd->nghd.netdev = gmacpdata->netdev;
	shd->nghd.mac_id = gmacpdata->macid;
	shd->nghd.duplex_mode = DUPLEX_FULL;

	set_bit(__NSS_DP_RXCSUM, &dp_priv->flags);

	/*
	 * Populate the mac base addresses
	 */
	shd->nghd.mac_base =
		devm_ioremap_nocache(&dp_priv->pdev->dev, res->start,
				     resource_size(res));
	if (!shd->nghd.mac_base) {
		netdev_dbg(ndev, "ioremap fail.\n");
		devm_kfree(&dp_priv->pdev->dev, shd);
		return NULL;
	}

	spin_lock_init(&shd->nghd.slock);

	netdev_dbg(ndev, "ioremap OK.Size 0x%x Ndev base 0x%lx macbase 0x%px\n",
			gmacpdata->reg_len,
			ndev->base_addr,
			shd->nghd.mac_base);

	syn_disable_interrupt_all(&shd->nghd);
	syn_dma_init(&shd->nghd);
	syn_ipc_offload_init(&shd->nghd);
	syn_promisc_enable(&shd->nghd);
	syn_broadcast_enable(&shd->nghd);
	syn_multicast_enable(&shd->nghd);
	syn_rx_enable(&shd->nghd);
	syn_tx_enable(&shd->nghd);

	/*
	 * Reset MIB Stats
	 */
	if (fal_mib_port_flush_counters(0, shd->nghd.mac_id)) {
		netdev_dbg(ndev, "MIB stats Reset fail.\n");
	}

	return (struct nss_gmac_hal_dev *)shd;
}

/*
 * syn_exit()
 */
static void syn_exit(struct nss_gmac_hal_dev *nghd)
{
	struct nss_dp_dev *dp_priv = NULL;

	dp_priv = netdev_priv(nghd->netdev);
	devm_iounmap(&dp_priv->pdev->dev,
			(void *)nghd->mac_base);
	devm_release_mem_region(&dp_priv->pdev->dev,
			(nghd->memres)->start,
			nghd->mac_reg_len);

	nghd->memres = NULL;
	nghd->mac_base = NULL;
}

struct nss_gmac_hal_ops syn_hal_ops = {
	.init = &syn_init,
	.start =  NULL,
	.stop = NULL,
	.exit = &syn_exit,
	.setmacaddr = &syn_set_mac_address,
	.getmacaddr = &syn_get_mac_address,
	.rxflowcontrol = &syn_rx_flow_control,
	.txflowcontrol = &syn_tx_flow_control,
	.setspeed = &syn_set_mac_speed,
	.getspeed = &syn_get_mac_speed,
	.setduplex = &syn_set_duplex_mode,
	.getduplex = &syn_get_duplex_mode,
	.setmaxframe = &syn_set_max_frame_size,
	.getmaxframe = &syn_get_max_frame_size,
	.getndostats = &syn_get_netdev_stats,
	.getssetcount = &syn_get_strset_count,
	.getstrings = &syn_get_strings,
	.getethtoolstats = &syn_get_eth_stats,
	.sendpause = &syn_send_pause_frame,
};

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

#define NSS_DP_HAL_MAX_PORTS			2
#define NSS_DP_HAL_CPU_NUM			2
#define NSS_DP_HAL_START_IFNUM			0
#define NSS_DP_GMAC_NORMAL_FRAME_MTU		1500
#define NSS_DP_GMAC_MINI_JUMBO_FRAME_MTU	1978
#define NSS_DP_GMAC_FULL_JUMBO_FRAME_MTU	9000
#define NSS_DP_HAL_MAX_MTU_SIZE			NSS_DP_GMAC_FULL_JUMBO_FRAME_MTU
#define NSS_DP_HAL_MAX_PACKET_LEN		65535

/*
 * TCSR_GMAC_AXI_CACHE_OVERRIDE register size
 */
#define TCSR_GMAC_AXI_CACHE_OVERRIDE_REG_SIZE	4

/*
 * TCSR_GMAC_AXI_CACHE_OVERRIDE Register offset
 */
#define TCSR_GMAC_AXI_CACHE_OVERRIDE_OFFSET	0x6224

/*
 * Value for TCSR_GMAC_AXI_CACHE_OVERRIDE register
 */
#define TCSR_GMAC_AXI_CACHE_OVERRIDE_VALUE	0x05050505

/*
 * GCC_SNOC_GMAC_AXI_CLOCK
 */
#define NSS_SNOC_GMAC_AXI_CLK			"nss-snoc-gmac-axi-clk"

/**
 * nss_dp_hal_gmac_stats
 *	The per-GMAC statistics structure.
 */
struct nss_dp_hal_gmac_stats {
	uint64_t rx_bytes;		/**< Number of RX bytes */
	uint64_t rx_packets;		/**< Number of RX packets */
	uint64_t rx_errors;		/**< Number of RX errors */
	uint64_t rx_receive_errors;	/**< Number of RX receive errors */
	uint64_t rx_descriptor_errors;	/**< Number of RX descriptor errors */
	uint64_t rx_late_collision_errors;
					/**< Number of RX late collision errors */
	uint64_t rx_dribble_bit_errors;	/**< Number of RX dribble bit errors */
	uint64_t rx_length_errors;	/**< Number of RX length errors */
	uint64_t rx_ip_header_errors;	/**< Number of RX IP header errors read from rxdec */
	uint64_t rx_ip_payload_errors;	/**< Number of RX IP payload errors */
	uint64_t rx_no_buffer_errors;	/**< Number of RX no-buffer errors */
	uint64_t rx_transport_csum_bypassed;
					/**< Number of RX packets where the transport checksum was bypassed */
	uint64_t tx_bytes;		/**< Number of TX bytes */
	uint64_t tx_packets;		/**< Number of TX packets */
	uint64_t tx_collisions;		/**< Number of TX collisions */
	uint64_t tx_errors;		/**< Number of TX errors */
	uint64_t tx_jabber_timeout_errors;
					/**< Number of TX jabber timeout errors */
	uint64_t tx_frame_flushed_errors;
					/**< Number of TX frame flushed errors */
	uint64_t tx_loss_of_carrier_errors;
					/**< Number of TX loss of carrier errors */
	uint64_t tx_no_carrier_errors;	/**< Number of TX no carrier errors */
	uint64_t tx_late_collision_errors;
					/**< Number of TX late collision errors */
	uint64_t tx_excessive_collision_errors;
					/**< Number of TX excessive collision errors */
	uint64_t tx_excessive_deferral_errors;
					/**< Number of TX excessive deferral errors */
	uint64_t tx_underflow_errors;	/**< Number of TX underflow errors */
	uint64_t tx_ip_header_errors;	/**< Number of TX IP header errors */
	uint64_t tx_ip_payload_errors;	/**< Number of TX IP payload errors */
	uint64_t tx_dropped;		/**< Number of TX dropped packets */
	uint64_t hw_errs[10];		/**< GMAC DMA error counters */
	uint64_t rx_missed;		/**< Number of RX packets missed by the DMA */
	uint64_t fifo_overflows;	/**< Number of RX FIFO overflows signalled by the DMA */
	uint64_t rx_scatter_errors;	/**< Number of scattered frames received by the DMA */
	uint64_t tx_ts_create_errors;	/**< Number of tx timestamp creation errors */
	uint64_t gmac_total_ticks;	/**< Total clock ticks spend inside the GMAC */
	uint64_t gmac_worst_case_ticks;	/**< Worst case iteration of the GMAC in ticks */
	uint64_t gmac_iterations;	/**< Number of iterations around the GMAC */
	uint64_t tx_pause_frames;	/**< Number of pause frames sent by the GMAC */
	uint64_t mmc_rx_overflow_errors;
					/**< Number of RX overflow errors */
	uint64_t mmc_rx_watchdog_timeout_errors;
					/**< Number of RX watchdog timeout errors */
	uint64_t mmc_rx_crc_errors;	/**< Number of RX CRC errors */
	uint64_t mmc_rx_ip_header_errors;
					/**< Number of RX IP header errors read from MMC counter*/
	uint64_t mmc_rx_octets_g;
					/**< Number of good octets received */
	uint64_t mmc_rx_ucast_frames;	/**< Number of Unicast frames received */
	uint64_t mmc_rx_bcast_frames;	/**< Number of Bcast frames received */
	uint64_t mmc_rx_mcast_frames;	/**< Number of Mcast frames received */
	uint64_t mmc_rx_undersize;
					/**< Number of RX undersize frames */
	uint64_t mmc_rx_oversize;
					/**< Number of RX oversize frames */
	uint64_t mmc_rx_jabber;		/**< Number of jabber frames */
	uint64_t mmc_rx_octets_gb;
					/**< Number of good/bad octets */
	uint64_t mmc_rx_frag_frames_g;	/**< Number of good ipv4 frag frames */
	uint64_t mmc_tx_octets_g;	/**< Number of good octets sent */
	uint64_t mmc_tx_ucast_frames;	/**< Number of Unicast frames sent*/
	uint64_t mmc_tx_bcast_frames;	/**< Number of Broadcast frames sent */
	uint64_t mmc_tx_mcast_frames;	/**< Number of Multicast frames sent */
	uint64_t mmc_tx_deferred;	/**< Number of Deferred frames sent */
	uint64_t mmc_tx_single_col;	/**< Number of single collisions */
	uint64_t mmc_tx_multiple_col;	/**< Number of multiple collisions */
	uint64_t mmc_tx_octets_gb;	/**< Number of good/bad octets sent*/
};

extern struct nss_dp_data_plane_ops nss_dp_gmac_ops;

#endif /* __NSS_DP_ARCH_H__ */

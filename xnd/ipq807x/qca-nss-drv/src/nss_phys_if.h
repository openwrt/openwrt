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

/**
 * nss_phys_if
 *	Physical interface message structure
 */

#ifndef __NSS_PHYS_IF_H
#define __NSS_PHYS_IF_H

/**
 * Physical IF
 */

/**
 * The NSS per-GMAC statistics sync structure.
 */
struct nss_phys_if_estats {
	uint32_t rx_errors;		/**< Number of RX errors */
	uint32_t rx_receive_errors;	/**< Number of RX receive errors */
	uint32_t rx_descriptor_errors;	/**< Number of RX descriptor errors */
	uint32_t rx_late_collision_errors;
					/**< Number of RX late collision errors */
	uint32_t rx_dribble_bit_errors;	/**< Number of RX dribble bit errors */
	uint32_t rx_length_errors;	/**< Number of RX length errors */
	uint32_t rx_ip_header_errors;	/**< Number of RX IP header errors */
	uint32_t rx_ip_payload_errors;	/**< Number of RX IP payload errors */
	uint32_t rx_no_buffer_errors;	/**< Number of RX no-buffer errors */
	uint32_t rx_transport_csum_bypassed;
					/**< Number of RX packets where the transport checksum was bypassed */
	uint32_t tx_collisions;		/**< Number of TX collisions */
	uint32_t tx_errors;		/**< Number of TX errors */
	uint32_t tx_jabber_timeout_errors;
					/**< Number of TX jabber timeout errors */
	uint32_t tx_frame_flushed_errors;
					/**< Number of TX frame flushed errors */
	uint32_t tx_loss_of_carrier_errors;
					/**< Number of TX loss of carrier errors */
	uint32_t tx_no_carrier_errors;	/**< Number of TX no carrier errors */
	uint32_t tx_late_collision_errors;
					/**< Number of TX late collision errors */
	uint32_t tx_excessive_collision_errors;
					/**< Number of TX excessive collision errors */
	uint32_t tx_excessive_deferral_errors;
					/**< Number of TX excessive deferral errors */
	uint32_t tx_underflow_errors;	/**< Number of TX underflow errors */
	uint32_t tx_ip_header_errors;	/**< Number of TX IP header errors */
	uint32_t tx_ip_payload_errors;	/**< Number of TX IP payload errors */
	uint32_t tx_dropped;		/**< Number of TX dropped packets */
	uint32_t hw_errs[10];		/**< GMAC DMA error counters */
	uint32_t rx_missed;		/**< Number of RX packets missed by the DMA */
	uint32_t fifo_overflows;	/**< Number of RX FIFO overflows signalled by the DMA */
	uint32_t rx_scatter_errors;	/**< Number of scattered frames received by the DMA */
	uint32_t tx_ts_create_errors;	/**< Number of tx timestamp creation errors */
	uint32_t gmac_total_ticks;	/**< Total clock ticks spend inside the GMAC */
	uint32_t gmac_worst_case_ticks;	/**< Worst case iteration of the GMAC in ticks */
	uint32_t gmac_iterations;	/**< Number of iterations around the GMAC */
	uint32_t tx_pause_frames;	/**< Number of pause frames sent by the GMAC */
	uint32_t mmc_rx_overflow_errors;
					/**< Number of RX overflow errors */
	uint32_t mmc_rx_watchdog_timeout_errors;
					/**< Number of RX watchdog timeout errors */
	uint32_t mmc_rx_crc_errors;	/**< Number of RX CRC errors */
	uint32_t mmc_rx_ip_header_errors;
					/**< Number of RX IP header errors */
	uint32_t mmc_rx_octets_g;		/* Number of good octets received */
	uint32_t mmc_rx_ucast_frames;	/* Number of Unicast frames received */
	uint32_t mmc_rx_bcast_frames;	/* Number of Bcast frames received */
	uint32_t mmc_rx_mcast_frames;	/* Number of Mcast frames received */
	uint32_t mmc_rx_undersize;	/* Number of RX undersize frames */
	uint32_t mmc_rx_oversize;	/* Number of RX oversize frames */
	uint32_t mmc_rx_jabber;		/* Number of jabber frames */
	uint32_t mmc_rx_octets_gb;	/* Number of good/bad octets */
	uint32_t mmc_rx_frag_frames_g;	/* Number of good ipv4 frag frames */
	uint32_t mmc_tx_octets_g;	/* Number of good octets sent */
	uint32_t mmc_tx_ucast_frames;	/* Number of Unicast frames sent*/
	uint32_t mmc_tx_bcast_frames;	/* Number of Broadcast frames sent */
	uint32_t mmc_tx_mcast_frames;	/* Number of Multicast frames sent */
	uint32_t mmc_tx_deferred;	/* Number of Deferred frames sent */
	uint32_t mmc_tx_single_col;	/* Number of single collisions */
	uint32_t mmc_tx_multiple_col;	/* Number of multiple collisions */
	uint32_t mmc_tx_octets_gb;	/* Number of good/bad octets sent*/
};

/**
 * The NSS GMAC statistics sync structure.
 */
struct nss_phys_if_stats {
	struct nss_cmn_node_stats if_stats;		/**< Generic interface stats */
	struct nss_phys_if_estats estats;	/**< Extended Statistics specific to GMAC */
};

/**
 * @brief Request/Response types
 */
enum nss_phys_if_msg_types {
	NSS_PHYS_IF_OPEN = NSS_IF_OPEN,
	NSS_PHYS_IF_CLOSE = NSS_IF_CLOSE,
	NSS_PHYS_IF_LINK_STATE_NOTIFY = NSS_IF_LINK_STATE_NOTIFY,
	NSS_PHYS_IF_MTU_CHANGE = NSS_IF_MTU_CHANGE,
	NSS_PHYS_IF_MAC_ADDR_SET = NSS_IF_MAC_ADDR_SET,
	NSS_PHYS_IF_STATS = NSS_IF_STATS,
	NSS_PHYS_IF_ISHAPER_ASSIGN = NSS_IF_ISHAPER_ASSIGN,
	NSS_PHYS_IF_BSHAPER_ASSIGN = NSS_IF_BSHAPER_ASSIGN,
	NSS_PHYS_IF_ISHAPER_UNASSIGN = NSS_IF_ISHAPER_UNASSIGN,
	NSS_PHYS_IF_BSHAPER_UNASSIGN = NSS_IF_BSHAPER_UNASSIGN,
	NSS_PHYS_IF_ISHAPER_CONFIG = NSS_IF_ISHAPER_CONFIG,
	NSS_PHYS_IF_BSHAPER_CONFIG = NSS_IF_BSHAPER_CONFIG,
	NSS_PHYS_IF_PAUSE_ON_OFF = NSS_IF_PAUSE_ON_OFF,
	NSS_PHYS_IF_VSI_ASSIGN = NSS_IF_VSI_ASSIGN,
	NSS_PHYS_IF_VSI_UNASSIGN = NSS_IF_VSI_UNASSIGN,
	NSS_PHYS_IF_SET_NEXTHOP = NSS_IF_SET_NEXTHOP,
	NSS_PHYS_IF_RESET_NEXTHOP = NSS_IF_RESET_NEXTHOP,
	NSS_PHYS_IF_EXTENDED_STATS_SYNC = NSS_IF_MAX_MSG_TYPES + 1,
	NSS_PHYS_IF_MAX_MSG_TYPES
};

/**
 * Message structure to send/receive physical interface commands
 *
 * NOTE: Do not adjust the location of if_msg relative to new
 * message types as it represents the base messages for all
 * intefaces.
 */
struct nss_phys_if_msg {
	struct nss_cmn_msg cm;				/**< Message Header */
	union {
		union nss_if_msgs if_msg;		/**< Interfaces messages */
		struct nss_phys_if_stats stats;		/**< Phys If Statistics */
	} msg;
};

/**
 * @brief Callback to receive physical interface messages
 *
 * @param app_data Application context for this message
 * @param msg NSS physical interface message
 *
 * @return void
 */
typedef void (*nss_phys_if_msg_callback_t)(void *app_data, struct nss_phys_if_msg *msg);

/**
 * @brief Callback to send physical interface data to the tranmsit path.
 *
 * @param netdev Net device
 * @param skb Data buffer
 *
 * @return void
 */
typedef void (*nss_phys_if_xmit_callback_t)(struct net_device *netdev, struct sk_buff *skb);

/**
 * @brief Callback to receive physical interface data
 * 	 TODO: Adjust to pass app_data as unknown to the
 * 	 list layer and netdev/sk as known.
 *
 * @param app_data Application context for this message
 * @param os_buf Data buffer
 *
 * @return void
 */
typedef void (*nss_phys_if_rx_callback_t)(struct net_device *netdev, struct sk_buff *skb, struct napi_struct *napi);

/**
 * @brief Callback to recieve extended data plane packet on interface.
 *
 * @param app_data Application context for this message
 * @param skb Data buffer
 * @param napi napi pointer
 *
 * @return void
 */
typedef void (*nss_phys_if_rx_ext_data_callback_t)(struct net_device *netdev, struct sk_buff *skb, struct napi_struct *napi);

/**
 * @brief Register to send/receive GMAC packets/messages
 *
 * @param if_num GMAC i/f number
 * @param rx_callback Receive callback for packets
 * @param event_callback Receive callback for events
 * @param netdev netdevice associated with this interface.
 * @param features denote the skb types supported by this interface
 *
 * @return void* NSS context
 */
struct nss_ctx_instance *nss_phys_if_register(uint32_t if_num,
					nss_phys_if_rx_callback_t rx_callback,
					nss_phys_if_msg_callback_t msg_callback,
					struct net_device *netdev,
					uint32_t features);

/**
 * @brief Send GMAC packet
 *
 * @param nss_ctx NSS context
 * @param os_buf OS buffer (e.g. skbuff)
 * @param if_num GMAC i/f number
 *
 * @return nss_tx_status_t Tx status
 */
nss_tx_status_t nss_phys_if_buf(struct nss_ctx_instance *nss_ctx, struct sk_buff *os_buf, uint32_t if_num);

/**
 * @brief Send message to physical interface
 *
 * @param nim Physical interface message
 *
 * @return command Tx status
 */
nss_tx_status_t nss_phys_if_msg(struct nss_ctx_instance *nss_ctx, struct nss_phys_if_msg *nim);

/**
 * @brief Send a message to physical interface & wait for the response.
 *
 * @param nim Physical interface message
 *
 * @return command Tx status
 */
nss_tx_status_t nss_phys_if_msg_sync(struct nss_ctx_instance *nss_ctx, struct nss_phys_if_msg *nim);

/**
 * @brief Open GMAC interface on NSS
 *
 * @param nss_ctx NSS context
 * @param tx_desc_ring Tx descriptor ring address
 * @param rx_desc_ring Rx descriptor ring address
 * @param if_num GMAC i/f number
 * @param bypass_nw_process network processing in nss is bypassed for GMAC
 *
 * @return nss_tx_status_t Tx status
 */
nss_tx_status_t nss_phys_if_open(struct nss_ctx_instance *nss_ctx, uint32_t tx_desc_ring, uint32_t rx_desc_ring, uint32_t mode, uint32_t if_num,
								uint32_t bypass_nw_process);

/**
 * @brief Close GMAC interface on NSS
 *
 * @param nss_ctx NSS context
 * @param if_num GMAC i/f number
 *
 * @return nss_tx_status_t Tx status
 */
nss_tx_status_t nss_phys_if_close(struct nss_ctx_instance *nss_ctx, uint32_t if_num);

/**
 * @brief Send link state message to NSS
 *
 * @param nss_ctx NSS context
 * @param link_state Link state
 * @param if_num GMAC i/f number
 *
 * @return nss_tx_status_t Tx status
 */
nss_tx_status_t nss_phys_if_link_state(struct nss_ctx_instance *nss_ctx, uint32_t link_state, uint32_t if_num);

/**
 * @brief Send MAC address to NSS
 *
 * @param nss_ctx NSS context
 * @param addr MAC address pointer
 * @param if_num GMAC i/f number
 *
 * @return nss_tx_status_t Tx status
 */
nss_tx_status_t nss_phys_if_mac_addr(struct nss_ctx_instance *nss_ctx, uint8_t *addr, uint32_t if_num);

/**
 * @brief Send MTU change notification to NSS
 *
 * @param nss_ctx NSS context
 * @param mtu MTU
 * @param if_num GMAC i/f number
 *
 * @return nss_tx_status_t Tx status
 */
nss_tx_status_t nss_phys_if_change_mtu(struct nss_ctx_instance *nss_ctx, uint32_t mtu, uint32_t if_num);

/**
 * @brief Send vsi assign to NSS
 *
 * @param nss_ctx NSS context
 * @param vsi VSI number
 * @param if_num GMAC i/f number
 *
 * @return nss_tx_status_t Tx status
 */
nss_tx_status_t nss_phys_if_vsi_assign(struct nss_ctx_instance *nss_ctx, uint32_t vsi, uint32_t if_num);

/**
 * @brief Send vsi unassign to NSS
 *
 * @param nss_ctx NSS context
 * @param vsi VSI number
 * @param if_num GMAC i/f number
 *
 * @return nss_tx_status_t Tx status
 */
nss_tx_status_t nss_phys_if_vsi_unassign(struct nss_ctx_instance *nss_ctx, uint32_t vsi, uint32_t if_num);

/**
 * @brief Send pause frame enabled notification to NSS
 *
 * @param nss_ctx NSS context
 * @param pause_on Pause on or off
 * @param if_num GMAC i/f number
 *
 * @return nss_tx_status_t Tx status
 */
nss_tx_status_t nss_phys_if_pause_on_off(struct nss_ctx_instance *nss_ctx, uint32_t pause_on, uint32_t if_num);

#endif /* __NSS_PHYS_IF_H */

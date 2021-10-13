/*
 **************************************************************************
 * Copyright (c) 2014-2015, 2017-2021, The Linux Foundation. All rights reserved.
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
 * @file nss_gre_redir.h
 *	NSS GRE Redirect interface definitions.
 */

#ifndef __NSS_GRE_REDIR_H
#define __NSS_GRE_REDIR_H

/**
 * @addtogroup nss_gre_redirect_subsystem
 * @{
 */

#define NSS_GRE_REDIR_MAX_INTERFACES 24			/**< Maximum number of redirect interfaces. */
#define NSS_GRE_REDIR_IP_DF_OVERRIDE_FLAG 0x80		/**< Override Do not Fragment bit in IPv4 flags. */
#define NSS_GRE_REDIR_PER_PACKET_METADATA_OFFSET 4	/**< Offset of per packet metadata from start of packet. */
#define NSS_GRE_REDIR_MAX_RADIO 5			/**< Maximum number of radios. */
#define NSS_GRE_REDIR_HEADER_VERSION 0			/**< Version for GRE header. */

/**
 * nss_gre_redir_ip_hdr_type
 *	IP header types.
 */
enum nss_gre_redir_ip_hdr_type {
	NSS_GRE_REDIR_IP_HDR_TYPE_IPV4 = 1,
	NSS_GRE_REDIR_IP_HDR_TYPE_IPV6 = 2,
};

/**
 * nss_gre_redir_message_types
 *	Message types for GRE redirect requests and responses.
 */
enum nss_gre_redir_message_types {
	NSS_GRE_REDIR_TX_TUNNEL_INNER_CONFIGURE_MSG,	/**< Configure message for inner node. */
	NSS_GRE_REDIR_TX_TUNNEL_OUTER_CONFIGURE_MSG,	/**< Configure message for outer node. */
	NSS_GRE_REDIR_TX_INTERFACE_MAP_MSG,		/**< Interface map message. */
	NSS_GRE_REDIR_TX_INTERFACE_UNMAP_MSG,		/**< Interface unmap message. */
	NSS_GRE_REDIR_TX_SJACK_MAP_MSG,			/**< SJACK map message. */
	NSS_GRE_REDIR_TX_SJACK_UNMAP_MSG,		/**< SJACK unmap message. */
	NSS_GRE_REDIR_RX_STATS_SYNC_MSG,		/**< Statistics synchronization message. */
	NSS_GRE_REDIR_EXCEPTION_DS_REG_CB_MSG,		/**< Configure message to register callback. */
	NSS_GRE_REDIR_MAX_MSG_TYPES,			/**< Maximum message type. */
};

/**
 * nss_gre_redir_error_types
 *	Error types for GRE redirect configuration messages.
 */
enum nss_gre_redir_error_types {
	NSS_GRE_REDIR_ERROR_NONE,			/**< Configuration successful. */
	NSS_GRE_REDIR_ERROR_UNKNOWN_MSG_TYPE,		/**< Unknown configuration message type error. */
	NSS_GRE_REDIR_ERROR_INVALID_IP_HDR_TYPE,	/**< Invalid IP header type error. */
	NSS_GRE_REDIR_ERROR_MAP_TABLE_FULL,		/**< Map table full error. */
	NSS_GRE_REDIR_ERROR_MAP_INVALID_PARAM,		/**< Invalid parameter with map message error. */
	NSS_GRE_REDIR_ERROR_UNMAP_INVALID_PARAM,	/**< Invalid parameter with unmap message error. */
	NSS_GRE_REDIR_ERROR_ENCAP_MAP_EXIST,		/**< Encapsulation map entry already exist. */
	NSS_GRE_REDIR_ERROR_DECAP_MAP_EXIST,		/**< Decapsulation map entry already exist. */
	NSS_GRE_REDIR_ERROR_ENCAP_MAP_ALLOC_FAIL,	/**< Encapsulation map entry allocation failure error. */
	NSS_GRE_REDIR_ERROR_DECAP_MAP_ALLOC_FAIL,	/**< Decapsulation map entry allocation failure error. */
	NSS_GRE_REDIR_ERROR_ENCAP_ENTRY_UNMAPPED,	/**< Encapsulation map entry already unmapped. */
	NSS_GRE_REDIR_ERROR_DECAP_ENTRY_UNMAPPED,	/**< Decapsulation map entry already unmapped. */
	NSS_GRE_REDIR_ERROR_INVALID_ETH_IF,		/**< Invalid Ethernet NSS interface. */
	NSS_GRE_REDIR_ERROR_INVALID_VAP_NEXTHOP_IF,	/**< Invalid nexthop NSS interface. */
	NSS_GRE_REDIR_ERROR_INVALID_PEER_INTERFACE,	/**< Invalid peer interface during tunnel configuration. */
};

/**
 * nss_gre_redir_tunnel_types
 *	GRE tunnel types.
 */
enum nss_gre_redir_tunnel_types {
	NSS_GRE_REDIR_TUNNEL_TYPE_UNKNOWN,	/**< Reserved. */
	NSS_GRE_REDIR_TUNNEL_TYPE_TUN,		/**< Tunnel mode. */
	NSS_GRE_REDIR_TUNNEL_TYPE_DTUN,		/**< D-tunnel mode. */
	NSS_GRE_REDIR_TUNNEL_TYPE_SPLIT,	/**< Split mode. */
	NSS_GRE_REDIR_TUNNEL_TYPE_MAX,		/**< Maximum tunnel type. */
};

/**
 * nss_gre_redir_stats_types
 *	GRE redirect statistics types.
 */
enum nss_gre_redir_stats_types {
	NSS_GRE_REDIR_STATS_TX_DROPS = NSS_STATS_NODE_MAX,
						/**< Dropped transmit packets. */
	NSS_GRE_REDIR_STATS_SJACK_RX_PKTS,	/**< SJACK receive packet counter. */
	NSS_GRE_REDIR_STATS_SJACK_TX_PKTS,	/**< SJACK transmit packet counter. */
	NSS_GRE_REDIR_STATS_OFFLOAD_RX_PKTS_0,	/**< Offload receive packet counter 0. */
	NSS_GRE_REDIR_STATS_OFFLOAD_RX_PKTS_1,	/**< Offload receive packet counter 1. */
	NSS_GRE_REDIR_STATS_OFFLOAD_RX_PKTS_2,	/**< Offload receive packet counter 2. */
	NSS_GRE_REDIR_STATS_OFFLOAD_RX_PKTS_3,	/**< Offload receive packet counter 3. */
	NSS_GRE_REDIR_STATS_OFFLOAD_RX_PKTS_4,	/**< Offload receive packet counter 4. */
	NSS_GRE_REDIR_STATS_OFFLOAD_TX_PKTS_0,	/**< Offload transmit packet counter 0. */
	NSS_GRE_REDIR_STATS_OFFLOAD_TX_PKTS_1,	/**< Offload transmit packet counter 1. */
	NSS_GRE_REDIR_STATS_OFFLOAD_TX_PKTS_2,	/**< Offload transmit packet counter 2. */
	NSS_GRE_REDIR_STATS_OFFLOAD_TX_PKTS_3,	/**< Offload transmit packet counter 3. */
	NSS_GRE_REDIR_STATS_OFFLOAD_TX_PKTS_4,	/**< Offload transmit packet counter 4. */
	NSS_GRE_REDIR_STATS_EXCEPTION_US_RX_PKTS,
						/**< Upstream exception receive packet counter. */
	NSS_GRE_REDIR_STATS_EXCEPTION_US_TX_PKTS,
						/**< Upstream exception transmit packet counter. */
	NSS_GRE_REDIR_STATS_EXCEPTION_DS_RX_PKTS,
						/**< Downstream exception receive packet counter. */
	NSS_GRE_REDIR_STATS_EXCEPTION_DS_TX_PKTS,
						/**< Downstream exception transmit packet counter. */
	NSS_GRE_REDIR_STATS_ENCAP_SG_ALLOC_DROP,
						/**< Encapsulation drop counters due to scatter gather buffer allocation failure. */
	NSS_GRE_REDIR_STATS_DECAP_FAIL_DROP,
						/**< Decapsulation drop counters due to invalid IP header. */
	NSS_GRE_REDIR_STATS_DECAP_SPLIT_DROP,
						/**< Decapsulation drop counters due to split flow processing. */
	NSS_GRE_REDIR_STATS_SPLIT_SG_ALLOC_FAIL,
						/**< Split processing fail counter due to scatter gather buffer allocation failure. */
	NSS_GRE_REDIR_STATS_SPLIT_LINEAR_COPY_FAIL,
						/**< Split processing fail counter due to linear copy fail. */
	NSS_GRE_REDIR_STATS_SPLIT_NOT_ENOUGH_TAILROOM,
						/**< Split processing fail counter due to insufficient tailroom. */
	NSS_GRE_REDIR_STATS_EXCEPTION_DS_INVALID_DST_DROP,
						/**< Downstream exception handling fail counter due to invalid destination. */
	NSS_GRE_REDIR_STATS_DECAP_EAPOL_FRAMES,
						/**< Decapsulation EAPoL frame counters. */
	NSS_GRE_REDIR_STATS_EXCEPTION_DS_INV_APPID,
						/**< Invalid application ID for the transmit completion packets on exception downstream node. */
	NSS_GRE_REDIR_STATS_HEADROOM_UNAVAILABLE,
						/**< Packet headroom unavailable to write metadata. */
	NSS_GRE_REDIR_STATS_TX_COMPLETION_SUCCESS,
						/**< Host enqueue success count for the transmit completion packets. */
	NSS_GRE_REDIR_STATS_TX_COMPLETION_DROP,
						/**< Host enqueue drop count for the transmit completion packets. */
	NSS_GRE_REDIR_STATS_MAX			/**< Maximum statistics type. */
};

/**
 * nss_gre_redir_inner_configure_msg
 *	Message information for configuring GRE inner node.
 */
struct nss_gre_redir_inner_configure_msg {
	uint32_t ip_hdr_type;	/**< IP header type (IPv4 or IPv6). */

	/**
	 * IPv4 or IPv6 source address (lower 4 bytes are applicable for IPv4).
	 */
	uint32_t ip_src_addr[4];

	/**
	 * IPv4 or IPv6 destination address (lower 4 bytes are applicable for IPv4).
	 */
	uint32_t ip_dest_addr[4];

	/**
	 * The host outer-interface which handles post-encapsulation exception packets
	 * originating from this inner interface.
	 */
	uint32_t except_outerif;

	uint8_t ip_df_policy;	/**< Default Do Not Fragment policy for the IP header. */
	uint8_t ip_ttl;		/**< Time-to-live value for the IP header. */
	uint8_t gre_version;	/**< Header version. */
};

/**
 * nss_gre_redir_outer_configure_msg
 *	Message information for configuring GRE outer node.
 */
struct nss_gre_redir_outer_configure_msg {
	uint32_t ip_hdr_type;		     /**< IP header type (IPv4 or IPv6). */

	/**
	 * The host inner-interface which handles post-decapsulation exception packets
	 * originating from this outer interface, for flows destined to a VAP handled
	 * by host.
	 */
	uint32_t except_hostif;

	/**
	 * The host inner-interface which handles post-decapsulation exception packets
	 * originating from this outer interface, for flows destined to a VAP handled
	 * by NSS.
	 */
	uint32_t except_offlif;

	/**
	 * The host inner-interface which handles post-decapsulation exception packets
	 * originating from this outer interface, for flows destined to SJACK.
	 */
	uint32_t except_sjackif;

	/**
	 * CPU core to which these packets should be steered.
	 * - 0 -- Use core 0
	 * - 1 -- Use core 1
	 * - 2 -- Use core 2
	 * - 3 -- Use core 3
	 */
	uint8_t rps_hint;

	/**
	 * Flag to indicate validity of RPS hint.
	 */
	uint8_t rps_hint_valid;

};

/**
 * nss_gre_redir_exception_ds_reg_cb_msg
 *	Message information to register callback on VAP for GRE exception downstream.
 */
struct nss_gre_redir_exception_ds_reg_cb_msg {
	uint32_t dst_vap_nssif;	/**< NSS VAP interface on which the callback is registered. */
};

/**
 * nss_gre_redir_interface_map_msg
 *	Message information for adding a VAP interface-to-tunnel ID mapping.
 */
struct nss_gre_redir_interface_map_msg {
	uint32_t vap_nssif;			/**< NSS VAP interface. */
	uint32_t nexthop_nssif;			/**< Next hop NSS interface number. */
	uint16_t radio_id;			/**< Radio ID to derive tunnel ID. */
	uint16_t vap_id;			/**< VAP ID to derive tunnel ID. */
	uint16_t lag_en;			/**< Flag for LAG mode. */
	uint16_t tunnel_type;			/**< Type of tunnel. */

	/**
	 * IPsec security association pattern. Pattern
	 * 0x5A is supported only.
	 */
	uint8_t ipsec_pattern;
};

/**
 * nss_gre_redir_interface_unmap_msg
 *	Message information for deleting a VAP interface-to-tunnel ID mapping.
 */
struct nss_gre_redir_interface_unmap_msg {
	uint32_t vap_nssif;	/**< NSS VAP interface. */
	uint16_t radio_id;	/**< Radio ID to derive tunnel ID. */
	uint16_t vap_id;	/**< VAP ID to derive tunnel ID. */
};

/**
 * nss_gre_redir_sjack_map_msg
 *	Message information for adding an Ethernet interface-to-tunnel ID mapping.
 */
struct nss_gre_redir_sjack_map_msg {
	uint32_t eth_nssif;			/**< NSS Ethernet interface number. */
	uint32_t eth_id;			/**< Ethernet interface ID. */

	/**
	 * IPsec security association pattern. Pattern
	 * 0x5A is supported only.
	 */
	uint8_t ipsec_pattern;
};

/**
 * nss_gre_redir_sjack_unmap_msg
 *	Message information for deleting an Ethernet interface-to-tunnel ID mapping.
 */
struct nss_gre_redir_sjack_unmap_msg {
	uint32_t eth_nssif;	/**< NSS Ethernet interface number. */
	uint32_t eth_id;	/**< Ethernet interface ID. */
};

/**
 * nss_gre_redir_stats_sync_msg
 *	Message information for synchronized GRE redirect statistics.
 */
struct nss_gre_redir_stats_sync_msg {
	struct nss_cmn_node_stats node_stats;		/**< Common node statistics. */
	uint32_t sjack_rx_packets;			/**< SJACK packet counter. */
	uint32_t offl_rx_pkts[NSS_GRE_REDIR_MAX_RADIO];	/**< Offload packet counter. */
	uint32_t encap_sg_alloc_drop;			/**< Encapsulation drop counters due to scatter gather buffer allocation failure. */
	uint32_t decap_fail_drop;			/**< Decapsulation drop counters due to invalid IP header. */
	uint32_t decap_split_drop;			/**< Decapsulation drop counters due to split flow processing. */
	uint32_t split_sg_alloc_fail;			/**< Split processing fail counter due to scatter gather buffer allocation failure. */
	uint32_t split_linear_copy_fail;		/**< Split processing fail counter due to linear copy fail. */
	uint32_t split_not_enough_tailroom;		/**< Split processing fail counter due to insufficient tailroom. */
	uint32_t exception_ds_invalid_dst_drop;		/**< Downstream exception handling fail counter due to invalid destination. */
	uint32_t decap_eapol_frames;			/**< Decapsulation EAPoL frame counters. */
	uint32_t exception_ds_inv_appid;		/**< Invalid application ID for the Tx completion packets on exception downstream node. */
	uint32_t headroom_unavail;			/**< Packet headroom unavailable to write metadata. */
	uint32_t tx_completion_success;			/**< Host enqueue success count for the Tx completion packets. */
	uint32_t tx_completion_drop;			/**< Host enqueue drop count for the Tx completion packets. */
};

/**
 * nss_gre_redir_tun_stats
 *	GRE redirect statistics to accumulate all the stats values.
 */
struct nss_gre_redir_tun_stats {
	uint64_t rx_packets;		/**< Number of packets received. */
	uint64_t rx_bytes;		/**< Number of bytes received. */
	uint64_t tx_packets;		/**< Number of packets transmitted. */
	uint64_t tx_bytes;		/**< Number of bytes transmitted. */
	uint64_t rx_dropped[NSS_MAX_NUM_PRI];
					/**< Packets dropped on receive due to queue full. */
	uint64_t tx_dropped;		/**< Dropped transmit packets. */
	uint64_t sjack_rx_packets;	/**< SJACK receive packet counter. */
	uint64_t sjack_tx_packets;	/**< SJACK transmit packet counter. */
	uint64_t offl_rx_pkts[NSS_GRE_REDIR_MAX_RADIO];	/**< Offload receive packet counter per radio. */
	uint64_t offl_tx_pkts[NSS_GRE_REDIR_MAX_RADIO];	/**< Offload transmit packet counter per radio. */
	uint64_t exception_us_rx;	/**< Upstream exception receive packet counter. */
	uint64_t exception_us_tx;	/**< Upstream exception transmit packet counter. */
	uint64_t exception_ds_rx;	/**< Downstream exception receive packet counter. */
	uint64_t exception_ds_tx;	/**< Downstream exception transmit packet counter. */
	uint64_t encap_sg_alloc_drop;
					/**< Encapsulation drop counters due to scatter gather buffer allocation failure. */
	uint64_t decap_fail_drop;	/**< Decapsulation drop counters due to invalid IP header. */
	uint64_t decap_split_drop;	/**< Decapsulation drop counters due to split flow processing. */
	uint64_t split_sg_alloc_fail;
					/**< Split processing fail counter due to scatter gather buffer allocation failure. */
	uint64_t split_linear_copy_fail;
					/**< Split processing fail counter due to linear copy fail. */
	uint64_t split_not_enough_tailroom;
					/**< Split processing fail counter due to insufficient tailroom. */
	uint64_t exception_ds_invalid_dst_drop;
					/**< Downstream exception handling fail counter due to invalid destination. */
	uint64_t decap_eapol_frames;	/**< Decapsulation EAPoL frame counters. */
	uint64_t exception_ds_inv_appid;
					/**< Invalid application ID for the transmit completion packets on exception downstream node. */
	uint64_t headroom_unavail;	/**< Packet headroom unavailable to write metadata. */
	uint64_t tx_completion_success;	/**< Host enqueue success count for the transmit completion packets. */
	uint64_t tx_completion_drop;	/**< Host enqueue drop count for the transmit completion packets. */
};

/**
 * nss_gre_redir_tunnel_stats
 *	GRE redirect statistics as seen by the HLOS.
 */
struct nss_gre_redir_tunnel_stats {
	struct net_device *dev;			/**< Net device. */
	struct nss_gre_redir_tun_stats tstats;	/**< Structure to accumulate all the statistics. */
	uint32_t ref_count;			/**< Reference count for statistics. */
};

/**
 * nss_gre_redir_stats_notification
 *	GRE redirect transmission statistics structure.
 */
struct nss_gre_redir_stats_notification {
	struct nss_gre_redir_tunnel_stats stats_ctx;	/**< Context transmission statistics. */
	uint32_t core_id;				/**< Core ID. */
	uint32_t if_num;				/**< Interface number. */
};

/**
 * nss_gre_redir_msg
 *	Data for sending and receiving GRE tunnel redirect messages.
 */
struct nss_gre_redir_msg {
	struct nss_cmn_msg cm;	/**< Common message header. */

	/**
	 * Payload of a GRE tunnel redirect message.
	 */
	union {
		struct nss_gre_redir_inner_configure_msg inner_configure;
				/**< Configure a GRE inner node. */
		struct nss_gre_redir_outer_configure_msg outer_configure;
				/**< Configure a GRE outer node. */
		struct nss_gre_redir_interface_map_msg interface_map;
				/**< Add a VAP interface-to-tunnel ID mapping. */
		struct nss_gre_redir_interface_unmap_msg interface_unmap;
				/**< Delete a VAP interafce-to-tunnel ID mapping. */
		struct nss_gre_redir_sjack_map_msg sjack_map;
				/**< Add an Ethernet interface-to-tunnel ID mapping for SJACK. */
		struct nss_gre_redir_sjack_unmap_msg sjack_unmap;
				/**< Delete an Ethernet interface-to-tunnel ID mapping for SJACK. */
		struct nss_gre_redir_stats_sync_msg stats_sync;
				/**< Synchronized tunnel statistics. */
		struct nss_gre_redir_exception_ds_reg_cb_msg exception_ds_configure;
				/**< Registering callback on VAP for the GRE downstream flows. */
	} msg;			/**< Message payload for GRE redirect messages exchanged with NSS core. */

};

/**
 * nss_gre_redir_encap_per_pkt_metadata
 *	Metadata information for an HLOS-to-NSS packet.
 */
struct nss_gre_redir_encap_per_pkt_metadata {
	uint16_t gre_tunnel_id;			/**< ID of the tunnel. */
	uint8_t gre_flags;			/**< Flags field from GRE header. */
	uint8_t gre_prio;			/**< Priority field from GRE header. */
	uint8_t gre_seq;			/**< Sequence number. */
	uint8_t ip_dscp;			/**< DSCP values. */

	/**
	 * Override the default DF policy for the packet by setting bit 8.
	 * The lower 7 bits provide the DF value to be used for this packet.
	 */
	uint8_t ip_df_override;

	/**
	 * IPsec security association pattern. Pattern
	 * 0x5A is supported only.
	 */
	uint8_t ipsec_pattern;
};

/**
 * nss_gre_redir_decap_per_pkt_metadata
 *	Metadata information for an NSS-to-HLOS packet.
 */
struct nss_gre_redir_decap_per_pkt_metadata {
	uint32_t src_if_num;    /**< Number of the source Ethernet interface. */
	uint16_t gre_tunnel_id; /**< ID of the tunnel. */
	uint8_t gre_flags;	/**< Flags from GRE header. */
	uint8_t gre_prio;	/**< Priority from GRE header. */
	uint8_t gre_seq;	/**< Sequence number. */
};

/**
 * nss_gre_redir_exception_us_metadata
 *	Metadata information for upstream exception packet.
 *
 * Note: Additional fields need to be added by customer as required.
 */
struct nss_gre_redir_exception_us_metadata {
	uint8_t tid;		/**< TID value. */
};

/**
 * nss_gre_redir_exception_ds_metadata
 *	Metadata information for downstream exception packet.
 *
 * Note: Additional fields need to be added by customer as required.
 */
struct nss_gre_redir_exception_ds_metadata {
	uint32_t dst_vap_nssif;	/**< Destination VAP interface number. */
	uint8_t tid;		/**< TID value. */
	uint8_t app_id;		/**< Application ID. */
	uint16_t hw_hash_idx;	/**< Hardware AST hash index value. */
	uint32_t tx_status;	/**< Tx status. */
};

/**
 * Callback function for receiving GRE tunnel data.
 *
 * @datatypes
 * net_device \n
 * sk_buff \n
 * napi_struct
 *
 * @param[in] netdev  Pointer to the associated network device.
 * @param[in] skb     Pointer to the data socket buffer.
 * @param[in] napi    Pointer to the NAPI structure.
 */
typedef void (*nss_gre_redir_data_callback_t)(struct net_device *netdev, struct sk_buff *skb, struct napi_struct *napi);

/**
 * Callback function for receiving GRE tunnel messages.
 *
 * @datatypes
 * nss_cmn_msg
 *
 * @param[in] app_data  Pointer to the application context of the message.
 * @param[in] msg       Pointer to the message data.
 */
typedef void (*nss_gre_redir_msg_callback_t)(void *app_data, struct nss_cmn_msg *msg);

/**
 * nss_gre_redir_unregister_if
 *	Deregisters a GRE tunnel interface from the NSS.
 *
 * @param[in] if_num  NSS interface number.
. *
 * @return
 * None.
 *
 * @dependencies
 * The tunnel interface must have been previously registered.
 *
 * @return
 * True if successful, else false.
 */
extern bool nss_gre_redir_unregister_if(uint32_t if_num);

/**
 * nss_gre_redir_tx_msg
 *	Sends GRE redirect tunnel messages.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_gre_redir_msg
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 * @param[in] msg      Pointer to the message data.
 *
 * @return
 * Status of the Tx operation.
 */
extern nss_tx_status_t nss_gre_redir_tx_msg(struct nss_ctx_instance *nss_ctx, struct nss_gre_redir_msg *msg);

/**
 * nss_gre_redir_tx_buf
 *	Sends GRE redirect tunnel packets.
 *
 * @datatypes
 * nss_ctx_instance \n
 * sk_buff
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 * @param[in] os_buf   Pointer to the OS buffer (e.g., skbuff).
 * @param[in] if_num   Tunnel interface number.
 *
 * @return
 * Status of the Tx operation.
 */
extern nss_tx_status_t nss_gre_redir_tx_buf(struct nss_ctx_instance *nss_ctx, struct sk_buff *os_buf,
		uint32_t if_num);

/**
 * nss_gre_redir_tx_buf_noreuse
 *	Sends GRE redirect tunnel packets.
 *
 * Note: The buffers will be not be reused or
 * kept in the accelerator.
 *
 * @datatypes
 * nss_ctx_instance \n
 * sk_buff
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 * @param[in] os_buf   Pointer to the OS buffer (e.g., skbuff).
 * @param[in] if_num   Tunnel interface number.
 *
 * @return
 * Status of the Tx operation.
 */
extern nss_tx_status_t nss_gre_redir_tx_buf_noreuse(struct nss_ctx_instance *nss_ctx, struct sk_buff *os_buf,
			uint32_t if_num);

/**
 * nss_gre_redir_stats_get
 *	Gets GRE redirect tunnel statistics.
 *
 * @datatypes
 * nss_gre_redir_tunnel_stats
 *
 * @param[in]  index  Index in the tunnel statistics array.
 * @param[out] stats  Pointer to the tunnel statistics.
 *
 * @return
 * TRUE or FALSE.
 */
extern bool nss_gre_redir_stats_get(int index, struct nss_gre_redir_tunnel_stats *stats);

/**
 * nss_gre_redir_alloc_and_register_node
 *	Allocates and registers GRE redirect dynamic node with NSS.
 *
 * @datatypes
 * net_device \n
 * nss_gre_redir_data_callback_t \n
 * nss_gre_redir_msg_callback_t \n
 *
 * @param[in] dev      Pointer to the associated network device.
 * @param[in] data_cb  Callback for the data.
 * @param[in] msg_cb   Callback for the message.
 * @param[in] type     Type of dynamic node.
 * @param[in] app_ctx  Application context for notify callback.
 *
 * @return
 * NSS interface number allocated.
 */
extern int nss_gre_redir_alloc_and_register_node(struct net_device *dev,
		nss_gre_redir_data_callback_t data_cb,
		nss_gre_redir_msg_callback_t msg_cb,
		uint32_t type, void *app_ctx);

/**
 * nss_gre_redir_configure_inner_node
 *	Configures inner GRE redirect node.
 *
 * @datatypes
 * nss_gre_redir_inner_configure_msg
 *
 * @param[in] ifnum              NSS interface number.
 * @param[in] ngrcm              Inner node configuration message.
 *
 * @return
 * Status of Tx operation.
 */
extern nss_tx_status_t nss_gre_redir_configure_inner_node(int ifnum,
		struct nss_gre_redir_inner_configure_msg *ngrcm);

/**
 * nss_gre_redir_configure_outer_node
 *	Configures outer GRE redirect node.
 *
 * @datatypes
 * nss_gre_redir_outer_configure_msg
 *
 * @param[in] ifnum              NSS interface number.
 * @param[in] ngrcm              Outer node configuration message.
 *
 * @return
 * Status of Tx operation.
 */
extern nss_tx_status_t nss_gre_redir_configure_outer_node(int ifnum,
		struct nss_gre_redir_outer_configure_msg *ngrcm);

/**
 * nss_gre_redir_exception_ds_reg_cb
 *	Configure a callback on VAP for downstream GRE exception flows.
 *
 * @datatypes
 * nss_gre_redir_exception_ds_reg_cb_msg
 *
 * @param[in] ifnum              NSS interface number.
 * @param[in] ngrcm              Downstream exception callback registration message.
 *
 * @return
 * Status of Tx operation.
 */
extern nss_tx_status_t nss_gre_redir_exception_ds_reg_cb(int ifnum,
		struct nss_gre_redir_exception_ds_reg_cb_msg *ngrcm);

/**
 * nss_gre_redir_tx_msg_sync
 *	Sends messages to NSS firmware synchronously.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_gre_redir_msg
 *
 * @param[in] nss_ctx  NSS core context.
 * @param[in] ngrm     Pointer to GRE redirect message data.
 *
 * @return
 * Status of Tx operation.
 */
extern nss_tx_status_t nss_gre_redir_tx_msg_sync(struct nss_ctx_instance *nss_ctx, struct nss_gre_redir_msg *ngrm);

/**
 * nss_gre_redir_get_context
 *	Gets the GRE redirect context.
 *
 * @return
 * Pointer to the NSS core context.
 */
extern struct nss_ctx_instance *nss_gre_redir_get_context(void);

/**
 * nss_gre_redir_get_dentry
 *	Returns directory entry created in debugfs for statistics.
 *
 * @return
 * Pointer to created directory entry for GRE redirect.
 */
extern struct dentry *nss_gre_redir_get_dentry(void);

/**
 * nss_gre_redir_get_device
 *	Gets the original device from probe.
 *
 * @return
 * Pointer to the device.
 */
extern struct device *nss_gre_redir_get_device(void);

/**
 * nss_gre_redir_stats_unregister_notifier
 *	Deregisters a statistics notifier.
 *
 * @datatypes
 *	notifier_block
 *
 * @param[in] nb Notifier block.
 *
 * @return
 * 0 on success or non-zero on failure.
 */
extern int nss_gre_redir_stats_unregister_notifier(struct notifier_block *nb);

/**
 * nss_gre_redir_stats_register_notifier
 *	Registers a statistics notifier.
 *
 * @datatypes
 *	notifier_block
 *
 * @param[in] nb Notifier block.
 *
 * @return
 * 0 on success or non-zero on failure.
 */
extern int nss_gre_redir_stats_register_notifier(struct notifier_block *nb);

/**
 * @}
 */

#endif /* __NSS_GRE_REDIR_H */

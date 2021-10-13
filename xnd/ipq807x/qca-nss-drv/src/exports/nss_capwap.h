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

/*
 * @file nss_capwap.h
 *	NSS CAPWAP interface definitions.
 */

#ifndef __NSS_CAPWAP_H
#define __NSS_CAPWAP_H

/**
 * @addtogroup nss_capwap_subsystem
 * @{
 */

/**
 * Size of the headroom required for CAPWAP packets.
 */
#define NSS_CAPWAP_HEADROOM 256

/**
 * nss_capwap_stats_encap_types
 *	CAPWAP encapsulation statistics.
 */
enum nss_capwap_stats_encap_types {
	NSS_CAPWAP_STATS_ENCAP_TX_PKTS,
	NSS_CAPWAP_STATS_ENCAP_TX_BYTES,
	NSS_CAPWAP_STATS_ENCAP_TX_SEGMENTS,
	NSS_CAPWAP_STATS_ENCAP_TX_DROP_SG_REF,
	NSS_CAPWAP_STATS_ENCAP_TX_DROP_VER_MISMATCH,
	NSS_CAPWAP_STATS_ENCAP_TX_DROP_UNALIGN,
	NSS_CAPWAP_STATS_ENCAP_TX_DROP_HEADER_ROOM,
	NSS_CAPWAP_STATS_ENCAP_TX_DROP_DTLS,
	NSS_CAPWAP_STATS_ENCAP_TX_DROP_NWIRELESS,
	NSS_CAPWAP_STATS_ENCAP_TX_DROP_QUEUE_FULL,
	NSS_CAPWAP_STATS_ENCAP_TX_DROP_MEM_FAIL,
	NSS_CAPWAP_STATS_ENCAP_FAST_MEM,
	NSS_CAPWAP_STATS_ENCAP_MAX
};

/**
 * nss_capwap_stats_decap_types
 *	CAPWAP decapsulation statistics.
 */
enum nss_capwap_stats_decap_types {
	NSS_CAPWAP_STATS_DECAP_RX_PKTS,
	NSS_CAPWAP_STATS_DECAP_RX_BYTES,
	NSS_CAPWAP_STATS_DECAP_RX_DTLS_PKTS,
	NSS_CAPWAP_STATS_DECAP_RX_SEGMENTS,
	NSS_CAPWAP_STATS_DECAP_RX_DROP,
	NSS_CAPWAP_STATS_DECAP_RX_DROP_OVERSIZE,
	NSS_CAPWAP_STATS_DECAP_RX_DROP_FRAG_TIMEOUT,
	NSS_CAPWAP_STATS_DECAP_RX_DROP_DUP_FRAG,
	NSS_CAPWAP_STATS_DECAP_RX_DROP_FRAG_GAP,
	NSS_CAPWAP_STATS_DECAP_RX_DROP_QUEUE_FULL,
	NSS_CAPWAP_STATS_DECAP_RX_DROP_N2H_QUEUE_FULL,
	NSS_CAPWAP_STATS_DECAP_RX_DROP_MEM_FAIL,
	NSS_CAPWAP_STATS_DECAP_RX_DROP_CHECKSUM,
	NSS_CAPWAP_STATS_DECAP_RX_MALFORMED,
	NSS_CAPWAP_STATS_DECAP_FAST_MEM,
	NSS_CAPWAP_STATS_DECAP_MAX
};

/**
 * nss_capwap_msg_type
 *	CAPWAP message types.
 */
typedef enum nss_capwap_msg_type {
	NSS_CAPWAP_MSG_TYPE_NONE,
	NSS_CAPWAP_MSG_TYPE_CFG_RULE,
	NSS_CAPWAP_MSG_TYPE_UNCFG_RULE,
	NSS_CAPWAP_MSG_TYPE_ENABLE_TUNNEL,
	NSS_CAPWAP_MSG_TYPE_DISABLE_TUNNEL,
	NSS_CAPWAP_MSG_TYPE_UPDATE_PATH_MTU,
	NSS_CAPWAP_MSG_TYPE_SYNC_STATS,
	NSS_CAPWAP_MSG_TYPE_VERSION,		/**< Default is version 1. */
	NSS_CAPWAP_MSG_TYPE_DTLS,
	NSS_CAPWAP_MSG_TYPE_FLOW_RULE_ADD,
	NSS_CAPWAP_MSG_TYPE_FLOW_RULE_DEL,
	NSS_CAPWAP_MSG_TYPE_MAX,
} nss_capwap_msg_type_t;

/**
 * nss_capwap_msg_response
 *	Error types for CAPWAP responses to messages from the host.
 */
typedef enum nss_capwap_msg_response {
	NSS_CAPWAP_ERROR_MSG_INVALID_REASSEMBLY_TIMEOUT,
	NSS_CAPWAP_ERROR_MSG_INVALID_PATH_MTU,
	NSS_CAPWAP_ERROR_MSG_INVALID_MAX_FRAGMENT,
	NSS_CAPWAP_ERROR_MSG_INVALID_BUFFER_SIZE,
	NSS_CAPWAP_ERROR_MSG_INVALID_L3_PROTO,
	NSS_CAPWAP_ERROR_MSG_INVALID_UDP_PROTO,
	NSS_CAPWAP_ERROR_MSG_INVALID_VERSION,
	NSS_CAPWAP_ERROR_MSG_TUNNEL_DISABLED,
	NSS_CAPWAP_ERROR_MSG_TUNNEL_ENABLED,
	NSS_CAPWAP_ERROR_MSG_TUNNEL_NOT_CFG,
	NSS_CAPWAP_ERROR_MSG_INVALID_IP_NODE,
	NSS_CAPWAP_ERROR_MSG_INVALID_TYPE_FLAG,
	NSS_CAPWAP_ERROR_MSG_INVALID_DTLS_CFG,
	NSS_CAPWAP_ERROR_MSG_FLOW_TABLE_FULL,
	NSS_CAPWAP_ERROR_MSG_FLOW_EXIST,
	NSS_CAPWAP_ERROR_MSG_FLOW_NOT_EXIST,
	NSS_CAPWAP_ERROR_MSG_MAX,
} nss_capwap_msg_response_t;

/**
 * nss_capwap_stats_msg
 *	Per-tunnel statistics messages from the NSS firmware.
 */
struct nss_capwap_stats_msg {
	struct nss_cmn_node_stats pnode_stats;	/**< Common firmware statistics. */
	uint32_t dtls_pkts;		/**< Number of DTLS packets flowing through. */

	/*
	 * Rx/decap stats
	 */
	uint32_t rx_dup_frag;		/**< Number of duplicate fragments. */
	uint32_t rx_segments;		/**< Number of segments or fragments. */

	/**
	 * Packets dropped because they are larger than the payload size.
	 */
	uint32_t rx_oversize_drops;

	uint32_t rx_frag_timeout_drops;
			/**< Packets dropped because of a reassembly timeout. */
	uint32_t rx_n2h_drops;
			/**< Packets dropped because of error in packet processing. */
	uint32_t rx_n2h_queue_full_drops;
			/**< Packets dropped because the NSS-to-host queue is full. */
	uint32_t rx_csum_drops;
			/**< Packets dropped because of a checksum mismatch. */
	uint32_t rx_malformed;
			/**< Packets dropped because of a malformed packet. */
	uint32_t rx_mem_failure_drops;
			/**< Packets dropped because of a memory failure. */
	uint32_t rx_frag_gap_drops;
			/**< Packets dropped because of a non-sequential fragment offset. */

	/*
	 * Tx/encap stats
	 */
	uint32_t tx_segments;		/**< Number of segments or fragments. */
	uint32_t tx_queue_full_drops;	/**< Packets dropped because of a full queue. */
	uint32_t tx_mem_failure_drops;
			/**< Packets dropped because of a memory failure. */
	uint32_t tx_dropped_sg_ref;
			/**< Packets dropped because of a scatter-gather reference. */
	uint32_t tx_dropped_ver_mis;
			/**< Packets dropped because of a version mismatch. */
	uint32_t Reserved;
			/**< Reserved. */
	uint32_t tx_dropped_hroom;
			/**< Packets dropped because of insufficent headroom. */
	uint32_t tx_dropped_dtls;
			/**< Packets dropped because of a DTLS packet. */
	uint32_t tx_dropped_nwireless;
			/**< Packets dropped because the nwireless field information is wrong. */

	uint32_t fast_mem;
			/**< Set to 1 when tunnel is operating in fast memory. */
};

/**
 * nss_capwap_ip
 *	IP versions.
 */
struct nss_capwap_ip {
	/**
	 * Union of IPv4 and IPv6 IP addresses.
	 */
	union {
		uint32_t ipv4;		/**< IPv4 address. */
		uint32_t ipv6[4];	/**< IPv6 address. */
	} ip;		/**< Union of IPv4 and IPv6 IP addresses. */
};

/**
 * nss_capwap_encap_rule
 *	Encapsulation information for a CAPWAP tunnel.
 */
struct nss_capwap_encap_rule {
	struct  nss_capwap_ip src_ip;	/**< Source IP. */
	uint32_t src_port;		/**< Source port. */
	struct nss_capwap_ip dest_ip;	/**< Destination IP. */
	uint32_t dest_port;		/**< Destination port. */
	uint32_t path_mtu;		/**< MTU on the path. */
};

/**
 * nss_capwap_decap_rule
 *	Decapsulation information for a CAPWAP tunnel.
 */
struct nss_capwap_decap_rule {
	uint32_t reassembly_timeout;	/**< Timeout in milliseconds. */
	uint32_t max_fragments;		/**< Maximum number of fragments expected. */
	uint32_t max_buffer_size;	/**< Maximum size of the payload buffer. */
};

/**
 * nss_capwap_rule_msg
 *	CAPWAP rule message.
 *
 * The same rule structure applies for both encapsulation and decapsulation
 * in a tunnel.
 */
struct nss_capwap_rule_msg {
	struct nss_capwap_encap_rule encap;	/**< Encapsulation portion of the rule. */
	struct nss_capwap_decap_rule decap;	/**< Decapsulation portion of the rule. */
	uint32_t stats_timer;	/**< Statistics interval timer in milliseconds. */

	/**
	 * Core to choose for receiving packets.
	 *
	 * Set to -1 for the NSS firmware to decide.
	 */
	int8_t rps;

	uint8_t type_flags;		/**< VLAN or PPPOE is configured. */
	uint8_t l3_proto;
			/**< Prototype is NSS_CAPWAP_TUNNEL_IPV4 or NSS_CAPWAP_TUNNEL_IPV6. */
	uint8_t which_udp;		/**< Tunnel uses the UDP or UDPLite protocol. */
	uint32_t mtu_adjust;	/**< MTU is reserved for a DTLS process. */
	uint32_t gmac_ifnum;	/**< Outgoing physical interface. */
	uint32_t enabled_features;
			/**< Tunnel enabled features bit flag. */

	/*
	 * Parameters for each features
	 */
	uint32_t dtls_inner_if_num;	/**< Interface number of the associated DTLS node. */
	uint8_t bssid[ETH_ALEN];	/**< BSSID value. */
	uint16_t outer_sgt_value;
			/**< Security Group Tag value configured for this tunnel. */
};

/**
 * nss_capwap_version_msg
 *	Message to set the CAPWAP version.
 */
struct nss_capwap_version_msg {
	uint32_t version;	/**< CAPWAP protocol version. */
};

/**
 * nss_capwap_path_mtu_msg
 *	Message information for the path MTU.
 */
struct nss_capwap_path_mtu_msg {
	uint32_t path_mtu;	/**< Path MTU value between the controller and access point. */
};

/**
 * nss_capwap_dtls_msg
 *	DTLS message information.
 */
struct nss_capwap_dtls_msg {
	uint32_t enable;			/**< Enable or disable DTLS. */
	uint32_t dtls_inner_if_num;	/**< Interface number of the associated DTLS. */
	uint32_t mtu_adjust;		/**< MTU adjustment reported by the DTLS node. */
	uint32_t reserved;			/**< Reserved field for future use. */
};

/**
 * nss_capwap_flow_rule_msg
 *	CAPWAP flow rule message structure.
 */
struct nss_capwap_flow_rule_msg {
	/*
	 * 5-tuple info.
	 */
	uint16_t ip_version;            /**< IP version. */
	uint16_t protocol;              /**< Layer 4 protocol. */
	uint16_t src_port;		/**< Source port. */
	uint16_t dst_port;		/**< Destination port. */
	uint32_t src_ip[4];		/**< Source IP address. */
	uint32_t dst_ip[4];		/**< Destination IP address. */

	/*
	 * Flow attributes.
	 */
	uint32_t flow_id;		/**< Flow identification. */
};

/**
 * nss_capwap_enable_tunnel_msg
 *	Structure to update sibling interface number.
 */
struct nss_capwap_enable_tunnel_msg {
	uint32_t sibling_if_num; /**< Sibling interface Number. */
};

/**
 * nss_capwap_msg
 *	Data for sending and receiving CAPWAP messages.
 */
struct nss_capwap_msg {
	struct nss_cmn_msg cm;		/**< Common message header. */

	/**
	 * Payload of a CAPWAP common message.
	 */
	union {
		struct nss_capwap_rule_msg rule;
				/**< Rule information. */
		struct nss_capwap_path_mtu_msg mtu;
				/**< New MTU information. */
		struct nss_capwap_stats_msg stats;
				/**< CAPWAP statistics. */
		struct nss_capwap_version_msg version;
				/**< CAPWAP version to use. */
		struct nss_capwap_dtls_msg dtls;
				/**< DTLS configuration. */
		struct nss_capwap_flow_rule_msg flow_rule_add;
				/**< Flow rule add message. */
		struct nss_capwap_flow_rule_msg flow_rule_del;
				/**< Flow rule delete message. */
		struct nss_capwap_enable_tunnel_msg enable_tunnel;
				/**< Enable tunnel message. */
	} msg;			/**< Message payload. */
};

/**
 * nss_capwap_pn_stats
 *	Pnode statistics (64-bit version).
 */
struct nss_capwap_pn_stats {
	uint64_t rx_packets;		/**< Number of packets received. */
	uint64_t rx_bytes;		/**< Number of bytes received. */
	uint64_t rx_dropped;		/**< Number of dropped Rx packets. */
	uint64_t tx_packets;		/**< Number of packets transmitted. */
	uint64_t tx_bytes;		/**< Number of bytes transmitted. */
};

/**
 * nss_capwap_tunnel_stats
 *	Per-tunnel statistics seen by the HLOS.
 */
struct nss_capwap_tunnel_stats {
	struct nss_capwap_pn_stats pnode_stats;	/**< Common firmware statistics. */
	uint64_t dtls_pkts;		/**< Number of DTLS packets flowing through. */

	/*
	 * Rx/decap stats
	 */
	uint64_t rx_dup_frag;		/**< Number of duplicate fragments. */
	uint64_t rx_segments;		/**< Number of segments or fragments. */

	/**
	 * Packets dropped because they are larger than the payload size.
	 */
	uint64_t rx_oversize_drops;

	uint64_t rx_frag_timeout_drops;
			/**< Packets dropped because of a reassembly timeout. */
	uint64_t rx_n2h_drops;
			/**< Packets dropped because of error in processing the packet. */
	uint64_t rx_n2h_queue_full_drops;
			/**< Packets dropped because the NSS-to-host queue is full. */
	uint64_t rx_csum_drops;
			/**< Packets dropped because of a checksum mismatch. */
	uint64_t rx_malformed;
			/**< Packets dropped because of a malformed packet. */
	uint64_t rx_mem_failure_drops;
			/**< Packets dropped because of a memory failure. */
	uint64_t rx_frag_gap_drops;
			/**< Packets dropped because of a non-sequential fragment offset. */

	/*
	 * Tx/encap stats
	 */
	uint64_t tx_dropped_inner;	/**<Packets dropped due to inflow queue full. */
	uint64_t tx_segments;		/**< Number of segments or fragments. */
	uint64_t tx_queue_full_drops;
			/**< Packets dropped because the queue is full. */
	uint64_t tx_mem_failure_drops;
			/**< Packets dropped because of a memory failure. */

	uint64_t tx_dropped_sg_ref;
			/**< Packets dropped because of a scatter-gather reference. */
	uint64_t tx_dropped_ver_mis;
			/**< Packets dropped because of a version mismatch. */
	uint64_t Reserved;
			/**< Reserved. */
	uint64_t tx_dropped_hroom;
			/**< Packets dropped because of insufficent headroom. */
	uint64_t tx_dropped_dtls;
			/**< Packets dropped because of a DTLS packet. */
	uint64_t tx_dropped_nwireless;
			/**< Packets dropped because the nwireless field information is wrong. */

	uint32_t fast_mem;
			/**< Set to 1 when tunnel is operating in fast memory. */
};

/**
 * nss_capwap_stats_notification
 *	CAPWAP statistics structure.
 */
struct nss_capwap_stats_notification {
	uint32_t core_id;			/**< Core ID. */
	uint32_t if_num;			/**< Interface number. */
	struct nss_capwap_tunnel_stats stats;	/**< Per-tunnel statistics. */
};

#ifdef __KERNEL__ /* only kernel will use. */

/**
 * Callback function for receiving CAPWAP tunnel data.
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
typedef void (*nss_capwap_buf_callback_t)(struct net_device *netdev, struct sk_buff *skb, struct napi_struct *napi);

/**
 * Callback function for receiving CAPWAP tunnel messages.
 *
 * @datatypes
 * nss_capwap_msg
 *
 * @param[in] app_data  Pointer to the application context of the message.
 * @param[in] msg       Pointer to the message data.
 */
typedef void (*nss_capwap_msg_callback_t)(void *app_data, struct nss_capwap_msg *msg);

/**
 * nss_capwap_data_register
 *	Registers the CAPWAP tunnel interface with the NSS for sending and
 *	receiving tunnel messages.
 *
 * @datatypes
 * nss_capwap_buf_callback_t \n
 * net_device
 *
 * @param[in] if_num           NSS interface number.
 * @param[in] capwap_callback  Callback for the CAPWAP tunnel data.
 * @param[in] netdev           Pointer to the associated network device.
 * @param[in] features         Data socket buffer types supported by this interface.
 *
 * @return
 * Pointer to the NSS core context.
 */
extern struct nss_ctx_instance *nss_capwap_data_register(uint32_t if_num, nss_capwap_buf_callback_t capwap_callback, struct net_device *netdev, uint32_t features);

/**
 * nss_capwap_tx_msg
 *	Sends CAPWAP tunnel messages to the NSS.
 *
 * Do not call this function from a softirq or interrupt because it
 * might sleep if the NSS firmware is busy serving another host thread.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_capwap_msg
 *
 * @param[in]     nss_ctx  Pointer to the NSS context.
 * @param[in,out] msg      Pointer to the message data.
 *
 * @return
 * Status of the Tx operation.
 */
extern nss_tx_status_t nss_capwap_tx_msg(struct nss_ctx_instance *nss_ctx, struct nss_capwap_msg *msg);

/**
 * nss_capwap_tx_buf
 *	Sends a CAPWAP tunnel data buffer to the NSS interface.
 *
 * @datatypes
 * nss_ctx_instance \n
 * sk_buff
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 * @param[in] os_buf   Pointer to the OS data buffer.
 * @param[in] if_num   NSS interface number.
 *
 * @return
 * Status of the Tx operation.
 */
extern nss_tx_status_t nss_capwap_tx_buf(struct nss_ctx_instance *nss_ctx, struct sk_buff *os_buf, uint32_t if_num);

/**
 * nss_capwap_data_unregister
 *	Deregisters the CAPWAP tunnel interface from the NSS interface.
 *
 * @param[in] if_num  NSS interface number.
 *
 * @return
 * TRUE or FALSE
 *
 * @dependencies
 * The tunnel interface must have been previously registered.
 */
extern bool nss_capwap_data_unregister(uint32_t if_num);

/**
 * nss_capwap_notify_register
 *	Registers an event callback handler with the HLOS driver.
 *
 * @datatypes
 * nss_capwap_msg_callback_t
 *
 * @param[in] if_num    NSS interface number.
 * @param[in] cb        Callback function for the message.
 * @param[in] app_data  Pointer to the application context of the message.
 *
 * @return
 * Pointer to the NSS core context.
 */
extern struct nss_ctx_instance *nss_capwap_notify_register(uint32_t if_num, nss_capwap_msg_callback_t cb, void *app_data);

/**
 * nss_capwap_notify_unregister
 *	Deregisters a message notifier from the HLOS driver.
 *
 * @datatypes
 * nss_ctx_instance
 *
 * @param[in] ctx     Pointer to the context of the HLOS driver.
 * @param[in] if_num  NSS interface number.
 *
 * @return
 * None.
 *
 * @dependencies
 * The message notifier must have been previously registered.
 */
extern nss_tx_status_t nss_capwap_notify_unregister(struct nss_ctx_instance *ctx, uint32_t if_num);

/**
 * nss_capwap_get_ctx
 *	Gets the NSS context.
 *
 * @return
 * Pointer to the NSS core context.
 */
extern struct nss_ctx_instance *nss_capwap_get_ctx(void);

/**
 * nss_capwap_ifnum_with_core_id
 *	Gets the CAPWAP interface number with the core ID.
 *
 * @param[in] if_num  NSS interface number.
 *
 * @return
 * Interface number with the core ID.
 */
extern int nss_capwap_ifnum_with_core_id(int if_num);

/**
 * nss_capwap_get_max_buf_size
 *	Gets the NSS maximum buffer size.
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 *
 * @return
 * Maximum buffer size of this NSS core.
 */
extern uint32_t nss_capwap_get_max_buf_size(struct nss_ctx_instance *nss_ctx);

/**
 * nss_capwap_get_stats
 *	Gets per-tunnel statistics.
 *
 * @datatypes
 * nss_capwap_tunnel_stats
 *
 * @param[in]  if_num  NSS interface number.
 * @param[out] stats   Pointer to the CAPWAP tunnel statistics.
 *
 * @return
 * TRUE or FALSE.
 */
extern bool nss_capwap_get_stats(uint32_t if_num, struct nss_capwap_tunnel_stats *stats);

/**
 * nss_capwap_init
 *	Initializes the CAPWAP interface.
 *
 * @return
 * None.
 */
extern void nss_capwap_init(void);

/**
 * nss_capwap_msg_init
 *	Initializes a CAPWAP message.
 *
 * @datatypes
 * nss_capwap_msg \n
 * nss_capwap_msg_callback_t
 *
 * @param[in,out] ncm       Pointer to the message.
 * @param[in]     if_num    NSS interface number.
 * @param[in]     type      Type of message.
 * @param[in]     len       Size of the payload.
 * @param[in]     cb        Callback function for the message.
 * @param[in]     app_data  Pointer to the application context of the message.
 *
 * @return
 * None.
 */
extern void nss_capwap_msg_init(struct nss_capwap_msg *ncm, uint16_t if_num, uint32_t type, uint32_t len,
								nss_capwap_msg_callback_t cb, void *app_data);
/**
 * nss_capwap_stats_register_notifier
 *	Registers a statistics notifier.
 *
 * @datatypes
 * notifier_block
 *
 * @param[in] nb Notifier block.
 *
 * @return
 * 0 on success or -2 on failure.
 */
extern int nss_capwap_stats_register_notifier(struct notifier_block *nb);

/**
 * nss_capwap_stats_unregister_notifier
 *	Deregisters a statistics notifier.
 *
 * @datatypes
 * notifier_block
 *
 * @param[in] nb Notifier block.
 *
 * @return
 * 0 on success or -2 on failure.
 */
extern int nss_capwap_stats_unregister_notifier(struct notifier_block *nb);

/**
 * @}
 */

#endif /* __KERNEL__ */
#endif /* __NSS_CAPWAP_H */

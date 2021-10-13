/*
 **************************************************************************
 * Copyright (c) 2019-2020, The Linux Foundation. All rights reserved.
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
 * @file nss_pvxlan.h
 *	NSS proxy VxLAN interface definitions.
 */

#ifndef __NSS_PVXLAN_H
#define __NSS_PVXLAN_H

/**
 * @addtogroup nss_pvxlan_subsystem
 * @{
 */

/**
 * Size of the headroom required for proxy VxLAN packets.
 */
#define NSS_PVXLAN_HEADROOM 256

/**
 * Maximum number of supported proxy VxLAN tunnel sessions.
 */
#define NSS_PVXLAN_MAX_INTERFACES 32

/*
 * Proxy VxLAN Rule configure message flags
 */
#define NSS_PVXLAN_TUNNEL_IPV4 0x0001		/**< IPv4 tunnel. */
#define NSS_PVXLAN_TUNNEL_IPV6 0x0002		/**< IPv6 tunnel. */
#define NSS_PVXLAN_TUNNEL_UDP 0x0010		/**< UDP tunnel. */
#define NSS_PVXLAN_TUNNEL_UDPLite 0x0020	/**< UDPLite tunnel. */
#define NSS_PVXLAN_TUNNEL_ENCAP_UDPLITE_HDR_CSUM 0x0100
	/**< Generate only UDPLite header checksum. Otherwise whole UDPLite payload. */

/**
 * nss_pvxlan_msg_type
 *	Proxy VxLAN message types.
 */
typedef enum nss_pvxlan_msg_type {
	NSS_PVXLAN_MSG_TYPE_SYNC_STATS,		/**< Statistics synchronization message. */
	NSS_PVXLAN_MSG_TYPE_TUNNEL_CREATE_RULE,	/**< Creating tunnel rule. */
	NSS_PVXLAN_MSG_TYPE_TUNNEL_DESTROY_RULE,
						/**< Destroying tunnel rule. */
	NSS_PVXLAN_MSG_TYPE_TUNNEL_ENABLE,	/**< Enable the tunnel. */
	NSS_PVXLAN_MSG_TYPE_TUNNEL_DISABLE,	/**< Disable the tunnel. */
	NSS_PVXLAN_MSG_TYPE_MAC_ADD,		/**< Add MAC rule to the database. */
	NSS_PVXLAN_MSG_TYPE_MAC_DEL,		/**< Remove MAC rule from the database. */
	NSS_PVXLAN_MSG_TYPE_MAX,		/**< Maximum message type. */
} nss_pvxlan_msg_type_t;

/**
 * nss_pvxlan_error_response_types
 *	Error types for proxy VxLAN responses to messages from the host.
 */
typedef enum nss_pvxlan_error_response_types {
	NSS_PVXLAN_ERROR_UNKNOWN_TYPE = 1,	/**< Unknown type error. */
	NSS_PVXLAN_ERROR_INVALID_L3_PROTO,	/**< L3 Protocol is invalid error. */
	NSS_PVXLAN_ERROR_INVALID_UDP_PROTO,	/**< UDP Protocol is invalid error. */
	NSS_PVXLAN_ERROR_TUNNEL_DISABLED,	/**< Tunnel is already disabled error. */
	NSS_PVXLAN_ERROR_TUNNEL_ENABLED,	/**< Tunnel is already enabled error. */
	NSS_PVXLAN_ERROR_TUNNEL_ENTRY_EXIST,
						/**< Tunnel is already exist error. */
	NSS_PVXLAN_ERROR_TUNNEL_TABLE_FULL,
						/**< Tunnel table is full error. */
	NSS_PVXLAN_ERROR_INVALID_TUNNEL_ID,	/**< Tunnel ID is invalid error. */
	NSS_PVXLAN_ERROR_MAC_TABLE_FULL,	/**< MAC table is full error. */
	NSS_PVXLAN_ERROR_MAC_EXIST,		/**< MAC does already exist in the table error. */
	NSS_PVXLAN_ERROR_MAC_NOT_EXIST,		/**< MAC does not exist in the table error. */
	NSS_PVXLAN_ERROR_MAC_ENTRY_UNHASHED,
						/**< MAC entry is not hashed in table. */
	NSS_PVXLAN_ERROR_MAC_ENTRY_INSERT_FAILED,
						/**< Insertion to MAC table is failed. */
	NSS_PVXLAN_ERROR_UDP_ENCAP_TUNNEL_ID_IN_USE,
						/**< Given tunnel ID is currently used. */
	PVXLAN_ERROR_MSG_TUNNEL_ADD_FAILED,	/**< Tunnel add information failed. */
	PVXLAN_ERROR_MSG_MAC_ENTRY_ALLOC_FAILED,
						/**< MAC entry allocation failed. */
	PVXLAN_ERROR_MSG_MAC_ENTRY_DELETE_FAILED,
						/**< MAC entry deletion failed. */
	NSS_PVXLAN_ERROR_MAX,			/**< Maximum error type. */
} nss_pvxlan_error_response_t;

/**
 * nss_pvxlan_stats_msg
 *	Per-tunnel statistics messages from the NSS firmware.
 */
struct nss_pvxlan_stats_msg {
	struct nss_cmn_node_stats node_stats;	/**< Common firmware statistics. */
	uint32_t mac_db_lookup_failed;		/**< MAC Database look up failed. */
	uint32_t udp_encap_lookup_failed;	/**< MAC Database look up failed. */
	uint32_t dropped_malformed;		/**< Packet is malformed. */
	uint32_t dropped_next_node_queue_full;	/**< Next node dropped the packet. */
	uint32_t dropped_hroom;			/**< Transmit dropped due to insufficent headroom. */
	uint32_t dropped_ver_mis;		/**< Transmit dropped due to version mismatch. */
	uint32_t dropped_zero_sized_packet;	/**< Transmit dropped due to zero sized packet. */
	uint32_t dropped_pbuf_alloc_failed;	/**< Receive side pbuf allocation failed. */
	uint32_t dropped_linear_failed;		/**< Receive side linearization failed. */
};

/**
 * nss_pvxlan_ip
 *	IP versions.
 */
struct nss_pvxlan_ip {
	/**
	 * Union of IPv4 and IPv6 IP addresses.
	 */
	union {
		uint32_t ipv4;		/**< IPv4 address. */
		uint32_t ipv6[4];	/**< IPv6 address. */
	} ip;		/**< Union of IPv4 and IPv6 IP addresses. */
};

/**
 * nss_pvxlan_encap_rule
 *	Encapsulation information for a proxy VxLAN tunnel.
 */
struct nss_pvxlan_encap_rule {
	struct  nss_pvxlan_ip src;	/**< Source IP. */
	uint32_t src_port;		/**< Source port. */
	struct nss_pvxlan_ip dest;	/**< Destination IP. */
	uint32_t dest_port;		/**< Destination port. */
};

/**
 * nss_pvxlan_rule_msg
 *	Proxy VxLAN rule message.
 *
 * The same rule structure applies for both encapsulation and decapsulation
 * in a tunnel.
 */
struct nss_pvxlan_rule_msg {
	struct nss_pvxlan_encap_rule encap;	/**< Encapsulation portion of the rule. */
	uint32_t tunnel_id;			/**< Tunnel ID. */
	uint16_t flags;				/**< Tunnel type flags. */
	int8_t rps;
			/**< Receive packet steering number. Set -1 to let NSS firmware decide. */
};

/**
 * nss_pvxlan_tunnel_state_msg
 *	To enable/disable the tunnel.
 */
struct nss_pvxlan_tunnel_state_msg {
	uint32_t sibling_if_num;		/**< Sibling interface number. */
};

/**
 * nss_pvxlan_mac_msg
 *	Proxy VxLAN MAC message structure.
 */
struct nss_pvxlan_mac_msg {
	uint16_t mac_addr[3];			/**< MAC address. */
	uint16_t flags;				/**< Flags. */
	uint32_t vnet_id;			/**< Virtual net ID. */
	uint32_t tunnel_id;			/**< Tunnel ID. */
	uint16_t policy_id;			/**< Policy ID. */
};

/**
 * nss_pvxlan_msg
 *	Data for sending and receiving proxy VxLAN messages.
 */
struct nss_pvxlan_msg {
	struct nss_cmn_msg cm;		/**< Common message header. */

	/**
	 * Payload of a proxy VxLAN common message.
	 */
	union {
		struct nss_pvxlan_stats_msg stats;
				/**< Proxy VxLAN statistics. */
		struct nss_pvxlan_rule_msg rule_cfg;
				/**< Rule information. */
		struct nss_pvxlan_rule_msg rule_uncfg;
				/**< Rule information. */
		struct nss_pvxlan_tunnel_state_msg enable;
				/**< Enable the tunnel. */
		struct nss_pvxlan_mac_msg mac_add;
				/**< MAC rule add message. */
		struct nss_pvxlan_mac_msg mac_del;
				/**< MAC rule delete message. */
	} msg;			/**< Message payload. */
};

/**
 * Callback function for receiving proxy VxLAN tunnel data.
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
typedef void (*nss_pvxlan_buf_callback_t)(struct net_device *netdev, struct sk_buff *skb, struct napi_struct *napi);

/**
 * Callback function for receiving proxy VxLAN tunnel messages.
 *
 * @datatypes
 * nss_pvxlan_msg
 *
 * @param[in] app_data  Pointer to the application context of the message.
 * @param[in] msg       Pointer to the message data.
 */
typedef void (*nss_pvxlan_msg_callback_t)(void *app_data, struct nss_pvxlan_msg *msg);

/**
 * nss_pvxlan_tx_msg
 *	Sends proxy VxLAN tunnel messages to the NSS.
 *
 * Do not call this function from a softirq or interrupt because it
 * might sleep if the NSS firmware is busy serving another host thread.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_pvxlan_msg
 *
 * @param[in]     nss_ctx  Pointer to the NSS context.
 * @param[in,out] msg      Pointer to the message data.
 *
 * @return
 * Status of the Tx operation.
 */
extern nss_tx_status_t nss_pvxlan_tx_msg(struct nss_ctx_instance *nss_ctx, struct nss_pvxlan_msg *msg);

/**
 * nss_pvxlan_tx_msg_sync
 *	Sends proxy VxLAN tunnel messages to the NSS.
 *
 * Do not call this function from a softirq or interrupt because it
 * might sleep if the NSS firmware is busy serving another host thread.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_pvxlan_msg
 *
 * @param[in]     nss_ctx  Pointer to the NSS context.
 * @param[in,out] msg      Pointer to the message data.
 *
 * @return
 * Status of the Tx operation.
 */
extern nss_tx_status_t nss_pvxlan_tx_msg_sync(struct nss_ctx_instance *nss_ctx, struct nss_pvxlan_msg *msg);

/**
 * nss_pvxlan_tx_buf
 *	Sends a proxy VXLAN tunnel data buffer to the NSS interface.
 *
 * @datatypes
 * nss_ctx_instance \n
 * sk_buff
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 * @param[in] buf   Pointer to the data buffer.
 * @param[in] if_num   NSS interface number.
 *
 * @return
 * Status of the Tx operation.
 */
extern nss_tx_status_t nss_pvxlan_tx_buf(struct nss_ctx_instance *nss_ctx, struct sk_buff *buf, uint32_t if_num);

/**
 * nss_pvxlan_unregister
 *	Deregisters the proxy VxLAN tunnel interface from the NSS interface.
 *
 * @param[in] if_num  NSS interface number.
 *
 * @return
 * TRUE or FALSE
 *
 * @dependencies
 * The tunnel interface must have been previously registered.
 */
extern bool nss_pvxlan_unregister(uint32_t if_num);

/**
 * nss_pvxlan_register
 *	Registers the proxy VxLAN tunnel interface with the NSS for sending and
 *	receiving tunnel messages.
 *
 * @datatypes
 * nss_pvxlan_msg_callback_t \n
 * nss_pvxlan_buf_callback_t \n
 * net_device
 *
 * @param[in] if_num           NSS interface number.
 * @param[in] data_cb          Data callback for the proxy VXLAN tunnel data.
 * @param[in] notify_cb        Notify callback for the proxy VXLAN tunnel data.
 * @param[in] netdev           Pointer to the associated network device.
 * @param[in] features         Data socket buffer types supported by this interface.
 *
 * @return
 * Pointer to the NSS core context.
 */
extern struct nss_ctx_instance *nss_pvxlan_register(uint32_t if_num, nss_pvxlan_buf_callback_t data_cb,
			nss_pvxlan_msg_callback_t notify_cb, struct net_device *netdev, uint32_t features);

/**
 * nss_pvxlan_get_ctx
 *	Gets the NSS context.
 *
 * @return
 * Pointer to the NSS core context.
 */
extern struct nss_ctx_instance *nss_pvxlan_get_ctx(void);

/**
 * nss_pvxlan_ifnum_with_core_id
 *	Gets the proxy VxLAN interface number with the core ID.
 *
 * @param[in] if_num  NSS interface number.
 *
 * @return
 * Interface number with the core ID.
 */
extern int nss_pvxlan_ifnum_with_core_id(int if_num);

/**
 * nss_pvxlan_init
 *	Initializes the proxy VXLAN interface.
 *
 * @return
 * None.
 */
extern void nss_pvxlan_init(void);

/**
 * nss_pvxlan_msg_init
 *	Initializes a proxy VxLAN message.
 *
 * @datatypes
 * nss_pvxlan_msg \n
 * nss_pvxlan_msg_callback_t
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
extern void nss_pvxlan_msg_init(struct nss_pvxlan_msg *ncm, uint16_t if_num, uint32_t type, uint32_t len,
								nss_pvxlan_msg_callback_t cb, void *app_data);

/**
 * @}
 */

#endif /* __NSS_PVXLAN_H */

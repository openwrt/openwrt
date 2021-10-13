/*
 **************************************************************************
 * Copyright (c) 2019, 2021 The Linux Foundation. All rights reserved.
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
 * @file nss_vxlan.h
 *	NSS VxLAN interface definitions.
 */

#ifndef __NSS_VXLAN_H
#define __NSS_VXLAN_H

/**
 * @addtogroup nss_vxlan_subsystem
 * @{
 */

/**
 * Maximum number of supported VxLAN tunnel sessions.
 */
#define NSS_VXLAN_MAX_TUNNELS 64

/**
 * Maximum number of supported VxLAN FDB entries.
 */
#define NSS_VXLAN_MACDB_ENTRIES_MAX 1024

/**
 * MAC database entries per message.
 */
#define NSS_VXLAN_MACDB_ENTRIES_PER_MSG 20

/*
 *  VxLAN Rule configure message flags
 */
#define NSS_VXLAN_RULE_FLAG_GBP_ENABLED 0x0001			/**< Group Policy ID is eanbled. */
#define NSS_VXLAN_RULE_FLAG_INHERIT_TOS 0x0002			/**< Use inner TOS for encapsulation. */
#define NSS_VXLAN_RULE_FLAG_ENCAP_L4_CSUM_REQUIRED 0x0004	/**< Generate transmit checksum. */
#define NSS_VXLAN_RULE_FLAG_IPV4 0x0010				/**< IPv4 tunnel. */
#define NSS_VXLAN_RULE_FLAG_IPV6 0x0020				/**< IPv6 tunnel. */
#define NSS_VXLAN_RULE_FLAG_UDP 0x0100				/**< UDP tunnel. */

/**
 * nss_vxlan_msg_type
 *	Message types for VxLAN tunnel.
 */
enum nss_vxlan_msg_type {
	NSS_VXLAN_MSG_TYPE_STATS_SYNC,		/**< Statistics synchronization message. */
	NSS_VXLAN_MSG_TYPE_TUN_CONFIGURE,	/**< Creating tunnel rule. */
	NSS_VXLAN_MSG_TYPE_TUN_UNCONFIGURE,	/**< Destroying tunnel rule. */
	NSS_VXLAN_MSG_TYPE_TUN_ENABLE,		/**< Enable the tunnel. */
	NSS_VXLAN_MSG_TYPE_TUN_DISABLE,		/**< Disable the tunnel. */
	NSS_VXLAN_MSG_TYPE_MAC_ADD,		/**< Add MAC rule to the database. */
	NSS_VXLAN_MSG_TYPE_MAC_DEL,		/**< Remove MAC rule from the database. */
	NSS_VXLAN_MSG_TYPE_MACDB_STATS,		/**< MAC database statistics synchronization message. */
	NSS_VXLAN_MSG_TYPE_MAX,			/**< Maximum message type. */
};

/**
 * nss_vxlan_error_response_types
 *	Error types for VxLAN responses to messages from the host.
 */
enum nss_vxlan_error_type {
	NSS_VXLAN_ERROR_TYPE_NONE = 1,			/**< Unknown type error. */
	NSS_VXLAN_ERROR_TYPE_DECAP_REGISTER_FAIL,	/**< Decapsulation node registration failed. */
	NSS_VXLAN_ERROR_TYPE_DEST_IP_MISMATCH,		/**< Destination IP address mismatch. */
	NSS_VXLAN_ERROR_TYPE_INVALID_VNI,		/**< Invalid virtual network ID. */
	NSS_VXLAN_ERROR_TYPE_INVALID_L3_PROTO,		/**< L3 Protocol is invalid error. */
	NSS_VXLAN_ERROR_TYPE_INVALID_UDP_PROTO,		/**< UDP Protocol is invalid error. */
	NSS_VXLAN_ERROR_TYPE_INVALID_SRC_PORT,		/**< Source port range is invalid. */
	NSS_VXLAN_ERROR_TYPE_MAC_BAD_ENTRY,		/**< MAC table has a bad entry. */
	NSS_VXLAN_ERROR_TYPE_MAC_EXISTS,		/**< MAC entry exists in the table error. */
	NSS_VXLAN_ERROR_TYPE_MAC_NOT_EXIST,		/**< MAC does not exist in the table error. */
	NSS_VXLAN_ERROR_TYPE_MAC_ENTRY_UNHASHED,	/**< MAC entry is not hashed in table. */
	NSS_VXLAN_ERROR_TYPE_MAC_ENTRY_ALLOC_FAILED,	/**< MAC entry allocation failed. */
	NSS_VXLAN_ERROR_TYPE_MAC_ENTRY_DELETE_FAILED,	/**< MAC entry deletion failed. */
	NSS_VXLAN_ERROR_TYPE_MAC_TABLE_FULL,		/**< MAC table is full error. */
	NSS_VXLAN_ERROR_TYPE_SIBLING_NODE_NOT_EXIST,	/**< Sibling node does not exist. */
	NSS_VXLAN_ERROR_TYPE_TUNNEL_CONFIGURED,		/**< Tunnel is already configured. */
	NSS_VXLAN_ERROR_TYPE_TUNNEL_UNCONFIGURED,	/**< Tunnel is not configured. */
	NSS_VXLAN_ERROR_TYPE_TUNNEL_ADD_FAILED,		/**< Adding tunnel information failed. */
	NSS_VXLAN_ERROR_TYPE_TUNNEL_DISABLED,		/**< Tunnel is already disabled error. */
	NSS_VXLAN_ERROR_TYPE_TUNNEL_ENABLED,		/**< Tunnel is already enabled error. */
	NSS_VXLAN_ERROR_TYPE_TUNNEL_ENTRY_EXISTS,	/**< Tunnel already exists. */
	NSS_VXLAN_ERROR_TYPE_MAX,			/**< Maximum error type. */
};

/**
 * nss_vxlan_stats_msg
 *	Per-tunnel statistics messages from the NSS firmware.
 */
struct nss_vxlan_stats_msg {
	struct nss_cmn_node_stats node_stats;	/**< Common firmware statistics. */
	uint32_t except_mac_db_lookup_failed;	/**< MAC database look up failed. */
	uint32_t except_mac_move;		/**< User is moved. */
	uint32_t except_low_hroom;		/**< Transmit exception due to insufficient headroom. */
	uint32_t except_no_policy_id;		/**< Policy ID does not exist. */
	uint32_t except_extra_vxlan_hdr_flags;	/**< More flags are set than NSS can process. */
	uint32_t except_vni_lookup_failed;	/**< Virtual network ID look up failed. */
	uint32_t dropped_malformed;		/**< Packet is malformed. */
	uint32_t dropped_next_node_queue_full;	/**< Next node dropped the packet. */
	uint32_t except_inner_hash;		/**< Inner hash calculation failed. */
};

/**
 * nss_vxlan_rule_msg
 *	 VxLAN rule message.
 *
 * The same rule structure applies for both encapsulation and decapsulation
 * in a tunnel.
 */
struct nss_vxlan_rule_msg {
	/*
	 * VxLAN Rules
	 */
	uint32_t sibling_if_num;	/**< Sibling node interface number. */
	uint32_t vni;			/**< Virtual network ID. */
	uint16_t tunnel_flags;		/**< VxLAN tunnel flags. */

	/*
	 * IP rules
	 */
	uint16_t flow_label;		/**< Flow label. */
	uint8_t tos;			/**< Type of service/traffic class. */
	uint8_t ttl;			/**< TTL/Hop Limit. */

	/*
	 * L4 rules
	 */
	uint16_t src_port_min;		/**< Minimum permissible port number. */
	uint16_t src_port_max;		/**< Maximum permissible port number. */
	uint16_t dest_port;		/**< UDP destination port. */
};

/**
 * nss_vxlan_encap_rule
 *	Encapsulation information for a VxLAN tunnel.
 */
struct nss_vxlan_encap_rule {
	uint32_t src_ip[4];	/**< Source IP. */
	uint32_t dest_ip[4];	/**< Destination IP. */
};

/**
 * nss_vxlan_mac_msg
 *	VxLAN MAC message structure.
 */
struct nss_vxlan_mac_msg {
	struct nss_vxlan_encap_rule encap;
					/**< Tunnel encapsulation header. */
	uint32_t vni;			/**< VxLAN network identifier. */
	uint16_t mac_addr[3];		/**< MAC address. */
};

/**
 * nss_vxlan_macdb_stats_entry
 *	MAC database statistics entry.
 */
struct nss_vxlan_macdb_stats_entry {
	uint32_t hits;			/**< Total hash hits on this hash entry. */
	uint16_t mac[3];		/**< MAC address. */
};

/**
 * nss_vxlan_macdb_stats_msg
 *	VxLAN MAC database statistics.
 */
struct nss_vxlan_macdb_stats_msg {
	uint16_t cnt;			/**< Number of MAC database entries copied. */
	uint16_t reserved;		/**< Reserved for future use. */
	struct nss_vxlan_macdb_stats_entry entry[NSS_VXLAN_MACDB_ENTRIES_PER_MSG];
					/**< MAC database entries. */
};

/**
 * nss_vxlan_msg
 *	Data structure for sending and receiving VxLAN messages.
 */
struct nss_vxlan_msg {
	struct nss_cmn_msg cm;			/**< Common message header. */

	/**
	 * Payload of a VxLAN message.
	 */
	union {
		struct nss_vxlan_stats_msg stats;
				/**< Synchronized statistics for the VxLAN interface. */
		struct nss_vxlan_rule_msg vxlan_create;
				/**< Allocate VxLAN tunnel node. */
		struct nss_vxlan_rule_msg vxlan_destroy;
				/**< Destroy VxLAN tunnel node. */
		struct nss_vxlan_mac_msg mac_add;
				/**< MAC add message for UDP encapsulation. */
		struct nss_vxlan_mac_msg mac_del;
				/**< MAC delete message. */
		struct nss_vxlan_macdb_stats_msg db_stats;
				/**< MAC database statistics. */
	} msg;			/**< Payload for VxLAN tunnel messages exchanged with the NSS core. */
};

/**
 * Callback function for receiving VxLAN tunnel data.
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
typedef void (*nss_vxlan_buf_callback_t)(struct net_device *netdev, struct sk_buff *skb, struct napi_struct *napi);

/**
 * Callback function for receiving VxLAN messages.
 *
 * @datatypes
 * nss_cmn_msg
 *
 * @param[in] app_data  Pointer to the application context of the message.
 * @param[in] msg       Pointer to the message data.
 */
typedef void (*nss_vxlan_msg_callback_t)(void *app_data, struct nss_cmn_msg *msg);

/**
 * nss_vxlan_tx_msg
 *	Sends VxLAN tunnel messages to the NSS.
 *
 * Do not call this function from a softirq or interrupt because it
 * might sleep if the NSS firmware is busy serving another host thread.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_vxlan_msg
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 * @param[in] nvm      Pointer to the message data.
 *
 * @return
 * Status of the Tx operation.
 */
extern nss_tx_status_t nss_vxlan_tx_msg(struct nss_ctx_instance *nss_ctx, struct nss_vxlan_msg *nvm);

/**
 * nss_vxlan_tx_msg_sync
 *	Sends a VxLAN message to the NSS synchronously.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_vxlan_msg
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 * @param[in] nvm      Pointer to the message data.
 *
 * @return
 * Status of the Tx operation.
 */
extern nss_tx_status_t nss_vxlan_tx_msg_sync(struct nss_ctx_instance *nss_ctx, struct nss_vxlan_msg *nvm);

/**
 * nss_vxlan_unregister_if
 *	Deregisters the VxLAN interface from the NSS.
 *
 * @param[in] if_num  NSS interface number.
 *
 * @return
 * None.
 */
extern bool nss_vxlan_unregister_if(uint32_t if_num);

/**
 * nss_vxlan_register_if
 *	Registers the VxLAN interface with the NSS.
 *
 * @datatypes
 * nss_vxlan_buf_callback_t \n
 * nss_vxlan_msg_callback_t \n
 * net_device
 *
 * @param[in] if_num             NSS interface number.
 * @param[in] type               Dynamic interface type.
 * @param[in] data_cb            Callback for the data.
 * @param[in] notify_cb          Callback for the message.
 * @param[in] netdev             Pointer to the associated network device.
 * @param[in] features           Data socket buffer types supported by this interface.
 *
 * @return
 * Pointer to the NSS core context.
 */
extern struct nss_ctx_instance *nss_vxlan_register_if(uint32_t if_num, uint32_t type, nss_vxlan_buf_callback_t data_cb,
					nss_vxlan_msg_callback_t notify_cb, struct net_device *netdev, uint32_t features);

/**
 * nss_vxlan_register_handler
 *	Initializes VxLAN module in NSS
 *
 * @return
 * None.
 */
extern void nss_vxlan_init(void);

/**
 * nss_vxlan_msg_init
 *	Initializes a VxLAN message.
 *
 * @datatypes
 * nss_vxlan_msg \n
 * nss_vxlan_msg_callback_t
 *
 * @param[in,out] nvm       Pointer to the VxLAN tunnel message.
 * @param[in]     if_num    NSS interface number.
 * @param[in]     type      Type of message.
 * @param[in]     len       Size of the message.
 * @param[in]     cb        Pointer to the message callback.
 * @param[in]     app_data  Pointer to the application context of the message.
 *
 * @return
 * None.
 */
extern void nss_vxlan_msg_init(struct nss_vxlan_msg *nvm, uint16_t if_num, uint32_t type, uint32_t len,
				nss_vxlan_msg_callback_t cb, void *app_data);

/**
 * nss_vxlan_get_ctx()
 *	Get VxLAN context.
 *
 * @return
 * Pointer to the NSS core context.
 */
extern struct nss_ctx_instance *nss_vxlan_get_ctx(void);

/**
 * @}
 */

#endif
/* __NSS_VXLAN_H */

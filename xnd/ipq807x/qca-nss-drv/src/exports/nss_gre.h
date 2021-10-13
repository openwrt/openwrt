/*
 ****************************************************************************
 * Copyright (c) 2017-2019, 2021, The Linux Foundation. All rights reserved.
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
 ****************************************************************************
 */

/**
 * @file nss_gre.h
 *	NSS GRE interface definitions.
 */
#ifndef _NSS_GRE_H_
#define _NSS_GRE_H_

#include <net/ip_tunnels.h>
#include <net/ip6_tunnel.h>

/**
 * @addtogroup nss_gre_subsystem
 * @{
 */

/**
 * Maximum number of session debug statistics
 */
#define NSS_GRE_MAX_DEBUG_SESSION_STATS 16

/**
 * GRE flags
 */
#define NSS_GRE_CONFIG_IKEY_VALID	0x00000001	/**< Incoming key of GRE header. */
#define NSS_GRE_CONFIG_OKEY_VALID	0x00000002	/**< Key for outgoing GRE header. */
#define NSS_GRE_CONFIG_ISEQ_VALID	0x00000004	/**< Enable sequence checking for incoming GRE traffic. */
#define NSS_GRE_CONFIG_OSEQ_VALID	0x00000008	/**< Add sequence number for out going GRE packets. */
#define NSS_GRE_CONFIG_ICSUM_VALID	0x00000010	/**< Validate incoming GRE header checksum. */
#define NSS_GRE_CONFIG_OCSUM_VALID	0x00000020	/**< Add checksum header to GRE header. */
#define NSS_GRE_CONFIG_TOS_INHERIT	0x00000040	/**< Inherit inner IP TOS to tunnel header, if not set configure provided TOS. */
#define NSS_GRE_CONFIG_TTL_INHERIT	0x00000080	/**< Inherit inner IP TTL to tunnel header, if not set configure provided TTL. */
#define NSS_GRE_CONFIG_SET_DF		0x00000100	/**< Enable DF bit on tunnel IP header. */
#define NSS_GRE_CONFIG_SET_MAC		0x00000200	/**< Add MAC header to GRE+IP tunnel header. */
#define NSS_GRE_CONFIG_SET_PADDING	0x00000400	/**< Add PADDING to align tunnel IP/GRE header. */
#define NSS_GRE_CONFIG_NEXT_NODE_AVAILABLE  0x00000800	/**< Use provided next node instead of existing next node. */
#define NSS_GRE_CONFIG_COPY_METADATA 	0x00001000	/**< Enable metadata copy in NSS during alignment. */
#define NSS_GRE_CONFIG_USE_UNALIGNED 	0x00002000	/**< Use unaligned infrastructure in NSS. */
#define NSS_GRE_CONFIG_DSCP_VALID	0x00004000	/**< Add DSCP per packet. */

/**
 * nss_gre_error_types.
 *	Error types for GRE configuration messages.
 */
enum nss_gre_error_types {
	NSS_GRE_ERR_UNKNOWN_MSG = 1,		/**< Unknown message. */
	NSS_GRE_ERR_IF_INVALID = 2,		/**< Invalid interface. */
	NSS_GRE_ERR_MODE_INVALID = 3,		/**< Invalid mode type. */
	NSS_GRE_ERR_IP_INVALID = 4,		/**< Invalid IP type. */
	NSS_GRE_ERR_GRE_SESSION_PARAMS_INVALID = 5,	/**< Invalid GRE session parameters provided. */
	NSS_GRE_ERR_DSCP_CFG_INVALID = 6,	/**< Both TOS and DSCP flags are enabled. */
	NSS_GRE_ERR_MAX,			/**< Maximum GRE error. */
};

/**
 * nss_gre_info
 *	GRE private information.
 */
struct nss_gre_info {
	/**
	 * Union of IPv4/IPv6 tunnel.
	 */
	union {
		struct ip_tunnel t4;		/**< IPv4 tunnel. */
		struct ip6_tnl t6;		/**< IPv6 tunnel. */
	} t;		/**< IPv4 and IPv6 tunnel. */
	int nss_if_number_inner;		/**< NSS interface number for GRE inner. */
	struct net_device *next_dev_inner;	/**< Next network device for inner flow. */
	struct net_device *next_dev_outer;	/**< Next network device for outer flow. */
	uint8_t gre_hlen;			/**< GRE header length. */
	uint8_t pad_len;			/**< Pad length. */
};

/**
 * nss_gre_msg_types
 *	Message types for GRE requests and responses.
 */
enum nss_gre_msg_types {
	NSS_GRE_MSG_ENCAP_CONFIGURE = NSS_IF_MAX_MSG_TYPES + 1,
	NSS_GRE_MSG_DECAP_CONFIGURE,
	NSS_GRE_MSG_ENCAP_DECONFIGURE,
	NSS_GRE_MSG_DECAP_DECONFIGURE,
	NSS_GRE_MSG_SESSION_STATS,
	NSS_GRE_MSG_BASE_STATS,
	NSS_GRE_MSG_MAX
};

/**
 * GRE Mode Types
 */
enum nss_gre_mode {
	NSS_GRE_MODE_TUN,	/**< GRE Tunnel interface. */
	NSS_GRE_MODE_TAP,	/**< GRE Tap interface. */
	NSS_GRE_MODE_MAX	/**< Maxmum GRE mode. */
};

/**
 * GRE IP Types
 */
enum nss_gre_ip_types {
	NSS_GRE_IP_IPV4,	/**<  Outer Tunnel is IPV4. */
	NSS_GRE_IP_IPV6,	/**<  Outer Tunnel is IPV6. */
	NSS_GRE_IP_MAX,		/**<  Maximum IP Types. */
};

/**
 * nss_gre_base_types
 *	GRE base debug statistics.
 */
enum nss_gre_base_types {
	NSS_GRE_BASE_RX_PACKETS,		/**< Receive packet count. */
	NSS_GRE_BASE_RX_DROPPED,		/**< Number of packet dropped at receive. */
	NSS_GRE_BASE_EXP_ETH_HDR_MISSING,	/**< Ethernet header missing. */
	NSS_GRE_BASE_EXP_ETH_TYPE_NON_IP,	/**< Packet is not IPV4 or IPV6. */
	NSS_GRE_BASE_EXP_IP_UNKNOWN_PROTOCOL,	/**< Packet protocol is unknown. */
	NSS_GRE_BASE_EXP_IP_HEADER_INCOMPLETE,	/**< Bad IP header. */
	NSS_GRE_BASE_EXP_IP_BAD_TOTAL_LENGTH,	/**< IP total length is invalid. */
	NSS_GRE_BASE_EXP_IP_BAD_CHECKSUM,	/**< IP checksum is bad. */
	NSS_GRE_BASE_EXP_IP_DATAGRAM_INCOMPLETE,/**< Bad packet. */
	NSS_GRE_BASE_EXP_IP_FRAGMENT,		/**< IP packet is a fragment. */
	NSS_GRE_BASE_EXP_IP_OPTIONS_INCOMPLETE,	/**< IP option is invalid. */
	NSS_GRE_BASE_EXP_IP_WITH_OPTIONS,	/**< IP packet with options. */
	NSS_GRE_BASE_EXP_IPV6_UNKNOWN_PROTOCOL,	/**< Protocol is unknown. */
	NSS_GRE_BASE_EXP_IPV6_HEADER_INCOMPLETE,/**< Incomplete ipv6 header. */
	NSS_GRE_BASE_EXP_GRE_UNKNOWN_SESSION,	/**< Unknown GRE session. */
	NSS_GRE_BASE_EXP_GRE_NODE_INACTIVE,	/**< GRE node is inactive. */
	NSS_GRE_BASE_DEBUG_MAX,			/**< GRE base debug maximum. */
};

/**
 * nss_gre_session_types
 *	GRE session packet drop and exception events.
 */
enum nss_gre_session_types {
	NSS_GRE_SESSION_PBUF_ALLOC_FAIL,		/**< Pbuf allocation failure. */
	NSS_GRE_SESSION_DECAP_FORWARD_ENQUEUE_FAIL,	/**< Receive forward enqueue failure. */
	NSS_GRE_SESSION_ENCAP_FORWARD_ENQUEUE_FAIL,	/**< Transmit forward enqueue failure. */
	NSS_GRE_SESSION_DECAP_TX_FORWARDED,		/**< Number of packets forwarded after decapsulation. */
	NSS_GRE_SESSION_ENCAP_RX_RECEIVED,		/**< Number of packets received for encapsulation. */
	NSS_GRE_SESSION_ENCAP_RX_DROPPED,		/**< Packets dropped while enqueuing for encapsulation. */
	NSS_GRE_SESSION_ENCAP_RX_LINEAR_FAIL,		/**< Packets dropped during encapsulation linearization. */
	NSS_GRE_SESSION_EXP_RX_KEY_ERROR,		/**< Receive key error. */
	NSS_GRE_SESSION_EXP_RX_SEQ_ERROR,		/**< Receive Sequence number error. */
	NSS_GRE_SESSION_EXP_RX_CS_ERROR,		/**< Receive checksum error */
	NSS_GRE_SESSION_EXP_RX_FLAG_MISMATCH,		/**< Receive flag mismatch. */
	NSS_GRE_SESSION_EXP_RX_MALFORMED,		/**< Receive packet is malformed. */
	NSS_GRE_SESSION_EXP_RX_INVALID_PROTOCOL,	/**< Receive packet protocol is invalid. */
	NSS_GRE_SESSION_EXP_RX_NO_HEADROOM,		/**< Packet does not have enough headroom. */
	NSS_GRE_SESSION_DEBUG_MAX,			/**< Session debug maximum. */
};

/**
 * GRE create message structure.
 */
struct nss_gre_config_msg {
	uint32_t src_ip[4];			/**< Source IPv4 or IPv6 Adddress. */
	uint32_t dest_ip[4];			/**< Destination IPv4 or IPv6 Adddress. */
	uint32_t flags;				/**< GRE Flags. */
	uint32_t ikey;				/**< GRE rx KEY.*/
	uint32_t okey;				/**< GRE tx KEY. */
	uint32_t mode;				/**< GRE TUN or TAP. */
	uint32_t ip_type;			/**< IPv4 or IPv6 type. */
	uint32_t next_node_if_num;		/**< To whom to forward packets. */
	uint32_t sibling_if_num;        	/**< Sibling interface number. */
	uint16_t src_mac[3];			/**< Source MAC address. */
	uint16_t dest_mac[3];			/**< Destination MAC address. */
	uint8_t ttl;				/**< TTL or HOPLIMIT. */
	uint8_t tos;				/**< Type of service. */
	uint16_t metadata_size;			/**< Metadata copy size. */
};

/**
 * GRE link up message structure
 */
struct nss_gre_linkup_msg {
	int if_number;			/**< Interface number. */
};

/**
 * GRE link down message structure
 */
struct nss_gre_linkdown_msg {
	int if_number;			/**< Interface number. */
};

/**
 * GRE deconfig message structure
 */
struct nss_gre_deconfig_msg {
	int if_number;			/**< Interface number */
};

/**
 * GRE session statistics message
 */
struct nss_gre_session_stats_msg {
	struct nss_cmn_node_stats node_stats;		/**< Common node statistics. */
	uint32_t stats[NSS_GRE_SESSION_DEBUG_MAX];	/**< Session debug statistics. */
};

/**
 * GRE base statistics message
 */
struct nss_gre_base_stats_msg {
	uint32_t stats[NSS_GRE_BASE_DEBUG_MAX];		/**< Base debug statistics. */
};

/**
 * nss_gre_base_stats_notification
 *	GRE transmission statistics structure.
 */
struct nss_gre_base_stats_notification {
	uint64_t stats_base_ctx[NSS_GRE_BASE_DEBUG_MAX];	/**< Base debug transmission statistics. */
	uint32_t core_id;					/**< Core ID. */
};

/**
 * nss_gre_session_stats_notification
 *	GRE transmission statistics structure.
 */
struct nss_gre_session_stats_notification {
	uint64_t stats_session_ctx[NSS_GRE_SESSION_DEBUG_MAX];		/**< Session debug transmission statistics. */
	uint32_t core_id;						/**< Core ID. */
	uint32_t if_num;						/**< Interface number. */
};

/**
 * nss_gre_msg
 *	Message structure to send/receive GRE messages
 */
struct nss_gre_msg {
	struct nss_cmn_msg cm;					/**< Common message header */

	/**
	 * Payload of a GRE message.
	 */
	union {
		struct nss_gre_config_msg cmsg;			/**< GRE session config message. */
		struct nss_gre_deconfig_msg dmsg;		/**< GRE session deconfig message. */
		struct nss_gre_linkup_msg linkup;		/**< GRE link up message. */
		struct nss_gre_linkdown_msg linkdown;		/**< GRE link down message. */
		struct nss_gre_session_stats_msg sstats;	/**< GRE session statistics message. */
		struct nss_gre_base_stats_msg bstats;		/**< Base statistics message. */
	} msg;							/**< Message payload. */
};

/**
 * Callback function to receive GRE messages
 *
 * @datatypes
 * nss_gre_msg
 *
 * @param[in] app_data  Pointer to the application context of the message.
 * @param[in] msg       Pointer to the message data.
 */
typedef void (*nss_gre_msg_callback_t)(void *app_data, struct nss_gre_msg *msg);

/**
 * nss_gre_tx_msg
 *	Sends GRE messages to the NSS.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_gre_msg
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 * @param[in] msg      Pointer to the message data.
 *
 * @return
 * Status of the Tx operation.
 */
extern nss_tx_status_t nss_gre_tx_msg(struct nss_ctx_instance *nss_ctx, struct nss_gre_msg *msg);

/**
 * nss_gre_tx_msg_sync
 *	Sends GRE messages to the NSS synchronously.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_gre_msg
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 * @param[in] msg      Pointer to the message data.
 *
 * @return
 * Status of the Tx operation.
 */
extern nss_tx_status_t nss_gre_tx_msg_sync(struct nss_ctx_instance *nss_ctx, struct nss_gre_msg *msg);

/**
 * nss_gre_tx_buf
 *	Sends packet to the NSS
 *
 * @datatypes
 * nss_ctx_instance \n
 * sk_buff
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 * @param[in] if_num   Nss interface number.
 * @param[in] skb      Pointer to sk_buff.
 *
 * @return Tx status
 */
extern nss_tx_status_t nss_gre_tx_buf(struct nss_ctx_instance *nss_ctx, uint32_t if_num, struct sk_buff *skb);

/**
 * nss_gre_get_context.
 *	Gets the GRE context used in nss_gre_tx.
 *
 * @return
 * Pointer to the NSS core context.
 */
extern struct nss_ctx_instance *nss_gre_get_context(void);

/**
 *
 * nss_gre_ifnum_with_core_id
 * 	Append core ID on GRE interface.
 *
 * @param[in] if_num   NSS interface number.
 *
 * @return
 * GRE interface number with core ID.
 */
extern int nss_gre_ifnum_with_core_id(int if_num);

/**
 * Callback function for receiving GRE session data.
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
typedef void (*nss_gre_data_callback_t)(struct net_device *netdev, struct sk_buff *skb, struct napi_struct *napi);

/**
 * nss_gre_register_if
 *	Registers the GRE interface with the NSS for sending and
 *	receiving messages.
 *
 * @datatypes
 * nss_gre_data_callback_t \n
 * nss_gre_msg_callback_t \n
 * net_device
 *
 * @param[in] if_num         NSS interface number.
 * @param[in] type           NSS interface type.
 * @param[in] gre_callback   Callback for the data.
 * @param[in] msg_callback   Callback for the message.
 * @param[in] netdev         Pointer to the associated network device.
 * @param[in] features       Socket buffer types supported by this interface.
 *
 * @return
 * Pointer to the NSS core context.
 */
extern struct nss_ctx_instance *nss_gre_register_if(uint32_t if_num, uint32_t type, nss_gre_data_callback_t gre_callback,
					nss_gre_msg_callback_t msg_callback, struct net_device *netdev, uint32_t features);

/**
 * nss_gre_unregister_if
 *	Deregisters the GRE interface from the NSS.
 *
 * @param[in] if_num  NSS interface number.
. *
 * @return
 * None.
 *
 * @dependencies
 * The tunnel interface must have been previously registered.
 */
extern void nss_gre_unregister_if(uint32_t if_num);

/**
 * nss_gre_msg_init
 *	Initializes a GRE message.
 *
 * @datatypes
 * nss_gre_msg
 *
 * @param[in,out] ncm       Pointer to the message.
 * @param[in]     if_num    Interface number
 * @param[in]     type      Type of message.
 * @param[in]     len       Size of the payload.
 * @param[in]     cb        Pointer to the message callback.
 * @param[in]     app_data  Pointer to the application context of the message.
 *
 * @return
 * None.
 */
extern void nss_gre_msg_init(struct nss_gre_msg *ncm, uint16_t if_num, uint32_t type,  uint32_t len, void *cb, void *app_data);

/**
 * nss_gre_register_handler
 *	Registers the GRE interface with the NSS debug statistics handler.
 *
 * @return
 * None.
 */
extern void nss_gre_register_handler(void);

/**
 * Callback function for updating stats.
 *
 * @datatypes
 * net_device \n
 * sk_buff \n
 *
 * @param[in] netdev  Pointer to the associated network device.
 * @param[in] skb     Pointer to the data socket buffer.
 *
 * @return
 * None.
 */
typedef void (*nss_gre_pkt_callback_t)(struct net_device *netdev, struct sk_buff *skb);

/**
 * nss_gre_register_pkt_callback
 *	Register for rx packet call back.
 *
 * @datatypes
 * nss_gre_pkt_callback_t
 *
 * @param[in] cb  Call back function which needs to be registered.
 *
 * @return
 * None.
 */
extern void nss_gre_register_pkt_callback(nss_gre_pkt_callback_t cb);

/**
 * nss_gre_unregister_pkt_callback
 *	Unregister for rx packet call back.
 *
 * @datatypes
 * nss_gre_pkt_callback_t
 *
 * @return
 * None.
 */
extern void nss_gre_unregister_pkt_callback(void);

/**
 * nss_gre_stats_unregister_notifier
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
extern int nss_gre_stats_unregister_notifier(struct notifier_block *nb);

/**
 * nss_gre_stats_register_notifier
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
extern int nss_gre_stats_register_notifier(struct notifier_block *nb);

/**
 * @}
 */

#endif /* _NSS_GRE_H_ */

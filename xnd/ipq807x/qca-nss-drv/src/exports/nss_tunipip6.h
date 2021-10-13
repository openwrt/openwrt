/*
 **************************************************************************
 * Copyright (c) 2014, 2017-2018, 2020, The Linux Foundation. All rights reserved.
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
 * @file nss_tunipip6.h
 *	NSS TUNIPIP6 interface definitions.
 */

#ifndef __NSS_TUNIPIP6_H
#define __NSS_TUNIPIP6_H

/**
 * Maximum number of supported TUNIPIP6 tunnels.
 */
#define NSS_TUNIPIP6_TUNNEL_MAX 32

/**
 * @addtogroup nss_tunipip6_subsystem
 * @{
 */

/**
 * nss_tunipip6_map_rule
 *	Mapping rule (FMR/BMR) for forwarding traffic to the node in the same domain.
 */
struct nss_tunipip6_map_rule {
	uint32_t ip6_prefix[4];		/**< An IPv6 prefix assigned by a mapping rule. */
	uint32_t ip4_prefix;		/**< An IPv4 prefix assigned by a mapping rule. */
	uint32_t ip6_prefix_len;	/**< IPv6 prefix length. */
	uint32_t ip4_prefix_len;	/**< IPv4 prefix length. */
	uint32_t ip6_suffix[4];		/**< IPv6 suffix. */
	uint32_t ip6_suffix_len;	/**< IPv6 suffix length. */
	uint32_t ea_len;		/**< Embedded Address (EA) bits. */
	uint32_t psid_offset;		/**< PSID offset default 6. */
};

/*
 * nss_tunipip6_err_types
 * 	Error types for response to messages from the host.
 */
enum nss_tunipip6_err_types {
	NSS_TUNIPIP6_ERR_TYPE_MAX_TUNNELS,		/**< Maximum number of tunnel reached. */
	NSS_TUNIPIP6_ERR_TYPE_TUNNEL_EXIST,		/**< Tunnel already exists. */
	NSS_TUNIPIP6_ERR_TYPE_ENCAP_BAD_PARAM,	/**< Bad configuration. */
	NSS_TUNIPIP6_ERR_TYPE_ENCAP_FMR_EXIST,	/**< FMR already exists. */
	NSS_TUNIPIP6_ERR_TYPE_ENCAP_NO_FMR,		/**< No FMR configured.*/
	NSS_TUNIPIP6_ERR_TYPE_ENCAP_FMR_FULL,	/**< FMR table is full. */
	NSS_TUNIPIP6_ERR_TYPE_ENCAP_INVALID_FMR,	/**< Invalid FMR configured.*/
	NSS_TUNIPIP6_ERR_TYPE_ENCAP_BMR_EXIST,	/**< BMR already exists. */
	NSS_TUNIPIP6_ERR_TYPE_ENCAP_NO_BMR,		/**< No BMR configured. */
	NSS_TUNIPIP6_ERR_TYPE_ENCAP_FMR_MEM_ALLOC_FAILED,	/**< Pool allocation for FMR failed. */
	NSS_TUNIPIP6_ERR_TYPE_UNKNOWN,		/**< Unknown message type. */
	NSS_TUNIPIP6_ERROR_MAX,			/**< Maximum number of errors. */
};

/**
 * nss_tunipip6_metadata_types
 *	Message types for TUNIPIP6 (IPv4 in IPv6) tunnel requests and responses.
 */
enum nss_tunipip6_metadata_types {
	NSS_TUNIPIP6_TX_ENCAP_IF_CREATE,
	NSS_TUNIPIP6_TX_DECAP_IF_CREATE,
	NSS_TUNIPIP6_STATS_SYNC,
	NSS_TUNIPIP6_FMR_RULE_ADD,
	NSS_TUNIPIP6_FMR_RULE_DEL,
	NSS_TUNIPIP6_FMR_RULE_FLUSH,
	NSS_TUNIPIP6_BMR_RULE_ADD,
	NSS_TUNIPIP6_BMR_RULE_DEL,
	NSS_TUNIPIP6_MAX,
};

/**
 * nss_tunipip6_create_msg
 *	Payload for configuring the TUNIPIP6 interface.
 */
struct nss_tunipip6_create_msg {
	uint32_t saddr[4];						/**< Tunnel source address. */
	uint32_t daddr[4];						/**< Tunnel destination address. */
	uint32_t flowlabel;						/**< Tunnel IPv6 flow label. */
	uint32_t flags;							/**< Tunnel additional flags. */
	uint32_t sibling_if_num;					/**< Sibling interface number. */
	uint8_t hop_limit;						/**< Tunnel IPv6 hop limit. */
	uint8_t draft03;						/**< Use MAP-E draft03 specification. */
	uint8_t ttl_inherit;						/**< Inherit IPv4 TTL to hoplimit. */
	uint8_t tos_inherit;						/**< Inherit IPv4 ToS. */
	uint8_t frag_id_update;						/**< Enable update of fragment identifier of IPv4. */
	uint8_t reserved[3];						/**< Reserved bytes. */
	uint32_t fmr_max;						/**< Maximum number of FMRs that can be configured. */
};

/**
 * nss_tunipip6_debug_stats
 * 	TUNIPIP6 debug statistics.
 */
struct nss_tunipip6_debug_stats {
	struct {
		struct {
			uint32_t low_headroom;		/**< Low headroom for encapsulation. */
			uint32_t unhandled_proto;	/**< Unhandled protocol for encapsulation. */
		} exp;

		struct {
			uint32_t enqueue_fail;		/**< Encapsulation enqueue fail. */
		} drop;

		struct {
			uint32_t err_tunnel_cfg;	/**< Tunnel configuration error. */
			uint32_t total_fmr;		/**< Total number of existing FMRs. */
			uint32_t fmr_add_req;		/**< FMR add requests. */
			uint32_t fmr_del_req;		/**< FMR delete requests. */
			uint32_t fmr_flush_req;		/**< FMR flush requests. */
			uint32_t fmr_update_req;	/**< FMR update requests. */
			uint32_t fmr_add_fail;		/**< FMR addition failed. */
			uint32_t fmr_del_fail;		/**< FMR deletion failed. */
			uint32_t err_no_fmr;		/**< No FMR configured. */
			uint32_t bmr_add_req;		/**< BMR add requests. */
			uint32_t bmr_del_req;		/**< BMR delete requests. */
			uint32_t err_bmr_exist;		/**< BMR already configured. */
			uint32_t err_no_bmr;		/**< No BMR configured. */
		} cfg;
	} encap;

	struct {
		struct {
			uint32_t enqueue_fail;		/**< Decapsulation enqueue fail. */
		} drop;
	} decap;
};

/**
 * nss_tunipip6_stats_sync_msg
 *	Message information for TUNIPIP6 synchronization statistics.
 */
struct nss_tunipip6_stats_sync_msg {
	struct nss_cmn_node_stats node_stats;		/**< Common node statistics. */
	struct nss_tunipip6_debug_stats tun_stats;	/**< TUNIPIP6 debug statistics. */
};

/**
 * nss_tunipip6_msg
 *	Data for sending and receiving TUNIPIP6 messages.
 */
struct nss_tunipip6_msg {
	struct nss_cmn_msg cm;			/**< Common message header. */

	/**
	 * Payload of a TUNIPIP6 message.
	 */
	union {
		struct nss_tunipip6_create_msg tunipip6_create;
				/**< Create a TUNIPIP6 tunnel. */
		struct nss_tunipip6_stats_sync_msg stats;
				/**< Synchronized statistics for the TUNIPIP6 interface. */
		struct nss_tunipip6_map_rule map_rule;
				/**< BMR/FMR rule to add/delete, new or existing rules. */
	} msg;			/**< Message payload for TUNIPIP6 messages exchanged with NSS core. */
};

/**
 * Callback function for receiving TUNIPIP6 messages.
 *
 * @datatypes
 * nss_tunipip6_msg
 *
 * @param[in] app_data  Pointer to the application context of the message.
 * @param[in] msg       Pointer to the message data.
 */
typedef void (*nss_tunipip6_msg_callback_t)(void *app_data, struct nss_tunipip6_msg *msg);

/**
 * nss_tunipip6_tx
 *	Sends a TUNIPIP6 message to NSS core.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_tunipip6_msg
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 * @param[in] msg      Pointer to the message data.
 *
 * @return
 * Status of the Tx operation.
 */
extern nss_tx_status_t nss_tunipip6_tx(struct nss_ctx_instance *nss_ctx, struct nss_tunipip6_msg *msg);

/**
 * nss_tunipip6_tx_sync
 *	Sends a TUNIPIP6 message to NSS core synchronously.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_tunipip6_msg
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 * @param[in] msg      Pointer to the message data.
 *
 * @return
 * Status of the Tx operation.
 */
extern nss_tx_status_t nss_tunipip6_tx_sync(struct nss_ctx_instance *nss_ctx, struct nss_tunipip6_msg *msg);

/**
 * Callback function for receiving TUNIPIP6 data.
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
typedef void (*nss_tunipip6_callback_t)(struct net_device *netdev, struct sk_buff *skb, struct napi_struct *napi);

/**
 * nss_register_tunipip6_if
 *	Registers the TUNIPIP6 interface with the NSS for sending and receiving
 *	TUNIPIP6 messages.
 *
 * @datatypes
 * nss_tunipip6_callback_t \n
 * nss_tunipip6_msg_callback_t \n
 * net_device
 *
 * @param[in] if_num             NSS interface number.
 * @param[in] type               Dynamic interface type.
 * @param[in] tunipip6_callback  Callback for the data.
 * @param[in] event_callback     Callback for the message.
 * @param[in] netdev             Pointer to the associated network device.
 * @param[in] features           Data socket buffer types supported by this interface.
 *
 * @return
 * Pointer to the NSS core context.
 */
extern struct nss_ctx_instance *nss_register_tunipip6_if(uint32_t if_num, uint32_t type, nss_tunipip6_callback_t tunipip6_callback,
					nss_tunipip6_msg_callback_t event_callback, struct net_device *netdev, uint32_t features);

/**
 * nss_unregister_tunipip6_if
 *	Deregisters the TUNIPIP6 interface from the NSS.
 *
 * @param[in] if_num  NSS interface number.
 *
 * @return
 * None.
 */
extern void nss_unregister_tunipip6_if(uint32_t if_num);

/**
 * nss_tunipip6_msg_init
 *	Initializes a TUNIPIP6 message.
 *
 * @datatypes
 * nss_tunipip6_msg
 *
 * @param[in,out] ntm       Pointer to the IPIP6 tunnel message.
 * @param[in]     if_num    NSS interface number.
 * @param[in]     type      Type of message.
 * @param[in]     len       Size of the message.
 * @param[in]     cb        Pointer to the message callback.
 * @param[in]     app_data  Pointer to the application context of the message.
 *
 * @return
 * None.
 */
extern void nss_tunipip6_msg_init(struct nss_tunipip6_msg *ntm, uint16_t if_num, uint32_t type,  uint32_t len, void *cb, void *app_data);

/**
 * nss_tunipip6_get_context()
 *	Get TUNIPIP6 context.
 *
 * @return
 * Pointer to the NSS core context.
 */
extern struct nss_ctx_instance *nss_tunipip6_get_context(void);

/** @} */ /* end_addtogroup nss_tunipip6_subsystem */

#endif /* __NSS_TUN6RD_H */

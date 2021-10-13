/*
 **************************************************************************
 * Copyright (c) 2016-2021, The Linux Foundation. All rights reserved.
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
 * @file nss_map_t.h
 *	NSS MAP-T interface definitions.
 */

#ifndef _NSS_MAP_T_H_
#define _NSS_MAP_T_H_

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#include "nss_dynamic_interface.h"
#endif

/**
 * @addtogroup nss_map_t_subsystem
 * @{
 */

/**
 * Maximum number of supported MAP-T instances.
 */
#define NSS_MAX_MAP_T_DYNAMIC_INTERFACES 4

#define NSS_MAPT_MDATA_FLAG_DF_BIT  (1 << 0)

/*
 * mapt meta data
 */
struct nss_map_t_mdata {
	uint16_t flags;
	uint16_t res[6];
};

/**
 * nss_map_t_msg_types
 *	Message types for MAP-T requests and responses.
 */
enum nss_map_t_msg_types {
	NSS_MAP_T_MSG_INSTANCE_RULE_CONFIGURE,
	NSS_MAP_T_MSG_INSTANCE_RULE_DECONFIGURE,
	NSS_MAP_T_MSG_SYNC_STATS,
	NSS_MAP_T_MSG_MAX
};

/**
 * nss_map_t_stats_instance
 *	MAP-T debug error types.
 */
enum nss_map_t_stats_instance {
	NSS_MAP_T_STATS_V4_TO_V6_PBUF_EXCEPTION,
	NSS_MAP_T_STATS_V4_TO_V6_PBUF_NO_MATCHING_RULE,
	NSS_MAP_T_STATS_V4_TO_V6_PBUF_NOT_TCP_OR_UDP,
	NSS_MAP_T_STATS_V4_TO_V6_RULE_ERR_LOCAL_PSID,
	NSS_MAP_T_STATS_V4_TO_V6_RULE_ERR_LOCAL_IPV6,
	NSS_MAP_T_STATS_V4_TO_V6_RULE_ERR_REMOTE_PSID,
	NSS_MAP_T_STATS_V4_TO_V6_RULE_ERR_REMOTE_EA_BITS,
	NSS_MAP_T_STATS_V4_TO_V6_RULE_ERR_REMOTE_IPV6,
	NSS_MAP_T_STATS_V6_TO_V4_PBUF_EXCEPTION,
	NSS_MAP_T_STATS_V6_TO_V4_PBUF_NO_MATCHING_RULE,
	NSS_MAP_T_STATS_V6_TO_V4_PBUF_NOT_TCP_OR_UDP,
	NSS_MAP_T_STATS_V6_TO_V4_RULE_ERR_LOCAL_IPV4,
	NSS_MAP_T_STATS_V6_TO_V4_RULE_ERR_REMOTE_IPV4,
	NSS_MAP_T_STATS_MAX
};

/**
 * nss_map_t_stats_notification
 *	MAP-T statistics structure.
 */
struct nss_map_t_stats_notification {
	uint32_t core_id;				/**< Core ID. */
	uint32_t if_num;				/**< Interface number. */
	enum nss_dynamic_interface_type if_type;	/**< Dynamic interface type. */
	uint64_t stats[NSS_MAP_T_STATS_MAX];		/**< MAP-T statistics. */
};

#ifdef __KERNEL__ /* only kernel will use. */
/**
 * nss_map_t_instance_rule_config_msg
 *	Message information for configuring a MAP-T instance.
 */
struct nss_map_t_instance_rule_config_msg {
	uint32_t rule_num;			/**< Rule sequence number */
	uint32_t total_rules;			/**< Total number of NAT64 rules configured. */
	uint32_t local_ipv6_prefix_len;		/**< Local IPv6 prefix length. */
	uint32_t local_ipv4_prefix;		/**< Local IPv4 prefix. */
	uint32_t local_ipv4_prefix_len;		/**< Local IPv4 prefix length. */
	uint32_t local_ea_len;			/**< Local EA bits length. */
	uint32_t local_psid_offset;		/**< Local PSID offset. */

	uint32_t reserve_a;			/**< Reserved for backward compatibility. */

	uint32_t remote_ipv6_prefix_len;	/**< Remote IPv6 prefix length. */
	uint32_t remote_ipv4_prefix;		/**< Remote IPv4 prefix. */
	uint32_t remote_ipv4_prefix_len;	/**< Remote IPv4 prefix length. */
	uint32_t remote_ea_len;			/**< Remote EA bits length. */
	uint32_t remote_psid_offset;		/**< Remote PSID offset. */

	uint32_t local_map_style;		/**< Local MAP style. */
	uint32_t remote_map_style;		/**< Remote MAP style. */

	uint32_t sibling_if;			/**< Sibling interface number. */

	uint8_t local_ipv6_prefix[16];		/**< Local IPv6 prefix. */
	uint8_t reserve_b[16];			/**< Reserved for backward compatibility. */
	uint8_t remote_ipv6_prefix[16];		/**< Remote IPv6 prefix. */

	uint8_t valid_rule;			/**< MAP-T rule validity. */
	uint8_t flags;				/**< MAP-T flags. */
	uint8_t reserved[2];			/**< Reserved for byte alignment. */
};

/**
 * nss_map_t_instance_rule_deconfig_msg
 *	Message information for deleting a MAP-T instance.
 */
struct nss_map_t_instance_rule_deconfig_msg {
	int32_t if_number;			/**< Interface number. */
};

/**
 * nss_map_t_sync_stats_msg
 *	Message information for MAP-T synchronization statistics.
 */
struct nss_map_t_sync_stats_msg {
	struct nss_cmn_node_stats node_stats;	/**< Common node statistics. */
	uint32_t tx_dropped;			/**< Dropped Tx packets. */

	/**
	 * Debug statistics for MAP-T.
	 */
	union {

		/**
		 * IPv4 to IPv6 path debug statistics.
		 */
		struct {
			uint32_t exception_pkts;
					/**< Number of packets exceptioned to host in IPv4 to IPv6 fast path. */
			uint32_t no_matching_rule;
					/**< No matching of any rule. */
			uint32_t not_tcp_or_udp;
					/**< Number of packets which are neither UDP nor TCP. */
			uint32_t rule_err_local_psid;
					/**< Calculate the local PSID error. */
			uint32_t rule_err_local_ipv6;
					/**< Calculate local IPv6 error. */
			uint32_t rule_err_remote_psid;
					/**< Calculate remote PSID error. */
			uint32_t rule_err_remote_ea_bits;
					/**< Calculate remote EA bits error. */
			uint32_t rule_err_remote_ipv6;
					/**< Calculate remote IPv6 error. */
		} v4_to_v6;	/**< IPv4 to IPv6 debug statistics object. */

		/**
		 * IPv6 to IPv4 path debug statistics.
		 */
		struct {
			uint32_t exception_pkts;
					/**< Number of packets exception to host in IPv6 to IPv4 fast path. */
			uint32_t no_matching_rule;
					/**< No matching of any rule. */
			uint32_t not_tcp_or_udp;
					/**< Number of packets which are neither UDP nor TCP. */
			uint32_t rule_err_local_ipv4;
					/**< Calculate local IPv4 error. */
			uint32_t rule_err_remote_ipv4;
					/**< Calculate remote IPv4 error. */
		} v6_to_v4;	/**< IPv6 to IPv4 debug statistics object */

	} debug_stats;		/**< Payload of debug statistics. */
};

/**
 * nss_map_t_msg
 *	Data for sending and receiving MAP-T messages.
 */
struct nss_map_t_msg {
	struct nss_cmn_msg cm;		/**< Common message header. */

	/**
	 * Payload of a MAP-T message.
	 */
	union {
		struct nss_map_t_instance_rule_config_msg create_msg;
				/**< Create message. */
		struct nss_map_t_instance_rule_deconfig_msg destroy_msg;
				/**< Destroy message. */
		struct nss_map_t_sync_stats_msg stats;
				/**< Statistics message to host. */
	} msg;			/**< Message payload. */
};

/**
 * Callback function for receiving MAP-T messages.
 *
 * @datatypes
 * nss_map_t_msg
 *
 * @param[in] app_data  Pointer to the application context of the message.
 * @param[in] msg       Pointer to the message data.
 */
typedef void (*nss_map_t_msg_callback_t)(void *app_data, struct nss_map_t_msg *msg);

/**
 * nss_map_t_tx
 *	Sends a MAP-T message to the NSS.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_map_t_msg
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 * @param[in] msg      Pointer to the message data.
 *
 * @return
 * Status of the Tx operation.
 */
extern nss_tx_status_t nss_map_t_tx(struct nss_ctx_instance *nss_ctx, struct nss_map_t_msg *msg);

/**
 * nss_map_t_tx_sync
 *	Sends a MAP-T message synchronously to the NSS.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_map_t_msg
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 * @param[in] msg      Pointer to the message data.
 *
 * @return
 * Status of the Tx operation.
 */
extern nss_tx_status_t nss_map_t_tx_sync(struct nss_ctx_instance *nss_ctx, struct nss_map_t_msg *msg);

/**
 * nss_map_t_get_context
 *	Gets the MAP-T context used in nss_map_t_tx.
 *
 * @return
 * Pointer to the NSS core context.
 */
extern struct nss_ctx_instance *nss_map_t_get_context(void);

/**
 * Callback function for receiving MAP-T tunnel data.
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
typedef void (*nss_map_t_callback_t)(struct net_device *netdev, struct sk_buff *skb, struct napi_struct *napi);

/**
 * nss_map_t_register_if
 *	Registers a MAP-T interface with the NSS for sending and receiving tunnel messages.
 *
 * @datatypes
 * nss_map_t_callback_t \n
 * nss_map_t_msg_callback_t \n
 * net_device
 *
 * @param[in] if_num          NSS interface number.
 * @param[in] type            NSS interface type.
 * @param[in] map_t_callback  Callback for the MAP-T data.
 * @param[in] msg_callback    Callback for the MAP-T message.
 * @param[in] netdev          Pointer to the associated network device.
 * @param[in] features        Data socket buffer types supported by this interface.
 *
 * @return
 * Pointer to the NSS core context.
 */
extern struct nss_ctx_instance *nss_map_t_register_if(uint32_t if_num, uint32_t type, nss_map_t_callback_t map_t_callback,
					nss_map_t_msg_callback_t msg_callback, struct net_device *netdev, uint32_t features);

/**
 * nss_map_t_unregister_if
 *	Deregisters a MAP-T tunnel interface from the NSS.
 *
 * @param[in] if_num  NSS interface number
 *
 * @return
 * None.
 */
extern void nss_map_t_unregister_if(uint32_t if_num);

/**
 * nss_map_t_msg_init
 *	Initializes a MAP-T message.
 *
 * @datatypes
 * nss_map_t_msg_init
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
extern void nss_map_t_msg_init(struct nss_map_t_msg *ncm, uint16_t if_num, uint32_t type,  uint32_t len, void *cb, void *app_data);

/**
 * nss_map_t_register_handler
 *	Registers the MAP-T debug statistics handler with the NSS.
 *
 * @return
 * None.
 */
extern void nss_map_t_register_handler(void);

/**
 * nss_map_t_instance_debug_stats_get
 *	Gets debug statistics for a MAP-T instance.
 *
 * @param[out] stats_mem  Pointer to the memory address, which must be large enough to
                         hold all the statistics.
 *
 * @return
 * None.
 */
extern void nss_map_t_instance_debug_stats_get(void *stats_mem);

/**
 * nss_map_t_stats_register_notifier
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
extern int nss_map_t_stats_register_notifier(struct notifier_block *nb);

/**
 * nss_map_t_stats_unregister_notifier
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
extern int nss_map_t_stats_unregister_notifier(struct notifier_block *nb);
#endif /*__KERNEL__ */

/**
 * @}
 */

#endif /* _NSS_MAP_T_H_ */

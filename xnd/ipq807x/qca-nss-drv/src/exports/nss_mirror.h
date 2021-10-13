/*
 **************************************************************************
 * Copyright (c) 2020-2021, The Linux Foundation. All rights reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all copies.
 *
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
 * @file nss_mirror.h
 *	NSS mirror interface definitions.
 */

#ifndef _NSS_MIRROR_H_
#define _NSS_MIRROR_H_

/**
 * @addtogroup nss_mirror_subsystem
 * @{
 */

/**
 * Maximum number of supported mirror interfaces.
 */
#define NSS_MAX_MIRROR_DYNAMIC_INTERFACES 8

/**
 * nss_mirror_msg_types
 *	Message types for mirror interface requests and responses.
 */
enum nss_mirror_msg_types {
	NSS_MIRROR_MSG_CONFIGURE,		/**< Configure message type. */
	NSS_MIRROR_MSG_ENABLE,			/**< Enable message type. */
	NSS_MIRROR_MSG_DISABLE,			/**< Disable message type. */
	NSS_MIRROR_MSG_SET_NEXTHOP,		/**< Set nexthop message type. */
	NSS_MIRROR_MSG_RESET_NEXTHOP,		/**< Reset nexthop message type. */
	NSS_MIRROR_MSG_SYNC_STATS,		/**< Statistics synchronization message type. */
	NSS_MIRROR_MSG_MAX			/**< Maximum message type. */
};

/**
 * nss_mirror_pkt_clone_point
 *	Clone point to use for mirroring the packet.
 */
enum nss_mirror_pkt_clone_point {
	NSS_MIRROR_PKT_CLONE_POINT_DEFAULT = 1,			/**< Clone the packet from the start. */
	NSS_MIRROR_PKT_CLONE_POINT_BEFORE_PACKET_START,		/**< Clone n-bytes before packet start. */
	NSS_MIRROR_PKT_CLONE_POINT_AFTER_PACKET_START,		/**< Clone n-bytes after packet start. */
	NSS_MIRROR_PKT_CLONE_POINT_MAX
};

/**
 * nss_mirror_error_type
 *	Error types for mirror responses to messages from the host.
 */
enum nss_mirror_error_type {
	NSS_MIRROR_ERROR_TYPE_NONE,			/**< No error. */
	NSS_MIRROR_ERROR_TYPE_NO_MEMORY,		/**< No memory to copy. */
	NSS_MIRROR_ERROR_TYPE_TX_FAILURE,		/**< Transmit failure. */
	NSS_MIRROR_ERROR_TYPE_BAD_PARAM,		/**< Bad parameter. */
	NSS_MIRROR_ERROR_TYPE_BAD_CLONE_POINT,		/**< Bad packet clone point. */
	NSS_MIRROR_ERROR_TYPE_INSTANCE_CONFIGURED,	/**< Instance already active. */
	NSS_MIRROR_ERROR_TYPE_INSTANCE_DISABLED,	/**< Instance already disabled. */
	NSS_MIRROR_ERROR_TYPE_BAD_NEXTHOP,		/**< Incorrect nexthop interface. */
	NSS_MIRROR_ERROR_TYPE_NEXTHOP_CONFIGURED,	/**< Nexthop already interface. */
	NSS_MIRROR_ERROR_TYPE_NEXTHOP_RESET,		/**< Nexthop already reset. */
	NSS_MIRROR_ERROR_TYPE_UNKNOWN_MSG,		/**< Unknown message. */
	NSS_MIRROR_ERROR_TYPE_MAX,			/**< Maximum message type. */
};

/**
 * nss_mirror_stats
 *	Mirror interface debug statistics.
 */
enum nss_mirror_stats {
	NSS_MIRROR_STATS_PKTS,			/**< Number of packets exceptioned to host. */
	NSS_MIRROR_STATS_BYTES,			/**< Number of bytes exceptioned to host. */
	NSS_MIRROR_STATS_TX_SEND_FAIL,		/**< Transmit send failures. */
	NSS_MIRROR_STATS_DEST_LOOKUP_FAIL,	/**< Destination lookup failures. */
	NSS_MIRROR_STATS_MEM_ALLOC_FAIL,	/**< Memory allocation failures. */
	NSS_MIRROR_STATS_COPY_FAIL,		/**< Copy failures. */
	NSS_MIRROR_STATS_MAX			/**< Maximum statistics count. */
};

/**
 * nss_mirror_configure_msg
 *	Mirror interface configuration information.
 */
struct nss_mirror_configure_msg {
	uint32_t pkt_clone_point;	/**< Point in the packet to copy from. */
	uint16_t pkt_clone_size;	/**< Number of bytes to copy. */
	uint16_t pkt_clone_offset;	/**< Copy offset. */
};

/**
 * nss_mirror_set_nexthop_msg
 *	Mirror interface set nexthop information.
 */
struct nss_mirror_set_nexthop_msg {
	uint32_t if_num;		/**< Nexthop interface number. */
};

/**
 * nss_mirror_node_stats
 *	Mirror interface debug statistics structure.
 */
struct nss_mirror_node_stats {
	uint32_t mirror_pkts;		/**< Number of packets exceptioned to host. */
	uint32_t mirror_bytes;		/**< Number of bytes exceptioned to host. */
	uint32_t tx_send_fail;		/**< Transmit send failures. */
	uint32_t dest_lookup_fail;	/**< Destination lookup failures. */
	uint32_t mem_alloc_fail;	/**< Memory allocation failures. */
	uint32_t copy_fail;		/**< Copy failures. */
	uint32_t bad_param;		/**< Bad parameter. */
};

/**
 * nss_mirror_stats_sync_msg
 *	Message information for mirror interface synchronization statistics.
 */
struct nss_mirror_stats_sync_msg {
	struct nss_cmn_node_stats node_stats;	/**< Common node statistics. */
	struct nss_mirror_node_stats mirror_stats;	/**< Debug statistics for mirror. */
};

/**
 * nss_mirror_stats_notification
 *	Mirror transmission statistics structure.
 */
struct nss_mirror_stats_notification {
	uint64_t stats_ctx[NSS_MIRROR_STATS_MAX];	/**< Context transmission statistics. */
	uint32_t core_id;				/**< Core ID. */
	uint32_t if_num;				/**< Interface number. */
};

/**
 * nss_mirror_msg
 *	Data for sending and receiving mirror interface messages.
 */
struct nss_mirror_msg {
	struct nss_cmn_msg cm;		/**< Common message header. */

	/**
	 * Payload of a mirror interface message.
	 */
	union {
		struct nss_mirror_configure_msg config;
				/**< Mirror interface configure message. */
		struct nss_mirror_set_nexthop_msg nexthop;
				/**< Mirror interface set nexthop message. */
		struct nss_mirror_stats_sync_msg stats;
				/**< Statistics message to host. */
	} msg;			/**< Message payload. */
};

/**
 * Callback function for receiving mirror instance data.
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
typedef void (*nss_mirror_data_callback_t)(struct net_device *netdev, struct sk_buff *skb, struct napi_struct *napi);

/**
 * Callback function for receiving mirror interface messages.
 *
 * @datatypes
 * nss_cmn_msg
 *
 * @param[in] app_data  Pointer to the application context of the message.
 * @param[in] msg       Pointer to the message data.
 */
typedef void (*nss_mirror_msg_callback_t)(void *app_data, struct nss_cmn_msg *msg);

/**
 * nss_mirror_get_context
 *	Gets the mirror interface context.
 *
 * @return
 * Pointer to the NSS core context.
 */
extern struct nss_ctx_instance *nss_mirror_get_context(void);

/**
 * nss_mirror_tx_msg
 *	Sends mirror interface messages to the NSS.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_mirror_msg
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 * @param[in] msg      Pointer to the message data.
 *
 * @return
 * Status of the Tx operation.
 */
extern nss_tx_status_t nss_mirror_tx_msg(struct nss_ctx_instance *nss_ctx, struct nss_mirror_msg *msg);

/**
 * nss_mirror_tx_msg_sync
 *	Sends a mirror interface message to the NSS synchronously.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_mirror_msg
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 * @param[in] msg      Pointer to the message data.
 *
 * @return
 * Status of the Tx operation.
 */
extern nss_tx_status_t nss_mirror_tx_msg_sync(struct nss_ctx_instance *nss_ctx, struct nss_mirror_msg *msg);

/**
 * nss_mirror_unregister_if
 *	Deregisters a mirror interface from the NSS.
 *
 * @param[in] if_num  NSS interface number.
 *
 * @return
 * None.
 */
extern void nss_mirror_unregister_if(uint32_t if_num);

/**
 * nss_mirror_register_if
 *	Registers a mirror interface with the NSS for sending and receiving messages.
 *
 * @datatypes
 * nss_mirror_data_callback_t \n
 * nss_mirror_msg_callback_t \n
 * net_device
 *
 * @param[in] if_num          NSS interface number.
 * @param[in] data_callback   Callback for the mirror interface data.
 * @param[in] event_callback    Callback for the mirror interface message.
 * @param[in] netdev          Pointer to the associated network device.
 * @param[in] features        Data socket buffer types supported by this interface.
 *
 * @return
 * Pointer to the NSS core context.
 */
extern struct nss_ctx_instance *nss_mirror_register_if(uint32_t if_num,
		nss_mirror_data_callback_t data_callback,
		nss_mirror_msg_callback_t event_callback,
		struct net_device *netdev, uint32_t features);

/**
 * nss_mirror_verify_if_num
 *	Verify whether the interface is an mirror interface or not.
 *
 * @param[in] if_num  NSS interface number.
 *
 * @return
 * True if the interface is an mirror interface.
 */
extern bool nss_mirror_verify_if_num(uint32_t if_num);

/**
 * nss_mirror_register_handler
 *	Registers the mirror interface debug statistics handler with the NSS.
 *
 * @return
 * None.
 */
extern void nss_mirror_register_handler(void);

/**
 * nss_mirror_stats_unregister_notifier
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
extern int nss_mirror_stats_unregister_notifier(struct notifier_block *nb);

/**
 * nss_mirror_stats_register_notifier
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
extern int nss_mirror_stats_register_notifier(struct notifier_block *nb);

/**
 * @}
 */
#endif

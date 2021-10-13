/*
 **************************************************************************
 * Copyright (c) 2019, 2021, The Linux Foundation. All rights reserved.
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
 * @file nss_gre_redir_mark.h
 *	NSS GRE Redirect mark interface definitions.
 */

#ifndef __NSS_GRE_REDIR_MARK_H
#define __NSS_GRE_REDIR_MARK_H

#define NSS_GRE_REDIR_MARK_HLOS_MAGIC 0xaade	/**< Metadata magic set by HLOS. */
#define NSS_GRE_REDIR_MARK_NSS_MAGIC 0xaadf	/**< Metadata magic set by NSS. */

extern struct nss_gre_redir_mark_stats gre_mark_stats;

/**
 * nss_gre_redir_mark messages
 *	Message types for GRE redirect mark requests and responses.
 */
enum nss_gre_redir_mark_msg_types {
	NSS_GRE_REDIR_MARK_REG_CB_MSG,		/**< Register callback configuration message. */
	NSS_GRE_REDIR_MARK_STATS_SYNC_MSG,	/**< Statistics synchronization message. */
	NSS_GRE_REDIR_MARK_MSG_MAX,		/**< Maximum message type. */
};

/**
 * nss_gre_redir_mark errors
 *	Error codes for GRE redirect mark configuration message.
 */
enum nss_gre_redir_mark_error_types {
	NSS_GRE_REDIR_MARK_ERROR_NONE,		/**< Configuration successful. */
	NSS_GRE_REDIR_MARK_ERROR_INV_IF_NUM,	/**< Invalid interface number for callback registration. */
	NSS_GRE_REDIR_MARK_ERROR_INV_ETH_TYPE,	/**< Invalid Ethernet type for the destination interface. */
	NSS_GRE_REDIR_MARK_ERROR_TYPE_MAX
};

/**
 * nss_gre_redir_mark_stats_types
 *	GRE redirect mark statistics types.
 */
enum nss_gre_redir_mark_stats_types {
	NSS_GRE_REDIR_MARK_STATS_HLOS_MAGIC_FAILED = NSS_STATS_NODE_MAX,
							/**< HLOS magic fail count. */
	NSS_GRE_REDIR_MARK_STATS_INV_DST_IF_DROPS,	/**< Invalid transmit interface drop count. */
	NSS_GRE_REDIR_MARK_STATS_DST_IF_ENQUEUE,	/**< Next egress interface enqueue success count. */
	NSS_GRE_REDIR_MARK_STATS_DST_IF_ENQUEUE_DROPS,	/**< Next egress interface enqueue drop count. */
	NSS_GRE_REDIR_MARK_STATS_INV_APPID,		/**< Invalid application ID for the transmit completion packets. */
	NSS_GRE_REDIR_MARK_STATS_HEADROOM_UNAVAILABLE,	/**< Packet headroom unavailable to write metadata. */
	NSS_GRE_REDIR_MARK_STATS_TX_COMPLETION_SUCCESS,	/**< Transmit completion host enqueue success count. */
	NSS_GRE_REDIR_MARK_STATS_TX_COMPLETION_DROPS,	/**< Transmit completion host enqueue drop count. */
	NSS_GRE_REDIR_MARK_STATS_MAX			/**< Maximum statistics type. */
};

/**
 * nss_gre_redir_mark_metadata
 *	HLOS to NSS per packet downstream metadata.
 */
struct nss_gre_redir_mark_metadata {
	uint32_t dst_ifnum;		/**< Destination Tx interface number. */
	uint8_t wifi_tid;		/**< TID value. */
	uint8_t app_id;			/**< Application ID. */
	uint16_t hw_hash_idx;		/**< Hardware AST hash index value. */
	uint32_t tx_status;		/**< Transmit status. */
	uint16_t offset;		/**< Buffer offset from the metadata. */
	uint16_t magic;			/**< Metadata magic. */
};

/**
 * nss_gre_redir_mark_stats
 *	GRE redirect mark statistics.
 */
struct nss_gre_redir_mark_stats_sync_msg {
	struct nss_cmn_node_stats node_stats;	/**< Common node statistics. */
	uint32_t hlos_magic_fail;               /**< HLOS magic fail count. */
	uint32_t invalid_dst_drop;		/**< Invalid transmit interface drop count. */
	uint32_t dst_enqueue_success;		/**< Next egress interface enqueue success count. */
	uint32_t dst_enqueue_drop;		/**< Next egress interface enqueue drop count. */
	uint32_t inv_appid;			/**< Invalid application ID for the transmit completion packets. */
	uint32_t headroom_unavail;		/**< Packet headroom unavailable to write metadata. */
	uint32_t tx_completion_success;		/**< Transmit completion host enqueue success count. */
	uint32_t tx_completion_drop;		/**< Transmit completion host enqueue drop count. */
};

/**
 * nss_gre_redir_mark_register_cb_msg
 *	Transmit completion function register configuration message.
 */
struct nss_gre_redir_mark_register_cb_msg {
	uint32_t nss_if_num;	/**< NSS transmit interface number on which callback needs to be registered. */
};

/**
 * nss_gre_redir_mark_stats_notification
 *	GRE redirect mark transmission statistics structure.
 */
struct nss_gre_redir_mark_stats_notification {
	uint64_t stats_ctx[NSS_GRE_REDIR_MARK_STATS_MAX];	/**< Context transmission statistics. */
	uint32_t core_id;					/**< Core ID. */
	uint32_t if_num;					/**< Interface number. */
};

/**
 * nss_gre_redir_mark_msg
 *	Structure that describes the interface message.
 */
struct nss_gre_redir_mark_msg {
	struct nss_cmn_msg cm;		/**< Common message. */

	/**
	 * Payload of a GRE redirect mark message.
	 */
	union {
		struct nss_gre_redir_mark_register_cb_msg reg_cb_msg;
					/**< Configuration message to register for callback on completion. */
		struct nss_gre_redir_mark_stats_sync_msg stats_sync;
					/**< Mark node statistics synchronization. */
	} msg;				/**< Message payload for GRE redirect mark messages exchanged with NSS core. */
};

/**
 * Callback function for receiving GRE redirect mark data.
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
typedef void (*nss_gre_redir_mark_data_callback_t)(struct net_device *netdev, struct sk_buff *skb, struct napi_struct *napi);

/**
 * Callback function for receiving GRE redirect mark messages.
 *
 * @datatypes
 * nss_cmn_msg
 *
 * @param[in] app_data  Pointer to the application context of the message.
 * @param[in] msg       Pointer to the message data.
 */
typedef void (*nss_gre_redir_mark_msg_callback_t)(void *app_data, struct nss_cmn_msg *msg);

/**
 * nss_gre_redir_mark_unregister_if
 *      Deregisters a GRE redirect mark interface from the NSS.
 *
 * @param[in] if_num  GRE redirect mark interface number.
 *
 * @return
 * None.
 *
 * @dependencies
 * The GRE redirect mark interface must have been previously registered.
 *
 * @return
 * True if successful, else false.
 */
extern bool nss_gre_redir_mark_unregister_if(uint32_t if_num);

/**
 * nss_gre_redir_mark_tx_buf
 *      Sends data buffers to NSS firmware asynchronously.
 *
 * @datatypes
 * nss_ctx_instance \n
 * sk_buff
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 * @param[in] os_buf   Pointer to the OS buffer (e.g. skbuff).
 * @param[in] if_num   GRE redirect mark interface number.
 *
 * @return
 * Status of the Tx operation.
 */
extern nss_tx_status_t nss_gre_redir_mark_tx_buf(struct nss_ctx_instance *nss_ctx, struct sk_buff *os_buf,
		uint32_t if_num);

/**
 * nss_gre_redir_mark_reg_cb
 *	Configure a callback on VAP for downstream application flows.
 *
 * @datatypes
 * nss_gre_redir_mark_register_cb_msg
 *
 * @param[in] ifnum  NSS interface number.
 * @param[in] ngrcm  Downstream application callback registration message.
 *
 * @return
 * Status of Tx operation.
 */
extern nss_tx_status_t nss_gre_redir_mark_reg_cb(int ifnum,
                struct nss_gre_redir_mark_register_cb_msg *ngrcm);

/**
 * nss_gre_redir_mark_tx_msg
 *	Sends GRE redirect mark messages.
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
extern nss_tx_status_t nss_gre_redir_mark_tx_msg(struct nss_ctx_instance *nss_ctx, struct nss_gre_redir_mark_msg *msg);

/**
 * nss_gre_redir_mark_tx_msg_sync
 *	Sends messages to NSS firmware synchronously.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_gre_redir_mark_msg
 *
 * @param[in] nss_ctx  NSS core context.
 * @param[in] ngrm     Pointer to GRE redirect mark message data.
 *
 * @return
 * Status of Tx operation.
 */
extern nss_tx_status_t nss_gre_redir_mark_tx_msg_sync(struct nss_ctx_instance *nss_ctx, struct nss_gre_redir_mark_msg *ngrm);

/**
 * nss_gre_redir_mark_stats_get
 *      Gets GRE redirect mark statistics.
 *
 * @datatypes
 * nss_gre_redir_mark_stats
 *
 * @param[out] stats  Pointer to the memory address, which must be large enough to
 *                    hold all the statistics.
 *
 * @return
 * TRUE or FALSE.
 */
extern bool nss_gre_redir_mark_stats_get(struct nss_gre_redir_mark_stats *stats);

/**
 * nss_gre_redir_alloc_and_register_node
 *      Registers GRE redirect mark static node with NSS.
 *
 * @datatypes
 * net_device \n
 * nss_gre_redir_mark_data_callback_t \n
 * nss_gre_redir_mark_msg_callback_t
 *
 * @param[in] netdev       Pointer to the associated network device.
 * @param[in] if_num       NSS interface number.
 * @param[in] cb_func_data Callback for the data.
 * @param[in] cb_func_msg  Callback for the message.
 * @param[in] features     Data socket buffer types supported by this interface.
 *
 * @return
 * NSS interface number allocated.
 */
extern struct nss_ctx_instance *nss_gre_redir_mark_register_if(struct net_device *netdev, uint32_t if_num,
		nss_gre_redir_mark_data_callback_t cb_func_data, nss_gre_redir_mark_msg_callback_t cb_func_msg,
		uint32_t features);

/**
 * nss_gre_redir_mark_get_context
 *	Gets the GRE redirect mark context.
 *
 * @return
 * Pointer to the NSS core context.
 */
extern struct nss_ctx_instance *nss_gre_redir_mark_get_context(void);

/**
 * nss_gre_redir_mark_get_dentry
 *	Returns directory entry created in debug filesystem for statistics.
 *
 * @return
 * Pointer to created directory entry for GRE redirect mark.
 */
extern struct dentry *nss_gre_redir_mark_get_dentry(void);

/*
 * nss_gre_redir_mark_get_device
 *	Gets the original device from probe.
 *
 * @return
 * Pointer to the device.
 */
extern struct device *nss_gre_redir_mark_get_device(void);

/**
 * nss_gre_redir_mark_stats_unregister_notifier
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
extern int nss_gre_redir_mark_stats_unregister_notifier(struct notifier_block *nb);

/**
 * nss_gre_redir_mark_stats_register_notifier
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
extern int nss_gre_redir_mark_stats_register_notifier(struct notifier_block *nb);

/**
 * @}
 */

#endif /* __NSS_GRE_REDIR_MARK_H */

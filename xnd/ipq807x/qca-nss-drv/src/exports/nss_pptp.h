/*
 **************************************************************************
 * Copyright (c) 2015-2020, The Linux Foundation. All rights reserved.
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
 * @file nss_pptp.h
 *	NSS PPTP interface definitions.
 */

#ifndef _NSS_PPTP_H_
#define _NSS_PPTP_H_

#include "nss_dynamic_interface.h"

/**
 * @addtogroup nss_pptp_subsystem
 * @{
 */

/**
 * Maximum number of supported PPTP sessions is 4.
 * Number of dynamic intefaces per session is 3.
 * Total 4 * 3 = 12
 */
#define NSS_MAX_PPTP_DYNAMIC_INTERFACES 12

/**
 * nss_pptp_metadata_types
 *	Message types for PPTP requests and responses.
 */
enum nss_pptp_metadata_types {
	NSS_PPTP_MSG_SESSION_CONFIGURE,
	NSS_PPTP_MSG_SESSION_DECONFIGURE,
	NSS_PPTP_MSG_SYNC_STATS,
	NSS_PPTP_MSG_MAX
};

/**
 * nss_pptp_exception_events
 *	Exception events for PPTP encapsulation and decapsulation packets.
 */
enum nss_pptp_exception_events {
	PPTP_EXCEPTION_EVENT_ENCAP_HEADROOM_ERR,
	PPTP_EXCEPTION_EVENT_ENCAP_SMALL_SIZE,
	PPTP_EXCEPTION_EVENT_ENCAP_PNODE_ENQUEUE_FAIL,
	PPTP_EXCEPTION_EVENT_DECAP_NO_SEQ_NOR_ACK,
	PPTP_EXCEPTION_EVENT_DECAP_INVAL_GRE_FLAGS,
	PPTP_EXCEPTION_EVENT_DECAP_INVAL_GRE_PROTO,
	PPTP_EXCEPTION_EVENT_DECAP_WRONG_SEQ,
	PPTP_EXCEPTION_EVENT_DECAP_INVAL_PPP_HDR,
	PPTP_EXCEPTION_EVENT_DECAP_PPP_LCP,
	PPTP_EXCEPTION_EVENT_DECAP_UNSUPPORTED_PPP_PROTO,
	PPTP_EXCEPTION_EVENT_DECAP_PNODE_ENQUEUE_FAIL,
	PPTP_EXCEPTION_EVENT_MAX,
};

/**
 * nss_pptp_stats_session
 * 	PPTP debug statistics.
 */
enum nss_pptp_stats_session {
	NSS_PPTP_STATS_ENCAP_RX_PACKETS,
	NSS_PPTP_STATS_ENCAP_RX_BYTES,
	NSS_PPTP_STATS_ENCAP_TX_PACKETS,
	NSS_PPTP_STATS_ENCAP_TX_BYTES,
	NSS_PPTP_STATS_ENCAP_RX_QUEUE_0_DROP,
	NSS_PPTP_STATS_ENCAP_RX_QUEUE_1_DROP,
	NSS_PPTP_STATS_ENCAP_RX_QUEUE_2_DROP,
	NSS_PPTP_STATS_ENCAP_RX_QUEUE_3_DROP,
	NSS_PPTP_STATS_DECAP_RX_PACKETS,
	NSS_PPTP_STATS_DECAP_RX_BYTES,
	NSS_PPTP_STATS_DECAP_TX_PACKETS,
	NSS_PPTP_STATS_DECAP_TX_BYTES,
	NSS_PPTP_STATS_DECAP_RX_QUEUE_0_DROP,
	NSS_PPTP_STATS_DECAP_RX_QUEUE_1_DROP,
	NSS_PPTP_STATS_DECAP_RX_QUEUE_2_DROP,
	NSS_PPTP_STATS_DECAP_RX_QUEUE_3_DROP,
	NSS_PPTP_STATS_SESSION_ENCAP_HEADROOM_ERR,
	NSS_PPTP_STATS_SESSION_ENCAP_SMALL_SIZE,
	NSS_PPTP_STATS_SESSION_ENCAP_PNODE_ENQUEUE_FAIL,
	NSS_PPTP_STATS_SESSION_DECAP_NO_SEQ_NOR_ACK,
	NSS_PPTP_STATS_SESSION_DECAP_INVAL_GRE_FLAGS,
	NSS_PPTP_STATS_SESSION_DECAP_INVAL_GRE_PROTO,
	NSS_PPTP_STATS_SESSION_DECAP_WRONG_SEQ,
	NSS_PPTP_STATS_SESSION_DECAP_INVAL_PPP_HDR,
	NSS_PPTP_STATS_SESSION_DECAP_PPP_LCP,
	NSS_PPTP_STATS_SESSION_DECAP_UNSUPPORTED_PPP_PROTO,
	NSS_PPTP_STATS_SESSION_DECAP_PNODE_ENQUEUE_FAIL,
	NSS_PPTP_STATS_SESSION_MAX
};

/**
 * nss_pptp_stats_notification
 *	PPTP statistics structure.
 */
struct nss_pptp_stats_notification {
	uint32_t core_id;				/**< Core ID. */
	uint32_t if_num;				/**< Interface number. */
	enum nss_dynamic_interface_type if_type;	/**< Dynamic interface type. */
	uint64_t stats[NSS_PPTP_STATS_SESSION_MAX];	/**< PPTP statistics. */
};

/**
 * nss_pptp_session_configure_msg
 *	Message information for configuring a PPTP session.
 */
struct nss_pptp_session_configure_msg {
	uint16_t src_call_id;		/**< Local call ID for caller or callee. */
	uint16_t dst_call_id;		/**< Peer call ID for caller or callee. */
	uint32_t sip;			/**< Local tunnel endpoint. */
	uint32_t dip;			/**< Remote tunnel endpoint. */
	uint32_t sibling_ifnum_pri;	/**< Primary sibling interface. */
	uint32_t sibling_ifnum_aux;	/**< Auxiliary sibling interface. */
};

/**
 * nss_pptp_session_deconfigure_msg
 *	Message information for deleting a PPTP session.
 */
struct nss_pptp_session_deconfigure_msg {
	uint16_t src_call_id;		/**< Local call ID */
};

/**
 * nss_pptp_sync_session_stats_msg
 *	Message information for PPTP synchronization statistics.
 */
struct nss_pptp_sync_session_stats_msg {
	struct nss_cmn_node_stats node_stats;
			/**< Common node statistics for the encapsulation direction. */
	uint32_t exception_events[PPTP_EXCEPTION_EVENT_MAX];
			/**< Statistics of events which casued packets to exception to host. */
};

/**
 * nss_pptp_msg
 *	Data for sending and receiving PPTP messages.
 */
struct nss_pptp_msg {
	struct nss_cmn_msg cm;		/**< Common message header. */

	/**
	 * Payload of a PPTP message.
	 */
	union {
		struct nss_pptp_session_configure_msg session_configure_msg;
				/**< Session configuration message. */
		struct nss_pptp_session_deconfigure_msg session_deconfigure_msg;
				/**< Session de-configuration message. */
		struct nss_pptp_sync_session_stats_msg stats;
				/**< Session statistics message. */
	} msg;			/**< Message payload. */
};

/**
 * Callback function for receiving PPTP messages.
 *
 * @datatypes
 * nss_pptp_msg
 *
 * @param[in] app_data  Pointer to the application context of the message.
 * @param[in] msg       Pointer to the message data.
 */
typedef void (*nss_pptp_msg_callback_t)(void *app_data, struct nss_pptp_msg *msg);

/**
 * nss_pptp_tx_msg_sync
 *	Sends a PPTP message synchronously to NSS.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_pptp_msg
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 * @param[in] msg      Pointer to the message data.
 *
 * @return
 * Status of the Tx operation.
 */
extern nss_tx_status_t nss_pptp_tx_msg_sync(struct nss_ctx_instance *nss_ctx,
					    struct nss_pptp_msg *msg);

/**
 * nss_pptp_tx_buf
 *	Sends a data packet to the firmware.
 *
 * @datatypes
 * nss_ctx_instance \n
 * sk_buff
 *
 * @param[in]     nss_ctx  Pointer to the NSS context.
 * @param[in]     if_num   NSS interface number.
 * @param[in]     skb      Pointer to the data socket buffer.
 *
 * @return
 * Status of the Tx operation.
 */
extern nss_tx_status_t nss_pptp_tx_buf(struct nss_ctx_instance *nss_ctx, uint32_t if_num, struct sk_buff *skb);

/**
 * nss_pptp_get_context
 *	Gets the PPTP context used in nss_pptp_tx.
 *
 * @return
 * Pointer to the NSS core context.
 */
extern struct nss_ctx_instance *nss_pptp_get_context(void);

/**
 * Callback function for receiving PPTP tunnel data.
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
typedef void (*nss_pptp_callback_t)(struct net_device *netdev, struct sk_buff *skb, struct napi_struct *napi);

/**
 * nss_register_pptp_if
 *	Registers the PPTP tunnel interface with the NSS for sending and
 *	receiving messages.
 *
 * @datatypes
 * nss_pptp_callback_t \n
 * nss_pptp_msg_callback_t \n
 * net_device
 *
 * @param[in] if_num                 NSS interface number.
 * @param[in] type                   Dynamic interface type.
 * @param[in] pptp_data_callback     Callback for the data.
 * @param[in] notification_callback  Callback for the message.
 * @param[in] netdev                 Pointer to the associated network device.
 * @param[in] features               Socket buffer types supported by this interface.
 * @param[in] app_ctx                Pointer to the application context of the message.
 *
 * @return
 * Pointer to the NSS core context.
 */
extern struct nss_ctx_instance *nss_register_pptp_if(uint32_t if_num, uint32_t type, nss_pptp_callback_t pptp_data_callback,
					nss_pptp_msg_callback_t notification_callback, struct net_device *netdev, uint32_t features, void *app_ctx);

/**
 * nss_unregister_pptp_if
 *	Deregisters the PPTP tunnel interface from the NSS.
 *
 * @param[in] if_num  NSS interface number.
. *
 * @return
 * None.
 *
 * @dependencies
 * The tunnel interface must have been previously registered.
 */
extern void nss_unregister_pptp_if(uint32_t if_num);

/**
 * nss_pptp_msg_init
 *	Initializes a PPTP message.
 *
 * @datatypes
 * nss_pptp_msg
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
extern void nss_pptp_msg_init(struct nss_pptp_msg *ncm, uint16_t if_num, uint32_t type,  uint32_t len, void *cb, void *app_data);

/**
 * nss_pptp_register_handler
 *	Registers the PPTP interface with the NSS debug statistics handler.
 *
 * @return
 * None.
 */
extern void nss_pptp_register_handler(void);

/**
 * nss_pptp_session_debug_stats_get
 *	Gets NSS session debug statistics.
 *
 * @param[out] stats_mem  Pointer to the memory address, which must be large
 *                         enough to hold all the statistics.
 *
 * @return
 * None.
 */
extern void nss_pptp_session_debug_stats_get(void *stats_mem);

/**
 * nss_pptp_stats_register_notifier
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
extern int nss_pptp_stats_register_notifier(struct notifier_block *nb);

/**
 * nss_pptp_stats_unregister_notifier
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
extern int nss_pptp_stats_unregister_notifier(struct notifier_block *nb);

/**
 * @}
 */

#endif /* _NSS_PPTP_H_ */

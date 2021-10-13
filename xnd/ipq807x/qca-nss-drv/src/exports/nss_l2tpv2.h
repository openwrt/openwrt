/*
 **************************************************************************
 * Copyright (c) 2015, 2017-2020, The Linux Foundation. All rights reserved.
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
 * @file nss_l2tpv2.h
 *	NSS L2TPv2 interface definitions.
 */

#ifndef _NSS_L2TP_V2_H_
#define _NSS_L2TP_V2_H_

/**
 * @addtogroup nss_l2tpv2_subsystem
 * @{
 */

/**
 * Maximum number of supported L2TPv2 sessions.
 */
#define NSS_MAX_L2TPV2_DYNAMIC_INTERFACES 4

/**
 * nss_l2tpv2_metadata_types
 *	Message types for L2TPv2 requests and responses.
 */
enum nss_l2tpv2_metadata_types {
	NSS_L2TPV2_MSG_SESSION_CREATE,
	NSS_L2TPV2_MSG_SESSION_DESTROY,
	NSS_L2TPV2_MSG_SYNC_STATS,
	NSS_L2TPV2_MSG_BIND_IPSEC_IF,
	NSS_L2TPV2_MSG_MAX
};

/**
 * nss_l2tpv2_stats_session
 *	L2TPv2 debug statistics.
 */
enum nss_l2tpv2_stats_session {
	NSS_L2TPV2_STATS_SESSION_RX_PPP_LCP_PKTS,		/**< Number of PPP LCP packets received. */
	NSS_L2TPV2_STATS_SESSION_RX_EXP_DATA_PKTS,		/**< Number of Rx exceptioned packets. */
	NSS_L2TPV2_STATS_SESSION_ENCAP_PBUF_ALLOC_FAIL_PKTS,	/**< Number of times packet buffer allocation failed during encapsulation. */
	NSS_L2TPV2_STATS_SESSION_DECAP_PBUF_ALLOC_FAIL_PKTS,	/*< Number of times packet buffer allocation failed during decapsulation. */
	NSS_L2TPV2_STATS_SESSION_DECAP_L2TPOIPSEC_SRC_ERR,
						/**< Number of packets dropped due to source error in L2TP over IPsec flow in decapsulation. */
	NSS_L2TPV2_STATS_SESSION_MAX		/**< Maximum message type. */
};

/**
 * nss_l2tpv2_stats_notification
 *	L2TPv2 statistics structure.
 */
struct nss_l2tpv2_stats_notification {
	uint32_t core_id;				/**< Core ID. */
	uint32_t if_num;				/**< Interface number. */
	uint64_t stats[NSS_L2TPV2_STATS_SESSION_MAX];	/**< L2TPv2 statistics. */
};

#ifdef __KERNEL__ /* only kernel will use. */
/**
 * nss_l2tpv2_session_create_msg
 *	Payload for creating an L2TPv2 session.
 */
struct nss_l2tpv2_session_create_msg {
	uint16_t local_tunnel_id;	/**< Local identifier for the control connection. */
	uint16_t local_session_id;	/**< Local identifier of session inside a tunnel. */
	uint16_t peer_tunnel_id;	/**< Remote identifier for the control connection. */
	uint16_t peer_session_id;	/**< Remote identifier of session inside a tunnel. */

	uint32_t sip;			/**< Local tunnel endpoint IP address. */
	uint32_t dip;			/**< Remote tunnel endpoint IP address. */
	uint32_t reorder_timeout;	/**< Reorder timeout for out of order packets */

	uint16_t sport;			/**< Local source port. */
	uint16_t dport;			/**< Remote source port. */

	uint8_t recv_seq;		/**< Sequence number received. */
	uint8_t oip_ttl;		/**< Maximum time-to-live value for outer IP packet. */
	uint8_t udp_csum;		/**< UDP checksum. */
	uint8_t reserved;		/**< Alignment padding. */
};

/**
 * nss_l2tpv2_session_destroy_msg
 *	Payload for deletion an L2TPv2 session.
 */
struct nss_l2tpv2_session_destroy_msg {
	uint16_t local_tunnel_id;	/**< ID of the local tunnel. */
	uint16_t local_session_id;	/**< ID of the local session. */
};

/**
 * nss_l2tpv2_bind_ipsec_if_msg
 *	Message for binding the IPsec interface with L2TP.
 *
 * Message for configuring the L2TP session with an
 * IPsec inner interface number. This is used when
 * L2TP tunnel is enabled with IPsec.
 */
struct nss_l2tpv2_bind_ipsec_if_msg {
	uint32_t ipsec_ifnum;	/**< Inner IPSec interface number. */
};

/**
 * nss_l2tpv2_sync_session_stats_msg
 *	Message information for L2TPv2 synchronization statistics.
 */
struct nss_l2tpv2_sync_session_stats_msg {
	struct nss_cmn_node_stats node_stats;	/**< Common node statistics. */
	uint32_t rx_errors;			/**< Not used. Reserved for backward compatibility. */
	uint32_t rx_seq_discards;
			/**< Rx packets discarded because of a sequence number check. */
	uint32_t rx_oos_packets;		/**< Number of out of sequence packets received. */
	uint32_t tx_errors;			/**< Not used. Reserved for backward compatibility. */
	uint32_t tx_dropped;			/**< Tx packets dropped because of encapsulation failure or next node's queue is full. */

	/**
	 * Debug statistics for L2tp v2.
	 */
	struct {
		uint32_t rx_ppp_lcp_pkts;
				/**< Number of PPP LCP packets received. */
		uint32_t rx_exception_data_pkts;
				/**< Data packet exceptions sent to the host. */
		uint32_t encap_pbuf_alloc_fail;
				/**< Buffer allocation failure during encapsulation. */
		uint32_t decap_pbuf_alloc_fail;
				/**< Buffer allocation failure during decapsulation. */
		uint32_t decap_l2tpoipsec_src_error;
				/**< Packets dropped due to the wrong source for the L2TPoIPsec flow. */
	} debug_stats;	/**< Debug statistics object for l2tp v2. */
};

/**
 * nss_l2tpv2_msg
 *	Data for sending and receiving L2TPv2 messages.
 */
struct nss_l2tpv2_msg {
	struct nss_cmn_msg cm;		/**< Common message header. */

	/**
	 * Payload of an L2TPv2 message.
	 */
	union {
		struct nss_l2tpv2_session_create_msg session_create_msg;
				/**< Session create message. */
		struct nss_l2tpv2_session_destroy_msg session_destroy_msg;
				/**< Session delete message. */
		struct nss_l2tpv2_sync_session_stats_msg stats;
				/**< Session statistics. */
		struct nss_l2tpv2_bind_ipsec_if_msg bind_ipsec_if_msg;
				/**< Bind IPsec interface message. */
	} msg;			/**< Message payload. */
};

/**
 * Callback function for receiving L2TPv2 messages.
 *
 * @datatypes
 * nss_l2tpv2_msg
 *
 * @param[in] app_data  Pointer to the application context of the message.
 * @param[in] msg       Pointer to the message data.
 */
typedef void (*nss_l2tpv2_msg_callback_t)(void *app_data, struct nss_l2tpv2_msg *msg);

/**
 * nss_l2tpv2_tx
 *	Sends L2TPv2 messages to the NSS.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_l2tpv2_msg
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 * @param[in] msg      Pointer to the message data.
 *
 * @return
 * Status of the Tx operation.
 */
extern nss_tx_status_t nss_l2tpv2_tx(struct nss_ctx_instance *nss_ctx, struct nss_l2tpv2_msg *msg);

/**
 * nss_l2tpv2_get_context.
 *	Gets the L2TPv2 context used in L2TPv2 messages sent to the NSS.
 *
 * @return
 * Pointer to the NSS core context.
 */
extern struct nss_ctx_instance *nss_l2tpv2_get_context(void);

/**
 * Callback function for receiving L2TPv2 tunnel data.
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
typedef void (*nss_l2tpv2_callback_t)(struct net_device *netdev, struct sk_buff *skb, struct napi_struct *napi);

/**
 * nss_register_l2tpv2_if
 *	Registers the L2TPv2 tunnel interface with the NSS for sending and
 *	receiving messages.
 *
 * @datatypes
 * nss_l2tpv2_callback_t \n
 * nss_l2tpv2_msg_callback_t \n
 * net_device
 *
 * @param[in] if_num           NSS interface number.
 * @param[in] l2tpv2_callback  Callback for the L2TP tunnel data.
 * @param[in] msg_callback     Callback for the L2TP tunnel message.
 * @param[in] netdev           Pointer to the associated network device.
 * @param[in] features         SKB types supported by this interface.
 *
 * @return
 * Pointer to the NSS core context.
 */
extern struct nss_ctx_instance *nss_register_l2tpv2_if(uint32_t if_num, nss_l2tpv2_callback_t l2tpv2_callback,
					nss_l2tpv2_msg_callback_t msg_callback, struct net_device *netdev, uint32_t features);

/**
 * nss_unregister_l2tpv2_if
 *	Deregisters the L2TPv2 tunnel interface from the NSS.
 *
 * @param[in] if_num  NSS interface number.
 *
 * @return
 * None.
 *
 * @dependencies
 * The tunnel interface must have been previously registered.
 */
extern void nss_unregister_l2tpv2_if(uint32_t if_num);

/**
 * nss_l2tpv2_msg_init
 *	Initializes an L2TPv2 message.
 *
 * @datatypes
 * nss_l2tpv2_msg
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
extern void nss_l2tpv2_msg_init(struct nss_l2tpv2_msg *ncm, uint16_t if_num, uint32_t type,  uint32_t len, void *cb, void *app_data);

/**
 * nss_l2tpv2_register_handler
 *	Registers the L2TPv2 interface with the NSS debug statistics handler.
 *
 * @return
 * None.
 */
extern void nss_l2tpv2_register_handler(void);

/**
 * nss_l2tpv2_session_debug_stats_get
 *	Gets L2TPv2 NSS session debug statistics.
 *
 * @param[out] stats_mem  Pointer to the memory address, which must be large
 *                        enough to hold all the statistics.
 *
 * @return
 * None.
 */
extern void nss_l2tpv2_session_debug_stats_get(void  *stats_mem);

/**
 * nss_l2tpv2_stats_register_notifier
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
extern int nss_l2tpv2_stats_register_notifier(struct notifier_block *nb);

/**
 * nss_l2tpv2_stats_unregister_notifier
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
extern int nss_l2tpv2_stats_unregister_notifier(struct notifier_block *nb);
#endif /*__KERNEL__ */

/**
 * @}
 */

#endif /* _NSS_L2TP_V2_H_ */

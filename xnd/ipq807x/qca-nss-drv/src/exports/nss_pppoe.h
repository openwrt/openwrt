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

/**
 * @file nss_pppoe.h
 * 	NSS PPPoE interface definitions.
 */

#ifndef __NSS_PPPOE_H
#define __NSS_PPPOE_H

/**
 * @addtogroup nss_pppoe_subsystem
 * @{
 */

/**
 * Maximum number of supported PPPoE sessions.
 */
#define NSS_MAX_PPPOE_DYNAMIC_INTERFACES 8

/**
 * nss_pppoe_metadata_types
 *	Message types for PPPoE requests and responses.
 */
enum nss_pppoe_metadata_types {
	NSS_PPPOE_MSG_SESSION_CREATE,
	NSS_PPPOE_MSG_SESSION_DESTROY,
	NSS_PPPOE_MSG_SYNC_STATS,
	NSS_PPPOE_MSG_BR_ACCEL_CFG,
	NSS_PPPOE_MSG_MAX
};

/**
 * nss_pppoe_session_exception_events
 *	Session exception events from the PPPoE handler.
 */
enum nss_pppoe_session_exception_events {
	NSS_PPPOE_SESSION_EXCEPTION_EVENT_WRONG_VERSION_OR_TYPE,
	NSS_PPPOE_SESSION_EXCEPTION_EVENT_WRONG_CODE,
	NSS_PPPOE_SESSION_EXCEPTION_EVENT_UNSUPPORTED_PPP_PROTOCOL,
	NSS_PPPOE_SESSION_EXCEPTION_EVENT_MAX
};

/**
 * pppoe_base_exception_events
 * 	Base node exception events from the PPPoE handler.
 */
enum nss_pppoe_base_exception_events {
	NSS_PPPOE_BASE_EXCEPTION_EVENT_SHORT_PPPOE_HDR_LENGTH,
	NSS_PPPOE_BASE_EXCEPTION_EVENT_SHORT_PACKET_LENGTH,
	NSS_PPPOE_BASE_EXCEPTION_EVENT_WRONG_VERSION_OR_TYPE,
	NSS_PPPOE_BASE_EXCEPTION_EVENT_WRONG_CODE,
	NSS_PPPOE_BASE_EXCEPTION_EVENT_UNSUPPORTED_PPP_PROTOCOL,
	NSS_PPPOE_BASE_EXCEPTION_EVENT_DISABLED_BRIDGE_PACKET,
	NSS_PPPOE_BASE_EXCEPTION_EVENT_MAX
};

/**
 * nss_pppoe_br_accel_mode
 *	PPPoE bridge acceleration modes.
 */
enum nss_pppoe_br_accel_modes {
	NSS_PPPOE_BR_ACCEL_MODE_DIS,
	NSS_PPPOE_BR_ACCEL_MODE_EN_5T,
	NSS_PPPOE_BR_ACCEL_MODE_EN_3T,
	NSS_PPPOE_BR_ACCEL_MODE_MAX
};

/**
 * nss_pppoe_base_stats
 * 	PPPoE base node synchronization statistics.
 */
struct nss_pppoe_base_stats {
	struct nss_cmn_node_stats node;	/**< Common node statistics. */
	uint32_t exception[NSS_PPPOE_BASE_EXCEPTION_EVENT_MAX];
						/**< PPPoE base node exception events. */
};

/**
 * nss_pppoe_session_stats
 * 	PPPoE synchronization statistics per session.
 */
struct nss_pppoe_session_stats {
	struct nss_cmn_node_stats node;	/**< Common node statistics. */
	uint32_t exception[NSS_PPPOE_SESSION_EXCEPTION_EVENT_MAX];
						/**< PPPoE session exception events. */
};

/**
 * nss_pppoe_stats_session
 *	PPPoE session statistics.
 */
enum nss_pppoe_stats_session {
	NSS_PPPOE_STATS_SESSION_RX_PACKETS,
	NSS_PPPOE_STATS_SESSION_RX_BYTES,
	NSS_PPPOE_STATS_SESSION_TX_PACKETS,
	NSS_PPPOE_STATS_SESSION_TX_BYTES,
	NSS_PPPOE_STATS_SESSION_WRONG_VERSION_OR_TYPE,
	NSS_PPPOE_STATS_SESSION_WRONG_CODE,
	NSS_PPPOE_STATS_SESSION_UNSUPPORTED_PPP_PROTOCOL,
	NSS_PPPOE_STATS_SESSION_MAX
};

/**
 * nss_pppoe_stats_base
 *	PPPoE base node statistics.
 */
enum nss_pppoe_stats_base {
	NSS_PPPOE_STATS_BASE_RX_PACKETS,
	NSS_PPPOE_STATS_BASE_RX_BYTES,
	NSS_PPPOE_STATS_BASE_TX_PACKETS,
	NSS_PPPOE_STATS_BASE_TX_BYTES,
	NSS_PPPOE_STATS_BASE_RX_QUEUE_0_DROPPED,
	NSS_PPPOE_STATS_BASE_RX_QUEUE_1_DROPPED,
	NSS_PPPOE_STATS_BASE_RX_QUEUE_2_DROPPED,
	NSS_PPPOE_STATS_BASE_RX_QUEUE_3_DROPPED,
	NSS_PPPOE_STATS_BASE_SHORT_PPPOE_HDR_LENGTH,
	NSS_PPPOE_STATS_BASE_SHORT_PACKET_LENGTH,
	NSS_PPPOE_STATS_BASE_WRONG_VERSION_OR_TYPE,
	NSS_PPPOE_STATS_BASE_WRONG_CODE,
	NSS_PPPOE_STATS_BASE_UNSUPPORTED_PPP_PROTOCOL,
	NSS_PPPOE_STATS_BASE_DISABLED_BRIDGE_PACKET,
	NSS_PPPOE_STATS_BASE_MAX
};

/**
 * nss_pppoe_stats_notification
 *	PPPoE statistics structure.
 */
struct nss_pppoe_stats_notification {
	uint32_t core_id;					/**< Core ID. */
	uint32_t if_num;					/**< Interface number. */
	uint64_t session_stats[NSS_PPPOE_STATS_SESSION_MAX];	/**< PPPoE statistics. */
	uint64_t base_stats[NSS_PPPOE_STATS_BASE_MAX];		/**< PPPoE base node statistics. */
};

#ifdef __KERNEL__ /* only kernel will use. */

/**
 * nss_pppoe_sync_stats_msg
 *	PPPoE synchronization statistics.
 */
struct nss_pppoe_sync_stats_msg {
	struct nss_pppoe_session_stats session_stats;	/**< Session statistics. */
	struct nss_pppoe_base_stats base_stats;		/**< Base node statistics. */
};

/**
 * nss_pppoe_destroy_msg
 *	PPPoE session destroy message.
 */
struct nss_pppoe_destroy_msg {
	uint16_t session_id;		/**< PPPoE session identification number. */
	uint8_t server_mac[ETH_ALEN];	/**< PPPoE server MAC address. */
	uint8_t local_mac[ETH_ALEN];	/**< Local physical interface MAC address. */
};

/**
 * nss_pppoe_create_msg
 *	PPPoE session create message.
 */
struct nss_pppoe_create_msg {
	int32_t base_if_num;		/**< Base NSS interface number which PPPoE session created on. */
	uint32_t mtu;			/**< PPPoE interface MTU value. */
	uint8_t server_mac[ETH_ALEN];	/**< PPPoE server MAC address. */
	uint8_t local_mac[ETH_ALEN];	/**< Local physical interface MAC address. */
	uint16_t session_id;		/**< PPPoE session identification number. */
};

/**
 * nss_pppoe_br_accel_cfg_msg
 *	PPPoE bridge acceleration configuration message.
 */
struct nss_pppoe_br_accel_cfg_msg {
	uint32_t br_accel_cfg;		/**< PPPoE bridge acceleration configuration. */
};

/**
 * nss_pppoe_msg
 *	Data for sending and receiving PPPoE messages.
 */
struct nss_pppoe_msg {
	struct nss_cmn_msg cm;		/**< Common message header. */

	/**
	 * Payload of a PPPoE message.
	 */
	union {
		struct nss_pppoe_create_msg create;
					/**< Session create message. */
		struct nss_pppoe_destroy_msg destroy;
					/**< Session destroy message. */
		struct nss_pppoe_sync_stats_msg sync_stats;
					/**< Session statistics message. */
		struct nss_pppoe_br_accel_cfg_msg br_accel;
					/**< PPPoE bridge acceleration configuration message. */
	} msg;				/**< Message payload. */
};

/**
 * Callback function for receiving PPPoE messages.
 *
 * @datatypes
 * nss_pppoe_msg
 *
 * @param[in] app_data  Pointer to the application context of the message.
 * @param[in] msg       Pointer to the message data.
 */
typedef void (*nss_pppoe_msg_callback_t)(void *app_data, struct nss_pppoe_msg *msg);

/**
 * nss_pppoe_tx_msg_sync
 *	Sends a PPPoE message synchronously to NSS.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_pppoe_msg
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 * @param[in] msg      Pointer to the message data.
 *
 * @return
 * Status of the Tx operation.
 */
extern nss_tx_status_t nss_pppoe_tx_msg_sync(struct nss_ctx_instance *nss_ctx,
						struct nss_pppoe_msg *msg);

/**
 * nss_register_pppoe_session_if
 *	Registers the PPPoE session interface with the NSS for sending and
 *	receiving messages.
 *
 * @datatypes
 * nss_pppoe_msg_callback_t \n
 * net_device
 *
 * @param[in] if_num                 NSS interface number.
 * @param[in] notification_callback  Callback for the message.
 * @param[in] netdev                 Pointer to the associated network device.
 * @param[in] features               Socket buffer types supported by this interface.
 * @param[in] app_ctx                Pointer to the application context of the message.
 *
 * @return
 * Pointer to the NSS core context.
 */
extern struct nss_ctx_instance *nss_register_pppoe_session_if(uint32_t if_num,
						       nss_pppoe_msg_callback_t notification_callback,
						       struct net_device *netdev, uint32_t features, void *app_ctx);

/**
 * nss_unregister_pppoe_session_if
 *	Deregisters the PPPoE session interface from the NSS.
 *
 * @param[in] if_num  NSS interface number.
. *
 * @return
 * None.
 *
 * @dependencies
 * The tunnel interface must have been previously registered.
 */
extern void nss_unregister_pppoe_session_if(uint32_t if_num);

/**
 * nss_pppoe_get_context
 *	Gets the PPPoE context used in PPPoE transmit message.
 *
 * @return
 * Pointer to the NSS core context.
 */
extern struct nss_ctx_instance *nss_pppoe_get_context(void);

/**
 * nss_pppoe_debug_stats_get
 *	Gets NSS session debug statistics.
 *
 * @param[out] stats_mem  Pointer to the memory address, which must be large
 *                         enough to hold all the statistics.
 *
 * @return
 * None.
 */
extern void nss_pppoe_debug_stats_get(void *stats_mem);

/**
 * nss_pppoe_get_bridge_accel_mode
 *	Gets the PPPoE bridge acceleration mode.
 *
 * @return
 * Current PPPoE bridge acceleration mode.
 */
extern enum nss_pppoe_br_accel_modes nss_pppoe_get_br_accel_mode(void);

/**
 * nss_pppoe_register_sysctl
 *	Registers the PPPoE system control table.
 *
 * @return
 * None.
 */
void nss_pppoe_register_sysctl(void);

/**
 * nss_pppoe_unregister_sysctl
 *	Deregisters the PPPoE system control table.
 *
 * @return
 * None.
 *
 * @dependencies
 * The system control table must have been previously registered.
 */
void nss_pppoe_unregister_sysctl(void);

/**
 * nss_pppoe_msg_init
 *	Initializes a PPPoE message.
 *
 * @datatypes
 * nss_pppoe_msg
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
extern void nss_pppoe_msg_init(struct nss_pppoe_msg *ncm,
				uint16_t if_num, uint32_t type, uint32_t len,
				void *cb, void *app_data);

/**
 * nss_pppoe_stats_register_notifier
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
extern int nss_pppoe_stats_register_notifier(struct notifier_block *nb);

/**
 * nss_pppoe_stats_unregister_notifier
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
extern int nss_pppoe_stats_unregister_notifier(struct notifier_block *nb);

#endif /*__KERNEL__ */

/**
 * @}
 */

#endif /* __NSS_PPPOE_H */

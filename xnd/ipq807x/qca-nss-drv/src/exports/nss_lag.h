/*
 **************************************************************************
 * Copyright (c) 2014, 2015, 2017, The Linux Foundation. All rights reserved.
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
 * @file nss_lag.h
 *	NSS LAG APIs
 */

/**
 * @addtogroup nss_lag_subsystem
 * @{
 */

/**
 * nss_lag_metadata_types
 *	Types of LAG metadata.
 */
enum nss_lag_metadata_types {
	NSS_TX_METADATA_LAG_STATE_CHANGE = 0,
	NSS_TX_METADATA_LAG_MAX,
};

/**
 * nss_lag_state_change_ev
 *	LAG change events.
 */
enum nss_lag_state_change_ev {
	NSS_LAG_RELEASE = 0,
	NSS_LAG_ENSLAVE = 1,
};

/**
 * nss_lag_error_types
 *	LAG return values.
 */
enum nss_lag_error_types {
	NSS_LAG_ERROR_EINTERFACE = 1,
	NSS_LAG_ERROR_EMSG = 2,
};

/**
 * nss_lag_state_change
 *	Link aggregation (LAG) state changes.
 */
struct nss_lag_state_change {
	uint32_t lagid;				/**< ID of the link aggregation group. */
	uint32_t interface;
			/**< Physical interface on which the state change occurred. */
	enum nss_lag_state_change_ev event;	/**< Type of state change event. */
};

/**
 * nss_lag_msg
 *	Data for sending and receiving LAG messages.
 */
struct nss_lag_msg {
	struct nss_cmn_msg cm;	/**< Common message header. */

	/**
	 * Payload of a LAG message.
	 */
	union {
		struct nss_lag_state_change state;
				/**< State change message. */
	} msg;			/**< Message payload for LAG configuration messages exchanged with NSS core. */
};

/**
 * nss_lag_tx
 *	Sends a LAG message to the NSS.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_lag_msg
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 * @param[in] msg      Pointer to the message data.
 *
 * @return
 * Status of the Tx operation.
 */
extern nss_tx_status_t nss_lag_tx(struct nss_ctx_instance *nss_ctx, struct nss_lag_msg *msg);

/**
 * Callback function for receiving LAG data.
 *
 * @datatypes
 * net_device \n
 * sk_buff \n
 * napi_struct
 *
 * @param[in] dev   Pointer to the associated network device.
 * @param[in] skb   Pointer to the data socket buffer.
 * @param[in] napi  Pointer to the NAPI structure.
 */
typedef void (*nss_lag_callback_t)(struct net_device *dev, struct sk_buff *skb, struct napi_struct *napi);

/**
 * Callback function for receiving a LAG message.
 *
 * @datatypes
 * nss_lag_msg
 *
 * @param[in] ctx  Pointer to the application context for this message.
 * @param[in] nm   Pointer to the message data.
 */
typedef void (*nss_lag_msg_callback_t)(void *ctx, struct nss_lag_msg *nm);

/**
 * Callback function for receiving a LAG event.
 *
 * @datatypes
 * nss_lag_msg
 *
 * @param[in] app_data  Pointer to the application context of the message.
 * @param[in] msg       Pointer to the message data.
 */
typedef void (*nss_lag_event_callback_t)(void *app_data, struct nss_lag_msg *msg);

/**
 * nss_register_lag_if
 *	Registers the LAG interface with the NSS for sending and receiving data
 *	and messages.
 *
 * @datatypes
 * nss_lag_callback_t \n
 * nss_lag_event_callback_t \n
 * net_device
 *
 * @param[in] if_num     NSS interface number.
 * @param[in] lag_cb     Callback to receive LAG data.
 * @param[in] lag_ev_cb  Callback to receive LAG events.
 * @param[in] netdev     Pointer to the associated network device.
 *
 * @return
 * Pointer to the NSS core context.
 */
extern void *nss_register_lag_if(uint32_t if_num,
			 nss_lag_callback_t lag_cb,
			 nss_lag_event_callback_t lag_ev_cb,
			 struct net_device *netdev);

/**
 * nss_unregister_lag_if
 *	Deregisters the LAG interface from the NSS.
 *
 * @param[in] if_num  LAG interface number
 *
 * @return
 * None.
 *
 * @dependencies
 * The LAG interface must have been previously registered.
 */
extern void nss_unregister_lag_if(uint32_t if_num);

/**
 * nss_lag_msg_init
 *	Initializes a LAG message.
 *
 * @datatypes
 * nss_lag_msg \n
 * nss_lag_msg_callback_t
 *
 * @param[in,out] nlm       Pointer to the message.
 * @param[in]     lag_num   LAG interface number.
 * @param[in]     type      Type of buffer.
 * @param[in]     len       Length of the buffer.
 * @param[in]     cb        Callback function for the message.
 * @param[in]     app_data  Pointer to the application context of the message.
 *
 * @return
 * None.
 */
extern void nss_lag_msg_init(struct nss_lag_msg *nlm, uint16_t lag_num, uint32_t type, uint32_t len,
				nss_lag_msg_callback_t cb, void *app_data);

/**
 * nss_lag_tx_slave_state
 *	Sends LAG slave state.
 *
 * @datatypes
 * nss_lag_state_change_ev
 *
 * @param[in] lagid        LAG Group ID.
 * @param[in] slave_ifnum  Slave interface number.
 * @param[in] slave_state  Slave state.
 *
 * @return
 * Status of the Tx operation.
 */
extern nss_tx_status_t nss_lag_tx_slave_state(uint16_t lagid,
		int32_t slave_ifnum,
		enum nss_lag_state_change_ev slave_state);

/**
 * @}
 */

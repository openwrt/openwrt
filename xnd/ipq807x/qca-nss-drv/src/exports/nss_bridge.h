/*
 **************************************************************************
 * Copyright (c) 2016-2018,2020, The Linux Foundation. All rights reserved.
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
 * @file nss_bridge.h
 *	NSS Bridge interface definitions.
 */

#ifndef __NSS_BRIDGE_H
#define __NSS_BRIDGE_H

/**
 * @addtogroup nss_bridge_subsystem
 * @{
 */

/**
 * nss_bridge_msg_types
 *	Message types for the NSS bridge.
 */
enum nss_bridge_msg_types {
	NSS_BRIDGE_MSG_JOIN = NSS_IF_MAX_MSG_TYPES + 1,
	NSS_BRIDGE_MSG_LEAVE,
	NSS_BRIDGE_MSG_SET_FDB_LEARN,
	NSS_BRIDGE_MSG_TYPE_MAX,
};

/**
 * nss_bridge_error_types
 *	Error types for the NSS bridge.
 */
enum nss_bridge_error_types {
	NSS_BRIDGE_ERROR_UNKNOWN_MSG = NSS_IF_ERROR_TYPE_MAX + 1,
	NSS_BRIDGE_ERROR_TYPE_MAX,
};

/**
 * nss_bridge_fdb_learn_mode
 *	FDB learning mode for the NSS bridge.
 */
enum nss_bridge_fdb_learn_mode {
	NSS_BRIDGE_FDB_LEARN_ENABLE,
	NSS_BRIDGE_FDB_LEARN_DISABLE,
	NSS_BRIDGE_FDB_LEARN_MODE_MAX,
};

/**
 * nss_bridge_join_msg
 *	Information for joining the bridge.
 */
struct nss_bridge_join_msg {
	uint32_t if_num;	/**< NSS interface to add to a bridge. */
};

/**
 * nss_bridge_leave_msg
 *	Information for leaving the bridge.
 */
struct nss_bridge_leave_msg {
	uint32_t if_num;	/**< NSS interface to remove from a bridge. */
};

/**
 * nss_bridge_set_fdb_learn_msg
 *	Information for FDB learning status on bridge interface.
 */
struct nss_bridge_set_fdb_learn_msg {
	uint32_t mode;		/**< FDB learning mode of bridge interface. */
};

/**
 * nss_bridge_msg
 *	Data for sending and receiving bridge interface messages.
 */
struct nss_bridge_msg {
	struct nss_cmn_msg cm;	/**< Common message header. */

	/**
	 * Payload of a bridge interface message.
	 */
	union {
		union nss_if_msgs if_msg;
				/**< NSS interface base message. */
		struct nss_bridge_join_msg br_join;
				/**< Join the bridge. */
		struct nss_bridge_leave_msg br_leave;
				/**< Leave the bridge. */
		struct nss_bridge_set_fdb_learn_msg fdb_learn;
				/**< FDB learning status of bridge. */
	} msg;			/**< Message payload. */
};

/**
 * nss_bridge_verify_if_num
 *	Verifies if the interface is type bridge.
 *
 * @param[in] if_num  Interface number to be verified.
 *
 * @return
 * True if if_num is of type bridge.
 */
bool nss_bridge_verify_if_num(uint32_t if_num);

/**
 * nss_bridge_tx_msg
 *	Sends bridge messages to the NSS.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_bridge_msg
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 * @param[in] msg      Pointer to the message data.
 *
 * @return
 * Status of the Tx operation.
 */
nss_tx_status_t nss_bridge_tx_msg(struct nss_ctx_instance *nss_ctx, struct nss_bridge_msg *msg);

/**
 * nss_bridge_tx_msg_sync
 *	Sends bridge messages synchronously to the NSS.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_bridge_msg
 *
 * @param[in]     nss_ctx  Pointer to the NSS context.
 * @param[in,out] msg      Pointer to the message data.
 *
 * @return
 * Status of the Tx operation.
 */
nss_tx_status_t nss_bridge_tx_msg_sync(struct nss_ctx_instance *nss_ctx, struct nss_bridge_msg *msg);

/**
 * nss_bridge_msg_init
 *	Initializes a bridge message.
 *
 * @datatypes
 * nss_bridge_msg
 *
 * @param[in,out] ncm       Pointer to the message.
 * @param[in]     if_num    Interface number
 * @param[in]     type      Type of message.
 * @param[in]     len       Size of the payload.
 * @param[in]     cb        Callback function for the message.
 * @param[in]     app_data  Pointer to the application context of the message.
 *
 * @return
 * None.
 */
void nss_bridge_msg_init(struct nss_bridge_msg *ncm, uint16_t if_num, uint32_t type,  uint32_t len, void *cb, void *app_data);

/**
 * nss_bridge_get_context
 *	Gets the bridge context used in nss_bridge_tx.
 *
 * @return
 * Pointer to the NSS core context.
 */
struct nss_ctx_instance *nss_bridge_get_context(void);

/**
 * Callback function for receiving bridge data.
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
typedef void (*nss_bridge_callback_t)(struct net_device *netdev, struct sk_buff *skb, struct napi_struct *napi);

/**
 * Callback function for receiving bridge messages.
 *
 * @datatypes
 * nss_bridge_msg
 *
 * @param[in] app_data  Pointer to the application context of the message.
 * @param[in] msg       Pointer to the message data.
 */
typedef void (*nss_bridge_msg_callback_t)(void *app_data, struct nss_bridge_msg *msg);

/**
 * nss_bridge_register
 *	Registers the bridge interface with the NSS for sending and receiving
 *	messages.
 *
 * @param[in] if_num          NSS interface number.
 * @param[in] netdev          Pointer to the associated network device.
 * @param[in] bridge_data_cb  Callback for the bridge data.
 * @param[in] bridge_msg_cb   Callback for the bridge message.
 * @param[in] features        Data socket buffer types supported by this interface.
 * @param[in] app_data        Pointer to the application context of the message.
 *
 * @return
 * Pointer to the NSS core context.
 */
struct nss_ctx_instance *nss_bridge_register(uint32_t if_num, struct net_device *netdev, nss_bridge_callback_t bridge_data_cb, nss_bridge_msg_callback_t bridge_msg_cb, uint32_t features, void *app_data);

/**
 * nss_bridge_unregister
 *	Deregisters the bridge interface from the NSS.
 *
 * @param[in] if_num  NSS interface number.
 *
 * @return
 * None.
 */
void nss_bridge_unregister(uint32_t if_num);

/**
 * nss_bridge_notify_register
 *	Registers a notifier callback for bridge messages with the NSS.
 *
 * @datatypes
 * nss_bridge_msg_callback_t
 *
 * @param[in] cb        Callback function for the message.
 * @param[in] app_data  Pointer to the application context of the message.
 *
 * @return
 * Pointer to the NSS core context.
 */
struct nss_ctx_instance *nss_bridge_notify_register(nss_bridge_msg_callback_t cb, void *app_data);

/**
 * nss_bridge_notify_unregister
 *	Deregisters a bridge message notifier callback from the NSS.
 *
 * @return
 * None.
 */
void nss_bridge_notify_unregister(void);

/**
 * nss_bridge_tx_set_mtu_msg
 *	Sends a message to the bridge to set the MTU.
 *
 * @param[in] bridge_if_num  Interface number of the bridge.
 * @param[in] mtu            MTU value to set.
 *
 * @return
 * Status of the Tx operation.
 */
nss_tx_status_t nss_bridge_tx_set_mtu_msg(uint32_t bridge_if_num, uint32_t mtu);

/**
 * nss_bridge_tx_set_mac_addr_msg
 *	Sends a message to the bridge to set the MAC address.
 *
 * @param[in] bridge_if_num  Interface number of the bridge.
 * @param[in] addr           Pointer to the MAC address.
 *
 * @return
 * Status of the Tx operation.
 */
nss_tx_status_t nss_bridge_tx_set_mac_addr_msg(uint32_t bridge_if_num, uint8_t *addr);

/**
 * nss_bridge_tx_join_msg
 *	Sends the bridge a message to join with a slave interface.
 *
 * @datatypes
 * net_device
 *
 * @param[in] bridge_if_num  Interface number of the bridge.
 * @param[in] netdev         Pointer to the associated network device (the
 *                           slave interface).
 *
 * @return
 * Status of the Tx operation.
 */
nss_tx_status_t nss_bridge_tx_join_msg(uint32_t bridge_if_num, struct net_device *netdev);

/**
 * nss_bridge_tx_leave_msg
 *	Sends the bridge a message that the slave interface is leaving the bridge.
 *
 * @datatypes
 * net_device
 *
 * @param[in] bridge_if_num  Interface number of the bridge.
 * @param[in] netdev         Pointer to the associated network device (the
 *                           slave interface).
 *
 * @return
 * Status of the Tx operation.
 */
nss_tx_status_t nss_bridge_tx_leave_msg(uint32_t bridge_if_num, struct net_device *netdev);

/**
 * nss_bridge_tx_vsi_assign_msg
 *	Sends the bridge a message to assign a VSI.
 *
 * @param[in] if_num  Interface number of the bridge.
 * @param[in] vsi     VSI to assign.
 *
 * @return
 * Status of the Tx operation.
 */
nss_tx_status_t nss_bridge_tx_vsi_assign_msg(uint32_t if_num, uint32_t vsi);

/**
 * nss_bridge_tx_vsi_unassign_msg
 *	Sends the bridge a message to unassign a VSI.
 *
 * @param[in] if_num  Interface number of the bridge.
 * @param[in] vsi     VSI to unassign.
 *
 * @return
 * Status of the Tx operation.
 */
nss_tx_status_t nss_bridge_tx_vsi_unassign_msg(uint32_t if_num, uint32_t vsi);

/**
 * nss_bridge_tx_set_fdb_learn_msg
 *	Sends a message to notify NSS about FDB learning enable/disable event.
 *
 * @datatypes
 * nss_bridge_fdb_learn_mode
 *
 * @param[in] bridge_if_num  Interface number of the bridge.
 * @param[in] fdb_learn      FDB learning disable/enable.
 *
 * @return
 * Status of the Tx operation.
 */
nss_tx_status_t nss_bridge_tx_set_fdb_learn_msg(uint32_t bridge_if_num, enum nss_bridge_fdb_learn_mode fdb_learn);

/**
 * nss_bridge_init
 *	Initializes the bridge.
 *
 * @return
 * None.
 */
void nss_bridge_init(void);

/**
 * @}
 */

#endif /* __NSS_BRIDGE_H */

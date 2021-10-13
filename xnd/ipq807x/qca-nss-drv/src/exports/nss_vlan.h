/*
 **************************************************************************
 * Copyright (c) 2017, The Linux Foundation. All rights reserved.
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
 * @file nss_vlan.h
 *	NSS VLAN interface definitions.
 */

#ifndef __NSS_VLAN_H
#define __NSS_VLAN_H

/**
 * @addtogroup nss_vlan_subsystem
 * @{
 */

/**
 * nss_vlan_msg_types
 *	VLAN message types.
 */
enum nss_vlan_msg_types {
	NSS_VLAN_MSG_ADD_TAG = NSS_IF_MAX_MSG_TYPES + 1,
	NSS_VLAN_MSG_TYPE_MAX,
};

/**
 * nss_vlan_error_types
 *	VLAN error types
 */
enum nss_vlan_error_types {
	NSS_VLAN_ERROR_UNKNOWN_MSG = NSS_IF_ERROR_TYPE_MAX + 1,
	NSS_VLAN_ERROR_TYPE_MAX,
};

#define NSS_VLAN_TYPE_SINGLE 0	/**< Single VLAN tag in message. */
#define NSS_VLAN_TYPE_DOUBLE 1	/**< Double VLAN tag in message. */

/**
 * nss_vlan_msg_add_tag
 *	VLAN message data for adding a VLAN tag.
 */
struct nss_vlan_msg_add_tag {
	uint32_t vlan_tag;	/**< VLAN tag information. */
	uint32_t next_hop;	/**< Parent interface. */
	uint32_t if_num;	/**< Actual physical interface. */
};

/**
 * nss_vlan_msg
 *	Data for sending and receiving VLAN messages.
 */
struct nss_vlan_msg {
	struct nss_cmn_msg cm;		/**< Common message header. */

	/**
	 * Payload of a VLAN message.
	 */
	union {
		union nss_if_msgs if_msg;
				/**< NSS interface base messages. */
		struct nss_vlan_msg_add_tag add_tag;
				/**< VLAN add-a-tag message. */
	} msg;			/**< Message payload. */
};

/**
 * nss_vlan_tx_msg
 *	Sends a VLAN message to the NSS.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_vlan_msg
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 * @param[in] msg      Pointer to the message data.
 *
 * @return
 * Status of the Tx operation.
 */
nss_tx_status_t nss_vlan_tx_msg(struct nss_ctx_instance *nss_ctx, struct nss_vlan_msg *msg);

/**
 * nss_vlan_tx_msg_sync
 *	Sends a VLAN message to the NSS synchronously.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_vlan_msg
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 * @param[in] msg      Pointer to the message data.
 *
 * @return
 * Status of the Tx operation.
 */
nss_tx_status_t nss_vlan_tx_msg_sync(struct nss_ctx_instance *nss_ctx, struct nss_vlan_msg *msg);

/**
 * Initializes a VLAN message.
 *
 * @datatypes
 * nss_vlan_msg
 *
 * @param[in,out] ncm       Pointer to the message.
 * @param[in]     if_num    NSS interface number.
 * @param[in]     type      Type of message.
 * @param[in]     len       Size of the payload.
 * @param[in]     cb        Pointer to the message callback.
 * @param[in]     app_data  Pointer to the application context of the message.
 *
 * @return
 * None.
 */
void nss_vlan_msg_init(struct nss_vlan_msg *ncm, uint16_t if_num, uint32_t type,  uint32_t len, void *cb, void *app_data);

/**
 * nss_vlan_get_context
 *	Gets the VLAN context used in nss_vlan_tx.
 *
 * @return
 * Pointer to the NSS core context.
 */
struct nss_ctx_instance *nss_vlan_get_context(void);

/**
 * Callback when VLAN data is received
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
typedef void (*nss_vlan_callback_t)(struct net_device *netdev, struct sk_buff *skb, struct napi_struct *napi);

/**
 * Callback to receive VLAN messages
 *
 * @datatypes
 * nss_vlan_msg
 *
 * @param[in] app_data  Pointer to the application context of the message.
 * @param[in] msg       Pointer to the message data.
 */
typedef void (*nss_vlan_msg_callback_t)(void *app_data, struct nss_vlan_msg *msg);

/**
 * nss_register_vlan_if
 *	Register to send/receive VLAN messages to NSS
 *
 * @datatypes
 * nss_vlan_callback_t \n
 * net_device
 *
 * @param[in] if_num              NSS interface number.
 * @param[in] vlan_data_callback  Callback for the data.
 * @param[in] netdev              Pointer to the associated network device.
 * @param[in] features            Data socket buffer types supported by this interface.
 * @param[in] app_ctx             Pointer to the application context of the message.
 *
 * @return
 * Pointer to the NSS core context.
 */
struct nss_ctx_instance *nss_register_vlan_if(uint32_t if_num, nss_vlan_callback_t vlan_data_callback,
					      struct net_device *netdev, uint32_t features, void *app_ctx);

/**
 * Deregisters the VLAN interface from the NSS.
 *
 * @return
 * None.
 */
void nss_unregister_vlan_if(uint32_t if_num);

/**
 * nss_vlan_tx_set_mtu_msg
 *	Sends a VLAN message to set the MTU.
 *
 * @param[in] vlan_if_num  VLAN interface number.
 * @param[in] mtu          MTU value to set.
 *
 * @return
 * Status of the Tx operation.
 */
nss_tx_status_t nss_vlan_tx_set_mtu_msg(uint32_t vlan_if_num, uint32_t mtu);

/**
 * nss_vlan_tx_set_mac_addr_msg
 *	Sends a VLAN message to set the MAC address.
 *
 * @param[in] vlan_if_num  VLAN interface number.
 * @param[in] addr         Pointer to the MAC address.
 *
 * @return
 * Status of the Tx operation.
 */
nss_tx_status_t nss_vlan_tx_set_mac_addr_msg(uint32_t vlan_if_num, uint8_t *addr);

/**
 * nss_vlan_tx_vsi_attach_msg
 *	Send a VLAN message to attach a VSI.
 *
 * @param[in] vlan_if_num  VLAN interface number.
 * @param[in] vsi          PPE VSI to attach.
 *
 * @return
 * Status of the Tx operation.
 */
nss_tx_status_t nss_vlan_tx_vsi_attach_msg(uint32_t vlan_if_num, uint32_t vsi);

/**
 * nss_vlan_tx_vsi_detach_msg
 *	Sends a VLAN message to detach VSI.
 *
 * @param[in] vlan_if_num  VLAN interface number.
 * @param[in] vsi          VSI to detach.
 *
 * @return
 * Status of the Tx operation.
 */
nss_tx_status_t nss_vlan_tx_vsi_detach_msg(uint32_t vlan_if_num, uint32_t vsi);

/**
 * nss_vlan_tx_add_tag_msg
 *	Sends a VLAN add tag message.
 *
 * @param[in] vlan_if_num   VLAN interface number.
 * @param[in] vlan_tag      VLAN tag information.
 * @param[in] next_hop      Parent interface.
 * @param[in] physical_dev  Physical port to which to add the VLAN tag.
 *
 * @return
 * Status of the Tx operation.
 */
nss_tx_status_t nss_vlan_tx_add_tag_msg(uint32_t vlan_if_num, uint32_t vlan_tag, uint32_t next_hop, uint32_t physical_dev);

/**
 * Registers the VLAN handler with the NSS.
 *
 * @return
 * None.
 */
void nss_vlan_register_handler(void);

/**
 * @}
 */

#endif /* __NSS_VLAN_H */

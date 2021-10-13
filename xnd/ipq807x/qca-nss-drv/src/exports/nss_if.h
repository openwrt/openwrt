/*
 **************************************************************************
 * Copyright (c) 2014-2021, The Linux Foundation. All rights reserved.
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
 * @file nss_if.h
 *	NSS interface definitions.
 */

#ifndef __NSS_IF_H
#define __NSS_IF_H

#define NSS_IF_TX_TIMEOUT 3000 /* 3 Seconds */

/**
 * @addtogroup nss_driver_subsystem
 * @{
 */

/**
 * nss_if_message_types
 *	Message types for the NSS interface.
 */
enum nss_if_message_types {
	NSS_IF_OPEN,
	NSS_IF_CLOSE,
	NSS_IF_LINK_STATE_NOTIFY,
	NSS_IF_MTU_CHANGE,
	NSS_IF_MAC_ADDR_SET,
	NSS_IF_STATS,
	NSS_IF_ISHAPER_ASSIGN,
	NSS_IF_BSHAPER_ASSIGN,
	NSS_IF_ISHAPER_UNASSIGN,
	NSS_IF_BSHAPER_UNASSIGN,
	NSS_IF_ISHAPER_CONFIG,
	NSS_IF_BSHAPER_CONFIG,
	NSS_IF_PAUSE_ON_OFF,
	NSS_IF_VSI_ASSIGN,
	NSS_IF_VSI_UNASSIGN,
	NSS_IF_SET_NEXTHOP,
	NSS_IF_SET_IGS_NODE,
	NSS_IF_CLEAR_IGS_NODE,
	NSS_IF_RESET_NEXTHOP,
	NSS_IF_PPE_PORT_CREATE,
	NSS_IF_PPE_PORT_DESTROY,
	NSS_IF_MAX_MSG_TYPES = 9999,
};

/**
 * nss_if_error_types
 *	Error types for the NSS interface.
 */
enum nss_if_error_types {
	NSS_IF_ERROR_NO_ISHAPERS,
	NSS_IF_ERROR_NO_BSHAPERS,
	NSS_IF_ERROR_NO_ISHAPER,
	NSS_IF_ERROR_NO_BSHAPER,
	NSS_IF_ERROR_ISHAPER_OLD,
	NSS_IF_ERROR_BSHAPER_OLD,
	NSS_IF_ERROR_ISHAPER_CONFIG_FAILED,
	NSS_IF_ERROR_BSHAPER_CONFIG_FAILED,
	NSS_IF_ERROR_TYPE_UNKNOWN,
	NSS_IF_ERROR_TYPE_EOPEN,
	NSS_IF_ERROR_TYPE_INVALID_MTU,
	NSS_IF_ERROR_TYPE_INVALID_MAC_ADDR,
	NSS_IF_ERROR_TYPE_VSI_NOT_MATCH,
	NSS_IF_ERROR_TYPE_VSI_REASSIGN,
	NSS_IF_ERROR_TYPE_VSI_INVALID,
	NSS_IF_ERROR_TYPE_MAX = 9999,
};

/**
 * nss_if_data_align
 *	Data alignment modes for the NSS interface.
 */
enum nss_if_data_align {
	NSS_IF_DATA_ALIGN_2BYTE = 0,
	NSS_IF_DATA_ALIGN_4BYTE = 2,
};

/**
 * nss_if_open
 *	Message information for opening the NSS interface.
 */
struct nss_if_open {
	uint32_t tx_desc_ring;	/**< Tx descriptor ring address. */
	uint32_t rx_desc_ring;	/**< Rx descriptor ring address. */
	uint32_t rx_forward_if;	/**< Forward received packets to this interface. */
	uint32_t alignment_mode;/**< Header alignment mode. */
};

/**
 * nss_if_close
 *	Message information for closing the NSS interface.
 */
struct nss_if_close {
	uint32_t reserved;	/**< Placeholder for the structure. */
};

/**
 * nss_if_link_state_notify
 *	Link state notification sent to the NSS interface.
 */
struct nss_if_link_state_notify {
	uint32_t state;
			/**< Link state UP is bit 0 set. Other bits are as defined by Linux to indicate speed and duplex. */
};

/**
 * nss_if_mtu_change
 *	MTU change for the NSS interface.
 */
struct nss_if_mtu_change {
	uint16_t min_buf_size;	/**< Changed value for the minimum buffer size. */
};

/**
 * nss_if_pause_on_off
 *	Enables or disables a pause frame for the NSS interface.
 */
struct nss_if_pause_on_off {
	uint32_t pause_on;	/**< Turn the pause frame ON or OFF. */
};

/**
 * nss_if_mac_address_set
 *	MAC address setting.
 */
struct nss_if_mac_address_set {
	uint8_t mac_addr[ETH_ALEN];	/**< MAC address. */
};

/**
 * nss_if_shaper_assign
 *	Shaper assignment message.
 */
struct nss_if_shaper_assign {
	uint32_t shaper_id;		/**< ID of the request. */
	uint32_t new_shaper_id;		/**< ID of the response. */
};

/**
 * nss_if_shaper_unassign
 *	Shaper unassign message.
 */
struct nss_if_shaper_unassign {
	uint32_t shaper_id;		/**< ID of the request. */
};

/**
 * nss_if_shaper_configure
 *	Shaper configuration message.
 */
struct nss_if_shaper_configure {
	struct nss_shaper_configure config;	/**< Specific shaper message for a particular interface. */
};

/**
 * nss_if_vsi_assign
 *	VSI assignment message.
 */
struct nss_if_vsi_assign {
	uint32_t vsi;		/**< Virtual interface number. */
};

/**
 * nss_if_vsi_unassign
 *	VSI unassign message.
 */
struct nss_if_vsi_unassign {
	uint32_t vsi;		/**< Virtual interface number. */
};

/**
 * nss_if_set_nexthop
 *	Message to set nexthop for an interface.
 */
struct nss_if_set_nexthop {
	uint32_t nexthop;	/**< Nexthop interface number. */
};

/**
 * nss_if_igs_config
 *	Ingress shaper set/clear configure message structure.
 */
struct nss_if_igs_config {
	int32_t igs_num;	/**< Ingress shaper interface number. */
};

/**
 * nss_if_ppe_port_create
 *	Message to create PPE port.
 */
struct nss_if_ppe_port_create {
	int32_t ppe_port_num;	/**< PPE port number returned by NSS. */
};

/**
 * nss_if_msgs
 *	Information for physical NSS interface command messages.
 */
union nss_if_msgs {
	struct nss_if_link_state_notify link_state_notify;
			/**< Link status notification. */
	struct nss_if_open open;
			/**< Open the NSS interface. */
	struct nss_if_close close;
			/**< Close the NSS interface. */
	struct nss_if_mtu_change mtu_change;
			/**< MTU change notification. */
	struct nss_if_mac_address_set mac_address_set;
			/**< MAC address setting. */
	struct nss_cmn_node_stats stats;
			/**< Synchronize the satistics. */
	struct nss_if_shaper_assign shaper_assign;
			/**< Assign the shaper. */
	struct nss_if_shaper_unassign shaper_unassign;
			/**< Unassign the shaper. */
	struct nss_if_shaper_configure shaper_configure;
			/**< Configure the shaper. */
	struct nss_if_pause_on_off pause_on_off;
			/**< ON or OFF notification for a Pause frame. */
	struct nss_if_vsi_assign vsi_assign;
			/**< Assign the VSI. */
	struct nss_if_vsi_unassign vsi_unassign;
			/**< Remove the VSI assignment. */
	struct nss_if_set_nexthop set_nexthop;
			/**< Set nexthop of interface. */
	struct nss_if_igs_config config_igs;
			/**< Configure an ingress shaper interface. */
	struct nss_if_ppe_port_create ppe_port_create;
			/**< Create a PPE port. */
};

/**
 * nss_if_msg
 *	Data for sending and receiving base class messages for all interface types.
 */
struct nss_if_msg {
	struct nss_cmn_msg cm;		/**< Common message header. */
	union nss_if_msgs msg;		/**< Message payload. */
};

/**
 * Callback function for receiving NSS interface messages.
 *
 * @datatypes
 * nss_if_msg
 *
 * @param[in] app_data  Pointer to the application context of the message.
 * @param[in] msg       Pointer to the message data.
 */
typedef void (*nss_if_msg_callback_t)(void *app_data, struct nss_if_msg *msg);

/**
 * Callback function for receiving NSS interface data.
 *
 * TODO: Adjust to pass app_data as unknown to the
 * list layer and netdev/sk as known.
 *
 * @datatypes
 * net_device \n
 * sk_buff
 *
 * @param[in] netdev  Pointer to the associated network device.
 * @param[in] skb     Pointer to the data socket buffer.
 */
typedef void (*nss_if_rx_callback_t)(struct net_device *netdev, struct sk_buff *skb);

/**
 * nss_if_register
 *	Registers the NSS interface for sending and receiving GMAC packets and messages.
 *
 * @datatypes
 * nss_if_rx_callback_t \n
 * nss_if_msg_callback_t \n
 * net_device
 *
 * @param[in] if_num        NSS interface number.
 * @param[in] rx_callback   Receive callback for the packet.
 * @param[in] msg_callback  Receive callback for message.
 * @param[in] if_ctx        Pointer to the interface context provided in the
                            callback. This context must be the OS network
                            device context pointer (net_device in Linux).
 *
 * @return
 * Pointer to the NSS core context.
 */
extern struct nss_ctx_instance *nss_if_register(uint32_t if_num,
					nss_if_rx_callback_t rx_callback,
					nss_if_msg_callback_t msg_callback,
					struct net_device *if_ctx);

/**
 * nss_if_tx_buf
 *	Sends GMAC packets to a specific physical or virtual network interface.
 *
 * @datatypes
 * nss_ctx_instance \n
 * sk_buff
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 * @param[in] os_buf   Pointer to the OS buffer (e.g., skbuff).
 * @param[in] if_num   Network physical or virtual interface number.
 *
 * @return
 * Status of the Tx operation.
 */
extern nss_tx_status_t nss_if_tx_buf(struct nss_ctx_instance *nss_ctx, struct sk_buff *os_buf, uint32_t if_num);

/**
 * nss_if_tx_msg
 *	Sends a message to the NSS interface.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_if_msg
 *
 * @param[in,out] nss_ctx  Pointer to the NSS context.
 * @param[in]     nim      Pointer to the NSS interface message.
 *
 * @return
 * Status of the Tx operation.
 */
nss_tx_status_t nss_if_tx_msg(struct nss_ctx_instance *nss_ctx, struct nss_if_msg *nim);

/**
 * nss_if_msg_sync
 *	Sends a message to the NSS interface and wait for the response.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_if_msg
 *
 * @param[in,out] nss_ctx  Pointer to the NSS context.
 * @param[in]     nim      Pointer to the NSS interface message.
 *
 * @return
 * Status of the Tx operation.
 */
nss_tx_status_t nss_if_msg_sync(struct nss_ctx_instance *nss_ctx, struct nss_if_msg *nim);

/**
 * nss_if_set_nexthop
 *	Configure the next hop for an interface.
 *
 * @datatypes
 * nss_ctx_instance
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 * @param[in] if_num   NSS interface number.
 * @param[in] nexthop  NSS interface number for next hop node.
 *
 * @return
 * Status of the Tx operation.
 */
nss_tx_status_t nss_if_set_nexthop(struct nss_ctx_instance *nss_ctx, uint32_t if_num, uint32_t nexthop);

/**
 * nss_if_reset_nexthop
 *	De-configure the next hop for an interface.
 *
 * @datatypes
 * nss_ctx_instance
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 * @param[in] if_num   NSS interface number.
 *
 * @return
 * Status of the Tx operation.
 */
nss_tx_status_t nss_if_reset_nexthop(struct nss_ctx_instance *nss_ctx, uint32_t if_num);

/**
 * nss_if_change_mtu
 *	Change the MTU of the interface.
 *
 * @datatypes
 * nss_ctx_instance
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 * @param[in] if_num   NSS interface number.
 * @param[in] mtu      New MTU.
 *
 * @return
 * Status of the transmit operation.
 */
nss_tx_status_t nss_if_change_mtu(struct nss_ctx_instance *nss_ctx, nss_if_num_t if_num, uint16_t mtu);

/**
 * nss_if_change_mac_addr
 *	Change the MAC address of the interface.
 *
 * @datatypes
 * nss_ctx_instance
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 * @param[in] if_num   NSS interface number.
 * @param[in] mac_addr New MAC address.
 *
 * @return
 * Status of the transmit operation.
 */
nss_tx_status_t nss_if_change_mac_addr(struct nss_ctx_instance *nss_ctx, nss_if_num_t if_num, uint8_t *mac_addr);

/**
 * nss_if_vsi_unassign
 *	Detach the VSI ID from the given interface.
 *
 * @datatypes
 * nss_ctx_instance
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 * @param[in] if_num   NSS interface number.
 * @param[in] vsi      VSI ID.
 *
 * @return
 * Status of the transmit operation.
 */
nss_tx_status_t nss_if_vsi_unassign(struct nss_ctx_instance *nss_ctx, nss_if_num_t if_num, uint32_t vsi);

/**
 * nss_if_vsi_assign
 *	Attach the VSI ID to the given interface.
 *
 * @datatypes
 * nss_ctx_instance
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 * @param[in] if_num   NSS interface number.
 * @param[in] vsi      VSI ID.
 *
 * @return
 * Status of the transmit operation.
 */
nss_tx_status_t nss_if_vsi_assign(struct nss_ctx_instance *nss_ctx, nss_if_num_t if_num, uint32_t vsi);

/**
 * @}
 */

#endif /*  __NSS_IF_H */

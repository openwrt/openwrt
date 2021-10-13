/*
 **************************************************************************
 * Copyright (c) 2014-2017, 2019, The Linux Foundation. All rights reserved.
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

/*
 * @file nss_virt_if.h
 *	NSS Virtual interface message Structure and APIs
 */

#ifndef __NSS_VIRT_IF_H
#define __NSS_VIRT_IF_H

#include "nss_if.h"

/**
 * @addtogroup nss_virtual_if_subsystem
 * @{
 */

/**
 * nss_virt_if_msg_types
 *	Message types for virtual interface requests and responses.
 */
enum nss_virt_if_msg_types {
	NSS_VIRT_IF_OPEN = NSS_IF_OPEN,
	NSS_VIRT_IF_CLOSE = NSS_IF_CLOSE,
	NSS_VIRT_IF_LINK_STATE_NOTIFY = NSS_IF_LINK_STATE_NOTIFY,
	NSS_VIRT_IF_MTU_CHANGE = NSS_IF_MTU_CHANGE,
	NSS_VIRT_IF_MAC_ADDR_SET = NSS_IF_MAC_ADDR_SET,
	NSS_VIRT_IF_STATS_SYNC = NSS_IF_STATS,
	NSS_VIRT_IF_ISHAPER_ASSIGN = NSS_IF_ISHAPER_ASSIGN,
	NSS_VIRT_IF_BSHAPER_ASSIGN = NSS_IF_BSHAPER_ASSIGN,
	NSS_VIRT_IF_ISHAPER_UNASSIGN = NSS_IF_ISHAPER_UNASSIGN,
	NSS_VIRT_IF_BSHAPER_UNASSIGN = NSS_IF_BSHAPER_UNASSIGN,
	NSS_VIRT_IF_ISHAPER_CONFIG = NSS_IF_ISHAPER_CONFIG,
	NSS_VIRT_IF_BSHAPER_CONFIG = NSS_IF_BSHAPER_CONFIG,
	NSS_VIRT_IF_VSI_ASSIGN = NSS_IF_VSI_ASSIGN,
	NSS_VIRT_IF_VSI_UNASSIGN = NSS_IF_VSI_UNASSIGN,
	NSS_VIRT_IF_TX_CONFIG_MSG = NSS_IF_MAX_MSG_TYPES + 1,
	NSS_VIRT_IF_STATS_SYNC_MSG,
	NSS_VIRT_IF_MAX_MSG_TYPES,
};

/**
 * nss_virt_if_error_types
 *	Error types for the virtual interface.
 */
enum nss_virt_if_error_types {
	NSS_VIRT_IF_SUCCESS,
	NSS_VIRT_IF_CORE_FAILURE,
	NSS_VIRT_IF_ALLOC_FAILURE,
	NSS_VIRT_IF_DYNAMIC_IF_FAILURE,
	NSS_VIRT_IF_MSG_TX_FAILURE,
	NSS_VIRT_IF_REG_FAILURE,
	NSS_VIRT_IF_CORE_NOT_INITIALIZED,
};

/**
 * nss_virt_if_base_node_stats
 *	Virtual interface statistics of NSS base node.
 */
struct nss_virt_if_base_node_stats {
	uint32_t active_interfaces;	/**< Number of active virtual interfaces. */
	uint32_t ocm_alloc_failed;	/**< Number of interface allocation failure on OCM. */
	uint32_t ddr_alloc_failed;	/**< Number of interface allocation failure on DDR. */
};

/**
 * nss_virt_if_interface_stats
 *	Virtual interface statistics of each pair of interfaces.
 */
struct nss_virt_if_interface_stats {
	struct nss_cmn_node_stats node_stats;	/**< Common statistics. */
	uint32_t tx_enqueue_failed;		/**< Tx enqueue failures in the firmware. */
	uint32_t shaper_enqueue_failed;		/**< Shaper enqueue failures in the firmware. */
	uint32_t ocm_alloc_failed;		/**< Number of allocation failure on OCM. */
};

/**
 * nss_virt_if_stats
 *	Virtual interface statistics received from the NSS.
 */
struct nss_virt_if_stats {
	struct nss_virt_if_base_node_stats base_stats;
	struct nss_virt_if_interface_stats if_stats;
};

/**
 * nss_virt_if_config_msg
 *	Message information for configuring the virtual interface.
 */
struct nss_virt_if_config_msg {
	uint32_t flags;			/**< Interface flags. */
	uint32_t sibling;		/**< Sibling interface number. */
	uint32_t nexthop;		/**< Next hop interface number. */
	uint8_t mac_addr[ETH_ALEN];	/**< MAC address. */
};

/**
 * nss_virt_if_msg
 *	Data for sending and receiving virtual interface messages.
 */
struct nss_virt_if_msg {
	struct nss_cmn_msg cm;		/**< Common message header. */

	/**
	 * Payload of a virtual interface message.
	 */
	union {
		union nss_if_msgs if_msgs;
				/**< NSS interface base message. */
		struct nss_virt_if_config_msg if_config;
				/**< Rule for creating a virtual interface. */
		struct nss_virt_if_stats stats;
				/**< Virtual interface statistics. */
	} msg;			/**< Message payload. */
};

/*
 * nss_virt_if_pvt
 *	Private data information for the virtual interface.
 */
struct nss_virt_if_pvt {
	struct semaphore sem;
			/**< Semaphore to ensure that only one instance of a message is sent to the NSS. */
	struct completion complete;
			/**< Waits for message completion or time out. */
	int response;		/**< Message process response from the NSS firmware. */
	int sem_init_done;	/**< Semaphore initialization is done. */
};

/**
 * Callback to transmit virtual interface data received from NSS
 * to the transmit path of the virtual interface.
 *
 * @datatypes
 * net_device \n
 * sk_buff
 *
 * @param[in] netdev  Pointer to the associated network device.
 * @param[in] skb     Pointer to the data socket buffer.
 */
typedef void (*nss_virt_if_xmit_callback_t)(struct net_device *netdev, struct sk_buff *skb);

/**
 * Callback function for virtual interface data.
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
typedef void (*nss_virt_if_data_callback_t)(struct net_device *netdev, struct sk_buff *skb, struct napi_struct *napi);

/**
 * Callback function for virtual interface messages.
 *
 * @datatypes
 * nss_cmn_msg
 *
 * @param[in] app_data  Pointer to the application context of the message.
 * @param[in] msg       Pointer to the message data.
 */
typedef void (*nss_virt_if_msg_callback_t)(void *app_data, struct nss_cmn_msg *msg);

/**
 * nss_virt_if_handle
 *	Context information for WLAN-to-NSS communication.
 */
struct nss_virt_if_handle {
	struct nss_ctx_instance *nss_ctx;	/**< NSS context. */
	int32_t if_num_n2h;			/**< Redirect interface number on NSS-to-host path. */
	int32_t if_num_h2n;			/**< Redirect interface number on host-to-NSS path. */
	struct net_device *ndev;		/**< Associated network device. */
	struct nss_virt_if_pvt *pvt;		/**< Private data structure. */
	struct nss_virt_if_stats stats;		/**< Virtual interface statistics. */
	atomic_t refcnt;			/**< Reference count. */
	nss_virt_if_msg_callback_t cb;		/**< Message callback. */
	void *app_data;		/**< Application data to be passed to the callback. */
};

/**
 * nss_virt_if_dp_type
 *	Virtual interface datapath types. Redirect interface on NSS-to-host path will be seen by ECM for rules.
 */
enum nss_virt_if_dp_type {
	NSS_VIRT_IF_DP_REDIR_N2H,		/**< Redirect interface on NSS-to-host path has zero value. */
	NSS_VIRT_IF_DP_REDIR_H2N,		/**< Redirect interface on host-to-NSS path has non-zero value. */
};

/**
 * nss_virt_if_create
 *	Creates a virtual interface asynchronously.
 *
 * @datatypes
 * net_device \n
 * nss_virt_if_msg_callback_t
 *
 * @param[in] netdev    Pointer to the associated network device.
 * @param[in] cb        Callback function for the message. This callback is
                        invoked when the response from the firmware is received.
 * @param[in] app_data  Pointer to the application context of the message.
 *
 * @return
 * Status of the Tx operation.
 */
extern int nss_virt_if_create(struct net_device *netdev, nss_virt_if_msg_callback_t cb, void *app_data);

/**
 * nss_virt_if_create_sync
 *	Creates a virtual interface synchronously with the default nexthop values
 *	NSS_N2H_INTERFACE and NSS_ETH_RX_INTERFACE.
 *
 * @datatypes
 * net_device
 *
 * @param[in] netdev  Pointer to the associated network device.
 *
 * @return
 * Pointer to nss_virt_if_handle.
 */
extern struct nss_virt_if_handle *nss_virt_if_create_sync(struct net_device *netdev);

/**
 * nss_virt_if_create_sync_nexthop
 *	Creates a virtual interface synchronously with specified nexthops.
 *
 * @datatypes
 * net_device
 *
 * @param[in] netdev       Pointer to the associated network device.
 * @param[in] nexthop_n2h  Nexthop interface number of network-to-host dynamic interface.
 * @param[in] nexthop_h2n  Nexthop interface number of host-to-network dynamic interface.
 *
 * @return
 * Pointer to NSS virtual interface handle.
 */
extern struct nss_virt_if_handle *nss_virt_if_create_sync_nexthop(struct net_device *netdev, uint32_t nexthop_n2h, uint32_t nexthop_h2n);

/**
 * nss_virt_if_destroy
 *	Destroys the virtual interface asynchronously.
 *
 * @datatypes
 * nss_virt_if_handle \n
 * nss_virt_if_msg_callback_t
 *
 * @param[in,out] handle    Pointer to the virtual interface handle (provided during
 *                          dynamic interface allocation).
 * @param[in]     cb        Callback function for the message. This callback is
 *                          invoked when the response from the firmware is received.
 * @param[in]     app_data  Pointer to the application context of the message.
 *
 * @return
 * Status of the Tx operation.
 *
 * @dependencies
 * The interface must have been previously created.
 */
extern nss_tx_status_t nss_virt_if_destroy(struct nss_virt_if_handle *handle, nss_virt_if_msg_callback_t cb, void *app_data);

/**
 * nss_virt_if_destroy_sync
 *	Destroys the virtual interface synchronously.
 *
 * @datatypes
 * nss_virt_if_handle
 *
 * @param[in,out] handle  Pointer to the virtual interface handle (provided during
 *                        dynamic interface allocation).
 *
 * @return
 * Status of the Tx operation.
 *
 * @dependencies
 * The interface must have been previously created.
 */
extern nss_tx_status_t nss_virt_if_destroy_sync(struct nss_virt_if_handle *handle);

/**
 * nss_virt_if_tx_msg
 *	Sends a message to the virtual interface.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_virt_if_msg
 *
 * @param[in] nss_ctx  Pointer to the NSS context (provided during registration).
 * @param[in] nvim     Pointer to the virtual interface message.
 *
 * @return
 * Command Tx status.
 */
extern nss_tx_status_t nss_virt_if_tx_msg(struct nss_ctx_instance *nss_ctx, struct nss_virt_if_msg *nvim);

/**
 * nss_virt_if_tx_buf
 *	Forwards virtual interface packets to the NSS.
 *
 * @datatypes
 * nss_virt_if_handle \n
 * sk_buff
 *
 * @param[in,out] handle  Pointer to the virtual interface handle (provided during
 *                        registration).
 * @param[in]    skb     Pointer to the data socket buffer.
 *
 * @return
 * Status of the Tx operation.
 */
extern nss_tx_status_t nss_virt_if_tx_buf(struct nss_virt_if_handle *handle,
						struct sk_buff *skb);

/**
 * nss_virt_if_xmit_callback_register
 *	Registers a transmit callback to a virtual interface.
 *
 * @datatypes
 * nss_virt_if_handle \n
 * nss_virt_if_xmit_callback_t
 *
 * @param[in,out] handle        Pointer to the virtual interface handle (provided during
 *                              dynamic interface allocation).
 * @param[in]    cb             Callback handler for virtual data packets.
 *
 * @return
 * None.
 */
extern void nss_virt_if_xmit_callback_register(struct nss_virt_if_handle *handle,
				nss_virt_if_xmit_callback_t cb);

/**
 * nss_virt_if_xmit_callback_unregister
 *	Deregisters the transmit callback from the virtual interface.
 *
 * @datatypes
 * nss_virt_if_handle
 *
 * @param[in,out] handle  Pointer to the virtual interface handle.
 *
 * @return
 * None.
 */
extern void nss_virt_if_xmit_callback_unregister(struct nss_virt_if_handle *handle);

/**
 * nss_virt_if_register
 *	Registers a virtual Interface with NSS driver.
 *
 * @datatypes
 * nss_virt_if_handle \n
 * nss_virt_if_data_callback_t \n
 * net_device
 *
 * @param[in,out] handle        Pointer to the virtual interface handle(provided during
 *                              dynamic interface allocation).
 * @param[in]    data_callback  Callback handler for virtual data packets
 * @param[in]    netdev         Pointer to the associated network device.
 *
 * @return
 * Status of the Tx operation.
 */
extern void nss_virt_if_register(struct nss_virt_if_handle *handle,
					nss_virt_if_data_callback_t data_callback,
					struct net_device *netdev);

/**
 * nss_virt_if_unregister
 *	Deregisters a virtual interface from the NSS driver.
 *
 * @datatypes
 * nss_virt_if_handle
 *
 * @param[in,out] handle  Pointer to the virtual interface handle.
 *
 * @return
 * None.
 */
extern void nss_virt_if_unregister(struct nss_virt_if_handle *handle);

/**
 * nss_virt_if_get_interface_num
 *	Returns the virtual interface number associated with the handle.
 *
 * @datatypes
 * nss_virt_if_handle
 *
 * @param[in] handle  Pointer to the virtual interface handle(provided during
                      dynamic interface allocation).
 *
 * @return
 * Virtual interface number.
 */
extern int32_t nss_virt_if_get_interface_num(struct nss_virt_if_handle *handle);

/**
 * nss_virt_if_verify_if_num
 *	Verifies if the interface is 802.3 redirect type.
 *
 * @param[in] if_num  Interface number to be verified.
 *
 * @return
 * True if if_num is 802.3 redirect type.
 */
bool nss_virt_if_verify_if_num(uint32_t if_num);

/**
 * nss_virt_if_get_context
 *	Gets the virtual interface context.
 *
 * @return
 * Pointer to the NSS core context.
 */
struct nss_ctx_instance *nss_virt_if_get_context(void);

/**
 * @}
 */

#endif /* __NSS_VIRT_IF_H */

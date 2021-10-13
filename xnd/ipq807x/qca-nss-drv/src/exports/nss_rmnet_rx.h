/*
 **************************************************************************
 * Copyright (c) 2019-2020, The Linux Foundation. All rights reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 **************************************************************************
 */

/*
 * @file nss_rmnet_rx.h
 *	NSS RMNET interface message Structure and APIs
 */

#ifndef __NSS_RMNET_RX_H
#define __NSS_RMNET_RX_H

#include "nss_if.h"

/**
 * @addtogroup nss_rmnet_subsystem
 * @{
 */

/**
 * Maximum number of DMA channel.
 */
#define NSS_RMNET_RX_CHANNEL_MAX 12

/**
 * nss_rmnet_rx_dp_type
 *	Interface datapath types. NSS-to-host path will be seen by ECM for rules.
 */
enum nss_rmnet_rx_dp_type {
	NSS_RMNET_RX_DP_N2H,	/**< Interface on NSS-to-host path has zero value. */
	NSS_RMNET_RX_DP_H2N,	/**< Interface on host-to-NSS path has non-zero value. */
};

/**
 * nss_rmnet_rx_msg_types
 *	Message types for interface requests and responses.
 */
enum nss_rmnet_rx_msg_types {
	NSS_RMNET_RX_TX_CONFIG_MSG = NSS_IF_MAX_MSG_TYPES + 1,
					/**< Configuration message. */
	NSS_RMNET_RX_STATS_SYNC_MSG,	/**< Statistic synchronization message. */
	NSS_RMNET_RX_MAX_MSG_TYPES,	/**< Maximum message type. */
};

/**
 * nss_rmnet_rx_error_types
 *	Error types for the RMNET interface.
 */
enum nss_rmnet_rx_error_types {
	NSS_RMNET_RX_SUCCESS,				/**< No error. */
	NSS_RMNET_RX_ERROR_TYPE_MSG_UNKNOWN,		/**< Unknown message type. */
	NSS_RMNET_RX_ERROR_TYPE_ALREADY_CONFIGURED,	/**< Tunnel is already configured. */
	NSS_RMNET_RX_ERROR_TYPE_SIBLING_NOTFOUND,	/**< Sibling interface is not found. */
	NSS_RMNET_RX_ERROR_TYPE_NEXTHOP_NOTFOUND,	/**< Next hop is not found. */
	NSS_RMNET_RX_ERROR_TYPE_SIBLING_MISMATCH,	/**< Sibling interface type mismatches. */
	NSS_RMNET_RX_ERROR_TYPE_DMA_CHANNEL_FAIL,	/**< DMA Channel allocation failed. */
	NSS_RMNET_RX_ERROR_TYPE_RMNET_INVALID,		/**< Interface type is invalid. */
	NSS_RMNET_RX_ERROR_TYPE_SHAPER_INVALID,		/**< Shaper is invalid. */
	NSS_RMNET_RX_REG_FAILURE,			/**< Registration failed. */
	NSS_RMNET_RX_ALLOC_FAILURE,			/**< Memory allocation failed. */
	NSS_RMNET_RX_ERROR_MAX,				/**< Maximum error type. */
};

/**
 * nss_rmnet_rx_pvt
 *	Private data information for the interface.
 */
struct nss_rmnet_rx_pvt {
	struct semaphore sem;
			/**< Semaphore to ensure that only one instance of a message is sent to the NSS. */
	struct completion complete;
			/**< Waits for message completion or time out. */
	int response;		/**< Message process response from the NSS firmware. */
	int sem_init_done;	/**< Semaphore initialization is done. */
};

/**
 * nss_rmnet_rx_config_msg
 *	Message information for configuring the interface.
 */
struct nss_rmnet_rx_config_msg {
	uint32_t flags;			/**< Interface flags. */
	uint32_t sibling;		/**< Sibling interface number. */
	uint32_t nexthop;		/**< Next hop interface number. */
	uint32_t no_channel;		/**< Number of channels. */
};

/**
 * nss_rmnet_rx_stats
 *	Interface statistics received from the NSS.
 */
struct nss_rmnet_rx_stats {
	struct nss_cmn_node_stats node_stats;
					/**< Common statistics. */
	uint32_t enqueue_failed;	/**< Enqueue to next node is failed. */
	uint32_t no_avail_channel;	/**< No available DMA channel. */
	uint32_t num_linear_pbuf;	/**< Number of linear pbufs. */
	uint32_t no_pbuf_to_linear;	/**< No pbuf to linearize. */
	uint32_t no_enough_room;	/**< Not enough headroom to linearize the pbuf. */
	uint32_t using_channel[NSS_RMNET_RX_CHANNEL_MAX];
					/**< How many times a channel is used. */
	uint32_t dma_failed;		/**< DMA copy call failed. */
};


/**
 * nss_rmnet_rx_msg
 *	Data for sending and receiving interface messages.
 */
struct nss_rmnet_rx_msg {
	struct nss_cmn_msg cm;		/**< Common message header. */

	/**
	 * Payload of an RMNET interface message.
	 */
	union {
		struct nss_rmnet_rx_config_msg if_config;
				/**< Rule for creating an RMNET interface. */
		struct nss_rmnet_rx_stats stats;
				/**< RMNET interface statistics. */
	} msg;			/**< Message payload. */
};

/**
 * Callback to transmit interface data received from NSS
 * to the transmit path of the RMNET interface.
 *
 * @datatypes
 * net_device \n
 * sk_buff
 *
 * @param[in] netdev  Pointer to the associated network device.
 * @param[in] skb     Pointer to the data socket buffer.
 */
typedef void (*nss_rmnet_rx_xmit_callback_t)(struct net_device *netdev, struct sk_buff *skb);

/**
 * Callback function for interface data.
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
typedef void (*nss_rmnet_rx_data_callback_t)(struct net_device *netdev, struct sk_buff *skb, struct napi_struct *napi);

/**
 * Callback function for interface messages.
 *
 * @datatypes
 * nss_cmn_msg
 *
 * @param[in] app_data  Pointer to the application context of the message.
 * @param[in] msg       Pointer to the message data.
 */
typedef void (*nss_rmnet_rx_msg_callback_t)(void *app_data, struct nss_cmn_msg *msg);

/**
 * nss_rmnet_rx_handle
 *	Context information for NSS communication.
 */
struct nss_rmnet_rx_handle {
	struct nss_ctx_instance *nss_ctx;	/**< NSS context. */
	int32_t if_num_n2h;			/**< Redirect interface number on NSS-to-host path. */
	int32_t if_num_h2n;			/**< Redirect interface number on host-to-NSS path. */
	struct net_device *ndev;		/**< Associated network device. */
	struct nss_rmnet_rx_pvt *pvt;		/**< Private data structure. */
	uint64_t *stats_n2h;			/**< RMNET interface statistics from NSS-to-host. */
	uint64_t *stats_h2n;			/**< RMNET interface statistics from host-to-NSS. */
	atomic_t refcnt;			/**< Reference count. */
	nss_rmnet_rx_msg_callback_t cb;		/**< Message callback. */
	void *app_data;		/**< Application data to be passed to the callback. */
};

/**
 * nss_rmnet_rx_destroy_sync
 *	Destroys the RMNET interface synchronously.
 *
 * @datatypes
 * nss_rmnet_rx_handle
 *
 * @param[in,out] handle  Pointer to the RMNET interface handle (provided during
 *                        dynamic interface allocation).
 *
 * @return
 * Status of the Tx operation.
 *
 * @dependencies
 * The interface must have been previously created.
 */
extern nss_tx_status_t nss_rmnet_rx_destroy_sync(struct nss_rmnet_rx_handle *handle);

/**
 * nss_rmnet_rx_create_sync_nexthop
 *	Creates an RMNET interface synchronously with specified nexthops.
 *
 * @datatypes
 * net_device
 *
 * @param[in] netdev       Pointer to the associated network device.
 * @param[in] nexthop_n2h  Nexthop interface number of NSS-to-host dynamic interface.
 * @param[in] nexthop_h2n  Nexthop interface number of host-to-NSS dynamic interface.
 *
 * @return
 * Pointer to the NSS RMNET interface handle.
 */
extern struct nss_rmnet_rx_handle *nss_rmnet_rx_create_sync_nexthop(struct net_device *netdev, uint32_t nexthop_n2h, uint32_t nexthop_h2n);

/**
 * nss_rmnet_rx_create
 *	Creates an RMNET interface synchronously with generic nexthops.
 *
 * @datatypes
 * net_device
 *
 * @param[in] netdev       Pointer to the associated network device.
 *
 * @return
 * Pointer to the NSS RMNET interface handle.
 */
extern struct nss_rmnet_rx_handle *nss_rmnet_rx_create(struct net_device *netdev);

/**
 * nss_rmnet_rx_tx_buf
 *	Forwards RMNET interface packets to the NSS.
 *
 * @datatypes
 * nss_rmnet_rx_handle \n
 * sk_buff
 *
 * @param[in,out] handle  Pointer to the RMNET interface handle (provided during
 *                        registration).
 * @param[in]    skb     Pointer to the data socket buffer.
 *
 * @return
 * Status of the Tx operation.
 */
extern nss_tx_status_t nss_rmnet_rx_tx_buf(struct nss_rmnet_rx_handle *handle,
						struct sk_buff *skb);

/**
 * nss_rmnet_rx_tx_msg
 *	Sends a message to the RMNET interface.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_rmnet_rx_msg
 *
 * @param[in] nss_ctx  Pointer to the NSS context (provided during registration).
 * @param[in] nvim     Pointer to the RMNET interface message.
 *
 * @return
 * Command Tx status.
 */
extern nss_tx_status_t nss_rmnet_rx_tx_msg(struct nss_ctx_instance *nss_ctx, struct nss_rmnet_rx_msg *nvim);

/**
 * nss_rmnet_rx_xmit_callback_unregister
 *	Deregisters the transmit callback from the RMNET interface.
 *
 * @datatypes
 * nss_rmnet_rx_handle
 *
 * @param[in,out] handle  Pointer to the RMNET interface handle.
 *
 * @return
 * None.
 */
extern void nss_rmnet_rx_xmit_callback_unregister(struct nss_rmnet_rx_handle *handle);

/**
 * nss_rmnet_rx_xmit_callback_register
 *	Registers a transmit callback to an RMNET interface.
 *
 * @datatypes
 * nss_rmnet_rx_handle \n
 * nss_rmnet_rx_xmit_callback_t
 *
 * @param[in,out] handle        Pointer to the RMNET interface handle (provided during
 *                              dynamic interface allocation).
 * @param[in]    cb             Callback handler for RMNET data packets.
 *
 * @return
 * None.
 */
extern void nss_rmnet_rx_xmit_callback_register(struct nss_rmnet_rx_handle *handle,
				nss_rmnet_rx_xmit_callback_t cb);

/**
 * nss_rmnet_rx_unregister
 *	Deregisters an RMNET interface from the NSS driver.
 *
 * @datatypes
 * nss_rmnet_rx_handle
 *
 * @param[in,out] handle  Pointer to the RMNET interface handle.
 *
 * @return
 * None.
 */
extern void nss_rmnet_rx_unregister(struct nss_rmnet_rx_handle *handle);

/**
 * nss_rmnet_rx_register
 *	Registers an RMNET Interface with NSS driver.
 *
 * @datatypes
 * nss_rmnet_rx_handle \n
 * nss_rmnet_rx_data_callback_t \n
 * net_device
 *
 * @param[in,out] handle        Pointer to the RMNET interface handle (provided during
 *                              dynamic interface allocation).
 * @param[in]    data_callback  Callback handler for RMNET data packets.
 * @param[in]    netdev         Pointer to the associated network device.
 *
 * @return
 * Status of the Tx operation.
 */
extern void nss_rmnet_rx_register(struct nss_rmnet_rx_handle *handle,
					nss_rmnet_rx_data_callback_t data_callback,
					struct net_device *netdev);

/**
 * nss_rmnet_rx_get_ifnum_with_coreid
 *	Returns the interface number with the core ID.
 *
 * @param[in] if_num  NSS interface number
 *
 * @return
 * Interface number with the core ID.
 */
extern int32_t nss_rmnet_rx_get_ifnum_with_core_id(int32_t if_num);

/**
 * nss_rmnet_rx_get_ifnum
 *	Returns the interface number with appending core ID.
 *
 * @param[in] dev Net device
 *
 * @return
 * Interface number with the core ID.
 */
extern int32_t nss_rmnet_rx_get_ifnum(struct net_device *dev);

/**
 * nss_rmnet_rx_get_interface_num
 *	Returns the RMNET interface number associated with the handle.
 *
 * @datatypes
 * nss_rmnet_rx_handle
 *
 * @param[in] handle  Pointer to the RMNET interface handle (provided during
		      dynamic interface allocation).
 *
 * @return
 * RMNET interface number.
 */
extern int32_t nss_rmnet_rx_get_interface_num(struct nss_rmnet_rx_handle *handle);

/**
 * nss_rmnet_rx_get_context
 *	Gets the RMNET interface context.
 *
 * @return
 * Pointer to the NSS core context.
 */
struct nss_ctx_instance *nss_rmnet_rx_get_context(void);

/**
 * @}
 */

#endif /* __NSS_RMNET_RX_H */

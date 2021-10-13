/*
 **************************************************************************
 * Copyright (c) 2014, 2016-2020, The Linux Foundation. All rights reserved.
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
 * @file nss_cmn.h
 *	NSS Common Message Structure and APIs
 */

#ifndef __NSS_CMN_H
#define __NSS_CMN_H

/**
 * @addtogroup nss_common_subsystem
 * @{
 */

/*
 * Interface Number 1 Valid 7 Core 8 Type 16 Index
 */
typedef int32_t nss_if_num_t;
#define NSS_IF_IS_TYPE_DYNAMIC(if_num)		(if_num != -1)
#define NSS_IF_IS_VALID(if_num)		(if_num < NSS_MAX_NET_INTERFACES)

/**
 * @struct nss_ctx_instance
 *	Forward declaration for structure that contains instance data for each
 *	NSS core. Contents of structure are private to the NSS driver.
 */
struct nss_ctx_instance;

/*
 * The first 8 bits of an interfaces number is representing the core_id,
 * 0 means local core.
 */

#define NSS_CORE_ID_SHIFT 24		/**< Number of bits to shift a core local interface number. */

/**
 * Macro that appends the core identifier to an interface number.
 */
#define NSS_INTERFACE_NUM_APPEND_COREID(nss_ctx, interface) ((interface) | ((nss_ctx->id + 1) << NSS_CORE_ID_SHIFT))

/**
 * Macro to obtain a core local interface number.
 */
#define NSS_INTERFACE_NUM_GET(interface) ((interface) & 0xffffff)

/**
 * Macro to obtain an interface core number.
 */
#define NSS_INTERFACE_NUM_GET_COREID(interface) ((interface >> NSS_CORE_ID_SHIFT) & 0xff)

/*
 * Common enumerations.
 */

/**
 * nss_tx_status_t
 *	Tx command failure results.
 *
 * Types starting with NSS_TX_FAILURE_SYNC_ are only used by synchronous messages.
 */
typedef enum {
	NSS_TX_SUCCESS = 0,
	NSS_TX_FAILURE,
	NSS_TX_FAILURE_QUEUE,
	NSS_TX_FAILURE_NOT_READY,
	NSS_TX_FAILURE_TOO_LARGE,
	NSS_TX_FAILURE_TOO_SHORT,
	NSS_TX_FAILURE_NOT_SUPPORTED,
	NSS_TX_FAILURE_BAD_PARAM,
	NSS_TX_FAILURE_NOT_ENABLED,
	NSS_TX_FAILURE_SYNC_BAD_PARAM,
	NSS_TX_FAILURE_SYNC_TIMEOUT,
	NSS_TX_FAILURE_SYNC_FW_ERR,
	NSS_TX_FAILURE_MAX,
} nss_tx_status_t;

/**
 * nss_state_t
 *	Initialization states.
 */
typedef enum {
	NSS_STATE_UNINITIALIZED = 0,
	NSS_STATE_INITIALIZED
} nss_state_t;

/**
 * nss_core_id_t
 *	NSS core IDs.
 */
typedef enum {
	NSS_CORE_0 = 0,
	NSS_CORE_1,
	NSS_CORE_MAX
} nss_core_id_t;

/**
 * nss_cb_register_status_t
 *	Callback registration states.
 */
typedef enum {
	NSS_CB_REGISTER_SUCCESS = 0,
	NSS_CB_REGISTER_FAILED,
} nss_cb_register_status_t;

/**
 * nss_cb_unregister_status_t
 *	Callback deregistration states.
 */
typedef enum {
	NSS_CB_UNREGISTER_SUCCESS = 0,
	NSS_CB_UNREGISTER_FAILED,
} nss_cb_unregister_status_t;

/**
 * nss_cmn_response
 *	Responses for a common message.
 */
enum nss_cmn_response {
	NSS_CMN_RESPONSE_ACK,
	NSS_CMN_RESPONSE_EVERSION,
	NSS_CMN_RESPONSE_EINTERFACE,
	NSS_CMN_RESPONSE_ELENGTH,
	NSS_CMN_RESPONSE_EMSG,
	NSS_CMN_RESPONSE_NOTIFY,
	NSS_CMN_RESPONSE_LAST
};

/**
 * Array of log messages for common NSS responses.
 */
extern int8_t *nss_cmn_response_str[NSS_CMN_RESPONSE_LAST];

/**
 * nss_cmn_msg
 *	Common message information.
 */
struct nss_cmn_msg {
	uint16_t version;	/**< Version ID for the main message format. */
	uint16_t len;		/**< Length of the message, excluding the header. */
	uint32_t interface;	/**< Primary key for all messages. */
	enum nss_cmn_response response;
			/**< Primary response. All messages must specify one of these responses. */

	uint32_t type;	/**< Decentralized request number used to match response numbers. */
	uint32_t error;	/**< Decentralized specific error message (response == EMSG). */

	/**
	 * Padding used to start the callback from a 64-bit boundary. This field can be reused.
	 */
	uint32_t reserved;

	nss_ptr_t cb;		/**< Contains the callback pointer. */
#ifndef __LP64__
	uint32_t padding1;	/**< Padding used to fit 64 bits. Do not reuse. */
#endif
	nss_ptr_t app_data;	/**< Contains the application data. */
#ifndef __LP64__
	uint32_t padding2;	/**< Padding used to fit 64 bits. Do not reuse. */
#endif
};

/**
 * nss_cmn_node_stats
 *	Common per-node statistics.
 */
struct nss_cmn_node_stats {
	uint32_t rx_packets;			/**< Number of packets received. */
	uint32_t rx_bytes;			/**< Number of bytes received. */
	uint32_t tx_packets;			/**< Number of packets transmitted. */
	uint32_t tx_bytes;			/**< Number of bytes transmitted. */
	uint32_t rx_dropped[NSS_MAX_NUM_PRI];	/**< Packets dropped on receive due to queue full. */
};

/**
 * nss_cmn_get_msg_len
 *	Gets the message length of a host-to-NSS message.
 *
 * @datatypes
 * nss_cmn_get_msg_len
 *
 * @param[in] ncm  Pointer to the common message.
 *
 * @return
 * Length of the message specified in the argument to this function.
 */
static inline uint32_t nss_cmn_get_msg_len(struct nss_cmn_msg *ncm)
{
	return ncm->len + sizeof(struct nss_cmn_msg);
}

#ifdef __KERNEL__ /* only for kernel to use. */

/**
 * nss_cmn_msg_init
 *	Initializes the common area of an asynchronous host-to-NSS message.
 *
 * @datatypes
 * nss_cmn_msg
 *
 * @param[in,out] ncm       Pointer to the common message.
 * @param[in]     if_num    NSS interface number.
 * @param[in]     type      Type of message.
 * @param[in]     len       Size of the payload.
 * @param[in]     cb        Pointer to the callback function.
 * @param[in]     app_data  Pointer to the application context for this message.
 *
 * @return
 * None.
 */
extern void nss_cmn_msg_init(struct nss_cmn_msg *ncm, uint32_t if_num, uint32_t type, uint32_t len,
	void *cb, void *app_data);

/**
 * nss_cmn_msg_sync_init
 *	Initializes the common message of a synchronous host-to-NSS message.
 *
 * @datatypes
 * nss_cmn_msg
 *
 * @param[in,out] ncm     Pointer to the common message.
 * @param[in]     if_num  NSS interface number.
 * @param[in]     type    Type of message.
 * @param[in]     len     Size of the payload.
 *
 * @return
 * None.
 */
extern void nss_cmn_msg_sync_init(struct nss_cmn_msg *ncm, uint32_t if_num, uint32_t type,  uint32_t len);

/**
 * nss_cmn_get_interface_number
 *	Gets the interface number.
 *
 * @datatypes
 * nss_ctx_instance \n
 * net_device
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 * @param[in] dev      Pointer to the OS network device pointer.
 *
 * @return
 * Interface number.
 */
extern int32_t nss_cmn_get_interface_number(struct nss_ctx_instance *nss_ctx, struct net_device *dev);

/**
 * nss_cmn_get_interface_number_by_dev
 *	Gets the interface number of a device.
 *
 * @datatypes
 * net_device
 *
 * @param[in] dev  Pointer to the OS network device pointer.
 *
 * @return
 * Interface number, or -1 on failure.
 */
extern int32_t nss_cmn_get_interface_number_by_dev(struct net_device *dev);

/**
 * nss_cmn_get_interface_number_by_dev_and_type
 *	Gets the interface number by a device and its type.
 *
 * @datatypes
 * net_device
 *
 * @param[in] dev   Pointer to the OS network device pointer.
 * @param[in] type  Type of this interface.
 *
 * @return
 * Interface number, or -1 on failure.
 */
extern int32_t nss_cmn_get_interface_number_by_dev_and_type(struct net_device *dev, uint32_t type);

/**
 * nss_cmn_interface_is_redirect
 *	Determines if the interface number is a redirect interface.
 *
 * @param[in] nss_ctx        Pointer to the NSS context.
 * @param[in] interface_num  NSS interface number.
 *
 * @return
 * TRUE if the number is a redirect interface. Otherwise FALSE.
 */
extern bool nss_cmn_interface_is_redirect(struct nss_ctx_instance *nss_ctx, int32_t interface_num);

/**
 * nss_cmn_append_core_id
 * 	Append core ID on NSS interface number.
 *
 * @datatypes
 * nss_ctx_instance
 *
 * @param[in] nss_ctx   Pointer to the NSS context.
 * @param[in] if_num    NSS interface number.
 *
 * @return
 * Interface number with core ID.
 */
extern int nss_cmn_append_core_id(struct nss_ctx_instance *nss_ctx, int if_num);

/**
 * nss_cmn_get_interface_dev
 *	Gets an interface device pointer.
 *
 * @datatypes
 * nss_ctx_instance
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 * @param[in] if_num   NSS interface number.
 *
 * @return
 * Interface device pointer.
 */
extern struct net_device *nss_cmn_get_interface_dev(struct nss_ctx_instance *nss_ctx, uint32_t if_num);

/**
 * nss_cmn_get_state
 *	Obtains the NSS state.
 *
 * @datatypes
 * nss_ctx_instance
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 *
 * @return
 * NSS state that indicates whether the NSS core is initialized. For possible values, see nss_state_t.
 */
extern nss_state_t nss_cmn_get_state(struct nss_ctx_instance *nss_ctx);

/**
 * Callback function for queue decongestion messages.
 *
 * @param[in] app_data  Pointer to the application context for this message.
 */
typedef void (*nss_cmn_queue_decongestion_callback_t)(void *app_data);

/**
 * nss_cmn_register_queue_decongestion
 *	Registers a queue for a decongestion event.
 *
 * The callback function is called with the spinlock held. The function should avoid deadlocks
 * caused by attempting to acquire multiple spinlocks.

 * @datatypes
 * nss_ctx_instance \n
 * nss_cmn_queue_decongestion_callback_t
 *
 * @param[in,out] nss_ctx         Pointer to the NSS context.
 * @param[in]     event_callback  Callback for the message.
 * @param[in]     app_data        Pointer to the application context to be returned in the
 *                                callback.
 *
 * @return
 * #NSS_CB_REGISTER_SUCCESS if registration is successful.
 * @par
 * Otherwise, #NSS_CB_REGISTER_FAILED.
 */
extern nss_cb_register_status_t nss_cmn_register_queue_decongestion(struct nss_ctx_instance *nss_ctx, nss_cmn_queue_decongestion_callback_t event_callback, void *app_data);

/**
 * nss_cmn_unregister_queue_decongestion
 *	Deregisters a queue from receiving a decongestion event.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_cmn_queue_decongestion_callback_t
 *
 * @param[in,out] nss_ctx         Pointer to the NSS context.
 * @param[in]     event_callback  Callback for the message.
 *
 * @return
 * #NSS_CB_REGISTER_SUCCESS if registration is successful.
 * @par
 * Otherwise, #NSS_CB_REGISTER_FAILED.
 *
 * @dependencies
 * The callback function must have been previously registered.
 */
extern nss_cb_unregister_status_t nss_cmn_unregister_queue_decongestion(struct nss_ctx_instance *nss_ctx, nss_cmn_queue_decongestion_callback_t event_callback);

/**
 * Callback function for packets with service code.
 *
 * @param[in] app_data  Pointer to the application context for this message.
 * @param[in] nbuf      Pointer to the socket buffer.
 */
typedef void (*nss_cmn_service_code_callback_t)(void *app_data, struct sk_buff *nbuf);

/**
 * nss_cmn_register_service_code
 *	Registers a callback for a service code.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_cmn_service_code_callback_t
 *
 * @param[in,out] nss_ctx         Pointer to the NSS context.
 * @param[in]     cb              Callback for the message.
 * @param[in]     service_code    Service code found attached to the packet.
 * @param[in]     app_data        Pointer to the application context to be returned in the
 *                                callback.
 *
 * @return
 * #NSS_CB_REGISTER_SUCCESS if registration is successful.
 * @par
 * Otherwise, #NSS_CB_REGISTER_FAILED.
 */
extern nss_cb_register_status_t nss_cmn_register_service_code(struct nss_ctx_instance *nss_ctx, nss_cmn_service_code_callback_t cb, uint8_t service_code, void *app_data);

/**
 * nss_cmn_unregister_service_code
 *	Deregisters a callback for the given service code.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_cmn_service_code_callback_t
 *
 * @param[in,out] nss_ctx         Pointer to the NSS context.
 * @param[in]     cb              Callback for the message.
 * @param[in]     service_code    Service code found attached to the packet.
 *
 * @return
 * #NSS_CB_REGISTER_SUCCESS if registration is successful.
 * @par
 * Otherwise, #NSS_CB_REGISTER_FAILED.
 *
 * @dependencies
 * The callback function must have been previously registered.
 */
extern nss_cb_unregister_status_t nss_cmn_unregister_service_code(struct nss_ctx_instance *nss_ctx, nss_cmn_service_code_callback_t cb, uint8_t service_code);

/**
 * nss_cmn_get_nss_enabled
 *	Checks whether the NSS mode is supported on the platform.
 *
 * @return
 * TRUE if NSS is supported. \n
 * Otherwise, FALSE.
 */
extern bool nss_cmn_get_nss_enabled(void);

/**
 * nss_cmn_rx_dropped_sum
 *	Sums dropped packet count of all NSS pnode queues.
 *
 * @datatypes
 * nss_cmn_node_stats \n
 *
 * @param[in] node_stats  Pointer to node statistics.
 *
 * @return
 * Total dropped packets count.
 */
extern uint32_t nss_cmn_rx_dropped_sum(struct nss_cmn_node_stats *node_stats);

#endif /* __KERNEL__ */

/**
 * @}
 */

#endif /* __NSS_CMN_MSG_H */

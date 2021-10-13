/*
 **************************************************************************
 * Copyright (c) 2015, 2017 The Linux Foundation. All rights reserved.
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
 * @file nss_portid.h
 *	NSS Port ID interface definitions.
 */

#ifndef __NSS_PORTID_H
#define __NSS_PORTID_H

/**
 * @addtogroup nss_portid_subsystem
 * @{
 */

/**
 * Maximum number of ports on the S17c switch chip.
 *
 * If a new switch has more ports than S17c, this value must be updated.
 */
#define NSS_PORTID_MAX_SWITCH_PORT 7

/**
 * nss_portid_msg_types
 *	Message types for port ID requests and responses.
 */
enum nss_portid_msg_types {
	NSS_PORTID_CONFIGURE_MSG,
	NSS_PORTID_UNCONFIGURE_MSG,
	NSS_PORTID_STATS_SYNC_MSG,
	NSS_PORTID_MAX_MSG_TYPE
};

/**
 * nss_portid_configure_msg
 *	Message information for configuring a port ID.
 */
struct nss_portid_configure_msg {
	uint32_t port_if_num;
			/**< Interface number corresponding to the port ID of the device. */
	uint8_t port_id;		/**< ID of the mapped switch port. */
	uint8_t gmac_id;		/**< ID of the mapped GMAC interface. */
	uint8_t reserved[2];		/**< Reserved for word alignment. */
};

/**
 * nss_portid_unconfigure_msg
 *	Message information for deleting a port ID.
 */
struct nss_portid_unconfigure_msg {
	uint32_t port_if_num;
			/**< Interface number corresponding to the port ID of the device. */
	uint8_t port_id;		/**< ID of the mapped switch port. */
	uint8_t reserved[3];		/**< Reserved for word alignment. */
};

/**
 * nss_portid_stats_sync_msg
 *	Message information for port ID synchronization statistics.
 */
struct nss_portid_stats_sync_msg {
	struct nss_cmn_node_stats node_stats;	/**< Common node statistics. */
	uint32_t rx_invalid_header;		/**< Rx with an invalid header. */
	uint8_t port_id;			/**< ID of the mapped switch port. */
	uint8_t reserved[3];			/**< Reserved for word alignment. */
};

/**
 * nss_portid_msg
 *	Data for sending and receiving port ID messages.
 */
struct nss_portid_msg {
	struct nss_cmn_msg cm;		/**< Common message header. */

	/**
	 * Payload of a port ID message.
	 */
	union {
		struct nss_portid_configure_msg configure;
				/**< Port ID configuration information. */
		struct nss_portid_unconfigure_msg unconfigure;
				/**< Port ID de-configuration information. */
		struct nss_portid_stats_sync_msg stats_sync;
				/**< Synchronization statistics for the port ID. */
	} msg;			/**< Message payload. */
};

/**
 * Callback function for receiving port ID messages.
 *
 * @datatypes
 * nss_portid_msg
 *
 * @param[in] app_data  Pointer to the application context of the message.
 * @param[in] npm       Pointer to the NSS Profiler message.
 */
typedef void (*nss_portid_msg_callback_t)(void *app_data, struct nss_portid_msg *npm);

/**
 * Callback function for receiving port ID interface data.
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
typedef void (*nss_portid_buf_callback_t)(struct net_device *netdev, struct sk_buff *skb, struct napi_struct *napi);

/**
 * nss_portid_get_stats
 *	Gets statistics from a port interface.
 *
 * @datatypes
 * rtnl_link_stats64
 *
 * @param[in]  if_num  NSS interface number.
 * @param[out] stats   Container for the statistic counters.
 *
 * @return
 * TRUE or FALSE.
 */
bool nss_portid_get_stats(uint32_t if_num, struct rtnl_link_stats64 *stats);

/**
 * nss_portid_msg_init
 *	Initializes a port ID message.
 *
 * @datatypes
 * nss_portid_msg \n
 * nss_portid_msg_callback_t
 *
 * @param[in,out] npm       Pointer to the NSS port ID message.
 * @param[in]     if_num    NSS interface number.
 * @param[in]     type      Type of message.
 * @param[in]     len       Size of the payload.
 * @param[in]     cb        Callback function for the message.
 * @param[in]     app_data  Pointer to the application context of the message.
 *
 * @return
 * None.
 */
extern void nss_portid_msg_init(struct nss_portid_msg *npm, uint16_t if_num, uint32_t type, uint32_t len,
							nss_portid_msg_callback_t cb, void *app_data);

/**
 * nss_portid_if_tx_data
 *	Transmits a data packet to the NSS port ID interface.
 *
 * @datatypes
 * nss_ctx_instance \n
 * sk_buff
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 * @param[in] os_buf   Pointer to the OS data buffer.
 * @param[in] if_num   NSS interface number.
 *
 * @return
 * Status of the Tx operation.
 */
extern nss_tx_status_t nss_portid_if_tx_data(struct nss_ctx_instance *nss_ctx, struct sk_buff *os_buf, uint32_t if_num);

/**
 * nss_portid_tx_msg
 *	Sends a port ID message to the NSS.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_portid_msg
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 * @param[in] msg      Pointer to the message data.
 *
 * @return
 * Status of the Tx operation.
 */
extern nss_tx_status_t nss_portid_tx_msg(struct nss_ctx_instance *nss_ctx, struct nss_portid_msg *msg);

/**
 * nss_portid_tx_msg_sync
 *	Sends a port ID message to the NSS and waits for a response.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_portid_msg
 *
 * @param[in]     nss_ctx  Pointer to the NSS context.
 * @param[in,out] msg      Pointer to the message data.
 *
 * @return
 * Status of the Tx operation.
 */
extern nss_tx_status_t nss_portid_tx_msg_sync(struct nss_ctx_instance *nss_ctx, struct nss_portid_msg *msg);

/**
 * nss_portid_register_port_if
 *	Registers the port interface with the NSS.
 *
 * @datatypes
 * net_device \n
 * nss_portid_buf_callback_t
 *
 * @param[in] if_num   NSS interface number.
 * @param[in] port_id  Physical port ID of this interface.
 * @param[in] ndev     Pointer to the associated network device.
 * @param[in] buf_cb   Callback for the data.
 *
 * @return
 * Pointer to the NSS core context.
 */
extern struct nss_ctx_instance *nss_portid_register_port_if(uint32_t if_num, uint32_t port_id, struct net_device *ndev, nss_portid_buf_callback_t buf_cb);

/**
 * nss_portid_unregister_port_if
 *	Deregisters the port interface from the NSS.
 *
 * @param[in] if_num  NSS interface number.
 *
 * @return
 * TRUE or FALSE.
 *
 * @dependencies
 * The interface must have been previously registered.
 */
extern bool nss_portid_unregister_port_if(uint32_t if_num);

/**
 * nss_portid_tx_configure_port_if_msg
 *	Sends a port interface configuration message to the NSS.
 *
 * @datatypes
 * nss_ctx_instance
 *
 * @param[in] nss_ctx      Pointer to the NSS context.
 * @param[in] port_if_num  Interface number of the port node.
 * @param[in] port_id      ID of the mapped switch port.
 * @param[in] gmac_id      ID of the mapped GMAC interface.
 *
 * @return
 * Status of the Tx operation.
 */
extern nss_tx_status_t nss_portid_tx_configure_port_if_msg(struct nss_ctx_instance *nss_ctx, uint32_t port_if_num, uint8_t port_id, uint8_t gmac_id);

/**
 * nss_portid_tx_unconfigure_port_if_msg
 *	Sends a port interface de-configuration message to the NSS.
 *
 * @datatypes
 * nss_ctx_instance
 *
 * @param[in] nss_ctx      Pointer to the NSS context.
 * @param[in] port_if_num  Interface number of the port node.
 * @param[in] port_id      ID of the mapped switch port.
 *
 * @return
 * Status of the Tx operation.
 *
 * @dependencies
 * The interface must have been previously configured.
 */
extern nss_tx_status_t nss_portid_tx_unconfigure_port_if_msg(struct nss_ctx_instance *nss_ctx, uint32_t port_if_num, uint8_t port_id);

/**
 * @}
 */

#endif /* __NSS_PORTID_H */

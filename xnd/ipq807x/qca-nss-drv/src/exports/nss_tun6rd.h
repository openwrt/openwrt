/*
 **************************************************************************
 * Copyright (c) 2014, 2017-2019, The Linux Foundation. All rights reserved.
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
 * @file nss_tun6rd.h
 *	NSS TUN6RD interface definitions.
 */

#ifndef __NSS_TUN6RD_H
#define __NSS_TUN6RD_H

/**
 * @addtogroup nss_tun6rd_subsystem
 * @{
 */

/**
 * nss_tun6rd_metadata_types
 *	Message types for 6RD (IPv6 in IPv4) tunnel requests and responses.
 */
enum nss_tun6rd_metadata_types {
	NSS_TUN6RD_ATTACH_PNODE,
	NSS_TUN6RD_RX_STATS_SYNC,
	NSS_TUN6RD_ADD_UPDATE_PEER,
	NSS_TUN6RD_MAX,
};

/**
 * nss_tun6rd_attach_tunnel_msg
 *	Message information for configuring the 6RD tunnel.
 */
struct nss_tun6rd_attach_tunnel_msg {
	uint32_t saddr;			/**< Source address of the tunnel. */
	uint32_t daddr;			/**< Destination address of the tunnel. */
	uint8_t  tos;			/**< Type Of Service field added to the outer header. */
	uint8_t  ttl;			/**< Time-to-live value for the tunnel. */
	uint32_t sibling_if_num;	/**< Sibling interface number. */
	uint16_t reserved;		/**< Reserved field added for alignment. */
};

/**
 * nss_tun6rd_sync_stats_msg
 *	Message information for 6RD tunnel synchronization statistics.
 */
struct nss_tun6rd_sync_stats_msg {
	struct nss_cmn_node_stats node_stats;	/**< Common node statistics. */
};

/**
 * nss_tun6rd_set_peer_msg
 *	Message information for the 6RD tunnel peer address.
 */
struct nss_tun6rd_set_peer_msg {
	uint32_t ipv6_address[4];	/**< IPv6 address. */
	uint32_t dest;			/**< IPv4 address. */
};

/**
 * nss_tun6rd_msg
 *	Data for sending and receiving 6RD tunnel messages.
 */
struct nss_tun6rd_msg {
	struct nss_cmn_msg cm;		/**< Common message header. */

	/**
	 * Payload of a 6RD tunnel message.
	 */
	union {
		struct nss_tun6rd_attach_tunnel_msg tunnel;
				/**< Attach a 6RD tunnel. */
		struct nss_tun6rd_sync_stats_msg stats;
				/**< Synchronized statistics for the interface. */
		struct nss_tun6rd_set_peer_msg peer;
				/**< Add or update the peer. */
	} msg;			/**< Message payload for 6RD tunnel messages exchanged with NSS core. */
};

/**
 * Callback function for receiving 6RD tunnel messages.
 *
 * @datatypes
 * nss_tun6rd_msg
 *
 * @param[in] app_data  Pointer to the application context of the message
 * @param[in] msg       Pointer to the message data.
 */
typedef void (*nss_tun6rd_msg_callback_t)(void *app_data, struct nss_tun6rd_msg *msg);

/**
 * nss_tun6rd_tx
 *	Sends a 6RD tunnel message.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_tun6rd_msg
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 * @param[in] msg      Pointer to the message data.
 *
 * @return
 * Status of the Tx operation.
 */
extern nss_tx_status_t nss_tun6rd_tx(struct nss_ctx_instance *nss_ctx, struct nss_tun6rd_msg *msg);

/**
 * nss_tun6rd_get_context
 *	Gets the TUN6RD context used in nss_tun6rd_tx().
 *
 * @return
 * Pointer to the NSS core context.
 */
extern struct nss_ctx_instance *nss_tun6rd_get_context(void);

/**
 * Callback function for receiving 6RD tunnel data.
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
typedef void (*nss_tun6rd_callback_t)(struct net_device *netdev, struct sk_buff *skb, struct napi_struct *napi);

/**
 * nss_register_tun6rd_if
 *	Registers the TUN6RD interface with the NSS for sending and receiving messages.
 *
 * @datatypes
 * nss_tun6rd_callback_t \n
 * nss_tun6rd_msg_callback_t \n
 * net_device
 *
 * @param[in] if_num           NSS interface number.
 * @param[in] type             NSS interface type.
 * @param[in] tun6rd_callback  Callback for the data.
 * @param[in] msg_callback     Callback for the message.
 * @param[in] netdev           Pointer to the associated network device.
 * @param[in] features         Data socket buffer types supported by this interface.
 *
 * @return
 * Pointer to the NSS core context.
 */
extern struct nss_ctx_instance *nss_register_tun6rd_if(uint32_t if_num, uint32_t type, nss_tun6rd_callback_t tun6rd_callback,
					nss_tun6rd_msg_callback_t msg_callback, struct net_device *netdev, uint32_t features);

/**
 * nss_unregister_tun6rd_if
 *	Deregisters the TUN6RD interface from the NSS.
 *
 * @param[in] if_num  NSS interface number.
. *
 * @return
 * None.
 *
 * @dependencies
 * The 6RD tunnel interface must have been previously registered.
 */
extern void nss_unregister_tun6rd_if(uint32_t if_num);

/**
 * nss_tun6rd_msg_init
 *	Initializes a TUN6RD message.
 *
 * @datatypes
 * nss_tun6rd_msg
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
extern void nss_tun6rd_msg_init(struct nss_tun6rd_msg *ncm, uint16_t if_num, uint32_t type,  uint32_t len, void *cb, void *app_data);

/** @} */ /* end_addtogroup nss_tun6rd_subsystem */

#endif /* __NSS_TUN6RD_H */

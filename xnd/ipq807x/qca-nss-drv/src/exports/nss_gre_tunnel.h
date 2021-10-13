/*
 ****************************************************************************
 * Copyright (c) 2016-2018, 2020-2021, The Linux Foundation. All rights reserved.
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
 ****************************************************************************
 */

/**
 * @file nss_gre_tunnel.h
 *	NSS GRE Tunnel interface definitions.
 */

#ifndef __NSS_GRE_TUNNEL_H
#define __NSS_GRE_TUNNEL_H

/**
 * @addtogroup nss_gre_tunnel_subsystem
 * @{
 */

/**
 * Maximum number of supported GRE tunnel sessions.
 */
#define NSS_MAX_GRE_TUNNEL_SESSIONS 16

/**
 * nss_gre_tunnel_message_types
 *	Message types for a GRE tunnel rule.
 */
enum nss_gre_tunnel_message_types {
	NSS_GRE_TUNNEL_MSG_CONFIGURE,
	NSS_GRE_TUNNEL_MSG_SESSION_DESTROY,
	NSS_GRE_TUNNEL_MSG_STATS,
	NSS_GRE_TUNNEL_MSG_CONFIGURE_DI_TO_WLAN_ID,
	NSS_GRE_TUNNEL_MSG_INQUIRY,
	NSS_GRE_TUNNEL_MSG_MAX,
};

/**
 * nss_gre_tunnel_encrypt_types
 *	Encryption types for a GRE tunnel.
 */
enum nss_gre_tunnel_encrypt_types {
	NSS_GRE_TUNNEL_ENCRYPT_NONE,
	NSS_GRE_TUNNEL_ENCRYPT_AES128_CBC,
	NSS_GRE_TUNNEL_ENCRYPT_AES256_CBC,
	NSS_GRE_TUNNEL_ENCRYPT_MAX,
};

/**
 * nss_gre_tunnel_mode_types
 *	Mode types for a GRE tunnel.
 */
enum nss_gre_tunnel_mode_types {
	NSS_GRE_TUNNEL_MODE_GRE,
	NSS_GRE_TUNNEL_MODE_GRE_UDP,
	NSS_GRE_TUNNEL_MODE_MAX,
};

/**
 * nss_gre_tunnel_ip_types
 *	IP types for a GRE tunnel.
 */
enum nss_gre_tunnel_ip_types {
	NSS_GRE_TUNNEL_IP_IPV4,
	NSS_GRE_TUNNEL_IP_IPV6,
	NSS_GRE_TUNNEL_IP_MAX,
};

/**
 * nss_gre_tunnel_error_types
 *	Error types for a GRE tunnel.
 */
enum nss_gre_tunnel_error_types {
	NSS_GRE_TUNNEL_ERR_UNKNOWN_MSG = 1,
	NSS_GRE_TUNNEL_ERR_IF_INVALID = 2,
	NSS_GRE_TUNNEL_ERR_CPARAM_INVALID = 3,
	NSS_GRE_TUNNEL_ERR_MODE_INVALID = 4,
	NSS_GRE_TUNNEL_ERR_ENCRYPT_INVALID = 5,
	NSS_GRE_TUNNEL_ERR_IP_INVALID = 6,
	NSS_GRE_TUNNEL_ERR_ENCRYPT_IDX_INVALID = 7,
	NSS_GRE_TUNNEL_ERR_NOMEM = 8,
	NSS_GRE_TUNNEL_ERR_PROTO_TEB_INVALID = 9,
	NSS_GRE_TUNNEL_ERR_SIBLING_IF = 10,
	NSS_GRE_TUNNEL_ERR_CRYPTO_NODE_ID = 11,
	NSS_GRE_TUNNEL_ERR_RPS = 12,
	NSS_GRE_TUNNEL_ERR_DI_INVALID = 13,
	NSS_GRE_TUNNEL_ERR_MAX,
};

/**
 *  nss_gre_tunnel_stats_type
 *	GRE tunnel session debug statistic counters.
 */
enum nss_gre_tunnel_stats_type {
	NSS_GRE_TUNNEL_STATS_SESSION_RX_PKTS,			/**< Number of packets received. */
	NSS_GRE_TUNNEL_STATS_SESSION_TX_PKTS,			/**< Number of packets transmitted. */
	NSS_GRE_TUNNEL_STATS_SESSION_RX_QUEUE_0_DROPPED,	/**< Dropped receive packets 0. */
	NSS_GRE_TUNNEL_STATS_SESSION_RX_QUEUE_1_DROPPED,	/**< Dropped receive packets 1. */
	NSS_GRE_TUNNEL_STATS_SESSION_RX_QUEUE_2_DROPPED,	/**< Dropped receive packets 2. */
	NSS_GRE_TUNNEL_STATS_SESSION_RX_QUEUE_3_DROPPED,	/**< Dropped receive packets 3. */
	NSS_GRE_TUNNEL_STATS_SESSION_RX_MALFORMED,		/**< Malformed packet was received. */
	NSS_GRE_TUNNEL_STATS_SESSION_RX_INVALID_PROT,		/**< Invalid protocol was received. */
	NSS_GRE_TUNNEL_STATS_SESSION_DECAP_QUEUE_FULL,		/**< Decapsulation queue is full. */
	NSS_GRE_TUNNEL_STATS_SESSION_RX_SINGLE_REC_DGRAM,	/**< Single fragment was received. */
	NSS_GRE_TUNNEL_STATS_SESSION_RX_INVALID_REC_DGRAM,	/**< Invalid fragment was received. */
	NSS_GRE_TUNNEL_STATS_SESSION_BUFFER_ALLOC_FAIL,		/**< Buffer memory allocation failed. */
	NSS_GRE_TUNNEL_STATS_SESSION_BUFFER_COPY_FAIL,		/**< Buffer memory copy failed. */
	NSS_GRE_TUNNEL_STATS_SESSION_OUTFLOW_QUEUE_FULL,	/**< Outflow queue is full. */
	NSS_GRE_TUNNEL_STATS_SESSION_RX_DROPPED_HROOM,	/**< Packets dropped because of insufficent headroom. */
	NSS_GRE_TUNNEL_STATS_SESSION_RX_CBUFFER_ALLOC_FAIL,	/**< Receive crypto buffer allocation failed. */
	NSS_GRE_TUNNEL_STATS_SESSION_RX_CENQUEUE_FAIL,		/**< Receive enqueue-to-crypto failed. */
	NSS_GRE_TUNNEL_STATS_SESSION_RX_DECRYPT_DONE,		/**< Receive decryption is complete. */
	NSS_GRE_TUNNEL_STATS_SESSION_RX_FORWARD_ENQUEUE_FAIL,	/**< Receive forward enqueue failed. */
	NSS_GRE_TUNNEL_STATS_SESSION_TX_CBUFFER_ALLOC_FAIL,	/**< Receive crypto buffer allocation failed. */
	NSS_GRE_TUNNEL_STATS_SESSION_TX_CENQUEUE_FAIL,		/**< Transmit enqueue-to-crypto failed. */
	NSS_GRE_TUNNEL_STATS_SESSION_RX_DROPPED_TROOM,	/**< Packets dropped because of insufficent tailroom. */
	NSS_GRE_TUNNEL_STATS_SESSION_TX_FORWARD_ENQUEUE_FAIL,	/**< Transmit forward enqueue failed. */
	NSS_GRE_TUNNEL_STATS_SESSION_TX_CIPHER_DONE,		/**< Transmit cipher is complete. */
	NSS_GRE_TUNNEL_STATS_SESSION_CRYPTO_NOSUPP,		/**< Error count for non-supported crypto packets. */
	NSS_GRE_TUNNEL_STATS_SESSION_RX_DROPPED_MH_VERSION,	/**< Receive drop: bad meta header. */
	NSS_GRE_TUNNEL_STATS_SESSION_RX_UNALIGNED_PKT,		/**< Counter for unaligned packets. */
	NSS_GRE_TUNNEL_STATS_SESSION_MAX,			/**< Maximum statistics type. */
};

/**
 * nss_gre_tunnel_di_to_wlan_id
 *	Dynamic interface to WLAN ID message structure.
 */
struct nss_gre_tunnel_di_to_wlan_id {
	uint32_t dynamic_interface_num;		/**< Dynamic interface number. */
	uint16_t wlan_id;			/**< WLAN ID number. */
	uint16_t fwd_policy;                    /**< Forward policy bits. */
};

/**
 * nss_gre_tunnel_configure
 *	Message information for configuring a GRE tunnel.
 */
struct nss_gre_tunnel_configure {
	uint32_t mh_version;		/**< Meta header version. */
	uint8_t gre_mode;		/**< GRE or GRE plus UDP. */
	uint8_t ip_type;		/**< IPv4 or IPv6. */
	uint16_t encrypt_type;		/**< Encryption type. */
	uint32_t src_ip[4];		/**< Source IPv4 or IPv6 address. */
	uint32_t dest_ip[4];		/**< Destination IPv4 or IPv6 address. */
	uint16_t src_port;		/**< GRE plus UDP only for the source. */
	uint16_t dest_port;		/**< GRE plus UDP only for the destination. */
	uint32_t crypto_node_id;	/**< Cryto node identifier. */
	uint32_t crypto_idx_encrypt;	/**< Crypto index for encryption. */
	uint32_t crypto_idx_decrypt;	/**< Crypto index for decryption. */
	uint32_t word0;			/**< Word0 header. */
	uint8_t iv_val[16];		/**< Initialization vector value. */
	uint32_t sibling_if;		/**< Sibling interface number. */
	uint8_t ttl;			/**< Time-to-live value of the IP header. */
	int8_t rps;			/**< Steer packets to host core. */
	uint16_t reserved;		/**< Reserved space. */
	uint32_t word1;			/**< Word1 header. */
	uint32_t word2;			/**< Word2 header. */
	uint32_t word3;			/**< Word3 header. */
};

/**
 * nss_gre_tunnel_stats
 *	Message statistics for a GRE tunnel.
 */
struct nss_gre_tunnel_stats {
	struct nss_cmn_node_stats node_stats;	/**< Common node statistics. */
	uint32_t rx_malformed;			/**< Malformed packet was received. */
	uint32_t rx_invalid_prot;		/**< Invalid protocol was received. */
	uint32_t decap_queue_full;		/**< Decapsulation queue is full. */
	uint32_t rx_single_rec_dgram;		/**< Single fragment was received. */
	uint32_t rx_invalid_rec_dgram;		/**< Invalid fragment was received. */
	uint32_t buffer_alloc_fail;		/**< Buffer memory allocation failed. */
	uint32_t buffer_copy_fail;		/**< Buffer memory copy failed. */
	uint32_t outflow_queue_full;		/**< Outflow queue is full. */
	uint32_t rx_dropped_hroom;
				/**< Packets dropped because of insufficent headroom. */
	uint32_t rx_cbuf_alloc_fail;
				/**< Receive crypto buffer allocation failed. */
	uint32_t rx_cenqueue_fail;		/**< Receive enqueue-to-crypto failed. */
	uint32_t rx_decrypt_done;		/**< Receive decryption is complete. */
	uint32_t rx_forward_enqueue_fail;	/**< Receive forward enqueue failed. */
	uint32_t tx_cbuf_alloc_fail;
				/**< Receive crypto buffer allocation failed. */
	uint32_t tx_cenqueue_fail;		/**< Transmit enqueue-to-crypto failed. */
	uint32_t rx_dropped_troom;
				/**< Packets dropped because of insufficent tailroom. */
	uint32_t tx_forward_enqueue_fail;	/**< Transmit forward enqueue failed. */
	uint32_t tx_cipher_done;		/**< Transmit cipher is complete. */
	uint32_t crypto_nosupp;
				/**< Error count for non-supported crypto packets. */
	uint32_t rx_dropped_mh_ver;		/**< Receive drop: bad meta header. */
	uint32_t rx_unaligned_pkt;		/**< Counter for unaligned packets. */
#if defined(NSS_HAL_IPQ807x_SUPPORT)
	uint32_t crypto_resp_error[NSS_CRYPTO_CMN_RESP_ERROR_MAX];
						/** Crypto response errors. */
#endif
};

/**
 * nss_gre_tunnel_stats_notification
 *	GRE tunnel transmission statistics structure.
 */
struct nss_gre_tunnel_stats_notification {
	uint64_t stats_ctx[NSS_GRE_TUNNEL_STATS_SESSION_MAX + NSS_CRYPTO_CMN_RESP_ERROR_MAX];
							/**< Context transmission statistics. */
	uint32_t core_id;				/**< Core ID. */
	uint32_t if_num;				/**< Interface number. */
};

/**
 * nss_gre_tunnel_msg
 *	Data for sending and receiving GRE tunnel messages.
 */
struct nss_gre_tunnel_msg {
	struct nss_cmn_msg cm;					/**< Common message header. */

	/**
	 * Payload of a GRE tunnel message.
	 */
	union {
		struct nss_gre_tunnel_configure configure;	/**< Tunnel configuration data. */
		struct nss_gre_tunnel_stats stats;		/**< Tunnel statistics. */
		struct nss_gre_tunnel_di_to_wlan_id dtwi;	/**< Tunnel dynamic interface number to WLAN ID mapping. */
	} msg;							/**< Message payload. */
};

/**
 * Callback function for receiving GRE tunnel messages.
 *
 * @datatypes
 * nss_gre_tunnel_msg
 *
 * @param[in] app_data  Pointer to the application context of the message.
 * @param[in] msg       Pointer to the message data.
 */
typedef void (*nss_gre_tunnel_msg_callback_t)(void *app_data, struct nss_gre_tunnel_msg *msg);

/**
 * Callback function for receiving GRE tunnel session data.
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
typedef void (*nss_gre_tunnel_data_callback_t)(struct net_device *netdev, struct sk_buff *skb, struct napi_struct *napi);

/**
 * nss_gre_tunnel_tx_buf
 *	Sends a GRE tunnel packet.
 *
 * @datatypes
 * sk_buff \n
 * nss_ctx_instance
 *
 * @param[in] skb      Pointer to the data socket buffer.
 * @param[in] if_num   Tunnel interface number.
 * @param[in] nss_ctx  Pointer to the NSS context.
 *
 * @return
 * Status of the Tx operation.
 */
extern nss_tx_status_t nss_gre_tunnel_tx_buf(struct sk_buff *skb, uint32_t if_num, struct nss_ctx_instance *nss_ctx);

/**
 * nss_gre_tunnel_tx_msg
 *	Sends a GRE tunnel message.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_gre_tunnel_msg
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 * @param[in] msg      Pointer to the message data.
 *
 * @return
 * Status of the Tx operation.
 */
extern nss_tx_status_t nss_gre_tunnel_tx_msg(struct nss_ctx_instance *nss_ctx, struct nss_gre_tunnel_msg *msg);

/**
 * nss_gre_tunnel_tx_msg_sync
 *	Sends a GRE tunnel message synchronously.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_gre_tunnel_msg
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 * @param[in] msg      Pointer to the message data.
 *
 * @return
 * Status of the Tx operation.
 */
extern nss_tx_status_t nss_gre_tunnel_tx_msg_sync(struct nss_ctx_instance *nss_ctx, struct nss_gre_tunnel_msg *msg);

/**
 * nss_gre_tunnel_msg_init
 *	Initalizes a GRE tunnel message.
 *
 * @datatypes
 * nss_gre_tunnel_msg
 *
 * @param[in] ngtm      Pointer to the tunnel message.
 * @param[in] if_num    Tunnel interface number.
 * @param[in] type      Type of message.
 * @param[in] len       Size of the payload.
 * @param[in] cb        Pointer to the message callback.
 * @param[in] app_data  Pointer to the application context of the message.
 *
 * @return
 * None.
 */
extern void nss_gre_tunnel_msg_init(struct nss_gre_tunnel_msg *ngtm, uint16_t if_num, uint32_t type, uint32_t len, void *cb, void *app_data);

/**
 * nss_gre_tunnel_get_ctx
 *	Returns the NSS context.
 *
 * @return
 * Pointer to the NSS core context.
 */
extern struct nss_ctx_instance *nss_gre_tunnel_get_ctx(void);

/**
 * nss_gre_tunnel_register_if
 *	Registers a network device with the NSS for sending and receiving tunnel
 *	messages.
 *
 * @datatypes
 * nss_gre_tunnel_data_callback_t \n
 * nss_gre_tunnel_msg_callback_t \n
 * net_device
 *
 * @param[in] if_num    NSS interface number.
 * @param[in] cb        Callback function for the message.
 * @param[in] ev_cb     Callback for the GRE tunnel message.
 * @param[in] netdev    Pointer to the associated network device.
 * @param[in] features  Data socket buffer types supported by this interface.
 * @param[in] app_ctx   Pointer to the application context.
 *
 * @return
 * Pointer to the NSS core context.
 */
extern struct nss_ctx_instance *nss_gre_tunnel_register_if(uint32_t if_num,
						nss_gre_tunnel_data_callback_t cb,
						nss_gre_tunnel_msg_callback_t ev_cb,
						struct net_device *netdev,
						uint32_t features,
						void *app_ctx);

/**
 * nss_gre_tunnel_unregister_if
 *	Deregisters a network device from the NSS.
 *
 * @param[in] if_num  NSS interface number.
 *
 * @return
 * None.
 *
 * @dependencies
 * The network device must have been previously registered.
 */
extern void nss_gre_tunnel_unregister_if(uint32_t if_num);

/**
 * nss_gre_tunnel_inquiry()
 *	Inquiry if a GRE tunnel has been established in NSS FW.
 *
 * @param[in] inquiry_info  Query parameters similar to creation parameters.
 * @param[in] cb            Pointer to the message callback.
 * @param[in] app_data      Pointer to the application context of the message.
 *
 * @return
 * Status of the Tx operation
 */
extern nss_tx_status_t nss_gre_tunnel_inquiry(
		struct nss_gre_tunnel_configure *inquiry_info,
		nss_gre_tunnel_msg_callback_t cb, void *app_data);

/**
 * nss_gre_tunnel_stats_unregister_notifier
 *	Deregisters a statistics notifier.
 *
 * @datatypes
 *	notifier_block
 *
 * @param[in] nb Notifier block.
 *
 * @return
 * 0 on success or non-zero on failure.
 */
extern int nss_gre_tunnel_stats_unregister_notifier(struct notifier_block *nb);

/**
 * nss_gre_tunnel_stats_register_notifier
 *	Registers a statistics notifier.
 *
 * @datatypes
 *	notifier_block
 *
 * @param[in] nb Notifier block.
 *
 * @return
 * 0 on success or non-zero on failure.
 */
extern int nss_gre_tunnel_stats_register_notifier(struct notifier_block *nb);

/**
 * @}
 */

#endif /* __NSS_GRE_TUNNEL_H */

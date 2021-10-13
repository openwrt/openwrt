/*
 **************************************************************************
 * Copyright (c) 2016-2017, The Linux Foundation. All rights reserved.
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
 * @file nss_dtls.h
 *	NSS DTLS interface definitions.
 */

#ifndef _NSS_DTLS_H_
#define _NSS_DTLS_H_

/**
 * @addtogroup nss_dtls_subsystem
 * @{
 */

#define NSS_MAX_DTLS_SESSIONS 8		/**< Maximum number of supported DTLS sessions. */

/**
 * nss_dtls_metadata_types
 *	Message types for DTLS requests and responses.
 */
enum nss_dtls_metadata_types {
	NSS_DTLS_MSG_SESSION_CONFIGURE,
	NSS_DTLS_MSG_SESSION_DESTROY,
	NSS_DTLS_MSG_SESSION_STATS,
	NSS_DTLS_MSG_REKEY_ENCAP_CIPHER_UPDATE,
	NSS_DTLS_MSG_REKEY_ENCAP_CIPHER_SWITCH,
	NSS_DTLS_MSG_REKEY_DECAP_CIPHER_UPDATE,
	NSS_DTLS_MSG_REKEY_DECAP_CIPHER_SWITCH,
	NSS_DTLS_MSG_MAX
};

/**
 * nss_dtls_error_response_types
 *	Error types for DTLS responses.
 */
enum nss_dtls_error_response_types {
	NSS_DTLS_ERR_UNKNOWN_MSG = 1,
	NSS_DTLS_ERR_INVALID_APP_IF = 2,
	NSS_DTLS_ERR_INVALID_CPARAM = 3,
	NSS_DTLS_ERR_INVALID_VER = 4,
	NSS_DTLS_ERR_NOMEM = 5,
	NSS_DTLS_ERR_MAX,
};

/**
 * nss_dtls_session_stats
 *	DTLS session statistics.
 */
struct nss_dtls_session_stats {
	struct nss_cmn_node_stats node_stats;	/**< Common node statistics. */
	uint32_t tx_auth_done;		/**< Tx authentication is done. */
	uint32_t rx_auth_done;		/**< Rx authentication is successful. */
	uint32_t tx_cipher_done;	/**< Tx cipher is complete. */
	uint32_t rx_cipher_done;	/**< Rx cipher is complete. */
	uint32_t tx_cbuf_alloc_fail;	/**< Tx crypto buffer allocation failure. */
	uint32_t rx_cbuf_alloc_fail;	/**< Rx crypto buffer allocation failure. */
	uint32_t tx_cenqueue_fail;	/**< Tx enqueue-to-crypto failure. */
	uint32_t rx_cenqueue_fail;	/**< Rx enqueue-to-crypto failure. */
	uint32_t tx_dropped_hroom;
			/**< Tx packets dropped because of insufficent headroom. */
	uint32_t tx_dropped_troom;
			/**< Tx packets dropped because of insufficent tailroom. */
	uint32_t tx_forward_enqueue_fail;
			/**< Tx enqueue failed to forward a node after encapsulation. */
	uint32_t rx_forward_enqueue_fail;
			/**< Rx enqueue failed to receive a node after decapsulation. */
	uint32_t rx_invalid_version;	/**< Rx invalid DTLS version. */
	uint32_t rx_invalid_epoch;	/**< Rx invalid DTLS epoch. */
	uint32_t rx_malformed;		/**< Rx malformed DTLS record. */
	uint32_t rx_cipher_fail;	/**< Rx cipher failure. */
	uint32_t rx_auth_fail;		/**< Rx authentication failure. */
	uint32_t rx_capwap_classify_fail;	/**< Rx CAPWAP classification failure. */
	uint32_t rx_single_rec_dgram;	/**< Rx single record datagrams processed. */
	uint32_t rx_multi_rec_dgram;	/**< Rx multi-record datagrams processed. */
	uint32_t rx_replay_fail;	/**< Rx anti-replay failures. */
	uint32_t rx_replay_duplicate;
			/**< Rx anti-replay failed because of a duplicate record. */
	uint32_t rx_replay_out_of_window;
			/**< Rx anti-replay failed because of an out-of-window record. */
	uint32_t outflow_queue_full;
			/**< Tx packets dropped because the encapsulation queue is full. */
	uint32_t decap_queue_full;
			/**< Rx packets dropped because the decapsulation queue is full. */
	uint32_t pbuf_alloc_fail;
			/**< Packets dropped because of a buffer allocation failure. */
	uint32_t pbuf_copy_fail;
			/**< Packets dropped because of a buffer copy failure. */
	uint16_t epoch;		/**< Current epoch. */
	uint16_t tx_seq_high;	/**< Upper 16 bits of the current sequence number. */
	uint32_t tx_seq_low;	/**< Lower 32 bits of the current sequence number. */
};

/**
 * nss_dtls_session_cipher_update
 *	Information for a cipher update message in a DTLS session.
 */
struct nss_dtls_session_cipher_update {
	uint32_t crypto_idx;	/**< Crypto index for encapsulation. */
	uint32_t hash_len;	/**< Authentication hash length for encapsulation. */
	uint32_t iv_len;	/**< Crypto IV length for encapsulation. */
	uint32_t cipher_algo;	/**< Encapsulation cipher. */
	uint32_t auth_algo;	/**< Encapsulation authentication algorithm. */
	uint16_t epoch;		/**< Epoch indicator. */
	uint16_t reserved;	/**< Reserved for message alignment.*/
};

/**
 * nss_dtls_session_configure
 *	Configuration message for a DTLS session.
 */
struct nss_dtls_session_configure {
	uint32_t ver;			/**< DTLS version. */
	uint32_t flags;			/**< DTLS flags. */
	uint32_t crypto_idx_encap;	/**< Crypto index for encapsulation. */
	uint32_t crypto_idx_decap;	/**< Crypto index for decapsulation. */
	uint32_t iv_len_encap;		/**< Crypto IV length for encapsulation. */
	uint32_t iv_len_decap;		/**< Crypto IV length for decapsulation. */
	uint32_t hash_len_encap;
			/**< Authentication hash length for encapsulation. */
	uint32_t hash_len_decap;
			/**< Authentication hash length for decapsulation. */
	uint32_t cipher_algo_encap;	/**< Cipher algorithm for encapsulation. */
	uint32_t auth_algo_encap;	/**< Authentication algorithm encapsulation. */
	uint32_t cipher_algo_decap;	/**< Cipher algorithm for decapsulation. */
	uint32_t auth_algo_decap;	/**< Authentication algorithm decapsulation. */
	uint32_t nss_app_if;
			/**< Interface of the node that receives decapsulated packets. */
	uint16_t sport;			/**< Source UDP/UDPLite port. */
	uint16_t dport;			/**< Destination UDP/UDPLite port. */
	uint32_t sip[4];		/**< Source IPv4/IPv6 address. */
	uint32_t dip[4];		/**< Destination IPv4/IPv6 address. */
	uint16_t window_size;		/**< Anti-replay window size. */
	uint16_t epoch;			/**< Epoch indicator. */
	uint8_t oip_ttl;		/**< Maximum outer IP time-to-live value. */
	uint8_t reserved1;		/**< Reserved for message alignment. */
	uint16_t reserved2;		/**< Reserved for message alignment. */
};

/**
 * nss_dtls_msg
 *	Data for sending and receiving DTLS messages.
 */
struct nss_dtls_msg {
	struct nss_cmn_msg cm;		/**< Common message header. */

	/**
	 * Payload of a DTLS message.
	 */
	union {
		struct nss_dtls_session_configure cfg;
				/**< Session configuration. */
		struct nss_dtls_session_cipher_update cipher_update;
				/**< Cipher update information. */
		struct nss_dtls_session_stats stats;
				/**< Session statistics. */
	} msg;			/**< Message payload for DTLS session messages exchanged with NSS core. */
};

/**
 * nss_dtls_tx_buf
 *	Sends a DTLS data packet to the NSS.
 *
 * @datatypes
 * sk_buff \n
 * nss_ctx_instance
 *
 * @param[in]     os_buf   Pointer to the OS data buffer.
 * @param[in]     if_num   NSS interface number.
 * @param[in]     nss_ctx  Pointer to the NSS core context.
 *
 * @return
 * Status of Tx buffer forwarded to NSS for DTLS operation.
 */
nss_tx_status_t nss_dtls_tx_buf(struct sk_buff *os_buf, uint32_t if_num,
				struct nss_ctx_instance *nss_ctx);

/**
 * nss_dtls_tx_msg
 *	Sends DTLS messages.
 *
 * @param[in]     nss_ctx  Pointer to the NSS core context.
 * @param[in,out] msg      Pointer to the message data.
 *
 * @return
 * Status of the Tx operation.
 */
extern nss_tx_status_t nss_dtls_tx_msg(struct nss_ctx_instance *nss_ctx,
				       struct nss_dtls_msg *msg);

/**
 * nss_dtls_tx_msg_sync
 *	Sends DTLS messages synchronously.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_dtls_msg
 *
 * @param[in]     nss_ctx  Pointer to the NSS context.
 * @param[in,out] msg      Pointer to the message data.
 *
 * @return
 * Status of the Tx operation.
 */
extern nss_tx_status_t nss_dtls_tx_msg_sync(struct nss_ctx_instance *nss_ctx,
					    struct nss_dtls_msg *msg);

/**
 * Callback function for receiving DTLS messages.
 *
 * @datatypes
 * nss_dtls_msg
 *
 * @param[in] app_data  Pointer to the application context of the message.
 * @param[in] msg       Pointer to the message data.
 */
typedef void (*nss_dtls_msg_callback_t)(void *app_data,
					struct nss_dtls_msg *msg);

/**
 * Callback function for receiving DTLS session data.
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
typedef void (*nss_dtls_data_callback_t)(struct net_device *netdev,
					struct sk_buff *skb,
					struct napi_struct *napi);

/**
 * nss_dtls_register_if
 *	Registers a DTLS session interface with the NSS for sending and receiving
 *	messages.
 *
 * @datatypes
 * nss_dtls_data_callback_t \n
 * nss_dtls_msg_callback_t
 *
 * @param[in] if_num        NSS interface number.
 * @param[in] cb            Callback function for the message.
 * @param[in] msg_callback  Callback for DTLS tunnel message.
 * @param[in] netdev        Pointer to the associated network device.
 * @param[in] features      Data socket buffer types supported by this interface.
 * @param[in] app_ctx       Pointer to the application context.
 *
 * @return
 * Pointer to the NSS core context.
 */
extern struct nss_ctx_instance *nss_dtls_register_if(uint32_t if_num,
							nss_dtls_data_callback_t cb,
							nss_dtls_msg_callback_t msg_callback,
							struct net_device *netdev,
							uint32_t features,
							void *app_ctx);

/**
 * nss_dtls_unregister_if
 *	Deregisters a DTLS session interface from the NSS.
 *
 * @param[in] if_num  NSS interface number.
 *
 * @return
 * None.
 *
 * @dependencies
 * The DTLS session interface must have been previously registered.
 */
extern void nss_dtls_unregister_if(uint32_t if_num);

/**
 * nss_dtls_msg_init
 *	Initializes a DTLS message.
 *
 * @datatypes
 * nss_dtls_msg
 *
 * @param[in,out] ncm       Pointer to the message.
 * @param[in]     if_num    NSS interface number.
 * @param[in]     type      Type of message.
 * @param[in]     len       Size of the payload.
 * @param[in]     cb        Pointer to the message callback.
 * @param[in]     app_data  Pointer to the application context.
 *
 * @return
 * None.
 */
extern void nss_dtls_msg_init(struct nss_dtls_msg *ncm, uint16_t if_num,
				uint32_t type, uint32_t len, void *cb,
				void *app_data);

/**
 * nss_dtls_get_context
 *	Gets the NSS core context for the DTLS session.
 *
 * @return
 * Pointer to the NSS core context.
 */
extern struct nss_ctx_instance *nss_dtls_get_context(void);

/**
 * nss_dtls_get_ifnum_with_coreid
 *	Gets the DTLS interface number with a core ID.
 *
 * @param[in] if_num  NSS interface number.
 *
 * @return
 * Interface number with the core ID.
 */
extern int32_t nss_dtls_get_ifnum_with_coreid(int32_t if_num);

/**
 * @}
 */

#endif /* _NSS_DTLS_H_. */

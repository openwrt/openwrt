/*
 **************************************************************************
 * Copyright (c) 2014-2017, The Linux Foundation. All rights reserved.
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
 * @file nss_crypto.h
 *	NSS Crypto interface definitions.
 */

#ifndef __NSS_CRYPTO_H
#define __NSS_CRYPTO_H

/**
 * @addtogroup nss_crypto_subsystem
 * @{
 */

#define NSS_CRYPTO_MAX_IDXS 64		/**< Maximum number of supported sessions. */
#define NSS_CRYPTO_MAX_ENGINES 4	/**< Maximum number of engines available. */
#define NSS_CRYPTO_BAM_PP 2		/**< Bus Access Manager pipe pairs. */

/**
 * nss_crypto_hash
 *	Hash sizes supported by the hardware.
 */
enum nss_crypto_hash {
	NSS_CRYPTO_HASH_SHA96 = 12,
	NSS_CRYPTO_HASH_SHA128 = 16,
	NSS_CRYPTO_HASH_SHA160 = 20,
	NSS_CRYPTO_HASH_SHA256 = 32
};

/**
 * nss_crypto_cipher
 *	Cipher algorithms.
 */
enum nss_crypto_cipher {
	NSS_CRYPTO_CIPHER_NONE = 0,
	NSS_CRYPTO_CIPHER_AES_CBC,	/**< AES, and CBC for 128-bit and 256-bit key sizes. */
	NSS_CRYPTO_CIPHER_DES,		/**< DES, and CBC for 64-bit key size. */
	NSS_CRYPTO_CIPHER_NULL,		/**< NULL and CBC. */
	NSS_CRYPTO_CIPHER_AES_CTR,	/**< AES, and CTR for 128-bit and 256-bit key sizes. */
	NSS_CRYPTO_CIPHER_MAX
};

/**
 * nss_crypto_auth
 *	Authentication algorithms.
 */
enum nss_crypto_auth {
	NSS_CRYPTO_AUTH_NONE = 0,
	NSS_CRYPTO_AUTH_SHA1_HMAC,
	NSS_CRYPTO_AUTH_SHA256_HMAC,
	NSS_CRYPTO_AUTH_NULL,
	NSS_CRYPTO_AUTH_MAX
};

/**
 * nss_crypto_msg_type
 *	Synchronization types.
 */
enum nss_crypto_msg_type {
	NSS_CRYPTO_MSG_TYPE_NONE = 0,
	NSS_CRYPTO_MSG_TYPE_OPEN_ENG = 1,
	NSS_CRYPTO_MSG_TYPE_CLOSE_ENG = 2,
	NSS_CRYPTO_MSG_TYPE_UPDATE_SESSION = 3,
	NSS_CRYPTO_MSG_TYPE_STATS = 4,
	NSS_CRYPTO_MSG_TYPE_MAX
};

/**
 * nss_crypto_msg_error
 *	Response types.
 */
enum nss_crypto_msg_error {
	NSS_CRYPTO_MSG_ERROR_NONE = 0,
	NSS_CRYPTO_MSG_ERROR_INVAL_ENG = 1,
	NSS_CRYPTO_MSG_ERROR_UNSUPP_OP = 2,
	NSS_CRYPTO_MSG_ERROR_INVAL_OP = 3,
	NSS_CRYPTO_MSG_ERROR_INVAL_IDX_RANGE = 4,
	NSS_CRYPTO_MSG_ERROR_IDX_ALLOC_FAIL = 5,
	NSS_CRYPTO_MSG_ERROR_MAX
};

/**
 * nss_crypto_session_state
 *	Session states.
 */
enum nss_crypto_session_state {
	NSS_CRYPTO_SESSION_STATE_NONE = 0,
	NSS_CRYPTO_SESSION_STATE_ACTIVE = 1,
	NSS_CRYPTO_SESSION_STATE_FREE = 2
};

/**
 * nss_crypto_buf_origin
 *	Origins of the crypto session.
 */
enum nss_crypto_buf_origin {
	NSS_CRYPTO_BUF_ORIGIN_HOST = 0x001,
	NSS_CRYPTO_BUF_ORIGIN_NSS = 0x0002,
};

/**
 * nss_crypto_idx
 *	Crypto session index information.
 */
struct nss_crypto_idx {
	uint16_t pp_num;		/**< Pipe pair index. */
	uint16_t cmd_len;		/**< Command block length to program. */
	uint32_t cblk_paddr;		/**< Physical address of the command block. */
};

/**
 * nss_crypto_config_eng
 *	Engine configuration information for opening the engine from the host.
 *
 * This structure is called to initialize the crypto NSS engine-specific data
 * structures. Ideally, the host can send a single probe for all engines, but
 * the current implementation relies on probes per engine.
 */
struct nss_crypto_config_eng {
	uint32_t eng_id;		/**< Engine number to open. */
	uint32_t bam_pbase;		/**< BAM base address (physical). */
	uint32_t crypto_pbase;		/**< Crypto base address (physical). */
	uint32_t desc_paddr[NSS_CRYPTO_BAM_PP];
					/**< Pipe description address (physical). */
	struct nss_crypto_idx idx[NSS_CRYPTO_MAX_IDXS];
					/**< Allocated session indices. */
};

/**
 * nss_crypto_config_session
 *	Session-related state configuration.
 */
struct nss_crypto_config_session {
	uint32_t idx;		/**< Session index on which the state is reset. */
	uint32_t state;		/**< Index state of the session. */
	uint32_t iv_len;	/**< Length of the initialization vector. */
};

/**
 * nss_crypto_stats
 *	Crypto statistics.
 */
struct nss_crypto_stats {
	uint32_t queued;	/**< Number of frames waiting to be processed. */
	uint32_t completed;	/**< Number of frames processed. */
	uint32_t dropped;	/**< Number of frames dropped or not processed. */
};

/**
 * nss_crypto_sync_stats
 *	Statistics synchronized to the host.
 */
struct nss_crypto_sync_stats {
	struct nss_crypto_stats eng_stats[NSS_CRYPTO_MAX_ENGINES];
			/**< Tx or Rx statistics captured per crypto engine. */
	struct nss_crypto_stats idx_stats[NSS_CRYPTO_MAX_IDXS];
			/**< Tx or Rx statistics captured per session. */
	struct nss_crypto_stats total;
			/**< Total statistics captured in and out of the engine. */
};

/**
 * nss_crypto_msg
 *	Data for sending and receiving crypto messages.
 */
struct nss_crypto_msg {
	struct nss_cmn_msg cm;		/**< Common message header. */

	/**
	 * Payload of a crypto message.
	 */
	union {
		struct nss_crypto_config_eng eng;
				/**< Opens an engine. */
		struct nss_crypto_config_session session;
				/**< Resets the statistics. */
		struct nss_crypto_sync_stats stats;
				/**< Synchronized statistics for crypto. */
	} msg;			/**< Message payload. */
};

#ifdef __KERNEL__  /* only kernel will use. */

/**
 * Message notification callback.
 *
 * @datatypes
 * nss_crypto_msg
 *
 * @param[in] app_data  Pointer to the application context of the message.
 * @param[in] msg       Pointer to the message data.
 */
typedef void (*nss_crypto_msg_callback_t)(void *app_data, struct nss_crypto_msg *msg);

/**
 * Data callback.
 *
 * @datatypes
 * net_device \n
 * sk_buff \n
 * napi_struct
 *
 * @param[in] netdev  Pointer to the network device.
 * @param[in] skb     Pointer to the data socket buffer.
 * @param[in] napi    Pointer to the NAPI structure.
 */
typedef void (*nss_crypto_buf_callback_t)(struct net_device *netdev, struct sk_buff *skb, struct napi_struct *napi);

/**
 * Power management event callback.
 *
 * @param[in] app_data    Pointer to the application context of the message.
 * @param[in] turbo       Turbo mode event.
 * @param[in] auto_scale  Specifies the auto scaling of the NSS clock frequency.
 *
 * @return
 * TRUE if crypto is scaled to turbo.
 */
typedef bool (*nss_crypto_pm_event_callback_t)(void *app_data, bool turbo, bool auto_scale);

/**
 * nss_crypto_tx_msg
 *	Sends a crypto message.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_crypto_msg
 *
 * @param[in] nss_ctx  Pointer to the NSS context of the HLOS driver.
 * @param[in] msg      Pointer to the message data.
 *
 * @return
 * None.
 */
extern nss_tx_status_t nss_crypto_tx_msg(struct nss_ctx_instance *nss_ctx, struct nss_crypto_msg *msg);

/**
 * nss_crypto_tx_buf
 *	Sends a crypto data packet.
 *
 * @datatypes
 * nss_ctx_instance \n
 * sk_buff
 *
 * @param[in] nss_ctx  Pointer to the NSS context of the HLOS driver
 * @param[in] if_num   NSS interface number.
 * @param[in] skb      Pointer to the data socket buffer.
 *
 * @return
 * None.
 */
extern nss_tx_status_t nss_crypto_tx_buf(struct nss_ctx_instance *nss_ctx, uint32_t if_num, struct sk_buff *skb);

/**
 * nss_crypto_notify_register
 *	Registers an event callback handler with the HLOS driver.
 *
 * @datatypes
 * nss_crypto_msg_callback_t
 *
 * @param[in] cb        Callback function for the message.
 * @param[in] app_data  Pointer to the application context of the message.
 *
 * @return
 * None.
 */
extern struct nss_ctx_instance *nss_crypto_notify_register(nss_crypto_msg_callback_t cb, void *app_data);

/**
 * nss_crypto_data_register
 *	Registers a data callback handler with the HLOS driver.
 *
 * @datatypes
 * nss_crypto_buf_callback_t \n
 * net_device
 *
 * @param[in] if_num    NSS interface number.
 * @param[in] cb        Callback function for the data.
 * @param[in] netdev    Pointer to the network device.
 * @param[in] features  Data socket buffer types supported by this interface.
 *
 * @return
 * None.
 */
extern struct nss_ctx_instance *nss_crypto_data_register(uint32_t if_num, nss_crypto_buf_callback_t cb,
		struct net_device *netdev, uint32_t features);

/**
 * nss_crypto_pm_notify_register
 *	Registers a power management event callback handler with the HLOS driver.
 *
 * @datatypes
 * nss_crypto_pm_event_callback_t
 *
 * @param[in] cb        Callback function for the message.
 * @param[in] app_data  Pointer to the application context of the message.
 *
 * @return
 * None.
 */
extern void nss_crypto_pm_notify_register(nss_crypto_pm_event_callback_t cb, void *app_data);

/**
 * nss_crypto_notify_unregister
 *	Deregisters an event callback handler notifier from the HLOS driver.
 *
 * @datatypes
 * nss_ctx_instance
 *
 * @param[in,out] ctx  Pointer to the context of the HLOS driver.
 *
 * @return
 * None.
 *
 * @dependencies
 * The event callback handler must have been previously registered.
 */
extern void nss_crypto_notify_unregister(struct nss_ctx_instance *ctx);

/**
 * nss_crypto_data_unregister
 *	Deregisters a data callback handler from the HLOS driver.
 *
 * @datatypes
 * nss_ctx_instance
 *
 * @param[in,out] ctx     Pointer to the context of the HLOS driver.
 * @param[in]     if_num  NSS interface number.
 *
 * @return
 * None.
 *
 * @dependencies
 * The callback handler must have been previously registered.
 */
extern void nss_crypto_data_unregister(struct nss_ctx_instance *ctx, uint32_t if_num);

/**
 * nss_crypto_pm_notify_unregister
 *	Deregisters a power management event callback handler from the HLOS driver.
 *
 * @return
 * None.
 *
 * @dependencies
 * The callback handler must have been previously registered.
 */
extern void nss_crypto_pm_notify_unregister(void);

/**
 * nss_crypto_msg_init
 *	Initializes a crypto-specific message.
 *
 * @datatypes
 * nss_crypto_msg \n
 * nss_crypto_msg_callback_t
 *
 * @param[in,out] ncm       Pointer to the message.
 * @param[in]     if_num    NSS interface number.
 * @param[in]     type      Type of message.
 * @param[in]     len       Size of the payload.
 * @param[in]     cb        Callback function for the message.
 * @param[in]     app_data  Pointer to the application context of the message.
 *
 * @return
 * None.
 */
extern void nss_crypto_msg_init(struct nss_crypto_msg *ncm, uint16_t if_num, uint32_t type, uint32_t len,
								nss_crypto_msg_callback_t cb, void *app_data);

#endif /*__KERNEL__ */

/**
 * @}
 */

#endif /* __NSS_CRYPTO_H */

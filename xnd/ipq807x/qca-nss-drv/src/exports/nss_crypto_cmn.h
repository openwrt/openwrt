/*
 **************************************************************************
 * Copyright (c) 2017-2019, The Linux Foundation. All rights reserved.
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
 * @file nss_crypto_cmn.h
 *	NSS Crypto common interface definitions.
 */
#ifndef __NSS_CRYPTO_CMN_H
#define __NSS_CRYPTO_CMN_H

/**
 * @addtogroup nss_crypto_subsystem
 * @{
 */

/*
 * Context message related array sizes
 */
#define NSS_CRYPTO_CMN_CTX_SPARE 4 /**< Context spare words size. */
#define NSS_CRYPTO_CMN_VER_WORDS 4 /**< Firmware version words size.*/
#define NSS_CRYPTO_CIPHER_KEYLEN_MAX 32 /**< Maximum cipher keysize. */
#define NSS_CRYPTO_AUTH_KEYLEN_MAX 128 /**< Maximum authorization keysize. */
#define NSS_CRYPTO_NONCE_SIZE_MAX 4 /**< Maximum authorization keysize. */

/**
 * nss_crypto_cmn_algo
 *	List of crypto algorithms supported.
 */
enum nss_crypto_cmn_algo {
	NSS_CRYPTO_CMN_ALGO_NULL,			/**< NULL transform. */
	NSS_CRYPTO_CMN_ALGO_3DES_CBC,			/**< Asynchronous block cipher. */
	NSS_CRYPTO_CMN_ALGO_AES128_CBC,			/**< Asynchronous block cipher. */
	NSS_CRYPTO_CMN_ALGO_AES192_CBC,			/**< Asynchronous block cipher. */
	NSS_CRYPTO_CMN_ALGO_AES256_CBC,			/**< Asynchronous block cipher. */
	NSS_CRYPTO_CMN_ALGO_AES128_CTR,			/**< Asynchronous block cipher. */
	NSS_CRYPTO_CMN_ALGO_AES192_CTR,			/**< Asynchronous block cipher. */
	NSS_CRYPTO_CMN_ALGO_AES256_CTR,			/**< Asynchronous block cipher. */
	NSS_CRYPTO_CMN_ALGO_AES128_ECB,			/**< Asynchronous block cipher. */
	NSS_CRYPTO_CMN_ALGO_AES192_ECB,			/**< Asynchronous block cipher. */
	NSS_CRYPTO_CMN_ALGO_AES256_ECB,			/**< Asynchronous block cipher. */
	NSS_CRYPTO_CMN_ALGO_AES128_GCM,			/**< Asynchronous block cipher. */
	NSS_CRYPTO_CMN_ALGO_AES192_GCM,			/**< Asynchronous block cipher. */
	NSS_CRYPTO_CMN_ALGO_AES256_GCM,			/**< Asynchronous block cipher. */
	NSS_CRYPTO_CMN_ALGO_MD5_HASH,			/**< Asynchronous digest. */
	NSS_CRYPTO_CMN_ALGO_SHA160_HASH,		/**< Asynchronous digest. */
	NSS_CRYPTO_CMN_ALGO_SHA224_HASH,		/**< Asynchronous digest. */
	NSS_CRYPTO_CMN_ALGO_SHA256_HASH,		/**< Asynchronous digest. */
	NSS_CRYPTO_CMN_ALGO_SHA384_HASH,		/**< Asynchronous digest. */
	NSS_CRYPTO_CMN_ALGO_SHA512_HASH,		/**< Asynchronous digest. */
	NSS_CRYPTO_CMN_ALGO_MD5_HMAC,			/**< Asynchronous digest. */
	NSS_CRYPTO_CMN_ALGO_SHA160_HMAC,		/**< Asynchronous digest. */
	NSS_CRYPTO_CMN_ALGO_SHA224_HMAC,		/**< Asynchronous digest. */
	NSS_CRYPTO_CMN_ALGO_SHA256_HMAC,		/**< Asynchronous digest. */
	NSS_CRYPTO_CMN_ALGO_SHA384_HMAC,		/**< Asynchronous digest. */
	NSS_CRYPTO_CMN_ALGO_SHA512_HMAC,		/**< Asynchronous digest. */
	NSS_CRYPTO_CMN_ALGO_AES128_GMAC,		/**< Asynchronous digest. */
	NSS_CRYPTO_CMN_ALGO_AES192_GMAC,		/**< Asynchronous digest. */
	NSS_CRYPTO_CMN_ALGO_AES256_GMAC,		/**< Asynchronous digest. */
	NSS_CRYPTO_CMN_ALGO_AES128_GCM_GMAC,		/**< AEAD transform. */
	NSS_CRYPTO_CMN_ALGO_AES128_CBC_MD5_HMAC,	/**< AEAD transform. */
	NSS_CRYPTO_CMN_ALGO_AES128_CBC_SHA160_HMAC,	/**< AEAD transform. */
	NSS_CRYPTO_CMN_ALGO_AES128_CBC_SHA256_HMAC,	/**< AEAD transform. */
	NSS_CRYPTO_CMN_ALGO_AES128_CBC_SHA384_HMAC,	/**< AEAD transform. */
	NSS_CRYPTO_CMN_ALGO_AES128_CBC_SHA512_HMAC,	/**< AEAD transform. */
	NSS_CRYPTO_CMN_ALGO_AES192_GCM_GMAC,		/**< AEAD transform. */
	NSS_CRYPTO_CMN_ALGO_AES192_CBC_MD5_HMAC,	/**< AEAD transform. */
	NSS_CRYPTO_CMN_ALGO_AES192_CBC_SHA160_HMAC,	/**< AEAD transform. */
	NSS_CRYPTO_CMN_ALGO_AES192_CBC_SHA256_HMAC,	/**< AEAD transform. */
	NSS_CRYPTO_CMN_ALGO_AES192_CBC_SHA384_HMAC,	/**< AEAD transform. */
	NSS_CRYPTO_CMN_ALGO_AES192_CBC_SHA512_HMAC,	/**< AEAD transform. */
	NSS_CRYPTO_CMN_ALGO_AES256_GCM_GMAC,		/**< AEAD transform. */
	NSS_CRYPTO_CMN_ALGO_AES256_CBC_MD5_HMAC,	/**< AEAD transform. */
	NSS_CRYPTO_CMN_ALGO_AES256_CBC_SHA160_HMAC,	/**< AEAD transform. */
	NSS_CRYPTO_CMN_ALGO_AES256_CBC_SHA256_HMAC,	/**< AEAD transform. */
	NSS_CRYPTO_CMN_ALGO_AES256_CBC_SHA384_HMAC,	/**< AEAD transform. */
	NSS_CRYPTO_CMN_ALGO_AES256_CBC_SHA512_HMAC,	/**< AEAD transform. */
	NSS_CRYPTO_CMN_ALGO_AES128_CTR_MD5_HMAC,	/**< AEAD transform. */
	NSS_CRYPTO_CMN_ALGO_AES128_CTR_SHA160_HMAC,	/**< AEAD transform. */
	NSS_CRYPTO_CMN_ALGO_AES128_CTR_SHA256_HMAC,	/**< AEAD transform. */
	NSS_CRYPTO_CMN_ALGO_AES128_CTR_SHA384_HMAC,	/**< AEAD transform. */
	NSS_CRYPTO_CMN_ALGO_AES128_CTR_SHA512_HMAC,	/**< AEAD transform. */
	NSS_CRYPTO_CMN_ALGO_AES192_CTR_MD5_HMAC,	/**< AEAD transform. */
	NSS_CRYPTO_CMN_ALGO_AES192_CTR_SHA160_HMAC,	/**< AEAD transform. */
	NSS_CRYPTO_CMN_ALGO_AES192_CTR_SHA256_HMAC,	/**< AEAD transform. */
	NSS_CRYPTO_CMN_ALGO_AES192_CTR_SHA384_HMAC,	/**< AEAD transform. */
	NSS_CRYPTO_CMN_ALGO_AES192_CTR_SHA512_HMAC,	/**< AEAD transform. */
	NSS_CRYPTO_CMN_ALGO_AES256_CTR_MD5_HMAC,	/**< AEAD transform. */
	NSS_CRYPTO_CMN_ALGO_AES256_CTR_SHA160_HMAC,	/**< AEAD transform. */
	NSS_CRYPTO_CMN_ALGO_AES256_CTR_SHA256_HMAC,	/**< AEAD transform. */
	NSS_CRYPTO_CMN_ALGO_AES256_CTR_SHA384_HMAC,	/**< AEAD transform. */
	NSS_CRYPTO_CMN_ALGO_AES256_CTR_SHA512_HMAC,	/**< AEAD transform. */
	NSS_CRYPTO_CMN_ALGO_3DES_CBC_MD5_HMAC,		/**< AEAD transform. */
	NSS_CRYPTO_CMN_ALGO_3DES_CBC_SHA160_HMAC,	/**< AEAD transform. */
	NSS_CRYPTO_CMN_ALGO_3DES_CBC_SHA256_HMAC,	/**< AEAD transform. */
	NSS_CRYPTO_CMN_ALGO_3DES_CBC_SHA384_HMAC,	/**< AEAD transform. */
	NSS_CRYPTO_CMN_ALGO_3DES_CBC_SHA512_HMAC,	/**< AEAD transform. */
	NSS_CRYPTO_CMN_ALGO_MAX
};

/**
 * nss_crypto_cmn_resp_error
 *	Response errors from crypto hardware
 */
enum nss_crypto_cmn_resp_error {
	NSS_CRYPTO_CMN_RESP_ERROR_NONE = 0,			/**< No error. */
	NSS_CRYPTO_CMN_RESP_ERROR_HDR_VERSION,		/**< Header version mismatch. */
	NSS_CRYPTO_CMN_RESP_ERROR_CTX_RANGE,		/**< Crypto index out-of-range. */
	NSS_CRYPTO_CMN_RESP_ERROR_CTX_NOUSE,		/**< Crypto index is freed. */
	NSS_CRYPTO_CMN_RESP_ERROR_DATA_EMPTY,		/**< Crypto data is empty. */
	NSS_CRYPTO_CMN_RESP_ERROR_DATA_LEN,			/**< Crypto data length. */
	NSS_CRYPTO_CMN_RESP_ERROR_DATA_TIMEOUT,		/**< Data timeout from hardware. */
	NSS_CRYPTO_CMN_RESP_ERROR_CIPHER_ALGO,		/**< Cipher algorithm is not supported. */
	NSS_CRYPTO_CMN_RESP_ERROR_CIPHER_MODE,		/**< Cipher mode is not supported. */
	NSS_CRYPTO_CMN_RESP_ERROR_CIPHER_BLK_LEN,	/**< Cipher block length is not aligned. */
	NSS_CRYPTO_CMN_RESP_ERROR_HASH_CHECK,		/**< Hash check failed. */
	NSS_CRYPTO_CMN_RESP_ERROR_HASH_NOSPACE,		/**< No space to write hash. */
	NSS_CRYPTO_CMN_RESP_ERROR_HW_STATUS,		/**< More errors in hardware status. */
	NSS_CRYPTO_CMN_RESP_ERROR_MAX
};

/**
 * nss_crypto_cmn_msg_type
 *	Message types supported.
 */
enum nss_crypto_cmn_msg_type {
	NSS_CRYPTO_CMN_MSG_TYPE_NONE = 0,		/**< Invalid message. */
	NSS_CRYPTO_CMN_MSG_TYPE_SETUP_NODE,		/**< Initialize node. */
	NSS_CRYPTO_CMN_MSG_TYPE_SETUP_ENG,		/**< Initialize engine. */
	NSS_CRYPTO_CMN_MSG_TYPE_SETUP_DMA,		/**< Initialize DMA pair. */
	NSS_CRYPTO_CMN_MSG_TYPE_SETUP_CTX,		/**< Update context information. */
	NSS_CRYPTO_CMN_MSG_TYPE_CLEAR_CTX,		/**< Clear context information. */
	NSS_CRYPTO_CMN_MSG_TYPE_VERIFY_CTX,		/**< Verify if context is active. */
	NSS_CRYPTO_CMN_MSG_TYPE_SYNC_NODE_STATS,	/**< Synchronous node statistics. */
	NSS_CRYPTO_CMN_MSG_TYPE_SYNC_ENG_STATS,		/**< Synchronous engine statistics. */
	NSS_CRYPTO_CMN_MSG_TYPE_SYNC_CTX_STATS,		/**< Synchronous context statistics. */
	NSS_CRYPTO_CMN_MSG_TYPE_MAX
};

/**
 * nss_crypto_cmn_msg_error
 *	Message error types supported.
 */
enum nss_crypto_cmn_msg_error {
	NSS_CRYPTO_CMN_MSG_ERROR_NONE = 0,
	NSS_CRYPTO_CMN_MSG_ERROR_HDR_VERSION_NOSUPP,	/**< Common header version not supported. */
	NSS_CRYPTO_CMN_MSG_ERROR_NODE_CTX_RANGE,	/**< Context index out-of-range for node. */
	NSS_CRYPTO_CMN_MSG_ERROR_DMA_MASK,		/**< DMA mask is out-of-range. */
	NSS_CRYPTO_CMN_MSG_ERROR_DMA_POW2,		/**< DMA count is not a power-of-two. */
	NSS_CRYPTO_CMN_MSG_ERROR_DMA_MAX_TOKEN,		/**< DMA count exceeds token count. */
	NSS_CRYPTO_CMN_MSG_ERROR_DMA_TOKEN_ALLOC,	/**< Failed to allocate token. */
	NSS_CRYPTO_CMN_MSG_ERROR_CTX_RANGE,		/**< Context index out-of-range. */
	NSS_CRYPTO_CMN_MSG_ERROR_CTX_INUSE,		/**< Context has references. */
	NSS_CRYPTO_CMN_MSG_ERROR_CTX_WORDS,		/**< Context size is bad. */
	NSS_CRYPTO_CMN_MSG_ERROR_CTX_ALGO,		/**< Context algorithm is bad. */
	NSS_CRYPTO_CMN_MSG_ERROR_CTX_ALLOC,		/**< Context alloc failed. */
	NSS_CRYPTO_CMN_MSG_ERROR_CTX_NOUSE,		/**< Context has no references. */
	NSS_CRYPTO_CMN_MSG_ERROR_CTX_FLAGS,		/**< Invalid context flags. */
	NSS_CRYPTO_CMN_MSG_ERROR_MAX
};

/**
 * nss_crypto_cmn_ctx_flags
 *	Context message specific flags.
 */
enum nss_crypto_cmn_ctx_flags {
	NSS_CRYPTO_CMN_CTX_FLAGS_NONE = 0,		/**< Invalid flags. */
	NSS_CRYPTO_CMN_CTX_FLAGS_SEC_OFFSET = 0x01,	/**< Secure offset is valid. */
	NSS_CRYPTO_CMN_CTX_FLAGS_SPARE0 = 0x02,		/**< Spare word-0 valid. */
	NSS_CRYPTO_CMN_CTX_FLAGS_SPARE1 = 0x04,		/**< Spare word-1 valid. */
	NSS_CRYPTO_CMN_CTX_FLAGS_SPARE2 = 0x08,		/**< Spare word-2 valid. */
	NSS_CRYPTO_CMN_CTX_FLAGS_SPARE3 = 0x10,		/**< Spare word-3 valid. */
	NSS_CRYPTO_CMN_CTX_FLAGS_MAX
};

/**
 * nss_crypto_cmn_node
 *	Node message for setting up the crypto node.
 *
 * Note: Upon boot this is the first message sent by Host to NSS crypto.
 *	- It notifies the maximum number of crypto context.
 *	- It notifies the maximum number of DMA rings.
 *	- It returns the maximum size of crypto context record.
 */
struct nss_crypto_cmn_node {
	uint32_t max_dma_rings;		/**< Maximum DMA rings supported. */
	uint32_t max_ctx;		/**< Maximum contexts. */
	uint32_t max_ctx_size;		/**< Maximum context size. */
};

/**
 * nss_crypto_cmn_engine
 *	Engine message for setting up the instance of crypto engine.
 *
 * Note: This is sent after 'node' message for each engine to
 *	- Get valid DMA pairs supported by firmware.
 *	- Get maximum request/token count available in firmware.
 */
struct nss_crypto_cmn_engine {
	uint32_t fw_ver[NSS_CRYPTO_CMN_VER_WORDS];	/**< Firmware version. */
	uint32_t dma_mask;				/**< Max DMA rings. */
	uint32_t req_count;				/**< Token count. */
};

/**
 * nss_crypto_cmn_dma
 *	DMA message for setting up each DMA pair per engine.
 */
struct nss_crypto_cmn_dma {
	uint16_t pair_id;		/**< DMA pair ID. */
};

/**
 * nss_crypto_cmn_ctx
 *	Context message for setting up a crypto context in firmware.
 */
struct nss_crypto_cmn_ctx {
	uint32_t spare[NSS_CRYPTO_CMN_CTX_SPARE];	/**< Context spare words. */
	uint16_t index;					/**< Crypto index. */
	uint16_t sec_offset;				/**< Secure offset for copying keys. */

	uint8_t cipher_key[NSS_CRYPTO_CIPHER_KEYLEN_MAX];	/**< Array containing cipher keys. */
	uint8_t auth_key[NSS_CRYPTO_AUTH_KEYLEN_MAX];	/**< Array containing authorization keys. */
	uint8_t nonce[NSS_CRYPTO_NONCE_SIZE_MAX];		/**< Nonce value. */

	uint16_t auth_keylen;				/**< Authorization key length. */
	uint8_t res[2];					/**< Reserved. */

	enum nss_crypto_cmn_algo algo;			/**< Crypto algorithm. */
	enum nss_crypto_cmn_ctx_flags flags;		/**< Context specific flags. */
};

/**
 * nss_crypto_cmn_stats
 *	Statistics message applicable for node/engine/context.
 */
struct nss_crypto_cmn_stats {
	struct nss_cmn_node_stats nstats;	/**< Common node statistics. */
	uint32_t fail_version;			/**< Version mismatch failures. */
	uint32_t fail_ctx;			/**< Context related failures. */
	uint32_t fail_dma;			/**< DMA descriptor full. */
};

/**
 * nss_crypto_cmn_msg
 *	Crypto common configuration message.
 */
struct nss_crypto_cmn_msg {
	struct nss_cmn_msg cm;				/**< Common header. */
	uint32_t seq_num;				/**< Sequence number for messages. */
	uint32_t uid;					/**< Unique ID to identify engine and context. */

	union {
		struct nss_crypto_cmn_node node;	/**< Node message. */
		struct nss_crypto_cmn_engine eng;	/**< Engine message. */
		struct nss_crypto_cmn_dma dma;		/**< DMA message. */
		struct nss_crypto_cmn_ctx ctx;		/**< Context message. */
		struct nss_crypto_cmn_stats stats;	/**< Statistics message. */
	} msg;
};

#ifdef __KERNEL__  /* only kernel will use */

/**
 * Callback function for receiving crypto transformation upon completion.
 *
 * @datatypes
 * net_device \n
 * sk_buff \n
 * napi_struct
 *
 * @param[in] netdev  Networking device registered for callback.
 * @param[in] skb     Packet buffer.
 * @param[in] napi    NAPI pointer for Linux NAPI handling.
 *
 * @return
 * None.
 */
typedef void (*nss_crypto_cmn_buf_callback_t)(struct net_device *netdev, struct sk_buff *skb,
						struct napi_struct *napi);

/**
 * Callback function for receiving crypto_cmn messages.
 *
 * @datatypes
 * nss_crypto_cmn_msg
 *
 * @param[in] app_data  Context of the callback user.
 * @param[in] msg       Crypto common message.
 *
 * @return
 * None.
 */
typedef void (*nss_crypto_cmn_msg_callback_t)(void *app_data, struct nss_crypto_cmn_msg *msg);

/**
 * nss_crypto_cmn_tx_buf
 *	Send crypto payload to firmware for transformation.
 *
 * @datatypes
 * nss_ctx_instance \n
 * sk_buff
 *
 * @param[in] nss_ctx  NSS context per NSS core.
 * @param[in] if_num   Crypto interface to send the buffer.
 * @param[in] skb      Crypto payload.
 *
 * @return
 * Status of the TX operation.
 */
extern nss_tx_status_t nss_crypto_cmn_tx_buf(struct nss_ctx_instance *nss_ctx, uint32_t if_num, struct sk_buff *skb);

/**
 * nss_crypto_cmn_tx_msg
 *	Send crypto message to firmware for configuration.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_crypto_cmn_msg
 *
 * @param[in] nss_ctx]  NSS context per NSS core.
 * @param[in] msg       Control message.
 *
 * @return
 * Status of the TX operation.
 */
extern nss_tx_status_t nss_crypto_cmn_tx_msg(struct nss_ctx_instance *nss_ctx, struct nss_crypto_cmn_msg *msg);

/**
 * nss_crypto_cmn_tx_msg
 *	Send crypto message to firmware for configuration synchronously.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_crypto_cmn_msg
 *
 * @param[in]     nss_ctx  NSS context per NSS core.
 * @param[in,out] msg      Crypto message, response data is copied.
 *
 * @return
 * Status of the TX operation.
 *
 * @note
 * Response data for the  message is copied into the 'msg'.
 * The caller should read the content of the 'msg' to find out errors.
 * The caller needs to invoke this from a non-atomic context.
 */
extern nss_tx_status_t nss_crypto_cmn_tx_msg_sync(struct nss_ctx_instance *nss_ctx, struct nss_crypto_cmn_msg *msg);

/**
 * nss_crypto_cmn_notify_register
 *	Register a event callback handler with NSS driver
 *
 * @datatypes
 * nss_crypto_cmn_msg_callback_t
 *
 * @param[in] cb        Event callback function.
 * @param[in] app_data  Context of the callback user.
 *
 * @return
 * Pointer to the NSS core context.
 */
extern struct nss_ctx_instance *nss_crypto_cmn_notify_register(nss_crypto_cmn_msg_callback_t cb, void *app_data);

/**
 * nss_crypto_cmn_notify_unregister
 *	De-register the event callback handler with NSS driver.
 *
 * @datatypes
 * nss_ctx_instance
 *
 * @param[in] ctx  Pointer to the NSS context per NSS core.
 *
 * @return
 * None.
 */
extern void nss_crypto_cmn_notify_unregister(struct nss_ctx_instance *ctx);

/**
 * nss_crypto_cmn_data_register
 *	Crypto data register.
 *
 * @datatypes
 * nss_crypto_cmn_buf_callback_t \n
 * net_device
 *
 * @param[in] if_num    Interface number.
 * @param[in] cb        Callback function.
 * @param[in] netdev    Net device.
 * @param[in] features  Features supported.
 *
 * @return
 * Pointer to the NSS core context.
 */
extern struct nss_ctx_instance *nss_crypto_cmn_data_register(uint32_t if_num,
						nss_crypto_cmn_buf_callback_t cb,
						struct net_device *netdev,
						uint32_t features);

/**
 * nss_crypto_cmn_data_unregister
 *	Crypto data de-register.
 *
 * @param[in] ctx     NSS context per NSS core.
 * @param[in] if_num  Interface number.
 *
 * @return
 * None.
 */
extern void nss_crypto_cmn_data_unregister(struct nss_ctx_instance *ctx, uint32_t if_num);

/**
 * nss_crypto_cmn_get_context
 *	Get the per NSS core context enabled for crypto.
 *
 * @return
 * Pointer to the NSS core context.
 */
extern struct nss_ctx_instance *nss_crypto_cmn_get_context(void);

/**
 * nss_crypto_cmn_msg_init
 *	Crypto common message initialization.
 *
 * @datatypes
 * nss_crypto_cmn_msg \n
 * nss_crypto_cmn_msg_callback_t
 *
 * @param[in] ncm       Crypto common message.
 * @param[in] if_num    Interface number.
 * @param[in] type      Message type.
 * @param[in] len       Common message length.
 * @param[in] cb        Callback function.
 * @param[in] app_data  Appllication data.
 *
 * @return
 * None.
 */
extern void nss_crypto_cmn_msg_init(struct nss_crypto_cmn_msg *ncm, uint16_t if_num,
				uint32_t type, uint32_t len, nss_crypto_cmn_msg_callback_t cb,
				void *app_data);

#endif /*__KERNEL__ */

/**
 * @}
 */
#endif

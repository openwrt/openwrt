/*
 **************************************************************************
 * Copyright (c) 2017-2021, The Linux Foundation. All rights reserved.
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
 * @file nss_dtls_cmn.h
 *	NSS DTLS common interface definitions, supports inner/outer interface split.
 */

#ifndef _NSS_DTLS_CMN_H_
#define _NSS_DTLS_CMN_H_

/**
 * @addtogroup nss_dtls_subsystem
 * @{
 */

#define NSS_DTLS_CMN_CTX_HDR_IPV6 0x0001		/**< DTLS with IPv6. */
#define NSS_DTLS_CMN_CTX_HDR_UDPLITE 0x0002		/**< DTLS with UDPLite. */
#define NSS_DTLS_CMN_CTX_HDR_CAPWAP 0x0004		/**< DTLS with CAPWAP. */
#define NSS_DTLS_CMN_CTX_CIPHER_MODE_GCM 0x0008		/**< DTLS with GCM cipher mode. */
#define NSS_DTLS_CMN_CTX_ENCAP_UDPLITE_CSUM 0x10000	/**< Checksum only UDPLite header. */
#define NSS_DTLS_CMN_CTX_ENCAP_METADATA 0x20000		/**< Valid metadata in encapsulation direction. */
#define NSS_DTLS_CMN_CTX_DECAP_ACCEPT_ALL 0x40000	/**< Exception all error packets to host. */

#define NSS_DTLS_CMN_CLE_MAX 32				/**< Max classification error. */

/**
 * nss_dtls_cmn_metadata_types
 *	Message types for DTLS requests and responses.
 */
enum nss_dtls_cmn_msg_type {
	NSS_DTLS_CMN_MSG_TYPE_CONFIGURE_NODE,	/**< Configure DTLS firmware node. */
	NSS_DTLS_CMN_MSG_TYPE_CONFIGURE_HDR,	/**< Configure the base context parameter. */
	NSS_DTLS_CMN_MSG_TYPE_CONFIGURE_DTLS,	/**< Configure DTLS parameters. */
	NSS_DTLS_CMN_MSG_TYPE_SWITCH_DTLS,	/**< Switch to new DTLS transform. */
	NSS_DTLS_CMN_MSG_TYPE_DECONFIGURE,	/**< Deconfigure context. */
	NSS_DTLS_CMN_MSG_TYPE_SYNC_STATS,	/**< Synchronize statistics. */
	NSS_DTLS_CMN_MSG_TYPE_NODE_STATS,	/**< Node statistics. */
	NSS_DTLS_CMN_MSG_MAX
};

/**
 * nss_dtls_cmn_error_response_types
 *	Error types for DTLS responses.
 */
enum nss_dtls_cmn_error {
	NSS_DTLS_CMN_ERROR_NONE = 0,
	NSS_DTLS_CMN_ERROR_UNKNOWN_MSG,
	NSS_DTLS_CMN_ERROR_INVALID_DESTIF,
	NSS_DTLS_CMN_ERROR_INVALID_SRCIF,
	NSS_DTLS_CMN_ERROR_INVALID_CRYPTO,
	NSS_DTLS_CMN_ERROR_INVALID_VER,
	NSS_DTLS_CMN_ERROR_INVALID_CTX_TYPE,
	NSS_DTLS_CMN_ERROR_INVALID_CTX_WORDS,
	NSS_DTLS_CMN_ERROR_FAIL_ALLOC_HWCTX,
	NSS_DTLS_CMN_ERROR_FAIL_COPY_CTX,
	NSS_DTLS_CMN_ERROR_FAIL_SWITCH_HWCTX,
	NSS_DTLS_CMN_ERROR_ALREADY_CONFIGURED,
	NSS_DTLS_CMN_ERROR_FAIL_NOMEM,
	NSS_DTLS_CMN_ERROR_FAIL_COPY_NONCE,
	NSS_DTLS_CMN_ERROR_MAX,
};

/**
 * nss_dtls_cmn_ctx_stats_types
 *	DTLS common context statistics types.
 */
enum nss_dtls_cmn_ctx_stats_types {
	NSS_DTLS_CMN_STATS_RX_SINGLE_REC = NSS_STATS_NODE_MAX,
						/**< Received single DTLS record datagrams. */
	NSS_DTLS_CMN_STATS_RX_MULTI_REC,	/**< Received multiple DTLS record datagrams. */
	NSS_DTLS_CMN_STATS_FAIL_CRYPTO_RESOURCE,/**< Failure in crypto resource allocation. */
	NSS_DTLS_CMN_STATS_FAIL_CRYPTO_ENQUEUE,	/**< Failure due to full queue in crypto or hardware. */
	NSS_DTLS_CMN_STATS_FAIL_HEADROOM,	/**< Failure in headroom check. */
	NSS_DTLS_CMN_STATS_FAIL_TAILROOM,	/**< Failure in tailroom check. */
	NSS_DTLS_CMN_STATS_FAIL_VER,		/**< Failure in DTLS version check. */
	NSS_DTLS_CMN_STATS_FAIL_EPOCH,		/**< Failure in DTLS epoch check. */
	NSS_DTLS_CMN_STATS_FAIL_DTLS_RECORD,	/**< Failure in reading DTLS record. */
	NSS_DTLS_CMN_STATS_FAIL_CAPWAP,		/**< Failure in CAPWAP classification. */
	NSS_DTLS_CMN_STATS_FAIL_REPLAY,		/**< Failure in anti-replay check. */
	NSS_DTLS_CMN_STATS_FAIL_REPLAY_DUP,	/**< Failure in anti-replay; duplicate records. */
	NSS_DTLS_CMN_STATS_FAIL_REPLAY_WIN,	/**< Failure in anti-replay; packet outside the window. */
	NSS_DTLS_CMN_STATS_FAIL_QUEUE,		/**< Failure due to full queue in DTLS. */
	NSS_DTLS_CMN_STATS_FAIL_QUEUE_NEXTHOP,	/**< Failure due to full queue in next hop. */
	NSS_DTLS_CMN_STATS_FAIL_PBUF_ALLOC,	/**< Failure in pbuf allocation. */
	NSS_DTLS_CMN_STATS_FAIL_PBUF_LINEAR,	/**< Failure in pbuf linearization. */
	NSS_DTLS_CMN_STATS_FAIL_PBUF_STATS,	/**< Failure in pbuf allocation for statistics. */
	NSS_DTLS_CMN_STATS_FAIL_PBUF_ALIGN,	/**< Failure in pbuf alignment. */
	NSS_DTLS_CMN_STATS_FAIL_CTX_ACTIVE,	/**< Failure in enqueue due to inactive context. */
	NSS_DTLS_CMN_STATS_FAIL_HWCTX_ACTIVE,	/**< Failure in enqueue due to inactive hardware context. */
	NSS_DTLS_CMN_STATS_FAIL_CIPHER,		/**< Failure in decrypting the data. */
	NSS_DTLS_CMN_STATS_FAIL_AUTH,		/**< Failure in authenticating the data. */
	NSS_DTLS_CMN_STATS_FAIL_SEQ_OVF,	/**< Failure due to sequence number overflow. */
	NSS_DTLS_CMN_STATS_FAIL_BLK_LEN,	/**< Failure in decapsulation due to bad cipher length. */
	NSS_DTLS_CMN_STATS_FAIL_HASH_LEN,	/**< Failure in decapsulation due to bad hash length. */
	NSS_DTLS_CMN_STATS_LEN_ERROR,		/**< Length error. */
	NSS_DTLS_CMN_STATS_TOKEN_ERROR,		/**< Token error, unknown token command or instruction. */
	NSS_DTLS_CMN_STATS_BYPASS_ERROR,	/**< Token contains too much bypass data. */
	NSS_DTLS_CMN_STATS_CONFIG_ERROR,	/**< Invalid command, algorithm, or mode combination. */
	NSS_DTLS_CMN_STATS_ALGO_ERROR,		/**< Unsupported algorithm. */
	NSS_DTLS_CMN_STATS_HASH_OVF_ERROR,	/**< Hash input overflow. */
	NSS_DTLS_CMN_STATS_TTL_ERROR,		/**< TTL or HOP-Limit underflow. */
	NSS_DTLS_CMN_STATS_CSUM_ERROR,		/**< Checksum error. */
	NSS_DTLS_CMN_STATS_TIMEOUT_ERROR,	/**< Data timed out. */
	NSS_DTLS_CMN_STATS_CLE_ERROR_0,		/**< Classification failure 0. */
	NSS_DTLS_CMN_STATS_CLE_ERROR_1,		/**< Classification failure 1. */
	NSS_DTLS_CMN_STATS_CLE_ERROR_2,		/**< Classification failure 2. */
	NSS_DTLS_CMN_STATS_CLE_ERROR_3,		/**< Classification failure 3. */
	NSS_DTLS_CMN_STATS_CLE_ERROR_4,		/**< Classification failure 4. */
	NSS_DTLS_CMN_STATS_CLE_ERROR_5,		/**< Classification failure 5. */
	NSS_DTLS_CMN_STATS_CLE_ERROR_6,		/**< Classification failure 6. */
	NSS_DTLS_CMN_STATS_CLE_ERROR_7,		/**< Classification failure 7. */
	NSS_DTLS_CMN_STATS_CLE_ERROR_8,		/**< Classification failure 8. */
	NSS_DTLS_CMN_STATS_CLE_ERROR_9,		/**< Classification failure 9. */
	NSS_DTLS_CMN_STATS_CLE_ERROR_10,	/**< Classification failure 10. */
	NSS_DTLS_CMN_STATS_CLE_ERROR_11,	/**< Classification failure 11. */
	NSS_DTLS_CMN_STATS_CLE_ERROR_12,	/**< Classification failure 12. */
	NSS_DTLS_CMN_STATS_CLE_ERROR_13,	/**< Classification failure 13. */
	NSS_DTLS_CMN_STATS_CLE_ERROR_14,	/**< Classification failure 14. */
	NSS_DTLS_CMN_STATS_CLE_ERROR_15,	/**< Classification failure 15. */
	NSS_DTLS_CMN_STATS_CLE_ERROR_16,	/**< Classification failure 16. */
	NSS_DTLS_CMN_STATS_CLE_ERROR_17,	/**< Classification failure 17. */
	NSS_DTLS_CMN_STATS_CLE_ERROR_18,	/**< Classification failure 18. */
	NSS_DTLS_CMN_STATS_CLE_ERROR_19,	/**< Classification failure 19. */
	NSS_DTLS_CMN_STATS_CLE_ERROR_20,	/**< Classification failure 20. */
	NSS_DTLS_CMN_STATS_CLE_ERROR_21,	/**< Classification failure 21. */
	NSS_DTLS_CMN_STATS_CLE_ERROR_22,	/**< Classification failure 22. */
	NSS_DTLS_CMN_STATS_CLE_ERROR_23,	/**< Classification failure 23. */
	NSS_DTLS_CMN_STATS_CLE_ERROR_24,	/**< Classification failure 24. */
	NSS_DTLS_CMN_STATS_CLE_ERROR_25,	/**< Classification failure 25. */
	NSS_DTLS_CMN_STATS_CLE_ERROR_26,	/**< Classification failure 26. */
	NSS_DTLS_CMN_STATS_CLE_ERROR_27,	/**< Classification failure 27. */
	NSS_DTLS_CMN_STATS_CLE_ERROR_28,	/**< Classification failure 28. */
	NSS_DTLS_CMN_STATS_CLE_ERROR_29,	/**< Classification failure 29. */
	NSS_DTLS_CMN_STATS_CLE_ERROR_30,	/**< Classification failure 30. */
	NSS_DTLS_CMN_STATS_CLE_ERROR_31,	/**< Classification failure 31. */
	NSS_DTLS_CMN_STATS_SEQ_LOW,		/**< Lower 32 bits of current transmit sequence number. */
	NSS_DTLS_CMN_STATS_SEQ_HIGH,		/**< Upper 16 bits of current transmit sequence number. */
	NSS_DTLS_CMN_STATS_EPOCH,		/**< Current epoch value. */
	NSS_DTLS_CMN_CTX_STATS_MAX,		/**< Maximum message type. */
};

/**
 * nss_dtls_cmn_node_stats
 * 	DTLS node statistics.
 */
struct nss_dtls_cmn_node_stats {
	uint32_t fail_ctx_alloc;	/**< Failure in allocating a context. */
	uint32_t fail_ctx_free;		/**< Failure in freeing up the context. */
	uint32_t fail_pbuf_stats;	/**< Failure in pbuf allocation for statistics. */
};

/**
 * nss_dtls_cmn_hw_stats
 * 	DTLS hardware statistics.
 */
struct nss_dtls_cmn_hw_stats {
	uint32_t len_error;             /**< Length error. */
	uint32_t token_error;           /**< Token error, unknown token command/instruction. */
	uint32_t bypass_error;          /**< Token contains too much bypass data. */
	uint32_t config_error;          /**< Invalid command/algorithm/mode/combination. */
	uint32_t algo_error;            /**< Unsupported algorithm. */
	uint32_t hash_ovf_error;        /**< Hash input overflow. */
	uint32_t ttl_error;             /**< TTL or HOP-Limit underflow. */
	uint32_t csum_error;            /**< Checksum error. */
	uint32_t timeout_error;         /**< Data timed-out. */
};

/**
 * nss_dtls_cmn_ctx_stats
 *	DTLS session statistics.
 */
struct nss_dtls_cmn_ctx_stats {
	struct nss_cmn_node_stats pkt;		/**< Common node statistics. */
	uint32_t rx_single_rec;			/**< Received single DTLS record datagrams. */
	uint32_t rx_multi_rec;			/**< Received multiple DTLS record datagrams. */
	uint32_t fail_crypto_resource;		/**< Failure in allocation of crypto resource. */
	uint32_t fail_crypto_enqueue;		/**< Failure due to queue full in crypto or hardware. */
	uint32_t fail_headroom;			/**< Failure in headroom check. */
	uint32_t fail_tailroom;			/**< Failure in tailroom check. */
	uint32_t fail_ver;			/**< Failure in DTLS version check. */
	uint32_t fail_epoch;			/**< Failure in DTLS epoch check. */
	uint32_t fail_dtls_record;		/**< Failure in reading DTLS record. */
	uint32_t fail_capwap;			/**< Failure in CAPWAP classification. */
	uint32_t fail_replay;			/**< Failure in anti-replay check. */
	uint32_t fail_replay_dup;		/**< Failure in anti-replay; duplicate records. */
	uint32_t fail_replay_win;		/**< Failure in anti-replay; packet outside the window. */
	uint32_t fail_queue;			/**< Failure due to queue full in DTLS. */
	uint32_t fail_queue_nexthop;		/**< Failure due to queue full in next_hop. */
	uint32_t fail_pbuf_alloc;		/**< Failure in pbuf allocation. */
	uint32_t fail_pbuf_linear;		/**< Failure in pbuf linearization. */
	uint32_t fail_pbuf_stats;		/**< Failure in pbuf allocation for statistics. */
	uint32_t fail_pbuf_align;		/**< Failure in pbuf alignment. */
	uint32_t fail_ctx_active;		/**< Failure in enqueue due to inactive context. */
	uint32_t fail_hwctx_active;		/**< Failure in enqueue due to inactive hardware context. */
	uint32_t fail_cipher;			/**< Failure in decrypting the data. */
	uint32_t fail_auth;			/**< Failure in authenticating the data. */
	uint32_t fail_seq_ovf;			/**< Failure due to sequence number overflow. */
	uint32_t fail_blk_len;			/**< Failure in decapsulation due to bad cipher block length. */
	uint32_t fail_hash_len;			/**< Failure in decapsulation due to bad hash block length. */

	struct nss_dtls_cmn_hw_stats fail_hw;	/**< Hardware failure statistics. */

	uint32_t fail_cle[NSS_DTLS_CMN_CLE_MAX];/**< Classification errors. */

	uint32_t seq_low;			/**< Lower 32 bits of current Tx sequence number. */
	uint32_t seq_high;			/**< Upper 16 bits of current Tx sequence number. */

	uint16_t epoch;				/**< Current epoch value. */
	uint8_t res1[2];			/**< Reserved for future use. */

	uint8_t res2[16];			/**< Reserved for future use. */
};

/**
 * nss_dtls_cmn_ctx_config_hdr
 *	Parameters for outer header transform.
 */
struct nss_dtls_cmn_ctx_config_hdr {
	uint32_t flags;		/**< Context flags. */
	uint32_t dest_ifnum;	/**< Destination interface for packets. */
	uint32_t src_ifnum;	/**< Source interface of packets. */
	uint32_t sip[4];	/**< Source IPv4/v6 address. */
	uint32_t dip[4];	/**< Destination IPv4/v6 address. */

	uint16_t sport;		/**< Source UDP/UDPLite port. */
	uint16_t dport;		/**< Destination UDP/UDPLite port. */

	uint8_t hop_limit_ttl;	/**< IP header TTL field. */
	uint8_t dscp;		/**< DSCP value. */
	uint8_t dscp_copy; 	/**< Copy DSCP value. */
	uint8_t df;		/**< Do not fragment DTLS over IPv4. */
};

/**
 * nss_dtls_cmn_ctx_config_dtls
 *	Parameters for DTLS transform.
 */
struct nss_dtls_cmn_ctx_config_dtls {
	uint32_t ver;		/**< Version (enum dtls_cmn_ver). */
	uint32_t crypto_idx;	/**< Crypto index for cipher context. */

	uint16_t window_size;	/**< Anti-replay window size. */
	uint16_t epoch;		/**< Initial epoch value. */

	uint8_t iv_len;		/**< Crypto IV length for encapsulation. */
	uint8_t hash_len;	/**< Auth hash length for encapsulation. */
	uint8_t blk_len;	/**< Cipher block length. */
	uint8_t res1;		/**< Reserved for alignment. */
};

/**
 * nss_dtls_cmn_stats_notification
 *	DTLS common transmission statistics structure.
 */
struct nss_dtls_cmn_stats_notification {
	uint64_t stats_ctx[NSS_DTLS_CMN_CTX_STATS_MAX];		/**< Context transmission statistics. */
	uint32_t core_id;					/**< Core ID. */
	uint32_t if_num;					/**< Interface number. */
};

/**
 * nss_dtls_cmn_msg
 *	Data for sending and receiving DTLS messages.
 */
struct nss_dtls_cmn_msg {
	struct nss_cmn_msg cm;		/**< Common message header. */

	/**
	 * Payload of a DTLS message.
	 */
	union {
		struct nss_dtls_cmn_ctx_config_hdr hdr_cfg;	/**< Session configuration. */
		struct nss_dtls_cmn_ctx_config_dtls dtls_cfg;	/**< Cipher update information. */
		struct nss_dtls_cmn_ctx_stats stats;		/**< Session statistics. */
		struct nss_dtls_cmn_node_stats node_stats;	/**< Node statistics. */
	} msg;			/**< Message payload for DTLS session messages exchanged with NSS core. */
};

#ifdef __KERNEL__ /* only for kernel use. */
/**
 * Callback function for receiving DTLS messages.
 *
 * @datatypes
 * nss_dtls_cmn_msg
 *
 * @param[in] app_data  Pointer to the application context of the message.
 * @param[in] msg       Pointer to the message data.
 */
typedef void (*nss_dtls_cmn_msg_callback_t)(void *app_data, struct nss_cmn_msg *msg);

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
typedef void (*nss_dtls_cmn_data_callback_t)(struct net_device *netdev, struct sk_buff *skb, struct napi_struct *napi);

/**
 * nss_dtls_cmn_tx_buf
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
nss_tx_status_t nss_dtls_cmn_tx_buf(struct sk_buff *os_buf, uint32_t if_num, struct nss_ctx_instance *nss_ctx);

/**
 * nss_dtls_cmn_tx_msg
 *	Sends DTLS messages.
 *
 * @param[in]     nss_ctx  Pointer to the NSS core context.
 * @param[in,out] msg      Pointer to the message data.
 *
 * @return
 * Status of the Tx operation.
 */
extern nss_tx_status_t nss_dtls_cmn_tx_msg(struct nss_ctx_instance *nss_ctx, struct nss_dtls_cmn_msg *msg);

/**
 * nss_dtls_cmn_tx_msg_sync
 *	Sends DTLS messages synchronously.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_dtls_cmn_msg_type \n
 * nss_dtls_cmn_msg \n
 * nss_dtls_cmn_error
 *
 * @param[in]     nss_ctx  Pointer to the NSS context.
 * @param[in]     if_num   NSS interface number.
 * @param[in]     type     Type of message.
 * @param[in]     len      Size of the payload.
 * @param[in]     ndcm     Pointer to the message data.
 * @param[in,out] resp     Response for the configuration.
 *
 * @return
 * Status of the Tx operation.
 */
extern nss_tx_status_t nss_dtls_cmn_tx_msg_sync(struct nss_ctx_instance *nss_ctx, uint32_t if_num,
						enum nss_dtls_cmn_msg_type type, uint16_t len,
						struct nss_dtls_cmn_msg *ndcm, enum nss_dtls_cmn_error *resp);

/**
 * nss_dtls_cmn_unregister_if
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
extern void nss_dtls_cmn_unregister_if(uint32_t if_num);

/**
 * nss_dtls_cmn_register_if
 *	Registers a DTLS session interface with the NSS for sending and receiving
 *	messages.
 *
 * @datatypes
 * nss_dtls_cmn_data_callback_t \n
 * nss_dtls_cmn_msg_callback_t
 *
 * @param[in] if_num    NSS interface number.
 * @param[in] data_cb   Callback function for the message.
 * @param[in] msg_cb    Callback for DTLS tunnel message.
 * @param[in] netdev    Pointer to the associated network device.
 * @param[in] features  Data socket buffer types supported by this interface.
 * @param[in] type      Type of message.
 * @param[in] app_ctx   Pointer to the application context.
 *
 * @return
 * Pointer to the NSS core context.
 */
extern struct nss_ctx_instance *nss_dtls_cmn_register_if(uint32_t if_num,
							 nss_dtls_cmn_data_callback_t data_cb,
							 nss_dtls_cmn_msg_callback_t msg_cb,
							 struct net_device *netdev,
							 uint32_t features,
							 uint32_t type,
							 void *app_ctx);

/**
 * nss_dtls_cmn_notify_unregister
 *	Deregisters an event callback.
 *
 * @param[in] ifnum  NSS interface number.
 *
 * @return
 * None.
 */
extern void nss_dtls_cmn_notify_unregister(uint32_t ifnum);

/**
 * nss_dtls_cmn_notify_register
 *	Registers an event callback to handle notification from DTLS firmware package.
 *
 * @param[in] ifnum     NSS interface number.
 * @param[in] ev_cb     Callback for DTLS tunnel message.
 * @param[in] app_data  Pointer to the application context.
 *
 * @return
 * Pointer to NSS core context.
 */
extern struct nss_ctx_instance *nss_dtls_cmn_notify_register(uint32_t ifnum, nss_dtls_cmn_msg_callback_t ev_cb,
							     void *app_data);

/**
 * nss_dtls_cmn_msg_init
 *	Initializes a DTLS message.
 *
 * @datatypes
 * nss_dtls_cmn_msg
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
extern void nss_dtls_cmn_msg_init(struct nss_dtls_cmn_msg *ncm, uint32_t if_num, uint32_t type, uint32_t len, void *cb,
				void *app_data);

/**
 * nss_dtls_cmn_get_context
 *	Gets the NSS core context for the DTLS session.
 *
 * @return
 * Pointer to the NSS core context.
 */
extern struct nss_ctx_instance *nss_dtls_cmn_get_context(void);

/**
 * nss_dtls_cmn_get_ifnum
 *	Gets the DTLS interface number with a core ID.
 *
 * @param[in] if_num  NSS interface number.
 *
 * @return
 * Interface number with the core ID.
 */
extern int32_t nss_dtls_cmn_get_ifnum(int32_t if_num);

/**
 * nss_dtls_cmn_stats_unregister_notifier
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
extern int nss_dtls_cmn_stats_unregister_notifier(struct notifier_block *nb);

/**
 * nss_dtls_cmn_stats_register_notifier
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
extern int nss_dtls_cmn_stats_register_notifier(struct notifier_block *nb);

/**
 * @}
 */

#endif /* __KERNEL__ */
#endif /* _NSS_DTLS_CMN_H_. */

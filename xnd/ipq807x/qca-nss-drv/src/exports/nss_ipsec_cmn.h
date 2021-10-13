/*
 **************************************************************************
 * Copyright (c) 2018-2021, The Linux Foundation. All rights reserved.
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
 * @file nss_ipsec_cmn.h
 *	NSS IPsec interface definitions.
 */

#ifndef __NSS_IPSEC_CMN_H_
#define __NSS_IPSEC_CMN_H_

/**
 * @addtogroup nss_ipsec_subsystem
 * @{
 */

#define NSS_IPSEC_CMN_ARPHRD_IPSEC 31 /**< ARP (iana.org) hardware type for an IPsec tunnel. */

/**
 * Flags for SA configuration.
 */
#define NSS_IPSEC_CMN_FLAG_IPV6 (0x1 << 0)		/**< IPv6 header. */
#define NSS_IPSEC_CMN_FLAG_IPV4_NATT (0x1 << 1)		/**< IPv4 NAT traversal. */
#define NSS_IPSEC_CMN_FLAG_IPV4_UDP (0x1 << 2)		/**< IPv4 UDP traversal. */
#define NSS_IPSEC_CMN_FLAG_ESP_ESN (0x1 << 3)		/**< Enable ESP extended sequence number. */
#define NSS_IPSEC_CMN_FLAG_ESP_SKIP (0x1 << 4)		/**< Skip ESP sequence number and ICV. */
#define NSS_IPSEC_CMN_FLAG_ESP_REPLAY (0x1 << 5)	/**< Check ESP replay counter. */
#define NSS_IPSEC_CMN_FLAG_CIPHER_NULL (0x1 << 6)	/**< NULL cipher mode. */
#define NSS_IPSEC_CMN_FLAG_CIPHER_GCM (0x1 << 7)	/**< Galios counter mode. */
#define NSS_IPSEC_CMN_FLAG_COPY_DSCP (0x1 << 8)		/**< Copy DSCP from inner to outer header. */
#define NSS_IPSEC_CMN_FLAG_COPY_DF (0x1 << 9)		/**< Copy DF from inner node to outer node. */
#define NSS_IPSEC_CMN_FLAG_MODE_TRANS (0x1 << 10)	/**< Encapsulate or decapsulate in transport mode (default is tunnel mode). */

#define NSS_IPSEC_CMN_FLAG_HDR_MASK \
	(NSS_IPSEC_CMN_FLAG_IPV6 | NSS_IPSEC_CMN_FLAG_IPV4_NATT | NSS_IPSEC_CMN_FLAG_IPV4_UDP)
							/**< Flag header mask. */

#define NSS_IPSEC_CMN_FEATURE_INLINE_ACCEL 0x1		/**< Interface enabled for inline exception. */

#define NSS_IPSEC_CMN_MDATA_VERSION 0x01		/**< Metadata version. */
#define NSS_IPSEC_CMN_MDATA_MAGIC 0x8893		/**< Metadata magic. */
#define NSS_IPSEC_CMN_MDATA_ORIGIN_HOST 0x01		/**< Metadata originates at the host. */
#define NSS_IPSEC_CMN_MDATA_ALIGN_SZ sizeof(uint32_t)	/**< Metadata alignment size. */
/**
 * nss_ipsec_cmn_msg_type
 *	IPsec message types.
 */
enum nss_ipsec_cmn_msg_type {
	NSS_IPSEC_CMN_MSG_TYPE_NONE = 0,		/**< Nothing to do. */
	NSS_IPSEC_CMN_MSG_TYPE_NODE_CONFIG = 1,		/**< Configure IPsec node. */
	NSS_IPSEC_CMN_MSG_TYPE_CTX_CONFIG = 2,		/**< Configure IPsec dynamic node. */
	NSS_IPSEC_CMN_MSG_TYPE_CTX_SYNC = 3,		/**< Synchronize context statistics to host. */
	NSS_IPSEC_CMN_MSG_TYPE_SA_CREATE = 4,		/**< Create SA. */
	NSS_IPSEC_CMN_MSG_TYPE_SA_DESTROY = 5,		/**< Destroy SA. */
	NSS_IPSEC_CMN_MSG_TYPE_SA_SYNC = 6,		/**< Synchronize SA statistics to host. */
	NSS_IPSEC_CMN_MSG_TYPE_FLOW_CREATE = 7,		/**< Create flow. */
	NSS_IPSEC_CMN_MSG_TYPE_FLOW_DESTROY = 8,	/**< Delete flow. */
	NSS_IPSEC_CMN_MSG_TYPE_MAX
};

/**
 * nss_ipsec_cmn_msg_error
 *	IPsec message error types.
 */
enum nss_ipsec_cmn_msg_error {
	NSS_IPSEC_CMN_MSG_ERROR_NONE = 0,		/**< No error. */
	NSS_IPSEC_CMN_MSG_ERROR_CTX_INVAL = 1,		/**< Invalid context. */
	NSS_IPSEC_CMN_MSG_ERROR_SA_ALLOC = 2,		/**< Failed to allocate SA. */
	NSS_IPSEC_CMN_MSG_ERROR_SA_INVAL = 3,		/**< Invalid SA. */
	NSS_IPSEC_CMN_MSG_ERROR_SA_DUP = 4,			/**< SA exists. */
	NSS_IPSEC_CMN_MSG_ERROR_SA_INUSE = 5,		/**< SA is in use. */
	NSS_IPSEC_CMN_MSG_ERROR_FLOW_ALLOC = 6,		/**< Failed to allocate flow. */
	NSS_IPSEC_CMN_MSG_ERROR_FLOW_INVAL = 7,		/**< Flow not found. */
	NSS_IPSEC_CMN_MSG_ERROR_FLOW_DUP = 8,		/**< Duplicate flow. */
	NSS_IPSEC_CMN_MSG_ERROR_FLOW_SA = 9,		/**< Failed to find SA for the flow. */
	NSS_IPSEC_CMN_MSG_ERROR_NODE_REG_DYNIF = 10,
							/**< Error registering dynamic interface. */
	NSS_IPSEC_CMN_MSG_ERROR_UNHANDLED_MSG= 11,	/**< Unhandled message type. */
	NSS_IPSEC_CMN_MSG_ERROR_MAX			/**< Maximum error message. */
};

/**
 * nss_ipsec_cmn_ctx_type
 *	IPsec context type.
 */
enum nss_ipsec_cmn_ctx_type {
	NSS_IPSEC_CMN_CTX_TYPE_NONE = 0,	/**< Invalid direction. */
	NSS_IPSEC_CMN_CTX_TYPE_INNER,		/**< Encapsulation. */
	NSS_IPSEC_CMN_CTX_TYPE_MDATA_INNER,	/**< Metadata for encapsulation. */
	NSS_IPSEC_CMN_CTX_TYPE_OUTER,		/**< Decapsulation. */
	NSS_IPSEC_CMN_CTX_TYPE_MDATA_OUTER,	/**< Metadata for decapsulation. */
	NSS_IPSEC_CMN_CTX_TYPE_REDIR,		/**< Redirect. */
	NSS_IPSEC_CMN_CTX_TYPE_MAX
};

/**
 * nss_ipsec_cmn_stats_types
 *	IPsec common statistics types.
 */
enum nss_ipsec_cmn_stats_types {
	NSS_IPSEC_CMN_STATS_FAIL_HEADROOM = NSS_STATS_NODE_MAX,
						/**< Failure in headroom check. */
	NSS_IPSEC_CMN_STATS_FAIL_TAILROOM,	/**< Failure in tailroom check. */
	NSS_IPSEC_CMN_STATS_FAIL_REPLAY,	/**< Failure in anti-replay check. */
	NSS_IPSEC_CMN_STATS_FAIL_REPLAY_DUP,	/**< Failure in anti-replay; duplicate records. */
	NSS_IPSEC_CMN_STATS_FAIL_REPLAY_WIN,	/**< Failure in anti-replay; packet outside the window. */
	NSS_IPSEC_CMN_STATS_FAIL_PBUF_CRYPTO,	/**< Failure in crypto pbuf allocation. */
	NSS_IPSEC_CMN_STATS_FAIL_QUEUE,		/**< Failure due to queue full in IPsec. */
	NSS_IPSEC_CMN_STATS_FAIL_QUEUE_CRYPTO,	/**< Failure due to queue full in crypto. */
	NSS_IPSEC_CMN_STATS_FAIL_QUEUE_NEXTHOP,	/**< Failure due to queue full in next hop. */
	NSS_IPSEC_CMN_STATS_FAIL_PBUF_ALLOC,	/**< Failure in pbuf allocation. */
	NSS_IPSEC_CMN_STATS_FAIL_PBUF_LINEAR,	/**< Failure in pbuf linearization. */
	NSS_IPSEC_CMN_STATS_FAIL_PBUF_STATS,	/**< Failure in pbuf allocation for statistics. */
	NSS_IPSEC_CMN_STATS_FAIL_PBUF_ALIGN,	/**< Failure in pbuf access due to non-word alignmnt */
	NSS_IPSEC_CMN_STATS_FAIL_CIPHER,	/**< Failure in decrypting the data. */
	NSS_IPSEC_CMN_STATS_FAIL_AUTH,		/**< Failure in authenticating the data. */
	NSS_IPSEC_CMN_STATS_FAIL_SEQ_OVF,	/**< Failure due to sequence number rollover. */
	NSS_IPSEC_CMN_STATS_FAIL_BLK_LEN,	/**< Failure in decapsulation due to bad cipher block length. */
	NSS_IPSEC_CMN_STATS_FAIL_HASH_LEN,	/**< Failure in decapsulation due to bad hash block length. */
	NSS_IPSEC_CMN_STATS_FAIL_TRANSFORM,	/**< Failure in transformation; general error. */
	NSS_IPSEC_CMN_STATS_FAIL_CRYPTO,	/**< Failure in crypto transformation. */
	NSS_IPSEC_CMN_STATS_FAIL_CLE,		/**< Failure in classification; general failure. */
	NSS_IPSEC_CMN_STATS_IS_STOPPED,		/**< Indicates if SA is stopped; for example: sequence overflow. */
	NSS_IPSEC_CMN_STATS_MAX,		/**< Maximum statistics type. */
};

/**
 * nss_ipsec_cmn_flow_tuple
 *	IPsec tuple for creating flow entries.
 *
 * Note: This is a common selector which is used for preparing
 * a lookup tuple for incoming packets. The tuple is used
 * for computing the hash index in the flow table. There are multiple
 * fields in the tuple and the recipient node decides which fields
 * it must use from the tuple to calculate the hash index. The host
 * has no view of the hash index and hence must compute its own index
 * based on the tuple.
 */
struct nss_ipsec_cmn_flow_tuple {
	uint32_t dest_ip[4];		/**< Destination IP. */
	uint32_t src_ip[4];		/**< Source IP. */
	uint32_t spi_index;		/**< ESP SPI index. */

	uint16_t dst_port;		/**< Destination L4 port. */
	uint16_t src_port;		/**< Source L4 port. */

	uint8_t user_pattern;		/**< User defined field. */
	uint8_t protocol;		/**< IP protocol types. */
	uint8_t ip_ver;			/**< IP version. */
};

/**
 *nss_ipsec_cmn_sa_tuple
 *	IPsec outer header configuration.
 */
struct nss_ipsec_cmn_sa_tuple {
	uint32_t dest_ip[4];		/**< Destination IP. */
	uint32_t src_ip[4];		/**< Source IP. */
	uint32_t spi_index;		/**< ESP SPI index. */

	uint16_t dest_port;		/* Destination L4 port. */
	uint16_t src_port;		/* Source L4 port. */

	uint16_t crypto_index;		/**< Crypto index for the SA. */
	uint8_t protocol;		/**< Outer protocol. */
	uint8_t ip_ver;			/**< IP version. */

	uint8_t hop_limit;		/**< Time-to-Live or next hop limit. */
	uint8_t res[3];			/**< Reserved. */
};

/**
 *nss_ipsec_cmn_sa_data
 *	IPsec SA data used for transformation.
 */
struct nss_ipsec_cmn_sa_data {
	uint32_t seq_start;		/**< Starting sequence number. */
	uint32_t flags;			/**< Configuration flags. */

	uint16_t window_size;		/**< ESP sequence number window. */
	uint8_t dscp;			/**< Default DSCP value of the SA. */
	uint8_t df;			/**< Default do not fragment value of the SA. */

	uint8_t blk_len;		/**< Cipher block length. */
	uint8_t iv_len;			/**< IV length. */
	uint8_t icv_len;		/**< ESP trailers ICV length to apply. */
	uint8_t res1;			/**< Reserved. */

	uint32_t res2[4];		/**< Reserved for future use. */
};

/**
 * nss_ipsec_cmn_flow
 *	IPsec flow configuration message.
 */
struct nss_ipsec_cmn_flow {
	struct nss_ipsec_cmn_flow_tuple flow_tuple;	/**< Flow tuple. */
	struct nss_ipsec_cmn_sa_tuple sa_tuple;		/**< SA tuple. */
};

/**
 * nss_ipsec_cmn_sa
 *	IPsec SA configuration message.
 */
struct nss_ipsec_cmn_sa {
	struct nss_ipsec_cmn_sa_tuple sa_tuple;		/**< SA tuple. */
	struct nss_ipsec_cmn_sa_data sa_data;		/**< SA data. */
};

/**
 * nss_ipsec_cmn_ctx
 *	IPsec context configuration.
 */
struct nss_ipsec_cmn_ctx {
	enum nss_ipsec_cmn_ctx_type type;	/**< Node type. */
	uint32_t except_ifnum;			/**< Exception interface for egress. */
	uint32_t sibling_ifnum;			/**< Sibling interface. */
};

/**
 * nss_ipsec_cmn_node
 *	IPsec node configuration.
 */
struct nss_ipsec_cmn_node {
	bool dma_redirect;		/**< Enable redirect DMA ring. */
	bool dma_lookaside;		/**< Enable lookaside DMA ring. */
	uint16_t max_sa;		/**< Maximum number of SA(s) supported. */
};

/**
 * nss_ipsec_cmn_sa_replay
 *	IPsec replay statistics
 */
struct nss_ipsec_cmn_sa_replay {
	uint64_t seq_start;		/**< Start of replay window. */
	uint64_t seq_cur;		/**< Current sequence number. */
	uint16_t window_size;		/**< Window size. */
	uint8_t res[6];			/**< Reserved for future use. */
};

/**
 * nss_ipsec_cmn_sa_stats
 *	IPsec SA statistics.
 */
struct nss_ipsec_cmn_sa_stats {
	struct nss_cmn_node_stats cmn_stats;	/**< Packet statistics. */
	uint32_t fail_headroom;			/**< Failed headroom check. */
	uint32_t fail_tailroom;			/**< Failed tailroom check. */
	uint32_t fail_replay;			/**< Failure in anti-replay check. */
	uint32_t fail_replay_dup;		/**< Failure in anti-replay; duplicate records. */
	uint32_t fail_replay_win;		/**< Failure in anti-replay; packet outside the window. */
	uint32_t fail_pbuf_crypto;		/**< Failed to allocate crypto pbuf. */
	uint32_t fail_queue;			/**< Failure due to queue full in IPsec. */
	uint32_t fail_queue_crypto;		/**< Failure due to queue full in crypto. */
	uint32_t fail_queue_nexthop;		/**< Failure due to queue full in next hop. */
	uint32_t fail_pbuf_alloc;		/**< Failure in pbuf allocation. */
	uint32_t fail_pbuf_linear;		/**< Failure in pbuf linearization. */
	uint32_t fail_pbuf_stats;		/**< Failure in pbuf allocation for statistics. */
	uint32_t fail_pbuf_align;		/**< Failure in pbuf access due to non-word alignment. */
	uint32_t fail_cipher;			/**< Failure in decrypting the data. */
	uint32_t fail_auth;			/**< Failure in authenticating the data. */
	uint32_t fail_seq_ovf;			/**< Failure due to sequence number rollover. */
	uint32_t fail_blk_len;			/**< Failure in decapsulation due to bad cipher block length. */
	uint32_t fail_hash_len;			/**< Failure in decapsulation due to bad hash block length. */
	uint32_t fail_transform;		/**< Failure in transformation; general error. */
	uint32_t fail_crypto;			/**< Failure in crypto transformation. */
	uint32_t fail_cle;			/**< Failure in classification; general failure. */
	uint32_t is_stopped;			/**< Indicates if SA is stopped; for example, seq overflow. */
};

/**
 * nss_ipsec_cmn_sa_sync
 *	IPsec SA sync message.
 */
struct nss_ipsec_cmn_sa_sync {
	struct nss_ipsec_cmn_sa_replay replay;	/**< Replay statistics. */
	struct nss_ipsec_cmn_sa_tuple sa_tuple;	/**< SA tuple. */
	struct nss_ipsec_cmn_sa_stats stats;	/**< Packet and failure statistics. */
};

/**
 * nss_ipsec_cmn_ctx_stats
 *	IPsec context statistics.
 */
struct nss_ipsec_cmn_ctx_stats {
	struct nss_cmn_node_stats cmn_stats;
					/**< Packet statistics. */
	uint32_t exceptioned;		/**< Exceptioned to host. */
	uint32_t linearized;		/**< Linearized packets. */
	uint32_t redirected;		/**< Redirected from inline. */
	uint32_t dropped;		/**< Total dropped packets. */
	uint32_t fail_sa;		/**< Failed to find SA. */
	uint32_t fail_flow;		/**< Failed to find flow. */
	uint32_t fail_stats;		/**< Failed to send statistics. */
	uint32_t fail_exception;	/**< Failed to exception. */
	uint32_t fail_transform;	/**< Failed to produce output. */
	uint32_t fail_linearized;	/**< Failed to linearize. */
	uint32_t fail_mdata_ver;	/**< Invalid metadata version. */
	uint32_t fail_ctx_active;	/**< Failed to queue as context is not active. */
	uint32_t fail_pbuf_crypto;	/**< Failed to allocate pbuf for crypto operation. */
	uint32_t fail_queue_crypto;	/**< Failed to queue pbuf to crypto pnode. */
};

/**
 * nss_ipsec_cmn_ctx_sync
 *	IPsec context synchronous message.
 */
struct nss_ipsec_cmn_ctx_sync {
	enum nss_ipsec_cmn_ctx_type type;	/**< IPsec context type. */
	struct nss_ipsec_cmn_ctx_stats stats;	/**< Context statistics. */
};

/**
 * nss_ipsec_cmn_mdata_cmn
 *	IPsec common metadata information.
 */
struct nss_ipsec_cmn_mdata_cmn {
	uint8_t version;		/**< Metadata version. */
	uint8_t origin;			/**< Metadata origin (host or NSS). */
	uint16_t len;			/**< Metadata length including extra bytes. */
	uint8_t res[2];			/**< Reserved for future. */
	uint16_t magic;			/**< Metadata magic. */
};

/**
 * nss_ipsec_cmn_mdata_encap
 *	IPsec encapsulation metadata information.
 */
struct nss_ipsec_cmn_mdata_encap {
	struct nss_ipsec_cmn_sa_tuple sa;	/**< SA tuple. */
	uint32_t seq_num;			/**< Sequence number for encapsulation (zero disables it). */
	uint16_t data_len;			/**< Length of data to encapsulate. */
	uint16_t flags;				/**< Encapsulation metadata flags. */
};

/**
 * nss_ipsec_cmn_mdata_decap
 *	IPsec decapsulation metadata information.
 */
struct nss_ipsec_cmn_mdata_decap {
	struct nss_ipsec_cmn_sa_tuple sa;	/**< SA tuple. */
};

/**
 * nss_ipsec_cmn_mdata
 *	IPsec metadata for host originated packets.
 */
struct nss_ipsec_cmn_mdata {
	struct nss_ipsec_cmn_mdata_cmn cm;		/**< Common metadata. */

	union {
		struct nss_ipsec_cmn_mdata_encap encap;	/**< Encapsulation metadata. */
		struct nss_ipsec_cmn_mdata_decap decap;	/**< Decapsulation metadata. */
	} data;						/**< Metadata payload. */
};

/**
 * nss_ipsec_cmn_stats_notification
 *	IPsec common transmission statistics structure.
 */
struct nss_ipsec_cmn_stats_notification {
	uint64_t stats_ctx[NSS_IPSEC_CMN_STATS_MAX];	/**< Context transmission statistics. */
	uint32_t core_id;				/**< Core ID. */
	uint32_t if_num;				/**< Interface number. */
};

/**
 * nss_ipsec_cmn_msg
 *	Message structure for NSS IPsec messages.
 */
struct nss_ipsec_cmn_msg {
	struct nss_cmn_msg cm;				/**< Common message header. */

	/**
	 * Payload of IPsec interface message.
	 */
	union {
		struct nss_ipsec_cmn_node node;		/**< Node configuration message. */
		struct nss_ipsec_cmn_ctx ctx;		/**< Context configuration message. */
		struct nss_ipsec_cmn_sa sa;		/**< SA configuration message. */
		struct nss_ipsec_cmn_flow flow;		/**< Flow configuration message. */
		struct nss_ipsec_cmn_sa_sync sa_sync;	/**< SA statistics message. */
		struct nss_ipsec_cmn_ctx_sync ctx_sync; /**< Context statistics message. */
	} msg;						/**< Message payload. */
};

/**
 * nss_ipsec_cmn_mdata_init
 *	Initialize the metadata common fields.
 *
 * @datatypes
 * nss_ipsec_cmn_mdata
 *
 * @param[in] mdata Metadata pointer.
 * @param[in] len   Metadata length including extra bytes.
 *
 * @return
 * Pointer to metadata payload.
 */
static inline void *nss_ipsec_cmn_mdata_init(struct nss_ipsec_cmn_mdata *mdata, uint16_t len)
{
	mdata->cm.len = len;
	mdata->cm.magic = NSS_IPSEC_CMN_MDATA_MAGIC;
	mdata->cm.version = NSS_IPSEC_CMN_MDATA_VERSION;
	mdata->cm.origin = NSS_IPSEC_CMN_MDATA_ORIGIN_HOST;

	return &mdata->data;
}

/**
 * Callback function for receiving message notifications.
 *
 * @datatypes
 * nss_ipsec_cmn_msg
 *
 * @param[in] app_data  Pointer to the application context of the message.
 * @param[in] msg       Pointer to the message data.
 */
typedef void (*nss_ipsec_cmn_msg_callback_t)(void *app_data, struct nss_cmn_msg *msg);

/**
 * Callback function for receiving data.
 *
 * @datatypes
 * net_device \n
 * sk_buff \n
 * napi_struct
 *
 * @param[in] netdev  Pointer to the associated network device.
 * @param[in] skb     Pointer to the message data.
 * @param[in] napi    Pointer to the NAPI structure.
 */
typedef void (*nss_ipsec_cmn_data_callback_t)(struct net_device *netdev, struct sk_buff *skb, struct napi_struct *napi);

/**
 * nss_ipsec_cmn_get_context
 *	Gets the NSS context for the IPsec handle.
 *
 * @return
 * Pointer to the NSS core context.
 */
extern struct nss_ctx_instance *nss_ipsec_cmn_get_context(void);

/**
 * nss_ipsec_cmn_get_ifnum_with_coreid
 *	Gets the IPsec interface number with a core ID.
 *
 * @param[in] ifnum  NSS interface number.
 *
 * @return
 * Interface number with the core ID.
 */
extern uint32_t nss_ipsec_cmn_get_ifnum_with_coreid(int32_t ifnum);

/**
 * nss_ipsec_cmn_unregister_if
 *	Deregisters an IPSEC tunnel interface from the NSS.
 *
 * @param[in] if_num  NSS interface number.
 *
 * @return
 * None.
 *
 * @dependencies
 * The tunnel interface must have been previously registered.
 *
 * @return
 * True if successful, else false.
 */
extern bool nss_ipsec_cmn_unregister_if(uint32_t if_num);

/**
 * nss_ipsec_cmn_register_if
 *	Registers the IPsec interface with the NSS for sending and
 *	receiving messages.
 *
 * @datatypes
 * nss_ipsec_cmn_data_callback_t \n
 * nss_ipsec_cmn_msg_callback_t \n
 * nss_dynamic_interface_type \n
 * net_device
 *
 * @param[in] if_num        NSS interface number.
 * @param[in] netdev        Pointer to the associated network device.
 * @param[in] cb_data       Callback for the data.
 * @param[in] cb_msg        Callback for the message.
 * @param[in] features      Socket buffer types supported by this interface.
 * @param[in] type          Dynamic interface type.
 * @param[in] app_data      Application context.
 *
 * @return
 * Pointer to the NSS core context.
 */
extern struct nss_ctx_instance *nss_ipsec_cmn_register_if(uint32_t if_num, struct net_device *netdev,
						nss_ipsec_cmn_data_callback_t cb_data,
						nss_ipsec_cmn_msg_callback_t cb_msg,
						uint32_t features, enum nss_dynamic_interface_type type, void *app_data);

/**
 * nss_ipsec_cmn_notify_unregister
 *	Deregisters the message notifier from the HLOS driver.
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
 * The message notifier must have been previously registered.
 */
extern void nss_ipsec_cmn_notify_unregister(struct nss_ctx_instance *ctx, uint32_t if_num);

/**
 * nss_ipsec_cmn_notify_register
 *	Registers an event callback to handle notifications from the IPsec firmware package.
 *
 * @datatypes
 * nss_ipsec_cmn_msg_callback_t \n
 *
 * @param[in] ifnum     NSS interface number.
 * @param[in] cb        Callback for IPsec message.
 * @param[in] app_data  Pointer to the application context.
 *
 * @return
 * Pointer to the NSS core context.
 */
extern struct nss_ctx_instance *nss_ipsec_cmn_notify_register(uint32_t ifnum, nss_ipsec_cmn_msg_callback_t cb, void *app_data);

/**
 * nss_ipsec_cmn_msg_init
 *	Initializes an IPsec message.
 *
 * @datatypes
 * nss_ipsec_cmn_msg \n
 * nss_ipsec_cmn_msg_type \n
 * nss_ipsec_cmn_msg_callback_t
 *
 * @param[in,out] nim       Pointer to the NSS interface message.
 * @param[in]     if_num    NSS interface number.
 * @param[in]     type      Type of message.
 * @param[in]     len       Size of the payload.
 * @param[in]     cb        Callback function for the message.
 * @param[in]     app_data  Pointer to the application context of the message.
 *
 * @return
 * None.
 */
extern void nss_ipsec_cmn_msg_init(struct nss_ipsec_cmn_msg *nim, uint16_t if_num, enum nss_ipsec_cmn_msg_type type,
				uint16_t len, nss_ipsec_cmn_msg_callback_t cb, void *app_data);

/**
 * nss_ipsec_cmn_tx_msg
 *	Sends an asynchronous IPsec message to the NSS.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_ipsec_cmn_msg
 *
 * @param[in] nss_ctx  Pointer to the NSS HLOS driver context.
 * @param[in] msg      Pointer to the message data.
 *
 * @return
 * Status of the Tx operation.
 */
extern nss_tx_status_t nss_ipsec_cmn_tx_msg(struct nss_ctx_instance *nss_ctx, struct nss_ipsec_cmn_msg *msg);

/**
 * nss_ipsec_cmn_tx_msg_sync
 *	Sends a synchronous IPsec message to the NSS.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_ipsec_cmn_msg_type \n
 * nss_ipsec_cmn_msg
 *
 * @param[in]  nss_ctx  Pointer to the NSS HLOS driver context.
 * @param[in]  if_num   NSS interface number.
 * @param[in]  type     Type of message.
 * @param[in]  len      Size of the payload.
 * @param[in]  nicm     Pointer to the NSS IPsec message.
 *
 * @return
 * Status of the Tx operation.
 */
extern nss_tx_status_t nss_ipsec_cmn_tx_msg_sync(struct nss_ctx_instance *nss_ctx, uint32_t if_num,
						enum nss_ipsec_cmn_msg_type type, uint16_t len,
						struct nss_ipsec_cmn_msg *nicm);

/**
 * nss_ipsec_cmn_tx_buf
 *	Sends a buffer to NSS for IPsec encapsulation or de-capsulation.
 *
 * @datatypes
 * sk_buff \n
 * nss_ctx_instance
 *
 * @param[in] nss_ctx Pointer to the NSS HLOS driver context.
 * @param[in] skb     Pointer to the message data.
 * @param[in] if_num  Pointer to the NSS interface number.
 *
 * @return
 * Status of the Tx operation.
 */
extern nss_tx_status_t nss_ipsec_cmn_tx_buf(struct nss_ctx_instance *nss_ctx, struct sk_buff *skb, uint32_t if_num);

/**
 * nss_ipsec_cmn_ppe_port_config
 *	Configure Packet Processing Engine IPsec port.
 *
 * @datatypes
 * nss_ctx_instance \n
 * net_device
 *
 * @param[in] ctx     Pointer to the context of the HLOS driver.
 * @param[in] netdev  Pointer to the associated network device.
 * @param[in] if_num  Data interface number.
 * @param[in] vsi_num Virtual switch instance number.
 *
 * @return
 * True if successful, else false.
 */
extern bool nss_ipsec_cmn_ppe_port_config(struct nss_ctx_instance *ctx, struct net_device *netdev,
					uint32_t if_num, uint32_t vsi_num);

/**
 * nss_ipsec_cmn_ppe_mtu_update()
 *	Configure Packet Processing Engine MTU for IPsec inline.
 *
 * @datatypes
 * nss_ctx_instance
 *
 * @param[in] ctx     Pointer to the context of the HLOS driver.
 * @param[in] if_num  Data interface number.
 * @param[in] mtu     Maximum transmission unit of interface number.
 * @param[in] mru     Maximum receive unit of interface number.
 *
 * @return
 * True if successful, else false.
 */
bool nss_ipsec_cmn_ppe_mtu_update(struct nss_ctx_instance *ctx, uint32_t if_num, uint16_t mtu, uint16_t mru);

/**
 * nss_ipsec_cmn_stats_unregister_notifier
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
extern int nss_ipsec_cmn_stats_unregister_notifier(struct notifier_block *nb);

/**
 * nss_ipsec_cmn_stats_register_notifier
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
extern int nss_ipsec_cmn_stats_register_notifier(struct notifier_block *nb);

/**
 * @}
 */

#endif /* !__NSS_IPSEC_CMN_H */

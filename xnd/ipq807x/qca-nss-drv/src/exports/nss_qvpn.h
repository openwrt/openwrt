/*
 **************************************************************************
 * Copyright (c) 2019, 2021, The Linux Foundation. All rights reserved.
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
 * @file nss_qvpn.h
 *	NSS QVPN interface definitions.
 */

#ifndef _NSS_QVPN_H_
#define _NSS_QVPN_H_

/**
 * @addtogroup nss_qvpn_subsystem
 * @{
 */

#define NSS_QVPN_CMDS_MAX 10			/**< Maximum number of QVPN commands supported. */
#define NSS_QVPN_VPN_HDR_HEAD_SIZE_MAX 64	/**< Maximum size of QVPN header. */
#define NSS_QVPN_VPN_HDR_TAIL_SIZE_MAX 32	/**< Maximum size of QVPN tail. */
#define NSS_QVPN_IV_SIZE_MAX 16			/**< Maximum size of IV supported. */
#define NSS_QVPN_SESS_ID_SIZE_MAX 8		/**< Maximum size of session ID. */

/*
 * QVPN L3/L4 header flags.
 */
#define NSS_QVPN_HDR_FLAG_IPV6   0x0001		/**< Outer L3 header is IPv6. */
#define NSS_QVPN_HDR_FLAG_L4_UDP 0x0002		/**< L4 is UDP. */

/**
 * nss_qvpn_msg_type
 *	Message types for QVPN NSS firmware.
 */
enum nss_qvpn_msg_type {
	NSS_QVPN_MSG_TYPE_TUNNEL_CONFIGURE,	/**< Configure QVPN tunnel instance. */
	NSS_QVPN_MSG_TYPE_TUNNEL_DECONFIGURE,	/**< Deconfigure QVPN tunnel instance. */
	NSS_QVPN_MSG_TYPE_CRYPTO_KEY_ADD,	/**< Add a new crypto key. */
	NSS_QVPN_MSG_TYPE_CRYPTO_KEY_DEL,	/**< Delete crypto key. */
	NSS_QVPN_MSG_TYPE_CRYPTO_KEY_ACTIVATE,	/**< Activate crypto key. */
	NSS_QVPN_MSG_TYPE_CRYPTO_KEY_DEACTIVATE,/**< Deactivate crypto key. */
	NSS_QVPN_MSG_TYPE_SYNC_STATS,		/**< Statistics synchronization. */
	NSS_QVPN_MSG_TYPE_MAX			/**< Maximum QVPN message type. */
};

/**
 * nss_qvpn_cmds_type
 *	Processing commands for QVPN.
 */
enum nss_qvpn_cmds_type {
	NSS_QVPN_CMDS_TYPE_NONE,		/**< Add VPN header to packet. */
	NSS_QVPN_CMDS_TYPE_ADD_VPN_HDR,		/**< Add VPN header to packet. */
	NSS_QVPN_CMDS_TYPE_REMOVE_VPN_HDR,	/**< Remove VPN header from packet. */
	NSS_QVPN_CMDS_TYPE_ADD_L3_L4_HDR,	/**< Add L3/L4 header to packet. */
	NSS_QVPN_CMDS_TYPE_REMOVE_L3_L4_HDR,	/**< Remove L3/L4 header from packet. */
	NSS_QVPN_CMDS_TYPE_ENCRYPT,		/**< Send packet for encryption. */
	NSS_QVPN_CMDS_TYPE_DECRYPT,		/**< Send packet for decryption. */
	NSS_QVPN_CMDS_TYPE_ANTI_REPLAY,		/**< Sequence number processing. */
	NSS_QVPN_CMDS_TYPE_MAX			/**< Maximum command supported. */
};

/**
 * nss_qvpn_profile
 *	QVPN profiles supported.
 */
enum nss_qvpn_profile {
	NSS_QVPN_PROFILE_NONE,		/**< No profile supported. */
	NSS_QVPN_PROFILE_CRYPTO_ENCAP,	/**< Encapsulation profile with crypto enabled. */
	NSS_QVPN_PROFILE_CRYPTO_DECAP,	/**< Decapsulation profile with crypto enabled. */
	NSS_QVPN_PROFILE_ENCAP,		/**< Encapsulation Profile with crypto disabled. */
	NSS_QVPN_PROFILE_DECAP,		/**< Decapsulation Profile with crypto disabled. */
	NSS_QVPN_PROFILE_MAX,		/**< Maximum profile. */
};

/**
 * nss_qvpn_pkt_drop_event
 *	Packets drop statistics from QVPN node.
 */
enum nss_qvpn_pkt_drop_event {
	NSS_QVPN_PKT_DROP_EVENT_NO_TAILROOM,		/**< Packet tail room not enough to copy HMAC to tail. */
	NSS_QVPN_PKT_DROP_EVENT_NO_HEADROOM,		/**< Packet head room not enough to add QVPN headers. */
	NSS_QVPN_PKT_DROP_EVENT_CBUF_ALLOC_FAIL,	/**< Received packet dropped as crypto buffer allocation failed. */
	NSS_QVPN_PKT_DROP_EVENT_PBUF_ALLOC_FAIL,	/**< Received packet dropped as associated pbuf allocation failed. */
	NSS_QVPN_PKT_DROP_EVENT_SYNC_ALLOC_FAIL,	/**< Pbuf dropped while doing statistics synchronization. */
	NSS_QVPN_PKT_DROP_EVENT_PBUF_UNALIGN,		/**< Received packet dropped as unaligned buffer. */
	NSS_QVPN_PKT_DROP_EVENT_CRYPTO_ENQ_FAIL,	/**< Received packet dropped as crypto enqueue failed. */
	NSS_QVPN_PKT_DROP_EVENT_LINEAR_COPY_FAIL,	/**< Received packet dropped as scatter-gather linear copy failed. */
	NSS_QVPN_PKT_DROP_EVENT_FWD_ENQ_FAIL,		/**< Received packet dropped as enqueue to next node failed. */
	NSS_QVPN_PKT_DROP_EVENT_POST_CRYPTO_Q_FULL,	/**< Post crypto queue is full dropping pbuf. */
	NSS_QVPN_PKT_DROP_EVENT_NODE_INACTIVE,		/**< Node is inactive dropping crypto processed packet. */
	NSS_QVPN_PKT_DROP_EVENT_NON_CRYPTO_PB,		/**< Non crypto processed packet enqueued to post crypto queue. */
	NSS_QVPN_PKT_DROP_EVENT_PAD_INVALID,		/**< Packet received with invalid padding. */
	NSS_QVPN_PKT_DROP_EVENT_BLK_UNALIGNED,		/**< Received pbuf length is not cipher block aligned. */
	NSS_QVPN_PKT_DROP_EVENT_MAX			/**< End of packet drop event list. */
};

/**
 * nss_qvpn_exception_event
 *	Exception events from QVPN node.
 */
enum nss_qvpn_exception_event {
	NSS_QVPN_EXCEPTION_EVENT_RX_CONTROL_PKT,	/**< QVPN control packet received. */
	NSS_QVPN_EXCEPTION_EVENT_RX_TAIL_NOSUPP,	/**< Protocol with tail not supported. */
	QVPN_TUN_EXCEPTION_EVENT_RX__HR_INSUFF,		/**< Insufficient headroom. */
	NSS_QVPN_EXCEPTION_EVENT_RX_SESS_ID_INVALID,	/**< Invalid session ID. */
	NSS_QVPN_EXCEPTION_EVENT_RX_DATA_PKT,		/**< Data packets exceptioned to host. */
	NSS_QVPN_EXCEPTION_EVENT_RX_MALFORMED,		/**< Malformed packet received. */
	NSS_QVPN_EXCEPTION_EVENT_MAX			/**< End of exception event list. */
};

/**
 * nss_qvpn_error_type
 *	Error types for the QVPN interface.
 */
enum nss_qvpn_error_type {
	NSS_QVPN_ERROR_TYPE_NONE,			/**< No error. */
	NSS_QVPN_ERROR_TYPE_UNKNOWN_MSG,		/**< Unknown message. */
	NSS_QVPN_ERROR_TUN_ALREADY_CONFIGURED,		/**< Tunnel already configured. */
	NSS_QVPN_ERROR_TYPE_IF_INVALID,			/**< Invalid interface. */
	NSS_QVPN_ERROR_TYPE_SIBLING_IF,			/**< Invalid sibling interface number. */
	NSS_QVPN_ERROR_TYPE_IV_SIZE_INVALID,		/**< Invalid IV size. */
	NSS_QVPN_ERROR_TYPE_HMAC_SIZE_INVALID,		/**< Invalid HMAC size. */
	NSS_QVPN_ERROR_TYPE_CRYPTO_BLK_SIZE_INVALID,	/**< Invalid crypto block size. */
	NSS_QVPN_ERROR_TYPE_SESSION_IDX_SIZE_INVALID,	/**< Invalid session index size. */
	NSS_QVPN_ERROR_TYPE_CMD_NOT_SUPPORTED,		/**< Command not supported. */
	NSS_QVPN_ERROR_TYPE_L4_PROTO_INVALID,		/**< L4 protocol encapsulation is not supported. */
	NSS_QVPN_ERROR_TYPE_SIBLING_IF_TYPE,		/**< Invalid sibling interface type. */
	NSS_QVPN_ERROR_TYPE_CMDS_COUNT_INVALID,		/**< Total number of commands is invalid. */
	NSS_QVPN_ERROR_TYPE_ENTRY_NOT_FOUND,		/**< Entry not found. */
	NSS_QVPN_ERROR_TYPE_ENTRY_NOT_ACTIVE,		/**< Entry not active. */
	NSS_QVPN_ERROR_TYPE_ENTRY_ALREADY_ACTIVE,	/**< Entry already active. */
	NSS_QVPN_ERROR_TYPE_CRYPTO_IDX_MISMATCH,	/**< Invalid crypto index. */
	NSS_QVPN_ERROR_TYPE_KI_ALLOC_FAIL,		/**< Key information allocation failure. */
	NSS_QVPN_ERROR_TYPE_PROFILE_INVALID,		/**< Invalid command profile. */
	NSS_QVPN_ERROR_TYPE_RX_TAIL_NOSUPP,		/**< VPN with tail not supported. */
	NSS_QVPN_ERROR_TYPE_MAX				/**< End of error list. */
};

/**
 * nss_qvpn_iv_type
 *	IV type for generating and copying in packet.
 */
enum nss_qvpn_iv_type {
	NSS_QVPN_IV_TYPE_NONE,		/**< No IV. */
	NSS_QVPN_IV_TYPE_STATIC,	/**< Use static IV configured. */
	NSS_QVPN_IV_TYPE_DYNAMIC_RAND,	/**< Generate IV randomly. */
	NSS_QVPN_IV_TYPE_MAX		/**< End of IV type list. */
};

/**
 * nss_qvpn_pad_type
 *	Pad type for generating and copying in packet.
 */
enum nss_qvpn_pad_type {
	NSS_QVPN_PAD_TYPE_NONE,		/**< No padding. */
	NSS_QVPN_PAD_TYPE_PKCS7,	/**< Generate pad buffer using PKCS7. */
	NSS_QVPN_PAD_TYPE_INC,		/**< Generate pad buffer monotonically increasing sequence. */
	NSS_QVPN_PAD_TYPE_MAX		/**< End of pad type. */
};

/**
 * nss_qvpn_anti_replay_alg
 *	Anti-replay algorithms supported.
 */
enum nss_qvpn_anti_replay_alg {
	NSS_QVPN_ANTI_REPLAY_ALG_NONE,			/**< No anti-replay. */
	NSS_QVPN_ANTI_REPLAY_ALG_REPLAY_WINDOW,		/**< Generate pad buffer monotonically increasing sequence. */
	NSS_QVPN_ANTI_REPLAY_ALG_MAX			/**< End of anti-replay algorithm. */
};

/**
 * nss_qvpn_crypto_mode
 *	Crypto modes supported.
 */
enum nss_qvpn_crypto_mode {
	NSS_QVPN_CRYPTO_MODE_NONE,		/**< NULL cipher and NULL authentication. */
	NSS_QVPN_CRYPTO_MODE_ENC,		/**< Encryption only. */
	NSS_QVPN_CRYPTO_MODE_DEC,		/**< Decryption only. */
	NSS_QVPN_CRYPTO_MODE_AUTH,		/**< Authentication only. */
	NSS_QVPN_CRYPTO_MODE_ENC_AUTH,		/**< Encryption and then authentication. */
	NSS_QVPN_CRYPTO_MODE_AUTH_DEC,		/**< Authentication and then decryption. */
	NSS_QVPN_CRYPTO_MODE_AUTH_ENC,		/**< Authentication and then encryption. */
	NSS_QVPN_CRYPTO_MODE_DEC_AUTH,		/**< Decryption and then authentication. */
	NSS_QVPN_CRYPTO_MODE_MAX		/**< End of crypto mode. */
};

/**
 * nss_qvpn_hdr_configure_msg
 *	QVPN header configuration.
 */
struct nss_qvpn_hdr_configure_msg {
	uint32_t src_ip[4];					/**< Source IP address. */
	uint32_t dst_ip[4];					/**< Destination IP address. */
	uint16_t src_port;					/**< Source port. */
	uint16_t dst_port;					/**< Destination port. */
	uint16_t hdr_flags;					/**< Header flags. */
	uint16_t seqnum_size;					/**< Size of sequence number. */
	uint16_t seqnum_offset;					/**< Sequence number offset. */
	uint16_t anti_replay_alg;				/**< Anti-replay algorithm. */
	uint16_t session_id_size;				/**< Session ID size. */
	uint16_t session_id_offset;				/**< Session ID offset. */
	uint16_t vpn_hdr_head_size;				/**< VPN header size. */
	uint16_t vpn_hdr_head_offset;				/**< VPN header offset. */
	uint16_t vpn_hdr_tail_size;				/**< Size of tail. */
	uint16_t res;						/**< Reserved for alignment. */
	uint8_t vpn_hdr_head[NSS_QVPN_VPN_HDR_HEAD_SIZE_MAX];	/**< Content of VPN header. */
	uint8_t vpn_hdr_tail[NSS_QVPN_VPN_HDR_TAIL_SIZE_MAX];	/**< VPN header tail content. */
	uint8_t hop_limit;					/**< TTL or hop limit. */
	uint8_t res1[3];					/**< Reserved for alignment. */
};

/**
 * nss_qvpn_crypto_configure_msg
 *	QVPN crypto configuration message.
 */
struct nss_qvpn_crypto_configure_msg {
	uint16_t hmac_len;			/**< Length of HMAC to copy. */
	uint16_t hmac_offset;			/**< Offset to copy HMAC. */
	uint16_t auth_offset;			/**< Data offset to start authentication. */
	uint16_t cipher_op_offset;		/**< Start of cipher data. */
	uint16_t cipher_blk_size;		/**< Cipher block size. */
	uint16_t pad_type;			/**< Pad algorithm. */
	uint16_t crypto_mode;			/**< Crypto mode. */
	uint16_t iv_len;			/**< Length of IV. */
	uint16_t iv_offset;			/**< IV offset. */
	uint16_t iv_type;			/**< IV generation algorithm. */
	uint8_t iv_val[NSS_QVPN_IV_SIZE_MAX];	/**< IV to be used. */
};

/**
 * nss_qvpn_crypto_key_add_msg
 *	QVPN key add message.
 */
struct nss_qvpn_crypto_key_add_msg {
	uint32_t crypto_idx;				/**< Crypto session ID. */
	uint8_t session_id[NSS_QVPN_SESS_ID_SIZE_MAX];	/**< Session ID. */
};

/**
 * nss_qvpn_crypto_key_del_msg
 *	Delete/Deactivate key message.
 */
struct nss_qvpn_crypto_key_del_msg {
	uint32_t crypto_idx;	/**< Crypto index to delete/deactivate. */
};

/**
 * nss_qvpn_tunnel_config_msg
 *	QVPN tunnel configure message.
 */
struct nss_qvpn_tunnel_config_msg {
	uint32_t sibling_if;					/**< Sibling interface number. */
	uint16_t total_cmds;					/**< Total number of commands. */
	uint16_t cmd_profile;					/**< Command processing profile. */
	uint16_t cmd[NSS_QVPN_CMDS_MAX];			/**< Commands to execute. */
	struct nss_qvpn_crypto_key_add_msg crypto_key;		/**< Initial crypto key. */
	struct nss_qvpn_hdr_configure_msg hdr_cfg;		/**< VPN header configuration. */
	struct nss_qvpn_crypto_configure_msg crypto_cfg;	/**< Crypto configuration. */
};

/**
 * nss_qvpn_crypto_key_activate_msg
 *	Activate key message.
 */
struct nss_qvpn_crypto_key_activate_msg {
	uint32_t crypto_idx;					/**< Crypto session ID. */
	uint8_t vpn_hdr_head[NSS_QVPN_VPN_HDR_HEAD_SIZE_MAX];	/**< Content of VPN header. */
};

/**
 * nss_qvpn_stats_sync_msg
 *	Message information for QVPN synchronization statistics.
 */
struct nss_qvpn_stats_sync_msg {
	struct nss_cmn_node_stats node_stats;				/**< Common node statistics. */
	uint32_t crypto_resp_error[NSS_CRYPTO_CMN_RESP_ERROR_MAX];	/**< Crypto response errors. */
	uint32_t pkt_drop_event[NSS_QVPN_PKT_DROP_EVENT_MAX];		/**< Packet drop events. */
	uint32_t exception_event[NSS_QVPN_EXCEPTION_EVENT_MAX];		/**< QVPN exception events. */
};

/**
 * nss_qvpn_stats_notification
 *	QVPN transmission statistics structure.
 */
struct nss_qvpn_stats_notification {
	uint64_t stats_ctx[NSS_STATS_NODE_MAX];		/**< Context transmission statistics. */
	uint32_t core_id;				/**< Core ID. */
	uint32_t if_num;				/**< Interface number. */
};

/**
 * nss_qvpn_msg
 *	QVPN message structure for configuration and statistics.
 */
struct nss_qvpn_msg {
	struct nss_cmn_msg cm;							/**< Common message header. */

	/**< QVPN configuration messages. */
	union {
		struct nss_qvpn_tunnel_config_msg tunnel_config;		/**< QVPN tunnel configure message. */
		struct nss_qvpn_crypto_key_add_msg key_add;			/**< Crypto key add message. */
		struct nss_qvpn_crypto_key_del_msg key_del;			/**< Crypto key delete message. */
		struct nss_qvpn_crypto_key_activate_msg key_activate;		/**< Crypto key active message. */
		struct nss_qvpn_stats_sync_msg stats;				/**< QVPN statistics synchronization message. */
	} msg;	/**< QVPN configuration message. */
};

/**
 * nss_qvpn_tx_msg
 *	Sends an QVPN message to the NSS.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_qvpn_msg
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 * @param[in] msg      Pointer to the message data.
 *
 * @return
 * Status of the Tx operation.
 */
nss_tx_status_t nss_qvpn_tx_msg(struct nss_ctx_instance *nss_ctx, struct nss_qvpn_msg *msg);

/**
 * nss_qvpn_tx_msg_sync
 *	Sends an QVPN message to the NSS synchronously.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_qvpn_msg \n
 * nss_qvpn_msg_type \n
 * nss_qvpn_error_type
 *
 * @param[in]     nss_ctx  Pointer to the NSS context.
 * @param[in]     nvm      Pointer to the message data.
 * @param[in]     if_num   NSS interface number.
 * @param[in]     type     Type of message.
 * @param[in]     len      Size of the payload.
 * @param[in,out] resp     Response for the configuration.
 *
 * @return
 * Status of the Tx operation.
 */
nss_tx_status_t nss_qvpn_tx_msg_sync(struct nss_ctx_instance *nss_ctx, struct nss_qvpn_msg *nvm,
		uint32_t if_num, enum nss_qvpn_msg_type type, uint16_t len,
		enum nss_qvpn_error_type *resp);

/**
 * nss_qvpn_tx_buf
 *	Sends data packet for QVPN encapsulation/decapsulation.
 *
 * @datatypes
 * nss_ctx_instance
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 * @param[in] if_num   NSS interface number.
 * @param[in] skb      Pointer to sk_buff.
 *
 * @return
 * Status of the Tx operation.
 */
nss_tx_status_t nss_qvpn_tx_buf(struct nss_ctx_instance *nss_ctx, uint32_t if_num, struct sk_buff *skb);

/**
 * nss_qvpn_msg_init
 *	Initializes an QVPN message.
 *
 * @datatypes
 * nss_qvpn_msg
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
void nss_qvpn_msg_init(struct nss_qvpn_msg *ncm, uint16_t if_num, uint32_t type,  uint32_t len, void *cb, void *app_data);

/**
 * nss_qvpn_get_context
 *	Gets the QVPN context used in nss_qvpn_tx.
 *
 * @return
 * Pointer to the NSS core context.
 */
struct nss_ctx_instance *nss_qvpn_get_context(void);

/**
 * Callback when QVPN data is received.
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
typedef void (*nss_qvpn_callback_t)(struct net_device *netdev, struct sk_buff *skb, struct napi_struct *napi);

/**
 * Callback to receive QVPN messages.
 *
 * @datatypes
 * nss_cmn_msg
 *
 * @param[in] app_data  Pointer to the application context of the message.
 * @param[in] msg       Pointer to the message data.
 */
typedef void (*nss_qvpn_msg_callback_t)(void *app_data, struct nss_cmn_msg *msg);

/**
 * nss_qvpn_unregister_if
 *	Deregisters the QVPN interface from the NSS.
 *
 * @param[in] if_num              NSS interface number.
 *
 * @return
 * None.
 */
void nss_qvpn_unregister_if(uint32_t if_num);

/**
 * nss_qvpn_register_if
 *	Register to send/receive QVPN messages to NSS.
 *
 * @datatypes
 * nss_qvpn_callback_t \n
 * net_device
 *
 * @param[in] if_num               NSS interface number.
 * @param[in] qvpn_data_callback   Callback for the data.
 * @param[in] qvpn_event_callback  Callback for receiving events.
 * @param[in] netdev               Pointer to the associated network device.
 * @param[in] features             Data socket buffer types supported by this interface.
 * @param[in] app_ctx              Pointer to the application context of the message.
 *
 * @return
 * Pointer to the NSS core context.
 */
struct nss_ctx_instance *nss_qvpn_register_if(uint32_t if_num, nss_qvpn_callback_t qvpn_data_callback,
			nss_qvpn_msg_callback_t qvpn_event_callback, struct net_device *netdev,
			uint32_t features, void *app_ctx);

/**
 * nss_qvpn_ifnum_with_core_id
 *	Gets the QVPN interface number with the core ID.
 *
 * @param[in] if_num  NSS interface number.
 *
 * @return
 * Interface number with the core ID.
 */
int nss_qvpn_ifnum_with_core_id(int if_num);

/**
 * nss_qvpn_register_handler
 *	Registers the QVPN handler with the NSS.
 *
 * @return
 * None.
 */
void nss_qvpn_register_handler(void);

/**
 * nss_qvpn_ifmap_get
 *	Returns active QVPN interfaces.
 *
 * @return
 * Pointer to interface map.
 */
unsigned long *nss_qvpn_ifmap_get(void);

/**
 * nss_qvpn_stats_unregister_notifier
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
extern int nss_qvpn_stats_unregister_notifier(struct notifier_block *nb);

/**
 * nss_qvpn_stats_register_notifier
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
extern int nss_qvpn_stats_register_notifier(struct notifier_block *nb);

/**
 * @}
 */

#endif /* _NSS_QVPN_H_ */

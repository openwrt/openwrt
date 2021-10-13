/*
 **************************************************************************
 * Copyright (c) 2014-2018, The Linux Foundation. All rights reserved.
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
 * @file nss_ipsec.h
 *	NSS IPSec interface definitions.
 */

#ifndef __NSS_IPSEC_H
#define __NSS_IPSEC_H

/*
 * For some reason Linux doesn't define this in if_arp.h,
 * refer http://www.iana.org/assignments/arp-parameters/arp-parameters.xhtml
 * for the full list
 */

/**
 * @addtogroup nss_ipsec_subsystem
 * @{
 */

#define NSS_IPSEC_ARPHRD_IPSEC 31
		/**< ARP (iana.org) hardware type for an IPsec tunnel. */
#define NSS_IPSEC_MAX_RULES 256
		/**< Maximum number of rules supported. */
#define NSS_IPSEC_MAX_SA NSS_CRYPTO_MAX_IDXS
		/**< Maximum number of SAs supported. */

#if (~(NSS_IPSEC_MAX_RULES - 1) & (NSS_IPSEC_MAX_RULES >> 1))
#error "NSS Max SA should be a power of 2"
#endif

/**
 * Size of an IPsec message.
 */
#define NSS_IPSEC_MSG_LEN (sizeof(struct nss_ipsec_msg) - sizeof(struct nss_cmn_msg))

/**
 * nss_ipsec_msg_type
 *	Rules for the IPsec interface.
 */
enum nss_ipsec_msg_type {
	NSS_IPSEC_MSG_TYPE_NONE = 0,
	NSS_IPSEC_MSG_TYPE_ADD_RULE = 1,
	NSS_IPSEC_MSG_TYPE_DEL_RULE = 2,
	NSS_IPSEC_MSG_TYPE_FLUSH_TUN = 3,
	NSS_IPSEC_MSG_TYPE_SYNC_SA_STATS = 4,
	NSS_IPSEC_MSG_TYPE_SYNC_FLOW_STATS = 5,
	NSS_IPSEC_MSG_TYPE_SYNC_NODE_STATS = 6,
	NSS_IPSEC_MSG_TYPE_CONFIGURE_NODE = 7,
	NSS_IPSEC_MSG_TYPE_MAX
};

/**
 * nss_ipsec_status
 *	Status types for the IPsec interface.
 */
typedef enum nss_ipsec_status {
	NSS_IPSEC_STATUS_OK = 0,
	NSS_IPSEC_STATUS_ENOMEM = 1,
	NSS_IPSEC_STATUS_ENOENT = 2,
	NSS_IPSEC_STATUS_MAX
} nss_ipsec_status_t;

/**
 * nss_ipsec_error_type
 *	Error types for the IPsec interface.
 */
enum nss_ipsec_error_type {
	NSS_IPSEC_ERROR_TYPE_NONE = 0,
	NSS_IPSEC_ERROR_TYPE_HASH_DUPLICATE = 1,
	NSS_IPSEC_ERROR_TYPE_HASH_COLLISION = 2,
	NSS_IPSEC_ERROR_TYPE_UNHANDLED_MSG = 3,
	NSS_IPSEC_ERROR_TYPE_INVALID_RULE = 4,
	NSS_IPSEC_ERROR_TYPE_MAX_SA = 5,
	NSS_IPSEC_ERROR_TYPE_MAX_FLOW = 6,
	NSS_IPSEC_ERROR_TYPE_INVALID_CINDEX = 7,
	NSS_IPSEC_ERROR_TYPE_INVALID_IPVER = 8,
	NSS_IPSEC_ERROR_TYPE_MAX
};

/**
 * nss_ipsec_type
 *	Operation types for the IPsec interface.
 */
enum nss_ipsec_type {
	NSS_IPSEC_TYPE_NONE = 0,
	NSS_IPSEC_TYPE_ENCAP = 1,
	NSS_IPSEC_TYPE_DECAP = 2,
	NSS_IPSEC_TYPE_MAX
};

/**
 * nss_ipsec_tuple
 *	Common IPsec rule selector tuple for encapsulation and decapsulation.
 *
 * This selector is used for preparing a lookup tuple for incoming packets.
 * The tuple is used to derive the index into the rule table.
 *
 * Choosing the selector fields depends on the IPsec encapsulation or decapsulation
 * package. The host has no understanding of the index derived from the selector fields,
 * and thus it provides information for all entries in the structure.
 *
 * The encapsulation and decapsulation packages return the index in their respective
 * tables to the host. The host stores the rule for future reference purposes.
 */
struct nss_ipsec_tuple {
	uint32_t dst_addr[4];		/**< Destination IP address. */
	uint32_t src_addr[4];		/**< Source IP address. */

	uint32_t esp_spi;		/**< SPI index. */

	uint16_t dst_port;		/**< Destination port (UDP or TCP). */
	uint16_t src_port;		/**< Source port (UDP or TCP). */

	uint8_t proto_next_hdr;		/**< IP header type. */
	uint8_t ip_ver;			/**< IP version. */
	uint8_t res[2];			/**< Reserved for 4-byte alignment. */
};

/**
 * nss_ipsec_rule_oip
 *	Common information about the IPsec rule outer IP header.
 */
struct nss_ipsec_rule_oip {
	uint32_t dst_addr[4];		/**< IPv4 destination address to apply. */
	uint32_t src_addr[4];		/**< IPv4 source address to apply. */

	uint32_t esp_spi;		/**< ESP SPI index to apply. */

	uint16_t dst_port;		/**< Destination port (UDP or TCP). */
	uint16_t src_port;		/**< Source port (UDP or TCP). */

	uint8_t ttl_hop_limit;		/**< IPv4 time-to-live value to apply. */
	uint8_t ip_ver;			/**< IP version. */
	uint8_t proto_next_hdr;         /**< IP header type. */
	uint8_t res;			/**< Reserved for 4-byte alignment. */
};

/**
 * nss_ipsec_rule_data
 *	IPsec rule data used for per-packet transformation.
 */
struct nss_ipsec_rule_data {

	uint16_t crypto_index;		/**< Crypto index for the security association. */
	uint16_t window_size;		/**< ESP sequence number window. */

	uint8_t cipher_blk_len;		/**< Size of the cipher block. */
	uint8_t iv_len;			/**< Size of the initialization vector. */
	uint8_t nat_t_req;		/**< NAT-T required. */
	uint8_t esp_icv_len;		/**< Size of the ICV to be produced as a result of authentication. */

	uint8_t esp_seq_skip;		/**< Skip an ESP sequence number. */
	uint8_t esp_tail_skip;		/**< Skip an ESP trailer. */
	uint8_t use_pattern;		/**< Use random pattern in a hash calculation. */
	uint8_t enable_esn;		/**< Enable extended sequence number. */

	uint8_t dscp;                   /**< Default DSCP value of the SA. */
	uint8_t df;			/**< Default dont fragment value of the SA. */
	uint8_t copy_dscp;		/**< The flag tells whether to copy DSCP from inner header. */
	uint8_t copy_df;		/**< The flag tells Whether to copy DF from inner header. */

	uint32_t res2[4];		/**< Reserved 16 bytes for future use. */
};

/**
 * nss_ipsec_rule
 *	Push message for IPsec rules.
 *
 * This message is sent from the host to the NSS for performing an operation
 * on NSS rule tables.
 */
struct nss_ipsec_rule {
	struct nss_ipsec_rule_oip oip;	/**< Per rule outer IP information. */
	struct nss_ipsec_rule_data data;/**< Per rule data. */

	uint32_t index;		/**< Index provided by the NSS. */
	uint32_t sa_idx;	/**< Rule index for the security association table. */
};

/**
 * nss_ipsec_configure_node
 *	Push message for setting IPsec inline mode and initializing DMA rings.
 */
struct nss_ipsec_configure_node {
	bool dma_redirect;	/**< Program redirect DMA ring. */
	bool dma_lookaside;	/**< Program lookaside DMA ring. */
};

/**
 * nss_ipsec_sa_stats
 *	Packet statistics per security association.
 */
struct nss_ipsec_sa_stats {
	uint32_t count;			/**< Packets processed. */
	uint32_t bytes;			/**< Bytes processed. */
	uint32_t no_headroom;		/**< Insufficient headroom. */
	uint32_t no_tailroom;		/**< Insufficient tailroom. */
	uint32_t no_resource;		/**< No crypto buffer. */
	uint32_t fail_queue;		/**< Failed to enqueue. */
	uint32_t fail_hash;		/**< Hash mismatch. */
	uint32_t fail_replay;		/**< Replay check failure. */
	uint64_t seq_num;		/**< Current sequence number. */
	uint64_t window_max;		/**< Maximum size of the window. */
	uint32_t window_size;		/**< Current window size. */
	uint32_t fail_hash_cont;	/**< Consecutive hash fail count. */
	uint8_t esn_enabled;		/**< Indicates whether ESN is enabled. */
	uint8_t res[3];			/**< Reserved for future use. */
} /** @cond */ __attribute__((packed))/** @endcond */;

/**
 * nss_ipsec_flow_stats
 *	Per-flow statistics.
 */
struct nss_ipsec_flow_stats {
	uint32_t processed;		/**< Packets processed for this flow. */

	uint8_t use_pattern;		/**< Use random pattern. */
	uint8_t res[3];			/**< Reserved for 4-byte alignment padding. */
};

/**
 * nss_ipsec_node_stats
 *	Per-node statistics.
 */
struct nss_ipsec_node_stats {
	uint32_t enqueued;		/**< Packets enqueued to the node. */
	uint32_t completed;		/**< Packets processed by the node. */
	uint32_t linearized;		/**< Packet is linear. */
	uint32_t exceptioned;		/**< Packets exception from the NSS. */
	uint32_t fail_enqueue;		/**< Packets failed to enqueue. */
	uint32_t redir_rx;		/**< Packets received in redirect ring. */
	uint32_t fail_redir;		/**< Packets dropped in redirect ring. */
};

/**
 * nss_ipsec_stats
 *	Common statistics structure.
 */
union nss_ipsec_stats {
	struct nss_ipsec_sa_stats sa;		/**< Security association statistics. */
	struct nss_ipsec_flow_stats flow;	/**< Flow statistics. */
	struct nss_ipsec_node_stats node;	/**< Node statistics. */
};

/**
 * nss_ipsec_msg
 *	Data for sending and receiving IPsec messages.
 */
struct nss_ipsec_msg {
	struct nss_cmn_msg cm;		/**< Common message header. */

	uint32_t tunnel_id;		/**< ID of the tunnel associated with the message. */
	struct nss_ipsec_tuple tuple;
			/**< Tuple to look up the SA table for encapsulation or decapsulation. */
	enum nss_ipsec_type type;	/**< Encapsulation or decapsulation operation. */

	/**
	 * Payload of an IPsec message.
	 */
	union {
		struct nss_ipsec_rule rule;
				/**< IPsec rule message. */
		struct nss_ipsec_configure_node node;
				/**< IPsec node message. */
		union nss_ipsec_stats stats;
				/**< Retrieve statistics for the tunnel. */
	} msg;			/**< Message payload. */
};

/**
 * Callback function for receiving message notifications.
 *
 * @datatypes
 * nss_ipsec_msg
 *
 * @param[in] app_data  Pointer to the application context of the message.
 * @param[in] msg       Pointer to the message data.
 */
typedef void (*nss_ipsec_msg_callback_t)(void *app_data, struct nss_ipsec_msg *msg);

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
typedef void (*nss_ipsec_buf_callback_t)(struct net_device *netdev, struct sk_buff *skb, struct napi_struct *napi);

/**
 * nss_ipsec_tx_msg
 *	Sends an IPsec message to the NSS HLOS driver.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_ipsec_msg
 *
 * @param[in] nss_ctx  Pointer to the NSS HLOS driver context.
 * @param[in] msg      Pointer to the message data.
 *
 * @return
 * Status of the Tx operation.
 */
extern nss_tx_status_t nss_ipsec_tx_msg(struct nss_ctx_instance *nss_ctx, struct nss_ipsec_msg *msg);

/**
 * nss_ipsec_tx_msg_sync
 *	Sends IPsec messages synchronously.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_ipsec_msg_type \n
 * nss_ipsec_msg \n
 * nss_ipsec_error_type
 *
 * @param[in]     nss_ctx  Pointer to the NSS context.
 * @param[in]     if_num   Configuration interface number.
 * @param[in]     type     Type of the message.
 * @param[in]     len      Size of the payload.
 * @param[in]     nim      Pointer to the message data.
 * @param[in,out] resp     Response for the configuration.
 *
 * @return
 * Status of the Tx operation.
 */
extern nss_tx_status_t nss_ipsec_tx_msg_sync(struct nss_ctx_instance *nss_ctx, uint32_t if_num,
						enum nss_ipsec_msg_type type, uint16_t len,
						struct nss_ipsec_msg *nim, enum nss_ipsec_error_type *resp);

/**
 * nss_ipsec_tx_buf
 *	Sends a plain text packet to NSS for IPsec encapsulation or decapsulation.
 *
 * @datatypes
 * sk_buff
 *
 * @param[in] skb     Pointer to the message data.
 * @param[in] if_num  Pointer to the NSS interface number.
 *
 * @return
 * Status of the Tx operation.
 */
extern nss_tx_status_t nss_ipsec_tx_buf(struct sk_buff *skb, uint32_t if_num);

/**
 * nss_ipsec_notify_register
 *	Registers an event callback handler with the HLOS driver.
 *
 * When registered, the message callback is called when the NSS
 * sends a response to the message sent by the host.
 *
 * @datatypes
 * nss_ipsec_msg_callback_t
 *
 * @param[in] if_num    NSS interface number.
 * @param[in] cb        Callback function for the message.
 * @param[in] app_data  Pointer to the context of the message.
 *
 * @return
 * Pointer to the NSS core context.
 */
extern struct nss_ctx_instance *nss_ipsec_notify_register(uint32_t if_num, nss_ipsec_msg_callback_t cb, void *app_data);

/**
 * nss_ipsec_data_register
 *	Registers a data callback handler with the HLOS driver.
 *
 * The HLOS driver calls the registered data callback to return
 * the packet to the OS.
 *
 * @datatypes
 * nss_ipsec_buf_callback_t \n
 * net_device
 *
 * @param[in] if_num    NSS interface number.
 * @param[in] cb        Callback function for the data.
 * @param[in] netdev    Pointer to the associated network device.
 * @param[in] features  Data socket buffer types supported by this interface.
 *
 * @return
 * Pointer to the NSS core context.
 */
extern struct nss_ctx_instance *nss_ipsec_data_register(uint32_t if_num, nss_ipsec_buf_callback_t cb, struct net_device *netdev, uint32_t features);

/**
 * nss_ipsec_notify_unregister
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
extern void nss_ipsec_notify_unregister(struct nss_ctx_instance *ctx, uint32_t if_num);

/**
 * nss_ipsec_data_unregister
 *	Deregisters the data notifier from the HLOS driver.
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
 * The data notifier must have been previously registered.
 */
extern void nss_ipsec_data_unregister(struct nss_ctx_instance *ctx, uint32_t if_num);

/**
 * nss_ipsec_get_context
 *	Gets the NSS context for the IPsec handle.
 *
 * @return
 * Pointer to the NSS core context.
 */
extern struct nss_ctx_instance *nss_ipsec_get_context(void);

/**
 * nss_ipsec_get_ifnum
 *	Gets the IPsec interface number with a core ID.
 *
 * @param[in] if_num  NSS interface number.
 *
 * @return
 * Interface number with the core ID.
 */
extern int32_t nss_ipsec_get_ifnum(int32_t if_num);

/**
 * nss_ipsec_msg_init
 *	Initializes an IPsec message.
 *
 * @datatypes
 * nss_ipsec_msg \n
 * nss_ipsec_msg_callback_t
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
extern void nss_ipsec_msg_init(struct nss_ipsec_msg *nim, uint16_t if_num, uint32_t type, uint32_t len,
				nss_ipsec_msg_callback_t cb, void *app_data);

/**
 * nss_ipsec_get_encap_interface
 *	Gets the NSS interface number to be used for IPsec encapsulation message.
 *
 * @return
 * Encapsulation interface number.
 */
extern int32_t nss_ipsec_get_encap_interface(void);

/**
 * nss_ipsec_get_decap_interface
 *	Gets the NSS interface number to be used for an IPsec decapsulation message.
 *
 * @return
 * Decapsulation interface number.
 */
extern int32_t nss_ipsec_get_decap_interface(void);

/**
 * nss_ipsec_get_data_interface
 *	Gets the NSS interface number to be used for an IPsec data transfer.
 *
 * @return
 * NSS interface number.
 */
extern int32_t nss_ipsec_get_data_interface(void);

/**
 * nss_ipsec_ppe_port_config
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
extern bool nss_ipsec_ppe_port_config(struct nss_ctx_instance *ctx, struct net_device *netdev,
				uint32_t if_num, uint32_t vsi_num);

/**
 * nss_ipsec_ppe_mtu_update()
 *	Configure Packet Processing Engine MTU for IPsec in-line.
 *
 * @datatypes
 * nss_ctx_instance \n
 *
 * @param[in] ctx     Pointer to the context of the HLOS driver.
 * @param[in] if_num  Data interface number.
 * @param[in] mtu     Maximum transmission unit of Interface number.
 * @param[in] mru     Maximum Receive unit of Interface number.
 *
 * @return
 * True if successful, else false.
 */
bool nss_ipsec_ppe_mtu_update(struct nss_ctx_instance *ctx, uint32_t if_num, uint16_t mtu, uint16_t mru);

/**
 * @}
 */

#endif /* __NSS_IPSEC_H */

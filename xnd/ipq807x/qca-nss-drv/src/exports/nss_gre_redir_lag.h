/*
 **************************************************************************
 * Copyright (c) 2018, 2021, The Linux Foundation. All rights reserved.
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
 * @file nss_gre_redir_lag.h
 *	NSS GRE redirect LAG interface definitions.
 */

#ifndef __NSS_GRE_REDIR_LAG_H
#define __NSS_GRE_REDIR_LAG_H

/**
 * @addtogroup nss_gre_redirect_subsystem
 * @{
 */

#define NSS_GRE_REDIR_LAG_MAX_NODE 12			/**< Maximum number of LAG nodes. */
#define NSS_GRE_REDIR_LAG_MAX_SLAVE 8			/**< Maximum number of GRE redirect nodes per LAG node. */
#define NSS_GRE_REDIR_LAG_MIN_SLAVE 2			/**< Minimum required GRE redirect nodes per LAG node. */
#define NSS_GRE_REDIR_LAG_US_STATS_SYNC_RETRY 3		/**< Number of retries for sending query hash messages. */
#define NSS_GRE_REDIR_LAG_US_MAX_HASH_PER_MSG 80	/**< Maximum hash entries per message. */

/*
 * nss_gre_redir_lag_err_types
 *	GRE redirect LAG error types.
 */
enum nss_gre_redir_lag_err_types {
	NSS_GRE_REDIR_LAG_SUCCESS,
	NSS_GRE_REDIR_LAG_ERR_INCORRECT_IFNUM,
	NSS_GRE_REDIR_LAG_ERR_CORE_UNREGISTER_FAILED,
	NSS_GRE_REDIR_LAG_ERR_STATS_INDEX_NOT_FOUND,
	NSS_GRE_REDIR_LAG_ERR_DEALLOC_FAILED,
	NSS_GRE_REDIR_LAG_ERR_MAX,
};

/**
 * nss_gre_redir_lag_us_message_types
 *	GRE redirect LAG upstream message types.
 */
enum nss_gre_redir_lag_us_message_types {
	NSS_GRE_REDIR_LAG_US_CONFIG_MSG,
	NSS_GRE_REDIR_LAG_US_ADD_HASH_NODE_MSG,
	NSS_GRE_REDIR_LAG_US_DEL_HASH_NODE_MSG,
	NSS_GRE_REDIR_LAG_US_QUERY_HASH_NODE_MSG,
	NSS_GRE_REDIR_LAG_US_CMN_STATS_SYNC_MSG,
	NSS_GRE_REDIR_LAG_US_DB_HASH_NODE_MSG,
	NSS_GRE_REDIR_LAG_US_MAX_MSG_TYPES,
};

/**
 * nss_gre_redir_lag_ds_message_types
 *	GRE redirect LAG downstream message types.
 */
enum nss_gre_redir_lag_ds_message_types {
	NSS_GRE_REDIR_LAG_DS_ADD_STA_MSG,
	NSS_GRE_REDIR_LAG_DS_DEL_STA_MSG,
	NSS_GRE_REDIR_LAG_DS_UPDATE_STA_MSG,
	NSS_GRE_REDIR_LAG_DS_STATS_SYNC_MSG,
	NSS_GRE_REDIR_LAG_DS_MAX_MSG_TYPES,
};

/**
 * nss_gre_redir_lag_ds_stats_types
 *	GRE redirect LAG downstream statistics.
 */
enum nss_gre_redir_lag_ds_stats_types {
	NSS_GRE_REDIR_LAG_DS_STATS_DST_INVALID = NSS_STATS_NODE_MAX,
							/**< Packets that do not have a valid destination. */
	NSS_GRE_REDIR_LAG_DS_STATS_EXCEPTION_PKT,	/**< Packets that are exceptioned to host. */
	NSS_GRE_REDIR_LAG_DS_STATS_MAX,			/**< Maximum statistics type. */
};

/**
 * nss_gre_redir_lag_us_stats_types
 *	GRE redirect LAG upstream statistics.
 */
enum nss_gre_redir_lag_us_stats_types {
	NSS_GRE_REDIR_LAG_US_STATS_AMSDU_PKTS = NSS_STATS_NODE_MAX,
							/**< Number of AMSDU packets seen. */
	NSS_GRE_REDIR_LAG_US_STATS_AMSDU_PKTS_ENQUEUED,	/**< Number of AMSDU packets enqueued. */
	NSS_GRE_REDIR_LAG_US_STATS_AMSDU_PKTS_EXCEPTIONED,
							/**< Number of AMSDU packets exceptioned. */
	NSS_GRE_REDIR_LAG_US_STATS_EXCEPTIONED,		/**< Number of exceptioned packets. */
	NSS_GRE_REDIR_LAG_US_STATS_FREED,		/**< Freed packets when equeue to NSS to host fails. */
	NSS_GRE_REDIR_LAG_US_STATS_ADD_ATTEMPT,		/**< Add hash attempts. */
	NSS_GRE_REDIR_LAG_US_STATS_ADD_SUCCESS,		/**< Add hash success. */
	NSS_GRE_REDIR_LAG_US_STATS_ADD_FAIL_TABLE_FULL,	/**< Add hash failed due to full table. */
	NSS_GRE_REDIR_LAG_US_STATS_ADD_FAIL_EXISTS,	/**< Add hash failed as entry already exists. */
	NSS_GRE_REDIR_LAG_US_STATS_DEL_ATTEMPT,		/**< Delete hash attempts. */
	NSS_GRE_REDIR_LAG_US_STATS_DEL_SUCCESS,		/**< Delete hash success. */
	NSS_GRE_REDIR_LAG_US_STATS_DEL_FAIL_NOT_FOUND,	/**< Delete hash failed as entry not found in hash table. */
	NSS_GRE_REDIR_LAG_US_STATS_MAX,			/**< Maximum statistics type. */
};

/**
 * nss_gre_redir_lag_us_hash_mode
 *	GRE redirect LAG upstream hash modes.
 */
enum nss_gre_redir_lag_us_hash_mode {
	NSS_GRE_REDIR_LAG_US_HASH_MODE_SRC_AND_DEST,
	NSS_GRE_REDIR_LAG_US_HASH_MODE_SRC,
	NSS_GRE_REDIR_LAG_US_HASH_MODE_DEST,
	NSS_GRE_REDIR_LAG_US_HASH_MODE_MAX,
};

/**
 * nss_gre_redir_lag_us_config_msg
 *	Upstream configure message.
 */
struct nss_gre_redir_lag_us_config_msg {
	uint32_t hash_mode;					/**< Hash operating mode. */
	uint32_t num_slaves;					/**< Number of slaves. */
	uint32_t if_num[NSS_GRE_REDIR_LAG_MAX_SLAVE];		/**< NSS interface numbers of GRE redirect tunnels. */
};

/**
 * nss_gre_redir_lag_us_add_hash_node_msg
 *	Message to add hash entry.
 */
struct nss_gre_redir_lag_us_add_hash_node_msg {
	uint32_t if_num;			/**< NSS interface number of GRE redirect. */
	uint16_t src_mac[ETH_ALEN / 2];		/**< Source MAC address. */
	uint16_t dest_mac[ETH_ALEN / 2];	/**< Destination MAC address. */
};

/**
 * nss_gre_redir_lag_us_del_hash_node_msg
 *	Message to delete hash entry.
 */
struct nss_gre_redir_lag_us_del_hash_node_msg {
	uint16_t src_mac[ETH_ALEN / 2];		/**< Source MAC address. */
	uint16_t dest_mac[ETH_ALEN / 2];	/**< Destination MAC address. */
};

/**
 * nss_gre_redir_lag_us_query_hash_node_msg
 *	Message to query if a hash entry is present.
 */
struct nss_gre_redir_lag_us_query_hash_node_msg {
	uint16_t src_mac[ETH_ALEN / 2];		/**< Source MAC address. */
	uint16_t dest_mac[ETH_ALEN / 2];	/**< Destination MAC address. */
	uint32_t ifnum;				/**< NSS interface number of GRE redirect. */
};

/**
 * nss_gre_redir_lag_us_cmn_sync_stats
 *	GRE redirect LAG upstream statistics.
 */
struct nss_gre_redir_lag_us_cmn_sync_stats {
	uint32_t amsdu_pkts;			/**< Number of AMSDU packets seen. */
	uint32_t amsdu_pkts_enqueued;		/**< Number of AMSDU packets enqueued. */
	uint32_t amsdu_pkts_exceptioned;	/**< Number of AMSDU packets exceptioned. */
	uint32_t exceptioned;			/**< Number of exceptioned packets. */
	uint32_t freed;				/**< Number of freed packets. */
};

/**
 * nss_gre_redir_lag_us_cmn_db_sync_stats
 *	Upstream database statistics.
 */
struct nss_gre_redir_lag_us_cmn_db_sync_stats {
	uint32_t add_attempt;		/**< Add hash attempts. */
	uint32_t add_success;		/**< Add hash success. */
	uint32_t add_fail_table_full;	/**< Add hash failed due to full table. */
	uint32_t add_fail_exists;	/**< Add hash failed as entry already exists. */
	uint32_t del_attempt;		/**< Delete hash attempts. */
	uint32_t del_success;		/**< Delete hash success. */
	uint32_t del_fail_not_found;	/**< Delete hash failed as entry not found in hash table. */
};

/**
 * nss_gre_redir_lag_us_tunnel_hash_node_stats
 *	Hash statistics for GRE redirect LAG.
 */
struct nss_gre_redir_lag_us_tunnel_hash_node_stats {
	uint64_t hits;					/**< Number of hits on this hash entry. */
	uint32_t if_num;				/**< GRE redirect interface number. */
	uint16_t src_mac[ETH_ALEN / 2];			/**< Source MAC address. */
	uint16_t dest_mac[ETH_ALEN / 2];		/**< Destination MAC address. */
};

/**
 * nss_gre_redir_lag_us_hash_stats_query_msg
 *	Hash statistics synchronization message.
 */
struct nss_gre_redir_lag_us_hash_stats_query_msg {
	/*
	 * Request.
	 * Hash stats request has starting index of hash entry.
	 * Request is initiated by driver periodically.
	 */
	uint16_t db_entry_idx;			/**< Starting index of request. */

	/*
	 * Response.
	 * Response contains count of hash entries. It also has next field
	 * which used as the request index in subsequent request by caller.
	 */
	uint16_t db_entry_next;			/**< Next index to be requested. */
	uint16_t count;			/**< Number of hash entries in the message. */
	uint16_t reserved;		/**< Reserved. */
	struct nss_gre_redir_lag_us_tunnel_hash_node_stats hstats[NSS_GRE_REDIR_LAG_US_MAX_HASH_PER_MSG];
					/**< Array of hash table entries. */
};

/**
 * nss_gre_redir_lag_us_cmn_sync_stats_msg
 *	Upstream statistics synchronization message.
 */
struct nss_gre_redir_lag_us_cmn_sync_stats_msg {
	struct nss_cmn_node_stats node_stats;				/**< Common node statistics. */
	struct nss_gre_redir_lag_us_cmn_sync_stats us_stats;		/**< Upstream statistics. */
	struct nss_gre_redir_lag_us_cmn_db_sync_stats db_stats;		/**< Common hash statistics. */
};

/**
 *nss_gre_redir_lag_us_msg
 *	GRE redirect LAG upstream messages.
 */
struct nss_gre_redir_lag_us_msg {
	struct nss_cmn_msg cm;		/**< Common message header. */

	/*
	 * Payload of a GRE redirect LAG message.
	 */
	union {
		struct nss_gre_redir_lag_us_config_msg config_us;			/**< Upstream configuration message. */
		struct nss_gre_redir_lag_us_add_hash_node_msg add_hash;			/**< Add hash entry. */
		struct nss_gre_redir_lag_us_del_hash_node_msg del_hash;			/**< Delete hash entry. */
		struct nss_gre_redir_lag_us_query_hash_node_msg query_hash;		/**< Hash entry query message. */
		struct nss_gre_redir_lag_us_cmn_sync_stats_msg us_sync_stats;		/**< Upstream statistics. */
		struct nss_gre_redir_lag_us_hash_stats_query_msg hash_stats;		/**< Hash statistics message. */
	} msg;				/**< GRE redirect LAG upstream message payload. */
};

/**
 * nss_gre_redir_lag_us_cmn_stats
 *	GRE redirect LAG upstream statistics.
 */
struct nss_gre_redir_lag_us_cmn_stats {
	uint64_t amsdu_pkts;			/**< Number of AMSDU packets seen. */
	uint64_t amsdu_pkts_enqueued;		/**< Number of AMSDU packets enqueued. */
	uint64_t amsdu_pkts_exceptioned;	/**< Number of AMSDU packets exceptioned. */
	uint64_t exceptioned;			/**< Number of exceptioned packets. */
	uint64_t freed;				/**< Freed packets when equeue to NSS to host fails. */
};

/**
 * nss_gre_redir_lag_us_cmn_db_stats
 *	Upstream database statistics.
 */
struct nss_gre_redir_lag_us_cmn_db_stats {
	uint64_t add_attempt;		/**< Add hash attempts. */
	uint64_t add_success;		/**< Add hash success. */
	uint64_t add_fail_table_full;	/**< Add hash failed due to full table. */
	uint64_t add_fail_exists;	/**< Add hash failed as entry already exists. */
	uint64_t del_attempt;		/**< Delete hash attempts. */
	uint64_t del_success;		/**< Delete hash success. */
	uint64_t del_fail_not_found;	/**< Delete hash failed as entry not found in hash table. */
};

/**
 * nss_gre_redir_lag_us_tunnel_stats
 *	Upstream tunnel node statistics.
 */
struct nss_gre_redir_lag_us_tunnel_stats {
	uint64_t rx_packets;					/**< Received packets. */
	uint64_t rx_bytes;					/**< Received bytes. */
	uint64_t tx_packets;					/**< Transmit packets. */
	uint64_t tx_bytes;					/**< Transmit bytes. */
	uint64_t rx_dropped[NSS_MAX_NUM_PRI];			/**< Packets dropped on receive due to queue full. */
	struct nss_gre_redir_lag_us_cmn_stats us_stats;		/**< Common node statistics. */
	struct nss_gre_redir_lag_us_cmn_db_stats db_stats;	/**< Common hash statistics. */
};

/**
 * nss_gre_redir_lag_us_stats_notification
 *	GRE redirect LAG upstream transmission statistics structure.
 */
struct nss_gre_redir_lag_us_stats_notification {
	struct nss_gre_redir_lag_us_tunnel_stats stats_ctx;	/**< Context transmission statistics. */
	uint32_t core_id;					/**< Core ID. */
	uint32_t if_num;					/**< Interface number. */
};

/**
 * nss_gre_redir_lag_ds_add_sta_msg
 *	Message to add station in LAG deployment.
 */
struct nss_gre_redir_lag_ds_add_sta_msg {
	uint16_t mac[ETH_ALEN / 2];		/**< Station MAC address. */
	uint8_t reorder_type;			/**< Reorder type for downstream. */
};

/**
 * nss_gre_redir_lag_ds_delete_sta_msg
 *	Message to delete station in LAG deployment.
 */
struct nss_gre_redir_lag_ds_delete_sta_msg {
	uint16_t mac[ETH_ALEN / 2];	/**< Station MAC address. */
};

/**
 * nss_gre_redir_lag_ds_update_sta_msg
 *	Message to update station.
 */
struct nss_gre_redir_lag_ds_update_sta_msg {
	uint16_t mac[ETH_ALEN / 2];		/**< Station MAC address. */
	uint8_t reorder_type;			/**< Reorder type for downstream. */
};

/**
 * nss_gre_redir_lag_ds_stats
 * 	GRE redirect link aggregation downstream statistics structure.
 */
struct nss_gre_redir_lag_ds_stats {
	uint32_t dst_invalid;			/**< Invalid destination packets. */
	uint32_t exception_cnt;			/**< Exception count. */
};

/**
 * nss_gre_redir_lag_ds_sync_stats_msg
 *	Downstream statistics synchronization message.
 */
struct nss_gre_redir_lag_ds_sync_stats_msg {
	struct nss_cmn_node_stats node_stats;		/**< Common node statistics. */
	struct nss_gre_redir_lag_ds_stats ds_stats;	/**< GRE redirect LAG downstream statistics. */
};

/**
 *nss_gre_redir_lag_ds_msg
 *	GRE redirect LAG downstream messages.
 */
struct nss_gre_redir_lag_ds_msg {
	struct nss_cmn_msg cm;		/**< Common message header. */

	/**
	 * Payload of a GRE redirect LAG downstream message.
	 */
	union {
		struct nss_gre_redir_lag_ds_add_sta_msg add_sta;		/**< Add station entry. */
		struct nss_gre_redir_lag_ds_delete_sta_msg del_sta;		/**< Delete station entry. */
		struct nss_gre_redir_lag_ds_update_sta_msg update_sta;		/**< Station entry update message. */
		struct nss_gre_redir_lag_ds_sync_stats_msg ds_sync_stats;	/**< Downstream statistics. */
	} msg;				/**< GRE redirect LAG downstream message payload. */
};

/**
 * nss_gre_redir_lag_ds_tun_stats
 *	Downstream statistics.
 */
struct nss_gre_redir_lag_ds_tun_stats {
	uint64_t rx_packets;		/**< Received packets. */
	uint64_t rx_bytes;		/**< Received bytes. */
	uint64_t tx_packets;		/**< Transmit packets. */
	uint64_t tx_bytes;		/**< Transmit bytes. */
	uint64_t rx_dropped[NSS_MAX_NUM_PRI];
					/**< Packets dropped on receive due to queue full. */
	uint64_t dst_invalid;		/**< Packets that do not have a valid destination. */
	uint64_t exception_cnt;		/**< Packets that are exceptioned to host. */
	uint32_t ifnum;			/**< NSS interface number. */
	bool valid;			/**< Valid flag. */
};

/**
 * nss_gre_redir_lag_ds_stats_notification
 *	GRE redirect LAG downstream transmission statistics structure.
 */
struct nss_gre_redir_lag_ds_stats_notification {
	struct nss_gre_redir_lag_ds_tun_stats stats_ctx;	/**< Context transmission statistics. */
	uint32_t core_id;					/**< Core ID. */
	uint32_t if_num;					/**< Interface number. */
};

/**
 * Callback function for receiving GRE redirect LAG upstream data.
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
typedef void (*nss_gre_redir_lag_us_data_callback_t)(struct net_device *netdev, struct sk_buff *skb, struct napi_struct *napi);

/**
 * Callback function for receiving GRE redirect LAG downstream data.
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
typedef void (*nss_gre_redir_lag_ds_data_callback_t)(struct net_device *netdev, struct sk_buff *skb, struct napi_struct *napi);

 /**
 * Callback function for receiving GRE redirect LAG upstream messages.
 *
 * @datatypes
 * nss_cmn_msg
 *
 * @param[in] app_data  Pointer to the application context of the message.
 * @param[in] msg       Pointer to the message data.
 */
typedef void (*nss_gre_redir_lag_us_msg_callback_t)(void *app_data, struct nss_cmn_msg *msg);

 /**
 * Callback function for receiving GRE redirect LAG downstream messages.
 *
 * @datatypes
 * nss_cmn_msg
 *
 * @param[in] app_data  Pointer to the application context of the message.
 * @param[in] msg       Pointer to the message data.
 */
typedef void (*nss_gre_redir_lag_ds_msg_callback_t)(void *app_data, struct nss_cmn_msg *msg);

/**
 * nss_gre_redir_lag_us_alloc_and_register_node
 *	Allocates and registers GRE redirect upstream LAG node.
 *
 * @datatypes
 * net_device \n
 * nss_gre_redir_lag_us_data_callback_t \n
 * nss_gre_redir_lag_us_msg_callback_t
 *
 * @param[in] dev           Net device pointer.
 * @param[in] cb_func_data  Data callback function.
 * @param[in] cb_func_msg   Message callback function.
 * @param[in] app_ctx       Application context for notify callback.
 *
 * @return
 * Interface number if allocation and registration is succesful, else -1.
 */
extern int nss_gre_redir_lag_us_alloc_and_register_node(struct net_device *dev,
		nss_gre_redir_lag_us_data_callback_t cb_func_data,
		nss_gre_redir_lag_us_msg_callback_t cb_func_msg, void *app_ctx);

/**
 * nss_gre_redir_lag_ds_alloc_and_register_node
 *	Allocates and registers GRE redirect downstream LAG node.
 *
 * @datatypes
 * net_device \n
 * nss_gre_redir_lag_ds_data_callback_t \n
 * nss_gre_redir_lag_ds_msg_callback_t
 *
 * @param[in] dev           Net device pointer.
 * @param[in] cb_func_data  Data callback function.
 * @param[in] cb_func_msg   Message callback function.
 * @param[in] app_ctx       Application context for notify callback.
 *
 * @return
 * Interface number if allocation and registration is succesful, else -1.
 */
extern int nss_gre_redir_lag_ds_alloc_and_register_node(struct net_device *dev,
		nss_gre_redir_lag_ds_data_callback_t cb_func_data,
		nss_gre_redir_lag_ds_msg_callback_t cb_func_msg, void *app_data);

/**
 * nss_gre_redir_lag_us_configure_node
 *	Configures LAG upstream node.
 *
 * @datatypes
 * nss_gre_redir_lag_us_config_msg
 *
 * @param[in] ifnum              NSS interface number.
 * @param[in] ngluc              Pointer to LAG upstream node configuration message.
 *
 * @return
 * True if successful, else false.
 */
extern bool nss_gre_redir_lag_us_configure_node(uint32_t ifnum,
		struct nss_gre_redir_lag_us_config_msg *ngluc);

/**
 * nss_gre_redir_lag_us_unregister_and_dealloc
 *	Deregister and deallocate GRE redirect upstream node.
 *
 * @param[in] if_num  NSS interface number.
 *
 * @return
 * Error code.
 *
 * @dependencies
 * The GRE redirect LAG interface must have been previously registered.
 */
extern enum nss_gre_redir_lag_err_types nss_gre_redir_lag_us_unregister_and_dealloc(uint32_t if_num);

/**
 * nss_gre_redir_lag_ds_unregister_and_dealloc
 *	Deregisters and dealloc GRE redirect LAG downstream interface from the NSS.
 *
 * @param[in] if_num  NSS interface number.
 *
 * @return
 * Error code.
 *
 * @dependencies
 * The GRE redirect LAG interface must have been previously registered.
 */
extern enum nss_gre_redir_lag_err_types nss_gre_redir_lag_ds_unregister_and_dealloc(uint32_t if_num);

/**
 * nss_gre_redir_lag_us_tx_msg
 *	Sends GRE redirect upstream LAG messages asynchronously.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_gre_redir_lag_us_msg
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 * @param[in] msg      Pointer to the message data.
 *
 * @return
 * Status of the Tx operation.
 */
extern nss_tx_status_t nss_gre_redir_lag_us_tx_msg(struct nss_ctx_instance *nss_ctx, struct nss_gre_redir_lag_us_msg *msg);

/**
 * nss_gre_redir_lag_ds_tx_msg
 *	Sends GRE redirect downstream LAG messages asynchronously.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_gre_redir_lag_ds_msg
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 * @param[in] msg      Pointer to the message data.
 *
 * @return
 * Status of the Tx operation.
 */
extern nss_tx_status_t nss_gre_redir_lag_ds_tx_msg(struct nss_ctx_instance *nss_ctx, struct nss_gre_redir_lag_ds_msg *msg);

/**
 * nss_gre_redir_lag_us_tx_buf
 *	Sends packets to GRE Redirect LAG upstream node.
 *
 * @datatypes
 * nss_ctx_instance \n
 * sk_buff
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 * @param[in] os_buf   Pointer to the OS buffer (e.g., skbuff).
 * @param[in] if_num   Tunnel interface number.
 *
 * @return
 * Status of the Tx operation.
 */
extern nss_tx_status_t nss_gre_redir_lag_us_tx_buf(struct nss_ctx_instance *nss_ctx, struct sk_buff *os_buf,
						uint32_t if_num);

/**
 * nss_gre_redir_lag_ds_tx_buf
 *	Sends packets to GRE Redirect LAG downstream node.
 *
 * @datatypes
 * nss_ctx_instance \n
 * sk_buff
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 * @param[in] os_buf   Pointer to the OS buffer (e.g., skbuff).
 * @param[in] if_num   Tunnel interface number.
 *
 * @return
 * Status of the Tx operation.
 */
extern nss_tx_status_t nss_gre_redir_lag_ds_tx_buf(struct nss_ctx_instance *nss_ctx, struct sk_buff *os_buf,
						uint32_t if_num);

/**
 * nss_gre_redir_lag_us_tx_msg_sync
 *	Sends upstream LAG messages to NSS firmware synchronously.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_gre_redir_lag_us_msg
 *
 * @param[in] nss_ctx  NSS core context.
 * @param[in] ngrm     Pointer to GRE redirect upstream LAG message data.
 *
 * @return
 * Status of Tx operation.
 */
extern nss_tx_status_t nss_gre_redir_lag_us_tx_msg_sync(struct nss_ctx_instance *nss_ctx, struct nss_gre_redir_lag_us_msg *ngrm);

/**
 * nss_gre_redir_lag_ds_tx_msg_sync
 *	Sends downstream LAG messages to NSS firmware synchronously.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_gre_redir_lag_ds_msg
 *
 * @param[in] nss_ctx  NSS core context.
 * @param[in] ngrm     Pointer to GRE redirect downstream LAG message data.
 *
 * @return
 * Status of Tx operation.
 */
extern nss_tx_status_t nss_gre_redir_lag_ds_tx_msg_sync(struct nss_ctx_instance *nss_ctx, struct nss_gre_redir_lag_ds_msg *ngrm);

/**
 * nss_gre_redir_lag_us_stats_get
 *	Fetches common node statistics for upstream GRE Redir LAG.
 *
 * @datatypes
 * nss_gre_redir_lag_us_tunnel_stats
 *
 * @param[out] cmn_stats  Pointer to common node statistics structure.
 * @param[in]  index      Index to fetch statistics from.
 *
 * @return
 * True if successful, else false.
 */
extern bool nss_gre_redir_lag_us_stats_get(struct nss_gre_redir_lag_us_tunnel_stats *cmn_stats, uint32_t index);

/**
 * nss_gre_redir_lag_ds_stats_get
 *	Fetches common node statistics for downstream GRE Redir LAG.
 *
 * @datatypes
 * nss_gre_redir_lag_ds_tun_stats
 *
 * @param[out] cmn_stats  Pointer to common node statistics structure.
 * @param[in]  index      Index to fetch statistics from.
 *
 * @return
 * True if successful, else false.
 */
extern bool nss_gre_redir_lag_ds_stats_get(struct nss_gre_redir_lag_ds_tun_stats *cmn_stats, uint32_t index);

/**
 * nss_gre_redir_lag_us_get_context
 *	Gets the GRE redirect LAG upstream context.
 *
 * @return
 * Pointer to the NSS core context.
 */
extern struct nss_ctx_instance *nss_gre_redir_lag_us_get_context(void);

/**
 * nss_gre_redir_lag_ds_get_context
 *	Gets the GRE redirect LAG downstream context.
 *
 * @return
 * Pointer to the NSS core context.
 */
extern struct nss_ctx_instance *nss_gre_redir_lag_ds_get_context(void);

/**
 * nss_gre_redir_lag_ds_stats_unregister_notifier
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
extern int nss_gre_redir_lag_ds_stats_unregister_notifier(struct notifier_block *nb);

/**
 * nss_gre_redir_lag_ds_stats_register_notifier
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
extern int nss_gre_redir_lag_ds_stats_register_notifier(struct notifier_block *nb);

/**
 * nss_gre_redir_lag_us_stats_unregister_notifier
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
extern int nss_gre_redir_lag_us_stats_unregister_notifier(struct notifier_block *nb);

/**
 * nss_gre_redir_lag_us_stats_register_notifier
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
extern int nss_gre_redir_lag_us_stats_register_notifier(struct notifier_block *nb);

/**
 * @}
 */

#endif /* __NSS_GRE_REDIR_LAG_H */

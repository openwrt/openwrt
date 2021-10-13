/*
 **************************************************************************
 * Copyright (c) 2021, The Linux Foundation. All rights reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all copies.
 *
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
 * @file nss_udp_st.h
 *	NSS Ethernet interface definitions.
 */

#ifndef __NSS_UDP_ST_H
#define __NSS_UDP_ST_H

/**
 * @addtogroup nss_udp_st_subsystem
 * @{
 */

#define NSS_UDP_ST_TX_CONN_MAX 16
#define NSS_UDP_ST_FLAG_IPV4 4		/**< L3 Protocol - IPv4. */
#define NSS_UDP_ST_FLAG_IPV6 6		/**< L3 Protocol - IPv6. */

/**
 * nss_udp_st_message_types
 *	UDP speed test message types.
 */
enum nss_udp_st_message_types {
	NSS_UDP_ST_START_MSG,			/**< Start message. */
	NSS_UDP_ST_STOP_MSG,			/**< Stop message. */
	NSS_UDP_ST_CFG_RULE_MSG,		/**< Configure IPv4/IPv6 rule. */
	NSS_UDP_ST_UNCFG_RULE_MSG,		/**< Unconfigure IPv4/IPv6 rule. */
	NSS_UDP_ST_STATS_SYNC_MSG,		/**< Statistic syncronization. */
	NSS_UDP_ST_TX_CREATE_MSG,		/**< Create transmit node. */
	NSS_UDP_ST_TX_DESTROY_MSG,		/**< Destroy transmit node. */
	NSS_UDP_ST_RESET_STATS_MSG,		/**< Reset existing statistics. */
	NSS_UDP_ST_MAX_MSG_TYPES,		/**< Maximum message type. */
};

/**
 * nss_udp_st_test_types
 *	Test types of the UDP speed test.
 */
enum nss_udp_st_test_types {
	NSS_UDP_ST_TEST_RX,			/**< Test type is receive. */
	NSS_UDP_ST_TEST_TX,			/**< Test type is transmit. */
	NSS_UDP_ST_TEST_MAX			/**< Maximum test type. */
};

/**
 * nss_udp_st_error
 *	UDP speed test error types.
 */
enum nss_udp_st_error {
	NSS_UDP_ST_ERROR_NONE,			/**< No error. */
	NSS_UDP_ST_ERROR_INCORRECT_RATE,	/**< Incorrect Tx rate. */
	NSS_UDP_ST_ERROR_INCORRECT_BUFFER_SIZE,	/**< Incorrect buffer size. */
	NSS_UDP_ST_ERROR_MEMORY_FAILURE,	/**< Memory allocation failed. */
	NSS_UDP_ST_ERROR_INCORRECT_STATE,	/**< Trying to configure during incorrect state. */
	NSS_UDP_ST_ERROR_INCORRECT_FLAGS,	/**< Incorrect flag configuration. */
	NSS_UDP_ST_ERROR_ENTRY_EXIST,		/**< Given tunnel entry already exists. */
	NSS_UDP_ST_ERROR_ENTRY_ADD_FAILED,	/**< UDP ST Encap entry addition failed. */
	NSS_UDP_ST_ERROR_ENTRY_NOT_EXIST,	/**< Given tunnel entry does not exists. */
	NSS_UDP_ST_ERROR_WRONG_START_MSG_TYPE,	/**< Start message type error. */
	NSS_UDP_ST_ERROR_WRONG_STOP_MSG_TYPE,	/**< Stop message type error. */
	NSS_UDP_ST_ERROR_TOO_MANY_USERS,	/**< Too many users tried to be added. */
	NSS_UDP_ST_ERROR_UNKNOWN_MSG_TYPE,	/**< Unknown message type failure. */
	NSS_UDP_ST_ERROR_PB_ALLOC,		/**< Pbuf allocation failed. */
	NSS_UDP_ST_ERROR_PB_SIZE,		/**< Pbuf size is too small to fit buffer. */
	NSS_UDP_ST_ERROR_DROP_QUEUE,		/**< Packet dropped enqueue next node. */
	UDP_ST_ERROR_TIMER_MISSED,		/**< Timer call is missed. */
	NSS_UDP_ST_ERROR_MAX,			/**< Maximum error type. */
};

/**
 * nss_udp_st_stats_time
 *	UDP speed test time statistics types.
 */
enum nss_udp_st_stats_time {
	NSS_UDP_ST_STATS_TIME_START,		/**< Start time of the test. */
	NSS_UDP_ST_STATS_TIME_CURRENT,		/**< Current time of the running test. */
	NSS_UDP_ST_STATS_TIME_ELAPSED,		/**< Elapsed time of the current test. */
	NSS_UDP_ST_STATS_TIME_MAX		/**< Maximum time statistics. */
};

/**
 * Create TX node to start pushing rules.
 */
struct nss_udp_st_tx_create {
	uint32_t rate;			/**< Rate in Mbps. */
	uint32_t buffer_size;		/**< UDP buffer size. */
	uint8_t dscp;			/**< DSCP value. */
};

/**
 * Destroy Tx node.
 */
struct nss_udp_st_tx_destroy {
	uint32_t flag;			/**< Tx destroy flag. */
};

/**
 * NSS UDP speed test start structure.
 */
struct nss_udp_st_start {
	uint32_t type;	/**< Started test type (for example, receive or transmit). */

};

/**
 * NSS UDP speed test stop structure.
 */
struct nss_udp_st_stop {
	uint32_t type;	/**< Stopped test type (for example, receive or transmit). */
};

/**
 * NSS UDP speed test ip structure
 */
struct nss_udp_st_ip {
	union {
		uint32_t ipv4;          /**< IPv4 address. */
		uint32_t ipv6[4];       /**< IPv6 address. */
	} ip;
};

/**
 * NSS UDP speed test IPv4/IPv6 configuration structure.
 */
struct nss_udp_st_cfg {
	struct nss_udp_st_ip src_ip;	/**< Source IP address. */
	int32_t src_port;		/**< Source L4 port. */
	struct nss_udp_st_ip dest_ip;	/**< Destination IP address. */
	int32_t dest_port;		/**< Destination L4 port. */
	uint32_t type;			/**< Started test type (for example, receive or transmit). */
	uint16_t ip_version;		/**< IP version to indicate IPv4 or IPv6. */
};

/**
 * NSS UDP speed test node statistics structure.
 */
struct nss_udp_st_node_stats {
	struct nss_cmn_node_stats node_stats;	/**< Common node statistics for the UDP speed test. */
	uint32_t errors[NSS_UDP_ST_ERROR_MAX];	/**< Error statistics. */
};

/**
 * NSS UDP speed test statistics structure.
 */
struct nss_udp_st_stats {
	struct nss_udp_st_node_stats nstats;	/**< Node statistics for the UDP speed test. */
	uint32_t time_stats[NSS_UDP_ST_TEST_MAX][NSS_UDP_ST_STATS_TIME_MAX];
						/**< Time statistics. */
};

/**
 * NSS UDP speed test reset statistics structure.
 */
struct nss_udp_st_reset_stats {
	uint32_t flag;  /**< Reset statistics flag. */
};

/**
 * Message structure of the UDP speed test commands.
 */
struct nss_udp_st_msg {
	struct nss_cmn_msg cm;          /**< Message header. */
	union {
		struct nss_udp_st_tx_create create;	/**< Prepare transmit message. */
		struct nss_udp_st_tx_destroy destroy;	/**< Destroy transmit message. */
		struct nss_udp_st_start start;		/**< Start message. */
		struct nss_udp_st_stop stop;		/**< Stop message. */
		struct nss_udp_st_cfg cfg;		/**< IPv4/IPv6 configuration message. */
		struct nss_udp_st_cfg uncfg;		/**< IPv4/IPv6 unconfiguration message. */
		struct nss_udp_st_stats stats;		/**< Statistics synchronization message. */
		struct nss_udp_st_reset_stats reset_stats;
							/**< Reset statistics message. */
	} msg;
};

/**
 * Callback function for receiving UDP speed test messages.
 *
 * @datatypes
 * nss_udp_st_msg
 *
 * @param[in] app_data  Pointer to the application context of the message.
 * @param[in] msg       Pointer to the message data.
 */
typedef void (*nss_udp_st_msg_callback_t)(void *app_data, struct nss_udp_st_msg *msg);

/**
 * nss_udp_st_register_handler
 *	Registers the UDP speed test message handler.
 *
 * @datatypes
 * nss_ctx_instance
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 *
 * @return
 * None.
 */
extern void nss_udp_st_register_handler(struct nss_ctx_instance *nss_ctx);

/**
 * nss_udp_st_tx
 *	Transmits a UDP speed test message to the NSS.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_udp_st_msg
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 * @param[in] num      Pointer to the message data.
 *
 * @return
 * Status of the transmit operation.
 */
extern nss_tx_status_t nss_udp_st_tx(struct nss_ctx_instance *nss_ctx, struct nss_udp_st_msg *num);

/**
 * nss_udp_st_tx_sync
 *	Transmits a synchronous UDP speed test message to the NSS.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_udp_st_msg
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 * @param[in] num      Pointer to the message data.
 *
 * @return
 * Status of the transmit operation.
 */
extern nss_tx_status_t nss_udp_st_tx_sync(struct nss_ctx_instance *nss_ctx, struct nss_udp_st_msg *num);

/**
 * nss_udp_st_msg_init
 *	Initializes UDP speed test messages.
 *
 * @datatypes
 * nss_udp_st_msg \n
 * nss_udp_st_msg_callback_t
 *
 * @param[in,out] num       Pointer to the NSS interface message.
 * @param[in]     if_num    NSS interface number.
 * @param[in]     type      Type of message.
 * @param[in]     len       Size of the payload.
 * @param[in]     cb        Callback function for the message.
 * @param[in]     app_data  Pointer to the application context of the message.
 *
 * @return
 * None.
 */
extern void nss_udp_st_msg_init(struct nss_udp_st_msg *num, uint16_t if_num, uint32_t type, uint32_t len,
			nss_udp_st_msg_callback_t cb, void *app_data);

/**
 * nss_udp_st_get_mgr
 *	Gets the NSS context that is managing UDP speed sest processes.
 *
 * @return
 * Pointer to the NSS core context.
 */
extern struct nss_ctx_instance *nss_udp_st_get_mgr(void);

/**
 *@}
 */

#endif /* __NSS_UDP_ST_H */

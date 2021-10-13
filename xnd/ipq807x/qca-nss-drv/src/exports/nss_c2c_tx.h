/*
 **************************************************************************
 * Copyright (c) 2018-2020, The Linux Foundation. All rights reserved.
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
 * @file nss_c2c_tx.h
 *	NSS core-to-core transmission interface definitions.
 */

#ifndef __NSS_C2C_TX_H
#define __NSS_C2C_TX_H

/**
 * @addtogroup nss_c2c_tx_subsystem
 * @{
 */

/**
 * nss_c2c_tx_msg_type
 *	Supported message types.
 */
enum nss_c2c_tx_msg_type {
	NSS_C2C_TX_MSG_TYPE_STATS,		/**< Statistics synchronization. */
	NSS_C2C_TX_MSG_TYPE_TX_MAP,		/**< Open engine synchronization. */
	NSS_C2C_TX_MSG_TYPE_PERFORMANCE_TEST,	/**< Performance test. */
	NSS_C2C_TX_MSG_TYPE_MAX			/**< Maximum message type. */
};

/**
 * nss_c2c_tx_msg_error
 *	Message error types.
 */
enum nss_c2c_tx_msg_error {
	NSS_C2C_TX_MSG_ERROR_NONE,		/**< No error. */
	NSS_C2C_TX_MSG_ERROR_INVAL_OP,		/**< Invalid operation. */
	NSS_C2C_TX_MSG_ERROR_INVALID_TEST_ID,	/**< Invalid test ID. */
	NSS_C2C_TX_MSG_ERROR_MAX		/**< Maximum error type. */
};

/**
 * nss_c2c_tx_test_type
 *	Supported core-to core transmission tests.
 */
enum nss_c2c_tx_test_type {
	NSS_C2C_TX_TEST_TYPE_SIMPLE = 1,
			/**< Tests the performance of simple pbufs. */
	NSS_C2C_TX_TEST_TYPE_SG_CHAIN,
			/**< Tests the performance of scatter-gather chain pbufs. */
	NSS_C2C_TX_TEST_TYPE_SG_REF,
			/**< Tests the performance of scatter-gather pbuf that has references. */
	NSS_C2C_TX_TEST_TYPE_SG_REFED,
			/**< Tests the performance of referenced pbuf. */
	NSS_C2C_TX_TEST_TYPE_MAX
			/**< Maximum message type. */
};

/**
 * nss_c2c_tx_stats_types
 *	Core-to-core transmission node statistics.
 */
enum nss_c2c_tx_stats_types {
	NSS_C2C_TX_STATS_PBUF_SIMPLE = NSS_STATS_NODE_MAX,
						/**< Number of received simple pbuf. */
	NSS_C2C_TX_STATS_PBUF_SG,		/**< Number of scatter-gather pbuf received. */
	NSS_C2C_TX_STATS_PBUF_RETURNING,	/**< Number of returning scatter-gather pbuf. */
	NSS_C2C_TX_STATS_MAX,			/**< Maximum message type. */
};

/**
 * nss_c2c_tx_stats_notification
 *	Core-to-core transmission statistics structure.
 */
struct nss_c2c_tx_stats_notification {
	uint32_t core_id;			/**< Core ID. */
	uint64_t stats[NSS_C2C_TX_STATS_MAX];	/**< Core-to-core transmission statistics. */
};

#ifdef __KERNEL__ /* only kernel will use. */
/**
 * nss_c2c_tx_map
 *	Core-to-core transmission queue address and interrupt address.
 */
struct nss_c2c_tx_map {
	uint32_t tx_map;		/**< Peer core core-to-core receiver queue start address. */
	uint32_t c2c_intr_addr;		/**< Peer core core-to-core interrupt register address. */
};

/**
 * nss_c2c_tx_stats
 *	The NSS core-to-core transmission node statistics structure.
 */
struct nss_c2c_tx_stats {
	struct nss_cmn_node_stats node_stats;
					/**< Common node statistics for core-to-core transmissions. */
	uint32_t pbuf_simple;		/**< Number of received simple pbuf. */
	uint32_t pbuf_sg;		/**< Number of scattered/gathered pbuf received. */
	uint32_t pbuf_returning;	/**< Number of returning scattered/gathered pbuf. */
};

/**
 * nss_c2c_tx_test
 *	Start performance test for the given test ID.
 */
struct nss_c2c_tx_test {
	uint32_t test_id;		/**< ID of the core-to-core communication test. */
};

/**
 * nss_c2c_tx_msg
 *	Message structure to send/receive core-to-core transmission commands.
 */
struct nss_c2c_tx_msg {
	struct nss_cmn_msg cm;			/**< Common message header. */

	/**
	 * Payload of a NSS core-to-core transmission rule or statistics message.
	 */
	union {
		struct nss_c2c_tx_map map;	/**< Core-to-core transmissions memory map. */
		struct nss_c2c_tx_stats stats;	/**< Core-to-core transmissions statistics. */
		struct nss_c2c_tx_test test;	/**< Core-to-core performance test. */
	} msg;					/**< Message payload. */
};

/**
 * nss_c2c_tx_register_handler
 *	Registers the core-to-core transmissions message handler.
 *
 * @datatypes
 * nss_ctx_instance
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 *
 * @return
 * None.
 */
void nss_c2c_tx_register_handler(struct nss_ctx_instance *nss_ctx);

/**
 * Callback function for receiving core-to-core transmissions messages.
 *
 * @datatypes
 * nss_c2c_tx_msg
 *
 * @param[in] app_data  Pointer to the application context of the message.
 * @param[in] msg       Pointer to the message data.
 */
typedef void (*nss_c2c_tx_msg_callback_t)(void *app_data, struct nss_c2c_tx_msg *msg);

/**
 * nss_c2c_tx_tx_msg
 *	Transmits a core-to-core transmissions message to the NSS.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_c2c_tx_msg
 *
 * @param[in] nss_ctx   Pointer to the NSS context.
 * @param[in] nctm      Pointer to the message data.
 *
 * @return
 * Status of the transmit operation.
 */
extern nss_tx_status_t nss_c2c_tx_tx_msg(struct nss_ctx_instance *nss_ctx, struct nss_c2c_tx_msg *nctm);

/**
 * nss_c2c_tx_msg_init
 *	Initializes core-to-core transmissions messages.
 *
 * @datatypes
 * nss_c2c_tx_msg \n
 * nss_c2c_tx_msg_callback_t
 *
 * @param[in]     nct       Pointer to the NSS interface message.
 * @param[in]     if_num    NSS interface number.
 * @param[in]     type      Type of message.
 * @param[in]     len       Size of the payload.
 * @param[in]     cb        Callback function for the message.
 * @param[in]     app_data  Pointer to the application context of the message.
 *
 * @return
 * None.
 */
extern void nss_c2c_tx_msg_init(struct nss_c2c_tx_msg *nct, uint16_t if_num, uint32_t type, uint32_t len,
			nss_c2c_tx_msg_callback_t cb, void *app_data);

/**
 * nss_c2c_tx_notify_register
 *	Registers a notifier callback for core-to-core transmission messages with the NSS.
 *
 * @datatypes
 * nss_c2c_tx_msg_callback_t
 *
 * @param[in] core      NSS core number index to the notifier callback table.
 * @param[in] cb        Callback function for the message.
 * @param[in] app_data  Pointer to the application context of the message.
 *
 * @return
 * Pointer to the NSS core context.
 */
struct nss_ctx_instance *nss_c2c_tx_notify_register(int core, nss_c2c_tx_msg_callback_t cb, void *app_data);

/**
 * nss_c2c_tx_notify_unregister
 *	Deregisters a core-to-core transmission message notifier callback from the NSS.
 *
 * @param[in] core NSS core number index to the notifier callback table.
 *
 * @return
 * None.
 *
 * @dependencies
 * The notifier callback must have been previously registered.
 */
void nss_c2c_tx_notify_unregister(int core);

/**
 * nss_c2c_tx_msg_cfg_map
 *	Sends core-to-core transmissions map to NSS
 *
 * @datatypes
 * nss_ctx_instance \n
 *
 * @param[in] nss_ctx   Pointer to the NSS context.
 * @param[in] tx_map    Peer core core-to-core receiver queue start address.
 * @param[in] c2c_addr  Peer core core-to-core interrupt register address.
 *
 * @return
 * Status of the transmit operation.
 */
extern nss_tx_status_t nss_c2c_tx_msg_cfg_map(struct nss_ctx_instance *nss_ctx, uint32_t tx_map, uint32_t c2c_addr);

/**
 * nss_c2c_tx_register_sysctl
 *     Registers the core-to-core transmission sysctl entries to the sysctl tree.
 *
 * @return
 * None.
 */
extern void nss_c2c_tx_register_sysctl(void);

/**
 * nss_c2c_tx_unregister_sysctl
 *     Deregisters the core-to-core transmission sysctl entries from the sysctl tree.
 *
 * @return
 * None.
 *
 * @dependencies
 * The system control must have been previously registered.
 */
extern void nss_c2c_tx_unregister_sysctl(void);

/**
 * nss_c2c_tx_init
 *	Initializes the core-to-core transmission.
 *
 * @return
 * None.
 */
void nss_c2c_tx_init(void);

/**
 * nss_c2c_tx_stats_register_notifier
 *	Registers a statistics notifier.
 *
 * @datatypes
 * notifier_block
 *
 * @param[in] nb Notifier block.
 *
 * @return
 * 0 on success or -2 on failure.
 */
extern int nss_c2c_tx_stats_register_notifier(struct notifier_block *nb);

/**
 * nss_c2c_tx_stats_unregister_notifier
 *	Deregisters a statistics notifier.
 *
 * @datatypes
 * notifier_block
 *
 * @param[in] nb Notifier block.
 *
 * @return
 * 0 on success or -2 on failure.
 */
extern int nss_c2c_tx_stats_unregister_notifier(struct notifier_block *nb);
#endif /*__KERNEL__ */

/**
 * @}
 */

#endif /* __NSS_C2C_TX_H */

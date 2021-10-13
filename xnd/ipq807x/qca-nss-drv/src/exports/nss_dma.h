/*
 **************************************************************************
 * Copyright (c) 2020-2021, The Linux Foundation. All rights reserved.
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
 * @file nss_dma.h
 *	NSS DMA for linearization and split interface definitions.
 */

#ifndef __NSS_DMA_H
#define __NSS_DMA_H

/**
 * @addtogroup nss_dma_subsystem
 * @{
 */

/**
 * nss_dma_msg_type
 *	Supported message types.
 */
enum nss_dma_msg_type {
	NSS_DMA_MSG_TYPE_NONE,			/**< Invalid message type. */
	NSS_DMA_MSG_TYPE_CONFIGURE,		/**< Configure DMA. */
	NSS_DMA_MSG_TYPE_SYNC_STATS,		/**< Statistics synchronization. */
	NSS_DMA_MSG_TYPE_TEST_PERF,		/**< Performance test. */
	NSS_DMA_MSG_TYPE_MAX			/**< Maximum message type. */
};

/**
 * nss_dma_msg_error
 *	Message error types.
 */
enum nss_dma_msg_error {
	NSS_DMA_MSG_ERROR_NONE,			/**< No error. */
	NSS_DMA_MSG_ERROR_HW_INIT,		/**< Invalid operation. */
	NSS_DMA_MSG_ERROR_UNHANDLED,		/**< Invalid test ID. */
	NSS_DMA_MSG_ERROR_TEST,			/**< Performance test failed. */
	NSS_DMA_MSG_ERROR_MAX			/**< Maximum error type. */
};

/**
 * nss_dma_stats_types
 *	DMA node statistics.
 */
enum nss_dma_stats_types {
	NSS_DMA_STATS_NO_REQ = NSS_STATS_NODE_MAX,
					/**< Request descriptor not available. */
	NSS_DMA_STATS_NO_DESC,		/**< DMA descriptors not available. */
	NSS_DMA_STATS_NEXTHOP,		/**< Failed to retrive next hop. */
	NSS_DMA_STATS_FAIL_NEXTHOP_QUEUE,
					/**< Failed to queue next hop. */
	NSS_DMA_STATS_FAIL_LINEAR_SZ,	/**< Failed to get memory for linearization. */
	NSS_DMA_STATS_FAIL_LINEAR_ALLOC,/**< Failed to allocate buffer for linearization. */
	NSS_DMA_STATS_FAIL_LINEAR_NO_SG,/**< Skip linearization due to non-SG packet. */
	NSS_DMA_STATS_FAIL_SPLIT_SZ,	/**< Failed to spliting buffer into multiple buffers. */
	NSS_DMA_STATS_FAIL_SPLIT_ALLOC,	/**< Failed to allocate buffer for split. */
	NSS_DMA_STATS_FAIL_SYNC_ALLOC,	/**< Failed to allocate buffer for sending statistics. */
	NSS_DMA_STATS_FAIL_CTX_ACTIVE,	/**< Failed to queue as the node is not active. */
	NSS_DMA_STATS_FAIL_HW_E0,	/**< Failed to process in hardware, error code E0. */
	NSS_DMA_STATS_FAIL_HW_E1,	/**< Failed to process in hardware, error code E1. */
	NSS_DMA_STATS_FAIL_HW_E2,	/**< Failed to process in hardware, error code E2. */
	NSS_DMA_STATS_FAIL_HW_E3,	/**< Failed to process in hardware, error code E3. */
	NSS_DMA_STATS_FAIL_HW_E4,	/**< Failed to process in hardware, error code E4. */
	NSS_DMA_STATS_FAIL_HW_E5,	/**< Failed to process in hardware, error code E5. */
	NSS_DMA_STATS_FAIL_HW_E6,	/**< Failed to process in hardware, error code E6. */
	NSS_DMA_STATS_FAIL_HW_E7,	/**< Failed to process in hardware, error code E7. */
	NSS_DMA_STATS_FAIL_HW_E8,	/**< Failed to process in hardware, error code E8. */
	NSS_DMA_STATS_FAIL_HW_E9,	/**< Failed to process in hardware, error code E9. */
	NSS_DMA_STATS_FAIL_HW_E10,	/**< Failed to process in hardware, error code E10. */
	NSS_DMA_STATS_FAIL_HW_E11,	/**< Failed to process in hardware, error code E11. */
	NSS_DMA_STATS_FAIL_HW_E12,	/**< Failed to process in hardware, error code E12. */
	NSS_DMA_STATS_FAIL_HW_E13,	/**< Failed to process in hardware, error code E13. */
	NSS_DMA_STATS_FAIL_HW_E14,	/**< Failed to process in hardware, error code E14. */
	NSS_DMA_STATS_FAIL_HW_E15,	/**< Failed to process in hardware, error code E15. */
	NSS_DMA_STATS_MAX,		/**< Maximum message type. */
};

/**
 * nss_dma_test_type
 *	DMA Test types.
 */
enum nss_dma_test_type {
	NSS_DMA_TEST_TYPE_DEFAULT = 0,		/**< Test default segment size. */
	NSS_DMA_TEST_TYPE_SWEEP,		/**< Test sweep segment size. */
	NSS_DMA_TEST_TYPE_LARGE,		/**< Test large segment size. */
	NSS_DMA_TEST_TYPE_VERIFY,		/**< Verify contents at receive processing. */
	NSS_DMA_TEST_TYPE_MAX			/**< Maximum test type. */
};

/**
 * nss_dma_stats_notification
 *	DMA transmission statistics structure.
 */
struct nss_dma_stats_notification {
	uint64_t stats_ctx[NSS_DMA_STATS_MAX];		/**< Context transmission statistics. */
	uint32_t core_id;				/**< Core ID. */
};

#ifdef __KERNEL__ /* only kernel will use. */
/*
 * Maximum number of HW specific statistics
 */
#define NSS_DMA_HW_ERROR_MAX 16

/*
 * Test configuration flags
 */
#define NSS_DMA_TEST_FLAGS_LINEARIZE 0x01	/**< Linearize test. */
#define NSS_DMA_TEST_FLAGS_SPLIT 0x02		/**< Split test. */

/**
 * nss_dma_test_cfg
 *	Test configuration.
 */
struct nss_dma_test_cfg {
	struct nss_cmn_node_stats node_stats;	/**< Common node statistics for DMA interface. */
	uint32_t flags;				/**< Test configuration flags. */
	uint32_t time_delta;			/**< Difference between start and end. */
	uint16_t packet_count;			/**< Number of packets to send. */
	uint16_t type;				/**< Type of test to run. */
};

/**
 * nss_dma_stats
 *	DMA statistics.
 */
struct nss_dma_stats {
	struct nss_cmn_node_stats node_stats;	/**< Common node statistics for DMA interface. */
	uint32_t no_req;			/**< Request descriptor not available. */
	uint32_t no_desc;			/**< DMA descriptors not available. */
	uint32_t fail_nexthop;			/**< Failed to retrive next hop. */
	uint32_t fail_nexthop_queue;		/**< Failed to queue next hop. */
	uint32_t fail_linear_sz;		/**< Failed to get memory for linearization. */
	uint32_t fail_linear_alloc;		/**< Failed to allocate buffer for linearization. */
	uint32_t fail_linear_no_sg;		/**< Skip linearization due to non-SG packet. */
	uint32_t fail_split_sz;			/**< Failed to spliting buffer into multiple buffers. */
	uint32_t fail_split_alloc;		/**< Failed to allocate buffer for split. */
	uint32_t fail_sync_alloc;		/**< Failed to allocate buffer for sending statistics. */
	uint32_t fail_ctx_active;		/**< Failed to queue as the node is not active. */
	uint32_t fail_hw[NSS_DMA_HW_ERROR_MAX]; /**< Hardware failures. */
};

/**
 * nss_dma_msg
 *	Message structure for configuring the DMA interface.
 */
struct nss_dma_msg {
	struct nss_cmn_msg cm;			/**< Common message header. */

	/**
	 * Payload of a NSS core-to-core transmission rule or statistics message.
	 */
	union {
		struct nss_dma_test_cfg test_cfg;
						/**< DMA test configuration. */
		struct nss_dma_stats stats;	/**< DMA interface statistics. */
	} msg;					/**< Message payload. */
};

/**
 * nss_dma_register_handler
 *	Registers the DMA message handler.
 *
 * @return
 * None.
 */
void nss_dma_register_handler(void);

/**
 * Callback function for receiving DMA messages.
 *
 * @datatypes
 * nss_c2c_tx_msg
 *
 * @param[in] app_data  Pointer to the application context of the message.
 * @param[in] msg       Pointer to the message data.
 */
typedef void (*nss_dma_msg_callback_t)(void *app_data, struct nss_cmn_msg *msg);

/**
 * nss_dma_tx_msg
 *	Transmits a DMA message to the NSS.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_dma_msg
 *
 * @param[in] nss_ctx   Pointer to the NSS context.
 * @param[in] ndm      Pointer to the message data.
 *
 * @return
 * Status of the transmit operation.
 */
extern nss_tx_status_t nss_dma_tx_msg(struct nss_ctx_instance *nss_ctx, struct nss_dma_msg *ndm);

/**
 * nss_dma_msg_init
 *	Initializes DMA messages.
 *
 * @datatypes
 * nss_dma_msg \n
 * nss_dma_msg_callback_t
 *
 * @param[in]     ndm       Pointer to the NSS interface message.
 * @param[in]     if_num    NSS interface number.
 * @param[in]     type      Type of message.
 * @param[in]     len       Size of the payload.
 * @param[in]     cb        Callback function for the message.
 * @param[in]     app_data  Pointer to the application context of the message.
 *
 * @return
 * None.
 */
extern void nss_dma_msg_init(struct nss_dma_msg *ndm, uint16_t if_num, uint32_t type, uint32_t len,
			nss_dma_msg_callback_t cb, void *app_data);

/**
 * nss_dma_notify_register
 *	Registers a notifier callback for DMA messages with the NSS.
 *
 * @datatypes
 * nss_dma_msg_callback_t
 *
 * @param[in] core      NSS core number index to the notifier callback table.
 * @param[in] cb        Callback function for the message.
 * @param[in] app_data  Pointer to the application context of the message.
 *
 * @return
 * Pointer to the NSS core context.
 */
struct nss_ctx_instance *nss_dma_notify_register(int core, nss_dma_msg_callback_t cb, void *app_data);

/**
 * nss_dma_notify_unregister
 *	Deregisters a DMA message notifier callback from the NSS.
 *
 * @param[in] core  NSS core number index to the notifier callback table.
 *
 * @return
 * None.
 *
 * @dependencies
 * The notifier callback must have been previously registered.
 */
void nss_dma_notify_unregister(int core);

/**
 * nss_dma_register_sysctl
 *     Registers the DMA interface to Linux system control tree.
 *
 * @return
 * None.
 */
extern void nss_dma_register_sysctl(void);

/**
 * nss_dma_unregister_sysctl
 *     Deregisters the DMA interface from Linux system control tree.
 *
 * @return
 * None.
 *
 * @dependencies
 * The system control must have been previously registered.
 */
extern void nss_dma_unregister_sysctl(void);

/**
 * nss_dma_init
 *	Initializes the DMA interface.
 *
 * @return
 * None.
 */
void nss_dma_init(void);

/**
 * nss_dma_get_context
 *	Get the per NSS core context enabled for DMA.
 *
 * @return
 * Pointer to the NSS core context.
 */
extern struct nss_ctx_instance *nss_dma_get_context(void);

/**
 * nss_dma_stats_unregister_notifier
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
extern int nss_dma_stats_unregister_notifier(struct notifier_block *nb);

/**
 * nss_dma_stats_register_notifier
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
extern int nss_dma_stats_register_notifier(struct notifier_block *nb);
#endif /*__KERNEL__ */

/**
 * @}
 */

#endif /* __NSS_C2C_TX_H */

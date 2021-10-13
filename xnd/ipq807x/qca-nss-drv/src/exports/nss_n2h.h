/*
 **************************************************************************
 * Copyright (c) 2014-2020, The Linux Foundation. All rights reserved.
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
 * @file nss_n2h.h
 *	NSS to HLOS interface definitions.
 */

#ifndef __NSS_N2H_H
#define __NSS_N2H_H

/**
 * @addtogroup nss_n2h_subsystem
 * @{
 */

#define MAX_PAGES_PER_MSG 32	/**< Maximum number of pages per message. */
#define NSS_N2H_RPS_PRI_DEFAULT -1	/**< Default RPS priority mapping. */

/**
 * nss_n2h_payload_info
 *	Payload configuration based on the watermark.
 */
struct nss_n2h_payload_info {
	uint32_t pool_size;		/**< Empty buffer pool size. */

	/**
	 * Low watermark.
	 * Set this field to 0 for the system to automatically determine the watermark.
	 */
	uint32_t low_water;

	/**
	 * High watermark.
	 * Set this field to 0 for the system to automatically determine the watermark.
	 */
	uint32_t high_water;
};

#ifdef __KERNEL__ /* only kernel will use. */

/**
 * nss_n2h_cfg_pvt
 *	N2H private data configuration.
 */
struct nss_n2h_cfg_pvt {
	struct semaphore sem;					/**< Semaphore for SMP synchronization. */
	struct completion complete;				/**< Waits for the NSS to process the message. */
	struct nss_n2h_payload_info empty_buf_pool_info;	/**< Empty buffer pool information. */
	struct nss_n2h_payload_info empty_paged_buf_pool_info;	/**< Paged buffer pool information. */
	int wifi_pool;						/**< Size of the empty Wi-Fi buffer pool. */
	int response;						/**< Response from the firmware. */
};

#endif /*__KERNEL__ */

/**
 * nss_n2h_stats_types
 *	N2H node statistics.
 */
enum nss_n2h_stats_types {
	NSS_N2H_STATS_QUEUE_DROPPED = NSS_STATS_NODE_MAX,
		/**< Number of packets dropped because the exception queue is too full. */
	NSS_N2H_STATS_TOTAL_TICKS,	/**< Total clock ticks spend inside the N2H. */
	NSS_N2H_STATS_WORST_CASE_TICKS,	/**< Worst case iteration of the exception path in ticks. */
	NSS_N2H_STATS_ITERATIONS,	/**< Number of iterations around the N2H. */
	NSS_N2H_STATS_PBUF_OCM_TOTAL_COUNT,	/**< Number of pbuf OCM total count. */
	NSS_N2H_STATS_PBUF_OCM_FREE_COUNT,	/**< Number of pbuf OCM free count. */
	NSS_N2H_STATS_PBUF_OCM_ALLOC_FAILS_WITH_PAYLOAD,
					/**< Number of pbuf OCM allocations that have failed with payload. */
	NSS_N2H_STATS_PBUF_OCM_ALLOC_FAILS_NO_PAYLOAD,
					/**< Number of pbuf OCM allocations that have failed without payload. */
	NSS_N2H_STATS_PBUF_DEFAULT_TOTAL_COUNT,	/**< Number of pbuf default total count. */
	NSS_N2H_STATS_PBUF_DEFAULT_FREE_COUNT,	/**< Number of pbuf default free count. */
	NSS_N2H_STATS_PBUF_DEFAULT_ALLOC_FAILS_WITH_PAYLOAD,
					/**< Number of pbuf default allocations that have failed with payload. */
	NSS_N2H_STATS_PBUF_DEFAULT_ALLOC_FAILS_NO_PAYLOAD,
	/**< Number of pbuf default allocations that have failed without payload. */

	NSS_N2H_STATS_PAYLOAD_ALLOC_FAILS,	/**< Number of pbuf allocations that have failed because there were no free payloads. */
	NSS_N2H_STATS_PAYLOAD_FREE_COUNT,	/**< Number of free payloads that exist. */

	NSS_N2H_STATS_H2N_CONTROL_PACKETS,	/**< Control packets received from HLOS. */
	NSS_N2H_STATS_H2N_CONTROL_BYTES,	/**< Control bytes received from HLOS. */
	NSS_N2H_STATS_N2H_CONTROL_PACKETS,	/**< Control packets sent to HLOS. */
	NSS_N2H_STATS_N2H_CONTROL_BYTES,	/**< Control bytes sent to HLOS. */

	NSS_N2H_STATS_H2N_DATA_PACKETS,		/**< Data packets received from HLOS. */
	NSS_N2H_STATS_H2N_DATA_BYTES,		/**< Data bytes received from HLOS. */
	NSS_N2H_STATS_N2H_DATA_PACKETS,		/**< Data packets sent to HLOS. */
	NSS_N2H_STATS_N2H_DATA_BYTES,		/**< Data bytes sent to HLOS. */
	NSS_N2H_STATS_N2H_TOT_PAYLOADS,		/**< Number of payloads in NSS. */
	NSS_N2H_STATS_N2H_INTERFACE_INVALID,	/**< Number of bad interface access. */
	NSS_N2H_STATS_ENQUEUE_RETRIES,		/**< Number of enqueue retries by N2H. */

	NSS_N2H_STATS_MAX,			/**< Maximum message type. */
};

/**
 * nss_n2h_metadata_types
 *	Message types for N2H requests and responses.
 */
enum nss_n2h_metadata_types {
	NSS_RX_METADATA_TYPE_N2H_STATS_SYNC = 0,
	NSS_TX_METADATA_TYPE_N2H_RPS_CFG,
	NSS_TX_METADATA_TYPE_N2H_EMPTY_POOL_BUF_CFG,
	NSS_TX_METADATA_TYPE_N2H_FLUSH_PAYLOADS,
	NSS_TX_METADATA_TYPE_N2H_MITIGATION_CFG,
	NSS_METADATA_TYPE_N2H_ADD_BUF_POOL,
	NSS_TX_METADATA_TYPE_SET_WATER_MARK,
	NSS_TX_METADATA_TYPE_GET_WATER_MARK,
	NSS_TX_METADATA_TYPE_N2H_WIFI_POOL_BUF_CFG,
	NSS_TX_DDR_INFO_VIA_N2H_CFG,
	NSS_TX_METADATA_TYPE_N2H_SET_PNODE_QUEUE_CFG,
	NSS_TX_METADATA_TYPE_N2H_EMPTY_PAGED_POOL_BUF_CFG,
	NSS_TX_METADATA_TYPE_SET_PAGED_WATER_MARK,
	NSS_TX_METADATA_TYPE_GET_PAGED_WATER_MARK,
	NSS_TX_METADATA_TYPE_N2H_RPS_PRI_MAP_CFG,
	NSS_TX_METADATA_TYPE_N2H_QUEUE_LIMIT_CFG,
	NSS_TX_METADATA_TYPE_N2H_PAGED_BUFFER_POOL_INIT,
	NSS_TX_METADATA_TYPE_N2H_HOST_BACK_PRESSURE_CFG,
	NSS_METADATA_TYPE_N2H_MAX,
};

/*
 * nss_n2h_error_types
 *	N2H error types.
 */
enum nss_n2h_error_types {
	N2H_EUNKNOWN = 1,
	N2H_ALREADY_CFG,
	N2H_LOW_WATER_MIN_INVALID,
	N2H_HIGH_WATER_LESS_THAN_LOW,
	N2H_HIGH_WATER_LIMIT_INVALID,
	N2H_LOW_WATER_LIMIT_INVALID,
	N2H_WATER_MARK_INVALID,
	N2H_EMPTY_BUFFER_TOO_HIGH,
	N2H_EMPTY_BUFFER_TOO_LOW,
	N2H_MMU_ENTRY_IS_INVALID,
	N2H_PN_QUEUE_SET_FAILED,
	N2H_PAGES_PER_MSG_EXCEEDED,
	N2H_RPS_PRI_MAP_TOO_HIGH,
};

/**
 * nss_n2h_stats_notification
 *	N2H statistics structure.
 */
struct nss_n2h_stats_notification {
	uint32_t core_id;			/**< Core ID. */
	uint64_t n2h_stats[NSS_N2H_STATS_MAX];	/**< N2H statistics. */
	uint64_t drv_stats[NSS_STATS_DRV_MAX];	/**< Driver statistics. */
};

/**
 * nss_n2h_rps
 *	N2H RPS configuration.
 */
struct nss_n2h_rps {
	uint32_t enable;	/**< Enable RPS. */
};

/**
 * nss_n2h_rps_pri_map
 *	N2H priority configuration.
 *
 * This is used to direct packets with a given priority to a specific host CPU.
 * A value of -1 in pri_map[pri] is treated as invalid and will not override
 * RPS for that priority.
 */
struct nss_n2h_rps_pri_map {
	int32_t pri_map[NSS_MAX_NUM_PRI];	/**< Priority to RPS map. */
};

/**
 * nss_n2h_mitigation
 *	N2H mitigation configuration.
 */
struct nss_n2h_mitigation {
	uint32_t enable;	/**< Enable NSS mitigation. */
};

/**
 * nss_n2h_buf_pool
 *	N2H buffer pool configuration.
 */
struct nss_n2h_buf_pool {
	uint32_t nss_buf_page_size;	/**< Size of the buffer page. */
	uint32_t nss_buf_num_pages;	/**< Number of buffer pages. */

	uint32_t nss_buf_pool_addr[MAX_PAGES_PER_MSG];
			/**< Buffer addresses. */
	nss_ptr_t nss_buf_pool_vaddr[MAX_PAGES_PER_MSG];
			/**< Virtual addresses of the buffers. */
#ifndef __LP64__
	uint32_t padding[MAX_PAGES_PER_MSG];
			/**< Pad to fit 64 bits, do not reuse. */
#endif
};

/**
 * nss_n2h_pnode_queue_config
 *	Queue configuration command for pnodes in NSS.
 */
struct nss_n2h_pnode_queue_config {
	uint8_t mq_en;		/**< Enable multiple queues. */
	uint8_t reserved[3];	/**< Reserved for alignment. */
	uint16_t qlimits[NSS_MAX_NUM_PRI];
				/**< Limits of each queue. */
#if (NSS_MAX_NUM_PRI & 1)
	uint16_t reserved2;
#endif
};

/**
 * nss_n2h_empty_pool_buf
 *	Old way of setting the number of empty pool buffers (payloads).
 *
 * The NSS firmware sets the low watermark to n -- ring_size, and the high
 * watermark to n + ring_size.
 */
struct nss_n2h_empty_pool_buf {
	uint32_t pool_size;		/**< Size of the empty buffer pool. */
};

/**
 * nss_n2h_water_mark
 *	New way of setting the low and high watermarks in the NSS firmware.
 */
struct nss_n2h_water_mark {
	/**
	 * Low watermark.
	 * Lower threshold for the number of payloads that can be held by NSS firmware.
	 * Setting this value to 0 gets the system to automatically determine the watermark.
	 */
	uint32_t low_water;

	/**
	 * High watermark.
	 * Upper threshold for the number of paylods that be held by the NSS firmware.
	 * Setting this value to 0 gets the system to automatically determine the watermark.
	 */
	uint32_t high_water;
};

/**
 * nss_n2h_flush_payloads
 *	Flush payload configuration.
 */
struct nss_n2h_flush_payloads {
	uint32_t reserved;		/**< Reserved for future use. */
};

/**
 * nss_n2h_wifi_payloads
 *	Payloads required for Wi-Fi offloading.
 */
struct nss_n2h_wifi_payloads {
	uint32_t payloads;	/**< Number of payloads for Wi-Fi use. */
};

/**
 * nss_n2h_pbuf_mgr_stats
 *	Payload buffer manager statistics.
 */
struct nss_n2h_pbuf_mgr_stats {
	uint32_t pbuf_total_count;	/**< Total number of buffers, free or in use. */
	uint32_t pbuf_free_count;	/**< Number of currently free buffers. */
	uint32_t pbuf_alloc_fails_with_payload;
					/**< Number of buffer allocation failures. */
	uint32_t pbuf_alloc_fails_no_payload;
					/**< Number of buffer allocation failures without payload. */
};

/**
 * nss_n2h_paged_buffer_pool_init
 *	Paged buffer configuration initialization.
 */
struct nss_n2h_paged_buffer_pool_init {
	uint32_t reserved;		/**< Reserved for future use. */
};

/**
 * nss_n2h_stats_sync
 *	N2H synchronization statistics.
 */
struct nss_n2h_stats_sync {
	struct nss_cmn_node_stats node_stats;	/**< Common node statistics. */
	uint32_t queue_dropped;
			/**< Number of packets dropped because the N2H queue is too full. */
	uint32_t total_ticks;		/**< Total clock ticks spent inside the N2H handler. */
	uint32_t worst_case_ticks;	/**< Worst case iteration of the N2H handler in ticks. */
	uint32_t iterations;		/**< Number of iterations around the N2H handler. */

	struct nss_n2h_pbuf_mgr_stats pbuf_ocm_stats;
			/**< Statistics for on-chip memory payload buffers. */
	struct nss_n2h_pbuf_mgr_stats pbuf_default_stats;
			/**< Statistics for DDR memory payload buffers. */

	uint32_t payload_alloc_fails;	/**< Number of payload allocation failures. */
	uint32_t payload_free_count;	/**< Number of free payloads. */

	uint32_t h2n_ctrl_pkts;		/**< Control packets received from the HLOS. */
	uint32_t h2n_ctrl_bytes;	/**< Control bytes received from the HLOS. */
	uint32_t n2h_ctrl_pkts;		/**< Control packets sent to the HLOS. */
	uint32_t n2h_ctrl_bytes;	/**< Control bytes sent to the HLOS. */

	uint32_t h2n_data_pkts;		/**< Data packets received from the HLOS. */
	uint32_t h2n_data_bytes;	/**< Data bytes received from the HLOS. */
	uint32_t n2h_data_pkts;		/**< Data packets sent to the HLOS. */
	uint32_t n2h_data_bytes;	/**< Data bytes sent to the HLOS. */
	uint32_t tot_payloads;		/**< Total number of payloads in the NSS firmware. */

	/**
	 * Number of data packets with invalid interface received from the host.
	 */
	uint32_t data_interface_invalid;
	uint32_t enqueue_retries;	/**< Number of times N2H retried enqueue to next node. */
};

/**
 * nss_mmu_ddr_info
 *	System DDR memory information required by the firmware MMU to set range guards.
 */
struct nss_mmu_ddr_info {
	uint32_t ddr_size;	/**< Total size of the DDR. */
	uint32_t start_address;	/**< System start address. */
	uint32_t num_active_cores;
				/**< Number of active cores. */
	uint32_t nss_ddr_size;	/**< Total memory for NSS SoC. */
};

/**
 * nss_n2h_queue_limit_config
 *	Queue length limit for N2H node.
 */
struct nss_n2h_queue_limit_config {
	uint32_t qlimit;	/**< Queue length size. */
};

/**
 * nss_n2h_host_back_pressure
 *	Host back pressure configuration.
 */
struct nss_n2h_host_back_pressure {
	uint32_t enable;		/**< Enable host back pressure. */
};

/**
 * nss_n2h_msg
 *	Data for sending and receiving N2H messages.
 */
struct nss_n2h_msg {
	struct nss_cmn_msg cm;		/**< Common message header. */

	/**
	 * Payload of an N2H message.
	 */
	union {
		struct nss_n2h_stats_sync stats_sync;
				/**< N2H statistics synchronization. */
		struct nss_n2h_rps rps_cfg;
				/**< RPS configuration. */
		struct nss_n2h_rps_pri_map rps_pri_map;
				/**< RPS priority map. */
		struct nss_n2h_empty_pool_buf empty_pool_buf_cfg;
				/**< Empty pool buffer configuration. */
		struct nss_n2h_empty_pool_buf empty_paged_pool_buf_cfg;
				/**< Empty paged pool buffer configuration. */
		struct nss_n2h_flush_payloads flush_payloads;
				/**< Flush payloads present in the NSS. */
		struct nss_n2h_mitigation mitigation_cfg;
				/**< Mitigation configuration. */
		struct nss_n2h_buf_pool buf_pool;
				/**< Pool buffer coniguration. */
		struct nss_n2h_water_mark wm;
				/**< Sets low and high watermarks. */
		struct nss_n2h_water_mark wm_paged;
				/**< Sets low and high watermarks for paged pool. */
		struct nss_n2h_payload_info payload_info;
				/**< Gets the payload information. */
		struct nss_n2h_payload_info paged_payload_info;
				/**< Gets the paged payload information. */
		struct nss_n2h_wifi_payloads wp;
				/**< Sets the number of Wi-Fi payloads. */
		struct nss_mmu_ddr_info mmu;
				/**< Gets the DDR size and start address to configure the MMU. */
		struct nss_n2h_pnode_queue_config pn_q_cfg;
				/**< Pnode queueing configuration. */
		struct nss_n2h_queue_limit_config ql_cfg;
				/**< Queue limit configuration. */
		struct nss_n2h_paged_buffer_pool_init paged_buffer_pool_init;
				/**< Paged buffer pool initialization. */
		struct nss_n2h_host_back_pressure host_bp_cfg;
				/**< Host back pressure configuration. */
	} msg;			/**< Message payload. */
};

/**
 * Callback function for receiving N2H messages.
 *
 * @datatypes
 * nss_n2h_msg
 *
 * @param[in] app_data  Pointer to the application context of the message.
 * @param[in] msg       Pointer to the N2H message.
 */
typedef void (*nss_n2h_msg_callback_t)(void *app_data, struct nss_n2h_msg *msg);

/**
 * nss_n2h_tx_msg
 *	Sends messages to the N2H package.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_n2h_msg
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 * @param[in] nnm      Pointer to the N2H message.
 *
 * @return
 * Status of the Tx operation.
 */
extern nss_tx_status_t nss_n2h_tx_msg(struct nss_ctx_instance *nss_ctx, struct nss_n2h_msg *nnm);

/**
 * nss_n2h_single_core_register_sysctl
 *	Registers the N2H sysctl entry to the sysctl tree for a single-core system.
 *
 * @return
 * None.
 */
extern void nss_n2h_single_core_register_sysctl(void);

/**
 * nss_n2h_multi_core_register_sysctl
 *	Registers the N2H sysctl entry to the sysctl tree for a multi-core system.
 *
 * @return
 * None.
 */
extern void nss_n2h_multi_core_register_sysctl(void);

/**
 * nss_n2h_unregister_sysctl
 *	Deregisters the N2H sysctl entry from the sysctl tree.
 *
 * @return
 * None.
 *
 * @dependencies
 * The system control must have been previously registered.
 */
extern void nss_n2h_unregister_sysctl(void);

/**
 * nss_n2h_flush_payloads
 *	Sends flush payloads message to NSS
 *
 * @datatypes
 * nss_ctx_instance
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 *
 * @return
 * Status of the Tx operation.
 */
extern nss_tx_status_t nss_n2h_flush_payloads(struct nss_ctx_instance *nss_ctx);

/**
 * nss_n2h_msg_init
 *	Initializes messages from the host to the NSS.
 *
 * @datatypes
 * nss_n2h_msg \n
 * nss_n2h_msg_callback_t
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
extern void nss_n2h_msg_init(struct nss_n2h_msg *nim, uint16_t if_num, uint32_t type, uint32_t len,
			nss_n2h_msg_callback_t cb, void *app_data);

/**
 * nss_n2h_update_queue_config_sync
 *	Synchrounous method to update pnode queue configuration to NSS.
 *
 * @datatypes
 * nss_ctx_instance
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 * @param[in] mq_en    Enable multiple pnode queues.
 * @param[in] qlimits  Maximum number of packets in each queues.
 *
 * @return
 * Status of the configuration update operation.
 */
extern nss_tx_status_t nss_n2h_update_queue_config_sync(struct nss_ctx_instance *nss_ctx, bool mq_en, uint16_t *qlimits);

/**
 * nss_n2h_update_queue_config_async
 *	Asynchrounous method to update pnode queue configuration to NSS.
 *
 * @datatypes
 * nss_ctx_instance
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 * @param[in] mq_en    Enable multiple pnode queues.
 * @param[in] qlimits  Maximum number of packets in each queues.
 *
 * @return
 * Status of the configuration update operation.
 */
extern nss_tx_status_t nss_n2h_update_queue_config_async(struct nss_ctx_instance *nss_ctx, bool mq_en, uint16_t *qlimits);

#ifdef __KERNEL__ /* only kernel will use. */

/**
 * nss_n2h_stats_register_notifier
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
extern int nss_n2h_stats_register_notifier(struct notifier_block *nb);

/**
 * nss_n2h_stats_unregister_notifier
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
extern int nss_n2h_stats_unregister_notifier(struct notifier_block *nb);

#endif /*__KERNEL__ */

/**
 * @}
 */

#endif /* __NSS_N2H_H */

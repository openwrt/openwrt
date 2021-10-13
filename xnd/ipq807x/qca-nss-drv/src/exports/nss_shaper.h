/*
 **************************************************************************
 * Copyright (c) 2014, 2017-2018 The Linux Foundation. All rights reserved.
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
 * @file nss_shaper.h
 *	NSS Shaper definitions
 */

#ifndef __NSS_SHAPER_H
#define __NSS_SHAPER_H

/**
 * @addtogroup nss_shaper_subsystem
 * @{
 */

/**
 * nss_shaper_node_types
 *	Types of shaper nodes that are exported to the HLOS.
  */
enum nss_shaper_node_types {
	NSS_SHAPER_NODE_TYPE_CODEL = 1,
	NSS_SHAPER_NODE_TYPE_PRIO = 3,
	NSS_SHAPER_NODE_TYPE_FIFO = 4,
	NSS_SHAPER_NODE_TYPE_TBL = 5,
	NSS_SHAPER_NODE_TYPE_BF = 6,
	NSS_SHAPER_NODE_TYPE_BF_GROUP = 7,
	NSS_SHAPER_NODE_TYPE_WRR = 9,
	NSS_SHAPER_NODE_TYPE_WRR_GROUP = 10,
	NSS_SHAPER_NODE_TYPE_HTB = 11,
	NSS_SHAPER_NODE_TYPE_HTB_GROUP = 12,
	NSS_SHAPER_NODE_TYPE_WRED = 13,
	NSS_SHAPER_NODE_TYPE_PPE_SN = 14,
	NSS_SHAPER_NODE_TYPE_MAX,
};

typedef enum nss_shaper_node_types nss_shaper_node_type_t;
		/**< Types of shaper nodes that are exported to the HLOS. */

/**
 * nss_shaper_config_types
 *	Types of shaper configuration messages.
 */
enum nss_shaper_config_types {
	NSS_SHAPER_CONFIG_TYPE_ALLOC_SHAPER_NODE,
	NSS_SHAPER_CONFIG_TYPE_FREE_SHAPER_NODE,
	NSS_SHAPER_CONFIG_TYPE_SET_DEFAULT,
	NSS_SHAPER_CONFIG_TYPE_SET_ROOT,
	NSS_SHAPER_CONFIG_TYPE_SHAPER_NODE_BASIC_STATS_GET,
	NSS_SHAPER_CONFIG_TYPE_SHAPER_NODE_ATTACH,
	NSS_SHAPER_CONFIG_TYPE_SHAPER_NODE_DETACH,
	NSS_SHAPER_CONFIG_TYPE_SHAPER_NODE_CHANGE_PARAM,
	NSS_SHAPER_CONFIG_TYPE_HYBRID_MODE_ENABLE,
	NSS_SHAPER_CONFIG_TYPE_HYBRID_MODE_DISABLE,
	NSS_SHAPER_CONFIG_TYPE_SHAPER_NODE_MEM_REQ,
};

typedef enum nss_shaper_config_types nss_shaper_config_type_t;
		/**< Types of shaper configuration messages. */

/**
 * nss_shaper_response_types
 *	Types of shaper configuration responses.
 */
enum nss_shaper_response_types {
	NSS_SHAPER_RESPONSE_TYPE_SUCCESS,
	NSS_SHAPER_RESPONSE_TYPE_NO_SHAPER_NODE,
	NSS_SHAPER_RESPONSE_TYPE_NO_SHAPER_NODES,
	NSS_SHAPER_RESPONSE_TYPE_OLD,
	NSS_SHAPER_RESPONSE_TYPE_UNRECOGNISED,
	NSS_SHAPER_RESPONSE_TYPE_BAD_DEFAULT_CHOICE,
	NSS_SHAPER_RESPONSE_TYPE_DUPLICATE_QOS_TAG,
	NSS_SHAPER_RESPONSE_TYPE_TBL_CIR_RATE_AND_BURST_REQUIRED,
	NSS_SHAPER_RESPONSE_TYPE_TBL_CIR_BURST_LESS_THAN_MTU,
	NSS_SHAPER_RESPONSE_TYPE_CODEL_ALL_PARAMS_REQUIRED,
	NSS_SHAPER_RESPONSE_TYPE_BF_GROUP_RATE_AND_BURST_REQUIRED,
	NSS_SHAPER_RESPONSE_TYPE_BF_GROUP_BURST_LESS_THAN_MTU,
	NSS_SHAPER_RESPONSE_TYPE_CHILD_NOT_BF_GROUP,
	NSS_SHAPER_RESPONSE_TYPE_WRR_GROUP_INVALID_QUANTUM,
	NSS_SHAPER_RESPONSE_TYPE_CHILD_NOT_WRR_GROUP,
	NSS_SHAPER_RESPONSE_TYPE_WRR_INVALID_OPERATION_MODE,
	NSS_SHAPER_RESPONSE_TYPE_HTB_GROUP_BURST_LESS_THAN_MTU,
	NSS_SHAPER_RESPONSE_TYPE_HTB_GROUP_PRIORITY_OUT_OF_RANGE,
	NSS_SHAPER_RESPONSE_TYPE_CHILDREN_BELONG_TO_MIXED_TYPES,
	NSS_SHAPER_RESPONSE_TYPE_CHILD_ALREADY_PRESENT,
	NSS_SHAPER_RESPONSE_TYPE_CHILD_MISMATCH,
	NSS_SHAPER_RESPONSE_TYPE_CHILD_UNSUPPORTED,
	NSS_SHAPER_RESPONSE_TYPE_CHILD_NOT_FOUND,
	NSS_SHAPER_RESPONSE_TYPE_ATTACH_FAIL,
	NSS_SHAPER_RESPONSE_TYPE_WRED_WEIGHT_MODE_INVALID,
	NSS_SHAPER_RESPONSE_TYPE_PPE_SN_UCAST_BASE_OFFSET_INVALID,
	NSS_SHAPER_RESPONSE_TYPE_PPE_SN_MCAST_BASE_OFFSET_INVALID,
	NSS_SHAPER_RESPONSE_TYPE_PPE_SN_UCAST_QUEUE_ALLOC_FAILED,
	NSS_SHAPER_RESPONSE_TYPE_PPE_SN_MCAST_QUEUE_ALLOC_FAILED,
	NSS_SHAPER_RESPONSE_TYPE_PPE_SN_INVALID_LIMIT,
	NSS_SHAPER_RESPONSE_TYPE_PPE_SN_UCAST_QUEUE_CHANGED,
	NSS_SHAPER_RESPONSE_TYPE_PPE_SN_MCAST_QUEUE_CHANGED,
	NSS_SHAPER_RESPONSE_TYPE_CODEL_FQ_MEM_INSUFFICIENT,
	NSS_SHAPER_RESPONSE_TYPE_CODEL_FQ_COUNT_CHANGE_NOT_ALLOWED,
	NSS_SHAPER_RESPONSE_TYPE_CODEL_FQ_COUNT_INVALID,
	NSS_SHAPER_RESPONSE_TYPE_CODEL_MODE_CHANGE_NOT_ALLOWED,
};

typedef enum nss_shaper_response_types nss_shaper_response_type_t;
		/**< Types of shaper configuration responses. */

/**
 * nss_shaper_config_alloc_shaper_node
 *	Message information for allocating a shaper node for a NSS interface.
 */
struct nss_shaper_config_alloc_shaper_node {
	nss_shaper_node_type_t node_type;	/**< Type of shaper node. */
	uint32_t qos_tag;			/**< QoS tag of the node. */
};

/**
 * nss_shaper_config_free_shaper_node
 *	Message information for freeing a shaper node from a NSS interface.
 */
struct nss_shaper_config_free_shaper_node {
	uint32_t qos_tag;	/**< QoS tag of the node. */
};

/**
 * nss_shaper_config_set_root_node
 *	Message information for setting a shaper node as the root.
 */
struct nss_shaper_config_set_root_node {
	uint32_t qos_tag;	/**< QoS tag of the node. */
};

/**
 * nss_shaper_config_set_default_node
 *	Message information for setting a shaper node as the default node for enqueueing.
 */
struct nss_shaper_config_set_default_node {
	uint32_t qos_tag;	/**< QoS tag of the node. */
};

/**
 * nss_shaper_config_set_hybrid_mode
 *	Message information for setting a shaper to operate in hybrid mode.
 */
struct nss_shaper_config_set_hybrid_mode {
	uint32_t offset;	/**< Queue offset for packets sent to the hardware. */
};

/**
 * nss_shaper_config_prio_attach
 *	Message information for attaching a shaper node to a PRIO shaper node.
 */
struct nss_shaper_config_prio_attach {
	uint32_t child_qos_tag;	/**< QoS tag of the child shaper node. */
	uint32_t priority;	/**< Priority of the child shaper node. */
};

/**
 * nss_shaper_config_prio_detach
 *	Message information for detaching a shaper node from a PRIO shaper node.
 */
struct nss_shaper_config_prio_detach {
	uint32_t priority;	/**< Priority of the child shaper node. */
};

/**
 * nss_shaper_config_codel_alg_param
 *	Message information for configuring a CoDel algorithm.
 */
struct nss_shaper_config_codel_alg_param {
	uint16_t interval;	/**< Buffer time to smooth a state transition. */
	uint16_t target;	/**< Acceptable delay associated with a queue. */
	uint16_t mtu;		/**< MTU for the associated interface. */
	uint16_t reserved;	/**< Alignment padding. */
};

/**
 * nss_shaper_config_codel_param
 *	Message information for configuring a CoDel shaper node.
 */
struct nss_shaper_config_codel_param {
	int32_t qlen_max;	/**< Maximum number of packets that can be enqueued. */
	struct nss_shaper_config_codel_alg_param cap;
				/**< Configuration for the CoDel algorithm. */
	uint32_t flows;		/**< Number of flow hash buckets. */
	uint32_t flows_mem;	/**< Host allocated memory for flow queues. */
	uint32_t flows_mem_sz;	/**< Memory size allocated for flow queues. */
	uint32_t quantum;	/**< Quantum (in bytes) to round-robin the flow buckets. */
	uint32_t ecn;		/**< 0 - ECN disabled, 1 - ECN enabled. */
};

/**
 * nss_shaper_config_codel_mem_req
 *	Message to get CoDel memory requirement per flow queue (needed for fq_codel).
 */
struct nss_shaper_config_codel_mem_req {
	uint32_t mem_req;	/**< Memory needed per flow queue (in bytes). */
};

/**
 * nss_shaper_config_rate_param
 *	Message information for configuring the rate limiter algorithm.
 */
struct nss_shaper_config_rate_param {
	uint32_t rate;
			/**< Allowed traffic rate measured in bytes per second. */
	uint32_t burst;
			/**< Maximum bytes that can be sent in a burst. */
	uint32_t max_size;
			/**< Maximum size of the supported packets (in bytes). */

	/**
	 * Specifies whether the rate limiter will be bypassed (short circuited).
	 */
	bool short_circuit;
};

/**
 * nss_shaper_configure_tbl_attach
 *	Message information for attaching a shaper node to a TBL shaper node.
 */
struct nss_shaper_config_tbl_attach {
	uint32_t child_qos_tag;	/**< QoS tag of the child shaper node. */
};

/**
 * nss_shaper_configure_tbl_param
 *	Message information for detaching a shaper node from a TBL shaper node.
 */
struct nss_shaper_config_tbl_param {
	struct nss_shaper_config_rate_param lap_cir;
		/**< Configuration parameters for the committed information rate. */
	struct nss_shaper_config_rate_param lap_pir;
		/**< Configuration parameters for the peak information rate. */
};

/**
 * nss_shaper_config_bf_attach
 *	Message information for attaching a shaper node to a BF shaper node.
 */
struct nss_shaper_config_bf_attach {
	uint32_t child_qos_tag;	/**< QoS tag of the child shaper node. */
};

/**
 * nss_shaper_config_bf_detach
 *	Message information for detaching a shaper node from a BF shaper node.
 */
struct nss_shaper_config_bf_detach {
	uint32_t child_qos_tag;	/**< QoS tag of the child shaper node. */
};

/**
 * nss_shaper_config_bf_group_attach
 *	Message information for attaching a shaper node to a BF group shaper node.
 */
struct nss_shaper_config_bf_group_attach {
	uint32_t child_qos_tag;	/**< QoS tag of the child shaper node. */
};

/**
 * nss_shaper_config_bf_group_param
 *	Configuration parameters for a BF group shaper node.
 */
struct nss_shaper_config_bf_group_param {
	uint32_t quantum;
		/**< Smallest increment value for the DRRs. */
	struct nss_shaper_config_rate_param lap;
		/**< Configuration of the rate control algorithm. */
};

/**
 * nss_shaper_config_fifo_limit_set
 *	Drop modes for the FIFO shaper in the NSS interface.
 */
enum nss_shaper_config_fifo_drop_modes {
	NSS_SHAPER_FIFO_DROP_MODE_HEAD = 0,
	NSS_SHAPER_FIFO_DROP_MODE_TAIL,
	NSS_SHAPER_FIFO_DROP_MODES,
};

typedef enum nss_shaper_config_fifo_drop_modes nss_shaper_config_fifo_drop_mode_t;
		/**< Drop modes for the FIFO shaper in the NSS interface. */

/**
 * nss_shaper_config_fifo_param
 *	Message information for configuring a FIFO shaper node.
 */
struct nss_shaper_config_fifo_param {
	uint32_t limit;		/**< Queue limit in packets. */
	nss_shaper_config_fifo_drop_mode_t drop_mode;
				/**< FIFO drop mode when a queue is full. */
};

/**
 * nss_shaper_config_wred_weight_modes
 *	Supported weight modes.
 */
enum nss_shaper_config_wred_weight_modes {
	NSS_SHAPER_WRED_WEIGHT_MODE_DSCP = 0,
	NSS_SHAPER_WRED_WEIGHT_MODES,
};

typedef enum nss_shaper_config_wred_weight_modes nss_shaper_config_wred_weight_mode_t;
		/**< Supported weight modes. */

/**
 * nss_shaper_red_alg_param
 *	Message information for configuring the RED algorithm.
 */
struct nss_shaper_red_alg_param {
	uint32_t min;			/**< Minimum size of the queue. */
	uint32_t max;			/**< Maximum size of the queue. */

	/**
	 * Probability of dropped packets when the average queue size (qlen_avg) = max.
	 */
	uint32_t probability;

	/**
	 * Exponential weight factor to calculate the average queue size.
	 */
	uint32_t exp_weight_factor;
};

/**
 * nss_shaper_config_wred_param
 *	Message information for configuring the WRED algorithm.
 */
struct nss_shaper_config_wred_param {
	uint32_t limit;			/**< Queue limit in bytes. */
	nss_shaper_config_wred_weight_mode_t weight_mode;
					/**< WRED weight mode. */
	uint32_t traffic_classes;	/**< Number of traffic classes (drop probability). */
	uint32_t def_traffic_class;	/**< Default traffic class used when there is no match. */
	uint32_t traffic_id;		/**< Traffic class to configure. */
	uint32_t weight_mode_value;	/**< Value to match the selected header field against. */
	struct nss_shaper_red_alg_param rap;
					/**< Configuration parameters for the RED algorithm. */
	uint8_t ecn;			/**< Mark an ECN bit or drop packet. */
};

/**
 * nss_shaper_config_wrr_attach
 *	Message information for attaching a shaper node to a WRR shaper node.
 */
struct nss_shaper_config_wrr_attach {
	uint32_t child_qos_tag;	/**< QoS tag of the child shaper node. */
};

/**
 * nss_shaper_config_wrr_detach
 *	Message information for detaching a child node from a WRR shaper node.
 */
struct nss_shaper_config_wrr_detach {
	uint32_t child_qos_tag;	/**< QoS tag of the child shaper node. */
};

/**
 * nss_shaper_config_wrr_group_attach
 *	Message information for attaching a shaper node to a WRR group.
 */
struct nss_shaper_config_wrr_group_attach {
	uint32_t child_qos_tag;	/**< QoS tag of the child shaper node. */
};

/**
 * nss_shaper_wrr_operation_modes
 *	Modes of WRR operation.
 */
enum nss_shaper_wrr_operation_modes {
	NSS_SHAPER_WRR_MODE_ROUND_ROBIN = 0,
	NSS_SHAPER_WRR_MODE_FAIR_QUEUEING = 1,
	NSS_SHAPER_WRR_MODE_TYPE_MAX,
};

/**
 * nss_shaper_config_wrr_param
 *	Message information for configuring the operation mode of a WRR shaper node.
 */
struct nss_shaper_config_wrr_param {
	uint32_t operation_mode;	/**< Mode in which to operate. */
};

/**
 * nss_shaper_config_wrr_group_param
 *	Message information for configuring a quantum value of a WRR group shaper node.
 */
struct nss_shaper_config_wrr_group_param {
	uint32_t quantum;	/**< Smallest increment value for the DRRs. */
};

/**
 * nss_shaper_config_htb_attach
 *	Message information for attaching a shaper node to an HTB shaper node.
 */
struct nss_shaper_config_htb_attach {
	uint32_t child_qos_tag;	/**< QoS tag of the child shaper node. */
};

/**
 * nss_shaper_config_htb_group_attach
 *	Message information for attaching a shaper node to an HTB group.
 */
struct nss_shaper_config_htb_group_attach {
	uint32_t child_qos_tag;	/**< QoS tag of the child shaper node. */
};

/**
 * nss_shaper_config_htb_group_detach
 *	Message information for detaching a shaper node from an HTB group.
 */
struct nss_shaper_config_htb_group_detach {
	uint32_t child_qos_tag;	/**< QoS tag of the child shaper node. */
};

/**
 * nss_shaper_config_htb_group_param
 *	Message information for configuring an HTB group shaper node.
 */
struct nss_shaper_config_htb_group_param {
	uint32_t quantum;	/**< Smallest increment value for the DRRs. */
	uint32_t priority;	/**< Value of the priority for this group. */
	uint32_t overhead;	/**< Overhead in bytes to be added per packet. */
	struct nss_shaper_config_rate_param rate_police;
		/**< Configuration parameters for the policing rate. */
	struct nss_shaper_config_rate_param rate_ceil;
		/**< Configuration parameters for the ceiling rate. */
};

/**
 * nss_shaper_config_ppe_sn_attach
 *	Message information for attaching a shaper node to a PPE shaper node.
 */
struct nss_shaper_config_ppe_sn_attach {
	uint32_t child_qos_tag;	/**< QoS tag of the child shaper node. */
};

/**
 * nss_shaper_config_ppe_sn_detach
 *	Message information for detaching a shaper node from a PPE shaper node.
 */
struct nss_shaper_config_ppe_sn_detach {
	uint32_t child_qos_tag;	/**< QoS tag of the child shaper node. */
};

/**
 * nss_shaper_config_ppe_sn_type
 *	Types of PPE shaper nodes.
 */
enum nss_shaper_config_ppe_sn_type {
	/*
	 * Scheduler types.
	 */
	NSS_SHAPER_CONFIG_PPE_SN_TYPE_HTB,
	NSS_SHAPER_CONFIG_PPE_SN_TYPE_HTB_GROUP,
	NSS_SHAPER_CONFIG_PPE_SN_TYPE_TBL,
	NSS_SHAPER_CONFIG_PPE_SN_TYPE_WRR,
	NSS_SHAPER_CONFIG_PPE_SN_TYPE_WRR_GROUP,
	NSS_SHAPER_CONFIG_PPE_SN_TYPE_PRIO,
	NSS_SHAPER_CONFIG_PPE_SN_SCH_MAX = 0xFF,

	/*
	 * Queue types.
	 */
	NSS_SHAPER_CONFIG_PPE_SN_TYPE_FIFO,
	NSS_SHAPER_CONFIG_PPE_SN_TYPE_RED,
	NSS_SHAPER_CONFIG_PPE_SN_TYPE_MAX,
};

/**
 * nss_shaper_config_ppe_sn_param
 *	Message information for configuring a PPE shaper node.
 */
struct nss_shaper_config_ppe_sn_param {
	enum nss_shaper_config_ppe_sn_type type;
				/**< Type of PPE shaper node. */
	uint16_t ucast_base;	/**< Resource ID of the base hardware for unicast queue. */
	uint16_t ucast_offset;	/**< Offset from the base resource ID for unicast queue. */
	uint16_t mcast_base;	/**< Resource ID of the base hardware for multicast queue. */
	uint16_t mcast_offset;	/**< Offset from the base resource ID for multicast queue. */
	uint8_t port;		/**< PPE port on which this shaper node is configured. */
	uint8_t reserved;	/**< Reserved for padding. */
	uint16_t limit;		/**< Limit of the queue. */
};

/*
 * nss_shaper_node_config
 *	Configuration messages for all types of shaper nodes.
 */
struct nss_shaper_node_config {
	uint32_t qos_tag;	/**< ID of the shaper node to be configured. */

	/**
	 * Configuration messages for all types of shaper nodes.
	 */
	union {
		struct nss_shaper_config_prio_attach prio_attach;
			/**< Attach a shaper node to a PRIO shaper node. */
		struct nss_shaper_config_prio_detach prio_detach;
			/**< Detach a shaper node from a PRIO shaper node. */

		struct nss_shaper_config_codel_param codel_param;
			/**< Configure a CoDel shaper node. */

		struct nss_shaper_config_codel_mem_req codel_mem_req;
			/**< Get CoDel memory requirement. */

		struct nss_shaper_config_tbl_attach tbl_attach;
			/**< Attach a shaper node to a TBL shaper node. */
		struct nss_shaper_config_tbl_param tbl_param;
			/**< Configuration parameters for a TBL shaper node. */

		struct nss_shaper_config_bf_attach bf_attach;
			/**< Attach a shaper node to a BF shaper node. */
		struct nss_shaper_config_bf_detach bf_detach;
			/**< Detach a child shaper node from BF shaper node. */
		struct nss_shaper_config_bf_group_attach bf_group_attach;
			/**< Attach a shaper node to a BF group shaper node. */
		struct nss_shaper_config_bf_group_param bf_group_param;
			/**< Configure parameters for a BF group shaper node. */

		struct nss_shaper_config_fifo_param fifo_param;
			/**< Configure a FIFO shaper node. */

		struct nss_shaper_config_wrr_attach wrr_attach;
			/**< Attach a shaper node to a WRR shaper node. */
		struct nss_shaper_config_wrr_detach wrr_detach;
			/**< Detach a shaper node from a WRR shaper node. */
		struct nss_shaper_config_wrr_param wrr_param;
			/**< Configuration parameters for a WRR shaper node . */
		struct nss_shaper_config_wrr_group_attach wrr_group_attach;
			/**< Attach a shaper node to a WRR group shaper node. */
		struct nss_shaper_config_wrr_group_param wrr_group_param;
			/**< Configure a WRR group shaper node with a quantum value. */
		struct nss_shaper_config_htb_attach htb_attach;
			/**< Attach a shaper node to an HTB shaper node. */
		struct nss_shaper_config_htb_group_attach htb_group_attach;
			/**< Attach a shaper node to an HTB group shaper node. */
		struct nss_shaper_config_htb_group_detach htb_group_detach;
			/**< Detach a shaper node from an HTB group shaper node. */
		struct nss_shaper_config_htb_group_param htb_group_param;
			/**< Configuration parameters for an HTB group shaper node. */
		struct nss_shaper_config_wred_param wred_param;
			/**< Configuration parameters for a WRED shaper node. */
		struct nss_shaper_config_ppe_sn_attach ppe_sn_attach;
			/**< Attach a shaper node to a PPE shaper node. */
		struct nss_shaper_config_ppe_sn_detach ppe_sn_detach;
			/**< Detach a shaper node from a PPE shaper node. */
		struct nss_shaper_config_ppe_sn_param ppe_sn_param;
			/**< Configuration parameters for a PPE shaper node. */
	} snc;	/**< Types of shaper node configuration messages. */
};

/**
 * nss_shaper_node_codel_fq_stats_delta
 *	CoDel flow queue mode statistics sent as deltas.
 */
struct nss_shaper_node_codel_fq_stats_delta {
	uint32_t new_flow_cnt;		/**< Total number of new flows seen. */
	uint32_t ecn_mark_cnt;		/**< Number of packets marked with ECN. */
};

/**
 * nss_shaper_node_codel_fq_stats
 *      CoDel flow queue mode statistics.
 */
struct nss_shaper_node_codel_fq_stats {
	struct nss_shaper_node_codel_fq_stats_delta delta;
					/**< CoDel flow queue statistics sent as deltas. */
	uint32_t new_flows_len;		/**< Current number of new flows. */
	uint32_t old_flows_len;		/**< Current number of old flows. */
	uint32_t maxpacket;		/**< Largest packet seen so far. */
};

/**
 * nss_shaper_node_codel_sq_stats
 *      CoDel single queue mode statistics.
 */
struct nss_shaper_node_codel_sq_stats {
	/**
	 * Maximum amount of time (in milliseconds) that a packet was in this shaper
	 * node before being dequeued.
	 */
        uint32_t packet_latency_peak_msec_dequeued;

	/**
	 * Maximum amount of time (in milliseconds) that a packet was in this shaper
	 * node before being dropped.
	 */
        uint32_t packet_latency_peak_msec_dropped;
};

/**
 * nss_shaper_node_codel_stats
 *      CoDel shaper node statistics.
 */
struct nss_shaper_node_codel_stats {
        struct nss_shaper_node_codel_sq_stats sq;   /**< Single queue mode statistics. */
        struct nss_shaper_node_codel_fq_stats fq;   /**< Flow queue mode statistics. */
};

/**
 * nss_shaper_node_stats_delta
 *	Statistics that are sent as deltas.
 */
struct nss_shaper_node_stats_delta {
	uint32_t enqueued_bytes;	/**< Bytes enqueued successfully. */
	uint32_t enqueued_packets;	/**< Packets enqueued successfully. */

	/**
	 * Bytes dropped during an enqueue operation because of node limits.
	 */
	uint32_t enqueued_bytes_dropped;

	/**
	 * Packets dropped during an enqueue operation because of node limits.
	 */
	uint32_t enqueued_packets_dropped;

	uint32_t dequeued_bytes;
			/**< Bytes dequeued successfully from a shaper node. */
	uint32_t dequeued_packets;
			/**< Packets dequeued successfully from a shaper node. */

	/**
	 * Bytes dropped by this node during dequeuing (some nodes drop packets during
	 * dequeuing rather than enqueuing).
	 */
	uint32_t dequeued_bytes_dropped;

	/**
	 * Packets dropped by this node during dequeuing (some nodes drop packets during
	 * dequeuing rather than enqueuing).
	 */
	uint32_t dequeued_packets_dropped;

	/**
	 * Number of times any queue limit was overrun, leading to packet drops.
	 */
	uint32_t queue_overrun;

	uint32_t unused[4];		/**< Reserved for future statistics expansion. */
};

/**
 * nss_shaper_node_stats
 *	Common shaper node statistics.
 */
struct nss_shaper_node_stats {
	uint32_t qlen_bytes;	/**< Total size of packets waiting in the queue. */
	uint32_t qlen_packets;	/**< Number of packets waiting in the queue. */
	uint32_t unused[4];	/**< Reserved for future statistics expansion. */
	struct nss_shaper_node_stats_delta delta;
				/**< Statistics that are sent as deltas. */
};

/**
 * nss_shaper_node_stats_response
 *	Statistics response for shaper nodes.
 */
struct nss_shaper_node_stats_response {
	struct nss_shaper_node_stats sn_stats;	/**< Common shaper node statistics. */

	/**
	 * All shaper nodes that need to maintain unique statistics need
	 * to add their statistics structure here.
	 */
	union {
		struct nss_shaper_node_codel_stats codel;
						/**< CoDel specific statistics. */
	} per_sn_stats;				/**< Shaper specific statistics. */
};

/**
 * nss_shaper_node_stats_get
 *	Statistics of a shaper node.
 */
struct nss_shaper_node_stats_get {

	/*
	 * Request
	 */
	uint32_t qos_tag;	/**< QoS tag of the shaper node. */

	/*
	 * Response
	 */
	struct nss_shaper_node_stats_response response;
				/**< Shaper node statistics response */
};

/**
 * nss_shaper_configure
 *	Configuration message for a shaper node.
 */
struct nss_shaper_configure {
	nss_shaper_config_type_t request_type;		/**< Message is a request. */
	nss_shaper_response_type_t response_type;	/**< Message is a response. */

	/**
	 * Types of configuration messages for a shaper node.
	 */
	union {
		struct nss_shaper_config_alloc_shaper_node alloc_shaper_node;
				/**< Allocate a shaper node in the NSS interface. */
		struct nss_shaper_config_free_shaper_node free_shaper_node;
				/**< Free a shaper node from the NSS interface. */
		struct nss_shaper_config_set_default_node set_default_node;
				/**< Set a shaper node as the default node for a queue. */
		struct nss_shaper_config_set_root_node set_root_node;
				/**< Set a shaper node as the root shaper nod. */
		struct nss_shaper_config_set_hybrid_mode set_hybrid_mode;
				/**< Set a shaper to operate in Hybrid mode. */
		struct nss_shaper_node_config shaper_node_config;
				/**< Configuration message for any type of shaper node. */
		struct nss_shaper_node_stats_get shaper_node_stats_get;
				/**< Statistics for a shaper node. */
	} msg;			/**< Types of configuration messages. */
};

/**
 * Registrant callback to receive shaper bounced packets
 *
 * @datatypes
 * sk_buff
 *
 * @param[in] app_data  Pointer to the application context of the message.
 * @param[in] skb       Pointer to the data socket buffer.
 */
typedef void (*nss_shaper_bounced_callback_t)(void *app_data, struct sk_buff *skb);

/**
 * nss_shaper_register_shaping
 *	Registers a shaper node with the NSS interface for basic shaping operations.
 *
 * @return
 * Pointer to the NSS core context.
 */
extern void *nss_shaper_register_shaping(void);

/**
 * nss_shaper_unregister_shaping
 *	Deregisters a shaper node from the NSS interface.
 *
 * @param[in] ctx  Pointer to the NSS context.
 *
 * @dependencies
 * The shaper node must have been previously registered.
 */
extern void nss_shaper_unregister_shaping(void *ctx);

/**
 * nss_shaper_register_shaper_bounce_interface
 *	Registers a shaper bounce interface with the NSS interface for receiving
 *	shaper-bounced packets.
 *
 * @datatypes
 * nss_shaper_bounced_callback_t \n
 * module
 *
 * @param[in] if_num    NSS interface number.
 * @param[in] cb        Callback function for the message. This callback is
 *                      invoked when the NSS returns a sk_buff after shaping.
 * @param[in] app_data  Pointer to the application context of the message.
 *                      This context is passed to the callback together with the
 *                      sk_buff to provide context to the registrant (state).
 * @param[in] owner     Pointer to the kernel module. The module is held until it deregisters.
 *
 * @return
 * Pointer to the NSS core context.
 */
extern void *nss_shaper_register_shaper_bounce_interface(uint32_t if_num, nss_shaper_bounced_callback_t cb, void *app_data, struct module *owner);

/**
 * nss_shaper_unregister_shaper_bounce_interface
 *	Deregisters a shaper bounce interface from the NSS interface.
 *
 * @param[in] if_num  NSS interface number.
 *
 * @return
 * None.
 *
 * @dependencies
 * The shaper bounce interface must have been previously registered.
 */
extern void nss_shaper_unregister_shaper_bounce_interface(uint32_t if_num);

/**
 * nss_shaper_register_shaper_bounce_bridge
 *	Registers a shaper bounce bridge with the NSS interface for receiving
 *	shaper-bounced packets.
 *
 * @datatypes
 * nss_shaper_bounced_callback_t \n
 * module
 *
 * @param[in] if_num    NSS interface number.
 * @param[in] cb        Callback function for the message. This callback is
 *                      invoked when the NSS returns a sk_buff after shaping.
 * @param[in] app_data  Pointer to the application context of the message.
 *                      This context is passed to the callback together with the
 *                      sk_buff to provide context to the registrant (state).
 * @param[in] owner     Pointer to the kernel module.
 *
 * @return
 * Pointer to the NSS core context.
 */
extern void *nss_shaper_register_shaper_bounce_bridge(uint32_t if_num, nss_shaper_bounced_callback_t cb, void *app_data, struct module *owner);

/**
 * nss_shaper_unregister_shaper_bounce_bridge
 *	Deregisters a shaper bounce bridge from the NSS interface.
 *
 * @param[in] if_num  NSS interface number.
 *
 * @return
 * None.
 *
 * @dependencies
 * The shaper bounce bridge must have been previously registered.
 */
extern void nss_shaper_unregister_shaper_bounce_bridge(uint32_t if_num);

/**
 * nss_shaper_bounce_interface_packet
 *	Issues a packet for shaping via a bounce operation.
 *
 * @datatypes
 * sk_buff
 *
 * @param[in]     ctx     Pointer to the NSS context provided during registration.
 * @param[in]     if_num  NSS interface number.
 * @param[in]     skb     Pointer to the data socket buffer.
 *
 * @return
 * Success or failure.
 */
extern nss_tx_status_t nss_shaper_bounce_interface_packet(void *ctx, uint32_t if_num, struct sk_buff *skb);

/**
 * nss_shaper_bounce_bridge_packet
 *	Issues a packet for shaping via a bounce bridge.
 *
 * @datatypes
 * sk_buff
 *
 * @param[in]     ctx     Pointer to the NSS context provided during registration.
 * @param[in]     if_num  NSS interface number.
 * @param[in]     skb     Pointer to the data socket buffer.
 *
 * @return
 * Success or failure.
 */
extern nss_tx_status_t nss_shaper_bounce_bridge_packet(void *ctx, uint32_t if_num, struct sk_buff *skb);

/**
 * nss_shaper_config_send
 *	Sends a shaping configuration message.
 *
 * @datatypes
 * nss_shaper_configure
 *
 * @param[in]     ctx     Pointer to the NSS context.
 * @param[in]     config  Pointer to the configuration message.
 *
 * @return
 * Indication if the configuration message was issued.
 * @par
 * This indication does not mean the configuration message was successfully
 * processed. Success or failure is provided in the response issued to the
 * given callback function as specified in nss_shaper_configure.
 */
nss_tx_status_t nss_shaper_config_send(void *ctx, struct nss_shaper_configure *config);

/**
 * nss_shaper_get_device
 *	Gets the original device from probe.
 *
 * @return
 * Pointer to the device.
 */
extern struct device *nss_shaper_get_dev(void);

/**
 * @}
 */

#endif

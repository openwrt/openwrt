/*
 **************************************************************************
 * Copyright (c) 2015-2018, 2021, The Linux Foundation. All rights reserved.
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
 * @file nss_wifi.h
 *	NSS TO HLOS Wi-Fi interface definitions.
 */

#ifndef __NSS_WIFI_H
#define __NSS_WIFI_H

/**
 * @addtogroup nss_wifi_subsystem
 * @{
 */

#define NSS_WIFI_MGMT_DATA_LEN  128		/**< Size of the Wi-Fi management data. */
#define NSS_WIFI_FW_STATS_DATA_LEN  480		/**< Size of the firmware statictics data. */
#define NSS_WIFI_RAWDATA_MAX_LEN  64		/**< Maximum size of the raw Wi-Fi data. */
#define NSS_WIFI_TX_NUM_TOS_TIDS 8		/**< Number of TIDs. */
#define NSS_WIFI_PEER_STATS_DATA_LEN 232	/**< Size of the peer statistics data. */
#define NSS_WIFI_IPV6_ADDR_LEN 16		/**< Size of the IPv6 address. */
#define NSS_WIFI_MAX_RSSI_CHAINS 4		/**< Maximum number of RSSI chains. */
#define NSS_WIFI_WME_NUM_AC 4			/**< Number of ACs. */

/**
 * Maximum number of Wi-Fi peers per radio as a sum of
 * maximum number of station peers (513),
 * maximum numbero of AP VAP peers (16), and
 * maximum number of monitor VAP peers (1).
 */
#define NSS_WIFI_MAX_PEER 530

/**
 * nss_wifi_metadata_types
 *	Wi-Fi interface request and response message types.
 */
enum nss_wifi_metadata_types {
	NSS_WIFI_INIT_MSG,
	NSS_WIFI_POST_RECV_MSG,
	NSS_WIFI_HTT_INIT_MSG,
	NSS_WIFI_TX_INIT_MSG,
	NSS_WIFI_RAW_SEND_MSG,
	NSS_WIFI_MGMT_SEND_MSG,
	NSS_WIFI_WDS_PEER_ADD_MSG,
	NSS_WIFI_WDS_PEER_DEL_MSG,
	NSS_WIFI_STOP_MSG,
	NSS_WIFI_RESET_MSG,
	NSS_WIFI_STATS_MSG,
	NSS_WIFI_PEER_FREELIST_APPEND_MSG,
	NSS_WIFI_RX_REORDER_ARRAY_FREELIST_APPEND_MSG,
	NSS_WIFI_SEND_PEER_MEMORY_REQUEST_MSG,
	NSS_WIFI_SEND_RRA_MEMORY_REQUEST_MSG,
	NSS_WIFI_FW_STATS_MSG,
	NSS_WIFI_MONITOR_FILTER_SET_MSG,
	NSS_WIFI_PEER_BS_STATE_MSG,
	NSS_WIFI_MSDU_TTL_SET_MSG,
	NSS_WIFI_RX_VOW_EXTSTATS_SET_MSG,
	NSS_WIFI_PKTLOG_CFG_MSG,
	NSS_WIFI_ENABLE_PERPKT_TXSTATS_MSG,
	NSS_WIFI_IGMP_MLD_TOS_OVERRIDE_MSG,
	NSS_WIFI_OL_STATS_CFG_MSG,
	NSS_WIFI_OL_STATS_MSG,
	NSS_WIFI_TX_QUEUE_CFG_MSG,
	NSS_WIFI_TX_MIN_THRESHOLD_CFG_MSG,
	NSS_WIFI_DBDC_PROCESS_ENABLE_MSG,
	NSS_WIFI_PRIMARY_RADIO_SET_MSG,
	NSS_WIFI_FORCE_CLIENT_MCAST_TRAFFIC_SET_MSG,
	NSS_WIFI_STORE_OTHER_PDEV_STAVAP_MSG,
	NSS_WIFI_STA_KICKOUT_MSG,
	NSS_WIFI_WNM_PEER_RX_ACTIVITY_MSG,
	NSS_WIFI_PEER_STATS_MSG,
	NSS_WIFI_WDS_VENDOR_MSG,
	NSS_WIFI_TX_CAPTURE_SET_MSG,
	NSS_WIFI_ALWAYS_PRIMARY_SET_MSG,
	NSS_WIFI_FLUSH_HTT_CMD_MSG,
	NSS_WIFI_CMD_MSG,
	NSS_WIFI_ENABLE_OL_STATSV2_MSG,
	NSS_WIFI_OL_PEER_TIME_MSG,
	NSS_WIFI_PEER_SET_VLAN_ID_MSG,
	NSS_WIFI_PEER_ISOLATION_MSG,
	NSS_WIFI_MAX_MSG
};

/*
 * wifi_error_types
 *	Wi-Fi error types.
 */
enum wifi_error_types {
	NSS_WIFI_EMSG_NONE = 0,
	NSS_WIFI_EMSG_UNKNOWN,
	NSS_WIFI_EMSG_MGMT_DLEN,
	NSS_WIFI_EMSG_MGMT_SEND,
	NSS_WIFI_EMSG_CE_INIT_FAIL,
	NSS_WIFI_EMSG_PDEV_INIT_FAIL,
	NSS_WIFI_EMSG_HTT_INIT_FAIL,
	NSS_WIFI_EMSG_PEER_ADD,
	NSS_WIFI_EMSG_WIFI_START_FAIL,
	NSS_WIFI_EMSG_STATE_NOT_RESET,
	NSS_WIFI_EMSG_STATE_NOT_INIT_DONE,
	NSS_WIFI_EMSG_STATE_NULL_CE_HANDLE,
	NSS_WIFI_EMSG_STATE_NOT_CE_READY,
	NSS_WIFI_EMSG_STATE_NOT_HTT_READY,
	NSS_WIFI_EMSG_FW_STATS_DLEN,
	NSS_WIFI_EMSG_FW_STATS_SEND,
	NSS_WIFI_EMSG_STATE_TX_INIT_FAILED,
	NSS_WIFI_EMSG_IGMP_MLD_TOS_OVERRIDE_CFG,
	NSS_WIFI_EMSG_PDEV_INVALID,
	NSS_WIFI_EMSG_OTHER_PDEV_STAVAP_INVALID,
	NSS_WIFI_EMSG_HTT_SEND_FAIL,
	NSS_WIFI_EMSG_CE_RING_INIT,
	NSS_WIFI_EMSG_NOTIFY_CB,
	NSS_WIFI_EMSG_PEERID_INVALID,
	NSS_WIFI_EMSG_PEER_INVALID,
	NSS_WIFI_EMSG_UNKNOWN_CMD,
	NSS_WIFI_EMSG_MAX,
};

/**
 * nss_wifi_ext_data_pkt_type
 *	Exception types for Wi-Fi extended data.
 */
enum nss_wifi_ext_data_pkt_type {
	NSS_WIFI_RX_EXT_INV_PEER_TYPE,
	NSS_WIFI_RX_EXT_PKTLOG_TYPE,
	NSS_WIFI_RX_STATS_V2_EXCEPTION,
	NSS_WIFI_RX_MGMT_NULL_TYPE,
	NSS_WIFI_RX_EXT_MAX_TYPE,
};

/**
 * nss_wifi_cmd
 *	Wi-Fi commands.
 */
enum nss_wifi_cmd {
	NSS_WIFI_FILTER_NEIGH_PEERS_CMD,
	NSS_WIFI_MAX_CMD
};

/**
 * nss_wifi_ce_ring_state_msg
 *	Internal state information for the copy engine ring.
 */
struct nss_wifi_ce_ring_state_msg {
	uint32_t nentries;		/**< Number of entries in the copy engine ring. */
	uint32_t nentries_mask;		/**< Number of entry masks. */
	uint32_t sw_index;		/**< Initial software index. */
	uint32_t write_index;		/**< Initial write index. */
	uint32_t hw_index;		/**< Initial hardware index. */
	uint32_t base_addr_CE_space;
			/**< Physical address of the copy engine hardware ring. */
	uint32_t base_addr_owner_space;
			/**< Virtual address of the copy engine hardware ring. */
};

/**
 * nss_wifi_ce_state_msg
 *	Internal state information for the copy engine.
 */
struct nss_wifi_ce_state_msg {
	struct nss_wifi_ce_ring_state_msg src_ring;
			/**< Source ring information. */
	struct nss_wifi_ce_ring_state_msg dest_ring;
			/**< Destination ring information. */
	uint32_t ctrl_addr;
			/**< Control address relative to PCIe BAR. */
};

/**
 * nss_wifi_init_msg
 *	Wi-Fi initialization data.
 */
struct nss_wifi_init_msg {
	uint32_t radio_id ;		/**< Radio index. */
	uint32_t pci_mem;		/**< PCI memory address. */
	uint32_t target_type;		/**< Wi-Fi target type. */
	uint32_t mu_mimo_enhancement_en;
			/**< Enable MU-MIMO enhancement. */
	struct nss_wifi_ce_state_msg ce_tx_state;
			/**< Transmit copy engine information. */
	struct nss_wifi_ce_state_msg ce_rx_state;
			/**< Receive copy engine information. */

	/**
	 * Indicates whether network processing is bypassed for this radio.
	 */
	uint32_t bypass_nw_process;
};

/**
 * nss_wifi_htt_init_msg
 *	Wi-Fi Host-to-Target (HTT) initialization data.
 */
struct nss_wifi_htt_init_msg {
	uint32_t radio_id;		/**< Radio index. */
	uint32_t ringsize;		/**< WLAN hardware MAC ring size. */
	uint32_t fill_level;		/**< Initial fill level. */
	uint32_t paddrs_ringptr;
			/**< Physical address of the WLAN MAC hardware ring. */
	uint32_t paddrs_ringpaddr;
			/**< Virtual address of the WLAN MAC hardware ring. */
	uint32_t alloc_idx_vaddr;
			/**< Virtual address of the hardware ring index. */
	uint32_t alloc_idx_paddr;
			/**< Physical address of the hardware ring index. */
};

/**
 * nss_wifi_tx_init_msg
 *	Wi-Fi Tx initialization data.
 */
struct nss_wifi_tx_init_msg {
	uint32_t radio_id;		/**< Radio index. */
	uint32_t desc_pool_size;	/**< Number of descriptor pools allocated. */
	uint32_t tx_desc_array;
			/**< Host-initialized software WLAN descriptor pool memory. */
	uint32_t wlanextdesc_addr;
			/**< Starting address of the WLAN MAC extenstion descriptor pool. */
	uint32_t wlanextdesc_size;
			/**< Descriptor size of the WLAN MAC extenstion. */

	/**
	 * Starting virtual address, as shared by the Wi-Fi firmware, for HTT Tx descriptor memory.
	 */
	uint32_t htt_tx_desc_base_vaddr;

	/**
	 * HTT Tx descriptor memory start physical address as shared by Wi-Fi firmware.
	 */
	uint32_t htt_tx_desc_base_paddr;

	uint32_t htt_tx_desc_offset;
			/**< Descriptor size of the firmware shared HTT Tx. */
	uint32_t pmap_addr;
			/**< Firmware shared peer or TID map. */
};

/**
 * nss_wifi_tx_queue_cfg_msg
 *	Wi-Fi Tx queue configuration.
 */
struct nss_wifi_tx_queue_cfg_msg {
	uint32_t size;			/**< Size of the Tx queue. */
	uint32_t range;			/**< Peer range. */
};

/**
 * nss_wifi_tx_min_threshold_cfg_msg
 *	Minimum threshold configuration data for the Wi-Fi Tx queue.
 */
struct nss_wifi_tx_min_threshold_cfg_msg {
	uint32_t min_threshold;		/**< Minimum threshold value of Tx queue. */
};

/**
 * nss_wifi_rawsend_msg
 *	Information for Wi-Fi raw data.
 */
struct nss_wifi_rawsend_msg {
	uint32_t radio_id ;		/**< Radio index. */
	uint32_t len;			/**< Size of the raw data. */
	uint32_t array[NSS_WIFI_RAWDATA_MAX_LEN];
					/**< Array of raw data. */
};

/**
 * nss_wifi_mgmtsend_msg
 *	Information for Wi-Fi management data.
 */
struct nss_wifi_mgmtsend_msg {
	uint32_t desc_id;		/**< Descriptor index. */
	uint32_t len;			/**< Size of the management data. */
	uint8_t array[NSS_WIFI_MGMT_DATA_LEN];
					/**< Array of management data. */
};

/**
 *  nss_wifi_fw_stats_msg
 *	Information for Wi-Fi firmware statistics.
 */
struct nss_wifi_fw_stats_msg {
	uint32_t len;			/**< Size of the statistics data. */
	uint8_t array[NSS_WIFI_FW_STATS_DATA_LEN];
					/**< Array of statistics data. */
};

/**
 *  nss_wifi_monitor_set_filter_msg
 *	Wi-Fi Monitor mode for setting filter messages.
 */
struct nss_wifi_monitor_set_filter_msg {
	uint32_t filter_type;		/**< Type of Monitor mode filter. */
};

/**
 * nss_wifi_wds_peer_msg
 *	Wi-Fi WDS peer-specific message.
 */
struct nss_wifi_wds_peer_msg {
	uint8_t dest_mac[ETH_ALEN];	/**< MAC address of the destination. */
	uint8_t reserved[2];		/**< Reserved for 4-byte alignment padding. */
	uint8_t peer_mac[ETH_ALEN];	/**< MAC address of the base peer. */
	uint8_t reserved1[2];		/**< Reserved for 4-byte alignment padding. */
};

/**
 * nss_wifi_tx_capture_msg
 *	Wi-Fi Tx data capture configuration.
 */
struct nss_wifi_tx_capture_msg {
	uint32_t tx_capture_enable;	/**< Enable or disable Tx data capture. */
};

/**
 * nss_wifi_reset_msg
 *	Message to reset the Wi-Fi Radio.
 */
struct nss_wifi_reset_msg {
	uint32_t radio_id;		/**< Radio index. */
};

/**
 * nss_wifi_stop_msg
 *	Message to stop the Wi-Fi Radio.
 */
struct nss_wifi_stop_msg {
	uint32_t radio_id;		/**< Radio index. */
};

/**
 * nss_wifi_pktlog_cfg_msg
 *	Configuration information for a Wi-Fi packet log.
 */
struct nss_wifi_pktlog_cfg_msg {
	uint32_t enable;		/**< Enables or disables a packet log. */
	uint32_t bufsize;		/**< Size of the packet log buffer. */
	uint32_t hdrsize;		/**< Size of the packet log header. */
	uint32_t msdu_id_offset;	/**< Offset for the MSDU ID in the message. */
};

/**
 * nss_wifi_ol_stats_cfg_msg
 *	Wi-Fi offload statistics configuration.
 */
struct nss_wifi_ol_stats_cfg_msg {
	uint32_t stats_cfg;	/**< Enable or disable offload statistics configuration. */
};

/**
 * nss_wifi_enable_perpkt_txstats_msg
 *	Wi-Fi per-packet Tx statistics configuration.
 */
struct nss_wifi_enable_perpkt_txstats_msg {
	uint32_t perpkt_txstats_flag;	/**< Enable or disable Tx statistics. */
};

/**
 * nss_wifi_peer_txtime_stats
 *	Peer Tx timestamp statistics per TID.
 */
struct nss_wifi_peer_txtime_stats {
	uint32_t sum_tx;	/**< Sum of sojourn for each packet. */
	uint32_t sum_msdus;	/**< Number of MSDU per peer per TID. */
};

/**
 * nss_wifi_peer_tstamp_stats
 *	Peer ID and timestamp statistics per TID.
 */
struct nss_wifi_peer_tstamp_stats {
	uint32_t peer_id;			/**< TID value. */
	struct nss_wifi_peer_txtime_stats sum[NSS_WIFI_TX_NUM_TOS_TIDS];
							/**< Timestamps. */
	uint32_t avg[NSS_WIFI_TX_NUM_TOS_TIDS];	/**< Exponential weighted average. */
};

/**
 * nss_wifi_ol_peer_time_msg
 *	NSS Wi-Fi Tx timestamp message for n number of peers.
 */
struct nss_wifi_ol_peer_time_msg {
	uint32_t npeers;			/**< Number of peers. */
	struct nss_wifi_peer_tstamp_stats tstats[1];
						/**< One instance of struct. */
};

/**
 * nss_wifi_enable_ol_statsv2
 *	Wi-Fi enable/disable send packet to host.
 */
struct nss_wifi_enable_ol_statsv2 {
	uint32_t enable_ol_statsv2;	/**< Flag to send packet to host. */
};

/**
 * nss_wifi_dbdc_process_enable_msg
 *	Wi-Fi DBDC repeater process configuration.
 */
struct nss_wifi_dbdc_process_enable_msg {
	uint32_t dbdc_process_enable;	/**< Enable or disable the DBDC process. */
};

/**
 * nss_wifi_primary_radio_set_msg
 *	Wi-Fi primary radio configuration message.
 */
struct nss_wifi_primary_radio_set_msg {
	/**
	 * Enable/Disable Flag to set the current radio as primary.
	 */
	uint32_t flag;
};

/**
 * nss_wifi_always_primary_set_msg
 *	Always set the Wi-Fi primary radio.
 *
 * The primary radio is set using the nss_wifi_primary_radio_set_msg flag.
 * When the nss_wifi_always_primary_set_msg flag is set:
 * - Tx -- Do not drop a unicast packet on the secondary station the VAP. Instead, give that
 *   packet to the primary station the VAP.
 * - Rx -- Do not drop a received unicast packet on the secondary station the VAP. Instead,
 *   give that packet to the bridge by changing the SKB device as the primary station
 *   VAP.
 *
 * Primary usage of this feature is to avoid a loopback.
 */
struct nss_wifi_always_primary_set_msg {
	/**
	 * Always use the primary radio for Tx and Rx in the DBDC repeater process.
	 */
	uint32_t flag;
};

/**
 * nss_wifi_force_client_mcast_traffic_set_msg
 *	Wi-Fi message to set the client multi-cast traffic for a radio.
 */
struct nss_wifi_force_client_mcast_traffic_set_msg {
	uint32_t flag;		/**< Flag to force set the multi-cast traffic in a radio. */
};

/**
 * wifi_store_other_pdev_stavap_msg
 *	Store the other radio's station vap.
 */
struct nss_wifi_store_other_pdev_stavap_msg {
	int stavap_ifnum;	/**< Station VAP interface number of the other radio. */
};

/**
 * nss_wifi_pl_metadata
 *	Wi-Fi packet log metadata.
 */
struct nss_wifi_pl_metadata {
	uint32_t len;		/**< Length of single buffer in MSDU. */
	uint32_t msdu_len;	/**< Total MSDU length. */
	uint16_t da_tail;	/**< Destination address tail bytes. */
	uint16_t sa_tail;	/**< Source address tail bytes. */
	uint8_t vdev_id;	/**< Virtual device ID. */
	uint8_t res1;		/**< Reserved for alignment. */
	uint16_t res2;		/**< Reserved for alignment. */
};

/**
 * nss_wifi_rx_ext_metadata
 *	Wi-Fi Rx extended data plane metadata.
 */
struct nss_wifi_rx_ext_metadata{
	uint16_t peer_id;	/**< ID of associated Peer. */
	uint8_t htt_rx_status;	/**< Rx status of the HTT. */
	uint8_t type;		/**< Reserved for 4 byte alignment. */
};

/**
 * nss_wifi_mc_enhance_stats
 *	Wi-Fi multicast enhancement statistics.
 */
struct nss_wifi_mc_enhance_stats {
	uint32_t rcvd;	/**< Number of multicast frames received for conversion. */

	/**
	 * Number of unicast frames sent as part of multicast enhancement conversion.
	 */
	uint32_t ucast_converted;

	/**
	 * Number of multicast enhancement frames dropped because of an allocation
	 * failure.
	 */
	uint32_t alloc_fail;

	/**
	 * Number of multicast enhancement frames dropped because of an enqueue failure.
	 */
	uint32_t enqueue_fail;

	/**
	 * Number of multicast enhancement frames dropped because of a copy failure.
	 */
	uint32_t copy_fail;

	/**
	 * Number of multicast enhancement frames dropped because of a peer flow control
	 * send failure.
	 */
	uint32_t peer_flow_ctrl_send_fail;

	/**
	 * Number of multicast enhancement frames dropped when the destination MAC
	 * address is the same as the source MAC address.
	 */
	uint32_t loopback_err;

	/**
	 * Number of multicast enhancement buffer frames dropped because of an empty
	 * destination MAC address.
	 */
	uint32_t dst_addr_err;
};

/**
 * nss_wifi_stats_sync_msg
 *	Wi-Fi synchronization statistics.
 */
struct nss_wifi_stats_sync_msg {
	struct nss_cmn_node_stats node_stats;	/**< Common node statistics. */
	uint32_t tx_transmit_dropped;
			/**< Number of packets dropped during transmission. */
	uint32_t tx_transmit_completions;
			/**< Number of packets for which Tx completions are received. */
	uint32_t tx_mgmt_rcv_cnt;
			/**< Number of management packets received from the host for Tx. */
	uint32_t tx_mgmt_pkts;
			/**< Number of management packets transmitted over Wi-Fi. */

	/**
	 * Number of management packets dropped because of a Tx failure.
	 */
	uint32_t tx_mgmt_dropped;

	/**
	 * Number of management packets for which Tx completions are received.
	 */
	uint32_t tx_mgmt_completions;

	/**
	 * Number of packets for which an Tx enqueue failed because of an invalid peer.
	 */
	uint32_t tx_inv_peer_enq_cnt;

	/**
	 * Number of packets with an invalid peer ID received from Wi-Fi.
	 */
	uint32_t rx_inv_peer_rcv_cnt;

	uint32_t rx_pn_check_failed;
			/**< Number of Rx packets that failed a packet number check. */

	/**
	 * Number of Rx packets that the Wi-Fi driver successfully processed.
	 */
	uint32_t rx_pkts_deliverd;

	/**
	 * Number of Rx bytes that the Wi-Fi driver successfully processed.
	 */
	uint32_t rx_bytes_deliverd;

	uint32_t tx_bytes_transmit_completions;
			/**< Number of bytes for which Tx completions are received. */

	/**
	 * Number of unaligned data packets that were received from Wi-Fi and dropped.
	 */
	uint32_t rx_deliver_unaligned_drop_cnt;

	uint32_t tidq_enqueue_cnt[NSS_WIFI_TX_NUM_TOS_TIDS];
			/**< Number of packets enqueued to TID Queue (TIDQ). */
	uint32_t tidq_dequeue_cnt[NSS_WIFI_TX_NUM_TOS_TIDS];
			/**< Number of packets dequeued from  TIDQ. */
	uint32_t tidq_enqueue_fail_cnt[NSS_WIFI_TX_NUM_TOS_TIDS];
			/**< Number of enqueue failures. */
	uint32_t tidq_ttl_expire_cnt[NSS_WIFI_TX_NUM_TOS_TIDS];
			/**< Number of packets expired from TIDQ. */
	uint32_t tidq_dequeue_req_cnt[NSS_WIFI_TX_NUM_TOS_TIDS];
			/**< Number of dequeue requests from the Wi-Fi firmware. */
	uint32_t total_tidq_depth;
			/**< Current queue Depth. */

	/**
	 * Total number of HTT fetch messages received from the Wi-Fi firmware.
	 */
	uint32_t rx_htt_fetch_cnt;

	/**
	 * Total number of packets that bypassed TIDQ and are sent to the Wi-Fi
	 * firmware.
	 */
	uint32_t total_tidq_bypass_cnt;

	/**
	 * Total number of packets dropped because of a global queue full condition.
	 */
	uint32_t global_q_full_cnt;

	/**
	 * Total number of packets dropped because of a TID queue full condition.
	 */
	uint32_t tidq_full_cnt;

	struct nss_wifi_mc_enhance_stats mc_enhance_stats;
			/**< Multicast enhancement statistics. */

	/**
	 * Number of times a group entry was not present for multicast enhancement.
	 */
	uint32_t mc_enhance_group_entry_miss;

	/**
	 * Number of times a deny list was hit during multicast enhancement.
	 */
	uint32_t mc_enhance_denylist_hit;
};

/**
 * nss_wifi_peer_freelist_append_msg
 *	Information for creating a Wi-Fi peer freelist.
 */
struct nss_wifi_peer_freelist_append_msg {
	uint32_t addr;		/**< Starting address of peer freelist pool. */
	uint32_t length;	/**< Size of peer freelist pool. */
	uint32_t num_peers;	/**< Maximum peer entries supported in the pool. */
};

/**
 * nss_wifi_rx_reorder_array_freelist_append_msg
 *	Information for creating a Wi-Fi TIDQ peer freelist array.
 */
struct nss_wifi_rx_reorder_array_freelist_append_msg {
	uint32_t addr;		/**< Starting address of the TIDQ freelist pool. */
	uint32_t length;	/**< Size of the TIDQ freelist pool. */

	/**
	 * Maximum number of Rx reorder array entries supported in the freelist pool.
	 */
	uint32_t num_rra;
};

/**
 * wifi_bs_peer_inactivity
 *	Active state information of the peer.
 */
struct nss_wifi_bs_peer_activity {
	uint16_t nentries;	/**< Number of entries in the peer ID array. */
	uint16_t peer_id[1];	/**< Array holding the peer IDs. */
};

/**
 * nss_wifi_msdu_ttl_set_msg
 *	Information for setting the Wi-Fi MSDU time-to-live value.
 */
struct nss_wifi_msdu_ttl_set_msg {
	uint32_t msdu_ttl;	/**< TTL value to be set. */
};

/**
 * nss_wifi_rx_vow_extstats_set_msg
 *	VoW extended statitics set.
 */
struct nss_wifi_rx_vow_extstats_set_msg {
	uint32_t vow_extstats_en;	/**< VoW extended statistics enable. */
};

/**
 * nss_wifi_igmp_mld_override_tos_msg
 *	Information for overriding TOS.
 */
struct nss_wifi_igmp_mld_override_tos_msg {
	uint8_t igmp_mld_ovride_tid_en;
			/**< Flag to enable TID override feature for IGMP/MLD configuration. */
	uint8_t igmp_mld_ovride_tid_val;
			/**< Value of TID to be overriden for IGMP/MLD. */
	uint8_t res[2];	/**< Reserved for 4-byte alignment. */
};

/**
 * nss_wifi_peer_ol_stats
 *	Wi-Fi offload statistics.
 */
struct nss_wifi_peer_ol_stats {
	uint32_t peer_id;	/**< ID of associated peer. */
	uint32_t seq_num;	/**< Sequence number of the PPDU. */
	uint32_t tx_unaggr;	/**< Number of unaggregated packets transmitted. */
	uint32_t tx_aggr;	/**< Number of aggregated packets transmitted. */
	uint32_t tx_mcast;	/**< Number of multicast packets sent. */
	uint32_t tx_ucast;	/**< Number of unicast packets sent. */
	uint32_t tx_data;	/**< Number data packets sent. */
	uint32_t tx_bytes;	/**< Number of bytes sent. */
	uint32_t tx_fail;	/**< Number of failed Tx packets. */
	uint32_t thrup_bytes;	/**< Number of throughput bytes. */
	uint32_t tx_bcast_pkts;	/**< Number of broadcast packets sent. */
	uint32_t tx_bcast_bytes;/**< Number of broadcast bytes sent. */
	uint32_t tx_mgmt;	/**< Number of Tx management frames. */
	uint32_t tx_wme[NSS_WIFI_WME_NUM_AC];
				/**< Data frames transmitted per AC. */
	uint32_t rx_wme[NSS_WIFI_WME_NUM_AC];
				/**< Data frames received per AC. */
	uint32_t ppdu_retries;	/**< Number of PPDU retries. */
	uint32_t rssi_chains[NSS_WIFI_MAX_RSSI_CHAINS];
				/**< Acknowledgment RSSI per chain. */
	uint32_t rx_msdus;	/**< Number of MSDUs received. */
	uint32_t rx_bytes;	/**< Number of bytes received. */
	uint32_t rx_mpdus;	/**< Number of MPDUs received. */
	uint32_t rx_retries;	/**< Number of MPDU retries. */
};

/**
 * nss_wifi_ol_stats_msg
 *	Wi-Fi offload statistics.
 */
struct nss_wifi_ol_stats_msg {
	uint32_t bawadv_cnt;	/**< Number of block-acknowledgment window advancements. */
	uint32_t bcn_cnt;	/**< Number of beacons. */
	uint32_t npeers;	/**< Number of peer statistics entries. */
	struct nss_wifi_peer_ol_stats peer_ol_stats[1];
				/**< Array to hold the peer statistics. */
};

/**
 * nss_wifi_sta_kickout_msg
 *	Station kickout message from NSS Firmware
 */
struct nss_wifi_sta_kickout_msg {
	uint32_t peer_id;	/**< Peer ID. */
};

/**
 * nss_wifi_peer_isolation_msg
 *	Peer isolation message
 */
struct nss_wifi_peer_isolation_msg {
	uint16_t peer_id;	/**< Peer ID. */
	uint16_t isolation;	/**< Isolation enabled/disabled. */
};

/**
 * nss_wifi_wnm_peer_rx_activity_msg
 *	Rx active state information for the peer.
 */
struct nss_wifi_wnm_peer_rx_activity_msg {
	uint16_t nentries;	/**< Number of entries. */

	/**
	 * Array to hold the peer IDs for which the activity is reported.
	 */
	uint16_t peer_id[NSS_WIFI_MAX_PEER];
};

/**
 * nss_wifi_append_metaheader
 * 	Append metaheader after pbuf->data for stats_v2.
 */
struct nss_wifi_append_statsv2_metahdr {
	uint32_t rxstatsmagic;	/**< Magic to be verified on host. */
	uint32_t seq_number;	/**< Sequence number of packets sent from NSS. */
	uint16_t peer_id;	/**< Peer ID of peer. */
	uint16_t num_msdus;	/**< Number of MSDU in PPDU. */
	uint16_t num_retries;	/**< Number of retries in PPDU. */
	uint16_t num_mpdus;	/**< Number of MPDU in PPDU. */
	uint32_t num_bytes;	/**< Number of bytes in PPDU. */
};

/**
 * nss_wifi_peer_stats_msg
 *	Wi-Fi peer statistics.
 */
struct nss_wifi_peer_stats_msg {
	uint32_t peer_id;		/**< Peer ID. */
	uint32_t tidq_byte_cnt[NSS_WIFI_TX_NUM_TOS_TIDS];
			/**< Number of bytes in each TIDQ. */
	uint32_t tidq_queue_max[NSS_WIFI_TX_NUM_TOS_TIDS];
			/**< Maximum depth for the TID queue. */
	uint32_t tidq_enqueue_cnt[NSS_WIFI_TX_NUM_TOS_TIDS];
			/**< Number of packets enqueued to the TIDQ. */
	uint32_t tidq_dequeue_cnt[NSS_WIFI_TX_NUM_TOS_TIDS];
			/**< Number of packets dequeued from the TIDQ. */
	uint32_t tidq_ttl_expire_cnt[NSS_WIFI_TX_NUM_TOS_TIDS];
			/**< Number of expired packets from the TIDQ. */
	uint32_t tidq_dequeue_req_cnt[NSS_WIFI_TX_NUM_TOS_TIDS];
			/**< Number of dequeue requests from the Wi-Fi firmware. */

	/**
	 * Total number of packets dropped because the TID queue is full.
	 */
	uint32_t tidq_full_cnt[NSS_WIFI_TX_NUM_TOS_TIDS];
};

/**
 * nss_wifi_wds_extn_peer_cfg_msg
 *	Configuration information when the WDS extension is enabled.
 */
struct nss_wifi_wds_extn_peer_cfg_msg {
	uint8_t mac_addr[ETH_ALEN];	/**< Mac address of the peer. */
	uint8_t wds_flags;		/**< WDS flags populated from the host. */
	uint8_t reserved;		/**< Alignment padding. */
	uint16_t peer_id;		/**< ID of the peer. */
};

/**
 * nss_wifi_cmd_msg
 *	Wi-Fi radio specific special commands to NSS Firmware
 */
struct nss_wifi_cmd_msg {
	uint32_t cmd;			/**< Type of command message. */
	uint32_t value;			/**< Value of the command. */
};

/**
 * nss_wifi_msg
 *	Data for sending and receiving Wi-Fi messages.
 */
struct nss_wifi_msg {
	struct nss_cmn_msg cm;		/**< Common message header. */

	/**
	 * Message Payload.
	 */
	union {
		struct nss_wifi_init_msg initmsg;
				/**< Wi-Fi Radio initialization message. */
		struct nss_wifi_stop_msg stopmsg;
				/**< Wi-Fi Radio stop message. */
		struct nss_wifi_reset_msg resetmsg;
				/**< Wi-Fi Radio reset message. */
		struct nss_wifi_htt_init_msg httinitmsg;
				/**< HTT initialization message. */
		struct nss_wifi_tx_init_msg pdevtxinitmsg;
				/**< Tx initialization message. */
		struct nss_wifi_rawsend_msg rawmsg;
				/**< Wi-Fi raw data send message. */
		struct nss_wifi_mgmtsend_msg mgmtmsg;
				/**< Wi-Fi management data send message. */
		struct nss_wifi_wds_peer_msg pdevwdspeermsg;
				/**< WDS peer-specific message. */
		struct nss_wifi_stats_sync_msg statsmsg;
				/**< Synchronization statistics. */
		struct nss_wifi_peer_freelist_append_msg peer_freelist_append;
				/**< Message for creating/appending peer freelist memory. */

		/**
		 * Message for creating/appending a reorder array for Wi-Fi Receive Defragmentation.
		 */
		struct nss_wifi_rx_reorder_array_freelist_append_msg rx_reorder_array_freelist_append;

		struct nss_wifi_fw_stats_msg fwstatsmsg;
				/**< Wi-Fi firmware statistics information message. */
		struct nss_wifi_monitor_set_filter_msg monitor_filter_msg;
				/**< Set the filter message for Monitor mode. */
		struct nss_wifi_bs_peer_activity peer_activity;
				/**< Message to get the active peer for a radio. */
		struct nss_wifi_msdu_ttl_set_msg msdu_ttl_set_msg;
				/**< Set MSDU time-to-live. */
		struct nss_wifi_rx_vow_extstats_set_msg vow_extstats_msg;
				/**< Enable VoW extended statistics message. */
		struct nss_wifi_pktlog_cfg_msg pcm_msg;
				/**< Packet log configuration message. */
		struct nss_wifi_enable_perpkt_txstats_msg ept_msg;
				/**< Enable or disable per-packet Tx statistics. */
		struct nss_wifi_igmp_mld_override_tos_msg wigmpmldtm_msg;
				/**< Message to enable TID override for IGMP/MLD. */
		struct nss_wifi_ol_stats_cfg_msg scm_msg;
				/**< Enable or disable offload statistics configuration. */
		struct nss_wifi_ol_stats_msg ol_stats_msg;
				/**< Offload statistics. */
		struct nss_wifi_tx_queue_cfg_msg wtxqcm;
				/**< Tx queue configuration. */

		/**
		 * Minimum threshold configuration data for the Tx queue.
		 */
		struct nss_wifi_tx_min_threshold_cfg_msg wtx_min_threshold_cm;

		struct nss_wifi_dbdc_process_enable_msg dbdcpe_msg;
				/**< Enable or disable the DBDC repeater process. */
		struct nss_wifi_primary_radio_set_msg wprs_msg;
				/**< Set the current radio as the primary radio. */
		struct nss_wifi_force_client_mcast_traffic_set_msg wfcmts_msg;
				/**< Message to force multicast traffic for a radio. */
		struct nss_wifi_store_other_pdev_stavap_msg wsops_msg;
				/**< Message to store the other radio's station vap. */
		struct nss_wifi_sta_kickout_msg sta_kickout_msg;
				/**< Station kickout message from NSS firmware. */
		struct nss_wifi_wnm_peer_rx_activity_msg wprm;
				/**< Rx activity for the peer. */
		struct nss_wifi_peer_stats_msg peer_stats_msg;
				/**< Peer statistics message. */
		struct nss_wifi_wds_extn_peer_cfg_msg wpeercfg;
				/**< Configuartion information message when the WDS extension is enabled. */
		struct nss_wifi_tx_capture_msg tx_capture_msg;
				/**< Enable or disable Tx data capture. */
		struct nss_wifi_always_primary_set_msg waps_msg;
				/**< Message to always set the current radio as primary radio. */
		struct nss_wifi_cmd_msg wcmdm;
				/**< Pdev command information. */
		struct nss_wifi_enable_ol_statsv2 wesh_msg;
				/**< Enable version 2 tx/rx stats. */
		struct nss_wifi_ol_peer_time_msg wopt_msg;
				/**< Send per peer/TID timestamp statistics to host. */
		struct nss_wifi_peer_isolation_msg isolation_msg;
				/**< Enable or disable peer isolation. */
	} msg; /**< Message Payload. */
};

/**
 * nss_wifi_get_context
 *	Gets the Wi-Fi context used in nss_gre_tx.
 *
 * @return
 * Pointer to the NSS core context.
 */
extern struct nss_ctx_instance *nss_wifi_get_context(void);

/**
 * nss_wifi_tx_msg
 *	Sends a Wi-Fi message to the NSS firmware.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_wifi_if_msg
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 * @param[in] msg      Pointer to the message data.
 *
 * @return
 * Status of the Tx operation.
 */
extern nss_tx_status_t nss_wifi_tx_msg(struct nss_ctx_instance *nss_ctx, struct nss_wifi_msg *msg);

/**
 * Callback function for receiving Wi-Fi messages.
 *
 * @datatypes
 * nss_wifi_msg
 *
 * @param[in] app_data  Pointer to the application context of the message.
 * @param[in] msg       Pointer to the message data.
 */
typedef void (*nss_wifi_msg_callback_t)(void *app_data, struct nss_wifi_msg *msg);

/**
 * Callback function for receiving Wi-Fi data.
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
typedef void (*nss_wifi_callback_t)(struct net_device *netdev, struct sk_buff *skb, struct napi_struct *napi);

/**
 * nss_register_wifi_if
 *	Registers the Wi-Fi interface with the NSS for sending and receiving messages.
 *
 * @datatypes
 * nss_wifi_callback_t \n
 * nss_wifi_msg_callback_t \n
 * net_device
 *
 * @param[in] if_num             NSS interface number.
 * @param[in] wifi_callback      Callback for the data.
 * @param[in] wifi_ext_callback  Callback for the extended data.
 * @param[in] event_callback     Callback for the message.
 * @param[in] netdev             Pointer to the associated network device.
 * @param[in] features           Data socket buffer types supported by this interface.
 *
 * @return
 * Pointer to the NSS core context.
 */
struct nss_ctx_instance *nss_register_wifi_if(uint32_t if_num, nss_wifi_callback_t wifi_callback,
						nss_wifi_callback_t wifi_ext_callback, nss_wifi_msg_callback_t event_callback, struct net_device *netdev, uint32_t features);

/**
 * nss_unregister_wifi_if
 *	Deregisters the Wi-Fi interface from the NSS.
 *
 * @param[in] if_num  NSS interface number.
 *
 * @return
 * None.
 */
void nss_unregister_wifi_if(uint32_t if_num);

/**
 * @}
 */

#endif /* __NSS_WIFI_H */

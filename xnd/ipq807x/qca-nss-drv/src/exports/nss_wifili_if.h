/*
 **************************************************************************
 * Copyright (c) 2017-2021, The Linux Foundation. All rights reserved.
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
  * @file nss_wifili_if.h
  *	NSS TO HLOS interface definitions.
  *	NOTE: Here we will use wifili as a reference to
  *	the IPQ807x Wi-Fi object.
  */
#ifndef __NSS_WIFILI_H
#define __NSS_WIFILI_H

 /**
  * @addtogroup nss_wifili_subsystem
  * @{
  */

#define NSS_WIFILI_MAX_SRNG_REG_GROUPS_MSG 2
				/**< Maximum srng (ring) register groups. */
#define NSS_WIFILI_MAX_NUMBER_OF_PAGE_MSG 32
				/**< Maximum number of pages allocated from host. */
#define NSS_WIFILI_MAX_TCL_DATA_RINGS_MSG 4
				/**< Maximum number of Transmit Classifier data ring for NSS. */
#define NSS_WIFILI_MAX_REO_DATA_RINGS_MSG 4
				/**< Maximum number of Rx reorder data ring for NSS. */
#define NSS_WIFILI_SOC_PER_PACKET_METADATA_OFFSET 4
				/**< Metadata area for storing Rx statistics. */
#define NSS_WIFILI_MAX_TXDESC_POOLS_MSG 4
				/**< Maximum number of Tx Descriptor software pools. */
#define NSS_WIFILI_MAX_TX_EXT_DESC_POOLS_MSG 4
				/**< Maximum number of Tx Descriptor Extended software pools. */
#define NSS_WIFILI_MAX_SOC_NUM 3
				/**< Maximum number of SoC devices. */
#define NSS_WIFILI_MAX_PDEV_NUM_MSG 3
				/**< Maximum number of pdev devices. */
#define NSS_WIFILI_MAX_MCS 12
				/**< Maximum Modulaton And Coding Scheme (MCS) count. */
#define NSS_WIFILI_MAX_MCS_11A 8
				/**< Maximum MCS for 11a mode. */
#define NSS_WIFILI_MAX_MCS_11B 7
				/**< Maximum MCS for 11b mode. */
#define NSS_WIFILI_MAX_MCS_11AC 10
				/**< Maximum MCS for 11ac mode. */
#define NSS_WIFILI_MAX_MCS_11AX 10
				/**< Maximum MCS for 11ax mode. */
#define NSS_WIFILI_SS_COUNT 8
				/**< Maximum spatial streams count. */
#define NSS_WIFILI_SUPPORTED_BW 4
				/**< Maximum number of bandwidth supported. */
#define NSS_WIFILI_REPT_MU_MIMO 1
#define NSS_WIFILI_REPT_MU_OFDMA_MIMO 3
#define NSS_WIFILI_MAX_RESERVED_TYPE 2
				/**< Maximum reserved type. */
#define NSS_WIFILI_SOC_PER_PACKET_METADATA_SIZE 60
				/**< Metadata area total size. */
#define NSS_WIFILI_MEC_PEER_ID 0xDEAD
				/**< MEC (Multicast echo check) peer ID. */
#define NSS_WIFILI_DA_PEER_ID 0xDAAD
				/**< Destination address peer ID. */
#define NSS_WIFILI_MIC_KEY_LEN 8
				/**< MIC (Message integrity code) key length. */
#define NSS_WIFILI_TQM_RR_MAX 7
				/**< Maximum transmit queue release reasons. */
#define NSS_WIFILI_HTT_STATUS_MAX 7
				/**< Maximum HTT completion status. */
#define NSS_WIFILI_TQM_STATUS_MAX 9
				/**< Maximum TQM completion status. */
#define NSS_WIFILI_REO_CODE_MAX 15
				/**< Maximum Rx reorder error codes. */
#define NSS_WIFILI_DMA_CODE_MAX 14
				/**< Maximum DMA error codes. */
#define NSS_WIFILI_MAX_TID 8
				/**< Maximum TID values. */
#define NSS_WIFILI_DELAY_INDEX_MAX 10
				/**< Maximum software enqueue delay buckets. */
#define NSS_WIFILI_MAX_NUMBER_OF_ADDTNL_SEG 64
				/**< Maximum number of additional pages allocated from host. */
#define NSS_WIFILI_SOC_ATTACHED_MAX_PDEV_NUM 1
				/**< Maximum number of physical devices on the external SoC. */
#define NSS_WIFILI_PEER_AST_FLOWQ_MAX 4
				/**< Maximum number of flow queues. */
#define NSS_WIFILI_WBM_INTERNAL_ERR_MAX 5
				/**< WBM internal maximum errors. */

/*
 * Peer Size in Bytes
 */
#define NSS_WIFILI_PEER_SIZE 1600

/*
 * Radio specific flags
 */
#define NSS_WIFILI_PDEV_FLAG_V3_STATS_ENABLED 0x00000008
				/**< Flag to enable version 3 statistics. */
/**
 * Peer message flags.
 */
#define NSS_WIFILI_PEER_MSG_DISABLE_4ADDR 0x01

#ifdef __KERNEL__ /* only kernel will use. */

/**
 * Wireless Multimedia Extention Access Category to TID. @hideinitializer
 */
#define NSS_WIFILI_WME_AC_TO_TID(_ac) (	\
		((_ac) == NSS_WIFILI_WME_AC_VO) ? 6 : \
		(((_ac) == NSS_WIFILI_WME_AC_VI) ? 5 : \
		(((_ac) == NSS_WIFILI_WME_AC_BK) ? 1 : \
		0)))

/**
 * Wireless TID to Wireless Extension Multimedia Access Category. @hideinitializer
 */
#define NSS_WIFILI_TID_TO_WME_AC(_tid) (	\
		(((_tid) == 0) || ((_tid) == 3)) ? NSS_WIFILI_WME_AC_BE : \
		((((_tid) == 1) || ((_tid) == 2)) ? NSS_WIFILI_WME_AC_BK : \
		((((_tid) == 4) || ((_tid) == 5)) ? NSS_WIFILI_WME_AC_VI : \
		NSS_WIFILI_WME_AC_VO)))
#endif /* __KERNEL */

/**
 * nss_wifili_thread_scheme_id
 *	List of thread scheme IDs.
 */
enum nss_wifili_thread_scheme_id {
	NSS_WIFILI_THREAD_SCHEME_ID_0,		/**< High priority scheme index. */
	NSS_WIFILI_THREAD_SCHEME_ID_1,		/**< Low priority scheme index. */
	NSS_WIFILI_THREAD_SCHEME_ID_2,		/**< High priority scheme index. */
	NSS_WIFILI_THREAD_SCHEME_ID_MAX		/**< Maximum value of scheme index. */
};

/*
 * nss_wifili_thread_scheme_priority
 *	List of wifili thread scheme priority.
 */
enum nss_wifili_thread_scheme_priority {
	NSS_WIFILI_LOW_PRIORITY_SCHEME,		/**< Low priority scheme. */
	NSS_WIFILI_HIGH_PRIORITY_SCHEME,	/**< High priority scheme. */
};

/**
 * nss_wifili_wme_stream_classes
 *	WME stream classes.
 */
enum nss_wifili_wme_stream_classes {
	NSS_WIFILI_WME_AC_BE,	/**< Best effort. */
	NSS_WIFILI_WME_AC_BK,	/**< Background. */
	NSS_WIFILI_WME_AC_VI,	/**< Video. */
	NSS_WIFILI_WME_AC_VO,	/**< Voice. */
	NSS_WIFILI_WME_AC_MAX	/**< Maximum AC Value. */
};

/**
 * nss_wifili_packet_type
 *	Different Packet Types.
 */
enum nss_wifili_packet_type {
	NSS_WIFILI_DOT11_A,		/**< 802.11a packet type. */
	NSS_WIFILI_DOT11_B,		/**< 802.11b packet type. */
	NSS_WIFILI_DOT11_N,		/**< 802.11n packet type. */
	NSS_WIFILI_DOT11_AC,		/**< 802.11ac packet type. */
	NSS_WIFILI_DOT11_AX ,		/**< 802.11ax packet type. */
	NSS_WIFILI_DOT11_MAX		/**< Maximum 802.11 packet types. */
};

/*
 * nss_wifili_decap_pkt_type
 *	Different Decapsulation packet types
 */
enum wifili_decap_pkt_type {
	NSS_WIFILI_DECAP_TYPE_RAW,		/**< Raw packet type. */
	NSS_WIFILI_DECAP_TYPE_NATIVE_WIFI,	/**< Native Wi-Fi packet type. */
	NSS_WIFILI_DECAP_TYPE_ETHERNET,		/**< Ethernet packet type. */
	NSS_WIFILI_DECAP_TYPE_MAX,		/**< Maximum packet type. */
};

/**
 * nss_wifili_msg_types
 *	NSS wifili messages.
 */
enum nss_wifili_msg_types {
	NSS_WIFILI_INIT_MSG,
	NSS_WIFILI_SOC_RESET_MSG,
	NSS_WIFILI_PDEV_INIT_MSG,
	NSS_WIFILI_PDEV_DEINIT_MSG,
	NSS_WIFILI_START_MSG,
	NSS_WIFILI_STOP_MSG,
	NSS_WIFILI_PEER_CREATE_MSG,
	NSS_WIFILI_PEER_DELETE_MSG,
	NSS_WIFILI_SEND_PEER_MEMORY_REQUEST_MSG,
	NSS_WIFILI_PEER_FREELIST_APPEND_MSG,
	NSS_WIFILI_STATS_MSG,
	NSS_WIFILI_WDS_VENDOR_MSG,
	NSS_WIFILI_PEER_STATS_MSG,
	NSS_WIFILI_WDS_PEER_ADD_MSG,
	NSS_WIFILI_WDS_PEER_DEL_MSG,
	NSS_WIFILI_WDS_PEER_MAP_MSG,
	NSS_WIFILI_WDS_ACTIVE_INFO_MSG,
	NSS_WIFILI_STATS_CFG_MSG,
	NSS_WIFILI_TID_REOQ_SETUP_MSG,
	NSS_WIFILI_RADIO_CMD_MSG,
	NSS_WIFILI_LINK_DESC_INFO_MSG,
	NSS_WIFILI_PEER_SECURITY_TYPE_MSG,
	NSS_WIFILI_PEER_NAWDS_ENABLE_MSG,
	NSS_WIFILI_RADIO_BUF_CFG,
	NSS_WIFILI_DBDC_REPEATER_SET_MSG,
	NSS_DBDC_REPEATER_AST_FLUSH_MSG,
	NSS_WIFILI_SET_HMMC_DSCP_OVERRIDE_MSG,
	NSS_WIFILI_SET_HMMC_DSCP_TID_MSG,
	NSS_WIFILI_PDEV_STATS_V3_TXRX_SYNC_MSG,
	NSS_WIFILI_PDEV_STATS_V3_DELAY_SYNC_MSG,
	NSS_WIFILI_ENABLE_V3_STATS_MSG,
	NSS_WIFILI_WDS_PEER_UPDATE_MSG,
	NSS_WIFILI_STATS_V2_CFG_MSG,
	NSS_WIFILI_SOJOURN_STATS_MSG,
	NSS_WIFILI_PEER_SET_VLAN_ID,
	NSS_WIFILI_UPDATE_PDEV_LMAC_ID_MSG,
	NSS_WIFILI_PEER_AST_FLOWID_MAP_MSG,
	NSS_WIFILI_PEER_MEC_AGEOUT_MSG,
	NSS_WIFILI_JITTER_STATS_MSG,
	NSS_WIFILI_ISOLATION_MSG,
	NSS_WIFILI_PEER_EXT_STATS_MSG,
	NSS_WIFILI_CLR_STATS,
	NSS_WIFILI_PEER_4ADDR_EVENT_MSG,
	NSS_WIFILI_DBDC_REPEATER_LOOP_DETECTION_MSG,
	NSS_WIFILI_PEER_UPDATE_AUTH_FLAG,
	NSS_WIFILI_SEND_MESH_CAPABILITY_INFO,
	NSS_WIFILI_MAX_MSG
};

/**
 * nss_wifili_error_types
 *	Wifili error message types for functions.
 */
enum nss_wifili_error_types {
	NSS_WIFILI_EMSG_NONE,
			/**< No error. */
	NSS_WIFILI_EMSG_INIT_FAIL_IMPROPER_STATE,
			/**< Device initialization failure due to improper state of device. */
	NSS_WIFILI_EMSG_RINGS_INIT_FAIL,
			/**< Device ring initialization failure. */
	NSS_WIFILI_EMSG_PDEV_INIT_IMPROPER_STATE_FAIL,
			/**< Radio initialization failure due to improper state of device. */
	NSS_WIFILI_EMSG_PDEV_INIT_INVALID_RADIOID_FAIL,
			/**< Radio initialization failed due to invalid radio ID. */
	WIFILI_EMSG_PDEV_INIT_INVALID_TARGETPDEVID_FAIL,
			/**< Radio initialization failed due to invalid target physical device ID. */
	NSS_WIFILI_EMSG_PDEV_TX_IRQ_ALLOC_FAIL,
			/**< IRQ line allocation for radio transmission failed. */
	NSS_WIFILI_EMSG_PDEV_RESET_INVALID_RADIOID_FAIL,
			/**< Radio reset failed due to invalid radio ID. */
	NSS_WIFILI_EMSG_PDEV_RESET_PDEV_NULL_FAIL,
			/**< Radio reset failed due to NULL physical device. */
	NSS_WIFILI_EMSG_PDEV_RESET_IMPROPER_STATE_FAIL,
			/**< Radio reset failed due to improper state of pdev. */
	NSS_WIFILI_EMSG_START_IMPROPER_STATE_FAIL,
			/**< Device start fail due to improper state */
	NSS_WIFILI_EMSG_PEER_CREATE_FAIL,
			/**< Peer creation failed. */
	NSS_WIFILI_EMSG_PEER_DELETE_FAIL,
			/**< Peer deletion failed. */
	NSS_WIFILI_EMSG_HASHMEM_INIT_FAIL,
			/**< Peer hash memory allocation failed. */
	NSS_WIFILI_EMSG_PEER_FREELIST_APPEND_FAIL,
			/**< Appending peer to freelist failed. */
	NSS_WIFILI_EMSG_PEER_CREATE_INVALID_VDEVID_FAIL,
			/**< Peer creation failure due to invalid virtual device ID. */
	NSS_WIFILI_EMSG_PEER_CREATE_INVALID_PEER_ID_FAIL,
			/**< Peer creation failure due to invalid peer ID. */
	NSS_WIFILI_EMSG_PEER_CREATE_VDEV_NULL_FAIL,
			/**< Peer creation failure due to NULL virtual device. */
	NSS_WIFILI_EMSG_PEER_CREATE_PDEV_NULL_FAIL,
			/**< Peer creation failure due to NULL physical device. */
	NSS_WIFILI_EMSG_PEER_CREATE_ALLOC_FAIL,
			/**< Peer creation failure due to memory allocation failure. */
	NSS_WIFILI_EMSG_PEER_DELETE_VAPID_INVALID_FAIL,
			/**< Peer deletion failure due to invalid virtual device ID. */
	NSS_WIFILI_EMSG_PEER_DELETE_INVALID_PEERID_FAIL,
			/**< Peer deletion failed due to invalid peer ID. */
	NSS_WIFILI_EMSG_PEER_DELETE_VDEV_NULL_FAIL,
			/**< Peer deletion failure due to NULL virtual device. */
	NSS_WIFILI_EMSG_PEER_DELETE_PDEV_NULL_FAIL,
			/**< Peer deletion failure due to NULL physical device. */
	NSS_WIFILI_EMSG_PEER_DELETE_PEER_NULL_FAIL,
			/**< Peer deletion failure due to NULL peer. */
	NSS_WIFILI_EMSG_PEER_DELETE_PEER_CORRUPTED_FAIL,
			/**< Peer creation failure due to corrupted peer. */
	NSS_WIFILI_EMSG_PEER_DUPLICATE_AST_INDEX_PEER_ID_FAIL,
			/**< AST index provided is duplicate. */
	NSS_WIFILI_EMSG_GROUP0_TIMER_ALLOC_FAIL,
			/**< Timer allocation failure. */
	NSS_WIFILI_EMSG_INSUFFICIENT_WT_FAIL,
			/**< Insufficient worker thread error. */
	NSS_WIFILI_EMSG_INVALID_NUM_TCL_RING_FAIL,
			/**< Invalid number of Transmit Classifier rings provided in initialization message. */
	NSS_WIFILI_EMSG_INVALID_NUM_REO_DST_RING_FAIL,
			/**< Invalid number of Rx reorder destination ring in initialization message. */
	NSS_WIFILI_EMSG_HAL_SRNG_SOC_ALLOC_FAIL,
			/**< Srng SoC memory allocation failure. */
	NSS_WIFILI_EMSG_HAL_SRNG_INVALID_RING_INFO_FAIL,
			/**< Device ring information is invalid. */
	NSS_WIFILI_EMSG_HAL_SRNG_TCL_ALLOC_FAIL,
			/**< Transmit Classifier srng ring allocation failure. */
	NSS_WIFILI_EMSG_HAL_SRNG_TXCOMP_ALLOC_FAIL,
			/**< Txcomp srng ring allocation failure. */
	NSS_WIFILI_EMSG_HAL_SRNG_REODST_ALLOC_FAIL,
			/**< Rx reorder destination srng ring allocation failure. */
	NSS_WIFILI_EMSG_HAL_SRNG_REOREINJECT_ALLOC_FAIL,
			/**< Rx reorder reinject srng ring allocation failure. */
	NSS_WIFILI_EMSG_HAL_SRNG_RXRELEASE_ALLOC_FAIL,
			/**< Rx release srng ring allocation failure. */
	NSS_WIFILI_EMSG_HAL_SRNG_RXEXCP_ALLOC_FAIL,
			/**< Rx exception srng ring allocation failure. */
	NSS_WIFILI_EMSG_HAL_TX_MEMALLOC_FAIL,
			/**< Tx HAL (hardware abstraction layer) srng ring allocation failure. */
	NSS_WIFILI_EMSG_HAL_TX_INVLID_POOL_NUM_FAIL,
			/**< Invalid pool number in initialization message. */
	NSS_WIFILI_EMSG_HAL_TX_INVALID_PAGE_NUM_FAIL,
			/**< Invalid page numner in initialization message. */
	NSS_WIFILI_EMSG_HAL_TX_DESC_MEM_ALLOC_FAIL,
			/**< Tx descriptor memory allocation failure. */
	NSS_WIFILI_EMSG_HAL_RX_MEMALLOC_FAIL,
			/**< Rx memory allocation failure. */
	NSS_WIFILI_EMSG_PDEV_RXDMA_RING_ALLOC_FAIL,
			/**< Rx DMA ring allocation failed. */
	NSS_WIFILI_EMSG_NAWDSEN_PEERID_INVALID,
			/**< Peer NAWDS enable failure due to invalid peer ID. */
	NSS_WIFILI_EMSG_NAWDSEN_PEER_NULL,
			/**< Peer NAWDS enable failure due to peer being NULL. */
	NSS_WIFILI_EMSG_NAWDSEN_PEER_CORRUPTED,
			/**< Peer NAWDS enable failure due to corrupted peer. */
	NSS_WIFILI_EMSG_WDS_PEER_CFG_FAIL,
			/**< WDS peer configuration failure. */
	NSS_WIFILI_EMSG_RESET_NO_STOP,
			/**< Reset issued without stopping the device. */
	NSS_WIFILI_EMSG_HAL_SRNG_INVALID_RING_BASE_FAIL,
			/**< Ring base address is invalid. */
	NSS_WIFILI_EMSG_PDEV_RX_INIT_FAIL,
			/**< Pdev Rx initialization failure. */
	NSS_WIFILI_EMESG_AST_ADD_FAIL,
			/**< AST entry addition failure for connected peer. */
	NSS_WIFILI_EMESG_AST_REMOVE_FAIL,
			/**< AST entry removal failure for connected peer. */
	NSS_WIFILI_EMESG_WDS_ADD_FAIL,
			/**< WDS peer AST entry addition failure. */
	NSS_WIFILI_EMESG_WDS_REMOVE_FAIL,
			/**< WDS peer AST entry removal failure. */
	NSS_WIFILI_EMESG_WDS_MAP_FAIL,
			/**< WDS peer AST entry hardware index mapping failure. */
	NSS_WIFILI_EMSG_WDS_INVALID_PEERID_FAIL,
			 /**< Invalid peer id passed in WDS messages. */
	NSS_WIFILI_EMSG_WDS_DUPLICATE_AST_INDEX_PEER_ID_FAIL,
			/**< AST entry index is already filled. */
	NSS_WIFILI_EMSG_INVALID_RADIO_CMD,
			/**< Radio command is invalid. */
	NSS_WIFILI_EMSG_INVALID_RADIO_IFNUM,
			/**< Radio interface number is invalid. */
	NSS_WIFILI_EMSG_PEER_SECURITY_PEER_NULL_FAIL,
			/**< Security message failed as peer is NULL for a peer ID. */
	NSS_WIFILI_EMSG_PEER_SECURITY_PEER_CORRUPTED_FAIL,
			/**< Security message failed as peer is corrupted. */
	NSS_WIFILI_EMSG_RADIO_INVALID_BUF_CFG,
			/**< Buffer configuration message failed as invalid range value is provided. */
	NSS_WIFILI_EMSG_INIT_FAIL_INVALID_TARGET,
			/**< Invalid target SoC type from host. */
	NSS_WIFILI_EMSG_PDEV_INIT_FAIL_INVALID_LMAC_ID,
			/**< Invalid lower MAC ID from host. */
	NSS_WIFILI_EMSG_STATE_PDEV_NOT_INITIALIZED,
			/**< Configured message issued when radio is not initialized. */
	NSS_WIFILI_EMESG_RX_TLV_INVALID,
			/**< Invalid TLV length. */
	NSS_WIFILI_EMESG_RX_BUF_LEN_INVALID,
			/**< Invalid Rx buffer length. */
	NSS_WIFILI_EMSG_UNKNOWN
			/**< Unknown error message. */
};

/**
 * nss_wifili_soc_extended_data_types
 *	Enumeration of extended data type to host.
 */
enum nss_wifili_soc_extended_data_types {
	NSS_WIFILI_SOC_EXT_DATA_PKT_TYPE_NONE,		/**< Packet type is none. */
	NSS_WIFILI_SOC_EXT_DATA_PKT_MSDU_LINK_DESC,	/**< Packet type is MSDU link descriptor. */
	NSS_WIFILI_SOC_EXT_DATA_PKT_INVALID_PEER,	/**< Packet type is invalid peer. */
	NSS_WIFILI_SOC_EXT_DATA_PKT_MIC_ERROR,		/**< Packet received with MIC error. */
	NSS_WIFILI_SOC_EXT_DATA_PKT_2K_JUMP_ERROR,	/**< Packet received with 2K jump in sequence number. */
	NSS_WIFILI_SOC_EXT_DATA_PKT_WIFI_PARSE_ERROR,	/**< Packet received with Wi-Fi parse error. */
	NSS_WIFILI_SOC_EXT_DATA_PKT_TYPE_MAX		/**< Maximum extended data types. */
};

/**
 * nss_wifili_radio_cmd
 *	Wi-Fi radio commands for wifili.
 */
enum nss_wifili_radio_cmd {
	NSS_WIFILI_RADIO_TX_CAPTURE_CMD,		/**< Enable Tx capture. */
	NSS_WIFILI_SET_PRIMARY_RADIO,			/**< Set current radio as primary. */
	NSS_WIFILI_SET_ALWAYS_PRIMARY,			/**< Set always primary flag. */
	NSS_WIFILI_SET_FORCE_CLIENT_MCAST_TRAFFIC,	/**< Flag to force multicast traffic for a radio. */
	NSS_WIFILI_SET_DROP_SECONDARY_MCAST,		/**< Flag to drop multicast traffic on secondary radio. */
	NSS_WIFILI_SET_DBDC_FASTLANE,			/**< Flag to set DBDC fast-lane mode. */
	NSS_WIFILI_SET_DBDC_NOBACKHAUL_RADIO,           /**< Flag to set DBDC to no backhaul radio. */
	NSS_WIFILI_RADIO_MAX_CMD			/**< Maximum radio command index. */
};

/*
 * WARNING: There is a 1:1 mapping between values of enum nss_wifili_stats_txrx and corresponding
 * statistics string array in nss_stats.c.
 */

/**
 * nss_wifili_stats_txrx
 *	Wifili Tx or Rx statistics.
 */
enum nss_wifili_stats_txrx {
	NSS_WIFILI_STATS_RX_MSDU_ERROR,
		/**< Number of Rx packets received from ring with MSDU error. */
	NSS_WIFILI_STATS_RX_INV_PEER_RCV,
		/**< Number of Rx packets with invalid peer ID. */
	NSS_WIFILI_STATS_RX_WDS_SRCPORT_EXCEPTION,
		/**< Number of Rx packets exceptioned to host because of source port learn fail. */
	NSS_WIFILI_STATS_RX_WDS_SRCPORT_EXCEPTION_FAIL,
		/**< Number of Rx source port learn fail packets failed to get enqueued to host. */
	NSS_WIFILI_STATS_RX_DELIVERD,
		/**< Number of packets wifili has given to next node. */
	NSS_WIFILI_STATS_RX_DELIVER_DROPPED,
		/**< Number of packets which wifili failed to enqueue to next node. */
	NSS_WIFILI_STATS_RX_INTRA_BSS_UCAST,
		/**< Number of packets that wifili sent for intra-BSS unicast packet. */
	NSS_WIFILI_STATS_RX_INTRA_BSS_UCAST_FAIL,
		/**< Number of packets that wifili sent for intra-BSS unicast packet failed. */
	NSS_WIFILI_STATS_RX_INTRA_BSS_MCAST,
		/**< Number of packets that wifili sent for intra-BSS multicast packet. */
	NSS_WIFILI_STATS_RX_INTRA_BSS_MCAST_FAIL,
		/**< Number of packets that wifili sent for intra-BSS multicast packet failed. */
	NSS_WIFILI_STATS_RX_SG_RCV_SEND,
		/**< Number of packets scatter-gather sent. */
	NSS_WIFILI_STATS_RX_SG_RCV_FAIL,
		/**< Number of packets scatter-gather received failure. */
	NSS_STATS_WIFILI_RX_MCAST_ECHO,
		/**< Number of multicast echo packets received. */
	NSS_STATS_WIFILI_RX_INV_TID,
		/**< Number of invalid TID. */

	/*
	 * TODO: Move per TID based
	 */
	NSS_WIFILI_STATS_RX_FRAG_INV_SC,
		/**< Number of fragments with invalid sequence control. */
	NSS_WIFILI_STATS_RX_FRAG_INV_FC,
		/**< Number of fragments with invalid frame control. */
	NSS_WIFILI_STATS_RX_FRAG_NON_FRAG,
		/**< Number of non-fragments received in fragments. */
	NSS_WIFILI_STATS_RX_FRAG_RETRY,
		/**< Number of retries for fragments. */
	NSS_WIFILI_STATS_RX_FRAG_OOO,
		/**< Number of out-of-order fragments. */
	NSS_WIFILI_STATS_RX_FRAG_OOO_SEQ,
		/**< Number of out-of-order sequence. */
	NSS_WIFILI_STATS_RX_FRAG_ALL_FRAG_RCV,
		/**< Number of times all fragments for a sequence has been received. */
	NSS_WIFILI_STATS_RX_FRAG_DELIVER,
		/**< Number of fragments delivered to host. */
	NSS_WIFILI_STATS_TX_ENQUEUE,
		/**< Number of packets that got enqueued to wifili. */
	NSS_WIFILI_STATS_TX_ENQUEUE_DROP,
		/**< Number of packets that dropped during enqueue to wifili. */
	NSS_WIFILI_STATS_TX_DEQUEUE,
		/**< Number of packets that are dequeued by wifili. */
	NSS_WIFILI_STATS_TX_HW_ENQUEUE_FAIL,
		/**< Number of Rx packets that NSS Wi-Fi offload path could successfully process. */
	NSS_WIFILI_STATS_TX_SENT_COUNT,
		/**< Number of Tx packets sent to hardware. */
	NSS_WIFILI_STATS_TXRX_MAX,
		/**< Number of maximum Tx or Rx statistics. */
};

/*
 * WARNING: There is a 1:1 mapping between values of enum nss_wifili_stats_tcl and corresponding
 * statistics string array in nss_stats.c.
 */

/**
 * nss_wifili_stats_tcl
 *	Wifili transmit classifier statistics.
 */
enum nss_wifili_stats_tcl {
	NSS_WIFILI_STATS_TCL_NO_HW_DESC,		/**< Number of transmit classifier hardware descriptor. */
	NSS_WIFILI_STATS_TCL_RING_FULL,			/**< Number of times transmit classifier ring was full. */
	NSS_WIFILI_STATS_TCL_RING_SENT,			/**< Number of times transmit classifier descriptor sent. */
	NSS_WIFILI_STATS_TCL_MAX,			/**< Number of maximum transmit classifier statistics. */
};

/*
 * WARNING: There is a 1:1 mapping between values of enum nss_wifili_stats_tx_comp and corresponding
 * statistics string array in nss_stats.c.
 */

/**
 * nss_wifili_stats_tx_comp
 *	Wifili Tx completion statistics.
 */
enum nss_wifili_stats_tx_comp {
	NSS_WIFILI_STATS_TX_DESC_FREE_INV_BUFSRC,	/**< Number of invalid buffer source packets. */
	NSS_WIFILI_STATS_TX_DESC_FREE_INV_COOKIE,	/**< Number of invalid cookie packets. */
	NSS_WIFILI_STATS_TX_DESC_FREE_HW_RING_EMPTY,	/**< Number of times hardware ring empty found. */
	NSS_WIFILI_STATS_TX_DESC_FREE_REAPED,		/**< Number of Tx packets that are reaped out of the Tx completion ring. */
	NSS_WIFILI_STATS_TX_DESC_FREE_MAX,		/**< Number of Tx completion statistics. */
};

/*
 * WARNING: There is a 1:1 mapping between values of enum nss_wifili_stats_reo and corresponding
 * statistics string array in nss_stats.c.
 */

/**
 * nss_wifili_stats_reo
 *	Wifili Rx reorder statistics.
 */
enum nss_wifili_stats_reo {
	NSS_WIFILI_STATS_REO_ERROR,			/**< Number of reorder error. */
	NSS_WIFILI_STATS_REO_REAPED,			/**< Number of reorder reaped. */
	NSS_WIFILI_STATS_REO_INV_COOKIE,		/**< Number of invalid cookie. */
	NSS_WIFILI_STATS_REO_FRAG_RCV,			/**< Number of fragmented packets received. */
	NSS_WIFILI_STATS_REO_MAX,			/**< Number of reorder statistics. */
};

/*
 * WARNING: There is a 1:1 mapping between values of enum nss_wifili_stats_txsw_pool and corresponding
 * statistics string array in nss_stats.c.
 */

/**
 * nss_wifili_stats_txsw_pool
 *	Wifili Tx descriptor statistics.
 */
enum nss_wifili_stats_txsw_pool {
	NSS_WIFILI_STATS_TX_DESC_IN_USE,		/**< Number of Tx packets that are currently in flight. */
	NSS_WIFILI_STATS_TX_DESC_ALLOC_FAIL,		/**< Number of Tx software descriptor allocation failures. */
	NSS_WIFILI_STATS_TX_DESC_ALREADY_ALLOCATED,	/**< Number of Tx software descriptor already allocated. */
	NSS_WIFILI_STATS_TX_DESC_INVALID_FREE,		/**< Number of Tx software descriptor invalid free. */
	NSS_WIFILI_STATS_TX_DESC_FREE_SRC_FW,		/**< Number of Tx descriptor for which release source is firmware. */
	NSS_WIFILI_STATS_TX_DESC_FREE_COMPLETION,	/**< Number of Tx descriptor completion. */
	NSS_WIFILI_STATS_TX_DESC_NO_PB,			/**< Number of Tx descriptor pbuf is NULL. */
	NSS_WIFILI_STATS_TX_QUEUELIMIT_DROP,		/**< Number of Tx dropped because of queue limit. */
	NSS_WIFILI_STATS_TX_DESC_MAX,			/**< Number of Tx descriptor statistics. */
};

/*
 * WARNING: There is a 1:1 mapping between values of enum nss_wifili_stats_ext_txsw_pool and corresponding
 * statistics string array in nss_stats.c
 */

/**
 * nss_wifili_stats_ext_txsw_pool
 *	Wifili Rx extended descriptor statistics.
 */
enum nss_wifili_stats_ext_txsw_pool {
	NSS_WIFILI_STATS_EXT_TX_DESC_IN_USE,		/**< Number of extended Tx packets that are currently in flight. */
	NSS_WIFILI_STATS_EXT_TX_DESC_ALLOC_FAIL,	/**< Number of extended Tx software descriptor allocation failures. */
	NSS_WIFILI_STATS_EXT_TX_DESC_ALREADY_ALLOCATED,	/**< Number of extended Tx software descriptor already allocated. */
	NSS_WIFILI_STATS_EXT_TX_DESC_INVALID_FREE,	/**< Number of extended Tx software descriptor invalid free. */
	NSS_WIFILI_STATS_EXT_TX_DESC_MAX,		/**< Number of extended Tx descriptor statistics. */
};

/*
 * WARNING: There is a 1:1 mapping between values of enum nss_wifili_stats_rxdma_pool and corresponding
 * statistics string array in nss_stats.c
 */

/**
 * nss_wifili_stats_rxdma_pool
 *	Wifili Rx descriptor statistics.
 */
enum nss_wifili_stats_rxdma_pool {
	NSS_WIFILI_STATS_RX_DESC_NO_PB,			/**< Number of Rx descriptors that have no pbufs. */
	NSS_WIFILI_STATS_RX_DESC_ALLOC_FAIL,		/**< Number of Rx descriptor allocation failures. */
	NSS_WIFILI_STATS_RX_DESC_IN_USE,		/**< Number of Rx descriptor allocations in use. */
	NSS_WIFILI_STATS_RX_DESC_MAX,			/**< Maximum number of Rx descriptor statistics. */
};

/*
 * WARNING: There is a 1:1 mapping between values of enum nss_wifili_stats_rxdma_ring and corresponding
 * statistics string array in nss_stats.c.
 */

/**
 * nss_wifili_stats_rxdma_ring
 *	Wifili Rx DMA(Direct Memory Access) ring statistics.
 */
enum nss_wifili_stats_rxdma_ring {
	NSS_WIFILI_STATS_RXDMA_DESC_UNAVAILABLE,	/**< Number of Rx DMA descriptor unavailable. */
	NSS_WIFILI_STATS_RXDMA_BUF_REPLENISHED,		/**< Number of Rx DMA buffer replenished. */
	NSS_WIFILI_STATS_RXDMA_DESC_MAX,		/**< Number of Rx DMA descriptor statistics. */
};

/*
 * WARNING: There is a 1:1 mapping between values of enum nss_wifili_stats_wbm and corresponding
 * statistics string array in nss_stats.c.
 */

/**
 * nss_wifili_stats_wbm
 *	Wifili WBM(Wireless Buffer Manager) ring statistics.
 */
enum nss_wifili_stats_wbm {
	NSS_WIFILI_STATS_WBM_IE_LOCAL_ALLOC_FAIL,	/**< Number of Wireless Buffer Manager internal local allocation failures. */
	NSS_WIFILI_STATS_WBM_SRC_DMA,			/**< Number of receive invalid source DMA. */
	NSS_WIFILI_STATS_WBM_SRC_DMA_CODE_INV,		/**< Number of receive invalid source DMA. */
	NSS_WIFILI_STATS_WBM_SRC_REO,			/**< Number of receive invalid source reorder. */
	NSS_WIFILI_STATS_WBM_SRC_REO_CODE_NULLQ,	/**< Number of receive invalid reorder error with NULL queue. */
	NSS_WIFILI_STATS_WBM_SRC_REO_CODE_INV,		/**< Number of receive invalid reorder code invalid. */
	NSS_WIFILI_STATS_WBM_SRC_INV,			/**< Number of receive invalid source invalid. */
	NSS_WIFILI_STATS_WBM_MAX,			/**< Number of receive Wireless Buffer Manager statistics. */
};

/**
 * nss_wifili_stats
 *	NSS wifili statistics.
 */
struct nss_wifili_stats {
	uint64_t stats_txrx[NSS_WIFILI_MAX_PDEV_NUM_MSG][NSS_WIFILI_STATS_TXRX_MAX];
							/**< Number of Tx or Rx statistics. */
	uint64_t stats_tcl_ring[NSS_WIFILI_MAX_TCL_DATA_RINGS_MSG][NSS_WIFILI_STATS_TCL_MAX];
							/**< TCL statistics for each ring. */
	uint64_t stats_tx_comp[NSS_WIFILI_MAX_TCL_DATA_RINGS_MSG][NSS_WIFILI_STATS_TX_DESC_FREE_MAX];
							/**< Tx completion ring statistics. */
	uint64_t stats_tx_desc[NSS_WIFILI_MAX_TXDESC_POOLS_MSG][NSS_WIFILI_STATS_TX_DESC_MAX];
							/**< Tx descriptor pool statistics. */
	uint64_t stats_ext_tx_desc[NSS_WIFILI_MAX_TX_EXT_DESC_POOLS_MSG][NSS_WIFILI_STATS_EXT_TX_DESC_MAX];
							/**< Tx extended descriptor pool statistics. */
	uint64_t stats_reo[NSS_WIFILI_MAX_REO_DATA_RINGS_MSG][NSS_WIFILI_STATS_REO_MAX];
							/**< Rx reorder ring statistics. */
	uint64_t stats_rx_desc[NSS_WIFILI_MAX_PDEV_NUM_MSG][NSS_WIFILI_STATS_RX_DESC_MAX];
							/**< Rx software pool statistics. */
	uint64_t stats_rxdma[NSS_WIFILI_MAX_PDEV_NUM_MSG][NSS_WIFILI_STATS_RXDMA_DESC_MAX];
							/**< Rx DMA ring statistics. */
	uint64_t stats_wbm[NSS_WIFILI_STATS_WBM_MAX];
							/**< Wireless Buffer Manager error ring statistics. */
};

/*
 * NSS wifili soc stats
 */
struct nss_wifili_soc_stats {
	uint32_t soc_maxpdev;	/**< Maximum number of radios per SoC. */
	struct nss_wifili_stats stats_wifili;
				/**< Per-SoC statistics. */
};

/**
 * nss_wifili_stats_notification
 *	Data for sending wifili statistics.
 */
struct nss_wifili_stats_notification {
	uint32_t core_id;		/**< Core ID. */
	uint32_t if_num;		/**< Interface number for this wifili. */
	struct nss_wifili_stats stats;	/**< Wifili statistics. */
};

#ifdef __KERNEL__ /* only kernel will use. */

/**
 * nss_wifili_hal_srng_info
 *	Wifili HAL srng information.
 */
struct nss_wifili_hal_srng_info{
	uint8_t ring_id;
			/**< Ring ID. */
	uint8_t mac_id;
			/**< Pdev ID. */
	uint8_t resv[2];
	uint32_t ring_base_paddr;
			/**< Physical base address of the ring. */
	uint32_t num_entries;
			/**< Number of entries in ring. */
	uint32_t flags;	/**< Miscellaneous flags. */
	uint32_t ring_dir;
			/**< Ring direction: source or destination. */
	uint32_t entry_size;
			/**< Ring entry size. */
	uint32_t low_threshold;
			/**< Low threshold – in number of ring entries (valid for source rings only). */
	uint32_t hwreg_base[NSS_WIFILI_MAX_SRNG_REG_GROUPS_MSG];
			/**< Hardware ring base address. */
};

/**
 * nss_wifili_hal_srng_soc_msg
 *	Wifili hal srng message.
 */
struct nss_wifili_hal_srng_soc_msg {
	uint32_t dev_base_addr;
			/**< Base address of WLAN device. */
	uint32_t shadow_rdptr_mem_addr;
			/**< Shadow read pointer address. */
	uint32_t shadow_wrptr_mem_addr;
			/**< Shadow write pointer address. */
	uint32_t lmac_rings_start_id;
			/**< start id of LMAC rings. */
};

/**
 * struct wifili_tx_desc_addtnl_mem_msg
 * 	Wifili additional host memory message for increeased descriptors
 */
struct nss_wifili_tx_desc_addtnl_mem_msg {
	uint32_t num_addtnl_addr;
			/**< Number of additional memory pages provided. */
	uint32_t addtnl_memory_addr[NSS_WIFILI_MAX_NUMBER_OF_ADDTNL_SEG];
			/**< Physical memory addresse of each additional page. */
	uint32_t addtnl_memory_size[NSS_WIFILI_MAX_NUMBER_OF_ADDTNL_SEG];
			/**< Size of each additional page. */
};

/**
 * nss_wifili_tx_desc_init_msg
 *	Wifili software descriptor pool initialization message.
 */
struct nss_wifili_tx_desc_init_msg {
	uint32_t num_tx_desc;
			/**< Count of the software descriptors. */
	uint32_t num_tx_desc_ext;
			/**< Count of software extented descriptors. */
	uint32_t num_pool;
			/**< Number of descriptor pools. */
	uint32_t memory_addr[NSS_WIFILI_MAX_NUMBER_OF_PAGE_MSG];
			/**< Memory start address of each page. */
	uint32_t memory_size[NSS_WIFILI_MAX_NUMBER_OF_PAGE_MSG];
			/**< Memory size. */
	uint32_t num_memaddr;
			/**< Number of memory address. */
	uint32_t ext_desc_page_num;
			/**< Extended descriptor page number. */
	uint32_t num_tx_desc_2;
			/**< Count of the software descriptors for second radio. */
	uint32_t num_tx_desc_ext_2;
			/**< Count of software extended descriptors for second radio. */
	uint32_t num_tx_desc_3;
			/**< Count of the software descriptors for third radio. */
	uint32_t num_tx_desc_ext_3;
			/**< Count of software extended descriptors for third radio. */
	uint32_t num_tx_device_limit;
			/**< Count of software Tx descriptors for the device. */
};

/**
 * nss_wifili_rx_init_param
 *	Rx initialization parameters.
 */
struct nss_wifili_rx_init_param {
	uint16_t tlv_size;	/**< Size of Rx TLV structure. */
	uint16_t rx_buf_len;	/**< Rx buffer length programmed to hardware. */
};

/**
 * nss_wifili_init_msg
 *	Wifili SoC initialization message.
 */
struct nss_wifili_init_msg {
	struct nss_wifili_hal_srng_soc_msg hssm;
	uint8_t num_tcl_data_rings;
			/**< Number of Transmit Classifier data rings. */
	uint8_t num_reo_dest_rings;
			/**< Number of Rx reorder rings. */
	uint8_t flags;
			/**< Flags for SoC initialization */
	uint8_t soc_mem_profile;
			/**< SoC memory profile (256M/512M/1G). */
	struct nss_wifili_hal_srng_info tcl_ring_info[NSS_WIFILI_MAX_TCL_DATA_RINGS_MSG];
			/**< Transmit Classifier data ring configuration information. */
	struct nss_wifili_hal_srng_info tx_comp_ring[NSS_WIFILI_MAX_TCL_DATA_RINGS_MSG];
			/**< Tx completion ring configuration information. */
	struct nss_wifili_hal_srng_info reo_dest_ring[NSS_WIFILI_MAX_REO_DATA_RINGS_MSG];
			/**< Rx reorder destination ring configuration information. */
	struct nss_wifili_hal_srng_info reo_exception_ring;
			/**< Rx reorder exception ring configuration information. */
	struct nss_wifili_hal_srng_info rx_rel_ring;
			/**< Wireless Buffer Manager release ring configuration information. */
	struct nss_wifili_hal_srng_info reo_reinject_ring;
			/**< Reinject ring configuration information. */
	struct nss_wifili_tx_desc_init_msg wtdim;
			/**< Tx descriptor initialization message. */
	uint32_t target_type;
			/**< Target type based on SoC. */
	struct nss_wifili_rx_init_param wrip;
			/**< Rx parameters to initialize Rx context. */
	struct nss_wifili_tx_desc_addtnl_mem_msg wtdam;
			/**< Tx descriptor additional memory message. */
	uint32_t tx_sw_internode_queue_size;
			/**< Tx software internode queue size. */
};

/**
 * nss_wifili_pdev_deinit_msg
 *	Wifili pdev deinit message.
 */
struct nss_wifili_pdev_deinit_msg {
	uint32_t ifnum;	/**< NSS interface number of pdev. */
};

/**
 * nss_wifili_pdev_init_msg
 *	Wifili pdev initialization message.
 */
struct nss_wifili_pdev_init_msg {
	struct nss_wifili_hal_srng_info rxdma_ring;
			/**< MAC (Media Access Control) ring configuration. */
	uint32_t radio_id;
			/**< MAC radio ID. */
	uint32_t hwmode;
			/**< MAC hardware mode. */
	uint32_t lmac_id;
			/**< Lower MAC ID. */
	uint32_t num_rx_swdesc;
			/**< Number of descriptors per Rx pool. */
	uint32_t target_pdev_id;
			/**< Target physical device ID. */
	uint8_t scheme_id;
			/**< Radio scheme ID. */
	uint8_t reserved[3];
			/**< Padding for alignment. */
};

/**
 * nss_wifili_peer_ast_flowid_map_msg
 *	Wifili peer AST flow ID map message.
 */
struct nss_wifili_peer_ast_flowid_map_msg {
	uint8_t peer_mac_addr[ETH_ALEN];
			/**< Peer MAC address. */
	uint16_t vdev_id;
			/**< VAP ID. */
	uint16_t ast_idx[NSS_WIFILI_PEER_AST_FLOWQ_MAX];
			/**< Address search table index. */
	uint8_t tid_valid_mask[NSS_WIFILI_PEER_AST_FLOWQ_MAX];
			/**< TID valid mask for a flow. */
	uint8_t is_valid[NSS_WIFILI_PEER_AST_FLOWQ_MAX];
			/**< Valid bit. */
	uint8_t flowQ[NSS_WIFILI_PEER_AST_FLOWQ_MAX];
			/**< Flow queue. */
	uint16_t peer_id;
			/**< Peer ID. */
	uint8_t reserved[2];
			/**< Padding for alignment. */
};

/**
 * nss_wifili_peer_ast
 *	Wifili peer creation message.
 */
struct nss_wifili_peer_msg {
	uint8_t peer_mac_addr[6];
			/**< Peer MAC address. */
	uint16_t vdev_id;
			/**< VAP ID. */
	uint16_t peer_id;
			/**< Peer ID. */
	uint16_t hw_ast_idx;
			/**< Hardware address search table index. */
	uint8_t is_nawds;
			/**< NAWDS enabled for peer. */
	uint8_t pext_stats_valid;
			/**< Peer extended statistics valid. */
	uint16_t psta_vdev_id;
			/**< Proxy station VAP ID. */
	uint32_t nss_peer_mem;
			/**< Holds peer memory adderss for NSS. */
	uint32_t tx_ast_hash;
			/**< AST hash to be used during packet transmission. */
	uint32_t pext_stats_mem;
			/**< Peer extended statistics memory. */
	uint32_t flags;
			/**< Peer flags. */
};

/**
 * nss_wifili_peer_freelist_append_msg
 *	Peer memory request.
 */
struct nss_wifili_peer_freelist_append_msg {
	uint32_t addr;
			/**< Starting address of peer_freelist pool. */
	uint32_t length;
			/**< Length of peer freelist pool. */
	uint32_t num_peers;
			/**< Maximum number of peer entries supported in pool. */
};

/**
 * nss_wifili_wds_extn_peer_cfg_msg
 *	Configuration information when the WDS vendor extension is enabled.
 */
struct nss_wifili_wds_extn_peer_cfg_msg {
	uint8_t peer_mac_addr[ETH_ALEN];	/**< Peer MAC address. */
	uint8_t wds_flags;			/**< WDS flags populated from the host. */
	uint8_t reserved;			/**< Alignment padding. */
	uint16_t peer_id;			/**< Peer ID. */
};

/**
 * nss_wifili_tx_stats
 *	Tx statistics.
 */
struct nss_wifili_tx_stats {
	uint32_t tx_enqueue_dropped;
			/**< Tx enqueue drop count. */
	uint32_t tx_enqueue_cnt;
			/**< Tx enqueue succesful count. */
	uint32_t tx_dequeue_cnt;
			/**< Tx dequeue count. */
	uint32_t tx_send_fail_cnt;
			/**< Hardware send failure count. */
	uint32_t inv_peer;
			/**< Invalid peer enqueue count. */
	uint32_t inv_peer_drop_byte_cnt;
			/**< Invalid peer drop byte count. */
	uint32_t tx_input_pkt;
			/**< Tx packets ready to sent. */
	uint32_t tx_processed_pkt;
			/**< Tx numner of packets sent. */
	uint32_t tx_processed_bytes;
			/**< Tx number of bytes processed. */
};

/**
 * nss_wifili_rx_stats
 *	Rx statistics.
 */
struct nss_wifili_rx_stats {
	uint32_t rx_msdu_err;
					/**< Rx msdu error count. */
	uint32_t rx_inv_peer;
					/**< Rx invalid peer count. */
	uint32_t rx_scatter_inv_peer;
					/**< Rx scatter invalid peer count. */
	uint32_t rx_wds_learn_send;
					/**< WDS source port learn packet. */
	uint32_t rx_wds_learn_send_fail;
					/**< WDS source port learn exception send failure count. */
	uint32_t rx_send_dropped;
					/**< Rx send dropped count. */
	uint32_t rx_deliver_cnt;
					/**< Rx deliver count to next node. */
	uint32_t rx_deliver_cnt_fail;
					/**< Rx deliver count failure. */
	uint32_t rx_intra_bss_ucast_send;
					/**< Intra-BSS unicast sent count. */
	uint32_t rx_intra_bss_ucast_send_fail;
					/**< Intra-BSS unicast send failure count. */
	uint32_t rx_intra_bss_mcast_send;
					/**< Intra-BSS multicast send count. */
	uint32_t rx_intra_bss_mcast_send_fail;
					/**< Intra-BSS multicast send failure count. */
	uint32_t rx_sg_recv_send;
					/**< Rx scatter-gather receive send count. */
	uint32_t rx_sg_recv_fail;
					/**< Rx scatter-gather receive failure count. */
	uint32_t rx_me_pkts;		/**< Rx multicast echo packets count. */
	uint32_t rx_inv_tid;		/**< Rx invalid TID. */

	/*
	 * TODO: Move per tid based.
	 */
	uint32_t rx_frag_inv_sc;		/**< Rx invalid frame sequence control. */
	uint32_t rx_frag_inv_fc;		/**< Rx invalid frame control count. */
	uint32_t rx_non_frag_err;		/**< Rx non-fragment received in fragmention. */
	uint32_t rx_repeat_fragno;		/**< Rx fragment retry counters. */
	uint32_t rx_ooo_frag;			/**< Rx out-of-order fragments count. */
	uint32_t rx_ooo_frag_seq;		/**< Rx out-of-order sequence count. */
	uint32_t rx_all_frag_rcv;		/**< Rx all fragments received count. */
	uint32_t rx_frag_deliver;		/**< Rx fragment deliver counters. */
};

/**
 * nss_wifili_tx_tcl_ring_stats
 *	Transmit Classifier ring specific statistics.
 */
struct nss_wifili_tx_tcl_ring_stats {
	uint32_t tcl_no_hw_desc;	/**< Number of Transmit Classifier hardware descriptors. */
	uint32_t tcl_ring_full;		/**< Number of times Transmit Classifier ring full. */
	uint32_t tcl_ring_sent;		/**< Total number of ring sent. */
};

/**
 * nss_wifili_tx_comp_ring_stats
 *	Tx completion ring statistics.
 */
struct nss_wifili_tx_comp_ring_stats {
	uint32_t invalid_bufsrc;	/**< Tx comp (Completion) ring descriptor invalid buffer source. */
	uint32_t invalid_cookie;	/**< Tx comletion ring descriptor has invalid cookies. */
	uint32_t hw_ring_empty;		/**< Tx completion hardware ring empty. */
	uint32_t ring_reaped;		/**< Tx completion successfull ring reaped. */
};

/**
 * nss_wifili_tx_sw_pool_stats
 *	Tx completion sw statistics.
 */
struct nss_wifili_tx_sw_pool_stats {
	uint32_t desc_alloc;			/**< Tx descriptor software pool descriptor in use. */
	uint32_t desc_alloc_fail;		/**< Tx descriptor software pool allocation failure . */
	uint32_t desc_already_allocated;	/**< Tx descriptor re-allocation for allocated descriptor. */
	uint32_t desc_invalid_free;		/**< Tx descriptor freeing of allocated descriptor. */
	uint32_t tx_rel_src_fw;			/**< Tx descriptor source is firmware. */
	uint32_t tx_rel_ext_desc;		/**< Tx descriptor scatter-gather. */
	uint32_t tx_rel_tx_desc;		/**< Tx descriptor source is hardware*/
	uint32_t tx_rel_no_pb;			/**< Tx descriptor has pbuf present. */
	uint32_t tx_queue_limit_drop;		/**< Tx number of packets dropped because of queueing limits. */
};

/**
 * wifili_tx_ext_sw_pool_stats
 *	Tx extended descriptor pool.
 */
struct nss_wifili_tx_ext_sw_pool_stats {
	uint32_t desc_alloc;			/**< Tx extend (scatter gather) descriptor in use. */
	uint32_t desc_alloc_fail;		/**< Tx extend descriptor allocation failure. */
	uint32_t desc_already_allocated;	/**< Tx extend descriptor already allocated. */
	uint32_t desc_invalid_free;		/**< Tx descriptor invalid source. */

};

/**
 * nss_wifili_rx_wbm_ring_stats
 *	WBM (Wireless Buffer Manager) release ring statistics.
 */
struct nss_wifili_rx_wbm_ring_stats {
	uint32_t invalid_buf_mgr;		/**< Invalid buffer manager. */
	uint32_t err_src_rxdma;			/**< Wireless Buffer Manager source is Rx DMA ring. */
	uint32_t err_src_rxdma_code_inv;	/**< Wireless Buffer Manager source DMA reason unknown. */
	uint32_t err_src_reo;			/**< Wireless Buffer Manager source is receive reorder ring. */
	uint32_t err_src_reo_code_nullq;	/**< Wireless Buffer Manager source receive reorder ring because of NULL TLV. */
	uint32_t err_src_reo_code_inv;		/**< Wireless Buffer Manager source receive reorder ring reason unknown. */
	uint32_t err_src_invalid;		/**< Wireless Buffer Manager source is unknown. */
	uint32_t err_reo_codes[NSS_WIFILI_REO_CODE_MAX];
						/**< Receive reoder error codes. */
	uint32_t err_dma_codes[NSS_WIFILI_DMA_CODE_MAX];
						/**< DMA error codes. */
	uint32_t err_internal_codes[NSS_WIFILI_WBM_INTERNAL_ERR_MAX];
						/**< Wireless Buffer Manager error codes. */
};

/**
 * nss_wifili_rx_reo_ring_stats
 *	Rx reorder error statistics.
 */
struct nss_wifili_rx_reo_ring_stats {
	uint32_t ring_error;			/**< Rx reorder ring error. */
	uint32_t ring_reaped;			/**< Number of ring descriptor reaped. */
	uint32_t invalid_cookie;		/**< Number of invalid cookie. */
	uint32_t defrag_reaped;			/**< Rx defragment receive count. */
};

/**
 * nss_wifili_rx sw_pool_stats
 *	Wifili DMA sw pool statistics.
 */
struct nss_wifili_rx_sw_pool_stats {
	uint32_t rx_no_pb;			/**< Rx software descriptor number of buffer available. */
	uint32_t desc_alloc;			/**< Number of descriptor in use. */
	uint32_t desc_alloc_fail;		/**< Number of descriptor allocation failure. */
};

/**
 * nss_wifili_rx_dma_ring_stats
 *	Wifili Rx DMA ring statistics.
 */
struct nss_wifili_rx_dma_ring_stats {
	uint32_t rx_hw_desc_unavailable;	/**< Number of times hardware descriptor is unavailable. */
	uint32_t rx_buf_replenished;		/**< Number of buffers replenished. */
};

/**
 * nss_wifili_dbdc_mode_stats
 *	Wifili DBDC mode statistics.
 */
struct nss_wifili_dbdc_mode_stats {
	uint32_t dbdc_flush_ast_failed;
			/**< Number of times DBDC AST flush message send has failed. */
	uint32_t dbdc_drop_rx_secmcast;
			/**< Number of packets dropped in DBDC Rx for secondary multicast. */
	uint32_t dbdc_drop_tx_secmcast;
			/**< Number of packets dropped in DBDC Tx for secondary multicast. */
	uint32_t dbdc_drop_rx_alwaysprimary;
			/**< Number of packets dropped in DBDC Rx for always primary. */
	uint32_t dbdc_drop_tx_alwaysprimary;
			/**< Number of packets dropped in DBDC Tx for always primary. */
	uint32_t dbdc_drop_loop_rx;
			/**< Number of packets dropped in DBDC Rx for DBDC loop. */
	uint32_t dbdc_drop_loop_tx;
			/**< Number of packets dropped in DBDC Tx for DBDC loop. */
};

/**
 * nss_wifili_delay_stats
 * 	Wifili delay statistics.
 */
struct nss_wifili_delay_stats {
	uint32_t delay_bucket[NSS_WIFILI_DELAY_INDEX_MAX];
			/**< Delay buckets for histogram. */
	uint32_t min_delay;
			/**< Minimum delay. */
	uint32_t avg_delay;
			/**< Average delay. */
	uint32_t max_delay;
			/**< Maximum delay. */
};

/**
 * nss_wifili_v3_delay_per_tid_stats
 * 	Wifili version 3 delay per TID statistics.
 */
struct nss_wifili_v3_delay_per_tid_stats {
	struct nss_wifili_delay_stats swq_delay;
				/**< Software enqueue delay. */
	struct nss_wifili_delay_stats hwtx_delay;
				/**< Hardware transmit delay. */
	struct nss_wifili_delay_stats tx_intfrm_delay;
				/**< Transmit interframe delay at radio entry. */
	struct nss_wifili_delay_stats rx_intfrm_delay;
				/**< Receive interframe delay. */
};

/**
 * nss_wifili_v3_per_tid_tx_rx_stats
 * 	Wifili version 3 Tx and Rx statistics per TID.
 */
struct nss_wifili_v3_tx_rx_per_tid_stats {
	uint32_t radio_ingress_enq_drop_cnt;
				/**< Ingress enqueue drop count. */
	uint32_t transmit_succes_cnt;
				/**< Total successful transmit count. */
	uint32_t transmit_fwdrop_cnt;
				/**< Firmware drop count. */
	uint32_t transmit_hwdrop_cnt;
				/**< Hardware drop count. */
	uint32_t transmit_desc_fail_cnt;
				/**< Transmit descriptor fail count. */
	uint32_t transmit_complete_cnt;
				/**< Total transmit count. */
	uint32_t rx_delivered_cnt;
				/**< Total Rx packets delivered to next node. */
	uint32_t rx_deliver_fail_cnt;
				/**< Rx deliver fail count. */
	uint32_t rx_intrabss_cnt;
				/**< Intra-BSS Rx count. */
	uint32_t rx_intrabss_fail_cnt;
				/**< Intra-BSS Rx fail count. */
	uint32_t num_msdu_recived;
				/**< Number of MSDU received from hardware. */
	uint32_t num_mcast_msdu_recived;
				/**< Number of broadcast MSDU received. */
	uint32_t num_bcast_msdu_recived;
				/**< Number of multicast MSDU received. */
	uint32_t transmit_tqm_status_cnt[NSS_WIFILI_TQM_STATUS_MAX];
				/**< Number of frames with this TQM completion status. */
	uint32_t transmit_htt_status_cnt[NSS_WIFILI_HTT_STATUS_MAX];
				/**< Number of frames with this HTT completion status. */
};

/**
 * nss_wifili_v3_tx_rx_per_ac_stats
 * 	Wifili version 3 Tx and Rx statistics per AC.
 */
struct nss_wifili_v3_tx_rx_per_ac_stats {
	uint32_t radio_ingress_enq_cnt;
				/**< Ingress enqueue packet count. */
	uint32_t radio_ingress_deq_cnt;
				/**< Ingress dequeue count. */
	uint32_t transmit_enq_cnt;
				/**< Transmit enqueue count. */
};

/**
 * nss_wifili_radio_tx_rx_stats_v3
 * 	Wifili version 3 radio Tx and Rx statistics.
 */
struct nss_wifili_radio_tx_rx_stats_v3 {
	struct nss_wifili_v3_tx_rx_per_tid_stats tid_stats[NSS_WIFILI_MAX_TID];
				/**< Per-TID Tx and Rx statistics. */
	struct nss_wifili_v3_tx_rx_per_ac_stats ac_stats[NSS_WIFILI_WME_AC_MAX];
				/**< Per-Access Category Tx and Rx statistics. */
};

/**
 * nss_wifili_radio_delay_stats_v3
 * 	Wifili version 3 radio delay statistics.
 */
struct nss_wifili_radio_delay_stats_v3 {
	struct nss_wifili_v3_delay_per_tid_stats v3_delay_stats[NSS_WIFILI_MAX_TID];
				/**< Per-TID delay statistics. */
};

/**
 * nss_wifili_pdev_v3_tx_rx_stats_sync_msg
 * 	Wifili message to synchronize version 3 Tx and Rx statistics to HLOS.
 */
struct nss_wifili_pdev_v3_tx_rx_stats_sync_msg {
	uint32_t radio_id;
			/**< Radio ID. */
	struct nss_wifili_radio_tx_rx_stats_v3 wlpv3_txrx_stats;
			/**< Wifli version 3 Tx and Rx statistics. */
};

/**
 * nss_wifili_pdev_v3_delay_stats_sync_msg
 * 	Wifili message to synchronize version 3 delay statistics to HLOS.
 */
struct nss_wifili_pdev_v3_delay_stats_sync_msg {
	uint32_t radio_id;
			/**< Radio ID. */
	struct nss_wifili_radio_delay_stats_v3 wlpv3_delay_stats;
			/**< Wifli version 3 delay statistics. */
};

/**
 * nss_wifili_device_stats
 * 	Wifili specific statistics.
 */
struct nss_wifili_device_stats {
	struct nss_wifili_tx_tcl_ring_stats tcl_stats[NSS_WIFILI_MAX_TCL_DATA_RINGS_MSG];
									/**< Transmit Classifier ring statistics. */
	struct nss_wifili_tx_comp_ring_stats txcomp_stats[NSS_WIFILI_MAX_TCL_DATA_RINGS_MSG];
									/**< Tx completion ring statistics. */
	struct nss_wifili_tx_sw_pool_stats tx_sw_pool_stats[NSS_WIFILI_MAX_TXDESC_POOLS_MSG];
									/**< Tx software pool statistics. */
	struct nss_wifili_tx_ext_sw_pool_stats tx_ext_sw_pool_stats[NSS_WIFILI_MAX_TX_EXT_DESC_POOLS_MSG];
									/**< Tx extended software pool statistics. */
	struct nss_wifili_tx_stats tx_data_stats[NSS_WIFILI_MAX_PDEV_NUM_MSG];
									/**< Tx data statistics for each pdev. */
	struct nss_wifili_rx_reo_ring_stats rxreo_stats[NSS_WIFILI_MAX_REO_DATA_RINGS_MSG];
									/**< Rx reorder ring statistics. */
	struct nss_wifili_rx_sw_pool_stats rx_sw_pool_stats[NSS_WIFILI_MAX_PDEV_NUM_MSG];
									/**< Rx DMA software pool statistics. */
	struct nss_wifili_rx_stats rx_data_stats[NSS_WIFILI_MAX_PDEV_NUM_MSG];
									/**< Rx data statistics for each pdev. */
	struct nss_wifili_rx_dma_ring_stats rxdma_stats[NSS_WIFILI_MAX_PDEV_NUM_MSG];
									/**< Rx DMA ring statistics. */
	struct nss_wifili_rx_wbm_ring_stats rxwbm_stats;
									/**< Wireless Buffer Manager ring statistics. */
	struct nss_wifili_dbdc_mode_stats dbdc_stats;
									/**< DBDC mode statistics. */
};

/**
 * nss_wifili_stats_sync_msg
 *	Wifili SoC statistics synchronization message.
 */
struct nss_wifili_stats_sync_msg {
	struct nss_wifili_device_stats stats;
			/**< Device statistics. */
};

/**
 * nss_wifili_soc_linkdesc_per_packet_metadata
 *	Link descriptor per packet metadata.
 */
struct nss_wifili_soc_linkdesc_per_packet_metadata
{
	uint32_t desc_addr;	/**< Link descriptor address. */
};

/**
 * nss_wifili_soc_per_packet_metadata
 *	Per packet special data that has to be sent to host.
 */
struct nss_wifili_soc_per_packet_metadata {
	uint16_t pkt_type;	/**< Packet type. */
	uint8_t pool_id;	/**< Pool ID of invalid peer packets. */
	uint8_t reserved;	/**< Alignment padding. */

	/**
	 *  Link descriptor per packet metadata.
	 */
	union {
		struct nss_wifili_soc_linkdesc_per_packet_metadata linkdesc_metadata;
	} metadata;	/**< Per packet link descriptor metadata. */
};

/**
 * nss_wifili_tx_dropped
 *	Tx peer dropped packets.
 */
struct nss_wifili_tx_dropped {
	uint32_t drop_stats[NSS_WIFILI_TQM_RR_MAX];	/**< Discarded by firmware. */
	uint32_t tx_nawds_mcast_drop_cnt;		/**< Total number of NAWDS multicast packets dropped. */
};

/**
 * nss_wifili_tx_ctrl_stats
 *	Tx peer statistics.
 */
struct nss_wifili_tx_ctrl_stats {
	uint32_t ofdma; 		/**< Number of orthogonal frequency-division multiple
					  access packets. */
	uint32_t non_amsdu_cnt; 	/**< Number of MSDUs with no MSDU level aggregation. */
	uint32_t amsdu_cnt;		/**< Number of MSDUs part of AMSDU. */
	uint32_t tx_mcast_cnt;          /**< Total number of multicast packets sent. */
	uint32_t tx_mcast_bytes;        /**< Total number of multicast bytes sent. */
	uint32_t tx_ucast_cnt;          /**< Total number of unicast packets sent. */
	uint32_t tx_ucast_bytes;        /**< Total number of unicast bytes sent. */
	uint32_t tx_bcast_bytes;        /**< Total number of broadcast bytes sent. */
	uint32_t tx_bcast_cnt;          /**< Total number of broadcast packets sent. */
	struct nss_wifili_tx_dropped dropped;	/**< Tx peer dropped. */
	uint32_t tx_success_cnt;	/**< Total number of packets sent successfully. */
	uint32_t tx_success_bytes;	/**< Total number of bytes sent successfully. */
	uint32_t tx_nawds_mcast_cnt;	/**< Total number of NAWDS multicast packets sent. */
	uint32_t tx_nawds_mcast_bytes;	/**< Total number of NAWDS multicast bytes sent. */
	uint32_t retries;		/**< Total number of retries. */
};

/**
 * nss_wifili_peer_rx_err
 *	Rx peer errors.
 */
struct nss_wifili_rx_err {
	uint32_t mic_err;	/**< Rx MIC errors. */
	uint32_t decrypt_err;	/**< Rx Decryption errors. */
};

/**
 * nss_wifili_rx_ctrl_stats
 *	Peer Rx statistics.
 */
struct nss_wifili_rx_ctrl_stats {
	struct nss_wifili_rx_err err;			/**< Rx peer errors. */
	uint32_t multipass_rx_pkt_drop;         /**< Total number of multipass packets without a VLAN header. */
	uint32_t peer_unauth_rx_pkt_drop;		/**< Number of receive packets dropped due to an authorized peer. */
	uint32_t reserved_type[NSS_WIFILI_MAX_RESERVED_TYPE];	/**< Reserved type for future use. */
	uint32_t non_amsdu_cnt;			/**< Number of MSDUs with no MSDU level aggregation. */
	uint32_t amsdu_cnt;			/**< Number of MSDUs part of AMSDU. */
	uint32_t mcast_rcv_cnt;			/**< Total number of multicast packets received. */
	uint32_t mcast_rcv_bytes;		/**< Total number of multicast bytes received. */
	uint32_t rx_recvd;			/**< Total Rx received count. */
	uint32_t rx_recvd_bytes;		/**< Total Rx received count. */
	uint32_t nawds_mcast_drop;		/**< Total NAWDS drop count. */
	uint32_t nawds_mcast_drop_bytes;	/**< Total NAWDS drop count. */
	uint32_t rx_intra_bss_pkts_num;		/**< Total Intra-BSS packets received. */
	uint32_t rx_intra_bss_pkts_bytes;	/**< Total Intra-BSS bytes received. */
	uint32_t rx_intra_bss_fail_num;		/**< Total Intra-BSS packets failed. */
	uint32_t rx_intra_bss_fail_bytes;	/**< Total Intra-BSS bytes received. */
	uint32_t bcast_rcv_cnt;			/**< Total number of broadcast packets received. */
	uint32_t bcast_rcv_bytes;		/**< Total number of broadcast bytes received. */
};

/**
 * nss_wifili_peer_ctrl_stats
 *	Wifili peer control statistics.
 */
struct nss_wifili_peer_ctrl_stats {
	uint32_t peer_id;	/**< Peer ID. */
	struct nss_wifili_tx_ctrl_stats tx;
				/**< Peer Tx control statistics. */
	struct nss_wifili_rx_ctrl_stats rx;
				/**< Peer Rx control statistics. */
};

/**
 * nss_wifili peer_stats
 *	Wifili peer statistics.
 */
struct nss_wifili_peer_stats {
	uint32_t npeers;	/**< Number of entries of peer statistics. */
	struct nss_wifili_peer_ctrl_stats wpcs[1];
				/**< Wifili peer control statistics. */
};

/**
 * nss_wifili_peer_stats_msg
 *	Wifili peer statistics message.
 */
struct nss_wifili_peer_stats_msg {
	struct nss_wifili_peer_stats stats;
			/**< Wifili peer statistics. */
};

/**
 * nss_wifili_sojourn_per_tid_stats
 *      Wifili sojourn per TID statistics.
 */
struct nss_wifili_sojourn_per_tid_stats {
	uint32_t avg_sojourn_msdu;	/**< Average per-TID of all time difference. */
	uint32_t sum_sojourn_msdu;	/**< Sum per-TID of all time difference. */
	uint32_t num_msdus;		/**< MSDUs per TID. */
};

/**
 * nss_wifili_sojourn_peer_stats
 *      Wifili sojourn peer statistics.
 */
struct nss_wifili_sojourn_peer_stats {
	uint32_t peer_id;				/**< Peer ID. **/
	struct nss_wifili_sojourn_per_tid_stats stats[NSS_WIFILI_MAX_TID];	/**< Statistics per TID. **/
};

/**
 * nss_wifili_sojourn_stats_msg
 *      Wifili sojourn statistics message.
 */
struct nss_wifili_sojourn_stats_msg {
	uint32_t npeers;					/**< Number of peers. */
	struct nss_wifili_sojourn_peer_stats sj_peer_stats[1];	/**< Per-peer sojourn statistics. */
};

/*
 * nss_wifili_jitter_tid_stats
 *	Per TID jitter statistics.
 */
struct nss_wifili_jitter_tid_stats {
	uint32_t avg_jitter;				/**< Average jitter. */
	uint32_t avg_delay;				/**< Average delay. */
	uint32_t avg_err;				/**< Average count error. */
	uint32_t success;				/**< Transmit success count. */
	uint32_t drop;					/**< Transmit drop count. */
};

/*
 * nss_wifili_jitter_stats
 *	Wifili jitter statistics.
 */
struct nss_wifili_jitter_stats {
	uint32_t peer_id;		/**< Peer ID. */
	struct nss_wifili_jitter_tid_stats stats[NSS_WIFILI_MAX_TID];	/**< Per-TID jitter statistics. */
};

/*
 * nss_wifili_jitter_stats_msg
 *	Wifili jitter message.
 */
struct nss_wifili_jitter_stats_msg {
	uint32_t npeers;				/**< Number of peers. */
	struct nss_wifili_jitter_stats jitter_stats[1];	/**< Jitter statistics. */
};

/**
 * nss_wifili_wds_peer_msg
 *	Wi-Fi Wireless distribution system (WDS) peer-specific message.
 */
struct nss_wifili_wds_peer_msg {
	uint8_t dest_mac[ETH_ALEN];	/**< MAC address of the destination. */
	uint8_t peer_mac[ETH_ALEN];	/**< MAC address of the base peer. */
	uint8_t ast_type;		/**< AST (Address Search Table) type for this peer. */
	uint8_t pdev_id;		/**< Radio ID for next hop peer. */
	uint16_t peer_id;		/**< Peer ID of next hop peer. */
};

/**
 * nss_wifili_peer_delay_stats
 *	Per-peer delay statistics.
 */
struct nss_wifili_peer_delay_stats {
        struct nss_wifili_delay_stats swq_delay;                    /**< Software enqueue delay. */
        struct nss_wifili_delay_stats hwtx_delay;                   /**< Hardware transmit delay. */
};

/**
 * nss_wifili_peer_ext_stats
 *      Peer extended statistics.
 */
struct nss_wifili_peer_ext_stats {
        uint32_t peer_id;                       /**< Peer ID. */
        struct nss_wifili_peer_delay_stats delay_stats[NSS_WIFILI_MAX_TID];
                                                /**< Delay statistics. */
};

/**
 * nss_wifili_peer_ext_stats_msg
 *      Peer extended statistics message.
 */
struct nss_wifili_peer_ext_stats_msg {
        uint32_t npeers;                                /**< Number of peers. */
        struct nss_wifili_peer_ext_stats ext_stats[1];      /**< Extended statistics. */
};

/**
 * nss_wifili_stats_cfg_msg
 *	Wifili stats enable/disable configuration message.
 */
struct nss_wifili_stats_cfg_msg {
	uint32_t cfg;	/**< Enable or disable configuration. */
};

/**
 * nss_wifili_wds_peer_map_msg
 *	Wi-Fi Wireless distribution system(WDS) peer-specific message.
 */
struct nss_wifili_wds_peer_map_msg {
	uint8_t dest_mac[ETH_ALEN];	/**< MAC address of the destination. */
	uint16_t peer_id;		/**< Connected peer ID for this WDS peer. */
	uint16_t ast_idx;		/**< AST (address search table) index for this peer in host. */
	uint16_t vdev_id;;		/**< VAP ID. */
};

/**
 * nss_wifili_wds_active_info
 *	Wi-Fi WDS active information.
 */
struct nss_wifili_wds_active_info {
	uint16_t ast_idx;	/**< Hardware AST index. */
};

/**
 * nss_wifili_wds_active_info_msg
 *	Wi-Fi Wireless distribution system active information message.
 */
struct nss_wifili_wds_active_info_msg {
	uint16_t nentries;		/**< Number of WDS entries. */
	struct nss_wifili_wds_active_info info[1];
					/**< WDS active information. */
};

/**
 * nss_wifili_mec_ageout_info
 *	Wi-Fi multicast echo check ageout information.
 */
struct nss_wifili_mec_ageout_info {
	uint8_t mac_addr[6];	/**< MAC address. */
	uint8_t radio_id;		/**< Radio ID. */
	uint8_t pad;			/**< Pad for word align structure. */

};

/**
 * nss_wifili_mec_ageout_info_msg
 *	Wi-Fi multicast echo check ageout information message.
 */
struct nss_wifili_mec_ageout_info_msg {
	uint16_t nentries;				/**< Number of entries. */
	struct nss_wifili_mec_ageout_info info[1];
					/**<  Multicast echo check active information. */
};

/**
 * nss_wifili_soc_linkdesc_buf_info_msg
 *	Link descriptor buffer addresss information.
 */
struct nss_wifili_soc_linkdesc_buf_info_msg {
	uint32_t buffer_addr_low;	/**< Link descriptor low address. */
	uint32_t buffer_addr_high;	/**< Link descriptor high address. */
};

/**
 * nss_wifili_peer_security_type_msg
 *	Wifili security type message.
 */
struct nss_wifili_peer_security_type_msg {
	uint16_t peer_id;			/**< Peer ID. */
	uint8_t pkt_type;			/**< Unicast or broadcast packet type. */
	uint8_t security_type;			/**< Security type. */
	uint8_t mic_key[NSS_WIFILI_MIC_KEY_LEN];
						/**< MIC key. */
};

/**
 * nss_wifili_peer_nawds_enable_msg
 *	Wifili NAWDS enable for this peer.
 */
struct nss_wifili_peer_nawds_enable_msg {
	uint16_t peer_id;			/**< Peer ID. */
	uint16_t is_nawds;			/**< Enable NAWDS on this peer. */
};

/**
 * nss_wifili_peer_vlan_id_msg
 *	Wifili peer VLAN ID message.
 */
struct nss_wifili_peer_vlan_id_msg {
	uint16_t peer_id;			/**< Peer ID. */
	uint16_t vlan_id;			/**< VLAN ID. */
};

/**
 * nss_wifili_peer_isolation_msg
 *	Wifili peer isolation message.
 */
struct nss_wifili_peer_isolation_msg {
	uint16_t peer_id;			/**< Peer ID. */
	uint16_t isolation;			/**< Isolation enabled/disabled. */
};

/**
 * nss_wifili_dbdc_repeater_loop_detection_msg
 *	Wifili DBDC repeater loop detection message.
 */
struct nss_wifili_dbdc_repeater_loop_detection_msg {
	bool dbdc_loop_detected;		/**< DBDC repeater loop detection flag. */
};

/**
 * nss_wifili_dbdc_repeater_set_msg
 *	Wifili DBDC repeater set message.
 */
struct nss_wifili_dbdc_repeater_set_msg {
	uint32_t is_dbdc_en;			/**< DBDC enable flag. */
};

/**
 * nss_wifili_hmmc_dscp_tid_set_msg
 *	Wifili Hy-Fi managed multicast DSCP TID set message.
 */
struct nss_wifili_hmmc_dscp_tid_set_msg {
	uint16_t radio_id;			/**< Radio ID. */
	uint16_t value;			        /**< Hy-Fi managed multicast TID value. */
};

/**
 * nss_wifili_hmmc_dscp_override_set_msg
 *	Wifili Hy-Fi managed multicast DSCP override set message.
 */
struct nss_wifili_hmmc_dscp_override_set_msg {
	uint16_t radio_id;			/**< Radio ID. */
	uint16_t value;			        /**< Hy-Fi managed multicast DSCP override value. */
};

/**
 * nss_wifili_reo_tidq_msg
 *	Rx reorder TID queue setup message.
 */
struct nss_wifili_reo_tidq_msg {
	uint32_t tid;		/**< TID (traffic identification) value. */
	uint16_t peer_id;	/**< Peer ID. */
};

/**
 * nss_wifili_enable_v3_stats_msg
 * 	Version 3 statistics enable message.
 */
struct nss_wifili_enable_v3_stats_msg {
	uint32_t radio_id;	/**< Radio ID. */
	uint32_t flag;		/**< Flag to enable version 3 statistics. */
};

/**
 * nss_wifili_clr_stats_msg
 *	NSS firmware statistics clear message.
 */
struct nss_wifili_clr_stats_msg {
	uint8_t vdev_id;;	/**< VAP ID. */
};

/**
 * nss_wifili_update_auth_flag
 * 	Peer authentication flag message.
 */
struct nss_wifili_peer_update_auth_flag {
	uint16_t peer_id;		/**< Peer ID. */
	uint8_t auth_flag;		/**< Peer authentication flag. */
	uint8_t reserved;		/**< Alignment padding. */
};

/**
 * nss_wifili_update_pdev_lmac_id_msg
 * 	Physical device ID and lower MAC ID update message.
 */
struct nss_wifili_update_pdev_lmac_id_msg {
	uint32_t pdev_id;			/**< Physical device ID. */
	uint32_t lmac_id;			/**< Lower MAC ID. */
	uint32_t target_pdev_id;	/**< Target physical device ID. */
};

/**
 * nss_wifili_radio_cmd_msg
 * 	Wi-Fi radio specific special commands.
 */
struct nss_wifili_radio_cmd_msg {
	enum nss_wifili_radio_cmd cmd;
							/**< Type of command message. */
	uint32_t value;			/**< Value of the command. */
};

/**
 * nss_wifili_radio_buf_cfg_msg
 *	Wi-Fi Radio buffer requirement configuration.
 *
 * Number of payloads needed in NSS for multi-client scenarios are configured
 * from Wi-Fi driver as per following ranges:
 * 0-64 peers range 1.
 * 64-128 peers range 2.
 * 128-256 peers range 3.
 * >256 peers range 4.
 * Number of payloads needed in for each peer range is configured by Wi-Fi driver
 * for flexibility.
 */
struct nss_wifili_radio_buf_cfg_msg {
	uint32_t buf_cnt;		/**< Number of buffers required. */
	uint32_t range;			/**< Peer range. */
};

/**
 * nss_wifili_radio_cfg_msg
 *	Wi-Fi radio specific special configurations.
 */
struct nss_wifili_radio_cfg_msg {
	uint32_t radio_if_num;	/**< NSS assigned interface number for radio. */

	/**
	 * Wi-Fi radio specific special command message.
	 */
	union {
		struct nss_wifili_radio_cmd_msg radiocmdmsg;
							/**< Radio specific commands. */
		struct nss_wifili_radio_buf_cfg_msg radiobufcfgmsg;
							/**< Radio specific buffer configurations. */
	} radiomsg;	/**< Wi-Fi radio command message. */
};

/**
 * struct wifili_peer_wds_4addr_allow_msg
 *	Per-peer four address configuration message.
 */
struct nss_wifili_peer_wds_4addr_allow_msg {
	uint32_t peer_id;	/**< Peer ID. */
	uint32_t if_num;	/**< Associate virtual interface number. */
	bool enable;		/**< Boolean flag to enable/disable four address frames. */
};

/**
 * struct nss_wifili_mesh_capability_info
 * 	Wi-Fi mesh capability flag.
 */
struct nss_wifili_mesh_capability_info {
	bool mesh_enable;	/**< Wi-Fi mesh capability flag. */
};

/**
 * nss_wifili_msg
 *	Structure that describes wifili messages.
 */
struct nss_wifili_msg {
	struct nss_cmn_msg cm;                  /**< Common message header. */

	/**
	 * Payload of wifili message.
	 */
	union {
		struct nss_wifili_init_msg init;
				/**< Wi-Fi initialization data. */
		struct nss_wifili_pdev_init_msg pdevmsg;
				/**< Tx initialization data. */
		struct nss_wifili_pdev_deinit_msg pdevdeinit;
				/**< Tx de-initialization data. */
		struct nss_wifili_peer_msg peermsg;
				/**< Peer-specific data for the physical device. */
		struct nss_wifili_peer_freelist_append_msg peer_freelist_append;
				/**< Information for creating a peer freelist. */
		struct nss_wifili_stats_sync_msg wlsoc_stats;
				/**< Synchronization statistics. */
		struct nss_wifili_peer_stats_msg peer_stats;
				/**< Wifili peer statistics. */
		struct nss_wifili_wds_peer_msg wdspeermsg;
				/**< WDS peer-specific message. */
		struct nss_wifili_wds_peer_map_msg wdspeermapmsg;
				/**< WDS peer-mapping specific message. */
		struct nss_wifili_wds_active_info_msg wdsinfomsg;
				/**< WDS active information specific message. */
		struct nss_wifili_stats_cfg_msg scm;
				/**< Wifili peer statistics configuration message. */
		struct nss_wifili_reo_tidq_msg reotidqmsg;
				/**< Rx reorder TID queue setup message. */
		struct nss_wifili_radio_cfg_msg radiocfgmsg;
				/**< Radio command message. */
		struct nss_wifili_wds_extn_peer_cfg_msg wpeercfg;
				/**< WDS vendor configuration message. */
		struct nss_wifili_soc_linkdesc_buf_info_msg linkdescinfomsg;
				/**< Link descriptor buffer address information. */
		struct nss_wifili_peer_security_type_msg securitymsg;
				/**< Wifili peer security message. */
		struct nss_wifili_peer_nawds_enable_msg nawdsmsg;
				/**< Wifili peer enable NAWDS message. */
		struct nss_wifili_dbdc_repeater_set_msg dbdcrptrmsg;
				/**< Wifili DBDC repeater enable message. */
		struct nss_wifili_hmmc_dscp_override_set_msg shmmcdscpmsg;
				/**< Wifili Hy-Fi managed multicast DSCP override set message. */
		struct nss_wifili_hmmc_dscp_tid_set_msg shmmcdcptidmsg;
				/**< Wifili Hy-Fi managed multicast DSCP TID map set message. */
		struct nss_wifili_pdev_v3_tx_rx_stats_sync_msg v3_txrx_stats_msg;
				/**< Wifili version 3 Tx and Rx statistics message. */
		struct nss_wifili_pdev_v3_delay_stats_sync_msg v3_delay_stats_msg;
				/**< Wifili version 3 delay statistics message. */
		struct nss_wifili_enable_v3_stats_msg enablev3statsmsg;
				/**< Wifili version 3 statistics enable message. */
		struct nss_wifili_sojourn_stats_msg sj_stats_msg;
				/**< Wifili sojourn statistics message. */
		struct nss_wifili_peer_vlan_id_msg peervlan;
				/**< Wifili peer VLAN ID message. */
		struct nss_wifili_update_pdev_lmac_id_msg update_pdev_lmac_id_msg;
				/**< Wifili peer update lower MAC ID message. */
		struct nss_wifili_peer_ast_flowid_map_msg peer_ast_flowid_msg;
				/**< Wifili peer AST index flow ID map message. */
		struct nss_wifili_mec_ageout_info_msg mecagemsg;
				/**< Multicast echo check active information specific message. */
		struct nss_wifili_jitter_stats_msg jt_stats_msg;
				/**<Jitter statistics message. */
		struct nss_wifili_peer_isolation_msg isolation_msg;
				/**< Peer isolation message. */
				/**< Jitter statistics message. */
		struct nss_wifili_peer_ext_stats_msg pext_msg;
				/**< Peer extended statistics message. */
		struct nss_wifili_clr_stats_msg clrstats;
				/**< Clear NSS firmware statistics. */
		struct nss_wifili_peer_wds_4addr_allow_msg wpswm;
				/**< Peer four-address event message. */
		struct nss_wifili_dbdc_repeater_loop_detection_msg wdrldm;
				/**< Wifili DBDC repeater loop detection message. */
		struct nss_wifili_peer_update_auth_flag peer_auth;
				/**< Peer authentication flag message. */
		struct nss_wifili_mesh_capability_info cap_info;
				/**< Mesh capability flag. */
	} msg;			/**< Message payload. */
};

/**
 * nss_wifili_tx_msg
 *	Send wifili messages.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_wifili_msg
 *
 * @param[in] nss_ctx NSS context.
 * @param[in] msg     NSS Wi-Fi message.
 *
 * @return
 * nss_tx_status_t Tx status
 */
extern nss_tx_status_t nss_wifili_tx_msg(struct nss_ctx_instance *nss_ctx, struct nss_wifili_msg *msg);

/**
 * nss_wifili_tx_msg_sync
 *	Send wifili messages synchronously.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_wifili_msg
 *
 * @param[in] nss_ctx NSS context.
 * @param[in] msg     NSS Wi-Fi message.
 *
 * @return
 * nss_tx_status_t Tx status.
 */
extern nss_tx_status_t nss_wifili_tx_msg_sync(struct nss_ctx_instance *nss_ctx, struct nss_wifili_msg *msg);

/**
 * nss_wifili_msg_callback_t
 *	Callback to receive wifili messages.
 *
 * @datatypes
 * nss_wifili_msg
 *
 * @param[in] app_data Application context of the message.
 * @param[in] msg      Message data.
 *
 * @return
 * void
 */
typedef void (*nss_wifili_msg_callback_t)(void *app_data, struct nss_wifili_msg *msg);

/**
 * nss_wifili_callback_t
 *	Callback to receive Wi-Fi data.
 *
 * @datatypes
 * net_device \n
 * sk_buff \n
 * napi_struct
 *
 *
 * @param[in] netdev  Pointer to the associated network device.
 * @param[in] skb     Pointer to the data socket buffer.
 * @param[in] napi    Pointer to the NAPI structure.
 *
 * @return
 * void
 */
typedef void (*nss_wifili_callback_t)(struct net_device *netdev, struct sk_buff *skb, struct napi_struct *napi);

/**
 * nss_register_wifili_if
 *	Register to send/receive wifili SoC messages to NSS.
 *
 * @datatypes
 * nss_wifili_callback_t \n
 * nss_wifili_msg_callback_t \n
 * net_device
 *
 * @param[in] if_num             NSS interface number.
 * @param[in] wifi_callback      Callback for the Wi-Fi virtual device data.
 * @param[in] wifi_ext_callback  Callback for the extended data.
 * @param[in] event_callback     Callback for the message.
 * @param[in] netdev             Pointer to the associated network device.
 * @param[in] features           Data socket buffer types supported by this
 *                               interface.
 *
 * @return
 * nss_ctx_instance* NSS context
 */
struct nss_ctx_instance *nss_register_wifili_if(uint32_t if_num, nss_wifili_callback_t wifi_callback,
			nss_wifili_callback_t wifi_ext_callback, nss_wifili_msg_callback_t event_callback, struct net_device *netdev, uint32_t features);

/**
 * nss_unregister_wifili_if
 *	Deregister wifili SoC interface with NSS.
 *
 * @param[in] if_num NSS interface number.
 *
 * @return
 * void
 */
void nss_unregister_wifili_if(uint32_t if_num);

/**
 * nss_register_wifili_radio_if
 *	Register to send/receive wifili radio messages to NSS.
 *
 * @datatypes
 * nss_wifili_callback_t \n
 * nss_wifili_msg_callback_t \n
 * net_device
 *
 * @param[in] if_num             NSS interface number.
 * @param[in] wifi_callback      Callback for the Wi-Fi radio virtual device data.
 * @param[in] wifi_ext_callback  Callback for the extended data.
 * @param[in] event_callback     Callback for the message.
 * @param[in] netdev             Pointer to the associated network device.
 * @param[in] features           Data socket buffer types supported by this
 *                               interface.
 *
 * @return
 * nss_ctx_instance* NSS context
 */
struct nss_ctx_instance *nss_register_wifili_radio_if(uint32_t if_num, nss_wifili_callback_t wifi_callback,
			nss_wifili_callback_t wifi_ext_callback, nss_wifili_msg_callback_t event_callback, struct net_device *netdev, uint32_t features);

/**
 * nss_unregister_wifili_radio_if
 *	Deregister wifili radio interface with NSS.
 *
 * @param[in] if_num NSS interface number
 *
 * @return
 * void
 */
void nss_unregister_wifili_radio_if(uint32_t if_num);

/**
 * nss_get_available_wifili_external_if
 *	Check and return the available external interface.
 *
 * @return
 * External interface number.
 */
nss_if_num_t nss_get_available_wifili_external_if(void);

/**
 * nss_wifili_release_external_if
 *	Release the used interface number
 *
 * @datatypes
 * nss_if_num
 *
 * @param[in] if_num             NSS interface number.
 *
 * @return
 * void
 */
void nss_wifili_release_external_if(nss_if_num_t ifnum);

/**
 * nss_wifili_thread_scheme_alloc
 *	Allocate thread scheme entry and return scheme index.
 *
 * @param[in] nss_ctx  NSS context pointer.
 * @param[in] radio_ifnum  Radio interface number.
 * @param[in] radio_priority  Radio Priority requested.
 *
 * @return
 * uint8_t.
 */
uint8_t nss_wifili_thread_scheme_alloc(struct nss_ctx_instance *nss_ctx,
				int32_t radio_ifnum,
				uint32_t radio_priority);

/**
 * nss_wifili_thread_scheme_dealloc
 *	Release thread scheme database entry.
 *
 * @param[in] nss_ctx  NSS context pointer.
 * @param[in] radio_ifnum  Radio interface number.
 *
 * @return
 * void.
 */
void nss_wifili_thread_scheme_dealloc(struct nss_ctx_instance *nss_ctx,
				int32_t radio_ifnum);

/**
 * nss_wifili_get_radio_num
 *    Get radio number.
 *
 * @param[in] nss_ctx  NSS context pointer.
 *
 * @return
 * uint32_t.
 */
uint32_t nss_wifili_get_radio_num(struct nss_ctx_instance *nss_ctx);

/**
 * nss_wifili_stats_register_notifier
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
extern int nss_wifili_stats_register_notifier(struct notifier_block *nb);

/**
 * nss_wifili_stats_unregister_notifier
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
extern int nss_wifili_stats_unregister_notifier(struct notifier_block *nb);

#endif /*__KERNEL__ */

/**
 * @}
 */

#endif /* __NSS_WIFILI_H */

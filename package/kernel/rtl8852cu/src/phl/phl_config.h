/******************************************************************************
 *
 * Copyright(c) 2019 - 2021 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *****************************************************************************/
#ifndef _PHL_CONFIG_H_
#define _PHL_CONFIG_H_

/* Define correspoding PHL Feature based on information from the Core */
#ifdef PHL_PLATFORM_AP
#define PHL_FEATURE_AP
#elif defined(PHL_PLATFORM_LINUX) || defined(PHL_PLATFORM_WINDOWS)
#define PHL_FEATURE_NIC
#else
#define PHL_FEATURE_NONE
#endif

/******************* PLATFORM Section **************************/
#ifdef PHL_FEATURE_NONE/* enable compile flag for phl only compilation check */
	#define CONFIG_DFS 1
	#ifdef CONFIG_USB_HCI
		#define CONFIG_USB_TX_AGGREGATION
		#define CONFIG_USB_RX_AGGREGATION
		#define CONFIG_USB_TX_PADDING_CHK
	#endif
	#define CONFIG_LOAD_PHY_PARA_FROM_FILE

	#define CONFIG_WOW
	#define CONFIG_WPA3_SUITEB_SUPPORT
	#define CONFIG_SYNC_INTERRUPT

	#define CONFIG_MR_SUPPORT
	#ifdef CONFIG_MR_SUPPORT
		#if !defined(PHL_PLATFORM_LINUX_FREESTANDING)
		#define CONFIG_DBCC_SUPPORT
		#endif
		#define DBG_PHL_CHAN
		#define DBG_PHL_MR
		#define PHL_MR_PROC_CMD
		#define DBG_CHCTX_RMAP
	#endif /*CONFIG_MR_SUPPORT*/
#if defined(CONFIG_MR_SUPPORT) || defined(CONFIG_BTCOEX)
	#define CONFIG_MR_COEX_SUPPORT
#endif /* CONFIG_MR_SUPPORT || CONFIG_BTCOEX */
#ifdef CONFIG_MR_COEX_SUPPORT
	#define CONFIG_MCC_SUPPORT
	#ifdef CONFIG_MCC_SUPPORT
		#define MCC_ROLE_NUM 2
		#define RTW_WKARD_GO_BT_TS_ADJUST_VIA_NOA
		#define RTW_WKARD_HALRF_MCC
		#define RTW_WKARD_TDMRA_AUTO_GET_STAY_ROLE
	#endif /*CONFIG_MCC_SUPPORT*/
#endif/* CONFIG_MR_COEX_SUPPORT */

	#define DBG_PHL_MAC_REG_RW

	#define CONFIG_RTW_ACS
	#define CONFIG_RX_PSTS_PER_PKT

	#define CONFIG_PHL_TXSC
	#define CONFIG_RTW_TXSC_USE_HW_SEQ
	#define DEBUG_PHL_TX
	#define PHL_RXSC_AMPDU
	#define RTW_PHL_BCN
	#define CONFIG_PHL_SDIO_RX_NETBUF_ALLOC_IN_PHL
	#define CONFIG_PHL_TWT
	#define CONFIG_CMD_DISP
	#ifdef CONFIG_CMD_DISP
		#define CONFIG_PHL_ECSA
		/*#define CONFIG_CMD_DISP_SOLO_MODE*/
		#define CONFIG_PHL_CMD_SCAN
		#define CONFIG_PHL_CMD_SER
		#define CONFIG_PHL_CMD_BTC
		#define CONFIG_PHL_CMD_BF
	#endif
	#ifdef CONFIG_PCI_HCI
		#define PCIE_TRX_MIT_EN
		#define PHL_RXSC_ISR
		#define CONFIG_RTW_MULTI_DEV_MULTI_BAND
	#endif
	#define DEBUG_PHL_RX
	#define CONFIG_PHL_P2PPS
	#define RTW_WKARD_BFEE_SET_AID
	#define CONFIG_PHL_THERMAL_PROTECT
	#define CONFIG_PHL_TX_DBG
	#define CONFIG_PHL_RELEASE_RPT_ENABLE
	#define PHL_WATCHDOG_REFINE
	#define CONFIG_PHL_DRV_HAS_NVM
	#define CONFIG_RTW_OS_HANDLER_EXT
	#define CONFIG_RTW_DEBUG_BCN_TX
	#define CONFIG_RTW_SUPPORT_MBSSID_VAP
	#define CONFIG_LIMITED_VAP_NUM (3)
	#define CONFIG_PHL_FW_DUMP_EFUSE
	#define CONFIG_PHL_CHANNEL_INFO
	#ifdef CONFIG_PHL_CHANNEL_INFO
		#define CONFIG_PHL_WKARD_CHANNEL_INFO_ACK
	#endif
#endif /* PHL_FEATURE_NONE */

#ifdef PHL_PLATFORM_WINDOWS
	#ifndef CONFIG_CMD_DISP
		#define CONFIG_CMD_DISP
	#endif
	#define DRV_BB_CNSL_CMN_INFO
	#ifndef RTW_WD_PAGE_USE_SHMEM_POOL
		#define RTW_WD_PAGE_USE_SHMEM_POOL
	#endif

	#ifdef CONFIG_POWER_SAVE
	#define CONFIG_HW_RADIO_ONOFF_DETECT
	#endif
	#define DBG_DUMP_CMAC_CFG_INFO
#endif

#ifdef PHL_PLATFORM_LINUX
#ifdef CONFIG_PCI_HCI
/* Driver needs to handle cache-memory coherence for DMA */
#ifndef CONFIG_RTW_DMA_IS_COHERENT
#define PHL_DMA_NONCOHERENT
#endif
/* DMA is IOMMU. map/unmap must be paired, including size */
#ifdef CONFIG_RTW_UNPAIRED_DMA_MAP_UNMAP
#define PHL_UNPAIRED_DMA_MAP_UNMAP
#endif
/* DMA address is 64-bit */
#ifdef CONFIG_ARCH_DMA_ADDR_T_64BIT
#define PHL_DMA_ADDR_64
#endif
#endif /* CONFIG_PCI_HCI */
	/* comment out cfg temporarily */
	/*
	#define CONFIG_FSM

	#ifndef CONFIG_FSM
		#define CONFIG_CMD_DISP
	#endif
	*/
#endif

/******************* Feature flags **************************/
#define MAC_PHL_TEMP_CROSS_DEFINE

#ifdef CONFIG_PHL_TEST_SUITE
#define CONFIG_PHL_TEST_MP
#define CONFIG_PHL_TEST_VERIFY
#endif

#ifdef CONFIG_CORE_SYNC_INTERRUPT
#define CONFIG_SYNC_INTERRUPT
#endif

#define CONFIG_PHL_SCANOFLD

#ifdef CONFIG_WOW
#define CONFIG_WOWLAN
/* #define RTW_WKARD_WOW_SKIP_AOAC_RPT */
/* #define RTW_WKARD_WOW_SKIP_WOW_CAM_CONFIG */
#define RTW_WKARD_WOW_L2_PWR
#define DBG_RST_BDRAM_TIME
#ifndef CONFIG_PHL_SCANOFLD
#define CONFIG_PHL_SCANOFLD
#endif
#endif

#define DBG_PHY_ON_TIME

/*CONFIG_IFACE_NUMBER*/
#ifdef CONFIG_IFACE_NUMBER
#define MAX_WIFI_ROLE_NUMBER CONFIG_IFACE_NUMBER
#else
#define MAX_WIFI_ROLE_NUMBER 5
#endif

#if defined(CONFIG_RTW_SUPPORT_MBSSID_VAP) && defined(CONFIG_LIMITED_VAP_NUM)
#define MAX_MBSSID_NUMBER       (CONFIG_LIMITED_VAP_NUM)
#else
#define MAX_MBSSID_NUMBER       (0)
#endif /* CONFIG_RTW_SUPPORT_MBSSID_VAP */

#ifdef CONFIG_CONCURRENT_MODE
#define CONFIG_MR_SUPPORT
#endif

#ifdef CONFIG_REUSED_FWDL_BUF
	#define CONFIG_PHL_REUSED_FWDL_BUF
#endif

#ifdef CONFIG_MR_SUPPORT
	/*#define CONFIG_DBCC_SUPPORT*/
	#ifdef CONFIG_DBCC_SUPPORT
		/*#define DBG_DBCC_MONITOR_TIME*/
		#ifdef DBG_DBCC_MONITOR_TIME
			#define DBG_MONITOR_TIME
		#endif
	#endif
	#define DBG_PHL_CHAN
	#define DBG_PHL_MR
	#define PHL_MR_PROC_CMD
	#define DBG_CHCTX_RMAP
#endif
#if defined(CONFIG_MR_SUPPORT) || defined(CONFIG_BTCOEX)
	#define CONFIG_MR_COEX_SUPPORT
#endif /* CONFIG_MR_SUPPORT || CONFIG_BTCOEX */
#ifdef CONFIG_MR_COEX_SUPPORT
	#define CONFIG_MCC_SUPPORT
	#ifdef CONFIG_MCC_SUPPORT
		#define MCC_ROLE_NUM 2
		#define RTW_WKARD_GO_BT_TS_ADJUST_VIA_NOA
		#define RTW_WKARD_HALRF_MCC
		#define RTW_WKARD_TDMRA_AUTO_GET_STAY_ROLE
	#endif /*CONFIG_MCC_SUPPORT*/
#endif /* CONFIG_MR_COEX_SUPPORT */


#define DBG_PHL_STAINFO
#define PHL_MAX_STA_NUM 128
#define PHL_MAX_MLD_NUM (PHL_MAX_STA_NUM)

/**** CONFIG_CMD_DISP ***/
#ifdef DISABLE_CMD_DISPR
#undef CONFIG_CMD_DISP
#endif

#ifdef CONFIG_CMD_DISP
/* enable SOLO mode define to create seperated background thread per dispatcher,
 * otherwise, all dispatcher would share single background thread, which is in share mode.
*/
/*#define CONFIG_CMD_DISP_SOLO_MODE*/

/* Enable Self-Defined Sequence feature for sender to rearrange dispatch order,
 * Since this is not a mandatory feature and would have addiional memory cost (arround 2200 Bytes)
 * Disable by default.
*/
/*#define CONFIG_CMD_DISP_SUPPORT_CUSTOM_SEQ*/

#define CONFIG_PHL_CMD_SCAN

#ifdef CONFIG_CMD_SER
#define CONFIG_PHL_CMD_SER
#endif

#define CONFIG_PHL_CMD_BF

#ifdef CONFIG_MSG_NUM
	#define CONFIG_PHL_MSG_NUM CONFIG_MSG_NUM
#endif

#endif /**** CONFIG_CMD_DISP ***/

#define CONFIG_GEN_GIT_INFO 1
/*#define CONFIG_NEW_HALMAC_INTERFACE*/

/* AP mode not suppot BTC currently */
#ifndef PHL_FEATURE_AP
#define CONFIG_BTCOEX
#define CONFIG_PHL_CMD_BTC
#endif /* PHL_FEATURE_AP */

#ifdef CONFIG_USB_TX_PADDING_CHK
#define CONFIG_PHL_USB_TX_PADDING_CHK
#endif

#ifdef CONFIG_USB_TX_AGGREGATION
#define CONFIG_PHL_USB_TX_AGGREGATION
#endif

#ifdef CONFIG_USB_RX_AGGREGATION
#define CONFIG_PHL_USB_RX_AGGREGATION
#endif

#if CONFIG_DFS
#define CONFIG_PHL_DFS_SWITCH_CH_WITH_CSA
#ifdef CONFIG_DFS_MASTER
#define CONFIG_PHL_DFS
#endif
#endif

#ifdef CONFIG_WPP
#define CONFIG_PHL_WPP
#endif

#ifdef CONFIG_TCP_CSUM_OFFLOAD_RX
#define CONFIG_PHL_CSUM_OFFLOAD_RX
#endif

#ifdef CONFIG_RX_PSTS_PER_PKT
#define CONFIG_PHL_RX_PSTS_PER_PKT
#define RTW_WKARD_DISABLE_PSTS_PER_PKT_DATA
#endif

#ifdef CONFIG_SDIO_RX_NETBUF_ALLOC_IN_PHL
#define CONFIG_PHL_SDIO_RX_NETBUF_ALLOC_IN_PHL
#endif

#ifdef CONFIG_SDIO_READ_RXFF_IN_INT
#define CONFIG_PHL_SDIO_READ_RXFF_IN_INT
#endif

#ifdef CONFIG_ECSA
#define CONFIG_PHL_ECSA
#ifdef CONFIG_ECSA_EXTEND_OPTION
#define CONFIG_PHL_ECSA_EXTEND_OPTION
#endif
#endif

#ifdef CONFIG_TWT
#define CONFIG_PHL_TWT
#endif

#ifdef CONFIG_RA_TXSTS_DBG
#define CONFIG_PHL_RA_TXSTS_DBG
#endif

#ifdef CONFIG_RELEASE_RPT
#define CONFIG_PHL_RELEASE_RPT_ENABLE
#endif

#ifdef CONFIG_PS_FW_DBG
#define CONFIG_PHL_PS_FW_DBG
#endif

#ifdef CONFIG_P2PPS
#define CONFIG_PHL_P2PPS
#endif

#ifdef CONFIG_WD_PAGE_RESET
#define CONFIG_PHL_WD_PAGE_RESET
#endif

#ifdef CONFIG_TX_DBG
#define CONFIG_PHL_TX_DBG
#endif
#ifdef CONFIG_PCI_HCI
#ifdef CONFIG_PCIE_TRX_MIT
#define PCIE_TRX_MIT_EN
#endif
#endif
#ifdef CONFIG_THERMAL_PROTECT
#define CONFIG_PHL_THERMAL_PROTECT
#endif

#ifdef CONFIG_RX_BATCH_IND
#define PHL_RX_BATCH_IND
#endif

#ifdef CONFIG_TDLS
#define CONFIG_PHL_TDLS
#endif

#ifdef CONFIG_SDIO_HCI
/* For SDIO TX TP TST - START */
#ifdef CONFIG_SDIO_TX_AGG_NUM_MAX
#define PHL_SDIO_TX_AGG_MAX	CONFIG_SDIO_TX_AGG_NUM_MAX
#endif /* CONFIG_SDIO_TX_AGG_NUM_MAX */
#define CONFIG_PHL_SDIO_TX_CB_THREAD
/*
 * RTW_WKARD_SDIO_TX_USE_YIELD
 * Define this would use yield() instead of event wait mechanism to improve
 * throughput on Linux platform.
 * But yield() doesn't been encouraged to use in Linux,
 * so if we figure out what happened and find another way to improve
 * throughput, this workaround would be removed later.
 *
 * RTW_WKARD_SDIO_TX_USE_YIELD is depended on CONFIG_PHL_SDIO_TX_CB_THREAD,
 * because the mechanism only been used with CONFIG_PHL_SDIO_TX_CB_THREAD.
 *
 * Usually this flag would be defined from core layer.
 */
/*#define RTW_WKARD_SDIO_TX_USE_YIELD*/
#define SDIO_TX_THREAD			/* Use dedicate thread for SDIO TX */
/* For SDIO TX TP TST - ENDT */

/* For SDIO RX TP TST - START */
#define CONFIG_PHL_SDIO_RX_CB_THREAD
/* For SDIO RX TP TST - END */

#endif /* CONFIG_SDIO_HCI */

#ifdef CONFIG_MAC_REG_RW_CHK
#define DBG_PHL_MAC_REG_RW
#endif

#ifdef CONFIG_RTW_DRV_HAS_NVM
/* Driver has it's own NVM to store MP calibration values
 * and system configuration instead of efuse. */
#define CONFIG_PHL_DRV_HAS_NVM
#endif /* CONFIG_RTW_DRV_HAS_NVM */

#ifdef CONFIG_PCI_TRX_RES_DBG
#define CONFIG_PHL_PCI_TRX_RES_DBG
#endif

#ifdef CONFIG_ACS
#define CONFIG_RTW_ACS
#endif

#ifdef CONFIG_RTW_CSI_CHANNEL_INFO
#define CONFIG_PHL_CHANNEL_INFO /*WiFi Sensing*/
#ifdef CONFIG_PHL_CHANNEL_INFO
	#define CONFIG_PHL_WKARD_CHANNEL_INFO_ACK
#endif
#define CONFIG_PHL_CHANNEL_INFO_DBG
#endif

#ifdef CONFIG_FW_DUMP_EFUSE
#define CONFIG_PHL_FW_DUMP_EFUSE
#endif

#ifdef CONFIG_PHL_WACHDOG_REFINE
#define PHL_WATCHDOG_REFINE
#endif

#ifdef CONFIG_NARROWBAND_SUPPORTING
#define CONFIG_PHL_NARROW_BW
#endif /*CONFIG_NARROWBAND_SUPPORTING*/

/******************* WKARD flags **************************/
#define RTW_WKARD_P2PPS_REFINE
#define RTW_WKARD_P2PPS_SINGLE_NOA
#define RTW_WKARD_P2PPS_NOA_MCC

#ifdef PHL_PLATFORM_LINUX
#define RTW_WKARD_RF_CR_DUMP
#define RTW_WKARD_LINUX_CMD_WKARD
#endif

#ifdef PHL_PLATFORM_WINDOWS
/* Workaround for doing hal reset in changing MP mode will lost the mac entry */
#define RTW_WKARD_MP_MODE_CHANGE
#define RTW_WKARD_WIN_TRX_BALANCE
#define RTW_WKARD_DYNAMIC_LTR
#endif

#ifdef PHL_PLATFORM_AP
#define RTW_WKARD_98D_INTR_EN_TIMING
#define RTW_WKARD_AP_MP
#endif


#define RTW_WKARD_PHY_CAP

#define RTW_WKARD_BTC_STBC_CAP

#define RTW_WKARD_LAMODE

#define RTW_WKARD_TXSC

#define RTW_WKARD_BB_C2H

#define RTW_WKARD_STA_BCN_INTERVAL

#define RTW_WKARD_SER_L1_EXPIRE

#define RTW_WKARD_RESET_LPS_STS

#ifdef CONFIG_USB_HCI
#define RTW_WKARD_SER_USB_POLLING_EVENT
#endif

/* #define RTW_WKARD_SER_USB_DISABLE_L1_RCVY_FLOW */

#define RTW_WKARD_BTC_RFETYPE

#define RTW_WKARD_TXBD_UPD_LMT 	/* 8852AE/8852BE txbd index update limitation */

#ifdef CONFIG_WPA3_SUITEB_SUPPORT
#define RTW_WKARD_HW_MGNT_GCMP_256_DISABLE
#endif

/* Workaround for cmac table config
 * - Default is disabled until halbb is ready
 * - This workaround will be removed once fw handles this cfg
 */
/*#define RTW_WKARD_DEF_CMACTBL_CFG*/

/* Workaround for cmac table config
 * - This workaround will be removed once fw handles this cfg
 */
#define RTW_WKARD_DEF_CMACTBL_CFG

/* Workaround for NICCE FW to remove LPS-PG and IPS-PG
 * - This workaround will remove PG when using SCC_TURBO FW
 *   in NICCE
 */
#ifdef MAC_FW_CATEGORY_NICCE
#define RTW_WKARD_NICCE_FW_DIS_PG
#endif

#define RTW_WKARD_USB_TXAGG_BULK_END_WD
#ifdef CONFIG_HOMOLOGATION
#define CONFIG_PHL_HOMOLOGATION
#endif

#ifdef RTW_WKARD_TX_DISABLE_BFEE
#define RTW_WKARD_DYNAMIC_BFEE_CAP
#endif

#ifdef RTW_WKARD_NTFY_MEDIA_STS
#define RTW_WKARD_PHL_NTFY_MEDIA_STS
#endif

#ifdef PHL_PLATFORM_WINDOWS
#define CONFIG_WOW_WITH_SER
#endif

#ifdef PHL_PLATFORM_WINDOWS
#define CONFIG_PHL_PATH_DIV
#endif
/*
 * Workaround for phl_mr_offch_hdl sleep after issue null data,
 * - This workaround will be removed once tx report is ready
 */
#ifndef RTW_WKARD_TX_NULL_WD_RP
#define RTW_WKARD_ISSUE_NULL_SLEEP_PROTECTION
#endif
#ifdef RTW_WKARD_LPS_IQK_TWICE
#define RTW_WKARD_PHL_LPS_IQK_TWICE
#endif

#define RTW_WKARD_BUSCAP_IN_HALSPEC

#define RTW_WKARD_IBSS_SNIFFER_MODE


#define RTW_WKARD_SINGLE_PATH_RSSI

#define RTW_WKARD_BFEE_DISABLE_NG16

#define RTW_WKARD_HW_WMM_ALLOCATE

#ifdef RTW_WKARD_BFEE_AID
#define RTW_WKARD_BFEE_SET_AID
#endif

#ifdef RTW_WKARD_TRIGGER_FRAME_PARSER
#define RTW_WKARD_RX_FLTR_HE_TF
#endif

#define RTW_WKARD_PHY_RPT_CCK_CH_IDX
#ifdef RTW_WKARD_DISABLE_2G40M_ULOFDMA
#define RTW_WKARD_BB_DISABLE_STA_2G40M_ULOFDMA
#endif

#define RTW_WKARD_WPOFFSET

#ifdef PHL_PLATFORM_LINUX
#define RTW_TX_COALESCE_BAK_PKT_LIST
#endif

#endif /*_PHL_CONFIG_H_*/

/******************************************************************************
 *
 * Copyright(c) 2015 - 2021 Realtek Corporation.
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
#define CONFIG_SINGLE_IMG
/***** temporarily flag *******/
/*#define CONFIG_NO_FW*/
/*#define CONFIG_DISABLE_ODM*/

#define RTW_WKARD_CORE_RSSI_V1
#ifdef RTW_WKARD_CORE_RSSI_V1
#define CONFIG_RX_PSTS_PER_PKT
#define CONFIG_SIGNAL_STAT_PROCESS
/*#define DBG_RX_SIGNAL_DISPLAY_PROCESSING*/
#define DBG_RX_SIGNAL_DISPLAY_SSID_MONITORED "rtw_ap"
#endif


//#ifndef DBG_MEM_ALLOC
//#define DBG_MEM_ALLOC

//#define DBG_PHL_MEM_ALLOC
//#define DBG_HAL_MAC_MEM_MOINTOR
//#define DBG_HAL_MEM_MOINTOR
//#endif
/*#define CONFIG_PHL_USE_KMEM_ALLOC*/

/*
 * Work around Config
 */
#define RTW_WKARD_DIS_PROBE_REQ_RPT_TO_HOSTAPD

#ifdef CONFIG_BTC
#define RTK_WKARD_CORE_BTC_STBC_CAP
#endif

#define RTW_WKARD_LIMIT_MAX_TXAGG

#define CONFIG_MSG_NUM 128
/*
 * Public  General Config
 */
#define AUTOCONF_INCLUDED
#define DRV_NAME "rtl8852cu"

#define CONFIG_USB_HCI

/*
 * Wi-Fi Functions Config
 */


#define CONFIG_80211N_HT
#define CONFIG_80211AC_VHT
#define CONFIG_80211AX_HE
#ifdef CONFIG_80211AC_VHT
	#ifndef CONFIG_80211N_HT
		#define CONFIG_80211N_HT
	#endif
#endif
#ifdef CONFIG_80211AX_HE
	#ifndef CONFIG_80211N_HT
		#define CONFIG_80211N_HT
	#endif
	#ifndef CONFIG_80211AC_VHT
		#define CONFIG_80211AC_VHT
	#endif
#endif

#define CONFIG_BEAMFORMING
#ifdef CONFIG_BEAMFORMING
/*#define RTW_WKARD_TX_DISABLE_BFEE*/
#endif

/* set CONFIG_IOCTL_CFG80211 from Makefile */
#ifdef CONFIG_IOCTL_CFG80211
	/*
	 * Indecate new sta asoc through cfg80211_new_sta
	 * If kernel version >= 3.2 or
	 * version < 3.2 but already apply cfg80211 patch,
	 * RTW_USE_CFG80211_STA_EVENT must be defiend!
	 */
	/* Set RTW_USE_CFG80211_STA_EVENT from Makefile */
	/* #define RTW_USE_CFG80211_STA_EVENT */ /* Indecate new sta asoc through cfg80211_new_sta */
	#define CONFIG_CFG80211_FORCE_COMPATIBLE_2_6_37_UNDER
	/* #define CONFIG_DEBUG_CFG80211 */
	#define CONFIG_SET_SCAN_DENY_TIMER
#endif

#define CONFIG_TX_AMSDU
#ifdef CONFIG_TX_AMSDU
	#define CONFIG_TX_AMSDU_SW_MODE	1
#endif

#define CONFIG_HW_RTS

/*
 * Internal  General Config
 */
#define CONFIG_EMBEDDED_FWIMG
#ifdef CONFIG_EMBEDDED_FWIMG
	#define	LOAD_FW_HEADER_FROM_DRIVER
#endif
/* #define CONFIG_FILE_FWIMG */

#define CONFIG_XMIT_ACK
#ifdef CONFIG_XMIT_ACK
	#define CONFIG_XMIT_ACK_BY_CCX_RPT
	#ifdef CONFIG_XMIT_ACK_BY_CCX_RPT
		#define RTW_WKARD_CCX_RPT_LIMIT_CTRL
		#define CONFIG_PHL_DEFAULT_MGNT_Q_RPT_EN
	#endif
	#define CONFIG_ACTIVE_KEEP_ALIVE_CHECK
	#define RTW_WKARD_TX_NULL_WD_RP
#endif

/*#define CONFIG_RECV_REORDERING_CTRL*/

#define CONFIG_USB_INTERRUPT_IN_PIPE

/* #define CONFIG_SIGNAL_STAT_PROCESS */

#ifdef CONFIG_POWER_SAVE
	/* #define CONFIG_RTW_IPS */
	/* #define CONFIG_RTW_LPS */

	#if defined(CONFIG_RTW_IPS) || defined(CONFIG_RTW_LPS)
		#define CONFIG_PS_FW_DBG
	#endif
#endif /* CONFIG_POWER_SAVE */

	/* before link */
	/* #define CONFIG_ANTENNA_DIVERSITY */

	/* after link */
	#ifdef CONFIG_ANTENNA_DIVERSITY
	#define CONFIG_HW_ANTENNA_DIVERSITY
	#endif


/*#else*/	/* CONFIG_MP_INCLUDED */

/*#endif*/	/* CONFIG_MP_INCLUDED */

#define CONFIG_AP_MODE
#ifdef CONFIG_AP_MODE
	/* #define CONFIG_INTERRUPT_BASED_TXBCN */ /* Tx Beacon when driver BCN_OK ,BCN_ERR interrupt occurs */
	#if defined(CONFIG_CONCURRENT_MODE) && defined(CONFIG_INTERRUPT_BASED_TXBCN)
		#undef CONFIG_INTERRUPT_BASED_TXBCN
	#endif
	#ifdef CONFIG_INTERRUPT_BASED_TXBCN
		/* #define CONFIG_INTERRUPT_BASED_TXBCN_EARLY_INT */
		#define CONFIG_INTERRUPT_BASED_TXBCN_BCN_OK_ERR
	#endif

	#define CONFIG_NATIVEAP_MLME
	#ifndef CONFIG_NATIVEAP_MLME
		#define CONFIG_HOSTAPD_MLME
	#endif
	/* #define CONFIG_RTW_HOSTAPD_ACS */
	/*#define CONFIG_FIND_BEST_CHANNEL*/
#endif

#define CONFIG_P2P
#ifdef CONFIG_P2P
	/* The CONFIG_WFD is for supporting the Wi-Fi display */
	#define CONFIG_WFD

	#define CONFIG_P2P_REMOVE_GROUP_INFO

	/* #define CONFIG_DBG_P2P */

	#define CONFIG_P2P_PS
	/* #define CONFIG_P2P_IPS */
	#define CONFIG_P2P_OP_CHK_SOCIAL_CH
	#define CONFIG_CFG80211_ONECHANNEL_UNDER_CONCURRENT  /* replace CONFIG_P2P_CHK_INVITE_CH_LIST flag */
	/*#define CONFIG_P2P_INVITE_IOT*/
#endif

/*	Added by Kurt 20110511 */
#ifdef CONFIG_TDLS
	#define CONFIG_TDLS_DRIVER_SETUP
/*
	#ifndef CONFIG_WFD
		#define CONFIG_WFD
	#endif
*/
	/* #define CONFIG_TDLS_AUTOSETUP */
	#define CONFIG_TDLS_AUTOCHECKALIVE
	/* #define CONFIG_TDLS_CH_SW */	/* Not support yet */
#endif


#define CONFIG_SKB_COPY /* amsdu */

/*#define CONFIG_RTW_LED*/
#ifdef CONFIG_RTW_LED
	#define CONFIG_RTW_SW_LED
	#ifdef CONFIG_RTW_SW_LED
		/* #define CONFIG_RTW_LED_HANDLED_BY_CMD_THREAD */
	#endif
#endif /* CONFIG_RTW_LED */

#define CONFIG_GLOBAL_UI_PID

/*#define CONFIG_RTW_80211K*/

/*#define CONFIG_ADAPTOR_INFO_CACHING_FILE */ /* now just applied on 8192cu only, should make it general... */
/*#define CONFIG_RESUME_IN_WORKQUEUE */
/*#define CONFIG_SET_SCAN_DENY_TIMER */
#define CONFIG_LONG_DELAY_ISSUE
/* #define CONFIG_SIGNAL_DISPLAY_DBM */ /*display RX signal with dbm */
#ifdef CONFIG_SIGNAL_DISPLAY_DBM
/* #define CONFIG_BACKGROUND_NOISE_MONITOR */
#endif



/*
 * Interface  Related Config
 */

#ifndef CONFIG_MINIMAL_MEMORY_USAGE
	#define CONFIG_USB_TX_AGGREGATION
	#define CONFIG_USB_RX_AGGREGATION
#endif

#ifndef CONFIG_DYNAMIC_RX_BUF
#define CONFIG_DYNAMIC_RX_BUF
#endif

/* #define CONFIG_REDUCE_USB_TX_INT*/ /* Trade-off: Improve performance, but may cause TX URBs blocked by USB Host/Bus driver on few platforms. */

/*
 * CONFIG_USE_USB_BUFFER_ALLOC_XX uses Linux USB Buffer alloc API and is for Linux platform only now!
 */
/* #define CONFIG_USE_USB_BUFFER_ALLOC_TX*/	/* Trade-off: For TX path, improve stability on some platforms, but may cause performance degrade on other platforms. */
/* #define CONFIG_USE_USB_BUFFER_ALLOC_RX*/ /* For RX path */

/*
 * USB VENDOR REQ BUFFER ALLOCATION METHOD
 * if not set we'll use function local variable (stack memory)
 */
/* #define CONFIG_USB_VENDOR_REQ_BUFFER_DYNAMIC_ALLOCATE */
#define CONFIG_USB_VENDOR_REQ_BUFFER_PREALLOC

#define CONFIG_USB_VENDOR_REQ_MUTEX
#define CONFIG_VENDOR_REQ_RETRY

/* #define CONFIG_USB_SUPPORT_ASYNC_VDN_REQ*/

/*
 * If bus rate is lower than Wi-Fi phy rate, it probably causes unstable
 * throughput for rx. So disable AMSDU may be suggested by SD1.
 */
#define CONFIG_DISBALE_RX_AMSDU_FOR_BUS_LOW_SPEED

/*
 * HAL  Related Config
 */
#define CONFIG_RX_PACKET_APPEND_FCS

#define DISABLE_BB_RF	0

#ifdef CONFIG_MP_INCLUDED
	#define MP_DRIVER 1
	/*
	 #undef CONFIG_USB_TX_AGGREGATION
	 #undef CONFIG_USB_RX_AGGREGATION
	*/
#else
	#define MP_DRIVER 0
#endif

/*
 * Platform  Related Config
 */
#if defined(CONFIG_PLATFORM_ACTIONS_ATM702X)
	#ifdef CONFIG_USB_TX_AGGREGATION
		#undef CONFIG_USB_TX_AGGREGATION
	#endif
	#ifndef CONFIG_USE_USB_BUFFER_ALLOC_TX
		#define CONFIG_USE_USB_BUFFER_ALLOC_TX
	#endif
	#ifndef CONFIG_USE_USB_BUFFER_ALLOC_RX
		#define CONFIG_USE_USB_BUFFER_ALLOC_RX
	#endif
#endif

#ifdef CONFIG_USB_TX_AGGREGATION
/* #define CONFIG_TX_EARLY_MODE */
#endif

#define MAX_XMITBUF_SZ	20480
#define NR_XMITBUFF	4
#define MAX_MGNT_XMITBUF_SZ	1536
#define NR_MGNT_XMITBUFF	32
#define MAX_RECVBUF_SZ 32768
#define NR_RECVBUFF 128
#define NR_RECV_URB 8

/*
 * Reduce FW code size
 */
#define CONFIG_FW_SPECIFY_FROM_CORE
#ifdef CONFIG_FW_SPECIFY_FROM_CORE
	#ifdef CONFIG_WOWLAN
		#define MAC_FW_CATEGORY_WOWLAN
	#endif
	#define MAC_FW_8852C_U2
	#define MAC_FW_CATEGORY_NIC
#endif

/*
 * Debug Related Config
 */
#define RTW_DETECT_HANG
#define DBG	1

/*#define DBG_CONFIG_ERROR_DETECT*/

/* #define CONFIG_DIS_UPHY */
/*
#define DBG_CONFIG_ERROR_DETECT_INT
#define DBG_CONFIG_ERROR_RESET

#define DBG_IO
#define DBG_DELAY_OS
#define DBG_MEM_ALLOC
#define DBG_IOCTL

#define DBG_TX
#define DBG_XMIT_BUF
#define DBG_XMIT_BUF_EXT
#define DBG_TX_DROP_FRAME

#define DBG_RX_DROP_FRAME
#define DBG_RX_SEQ
#define DBG_RX_SIGNAL_DISPLAY_PROCESSING
#define DBG_RX_SIGNAL_DISPLAY_SSID_MONITORED "jeff-ap"


#define DBG_ROAMING_TEST

#define DBG_HAL_INIT_PROFILING

#define DBG_MEMORY_LEAK
*/

/*#define DBG_FW_DEBUG_MSG_PKT*/  /* FW use this feature to tx debug broadcast pkt. This pkt include FW debug message*/

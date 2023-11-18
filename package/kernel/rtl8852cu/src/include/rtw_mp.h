/******************************************************************************
 *
 * Copyright(c) 2007 - 2019 Realtek Corporation.
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
#ifndef _RTW_MP_H_
#define _RTW_MP_H_

#include <drv_types.h>

#define RTWPRIV_VER_INFO	1

#define MAX_MP_XMITBUF_SZ	2048
#define NR_MP_XMITFRAME		8
#define MP_READ_REG_MAX_OFFSET 0x4FFF

#define TX_POWER_BASE 4  /* dbm * 4 */
#define TX_POWER_CODE_WORD_BASE 8 /* dbm * 8 */

struct mp_xmit_frame {
	_list	list;

	struct pkt_attrib attrib;

	struct sk_buff *pkt;

	int frame_tag;

	_adapter *padapter;

#ifdef CONFIG_USB_HCI

	/* insert urb, irp, and irpcnt info below... */
	/* max frag_cnt = 8 */
	u8 *mem_addr;
	u32 sz[8];
	u8 bpending[8];
	sint ac_tag[8];
	sint last[8];
	uint irpcnt;
	uint fragcnt;
#endif /* CONFIG_USB_HCI */

	uint mem[(MAX_MP_XMITBUF_SZ >> 2)];
};

struct mp_wiparam {
	u32 bcompleted;
	u32 act_type;
	u32 io_offset;
	u32 io_value;
};

typedef void(*wi_act_func)(void *padapter);

struct mp_tx {
	u8 stop;
	u32 count, sended;
	u8 payload;
	struct pkt_attrib attrib;
	/* struct tx_desc desc; */
	/* u8 resvdtx[7]; */
	u8 desc[TXDESC_SIZE];
	u8 *pallocated_buf;
	u8 *buf;
	u32 buf_size, write_size;
	_thread_hdl_ PktTxThread;
};

#define MP_MAX_LINES		1000
#define MP_MAX_LINES_BYTES	256


typedef struct _RT_PMAC_PKT_INFO {
	u8			MCS;
	u8			Nss;
	u8			Nsts;
	u32			N_sym;
	u8			SIGA2B3;
} RT_PMAC_PKT_INFO, *PRT_PMAC_PKT_INFO;

typedef struct _RT_PMAC_TX_INFO {
	u8			bEnPMacTx:1;		/* 0: Disable PMac 1: Enable PMac */
	u8			Mode:3;				/* 0: Packet TX 3:Continuous TX */
	u8			Ntx:4;				/* 0-7 */
	u8			TX_RATE;			/* MPT_RATE_E */
	u8			TX_RATE_HEX;
	u8			TX_SC;
	u8			bSGI:1;
	u8			bSPreamble:1;
	u8			bSTBC:1;
	u8			bLDPC:1;
	u8			NDP_sound:1;
	u8			BandWidth:3;		/* 0: 20 1:40 2:80Mhz */
	u8			m_STBC;			/* bSTBC + 1 */
	u16			PacketPeriod;
	u32		PacketCount;
	u32		PacketLength;
	u8			PacketPattern;
	u16			SFD;
	u8			SignalField;
	u8			ServiceField;
	u16			LENGTH;
	u8			CRC16[2];
	u8			LSIG[3];
	u8			HT_SIG[6];
	u8			VHT_SIG_A[6];
	u8			VHT_SIG_B[4];
	u8			VHT_SIG_B_CRC;
	u8			VHT_Delimiter[4];
	u8			MacAddress[6];
} RT_PMAC_TX_INFO, *PRT_PMAC_TX_INFO;

struct rtw_mp_giltf_data {
	u8 gi;
	u8 ltf;
	char type_str[8];
};

typedef void (*MPT_WORK_ITEM_HANDLER)(void *adapter);
typedef struct _MPT_CONTEXT {
	/* Indicate if we have started Mass Production Test. */
	BOOLEAN			bMassProdTest;

	/* Indicate if the driver is unloading or unloaded. */
	BOOLEAN			bMptDrvUnload;

	_sema			MPh2c_Sema;
	_timer			MPh2c_timeout_timer;
	/* Event used to sync H2c for BT control */

	BOOLEAN		MptH2cRspEvent;
	BOOLEAN		MptBtC2hEvent;
	BOOLEAN		bMPh2c_timeout;

	/* 8190 PCI does not support NDIS_WORK_ITEM. */
	/* Work Item for Mass Production Test. */
	/* NDIS_WORK_ITEM	MptWorkItem;
	*	RT_WORK_ITEM		MptWorkItem; */
	/* Event used to sync the case unloading driver and MptWorkItem is still in progress.
	*	NDIS_EVENT		MptWorkItemEvent; */
	/* To protect the following variables.
	*	NDIS_SPIN_LOCK		MptWorkItemSpinLock; */
	/* Indicate a MptWorkItem is scheduled and not yet finished. */
	BOOLEAN			bMptWorkItemInProgress;
	/* An instance which implements function and context of MptWorkItem. */
	MPT_WORK_ITEM_HANDLER	CurrMptAct;

	/* 1=Start, 0=Stop from UI. */
	u32			MptTestStart;
	/* _TEST_MODE, defined in MPT_Req2.h */
	u32			MptTestItem;
	/* Variable needed in each implementation of CurrMptAct. */
	u32			MptActType;	/* Type of action performed in CurrMptAct. */
	/* The Offset of IO operation is depend of MptActType. */
	u32			MptIoOffset;
	/* The Value of IO operation is depend of MptActType. */
	u32			MptIoValue;
	/* The RfPath of IO operation is depend of MptActType. */

	u32			mpt_rf_path;

	u8			MptChannelToSw;	/* Channel to switch. */
	u8			MptInitGainToSet;	/* Initial gain to set. */
	/* u32			bMptAntennaA;		 */ /* TRUE if we want to use antenna A. */
	u32			MptBandWidth;		/* bandwidth to switch. */

	u32			mpt_rate_index;/* rate index. */

	/* Register value kept for Single Carrier Tx test. */
	u8			btMpCckTxPower;
	/* Register value kept for Single Carrier Tx test. */
	u8			btMpOfdmTxPower;
	/* For MP Tx Power index */
	u8			TxPwrLevel[4];	/* rf-A, rf-B*/
	u32			RegTxPwrLimit;
	/* Content of RCR Regsiter for Mass Production Test. */
	u32			MptRCR;
	/* TRUE if we only receive packets with specific pattern. */
	BOOLEAN			bMptFilterPattern;
	/* Rx OK count, statistics used in Mass Production Test. */
	u32			MptRxOkCnt;
	/* Rx CRC32 error count, statistics used in Mass Production Test. */
	u32			MptRxCrcErrCnt;

	BOOLEAN			bCckContTx;	/* TRUE if we are in CCK Continuous Tx test. */
	BOOLEAN			bOfdmContTx;	/* TRUE if we are in OFDM Continuous Tx test. */
		/* TRUE if we have start Continuous Tx test. */
	BOOLEAN			is_start_cont_tx;

	/* TRUE if we are in Single Carrier Tx test. */
	BOOLEAN			bSingleCarrier;
	/* TRUE if we are in Carrier Suppression Tx Test. */

	BOOLEAN			is_carrier_suppression;

	/* TRUE if we are in Single Tone Tx test. */

	BOOLEAN			is_single_tone;


	/* ACK counter asked by K.Y.. */
	BOOLEAN			bMptEnableAckCounter;
	u32			MptAckCounter;

	/* SD3 Willis For 8192S to save 1T/2T RF table for ACUT	Only fro ACUT delete later ~~~! */
	/* s8		BufOfLines[2][MAX_LINES_HWCONFIG_TXT][MAX_BYTES_LINE_HWCONFIG_TXT]; */
	/* s8			BufOfLines[2][MP_MAX_LINES][MP_MAX_LINES_BYTES]; */
	/* s32			RfReadLine[2]; */

	u8		APK_bound[2];	/* for APK	path A/path B */
	BOOLEAN		bMptIndexEven;

	u8		backup0xc50;
	u8		backup0xc58;
	u8		backup0xc30;
	u8		backup0x52_RF_A;
	u8		backup0x52_RF_B;

	u32			backup0x58_RF_A;
	u32			backup0x58_RF_B;

	u8			h2cReqNum;
	u8			c2hBuf[32];

	u8          btInBuf[100];
	u32			mptOutLen;
	u8          mptOutBuf[100];
	RT_PMAC_TX_INFO	PMacTxInfo;
	RT_PMAC_PKT_INFO	PMacPktInfo;
	u8 HWTxmode;

	BOOLEAN			bldpc;
	BOOLEAN			bstbc;
} MPT_CONTEXT, *PMPT_CONTEXT;
/* #endif */


/* #define RTPRIV_IOCTL_MP					( SIOCIWFIRSTPRIV + 0x17) */
enum {
	WRITE_REG = 1,
	READ_REG,
	WRITE_RF,
	READ_RF,
	MP_START,
	MP_STOP,
	MP_RATE,
	MP_CHANNEL,
	MP_TRXSC_OFFSET,
	MP_BANDWIDTH,
	MP_TXPOWER,
	MP_ANT_TX,
	MP_ANT_RX,
	MP_CTX,
	MP_QUERY,
	MP_ARX,
	MP_PSD,
	MP_PWRTRK,
	MP_THER,
	MP_IOCTL,
	EFUSE_GET,
	EFUSE_SET,
	MP_RESET_STATS,
	MP_DUMP,
	MP_PHYPARA,
	MP_SetRFPathSwh,
	MP_QueryDrvStats,
	CTA_TEST,
	MP_DISABLE_BT_COEXIST,
	MP_PwrCtlDM,
	MP_GETVER,
	MP_MON,
	EFUSE_BT_MASK,
	EFUSE_MASK,
	EFUSE_FILE,
	EFUSE_FILE_STORE,
	MP_TX,
	MP_RX,
	MP_IQK,
	MP_LCK,
	MP_HW_TX_MODE,
	MP_GET_TXPOWER_INX,
	MP_CUSTOMER_STR,
	MP_PWRLMT,
	MP_PWRBYRATE,
	BT_EFUSE_FILE,
	MP_SWRFPath,
	MP_LINK,
	MP_DPK_TRK,
	MP_DPK,
	MP_GET_TSSIDE,
	MP_SET_TSSIDE,
	MP_GET_PHL_TEST,
	MP_SET_PHL_TEST,
	MP_SET_PHL_TX_PATTERN,
	MP_SET_PHL_PLCP_TX_DATA,
	MP_SET_PHL_PLCP_TX_USER,
	MP_SET_PHL_TX_METHOD,
	MP_SET_PHL_CONIFG_PHY_NUM,
	MP_PHL_RFK,
	MP_PHL_BTC_PATH,
	MP_GET_HE,
	MP_BAND,
	MP_UUID,
	MP_NULL,
#ifdef CONFIG_APPEND_VENDOR_IE_ENABLE
	VENDOR_IE_SET ,
	VENDOR_IE_GET ,
#endif
#if defined(RTW_PHL_TX) || defined(RTW_PHL_RX) || defined(CONFIG_PHL_TEST_SUITE)
	PHL_TEST_SET,
	PHL_TEST_GET,
#endif
#ifdef CONFIG_WOWLAN
	MP_WOW_ENABLE,
	MP_WOW_SET_PATTERN,
#endif
#ifdef CONFIG_AP_WOWLAN
	MP_AP_WOW_ENABLE,
#endif
	MP_SD_IREAD,
	MP_SD_IWRITE,
#ifdef CONFIG_FPGA_INCLUDED
	FPGA_SET,
#endif
};

struct rtw_plcp_user {
	u8 plcp_usr_idx;
	u16 plcp_mcs;
	u8 coding;
	u8 dcm;
	u8 aid;
	u32 plcp_txlen; /*apep*/
	u32 ru_alloc;
	u8 plcp_nss;
	u8 txbf;
	u8 pwr_boost_db;
};

struct mp_priv {
	_adapter *papdater;

	/* Testing Flag */
	u32 mode;/* 0 for normal type packet, 1 for loopback packet (16bytes TXCMD) */

	u32 prev_fw_state;

	/* OID cmd handler */
	struct mp_wiparam workparam;
	/*	u8 act_in_progress; */

	/* Tx Section */
	u8 TID;
	u32 tx_pktcount;
	u32 pktInterval;
	u32 pktLength;
	struct mp_tx tx;

	/* Rx Section */
	u32 rx_bssidpktcount;
	u32 rx_pktcount;
	u32 rx_pktcount_filter_out;
	u32 rx_crcerrpktcount;
	u32 rx_pktloss;
	BOOLEAN  rx_bindicatePkt;
	struct recv_stat rxstat;
	BOOLEAN brx_filter_beacon;

	/* RF/BB relative */
	u8 band;
	u8 channel;
	u8 bandwidth;
	u8 prime_channel_offset;
	u8 txpoweridx;
	s16 txpowerdbm;
	u16 rateidx;
	s16 pre_refcw_cck_pwridxa;
	s16 pre_refcw_cck_pwridxb;
	s16 pre_refcw_ofdm_pwridxa;
	s16 pre_refcw_ofdm_pwridxb;

	u32 preamble;
	/*	u8 modem; */
	u32 CrystalCap;
	/*	u32 curr_crystalcap; */

	u8 antenna_tx;
	u8 antenna_rx;
	u8 antenna_trx;
	/*	u8 curr_rfpath; */

	u8 check_mp_pkt;

	u8 bSetTxPower;
	/*	uint ForcedDataRate; */
	u8 mp_dm;
	u8 mac_filter[ETH_ALEN];
	u8 bmac_filter;

	/* RF PATH Setting for WLG WLA BTG BT */
	u8 rf_path_cfg;
	u8 btc_path; /* BTC_MODE_NORMAL, BTC_MODE_WL,BTC_MODE_BT */

	struct wlan_network mp_network;
	NDIS_802_11_MAC_ADDRESS network_macaddr;

	u8 *pallocated_mp_xmitframe_buf;
	u8 *pmp_xmtframe_buf;
	_queue free_mp_xmitqueue;
	u32 free_mp_xmitframe_cnt;
	BOOLEAN bSetRxBssid;
	BOOLEAN bTxBufCkFail;
	BOOLEAN bRTWSmbCfg;
	BOOLEAN bloopback;
	BOOLEAN bloadefusemap;
	BOOLEAN bloadBTefusemap;
	BOOLEAN bprocess_mp_mode;

	MPT_CONTEXT	mpt_ctx;

	u8		*TXradomBuffer;
	u8		mp_keep_btc_mode;
	u8		mplink_buf[2048];
	u32		mplink_rx_len;
	BOOLEAN mplink_brx;
	BOOLEAN mplink_btx;

	bool tssitrk_on;
	u8 tssi_mode;
	u8 rtw_mp_cur_phy;
	u8 rtw_mp_dbcc;
	s16 path_pwr_offset[4];	/* rf-A, rf-B*/
	u8 rtw_mp_tx_method;
	u16 rtw_mp_tx_time;
	u8 rtw_mp_tx_state;
	u8 rtw_mp_pmact_patt_idx;
	u8 rtw_mp_pmact_ppdu_type;
	u8 rtw_mp_data_bandwidth;
	u8 rtw_mp_stbc;
	u8 rtw_mp_plcp_gi;
	u8 rtw_mp_plcp_ltf;
	u8 rtw_mp_he_sigb;
	u8 rtw_mp_he_sigb_dcm;
	u32 rtw_mp_plcp_tx_time;
	u8 rtw_mp_plcp_tx_mode;

	u8 rtw_mp_he_er_su_ru_106_en;
	u8 rtw_mp_trxsc;
	u16 rtw_mp_plcp_rualloc;
	u8 rtw_mp_plcp_tx_user;
	u32 rtw_mp_ru_tone;
	u8 ru_tone_sel_list[6];
	u8 ru_alloc_list[68];

	struct rtw_mp_giltf_data st_giltf[5];
	struct rtw_plcp_user mp_plcp_user[4];
	u8 mp_plcp_useridx;

	u8 keep_ips_status;
	u8 keep_lps_status;
};

#define PPDU_TYPE_STR(idx)\
	(idx == RTW_MP_TYPE_CCK) ? "CCK" :\
	(idx == RTW_MP_TYPE_LEGACY) ? "LEGACY" :\
	(idx == RTW_MP_TYPE_HT_MF) ? "HT_MF" :\
	(idx == RTW_MP_TYPE_HT_GF) ? "HT_GF" :\
	(idx == RTW_MP_TYPE_VHT) ? "VHT" :\
	(idx == RTW_MP_TYPE_HE_SU) ? "HE_SU" :\
	(idx == RTW_MP_TYPE_HE_ER_SU) ? "HE_ER_SU" :\
	(idx == RTW_MP_TYPE_HE_MU_OFDMA) ? "HE_MU" :\
	(idx == RTW_MP_TYPE_HE_TB) ? "HE_TB" :\
	"UNknow"


typedef struct _IOCMD_STRUCT_ {
	u8	cmdclass;
	u16	value;
	u8	index;
} IOCMD_STRUCT;

struct rf_reg_param {
	u32 path;
	u32 offset;
	u32 value;
};

struct bb_reg_param {
	u32 offset;
	u32 value;
};

/* *********************************************************************** */

#define LOWER	_TRUE
#define RAISE	_FALSE

/* Hardware Registers */
#if 0
#if 0
#define IOCMD_CTRL_REG			0x102502C0
#define IOCMD_DATA_REG			0x102502C4
#else
#define IOCMD_CTRL_REG			0x10250370
#define IOCMD_DATA_REG			0x10250374
#endif

#define IOCMD_GET_THERMAL_METER		0xFD000028

#define IOCMD_CLASS_BB_RF		0xF0
#define IOCMD_BB_READ_IDX		0x00
#define IOCMD_BB_WRITE_IDX		0x01
#define IOCMD_RF_READ_IDX		0x02
#define IOCMD_RF_WRIT_IDX		0x03
#endif
#define BB_REG_BASE_ADDR		0x800

/* MP variables */
#if 0
#define _2MAC_MODE_	0
#define _LOOPBOOK_MODE_	1
#endif

typedef enum _MP_MODE_ {
	MP_OFF,
	MP_ON,
	MP_ERR,
	MP_CONTINUOUS_TX,
	MP_SINGLE_CARRIER_TX,
	MP_CARRIER_SUPPRISSION_TX,
	MP_SINGLE_TONE_TX,
	MP_PACKET_TX,
	MP_PACKET_RX
} MP_MODE;

typedef enum _TEST_MODE {
	TEST_NONE                 ,
	PACKETS_TX                ,
	PACKETS_RX                ,
	CONTINUOUS_TX             ,
	OFDM_Single_Tone_TX       ,
	CCK_Carrier_Suppression_TX
} TEST_MODE;

typedef enum _MPT_BANDWIDTH {
	MPT_BW_20MHZ = 0,
	MPT_BW_40MHZ_DUPLICATE = 1,
	MPT_BW_40MHZ_ABOVE = 2,
	MPT_BW_40MHZ_BELOW = 3,
	MPT_BW_40MHZ = 4,
	MPT_BW_80MHZ = 5,
	MPT_BW_80MHZ_20_ABOVE = 6,
	MPT_BW_80MHZ_20_BELOW = 7,
	MPT_BW_80MHZ_20_BOTTOM = 8,
	MPT_BW_80MHZ_20_TOP = 9,
	MPT_BW_80MHZ_40_ABOVE = 10,
	MPT_BW_80MHZ_40_BELOW = 11,
} MPT_BANDWIDTHE, *PMPT_BANDWIDTH;

#define MAX_RF_PATH_NUMS	RF_PATH_MAX


extern u8 mpdatarate[NumRates];

/* MP set force data rate base on the definition. */
typedef enum _MPT_RATE_INDEX {
	/* CCK rate. */
	MPT_RATE_1M = 1 ,	/* 0 */
	MPT_RATE_2M,
	MPT_RATE_55M,
	MPT_RATE_11M,	/* 3 */

	/* OFDM rate. */
	MPT_RATE_6M,	/* 4 */
	MPT_RATE_9M,
	MPT_RATE_12M,
	MPT_RATE_18M,
	MPT_RATE_24M,
	MPT_RATE_36M,
	MPT_RATE_48M,
	MPT_RATE_54M,	/* 11 */

	/* HT rate. */
	MPT_RATE_MCS0,	/* 12 */
	MPT_RATE_MCS1,
	MPT_RATE_MCS2,
	MPT_RATE_MCS3,
	MPT_RATE_MCS4,
	MPT_RATE_MCS5,
	MPT_RATE_MCS6,
	MPT_RATE_MCS7,	/* 19 */
	MPT_RATE_MCS8,
	MPT_RATE_MCS9,
	MPT_RATE_MCS10,
	MPT_RATE_MCS11,
	MPT_RATE_MCS12,
	MPT_RATE_MCS13,
	MPT_RATE_MCS14,
	MPT_RATE_MCS15,	/* 27 */
	MPT_RATE_MCS16,
	MPT_RATE_MCS17, /*  #29 */
	MPT_RATE_MCS18,
	MPT_RATE_MCS19,
	MPT_RATE_MCS20,
	MPT_RATE_MCS21,
	MPT_RATE_MCS22, /*  #34 */
	MPT_RATE_MCS23,
	MPT_RATE_MCS24,
	MPT_RATE_MCS25,
	MPT_RATE_MCS26,
	MPT_RATE_MCS27, /*  #39 */
	MPT_RATE_MCS28, /*  #40 */
	MPT_RATE_MCS29, /*  #41 */
	MPT_RATE_MCS30, /*  #42 */
	MPT_RATE_MCS31, /*  #43 */
	/* VHT rate. Total: 20*/
	MPT_RATE_VHT1SS_MCS0 = 100,/*  #44*/
	MPT_RATE_VHT1SS_MCS1, /*  # */
	MPT_RATE_VHT1SS_MCS2,
	MPT_RATE_VHT1SS_MCS3,
	MPT_RATE_VHT1SS_MCS4,
	MPT_RATE_VHT1SS_MCS5,
	MPT_RATE_VHT1SS_MCS6, /*  # */
	MPT_RATE_VHT1SS_MCS7,
	MPT_RATE_VHT1SS_MCS8,
	MPT_RATE_VHT1SS_MCS9, /* #53 */
	MPT_RATE_VHT2SS_MCS0, /* #54 */
	MPT_RATE_VHT2SS_MCS1,
	MPT_RATE_VHT2SS_MCS2,
	MPT_RATE_VHT2SS_MCS3,
	MPT_RATE_VHT2SS_MCS4,
	MPT_RATE_VHT2SS_MCS5,
	MPT_RATE_VHT2SS_MCS6,
	MPT_RATE_VHT2SS_MCS7,
	MPT_RATE_VHT2SS_MCS8,
	MPT_RATE_VHT2SS_MCS9, /* #63 */
	MPT_RATE_VHT3SS_MCS0,
	MPT_RATE_VHT3SS_MCS1,
	MPT_RATE_VHT3SS_MCS2,
	MPT_RATE_VHT3SS_MCS3,
	MPT_RATE_VHT3SS_MCS4,
	MPT_RATE_VHT3SS_MCS5,
	MPT_RATE_VHT3SS_MCS6, /*  #126 */
	MPT_RATE_VHT3SS_MCS7,
	MPT_RATE_VHT3SS_MCS8,
	MPT_RATE_VHT3SS_MCS9,
	MPT_RATE_VHT4SS_MCS0,
	MPT_RATE_VHT4SS_MCS1, /*  #131 */
	MPT_RATE_VHT4SS_MCS2,
	MPT_RATE_VHT4SS_MCS3,
	MPT_RATE_VHT4SS_MCS4,
	MPT_RATE_VHT4SS_MCS5,
	MPT_RATE_VHT4SS_MCS6, /*  #136 */
	MPT_RATE_VHT4SS_MCS7,
	MPT_RATE_VHT4SS_MCS8,
	MPT_RATE_VHT4SS_MCS9,
	MPT_RATE_LAST
} MPT_RATE_E, *PMPT_RATE_E;

#define MAX_TX_PWR_INDEX_N_MODE 64	/* 0x3F */

#define MPT_IS_CCK_RATE(_value)		(MPT_RATE_1M <= _value && _value <= MPT_RATE_11M)
#define MPT_IS_OFDM_RATE(_value)	(MPT_RATE_6M <= _value && _value <= MPT_RATE_54M)
#define MPT_IS_HT_RATE(_value)		(MPT_RATE_MCS0 <= _value && _value <= MPT_RATE_MCS31)
#define MPT_IS_HT_1S_RATE(_value)	(MPT_RATE_MCS0 <= _value && _value <= MPT_RATE_MCS7)
#define MPT_IS_HT_2S_RATE(_value)	(MPT_RATE_MCS8 <= _value && _value <= MPT_RATE_MCS15)
#define MPT_IS_HT_3S_RATE(_value)	(MPT_RATE_MCS16 <= _value && _value <= MPT_RATE_MCS23)
#define MPT_IS_HT_4S_RATE(_value)	(MPT_RATE_MCS24 <= _value && _value <= MPT_RATE_MCS31)

#define MPT_IS_VHT_RATE(_value)		(MPT_RATE_VHT1SS_MCS0 <= _value && _value <= MPT_RATE_VHT4SS_MCS9)
#define MPT_IS_VHT_1S_RATE(_value)	(MPT_RATE_VHT1SS_MCS0 <= _value && _value <= MPT_RATE_VHT1SS_MCS9)
#define MPT_IS_VHT_2S_RATE(_value)	(MPT_RATE_VHT2SS_MCS0 <= _value && _value <= MPT_RATE_VHT2SS_MCS9)
#define MPT_IS_VHT_3S_RATE(_value)	(MPT_RATE_VHT3SS_MCS0 <= _value && _value <= MPT_RATE_VHT3SS_MCS9)
#define MPT_IS_VHT_4S_RATE(_value)	(MPT_RATE_VHT4SS_MCS0 <= _value && _value <= MPT_RATE_VHT4SS_MCS9)

#define MPT_IS_2SS_RATE(_rate) ((MPT_RATE_MCS8 <= _rate && _rate <= MPT_RATE_MCS15) || \
	(MPT_RATE_VHT2SS_MCS0 <= _rate && _rate <= MPT_RATE_VHT2SS_MCS9))
#define MPT_IS_3SS_RATE(_rate) ((MPT_RATE_MCS16 <= _rate && _rate <= MPT_RATE_MCS23) || \
	(MPT_RATE_VHT3SS_MCS0 <= _rate && _rate <= MPT_RATE_VHT3SS_MCS9))
#define MPT_IS_4SS_RATE(_rate) ((MPT_RATE_MCS24 <= _rate && _rate <= MPT_RATE_MCS31) || \
	(MPT_RATE_VHT4SS_MCS0 <= _rate && _rate <= MPT_RATE_VHT4SS_MCS9))

typedef enum _POWER_MODE_ {
	POWER_LOW = 0,
	POWER_NORMAL
} POWER_MODE;

/* The following enumeration is used to define the value of Reg0xD00[30:28] or JaguarReg0x914[18:16]. */
typedef enum _OFDM_TX_MODE {
	OFDM_ALL_OFF		= 0,
	OFDM_ContinuousTx	= 1,
	OFDM_SingleCarrier	= 2,
	OFDM_SingleTone	= 4,
} OFDM_TX_MODE;


#define RX_PKT_BROADCAST	1
#define RX_PKT_DEST_ADDR	2
#define RX_PKT_PHY_MATCH	3

typedef enum _ENCRY_CTRL_STATE_ {
	HW_CONTROL,		/* hw encryption& decryption */
	SW_CONTROL,		/* sw encryption& decryption */
	HW_ENCRY_SW_DECRY,	/* hw encryption & sw decryption */
	SW_ENCRY_HW_DECRY	/* sw encryption & hw decryption */
} ENCRY_CTRL_STATE;

typedef enum	_MPT_TXPWR_DEF {
	MPT_CCK,
	MPT_OFDM, /* L and HT OFDM */
	MPT_OFDM_AND_HT,
	MPT_HT,
	MPT_VHT
} MPT_TXPWR_DEF;


#define IS_MPT_HT_RATE(_rate)			(_rate >= MPT_RATE_MCS0 && _rate <= MPT_RATE_MCS31)
#define IS_MPT_VHT_RATE(_rate)			(_rate >= MPT_RATE_VHT1SS_MCS0 && _rate <= MPT_RATE_VHT4SS_MCS9)
#define IS_MPT_CCK_RATE(_rate)			(_rate >= MPT_RATE_1M && _rate <= MPT_RATE_11M)
#define IS_MPT_OFDM_RATE(_rate)			(_rate >= MPT_RATE_6M && _rate <= MPT_RATE_54M)

typedef enum _mp_tx_pkt_payload{
	MP_TX_Payload_00 = 0,
	MP_TX_Payload_a5,
	MP_TX_Payload_5a,
	MP_TX_Payload_ff,
	MP_TX_Payload_prbs9,
	MP_TX_Payload_default_random
} mp_tx_pkt_payload;

/*************************************************************************/
#if 0
extern struct mp_xmit_frame *alloc_mp_xmitframe(struct mp_priv *pmp_priv);
extern int free_mp_xmitframe(struct xmit_priv *pxmitpriv, struct mp_xmit_frame *pmp_xmitframe);
#endif
enum rtw_mp_tx_method {
	RTW_MP_SW_TX = 0,
	RTW_MP_PMACT_TX,
	RTW_MP_TMACT_TX,
	RTW_MP_FW_PMACT_TX,
};

enum rtw_mp_tx_cmd {
	RTW_MP_TX_NONE = 0,
	RTW_MP_TX_PACKETS,
	RTW_MP_TX_CONTINUOUS,
	RTW_MP_TX_SINGLE_TONE,
	RTW_MP_TX_CCK_Carrier_Suppression,
	RTW_MP_TX_CONFIG_PLCP_COMMON_INFO,
	RTW_MP_TX_CMD_PHY_OK,
	RTW_MP_TX_CONFIG_PLCP_PATTERN,
	RTW_MP_TX_CONFIG_PLCP_USER_INFO,
	RTW_MP_TX_MODE_SWITCH,
	RTW_MP_TX_F2P,
	RTW_MP_TX_TB_TEST,
	RTW_MP_TX_DPD_BYPASS,
	RTW_MP_TX_CHECK_TX_IDLE,
	RTW_MP_TX_CMD_MAX,
};

enum rtw_mp_pmac_mode {
	RTW_MP_PMAC_NONE_TEST,
	RTW_MP_PMAC_PKTS_TX,
	RTW_MP_PMAC_PKTS_RX,
	RTW_MP_PMAC_CONT_TX,
	RTW_MP_PMAC_FW_TRIG_TX,
	RTW_MP_PMAC_OFDM_SINGLE_TONE_TX,
	RTW_MP_PMAC_CCK_CARRIER_SIPPRESSION_TX
};

enum rtw_mp_ppdu_type {
	RTW_MP_TYPE_CCK			= 0,
	RTW_MP_TYPE_LEGACY,
	RTW_MP_TYPE_HT_MF,
	RTW_MP_TYPE_HT_GF,
	RTW_MP_TYPE_VHT,
	RTW_MP_TYPE_HE_SU,
	RTW_MP_TYPE_HE_ER_SU,
	RTW_MP_TYPE_HE_MU_OFDMA,
	RTW_MP_TYPE_HE_TB
};

/* mp command class */
enum rtw_mp_class {
	RTW_MP_CLASS_CONFIG = 0,
	RTW_MP_CLASS_TX = 1,
	RTW_MP_CLASS_RX = 2,
	RTW_MP_CLASS_EFUSE = 3,
	RTW_MP_CLASS_REG = 4,
	RTW_MP_CLASS_TXPWR = 5,
	RTW_MP_CLASS_CAL = 6,
	RTW_MP_CLASS_FLASH = 7,
	RTW_MP_CLASS_MAX,
};

/* mp rx command */
enum rtw_mp_rx_cmd {
	RTW_MP_RX_CMD_PHY_CRC_OK = 0,
	RTW_MP_RX_CMD_PHY_CRC_ERR = 1,
	RTW_MP_RX_CMD_MAC_CRC_OK = 2,
	RTW_MP_RX_CMD_MAC_CRC_ERR = 3,
	RTW_MP_RX_CMD_DRV_CRC_OK = 4,
	RTW_MP_RX_CMD_DRV_CRC_ERR = 5,
	RTW_MP_RX_CMD_GET_RSSI = 6,
	RTW_MP_RX_CMD_GET_RXEVM = 7,
	RTW_MP_RX_CMD_GET_PHYSTS = 8,
	RTW_MP_RX_CMD_TRIGGER_RXEVM = 9,
	RTW_MP_RX_CMD_SET_GAIN_OFFSET = 10,
	RTW_MP_RX_CMD_MAX,

};

/* mp config command */
enum rtw_mp_config_cmdid {
	RTW_MP_CONFIG_CMD_GET_BW,
	RTW_MP_CONFIG_CMD_GET_RF_STATUS,
	RTW_MP_CONFIG_CMD_SET_RATE_IDX,
	RTW_MP_CONFIG_CMD_SET_RF_TXRX_PATH,
	RTW_MP_CONFIG_CMD_SET_RESET_PHY_COUNT,
	RTW_MP_CONFIG_CMD_SET_RESET_MAC_COUNT,
	RTW_MP_CONFIG_CMD_SET_RESET_DRV_COUNT,
	RTW_MP_CONFIG_CMD_SET_TXRX_MODE,
	RTW_MP_CONFIG_CMD_PBC,
	RTW_MP_CONFIG_CMD_START_DUT,
	RTW_MP_CONFIG_CMD_STOP_DUT,
	RTW_MP_CONFIG_CMD_GET_MIMPO_RSSI,
	RTW_MP_CONFIG_CMD_GET_BOARD_TYPE,
	RTW_MP_CONFIG_CMD_GET_MODULATION,
	RTW_MP_CONFIG_CMD_GET_RF_MODE,
	RTW_MP_CONFIG_CMD_GET_RF_PATH,
	RTW_MP_CONFIG_CMD_SET_MODULATION,
	RTW_MP_CONFIG_CMD_GET_DEVICE_INFO,
	RTW_MP_CONFIG_CMD_SET_PHY_INDEX,
	RTW_MP_CONFIG_CMD_GET_MAC_ADDR,
	RTW_MP_CONFIG_CMD_SET_MAC_ADDR,
	RTW_MP_CONFIG_CMD_SET_CH_BW,
	RTW_MP_CONFIG_CMD_GET_TX_NSS,
	RTW_MP_CONFIG_CMD_GET_RX_NSS,
	RTW_MP_CONFIG_CMD_SWITCH_BT_PATH,
	RTW_MP_CONFIG_CMD_GET_RFE_TYPE,
	RTW_MP_CONFIG_CMD_GET_DEV_IDX,
	RTW_MP_CONFIG_CMD_TRIGGER_FW_CONFLICT,
	RTW_MP_CONFIG_CMD_GET_UUID,
	RTW_MP_CONFIG_CMD_SET_REGULATION,
	RTW_MP_CONFIG_CMD_MAX,
};

typedef enum _mp_ant_path {
	MP_ANTENNA_NONE	= 0,
	MP_ANTENNA_D	= 1,
	MP_ANTENNA_C	= 2,
	MP_ANTENNA_CD	= 3,
	MP_ANTENNA_B	= 4,
	MP_ANTENNA_BD	= 5,
	MP_ANTENNA_BC	= 6,
	MP_ANTENNA_BCD	= 7,
	MP_ANTENNA_A	= 8,
	MP_ANTENNA_AD	= 9,
	MP_ANTENNA_AC	= 10,
	MP_ANTENNA_ACD	= 11,
	MP_ANTENNA_AB	= 12,
	MP_ANTENNA_ABD	= 13,
	MP_ANTENNA_ABC	= 14,
	MP_ANTENNA_ABCD	= 15
} mp_ant_path;

#define RTW_MP_TEST_NAME_LEN		32
#define RTW_MP_TEST_RPT_RSN_LEN	32

struct rtw_mp_test_rpt {
	char name[RTW_MP_TEST_NAME_LEN];
	u8 status;
	char rsn[RTW_MP_TEST_RPT_RSN_LEN];
	u32 total_time; // in ms
};

struct rtw_mp_cmd_arg {
	u8 mp_class;
	u8 cmd;
	u8 cmd_ok;
};

struct rtw_mp_config_arg {
	u8 mp_class;
	u8 cmd;
	u8 cmd_ok;
	u8 status;
	u8 channel;
	u8 bandwidth;
	u8 rate_idx;
	u8 ant_tx;
	u8 ant_rx;
	u8 rf_path;
	u8 get_rfstats;
	u8 modulation;
	u8 bustype;
	u32 chipid;
	u8 cur_phy;
	u8 mac_addr[6];
	u8 sc_idx;
	u8 dbcc_en;
	u8 btc_mode;
	u8 rfe_type;
	u8 dev_id;
	u32 offset;
	u8 voltag;
	u8 band;
	u32 uuid;
	u8 regulation;
};

struct rtw_mp_tx_arg {
	u8 mp_class;
	u8 cmd;
	u8 cmd_ok;
	u8 status;
	u8 tx_method;
	u8 plcp_ppdu_type;	/*offline gen*/
	u16 plcp_case_id;	/*offline gen*/
	u8 bCarrierSuppression;
	u8 is_cck;
	u8 start_tx;
	u16 tx_cnt;
	u16 period;		/* us */
	u16 tx_time;	/* us */
	u32 tx_ok;
	u8 tx_path;
	u8 tx_mode;		/* mode: 0 = tmac, 1 = pmac */
	u8 tx_concurrent_en;	/* concurrent tx */
	u8 dpd_bypass;
	/* plcp info */
	u32 dbw; /*0:BW20, 1:BW40, 2:BW80, 3:BW160/BW80+80*/
	u32 source_gen_mode;
	u32 locked_clk;
	u32 dyn_bw;
	u32 ndp_en;
	u32 long_preamble_en; /*bmode*/
	u32 stbc;
	u32 gi; /*0:0.4,1:0.8,2:1.6,3:3.2*/
	u32 tb_l_len;
	u32 tb_ru_tot_sts_max;
	u32 vht_txop_not_allowed;
	u32 tb_disam;
	u32 doppler;
	u32 he_ltf_type; /*0:1x,1:2x,2:4x*/
	u32 ht_l_len;
	u32 preamble_puncture;
	u32 he_mcs_sigb;/*0~5*/
	u32 he_dcm_sigb;
	u32 he_sigb_compress_en;
	u32 max_tx_time_0p4us;
	u32 ul_flag;
	u32 tb_ldpc_extra;
	u32 bss_color;
	u32 sr;
	u32 beamchange_en;
	u32 he_er_u106ru_en;
	u32 ul_srp1;
	u32 ul_srp2;
	u32 ul_srp3;
	u32 ul_srp4;
	u32 mode;
	u32 group_id;
	u32 ppdu_type;/*0: bmode,1:Legacy,2:HT_MF,3:HT_GF,4:VHT,5:HE_SU,6:HE_ER_SU,7:HE_MU,8:HE_TB*/
	u32 txop;
	u32 tb_strt_sts;
	u32 tb_pre_fec_padding_factor;
	u32 cbw;
	u32 txsc;
	u32 tb_mumimo_mode_en;
	u32 nominal_t_pe; /* def = 2*/
	u32 ness; /* def = 0*/
	u32 n_user;
	u32 tb_rsvd;/*def = 0*/
	/* plcp user info */
	u32 plcp_usr_idx;
	u32 mcs;
	u32 mpdu_len;
	u32 n_mpdu;
	u32 fec;
	u32 dcm;
	u32 aid;
	u32 scrambler_seed; /* rand (1~255)*/
	u32 random_init_seed; /* rand (1~255)*/
	u32 apep;
	u32 ru_alloc;
	u32 nss;
	u32 txbf;
	u32 pwr_boost_db;
	//struct mp_plcp_param_t plcp_param;	/*online gen*/
	u32 data_rate;
	u8 plcp_sts;

	/*HE-TB Test*/
	u8 bSS_id_addr0;
	u8 bSS_id_addr1;
	u8 bSS_id_addr2;
	u8 bSS_id_addr3;
	u8 bSS_id_addr4;
	u8 bSS_id_addr5;
	u8 is_link_mode;

	/* f2p cmd */
	u32 pref_AC_0;
	u32 aid12_0;
	u32 ul_mcs_0;
	u32 macid_0;
	u32 ru_pos_0;
	u32 ul_fec_code_0;
	u32 ul_dcm_0;
	u32 ss_alloc_0;
	u32 ul_tgt_rssi_0;
	u32 pref_AC_1;
	u32 aid12_1;
	u32 ul_mcs_1;
	u32 macid_1;
	u32 ru_pos_1;
	u32 ul_fec_code_1;
	u32 ul_dcm_1;
	u32 ss_alloc_1;
	u32 ul_tgt_rssi_1;
	u32 pref_AC_2;
	u32 aid12_2;
	u32 ul_mcs_2;
	u32 macid_2;
	u32 ru_pos_2;
	u32 ul_fec_code_2;
	u32 ul_dcm_2;
	u32 ss_alloc_2;
	u32 ul_tgt_rssi_2;
	u32 pref_AC_3;
	u32 aid12_3;
	u32 ul_mcs_3;
	u32 macid_3;
	u32 ru_pos_3;
	u32 ul_fec_code_3;
	u32 ul_dcm_3;
	u32 ss_alloc_3;
	u32 ul_tgt_rssi_3;
	u32 ul_bw;
	u32 gi_ltf;
	u32 num_he_ltf;
	u32 ul_stbc;
	u32 pkt_doppler;
	u32 ap_tx_power;
	u32 user_num;
	u32 pktnum;
	u32 pri20_bitmap;
	u32 datarate;
	u32 mulport_id;
	u32 pwr_ofset;
	u32 f2p_mode;
	u32 frexch_type;
	u32 sigb_len;
	/* dword 0 */
	u32 cmd_qsel;
	u32 ls;
	u32 fs;
	u32 total_number;
	u32 seq;
	u32 length;
	/* dword 1 */
	/* dword 0 */
	u32 cmd_type;
	u32 cmd_sub_type;
	u32 dl_user_num;
	u32 bw;
	u32 tx_power;
	/* dword 1 */
	u32 fw_define;
	u32 ss_sel_mode;
	u32 next_qsel;
	u32 twt_group;
	u32 dis_chk_slp;
	u32 ru_mu_2_su;
	u32 dl_t_pe;
	/* dword 2 */
	u32 sigb_ch1_len;
	u32 sigb_ch2_len;
	u32 sigb_sym_num;
	u32 sigb_ch2_ofs;
	u32 dis_htp_ack;
	u32 tx_time_ref;
	u32 pri_user_idx;
	/* dword 3 */
	u32 ampdu_max_txtime;
	u32 d3_group_id;
	u32 twt_chk_en;
	u32 twt_port_id;
	/* dword 4 */
	u32 twt_start_time;
	/* dword 5 */
	u32 twt_end_time;
	/* dword 6 */
	u32 apep_len;
	u32 tri_pad;
	u32 ul_t_pe;
	u32 rf_gain_idx;
	u32 fixed_gain_en;
	u32 ul_gi_ltf;
	u32 ul_doppler;
	u32 d6_ul_stbc;
	/* dword 7 */
	u32 ul_mid_per;
	u32 ul_cqi_rrp_tri;
	u32 sigb_dcm;
	u32 sigb_comp;
	u32 d7_doppler;
	u32 d7_stbc;
	u32 mid_per;
	u32 gi_ltf_size;
	u32 sigb_mcs;
	/* dword 8 */
	u32 macid_u0;
	u32 ac_type_u0;
	u32 mu_sta_pos_u0;
	u32 dl_rate_idx_u0;
	u32 dl_dcm_en_u0;
	u32 ru_alo_idx_u0;
	/* dword 9 */
	u32 pwr_boost_u0;
	u32 agg_bmp_alo_u0;
	u32 ampdu_max_txnum_u0;
	u32 user_define_u0;
	u32 user_define_ext_u0;
	/* dword 10 */
	u32 ul_addr_idx_u0;
	u32 ul_dcm_u0;
	u32 ul_fec_cod_u0;
	u32 ul_ru_rate_u0;
	u32 ul_ru_alo_idx_u0;
	/* dword 11 */
	/* dword 12 */
	u32 macid_u1;
	u32 ac_type_u1;
	u32 mu_sta_pos_u1;
	u32 dl_rate_idx_u1;
	u32 dl_dcm_en_u1;
	u32 ru_alo_idx_u1;
	/* dword 13 */
	u32 pwr_boost_u1;
	u32 agg_bmp_alo_u1;
	u32 ampdu_max_txnum_u1;
	u32 user_define_u1;
	u32 user_define_ext_u1;
	/* dword 14 */
	u32 ul_addr_idx_u1;
	u32 ul_dcm_u1;
	u32 ul_fec_cod_u1;
	u32 ul_ru_rate_u1;
	u32 ul_ru_alo_idx_u1;
	/* dword 15 */
	/* dword 16 */
	u32 macid_u2;
	u32 ac_type_u2;
	u32 mu_sta_pos_u2;
	u32 dl_rate_idx_u2;
	u32 dl_dcm_en_u2;
	u32 ru_alo_idx_u2;
	/* dword 17 */
	u32 pwr_boost_u2;
	u32 agg_bmp_alo_u2;
	u32 ampdu_max_txnum_u2;
	u32 user_define_u2;
	u32 user_define_ext_u2;
	/* dword 18 */
	u32 ul_addr_idx_u2;
	u32 ul_dcm_u2;
	u32 ul_fec_cod_u2;
	u32 ul_ru_rate_u2;
	u32 ul_ru_alo_idx_u2;
	/* dword 19 */
	/* dword 20 */
	u32 macid_u3;
	u32 ac_type_u3;
	u32 mu_sta_pos_u3;
	u32 dl_rate_idx_u3;
	u32 dl_dcm_en_u3;
	u32 ru_alo_idx_u3;
	/* dword 21 */
	u32 pwr_boost_u3;
	u32 agg_bmp_alo_u3;
	u32 ampdu_max_txnum_u3;
	u32 user_define_u3;
	u32 user_define_ext_u3;
	/* dword 22 */
	u32 ul_addr_idx_u3;
	u32 ul_dcm_u3;
	u32 ul_fec_cod_u3;
	u32 ul_ru_rate_u3;
	u32 ul_ru_alo_idx_u3;
	/* dword 23 */
	/* dword 24 */
	u32 pkt_id_0;
	u32 valid_0;
	u32 ul_user_num_0;
	/* dword 25 */
	u32 pkt_id_1;
	u32 valid_1;
	u32 ul_user_num_1;
	/* dword 26 */
	u32 pkt_id_2;
	u32 valid_2;
	u32 ul_user_num_2;
	/* dword 27 */
	u32 pkt_id_3;
	u32 valid_3;
	u32 ul_user_num_3;
	/* dword 28 */
	u32 pkt_id_4;
	u32 valid_4;
	u32 ul_user_num_4;
	/* dword 29 */
	u32 pkt_id_5;
	u32 valid_5;
	u32 ul_user_num_5;
	/* tx state*/
	u8 tx_state;
};


struct rtw_mp_rx_arg {
	u8 mp_class;
	u8 cmd;
	u8 cmd_ok;
	u8 status;
	u32 rx_ok;
	u32 rx_err;
	u8 rssi;
	u8 rx_path;
	u8 rx_evm;
	u8 user;
	u8 strm;
	u8 rxevm_table;
	u8 enable;
	u32 phy0_user0_rxevm;
	u32 phy0_user1_rxevm;
	u32 phy0_user2_rxevm;
	u32 phy0_user3_rxevm;
	u32 phy1_user0_rxevm;
	u32 phy1_user1_rxevm;
	u32 phy1_user2_rxevm;
	u32 phy1_user3_rxevm;
	s8 offset;
	u8 rf_path;
	u8 iscck;
	s16 rssi_ex;
};

/* mp tx power command */
enum rtw_mp_txpwr_cmd {
	RTW_MP_TXPWR_CMD_READ_PWR_TABLE = 0,
	RTW_MP_TXPWR_CMD_GET_PWR_TRACK_STATUS = 1,
	RTW_MP_TXPWR_CMD_SET_PWR_TRACK_STATUS = 2,
	RTW_MP_TXPWR_CMD_SET_TXPWR = 3,
	RTW_MP_TXPWR_CMD_GET_TXPWR = 4,
	RTW_MP_TXPWR_CMD_GET_TXPWR_INDEX = 5,
	RTW_MP_TXPWR_CMD_GET_THERMAL = 6,
	RTW_MP_TXPWR_CMD_GET_TSSI = 7,
	RTW_MP_TXPWR_CMD_SET_TSSI = 8,
	RTW_MP_TXPWR_CMD_GET_TXPWR_REF = 9,
	RTW_MP_TXPWR_CMD_GET_TXPWR_REF_CW = 10,
	RTW_MP_TXPWR_CMD_SET_TXPWR_INDEX = 11,
	RTW_MP_TXPWR_CMD_GET_TXINFOPWR = 12,
	RTW_MP_TXPWR_CMD_SET_RFMODE = 13,
	RTW_MP_TXPWR_CMD_SET_TSSI_OFFSET = 14,
	RTW_MP_TXPWR_CMD_GET_ONLINE_TSSI_DE = 15,
	RTW_MP_TXPWR_CMD_SET_PWR_LMT_EN = 16,
	RTW_MP_TXPWR_CMD_GET_PWR_LMT_EN = 17,
	RTW_MP_TXPWR_CMD_MAX,
};

enum rtw_mp_tssi_pwrtrk_type{
	RTW_MP_TSSI_OFF = 0,
	RTW_MP_TSSI_ON,
	RTW_MP_TSSI_CAL
};

struct rtw_mp_txpwr_arg {
	u8 mp_class;
	u8 cmd;
	u8 cmd_ok;
	u8 status;
	s16 txpwr;
	u16 txpwr_index;
	u8 txpwr_track_status;
	u8 txpwr_status;
	u32 tssi;
	u8 thermal;
	u8 rfpath;
	u8 ofdm;
	u8 tx_path;
	u16 rate;
	u8 bandwidth;
	u8 channel;
	s16 table_item; /*get an element of power table*/
	u8 dcm;
	u8 beamforming;
	u8 offset;
	s16 txpwr_ref;
	u8 is_cck;
	u8 rf_mode;
	u32 tssi_de_offset;
	s32 dbm;
	s32 pout;
	s32 online_tssi_de;
	bool pwr_lmt_en;
	u8 sharp_id;
};

/* mp reg command */
enum rtw_mp_reg_cmd {
	RTW_MP_REG_CMD_READ_MAC = 0,
	RTW_MP_REG_CMD_WRITE_MAC = 1,
	RTW_MP_REG_CMD_READ_RF = 2,
	RTW_MP_REG_CMD_WRITE_RF = 3,
	RTW_MP_REG_CMD_READ_SYN = 4,
	RTW_MP_REG_CMD_WRITE_SYN = 5,
	RTW_MP_REG_CMD_READ_BB = 6,
	RTW_MP_REG_CMD_WRITE_BB = 7,
	RTW_MP_REG_CMD_SET_XCAP = 8,
	RTW_MP_REG_CMD_GET_XCAP = 9,
	RTW_MP_REG_CMD_MAX,
};

struct rtw_mp_reg_arg {
	u8 mp_class;
	u8 cmd;
	u8 cmd_ok;
	u8 status;
	u32 io_offset;
	u32 io_value;
	u8 io_type;
	u8 ofdm;
	u8 rfpath;
	u8 sc_xo;
	u8 xsi_offset;
	u8 xsi_value;
};

struct rtw_mp_cal_arg {
	u8 mp_class;
	u8 cmd;
	u8 cmd_ok;
	u8 status;
	u8 cal_type;
	u8 enable;
	u8 rfpath;
	u16 io_value;
	u8 channel;
	u8 bandwidth;
	s32 xdbm;
	u8 path;
	u8 iq_path;
	u32 avg;
	u32 fft;
	s32 point;
	u32 upoint;
	u32 start_point;
	u32 stop_point;
	u32 buf;
	u32 outbuf[450];
};

enum rtw_mp_cal_cmd {
	RTW_MP_CAL_CMD_TRIGGER_CAL = 0,
	RTW_MP_CAL_CMD_SET_CAPABILITY_CAL = 1,
	RTW_MP_CAL_CMD_GET_CAPABILITY_CAL = 2,
	RTW_MP_CAL_CMD_GET_TSSI_DE_VALUE = 3,
	RTW_MP_CAL_CMD_SET_TSSI_DE_TX_VERIFY = 4,
	RTW_MP_CAL_CMD_GET_TXPWR_FINAL_ABS = 5,
	RTW_MP_CAL_CMD_TRIGGER_DPK_TRACKING = 6,
	RTW_MP_CAL_CMD_SET_TSSI_AVG = 7,
	RTW_MP_CAL_CMD_PSD_INIT = 8,
	RTW_MP_CAL_CMD_PSD_RESTORE = 9,
	RTW_MP_CAL_CMD_PSD_GET_POINT_DATA = 10,
	RTW_MP_CAL_CMD_PSD_QUERY = 11,
	RTW_MP_CAL_CMD_MAX,
};

enum rtw_mp_calibration_type {
	RTW_MP_CAL_CHL_RFK = 0,
	RTW_MP_CAL_DACK = 1,
	RTW_MP_CAL_IQK = 2,
	RTW_MP_CAL_LCK = 3,
	RTW_MP_CAL_DPK = 4,
	RTW_MP_CAL_DPK_TRACK = 5,
	RTW_MP_CAL_TSSI = 6,
	RTW_MP_CAL_GAPK = 7,
	RTW_MP_CAL_MAX,
};

enum RTW_TEST_SUB_MODULE {
	RTW_TEST_SUB_MODULE_MP = 0,
	RTW_TEST_SUB_MODULE_FPGA = 1,
	RTW_TEST_SUB_MODULE_VERIFY = 2,
	RTW_TEST_SUB_MODULE_TOOL = 3,
	RTW_TEST_SUB_MODULE_TRX = 4,
	RTW_TEST_SUB_MODULE_UNKNOWN,
};

struct rtw_test_module_info {
	u8 tm_type;
	u8 tm_mode;
};

#define RTW_MAX_TEST_CMD_BUF 2000
struct rtw_mp_test_cmdbuf {
	u8 type;
	u16 len;
	u8 buf[RTW_MAX_TEST_CMD_BUF];
};

enum rtw_mp_nss
 {
	MP_NSS1,
	MP_NSS2,
	MP_NSS3,
	MP_NSS4
 };

#define RU_TONE_STR(idx)\
	(idx == MP_RU_TONE_26) ? "26-Tone" :\
	(idx == MP_RU_TONE_52) ? "52-Tone" :\
	(idx == MP_RU_TONE_106) ? "106-Tone" :\
	(idx == MP_RU_TONE_242) ? "242-Tone" :\
	(idx == MP_RU_TONE_484) ? "484-Tone" :\
	(idx == MP_RU_TONE_966) ? "966-Tone" :\
	"UNknow"

enum rtw_mp_resourceUnit
{
	MP_RU_TONE_26 = 0,
	MP_RU_TONE_52,
	MP_RU_TONE_106,
	MP_RU_TONE_242,
	MP_RU_TONE_484,
	MP_RU_TONE_966
};

#define MP_IS_HT_HRATE(_rate)	((_rate) >= HRATE_MCS0 && (_rate) <= HRATE_MCS31)
#define MP_IS_VHT_HRATE(_rate)	((_rate) >= HRATE_VHT_NSS1_MCS0 && (_rate) <= HRATE_VHT_NSS4_MCS9)
#define MP_IS_CCK_HRATE(_rate)	((_rate) == HRATE_CCK1 || (_rate) == HRATE_CCK2 || \
								 (_rate) == HRATE_CCK5_5 || (_rate) == HRATE_CCK11)

#define MP_IS_OFDM_HRATE(_rate)	((_rate) >= HRATE_OFDM6 && (_rate) <= HRATE_OFDM54)
#define MP_IS_HE_HRATE(_rate)	((_rate) >= HRATE_HE_NSS1_MCS0 && (_rate) <= HRATE_HE_NSS4_MCS11)

#define MP_IS_HT1SS_HRATE(_rate) ((_rate) >= HRATE_MCS0 && (_rate) <= HRATE_MCS7)
#define MP_IS_HT2SS_HRATE(_rate) ((_rate) >= HRATE_MCS8 && (_rate) <= HRATE_MCS15)
#define MP_IS_HT3SS_HRATE(_rate) ((_rate) >= HRATE_MCS16 && (_rate) <= HRATE_MCS23)
#define MP_IS_HT4SS_HRATE(_rate) ((_rate) >= HRATE_MCS24 && (_rate) <= HRATE_MCS31)

#define MP_IS_VHT1SS_HRATE(_rate) ((_rate) >= HRATE_VHT_NSS1_MCS0 && (_rate) <= HRATE_VHT_NSS1_MCS9)
#define MP_IS_VHT2SS_HRATE(_rate) ((_rate) >= HRATE_VHT_NSS2_MCS0 && (_rate) <= HRATE_VHT_NSS2_MCS9)
#define MP_IS_VHT3SS_HRATE(_rate) ((_rate) >= HRATE_VHT_NSS3_MCS0 && (_rate) <= HRATE_VHT_NSS3_MCS9)
#define MP_IS_VHT4SS_HRATE(_rate) ((_rate) >= HRATE_VHT_NSS4_MCS0 && (_rate) <= HRATE_VHT_NSS4_MCS9)

#define MP_IS_HE1SS_HRATE(_rate) ((_rate) >= HRATE_HE_NSS1_MCS0 && (_rate) <= HRATE_HE_NSS1_MCS11)
#define MP_IS_HE2SS_HRATE(_rate) ((_rate) >= HRATE_HE_NSS2_MCS0 && (_rate) <= HRATE_HE_NSS2_MCS11)
#define MP_IS_HE3SS_HRATE(_rate) ((_rate) >= HRATE_HE_NSS3_MCS0 && (_rate) <= HRATE_HE_NSS3_MCS11)
#define MP_IS_HE4SS_HRATE(_rate) ((_rate) >= HRATE_HE_NSS4_MCS0 && (_rate) <= HRATE_HE_NSS4_MCS11)

#define MP_IS_1T_HRATE(_rate)	(MP_IS_CCK_HRATE((_rate)) || MP_IS_OFDM_HRATE((_rate)) \
								|| MP_IS_HT1SS_HRATE((_rate)) || MP_IS_VHT1SS_HRATE((_rate)) \
								|| MP_IS_HE1SS_HRATE((_rate)))

#define MP_IS_2T_HRATE(_rate)	(MP_IS_HT2SS_HRATE((_rate)) || MP_IS_VHT2SS_HRATE((_rate)) \
								|| MP_IS_HE2SS_HRATE((_rate)))

#define MP_IS_3T_HRATE(_rate)	(MP_IS_HT3SS_HRATE((_rate)) || MP_IS_VHT3SS_HRATE((_rate)) \
								|| MP_IS_HE3SS_HRATE((_rate)))

#define MP_IS_4T_HRATE(_rate)	(MP_IS_HT4SS_HRATE((_rate)) || MP_IS_VHT4SS_HRATE((_rate)) \
								|| MP_IS_HE4SS_HRATE((_rate)))



void rtw_mp_get_phl_cmd(_adapter *padapter, void* buf, u32 buflen);
void rtw_mp_set_phl_cmd(_adapter *padapter, void* buf, u32 buflen);

bool rtw_mp_phl_config_arg(_adapter *padapter, enum rtw_mp_config_cmdid cmdid);
void rtw_mp_phl_rx_physts(_adapter *padapter, struct rtw_mp_rx_arg *rx_arg, bool bstart);
void rtw_mp_phl_rx_rssi(_adapter *padapter, struct rtw_mp_rx_arg *rx_arg);
void rtw_mp_phl_rx_gain_offset(_adapter *padapter, struct rtw_mp_rx_arg *rx_arg, u8 path_num);
void rtw_mp_phl_query_rx(_adapter *padapter, struct rtw_mp_rx_arg *rx_arg ,u8 rx_qurey_type);
u8 rtw_mp_phl_txpower(_adapter *padapter, struct rtw_mp_txpwr_arg	*ptxpwr_arg, u8 cmdid);
void rtw_mp_set_crystal_cap(_adapter *padapter, u32 xcapvalue);
u8 rtw_mp_phl_calibration(_adapter *padapter, struct rtw_mp_cal_arg	*pcal_arg, u8 cmdid);
u8 rtw_mp_phl_reg(_adapter *padapter, struct rtw_mp_reg_arg	*reg_arg, u8 cmdid);


u8 rtw_update_giltf(_adapter *padapter);
void rtw_mp_update_coding(_adapter *padapter);
u8 rtw_mp_update_ru_tone(_adapter *padapter);
u8 rtw_mp_update_ru_alloc(_adapter *padapter);

bool rtw_mp_is_cck_rate(u16 rate);

extern s32 init_mp_priv(_adapter *padapter);
extern void free_mp_priv(struct mp_priv *pmp_priv);
extern s32 MPT_InitializeAdapter(_adapter *padapter, u8 Channel);
extern void MPT_DeInitAdapter(_adapter *padapter);
extern s32 mp_start_test(_adapter *padapter);
extern void mp_stop_test(_adapter *padapter);


extern void write_bbreg(_adapter *padapter, u32 addr, u32 bitmask, u32 val);
extern u32 read_rfreg(_adapter *padapter, u8 rfpath, u32 addr);
extern void write_rfreg(_adapter *padapter, u8 rfpath, u32 addr, u32 val);
#ifdef CONFIG_ANTENNA_DIVERSITY
u8 rtw_mp_set_antdiv(_adapter *padapter, BOOLEAN bMain);
#endif
void	SetChannel(_adapter *adapter);
void	SetBandwidth(_adapter *adapter);
int	rtw_mp_txpoweridx(_adapter *adapter);
u16 rtw_mp_txpower_dbm(_adapter *adapter, u8 rf_path);
u16 rtw_mp_get_pwrtab_dbm(_adapter *adapter, u8 rfpath);

void	SetAntenna(_adapter *adapter);
void	SetDataRate(_adapter *adapter);
s32	SetThermalMeter(_adapter *adapter, u8 target_ther);
void	GetThermalMeter(_adapter *adapter, u8 rfpath ,u8 *value);
void	GetUuid(_adapter *adapter, u32 *uuid);
void	rtw_mp_continuous_tx(_adapter *adapter, u8 bstart);
void	rtw_mp_singlecarrier_tx(_adapter *adapter, u8 bstart);
void	rtw_mp_singletone_tx(_adapter *adapter, u8 bstart);
void	rtw_mp_carriersuppr_tx(_adapter *adapter, u8 bstart);
void	rtw_mp_txpwr_level(_adapter *adapter);
void	fill_txdesc_for_mp(_adapter *padapter, u8 *ptxdesc);
void	rtw_set_phl_packet_tx(_adapter *padapter, u8 bStart);
u8	rtw_phl_mp_tx_cmd(_adapter *padapter, enum rtw_mp_tx_cmd cmdid,
						enum rtw_mp_tx_method tx_method, boolean bstart);

void	rtw_mp_set_packet_tx(_adapter *padapter);
void	rtw_mp_reset_phy_count(_adapter *adapter);

s32	SetPowerTracking(_adapter *padapter, u8 enable);
void	GetPowerTracking(_adapter *padapter, u8 *enable);
u32	mp_query_psd(_adapter *adapter, u8 *data);
void	rtw_mp_trigger_iqk(_adapter *padapter);
void	rtw_mp_trigger_lck(_adapter *padapter);
void	rtw_mp_trigger_dpk(_adapter *padapter);
u8 rtw_mp_mode_check(_adapter *padapter);
bool rtw_is_mp_tssitrk_on(_adapter *adapter);

void mpt_ProSetPMacTx(_adapter *adapter);
void MP_PHY_SetRFPathSwitch(_adapter *adapter , BOOLEAN bMain);
void mp_phy_switch_rf_path_set(_adapter *adapter , u8 *pstate);
u8 MP_PHY_QueryRFPathSwitch(_adapter *adapter);
u32 mpt_ProQueryCalTxPower(_adapter *adapter, u8 RfPath);
u8 mpt_to_mgnt_rate(u32	MptRateIdx);
u16 rtw_mp_rate_parse(_adapter *adapter, u8 *target_str);
u32 mp_join(_adapter *padapter, u8 mode);
u32 hal_mpt_query_phytxok(_adapter *adapter);
u32 mpt_get_tx_power_finalabs_val(_adapter *padapter, u8 rf_path);
void mpt_trigger_tssi_tracking(_adapter *adapter, u8 rf_path);
u8 rtw_mpt_set_power_limit_en(_adapter *padapter, bool en_val);
bool rtw_mpt_get_power_limit_en(_adapter *padapter);

u32 rtw_mp_get_tssi_de(_adapter *padapter, u8 rf_path);
s32 rtw_mp_get_online_tssi_de(_adapter *padapter, s32 out_pwr, s32 tgdbm, u8 rf_path);
u8 rtw_mp_set_tsside2verify(_adapter *padapter, u32 tssi_de, u8 rf_path);
u8 rtw_mp_set_tssi_offset(_adapter *padapter, u32 tssi_offset, u8 rf_path);
u8 rtw_mp_set_tssi_pwrtrk(_adapter *padapter, u8 tssi_state);
u8 rtw_mp_get_tssi_pwrtrk(_adapter *padapter);

void rtw_mp_cal_trigger(_adapter *padapter, u8 cal_tye);
void rtw_mp_cal_capab(_adapter *padapter, u8 cal_tye, u8 benable);

void
PMAC_Get_Pkt_Param(
	PRT_PMAC_TX_INFO	pPMacTxInfo,
	PRT_PMAC_PKT_INFO	pPMacPktInfo
);
void
CCK_generator(
	PRT_PMAC_TX_INFO	pPMacTxInfo,
	PRT_PMAC_PKT_INFO	pPMacPktInfo
);
void
PMAC_Nsym_generator(
	PRT_PMAC_TX_INFO	pPMacTxInfo,
	PRT_PMAC_PKT_INFO	pPMacPktInfo
);
void
L_SIG_generator(
	u32	N_SYM,		/* Max: 750*/
	PRT_PMAC_TX_INFO	pPMacTxInfo,
	PRT_PMAC_PKT_INFO	pPMacPktInfo
);

void HT_SIG_generator(
	PRT_PMAC_TX_INFO	pPMacTxInfo,
	PRT_PMAC_PKT_INFO	pPMacPktInfo);

void VHT_SIG_A_generator(
	PRT_PMAC_TX_INFO	pPMacTxInfo,
	PRT_PMAC_PKT_INFO	pPMacPktInfo);

void VHT_SIG_B_generator(
	PRT_PMAC_TX_INFO	pPMacTxInfo);

void VHT_Delimiter_generator(
	PRT_PMAC_TX_INFO	pPMacTxInfo);


int rtw_mp_write_reg(struct net_device *dev,
		struct iw_request_info *info,
		struct iw_point *wrqu, char *extra);
int rtw_mp_read_reg(struct net_device *dev,
		struct iw_request_info *info,
		struct iw_point *wrqu, char *extra);
int rtw_mp_write_rf(struct net_device *dev,
		struct iw_request_info *info,
		struct iw_point *wrqu, char *extra);
int rtw_mp_read_rf(struct net_device *dev,
		struct iw_request_info *info,
		struct iw_point *wrqu, char *extra);
int rtw_mp_start(struct net_device *dev,
		struct iw_request_info *info,
		struct iw_point *wrqu, char *extra);
int rtw_mp_stop(struct net_device *dev,
		struct iw_request_info *info,
		struct iw_point *wrqu, char *extra);
int rtw_mp_rate(struct net_device *dev,
		struct iw_request_info *info,
		struct iw_point *wrqu, char *extra);
int rtw_mp_channel(struct net_device *dev,
		struct iw_request_info *info,
		struct iw_point *wrqu, char *extra);
int rtw_mp_trxsc_offset(struct net_device *dev,
		struct iw_request_info *info,
		struct iw_point *wrqu, char *extra);
int rtw_mp_bandwidth(struct net_device *dev,
		struct iw_request_info *info,
		struct iw_point *wrqu, char *extra);
int rtw_mp_txpower_index(struct net_device *dev,
		struct iw_request_info *info,
		struct iw_point *wrqu, char *extra);
int rtw_mp_txpower(struct net_device *dev,
		struct iw_request_info *info,
		struct iw_point *wrqu, char *extra);
int rtw_mp_ant_tx(struct net_device *dev,
		struct iw_request_info *info,
		struct iw_point *wrqu, char *extra);
int rtw_mp_ant_rx(struct net_device *dev,
		struct iw_request_info *info,
		struct iw_point *wrqu, char *extra);
int rtw_set_ctx_destAddr(struct net_device *dev,
		struct iw_request_info *info,
		struct iw_point *wrqu, char *extra);
int rtw_mp_ctx(struct net_device *dev,
		struct iw_request_info *info,
		struct iw_point *wrqu, char *extra);
int rtw_mp_disable_bt_coexist(struct net_device *dev,
		struct iw_request_info *info,
		union iwreq_data *wrqu, char *extra);
int rtw_mp_disable_bt_coexist(struct net_device *dev,
		struct iw_request_info *info,
		union iwreq_data *wrqu, char *extra);
int rtw_mp_arx(struct net_device *dev,
		struct iw_request_info *info,
		struct iw_point *wrqu, char *extra);
int rtw_mp_trx_query(struct net_device *dev,
		struct iw_request_info *info,
		struct iw_point *wrqu, char *extra);
int rtw_mp_pwrtrk(struct net_device *dev,
		struct iw_request_info *info,
		struct iw_point *wrqu, char *extra);
int rtw_mp_psd(struct net_device *dev,
		struct iw_request_info *info,
		struct iw_point *wrqu, char *extra);
int rtw_mp_thermal(struct net_device *dev,
		struct iw_request_info *info,
		struct iw_point *wrqu, char *extra);
int rtw_mp_UUID(struct net_device *dev,
		struct iw_request_info *info,
		struct iw_point *wrqu, char *extra);
int rtw_mp_reset_stats(struct net_device *dev,
		struct iw_request_info *info,
		struct iw_point *wrqu, char *extra);
int rtw_mp_dump(struct net_device *dev,
		struct iw_request_info *info,
		struct iw_point *wrqu, char *extra);
int rtw_mp_phypara(struct net_device *dev,
		struct iw_request_info *info,
		struct iw_point *wrqu, char *extra);
int rtw_mp_SetRFPath(struct net_device *dev,
		struct iw_request_info *info,
		struct iw_point *wrqu, char *extra);
int rtw_mp_switch_rf_path(struct net_device *dev,
			struct iw_request_info *info,
			struct iw_point *wrqu, char *extra);
int rtw_mp_link(struct net_device *dev,
		struct iw_request_info *info,
		struct iw_point *wrqu, char *extra);
int rtw_mp_QueryDrv(struct net_device *dev,
		struct iw_request_info *info,
		union iwreq_data *wrqu, char *extra);
int rtw_mp_PwrCtlDM(struct net_device *dev,
		struct iw_request_info *info,
		struct iw_point *wrqu, char *extra);
int rtw_mp_getver(struct net_device *dev,
		struct iw_request_info *info,
		union iwreq_data *wrqu, char *extra);
int rtw_mp_mon(struct net_device *dev,
		struct iw_request_info *info,
		union iwreq_data *wrqu, char *extra);
int rtw_mp_pwrlmt(struct net_device *dev,
		struct iw_request_info *info,
		union iwreq_data *wrqu, char *extra);
int rtw_mp_dpk_track(struct net_device *dev,
			struct iw_request_info *info,
			union iwreq_data *wrqu, char *extra);
int rtw_mp_dpk(struct net_device *dev,
			struct iw_request_info *info,
			union iwreq_data *wrqu, char *extra);
#if 0
int rtw_efuse_mask_file(struct net_device *dev,
		struct iw_request_info *info,
		union iwreq_data *wrqu, char *extra);
int rtw_bt_efuse_mask_file(struct net_device *dev,
		struct iw_request_info *info,
		union iwreq_data *wrqu, char *extra);

int rtw_efuse_file_map(struct net_device *dev,
		struct iw_request_info *info,
		union iwreq_data *wrqu, char *extra);
int rtw_efuse_file_map_store(struct net_device *dev,
		struct iw_request_info *info,
		union iwreq_data *wrqu, char *extra);
int rtw_bt_efuse_file_map(struct net_device *dev,
		struct iw_request_info *info,
		union iwreq_data *wrqu, char *extra);
#endif

int rtw_mp_SetBT(struct net_device *dev,
		struct iw_request_info *info,
		union iwreq_data *wrqu, char *extra);
int rtw_mp_pretx_proc(_adapter *padapter, u8 bStartTest, char *extra);
int rtw_mp_tx(struct net_device *dev,
		struct iw_request_info *info,
		union iwreq_data *wrqu, char *extra);
int rtw_mp_rx(struct net_device *dev,
		struct iw_request_info *info,
		union iwreq_data *wrqu, char *extra);
int rtw_mp_hwtx(struct net_device *dev,
		struct iw_request_info *info,
		union iwreq_data *wrqu, char *extra);
u8 rtw_mp_hwrate2mptrate(u8 rate);
int rtw_mp_iqk(struct net_device *dev,
		 struct iw_request_info *info,
		 struct iw_point *wrqu, char *extra);
int rtw_mp_lck(struct net_device *dev,
		struct iw_request_info *info,
		struct iw_point *wrqu, char *extra);
int rtw_mp_get_tsside(struct net_device *dev,
		struct iw_request_info *info,
		struct iw_point *wrqu, char *extra);
int rtw_mp_set_tsside(struct net_device *dev,
		struct iw_request_info *info,
		struct iw_point *wrqu, char *extra);

int rtw_priv_mp_set(struct net_device *dev,
			   struct iw_request_info *info,
			   union iwreq_data *wdata, char *extra);

int rtw_priv_mp_get(struct net_device *dev,
			   struct iw_request_info *info,
			   union iwreq_data *wdata, char *extra);

int rtw_mp_set_phl_io(struct net_device *dev,
			 struct iw_request_info *info,
			 struct iw_point *wrqu, char *extra);

int rtw_mp_get_phl_io(struct net_device *dev,
			 struct iw_request_info *info,
			 struct iw_point *wrqu, char *extra);

int rtw_mp_tx_pattern_idx(struct net_device *dev,
			 struct iw_request_info *info,
			 union iwreq_data *wrqu, char *extra);

int rtw_mp_tx_plcp_tx_data(struct net_device *dev,
			 struct iw_request_info *info,
			 union iwreq_data *wrqu, char *extra);

int rtw_mp_tx_plcp_tx_user(struct net_device *dev,
			 struct iw_request_info *info,
			 union iwreq_data *wrqu, char *extra);

int rtw_mp_tx_method(struct net_device *dev,
			 struct iw_request_info *info,
			 union iwreq_data *wrqu, char *extra);

int rtw_mp_config_phy(struct net_device *dev,
			 struct iw_request_info *info,
			 union iwreq_data *wrqu, char *extra);

int rtw_mp_phl_rfk(struct net_device *dev,
			 struct iw_request_info *info,
			 union iwreq_data *wrqu, char *extra);
int rtw_mp_phl_btc_path(struct net_device *dev,
			 struct iw_request_info *info,
			 union iwreq_data *wrqu, char *extra);
int rtw_mp_get_he(struct net_device *dev,
			 struct iw_request_info *info,
			 union iwreq_data *wrqu, char *extra);
int rtw_mp_band(struct net_device *dev,
			 struct iw_request_info *info,
			 union iwreq_data *wrqu, char *extra);
#endif /* _RTW_MP_H_ */

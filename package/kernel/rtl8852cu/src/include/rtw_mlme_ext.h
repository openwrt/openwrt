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
#ifndef __RTW_MLME_EXT_H_
#define __RTW_MLME_EXT_H_

/*
 * SURVEY_TO: dwell time (ms) for passive channel scan or other scan functions
 * in addition to normal scan.
 * ACTIVE_CH_SURVEY_TO: dwell time (ms) for active scan channel in nomal scan.
 */
#define SURVEY_TO		(100)
#define ACTIVE_CH_SURVEY_TO	(50)

#define REAUTH_TO		(300) /* (50) */
#define REASSOC_TO		(300) /* (50) */
/* #define DISCONNECT_TO	(3000) */
#define ADDBA_TO			(2000)

#define LINKED_TO (1) /* unit:2 sec, 1x2 = 2 sec */

#define REAUTH_LIMIT	(4)
#define REASSOC_LIMIT	(4)
#define READDBA_LIMIT	(2)

#ifdef CONFIG_GSPI_HCI
	#define ROAMING_LIMIT	5
#else
	#define ROAMING_LIMIT	8
#endif

/*net_type, pmlmeinfo->state, pstat->state*/
#define	_HW_STATE_NOLINK_	0x00
#define	_HW_STATE_ADHOC_	0x01
#define	_HW_STATE_STATION_	0x02
#define	_HW_STATE_AP_		0x03
#define	_HW_STATE_MONITOR_ 	0x04

#define	WIFI_FW_NULL_STATE		_HW_STATE_NOLINK_
#define	WIFI_FW_STATION_STATE		_HW_STATE_STATION_
#define	WIFI_FW_AP_STATE		_HW_STATE_AP_
#define	WIFI_FW_ADHOC_STATE		_HW_STATE_ADHOC_

#define 	WIFI_FW_PRE_LINK		0x00000800
#define	WIFI_FW_AUTH_NULL		0x00000100
#define	WIFI_FW_AUTH_STATE		0x00000200
#define	WIFI_FW_AUTH_SUCCESS		0x00000400

#define	WIFI_FW_ASSOC_STATE		0x00002000
#define	WIFI_FW_ASSOC_SUCCESS		0x00004000

#define	WIFI_FW_LINKING_STATE		(WIFI_FW_AUTH_NULL | WIFI_FW_AUTH_STATE | WIFI_FW_AUTH_SUCCESS | WIFI_FW_ASSOC_STATE)


#define		_1M_RATE_	0
#define		_2M_RATE_	1
#define		_5M_RATE_	2
#define		_11M_RATE_	3
#define		_6M_RATE_	4
#define		_9M_RATE_	5
#define		_12M_RATE_	6
#define		_18M_RATE_	7
#define		_24M_RATE_	8
#define		_36M_RATE_	9
#define		_48M_RATE_	10
#define		_54M_RATE_	11

/********************************************************
MCS rate definitions
*********************************************************/
#define MCS_RATE_1R	(0x000000ff)
#define MCS_RATE_2R	(0x0000ffff)
#define MCS_RATE_3R	(0x00ffffff)
#define MCS_RATE_4R	(0xffffffff)
#define MCS_RATE_2R_13TO15_OFF	(0x00001fff)


extern unsigned char RTW_WPA_OUI[];
extern unsigned char WMM_OUI[];
extern unsigned char WPS_OUI[];
extern unsigned char WFD_OUI[];
extern unsigned char P2P_OUI[];
extern unsigned char MULTI_AP_OUI[];

extern unsigned char WMM_INFO_OUI[];
extern unsigned char WMM_PARA_OUI[];

#if defined(PRIVATE_R) && defined(CONFIG_P2P)
extern int rtw_go_hidden_ssid_mode;
enum {
	MIRACAST = 0,
	PAIRING_MODE = 1,
	CAPTIVE_PORTAL = 2,
};
#define ALL_HIDE_SSID 0xff
#define MIRACAST_UNHIDE_SSID 0xfe
#endif

typedef enum _HT_IOT_PEER {
	HT_IOT_PEER_UNKNOWN			= 0,
	HT_IOT_PEER_REALTEK			= 1,
	HT_IOT_PEER_REALTEK_92SE		= 2,
	HT_IOT_PEER_BROADCOM		= 3,
	HT_IOT_PEER_RALINK			= 4,
	HT_IOT_PEER_ATHEROS			= 5,
	HT_IOT_PEER_CISCO				= 6,
	HT_IOT_PEER_MERU				= 7,
	HT_IOT_PEER_MARVELL			= 8,
	HT_IOT_PEER_REALTEK_SOFTAP 	= 9,/* peer is RealTek SOFT_AP, by Bohn, 2009.12.17 */
	HT_IOT_PEER_SELF_SOFTAP 		= 10, /* Self is SoftAP */
	HT_IOT_PEER_AIRGO				= 11,
	HT_IOT_PEER_INTEL				= 12,
	HT_IOT_PEER_RTK_APCLIENT		= 13,
	HT_IOT_PEER_REALTEK_81XX		= 14,
	HT_IOT_PEER_REALTEK_WOW		= 15,
	HT_IOT_PEER_REALTEK_JAGUAR_CBVAP = 16,
	HT_IOT_PEER_REALTEK_JAGUAR_CCVAP = 17,
	HT_IOT_PEER_MAX				= 18
} HT_IOT_PEER_E, *PHTIOT_PEER_E;


typedef enum _RT_HT_INF0_CAP {
	RT_HT_CAP_USE_TURBO_AGGR = 0x01,
	RT_HT_CAP_USE_LONG_PREAMBLE = 0x02,
	RT_HT_CAP_USE_AMPDU = 0x04,
	RT_HT_CAP_USE_WOW = 0x8,
	RT_HT_CAP_USE_SOFTAP = 0x10,
	RT_HT_CAP_USE_92SE = 0x20,
	RT_HT_CAP_USE_88C_92C = 0x40,
	RT_HT_CAP_USE_AP_CLIENT_MODE = 0x80,	/* AP team request to reserve this bit, by Emily */
} RT_HT_INF0_CAPBILITY, *PRT_HT_INF0_CAPBILITY;

typedef enum _RT_HT_INF1_CAP {
	RT_HT_CAP_USE_VIDEO_CLIENT = 0x01,
	RT_HT_CAP_USE_JAGUAR_CBV = 0x02,
	RT_HT_CAP_USE_JAGUAR_CCV = 0x04,
} RT_HT_INF1_CAPBILITY, *PRT_HT_INF1_CAPBILITY;

struct mlme_handler {
	unsigned int   num;
	char *str;
	unsigned int (*func)(_adapter *padapter, union recv_frame *precv_frame);
};

struct action_handler {
	unsigned int   num;
	char *str;
	unsigned int (*func)(_adapter *padapter, union recv_frame *precv_frame);
};

#ifdef CONFIG_TDLS
enum TDLS_option {
	TDLS_ESTABLISHED = 1,
	TDLS_ISSUE_PTI,
	TDLS_CH_SW_RESP,
	TDLS_CH_SW_PREPARE,
	TDLS_CH_SW_START,
	TDLS_CH_SW_TO_OFF_CHNL,
	TDLS_CH_SW_TO_BASE_CHNL_UNSOLICITED,
	TDLS_CH_SW_TO_BASE_CHNL,
	TDLS_CH_SW_END_TO_BASE_CHNL,
	TDLS_CH_SW_END,
	TDLS_RS_RCR,
	TDLS_TEARDOWN_STA,
	TDLS_TEARDOWN_STA_TOOFAR,
	TDLS_TEARDOWN_STA_NO_WAIT,
	TDLS_TEARDOWN_STA_LOCALLY,
	TDLS_TEARDOWN_STA_LOCALLY_POST,
	maxTDLS,
};

#endif /* CONFIG_TDLS */

typedef enum {
	DISCONNECTION_NOT_YET_OCCUR,
	DISCONNECTION_BY_SYSTEM_DUE_TO_HIGH_LAYER_COMMAND,
	DISCONNECTION_BY_SYSTEM_DUE_TO_NET_DEVICE_DOWN,
	DISCONNECTION_BY_SYSTEM_DUE_TO_SYSTEM_IN_SUSPEND,
	DISCONNECTION_BY_DRIVER_DUE_TO_CONNECTION_EXIST,
	DISCONNECTION_BY_DRIVER_DUE_TO_EACH_IFACE_CHBW_NOT_SYNC,
	/* DISCONNECTION_BY_DRIVER_DUE_TO_DFS_DETECTION, */
	DISCONNECTION_BY_DRIVER_DUE_TO_IOCTL_DBG_PORT,
	DISCONNECTION_BY_DRIVER_DUE_TO_AP_BEACON_CHANGED,
	DISCONNECTION_BY_DRIVER_DUE_TO_KEEPALIVE_TIMEOUT,
	DISCONNECTION_BY_DRIVER_DUE_TO_LAYER2_ROAMING_TERMINATE,
	DISCONNECTION_BY_DRIVER_DUE_TO_JOINBSS_TIMEOUT,
	DISCONNECTION_BY_FW_DUE_TO_FW_DECISION_IN_WOW_RESUME,
	DISCONNECTION_BY_AP_DUE_TO_RECEIVE_DISASSOC_IN_WOW_RESUME,
	DISCONNECTION_BY_AP_DUE_TO_RECEIVE_DEAUTH_IN_WOW_RESUME,
	DISCONNECTION_BY_AP_DUE_TO_RECEIVE_DEAUTH,
	DISCONNECTION_BY_AP_DUE_TO_RECEIVE_DISASSOC,
	/* DISCONNECTION_BY_DRIVER_DUE_TO_RECEIVE_CSA_NON_DFS, */
	/* DISCONNECTION_BY_DRIVER_DUE_TO_RECEIVE_CSA_DFS, */
	DISCONNECTION_BY_DRIVER_DUE_TO_RECEIVE_INVALID_CSA,
	DISCONNECTION_BY_DRIVER_DUE_TO_JOIN_WRONG_CHANNEL,
	DISCONNECTION_BY_DRIVER_DUE_TO_FT,
	DISCONNECTION_BY_DRIVER_DUE_TO_ROAMING,
	DISCONNECTION_BY_DRIVER_DUE_TO_SA_QUERY_TIMEOUT,
} Disconnect_type;

#define SSID_CHANGED BIT0
#define SSID_LENGTH_CHANGED BIT1
#define BEACON_CHANNEL_CHANGED BIT2
#define ENCRYPT_PROTOCOL_CHANGED BIT3
#define PAIRWISE_CIPHER_CHANGED BIT4
#define GROUP_CIPHER_CHANGED BIT5
#define IS_8021X_CHANGED BIT6

/*
 * Usage:
 * When one iface acted as AP mode and the other iface is STA mode and scanning,
 * it should switch back to AP's operating channel periodically.
 * Parameters info:
 * When the driver scanned RTW_SCAN_NUM_OF_CH channels, it would switch back to AP's operating channel for
 * RTW_BACK_OP_CH_MS milliseconds.
 * Example:
 * For chip supports 2.4G + 5GHz and AP mode is operating in channel 1,
 * RTW_SCAN_NUM_OF_CH is 8, RTW_BACK_OP_CH_MS is 300
 * When it's STA mode gets set_scan command,
 * it would
 * 1. Doing the scan on channel 1.2.3.4.5.6.7.8
 * 2. Back to channel 1 for 300 milliseconds
 * 3. Go through doing site survey on channel 9.10.11.36.40.44.48.52
 * 4. Back to channel 1 for 300 milliseconds
 * 5. ... and so on, till survey done.
 */
#if defined(CONFIG_ATMEL_RC_PATCH)
	#define RTW_SCAN_NUM_OF_CH 2
	#define RTW_BACK_OP_CH_MS 200
#else
	#define RTW_SCAN_NUM_OF_CH 3
	#define RTW_BACK_OP_CH_MS 400
#endif

#define RTW_IP_ADDR_LEN 4
#define RTW_IPv6_ADDR_LEN 16

struct link_mlme_ext_info {
	u16	bcn_interval;
	u16	capability;
	u8	slotTime;
	u8	preamble_mode;
	u8	WMM_enable;
	u8	ERP_enable;
	u8	ERP_IE;
	u8	HT_enable;
	u8	HT_caps_enable;
	u8	HT_info_enable;
	u8	HT_protection;
	u8	SM_PS;
	u8	bwmode_updated;
	u8	VHT_enable;

	u8	HE_enable;

	struct WMM_para_element	WMM_param;
	struct HT_caps_element	HT_caps;
	struct HT_info_element		HT_info;
	WLAN_BSSID_EX			network;/* join network or bss_network, if in ap mode, it is the same to cur_network.network */
#ifdef PRIVATE_R
	/*infra mode, store supported rates from AssocRsp*/
	NDIS_802_11_RATES_EX	SupportedRates_infra_ap;
	u8 ht_vht_received;/*ht_vht_received used to show debug msg BIT(0):HT BIT(1):VHT */
#endif /* PRIVATE_R */
};

struct mlme_ext_info {
	u32	state;
	u32	reauth_count;
	u32	reassoc_count;
	u32	link_count;
	u32	auth_seq;
	u32	auth_algo;	/* 802.11 auth, could be open, shared, auto */
	u16 auth_status;
	u32	authModeToggle;
	u32	enc_algo;/* encrypt algorithm; */
	u32	key_index;	/* this is only valid for legendary wep, 0~3 for key id. */
	u32	iv;
	u8	chg_txt[128];
	u8	assoc_AP_vendor;
	u8	agg_enable_bitmap;
	u8	ADDBA_retry_count;
	u8	candidate_tid_bitmap;
	u8	dialogToken;
	/* Accept ADDBA Request */
	BOOLEAN bAcceptAddbaReq;
	u8	hidden_ssid_mode;

	u8 ip_addr[RTW_IP_ADDR_LEN];
	u8 ip6_addr[RTW_IPv6_ADDR_LEN];

	struct ADDBA_request		ADDBA_req;
	WLAN_BSSID_EX			dev_network;/* join network or bss_network, if in ap mode, it is the same to cur_network.network */

	Disconnect_type disconnect_code;
	u32 illegal_beacon_code;
	u32 wifi_reason_code;
	u32 disconnect_occurred_time;
};

void init_channel_list(_adapter *padapter);

int rtw_rfctl_init(struct dvobj_priv *dvobj);
void rtw_rfctl_deinit(struct dvobj_priv *dvobj);
void rtw_rfctl_update_op_mode(struct rf_ctl_t *rfctl, u8 ifbmp_mod, u8 if_op);

bool rtw_mlme_band_check(_adapter *adapter, const u32 ch);


enum {
	BAND_24G = BIT0,
	BAND_5G = BIT1,
};
void RTW_SET_SCAN_BAND_SKIP(_adapter *padapter, int skip_band);
void RTW_CLR_SCAN_BAND_SKIP(_adapter *padapter, int skip_band);
int RTW_GET_SCAN_BAND_SKIP(_adapter *padapter);

bool rtw_mlme_ignore_chan(_adapter *adapter, const u32 ch);

/* P2P_MAX_REG_CLASSES - Maximum number of regulatory classes */
#define P2P_MAX_REG_CLASSES 10

/* P2P_MAX_REG_CLASS_CHANNELS - Maximum number of channels per regulatory class */
#define P2P_MAX_REG_CLASS_CHANNELS 20

/* struct p2p_channels - List of supported channels */
struct p2p_channels {
	/* struct p2p_reg_class - Supported regulatory class */
	struct p2p_reg_class {
		/* reg_class - Regulatory class (IEEE 802.11-2007, Annex J) */
		u8 reg_class;

		/* channel - Supported channels */
		u8 channel[P2P_MAX_REG_CLASS_CHANNELS];

		/* channels - Number of channel entries in use */
		size_t channels;
	} reg_class[P2P_MAX_REG_CLASSES];

	/* reg_classes - Number of reg_class entries in use */
	size_t reg_classes;
};

struct p2p_oper_class_map {
	enum hw_mode {IEEE80211G, IEEE80211A} mode;
	u8 op_class;
	u8 min_chan;
	u8 max_chan;
	u8 inc;
	enum { BW20, BW40PLUS, BW40MINUS } bw;
};

struct link_mlme_ext_priv {
	struct rtw_chan_def chandef;
	unsigned char	cur_wireless_mode;	/* NETWORK_TYPE */
	unsigned char	basicrate[NumRates];
	unsigned char	datarate[NumRates];
#ifdef CONFIG_80211N_HT
	unsigned char default_supported_mcs_set[16];
#endif
	struct link_mlme_ext_info	mlmext_info;/* for sta/adhoc mode, including current scanning/connecting/connected related info.
					   * for ap mode, network includes ap's cap_info */
	u8	tx_rate; /* TXRATE when USERATE is set. */
	u64 TSFValue;
	u32 bcn_cnt;
	u32 last_bcn_cnt;
	u8 cur_bcn_cnt;/*2s*/
	u8 dtim;/*DTIM Period*/
#ifdef DBG_RX_BCN
	u8 tim[4];
#endif
#ifdef CONFIG_BCN_RECV_TIME
	u16 bcn_rx_time;
#endif
#ifdef CONFIG_AP_MODE
	unsigned char bstart_bss;
#endif
};

struct mlme_ext_priv {
	_adapter	*padapter;
	ATOMIC_T		event_seq;
	u16	mgnt_seq;
#ifdef CONFIG_IEEE80211W
	u16	sa_query_seq;
#endif
	/* struct fw_priv 	fwpriv; */

	unsigned char	dev_cur_wireless_mode;	/* NETWORK_TYPE */

	struct ss_res		sitesurvey_res;
	struct mlme_ext_info	mlmext_info;/* for sta/adhoc mode, including current scanning/connecting/connected related info.
                                                      * for ap mode, network includes ap's cap_info */
	/*_timer		survey_timer;*/
	_timer		link_timer;

#ifdef CONFIG_RTW_80211R
	_timer		ft_link_timer;
	_timer		ft_roam_timer;
#endif
#ifdef CONFIG_RTW_TOKEN_BASED_XMIT
	_timer		tbtx_xmit_timer;
	_timer		tbtx_token_dispatch_timer;
#endif
#ifdef PRIVATE_R
	_timer		find_remote_timer;
#ifdef CONFIG_P2P
	_timer		hide_ssid_timer;
#endif
#endif

	systime last_scan_time;
	u8 scan_abort;
	bool scan_abort_to;
	u8 join_abort;

	u32	retry; /* retry for issue probereq */

#ifdef CONFIG_AP_MODE
	unsigned char bstart_bss;
#endif

	/* recv_decache check for Action_public frame */
	u8 action_public_dialog_token;
	u16	 action_public_rxseq;

	/* #ifdef CONFIG_ACTIVE_KEEP_ALIVE_CHECK */
	u8 active_keep_alive_check;
	/* #endif */
#ifdef DBG_FIXED_CHAN
	u8 fixed_chan;
#endif

#ifdef CONFIG_SUPPORT_STATIC_SMPS
	u8 ssmps_en;
	u16 ssmps_tx_tp_th;/*Mbps*/
	u16 ssmps_rx_tp_th;/*Mbps*/
	#ifdef DBG_STATIC_SMPS
	u8 ssmps_test;
	u8 ssmps_test_en;
	#endif
#endif
#ifdef CONFIG_CTRL_TXSS_BY_TP
	u8 txss_ctrl_en;
	u16 txss_tp_th;/*Mbps*/
	u8 txss_tp_chk_cnt;/*unit 2s*/
	bool txss_1ss;
#endif
	u8 txss_bk;
#ifdef CONFIG_ECSA_PHL
	_timer csa_wait_bcn_timer;
#endif
};

struct support_rate_handler {
	u8 rate;
	bool basic;
	bool existence;
};

static inline u8 check_mlmeinfo_state(struct mlme_ext_priv *plmeext, sint state)
{
	if ((plmeext->mlmext_info.state & 0x03) == state)
		return _TRUE;

	return _FALSE;
}

#define mlmeext_msr(mlmeext) ((mlmeext)->mlmext_info.state & 0x03)

int init_mlme_ext_priv(_adapter *padapter);
int init_hw_mlme_ext(_adapter *padapter);
void free_mlme_ext_priv(struct mlme_ext_priv *pmlmeext);
int rtw_init_link_mlme_ext_priv(struct _ADAPTER_LINK *padapter_link);
extern struct xmit_frame *alloc_mgtxmitframe(struct xmit_priv *pxmitpriv);
struct xmit_frame *alloc_mgtxmitframe_once(struct xmit_priv *pxmitpriv);

void get_rate_set(_adapter *padapter, struct _ADAPTER_LINK *padapter_link,
			unsigned char *pbssrate, int *bssrate_len);
void set_mcs_rate_by_mask(u8 *mcs_set, u32 mask);
void UpdateBrateTbl(_adapter *padapter, u8 *mBratesOS);
void UpdateBrateTblForSoftAP(u8 *bssrateset, u32 bssratelen);
void change_band_update_ie(_adapter *padapter, struct _ADAPTER_LINK *padapter_link,
				WLAN_BSSID_EX *pnetwork, struct rtw_chan_def *chandef);

void rtw_set_external_auth_status(_adapter *padapter, const void *data, int len);

void rtw_get_oper_chdef(_adapter *adapter, struct _ADAPTER_LINK *adapter_link,
				struct rtw_chan_def *chandef);
u8 rtw_get_oper_band(_adapter *adapter, struct _ADAPTER_LINK *adapter_link);
u8 rtw_get_oper_ch(_adapter *adapter, struct _ADAPTER_LINK *adapter_link);
u8 rtw_get_oper_bw(_adapter *adapter, struct _ADAPTER_LINK *adapter_link);
u8 rtw_get_oper_choffset(_adapter *adapter, struct _ADAPTER_LINK *adapter_link);
int rtw_get_oper_chdef_by_hwband(struct dvobj_priv *dvobj, enum phl_band_idx band_idx
	, struct rtw_chan_def *chandef);
int rtw_get_oper_bchbw_by_hwband(struct dvobj_priv *dvobj, enum phl_band_idx band_idx
	, enum band_type *band, u8 *ch, u8 *bw, u8 *offset);

systime rtw_get_on_oper_ch_time(_adapter *adapter);
systime rtw_get_on_cur_ch_time(_adapter *adapter);

RTW_FUNC_2G_5G_ONLY void set_channel_bwmode(_adapter *padapter,
		struct _ADAPTER_LINK *padapter_link,
		unsigned char channel,
		unsigned char channel_offset,
		unsigned short bwmode,
		u8 do_rfk);

void set_bch_bwmode(_adapter *padapter,
		struct _ADAPTER_LINK *padapter_link,
		enum band_type band,
		unsigned char channel,
		unsigned char channel_offset,
		unsigned short bwmode,
		u8 do_rfk);

unsigned int decide_wait_for_beacon_timeout(unsigned int bcn_interval);

BOOLEAN IsLegal5GChannel(_adapter *adapter, u8 channel);

u8 collect_bss_info(_adapter *padapter, union recv_frame *precv_frame, WLAN_BSSID_EX *bssid);
void rtw_update_adapter_network(WLAN_BSSID_EX *dst, WLAN_BSSID_EX *src, _adapter *padapter, bool update_ie);
void rtw_update_network(WLAN_BSSID_EX *dst, WLAN_BSSID_EX *src, _adapter *padapter, bool update_ie);

u8 *get_my_bssid(WLAN_BSSID_EX *pnetwork);
u16 get_beacon_interval(WLAN_BSSID_EX *bss);

int is_client_associated_to_ap(_adapter *padapter);
int is_client_associated_to_ibss(_adapter *padapter);
int is_IBSS_empty(_adapter *padapter);

unsigned char check_assoc_AP(u8 *pframe, uint len);
void get_assoc_AP_Vendor(char *vendor, u8 assoc_AP_vendor);
#ifdef CONFIG_RTS_FULL_BW
void rtw_parse_sta_vendor_ie_8812(_adapter *adapter, struct sta_info *sta, u8 *tlv_ies, u16 tlv_ies_len);
#endif/*CONFIG_RTS_FULL_BW*/
#ifdef CONFIG_80211AC_VHT
void get_vht_bf_cap(u8 *pframe, uint len, struct vht_bf_cap *bf_cap);
#endif

int WMM_param_handler(_adapter *padapter, struct _ADAPTER_LINK *padapter_link,
			PNDIS_802_11_VARIABLE_IEs	pIE);
#ifdef CONFIG_WFD
void rtw_process_wfd_ie(_adapter *adapter, u8 *ie, u8 ie_len, const char *tag);
void rtw_process_wfd_ies(_adapter *adapter, u8 *ies, u8 ies_len, const char *tag);
#endif
void WMMOnAssocRsp(_adapter *padapter, struct _ADAPTER_LINK *padapter_link);
#ifdef CONFIG_RTW_TOKEN_BASED_XMIT
u8 rtw_is_tbtx_capabilty(u8 *p, u8 len);
#endif

void HT_caps_handler(_adapter *padapter, struct _ADAPTER_LINK *padapter_link,
			PNDIS_802_11_VARIABLE_IEs pIE);
#ifdef PRIVATE_R
void HT_caps_handler_infra_ap(_adapter *padapter, struct _ADAPTER_LINK *padapter_link,
			PNDIS_802_11_VARIABLE_IEs pIE);
#endif
void HT_info_handler(_adapter *padapter, struct _ADAPTER_LINK *padapter_link,
			PNDIS_802_11_VARIABLE_IEs pIE);
void HTOnAssocRsp(_adapter *padapter);

#ifdef PRIVATE_R
void Supported_rate_infra_ap(_adapter *padapter, struct _ADAPTER_LINK *padapter_link,
				PNDIS_802_11_VARIABLE_IEs pIE);
void Extended_Supported_rate_infra_ap(_adapter *padapter, struct _ADAPTER_LINK *padapter_link,
				PNDIS_802_11_VARIABLE_IEs pIE);
#endif

void ERP_IE_handler(_adapter *padapter,struct _ADAPTER_LINK *padapter_link,
			PNDIS_802_11_VARIABLE_IEs pIE);
void VCS_update(_adapter *padapter, struct sta_info *psta);
void	update_ldpc_stbc_cap(struct sta_info *psta);

bool rtw_validate_value(u16 EID, u8 *p, u16 len);
bool is_hidden_ssid(char *ssid, int len);
bool hidden_ssid_ap(WLAN_BSSID_EX *snetwork);
void rtw_absorb_ssid_ifneed(_adapter *padapter, WLAN_BSSID_EX *bssid, u8 *pframe);

int rtw_get_bcn_keys(_adapter *adapter, struct _ADAPTER_LINK *adapter_link
	, u8 *whdr, u32 flen, struct beacon_keys *bcn_keys);
int rtw_get_bcn_keys_from_bss(WLAN_BSSID_EX *bss, struct beacon_keys *bcn_keys);
int rtw_update_bcn_keys_of_network(struct wlan_network *network);

int validate_beacon_len(u8 *pframe, uint len);
void rtw_dump_bcn_keys(void *sel, struct beacon_keys *recv_beacon);
bool rtw_bcn_key_compare(struct beacon_keys *cur, struct beacon_keys *recv);
int rtw_check_bcn_info(_adapter *adapter, struct _ADAPTER_LINK *adapter_link,
		u8 *pframe, u32 packet_len);
void update_beacon_info(_adapter *padapter, u8 *pframe, uint len, struct sta_info *psta);
#ifdef CONFIG_ECSA_PHL
void process_csa_ie(_adapter *padapter, u8 *ies, uint ies_len);
#endif
void update_capinfo(_adapter *adapter, struct _ADAPTER_LINK *adapter_link, u16 updateCap);
void update_wireless_mode(_adapter *padapter, struct _ADAPTER_LINK *padapter_link);
void update_tx_basic_rate(_adapter *padapter, struct _ADAPTER_LINK *padapter_link, u8 modulation);
void update_sta_basic_rate(struct sta_info *psta, u8 wireless_mode);
int rtw_ies_get_supported_rate(u8 *ies, uint ies_len, u8 *rate_set, u8 *rate_num);

/* for sta/adhoc mode */
void update_sta_info(_adapter *padapter, struct sta_info *psta);
void update_sta_ra_info(_adapter *padapter, struct sta_info *psta);

s8 rtw_get_sta_rx_nss(_adapter *adapter, struct sta_info *psta);
s8 rtw_get_sta_tx_nss(_adapter *adapter, struct sta_info *psta);


unsigned int update_basic_rate(unsigned char *ptn, unsigned int ptn_sz);
unsigned int update_supported_rate(unsigned char *ptn, unsigned int ptn_sz);

unsigned int receive_disconnect(_adapter *padapter, unsigned char *MacAddr, unsigned short reason, u8 locally_generated);

unsigned char get_highest_rate_idx(u64 mask);
unsigned char get_lowest_rate_idx_ex(u64 mask, int start_bit);
#define get_lowest_rate_idx(mask) get_lowest_rate_idx_ex(mask, 0)

u8 get_highest_bw_cap(u8 bwmode);

int support_short_GI(_adapter *padapter, struct _ADAPTER_LINK *padapter_link,
			struct HT_caps_element *pHT_caps, u8 bwmode);
unsigned int is_ap_in_tkip(struct _ADAPTER_LINK *padapter_link);
unsigned int is_ap_in_wep(struct _ADAPTER_LINK *padapter_link);
unsigned int should_forbid_n_rate(_adapter *padapter);

enum eap_type parsing_eapol_packet(_adapter *padapter, u8 *key_payload, struct sta_info *psta, u8 trx_type);

bool rtw_bmp_is_set(const u8 *bmp, u8 bmp_len, u8 id);
void rtw_bmp_set(u8 *bmp, u8 bmp_len, u8 id);
void rtw_bmp_clear(u8 *bmp, u8 bmp_len, u8 id);
bool rtw_bmp_not_empty(const u8 *bmp, u8 bmp_len);
bool rtw_bmp_not_empty_exclude_bit0(const u8 *bmp, u8 bmp_len);

#ifdef CONFIG_AP_MODE
bool rtw_tim_map_is_set(_adapter *padapter, const u8 *map, u8 id);
void rtw_tim_map_set(_adapter *padapter, u8 *map, u8 id);
void rtw_tim_map_clear(_adapter *padapter, u8 *map, u8 id);
bool rtw_tim_map_anyone_be_set(_adapter *padapter, const u8 *map);
bool rtw_tim_map_anyone_be_set_exclude_aid0(_adapter *padapter, const u8 *map);
#endif /* CONFIG_AP_MODE */

u32 report_join_res(_adapter *padapter, int aid_res, u16 status);
void report_survey_event(_adapter *padapter, union recv_frame *precv_frame);
void report_surveydone_event(_adapter *padapter, bool acs, u8 flags);
u32 report_del_sta_event(_adapter *padapter, unsigned char *MacAddr, unsigned short reason, bool enqueue, u8 locally_generated);
void report_add_sta_event(_adapter *padapter, unsigned char *MacAddr);
void report_wmm_edca_update(_adapter *padapter);

void beacon_timing_control(_adapter *padapter);
u8 chk_bmc_sleepq_cmd(_adapter *padapter);
extern u8 set_tx_beacon_cmd(_adapter *padapter, struct _ADAPTER_LINK *padapter_link, u8 flags);
unsigned int setup_beacon_frame(_adapter *padapter, unsigned char *beacon_frame);
void update_mgnt_tx_rate(_adapter *padapter, struct _ADAPTER_LINK *padapter_link, u8 rate);
void update_monitor_frame_attrib(_adapter *padapter, struct _ADAPTER_LINK *padapter_link,
			struct pkt_attrib *pattrib);
void update_mgntframe_attrib(_adapter *padapter, struct _ADAPTER_LINK *padapter_link,
			struct pkt_attrib *pattrib);
void update_mgntframe_attrib_addr(_adapter *padapter, struct xmit_frame *pmgntframe);
void dump_mgntframe(_adapter *padapter, struct xmit_frame *pmgntframe);
s32 dump_mgntframe_and_wait(_adapter *padapter, struct xmit_frame *pmgntframe, int timeout_ms);
s32 dump_mgntframe_and_wait_ack(_adapter *padapter, struct xmit_frame *pmgntframe);
s32 dump_mgntframe_and_wait_ack_timeout(_adapter *padapter, struct xmit_frame *pmgntframe, int timeout_ms);

#ifdef CONFIG_P2P
void issue_probersp_p2p(_adapter *padapter, unsigned char *da);
void issue_probereq_p2p(_adapter *padapter, u8 *da);
#endif /* CONFIG_P2P */
int rtw_build_assocreq_ies(_adapter *padapter, struct _ADAPTER_LINK *padapter_link,
		u8 *pframe, struct pkt_attrib *pattrib, u8 is_reassoc);
u8 *rtw_build_probersp_ies(_adapter *padapter, struct _ADAPTER_LINK *padapter_link,
		u8 *pframe, struct pkt_attrib *pattrib, u8 is_valid_p2p_probereq);

void issue_beacon(_adapter *padapter, int timeout_ms);
void issue_probersp(_adapter *padapter, struct _ADAPTER_LINK *padapter_link,
		unsigned char *da, u8 is_valid_p2p_probereq);
#ifdef CONFIG_80211BE_EHT
void issue_ml_probersp(_adapter *padapter, struct _ADAPTER_LINK *padapter_link,
		unsigned char *da, u8 is_valid_p2p_probereq);
#endif
void _issue_assocreq(_adapter *padapter, u8 is_assoc);
void issue_assocreq(_adapter *padapter);
void issue_reassocreq(_adapter *padapter);
void issue_asocrsp(_adapter *padapter, unsigned short status, struct sta_info *pstat, int pkt_type);
void issue_auth(_adapter *padapter, struct sta_info *psta, unsigned short status);
void issue_probereq(_adapter *padapter, struct _ADAPTER_LINK *padapter_link, const NDIS_802_11_SSID *pssid, const u8 *da);
s32 issue_probereq_ex(_adapter *padapter, struct _ADAPTER_LINK *padapter_link, const NDIS_802_11_SSID *pssid, const u8 *da, u8 ch, bool append_wps, int try_cnt, int wait_ms);
int rtw_match_self_addr(_adapter *padapter, const u8 *mac_addr);
int rtw_match_bssid(_adapter *padapter, const u8 *mac_addr);
int rtw_match_bssid_ext(_adapter *padapter, const u8 *mac_addr);
int issue_nulldata(_adapter *padapter, struct _ADAPTER_LINK *padapter_link,
			unsigned char *da, unsigned int power_mode, int try_cnt, int wait_ms);
bool rtw_core_issu_null_data(void *priv, u8 ridx, u8 link_idx, bool ps);

int issue_qos_nulldata(_adapter *padapter, struct _ADAPTER_LINK *padapter_link,
		unsigned char *da, u16 tid, u8 ps, int try_cnt, int wait_ms, u8 need_om);
int issue_deauth(_adapter *padapter, unsigned char *da, unsigned short reason);
int issue_deauth_ex(_adapter *padapter, u8 *da, unsigned short reason, int try_cnt, int wait_ms);
int issue_disassoc(_adapter *padapter, unsigned char *da, unsigned short reason);
void issue_action_spct_ch_switch(_adapter *padapter, struct _ADAPTER_LINK *adapter_link,
			u8 *ra, u8 new_ch, u8 ch_offset);
void issue_addba_req(_adapter *adapter, unsigned char *ra, u8 tid);
void issue_addba_rsp(_adapter *adapter, unsigned char *ra, u8 tid, u16 status,
	             u8 size, struct ADDBA_request *paddba_req);
u8 issue_addba_rsp_wait_ack(_adapter *adapter, unsigned char *ra, u8 tid,
	                    u16 status, u8 size, struct ADDBA_request *paddba_req,
	                    int try_cnt, int wait_ms);
void issue_del_ba(_adapter *adapter, unsigned char *ra, u8 tid, u16 reason, u8 initiator);
int issue_del_ba_ex(_adapter *adapter, unsigned char *ra, u8 tid, u16 reason, u8 initiator, int try_cnt, int wait_ms);
void issue_action_BSSCoexistPacket(_adapter *padapter);

#ifdef CONFIG_IEEE80211W
void issue_action_SA_Query(_adapter *padapter, struct _ADAPTER_LINK *padapter_link,
			unsigned char *raddr, unsigned char action, unsigned short tid, u8 key_type);
int issue_deauth_11w(_adapter *padapter, unsigned char *da, unsigned short reason, u8 key_type);
#endif /* CONFIG_IEEE80211W */
int issue_action_SM_PS(_adapter *padapter ,  unsigned char *raddr , u8 NewMimoPsMode);
int issue_action_SM_PS_wait_ack(_adapter *padapter, unsigned char *raddr, u8 NewMimoPsMode, int try_cnt, int wait_ms);
#ifdef PRIVATE_R
int issue_action_find_remote(_adapter *padapter);
#endif

unsigned int send_delba_sta_tid(_adapter *adapter, u8 initiator, struct sta_info *sta, u8 tid, u8 force);
unsigned int send_delba_sta_tid_wait_ack(_adapter *adapter, u8 initiator, struct sta_info *sta, u8 tid, u8 force);

unsigned int send_delba(_adapter *padapter, u8 initiator, u8 *addr);
unsigned int send_beacon(_adapter *padapter);

void start_clnt_assoc(_adapter *padapterf);
void start_clnt_auth(_adapter *padapter);
void start_clnt_join(_adapter *padapter);
void start_create_ibss(_adapter *padapter);
#if defined(CONFIG_LAYER2_ROAMING) && defined(CONFIG_RTW_80211K)
void rtw_roam_nb_discover(_adapter *padapter, u8 bfroce);
#endif
void rtw_ie_handler(struct _ADAPTER *padapter, struct _ADAPTER_LINK *padapter_link, u8 *ie, u32 ie_len);
unsigned int OnAssocReq(_adapter *padapter, union recv_frame *precv_frame);
unsigned int OnAssocRsp(_adapter *padapter, union recv_frame *precv_frame);
unsigned int OnProbeReq(_adapter *padapter, union recv_frame *precv_frame);
unsigned int OnProbeRsp(_adapter *padapter, union recv_frame *precv_frame);
unsigned int DoReserved(_adapter *padapter, union recv_frame *precv_frame);
unsigned int OnBeacon(_adapter *padapter, union recv_frame *precv_frame);
unsigned int OnAtim(_adapter *padapter, union recv_frame *precv_frame);
unsigned int OnDisassoc(_adapter *padapter, union recv_frame *precv_frame);
unsigned int OnAuth(_adapter *padapter, union recv_frame *precv_frame);
unsigned int OnAuthClient(_adapter *padapter, union recv_frame *precv_frame);
unsigned int OnDeAuth(_adapter *padapter, union recv_frame *precv_frame);
unsigned int OnAction(_adapter *padapter, union recv_frame *precv_frame);

unsigned int on_action_spct(_adapter *padapter, union recv_frame *precv_frame);
unsigned int OnAction_qos(_adapter *padapter, union recv_frame *precv_frame);
unsigned int OnAction_dls(_adapter *padapter, union recv_frame *precv_frame);
#ifdef CONFIG_RTW_WNM
unsigned int on_action_wnm(_adapter *adapter, union recv_frame *rframe);
#endif

#define RX_AMPDU_ACCEPT_INVALID 0xFF
#define RX_AMPDU_SIZE_INVALID 0xFF

enum rx_ampdu_reason {
	RX_AMPDU_DRV_FIXED = 1,
	RX_AMPDU_BTCOEX = 2, /* not used, because BTCOEX has its own variable management */
	RX_AMPDU_DRV_SCAN = 3,
};
u8 rtw_rx_ampdu_size(_adapter *adapter);
bool rtw_rx_ampdu_is_accept(_adapter *adapter);
bool rtw_rx_ampdu_set_size(_adapter *adapter, u8 size, u8 reason);
bool rtw_rx_ampdu_set_accept(_adapter *adapter, u8 accept, u8 reason);
u8 rx_ampdu_apply_sta_tid(_adapter *adapter, struct sta_info *sta, u8 tid, u8 accept, u8 size);
u8 rx_ampdu_size_sta_limit(_adapter *adapter, struct sta_info *sta);
u8 rx_ampdu_apply_sta(_adapter *adapter, struct sta_info *sta, u8 accept, u8 size);
u16 rtw_rx_ampdu_apply(_adapter *adapter);

unsigned int OnAction_back(_adapter *padapter, union recv_frame *precv_frame);
unsigned int on_action_public(_adapter *padapter, union recv_frame *precv_frame);
unsigned int OnAction_ft(_adapter *padapter, union recv_frame *precv_frame);
unsigned int OnAction_ht(_adapter *padapter, union recv_frame *precv_frame);
#ifdef CONFIG_IEEE80211W
unsigned int OnAction_sa_query(_adapter *padapter, union recv_frame *precv_frame);
#endif /* CONFIG_IEEE80211W */
unsigned int on_action_rm(_adapter *padapter, union recv_frame *precv_frame);
unsigned int OnAction_wmm(_adapter *padapter, union recv_frame *precv_frame);
unsigned int OnAction_vht(_adapter *padapter, union recv_frame *precv_frame);
unsigned int OnAction_he(_adapter *padapter, union recv_frame *precv_frame);
unsigned int OnAction_protected_he(_adapter *padapter, union recv_frame *precv_frame);
unsigned int OnAction_p2p(_adapter *padapter, union recv_frame *precv_frame);
#ifdef CONFIG_RTW_TOKEN_BASED_XMIT
unsigned int OnAction_tbtx_token(_adapter *padapter, union recv_frame *precv_frame);
#endif

#ifdef CONFIG_RTW_TOKEN_BASED_XMIT
void rtw_issue_action_token_req(_adapter *padapter, struct sta_info *pstat);
void rtw_issue_action_token_rel(_adapter *padapter);
#endif

void rtw_mlmeext_disconnect(_adapter *padapter);
int rtw_set_hw_after_join(struct _ADAPTER *a, int join_res);
void mlmeext_sta_del_event_callback(_adapter *padapter);
void mlmeext_sta_add_event_callback(_adapter *padapter, struct sta_info *psta);

int rtw_get_rx_chk_limit(_adapter *adapter);
void rtw_set_rx_chk_limit(_adapter *adapter, int limit);
void linked_status_chk(_adapter *padapter, u8 from_timer);
void dynamic_update_bcn_check(_adapter *padapter);

#define rtw_get_bcn_cnt(adapter_link)	(adapter_link->mlmeextpriv.cur_bcn_cnt)
#define rtw_get_bcn_dtim_period(adapter_link)	(adapter_link->mlmeextpriv.dtim)
void rtw_collect_bcn_info(_adapter *adapter, struct _ADAPTER_LINK *adapter_link);

void _linked_info_dump(_adapter *padapter);

void link_timer_hdl(void *ctx);
void addba_timer_hdl(void *ctx);
#ifdef CONFIG_RTW_TOKEN_BASED_XMIT
void rtw_tbtx_xmit_timer_hdl(void *ctx);
void rtw_tbtx_token_dispatch_timer_hdl(void *ctx);
#endif
#ifdef CONFIG_IEEE80211W
void sa_query_timer_hdl(void *ctx);
#endif /* CONFIG_IEEE80211W */
#if 0
void reauth_timer_hdl(_adapter *padapter);
void reassoc_timer_hdl(_adapter *padapter);
#endif
#ifdef PRIVATE_R
void find_remote_timer_hdl(void *ctx);
#ifdef CONFIG_P2P
void hide_ssid_hdl(void *ctx);
#endif
#endif
#ifdef CONFIG_ECSA_PHL
void csa_wait_bcn_timer_hdl(void *ctx);
#endif

#define set_link_timer(mlmeext, ms) \
	do { \
		/*RTW_INFO("%s set_link_timer(%p, %d)\n", __FUNCTION__, (mlmeext), (ms));*/ \
		_set_timer(&(mlmeext)->link_timer, (ms)); \
	} while (0)
#define cancel_link_timer(mlmeext) \
	do { \
		/*RTW_INFO("%s cancel_link_timer(%p)\n", __FUNCTION__, (mlmeext));*/ \
		_cancel_timer_ex(&(mlmeext)->link_timer); \
	} while (0)
#ifdef PRIVATE_R
#define set_find_remote_timer(mlmeext, ms) \
	do { \
		/*RTW_INFO("%s set_find_remote_timer(%p, %d)\n", __FUNCTION__, (mlmeext), (ms));*/ \
		_set_timer(&(mlmeext)->find_remote_timer, (ms)); \
	} while (0)
#endif

bool rtw_is_cck_rate(u8 rate);
bool rtw_is_ofdm_rate(u8 rate);
bool rtw_is_basic_rate_cck(u8 rate);
bool rtw_is_basic_rate_ofdm(u8 rate);
bool rtw_is_basic_rate_mix(u8 rate);

extern int cckrates_included(unsigned char *rate, int ratelen);
extern int cckratesonly_included(unsigned char *rate, int ratelen);

extern void process_addba_req(_adapter *padapter, u8 *paddba_req, u8 *addr);

extern void update_TSF(struct link_mlme_ext_priv *pmlmeext, u8 *pframe, uint len);

#ifdef CONFIG_BCN_RECV_TIME
void rtw_rx_bcn_time_update(_adapter *adapter, struct _ADAPTER_LINK *adapter_link, uint bcn_len, u8 data_rate);
#endif
extern u8 traffic_status_watchdog(_adapter *padapter, u8 from_timer);

void rtw_process_bar_frame(_adapter *padapter, union recv_frame *precv_frame);
void rtw_join_done_chk_ch(_adapter *padapter, int join_res);

void survey_done_set_ch_bw(_adapter *padapter);

#ifdef RTW_BUSY_DENY_SCAN
#ifndef BUSY_TRAFFIC_SCAN_DENY_PERIOD
#ifdef CONFIG_RTW_ANDROID
#ifdef CONFIG_PLATFORM_ARM_SUN8I
	#define BUSY_TRAFFIC_SCAN_DENY_PERIOD	8000
#else
	#define BUSY_TRAFFIC_SCAN_DENY_PERIOD	12000
#endif
#else /* !CONFIG_ANDROID */
#define BUSY_TRAFFIC_SCAN_DENY_PERIOD	16000
#endif /* !CONFIG_ANDROID */
#endif /* !BUSY_TRAFFIC_SCAN_DENY_PERIOD */
#endif /* RTW_BUSY_DENY_SCAN */

void rtw_leave_opch(_adapter *adapter);
void rtw_back_opch(_adapter *adapter);

#ifdef CONFIG_STA_CMD_DISPR
void update_join_info(struct _ADAPTER *a, struct _WLAN_BSSID_EX *pbuf);
void update_join_link_info(struct _ADAPTER *a, struct _ADAPTER_LINK *alink, struct _WLAN_BSSID_EX *pbuf);
/*int rtw_chk_start_clnt_join(_adapter *adapter, u8 *ch, u8 *bw, u8 *offset);*/
#endif /* CONFIG_STA_CMD_DISPR */

u8 rtw_join_cmd_hdl(_adapter *padapter, u8 *pbuf);
u8 disconnect_hdl(_adapter *padapter, u8 *pbuf);
u8 createbss_hdl(_adapter *padapter, u8 *pbuf);
void rtw_disconnect_ch_switch(_adapter *adapter);
#ifdef CONFIG_AP_MODE
u8 stop_ap_hdl(_adapter *adapter);
#endif
#ifdef CONFIG_RTW_TOKEN_BASED_XMIT
u8 tx_control_hdl(_adapter *adapter);
#endif
u8 setopmode_hdl(_adapter *padapter, u8 *pbuf);

u8 setauth_hdl(_adapter *padapter, u8 *pbuf);
#ifdef CONFIG_CMD_DISP
u8 setkey_hdl(struct _ADAPTER *a, struct _ADAPTER_LINK *alink,
		struct setkey_parm *key, enum phl_cmd_type cmd_type,  u32 cmd_timeout);
u8 set_stakey_hdl(_adapter *padapter, struct set_stakey_parm *key,
		enum phl_cmd_type cmd_type,  u32 cmd_timeout);
#else
/* CONFIG_FSM */
u8 setkey_hdl(_adapter *padapter, u8 *pbuf);
u8 set_stakey_hdl(_adapter *padapter, u8 *pbuf);
#endif
u8 set_assocsta_hdl(_adapter *padapter, u8 *pbuf);
u8 del_assocsta_hdl(_adapter *padapter, u8 *pbuf);
u8 add_ba_hdl(_adapter *padapter, unsigned char *pbuf);
u8 add_ba_rsp_hdl(_adapter *padapter, unsigned char *pbuf);
u8 delba_hdl(struct _ADAPTER *a, unsigned char *pbuf);

void rtw_ap_wep_pk_setting(_adapter *adapter, struct sta_info *psta);

u8 mlme_evt_hdl(_adapter *padapter, unsigned char *pbuf);
u8 chk_bmc_sleepq_hdl(_adapter *padapter, unsigned char *pbuf);
u8 tx_beacon_hdl(_adapter *padapter, unsigned char *pbuf);
u8 rtw_set_chbw_hdl(_adapter *padapter, u8 *pbuf);
u8 led_blink_hdl(_adapter *padapter, unsigned char *pbuf);
u8 tdls_hdl(_adapter *padapter, unsigned char *pbuf);
u8 run_in_thread_hdl(_adapter *padapter, u8 *pbuf);


int rtw_sae_preprocess(_adapter *adapter, const u8 *buf, u32 len, u8 tx);

u32 rtw_desc_rate_to_bitrate(u8 bw, u16 rate_idx, u8 sgi);
u16 rtw_get_current_tx_rate(_adapter *padapter, struct sta_info *psta);
u8 rtw_get_current_tx_sgi(_adapter *padapter, struct sta_info *psta);
bool rtw_chk_phy_can_append_actrl(_adapter *padapter, struct sta_info *psta);
void rtw_get_current_rx_info(_adapter *adapter, struct sta_info *psta,
	u16 *rate, u8 *bw, u8 *gi_ltf);

#ifdef CONFIG_RTW_MESH
extern u8 rtw_mesh_set_plink_state_cmd_hdl(_adapter *adapter, u8 *parmbuf);
#else
static inline u8 rtw_mesh_set_plink_state_cmd_hdl(_adapter *adapter, u8 *parmbuf) { return H2C_CMD_FAIL; };
#endif /*CONFIG_RTW_MESH*/

#if defined(CONFIG_RTW_ACS) && defined(WKARD_ACS)
#define IS_ACS_ENABLE(dvobj)					(_FALSE)

enum rtw_nhm_pid {
	NHM_PID_IEEE_11K_HIGH,
	NHM_PID_ACS,
};
void rtw_acs_select_best_chan(_adapter *adapter);
void rtw_acs_trigger(_adapter *padapter, u32 scan_ms, u8 scan_ch, u8 pid);
void rtw_acs_get_rst(_adapter *adapter);
#endif /* defined(CONFIG_RTW_ACS) && defined(WKARD_ACS) */

#ifdef CONFIG_RTW_MULTI_AP
u8 rtw_get_ch_utilization(_adapter *adapter);
void rtw_ch_util_rpt(_adapter *adapter);
#endif

void rtw_set_mac_addr_hw(_adapter *adapter, u8 *mac_addr);

#define GEN_MLME_EXT_HANDLER(name, cmd, callback)	{name, cmd, callback},

struct rtw_cmd {
	char name[32];
	u8(*cmd_hdl)(_adapter *padapter, u8 *pbuf);
	void (*callback)(_adapter  *padapter, struct cmd_obj *cmd);
};

#ifdef _RTW_CMD_C_
struct rtw_cmd wlancmds[] = {
	GEN_MLME_EXT_HANDLER("CMD_JOINBSS", rtw_join_cmd_hdl, NULL) /*CMD_JOINBSS*/
	GEN_MLME_EXT_HANDLER("CMD_DISCONNECT", disconnect_hdl, rtw_disassoc_cmd_callback) /*CMD_DISCONNECT*/
	GEN_MLME_EXT_HANDLER("CMD_CREATE_BSS", createbss_hdl, NULL) /*CMD_CREATE_BSS*/
	GEN_MLME_EXT_HANDLER("CMD_SET_OPMODE", setopmode_hdl, NULL) /*CMD_SET_OPMODE*/
	#ifdef CONFIG_FSM
	GEN_MLME_EXT_HANDLER("CMD_SITE_SURVEY", sitesurvey_cmd_hdl, rtw_survey_cmd_callback) /*CMD_SITE_SURVEY*/
	#endif
	GEN_MLME_EXT_HANDLER("CMD_SET_AUTH", setauth_hdl, NULL) /*CMD_SET_AUTH*/
	#ifndef CONFIG_CMD_DISP
	GEN_MLME_EXT_HANDLER("CMD_SET_KEY", setkey_hdl, NULL) /*CMD_SET_KEY*/
	GEN_MLME_EXT_HANDLER("CMD_SET_STAKEY", set_stakey_hdl, rtw_setstaKey_cmdrsp_callback) /*CMD_SET_STAKEY*/
	#endif
	GEN_MLME_EXT_HANDLER("CMD_ADD_BAREQ", add_ba_hdl, NULL) /*CMD_ADD_BAREQ*/
	GEN_MLME_EXT_HANDLER("CMD_SET_CHANNEL", rtw_set_chbw_hdl, NULL) /*CMD_SET_CHANNEL*/
	GEN_MLME_EXT_HANDLER("CMD_TX_BEACON", tx_beacon_hdl, NULL) /*CMD_TX_BEACON*/
	GEN_MLME_EXT_HANDLER("CMD_SET_MLME_EVT", mlme_evt_hdl, NULL) /*CMD_SET_MLME_EVT*/
	GEN_MLME_EXT_HANDLER("CMD_SET_DRV_EXTRA", rtw_drvextra_cmd_hdl, NULL) /*CMD_SET_DRV_EXTRA*/
	GEN_MLME_EXT_HANDLER("CMD_SET_CHANPLAN", rtw_set_chplan_hdl, NULL) /*CMD_SET_CHANPLAN*/
	GEN_MLME_EXT_HANDLER("CMD_LEDBLINK", led_blink_hdl, NULL) /*CMD_LEDBLINK*/
	GEN_MLME_EXT_HANDLER("CMD_TDLS", tdls_hdl, NULL) /*CMD_TDLS*/
	GEN_MLME_EXT_HANDLER("CMD_CHK_BMCSLEEPQ", chk_bmc_sleepq_hdl, NULL) /*CMD_CHK_BMCSLEEPQ*/
	GEN_MLME_EXT_HANDLER("CMD_RUN_INTHREAD", run_in_thread_hdl, NULL) /*CMD_RUN_INTHREAD*/
	GEN_MLME_EXT_HANDLER("CMD_ADD_BARSP", add_ba_rsp_hdl, NULL) /*CMD_ADD_BARSP*/
	GEN_MLME_EXT_HANDLER("CMD_RM_POST_EVENT", rm_post_event_hdl, NULL) /*CMD_RM_POST_EVENT*/
	GEN_MLME_EXT_HANDLER("CMD_SET_MESH_PLINK_STATE", rtw_mesh_set_plink_state_cmd_hdl, NULL) /*CMD_SET_MESH_PLINK_STATE*/
	GEN_MLME_EXT_HANDLER("CMD_DELBA", delba_hdl, NULL) /*CMD_DELBA*/
	GEN_MLME_EXT_HANDLER("CMD_GET_CHANPLAN", rtw_get_chplan_hdl, NULL) /* CMD_GET_CHANPLAN */
};
#endif /*_RTW_CMD_C_*/
char *rtw_cmd_name(struct cmd_obj *pcmd);

/*rtw_event*/
struct rtw_evt_header {
	u8 id;
	u8 seq;
	u16 len;
};

enum rtw_event_id {
	EVT_SURVEY, /*0*/
	EVT_SURVEY_DONE, /*1*/
	EVT_JOINBSS, /*2*/
	EVT_ADD_STA, /*3*/
	EVT_DEL_STA, /*4*/
	EVT_WMM_UPDATE, /*5*/
#ifdef CONFIG_IEEE80211W
	EVT_TIMEOUT_STA, /*6*/
#endif /* CONFIG_IEEE80211W */
#ifdef CONFIG_RTW_80211R
	EVT_FT_REASSOC, /*7*/
#endif
	EVT_ID_MAX
};

struct rtw_event {
	char name[32];
	u32 parmsize;
	void (*event_callback)(_adapter *dev, u8 *pbuf);
};
#ifdef _RTW_MLME_EXT_C_
static struct rtw_event wlanevents[] = {
	{"EVT_SURVEY", sizeof(struct survey_event), &rtw_survey_event_callback}, /*EVT_SURVEY*/
	{"EVT_SURVEY_DONE", sizeof(struct surveydone_event), &rtw_surveydone_event_callback}, /*EVT_SURVEY_DONE*/
	{"EVT_JOINBSS", sizeof(struct joinbss_event), &rtw_joinbss_event_callback}, /*EVT_JOINBSS*/
	{"EVT_ADD_STA", sizeof(struct stassoc_event), &rtw_stassoc_event_callback}, /*EVT_ADD_STA*/
	{"EVT_DEL_STA", sizeof(struct stadel_event), &rtw_stadel_event_callback}, /*EVT_DEL_STA*/
	{"EVT_WMM_UPDATE", sizeof(struct wmm_event), &rtw_wmm_event_callback}, /*EVT_WMM_UPDATE*/
	#ifdef CONFIG_IEEE80211W
	{"EVT_TIMEOUT_STA", sizeof(struct stadel_event), &rtw_sta_timeout_event_callback}, /*EVT_TIMEOUT_STA*/
	#endif /* CONFIG_IEEE80211W */
	#ifdef CONFIG_RTW_80211R
	{"EVT_FT_REASSOC", sizeof(struct stassoc_event), &rtw_ft_reassoc_event_callback}, /*EVT_FT_REASSOC*/
	#endif
};
#endif/*_RTW_MLME_EXT_C_*/
char *rtw_evt_name(struct rtw_evt_header *pev);

#endif

/******************************************************************************
 *
 * Copyright(c) 2007 - 2022 Realtek Corporation.
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
#define _OS_INTFS_C_

#include <drv_types.h>

/* module param defaults */
int rtw_chip_version = 0x00;
int rtw_rfintfs = HWPI;
int rtw_lbkmode = 0;/* RTL8712_AIR_TRX; */
#ifdef DBG_LA_MODE
int rtw_la_mode_en=0;
module_param(rtw_la_mode_en, int, 0644);
#endif
int rtw_network_mode = Ndis802_11IBSS;/* Ndis802_11Infrastructure; */ /* infra, ad-hoc, auto */
/* NDIS_802_11_SSID	ssid; */
int rtw_band = BAND_ON_5G;
int rtw_channel = 36;/* ad-hoc support requirement */
int rtw_wireless_mode = WLAN_MD_MAX;
int rtw_band_type = BAND_CAP_2G | BAND_CAP_5G | BAND_CAP_6G;
module_param(rtw_wireless_mode, int, 0644);
module_param(rtw_band_type, int, 0644);
#ifdef CONFIG_HW_RTS
int rtw_vrtl_carrier_sense = ENABLE_VCS;
int rtw_vcs_type = RTS_CTS;
int rtw_hw_rts_en = 1;
#else
int rtw_vrtl_carrier_sense = AUTO_VCS;
int rtw_vcs_type = RTS_CTS;
int rtw_hw_rts_en = 0;
#endif
int rtw_rts_thresh = 2347;
int rtw_frag_thresh = 2346;
int rtw_preamble = PREAMBLE_LONG;/* long, short, auto */
int rtw_scan_mode = 1;/* active, passive */
#ifdef CONFIG_RTW_IPS
	int rtw_ips_mode = PS_OP_MODE_AUTO;
#ifdef CONFIG_FWIPS
	int rtw_ips_cap = PS_CAP_PWRON |
			PS_CAP_RF_OFF |
			PS_CAP_CLK_GATED |
			PS_CAP_PWR_GATED;
#endif /* CONFIG_FWIPS */
#else
	int rtw_ips_mode = PS_OP_MODE_DISABLED;
#endif /* CONFIG_RTW_IPS */
#ifdef CONFIG_RTW_LPS
	int rtw_lps_mode = PS_OP_MODE_AUTO;
	int rtw_lps_cap = PS_CAP_PWRON |
			PS_CAP_RF_OFF |
			PS_CAP_CLK_GATED |
			PS_CAP_PWR_GATED;
#else
	int rtw_lps_mode = PS_OP_MODE_DISABLED;
	int rtw_lps_cap = PS_CAP_PWRON;
#endif /* CONFIG_RTW_LPS */

#ifdef CONFIG_NARROWBAND_SUPPORTING
int rtw_nb_config = CONFIG_NB_VALUE;
module_param(rtw_nb_config, int, 0644);
MODULE_PARM_DESC(rtw_nb_config, "5M/10M/Normal bandwidth configuration");
#endif

int rtw_max_bss_cnt = 0;
module_param(rtw_max_bss_cnt, int, 0644);

#ifdef CONFIG_TX_EARLY_MODE
int rtw_early_mode = 1;
#endif

int rtw_usb_rxagg_mode = 2;/* RX_AGG_DMA=1, RX_AGG_USB=2 */
module_param(rtw_usb_rxagg_mode, int, 0644);

int rtw_dynamic_agg_enable = 1;
module_param(rtw_dynamic_agg_enable, int, 0644);

/* set log level when inserting driver module, default log level is _DRV_INFO_ = 4,
* please refer to "How_to_set_driver_debug_log_level.doc" to set the available level.
*/
#ifdef CONFIG_RTW_DEBUG
#ifdef RTW_LOG_LEVEL
	uint rtw_drv_log_level = (uint)RTW_LOG_LEVEL; /* from Makefile */
#else
	uint rtw_drv_log_level = _DRV_INFO_;
#endif
module_param(rtw_drv_log_level, uint, 0644);
MODULE_PARM_DESC(rtw_drv_log_level, "set log level when insert driver module, default log level is _DRV_INFO_ = 4");
#endif
int rtw_radio_enable = 1;
int rtw_long_retry_lmt = 7;
int rtw_short_retry_lmt = 7;
int rtw_busy_thresh = 40;
/* int qos_enable = 0; */ /* * */
int rtw_ack_policy = NORMAL_ACK;

int rtw_mp_mode = 0;

#if defined(CONFIG_MP_INCLUDED) && defined(CONFIG_RTW_CUSTOMER_STR)
uint rtw_mp_customer_str = 0;
module_param(rtw_mp_customer_str, uint, 0644);
MODULE_PARM_DESC(rtw_mp_customer_str, "Whether or not to enable customer str support on MP mode");
#endif

int rtw_software_encrypt = 0;
int rtw_software_decrypt = 0;

int rtw_acm_method = 0;/* 0:By SW 1:By HW. */

int rtw_wmm_enable = 1;/* default is set to enable the wmm. */

#if defined(CONFIG_RTL8821C) || defined(CONFIG_RTL8822B) || defined(CONFIG_RTL8822C)
	/*PHYDM API, must enable by default*/
	int rtw_pwrtrim_enable = 1;
#else
	int rtw_pwrtrim_enable = 0; /* Default Enalbe  power trim by efuse config */
#endif

uint rtw_tx_bw_mode = 0x331;
module_param(rtw_tx_bw_mode, uint, 0644);
MODULE_PARM_DESC(rtw_tx_bw_mode, "The max tx bw for 2G/5G/6G. format is the same as rtw_bw_mode");


#ifdef CONFIG_80211N_HT
int rtw_ht_enable = 1;
/* 0: 20 MHz, 1: 40 MHz, 2: 80 MHz, 3: 160MHz, 4: 80+80MHz
* 2.4G use bit 0 ~ 3, 5G use bit 4 ~ 7
* 6GHz use bits 8 ~ 11
* 0x21 means enable 2.4G 40MHz & 5G 80MHz */
#ifdef CONFIG_RTW_CUSTOMIZE_BWMODE
int rtw_bw_mode = CONFIG_RTW_CUSTOMIZE_BWMODE;
#else
int rtw_bw_mode = 0x331;
#endif

int rtw_ampdu_enable = 1;/* for enable tx_ampdu , */ /* 0: disable, 0x1:enable */

int rtw_rx_ampdu_amsdu = 2;/* 0: disabled, 1:enabled, 2:auto . There is an IOT issu with DLINK DIR-629 when the flag turn on */

/* 10.12 A-MSDU operation
 * HT -   0: 3839, 1: 7935  octets - Maximum A-MSDU Length
 * VHT - 0: 3895, 1: 7991, 2:11454  octets - Maximum MPDU Length
 */

#if defined(CONFIG_RTW_REDUCE_MEM) && defined(CONFIG_PCI_HCI)
int rtw_max_amsdu_len = MAX_ASMDU_LEN;
#else
int rtw_max_amsdu_len = 1;
#endif

module_param(rtw_max_amsdu_len, uint, 0644);

/*
* 2: Follow the AMSDU filed in ADDBA Resp. (Deault)
* 0: Force the AMSDU filed in ADDBA Resp. to be disabled.
* 1: Force the AMSDU filed in ADDBA Resp. to be enabled.
*/
int rtw_tx_ampdu_amsdu = 2;

/* 0: Default by halmac provided. */
/* other: Setting by driver. The final decision will take a samller value from halmac or driver. */
int rtw_tx_ampdu_num = 0;
module_param(rtw_tx_ampdu_num, int, 0644);

int rtw_quick_addba_req = 0;

static uint rtw_rx_ampdu_sz_limit_1ss[4] = CONFIG_RTW_RX_AMPDU_SZ_LIMIT_1SS;
static uint rtw_rx_ampdu_sz_limit_1ss_num = 0;
module_param_array(rtw_rx_ampdu_sz_limit_1ss, uint, &rtw_rx_ampdu_sz_limit_1ss_num, 0644);
MODULE_PARM_DESC(rtw_rx_ampdu_sz_limit_1ss, "RX AMPDU size limit for 1SS link of each BW, 0xFF: no limitation");

static uint rtw_rx_ampdu_sz_limit_2ss[4] = CONFIG_RTW_RX_AMPDU_SZ_LIMIT_2SS;
static uint rtw_rx_ampdu_sz_limit_2ss_num = 0;
module_param_array(rtw_rx_ampdu_sz_limit_2ss, uint, &rtw_rx_ampdu_sz_limit_2ss_num, 0644);
MODULE_PARM_DESC(rtw_rx_ampdu_sz_limit_2ss, "RX AMPDU size limit for 2SS link of each BW, 0xFF: no limitation");

static uint rtw_rx_ampdu_sz_limit_3ss[4] = CONFIG_RTW_RX_AMPDU_SZ_LIMIT_3SS;
static uint rtw_rx_ampdu_sz_limit_3ss_num = 0;
module_param_array(rtw_rx_ampdu_sz_limit_3ss, uint, &rtw_rx_ampdu_sz_limit_3ss_num, 0644);
MODULE_PARM_DESC(rtw_rx_ampdu_sz_limit_3ss, "RX AMPDU size limit for 3SS link of each BW, 0xFF: no limitation");

static uint rtw_rx_ampdu_sz_limit_4ss[4] = CONFIG_RTW_RX_AMPDU_SZ_LIMIT_4SS;
static uint rtw_rx_ampdu_sz_limit_4ss_num = 0;
module_param_array(rtw_rx_ampdu_sz_limit_4ss, uint, &rtw_rx_ampdu_sz_limit_4ss_num, 0644);
MODULE_PARM_DESC(rtw_rx_ampdu_sz_limit_4ss, "RX AMPDU size limit for 4SS link of each BW, 0xFF: no limitation");

/* Short GI support Bit Map
* BIT0 - 20MHz, 0: non-support, 1: support
* BIT1 - 40MHz, 0: non-support, 1: support
* BIT2 - 80MHz, 0: non-support, 1: support
* BIT3 - 160MHz, 0: non-support, 1: support */
int rtw_short_gi = 0xf;
/* BIT0: Enable VHT LDPC Rx, BIT1: Enable VHT LDPC Tx
 * BIT2: Enable HE LDPC Rx, BIT3: Enable HE LDPC Tx
 * BIT4: Enable HT LDPC Rx, BIT5: Enable HT LDPC Tx
 */
int rtw_ldpc_cap = 0x3f;
/* BIT0: Enable VHT STBC Rx, BIT1: Enable VHT STBC Tx
* BIT4: Enable HT STBC Rx, BIT5: Enable HT STBC Tx
* BIT8: Enable HE STBC Rx, BIT9: Enable HE STBC Rx(greater than 80M)
* BIT10: Enable HE STBC Tx, BIT11: Enable HE STBC Tx(greater than 80M)
*/
int rtw_stbc_cap = 0x133;
#endif /* CONFIG_80211N_HT */

#ifdef CONFIG_BEAMFORMING
/*
* BIT0: Enable VHT SU Beamformer
* BIT1: Enable VHT SU Beamformee
* BIT2: Enable VHT MU Beamformer, depend on VHT SU Beamformer
* BIT3: Enable VHT MU Beamformee, depend on VHT SU Beamformee
* BIT4: Enable HT Beamformer
* BIT5: Enable HT Beamformee
* BIT6: Enable HE SU Beamformer
* BIT7: Enable HE SU Beamformee
* BIT8: Enable HE MU Beamformer
* BIT9: Enable HE MU Beamformee
*/
int rtw_beamform_cap = BIT(1) | BIT(7);  /* For sw role BF cap. */
int rtw_sw_proto_bf_cap_phy0 = BIT(1) | BIT(7);
int rtw_sw_proto_bf_cap_phy1 = BIT(1) | BIT(7);
int rtw_dyn_txbf = 1;
int rtw_bfer_rf_number = 0; /*BeamformerCapRfNum Rf path number, 0 for auto, others for manual*/
int rtw_bfee_rf_number = 0; /*BeamformeeCapRfNum  Rf path number, 0 for auto, others for manual*/
#endif

#ifdef CONFIG_80211AC_VHT
int rtw_vht_enable = 1; /* 0:disable, 1:enable, 2:force auto enable */
module_param(rtw_vht_enable, int, 0644);

int rtw_vht_24g_enable = 0; /* 0:disable, 1:enable */
module_param(rtw_vht_24g_enable, int, 0644);

int rtw_ampdu_factor = 7;

uint rtw_vht_rx_mcs_map = 0xaaaa;
module_param(rtw_vht_rx_mcs_map, uint, 0644);
MODULE_PARM_DESC(rtw_vht_rx_mcs_map, "VHT RX MCS map");
#endif /* CONFIG_80211AC_VHT */

#ifdef CONFIG_80211AX_HE
int rtw_he_enable = 1; /* 0:disable, 1:enable, 2:force auto enable */
module_param(rtw_he_enable, int, 0644);
#endif

int rtw_lowrate_two_xmit = 1;/* Use 2 path Tx to transmit MCS0~7 and legacy mode */


/* 0: not check in watch dog, 1: check in watch dog  */
int rtw_check_hw_status = 0;

int rtw_low_power = 0;
int rtw_wifi_spec = 0;

#ifdef CONFIG_SPECIAL_RF_PATH /* configure Nss/xTxR IC to 1ss/1T1R */
int rtw_rf_path = RF_1T1R;
int rtw_tx_nss = 1;
int rtw_rx_nss = 1;
#else
int rtw_rf_path = RF_TYPE_MAX;
int rtw_tx_nss = 0;
int rtw_rx_nss = 0;
#endif
module_param(rtw_rf_path, int, 0644);
module_param(rtw_tx_nss, int, 0644);
module_param(rtw_rx_nss, int, 0644);

#ifdef CONFIG_REGD_SRC_FROM_OS
static uint rtw_regd_src = CONFIG_RTW_REGD_SRC;
module_param(rtw_regd_src, uint, 0644);
MODULE_PARM_DESC(rtw_regd_src, "The default regd source selection, 0:RTK_PRIV, 1:OS");
#endif

uint rtw_init_regd_always_apply = CONFIG_RTW_INIT_REGD_ALWAYS_APPLY;
module_param(rtw_init_regd_always_apply, uint, 0644);
MODULE_PARM_DESC(rtw_init_regd_always_apply, "Whether INIT regd request is always applied"
	" (being included when taking intersection together with higher priority requests)"
	" when regd source is RTK_PRIV");

uint rtw_user_regd_always_apply = CONFIG_RTW_USER_REGD_ALWAYS_APPLY;
module_param(rtw_user_regd_always_apply, uint, 0644);
MODULE_PARM_DESC(rtw_user_regd_always_apply, "Whether USER regd request is always applied"
	" (being included when taking intersection together with higher priority requests)"
	" when regd source is RTK_PRIV");

char *rtw_country_code = CONFIG_RTW_COUNTRY_CODE;
module_param(rtw_country_code, charp, 0644);
MODULE_PARM_DESC(rtw_country_code, "The default country code (in alpha2)");

uint rtw_channel_plan = CONFIG_RTW_CHPLAN;
module_param(rtw_channel_plan, uint, 0644);
MODULE_PARM_DESC(rtw_channel_plan, "The default chplan ID when rtw_alpha2 is not specified or valid");

static uint rtw_excl_chs[MAX_CHANNEL_NUM_2G_5G] = CONFIG_RTW_EXCL_CHS;
static int rtw_excl_chs_num = 0;
module_param_array(rtw_excl_chs, uint, &rtw_excl_chs_num, 0644);
MODULE_PARM_DESC(rtw_excl_chs, "Exclusive channel list of 2G and 5G band");

#if CONFIG_IEEE80211_BAND_6GHZ
uint rtw_channel_plan_6g = CONFIG_RTW_CHPLAN_6G;
module_param(rtw_channel_plan_6g, uint, 0644);
MODULE_PARM_DESC(rtw_channel_plan_6g, "The default chplan_6g ID when rtw_alpha2 is not specified or valid");

static uint rtw_excl_chs_6g[MAX_CHANNEL_NUM_6G] = CONFIG_RTW_EXCL_CHS_6G;
static int rtw_excl_chs_6g_num = 0;
module_param_array(rtw_excl_chs_6g, uint, &rtw_excl_chs_6g_num, 0644);
MODULE_PARM_DESC(rtw_excl_chs_6g, "Exclusive channel list of 6G band");
#endif /* CONFIG_IEEE80211_BAND_6GHZ */

char *rtw_dis_ch_flags = CONFIG_RTW_DIS_CH_FLAGS;
module_param(rtw_dis_ch_flags, charp, 0644);
MODULE_PARM_DESC(rtw_dis_ch_flags, "The flags with which channel is to be disabled");

static uint rtw_bcn_hint_valid_ms = CONFIG_RTW_BCN_HINT_VALID_MS;
module_param(rtw_bcn_hint_valid_ms, uint, 0644);
MODULE_PARM_DESC(rtw_bcn_hint_valid_ms, "The length of time beacon hint continue");

#ifdef CONFIG_80211D
static uint rtw_country_ie_slave_en_mode = CONFIG_RTW_COUNTRY_IE_SLAVE_EN_MODE;
module_param(rtw_country_ie_slave_en_mode, uint, 0644);
MODULE_PARM_DESC(rtw_country_ie_slave_en_mode, "802.11d country IE slave enable mode:"
	" 0: disable, 1: enable, 2: enable when INIT/USER set world wide mode");

static uint rtw_country_ie_slave_flags = CONFIG_RTW_COUNTRY_IE_SLAVE_FLAGS;
module_param(rtw_country_ie_slave_flags, uint, 0644);
MODULE_PARM_DESC(rtw_country_ie_slave_flags, "802.11d country IE slave flags:"
	" BIT0: take intersection when having multiple received IEs, otherwise choose effected one from received IEs"
	", BIT1: consider all environment BSSs, otherwise associated BSSs only");

static uint rtw_country_ie_slave_en_role = CONFIG_RTW_COUNTRY_IE_SLAVE_EN_ROLE;
module_param(rtw_country_ie_slave_en_role, uint, 0644);
MODULE_PARM_DESC(rtw_country_ie_slave_en_role, "802.11d country IE slave enable role: BIT0:pure STA mode, BIT1:P2P group client");

static uint rtw_country_ie_slave_en_ifbmp = CONFIG_RTW_COUNTRY_IE_SLAVE_EN_IFBMP;
module_param(rtw_country_ie_slave_en_ifbmp, uint, 0644);
MODULE_PARM_DESC(rtw_country_ie_slave_en_ifbmp, "802.11d country IE slave enable iface bitmap");

static uint rtw_country_ie_slave_scan_int_ms = CONFIG_RTW_COUNTRY_IE_SLAVE_SCAN_INT_MS;
module_param(rtw_country_ie_slave_scan_int_ms, uint, 0644);
MODULE_PARM_DESC(rtw_country_ie_slave_scan_int_ms, "802.11d country IE slave auto scan interval in ms to find environment BSSs."
	" 0: no environment BSS auto scan triggered by driver self");
#endif

/*if concurrent softap + p2p(GO) is needed, this param lets p2p response full channel list.
But Softap must be SHUT DOWN once P2P decide to set up connection and become a GO.*/
#ifdef CONFIG_FULL_CH_IN_P2P_HANDSHAKE
	int rtw_full_ch_in_p2p_handshake = 1; /* reply full channel list*/
#else
	int rtw_full_ch_in_p2p_handshake = 0; /* reply only softap channel*/
#endif

#ifdef CONFIG_BTC
int rtw_btcoex_enable = 2;
module_param(rtw_btcoex_enable, int, 0644);
MODULE_PARM_DESC(rtw_btcoex_enable, "BT co-existence on/off, 0:off, 1:on, 2:by efuse");

int rtw_ant_num = 0;
module_param(rtw_ant_num, int, 0644);
MODULE_PARM_DESC(rtw_ant_num, "Antenna number setting, 0:by efuse");

int rtw_bt_iso = 2;/* 0:Low, 1:High, 2:From Efuse */
int rtw_bt_sco = 3;/* 0:Idle, 1:None-SCO, 2:SCO, 3:From Counter, 4.Busy, 5.OtherBusy */
int rtw_bt_ampdu = 1 ; /* 0:Disable BT control A-MPDU, 1:Enable BT control A-MPDU. */
#endif /* CONFIG_BTC */

int rtw_AcceptAddbaReq = _TRUE;/* 0:Reject AP's Add BA req, 1:Accept AP's Add BA req. */

int rtw_antdiv_cfg = 2; /* 0:OFF , 1:ON, 2:decide by Efuse config */
int rtw_antdiv_type = 0; /* 0:decide by efuse  1: for 88EE, 1Tx and 1RxCG are diversity.(2 Ant with SPDT), 2:  for 88EE, 1Tx and 2Rx are diversity.( 2 Ant, Tx and RxCG are both on aux port, RxCS is on main port ), 3: for 88EE, 1Tx and 1RxCG are fixed.(1Ant, Tx and RxCG are both on aux port) */

int rtw_drv_ant_band_switch = 1; /* 0:OFF , 1:ON, Driver control antenna band switch*/

int rtw_single_ant_path; /*0:main ant , 1:aux ant , Fixed single antenna path, default main ant*/

/* 0: doesn't switch, 1: switch to usb 3.0 , 2: switch to usb 2.0 */
int rtw_switch_usb_mode = 0;



#ifdef CONFIG_HW_PWRP_DETECTION
int rtw_hwpwrp_detect = 1;
#else
int rtw_hwpwrp_detect = 0; /* HW power  ping detect 0:disable , 1:enable */
#endif

#ifdef CONFIG_USB_HCI
int rtw_hw_wps_pbc = 1;
#else
int rtw_hw_wps_pbc = 0;
#endif

#ifdef CONFIG_PCI_ASPM
/* CLK_REQ:BIT0 L0s:BIT1 ASPM_L1:BIT2 L1Off:BIT3*/
int	rtw_pci_aspm_enable = 0x5;
#else
int	rtw_pci_aspm_enable;
#endif

/*
 * BIT [15:12] mask of ps mode
 * BIT [11:8] val of ps mode
 * BIT [7:4] mask of perf mode
 * BIT [3:0] val of perf mode
 *
 * L0s:BIT[+0] L1:BIT[+1]
 *
 * 0x0030: change value only if perf mode
 * 0x3300: change value only if ps mode
 * 0x3330: change value in both perf and ps mode
 */
#ifdef CONFIG_PCI_DYNAMIC_ASPM
#ifdef CONFIG_PCI_ASPM
int rtw_pci_dynamic_aspm_linkctrl = 0x3330;
#else
int rtw_pci_dynamic_aspm_linkctrl = 0x0030;
#endif
#else
int rtw_pci_dynamic_aspm_linkctrl = 0x0000;
#endif
module_param(rtw_pci_dynamic_aspm_linkctrl, int, 0644);

#ifdef CONFIG_QOS_OPTIMIZATION
int rtw_qos_opt_enable = 1; /* 0: disable,1:enable */
#else
int rtw_qos_opt_enable = 0; /* 0: disable,1:enable */
#endif
module_param(rtw_qos_opt_enable, int, 0644);

#ifdef CONFIG_RTW_ACS
int rtw_acs_auto_scan = 0; /*0:disable, 1:enable*/
module_param(rtw_acs_auto_scan, int, 0644);

int rtw_acs = 1;
module_param(rtw_acs, int, 0644);
#endif

char *ifname = "wlan%d";
module_param(ifname, charp, 0644);
MODULE_PARM_DESC(ifname, "The default name to allocate for first interface");

#ifdef CONFIG_PLATFORM_ANDROID
	char *if2name = "p2p%d";
#else /* CONFIG_PLATFORM_ANDROID */
	char *if2name = "wlan%d";
#endif /* CONFIG_PLATFORM_ANDROID */
module_param(if2name, charp, 0644);
MODULE_PARM_DESC(if2name, "The default name to allocate for second interface");

#if defined(CONFIG_PLATFORM_ANDROID) && (CONFIG_IFACE_NUMBER > 2)
	char *if3name = "ap%d";
module_param(if3name, charp, 0644);
MODULE_PARM_DESC(if3name, "The default name to allocate for third interface");
#endif

char *rtw_initmac = 0;  /* temp mac address if users want to use instead of the mac address in Efuse */

#ifdef CONFIG_CONCURRENT_MODE

	#if (CONFIG_IFACE_NUMBER > 2)
		int rtw_virtual_iface_num = CONFIG_IFACE_NUMBER - 1;
		module_param(rtw_virtual_iface_num, int, 0644);
	#else
		int rtw_virtual_iface_num = 1;
	#endif

#if defined(CONFIG_CONCURRENT_MODE) && !RTW_P2P_GROUP_INTERFACE
#ifdef CONFIG_P2P
	#ifdef CONFIG_SEL_P2P_IFACE
	int rtw_sel_p2p_iface = CONFIG_SEL_P2P_IFACE;
	#else
	int rtw_sel_p2p_iface = (CONFIG_RTW_STATIC_NDEV_NUM - 1);
	#endif

	module_param(rtw_sel_p2p_iface, int, 0644);

#endif
#endif

#ifdef CONFIG_IGNORE_GO_AND_LOW_RSSI_IN_SCAN_LIST
	int rtw_ignore_go_in_scan = 1;
	uint rtw_ignore_low_rssi_in_scan = 30;

	module_param(rtw_ignore_go_in_scan, int, 0644);
	module_param(rtw_ignore_low_rssi_in_scan, uint, 0644);
#endif /*CONFIG_IGNORE_GO_AND_LOW_RSSI_IN_SCAN_LIST*/

#endif

/* affect ap/go cw only so far , 0 is no change*/
uint rtw_vo_edca = 0;
module_param(rtw_vo_edca, uint, 0644);

#ifdef CONFIG_AP_MODE
u8 rtw_bmc_tx_rate = MGN_UNKNOWN;

#if CONFIG_RTW_AP_DATA_BMC_TO_UC
int rtw_ap_src_b2u_flags = CONFIG_RTW_AP_SRC_B2U_FLAGS;
module_param(rtw_ap_src_b2u_flags, int, 0644);

int rtw_ap_fwd_b2u_flags = CONFIG_RTW_AP_FWD_B2U_FLAGS;
module_param(rtw_ap_fwd_b2u_flags, int, 0644);
#endif /* CONFIG_RTW_AP_DATA_BMC_TO_UC */
#endif /* CONFIG_AP_MODE */

#ifdef CONFIG_RTW_MESH
#if CONFIG_RTW_MESH_DATA_BMC_TO_UC
int rtw_msrc_b2u_flags = CONFIG_RTW_MSRC_B2U_FLAGS;
module_param(rtw_msrc_b2u_flags, int, 0644);

int rtw_mfwd_b2u_flags = CONFIG_RTW_MFWD_B2U_FLAGS;
module_param(rtw_mfwd_b2u_flags, int, 0644);
#endif /* CONFIG_RTW_MESH_DATA_BMC_TO_UC */
#endif /* CONFIG_RTW_MESH */

#ifdef RTW_WOW_STA_MIX
int rtw_wowlan_sta_mix_mode = 1;
#else
int rtw_wowlan_sta_mix_mode = 0;
#endif
module_param(rtw_wowlan_sta_mix_mode, int, 0644);
module_param(rtw_pwrtrim_enable, int, 0644);
module_param(rtw_initmac, charp, 0644);
module_param(rtw_chip_version, int, 0644);
module_param(rtw_rfintfs, int, 0644);
module_param(rtw_lbkmode, int, 0644);
module_param(rtw_network_mode, int, 0644);
module_param(rtw_band, int, 0644);
module_param(rtw_channel, int, 0644);
module_param(rtw_mp_mode, int, 0644);
module_param(rtw_wmm_enable, int, 0644);
module_param(rtw_vrtl_carrier_sense, int, 0644);
module_param(rtw_vcs_type, int, 0644);
module_param(rtw_hw_rts_en, int, 0644);
module_param(rtw_busy_thresh, int, 0644);

#ifdef CONFIG_80211N_HT
module_param(rtw_ht_enable, int, 0644);
module_param(rtw_bw_mode, int, 0644);
module_param(rtw_ampdu_enable, int, 0644);
module_param(rtw_stbc_cap, int, 0644);
module_param(rtw_rx_ampdu_amsdu, int, 0644);
module_param(rtw_tx_ampdu_amsdu, int, 0644);
module_param(rtw_quick_addba_req, int, 0644);
#endif /* CONFIG_80211N_HT */

#ifdef CONFIG_BEAMFORMING
module_param(rtw_beamform_cap, int, 0644);
module_param(rtw_sw_proto_bf_cap_phy0, int, 0644);
module_param(rtw_sw_proto_bf_cap_phy1, int, 0644);
module_param(rtw_dyn_txbf, int, 0644);
#endif
module_param(rtw_lowrate_two_xmit, int, 0644);

module_param(rtw_low_power, int, 0644);
module_param(rtw_wifi_spec, int, 0644);

module_param(rtw_full_ch_in_p2p_handshake, int, 0644);
module_param(rtw_antdiv_cfg, int, 0644);
module_param(rtw_antdiv_type, int, 0644);

module_param(rtw_drv_ant_band_switch, int, 0644);
module_param(rtw_single_ant_path, int, 0644);

module_param(rtw_switch_usb_mode, int, 0644);

module_param(rtw_hwpwrp_detect, int, 0644);

module_param(rtw_hw_wps_pbc, int, 0644);
module_param(rtw_check_hw_status, int, 0644);

#if defined(CONFIG_PCI_HCI) && defined(CONFIG_PCI_ASPM)
module_param(rtw_pci_aspm_enable, int, 0644);
#endif

#ifdef CONFIG_TX_EARLY_MODE
module_param(rtw_early_mode, int, 0644);
#endif
#ifdef CONFIG_ADAPTOR_INFO_CACHING_FILE
char *rtw_adaptor_info_caching_file_path = "/data/misc/wifi/rtw_cache";
module_param(rtw_adaptor_info_caching_file_path, charp, 0644);
MODULE_PARM_DESC(rtw_adaptor_info_caching_file_path, "The path of adapter info cache file");
#endif /* CONFIG_ADAPTOR_INFO_CACHING_FILE */

#ifdef CONFIG_LAYER2_ROAMING
uint rtw_max_roaming_times = 2;
module_param(rtw_max_roaming_times, uint, 0644);
MODULE_PARM_DESC(rtw_max_roaming_times, "The max roaming times to try");
#endif /* CONFIG_LAYER2_ROAMING */

#ifdef CONFIG_FILE_FWIMG
char *rtw_fw_file_path = CONFIG_FIRMWARE_PATH; /* "/system/etc/firmware/rtlwifi/FW_NIC.BIN"; */

module_param(rtw_fw_file_path, charp, 0644);
MODULE_PARM_DESC(rtw_fw_file_path, "The path of fw image");

char *rtw_fw_wow_file_path = "/system/etc/firmware/rtlwifi/FW_WoWLAN.BIN";
module_param(rtw_fw_wow_file_path, charp, 0644);
MODULE_PARM_DESC(rtw_fw_wow_file_path, "The path of fw for Wake on Wireless image");

#ifdef CONFIG_MP_INCLUDED
char *rtw_fw_mp_bt_file_path = "";
module_param(rtw_fw_mp_bt_file_path, charp, 0644);
MODULE_PARM_DESC(rtw_fw_mp_bt_file_path, "The path of fw for MP-BT image");
#endif /* CONFIG_MP_INCLUDED */
#endif /* CONFIG_FILE_FWIMG */

uint rtw_hiq_filter = CONFIG_RTW_HIQ_FILTER;
module_param(rtw_hiq_filter, uint, 0644);
MODULE_PARM_DESC(rtw_hiq_filter, "0:allow all, 1:allow special, 2:deny all");

uint rtw_adaptivity_en = CONFIG_RTW_ADAPTIVITY_EN;
module_param(rtw_adaptivity_en, uint, 0644);
MODULE_PARM_DESC(rtw_adaptivity_en, "0:disable, 1:enable, 2:auto");

uint rtw_adaptivity_mode = CONFIG_RTW_ADAPTIVITY_MODE;
module_param(rtw_adaptivity_mode, uint, 0644);
MODULE_PARM_DESC(rtw_adaptivity_mode, "0:normal, 1:carrier sense");

int rtw_adaptivity_th_l2h_ini = CONFIG_RTW_ADAPTIVITY_TH_L2H_INI;
module_param(rtw_adaptivity_th_l2h_ini, int, 0644);
MODULE_PARM_DESC(rtw_adaptivity_th_l2h_ini, "th_l2h_ini for Adaptivity");

int rtw_adaptivity_th_edcca_hl_diff = CONFIG_RTW_ADAPTIVITY_TH_EDCCA_HL_DIFF;
module_param(rtw_adaptivity_th_edcca_hl_diff, int, 0644);
MODULE_PARM_DESC(rtw_adaptivity_th_edcca_hl_diff, "th_edcca_hl_diff for Adaptivity");


int rtw_adaptivity_idle_probability = 0;
module_param(rtw_adaptivity_idle_probability, int, 0644);
MODULE_PARM_DESC(rtw_adaptivity_idle_probability, "rtw_adaptivity_idle_probability");

#ifdef CONFIG_DFS_MASTER
uint rtw_dfs_region_domain = CONFIG_RTW_DFS_REGION_DOMAIN;
module_param(rtw_dfs_region_domain, uint, 0644);
MODULE_PARM_DESC(rtw_dfs_region_domain, "0:NONE, 1:FCC, 2:MKK, 3:ETSI, 4:KCC");
#endif

uint rtw_amsdu_mode = RTW_AMSDU_MODE_NON_SPP;
module_param(rtw_amsdu_mode, uint, 0644);
MODULE_PARM_DESC(rtw_amsdu_mode, "0:non-spp, 1:spp, 2:all drop");

uint rtw_amplifier_type_2g = CONFIG_RTW_AMPLIFIER_TYPE_2G;
module_param(rtw_amplifier_type_2g, uint, 0644);
MODULE_PARM_DESC(rtw_amplifier_type_2g, "BIT3:2G ext-PA, BIT4:2G ext-LNA");

uint rtw_amplifier_type_5g = CONFIG_RTW_AMPLIFIER_TYPE_5G;
module_param(rtw_amplifier_type_5g, uint, 0644);
MODULE_PARM_DESC(rtw_amplifier_type_5g, "BIT6:5G ext-PA, BIT7:5G ext-LNA");

uint rtw_rfe_type = CONFIG_RTW_RFE_TYPE;
module_param(rtw_rfe_type, uint, 0644);
MODULE_PARM_DESC(rtw_rfe_type, "default init value:64");

#ifdef CONFIG_DBCC_SUPPORT
/*0:disable ,1: enable*/
int rtw_dbcc_en = 1;
module_param(rtw_dbcc_en, int, 0644);
MODULE_PARM_DESC(rtw_dbcc_en, "0:Disable, 1:Enable DBCC");

int rtw_dbcc_itf_ctl = 0;
module_param(rtw_dbcc_itf_ctl, int, 0644);
MODULE_PARM_DESC(rtw_dbcc_itf_ctl, "0:can't control interference, 1:can control interference");

#ifdef CONFIG_DBCC_FORCE
/* rmap - bitmap of wrole(idx) - force roles (idx) operate in band1*/
int rtw_dbcc_force_rmap = 0;
module_param(rtw_dbcc_force_rmap, int, 0644);
MODULE_PARM_DESC(rtw_dbcc_force_rmap, "Band-1's Role idx");

int rtw_dbcc_force_cck_phyidx = 0;
module_param(rtw_dbcc_force_cck_phyidx, int, 0644);
MODULE_PARM_DESC(rtw_dbcc_force_cck_phyidx, "cck phy-idx");
#endif /*CONFIG_DBCC_FORCE*/
#endif /*CONFIG_DBCC_SUPPORT*/

uint rtw_powertracking_type = 64;
module_param(rtw_powertracking_type, uint, 0644);
MODULE_PARM_DESC(rtw_powertracking_type, "default init value:64");

uint rtw_GLNA_type = CONFIG_RTW_GLNA_TYPE;
module_param(rtw_GLNA_type, uint, 0644);
MODULE_PARM_DESC(rtw_GLNA_type, "default init value:0");

uint rtw_TxBBSwing_2G = 0xFF;
module_param(rtw_TxBBSwing_2G, uint, 0644);
MODULE_PARM_DESC(rtw_TxBBSwing_2G, "default init value:0xFF");

uint rtw_TxBBSwing_5G = 0xFF;
module_param(rtw_TxBBSwing_5G, uint, 0644);
MODULE_PARM_DESC(rtw_TxBBSwing_5G, "default init value:0xFF");

uint rtw_OffEfuseMask = 0;
module_param(rtw_OffEfuseMask, uint, 0644);
MODULE_PARM_DESC(rtw_OffEfuseMask, "default open Efuse Mask value:0");

uint rtw_FileMaskEfuse = 0;
module_param(rtw_FileMaskEfuse, uint, 0644);
MODULE_PARM_DESC(rtw_FileMaskEfuse, "default drv Mask Efuse value:0");

uint rtw_rxgain_offset_2g = 0;
module_param(rtw_rxgain_offset_2g, uint, 0644);
MODULE_PARM_DESC(rtw_rxgain_offset_2g, "default RF Gain 2G Offset value:0");

uint rtw_rxgain_offset_5gl = 0;
module_param(rtw_rxgain_offset_5gl, uint, 0644);
MODULE_PARM_DESC(rtw_rxgain_offset_5gl, "default RF Gain 5GL Offset value:0");

uint rtw_rxgain_offset_5gm = 0;
module_param(rtw_rxgain_offset_5gm, uint, 0644);
MODULE_PARM_DESC(rtw_rxgain_offset_5gm, "default RF Gain 5GM Offset value:0");

uint rtw_rxgain_offset_5gh = 0;
module_param(rtw_rxgain_offset_5gh, uint, 0644);
MODULE_PARM_DESC(rtw_rxgain_offset_5gm, "default RF Gain 5GL Offset value:0");

uint rtw_pll_ref_clk_sel = CONFIG_RTW_PLL_REF_CLK_SEL;
module_param(rtw_pll_ref_clk_sel, uint, 0644);
MODULE_PARM_DESC(rtw_pll_ref_clk_sel, "force pll_ref_clk_sel, 0xF:use autoload value");

int rtw_tx_pwr_by_rate = CONFIG_TXPWR_BY_RATE_EN;
module_param(rtw_tx_pwr_by_rate, int, 0644);
MODULE_PARM_DESC(rtw_tx_pwr_by_rate, "0:Disable, 1:Enable, 2: Depend on efuse");

#if CONFIG_TXPWR_LIMIT
int rtw_tx_pwr_lmt_enable = CONFIG_TXPWR_LIMIT_EN;
module_param(rtw_tx_pwr_lmt_enable, int, 0644);
MODULE_PARM_DESC(rtw_tx_pwr_lmt_enable, "0:Disable, 1:Enable, 2: Depend on efuse");
#endif

static int rtw_target_tx_pwr_2g_a[RATE_SECTION_NUM] = CONFIG_RTW_TARGET_TX_PWR_2G_A;
static int rtw_target_tx_pwr_2g_a_num = 0;
module_param_array(rtw_target_tx_pwr_2g_a, int, &rtw_target_tx_pwr_2g_a_num, 0644);
MODULE_PARM_DESC(rtw_target_tx_pwr_2g_a, "2.4G target tx power (unit:dBm) of RF path A for each rate section, should match the real calibrate power, -1: undefined");

static int rtw_target_tx_pwr_2g_b[RATE_SECTION_NUM] = CONFIG_RTW_TARGET_TX_PWR_2G_B;
static int rtw_target_tx_pwr_2g_b_num = 0;
module_param_array(rtw_target_tx_pwr_2g_b, int, &rtw_target_tx_pwr_2g_b_num, 0644);
MODULE_PARM_DESC(rtw_target_tx_pwr_2g_b, "2.4G target tx power (unit:dBm) of RF path B for each rate section, should match the real calibrate power, -1: undefined");

static int rtw_target_tx_pwr_2g_c[RATE_SECTION_NUM] = CONFIG_RTW_TARGET_TX_PWR_2G_C;
static int rtw_target_tx_pwr_2g_c_num = 0;
module_param_array(rtw_target_tx_pwr_2g_c, int, &rtw_target_tx_pwr_2g_c_num, 0644);
MODULE_PARM_DESC(rtw_target_tx_pwr_2g_c, "2.4G target tx power (unit:dBm) of RF path C for each rate section, should match the real calibrate power, -1: undefined");

static int rtw_target_tx_pwr_2g_d[RATE_SECTION_NUM] = CONFIG_RTW_TARGET_TX_PWR_2G_D;
static int rtw_target_tx_pwr_2g_d_num = 0;
module_param_array(rtw_target_tx_pwr_2g_d, int, &rtw_target_tx_pwr_2g_d_num, 0644);
MODULE_PARM_DESC(rtw_target_tx_pwr_2g_d, "2.4G target tx power (unit:dBm) of RF path D for each rate section, should match the real calibrate power, -1: undefined");

#if CONFIG_IEEE80211_BAND_5GHZ
static int rtw_target_tx_pwr_5g_a[RATE_SECTION_NUM - 1] = CONFIG_RTW_TARGET_TX_PWR_5G_A;
static int rtw_target_tx_pwr_5g_a_num = 0;
module_param_array(rtw_target_tx_pwr_5g_a, int, &rtw_target_tx_pwr_5g_a_num, 0644);
MODULE_PARM_DESC(rtw_target_tx_pwr_5g_a, "5G target tx power (unit:dBm) of RF path A for each rate section, should match the real calibrate power, -1: undefined");

static int rtw_target_tx_pwr_5g_b[RATE_SECTION_NUM - 1] = CONFIG_RTW_TARGET_TX_PWR_5G_B;
static int rtw_target_tx_pwr_5g_b_num = 0;
module_param_array(rtw_target_tx_pwr_5g_b, int, &rtw_target_tx_pwr_5g_b_num, 0644);
MODULE_PARM_DESC(rtw_target_tx_pwr_5g_b, "5G target tx power (unit:dBm) of RF path B for each rate section, should match the real calibrate power, -1: undefined");

static int rtw_target_tx_pwr_5g_c[RATE_SECTION_NUM - 1] = CONFIG_RTW_TARGET_TX_PWR_5G_C;
static int rtw_target_tx_pwr_5g_c_num = 0;
module_param_array(rtw_target_tx_pwr_5g_c, int, &rtw_target_tx_pwr_5g_c_num, 0644);
MODULE_PARM_DESC(rtw_target_tx_pwr_5g_c, "5G target tx power (unit:dBm) of RF path C for each rate section, should match the real calibrate power, -1: undefined");

static int rtw_target_tx_pwr_5g_d[RATE_SECTION_NUM - 1] = CONFIG_RTW_TARGET_TX_PWR_5G_D;
static int rtw_target_tx_pwr_5g_d_num = 0;
module_param_array(rtw_target_tx_pwr_5g_d, int, &rtw_target_tx_pwr_5g_d_num, 0644);
MODULE_PARM_DESC(rtw_target_tx_pwr_5g_d, "5G target tx power (unit:dBm) of RF path D for each rate section, should match the real calibrate power, -1: undefined");
#endif /* CONFIG_IEEE80211_BAND_5GHZ */

#ifdef CONFIG_RTW_TX_NPATH_EN
/*0:disable ,1: 2path*/
int rtw_tx_npath_enable = 1;
module_param(rtw_tx_npath_enable, int, 0644);
MODULE_PARM_DESC(rtw_tx_npath_enable, "0:Disable, 1:TX-2PATH");
#endif

#ifdef CONFIG_RTW_PATH_DIV
/*0:disable ,1: path diversity*/
int rtw_path_div_enable = 1;
module_param(rtw_path_div_enable, int, 0644);
MODULE_PARM_DESC(rtw_path_div_enable, "0:Disable, 1:Enable path diversity");
#endif

#ifdef CONFIG_LOAD_PHY_PARA_FROM_FILE
char *rtw_phy_file_path = REALTEK_CONFIG_PATH;
module_param(rtw_phy_file_path, charp, 0644);
MODULE_PARM_DESC(rtw_phy_file_path, "The path of phy parameter");
/* PHY FILE Bit Map
* BIT0 - MAC_REG,			0: non-support, 1: support
* BIT1 - BB_PHY_REG,			0: non-support, 1: support
* BIT2 - BB_PHY_REG_MP,			0: non-support, 1: support
* BIT3 - RF_RADIO,			0: non-support, 1: support
* BIT4 - RF_TXPWR_BY_RATE,		0: non-support, 1: support
* BIT5 - RF_TXPWR_TRACK,		0: non-support, 1: support
* BIT6 - RF_TXPWR_LMT,			0: non-support, 1: support
* BIT7 - RF_TXPWR_LMT_RU,		0: non-support, 1: support
* BIT8 - BB_PHY_REG_GAIN_FILE		0: non-support, 1: support
* BIT9 - RF_TXPWR_LMT_6G_FILE		0: non-support, 1: support
* BIT10 -RF_TXPWR_LMT_RU_6G_FILE	0: non-support, 1: support
*/
int rtw_load_phy_file = ( BIT4 | BIT6 | BIT7 | BIT9 | BIT10);
module_param(rtw_load_phy_file, int, 0644);
MODULE_PARM_DESC(rtw_load_phy_file, "PHY File Bit Map");
int rtw_decrypt_phy_file = 0;
module_param(rtw_decrypt_phy_file, int, 0644);
MODULE_PARM_DESC(rtw_decrypt_phy_file, "Enable Decrypt PHY File");
bool rtw_is_free_param_info = true;
#endif


uint rtw_phydm_ability = 0xffffffff;
module_param(rtw_phydm_ability, uint, 0644);

uint rtw_halrf_ability = 0xffffffff;
module_param(rtw_halrf_ability, uint, 0644);


uint rtw_rfk_ability = 0xffffffff;
module_param(rtw_rfk_ability, uint, 0644);

#ifdef CONFIG_FW_IO_OFLD_SUPPORT
uint rtw_fw_ofld_cap = 1;
module_param(rtw_fw_ofld_cap, uint, 0644);
#endif /*CONFIG_FW_IO_OFLD_SUPPORT*/

uint rtw_edcca_th_2g = 0;
module_param(rtw_edcca_th_2g, uint, 0644);
uint rtw_edcca_th_5g = 0;
module_param(rtw_edcca_th_5g, uint, 0644);
uint rtw_edcca_cs_th = 0;
module_param(rtw_edcca_cs_th, uint, 0644);

#ifdef CONFIG_RTW_MESH
uint rtw_peer_alive_based_preq = 1;
module_param(rtw_peer_alive_based_preq, uint, 0644);
MODULE_PARM_DESC(rtw_peer_alive_based_preq,
	"On demand PREQ will reference peer alive status. 0: Off, 1: On");
#endif

#ifdef CONFIG_RTW_NAPI
/*following setting should define NAPI in Makefile
enable napi only = 1, disable napi = 0*/
int rtw_en_napi = 1;
module_param(rtw_en_napi, int, 0644);
#ifdef CONFIG_RTW_NAPI_DYNAMIC
int rtw_napi_threshold = 100; /* unit: Mbps */
module_param(rtw_napi_threshold, int, 0644);
#endif /* CONFIG_RTW_NAPI_DYNAMIC */
#ifdef CONFIG_RTW_GRO
/*following setting should define GRO in Makefile
enable gro = 1, disable gro = 0*/
int rtw_en_gro = 1;
module_param(rtw_en_gro, int, 0644);
#endif /* CONFIG_RTW_GRO */
#endif /* CONFIG_RTW_NAPI */

#ifdef RTW_IQK_FW_OFFLOAD
int rtw_iqk_fw_offload = 1;
#else
int rtw_iqk_fw_offload;
#endif /* RTW_IQK_FW_OFFLOAD */
module_param(rtw_iqk_fw_offload, int, 0644);

#ifdef RTW_CHANNEL_SWITCH_OFFLOAD
int rtw_ch_switch_offload = 0;
#else
int rtw_ch_switch_offload;
#endif /* RTW_CHANNEL_SWITCH_OFFLOAD */
module_param(rtw_ch_switch_offload, int, 0644);

#ifdef CONFIG_TDLS
int rtw_en_tdls = 1;
module_param(rtw_en_tdls, int, 0644);
#endif

#ifdef CONFIG_FW_OFFLOAD_PARAM_INIT
int rtw_fw_param_init = 1;
module_param(rtw_fw_param_init, int, 0644);
#endif

#ifdef CONFIG_TDMADIG
int rtw_tdmadig_en = 1;
/*
1:MODE_PERFORMANCE
2:MODE_COVERAGE
*/
int rtw_tdmadig_mode = 1;
int rtw_dynamic_tdmadig = 0;
module_param(rtw_tdmadig_en, int, 0644);
module_param(rtw_tdmadig_mode, int, 0644);
module_param(rtw_dynamic_tdmadig, int, 0644);
#endif/*CONFIG_TDMADIG*/

/*dynamic RRSR default enable*/
int rtw_en_dyn_rrsr = 1;
int rtw_rrsr_value = 0xFFFFFFFF;
module_param(rtw_en_dyn_rrsr, int, 0644);
module_param(rtw_rrsr_value, int, 0644);

#ifdef CONFIG_WOWLAN
/*
 * 0: disable, 1: enable
 */
uint rtw_wow_enable = 1;
module_param(rtw_wow_enable, uint, 0644);
/*
 * bit[0]: magic packet wake up
 * bit[1]: unucast packet(HW/FW unuicast)
 * bit[2]: deauth wake up
 */
uint rtw_wakeup_event = RTW_WAKEUP_EVENT;
module_param(rtw_wakeup_event, uint, 0644);
/*
 * 0: common WOWLAN
 * bit[0]: disable BB RF
 * bit[1]: For wireless remote controller with or without connection
 */
uint rtw_suspend_type = RTW_SUSPEND_TYPE;
module_param(rtw_suspend_type, uint, 0644);
#endif

#if defined(PRIVATE_R) && defined(CONFIG_P2P)
int rtw_go_hidden_ssid_mode = ALL_HIDE_SSID;
module_param(rtw_go_hidden_ssid_mode, int, 0644);
#endif

#ifdef RTW_BUSY_DENY_SCAN
uint rtw_scan_interval_thr = BUSY_TRAFFIC_SCAN_DENY_PERIOD;
module_param(rtw_scan_interval_thr, uint, 0644);
MODULE_PARM_DESC(rtw_scan_interval_thr, "Threshold used to judge if scan " \
		 "request comes from scan UI, unit is ms.");
#endif /* RTW_BUSY_DENY_SCAN */

#ifdef CONFIG_HW_HDR_CONVERSION
int rtw_hw_hdr_conv = true;
#else
int rtw_hw_hdr_conv = false;
#endif

#ifdef CONFIG_MCC_MODE
int rtw_mcc_en = _TRUE;
module_param(rtw_mcc_en, int, 0644);
#endif

#ifdef CONFIG_RTW_MULTI_AP
static int rtw_unassoc_sta_mode_of_stype[UNASOC_STA_SRC_NUM] = CONFIG_RTW_UNASOC_STA_MODE_OF_STYPE;
static int rtw_unassoc_sta_mode_of_stype_num = 0;
module_param_array(rtw_unassoc_sta_mode_of_stype, int, &rtw_unassoc_sta_mode_of_stype_num, 0644);

uint rtw_max_unassoc_sta_cnt = 0;
module_param(rtw_max_unassoc_sta_cnt, uint, 0644);
#endif

#ifdef CONFIG_IOCTL_CFG80211
uint rtw_roch_min_home_dur = 1500;
uint rtw_roch_max_away_dur = 500;
uint rtw_roch_extend_dur = 500;
module_param(rtw_roch_min_home_dur, uint, 0644);
module_param(rtw_roch_max_away_dur, uint, 0644);
module_param(rtw_roch_extend_dur, uint, 0644);
#endif

#if CONFIG_DFS
#ifdef CONFIG_ECSA_PHL
uint rtw_en_ecsa = 1;
module_param(rtw_en_ecsa, uint, 0644);
#endif
#endif

#ifdef CONFIG_QUOTA_TURBO_ENABLE
int rtw_quota_turbo_en = true;
#else
int rtw_quota_turbo_en = false;
#endif
module_param(rtw_quota_turbo_en, int, 0644);

uint rtw_scan_pch_ex = 0;
module_param(rtw_scan_pch_ex, uint, 0644);

static void rtw_regsty_load_target_tx_power(struct registry_priv *regsty)
{
	int path, rs;
	int *target_tx_pwr;

	for (path = RF_PATH_A; path < RF_PATH_MAX; path++) {
		if (path == RF_PATH_A)
			target_tx_pwr = rtw_target_tx_pwr_2g_a;
		else if (path == RF_PATH_B)
			target_tx_pwr = rtw_target_tx_pwr_2g_b;
		else if (path == RF_PATH_C)
			target_tx_pwr = rtw_target_tx_pwr_2g_c;
		else if (path == RF_PATH_D)
			target_tx_pwr = rtw_target_tx_pwr_2g_d;

		for (rs = CCK; rs < RATE_SECTION_NUM; rs++)
			regsty->target_tx_pwr_2g[path][rs] = target_tx_pwr[rs];
	}

#if CONFIG_IEEE80211_BAND_5GHZ
	for (path = RF_PATH_A; path < RF_PATH_MAX; path++) {
		if (path == RF_PATH_A)
			target_tx_pwr = rtw_target_tx_pwr_5g_a;
		else if (path == RF_PATH_B)
			target_tx_pwr = rtw_target_tx_pwr_5g_b;
		else if (path == RF_PATH_C)
			target_tx_pwr = rtw_target_tx_pwr_5g_c;
		else if (path == RF_PATH_D)
			target_tx_pwr = rtw_target_tx_pwr_5g_d;

		for (rs = OFDM; rs < RATE_SECTION_NUM; rs++)
			regsty->target_tx_pwr_5g[path][rs - 1] = target_tx_pwr[rs - 1];
	}
#endif /* CONFIG_IEEE80211_BAND_5GHZ */
}

static inline void rtw_regsty_load_chplan(struct registry_priv *regsty)
{
	u16 chplan = RTW_CHPLAN_UNSPECIFIED;
	u16 chplan_6g = RTW_CHPLAN_6G_UNSPECIFIED;

	chplan = rtw_channel_plan;
#if CONFIG_IEEE80211_BAND_6GHZ
	chplan_6g = rtw_channel_plan_6g;
#endif

	rtw_chplan_ioctl_input_mapping(&chplan, &chplan_6g);

	regsty->channel_plan = chplan;
#if CONFIG_IEEE80211_BAND_6GHZ
	regsty->channel_plan_6g = chplan_6g;
#endif
}

static inline void rtw_regsty_load_alpha2(struct registry_priv *regsty)
{
	if (!rtw_country_code || strlen(rtw_country_code) != 2
		|| (!IS_ALPHA2_WORLDWIDE(rtw_country_code)
			&& (is_alpha(rtw_country_code[0]) == _FALSE
				|| is_alpha(rtw_country_code[1]) == _FALSE)
			)
	) {
		if (rtw_country_code && rtw_country_code[0] != '\0')
			RTW_ERR("%s discard rtw_country_code not in alpha2 or \"%s\"\n", __func__, WORLDWIDE_ALPHA2);
		SET_UNSPEC_ALPHA2(regsty->alpha2);
	} else
		_rtw_memcpy(regsty->alpha2, rtw_country_code, 2);
}

static void rtw_regsty_load_addl_ch_disable_conf(struct registry_priv *regsty)
{
	int i;
	int ch_num = 0;

	if (rtw_dis_ch_flags && strlen(rtw_dis_ch_flags)) {
		char *buf = rtw_malloc(strlen(rtw_dis_ch_flags) + 1);

		if (buf) {
			char *c;
			enum rtw_ch_type ch_type;

			_rtw_memcpy(buf, rtw_dis_ch_flags, strlen(rtw_dis_ch_flags) + 1);
			for (c = strsep(&buf, ","); c; c = strsep(&buf, ",")) {
				ch_type = get_ch_type_from_str(c, strlen(c));
				if (ch_type != RTW_CHT_NUM)
					regsty->dis_ch_flags |= BIT(ch_type);
			}
			rtw_mfree(buf, strlen(rtw_dis_ch_flags) + 1);
		} else
			RTW_WARN("%s rtw_malloc(strlen(rtw_dis_ch_flags) fail\n", __func__);
	}

	for (i = 0; i < MAX_CHANNEL_NUM_2G_5G; i++)
		if (((u8)rtw_excl_chs[i]) != 0)
			regsty->excl_chs[ch_num++] = (u8)rtw_excl_chs[i];

	if (ch_num < MAX_CHANNEL_NUM_2G_5G)
		regsty->excl_chs[ch_num] = 0;

#if CONFIG_IEEE80211_BAND_6GHZ
	ch_num = 0;
	for (i = 0; i < MAX_CHANNEL_NUM_6G; i++)
		if (((u8)rtw_excl_chs_6g[i]) != 0)
			regsty->excl_chs_6g[ch_num++] = (u8)rtw_excl_chs_6g[i];

	if (ch_num < MAX_CHANNEL_NUM_6G)
		regsty->excl_chs_6g[ch_num] = 0;
#endif
}

#ifdef CONFIG_80211D
inline void rtw_regsty_load_country_ie_slave_settings(struct registry_priv *regsty)
{
	regsty->country_ie_slave_en_mode = rtw_country_ie_slave_en_mode;
	regsty->country_ie_slave_flags = rtw_country_ie_slave_flags;
	regsty->country_ie_slave_en_role = rtw_country_ie_slave_en_role;
	regsty->country_ie_slave_en_ifbmp = rtw_country_ie_slave_en_ifbmp;
	regsty->country_ie_slave_scan_int_ms = rtw_country_ie_slave_scan_int_ms;
}
#endif

#ifdef CONFIG_DFS_MASTER
static void rtw_regsty_load_dfs_region_domain_settings(struct registry_priv *regsty)
{
	regsty->dfs_region_domain = (u8)rtw_dfs_region_domain;
	if (regsty->dfs_region_domain >= RTW_DFS_REGD_NUM) {
		RTW_WARN("%s invalid DFS region domain(%u), set to %s\n", __func__
			, regsty->dfs_region_domain, rtw_dfs_regd_str(RTW_DFS_REGD_NONE));
		regsty->dfs_region_domain = RTW_DFS_REGD_NONE;
		return;
	}
	#ifdef CONFIG_REGD_SRC_FROM_OS
	if (rtw_regd_src == REGD_SRC_OS && regsty->dfs_region_domain != RTW_DFS_REGD_NONE) {
		RTW_WARN("%s force disable radar detection capability when regd_src is OS\n", __func__);
		regsty->dfs_region_domain = RTW_DFS_REGD_NONE;
	}
	#endif
}
#endif

#ifdef CONFIG_80211N_HT
static inline void rtw_regsty_init_rx_ampdu_sz_limit(struct registry_priv *regsty)
{
	int i, j;
	uint *sz_limit;

	for (i = 0; i < 4; i++) {
		if (i == 0)
			sz_limit = rtw_rx_ampdu_sz_limit_1ss;
		else if (i == 1)
			sz_limit = rtw_rx_ampdu_sz_limit_2ss;
		else if (i == 2)
			sz_limit = rtw_rx_ampdu_sz_limit_3ss;
		else if (i == 3)
			sz_limit = rtw_rx_ampdu_sz_limit_4ss;

		for (j = 0; j < 4; j++)
			regsty->rx_ampdu_sz_limit_by_nss_bw[i][j] = sz_limit[j];
	}
}
#endif /* CONFIG_80211N_HT */

#ifdef CONFIG_RTW_MULTI_AP
inline void rtw_regsty_init_unassoc_sta_param(struct registry_priv *regsty)
{
	int i;

	for (i = 0; i < UNASOC_STA_SRC_NUM; i++)
		regsty->unassoc_sta_mode_of_stype[i] = rtw_unassoc_sta_mode_of_stype[i];

	regsty->max_unassoc_sta_cnt = (u16)rtw_max_unassoc_sta_cnt;
}
#endif

static void rtw_load_phy_file_path (struct dvobj_priv *dvobj)
{
	struct rtw_phl_com_t *phl_com = GET_PHL_COM(dvobj);

#ifdef CONFIG_LOAD_PHY_PARA_FROM_FILE
	if (rtw_load_phy_file & LOAD_BB_PHY_REG_FILE) {
		phl_com->phy_sw_cap[0].bb_phy_reg_info.para_src = RTW_PARA_SRC_EXTNAL;
		phl_com->phy_sw_cap[1].bb_phy_reg_info.para_src = RTW_PARA_SRC_EXTNAL;
	}

	if (rtw_load_phy_file & LOAD_RF_RADIO_FILE) {
		phl_com->phy_sw_cap[0].rf_radio_a_info.para_src = RTW_PARA_SRC_EXTNAL;
		phl_com->phy_sw_cap[1].rf_radio_a_info.para_src = RTW_PARA_SRC_EXTNAL;
		phl_com->phy_sw_cap[0].rf_radio_b_info.para_src = RTW_PARA_SRC_EXTNAL;
		phl_com->phy_sw_cap[1].rf_radio_b_info.para_src = RTW_PARA_SRC_EXTNAL;
	}

	if (rtw_load_phy_file & LOAD_RF_TXPWR_BY_RATE) {
		phl_com->phy_sw_cap[0].rf_txpwr_byrate_info.para_src = RTW_PARA_SRC_EXTNAL;
		phl_com->phy_sw_cap[1].rf_txpwr_byrate_info.para_src = RTW_PARA_SRC_EXTNAL;
	}

	if (rtw_load_phy_file & LOAD_RF_TXPWR_TRACK_FILE) {
		phl_com->phy_sw_cap[0].rf_txpwrtrack_info.para_src = RTW_PARA_SRC_EXTNAL;
		phl_com->phy_sw_cap[1].rf_txpwrtrack_info.para_src = RTW_PARA_SRC_EXTNAL;
	}

	if (rtw_load_phy_file & LOAD_RF_TXPWR_LMT_FILE) {
		phl_com->phy_sw_cap[0].rf_txpwrlmt_info.para_src = RTW_PARA_SRC_EXTNAL;
		phl_com->phy_sw_cap[1].rf_txpwrlmt_info.para_src = RTW_PARA_SRC_EXTNAL;
	}

	if (rtw_load_phy_file & LOAD_RF_TXPWR_LMT_RU_FILE) {
		phl_com->phy_sw_cap[0].rf_txpwrlmt_ru_info.para_src = RTW_PARA_SRC_EXTNAL;
		phl_com->phy_sw_cap[1].rf_txpwrlmt_ru_info.para_src = RTW_PARA_SRC_EXTNAL;
	}

	if (rtw_load_phy_file & LOAD_BB_PHY_REG_GAIN_FILE) {
		phl_com->phy_sw_cap[0].bb_phy_reg_gain_info.para_src = RTW_PARA_SRC_EXTNAL;
		phl_com->phy_sw_cap[1].bb_phy_reg_gain_info.para_src = RTW_PARA_SRC_EXTNAL;
	}

	if (rtw_load_phy_file & LOAD_RF_TXPWR_LMT_6G_FILE) {
		phl_com->phy_sw_cap[0].rf_txpwrlmt_6g_info.para_src = RTW_PARA_SRC_EXTNAL;
		phl_com->phy_sw_cap[1].rf_txpwrlmt_6g_info.para_src = RTW_PARA_SRC_EXTNAL;
	}

	if (rtw_load_phy_file & LOAD_RF_TXPWR_LMT_RU_6G_FILE) {
		phl_com->phy_sw_cap[0].rf_txpwrlmt_ru_6g_info.para_src = RTW_PARA_SRC_EXTNAL;
		phl_com->phy_sw_cap[1].rf_txpwrlmt_ru_6g_info.para_src = RTW_PARA_SRC_EXTNAL;
	}

#endif/* CONFIG_LOAD_PHY_PARA_FROM_FILE */
}

void rtw_core_update_default_setting (struct dvobj_priv *dvobj)
{
	int i;
	struct rtw_phl_com_t *phl_com = GET_PHL_COM(dvobj);
#ifdef CONFIG_PCIE_TRX_MIT_DYN
	struct rtw_pcie_trx_mit_info_t mit_info = {0};
	void *phl = GET_PHL_INFO(dvobj);
#endif
	u8 max_bw_mode = 0;
	#ifdef CONFIG_LOAD_PHY_PARA_FROM_FILE
	rtw_load_phy_file_path(dvobj);
	phl_com->dev_sw_cap.bfree_para_info = rtw_is_free_param_info;
	#endif /* CONFIG_LOAD_PHY_PARA_FROM_FILE */

	if (is_supported_24g(rtw_band_type))
		max_bw_mode = BW_MODE_2G(rtw_bw_mode);
	if (is_supported_5g(rtw_band_type))
		max_bw_mode = rtw_max(BW_MODE_5G(rtw_bw_mode), max_bw_mode);
	if (is_supported_6g(rtw_band_type))
		max_bw_mode = rtw_max(BW_MODE_6G(rtw_bw_mode), max_bw_mode);

	phl_com->dev_sw_cap.fw_cap.dlram_en = true;
	phl_com->dev_sw_cap.fw_cap.dlrom_en = false;
	#ifdef CONFIG_FILE_FWIMG
	phl_com->dev_sw_cap.fw_cap.fw_src = RTW_FW_SRC_EXTNAL;
	#else /* !CONFIG_FILE_FWIMG */
	phl_com->dev_sw_cap.fw_cap.fw_src = RTW_FW_SRC_INTNAL;
	#endif /* !CONFIG_FILE_FWIMG */

	#ifdef CONFIG_FW_IO_OFLD_SUPPORT
	phl_com->dev_sw_cap.fw_cap.offload_cap = rtw_fw_ofld_cap;
	#endif

	phl_com->phy_sw_cap[0].proto_sup = rtw_wireless_mode;
	phl_com->phy_sw_cap[0].band_sup = rtw_band_type;
	phl_com->phy_sw_cap[0].bw_sup = ch_width_to_bw_cap(max_bw_mode + 1) - 1; /* max supported bw */
	phl_com->phy_sw_cap[0].txss = rtw_tx_nss;
	phl_com->phy_sw_cap[0].rxss = rtw_rx_nss;
	phl_com->phy_sw_cap[0].txagg_num = rtw_tx_ampdu_num;

	phl_com->phy_sw_cap[1].proto_sup = rtw_wireless_mode;
	phl_com->phy_sw_cap[1].band_sup = rtw_band_type;
	phl_com->phy_sw_cap[1].bw_sup = ch_width_to_bw_cap(max_bw_mode + 1) - 1; /* max supported bw */
	phl_com->phy_sw_cap[1].txss = rtw_tx_nss;
	phl_com->phy_sw_cap[1].rxss = rtw_rx_nss;
	phl_com->phy_sw_cap[1].txagg_num = rtw_tx_ampdu_num;

	phl_com->phy_sw_cap[0].hw_rts_time_th = 88;
	phl_com->phy_sw_cap[0].hw_rts_len_th = 4080;
	phl_com->phy_sw_cap[1].hw_rts_time_th = 88;
	phl_com->phy_sw_cap[1].hw_rts_len_th = 4080;

	/*phl_com->dev_sw_cap.pkg_type = rtw_pkg_type;*/
	phl_com->dev_sw_cap.rfe_type = rtw_rfe_type;
#ifdef DBG_LA_MODE
	phl_com->dev_sw_cap.la_mode = rtw_la_mode_en;
#endif

#ifdef CONFIG_DBCC_SUPPORT
	phl_com->dev_sw_cap.dbcc_sup = rtw_dbcc_en;
	phl_com->dev_sw_cap.mcmb_itf_ctrl = rtw_dbcc_itf_ctl;
#ifdef CONFIG_DBCC_FORCE
	phl_com->dev_sw_cap.dbcc_force_rmap = rtw_dbcc_force_rmap;
	phl_com->dev_sw_cap.dbcc_force_cck_phyidx = rtw_dbcc_force_cck_phyidx;
#endif /*CONFIG_DBCC_FORCE*/
#endif /*CONFIG_DBCC_SUPPORT*/

	phl_com->dev_sw_cap.hw_hdr_conv = rtw_hw_hdr_conv;

	phl_com->proto_sw_cap[0].max_amsdu_len = rtw_max_amsdu_len;
	phl_com->proto_sw_cap[1].max_amsdu_len = rtw_max_amsdu_len;

#if defined(CONFIG_PCI_HCI)
	#if defined(CONFIG_PCI_ASPM)
	phl_com->bus_sw_cap.l0s_ctrl = (rtw_pci_aspm_enable & BIT1) ? RTW_PCIE_BUS_FUNC_ENABLE : RTW_PCIE_BUS_FUNC_DISABLE;
	phl_com->bus_sw_cap.l1_ctrl = (rtw_pci_aspm_enable & BIT2) ? RTW_PCIE_BUS_FUNC_ENABLE : RTW_PCIE_BUS_FUNC_DISABLE;
	phl_com->bus_sw_cap.l1ss_ctrl = (rtw_pci_aspm_enable & BIT3) ? RTW_PCIE_BUS_FUNC_ENABLE : RTW_PCIE_BUS_FUNC_DISABLE;
	phl_com->bus_sw_cap.wake_ctrl = RTW_PCIE_BUS_FUNC_DEFAULT;
	phl_com->bus_sw_cap.crq_ctrl = (rtw_pci_aspm_enable & BIT0) ? RTW_PCIE_BUS_FUNC_ENABLE : RTW_PCIE_BUS_FUNC_DISABLE;
	#else
	phl_com->bus_sw_cap.l0s_ctrl = RTW_PCIE_BUS_FUNC_DISABLE;
	phl_com->bus_sw_cap.l1_ctrl = RTW_PCIE_BUS_FUNC_DISABLE;
	phl_com->bus_sw_cap.l1ss_ctrl = RTW_PCIE_BUS_FUNC_DISABLE;
	phl_com->bus_sw_cap.wake_ctrl = RTW_PCIE_BUS_FUNC_DEFAULT;
	phl_com->bus_sw_cap.crq_ctrl = RTW_PCIE_BUS_FUNC_DISABLE;
	#endif
	phl_com->bus_sw_cap.txbd_num = 256;
#if defined(CONFIG_TX_WD_NUM) && (CONFIG_TX_WD_NUM > 0)
	phl_com->bus_sw_cap.wd_num = CONFIG_TX_WD_NUM;
#else
	phl_com->bus_sw_cap.wd_num = 0;
#endif
	phl_com->bus_sw_cap.rxbd_num = 256;
	phl_com->bus_sw_cap.rpbd_num = 256;
#ifdef CONFIG_RXBUF_NUM_1024
	phl_com->bus_sw_cap.rxbuf_num = 1024;
#else
	phl_com->bus_sw_cap.rxbuf_num = 512;
#endif
	phl_com->bus_sw_cap.rpbuf_num = 1024;
#ifdef CONFIG_RTW_REDUCE_MEM
	phl_com->bus_sw_cap.rxbuf_size = CORE_RXBUF_SIZE;
	phl_com->bus_sw_cap.rpbuf_size = CORE_RPBUF_SIZE;
#else
	phl_com->bus_sw_cap.rxbuf_size = 11460;
	phl_com->bus_sw_cap.rpbuf_size = 11460;
#endif
#ifdef CONFIG_PCIE_TRX_MIT_DYN
	mit_info.tx_timer = 0;
	mit_info.tx_counter = 0;
	mit_info.rx_timer = 0;
	mit_info.rx_counter = 0;
	mit_info.fixed_mitigation = 0;
	mit_info.rx_mit_timer_high = PCIE_RX_INT_MIT_TIMER;
	mit_info.rx_mit_counter_high = 0;
	rtw_phl_pcie_trx_mit_cfg(phl, &mit_info); /* send to the phl cfg */
#endif
#endif /*CONFIG_PCI_HCI*/

#ifdef CONFIG_BTC
	phl_com->dev_sw_cap.btc_mode = BTC_MODE_NORMAL;
#else
	phl_com->dev_sw_cap.btc_mode = BTC_MODE_WL;
#endif
#ifdef CONFIG_MCC_MODE
	phl_com->dev_sw_cap.mcc_sup = rtw_mcc_en;
#endif
#ifdef CONFIG_USB_HCI
	phl_com->bus_sw_cap.tx_buf_size = MAX_XMITBUF_SZ;
	phl_com->bus_sw_cap.tx_buf_num = NR_XMITBUFF;
	phl_com->bus_sw_cap.tx_mgnt_buf_size = MAX_MGNT_XMITBUF_SZ;
	phl_com->bus_sw_cap.tx_mgnt_buf_num = NR_MGNT_XMITBUFF;
	phl_com->bus_sw_cap.rx_buf_size = MAX_RECVBUF_SZ;
	phl_com->bus_sw_cap.rx_buf_num = NR_RECVBUFF;
	phl_com->bus_sw_cap.in_token_num = NR_RECV_URB;
#endif
#ifdef CONFIG_SDIO_HCI
#ifdef MAX_XMITBUF_SZ
	phl_com->bus_sw_cap.tx_buf_size = MAX_XMITBUF_SZ;
#endif
#ifdef NR_XMITBUFF
	phl_com->bus_sw_cap.tx_buf_num = NR_XMITBUFF;
#endif
#ifdef MAX_MGNT_XMITBUF_SZ
	phl_com->bus_sw_cap.tx_mgnt_buf_size = MAX_MGNT_XMITBUF_SZ;
#endif
#ifdef NR_MGNT_XMITBUFF
	phl_com->bus_sw_cap.tx_mgnt_buf_num = NR_MGNT_XMITBUFF;
#endif
#ifdef MAX_RECVBUF_SZ
	phl_com->bus_sw_cap.rx_buf_size = MAX_RECVBUF_SZ;
#endif
#ifdef NR_RECVBUFF
	phl_com->bus_sw_cap.rx_buf_num = NR_RECVBUFF;
#endif
#endif /* CONFIG_SDIO_HCI */

	/* Set STBC Tx/Rx sw role cap */
	phl_com->role_sw_cap.stbc_cap = 0;
	phl_com->role_sw_cap.stbc_cap |=
		(rtw_stbc_cap & BIT5) ? HW_CAP_STBC_HT_TX : 0;
	phl_com->role_sw_cap.stbc_cap |=
		(rtw_stbc_cap & BIT1) ? HW_CAP_STBC_VHT_TX : 0;
	phl_com->role_sw_cap.stbc_cap |=
		(rtw_stbc_cap & BIT10) ? HW_CAP_STBC_HE_TX : 0;
	phl_com->role_sw_cap.stbc_cap |=
		(rtw_stbc_cap & BIT11) ? HW_CAP_STBC_HE_TX_GT_80M : 0;

	phl_com->role_sw_cap.stbc_cap |=
		(rtw_stbc_cap & BIT4) ? HW_CAP_STBC_HT_RX : 0;
	phl_com->role_sw_cap.stbc_cap |=
		(rtw_stbc_cap & BIT0) ? HW_CAP_STBC_VHT_RX : 0;
	phl_com->role_sw_cap.stbc_cap |=
		(rtw_stbc_cap & BIT8) ? HW_CAP_STBC_HE_RX : 0;
	phl_com->role_sw_cap.stbc_cap |=
		(rtw_stbc_cap & BIT9) ? HW_CAP_STBC_HE_RX_GT_80M : 0;

	/*Band0*/
	phl_com->proto_sw_cap[0].stbc_ht_tx = (rtw_stbc_cap & BIT5) ? 1 : 0;
	phl_com->proto_sw_cap[0].stbc_vht_tx = (rtw_stbc_cap & BIT1) ? 1 : 0;
	phl_com->proto_sw_cap[0].stbc_he_tx = (rtw_stbc_cap & BIT10) ? 1 : 0;
	phl_com->proto_sw_cap[0].stbc_tx_greater_80mhz = (rtw_stbc_cap & BIT11) ? 1 : 0;

	phl_com->proto_sw_cap[0].stbc_ht_rx = (rtw_stbc_cap & BIT4) ? 1 : 0;
	phl_com->proto_sw_cap[0].stbc_vht_rx = (rtw_stbc_cap & BIT0) ? 1 : 0;
	phl_com->proto_sw_cap[0].stbc_he_rx = (rtw_stbc_cap & BIT8) ? 1 : 0;
	phl_com->proto_sw_cap[0].stbc_rx_greater_80mhz = (rtw_stbc_cap & BIT9) ? 1 : 0;

	/*Band1*/
	phl_com->proto_sw_cap[1].stbc_ht_tx = (rtw_stbc_cap & BIT5) ? 1 : 0;
	phl_com->proto_sw_cap[1].stbc_vht_tx = (rtw_stbc_cap & BIT1) ? 1 : 0;
	phl_com->proto_sw_cap[1].stbc_he_tx = (rtw_stbc_cap & BIT10) ? 1 : 0;
	phl_com->proto_sw_cap[1].stbc_tx_greater_80mhz = (rtw_stbc_cap & BIT11) ? 1 : 0;

	phl_com->proto_sw_cap[1].stbc_ht_rx = (rtw_stbc_cap & BIT4) ? 1 : 0;
	phl_com->proto_sw_cap[1].stbc_vht_rx = (rtw_stbc_cap & BIT0) ? 1 : 0;
	phl_com->proto_sw_cap[1].stbc_he_rx = (rtw_stbc_cap & BIT8) ? 1 : 0;
	phl_com->proto_sw_cap[1].stbc_rx_greater_80mhz = (rtw_stbc_cap & BIT9) ? 1 : 0;

#ifdef CONFIG_BEAMFORMING
	phl_com->role_sw_cap.bf_cap = 0;
	phl_com->role_sw_cap.bf_cap |= (rtw_beamform_cap & BIT0) ? HW_CAP_BFER_VHT_SU : 0;
	phl_com->role_sw_cap.bf_cap |= (rtw_beamform_cap & BIT1) ? HW_CAP_BFEE_VHT_SU: 0;
	phl_com->role_sw_cap.bf_cap |= (rtw_beamform_cap & BIT2) ? HW_CAP_BFER_VHT_MU: 0;
	phl_com->role_sw_cap.bf_cap |= (rtw_beamform_cap & BIT3) ? HW_CAP_BFEE_VHT_MU: 0;
	phl_com->role_sw_cap.bf_cap |= (rtw_beamform_cap & BIT4) ? HW_CAP_BFER_HT_SU: 0;
	phl_com->role_sw_cap.bf_cap |= (rtw_beamform_cap & BIT5) ? HW_CAP_BFEE_HT_SU: 0;
	phl_com->role_sw_cap.bf_cap |= (rtw_beamform_cap & BIT6) ? HW_CAP_BFER_HE_SU: 0;
	phl_com->role_sw_cap.bf_cap |= (rtw_beamform_cap & BIT7) ? HW_CAP_BFEE_HE_SU: 0;
	phl_com->role_sw_cap.bf_cap |= (rtw_beamform_cap & BIT8) ? HW_CAP_BFER_HE_MU: 0;
	phl_com->role_sw_cap.bf_cap |= (rtw_beamform_cap & BIT9) ? HW_CAP_BFEE_HE_MU: 0;

	/*Band0*/
	phl_com->proto_sw_cap[0].vht_su_bfmr = (rtw_sw_proto_bf_cap_phy0 & BIT0) ? 1 : 0;
	phl_com->proto_sw_cap[0].vht_su_bfme = (rtw_sw_proto_bf_cap_phy0 & BIT1) ? 1 : 0;
	phl_com->proto_sw_cap[0].vht_mu_bfmr = (rtw_sw_proto_bf_cap_phy0 & BIT2) ? 1 : 0;
	phl_com->proto_sw_cap[0].vht_mu_bfme = (rtw_sw_proto_bf_cap_phy0 & BIT3) ? 1 : 0;
	phl_com->proto_sw_cap[0].ht_su_bfmr = (rtw_sw_proto_bf_cap_phy0 & BIT4) ? 1 : 0;
	phl_com->proto_sw_cap[0].ht_su_bfme = (rtw_sw_proto_bf_cap_phy0 & BIT5) ? 1 : 0;
	phl_com->proto_sw_cap[0].he_su_bfmr = (rtw_sw_proto_bf_cap_phy0 & BIT6) ? 1 : 0;
	phl_com->proto_sw_cap[0].he_su_bfme = (rtw_sw_proto_bf_cap_phy0 & BIT7) ? 1 : 0;
	phl_com->proto_sw_cap[0].he_mu_bfmr = (rtw_sw_proto_bf_cap_phy0 & BIT8) ? 1 : 0;
	phl_com->proto_sw_cap[0].he_mu_bfme = (rtw_sw_proto_bf_cap_phy0 & BIT9) ? 1 : 0;

	/*Band1*/
	phl_com->proto_sw_cap[1].vht_su_bfmr = (rtw_sw_proto_bf_cap_phy1 & BIT0) ? 1 : 0;
	phl_com->proto_sw_cap[1].vht_su_bfme = (rtw_sw_proto_bf_cap_phy1 & BIT1) ? 1 : 0;
	phl_com->proto_sw_cap[1].vht_mu_bfmr = (rtw_sw_proto_bf_cap_phy1 & BIT2) ? 1 : 0;
	phl_com->proto_sw_cap[1].vht_mu_bfme = (rtw_sw_proto_bf_cap_phy1 & BIT3) ? 1 : 0;
	phl_com->proto_sw_cap[1].ht_su_bfmr = (rtw_sw_proto_bf_cap_phy1 & BIT4) ? 1 : 0;
	phl_com->proto_sw_cap[1].ht_su_bfme = (rtw_sw_proto_bf_cap_phy1 & BIT5) ? 1 : 0;
	phl_com->proto_sw_cap[1].he_su_bfmr = (rtw_sw_proto_bf_cap_phy1 & BIT6) ? 1 : 0;
	phl_com->proto_sw_cap[1].he_su_bfme = (rtw_sw_proto_bf_cap_phy1 & BIT7) ? 1 : 0;
	phl_com->proto_sw_cap[1].he_mu_bfmr = (rtw_sw_proto_bf_cap_phy1 & BIT8) ? 1 : 0;
	phl_com->proto_sw_cap[1].he_mu_bfme = (rtw_sw_proto_bf_cap_phy1 & BIT9) ? 1 : 0;
#endif

#if CONFIG_TXPWR_LIMIT
	if (rtw_tx_pwr_lmt_enable == 2)
		phl_com->dev_sw_cap.pwrlmt_type = RTW_PWLMT_BY_EFUSE;
	else if (rtw_tx_pwr_lmt_enable == 1)
		phl_com->dev_sw_cap.pwrlmt_type = RTW_PWBYRATE_AND_PWLMT;
	else if (rtw_tx_pwr_lmt_enable == 0)
		phl_com->dev_sw_cap.pwrlmt_type = RTW_PWLMT_DISABLE;
#else
	phl_com->dev_sw_cap.pwrlmt_type = RTW_PWLMT_DISABLE;
#endif

	if (rtw_tx_pwr_by_rate == 1)
		phl_com->dev_sw_cap.pwrbyrate_off = RTW_PW_BY_RATE_ON;
	else if (rtw_tx_pwr_by_rate == 0)
		phl_com->dev_sw_cap.pwrbyrate_off = RTW_PW_BY_RATE_ALL_SAME;
	else
		phl_com->dev_sw_cap.pwrbyrate_off = RTW_PW_BY_RATE_ON;

	/* If rf_board_opt is not assigned to specific value, it must be set to 0xFF as default. */
	phl_com->dev_sw_cap.rf_board_opt = 0xFF;

#ifdef CONFIG_POWER_SAVE
#ifdef CONFIG_RTW_IPS
	phl_com->dev_sw_cap.ps_cap.ips_en = rtw_ips_mode;
#ifdef CONFIG_FWIPS
	phl_com->dev_sw_cap.ps_cap.ips_cap = rtw_ips_cap;
#endif /* CONFIG_FWIPS */
#endif
#ifdef CONFIG_RTW_LPS
	phl_com->dev_sw_cap.ps_cap.lps_en = rtw_lps_mode;
	phl_com->dev_sw_cap.ps_cap.lps_cap = rtw_lps_cap;
#endif
	phl_com->dev_sw_cap.ps_cap.lps_wow_en = PS_OP_MODE_FORCE_ENABLED;
	phl_com->dev_sw_cap.ps_cap.lps_wow_cap =
		PS_CAP_PWRON | PS_CAP_RF_OFF | PS_CAP_CLK_GATED | PS_CAP_PWR_GATED;
#endif /* CONFIG_POWER_SAVE */

	phl_com->dev_sw_cap.rfk_cap = rtw_rfk_ability;

	phl_com->dev_sw_cap.edcca_cap.edcca_adap_th_2g = rtw_edcca_th_2g;
	phl_com->dev_sw_cap.edcca_cap.edcca_adap_th_5g = rtw_edcca_th_5g;
	phl_com->dev_sw_cap.edcca_cap.edcca_carrier_sense_th = rtw_edcca_cs_th;

	/* ref: rtw_update_phl_iot() */
	for (i = 0; i < MAX_WIFI_ROLE_NUMBER; i++)
		phl_com->id.iot_id[i] = IOT_ID(0);
#ifdef CONFIG_RTW_LED
	rtw_phl_led_set_ctrl_mode(GET_PHL_INFO(dvobj), 0, RTW_LED_CTRL_HW_TX_MODE);
#endif

#if defined (CONFIG_RPQ_AGG_NUM) && (CONFIG_RPQ_AGG_NUM > 0)
	phl_com->dev_sw_cap.rpq_agg_num = CONFIG_RPQ_AGG_NUM;
#else
	phl_com->dev_sw_cap.rpq_agg_num = 0; /* MAC default num: 121 for all IC */
#endif

	phl_com->dev_sw_cap.quota_turbo = rtw_quota_turbo_en;

#ifdef CONFIG_HW_FORM_SEC_HEADER
	dvobj->phl_com->dev_sw_cap.sec_cap.hw_form_hdr = true;
#else
	dvobj->phl_com->dev_sw_cap.sec_cap.hw_form_hdr = false;
#endif
	dvobj->phl_com->dev_sw_cap.sec_cap.hw_sec_iv = false;
	dvobj->phl_com->dev_sw_cap.sec_cap.hw_tx_search_key = false;
}

u8 rtw_load_dvobj_registry(struct dvobj_priv *dvobj)
{
	/*struct rtw_phl_com_t *phl_com = GET_PHL_COM(dvobj);*/
	#ifdef CONFIG_CONCURRENT_MODE
	dvobj->virtual_iface_num = (u8)rtw_virtual_iface_num;
	#endif
	return _SUCCESS;
}

uint rtw_load_registry(_adapter *padapter)
{
	uint status = _SUCCESS;
	struct registry_priv  *registry_par = &padapter->registrypriv;


#ifdef CONFIG_RTW_DEBUG
	if (rtw_drv_log_level >= _DRV_MAX_)
		rtw_drv_log_level = _DRV_DEBUG_;
#endif

	registry_par->chip_version = (u8)rtw_chip_version;
	registry_par->rfintfs = (u8)rtw_rfintfs;
	registry_par->lbkmode = (u8)rtw_lbkmode;
	/* registry_par->hci = (u8)hci; */
	registry_par->network_mode  = (u8)rtw_network_mode;

	_rtw_memcpy(registry_par->ssid.Ssid, "ANY", 3);
	registry_par->ssid.SsidLength = 3;

	registry_par->band = (u8)rtw_band;
	registry_par->channel = (u8)rtw_channel;
#ifdef CONFIG_NARROWBAND_SUPPORTING
	if (rtw_nb_config != RTW_NB_CONFIG_NONE)
		rtw_wireless_mode &= ~WIRELESS_11B;
#endif
	registry_par->wireless_mode = (u8)rtw_wireless_mode;
	registry_par->band_type = (u8)rtw_band_type;

	if (is_supported_24g(registry_par->band_type) && (!is_supported_5g(registry_par->band_type))
		&& (registry_par->channel > 14)) {
		registry_par->band = BAND_ON_24G;
		registry_par->channel = 1;
	} else if (is_supported_5g(registry_par->band_type) && (!is_supported_24g(registry_par->band_type))
		&& (registry_par->channel <= 14)) {
		registry_par->band = BAND_ON_5G;
		registry_par->channel = 36;
	} else if (is_supported_6g(registry_par->band_type) && !is_supported_24g(registry_par->band_type)
		&& !is_supported_5g(registry_par->band_type) && (registry_par->band != BAND_ON_6G)){
		registry_par->band = BAND_ON_6G;
		registry_par->channel = 33;
	}

	registry_par->adaptivity_en = (u8)rtw_adaptivity_en;
	registry_par->adaptivity_mode = (u8)rtw_adaptivity_mode;
	registry_par->adaptivity_th_l2h_ini = (s8)rtw_adaptivity_th_l2h_ini;
	registry_par->adaptivity_th_edcca_hl_diff = (s8)rtw_adaptivity_th_edcca_hl_diff;
	registry_par->adaptivity_idle_probability = (u8)rtw_adaptivity_idle_probability;

	if (registry_par->adaptivity_idle_probability == 1) {
		rtw_vrtl_carrier_sense = DISABLE_VCS;
		rtw_vcs_type = NONE_VCS;
		rtw_hw_rts_en = 0;
	}

	registry_par->vrtl_carrier_sense = (u8)rtw_vrtl_carrier_sense ;
	registry_par->vcs_type = (u8)rtw_vcs_type;
	registry_par->rts_thresh = (u16)rtw_rts_thresh;
	registry_par->hw_rts_en = (u8)rtw_hw_rts_en;
	registry_par->frag_thresh = (u16)rtw_frag_thresh;
	registry_par->preamble = (u8)rtw_preamble;
	registry_par->scan_mode = (u8)rtw_scan_mode;
	#ifdef CONFIG_TDMADIG
	registry_par->tdmadig_en = (u8)rtw_tdmadig_en;
	registry_par->tdmadig_mode = (u8)rtw_tdmadig_mode;
	registry_par->tdmadig_dynamic = (u8) rtw_dynamic_tdmadig;
	#endif/*CONFIG_TDMADIG*/
	registry_par->ips_mode = (u8)rtw_ips_mode;
	registry_par->lps_mode = (u8)rtw_lps_mode;
	registry_par->en_dyn_rrsr = (u8)rtw_en_dyn_rrsr;
	registry_par->set_rrsr_value = (u32)rtw_rrsr_value;

	registry_par->radio_enable = (u8)rtw_radio_enable;
	registry_par->long_retry_lmt = (u8)rtw_long_retry_lmt;
	registry_par->short_retry_lmt = (u8)rtw_short_retry_lmt;
	registry_par->busy_thresh = (u16)rtw_busy_thresh;
	registry_par->max_bss_cnt = (u16)rtw_max_bss_cnt;
	/* registry_par->qos_enable = (u8)rtw_qos_enable; */
	registry_par->ack_policy = (u8)rtw_ack_policy;
	registry_par->mp_mode = (u8)rtw_mp_mode;
#if defined(CONFIG_MP_INCLUDED) && defined(CONFIG_RTW_CUSTOMER_STR)
	registry_par->mp_customer_str = (u8)rtw_mp_customer_str;
#endif
	registry_par->software_encrypt = (u8)rtw_software_encrypt;
	registry_par->software_decrypt = (u8)rtw_software_decrypt;

	registry_par->acm_method = (u8)rtw_acm_method;
	registry_par->usb_rxagg_mode = (u8)rtw_usb_rxagg_mode;
	registry_par->dynamic_agg_enable = (u8)rtw_dynamic_agg_enable;

	/* WMM */
	registry_par->wmm_enable = (u8)rtw_wmm_enable;

	registry_par->RegPwrTrimEnable = (u8)rtw_pwrtrim_enable;

	registry_par->tx_bw_mode = (u8)rtw_tx_bw_mode;

#ifdef CONFIG_80211N_HT
	registry_par->ht_enable = (u8)rtw_ht_enable;
	if (registry_par->ht_enable && is_supported_ht(registry_par->wireless_mode)) {
#ifdef CONFIG_NARROWBAND_SUPPORTING
	if (rtw_nb_config != RTW_NB_CONFIG_NONE)
		rtw_bw_mode = 0;
#endif
		registry_par->bw_mode = (u16)rtw_bw_mode;
		registry_par->ampdu_enable = (u8)rtw_ampdu_enable;
		registry_par->rx_ampdu_amsdu = (u8)rtw_rx_ampdu_amsdu;
#ifdef CONFIG_DISBALE_RX_AMSDU_FOR_BUS_LOW_SPEED
#ifdef CONFIG_USB_HCI
		if (dvobj_to_usb(adapter_to_dvobj(padapter))->usb_speed < RTW_USB_SPEED_SUPER)
			registry_par->rx_ampdu_amsdu = 0;
#endif
#endif
		registry_par->tx_ampdu_amsdu = (u8)rtw_tx_ampdu_amsdu;
		registry_par->tx_ampdu_num = (u8)rtw_tx_ampdu_num;
		registry_par->tx_quick_addba_req = (u8)rtw_quick_addba_req;
		registry_par->short_gi = (u8)rtw_short_gi;
		registry_par->ldpc_cap = (u8)rtw_ldpc_cap;

#ifdef CONFIG_RTW_TX_NPATH_EN
		registry_par->tx_npath = (u8)rtw_tx_npath_enable;
#endif
#ifdef CONFIG_RTW_PATH_DIV
		registry_par->path_div = (u8)rtw_path_div_enable;
#endif
		registry_par->stbc_cap = (u16)rtw_stbc_cap;
#ifdef CONFIG_BEAMFORMING
		registry_par->beamform_cap = (u16)rtw_beamform_cap;
		registry_par->dyn_txbf = (u8)rtw_dyn_txbf;
		registry_par->beamformer_rf_num = (u8)rtw_bfer_rf_number;
		registry_par->beamformee_rf_num = (u8)rtw_bfee_rf_number;
#endif
		rtw_regsty_init_rx_ampdu_sz_limit(registry_par);
	}
#endif

#if 0
int rtw_short_gi = 0xf;
/* BIT0: Enable VHT LDPC Rx, BIT1: Enable VHT LDPC Tx, BIT4: Enable HT LDPC Rx, BIT5: Enable HT LDPC Tx */
int rtw_ldpc_cap = 0x33;
/* BIT0: Enable VHT STBC Rx, BIT1: Enable VHT STBC Tx, BIT4: Enable HT STBC Rx, BIT5: Enable HT STBC Tx */
int rtw_stbc_cap = 0x13;
#endif

#ifdef DBG_LA_MODE
	registry_par->la_mode_en = (u8)rtw_la_mode_en;
#endif
#ifdef CONFIG_NARROWBAND_SUPPORTING
	registry_par->rtw_nb_config = (u8)rtw_nb_config;
#endif

#ifdef CONFIG_80211AC_VHT
	registry_par->vht_enable = (u8)rtw_vht_enable;
	registry_par->vht_24g_enable = (u8)rtw_vht_24g_enable;
	registry_par->ampdu_factor = (u8)rtw_ampdu_factor;
	registry_par->vht_rx_mcs_map[0] = (u8)(rtw_vht_rx_mcs_map & 0xFF);
	registry_par->vht_rx_mcs_map[1] = (u8)((rtw_vht_rx_mcs_map & 0xFF00) >> 8);
#endif

#ifdef CONFIG_80211AX_HE
	registry_par->he_enable = (u8)rtw_he_enable;
#endif

#ifdef CONFIG_TX_EARLY_MODE
	registry_par->early_mode = (u8)rtw_early_mode;
#endif
	registry_par->lowrate_two_xmit = (u8)rtw_lowrate_two_xmit;
	registry_par->rf_path = (u8)rtw_rf_path; /*rf_config/rtw_rf_config*/
	registry_par->tx_nss = (u8)rtw_tx_nss;
	registry_par->rx_nss = (u8)rtw_rx_nss;
	registry_par->low_power = (u8)rtw_low_power;

	registry_par->check_hw_status = (u8)rtw_check_hw_status;

	registry_par->wifi_spec = (u8)rtw_wifi_spec;

#ifdef CONFIG_REGD_SRC_FROM_OS
	if (regd_src_is_valid(rtw_regd_src))
		registry_par->regd_src = (u8)rtw_regd_src;
	else {
		RTW_WARN("%s invalid rtw_regd_src(%u), use REGD_SRC_RTK_PRIV instead\n", __func__, rtw_regd_src);
		registry_par->regd_src = REGD_SRC_RTK_PRIV;
	}
#endif

	registry_par->init_regd_always_apply = !!rtw_init_regd_always_apply;
	registry_par->user_regd_always_apply = !!rtw_user_regd_always_apply;
	rtw_regsty_load_alpha2(registry_par);
	rtw_regsty_load_chplan(registry_par);
	rtw_regsty_load_addl_ch_disable_conf(registry_par);
	registry_par->bcn_hint_valid_ms = rtw_bcn_hint_valid_ms;
#ifdef CONFIG_80211D
	rtw_regsty_load_country_ie_slave_settings(registry_par);
#endif

	registry_par->full_ch_in_p2p_handshake = (u8)rtw_full_ch_in_p2p_handshake;
#ifdef CONFIG_BTC
	registry_par->btcoex = (u8)rtw_btcoex_enable;
	registry_par->bt_iso = (u8)rtw_bt_iso;
	registry_par->bt_sco = (u8)rtw_bt_sco;
	registry_par->bt_ampdu = (u8)rtw_bt_ampdu;
	registry_par->ant_num = (u8)rtw_ant_num;
	registry_par->single_ant_path = (u8) rtw_single_ant_path;
#endif

	registry_par->bAcceptAddbaReq = (u8)rtw_AcceptAddbaReq;

	registry_par->antdiv_cfg = (u8)rtw_antdiv_cfg;
	registry_par->antdiv_type = (u8)rtw_antdiv_type;

	registry_par->drv_ant_band_switch = (u8) rtw_drv_ant_band_switch;

	registry_par->switch_usb_mode = (u8)rtw_switch_usb_mode;

	registry_par->hw_wps_pbc = (u8)rtw_hw_wps_pbc;

#ifdef CONFIG_ADAPTOR_INFO_CACHING_FILE
	snprintf(registry_par->adaptor_info_caching_file_path, PATH_LENGTH_MAX, "%s", rtw_adaptor_info_caching_file_path);
	registry_par->adaptor_info_caching_file_path[PATH_LENGTH_MAX - 1] = 0;
#endif

#ifdef CONFIG_LAYER2_ROAMING
	registry_par->max_roaming_times = (u8)rtw_max_roaming_times;
#endif

	snprintf(registry_par->ifname, 16, "%s", ifname);
	snprintf(registry_par->if2name, 16, "%s", if2name);
#if defined(CONFIG_PLATFORM_ANDROID) && (CONFIG_IFACE_NUMBER > 2)
	snprintf(registry_par->if3name, 16, "%s", if3name);
#endif

#if defined(CONFIG_CONCURRENT_MODE) && !RTW_P2P_GROUP_INTERFACE
#ifdef CONFIG_P2P
	if (CONFIG_RTW_STATIC_NDEV_NUM <= rtw_sel_p2p_iface) {
		RTW_ERR("rtw_sel_p2p_iface out of range\n");
		rtw_sel_p2p_iface = CONFIG_RTW_STATIC_NDEV_NUM - 1;
	}

	registry_par->sel_p2p_iface = (u8)rtw_sel_p2p_iface;
	RTW_INFO("%s, Select P2P interface: iface_id:%d\n", __func__, registry_par->sel_p2p_iface);
#endif
#endif

#ifdef CONFIG_IGNORE_GO_AND_LOW_RSSI_IN_SCAN_LIST
	registry_par->ignore_go_in_scan = (u8)rtw_ignore_go_in_scan;
	registry_par->ignore_low_rssi_in_scan = rtw_ignore_low_rssi_in_scan;
#endif /*CONFIG_IGNORE_GO_AND_LOW_RSSI_IN_SCAN_LIST*/
	registry_par->vo_edca = rtw_vo_edca;

	registry_par->pll_ref_clk_sel = (u8)rtw_pll_ref_clk_sel;

	rtw_regsty_load_target_tx_power(registry_par);

	registry_par->TxBBSwing_2G = (s8)rtw_TxBBSwing_2G;
	registry_par->TxBBSwing_5G = (s8)rtw_TxBBSwing_5G;
	registry_par->bEn_RFE = 1;
	registry_par->RFE_Type = (u8)rtw_rfe_type;
	registry_par->PowerTracking_Type = (u8)rtw_powertracking_type;
	registry_par->AmplifierType_2G = (u8)rtw_amplifier_type_2g;
	registry_par->AmplifierType_5G = (u8)rtw_amplifier_type_5g;
	registry_par->GLNA_Type = (u8)rtw_GLNA_type;
#ifdef CONFIG_LOAD_PHY_PARA_FROM_FILE
	registry_par->load_phy_file = (u8)rtw_load_phy_file;
	registry_par->RegDecryptCustomFile = (u8)rtw_decrypt_phy_file;
#endif
	registry_par->qos_opt_enable = (u8)rtw_qos_opt_enable;

	registry_par->hiq_filter = (u8)rtw_hiq_filter;

	registry_par->boffefusemask = (u8)rtw_OffEfuseMask;
	registry_par->bFileMaskEfuse = (u8)rtw_FileMaskEfuse;
	registry_par->bBTFileMaskEfuse = (u8)rtw_FileMaskEfuse;

#ifdef CONFIG_RTW_ACS
	registry_par->acs_mode = (u8)rtw_acs;
	registry_par->acs_auto_scan = (u8)rtw_acs_auto_scan;
#endif

	registry_par->reg_rxgain_offset_2g = (u32) rtw_rxgain_offset_2g;
	registry_par->reg_rxgain_offset_5gl = (u32) rtw_rxgain_offset_5gl;
	registry_par->reg_rxgain_offset_5gm = (u32) rtw_rxgain_offset_5gm;
	registry_par->reg_rxgain_offset_5gh = (u32) rtw_rxgain_offset_5gh;

#ifdef CONFIG_DFS_MASTER
	rtw_regsty_load_dfs_region_domain_settings(registry_par);
#endif

#ifdef CONFIG_WOWLAN
	registry_par->wowlan_enable = rtw_wow_enable;
	registry_par->wakeup_event = rtw_wakeup_event;
	registry_par->suspend_type = rtw_suspend_type;
#endif

	registry_par->wowlan_sta_mix_mode = rtw_wowlan_sta_mix_mode;

#ifdef CONFIG_PCI_HCI
	registry_par->pci_aspm_config = rtw_pci_aspm_enable;
	registry_par->pci_dynamic_aspm_linkctrl = rtw_pci_dynamic_aspm_linkctrl;
#endif

#ifdef CONFIG_RTW_NAPI
	registry_par->en_napi = (u8)rtw_en_napi;
#ifdef CONFIG_RTW_NAPI_DYNAMIC
	registry_par->napi_threshold = (u32)rtw_napi_threshold;
#endif /* CONFIG_RTW_NAPI_DYNAMIC */
#ifdef CONFIG_RTW_GRO
	registry_par->en_gro = (u8)rtw_en_gro;
	if (!registry_par->en_napi && registry_par->en_gro) {
		registry_par->en_gro = 0;
		RTW_WARN("Disable GRO because NAPI is not enabled\n");
	}
#endif /* CONFIG_RTW_GRO */
#endif /* CONFIG_RTW_NAPI */

	registry_par->iqk_fw_offload = (u8)rtw_iqk_fw_offload;
	registry_par->ch_switch_offload = (u8)rtw_ch_switch_offload;

#ifdef CONFIG_TDLS
	registry_par->en_tdls = rtw_en_tdls;
#endif


#ifdef CONFIG_FW_OFFLOAD_PARAM_INIT
	registry_par->fw_param_init = rtw_fw_param_init;
#endif
#ifdef CONFIG_AP_MODE
	registry_par->bmc_tx_rate = rtw_bmc_tx_rate;
	#if CONFIG_RTW_AP_DATA_BMC_TO_UC
	registry_par->ap_src_b2u_flags = rtw_ap_src_b2u_flags;
	registry_par->ap_fwd_b2u_flags = rtw_ap_fwd_b2u_flags;
	#endif
#endif /* CONFIG_AP_MODE */

#ifdef CONFIG_RTW_MESH
	#if CONFIG_RTW_MESH_DATA_BMC_TO_UC
	registry_par->msrc_b2u_flags = rtw_msrc_b2u_flags;
	registry_par->mfwd_b2u_flags = rtw_mfwd_b2u_flags;
	#endif
#endif /* CONFIG_RTW_MESH */

	registry_par->phydm_ability = rtw_phydm_ability;
	registry_par->halrf_ability = rtw_halrf_ability;
#ifdef CONFIG_RTW_MESH
	registry_par->peer_alive_based_preq = rtw_peer_alive_based_preq;
#endif

#ifdef RTW_BUSY_DENY_SCAN
	registry_par->scan_interval_thr = rtw_scan_interval_thr;
#endif

#ifdef CONFIG_RTW_MULTI_AP
	rtw_regsty_init_unassoc_sta_param(registry_par);
#endif

#ifdef CONFIG_IOCTL_CFG80211
	registry_par->roch_min_home_dur = (u16)rtw_roch_min_home_dur;
	registry_par->roch_max_away_dur = (u16)rtw_roch_max_away_dur;
	registry_par->roch_extend_dur = (u16)rtw_roch_extend_dur;
#endif

#if defined(PRIVATE_R) && defined(CONFIG_P2P)
	registry_par->go_hidden_ssid_mode = rtw_go_hidden_ssid_mode;
	ATOMIC_SET(&registry_par->set_hide_ssid_timer, 0);
	registry_par->p2p_go_skip_keep_alive = _TRUE;
#else
	registry_par->p2p_go_skip_keep_alive = _FALSE;
#endif
	registry_par->amsdu_mode = (u8)rtw_amsdu_mode;

#if CONFIG_DFS
#ifdef CONFIG_ECSA_PHL
	registry_par->en_ecsa = rtw_en_ecsa;
#endif
#endif
	registry_par->scan_pch_ex_time = (u16)rtw_scan_pch_ex;
	return status;
}

static void rtw_cfg_adaptivity_en_msg(void *sel, _adapter *adapter)
{
	struct registry_priv *regsty = &adapter->registrypriv;

	RTW_PRINT_SEL(sel, "RTW_ADAPTIVITY_EN_");

	if (regsty->adaptivity_en == RTW_ADAPTIVITY_EN_DISABLE)
		_RTW_PRINT_SEL(sel, "DISABLE\n");
	else if (regsty->adaptivity_en == RTW_ADAPTIVITY_EN_ENABLE)
		_RTW_PRINT_SEL(sel, "ENABLE\n");
	else if (regsty->adaptivity_en == RTW_ADAPTIVITY_EN_AUTO)
		_RTW_PRINT_SEL(sel, "AUTO\n");
	else
		_RTW_PRINT_SEL(sel, "INVALID\n");
}

static void rtw_cfg_adaptivity_mode_msg(void *sel, _adapter *adapter)
{
	struct registry_priv *regsty = &adapter->registrypriv;

	if (regsty->adaptivity_en != RTW_ADAPTIVITY_EN_ENABLE)
		return;

	RTW_PRINT_SEL(sel, "RTW_ADAPTIVITY_MODE_");

	if (regsty->adaptivity_mode == RTW_ADAPTIVITY_MODE_NORMAL)
		_RTW_PRINT_SEL(sel, "NORMAL\n");
	else if (regsty->adaptivity_mode == RTW_ADAPTIVITY_MODE_CARRIER_SENSE)
		_RTW_PRINT_SEL(sel, "CARRIER_SENSE\n");
	else
		_RTW_PRINT_SEL(sel, "INVALID\n");
}

void rtw_cfg_adaptivity_config_msg(void *sel, _adapter *adapter)
{
	struct registry_priv *regsty = &adapter->registrypriv;
	rtw_cfg_adaptivity_en_msg(sel, adapter);
	rtw_cfg_adaptivity_mode_msg(sel, adapter);
	_RTW_PRINT_SEL(sel, "adaptivity_idle_probability = %u\n", regsty->adaptivity_idle_probability);
}

bool rtw_cfg_adaptivity_needed(_adapter *adapter)
{
	struct registry_priv *regsty = &adapter->registrypriv;
	bool ret = _FALSE;

	if (regsty->adaptivity_en)
		ret = _TRUE;

	return ret;
}


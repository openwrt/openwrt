/** @file */
/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation. All rights reserved.
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
 ******************************************************************************/
#ifndef _MAC_EXP_DEF_H_
#define _MAC_EXP_DEF_H_

// for core layer ref definition defined here
/*--------------------Define -------------------------------------------*/
#define RTW_PHL_PROXY_V4_ADDR_NUM 4
#define RTW_PHL_PROXY_V6_ADDR_NUM 4

#define RTW_PHL_PROXY_MDNS_MAX_MACHINE_NUM 3
#define RTW_PHL_PROXY_MDNS_MAX_MACHINE_LEN 64
#define RTW_PHL_PROXY_MDNS_MAX_DOMAIN_LEN 64
#define RTW_PHL_PROXY_MDNS_MAX_SERVNAME_LEN 86
#define RTW_PHL_PROXY_MDNS_MAX_TARGET_LEN 64
#define RTW_PHL_PROXY_MDNS_MAX_SERV_NUM 10
#define RTW_PHL_PROXY_MDNS_RSP_HDR_LEN sizeof(struct rtw_hal_mac_proxy_mdns_rsp_hdr)
#define RTW_PHL_PROXY_PTCL_PATTERN_MAX_NUM 5
#define RTW_PHL_PROXY_PTCL_PATTERN_MAX_LEN 128

#define RTW_PHL_SOFTAP_MAX_CLIENT_NUM 8

#define MAC_WLANFW_CAP_MAX_SIZE 48
#define MAC_WLANFW_CAP_MAGIC_CODE 0xFCABFCAB

#define MAC_WLANFW_MAC_CAP_SUBID 0xAC
#define MAC_WLANFW_BB_CAP_SUBID 0xBB
#define MAC_WLANFW_RF_CAP_SUBID 0xFF
#define MAC_WLANFW_BTC_CAP_SUBID 0xBC

#define RTW_PHL_WLANFW_MAC_CAP_LEN 4
#define RTW_PHL_WLANFW_BB_CAP_LEN 1
#define RTW_PHL_WLANFW_RF_CAP_LEN 1
#define RTW_PHL_WLANFW_BTC_CAP_LEN 1
/*--------------------Define MACRO--------------------------------------*/
/*--------------------Define Enum---------------------------------------*/
enum rtw_mac_gfunc {
	RTW_MAC_GPIO_WL_PD,
	RTW_MAC_GPIO_BT_PD,
	RTW_MAC_GPIO_WL_EXTWOL,
	RTW_MAC_GPIO_BT_GPIO,
	RTW_MAC_GPIO_WL_SDIO_INT,
	RTW_MAC_GPIO_BT_SDIO_INT,
	RTW_MAC_GPIO_WL_FLASH,
	RTW_MAC_GPIO_BT_FLASH,
	RTW_MAC_GPIO_SIC,
	RTW_MAC_GPIO_LTE_UART,
	RTW_MAC_GPIO_LTE_3W,
	RTW_MAC_GPIO_WL_PTA,
	RTW_MAC_GPIO_BT_PTA,
	RTW_MAC_GPIO_MAILBOX,
	RTW_MAC_GPIO_WL_LED,
	RTW_MAC_GPIO_OSC,
	RTW_MAC_GPIO_XTAL_CLK,
	RTW_MAC_GPIO_EXT_XTAL_CLK,
	RTW_MAC_GPIO_DBG_GNT,
	RTW_MAC_GPIO_WL_RFE_CTRL,
	RTW_MAC_GPIO_BT_UART_RQB,
	RTW_MAC_GPIO_BT_WAKE_HOST,
	RTW_MAC_GPIO_HOST_WAKE_BT,
	RTW_MAC_GPIO_DBG,
	RTW_MAC_GPIO_WL_UART_TX,
	RTW_MAC_GPIO_WL_UART_RX,
	RTW_MAC_GPIO_WL_JTAG,
	RTW_MAC_GPIO_SW_IO,

	/* keep last */
	RTW_MAC_GPIO_LAST,
	RTW_MAC_GPIO_MAX = RTW_MAC_GPIO_LAST,
	RTW_MAC_GPIO_INVALID = RTW_MAC_GPIO_LAST,
	RTW_MAC_GPIO_DFLT = RTW_MAC_GPIO_LAST,
};

/**
 * @enum mac_ax_wow_wake_reason
 *
 * @brief mac_ax_wow_wake_reason
 *
 * @var mac_ax_wow_wake_reason::MAC_AX_WOW_RX_PAIRWISEKEY
 * Please Place Description here.
 * @var mac_ax_wow_wake_reason::MAC_AX_WOW_RX_GTK
 * Please Place Description here.
 * @var mac_ax_wow_wake_reason::MAC_AX_WOW_RX_FOURWAY_HANDSHAKE
 * Please Place Description here.
 * @var mac_ax_wow_wake_reason::MAC_AX_WOW_RX_DISASSOC
 * Please Place Description here.
 * @var mac_ax_wow_wake_reason::MAC_AX_WOW_RX_DEAUTH
 * Please Place Description here.
 * @var mac_ax_wow_wake_reason::MAC_AX_WOW_RX_ARP_REQUEST
 * Please Place Description here.
 * @var mac_ax_wow_wake_reason::MAC_AX_WOW_RX_NS
 * Please Place Description here.
 * @var mac_ax_wow_wake_reason::MAC_AX_WOW_RX_EAPREQ_IDENTIFY
 * Please Place Description here.
 * @var mac_ax_wow_wake_reason::MAC_AX_WOW_FW_DECISION_DISCONNECT
 * Please Place Description here.
 * @var mac_ax_wow_wake_reason::MAC_AX_WOW_RX_MAGIC_PKT
 * Please Place Description here.
 * @var mac_ax_wow_wake_reason::MAC_AX_WOW_RX_UNICAST_PKT
 * Please Place Description here.
 * @var mac_ax_wow_wake_reason::MAC_AX_WOW_RX_PATTERN_PKT
 * Please Place Description here.
 * @var mac_ax_wow_wake_reason::MAC_AX_WOW_RTD3_SSID_MATCH
 * Please Place Description here.
 * @var mac_ax_wow_wake_reason::MAC_AX_WOW_RX_DATA_PKT
 * Please Place Description here.
 * @var mac_ax_wow_wake_reason::MAC_AX_WOW_RX_SSDP_MATCH
 * Please Place Description here.
 * @var mac_ax_wow_wake_reason::MAC_AX_WOW_RX_WSD_MATCH
 * Please Place Description here.
 * @var mac_ax_wow_wake_reason::MAC_AX_WOW_RX_SLP_MATCH
 * Please Place Description here.
 * @var mac_ax_wow_wake_reason::MAC_AX_WOW_RX_LLTD_MATCH
 * Please Place Description here.
 * @var mac_ax_wow_wake_reason::MAC_AX_WOW_RX_MDNS_MATCH
 * Please Place Description here.
 * @var mac_ax_wow_wake_reason::MAC_AX_WOW_RX_REALWOW_V2_WAKEUP_PKT
 * Please Place Description here.
 * @var mac_ax_wow_wake_reason::MAC_AX_WOW_RX_REALWOW_V2_ACK_LOST
 * Please Place Description here.
 * @var mac_ax_wow_wake_reason::MAC_AX_WOW_RX_REALWOW_V2_TX_KAPKT
 * Please Place Description here.
 * @var mac_ax_wow_wake_reason::MAC_AX_WOW_ENABLE_FAIL_DMA_IDLE
 * Please Place Description here.
 * @var mac_ax_wow_wake_reason::MAC_AX_WOW_ENABLE_FAIL_DMA_PAUSE
 * Please Place Description here.
 * @var mac_ax_wow_wake_reason::MAC_AX_WOW_RTIME_FAIL_DMA_IDLE
 * Please Place Description here.
 * @var mac_ax_wow_wake_reason::MAC_AX_WOW_RTIME_FAIL_DMA_PAUSE
 * Please Place Description here.
 * @var mac_ax_wow_wake_reason::MAC_AX_WOW_RX_SNMP_MISMATCHED_PKT
 * Please Place Description here.
 * @var mac_ax_wow_wake_reason::MAC_AX_WOW_RX_DESIGNATED_MAC_PKT
 * Please Place Description here.
 * @var mac_ax_wow_wake_reason::MAC_AX_WOW_NLO_SSID_MACH
 * Please Place Description here.
 * @var mac_ax_wow_wake_reason::MAC_AX_WOW_AP_OFFLOAD_WAKEUP
 * Please Place Description here.
 * @var mac_ax_wow_wake_reason::MAC_AX_WOW_DMAC_ERROR_OCCURRED
 * Please Place Description here.
 * @var mac_ax_wow_wake_reason::MAC_AX_WOW_EXCEPTION_OCCURRED
 * Please Place Description here.
 * @var mac_ax_wow_wake_reason::MAC_AX_WOW_CLK_32K_UNLOCK
 * Please Place Description here.
 * @var mac_ax_wow_wake_reason::MAC_AX_WOW_CLK_32K_LOCK
 * Please Place Description here.
 */
enum mac_ax_wow_wake_reason {
	MAC_AX_WOW_RX_PAIRWISEKEY = 0x01,
	MAC_AX_WOW_RX_GTK = 0x02,
	MAC_AX_WOW_RX_FOURWAY_HANDSHAKE = 0x03,
	MAC_AX_WOW_RX_DISASSOC = 0x04,
	MAC_AX_WOW_RX_DEAUTH = 0x08,
	MAC_AX_WOW_RX_ARP_REQUEST = 0x09,
	MAC_AX_WOW_RX_NS = 0x0A,
	MAC_AX_WOW_RX_EAPREQ_IDENTIFY = 0x0B,
	MAC_AX_WOW_FW_DECISION_DISCONNECT = 0x10,
	MAC_AX_WOW_RX_MAGIC_PKT = 0x21,
	MAC_AX_WOW_RX_UNICAST_PKT = 0x22,
	MAC_AX_WOW_RX_PATTERN_PKT = 0x23,
	MAC_AX_WOW_RTD3_SSID_MATCH = 0x24,
	MAC_AX_WOW_RX_DATA_PKT = 0x25,
	MAC_AX_WOW_RX_SSDP_MATCH = 0x26,
	MAC_AX_WOW_RX_WSD_MATCH = 0x27,
	MAC_AX_WOW_RX_SLP_MATCH = 0x28,
	MAC_AX_WOW_RX_LLTD_MATCH = 0x29,
	MAC_AX_WOW_RX_MDNS_MATCH = 0x2A,
	MAC_AX_WOW_RX_REALWOW_V2_WAKEUP_PKT = 0x30,
	MAC_AX_WOW_RX_REALWOW_V2_ACK_LOST = 0x31,
	MAC_AX_WOW_RX_REALWOW_V2_TX_KAPKT = 0x32,
	MAC_AX_WOW_ENABLE_FAIL_DMA_IDLE = 0x40,
	MAC_AX_WOW_ENABLE_FAIL_DMA_PAUSE = 0x41,
	MAC_AX_WOW_RTIME_FAIL_DMA_IDLE = 0x42,
	MAC_AX_WOW_RTIME_FAIL_DMA_PAUSE = 0x43,
	MAC_AX_WOW_RX_SNMP_MISMATCHED_PKT = 0x50,
	MAC_AX_WOW_RX_DESIGNATED_MAC_PKT = 0x51,
	MAC_AX_WOW_NLO_SSID_MACH = 0x55,
	MAC_AX_WOW_AP_OFFLOAD_WAKEUP = 0x66,
	MAC_AX_WOW_DMAC_ERROR_OCCURRED = 0x70,
	MAC_AX_WOW_EXCEPTION_OCCURRED = 0x71,
	MAC_AX_WOW_L0_TO_L1_ERROR_OCCURRED = 0x72,
	MAC_AX_WOW_ASSERT_OCCURRED = 0x73,
	MAC_AX_WOW_L2_ERROR_OCCURRED = 0x74,
	MAC_AX_WOW_WDT_TIMEOUT_WAKE = 0x75,
	MAC_AX_WOW_RX_ACTION = 0xD0,
	MAC_AX_WOW_CLK_32K_UNLOCK = 0xFD,
	MAC_AX_WOW_CLK_32K_LOCK = 0xFE
};

enum rtw_mac_wow_wake_reason {
	RTW_MAC_WOW_UNKNOWN = 0x00,
	RTW_MAC_WOW_RX_PAIRWISEKEY = 0x01,
	RTW_MAC_WOW_RX_GTK = 0x02,
	RTW_MAC_WOW_RX_FOURWAY_HANDSHAKE = 0x03,
	RTW_MAC_WOW_RX_DISASSOC = 0x04,
	RTW_MAC_WOW_RX_DEAUTH = 0x08,
	RTW_MAC_WOW_RX_ARP_REQUEST = 0x09,
	RTW_MAC_WOW_RX_NS = 0x0A,
	RTW_MAC_WOW_RX_EAPREQ_IDENTIFY = 0x0B,
	RTW_MAC_WOW_FW_DECISION_DISCONNECT = 0x10,
	RTW_MAC_WOW_RX_MAGIC_PKT = 0x21,
	RTW_MAC_WOW_RX_UNICAST_PKT = 0x22,
	RTW_MAC_WOW_RX_PATTERN_PKT = 0x23,
	RTW_MAC_WOW_RTD3_SSID_MATCH = 0x24,
	RTW_MAC_WOW_RX_DATA_PKT = 0x25,
	RTW_MAC_WOW_RX_SSDP_MATCH = 0x26,
	RTW_MAC_WOW_RX_WSD_MATCH = 0x27,
	RTW_MAC_WOW_RX_SLP_MATCH = 0x28,
	RTW_MAC_WOW_RX_LLTD_MATCH = 0x29,
	RTW_MAC_WOW_RX_MDNS_MATCH = 0x2A,
	RTW_MAC_WOW_RX_REALWOW_V2_WAKEUP_PKT = 0x30,
	RTW_MAC_WOW_RX_REALWOW_V2_ACK_LOST = 0x31,
	RTW_MAC_WOW_RX_REALWOW_V2_TX_KAPKT = 0x32,
	RTW_MAC_WOW_ENABLE_FAIL_DMA_IDLE = 0x40,
	RTW_MAC_WOW_ENABLE_FAIL_DMA_PAUSE = 0x41,
	RTW_MAC_WOW_RTIME_FAIL_DMA_IDLE = 0x42,
	RTW_MAC_WOW_RTIME_FAIL_DMA_PAUSE = 0x43,
	RTW_MAC_WOW_RX_SNMP_MISMATCHED_PKT = 0x50,
	RTW_MAC_WOW_RX_DESIGNATED_MAC_PKT = 0x51,
	RTW_MAC_WOW_NLO_SSID_MACH = 0x55,
	RTW_MAC_WOW_AP_OFFLOAD_WAKEUP = 0x66,
	RTW_MAC_WOW_DMAC_ERROR_OCCURRED = 0x70,
	RTW_MAC_WOW_EXCEPTION_OCCURRED = 0x71,
	RTW_MAC_WOW_L0_TO_L1_ERROR_OCCURRED = 0x72,
	RTW_MAC_WOW_ASSERT_OCCURRED = 0x73,
	RTW_MAC_WOW_L2_ERROR_OCCURRED = 0x74,
	RTW_MAC_WOW_WDT_TIMEOUT_WAKE = 0x75,
	RTW_MAC_WOW_NO_WAKE_RX_PAIRWISEKEY = 0xB0,
	RTW_MAC_WOW_NO_WAKE_RX_GTK = 0xB1,
	RTW_MAC_WOW_NO_WAKE_RX_DISASSOC = 0xB2,
	RTW_MAC_WOW_NO_WAKE_RX_DEAUTH = 0xB3,
	RTW_MAC_WOW_NO_WAKE_RX_EAPREQ_IDENTIFY = 0xB4,
	RTW_MAC_WOW_NO_WAKE_FW_DECISION_DISCONNECT = 0xB5,
	RTW_MAC_WOW_RX_ACTION = 0xD0,
	RTW_MAC_WOW_CLK_32K_UNLOCK = 0xFD,
	RTW_MAC_WOW_CLK_32K_LOCK = 0xFE
};

enum rtw_mac_proxy_pattern_ptcl {
	RTW_MAC_PROXY_PATTERN_SSDP = 0x0,
	RTW_MAC_PROXY_PATTERN_WSD = 0x1,
	RTW_MAC_PROXY_PATTERN_SLP = 0x2,
	RTW_MAC_PROXY_PATTERN_MAX
};

/*for device environment*/
enum rtw_mac_env_mode {
	DUT_ENV_ASIC = 0,
	DUT_ENV_FPGA = 1,
	DUT_ENV_PXP = 2,
};

/*--------------------Define Struct-------------------------------------*/
struct mac_ser_status {
	u32 l0_cnt:8;
	u32 l1_cnt:4;
	u32 l0_pro_event:4;
	u32 rmac_ppdu_hang_cnt:8;
	u32 rsvd:8;
};

struct hal_txmap_cfg {
	u32 macid:8;
	u32 n_tx_en:4;
	u32 map_a:2;
	u32 map_b:2;
	u32 map_c:2;
	u32 map_d:2;
	u32 rsvd:12;
};

struct rtw_phl_ax_ru_rate_ent {
	u8 dcm:1;
	u8 ss:3;
	u8 mcs:4;
};

struct rtw_phl_ax_rura_report {
	u8 rt_tblcol: 6;
	u8 prtl_alloc: 1;
	u8 rate_chg: 1;
};

struct rtw_phl_ax_ulru_out_sta_ent {
	u8 dropping: 1;
	u8 tgt_rssi: 7;
	u8 mac_id;
	u8 ru_pos;
	u8 coding: 1;
	u8 vip_flag: 1;
	u8 rsvd1: 6;
	u16 bsr_length: 15;
	u16 rsvd2: 1;
	struct rtw_phl_ax_ru_rate_ent rate;
	struct rtw_phl_ax_rura_report rpt;
};

struct  rtw_phl_ax_tbl_hdr {
	u8 rw:1;
	u8 idx:7;
	u16 offset:5;
	u16 len:10;
	u16 type:1;
};

#define RTW_PHL_MAX_RU_NUM 8
struct  rtw_phl_ax_ulrua_output {
	u8 ru2su: 1;
	u8 ppdu_bw: 2;
	u8 gi_ltf: 3;
	u8 stbc: 1;
	u8 doppler: 1;
	u8 n_ltf_and_ma: 3;
	u8 sta_num: 4;
	u8 rsvd1: 1;
	u16 rf_gain_fix: 1;
	u16 rf_gain_idx: 10;
	u16 tb_t_pe_nom: 2;
	u16 rsvd2: 3;

	u32 grp_mode: 1;
	u32 grp_id: 6;
	u32 fix_mode: 1;
	u32 rsvd3: 24;
	struct  rtw_phl_ax_ulru_out_sta_ent sta[RTW_PHL_MAX_RU_NUM];
};

struct rtw_phl_ul_macid_info {
	u8 macid;
	u8 pref_AC:2;
	u8 ul_mu_dis:1;
	u8 rsvd:5;
};

struct  rtw_phl_ul_mode_cfg {
	u32 mode:2; /* 0: peoridic ; 1: normal ; 2: non_tgr; 3 tf_peoridic;*/
	u32 interval:6; /* unit: sec */
	u32 bsr_thold:8;
	u32 storemode:2;
	u32 rsvd:14;
};

struct  rtw_phl_ax_ul_fixinfo {
	struct  rtw_phl_ax_tbl_hdr tbl_hdr;
	struct  rtw_phl_ul_mode_cfg cfg;

	u32 ndpa_dur:16;
	u32 tf_type:3;
	u32 sig_ta_pkten:1;
	u32 sig_ta_pktsc:4;
	u32 murts_flag:1;
	u32 ndpa:2;
	u32 snd_pkt_sel:2;
	u32 gi_ltf:3;

	u32 data_rate:9;
	u32 data_er:1;
	u32 data_bw:2;
	u32 data_stbc:2;
	u32 data_ldpc:1;
	u32 data_dcm:1;
	u32 apep_len:12;
	u32 more_tf:1;
	u32 data_bw_er:1;
	u32 istwt:1;
	u32 ul_logo_test:1;

	u32 multiport_id:3;
	u32 mbssid:4;
	u32 txpwr_mode:3;
	u32 ulfix_usage:3;
	u32 twtgrp_stanum_sel:2;
	u32 store_idx:4;
	u32 rsvd1:13;
	struct  rtw_phl_ul_macid_info sta[RTW_PHL_MAX_RU_NUM];
	struct  rtw_phl_ax_ulrua_output ulrua;
};

struct rtw_hal_mac_ax_cctl_info {
	/* dword 0 */
	u32 datarate:9;
	u32 force_txop:1;
	u32 data_bw:2;
	u32 data_gi_ltf:3;
	u32 darf_tc_index:1;
	u32 arfr_ctrl:4;
	u32 acq_rpt_en:1;
	u32 mgq_rpt_en:1;
	u32 ulq_rpt_en:1;
	u32 twtq_rpt_en:1;
	u32 rsvd0:1;
	u32 disrtsfb:1;
	u32 disdatafb:1;
	u32 tryrate:1;
	u32 ampdu_density:4;
	/* dword 1 */
	u32 data_rty_lowest_rate:9;
	u32 ampdu_time_sel:1;
	u32 ampdu_len_sel:1;
	u32 rts_txcnt_lmt_sel:1;
	u32 rts_txcnt_lmt:4;
	u32 rtsrate:9;
	u32 rsvd1:2;
	u32 vcs_stbc:1;
	u32 rts_rty_lowest_rate:4;
	/* dword 2 */
	u32 data_tx_cnt_lmt:6;
	u32 data_txcnt_lmt_sel:1;
	u32 max_agg_num_sel:1;
	u32 rts_en:1;
	u32 cts2self_en:1;
	u32 cca_rts:2;
	u32 hw_rts_en:1;
	u32 rts_drop_data_mode:2;
	u32 preld_en:1;
	u32 ampdu_max_len:11;
	u32 ul_mu_dis:1;
	u32 ampdu_max_time:4;
	/* dword 3 */
	u32 max_agg_num:9;
	u32 ba_bmap:2;
	u32 rsvd3:5;
	u32 vo_lftime_sel:3;
	u32 vi_lftime_sel:3;
	u32 be_lftime_sel:3;
	u32 bk_lftime_sel:3;
	u32 sectype:4;
	/* dword 4 */
	u32 multi_port_id:3;
	u32 bmc:1;
	u32 mbssid:4;
	u32 navusehdr:1;
	u32 txpwr_mode:3;
	u32 data_dcm:1;
	u32 data_er:1;
	u32 data_ldpc:1;
	u32 data_stbc:1;
	u32 a_ctrl_bqr:1;
	u32 a_ctrl_uph:1;
	u32 a_ctrl_bsr:1;
	u32 a_ctrl_cas:1;
	u32 data_bw_er:1;
	u32 lsig_txop_en:1;
	u32 rsvd4:5;
	u32 ctrl_cnt_vld:1;
	u32 ctrl_cnt:4;
	/* dword 5 */
	u32 resp_ref_rate:9;
	u32 rsvd5:3;
	u32 all_ack_support:1;
	u32 bsr_queue_size_format:1;
	u32 bsr_om_upd_en:1;
	u32 macid_fwd_idc:1;
	u32 ntx_path_en:4;
	u32 path_map_a:2;
	u32 path_map_b:2;
	u32 path_map_c:2;
	u32 path_map_d:2;
	u32 antsel_a:1;
	u32 antsel_b:1;
	u32 antsel_c:1;
	u32 antsel_d:1;
	/* dword 6 */
	u32 addr_cam_index:8;
	u32 paid:9;
	u32 uldl:1;
	u32 doppler_ctrl:2;
	u32 nominal_pkt_padding:2;
	u32 nominal_pkt_padding40:2;
	u32 txpwr_tolerence:6;
	/*u32 rsvd9:2;*/
	u32 nominal_pkt_padding80:2;
	/* dword 7 */
	u32 nc:3;
	u32 nr:3;
	u32 ng:2;
	u32 cb:2;
	u32 cs:2;
	u32 csi_txbf_en:1;
	u32 csi_stbc_en:1;
	u32 csi_ldpc_en:1;
	u32 csi_para_en:1;
	u32 csi_fix_rate:9;
	u32 csi_gi_ltf:3;
	u32 nominal_pkt_padding160:2;
	u32 csi_bw:2;
};

/**
 * @struct mac_ax_fw_log
 * @brief mac_ax_fw_log
 *
 * @var mac_ax_fw_log::level
 * Please Place Description here.
 * @var mac_ax_fw_log::output
 * Please Place Description here.
 * @var mac_ax_fw_log::comp
 * Please Place Description here.
 * @var mac_ax_fw_log::comp_ext
 * Please Place Description here.
 */
struct mac_ax_fw_log {
#define MAC_AX_FL_LV_OFF 0
#define MAC_AX_FL_LV_CRT 1
#define MAC_AX_FL_LV_SER 2
#define MAC_AX_FL_LV_WARN 3
#define MAC_AX_FL_LV_LOUD 4
#define MAC_AX_FL_LV_TR 5
	u32 level;
#define MAC_AX_FL_LV_UART BIT(0)
#define MAC_AX_FL_LV_C2H BIT(1)
#define MAC_AX_FL_LV_SNI BIT(2)
	u32 output;
#define MAC_AX_FL_COMP_VER BIT(0)
#define MAC_AX_FL_COMP_INIT BIT(1)
#define MAC_AX_FL_COMP_TASK BIT(2)
#define MAC_AX_FL_COMP_CNS BIT(3)
#define MAC_AX_FL_COMP_H2C BIT(4)
#define MAC_AX_FL_COMP_C2H BIT(5)
#define MAC_AX_FL_COMP_TX BIT(6)
#define MAC_AX_FL_COMP_RX BIT(7)
#define MAC_AX_FL_COMP_IPSEC BIT(8)
#define MAC_AX_FL_COMP_TIMER BIT(9)
#define MAC_AX_FL_COMP_DBGPKT BIT(10)
#define MAC_AX_FL_COMP_PS BIT(11)
#define MAC_AX_FL_COMP_ERROR BIT(12)
#define MAC_AX_FL_COMP_WOWLAN BIT(13)
#define MAC_AX_FL_COMP_SECURE_BOOT BIT(14)
#define MAC_AX_FL_COMP_BTC BIT(15)
#define MAC_AX_FL_COMP_BB BIT(16)
#define MAC_AX_FL_COMP_TWT BIT(17)
#define MAC_AX_FL_COMP_RF BIT(18)
#define MAC_AX_FL_COMP_MCC BIT(20)
	u32 comp;
	u32 comp_ext;
};

#pragma pack(push)
#pragma pack(1)

struct rtw_hal_mac_proxyofld {
	u8 proxy_en:1;
	u8 arp_rsp:1;
	u8 ns_rsp:1;
	u8 icmp_v4_rsp:1;
	u8 icmp_v6_rsp:1;
	u8 netbios_rsp:1;
	u8 llmnr_v4_rsp:1;
	u8 llmnr_v6_rsp:1;
	u8 snmp_v4_rsp:1;
	u8 snmp_v6_rsp:1;
	u8 snmp_v4_wake:1;
	u8 snmp_v6_wake:1;
	u8 ssdp_v4_wake:1;
	u8 ssdp_v6_wake:1;
	u8 wsd_v4_wake:1;
	u8 wsd_v6_wake:1;
	u8 slp_v4_wake:1;
	u8 slp_v6_wake:1;
	u8 mdns_v4_rsp:1;
	u8 mdns_v6_rsp:1;
	u8 target_mac_wake:1;
	u8 lltd_wake:1;
	u8 mdns_v4_wake:1;
	u8 mdns_v6_wake:1;
	u8 rsvd0;
	u8 v4addr[RTW_PHL_PROXY_V4_ADDR_NUM][4];
	u8 v6addr[RTW_PHL_PROXY_V6_ADDR_NUM][16];
};

struct rtw_hal_mac_proxy_mdns_machine {
	u32 len;
	u8 name[RTW_PHL_PROXY_MDNS_MAX_MACHINE_LEN];
};

struct rtw_hal_mac_proxy_mdns_rsp_hdr {
	u8 rspTypeB0;
	u8 rspTypeB1;
	u8 cache_class_B0;
	u8 cache_class_B1;
	u32 ttl;
	u16 dataLen;
};

struct rtw_hal_mac_proxy_mdns_a {
	struct rtw_hal_mac_proxy_mdns_rsp_hdr hdr;
	u8 ipv4Addr[4];
};

struct rtw_hal_mac_proxy_mdns_aaaa {
	struct rtw_hal_mac_proxy_mdns_rsp_hdr hdr;
	u8 ipv6Addr[16];
};

struct rtw_hal_mac_proxy_mdns_ptr {
	struct rtw_hal_mac_proxy_mdns_rsp_hdr hdr;
	u8 domain[RTW_PHL_PROXY_MDNS_MAX_DOMAIN_LEN];
	u8 compression;
	u8 compression_loc;
};

struct rtw_hal_mac_proxy_mdns {
	u8 ipv4_pktid;
	u8 ipv6_pktid;
	u8 num_supported_services;
	u8 num_machine_names;
	u8 macid;
	u8 serv_pktid[RTW_PHL_PROXY_MDNS_MAX_SERV_NUM];
	u8 rsvd;
	struct rtw_hal_mac_proxy_mdns_machine machines[RTW_PHL_PROXY_MDNS_MAX_MACHINE_NUM];
	struct rtw_hal_mac_proxy_mdns_a a_rsp;
	struct rtw_hal_mac_proxy_mdns_aaaa aaaa_rsp;
	struct rtw_hal_mac_proxy_mdns_ptr ptr_rsp;
};

struct rtw_hal_mac_proxy_mdns_txt {
	struct rtw_hal_mac_proxy_mdns_rsp_hdr hdr;
	u16 content_len;
	u8 *content;
};

struct rtw_hal_mac_proxy_mdns_service {
	u8 name_len;
	u8 *name; //should contains 1 byte of delimiter at the end
	struct rtw_hal_mac_proxy_mdns_rsp_hdr hdr;
	u16 priority;
	u16 weight;
	u16 port;
	u8 target_len;
	u8 *target;
	u8 compression;
	u8 compression_loc;
	u8 has_txt;
	u8 txt_pktid;
	u8 txt_id;
};

struct rtw_hal_mac_proxy_ptcl_pattern {
	u8 macid;
	u8 num_pattern;
	enum rtw_mac_proxy_pattern_ptcl ptcl;
	u8 pattern_len[RTW_PHL_PROXY_PTCL_PATTERN_MAX_NUM];
	u8 patterns[RTW_PHL_PROXY_PTCL_PATTERN_MAX_NUM][RTW_PHL_PROXY_PTCL_PATTERN_MAX_LEN];
};

#pragma pack(pop)

struct rtw_hal_mac_sensing_csi_param{
	u8 macid;
	u8 en:1;
	u8 rsvd:7;
	u16 period; // Unit: ms
	u8 retry_cnt;
	u16 rate; // AX use [8:0], BE use [10:0]
	u8 pkt_num;
	u8 *pkt_id; // Array of packet offload id
};

struct rtw_hal_mac_sensing_csi_tx_rpt{
	u8 result;
	u8 rsvd[3];
};

struct rtw_hal_mac_sensing_csi_tx_result{
	u8 macid;
	u8 num; // Number of result
	u8 seq_num;
	u8 rsvd;
	struct rtw_hal_mac_sensing_csi_tx_rpt tx_rpt[RTW_PHL_SOFTAP_MAX_CLIENT_NUM];
};

struct rtw_wcpu_mac_cap_t {
	/* ---- dword 0 ---- */
	u32 io_offload: 1;
	u32 chsw_offload: 1;
	u32 lps_pg: 1;
	u32 twt_sta: 1;
	u32 nan: 1;
	u32 ftm: 1;
	u32 scan_offload: 1;
	u32 sensing_csi: 1;
	u32 rsvd0: 24;
	/* ---- dword 1 ---- */
	u32 rsvd1: 32;
	/* ---- dword 2 ---- */
	u32 rsvd2: 32;
	/* ---- dword 3 ---- */
	u32 rsvd3: 32;
};

struct rtw_wcpu_cap_t {
	struct rtw_wcpu_mac_cap_t mac_ofld_cap;
	u32 bb_ofld_cap[RTW_PHL_WLANFW_BB_CAP_LEN];
	u32 rf_ofld_cap[RTW_PHL_WLANFW_RF_CAP_LEN];
	u32 btc_ofld_cap[RTW_PHL_WLANFW_BTC_CAP_LEN];
};

#pragma pack(push)
#pragma pack(1)
struct mac_wlanfw_cap_hdr {
	u32 magic_code;
	u8 num_mods;
};

#pragma pack(pop)
#endif

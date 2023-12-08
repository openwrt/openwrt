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
#ifndef _HAL_DEF_H_
#define _HAL_DEF_H_

#define halcom_to_drvpriv(_hcom)	(_hcom->drv_priv)
#define hal_get_chip_id(_halcom) (_halcom->chip_id)

#define MAX_WD_LEN		(48)
#define MAX_WD_BODY_LEN (24)
#define MAX_BAENTRY		16


enum hal_path {
	PATH_NON = 0,
	PATH_A = 0x00000001,
	PATH_B = 0x00000002,
	PATH_C = 0x00000004,
	PATH_D = 0x00000008,

	PATH_AB = (PATH_A | PATH_B),
	PATH_AC = (PATH_A | PATH_C),
	PATH_AD = (PATH_A | PATH_D),
	PATH_BC = (PATH_B | PATH_C),
	PATH_BD = (PATH_B | PATH_D),
	PATH_CD = (PATH_C | PATH_D),

	PATH_ABC = (PATH_A | PATH_B | PATH_C),
	PATH_ABD = (PATH_A | PATH_B | PATH_D),
	PATH_ACD = (PATH_A | PATH_C | PATH_D),
	PATH_BCD = (PATH_B | PATH_C | PATH_D),

	PATH_ABCD = (PATH_A | PATH_B | PATH_C | PATH_D),
	PATH_AUTO = 0xff /*for auto path selection*/
};

enum HAL_CMD_ID {
	HAL_HELP,
	MAC_DD_DBG,
	MAC_DUMP_SEC_CAM_TBL
};

struct hal_cmd_info {
	char name[16];
	u8 id;
};

static const struct hal_cmd_info hal_cmd_i[] = {
	{"-h", HAL_HELP},
	{"dd_dbg", MAC_DD_DBG},/*@do not move this element to other position*/
	{"sec_cam_tbl", MAC_DUMP_SEC_CAM_TBL}
};

enum rtw_hal_ser_rsn {
	HAL_SER_RSN_NONE = 0,
	HAL_SER_RSN_WOW,
	HAL_SER_RSN_RFK
};

enum rtw_hal_status {
	RTW_HAL_STATUS_SUCCESS, /* 0 */
	RTW_HAL_STATUS_FAILURE, /* 1 */
	RTW_HAL_STATUS_RESOURCE, /* 2 */
	RTW_HAL_STATUS_IO_INIT_FAILURE, /* 3 */
	RTW_HAL_STATUS_MAC_INIT_FAILURE, /* 4 */
	RTW_HAL_STATUS_BB_INIT_FAILURE, /* 5 */
	RTW_HAL_STATUS_RF_INIT_FAILURE, /* 6 */
	RTW_HAL_STATUS_BTC_INIT_FAILURE, /* 7 */
	RTW_HAL_STATUS_HAL_INIT_FAILURE, /* 8 */
	RTW_HAL_STATUS_EFUSE_UNINIT, /* 9 */
	RTW_HAL_STATUS_EFUSE_IVALID_OFFSET, /* 10 */
	RTW_HAL_STATUS_EFUSE_PG_FAIL, /* 11 */
	RTW_HAL_STATUS_MAC_API_FAILURE, /* 12 */
	RTW_HAL_STATUS_BB_CH_INFO_LAST_SEG, /*13*/
	RTW_HAL_STATUS_UNKNOWN_RFE_TYPE, /* 14 */
	RTW_HAL_STATUS_TIMEOUT, /* 15 */
};

#define FW_FILE_NIC_POSTFIX ""
#define FW_FILE_WOWLAN_POSTFIX "_wowlan"
#define FW_FILE_SPIC_POSTFIX "_spic"
#define FW_FILE_AP_POSTFIX "_ap"

enum rtw_fw_type {
	RTW_FW_NIC, /* 1 */
	RTW_FW_WOWLAN, /* 2 */
	RTW_FW_AP, /* 3 */
	RTW_FW_ROM, /* 4 */
	RTW_FW_SPIC, /* 5 */
	RTW_FW_VRAP, /* 6 */
	RTW_FW_NIC_CE, /* 7 */
	RTW_FW_MAX
};

enum _rtw_hal_query_info {
	RTW_HAL_RXDESC_SIZE,
};

enum rtw_h2c_pkt_type {
	H2CB_TYPE_CMD = 0,
	H2CB_TYPE_DATA = 1,
	H2CB_TYPE_LONG_DATA = 2,
	H2CB_TYPE_MAX = 0x3
};

enum tx_pause_rson {
	PAUSE_RSON_NOR_SCAN, /*normal scan*/
	PAUSE_RSON_UNSPEC_BY_MACID, /*P2P_SCAN*/
	PAUSE_RSON_RFK,
	PAUSE_RSON_PSD,
	PAUSE_RSON_DFS,
	PAUSE_RSON_DFS_CSA, /* allow beacon only */
	PAUSE_RSON_DFS_CSA_MG, /* allow beacon and mgnt frame */
	PAUSE_RSON_DFS_CAC,
	PAUSE_RSON_DBCC,
	PAUSE_RSON_RESET,
	PAUSE_RSON_MAX
};

enum rtw_hal_config_int {
	RTW_HAL_EN_DEFAULT_INT,
	RTW_HAL_DIS_DEFAULT_INT,
	RTW_HAL_STOP_RX_INT,
	RTW_HAL_RESUME_RX_INT,
	RTW_HAL_SER_HANDSHAKE_MODE,
	RTW_HAL_EN_HCI_INT,
	RTW_HAL_DIS_HCI_INT,
	RTW_HAL_CONFIG_INT_MAX
};

enum hal_mp_efuse_type {
	HAL_MP_EFUSE_WIFI = 0,
	HAL_MP_EFUSE_BT,
	HAL_MP_EFUSE_NONE,
};

struct rtw_g6_h2c_hdr {
	u8 h2c_class; //0x0~0x7: Phydm; 0x8~0xF: RF; 0x10~0x17: BTC
	u8 h2c_func;
	u8 seq_valid:1;
	u8 seq:3;
	u8 seq_stop:1;
	enum rtw_h2c_pkt_type type; //0:cmd ; 1:cmd+data ; 2:cmd+long data
	u16 content_len:12;
	u16 rec_ack:1; //Ack when receive H2C
	u16 done_ack:1; //Ack when FW execute H2C cmd done
	u16 rsvd2:2;
};

/**
 * @c2h_cat: target category of this c2h / c2h ack
 * @c2h_class: target class of this c2h / c2h ack
 * @c2h_func: target function of this c2h / c2h ack
 * @type_rec_ack: set 1 as a receive ack to this c2h
 * @type_done_ack: set 1 as a done ack to this c2h
 * @h2c_return: status code of done ack responding to h2c
 *
 */
struct rtw_c2h_info {
	u8 c2h_cat;
	u8 c2h_class;
	u8 c2h_func;
	u8 type_rec_ack:1;
	u8 type_done_ack:1;
	u8 rsvd:6;
	u8 h2c_return;/*H2C return value, 0 = success*/
	u8 h2c_seq;
	u16 content_len;
	u8 *content;
};


#define RTW_BTC_OVERWRITE_BUF_LEN 10
struct hal_bt_msg {
	_os_lock lock;
	u16 len;
	u16 cnt; /* update cnt */
	u8 latest[RTW_BTC_OVERWRITE_BUF_LEN];
	u8 working[RTW_BTC_OVERWRITE_BUF_LEN];
};

struct btc_fw_msg {
	_os_lock lock;
	u32 fev_cnt; /* fw event cnt, need to be protected by lock */

	struct hal_bt_msg btinfo;
	struct hal_bt_msg scbd;

	/* common fwinfo queue */
	struct phl_queue idleq;
	struct phl_queue waitq;
};

struct btc_ctrl_t {
	u8 lps;
	u8 tx_time;
	u8 tx_retry;
	u8 disable_rx_stbc;
};

/*except version*/
struct ver_ctrl_t {
	u64 mac_ver;
	u64 bb_ver;
	u64 rf_ver;
	u64 btc_ver;
	u64 fw_ver;
};


struct hal_mu_score_tbl_ctrl {
	u32 mu_sc_thr:2;
	u32 mu_opt:1;
	u32 rsvd:29;
};

#define HAL_MAX_MU_SCORE_SIZE 8 /* Unit: Byte */
struct hal_mu_score_tbl_score {
	u8 valid;
	u16 macid;
	u8 score[HAL_MAX_MU_SCORE_SIZE]; /*by case: [1:0], [3:2], ..... */
};

#define HAL_MAX_MU_STA_NUM 6
struct hal_mu_score_tbl {
	struct hal_mu_score_tbl_ctrl mu_ctrl;
	_os_lock lock;
	u8 num_mu_sta; /*By IC, shall alway <= than HAL_MAX_MU_STA_NUM , 0 = tbl invalid */
	u8 sz_mu_score; /*By IC, shall alway <= than HAL_MAX_MU_SCORE_SIZE , 0 = score invalid */
	struct hal_mu_score_tbl_score mu_score[HAL_MAX_MU_STA_NUM]; /* mu_score[num_mu_sta] */
};


#define HAL_MAX_VHT_BFRP_NUM 3
#define HAL_MAX_HE_BFRP_NUM 2
#define HLA_MAX_BFRP_NUM ((HAL_MAX_VHT_BFRP_NUM > HAL_MAX_HE_BFRP_NUM) ?\
			  HAL_MAX_VHT_BFRP_NUM : HAL_MAX_HE_BFRP_NUM)
#define HAL_MAX_VHT_SND_STA_NUM HAL_MAX_VHT_BFRP_NUM + 1
#define HAL_MAX_HE_BFRP_STA_NUM 4
#define HAL_MAX_HE_SND_STA_NUM HAL_MAX_HE_BFRP_NUM * HAL_MAX_HE_BFRP_STA_NUM

struct hal_frame_hdr {
	u16 frame_ctl;
	u16 duration;
	u8 addr1[MAC_ALEN];
	u8 addr2[MAC_ALEN];
};
struct hal_npda_dialog_token {
	u8 rsvd:1;
	u8 he:1;
	u8 token:6;
};

#define HAL_NPDA_AC_SU 0
#define HAL_NPDA_AC_MU 1
#define HAL_NDPA_AX_FB_SU_NG_4 0
#define HAL_NDPA_AX_FB_SU_NG_16 2
#define HAL_NDPA_AX_FB_MU_NG_4 1
#define HAL_NDPA_AX_FB_MU_NG_16 3
#define HAL_NDPA_AX_FB_CQI 3
#define HAL_NPDA_AX_CB_SU42_MU75 0
#define HAL_NPDA_AX_CB_SU64_MU97 1

struct hal_he_snd_f2p {
	u16 csi_len_bfrp:12;
	u16 tb_t_pe_bfrp:2;
	u16 tri_pad_bfrp:2;

	u16 ul_cqi_rpt_tri_bfrp:1;
	u16 rf_gain_idx_bfrp:10;
	u16 fix_gain_en_bfrp:1;
	u16 rsvd:4;
};

struct hal_vht_ndpa_sta_info {
	u32 aid12:12;
	u32 feedback_type:1;
	u32 nc:3;
	u32 rsvd:16;
};

struct hal_he_ndpa_sta_info {
	u32 aid:11;
	u32 bw:14;
	u32 fb_ng:2;
	u32 disambiguation:1;
	u32 cb:1;
	u32 nc:3;
};

struct hal_ndpa_para {
	struct hal_frame_hdr common;
	struct hal_npda_dialog_token snd_dialog;
	u32 ndpa_sta_info[HAL_MAX_HE_SND_STA_NUM];
};

struct hal_he_trg_frm_cmn {
	u32 tgr_info: 4;
	u32 ul_len: 12;
	u32 more_tf: 1;
	u32 cs_rqd: 1;
	u32 ul_bw: 2;
	u32 gi_ltf: 2;
	u32 mimo_ltfmode: 1;
	u32 num_heltf: 3;
	u32 ul_pktext: 3;
	u32 ul_stbc: 1;
	u32 ldpc_extra_sym: 1;
	u32 dplr: 1;
	u32 ap_tx_pwr: 6;
	u32 ul_sr: 16;
	u32 ul_siga2_rsvd: 9;
	u32 rsvd: 1;
};

struct hal_he_trg_fm_user {
	u16 aid12;
	u8 ru_pos;
	u8 ul_fec_code;
	u8 ul_mcs;
	u8 ul_dcm;
	u8 ss_alloc;
	u8 ul_tgt_rssi;
};

struct hal_bfrp_he {
	struct hal_he_trg_frm_cmn common;
	struct hal_he_trg_fm_user user[HAL_MAX_HE_BFRP_STA_NUM];
	u8 fbseg_rexmit_bmp[HAL_MAX_HE_BFRP_STA_NUM];/*BFRP only*/
	struct hal_he_snd_f2p f2p_info;
};


struct hal_bfrp_vht {
	u8 rexmit_bmp;
};

struct hal_bfrp_para {
	struct hal_frame_hdr hdr[HLA_MAX_BFRP_NUM];
	struct hal_bfrp_he he_para[HAL_MAX_HE_BFRP_NUM];
	struct hal_bfrp_vht vht_para[HAL_MAX_VHT_BFRP_NUM];
};

struct hal_snd_wd_para {
	u16 txpktsize;
	u16 ndpa_duration;
	u16 datarate:9;
	u16 macid:7;
	u8 force_txop:1;
	u8 data_bw:2;
	u8 gi_ltf:3;
	u8 data_er:1;
	u8 data_dcm:1;
	u8 data_stbc:1;
	u8 data_ldpc:1;
	u8 data_bw_er:1;
	u8 multiport_id:1;
	u8 mbssid:4;
	u8 signaling_ta_pkt_sc:4;
	u8 sw_define:4;
	u8 txpwr_ofset_type:3;
	u8 lifetime_sel:3;
	u8 stf_mode:1;
	u8 disdatafb:1;
	u8 data_txcnt_lmt_sel:1;
	u8 data_txcnt_lmt:6;
	u8 sifs_tx:1;
	u8 snd_pkt_sel:3;
	u8 ndpa:2;
	u8 rsvd:3;
};

#define HAL_FEXG_TYPE_AC_SU 31
#define HAL_FEXG_TYPE_AC_MU_1 32
#define HAL_FEXG_TYPE_AC_MU_2 33
#define HAL_FEXG_TYPE_AC_MU_3 34
#define HAL_FEXG_TYPE_AX_SU 35
#define HAL_FEXG_TYPE_AX_MU_1 36
#define HAL_FEXG_TYPE_AX_MU_2 37

struct hal_ax_fwcmd_snd {
	u8 frame_ex_type;
	u8 bfrp0_sta_nr;
	u8 bfrp1_sta_nr;
	u16 macid[8];
	struct hal_ndpa_para ndpa;
	struct hal_bfrp_para bfrp;
	/**
	 * HE: NDPA NDP HE_Trigger_BFRP (CSI) BFRP (CSI)
	 * VHT: NDPA NDP (CSI) VHT_BFRP (CSI) BFRP (CSI) BFRP (CSI)
	 **/
	struct hal_snd_wd_para wd[HLA_MAX_BFRP_NUM + 2];
};


struct rtw_hal_com_t;
struct hal_io_ops {
	u8(*_read8)(struct rtw_hal_com_t *hal, u32 addr);
	u16(*_read16)(struct rtw_hal_com_t *hal, u32 addr);
	u32(*_read32)(struct rtw_hal_com_t *hal, u32 addr);
	void (*_read_mem)(struct rtw_hal_com_t *hal, u32 addr, u32 cnt, u8 *pmem);

	int (*_write8)(struct rtw_hal_com_t *hal, u32 addr, u8 val);
	int (*_write16)(struct rtw_hal_com_t *hal, u32 addr, u16 val);
	int (*_write32)(struct rtw_hal_com_t *hal, u32 addr, u32 val);
	int (*_write_mem)(struct rtw_hal_com_t *hal, u32 addr, u32 length, u8 *pdata);

#ifdef RTW_WKARD_BUS_WRITE
	int (*_write_post_cfg)(struct rtw_hal_com_t *hal, u32 addr, u32 val);
#endif

#ifdef CONFIG_SDIO_HCI
	u8(*_sd_f0_read8)(struct rtw_hal_com_t *hal, u32 addr);
#ifdef CONFIG_SDIO_INDIRECT_ACCESS
	u8(*_sd_iread8)(struct rtw_hal_com_t *hal, u32 addr);
	u16(*_sd_iread16)(struct rtw_hal_com_t *hal, u32 addr);
	u32(*_sd_iread32)(struct rtw_hal_com_t *hal, u32 addr);
	int (*_sd_iwrite8)(struct rtw_hal_com_t *hal, u32 addr, u8 val);
	int (*_sd_iwrite16)(struct rtw_hal_com_t *hal, u32 addr, u16 val);
	int (*_sd_iwrite32)(struct rtw_hal_com_t *hal, u32 addr, u32 val);
#endif /* CONFIG_SDIO_INDIRECT_ACCESS */
#endif
};

struct hal_io_priv {
	#ifdef CONFIG_SDIO_INDIRECT_ACCESS
	_os_mutex sd_indirect_access_mutex;
	#endif
	struct hal_io_ops io_ops;
};

enum pcfg_type {
	PCFG_FUNC_SW,
	PCFG_TBTT_AGG,
	PCFG_TBTT_SHIFT,
	PCFG_HIQ_WIN,
	PCFG_HIQ_DTIM,
	PCFG_HIQ_MAX,
	PCFG_BCN_INTERVAL,	/* Beacon Interval */
	PCFG_BSS_CLR,
	PCFG_BCN_EN,
	PCFG_MBSSID_EN,		/* M-BSSID ID enable */
	PCFG_BCN_DRP_ALL
};

/*
 * refers to _usb.h
 * #define USB11                0x1
 * #define USB2                 0x2
 * #define USB3                 0x3
 * */
enum usb_type {
	USB_1_1 = 1,
	USB_2_0,
	USB_3_0,
};
#ifdef RTW_PHL_BCN
struct bcn_entry_pool {
	u8 bcn_num;
	_os_list bcn_list;
	_os_lock bcn_lock;
};
#endif

enum rtw_hal_int_set_opt {
	INT_SET_OPT_HAL_INIT,
	INT_SET_OPT_SER_START,
	INT_SET_OPT_SER_DONE,
	INT_SET_OPT_LPS_START,
	INT_SET_OPT_LPS_STOP
};

struct hal_intr_mask_cfg {
	u8 halt_c2h_en;
	u8 wdt_en;
};

struct hal_halt_c2h_int {
	/* halt c2h */
	u32 intr;
	u32 val_mask;
	u32 val_default;
};

struct hal_watchdog_timer_int {
	/* watchdog timer */
	u32 intr;
	u32 val_mask;
	u32 val_default;
};

struct hal_int_array {
	struct hal_halt_c2h_int halt_c2h_int;
	struct hal_watchdog_timer_int watchdog_timer_int;
};

/* c2h event id for hal/phl layer */
enum rtw_hal_c2h_ev {
	HAL_C2H_EV_DO_NOTHING = 0,
	HAL_C2H_EV_BB_MUGRP_DOWN = 1,/* BB Process C2H mu-score-tbl done */
	HAL_C2H_EV_BTC_INFO = 2,	/* BTC event */
	HAL_C2H_EV_BTC_SCBD = 3,	/* BTC event */
	HAL_C2H_EV_MAC_TSF32_TOG = 4,	/* MAC event */
	HAL_C2H_EV_MAX
};

/* ppdu status : per user info */
struct hal_ppdu_sts_usr {
	/* MAC */
	u8 vld:1;
	u8 has_data:1;
	u8 has_ctrl:1;
	u8 has_mgnt:1;
	u8 has_bcn:1;
	u16 macid;
};

enum hal_rxcnt_sel {
	HAL_RXCNT_OFDM_OK = 0,
	HAL_RXCNT_OFDM_FAIL = 1,
	HAL_RXCNT_OFDM_FAM = 2,
	HAL_RXCNT_CCK_OK = 3,
	HAL_RXCNT_CCK_FAIL = 4,
	HAL_RXCNT_CCK_FAM = 5,
	HAL_RXCNT_HT_OK = 6,
	HAL_RXCNT_HT_FAIL = 7,
	HAL_RXCNT_HT_PPDU = 8,
	HAL_RXCNT_HT_FAM = 9,
	HAL_RXCNT_VHTSU_OK = 0xA,
	HAL_RXCNT_VHTSU_FAIL = 0xB,
	HAL_RXCNT_VHTSU_PPDU = 0xC,
	HAL_RXCNT_VHTSU_FAM = 0xD,
	HAL_RXCNT_VHTMU_OK = 0xE,
	HAL_RXCNT_VHTMU_FAIL = 0xF,
	HAL_RXCNT_VHTMU_PPDU = 0x10,
	HAL_RXCNT_VHTMU_FAM = 0x11,
	HAL_RXCNT_HESU_OK = 0x12,
	HAL_RXCNT_HESU_FAIL = 0x13,
	HAL_RXCNT_HESU_PPDU = 0x14,
	HAL_RXCNT_HESU_FAM = 0x15,
	HAL_RXCNT_HEMU_OK = 0x16,
	HAL_RXCNT_HEMU_FAIL = 0x17,
	HAL_RXCNT_HEMU_PPDU = 0x18,
	HAL_RXCNT_HEMU_FAM = 0x19,
	HAL_RXCNT_HETB_OK = 0x1A,
	HAL_RXCNT_HETB_FAIL = 0x1B,
	HAL_RXCNT_HETB_PPDU = 0x1C,
	HAL_RXCNT_HETB_FAM = 0x1D,
	HAL_RXCNT_INVD = 0x1E,
	HAL_RXCNT_RECCA = 0x1F,
	HAL_RXCNT_FULLDRP = 0x20,
	HAL_RXCNT_FULLDRP_PKT = 0x21,
	HAL_RXCNT_RXDMA = 0x22,
	HAL_RXCNT_USER0 = 0x23,
	HAL_RXCNT_USER1 = 0x24,
	HAL_RXCNT_USER2 = 0x25,
	HAL_RXCNT_USER3 = 0x26,
	HAL_RXCNT_CONT_FCS = 0x27,
	HAL_RXCNT_PKTFLTR_DRP = 0x28,
	HAL_RXCNT_CSIPKT_DMA_OK = 0x29,
	HAL_RXCNT_CSIPKT_DMA_DROP = 0x2A,
	HAL_RXCNT_MAX
};

enum hal_rate_mode {
	HAL_LEGACY_MODE	= 0,
	HAL_HT_MODE	= 1,
	HAL_VHT_MODE	= 2,
	HAL_HE_MODE	= 3
};

enum hal_rate_bw {
	HAL_RATE_BW_20	= 0,
	HAL_RATE_BW_40	= 1,
	HAL_RATE_BW_80	= 2,
	HAL_RATE_BW_160	= 3,
};

struct hal_ppdu_rx_cnt {
	u16 ppdu_cnt[HAL_RXCNT_MAX];
};

/* ppdu sts mac bmp_append_info */
#define HAL_PPDU_MAC_INFO BIT(1)
#define HAL_PPDU_PLCP BIT(3)
#define HAL_PPDU_RX_CNT BIT(2)
/* ppdu sts mac bmp_filter */
#define HAL_PPDU_HAS_A1M BIT(4)
#define HAL_PPDU_HAS_CRC_OK BIT(5)
#define HAL_PPDU_HAS_DMA_OK BIT(6)

/* ppdu status (mac info + phy info) */
struct hal_ppdu_sts {
#define RTW_HAL_PPDU_STS_MAX_USR 4
	/* MAC */
	/* NOTE: (rx_cnt, plcp, phy_st)_ptr are only available before phl_recycle_rx_buf() */
	u8 *rx_cnt_ptr;
	u8 *plcp_ptr;
	u8 *phy_st_ptr; /* for bb phy status */
	u8 plcp_size;
	u32 phy_st_size;/* for bb phy status */
	u32 rx_cnt_size;
	u8 usr_num;
	struct hal_ppdu_sts_usr usr[RTW_HAL_PPDU_STS_MAX_USR];
	struct hal_ppdu_rx_cnt rx_cnt;
};

#define HAL_RSSI_MAVG_NUM 16
#define STA_UPDATE_MA_RSSI_FAST(_RSSI, _VAL) _RSSI = ((_RSSI * \
					     (HAL_RSSI_MAVG_NUM - 1)) + _VAL) \
					     / HAL_RSSI_MAVG_NUM

struct rtw_cfo_info {
	s32		cfo_tail;
	s32		pre_cfo_avg;
	s32		cfo_avg;
	u16		cfo_cnt;
	u32		tp;
};


struct rtw_rssi_info {
	u8 rssi; /* u(8,1), hal-bb provide, read only : 0~110 (dBm = rssi -110) */
	u16 rssi_ma; /* u(16,5),  hal-bb provide, read only : u16 U(12,4)*/
	u16 rssi_ma_path[4];
	u16 pkt_cnt_data;
	u8 rssi_bcn; /* u(8,1), beacon RSSI, hal-bb provide, read only : 0~110 (dBm = rssi -110) */
	u16 rssi_bcn_ma; /* u(16,5),  beacon RSSI, hal-bb provide, read only*/
	u16 rssi_bcn_ma_path[4];
	u16 pkt_cnt_bcn;
	u8 ma_factor:4;
	u8 ma_factor_bcn:4;
	u8 rssi_ofdm; /* u(8,1),  hal-bb provide, read only : packet, for debug */
	u8 rssi_cck; /* u(8,1),  hal-bb provide, read only : packet, for debug */
	u8 assoc_rssi; /* phl_rx provide, read only */
	/* phl_rx provide, read only : Moving Average RSSI information for the STA */
	u8 ma_rssi; /* moving average : 0 ~ PHL_MAX_RSSI (dBm = rssi - PHL_MAX_RSSI) */
	u8 ma_rssi_mgnt; /* moving average rssi for beacon/probe : 0 ~ PHL_MAX_RSSI (dBm = rssi - PHL_MAX_RSSI) */
	u16 snr_ma; /* u(16,4), hal-bb provide, read only, SNR= snr_ma dBm*/
	u16 snr_ma_path[4];
};

struct rtw_rate_info {
 	enum rtw_gi_ltf gi_ltf; /* 3bit GILTF */
	enum hal_rate_mode mode; /* 2bit 0:legacy, 1:HT, 2:VHT, 3:HE*/
	enum hal_rate_bw bw; /*2bit 0:5M/10M/20M, 1:40M, 2:80M, 3:160M or 80+80*/
	u8 mcs_ss_idx; /*HE: 3bit SS + 4bit MCS; non-HE: 5bit MCS/rate idx */
	u8 mcs_idx;
	u8 ss; /* 0: 1ss, 1:2ss, ... */
	bool is_actrl; /* 0: don't append a-ctrl field; 1: append a-ctrl field */
};

/* from cmn_sta_info */
struct rtw_ra_sta_info {
	/*u8 rate_id;			remove !!! use wmode in phl, [PHYDM] ratr_idx*/
	/*u8 rssi_level;			[PHYDM]*/
	/*u8 is_first_connect:1;		change connect flow, [PHYDM] CE: ra_rpt_linked, AP: H2C_rssi_rpt*/
	/*u8 is_support_sgi:1;		mov to phl [driver]*/
	/*u8 is_vht_enable:2;		mov to phl [driver]*/
	/*u8 disable_ra:1;			mov to hal [driver]*/
	/*u8 disable_pt:1;			remove for no PT, [driver] remove is_disable_power_training*/
	/*u8 txrx_state:2;			///////////////need to check if needed, [PHYDM] 0: Tx, 1:Rx, 2:bi-direction*/
	/*u8 is_noisy:1;			///////////////need to check if needed, [PHYDM]*/
	u16 curr_tx_rate;			/*use struct bb_rate_info, [PHYDM] FW->Driver*/
	enum channel_width ra_bw_mode;	/*remove to phl, [Driver] max bandwidth, for RA only*/
	enum channel_width curr_tx_bw;	/*bb_rate_info, [PHYDM] FW->Driver*/
	/* u8 drv_ractrl; */

	/* Ctrl */
	bool dis_ra; /*move from rtw_hal_stainfo_t*/
	bool ra_registered;/*move from rtw_hal_stainfo_t*/
	u64 ra_mask;/*move from rtw_hal_stainfo_t*/ /*drv decide by specific req*/
	u64 cur_ra_mask;/*move from rtw_hal_stainfo_t*/
	/*halbb create, mod by driver and decide by rssi or other*/
	u8 cal_giltf; /* 3bit gi_ltf ctrl by driver*/
	bool fix_giltf_en; /*giltf from cal_giltf or halbb*/
	bool fixed_rt_en;
	struct rtw_rate_info fixed_rt_i;
	u8 rainfo_cfg1; /* prepare for other control*/
	u8 rainfo_cfg2; /* prepare for other control*/

	/* Report */
	struct rtw_rate_info rpt_rt_i;
	u8 curr_retry_ratio;		/*[HALBB] FW->Driver*/

	bool ra_csi_rate_en;
	bool fixed_csi_rate_en;
	u8 band_num;
	struct rtw_rate_info csi_rate;

	u8 avg_agg;
	u32 tx_ok_cnt[4];
	u32 tx_retry_cnt[4];
	u32 tx_total_cnt;
	/*u64 ramask;*/
};

struct rtw_mura_info {
	/* Ctrl */
	bool fixed_rt_en;
	struct rtw_rate_info fixed_rt_i;
	/* Report */
	struct rtw_rate_info rpt_rt_i;
	u8 curr_retry_ratio;		/*[PHYDM] FW->Driver*/
};

/* from cmn_sta_info */
struct rtw_dtp_info {
	u8 dyn_tx_power;	/*Dynamic Tx power offset*/
	u8 last_tx_power;
	u8 sta_tx_high_power_lvl:4;
	u8 sta_last_dtp_lvl:4;
};

struct rtw_hal_muba_info {
	u32 fix_ba:1;
	u32 ru_psd:9;
	u32 tf_rate:9;
	u32 rf_gain_fix:1;
	u32 rf_gain_idx:10;
	u32 tb_ppdu_bw:2;
	u8 dcm:1;
	u8 ss:3;
	u8 mcs:4;
	u8 gi_ltf:3;
	u8 doppler:1;
	u8 stbc:1;
	u8 sta_coding:1;
	u8 tb_t_pe_nom:2;
	u8 pr20_bw_en:1;
	u8 ma_type: 1;
	u8 rsvd1: 6;
};

enum rtw_hal_protection_type {
	HAL_PROT_NO_PROETCT = 0,
	HAL_PROT_PRIUSER_HW_RTS = 1,
	HAL_PROT_RTS = 2,
	HAL_PROT_CTS2SELF = 3,
	HAL_PROT_MU_RTS = 4,
	HAL_PROT_HW_DEFAULT_ = 5
};

enum rtw_hal_ack_resp_type {
	HAL_ACK_N_MINUS_1USER_BA = 0,
	HAL_ACK_N_USER_BA = 1,
	HAL_ACK_MU_BAR = 2,
	HAL_ACK_HTP_ACK = 3,
	HAL_ACK_HW_DEFAULT = 4
};

struct rtw_wp_rpt_stats {
	u32 busy_cnt;
	u32 tx_ok_cnt;
	u32 tx_fail_cnt;
	u32 rty_fail_cnt;
	u32 lifetime_drop_cnt;
	u32 macid_drop_cnt;
	u32 sw_drop_cnt;
	u32 recycle_fail_cnt;
	u32 delay_tx_ok_cnt;
	u32 delay_rty_fail_cnt;
	u32 delay_lifetime_drop_cnt;
	u32 delay_macid_drop_cnt;
};

struct rtw_trx_stat {
	u32 rx_ok_cnt;
	u32 rx_err_cnt;
	u16 rx_rate_plurality;
	/* add lock for tx statistics */
	_os_lock tx_sts_lock;
	/* Below info is for release report*/
	u32 tx_fail_cnt;
	u32 tx_ok_cnt;
#if defined(CONFIG_PHL_RELEASE_RPT_ENABLE) || defined(CONFIG_PCI_HCI)
	struct rtw_wp_rpt_stats *wp_rpt_stats;
#endif
#ifdef CONFIG_PCI_HCI
	u32 ltr_tx_dly_count;
	u32 ltr_last_tx_dly_time;
	u32 rx_rdu_cnt;
#endif
#ifdef CONFIG_VW_REFINE
	u16 pretx_fail; /* pretx fail count */
	u16 phltx_cnt;  /* phl_tx run count */

	u32 vw_cnt_snd; /* total send count for veriwave data */
	u32 vw_cnt_rev; /* total recv count for veriwave data */
	u32 vw_cnt_err; /* total err  count for veriwave data */
#endif
	u16 rx_rate;
	u8 rx_bw;
	u8 rx_gi_ltf;
};

struct bacam_ctrl_t {
	u8 used_map[MAX_BAENTRY];
	u8 tid[MAX_BAENTRY];
	u16 mac_id[MAX_BAENTRY];
	u8 count;
};

struct rtw_hal_stainfo_t {
	/* from cmn_sta_info */
	u16	dm_ctrl;
	/* struct su_ra_info */
	struct rtw_rssi_info rssi_stat;
	struct rtw_cfo_info cfo_stat;

	/* Beamform Related */
	u8 bf_cap;/* sta's beamform capability : ht/vht/he + bfee/bfer */
	void *bf_entry;
	u16 bf_csi_buf;
	u16 bf_csi_buf_swap;/*used in mu swap mode*/
	/* BFee capability */
	u8 max_nc;
	u8 nr;
	u8 ng16;/* 0:non-support ; BIT0:support SU ; BIT1:support MU */
	u8 cb_sz;/* 0:non-support ; BIT0:support (4,2) SU ; BIT1:support (7,5) MU */
	u8 support_cqi_fb;
	/*mu group*/
	u8 mugrp_bmp;
	u32 mu_score;
	/*FW Frame Exchange : when STA is primary STA, prefer protect type and ack resp type in MU*/
	enum rtw_hal_protection_type prot_type;
	enum rtw_hal_ack_resp_type resp_type;

	/* from cmn_sta_info */
	struct rtw_ra_sta_info	ra_info;
	/* from cmn_sta_info */
	struct rtw_dtp_info dtp_stat;
	struct rtw_trx_stat trx_stat;
	void *hw_cfg_tab;
	void *bb_sta;
};



struct bus_hw_cap_t {
#ifdef CONFIG_PCI_HCI
	enum rtw_pcie_bus_func_cap_t l0s_ctrl;
	enum rtw_pcie_bus_func_cap_t l1_ctrl;
	enum rtw_pcie_bus_func_cap_t l1ss_ctrl;
	enum rtw_pcie_bus_func_cap_t wake_ctrl;
	enum rtw_pcie_bus_func_cap_t crq_ctrl;
	u8 clkdly_ctrl;
	u8 l0sdly_ctrl;
	u8 l1dly_ctrl;
	u8 ltr_sw_ctrl; /* whether ltr can be controlled by sw */
	u8 ltr_hw_ctrl;
	u16 max_txbd_num;
	u16 max_rxbd_num;
	u16 max_rpbd_num;
	u32 max_rxbuf_size;
	u32 max_rpbuf_size;
	u8 max_phyaddr_num;
	u8 max_wd_page_size;
	u8 txbd_len;
	u8 rxbd_len;
	u8 wdb_size;
	u8 wdi_size;
	u8 addr_info_size;
	u8 seq_info_size;
#elif defined (CONFIG_USB_HCI)
	u32 tx_buf_size;
	u32 tx_buf_num;
	u32 tx_mgnt_buf_size;
	u32 tx_mgnt_buf_num;
	u32 tx_h2c_buf_num;
	u32 rx_buf_size;
	u32 rx_buf_num;
	u32 in_token_num;
#elif defined (CONFIG_SDIO_HCI)
	u32 tx_buf_size;
	u32 tx_buf_num;
	u32 tx_mgnt_buf_size;
	u32 tx_mgnt_buf_num;
	u32 rx_buf_size;
	u32 rx_buf_num;
#else
	u8 temp_for_struct_empty; /* for undefined interface */
#endif
};

/* phy capability of phy */
struct phy_hw_cap_t {
	#ifdef RTW_WKARD_BTC_RFETYPE
	u8 rfe_type;
	#endif
	u8 tx_num;
	u8 rx_num;
	u8 tx_path_num;
	u8 rx_path_num;
	u16 hw_rts_time_th;
	u16 hw_rts_len_th;
	u32 txagg_num;
};


/*PHYx + Sx*/
enum phl_phy_idx {
	HW_PHY_0,
	HW_PHY_1,
	HW_PHY_MAX
};

enum phl_pwr_table {
	PWR_BY_RATE	= BIT0,
	PWR_LIMIT	= BIT1,
	PWR_LIMIT_RU	= BIT2
};

enum phl_rf_mode {
    RF_MODE_NORMAL		= 0,
    RF_MODE_SHUTDOWN	= 1,
    RF_MODE_STANDBY		= 2,
	RF_MODE_RX		= 3,
	RF_MODE_TX		= 4,
	RF_MODE_MAX
};

enum phl_pwr_ctrl {
	ALL_TIME_CTRL = 0,
        GNT_TIME_CTRL,
        PWR_CTRL_MAX
};

/*--------------------------------------------------------------------------*/
/*[TX Power Unit(TPU) array size]*/
#define TPU_SIZE_PWR_TAB	16 /*MCS0~MCS11(12) + {dcm_0,1,3,4}4 = 16*/
#define TPU_SIZE_PWR_TAB_lGCY	12 /*cck(4) + ofdm(8) = 12*/
#define TPU_SIZE_MODE		5  /*0~4: HE, VHT, HT, Legacy, CCK, */
#define TPU_SIZE_BW		5 /*0~4: 80_80, 160, 80, 40, 20*/
#define TPU_SIZE_RUA		3 /*{26, 52, 106}*/
#define TPU_SIZE_BW20_SC	8 /*8 * 20M = 160M*/
#define TPU_SIZE_BW40_SC	4 /*4 * 40M = 160M*/
#define TPU_SIZE_BW80_SC	2 /*2 * 80M = 160M*/
#define TPU_SIZE_BF		2 /*{NON_BF, BF}*/

#if (defined(CONFIG_RTL8851A) || defined(CONFIG_RTL8851B))
	#define HAL_COMPILE_IC_1SS
#endif

#if (defined(CONFIG_RTL8852A) || defined(CONFIG_RTL8852B) || defined(CONFIG_RTL8852BP) || defined(CONFIG_RTL8852C) || \
    defined(CONFIG_RTL8192XB) || defined(CONFIG_RTL8832BR))
	#define HAL_COMPILE_IC_2SS
#endif

#if defined(CONFIG_RTL8853A)
	#define HAL_COMPILE_IC_3SS
#endif

#if defined(CONFIG_RTL8834A)
	#define HAL_COMPILE_IC_4SS
#endif

/*@==========================================================================*/
#if (defined(HAL_COMPILE_IC_4SS))
	#define HAL_COMPILE_ABOVE_4SS
#endif

#if (defined(HAL_COMPILE_IC_3SS) || defined(HAL_COMPILE_ABOVE_4SS))
	#define HAL_COMPILE_ABOVE_3SS
#endif

#if (defined(HAL_COMPILE_IC_2SS) || defined(HAL_COMPILE_ABOVE_3SS))
	#define HAL_COMPILE_ABOVE_2SS
#endif

#if (defined(HAL_COMPILE_IC_1SS) || defined(HAL_COMPILE_ABOVE_2SS))
	#define HAL_COMPILE_ABOVE_1SS
#endif

#if (defined(HAL_COMPILE_ABOVE_4SS))
	#define HAL_MAX_PATH	4
#elif (defined(HAL_COMPILE_ABOVE_3SS))
	#define HAL_MAX_PATH	3
#elif (defined(HAL_COMPILE_ABOVE_2SS))
	#define HAL_MAX_PATH	2
#else
	#define HAL_MAX_PATH	1
#endif

/*--------------------------[Structure]-------------------------------------*/
enum rtw_tpu_op_mode {
	TPU_NORMAL_MODE		= 0,
	TPU_DBG_MODE		= 1
};

struct rtw_tpu_pwr_by_rate_info { /*TX Power Unit (TPU)*/
	s8 pwr_by_rate_lgcy[TPU_SIZE_PWR_TAB_lGCY];
	s8 pwr_by_rate[HAL_MAX_PATH][TPU_SIZE_PWR_TAB];
};

struct rtw_tpu_pwr_imt_info { /*TX Power Unit (TPU)*/
	s8 pwr_lmt_cck_20m[HAL_MAX_PATH][TPU_SIZE_BF];
	s8 pwr_lmt_cck_40m[HAL_MAX_PATH][TPU_SIZE_BF];
	s8 pwr_lmt_lgcy_20m[HAL_MAX_PATH][TPU_SIZE_BF]; /*ofdm*/
	s8 pwr_lmt_20m[HAL_MAX_PATH][TPU_SIZE_BW20_SC][TPU_SIZE_BF];
	s8 pwr_lmt_40m[HAL_MAX_PATH][TPU_SIZE_BW40_SC][TPU_SIZE_BF];
	s8 pwr_lmt_80m[HAL_MAX_PATH][TPU_SIZE_BW80_SC][TPU_SIZE_BF];
	s8 pwr_lmt_160m[HAL_MAX_PATH][TPU_SIZE_BF];
	s8 pwr_lmt_40m_0p5[HAL_MAX_PATH][TPU_SIZE_BF];
	s8 pwr_lmt_40m_2p5[HAL_MAX_PATH][TPU_SIZE_BF];
};

struct rtw_tpu_info { /*TX Power Unit (TPU)*/
	enum rtw_tpu_op_mode op_mode; /*In debug mode, only debug tool control TPU APIs*/
	bool normal_mode_lock_en;
	s8 ofst_int; /*SW: S(8,3) -16 ~ +15.875 (dB)*/
	u8 ofst_fraction; /*[0:3] * 0.125(dBm)*/
	enum hal_path ref_pow_path; /*Select the path with larger pow as the re_ path*/
	u8 path_pow_ofst_decrease; /* Select the path with lower pow and subtract pow_path_ofst_decrease from path_ref*/
	u8 base_cw_0db; /*[63~39~15]: [+24~0~-24 dBm]*/
	u16 tssi_16dBm_cw;
	/*[Ref Pwr]*/
	s16 ref_pow_ofdm; /*-> HW: s(9,2)*/
	s16 ref_pow_cck; /*-> HW: s(9,2)*/
	u16 ref_pow_ofdm_cw; /*BBCR 0x58E0[9:0]*/
	u16 ref_pow_cck_cw; /*BBCR 0x58E0[21:12]*/
	/*[Pwr Ofsset]*/ /*-> HW: s(7,1)*/
	s8 pwr_ofst_mode[TPU_SIZE_MODE]; /*0~4: HE, VHT, HT, Legacy, CCK, */
	s8 pwr_ofst_bw[TPU_SIZE_BW]; /*0~4: 80_80, 160, 80, 40, 20*/
	/*[Pwr By rate]*/ /*-> HW: s(7,1)*/
	struct rtw_tpu_pwr_by_rate_info rtw_tpu_pwr_by_rate_i;
	/*[Pwr Limit]*/ /*-> HW: s(7,1)*/
	struct rtw_tpu_pwr_imt_info rtw_tpu_pwr_imt_i;
	/*[Pwr Limit RUA]*/ /*-> HW: s(7,1)*/
	s8 pwr_lmt_ru[HAL_MAX_PATH][TPU_SIZE_RUA][TPU_SIZE_BW20_SC];
	u16 pwr_lmt_ru_mem_size;
	bool pwr_lmt_en;
	bool ext_pwr_lmt_en;
	struct rtw_phl_ext_pwr_lmt_info ext_pwr_lmt_i;
	u8 tx_ptrn_shap_idx;
	u8 tx_ptrn_shap_idx_cck;
	u16 pwr_constraint_mb;
};

struct rtw_hal_stat_info {
	u32 cnt_fail_all;
	u32 cnt_cck_fail;
	u32 cnt_ofdm_fail;
	u32 cnt_cca_all;
	u32 cnt_ofdm_cca;
	u32 cnt_cck_cca;
	u32 cnt_crc32_error_all;
	u32 cnt_he_crc32_error;
	u32 cnt_vht_crc32_error;
	u32 cnt_ht_crc32_error ;
	u32 cnt_ofdm_crc32_error;
	u32 cnt_cck_crc32_error;
	u32 cnt_crc32_ok_all;
	u32 cnt_he_crc32_ok;
	u32 cnt_vht_crc32_ok;
	u32 cnt_ht_crc32_ok;
	u32 cnt_ofdm_crc32_ok;
	u32 cnt_cck_crc32_ok;
	u32 igi_fa_rssi;
};

struct rtw_hw_band {
	struct rtw_chan_def cur_chandef;
	u8 ppdu_sts_appen_info;
	u8 ppdu_sts_filter;
	struct rtw_tpu_info rtw_tpu_i; /*TX Power Unit (TPU)*/
	u16 tx_pause[PAUSE_RSON_MAX]; /* ref: enum rtw_sch_txen_cfg */
	struct rtw_hal_stat_info stat_info;
	u8 assoc_sta_cnt; /*number of associated nodes (sta or ap)*/
	enum rtw_rx_fltr_opt_mode rx_fltr_opt_mode;
};

struct rtw_intr_t {
	bool en;
	u32 mask_dflt;
	u32 mask;
	u32 val;
};

struct rtw_hal_com_t {
	enum rtw_chip_id chip_id;
	enum rtw_cv cv;
	enum rtw_cv acv;

	struct ver_ctrl_t mac_vc;
	struct ver_ctrl_t bb_vc;
	struct ver_ctrl_t rf_vc;
	struct ver_ctrl_t btc_vc;
	struct ver_ctrl_t fw_vc;

	struct protocol_cap_t proto_hw_cap[MAX_BAND_NUM]; /* wifi protocol capability from EFUSE/halmac/halbb/halrf/... */
	struct phy_hw_cap_t phy_hw_cap[MAX_BAND_NUM]; /* phy capability from EFUSE/halmac/halbb/halrf/... */
	struct dev_cap_t dev_hw_cap;

	struct bus_hw_cap_t bus_hw_cap; /* Bus HW capability */
	struct bus_cap_t bus_cap; /* Final bus capability */
	struct hal_io_priv iopriv;
	#ifdef DBG_HAL_MAC_MEM_MOINTOR
	_os_atomic hal_mac_mem;
	#endif
	#ifdef DBG_HAL_MEM_MOINTOR
	_os_atomic hal_mem;
	#endif
	bool is_hal_init;
	struct rtw_hw_band band[MAX_BAND_NUM];/*band0/band1 for DBCC*/

	bool dbcc_en;
	u8 assoc_sta_cnt; /*number of associated nodes (sta or ap)*/

#ifdef RTW_WKARD_SINGLE_PATH_RSSI
	enum rf_path cur_rx_rfpath;
#endif
#ifdef CONFIG_PCI_HCI /*TODO move to hal_info_t*/
	/*interrupt*/
	u32 int_array[4];
	u32 int_mask[4];
	u32 int_mask_default[4];

	struct rtw_intr_t _intr_ind[4];
	struct rtw_intr_t _intr[4];
	bool in_lps_intr_cfg;
#ifdef PHL_RXSC_ISR
	u32 rx_int_array;
#endif
#endif /* CONFIG_PCI_HCI */
#ifdef CONFIG_SDIO_HCI /*TODO move to hal_info_t*/
	u32 block_sz;

	/*interrupt*/
	/*
	 * Change type of int_mask from u32 to unsigned long for bit operation
	 * API, ex: _os_test_and_clear_bit() and _os_test_and_set_bit() .
	 */
	unsigned long int_mask;
	u32 int_mask_default;
#endif /* CONFIG_SDIO_HCI */

	struct hal_int_array intr;

#ifdef RTW_PHL_BCN
	struct bcn_entry_pool bcn_pool;
#endif
	struct rtw_trx_stat trx_stat;
	void *hal_priv;/*pointer to hal_info*/
	void *drv_priv; /*drv priv*/

	void *csi_obj;
	void *bf_obj;
	void *snd_obj;

	struct hal_mu_score_tbl bb_mu_score_tbl;
	bool csi_para_ctrl_sel;

	struct btc_ctrl_t btc_ctrl;
	struct btc_fw_msg btc_msg;
#ifdef RTW_WKARD_CCX_RPT_LIMIT_CTRL
	u8 spe_pkt_cnt_lmt;
#endif
	u32 uuid;
	u8 scanofld_en;
	struct bacam_ctrl_t ba_ctl;
};

#define FL_CFG_OP_SET 0
#define FL_CFG_OP_CLR 1
#define FL_CFG_OP_INFO 2

#define FL_CFG_TYPE_LEVEL 0
#define FL_CFG_TYPE_OUTPUT 1
#define FL_CFG_TYPE_COMP 2
#define FL_CFG_TYPE_COMP_EXT 3

#define FL_LV_OFF 0
#define FL_LV_CRT 1
#define FL_LV_SER 2
#define FL_LV_WARN 3
#define FL_LV_LOUD 4
#define FL_LV_TR 5

#define FL_OP_UART BIT0
#define FL_OP_C2H BIT1
#define FL_OP_SNI BIT2

struct rtw_hal_fw_log_cfg {
	u32 level;
	u32 output;
	u32 comp;
	u32 comp_ext;
};

enum pkt_ofld_op_type {
	PKT_OFLD_ADD = 0,
	PKT_OFLD_DEL = 1,
	PKT_OFLD_READ = 2,
	PKT_OFLD_MAX
};

struct pkt_ofld_info {
	struct list_head req_q;
	u32 req_cnt;
	u8 id;
};

struct pkt_ofld_entry {
	struct list_head list;
	u16 macid;
	struct pkt_ofld_info pkt_info[PKT_OFLD_TYPE_MAX];
};

enum rtw_c2h_cat {
	C2H_CAT_TEST = 0,
	C2H_CAT_MAC = 1,
	C2H_CAT_OUTSRC = 2,
	C2H_CAT_MAX = 3
};

enum rtw_c2h_clas {
	C2H_CLS_PHYDM_MIN = 0,
	C2H_CLS_PHYDM_MAX = 7,
	C2H_CLS_RF_MIN = 8,
	C2H_CLS_RF_MAX = 0xf,
	C2H_CLS_BTC_MIN = 0x10,
	C2H_CLS_BTC_MAX = 0x17,
	C2H_CLS_MAX = 0x18
};

#define C2H_CLS_MAC_MIN 0x00
#define C2H_CLS_MAC_MAX 0xFF

enum rtw_hal_ps_pwr_req_src {
	HAL_BB_PWR_REQ = 0,
	HAL_RF_PWR_REQ = 1,
	HAL_MAC_PWR_REQ = 2,
	HAL_BTC_PWR_REQ = 3
};

struct rtw_hal_lps_info {
	u8 en;
	u16 macid;
	enum rtw_lps_listen_bcn_mode listen_bcn_mode;
	u8 awake_interval;
	enum rtw_lps_smart_ps_mode smart_ps_mode;
	u8 bcnnohit_en;
};

struct rtw_hal_ips_info {
	u8 en;
	u16 macid;
};

enum ps_pwr_state {
	PS_PWR_STATE_ACTIVE = 0,
	PS_PWR_STATE_BAND0_RFON = 1,
	PS_PWR_STATE_BAND1_RFON = 2,
	PS_PWR_STATE_BAND0_RFOFF = 3,
	PS_PWR_STATE_BAND1_RFOFF = 4,
	PS_PWR_STATE_CLK_GATED = 5,
	PS_PWR_STATE_PWR_GATED = 6,
	PS_PWR_STATE_MAX,
};

#ifdef CONFIG_PHL_DFS
struct hal_mac_dfs_rpt_cfg {
	bool rpt_en;
	u8 rpt_num_th;
	bool rpt_en_to;
	u8 rpt_to;
};

struct hal_dfs_rpt {
	u8 *dfs_ptr;
	u16 dfs_num;
	u8 phy_idx; /*phy0,phy1*/
};
#endif

#ifdef CONFIG_WOWLAN
struct rtw_hal_wow_cfg {
	struct rtw_keep_alive_info *keep_alive_cfg;
	struct rtw_disc_det_info *disc_det_cfg;
	struct rtw_nlo_info *nlo_cfg;
	struct rtw_arp_ofld_info *arp_ofld_cfg;
	struct rtw_ndp_ofld_info *ndp_ofld_cfg;
	struct rtw_gtk_ofld_info *gtk_ofld_cfg;
	struct rtw_realwow_info *realwow_cfg;
	struct rtw_wow_wake_info *wow_wake_cfg;
	struct rtw_pattern_match_info *pattern_match_info;
	struct rtw_wow_gpio_info *wow_gpio;
	struct rtw_periodic_wake_info *periodic_wake_cfg;
};
#endif /* CONFIG_WOWLAN */


enum hal_tsf_sync_act {
	HAL_TSF_SYNC_NOW_ONCE = 0,
	HAL_TSF_EN_SYNC_AUTO = 1,
	HAL_TSF_DIS_SYNC_AUTO = 2,
};

struct watchdog_nhm_report {
	u8 ccx_rpt_stamp;
	u8 ccx_rpt_result;
	s8 nhm_pwr_dbm;
	u8 nhm_ratio;
};

#define GT3_TIMEOUT_MASK 0x0FFFFFFF
struct hal_mac_dbg_dump_cfg {
	u32 ss_dbg_0;
	u32 ss_dbg_1;
	u8 ss_dbg;
	u8 dle_dbg;
	u8 dmac_dbg;
	u8 cmac_dbg;
	u8 mac_dbg_port;
	u8 plersvd_dbg;
	u8 tx_flow_dbg;
};

/**
 * @enum rtw_hal_lps_flg_state
 * @brief rtw_hal_lps_flg_state
 *
 * @var rtw_hal_lps_flg_state::RTW_HAL_LPS_FLG_STATE_ACTIVE
 * Active mode or lps rf off
 * @var rtw_hal_lps_flg_state::RTW_HAL_LPS_FLG_STATE_LPS
 * Enter lps cg or lps pg
 *
 */
enum rtw_hal_lps_flg_state {
	RTW_HAL_LPS_FLG_STATE_ACTIVE = 0,
	RTW_HAL_LPS_FLG_STATE_LPS = 1,
	RTW_HAL_LPS_FLG_STATE_MAX,
};
#endif /*_HAL_DEF_H_*/

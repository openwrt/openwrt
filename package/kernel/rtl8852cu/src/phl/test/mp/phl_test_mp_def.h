/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation.
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
#ifndef _PHL_TEST_MP_DEF_H_
#define _PHL_TEST_MP_DEF_H_

#ifdef CONFIG_PHL_TEST_MP
enum mp_cmd_status {
	MP_STATUS_NOT_INIT = 0,
	MP_STATUS_INIT = 1,
	MP_STATUS_WAIT_CMD = 2,
	MP_STATUS_CMD_EVENT = 3,
	MP_STATUS_RUN_CMD = 4,
};

/* mp command class */
enum mp_class {
	MP_CLASS_CONFIG = 0,
	MP_CLASS_TX = 1,
	MP_CLASS_RX = 2,
	MP_CLASS_EFUSE = 3,
	MP_CLASS_REG = 4,
	MP_CLASS_TXPWR = 5,
	MP_CLASS_CAL = 6,
	MP_CLASS_FLASH = 7,
	MP_CLASS_MAX,
};

enum mp_tx_method {
	MP_SW_TX = 0,
	MP_PMACT_TX,
	MP_TMACT_TX,
	MP_FW_PMAC_TX,
};

enum mp_tx_cmd {
	MP_TX_NONE = 0,
	MP_TX_PACKETS,
	MP_TX_CONTINUOUS,
	MP_TX_SINGLE_TONE,
	MP_TX_CCK_Carrier_Suppression,
	MP_TX_CONFIG_PLCP_COMMON_INFO,
	MP_TX_CMD_PHY_OK,
	MP_TX_CONFIG_PLCP_PATTERN,
	MP_TX_CONFIG_PLCP_USER_INFO,
	MP_TX_TB_TEST,
	MP_TX_DPD_BYPASS,
	MP_TX_CHECK_TX_IDLE,
	MP_TX_CMD_BB_LOOPBCK,
	MP_TX_CMD_MAX,
};

/* mp config command */
enum mp_config_cmd {
	MP_CONFIG_CMD_GET_BW,
	MP_CONFIG_CMD_GET_RF_STATUS,
	MP_CONFIG_CMD_SET_RATE_IDX,
	MP_CONFIG_CMD_SET_RF_TXRX_PATH,
	MP_CONFIG_CMD_SET_RESET_PHY_COUNT,
	MP_CONFIG_CMD_SET_RESET_MAC_COUNT,
	MP_CONFIG_CMD_SET_RESET_DRV_COUNT,
	MP_CONFIG_CMD_SET_TXRX_MODE,
	MP_CONFIG_CMD_PBC,
	MP_CONFIG_CMD_START_DUT,
	MP_CONFIG_CMD_STOP_DUT,
	MP_CONFIG_CMD_GET_MIMPO_RSSI,
	MP_CONFIG_CMD_GET_BOARD_TYPE,
	MP_CONFIG_CMD_GET_MODULATION,
	MP_CONFIG_CMD_GET_RF_MODE,
	MP_CONFIG_CMD_GET_RF_PATH,
	MP_CONFIG_CMD_SET_MODULATION,
	MP_CONFIG_CMD_GET_DEVICE_INFO,
	MP_CONFIG_CDM_SET_PHY_INDEX,
	MP_CONFIG_CMD_GET_MAC_ADDR,
	MP_CONFIG_CMD_SET_MAC_ADDR,
	MP_CONFIG_CMD_SET_CH_BW,
	MP_CONFIG_CMD_GET_TX_NSS,
	MP_CONFIG_CMD_GET_RX_NSS,
	MP_CONFIG_CMD_SWITCH_BT_PATH,
	MP_CONFIG_CMD_GET_RFE_TYPE,
	MP_CONFIG_CMD_GET_DEV_IDX,
	MP_CONFIG_CMD_TRIGGER_FW_CONFLICT,
	MP_CONFIG_CMD_GET_UUID,
	MP_CONFIG_CMD_SET_REGULATION,
	MP_CONFIG_CMD_SET_BT_UART,
	MP_CONFIG_CMD_MAX,
};

/* mp rx command */
enum mp_rx_cmd {
	MP_RX_CMD_PHY_CRC_OK = 0,
	MP_RX_CMD_PHY_CRC_ERR = 1,
	MP_RX_CMD_MAC_CRC_OK = 2,
	MP_RX_CMD_MAC_CRC_ERR = 3,
	MP_RX_CMD_DRV_CRC_OK = 4,
	MP_RX_CMD_DRV_CRC_ERR = 5,
	MP_RX_CMD_GET_RSSI = 6,
	MP_RX_CMD_GET_RXEVM = 7,
	MP_RX_CMD_GET_PHYSTS = 8,
	MP_RX_CMD_TRIGGER_RXEVM = 9,
	MP_RX_CMD_SET_GAIN_OFFSET = 10,
	MP_RX_CMD_GET_RSSI_EX = 11,
	MP_RX_CMD_MAX,
};

/* mp efuse command */
enum mp_efuse_cmd {
	MP_EFUSE_CMD_WIFI_READ = 0,
	MP_EFUSE_CMD_WIFI_WRITE = 1,
	MP_EFUSE_CMD_WIFI_UPDATE = 2,
	MP_EFUSE_CMD_WIFI_UPDATE_MAP = 3,
	MP_EFUSE_CMD_WIFI_GET_OFFSET_MASK = 4,
	MP_EFUSE_CMD_WIFI_GET_USAGE = 5,
	MP_EFUSE_CMD_BT_READ = 6,
	MP_EFUSE_CMD_BT_WRITE = 7,
	MP_EFUSE_CMD_BT_UPDATE = 8,
	MP_EFUSE_CMD_BT_UPDATE_MAP = 9,
	MP_EFUSE_CMD_BT_GET_OFFSET_MASK = 10,
	MP_EFUSE_CMD_BT_GET_USAGE = 11,
	MP_EFUSE_CMD_WIFI_GET_LOG_SIZE = 12,
	MP_EFUSE_CMD_WIFI_GET_SIZE = 13,
	MP_EFUSE_CMD_WIFI_GET_AVL_SIZE = 14,
	MP_EFUSE_CMD_AUTOLOAD_STATUS = 15,
	MP_EFUSE_CMD_SHADOW_MAP2BUF = 16,
	MP_EFUSE_CMD_FILE_MAP_LOAD = 17,
	MP_EFUSE_CMD_FILE_MASK_LOAD = 18,
	MP_EFUSE_CMD_GET_INFO = 19,
	/* BT */
	MP_EFUSE_CMD_BT_GET_LOG_SIZE = 20,
	MP_EFUSE_CMD_BT_GET_SIZE = 21,
	MP_EFUSE_CMD_BT_GET_AVL_SIZE = 22,
	MP_EFUSE_CMD_BT_SHADOW_MAP2BUF = 23,
	MP_EFUSE_CMD_BT_FILE_MAP_LOAD = 24,
	MP_EFUSE_CMD_BT_FILE_MASK_LOAD = 25,
	MP_EFUSE_CMD_BT_READ_HIDDEN = 26,
	MP_EFUSE_CMD_BT_WRITE_HIDDEN = 27,
	MP_EFUSE_CMD_WIFI_GET_MAP_FROM = 28,
	MP_EFUSE_CMD_WIFI_GET_PHY_MAP = 29,
	MP_EFUSE_CMD_BT_GET_PHY_MAP = 30,
	MP_EFUSE_CMD_WIFI_SET_RENEW = 31,
	MP_EFUSE_CMD_WIFI_GET_MASK_BUF = 32,
	MP_EFUSE_CMD_BT_GET_MASK_BUF = 33,
	MP_EFUSE_CMD_MAX,
};

/* mp reg command */
enum mp_reg_cmd {
	MP_REG_CMD_READ_MAC = 0,
	MP_REG_CMD_WRITE_MAC = 1,
	MP_REG_CMD_READ_RF = 2,
	MP_REG_CMD_WRITE_RF = 3,
	MP_REG_CMD_READ_SYN = 4,
	MP_REG_CMD_WRITE_SYN = 5,
	MP_REG_CMD_READ_BB = 6,
	MP_REG_CMD_WRITE_BB = 7,
	MP_REG_CMD_SET_XCAP = 8,
	MP_REG_CMD_GET_XCAP = 9,
	MP_REG_CMD_GET_XSI=10,
	MP_REG_CMD_SET_XSI=11,
	MP_REG_CMD_MAX,
};


/* mp tx power command */
enum mp_txpwr_cmd {
	MP_TXPWR_CMD_READ_PWR_TABLE = 0,
	MP_TXPWR_CMD_GET_PWR_TRACK_STATUS = 1,
	MP_TXPWR_CMD_SET_PWR_TRACK_STATUS = 2,
	MP_TXPWR_CMD_SET_TXPWR = 3,
	MP_TXPWR_CMD_GET_TXPWR = 4,
	MP_TXPWR_CMD_GET_TXPWR_INDEX = 5,
	MP_TXPWR_CMD_GET_THERMAL = 6,
	MP_TXPWR_CMD_GET_TSSI = 7,
	MP_TXPWR_CMD_SET_TSSI = 8,
	MP_TXPWR_CMD_GET_TXPWR_REF = 9,
	MP_TXPWR_CMD_GET_TXPWR_REF_CW = 10,
	MP_TXPWR_CMD_SET_TXPWR_INDEX = 11,
	MP_TXPWR_CMD_GET_TXINFOPWR = 12,
	MP_TXPWR_CMD_SET_RFMODE = 13,
	MP_TXPWR_CMD_SET_TSSI_OFFSET = 14,
	MP_TXPWR_CMD_GET_ONLINE_TSSI_DE = 15,
	MP_TXPWR_CMD_SET_PWR_LMT_EN = 16,
	MP_TXPWR_CMD_GET_PWR_LMT_EN = 17,
	MP_TXPWR_CMD_SET_TX_POW_PATTERN_SHARP = 18,
	MP_TXPWR_CMD_MAX,
};

enum mp_flash_cmd {
	MP_FLASH_CMD_WIFI_READ,
	MP_FLASH_CMD_WIFI_WRITE,
	MP_FLASH_CMD_MAX,
};

/* mp cal command */
enum mp_cal_cmd {
	MP_CAL_CMD_TRIGGER_CAL = 0,
	MP_CAL_CMD_SET_CAPABILITY_CAL = 1,
	MP_CAL_CMD_GET_CAPABILITY_CAL = 2,
	MP_CAL_CMD_GET_TSSI_DE_VALUE = 3,
	MP_CAL_CMD_SET_TSSI_DE_TX_VERIFY = 4,
	MP_CAL_CMD_GET_TXPWR_FINAL_ABS = 5,
	MP_CAL_CMD_TRIGGER_DPK_TRACKING = 6,
	MP_CAL_CMD_SET_TSSI_AVG = 7,
	MP_CAL_CMD_PSD_INIT = 8,
	MP_CAL_CMD_PSD_RESTORE = 9,
	MP_CAL_CMD_PSD_GET_POINT_DATA = 10,
	MP_CAL_CMD_PSD_QUERY = 11,
	MP_CAL_CMD_EVENT_TRIGGER = 12,
	MP_CAL_CMD_TRIGGER_WATCHDOG_CAL = 13,
	MP_CAL_CMD_MAX,
};

enum mp_plcp_pattern {
	MP_PLCP_PATTERN_1 = 1,
	MP_PLCP_PATTERN_2 = 2,
	MP_PLCP_PATTERN_MAX,
};

enum mp_calibration_type {
	MP_CAL_CHL_RFK = 0,
	MP_CAL_DACK = 1,
	MP_CAL_IQK = 2,
	MP_CAL_LCK = 3,
	MP_CAL_DPK = 4,
	MP_CAL_DPK_TRACK = 5,
	MP_CAL_TSSI = 6,
	MP_CAL_GAPK = 7,
	MP_CAL_MAX,
};


/*
 *	Command structure definition.
 *	Fixed part would be mp_class/cmd/cmd_ok for command and report parsing.
 *	Data members might have input or output usage.
 */

struct mp_arg_hdr {
	u8 mp_class;
	u8 cmd;
	u8 cmd_ok;
	u8 status;
};

struct mp_config_arg {
	u8 mp_class;
	u8 cmd;
	u8 cmd_ok;
	u8 status;
	u8 channel;
	u8 bandwidth;
	u8 rate_idx;
	u8 ant_tx;
	u8 tx_rfpath;
	u8 ant_rx;
	u8 rx_rfpath;
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
	u8 frc_switch;
	u8 is_tmac_mode;
	u32 drv_ver;
	u8 phy_idx;
	u8 is_bt_uart;
};

struct mp_tx_arg {
	u8 mp_class;
	u8 cmd;
	u8 cmd_ok;
	u8 status;
	u8 tx_method;
	u8 plcp_ppdu_type;	//offline gen
	u16 plcp_case_id;	//offline gen
	u8 bCarrierSuppression;
	u8 is_cck;
	u8 start_tx;
	u16 tx_cnt;
	u16 period;		/* us */
	u16 tx_time;	/* us */ /* tx duty for FW trigger tx */
	u32 tx_ok;
	u8 tx_path;
	u8 tx_mode;		/* mode: 0 = tmac, 1 = pmac */
	u8 tx_concurrent_en;	/* concurrent tx */
	u8 dpd_bypass;
	/* plcp info */
	u32 dbw; //0:BW20, 1:BW40, 2:BW80, 3:BW160/BW80+80
	u32 source_gen_mode;
	u32 locked_clk;
	u32 dyn_bw;
	u32 ndp_en;
	u32 long_preamble_en; //bmode
	u32 stbc;
	u32 gi; //0:0.4,1:0.8,2:1.6,3:3.2
	u32 tb_l_len;
	u32 tb_ru_tot_sts_max;
	u32 vht_txop_not_allowed;
	u32 tb_disam;
	u32 doppler;
	u32 he_ltf_type;//0:1x,1:2x,2:4x
	u32 ht_l_len;
	u32 preamble_puncture;
	u32 he_mcs_sigb;//0~5
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
	u32 ppdu_type;//0: bmode,1:Legacy,2:HT_MF,3:HT_GF,4:VHT,5:HE_SU,6:HE_ER_SU,7:HE_MU,8:HE_TB
	u32 txop;
	u32 tb_strt_sts;
	u32 tb_pre_fec_padding_factor;
	u32 cbw;
	u32 txsc;
	u32 tb_mumimo_mode_en;
	u32 nominal_t_pe; // def = 2
	u32 ness; // def = 0
	u32 n_user;
	u32 tb_rsvd;//def = 0
	/* plcp user info */
	u32 plcp_usr_idx;
	u32 mcs;
	u32 mpdu_len;
	u32 n_mpdu;
	u32 fec;
	u32 dcm;
	u32 aid;
	u32 scrambler_seed; // rand (1~255)
	u32 random_init_seed; // rand (1~255)
	u32 apep;
	u32 ru_alloc;
	u32 nss;
	u32 txbf;
	u32 pwr_boost_db;
	//struct mp_plcp_param_t plcp_param;	//online gen
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

	/* tx state*/
	u8 tx_state;

	/* bb loop back*/
	u8 enable;
	u8 is_dgt;
	u8 cck_lbk_en;
};

struct mp_rx_arg {
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
	u32 phy_user0_rxevm;
	u32 phy_user1_rxevm;
	u32 phy_user2_rxevm;
	u32 phy_user3_rxevm;
	s8 offset;
	u8 rf_path;
	u8 iscck;
	s8 rssi_ex[4];
	u8 rx_phy_idx;
};

struct mp_efuse_arg {
	u8 mp_class;
	u8 cmd;
	u8 cmd_ok;
	u8 status;
	u8 io_type;
	u16 io_offset;
	u32 io_value;
	u8 autoload;
	u8 pfile_path[200];
	u16 buf_len;
	u8 poutbuf[1536];
};

enum mp_efuse_type {
	PHL_MP_EFUSE_WIFI = 0,
	PHL_MP_EFUSE_BT,
	PHL_MP_EFUSE_NONE,
};

struct mp_reg_arg {
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

struct mp_txpwr_arg {
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
	s16 table_item; //get an element of power table
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
	u8 cur_phy;
};

struct mp_cal_arg {
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
	u32 outbuf[400];
	u8 event;
	u8 func;
};

struct mp_flash_arg {
	u8 mp_class;
	u8 cmd;
	u8 cmd_ok;
	u8 io_offset;
	u8 io_value;
};

struct rtw_phl_com_t;

struct phl_mp_watchdog {
	_os_timer wdog_timer;
	u16 period;
};

struct mp_context {
	u8 status;
	u8 cur_phy;
	u8 rx_physts;
	_os_sema mp_cmd_sema;
	void *buf;
	u32 buf_len;
	void *rpt;
	u32 rpt_len;
	struct test_obj_ctrl_interface mp_test_ctrl;
	u8 is_mp_test_end;
	struct phl_info_t *phl;
	struct rtw_phl_com_t *phl_com;
	void *hal;
	struct mp_usr_plcp_gen_in usr[4];
	u32 max_para;
	struct phl_mp_watchdog mp_wdog;
	struct mp_cal_arg cal_arg;
};
#endif /* CONFIG_PHL_TEST_MP */

#endif /* _PHL_TEST_MP_DEF_H_ */

/******************************************************************************
 *
 * Copyright(c) 2007 - 2020  Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * The full GNU General Public License is included in this distribution in the
 * file called LICENSE.
 *
 * Contact Information:
 * wlanfae <wlanfae@realtek.com>
 * Realtek Corporation, No. 2, Innovation Road II, Hsinchu Science Park,
 * Hsinchu 300, Taiwan.
 *
 * Larry Finger <Larry.Finger@lwfinger.net>
 *
 *****************************************************************************/

#ifndef __HALBB_H__
#define __HALBB_H__

/*@--------------------------[Define] ---------------------------------------*/

#define	ACTIVE_TP_THRESHOLD	1
#define BB_WATCH_DOG_PERIOD	2 /*sec*/
#define is_sta_active(sta)	((sta) && (sta->active))
#define HALBB_SNPRINT_SIZE	200
#define HALBB_SNPRINT_SIZE_S	20

#define BB_EFUSE_BAND_NUM	5
#define IC_LNA_NUM		7
#define IC_TIA_NUM		2
#define IC_LNA_OP1DB_NUM	7
#define IC_TIA_LNA_OP1DB_NUM	8
#define EFUSE_OFST_NUM		1
#define WB_GIDX_ELNA_NUM	16
#define GS_IDX_NUM		2
#define G_ELNA_NUM		2

/*@--------------------------[Enum]------------------------------------------*/

enum bb_trx_state_t {
	BB_TX_STATE	= 0,
	BB_RX_STATE	= 1,
	BB_BI_DIR_STATE	= 2
};

enum bb_trafic_t {
	TRAFFIC_NO_TP		= 0,
	TRAFFIC_ULTRA_LOW	= 1,
	TRAFFIC_LOW		= 2,
	TRAFFIC_MID		= 3,
	TRAFFIC_HIGH		= 4
};

enum efuse_bit_mask {
	LOW_MASK = 0,
	HIGH_MASK = 1
};
/*@--------------------------[Structure]-------------------------------------*/

struct halbb_pause_lv {
	s8			lv_fa_cnt;
	s8			lv_dig;
	s8			lv_cfo;
	s8			lv_edcca;
};

struct bb_func_hooker_info {
	void (*pause_bb_dm_handler)(struct bb_info *bb, u32 *val_buf, u8 val_len);
};

struct	bb_iot_info {
	u8		is_linked_cmw500:1;
	u8		patch_id_00000000:1;
	u8		rsvd:6;
};

struct bb_path_info {
	/*[Path info]*/
	u8			tx_path_en; /*TX path enable*/
	u8			rx_path_en; /*RX path enable*/
	#ifdef HALBB_COMPILE_ABOVE_4SS
	enum bb_path		tx_4ss_path_map; /*@Use N-X for 4STS rate*/
	#endif
	#ifdef HALBB_COMPILE_ABOVE_3SS
	enum bb_path		tx_3ss_path_map; /*@Use N-X for 3STS rate*/
	#endif
	#ifdef HALBB_COMPILE_ABOVE_2SS
	enum bb_path		tx_2ss_path_map; /*@Use N-X for 2STS rate*/
	#endif
	enum bb_path		tx_1ss_path_map; /*@Use N-X for 1STS rate*/
};

struct bb_link_info {
	/*[Link Info]*/
	bool			is_linked;
	bool			is_linked_pre;
	bool			first_connect;
	bool			first_disconnect;
	enum bb_trx_state_t	txrx_state_all;
	/*[One Entry TP Info]*/
	bool			is_one_entry_only;
	u32			one_entry_macid;
	u32			one_entry_tp;
	u32			one_entry_tp_pre;
	u16			tp_active_th;
	bool			one_entry_tp_active_occur;
	bool			is_match_bssid;
	/*[Client Number]*/
	u8			num_linked_client;
	u8			num_linked_client_pre;
	u8			num_active_client;
	u8			num_active_client_pre;
	/*[TP & Traffic]*/
	u8			traffic_load;
	u8			traffic_load_pre;
	u16			tx_rate;
	u16			rx_rate_plurality;
	u16			rx_rate_plurality_mu;
	u32			tx_tp;			/*@Mbps*/
	u32			rx_tp;			/*@Mbps*/
	u32			total_tp;		/*@Mbps*/
	u16			consecutive_idle_time;	/*@unit: second*/
	u16			wlan_mode_bitmap; /*enum wlan_mode*/
	/*[at least one macid BFer & BFee chk]*/
	bool			at_least_one_bfer; /*at least one macid Tx BFer*/
	bool			at_least_one_bfee; /*at least one macid Rx BFee*/
};

struct bb_ch_info {
	u8 			fc_ch_idx;
	bool			is_2g;
	u8			rssi_min; /*U(8.1), external rssi (from antenna)*/
	u16			rssi_min_macid;
	u8			pre_rssi_min;
	u8			rssi_max; /*U(8.1), external rssi (from antenna)*/
	u16			rssi_max_macid;
	u8			rxsc_160;
	u8			rxsc_80;
	u8			rxsc_40;
	u8			rxsc_20;
	u8			rxsc_l;
	u8			is_noisy;
	u8			rf_central_ch_cfg; /*report in phy-sts*/
	s8			ext_loss[HALBB_MAX_PATH]; /*S(8.2), update when switching ch*/
	s8			ext_loss_avg; /*S(8.2), update when switching ch*/
	u8			int_rssi_min; /*U(8.1), internal rssi (from ADC output) = external rssi - ext_loss*/
	u8			int_rssi_max; /*U(8.1), internal rssi (from ADC output) = external rssi - ext_loss*/
};

struct bb_cmn_backup_info {
	u8			cur_tx_path;
	u8			cur_rx_path;
	s16			cur_tx_pwr;
	u8			cur_pd_lower_bound;
	u8			last_rpl;
	u32			last_rssi;
	u16			last_cfo;
	s32			cck_ps_th_bk;
	s32			cck_rssi_ofst_bk;
	s32			cck_sbd_th_bk;
	u32			cur_rfmode_a_12ac;
	u32			cur_rfmode_a_12b0;
	u32			cur_rfmode_b_32ac;
	u32			cur_rfmode_b_32b0;
	struct rssi_physts 	last_rssi_rpt;
	struct rxevm_physts 	last_rxevm_rpt;
};

struct bb_gain_info {
	s8 lna_gain[BB_GAIN_BAND_NUM][HALBB_MAX_PATH][IC_LNA_NUM];
	s8 tia_gain[BB_GAIN_BAND_NUM][HALBB_MAX_PATH][IC_TIA_NUM];
	s8 lna_gain_bypass[BB_GAIN_BAND_NUM][HALBB_MAX_PATH][IC_LNA_NUM];
	s8 lna_op1db[BB_GAIN_BAND_NUM][HALBB_MAX_PATH][IC_LNA_NUM];
	s8 tia_lna_op1db[BB_GAIN_BAND_NUM][HALBB_MAX_PATH][IC_LNA_NUM + 1]; // TIA0_LNA0~6 + TIA1_LNA6
	s8 efuse_ofst[BB_GAIN_BAND_NUM][HALBB_MAX_PATH][EFUSE_OFST_NUM];
	s8 rpl_ofst_20[BB_GAIN_BAND_NUM][HALBB_MAX_PATH];
	s8 rpl_ofst_40[BB_GAIN_BAND_NUM][HALBB_MAX_PATH][BB_RXSC_NUM_40];
	s8 rpl_ofst_80[BB_GAIN_BAND_NUM][HALBB_MAX_PATH][BB_RXSC_NUM_80];
	s8 rpl_ofst_160[BB_GAIN_BAND_NUM][HALBB_MAX_PATH][BB_RXSC_NUM_160];
	u32 wb_gidx_elna[BB_GAIN_BAND_NUM][HALBB_MAX_PATH]; // {bit(15) ~ bit(0)}: {wb_gidx_15_elna ~ wb_gidx_0_elna}
	u8 wb_gidx_lna_tia[BB_GAIN_BAND_NUM][HALBB_MAX_PATH][WB_GIDX_ELNA_NUM];
	u32 gs_idx[BB_GAIN_BAND_NUM][HALBB_MAX_PATH][GS_IDX_NUM];
	u8 g_elna[BB_GAIN_BAND_NUM][HALBB_MAX_PATH][G_ELNA_NUM];
};

struct bb_efuse_info{
	bool normal_efuse_check;
	bool hidden_efuse_check;
	s8 gain_offset[HALBB_MAX_PATH][BB_BAND_NUM_MAX + 1]; // S(8,0)
	s8 gain_cs[HALBB_MAX_PATH][BB_BAND_NUM_MAX]; // S(8,0)
	s8 gain_cg[HALBB_MAX_PATH][BB_BAND_NUM_MAX]; // S(8,0)
	s8 hidden_efuse[BB_HIDE_EFUSE_SIZE];

	s8 lna_err_2g[HALBB_MAX_PATH][7]; // S(6,2)
	s8 lna_err_5g[HALBB_MAX_PATH][7]; // S(6,2)

	s8 frontend_loss[HALBB_MAX_PATH];
	s8 rpl_bias_comp[HALBB_MAX_PATH];
	s8 rssi_bias_comp[HALBB_MAX_PATH];

	s8 efuse_ofst[HW_PHY_MAX]; // 8852A:S(5,2) 8852B:S(8,4)
	s8 efuse_ofst_path[HALBB_MAX_PATH]; // 8852C: S(8,4)
	s8 efuse_ofst_tb[HW_PHY_MAX]; // 8852A:S(7,4) 8852B:S(8,4)
	s8 efuse_ofst_tb_path[HALBB_MAX_PATH]; // 8852C: S(8,4)

	u8 efuse_ft; // 8192XB: U(8,0)
	u8 efuse_adc_td; // 8192XB: U(2,0)
};

struct vht_mu_cr_backup_table {
	u32 muic_en_a;
	u32 lpbw_sw_symb0_a;
	u32 ch_tracking_symb0_a;
	u32 lpbw_sel_d1_a;
	u32 ch_tracking_symb1_a;
	u32 ch_tracking_a1_a;
	u32 lpbw_sel_p1_a;
	u32 noise_tracking_en_a;
};

struct bb_cmn_dbg_info {
	bool cmn_log_2_cnsl_en;
	bool cmn_log_2_drv_statistic_en;
};

struct bb_cmn_info {
	u8 bb_dm_number;
	bool cck_blk_en;
	enum phl_phy_idx cck_phy_map;
	bool bb_dbcc_en;
	bool ic_dual_phy_support;
	bool ic_dbcc_support;
#ifdef BB_8922A_DVLP_SPF
	enum mlo_dbcc_mode_type bb_mlo_dbcc_mode_t;
#endif
#ifdef HALBB_RA_SUPPORT
	struct bb_ra_info	bb_ra_i[PHL_MAX_STA_NUM];
#endif
#ifdef HALBB_PSD_SUPPORT
	struct bb_psd_info	bb_psd_i;
#endif
#ifdef HALBB_LA_MODE_SUPPORT
	struct bb_la_mode_info	bb_la_mode_i;
#endif
#ifdef HALBB_SNIF_SUPPORT
	struct bb_snif_info	bb_snif_i;
#endif
#ifdef HALBB_DYN_CSI_RSP_SUPPORT
	struct bf_ch_raw_info bf_ch_raw_i;
#endif
	struct bb_spur_info bb_spur_i;
	struct bb_echo_cmd_info	bb_echo_cmd_i;
	struct bb_func_hooker_info bb_func_hooker_i;
	struct bb_cmn_dbg_info bb_cmn_dbg_i;
#ifdef HALBB_FW_OFLD_SUPPORT
	u8 bbcr_fwofld_state;
	bool skip_io_init_en;
#endif
	bool bb_fwofld_in_progress;
	u32 bb_fwofld_sup_bitmap; /*enum fw_ofld_type.For HALBB to control DBCC-OFLD manully*/
	u32 bb_fwofld_start_time;
};

#ifdef HALBB_DIG_MCC_SUPPORT
#define PD_IDX_MIN	0
#define NUM_MAX_IGI_CNT	7
#define INVALID_INIT_VAL 0xff

/*For 2G/5G/6G*/
enum mcc_band {
	MCC_BAND_1 = 0,
	MCC_BAND_2,
	MCC_BAND_NUM
};

struct halbb_mcc_dm {
	bool		mcc_pre_status_en;
	u8		mcc_reg_id[NUM_MAX_IGI_CNT];
	u8		sta_cnt[MCC_BAND_NUM];
	u16		mcc_dm_reg[NUM_MAX_IGI_CNT];
	u16		mcc_dm_mask[NUM_MAX_IGI_CNT];
	u16		mcc_dm_val[NUM_MAX_IGI_CNT][MCC_BAND_NUM];
	/*mcc DIG*/
	u8		rssi_min[MCC_BAND_NUM];

	/* need to be config by driver*/
	bool		mcc_status_en;
	u8		softap_macid;
	struct rtw_chan_def mcc_rf_ch[MCC_BAND_NUM];

};
/**
 * @struct _mcc_h2c_
 * @brief _mcc_h2c_
 *
 */
 struct mcc_h2c_reg_content {
	// MCCDM
	u8 addr_lsb;
	u8 addr_msb;
	u8 bmask_lsb;
	u8 bmask_msb;
	u8 val_lsb;
	u8 val_msb;
};

struct mcc_h2c {
	// MCCDM
	u8 reg_cnt;
	u8 mcc_dm_en: 1;
	u8 mcc_ch_idx: 1;
	u8 mcc_set: 1;
	u8 phy0_en: 1;
	u8 phy1_en: 1;
	u8 rsvd0: 3;
	u8 ch_lsb;
	u8 ch_msb;
	struct mcc_h2c_reg_content mcc_reg_content[NUM_MAX_IGI_CNT];
};
#endif

struct bb_info {
	struct rtw_phl_com_t	*phl_com;
	struct rtw_hal_com_t	*hal_com;
	struct rtw_phl_stainfo_t *phl_sta_info[PHL_MAX_STA_NUM];
	u8			phl2bb_macid_table[PHL_MAX_STA_NUM];
	bool			sta_exist[PHL_MAX_STA_NUM];
	/*[DBCC]*/
#ifdef HALBB_DBCC_SUPPORT
	struct bb_info		*bb_phy_hooker;
#endif
	enum phl_phy_idx	bb_phy_idx;
	struct bb_cmn_info	*bb_cmn_hooker;
	/*[Common Info]*/
	u32			bb0_cr_offset;
	u32			bb0_mcu_cr_offset;
	struct bb_gain_info	bb_gain_i;
	struct bb_efuse_info	bb_efuse_i;
	enum bb_ic_t		ic_type;
	enum bb_ic_sub_t	ic_sub_type;
	enum bb_cr_t		cr_type;
	enum bb_80211spec_t	bb_80211spec;
	u8			num_rf_path;
	u8			bb_sta_cnt;
	/*[System Info]*/
	bool			is_mp_mode_pre;
	enum bb_watchdog_mode_t bb_watchdog_mode;
	bool			bb_cmn_info_init_ready;
	bool			bb_dm_init_ready;
	u32			bb_sys_up_time;
	bool			bb_watchdog_en;
	u8			bb_watchdog_period; /*2s, 4s, 8s...,254s*/
	bool			bb_ic_api_en;
	u8			pre_dbg_priority;
	char			dbg_buf[HALBB_SNPRINT_SIZE];
	/*[DM Info]*/
	bool			is_noisy;
	bool			adv_bb_dm_en;
	u64			support_ability;	/*HALBB function Supportability*/
	u64			manual_support_ability;
	u32			pause_ability;		/*HALBB function pause Supportability*/
	struct halbb_pause_lv	pause_lv_table;
	/*[FW Info]*/
	u8			fwofld_last_cmd;
	u64			fw_dbg_component;
	/*[Drv Dbg Info]*/
	u64			dbg_component;
	u64			mcu_dbg_component;
	u8			cmn_dbg_msg_period;
	u16			cmn_dbg_msg_component;
	u8			cmn_dbg_msg_cnt;
	bool			is_disable_phy_api;
	/*[Dummy]*/
	bool			bool_dummy;
	u8			u8_dummy;
	u16			u16_dummy;
	u32			u32_dummy;
	/*[Link Info]*/
	enum rf_path 		tx_path; /*PMAC Tx Path*/
	enum rf_path 		rx_path;
	/*[pmac]*/
	bool			dyn_pmac_tri_en;
	bool			pmac_tri_en;
	bool			pwr_comp_en;
	u32			pmac_tri_idx;
	u32			pmac_pwr_ofst;
	/*[btc]*/
	bool			bt_en; /*backup for ch_bw switch*/

	/*@=== [HALBB Structure] ============================================*/
#ifdef BB_8852A_2_SUPPORT
	struct bb_8852a_2_info	bb_8852a_2_i;
	struct bb_h2c_fw_cmw	bb_fw_cmw_i;
	struct vht_mu_cr_backup_table vht_mu_backup_val;
#endif
#ifdef HALBB_DBCC_SUPPORT
	struct bb_dbcc_info	bb_dbcc_i;
#endif
#ifdef HALBB_ENV_MNTR_SUPPORT
	struct bb_env_mntr_info bb_env_mntr_i;
#endif
#ifdef HALBB_EDCCA_SUPPORT
	struct bb_edcca_info	bb_edcca_i;
#endif
#ifdef HALBB_DFS_SUPPORT
	struct bb_dfs_info	bb_dfs_i;
#endif
#ifdef HALBB_STATISTICS_SUPPORT
	struct bb_stat_info	bb_stat_i;
#endif
#ifdef HALBB_DIG_SUPPORT
	struct bb_dig_info	bb_dig_i;
#endif
#ifdef HALBB_CFO_TRK_SUPPORT
	struct bb_cfo_trk_info	bb_cfo_trk_i;
#endif
#ifdef HALBB_UL_TB_CTRL_SUPPORT
	struct bb_ul_tb_info	bb_ul_tb_i;
#endif
#ifdef HALBB_PHYSTS_PARSING_SUPPORT
	struct bb_physts_info	bb_physts_i;
#endif
#ifdef HALBB_LA_MODE_SUPPORT
	struct bb_la_mode_info	bb_la_mode_i;
#endif

#ifdef HALBB_PWR_CTRL_SUPPORT
	struct bb_pwr_ctrl_info	bb_pwr_ctrl_i;
	struct bb_dyncca_info	bb_dyncca_i;
#endif

#ifdef HALBB_PMAC_TX_SUPPORT
	struct bb_plcp_info bb_plcp_i;
#endif
	struct bb_link_info	bb_link_i;
	struct bb_path_info	bb_path_i;
	struct bb_ch_info	bb_ch_i;
	struct bb_api_info	bb_api_i;
	struct bb_iot_info	bb_iot_i;
	struct bb_dbg_info	bb_dbg_i;
	struct bb_c2h_fw_tx_rpt	bb_fwtx_c2h_i;
	struct bb_h2c_fw_tx_setting	bb_fwtx_h2c_i;
	struct bb_h2c_fw_edcca	bb_fw_edcca_i;
	struct bb_h2c_ehtsig_sigb	bb_h2c_ehtsig_sigb_i;
	struct bb_fw_dbg_cmn_info	bb_fwdbg_i;
	struct bb_cmn_rpt_info	bb_cmn_rpt_i;
	struct bb_rpt_info bb_rpt_i;
	struct rxevm_physts	rxevm;
	struct bb_cmn_backup_info	bb_cmn_backup_i;
	struct bb_spur_info 	bb_spur_i;
#ifdef HALBB_CH_INFO_SUPPORT
	struct bb_ch_rpt_info	bb_ch_rpt_i;
#endif
#ifdef HALBB_AUTO_DBG_SUPPORT
	struct bb_auto_dbg_info bb_auto_dbg_i;
#endif
#ifdef HALBB_ANT_DIV_SUPPORT
	struct bb_antdiv_info bb_ant_div_i;
#endif
#ifdef HALBB_DYN_L2H_SUPPORT
	struct bb_dyn_l2h_info bb_dyn_l2h_i;
#endif
#ifdef HALBB_PATH_DIV_SUPPORT
	struct bb_pathdiv_info bb_path_div_i;
#endif
	/*@=== [HALBB Timer] ================================================*/
#ifdef HALBB_RUA_SUPPORT
	/*struct rtw_rua_tbl rtw_rua_t;*/
#endif
#ifdef HALBB_DIG_MCC_SUPPORT
	struct halbb_mcc_dm mcc_dm;
#endif
#ifdef HALBB_DYN_1R_CCA_SUPPORT
	struct bb_dyn_1r_cca_info bb_dyn_1r_cca_i;
#endif
#ifdef HALBB_DYN_DTR_SUPPORT
	struct bb_dyn_dtr_info bb_dyn_dtr_i;
#endif
#ifdef HALBB_FW_OFLD_SUPPORT
	enum phl_msg_evt_id bb_phl_evt;
#endif

};


/*@--------------------------[Prptotype]-------------------------------------*/
u8 halbb_get_rssi_min(struct bb_info *bb);
void halbb_cmn_info_self_reset(struct bb_info *bb);
void halbb_sta_info_dbg(struct bb_info *bb, char input[][16], u32 *_used,
			char *output, u32 *_out_len);
void halbb_supportability_dbg(struct bb_info *bb, char input[][16], u32 *_used,
			     char *output, u32 *_out_len);
void halbb_pause_func_dbg(struct bb_info *bb, char input[][16], u32 *_used,
			  char *output, u32 *_out_len);
void halbb_store_data(struct bb_info *bb);
#endif

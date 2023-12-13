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
#ifndef _HALRF_PWR_TRACK_H_
#define _HALRF_PWR_TRACK_H_

/*@--------------------------Define Parameters-------------------------------*/
#define AVG_THERMAL_NUM 8
#define MAX_RF_PATH 4
#define DELTA_SWINGIDX_SIZE 30
#define BAND_NUM 3
#define BAND_NUM_6G 4
#define DELTA_SWINTSSI_SIZE 61
#define TSSI_EFUSE_NUM 32
#define TSSI_BW_DIFF_EFUSE_NUM 14
#define TSSI_HIDE_EFUSE_NUM 8
#define TSSI_HIDE_EFUSE_NUM_6G 16
#define AVG_THERMAL_NUM_TSSI 2
#define MAX_CH_NUM 67
#define TSSI_ALIMK_VAULE_NUM 8

#define MAX_HALRF_PATH 2

#define TSSI_EXTRA_GROUP_BIT (BIT(31))
#define TSSI_EXTRA_GROUP(idx) (TSSI_EXTRA_GROUP_BIT | (idx))
#define IS_TSSI_EXTRA_GROUP(group) ((group) & TSSI_EXTRA_GROUP_BIT)
#define TSSI_EXTRA_GET_GROUP_IDX1(group) ((group) & ~TSSI_EXTRA_GROUP_BIT)
#define TSSI_EXTRA_GET_GROUP_IDX2(group) (TSSI_EXTRA_GET_GROUP_IDX1(group) + 1)

/*@---------------------------End Define Parameters---------------------------*/

enum halrf_tssi_rate_type {
	EFUSE_TSSI_CCK = 0,
	EFUSE_TSSI_MCS,
	EFUSE_TSSI_6G,
	EFUSE_TSSI_RATE_MAX
};

enum halrf_tssi_type{
	TSSI_OFF = 0,
	TSSI_ON,
	TSSI_CAL
};

enum halrf_tssi_slope_type{
	TSSI_SLOPE_DEFAULT = 0,
	TSSI_SLOPE_ON,
	TSSI_SLOPE_OFF
};

enum halrf_tssi_alimk_band{
	TSSI_ALIMK_2G = 0,
	TSSI_ALIMK_5GL,
	TSSI_ALIMK_5GM,
	TSSI_ALIMK_5GH,
	TSSI_ALIMK_MAX
};

struct halrf_pwr_track_info {
	/* u8 is_txpowertracking; */
	u8	tx_powercount;
	bool    is_txpowertracking_init;
	bool    is_txpowertracking;
	u8  	txpowertrack_control; /* for mp mode, turn off txpwrtracking as default */
	u8	tm_trigger;
	u8  	internal_pa_5g[2];	/* pathA / pathB */

	u8  	thermal_meter[2];    /* thermal_meter, index 0 for RFIC0, and 1 for RFIC1 */
	u8	thermal_value;
	u8	thermal_value_path[MAX_RF_PATH];
	u8	thermal_value_lck;
	u8	thermal_value_iqk;
	s8  	thermal_value_delta; /* delta of thermal_value and efuse thermal */
	u8	thermal_value_avg[AVG_THERMAL_NUM];
	u8	thermal_value_avg_path[MAX_RF_PATH][AVG_THERMAL_NUM];
	u8	thermal_value_avg_index;
	u8	thermal_value_avg_index_path[MAX_RF_PATH];
	s8	power_index_offset_path[MAX_RF_PATH];

	u8	thermal_value_rx_gain;
	u8	thermal_value_crystal;
	u8	thermal_value_dpk_store;
	u8	thermal_value_dpk_track;
	bool	txpowertracking_in_progress;

	bool	is_reloadtxpowerindex;
	u8	is_rf_pi_enable;
	u32 	txpowertracking_callback_cnt; /* cosa add for debug */

	u8	is_cck_in_ch14;
	u8	CCK_index;
	u8	OFDM_index[MAX_RF_PATH];
	s8	power_index_offset;
	s8	delta_power_index;
	s8	delta_power_index_path[MAX_RF_PATH];
	s8	delta_power_index_last;
	s8	delta_power_index_last_path[MAX_RF_PATH];
	bool is_tx_power_changed;

	/*struct iqk_matrix_regs_setting iqk_matrix_reg_setting[IQK_MATRIX_SETTINGS_NUM];*/
	u8	delta_lck;
	s8  delta_swing_table_idx_2g_cck_a_p[DELTA_SWINGIDX_SIZE];
	s8  delta_swing_table_idx_2g_cck_a_n[DELTA_SWINGIDX_SIZE];
	s8  delta_swing_table_idx_2g_cck_b_p[DELTA_SWINGIDX_SIZE];
	s8  delta_swing_table_idx_2g_cck_b_n[DELTA_SWINGIDX_SIZE];
	s8  delta_swing_table_idx_2g_cck_c_p[DELTA_SWINGIDX_SIZE];
	s8  delta_swing_table_idx_2g_cck_c_n[DELTA_SWINGIDX_SIZE];
	s8  delta_swing_table_idx_2g_cck_d_p[DELTA_SWINGIDX_SIZE];
	s8  delta_swing_table_idx_2g_cck_d_n[DELTA_SWINGIDX_SIZE];
	s8  delta_swing_table_idx_2ga_p[DELTA_SWINGIDX_SIZE];
	s8  delta_swing_table_idx_2ga_n[DELTA_SWINGIDX_SIZE];
	s8  delta_swing_table_idx_2gb_p[DELTA_SWINGIDX_SIZE];
	s8  delta_swing_table_idx_2gb_n[DELTA_SWINGIDX_SIZE];
	s8  delta_swing_table_idx_2gc_p[DELTA_SWINGIDX_SIZE];
	s8  delta_swing_table_idx_2gc_n[DELTA_SWINGIDX_SIZE];
	s8  delta_swing_table_idx_2gd_p[DELTA_SWINGIDX_SIZE];
	s8  delta_swing_table_idx_2gd_n[DELTA_SWINGIDX_SIZE];
	s8  delta_swing_table_idx_5ga_p[BAND_NUM][DELTA_SWINGIDX_SIZE];
	s8  delta_swing_table_idx_5ga_n[BAND_NUM][DELTA_SWINGIDX_SIZE];
	s8  delta_swing_table_idx_5gb_p[BAND_NUM][DELTA_SWINGIDX_SIZE];
	s8  delta_swing_table_idx_5gb_n[BAND_NUM][DELTA_SWINGIDX_SIZE];
	s8  delta_swing_table_idx_5gc_p[BAND_NUM][DELTA_SWINGIDX_SIZE];
	s8  delta_swing_table_idx_5gc_n[BAND_NUM][DELTA_SWINGIDX_SIZE];
	s8  delta_swing_table_idx_5gd_p[BAND_NUM][DELTA_SWINGIDX_SIZE];
	s8  delta_swing_table_idx_5gd_n[BAND_NUM][DELTA_SWINGIDX_SIZE];
	s8 delta_swing_table_idx_6ga_p[BAND_NUM_6G][DELTA_SWINGIDX_SIZE];
	s8 delta_swing_table_idx_6ga_n[BAND_NUM_6G][DELTA_SWINGIDX_SIZE];
	s8 delta_swing_table_idx_6gb_p[BAND_NUM_6G][DELTA_SWINGIDX_SIZE];
	s8 delta_swing_table_idx_6gb_n[BAND_NUM_6G][DELTA_SWINGIDX_SIZE];
	s8 delta_swing_table_idx_6gc_p[BAND_NUM_6G][DELTA_SWINGIDX_SIZE];
	s8 delta_swing_table_idx_6gc_n[BAND_NUM_6G][DELTA_SWINGIDX_SIZE];
	s8 delta_swing_table_idx_6gd_p[BAND_NUM_6G][DELTA_SWINGIDX_SIZE];
	s8 delta_swing_table_idx_6gd_n[BAND_NUM_6G][DELTA_SWINGIDX_SIZE];
	s8  delta_swing_tssi_table_2g_cck_a[DELTA_SWINTSSI_SIZE];
	s8  delta_swing_tssi_table_2g_cck_b[DELTA_SWINTSSI_SIZE];
	s8  delta_swing_tssi_table_2g_cck_c[DELTA_SWINTSSI_SIZE];
	s8  delta_swing_tssi_table_2g_cck_d[DELTA_SWINTSSI_SIZE];
	s8  delta_swing_tssi_table_2ga[DELTA_SWINTSSI_SIZE];
	s8  delta_swing_tssi_table_2gb[DELTA_SWINTSSI_SIZE];
	s8  delta_swing_tssi_table_2gc[DELTA_SWINTSSI_SIZE];
	s8  delta_swing_tssi_table_2gd[DELTA_SWINTSSI_SIZE];
	s8  delta_swing_tssi_table_5ga[BAND_NUM][DELTA_SWINTSSI_SIZE];
	s8  delta_swing_tssi_table_5gb[BAND_NUM][DELTA_SWINTSSI_SIZE];
	s8  delta_swing_tssi_table_5gc[BAND_NUM][DELTA_SWINTSSI_SIZE];
	s8  delta_swing_tssi_table_5gd[BAND_NUM][DELTA_SWINTSSI_SIZE];
	s8  delta_swing_table_xtal_p[DELTA_SWINGIDX_SIZE];
	s8  delta_swing_table_xtal_n[DELTA_SWINGIDX_SIZE];
	s8  delta_swing_table_idx_2ga_p_8188e[DELTA_SWINGIDX_SIZE];
	s8  delta_swing_table_idx_2ga_n_8188e[DELTA_SWINGIDX_SIZE];

	u8			bb_swing_idx_ofdm[MAX_RF_PATH];
	u8			bb_swing_idx_ofdm_current;
	u8			bb_swing_idx_ofdm_base;
	u8			bb_swing_idx_ofdm_base_path[MAX_RF_PATH];
	bool			bb_swing_flag_ofdm;
	u8			bb_swing_idx_cck;
	u8			bb_swing_idx_cck_current;
	u8			bb_swing_idx_cck_base;
	u8			default_ofdm_index;
	u8			default_cck_index;
	bool			bb_swing_flag_cck;

	s8			absolute_ofdm_swing_idx[MAX_RF_PATH];
	s8			remnant_ofdm_swing_idx[MAX_RF_PATH];
	s8			absolute_cck_swing_idx[MAX_RF_PATH];
	s8			remnant_cck_swing_idx;
	s8			modify_tx_agc_value;       /*Remnat compensate value at tx_agc */
	bool			modify_tx_agc_flag_path_a;
	bool			modify_tx_agc_flag_path_b;
	bool			modify_tx_agc_flag_path_c;
	bool			modify_tx_agc_flag_path_d;
	bool			modify_tx_agc_flag_path_a_cck;
	bool			modify_tx_agc_flag_path_b_cck;

	s8			kfree_offset[MAX_RF_PATH];
	/*Add by Yuchen for Kfree Phydm*/
	u8			reg_rf_kfree_enable;	/*for registry*/
	u8			rf_kfree_enable;		/*for efuse enable check*/
};

struct halrf_tssi_info{
	u8 thermal[MAX_HALRF_PATH];
	u8 do_tssi_thermal[MAX_HALRF_PATH];
	s32 tssi_de[MAX_HALRF_PATH];
	u8 tssi_type[MAX_HALRF_PATH];
	s8 tssi_efuse[MAX_HALRF_PATH][EFUSE_TSSI_RATE_MAX][TSSI_EFUSE_NUM];
	s8 tssi_bw_diff_efuse[MAX_HALRF_PATH][EFUSE_TSSI_RATE_MAX][CHANNEL_WIDTH_MAX][TSSI_BW_DIFF_EFUSE_NUM];
	s8 tssi_efuse_slope_gain_diff[MAX_HALRF_PATH][EFUSE_TSSI_RATE_MAX][TSSI_EFUSE_NUM];
	s8 tssi_efuse_slope_cw_diff[MAX_HALRF_PATH][EFUSE_TSSI_RATE_MAX][TSSI_EFUSE_NUM];
	bool tssi_slope_no_pg;
	s8 tssi_trim[MAX_HALRF_PATH][TSSI_HIDE_EFUSE_NUM];
	s8 tssi_trim_6g[MAX_HALRF_PATH][TSSI_HIDE_EFUSE_NUM_6G];
	s32 tssi_xdbm;
	s8 curr_tssi_cck_de[MAX_HALRF_PATH];
	s8 curr_tssi_efuse_cck_de[MAX_HALRF_PATH];
	s8 curr_tssi_ofdm_de[MAX_HALRF_PATH];
	s8 curr_tssi_ofdm_de_20m[MAX_HALRF_PATH];
	s8 curr_tssi_ofdm_de_80m[MAX_HALRF_PATH];
	s8 curr_tssi_ofdm_de_160m[MAX_HALRF_PATH];
	s8 curr_tssi_efuse_ofdm_de[MAX_HALRF_PATH];
	s8 curr_tssi_ofdm_de_diff_20m[MAX_HALRF_PATH];
	s8 curr_tssi_ofdm_de_diff_80m[MAX_HALRF_PATH];
	s8 curr_tssi_ofdm_de_diff_160m[MAX_HALRF_PATH];
	s8 curr_tssi_trim_de[MAX_HALRF_PATH];
	bool do_tssi;
	bool base_thermal_check[MAX_HALRF_PATH];
	u32 base_thermal[MAX_HALRF_PATH];
	u32 tssi_stop_power[MAX_HALRF_PATH];
	bool tssi_tracking_check[MAX_HALRF_PATH];
	u8 ther_avg[MAX_HALRF_PATH][AVG_THERMAL_NUM_TSSI]; /*path*/
	u8 ther_avg_idx[MAX_HALRF_PATH];
	u8 ther_avg_fifo_idx[MAX_HALRF_PATH];
	u32 ther_avg_final[MAX_HALRF_PATH];
	s8 extra_ofst[MAX_HALRF_PATH];
	/*bool normal_tssi_tracking;*/
	u8 normal_tssi_tracking_times;
	u8 default_txagc_offset_check;
	u8 default_txagc_offset_times;
	u32 default_txagc_offset[MAX_HALRF_PATH];
	s16 ref_pow_ofdm; /*-> HW: s(9,2)*/
	s16 ref_pow_ofdm_offset;
	u32 high_pwr_rst_cnt;
	u8 cur_ch;
	u8 cur_bw;
	u32 backup_txagc_offset[MAX_HALRF_PATH][MAX_CH_NUM];
	u8 backup_txagc_oft_ther[MAX_HALRF_PATH][MAX_CH_NUM];
	bool check_backup_txagc[MAX_CH_NUM];
	bool check_backup_aligmk[MAX_HALRF_PATH][MAX_CH_NUM];
	u32 start_time, finish_time;
	u32 alignment_value[MAX_HALRF_PATH][TSSI_ALIMK_MAX][TSSI_ALIMK_VAULE_NUM];
	u32 alignment_backup_by_ch[MAX_HALRF_PATH][MAX_CH_NUM][TSSI_ALIMK_VAULE_NUM];
	bool alignment_done[MAX_HALRF_PATH][TSSI_ALIMK_MAX];
	u32 tssi_total_time;
	u32 tssi_alimk_time;
	u32 tssi_slope_time;
	s32 slopek_cw_diff[MAX_HALRF_PATH];
};

struct halrf_xtal_info{
	s8  delta_swing_xtal_table_idx_p[DELTA_SWINGIDX_SIZE];
	s8  delta_swing_xtal_table_idx_n[DELTA_SWINGIDX_SIZE];
};

#endif
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

#ifndef _HAL_LD_FILE_H_
#define _HAL_LD_FILE_H_

#define PHYPG_BAND2G 0
#define PHYPG_BAND5G 1
#define PHYPG_RF1Tx 0
#define PHYPG_RF2Tx 1
#define PHYPG_OFFSET 15

enum PHY_PG_RATE_def {
	CCK_11M_1M = 0,
	OFDM_18M_6M = 1,
	OFDM_54M_24M = 2,
	HE1SS_MCS3_0 = 3,
	HE1SS_MCS7_4 = 4,
	HE1SS_MCS11_8 = 5,
	HE1SS_DCM4_0 = 6,
	HE2SS_MCS3_0 = 3,
	HE2SS_MCS7_4 = 4,
	HE2SS_MCS11_8 = 5,
	HE2SS_DCM4_0 = 6,
	Legacy_AllRate2_1 = 7,
	HE_AllRate2_2 = 8,
	OFDM_AllRate5_1 = 9,
	OFDM_AllRate6_1 = 10,
};

struct _hal_file_regd_ext {
		u16 domain;
		char country[2];
		char reg_name[10];
};

typedef struct hal_txpwr_lmt_t {
	u8			band;
	u8			bw;
	u8			ntx;
	u8			rs;
	u8			bf;
	u8			reg;
	u8			ch;
	s8			val;
	u8			tx_shap_idx;
} HAL_TXPWR_LMT_T , *PHAL_TXPWR_LMT_T;

typedef struct hal_txpwr_lmt_ru_t {
	u8			band;
	u8			rubw;
	u8			ntx;
	u8			rs;
	u8			reg;
	u8			ch;
	s8			val;
	u8			tx_shap_idx;
} Hal_Txpwr_lmt_Ru_t , *pHal_Txpwr_lmt_Ru_t;

enum _halrf_tx_pw_lmt_ru_bandwidth_type {
	_PW_LMT_RU_BW_RU26,
	_PW_LMT_RU_BW_RU52,
	_PW_LMT_RU_BW_RU106,
	_PW_LMT_RU_BW_NULL
};

enum _halrf_pw_lmt_band_type {
		_PW_LMT_BAND_2_4G = 0,
		_PW_LMT_BAND_5G = 1,
		_PW_LMT_BAND_6G = 2,
		_PW_LMT_MAX_BAND = 3
};

enum _halrf_pw_lmt_bandwidth_type {
		_PW_LMT_BW_20M = 0,
		_PW_LMT_BW_40M = 1,
		_PW_LMT_BW_80M = 2,
		_PW_LMT_BW_160M = 3,
		_PW_LMT_MAX_BANDWIDTH_NUM = 4
};

enum _halrf_pw_lmt_ratesection_type {
		_PW_LMT_RS_CCK = 0,
		_PW_LMT_RS_OFDM = 1,
		_PW_LMT_RS_HT = 2,
		_PW_LMT_RS_VHT = 3,
		_PW_LMT_RS_HE = 4,
		_PW_LMT_MAX_RS_NUM = 5
};

enum _halrf_pw_lmt_rfpath_type {
		_PW_LMT_PH_1T = 0,
		_PW_LMT_PH_2T = 1,
		_PW_LMT_PH_3T = 2,
		_PW_LMT_PH_4T = 3,
		_PW_LMT_MAX_PH_NUM = 4
};

enum _halrf_pw_lmt_beamforming_type {
		_PW_LMT_NONBF = 0,
		_PW_LMT_BF = 1,
		_PW_LMT_MAX_BF_NUM = 2
};

/*@--------------------------Define Parameters-------------------------------*/
#define AVG_THERMAL_NUM 8
#define MAX_RF_PATH 4
#define DELTA_SWINGIDX_SIZE 30
#define BAND_NUM 4
#define BAND_NUM_6G 4
#define DELTA_SWINTSSI_SIZE 61

/*@---------------------------End Define Parameters---------------------------*/

struct hal_txpwr_track_t {
	/* u8 is_txpowertracking; */
	u8 tx_powercount;
	bool is_txpowertracking_init;
	bool is_txpowertracking;
	u8 txpowertrack_control; /* for mp mode, turn off txpwrtracking as default */
	u8 tm_trigger;
	u8 internal_pa_5g[2];	/* pathA / pathB */

	u8 thermal_meter[2];	/* thermal_meter, index 0 for RFIC0, and 1 for RFIC1 */
	u8 thermal_value;
	u8 thermal_value_path[MAX_RF_PATH];
	u8 thermal_value_lck;
	u8 thermal_value_iqk;
	s8 thermal_value_delta; /* delta of thermal_value and efuse thermal */
	u8 thermal_value_avg[AVG_THERMAL_NUM];
	u8 thermal_value_avg_path[MAX_RF_PATH][AVG_THERMAL_NUM];
	u8 thermal_value_avg_index;
	u8 thermal_value_avg_index_path[MAX_RF_PATH];
	s8 power_index_offset_path[MAX_RF_PATH];

	u8 thermal_value_rx_gain;
	u8 thermal_value_crystal;
	u8 thermal_value_dpk_store;
	u8 thermal_value_dpk_track;
	bool txpowertracking_in_progress;

	bool is_reloadtxpowerindex;
	u8 is_rf_pi_enable;
	u32	txpowertracking_callback_cnt; /* cosa add for debug */

	u8 is_cck_in_ch14;
	u8 CCK_index;
	u8 OFDM_index[MAX_RF_PATH];
	s8 power_index_offset;
	s8 delta_power_index;
	s8 delta_power_index_path[MAX_RF_PATH];
	s8 delta_power_index_last;
	s8 delta_power_index_last_path[MAX_RF_PATH];
	bool is_tx_power_changed;

	/*struct iqk_matrix_regs_setting iqk_matrix_reg_setting[IQK_MATRIX_SETTINGS_NUM];*/
	u8 delta_lck;
	s8 delta_swing_table_idx_2g_cck_a_p[DELTA_SWINGIDX_SIZE];
	s8 delta_swing_table_idx_2g_cck_a_n[DELTA_SWINGIDX_SIZE];
	s8 delta_swing_table_idx_2g_cck_b_p[DELTA_SWINGIDX_SIZE];
	s8 delta_swing_table_idx_2g_cck_b_n[DELTA_SWINGIDX_SIZE];
	s8 delta_swing_table_idx_2g_cck_c_p[DELTA_SWINGIDX_SIZE];
	s8 delta_swing_table_idx_2g_cck_c_n[DELTA_SWINGIDX_SIZE];
	s8 delta_swing_table_idx_2g_cck_d_p[DELTA_SWINGIDX_SIZE];
	s8 delta_swing_table_idx_2g_cck_d_n[DELTA_SWINGIDX_SIZE];
	s8 delta_swing_table_idx_2ga_p[DELTA_SWINGIDX_SIZE];
	s8 delta_swing_table_idx_2ga_n[DELTA_SWINGIDX_SIZE];
	s8 delta_swing_table_idx_2gb_p[DELTA_SWINGIDX_SIZE];
	s8 delta_swing_table_idx_2gb_n[DELTA_SWINGIDX_SIZE];
	s8 delta_swing_table_idx_2gc_p[DELTA_SWINGIDX_SIZE];
	s8 delta_swing_table_idx_2gc_n[DELTA_SWINGIDX_SIZE];
	s8 delta_swing_table_idx_2gd_p[DELTA_SWINGIDX_SIZE];
	s8 delta_swing_table_idx_2gd_n[DELTA_SWINGIDX_SIZE];
	s8 delta_swing_table_idx_5ga_p[BAND_NUM][DELTA_SWINGIDX_SIZE];
	s8 delta_swing_table_idx_5ga_n[BAND_NUM][DELTA_SWINGIDX_SIZE];
	s8 delta_swing_table_idx_5gb_p[BAND_NUM][DELTA_SWINGIDX_SIZE];
	s8 delta_swing_table_idx_5gb_n[BAND_NUM][DELTA_SWINGIDX_SIZE];
	s8 delta_swing_table_idx_5gc_p[BAND_NUM][DELTA_SWINGIDX_SIZE];
	s8 delta_swing_table_idx_5gc_n[BAND_NUM][DELTA_SWINGIDX_SIZE];
	s8 delta_swing_table_idx_5gd_p[BAND_NUM][DELTA_SWINGIDX_SIZE];
	s8 delta_swing_table_idx_5gd_n[BAND_NUM][DELTA_SWINGIDX_SIZE];
	s8 delta_swing_table_idx_6ga_p[BAND_NUM_6G][DELTA_SWINGIDX_SIZE];
	s8 delta_swing_table_idx_6ga_n[BAND_NUM_6G][DELTA_SWINGIDX_SIZE];
	s8 delta_swing_table_idx_6gb_p[BAND_NUM_6G][DELTA_SWINGIDX_SIZE];
	s8 delta_swing_table_idx_6gb_n[BAND_NUM_6G][DELTA_SWINGIDX_SIZE];
	s8 delta_swing_table_idx_6gc_p[BAND_NUM_6G][DELTA_SWINGIDX_SIZE];
	s8 delta_swing_table_idx_6gc_n[BAND_NUM_6G][DELTA_SWINGIDX_SIZE];
	s8 delta_swing_table_idx_6gd_p[BAND_NUM_6G][DELTA_SWINGIDX_SIZE];
	s8 delta_swing_table_idx_6gd_n[BAND_NUM_6G][DELTA_SWINGIDX_SIZE];
	s8 delta_swing_tssi_table_2g_cck_a[DELTA_SWINTSSI_SIZE];
	s8 delta_swing_tssi_table_2g_cck_b[DELTA_SWINTSSI_SIZE];
	s8 delta_swing_tssi_table_2g_cck_c[DELTA_SWINTSSI_SIZE];
	s8 delta_swing_tssi_table_2g_cck_d[DELTA_SWINTSSI_SIZE];
	s8 delta_swing_tssi_table_2ga[DELTA_SWINTSSI_SIZE];
	s8 delta_swing_tssi_table_2gb[DELTA_SWINTSSI_SIZE];
	s8 delta_swing_tssi_table_2gc[DELTA_SWINTSSI_SIZE];
	s8 delta_swing_tssi_table_2gd[DELTA_SWINTSSI_SIZE];
	s8 delta_swing_tssi_table_5ga[BAND_NUM][DELTA_SWINTSSI_SIZE];
	s8 delta_swing_tssi_table_5gb[BAND_NUM][DELTA_SWINTSSI_SIZE];
	s8 delta_swing_tssi_table_5gc[BAND_NUM][DELTA_SWINTSSI_SIZE];
	s8 delta_swing_tssi_table_5gd[BAND_NUM][DELTA_SWINTSSI_SIZE];
	s8 delta_swing_table_xtal_p[DELTA_SWINGIDX_SIZE];
	s8 delta_swing_table_xtal_n[DELTA_SWINGIDX_SIZE];
	s8 delta_swing_table_idx_2ga_p_8188e[DELTA_SWINGIDX_SIZE];
	s8 delta_swing_table_idx_2ga_n_8188e[DELTA_SWINGIDX_SIZE];

	u8 bb_swing_idx_ofdm[MAX_RF_PATH];
	u8 bb_swing_idx_ofdm_current;
	u8 bb_swing_idx_ofdm_base;
	u8 bb_swing_idx_ofdm_base_path[MAX_RF_PATH];
	bool bb_swing_flag_ofdm;
	u8 bb_swing_idx_cck;
	u8 bb_swing_idx_cck_current;
	u8 bb_swing_idx_cck_base;
	u8 default_ofdm_index;
	u8 default_cck_index;
	bool bb_swing_flag_cck;

	s8 absolute_ofdm_swing_idx[MAX_RF_PATH];
	s8 remnant_ofdm_swing_idx[MAX_RF_PATH];
	s8 absolute_cck_swing_idx[MAX_RF_PATH];
	s8 remnant_cck_swing_idx;
	s8 modify_tx_agc_value;	/*Remnat compensate value at tx_agc */
	bool modify_tx_agc_flag_path_a;
	bool modify_tx_agc_flag_path_b;
	bool modify_tx_agc_flag_path_c;
	bool modify_tx_agc_flag_path_d;
	bool modify_tx_agc_flag_path_a_cck;
	bool modify_tx_agc_flag_path_b_cck;

	s8 kfree_offset[MAX_RF_PATH];
	/*Add by Yuchen for Kfree Phydm*/
	u8 reg_rf_kfree_enable;	/*for registry*/
	u8 rf_kfree_enable;		/*for efuse enable check*/
	};

int rtw_hal_find_ext_regd_num(struct rtw_para_pwrlmt_info_t *para_info, const char *regd_name);
void rtw_hal_dl_all_para_file(struct rtw_phl_com_t *phl_com, char *ic_name, void *hal);
u8 rtw_hal_efuse_shadow_file_load(void *hal, char *ic_name, bool is_limit);
u8 rtw_hal_ld_fw_symbol(struct rtw_phl_com_t *phl_com,
	struct rtw_hal_com_t *hal_com, const char *name, u8 **buf, u32 *buf_size);
#endif /*_HAL_LD_FILE_H_*/
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
#ifndef __HALBB_PHYSTS_H__
#define __HALBB_PHYSTS_H__

#define VAR_LENGTH 0xff
#define TRANS_2_RSSI(X) (X >> 1)
#define PHYSTS_HDR_LEN	8
#define TB_USER_MAX	4
#define MU_USER_MAX	4
#define PSTS_USR(x,y)	psts_##x_usr_##y

#define IE11_PKT_INFO_LEN 10

enum bb_physts_bw_info{
	PSTS_BW5 	= 0,
	PSTS_BW10 	= 1,
	PSTS_BW20 	= 2,
	PSTS_BW40 	= 3,
	PSTS_BW80 	= 4,
	PSTS_BW160 	= 5,
	PSTS_BW80_80	= 6,
	PSTS_BW_MAX
};

enum bb_physts_ie_t {
	IE00_CMN_CCK			= 0,
	IE01_CMN_OFDM			= 1,
	IE02_CMN_EXT_AX			= 2,
	IE03_CMN_EXT_SEG_1		= 3,
	IE04_CMN_EXT_PATH_A		= 4,
	IE05_CMN_EXT_PATH_B		= 5,
	IE06_CMN_EXT_PATH_C		= 6,
	IE07_CMN_EXT_PATH_D		= 7,
	IE08_FTR_CH			= 8,
	IE09_FTR_PLCP_0			= 9,
	IE10_FTR_PLCP_EXT		= 10,
	IE11_FTR_PLCP_HISTOGRAM		= 11,
	IE12_MU_EIGEN_INFO		= 12,
	IE13_DL_MU_DEF			= 13,
	IE14_TB_UL_CQI			= 14,
	IE15_TB_UL_DEF			= 15,
	IE16_RSVD16			= 16,
	IE17_TB_UL_CTRL			= 17,
	IE18_DBG_OFDM_FD_CMN		= 18,
	IE19_DBG_OFDM_TD_CMN		= 19,
	IE20_DBG_OFDM_FD_USER_SEG_0	= 20,
	IE21_DBG_OFDM_FD_USER_SEG_1	= 21,
	IE22_DBG_OFDM_FD_USER_AGC	= 22,
	IE23_RSVD23			= 23,
	IE24_DBG_OFDM_TD_PATH_A		= 24,
	IE25_DBG_OFDM_TD_PATH_B		= 25,
	IE26_DBG_OFDM_TD_PATH_C		= 26,
	IE27_DBG_OFDM_TD_PATH_D		= 27,
	IE28_DBG_CCK_PATH_A		= 28,
	IE29_DBG_CCK_PATH_B		= 29,
	IE30_DBG_CCK_PATH_C		= 30,
	IE31_DBG_CCK_PATH_D		= 31,
	IE_PHYSTS_LEN_ALL		= 32
};

struct bb_info;

static const char bb_physts_bitmap_type_t[][9] = {
	"SRH_FAIL",
	"BRK_BY_TX",
	"CCA_SPF",
	"OFDM_BRK",
	"CCK_BRK",
	"DLMU_SPF",
	"HE_MU",
	"VHT_MU",
	"TB_SPF",
	"N/A",
	"TB",
	"CCK",
	"LEGACY",
	"HT",
	"VHT",
	"HE",
	"EHT",
};

struct bb_physts_rslt_hdr_info {
	u8 ppdu_idx;
	u8 rssi[4];
	u8 rssi_td[4];
	u8 rssi_avg;
	enum bb_physts_bitmap_t ie_map_type;
};

struct bb_physts_rslt_0_info {
	u8 *ie_00_addr;
	u8 rx_path_en_cck;
	s16 cfo_avg_cck; /*S(12,2), -512~+511.75 kHz*/
	u8 evm_avg_cck;
	u8 avg_idle_noise_pwr_cck; /*u(8,1)*/
	u8 pop_idx_cck;
	u16 rpl; /*U(9,2)*/
};

struct bb_physts_rslt_1_info {
	u8 *ie_01_addr;
	u8 rx_path_en;
	s16 cfo_avg; /*S(12,2), -512~+511.75 kHz, FD tracking CFO*/
	s16 cfo_pab_avg; /*S(12,2), -512~+511.75 kHz, preamble CFO*/
	u8 evm_max;
	u8 evm_min;
	u8 snr_avg;
	u8 cn_avg;
	u8 avg_idle_noise_pwr; /*u(8,1)*/
	u8 pop_idx;
	u8 rxsc; /*for AX IC*/
	//u8 rxsb; /*for BE IC*/
	u8 ch_idx;
	enum band_type band;
	u8 rpl_fd; /*u(8,1)*/
	enum channel_width bw_idx;
	bool is_su; /*if (not MU && not OFDMA), is_su = 1*/
	bool is_ldpc;
	bool is_ndp;
	bool is_stbc;
	bool grant_bt;
	bool is_awgn;
	bool is_bf;
};

struct bb_physts_rslt_2_info {
	u8 *ie_02_addr;
	u8 max_nsts;
	u8 ltf_type;
	u8 gi;
	s32 c_cfo_i;
	u8 rx_info_1;
	u8 rx_state_feq;
	s32 c_cfo_q;
	u8 est_cmped_phase;
	u8 pkt_extension;
	s32 f_cfo_i;
	u8 n_ltf;
	u16 n_sym;
	s32 f_cfo_q;
	bool midamble;
	bool is_mu_mimo;
	bool is_dl_ofdma;
	bool is_dcm;
	bool is_doppler;
};

struct bb_physts_rslt_3_info {
	u8 *ie_03_addr;
	u8 avg_cn_seg1;
	u8 sigval_below_th_tone_cnt_seg1;
	u8 cn_excess_th_tone_cnt_seg1;
	u16 avg_cfo_seg1;
	u16 avg_cfo_premb_seg1;
	u8 est_cmped_phase_seg1;
	u8 avg_snr_seg1;
	u32 c_cfo_i_seg1;
	u32 c_cfo_q_seg1;
	u32 f_cfo_i_seg1;
	u32 f_cfo_q_seg1;
	u8 ch_idx_seg1;
	u8 evm_max_seg1;
	u8 evm_min_seg1;
};

struct bb_physts_rslt_4_to_7_info {
	u8 *ie_04_07_addr;
	u8 ant_idx;
	s8 sig_val_y;
	u8 rf_gain_idx;
	u8 snr_lgy;
	u8 evm_ss_y;
	u8 td_ant_weight;
	s8 dc_est_re;
	s8 dc_est_im;
	bool rf_tia_gain_idx;
	bool tia_shrink_indicator;
	//bool ant_idx_0_msb;
};

struct bb_physts_rslt_8_info {
	u8 *ie_08_addr;
	u8 rxsc;
	u8 n_rx;
	u8 n_sts;
	u16 ch_info_len;
	u8 evm_1_sts;
	u8 evm_2_sts;
	u8 avg_idle_noise_pwr;
	bool is_ch_info_len_valid;
	s16 *ch_info_addr;
};

struct bb_physts_rslt_9_info {
	u8 *ie_09_addr;
	u32 l_sig;
	u32 sig_a1;
	u32 sig_a2;
};

struct bb_physts_rslt_10_info {
	u8 *ie_10_addr;
	u8 *sigb_raw_data_bits_addr;
	u16 sigb_len;
};

struct bb_physts_rslt_11_info {
	u8 *ie_11_addr;
	u32 l_sig;
	u32 sig_a1;
	u16 sig_a2;
	u16 time_stamp;
	u8 rx_pkt_info_idx;
	u8 tx_pkt_info_idx;
};

struct bb_physts_user_info_ie_12{
	u8 sig_val_ss0_seg_cr_user_i;
	u8 sig_val_ss1_seg_cr_user_i;
	u8 sig_val_ss2_seg_cr_user_i;
	u8 sig_val_ss3_seg_cr_user_i;
	u8 sig_bad_tone_cnt_seg_cr_user_i;
	u8 cn_bad_tone_cnt_seg_cr_user_i;
};

struct bb_physts_rslt_12_info {
	u8 *ie_12_addr;
	u8 n_user;
	struct bb_physts_user_info_ie_12 bb_physts_uer_info[MU_USER_MAX];
};

struct bb_physts_user_info_ie_13{
	u8 is_bf;
	u8 fec_type;
	u8 mcs;
	u8 pilot_exist;
	u8 n_sts;
	u8 n_sts_ru_total;
	u8 is_awgn;
	u8 is_mu_mimo;
	u8 pdp_he_ltf_and_gi_type;
	u8 start_sts;
	u8 rx_evm_max_seg_cr;
	u8 rx_evm_min_seg_cr;
	s8 snr;
	u8 ru_alloc;
	u8 is_dcm;
	u8 avg_cn_seg_cr;
	u16 sta_id;
};

struct bb_physts_rslt_13_info {
	u8 *ie_13_addr;
	u8 n_user;
	struct bb_physts_user_info_ie_13 bb_physts_uer_info[MU_USER_MAX];
	u8 n_not_sup_sta;
};

struct bb_physts_user_info_ie_14{
	u8 cqi_bitmap_ul_tb;
	u8 cqi_raw_len_ul_tb;
	u8 *cqi_raw_ul_tb_addr;
};

struct bb_physts_rslt_14_info {
	u8 *ie_14_addr;
	u8 rxinfo_ndp_en;
	u8 n_user;
	struct bb_physts_user_info_ie_14 bb_physts_uer_info[MU_USER_MAX];
	
};

struct bb_physts_user_info_ie_15{
	/* 64bit cmn_info */
	u8 mcs;
	u8 fec_type;
	u8 is_bf;
	
	u8 n_sts_ru_total;
	u8 n_sts;
	u8 pilot_exist;

	u8 start_sts;
	u8 pdp_he_ltf_and_gi_type;
	u8 is_mu_mimo;
	u8 is_awgn;

	u8 rx_evm_max_seg_cr;
	u8 rx_evm_min_seg_cr;
	s8 snr;
	u8 ru_alloc;

	u8 avg_cn_seg_cr;
	u8 is_dcm;

	/* others */
	u8 uid;
	s16 avg_cfo_seg0;
	u16 rssi_m_ul_tb;
};

struct bb_physts_rslt_15_info {
	u8 *ie_15_addr;
	u8 n_user;
	struct bb_physts_user_info_ie_15 bb_physts_uer_info[MU_USER_MAX];
	
};

struct bb_physts_rslt_16_info {
	u8 *ie_16_addr;
	u8 tmp;
};

struct bb_physts_cmn_info_ie_17 {
	bool stbc_en;
	bool ldpc_extra;
	bool doppler_en;
	bool midamle_mode;
	u8 gi_type;
	u8 ltf_type;
	u8 n_ltf;
	u8 n_sym;
	u8 pe_idx;
	u8 pre_fec_factor;
	u8 n_usr;
	bool mumimo_ltf_mode_en;
	bool ndp;
	u8 pri_exp_rssi_dbm;
	u8 dbw_idx;
	
	u8 rsvd;
		
	u16 rxtime;
};

struct bb_physts_user_info_ie_17 {
	u8 u_id;	
	u8 ru_alloc;
	u8 n_sts_ru_tot;

	u8 rsvd1;

	u8 strt_sts;
	u8 n_sts;
	bool fec_type;
	u8 mcs;

	u8 rsvd2;

	bool dcm_en;
	
	u8 rsvd3;
};

struct bb_physts_rslt_17_info {
	u8 *ie_17_addr;
	u8 n_user;
	struct bb_physts_cmn_info_ie_17 bb_physts_cmn_info;
	struct bb_physts_user_info_ie_17 bb_physts_uer_info[MU_USER_MAX];
};

struct bb_physts_rslt_18_info {
	u8 *ie_18_addr;
	u16 rx_time;
	u8 ch_len_lgcy_seg0;
	u8 bw_det_seg0;
	u8 snr_idx_lgcy_seg0;
	u8 pdp_idx_lgcy_seg0;
	u16 pfd_flow;
	u8 ch_len_lgcy_seg1;
	u8 bw_det_seg1;
	u8 snr_idx_lgcy_seg1;
	u8 pdp_idx_lgcy_seg1;
	bool is_seg1_exist;
};

struct bb_physts_rslt_19_info {
	u8 *ie_19_addr;
	u8 ppdu_inpwrdbm_p20;
	u8 ppdu_inpwrdbm_s20;
	u8 ppdu_inpwrdbm_s40;
	u8 ppdu_inpwrdbm_s80;
	u8 ppdu_inpwrdbm_per20_1;
	u8 ppdu_inpwrdbm_per20_2;
	u8 ppdu_inpwrdbm_per20_3;
	u8 ppdu_inpwrdbm_per20_4;
	u8 edcca_rpt_cnt_p20;
	u8 edcca_rpt_p20_max;
	u8 edcca_rpt_p20_min;
	u8 edcca_total_smp_cnt;
	u8 edcca_rpt_cnt_s80;
	u8 edcca_rpt_cnt_s80_max;
	u8 edcca_rpt_cnt_s80_min;
	u8 pop_reg_pwr;
	u8 pop_trig_pwr;
	u8 early_drop_pwr;
	bool tx_over_flow;
};

struct bb_physts_user_info_20 {
	u8 ch_smo_n_block_lgcy_sub_0_seg_0;
	u8 ch_smo_n_block_lgcy_sub_1_seg_0;
	u8 ch_smo_n_block_lgcy_sub_2_seg_0;
	u8 ch_smo_n_block_lgcy_sub_3_seg_0;

	u8 ch_smo_en_lgcy_seg_0;
	u8 ch_smo_en_non_lgcy_seg_0;

	u8 ch_smo_n_block_non_lgcy_seg_0;

	u8 ch_len_non_lgcy_sts_0_seg_0;
	u8 ch_len_non_lgcy_sts_1_seg_0;
	u8 ch_len_non_lgcy_sts_2_seg_0;
	u8 ch_len_non_lgcy_sts_3_seg_0;

	u8 snr_idx_non_lgy_sts_0_seg_0;
	u8 snr_idx_non_lgy_sts_1_seg_0;
	u8 snr_idx_non_lgy_sts_2_seg_0;
	u8 snr_idx_non_lgy_sts_3_seg_0;
	u8 pdp_idx_non_lgcy_sts_0_seg_0;
	u8 pdp_idx_non_lgcy_sts_1_seg_0;
	u8 pdp_idx_non_lgcy_sts_2_seg_0;
	u8 pdp_idx_non_lgcy_sts_3_seg_0;

	u8 snr_non_lgy_sts_0_seg_0;
	u8 snr_non_lgy_sts_1_seg_0;
	u8 snr_non_lgy_sts_2_seg_0;
	u8 snr_non_lgy_sts_3_seg_0;
	u8 evm_ss_0_seg_0;
	u8 evm_ss_1_seg_0;
	u8 evm_ss_2_seg_0;
	u8 evm_ss_3_seg_0;
};


struct bb_physts_rslt_20_info {
	u8 *ie_20_addr;
	u8 n_user;
	struct bb_physts_user_info_20 bb_physts_usr_info[MU_USER_MAX];
};

struct bb_physts_user_info_21 {
	u8 ch_smo_n_block_lgcy_sub_0_seg_1;
	u8 ch_smo_n_block_lgcy_sub_1_seg_1;
	u8 ch_smo_n_block_lgcy_sub_2_seg_1;
	u8 ch_smo_n_block_lgcy_sub_3_seg_1;

	u8 ch_smo_en_lgcy_seg_1;
	u8 ch_smo_en_non_lgcy_seg_1;

	u8 ch_smo_n_block_non_lgcy_seg_1;

	u8 ch_len_non_lgcy_sts_0_seg_1;
	u8 ch_len_non_lgcy_sts_1_seg_1;
	u8 ch_len_non_lgcy_sts_2_seg_1;
	u8 ch_len_non_lgcy_sts_3_seg_1;

	u8 snr_idx_non_lgy_sts_0_seg_1;
	u8 snr_idx_non_lgy_sts_1_seg_1;
	u8 snr_idx_non_lgy_sts_2_seg_1;
	u8 snr_idx_non_lgy_sts_3_seg_1;
	u8 pdp_idx_non_lgcy_sts_0_seg_1;
	u8 pdp_idx_non_lgcy_sts_1_seg_1;
	u8 pdp_idx_non_lgcy_sts_2_seg_1;
	u8 pdp_idx_non_lgcy_sts_3_seg_1;

	u8 snr_non_lgy_sts_0_seg_1;
	u8 snr_non_lgy_sts_1_seg_1;
	u8 snr_non_lgy_sts_2_seg_1;
	u8 snr_non_lgy_sts_3_seg_1;
	u8 evm_ss_0_seg_1;
	u8 evm_ss_1_seg_1;
	u8 evm_ss_2_seg_1;
	u8 evm_ss_3_seg_1;
};

struct bb_physts_rslt_21_info {
	u8 *ie_21_addr;
	u8 n_user;
	struct bb_physts_user_info_21 bb_physts_usr_info[MU_USER_MAX];
};

struct bb_physts_rslt_22_info {
	u8 *ie_22_addr;
	u8 tmp;
};

struct bb_physts_rslt_23_info {
	u8 *ie_23_addr;
	u8 tmp;
};

struct bb_physts_rslt_24_info {
	u8 *ie_24_addr;
	u8 pre_agc_step_a;
	u8 l_fine_agc_step_a;
	u8 ht_fine_agc_step_a;
	u8 pre_gain_code_a;
	u8 l_fine_gain_code_a;
	u8 ht_fine_gain_code_a;
	u8 l_dagc_a;
	u8 ht_dagc_a;
	u8 pre_ibpwrdbm_a;
	u8 pre_wbpwrdbm_a;
	u8 l_ibpwrdbm_a;
	u8 l_wbpwrdbm_a;
	u8 ht_ibpwrdbm_a;
	u8 ht_wbpwrdbm_a;
	u8 l_dig_ibpwrdbm_a;
	u8 ht_dig_ibpwrdbm_a;
	u8 lna_inpwrdbm_a;
	u8 aci2sig_db;
	u8 sb5m_ratio_0;
	u8 sb5m_ratio_1;
	u8 sb5m_ratio_2;
	u8 sb5m_ratio_3;
	bool aci_indicator_a;
	bool tia_shrink_indicator_a;
	bool pre_gain_code_tia_a;
	bool l_fine_gain_code_tia_a;
	bool ht_fine_gain_code_tia_a;
	bool aci_det;
	u8 physts_aci_idx;

};

struct bb_physts_rslt_25_info {
	u8 *ie_25_addr;
	u8 pre_agc_step_b;
	u8 l_fine_agc_step_b;
	u8 ht_fine_agc_step_b;
	u8 pre_gain_code_b;
	u8 l_fine_gain_code_b;
	u8 ht_fine_gain_code_b;
	u8 l_dagc_b;
	u8 ht_dagc_b;
	u8 pre_ibpwrdbm_b;
	u8 pre_wbpwrdbm_b;
	u8 l_ibpwrdbm_b;
	u8 l_wbpwrdbm_b;
	u8 ht_ibpwrdbm_b;
	u8 ht_wbpwrdbm_b;
	u8 l_dig_ibpwrdbm_b;
	u8 ht_dig_ibpwrdbm_b;
	u8 lna_inpwrdbm_b;
	u8 aci2sig_db;
	u8 sb5m_ratio_0;
	u8 sb5m_ratio_1;
	u8 sb5m_ratio_2;
	u8 sb5m_ratio_3;
	bool aci_indicator_b;
	bool tia_shrink_indicator_b;
	bool pre_gain_code_tia_b;
	bool l_fine_gain_code_tia_b;
	bool ht_fine_gain_code_tia_b;
	bool aci_det;
};

struct bb_physts_rslt_26_info {
	u8 *ie_26_addr;
	u8 tmp;
};

struct bb_physts_rslt_27_info {
	u8 *ie_27_addr;
	u8 tmp;
};

struct bb_physts_rslt_28_info {
	u8 *ie_28_addr;
	u16 ant_weight_a;
	u8 h3_real_a;
	u8 h3_imag_a;
	u8 h4_real_a;
	u8 h4_imag_a;
	u8 h5_real_a;
	u8 h5_imag_a;
	u8 h6_real_a;
	u8 h6_imag_a;
	u8 h7_real_a;
	u8 h7_imag_a;
	u8 h8_real_a;
	u8 h8_imag_a;
	u8 h9_real_a;
	u8 h9_imag_a;
	u8 h10_real_a;
	u8 h10_imag_a;
	u8 h11_real_a;
	u8 h11_imag_a;
	u8 h12_real_a;
	u8 h12_imag_a;
	u8 h13_real_a;
	u8 h13_imag_a;
	u8 h14_real_a;
	u8 h14_imag_a;
	u8 h15_real_a;
	u8 h15_imag_a;
	u8 h16_real_a;
	u8 h16_imag_a;
	u8 h17_real_a;
	u8 h17_imag_a;
};

struct bb_physts_rslt_29_info {
	u8 *ie_29_addr;
	u16 ant_weight_b;
	u8 h3_real_b;
	u8 h3_imag_b;
	u8 h4_real_b;
	u8 h4_imag_b;
	u8 h5_real_b;
	u8 h5_imag_b;
	u8 h6_real_b;
	u8 h6_imag_b;
	u8 h7_real_b;
	u8 h7_imag_b;
	u8 h8_real_b;
	u8 h8_imag_b;
	u8 h9_real_b;
	u8 h9_imag_b;
	u8 h10_real_b;
	u8 h10_imag_b;
	u8 h11_real_b;
	u8 h11_imag_b;
	u8 h12_real_b;
	u8 h12_imag_b;
	u8 h13_real_b;
	u8 h13_imag_b;
	u8 h14_real_b;
	u8 h14_imag_b;
	u8 h15_real_b;
	u8 h15_imag_b;
	u8 h16_real_b;
	u8 h16_imag_b;
	u8 h17_real_b;
	u8 h17_imag_b;
};

struct bb_physts_rslt_30_info {
	u8 *ie_30_addr;
	u8 tmp;
};

struct bb_physts_rslt_31_info {
	u8 *ie_31_addr;
	u8 tmp;
};

struct bb_physts_cnt_info {
	u16 all_cnt;
	u16 is_2_self_cnt;
	u16 err_ie_cnt;
	u16 ok_ie_cnt;
	u16 err_len_cnt;
	bool invalid_he_occur;
	u32 invalid_he_cnt;
	u32 cck_brk_cnt;
	u32 ie_cnt[PHYSTS_BITMAP_NUM];
};

struct bb_physts_cr_info {
	u32 bitmap_search_fail;
	//u32 bitmap_search_fail_m;
	u32 bitmap_eht;
	u32 plcp_hist;
	u32 plcp_hist_m;
	u32 period_cnt_en;
};

struct bb_physts_info {
	bool init_physts_cr_success;
	u32 physts_bitmap_recv;
	u32 bitmap_type[PHYSTS_BITMAP_NUM];
	u8 rx_path_en;
	bool print_more_info;
	u8 physts_dump_mode; /*0: disable, 1:raw data, 2: msg mode, 3:raw data + msg mode*/
	u16 physts_dump_idx;
	bool is_valid; // used for UI parsing
	bool show_phy_sts_all_pkt;
	bool dfs_phy_sts_privilege;// used for CAC period in DFS channel
	u16 show_phy_sts_cnt;
	u16 show_phy_sts_max_cnt;
	// long term cfo rslt
	s32 l_ltf_cfo_i;
	s32 l_ltf_cfo_q;
	u16 ie_len_curr[IE_PHYSTS_LEN_ALL];
	u16 physts_rpt_len_byte[PHYSTS_BITMAP_NUM]; /*valid physts rpt len report by drv*/
	bool rssi_cvrt_2_rpl_en;
	u8 rpl_path[4]; /*u(8,1)*/
	u8 rpl_avg; /*u(8,1)*/
	u8 frc_mu; /*force data type to SU/MU(debug mode)*/
	u8 tmp_mcs;/*fake MCS (debug mode)*/
	u8 tmp_sts;/*fake STS (debug mode)*/
	struct bb_rate_info		bb_rate_i;
	struct bb_rate_info		bb_rate_mu_i;
	struct bb_physts_cr_info	bb_physts_cr_i;
	struct bb_physts_cnt_info	bb_physts_cnt_i;
	struct bb_physts_rslt_hdr_info	bb_physts_rslt_hdr_i;
	struct bb_physts_rslt_0_info	bb_physts_rslt_0_i;
	struct bb_physts_rslt_1_info	bb_physts_rslt_1_i;
	struct bb_physts_rslt_2_info	bb_physts_rslt_2_i;
	struct bb_physts_rslt_3_info	bb_physts_rslt_3_i;
	struct bb_physts_rslt_4_to_7_info bb_physts_rslt_4_i;
	struct bb_physts_rslt_4_to_7_info bb_physts_rslt_5_i;
	struct bb_physts_rslt_4_to_7_info bb_physts_rslt_6_i;
	struct bb_physts_rslt_4_to_7_info bb_physts_rslt_7_i;
	struct bb_physts_rslt_8_info	bb_physts_rslt_8_i;
	struct bb_physts_rslt_9_info	bb_physts_rslt_9_i;
	struct bb_physts_rslt_10_info	bb_physts_rslt_10_i;
	struct bb_physts_rslt_11_info	bb_physts_rslt_11_i;
	struct bb_physts_rslt_12_info	bb_physts_rslt_12_i;
	struct bb_physts_rslt_13_info	bb_physts_rslt_13_i;
	struct bb_physts_rslt_14_info	bb_physts_rslt_14_i;
	struct bb_physts_rslt_15_info	bb_physts_rslt_15_i;
	struct bb_physts_rslt_16_info	bb_physts_rslt_16_i;
	struct bb_physts_rslt_17_info	bb_physts_rslt_17_i;
	struct bb_physts_rslt_18_info	bb_physts_rslt_18_i;
	struct bb_physts_rslt_19_info	bb_physts_rslt_19_i;
	struct bb_physts_rslt_20_info	bb_physts_rslt_20_i;
	struct bb_physts_rslt_21_info	bb_physts_rslt_21_i;
	struct bb_physts_rslt_22_info	bb_physts_rslt_22_i;
	struct bb_physts_rslt_23_info	bb_physts_rslt_23_i;
	struct bb_physts_rslt_24_info	bb_physts_rslt_24_i;
	struct bb_physts_rslt_25_info	bb_physts_rslt_25_i;
	struct bb_physts_rslt_26_info	bb_physts_rslt_26_i;
	struct bb_physts_rslt_27_info	bb_physts_rslt_27_i;
	struct bb_physts_rslt_28_info	bb_physts_rslt_28_i;
	struct bb_physts_rslt_29_info	bb_physts_rslt_29_i;
	struct bb_physts_rslt_30_info	bb_physts_rslt_30_i;
	struct bb_physts_rslt_31_info	bb_physts_rslt_31_i;
};

struct bb_info;
/*@--------------------------[Prptotype]-------------------------------------*/
void halbb_mod_rssi_by_path_en(struct bb_info *bb, u8 rx_path_en);
void halbb_ch_idx_decode(struct bb_info *bb, u8 ch_idx_encoded,
			 u8 *ch_idx, enum band_type *band);
u8 halbb_physts_fd_snr_cvrt(struct bb_info *bb, u8 path);
void halbb_physts_fd_rpl_2_rssi_cvrt(struct bb_info *bb);
void halbb_ch_idx_encode(struct bb_info *bb, u8 ch_idx, enum band_type band,
			 u8 *ch_idx_encoded);
void halbb_physts_cvrt_2_mp(struct bb_info *bb);
void halbb_physts_rpt_gen(struct bb_info *bb, u32 physts_bitmap,
			  struct physts_result *rpt,
			  bool physts_rpt_valid, struct physts_rxd *desc,
			  bool is_cck_rate, bool is_ie8_valid);
void halbb_physts_ie_bitmap_set(struct bb_info *bb, u32 ie_page, u32 bitmap);
u32 halbb_physts_ie_bitmap_get(struct bb_info *bb, u32 ie_page);
void halbb_physts_ie_bitmap_en(struct bb_info *bb, enum bb_physts_bitmap_t type,
			       enum bb_physts_ie_t ie, bool en);
void halbb_phy_sts_manual_trig(struct bb_info *bb, enum bb_mode_type mode, u8 ss);
void halbb_physts_watchdog(struct bb_info *bb);
void halbb_physts_parsing_init_io_en(struct bb_info *bb);
void halbb_physts_parsing_init(struct bb_info *bb);

void halbb_physts_brk_fail_rpt_en(struct bb_info* bb, bool enable, enum phl_phy_idx phy_idx);

void halbb_physts_dbg(struct bb_info *bb, char input[][16], u32 *_used,
		  char *output, u32 *_out_len);
void halbb_cr_cfg_physts_init(struct bb_info *bb);

#endif

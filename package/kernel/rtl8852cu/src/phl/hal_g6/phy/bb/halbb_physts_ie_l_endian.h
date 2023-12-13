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
#ifndef __HALBB_PHYSTS_IE_L_ENDIAN_H__
#define __HALBB_PHYSTS_IE_L_ENDIAN_H__

 /*@--------------------------[Define] ---------------------------------------*/

 /*@--------------------------[Enum]------------------------------------------*/
 
 /*@--------------------------[Structure]-------------------------------------*/

struct physts_hdr_info {
	u8 ie_bitmap_select: 5;
	u8 rsvd_0:1;
	u8 null_tb_ppdu:1;
	u8 is_valid:1;		/*valid: total content length <= 1024 bytes*/
	u8 physts_total_length;	/*total length(unit: 8byte)*/
	u8 ppdu_idx;
	u8 rssi_avg_td;		/*U(8,1) RSSI=dBm+110. ex:-30dBm->RSSI:80%*/
	u8 rssi_td[4];
};

enum bb_physts_hdr_t {
	HDR_TYPE1 = 1, /*5bit fix length*/
	HDR_TYPE2 = 2 /*12bit variable length*/
};

struct physts_ie_0_info {
	/*[DW0]*/
	u8 ie_hdr:5;
	u8 pop_idx:2;
	u8 rpl_l:1;

	u8 rpl_m;
	u8 cca_time;
	u8 antwgt_gain_diff:5;
	u8 cck_hw_antsw_occur_a:1;
	u8 cck_hw_antsw_occur_b:1;
	u8 cck_hw_antsw_occur_c:1;
	/*[DW1]*/
	u8 avg_idle_noise_pwr;
	u8 avg_cfo_l;
	u8 avg_cfo_m:4;
	u8 coarse_cfo_l:4;
	u8 coarse_cfo_m;
	/*[DW2]*/
	u8 rxevm_hdr;
	u8 rxevm_pld;
	u8 sig_len_l;
	u8 sig_len_m;
	/*[DW3]*/
	u8 antdiv_rslt_a:1;
	u8 antdiv_rslt_b:1 ;
	u8 antdiv_rslt_c:1;
	u8 antdiv_rslt_d:1;
	u8 preamble_type:1;
	u8 sync_mode:1;
	u8 rsvd_1_dummy_1bit:1;
	u8 cck_hw_antsw_occur_d:1;

	u8 dagc_a:5;
	u8 dagc_b_l:3;

	u8 dagc_b_m:2;
	u8 dagc_c:5;
	u8 dagc_d_l:1;

	u8 dagc_d_m:4;
	u8 rx_path_en_bitmap:4;
};

struct physts_ie_1_info {
	/*[DW0]*/
	u8 ie_hdr:5;
	u8 pop_idx:2;
	u8 rsvd_0_dummy_1bit:1;
	u8 rssi_avg_fd;
	u8 ch_idx_seg0;
	u8 rxsc:4;
	u8 rx_path_en_bitmap:4;
	/*[DW1]*/
	u8 avg_idle_noise_pwr;
	u8 avg_cfo_seg0_l;

	u8 avg_cfo_seg0_m:4;
	u8 avg_cfo_premb_seg0_l:4;

	u8 avg_cfo_premb_seg0_m;
	/*[DW2]*/
	u8 avg_snr:6;
	u8 ant_idx_a:1;
	u8 ant_idx_b:1;
	u8 evm_max;
	u8 evm_min;
	u8 pdp_he_ltf_and_gi_type:3;
	u8 is_su:1;
	u8 is_ldpc:1;
	u8 is_ndp:1;
	u8 is_stbc:1;
	u8 grant_bt:1;
	/*[DW3]*/
	u8 bf_gain_max:7;
	u8 is_awgn:1;
	u8 is_bf:1;
	u8 avg_cn_seg0:7;
	u8 sigval_below_th_tone_cnt_seg0;
	u8 cn_excess_th_tone_cnt_seg0;
	/*[DW4]*/
	u8 pwr_to_cca_l;
	u8 pwr_to_cca_m;
	u8 cca_to_agc;
	u8 cca_to_sbd;
	/*[DW5]*/
	u8 rsvd_1_dummy_1bit:1;
	u8 edcca_rpt_cnt:7;
	u8 edcca_total_smp_cnt:7;
	u8 edcca_rpt_curr_bw_max_l:1;
	u8 edcca_rpt_curr_bw_max_m:6;
	u8 edcca_rpt_curr_bw_min_l:2;
	u8 edcca_rpt_curr_bw_min_m:5;
	u8 bw_idx:3; /*0~6: 5, 10, 20, 40, 80, 160, 80_80*/
};

struct physts_ie_1_info_type1 {
	/*[DW0]*/
	u8 ie_hdr:5;
	u8 pop_idx:2;
	u8 rsvd_0_dummy_1bit:1;
	u8 rssi_avg_fd;
	u8 ch_idx_seg0;
	u8 rxsc:4;
	u8 rx_path_en_bitmap:4;
	/*[DW1]*/
	u8 avg_idle_noise_pwr;
	u8 td_p_fd_tracking_cfo_seg0_l;
	u8 td_p_fd_tracking_cfo_seg0_m:4;
	u8 avg_cfo_premb_seg0_l:4;
	u8 avg_cfo_premb_seg0_m;
	/*[DW2]*/
	u8 avg_snr:6;
	u8 ant_idx_a:1;
	u8 ant_idx_b:1;
	u8 evm_max;
	u8 evm_min;
	u8 pdp_he_ltf_and_gi_type:3;
	u8 is_su:1;
	u8 is_ldpc:1;
	u8 is_ndp:1;
	u8 is_stbc:1;
	u8 grant_bt:1;
	/*[DW3]*/
	u8 bf_gain_max:7;
	u8 is_awgn:1;
	u8 is_bf:1;
	u8 avg_cn_seg0:7;
	u8 sigval_below_th_tone_cnt_seg0;
	u8 cn_excess_th_tone_cnt_seg0;
	/*[DW4]*/
	u8 pwr_to_cca_l;
	u8 pwr_to_cca_m;
	u8 cca_to_agc;
	u8 cca_to_sbd;
	/*[DW5]*/
	u8 rsvd_1_dummy_1bit:1;
	u8 edcca_rpt_cnt:7;
	u8 edcca_total_smp_cnt:7;
	u8 edcca_rpt_curr_bw_max_l:1;
	u8 edcca_rpt_curr_bw_max_m:6;
	u8 edcca_rpt_curr_bw_min_l:2;
	u8 edcca_rpt_curr_bw_min_m:5;
	u8 bw_idx:3; /*0~6: 5, 10, 20, 40, 80, 160, 80_80*/
	/*[DW6]*/
	u8 ftm_t_off_l;
	u8 ftm_t_off_m:4;
	u8 rsvd_2_dummy:4;
	u8 brk_src_idx;
	u8 outer_buf_2sts;
	/*[DW7]*/
	u8 bt_gnt_tx_at_cca:1;
	u8 bt_gnt_tx_cnt:3;
	u8 bt_gnt_rx_at_cca:1;
	u8 bt_gnt_rx_cnt:3;
	u8 rsvd_3_dummy_1;
	u8 rsvd_3_dummy_2;
	u8 rsvd_3_dummy_3;
};

struct physts_ie_2_info {
	/*[DW0]*/
	u8 ie_hdr:5;
	u8 max_nsts:3;

	u8 midamble:1;
	u8 ltf_type:2 ;
	u8 gi:2;
	u8 is_mu_mimo:1;
	u8 c_cfo_i_l:2;

	u8 c_cfo_i_m2;
	u8 c_cfo_i_m1;
	/*[DW1]*/
	u8 rx_info_1;

	u8 rx_state_feq:5;
	u8 is_dl_ofdma:1;
	u8 c_cfo_q_l:2;

	u8 c_cfo_q_m2;
	u8 c_cfo_q_m1;
	/*[DW2]*/
	u8 est_cmped_phase;

	u8 is_dcm:1;
	u8 is_doppler:1;
	u8 pkt_extension:3;
	u8 rsvd_0_dummy_1bit:1;
	u8 f_cfo_i_l:2;

	u8 f_cfo_i_m2;
	u8 f_cfo_i_m1;
	/*[DW3]*/
	u8 n_ltf:3;
	u8 n_sym_l:5;

	u8 n_sym_m:6;
	u8 f_cfo_q_l:2;

	u8 f_cfo_q_m2;
	u8 f_cfo_q_m1;
};

struct physts_ie_3_info {
	/*[DW0]*/
	u8 ie_hdr:5;
	u8 rsvd_0_dummy_3bit:3;

	u8 avg_cn_seg1:7;
	u8 rsvd_1_dummy_1bit:1;

	u8 sigval_below_th_tone_cnt_seg1;
	u8 cn_excess_th_tone_cnt_seg1;

	/*[DW1]*/
	u8 avg_cfo_seg1_l;

	u8 avg_cfo_seg1_m:4;
	u8 rsvd_2_dummy_4bit:4;

	u8 avg_cfo_premb_seg1_l;

	u8 avg_cfo_premb_seg1_m:4;
	u8 rsvd_3_dummy_4bit:4;

	/*[DW2]*/
	u8 est_cmped_phase_seg1;

	u8 avg_snr_seg1:6;
	u8 c_cfo_i_seg1_l:2;

	u8 c_cfo_i_seg1_m2;
	u8 c_cfo_i_seg1_m1;

	u8 c_cfo_q_seg1_l2;
	u8 c_cfo_q_seg1_l1;

	u8 c_cfo_q_seg1_m:2;
	u8 f_cfo_i_seg1_l:6;

	u8 f_cfo_i_seg1_lm;

	u8 f_cfo_i_seg1_m:4;
	u8 f_cfo_q_seg1_l:4;

	u8 f_cfo_q_seg1_lm;

	u8 f_cfo_q_seg1_m:6;
	u8 rsvd_5_dummy_10bit_l:2;

	u8 rsvd_5_dummy_10bit_m;
	u8 ch_idx_seg1;
	u8 evm_max_seg1;
	u8 evm_min_seg1;
	u8 rsvd_6_dummy_8bit;
};

/*physts_ie_4_to_7_info*/
struct physts_ie_4_to_7_info {
	/*[DW0]*/
	u8 ie_hdr:5;
	u8 ant_idx:3;

	u8 sig_val_y;
	u8 rf_gain_idx;

	u8 rf_tia_gain_idx:1;
	u8 tia_shrink_indicator:1;
	u8 snr_lgy:6;

	/*[DW0]*/
	u8 evm_ss_y;

	u8 td_ant_weight:7;
	u8 ant_idx_msb:1;

	u8 dc_est_re;
	u8 dc_est_im;
};

struct physts_ie_8_ch_info {
	/*[DW0]*/
	u8 ie_hdr_l;

	u8 ie_hdr_m:4;
	u8 rxsc:4;

	u8 n_rx:3;
	u8 n_sts:3;
	u8 ch_info_len_l:2;

	u8 ch_info_len_m;
	/*[DW1]*/
	u8 evm_1_sts;
	u8 evm_2_sts;
	u8 avg_idle_noise_pwr;

	u8 is_ch_info_len_valid:1;
	u8 rsvd_0_dummy_7bit:7;
	u8 rsvd_1[8];
	u8 rsvd_2[8];
};

struct physts_ie_8_ch_raw_info {
	u8 *channel_info_raw;
};

struct physts_ie_9_lgcy_info {
	/*[DW0]*/
	u8 ie_hdr:5;
	u8 l_sig_l:3;

	u8 l_sig_lm;

	u8 l_sig_m:6;
	u8 rsvd_0_dummy_2bit:2;

	u8 rsvd_0_dummy_8bit;
	u8 rsvd_1_dummy_8bit;
	u8 rsvd_2_dummy_8bit;
	u8 rsvd_3_dummy_8bit;
	u8 rsvd_4_dummy_8bit;

};

struct physts_ie_9_vht_info {
	/*[DW0]*/
	u8 ie_hdr:5;
	u8 l_sig_l:3;

	u8 l_sig_lm;

	u8 l_sig_m:6;
	u8 sig_a1_l:2;

	u8 sig_a1_lm2;
	u8 sig_a1_lm1;

	u8 sig_a1_m:6;
	u8 sig_a2_l:2;

	u8 sig_a2_m;

	u8 rsvd_0_dummy_8bit;
};

struct physts_ie_9_he_info {
	/*[DW0]*/
	u8 ie_hdr:5;
	u8 l_sig_l:3;

	u8 l_sig_lm;

	u8 l_sig_m:6;
	u8 sig_a1_l:2;

	u8 sig_a1_m3;
	u8 sig_a1_m2;
	u8 sig_a1_m1;

	u8 sig_a2_l;
	u8 sig_a2_m;
};

struct physts_ie_10_cmn_info {
	/*[DW0]*/
	u8 ie_hdr_l;

	u8 ie_hdr_m:4;
	u8 rsvd_0_dummy_4bit:4;

	u8 rsvd_0_dummy_8bit;
	u8 rsvd_1_dummy_8bit;
	u8 rsvd_2_dummy_8bit;
	u8 rsvd_3_dummy_8bit;
	u8 rsvd_4_dummy_8bit;
	u8 rsvd_5_dummy_8bit;

};

struct physts_ie_11_pkt_info {
	/*[DW0]*/
	u8 pkt_format:2;
	u8 l_rate:3;
	u8 l_length_l:3;

	u8 l_length_lm;

	u8 l_length_m:1;
	u8 info_type_0:7;

	u8 info_type_1;
	u8 info_type_2;
	u8 info_type_3;

	u8 info_type_4:3;
	u8 time_stamp_l:5;

	u8 time_stamp_m:6;
	u8 state:2;
};

struct physts_ie_11_pkt_info_type1 {
	/*[DW0/1]*/
	u8 pkt_format:3;
	u8 pop_idx:2;
	u8 mac_frame_control_1:3;

	u8 mac_frame_control_2;

	u8 mac_frame_control_3:1;
	u8 information_type_1:7;

	u8 information_type_2;
	u8 information_type_3;
	u8 information_type_4;

	u8 information_type_5:3;
	u8 time_stamp_l:5;

	u8 time_stamp_m:6;
	u8 state:2;
};

struct physts_ie_11_info {
	/*[DW0]*/
	u8 ie_hdr:5;
	u8 l_sig_l:3;

	u8 l_sig_lm;

	u8 l_sig_m:6;
	u8 sig_a1_l:2;

	u8 sig_a1_m3;
	u8 sig_a1_m2;
	u8 sig_a1_m1;

	u8 sig_a2_l;
	u8 sig_a2_m;

	u8 time_stamp_l;

	u8 time_stamp_m:3;
	u8 rx_pkt_info_idx:4;
	u8 tx_pkt_info_idx_l:1;

	u8 tx_pkt_info_idx_m:3;
	u8 rsvd_0_dummy_5bit:5;

	u8 rsvd_0_dummy_8bit;
	u8 rsvd_1_dummy_8bit;
	u8 rsvd_2_dummy_8bit;
	u8 rsvd_3_dummy_8bit;
	u8 rsvd_4_dummy_8bit;

	struct physts_ie_11_pkt_info pkt_info_rx_i[10];
	struct physts_ie_11_pkt_info pkt_info_tx_i[10];
};

struct physts_ie_11_info_type1 {
	/*[DW0]*/
	u8 ie_hdr:5;
	u8 time_stamp_l:3;
	u8 time_stamp_m;
	u8 rx_pktinfo_idx:4;
	u8 tx_pktinfo_idx:4;
	u8 rsvd_0_dummy_8bit;
	/*[DW1]*/
	u8 rsvd_1_dummy_8bit;
	u8 rsvd_2_dummy_8bit;
	u8 rsvd_3_dummy_8bit;
	u8 rsvd_4_dummy_8bit;
	/*[DW2/3]*/
	struct physts_ie_11_pkt_info_type1 pkt_info_rx_i[10];
	struct physts_ie_11_pkt_info_type1 pkt_info_tx_i[10];
};

struct physts_ie_12_user_info {
	u8 sig_val_ss0_seg_cr_user_i;
	u8 sig_val_ss1_seg_cr_user_i;
	u8 sig_val_ss2_seg_cr_user_i;
	u8 sig_val_ss3_seg_cr_user_i;
	u8 sig_bad_tone_cnt_seg_cr_user_i;
	u8 cn_bad_tone_cnt_seg_cr_user_i;
};

struct physts_ie_12_cmn_info {
	u8 ie_hdr_l;

	u8 ie_hdr_m:4;
	u8 rsvd_0_dummy_4bit:4;

	u8 n_user;
};

struct physts_ie_13_user_info {
	u8 mcs:6;
	u8 fec_type:1;
	u8 is_bf:1;

	u8 n_sts_ru_total:3;
	u8 n_sts:3;
	u8 pilot_exist:2;

	u8 start_sts:3;
	u8 pdp_he_ltf_and_gi_type:3;
	u8 is_mu_mimo:1;
	u8 is_awgn:1;

	u8 rx_evm_max_seg_cr;
	u8 rx_evm_min_seg_cr;
	u8 snr;
	u8 ru_alloc;

	u8 avg_cn_seg_cr:7;
	u8 is_dcm:1;

	u8 sta_id_l;

	u8 sta_id_m:3;
	u8 rsvd_0_dummy_5bit:5;
};

struct physts_ie_13_cmn_info_p1 {
	u8 ie_hdr_l;

	u8 ie_hdr_m:4;
	u8 rsvd_0_dummy_4bit:4;

	u8 n_user;
	u8 rsvd_1_dummy_8bit;
};

struct physts_ie_13_cmn_info_p2 {
	u8 n_not_sup_sta;

	u8 not_support_sta_id0_l;

	u8 not_support_sta_id0_m:3;
	u8 rsvd_0_dummy_5bit:5;

	u8 not_support_sta_id1_l;

	u8 not_support_sta_id1_m:3;
	u8 rsvd_1_dummy_5bit:5;

	u8 not_support_sta_id2_l;

	u8 not_support_sta_id2_m:3;
	u8 not_support_sta_id3_l:5;

	u8 not_support_sta_id3_m:6;
	u8 rsvd_2_dummy_2bit:2;
};

struct physts_ie_14_user_info {
	u8 cqi_bitmap_ul_tb;
	u8 cqi_raw_len_ul_tb;
	u8 *cqi_raw_ul_tb_addr;
};

struct physts_ie_14_cmn_info {
	u8 ie_hdr_l;
	u8 ie_hdr_m:4;
	u8 rxinfo_ndp_en:1;
	u8 rsvd_0_dummy_3bit:3;
	u8 n_user;
	u8 rxinfo_ndp_1;

	u8 rsvd_0_dummy_8bit;
	u8 rsvd_1_dummy_8bit;
	u8 rsvd_2_dummy_8bit;
	u8 rsvd_3_dummy_8bit;
};

struct physts_ie_15_cmn_info {
	u8 ie_hdr_l;

	u8 ie_hdr_m:4;
	u8 rsvd_0_dummy_4bit:4;

	u8 n_user;
};

struct physts_ie_15_user_info {
	/* 64bit cmn_info */
	u8 mcs:6;
	u8 fec_type:1;
	u8 is_bf:1;

	u8 n_sts_ru_total:3;
	u8 n_sts:3;
	u8 pilot_exist:2;

	u8 start_sts:3;
	u8 pdp_he_ltf_and_gi_type:3;
	u8 is_mu_mimo:1;
	u8 is_awgn:1;

	u8 rx_evm_max_seg_cr;
	u8 rx_evm_min_seg_cr;
	u8 snr;
	u8 ru_alloc;

	u8 avg_cn_seg_cr:7;
	u8 is_dcm:1;

	/* others */
	u8 uid;
	u8 avg_cfo_seg0_l;

	u8 avg_cfo_seg0_m:4;
	u8 rsvd_0_dummy_4bit:4;

	u8 rssi_m_ul_tb_l;

	u8 rssi_m_ul_tb_m:1;
	u8 rsvd_1_dummy_7bit:7;
};

struct physts_ie_17_cmn_info {
	u8 ie_hdr_l;

	u8 ie_hdr_m:4;
	u8 n_user_l:4;

	u8 n_user_m:4;
	u8 rsvd_0_dummy_4bit:4;

	u8 rsvd_0_dummy_8it;
	u8 rsvd_1_dummy_8it;
	u8 rsvd_2_dummy_8it;
	u8 rsvd_3_dummy_8it;
	u8 rsvd_4_dummy_8it;

	/*64bit rx_tb_cmn_ctrl*/

	u8 stbc_en:1;
	u8 ldpc_extra:1;
	u8 doppler_en:1;
	u8 midamle_mode:1;
	u8 gi_type:2;
	u8 ltf_type:2;

	u8 n_ltf:3;
	u8 n_sym_l:5;

	u8 n_sym_m:6;
	u8 pe_idx_l:2;

	u8 pe_idx_m:1;
	u8 pre_fec_factor:2;
	u8 n_usr_l:5;

	u8 n_usr_m:3;
	u8 mumimo_ltf_mode_en:1;
	u8 ndp:1;
	u8 pri_exp_rssi_dbm_l:3;

	u8 pri_exp_rssi_dbm_m:4;
	u8 dbw_idx:2;
	u8 rsvd1:2;

	u8 rxtime_l:8;

	u8 rxtime_m:6;
	u8 rsvd2:2;
};

struct physts_ie_17_user_info {
	/*64bit rx_tb_user_ctrl*/
	u8 u_id;

	u8 ru_alloc;

	u8 n_sts_ru_tot:3;
	u8 rsvd1:2;
	u8 strt_sts:3;

	u8 n_sts:3;
	u8 fec_type:1;
	u8 mcs:4;

	u8 rsvd2:2;
	u8 dcm_en:1;
	u8 rsvd3:5;

	u8 rsvd4;
	u8 rsvd5;
	u8 rsvd6;
};

struct physts_ie_18_info {
	/*[DW0]*/
	u8 ie_hdr:5;
	u8 is_seg1_exist:1;
	u8 rsvd_0_dummy_2bit:2;

	u8 rx_time_l;

	u8 rx_time_m:6;
	u8 rsvd_1_dummy_2bit:2;

	u8 ch_len_lgcy_seg0;
	u8 bw_det_seg0;

	u8 snr_idx_lgcy_seg0:3;
	u8 pdp_idx_lgcy_seg0:3;
	u8 rsvd_2_dummy_2bit:2;

	u8 pfd_flow_l;

	u8 pfd_flow_m:4;
	u8 rsvd_3_dummy_4bit:4;

	u8 ch_len_lgcy_seg1;
	u8 bw_det_seg1;

	u8 snr_idx_lgcy_seg1:3;
	u8 pdp_idx_lgcy_seg1:3;
	u8 rsvd_3_dummy_2bit:2;

	u8 rsvd_3_dummy_8bit;

	u8 zero_padding_l2;
	u8 zero_padding_l1;
	u8 zero_padding_m2;
	u8 zero_padding_m1;
};

struct physts_ie_18_info_type1 {
	/*[DW0]*/
	u8 ie_hdr:5;
	u8 rsvd_0_dummy_3bit:3;

	u8 rxtime_l;

	u8 rxtime_m:6;
	u8 pfd_flow_1:2;
	/*[DW1]*/
	u8 pfd_flow_2;

	u8 pfd_flow_3:2;
	u8 ch_len_lgcy_seg0_l:6;

	u8 ch_len_lgcy_seg0_m:3;
	u8 ch_len_lgcy_seg1_l:5;

	u8 ch_len_lgcy_seg1_m:4;
	u8 rsvd_0_dummy_4bit:4;

	u8 bw_det_seg0;
	u8 bw_det_seg1;
	u8 snr_lgy_patha;
	u8 snr_lgy_pathb;
	u8 snr_lgy_pathc;
	u8 snr_lgy_pathd;

	u8 snr_idx_lgy_patha:3;
	u8 tmax_idx_lgy_patha:3;
	u8 snr_idx_lgy_pathb_l:2;

	u8 snr_idx_lgy_pathb_m:1;
	u8 tmax_idx_lgy_pathb:3;
	u8 snr_idx_lgy_pathc:3;
	u8 tmax_idx_lgy_pathc_l:1;

	u8 tmax_idx_lgy_pathc_m:2;
	u8 snr_idx_lgy_pathd:3;
	u8 tmax_idx_lgy_pathd:3;

	u8 gd_phase_lgy_sub0_patha;
	u8 gd_phase_lgy_sub1_patha;
	u8 gd_phase_lgy_sub2_patha;
	u8 gd_phase_lgy_sub3_patha;
	u8 gd_phase_lgy_sub4_patha;
	u8 gd_phase_lgy_sub5_patha;
	u8 gd_phase_lgy_sub6_patha;
	u8 gd_phase_lgy_sub7_patha;
	u8 gd_phase_lgy_sub0_pathb;
	u8 gd_phase_lgy_sub1_pathb;
	u8 gd_phase_lgy_sub2_pathb;
	u8 gd_phase_lgy_sub3_pathb;
	u8 gd_phase_lgy_sub4_pathb;
	u8 gd_phase_lgy_sub5_pathb;
	u8 gd_phase_lgy_sub6_pathb;
	u8 gd_phase_lgy_sub7_pathb;
	u8 gd_phase_lgy_sub0_pathc;
	u8 gd_phase_lgy_sub1_pathc;
	u8 gd_phase_lgy_sub2_pathc;
	u8 gd_phase_lgy_sub3_pathc;
	u8 gd_phase_lgy_sub4_pathc;
	u8 gd_phase_lgy_sub5_pathc;
	u8 gd_phase_lgy_sub6_pathc;
	u8 gd_phase_lgy_sub7_pathc;
	u8 gd_phase_lgy_sub0_pathd;
	u8 gd_phase_lgy_sub1_pathd;
	u8 gd_phase_lgy_sub2_pathd;
	u8 gd_phase_lgy_sub3_pathd;
	u8 gd_phase_lgy_sub4_pathd;
	u8 gd_phase_lgy_sub5_pathd;
	u8 gd_phase_lgy_sub6_pathd;
	u8 gd_phase_lgy_sub7_pathd;
};

struct physts_ie_19_info {
	/*[DW0]*/
	u8 ie_hdr:5;
	u8 rsvd_0_dummy_2bit:2;
	u8 tx_over_flow:1;

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
	u8 rsvd_1_dummy_8bit;
	u8 pop_reg_pwr;
	u8 pop_trig_pwr;
	u8 early_drop_pwr;

	u8 rsvd_2_dummy_8bit;
	u8 rsvd_3_dummy_8bit;
	u8 rsvd_4_dummy_8bit;
	u8 rsvd_5_dummy_8bit;
};

struct physts_ie_19_info_type1 {
	/*[DW0]*/
	u8 ie_hdr:5;
	u8 rsvd_0_dummy_2bit:2;
	u8 tx_over_flow:1;

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
	u8 rsvd_1_dummy_8bit;
	u8 pop_ref_pwr;
	u8 pop_trig_pwr;
	u8 early_drop_pwr;

	u8 rsvd_2_dummy_8bit;
	u8 rsvd_3_dummy_8bit;

	u8 rsvd_4_dummy_7bit:7;
	u8 find_eof:1;

	u8 mpdu_stat_len;
};

struct physts_ie_20_user_info {
	u8 ch_smo_n_block_lgcy_sub_0_seg_0:2;
	u8 ch_smo_n_block_lgcy_sub_1_seg_0:2;
	u8 ch_smo_n_block_lgcy_sub_2_seg_0:2;
	u8 ch_smo_n_block_lgcy_sub_3_seg_0:2;

	u8 ch_smo_en_lgcy_seg_0:4;
	u8 ch_smo_en_non_lgcy_seg_0:4;

	u8 ch_smo_n_block_non_lgcy_seg_0:3;
	u8 rsvd_0_dummy_5bit:5;

	u8 ch_len_non_lgcy_sts_0_seg_0;
	u8 ch_len_non_lgcy_sts_1_seg_0;
	u8 ch_len_non_lgcy_sts_2_seg_0;
	u8 ch_len_non_lgcy_sts_3_seg_0;

	u8 snr_idx_non_lgy_sts_0_seg_0:3;
	u8 snr_idx_non_lgy_sts_1_seg_0:3;
	u8 snr_idx_non_lgy_sts_2_seg_0_l:2;

	u8 snr_idx_non_lgy_sts_2_seg_0_m:1;
	u8 snr_idx_non_lgy_sts_3_seg_0:3;
	u8 pdp_idx_non_lgcy_sts_0_seg_0:3;
	u8 pdp_idx_non_lgcy_sts_1_seg_0_l:1;

	u8 pdp_idx_non_lgcy_sts_1_seg_0_m:2;
	u8 pdp_idx_non_lgcy_sts_2_seg_0:3;
	u8 pdp_idx_non_lgcy_sts_3_seg_0:3;

	u8 snr_non_lgy_sts_0_seg_0;
	u8 snr_non_lgy_sts_1_seg_0;
	u8 snr_non_lgy_sts_2_seg_0;
	u8 snr_non_lgy_sts_3_seg_0;
	u8 evm_ss_0_seg_0;
	u8 evm_ss_1_seg_0;
	u8 evm_ss_2_seg_0;
	u8 evm_ss_3_seg_0;
};

struct physts_ie_20_cmn_info {
	u8 ie_hdr_l;

	u8 ie_hdr_m:4;
	u8 rsvd_0_dummy_4bit:4;

	u8 n_user;
};

struct physts_ie_20_21_info_type1{
	u8 ie_hdr_l;

	u8 ie_hdr_m:4;
	u8 user_idx_l:4;

	u8 user_idx_m:4;
	u8 rcfo_data_start_1:4;

	u8 rcfo_data_start_2;

	u8 rcfo_data_start_3:4;
	u8 rcfo_data_end_1:4;

	u8 rcfo_data_end_2;

	u8 rcfo_data_end_3:4;
	u8 ch_len_non_lgy_l:4;

	u8 ch_len_non_lgy_m:5;
	u8 rsvd_0_dummy_3bit:3;

	u8 snr_non_lgy_sts0_patha;
	u8 snr_non_lgy_sts1_patha;
	u8 snr_non_lgy_sts2_patha;
	u8 snr_non_lgy_sts3_patha;

	u8 snr_idx_non_lgy_sts0_patha:3;
	u8 snr_idx_non_lgy_sts1_patha:3;
	u8 snr_idx_non_lgy_sts2_patha_l:2;

	u8 snr_idx_non_lgy_sts2_patha_m:1;
	u8 snr_idx_non_lgy_sts3_patha:3;
	u8 tmax_idx_non_lgy_sts0_patha:3;
	u8 tmax_idx_non_lgy_sts1_patha_l:1;

	u8 tmax_idx_non_lgy_sts1_patha_m:2;
	u8 tmax_idx_non_lgy_sts2_patha:3;
	u8 tmax_idx_non_lgy_sts3_patha:3;

	u8 gd_phase_non_lgy_sts0_patha;
	u8 gd_phase_non_lgy_sts1_patha;
	u8 gd_phase_non_lgy_sts2_patha;
	u8 gd_phase_non_lgy_sts3_patha;
	u8 evm_ss0;
	u8 noise_var_start_patha_l;
	u8 noise_var_start_patha_m;
	u8 noise_var_end_patha_l;
	u8 noise_var_end_patha_m;

	u8 snr_non_lgy_sts0_pathb;
	u8 snr_non_lgy_sts1_pathb;
	u8 snr_non_lgy_sts2_pathb;
	u8 snr_non_lgy_sts3_pathb;

	u8 snr_idx_non_lgy_sts0_pathb:3;
	u8 snr_idx_non_lgy_sts1_pathb:3;
	u8 snr_idx_non_lgy_sts2_pathb_l:2;

	u8 snr_idx_non_lgy_sts2_pathb_m:1;
	u8 snr_idx_non_lgy_sts3_pathb:3;
	u8 tmax_idx_non_lgy_sts0_pathb:3;
	u8 tmax_idx_non_lgy_sts1_pathb_l:1;

	u8 tmax_idx_non_lgy_sts1_pathb_m:2;
	u8 tmax_idx_non_lgy_sts2_pathb:3;
	u8 tmax_idx_non_lgy_sts3_pathb:3;

	u8 gd_phase_non_lgy_sts0_pathb;
	u8 gd_phase_non_lgy_sts1_pathb;
	u8 gd_phase_non_lgy_sts2_pathb;
	u8 gd_phase_non_lgy_sts3_pathb;
	u8 evm_ss1;
	u8 noise_var_start_pathb_l;
	u8 noise_var_start_pathb_m;
	u8 noise_var_end_pathb_l;
	u8 noise_var_end_pathb_m;

	u8 snr_non_lgy_sts0_pathc;
	u8 snr_non_lgy_sts1_pathc;
	u8 snr_non_lgy_sts2_pathc;
	u8 snr_non_lgy_sts3_pathc;

	u8 snr_idx_non_lgy_sts0_pathc:3;
	u8 snr_idx_non_lgy_sts1_pathc:3;
	u8 snr_idx_non_lgy_sts2_pathc_l:2;

	u8 snr_idx_non_lgy_sts2_pathc_m:1;
	u8 snr_idx_non_lgy_sts3_pathc:3;
	u8 tmax_idx_non_lgy_sts0_pathc:3;
	u8 tmax_idx_non_lgy_sts1_pathc_l:1;

	u8 tmax_idx_non_lgy_sts1_pathc_m:2;
	u8 tmax_idx_non_lgy_sts2_pathc:3;
	u8 tmax_idx_non_lgy_sts3_pathc:3;

	u8 gd_phase_non_lgy_sts0_pathc;
	u8 gd_phase_non_lgy_sts1_pathc;
	u8 gd_phase_non_lgy_sts2_pathc;
	u8 gd_phase_non_lgy_sts3_pathc;
	u8 evm_ss2;
	u8 noise_var_start_pathc_l;
	u8 noise_var_start_pathc_m;
	u8 noise_var_end_pathc_l;
	u8 noise_var_end_pathc_m;

	u8 snr_non_lgy_sts0_pathd;
	u8 snr_non_lgy_sts1_pathd;
	u8 snr_non_lgy_sts2_pathd;
	u8 snr_non_lgy_sts3_pathd;

	u8 snr_idx_non_lgy_sts0_pathd:3;
	u8 snr_idx_non_lgy_sts1_pathd:3;
	u8 snr_idx_non_lgy_sts2_pathd_l:2;

	u8 snr_idx_non_lgy_sts2_pathd_m:1;
	u8 snr_idx_non_lgy_sts3_pathd:3;
	u8 tmax_idx_non_lgy_sts0_pathd:3;
	u8 tmax_idx_non_lgy_sts1_pathd_l:1;

	u8 tmax_idx_non_lgy_sts1_pathd_m:2;
	u8 tmax_idx_non_lgy_sts2_pathd:3;
	u8 tmax_idx_non_lgy_sts3_pathd:3;

	u8 gd_phase_non_lgy_sts0_pathd;
	u8 gd_phase_non_lgy_sts1_pathd;
	u8 gd_phase_non_lgy_sts2_pathd;
	u8 gd_phase_non_lgy_sts3_pathd;
	u8 evm_ss3;
	u8 noise_var_start_pathd_l;
	u8 noise_var_start_pathd_m;
	u8 noise_var_end_pathd_l;
	u8 noise_var_end_pathd_m;
};

struct physts_ie_21_user_info {
	u8 ch_smo_n_block_lgcy_sub_0_seg_1:2;
	u8 ch_smo_n_block_lgcy_sub_1_seg_1:2;
	u8 ch_smo_n_block_lgcy_sub_2_seg_1:2;
	u8 ch_smo_n_block_lgcy_sub_3_seg_1:2;

	u8 ch_smo_en_lgcy_seg_1:4;
	u8 ch_smo_en_non_lgcy_seg_1:4;

	u8 ch_smo_n_block_non_lgcy_seg_1:3;
	u8 rsvd_0_dummy_5bit:5;

	u8 ch_len_non_lgcy_sts_0_seg_1;
	u8 ch_len_non_lgcy_sts_1_seg_1;
	u8 ch_len_non_lgcy_sts_2_seg_1;
	u8 ch_len_non_lgcy_sts_3_seg_1;

	u8 snr_idx_non_lgy_sts_0_seg_1:3;
	u8 snr_idx_non_lgy_sts_1_seg_1:3;
	u8 snr_idx_non_lgy_sts_2_seg_1_l:2;

	u8 snr_idx_non_lgy_sts_2_seg_1_m:1;
	u8 snr_idx_non_lgy_sts_3_seg_1:3;
	u8 pdp_idx_non_lgcy_sts_0_seg_1:3;
	u8 pdp_idx_non_lgcy_sts_1_seg_1_l:1;

	u8 pdp_idx_non_lgcy_sts_1_seg_1_m:2;
	u8 pdp_idx_non_lgcy_sts_2_seg_1:3;
	u8 pdp_idx_non_lgcy_sts_3_seg_1:3;

	u8 snr_non_lgy_sts_0_seg_1;
	u8 snr_non_lgy_sts_1_seg_1;
	u8 snr_non_lgy_sts_2_seg_1;
	u8 snr_non_lgy_sts_3_seg_1;
	u8 evm_ss_0_seg_1;
	u8 evm_ss_1_seg_1;
	u8 evm_ss_2_seg_1;
	u8 evm_ss_3_seg_1;
};

struct physts_ie_21_cmn_info {
	u8 ie_hdr_l;

	u8 ie_hdr_m:4;
	u8 rsvd_0_dummy_4bit:4;

	u8 n_user;
};

struct physts_ie_22_user_info {
	u8 pw_norm_lgcy_path0;
	u8 pw_norm_lgcy_path1;
	u8 pw_norm_lgcy_path2;
	u8 pw_norm_lgcy_path3;
	u8 ant_wgt_lgcy_path0;
	u8 ant_wgt_lgcy_path1;
	u8 ant_wgt_lgcy_path2;
	u8 ant_wgt_lgcy_path3;
};

struct physts_ie_22_cmn_info {
	u8 ie_hdr_l;

	u8 ie_hdr_m:4;
	u8 rsvd_0_dummy_4bit:4;

	u8 n_user;

	u8 rsvd_0_dummy_8bit;
	u8 rsvd_1_dummy_8bit;
	u8 rsvd_2_dummy_8bit;
	u8 rsvd_3_dummy_8bit;
	u8 rsvd_4_dummy_8bit;

	u8 pw_norm_lgcy_path0;
	u8 pw_norm_lgcy_path1;
	u8 pw_norm_lgcy_path2;
	u8 pw_norm_lgcy_path3;
	u8 ant_wgt_lgcy_path0;
	u8 ant_wgt_lgcy_path1;
	u8 ant_wgt_lgcy_path2;
	u8 ant_wgt_lgcy_path3;
};

struct physts_ie_24_info {
	/*[DW0]*/
	u8 ie_hdr:5;
	u8 pre_agc_step_a:2;
	u8 aci_indicator_a:1;

	u8 l_fine_agc_step_a:2;
	u8 ht_fine_agc_step_a:2;
	u8 tia_shrink_indicator_a:1;
	u8 pre_gain_code_tia_a:1;
	u8 l_fine_gain_code_tia_a:1;
	u8 ht_fine_gain_code_tia_a:1;

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

	u8 aci2sig_db:7;
	u8 aci_det:1;

	u8 sb5m_ratio_0;
	u8 sb5m_ratio_1;
	u8 sb5m_ratio_2;
	u8 sb5m_ratio_3;

	u8 rsvd_0_dummy_8bit;
	u8 rsvd_1_dummy_8bit;
	u8 rsvd_2_dummy_8bit;
};

struct physts_ie_25_info {
	/*[DW0]*/
	u8 ie_hdr:5;
	u8 pre_agc_step_b:2;
	u8 aci_indicator_b:1;

	u8 l_fine_agc_step_b:2;
	u8 ht_fine_agc_step_b:2;
	u8 tia_shrink_indicator_b:1;
	u8 pre_gain_code_tia_b:1;
	u8 l_fine_gain_code_tia_b:1;
	u8 ht_fine_gain_code_tia_b:1;

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

	u8 aci2sig_db:7;
	u8 aci_det:1;

	u8 sb5m_ratio_0;
	u8 sb5m_ratio_1;
	u8 sb5m_ratio_2;
	u8 sb5m_ratio_3;

	u8 rsvd_0_dummy_8bit;
	u8 rsvd_1_dummy_8bit;
	u8 rsvd_2_dummy_8bit;
};

struct physts_ie_26_info {
	/*[DW0]*/
	u8 ie_hdr:5;
	u8 pre_agc_step_c:2;
	u8 aci_indicator_c:1;

	u8 l_fine_agc_step_c:2;
	u8 ht_fine_agc_step_c:2;
	u8 tia_shrink_indicator_c:1;
	u8 pre_gain_code_tia_c:1;
	u8 l_fine_gain_code_tia_c:1;
	u8 ht_fine_gain_code_tia_c:1;

	u8 pre_gain_code_c;
	u8 l_fine_gain_code_c;
	u8 ht_fine_gain_code_c;
	u8 l_dagc_c;
	u8 ht_dagc_c;
	u8 pre_ibpwrdbm_c;
	u8 pre_wbpwrdbm_c;
	u8 l_ibpwrdbm_c;
	u8 l_wbpwrdbm_c;
	u8 ht_ibpwrdbm_c;
	u8 ht_wbpwrdbm_c;
	u8 l_dig_ibpwrdbm_c;
	u8 ht_dig_ibpwrdbm_c;
	u8 lna_inpwrdbm_c;

	u8 aci2sig_db:7;
	u8 aci_det:1;

	u8 sb5m_ratio_0;
	u8 sb5m_ratio_1;
	u8 sb5m_ratio_2;
	u8 sb5m_ratio_3;

	u8 rsvd_0_dummy_8bit;
	u8 rsvd_1_dummy_8bit;
	u8 rsvd_2_dummy_8bit;
};

struct physts_ie_27_info {
	/*[DW0]*/
	u8 ie_hdr:5;
	u8 pre_agc_step_d:2;
	u8 aci_indicator_d:1;

	u8 l_fine_agc_step_d:2;
	u8 ht_fine_agc_step_d:2;
	u8 tia_shrink_indicator_d:1;
	u8 pre_gain_code_tia_d:1;
	u8 l_fine_gain_code_tia_d:1;
	u8 ht_fine_gain_code_tia_d:1;

	u8 pre_gain_code_d;
	u8 l_fine_gain_code_d;
	u8 ht_fine_gain_code_d;
	u8 l_dagc_d;
	u8 ht_dagc_d;
	u8 pre_ibpwrdbm_d;
	u8 pre_wbpwrdbm_d;
	u8 l_ibpwrdbm_d;
	u8 l_wbpwrdbm_d;
	u8 ht_ibpwrdbm_d;
	u8 ht_wbpwrdbm_d;
	u8 l_dig_ibpwrdbm_d;
	u8 ht_dig_ibpwrdbm_d;
	u8 lna_inpwrdbm_d;

	u8 aci2sig_db:7;
	u8 aci_det:1;

	u8 sb5m_ratio_0;
	u8 sb5m_ratio_1;
	u8 sb5m_ratio_2;
	u8 sb5m_ratio_3;

	u8 rsvd_0_dummy_8bit;
	u8 rsvd_1_dummy_8bit;
	u8 rsvd_2_dummy_8bit;
};

struct physts_ie_28_info {
	/*[DW0]*/
	u8 ie_hdr:5;
	u8 rsvd_0_dummy_2bit:2;
	u8 ant_weight_a_l:1;

	u8 ant_weight_a_m;
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

struct physts_ie_29_info {
	/*[DW0]*/
	u8 ie_hdr:5;
	u8 rsvd_0_dummy_2bit:2;
	u8 ant_weight_b_l:1;

	u8 ant_weight_b_m;
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

struct physts_ie_30_info {
	/*[DW0]*/
	u8 ie_hdr:5;
	u8 rsvd_0_dummy_2bit:2;
	u8 ant_weight_c_l:1;

	u8 ant_weight_c_m;
	u8 h3_real_c;
	u8 h3_imag_c;
	u8 h4_real_c;
	u8 h4_imag_c;
	u8 h5_real_c;
	u8 h5_imag_c;
	u8 h6_real_c;
	u8 h6_imag_c;
	u8 h7_real_c;
	u8 h7_imag_c;
	u8 h8_real_c;
	u8 h8_imag_c;
	u8 h9_real_c;
	u8 h9_imag_c;
	u8 h10_real_c;
	u8 h10_imag_c;
	u8 h11_real_c;
	u8 h11_imag_c;
	u8 h12_real_c;
	u8 h12_imag_c;
	u8 h13_real_c;
	u8 h13_imag_c;
	u8 h14_real_c;
	u8 h14_imag_c;
	u8 h15_real_c;
	u8 h15_imag_c;
	u8 h16_real_c;
	u8 h16_imag_c;
	u8 h17_real_c;
	u8 h17_imag_c;
};

struct physts_ie_31_info {
	/*[DW0]*/
	u8 ie_hdr:5;
	u8 rsvd_0_dummy_2bit:2;
	u8 ant_weight_d_l:1;

	u8 ant_weight_d_m;
	u8 h3_real_d;
	u8 h3_imag_d;
	u8 h4_real_d;
	u8 h4_imag_d;
	u8 h5_real_d;
	u8 h5_imag_d;
	u8 h6_real_d;
	u8 h6_imag_d;
	u8 h7_real_d;
	u8 h7_imag_d;
	u8 h8_real_d;
	u8 h8_imag_d;
	u8 h9_real_d;
	u8 h9_imag_d;
	u8 h10_real_d;
	u8 h10_imag_d;
	u8 h11_real_d;
	u8 h11_imag_d;
	u8 h12_real_d;
	u8 h12_imag_d;
	u8 h13_real_d;
	u8 h13_imag_d;
	u8 h14_real_d;
	u8 h14_imag_d;
	u8 h15_real_d;
	u8 h15_imag_d;
	u8 h16_real_d;
	u8 h16_imag_d;
	u8 h17_real_d;
	u8 h17_imag_d;
};

 /*@--------------------------[Prptotype]-------------------------------------*/
#endif

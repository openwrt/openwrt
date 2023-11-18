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
#ifndef __HALBB_PHYSTS_IE_B_ENDIAN_7_H__
#define __HALBB_PHYSTS_IE_B_ENDIAN_7_H__

 /*@--------------------------[Define] ---------------------------------------*/

 /*@--------------------------[Enum]------------------------------------------*/
 
 /*@--------------------------[Structure]-------------------------------------*/

struct physts_7_ie_0_info {
	/*[DW0]*/
	u8 rpl_l:1;
	u8 pop_idx:2;
	u8 ie_hdr:5;

	u8 rpl_m;
	u8 cca_time;
	u8 cck_hw_antsw_occur_c:1;
	u8 cck_hw_antsw_occur_b:1;
	u8 cck_hw_antsw_occur_a:1;
	u8 antwgt_gain_diff:5;
	/*[DW1]*/
	u8 avg_idle_noise_pwr;
	u8 avg_cfo_l;
	u8 coarse_cfo_l:4;
	u8 avg_cfo_m:4;
	u8 coarse_cfo_m;
	/*[DW2]*/
	u8 rxevm_hdr;
	u8 rxevm_pld;
	u8 sig_len_l;
	u8 sig_len_m;
	/*[DW3]*/
	u8 cck_hw_antsw_occur_d:1;
	u8 rsvd_1_dummy_1bit:1;
	u8 sync_mode:1;
	u8 preamble_type:1;
	u8 antdiv_rslt_d:1;
	u8 antdiv_rslt_c:1;
	u8 antdiv_rslt_b:1 ;
	u8 antdiv_rslt_a:1;

	u8 dagc_a_l;
	
	u8 dagc_b_l:6;
	u8 dagc_a_m:2;

	u8 rx_path_en_bitmap:4;
	u8 dagc_b_m:4;
	/*[DW4]*/
	u8 rpl_td_a_l;
	u8 rpl_td_b_l:7;
	u8 rpl_td_a_m:1;
	
	u8 rpl_td_c_l:6;
	u8 rpl_td_b_m:2;
	
	u8 rsvd_2_dummy_5bit:5;
	u8 rpl_td_c_m:3;
	/*[DW5]*/
	u8 rpl_td_d_l;
	
	u8 is_6g_idx:1;
	u8 rsvd_3_dummy_6bit:6;
	u8 rpl_td_d_m:1;

	u8 ch_idx;
	
	u8 rsvd_4_dummy_6bit:6;
	u8 band:2;
	/*[DW6]*/
	u8 dagc_c_l;
	
	u8 dagc_d_l:6;
	u8 dagc_c_m:2;
	
	u8 rsvd_5_dummy_4bit:4;
	u8 dagc_d_m:4;
	u8 rsvd_6;
	/*[DW7]*/
	u8 rsvd_7;
	u8 rsvd_8;
	u8 rsvd_9;
	u8 rsvd_0;
};

struct physts_7_ie_1_info {
	/*[DW0]*/
	u8 rsvd_0_dummy_1bit:1;
	u8 pop_idx:2;
	u8 ie_hdr:5;
	u8 rssi_avg_fd;
	u8 ch_idx_seg0;
	u8 rx_path_en_bitmap:4;
	u8 rxsb:4;
	/*[DW1]*/
	u8 avg_idle_noise_pwr;
	u8 avg_cfo_seg0_l;

	u8 avg_cfo_premb_seg0_l:4;
	u8 avg_cfo_seg0_m:4;

	u8 avg_cfo_premb_seg0_m;
	/*[DW2]*/
	u8 ant_idx_b:1;
	u8 ant_idx_a:1;
	u8 avg_snr:6; /*lgcy*/
	u8 evm_max; 			/*user_cr*/
	u8 evm_min; 			/*user_cr*/
	u8 grant_bt_rx:1;
	u8 is_stbc:1;
	u8 is_ndp:1;
	u8 is_ldpc:1;			/*user_cr*/
	u8 is_su:1;
	u8 pdp_he_ltf_and_gi_type:3;	/*user_cr*/
	/*[DW3]*/
	u8 is_awgn:1; /*lgcy*/
	u8 bf_gain_max:7;		/*user_cr*/
	u8 avg_cn_seg0:7;
	u8 is_bf:1;			/*user_cr*/
	u8 brk_src_idx;					/*change*/
	u8 outer_buf_2sts;				/*change*/
	/*[DW4]*/
	u8 pwr_to_cca_l;
	u8 pwr_to_cca_m;
	u8 cca_to_agc;
	u8 cca_to_sbd;
	/*[DW5]*/
	u8 edcca_rpt_cnt:7;
	u8 rsvd_1_dummy_1bit:1;
	u8 edcca_rpt_curr_bw_max_l:1;
	u8 edcca_total_smp_cnt:7;
	u8 edcca_rpt_curr_bw_min_l:2;
	u8 edcca_rpt_curr_bw_max_m:6;
	u8 bw_idx:3; /*0~6: 5, 10, 20, 40, 80, 160, 80_80*/
	u8 edcca_rpt_curr_bw_min_m:5;
	/*[DW6]*/
	u8 ftm_t_off_l;
	u8 sigval_below_th_tone_cnt_seg0_l:4;
	u8 ftm_t_off_m:4;
	u8 cn_excess_th_tone_cnt_seg0_l:2;
	u8 sigval_below_th_tone_cnt_seg0_m:6;
	u8 cn_excess_th_tone_cnt_seg0_m;
	/*[DW7]*/
	u8 phy_sts_dw7_0;
	u8 phy_sts_dw7_1;
	u8 phy_sts_dw7_2;
	u8 phy_sts_dw7_3;
	/*[DW8]*/
	u8 phy_sts_dw8_0;
	u8 phy_sts_dw8_1;
	u8 phy_sts_dw8_2;
	u8 phy_sts_dw8_3;
	/*[DW9]*/
	u8 phy_sts_dw9_0;
	u8 phy_sts_dw9_1;
	u8 phy_sts_dw9_2;
	u8 phy_sts_dw9_3;
};

/*physts_ie_4_to_7_info*/
struct physts_7_ie_4_to_7_info {
	/*[DW0]*/
	u8 ant_idx:3;
	u8 ie_hdr:5;

	u8 sig_val_y;
	u8 rf_gain_idx;

	u8 snr_lgy:6;
	u8 tia_shrink_indicator:1;
	u8 rf_tia_gain_idx:1;

	/*[DW1]*/
	u8 evm_ss_y;

	u8 ant_idx_msb:1;
	u8 td_ant_weight:7;

	u8 dc_est_re;
	u8 dc_est_im;
};

 /*@--------------------------[Prptotype]-------------------------------------*/
#endif

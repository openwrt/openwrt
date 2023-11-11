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
#ifndef __HALBB_PSD_H__
#define __HALBB_PSD_H__

/*@--------------------------[Define] ---------------------------------------*/

/*@--------------------------[Enum]------------------------------------------*/
enum igi_lv_sel {
	IGI_DEFAULT		= 0,/*@default value*/
	IGI_LEVEL1		= 1,
	IGI_LEVEL2		= 2,
	IGI_LEVEL3		= 3,
	IGI_LEVEL4		= 4,
	IGI_LEVEL5		= 5,
	IGI_LEVEL6		= 6,
	IGI_LEVEL7		= 7
};
/*@--------------------------[Structure]-------------------------------------*/

struct bb_psd_cr_info {
	u32 psd_clk_en;
	u32 psd_clk_en_m;
	u32 psd_pw;
	u32 psd_pw_m;
	u32 psd_ok_flag;
	u32 psd_ok_flag_m;
	u32 psd_fft_idx;
	u32 psd_fft_idx_m;
	u32 psd_iq_sel;
	u32 psd_iq_sel_m;
	u32 psd_l_avg;
	u32 psd_l_avg_m;
	u32 psd_n_dft;
	u32 psd_n_dft_m;
	u32 psd_path_sel;
	u32 psd_path_sel_m;
	u32 psd_source_sel;
	u32 psd_source_sel_m;
	u32 psd_en;
	u32 psd_en_m;
	
};

struct bb_psd_info {
	struct bb_psd_cr_info bb_psd_cr_i;
	u8	psd_in_progress;
	u8	psd_pwr_common_offset;
	u8	hw_avg_time;
	u16	sw_avg_time;
	u16	fft_point;
	u16	fft_point_pre;
	u32	rf_0x18_bkp;
	u32	rf_0x18_bkp_b;
	u8	pri_ch;
	u8	fc_ch;
	u32	psd_bw;
	u32	psd_result[128];
	u8	i_q_setting;
	u8	path;
	u8	bw;
	u8	input;
	u8	noise_k_en;
	u8	lna_bkp;
	u8	lna_bkp_b;
	u8	tia_bkp;
	u8	tia_bkp_b;
	u8	rxbb_bkp;
	u8	rxbb_bkp_b;
	u8	igi_pin;
	u32	*rpt;
	enum band_type band;
};

struct bb_info;
/*@--------------------------[Prptotype]-------------------------------------*/
void halbb_psd_igi_lv(struct bb_info *bb, enum igi_lv_sel igi_lv);
u32 halbb_get_psd_data(struct bb_info *bb, u32 psd_tone_idx, u32 igi_pin);
void halbb_psd_para_setting(struct bb_info *bb, u8 sw_avg_time, u8 hw_avg_time,
			    u8 i_q_setting, u16 fft_point, u8 path,
			    u8 psd_input, u8 pri_ch,u8 channel, u8 bw, enum band_type band);
bool halbb_psd_alloc_buff(struct bb_info *bb);
void halbb_get_gain_index(struct bb_info *bb);
u8 halbb_psd(struct bb_info *bb, enum igi_lv_sel igi, u16 start_point,
	     u16 stop_point);
void halbb_psd_dbg(struct bb_info *bb, char input[][16], u32 *_used,
		   char *output, u32 *_out_len);
void halbb_psd_init(struct bb_info *bb);
void halbb_psd_deinit(struct bb_info *bb);
bool halbb_get_psd_result(struct bb_info *bb, u32 *psd_data, u16 *psd_len);
void halbb_cr_cfg_psd_init(struct bb_info *bb);
#endif


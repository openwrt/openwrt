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
#ifndef __HALBB_UL_TB_CTRL_H__
#define __HALBB_UL_TB_CTRL_H__

/*@--------------------------[Define] ---------------------------------------*/
#define		TF_CNT_L2H_TH	100
#define		TF_CNT_H2L_TH	70
/*@--------------------------[Enum]------------------------------------------*/

/*@--------------------------[Structure]-------------------------------------*/

struct bb_ul_tb_cr_info {
	u32 if_bandedge;
	u32 if_bandedge_m;
};

struct bb_ul_tb_info {
	struct bb_ul_tb_cr_info bb_ul_tb_cr_i;
	bool	dyn_tb_bedge_en;
	bool	dyn_tb_tri_en;
	u8	def_if_bandedge;
	u8	def_tri_idx;
	u32	tf_cnt_l2h_th;
	u32	tf_cnt_h2l_th;
};

struct bb_info;
/*@--------------------------[Prptotype]-------------------------------------*/
void halbb_ul_tb_reset(struct bb_info *bb);
void halbb_ul_tb_chk(struct bb_info *bb);
void halbb_ul_tb_ctrl(struct bb_info *bb);
void halbb_ul_tb_dbg(struct bb_info *bb, char input[][16], u32 *_used,
			      char *output, u32 *_out_len);
void halbb_ul_tb_ctrl_init(struct bb_info *bb);
void halbb_cr_cfg_ul_tb_init(struct bb_info *bb);
#endif


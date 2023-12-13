/******************************************************************************
 *
 * Copyright(c) 2007 - 2017  Realtek Corporation.
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
#ifndef __HALBB_DYN_CSI_RSP_H__
#define __HALBB_DYN_CSI_RSP_H__
#ifdef HALBB_DYN_CSI_RSP_SUPPORT

struct bf_ch_raw_info {
	/*result*/
	bool 				is_csi_rsp_en;
	/*state machine*/
	u8				dyn_csi_rsp_dbg_en;
	u8				dyn_csi_rsp_en;
	u8				ch_chk_cnt;
	/*set val*/
	u32				ch_est_dly;
	u32				get_phy_sts_dly;
	u32				max_est_tone_num;
	s32				cablelink_cnt_th;
	enum channel_width		dcr_bw;
};

struct bb_info;

/*@--------------------------[Enum]------------------------------------------*/
enum dcr_csi_rsp {
	dcr_csi_rsp_dis	= 0,	/*Disale*/
	dcr_csi_rsp_en	= 1,	/*Enable*/
};
void halbb_dcr_init(struct bb_info *bb);
void halbb_dcr_reset(struct bb_info *bb);
void halbb_dyn_csi_rsp_dbg(struct bb_info *bb, char input[][16],
			   u32 *_used, char *output, u32 *_out_len);
void halbb_dyn_csi_rsp_main(struct bb_info *bb);
#endif
#endif

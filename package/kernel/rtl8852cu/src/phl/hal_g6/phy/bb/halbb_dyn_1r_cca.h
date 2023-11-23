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
#ifndef __HALBB_DYN_1R_CCA_H__
#define __HALBB_DYN_1R_CCA_H__
#ifdef HALBB_DYN_1R_CCA_SUPPORT

struct bb_dyn_1r_cca_info {
	bool			dyn_1r_cca_en;
	enum rf_path		dyn_1r_cca_cfg;
	u16			dyn_1r_cca_rssi_diff_th; /*RSSI  u(16,5)*/
	u16			dyn_1r_cca_rssi_min_th; /*RSSI  u(16,5)*/
};


void halbb_dyn_1r_cca_rst(struct bb_info *bb);
void halbb_dyn_1r_cca_dbg(struct bb_info *bb, char input[][16], 
			  u32 *_used, char *output, u32 *_out_len);
void halbb_dyn_1r_cca(struct bb_info *bb);
void halbb_dyn_1r_cca_init(struct bb_info *bb);



#endif
#endif

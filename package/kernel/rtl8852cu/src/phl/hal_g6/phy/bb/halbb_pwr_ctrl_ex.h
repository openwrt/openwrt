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
#ifndef __HALBB_PWR_CTRL_EX_H__
#define __HALBB_PWR_CTRL_EX_H__

/*@--------------------------[Enum]------------------------------------------*/
enum tssi_bandedge_cfg {
	TSSI_BANDEDGE_FLAT = 0,
	TSSI_BANDEDGE_LOW  = 1,
	TSSI_BANDEDGE_MID  = 2,
	TSSI_BANDEDGE_HIGH = 3
};
/*@--------------------------[Define] ---------------------------------------*/

bool halbb_set_pwr_ul_tb_ofst(struct bb_info *bb, s16 pw_ofst,
			      enum phl_phy_idx phy_idx);
void halbb_pwr_ctrl_txpwr_cfg(struct bb_info *bb, s8 pwr_lv1, s8 pwr_lv2, s8 pwr_lv3);
void halbb_pwr_ctrl_th_cfg(struct bb_info *bb, u8 th_lv1, u8 th_lv2, u8 th_lv3);
void halbb_tssi_ctrl_set_bandedge_cfg(struct bb_info *bb, enum phl_phy_idx phy_idx,
						enum tssi_bandedge_cfg bandedge_cfg);
#endif
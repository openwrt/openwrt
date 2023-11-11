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
#ifndef __HALBB_DBCC_EX_H__
#define __HALBB_DBCC_EX_H__

/*@--------------------------[Define] ---------------------------------------*/

/*@--------------------------[Enum]------------------------------------------*/

/*@--------------------------[Structure]-------------------------------------*/
struct bb_dbcc_cfg_info {
	bool dbcc_en;
	enum phl_phy_idx cck_phy_map;
};

/*@--------------------------[Prptotype]-------------------------------------*/
struct bb_info;
void halbb_dbcc_band_switch_notify(struct bb_info *bb);
bool halbb_ctrl_dbcc(struct bb_info *bb, bool dbcc_enable);
bool halbb_cfg_dbcc_cck_phy_map(struct bb_info *bb, enum phl_phy_idx cck_phy_map);
bool halbb_cfg_dbcc(struct bb_info *bb, struct bb_dbcc_cfg_info *cfg);
#endif

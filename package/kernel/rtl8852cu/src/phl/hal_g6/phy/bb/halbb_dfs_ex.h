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
#ifndef __HALBB_DFS_EX_H__
#define __HALBB_DFS_EX_H__
/*@--------------------------[Define] ---------------------------------------*/
/*@--------------------------[Enum]------------------------------------------*/
/*@--------------------------[Structure]-------------------------------------*/
struct bb_info;
struct hal_dfs_rpt;
/*@--------------------------[Prptotype]-------------------------------------*/
void halbb_dfs_init(struct bb_info *bb);
void halbb_radar_detect_reset(struct bb_info *bb);
void halbb_radar_detect_disable(struct bb_info *bb);
void halbb_radar_detect_enable(struct bb_info *bb);
void halbb_dfs_enable_cac_flag(struct bb_info* bb);
void halbb_dfs_disable_cac_flag(struct bb_info* bb);
void halbb_set_dfs_st_l2h(struct bb_info* bb, bool mask_dfs_en);
void halbb_dfs_change_dmn(struct bb_info *bb, u8 ch, u8 bw);
bool halbb_is_dfs_band(struct bb_info *bb, u8 ch, u8 bw);
bool halbb_radar_detect(struct bb_info *bb, struct hal_dfs_rpt *rpt);
#endif
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
#ifndef __HALBB_INTERFACE_EX_H__
#define __HALBB_INTERFACE_EX_H__

/*@--------------------------[Define] ---------------------------------------*/

/*@--------------------------[Enum]------------------------------------------*/

/*@--------------------------[Structure]-------------------------------------*/
 
/*@--------------------------[Prptotype]-------------------------------------*/
struct bb_info;
void halbb_set_reg(struct bb_info *bb, u32 addr, u32 mask, u32 val);
u32 halbb_get_reg(struct bb_info *bb, u32 addr, u32 bit_mask);
void halbb_set_reg_cmn(struct bb_info *bb, u32 addr, u32 mask, u32 val, enum phl_phy_idx phy_idx);
u32 halbb_get_reg_cmn(struct bb_info *bb, u32 addr, u32 mask, enum phl_phy_idx phy_idx);
u32 rtw_halbb_c2h_parsing(struct bb_info *bb, u8 classid, u8 cmdid, u16 len, u8 *c2h);
u8 halbb_set_cmac_txpwr_mode(struct bb_info *bb, struct rtw_phl_stainfo_t *phl_sta_i);
u8 halbb_set_cmac_ntx_en(struct bb_info *bb, struct rtw_phl_stainfo_t *phl_sta_i);
u8 halbb_set_cmac_path_map_a(struct bb_info *bb, struct rtw_phl_stainfo_t *phl_sta_i);
u8 halbb_set_cmac_path_map_b(struct bb_info *bb, struct rtw_phl_stainfo_t *phl_sta_i);
u8 halbb_set_cmac_path_map_c(struct bb_info *bb, struct rtw_phl_stainfo_t *phl_sta_i);
u8 halbb_set_cmac_path_map_d(struct bb_info *bb, struct rtw_phl_stainfo_t *phl_sta_i);
u8 halbb_set_cmac_antsel_a(struct bb_info *bb, struct rtw_phl_stainfo_t *phl_sta_i);
u8 halbb_set_cmac_antsel_b(struct bb_info *bb, struct rtw_phl_stainfo_t *phl_sta_i);
u8 halbb_set_cmac_antsel_c(struct bb_info *bb, struct rtw_phl_stainfo_t *phl_sta_i);
u8 halbb_set_cmac_antsel_d(struct bb_info *bb, struct rtw_phl_stainfo_t *phl_sta_i);
u8 halbb_set_cmac_pwr_tol(struct bb_info *bb, struct rtw_phl_stainfo_t *phl_sta_i);
u8 halbb_set_cmac_databw_er(struct bb_info *bb, struct rtw_phl_stainfo_t *phl_sta_i);
#endif

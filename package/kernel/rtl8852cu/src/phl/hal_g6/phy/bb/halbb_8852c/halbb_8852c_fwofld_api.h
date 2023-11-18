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
#ifndef __HALBB_8852C_FWOFLD_API_H__
#define __HALBB_8852C_FWOFLD_API_H__
#ifdef BB_8852C_SUPPORT
#ifdef HALBB_CONFIG_RUN_IN_DRV
#include "../halbb_api.h"
#include "../halbb_fwofld_ex.h"
#endif


/*@--------------------------[Define] ---------------------------------------*/

/*@--------------------------[Enum]------------------------------------------*/

/*@--------------------------[Structure]-------------------------------------*/


struct bb_info;
/*@--------------------------[Prptotype]-------------------------------------*/
#ifdef HALBB_FW_OFLD_SUPPORT
bool halbb_fwcfg_bb_phy_8852c(struct bb_info *bb, u32 addr, u32 data,
			    enum phl_phy_idx phy_idx);
bool halbb_fwofld_ctrl_ch_8852c(struct bb_info *bb, u8 central_ch, enum band_type band,
			 enum phl_phy_idx phy_idx);
bool halbb_fwofld_ctrl_bw_ch_8852c(struct bb_info *bb, u8 pri_ch, u8 central_ch,
			    enum channel_width bw, enum band_type band,
			    enum phl_phy_idx phy_idx);
void halbb_fwofld_ctrl_cck_en_8852c(struct bb_info *bb, bool cck_en,
			     enum phl_phy_idx phy_idx);
void halbb_fwofld_set_hidden_efuse_8852c(struct bb_info *bb, u8 central_ch, enum band_type band_type, enum rf_path path);
void halbb_fwofld_set_normal_efuse_8852c(struct bb_info *bb, u8 central_ch, enum band_type band_type, enum rf_path path);
void halbb_fwofld_set_gain_error_8852c(struct bb_info *bb, u8 central_ch,
				       enum band_type band_type,
				       enum rf_path path);
void halbb_fwofld_set_gain_cr_init_8852c(struct bb_info *bb);
void halbb_fwofld_set_rxsc_rpl_comp_8852c(struct bb_info *bb, u8 central_ch);
#endif
#endif
#endif
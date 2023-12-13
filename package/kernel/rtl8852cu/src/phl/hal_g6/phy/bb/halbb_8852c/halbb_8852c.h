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
#ifndef __HALBB_8852C_H__
#define __HALBB_8852C_H__
#ifdef BB_8852C_SUPPORT

#include "../halbb_pmac_setting_ex.h"

struct bb_info;
bool halbb_chk_pkg_valid_8852c(struct bb_info *bb, u8 bb_ver, u8 rf_ver);
void halbb_set_pmac_tx_8852c(struct bb_info *bb, struct halbb_pmac_info *tx_info,
			     enum phl_phy_idx phy_idx);
void halbb_set_tmac_tx_8852c(struct bb_info *bb, enum phl_phy_idx phy_idx);
void halbb_dpd_bypass_8852c(struct bb_info *bb, bool pdp_bypass,
			      enum phl_phy_idx phy_idx);
void halbb_ic_hw_setting_init_8852c(struct bb_info *bb);
void halbb_ic_hw_setting_8852c(struct bb_info *bb);

bool halbb_set_pd_lower_bound_8852c(struct bb_info *bb, u8 bound,
				      enum channel_width bw,
				      enum phl_phy_idx phy_idx);
bool halbb_set_pd_lower_bound_cck_8852c(struct bb_info *bb, u8 bound,
				      enum channel_width bw,
				      enum phl_phy_idx phy_idx);
u8 halbb_querry_pd_lower_bound_8852c(struct bb_info *bb, bool get_en_info,
				       enum phl_phy_idx phy_idx);
void halbb_pop_en_8852c(struct bb_info *bb, bool en, enum phl_phy_idx phy_idx);
bool halbb_querry_pop_en_8852c(struct bb_info *bb, enum phl_phy_idx phy_idx);
u16 halbb_get_per_8852c(struct bb_info *bb, enum phl_phy_idx phy_idx);
u8 halbb_get_losel_8852c(struct bb_info *bb);
bool halbb_chk_tx_idle_8852c(struct bb_info *bb, enum phl_phy_idx phy_idx);

#endif
#endif /*  __HALBB_8852C_H__ */

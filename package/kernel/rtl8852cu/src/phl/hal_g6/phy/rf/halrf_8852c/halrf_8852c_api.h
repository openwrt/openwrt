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
#ifndef __HALRF_8852C_API_H__
#define __HALRF_8852C_API_H__
#ifdef RF_8852C_SUPPORT

u8 halrf_get_thermal_8852c(struct rf_info *rf, enum rf_path rf_path);

bool halrf_set_dbcc_8852c(struct rf_info *rf, bool dbcc_en);

u32 halrf_mac_get_pwr_reg_8852c(struct rf_info *rf, enum phl_phy_idx phy,
	u32 addr, u32 mask);

u32 halrf_mac_set_pwr_reg_8852c(struct rf_info *rf, enum phl_phy_idx phy,
	u32 addr, u32 mask, u32 val);

void halrf_wlan_tx_power_control_8852c(struct rf_info *rf,
	enum phl_phy_idx phy, enum phl_pwr_ctrl pwr_ctrl_idx,
	u32 tx_power_val, bool enable);

bool halrf_wl_tx_power_control_8852c(struct rf_info *rf, u32 tx_power_val);

s8 halrf_get_ther_protected_threshold_8852c(struct rf_info *rf);

s8 halrf_xtal_tracking_offset_8852c(struct rf_info *rf, enum phl_phy_idx phy);

void halrf_syn1_onoff_8852c(struct rf_info *rf, enum phl_phy_idx phy, u8 path, bool syn1_turn_on);

void halrf_txck_force_8852c(struct rf_info *rf, enum rf_path path, bool force, enum dac_ck ck);

void halrf_rxck_force_8852c(struct rf_info *rf, enum rf_path path, bool force, enum adc_ck ck);

void halrf_set_gpio_8852c(struct rf_info *rf, enum phl_phy_idx phy);

void do_bkup_kip_8852c(struct rf_info *rf, u8 path);
void do_bkup_bb_8852c(struct rf_info *rf);
void do_bkup_rf_8852c(struct rf_info *rf, u8 path);
void do_reload_kip_8852c(struct rf_info *rf, u8 path);
void do_reload_bb_8852c(struct rf_info *rf);
void do_reload_rf_8852c(struct rf_info *rf, u8 path);
void halrf_set_regulation_from_driver_8852c(struct rf_info *rf, u8 regulation_idx);
void halrf_adie_pow_ctrl_8852c(struct rf_info *rf, bool rf_off, bool others_off);
void halrf_afe_pow_ctrl_8852c(struct rf_info *rf, bool adda_off, bool pll_off);
#endif
#endif /*  __INC_PHYDM_API_H_8852C__ */

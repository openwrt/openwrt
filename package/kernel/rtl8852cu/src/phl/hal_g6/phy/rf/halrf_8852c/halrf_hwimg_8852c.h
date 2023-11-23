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
#ifndef _HALRF_HW_IMG_8852C_H_
#define _HALRF_HW_IMG_8852C_H_
#ifdef RF_8852C_SUPPORT

#define DONT_CARE_8852C	0xff

#define IF_8852C	0x8
#define ELSE_IF_8852C	0x9
#define ELSE_8852C	0xa
#define END_8852C	0xb
#define CHK_8852C	0x4

#if 0
#define RADIO_TO_FW_PAGE_SIZE 6
#define RADIO_TO_FW_DATA_SIZE 500

#define FWCMD_H2C_RADIO_A_INIT_0 0x0
#define FWCMD_H2C_RADIO_A_INIT_1 0x1
#define FWCMD_H2C_RADIO_A_INIT_2 0x2
#define FWCMD_H2C_RADIO_A_INIT_3 0x3
#define FWCMD_H2C_RADIO_A_INIT_4 0x4
#define FWCMD_H2C_RADIO_A_INIT_5 0x5
#define FWCMD_H2C_RADIO_A_INIT_6 0x6
#define FWCMD_H2C_RADIO_A_INIT_7 0x7
#define FWCMD_H2C_RADIO_A_INIT_8 0x8
#define FWCMD_H2C_RADIO_A_INIT_9 0x9

#define FWCMD_H2C_RADIO_B_INIT_0 0x0
#define FWCMD_H2C_RADIO_B_INIT_1 0x1
#define FWCMD_H2C_RADIO_B_INIT_2 0x2
#define FWCMD_H2C_RADIO_B_INIT_3 0x3
#define FWCMD_H2C_RADIO_B_INIT_4 0x4
#define FWCMD_H2C_RADIO_B_INIT_5 0x5
#define FWCMD_H2C_RADIO_B_INIT_6 0x6
#define FWCMD_H2C_RADIO_B_INIT_7 0x7
#define FWCMD_H2C_RADIO_B_INIT_8 0x8
#define FWCMD_H2C_RADIO_B_INIT_9 0x9

struct halrf_radio_info {
	u32 write_times_a;
	u32 write_times_b;
	u32 radio_a_parameter[RADIO_TO_FW_PAGE_SIZE][RADIO_TO_FW_DATA_SIZE];
	u32 radio_b_parameter[RADIO_TO_FW_PAGE_SIZE][RADIO_TO_FW_DATA_SIZE];
};

#endif
void halrf_config_8852c_nctl_reg(struct rf_info *rf);
void halrf_config_8852c_radio_a_reg(struct rf_info *rf, enum phl_phy_idx phy);
void halrf_config_8852c_radio_b_reg(struct rf_info *rf, enum phl_phy_idx phy);
void halrf_config_8852c_store_power_by_rate(struct rf_info *rf, enum phl_phy_idx phy);
void halrf_config_8852c_store_power_limit(struct rf_info *rf, enum phl_phy_idx phy);
void halrf_config_8852c_store_power_limit_6g(struct rf_info *rf, enum phl_phy_idx phy);
void halrf_config_8852c_store_power_limit_ru(struct rf_info *rf, enum phl_phy_idx phy);
void halrf_config_8852c_store_power_limit_ru_6g(struct rf_info *rf, enum phl_phy_idx phy);
void halrf_config_8852c_store_pwr_track(struct rf_info *rf, enum phl_phy_idx phy);
void halrf_config_8852c_store_xtal_track(struct rf_info *rf, enum phl_phy_idx phy);

u32 halrf_get_8852c_nctl_reg_ver(void);
u32 halrf_get_8852c_radio_reg_ver(void);

void halrf_config_8852c_radio_to_fw(struct rf_info *rf);

void halrf_cfg_8852c_radio_b_w_bt_status(struct rf_info *rf, bool bt_connect);


#endif
#endif /*  _HALRF_HW_IMG_8852C_H_ */

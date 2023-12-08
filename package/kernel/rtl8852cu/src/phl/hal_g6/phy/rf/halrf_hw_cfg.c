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
#include "halrf_precomp.h"

bool halrf_init_reg_by_hdr(void *rf_void)
{
	bool result = true;

	return result;
}

bool halrf_nctl_init_reg_by_hdr(void *rf_void)
{
	struct rf_info *rf = (struct rf_info *)rf_void;

	bool result = true;
#ifdef RF_8852A_SUPPORT
	if (rf->ic_type == RF_RTL8852A)	{
		halrf_config_8852a_nctl_reg(rf);
	}
#endif
#ifdef RF_8852B_SUPPORT
	if (rf->ic_type == RF_RTL8852B) {
		halrf_config_8852b_nctl_reg(rf);
	}
#endif
#ifdef RF_8832BR_SUPPORT
	if (rf->ic_type == RF_RTL8832BR) {
		halrf_config_8832br_nctl_reg(rf);
	}
#endif
#ifdef RF_8192XB_SUPPORT
	if (rf->ic_type == RF_RTL8192XB) {
		halrf_config_8192xb_nctl_reg(rf);
	}
#endif
#ifdef RF_8852BP_SUPPORT
	if (rf->ic_type == RF_RTL8852BP) {
		halrf_config_8852bp_nctl_reg(rf);
	}
#endif
#ifdef RF_8730A_SUPPORT
	if (rf->ic_type == RF_RTL8730A) {
		halrf_config_8730a_nctl_reg(rf);
	}
#endif

#ifdef RF_8851B_SUPPORT
	if (rf->ic_type == RF_RTL8851B) {
		halrf_config_8851b_nctl_reg(rf);
	}
#endif

	return result;
}

bool halrf_config_radio_a_reg(void *rf_void, bool is_form_folder,
			   u32 folder_len, u32 *folder_array)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_com = rf->hal_com;
	bool result = true;

#ifdef RF_8852A_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852A) {
		halrf_config_8852a_radio_a_reg(rf, 0);
	}
#endif

#ifdef RF_8852B_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852B) {
		halrf_config_8852b_radio_a_reg(rf, 0);
	}
#endif

#ifdef RF_8852C_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852C) {
		halrf_config_8852c_radio_a_reg(rf, 0);
	}
#endif

#ifdef RF_8832BR_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8832BR) {
		halrf_config_8832br_radio_a_reg(rf, 0);
	}
#endif

#ifdef RF_8192XB_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8192XB) {
		halrf_config_8192xb_radio_a_reg(rf, 0);
	}
#endif

#ifdef RF_8852BP_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852BP) {
		halrf_config_8852bp_radio_a_reg(rf, 0);
	}
#endif
#ifdef RF_8730A_SUPPORT
	if (rf->ic_type == RF_RTL8730A) {
		halrf_config_8730a_radio_a_reg(rf, 0);
	}
#endif

#ifdef RF_8851B_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8851B) {
		halrf_config_8851b_radio_a_reg(rf, 0);
	}
#endif

	return result;
}

bool halrf_config_radio_b_reg(void *rf_void, bool is_form_folder,
			   u32 folder_len, u32 *folder_array)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_com = rf->hal_com;
	bool result = true;

#ifdef RF_8852A_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852A) {
		halrf_config_8852a_radio_b_reg(rf, 0);
	}
#endif

#ifdef RF_8852B_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852B) {
		halrf_config_8852b_radio_b_reg(rf, 0);
	}
#endif

#ifdef RF_8852C_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852C) {
		halrf_config_8852c_radio_b_reg(rf, 0);
	}
#endif

#ifdef RF_8832BR_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8832BR) {
		halrf_config_8832br_radio_b_reg(rf, 0);
	}
#endif
	
#ifdef RF_8192XB_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8192XB) {
		halrf_config_8192xb_radio_b_reg(rf, 0);
	}
#endif

#ifdef RF_8852BP_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852BP) {
		halrf_config_8852bp_radio_b_reg(rf, 0);
	}
#endif

	return result;
}

bool halrf_config_store_power_by_rate(void *rf_void,
		bool is_form_folder, u32 folder_len, u32 *folder_array)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_com = rf->hal_com;
	bool result = true;

#ifdef RF_8852A_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852A) {
		halrf_config_8852a_store_power_by_rate(rf, 0);
	}
#endif

#ifdef RF_8852B_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852B) {
		halrf_config_8852b_store_power_by_rate(rf, 0);
	}
#endif

#ifdef RF_8852C_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852C) {
		halrf_config_8852c_store_power_by_rate(rf, 0);
	}
#endif

#ifdef RF_8832BR_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8832BR) {
		halrf_config_8832br_store_power_by_rate(rf, 0);
	}
#endif

#ifdef RF_8192XB_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8192XB) {
		halrf_config_8192xb_store_power_by_rate(rf, 0);
	}
#endif

#ifdef RF_8852BP_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852BP) {
		halrf_config_8852bp_store_power_by_rate(rf, 0);
	}
#endif
#ifdef RF_8730A_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8730A) {
		halrf_config_8730a_store_power_by_rate(rf, 0);
	}
#endif

#ifdef RF_8851B_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8851B) {
		halrf_config_8851b_store_power_by_rate(rf, 0);
	}
#endif

	return result;
}

bool halrf_config_store_power_limit(void *rf_void,
		bool is_form_folder, u32 folder_len, u32 *folder_array)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_com = rf->hal_com;
	struct halrf_pwr_info *pwr = &rf->pwr_info;
	bool result = true;

	pwr->regulation[PW_LMT_BAND_2_4G][PW_LMT_REGU_NA] = true;
	pwr->regulation[PW_LMT_BAND_5G][PW_LMT_REGU_NA] = true;
	pwr->regulation[PW_LMT_BAND_6G][PW_LMT_REGU_NA] = true;

#ifdef RF_8852A_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852A) {
		halrf_config_8852a_store_power_limit(rf, 0);
	}
#endif

#ifdef RF_8852B_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852B) {
		halrf_config_8852b_store_power_limit(rf, 0);
	}
#endif

#ifdef RF_8852C_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852C) {
		halrf_config_8852c_store_power_limit(rf, 0);
		halrf_config_8852c_store_power_limit_6g(rf, 0);
	}
#endif

#ifdef RF_8832BR_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8832BR) {
		halrf_config_8832br_store_power_limit(rf, 0);
	}
#endif

#ifdef RF_8192XB_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8192XB) {
		halrf_config_8192xb_store_power_limit(rf, 0);
	}
#endif

#ifdef RF_8852BP_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852BP) {
		halrf_config_8852bp_store_power_limit(rf, 0);
	}
#endif
#ifdef RF_8730A_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8730A) {
		halrf_config_8730a_store_power_limit(rf, 0);
	}
#endif


#ifdef RF_8851B_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8851B) {
		halrf_config_8851b_store_power_limit(rf, 0);
	}
#endif

	return result;
}

bool halrf_config_store_power_limit_ru(void *rf_void,
		bool is_form_folder, u32 folder_len, u32 *folder_array)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_com = rf->hal_com;
	bool result = true;

#ifdef RF_8852A_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852A) {
		halrf_config_8852a_store_power_limit_ru(rf, 0);
	}
#endif

#ifdef RF_8852B_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852B) {
		halrf_config_8852b_store_power_limit_ru(rf, 0);
	}
#endif

#ifdef RF_8852C_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852C) {
		halrf_config_8852c_store_power_limit_ru(rf, 0);
		halrf_config_8852c_store_power_limit_ru_6g(rf, 0);
	}
#endif

#ifdef RF_8832BR_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8832BR) {
		halrf_config_8832br_store_power_limit_ru(rf, 0);
	}
#endif

#ifdef RF_8192XB_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8192XB) {
		halrf_config_8192xb_store_power_limit_ru(rf, 0);
	}
#endif

#ifdef RF_8852BP_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852BP) {
		halrf_config_8852bp_store_power_limit_ru(rf, 0);
	}
#endif
#ifdef RF_8730A_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8730A) {
		halrf_config_8730a_store_power_limit_ru(rf, 0);
	}
#endif

#ifdef RF_8851B_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8851B) {
		halrf_config_8851b_store_power_limit_ru(rf, 0);
	}
#endif

	return result;
}

bool halrf_config_store_power_track(void *rf_void,
		bool is_form_folder, u32 folder_len, u32 *folder_array)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_com = rf->hal_com;
	bool result = true;

#ifdef RF_8852A_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852A) {
		halrf_config_8852a_store_pwr_track(rf, 0);
	}
#endif

#ifdef RF_8852B_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852B) {
		halrf_config_8852b_store_pwr_track(rf, 0);
	}
#endif

#ifdef RF_8852C_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852C) {
		halrf_config_8852c_store_pwr_track(rf, 0);
	}
#endif

#ifdef RF_8832BR_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8832BR) {
		halrf_config_8832br_store_pwr_track(rf, 0);
	}
#endif

#ifdef RF_8192XB_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8192XB) {
		halrf_config_8192xb_store_pwr_track(rf, 0);
	}
#endif

#ifdef RF_8852BP_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852BP) {
		halrf_config_8852bp_store_pwr_track(rf, 0);
	}
#endif
#ifdef RF_8730A_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8730A) {
		halrf_config_8730a_store_pwr_track(rf, 0);
	}
#endif

#ifdef RF_8851B_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8851B) {
		halrf_config_8851b_store_pwr_track(rf, 0);
	}
#endif

	return result;
}

bool halrf_config_store_xtal_track(void *rf_void,
		bool is_form_folder, u32 folder_len, u32 *folder_array)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_com = rf->hal_com;
	bool result = true;

#ifdef RF_8852A_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852A) {
		halrf_config_8852a_store_xtal_track(rf, 0);
	}
#endif

#ifdef RF_8852B_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852B) {
		halrf_config_8852b_store_xtal_track(rf, 0);
	}
#endif

#ifdef RF_8852C_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852C) {
		halrf_config_8852c_store_xtal_track(rf, 0);
	}
#endif

#ifdef RF_8852BP_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852BP) {
		halrf_config_8852bp_store_xtal_track(rf, 0);
	}
#endif
#ifdef RF_8730A_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8730A) {
		halrf_config_8730a_store_xtal_track(rf, 0);
	}
#endif

#ifdef RF_8851B_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8851B) {
		halrf_config_8851b_store_xtal_track(rf, 0);
	}
#endif

	return result;
}

bool halrf_config_radio(void *rf_void, enum phl_phy_idx phy)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_com = rf->hal_com;
	bool result = true;

#ifdef RF_8852A_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852A) {
		halrf_config_8852a_radio_a_reg(rf, phy);
		halrf_config_8852a_radio_b_reg(rf, phy);
	}
#endif

#ifdef RF_8852B_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852B) {
		halrf_config_8852b_radio_a_reg(rf, phy);
		halrf_config_8852b_radio_b_reg(rf, phy);
	}
#endif

#ifdef RF_8852C_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852C) {
		halrf_config_8852c_radio_b_reg(rf, phy);
		halrf_config_8852c_radio_a_reg(rf, phy);
	}
#endif

#ifdef RF_8832BR_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8832BR) {
		halrf_config_8832br_radio_a_reg(rf, phy);
		halrf_config_8832br_radio_b_reg(rf, phy);
	}
#endif

#ifdef RF_8192XB_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8192XB) {
		halrf_config_8192xb_radio_a_reg(rf, phy);
		halrf_config_8192xb_radio_b_reg(rf, phy);
	}
#endif

#ifdef RF_8852BP_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852BP) {
		halrf_config_8852bp_radio_b_reg(rf, phy);
		halrf_config_8852bp_radio_a_reg(rf, phy);
	}
#endif
#ifdef RF_8730A_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8730A) {
		halrf_config_8730a_radio_a_reg(rf, phy);
	}
#endif

#ifdef RF_8851B_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8851B) {
		halrf_config_8851b_radio_a_reg(rf, phy);
	}
#endif

	return result;
}

bool halrf_config_power_by_rate(void *rf_void, enum phl_phy_idx phy)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_com = rf->hal_com;
	bool result = true;

#ifdef RF_8852A_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852A) {
		halrf_config_8852a_store_power_by_rate(rf, phy);
	}
#endif

#ifdef RF_8852B_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852B) {
		halrf_config_8852b_store_power_by_rate(rf, phy);
	}
#endif

#ifdef RF_8852C_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852C) {
		halrf_config_8852c_store_power_by_rate(rf, phy);
	}
#endif

#ifdef RF_8832BR_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8832BR) {
		halrf_config_8832br_store_power_by_rate(rf, phy);
	}
#endif

#ifdef RF_8192XB_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8192XB) {
		halrf_config_8192xb_store_power_by_rate(rf, phy);
	}
#endif

#ifdef RF_8852BP_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852BP) {
		halrf_config_8852bp_store_power_by_rate(rf, phy);
	}
#endif
#ifdef RF_8730A_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8730A) {
		halrf_config_8730a_store_power_by_rate(rf, phy);
	}
#endif

#ifdef RF_8851B_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8851B) {
		halrf_config_8851b_store_power_by_rate(rf, phy);
	}
#endif

	return result;
}

bool halrf_config_power_limit(void *rf_void, enum phl_phy_idx phy)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_com = rf->hal_com;
	bool result = true;

#ifdef RF_8852A_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852A) {
		halrf_config_8852a_store_power_limit(rf, phy);
	}
#endif

#ifdef RF_8852B_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852B) {
		halrf_config_8852b_store_power_limit(rf, phy);
	}
#endif

#ifdef RF_8852C_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852C) {
		halrf_config_8852c_store_power_limit(rf, phy);
		halrf_config_8852c_store_power_limit_6g(rf, phy);
	}
#endif

#ifdef RF_8832BR_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8832BR) {
		halrf_config_8832br_store_power_limit(rf, phy);
	}
#endif

#ifdef RF_8192XB_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8192XB) {
		halrf_config_8192xb_store_power_limit(rf, phy);
	}
#endif

#ifdef RF_8852BP_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852BP) {
		halrf_config_8852bp_store_power_limit(rf, phy);
		halrf_config_8852bp_store_power_limit_6g(rf, phy);
	}
#endif
#ifdef RF_8730A_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8730A) {
		halrf_config_8730a_store_power_limit(rf, phy);
	}
#endif

#ifdef RF_8851B_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8851B) {
		halrf_config_8851b_store_power_limit(rf, phy);
	}
#endif

	return result;
}

bool halrf_config_power_limit_6g(void *rf_void, enum phl_phy_idx phy)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	bool result = true;

	switch (rf->ic_type) {

#ifdef RF_8852C_SUPPORT
	case RF_RTL8852C:
		halrf_config_8852c_store_power_limit_6g(rf, phy);
		break;
#endif

#ifdef RF_8852BP_SUPPORT
	case RF_RTL8852BP:
		halrf_config_8852bp_store_power_limit_6g(rf, phy);
		break;
#endif

#ifdef RF_8832CRVU_SUPPORT
	case RF_RTL8832CR_VU:
		halrf_config_8832crvu_store_power_limit_6g(rf, phy);
		break;
#endif

	default:
		break;
	}

	return result;
}

bool halrf_config_power_limit_ru(void *rf_void, enum phl_phy_idx phy)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_com = rf->hal_com;
	bool result = true;

#ifdef RF_8852A_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852A) {
		halrf_config_8852a_store_power_limit_ru(rf, phy);
	}
#endif

#ifdef RF_8852B_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852B) {
		halrf_config_8852b_store_power_limit_ru(rf, phy);
	}
#endif

#ifdef RF_8852C_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852C) {
		halrf_config_8852c_store_power_limit_ru(rf, phy);
		halrf_config_8852c_store_power_limit_ru_6g(rf, phy);
	}
#endif

#ifdef RF_8832BR_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8832BR) {
		halrf_config_8832br_store_power_limit_ru(rf, phy);
	}
#endif

#ifdef RF_8192XB_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8192XB) {
		halrf_config_8192xb_store_power_limit_ru(rf, phy);
	}
#endif

#ifdef RF_8852BP_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852BP) {
		halrf_config_8852bp_store_power_limit_ru(rf, phy);
		halrf_config_8852bp_store_power_limit_ru_6g(rf, phy);
	}
#endif
#ifdef RF_8730A_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8730A) {
		halrf_config_8730a_store_power_limit_ru(rf, phy);
	}
#endif

#ifdef RF_8851B_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8851B) {
		halrf_config_8851b_store_power_limit_ru(rf, phy);
	}
#endif

	return result;
}

bool halrf_config_power_limit_ru_6g(void *rf_void, enum phl_phy_idx phy)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	bool result = true;

	switch (rf->ic_type) {
#ifdef RF_8852C_SUPPORT
	case RF_RTL8852C:
		halrf_config_8852c_store_power_limit_ru_6g(rf, phy);
		break;
#endif

#ifdef RF_8852BP_SUPPORT
	case RF_RTL8852BP:
		halrf_config_8852bp_store_power_limit_ru_6g(rf, phy);
		break;
#endif

#ifdef RF_8832CRVU_SUPPORT
	case RF_RTL8832CR_VU:
		halrf_config_8832crvu_store_power_limit_ru_6g(rf, phy);
		break;
#endif

	default:
		break;
	}

	return result;
}

bool halrf_config_power_track(void *rf_void, enum phl_phy_idx phy)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_com = rf->hal_com;
	bool result = true;

#ifdef RF_8852A_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852A) {
		halrf_config_8852a_store_pwr_track(rf, phy);
	}
#endif

#ifdef RF_8852B_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852B) {
		halrf_config_8852b_store_pwr_track(rf, phy);
	}
#endif

#ifdef RF_8852C_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852C) {
		halrf_config_8852c_store_pwr_track(rf, phy);
	}
#endif

#ifdef RF_8832BR_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8832BR) {
		halrf_config_8832br_store_pwr_track(rf, phy);
	}
#endif

#ifdef RF_8192XB_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8192XB) {
		halrf_config_8192xb_store_pwr_track(rf, phy);
	}
#endif

#ifdef RF_8852BP_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852BP) {
		halrf_config_8852bp_store_pwr_track(rf, phy);
	}
#endif
#ifdef RF_8730A_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8730A) {
		halrf_config_8730a_store_pwr_track(rf, phy);
	}
#endif

#ifdef RF_8851B_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8851B) {
		halrf_config_8851b_store_pwr_track(rf, phy);
	}
#endif

	return result;
}

bool halrf_config_xtal_track(void *rf_void, enum phl_phy_idx phy)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_com = rf->hal_com;
	bool result = true;

#ifdef RF_8852A_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852A) {
		halrf_config_8852a_store_xtal_track(rf, phy);
	}
#endif

#ifdef RF_8852B_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852B) {
		halrf_config_8852b_store_xtal_track(rf, phy);
	}
#endif

#ifdef RF_8852C_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852C) {
		halrf_config_8852c_store_xtal_track(rf, phy);
	}
#endif

#ifdef RF_8852BP_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852BP) {
		halrf_config_8852bp_store_xtal_track(rf, phy);
	}
#endif
#ifdef RF_8730A_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8730A) {
		halrf_config_8730a_store_xtal_track(rf, phy);
	}
#endif

#ifdef RF_8851B_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8851B) {
		halrf_config_8851b_store_xtal_track(rf, phy);
	}
#endif

	return result;
}

void halrf_config_limit_default(void *rf_void)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct halrf_pwr_info *pwr = &rf->pwr_info;
	u32 reg, bw, rate, bf, ch, tx_num;

	for (tx_num = 0; tx_num < MAX_HALRF_PATH; tx_num++) {
		for (ch = 0; ch < PW_LMT_MAX_CHANNEL_NUMBER_2G; ch++) {
			for (bf = 0; bf < PW_LMT_MAX_BF_NUM; bf++) {
				for (rate = 0; rate < PW_LMT_MAX_RS_NUM; rate++) {
					for (bw = 0; bw < PW_LMT_MAX_2G_BANDWITH_NUM; bw++) {
						for (reg = 0; reg < PW_LMT_MAX_REGULATION_NUM; reg++)
							pwr->tx_pwr_limit_2g[reg][bw][rate][bf][ch][tx_num] = -128;
					}
				}
			}
		}
	}

	for (tx_num = 0; tx_num < MAX_HALRF_PATH; tx_num++) {
		for (ch = 0; ch < PW_LMT_MAX_CHANNEL_NUMBER_5G; ch++) {
			for (bf = 0; bf < PW_LMT_MAX_BF_NUM; bf++) {
				for (rate = 0; rate < PW_LMT_MAX_RS_NUM; rate++) {
					for (bw = 0; bw < PW_LMT_MAX_BANDWIDTH_NUM; bw++) {
						for (reg = 0; reg < PW_LMT_MAX_REGULATION_NUM; reg++)
							pwr->tx_pwr_limit_5g[reg][bw][rate][bf][ch][tx_num] = -128;
					}
				}
			}
		}
	}

	for (tx_num = 0; tx_num < MAX_HALRF_PATH; tx_num++) {
		for (ch = 0; ch < PW_LMT_MAX_CHANNEL_NUMBER_6G; ch++) {
			for (bf = 0; bf < PW_LMT_MAX_BF_NUM; bf++) {
				for (rate = 0; rate < PW_LMT_MAX_RS_NUM; rate++) {
					for (bw = 0; bw < PW_LMT_MAX_BANDWIDTH_NUM; bw++) {
						for (reg = 0; reg < PW_LMT_MAX_REGULATION_NUM; reg++)
							pwr->tx_pwr_limit_6g[reg][bw][rate][bf][ch][tx_num] = -128;
					}
				}
			}
		}
	}

	for (tx_num = 0; tx_num < MAX_HALRF_PATH; tx_num++) {
		for (ch = 0; ch < PW_LMT_MAX_CHANNEL_NUMBER_2G; ch++) {
			for (rate = 0; rate < PW_LMT_MAX_RS_NUM; rate++) {
				for (bw = 0; bw < PW_LMT_RU_BW_NULL; bw++) {
					for (reg = 0; reg < PW_LMT_MAX_REGULATION_NUM; reg++)
						pwr->tx_pwr_limit_ru_2g[reg][bw][rate][ch][tx_num] = -128;
				}
			}
		}
	}

	for (tx_num = 0; tx_num < MAX_HALRF_PATH; tx_num++) {
		for (ch = 0; ch < PW_LMT_MAX_CHANNEL_NUMBER_5G; ch++) {
			for (rate = 0; rate < PW_LMT_MAX_RS_NUM; rate++) {
				for (bw = 0; bw < PW_LMT_RU_BW_NULL; bw++) {
					for (reg = 0; reg < PW_LMT_MAX_REGULATION_NUM; reg++)
						pwr->tx_pwr_limit_ru_5g[reg][bw][rate][ch][tx_num] = -128;
				}
			}
		}
	}

	for (tx_num = 0; tx_num < MAX_HALRF_PATH; tx_num++) {
		for (ch = 0; ch < PW_LMT_MAX_CHANNEL_NUMBER_6G; ch++) {
			for (rate = 0; rate < PW_LMT_MAX_RS_NUM; rate++) {
				for (bw = 0; bw < PW_LMT_RU_BW_NULL; bw++) {
					for (reg = 0; reg < PW_LMT_MAX_REGULATION_NUM; reg++)
						pwr->tx_pwr_limit_ru_6g[reg][bw][rate][ch][tx_num] = -128;
				}
			}
		}
	}
}

void halrf_config_rf_parameter(void *rf_void, enum phl_phy_idx phy)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_com = rf->hal_com;
	struct halrf_pwr_info *pwr = &rf->pwr_info;
	u32 start_time, finish_time;

	start_time = _os_get_cur_time_us();

	/*Radio A and B*/
	halrf_config_radio(rf, phy);

	/*Power by rate*/
	halrf_config_power_by_rate(rf, phy);

	/*Set Power limit, Power limit RU to -128*/
	halrf_config_limit_default(rf);

	/*Power limit*/
	halrf_config_power_limit(rf, phy);

	/*Power limit RU*/
	halrf_config_power_limit_ru(rf, phy);

	/*Power Track*/
	halrf_config_power_track(rf, phy);

	/*Xtal Track*/
	halrf_config_xtal_track(rf, phy);

	finish_time = _os_get_cur_time_us();

	rf->init_rf_reg_time = HALRF_ABS(finish_time, start_time) / 1000;
}


void halrf_cfg_radio_b_w_bt_status(void *rf_void, bool bt_connect)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_com = rf->hal_com;

#ifdef RF_8852C_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852C) {
		halrf_cfg_8852c_radio_b_w_bt_status(rf, bt_connect);
	}
#endif

#ifdef RF_8852BP_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852BP) {
		halrf_cfg_8852bp_radio_b_w_bt_status(rf, bt_connect);
	}
#endif


}





/******************************************************************************
 *
 * Copyright(c) 2021 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *****************************************************************************/
#define _HAL_TXPWR_C_
#include "hal_headers.h"

int rtw_hal_get_pw_lmt_regu_type_from_str(void *hal, const char *str)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	int regu;

	regu = rtw_hal_rf_get_predefined_pw_lmt_regu_type_from_str(str);
	if (regu == -1) {
		struct rtw_para_pwrlmt_info_t *para_info =
			&hal_info->phl_com->phy_sw_cap[HW_PHY_0].rf_txpwrlmt_info;

		regu = rtw_hal_find_ext_regd_num(para_info, str);
	}

	return regu;
}

const char *rtw_hal_get_pw_lmt_regu_str_from_type(void *hal, u8 regu)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	u8 predef_str_num;
	const char * const *predef_str_array = rtw_hal_rf_get_predefined_pw_lmt_regu_type_str_array(&predef_str_num);

	if (predef_str_array == NULL || predef_str_num == 0)
		return NULL;

	if (regu >= predef_str_num) {
		struct rtw_para_pwrlmt_info_t *para_info =
			&hal_info->phl_com->phy_sw_cap[HW_PHY_0].rf_txpwrlmt_info;

		return para_info->ext_regd_name[regu];
	} else
		return predef_str_array[regu];
}

u8 rtw_hal_get_pw_lmt_regu_type(void *hal, enum band_type band)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;

	return rtw_hal_rf_get_pw_lmt_regu_type(hal_info, band);
}

const char *rtw_hal_get_pw_lmt_regu_type_str(void *hal, enum band_type band)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;

	return rtw_hal_rf_get_pw_lmt_regu_type_str(hal_info, band);
}

bool rtw_hal_pw_lmt_regu_tbl_exist(void *hal, enum band_type band, u8 regu)
{
	struct hal_info_t *hal_info = hal;

	return rtw_hal_rf_pw_lmt_regu_tbl_exist(hal_info, band, regu);
}

u8 rtw_hal_ext_reg_codemap_search(void *hal, u16 domain_code, const char *country, const char **reg_name)
{
	struct hal_info_t *hal_info = hal;
	int aidx = rtw_hal_rf_file_regd_ext_search(hal_info, domain_code, country);

	if (aidx != -1) {
		struct rtw_para_pwrlmt_info_t *para_info =
			&hal_info->phl_com->phy_sw_cap[HW_PHY_0].rf_txpwrlmt_info;
		struct _hal_file_regd_ext *regd_codemap =
			(struct _hal_file_regd_ext *)para_info->ext_reg_codemap;
		u8 ret = 0;

		if (regd_codemap && para_info->ext_reg_map_num > aidx) {
			*reg_name = regd_codemap[aidx].reg_name;

			if (regd_codemap[aidx].domain == domain_code)
				ret |= RTW_PHL_EXT_REG_MATCH_DOMAIN;
			if (country && regd_codemap[aidx].country[0] == country[0]
				&& regd_codemap[aidx].country[1] == country[1])
				ret |= RTW_PHL_EXT_REG_MATCH_COUNTRY;

			_os_warn_on(ret == 0);
			return ret;
		}
	}

	return RTW_PHL_EXT_REG_MATCH_NONE;
}

bool rtw_hal_get_pwr_lmt_en(void *hal, u8 band_idx)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;

	return rtw_hal_mac_get_pwr_lmt_en_val(hal_info->hal_com, band_idx);
}

void rtw_hal_auto_pw_lmt_regu(void *hal)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;

	rtw_hal_rf_auto_pw_lmt_regu(hal_info);
}

void rtw_hal_force_pw_lmt_regu(void *hal,
	u8 regu_2g[], u8 regu_2g_len, u8 regu_5g[], u8 regu_5g_len, u8 regu_6g[], u8 regu_6g_len)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;

	rtw_hal_rf_force_pw_lmt_regu(hal_info, regu_2g, regu_2g_len
		, regu_5g, regu_5g_len, regu_6g, regu_6g_len);
}

u16 rtw_hal_get_pwr_constraint(void *hal, u8 band_idx)
{
	struct hal_info_t *hal_info = hal;
	struct rtw_tpu_info *tpu;

	if (band_idx >= MAX_BAND_NUM) {
		_os_warn_on(1);
		return 0;
	}

	tpu = &hal_info->hal_com->band[band_idx].rtw_tpu_i;
	return tpu->pwr_constraint_mb;
}

enum rtw_hal_status rtw_hal_set_pwr_constraint(void *hal, u8 band_idx, u16 mb)
{
	struct hal_info_t *hal_info = hal;
	struct rtw_tpu_info *tpu;

	if (band_idx >= MAX_BAND_NUM)
		return RTW_HAL_STATUS_FAILURE;

	tpu = &hal_info->hal_com->band[band_idx].rtw_tpu_i;

	if (tpu->pwr_constraint_mb != mb) {
		enum phl_phy_idx phy_idx = rtw_hal_hw_band_to_phy_idx(band_idx);

		/* software configuration only, no need to check for hwband ready */
		if (rtw_hal_rf_set_power_constraint(hal_info, phy_idx, mb) ==  RTW_HAL_STATUS_SUCCESS) {
			tpu->pwr_constraint_mb = mb;
			return RTW_HAL_STATUS_SUCCESS;
		}
		return RTW_HAL_STATUS_FAILURE;
	}

	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status rtw_hal_set_tx_power(void *hal, u8 band_idx,
					enum phl_pwr_table pwr_table)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;

	if (hal_info->hal_com->dbcc_en || band_idx == HW_BAND_0) {
		enum phl_phy_idx phy_idx = rtw_hal_hw_band_to_phy_idx(band_idx);

		return rtw_hal_rf_set_power(hal_info, phy_idx, pwr_table);
	}

	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status rtw_hal_get_txinfo_power(void *hal,
					s16 *txinfo_power_dbm)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_SUCCESS;
	s16 power_dbm = 0;

	hal_status = rtw_hal_bb_get_txinfo_power(hal_info, &power_dbm);
	*txinfo_power_dbm = power_dbm;

	return hal_status;
}

u8 rtw_hal_get_tx_tbl_to_pwr_times(void *hal)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;

	return rtw_hal_rf_get_tx_tbl_to_pwr_times(hal_info);
}

void rtw_hal_set_tx_pwr_comp(void *hal, enum phl_phy_idx phy,
			struct rtw_phl_regu_dyn_ant_gain *dyn_ag)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;

	rtw_hal_rf_set_tx_pwr_comp(hal_info, phy, dyn_ag);
}


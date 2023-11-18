/******************************************************************************
 *
 * Copyright(c)2021 Realtek Corporation.
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
#ifndef _HAL_TXPWR_H_
#define _HAL_TXPWR_H_

int rtw_hal_get_pw_lmt_regu_type_from_str(void *hal, const char *str);
const char *rtw_hal_get_pw_lmt_regu_str_from_type(void *hal, u8 regu);

u8 rtw_hal_get_pw_lmt_regu_type(void *hal, enum band_type band);
const char *rtw_hal_get_pw_lmt_regu_type_str(void *hal, enum band_type band);

bool rtw_hal_pw_lmt_regu_tbl_exist(void *hal, enum band_type band, u8 regu);
u8 rtw_hal_ext_reg_codemap_search(void *hal, u16 domain_code, const char *country, const char **reg_name);

bool rtw_hal_get_pwr_lmt_en(void *hal, u8 band_idx);

void rtw_hal_auto_pw_lmt_regu(void *hal);
void rtw_hal_force_pw_lmt_regu(void *hal,
	u8 regu_2g[], u8 regu_2g_len, u8 regu_5g[], u8 regu_5g_len, u8 regu_6g[], u8 regu_6g_len);

u16 rtw_hal_get_pwr_constraint(void *hal, u8 band_idx);
enum rtw_hal_status rtw_hal_set_pwr_constraint(void *hal, u8 band_idx, u16 mb);

enum rtw_hal_status rtw_hal_set_tx_power(void *hal, u8 band_idx,
					enum phl_pwr_table pwr_table);

enum rtw_hal_status rtw_hal_get_txinfo_power(void *hal,
					s16 *txinfo_power_dbm);
#endif
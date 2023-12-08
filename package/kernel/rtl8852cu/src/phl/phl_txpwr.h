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
#ifndef _PHL_TXPWR_H_
#define _PHL_TXPWR_H_

/*
 * This struct is for dynamic antenna gain feature.
 * Using antenna gain compensation to modify RF table of related regulatory.
 */
struct rtw_phl_regu_dyn_ant_gain {
	u8 regu;
	s8 ag_2g_comp; /* 2.4G antenna gain compensation */
	s8 ag_5g_comp; /* 5G antenna gain compensation */
	s8 ag_6g_comp; /* 6G antenna gain compensation */
};

int rtw_phl_get_pw_lmt_regu_type_from_str(void *phl, const char *str);
const char *rtw_phl_get_pw_lmt_regu_str_from_type(void *phl, u8 regu);

const char *rtw_phl_get_pw_lmt_regu_type_str(void *phl, enum band_type band);

bool rtw_phl_pw_lmt_regu_tbl_exist(void *phl, enum band_type band, u8 regu);
u8 rtw_phl_ext_reg_codemap_search(void *phl, u16 domain_code, const char *country, const char **reg_name);

bool rtw_phl_get_pwr_lmt_en(void *phl, u8 band_idx);

struct txpwr_regu_info_t *rtw_phl_get_pw_lmt_regu_info(void *phl);
void rtw_phl_free_pw_lmt_regu_info(void *phl, struct txpwr_regu_info_t *info);

enum rtw_phl_status
phl_cmd_txpwr_ctl_hdl(void *phl, u8 *param_buf);

enum rtw_phl_status rtw_phl_get_txinfo_pwr(void *phl, s16 *pwr_dbm);

#ifdef CONFIG_CMD_DISP
enum rtw_phl_status phl_cmd_tpe_update_hdl(void *phl, u8 *param);
#endif

#endif /*_PHL_TXPWR_H_*/

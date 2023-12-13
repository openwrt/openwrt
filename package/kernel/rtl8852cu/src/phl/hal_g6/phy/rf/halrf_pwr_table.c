/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation.
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
#include "halrf_precomp.h"

const char * const _pw_lmt_regu_type_str[PW_LMT_MAX_REGULATION_NUM] = {
	/* elements not listed here will get NULL */
	[PW_LMT_REGU_WW13]		= "WW",
	[PW_LMT_REGU_ETSI]		= "ETSI",
	[PW_LMT_REGU_FCC]		= "FCC",
	[PW_LMT_REGU_MKK]		= "MKK",
	[PW_LMT_REGU_NA]		= "NONE",
	[PW_LMT_REGU_IC]		= "IC",
	[PW_LMT_REGU_KCC]		= "KCC",
	[PW_LMT_REGU_ACMA]		= "ACMA",
	[PW_LMT_REGU_NCC]		= "NCC",
	[PW_LMT_REGU_MEXICO]		= "MEXICO",
	[PW_LMT_REGU_CHILE]		= "CHILE",
	[PW_LMT_REGU_UKRAINE]		= "UKRAINE",
	[PW_LMT_REGU_CN]		= "CN",
	[PW_LMT_REGU_QATAR]		= "QATAR",
	[PW_LMT_REGU_UK]		= "UK",
	[PW_LMT_REGU_EXT_PWR]		= "EXT",
	[PW_LMT_REGU_INTERSECT]		= "INTERSECT",
	[PW_LMT_REGU_NULL]		= NULL,
};

int halrf_get_predefined_pw_lmt_regu_type_from_str(const char *str)
{
	int i;

	for (i = 0; i < PW_LMT_MAX_REGULATION_NUM; i++)
		if (_pw_lmt_regu_type_str[i] && _os_strcmp(_pw_lmt_regu_type_str[i], str) == 0)
			return i;
	return -1;
}

const char * const *halrf_get_predefined_pw_lmt_regu_type_str_array(u8 *num)
{	
	if (num)
		*num = PW_LMT_REGU_PREDEF_NUM;
	return _pw_lmt_regu_type_str;
}

const enum halrf_pw_lmt_regulation_type _regulation_to_pw_lmt_regu_type[REGULATION_MAX] = {
	/* elements not listed here will get PW_LMT_REGU_WW13(0) */
	[REGULATION_WW]		= PW_LMT_REGU_WW13,
	[REGULATION_ETSI]	= PW_LMT_REGU_ETSI,
	[REGULATION_FCC]	= PW_LMT_REGU_FCC,
	[REGULATION_MKK]	= PW_LMT_REGU_MKK,
	[REGULATION_KCC]	= PW_LMT_REGU_KCC,
	[REGULATION_NCC]	= PW_LMT_REGU_NCC,
	[REGULATION_ACMA]	= PW_LMT_REGU_ACMA,
	[REGULATION_NA]		= PW_LMT_REGU_NA,
	[REGULATION_IC]		= PW_LMT_REGU_IC,
	[REGULATION_CHILE]	= PW_LMT_REGU_CHILE,
	[REGULATION_MEX]	= PW_LMT_REGU_MEXICO,
};

const enum halrf_pw_lmt_regulation_type _tpo_to_pw_lmt_regu_type[TPO_NA] = {
	/* elements not listed here will get PW_LMT_REGU_WW13(0) */
	[TPO_CHILE]		= PW_LMT_REGU_CHILE,
	[TPO_QATAR]		= PW_LMT_REGU_QATAR,
	[TPO_UKRAINE]		= PW_LMT_REGU_UKRAINE,
	[TPO_CN]		= PW_LMT_REGU_CN,
	[TPO_UK]		= PW_LMT_REGU_UK,
};

bool halrf_reg_tbl_exist(struct rf_info *rf, u8 band, u8 reg)
{
	struct halrf_pwr_info *pwr = &rf->pwr_info;

	return pwr->regulation[band][reg];
}

u8 halrf_get_regulation_info_force(struct rf_info *rf, u8 band)
{
	struct halrf_pwr_info *pwr = &rf->pwr_info;
	enum halrf_pw_lmt_regulation_type pw_lmt_type = PW_LMT_REGU_NULL;

	if (band == BAND_ON_24G)
		pw_lmt_type = pwr->reg_2g;
	else if (band == BAND_ON_5G)
		pw_lmt_type = pwr->reg_5g;
	else if (band == BAND_ON_6G)
		pw_lmt_type = pwr->reg_6g;

	return pw_lmt_type;
}

u8 halrf_get_regulation_info(struct rf_info *rf, u8 band)
{
	struct rtw_regulation_info rg_info = {0};
	struct halrf_pwr_info *pwr = &rf->pwr_info;
	enum halrf_pw_lmt_regulation_type pw_lmt_type = PW_LMT_REGU_NULL;
	u8 reg = REGULATION_NA;
	const char *bstr =  NULL;

	RF_DBG(rf, DBG_RF_POWER, "======>%s\n", __func__);

	if (pwr->regulation_force_en != true && pwr->extra_regd_idx != 0xff) {
		RF_DBG(rf, DBG_RF_POWER, "======>%s pwr->regulation_force_en != true, pwr->extra_regd_idx=%d\n",
			__func__, pwr->extra_regd_idx);
		return (u8)pwr->extra_regd_idx;
	}

#ifdef RTW_FLASH_98D
	if (pwr->regulation_idx != 0xff) {
		RF_DBG(rf, DBG_RF_POWER, "======>%s pwr->regulation_idx(0x%x) != 0xff\n",
			__func__, pwr->regulation_idx);
		return pwr->regulation_idx;
	}
#endif

	if (phl_is_mp_mode(rf->phl_com)) {
		RF_DBG(rf, DBG_RF_POWER, "======>%s phl_is_mp_mode(rf->phl_com)== true, Return pwr->mp_regulation=%d\n",
			__func__, pwr->mp_regulation);
		return regulation_to_pw_lmt_regu_type(pwr->mp_regulation);
	}

	if (pwr->regulation_force_en == true) {
		pw_lmt_type = halrf_get_regulation_info_force(rf, band);
		RF_DBG(rf, DBG_RF_POWER, "======>%s pwr->regulation_force_en == true\n", __func__);
		if (pw_lmt_type == PW_LMT_REGU_NULL)
			return pw_lmt_type;
		goto reg_tbl_chk;
	}

	halrf_query_regulation_info(rf, &rg_info);

	RF_DBG(rf, DBG_RF_POWER, "======>%s band=%d\n", __func__, band);

	RF_DBG(rf, DBG_RF_POWER, "domain_code=%d   regulation_2g=%d   regulation_5g=%d   regulation_6g=%d   chplan_ver=%d   country_ver=%d   rg_info.tpor=%d\n",
		rg_info.domain_code, rg_info.regulation_2g, rg_info.regulation_5g, rg_info.regulation_6g, rg_info.chplan_ver, rg_info.country_ver, rg_info.tpo);

	if (band == BAND_ON_24G) {
		reg = rg_info.regulation_2g;
		bstr = "2g";
	} else if (band == BAND_ON_5G) {
		reg = rg_info.regulation_5g;
		bstr = "5g";
	} else if (band == BAND_ON_6G) {
		reg = rg_info.regulation_6g;
		bstr = "6g";
	}

	if (rg_info.tpo != TPO_NA) {
		pw_lmt_type = tpo_to_pw_lmt_regu_type(rg_info.tpo);	
		RF_DBG(rf, DBG_RF_POWER, "%s extra pw_lmt_regu=%s(%d)\n",
			bstr, pw_lmt_regu_type_str(pw_lmt_type), pw_lmt_type);
	} else {
		if (reg == REGULATION_NA) {
			RF_DBG(rf, DBG_RF_POWER, "======>%s reg == REGULATION_NA\n", __func__);
			return PW_LMT_REGU_NULL; /* return this to distinguish from PW_LMT_REGU_NA done by NONE option from TXPWR_LMT.txt */
		}
		pw_lmt_type = regulation_to_pw_lmt_regu_type(reg);
	}

reg_tbl_chk:
	if (pwr->regulation[band][pw_lmt_type] != true) {
		RF_DBG(rf, DBG_RF_POWER, "%s pw_lmt_regu=%s(%d) is not exist return WW13 !!!\n",
			bstr, pw_lmt_regu_type_str(pw_lmt_type), pw_lmt_type);
		return PW_LMT_REGU_WW13;
	}

	return pw_lmt_type;
}

void _halrf_calc_intersect_pwr_limit_tbl(struct rf_info *rf,
	u8 reg_2g[], u8 reg_2g_len, u8 reg_5g[], u8 reg_5g_len, u8 reg_6g[], u8 reg_6g_len)
{
	struct halrf_pwr_info *pwr = &rf->pwr_info;
	u32 reg, bw, rate, bf, ch, tx_num;
	s8 tmp;

	RF_DBG(rf, DBG_RF_POWER, "======>%s\n",	__func__);

	/*Power Limit*/
	pwr->regulation[PW_LMT_BAND_2_4G][PW_LMT_REGU_INTERSECT] = true;
	pwr->regulation[PW_LMT_BAND_5G][PW_LMT_REGU_INTERSECT] = true;
	pwr->regulation[PW_LMT_BAND_6G][PW_LMT_REGU_INTERSECT] = true;
	for (tx_num = 0; tx_num < MAX_HALRF_PATH; tx_num++) {
		for (ch = 0; ch < PW_LMT_MAX_CHANNEL_NUMBER_2G; ch++) {
			for (bf = 0; bf < PW_LMT_MAX_BF_NUM; bf++) {
				for (rate = 0; rate < PW_LMT_MAX_RS_NUM; rate++) {
					for (bw = 0; bw < PW_LMT_MAX_2G_BANDWITH_NUM; bw++) {
						tmp = 127;
						for (reg = 0; reg < reg_2g_len; reg++) {
							RF_DBG(rf, DBG_RF_POWER, "pwr->tx_pwr_limit_2g[%d][%d][%d][%d][%d][%d]=   %d\n",
								reg_2g[reg], bw, rate, bf, ch, tx_num, pwr->tx_pwr_limit_2g[reg_2g[reg]][bw][rate][bf][ch][tx_num]);
							if (tmp > pwr->tx_pwr_limit_2g[reg_2g[reg]][bw][rate][bf][ch][tx_num] &&
								pwr->tx_pwr_limit_2g[reg_2g[reg]][bw][rate][bf][ch][tx_num] != -128) {
								tmp = pwr->tx_pwr_limit_2g[reg_2g[reg]][bw][rate][bf][ch][tx_num];
							}
						}
						if (tmp == 127)
							tmp = 0;
						pwr->tx_pwr_limit_2g[PW_LMT_REGU_INTERSECT][bw][rate][bf][ch][tx_num] = tmp;

						RF_DBG(rf, DBG_RF_POWER, "pwr->tx_pwr_limit_2g[%d][%d][%d][%d][%d][%d]=   %d\n",
								PW_LMT_REGU_INTERSECT, bw, rate, bf, ch, tx_num, pwr->tx_pwr_limit_2g[PW_LMT_REGU_INTERSECT][bw][rate][bf][ch][tx_num]);
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
						tmp = 127;
						for (reg = 0; reg < reg_5g_len; reg++) {
							RF_DBG(rf, DBG_RF_POWER, "pwr->tx_pwr_limit_5g[%d][%d][%d][%d][%d][%d]=   %d\n",
								reg_5g[reg], bw, rate, bf, ch, tx_num, pwr->tx_pwr_limit_5g[reg_5g[reg]][bw][rate][bf][ch][tx_num]);
							if (tmp > pwr->tx_pwr_limit_5g[reg_5g[reg]][bw][rate][bf][ch][tx_num] &&
								pwr->tx_pwr_limit_5g[reg_5g[reg]][bw][rate][bf][ch][tx_num] != -128) {
								tmp = pwr->tx_pwr_limit_5g[reg_5g[reg]][bw][rate][bf][ch][tx_num];
							}
						}
						if (tmp == 127)
							tmp = 0;
						pwr->tx_pwr_limit_5g[PW_LMT_REGU_INTERSECT][bw][rate][bf][ch][tx_num] = tmp;

						RF_DBG(rf, DBG_RF_POWER, "pwr->tx_pwr_limit_5g[%d][%d][%d][%d][%d][%d]=   %d\n",
								PW_LMT_REGU_INTERSECT, bw, rate, bf, ch, tx_num, pwr->tx_pwr_limit_5g[PW_LMT_REGU_INTERSECT][bw][rate][bf][ch][tx_num]);
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
						tmp = 127;
						for (reg = 0; reg < reg_6g_len; reg++) {
							RF_DBG(rf, DBG_RF_POWER, "pwr->tx_pwr_limit_6g[%d][%d][%d][%d][%d][%d]=   %d\n",
								reg_6g[reg], bw, rate, bf, ch, tx_num, pwr->tx_pwr_limit_6g[reg_6g[reg]][bw][rate][bf][ch][tx_num]);
							if (tmp > pwr->tx_pwr_limit_6g[reg_6g[reg]][bw][rate][bf][ch][tx_num] &&
								pwr->tx_pwr_limit_6g[reg_6g[reg]][bw][rate][bf][ch][tx_num] != -128) {
								tmp = pwr->tx_pwr_limit_6g[reg_6g[reg]][bw][rate][bf][ch][tx_num];
							}
						}
						if (tmp == 127)
							tmp = 0;
						pwr->tx_pwr_limit_6g[PW_LMT_REGU_INTERSECT][bw][rate][bf][ch][tx_num] = tmp;

						RF_DBG(rf, DBG_RF_POWER, "pwr->tx_pwr_limit_6g[%d][%d][%d][%d][%d][%d]=   %d\n",
								PW_LMT_REGU_INTERSECT, bw, rate, bf, ch, tx_num, pwr->tx_pwr_limit_6g[PW_LMT_REGU_INTERSECT][bw][rate][bf][ch][tx_num]);
					}
				}
			}
		}
	}

	/*Power Limit RU*/
	for (tx_num = 0; tx_num < MAX_HALRF_PATH; tx_num++) {
		for (ch = 0; ch < PW_LMT_MAX_CHANNEL_NUMBER_2G; ch++) {
			for (rate = 0; rate < PW_LMT_MAX_RS_NUM; rate++) {
				for (bw = 0; bw < PW_LMT_RU_BW_NULL; bw++) {
					tmp = 127;
					for (reg = 0; reg < reg_2g_len; reg++) {
						RF_DBG(rf, DBG_RF_INIT, "pwr->tx_pwr_limit_ru_2g[%d][%d][%d][%d][%d]=   %d\n",
							reg_2g[reg], bw, rate, ch, tx_num, pwr->tx_pwr_limit_ru_2g[reg_2g[reg]][bw][rate][ch][tx_num]);

						if (tmp > pwr->tx_pwr_limit_ru_2g[reg_2g[reg]][bw][rate][ch][tx_num] &&
							pwr->tx_pwr_limit_ru_2g[reg_2g[reg]][bw][rate][ch][tx_num] != -128) {
							tmp = pwr->tx_pwr_limit_ru_2g[reg_2g[reg]][bw][rate][ch][tx_num];
						}
					}
					if (tmp == 127)
						tmp = 0;
					pwr->tx_pwr_limit_ru_2g[PW_LMT_REGU_INTERSECT][bw][rate][ch][tx_num] = tmp;

					RF_DBG(rf, DBG_RF_INIT, "pwr->tx_pwr_limit_ru_2g[%d][%d][%d][%d][%d]=   %d\n",
							PW_LMT_REGU_INTERSECT, bw, rate, ch, tx_num, pwr->tx_pwr_limit_ru_2g[PW_LMT_REGU_INTERSECT][bw][rate][ch][tx_num]);

				}
			}
		}
	}

	for (tx_num = 0; tx_num < MAX_HALRF_PATH; tx_num++) {
		for (ch = 0; ch < PW_LMT_MAX_CHANNEL_NUMBER_5G; ch++) {
			for (rate = 0; rate < PW_LMT_MAX_RS_NUM; rate++) {
				for (bw = 0; bw < PW_LMT_RU_BW_NULL; bw++) {
					tmp = 127;
					for (reg = 0; reg < reg_5g_len; reg++) {
						RF_DBG(rf, DBG_RF_INIT, "pwr->tx_pwr_limit_ru_5g[%d][%d][%d][%d][%d]=   %d\n",
							reg_5g[reg], bw, rate, ch, tx_num, pwr->tx_pwr_limit_ru_5g[reg_5g[reg]][bw][rate][ch][tx_num]);

						if (tmp > pwr->tx_pwr_limit_ru_5g[reg_5g[reg]][bw][rate][ch][tx_num] &&
							pwr->tx_pwr_limit_ru_5g[reg_5g[reg]][bw][rate][ch][tx_num] != -128) {
							tmp = pwr->tx_pwr_limit_ru_5g[reg_5g[reg]][bw][rate][ch][tx_num];
						}
					}
					if (tmp == 127)
						tmp = 0;
					pwr->tx_pwr_limit_ru_5g[PW_LMT_REGU_INTERSECT][bw][rate][ch][tx_num] = tmp;

					RF_DBG(rf, DBG_RF_INIT, "pwr->tx_pwr_limit_ru_5g[%d][%d][%d][%d][%d]=   %d\n",
							PW_LMT_REGU_INTERSECT, bw, rate, ch, tx_num, pwr->tx_pwr_limit_ru_5g[PW_LMT_REGU_INTERSECT][bw][rate][ch][tx_num]);

				}
			}
		}
	}

	for (tx_num = 0; tx_num < MAX_HALRF_PATH; tx_num++) {
		for (ch = 0; ch < PW_LMT_MAX_CHANNEL_NUMBER_6G; ch++) {
			for (rate = 0; rate < PW_LMT_MAX_RS_NUM; rate++) {
				for (bw = 0; bw < PW_LMT_RU_BW_NULL; bw++) {
					tmp = 127;
					for (reg = 0; reg < reg_6g_len; reg++) {
						RF_DBG(rf, DBG_RF_INIT, "pwr->tx_pwr_limit_ru_6g[%d][%d][%d][%d][%d]=   %d\n",
							reg_6g[reg], bw, rate, ch, tx_num, pwr->tx_pwr_limit_ru_6g[reg_6g[reg]][bw][rate][ch][tx_num]);

						if (tmp > pwr->tx_pwr_limit_ru_6g[reg_6g[reg]][bw][rate][ch][tx_num] &&
							pwr->tx_pwr_limit_ru_6g[reg_6g[reg]][bw][rate][ch][tx_num] != -128) {
							tmp = pwr->tx_pwr_limit_ru_6g[reg_6g[reg]][bw][rate][ch][tx_num];
						}
					}
					if (tmp == 127)
						tmp = 0;
					pwr->tx_pwr_limit_ru_6g[PW_LMT_REGU_INTERSECT][bw][rate][ch][tx_num] = tmp;

					RF_DBG(rf, DBG_RF_INIT, "pwr->tx_pwr_limit_ru_6g[%d][%d][%d][%d][%d]=   %d\n",
							PW_LMT_REGU_INTERSECT, bw, rate, ch, tx_num, pwr->tx_pwr_limit_ru_6g[PW_LMT_REGU_INTERSECT][bw][rate][ch][tx_num]);

				}
			}
		}
	}
}

void halrf_force_regulation(struct rf_info *rf, bool enable,
	u8 reg_2g[], u8 reg_2g_len, u8 reg_5g[], u8 reg_5g_len, u8 reg_6g[], u8 reg_6g_len)
{
	struct halrf_pwr_info *pwr = &rf->pwr_info;
	u8 i;

	RF_DBG(rf, DBG_RF_POWER, "======>%s   enable=%d\n",
		__func__, enable);

	if (reg_2g_len == 0)
		pwr->reg_2g = PW_LMT_REGU_NULL;
	else if (reg_2g_len == 1)
		pwr->reg_2g = reg_2g[0];
	else
		pwr->reg_2g = PW_LMT_REGU_INTERSECT;

	if (reg_5g_len == 0)
		pwr->reg_5g = PW_LMT_REGU_NULL;
	else if (reg_5g_len == 1)
		pwr->reg_5g = reg_5g[0];
	else
		pwr->reg_5g = PW_LMT_REGU_INTERSECT;

	if (reg_6g_len == 0)
		pwr->reg_6g = PW_LMT_REGU_NULL;
	else if (reg_6g_len == 1)
		pwr->reg_6g = reg_6g[0];
	else
		pwr->reg_6g = PW_LMT_REGU_INTERSECT;

	pwr->regulation_force_en = enable;

	pwr->reg_2g_len = reg_2g_len;
	pwr->reg_5g_len = reg_5g_len;
	pwr->reg_6g_len = reg_6g_len;

	for (i = 0; i < reg_2g_len; i++) {
		if (i >= PW_LMT_MAX_REGULATION_NUM)
			break;
		pwr->reg_array_2g[i] = reg_2g[i];
		RF_DBG(rf, DBG_RF_POWER, "======>%s   reg_2g[%d]=%d   reg_2g_len=%d\n",
			__func__, i, reg_2g[i], reg_2g_len);
	}

	for (i = 0; i < reg_5g_len; i++) {
		if (i >= PW_LMT_MAX_REGULATION_NUM)
			break;
		pwr->reg_array_5g[i] = reg_5g[i];
		RF_DBG(rf, DBG_RF_POWER, "======>%s   reg_5g[%d]=%d   reg_5g_len=%d\n",
			__func__, i, reg_5g[i], reg_5g_len);
	}

	for (i = 0; i < reg_6g_len; i++) {
		if (i >= PW_LMT_MAX_REGULATION_NUM)
			break;
		pwr->reg_array_6g[i] = reg_6g[i];
		RF_DBG(rf, DBG_RF_POWER, "======>%s   reg_6g[%d]=%d   reg_6g_len=%d\n",
			__func__, i, reg_6g[i], reg_6g_len);
	}

	/*Calculate WW power limit, power limit RU */
	_halrf_calc_intersect_pwr_limit_tbl(rf,
		reg_2g, reg_2g_len, reg_5g, reg_5g_len, reg_6g, reg_6g_len);

}

const char *halrf_get_pw_lmt_regu_type_str(struct rf_info *rf, u8 band)
{
	u8 reg = halrf_get_regulation_info(rf, band);

	return pw_lmt_regu_type_str(reg);
}

#ifndef RF_8730A_SUPPORT
const char *halrf_get_pw_lmt_regu_type_str_extra(struct rf_info *rf, u8 band)
{
	struct rtw_para_pwrlmt_info_t *pwrlmt_info = NULL;
	u8 reg = halrf_get_regulation_info(rf, band);

	pwrlmt_info = &rf->phl_com->phy_sw_cap[HW_PHY_0].rf_txpwrlmt_info;

	if (reg >= PW_LMT_REGU_PREDEF_NUM)
		return pwrlmt_info->ext_regd_name[reg];

	return pw_lmt_regu_type_str(reg);
}
#endif

void halrf_power_by_rate_store_to_array(struct rf_info *rf,
			u32 band, u32 tx_num, u32 rate_id, u32 data)
{
	struct halrf_pwr_info *pwr = &rf->pwr_info;
	
	RF_DBG(rf, DBG_RF_INIT, "======>%s band=%d tx_num=%d rate_id=%d data=0x%x\n",
				__func__, band, tx_num, rate_id, data);

	if (band != (u32)BAND_ON_24G && band != (u32)BAND_ON_5G && band != (u32)BAND_ON_6G) {
		RF_DBG(rf, DBG_RF_INIT, "Invalid Band %d Return!!!\n", band);
		return;
	}

	if (tx_num > 4 && tx_num != 15) {
		RF_DBG(rf, DBG_RF_INIT, "Invalid TxNum %d Return!!!\n", tx_num);
		return;
	}

	if (band == BAND_ON_24G && tx_num == 0 && rate_id == PW_BYRATE_RATE_11M_1M) {
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_CCK1] = (s8)(data & 0xff);
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_CCK2] = (s8)((data & 0xff00) >> 8);
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_CCK5_5] = (s8)((data & 0xff0000) >> 16);
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_CCK11] = (s8)((data & 0xff000000) >> 24);
	} else if (tx_num == 0 && rate_id == PW_BYRATE_RATE_18M_6M) {
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_OFDM6] = (s8)(data & 0xff);
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_OFDM9] = (s8)((data & 0xff00) >> 8);
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_OFDM12] = (s8)((data & 0xff0000) >> 16);
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_OFDM18] = (s8)((data & 0xff000000) >> 24);
	} else if (tx_num == 0 && rate_id == PW_BYRATE_RATE_54M_24M) {
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_OFDM24] = (s8)(data & 0xff);
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_OFDM36] = (s8)((data & 0xff00) >> 8);
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_OFDM48] = (s8)((data & 0xff0000) >> 16);
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_OFDM54] = (s8)((data & 0xff000000) >> 24);
	} else if (tx_num == 0 && rate_id == PW_BYRATE_RATE_MCS3_0) {
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_MCS0] = (s8)(data & 0xff);
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_MCS1] = (s8)((data & 0xff00) >> 8);
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_MCS2] = (s8)((data & 0xff0000) >> 16);
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_MCS3] = (s8)((data & 0xff000000) >> 24);

		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_VHT_NSS1_MCS0] = (s8)(data & 0xff);
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_VHT_NSS1_MCS1] = (s8)((data & 0xff00) >> 8);
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_VHT_NSS1_MCS2] = (s8)((data & 0xff0000) >> 16);
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_VHT_NSS1_MCS3] = (s8)((data & 0xff000000) >> 24);

		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_HE_NSS1_MCS0] = (s8)(data & 0xff);
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_HE_NSS1_MCS1] = (s8)((data & 0xff00) >> 8);
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_HE_NSS1_MCS2] = (s8)((data & 0xff0000) >> 16);
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_HE_NSS1_MCS3] = (s8)((data & 0xff000000) >> 24);
	} else if (tx_num == 0 && rate_id == PW_BYRATE_RATE_MCS7_4) {
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_MCS4] = (s8)(data & 0xff);
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_MCS5] = (s8)((data & 0xff00) >> 8);
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_MCS6] = (s8)((data & 0xff0000) >> 16);
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_MCS7] = (s8)((data & 0xff000000) >> 24);

		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_VHT_NSS1_MCS4] = (s8)(data & 0xff);
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_VHT_NSS1_MCS5] = (s8)((data & 0xff00) >> 8);
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_VHT_NSS1_MCS6] = (s8)((data & 0xff0000) >> 16);
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_VHT_NSS1_MCS7] = (s8)((data & 0xff000000) >> 24);

		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_HE_NSS1_MCS4] = (s8)(data & 0xff);
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_HE_NSS1_MCS5] = (s8)((data & 0xff00) >> 8);
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_HE_NSS1_MCS6] = (s8)((data & 0xff0000) >> 16);
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_HE_NSS1_MCS7] = (s8)((data & 0xff000000) >> 24);
	} else if (tx_num == 0 && rate_id == PW_BYRATE_RATE_MCS11_8) {
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_VHT_NSS1_MCS8] = (s8)(data & 0xff);
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_VHT_NSS1_MCS9] = (s8)((data & 0xff00) >> 8);

		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_HE_NSS1_MCS8] = (s8)(data & 0xff);
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_HE_NSS1_MCS9] = (s8)((data & 0xff00) >> 8);
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_HE_NSS1_MCS10] = (s8)((data & 0xff0000) >> 16);
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_HE_NSS1_MCS11] = (s8)((data & 0xff000000) >> 24);
	} else if (tx_num == 0 && rate_id == PW_BYRATE_RATE_DCM4_0) {
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_HEDCM_NSS1_MCS0] = (s8)(data & 0xff);
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_HEDCM_NSS1_MCS1] = (s8)((data & 0xff00) >> 8);
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_HEDCM_NSS1_MCS3] = (s8)((data & 0xff0000) >> 16);
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_HEDCM_NSS1_MCS4] = (s8)((data & 0xff000000) >> 24);
	} else if (band == BAND_ON_24G && tx_num == 15 && rate_id == PW_BYRATE_RATE_AllRate2_1) {
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_VHT_OFFSET] = (s8)(data & 0xff);
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_HT_OFFSET] = (s8)((data & 0xff00) >> 8);
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_OFDM_OFFSET] = (s8)((data & 0xff0000) >> 16);
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_CCK_OFFSET] = (s8)((data & 0xff000000) >> 24);
	} else if (band == BAND_ON_24G && tx_num == 15 && rate_id == PW_BYRATE_RATE_AllRate2_2) {
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_HEDCM_OFFSET] = (s8)((data & 0xff000000) >> 24);
	} else if (tx_num == 1 && rate_id == PW_BYRATE_RATE_MCS3_0) {
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_MCS8] = (s8)(data & 0xff);
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_MCS9] = (s8)((data & 0xff00) >> 8);
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_MCS10] = (s8)((data & 0xff0000) >> 16);
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_MCS11] = (s8)((data & 0xff000000) >> 24);

		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_VHT_NSS2_MCS0] = (s8)(data & 0xff);
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_VHT_NSS2_MCS1] = (s8)((data & 0xff00) >> 8);
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_VHT_NSS2_MCS2] = (s8)((data & 0xff0000) >> 16);
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_VHT_NSS2_MCS3] = (s8)((data & 0xff000000) >> 24);

		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_HE_NSS2_MCS0] = (s8)(data & 0xff);
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_HE_NSS2_MCS1] = (s8)((data & 0xff00) >> 8);
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_HE_NSS2_MCS2] = (s8)((data & 0xff0000) >> 16);
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_HE_NSS2_MCS3] = (s8)((data & 0xff000000) >> 24);
	} else if (tx_num == 1 && rate_id == PW_BYRATE_RATE_MCS7_4) {
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_MCS12] = (s8)(data & 0xff);
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_MCS13] = (s8)((data & 0xff00) >> 8);
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_MCS14] = (s8)((data & 0xff0000) >> 16);
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_MCS15] = (s8)((data & 0xff000000) >> 24);

		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_VHT_NSS2_MCS4] = (s8)(data & 0xff);
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_VHT_NSS2_MCS5] = (s8)((data & 0xff00) >> 8);
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_VHT_NSS2_MCS6] = (s8)((data & 0xff0000) >> 16);
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_VHT_NSS2_MCS7] = (s8)((data & 0xff000000) >> 24);

		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_HE_NSS2_MCS4] = (s8)(data & 0xff);
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_HE_NSS2_MCS5] = (s8)((data & 0xff00) >> 8);
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_HE_NSS2_MCS6] = (s8)((data & 0xff0000) >> 16);
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_HE_NSS2_MCS7] = (s8)((data & 0xff000000) >> 24);
	} else if (tx_num == 1 && rate_id == PW_BYRATE_RATE_MCS11_8) {
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_VHT_NSS2_MCS8] = (s8)(data & 0xff);
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_VHT_NSS2_MCS9] = (s8)((data & 0xff00) >> 8);

		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_HE_NSS2_MCS8] = (s8)(data & 0xff);
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_HE_NSS2_MCS9] = (s8)((data & 0xff00) >> 8);
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_HE_NSS2_MCS10] = (s8)((data & 0xff0000) >> 16);
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_HE_NSS2_MCS11] = (s8)((data & 0xff000000) >> 24);
	} else if (tx_num == 1 && rate_id == PW_BYRATE_RATE_DCM4_0) {
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_HEDCM_NSS2_MCS0] = (s8)(data & 0xff);
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_HEDCM_NSS2_MCS1] = (s8)((data & 0xff00) >> 8);
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_HEDCM_NSS2_MCS3] = (s8)((data & 0xff0000) >> 16);
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_HEDCM_NSS2_MCS4] = (s8)((data & 0xff000000) >> 24);
	} else if (band == BAND_ON_5G && tx_num == 15 && rate_id == PW_BYRATE_RATE_AllRate5_1) {
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_HEDCM_OFFSET] = (s8)(data & 0xff);
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_VHT_OFFSET] = (s8)((data & 0xff00) >> 8);
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_HT_OFFSET] = (s8)((data & 0xff0000) >> 16);
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_OFDM_OFFSET] = (s8)((data & 0xff000000) >> 24);
	} else if (band == BAND_ON_6G && tx_num == 15 && rate_id == PW_BYRATE_RATE_AllRate6_1) {
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_HEDCM_OFFSET] = (s8)(data & 0xff);
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_VHT_OFFSET] = (s8)((data & 0xff00) >> 8);
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_HT_OFFSET] = (s8)((data & 0xff0000) >> 16);
		pwr->tx_pwr_by_rate[band][HALRF_DATA_RATE_OFDM_OFFSET] = (s8)((data & 0xff000000) >> 24);
	}
}

u8 halrf_get_ch_idx_to_limit_array(struct rf_info *rf, u8 channel)
{
	u8	channelIndex;

	if (channel >= 1 && channel <= 14)
		channelIndex = channel - 1;
	else if (channel >= 36 && channel <= 64)
		channelIndex = (channel - 36) / 2;
	else if (channel >= 100 && channel <= 144)
		channelIndex = ((channel - 100) / 2) + 15;
	else if (channel >= 149 && channel <= 177)
		channelIndex = ((channel - 149) / 2) + 38;
	else
		channelIndex = 0;

	return channelIndex;
}

u8 halrf_get_ch_idx_to_6g_limit_array(struct rf_info *rf, u8 channel)
{
	u8	channelIndex;

	if (channel >= 1 && channel <= 29)
		channelIndex = (channel - 1) / 2;
	else if (channel >= 33 && channel <= 61)
		channelIndex = (channel - 3) / 2;
	else if (channel >= 65 && channel <= 93)
		channelIndex = (channel - 5) / 2;
	else if (channel >= 97 && channel <= 125)
		channelIndex = (channel - 7) / 2;
	else if (channel >= 129 && channel <= 157)
		channelIndex = (channel - 9) / 2;
	else if (channel >= 161 && channel <= 189)
		channelIndex = (channel - 11) / 2;
	else if (channel >= 193 && channel <= 221)
		channelIndex = (channel - 13) / 2;
	else if (channel >= 225 && channel <= 253)
		channelIndex = (channel - 15) / 2;
	else
		channelIndex = 0;

	return channelIndex;
}

u8 halrf_get_limit_ch_idx_to_ch_idx(struct rf_info *rf, u8 band, u8 channel)
{
	u8	channelIndex;

	if (band == PW_LMT_BAND_2_4G) {
		if (channel >= 0 && channel <= 13)
			channelIndex = channel + 1;
		else
			channelIndex = 0;
	} else if (band == PW_LMT_BAND_5G) {
		if (channel >= 0 && channel <= 14)
			channelIndex = channel * 2 + 36;
		else if (channel >= 15 && channel <= 37)
			channelIndex = (channel - 15) * 2 + 100;
		else if (channel >= 38 && channel <= 52)
			channelIndex = (channel - 38) * 2 + 149;
		else
			channelIndex = 0;
	} else {
		if (channel >= 0 && channel <= 14)
			channelIndex = channel * 2 + 1;
		else if (channel >= 15 && channel <= 29)
			channelIndex = channel * 2 + 3;
		else if (channel >= 30 && channel <= 44)
			channelIndex = channel * 2 + 5;
		else if (channel >= 45 && channel <= 59)
			channelIndex = channel * 2 + 7;
		else if (channel >= 60 && channel <= 74)
			channelIndex = channel * 2 + 9;
		else if (channel >= 75 && channel <= 89)
			channelIndex = channel * 2 + 11;
		else if (channel >= 90 && channel <= 104)
			channelIndex = channel * 2 + 13;
		else if (channel >= 105 && channel <= 119)
			channelIndex = channel * 2 + 15;
		else
			channelIndex = 0;
	}

	return channelIndex;
}

u16 halrf_hw_rate_to_pwr_by_rate(struct rf_info *rf, u16 rate)
{
	u16 ret_rate = HALRF_DATA_RATE_OFDM6;

	switch(rate) {
	case RTW_DATA_RATE_CCK1:	ret_rate = HALRF_DATA_RATE_CCK1; break;
	case RTW_DATA_RATE_CCK2:	ret_rate = HALRF_DATA_RATE_CCK2; break;
	case RTW_DATA_RATE_CCK5_5:	ret_rate = HALRF_DATA_RATE_CCK5_5; break;
	case RTW_DATA_RATE_CCK11:	ret_rate = HALRF_DATA_RATE_CCK11; break;
	case RTW_DATA_RATE_OFDM6:	ret_rate = HALRF_DATA_RATE_OFDM6; break;
	case RTW_DATA_RATE_OFDM9:	ret_rate = HALRF_DATA_RATE_OFDM9; break;
	case RTW_DATA_RATE_OFDM12:	ret_rate = HALRF_DATA_RATE_OFDM12; break;
	case RTW_DATA_RATE_OFDM18:	ret_rate = HALRF_DATA_RATE_OFDM18; break;
	case RTW_DATA_RATE_OFDM24:	ret_rate = HALRF_DATA_RATE_OFDM24; break;
	case RTW_DATA_RATE_OFDM36:	ret_rate = HALRF_DATA_RATE_OFDM36; break;
	case RTW_DATA_RATE_OFDM48:	ret_rate = HALRF_DATA_RATE_OFDM48; break;
	case RTW_DATA_RATE_OFDM54:	ret_rate = HALRF_DATA_RATE_OFDM54; break;

	case RTW_DATA_RATE_MCS0:	ret_rate = HALRF_DATA_RATE_MCS0; break;
	case RTW_DATA_RATE_MCS1:	ret_rate = HALRF_DATA_RATE_MCS1; break;
	case RTW_DATA_RATE_MCS2:	ret_rate = HALRF_DATA_RATE_MCS2; break;
	case RTW_DATA_RATE_MCS3:	ret_rate = HALRF_DATA_RATE_MCS3; break;
	case RTW_DATA_RATE_MCS4:	ret_rate = HALRF_DATA_RATE_MCS4; break;
	case RTW_DATA_RATE_MCS5:	ret_rate = HALRF_DATA_RATE_MCS5; break;
	case RTW_DATA_RATE_MCS6:	ret_rate = HALRF_DATA_RATE_MCS6; break;
	case RTW_DATA_RATE_MCS7:	ret_rate = HALRF_DATA_RATE_MCS7; break;
	case RTW_DATA_RATE_MCS8:	ret_rate = HALRF_DATA_RATE_MCS8; break;
	case RTW_DATA_RATE_MCS9:	ret_rate = HALRF_DATA_RATE_MCS9; break;
	case RTW_DATA_RATE_MCS10:	ret_rate = HALRF_DATA_RATE_MCS10; break;
	case RTW_DATA_RATE_MCS11:	ret_rate = HALRF_DATA_RATE_MCS11; break;
	case RTW_DATA_RATE_MCS12:	ret_rate = HALRF_DATA_RATE_MCS12; break;
	case RTW_DATA_RATE_MCS13:	ret_rate = HALRF_DATA_RATE_MCS13; break;
	case RTW_DATA_RATE_MCS14:	ret_rate = HALRF_DATA_RATE_MCS14; break;
	case RTW_DATA_RATE_MCS15:	ret_rate = HALRF_DATA_RATE_MCS15; break;
	case RTW_DATA_RATE_MCS16:	ret_rate = HALRF_DATA_RATE_MCS16; break;
	case RTW_DATA_RATE_MCS17:	ret_rate = HALRF_DATA_RATE_MCS17; break;
	case RTW_DATA_RATE_MCS18:	ret_rate = HALRF_DATA_RATE_MCS18; break;
	case RTW_DATA_RATE_MCS19:	ret_rate = HALRF_DATA_RATE_MCS19; break;
	case RTW_DATA_RATE_MCS20:	ret_rate = HALRF_DATA_RATE_MCS20; break;
	case RTW_DATA_RATE_MCS21:	ret_rate = HALRF_DATA_RATE_MCS21; break;
	case RTW_DATA_RATE_MCS22:	ret_rate = HALRF_DATA_RATE_MCS22; break;
	case RTW_DATA_RATE_MCS23:	ret_rate = HALRF_DATA_RATE_MCS23; break;
	case RTW_DATA_RATE_MCS24:	ret_rate = HALRF_DATA_RATE_MCS24; break;
	case RTW_DATA_RATE_MCS25:	ret_rate = HALRF_DATA_RATE_MCS25; break;
	case RTW_DATA_RATE_MCS26:	ret_rate = HALRF_DATA_RATE_MCS26; break;
	case RTW_DATA_RATE_MCS27:	ret_rate = HALRF_DATA_RATE_MCS27; break;
	case RTW_DATA_RATE_MCS28:	ret_rate = HALRF_DATA_RATE_MCS28; break;
	case RTW_DATA_RATE_MCS29:	ret_rate = HALRF_DATA_RATE_MCS29; break;
	case RTW_DATA_RATE_MCS30:	ret_rate = HALRF_DATA_RATE_MCS30; break;
	case RTW_DATA_RATE_MCS31:	ret_rate = HALRF_DATA_RATE_MCS31; break;

	case RTW_DATA_RATE_VHT_NSS1_MCS0:	ret_rate = HALRF_DATA_RATE_VHT_NSS1_MCS0; break;
	case RTW_DATA_RATE_VHT_NSS1_MCS1:	ret_rate = HALRF_DATA_RATE_VHT_NSS1_MCS1; break;
	case RTW_DATA_RATE_VHT_NSS1_MCS2:	ret_rate = HALRF_DATA_RATE_VHT_NSS1_MCS2; break;
	case RTW_DATA_RATE_VHT_NSS1_MCS3:	ret_rate = HALRF_DATA_RATE_VHT_NSS1_MCS3; break;
	case RTW_DATA_RATE_VHT_NSS1_MCS4:	ret_rate = HALRF_DATA_RATE_VHT_NSS1_MCS4; break;
	case RTW_DATA_RATE_VHT_NSS1_MCS5:	ret_rate = HALRF_DATA_RATE_VHT_NSS1_MCS5; break;
	case RTW_DATA_RATE_VHT_NSS1_MCS6:	ret_rate = HALRF_DATA_RATE_VHT_NSS1_MCS6; break;
	case RTW_DATA_RATE_VHT_NSS1_MCS7:	ret_rate = HALRF_DATA_RATE_VHT_NSS1_MCS7; break;
	case RTW_DATA_RATE_VHT_NSS1_MCS8:	ret_rate = HALRF_DATA_RATE_VHT_NSS1_MCS8; break;
	case RTW_DATA_RATE_VHT_NSS1_MCS9:	ret_rate = HALRF_DATA_RATE_VHT_NSS1_MCS9; break;	
	case RTW_DATA_RATE_VHT_NSS2_MCS0:	ret_rate = HALRF_DATA_RATE_VHT_NSS2_MCS0; break;
	case RTW_DATA_RATE_VHT_NSS2_MCS1:	ret_rate = HALRF_DATA_RATE_VHT_NSS2_MCS1; break;
	case RTW_DATA_RATE_VHT_NSS2_MCS2:	ret_rate = HALRF_DATA_RATE_VHT_NSS2_MCS2; break;
	case RTW_DATA_RATE_VHT_NSS2_MCS3:	ret_rate = HALRF_DATA_RATE_VHT_NSS2_MCS3; break;
	case RTW_DATA_RATE_VHT_NSS2_MCS4:	ret_rate = HALRF_DATA_RATE_VHT_NSS2_MCS4; break;
	case RTW_DATA_RATE_VHT_NSS2_MCS5:	ret_rate = HALRF_DATA_RATE_VHT_NSS2_MCS5; break;
	case RTW_DATA_RATE_VHT_NSS2_MCS6:	ret_rate = HALRF_DATA_RATE_VHT_NSS2_MCS6; break;
	case RTW_DATA_RATE_VHT_NSS2_MCS7:	ret_rate = HALRF_DATA_RATE_VHT_NSS2_MCS7; break;
	case RTW_DATA_RATE_VHT_NSS2_MCS8:	ret_rate = HALRF_DATA_RATE_VHT_NSS2_MCS8; break;
	case RTW_DATA_RATE_VHT_NSS2_MCS9:	ret_rate = HALRF_DATA_RATE_VHT_NSS2_MCS9; break;	
	case RTW_DATA_RATE_VHT_NSS3_MCS0:	ret_rate = HALRF_DATA_RATE_VHT_NSS3_MCS0; break;
	case RTW_DATA_RATE_VHT_NSS3_MCS1:	ret_rate = HALRF_DATA_RATE_VHT_NSS3_MCS1; break;
	case RTW_DATA_RATE_VHT_NSS3_MCS2:	ret_rate = HALRF_DATA_RATE_VHT_NSS3_MCS2; break;
	case RTW_DATA_RATE_VHT_NSS3_MCS3:	ret_rate = HALRF_DATA_RATE_VHT_NSS3_MCS3; break;
	case RTW_DATA_RATE_VHT_NSS3_MCS4:	ret_rate = HALRF_DATA_RATE_VHT_NSS3_MCS4; break;
	case RTW_DATA_RATE_VHT_NSS3_MCS5:	ret_rate = HALRF_DATA_RATE_VHT_NSS3_MCS5; break;
	case RTW_DATA_RATE_VHT_NSS3_MCS6:	ret_rate = HALRF_DATA_RATE_VHT_NSS3_MCS6; break;
	case RTW_DATA_RATE_VHT_NSS3_MCS7:	ret_rate = HALRF_DATA_RATE_VHT_NSS3_MCS7; break;
	case RTW_DATA_RATE_VHT_NSS3_MCS8:	ret_rate = HALRF_DATA_RATE_VHT_NSS3_MCS8; break;
	case RTW_DATA_RATE_VHT_NSS3_MCS9:	ret_rate = HALRF_DATA_RATE_VHT_NSS3_MCS9; break;
	case RTW_DATA_RATE_VHT_NSS4_MCS0:	ret_rate = HALRF_DATA_RATE_VHT_NSS4_MCS0; break;
	case RTW_DATA_RATE_VHT_NSS4_MCS1:	ret_rate = HALRF_DATA_RATE_VHT_NSS4_MCS1; break;
	case RTW_DATA_RATE_VHT_NSS4_MCS2:	ret_rate = HALRF_DATA_RATE_VHT_NSS4_MCS2; break;
	case RTW_DATA_RATE_VHT_NSS4_MCS3:	ret_rate = HALRF_DATA_RATE_VHT_NSS4_MCS3; break;
	case RTW_DATA_RATE_VHT_NSS4_MCS4:	ret_rate = HALRF_DATA_RATE_VHT_NSS4_MCS4; break;
	case RTW_DATA_RATE_VHT_NSS4_MCS5:	ret_rate = HALRF_DATA_RATE_VHT_NSS4_MCS5; break;
	case RTW_DATA_RATE_VHT_NSS4_MCS6:	ret_rate = HALRF_DATA_RATE_VHT_NSS4_MCS6; break;
	case RTW_DATA_RATE_VHT_NSS4_MCS7:	ret_rate = HALRF_DATA_RATE_VHT_NSS4_MCS7; break;
	case RTW_DATA_RATE_VHT_NSS4_MCS8:	ret_rate = HALRF_DATA_RATE_VHT_NSS4_MCS8; break;
	case RTW_DATA_RATE_VHT_NSS4_MCS9:	ret_rate = HALRF_DATA_RATE_VHT_NSS4_MCS9; break;

	case RTW_DATA_RATE_HE_NSS1_MCS0:	ret_rate = HALRF_DATA_RATE_HE_NSS1_MCS0; break;
	case RTW_DATA_RATE_HE_NSS1_MCS1:	ret_rate = HALRF_DATA_RATE_HE_NSS1_MCS1; break;
	case RTW_DATA_RATE_HE_NSS1_MCS2:	ret_rate = HALRF_DATA_RATE_HE_NSS1_MCS2; break;
	case RTW_DATA_RATE_HE_NSS1_MCS3:	ret_rate = HALRF_DATA_RATE_HE_NSS1_MCS3; break;
	case RTW_DATA_RATE_HE_NSS1_MCS4:	ret_rate = HALRF_DATA_RATE_HE_NSS1_MCS4; break;
	case RTW_DATA_RATE_HE_NSS1_MCS5:	ret_rate = HALRF_DATA_RATE_HE_NSS1_MCS5; break;
	case RTW_DATA_RATE_HE_NSS1_MCS6:	ret_rate = HALRF_DATA_RATE_HE_NSS1_MCS6; break;
	case RTW_DATA_RATE_HE_NSS1_MCS7:	ret_rate = HALRF_DATA_RATE_HE_NSS1_MCS7; break;
	case RTW_DATA_RATE_HE_NSS1_MCS8:	ret_rate = HALRF_DATA_RATE_HE_NSS1_MCS8; break;
	case RTW_DATA_RATE_HE_NSS1_MCS9:	ret_rate = HALRF_DATA_RATE_HE_NSS1_MCS9; break; 
	case RTW_DATA_RATE_HE_NSS1_MCS10:	ret_rate = HALRF_DATA_RATE_HE_NSS1_MCS10; break;
	case RTW_DATA_RATE_HE_NSS1_MCS11:	ret_rate = HALRF_DATA_RATE_HE_NSS1_MCS11; break; 
	case RTW_DATA_RATE_HE_NSS2_MCS0:	ret_rate = HALRF_DATA_RATE_HE_NSS2_MCS0; break;
	case RTW_DATA_RATE_HE_NSS2_MCS1:	ret_rate = HALRF_DATA_RATE_HE_NSS2_MCS1; break;
	case RTW_DATA_RATE_HE_NSS2_MCS2:	ret_rate = HALRF_DATA_RATE_HE_NSS2_MCS2; break;
	case RTW_DATA_RATE_HE_NSS2_MCS3:	ret_rate = HALRF_DATA_RATE_HE_NSS2_MCS3; break;
	case RTW_DATA_RATE_HE_NSS2_MCS4:	ret_rate = HALRF_DATA_RATE_HE_NSS2_MCS4; break;
	case RTW_DATA_RATE_HE_NSS2_MCS5:	ret_rate = HALRF_DATA_RATE_HE_NSS2_MCS5; break;
	case RTW_DATA_RATE_HE_NSS2_MCS6:	ret_rate = HALRF_DATA_RATE_HE_NSS2_MCS6; break;
	case RTW_DATA_RATE_HE_NSS2_MCS7:	ret_rate = HALRF_DATA_RATE_HE_NSS2_MCS7; break;
	case RTW_DATA_RATE_HE_NSS2_MCS8:	ret_rate = HALRF_DATA_RATE_HE_NSS2_MCS8; break;
	case RTW_DATA_RATE_HE_NSS2_MCS9:	ret_rate = HALRF_DATA_RATE_HE_NSS2_MCS9; break;
	case RTW_DATA_RATE_HE_NSS2_MCS10:	ret_rate = HALRF_DATA_RATE_HE_NSS2_MCS10; break;
	case RTW_DATA_RATE_HE_NSS2_MCS11:	ret_rate = HALRF_DATA_RATE_HE_NSS2_MCS11; break; 
	case RTW_DATA_RATE_HE_NSS3_MCS0:	ret_rate = HALRF_DATA_RATE_HE_NSS3_MCS0; break;
	case RTW_DATA_RATE_HE_NSS3_MCS1:	ret_rate = HALRF_DATA_RATE_HE_NSS3_MCS1; break;
	case RTW_DATA_RATE_HE_NSS3_MCS2:	ret_rate = HALRF_DATA_RATE_HE_NSS3_MCS2; break;
	case RTW_DATA_RATE_HE_NSS3_MCS3:	ret_rate = HALRF_DATA_RATE_HE_NSS3_MCS3; break;
	case RTW_DATA_RATE_HE_NSS3_MCS4:	ret_rate = HALRF_DATA_RATE_HE_NSS3_MCS4; break;
	case RTW_DATA_RATE_HE_NSS3_MCS5:	ret_rate = HALRF_DATA_RATE_HE_NSS3_MCS5; break;
	case RTW_DATA_RATE_HE_NSS3_MCS6:	ret_rate = HALRF_DATA_RATE_HE_NSS3_MCS6; break;
	case RTW_DATA_RATE_HE_NSS3_MCS7:	ret_rate = HALRF_DATA_RATE_HE_NSS3_MCS7; break;
	case RTW_DATA_RATE_HE_NSS3_MCS8:	ret_rate = HALRF_DATA_RATE_HE_NSS3_MCS8; break;
	case RTW_DATA_RATE_HE_NSS3_MCS9:	ret_rate = HALRF_DATA_RATE_HE_NSS3_MCS9; break;
	case RTW_DATA_RATE_HE_NSS3_MCS10:	ret_rate = HALRF_DATA_RATE_HE_NSS3_MCS10; break;
	case RTW_DATA_RATE_HE_NSS3_MCS11:	ret_rate = HALRF_DATA_RATE_HE_NSS3_MCS11; break; 
	case RTW_DATA_RATE_HE_NSS4_MCS0:	ret_rate = HALRF_DATA_RATE_HE_NSS4_MCS0; break;
	case RTW_DATA_RATE_HE_NSS4_MCS1:	ret_rate = HALRF_DATA_RATE_HE_NSS4_MCS1; break;
	case RTW_DATA_RATE_HE_NSS4_MCS2:	ret_rate = HALRF_DATA_RATE_HE_NSS4_MCS2; break;
	case RTW_DATA_RATE_HE_NSS4_MCS3:	ret_rate = HALRF_DATA_RATE_HE_NSS4_MCS3; break;
	case RTW_DATA_RATE_HE_NSS4_MCS4:	ret_rate = HALRF_DATA_RATE_HE_NSS4_MCS4; break;
	case RTW_DATA_RATE_HE_NSS4_MCS5:	ret_rate = HALRF_DATA_RATE_HE_NSS4_MCS5; break;
	case RTW_DATA_RATE_HE_NSS4_MCS6:	ret_rate = HALRF_DATA_RATE_HE_NSS4_MCS6; break;
	case RTW_DATA_RATE_HE_NSS4_MCS7:	ret_rate = HALRF_DATA_RATE_HE_NSS4_MCS7; break;
	case RTW_DATA_RATE_HE_NSS4_MCS8:	ret_rate = HALRF_DATA_RATE_HE_NSS4_MCS8; break;
	case RTW_DATA_RATE_HE_NSS4_MCS9:	ret_rate = HALRF_DATA_RATE_HE_NSS4_MCS9; break;
	case RTW_DATA_RATE_HE_NSS4_MCS10:	ret_rate = HALRF_DATA_RATE_HE_NSS4_MCS10; break;
	case RTW_DATA_RATE_HE_NSS4_MCS11:	ret_rate = HALRF_DATA_RATE_HE_NSS4_MCS11; break; 

	}

	return ret_rate;
}

u16 halrf_get_dcm_offset_pwr_by_rate(struct rf_info *rf, u16 rate,
						u8 dcm, u8 offset)
{
	u16 rate_tmp = 0;

	if (dcm == 0)
		rate_tmp = halrf_hw_rate_to_pwr_by_rate(rf, rate);
	else if (dcm == 1) {
		if (rate == RTW_DATA_RATE_HE_NSS1_MCS0)
			rate_tmp = HALRF_DATA_RATE_HEDCM_NSS1_MCS0;
		else if (rate == RTW_DATA_RATE_HE_NSS1_MCS1)
			rate_tmp = HALRF_DATA_RATE_HEDCM_NSS1_MCS1;
		else if (rate == RTW_DATA_RATE_HE_NSS1_MCS3)
			rate_tmp = HALRF_DATA_RATE_HEDCM_NSS1_MCS3;
		else if (rate == RTW_DATA_RATE_HE_NSS1_MCS4)
			rate_tmp = HALRF_DATA_RATE_HEDCM_NSS1_MCS4;
		else if (rate == RTW_DATA_RATE_HE_NSS2_MCS0)
			rate_tmp = HALRF_DATA_RATE_HEDCM_NSS2_MCS0;
		else if (rate == RTW_DATA_RATE_HE_NSS2_MCS1)
			rate_tmp = HALRF_DATA_RATE_HEDCM_NSS2_MCS1;
		else if (rate == RTW_DATA_RATE_HE_NSS2_MCS3)
			rate_tmp = HALRF_DATA_RATE_HEDCM_NSS2_MCS3;
		else if (rate == RTW_DATA_RATE_HE_NSS2_MCS4)
			rate_tmp = HALRF_DATA_RATE_HEDCM_NSS2_MCS4;
	}

	if (offset == 1) {
		/*if (dcm == 1)*/
		if (rate >= RTW_DATA_RATE_HE_NSS1_MCS0 && rate <= RTW_DATA_RATE_HE_NSS4_MCS11)
			rate_tmp = HALRF_DATA_RATE_HEDCM_OFFSET;
		if (rate >= RTW_DATA_RATE_VHT_NSS1_MCS0 && rate <= RTW_DATA_RATE_VHT_NSS4_MCS9)
			rate_tmp = HALRF_DATA_RATE_VHT_OFFSET;
		if (rate >= RTW_DATA_RATE_MCS0 && rate <= RTW_DATA_RATE_MCS31)
			rate_tmp = HALRF_DATA_RATE_HT_OFFSET;
		if (rate >= RTW_DATA_RATE_OFDM6 && rate <= RTW_DATA_RATE_OFDM54)
			rate_tmp = HALRF_DATA_RATE_OFDM_OFFSET;
		if (rate >= RTW_DATA_RATE_CCK1 && rate <= RTW_DATA_RATE_CCK11)
			rate_tmp = HALRF_DATA_RATE_CCK_OFFSET;
		
	}

	return rate_tmp;

}

u8 halrf_hw_rate_to_limit_rate_tx_num(struct rf_info *rf, u16 rate)
{
	if (rate >= RTW_DATA_RATE_CCK1 && rate <= RTW_DATA_RATE_CCK11)
		return PW_LMT_RS_CCK;
	else if (rate >= RTW_DATA_RATE_OFDM6 && rate <= RTW_DATA_RATE_OFDM54)
		return PW_LMT_RS_OFDM;
	else if (rate >= RTW_DATA_RATE_MCS0 && rate <= RTW_DATA_RATE_MCS7)
		return PW_LMT_RS_HE;
	else if (rate >= RTW_DATA_RATE_MCS8 && rate <= RTW_DATA_RATE_MCS15)
		return PW_LMT_RS_HE;
	else if (rate >= RTW_DATA_RATE_MCS16 && rate <= RTW_DATA_RATE_MCS23)
		return PW_LMT_RS_HE;
	else if (rate >= RTW_DATA_RATE_MCS24 && rate <= RTW_DATA_RATE_MCS31)
		return PW_LMT_RS_HE;
	else if (rate >= RTW_DATA_RATE_VHT_NSS1_MCS0 && rate <= RTW_DATA_RATE_VHT_NSS1_MCS9)
		return PW_LMT_RS_HE;
	else if (rate >= RTW_DATA_RATE_VHT_NSS2_MCS0 && rate <= RTW_DATA_RATE_VHT_NSS2_MCS9)
		return PW_LMT_RS_HE;
	else if (rate >= RTW_DATA_RATE_VHT_NSS3_MCS0 && rate <= RTW_DATA_RATE_VHT_NSS3_MCS9)
		return PW_LMT_RS_HE;
	else if (rate >= RTW_DATA_RATE_VHT_NSS4_MCS0 && rate <= RTW_DATA_RATE_VHT_NSS4_MCS9)
		return PW_LMT_RS_HE;
	else if (rate >= RTW_DATA_RATE_HE_NSS1_MCS0 && rate <= RTW_DATA_RATE_HE_NSS1_MCS11)
		return PW_LMT_RS_HE;
	else if (rate >= RTW_DATA_RATE_HE_NSS2_MCS0 && rate <= RTW_DATA_RATE_HE_NSS2_MCS11)
		return PW_LMT_RS_HE;
	else if (rate >= RTW_DATA_RATE_HE_NSS3_MCS0 && rate <= RTW_DATA_RATE_HE_NSS3_MCS11)
		return PW_LMT_RS_HE;
	else if (rate >= RTW_DATA_RATE_HE_NSS4_MCS0 && rate <= RTW_DATA_RATE_HE_NSS4_MCS11)
		return PW_LMT_RS_HE;
	else
		return PW_LMT_RS_HE;

}

void halrf_power_limit_store_to_array(struct rf_info *rf,
			u8 regulation, u8 band, u8 bandwidth, u8 rate,
			u8 tx_num, u8 beamforming, u8 chnl, s8 val)
{
	struct halrf_pwr_info *pwr = &rf->pwr_info;
	u8 ch = 0;

	RF_DBG(rf, DBG_RF_INIT, "======>%s regulation=%d band=%d bandwidth=%d rate=%d\n",
		__func__, regulation, band, bandwidth, rate);

	RF_DBG(rf, DBG_RF_INIT, "======>%s tx_num=%d beamforming=%d chnl=%d val=%d\n",
		__func__, tx_num, beamforming, chnl, val);

	if (band == PW_LMT_BAND_2_4G) {
		ch = halrf_get_ch_idx_to_limit_array(rf, chnl);
		pwr->tx_pwr_limit_2g[regulation][bandwidth][rate][beamforming][ch][tx_num] = val;		
		RF_DBG(rf, DBG_RF_POWER, "pwr_limit(%d) = pwr->tx_pwr_limit_2g[%d][%d][%d][%d][%d][%d]\n",
			val, regulation, bandwidth, rate, beamforming, ch, tx_num);
	} else if (band == PW_LMT_BAND_5G) {
		ch = halrf_get_ch_idx_to_limit_array(rf, chnl);
		pwr->tx_pwr_limit_5g[regulation][bandwidth][rate][beamforming][ch][tx_num] = val;
		RF_DBG(rf, DBG_RF_POWER, "pwr_limit(%d) = pwr->tx_pwr_limit_5g[%d][%d][%d][%d][%d][%d]\n",
			val, regulation, bandwidth, rate, beamforming, ch, tx_num);
	} else if (band == PW_LMT_BAND_6G) {
		ch = halrf_get_ch_idx_to_6g_limit_array(rf, chnl);
		pwr->tx_pwr_limit_6g[regulation][bandwidth][rate][beamforming][ch][tx_num] = val;
		RF_DBG(rf, DBG_RF_POWER, "pwr_limit(%d) = pwr->tx_pwr_limit_6g[%d][%d][%d][%d][%d][%d]\n",
			val, regulation, bandwidth, rate, beamforming, ch, tx_num);
	}
}

void halrf_power_limit_set_worldwide(struct rf_info *rf)
{
	struct halrf_pwr_info *pwr = &rf->pwr_info;
	u32 reg, bw, rate, bf, ch, tx_num;
	s8 tmp;

	RF_DBG(rf, DBG_RF_INIT, "======>%s\n", __func__);

	/*Power Limit*/
	pwr->regulation[PW_LMT_BAND_2_4G][PW_LMT_REGU_WW13] = true;
	pwr->regulation[PW_LMT_BAND_5G][PW_LMT_REGU_WW13] = true;
	pwr->regulation[PW_LMT_BAND_6G][PW_LMT_REGU_WW13] = true;

	for (tx_num = 0; tx_num < MAX_HALRF_PATH; tx_num++) {
		for (ch = 0; ch < PW_LMT_MAX_CHANNEL_NUMBER_2G; ch++) {
			for (bf = 0; bf < PW_LMT_MAX_BF_NUM; bf++) {
				for (rate = 0; rate < PW_LMT_MAX_RS_NUM; rate++) {
					for (bw = 0; bw < PW_LMT_MAX_2G_BANDWITH_NUM; bw++) {
						tmp = 127;
						for (reg = 0; reg < PW_LMT_MAX_REGULATION_NUM; reg++) {
							RF_DBG(rf, DBG_RF_INIT, "pwr->tx_pwr_limit_2g[%d][%d][%d][%d][%d][%d]=   %d\n",
								reg, bw, rate, bf, ch, tx_num, pwr->tx_pwr_limit_2g[reg][bw][rate][bf][ch][tx_num]);
							if (tmp > pwr->tx_pwr_limit_2g[reg][bw][rate][bf][ch][tx_num] &&
								pwr->tx_pwr_limit_2g[reg][bw][rate][bf][ch][tx_num] != -128) {
								tmp = pwr->tx_pwr_limit_2g[reg][bw][rate][bf][ch][tx_num];
							}
						}
						if (tmp == 127)
							tmp = 0;
						pwr->tx_pwr_limit_2g[PW_LMT_REGU_WW13][bw][rate][bf][ch][tx_num] = tmp;

						RF_DBG(rf, DBG_RF_INIT, "pwr->tx_pwr_limit_2g[%d][%d][%d][%d][%d][%d]=   %d\n",
								PW_LMT_REGU_WW13, bw, rate, bf, ch, tx_num, pwr->tx_pwr_limit_2g[PW_LMT_REGU_WW13][bw][rate][bf][ch][tx_num]);
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
						tmp = 127;
						for (reg = 0; reg < PW_LMT_MAX_REGULATION_NUM; reg++) {
							RF_DBG(rf, DBG_RF_INIT, "pwr->tx_pwr_limit_5g[%d][%d][%d][%d][%d][%d]=   %d\n",
								reg, bw, rate, bf, ch, tx_num, pwr->tx_pwr_limit_5g[reg][bw][rate][bf][ch][tx_num]);
							if (tmp > pwr->tx_pwr_limit_5g[reg][bw][rate][bf][ch][tx_num] &&
								pwr->tx_pwr_limit_5g[reg][bw][rate][bf][ch][tx_num] != -128) {
								tmp = pwr->tx_pwr_limit_5g[reg][bw][rate][bf][ch][tx_num];
							}
						}
						if (tmp == 127)
							tmp = 0;
						pwr->tx_pwr_limit_5g[PW_LMT_REGU_WW13][bw][rate][bf][ch][tx_num] = tmp;

						RF_DBG(rf, DBG_RF_INIT, "pwr->tx_pwr_limit_5g[%d][%d][%d][%d][%d][%d]=   %d\n",
								PW_LMT_REGU_WW13, bw, rate, bf, ch, tx_num, pwr->tx_pwr_limit_5g[PW_LMT_REGU_WW13][bw][rate][bf][ch][tx_num]);
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
						tmp = 127;
						for (reg = 0; reg < PW_LMT_MAX_REGULATION_NUM; reg++) {
							RF_DBG(rf, DBG_RF_INIT, "pwr->tx_pwr_limit_6g[%d][%d][%d][%d][%d][%d]=   %d\n",
								reg, bw, rate, bf, ch, tx_num, pwr->tx_pwr_limit_6g[reg][bw][rate][bf][ch][tx_num]);
							if (tmp > pwr->tx_pwr_limit_6g[reg][bw][rate][bf][ch][tx_num] &&
								pwr->tx_pwr_limit_6g[reg][bw][rate][bf][ch][tx_num] != -128) {
								tmp = pwr->tx_pwr_limit_6g[reg][bw][rate][bf][ch][tx_num];
							}
						}
						if (tmp == 127)
							tmp = 0;
						pwr->tx_pwr_limit_6g[PW_LMT_REGU_WW13][bw][rate][bf][ch][tx_num] = tmp;

						RF_DBG(rf, DBG_RF_INIT, "pwr->tx_pwr_limit_6g[%d][%d][%d][%d][%d][%d]=   %d\n",
								PW_LMT_REGU_WW13, bw, rate, bf, ch, tx_num, pwr->tx_pwr_limit_6g[PW_LMT_REGU_WW13][bw][rate][bf][ch][tx_num]);
					}
				}
			}
		}
	}

	RF_DBG(rf, DBG_RF_INIT, "<======%s finish!!!\n", __func__);

}

void halrf_power_limit_ru_store_to_array(struct rf_info *rf,
			u8 band, u8 bandwidth, u8 tx_num, u8 rate,
			u8 regulation, u8 chnl, s8 val)
{
	struct halrf_pwr_info *pwr = &rf->pwr_info;
	u8 ch = 0;

	RF_DBG(rf, DBG_RF_INIT, "======>%s regulation=%d band=%d bandwidth=%d rate=%d\n",
		__func__, regulation, band, bandwidth, rate);

	RF_DBG(rf, DBG_RF_INIT, "======>%s tx_num=%d chnl=%d val=%d\n",
		__func__, tx_num, chnl, val);

	if (band == PW_LMT_BAND_2_4G) {
		ch = halrf_get_ch_idx_to_limit_array(rf, chnl);
		pwr->tx_pwr_limit_ru_2g[regulation][bandwidth][rate][ch][tx_num] = val;		
		RF_DBG(rf, DBG_RF_POWER, "pwr_limit_ru(%d) = pwr->tx_pwr_limit_ru_2g[%d][%d][%d][%d][%d]\n",
			val, regulation, bandwidth, rate, ch, tx_num);
	} else if (band == PW_LMT_BAND_5G) {
		ch = halrf_get_ch_idx_to_limit_array(rf, chnl);
		pwr->tx_pwr_limit_ru_5g[regulation][bandwidth][rate][ch][tx_num] = val;
		RF_DBG(rf, DBG_RF_POWER, "pwr_limit_ru(%d) = pwr->tx_pwr_limit_ru_5g[%d][%d][%d][%d][%d]\n",
			val, regulation, bandwidth, rate, ch, tx_num);
	} else if (band == PW_LMT_BAND_6G) {
		ch = halrf_get_ch_idx_to_6g_limit_array(rf, chnl);
		pwr->tx_pwr_limit_ru_6g[regulation][bandwidth][rate][ch][tx_num] = val;
		RF_DBG(rf, DBG_RF_POWER, "pwr_limit_ru(%d) = pwr->tx_pwr_limit_ru_6g[%d][%d][%d][%d][%d]\n",
			val, regulation, bandwidth, rate, ch, tx_num);
	}
}

void halrf_power_limit_ru_set_worldwide(struct rf_info *rf)
{
	struct halrf_pwr_info *pwr = &rf->pwr_info;
	u32 reg, bw, rate, ch, tx_num;
	s8 tmp;

	RF_DBG(rf, DBG_RF_INIT, "======>%s\n", __func__);

	for (tx_num = 0; tx_num < MAX_HALRF_PATH; tx_num++) {
		for (ch = 0; ch < PW_LMT_MAX_CHANNEL_NUMBER_2G; ch++) {
			for (rate = 0; rate < PW_LMT_MAX_RS_NUM; rate++) {
				for (bw = 0; bw < PW_LMT_RU_BW_NULL; bw++) {
					tmp = 127;
					for (reg = 0; reg < PW_LMT_MAX_REGULATION_NUM; reg++) {
						RF_DBG(rf, DBG_RF_INIT, "pwr->tx_pwr_limit_ru_2g[%d][%d][%d][%d][%d]=   %d\n",
							reg, bw, rate, ch, tx_num, pwr->tx_pwr_limit_ru_2g[reg][bw][rate][ch][tx_num]);

						if (tmp > pwr->tx_pwr_limit_ru_2g[reg][bw][rate][ch][tx_num] &&
							pwr->tx_pwr_limit_ru_2g[reg][bw][rate][ch][tx_num] != -128) {
							tmp = pwr->tx_pwr_limit_ru_2g[reg][bw][rate][ch][tx_num];
						}
					}
					if (tmp == 127)
						tmp = 0;
					pwr->tx_pwr_limit_ru_2g[PW_LMT_REGU_WW13][bw][rate][ch][tx_num] = tmp;

					RF_DBG(rf, DBG_RF_INIT, "pwr->tx_pwr_limit_ru_2g[%d][%d][%d][%d][%d]=   %d\n",
							PW_LMT_REGU_WW13, bw, rate, ch, tx_num, pwr->tx_pwr_limit_ru_2g[PW_LMT_REGU_WW13][bw][rate][ch][tx_num]);

				}
			}
		}
	}

	for (tx_num = 0; tx_num < MAX_HALRF_PATH; tx_num++) {
		for (ch = 0; ch < PW_LMT_MAX_CHANNEL_NUMBER_5G; ch++) {
			for (rate = 0; rate < PW_LMT_MAX_RS_NUM; rate++) {
				for (bw = 0; bw < PW_LMT_RU_BW_NULL; bw++) {
					tmp = 127;
					for (reg = 0; reg < PW_LMT_MAX_REGULATION_NUM; reg++) {
						RF_DBG(rf, DBG_RF_INIT, "pwr->tx_pwr_limit_ru_5g[%d][%d][%d][%d][%d]=   %d\n",
							reg, bw, rate, ch, tx_num, pwr->tx_pwr_limit_ru_5g[reg][bw][rate][ch][tx_num]);

						if (tmp > pwr->tx_pwr_limit_ru_5g[reg][bw][rate][ch][tx_num] &&
							pwr->tx_pwr_limit_ru_5g[reg][bw][rate][ch][tx_num] != -128) {
							tmp = pwr->tx_pwr_limit_ru_5g[reg][bw][rate][ch][tx_num];
						}
					}
					if (tmp == 127)
						tmp = 0;
					pwr->tx_pwr_limit_ru_5g[PW_LMT_REGU_WW13][bw][rate][ch][tx_num] = tmp;

					RF_DBG(rf, DBG_RF_INIT, "pwr->tx_pwr_limit_ru_5g[%d][%d][%d][%d][%d]=   %d\n",
							PW_LMT_REGU_WW13, bw, rate, ch, tx_num, pwr->tx_pwr_limit_ru_5g[PW_LMT_REGU_WW13][bw][rate][ch][tx_num]);

				}
			}
		}
	}

	for (tx_num = 0; tx_num < MAX_HALRF_PATH; tx_num++) {
		for (ch = 0; ch < PW_LMT_MAX_CHANNEL_NUMBER_6G; ch++) {
			for (rate = 0; rate < PW_LMT_MAX_RS_NUM; rate++) {
				for (bw = 0; bw < PW_LMT_RU_BW_NULL; bw++) {
					tmp = 127;
					for (reg = 0; reg < PW_LMT_MAX_REGULATION_NUM; reg++) {
						RF_DBG(rf, DBG_RF_INIT, "pwr->tx_pwr_limit_ru_6g[%d][%d][%d][%d][%d]=   %d\n",
							reg, bw, rate, ch, tx_num, pwr->tx_pwr_limit_ru_6g[reg][bw][rate][ch][tx_num]);

						if (tmp > pwr->tx_pwr_limit_ru_6g[reg][bw][rate][ch][tx_num] &&
							pwr->tx_pwr_limit_ru_6g[reg][bw][rate][ch][tx_num] != -128) {
							tmp = pwr->tx_pwr_limit_ru_6g[reg][bw][rate][ch][tx_num];
						}
					}
					if (tmp == 127)
						tmp = 0;
					pwr->tx_pwr_limit_ru_6g[PW_LMT_REGU_WW13][bw][rate][ch][tx_num] = tmp;

					RF_DBG(rf, DBG_RF_INIT, "pwr->tx_pwr_limit_ru_6g[%d][%d][%d][%d][%d]=   %d\n",
							PW_LMT_REGU_WW13, bw, rate, ch, tx_num, pwr->tx_pwr_limit_ru_6g[PW_LMT_REGU_WW13][bw][rate][ch][tx_num]);

				}
			}
		}
	}

	RF_DBG(rf, DBG_RF_INIT, "<======%s finish!!!\n", __func__);

}

void halrf_power_limit_set_ext_pwr_limit_table(struct rf_info *rf,
							enum phl_phy_idx phy)
{
#ifdef SPF_PHL_RF_019_SAR
	struct halrf_pwr_info *pwr = &rf->pwr_info;
	struct rtw_hal_com_t *hal = rf->hal_com;
	struct rtw_tpu_info *tpu = &rf->hal_com->band[phy].rtw_tpu_i;
	struct rtw_phl_ext_pwr_lmt_info *ext_pwr_info = &rf->hal_com->band[phy].rtw_tpu_i.ext_pwr_lmt_i;

	u32 reg, bw, rate, bf, ch, tx_num;
	s8 ext_pwr = 0;
	u8 real_ch;

	RF_DBG(rf, DBG_RF_INIT, "======>%s   phy=%d\n", __func__, phy);

	/*Power Limit*/
	pwr->regulation[PW_LMT_BAND_2_4G][PW_LMT_REGU_EXT_PWR] = true;
	pwr->regulation[PW_LMT_BAND_5G][PW_LMT_REGU_EXT_PWR] = true;
	pwr->regulation[PW_LMT_BAND_6G][PW_LMT_REGU_EXT_PWR] = true;

	reg = (u32)halrf_get_regulation_info(rf, BAND_ON_24G);

	hal_mem_set(hal, pwr->ext_pwr_diff_2_4g, 0, sizeof(pwr->ext_pwr_diff_2_4g));
	hal_mem_set(hal, pwr->ext_pwr_diff_5g_band1, 0, sizeof(pwr->ext_pwr_diff_5g_band1));
	hal_mem_set(hal, pwr->ext_pwr_diff_5g_band2, 0, sizeof(pwr->ext_pwr_diff_5g_band2));
	hal_mem_set(hal, pwr->ext_pwr_diff_5g_band3, 0, sizeof(pwr->ext_pwr_diff_5g_band3));
	hal_mem_set(hal, pwr->ext_pwr_diff_5g_band4, 0, sizeof(pwr->ext_pwr_diff_5g_band4));

	if (ext_pwr_info->ext_pwr_lmt_ant_2_4g[RF_PATH_A] > ext_pwr_info->ext_pwr_lmt_ant_2_4g[RF_PATH_B]) {
		pwr->ext_pwr_diff_2_4g[RF_PATH_B] =
			ext_pwr_info->ext_pwr_lmt_ant_2_4g[RF_PATH_B] - ext_pwr_info->ext_pwr_lmt_ant_2_4g[RF_PATH_A];
	} else {
		pwr->ext_pwr_diff_2_4g[RF_PATH_A] =
			ext_pwr_info->ext_pwr_lmt_ant_2_4g[RF_PATH_A] - ext_pwr_info->ext_pwr_lmt_ant_2_4g[RF_PATH_B];
	}

	RF_DBG(rf, DBG_RF_INIT, "ext_pwr_info->ext_pwr_lmt_2_4g[RF_PATH_A]=%d\n",
		ext_pwr_info->ext_pwr_lmt_ant_2_4g[RF_PATH_A]);

	RF_DBG(rf, DBG_RF_INIT, "ext_pwr_info->ext_pwr_lmt_2_4g[RF_PATH_B]=%d\n",
		ext_pwr_info->ext_pwr_lmt_ant_2_4g[RF_PATH_B]);

	RF_DBG(rf, DBG_RF_INIT, "pwr->ext_pwr_diff_2_4g[RF_PATH_A]=%d pwr->ext_pwr_diff_2_4g[RF_PATH_B]=%d\n",
		pwr->ext_pwr_diff_2_4g[RF_PATH_A], pwr->ext_pwr_diff_2_4g[RF_PATH_B]);

	for (tx_num = 0; tx_num < 2; tx_num++) {
		ext_pwr = ext_pwr_info->ext_pwr_lmt_2_4g[tx_num];
		for (ch = 0; ch < PW_LMT_MAX_CHANNEL_NUMBER_2G; ch++) {
			for (bf = 0; bf < PW_LMT_MAX_BF_NUM; bf++) {
				for (rate = 0; rate < PW_LMT_MAX_RS_NUM; rate++) {
					for (bw = 0; bw < PW_LMT_MAX_2G_BANDWITH_NUM; bw++) {
						
						RF_DBG(rf, DBG_RF_INIT, "ext_pwr_lmt_2_4g[%d]=%d   pwr->tx_pwr_limit_2g[%d][%d][%d][%d][%d][%d]=   %d\n",
							tx_num, ext_pwr,
							reg, bw, rate, bf, ch, tx_num , pwr->tx_pwr_limit_2g[reg][bw][rate][bf][ch][tx_num]);

						if (tpu->ext_pwr_lmt_en == true) {
							if (pwr->tx_pwr_limit_2g[reg][bw][rate][bf][ch][tx_num] != -128) {
								if (pwr->tx_pwr_limit_2g[reg][bw][rate][bf][ch][tx_num] < ext_pwr) {
									pwr->tx_pwr_limit_2g[PW_LMT_REGU_EXT_PWR][bw][rate][bf][ch][tx_num] =
										pwr->tx_pwr_limit_2g[reg][bw][rate][bf][ch][tx_num];
								} else {
									pwr->tx_pwr_limit_2g[PW_LMT_REGU_EXT_PWR][bw][rate][bf][ch][tx_num] = ext_pwr;
								}
							} else {
								pwr->tx_pwr_limit_2g[PW_LMT_REGU_EXT_PWR][bw][rate][bf][ch][tx_num] = ext_pwr;
							}
						} else {
							pwr->tx_pwr_limit_2g[PW_LMT_REGU_EXT_PWR][bw][rate][bf][ch][tx_num] =
								pwr->tx_pwr_limit_2g[reg][bw][rate][bf][ch][tx_num];
						}

						RF_DBG(rf, DBG_RF_INIT, "pwr->tx_pwr_limit_2g[%d][%d][%d][%d][%d][%d] =   %d\n",
								PW_LMT_REGU_EXT_PWR, bw, rate, bf, ch, tx_num ,
								pwr->tx_pwr_limit_2g[PW_LMT_REGU_EXT_PWR][bw][rate][bf][ch][tx_num]);
					}
				}
			}
		}
	}

	reg = (u32)halrf_get_regulation_info(rf, BAND_ON_5G);

	if (ext_pwr_info->ext_pwr_lmt_ant_5g_band1[RF_PATH_A] > ext_pwr_info->ext_pwr_lmt_ant_5g_band1[RF_PATH_B]) {
		pwr->ext_pwr_diff_5g_band1[RF_PATH_B] =
			ext_pwr_info->ext_pwr_lmt_ant_5g_band1[RF_PATH_B] - ext_pwr_info->ext_pwr_lmt_ant_5g_band1[RF_PATH_A];
	} else {
		pwr->ext_pwr_diff_5g_band1[RF_PATH_A] =
			ext_pwr_info->ext_pwr_lmt_ant_5g_band1[RF_PATH_A] - ext_pwr_info->ext_pwr_lmt_ant_5g_band1[RF_PATH_B];
	}

	if (ext_pwr_info->ext_pwr_lmt_ant_5g_band2[RF_PATH_A] > ext_pwr_info->ext_pwr_lmt_ant_5g_band2[RF_PATH_B]) {
		pwr->ext_pwr_diff_5g_band2[RF_PATH_B] =
			ext_pwr_info->ext_pwr_lmt_ant_5g_band2[RF_PATH_B] - ext_pwr_info->ext_pwr_lmt_ant_5g_band2[RF_PATH_A];
	} else {
		pwr->ext_pwr_diff_5g_band2[RF_PATH_A] =
			ext_pwr_info->ext_pwr_lmt_ant_5g_band2[RF_PATH_A] - ext_pwr_info->ext_pwr_lmt_ant_5g_band2[RF_PATH_B];
	}

	if (ext_pwr_info->ext_pwr_lmt_ant_5g_band3[RF_PATH_A] > ext_pwr_info->ext_pwr_lmt_ant_5g_band3[RF_PATH_B]) {
		pwr->ext_pwr_diff_5g_band3[RF_PATH_B] =
			ext_pwr_info->ext_pwr_lmt_ant_5g_band3[RF_PATH_B] - ext_pwr_info->ext_pwr_lmt_ant_5g_band3[RF_PATH_A];
	} else {
		pwr->ext_pwr_diff_5g_band3[RF_PATH_A] =
			ext_pwr_info->ext_pwr_lmt_ant_5g_band3[RF_PATH_A] - ext_pwr_info->ext_pwr_lmt_ant_5g_band3[RF_PATH_B];
	}

	if (ext_pwr_info->ext_pwr_lmt_ant_5g_band4[RF_PATH_A] > ext_pwr_info->ext_pwr_lmt_ant_5g_band4[RF_PATH_B]) {
		pwr->ext_pwr_diff_5g_band4[RF_PATH_B] =
			ext_pwr_info->ext_pwr_lmt_ant_5g_band4[RF_PATH_B] - ext_pwr_info->ext_pwr_lmt_ant_5g_band4[RF_PATH_A];
	} else {
		pwr->ext_pwr_diff_5g_band4[RF_PATH_A] =
			ext_pwr_info->ext_pwr_lmt_ant_5g_band4[RF_PATH_A] - ext_pwr_info->ext_pwr_lmt_ant_5g_band4[RF_PATH_B];
	}

	RF_DBG(rf, DBG_RF_INIT, "ext_pwr_lmt_5g_band1[RF_PATH_A]=%d  ext_pwr_lmt_5g_band1[RF_PATH_B]=%d\n",
		ext_pwr_info->ext_pwr_lmt_ant_5g_band1[RF_PATH_A],
		ext_pwr_info->ext_pwr_lmt_ant_5g_band1[RF_PATH_B]);

	RF_DBG(rf, DBG_RF_INIT, "pwr->ext_pwr_diff_5g_band1[RF_PATH_A]=%d pwr->ext_pwr_diff_5g_band1[RF_PATH_B]=%d\n",
		pwr->ext_pwr_diff_5g_band1[RF_PATH_A], pwr->ext_pwr_diff_5g_band1[RF_PATH_B]);
	
	RF_DBG(rf, DBG_RF_INIT, "ext_pwr_lmt_5g_band2[RF_PATH_A]=%d   ext_pwr_lmt_5g_band2[RF_PATH_B]=%d\n",
		ext_pwr_info->ext_pwr_lmt_ant_5g_band2[RF_PATH_A],
		ext_pwr_info->ext_pwr_lmt_ant_5g_band2[RF_PATH_B]);

	RF_DBG(rf, DBG_RF_INIT, "pwr->ext_pwr_diff_5g_band2[RF_PATH_A]=%d pwr->ext_pwr_diff_5g_band2[RF_PATH_B]=%d\n",
		pwr->ext_pwr_diff_5g_band2[RF_PATH_A], pwr->ext_pwr_diff_5g_band2[RF_PATH_B]);

	RF_DBG(rf, DBG_RF_INIT, "ext_pwr_lmt_5g_band3[RF_PATH_A]=%d   ext_pwr_lmt_5g_band3[RF_PATH_B]=%d\n",
		ext_pwr_info->ext_pwr_lmt_ant_5g_band3[RF_PATH_A],
		ext_pwr_info->ext_pwr_lmt_ant_5g_band3[RF_PATH_B]);

	RF_DBG(rf, DBG_RF_INIT, "pwr->ext_pwr_diff_5g_band3[RF_PATH_A]=%d pwr->ext_pwr_diff_5g_band3[RF_PATH_B]=%d\n",
		pwr->ext_pwr_diff_5g_band3[RF_PATH_A], pwr->ext_pwr_diff_5g_band3[RF_PATH_B]);

	RF_DBG(rf, DBG_RF_INIT, "ext_pwr_lmt_5g_band4[RF_PATH_A]=%d   ext_pwr_lmt_5g_band4[RF_PATH_B]=%d\n",
		ext_pwr_info->ext_pwr_lmt_ant_5g_band4[RF_PATH_A],
		ext_pwr_info->ext_pwr_lmt_ant_5g_band4[RF_PATH_B]);

	RF_DBG(rf, DBG_RF_INIT, "pwr->ext_pwr_diff_5g_band4[RF_PATH_A]=%d pwr->ext_pwr_diff_5g_band4[RF_PATH_B]=%d\n",
		pwr->ext_pwr_diff_5g_band4[RF_PATH_A], pwr->ext_pwr_diff_5g_band4[RF_PATH_B]);

	for (tx_num = 0; tx_num < 2; tx_num++) {
		for (ch = 0; ch < PW_LMT_MAX_CHANNEL_NUMBER_5G; ch++) {
			real_ch = halrf_get_limit_ch_idx_to_ch_idx(rf, PW_LMT_BAND_5G, (u8)ch);
			if (real_ch >= 36 && real_ch <= 48)
				ext_pwr = ext_pwr_info->ext_pwr_lmt_5g_band1[tx_num];
			else if (real_ch >= 50 && real_ch <= 64)
				ext_pwr = ext_pwr_info->ext_pwr_lmt_5g_band2[tx_num];
			else if (real_ch >= 100 && real_ch <= 144)
				ext_pwr = ext_pwr_info->ext_pwr_lmt_5g_band3[tx_num];
			else if (real_ch >= 149 && real_ch <= 177)
				ext_pwr = ext_pwr_info->ext_pwr_lmt_5g_band4[tx_num];

			for (bf = 0; bf < PW_LMT_MAX_BF_NUM; bf++) {
				for (rate = 0; rate < PW_LMT_MAX_RS_NUM; rate++) {
					for (bw = 0; bw < PW_LMT_MAX_BANDWIDTH_NUM; bw++) {

						RF_DBG(rf, DBG_RF_INIT, "ext_pwr_lmt_5g[%d]=%d   pwr->tx_pwr_limit_5g[%d][%d][%d][%d][%d][%d]=   %d\n",
							tx_num, ext_pwr,
							reg, bw, rate, bf, ch, tx_num , pwr->tx_pwr_limit_5g[reg][bw][rate][bf][ch][tx_num]);

						if (tpu->ext_pwr_lmt_en == true) {
							if (pwr->tx_pwr_limit_5g[reg][bw][rate][bf][ch][tx_num] != -128) {
								if (pwr->tx_pwr_limit_5g[reg][bw][rate][bf][ch][tx_num] < ext_pwr) {
									pwr->tx_pwr_limit_5g[PW_LMT_REGU_EXT_PWR][bw][rate][bf][ch][tx_num] =
										pwr->tx_pwr_limit_5g[reg][bw][rate][bf][ch][tx_num];
								} else {
									pwr->tx_pwr_limit_5g[PW_LMT_REGU_EXT_PWR][bw][rate][bf][ch][tx_num] = ext_pwr;
								}
							} else {
								pwr->tx_pwr_limit_5g[PW_LMT_REGU_EXT_PWR][bw][rate][bf][ch][tx_num] = ext_pwr;
							}
						} else {
							pwr->tx_pwr_limit_5g[PW_LMT_REGU_EXT_PWR][bw][rate][bf][ch][tx_num] =
								pwr->tx_pwr_limit_5g[reg][bw][rate][bf][ch][tx_num];
						}

						RF_DBG(rf, DBG_RF_INIT, "pwr->tx_pwr_limit_5g[%d][%d][%d][%d][%d][%d]=   %d\n",
							PW_LMT_REGU_EXT_PWR, bw, rate, bf, ch, tx_num,
							pwr->tx_pwr_limit_5g[PW_LMT_REGU_EXT_PWR][bw][rate][bf][ch][tx_num]);
					}
				}
			}
		}
	}

	if (ext_pwr_info->ext_pwr_lmt_6g_unii_5_1[RF_PATH_A] > ext_pwr_info->ext_pwr_lmt_6g_unii_5_1[RF_PATH_B]) {
		pwr->ext_pwr_diff_lmt_6g_unii_5_1[RF_PATH_B] =
			ext_pwr_info->ext_pwr_lmt_6g_unii_5_1[RF_PATH_B] - ext_pwr_info->ext_pwr_lmt_6g_unii_5_1[RF_PATH_A];
	} else {
		pwr->ext_pwr_diff_lmt_6g_unii_5_1[RF_PATH_A] =
			ext_pwr_info->ext_pwr_lmt_6g_unii_5_1[RF_PATH_A] - ext_pwr_info->ext_pwr_lmt_6g_unii_5_1[RF_PATH_B];
	}

	if (ext_pwr_info->ext_pwr_lmt_6g_unii_5_2[RF_PATH_A] > ext_pwr_info->ext_pwr_lmt_6g_unii_5_2[RF_PATH_B]) {
		pwr->ext_pwr_diff_lmt_6g_unii_5_2[RF_PATH_B] =
			ext_pwr_info->ext_pwr_lmt_6g_unii_5_2[RF_PATH_B] - ext_pwr_info->ext_pwr_lmt_6g_unii_5_2[RF_PATH_A];
	} else {
		pwr->ext_pwr_diff_lmt_6g_unii_5_2[RF_PATH_A] =
			ext_pwr_info->ext_pwr_lmt_6g_unii_5_2[RF_PATH_A] - ext_pwr_info->ext_pwr_lmt_6g_unii_5_2[RF_PATH_B];
	}

	if (ext_pwr_info->ext_pwr_lmt_6g_unii_6[RF_PATH_A] > ext_pwr_info->ext_pwr_lmt_6g_unii_6[RF_PATH_B]) {
		pwr->ext_pwr_diff_lmt_6g_unii_6[RF_PATH_B] =
			ext_pwr_info->ext_pwr_lmt_6g_unii_6[RF_PATH_B] - ext_pwr_info->ext_pwr_lmt_6g_unii_6[RF_PATH_A];
	} else {
		pwr->ext_pwr_diff_lmt_6g_unii_6[RF_PATH_A] =
			ext_pwr_info->ext_pwr_lmt_6g_unii_6[RF_PATH_A] - ext_pwr_info->ext_pwr_lmt_6g_unii_6[RF_PATH_B];
	}

	if (ext_pwr_info->ext_pwr_lmt_6g_unii_7_1[RF_PATH_A] > ext_pwr_info->ext_pwr_lmt_6g_unii_7_1[RF_PATH_B]) {
		pwr->ext_pwr_diff_lmt_6g_unii_7_1[RF_PATH_B] =
			ext_pwr_info->ext_pwr_lmt_6g_unii_7_1[RF_PATH_B] - ext_pwr_info->ext_pwr_lmt_6g_unii_7_1[RF_PATH_A];
	} else {
		pwr->ext_pwr_diff_lmt_6g_unii_7_1[RF_PATH_A] =
			ext_pwr_info->ext_pwr_lmt_6g_unii_7_1[RF_PATH_A] - ext_pwr_info->ext_pwr_lmt_6g_unii_7_1[RF_PATH_B];
	}

	if (ext_pwr_info->ext_pwr_lmt_6g_unii_7_2[RF_PATH_A] > ext_pwr_info->ext_pwr_lmt_6g_unii_7_2[RF_PATH_B]) {
		pwr->ext_pwr_diff_lmt_6g_unii_7_2[RF_PATH_B] =
			ext_pwr_info->ext_pwr_lmt_6g_unii_7_2[RF_PATH_B] - ext_pwr_info->ext_pwr_lmt_6g_unii_7_2[RF_PATH_A];
	} else {
		pwr->ext_pwr_diff_lmt_6g_unii_7_2[RF_PATH_A] =
			ext_pwr_info->ext_pwr_lmt_6g_unii_7_2[RF_PATH_A] - ext_pwr_info->ext_pwr_lmt_6g_unii_7_2[RF_PATH_B];
	}
	
	if (ext_pwr_info->ext_pwr_lmt_6g_unii_8[RF_PATH_A] > ext_pwr_info->ext_pwr_lmt_6g_unii_8[RF_PATH_B]) {
		pwr->ext_pwr_diff_lmt_6g_unii_8[RF_PATH_B] =
			ext_pwr_info->ext_pwr_lmt_6g_unii_8[RF_PATH_B] - ext_pwr_info->ext_pwr_lmt_6g_unii_8[RF_PATH_A];
	} else {
		pwr->ext_pwr_diff_lmt_6g_unii_8[RF_PATH_A] =
			ext_pwr_info->ext_pwr_lmt_6g_unii_8[RF_PATH_A] - ext_pwr_info->ext_pwr_lmt_6g_unii_8[RF_PATH_B];
	}

	RF_DBG(rf, DBG_RF_INIT, "ext_pwr_lmt_6g_unii_5_1[RF_PATH_A]=%d  ext_pwr_lmt_6g_unii_5_1[RF_PATH_B]=%d\n",
		ext_pwr_info->ext_pwr_lmt_6g_unii_5_1[RF_PATH_A],
		ext_pwr_info->ext_pwr_lmt_6g_unii_5_1[RF_PATH_B]);

	RF_DBG(rf, DBG_RF_INIT, "pwr->ext_pwr_diff_lmt_6g_unii_5_1[RF_PATH_A]=%d pwr->ext_pwr_diff_lmt_6g_unii_5_1[RF_PATH_B]=%d\n",
		pwr->ext_pwr_diff_lmt_6g_unii_5_1[RF_PATH_A], pwr->ext_pwr_diff_lmt_6g_unii_5_1[RF_PATH_B]);
	
	RF_DBG(rf, DBG_RF_INIT, "ext_pwr_lmt_6g_unii_5_2[RF_PATH_A]=%d   ext_pwr_lmt_6g_unii_5_2[RF_PATH_B]=%d\n",
		ext_pwr_info->ext_pwr_lmt_6g_unii_5_2[RF_PATH_A],
		ext_pwr_info->ext_pwr_lmt_6g_unii_5_2[RF_PATH_B]);

	RF_DBG(rf, DBG_RF_INIT, "pwr->ext_pwr_diff_lmt_6g_unii_5_2[RF_PATH_A]=%d pwr->ext_pwr_diff_lmt_6g_unii_5_2[RF_PATH_B]=%d\n",
		pwr->ext_pwr_diff_lmt_6g_unii_5_2[RF_PATH_A], pwr->ext_pwr_diff_lmt_6g_unii_5_2[RF_PATH_B]);

	RF_DBG(rf, DBG_RF_INIT, "ext_pwr_lmt_6g_unii_6[RF_PATH_A]=%d   ext_pwr_lmt_6g_unii_6[RF_PATH_B]=%d\n",
		ext_pwr_info->ext_pwr_lmt_6g_unii_6[RF_PATH_A],
		ext_pwr_info->ext_pwr_lmt_6g_unii_6[RF_PATH_B]);

	RF_DBG(rf, DBG_RF_INIT, "pwr->ext_pwr_diff_lmt_6g_unii_6[RF_PATH_A]=%d pwr->ext_pwr_diff_lmt_6g_unii_6[RF_PATH_B]=%d\n",
		pwr->ext_pwr_diff_lmt_6g_unii_6[RF_PATH_A], pwr->ext_pwr_diff_lmt_6g_unii_6[RF_PATH_B]);

	RF_DBG(rf, DBG_RF_INIT, "ext_pwr_lmt_6g_unii_7_1[RF_PATH_A]=%d   ext_pwr_lmt_6g_unii_7_1[RF_PATH_B]=%d\n",
		ext_pwr_info->ext_pwr_lmt_6g_unii_7_1[RF_PATH_A],
		ext_pwr_info->ext_pwr_lmt_6g_unii_7_1[RF_PATH_B]);

	RF_DBG(rf, DBG_RF_INIT, "pwr->ext_pwr_diff_lmt_6g_unii_7_1[RF_PATH_A]=%d pwr->ext_pwr_diff_lmt_6g_unii_7_1[RF_PATH_B]=%d\n",
		pwr->ext_pwr_diff_lmt_6g_unii_7_1[RF_PATH_A], pwr->ext_pwr_diff_lmt_6g_unii_7_1[RF_PATH_B]);
		
	RF_DBG(rf, DBG_RF_INIT, "ext_pwr_lmt_6g_unii_7_2[RF_PATH_A]=%d   ext_pwr_lmt_6g_unii_7_2[RF_PATH_B]=%d\n",
		ext_pwr_info->ext_pwr_lmt_6g_unii_7_2[RF_PATH_A],
		ext_pwr_info->ext_pwr_lmt_6g_unii_7_2[RF_PATH_B]);
	
	RF_DBG(rf, DBG_RF_INIT, "pwr->ext_pwr_diff_lmt_6g_unii_7_2[RF_PATH_A]=%d pwr->ext_pwr_diff_lmt_6g_unii_7_2[RF_PATH_B]=%d\n",
		pwr->ext_pwr_diff_lmt_6g_unii_7_2[RF_PATH_A], pwr->ext_pwr_diff_lmt_6g_unii_7_2[RF_PATH_B]);		
			
	RF_DBG(rf, DBG_RF_INIT, "ext_pwr_lmt_6g_unii_8[RF_PATH_A]=%d   ext_pwr_lmt_6g_unii_8[RF_PATH_B]=%d\n",
		ext_pwr_info->ext_pwr_lmt_6g_unii_8[RF_PATH_A],
		ext_pwr_info->ext_pwr_lmt_6g_unii_8[RF_PATH_B]);
	
	RF_DBG(rf, DBG_RF_INIT, "pwr->ext_pwr_diff_lmt_6g_unii_8[RF_PATH_A]=%d pwr->ext_pwr_diff_lmt_6g_unii_8[RF_PATH_B]=%d\n",
		pwr->ext_pwr_diff_lmt_6g_unii_8[RF_PATH_A], pwr->ext_pwr_diff_lmt_6g_unii_8[RF_PATH_B]);		

	reg = (u32)halrf_get_regulation_info(rf, BAND_ON_6G);

	for (tx_num = 0; tx_num < 2; tx_num++) {
		for (ch = 0; ch < PW_LMT_MAX_CHANNEL_NUMBER_6G; ch++) {
			real_ch = halrf_get_limit_ch_idx_to_ch_idx(rf, PW_LMT_BAND_6G, (u8)ch);

			if (real_ch >= 1 && real_ch <= 45)
				ext_pwr = ext_pwr_info->ext_pwr_lmt_6g_unii_5_1[tx_num];
			else if (real_ch >= 49 && real_ch <= 93)
				ext_pwr = ext_pwr_info->ext_pwr_lmt_6g_unii_5_2[tx_num];
			else if (real_ch >= 97 && real_ch <= 117)
				ext_pwr = ext_pwr_info->ext_pwr_lmt_6g_unii_6[tx_num];
			else if (real_ch >= 121 && real_ch <= 149)
				ext_pwr = ext_pwr_info->ext_pwr_lmt_6g_unii_7_1[tx_num];
			else if (real_ch >= 153 && real_ch <= 189)
				ext_pwr = ext_pwr_info->ext_pwr_lmt_6g_unii_7_2[tx_num];
			else if (real_ch >= 193 && real_ch <= 237)
				ext_pwr = ext_pwr_info->ext_pwr_lmt_6g_unii_8[tx_num];
			else if (real_ch >= 241 && real_ch <= 253)
				ext_pwr = ext_pwr_info->ext_pwr_lmt_6g_unii_8[tx_num];

			for (bf = 0; bf < PW_LMT_MAX_BF_NUM; bf++) {
				for (rate = 0; rate < PW_LMT_MAX_RS_NUM; rate++) {
					for (bw = 0; bw < PW_LMT_MAX_BANDWIDTH_NUM; bw++) {

						RF_DBG(rf, DBG_RF_INIT, "ext_pwr_lmt_6g[%d]=%d   pwr->tx_pwr_limit_6g[%d][%d][%d][%d][%d][%d]=   %d\n",
							tx_num, ext_pwr,
							reg, bw, rate, bf, ch, tx_num , pwr->tx_pwr_limit_6g[reg][bw][rate][bf][ch][tx_num]);

						if (tpu->ext_pwr_lmt_en == true) {
							if (pwr->tx_pwr_limit_6g[reg][bw][rate][bf][ch][tx_num] != -128) {
								if (pwr->tx_pwr_limit_6g[reg][bw][rate][bf][ch][tx_num] < ext_pwr) {
									pwr->tx_pwr_limit_6g[PW_LMT_REGU_EXT_PWR][bw][rate][bf][ch][tx_num] =
										pwr->tx_pwr_limit_6g[reg][bw][rate][bf][ch][tx_num];
								} else {
									pwr->tx_pwr_limit_6g[PW_LMT_REGU_EXT_PWR][bw][rate][bf][ch][tx_num] = ext_pwr;
								}
							} else {
								pwr->tx_pwr_limit_6g[PW_LMT_REGU_EXT_PWR][bw][rate][bf][ch][tx_num] = ext_pwr;
							}
						} else {
							pwr->tx_pwr_limit_6g[PW_LMT_REGU_EXT_PWR][bw][rate][bf][ch][tx_num] =
								pwr->tx_pwr_limit_6g[reg][bw][rate][bf][ch][tx_num];
						}

						RF_DBG(rf, DBG_RF_INIT, "pwr->tx_pwr_limit_6g[%d][%d][%d][%d][%d][%d] =   %d\n",
								PW_LMT_REGU_EXT_PWR, bw, rate, bf, ch, tx_num ,
								pwr->tx_pwr_limit_6g[PW_LMT_REGU_EXT_PWR][bw][rate][bf][ch][tx_num]);
					}
				}
			}
		}
	}

	RF_DBG(rf, DBG_RF_INIT, "<======%s finish!!!\n", __func__);
#endif
}

void halrf_power_limit_set_ext_pwr_limit_ru_table(struct rf_info *rf,
							enum phl_phy_idx phy)
{
#ifdef SPF_PHL_RF_019_SAR
	struct halrf_pwr_info *pwr = &rf->pwr_info;
	struct rtw_hal_com_t *hal = rf->hal_com;
	struct rtw_tpu_info *tpu = &rf->hal_com->band[phy].rtw_tpu_i;
	struct rtw_phl_ext_pwr_lmt_info *ext_pwr_info = &rf->hal_com->band[phy].rtw_tpu_i.ext_pwr_lmt_i;
	u32 reg, bw, rate, ch, tx_num;
	s8 ext_pwr = 0;
	u8 real_ch; 

	RF_DBG(rf, DBG_RF_INIT, "======>%s   phy=%d\n", __func__, phy);

	reg = (u32)halrf_get_regulation_info(rf, BAND_ON_24G);

	hal_mem_set(hal, pwr->ext_pwr_diff_2_4g, 0, sizeof(pwr->ext_pwr_diff_2_4g));
	hal_mem_set(hal, pwr->ext_pwr_diff_5g_band1, 0, sizeof(pwr->ext_pwr_diff_5g_band1));
	hal_mem_set(hal, pwr->ext_pwr_diff_5g_band2, 0, sizeof(pwr->ext_pwr_diff_5g_band2));
	hal_mem_set(hal, pwr->ext_pwr_diff_5g_band3, 0, sizeof(pwr->ext_pwr_diff_5g_band3));
	hal_mem_set(hal, pwr->ext_pwr_diff_5g_band4, 0, sizeof(pwr->ext_pwr_diff_5g_band4));

	if (ext_pwr_info->ext_pwr_lmt_ant_2_4g[RF_PATH_A] > ext_pwr_info->ext_pwr_lmt_ant_2_4g[RF_PATH_B]) {
	pwr->ext_pwr_diff_2_4g[RF_PATH_B] =
			ext_pwr_info->ext_pwr_lmt_ant_2_4g[RF_PATH_B] - ext_pwr_info->ext_pwr_lmt_ant_2_4g[RF_PATH_A];
	} else {
		pwr->ext_pwr_diff_2_4g[RF_PATH_A] =
			ext_pwr_info->ext_pwr_lmt_ant_2_4g[RF_PATH_A] - ext_pwr_info->ext_pwr_lmt_ant_2_4g[RF_PATH_B];
	}

	RF_DBG(rf, DBG_RF_INIT, "ext_pwr_info->ext_pwr_lmt_2_4g[RF_PATH_A]=%d\n",
		ext_pwr_info->ext_pwr_lmt_ant_2_4g[RF_PATH_A]);

	RF_DBG(rf, DBG_RF_INIT, "ext_pwr_info->ext_pwr_lmt_2_4g[RF_PATH_B]=%d\n",
		ext_pwr_info->ext_pwr_lmt_ant_2_4g[RF_PATH_B]);

	RF_DBG(rf, DBG_RF_INIT, "pwr->ext_pwr_diff_2_4g[RF_PATH_A]=%d pwr->ext_pwr_diff_2_4g[RF_PATH_B]=%d\n",
		pwr->ext_pwr_diff_2_4g[RF_PATH_A], pwr->ext_pwr_diff_2_4g[RF_PATH_B]);

	for (tx_num = 0; tx_num < 2; tx_num++) {
		ext_pwr = ext_pwr_info->ext_pwr_lmt_2_4g[tx_num];
		for (ch = 0; ch < PW_LMT_MAX_CHANNEL_NUMBER_2G; ch++) {
			for (rate = 0; rate < PW_LMT_MAX_RS_NUM; rate++) {
				for (bw = 0; bw < PW_LMT_RU_BW_NULL; bw++) {

					RF_DBG(rf, DBG_RF_INIT, "ext_pwr_lmt_2_4g[%d]=%d   pwr->tx_pwr_limit_ru_2g[%d][%d][%d][%d][%d]=   %d\n",
						tx_num, ext_pwr,
						reg, bw, rate, ch, tx_num, pwr->tx_pwr_limit_ru_2g[reg][bw][rate][ch][tx_num]);

					if (tpu->ext_pwr_lmt_en == true) {
						if (pwr->tx_pwr_limit_ru_2g[reg][bw][rate][ch][tx_num] != -128) {
							if (pwr->tx_pwr_limit_ru_2g[reg][bw][rate][ch][tx_num] < ext_pwr) {
								pwr->tx_pwr_limit_ru_2g[PW_LMT_REGU_EXT_PWR][bw][rate][ch][tx_num] =
									pwr->tx_pwr_limit_ru_2g[reg][bw][rate][ch][tx_num];
							} else {
								pwr->tx_pwr_limit_ru_2g[PW_LMT_REGU_EXT_PWR][bw][rate][ch][tx_num] = ext_pwr;
							}
						} else {
							pwr->tx_pwr_limit_ru_2g[PW_LMT_REGU_EXT_PWR][bw][rate][ch][tx_num] = ext_pwr;
						}
					} else {
						pwr->tx_pwr_limit_ru_2g[PW_LMT_REGU_EXT_PWR][bw][rate][ch][tx_num] =
							pwr->tx_pwr_limit_ru_2g[reg][bw][rate][ch][tx_num];
					}

					RF_DBG(rf, DBG_RF_INIT, "pwr->tx_pwr_limit_ru_2g[%d][%d][%d][%d][%d] =   %d\n",
						PW_LMT_REGU_EXT_PWR, bw, rate, ch, tx_num,
						pwr->tx_pwr_limit_ru_2g[PW_LMT_REGU_EXT_PWR][bw][rate][ch][tx_num]);
				}
			}
		}
	}

	reg = (u32)halrf_get_regulation_info(rf, BAND_ON_5G);

	if (ext_pwr_info->ext_pwr_lmt_ant_5g_band1[RF_PATH_A] > ext_pwr_info->ext_pwr_lmt_ant_5g_band1[RF_PATH_B]) {
		pwr->ext_pwr_diff_5g_band1[RF_PATH_B] =
			ext_pwr_info->ext_pwr_lmt_ant_5g_band1[RF_PATH_B] - ext_pwr_info->ext_pwr_lmt_ant_5g_band1[RF_PATH_A];
	} else {
		pwr->ext_pwr_diff_5g_band1[RF_PATH_A] =
			ext_pwr_info->ext_pwr_lmt_ant_5g_band1[RF_PATH_A] - ext_pwr_info->ext_pwr_lmt_ant_5g_band1[RF_PATH_B];
	}

	if (ext_pwr_info->ext_pwr_lmt_ant_5g_band2[RF_PATH_A] > ext_pwr_info->ext_pwr_lmt_ant_5g_band2[RF_PATH_B]) {
		pwr->ext_pwr_diff_5g_band2[RF_PATH_B] =
			ext_pwr_info->ext_pwr_lmt_ant_5g_band2[RF_PATH_B] - ext_pwr_info->ext_pwr_lmt_ant_5g_band2[RF_PATH_A];
	} else {
		pwr->ext_pwr_diff_5g_band2[RF_PATH_A] =
			ext_pwr_info->ext_pwr_lmt_ant_5g_band2[RF_PATH_A] - ext_pwr_info->ext_pwr_lmt_ant_5g_band2[RF_PATH_B];
	}

	if (ext_pwr_info->ext_pwr_lmt_ant_5g_band3[RF_PATH_A] > ext_pwr_info->ext_pwr_lmt_ant_5g_band3[RF_PATH_B]) {
		pwr->ext_pwr_diff_5g_band3[RF_PATH_B] =
			ext_pwr_info->ext_pwr_lmt_ant_5g_band3[RF_PATH_B] - ext_pwr_info->ext_pwr_lmt_ant_5g_band3[RF_PATH_A];
	} else {
		pwr->ext_pwr_diff_5g_band3[RF_PATH_A] =
			ext_pwr_info->ext_pwr_lmt_ant_5g_band3[RF_PATH_A] - ext_pwr_info->ext_pwr_lmt_ant_5g_band3[RF_PATH_B];
	}

	if (ext_pwr_info->ext_pwr_lmt_ant_5g_band4[RF_PATH_A] > ext_pwr_info->ext_pwr_lmt_ant_5g_band4[RF_PATH_B]) {
		pwr->ext_pwr_diff_5g_band4[RF_PATH_B] =
			ext_pwr_info->ext_pwr_lmt_ant_5g_band4[RF_PATH_B] - ext_pwr_info->ext_pwr_lmt_ant_5g_band4[RF_PATH_A];
	} else {
		pwr->ext_pwr_diff_5g_band4[RF_PATH_A] =
			ext_pwr_info->ext_pwr_lmt_ant_5g_band4[RF_PATH_A] - ext_pwr_info->ext_pwr_lmt_ant_5g_band4[RF_PATH_B];
	}

	RF_DBG(rf, DBG_RF_INIT, "ext_pwr_lmt_5g_band1[RF_PATH_A]=%d  ext_pwr_lmt_5g_band1[RF_PATH_B]=%d\n",
		ext_pwr_info->ext_pwr_lmt_ant_5g_band1[RF_PATH_A],
		ext_pwr_info->ext_pwr_lmt_ant_5g_band1[RF_PATH_B]);

	RF_DBG(rf, DBG_RF_INIT, "pwr->ext_pwr_diff_5g_band1[RF_PATH_A]=%d pwr->ext_pwr_diff_5g_band1[RF_PATH_B]=%d\n",
		pwr->ext_pwr_diff_5g_band1[RF_PATH_A], pwr->ext_pwr_diff_5g_band1[RF_PATH_B]);
	
	RF_DBG(rf, DBG_RF_INIT, "ext_pwr_lmt_5g_band2[RF_PATH_A]=%d   ext_pwr_lmt_5g_band2[RF_PATH_B]=%d\n",
		ext_pwr_info->ext_pwr_lmt_ant_5g_band2[RF_PATH_A],
		ext_pwr_info->ext_pwr_lmt_ant_5g_band2[RF_PATH_B]);

	RF_DBG(rf, DBG_RF_INIT, "pwr->ext_pwr_diff_5g_band2[RF_PATH_A]=%d pwr->ext_pwr_diff_5g_band2[RF_PATH_B]=%d\n",
		pwr->ext_pwr_diff_5g_band2[RF_PATH_A], pwr->ext_pwr_diff_5g_band2[RF_PATH_B]);

	RF_DBG(rf, DBG_RF_INIT, "ext_pwr_lmt_5g_band3[RF_PATH_A]=%d   ext_pwr_lmt_5g_band3[RF_PATH_B]=%d\n",
		ext_pwr_info->ext_pwr_lmt_ant_5g_band3[RF_PATH_A],
		ext_pwr_info->ext_pwr_lmt_ant_5g_band3[RF_PATH_B]);

	RF_DBG(rf, DBG_RF_INIT, "pwr->ext_pwr_diff_5g_band3[RF_PATH_A]=%d pwr->ext_pwr_diff_5g_band3[RF_PATH_B]=%d\n",
		pwr->ext_pwr_diff_5g_band3[RF_PATH_A], pwr->ext_pwr_diff_5g_band3[RF_PATH_B]);

	RF_DBG(rf, DBG_RF_INIT, "ext_pwr_lmt_5g_band4[RF_PATH_A]=%d   ext_pwr_lmt_5g_band4[RF_PATH_B]=%d\n",
		ext_pwr_info->ext_pwr_lmt_ant_5g_band4[RF_PATH_A],
		ext_pwr_info->ext_pwr_lmt_ant_5g_band4[RF_PATH_B]);

	RF_DBG(rf, DBG_RF_INIT, "pwr->ext_pwr_diff_5g_band4[RF_PATH_A]=%d pwr->ext_pwr_diff_5g_band4[RF_PATH_B]=%d\n",
		pwr->ext_pwr_diff_5g_band4[RF_PATH_A], pwr->ext_pwr_diff_5g_band4[RF_PATH_B]);

	for (tx_num = 0; tx_num < 2; tx_num++) {
		for (ch = 0; ch < PW_LMT_MAX_CHANNEL_NUMBER_5G; ch++) {
			real_ch = halrf_get_limit_ch_idx_to_ch_idx(rf, PW_LMT_BAND_5G, (u8)ch);
			if (real_ch >= 36 && real_ch <= 48)
				ext_pwr = ext_pwr_info->ext_pwr_lmt_5g_band1[tx_num];
			else if (real_ch >= 50 && real_ch <= 64)
				ext_pwr = ext_pwr_info->ext_pwr_lmt_5g_band2[tx_num];
			else if (real_ch >= 100 && real_ch <= 144)
				ext_pwr = ext_pwr_info->ext_pwr_lmt_5g_band3[tx_num];
			else if (real_ch >= 149 && real_ch <= 177)
				ext_pwr = ext_pwr_info->ext_pwr_lmt_5g_band4[tx_num];

			for (rate = 0; rate < PW_LMT_MAX_RS_NUM; rate++) {
				for (bw = 0; bw < PW_LMT_RU_BW_NULL; bw++) {

					RF_DBG(rf, DBG_RF_INIT, "ext_pwr_lmt_5g[%d]=%d   pwr->tx_pwr_limit_ru_5g[%d][%d][%d][%d][%d]=   %d\n",
						tx_num, ext_pwr,
						reg, bw, rate, ch, tx_num, pwr->tx_pwr_limit_ru_5g[reg][bw][rate][ch][tx_num]);

					if (tpu->ext_pwr_lmt_en == true) {
						if (pwr->tx_pwr_limit_ru_5g[reg][bw][rate][ch][tx_num] != -128) {
							if (pwr->tx_pwr_limit_ru_5g[reg][bw][rate][ch][tx_num] < ext_pwr) {
								pwr->tx_pwr_limit_ru_5g[PW_LMT_REGU_EXT_PWR][bw][rate][ch][tx_num] =
									pwr->tx_pwr_limit_ru_5g[reg][bw][rate][ch][tx_num];
							} else {
								pwr->tx_pwr_limit_ru_5g[PW_LMT_REGU_EXT_PWR][bw][rate][ch][tx_num] = ext_pwr;
							}
						} else {
							pwr->tx_pwr_limit_ru_5g[PW_LMT_REGU_EXT_PWR][bw][rate][ch][tx_num] = ext_pwr;
						}
					} else {
						pwr->tx_pwr_limit_ru_5g[PW_LMT_REGU_EXT_PWR][bw][rate][ch][tx_num] =
							pwr->tx_pwr_limit_ru_5g[reg][bw][rate][ch][tx_num];
					}

					RF_DBG(rf, DBG_RF_INIT, "pwr->tx_pwr_limit_ru_5g[%d][%d][%d][%d][%d]=   %d\n",
						PW_LMT_REGU_EXT_PWR, bw, rate, ch, tx_num,
						pwr->tx_pwr_limit_ru_5g[PW_LMT_REGU_EXT_PWR][bw][rate][ch][tx_num]);
				}
			}
		}
	}

	reg = (u32)halrf_get_regulation_info(rf, BAND_ON_6G);

	for (tx_num = 0; tx_num < 2; tx_num++) {
		for (ch = 0; ch < PW_LMT_MAX_CHANNEL_NUMBER_6G; ch++) {
			real_ch = halrf_get_limit_ch_idx_to_ch_idx(rf, PW_LMT_BAND_6G, (u8)ch);

			if (real_ch >= 1 && real_ch <= 45)
				ext_pwr = ext_pwr_info->ext_pwr_lmt_6g_unii_5_1[tx_num];
			else if (real_ch >= 49 && real_ch <= 93)
				ext_pwr = ext_pwr_info->ext_pwr_lmt_6g_unii_5_2[tx_num];
			else if (real_ch >= 97 && real_ch <= 117)
				ext_pwr = ext_pwr_info->ext_pwr_lmt_6g_unii_6[tx_num];
			else if (real_ch >= 121 && real_ch <= 149)
				ext_pwr = ext_pwr_info->ext_pwr_lmt_6g_unii_7_1[tx_num];
			else if (real_ch >= 153 && real_ch <= 189)
				ext_pwr = ext_pwr_info->ext_pwr_lmt_6g_unii_7_2[tx_num];
			else if (real_ch >= 193 && real_ch <= 237)
				ext_pwr = ext_pwr_info->ext_pwr_lmt_6g_unii_8[tx_num];
			else if (real_ch >= 241 && real_ch <= 253)
				ext_pwr = ext_pwr_info->ext_pwr_lmt_6g_unii_8[tx_num];

			for (rate = 0; rate < PW_LMT_MAX_RS_NUM; rate++) {
				for (bw = 0; bw < PW_LMT_RU_BW_NULL; bw++) {

					RF_DBG(rf, DBG_RF_INIT, "ext_pwr_lmt_6g[%d]=%d   pwr->tx_pwr_limit_ru_6g[%d][%d][%d][%d][%d]=   %d\n",
						tx_num, ext_pwr,
						reg, bw, rate, ch, tx_num, pwr->tx_pwr_limit_ru_6g[reg][bw][rate][ch][tx_num]);

					if (tpu->ext_pwr_lmt_en == true) {
						if (pwr->tx_pwr_limit_ru_6g[reg][bw][rate][ch][tx_num] != -128) {
							if (pwr->tx_pwr_limit_ru_6g[reg][bw][rate][ch][tx_num] < ext_pwr) {
								pwr->tx_pwr_limit_ru_6g[PW_LMT_REGU_EXT_PWR][bw][rate][ch][tx_num] =
									pwr->tx_pwr_limit_ru_6g[reg][bw][rate][ch][tx_num];
							} else {
								pwr->tx_pwr_limit_ru_6g[PW_LMT_REGU_EXT_PWR][bw][rate][ch][tx_num] = ext_pwr;
							}
						} else {
							pwr->tx_pwr_limit_ru_6g[PW_LMT_REGU_EXT_PWR][bw][rate][ch][tx_num] = ext_pwr;
						}
					} else {
						pwr->tx_pwr_limit_ru_6g[PW_LMT_REGU_EXT_PWR][bw][rate][ch][tx_num] =
							pwr->tx_pwr_limit_ru_6g[reg][bw][rate][ch][tx_num];
					}

					RF_DBG(rf, DBG_RF_INIT, "pwr->tx_pwr_limit_ru_6g[%d][%d][%d][%d][%d] =   %d\n",
						PW_LMT_REGU_EXT_PWR, bw, rate, ch, tx_num,
						pwr->tx_pwr_limit_ru_6g[PW_LMT_REGU_EXT_PWR][bw][rate][ch][tx_num]);
				}
			}
		}
	}

	RF_DBG(rf, DBG_RF_INIT, "<======%s finish!!!\n", __func__);
#endif
}

u8 halrf_get_tx_tbl_to_tx_pwr_times(struct rf_info *rf)
{
	return TX_TABLE_TO_TX_PWR;
}

s8 halrf_get_power_by_rate(struct rf_info *rf,
	enum phl_phy_idx phy,
	u8 rf_path, u16 rate, u8 dcm, u8 offset)
{
	struct halrf_pwr_info *pwr = &rf->pwr_info;
	u32 band = rf->hal_com->band[phy].cur_chandef.band;
	u16 rate_tmp;
	s8 pwr_by_rate = 0;

	RF_DBG(rf, DBG_RF_POWER, "======>%s rf_path=%d rate=0x%x dcm=%d\n",
		__func__, rf_path, rate, dcm);

	if (band >= PW_LMT_MAX_BAND) {
		RF_DBG(rf, DBG_RF_POWER, "======>%s band(%d) >= PW_LMT_MAX_BAND(%d) Return!!!\n",
			__func__, band, PW_LMT_MAX_BAND);

		return 0;
	}

	rate_tmp = halrf_get_dcm_offset_pwr_by_rate(rf, rate, dcm, offset);

	if ((rate_tmp >= HALRF_DATA_RATE_CCK1 && rate_tmp <= HALRF_DATA_RATE_CCK11) ||
		rate_tmp == HALRF_DATA_RATE_CCK_OFFSET) {
		if (rate_tmp < HALRF_DATA_RATE_MAX)
			pwr_by_rate = pwr->tx_pwr_by_rate[PW_LMT_BAND_2_4G][rate_tmp];

		RF_DBG(rf, DBG_RF_POWER, "pwr_by_rate(%d)=pwr->tx_pwr_by_rate[%d][%d] band=%d\n",
			pwr_by_rate, PW_LMT_BAND_2_4G, rate_tmp, band);
	} else {
		if ((rate_tmp >= HALRF_DATA_RATE_OFDM6) && (rate_tmp <= HALRF_DATA_RATE_OFDM_OFFSET))
			pwr_by_rate = pwr->tx_pwr_by_rate[band][rate_tmp];

		RF_DBG(rf, DBG_RF_POWER, "pwr_by_rate(%d)=pwr->tx_pwr_by_rate[%d][%d] band=%d\n",
			pwr_by_rate, band, rate_tmp, band);
	}

	if (offset == 0)
		return pwr_by_rate + halrf_get_pwr_control(rf, phy) + halrf_get_tx_rate_pwr_control(rf, phy);
	else
		return pwr_by_rate;
}

s8 halrf_get_power_by_rate_band(struct rf_info *rf,
	enum phl_phy_idx phy, u16 rate, u8 dcm, u8 offset, u32 band)
{
	struct halrf_pwr_info *pwr = &rf->pwr_info;
	u16 rate_tmp;
	s8 pwr_by_rate = 0;

	RF_DBG(rf, DBG_RF_POWER, "======>%s rate=0x%x dcm=%d band=%d\n",
		__func__, rate, dcm, band);

	if (band >= PW_LMT_MAX_BAND) {
		RF_DBG(rf, DBG_RF_POWER, "======>%s band(%d) >= PW_LMT_MAX_BAND(%d) Return!!!\n",
			__func__, band, PW_LMT_MAX_BAND);
		return 0;
	}

	rate_tmp = halrf_get_dcm_offset_pwr_by_rate(rf, rate, dcm, offset);

	if ((rate_tmp >= HALRF_DATA_RATE_CCK1 && rate_tmp <= HALRF_DATA_RATE_CCK11) ||
		rate_tmp == HALRF_DATA_RATE_CCK_OFFSET) {
		if (rate_tmp < HALRF_DATA_RATE_MAX)
			pwr_by_rate = pwr->tx_pwr_by_rate[PW_LMT_BAND_2_4G][rate_tmp];

		RF_DBG(rf, DBG_RF_POWER, "pwr_by_rate(%d)=pwr->tx_pwr_by_rate[%d][%d] band=%d\n",
			pwr_by_rate, PW_LMT_BAND_2_4G, rate_tmp, band);
	} else {
		if ((rate_tmp >= HALRF_DATA_RATE_OFDM6) && (rate_tmp <= HALRF_DATA_RATE_OFDM_OFFSET))
			pwr_by_rate = pwr->tx_pwr_by_rate[band][rate_tmp];

		RF_DBG(rf, DBG_RF_POWER, "pwr_by_rate(%d)=pwr->tx_pwr_by_rate[%d][%d] band=%d\n",
			pwr_by_rate, band, rate_tmp, band);
	}

	return pwr_by_rate;
}

s8 halrf_get_power_limit(struct rf_info *rf,
	enum phl_phy_idx phy, u8 rf_path, u16 rate, u8 bandwidth,
	u8 beamforming, u8 tx_num, u8 channel)
{
	struct halrf_pwr_info *pwr = &rf->pwr_info;
	struct rtw_phl_com_t *phl = rf->phl_com;
	u32 band = rf->hal_com->band[phy].cur_chandef.band;
	
#ifdef SPF_PHL_RF_019_SAR
	struct rtw_tpu_info *tpu = &rf->hal_com->band[phy].rtw_tpu_i;
#endif
	s8 pwr_limit = 0;
	u8 limit_rate = PW_LMT_RS_CCK, limit_ch, reg;

	RF_DBG(rf, DBG_RF_POWER, "======>%s rf_path=%d rate=0x%x beamforming=%d\n",
		__func__, rf_path, rate, beamforming);

	limit_rate = halrf_hw_rate_to_limit_rate_tx_num(rf, rate);

	if (band == BAND_ON_24G) {
		limit_ch = halrf_get_ch_idx_to_limit_array(rf, channel);
#ifdef SPF_PHL_RF_019_SAR
		if (tpu->ext_pwr_lmt_en == true)
			reg = PW_LMT_REGU_EXT_PWR;
		else
#endif
			reg = halrf_get_regulation_info(rf, BAND_ON_24G);

		pwr_limit = pwr->tx_pwr_limit_2g[reg][bandwidth][limit_rate][beamforming][limit_ch][tx_num];
		RF_DBG(rf, DBG_RF_POWER, "pwr_limit(%d) = pwr->tx_pwr_limit_2g[%d][%d][%d][%d][%d][%d]\n",
			pwr_limit, reg, bandwidth, limit_rate, beamforming, limit_ch, tx_num);
	} else if (band == BAND_ON_5G) {
		limit_ch = halrf_get_ch_idx_to_limit_array(rf, channel);
#ifdef SPF_PHL_RF_019_SAR
		if (tpu->ext_pwr_lmt_en == true)
			reg = PW_LMT_REGU_EXT_PWR;
		else
#endif
			reg = halrf_get_regulation_info(rf, BAND_ON_5G);

		pwr_limit = pwr->tx_pwr_limit_5g[reg][bandwidth][limit_rate][beamforming][limit_ch][tx_num];
		RF_DBG(rf, DBG_RF_POWER, "pwr_limit(%d) = pwr->tx_pwr_limit_5g[%d][%d][%d][%d][%d][%d]\n",
			pwr_limit, reg, bandwidth, limit_rate, beamforming, limit_ch, tx_num);
	}  else if (band == BAND_ON_6G) {
		limit_ch = halrf_get_ch_idx_to_6g_limit_array(rf, channel);
#ifdef SPF_PHL_RF_019_SAR
		if (tpu->ext_pwr_lmt_en == true)
			reg = PW_LMT_REGU_EXT_PWR;
		else
#endif
			reg = halrf_get_regulation_info(rf, BAND_ON_6G);

		pwr_limit = pwr->tx_pwr_limit_6g[reg][bandwidth][limit_rate][beamforming][limit_ch][tx_num];
		RF_DBG(rf, DBG_RF_POWER, "pwr_limit(%d) = pwr->tx_pwr_limit_6g[%d][%d][%d][%d][%d][%d]\n",
			pwr_limit, reg, bandwidth, limit_rate, beamforming, limit_ch, tx_num);
	}

	if (pwr_limit == 127 || pwr_limit == -128)
		pwr_limit = 0;

	return pwr_limit + halrf_get_pwr_control(rf, phy);
}

s8 halrf_get_power_limit_option(struct rf_info *rf,
	enum phl_phy_idx phy, u8 rf_path, u16 rate, u8 bandwidth,
	u8 beamforming, u8 tx_num, u8 channel, u32 band, u8 reg)
{
	struct halrf_pwr_info *pwr = &rf->pwr_info;
	struct rtw_phl_com_t *phl = rf->phl_com;

	s8 pwr_limit = 0;
	u8 limit_rate = PW_LMT_RS_CCK, limit_ch;

	RF_DBG(rf, DBG_RF_POWER, "======>%s rf_path=%d rate=0x%x beamforming=%d\n",
		__func__, rf_path, rate, beamforming);

	limit_rate = halrf_hw_rate_to_limit_rate_tx_num(rf, rate);

	if (band == BAND_ON_24G) {
		limit_ch = halrf_get_ch_idx_to_limit_array(rf, channel);

		pwr_limit = pwr->tx_pwr_limit_2g[reg][bandwidth][limit_rate][beamforming][limit_ch][tx_num];
		RF_DBG(rf, DBG_RF_POWER, "pwr_limit(%d) = pwr->tx_pwr_limit_2g[%d][%d][%d][%d][%d][%d]\n",
			pwr_limit, reg, bandwidth, limit_rate, beamforming, limit_ch, tx_num);
	} else if (band == BAND_ON_5G) {
		limit_ch = halrf_get_ch_idx_to_limit_array(rf, channel);

		pwr_limit = pwr->tx_pwr_limit_5g[reg][bandwidth][limit_rate][beamforming][limit_ch][tx_num];
		RF_DBG(rf, DBG_RF_POWER, "pwr_limit(%d) = pwr->tx_pwr_limit_5g[%d][%d][%d][%d][%d][%d]\n",
			pwr_limit, reg, bandwidth, limit_rate, beamforming, limit_ch, tx_num);
	}  else if (band == BAND_ON_6G) {
		limit_ch = halrf_get_ch_idx_to_6g_limit_array(rf, channel);

		pwr_limit = pwr->tx_pwr_limit_6g[reg][bandwidth][limit_rate][beamforming][limit_ch][tx_num];
		RF_DBG(rf, DBG_RF_POWER, "pwr_limit(%d) = pwr->tx_pwr_limit_6g[%d][%d][%d][%d][%d][%d]\n",
			pwr_limit, reg, bandwidth, limit_rate, beamforming, limit_ch, tx_num);
	}

	return pwr_limit;
}

s8 halrf_get_power_limit_ru(struct rf_info *rf,
	enum phl_phy_idx phy, u8 rf_path, u16 rate, u8 bandwidth,
	u8 tx_num, u8 channel)
{
	struct halrf_pwr_info *pwr = &rf->pwr_info;
	struct rtw_phl_com_t *phl = rf->phl_com;
	u32 band = rf->hal_com->band[phy].cur_chandef.band;

#ifdef SPF_PHL_RF_019_SAR
	struct rtw_tpu_info *tpu = &rf->hal_com->band[phy].rtw_tpu_i;
#endif
	s8 pwr_limit_ru = 0;
	u8 limit_rate = PW_LMT_RS_CCK, limit_ch, reg;

	RF_DBG(rf, DBG_RF_POWER, "======>%s phy=%d rf_path=%d rate=0x%x\n",
		__func__, phy, rf_path, rate);

	limit_rate = halrf_hw_rate_to_limit_rate_tx_num(rf, rate);

	if (band == BAND_ON_24G) {
		limit_ch = halrf_get_ch_idx_to_limit_array(rf, channel);
#ifdef SPF_PHL_RF_019_SAR
		if (tpu->ext_pwr_lmt_en == true)
			reg = PW_LMT_REGU_EXT_PWR;
		else
#endif
			reg = halrf_get_regulation_info(rf, BAND_ON_24G);

		pwr_limit_ru = pwr->tx_pwr_limit_ru_2g[reg][bandwidth][limit_rate][limit_ch][tx_num];
		RF_DBG(rf, DBG_RF_POWER, "pwr_limit_ru(%d) = pwr->tx_pwr_limit_ru_2g[%d][%d][%d][%d][%d]\n",
			pwr_limit_ru, reg, bandwidth, limit_rate, limit_ch, tx_num);
	} else if (band == BAND_ON_5G) {
		limit_ch = halrf_get_ch_idx_to_limit_array(rf, channel);
#ifdef SPF_PHL_RF_019_SAR
		if (tpu->ext_pwr_lmt_en == true)
			reg = PW_LMT_REGU_EXT_PWR;
		else
#endif
			reg = halrf_get_regulation_info(rf, BAND_ON_5G);

		pwr_limit_ru = pwr->tx_pwr_limit_ru_5g[reg][bandwidth][limit_rate][limit_ch][tx_num];
		RF_DBG(rf, DBG_RF_POWER, "pwr_limit_ru(%d) = pwr->tx_pwr_limit_ru_5g[%d][%d][%d][%d][%d]\n",
			pwr_limit_ru, reg, bandwidth, limit_rate, limit_ch, tx_num);
	} else if (band == BAND_ON_6G) {
		limit_ch = halrf_get_ch_idx_to_6g_limit_array(rf, channel);
#ifdef SPF_PHL_RF_019_SAR
		if (tpu->ext_pwr_lmt_en == true)
			reg = PW_LMT_REGU_EXT_PWR;
		else
#endif
			reg = halrf_get_regulation_info(rf, BAND_ON_6G);

		pwr_limit_ru = pwr->tx_pwr_limit_ru_6g[reg][bandwidth][limit_rate][limit_ch][tx_num];
		RF_DBG(rf, DBG_RF_POWER, "pwr_limit_ru(%d) = pwr->tx_pwr_limit_ru_6g[%d][%d][%d][%d][%d]\n",
			pwr_limit_ru, reg, bandwidth, limit_rate, limit_ch, tx_num);
	}

	if (pwr_limit_ru == 127 || pwr_limit_ru == -128)
		pwr_limit_ru = 0;

	return pwr_limit_ru + halrf_get_pwr_control(rf, phy);
}

s8 halrf_get_power_limit_ru_option(struct rf_info *rf,
	enum phl_phy_idx phy, u8 rf_path, u16 rate, u8 bandwidth,
	u8 tx_num, u8 channel, u32 band, u8 reg)
{
	struct halrf_pwr_info *pwr = &rf->pwr_info;
	struct rtw_phl_com_t *phl = rf->phl_com;

	s8 pwr_limit_ru = 0;
	u8 limit_rate = PW_LMT_RS_CCK, limit_ch;

	RF_DBG(rf, DBG_RF_POWER, "======>%s phy=%d rf_path=%d rate=0x%x\n",
		__func__, phy, rf_path, rate);

	limit_rate = halrf_hw_rate_to_limit_rate_tx_num(rf, rate);

	if (band == BAND_ON_24G) {
		limit_ch = halrf_get_ch_idx_to_limit_array(rf, channel);

		pwr_limit_ru = pwr->tx_pwr_limit_ru_2g[reg][bandwidth][limit_rate][limit_ch][tx_num];
		RF_DBG(rf, DBG_RF_POWER, "pwr_limit_ru(%d) = pwr->tx_pwr_limit_ru_2g[%d][%d][%d][%d][%d]\n",
			pwr_limit_ru, reg, bandwidth, limit_rate, limit_ch, tx_num);
	} else if (band == BAND_ON_5G) {
		limit_ch = halrf_get_ch_idx_to_limit_array(rf, channel);

		pwr_limit_ru = pwr->tx_pwr_limit_ru_5g[reg][bandwidth][limit_rate][limit_ch][tx_num];
		RF_DBG(rf, DBG_RF_POWER, "pwr_limit_ru(%d) = pwr->tx_pwr_limit_ru_5g[%d][%d][%d][%d][%d]\n",
			pwr_limit_ru, reg, bandwidth, limit_rate, limit_ch, tx_num);
	} else if (band == BAND_ON_6G) {
		limit_ch = halrf_get_ch_idx_to_6g_limit_array(rf, channel);

		pwr_limit_ru = pwr->tx_pwr_limit_ru_6g[reg][bandwidth][limit_rate][limit_ch][tx_num];
		RF_DBG(rf, DBG_RF_POWER, "pwr_limit_ru(%d) = pwr->tx_pwr_limit_ru_6g[%d][%d][%d][%d][%d]\n",
			pwr_limit_ru, reg, bandwidth, limit_rate, limit_ch, tx_num);
	}

	return pwr_limit_ru;
}

s16 halrf_get_power(void *rf_void,
	u8 rf_path, u16 rate, u8 dcm, u8 offset, u8 bandwidth,
	u8 beamforming, u8 channel)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct halrf_pwr_info *pwr = &rf->pwr_info;
	struct rtw_phl_com_t *phl = rf->phl_com;
	struct rtw_tpu_info *tpu = &rf->hal_com->band[HW_PHY_0].rtw_tpu_i;
	u8 band, limit_rate = PW_LMT_RS_CCK, tx_num = PW_LMT_PH_1T, limit_ch;
	u16 rate_tmp;
	s16 pwr_by_rate, pwr_limit, power;

	RF_DBG(rf, DBG_RF_INIT, "======>%s rf_path=%d rate=%d dcm=%d bw=%d bf=%d ch=%d\n",
		__func__, rf_path, rate, dcm, bandwidth, beamforming, channel);

	if (channel >= 1 && channel <= 14)
		band = PW_LMT_BAND_2_4G;
	else
		band = PW_LMT_BAND_5G;

	tx_num = rf_path;

	rate_tmp = halrf_get_dcm_offset_pwr_by_rate(rf, rate, dcm, offset);

	pwr_by_rate = (s16)pwr->tx_pwr_by_rate[band][rate_tmp];

	RF_DBG(rf, DBG_RF_INIT, "pwr_by_rate(%d)=(s16)pwr->tx_pwr_by_rate[%d][%d]\n",
		pwr_by_rate, band, rate_tmp);

	limit_rate = halrf_hw_rate_to_limit_rate_tx_num(rf, rate);
	limit_ch = halrf_get_ch_idx_to_limit_array(rf, channel);

	if (channel >= 1 && channel <= 14) {
		pwr_limit = (s16)pwr->tx_pwr_limit_2g[halrf_get_regulation_info(rf, BAND_ON_24G)][bandwidth][limit_rate][beamforming][limit_ch][tx_num];
		RF_DBG(rf, DBG_RF_INIT, "pwr_limit(%d) = (s16)pwr->tx_pwr_limit_2g[%d][%d][%d][%d][%d][%d]\n",
			pwr_limit, halrf_get_regulation_info(rf, BAND_ON_24G), bandwidth, limit_rate, beamforming, limit_ch, tx_num);
	} else {
		pwr_limit = (s16)pwr->tx_pwr_limit_5g[halrf_get_regulation_info(rf, BAND_ON_5G)][bandwidth][limit_rate][beamforming][limit_ch][tx_num];
		RF_DBG(rf, DBG_RF_INIT, "pwr_limit(%d) = (s16)pwr->tx_pwr_limit_5g[%d][%d][%d][%d][%d][%d]\n",
			pwr_limit, halrf_get_regulation_info(rf, BAND_ON_5G), bandwidth, limit_rate, beamforming, limit_ch, tx_num);
	}

	if (pwr_by_rate > pwr_limit && tpu->pwr_lmt_en == true)
		power = pwr_limit;
	else
		power = pwr_by_rate;

	RF_DBG(rf, DBG_RF_INIT, "power = %d\n", power);

	return power;
}

s16 halrf_get_band_power(void *rf_void, enum phl_phy_idx phy,
	u8 rf_path, u16 rate, u8 dcm, u8 offset, u8 bandwidth,
	u8 beamforming, u8 channel)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct halrf_pwr_info *pwr = &rf->pwr_info;
	struct rtw_phl_com_t *phl = rf->phl_com;
	u32 band = rf->hal_com->band[phy].cur_chandef.band;
	u8 limit_rate = PW_LMT_RS_CCK, tx_num = PW_LMT_PH_1T, limit_ch;
	u16 rate_tmp;
	s16 pwr_by_rate, pwr_limit = 0, power;

	RF_DBG(rf, DBG_RF_INIT, "======>%s rf_path=%d rate=%d dcm=%d bw=%d bf=%d ch=%d\n",
		__func__, rf_path, rate, dcm, bandwidth, beamforming, channel);

	tx_num = rf_path;

	rate_tmp = halrf_get_dcm_offset_pwr_by_rate(rf, rate, dcm, offset);

	pwr_by_rate = (s16)pwr->tx_pwr_by_rate[band][rate_tmp];

	RF_DBG(rf, DBG_RF_INIT, "pwr_by_rate(%d)=(s16)pwr->tx_pwr_by_rate[%d][%d]\n",
		pwr_by_rate, band, rate_tmp);

	limit_rate = halrf_hw_rate_to_limit_rate_tx_num(rf, rate);

	if (band == BAND_ON_24G) {
		limit_ch = halrf_get_ch_idx_to_limit_array(rf, channel);
		pwr_limit = (s16)pwr->tx_pwr_limit_2g[halrf_get_regulation_info(rf, BAND_ON_24G)][bandwidth][limit_rate][beamforming][limit_ch][tx_num];
		RF_DBG(rf, DBG_RF_INIT, "pwr_limit(%d) = (s16)pwr->tx_pwr_limit_2g[%d][%d][%d][%d][%d][%d]\n",
			pwr_limit, halrf_get_regulation_info(rf, BAND_ON_24G), bandwidth, limit_rate, beamforming, limit_ch, tx_num);
	} else if (band == BAND_ON_5G) {
		limit_ch = halrf_get_ch_idx_to_limit_array(rf, channel);
		pwr_limit = (s16)pwr->tx_pwr_limit_5g[halrf_get_regulation_info(rf, BAND_ON_5G)][bandwidth][limit_rate][beamforming][limit_ch][tx_num];
		RF_DBG(rf, DBG_RF_INIT, "pwr_limit(%d) = (s16)pwr->tx_pwr_limit_5g[%d][%d][%d][%d][%d][%d]\n",
			pwr_limit, halrf_get_regulation_info(rf, BAND_ON_5G), bandwidth, limit_rate, beamforming, limit_ch, tx_num);
	} else if (band == BAND_ON_6G) {
		limit_ch = halrf_get_ch_idx_to_6g_limit_array(rf, channel);
		pwr_limit = (s16)pwr->tx_pwr_limit_6g[halrf_get_regulation_info(rf, BAND_ON_6G)][bandwidth][limit_rate][beamforming][limit_ch][tx_num];
		RF_DBG(rf, DBG_RF_INIT, "pwr_limit(%d) = (s16)pwr->tx_pwr_limit_6g[%d][%d][%d][%d][%d][%d]\n",
			pwr_limit, halrf_get_regulation_info(rf, BAND_ON_6G), bandwidth, limit_rate, beamforming, limit_ch, tx_num);
	}

	if (pwr_by_rate > pwr_limit)
		power = pwr_limit;
	else
		power = pwr_by_rate;

	RF_DBG(rf, DBG_RF_INIT, "power = %d\n", power);

	return power;
}


bool halrf_set_power(struct rf_info *rf, enum phl_phy_idx phy,
	enum phl_pwr_table pwr_table)
{
	struct rtw_hal_com_t *hal_com = rf->hal_com;
	bool result = true;

	halrf_get_power_limit_extra(rf);
#ifdef RF_8852A_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852A) {
		halrf_set_power_8852a(rf, phy, pwr_table);
	}
#endif

#ifdef RF_8852B_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852B) {
		halrf_set_power_8852b(rf, phy, pwr_table);
	}
#endif

#ifdef RF_8852C_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852C) {
		halrf_set_power_8852c(rf, phy, pwr_table);
	}
#endif

#ifdef RF_8832BR_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8832BR) {
		halrf_set_power_8832br(rf, phy, pwr_table);
	}
#endif

#ifdef RF_8192XB_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8192XB) {
		halrf_set_power_8192xb(rf, phy, pwr_table);
	}
#endif

#ifdef RF_8852BP_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852BP) {
		halrf_set_power_8852bp(rf, phy, pwr_table);
	}
#endif

#ifdef RF_8730A_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8730A) {
		halrf_set_power_8730a(rf, phy, pwr_table);
	}
#endif

#ifdef RF_8851B_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8851B) {
		halrf_set_power_8851b(rf, phy, pwr_table);
	}
#endif

	return result;
}

bool halrf_get_efuse_power_table_switch(struct rf_info *rf, enum phl_phy_idx phy_idx)
{
	struct rtw_phl_com_t *phl_com = rf->phl_com;
	struct halrf_pwr_info *pwr = &rf->pwr_info;
	struct rtw_tpu_info *tpu = &rf->hal_com->band[phy_idx].rtw_tpu_i;
	bool result = true;

	pwr->pwr_table_switch_efuse = phl_com->dev_cap.rf_board_opt & 0x3;
	pwr->pwr_by_rate_switch = phl_com->dev_cap.pwrbyrate_off; 
	pwr->pwr_limit_switch =  phl_com->dev_cap.pwrlmt_type;

	RF_DBG(rf, DBG_RF_POWER, "======>%s From Driver pwr->pwr_by_rate_switch=%d\n",
		__func__, pwr->pwr_by_rate_switch);

	RF_DBG(rf, DBG_RF_POWER, "======>%s From Driver pwr->pwr_limit_switch=%d\n",
			__func__, pwr->pwr_limit_switch);

	RF_DBG(rf, DBG_RF_POWER, "======>%s From Efuse pwr->pwr_table_switch_efuse=%d\n",
			__func__, pwr->pwr_table_switch_efuse);

	if (pwr->pwr_limit_switch == PWLMT_DISABLE)
		tpu->pwr_lmt_en = false;
	else if (pwr->pwr_limit_switch == PWBYRATE_AND_PWLMT)
		tpu->pwr_lmt_en = true;
	else {
		if (pwr->pwr_table_switch_efuse == 0)
			tpu->pwr_lmt_en = false;
		else if (pwr->pwr_table_switch_efuse == 2)
			tpu->pwr_lmt_en = false;
		else
			tpu->pwr_lmt_en = true;
	}

	halrf_mac_write_pwr_limit_en(rf, phy_idx);

	return result;
}

void halrf_set_power_table_switch(struct rf_info *rf,
	enum phl_phy_idx phy, u8 pwr_by_rate, u8 pwr_limt)
{
	struct halrf_pwr_info *pwr = &rf->pwr_info;

	pwr->pwr_by_rate_switch = pwr_by_rate;
	pwr->pwr_limit_switch = pwr_limt;
}

#ifndef RF_8730A_SUPPORT
u8 halrf_get_power_limit_extra(struct rf_info *rf)
{
	struct halrf_pwr_info *pwr = &rf->pwr_info;
	struct rtw_regulation_info rg_info = {0};

	if (pwr->regulation_force_en == true) {
		RF_DBG(rf, DBG_RF_POWER, "======>%s   pwr->regulation_force_en=%d   Return!!!\n",
			__func__, pwr->regulation_force_en);
		pwr->extra_regd_idx = 0xff; 
		goto exit;
	}

	halrf_query_regulation_info(rf, &rg_info);

	RF_DBG(rf, DBG_RF_POWER, "domain_code=0x%x   country=%s   regulation_2g=%d   regulation_5g=%d   regulation_6g=%d   chplan_ver=%d   country_ver=%d\n",
		rg_info.domain_code, rg_info.country, rg_info.regulation_2g, rg_info.regulation_5g, rg_info.regulation_6g, rg_info.chplan_ver, rg_info.country_ver);

	pwr->extra_regd_idx = halrf_file_regd_ext_search(rf, rg_info.domain_code, rg_info.country, NULL);

	RF_DBG(rf, DBG_RF_POWER, "Return regd_idx = %d\n", pwr->extra_regd_idx);

exit:
	return (u8)pwr->extra_regd_idx;
}

u8 halrf_file_regd_ext_search(struct rf_info *rf, u16 domain_code, char *country, int *aidx_match)
{
	struct rtw_para_pwrlmt_info_t *pwrlmt_info = NULL;
	struct _halrf_file_regd_ext *pregd_codemap = NULL;
	struct _halrf_file_regd_ext *array = NULL;
	struct rtw_hal_com_t *hal = rf->hal_com;
	u16 i, j, regd_idx = 0xff;

	if (aidx_match)
		*aidx_match = -1;

	pwrlmt_info = &rf->phl_com->phy_sw_cap[HW_PHY_0].rf_txpwrlmt_info;
	pregd_codemap = (struct _halrf_file_regd_ext *) pwrlmt_info->ext_reg_codemap;

	if (pwrlmt_info->para_src == 0) {
		RF_DBG(rf, DBG_RF_POWER, "======>%s   is_form_folder=%d   Return!!!\n",
			__func__, pwrlmt_info->para_src);
		return 0xff;
	}

	RF_DBG(rf, DBG_RF_POWER, "======>%s  total_regd_idx=%d  extra_limit_idx=%d\n",
		__func__, pwrlmt_info->ext_regd_arridx, pwrlmt_info->ext_reg_map_num);

	for (i = 0; i <= pwrlmt_info->ext_regd_arridx; i++)
		RF_DBG(rf, DBG_RF_POWER, "ext_regd_name[%d]=%s\n", i, pwrlmt_info->ext_regd_name[i]);

	RF_DBG(rf, DBG_RF_POWER, "======>   Start domain code Search\n");
	for (i = 0; i < pwrlmt_info->ext_reg_map_num; i++) {
		array = (struct _halrf_file_regd_ext *)&pregd_codemap[i];

		RF_DBG(rf, DBG_RF_POWER, "======>   %d\n", i);
		RF_DBG(rf, DBG_RF_POWER, "ent->domain=0x%x\n", array->domain);
		RF_DBG(rf, DBG_RF_POWER, "ent->country=%s\n", array->country);
		RF_DBG(rf, DBG_RF_POWER, "ent->reg_name=%s\n", array->reg_name);

		if (domain_code == array->domain) {
			RF_DBG(rf, DBG_RF_POWER, "Search rg_info.domain_code == Power limit array->domain\n");
			for (j = 0; j <= pwrlmt_info->ext_regd_arridx; j++) {
				if (_os_strcmp(array->reg_name, pwrlmt_info->ext_regd_name[j]) == 0) {
					regd_idx = j;
					if (aidx_match)
						*aidx_match = i;
					i = pwrlmt_info->ext_reg_map_num;
					RF_DBG(rf, DBG_RF_POWER, "======>   Search regd_idx=%d\n", regd_idx);
					break;
				}
			}
		}
	}
	RF_DBG(rf, DBG_RF_POWER, "<======   End domain code Search\n");

	RF_DBG(rf, DBG_RF_POWER, "======>   Start Country code Search\n");
	for (i = 0; i < pwrlmt_info->ext_reg_map_num; i++) {
		array = (struct _halrf_file_regd_ext *)&pregd_codemap[i];
		
		RF_DBG(rf, DBG_RF_POWER, "======>   %d\n", i);
		RF_DBG(rf, DBG_RF_POWER, "ent->domain=0x%x\n", array->domain);
		RF_DBG(rf, DBG_RF_POWER, "ent->country=%s\n", array->country);
		RF_DBG(rf, DBG_RF_POWER, "ent->reg_name=%s\n", array->reg_name);

		if (hal_mem_cmp(hal, country, array->country, 2) == 0 && _os_strcmp(country, "") != 0) {
			RF_DBG(rf, DBG_RF_POWER, "Search rg_info.country == Power limit array->country\n");
			for (j = 0; j <= pwrlmt_info->ext_regd_arridx; j++) {
				if (_os_strcmp(array->reg_name, pwrlmt_info->ext_regd_name[j]) == 0) {
					regd_idx = j;
					if (aidx_match)
						*aidx_match = i;
					i = pwrlmt_info->ext_reg_map_num;
					RF_DBG(rf, DBG_RF_POWER, "======>   Search regd_idx=%d\n", regd_idx);
					break;
				}
			}
		}
	}
	RF_DBG(rf, DBG_RF_POWER, "<======   End Country code Search\n");

	return (u8)regd_idx;
}

void halrf_reload_pwr_limit_tbl_and_set(struct rf_info *rf,
	enum phl_phy_idx phy, enum phl_pwr_table pwr_table)
{
	struct rtw_para_pwrlmt_info_t *pwrlmt_info = NULL;
	
	bool is_form_folder;
	u32 folder_len;
	u32 *folder_array;

	RF_DBG(rf, DBG_RF_POWER, "======>%s\n", __func__);

	if (pwr_table == PWR_LIMIT) {
		pwrlmt_info = &rf->phl_com->phy_sw_cap[HW_PHY_0].rf_txpwrlmt_info;

		is_form_folder = pwrlmt_info->para_src;
		folder_len = pwrlmt_info->para_data_len;
		folder_array = pwrlmt_info->para_data;

		RF_DBG(rf, DBG_RF_POWER, "Reload power limit table and set. is_form_folder=%d folder_len=%d\n",
			is_form_folder, folder_len);

		halrf_config_store_power_limit(rf , is_form_folder,
				folder_len, folder_array);

		halrf_set_power(rf, phy, pwr_table);
	}

	if (pwr_table == PWR_LIMIT_RU) {
		pwrlmt_info = &rf->phl_com->phy_sw_cap[HW_PHY_0].rf_txpwrlmt_ru_info;

		is_form_folder = pwrlmt_info->para_src;
		folder_len = pwrlmt_info->para_data_len;
		folder_array = pwrlmt_info->para_data;
		
		RF_DBG(rf, DBG_RF_POWER, "Reload power limit RU table and set. is_form_folder=%d folder_len=%d\n",
			is_form_folder, folder_len);

		halrf_config_store_power_limit_ru(rf , is_form_folder,
				folder_len, folder_array);

		halrf_set_power(rf, phy, pwr_table);
	}

	RF_DBG(rf, DBG_RF_POWER, "<======%s finish!\n", __func__);
}
#endif

void halrf_modify_pwr_table_bitmask(struct rf_info *rf,
	enum phl_phy_idx phy, enum phl_pwr_table pwr_table)
{
	struct rtw_tpu_info *tpu = &rf->hal_com->band[phy].rtw_tpu_i;
	struct rtw_tpu_pwr_by_rate_info *rate = &tpu->rtw_tpu_pwr_by_rate_i;
	struct rtw_tpu_pwr_imt_info *lmt = &rf->hal_com->band[phy].rtw_tpu_i.rtw_tpu_pwr_imt_i;
	
	u8 i, j, k;

	if (pwr_table & PWR_BY_RATE) {
		for (i = 0; i < TPU_SIZE_PWR_TAB_lGCY; i++)
			rate->pwr_by_rate_lgcy[i] &= 0x7f;

		for (i = 0; i < HAL_MAX_PATH; i++) {
			for (j = 0; j < TPU_SIZE_PWR_TAB; j++)
				rate->pwr_by_rate[i][j] &= 0x7f;
		}
	}

	if (pwr_table & PWR_LIMIT) {
		for (i = 0; i < HAL_MAX_PATH; i++) {
			for (k = 0; k < TPU_SIZE_BF; k++) {
				lmt->pwr_lmt_cck_20m[i][k] &= 0x7f;
				lmt->pwr_lmt_cck_40m[i][k] &= 0x7f;
				
				lmt->pwr_lmt_lgcy_20m[i][k] &= 0x7f;

				for (j = 0; j < TPU_SIZE_BW20_SC; j++)
					lmt->pwr_lmt_20m[i][j][k] &= 0x7f;

				for (j = 0; j < TPU_SIZE_BW40_SC; j++)
					lmt->pwr_lmt_40m[i][j][k] &= 0x7f;

				for (j = 0; j < TPU_SIZE_BW80_SC; j++)
					lmt->pwr_lmt_80m[i][j][k] &= 0x7f;

				lmt->pwr_lmt_160m[i][k] &= 0x7f;
				lmt->pwr_lmt_40m_2p5[i][k] &= 0x7f;
				lmt->pwr_lmt_40m_0p5[i][k] &= 0x7f;
			}
		}
	}

	if (pwr_table & PWR_LIMIT_RU) {
		for (i = 0; i < HAL_MAX_PATH; i++)
			for (j = 0; j < TPU_SIZE_RUA; j++)
				for (k = 0; k < TPU_SIZE_BW20_SC; k++)
					tpu->pwr_lmt_ru[i][j][k] &= 0x7f;
	}
}

s8 halrf_get_pwr_control(struct rf_info *rf, enum phl_phy_idx phy)
{
	struct halrf_pwr_info *pwr = &rf->pwr_info;
	s8 tmp;

	tmp = (pwr->power_constraint[phy] * -1) + pwr->dpk_mcc_power;
	
	return tmp;
}

s8 halrf_get_tx_rate_pwr_control(struct rf_info *rf, enum phl_phy_idx phy)
{
	struct halrf_pwr_info *pwr = &rf->pwr_info;
	s8 tmp;

	tmp = pwr->tx_rate_power_control[phy];
	
	return tmp;
}

bool halrf_pwr_is_minus(struct rf_info *rf, u32 reg_tmp)
{
	return reg_tmp & BIT(6);
}

s32 halrf_show_pwr_table(struct rf_info *rf, u32 reg_tmp)
{
	s32 tmp;

	(reg_tmp & BIT(6)) ? (tmp = (reg_tmp | 0xffffff80)) : (tmp = reg_tmp);

	if (tmp < 0)
		return tmp * -1 * 10 / 2;
	else
		return tmp * 10 / 2;
}

/*mb = 100 * dB*/
bool halrf_set_power_constraint (void *rf_void, enum phl_phy_idx phy_idx, u16 mb, bool apply_to_hw)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct halrf_pwr_info *pwr = &rf->pwr_info;

	pwr->power_constraint[phy_idx] = (u8)((mb * 4) / 100);

	if (apply_to_hw) {
		halrf_set_power(rf, phy_idx, PWR_BY_RATE);
		halrf_set_power(rf, phy_idx, PWR_LIMIT);
		halrf_set_power(rf, phy_idx, PWR_LIMIT_RU);
	}
	return true;
}

/*mb = 100 * dB*/
bool halrf_control_tx_rate_power (void *rf_void, enum phl_phy_idx phy_idx, s32 mb)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct halrf_pwr_info *pwr = &rf->pwr_info;

	pwr->tx_rate_power_control[phy_idx] = (s8)((mb * 4) / 100);

	halrf_set_power(rf, phy_idx, PWR_BY_RATE);

	return true;
}

void halrf_set_tpe_default_control(struct rf_info *rf, enum phl_phy_idx phy)
{
	RF_DBG(rf, DBG_RF_POWER, "[TPE] ======>%s\n", __func__);

	halrf_set_power(rf, phy, PWR_LIMIT | PWR_LIMIT_RU);
}

void halrf_set_eirp_tpe_control(struct rf_info *rf, enum phl_phy_idx phy, s8 *power, u8 valid_tpe_cnt)
{
	struct rtw_tpu_pwr_imt_info *lmt = &rf->hal_com->band[phy].rtw_tpu_i.rtw_tpu_pwr_imt_i;
	struct rtw_hal_com_t *hal = rf->hal_com;
	u32 i, j, k;
	s8 power_20m, power_40m, power_80m, power_160m;

	/* pwr_intpn = PWR_INTPN_EIRP(1)
	* valid_pwr_cnt = 1, 2, 3, 4, otherwise invalid
	* max_tx_pwr[0] maximum transmit power for 20 MHz
	* max_tx_pwr[1] maximum transmit power for 40 MHz
	* max_tx_pwr[2] maximum transmit power for 80 MHz
	* max_tx_pwr[3] maximum transmit power for 160/80+80 MHz
	*/

	RF_DBG(rf, DBG_RF_POWER, "======>%s valid_tpe_cnt=%d\n", __func__, valid_tpe_cnt);

	for (i = 0; i < TX_PWR_LIMIT_NUM_MAC; i = i + 4) {
		RF_DBG(rf, DBG_RF_POWER, "Get MAC 0x%x=0x%08x\n",
			(0xd2ec + i), halrf_mac_get_pwr_reg(rf, phy, (0xd2ec + i), 0xffffffff));
	}

	if (valid_tpe_cnt == 2) {
		/*max_tx_pwr[1] */
		power_20m = power[0];
		power_40m = power[1];
		power_80m = power[0];
		power_160m = power[0];
		RF_DBG(rf, DBG_RF_POWER, "power_20m=%d power_40m=%d power_80m=%d power_160m=%d\n",
			power_20m, power_40m, power_80m, power_160m);
	} else if (valid_tpe_cnt == 3) {
		/*max_tx_pwr[2] */
		power_20m = power[0];
		power_40m = power[1];
		power_80m = power[2];
		power_160m = power[0];
		RF_DBG(rf, DBG_RF_POWER, "power_20m=%d power_40m=%d power_80m=%d power_160m=%d\n",
			power_20m, power_40m, power_80m, power_160m);
	} else if (valid_tpe_cnt == 4) {
		/*max_tx_pwr[3] */
		power_20m = power[0];
		power_40m = power[1];
		power_80m = power[2];
		power_160m = power[3];
		RF_DBG(rf, DBG_RF_POWER, "power_20m=%d power_40m=%d power_80m=%d power_160m=%d\n",
			power_20m, power_40m, power_80m, power_160m);
	} else {
		/*max_tx_pwr[0] */
		power_20m = power[0];
		power_40m = power[0];
		power_80m = power[0];
		power_160m = power[0];
		RF_DBG(rf, DBG_RF_POWER, "power_20m=%d power_40m=%d power_80m=%d power_160m=%d\n",
			power_20m, power_40m, power_80m, power_160m);
	}

	for (i = 0; i < HAL_MAX_PATH; i++) {
		for (j = 0; j < TPU_SIZE_BF; j++) {
			RF_DBG(rf, DBG_RF_POWER, "lmt->pwr_lmt_lgcy_20m[%d][%d]=%d power_20m=%d\n",
				i, j, lmt->pwr_lmt_lgcy_20m[i][j], power_20m);
			if (lmt->pwr_lmt_lgcy_20m[i][j] >= power_20m) {
				lmt->pwr_lmt_lgcy_20m[i][j] = power_20m;
			}
		}
	}

	for (i = 0; i < HAL_MAX_PATH; i++) {
		for (j = 0; j < TPU_SIZE_BW20_SC; j++) {
			for (k = 0; k < TPU_SIZE_BF; k++) {
				RF_DBG(rf, DBG_RF_POWER, "lmt->pwr_lmt_20m[%d][%d][%d]=%d power_20m=%d\n",
					i, j, k, lmt->pwr_lmt_20m[i][j][k], power_20m);
				if (lmt->pwr_lmt_20m[i][j][k] >= power_20m) {
					lmt->pwr_lmt_20m[i][j][k] = power_20m;
				}
			}
		}
	}

	for (i = 0; i < HAL_MAX_PATH; i++) {
		for (j = 0; j < TPU_SIZE_BW40_SC; j++) {
			for (k = 0; k < TPU_SIZE_BF; k++) {
				RF_DBG(rf, DBG_RF_POWER, "lmt->pwr_lmt_40m[%d][%d][%d]=%d power_40m=%d\n",
					i, j, k, lmt->pwr_lmt_40m[i][j][k], power_40m);
				if (lmt->pwr_lmt_40m[i][j][k] >= power_40m) {
					lmt->pwr_lmt_40m[i][j][k] = power_40m;
				}
			}
		}
	}

	for (i = 0; i < HAL_MAX_PATH; i++) {
		for (j = 0; j < TPU_SIZE_BW80_SC; j++) {
			for (k = 0; k < TPU_SIZE_BF; k++) {
				RF_DBG(rf, DBG_RF_POWER, "lmt->pwr_lmt_80m[%d][%d][%d]=%d power_80m=%d\n",
					i, j, k, lmt->pwr_lmt_80m[i][j][k], power_80m);
				if (lmt->pwr_lmt_80m[i][j][k] >= power_80m) {
					lmt->pwr_lmt_80m[i][j][k] = power_80m;
				}
			}
		}
	}

	for (i = 0; i < HAL_MAX_PATH; i++) {
		for (k = 0; k < TPU_SIZE_BF; k++) {
			RF_DBG(rf, DBG_RF_POWER, "lmt->pwr_lmt_160m[%d][%d]=%d power_160m=%d\n",
				i, k, lmt->pwr_lmt_160m[i][k], power_160m);
			if (lmt->pwr_lmt_160m[i][k] >= power_160m) {
				lmt->pwr_lmt_160m[i][k] = power_160m;
			}
		}
	}

	for (i = 0; i < HAL_MAX_PATH; i++) {
		for (k = 0; k < TPU_SIZE_BF; k++) {
			RF_DBG(rf, DBG_RF_POWER, "lmt->pwr_lmt_40m_0p5[%d][%d]=%d power_20m=%d\n",
				i, k, lmt->pwr_lmt_40m_0p5[i][k], power_20m);
			if (lmt->pwr_lmt_40m_0p5[i][k] >= power_20m) {
				lmt->pwr_lmt_40m_0p5[i][k] = power_20m;
			}
		}
	}

	for (i = 0; i < HAL_MAX_PATH; i++) {
		for (k = 0; k < TPU_SIZE_BF; k++) {
			RF_DBG(rf, DBG_RF_POWER, "lmt->pwr_lmt_40m_2p5[%d][%d]=%d power_20m=%d\n",
				i, k, lmt->pwr_lmt_40m_2p5[i][k], power_20m);
			if (lmt->pwr_lmt_40m_2p5[i][k] >= power_20m) {
				lmt->pwr_lmt_40m_2p5[i][k] = power_20m;
			}
		}
	}

	halrf_mac_write_pwr_limit_reg(rf, phy);

	if (rf->dbg_component & DBG_RF_POWER) {
		halrf_delay_ms(rf, 100);

		for (i = 0; i < TX_PWR_LIMIT_NUM_MAC; i = i + 4)
			RF_DBG(rf, DBG_RF_POWER, "Get MAC 0x%x=0x%08x\n",
				(0xd2ec + i), halrf_mac_get_pwr_reg(rf, phy, (0xd2ec + i), 0xffffffff));
	}
}

void halrf_set_eirp_psd_special_tpe_control(struct rf_info *rf, enum phl_phy_idx phy, s8 power)
{
	struct rtw_tpu_pwr_imt_info *lmt = &rf->hal_com->band[phy].rtw_tpu_i.rtw_tpu_pwr_imt_i;
	struct rtw_hal_com_t *hal = rf->hal_com;
	u32 i, j, k;
	u32 mlog20m = 1301;
	s32 eirp_psd_pwr;

	/* pwr_intpn = PWR_INTPN_EIRP_PSD(2)
	* valid_pwr_cnt = 0, 1, 2, 4, 8, otherwise invalid
	* valid_pwr_cnt = 0 is a special case
	*                 use one max_tx_pwr to represent any bw's PSD
	* maximum transmit power spectral density (PSD)
	* max_tx_pwr[0] maximum transmit PSD 1
	* max_tx_pwr[1] maximum transmit PSD 2
	* ...
	* max_tx_pwr[7] maximum transmit PSD 8
	*/

	eirp_psd_pwr = power + mlog20m * 2 / 100;

	RF_DBG(rf, DBG_RF_POWER, "======>%s power=%d eirp_psd_pwr=%d\n",
		__func__, power, eirp_psd_pwr);

	for (i = 0; i < TX_PWR_LIMIT_NUM_MAC; i = i + 4) {
		RF_DBG(rf, DBG_RF_POWER, "Get MAC 0x%x=0x%08x\n",
			(0xd2ec + i), halrf_mac_get_pwr_reg(rf, phy, (0xd2ec + i), 0xffffffff));
	}	

	RF_DBG(rf, DBG_RF_POWER, "==============================\n");

	for (i = 0; i < HAL_MAX_PATH; i++) {
		for (j = 0; j < TPU_SIZE_BF; j++) {
			if (lmt->pwr_lmt_lgcy_20m[i][j] >= eirp_psd_pwr) {
				lmt->pwr_lmt_lgcy_20m[i][j] = (s8)eirp_psd_pwr;
			}
		}
	}

	for (i = 0; i < HAL_MAX_PATH; i++) {
		for (j = 0; j < TPU_SIZE_BW20_SC; j++) {
			for (k = 0; k < TPU_SIZE_BF; k++) {
				if (lmt->pwr_lmt_20m[i][j][k] >= eirp_psd_pwr) {
					lmt->pwr_lmt_20m[i][j][k] = (s8)eirp_psd_pwr;
				}
			}
		}
	}

	for (i = 0; i < HAL_MAX_PATH; i++) {
		for (j = 0; j < TPU_SIZE_BW40_SC; j++) {
			for (k = 0; k < TPU_SIZE_BF; k++) {
				if (lmt->pwr_lmt_40m[i][j][k] >= eirp_psd_pwr) {
					lmt->pwr_lmt_40m[i][j][k] = (s8)eirp_psd_pwr;
				}
			}
		}
	}

	for (i = 0; i < HAL_MAX_PATH; i++) {
		for (j = 0; j < TPU_SIZE_BW80_SC; j++) {
			for (k = 0; k < TPU_SIZE_BF; k++) {
				if (lmt->pwr_lmt_80m[i][j][k] >= eirp_psd_pwr) {
					lmt->pwr_lmt_80m[i][j][k] = (s8)eirp_psd_pwr;
				}
			}
		}
	}

	for (i = 0; i < HAL_MAX_PATH; i++) {
		for (k = 0; k < TPU_SIZE_BF; k++) {
			if (lmt->pwr_lmt_160m[i][k] >= eirp_psd_pwr) {
				lmt->pwr_lmt_160m[i][k] = (s8)eirp_psd_pwr;
			}
		}
	}

	for (i = 0; i < HAL_MAX_PATH; i++) {
		for (k = 0; k < TPU_SIZE_BF; k++) {
			if (lmt->pwr_lmt_40m_0p5[i][k] >= eirp_psd_pwr) {
				lmt->pwr_lmt_40m_0p5[i][k] = (s8)eirp_psd_pwr;
			}
		}
	}

	for (i = 0; i < HAL_MAX_PATH; i++) {
		for (k = 0; k < TPU_SIZE_BF; k++) {
			if (lmt->pwr_lmt_40m_2p5[i][k] >= eirp_psd_pwr) {
				lmt->pwr_lmt_40m_2p5[i][k] = (s8)eirp_psd_pwr;
			}
		}
	}

	halrf_mac_write_pwr_limit_reg(rf, phy);

	if (rf->dbg_component & DBG_RF_POWER) {
		halrf_delay_ms(rf, 100);

		for (i = 0; i < TX_PWR_LIMIT_NUM_MAC; i = i + 4) {
			RF_DBG(rf, DBG_RF_POWER, "Get MAC 0x%x=0x%08x\n",
				(0xd2ec + i), halrf_mac_get_pwr_reg(rf, phy, (0xd2ec + i), 0xffffffff));
		}
	}
}

void halrf_set_eirp_psd_tpe_control(struct rf_info *rf, enum phl_phy_idx phy, s8* power, u8 valid_tpe_cnt)
{
	struct rtw_tpu_pwr_imt_info *lmt = &rf->hal_com->band[phy].rtw_tpu_i.rtw_tpu_pwr_imt_i;
	struct rtw_hal_com_t *hal = rf->hal_com;
	u32 i, j, k;
	s8 power_40m, power_80m, power_160m;
	u32 mlog20m = 1301;
	s32 pwr[MAX_TPE_TX_PWR_CNT] = {0};

	/* pwr_intpn = PWR_INTPN_EIRP_PSD(2)
	* valid_pwr_cnt = 0, 1, 2, 4, 8, otherwise invalid
	* valid_pwr_cnt = 0 is a special case
	*                 use one max_tx_pwr to represent any bw's PSD
	* maximum transmit power spectral density (PSD)
	* max_tx_pwr[0] maximum transmit PSD 1
	* max_tx_pwr[1] maximum transmit PSD 2
	* ...
	* max_tx_pwr[7] maximum transmit PSD 8
	*/

	RF_DBG(rf, DBG_RF_POWER, "======>%s valid_tpe_cnt=%d\n",
		__func__, valid_tpe_cnt);

	for (i = 0; i < TX_PWR_LIMIT_NUM_MAC; i = i + 4) {
		RF_DBG(rf, DBG_RF_POWER, "Get MAC 0x%x=0x%08x\n",
			(0xd2ec + i), halrf_mac_get_pwr_reg(rf, phy, (0xd2ec + i), 0xffffffff));
	}

	for (i = 0; i < valid_tpe_cnt; i++) {
		pwr[i] = power[i] + mlog20m * 2 / 100;

		RF_DBG(rf, DBG_RF_POWER, "pwr[%d]=%d\n",
			i, pwr[i]);
	}

	for (i = 0; i < HAL_MAX_PATH; i++) {
		for (j = 0; j < TPU_SIZE_BF; j++) {
			if (lmt->pwr_lmt_lgcy_20m[i][j] >= pwr[0]) {
				lmt->pwr_lmt_lgcy_20m[i][j] = (s8)pwr[0];
			}
		}
	}

	for (i = 0; i < HAL_MAX_PATH; i++) {
		for (j = 0; j < TPU_SIZE_BW20_SC; j++) {
			for (k = 0; k < TPU_SIZE_BF; k++) {
				RF_DBG(rf, DBG_RF_POWER, "lmt->pwr_lmt_20m[%d][%d][%d]=%d pwr[%d]=%d\n",
					i, j, k, lmt->pwr_lmt_20m[i][j][k], j, pwr[j]);

				if (lmt->pwr_lmt_20m[i][j][k] >= pwr[j]) {
					lmt->pwr_lmt_20m[i][j][k] = (s8)pwr[j];
				}
			}
		}
	}

	for (i = 0; i < HAL_MAX_PATH; i++) {
		for (j = 0; j < TPU_SIZE_BW40_SC; j++) {
			for (k = 0; k < TPU_SIZE_BF; k++) {
				power_40m = (s8)((pwr[i * 2] + pwr[i * 2 + 1]) / 2);

				RF_DBG(rf, DBG_RF_POWER,
					"power_40m(%d) = (pwr[%d](%d) + pwr[%d](%d)) / 2\n",
					power_40m, i * 2, pwr[i * 2], i * 2 + 1, pwr[i * 2 + 1]);
		
				if (lmt->pwr_lmt_40m[i][j][k] >= power_40m) {
					lmt->pwr_lmt_40m[i][j][k] = power_40m;
				}
			}
		}
	}

	for (i = 0; i < HAL_MAX_PATH; i++) {
		for (j = 0; j < TPU_SIZE_BW80_SC; j++) {
			for (k = 0; k < TPU_SIZE_BF; k++) {
				power_80m =
					(s8)((pwr[i * 4] + pwr[i * 4 + 1] +
					pwr[i * 4 + 2] + pwr[i * 4 + 3]) / 4);

				RF_DBG(rf, DBG_RF_POWER,
					"power_80m(%d) = (pwr[%d](%d) + pwr[%d](%d) + pwr[%d](%d) + pwr[%d](%d)) / 4\n",
					power_80m, i * 4, pwr[i * 4],
					i * 4 + 1, pwr[i * 4 + 1],
					i * 4 + 2, pwr[i * 4 + 2],
					i * 4 + 3, pwr[i * 4 + 3]);
				
				if (lmt->pwr_lmt_80m[i][j][k] >= power_80m) {
					lmt->pwr_lmt_80m[i][j][k] = power_80m;
				}
			}
		}
	}

	for (i = 0; i < HAL_MAX_PATH; i++) {
		for (k = 0; k < TPU_SIZE_BF; k++) {
			power_160m =
				(s8)((pwr[0] + pwr[1] + pwr[2] + pwr[3] +
				pwr[4] + pwr[5] + pwr[6] + pwr[7]) / 8);

			RF_DBG(rf, DBG_RF_POWER, "power_160m(%d)\n", power_160m);

			if (lmt->pwr_lmt_160m[i][k] >= power_160m) {
				lmt->pwr_lmt_160m[i][k] = power_160m;
			}
		}
	}

	for (i = 0; i < HAL_MAX_PATH; i++) {
		for (k = 0; k < TPU_SIZE_BF; k++) {
			power_40m = (s8)((pwr[0] + pwr[1]) / 2);

			RF_DBG(rf, DBG_RF_POWER,
				"power_40m(%d) = (pwr[0](%d) + pwr[1](%d)) / 2\n",
				power_40m, pwr[0], pwr[1]);

			if (lmt->pwr_lmt_40m_0p5[i][k] >= power_40m) {
				lmt->pwr_lmt_40m_0p5[i][k] = power_40m;
			}
		}
	}

	for (i = 0; i < HAL_MAX_PATH; i++) {
		for (k = 0; k < TPU_SIZE_BF; k++) {
			power_40m = (s8)((pwr[0] + pwr[1]) / 2);

			RF_DBG(rf, DBG_RF_POWER,
				"power_40m(%d) = (pwr[0](%d) + pwr[1](%d)) / 2\n",
				power_40m, pwr[0], pwr[1]);

			if (lmt->pwr_lmt_40m_2p5[i][k] >= power_40m) {
				lmt->pwr_lmt_40m_2p5[i][k] = power_40m;
			}
		}
	}

	halrf_mac_write_pwr_limit_reg(rf, phy);

	if (rf->dbg_component & DBG_RF_POWER) {
		halrf_delay_ms(rf, 100);

		for (i = 0; i < TX_PWR_LIMIT_NUM_MAC; i = i + 4) {
			RF_DBG(rf, DBG_RF_POWER, "Get MAC 0x%x=0x%08x\n",
				(0xd2ec + i), halrf_mac_get_pwr_reg(rf, phy, (0xd2ec + i), 0xffffffff));
		}
	}
}

void halrf_set_tpe_control(struct rf_info *rf)
{
	RF_DBG(rf, DBG_RF_POWER, "[TPE] ======>%s hal_com->dbcc_en=%d\n",
		__func__, rf->hal_com->dbcc_en);

	if (rf->hal_com->dbcc_en) {
		halrf_set_tpe_control_dbcc(rf,HW_PHY_0);
		halrf_set_tpe_control_dbcc(rf,HW_PHY_1);
	} else
		halrf_set_tpe_control_dbcc(rf,HW_PHY_0);
}

void halrf_set_tpe_control_dbcc(struct rf_info *rf, enum phl_phy_idx phy)
{
	struct rtw_hal_com_t *hal = rf->hal_com;
	struct rtw_tpe_info_t *tpe = &rf->phl_com->tpe_info;
	struct rtw_tpu_info *tpu = &rf->hal_com->band[phy].rtw_tpu_i;
	struct rtw_tpu_pwr_imt_info *lmt = &rf->hal_com->band[phy].rtw_tpu_i.rtw_tpu_pwr_imt_i;
	u32 band = rf->hal_com->band[phy].cur_chandef.band;
	u32 i, j;
	//u32 mlog20m = 1301;
	//s32 eirp_psd_pwr;
	//mlog40m = 1602, mlog80m = 1903, mlog160m = 2204

#if 0
	tpe->valid_tpe_cnt = 1;
	tpe->r_tpe[0].pwr_intpn = PWR_INTPN_EIRP;
	tpe->r_tpe[0].valid_pwr_cnt = 1;
	tpe->r_tpe[0].max_tx_pwr[0] = 20;

	tpe->valid_tpe_cnt = 1;
	tpe->r_tpe[0].pwr_intpn = PWR_INTPN_EIRP_PSD;
	tpe->r_tpe[0].valid_pwr_cnt = 0;
	tpe->r_tpe[0].max_tx_pwr[0] = 0xf0;

	tpe->valid_tpe_cnt = 1;
	tpe->r_tpe[0].pwr_intpn = PWR_INTPN_EIRP_PSD;
	tpe->r_tpe[0].valid_pwr_cnt = 2;
	tpe->r_tpe[0].max_tx_pwr[0] = 0xf0;
	tpe->r_tpe[0].max_tx_pwr[1] = 0x2;
#endif

	RF_DBG(rf, DBG_RF_POWER, "[TPE] ======>%s\n", __func__);

	RF_DBG(rf, DBG_RF_POWER, "[TPE] tpe->valid_tpe_cnt = 0x%x\n", tpe->valid_tpe_cnt);

	for (i = 0; i < tpe->valid_tpe_cnt; i++) {
		RF_DBG(rf, DBG_RF_POWER, "[TPE] tpe->r_tpe[%d]->pwr_intpn = 0x%x\n",
			i, tpe->r_tpe[i].pwr_intpn);

		RF_DBG(rf, DBG_RF_POWER, "[TPE] tpe->r_tpe[%d]->valid_pwr_cnt = 0x%x\n",
			i, tpe->r_tpe[i].valid_pwr_cnt);

		for (j = 0; j <= tpe->r_tpe[i].valid_pwr_cnt; j++)
			RF_DBG(rf, DBG_RF_POWER, "[TPE] tpe->r_tpe[%d]->max_tx_pwr[%d] = 0x%x\n",
				i, j, tpe->r_tpe[i].max_tx_pwr[j]);
	}

	if (!(rf->support_ability & HAL_RF_TPE_CTRL)) {
		RF_DBG(rf, DBG_RF_POWER, "[TPE] ======>%s  rf->support_ability=0x%x Return !!!\n",
			__func__, rf->support_ability);
		halrf_set_tpe_default_control(rf, phy);
		return;
	}

	for (i = 0; i <= tpe->valid_tpe_cnt; i++) {
		if (band == BAND_ON_6G) {
			if (tpe->r_tpe[i].pwr_intpn == PWR_INTPN_UNDEFINED) {
				halrf_set_tpe_default_control(rf, phy);
			}

			if (tpe->r_tpe[i].pwr_intpn == PWR_INTPN_EIRP) {
				if (tpe->r_tpe[i].valid_pwr_cnt == 1 ||
					tpe->r_tpe[i].valid_pwr_cnt == 2 ||
					tpe->r_tpe[i].valid_pwr_cnt == 3 ||
					tpe->r_tpe[i].valid_pwr_cnt == 4) {
					halrf_set_eirp_tpe_control(rf, phy,
						(s8 *)tpe->r_tpe[i].max_tx_pwr,
						tpe->r_tpe[i].valid_pwr_cnt);
				} else
					halrf_set_tpe_default_control(rf, phy);
			}

			if (tpe->r_tpe[i].pwr_intpn == PWR_INTPN_EIRP_PSD) {
				if (tpe->r_tpe[i].valid_pwr_cnt == 0) {
					halrf_set_eirp_psd_special_tpe_control(rf, phy, (s8)tpe->r_tpe[i].max_tx_pwr[0]);	
				} else if (tpe->r_tpe[i].valid_pwr_cnt >= 1 && tpe->r_tpe[i].valid_pwr_cnt <= 7) {
					halrf_set_eirp_psd_tpe_control(rf, phy,
						(s8 *)tpe->r_tpe[i].max_tx_pwr,
						tpe->r_tpe[i].valid_pwr_cnt);
				} else
					halrf_set_tpe_default_control(rf, phy);
			} else
				halrf_set_tpe_default_control(rf, phy);
		} else {
			halrf_set_tpe_default_control(rf, phy);
		}
	}
}

void halrf_set_ant_gain_offset(struct rf_info *rf,
	enum phl_phy_idx phy, struct rtw_phl_regu_dyn_ant_gain *regu)
{
	struct halrf_pwr_info *pwr = &rf->pwr_info;
	u32 bw, rate, bf, ch, tx_num;
	s8 ext_pwr = 0;

	RF_DBG(rf, DBG_RF_POWER, "======>%s\n", __func__);
	
	RF_DBG(rf, DBG_RF_POWER, "regu=%d  ag_2g_comp=%d  ag_5g_comp=%d  regu->ag_6g_comp=%d\n",
		regu->regu, regu->ag_2g_comp, regu->ag_5g_comp, regu->ag_6g_comp);
	
	if (regu->regu < PW_LMT_MAX_REGULATION_NUM) {
		pwr->ant_gain_reg[regu->regu] = regu->regu;
		pwr->ant_gain_2g_oft[regu->regu] = regu->ag_2g_comp;
		pwr->ant_gain_5g_oft[regu->regu] = regu->ag_5g_comp;
		pwr->ant_gain_6g_oft[regu->regu] = regu->ag_6g_comp;
	}

	for (tx_num = 0; tx_num < MAX_HALRF_PATH; tx_num++) {
		for (ch = 0; ch < PW_LMT_MAX_CHANNEL_NUMBER_2G; ch++) {
			for (bf = 0; bf < PW_LMT_MAX_BF_NUM; bf++) {
				for (rate = 0; rate < PW_LMT_MAX_RS_NUM; rate++) {
					for (bw = 0; bw < PW_LMT_MAX_2G_BANDWITH_NUM; bw++) {
						RF_DBG(rf, DBG_RF_POWER, "Org   pwr->tx_pwr_limit_2g[%d][%d][%d][%d][%d][%d] =   %d\n",
							regu->regu, bw, rate, bf, ch, tx_num ,
							pwr->tx_pwr_limit_2g[regu->regu][bw][rate][bf][ch][tx_num]);

						if (regu->regu < PW_LMT_MAX_REGULATION_NUM &&
							pwr->tx_pwr_limit_2g[regu->regu][bw][rate][bf][ch][tx_num] != -128 &&
							pwr->tx_pwr_limit_2g[regu->regu][bw][rate][bf][ch][tx_num] != 127) {
							pwr->tx_pwr_limit_2g[regu->regu][bw][rate][bf][ch][tx_num] =
								pwr->tx_pwr_limit_2g[regu->regu][bw][rate][bf][ch][tx_num] + regu->ag_2g_comp;
						}

						RF_DBG(rf, DBG_RF_POWER, "Chage pwr->tx_pwr_limit_2g[%d][%d][%d][%d][%d][%d] =   %d\n",
							regu->regu, bw, rate, bf, ch, tx_num ,
							pwr->tx_pwr_limit_2g[regu->regu][bw][rate][bf][ch][tx_num]);
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
						RF_DBG(rf, DBG_RF_POWER, "Org   pwr->tx_pwr_limit_5g[%d][%d][%d][%d][%d][%d]=   %d\n",
							regu->regu, bw, rate, bf, ch, tx_num,
							pwr->tx_pwr_limit_5g[regu->regu][bw][rate][bf][ch][tx_num]);

						if (regu->regu < PW_LMT_MAX_REGULATION_NUM &&
							pwr->tx_pwr_limit_5g[regu->regu][bw][rate][bf][ch][tx_num] != -128 &&
							pwr->tx_pwr_limit_5g[regu->regu][bw][rate][bf][ch][tx_num] != 127) {
							pwr->tx_pwr_limit_5g[regu->regu][bw][rate][bf][ch][tx_num] =
								pwr->tx_pwr_limit_5g[regu->regu][bw][rate][bf][ch][tx_num] + regu->ag_5g_comp;
						}

						RF_DBG(rf, DBG_RF_POWER, "Chage pwr->tx_pwr_limit_5g[%d][%d][%d][%d][%d][%d]=   %d\n",
							regu->regu, bw, rate, bf, ch, tx_num,
							pwr->tx_pwr_limit_5g[regu->regu][bw][rate][bf][ch][tx_num]);
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
						RF_DBG(rf, DBG_RF_POWER, "Org   pwr->tx_pwr_limit_6g[%d][%d][%d][%d][%d][%d] =	 %d\n",
							regu->regu, bw, rate, bf, ch, tx_num ,
							pwr->tx_pwr_limit_6g[regu->regu][bw][rate][bf][ch][tx_num]);

						if (regu->regu < PW_LMT_MAX_REGULATION_NUM &&
							pwr->tx_pwr_limit_6g[regu->regu][bw][rate][bf][ch][tx_num] != -128 &&
							pwr->tx_pwr_limit_6g[regu->regu][bw][rate][bf][ch][tx_num] != 127) {
							pwr->tx_pwr_limit_6g[regu->regu][bw][rate][bf][ch][tx_num] =
								pwr->tx_pwr_limit_6g[regu->regu][bw][rate][bf][ch][tx_num] + regu->ag_6g_comp;
						}

						RF_DBG(rf, DBG_RF_POWER, "Chage pwr->tx_pwr_limit_6g[%d][%d][%d][%d][%d][%d] =   %d\n",
							regu->regu, bw, rate, bf, ch, tx_num ,
							pwr->tx_pwr_limit_6g[regu->regu][bw][rate][bf][ch][tx_num]);
					}
				}
			}
		}
	}

	for (tx_num = 0; tx_num < MAX_HALRF_PATH; tx_num++) {
		for (ch = 0; ch < PW_LMT_MAX_CHANNEL_NUMBER_2G; ch++) {
			for (rate = 0; rate < PW_LMT_MAX_RS_NUM; rate++) {
				for (bw = 0; bw < PW_LMT_RU_BW_NULL; bw++) {
					RF_DBG(rf, DBG_RF_POWER, "Org   pwr->tx_pwr_limit_ru_2g[%d][%d][%d][%d][%d] =   %d\n",
						regu->regu, bw, rate, ch, tx_num,
						pwr->tx_pwr_limit_ru_2g[regu->regu][bw][rate][ch][tx_num]);

					if (regu->regu < PW_LMT_MAX_REGULATION_NUM &&
						pwr->tx_pwr_limit_ru_2g[regu->regu][bw][rate][ch][tx_num] != -128 &&
						pwr->tx_pwr_limit_ru_2g[regu->regu][bw][rate][ch][tx_num] != 127) {
						pwr->tx_pwr_limit_ru_2g[regu->regu][bw][rate][ch][tx_num] =
							pwr->tx_pwr_limit_ru_2g[regu->regu][bw][rate][ch][tx_num] + regu->ag_2g_comp;
					}

					RF_DBG(rf, DBG_RF_POWER, "Chage pwr->tx_pwr_limit_ru_2g[%d][%d][%d][%d][%d] =   %d\n",
						regu->regu, bw, rate, ch, tx_num,
						pwr->tx_pwr_limit_ru_2g[regu->regu][bw][rate][ch][tx_num]);
				}
			}
		}
	}

	for (tx_num = 0; tx_num < MAX_HALRF_PATH; tx_num++) {
		for (ch = 0; ch < PW_LMT_MAX_CHANNEL_NUMBER_5G; ch++) {
			for (rate = 0; rate < PW_LMT_MAX_RS_NUM; rate++) {
				for (bw = 0; bw < PW_LMT_RU_BW_NULL; bw++) {
					RF_DBG(rf, DBG_RF_POWER, "Org   pwr->tx_pwr_limit_ru_5g[%d][%d][%d][%d][%d]=   %d\n",
						regu->regu, bw, rate, ch, tx_num,
						pwr->tx_pwr_limit_ru_5g[regu->regu][bw][rate][ch][tx_num]);
					
					if (regu->regu < PW_LMT_MAX_REGULATION_NUM &&
						pwr->tx_pwr_limit_ru_5g[regu->regu][bw][rate][ch][tx_num] != -128 &&
						pwr->tx_pwr_limit_ru_5g[regu->regu][bw][rate][ch][tx_num] != 127) {
						pwr->tx_pwr_limit_ru_5g[regu->regu][bw][rate][ch][tx_num] =
							pwr->tx_pwr_limit_ru_5g[regu->regu][bw][rate][ch][tx_num] + regu->ag_5g_comp;
					}

					RF_DBG(rf, DBG_RF_POWER, "Chage pwr->tx_pwr_limit_ru_5g[%d][%d][%d][%d][%d]=   %d\n",
						regu->regu, bw, rate, ch, tx_num,
						pwr->tx_pwr_limit_ru_5g[regu->regu][bw][rate][ch][tx_num]);
				}
			}
		}
	}

	for (tx_num = 0; tx_num < MAX_HALRF_PATH; tx_num++) {
		for (ch = 0; ch < PW_LMT_MAX_CHANNEL_NUMBER_6G; ch++) {
			for (rate = 0; rate < PW_LMT_MAX_RS_NUM; rate++) {
				for (bw = 0; bw < PW_LMT_RU_BW_NULL; bw++) {
					RF_DBG(rf, DBG_RF_POWER, "Org   pwr->tx_pwr_limit_ru_6g[%d][%d][%d][%d][%d] =   %d\n",
						regu->regu, bw, rate, ch, tx_num,
						pwr->tx_pwr_limit_ru_6g[regu->regu][bw][rate][ch][tx_num]);
					
					if (regu->regu < PW_LMT_MAX_REGULATION_NUM &&
						pwr->tx_pwr_limit_ru_6g[regu->regu][bw][rate][ch][tx_num] != -128 &&
						pwr->tx_pwr_limit_ru_6g[regu->regu][bw][rate][ch][tx_num] != 127) {
						pwr->tx_pwr_limit_ru_6g[regu->regu][bw][rate][ch][tx_num] =
							pwr->tx_pwr_limit_ru_6g[regu->regu][bw][rate][ch][tx_num] + regu->ag_6g_comp;
					}

					RF_DBG(rf, DBG_RF_POWER, "Chage pwr->tx_pwr_limit_ru_6g[%d][%d][%d][%d][%d] =   %d\n",
						regu->regu, bw, rate, ch, tx_num,
						pwr->tx_pwr_limit_ru_6g[regu->regu][bw][rate][ch][tx_num]);
				}
			}
		}
	}

	RF_DBG(rf, DBG_RF_POWER, "<======%s   finish.\n", __func__);
}

void halrf_set_dynamic_ant_gain(struct rf_info *rf,
	enum phl_phy_idx phy, struct rtw_phl_regu_dyn_ant_gain *regu)
{
	RF_DBG(rf, DBG_RF_POWER, "======>%s\n", __func__);

	/*Get power limit, limit RU default value*/
	halrf_config_power_limit(rf, phy);
	halrf_config_power_limit_ru(rf, phy);

	/*Set Ant Gain offset to power limit, limit RU array*/
	halrf_set_ant_gain_offset(rf, phy, regu);

	/*Set to HW Register*/
	halrf_set_power(rf, phy, (PWR_LIMIT & PWR_LIMIT_RU));
}


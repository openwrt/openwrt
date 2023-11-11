
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
#ifndef _PHL_EXT_TX_PWR_LMT_DEF_H_
#define _PHL_EXT_TX_PWR_LMT_DEF_H_


struct rtw_phl_ext_pwr_lmt_info { /* external tx power limit information */
	s8 ext_pwr_lmt_2_4g[RTW_PHL_MAX_RF_PATH];
	s8 ext_pwr_lmt_5g_band1[RTW_PHL_MAX_RF_PATH]; /*CH36 ~ CH48*/
	s8 ext_pwr_lmt_5g_band2[RTW_PHL_MAX_RF_PATH]; /*CH52 ~ CH64*/
	s8 ext_pwr_lmt_5g_band3[RTW_PHL_MAX_RF_PATH]; /*CH100 ~ CH144*/
	s8 ext_pwr_lmt_5g_band4[RTW_PHL_MAX_RF_PATH]; /*CH149 ~ CH165*/
	s8 ext_pwr_lmt_6g_unii_5_1[RTW_PHL_MAX_RF_PATH]; /* CH1 ~ CH45 */
	s8 ext_pwr_lmt_6g_unii_5_2[RTW_PHL_MAX_RF_PATH]; /* CH49 ~ CH93 */
	s8 ext_pwr_lmt_6g_unii_6[RTW_PHL_MAX_RF_PATH]; /* CH97 ~ CH117 */
	s8 ext_pwr_lmt_6g_unii_7_1[RTW_PHL_MAX_RF_PATH]; /* CH121 ~ CH149 */
	s8 ext_pwr_lmt_6g_unii_7_2[RTW_PHL_MAX_RF_PATH]; /* CH153 ~ CH189 */
	s8 ext_pwr_lmt_6g_unii_8[RTW_PHL_MAX_RF_PATH]; /* CH193 ~ CH237 */
	/* for different power limit by antenna */
	s8 ext_pwr_lmt_ant_2_4g[RTW_PHL_MAX_RF_PATH];
	s8 ext_pwr_lmt_ant_5g_band1[RTW_PHL_MAX_RF_PATH]; /*CH36 ~ CH48*/
	s8 ext_pwr_lmt_ant_5g_band2[RTW_PHL_MAX_RF_PATH]; /*CH52 ~ CH64*/
	s8 ext_pwr_lmt_ant_5g_band3[RTW_PHL_MAX_RF_PATH]; /*CH100 ~ CH144*/
	s8 ext_pwr_lmt_ant_5g_band4[RTW_PHL_MAX_RF_PATH]; /*CH149 ~ CH165*/
	s8 ext_pwr_lmt_ant_6g_unii_5_1[RTW_PHL_MAX_RF_PATH]; /* CH1 ~ CH45 */
	s8 ext_pwr_lmt_ant_6g_unii_5_2[RTW_PHL_MAX_RF_PATH]; /* CH49 ~ CH93 */
	s8 ext_pwr_lmt_ant_6g_unii_6[RTW_PHL_MAX_RF_PATH]; /* CH97 ~ CH117 */
	s8 ext_pwr_lmt_ant_6g_unii_7_1[RTW_PHL_MAX_RF_PATH]; /* CH121 ~ CH149 */
	s8 ext_pwr_lmt_ant_6g_unii_7_2[RTW_PHL_MAX_RF_PATH]; /* CH153 ~ CH189 */
	s8 ext_pwr_lmt_ant_6g_unii_8[RTW_PHL_MAX_RF_PATH]; /* CH193 ~ CH237 */
};

struct rtw_phl_cmd_epl_para { /* phl cmd external power limit parameters */
	struct rtw_phl_ext_pwr_lmt_info info;
	u8 upt_info; /* info is updated or not, 0: No, 1: Yes */
	u8 en; /* ext power limit state, 0: disable, 1: enable */
	u8 pwrlmt_rld; /* reload power limit table, 0: No, 1: Yes */
};

enum rtw_phl_status
phl_cmd_updt_ext_txpwr_lmt(void *phl, u8 *param);

#endif /*_PHL_EXT_TX_PWR_LMT_DEF_H_*/

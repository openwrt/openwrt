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
#include "../halrf_precomp.h"

#ifdef RF_8852C_SUPPORT

s8 _halrf_avg_power_8852c(struct rf_info *rf, enum phl_phy_idx phy, s8 *value, s8 n)
{
	u8 i;
	s16 total = 0;

	RF_DBG(rf, DBG_RF_POWER, "=======>%s\n", __func__);

	for (i = 0; i < n; i++) {
		total = total + value[i];
		RF_DBG(rf, DBG_RF_POWER, "value[%d]=%d total=%d n=%d\n", i, value[i], total, n);
	}

	total = total / n;

	return (s8)total;
}

void _halrf_bub_sort_8852c(struct rf_info *rf, enum phl_phy_idx phy, s8 *data, u32 n)
{
	s32 i, j, sp;
	s8 temp;
	u32 k;

	for (k = 0; k < n; k++)
		RF_DBG(rf, DBG_RF_POWER, "===> %s  Before data[%d]=%d\n", __func__, k, data[k]);

	for (i = n - 1; i >= 0; i--) {
		sp = 1;
		for (j = 0; j < i; j++) {
			if (data[j] < data[j + 1]) {
				temp = data[j];
				data[j] = data[j + 1];
				data[j + 1] = temp;
				sp = 0;
			}
		}
		if (sp == 1)
			break;
	}

	for (k = 0; k < n; k++)
		RF_DBG(rf, DBG_RF_POWER, "<=== %s  After data[%d]=%d\n", __func__, k, data[k]);
}

bool halrf_set_power_by_rate_to_struct_8852c(struct rf_info *rf, enum phl_phy_idx phy)
{
	struct rtw_tpu_info *tpu = &rf->hal_com->band[phy].rtw_tpu_i;
	struct rtw_tpu_pwr_by_rate_info *rate = &tpu->rtw_tpu_pwr_by_rate_i;
	u32 i, j;

	RF_DBG(rf, DBG_RF_POWER, "======>%s\n", __func__);

	rate->pwr_by_rate_lgcy[0] = halrf_get_power_by_rate(rf, phy, RF_PATH_A, RTW_DATA_RATE_CCK1, 0, 0) / 2;
	rate->pwr_by_rate_lgcy[1] = halrf_get_power_by_rate(rf, phy, RF_PATH_A, RTW_DATA_RATE_CCK2, 0, 0) / 2;
	rate->pwr_by_rate_lgcy[2] = halrf_get_power_by_rate(rf, phy, RF_PATH_A, RTW_DATA_RATE_CCK5_5, 0, 0) / 2;
	rate->pwr_by_rate_lgcy[3] = halrf_get_power_by_rate(rf, phy, RF_PATH_A, RTW_DATA_RATE_CCK11, 0, 0) / 2;

	rate->pwr_by_rate_lgcy[4] = halrf_get_power_by_rate(rf, phy, RF_PATH_A, RTW_DATA_RATE_OFDM6, 0, 0) / 2;
	rate->pwr_by_rate_lgcy[5] = halrf_get_power_by_rate(rf, phy, RF_PATH_A, RTW_DATA_RATE_OFDM9, 0, 0) / 2;
	rate->pwr_by_rate_lgcy[6] = halrf_get_power_by_rate(rf, phy, RF_PATH_A, RTW_DATA_RATE_OFDM12, 0, 0) / 2;
	rate->pwr_by_rate_lgcy[7] = halrf_get_power_by_rate(rf, phy, RF_PATH_A, RTW_DATA_RATE_OFDM18, 0, 0) / 2;

	rate->pwr_by_rate_lgcy[8] = halrf_get_power_by_rate(rf, phy, RF_PATH_A, RTW_DATA_RATE_OFDM24, 0, 0) / 2;
	rate->pwr_by_rate_lgcy[9] = halrf_get_power_by_rate(rf, phy, RF_PATH_A, RTW_DATA_RATE_OFDM36, 0, 0) / 2;
	rate->pwr_by_rate_lgcy[10] = halrf_get_power_by_rate(rf, phy, RF_PATH_A, RTW_DATA_RATE_OFDM48, 0, 0) / 2;
	rate->pwr_by_rate_lgcy[11] = halrf_get_power_by_rate(rf, phy, RF_PATH_A, RTW_DATA_RATE_OFDM54, 0, 0) / 2;

	rate->pwr_by_rate[PW_LMT_PH_1T][0] = halrf_get_power_by_rate(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS0, 0, 0) / 2;
	rate->pwr_by_rate[PW_LMT_PH_1T][1] = halrf_get_power_by_rate(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS1, 0, 0) / 2;
	rate->pwr_by_rate[PW_LMT_PH_1T][2] = halrf_get_power_by_rate(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS2, 0, 0) / 2;
	rate->pwr_by_rate[PW_LMT_PH_1T][3] = halrf_get_power_by_rate(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS3, 0, 0) / 2;

	rate->pwr_by_rate[PW_LMT_PH_1T][4] = halrf_get_power_by_rate(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS4, 0, 0) / 2;
	rate->pwr_by_rate[PW_LMT_PH_1T][5] = halrf_get_power_by_rate(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS5, 0, 0) / 2;
	rate->pwr_by_rate[PW_LMT_PH_1T][6] = halrf_get_power_by_rate(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS6, 0, 0) / 2;
	rate->pwr_by_rate[PW_LMT_PH_1T][7] = halrf_get_power_by_rate(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, 0, 0) / 2;

	rate->pwr_by_rate[PW_LMT_PH_1T][8] = halrf_get_power_by_rate(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS8, 0, 0) / 2;
	rate->pwr_by_rate[PW_LMT_PH_1T][9] = halrf_get_power_by_rate(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS9, 0, 0) / 2;
	rate->pwr_by_rate[PW_LMT_PH_1T][10] = halrf_get_power_by_rate(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS10, 0, 0) / 2;
	rate->pwr_by_rate[PW_LMT_PH_1T][11] = halrf_get_power_by_rate(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS11, 0, 0) / 2;

	rate->pwr_by_rate[PW_LMT_PH_1T][12] = halrf_get_power_by_rate(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS0, 1, 0) / 2;
	rate->pwr_by_rate[PW_LMT_PH_1T][13] = halrf_get_power_by_rate(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS1, 1, 0) / 2;
	rate->pwr_by_rate[PW_LMT_PH_1T][14] = halrf_get_power_by_rate(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS3, 1, 0) / 2;
	rate->pwr_by_rate[PW_LMT_PH_1T][15] = halrf_get_power_by_rate(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS4, 1, 0) / 2;

	rate->pwr_by_rate[PW_LMT_PH_2T][0] = halrf_get_power_by_rate(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS2_MCS0, 0, 0) / 2;
	rate->pwr_by_rate[PW_LMT_PH_2T][1] = halrf_get_power_by_rate(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS2_MCS1, 0, 0) / 2;
	rate->pwr_by_rate[PW_LMT_PH_2T][2] = halrf_get_power_by_rate(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS2_MCS2, 0, 0) / 2;
	rate->pwr_by_rate[PW_LMT_PH_2T][3] = halrf_get_power_by_rate(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS2_MCS3, 0, 0) / 2;

	rate->pwr_by_rate[PW_LMT_PH_2T][4] = halrf_get_power_by_rate(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS2_MCS4, 0, 0) / 2;
	rate->pwr_by_rate[PW_LMT_PH_2T][5] = halrf_get_power_by_rate(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS2_MCS5, 0, 0) / 2;
	rate->pwr_by_rate[PW_LMT_PH_2T][6] = halrf_get_power_by_rate(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS2_MCS6, 0, 0) / 2;
	rate->pwr_by_rate[PW_LMT_PH_2T][7] = halrf_get_power_by_rate(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS2_MCS7, 0, 0) / 2;

	rate->pwr_by_rate[PW_LMT_PH_2T][8] = halrf_get_power_by_rate(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS2_MCS8, 0, 0) / 2;
	rate->pwr_by_rate[PW_LMT_PH_2T][9] = halrf_get_power_by_rate(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS2_MCS9, 0, 0) / 2;
	rate->pwr_by_rate[PW_LMT_PH_2T][10] = halrf_get_power_by_rate(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS2_MCS10, 0, 0) / 2;
	rate->pwr_by_rate[PW_LMT_PH_2T][11] = halrf_get_power_by_rate(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS2_MCS11, 0, 0) / 2;

	rate->pwr_by_rate[PW_LMT_PH_2T][12] = halrf_get_power_by_rate(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS2_MCS0, 1, 0) / 2;
	rate->pwr_by_rate[PW_LMT_PH_2T][13] = halrf_get_power_by_rate(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS2_MCS1, 1, 0) / 2;
	rate->pwr_by_rate[PW_LMT_PH_2T][14] = halrf_get_power_by_rate(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS2_MCS3, 1, 0) / 2;
	rate->pwr_by_rate[PW_LMT_PH_2T][15] = halrf_get_power_by_rate(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS2_MCS4, 1, 0) / 2;

	rate->pwr_by_rate[PW_LMT_PH_2T][12] = halrf_get_power_by_rate(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS2_MCS0, 1, 0) / 2;
	rate->pwr_by_rate[PW_LMT_PH_2T][13] = halrf_get_power_by_rate(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS2_MCS1, 1, 0) / 2;
	rate->pwr_by_rate[PW_LMT_PH_2T][14] = halrf_get_power_by_rate(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS2_MCS3, 1, 0) / 2;
	rate->pwr_by_rate[PW_LMT_PH_2T][15] = halrf_get_power_by_rate(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS2_MCS4, 1, 0) / 2;

	/*CCK, Legacy, HT, VHT, HE*/
	tpu->pwr_ofst_mode[0] = halrf_get_power_by_rate(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS11, 0, 1) / 2;
	tpu->pwr_ofst_mode[1] = halrf_get_power_by_rate(rf, phy, RF_PATH_A, RTW_DATA_RATE_VHT_NSS1_MCS9, 0, 1) / 2;
	tpu->pwr_ofst_mode[2] = halrf_get_power_by_rate(rf, phy, RF_PATH_A, RTW_DATA_RATE_MCS7, 0, 1) / 2;
	tpu->pwr_ofst_mode[3] = halrf_get_power_by_rate(rf, phy, RF_PATH_A, RTW_DATA_RATE_OFDM54, 0, 1) / 2;
	tpu->pwr_ofst_mode[4] = halrf_get_power_by_rate(rf, phy, RF_PATH_A, RTW_DATA_RATE_CCK11, 0, 1) / 2;

	for (i = 0; i < 12; i = i + 4)
		RF_DBG(rf, DBG_RF_POWER, "power_by_rate ==> %d   %d   %d   %d\n",
			rate->pwr_by_rate_lgcy[i + 3],
			rate->pwr_by_rate_lgcy[i + 2],
			rate->pwr_by_rate_lgcy[i + 1],
			rate->pwr_by_rate_lgcy[i]);

	for (j = 0; j < 2; j++)
		for (i = 0; i < 16; i = i + 4)
			RF_DBG(rf, DBG_RF_POWER, "power_by_rate ==> %d   %d   %d   %d\n",
				rate->pwr_by_rate[j][i + 3],
				rate->pwr_by_rate[j][i + 2],
				rate->pwr_by_rate[j][i + 1],
				rate->pwr_by_rate[j][i]);

	RF_DBG(rf, DBG_RF_POWER, "power_by_rate_offset ==> %d   %d   %d   %d   %d\n",
		tpu->pwr_ofst_mode[4],
		tpu->pwr_ofst_mode[3],
		tpu->pwr_ofst_mode[2],
		tpu->pwr_ofst_mode[1],
		tpu->pwr_ofst_mode[0]);
	return true;
}

void halrf_set_fix_power_to_struct_8852c(struct rf_info *rf, enum phl_phy_idx phy, s8 dbm)
{
	struct rtw_hal_com_t *hal = rf->hal_com;
	struct rtw_tpu_info *tpu = &rf->hal_com->band[phy].rtw_tpu_i;
	struct rtw_tpu_pwr_by_rate_info *rate = &tpu->rtw_tpu_pwr_by_rate_i;
	struct rtw_tpu_pwr_imt_info *lmt = &rf->hal_com->band[phy].rtw_tpu_i.rtw_tpu_pwr_imt_i;

	RF_DBG(rf, DBG_RF_POWER, "======>%s\n", __func__);

	hal_mem_set(hal, rate, dbm, sizeof(*rate));
	hal_mem_set(hal, lmt, dbm, sizeof(*lmt));
	hal_mem_set(hal, tpu->pwr_ofst_mode, 0, sizeof(tpu->pwr_ofst_mode));
	hal_mem_set(hal, tpu->pwr_lmt_ru, dbm, sizeof(tpu->pwr_lmt_ru));

	halrf_mac_write_pwr_by_rate_reg(rf, phy);
	halrf_mac_write_pwr_ofst_mode(rf, phy);
	halrf_mac_write_pwr_limit_reg(rf, phy);
	halrf_mac_write_pwr_limit_rua_reg(rf, phy);
}

void halrf_get_power_limit_to_struct_20m_8852c(struct rf_info *rf, enum phl_phy_idx phy)
{
	struct rtw_tpu_pwr_imt_info *lmt = &rf->hal_com->band[phy].rtw_tpu_i.rtw_tpu_pwr_imt_i;
	struct rtw_hal_com_t *hal = rf->hal_com;
	u8 ch = rf->hal_com->band[phy].cur_chandef.center_ch;

	RF_DBG(rf, DBG_RF_POWER, "======>%s channel=%d\n", __func__, ch);

	hal_mem_set(hal, lmt, 0, sizeof(*lmt));

	/*1Tx CCK 20m  NOBF*/
	lmt->pwr_lmt_cck_20m[PW_LMT_PH_1T][PW_LMT_NONBF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_CCK11, PW_LMT_BW_20M, PW_LMT_NONBF, PW_LMT_PH_1T, ch) / 2;

	/*1Tx CCK 20m BF*/
	lmt->pwr_lmt_cck_20m[PW_LMT_PH_1T][PW_LMT_BF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_CCK11, PW_LMT_BW_20M, PW_LMT_BF, PW_LMT_PH_1T, ch) / 2;

	/*1Tx CCK 40m  NOBF*/
	lmt->pwr_lmt_cck_40m[PW_LMT_PH_1T][PW_LMT_NONBF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_CCK11, PW_LMT_BW_40M, PW_LMT_NONBF, PW_LMT_PH_1T, ch) / 2;

	/*1Tx CCK 40m BF*/
	lmt->pwr_lmt_cck_40m[PW_LMT_PH_1T][PW_LMT_BF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_CCK11, PW_LMT_BW_40M, PW_LMT_BF, PW_LMT_PH_1T, ch) / 2;

	/*1Tx OFDM  NOBF*/
	lmt->pwr_lmt_lgcy_20m[PW_LMT_PH_1T][PW_LMT_NONBF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_OFDM54, PW_LMT_BW_20M, PW_LMT_NONBF, PW_LMT_PH_1T, ch) / 2;

	/*1Tx OFDM BF*/
	lmt->pwr_lmt_lgcy_20m[PW_LMT_PH_1T][PW_LMT_BF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_OFDM54, PW_LMT_BW_20M, PW_LMT_BF, PW_LMT_PH_1T, ch) / 2;

	/*1Tx 20m NOBF*/
	lmt->pwr_lmt_20m[PW_LMT_PH_1T][0][PW_LMT_NONBF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_20M, PW_LMT_NONBF, PW_LMT_PH_1T, ch) / 2;

	/*1Tx 20m BF*/
	lmt->pwr_lmt_20m[PW_LMT_PH_1T][0][PW_LMT_BF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_20M, PW_LMT_BF, PW_LMT_PH_1T, ch) / 2;

	/*2Tx CCK 20m NOBF*/
	lmt->pwr_lmt_cck_20m[PW_LMT_PH_2T][PW_LMT_NONBF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_CCK11, PW_LMT_BW_20M, PW_LMT_NONBF, PW_LMT_PH_2T, ch) / 2;

	/*2Tx CCK 20m BF*/
	lmt->pwr_lmt_cck_20m[PW_LMT_PH_2T][PW_LMT_BF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_CCK11, PW_LMT_BW_20M, PW_LMT_BF, PW_LMT_PH_2T, ch) / 2;

	/*2Tx CCK 40m NOBF*/
	lmt->pwr_lmt_cck_40m[PW_LMT_PH_2T][PW_LMT_NONBF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_CCK11, PW_LMT_BW_40M, PW_LMT_NONBF, PW_LMT_PH_2T, ch) / 2;

	/*2Tx CCK 40m BF*/
	lmt->pwr_lmt_cck_40m[PW_LMT_PH_2T][PW_LMT_BF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_CCK11, PW_LMT_BW_40M, PW_LMT_BF, PW_LMT_PH_2T, ch) / 2;

	/*2Tx OFDM NOBF*/
	lmt->pwr_lmt_lgcy_20m[PW_LMT_PH_2T][PW_LMT_NONBF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_OFDM54, PW_LMT_BW_20M, PW_LMT_NONBF, PW_LMT_PH_2T, ch) / 2;

	/*2Tx OFDM BF*/
	lmt->pwr_lmt_lgcy_20m[PW_LMT_PH_2T][PW_LMT_BF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_OFDM54, PW_LMT_BW_20M, PW_LMT_BF, PW_LMT_PH_2T, ch) / 2;

	/*2Tx 20m NOBF*/
	lmt->pwr_lmt_20m[PW_LMT_PH_2T][0][PW_LMT_NONBF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_20M, PW_LMT_NONBF, PW_LMT_PH_2T, ch) / 2;

	/*2Tx 20m BF*/
	lmt->pwr_lmt_20m[PW_LMT_PH_2T][0][PW_LMT_BF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_20M, PW_LMT_BF, PW_LMT_PH_2T, ch) / 2;
}

void halrf_get_power_limit_to_struct_40m_8852c(struct rf_info *rf, enum phl_phy_idx phy)
{
	struct rtw_tpu_pwr_imt_info *lmt = &rf->hal_com->band[phy].rtw_tpu_i.rtw_tpu_pwr_imt_i;
	struct rtw_hal_com_t *hal = rf->hal_com;
	u8 ch = rf->hal_com->band[phy].cur_chandef.center_ch;
	u8 txsc_ch = rf->hal_com->band[phy].cur_chandef.chan;

	RF_DBG(rf, DBG_RF_POWER, "======>%s channel=%d   txsc_ch=0x%x\n", __func__, ch, txsc_ch);

	hal_mem_set(hal, lmt, 0, sizeof(*lmt));

	/*1Tx CCK 20m NONBF*/
	lmt->pwr_lmt_cck_20m[PW_LMT_PH_1T][PW_LMT_NONBF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_CCK11, PW_LMT_BW_20M, PW_LMT_NONBF, PW_LMT_PH_1T, ch - 2) / 2;

	/*1Tx CCK 20m BF*/
	lmt->pwr_lmt_cck_20m[PW_LMT_PH_1T][PW_LMT_BF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_CCK11, PW_LMT_BW_20M, PW_LMT_BF, PW_LMT_PH_1T, ch - 2) / 2;

	/*1Tx CCK 40m NONBF*/
	lmt->pwr_lmt_cck_40m[PW_LMT_PH_1T][PW_LMT_NONBF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_CCK11, PW_LMT_BW_40M, PW_LMT_NONBF, PW_LMT_PH_1T, ch) / 2;

	/*1Tx CCK 40m BF*/
	lmt->pwr_lmt_cck_40m[PW_LMT_PH_1T][PW_LMT_BF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_CCK11, PW_LMT_BW_40M, PW_LMT_BF, PW_LMT_PH_1T, ch) / 2;

	/*1Tx OFDM NONBF*/
	lmt->pwr_lmt_lgcy_20m[PW_LMT_PH_1T][PW_LMT_NONBF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_OFDM54, PW_LMT_BW_20M, PW_LMT_NONBF, PW_LMT_PH_1T, txsc_ch) / 2;

	/*1Tx OFDM BF*/
	lmt->pwr_lmt_lgcy_20m[PW_LMT_PH_1T][PW_LMT_BF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_OFDM54, PW_LMT_BW_20M, PW_LMT_BF, PW_LMT_PH_1T, txsc_ch) / 2;

	/*1Tx 20m NOBF*/
	lmt->pwr_lmt_20m[PW_LMT_PH_1T][0][PW_LMT_NONBF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_20M, PW_LMT_NONBF, PW_LMT_PH_1T, ch - 2) / 2;

	lmt->pwr_lmt_20m[PW_LMT_PH_1T][1][PW_LMT_NONBF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_20M, PW_LMT_NONBF, PW_LMT_PH_1T, ch + 2) / 2;

	/*1Tx 20m BF*/
	lmt->pwr_lmt_20m[PW_LMT_PH_1T][0][PW_LMT_BF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_20M, PW_LMT_BF, PW_LMT_PH_1T, ch - 2) / 2;

	lmt->pwr_lmt_20m[PW_LMT_PH_1T][1][PW_LMT_BF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_20M, PW_LMT_BF, PW_LMT_PH_1T, ch + 2) / 2;

	/*1Tx 40m NOBF*/
	lmt->pwr_lmt_40m[PW_LMT_PH_1T][0][PW_LMT_NONBF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_40M, PW_LMT_NONBF, PW_LMT_PH_1T, ch) / 2;

	/*1Tx 40m BF*/
	lmt->pwr_lmt_40m[PW_LMT_PH_1T][0][PW_LMT_BF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_40M, PW_LMT_BF, PW_LMT_PH_1T, ch) / 2;

	/*2Tx CCK 20m NONBF*/
	lmt->pwr_lmt_cck_20m[PW_LMT_PH_2T][PW_LMT_NONBF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_CCK11, PW_LMT_BW_20M, PW_LMT_NONBF, PW_LMT_PH_2T, ch - 2) / 2;

	/*2Tx CCK 20m BF*/
	lmt->pwr_lmt_cck_20m[PW_LMT_PH_2T][PW_LMT_BF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_CCK11, PW_LMT_BW_20M, PW_LMT_BF, PW_LMT_PH_2T, ch - 2) / 2;

	/*2Tx CCK 40m NONBF*/
	lmt->pwr_lmt_cck_40m[PW_LMT_PH_2T][PW_LMT_NONBF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_CCK11, PW_LMT_BW_40M, PW_LMT_NONBF, PW_LMT_PH_2T, ch) / 2;

	/*2Tx CCK 40m BF*/
	lmt->pwr_lmt_cck_40m[PW_LMT_PH_2T][PW_LMT_BF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_CCK11, PW_LMT_BW_40M, PW_LMT_BF, PW_LMT_PH_2T, ch) / 2;

	/*2Tx OFDM NONBF*/
	lmt->pwr_lmt_lgcy_20m[PW_LMT_PH_2T][PW_LMT_NONBF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_OFDM54, PW_LMT_BW_20M, PW_LMT_NONBF, PW_LMT_PH_2T, txsc_ch) / 2;

	/*2Tx OFDM BF*/
	lmt->pwr_lmt_lgcy_20m[PW_LMT_PH_2T][PW_LMT_BF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_OFDM54, PW_LMT_BW_20M, PW_LMT_BF, PW_LMT_PH_2T, txsc_ch) / 2;

	/*2Tx 20m NOBF*/
	lmt->pwr_lmt_20m[PW_LMT_PH_2T][0][PW_LMT_NONBF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_20M, PW_LMT_NONBF, PW_LMT_PH_2T, ch - 2) / 2;

	lmt->pwr_lmt_20m[PW_LMT_PH_2T][1][PW_LMT_NONBF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_20M, PW_LMT_NONBF, PW_LMT_PH_2T, ch + 2) / 2;

	/*2Tx 20m BF*/
	lmt->pwr_lmt_20m[PW_LMT_PH_2T][0][PW_LMT_BF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_20M, PW_LMT_BF, PW_LMT_PH_2T, ch - 2) / 2;

	lmt->pwr_lmt_20m[PW_LMT_PH_2T][1][PW_LMT_BF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_20M, PW_LMT_BF, PW_LMT_PH_2T, ch + 2) / 2;

	/*2Tx 40m NOBF*/
	lmt->pwr_lmt_40m[PW_LMT_PH_2T][0][PW_LMT_NONBF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_40M, PW_LMT_NONBF, PW_LMT_PH_2T, ch) / 2;

	/*2Tx 40m BF*/
	lmt->pwr_lmt_40m[PW_LMT_PH_2T][0][PW_LMT_BF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_40M, PW_LMT_BF, PW_LMT_PH_2T, ch) / 2;
}

void halrf_get_power_limit_to_struct_80m_8852c(struct rf_info *rf, enum phl_phy_idx phy)
{
	struct rtw_tpu_pwr_imt_info *lmt = &rf->hal_com->band[phy].rtw_tpu_i.rtw_tpu_pwr_imt_i;
	struct rtw_hal_com_t *hal = rf->hal_com;
	u8 ch = rf->hal_com->band[phy].cur_chandef.center_ch;
	u8 txsc_ch = rf->hal_com->band[phy].cur_chandef.chan;
	s8 tmp, tmp1, tmp2;

	RF_DBG(rf, DBG_RF_POWER, "======>%s channel=%d   txsc_ch=%d\n", __func__, ch, txsc_ch);

	hal_mem_set(hal, lmt, 0, sizeof(*lmt));

	/*1Tx OFDM NONBF*/
	lmt->pwr_lmt_lgcy_20m[PW_LMT_PH_1T][PW_LMT_NONBF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_OFDM54, PW_LMT_BW_20M, PW_LMT_NONBF, PW_LMT_PH_1T, txsc_ch) / 2;

	/*1Tx OFDM BF*/
	lmt->pwr_lmt_lgcy_20m[PW_LMT_PH_1T][PW_LMT_BF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_OFDM54, PW_LMT_BW_20M, PW_LMT_BF, PW_LMT_PH_1T, txsc_ch) / 2;

	/*1Tx 20m NOBF*/
	lmt->pwr_lmt_20m[PW_LMT_PH_1T][0][PW_LMT_NONBF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_20M, PW_LMT_NONBF, PW_LMT_PH_1T, ch - 6) / 2;

	lmt->pwr_lmt_20m[PW_LMT_PH_1T][1][PW_LMT_NONBF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_20M, PW_LMT_NONBF, PW_LMT_PH_1T, ch - 2) / 2;

	lmt->pwr_lmt_20m[PW_LMT_PH_1T][2][PW_LMT_NONBF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_20M, PW_LMT_NONBF, PW_LMT_PH_1T, ch + 2) / 2;

	lmt->pwr_lmt_20m[PW_LMT_PH_1T][3][PW_LMT_NONBF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_20M, PW_LMT_NONBF, PW_LMT_PH_1T, ch + 6) / 2;

	/*1Tx 20m BF*/
	lmt->pwr_lmt_20m[PW_LMT_PH_1T][0][PW_LMT_BF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_20M, PW_LMT_BF, PW_LMT_PH_1T, ch - 6) / 2;

	lmt->pwr_lmt_20m[PW_LMT_PH_1T][1][PW_LMT_BF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_20M, PW_LMT_BF, PW_LMT_PH_1T, ch - 2) / 2;

	lmt->pwr_lmt_20m[PW_LMT_PH_1T][2][PW_LMT_BF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_20M, PW_LMT_BF, PW_LMT_PH_1T, ch + 2) / 2;

	lmt->pwr_lmt_20m[PW_LMT_PH_1T][3][PW_LMT_BF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_20M, PW_LMT_BF, PW_LMT_PH_1T, ch + 6) / 2;

	/*1Tx 40m NOBF*/
	lmt->pwr_lmt_40m[PW_LMT_PH_1T][0][PW_LMT_NONBF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_40M, PW_LMT_NONBF, PW_LMT_PH_1T, ch - 4) / 2;

	lmt->pwr_lmt_40m[PW_LMT_PH_1T][1][PW_LMT_NONBF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_40M, PW_LMT_NONBF, PW_LMT_PH_1T, ch + 4) / 2;

	/*1Tx 40m BF*/
	lmt->pwr_lmt_40m[PW_LMT_PH_1T][0][PW_LMT_BF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_40M, PW_LMT_BF, PW_LMT_PH_1T, ch - 4) / 2;

	lmt->pwr_lmt_40m[PW_LMT_PH_1T][1][PW_LMT_BF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_40M, PW_LMT_BF, PW_LMT_PH_1T, ch + 4) / 2;

	/*1Tx 80m NOBF*/
	lmt->pwr_lmt_80m[PW_LMT_PH_1T][0][PW_LMT_NONBF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_80M, PW_LMT_NONBF, PW_LMT_PH_1T, ch) / 2;

	/*1Tx 80m BF*/
	lmt->pwr_lmt_80m[PW_LMT_PH_1T][0][PW_LMT_BF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_80M, PW_LMT_BF, PW_LMT_PH_1T, ch) / 2;

	/*1Tx 40m 0p5 NOBF*/
	tmp1 = halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_40M, PW_LMT_NONBF, PW_LMT_PH_1T, ch - 4) / 2;
	tmp2 = halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_40M, PW_LMT_NONBF, PW_LMT_PH_1T, ch + 4) / 2;
	if (tmp1 >= tmp2)
		tmp = tmp2;
	else
		tmp = tmp1;

	lmt->pwr_lmt_40m_0p5[PW_LMT_PH_1T][PW_LMT_NONBF] = tmp;

	/*1Tx 40m 0p5 BF*/
	tmp1 = halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_40M, PW_LMT_BF, PW_LMT_PH_1T, ch - 4) / 2;
	tmp2 = halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_40M, PW_LMT_BF, PW_LMT_PH_1T, ch + 4) / 2;
	if (tmp1 >= tmp2)
		tmp = tmp2;
	else
		tmp = tmp1;

	lmt->pwr_lmt_40m_0p5[PW_LMT_PH_1T][PW_LMT_BF] = tmp;

	/*2Tx OFDM NONBF*/
	lmt->pwr_lmt_lgcy_20m[PW_LMT_PH_2T][PW_LMT_NONBF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_OFDM54, PW_LMT_BW_20M, PW_LMT_NONBF, PW_LMT_PH_2T, txsc_ch) / 2;

	/*2Tx OFDM BF*/
	lmt->pwr_lmt_lgcy_20m[PW_LMT_PH_2T][PW_LMT_BF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_OFDM54, PW_LMT_BW_20M, PW_LMT_BF, PW_LMT_PH_2T, txsc_ch) / 2;

	/*2Tx 20m NOBF*/
	lmt->pwr_lmt_20m[PW_LMT_PH_2T][0][PW_LMT_NONBF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_20M, PW_LMT_NONBF, PW_LMT_PH_2T, ch - 6) / 2;

	lmt->pwr_lmt_20m[PW_LMT_PH_2T][1][PW_LMT_NONBF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_20M, PW_LMT_NONBF, PW_LMT_PH_2T, ch - 2) / 2;

	lmt->pwr_lmt_20m[PW_LMT_PH_2T][2][PW_LMT_NONBF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_20M, PW_LMT_NONBF, PW_LMT_PH_2T, ch + 2) / 2;

	lmt->pwr_lmt_20m[PW_LMT_PH_2T][3][PW_LMT_NONBF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_20M, PW_LMT_NONBF, PW_LMT_PH_2T, ch + 6) / 2;

	/*2Tx 20m BF*/
	lmt->pwr_lmt_20m[PW_LMT_PH_2T][0][PW_LMT_BF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_20M, PW_LMT_BF, PW_LMT_PH_2T, ch - 6) / 2;

	lmt->pwr_lmt_20m[PW_LMT_PH_2T][1][PW_LMT_BF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_20M, PW_LMT_BF, PW_LMT_PH_2T, ch - 2) / 2;

	lmt->pwr_lmt_20m[PW_LMT_PH_2T][2][PW_LMT_BF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_20M, PW_LMT_BF, PW_LMT_PH_2T, ch + 2) / 2;

	lmt->pwr_lmt_20m[PW_LMT_PH_2T][3][PW_LMT_BF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_20M, PW_LMT_BF, PW_LMT_PH_2T, ch + 6) / 2;

	/*2Tx 40m NOBF*/
	lmt->pwr_lmt_40m[PW_LMT_PH_2T][0][PW_LMT_NONBF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_40M, PW_LMT_NONBF, PW_LMT_PH_2T, ch - 4) / 2;

	lmt->pwr_lmt_40m[PW_LMT_PH_2T][1][PW_LMT_NONBF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_40M, PW_LMT_NONBF, PW_LMT_PH_2T, ch + 4) / 2;

	/*2Tx 40m BF*/
	lmt->pwr_lmt_40m[PW_LMT_PH_2T][0][PW_LMT_BF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_40M, PW_LMT_BF, PW_LMT_PH_2T, ch - 4) / 2;

	lmt->pwr_lmt_40m[PW_LMT_PH_2T][1][PW_LMT_BF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_40M, PW_LMT_BF, PW_LMT_PH_2T, ch + 4) / 2;

	/*2Tx 80m NOBF*/
	lmt->pwr_lmt_80m[PW_LMT_PH_2T][0][PW_LMT_NONBF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_80M, PW_LMT_NONBF, PW_LMT_PH_2T, ch) / 2;

	/*2Tx 80m BF*/
	lmt->pwr_lmt_80m[PW_LMT_PH_2T][0][PW_LMT_BF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_80M, PW_LMT_BF, PW_LMT_PH_2T, ch) / 2;

	/*2Tx 40m 0p5 NOBF*/
	tmp1 = halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_40M, PW_LMT_NONBF, PW_LMT_PH_2T, ch - 4) / 2;
	tmp2 = halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_40M, PW_LMT_NONBF, PW_LMT_PH_2T, ch + 4) / 2;
	if (tmp1 >= tmp2)
		tmp = tmp2;
	else
		tmp = tmp1;

	lmt->pwr_lmt_40m_0p5[PW_LMT_PH_2T][PW_LMT_NONBF] = tmp;

	/*2Tx 40m 0p5 BF*/
	tmp1 = halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_40M, PW_LMT_BF, PW_LMT_PH_2T, ch - 4) / 2;
	tmp2 = halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_40M, PW_LMT_BF, PW_LMT_PH_2T, ch + 4) / 2;
	if (tmp1 >= tmp2)
		tmp = tmp2;
	else
		tmp = tmp1;

	lmt->pwr_lmt_40m_0p5[PW_LMT_PH_2T][PW_LMT_BF] = tmp;
}

void halrf_get_power_limit_to_struct_160m_8852c(struct rf_info *rf, enum phl_phy_idx phy)
{
	struct rtw_tpu_pwr_imt_info *lmt = &rf->hal_com->band[phy].rtw_tpu_i.rtw_tpu_pwr_imt_i;
	struct rtw_hal_com_t *hal = rf->hal_com;
	u8 ch = rf->hal_com->band[phy].cur_chandef.center_ch;
	u8 txsc_ch = rf->hal_com->band[phy].cur_chandef.chan;
	s8 tmp, tmp1, tmp2;

	RF_DBG(rf, DBG_RF_POWER, "======>%s channel=%d\n", __func__, ch);

	hal_mem_set(hal, lmt, 0, sizeof(*lmt));

	/*1Tx OFDM NONBF*/
	lmt->pwr_lmt_lgcy_20m[PW_LMT_PH_1T][PW_LMT_NONBF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_OFDM54, PW_LMT_BW_20M, PW_LMT_NONBF, PW_LMT_PH_1T, txsc_ch) / 2;

	/*1Tx OFDM BF*/
	lmt->pwr_lmt_lgcy_20m[PW_LMT_PH_1T][PW_LMT_BF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_OFDM54, PW_LMT_BW_20M, PW_LMT_BF, PW_LMT_PH_1T, txsc_ch) / 2;

	/*1Tx 20m NOBF*/
	lmt->pwr_lmt_20m[PW_LMT_PH_1T][0][PW_LMT_NONBF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_20M, PW_LMT_NONBF, PW_LMT_PH_1T, ch - 14) / 2;

	lmt->pwr_lmt_20m[PW_LMT_PH_1T][1][PW_LMT_NONBF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_20M, PW_LMT_NONBF, PW_LMT_PH_1T, ch - 10) / 2;

	lmt->pwr_lmt_20m[PW_LMT_PH_1T][2][PW_LMT_NONBF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_20M, PW_LMT_NONBF, PW_LMT_PH_1T, ch - 6) / 2;

	lmt->pwr_lmt_20m[PW_LMT_PH_1T][3][PW_LMT_NONBF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_20M, PW_LMT_NONBF, PW_LMT_PH_1T, ch - 2) / 2;
	
	lmt->pwr_lmt_20m[PW_LMT_PH_1T][4][PW_LMT_NONBF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_20M, PW_LMT_NONBF, PW_LMT_PH_1T, ch + 2) / 2;

	lmt->pwr_lmt_20m[PW_LMT_PH_1T][5][PW_LMT_NONBF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_20M, PW_LMT_NONBF, PW_LMT_PH_1T, ch + 6) / 2;

	lmt->pwr_lmt_20m[PW_LMT_PH_1T][6][PW_LMT_NONBF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_20M, PW_LMT_NONBF, PW_LMT_PH_1T, ch + 10) / 2;

	lmt->pwr_lmt_20m[PW_LMT_PH_1T][7][PW_LMT_NONBF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_20M, PW_LMT_NONBF, PW_LMT_PH_1T, ch + 14) / 2;

	/*1Tx 20m BF*/
	lmt->pwr_lmt_20m[PW_LMT_PH_1T][0][PW_LMT_BF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_20M, PW_LMT_BF, PW_LMT_PH_1T, ch - 14) / 2;

	lmt->pwr_lmt_20m[PW_LMT_PH_1T][1][PW_LMT_BF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_20M, PW_LMT_BF, PW_LMT_PH_1T, ch - 10) / 2;

	lmt->pwr_lmt_20m[PW_LMT_PH_1T][2][PW_LMT_BF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_20M, PW_LMT_BF, PW_LMT_PH_1T, ch - 6) / 2;

	lmt->pwr_lmt_20m[PW_LMT_PH_1T][3][PW_LMT_BF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_20M, PW_LMT_BF, PW_LMT_PH_1T, ch - 2) / 2;

	lmt->pwr_lmt_20m[PW_LMT_PH_1T][4][PW_LMT_BF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_20M, PW_LMT_BF, PW_LMT_PH_1T, ch + 2) / 2;

	lmt->pwr_lmt_20m[PW_LMT_PH_1T][5][PW_LMT_BF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_20M, PW_LMT_BF, PW_LMT_PH_1T, ch + 6) / 2;

	lmt->pwr_lmt_20m[PW_LMT_PH_1T][6][PW_LMT_BF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_20M, PW_LMT_BF, PW_LMT_PH_1T, ch + 10) / 2;

	lmt->pwr_lmt_20m[PW_LMT_PH_1T][7][PW_LMT_BF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_20M, PW_LMT_BF, PW_LMT_PH_1T, ch + 14) / 2;

	/*1Tx 40m NOBF*/
	lmt->pwr_lmt_40m[PW_LMT_PH_1T][0][PW_LMT_NONBF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_40M, PW_LMT_NONBF, PW_LMT_PH_1T, ch - 12) / 2;

	lmt->pwr_lmt_40m[PW_LMT_PH_1T][1][PW_LMT_NONBF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_40M, PW_LMT_NONBF, PW_LMT_PH_1T, ch - 4) / 2;

	lmt->pwr_lmt_40m[PW_LMT_PH_1T][2][PW_LMT_NONBF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_40M, PW_LMT_NONBF, PW_LMT_PH_1T, ch + 4) / 2;

	lmt->pwr_lmt_40m[PW_LMT_PH_1T][3][PW_LMT_NONBF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_40M, PW_LMT_NONBF, PW_LMT_PH_1T, ch + 12) / 2;

	/*1Tx 40m BF*/
	lmt->pwr_lmt_40m[PW_LMT_PH_1T][0][PW_LMT_BF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_40M, PW_LMT_BF, PW_LMT_PH_1T, ch - 12) / 2;

	lmt->pwr_lmt_40m[PW_LMT_PH_1T][1][PW_LMT_BF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_40M, PW_LMT_BF, PW_LMT_PH_1T, ch - 4) / 2;

	lmt->pwr_lmt_40m[PW_LMT_PH_1T][2][PW_LMT_BF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_40M, PW_LMT_BF, PW_LMT_PH_1T, ch + 4) / 2;

	lmt->pwr_lmt_40m[PW_LMT_PH_1T][3][PW_LMT_BF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_40M, PW_LMT_BF, PW_LMT_PH_1T, ch + 12) / 2;

	/*1Tx 80m NOBF*/
	lmt->pwr_lmt_80m[PW_LMT_PH_1T][0][PW_LMT_NONBF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_80M, PW_LMT_NONBF, PW_LMT_PH_1T, ch - 8) / 2;

	lmt->pwr_lmt_80m[PW_LMT_PH_1T][1][PW_LMT_NONBF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_80M, PW_LMT_NONBF, PW_LMT_PH_1T, ch + 8) / 2;

	/*1Tx 80m BF*/
	lmt->pwr_lmt_80m[PW_LMT_PH_1T][0][PW_LMT_BF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_80M, PW_LMT_BF, PW_LMT_PH_1T, ch - 8) / 2;

	lmt->pwr_lmt_80m[PW_LMT_PH_1T][1][PW_LMT_BF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_80M, PW_LMT_BF, PW_LMT_PH_1T, ch + 8) / 2;

	/*1Tx 160m NOBF*/
	lmt->pwr_lmt_160m[PW_LMT_PH_1T][PW_LMT_NONBF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_160M, PW_LMT_NONBF, PW_LMT_PH_1T, ch) / 2;

	/*1Tx 160m BF*/
	lmt->pwr_lmt_160m[PW_LMT_PH_1T][PW_LMT_BF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_160M, PW_LMT_BF, PW_LMT_PH_1T, ch) / 2;

	/*1Tx 40m 0p5 NOBF*/
	tmp1 = halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_40M, PW_LMT_NONBF, PW_LMT_PH_1T, ch - 4) / 2;
	tmp2 = halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_40M, PW_LMT_NONBF, PW_LMT_PH_1T, ch + 4) / 2;
	if (tmp1 >= tmp2)
		tmp = tmp2;
	else
		tmp = tmp1;

	lmt->pwr_lmt_40m_0p5[PW_LMT_PH_1T][PW_LMT_NONBF] = tmp;

	/*1Tx 40m 0p5 BF*/
	tmp1 = halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_40M, PW_LMT_BF, PW_LMT_PH_1T, ch - 4) / 2;
	tmp2 = halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_40M, PW_LMT_BF, PW_LMT_PH_1T, ch + 4) / 2;
	if (tmp1 >= tmp2)
		tmp = tmp2;
	else
		tmp = tmp1;

	lmt->pwr_lmt_40m_0p5[PW_LMT_PH_1T][PW_LMT_BF] = tmp;

	/*1Tx 40m 2p5 NOBF*/
	tmp1 = halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_40M, PW_LMT_NONBF, PW_LMT_PH_1T, ch - 8) / 2;
	tmp2 = halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_40M, PW_LMT_NONBF, PW_LMT_PH_1T, ch + 8) / 2;
	if (tmp1 >= tmp2)
		tmp = tmp2;
	else
		tmp = tmp1;

	lmt->pwr_lmt_40m_2p5[PW_LMT_PH_1T][PW_LMT_NONBF] = tmp;

	/*1Tx 40m 2p5 BF*/
	tmp1 = halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_40M, PW_LMT_BF, PW_LMT_PH_1T, ch - 8) / 2;
	tmp2 = halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_40M, PW_LMT_BF, PW_LMT_PH_1T, ch + 8) / 2;
	if (tmp1 >= tmp2)
		tmp = tmp2;
	else
		tmp = tmp1;

	lmt->pwr_lmt_40m_2p5[PW_LMT_PH_1T][PW_LMT_BF] = tmp;

	/*2Tx OFDM NONBF*/
	lmt->pwr_lmt_lgcy_20m[PW_LMT_PH_2T][PW_LMT_NONBF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_OFDM54, PW_LMT_BW_20M, PW_LMT_NONBF, PW_LMT_PH_2T, txsc_ch) / 2;

	/*2Tx OFDM BF*/
	lmt->pwr_lmt_lgcy_20m[PW_LMT_PH_2T][PW_LMT_BF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_OFDM54, PW_LMT_BW_20M, PW_LMT_BF, PW_LMT_PH_2T, txsc_ch) / 2;

	/*2Tx 20m NOBF*/
	lmt->pwr_lmt_20m[PW_LMT_PH_2T][0][PW_LMT_NONBF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_20M, PW_LMT_NONBF, PW_LMT_PH_2T, ch - 14) / 2;

	lmt->pwr_lmt_20m[PW_LMT_PH_2T][1][PW_LMT_NONBF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_20M, PW_LMT_NONBF, PW_LMT_PH_2T, ch - 10) / 2;

	lmt->pwr_lmt_20m[PW_LMT_PH_2T][2][PW_LMT_NONBF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_20M, PW_LMT_NONBF, PW_LMT_PH_2T, ch - 6) / 2;

	lmt->pwr_lmt_20m[PW_LMT_PH_2T][3][PW_LMT_NONBF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_20M, PW_LMT_NONBF, PW_LMT_PH_2T, ch - 2) / 2;

	lmt->pwr_lmt_20m[PW_LMT_PH_2T][4][PW_LMT_NONBF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_20M, PW_LMT_NONBF, PW_LMT_PH_2T, ch + 2) / 2;

	lmt->pwr_lmt_20m[PW_LMT_PH_2T][5][PW_LMT_NONBF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_20M, PW_LMT_NONBF, PW_LMT_PH_2T, ch + 6) / 2;

	lmt->pwr_lmt_20m[PW_LMT_PH_2T][6][PW_LMT_NONBF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_20M, PW_LMT_NONBF, PW_LMT_PH_2T, ch + 10) / 2;

	lmt->pwr_lmt_20m[PW_LMT_PH_2T][7][PW_LMT_NONBF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_20M, PW_LMT_NONBF, PW_LMT_PH_2T, ch + 14) / 2;

	/*2Tx 20m BF*/
	lmt->pwr_lmt_20m[PW_LMT_PH_2T][0][PW_LMT_BF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_20M, PW_LMT_BF, PW_LMT_PH_2T, ch - 14) / 2;

	lmt->pwr_lmt_20m[PW_LMT_PH_2T][1][PW_LMT_BF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_20M, PW_LMT_BF, PW_LMT_PH_2T, ch - 10) / 2;

	lmt->pwr_lmt_20m[PW_LMT_PH_2T][2][PW_LMT_BF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_20M, PW_LMT_BF, PW_LMT_PH_2T, ch - 6) / 2;

	lmt->pwr_lmt_20m[PW_LMT_PH_2T][3][PW_LMT_BF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_20M, PW_LMT_BF, PW_LMT_PH_2T, ch - 2) / 2;

	lmt->pwr_lmt_20m[PW_LMT_PH_2T][4][PW_LMT_BF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_20M, PW_LMT_BF, PW_LMT_PH_2T, ch + 2) / 2;

	lmt->pwr_lmt_20m[PW_LMT_PH_2T][5][PW_LMT_BF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_20M, PW_LMT_BF, PW_LMT_PH_2T, ch + 6) / 2;

	lmt->pwr_lmt_20m[PW_LMT_PH_2T][6][PW_LMT_BF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_20M, PW_LMT_BF, PW_LMT_PH_2T, ch + 10) / 2;

	lmt->pwr_lmt_20m[PW_LMT_PH_2T][7][PW_LMT_BF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_20M, PW_LMT_BF, PW_LMT_PH_2T, ch + 14) / 2;

	/*2Tx 40m NOBF*/
	lmt->pwr_lmt_40m[PW_LMT_PH_2T][0][PW_LMT_NONBF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_40M, PW_LMT_NONBF, PW_LMT_PH_2T, ch - 12) / 2;

	lmt->pwr_lmt_40m[PW_LMT_PH_2T][1][PW_LMT_NONBF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_40M, PW_LMT_NONBF, PW_LMT_PH_2T, ch - 4) / 2;

	lmt->pwr_lmt_40m[PW_LMT_PH_2T][2][PW_LMT_NONBF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_40M, PW_LMT_NONBF, PW_LMT_PH_2T, ch + 4) / 2;

	lmt->pwr_lmt_40m[PW_LMT_PH_2T][3][PW_LMT_NONBF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_40M, PW_LMT_NONBF, PW_LMT_PH_2T, ch + 12) / 2;

	/*2Tx 40m BF*/
	lmt->pwr_lmt_40m[PW_LMT_PH_2T][0][PW_LMT_BF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_40M, PW_LMT_BF, PW_LMT_PH_2T, ch - 12) / 2;

	lmt->pwr_lmt_40m[PW_LMT_PH_2T][1][PW_LMT_BF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_40M, PW_LMT_BF, PW_LMT_PH_2T, ch - 4) / 2;

	lmt->pwr_lmt_40m[PW_LMT_PH_2T][2][PW_LMT_BF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_40M, PW_LMT_BF, PW_LMT_PH_2T, ch + 4) / 2;

	lmt->pwr_lmt_40m[PW_LMT_PH_2T][3][PW_LMT_BF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_40M, PW_LMT_BF, PW_LMT_PH_2T, ch + 12) / 2;

	/*2Tx 80m NOBF*/
	lmt->pwr_lmt_80m[PW_LMT_PH_2T][0][PW_LMT_NONBF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_80M, PW_LMT_NONBF, PW_LMT_PH_2T, ch - 8) / 2;

	lmt->pwr_lmt_80m[PW_LMT_PH_2T][1][PW_LMT_NONBF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_80M, PW_LMT_NONBF, PW_LMT_PH_2T, ch + 8) / 2;

	/*2Tx 80m BF*/
	lmt->pwr_lmt_80m[PW_LMT_PH_2T][0][PW_LMT_BF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_80M, PW_LMT_BF, PW_LMT_PH_2T, ch - 8) / 2;

	lmt->pwr_lmt_80m[PW_LMT_PH_2T][1][PW_LMT_BF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_80M, PW_LMT_BF, PW_LMT_PH_2T, ch + 8) / 2;

	/*2Tx 160m NOBF*/
	lmt->pwr_lmt_160m[PW_LMT_PH_2T][PW_LMT_NONBF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_160M, PW_LMT_NONBF, PW_LMT_PH_2T, ch) / 2;

	/*2Tx 160m BF*/
	lmt->pwr_lmt_160m[PW_LMT_PH_2T][PW_LMT_BF] =
		halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_160M, PW_LMT_BF, PW_LMT_PH_2T, ch) / 2;

	/*2Tx 40m 0p5 NOBF*/
	tmp1 = halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_40M, PW_LMT_NONBF, PW_LMT_PH_2T, ch - 4) / 2;
	tmp2 = halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_40M, PW_LMT_NONBF, PW_LMT_PH_2T, ch + 4) / 2;
	if (tmp1 >= tmp2)
		tmp = tmp2;
	else
		tmp = tmp1;

	lmt->pwr_lmt_40m_0p5[PW_LMT_PH_2T][PW_LMT_NONBF] = tmp;

	/*2Tx 40m 0p5 BF*/
	tmp1 = halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_40M, PW_LMT_BF, PW_LMT_PH_2T, ch - 4) / 2;
	tmp2 = halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_40M, PW_LMT_BF, PW_LMT_PH_2T, ch + 4) / 2;
	if (tmp1 >= tmp2)
		tmp = tmp2;
	else
		tmp = tmp1;

	lmt->pwr_lmt_40m_0p5[PW_LMT_PH_2T][PW_LMT_BF] = tmp;

	/*2Tx 40m 2p5 NOBF*/
	tmp1 = halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_40M, PW_LMT_NONBF, PW_LMT_PH_2T, ch - 8) / 2;
	tmp2 = halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_40M, PW_LMT_NONBF, PW_LMT_PH_2T, ch + 8) / 2;
	if (tmp1 >= tmp2)
		tmp = tmp2;
	else
		tmp = tmp1;

	lmt->pwr_lmt_40m_2p5[PW_LMT_PH_2T][PW_LMT_NONBF] = tmp;

	/*2Tx 40m 2p5 BF*/
	tmp1 = halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_40M, PW_LMT_BF, PW_LMT_PH_2T, ch - 8) / 2;
	tmp2 = halrf_get_power_limit(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_BW_40M, PW_LMT_BF, PW_LMT_PH_2T, ch + 8) / 2;
	if (tmp1 >= tmp2)
		tmp = tmp2;
	else
		tmp = tmp1;

	lmt->pwr_lmt_40m_2p5[PW_LMT_PH_2T][PW_LMT_BF] = tmp;
}

bool halrf_set_power_limit_to_struct_8852c(struct rf_info *rf, enum phl_phy_idx phy)
{
	struct rtw_tpu_pwr_imt_info *lmt = &rf->hal_com->band[phy].rtw_tpu_i.rtw_tpu_pwr_imt_i;
	u8 bw = rf->hal_com->band[phy].cur_chandef.bw;
	u32 i;
	s8 tmp;
	RF_DBG(rf, DBG_RF_POWER, "======>%s   bandwidth=%d\n",
		__func__, bw);

	if (bw == CHANNEL_WIDTH_20)
		halrf_get_power_limit_to_struct_20m_8852c(rf, phy);
	else if (bw == CHANNEL_WIDTH_40)
		halrf_get_power_limit_to_struct_40m_8852c(rf, phy);
	else if (bw == CHANNEL_WIDTH_80)
		halrf_get_power_limit_to_struct_80m_8852c(rf, phy);
	else
		halrf_get_power_limit_to_struct_160m_8852c(rf, phy);

	tmp = lmt->pwr_lmt_cck_40m[0][0];
	for (i = 0; i < 2; i++) {
		RF_DBG(rf, DBG_RF_POWER, "power_limit ==> %d   %d   %d   %d\n",
			lmt->pwr_lmt_cck_40m[i][1], lmt->pwr_lmt_cck_40m[i][0],
			lmt->pwr_lmt_cck_20m[i][1], lmt->pwr_lmt_cck_20m[i][0]);

		RF_DBG(rf, DBG_RF_POWER, "power_limit ==> %d   %d   %d   %d\n",
			lmt->pwr_lmt_20m[i][0][1], lmt->pwr_lmt_20m[i][0][0],
			lmt->pwr_lmt_lgcy_20m[i][1], lmt->pwr_lmt_lgcy_20m[i][0]);

		RF_DBG(rf, DBG_RF_POWER, "power_limit ==> %d   %d   %d   %d\n",
			lmt->pwr_lmt_20m[i][2][1], lmt->pwr_lmt_20m[i][2][0],
			lmt->pwr_lmt_20m[i][1][1], lmt->pwr_lmt_20m[i][1][0]);

		RF_DBG(rf, DBG_RF_POWER, "power_limit ==> %d   %d   %d	 %d\n",
			lmt->pwr_lmt_20m[i][4][1], lmt->pwr_lmt_20m[i][4][0],
			lmt->pwr_lmt_20m[i][3][1], lmt->pwr_lmt_20m[i][3][0]);

		
		RF_DBG(rf, DBG_RF_POWER, "power_limit ==> %d   %d   %d	 %d\n",
			lmt->pwr_lmt_20m[i][6][1], lmt->pwr_lmt_20m[i][6][0],
			lmt->pwr_lmt_20m[i][5][1], lmt->pwr_lmt_20m[i][5][0]);

		RF_DBG(rf, DBG_RF_POWER, "power_limit ==> %d   %d   %d	 %d\n",
			lmt->pwr_lmt_40m[i][0][1], lmt->pwr_lmt_40m[i][0][0],
			lmt->pwr_lmt_20m[i][7][1], lmt->pwr_lmt_20m[i][7][0]);

		RF_DBG(rf, DBG_RF_POWER, "power_limit ==> %d   %d   %d	 %d\n",
			lmt->pwr_lmt_40m[i][2][1], lmt->pwr_lmt_40m[i][2][0],
			lmt->pwr_lmt_40m[i][1][1], lmt->pwr_lmt_40m[i][1][0]);

		RF_DBG(rf, DBG_RF_POWER, "power_limit ==> %d   %d   %d	 %d\n",
			lmt->pwr_lmt_80m[i][0][1], lmt->pwr_lmt_80m[i][0][0],
			lmt->pwr_lmt_40m[i][3][1], lmt->pwr_lmt_40m[i][3][0]);

		RF_DBG(rf, DBG_RF_POWER, "power_limit ==> %d   %d   %d	 %d\n",
			lmt->pwr_lmt_160m[i][1], lmt->pwr_lmt_160m[i][0],
			lmt->pwr_lmt_80m[i][1][1], lmt->pwr_lmt_80m[i][1][0]);

		RF_DBG(rf, DBG_RF_POWER, "power_limit ==> %d   %d   %d	 %d\n",
			lmt->pwr_lmt_40m_2p5[i][1], lmt->pwr_lmt_40m_2p5[i][0],
			lmt->pwr_lmt_40m_0p5[i][1], lmt->pwr_lmt_40m_0p5[i][0]);
	}

	return true;
}

void halrf_get_power_limit_ru_to_struct_20m_8852c(struct rf_info *rf, enum phl_phy_idx phy)
{
	struct rtw_tpu_info *tpu = &rf->hal_com->band[phy].rtw_tpu_i;
	struct rtw_hal_com_t *hal = rf->hal_com;
	u8 ch = rf->hal_com->band[phy].cur_chandef.center_ch;

	RF_DBG(rf, DBG_RF_POWER, "======>%s channel=%d\n", __func__, ch);

	hal_mem_set(hal, tpu->pwr_lmt_ru, 0, sizeof(tpu->pwr_lmt_ru));

	/*1Tx RU26*/
	tpu->pwr_lmt_ru[PW_LMT_PH_1T][PW_LMT_RU_BW_RU26][0] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU26, PW_LMT_PH_1T, ch) / 2;

	/*1Tx RU52*/
	tpu->pwr_lmt_ru[PW_LMT_PH_1T][PW_LMT_RU_BW_RU52][0] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU52, PW_LMT_PH_1T, ch) / 2;

	/*1Tx RU106*/
	tpu->pwr_lmt_ru[PW_LMT_PH_1T][PW_LMT_RU_BW_RU106][0] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU106, PW_LMT_PH_1T, ch) / 2;

	/*2Tx RU26*/
	tpu->pwr_lmt_ru[PW_LMT_PH_2T][PW_LMT_RU_BW_RU26][0] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU26, PW_LMT_PH_2T, ch) / 2;

	/*2Tx RU52*/
	tpu->pwr_lmt_ru[PW_LMT_PH_2T][PW_LMT_RU_BW_RU52][0] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU52, PW_LMT_PH_2T, ch) / 2;

	/*2Tx RU106*/
	tpu->pwr_lmt_ru[PW_LMT_PH_2T][PW_LMT_RU_BW_RU106][0] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU106, PW_LMT_PH_2T, ch) / 2;
}

void halrf_get_power_limit_ru_to_struct_40m_8852c(struct rf_info *rf, enum phl_phy_idx phy)
{
	struct rtw_tpu_info *tpu = &rf->hal_com->band[phy].rtw_tpu_i;
	struct rtw_hal_com_t *hal = rf->hal_com;
	u8 ch = rf->hal_com->band[phy].cur_chandef.center_ch;

	RF_DBG(rf, DBG_RF_POWER, "======>%s channel=%d\n", __func__, ch);

	hal_mem_set(hal, tpu->pwr_lmt_ru, 0, sizeof(tpu->pwr_lmt_ru));

	/*1Tx RU26*/
	tpu->pwr_lmt_ru[PW_LMT_PH_1T][PW_LMT_RU_BW_RU26][0] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU26, PW_LMT_PH_1T, ch - 2) / 2;

	tpu->pwr_lmt_ru[PW_LMT_PH_1T][PW_LMT_RU_BW_RU26][1] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU26, PW_LMT_PH_1T, ch + 2) / 2;

	/*1Tx RU52*/
	tpu->pwr_lmt_ru[PW_LMT_PH_1T][PW_LMT_RU_BW_RU52][0] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU52, PW_LMT_PH_1T, ch - 2) / 2;

	tpu->pwr_lmt_ru[PW_LMT_PH_1T][PW_LMT_RU_BW_RU52][1] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU52, PW_LMT_PH_1T, ch + 2) / 2;

	/*1Tx RU106*/
	tpu->pwr_lmt_ru[PW_LMT_PH_1T][PW_LMT_RU_BW_RU106][0] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU106, PW_LMT_PH_1T, ch - 2) / 2;
	
	tpu->pwr_lmt_ru[PW_LMT_PH_1T][PW_LMT_RU_BW_RU106][1] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU106, PW_LMT_PH_1T, ch + 2) / 2;

	/*2TX RU26*/
	tpu->pwr_lmt_ru[PW_LMT_PH_2T][PW_LMT_RU_BW_RU26][0] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU26, PW_LMT_PH_2T, ch - 2) / 2;

	tpu->pwr_lmt_ru[PW_LMT_PH_2T][PW_LMT_RU_BW_RU26][1] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU26, PW_LMT_PH_2T, ch + 2) / 2;

	/*2TX RU52*/
	tpu->pwr_lmt_ru[PW_LMT_PH_2T][PW_LMT_RU_BW_RU52][0] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU52, PW_LMT_PH_2T, ch - 2) / 2;

	tpu->pwr_lmt_ru[PW_LMT_PH_2T][PW_LMT_RU_BW_RU52][1] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU52, PW_LMT_PH_2T, ch + 2) / 2;

	/*2TX RU106*/
	tpu->pwr_lmt_ru[PW_LMT_PH_2T][PW_LMT_RU_BW_RU106][0] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU106, PW_LMT_PH_2T, ch - 2) / 2;
	
	tpu->pwr_lmt_ru[PW_LMT_PH_2T][PW_LMT_RU_BW_RU106][1] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU106, PW_LMT_PH_2T, ch + 2) / 2;
}

void halrf_get_power_limit_ru_to_struct_80m_8852c(struct rf_info *rf, enum phl_phy_idx phy)
{
	struct rtw_tpu_info *tpu = &rf->hal_com->band[phy].rtw_tpu_i;
	struct rtw_hal_com_t *hal = rf->hal_com;
	u8 ch = rf->hal_com->band[phy].cur_chandef.center_ch;

	RF_DBG(rf, DBG_RF_POWER, "======>%s channel=%d\n", __func__, ch);

	hal_mem_set(hal, tpu->pwr_lmt_ru, 0, sizeof(tpu->pwr_lmt_ru));

	/*1TX RU26*/
	tpu->pwr_lmt_ru[PW_LMT_PH_1T][PW_LMT_RU_BW_RU26][0] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU26, PW_LMT_PH_1T, ch - 6) / 2;

	tpu->pwr_lmt_ru[PW_LMT_PH_1T][PW_LMT_RU_BW_RU26][1] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU26, PW_LMT_PH_1T, ch - 2) / 2;

	tpu->pwr_lmt_ru[PW_LMT_PH_1T][PW_LMT_RU_BW_RU26][2] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU26, PW_LMT_PH_1T, ch + 2) / 2;

	tpu->pwr_lmt_ru[PW_LMT_PH_1T][PW_LMT_RU_BW_RU26][3] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU26, PW_LMT_PH_1T, ch + 6) / 2;

	/*1TX RU52*/
	tpu->pwr_lmt_ru[PW_LMT_PH_1T][PW_LMT_RU_BW_RU52][0] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU52, PW_LMT_PH_1T, ch - 6) / 2;

	tpu->pwr_lmt_ru[PW_LMT_PH_1T][PW_LMT_RU_BW_RU52][1] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU52, PW_LMT_PH_1T, ch - 2) / 2;

	tpu->pwr_lmt_ru[PW_LMT_PH_1T][PW_LMT_RU_BW_RU52][2] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU52, PW_LMT_PH_1T, ch + 2) / 2;
	
	tpu->pwr_lmt_ru[PW_LMT_PH_1T][PW_LMT_RU_BW_RU52][3] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU52, PW_LMT_PH_1T, ch + 6) / 2;

	/*1TX RU106*/
	tpu->pwr_lmt_ru[PW_LMT_PH_1T][PW_LMT_RU_BW_RU106][0] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU106, PW_LMT_PH_1T, ch - 6) / 2;
	
	tpu->pwr_lmt_ru[PW_LMT_PH_1T][PW_LMT_RU_BW_RU106][1] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU106, PW_LMT_PH_1T, ch - 2) / 2;

	tpu->pwr_lmt_ru[PW_LMT_PH_1T][PW_LMT_RU_BW_RU106][2] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU106, PW_LMT_PH_1T, ch + 2) / 2;
	
	tpu->pwr_lmt_ru[PW_LMT_PH_1T][PW_LMT_RU_BW_RU106][3] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU106, PW_LMT_PH_1T, ch + 6) / 2;

	/*2TX RU26*/
	tpu->pwr_lmt_ru[PW_LMT_PH_2T][PW_LMT_RU_BW_RU26][0] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU26, PW_LMT_PH_2T, ch - 6) / 2;

	tpu->pwr_lmt_ru[PW_LMT_PH_2T][PW_LMT_RU_BW_RU26][1] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU26, PW_LMT_PH_2T, ch - 2) / 2;

	tpu->pwr_lmt_ru[PW_LMT_PH_2T][PW_LMT_RU_BW_RU26][2] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU26, PW_LMT_PH_2T, ch + 2) / 2;

	tpu->pwr_lmt_ru[PW_LMT_PH_2T][PW_LMT_RU_BW_RU26][3] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU26, PW_LMT_PH_2T, ch + 6) / 2;

	/*2TX RU52*/
	tpu->pwr_lmt_ru[PW_LMT_PH_2T][PW_LMT_RU_BW_RU52][0] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU52, PW_LMT_PH_2T, ch - 6) / 2;

	tpu->pwr_lmt_ru[PW_LMT_PH_2T][PW_LMT_RU_BW_RU52][1] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU52, PW_LMT_PH_2T, ch - 2) / 2;

	tpu->pwr_lmt_ru[PW_LMT_PH_2T][PW_LMT_RU_BW_RU52][2] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU52, PW_LMT_PH_2T, ch + 2) / 2;

	tpu->pwr_lmt_ru[PW_LMT_PH_2T][PW_LMT_RU_BW_RU52][3] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU52, PW_LMT_PH_2T, ch + 6) / 2;

	/*2TX RU106*/
	tpu->pwr_lmt_ru[PW_LMT_PH_2T][PW_LMT_RU_BW_RU106][0] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU106, PW_LMT_PH_2T, ch - 6) / 2;

	tpu->pwr_lmt_ru[PW_LMT_PH_2T][PW_LMT_RU_BW_RU106][1] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU106, PW_LMT_PH_2T, ch - 2) / 2;

	tpu->pwr_lmt_ru[PW_LMT_PH_2T][PW_LMT_RU_BW_RU106][2] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU106, PW_LMT_PH_2T, ch + 2) / 2;

	tpu->pwr_lmt_ru[PW_LMT_PH_2T][PW_LMT_RU_BW_RU106][3] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU106, PW_LMT_PH_2T, ch + 6) / 2;
}

void halrf_get_power_limit_ru_to_struct_160m_8852c(struct rf_info *rf, enum phl_phy_idx phy)
{
	struct rtw_tpu_info *tpu = &rf->hal_com->band[phy].rtw_tpu_i;
	struct rtw_hal_com_t *hal = rf->hal_com;
	u8 ch = rf->hal_com->band[phy].cur_chandef.center_ch;

	RF_DBG(rf, DBG_RF_POWER, "======>%s channel=%d\n", __func__, ch);

	hal_mem_set(hal, tpu->pwr_lmt_ru, 0, sizeof(tpu->pwr_lmt_ru));

	/*1TX RU26*/
	tpu->pwr_lmt_ru[PW_LMT_PH_1T][PW_LMT_RU_BW_RU26][0] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU26, PW_LMT_PH_1T, ch - 14) / 2;

	tpu->pwr_lmt_ru[PW_LMT_PH_1T][PW_LMT_RU_BW_RU26][1] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU26, PW_LMT_PH_1T, ch - 10) / 2;

	tpu->pwr_lmt_ru[PW_LMT_PH_1T][PW_LMT_RU_BW_RU26][2] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU26, PW_LMT_PH_1T, ch - 6) / 2;

	tpu->pwr_lmt_ru[PW_LMT_PH_1T][PW_LMT_RU_BW_RU26][3] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU26, PW_LMT_PH_1T, ch - 2) / 2;

	tpu->pwr_lmt_ru[PW_LMT_PH_1T][PW_LMT_RU_BW_RU26][4] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU26, PW_LMT_PH_1T, ch + 2) / 2;

	tpu->pwr_lmt_ru[PW_LMT_PH_1T][PW_LMT_RU_BW_RU26][5] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU26, PW_LMT_PH_1T, ch + 6) / 2;

	tpu->pwr_lmt_ru[PW_LMT_PH_1T][PW_LMT_RU_BW_RU26][6] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU26, PW_LMT_PH_1T, ch + 10) / 2;

	tpu->pwr_lmt_ru[PW_LMT_PH_1T][PW_LMT_RU_BW_RU26][7] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU26, PW_LMT_PH_1T, ch + 14) / 2;

	/*1TX RU52*/
	tpu->pwr_lmt_ru[PW_LMT_PH_1T][PW_LMT_RU_BW_RU52][0] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU52, PW_LMT_PH_1T, ch - 14) / 2;

	tpu->pwr_lmt_ru[PW_LMT_PH_1T][PW_LMT_RU_BW_RU52][1] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU52, PW_LMT_PH_1T, ch - 10) / 2;

	tpu->pwr_lmt_ru[PW_LMT_PH_1T][PW_LMT_RU_BW_RU52][2] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU52, PW_LMT_PH_1T, ch - 6) / 2;
	
	tpu->pwr_lmt_ru[PW_LMT_PH_1T][PW_LMT_RU_BW_RU52][3] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU52, PW_LMT_PH_1T, ch - 2) / 2;

	tpu->pwr_lmt_ru[PW_LMT_PH_1T][PW_LMT_RU_BW_RU52][4] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU52, PW_LMT_PH_1T, ch + 2) / 2;

	tpu->pwr_lmt_ru[PW_LMT_PH_1T][PW_LMT_RU_BW_RU52][5] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU52, PW_LMT_PH_1T, ch + 6) / 2;

	tpu->pwr_lmt_ru[PW_LMT_PH_1T][PW_LMT_RU_BW_RU52][6] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU52, PW_LMT_PH_1T, ch + 10) / 2;
	
	tpu->pwr_lmt_ru[PW_LMT_PH_1T][PW_LMT_RU_BW_RU52][7] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU52, PW_LMT_PH_1T, ch + 14) / 2;

	/*1TX RU106*/
	tpu->pwr_lmt_ru[PW_LMT_PH_1T][PW_LMT_RU_BW_RU106][0] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU106, PW_LMT_PH_1T, ch - 14) / 2;
	
	tpu->pwr_lmt_ru[PW_LMT_PH_1T][PW_LMT_RU_BW_RU106][1] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU106, PW_LMT_PH_1T, ch - 10) / 2;

	tpu->pwr_lmt_ru[PW_LMT_PH_1T][PW_LMT_RU_BW_RU106][2] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU106, PW_LMT_PH_1T, ch - 6) / 2;
	
	tpu->pwr_lmt_ru[PW_LMT_PH_1T][PW_LMT_RU_BW_RU106][3] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU106, PW_LMT_PH_1T, ch - 2) / 2;

	tpu->pwr_lmt_ru[PW_LMT_PH_1T][PW_LMT_RU_BW_RU106][4] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU106, PW_LMT_PH_1T, ch + 2) / 2;
	
	tpu->pwr_lmt_ru[PW_LMT_PH_1T][PW_LMT_RU_BW_RU106][5] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU106, PW_LMT_PH_1T, ch + 6) / 2;

	tpu->pwr_lmt_ru[PW_LMT_PH_1T][PW_LMT_RU_BW_RU106][6] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU106, PW_LMT_PH_1T, ch + 10) / 2;
	
	tpu->pwr_lmt_ru[PW_LMT_PH_1T][PW_LMT_RU_BW_RU106][7] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU106, PW_LMT_PH_1T, ch + 14) / 2;

	/*2TX RU26*/
	tpu->pwr_lmt_ru[PW_LMT_PH_2T][PW_LMT_RU_BW_RU26][0] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU26, PW_LMT_PH_2T, ch - 14) / 2;

	tpu->pwr_lmt_ru[PW_LMT_PH_2T][PW_LMT_RU_BW_RU26][1] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU26, PW_LMT_PH_2T, ch - 10) / 2;

	tpu->pwr_lmt_ru[PW_LMT_PH_2T][PW_LMT_RU_BW_RU26][2] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU26, PW_LMT_PH_2T, ch - 6) / 2;

	tpu->pwr_lmt_ru[PW_LMT_PH_2T][PW_LMT_RU_BW_RU26][3] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU26, PW_LMT_PH_2T, ch - 2) / 2;

	tpu->pwr_lmt_ru[PW_LMT_PH_2T][PW_LMT_RU_BW_RU26][4] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU26, PW_LMT_PH_2T, ch + 2) / 2;

	tpu->pwr_lmt_ru[PW_LMT_PH_2T][PW_LMT_RU_BW_RU26][5] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU26, PW_LMT_PH_2T, ch + 6) / 2;

	tpu->pwr_lmt_ru[PW_LMT_PH_2T][PW_LMT_RU_BW_RU26][6] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU26, PW_LMT_PH_2T, ch + 10) / 2;

	tpu->pwr_lmt_ru[PW_LMT_PH_2T][PW_LMT_RU_BW_RU26][7] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU26, PW_LMT_PH_2T, ch + 14) / 2;

	/*2TX RU52*/
	tpu->pwr_lmt_ru[PW_LMT_PH_2T][PW_LMT_RU_BW_RU52][0] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU52, PW_LMT_PH_2T, ch - 14) / 2;

	tpu->pwr_lmt_ru[PW_LMT_PH_2T][PW_LMT_RU_BW_RU52][1] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU52, PW_LMT_PH_2T, ch - 10) / 2;

	tpu->pwr_lmt_ru[PW_LMT_PH_2T][PW_LMT_RU_BW_RU52][2] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU52, PW_LMT_PH_2T, ch - 6) / 2;

	tpu->pwr_lmt_ru[PW_LMT_PH_2T][PW_LMT_RU_BW_RU52][3] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU52, PW_LMT_PH_2T, ch - 2) / 2;

	tpu->pwr_lmt_ru[PW_LMT_PH_2T][PW_LMT_RU_BW_RU52][4] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU52, PW_LMT_PH_2T, ch + 2) / 2;

	tpu->pwr_lmt_ru[PW_LMT_PH_2T][PW_LMT_RU_BW_RU52][5] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU52, PW_LMT_PH_2T, ch + 6) / 2;

	tpu->pwr_lmt_ru[PW_LMT_PH_2T][PW_LMT_RU_BW_RU52][6] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU52, PW_LMT_PH_2T, ch + 10) / 2;

	tpu->pwr_lmt_ru[PW_LMT_PH_2T][PW_LMT_RU_BW_RU52][7] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU52, PW_LMT_PH_2T, ch + 14) / 2;

	/*2TX RU106*/
	tpu->pwr_lmt_ru[PW_LMT_PH_2T][PW_LMT_RU_BW_RU106][0] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU106, PW_LMT_PH_2T, ch - 14) / 2;

	tpu->pwr_lmt_ru[PW_LMT_PH_2T][PW_LMT_RU_BW_RU106][1] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU106, PW_LMT_PH_2T, ch - 10) / 2;

	tpu->pwr_lmt_ru[PW_LMT_PH_2T][PW_LMT_RU_BW_RU106][2] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU106, PW_LMT_PH_2T, ch - 6) / 2;

	tpu->pwr_lmt_ru[PW_LMT_PH_2T][PW_LMT_RU_BW_RU106][3] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU106, PW_LMT_PH_2T, ch - 2) / 2;

	tpu->pwr_lmt_ru[PW_LMT_PH_2T][PW_LMT_RU_BW_RU106][4] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU106, PW_LMT_PH_2T, ch + 2) / 2;

	tpu->pwr_lmt_ru[PW_LMT_PH_2T][PW_LMT_RU_BW_RU106][5] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU106, PW_LMT_PH_2T, ch + 6) / 2;

	tpu->pwr_lmt_ru[PW_LMT_PH_2T][PW_LMT_RU_BW_RU106][6] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU106, PW_LMT_PH_2T, ch + 10) / 2;

	tpu->pwr_lmt_ru[PW_LMT_PH_2T][PW_LMT_RU_BW_RU106][7] =
		halrf_get_power_limit_ru(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, PW_LMT_RU_BW_RU106, PW_LMT_PH_2T, ch + 14) / 2;
}

bool halrf_set_power_limit_ru_to_struct_8852c(struct rf_info *rf, enum phl_phy_idx phy)
{
	struct rtw_tpu_info *tpu = &rf->hal_com->band[phy].rtw_tpu_i;
	u8 bw = rf->hal_com->band[phy].cur_chandef.bw;
	u8 i, j, k;

	RF_DBG(rf, DBG_RF_POWER, "======>%s   bandwidth=%d\n",
		__func__, bw);

	if (bw == CHANNEL_WIDTH_20)
		halrf_get_power_limit_ru_to_struct_20m_8852c(rf, phy);
	else if (bw == CHANNEL_WIDTH_40)
		halrf_get_power_limit_ru_to_struct_40m_8852c(rf, phy);
	else if (bw == CHANNEL_WIDTH_80)
		halrf_get_power_limit_ru_to_struct_80m_8852c(rf, phy);
	else
		halrf_get_power_limit_ru_to_struct_160m_8852c(rf, phy);

	tpu->pwr_lmt_ru_mem_size = tpu->pwr_lmt_ru_mem_size;

	for (k = 0; k < 2; k++) {
		for (j = 0; j < PW_LMT_RU_BW_NULL; j++) {
			for (i = 0; i < 8; i = i + 4) {
				RF_DBG(rf, DBG_RF_POWER,
					"power_limit_ru %dTx ==> %d   %d   %d   %d\n",
					k + 1,
					tpu->pwr_lmt_ru[k][j][i + 3],
					tpu->pwr_lmt_ru[k][j][i + 2],
					tpu->pwr_lmt_ru[k][j][i + 1],
					tpu->pwr_lmt_ru[k][j][i]);
			}
		}
	}
	return true;
}

void _halrf_set_tx_shape_8852c(struct rf_info *rf, enum phl_phy_idx phy)
{
	struct halrf_pwr_info *pwr = &rf->pwr_info;
	struct rtw_tpu_info *tpu = &rf->hal_com->band[phy].rtw_tpu_i;
	u32 band = rf->hal_com->band[phy].cur_chandef.band;
	u8 ch = rf->hal_com->band[phy].cur_chandef.center_ch;
	u8 reg;

	RF_DBG(rf, DBG_RF_POWER, "======>%s   channel=%d\n", __func__, ch);

	if (!(rf->support_ability & HAL_RF_TX_SHAPE)) {
		RF_DBG(rf, DBG_RF_POWER, "======>%s   rf->support_ability & HAL_RF_TX_SHAPE=%d   Return!!!\n",
			__func__, rf->support_ability & HAL_RF_TX_SHAPE);
		return;
	}

	if (pwr->set_tx_ptrn_shap_en == true) {
		RF_DBG(rf, DBG_RF_POWER, "======>%s   pwr->set_tx_ptrn_shap_en == true   Return!!!\n",
			__func__);
		return;
	} else {
		RF_DBG(rf, DBG_RF_POWER, "======>%s   pwr->set_tx_ptrn_shap_en == false  Set TX shape Default\n",
			__func__);
	}

	if (band == BAND_ON_24G) {
		reg = halrf_get_regulation_info(rf, BAND_ON_24G);

		RF_DBG(rf, DBG_RF_POWER, "======>%s   channel=%d   regulation=%d\n", __func__, ch, reg);
		RF_DBG(rf, DBG_RF_POWER, "tpu->tx_ptrn_shap_idx=%d   pwr->tx_shap_idx[%d][CCK][%d]=%d   pwr->tx_shap_idx[%d][OFDM][%d]=%d\n",
			tpu->tx_ptrn_shap_idx, PW_LMT_BAND_2_4G, reg,
			pwr->tx_shap_idx[PW_LMT_BAND_2_4G][TX_SHAPE_CCK][reg],
			PW_LMT_BAND_2_4G, reg,
			pwr->tx_shap_idx[PW_LMT_BAND_2_4G][TX_SHAPE_OFDM][reg]);

		tpu->tx_ptrn_shap_idx = pwr->tx_shap_idx[PW_LMT_BAND_2_4G][TX_SHAPE_CCK][reg];
		pwr->set_tx_ptrn_shap_idx[PW_LMT_BAND_2_4G][TX_SHAPE_CCK] =
			pwr->tx_shap_idx[PW_LMT_BAND_2_4G][TX_SHAPE_CCK][reg];

		halrf_bb_set_pow_patten_sharp(rf, ch, true,
			pwr->tx_shap_idx[PW_LMT_BAND_2_4G][TX_SHAPE_CCK][reg], phy);	/*cck*/

		RF_DBG(rf, DBG_RF_POWER, "[TX shape] tpu->tx_ptrn_shap_idx=%d  channel=%d  Set CCK Tx Shape!!!\n",
			tpu->tx_ptrn_shap_idx, ch);

		tpu->tx_ptrn_shap_idx = pwr->tx_shap_idx[PW_LMT_BAND_2_4G][TX_SHAPE_OFDM][reg];

		pwr->set_tx_ptrn_shap_idx[PW_LMT_BAND_2_4G][TX_SHAPE_OFDM] =
			pwr->tx_shap_idx[PW_LMT_BAND_2_4G][TX_SHAPE_OFDM][reg];

		halrf_bb_set_pow_patten_sharp(rf, ch, false,
			pwr->tx_shap_idx[PW_LMT_BAND_2_4G][TX_SHAPE_OFDM][reg], phy);	/*ofdm*/

		RF_DBG(rf, DBG_RF_POWER, "[TX shape] tpu->tx_ptrn_shap_idx=%d  channel=%d  Set OFDM Tx Shape!!!\n",
			tpu->tx_ptrn_shap_idx, ch);
	} else if (band == BAND_ON_5G) {
		reg = halrf_get_regulation_info(rf, BAND_ON_5G);

		RF_DBG(rf, DBG_RF_POWER, "======>%s   channel=%d   regulation=%d\n", __func__, ch, reg);
		RF_DBG(rf, DBG_RF_POWER, "tpu->tx_ptrn_shap_idx=%d   pwr->tx_shap_idx[%d][OFDM][%d]=%d\n",
			tpu->tx_ptrn_shap_idx, PW_LMT_BAND_5G, reg,
			pwr->tx_shap_idx[PW_LMT_BAND_5G][TX_SHAPE_OFDM][reg]);

		tpu->tx_ptrn_shap_idx = pwr->tx_shap_idx[PW_LMT_BAND_5G][TX_SHAPE_OFDM][reg];

		pwr->set_tx_ptrn_shap_idx[PW_LMT_BAND_5G][TX_SHAPE_OFDM] =
			pwr->tx_shap_idx[PW_LMT_BAND_5G][TX_SHAPE_OFDM][reg];

		halrf_bb_set_pow_patten_sharp(rf, ch, false,
			pwr->tx_shap_idx[PW_LMT_BAND_5G][TX_SHAPE_OFDM][reg], phy);	/*ofdm*/

		RF_DBG(rf, DBG_RF_POWER, "[TX shape] tpu->tx_ptrn_shap_idx=%d  channel=%d  Set Tx Shape!!!\n",
			tpu->tx_ptrn_shap_idx, ch);
	} else if (band == BAND_ON_6G) {
		reg = halrf_get_regulation_info(rf, BAND_ON_6G);

		RF_DBG(rf, DBG_RF_POWER, "======>%s   channel=%d   regulation=%d\n", __func__, ch, reg);
		RF_DBG(rf, DBG_RF_POWER, "tpu->tx_ptrn_shap_idx=%d   pwr->tx_shap_idx[%d][OFDM][%d]=%d\n",
			tpu->tx_ptrn_shap_idx, PW_LMT_BAND_6G, reg,
			pwr->tx_shap_idx[PW_LMT_BAND_6G][TX_SHAPE_OFDM][reg]);

		tpu->tx_ptrn_shap_idx = pwr->tx_shap_idx[PW_LMT_BAND_6G][TX_SHAPE_OFDM][reg];

		pwr->set_tx_ptrn_shap_idx[PW_LMT_BAND_6G][TX_SHAPE_OFDM] =
			pwr->tx_shap_idx[PW_LMT_BAND_6G][TX_SHAPE_OFDM][reg];

		halrf_bb_set_pow_patten_sharp(rf, ch, false,
			pwr->tx_shap_idx[PW_LMT_BAND_6G][TX_SHAPE_OFDM][reg], phy);	/*ofdm*/

		RF_DBG(rf, DBG_RF_POWER, "[TX shape] tpu->tx_ptrn_shap_idx=%d  channel=%d  Set Tx Shape!!!\n",
			tpu->tx_ptrn_shap_idx, ch);

	}
}

bool _halrf_set_power_8852c(struct rf_info *rf, enum phl_phy_idx phy,
	enum phl_pwr_table pwr_table)
{
	struct halrf_pwr_info *pwr = &rf->pwr_info;
	u8 i;

	RF_DBG(rf, DBG_RF_POWER, "======>%s phl_pwr_table=%d\n", __func__, pwr_table);

	RF_DBG(rf, DBG_RF_POWER, "======>%s From Driver pwr->pwr_by_rate_switch=%d\n",
			__func__, pwr->pwr_by_rate_switch);

	RF_DBG(rf, DBG_RF_POWER, "======>%s From Driver pwr->pwr_limit_switch=%d\n",
			__func__, pwr->pwr_limit_switch);

	RF_DBG(rf, DBG_RF_POWER, "======>%s From Efuse pwr->pwr_table_switch_efuse=%d\n",
			__func__, pwr->pwr_table_switch_efuse);

	if (pwr_table & PWR_BY_RATE) {
		if (pwr->pwr_by_rate_switch == PW_BY_RATE_ALL_SAME) {
			halrf_set_fix_power_to_struct_8852c(rf, phy,
				halrf_get_power_by_rate(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, 0, 0) / 2);
		} else {
			if (pwr->pwr_limit_switch == PWLMT_BY_EFUSE) {
				if (pwr->pwr_table_switch_efuse == 0) {
					halrf_set_power_by_rate_to_struct_8852c(rf, phy);
				} else if (pwr->pwr_table_switch_efuse == 2) {
					halrf_set_fix_power_to_struct_8852c(rf, phy,
						halrf_get_power_by_rate(rf, phy, RF_PATH_A, RTW_DATA_RATE_HE_NSS1_MCS7, 0, 0) / 2);
				} else {
					halrf_set_power_by_rate_to_struct_8852c(rf, phy);
				}
			} else
				halrf_set_power_by_rate_to_struct_8852c(rf, phy);
		}

		halrf_modify_pwr_table_bitmask(rf, phy, pwr_table);
		halrf_mac_write_pwr_by_rate_reg(rf, phy);
		halrf_mac_write_pwr_ofst_mode(rf, phy);

		if (rf->dbg_component & DBG_RF_POWER) {
			halrf_delay_ms(rf, 100);

			for (i = 0; i < TX_PWR_BY_RATE_NUM_MAC; i = i + 4)
				halrf_mac_get_pwr_reg_8852c(rf, phy, (0xd2c0 + i), 0xffffffff);
		}
	}

	if (pwr_table & PWR_LIMIT) {
		_halrf_set_tx_shape_8852c(rf, phy);
		if (!halrf_set_power_limit_to_struct_8852c(rf, phy)) {
			RF_DBG(rf, DBG_RF_POWER, "halrf_set_power_limit_to_struct_8852c return fail\n");
			return false;
		}

		halrf_modify_pwr_table_bitmask(rf, phy, pwr_table);
		halrf_mac_write_pwr_limit_reg(rf, phy);

		if (rf->dbg_component & DBG_RF_POWER) {
			halrf_delay_ms(rf, 100);

			for (i = 0; i < TX_PWR_LIMIT_NUM_MAC; i = i + 4)
				halrf_mac_get_pwr_reg_8852c(rf, phy, (0xd2ec + i), 0xffffffff);
		}
	}

	if (pwr_table & PWR_LIMIT_RU) {
		/*_halrf_set_tx_shape_8852c(rf, phy);*/
		if (!halrf_set_power_limit_ru_to_struct_8852c(rf, phy)) {
			RF_DBG(rf, DBG_RF_POWER, "halrf_set_power_limit_ru_to_struct_8852c return fail\n");
			return false;
		}

		halrf_modify_pwr_table_bitmask(rf, phy, pwr_table);
		halrf_mac_write_pwr_limit_rua_reg(rf, phy);

		if (rf->dbg_component & DBG_RF_POWER) {
			halrf_delay_ms(rf, 100);

			for (i = 0; i < 48; i = i + 4)
				halrf_mac_get_pwr_reg_8852c(rf, phy, (0xd33c + i), 0xffffffff);
		}
	}

	return true;
}

void _halrf_set_ext_power_diff_8852c(struct rf_info *rf, enum phl_phy_idx phy)
{
#ifdef SPF_PHL_RF_019_SAR
	struct halrf_pwr_info *pwr = &rf->pwr_info;
	struct rtw_phl_ext_pwr_lmt_info *ext_pwr_info = &rf->hal_com->band[phy].rtw_tpu_i.ext_pwr_lmt_i;
	u32 band = rf->hal_com->band[phy].cur_chandef.band;
	u8 channel = rf->hal_com->band[0].cur_chandef.center_ch;

	if (band == BAND_ON_24G) {
		pwr->ext_pwr_org[RF_PATH_A] = ext_pwr_info->ext_pwr_lmt_ant_2_4g[RF_PATH_A];
		pwr->ext_pwr_org[RF_PATH_B] = ext_pwr_info->ext_pwr_lmt_ant_2_4g[RF_PATH_B];
		pwr->ext_pwr[PW_LMT_PH_1T] = ext_pwr_info->ext_pwr_lmt_2_4g[PW_LMT_PH_1T];
		pwr->ext_pwr[PW_LMT_PH_2T] = ext_pwr_info->ext_pwr_lmt_2_4g[PW_LMT_PH_2T];
	} else if (band == BAND_ON_5G) {
		if (channel >= 36 && channel <= 48) {
			pwr->ext_pwr_org[RF_PATH_A] = ext_pwr_info->ext_pwr_lmt_ant_5g_band1[RF_PATH_A];
			pwr->ext_pwr_org[RF_PATH_B] = ext_pwr_info->ext_pwr_lmt_ant_5g_band1[RF_PATH_B];
			pwr->ext_pwr[PW_LMT_PH_1T] = ext_pwr_info->ext_pwr_lmt_5g_band1[PW_LMT_PH_1T];
			pwr->ext_pwr[PW_LMT_PH_2T] = ext_pwr_info->ext_pwr_lmt_5g_band1[PW_LMT_PH_2T];
		} else if (channel >= 50 && channel <= 64) {
			pwr->ext_pwr_org[RF_PATH_A] = ext_pwr_info->ext_pwr_lmt_ant_5g_band2[RF_PATH_A];
			pwr->ext_pwr_org[RF_PATH_B] = ext_pwr_info->ext_pwr_lmt_ant_5g_band2[RF_PATH_B];
			pwr->ext_pwr[PW_LMT_PH_1T] = ext_pwr_info->ext_pwr_lmt_5g_band2[PW_LMT_PH_1T];
			pwr->ext_pwr[PW_LMT_PH_2T] = ext_pwr_info->ext_pwr_lmt_5g_band2[PW_LMT_PH_2T];
		} else if (channel >= 100 && channel <= 144) {
			pwr->ext_pwr_org[RF_PATH_A] = ext_pwr_info->ext_pwr_lmt_ant_5g_band3[RF_PATH_A];
			pwr->ext_pwr_org[RF_PATH_B] = ext_pwr_info->ext_pwr_lmt_ant_5g_band3[RF_PATH_B];
			pwr->ext_pwr[PW_LMT_PH_1T] = ext_pwr_info->ext_pwr_lmt_5g_band3[PW_LMT_PH_1T];
			pwr->ext_pwr[PW_LMT_PH_2T] = ext_pwr_info->ext_pwr_lmt_5g_band3[PW_LMT_PH_2T];
		} else if (channel >= 149 && channel <= 177) {
			pwr->ext_pwr_org[RF_PATH_A] = ext_pwr_info->ext_pwr_lmt_ant_5g_band4[RF_PATH_A];
			pwr->ext_pwr_org[RF_PATH_B] = ext_pwr_info->ext_pwr_lmt_ant_5g_band4[RF_PATH_B];
			pwr->ext_pwr[PW_LMT_PH_1T] = ext_pwr_info->ext_pwr_lmt_5g_band4[PW_LMT_PH_1T];
			pwr->ext_pwr[PW_LMT_PH_2T] = ext_pwr_info->ext_pwr_lmt_5g_band4[PW_LMT_PH_2T];
		}
	 } else if (band == BAND_ON_6G) {
		if (channel >= 1 && channel <= 45) {
			pwr->ext_pwr_org[RF_PATH_A] = ext_pwr_info->ext_pwr_lmt_6g_unii_5_1[RF_PATH_A];
			pwr->ext_pwr_org[RF_PATH_B] = ext_pwr_info->ext_pwr_lmt_6g_unii_5_1[RF_PATH_B];
			pwr->ext_pwr[PW_LMT_PH_1T] = ext_pwr_info->ext_pwr_lmt_ant_6g_unii_5_1[PW_LMT_PH_1T];
			pwr->ext_pwr[PW_LMT_PH_2T] = ext_pwr_info->ext_pwr_lmt_ant_6g_unii_5_1[PW_LMT_PH_2T];
		} else if (channel >= 49 && channel <= 93) {
			pwr->ext_pwr_org[RF_PATH_A] = ext_pwr_info->ext_pwr_lmt_6g_unii_5_2[RF_PATH_A];
			pwr->ext_pwr_org[RF_PATH_B] = ext_pwr_info->ext_pwr_lmt_6g_unii_5_2[RF_PATH_B];
			pwr->ext_pwr[PW_LMT_PH_1T] = ext_pwr_info->ext_pwr_lmt_ant_6g_unii_5_2[PW_LMT_PH_1T];
			pwr->ext_pwr[PW_LMT_PH_2T] = ext_pwr_info->ext_pwr_lmt_ant_6g_unii_5_2[PW_LMT_PH_2T];
		} else if (channel >= 97 && channel <= 117) {
			pwr->ext_pwr_org[RF_PATH_A] = ext_pwr_info->ext_pwr_lmt_6g_unii_6[RF_PATH_A];
			pwr->ext_pwr_org[RF_PATH_B] = ext_pwr_info->ext_pwr_lmt_6g_unii_6[RF_PATH_B];
			pwr->ext_pwr[PW_LMT_PH_1T] = ext_pwr_info->ext_pwr_lmt_ant_6g_unii_6[PW_LMT_PH_1T];
			pwr->ext_pwr[PW_LMT_PH_2T] = ext_pwr_info->ext_pwr_lmt_ant_6g_unii_6[PW_LMT_PH_2T];
		} else if (channel >= 121 && channel <= 149) {
			pwr->ext_pwr_org[RF_PATH_A] = ext_pwr_info->ext_pwr_lmt_6g_unii_7_1[RF_PATH_A];
			pwr->ext_pwr_org[RF_PATH_B] = ext_pwr_info->ext_pwr_lmt_6g_unii_7_1[RF_PATH_B];
			pwr->ext_pwr[PW_LMT_PH_1T] = ext_pwr_info->ext_pwr_lmt_ant_6g_unii_7_1[PW_LMT_PH_1T];
			pwr->ext_pwr[PW_LMT_PH_2T] = ext_pwr_info->ext_pwr_lmt_ant_6g_unii_7_1[PW_LMT_PH_2T];
		} else if (channel >= 153 && channel <= 189) {
			pwr->ext_pwr_org[RF_PATH_A] = ext_pwr_info->ext_pwr_lmt_6g_unii_7_2[RF_PATH_A];
			pwr->ext_pwr_org[RF_PATH_B] = ext_pwr_info->ext_pwr_lmt_6g_unii_7_2[RF_PATH_B];
			pwr->ext_pwr[PW_LMT_PH_1T] = ext_pwr_info->ext_pwr_lmt_ant_6g_unii_7_2[PW_LMT_PH_1T];
			pwr->ext_pwr[PW_LMT_PH_2T] = ext_pwr_info->ext_pwr_lmt_ant_6g_unii_7_2[PW_LMT_PH_2T];
		} else if (channel >= 193 && channel <= 237) {
			pwr->ext_pwr_org[RF_PATH_A] = ext_pwr_info->ext_pwr_lmt_6g_unii_8[RF_PATH_A];
			pwr->ext_pwr_org[RF_PATH_B] = ext_pwr_info->ext_pwr_lmt_6g_unii_8[RF_PATH_B];
			pwr->ext_pwr[PW_LMT_PH_1T] = ext_pwr_info->ext_pwr_lmt_ant_6g_unii_8[PW_LMT_PH_1T];
			pwr->ext_pwr[PW_LMT_PH_2T] = ext_pwr_info->ext_pwr_lmt_ant_6g_unii_8[PW_LMT_PH_2T];
		}
	}

	if (pwr->ext_pwr_org[RF_PATH_A] == 0 && pwr->ext_pwr_org[RF_PATH_B] == 0) {
		rtw_hal_bb_set_tx_pow_per_path_lmt((rf)->hal_com, 0xff, 0xff);
		
		RF_DBG(rf, DBG_RF_POWER, "======>%s   Set PathA=0xff PathB=0xff Default Power\n",
			__func__);
	} else {
		rtw_hal_bb_set_tx_pow_per_path_lmt((rf)->hal_com,
			(s16)pwr->ext_pwr_org[RF_PATH_A], (s16)pwr->ext_pwr_org[RF_PATH_B]);

		RF_DBG(rf, DBG_RF_POWER, "======>%s   Set PathA=%d PathB=%d Different Power\n",
			__func__, pwr->ext_pwr_org[RF_PATH_A], pwr->ext_pwr_org[RF_PATH_B]);
	}

#endif
}

void halrf_set_ref_power_to_struct_8852c(struct rf_info *rf, enum phl_phy_idx phy)
{
	struct rtw_tpu_info *tpu = &rf->hal_com->band[phy].rtw_tpu_i;
	u8 rfe_type = rf->phl_com->dev_cap.rfe_type;

	RF_DBG(rf, DBG_RF_POWER, "======>%s\n", __func__);

	tpu->ofst_int = 0;
	tpu->ofst_fraction = 0;

	/*Set ref power*/
	if (rfe_type > 50)
		tpu->base_cw_0db = 0x21;
	else
		tpu->base_cw_0db = 0x27;

	tpu->tssi_16dBm_cw = 0x12c;

	/*[Ref Pwr]*/
	tpu->ref_pow_ofdm = 0;		/*0dBm*/
	tpu->ref_pow_cck = 0;		/*0dBm*/

	halrf_bb_set_tx_pow_ref(rf, phy);
}


bool halrf_set_power_8852c(struct rf_info *rf, enum phl_phy_idx phy,
	enum phl_pwr_table pwr_table)
{
#if 0
	if (rf->is_coex == true) {
		RF_DBG(rf, DBG_RF_POWER, "===>%s rf->is_coex==true return!!!\n", __func__);
		return true;
	}
#endif
	if (rf->hal_com->dbcc_en)
		halrf_get_efuse_power_table_switch(rf, HW_PHY_1);

	if (!_halrf_set_power_8852c(rf, phy, pwr_table)) {
		RF_DBG(rf, DBG_RF_POWER, "_halrf_set_power_8852c return fail\n");
		return false;
	}

	_halrf_set_ext_power_diff_8852c(rf, phy);
	halrf_set_bw160m_diff_8852c(rf, phy);

	return true;
}

void halrf_pwr_by_rate_info_8852c(struct rf_info *rf,
		char input[][16], u32 *_used, char *output, u32 *_out_len, enum phl_phy_idx phy)
{
	struct halrf_pwr_info *pwr = &rf->pwr_info;
	u8 channel = rf->hal_com->band[phy].cur_chandef.center_ch;
	u32 bw =  rf->hal_com->band[phy].cur_chandef.bw;
	u32 band = rf->hal_com->band[phy].cur_chandef.band;
	u32 reg_tmp, cck_ref, ofdm_ref;
	s32 s_cck_ref, s_ofdm_ref, diff_20m, diff_40m, diff_80m, diff_160m;
	s32 int_tmp[2], float_tmp[2];
	u32 power_constraint = pwr->power_constraint[phy];

	u32 used = *_used;
	u32 out_len = *_out_len;

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-30s = 0x%x\n",
		 "RF Para Ver", halrf_get_radio_ver_from_reg(rf));

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-30s = %d / %d\n",
		 "RFE type / PHY", rf->phl_com->dev_cap.rfe_type, phy);

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-30s = %s / %d / %s\n",
		 "Band / CH / BW", band == BAND_ON_24G ? "2G" : (band == BAND_ON_5G ? "5G" : "6G"),
		 channel,
		 bw == 0 ? "20M" : (bw == 1 ? "40M" : (bw == 2 ? "80M" : "160M")));

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-30s = %s / %s\n",
		 "Driver Swith / Efuse Switch",
		 (pwr->pwr_limit_switch == PWLMT_DISABLE) ? "Disable Limit" :
		 (pwr->pwr_limit_switch == PWBYRATE_AND_PWLMT) ? "Enable Limit" : "From Efuse",
		 (pwr->pwr_limit_switch == PWLMT_BY_EFUSE) ?
		 ((pwr->pwr_table_switch_efuse == 0) ? "Disable Limit" : (pwr->pwr_table_switch_efuse == 2) ? "The Same" : "Enable Limit")
		 : "From Driver");

	cck_ref = halrf_mac_get_pwr_reg_8852c(rf, phy, 0xd200, 0x0007fc00);
	ofdm_ref = halrf_mac_get_pwr_reg_8852c(rf, phy, 0xd200, 0x0ff80000);
	(cck_ref & BIT(8)) ? (s_cck_ref = cck_ref | 0xfffffe00) : (s_cck_ref = cck_ref);
	(ofdm_ref & BIT(8)) ? (s_ofdm_ref = ofdm_ref | 0xfffffe00) : (s_ofdm_ref = ofdm_ref);
	int_tmp[0] = s_cck_ref / 4;
	float_tmp[0] = s_cck_ref * 100 / 4 % 100;
	float_tmp[0] < 0 ? float_tmp[0] = float_tmp[0] * -1 : 0;
	int_tmp[1] = s_ofdm_ref / 4;
	float_tmp[1] = s_ofdm_ref * 100 / 4 % 100;
	float_tmp[1] < 0 ? float_tmp[1] = float_tmp[1] * -1 : 0;
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-30s = %s%d.%ddB / %s%d.%ddB\n",
		 "CCK REF / OFDM REF",
		 (int_tmp[0] == 0 && s_cck_ref < 0) ? "-" : "",
		 int_tmp[0], float_tmp[0],
		 (int_tmp[1] == 0 && s_ofdm_ref < 0) ? "-" : "",
		 int_tmp[1], float_tmp[1]);

	reg_tmp = halrf_mac_get_pwr_reg_8852c(rf, phy, 0xd208, 0xffffffff);
	diff_20m = (reg_tmp & 0x000f0000) >> 16;
	(diff_20m & BIT(3)) ? (diff_20m = diff_20m | 0xfffffff0) : (diff_20m = diff_20m);
	diff_20m = diff_20m * 10 / 2;
	diff_40m = (reg_tmp & 0x0000f000) >> 12;
	(diff_40m & BIT(3)) ? (diff_40m = diff_40m | 0xfffffff0) : (diff_40m = diff_40m);
	diff_40m = diff_40m * 10 / 2;
	diff_80m = (reg_tmp & 0x00000f00) >> 8;
	(diff_80m & BIT(3)) ? (diff_80m = diff_80m | 0xfffffff0) : (diff_80m = diff_80m);
	diff_80m = diff_80m * 10 / 2;
	diff_160m = (reg_tmp & 0x000000f0) >> 4;
	(diff_160m & BIT(3)) ? (diff_160m = diff_160m | 0xfffffff0) : (diff_160m = diff_160m);
	diff_160m = diff_160m * 10 / 2;
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-30s = %s%d.%d / %s%d.%d / %s%d.%d / %s%d.%d (dB)\n",
		"PwrDiff 20M / 40M / 80M / 160M",
		(diff_20m < 0) ? "-" : "",
		(diff_20m < 0) ? (diff_20m / 10) * -1 : (diff_20m / 10),
		(diff_20m < 0) ? (diff_20m % 10) * -1 : (diff_20m % 10),
		(diff_40m < 0) ? "-" : "",
		(diff_40m < 0) ? (diff_40m / 10) * -1 : (diff_40m / 10),
		(diff_40m < 0) ? (diff_40m % 10) * -1 : (diff_40m % 10),
		(diff_80m < 0) ? "-" : "",
		(diff_80m < 0) ? (diff_80m / 10) * -1 : (diff_80m / 10),
		(diff_80m < 0) ? (diff_80m % 10) * -1 : (diff_80m % 10),
		(diff_160m < 0) ? "-" : "",
		(diff_160m < 0) ? (diff_160m / 10) * -1 : (diff_160m / 10),
		(diff_160m < 0) ? (diff_160m % 10) * -1 : (diff_160m % 10));

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-30s = %d.%ddB\n",
		 "Power Constraint",
		 power_constraint / 4, (((power_constraint * 10) / 4) % 10));

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-30s = %s%d.%ddB\n",
		"DPK MCC Power Control",
		halrf_pwr_is_minus(rf, pwr->dpk_mcc_power / 2) ? "-" : "",
		halrf_show_pwr_table(rf, pwr->dpk_mcc_power / 2) / 10,
		halrf_show_pwr_table(rf, pwr->dpk_mcc_power / 2) % 10);

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-30s = %s%d.%ddB\n",
		"TX Rate Power Control",
		halrf_pwr_is_minus(rf, pwr->tx_rate_power_control[phy] / 2) ? "-" : "",
		halrf_show_pwr_table(rf, pwr->tx_rate_power_control[phy] / 2) / 10,
		halrf_show_pwr_table(rf, pwr->tx_rate_power_control[phy] / 2) % 10);

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, "1TX\n");

	reg_tmp = halrf_mac_get_pwr_reg_8852c(rf, phy, 0xd2c0, 0xffffffff);
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-36s = %s%d.%d / %s%d.%d / %s%d.%d / %s%d.%d\n",
		 "CCK 11M / 5.5M / 2M / 1M",
		 halrf_pwr_is_minus(rf, (reg_tmp & 0x7f000000) >> 24) ? "-" : "",
		 halrf_show_pwr_table(rf, (reg_tmp & 0x7f000000) >> 24) / 10,
		 halrf_show_pwr_table(rf, (reg_tmp & 0x7f000000) >> 24) % 10,
		 halrf_pwr_is_minus(rf, (reg_tmp & 0x7f0000) >> 16) ? "-" : "",
		 halrf_show_pwr_table(rf, (reg_tmp & 0x7f0000) >> 16) / 10,
		 halrf_show_pwr_table(rf, (reg_tmp & 0x7f0000) >> 16) % 10,
		 halrf_pwr_is_minus(rf, (reg_tmp & 0x7f00) >> 8) ? "-" : "",
		 halrf_show_pwr_table(rf, (reg_tmp & 0x7f00) >> 8) / 10,
		 halrf_show_pwr_table(rf, (reg_tmp & 0x7f00) >> 8) % 10,
		 halrf_pwr_is_minus(rf, reg_tmp & 0x7f) ? "-" : "",
		 halrf_show_pwr_table(rf, reg_tmp & 0x7f) / 10,
		 halrf_show_pwr_table(rf, reg_tmp & 0x7f) % 10
		 );

	reg_tmp = halrf_mac_get_pwr_reg_8852c(rf, phy, 0xd2c4, 0xffffffff);
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-36s = %s%d.%d / %s%d.%d / %s%d.%d / %s%d.%d\n",
		 "OFDM 18M / 12M / 9M / 6M",
		 halrf_pwr_is_minus(rf, (reg_tmp & 0x7f000000) >> 24) ? "-" : "",
		 halrf_show_pwr_table(rf, (reg_tmp & 0x7f000000) >> 24) / 10,
		 halrf_show_pwr_table(rf, (reg_tmp & 0x7f000000) >> 24) % 10,
		 halrf_pwr_is_minus(rf, (reg_tmp & 0x7f0000) >> 16) ? "-" : "",
		 halrf_show_pwr_table(rf, (reg_tmp & 0x7f0000) >> 16) / 10,
		 halrf_show_pwr_table(rf, (reg_tmp & 0x7f0000) >> 16) % 10,
		 halrf_pwr_is_minus(rf, (reg_tmp & 0x7f00) >> 8) ? "-" : "",
		 halrf_show_pwr_table(rf, (reg_tmp & 0x7f00) >> 8) / 10,
		 halrf_show_pwr_table(rf, (reg_tmp & 0x7f00) >> 8) % 10,
		 halrf_pwr_is_minus(rf, reg_tmp & 0x7f) ? "-" : "",
		 halrf_show_pwr_table(rf, reg_tmp & 0x7f) / 10,
		 halrf_show_pwr_table(rf, reg_tmp & 0x7f) % 10
		 );

	reg_tmp = halrf_mac_get_pwr_reg_8852c(rf, phy, 0xd2c8, 0xffffffff);
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-36s = %s%d.%d / %s%d.%d / %s%d.%d / %s%d.%d\n",
		 "OFDM 54M / 48M / 36M / 24M",
		 halrf_pwr_is_minus(rf, (reg_tmp & 0x7f000000) >> 24) ? "-" : "",
		 halrf_show_pwr_table(rf, (reg_tmp & 0x7f000000) >> 24) / 10,
		 halrf_show_pwr_table(rf, (reg_tmp & 0x7f000000) >> 24) % 10,
		 halrf_pwr_is_minus(rf, (reg_tmp & 0x7f0000) >> 16) ? "-" : "",
		 halrf_show_pwr_table(rf, (reg_tmp & 0x7f0000) >> 16) / 10,
		 halrf_show_pwr_table(rf, (reg_tmp & 0x7f0000) >> 16) % 10,
		 halrf_pwr_is_minus(rf, (reg_tmp & 0x7f00) >> 8) ? "-" : "",
		 halrf_show_pwr_table(rf, (reg_tmp & 0x7f00) >> 8) / 10,
		 halrf_show_pwr_table(rf, (reg_tmp & 0x7f00) >> 8) % 10,
		 halrf_pwr_is_minus(rf, reg_tmp & 0x7f) ? "-" : "",
		 halrf_show_pwr_table(rf, reg_tmp & 0x7f) / 10,
		 halrf_show_pwr_table(rf, reg_tmp & 0x7f) % 10
		 );

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-36s\n",
		 "HT / VHT / HE");

	reg_tmp = halrf_mac_get_pwr_reg_8852c(rf, phy, 0xd2cc, 0xffffffff);
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-36s = %s%d.%d / %s%d.%d / %s%d.%d / %s%d.%d\n",
		"MCS3 / MCS2 / MCS1 / MCS0",
		halrf_pwr_is_minus(rf, (reg_tmp & 0x7f000000) >> 24) ? "-" : "",
		halrf_show_pwr_table(rf, (reg_tmp & 0x7f000000) >> 24) / 10,
		halrf_show_pwr_table(rf, (reg_tmp & 0x7f000000) >> 24) % 10,
		halrf_pwr_is_minus(rf, (reg_tmp & 0x7f0000) >> 16) ? "-" : "",
		halrf_show_pwr_table(rf, (reg_tmp & 0x7f0000) >> 16) / 10,
		halrf_show_pwr_table(rf, (reg_tmp & 0x7f0000) >> 16) % 10,
		halrf_pwr_is_minus(rf, (reg_tmp & 0x7f00) >> 8) ? "-" : "",
		halrf_show_pwr_table(rf, (reg_tmp & 0x7f00) >> 8) / 10,
		halrf_show_pwr_table(rf, (reg_tmp & 0x7f00) >> 8) % 10,
		halrf_pwr_is_minus(rf, reg_tmp & 0x7f) ? "-" : "",
		halrf_show_pwr_table(rf, reg_tmp & 0x7f) / 10,
		halrf_show_pwr_table(rf, reg_tmp & 0x7f) % 10
		);

	reg_tmp = halrf_mac_get_pwr_reg_8852c(rf, phy, 0xd2d0, 0xffffffff);
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-36s = %s%d.%d / %s%d.%d / %s%d.%d / %s%d.%d\n",
		 "MCS7 / MCS6 / MCS5 / MCS4",
		halrf_pwr_is_minus(rf, (reg_tmp & 0x7f000000) >> 24) ? "-" : "",
		halrf_show_pwr_table(rf, (reg_tmp & 0x7f000000) >> 24) / 10,
		halrf_show_pwr_table(rf, (reg_tmp & 0x7f000000) >> 24) % 10,
		halrf_pwr_is_minus(rf, (reg_tmp & 0x7f0000) >> 16) ? "-" : "",
		halrf_show_pwr_table(rf, (reg_tmp & 0x7f0000) >> 16) / 10,
		halrf_show_pwr_table(rf, (reg_tmp & 0x7f0000) >> 16) % 10,
		halrf_pwr_is_minus(rf, (reg_tmp & 0x7f00) >> 8) ? "-" : "",
		halrf_show_pwr_table(rf, (reg_tmp & 0x7f00) >> 8) / 10,
		halrf_show_pwr_table(rf, (reg_tmp & 0x7f00) >> 8) % 10,
		halrf_pwr_is_minus(rf, reg_tmp & 0x7f) ? "-" : "",
		halrf_show_pwr_table(rf, reg_tmp & 0x7f) / 10,
		halrf_show_pwr_table(rf, reg_tmp & 0x7f) % 10
		);

	reg_tmp = halrf_mac_get_pwr_reg_8852c(rf, phy, 0xd2d4, 0xffffffff);
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-36s = %s%d.%d / %s%d.%d / %s%d.%d / %s%d.%d\n",
		 "MCS11 / MCS10 / MCS9 / MCS8",
		 halrf_pwr_is_minus(rf, (reg_tmp & 0x7f000000) >> 24) ? "-" : "",
		 halrf_show_pwr_table(rf, (reg_tmp & 0x7f000000) >> 24) / 10,
		 halrf_show_pwr_table(rf, (reg_tmp & 0x7f000000) >> 24) % 10,
		 halrf_pwr_is_minus(rf, (reg_tmp & 0x7f0000) >> 16) ? "-" : "",
		 halrf_show_pwr_table(rf, (reg_tmp & 0x7f0000) >> 16) / 10,
		 halrf_show_pwr_table(rf, (reg_tmp & 0x7f0000) >> 16) % 10,
		 halrf_pwr_is_minus(rf, (reg_tmp & 0x7f00) >> 8) ? "-" : "",
		 halrf_show_pwr_table(rf, (reg_tmp & 0x7f00) >> 8) / 10,
		 halrf_show_pwr_table(rf, (reg_tmp & 0x7f00) >> 8) % 10,
		 halrf_pwr_is_minus(rf, reg_tmp & 0x7f) ? "-" : "",
		 halrf_show_pwr_table(rf, reg_tmp & 0x7f) / 10,
		 halrf_show_pwr_table(rf, reg_tmp & 0x7f) % 10
		 );

	reg_tmp = halrf_mac_get_pwr_reg_8852c(rf, phy, 0xd2d8, 0xffffffff);
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-36s = %s%d.%d / %s%d.%d / %s%d.%d / %s%d.%d\n",
		 "DCM MCS4 / MCS3 / MCS1 / MCS0",
		halrf_pwr_is_minus(rf, (reg_tmp & 0x7f000000) >> 24) ? "-" : "",
		halrf_show_pwr_table(rf, (reg_tmp & 0x7f000000) >> 24) / 10,
		halrf_show_pwr_table(rf, (reg_tmp & 0x7f000000) >> 24) % 10,
		halrf_pwr_is_minus(rf, (reg_tmp & 0x7f0000) >> 16) ? "-" : "",
		halrf_show_pwr_table(rf, (reg_tmp & 0x7f0000) >> 16) / 10,
		halrf_show_pwr_table(rf, (reg_tmp & 0x7f0000) >> 16) % 10,
		halrf_pwr_is_minus(rf, (reg_tmp & 0x7f00) >> 8) ? "-" : "",
		halrf_show_pwr_table(rf, (reg_tmp & 0x7f00) >> 8) / 10,
		halrf_show_pwr_table(rf, (reg_tmp & 0x7f00) >> 8) % 10,
		halrf_pwr_is_minus(rf, reg_tmp & 0x7f) ? "-" : "",
		halrf_show_pwr_table(rf, reg_tmp & 0x7f) / 10,
		halrf_show_pwr_table(rf, reg_tmp & 0x7f) % 10
		);

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, "2TX\n");

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-36s\n",
		 "HT / VHT / HE");

	reg_tmp = halrf_mac_get_pwr_reg_8852c(rf, phy, 0xd2dc, 0xffffffff);
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-36s = %s%d.%d / %s%d.%d / %s%d.%d / %s%d.%d\n",
		 "MCS3 / MCS2 / MCS1 / MCS0",
		 halrf_pwr_is_minus(rf, (reg_tmp & 0x7f000000) >> 24) ? "-" : "",
		 halrf_show_pwr_table(rf, (reg_tmp & 0x7f000000) >> 24) / 10,
		 halrf_show_pwr_table(rf, (reg_tmp & 0x7f000000) >> 24) % 10,
		 halrf_pwr_is_minus(rf, (reg_tmp & 0x7f0000) >> 16) ? "-" : "",
		 halrf_show_pwr_table(rf, (reg_tmp & 0x7f0000) >> 16) / 10,
		 halrf_show_pwr_table(rf, (reg_tmp & 0x7f0000) >> 16) % 10,
		 halrf_pwr_is_minus(rf, (reg_tmp & 0x7f00) >> 8) ? "-" : "",
		 halrf_show_pwr_table(rf, (reg_tmp & 0x7f00) >> 8) / 10,
		 halrf_show_pwr_table(rf, (reg_tmp & 0x7f00) >> 8) % 10,
		 halrf_pwr_is_minus(rf, reg_tmp & 0x7f) ? "-" : "",
		 halrf_show_pwr_table(rf, reg_tmp & 0x7f) / 10,
		 halrf_show_pwr_table(rf, reg_tmp & 0x7f) % 10
		 );

	reg_tmp = halrf_mac_get_pwr_reg_8852c(rf, phy, 0xd2e0, 0xffffffff);
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-36s = %s%d.%d / %s%d.%d / %s%d.%d / %s%d.%d\n",
		 "MCS7 / MCS6 / MCS5 / MCS4",
		halrf_pwr_is_minus(rf, (reg_tmp & 0x7f000000) >> 24) ? "-" : "",
		halrf_show_pwr_table(rf, (reg_tmp & 0x7f000000) >> 24) / 10,
		halrf_show_pwr_table(rf, (reg_tmp & 0x7f000000) >> 24) % 10,
		halrf_pwr_is_minus(rf, (reg_tmp & 0x7f0000) >> 16) ? "-" : "",
		halrf_show_pwr_table(rf, (reg_tmp & 0x7f0000) >> 16) / 10,
		halrf_show_pwr_table(rf, (reg_tmp & 0x7f0000) >> 16) % 10,
		halrf_pwr_is_minus(rf, (reg_tmp & 0x7f00) >> 8) ? "-" : "",
		halrf_show_pwr_table(rf, (reg_tmp & 0x7f00) >> 8) / 10,
		halrf_show_pwr_table(rf, (reg_tmp & 0x7f00) >> 8) % 10,
		halrf_pwr_is_minus(rf, reg_tmp & 0x7f) ? "-" : "",
		halrf_show_pwr_table(rf, reg_tmp & 0x7f) / 10,
		halrf_show_pwr_table(rf, reg_tmp & 0x7f) % 10
		);

	reg_tmp = halrf_mac_get_pwr_reg_8852c(rf, phy, 0xd2e4, 0xffffffff);
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-36s = %s%d.%d / %s%d.%d / %s%d.%d / %s%d.%d\n",
		 "MCS11 / MCS10 / MCS9 / MCS8",
		 halrf_pwr_is_minus(rf, (reg_tmp & 0x7f000000) >> 24) ? "-" : "",
		 halrf_show_pwr_table(rf, (reg_tmp & 0x7f000000) >> 24) / 10,
		 halrf_show_pwr_table(rf, (reg_tmp & 0x7f000000) >> 24) % 10,
		 halrf_pwr_is_minus(rf, (reg_tmp & 0x7f0000) >> 16) ? "-" : "",
		 halrf_show_pwr_table(rf, (reg_tmp & 0x7f0000) >> 16) / 10,
		 halrf_show_pwr_table(rf, (reg_tmp & 0x7f0000) >> 16) % 10,
		 halrf_pwr_is_minus(rf, (reg_tmp & 0x7f00) >> 8) ? "-" : "",
		 halrf_show_pwr_table(rf, (reg_tmp & 0x7f00) >> 8) / 10,
		 halrf_show_pwr_table(rf, (reg_tmp & 0x7f00) >> 8) % 10,
		 halrf_pwr_is_minus(rf, reg_tmp & 0x7f) ? "-" : "",
		 halrf_show_pwr_table(rf, reg_tmp & 0x7f) / 10,
		 halrf_show_pwr_table(rf, reg_tmp & 0x7f) % 10
		 );

	reg_tmp = halrf_mac_get_pwr_reg_8852c(rf, phy, 0xd2e8, 0xffffffff);
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-36s = %s%d.%d / %s%d.%d / %s%d.%d / %s%d.%d\n",
		 "DCM MCS4 / MCS3 / MCS1 / MCS0",
		halrf_pwr_is_minus(rf, (reg_tmp & 0x7f000000) >> 24) ? "-" : "",
		halrf_show_pwr_table(rf, (reg_tmp & 0x7f000000) >> 24) / 10,
		halrf_show_pwr_table(rf, (reg_tmp & 0x7f000000) >> 24) % 10,
		halrf_pwr_is_minus(rf, (reg_tmp & 0x7f0000) >> 16) ? "-" : "",
		halrf_show_pwr_table(rf, (reg_tmp & 0x7f0000) >> 16) / 10,
		halrf_show_pwr_table(rf, (reg_tmp & 0x7f0000) >> 16) % 10,
		halrf_pwr_is_minus(rf, (reg_tmp & 0x7f00) >> 8) ? "-" : "",
		halrf_show_pwr_table(rf, (reg_tmp & 0x7f00) >> 8) / 10,
		halrf_show_pwr_table(rf, (reg_tmp & 0x7f00) >> 8) % 10,
		halrf_pwr_is_minus(rf, reg_tmp & 0x7f) ? "-" : "",
		halrf_show_pwr_table(rf, reg_tmp & 0x7f) / 10,
		halrf_show_pwr_table(rf, reg_tmp & 0x7f) % 10
		);

	*_used = used;
	*_out_len = out_len;

}

void halrf_pwr_limit_info_8852c(struct rf_info *rf,
		char input[][16], u32 *_used, char *output, u32 *_out_len, enum phl_phy_idx phy)
{
	struct halrf_pwr_info *pwr = &rf->pwr_info;
	struct rtw_tpu_info *tpu = &rf->hal_com->band[phy].rtw_tpu_i;
	u8 channel = rf->hal_com->band[phy].cur_chandef.center_ch;
	u8 txsc_ch = rf->hal_com->band[phy].cur_chandef.chan;
	u32 bw =  rf->hal_com->band[phy].cur_chandef.bw;
	u32 band = rf->hal_com->band[phy].cur_chandef.band;
	u32 reg_tmp, cck_ref, ofdm_ref;
	s32 s_cck_ref, s_ofdm_ref, diff_20m, diff_40m, diff_80m, diff_160m;
	s32 int_tmp[2], float_tmp[2];
	u32 power_constraint = pwr->power_constraint[phy];
	s32 tmp[2] = {0}, tmp1[2] = {0};

	u32 used = *_used;
	u32 out_len = *_out_len;

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-30s = 0x%x\n",
		 "RF Para Ver", halrf_get_radio_ver_from_reg(rf));

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-30s = %d / %d\n",
		 "RFE type / PHY", rf->phl_com->dev_cap.rfe_type, phy);

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-30s = %s / %d / %s / %d\n",
		 "Band / CH / BW / TXSC", band == BAND_ON_24G ? "2G" : (band == BAND_ON_5G ? "5G" : "6G"),
		 channel,
		 bw == 0 ? "20M" : (bw == 1 ? "40M" : (bw == 2 ? "80M" : "160M")),
		 txsc_ch);
	
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-30s = %s / %s / %s\n",
		 "Regulation 2G / 5G / 6G",
		 halrf_get_pw_lmt_regu_type_str_extra(rf, BAND_ON_24G),
		 halrf_get_pw_lmt_regu_type_str_extra(rf, BAND_ON_5G),
		 halrf_get_pw_lmt_regu_type_str_extra(rf, BAND_ON_6G));

	cck_ref = halrf_mac_get_pwr_reg_8852c(rf, phy, 0xd200, 0x0007fc00);
	ofdm_ref = halrf_mac_get_pwr_reg_8852c(rf, phy, 0xd200, 0x0ff80000);
	(cck_ref & BIT(8)) ? (s_cck_ref = cck_ref | 0xfffffe00) : (s_cck_ref = cck_ref);
	(ofdm_ref & BIT(8)) ? (s_ofdm_ref = ofdm_ref | 0xfffffe00) : (s_ofdm_ref = ofdm_ref);
	int_tmp[0] = s_cck_ref / 4;
	float_tmp[0] = s_cck_ref * 100 / 4 % 100;
	float_tmp[0] < 0 ? float_tmp[0] = float_tmp[0] * -1 : 0;
	int_tmp[1] = s_ofdm_ref / 4;
	float_tmp[1] = s_ofdm_ref * 100 / 4 % 100;
	float_tmp[1] < 0 ? float_tmp[1] = float_tmp[1] * -1 : 0;
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-30s = %s%d.%ddB / %s%d.%ddB\n",
		 "CCK REF / OFDM REF",
		 (int_tmp[0] == 0 && s_cck_ref < 0) ? "-" : "",
		 int_tmp[0], float_tmp[0],
		 (int_tmp[1] == 0 && s_ofdm_ref < 0) ? "-" : "",
		 int_tmp[1], float_tmp[1]);

	reg_tmp = halrf_mac_get_pwr_reg_8852c(rf, phy, 0xd208, 0xffffffff);
	diff_20m = (reg_tmp & 0x000f0000) >> 16;
	(diff_20m & BIT(3)) ? (diff_20m = diff_20m | 0xfffffff0) : (diff_20m = diff_20m);
	diff_20m = diff_20m * 10 / 2;
	diff_40m = (reg_tmp & 0x0000f000) >> 12;
	(diff_40m & BIT(3)) ? (diff_40m = diff_40m | 0xfffffff0) : (diff_40m = diff_40m);
	diff_40m = diff_40m * 10 / 2;
	diff_80m = (reg_tmp & 0x00000f00) >> 8;
	(diff_80m & BIT(3)) ? (diff_80m = diff_80m | 0xfffffff0) : (diff_80m = diff_80m);
	diff_80m = diff_80m * 10 / 2;
	diff_160m = (reg_tmp & 0x000000f0) >> 4;
	(diff_160m & BIT(3)) ? (diff_160m = diff_160m | 0xfffffff0) : (diff_160m = diff_160m);
	diff_160m = diff_160m * 10 / 2;
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-30s = %s%d.%d / %s%d.%d / %s%d.%d / %s%d.%d (dB)\n",
		"PwrDiff 20M / 40M / 80M / 160M",
		(diff_20m < 0) ? "-" : "",
		(diff_20m < 0) ? (diff_20m / 10) * -1 : (diff_20m / 10),
		(diff_20m < 0) ? (diff_20m % 10) * -1 : (diff_20m % 10),
		(diff_40m < 0) ? "-" : "",
		(diff_40m < 0) ? (diff_40m / 10) * -1 : (diff_40m / 10),
		(diff_40m < 0) ? (diff_40m % 10) * -1 : (diff_40m % 10),
		(diff_80m < 0) ? "-" : "",
		(diff_80m < 0) ? (diff_80m / 10) * -1 : (diff_80m / 10),
		(diff_80m < 0) ? (diff_80m % 10) * -1 : (diff_80m % 10),
		(diff_160m < 0) ? "-" : "",
		(diff_160m < 0) ? (diff_160m / 10) * -1 : (diff_160m / 10),
		(diff_160m < 0) ? (diff_160m % 10) * -1 : (diff_160m % 10));

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-30s = %d.%ddB\n",
		 "Power Constraint",
		 power_constraint / 4, (((power_constraint * 10) / 4) % 10));

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-30s = %s%d.%ddB\n",
		"DPK MCC Power Control",
		halrf_pwr_is_minus(rf, pwr->dpk_mcc_power / 2) ? "-" : "",
		halrf_show_pwr_table(rf, pwr->dpk_mcc_power / 2) / 10,
		halrf_show_pwr_table(rf, pwr->dpk_mcc_power / 2) % 10);

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-30s = %s\n",
		 "Power Limit (Reg)",
		 halrf_mac_get_pwr_reg_8852c(rf, phy, 0xd208, BIT(21)) ? "Enable Limit" : "Disable Limit");

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-30s = %s / %s\n",
		 "Driver Swith / Efuse Switch",
		 (pwr->pwr_limit_switch == PWLMT_DISABLE) ? "Disable Limit" :
		 (pwr->pwr_limit_switch == PWBYRATE_AND_PWLMT) ? "Enable Limit" : "From Efuse",
		 (pwr->pwr_limit_switch == PWLMT_BY_EFUSE) ?
		 ((pwr->pwr_table_switch_efuse == 0) ? "Disable Limit" : (pwr->pwr_table_switch_efuse == 2) ? "The Same" : "Enable Limit")
		 : "From Driver");

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-30s = %d / %d / %d\n",
		"TX Shape CCK / 2G / 5G",
		pwr->set_tx_ptrn_shap_idx[PW_LMT_BAND_2_4G][TX_SHAPE_CCK],
		pwr->set_tx_ptrn_shap_idx[PW_LMT_BAND_2_4G][TX_SHAPE_OFDM],
		pwr->set_tx_ptrn_shap_idx[PW_LMT_BAND_5G][TX_SHAPE_OFDM]);

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-30s = %s\n",
		"TX ext Power Stats",
		tpu->ext_pwr_lmt_en ? "Enable" : "Disable");

	tmp[PW_LMT_PH_1T] = pwr->ext_pwr[PW_LMT_PH_1T];
	tmp[PW_LMT_PH_2T] = pwr->ext_pwr[PW_LMT_PH_2T];
	(tmp[PW_LMT_PH_1T] < 0) ? (tmp[PW_LMT_PH_1T] = -1 * tmp[PW_LMT_PH_1T]) : (tmp[PW_LMT_PH_1T] = tmp[PW_LMT_PH_1T]);
	(tmp[PW_LMT_PH_2T] < 0) ? (tmp[PW_LMT_PH_2T] = -1 * tmp[PW_LMT_PH_2T]) : (tmp[PW_LMT_PH_2T] = tmp[PW_LMT_PH_2T]);
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-30s = %s%d.%02d / %s%d.%02d\n",
		"TX ext Power 1TX / 2TX",
		(pwr->ext_pwr[PW_LMT_PH_1T] < 0) ? "-" : "",
		tmp[PW_LMT_PH_1T] / 4,
		((tmp[PW_LMT_PH_1T] * 100) / 4) % 100,
		(pwr->ext_pwr[PW_LMT_PH_2T] < 0) ? "-" : "",
		tmp[PW_LMT_PH_2T] / 4,
		((tmp[PW_LMT_PH_2T] * 100) / 4) % 100
		);

	tmp1[RF_PATH_A] = pwr->ext_pwr_org[RF_PATH_A];
	tmp1[RF_PATH_B] = pwr->ext_pwr_org[RF_PATH_B];
	(tmp1[RF_PATH_A] < 0) ? (tmp1[RF_PATH_A] = -1 * tmp1[RF_PATH_A]) : (tmp1[RF_PATH_A] = tmp1[RF_PATH_A]);
	(tmp1[RF_PATH_B] < 0) ? (tmp1[RF_PATH_B] = -1 * tmp1[RF_PATH_B]) : (tmp1[RF_PATH_B] = tmp1[RF_PATH_B]);
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-30s = %s%d.%02d / %s%d.%02d\n",
		"TX ext Power diff A / B",
		(pwr->ext_pwr_org[RF_PATH_A] < 0) ? "-" : "",
		tmp1[RF_PATH_A] / 4,
		((tmp1[RF_PATH_A] * 100) / 4) % 100,
		(pwr->ext_pwr_org[RF_PATH_B] < 0) ? "-" : "",
		tmp1[RF_PATH_B] / 4,
		((tmp1[RF_PATH_B] * 100) / 4) % 100
		);

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-30s = %s\n",
		"6G Power limit",
		pwr->power_limit_6g_type == PW_LMT_6G_LOW ? "LOW" :
		(pwr->power_limit_6g_type == PW_LMT_6G_STD ? "STD" : "VLOW")
		);

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, "1TX\n");

	reg_tmp = halrf_mac_get_pwr_reg_8852c(rf, phy, 0xd2ec, 0xffffffff);
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-30s = %s%d.%d\n",
		"CCK 20M NOBF",
		halrf_pwr_is_minus(rf, reg_tmp & 0x7f) ? "-" : "",
		halrf_show_pwr_table(rf, reg_tmp & 0x7f) / 10,
		halrf_show_pwr_table(rf, reg_tmp & 0x7f) % 10);
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-30s = %s%d.%d\n",
		"CCK 40M NOBF",
		halrf_pwr_is_minus(rf, (reg_tmp & 0x7f0000) >> 16) ? "-" : "",
		halrf_show_pwr_table(rf, (reg_tmp & 0x7f0000) >> 16) / 10,
		halrf_show_pwr_table(rf, (reg_tmp & 0x7f0000) >> 16) % 10);

	reg_tmp = halrf_mac_get_pwr_reg_8852c(rf, phy, 0xd2f0, 0xffffffff);
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-30s = %s%d.%d\n",
		"OFDM NOBF",
		halrf_pwr_is_minus(rf, reg_tmp & 0x7f) ? "-" : "",
		halrf_show_pwr_table(rf, reg_tmp & 0x7f) / 10,
		halrf_show_pwr_table(rf, reg_tmp & 0x7f) % 10);
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-30s = %s%d.%d\n",
		"HT 20M NOBF",
		halrf_pwr_is_minus(rf, (reg_tmp & 0x7f0000) >> 16) ? "-" : "",
		halrf_show_pwr_table(rf, (reg_tmp & 0x7f0000) >> 16) / 10,
		halrf_show_pwr_table(rf, (reg_tmp & 0x7f0000) >> 16) % 10);

	reg_tmp = halrf_mac_get_pwr_reg_8852c(rf, phy, 0xd300, 0xffffffff);
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-30s = %s%d.%d\n",
		"HT 40M NOBF",
		halrf_pwr_is_minus(rf, (reg_tmp & 0x7f0000) >> 16) ? "-" : "",
		halrf_show_pwr_table(rf, (reg_tmp & 0x7f0000) >> 16) / 10,
		halrf_show_pwr_table(rf, (reg_tmp & 0x7f0000) >> 16) % 10);

	reg_tmp = halrf_mac_get_pwr_reg_8852c(rf, phy, 0xd308, 0xffffffff);
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-30s = %s%d.%d\n",
		"HT 80M NOBF",
		halrf_pwr_is_minus(rf, (reg_tmp & 0x7f0000) >> 16) ? "-" : "",
		halrf_show_pwr_table(rf, (reg_tmp & 0x7f0000) >> 16) / 10,
		halrf_show_pwr_table(rf, (reg_tmp & 0x7f0000) >> 16) % 10);

	reg_tmp = halrf_mac_get_pwr_reg_8852c(rf, phy, 0xd30c, 0xffffffff);
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-30s = %s%d.%d\n",
		"HT 160M NOBF",
		halrf_pwr_is_minus(rf, (reg_tmp & 0x7f0000) >> 16) ? "-" : "",
		halrf_show_pwr_table(rf, (reg_tmp & 0x7f0000) >> 16) / 10,
		halrf_show_pwr_table(rf, (reg_tmp & 0x7f0000) >> 16) % 10);

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, "2TX\n");

	reg_tmp = halrf_mac_get_pwr_reg_8852c(rf, phy, 0xd314, 0xffffffff);
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-30s = %s%d.%d\n",
		"CCK 20M NOBF",
		halrf_pwr_is_minus(rf, reg_tmp & 0x7f) ? "-" : "",
		halrf_show_pwr_table(rf, reg_tmp & 0x7f) / 10,
		halrf_show_pwr_table(rf, reg_tmp & 0x7f) % 10);
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-30s = %s%d.%d\n",
		"CCK 40M NOBF",
		halrf_pwr_is_minus(rf, (reg_tmp & 0x7f0000) >> 16) ? "-" : "",
		halrf_show_pwr_table(rf, (reg_tmp & 0x7f0000) >> 16) / 10,
		halrf_show_pwr_table(rf, (reg_tmp & 0x7f0000) >> 16) % 10);

	reg_tmp = halrf_mac_get_pwr_reg_8852c(rf, phy, 0xd318, 0xffffffff);
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-30s = %s%d.%d\n",
		"OFDM NOBF",
		halrf_pwr_is_minus(rf, reg_tmp & 0x7f) ? "-" : "",
		halrf_show_pwr_table(rf, reg_tmp & 0x7f) / 10,
		halrf_show_pwr_table(rf, reg_tmp & 0x7f) % 10);
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-30s = %s%d.%d / %s%d.%d\n",
		"HT 20M BF / NOBF",
		halrf_pwr_is_minus(rf, (reg_tmp & 0x7f000000) >> 24) ? "-" : "",
		halrf_show_pwr_table(rf, (reg_tmp & 0x7f000000) >> 24) / 10,
		halrf_show_pwr_table(rf, (reg_tmp & 0x7f000000) >> 24) % 10,
		halrf_pwr_is_minus(rf, (reg_tmp & 0x7f0000) >> 16) ? "-" : "",
		halrf_show_pwr_table(rf, (reg_tmp & 0x7f0000) >> 16) / 10,
		halrf_show_pwr_table(rf, (reg_tmp & 0x7f0000) >> 16) % 10);

	reg_tmp = halrf_mac_get_pwr_reg_8852c(rf, phy, 0xd328, 0xffffffff);
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-30s = %s%d.%d / %s%d.%d\n",
		"HT 40M BF / NOBF",
		halrf_pwr_is_minus(rf, (reg_tmp & 0x7f000000) >> 24) ? "-" : "",
		halrf_show_pwr_table(rf, (reg_tmp & 0x7f000000) >> 24) / 10,
		halrf_show_pwr_table(rf, (reg_tmp & 0x7f000000) >> 24) % 10,
		halrf_pwr_is_minus(rf, (reg_tmp & 0x7f0000) >> 16) ? "-" : "",
		halrf_show_pwr_table(rf, (reg_tmp & 0x7f0000) >> 16) / 10,
		halrf_show_pwr_table(rf, (reg_tmp & 0x7f0000) >> 16) % 10);

	reg_tmp = halrf_mac_get_pwr_reg_8852c(rf, phy, 0xd330, 0xffffffff);
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-30s = %s%d.%d / %s%d.%d\n",
		"HT 80M BF / NOBF",
		halrf_pwr_is_minus(rf, (reg_tmp & 0x7f000000) >> 24) ? "-" : "",
		halrf_show_pwr_table(rf, (reg_tmp & 0x7f000000) >> 24) / 10,
		halrf_show_pwr_table(rf, (reg_tmp & 0x7f000000) >> 24) % 10,
		halrf_pwr_is_minus(rf, (reg_tmp & 0x7f0000) >> 16) ? "-" : "",
		halrf_show_pwr_table(rf, (reg_tmp & 0x7f0000) >> 16) / 10,
		halrf_show_pwr_table(rf, (reg_tmp & 0x7f0000) >> 16) % 10);

	reg_tmp = halrf_mac_get_pwr_reg_8852c(rf, phy, 0xd334, 0xffffffff);
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-30s = %s%d.%d / %s%d.%d\n",
		"HT 160M BF / NOBF",
		halrf_pwr_is_minus(rf, (reg_tmp & 0x7f000000) >> 24) ? "-" : "",
		halrf_show_pwr_table(rf, (reg_tmp & 0x7f000000) >> 24) / 10,
		halrf_show_pwr_table(rf, (reg_tmp & 0x7f000000) >> 24) % 10,
		halrf_pwr_is_minus(rf, (reg_tmp & 0x7f0000) >> 16) ? "-" : "",
		halrf_show_pwr_table(rf, (reg_tmp & 0x7f0000) >> 16) / 10,
		halrf_show_pwr_table(rf, (reg_tmp & 0x7f0000) >> 16) % 10);

	*_used = used;
	*_out_len = out_len;

}

void halrf_pwr_limit_ru_info_8852c(struct rf_info *rf,
		char input[][16], u32 *_used, char *output, u32 *_out_len, enum phl_phy_idx phy)
{
	struct halrf_pwr_info *pwr = &rf->pwr_info;
	struct rtw_tpu_info *tpu = &rf->hal_com->band[phy].rtw_tpu_i;
	u8 channel = rf->hal_com->band[phy].cur_chandef.center_ch;
	u32 bw =  rf->hal_com->band[phy].cur_chandef.bw;
	u32 band = rf->hal_com->band[phy].cur_chandef.band;
	u32 reg_tmp, cck_ref, ofdm_ref;
	s32 s_cck_ref, s_ofdm_ref;
	s32 int_tmp[2], float_tmp[2];
	u32 power_constraint = pwr->power_constraint[phy];
	s32 tmp[2] = {0}, tmp1[2] = {0};

	u32 used = *_used;
	u32 out_len = *_out_len;

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-30s = 0x%x\n",
		 "RF Para Ver", halrf_get_radio_ver_from_reg(rf));

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-30s = %d / %d\n",
		 "RFE type / PHY", rf->phl_com->dev_cap.rfe_type, phy);

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-30s = %s / %d / %s\n",
		 "Band / CH / BW", band == BAND_ON_24G ? "2G" : (band == BAND_ON_5G ? "5G" : "6G"),
		 channel,
		 bw == 0 ? "20M" : (bw == 1 ? "40M" : (bw == 2 ? "80M" : "160M")));
	
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-30s = %s / %s / %s\n",
		 "Regulation 2G / 5G / 6G",
		 halrf_get_pw_lmt_regu_type_str_extra(rf, BAND_ON_24G),
		 halrf_get_pw_lmt_regu_type_str_extra(rf, BAND_ON_5G),
		 halrf_get_pw_lmt_regu_type_str_extra(rf, BAND_ON_6G));

	cck_ref = halrf_mac_get_pwr_reg_8852c(rf, phy, 0xd200, 0x0007fc00);
	ofdm_ref = halrf_mac_get_pwr_reg_8852c(rf, phy, 0xd200, 0x0ff80000);
	(cck_ref & BIT(8)) ? (s_cck_ref = cck_ref | 0xfffffe00) : (s_cck_ref = cck_ref);
	(ofdm_ref & BIT(8)) ? (s_ofdm_ref = ofdm_ref | 0xfffffe00) : (s_ofdm_ref = ofdm_ref);
	int_tmp[0] = s_cck_ref / 4;
	float_tmp[0] = s_cck_ref * 100 / 4 % 100;
	float_tmp[0] < 0 ? float_tmp[0] = float_tmp[0] * -1 : 0;
	int_tmp[1] = s_ofdm_ref / 4;
	float_tmp[1] = s_ofdm_ref * 100 / 4 % 100;
	float_tmp[1] < 0 ? float_tmp[1] = float_tmp[1] * -1 : 0;
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-30s = %s%d.%ddB / %s%d.%ddB\n",
		 "CCK REF / OFDM REF",
		 (int_tmp[0] == 0 && s_cck_ref < 0) ? "-" : "",
		 int_tmp[0], float_tmp[0],
		 (int_tmp[1] == 0 && s_ofdm_ref < 0) ? "-" : "",
		 int_tmp[1], float_tmp[1]);

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-30s = %d.%ddB\n",
		"Power Constraint",
		power_constraint / 4, (((power_constraint * 10) / 4) % 10));

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-30s = %s%d.%ddB\n",
		"DPK MCC Power Control",
		halrf_pwr_is_minus(rf, pwr->dpk_mcc_power / 2) ? "-" : "",
		halrf_show_pwr_table(rf, pwr->dpk_mcc_power / 2) / 10,
		halrf_show_pwr_table(rf, pwr->dpk_mcc_power / 2) % 10);

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-30s = %s\n",
		 "Power Limit (Reg)",
		 halrf_mac_get_pwr_reg_8852c(rf, phy, 0xd208, BIT(20)) ? "Enable Limit" : "Disable Limit");

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-30s = %s / %s\n",
		 "Driver Swith / Efuse Switch",
		 (pwr->pwr_limit_switch == PWLMT_DISABLE) ? "Disable Limit" :
		 (pwr->pwr_limit_switch == PWBYRATE_AND_PWLMT) ? "Enable Limit" : "From Efuse",
		 (pwr->pwr_limit_switch == PWLMT_BY_EFUSE) ?
		 ((pwr->pwr_table_switch_efuse == 0) ? "Disable Limit" : (pwr->pwr_table_switch_efuse == 2) ? "The Same" : "Enable Limit")
		 : "From Driver");

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-30s = %d / %d / %d\n",
		"TX Shape CCK / 2G / 5G",
		pwr->set_tx_ptrn_shap_idx[PW_LMT_BAND_2_4G][TX_SHAPE_CCK],
		pwr->set_tx_ptrn_shap_idx[PW_LMT_BAND_2_4G][TX_SHAPE_OFDM],
		pwr->set_tx_ptrn_shap_idx[PW_LMT_BAND_5G][TX_SHAPE_OFDM]);

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-30s = %s\n",
		"TX ext Power Stats",
		tpu->ext_pwr_lmt_en ? "Enable" : "Disable");

	tmp[PW_LMT_PH_1T] = pwr->ext_pwr[PW_LMT_PH_1T];
	tmp[PW_LMT_PH_2T] = pwr->ext_pwr[PW_LMT_PH_2T];
	(tmp[PW_LMT_PH_1T] < 0) ? (tmp[PW_LMT_PH_1T] = -1 * tmp[PW_LMT_PH_1T]) : (tmp[PW_LMT_PH_1T] = tmp[PW_LMT_PH_1T]);
	(tmp[PW_LMT_PH_2T] < 0) ? (tmp[PW_LMT_PH_2T] = -1 * tmp[PW_LMT_PH_2T]) : (tmp[PW_LMT_PH_2T] = tmp[PW_LMT_PH_2T]);
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-30s = %s%d.%02d / %s%d.%02d\n",
		"TX ext Power 1TX / 2TX",
		(pwr->ext_pwr[PW_LMT_PH_1T] < 0) ? "-" : "",
		tmp[PW_LMT_PH_1T] / 4,
		((tmp[PW_LMT_PH_1T] * 100) / 4) % 100,
		(pwr->ext_pwr[PW_LMT_PH_2T] < 0) ? "-" : "",
		tmp[PW_LMT_PH_2T] / 4,
		((tmp[PW_LMT_PH_2T] * 100) / 4) % 100
		);

	tmp1[RF_PATH_A] = pwr->ext_pwr_org[RF_PATH_A];
	tmp1[RF_PATH_B] = pwr->ext_pwr_org[RF_PATH_B];
	(tmp1[RF_PATH_A] < 0) ? (tmp1[RF_PATH_A] = -1 * tmp1[RF_PATH_A]) : (tmp1[RF_PATH_A] = tmp1[RF_PATH_A]);
	(tmp1[RF_PATH_B] < 0) ? (tmp1[RF_PATH_B] = -1 * tmp1[RF_PATH_B]) : (tmp1[RF_PATH_B] = tmp1[RF_PATH_B]);
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-30s = %s%d.%02d / %s%d.%02d\n",
		"TX ext Power diff A / B",
		(pwr->ext_pwr_org[RF_PATH_A] < 0) ? "-" : "",
		tmp1[RF_PATH_A] / 4,
		((tmp1[RF_PATH_A] * 100) / 4) % 100,
		(pwr->ext_pwr_org[RF_PATH_B] < 0) ? "-" : "",
		tmp1[RF_PATH_B] / 4,
		((tmp1[RF_PATH_B] * 100) / 4) % 100
		);

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-30s = %s\n",
		"6G Power limit",
		pwr->power_limit_6g_type == PW_LMT_6G_LOW ? "LOW" :
		(pwr->power_limit_6g_type == PW_LMT_6G_STD ? "STD" : "VLOW")
		);

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, "1TX\n");

	reg_tmp = halrf_mac_get_pwr_reg_8852c(rf, phy, 0xd33c, 0xffffffff);
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-30s = %s%d.%d\n",
		"RU26",
		halrf_pwr_is_minus(rf, reg_tmp & 0x7f) ? "-" : "",
		halrf_show_pwr_table(rf, reg_tmp & 0x7f) / 10,
		halrf_show_pwr_table(rf, reg_tmp & 0x7f) % 10);

	reg_tmp = halrf_mac_get_pwr_reg_8852c(rf, phy, 0xd344, 0xffffffff);
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-30s = %s%d.%d\n",
		"RU52",
		halrf_pwr_is_minus(rf, reg_tmp & 0x7f) ? "-" : "",
		halrf_show_pwr_table(rf, reg_tmp & 0x7f) / 10,
		halrf_show_pwr_table(rf, reg_tmp & 0x7f) % 10);

	reg_tmp = halrf_mac_get_pwr_reg_8852c(rf, phy, 0xd34c, 0xffffffff);
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-30s = %s%d.%d\n",
		"RU106",
		halrf_pwr_is_minus(rf, reg_tmp & 0x7f) ? "-" : "",
		halrf_show_pwr_table(rf, reg_tmp & 0x7f) / 10,
		halrf_show_pwr_table(rf, reg_tmp & 0x7f) % 10);

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, "2TX\n");

	reg_tmp = halrf_mac_get_pwr_reg_8852c(rf, phy, 0xd354, 0xffffffff);
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-30s = %s%d.%d\n",
		"RU26",
		halrf_pwr_is_minus(rf, reg_tmp & 0x7f) ? "-" : "",
		halrf_show_pwr_table(rf, reg_tmp & 0x7f) / 10,
		halrf_show_pwr_table(rf, reg_tmp & 0x7f) % 10);

	reg_tmp = halrf_mac_get_pwr_reg_8852c(rf, phy, 0xd35c, 0xffffffff);
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-30s = %s%d.%d\n",
		"RU52",
		halrf_pwr_is_minus(rf, reg_tmp & 0x7f) ? "-" : "",
		halrf_show_pwr_table(rf, reg_tmp & 0x7f) / 10,
		halrf_show_pwr_table(rf, reg_tmp & 0x7f) % 10);

	reg_tmp = halrf_mac_get_pwr_reg_8852c(rf, phy, 0xd364, 0xffffffff);
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-30s = %s%d.%d\n",
		"RU106",
		halrf_pwr_is_minus(rf, reg_tmp & 0x7f) ? "-" : "",
		halrf_show_pwr_table(rf, reg_tmp & 0x7f) / 10,
		halrf_show_pwr_table(rf, reg_tmp & 0x7f) % 10);

	*_used = used;
	*_out_len = out_len;

}

void halrf_set_tx_shape_8852c(struct rf_info *rf, u8 tx_shape_idx)
{
	struct rtw_tpu_info *tpu = &rf->hal_com->band[0].rtw_tpu_i;
	struct halrf_pwr_info *pwr = &rf->pwr_info;
	u8 ch = rf->hal_com->band[0].cur_chandef.center_ch;

	RF_DBG(rf, DBG_RF_POWER, "======>%s   ch=%d   tx_shape_idx=%d\n",
		__func__, ch, tx_shape_idx);

	tpu->tx_ptrn_shap_idx = tx_shape_idx;

	if (tx_shape_idx == 255) {
		pwr->set_tx_ptrn_shap_en = false;
		_halrf_set_tx_shape_8852c(rf, 0);
		return;
	} else
		pwr->set_tx_ptrn_shap_en = true;

	if (ch <= 14) {
		halrf_bb_set_pow_patten_sharp(rf, ch, true, tx_shape_idx, 0);	/*cck*/
		pwr->set_tx_ptrn_shap_idx[PW_LMT_BAND_2_4G][TX_SHAPE_CCK] = tx_shape_idx;
	}

	halrf_bb_set_pow_patten_sharp(rf, ch, false, tx_shape_idx, 0);	/*ofdm*/

	if (ch >= 1 && ch <= 14)
		pwr->set_tx_ptrn_shap_idx[PW_LMT_BAND_2_4G][TX_SHAPE_OFDM] = tx_shape_idx;
	else
		pwr->set_tx_ptrn_shap_idx[PW_LMT_BAND_5G][TX_SHAPE_OFDM] = tx_shape_idx;
}

void halrf_set_bw160m_diff_8852c(struct rf_info *rf, enum phl_phy_idx phy)
{
#if 0
	struct rtw_tpu_info *tpu = &rf->hal_com->band[phy].rtw_tpu_i;
	u32 band = rf->hal_com->band[phy].cur_chandef.band;
	s8 diff_bw160_bw40 = 0;

	if (band == BAND_ON_5G) {
		halrf_efuse_get_info(rf, EFUSE_INFO_RF_5G_BW160M_BW40M_DIFF, &diff_bw160_bw40, 1);
		if (diff_bw160_bw40 & BIT(3))
			diff_bw160_bw40 = diff_bw160_bw40 | 0xf0;
	} else if (band == BAND_ON_6G) {
		halrf_efuse_get_info(rf, EFUSE_INFO_RF_6G_BW160M_BW40M_DIFF, &diff_bw160_bw40, 1);
		if (diff_bw160_bw40 & BIT(3))
			diff_bw160_bw40 = diff_bw160_bw40 | 0xf0;
	} else
		diff_bw160_bw40 = 0;

	/*0~4: 80_80, 160, 80, 40, 20*/
	tpu->pwr_ofst_bw[1] = diff_bw160_bw40;

	RF_DBG(rf, DBG_RF_POWER, "======>%s   band=%d   diff_bw160_bw40=0x%x\n",
		__func__, band, diff_bw160_bw40);

	halrf_mac_write_pwr_ofst_bw(rf, phy);
#endif
}

#endif	/*RF_8852C_SUPPORT*/

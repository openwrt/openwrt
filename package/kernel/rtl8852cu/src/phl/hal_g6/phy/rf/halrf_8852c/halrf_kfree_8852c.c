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

void _halrf_get_total_efuse_8852c(struct rf_info *rf,
					enum phl_phy_idx phy)
{
	struct halrf_kfree_info *kfree = &rf->kfree_info;
	u32 i, array_size = sizeof(kfree->efuse_content);

	if (array_size < HIDE_EFUSE_SIZE_8852C) {
		RF_DBG(rf, DBG_RF_TSSI_TRIM, "======> %s   sizeof(kfree->efuse_content)(0x%x) < HIDE_EFUSE_SIZE_8852C(0x%x)\n",
			__func__, array_size, HIDE_EFUSE_SIZE_8852C);
		return;
	}

	halrf_phy_efuse_get_info(rf, HIDE_EFUSE_START_ADDR_8852C,
		HIDE_EFUSE_SIZE_8852C, kfree->efuse_content);

	for (i = 0; i < HIDE_EFUSE_SIZE_8852C; i++)
		RF_DBG(rf, DBG_RF_TSSI_TRIM, "======> %s   efuse_con[0x%x] = 0x%x\n",
			__func__, i + HIDE_EFUSE_START_ADDR_8852C, kfree->efuse_content[i]);
}

u8 _halrf_get_1byte_efuse_8852c(struct rf_info *rf, u32 addr, u8 *value)
{
	struct halrf_kfree_info *kfree = &rf->kfree_info;

	if (addr < HIDE_EFUSE_START_ADDR_8852C || addr > HIDE_EFUSE_END_ADDR_8852C) {
		RF_DBG(rf, DBG_RF_TSSI_TRIM, "===> %s addr(0x%x) < 0x%x(Min), > 0x%x(Max) Over Range Return !!!\n",
			__func__, addr, HIDE_EFUSE_START_ADDR_8852C, HIDE_EFUSE_END_ADDR_8852C);
		return 0xff;
	}

	*value = kfree->efuse_content[addr - HIDE_EFUSE_START_ADDR_8852C];

	return *value;
}

s8 _halrf_efuse_exchange_8852c(struct rf_info *rf, u8 value, u8 mask)
{
	s8 tmp = 0;

	if (mask == LOW_MASK) {
		tmp = value & 0xf;

		if (tmp & BIT(3))
			tmp = tmp | 0xf0;
	} else {
		tmp = (value & 0xf0) >> 4;

		if (tmp & BIT(3))
			tmp = tmp | 0xf0;
	}

	return tmp;
}

void _halrf_set_thermal_trim_8852c(struct rf_info *rf,
					enum phl_phy_idx phy)
{
	u8 thermal_a = 0xff, thermal_b = 0xff, tmp = 0;

	RF_DBG(rf, DBG_RF_THER_TRIM, "======> %s   phy=%d\n", __func__, phy);

	if (!(rf->support_ability & HAL_RF_THER_TRIM)) {
		RF_DBG(rf, DBG_RF_THER_TRIM, "<== %s phy=%d support_ability=0x%x Ther Trim Off!!!\n",
			__func__, phy, rf->support_ability);
		return;
	}

	tmp = _halrf_get_1byte_efuse_8852c(rf, THERMAL_TRIM_HIDE_EFUSE_A_8852C, &thermal_a);

	tmp = _halrf_get_1byte_efuse_8852c(rf, THERMAL_TRIM_HIDE_EFUSE_B_8852C, &thermal_b);

	RF_DBG(rf, DBG_RF_THER_TRIM, "efuse Ther_A=0x%x Ther_B=0x%x\n",
		thermal_a, thermal_b);

	if (thermal_a == 0xff && thermal_b == 0xff) {
		RF_DBG(rf, DBG_RF_THER_TRIM, "Ther_A, Ther_B=0xff no PG Return!!!\n");
		return; 
	}

	thermal_a = thermal_a & 0xf;
	thermal_a = ((thermal_a & 0x1) << 3) | (thermal_a >> 1);

	thermal_b = thermal_b & 0xf;
	thermal_b = ((thermal_b & 0x1) << 3) | (thermal_b >> 1);

	RF_DBG(rf, DBG_RF_THER_TRIM, "After Exchange Ther_A=0x%x Ther_B=0x%x\n",
		thermal_a, thermal_b);

	halrf_wrf(rf, RF_PATH_A, 0x43, 0x000f0000, thermal_a & 0xf);
	halrf_wrf(rf, RF_PATH_B, 0x43, 0x000f0000, thermal_b & 0xf);
}

void _halrf_set_pa_bias_trim_8852c(struct rf_info *rf,
					enum phl_phy_idx phy)
{
	u8 pa_bias_a = 0xff, pa_bias_b = 0xff, tmp = 0;
	u8 pa_bias_a_2g, pa_bias_b_2g, pa_bias_a_5g, pa_bias_b_5g;

	RF_DBG(rf, DBG_RF_PABIAS_TRIM, "======> %s   phy=%d\n", __func__, phy);

	if (!(rf->support_ability & HAL_RF_PABIAS_TRIM)) {
		RF_DBG(rf, DBG_RF_PABIAS_TRIM, "<== %s phy=%d support_ability=0x%x PA Bias K Off!!!\n",
			__func__, phy, rf->support_ability);
		return;
	}

	tmp = _halrf_get_1byte_efuse_8852c(rf, PABIAS_TRIM_HIDE_EFUSE_A_8852C, &pa_bias_a);

	tmp = _halrf_get_1byte_efuse_8852c(rf, PABIAS_TRIM_HIDE_EFUSE_B_8852C, &pa_bias_b);

	RF_DBG(rf, DBG_RF_PABIAS_TRIM, "efuse PA_Bias_A=0x%x PA_Bias_B=0x%x\n",
		pa_bias_a, pa_bias_b);

	if (pa_bias_a == 0xff && pa_bias_b == 0xff) {
		RF_DBG(rf, DBG_RF_PABIAS_TRIM, "PA_Bias_A, PA_Bias_B=0xff no PG Return!!!\n");
		return; 
	}

	pa_bias_a_2g = pa_bias_a & 0xf;
	pa_bias_a_5g = (pa_bias_a & 0xf0) >> 4;

	pa_bias_b_2g = pa_bias_b & 0xf;
	pa_bias_b_5g = (pa_bias_b & 0xf0) >> 4;

	RF_DBG(rf, DBG_RF_PABIAS_TRIM, "After Calculate PA_Bias_A_2G=0x%x PA_Bias_A_5G=0x%x\n",
		pa_bias_a_2g, pa_bias_a_5g);

	RF_DBG(rf, DBG_RF_PABIAS_TRIM, "After Calculate PA_Bias_B_2G=0x%x PA_Bias_B_5G=0x%x\n",
		pa_bias_b_2g, pa_bias_b_5g);

	halrf_wrf(rf, RF_PATH_A, 0x60, 0x0000f000, pa_bias_a_2g);
	halrf_wrf(rf, RF_PATH_A, 0x60, 0x000f0000, pa_bias_a_5g);

	halrf_wrf(rf, RF_PATH_B, 0x60, 0x0000f000, pa_bias_b_2g);
	halrf_wrf(rf, RF_PATH_B, 0x60, 0x000f0000, pa_bias_b_5g);
}

void _halrf_get_tssi_trim_8852c(struct rf_info *rf,
					enum phl_phy_idx phy)
{
	struct halrf_tssi_info *tssi = &rf->tssi;
	u8 i, j , check_tmp = 0, check_tmp_6g = 0;

	RF_DBG(rf, DBG_RF_TSSI_TRIM, "======> %s   phy=%d\n", __func__, phy);

	if (!(rf->support_ability & HAL_RF_TSSI_TRIM)) {
		RF_DBG(rf, DBG_RF_TSSI_TRIM, "<== %s phy=%d support_ability=0x%x TSSI Trim Off!!!\n",
			__func__, phy, rf->support_ability);
		return;
	}

	_halrf_get_1byte_efuse_8852c(rf, TSSI_TRIM_HIDE_EFUSE_2GL_A_8852C,
		(u8 *)&tssi->tssi_trim[RF_PATH_A][0]);
	_halrf_get_1byte_efuse_8852c(rf, TSSI_TRIM_HIDE_EFUSE_2GH_A_8852C,
		(u8 *)&tssi->tssi_trim[RF_PATH_A][1]);

	_halrf_get_1byte_efuse_8852c(rf, TSSI_TRIM_HIDE_EFUSE_5GL1_A_8852C,
		(u8 *)&tssi->tssi_trim[RF_PATH_A][2]);
	_halrf_get_1byte_efuse_8852c(rf, TSSI_TRIM_HIDE_EFUSE_5GL2_A_8852C,
		(u8 *)&tssi->tssi_trim[RF_PATH_A][3]);
	_halrf_get_1byte_efuse_8852c(rf, TSSI_TRIM_HIDE_EFUSE_5GM1_A_8852C,
		(u8 *)&tssi->tssi_trim[RF_PATH_A][4]);
	_halrf_get_1byte_efuse_8852c(rf, TSSI_TRIM_HIDE_EFUSE_5GM2_A_8852C,
		(u8 *)&tssi->tssi_trim[RF_PATH_A][5]);
	_halrf_get_1byte_efuse_8852c(rf, TSSI_TRIM_HIDE_EFUSE_5GH1_A_8852C,
		(u8 *)&tssi->tssi_trim[RF_PATH_A][6]);
	_halrf_get_1byte_efuse_8852c(rf, TSSI_TRIM_HIDE_EFUSE_5GH2_A_8852C,
		(u8 *)&tssi->tssi_trim[RF_PATH_A][7]);

	_halrf_get_1byte_efuse_8852c(rf, TSSI_TRIM_HIDE_EFUSE_2GL_B_8852C,
		(u8 *)&tssi->tssi_trim[RF_PATH_B][0]);
	_halrf_get_1byte_efuse_8852c(rf, TSSI_TRIM_HIDE_EFUSE_2GH_B_8852C,
		(u8 *)&tssi->tssi_trim[RF_PATH_B][1]);

	_halrf_get_1byte_efuse_8852c(rf, TSSI_TRIM_HIDE_EFUSE_5GL1_B_8852C,
		(u8 *)&tssi->tssi_trim[RF_PATH_B][2]);
	_halrf_get_1byte_efuse_8852c(rf, TSSI_TRIM_HIDE_EFUSE_5GL2_B_8852C,
		(u8 *)&tssi->tssi_trim[RF_PATH_B][3]);
	_halrf_get_1byte_efuse_8852c(rf, TSSI_TRIM_HIDE_EFUSE_5GM1_B_8852C,
		(u8 *)&tssi->tssi_trim[RF_PATH_B][4]);
	_halrf_get_1byte_efuse_8852c(rf, TSSI_TRIM_HIDE_EFUSE_5GM2_B_8852C,
		(u8 *)&tssi->tssi_trim[RF_PATH_B][5]);
	_halrf_get_1byte_efuse_8852c(rf, TSSI_TRIM_HIDE_EFUSE_5GH1_B_8852C,
		(u8 *)&tssi->tssi_trim[RF_PATH_B][6]);
	_halrf_get_1byte_efuse_8852c(rf, TSSI_TRIM_HIDE_EFUSE_5GH2_B_8852C,
		(u8 *)&tssi->tssi_trim[RF_PATH_B][7]);

	_halrf_get_1byte_efuse_8852c(rf, TSSI_TRIM_HIDE_EFUSE_6GL1_A_8852C,
		(u8 *)&tssi->tssi_trim_6g[RF_PATH_A][0]);
	_halrf_get_1byte_efuse_8852c(rf, TSSI_TRIM_HIDE_EFUSE_6GL2_A_8852C,
		(u8 *)&tssi->tssi_trim_6g[RF_PATH_A][1]);
	_halrf_get_1byte_efuse_8852c(rf, TSSI_TRIM_HIDE_EFUSE_6GL3_A_8852C,
		(u8 *)&tssi->tssi_trim_6g[RF_PATH_A][2]);
	_halrf_get_1byte_efuse_8852c(rf, TSSI_TRIM_HIDE_EFUSE_6GL4_A_8852C,
		(u8 *)&tssi->tssi_trim_6g[RF_PATH_A][3]);
	_halrf_get_1byte_efuse_8852c(rf, TSSI_TRIM_HIDE_EFUSE_6GM1_A_8852C,
		(u8 *)&tssi->tssi_trim_6g[RF_PATH_A][4]);
	_halrf_get_1byte_efuse_8852c(rf, TSSI_TRIM_HIDE_EFUSE_6GM2_A_8852C,
		(u8 *)&tssi->tssi_trim_6g[RF_PATH_A][5]);
	_halrf_get_1byte_efuse_8852c(rf, TSSI_TRIM_HIDE_EFUSE_6GM3_A_8852C,
		(u8 *)&tssi->tssi_trim_6g[RF_PATH_A][6]);
	_halrf_get_1byte_efuse_8852c(rf, TSSI_TRIM_HIDE_EFUSE_6GM4_A_8852C,
		(u8 *)&tssi->tssi_trim_6g[RF_PATH_A][7]);
	_halrf_get_1byte_efuse_8852c(rf, TSSI_TRIM_HIDE_EFUSE_6GH1_A_8852C,
		(u8 *)&tssi->tssi_trim_6g[RF_PATH_A][8]);
	_halrf_get_1byte_efuse_8852c(rf, TSSI_TRIM_HIDE_EFUSE_6GH2_A_8852C,
		(u8 *)&tssi->tssi_trim_6g[RF_PATH_A][9]);
	_halrf_get_1byte_efuse_8852c(rf, TSSI_TRIM_HIDE_EFUSE_6GH3_A_8852C,
		(u8 *)&tssi->tssi_trim_6g[RF_PATH_A][10]);
	_halrf_get_1byte_efuse_8852c(rf, TSSI_TRIM_HIDE_EFUSE_6GH4_A_8852C,
		(u8 *)&tssi->tssi_trim_6g[RF_PATH_A][11]);
	_halrf_get_1byte_efuse_8852c(rf, TSSI_TRIM_HIDE_EFUSE_6GUH1_A_8852C,
		(u8 *)&tssi->tssi_trim_6g[RF_PATH_A][12]);
	_halrf_get_1byte_efuse_8852c(rf, TSSI_TRIM_HIDE_EFUSE_6GUH2_A_8852C,
		(u8 *)&tssi->tssi_trim_6g[RF_PATH_A][13]);
	_halrf_get_1byte_efuse_8852c(rf, TSSI_TRIM_HIDE_EFUSE_6GUH3_A_8852C,
		(u8 *)&tssi->tssi_trim_6g[RF_PATH_A][14]);
	_halrf_get_1byte_efuse_8852c(rf, TSSI_TRIM_HIDE_EFUSE_6GUH4_A_8852C,
		(u8 *)&tssi->tssi_trim_6g[RF_PATH_A][15]);

	_halrf_get_1byte_efuse_8852c(rf, TSSI_TRIM_HIDE_EFUSE_6GL1_B_8852C,
		(u8 *)&tssi->tssi_trim_6g[RF_PATH_B][0]);
	_halrf_get_1byte_efuse_8852c(rf, TSSI_TRIM_HIDE_EFUSE_6GL2_B_8852C,
		(u8 *)&tssi->tssi_trim_6g[RF_PATH_B][1]);
	_halrf_get_1byte_efuse_8852c(rf, TSSI_TRIM_HIDE_EFUSE_6GL3_B_8852C,
		(u8 *)&tssi->tssi_trim_6g[RF_PATH_B][2]);
	_halrf_get_1byte_efuse_8852c(rf, TSSI_TRIM_HIDE_EFUSE_6GL4_B_8852C,
		(u8 *)&tssi->tssi_trim_6g[RF_PATH_B][3]);
	_halrf_get_1byte_efuse_8852c(rf, TSSI_TRIM_HIDE_EFUSE_6GM1_B_8852C,
		(u8 *)&tssi->tssi_trim_6g[RF_PATH_B][4]);
	_halrf_get_1byte_efuse_8852c(rf, TSSI_TRIM_HIDE_EFUSE_6GM2_B_8852C,
		(u8 *)&tssi->tssi_trim_6g[RF_PATH_B][5]);
	_halrf_get_1byte_efuse_8852c(rf, TSSI_TRIM_HIDE_EFUSE_6GM3_B_8852C,
		(u8 *)&tssi->tssi_trim_6g[RF_PATH_B][6]);
	_halrf_get_1byte_efuse_8852c(rf, TSSI_TRIM_HIDE_EFUSE_6GM4_B_8852C,
		(u8 *)&tssi->tssi_trim_6g[RF_PATH_B][7]);
	_halrf_get_1byte_efuse_8852c(rf, TSSI_TRIM_HIDE_EFUSE_6GH1_B_8852C,
		(u8 *)&tssi->tssi_trim_6g[RF_PATH_B][8]);
	_halrf_get_1byte_efuse_8852c(rf, TSSI_TRIM_HIDE_EFUSE_6GH2_B_8852C,
		(u8 *)&tssi->tssi_trim_6g[RF_PATH_B][9]);
	_halrf_get_1byte_efuse_8852c(rf, TSSI_TRIM_HIDE_EFUSE_6GH3_B_8852C,
		(u8 *)&tssi->tssi_trim_6g[RF_PATH_B][10]);
	_halrf_get_1byte_efuse_8852c(rf, TSSI_TRIM_HIDE_EFUSE_6GH4_B_8852C,
		(u8 *)&tssi->tssi_trim_6g[RF_PATH_B][11]);
	_halrf_get_1byte_efuse_8852c(rf, TSSI_TRIM_HIDE_EFUSE_6GUH1_B_8852C,
		(u8 *)&tssi->tssi_trim_6g[RF_PATH_B][12]);
	_halrf_get_1byte_efuse_8852c(rf, TSSI_TRIM_HIDE_EFUSE_6GUH2_B_8852C,
		(u8 *)&tssi->tssi_trim_6g[RF_PATH_B][13]);
	_halrf_get_1byte_efuse_8852c(rf, TSSI_TRIM_HIDE_EFUSE_6GUH3_B_8852C,
		(u8 *)&tssi->tssi_trim_6g[RF_PATH_B][14]);
	_halrf_get_1byte_efuse_8852c(rf, TSSI_TRIM_HIDE_EFUSE_6GUH4_B_8852C,
		(u8 *)&tssi->tssi_trim_6g[RF_PATH_B][15]);

	/*TSSI Trim 2G, 5G*/
	for (i = 0; i < 2; i++) {
		for (j = 0; j < TSSI_HIDE_EFUSE_NUM; j++) {
			RF_DBG(rf, DBG_RF_TSSI_TRIM, "2G/5G tssi->tssi_trim[%d][%d]=0x%x\n", i, j, tssi->tssi_trim[i][j]);
			if ((tssi->tssi_trim[i][j] & 0xff) == 0xff)
				check_tmp++;
		}
	}

	RF_DBG(rf, DBG_RF_TSSI_TRIM, "2G/5G check_tmp=%d\n", check_tmp);

	if (check_tmp == 2 * TSSI_HIDE_EFUSE_NUM) {
		for (i = 0; i < 2; i++) {
			for (j = 0; j < TSSI_HIDE_EFUSE_NUM; j++)
				tssi->tssi_trim[i][j] = 0;
		}

		RF_DBG(rf, DBG_RF_TSSI_TRIM, "2G/5G TSSI Trim no PG tssi->tssi_trim=0x0\n");
	}

	/*TSSI Trim 6G*/
	for (i = 0; i < 2; i++) {
		for (j = 0; j < TSSI_HIDE_EFUSE_NUM_6G; j++) {
			RF_DBG(rf, DBG_RF_TSSI_TRIM, "6G tssi->tssi_trim_6g[%d][%d]=0x%x\n", i, j, tssi->tssi_trim_6g[i][j]);
			if ((tssi->tssi_trim_6g[i][j] & 0xff) == 0xff)
				check_tmp_6g++;
		}
	}

	RF_DBG(rf, DBG_RF_TSSI_TRIM, "6G check_tmp_6g=%d\n", check_tmp_6g);

	if (check_tmp_6g == 2 * TSSI_HIDE_EFUSE_NUM_6G) {
		for (i = 0; i < 2; i++) {
			for (j = 0; j < TSSI_HIDE_EFUSE_NUM_6G; j++)
				tssi->tssi_trim_6g[i][j] = 0;
		}

		RF_DBG(rf, DBG_RF_TSSI_TRIM, "6G TSSI Trim no PG tssi->tssi_trim_6g=0x0\n");
	}
}

void halrf_get_efuse_trim_8852c(struct rf_info *rf,
					enum phl_phy_idx phy)
{
	_halrf_get_total_efuse_8852c(rf, phy);
	_halrf_set_thermal_trim_8852c(rf, phy);
	_halrf_set_pa_bias_trim_8852c(rf, phy);
	_halrf_get_tssi_trim_8852c(rf, phy);
}

void halrf_kfree_get_info_8852c(struct rf_info *rf, char input[][16], u32 *_used,
			 char *output, u32 *_out_len)
{
	u8 tmp;

	RF_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		 "\n=====================[ KFREE info ]=====================\n");

	RF_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		" %-30s : 0x%X = 0x%X\n",
		"Thermal A Efuse",
		THERMAL_TRIM_HIDE_EFUSE_A_8852C,
		_halrf_get_1byte_efuse_8852c(rf, THERMAL_TRIM_HIDE_EFUSE_A_8852C, &tmp));

	RF_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		" %-30s : %s = 0x%X\n",
		"Thermal A RFC",
		"0x43[19:16]",
		halrf_rrf(rf, RF_PATH_A, 0x43, 0x000f0000));

	RF_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		" %-30s : 0x%X = 0x%X\n",
		"Thermal B Efuse",
		THERMAL_TRIM_HIDE_EFUSE_B_8852C,
		_halrf_get_1byte_efuse_8852c(rf, THERMAL_TRIM_HIDE_EFUSE_B_8852C, &tmp));

	RF_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		" %-30s : %s = 0x%X\n",
		"Thermal B RFC",
		"0x43[19:16]",
		halrf_rrf(rf, RF_PATH_B, 0x43, 0x000f0000));

	RF_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		" %-30s : 0x%X = 0x%X\n",
		"PABias 2G A Efuse",
		PABIAS_TRIM_HIDE_EFUSE_A_8852C,
		_halrf_get_1byte_efuse_8852c(rf, PABIAS_TRIM_HIDE_EFUSE_A_8852C, &tmp));

	RF_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		" %-30s : %s = 0x%X\n",
		"PABias 2G A RFC",
		"0x60[15:12]",
		halrf_rrf(rf, RF_PATH_A, 0x60, 0x0000f000));

	RF_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		" %-30s : 0x%X = 0x%X\n",
		"PABias 2G B Efuse",
		PABIAS_TRIM_HIDE_EFUSE_B_8852C,
		_halrf_get_1byte_efuse_8852c(rf, PABIAS_TRIM_HIDE_EFUSE_B_8852C, &tmp));

	RF_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		" %-30s : %s = 0x%X\n",
		"PABias 2G B RFC",
		"0x60[15:12]",
		halrf_rrf(rf, RF_PATH_B, 0x60, 0x0000f000));

	RF_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		" %-30s : 0x%X = 0x%X\n",
		"PABias 5G A Efuse",
		PABIAS_TRIM_HIDE_EFUSE_A_8852C,
		_halrf_get_1byte_efuse_8852c(rf, PABIAS_TRIM_HIDE_EFUSE_A_8852C, &tmp));

	RF_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		" %-30s : %s = 0x%X\n",
		"PABias 5G A RFC",
		"0x60[19:16]",
		halrf_rrf(rf, RF_PATH_A, 0x60, 0x000f0000));

	RF_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		" %-30s : 0x%X = 0x%X\n",
		"PABias 5G B Efuse",
		PABIAS_TRIM_HIDE_EFUSE_B_8852C,
		_halrf_get_1byte_efuse_8852c(rf, PABIAS_TRIM_HIDE_EFUSE_B_8852C, &tmp));

	RF_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		" %-30s : %s = 0x%X\n",
		"PABias 5G B RFC",
		"0x60[19:16]",
		halrf_rrf(rf, RF_PATH_B, 0x60, 0x000f0000));
}

#endif	/*RF_8852C_SUPPORT*/

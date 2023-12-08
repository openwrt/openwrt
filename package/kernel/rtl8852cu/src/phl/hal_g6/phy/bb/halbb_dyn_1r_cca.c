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
#include "halbb_precomp.h"

#ifdef HALBB_DYN_1R_CCA_SUPPORT

void halbb_1r_cca_cr_cfg(struct bb_info *bb, enum rf_path rx_path)
{
	struct bb_dyn_1r_cca_info *dyn_1r_cca = &bb->bb_dyn_1r_cca_i;

	BB_DBG(bb, DBG_IC_API, "[%s]rx_path=%d\n", __func__, rx_path);

	switch (bb->ic_type) {

#ifdef BB_8852A_2_SUPPORT
	case BB_RTL8852A:
		halbb_1r_cca_cr_cfg_8852a_2(bb, rx_path);
		break;
#endif
	default:
		break;
	}
}

void halbb_dyn_1r_cca_cfg(struct bb_info *bb, enum rf_path rx_path)
{
	struct bb_dyn_1r_cca_info *dyn_1r_cca = &bb->bb_dyn_1r_cca_i;

	BB_DBG(bb, DBG_IC_API, "[%s]rx_path=%d\n", __func__, rx_path);

	if (dyn_1r_cca->dyn_1r_cca_cfg == rx_path)
		return;

	dyn_1r_cca->dyn_1r_cca_cfg = rx_path;

	halbb_1r_cca_cr_cfg(bb, rx_path);
}

void halbb_dyn_1r_cca(struct bb_info *bb)
{
	struct bb_dyn_1r_cca_info *dyn_1r_cca = &bb->bb_dyn_1r_cca_i;
	struct bb_link_info *bb_link = &bb->bb_link_i;
	struct rtw_phl_stainfo_t *phl_sta;
	struct rtw_rssi_info *rssi_t = NULL;
	enum rf_path cfg = RF_PATH_AB;
	bool sta_found = false;
	u16 rssi_diff, rssi_min; /*RSSI  u(16,5)*/
	u8 i, j;

	if (!dyn_1r_cca->dyn_1r_cca_en)
		return;

	BB_DBG(bb, DBG_IC_API, "[%s]\n", __func__);

	if (!bb_link->is_linked || !bb_link->is_one_entry_only) {
		BB_DBG(bb, DBG_IC_API, "is_linked=%d, one_entry_only=%d\n",
		       bb_link->is_linked, bb_link->is_one_entry_only);

		halbb_dyn_1r_cca_cfg(bb, RF_PATH_AB);
		return;
	}

	for (i = 0; i < PHL_MAX_STA_NUM; i++) {
		if (!bb->sta_exist[i])
			continue;

		phl_sta = bb->phl_sta_info[i];

		if (!is_sta_active(phl_sta))
			continue;

		rssi_t = &phl_sta->hal_sta->rssi_stat;

		for (j = 0; j < HALBB_MAX_PATH; j++) {
			halbb_print_sign_frac_digit(bb, rssi_t->rssi_ma_path[j], 16, 5, bb->dbg_buf, HALBB_SNPRINT_SIZE_S);
			BB_DBG(bb, DBG_IC_API, "[id:%d]rssi[%d]= %s\n",
			       i, j, bb->dbg_buf);
		}
		sta_found = true;
		break;
	}

	if (!sta_found)
		return;

	if (rssi_t->rssi_ma_path[0] >= rssi_t->rssi_ma_path[1]) {
		cfg = RF_PATH_A;
		rssi_diff = rssi_t->rssi_ma_path[0] - rssi_t->rssi_ma_path[1];
		rssi_min = rssi_t->rssi_ma_path[1];
	} else {
		cfg = RF_PATH_B;
		rssi_diff = rssi_t->rssi_ma_path[1] - rssi_t->rssi_ma_path[0];
		rssi_min = rssi_t->rssi_ma_path[0];
	}

	BB_DBG(bb, DBG_IC_API, "[Dyn 1R CCA]en=%d, diff_th=%d, min_th=%d\n",
	       dyn_1r_cca->dyn_1r_cca_en,
	       dyn_1r_cca->dyn_1r_cca_rssi_diff_th >> 5,
	       dyn_1r_cca->dyn_1r_cca_rssi_min_th >> 5);

	if (rssi_diff < dyn_1r_cca->dyn_1r_cca_rssi_diff_th &&
	    rssi_min > dyn_1r_cca->dyn_1r_cca_rssi_min_th) {
		BB_DBG(bb, DBG_IC_API, "[1R_CCA] path=%s\n", (cfg == RF_PATH_A) ? "A" : "B");
	} else {
		BB_DBG(bb, DBG_IC_API, "[2R_CCA]\n");
		cfg = RF_PATH_AB;
	}

	halbb_dyn_1r_cca_cfg(bb, cfg);
}

void halbb_dyn_1r_cca_rst(struct bb_info *bb)
{
	struct bb_dyn_1r_cca_info *dyn_1r_cca = &bb->bb_dyn_1r_cca_i;

	dyn_1r_cca->dyn_1r_cca_cfg = RF_PATH_ABCD;
}

void halbb_dyn_1r_cca_init(struct bb_info *bb)
{
	struct bb_dyn_1r_cca_info *dyn_1r_cca = &bb->bb_dyn_1r_cca_i;

	BB_DBG(bb, DBG_IC_API, "[%s]\n", __func__);

	dyn_1r_cca->dyn_1r_cca_en = false;
	dyn_1r_cca->dyn_1r_cca_cfg = RF_PATH_ABCD;
	dyn_1r_cca->dyn_1r_cca_rssi_diff_th= 10 << 5;
	dyn_1r_cca->dyn_1r_cca_rssi_min_th= 40 << 5;
}

void halbb_dyn_1r_cca_en(struct bb_info *bb, bool en)
{
	struct bb_dyn_1r_cca_info *dyn_1r_cca = &bb->bb_dyn_1r_cca_i;

	dyn_1r_cca->dyn_1r_cca_en = en;
}

void halbb_dyn_1r_cca_dbg(struct bb_info *bb, char input[][16], 
			  u32 *_used, char *output, u32 *_out_len)
{
	struct bb_dyn_1r_cca_info *dyn_1r_cca = &bb->bb_dyn_1r_cca_i;
	u32 val[5] = {0};
	enum rf_path cca_path;

	if (_os_strcmp(input[1], "-h") == 0) {
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			 "en {0/1}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			 "force {1:A, 2:B, 3:AB}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			 "diff_th {val}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			 "min_th {val}\n");

		return;
	}

	HALBB_SCAN(input[2], DCMD_DECIMAL, &val[0]);
	if (_os_strcmp(input[1], "en") == 0) {
		halbb_dyn_1r_cca_en(bb, (bool)val[0]);
	} else if (_os_strcmp(input[1], "force") == 0) {
		halbb_dyn_1r_cca_en(bb, false);

		if (val[0] == 1)
			cca_path = RF_PATH_A;
		else if (val[0] == 2)
			cca_path = RF_PATH_B;
		else if (val[0] == 3)
			cca_path = RF_PATH_AB;
		else
			return;
		halbb_dyn_1r_cca_cfg(bb, cca_path);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "Force cca_path=%d\n", cca_path);
	} else if (_os_strcmp(input[1], "diff_th") == 0) {
		dyn_1r_cca->dyn_1r_cca_rssi_diff_th= (u16)(val[0] << 5);
	} else if (_os_strcmp(input[1], "min_th") == 0) {
		dyn_1r_cca->dyn_1r_cca_rssi_min_th= (u16)(val[0] << 5);
	} else {
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			"Set Err\n");
	}

	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "[Dyn 1R CCA]en=%d, diff_th=%d, min_th=%d\n",
		    dyn_1r_cca->dyn_1r_cca_en,
		    dyn_1r_cca->dyn_1r_cca_rssi_diff_th >> 5,
		    dyn_1r_cca->dyn_1r_cca_rssi_min_th >> 5);
}

#endif

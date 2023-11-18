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

#ifdef HALBB_UL_TB_CTRL_SUPPORT
void halbb_ul_tb_reset(struct bb_info *bb)
{
	struct bb_ul_tb_info *bb_ul_tb = &bb->bb_ul_tb_i;
	struct bb_ul_tb_cr_info *cr = &bb->bb_ul_tb_i.bb_ul_tb_cr_i;
	struct rtw_tpu_info *tpu = &bb->hal_com->band[bb->bb_phy_idx].rtw_tpu_i;
	struct bb_api_info *bb_api = &bb->bb_api_i;
	bool is_ofdm = true;

	BB_DBG(bb, DBG_UL_TB_CTRL, "%s ======>\n", __func__);
	halbb_set_reg(bb, cr->if_bandedge, cr->if_bandedge_m, bb_ul_tb->def_if_bandedge);
	tpu->tx_ptrn_shap_idx = bb_ul_tb->def_tri_idx;
	halbb_set_tx_pow_pattern_shap(bb, bb_api->central_ch, is_ofdm, bb->bb_phy_idx);
}

void halbb_ul_tb_chk(struct bb_info *bb)
{
	struct bb_ul_tb_info *bb_ul_tb = &bb->bb_ul_tb_i;
	struct bb_ul_tb_cr_info *cr = &bb->bb_ul_tb_i.bb_ul_tb_cr_i;
	struct bb_api_info *bb_api = &bb->bb_api_i;
	struct rtw_tpu_info *tpu = &bb->hal_com->band[bb->bb_phy_idx].rtw_tpu_i;

	BB_DBG(bb, DBG_UL_TB_CTRL, "%s ======>\n", __func__);

	bb_ul_tb->def_if_bandedge = (u8)halbb_get_reg(bb, cr->if_bandedge, cr->if_bandedge_m);
	bb_ul_tb->def_tri_idx = tpu->tx_ptrn_shap_idx;

	BB_DBG(bb, DBG_UL_TB_CTRL, "band = %d, bw = %d\n", bb_api->band, 20 << bb_api->bw);
	if (bb_api->band >= BAND_ON_5G && bb_api->bw >= CHANNEL_WIDTH_40)
		bb_ul_tb->dyn_tb_bedge_en = true;
	else
		bb_ul_tb->dyn_tb_bedge_en = false;

	if (bb->ic_type == BB_RTL8852B && bb->hal_com->cv > CBV) {
		bb_ul_tb->dyn_tb_bedge_en = false;
		BB_DBG(bb, DBG_UL_TB_CTRL, "[%s] 52B C-cut turn off dyn bedge setting\n", __func__);
	}
	BB_DBG(bb, DBG_UL_TB_CTRL, "def_if_bandedge = %d, def_tri_idx = %d\n", bb_ul_tb->def_if_bandedge, bb_ul_tb->def_tri_idx);
	BB_DBG(bb, DBG_UL_TB_CTRL, "dyn_tb_bedge_en = %d, dyn_tb_tri_en = %d\n", bb_ul_tb->dyn_tb_bedge_en, bb_ul_tb->dyn_tb_tri_en);
}

void halbb_ul_tb_ctrl(struct bb_info *bb)
{
	struct bb_ul_tb_info *bb_ul_tb = &bb->bb_ul_tb_i;
	struct bb_link_info *bb_link = &bb->bb_link_i;
	struct rtw_phl_com_t *phl = bb->phl_com;
	struct dev_cap_t *dev = &phl->dev_cap;
	struct rtw_phl_stainfo_t *sta;
	struct bb_ul_tb_cr_info *cr = &bb->bb_ul_tb_i.bb_ul_tb_cr_i;
	struct bb_api_info *bb_api = &bb->bb_api_i;
	struct rtw_tpu_info *tpu = &bb->hal_com->band[bb->bb_phy_idx].rtw_tpu_i;
	bool is_ofdm = true;
	u8 num_high_tf_client = 0;
	u8 num_low_tf_client = 0;
	u8 num_active_client = 0;
	u32 i = 0;

	BB_DBG(bb, DBG_UL_TB_CTRL, "[%s]\n", __func__);

	if (!(bb->support_ability & BB_UL_TB_CTRL)) {
		BB_DBG(bb, DBG_UL_TB_CTRL, "[%s] Support ability DISABLED\n", __func__);
		return;
	}

	if (phl->fw_info.fw_type != RTW_FW_NIC) {
		BB_DBG(bb, DBG_UL_TB_CTRL, "Not STA mode, fw_type = %d\n", phl->fw_info.fw_type);
		return;
	}

	if (!bb_link->is_linked || bb_link->first_disconnect) {
		BB_DBG(bb, DBG_UL_TB_CTRL, "is_linked = %d, first_disconnect = %d\n", bb_link->is_linked, bb_link->first_disconnect);
		halbb_ul_tb_reset(bb);
		return;
	}

	for (i = 0; i < PHL_MAX_STA_NUM; i++) {
		if (!bb->sta_exist[i])
			continue;

		sta = bb->phl_sta_info[i];

		if (!is_sta_active(sta))
			continue;
		num_active_client ++;

		if (sta->stats.rx_tf_cnt - sta->stats.pre_rx_tf_cnt > bb_ul_tb->tf_cnt_l2h_th)
			num_high_tf_client++;
		else if (sta->stats.rx_tf_cnt - sta->stats.pre_rx_tf_cnt < bb_ul_tb->tf_cnt_h2l_th)
			num_low_tf_client++;

		BB_DBG(bb, DBG_UL_TB_CTRL, "[%d] macid=%d, rx_tf_cnt = %d, pre_rx_tf_cnt = %d, diff = %d\n", i, sta->macid, sta->stats.rx_tf_cnt, sta->stats.pre_rx_tf_cnt, sta->stats.rx_tf_cnt - sta->stats.pre_rx_tf_cnt);
		sta->stats.pre_rx_tf_cnt = sta->stats.rx_tf_cnt;
	}

	BB_DBG(bb, DBG_UL_TB_CTRL, "num_high_tf_client = %d, num_low_tf_client = %d\n", num_high_tf_client, num_low_tf_client);
	BB_DBG(bb, DBG_UL_TB_CTRL, "tf_cnt_l2h_th = %d, tf_cnt_h2l_th = %d\n", bb_ul_tb->tf_cnt_l2h_th, bb_ul_tb->tf_cnt_h2l_th);

	if (bb_ul_tb->dyn_tb_bedge_en && num_high_tf_client > 0) {
		halbb_set_reg(bb, cr->if_bandedge, cr->if_bandedge_m, 0);
		BB_DBG(bb, DBG_UL_TB_CTRL, "Turn off if_bandedge\n");
	} else if (bb_ul_tb->dyn_tb_bedge_en && num_active_client == num_low_tf_client){
		halbb_set_reg(bb, cr->if_bandedge, cr->if_bandedge_m, bb_ul_tb->def_if_bandedge);
		BB_DBG(bb, DBG_UL_TB_CTRL, "Default if_bandedge = %d\n", bb_ul_tb->def_if_bandedge);
	}

	if (bb_ul_tb->dyn_tb_tri_en && num_high_tf_client > 0) {
		tpu->tx_ptrn_shap_idx = 0;
		BB_DBG(bb, DBG_UL_TB_CTRL, "Tunr off Tx triangle\n");
	} else if (bb_ul_tb->dyn_tb_tri_en && num_active_client == num_low_tf_client){
		tpu->tx_ptrn_shap_idx = bb_ul_tb->def_tri_idx;
		BB_DBG(bb, DBG_UL_TB_CTRL, "Default tx_ptrn_shap_idx = %d\n", tpu->tx_ptrn_shap_idx);
	}

	halbb_set_tx_pow_pattern_shap(bb, bb_api->central_ch, is_ofdm, bb->bb_phy_idx);
}

void halbb_ul_tb_dbg(struct bb_info *bb, char input[][16], u32 *_used,
			      char *output, u32 *_out_len)
{
	struct bb_ul_tb_info *bb_ul_tb = &bb->bb_ul_tb_i;
	u32 var[10] = {0};

	if (_os_strcmp(input[1], "-h") == 0) {
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "{1} {dyn_tb_bedge_en}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "{2} {dyn_tb_tri_en}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "{3} {tf_cnt_l2h_th}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "{4} {tf_cnt_h2l_th}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "{100: show }\n");
		return;
	}

	HALBB_SCAN(input[1], DCMD_DECIMAL, &var[0]);
	if (var[0] == 1) {
		HALBB_SCAN(input[2], DCMD_DECIMAL, &var[1]);
		bb_ul_tb->dyn_tb_bedge_en = (bool)var[1];
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "set dyn_tb_bedge_en = %d\n", bb_ul_tb->dyn_tb_bedge_en);
	} else if (var[0] == 2) {
		HALBB_SCAN(input[2], DCMD_DECIMAL, &var[1]);
		bb_ul_tb->dyn_tb_tri_en = (bool)var[1];
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "set dyn_tb_tri_en = %d\n", bb_ul_tb->dyn_tb_tri_en);

	} else if (var[0] == 3) {
		HALBB_SCAN(input[2], DCMD_DECIMAL, &var[1]);
		bb_ul_tb->tf_cnt_l2h_th = var[1];
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "set tf_cnt_l2h_th = %d\n", bb_ul_tb->tf_cnt_l2h_th);
	} else if (var[0] == 4) {
		HALBB_SCAN(input[2], DCMD_DECIMAL, &var[1]);
		bb_ul_tb->tf_cnt_h2l_th = var[1];
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "set tf_cnt_h2l_th = %d\n", bb_ul_tb->tf_cnt_h2l_th);
	} else if (var[0] == 100) {
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "dyn_tb_bedge_en = %d\n", bb_ul_tb->dyn_tb_bedge_en);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "dyn_tb_tri_en = %d\n", bb_ul_tb->dyn_tb_tri_en);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "tf_cnt_l2h_th = %d\n", bb_ul_tb->tf_cnt_l2h_th);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "tf_cnt_h2l_th = %d\n", bb_ul_tb->tf_cnt_h2l_th);
	} else {
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "Set Error\n");
	} 
}

void halbb_ul_tb_ctrl_init(struct bb_info *bb)
{
	struct bb_ul_tb_info *bb_ul_tb = &bb->bb_ul_tb_i;

	BB_DBG(bb, DBG_UL_TB_CTRL, "[%s]\n", __func__);
	bb_ul_tb->dyn_tb_bedge_en = false;
	bb_ul_tb->dyn_tb_tri_en = true;
	bb_ul_tb->tf_cnt_l2h_th = TF_CNT_L2H_TH;
	bb_ul_tb->tf_cnt_h2l_th = TF_CNT_H2L_TH;
	
}

void halbb_cr_cfg_ul_tb_init(struct bb_info *bb)
{
	struct bb_ul_tb_cr_info *cr = &bb->bb_ul_tb_i.bb_ul_tb_cr_i;

	switch (bb->cr_type) {

	#ifdef HALBB_COMPILE_AP_SERIES
	case BB_AP:
		cr->if_bandedge = IF_BANDEDGE_A;
		cr->if_bandedge_m = IF_BANDEDGE_A_M;
		break;

	#endif
	#ifdef HALBB_COMPILE_CLIENT_SERIES
	case BB_CLIENT:
		cr->if_bandedge = IF_BANDEDGE_C;
		cr->if_bandedge_m = IF_BANDEDGE_C_M;
		
		break;
	#endif
	#ifdef HALBB_COMPILE_AP2_SERIES
	case BB_AP2:
		cr->if_bandedge = IF_BANDEDGE_A2;
		cr->if_bandedge_m = IF_BANDEDGE_A2_M;
		break;
	#endif

	default:
		BB_WARNING("[%s] BBCR Hook FAIL!\n", __func__);
		if (bb->bb_dbg_i.cr_fake_init_hook_en) {
			BB_TRACE("[%s] BBCR fake init\n", __func__);
			halbb_cr_hook_fake_init(bb, (u32 *)cr, (sizeof(struct bb_ul_tb_cr_info) >> 2));
		}
		break;
	}

	if (bb->bb_dbg_i.cr_init_hook_recorder_en) {
		BB_TRACE("[%s] BBCR Hook dump\n", __func__);
		halbb_cr_hook_init_dump(bb, (u32 *)cr, (sizeof(struct bb_ul_tb_cr_info) >> 2));
	}
}
#endif

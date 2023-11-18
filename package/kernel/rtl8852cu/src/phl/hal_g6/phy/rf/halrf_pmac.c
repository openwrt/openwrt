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

void halrf_set_pseudo_cw(struct rf_info *rf, enum rf_path path,
			u16 txagc_cw, bool en)
{
	u32 cw_addr[2] = {0x7c10, 0x7d10};

	halrf_wreg(rf, cw_addr[path], 0x000001FF, txagc_cw & 0x1ff);
	halrf_wreg(rf, cw_addr[path], BIT(9), en);

	if (en)
		RF_DBG(rf, DBG_RF_RFK,
		       "[RFK] Set S%d Pseudo_CW RF:0x%x, BB:%d(x0.125)\n",
		       path, (txagc_cw & 0x1f8) >> 3, txagc_cw & 0x7);
	else
		RF_DBG(rf, DBG_RF_RFK,
		       "[RFK] Set S%d Pseudo_CW off!!\n", path);
}

void halrf_set_plcp_usr_info(struct rf_info *rf, struct halbb_plcp_info *plcp,
				struct rf_pmac_tx_info *tx_info)
{
	plcp->usr[0].mcs = tx_info->mcs;
	plcp->usr[0].mpdu_len = 0; /*def*/
	plcp->usr[0].n_mpdu = 0;   /*def*/
	plcp->usr[0].fec = 0;
	plcp->usr[0].dcm = 0;
	plcp->usr[0].aid = 0;
	plcp->usr[0].scrambler_seed = 100;   /*rand(1~255)*/
	plcp->usr[0].random_init_seed = 100; /*rand(1~255)*/
	plcp->usr[0].apep = tx_info->length;
	plcp->usr[0].ru_alloc = 0;
	plcp->usr[0].nss = tx_info->nss;
	plcp->usr[0].txbf = 0; /*def*/
	plcp->usr[0].pwr_boost_db = 0;

	RF_DBG(rf, DBG_RF_RFK,
	       "[RFK] Set PLCP usr (mcs:%d, length:%d, nss:%d)\n",
	       tx_info->mcs, tx_info->length, tx_info->nss);
}

void halrf_set_plcp_para_info(struct rf_info *rf, struct halbb_plcp_info *plcp,
				struct rf_pmac_tx_info *tx_info)
{
	plcp->dbw = tx_info->bw; /*0:BW20, 1:BW40, 2:BW80, 3:BW160/BW80+80*/
	plcp->source_gen_mode = 3; /*def*/
	plcp->locked_clk = 1; /*def*/
	plcp->dyn_bw = 0; /*def*/
	plcp->ndp_en = 0; /*def*/
	plcp->long_preamble_en = tx_info->long_preamble_en;
	plcp->stbc = 0;
	plcp->gi = tx_info->gi;
	plcp->tb_l_len = 0;
	plcp->tb_ru_tot_sts_max = 0;
	plcp->vht_txop_not_allowed = 0;
	plcp->tb_disam = 0;
	plcp->doppler = 0; /*def*/
	plcp->he_ltf_type = 0;
	plcp->ht_l_len = 0; /*def*/
	plcp->preamble_puncture = 0; /*def*/
	plcp->he_mcs_sigb = 0;
	plcp->he_dcm_sigb = 0;
	plcp->he_sigb_compress_en = 1; /*def*/
	plcp->max_tx_time_0p4us = 0;
	plcp->ul_flag = 0; /*def*/
	plcp->tb_ldpc_extra = 0;
	plcp->bss_color = 0;
	plcp->sr = 0; /*def*/
	plcp->beamchange_en = 1; /*def*/
	plcp->he_er_u106ru_en = 0;
	plcp->ul_srp1 = 0; /*def*/
	plcp->ul_srp2 = 0; /*def*/
	plcp->ul_srp3 = 0; /*def*/
	plcp->ul_srp4 = 0; /*def*/
	plcp->mode = 0;
	plcp->group_id = 0;
	plcp->ppdu_type = tx_info->ppdu;
	plcp->txop = 127; /*def*/
	plcp->tb_strt_sts = 0;
	plcp->tb_pre_fec_padding_factor = 0;
	plcp->cbw = 0;
	plcp->txsc = 0;
	plcp->tb_mumimo_mode_en = 0;
	plcp->nominal_t_pe = 2; /*def*/
	plcp->ness = 0; /*def*/
	plcp->n_user = 1;
	plcp->tb_rsvd = 0; /*def*/

	/*halrf_mem_cpy(rf, plcp->usr, rf->usr, 4*sizeof(struct usr_plcp_gen_in));*/

	RF_DBG(rf, DBG_RF_RFK,
	       "[RFK] Set PLCP para (BW:%d, long_preamble:%d, GI:%d, PPDU:%d)\n",
	       tx_info->bw, tx_info->long_preamble_en, tx_info->gi ,tx_info->ppdu);
}

void halrf_set_pmac_plcp_gen(struct rf_info *rf, enum phl_phy_idx phy_idx,
			struct rf_pmac_tx_info *tx_info)
{
	struct halbb_plcp_info plcp = {0};
	u8 sts = 0;

	halrf_set_plcp_usr_info(rf, &plcp, tx_info);

	halrf_set_plcp_para_info(rf, &plcp, tx_info);

	rtw_hal_bb_set_plcp_tx((rf)->hal_com, (void*)&plcp, (void*)&plcp.usr, phy_idx, &sts);
}

void halrf_set_pmac_tx(struct rf_info *rf, enum phl_phy_idx phy_idx,
			enum rf_path path, struct rf_pmac_tx_info *tx,
			u8 enable, bool by_cw)
{
	if (enable) {
		RF_DBG(rf, DBG_RF_RFK,
			"[RFK] Set S%d PMAC Tx (PHY%d)\n", path, phy_idx);

		halrf_set_pmac_plcp_gen(rf, phy_idx, tx);

		if (path != RF_PATH_ABCD) {
			rtw_hal_bb_cfg_tx_path((rf)->hal_com, path, phy_idx);
			rtw_hal_bb_cfg_rx_path((rf)->hal_com, path, phy_idx);
		}

		if (by_cw)
			halrf_set_pseudo_cw(rf, path, tx->txagc_cw, true);
		else
			rtw_hal_bb_set_power((rf)->hal_com, tx->dbm, phy_idx);
	} else
		RF_DBG(rf, DBG_RF_RFK, "[RFK] Disable PMAC Tx!!\n");

	rtw_hal_bb_set_pmac_packet_tx((rf)->hal_com, enable, tx->is_cck, tx->cnt,
				 tx->period, tx->time, false, phy_idx);
}

#if 0
void halrf_set_pmac_tx(struct rf_info *rf, enum phl_phy_idx phy_idx,
			enum rf_path path, enum packet_format_t ppdu_type, u8 case_id,
			s16 dbm, u8 enable, u8 is_cck, u16 cnt ,u16 time, u16 period)
{
#if 1
	if (enable) {
		RF_DBG(rf, DBG_RF_RFK,
		       "[RFK] Set PMAC Tx (PHY%d, S%d, PPDU:%d, case:%d)\n",
		       phy_idx, path, ppdu_type, case_id);
		RF_DBG(rf, DBG_RF_RFK,
	 	       "[RFK] Set PMAC Tx (%ddBm, Cnt:%d, time:%d, period:%d)\n",
		       dbm, cnt, time, period);

		//halrf_set_pmac_pattern(rf, ppdu_type, case_id, phy_idx);
		halrf_set_pmac_plcp_gen(rf, 7, 0, 1, 0, 0, 1, ppdu_type, phy_idx);
		halrf_cfg_tx_path(rf, path);
		//halrf_set_pmac_power(rf, dbm, phy_idx);
		halrf_set_pseudo_cw(rf, path, 0x1d0, true);
	} else
		RF_DBG(rf, DBG_RF_RFK, "[RFK] Disable PMAC Tx!!\n");

	halrf_set_pmac_packet_tx(rf, enable, is_cck, cnt, period, time, phy_idx);
#endif
}
#endif


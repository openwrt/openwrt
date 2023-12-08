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

#ifdef HALBB_PHYSTS_PARSING_SUPPORT

void halbb_7_physts_detail_dump_ie_4_7(struct bb_info *bb, enum bb_physts_ie_t ie)
{
	struct bb_physts_info	*physts = &bb->bb_physts_i;
	struct bb_physts_rslt_4_to_7_info *psts_r = NULL;
	struct physts_7_ie_4_to_7_info *ie_4_7 = NULL;
	char dbg_buf_1[HALBB_SNPRINT_SIZE_S], dbg_buf_2[HALBB_SNPRINT_SIZE_S], dbg_buf_3[HALBB_SNPRINT_SIZE_S];

	if (ie < IE04_CMN_EXT_PATH_A)
		return;

	BB_TRACE("[%02d][Extend path-%c]\n", ie, ('A' + (u8)(ie - IE04_CMN_EXT_PATH_A)));

	if (ie == IE04_CMN_EXT_PATH_A)
		psts_r = &physts->bb_physts_rslt_4_i;
	else if (ie == IE05_CMN_EXT_PATH_B)
		psts_r = &physts->bb_physts_rslt_5_i;
	else if (ie == IE06_CMN_EXT_PATH_C)
		psts_r = &physts->bb_physts_rslt_6_i;
	else if (ie == IE07_CMN_EXT_PATH_D)
		psts_r = &physts->bb_physts_rslt_7_i;
	else
		return;

	ie_4_7 = (struct physts_7_ie_4_to_7_info *)psts_r->ie_04_07_addr;

	BB_TRACE(" *ie_hdr=%d\n", ie_4_7->ie_hdr);

	halbb_print_sign_frac_digit(bb, (s8)ie_4_7->sig_val_y, 8, 2, dbg_buf_1, HALBB_SNPRINT_SIZE_S);
	halbb_print_sign_frac_digit(bb, (s8)ie_4_7->td_ant_weight, 8, 6, dbg_buf_3, HALBB_SNPRINT_SIZE_S);

	BB_TRACE(" *ant=%d, sig_val=%s, rf_gain_idx=%d, snr_lgy=%d, evm_ss_y=%d.%02d, ant_weight=%s\n",
		 ie_4_7->ant_idx, dbg_buf_1, ie_4_7->rf_gain_idx,
		 ie_4_7->snr_lgy, ie_4_7->evm_ss_y >> 2,
		 halbb_show_fraction_num(ie_4_7->evm_ss_y & 0x3, 2), dbg_buf_3);

	halbb_print_sign_frac_digit(bb, (s8)ie_4_7->dc_est_re, 8, 7, dbg_buf_1, HALBB_SNPRINT_SIZE_S);
	halbb_print_sign_frac_digit(bb, (s8)ie_4_7->dc_est_im, 8, 7, dbg_buf_2, HALBB_SNPRINT_SIZE_S);

	BB_TRACE(" *dc_est_re/im={%s, %s}, tia_gain=%d, tia_shrink=%d\n",
		 dbg_buf_1, dbg_buf_2,
		 ie_4_7->rf_tia_gain_idx, ie_4_7->tia_shrink_indicator);
}

void halbb_7_physts_detail_dump(struct bb_info *bb, u32 bitmap, u32 bitmap_mask)
{
	struct bb_physts_info	*physts = &bb->bb_physts_i;
	struct bb_physts_rslt_hdr_info	*psts_h = &physts->bb_physts_rslt_hdr_i;
	struct bb_physts_rslt_0_info *psts_0 = &physts->bb_physts_rslt_0_i;
	struct bb_physts_rslt_1_info *psts_1 = &physts->bb_physts_rslt_1_i;
	struct physts_7_ie_1_info *ie_1 = NULL;
	struct physts_ie_1_info_type1 *ie_1_type1 = NULL;
	struct bb_physts_rslt_2_info *psts_2 = &physts->bb_physts_rslt_2_i;
	struct bb_physts_rslt_8_info *psts_8 = &physts->bb_physts_rslt_8_i;
	struct bb_physts_rslt_9_info *psts_9 = &physts->bb_physts_rslt_9_i;
	struct bb_physts_rslt_12_info *psts_12 = &physts->bb_physts_rslt_12_i;
	struct bb_physts_rslt_13_info *psts_13 = &physts->bb_physts_rslt_13_i;
	struct bb_physts_rslt_14_info *psts_14 = &physts->bb_physts_rslt_14_i;
	struct bb_physts_rslt_15_info *psts_15 = &physts->bb_physts_rslt_15_i;
	struct bb_physts_rslt_17_info *psts_17 = &physts->bb_physts_rslt_17_i;
	struct bb_cmn_rpt_info	*cmn_rpt = &bb->bb_cmn_rpt_i;
	char *tmp_char = NULL;
//	u8 i;

	if (!physts->is_valid) {
		BB_TRACE("physts invalid\n");
		return;
	}

	if (psts_h->ie_map_type > 16)
		return;

	if (bitmap_mask != 0xffffffff) {
		BB_TRACE("user_def_mask=0x%08x\n", bitmap_mask);
	}

	BB_TRACE("[Hdr] rssi={%d}{%d,%d,%d,%d}\n",
		 TRANS_2_RSSI(psts_h->rssi_avg),
		 TRANS_2_RSSI(psts_h->rssi_td[0]),
		 TRANS_2_RSSI(psts_h->rssi_td[1]),
		 TRANS_2_RSSI(psts_h->rssi_td[2]),
		 TRANS_2_RSSI(psts_h->rssi_td[3]));

	BB_TRACE("[Hdr] RPL={%d}{%d,%d,%d,%d}\n",
		 TRANS_2_RSSI(physts->rpl_avg),
		 TRANS_2_RSSI(physts->rpl_path[0]),
		 TRANS_2_RSSI(physts->rpl_path[1]),
		 TRANS_2_RSSI(physts->rpl_path[2]),
		 TRANS_2_RSSI(physts->rpl_path[3]));
	BB_TRACE("ppdu_idx=%d\n", psts_h->ppdu_idx);

	if (bitmap == 0) {
		BB_TRACE("Hdr only, empty IE\n");
		return;
	}

	if (bitmap & BIT(IE00_CMN_CCK)) {
		BB_TRACE("[00][CCK] path_en=0x%x, rpl=%d.%01d, cfo=%s%d.%02d kHz, evm=%d.%02d\n",
			 psts_0->rx_path_en_cck,
			 psts_0->rpl >> 2,
			 halbb_show_fraction_num(psts_0->rpl & 0x3, 2),
			 ((psts_0->cfo_avg_cck & BIT15)!= 0) ? "-": " ",
			 ABS_16(psts_0->cfo_avg_cck) >> 2,
			 halbb_show_fraction_num((u16)ABS_16(psts_0->cfo_avg_cck) & 0x3, 2),
			 psts_0->evm_avg_cck >> 2,
			 halbb_show_fraction_num(psts_0->evm_avg_cck & 0x3, 2));
		BB_TRACE("[00][CCK] idle_noise=%d.%01d, pop idx=%d\n",
			 psts_0->avg_idle_noise_pwr_cck >> 1,
			 halbb_show_fraction_num(psts_0->avg_idle_noise_pwr_cck & 0x1, 1),
			 psts_0->pop_idx_cck);
	}

	if (bitmap & BIT(IE01_CMN_OFDM)) {
			ie_1 = (struct physts_7_ie_1_info *)physts->bb_physts_rslt_1_i.ie_01_addr;

		BB_TRACE("[01][OFDM] path_en=0x%x, rpl=%d.%01d, cfo_fd=%s%d.%02d kHz, cfo_pab=%s%d.%02d kHz, Max&Min evm={%d.%02d,%d.%02d}, avg SNR=%d , CN=%d.%01d\n",
			 psts_1->rx_path_en,
			 psts_1->rpl_fd >> 1,
			 halbb_show_fraction_num(psts_1->rpl_fd & 0x1, 1),
			 ((psts_1->cfo_avg & BIT15) != 0) ? "-": " ",
			 ABS_16(psts_1->cfo_avg) >> 2,
			 halbb_show_fraction_num((u16)ABS_16(psts_1->cfo_avg) & 0x3, 2),
			 ((psts_1->cfo_pab_avg & BIT15) != 0) ? "-": " ",
			 ABS_16(psts_1->cfo_pab_avg) >> 2,
			 halbb_show_fraction_num((u16)ABS_16(psts_1->cfo_pab_avg) & 0x3, 2),
			 psts_1->evm_max >> 2,
			 halbb_show_fraction_num(psts_1->evm_max & 0x3, 2),
			 psts_1->evm_min >> 2,
			 halbb_show_fraction_num(psts_1->evm_min & 0x3, 2),
			 psts_1->snr_avg,
			 psts_1->cn_avg >> 1,
			 halbb_show_fraction_num(psts_1->cn_avg & 0x1, 1));
		BB_TRACE("[01][OFDM] idle_noise=%d, pop idx=%d, band=%d, ch=%d, bw=%d, rxsc=%d, su=%d, ldpc=%d, stbc=%d, gnt_bt=%d, awgn=%d, bf=%d\n",
			 psts_1->avg_idle_noise_pwr >> 1, psts_1->pop_idx,
			 psts_1->band, psts_1->ch_idx, psts_1->bw_idx, psts_1->rxsc,
			 psts_1->is_su, psts_1->is_ldpc, psts_1->is_stbc,
			 psts_1->grant_bt, psts_1->is_awgn, psts_1->is_bf);

		if (bb->ic_type == BB_RTL8852C) {
			BB_TRACE("[01][OFDM] pwr_to_cca=%d ns, cca_to_agc=%d ns, cca_to_sbd=%dns\n",
				 (BYTE_2_WORD((u32)ie_1_type1->pwr_to_cca_m, (u32)ie_1_type1->pwr_to_cca_l) * 25),
				 ie_1_type1->cca_to_agc * 25, ie_1_type1->cca_to_sbd * 25);
			BB_TRACE("[01][OFDM] bt_gnt_tx_at_cca=%d, bt_gnt_tx_cnt=%d, bt_gnt_rx_at_cca=%d, bt_gnt_rx_cnt=%d\n",
				 ie_1_type1->bt_gnt_tx_at_cca, ie_1_type1->bt_gnt_tx_cnt,
				 ie_1_type1->bt_gnt_rx_at_cca, ie_1_type1->bt_gnt_rx_cnt);
		} else {
			BB_TRACE("[01][OFDM] pwr_to_cca=%d ns, cca_to_agc=%d ns, cca_to_sbd=%dns\n",
				 (BYTE_2_WORD((u32)ie_1->pwr_to_cca_m, (u32)ie_1->pwr_to_cca_l) * 25),
				 ie_1->cca_to_agc * 25, ie_1->cca_to_sbd * 25);
		}
	}

	if (bitmap & BIT(IE02_CMN_EXT_AX)) {
		BB_TRACE("[02][HE] max_nsts=%d, ltf_type=%d, gi=%d, c_cfo_i/q={%s%d.%07d, %s%d.%07d}, f_cfo_i/q={%s%d.%07d, %s%d.%07d}\n",
			 psts_2->max_nsts, psts_2->ltf_type, psts_2->gi,
			 ((psts_2->c_cfo_i & BIT31)!= 0) ? "-": " ",
			 ((u32)ABS_32(psts_2->c_cfo_i) >> 7),
			 halbb_show_fraction_num((u32)ABS_32(psts_2->c_cfo_i) & 0x7f, 7),
			 ((psts_2->c_cfo_q & BIT31)!= 0) ? "-": " ",
			 ((u32)ABS_32(psts_2->c_cfo_q) >> 7),
			 halbb_show_fraction_num((u32)ABS_32(psts_2->c_cfo_q) & 0x7f, 7),
			 ((psts_2->f_cfo_i & BIT31)!= 0) ? "-": " ",
			 ((u32)ABS_32(psts_2->f_cfo_i) >> 7),
			 halbb_show_fraction_num((u32)ABS_32(psts_2->f_cfo_i) & 0x7f, 7),
			 ((psts_2->f_cfo_q & BIT31)!= 0) ? "-": " ",
			 ((u32)ABS_32(psts_2->f_cfo_q) >> 7),
			 halbb_show_fraction_num((u32)ABS_32(psts_2->f_cfo_q) & 0x7f, 7));

		BB_TRACE("[02][HE] rx_info_1=%d, rx_state_feq=%d, est_cmped_phase=0.%08d, pkt_ext=%d, n_ltf=%d, n_sym=%d\n",
			 psts_2->rx_info_1, psts_2->rx_state_feq,
			 halbb_show_fraction_num(psts_2->est_cmped_phase & 0xff, 8),
			 psts_2->pkt_extension, psts_2->n_ltf, psts_2->n_sym);
	}

	if (bitmap & BIT(IE03_CMN_EXT_SEG_1)) {
		/* suspended due to low priority */ 
		
	}

	if (bitmap & BIT(IE04_CMN_EXT_PATH_A))
		halbb_7_physts_detail_dump_ie_4_7(bb, IE04_CMN_EXT_PATH_A);

	if (bitmap & BIT(IE05_CMN_EXT_PATH_B))
		halbb_7_physts_detail_dump_ie_4_7(bb, IE05_CMN_EXT_PATH_B);

	if (bitmap & BIT(IE06_CMN_EXT_PATH_C))
		halbb_7_physts_detail_dump_ie_4_7(bb, IE06_CMN_EXT_PATH_C);

	if (bitmap & BIT(IE07_CMN_EXT_PATH_D))
		halbb_7_physts_detail_dump_ie_4_7(bb, IE07_CMN_EXT_PATH_D);

	if (bitmap & BIT(IE08_FTR_CH)) {
		BB_TRACE("[08][ch_info] rxsc=%d, n_rx=%d, sts=%d, EVM{1,2}={%d.%02d, %d.%02d}, avg_noise_pw=%d.%01d\n",
			 psts_8->rxsc, psts_8->n_rx, psts_8->n_sts,
			 psts_8->evm_1_sts >> 2,
			 halbb_show_fraction_num(psts_8->evm_1_sts & 0x3, 2),
			 psts_8->evm_2_sts >> 2,
			 halbb_show_fraction_num(psts_8->evm_2_sts & 0x3, 2),
			 (psts_8->avg_idle_noise_pwr >> 1),
			 halbb_show_fraction_num(psts_8->avg_idle_noise_pwr & 0x1, 1));
	}

	if (bitmap & BIT(IE09_FTR_PLCP_0)) {
		if (cmn_rpt->bb_rate_i.mode == BB_HE_MODE)
			tmp_char = "HE";
		else if (cmn_rpt->bb_rate_i.mode == BB_VHT_MODE)
			tmp_char = "VHT";
		else if (cmn_rpt->bb_rate_i.mode == BB_HT_MODE)
			tmp_char = "HT";
		else
			tmp_char = "Lgcy";

		BB_TRACE("[09][PLCP] mode=%s LSIG=0x%06x, SIGA1=0x%08x, SIGA2=0x%08x\n",
			 tmp_char, psts_9->l_sig, psts_9->sig_a1, psts_9->sig_a2);
	}
#if 0
	if (bitmap & BIT(IE11_FTR_PLCP_HISTOGRAM))
		halbb_physts_detail_dump_ie_11(bb);

	if (bitmap & BIT(IE12_MU_EIGEN_INFO)) {
		for (i = 0; i < psts_12->n_user; i++) {
			if (i >= MU_USER_MAX)
				break;

			BB_TRACE("[12][MU_eigen][U:%d] sig_val_ss[3:0]={%d,%d,%d,%d}, cn bad tone=%d, sig bad tone=%d\n",
				i, 
				psts_12->bb_physts_uer_info[i].sig_val_ss3_seg_cr_user_i,
				psts_12->bb_physts_uer_info[i].sig_val_ss2_seg_cr_user_i,
				psts_12->bb_physts_uer_info[i].sig_val_ss1_seg_cr_user_i,
				psts_12->bb_physts_uer_info[i].sig_val_ss0_seg_cr_user_i,
				psts_12->bb_physts_uer_info[i].cn_bad_tone_cnt_seg_cr_user_i,
				psts_12->bb_physts_uer_info[i].sig_bad_tone_cnt_seg_cr_user_i);
		}
	}

	if (bitmap & BIT(IE13_DL_MU_DEF)) {
		for (i = 0; i < psts_13->n_user; i++) {
			if (i >= MU_USER_MAX)
				break;

			BB_TRACE("[13][HEMU][U:%d] avg_cn=%d.%01d, fec_type=%d, awgn=%d, bf=%d, dcm=%d, mu=%d\n",
				i,
				(psts_13->bb_physts_uer_info[i].avg_cn_seg_cr >> 1),
				halbb_show_fraction_num(psts_13->bb_physts_uer_info[i].avg_cn_seg_cr & 0x1, 1),
				psts_13->bb_physts_uer_info[i].fec_type,
				psts_13->bb_physts_uer_info[i].is_awgn,
				psts_13->bb_physts_uer_info[i].is_bf,
				psts_13->bb_physts_uer_info[i].is_dcm,
				psts_13->bb_physts_uer_info[i].is_mu_mimo);

			BB_TRACE("[13][HEMU][U:%d] mcs=%d, sts=%d, sts_ru_total=%d, gi_ltf=%d, pilot_exist=%d, mu=%d\n",
				i,
				psts_13->bb_physts_uer_info[i].mcs,
				psts_13->bb_physts_uer_info[i].n_sts,
				psts_13->bb_physts_uer_info[i].n_sts_ru_total,
				psts_13->bb_physts_uer_info[i].pdp_he_ltf_and_gi_type,
				psts_13->bb_physts_uer_info[i].pilot_exist,
				psts_13->bb_physts_uer_info[i].is_mu_mimo);

			BB_TRACE("[13][HEMU][U:%d] ru_aloc=%d, evm_segN_max/min={%d.%02d, %d.%02d}, snr=%s%d.%01d, start_sts=%d\n",
				i, 
				psts_13->bb_physts_uer_info[i].ru_alloc,
				psts_13->bb_physts_uer_info[i].rx_evm_max_seg_cr >> 2,
				halbb_show_fraction_num(psts_13->bb_physts_uer_info[i].rx_evm_max_seg_cr & 0x3, 2),
				psts_13->bb_physts_uer_info[i].rx_evm_min_seg_cr >> 2,
				halbb_show_fraction_num(psts_13->bb_physts_uer_info[i].rx_evm_min_seg_cr & 0x3, 2),
				((psts_13->bb_physts_uer_info[i].snr & BIT7)!= 0) ? "-": " ",
				ABS_8(psts_13->bb_physts_uer_info[i].snr ) >> 1,
				halbb_show_fraction_num((u8)ABS_8(psts_13->bb_physts_uer_info[i].snr ) & 0x1, 1),
				psts_13->bb_physts_uer_info[i].start_sts);
		}

		BB_TRACE("[13][HEMU] n_not_sup_sta=%d\n", psts_13->n_not_sup_sta);
	}

	if (bitmap & BIT(IE14_TB_UL_CQI)) {
		BB_TRACE("[14][TB_CQI] ndp_en=%d, n_user=%d\n",
			psts_14->rxinfo_ndp_en, psts_14->n_user);
		
		for (i = 0; i < psts_14->n_user; i++) {
			if (i >= TB_USER_MAX)
				break;
			BB_TRACE("[14][TB_CQI][U:%d] cqi_bitmap_ul_tb=%d\n",
				i, 
				psts_14->bb_physts_uer_info[i].cqi_bitmap_ul_tb);
		}
	}

	if (bitmap & BIT(IE15_TB_UL_DEF)) {
		BB_TRACE("[15][TB_AC_MU] n_user=%d\n", psts_15->n_user);
		for (i = 0; i < psts_15->n_user; i++) {
			if (i >= TB_USER_MAX)
				break;

			BB_TRACE("[15][TB_AC_MU][U:%d] avg_cn=%d.%01d, fec_type=%d, awgn=%d, bf=%d, dcm=%d, mu=%d\n",
				i,
				(psts_15->bb_physts_uer_info[i].avg_cn_seg_cr >> 1),
				halbb_show_fraction_num(psts_15->bb_physts_uer_info[i].avg_cn_seg_cr & 0x1, 1),
				psts_15->bb_physts_uer_info[i].fec_type,
				psts_15->bb_physts_uer_info[i].is_awgn,
				psts_15->bb_physts_uer_info[i].is_bf,
				psts_15->bb_physts_uer_info[i].is_dcm,
				psts_15->bb_physts_uer_info[i].is_mu_mimo);

			BB_TRACE("[15][TB_AC_MU][U:%d] mcs=%d, sts=%d, sts_ru_total=%d, gi_ltf=%d, pilot_exist=%d, mu=%d\n",
				i, 
				psts_15->bb_physts_uer_info[i].mcs,
				psts_15->bb_physts_uer_info[i].n_sts,
				psts_15->bb_physts_uer_info[i].n_sts_ru_total,
				psts_15->bb_physts_uer_info[i].pdp_he_ltf_and_gi_type,
				psts_15->bb_physts_uer_info[i].pilot_exist,
				psts_15->bb_physts_uer_info[i].is_mu_mimo);

			BB_TRACE("[15][TB_AC_MU][U:%d] ru_aloc=%d, evm_segN_max/min={%d.%02d, %d.%02d}, snr=%s%d.%02d, start_sts=%d\n",
				i, 
				psts_15->bb_physts_uer_info[i].ru_alloc,
				psts_15->bb_physts_uer_info[i].rx_evm_max_seg_cr >> 2,
				halbb_show_fraction_num(psts_15->bb_physts_uer_info[i].rx_evm_max_seg_cr & 0x3, 2),
				(psts_15->bb_physts_uer_info[i].rx_evm_min_seg_cr >> 2),
				halbb_show_fraction_num(psts_15->bb_physts_uer_info[i].rx_evm_min_seg_cr & 0x3, 2),
				((psts_15->bb_physts_uer_info[i].snr & BIT7)!= 0) ? "-": " ",
				((u8)ABS_8(psts_15->bb_physts_uer_info[i].snr ) >> 1),
				halbb_show_fraction_num((u8)ABS_8(psts_15->bb_physts_uer_info[i].snr ) & 0x1, 1),
				(psts_15->bb_physts_uer_info[i].start_sts ));

			BB_TRACE("[15][TB_AC_MU][U:%d] uid=%d, avg_cfo_seg0=%s%d.%02d, rssi_m_ul_tb=%d.%02d\n",
				i, psts_15->bb_physts_uer_info[i].uid, 
				((psts_15->bb_physts_uer_info[i].avg_cfo_seg0 & BIT15)!= 0) ? "-": " ",
				((u16)ABS_16(psts_15->bb_physts_uer_info[i].avg_cfo_seg0) >> 2),
				halbb_show_fraction_num((u16)ABS_16(psts_15->bb_physts_uer_info[i].avg_cfo_seg0) & 0x3, 2),
				(psts_15->bb_physts_uer_info[i].rssi_m_ul_tb >> 2),
				halbb_show_fraction_num(psts_15->bb_physts_uer_info[i].rssi_m_ul_tb & 0x3, 2));

		}
	}
	if (bitmap & BIT(IE17_TB_UL_CTRL)) {
		BB_TRACE("[17][TB_UL_CTRL] n_user=%d\n", psts_17->n_user);

		BB_TRACE("[17][TB_UL_CTRL][CMN] stbc_en=%d, ldpc_extra=%d, doppler_en=%d, midamle_mode=%d, gi_type=%d, ltf_type=%d\n",
			psts_17->bb_physts_cmn_info.stbc_en ,
			psts_17->bb_physts_cmn_info.ldpc_extra,
			psts_17->bb_physts_cmn_info.doppler_en,
			psts_17->bb_physts_cmn_info.midamle_mode,
			psts_17->bb_physts_cmn_info.gi_type,
			psts_17->bb_physts_cmn_info.ltf_type);
		BB_TRACE("[17][TB_UL_CTRL][CMN] n_ltf=%d, n_sym=%d, pe_idx=%d, pre_fec_factor=%d, n_usr=%d, mumimo_ltf_mode_en=%d\n",
			psts_17->bb_physts_cmn_info.n_ltf,
			psts_17->bb_physts_cmn_info.n_sym,
			psts_17->bb_physts_cmn_info.pe_idx,
			psts_17->bb_physts_cmn_info.pre_fec_factor,
			psts_17->bb_physts_cmn_info.n_usr,
			psts_17->bb_physts_cmn_info.mumimo_ltf_mode_en);
		BB_TRACE("[17][TB_UL_CTRL][CMN] ndp=%d, pri_exp_rssi_dbm=%d, dbw_idx=%d, rxtime=%d\n",
			psts_17->bb_physts_cmn_info.ndp,
			psts_17->bb_physts_cmn_info.pri_exp_rssi_dbm,
			psts_17->bb_physts_cmn_info.dbw_idx,
			psts_17->bb_physts_cmn_info.rxtime);
		for (i = 0; i < psts_15->n_user; i++) {
			if (i >= TB_USER_MAX)
				break;

			BB_TRACE("[17][TB_UL_CTRL][U:%d] u_id=%d, ru_alloc=%d, n_sts_ru_tot=%d, strt_sts=%d, n_sts=%d, fec_type=%d, mcs=%d, dcm_en=%d\n",
				i,
				psts_17->bb_physts_uer_info[i].u_id,
				psts_17->bb_physts_uer_info[i].ru_alloc,
				psts_17->bb_physts_uer_info[i].n_sts_ru_tot,
				psts_17->bb_physts_uer_info[i].strt_sts,
				psts_17->bb_physts_uer_info[i].n_sts,
				psts_17->bb_physts_uer_info[i].fec_type,
				psts_17->bb_physts_uer_info[i].mcs,
				psts_17->bb_physts_uer_info[i].dcm_en);

		}
	}
	#endif
}

u8 halbb_7_physts_ie_hdr(struct bb_info *bb,
		       u8 *addr,
		       struct physts_rxd *desc, u16 *ie_length_hdr)
{
	struct bb_physts_info	*physts = &bb->bb_physts_i;
	struct bb_physts_rslt_hdr_info	*psts_h = &physts->bb_physts_rslt_hdr_i;
	struct physts_hdr_info *physts_hdr = NULL;
	u8 *addr_in_bkp = addr;

	/*---[Physts header parsing]------------------------------------------*/
	physts_hdr = (struct physts_hdr_info *)addr;

	#if 0
	if (desc->is_to_self) {
		BB_DBG(bb, DBG_PHY_STS, "ie_map=%02d, null_tb_ppdu=%d, valid=%d, length=%d, rssi= {%d}{%d, %d, %d, %d}\n\n", 
			physts_hdr->ie_bitmap_select,
			physts_hdr->null_tb_ppdu,
			physts_hdr->is_valid,
			physts_hdr->physts_total_length,
			TRANS_2_RSSI(physts_hdr->rssi_avg_td),
			TRANS_2_RSSI(physts_hdr->rssi_td[0]),
			TRANS_2_RSSI(physts_hdr->rssi_td[1]),
			TRANS_2_RSSI(physts_hdr->rssi_td[2]),
			TRANS_2_RSSI(physts_hdr->rssi_td[3]));
	}
	#endif

	psts_h->rssi_avg = physts_hdr->rssi_avg_td;
	psts_h->rssi[0] = physts_hdr->rssi_td[0];
	psts_h->rssi[1] = physts_hdr->rssi_td[1];
	psts_h->rssi[2] = physts_hdr->rssi_td[2];
	psts_h->rssi[3] = physts_hdr->rssi_td[3];
	psts_h->ppdu_idx = physts_hdr->ppdu_idx;
	psts_h->ie_map_type = (enum bb_physts_bitmap_t)physts_hdr->ie_bitmap_select;

	*ie_length_hdr = physts_hdr->physts_total_length;

	return physts_hdr->is_valid;
}

bool halbb_7_physts_ie_00(struct bb_info *bb,
			   u8 *addr,
			   u16 ie_length,
			   struct physts_rxd *desc)
{
	struct bb_physts_info	*physts = &bb->bb_physts_i;
	struct bb_physts_rslt_0_info *psts_0 = &physts->bb_physts_rslt_0_i;
	struct physts_7_ie_0_info *physts_ie_0 = NULL;
	u32 cfo_tmp;

	physts_ie_0 = (struct physts_7_ie_0_info *)addr;
	psts_0->ie_00_addr = addr;

	psts_0->rx_path_en_cck = physts_ie_0->rx_path_en_bitmap;
	cfo_tmp = (physts_ie_0->avg_cfo_m << 8) | physts_ie_0->avg_cfo_l;
	psts_0->cfo_avg_cck = (s16) halbb_cnvrt_2_sign(cfo_tmp, 12);
	psts_0->evm_avg_cck = physts_ie_0->rxevm_pld;
	psts_0->avg_idle_noise_pwr_cck = physts_ie_0->avg_idle_noise_pwr;
	psts_0->rpl = (physts_ie_0->rpl_m << 1) | physts_ie_0->rpl_l;
	psts_0->pop_idx_cck = physts_ie_0->pop_idx;

	halbb_mod_rssi_by_path_en(bb, psts_0->rx_path_en_cck);
	physts->rpl_avg = (u8)(psts_0->rpl >> 1);

	return true;
}

bool halbb_7_physts_ie_01(struct bb_info *bb,
			   u8 *addr,
			   u16 ie_length,
			   struct physts_rxd *desc)
{
	struct bb_physts_info	*physts = &bb->bb_physts_i;
	struct bb_physts_rslt_1_info *psts_1 = &physts->bb_physts_rslt_1_i;
	struct physts_7_ie_1_info *ie_1 = NULL;
	u32 cfo_tmp;
	u8 ch_idx_tmp = 0;
	enum band_type band_tmp = BAND_ON_24G;

	ie_1 = (struct physts_7_ie_1_info *)addr;
	psts_1->ie_01_addr = addr;

	halbb_ch_idx_decode(bb, ie_1->ch_idx_seg0, &ch_idx_tmp, &band_tmp);
	psts_1->ch_idx = ch_idx_tmp;
	psts_1->band = band_tmp;

	if (desc->data_rate <= BB_11M) {
		return true;
	}

	/*0~6: 5, 10, 20, 40, 80, 160, 80_80*/
	
	if (ie_1->bw_idx == 0)
		psts_1->bw_idx = CHANNEL_WIDTH_5;
	else if (ie_1->bw_idx == 1)
		psts_1->bw_idx = CHANNEL_WIDTH_10;
	else if (ie_1->bw_idx == 2)
		psts_1->bw_idx = CHANNEL_WIDTH_20;
	else if (ie_1->bw_idx == 3)
		psts_1->bw_idx = CHANNEL_WIDTH_40;
	else if (ie_1->bw_idx == 4)
		psts_1->bw_idx = CHANNEL_WIDTH_80;
	else if (ie_1->bw_idx == 5)
		psts_1->bw_idx = CHANNEL_WIDTH_160;
	else if (ie_1->bw_idx == 6)
		psts_1->bw_idx = CHANNEL_WIDTH_80_80;
	else
		psts_1->bw_idx = CHANNEL_WIDTH_20;

	psts_1->rx_path_en = ie_1->rx_path_en_bitmap;
#if 0
	cfo_tmp = (ie_1->avg_cfo_premb_seg0_m << 4) | ie_1->avg_cfo_premb_seg0_l;
	psts_1->cfo_pab_avg = (s16) halbb_cnvrt_2_sign(cfo_tmp, 12);
#else
	cfo_tmp = (((u32)ie_1->avg_cfo_seg0_m) << 8) | (u32)ie_1->avg_cfo_seg0_l;
	psts_1->cfo_avg = (s16)halbb_cnvrt_2_sign(cfo_tmp, 12);
	cfo_tmp = (((u32)ie_1->avg_cfo_premb_seg0_m) << 4) | (u32)ie_1->avg_cfo_premb_seg0_l;
	psts_1->cfo_pab_avg = (s16)halbb_cnvrt_2_sign(cfo_tmp, 12);
#endif
	psts_1->evm_max = ie_1->evm_max;
	psts_1->evm_min = ie_1->evm_min;
	psts_1->snr_avg = ie_1->avg_snr;
	psts_1->cn_avg = ie_1->avg_cn_seg0;
	psts_1->avg_idle_noise_pwr = ie_1->avg_idle_noise_pwr;
	psts_1->pop_idx = ie_1->pop_idx;
	psts_1->rpl_fd = ie_1->rssi_avg_fd;
	//psts_1->bw_idx = ie_1->bw_idx;
	psts_1->rxsc = ie_1->rxsb;
	psts_1->is_su = ie_1->is_su;
	psts_1->is_ldpc = ie_1->is_ldpc;
	psts_1->is_ndp = ie_1->is_ndp;
	psts_1->is_stbc = ie_1->is_stbc;
	psts_1->grant_bt = ie_1->grant_bt_rx;
	psts_1->is_awgn = ie_1->is_awgn;
	psts_1->is_bf = ie_1->is_bf;

	//BB_DBG(bb, DBG_CMN, "[1]cfo_avg=%d, evm_max=%d, evm_min=%d, cn_avg=%d\n",
	//	psts_1->cfo_avg, psts_1->evm_max, psts_1->evm_min, psts_1->cn_avg);
	halbb_mod_rssi_by_path_en(bb, psts_1->rx_path_en);
	physts->rpl_avg = psts_1->rpl_fd;

	return true;
}

bool halbb_7_physts_ie_02(struct bb_info *bb,
			   u8 *addr,
			   u16 ie_length,
			   struct physts_rxd *desc)
{
	struct bb_physts_info	*physts = &bb->bb_physts_i;
	struct bb_physts_rslt_2_info *psts_2 = &physts->bb_physts_rslt_2_i;
	struct physts_ie_2_info *ie_2 = NULL;
	u32 cfo_tmp;
	u32 lt_cfo_buffer_th = (1<<24);
	u32 lt_cfo_abs_tmp_i,lt_cfo_abs_tmp_q;

	if (physts->bb_physts_cnt_i.invalid_he_occur)
		return true;

	ie_2 = (struct physts_ie_2_info *)addr;
	psts_2->ie_02_addr = addr;

	psts_2->max_nsts = ie_2->max_nsts;
	psts_2->ltf_type = ie_2->ltf_type;
	psts_2->gi = ie_2->gi;
	cfo_tmp =  (ie_2->c_cfo_i_m1<<10) | (ie_2->c_cfo_i_m2<<2) | ie_2->c_cfo_i_l;
	psts_2->c_cfo_i = (s32) halbb_cnvrt_2_sign(cfo_tmp, 18);
	psts_2->rx_info_1 = ie_2->rx_info_1;
	psts_2->rx_state_feq = ie_2->rx_state_feq;
	cfo_tmp =  (ie_2->c_cfo_q_m1<<10) | (ie_2->c_cfo_q_m2<<2) | ie_2->c_cfo_q_l;
	psts_2->c_cfo_q = (s32) halbb_cnvrt_2_sign(cfo_tmp, 18);
	psts_2->est_cmped_phase = ie_2->est_cmped_phase;
	psts_2->pkt_extension = ie_2->pkt_extension;
	cfo_tmp =  (ie_2->f_cfo_i_m1<<10) | (ie_2->f_cfo_i_m2<<2) | ie_2->f_cfo_i_l;
	psts_2->f_cfo_i = (s32) halbb_cnvrt_2_sign(cfo_tmp, 18);
	psts_2->n_ltf = ie_2->n_ltf;
	psts_2->n_sym = (ie_2->n_sym_m<<5) | ie_2->n_sym_l;
	cfo_tmp =  (ie_2->f_cfo_q_m1<<10) | (ie_2->f_cfo_q_m2<<2) | ie_2->f_cfo_q_l;
	psts_2->f_cfo_q = (s32) halbb_cnvrt_2_sign(cfo_tmp, 18);
	psts_2->midamble = ie_2->midamble;
	psts_2->is_mu_mimo = ie_2->is_mu_mimo;
	psts_2->is_dl_ofdma = ie_2->is_dl_ofdma;
	psts_2->is_dcm = ie_2->is_dcm;
	psts_2->is_doppler = ie_2->is_doppler;

	physts->l_ltf_cfo_i += psts_2->f_cfo_i;
	physts->l_ltf_cfo_q += psts_2->f_cfo_q;
	lt_cfo_abs_tmp_i = ABS_32(physts->l_ltf_cfo_i);
	lt_cfo_abs_tmp_q = ABS_32(physts->l_ltf_cfo_q);
	if ( lt_cfo_abs_tmp_i > lt_cfo_buffer_th || lt_cfo_abs_tmp_q > lt_cfo_buffer_th ){
		physts->l_ltf_cfo_i = physts->l_ltf_cfo_i >> 16;
		physts->l_ltf_cfo_q = physts->l_ltf_cfo_q >> 16;
	}
	
	return true;
}
bool halbb_7_physts_ie_03(struct bb_info *bb,
			   u8 *addr,
			   u16 ie_length,
			   struct physts_rxd *desc)
{
	struct bb_physts_info	*physts = &bb->bb_physts_i;
	struct bb_physts_rslt_3_info *psts_3 = &physts->bb_physts_rslt_3_i;
	struct physts_ie_3_info *ie_3 = NULL;

	ie_3 = (struct physts_ie_3_info *)addr;
	psts_3->ie_03_addr = addr;

	psts_3->avg_cn_seg1 = ie_3->avg_cn_seg1;
	psts_3->sigval_below_th_tone_cnt_seg1 = ie_3->sigval_below_th_tone_cnt_seg1;
	psts_3->cn_excess_th_tone_cnt_seg1 = ie_3->cn_excess_th_tone_cnt_seg1;
	psts_3->avg_cfo_seg1 = (ie_3->avg_cfo_seg1_m<<8) | ie_3->avg_cfo_seg1_l;
	psts_3->avg_cfo_premb_seg1 = (ie_3->avg_cfo_premb_seg1_m<<8) | ie_3->avg_cfo_premb_seg1_l;
	psts_3->est_cmped_phase_seg1 = ie_3->est_cmped_phase_seg1;
	psts_3->avg_snr_seg1 = ie_3->avg_snr_seg1;
	psts_3->c_cfo_i_seg1 = (ie_3->c_cfo_i_seg1_m1<<10) | (ie_3->c_cfo_i_seg1_m2<<2) | ie_3->c_cfo_i_seg1_l;
	psts_3->c_cfo_q_seg1 = (ie_3->c_cfo_q_seg1_m<<16) | (ie_3->c_cfo_q_seg1_l1<<8) | ie_3->c_cfo_q_seg1_l2;
	psts_3->f_cfo_i_seg1 = (ie_3->f_cfo_i_seg1_m<<14) | (ie_3->f_cfo_i_seg1_lm<<6) | ie_3->f_cfo_i_seg1_l;
	psts_3->f_cfo_q_seg1 = (ie_3->f_cfo_q_seg1_m<<12) | (ie_3->f_cfo_q_seg1_lm<<4) | ie_3->f_cfo_q_seg1_l;
	psts_3->ch_idx_seg1 = ie_3->ch_idx_seg1;
	psts_3->evm_max_seg1 = ie_3->evm_max_seg1;
	psts_3->evm_min_seg1 = ie_3->evm_min_seg1;

	return true;
}

bool halbb_7_physts_ie_04_07(struct bb_info *bb,
			   enum bb_physts_ie_t ie,
			   u8 *addr,
			   u16 ie_length,
			   struct physts_rxd *desc)
{
	struct bb_physts_info	*physts = &bb->bb_physts_i;
	struct bb_physts_rslt_4_to_7_info *psts_r = NULL;
	struct physts_7_ie_4_to_7_info *ie_4_7 = NULL;
	u8 ant_idx_tmp, path = 0;

	if (ie == IE04_CMN_EXT_PATH_A)
		psts_r = &physts->bb_physts_rslt_4_i;
	else if (ie == IE05_CMN_EXT_PATH_B)
		psts_r = &physts->bb_physts_rslt_5_i;
	else if (ie == IE06_CMN_EXT_PATH_C)
		psts_r = &physts->bb_physts_rslt_6_i;
	else if (ie == IE07_CMN_EXT_PATH_D)
		psts_r = &physts->bb_physts_rslt_7_i;
	else
		return false;

	ie_4_7 = (struct physts_7_ie_4_to_7_info *)addr;
	psts_r->ie_04_07_addr = addr;

	ant_idx_tmp = (ie_4_7->ant_idx_msb << 3) | ie_4_7->ant_idx;

	psts_r->ant_idx = ant_idx_tmp;
	psts_r->sig_val_y = (s8)halbb_cnvrt_2_sign(ie_4_7->sig_val_y, 8);
	psts_r->rf_gain_idx = ie_4_7->rf_gain_idx;

	if (bb->ic_type & (BB_RTL8852A | BB_RTL8852B)) {
		path = (u8)SUBTRACT_TO_0(ie, IE04_CMN_EXT_PATH_A);
		psts_r->snr_lgy = halbb_physts_fd_snr_cvrt(bb, path);
	} else {
		psts_r->snr_lgy = ie_4_7->snr_lgy;
	}
	
	psts_r->evm_ss_y = ie_4_7->evm_ss_y;
	psts_r->td_ant_weight = ie_4_7->td_ant_weight;
	psts_r->dc_est_re = (s8)halbb_cnvrt_2_sign(ie_4_7->dc_est_re, 8);
	psts_r->dc_est_im = (s8)halbb_cnvrt_2_sign(ie_4_7->dc_est_im, 8);
	psts_r->rf_tia_gain_idx = ie_4_7->rf_tia_gain_idx;
	psts_r->tia_shrink_indicator = ie_4_7->tia_shrink_indicator;

	return true;
}

bool halbb_7_physts_ie_08(struct bb_info *bb,
			   u8 *addr,
			   u16 ie_length,
			   struct physts_rxd *desc)
{
	struct bb_physts_info	*physts = &bb->bb_physts_i;
	struct bb_physts_rslt_8_info *psts_8 = &physts->bb_physts_rslt_8_i;
	struct physts_ie_8_ch_info *ie_8 = NULL;
	#ifdef HALBB_CH_INFO_SUPPORT
	struct bb_ch_rpt_info *ch_rpt = &bb->bb_ch_rpt_i;
	struct bb_ch_info_raw_info *buf = &ch_rpt->bb_ch_info_raw_i;
	struct bb_ch_info_physts_info *ch_physts = &ch_rpt->bb_ch_info_physts_i;
	struct bb_ch_info_cr_cfg_info *cfg = &ch_rpt->bb_ch_info_cr_cfg_i;
	#endif
	u64 *phy_sts_tmp = NULL;

	ie_8 = (struct physts_ie_8_ch_info *)addr;
	psts_8->ie_08_addr = addr;
	addr += sizeof(struct physts_ie_8_ch_info);

	psts_8->rxsc = ie_8->rxsc;
	psts_8->n_rx = ie_8->n_rx;
	psts_8->n_sts = ie_8->n_sts;
	psts_8->ch_info_len = (ie_8->ch_info_len_m << 2) | ie_8->ch_info_len_l;
	psts_8->evm_1_sts= ie_8->evm_1_sts;
	psts_8->evm_2_sts = ie_8->evm_2_sts;
	psts_8->avg_idle_noise_pwr= ie_8->avg_idle_noise_pwr;
	psts_8->is_ch_info_len_valid= ie_8->is_ch_info_len_valid;
	
	if (!psts_8->is_ch_info_len_valid)
		return true;

	#ifdef BB_8852C_SUPPORT
	/* Fix bug for 52c physts CSI have 3 rsvd, ch_len-8 and shift 64 bits */
	if (bb->ic_type == BB_RTL8852C){
		psts_8->ch_info_len -=8;
		addr += 8; /* shift 64 bits*/
	}
	#endif
	
	halbb_ch_info_modify_ack_rxsc(bb, desc, (u32)psts_8->ch_info_len, &psts_8->rxsc);

	#ifdef HALBB_CH_INFO_SUPPORT

	BB_DBG(bb, DBG_CH_INFO, "[CH] valid_en=%d, len_valid=%d, psts_8->rxsc=%d\n",
	       ch_physts->valid_ch_info_only_en, psts_8->is_ch_info_len_valid, psts_8->rxsc);

	BB_DBG(bb, DBG_CH_INFO, "[CH] cbw=%d, rxsc=%d, rxdesc_rate_idx=%d\n",
	       physts->bb_physts_rslt_1_i.bw_idx, psts_8->rxsc, desc->data_rate);

	BB_DBG(bb, DBG_CH_INFO, "[CH] ch_len=%d\n", psts_8->ch_info_len);


	if (ch_physts->ch_info_state == CH_RPT_START_TO_WAIT ||
	    ch_physts->ch_info_state == CH_RPT_ALWAYS_ON) {

		if (ch_physts->valid_ch_info_only_en) {

			#ifdef BB_8852A_2_SUPPORT
			if (!halbb_ch_info_valid_chk_8852a(bb, desc))
				return true;
			#endif

			/* To avoid all-zero ch_info. due to 6M pkts detected as HE*/
			if (cfg->ch_i_type == BB_CH_MIMO_CH) {
				if (desc->data_rate <= BB_54M) {
					return true;
				}
			}

			#if defined(BB_8852A_2_SUPPORT) || defined(BB_8852C_SUPPORT)
				if (bb->ic_type & (BB_RTL8852A | BB_RTL8852C)) {
					/* To avoid ch_info. with mismatched data_rate & vw*/
					if ((desc->data_rate <= BB_54M) &&
					((physts->bb_physts_rslt_1_i.bw_idx != CHANNEL_WIDTH_20) && (psts_8->rxsc == 0))) {
						return true;
					}
				}
			#endif

		}

		BB_DBG(bb, DBG_CH_INFO, "filter_rxsc_en=%d, rxsc_tgrt_idx=%d\n",
		       ch_physts->filter_rxsc_en, ch_physts->filter_rxsc_tgrt_idx);

		if (ch_physts->filter_rxsc_en &&
		    physts->bb_physts_rslt_8_i.rxsc != ch_physts->filter_rxsc_tgrt_idx) {
		    return true;
		}

		ch_physts->rxsc = psts_8->rxsc;
		ch_physts->n_rx = psts_8->n_rx;
		ch_physts->n_sts = psts_8->n_sts;
		ch_physts->ch_info_len = psts_8->ch_info_len;
		ch_physts->evm_1_sts = psts_8->evm_1_sts;
		ch_physts->evm_2_sts = psts_8->evm_2_sts;
		ch_physts->avg_idle_noise_pwr = psts_8->avg_idle_noise_pwr;
		ch_physts->is_ch_info_len_valid = psts_8->is_ch_info_len_valid;
		ch_physts->gi_ltf = (enum rtw_gi_ltf)desc->gi_ltf;
		ch_physts->data_rate = desc->data_rate;
		ch_physts->bitmap_type_rpt = BIT(physts->bb_physts_rslt_hdr_i.ie_map_type);

		BB_DBG(bb, DBG_CH_INFO, "[CH][SUCCESS] bw_idx=%d, ch_i_grp_num_he=%d\n",
		       physts->bb_physts_rslt_1_i.bw_idx, cfg->ch_i_grp_num_he);

		if (ch_physts->ch_info_state == CH_RPT_START_TO_WAIT)
			ch_physts->ch_info_state = CH_RPT_GETTED;

		psts_8->ch_info_addr = (s16*)addr;

		#if defined(BB_8852A_2_SUPPORT) || defined(BB_8852B_SUPPORT)
		/* Bug_idx=8, tone-1 msb 8 bit is missing, fill with previous mem. content */
		if ((bb->ic_type & (BB_RTL8852A | BB_RTL8852B)) &&
		     cfg->ch_i_cmprs == 0) {
			addr = addr - 2; /* fill the previous 16 bits as meaningless content*/
		}
		#endif

		if (!buf->octet)
		{
			BB_WARNING("[Warning] Memory is not allocated for CH_INFO\n");
			return true;
		}

		if (psts_8->ch_info_len <= buf->ch_info_buf_len)
			halbb_mem_cpy(bb, buf->octet, (s16*)addr, psts_8->ch_info_len);

	}
	#endif

	return true;
}

void halbb_7_physts_ie_09_lgcy(struct bb_info *bb,
			   u8 *addr,
			   u8 rate_mode,
			   struct physts_rxd *desc)
{
	struct physts_ie_9_lgcy_info *ie_9 = NULL;
	struct bb_physts_info	*physts = &bb->bb_physts_i;
	struct bb_physts_rslt_9_info *psts_9 = &physts->bb_physts_rslt_9_i;

	ie_9 = (struct physts_ie_9_lgcy_info *)addr;
	psts_9->ie_09_addr = addr;
	psts_9->l_sig = (ie_9->l_sig_m << 11) | (ie_9->l_sig_lm << 3) | (ie_9->l_sig_l);
}

void halbb_7_physts_ie_09_vht(struct bb_info *bb,
			   u8 *addr,
			   u8 rate_mode,
			   struct physts_rxd *desc)
{
	struct physts_ie_9_vht_info *ie_9 = NULL;
	struct bb_physts_info	*physts = &bb->bb_physts_i;
	struct bb_physts_rslt_9_info *psts_9 = &physts->bb_physts_rslt_9_i;
	
	ie_9 = (struct physts_ie_9_vht_info *)addr;
	psts_9->ie_09_addr = addr;
	psts_9->l_sig = (ie_9->l_sig_m << 11) | (ie_9->l_sig_lm << 3) | (ie_9->l_sig_l);
	psts_9->sig_a1 = (ie_9->sig_a1_m << 18)| (ie_9->sig_a1_lm1 << 10) | (ie_9->sig_a1_lm2 << 2) | (ie_9->sig_a1_l);
	psts_9->sig_a2 = (ie_9->sig_a2_m << 2) | (ie_9->sig_a2_l);
}

void halbb_7_physts_ie_09_he(struct bb_info *bb,
			   u8 *addr,
			   u8 rate_mode,
			   struct physts_rxd *desc)
{
	struct physts_ie_9_he_info *ie_9 = NULL;
	struct bb_physts_info	*physts = &bb->bb_physts_i;
	struct bb_physts_rslt_9_info *psts_9 = &physts->bb_physts_rslt_9_i;
	
	ie_9 = (struct physts_ie_9_he_info *)addr;
	psts_9->ie_09_addr = addr;
	psts_9->l_sig = (ie_9->l_sig_m<<11) | (ie_9->l_sig_lm<<3) | (ie_9->l_sig_l);
	psts_9->sig_a1 = (ie_9->sig_a1_m1<<18) | (ie_9->sig_a1_m2<<10) | (ie_9->sig_a1_m3<<2) | (ie_9->sig_a1_l);
	psts_9->sig_a2 = (ie_9->sig_a2_m << 8) | (ie_9->sig_a2_l);

#ifdef BB_8852A_2_SUPPORT
	if (bb->ic_type == BB_RTL8852A &&
	    bb->hal_com->cv <= CBV) {
		if (psts_9->sig_a1 & BIT7 && /*DCM == 1*/
		    ((psts_9->sig_a1 & 0x3800000) >> 23) == 1 && /*NSTS == 2*/
		    psts_9->sig_a2 & BIT9) { /*STBC == 1*/
		    desc->data_rate &= ~0x70;
		    desc->data_rate |= 0x10; /*HW issue*/
		}
	}
#endif
}

bool halbb_7_physts_ie_09(struct bb_info *bb,
			   u8 *addr,
			   u16 ie_length,
			   struct physts_rxd *desc)
{
	struct bb_physts_info	*physts = &bb->bb_physts_i;
	u8 mode = physts->bb_rate_i.mode;
	
	/* To get rate mode for plcp ie parsing */

	/* allocate different plcp type structure */
	if (mode == BB_LEGACY_MODE) {
		halbb_7_physts_ie_09_lgcy(bb, addr, mode, desc);
	} else if ((mode == BB_HT_MODE) || (mode == BB_VHT_MODE)) {
		halbb_7_physts_ie_09_vht(bb, addr, mode, desc);
	} else if (mode == BB_HE_MODE) {
		halbb_7_physts_ie_09_he(bb, addr, mode, desc);
	}

	return true;
}

bool halbb_7_physts_ie_10(struct bb_info *bb,
			   u8 *addr,
			   u16 ie_length,
			   struct physts_rxd *desc)
{
	struct bb_physts_info	*physts = &bb->bb_physts_i;
	struct bb_physts_rslt_10_info *psts_10 = &physts->bb_physts_rslt_10_i;
	u16 non_sigb_part_size = 64;//sizeof(struct physts_ie_10_cmn_info);

	addr += non_sigb_part_size;

	psts_10->sigb_raw_data_bits_addr = addr;
	psts_10->sigb_len = ie_length - non_sigb_part_size;

	//if (bb->bb_physts_i.print_more_info) {
	//	BB_DBG(bb, DBG_PHY_STS, "[Physts][IE-10] SIG-B len(with zero-pad)=%d\n", sigb_len);
	//	halbb_print_buff_64(bb, psts_10.sigb_raw_data_bits_addr, sigb_len);
	//}

	return true;
}

bool halbb_7_physts_ie_11(struct bb_info *bb,
			   u8 *addr,
			   u16 ie_length,
			   struct physts_rxd *desc)
{
	struct bb_physts_info	*physts = &bb->bb_physts_i;
	struct bb_physts_rslt_11_info *psts_11 = &physts->bb_physts_rslt_11_i;
	struct physts_ie_11_info *ie_11 = NULL;

	ie_11 = (struct physts_ie_11_info *)addr;
	psts_11->ie_11_addr = addr;

	psts_11->l_sig = (ie_11->l_sig_m << 11) | (ie_11->l_sig_lm << 3) | (ie_11->l_sig_l);
	psts_11->sig_a1 = (ie_11->sig_a1_m1 << 18) | (ie_11->sig_a1_m2 << 10) | (ie_11->sig_a1_m3 << 2) | (ie_11->sig_a1_l);
	psts_11->sig_a2 = (ie_11->sig_a2_m<<8) | ie_11->sig_a2_l;
	psts_11->time_stamp = (ie_11->time_stamp_m << 8) | (ie_11->time_stamp_l);
	psts_11->rx_pkt_info_idx = ie_11->rx_pkt_info_idx;
	psts_11->tx_pkt_info_idx = (ie_11->tx_pkt_info_idx_m << 1) | (ie_11->tx_pkt_info_idx_l);

	return true;
}

bool halbb_7_physts_ie_12(struct bb_info *bb,
			   u8 *addr,
			   u16 ie_length,
			   struct physts_rxd *desc)
{
	struct bb_physts_info	*physts = &bb->bb_physts_i;
	struct bb_physts_rslt_12_info *psts_12 = &physts->bb_physts_rslt_12_i;
	
	struct physts_ie_12_cmn_info *physts_c = NULL; /*common*/
	struct physts_ie_12_user_info *physts_u[MU_USER_MAX] = {NULL}; /*per user*/
	u8 i;

	physts_c = (struct physts_ie_12_cmn_info *)addr;
	psts_12->ie_12_addr = addr;
	addr += sizeof(struct physts_ie_12_cmn_info);

	//BB_DBG(bb, DBG_PHY_STS, "[Physts][IE-12]n_user=%d\n",physts_c->n_user);
	psts_12->n_user = physts_c->n_user;

	for (i = 0; i < physts_c->n_user; i++) {
		if (i >= MU_USER_MAX)
			break;
		physts_u[i] = (struct physts_ie_12_user_info *)addr;

		/*
		physts_u[i].sig_val_ss0_seg_cr_user_i = *addr;
		physts_u[i].sig_val_ss1_seg_cr_user_i = *(++addr);
		physts_u[i].sig_val_ss2_seg_cr_user_i = *(++addr);
		physts_u[i].sig_val_ss3_seg_cr_user_i = *(++addr);
		physts_u[i].sig_bad_tone_cnt_seg_cr_user_i = *(++addr);
		physts_u[i].cn_bad_tone_cnt_seg_cr_user_i = *(++addr);
		*/

		psts_12->bb_physts_uer_info[i].cn_bad_tone_cnt_seg_cr_user_i =
			physts_u[i]->cn_bad_tone_cnt_seg_cr_user_i;
		psts_12->bb_physts_uer_info[i].sig_bad_tone_cnt_seg_cr_user_i =
			physts_u[i]->sig_bad_tone_cnt_seg_cr_user_i;
		psts_12->bb_physts_uer_info[i].sig_val_ss0_seg_cr_user_i =
			physts_u[i]->sig_val_ss0_seg_cr_user_i;
		psts_12->bb_physts_uer_info[i].sig_val_ss1_seg_cr_user_i =
			physts_u[i]->sig_val_ss1_seg_cr_user_i;
		psts_12->bb_physts_uer_info[i].sig_val_ss2_seg_cr_user_i =
			physts_u[i]->sig_val_ss2_seg_cr_user_i;
		psts_12->bb_physts_uer_info[i].sig_val_ss3_seg_cr_user_i =
			physts_u[i]->sig_val_ss3_seg_cr_user_i;
		
		addr += sizeof(struct physts_ie_12_user_info);
	}

	return true;
}

bool halbb_7_physts_ie_13(struct bb_info *bb,
			   u8 *addr,
			   u16 ie_length,
			   struct physts_rxd *desc)
{
	struct bb_physts_info	*physts = &bb->bb_physts_i;
	struct bb_physts_rslt_13_info *psts_13 = &physts->bb_physts_rslt_13_i;
	
	struct physts_ie_13_cmn_info_p1 *physts_c = NULL; /*common*/
	struct physts_ie_13_cmn_info_p2 *physts_c2 = NULL; /*common*/
	struct physts_ie_13_user_info *physts_u[MU_USER_MAX] = {NULL}; /*per user*/
	u8 i;
	u16 sta_id_tmp;

	physts_c = (struct physts_ie_13_cmn_info_p1 *)addr;
	psts_13->ie_13_addr = addr;
	addr += sizeof(struct physts_ie_13_cmn_info_p1);

	//BB_DBG(bb, DBG_PHY_STS, "n_user=%d\n", physts_c->n_user);
	psts_13->n_user = physts_c->n_user;

	for (i = 0; i < physts_c->n_user; i++) {
		if (i >= MU_USER_MAX)
			break;

		physts_u[i] = (struct physts_ie_13_user_info *)addr;


		psts_13->bb_physts_uer_info[i].avg_cn_seg_cr = physts_u[i]->avg_cn_seg_cr;
		psts_13->bb_physts_uer_info[i].fec_type = physts_u[i]->fec_type;
		psts_13->bb_physts_uer_info[i].is_awgn	= physts_u[i]->is_awgn;
		psts_13->bb_physts_uer_info[i].is_bf= physts_u[i]->is_bf;
		psts_13->bb_physts_uer_info[i].is_dcm = physts_u[i]->is_dcm;
		psts_13->bb_physts_uer_info[i].is_mu_mimo = physts_u[i]->is_mu_mimo;

		/*use fake MCS & STS when force data type to MU debug mode*/
		if (physts->frc_mu == 2) {
			psts_13->bb_physts_uer_info[i].mcs = physts->tmp_mcs;
			psts_13->bb_physts_uer_info[i].n_sts = physts->tmp_sts;
		} else {
			psts_13->bb_physts_uer_info[i].mcs = physts_u[i]->mcs;
			psts_13->bb_physts_uer_info[i].n_sts = physts_u[i]->n_sts;
		}

		psts_13->bb_physts_uer_info[i].n_sts_ru_total = physts_u[i]->n_sts_ru_total;
		psts_13->bb_physts_uer_info[i].pdp_he_ltf_and_gi_type = physts_u[i]->pdp_he_ltf_and_gi_type;
		psts_13->bb_physts_uer_info[i].pilot_exist = physts_u[i]->pilot_exist;
		psts_13->bb_physts_uer_info[i].ru_alloc = physts_u[i]->ru_alloc;
		psts_13->bb_physts_uer_info[i].rx_evm_max_seg_cr = physts_u[i]->rx_evm_max_seg_cr;
		psts_13->bb_physts_uer_info[i].rx_evm_min_seg_cr = physts_u[i]->rx_evm_min_seg_cr;
		psts_13->bb_physts_uer_info[i].snr = (s8)halbb_cnvrt_2_sign(physts_u[i]->snr,8);
		psts_13->bb_physts_uer_info[i].start_sts = physts_u[i]->start_sts;
		sta_id_tmp = (physts_u[i]->sta_id_m << 8) | physts_u[i]->sta_id_l;
		psts_13->bb_physts_uer_info[i].sta_id = sta_id_tmp;
		
		addr += sizeof(struct physts_ie_13_user_info);
	}

	physts_c2 = (struct physts_ie_13_cmn_info_p2 *)addr;

	psts_13->n_not_sup_sta = physts_c2->n_not_sup_sta;

	return true;
}

bool halbb_7_physts_ie_14(struct bb_info *bb,
			   u8 *addr,
			   u16 ie_length,
			   struct physts_rxd *desc)
{
	struct bb_physts_info	*physts = &bb->bb_physts_i;
	struct bb_physts_rslt_14_info *psts_14 = &physts->bb_physts_rslt_14_i;

	struct physts_ie_14_cmn_info *physts_c = NULL; /*common*/
	struct physts_ie_14_user_info physts_u[TB_USER_MAX]; /*per user*/
	u8 cqi_user_len = 0;
	u8 *cqi_user_addr;
	u8 i;

	physts_c = (struct physts_ie_14_cmn_info *)addr;
	psts_14->ie_14_addr = addr;
	addr += sizeof(struct physts_ie_14_cmn_info);

	psts_14->n_user = physts_c->n_user;
	psts_14->rxinfo_ndp_en= physts_c->rxinfo_ndp_en;

	for (i = 0; i < physts_c->n_user; i++) {
		if (i >= TB_USER_MAX)
			break;
				
		physts_u[i].cqi_bitmap_ul_tb = *addr;
		physts_u[i].cqi_raw_len_ul_tb = *(++addr);
		physts_u[i].cqi_raw_ul_tb_addr = (++addr);
		
		psts_14->bb_physts_uer_info[i].cqi_bitmap_ul_tb = physts_u[i].cqi_bitmap_ul_tb;
		psts_14->bb_physts_uer_info[i].cqi_raw_len_ul_tb = physts_u[i].cqi_raw_len_ul_tb;
		psts_14->bb_physts_uer_info[i].cqi_raw_ul_tb_addr= physts_u[i].cqi_raw_ul_tb_addr;

		
		addr += physts_u[i].cqi_raw_len_ul_tb;
	}
	
	for (i = 0; i < physts_c->n_user; i++) {
		if (i >= TB_USER_MAX)
			break;
		cqi_user_len = physts_u[i].cqi_raw_len_ul_tb;
		cqi_user_addr = physts_u[i].cqi_raw_ul_tb_addr;
		if (bb->bb_physts_i.print_more_info) {
			BB_DBG(bb, DBG_PHY_STS, "[Physts][IE-14][User:%d] len=%d\n", i, cqi_user_len);
			halbb_print_buff_64(bb, cqi_user_addr, cqi_user_len);
		}
	}

	return true;
}

bool halbb_7_physts_ie_15(struct bb_info *bb,
			   u8 *addr,
			   u16 ie_length,
			   struct physts_rxd *desc)
{
	struct bb_physts_info	*physts = &bb->bb_physts_i;
	struct bb_physts_rslt_15_info *psts_15 = &physts->bb_physts_rslt_15_i;
	
	struct physts_ie_15_cmn_info *physts_c = NULL; /*common*/
	struct physts_ie_15_user_info *physts_u[TB_USER_MAX] = {NULL}; /*per user*/
	u8 i;
	u16 rssi_tmp;
	s16 cfo_tmp;

	physts_c = (struct physts_ie_15_cmn_info *)addr;
	psts_15->ie_15_addr = addr;
	addr += sizeof(struct physts_ie_15_cmn_info);

	psts_15->n_user = physts_c->n_user;
	
	for (i = 0; i < physts_c->n_user; i++) {
		if (i >= TB_USER_MAX)
			break;
		physts_u[i] = (struct physts_ie_15_user_info *)addr;

		cfo_tmp = (s16) halbb_cnvrt_2_sign(((physts_u[i]->avg_cfo_seg0_m << 8) | physts_u[i]->avg_cfo_seg0_l), 12);
		rssi_tmp = (physts_u[i]->rssi_m_ul_tb_m << 8) | physts_u[i]->rssi_m_ul_tb_l;

		psts_15->bb_physts_uer_info[i].avg_cn_seg_cr = physts_u[i]->avg_cn_seg_cr;
		psts_15->bb_physts_uer_info[i].fec_type = physts_u[i]->fec_type;
		psts_15->bb_physts_uer_info[i].is_awgn	= physts_u[i]->is_awgn;
		psts_15->bb_physts_uer_info[i].is_bf= physts_u[i]->is_bf;
		psts_15->bb_physts_uer_info[i].is_dcm = physts_u[i]->is_dcm;
		psts_15->bb_physts_uer_info[i].is_mu_mimo = physts_u[i]->is_mu_mimo;
		psts_15->bb_physts_uer_info[i].mcs = physts_u[i]->mcs;
		psts_15->bb_physts_uer_info[i].n_sts = physts_u[i]->n_sts;
		psts_15->bb_physts_uer_info[i].n_sts_ru_total = physts_u[i]->n_sts_ru_total;
		psts_15->bb_physts_uer_info[i].pdp_he_ltf_and_gi_type = physts_u[i]->pdp_he_ltf_and_gi_type;
		psts_15->bb_physts_uer_info[i].pilot_exist = physts_u[i]->pilot_exist;
		psts_15->bb_physts_uer_info[i].ru_alloc = physts_u[i]->ru_alloc;
		psts_15->bb_physts_uer_info[i].rx_evm_max_seg_cr = physts_u[i]->rx_evm_max_seg_cr;
		psts_15->bb_physts_uer_info[i].rx_evm_min_seg_cr = physts_u[i]->rx_evm_min_seg_cr;
		psts_15->bb_physts_uer_info[i].snr = physts_u[i]->snr;
		psts_15->bb_physts_uer_info[i].start_sts = physts_u[i]->start_sts;
                                      
		psts_15->bb_physts_uer_info[i].uid = physts_u[i]->uid;
		psts_15->bb_physts_uer_info[i].avg_cfo_seg0 = cfo_tmp;
		psts_15->bb_physts_uer_info[i].rssi_m_ul_tb = rssi_tmp;

		
		addr += sizeof(struct physts_ie_15_user_info);
	}

	return true;
}

bool halbb_7_physts_ie_17(struct bb_info *bb,
			   u8 *addr,
			   u16 ie_length,
			   struct physts_rxd *desc)
{
	struct physts_ie_17_cmn_info *physts_c = NULL; /*common*/
	struct physts_ie_17_user_info *physts_u[TB_USER_MAX] = {NULL}; /*per user*/
	struct bb_physts_info	*physts = &bb->bb_physts_i;
	struct bb_physts_rslt_17_info *psts_17 = &physts->bb_physts_rslt_17_i;
	u8 i;
	u8 n_user;

	physts_c = (struct physts_ie_17_cmn_info *)addr;
	psts_17->ie_17_addr = addr;
	addr += sizeof(struct physts_ie_17_cmn_info);

	psts_17->bb_physts_cmn_info.stbc_en = physts_c->stbc_en;	
	psts_17->bb_physts_cmn_info.ldpc_extra = physts_c->ldpc_extra;
	psts_17->bb_physts_cmn_info.doppler_en = physts_c->doppler_en;
	psts_17->bb_physts_cmn_info.midamle_mode = physts_c->midamle_mode;	
	psts_17->bb_physts_cmn_info.gi_type = physts_c->gi_type;	
	psts_17->bb_physts_cmn_info.ltf_type = physts_c->ltf_type;
	psts_17->bb_physts_cmn_info.n_ltf = physts_c->n_ltf;
	psts_17->bb_physts_cmn_info.n_sym = (physts_c->n_sym_m<<5)|physts_c->n_sym_l;
	psts_17->bb_physts_cmn_info.pe_idx = ((physts_c->pe_idx_m<<2)|physts_c->pe_idx_l);
	psts_17->bb_physts_cmn_info.pre_fec_factor = physts_c->pre_fec_factor;
	psts_17->bb_physts_cmn_info.n_usr = ((physts_c->n_user_m<<5)|physts_c->n_user_l);
	psts_17->bb_physts_cmn_info.mumimo_ltf_mode_en = physts_c->mumimo_ltf_mode_en;
	psts_17->bb_physts_cmn_info.ndp = physts_c->ndp;
	psts_17->bb_physts_cmn_info.pri_exp_rssi_dbm = ((physts_c->pri_exp_rssi_dbm_m<<3)|physts_c->pri_exp_rssi_dbm_l);
	psts_17->bb_physts_cmn_info.dbw_idx = physts_c->dbw_idx;
	psts_17->bb_physts_cmn_info.rxtime = (physts_c->rxtime_m<<8)|physts_c->rxtime_l;

	
	n_user = (physts_c->n_user_m<<4) | physts_c->n_user_l;
	psts_17->n_user = n_user;
	
	//BB_DBG(bb, DBG_PHY_STS, "[Physts][IE-17]n_user=%d\n", n_user);

	for (i = 0; i < n_user; i++) {
		if (i >= TB_USER_MAX)
			break;
		physts_u[i] = (struct physts_ie_17_user_info *)addr;
		
		psts_17->bb_physts_uer_info[i].u_id= physts_u[i]->u_id;
		psts_17->bb_physts_uer_info[i].ru_alloc= physts_u[i]->ru_alloc;
		psts_17->bb_physts_uer_info[i].n_sts_ru_tot= physts_u[i]->n_sts_ru_tot;
		psts_17->bb_physts_uer_info[i].strt_sts= physts_u[i]->strt_sts;
		psts_17->bb_physts_uer_info[i].n_sts= physts_u[i]->n_sts;
		psts_17->bb_physts_uer_info[i].fec_type= physts_u[i]->fec_type;
		psts_17->bb_physts_uer_info[i].mcs= physts_u[i]->mcs;
		psts_17->bb_physts_uer_info[i].dcm_en= (bool)physts_u[i]->dcm_en;

		addr += sizeof(struct physts_ie_17_user_info);
	}

	return true;
}

bool halbb_7_physts_ie_18(struct bb_info *bb,
			   u8 *addr,
			   u16 ie_length,
			   struct physts_rxd *desc)
{
	struct bb_physts_info	*physts = &bb->bb_physts_i;
	struct bb_physts_rslt_18_info *psts_18 = &physts->bb_physts_rslt_18_i;
	struct physts_ie_18_info *ie_18 = NULL;

	ie_18 = (struct physts_ie_18_info *)addr;
	psts_18->ie_18_addr = addr;

	psts_18->rx_time = (ie_18->rx_time_m<<8) | ie_18->rx_time_l;
	psts_18->ch_len_lgcy_seg0 = ie_18->ch_len_lgcy_seg0;
	psts_18->bw_det_seg0 = ie_18->bw_det_seg0;
	psts_18->snr_idx_lgcy_seg0 = ie_18->snr_idx_lgcy_seg0;
	psts_18->pdp_idx_lgcy_seg0 = ie_18->pdp_idx_lgcy_seg0;
	psts_18->pfd_flow = (ie_18->pfd_flow_m<<8) | ie_18->pfd_flow_l;
	psts_18->ch_len_lgcy_seg1 = ie_18->ch_len_lgcy_seg1;
	psts_18->bw_det_seg1 = ie_18->bw_det_seg1;
	psts_18->snr_idx_lgcy_seg1 = ie_18->snr_idx_lgcy_seg1;
	psts_18->pdp_idx_lgcy_seg1 = ie_18->pdp_idx_lgcy_seg1;
	psts_18->is_seg1_exist = ie_18->is_seg1_exist;

	return true;
}

bool halbb_7_physts_ie_19(struct bb_info *bb,
			   u8 *addr,
			   u16 ie_length,
			   struct physts_rxd *desc)
{
	struct bb_physts_info	*physts = &bb->bb_physts_i;
	struct bb_physts_rslt_19_info *psts_19 = &physts->bb_physts_rslt_19_i;
	struct physts_ie_19_info *ie_19 = NULL;

	ie_19 = (struct physts_ie_19_info *)addr;
	psts_19->ie_19_addr = addr;

	psts_19->ppdu_inpwrdbm_p20 = ie_19->ppdu_inpwrdbm_p20;
	psts_19->ppdu_inpwrdbm_s20 = ie_19->ppdu_inpwrdbm_s20;
	psts_19->ppdu_inpwrdbm_s40 = ie_19->ppdu_inpwrdbm_s40;
	psts_19->ppdu_inpwrdbm_s80 = ie_19->ppdu_inpwrdbm_s80;
	psts_19->ppdu_inpwrdbm_per20_1 = ie_19->ppdu_inpwrdbm_per20_1;
	psts_19->ppdu_inpwrdbm_per20_2 = ie_19->ppdu_inpwrdbm_per20_2;
	psts_19->ppdu_inpwrdbm_per20_3 = ie_19->ppdu_inpwrdbm_per20_3;
	psts_19->ppdu_inpwrdbm_per20_4 = ie_19->ppdu_inpwrdbm_per20_4;
	psts_19->edcca_rpt_cnt_p20 = ie_19->edcca_rpt_cnt_p20;
	psts_19->edcca_rpt_p20_max = ie_19->edcca_rpt_p20_max;
	psts_19->edcca_rpt_p20_min = ie_19->edcca_rpt_p20_min;
	psts_19->edcca_total_smp_cnt = ie_19->edcca_total_smp_cnt;
	psts_19->edcca_rpt_cnt_s80 = ie_19->edcca_rpt_cnt_s80;
	psts_19->edcca_rpt_cnt_s80_max = ie_19->edcca_rpt_cnt_s80_max;
	psts_19->edcca_rpt_cnt_s80_min = ie_19->edcca_rpt_cnt_s80_min;
	psts_19->pop_reg_pwr = ie_19->pop_reg_pwr;
	psts_19->pop_trig_pwr = ie_19->pop_trig_pwr;
	psts_19->early_drop_pwr = ie_19->early_drop_pwr;
	psts_19->tx_over_flow = ie_19->tx_over_flow;

	return true;
}

bool halbb_7_physts_ie_20(struct bb_info *bb,
			   u8 *addr,
			   u16 ie_length,
			   struct physts_rxd *desc)
{
	struct bb_physts_info	*physts = &bb->bb_physts_i;
	struct bb_physts_rslt_20_info *psts_20 = &physts->bb_physts_rslt_20_i;
	
	struct physts_ie_20_cmn_info *physts_c = NULL; /*common*/
	struct physts_ie_20_user_info *physts_u[TB_USER_MAX] = {NULL}; /*per user*/
	u8 i;

	physts_c = (struct physts_ie_20_cmn_info *)addr;
	psts_20->ie_20_addr = addr;
	addr += sizeof(struct physts_ie_20_cmn_info);

	//BB_DBG(bb, DBG_PHY_STS, "[Physts][IE-20]n_user=%d\n", physts_c->n_user);

	for (i = 0; i < physts_c->n_user; i++) {
		if (i >= TB_USER_MAX)
			break;
		physts_u[i] = (struct physts_ie_20_user_info *)addr;
		addr += sizeof(struct physts_ie_20_user_info);

			/* mapping to the bb struct */	
		psts_20->bb_physts_usr_info[i].ch_smo_n_block_lgcy_sub_0_seg_0 =
		physts_u[i]->ch_smo_n_block_lgcy_sub_0_seg_0;
		psts_20->bb_physts_usr_info[i].ch_smo_n_block_lgcy_sub_1_seg_0 =
			physts_u[i]->ch_smo_n_block_lgcy_sub_1_seg_0;
		psts_20->bb_physts_usr_info[i].ch_smo_n_block_lgcy_sub_2_seg_0 =
			physts_u[i]->ch_smo_n_block_lgcy_sub_2_seg_0;
		psts_20->bb_physts_usr_info[i].ch_smo_n_block_lgcy_sub_3_seg_0 =
			physts_u[i]->ch_smo_n_block_lgcy_sub_3_seg_0;
		psts_20->bb_physts_usr_info[i].ch_smo_en_lgcy_seg_0 =
			physts_u[i]->ch_smo_en_lgcy_seg_0;
		psts_20->bb_physts_usr_info[i].ch_smo_en_non_lgcy_seg_0 =
			physts_u[i]->ch_smo_en_non_lgcy_seg_0;
		psts_20->bb_physts_usr_info[i].ch_smo_n_block_non_lgcy_seg_0 =
			physts_u[i]->ch_smo_n_block_non_lgcy_seg_0;
		psts_20->bb_physts_usr_info[i].ch_len_non_lgcy_sts_0_seg_0 =
			physts_u[i]->ch_len_non_lgcy_sts_0_seg_0;
		psts_20->bb_physts_usr_info[i].ch_len_non_lgcy_sts_1_seg_0 =
			physts_u[i]->ch_len_non_lgcy_sts_1_seg_0;
		psts_20->bb_physts_usr_info[i].ch_len_non_lgcy_sts_2_seg_0 =
			physts_u[i]->ch_len_non_lgcy_sts_2_seg_0;
		psts_20->bb_physts_usr_info[i].ch_len_non_lgcy_sts_3_seg_0 =
			physts_u[i]->ch_len_non_lgcy_sts_3_seg_0;
		
		psts_20->bb_physts_usr_info[i].snr_idx_non_lgy_sts_0_seg_0 =
			physts_u[i]->snr_idx_non_lgy_sts_0_seg_0;
		psts_20->bb_physts_usr_info[i].snr_idx_non_lgy_sts_1_seg_0 =
			physts_u[i]->snr_idx_non_lgy_sts_1_seg_0;
		psts_20->bb_physts_usr_info[i].snr_idx_non_lgy_sts_2_seg_0 = 
		(physts_u[i]->snr_idx_non_lgy_sts_2_seg_0_m<<2) | 
		 physts_u[i]->snr_idx_non_lgy_sts_2_seg_0_l;
		psts_20->bb_physts_usr_info[i].snr_idx_non_lgy_sts_3_seg_0 =
			physts_u[i]->snr_idx_non_lgy_sts_3_seg_0;
		
		psts_20->bb_physts_usr_info[i].pdp_idx_non_lgcy_sts_0_seg_0 =
			physts_u[i]->pdp_idx_non_lgcy_sts_0_seg_0;
		psts_20->bb_physts_usr_info[i].pdp_idx_non_lgcy_sts_1_seg_0 = 
		(physts_u[i]->pdp_idx_non_lgcy_sts_1_seg_0_m<<1) | 
		 physts_u[i]->pdp_idx_non_lgcy_sts_1_seg_0_l;
		psts_20->bb_physts_usr_info[i].pdp_idx_non_lgcy_sts_2_seg_0 =
			physts_u[i]->pdp_idx_non_lgcy_sts_2_seg_0;
		psts_20->bb_physts_usr_info[i].pdp_idx_non_lgcy_sts_3_seg_0 =
			physts_u[i]->pdp_idx_non_lgcy_sts_3_seg_0;
		
		psts_20->bb_physts_usr_info[i].snr_non_lgy_sts_0_seg_0 =
			physts_u[i]->snr_non_lgy_sts_0_seg_0;
		psts_20->bb_physts_usr_info[i].snr_non_lgy_sts_1_seg_0 =
			physts_u[i]->snr_non_lgy_sts_1_seg_0;
		psts_20->bb_physts_usr_info[i].snr_non_lgy_sts_2_seg_0 =
			physts_u[i]->snr_non_lgy_sts_2_seg_0;
		psts_20->bb_physts_usr_info[i].snr_non_lgy_sts_3_seg_0 =
			physts_u[i]->snr_non_lgy_sts_3_seg_0;

		psts_20->bb_physts_usr_info[i].evm_ss_0_seg_0 =
			physts_u[i]->evm_ss_0_seg_0;
		psts_20->bb_physts_usr_info[i].evm_ss_1_seg_0 =
			physts_u[i]->evm_ss_1_seg_0;
		psts_20->bb_physts_usr_info[i].evm_ss_2_seg_0 =
			physts_u[i]->evm_ss_2_seg_0;
		psts_20->bb_physts_usr_info[i].evm_ss_3_seg_0 =
			physts_u[i]->evm_ss_3_seg_0;
	}

	return true;
}

bool halbb_7_physts_ie_21(struct bb_info *bb,
			   u8 *addr,
			   u16 ie_length,
			   struct physts_rxd *desc)
{
	struct bb_physts_info	*physts = &bb->bb_physts_i;
	struct bb_physts_rslt_21_info *psts_21 = &physts->bb_physts_rslt_21_i;
	
	struct physts_ie_21_cmn_info *physts_c = NULL; /*common*/
	struct physts_ie_21_user_info *physts_u[TB_USER_MAX] = {NULL}; /*per user*/
	u8 i;

	physts_c = (struct physts_ie_21_cmn_info *)addr;
	psts_21->ie_21_addr = addr;
	addr += sizeof(struct physts_ie_21_cmn_info);

	//BB_DBG(bb, DBG_PHY_STS, "[Physts][IE-21]n_user=%d\n", physts_c->n_user);

	for (i = 0; i < physts_c->n_user; i++) {
		if (i >= TB_USER_MAX)
			break;
		physts_u[i] = (struct physts_ie_21_user_info *)addr;
		addr += sizeof(struct physts_ie_21_user_info);

			/* mapping to the bb struct */	
		psts_21->bb_physts_usr_info[i].ch_smo_n_block_lgcy_sub_0_seg_1 =
		physts_u[i]->ch_smo_n_block_lgcy_sub_0_seg_1;
		psts_21->bb_physts_usr_info[i].ch_smo_n_block_lgcy_sub_1_seg_1 =
			physts_u[i]->ch_smo_n_block_lgcy_sub_1_seg_1;
		psts_21->bb_physts_usr_info[i].ch_smo_n_block_lgcy_sub_2_seg_1 =
			physts_u[i]->ch_smo_n_block_lgcy_sub_2_seg_1;
		psts_21->bb_physts_usr_info[i].ch_smo_n_block_lgcy_sub_3_seg_1 =
			physts_u[i]->ch_smo_n_block_lgcy_sub_3_seg_1;
		psts_21->bb_physts_usr_info[i].ch_smo_en_lgcy_seg_1 =
			physts_u[i]->ch_smo_en_lgcy_seg_1;
		psts_21->bb_physts_usr_info[i].ch_smo_en_non_lgcy_seg_1 =
			physts_u[i]->ch_smo_en_non_lgcy_seg_1;
		psts_21->bb_physts_usr_info[i].ch_smo_n_block_non_lgcy_seg_1 =
			physts_u[i]->ch_smo_n_block_non_lgcy_seg_1;
		psts_21->bb_physts_usr_info[i].ch_len_non_lgcy_sts_0_seg_1 =
			physts_u[i]->ch_len_non_lgcy_sts_0_seg_1;
		psts_21->bb_physts_usr_info[i].ch_len_non_lgcy_sts_1_seg_1 =
			physts_u[i]->ch_len_non_lgcy_sts_1_seg_1;
		psts_21->bb_physts_usr_info[i].ch_len_non_lgcy_sts_2_seg_1 =
			physts_u[i]->ch_len_non_lgcy_sts_2_seg_1;
		psts_21->bb_physts_usr_info[i].ch_len_non_lgcy_sts_3_seg_1 =
			physts_u[i]->ch_len_non_lgcy_sts_3_seg_1;
		
		psts_21->bb_physts_usr_info[i].snr_idx_non_lgy_sts_0_seg_1 =
			physts_u[i]->snr_idx_non_lgy_sts_0_seg_1;
		psts_21->bb_physts_usr_info[i].snr_idx_non_lgy_sts_1_seg_1 =
			physts_u[i]->snr_idx_non_lgy_sts_1_seg_1;
		psts_21->bb_physts_usr_info[i].snr_idx_non_lgy_sts_2_seg_1 = 
		(physts_u[i]->snr_idx_non_lgy_sts_2_seg_1_m<<2) | 
		 physts_u[i]->snr_idx_non_lgy_sts_2_seg_1_l;
		psts_21->bb_physts_usr_info[i].snr_idx_non_lgy_sts_3_seg_1 =
			physts_u[i]->snr_idx_non_lgy_sts_3_seg_1;
		
		psts_21->bb_physts_usr_info[i].pdp_idx_non_lgcy_sts_0_seg_1 =
			physts_u[i]->pdp_idx_non_lgcy_sts_0_seg_1;
		psts_21->bb_physts_usr_info[i].pdp_idx_non_lgcy_sts_1_seg_1 = 
		(physts_u[i]->pdp_idx_non_lgcy_sts_1_seg_1_m<<1) | 
		 physts_u[i]->pdp_idx_non_lgcy_sts_1_seg_1_l;
		psts_21->bb_physts_usr_info[i].pdp_idx_non_lgcy_sts_2_seg_1 =
			physts_u[i]->pdp_idx_non_lgcy_sts_2_seg_1;
		psts_21->bb_physts_usr_info[i].pdp_idx_non_lgcy_sts_3_seg_1 =
			physts_u[i]->pdp_idx_non_lgcy_sts_3_seg_1;
		
		psts_21->bb_physts_usr_info[i].snr_non_lgy_sts_0_seg_1 =
			physts_u[i]->snr_non_lgy_sts_0_seg_1;
		psts_21->bb_physts_usr_info[i].snr_non_lgy_sts_1_seg_1 =
			physts_u[i]->snr_non_lgy_sts_1_seg_1;
		psts_21->bb_physts_usr_info[i].snr_non_lgy_sts_2_seg_1 =
			physts_u[i]->snr_non_lgy_sts_2_seg_1;
		psts_21->bb_physts_usr_info[i].snr_non_lgy_sts_3_seg_1 =
			physts_u[i]->snr_non_lgy_sts_3_seg_1;

		psts_21->bb_physts_usr_info[i].evm_ss_0_seg_1 =
			physts_u[i]->evm_ss_0_seg_1;
		psts_21->bb_physts_usr_info[i].evm_ss_1_seg_1 =
			physts_u[i]->evm_ss_1_seg_1;
		psts_21->bb_physts_usr_info[i].evm_ss_2_seg_1 =
			physts_u[i]->evm_ss_2_seg_1;
		psts_21->bb_physts_usr_info[i].evm_ss_3_seg_1 =
			physts_u[i]->evm_ss_3_seg_1;
	}

	return true;
}

bool halbb_7_physts_ie_22(struct bb_info *bb,
			   u8 *addr,
			   u16 ie_length,
			   struct physts_rxd *desc)
{
	struct physts_ie_22_cmn_info *physts_c = NULL; /*common*/
	struct physts_ie_22_user_info *physts_u[TB_USER_MAX] = {NULL}; /*per user*/
	u8 i;

	physts_c = (struct physts_ie_22_cmn_info *)addr;
	addr += sizeof(struct physts_ie_22_cmn_info);

	//BB_DBG(bb, DBG_PHY_STS, "[Physts][IE-22] n_user=%d\n", physts_c->n_user);

	for (i = 0; i < physts_c->n_user; i++) {
		if (i >= TB_USER_MAX)
			break;
		physts_u[i] = (struct physts_ie_22_user_info *)addr;
		addr += sizeof(struct physts_ie_22_user_info);
	}

	return true;
}

bool halbb_7_physts_ie_24(struct bb_info *bb,
			   u8 *addr,
			   u16 ie_length,
			   struct physts_rxd *desc)
{
	struct bb_physts_info	*physts = &bb->bb_physts_i;
	struct bb_physts_rslt_24_info *psts_24 = &physts->bb_physts_rslt_24_i;
	struct physts_ie_24_info *ie_24 = NULL;

	ie_24 = (struct physts_ie_24_info *)addr;
	psts_24->ie_24_addr = addr;

	psts_24->pre_agc_step_a = ie_24->pre_agc_step_a;
	psts_24->l_fine_agc_step_a = ie_24->l_fine_agc_step_a;
	psts_24->ht_fine_agc_step_a = ie_24->ht_fine_agc_step_a;
	psts_24->pre_gain_code_a = ie_24->pre_gain_code_a;
	psts_24->l_fine_gain_code_a = ie_24->l_fine_gain_code_a;
	psts_24->ht_fine_gain_code_a = ie_24->ht_fine_gain_code_a;
	psts_24->l_dagc_a = ie_24->l_dagc_a;
	psts_24->ht_dagc_a = ie_24->ht_dagc_a;
	psts_24->pre_ibpwrdbm_a = ie_24->pre_ibpwrdbm_a;
	psts_24->pre_wbpwrdbm_a = ie_24->pre_wbpwrdbm_a;
	psts_24->l_ibpwrdbm_a = ie_24->l_ibpwrdbm_a;
	psts_24->l_wbpwrdbm_a = ie_24->l_wbpwrdbm_a;
	psts_24->ht_ibpwrdbm_a = ie_24->ht_ibpwrdbm_a;
	psts_24->ht_wbpwrdbm_a = ie_24->ht_wbpwrdbm_a;
	psts_24->l_dig_ibpwrdbm_a = ie_24->l_dig_ibpwrdbm_a;
	psts_24->ht_dig_ibpwrdbm_a = ie_24->ht_dig_ibpwrdbm_a;
	psts_24->lna_inpwrdbm_a = ie_24->lna_inpwrdbm_a;
	psts_24->aci2sig_db = ie_24->aci2sig_db;
	psts_24->sb5m_ratio_0 = ie_24->sb5m_ratio_0;
	psts_24->sb5m_ratio_1 = ie_24->sb5m_ratio_1;
	psts_24->sb5m_ratio_2 = ie_24->sb5m_ratio_2;
	psts_24->sb5m_ratio_3 = ie_24->sb5m_ratio_3;
	psts_24->aci_indicator_a = ie_24->aci_indicator_a;
	psts_24->tia_shrink_indicator_a = ie_24->tia_shrink_indicator_a;
	psts_24->pre_gain_code_tia_a = ie_24->pre_gain_code_tia_a;
	psts_24->l_fine_gain_code_tia_a = ie_24->l_fine_gain_code_tia_a;
	psts_24->ht_fine_gain_code_tia_a = ie_24->ht_fine_gain_code_tia_a;
	psts_24->aci_det = ie_24->aci_det;

	BB_DBG(bb, DBG_PHY_STS, "[aci_det / aci_indicator_a / aci2sig_db] = [%d, %d, %d]\n",
	       psts_24->aci_det, psts_24->aci_indicator_a, psts_24->aci2sig_db);

	return true;
}

bool halbb_7_physts_ie_25(struct bb_info *bb,
			   u8 *addr,
			   u16 ie_length,
			   struct physts_rxd *desc)
{
	struct bb_physts_info	*physts = &bb->bb_physts_i;
	struct bb_physts_rslt_25_info *psts_25 = &physts->bb_physts_rslt_25_i;
	struct physts_ie_25_info *ie_25 = NULL;

	ie_25 = (struct physts_ie_25_info *)addr;
	psts_25->ie_25_addr = addr;

	psts_25->pre_agc_step_b = ie_25->pre_agc_step_b;
	psts_25->l_fine_agc_step_b = ie_25->l_fine_agc_step_b;
	psts_25->ht_fine_agc_step_b = ie_25->ht_fine_agc_step_b;
	psts_25->pre_gain_code_b = ie_25->pre_gain_code_b;
	psts_25->l_fine_gain_code_b = ie_25->l_fine_gain_code_b;
	psts_25->ht_fine_gain_code_b = ie_25->ht_fine_gain_code_b;
	psts_25->l_dagc_b = ie_25->l_dagc_b;
	psts_25->ht_dagc_b = ie_25->ht_dagc_b;
	psts_25->pre_ibpwrdbm_b = ie_25->pre_ibpwrdbm_b;
	psts_25->pre_wbpwrdbm_b = ie_25->pre_wbpwrdbm_b;
	psts_25->l_ibpwrdbm_b = ie_25->l_ibpwrdbm_b;
	psts_25->l_wbpwrdbm_b = ie_25->l_wbpwrdbm_b;
	psts_25->ht_ibpwrdbm_b = ie_25->ht_ibpwrdbm_b;
	psts_25->ht_wbpwrdbm_b = ie_25->ht_wbpwrdbm_b;
	psts_25->l_dig_ibpwrdbm_b = ie_25->l_dig_ibpwrdbm_b;
	psts_25->ht_dig_ibpwrdbm_b = ie_25->ht_dig_ibpwrdbm_b;
	psts_25->lna_inpwrdbm_b = ie_25->lna_inpwrdbm_b;
	psts_25->aci2sig_db = ie_25->aci2sig_db;
	psts_25->sb5m_ratio_0 = ie_25->sb5m_ratio_0;
	psts_25->sb5m_ratio_1 = ie_25->sb5m_ratio_1;
	psts_25->sb5m_ratio_2 = ie_25->sb5m_ratio_2;
	psts_25->sb5m_ratio_3 = ie_25->sb5m_ratio_3;
	psts_25->aci_indicator_b = ie_25->aci_indicator_b;
	psts_25->tia_shrink_indicator_b = ie_25->tia_shrink_indicator_b;
	psts_25->pre_gain_code_tia_b = ie_25->pre_gain_code_tia_b;
	psts_25->l_fine_gain_code_tia_b = ie_25->l_fine_gain_code_tia_b;
	psts_25->ht_fine_gain_code_tia_b = ie_25->ht_fine_gain_code_tia_b;
	psts_25->aci_det = ie_25->aci_det;

	return true;
}

bool halbb_7_physts_ie_26(struct bb_info *bb,
			   u8 *addr,
			   u16 ie_length,
			   struct physts_rxd *desc)
{
	struct bb_physts_info	*physts = &bb->bb_physts_i;
	struct physts_ie_26_info *ie_26 = NULL;

	ie_26 = (struct physts_ie_26_info *)addr;

	return true;
}

bool halbb_7_physts_ie_27(struct bb_info *bb,
			   u8 *addr,
			   u16 ie_length,
			   struct physts_rxd *desc)
{
	struct bb_physts_info	*physts = &bb->bb_physts_i;
	struct physts_ie_27_info *ie_27 = NULL;

	ie_27 = (struct physts_ie_27_info *)addr;

	return true;
}

bool halbb_7_physts_ie_28(struct bb_info *bb,
			   u8 *addr,
			   u16 ie_length,
			   struct physts_rxd *desc)
{
	struct bb_physts_info	*physts = &bb->bb_physts_i;
	struct bb_physts_rslt_28_info *psts_28 = &physts->bb_physts_rslt_28_i;
	struct physts_ie_28_info *ie_28 = NULL;

	ie_28 = (struct physts_ie_28_info *)addr;
	psts_28->ie_28_addr = addr;

	psts_28->ant_weight_a = (ie_28->ant_weight_a_m<<1) | ie_28->ant_weight_a_l;
	psts_28->h3_real_a = ie_28->h3_real_a;
	psts_28->h3_imag_a = ie_28->h3_imag_a;
	psts_28->h4_real_a = ie_28->h4_real_a;
	psts_28->h4_imag_a = ie_28->h4_imag_a;
	psts_28->h5_real_a = ie_28->h5_real_a;
	psts_28->h5_imag_a = ie_28->h5_imag_a;
	psts_28->h6_real_a = ie_28->h6_real_a;
	psts_28->h6_imag_a = ie_28->h6_imag_a;
	psts_28->h7_real_a = ie_28->h7_real_a;
	psts_28->h7_imag_a = ie_28->h7_imag_a;
	psts_28->h8_real_a = ie_28->h8_real_a;
	psts_28->h8_imag_a = ie_28->h8_imag_a;
	psts_28->h9_real_a = ie_28->h9_real_a;
	psts_28->h9_imag_a = ie_28->h9_imag_a;
	psts_28->h10_real_a = ie_28->h10_real_a;
	psts_28->h10_imag_a = ie_28->h10_imag_a;
	psts_28->h11_real_a = ie_28->h11_real_a;
	psts_28->h11_imag_a = ie_28->h11_imag_a;
	psts_28->h12_real_a = ie_28->h12_real_a;
	psts_28->h12_imag_a = ie_28->h12_imag_a;
	psts_28->h13_real_a = ie_28->h13_real_a;
	psts_28->h13_imag_a = ie_28->h13_imag_a;
	psts_28->h14_real_a = ie_28->h14_real_a;
	psts_28->h14_imag_a = ie_28->h14_imag_a;
	psts_28->h15_real_a = ie_28->h15_real_a;
	psts_28->h15_imag_a = ie_28->h15_imag_a;
	psts_28->h16_real_a = ie_28->h16_real_a;
	psts_28->h16_imag_a = ie_28->h16_imag_a;
	psts_28->h17_real_a = ie_28->h17_real_a;
	psts_28->h17_imag_a = ie_28->h17_imag_a;

	return true;
}

bool halbb_7_physts_ie_29(struct bb_info *bb,
			   u8 *addr,
			   u16 ie_length,
			   struct physts_rxd *desc)
{
	struct bb_physts_info	*physts = &bb->bb_physts_i;
	struct bb_physts_rslt_29_info *psts_29 = &physts->bb_physts_rslt_29_i;
	struct physts_ie_29_info *ie_29 = NULL;

	ie_29 = (struct physts_ie_29_info *)addr;
	psts_29->ie_29_addr = addr;

	psts_29->ant_weight_b = (ie_29->ant_weight_b_m<<1) | ie_29->ant_weight_b_l;
	psts_29->h3_real_b = ie_29->h3_real_b;
	psts_29->h3_imag_b = ie_29->h3_imag_b;
	psts_29->h4_real_b = ie_29->h4_real_b;
	psts_29->h4_imag_b = ie_29->h4_imag_b;
	psts_29->h5_real_b = ie_29->h5_real_b;
	psts_29->h5_imag_b = ie_29->h5_imag_b;
	psts_29->h6_real_b = ie_29->h6_real_b;
	psts_29->h6_imag_b = ie_29->h6_imag_b;
	psts_29->h7_real_b = ie_29->h7_real_b;
	psts_29->h7_imag_b = ie_29->h7_imag_b;
	psts_29->h8_real_b = ie_29->h8_real_b;
	psts_29->h8_imag_b = ie_29->h8_imag_b;
	psts_29->h9_real_b = ie_29->h9_real_b;
	psts_29->h9_imag_b = ie_29->h9_imag_b;
	psts_29->h10_real_b = ie_29->h10_real_b;
	psts_29->h10_imag_b = ie_29->h10_imag_b;
	psts_29->h11_real_b = ie_29->h11_real_b;
	psts_29->h11_imag_b = ie_29->h11_imag_b;
	psts_29->h12_real_b = ie_29->h12_real_b;
	psts_29->h12_imag_b = ie_29->h12_imag_b;
	psts_29->h13_real_b = ie_29->h13_real_b;
	psts_29->h13_imag_b = ie_29->h13_imag_b;
	psts_29->h14_real_b = ie_29->h14_real_b;
	psts_29->h14_imag_b = ie_29->h14_imag_b;
	psts_29->h15_real_b = ie_29->h15_real_b;
	psts_29->h15_imag_b = ie_29->h15_imag_b;
	psts_29->h16_real_b = ie_29->h16_real_b;
	psts_29->h16_imag_b = ie_29->h16_imag_b;
	psts_29->h17_real_b = ie_29->h17_real_b;
	psts_29->h17_imag_b = ie_29->h17_imag_b;

	return true;
}

bool halbb_7_physts_ie_30(struct bb_info *bb,
			   u8 *addr,
			   u16 ie_length,
			   struct physts_rxd *desc)
{
	struct bb_physts_info	*physts = &bb->bb_physts_i;
	struct physts_ie_30_info *ie_30 = NULL;

	ie_30 = (struct physts_ie_30_info *)addr;

	return true;
}

bool halbb_7_physts_ie_31(struct bb_info *bb,
			   u8 *addr,
			   u16 ie_length,
			   struct physts_rxd *desc)
{
	struct bb_physts_info	*physts = &bb->bb_physts_i;
	struct physts_ie_31_info *ie_31 = NULL;

	ie_31 = (struct physts_ie_31_info *)addr;

	return true;
}

void halbb_7_physts_print(struct bb_info *bb, struct physts_rxd *desc,
			u16 total_len, u8 *addr, u32 physts_bitmap)
{
	struct bb_physts_info	*physts = &bb->bb_physts_i;
	struct bb_physts_rslt_hdr_info	*psts_h = &physts->bb_physts_rslt_hdr_i;
	u8 i = 0, curr_ie = 0;

	if (physts->physts_dump_mode == 0)
		return;

	if (!physts->show_phy_sts_all_pkt) {
		if (!desc->is_to_self)
			return;
	}

	if (physts->show_phy_sts_cnt >= physts->show_phy_sts_max_cnt)
		return;

	physts->show_phy_sts_cnt++;
	
	BB_TRACE("[Dump_idx:%04d][%d: %s] len=%04d bitmap=0x%08x\n",
		 bb->bb_physts_i.physts_dump_idx,
		 psts_h->ie_map_type, 
		 bb_physts_bitmap_type_t[psts_h->ie_map_type],
		 total_len, physts_bitmap);

	bb->bb_physts_i.physts_dump_idx++;

	if (physts->bb_physts_cnt_i.invalid_he_occur) {
		BB_TRACE("invalid_he_cnt=%d\n", physts->bb_physts_cnt_i.invalid_he_cnt);
	}

	if (bb->bb_physts_i.physts_dump_mode & BIT(0)) {
		BB_TRACE("[HDR]\n");
		halbb_print_buff_64(bb, addr, PHYSTS_HDR_LEN);
		addr += PHYSTS_HDR_LEN;
		total_len -= PHYSTS_HDR_LEN;

		for (i = 0; i < IE_PHYSTS_LEN_ALL; i++) {
			if (total_len < 8)
				break;

			curr_ie = *addr & 0x1f;
			BB_TRACE("===[IE_%02d]===\n", curr_ie);	
			halbb_print_buff_64(bb, addr, physts->ie_len_curr[i]);
			addr += physts->ie_len_curr[i];
			if (total_len >= physts->ie_len_curr[i])
				total_len -= physts->ie_len_curr[i];
		}
		BB_TRACE("----------------------------\n");
	}

	if (bb->bb_physts_i.physts_dump_mode & BIT(1)) {
		halbb_print_rate_2_buff(bb, desc->data_rate, desc->gi_ltf, bb->dbg_buf, HALBB_SNPRINT_SIZE);
		
		BB_TRACE("is_to_self=%d, su=%d, user_num=%d, phy_idx=%d, total_len=%d\n",
		         desc->is_to_self, desc->is_su, desc->user_num, desc->phy_idx, total_len);
		BB_TRACE("Rate= %s (0x%x-%x), macid_su=%d\n",
		         bb->dbg_buf, desc->data_rate, desc->gi_ltf, desc->macid_su);

		if (desc->user_num >= 4)
			return;

		for (i = 0; i < desc->user_num; i++) {
			BB_TRACE("[%d]bcn=%d, ctrl=%d, data=%d, mgnt=%d\n", i,
			         desc->user_i[i].is_bcn, desc->user_i[i].is_ctrl,
			         desc->user_i[i].is_data, desc->user_i[i].is_mgnt);
		}
		BB_TRACE("----------------------------\n");
		halbb_7_physts_detail_dump(bb, physts_bitmap, 0xffffffff);
	}
	BB_TRACE("==============================================\n");
}

bool halbb_7_physts_parsing(struct bb_info *bb_0,
			     u8 *addr,
			     u16 physts_total_length,
			     struct physts_rxd *desc,
			     struct physts_result *bb_rpt)
{
	struct bb_info *bb = bb_0;
	struct bb_physts_info	*physts;
	u8 physts_ie_len_tab[IE_PHYSTS_LEN_ALL] = {4, 5, 3, 3, 1, 1, 1, 1,
					VAR_LENGTH, 1, VAR_LENGTH, 22, VAR_LENGTH, VAR_LENGTH, VAR_LENGTH, VAR_LENGTH,
					VAR_LENGTH, VAR_LENGTH, 2, 3, VAR_LENGTH, VAR_LENGTH, VAR_LENGTH, 0,
					3, 3, 3, 3, 4, 4, 4, 4}; /*Unit: 8Byte*/
	enum bb_physts_hdr_t physts_hdr_t;
	u32 physts_bitmap = 0;
	u16 ie_len;
	u16 acc_ie_len = 0;
	u16 total_ie_len = physts_total_length - PHYSTS_HDR_LEN; /*Unit: Byte*/
	u16 ie_length_hdr = 0;
	u8 i = 0;
	u8 curr_ie = 0xff;
	u8 *addr_in_bkp = addr;
	enum bb_physts_bitmap_t ie_map_type = PHYSTS_BITMAP_NUM;
	bool is_cck_rate = false;
	bool is_valid = true;
	bool is_ie8_valid = false;

#ifdef HALBB_DBCC_SUPPORT
	HALBB_GET_PHY_PTR(bb_0, bb, desc->phy_idx);
#endif
	physts = &bb->bb_physts_i;

	if (desc->is_su)
		desc->macid_su = desc->user_i[0].macid;

	physts->bb_physts_cnt_i.all_cnt++;
	if (desc->is_to_self) {
		physts->bb_physts_cnt_i.is_2_self_cnt++;
	}

	/*---[Physts header parsing]------------------------------------------*/

	if (!halbb_7_physts_ie_hdr(bb, addr, desc, &ie_length_hdr)) {
		BB_DBG(bb, DBG_PHY_STS, "[Warning] physts_hdr_valid = 0\n");
		return false;
	}

	addr += PHYSTS_HDR_LEN;
	ie_map_type = physts->bb_physts_rslt_hdr_i.ie_map_type;

	if (((ie_length_hdr << 3) + 8) != physts_total_length) {
		physts->bb_physts_cnt_i.err_len_cnt++;
		BB_DBG(bb, DBG_PHY_STS, "[Error 1] len=%d, t_len=%d\n",
		       ie_length_hdr, physts_total_length);
		is_valid = false;

		goto PARSING_END;
	}

	if (ie_map_type < PHYSTS_BITMAP_NUM)
		physts->bb_physts_cnt_i.ie_cnt[ie_map_type]++;

	if (ie_map_type == CCK_BRK)
		physts->bb_physts_cnt_i.cck_brk_cnt++;

	if ((ie_map_type <= DL_MU_SPOOFING) || (ie_map_type == UL_TB_SPOOFING)) {
		desc->is_to_self = 0;
		bb_rpt->is_pkt_with_data = false;
		for (i = 0; i < desc->user_num; i++) {
			desc->user_i[i].is_bcn = 0;
			desc->user_i[i].is_ctrl = 0;
			desc->user_i[i].is_data = 0;
			desc->user_i[i].is_mgnt = 0;
		}
	} else {
		bb_rpt->is_pkt_with_data = true;
	}

	if (!desc->is_to_self && !physts->show_phy_sts_all_pkt && !physts->dfs_phy_sts_privilege) {
		/*Only parsing Hdr if the packet is not "to_self"*/
		is_valid = true;
		goto PARSING_END;
	}

#if 1//just for dbg
	BB_DBG(bb, DBG_PHY_STS, "[%s] phy_idx=%d/%d\n", __func__, bb->bb_phy_idx, desc->phy_idx);
	BB_DBG(bb, DBG_PHY_STS, "[cnt] all=%d, err_len=%d, cck_brk=%d\n",
	       physts->bb_physts_cnt_i.all_cnt, physts->bb_physts_cnt_i.err_len_cnt,
	       physts->bb_physts_cnt_i.cck_brk_cnt);
	BB_DBG(bb, DBG_PHY_STS, "[cnt 00~04] [%05d, %05d, %05d, %05d, %05d]\n",
	       physts->bb_physts_cnt_i.ie_cnt[0],
	       physts->bb_physts_cnt_i.ie_cnt[1],
	       physts->bb_physts_cnt_i.ie_cnt[2],
	       physts->bb_physts_cnt_i.ie_cnt[3],
	       physts->bb_physts_cnt_i.ie_cnt[4]);
	BB_DBG(bb, DBG_PHY_STS, "[cnt 05~09] [%05d, %05d, %05d, %05d, %05d]\n",
	       physts->bb_physts_cnt_i.ie_cnt[5],
	       physts->bb_physts_cnt_i.ie_cnt[6],
	       physts->bb_physts_cnt_i.ie_cnt[7],
	       physts->bb_physts_cnt_i.ie_cnt[8],
	       physts->bb_physts_cnt_i.ie_cnt[9]);
	BB_DBG(bb, DBG_PHY_STS, "[cnt 10~14] [%05d, %05d, %05d, %05d, %05d]\n",
	       physts->bb_physts_cnt_i.ie_cnt[10],
	       physts->bb_physts_cnt_i.ie_cnt[11],
	       physts->bb_physts_cnt_i.ie_cnt[12],
	       physts->bb_physts_cnt_i.ie_cnt[13],
	       physts->bb_physts_cnt_i.ie_cnt[14]);
	BB_DBG(bb, DBG_PHY_STS, "[cnt 15~16] [%05d, %05d]\n",
	       physts->bb_physts_cnt_i.ie_cnt[15],
	       physts->bb_physts_cnt_i.ie_cnt[16]);

	halbb_print_rate_2_buff(bb, desc->data_rate, desc->gi_ltf, bb->dbg_buf, HALBB_SNPRINT_SIZE);
	
	BB_DBG(bb, DBG_PHY_STS, "is_to_self=%d, su=%d, user_num=%d, phy_idx=%d, total_len=%d\n",
	         desc->is_to_self, desc->is_su, desc->user_num, desc->phy_idx, physts_total_length);
	BB_DBG(bb, DBG_PHY_STS, "Rate= %s (0x%x-%x), macid_su=%d\n",
	         bb->dbg_buf, desc->data_rate, desc->gi_ltf, desc->macid_su);

	for (i = 0; i < desc->user_num; i++) {
		BB_DBG(bb, DBG_PHY_STS, "[%d]bcn=%d, ctrl=%d, data=%d, mgnt=%d\n", i,
		         desc->user_i[0].is_bcn, desc->user_i[0].is_ctrl,
		         desc->user_i[0].is_data, desc->user_i[0].is_mgnt);
	}

	if (bb->dbg_component & DBG_PHY_STS)
		halbb_print_buff_64(bb, addr_in_bkp, physts_total_length);

	
	BB_DBG(bb, DBG_PHY_STS, "----------------------------\n");
#endif
	

	if (total_ie_len == 0) {
		is_valid = true;
		goto PARSING_END;
	}

	halbb_rate_idx_parsor(bb, desc->data_rate, (enum rtw_gi_ltf)desc->gi_ltf, &physts->bb_rate_i);
	is_cck_rate = halbb_is_cck_rate(bb, desc->data_rate);

	if (physts->bb_physts_rslt_hdr_i.ie_map_type == HE_PKT &&
	    desc->data_rate <= BB_54M) {
		physts->bb_physts_cnt_i.invalid_he_occur = true;
		physts->bb_physts_cnt_i.invalid_he_cnt++;
	} else {
		physts->bb_physts_cnt_i.invalid_he_occur = false;
	}

	/*---[Physts per IE parsing]------------------------------------------*/
	for (i = 0; i < IE_PHYSTS_LEN_ALL; i++) {
		curr_ie = *addr & 0x1f;

		if (physts_bitmap & BIT(curr_ie)) {
			is_valid = false;
			BB_DBG(bb, DBG_PHY_STS, "[Error 2]bitmap=0x%x, ie=%d\n",
			       physts_bitmap, curr_ie);
			break;
		}

		if (physts_ie_len_tab[curr_ie] != VAR_LENGTH) {
			physts_hdr_t = HDR_TYPE1;
			ie_len = (u16)(physts_ie_len_tab[curr_ie] << 3); /*8-byte to byte*/
		} else {
			physts_hdr_t = HDR_TYPE2;
			ie_len = (u16)((((*(addr + 1) & 0xf) << 3) | ((*addr & 0xe0) >> 5)) << 3);
		}

		if (curr_ie == IE00_CMN_CCK) {
			halbb_7_physts_ie_00(bb, addr, ie_len, desc);
		} else if (curr_ie == IE01_CMN_OFDM) {
			halbb_7_physts_ie_01(bb, addr, ie_len, desc);
		} else if (curr_ie == IE02_CMN_EXT_AX) {
			halbb_7_physts_ie_02(bb, addr, ie_len, desc);
		} else if (curr_ie == IE03_CMN_EXT_SEG_1) {
			halbb_7_physts_ie_03(bb, addr, ie_len, desc);
		} else if (curr_ie >= IE04_CMN_EXT_PATH_A &&
			   curr_ie <= IE07_CMN_EXT_PATH_D) {
			halbb_7_physts_ie_04_07(bb, curr_ie, addr, ie_len, desc);
		} else if (curr_ie == IE08_FTR_CH) {
			halbb_7_physts_ie_08(bb, addr, ie_len, desc);
			is_ie8_valid = true;
		} else if (curr_ie == IE09_FTR_PLCP_0) {
			halbb_7_physts_ie_09(bb, addr, ie_len, desc);
		} else if (curr_ie == IE10_FTR_PLCP_EXT) {
			halbb_7_physts_ie_10(bb, addr, ie_len, desc);
		} else if (curr_ie == IE11_FTR_PLCP_HISTOGRAM) {
			halbb_7_physts_ie_11(bb, addr, ie_len, desc);
		} else if (curr_ie == IE12_MU_EIGEN_INFO) {
			halbb_7_physts_ie_12(bb, addr, ie_len, desc);
		} else if (curr_ie == IE13_DL_MU_DEF) {
			halbb_7_physts_ie_13(bb, addr, ie_len, desc);
		} else if (curr_ie == IE14_TB_UL_CQI) {
			halbb_7_physts_ie_14(bb, addr, ie_len, desc);
		} else if (curr_ie == IE15_TB_UL_DEF) {
			halbb_7_physts_ie_15(bb, addr, ie_len, desc);
		} else if (curr_ie == IE17_TB_UL_CTRL) {
			halbb_7_physts_ie_17(bb, addr, ie_len, desc);
		} else if (curr_ie == IE18_DBG_OFDM_FD_CMN) {
			halbb_7_physts_ie_18(bb, addr, ie_len, desc);
		} else if (curr_ie == IE19_DBG_OFDM_TD_CMN) {
			halbb_7_physts_ie_19(bb, addr, ie_len, desc);
		} else if (curr_ie == IE20_DBG_OFDM_FD_USER_SEG_0) {
			halbb_7_physts_ie_20(bb, addr, ie_len, desc);
		} else if (curr_ie == IE21_DBG_OFDM_FD_USER_SEG_1) {
			halbb_7_physts_ie_21(bb, addr, ie_len, desc);
		} else if (curr_ie == IE22_DBG_OFDM_FD_USER_AGC) {
			halbb_7_physts_ie_22(bb, addr, ie_len, desc);
		} else if (curr_ie == IE24_DBG_OFDM_TD_PATH_A) {
			halbb_7_physts_ie_24(bb, addr, ie_len, desc);
		} else if (curr_ie == IE25_DBG_OFDM_TD_PATH_B) {
			halbb_7_physts_ie_25(bb, addr, ie_len, desc);
		} else if (curr_ie == IE26_DBG_OFDM_TD_PATH_C) {
			halbb_7_physts_ie_26(bb, addr, ie_len, desc);
		} else if (curr_ie == IE27_DBG_OFDM_TD_PATH_D) {
			halbb_7_physts_ie_27(bb, addr, ie_len, desc);
		} else if (curr_ie == IE28_DBG_CCK_PATH_A) {
			halbb_7_physts_ie_28(bb, addr, ie_len, desc);
		} else if (curr_ie == IE29_DBG_CCK_PATH_B) {
			halbb_7_physts_ie_29(bb, addr, ie_len, desc);
		} else if (curr_ie == IE30_DBG_CCK_PATH_C) {
			halbb_7_physts_ie_30(bb, addr, ie_len, desc);
		} else if (curr_ie == IE31_DBG_CCK_PATH_D) {
			halbb_7_physts_ie_31(bb, addr, ie_len, desc);
		} else {
			break;
		}

		physts->ie_len_curr[i] = ie_len;

		addr += ie_len;
		acc_ie_len += ie_len;
		physts_bitmap |= BIT(curr_ie);

		BB_DBG(bb, DBG_PHY_STS, "IE=%d, Hdr_type=%d, len=%d, acc_ie_len=%d\n",
		       curr_ie, physts_hdr_t, ie_len, acc_ie_len);


		if (acc_ie_len == total_ie_len) {
			is_valid = true;
			physts->bb_physts_cnt_i.ok_ie_cnt++;
			physts->physts_rpt_len_byte[physts->bb_physts_rslt_hdr_i.ie_map_type] = physts_total_length;
			break;
		} else if (acc_ie_len > total_ie_len) {
			is_valid = false;
			physts->bb_physts_cnt_i.err_ie_cnt++;
			BB_DBG(bb, DBG_PHY_STS, "[Error 3]acc_len=%d,t_len=%d\n",
			       acc_ie_len, total_ie_len);

			break;
		}
	}

	BB_DBG(bb, DBG_PHY_STS, "[%d]Parsing_OK = %d, bitmap=0x%x\n",
	       physts->bb_physts_cnt_i.all_cnt, is_valid, physts_bitmap);

	/*===================================================================*/
PARSING_END:
	halbb_physts_fd_rpl_2_rssi_cvrt(bb);

	halbb_physts_rpt_gen(bb, physts_bitmap, bb_rpt, is_valid, desc, is_cck_rate,
			     is_ie8_valid);
	
	physts->is_valid = is_valid;

	if (!((ie_map_type <= DL_MU_SPOOFING) || (ie_map_type == UL_TB_SPOOFING))) {
		BB_DBG(bb, DBG_PHY_STS, "[%d]is_valid=%d\n", bb->bb_phy_idx, physts->is_valid);
	}

	if (!is_valid)
		return false;

	/*force data type to SU/MU(debug mode)*/
	if (physts->bb_physts_rslt_hdr_i.ie_map_type == VHT_PKT) {
		if (physts->frc_mu == 1)
			desc->is_su = 1;
		else if (physts->frc_mu == 2)
			desc->is_su = 0;
	}

	if (desc->is_to_self && bb_rpt->is_pkt_with_data) {
		physts->physts_bitmap_recv = physts_bitmap;

		halbb_cmn_rpt(bb, desc, physts_bitmap);
		#ifdef HALBB_CFO_TRK_SUPPORT
		halbb_parsing_cfo(bb, physts_bitmap, desc);
		#endif

		#ifdef HALBB_ANT_DIV_SUPPORT
		halbb_antdiv_phy_sts(bb, physts_bitmap, desc);
		#endif

		#ifdef HALBB_PATH_DIV_SUPPORT
		halbb_pathdiv_phy_sts(bb, desc);
		#endif

		#ifdef HALBB_SNIF_SUPPORT
		bb_rpt->snif_rpt_valid = halbb_sniffer_phy_sts(bb, bb_rpt, physts_bitmap, desc);
		#else
		bb_rpt->snif_rpt_valid = false;
		#endif
		BB_DBG(bb, DBG_SNIFFER, "snif_rpt_valid=%d\n", bb_rpt->snif_rpt_valid);
	}

	#ifdef HALBB_DFS_SUPPORT
	halbb_parsing_aci2sig(bb, physts_bitmap);
	#endif

	halbb_7_physts_print(bb, desc, physts_total_length, addr_in_bkp, physts_bitmap);

	return true;
}

#endif


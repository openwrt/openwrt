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

#ifdef HALBB_EDCCA_SUPPORT
bool halbb_edcca_abort(struct bb_info *bb)
{
	if (!(bb->support_ability & BB_EDCCA)) {
		BB_DBG(bb, DBG_EDCCA, "edcca disable\n");
		return true;
	}

	if (bb->pause_ability & BB_EDCCA) {
		BB_DBG(bb, DBG_EDCCA, "Return edcca pause\n");
		return true;
	}

	return false;
}

#ifdef HALBB_COMPILE_AP2_SERIES
void halbb_set_collision_thre(struct bb_info *bb)
{
	struct bb_edcca_info *bb_edcca = &bb->bb_edcca_i;
	struct bb_edcca_cr_info *cr = &bb->bb_edcca_i.bb_edcca_cr_i;
	u8 th = bb_edcca->colli_th;

	halbb_set_reg_curr_phy(bb, cr->collision_r2t_th, cr->collision_r2t_th_m,
			       th);
	halbb_set_reg_curr_phy(bb, cr->collision_t2r_th_mcs0,
			       cr->collision_t2r_th_mcs0_m, th);
	halbb_set_reg_curr_phy(bb, cr->collision_t2r_th_mcs1,
			       cr->collision_t2r_th_mcs1_m, th);
	halbb_set_reg_curr_phy(bb, cr->collision_t2r_th_mcs2,
			       cr->collision_t2r_th_mcs2_m, th);
	halbb_set_reg_curr_phy(bb, cr->collision_t2r_th_mcs3,
			       cr->collision_t2r_th_mcs3_m, th);
	halbb_set_reg_curr_phy(bb, cr->collision_t2r_th_mcs4,
			       cr->collision_t2r_th_mcs4_m, th);
	halbb_set_reg_curr_phy(bb, cr->collision_t2r_th_mcs5,
			       cr->collision_t2r_th_mcs5_m, th);
	halbb_set_reg_curr_phy(bb, cr->collision_t2r_th_mcs6,
			       cr->collision_t2r_th_mcs6_m, th);
	halbb_set_reg_curr_phy(bb, cr->collision_t2r_th_mcs7,
			       cr->collision_t2r_th_mcs7_m, th);
	halbb_set_reg_curr_phy(bb, cr->collision_t2r_th_mcs8,
			       cr->collision_t2r_th_mcs8_m, th);
	halbb_set_reg_curr_phy(bb, cr->collision_t2r_th_mcs9,
			       cr->collision_t2r_th_mcs9_m, th);
	halbb_set_reg_curr_phy(bb, cr->collision_t2r_th_mcs10,
			       cr->collision_t2r_th_mcs10_m, th);
	halbb_set_reg_curr_phy(bb, cr->collision_t2r_th_mcs11,
			       cr->collision_t2r_th_mcs11_m, th);
	halbb_set_reg_curr_phy(bb, cr->collision_t2r_th_cck,
			       cr->collision_t2r_th_cck_m, th);
}

void halbb_set_collision_th(struct bb_info *bb)
{
	struct bb_edcca_info *bb_edcca = &bb->bb_edcca_i;
	u8 rssi_min = bb->bb_ch_i.rssi_min >> 1;
	s8 collision_thre = 0;

	/*mapping between rssi and collision thre */
	collision_thre = 0 - (rssi_min - 110 + COLLOSION_TH_RSSI2VAL) + bb_edcca->colli_ofst;

	/* To avoid overflow*/
	if (collision_thre < COLLOSION_TH_LOW)
		collision_thre = COLLOSION_TH_LOW;
	else if (collision_thre > COLLOSION_TH_HIGH)
		collision_thre = COLLOSION_TH_HIGH;

	bb_edcca->colli_th = (u8)collision_thre;

	BB_DBG(bb, DBG_EDCCA,
	       "[Collision] rssi_min=%d, TH=-%d(dBm), OFST=%d, VAL=%d\n",
	       rssi_min, collision_thre + COLLOSION_TH_RSSI2VAL,
	       bb_edcca->colli_ofst, bb_edcca->colli_th);

	halbb_set_collision_thre(bb);
}
#endif

void halbb_set_edcca_thre(struct bb_info *bb)
{
	struct bb_edcca_info *bb_edcca = &bb->bb_edcca_i;
	struct bb_edcca_cr_info *cr = &bb->bb_edcca_i.bb_edcca_cr_i;
	u8 band = bb->hal_com->band[bb->bb_phy_idx].cur_chandef.band;
	u32 l2h = bb_edcca->th_h;

	halbb_set_reg_curr_phy(bb, cr->r_edcca_level_p, cr->r_edcca_level_p_m, l2h);
	halbb_set_reg_curr_phy(bb, cr->r_edcca_level, cr->r_edcca_level_m, l2h);
	halbb_set_reg_curr_phy(bb, cr->r_ppdu_level, cr->r_ppdu_level_m, l2h);
	halbb_set_reg_curr_phy(bb, cr->r_dwn_level, cr->r_dwn_level_m, (u32)bb_edcca->th_hl_diff);

#if defined(BB_8852C_SUPPORT) || defined(BB_8852B_SUPPORT) || defined(BB_8922A_SUPPORT)
	if ((bb->ic_type == BB_RTL8852C) || (bb->ic_type == BB_RTL8922A)
			  || (bb->ic_sub_type == BB_IC_SUB_TYPE_8852B_8852BP)) {
		if (bb_edcca->edcca_mode == EDCCA_CBP_MODE && band == BAND_ON_6G)
			halbb_set_reg_curr_phy(bb, cr->r_obss_level, cr->r_obss_level_m, l2h);
	}
#endif

#if defined(BB_8922A_SUPPORT)
	if (bb->ic_type == BB_RTL8922A) {
		halbb_set_reg_curr_phy(bb, cr->r_edcca_level_s80_1, cr->r_edcca_level_s80_1_m, l2h);
		halbb_set_reg_curr_phy(bb, cr->r_edcca_level_s80_1_p, cr->r_edcca_level_s80_1_p_m, l2h);
		halbb_set_reg_curr_phy(bb, cr->r_edcca_level_s80_2, cr->r_edcca_level_s80_2_m, l2h);
		halbb_set_reg_curr_phy(bb, cr->r_edcca_level_s80_2_p, cr->r_edcca_level_s80_2_p_m, l2h);
		halbb_set_reg_curr_phy(bb, cr->r_edcca_level_s80_3, cr->r_edcca_level_s80_3_m, l2h);
		halbb_set_reg_curr_phy(bb, cr->r_edcca_level_s80_3_p, cr->r_edcca_level_s80_3_p_m, l2h);
		halbb_set_reg_curr_phy(bb, cr->r_ppdu_level_p, cr->r_ppdu_level_p_m, l2h);
		halbb_set_reg_curr_phy(bb, cr->r_dwn_level_s80_1, cr->r_dwn_level_s80_1_m, (u32)bb_edcca->th_hl_diff);
		halbb_set_reg_curr_phy(bb, cr->r_dwn_level_s80_2, cr->r_dwn_level_s80_2_m, (u32)bb_edcca->th_hl_diff);
		halbb_set_reg_curr_phy(bb, cr->r_dwn_level_s80_3, cr->r_dwn_level_s80_3_m, (u32)bb_edcca->th_hl_diff);
	}
#endif
	/* cca_sec = edcca_sec || ppducca_sec : ppducca here is not equal to ofdm_cca from packet detection*/
	/* edcca_level_s20 = r_edcca_level*/
	/* edcca_level_s40 = r_edcca_level + 3*/
	/* edcca_level_s80 = r_edcca_level + 6*/
	/* ppdu_level_s20 = max(r_ppdu_level, r_obss_level)*/
	/* ppdu_level_s40 = max(r_ppdu_level, r_obss_level + 3)*/
	/* ppdu_level_s80 = max(r_ppdu_level + 3, r_obss_level + 6)*/
}

u8 halbb_edcca_thre_transfer_rssi(struct bb_info *bb)
{
	u8 rssi_min = bb->bb_ch_i.rssi_min >> 1;
	u8 edcca_thre = 0;

	/*mapping between rssi and edcca thre */
	/* use EDCCA_TH_REF to prevent power offset or PAPR for collision issue*/
	edcca_thre = rssi_min - 110 + 128 - EDCCA_TH_REF;

	if (edcca_thre <= EDCCA_TH_L2H_LB)
		edcca_thre = EDCCA_TH_L2H_LB;

	return edcca_thre;
}

void halbb_edcca_thre_calc(struct bb_info *bb)
{
	struct bb_edcca_info *bb_edcca = &bb->bb_edcca_i;
	u8 band = bb->hal_com->band[bb->bb_phy_idx].cur_chandef.band;
	enum channel_width bw = bb->hal_com->band[bb->bb_phy_idx].cur_chandef.bw;
	u8 th_h = 0;

	if (bb_edcca->edcca_mode == EDCCA_NORMAL_MODE) {
		BB_DBG(bb, DBG_EDCCA, "Normal Mode without EDCCA\n");
		th_h = halbb_edcca_thre_transfer_rssi(bb);
		bb_edcca->th_hl_diff = EDCCA_HL_DIFF_NORMAL;
	} else if (bb_edcca->edcca_mode == EDCCA_ADAPT_MODE) {
		if (band == BAND_ON_24G)
			th_h = bb_edcca->th_h_2p4g;
		else
			th_h = bb_edcca->th_h_5g;
		bb_edcca->th_hl_diff = EDCCA_HL_DIFF_ADPTVTY;
	} else if (bb_edcca->edcca_mode == EDCCA_CBP_MODE) {
		if (band == BAND_ON_6G && (bb->phl_com->id.id & 0x7) == 0x2)
			th_h = halbb_edcca_thre_transfer_rssi(bb); /* WA 6G 32CU noise figure=7X dBm causes edcca flag always=1 */
		else if (band == BAND_ON_6G && bw == CHANNEL_WIDTH_160)
			th_h = bb_edcca->th_h_6g - 10; /* 160M apply 80+80M filter would loss more when interference @ DC*/
		else if (band == BAND_ON_6G)
			th_h = bb_edcca->th_h_6g;
		else
			th_h = halbb_edcca_thre_transfer_rssi(bb);
		bb_edcca->th_hl_diff = EDCCA_HL_DIFF_ADPTVTY;
	} else if (bb_edcca->edcca_mode == EDCCA_CARRIER_SENSE_MODE) {
		th_h = bb_edcca->th_h_cs;
		bb_edcca->th_hl_diff = EDCCA_HL_DIFF_ADPTVTY;
	}
	bb_edcca->th_h = th_h;
	bb_edcca->th_l = bb_edcca->th_h - bb_edcca->th_hl_diff;

	halbb_set_edcca_thre(bb);
}

void halbb_set_edcca_pause_val(struct bb_info *bb, u32 *val_buf, u8 val_len)
{
	struct bb_edcca_info *bb_edcca = &bb->bb_edcca_i;

	if (val_len != 1) {
		BB_DBG(bb, DBG_EDCCA, "[Error][EDCCA]Need val_len=1\n");
		return;
	}
	BB_DBG(bb, DBG_EDCCA, "[%s] len=%d, val[0]=0x%x\n", __func__, val_len, val_buf[0]);

	bb_edcca->th_h = (u8)val_buf[0];
	halbb_set_edcca_thre(bb);
}

void halbb_edcca_event_nofity(struct bb_info *bb, u8 pause_type)
{
	struct bb_edcca_info *bb_edcca = &bb->bb_edcca_i;
	u8 pause_result = 0;
	u32 val[5] = {0};

	BB_DBG(bb, DBG_EDCCA, "[%s], pause_type=%d, edcca_mode=%d\n",
	       __func__, pause_type, bb_edcca->edcca_mode);

	val[0] = EDCCA_MAX;
	pause_result = halbb_pause_func(bb, F_EDCCA, pause_type, HALBB_PAUSE_LV_2, 1, val, bb->bb_phy_idx);
}

void halbb_edcca_log(struct bb_info *bb)
{
	struct bb_edcca_info *bb_edcca = &bb->bb_edcca_i;
	struct edcca_hw_rpt *rpt = &bb_edcca->edcca_rpt;
	struct bb_edcca_cr_info *cr = &bb->bb_edcca_i.bb_edcca_cr_i;
	enum channel_width bw = 0;
	struct bb_ch_info *ch = &bb->bb_ch_i;
	u8 band = bb->hal_com->band[bb->bb_phy_idx].cur_chandef.band;
	char ext_loss[HALBB_MAX_PATH][HALBB_SNPRINT_SIZE];
	u8 i = 0;

	bw = bb->hal_com->band[bb->bb_phy_idx].cur_chandef.bw;

	BB_DBG(bb, DBG_EDCCA, "[EDCCA] Mode=%d, Band=%d\n",
	       bb_edcca->edcca_mode, band);

	BB_DBG(bb, DBG_EDCCA,
	       "[EDCCA] CBP-6G_th=%d(dBm) Adapt-5G_th=%d(dBm), Adapt-2.4G_th=%d(dBm),Carrier-sense_th=%d(dBm)\n",
	       bb_edcca->th_h_6g - 128, bb_edcca->th_h_5g - 128,
	       bb_edcca->th_h_2p4g - 128, bb_edcca->th_h_cs - 128);

	BB_DBG(bb, DBG_EDCCA, "th_h=%d(dBm), th_l=%d(dBm)\n",
	       bb_edcca->th_h - 128, bb_edcca->th_l - 128);

	for (i = 0; i < HALBB_MAX_PATH; i++)
		halbb_print_sign_frac_digit(bb, ch->ext_loss[i], 8, 2,
					    ext_loss[i], HALBB_SNPRINT_SIZE);

	halbb_print_sign_frac_digit(bb, ch->ext_loss_avg, 8, 2, bb->dbg_buf,
				    HALBB_SNPRINT_SIZE);

	if (bb->num_rf_path >= 4) {
		BB_DBG(bb, DBG_EDCCA,
		       "loss{avg, a, b, c, d} = {%s,%s,%s,%s,%s} dB,pwrofst=%d dB\n",
		       bb->dbg_buf, ext_loss[0], ext_loss[1], ext_loss[2], ext_loss[3],
		       bb_edcca->pwrofst - 16);
	} else if (bb->num_rf_path >= 2) {
		BB_DBG(bb, DBG_EDCCA,
		       "loss{avg, a, b} = {%s,%s,%s} dB,pwrofst =%d dB\n",
		       bb->dbg_buf, ext_loss[0], ext_loss[1],
		       bb_edcca->pwrofst - 16);
	} else {
		BB_DBG(bb, DBG_EDCCA, "loss=%s dB,pwrofst =%d dB\n",
		       ext_loss[0], bb_edcca->pwrofst - 16);
	}

	switch (bw) {
	case CHANNEL_WIDTH_80_80:
	case CHANNEL_WIDTH_160:
		BB_DBG(bb, DBG_EDCCA,
		       "pwdb per20{0,1,2,3,4,5,6,7}={%d,%d,%d,%d,%d,%d,%d,%d}(dBm)\n",
		       rpt->pwdb_0, rpt->pwdb_1, rpt->pwdb_2, rpt->pwdb_3,
		       rpt->pwdb_4, rpt->pwdb_5, rpt->pwdb_6, rpt->pwdb_7);
		BB_DBG(bb, DBG_EDCCA,
		       "path=%d, flag {FB,p20,s20,s40,s80}={%d,%d,%d,%d,%d}\n",
		       rpt->path, rpt->flag_fb, rpt->flag_p20, rpt->flag_s20,
		       rpt->flag_s40, rpt->flag_s80);
		BB_DBG(bb, DBG_EDCCA,
		       "pwdb {FB,p20,s20,s40,s80}={%d,%d,%d,%d,%d}(dBm)\n",
		       rpt->pwdb_fb, rpt->pwdb_p20, rpt->pwdb_s20,
		       rpt->pwdb_s40, rpt->pwdb_s80);
		break;
	case CHANNEL_WIDTH_80:
		BB_DBG(bb, DBG_EDCCA,
		       "pwdb per20{0,1,2,3}={%d,%d,%d,%d}(dBm)\n",
		       rpt->pwdb_0, rpt->pwdb_1, rpt->pwdb_2, rpt->pwdb_3);
		BB_DBG(bb, DBG_EDCCA, "path=%d, flag {FB,p20,s20,s40}={%d,%d,%d,%d}\n",
		       rpt->path, rpt->flag_fb, rpt->flag_p20, rpt->flag_s20,
		       rpt->flag_s40);
		BB_DBG(bb, DBG_EDCCA,
		       "pwdb {FB,p20,s20,s40}={%d,%d,%d,%d}(dBm)\n",
		       rpt->pwdb_fb, rpt->pwdb_p20, rpt->pwdb_s20,
		       rpt->pwdb_s40);
		break;
	case CHANNEL_WIDTH_40:
		BB_DBG(bb, DBG_EDCCA, "pwdb per20{0,1}={%d,%d}(dBm)\n",
		       rpt->pwdb_0, rpt->pwdb_1);
		BB_DBG(bb, DBG_EDCCA, "path=%d, flag {FB,p20,s20}={%d,%d,%d}\n",
		       rpt->path, rpt->flag_fb, rpt->flag_p20, rpt->flag_s20);
		BB_DBG(bb, DBG_EDCCA, "pwdb {FB,p20,s20}={%d,%d,%d}(dBm)\n",
		       rpt->pwdb_fb, rpt->pwdb_p20, rpt->pwdb_s20);
		break;
	case CHANNEL_WIDTH_20:
		BB_DBG(bb, DBG_EDCCA, "pwdb per20{0}={%d}(dBm)\n", rpt->pwdb_0);
		BB_DBG(bb, DBG_EDCCA, "path=%d, flag {FB,p20}={%d,%d}\n",
		       rpt->path, rpt->flag_fb, rpt->flag_p20);
		BB_DBG(bb, DBG_EDCCA, "pwdb {FB,p20}={%d,%d}(dBm)\n",
		       rpt->pwdb_fb, rpt->pwdb_p20);
		break;
	default:
		break;
	}
}

void halbb_edcca_get_result(struct bb_info *bb)
{
	struct bb_edcca_info *bb_edcca = &bb->bb_edcca_i;
	struct edcca_hw_rpt *rpt = &bb_edcca->edcca_rpt;
	struct bb_edcca_cr_info *cr = &bb->bb_edcca_i.bb_edcca_cr_i;
	u32 tmp = 0;
	u64 tmp_linear = 0;
	u32 rpt_sel_addr = cr->r_edcca_rpt_sel;
	u32 rpt_sel_bmsk = cr->r_edcca_rpt_sel_m;
	u32 rpt_a_addr = cr->r_edcca_rpt_a, rpt_a_bmsk = cr->r_edcca_rpt_a_m;
	u32 rpt_b_addr = cr->r_edcca_rpt_b, rpt_b_bmsk = cr->r_edcca_rpt_b_m;
	enum channel_width bw = 0;

	bw = bb->hal_com->band[bb->bb_phy_idx].cur_chandef.bw;

#ifdef HALBB_DBCC_SUPPORT
	if (bb->bb_phy_idx == HW_PHY_1) {
		rpt_sel_addr = cr->r_edcca_rpt_sel_p1;
		rpt_sel_bmsk = cr->r_edcca_rpt_sel_p1_m;
		rpt_a_addr = cr->r_edcca_rpt_a_p1;
		rpt_a_bmsk = cr->r_edcca_rpt_a_p1_m;
		rpt_b_addr = cr->r_edcca_rpt_b_p1;
		rpt_b_bmsk = cr->r_edcca_rpt_b_p1_m;
	}
#endif
	halbb_set_reg(bb, rpt_sel_addr, rpt_sel_bmsk, 0);
	tmp = halbb_get_reg(bb, rpt_a_addr, rpt_a_bmsk);
	rpt->pwdb_1 = (s8)(((tmp & MASKBYTE2) >> 16) - 256);
	rpt->pwdb_0 = (s8)(((tmp & MASKBYTE3) >> 24) - 256);
	tmp = halbb_get_reg(bb, rpt_b_addr, rpt_b_bmsk);
	rpt->path = (u8)((tmp & 0x6) >> 1);
	rpt->flag_s80 = (bool)((tmp & BIT(3)) >> 3);
	rpt->flag_s40 = (bool)((tmp & BIT(4)) >> 4);
	rpt->flag_s20 = (bool)((tmp & BIT(5)) >> 5);
	rpt->flag_p20 = (bool)((tmp & BIT(6)) >> 6);
	rpt->flag_fb = (bool)((tmp & BIT(7)) >> 7);
	rpt->pwdb_s20 = (s8)(((tmp & MASKBYTE1) >> 8) - 256);
	rpt->pwdb_p20 = (s8)(((tmp & MASKBYTE2) >> 16) - 256);
	rpt->pwdb_fb = (s8)(((tmp & MASKBYTE3) >> 24) - 256);

	switch (bw) {
	case CHANNEL_WIDTH_80_80:
	case CHANNEL_WIDTH_160:
		halbb_set_reg(bb, rpt_sel_addr, rpt_sel_bmsk, 5);
		tmp = halbb_get_reg(bb, rpt_a_addr, rpt_a_bmsk);
		rpt->pwdb_3 = (s8)(((tmp & MASKBYTE2) >> 16) - 256);
		rpt->pwdb_2 = (s8)(((tmp & MASKBYTE3) >> 24) - 256);
		tmp = halbb_get_reg(bb, rpt_b_addr, rpt_b_bmsk);
		rpt->pwdb_s80 = (s8)(((tmp & MASKBYTE1) >> 8) - 256);
		rpt->pwdb_s40 = (s8)(((tmp & MASKBYTE2) >> 16) - 256);

		halbb_set_reg(bb, rpt_sel_addr, rpt_sel_bmsk, 2);
		tmp = halbb_get_reg(bb, rpt_a_addr, rpt_a_bmsk);
		rpt->pwdb_5 = (s8)(((tmp & MASKBYTE2) >> 16) - 256);
		rpt->pwdb_4 = (s8)(((tmp & MASKBYTE3) >> 24) - 256);
		
		halbb_set_reg(bb, rpt_sel_addr, rpt_sel_bmsk, 3);
		tmp = halbb_get_reg(bb, rpt_a_addr, rpt_a_bmsk);
		rpt->pwdb_7 = (s8)(((tmp & MASKBYTE2) >> 16) - 256);
		rpt->pwdb_6 = (s8)(((tmp & MASKBYTE3) >> 24) - 256);
		break;
	case CHANNEL_WIDTH_80:
		halbb_set_reg(bb, rpt_sel_addr, rpt_sel_bmsk, 5);
		tmp = halbb_get_reg(bb, rpt_a_addr, rpt_a_bmsk);
		rpt->pwdb_3 = (s8)(((tmp & MASKBYTE2) >> 16) - 256);
		rpt->pwdb_2 = (s8)(((tmp & MASKBYTE3) >> 24) - 256);
		tmp = halbb_get_reg(bb, rpt_b_addr, rpt_b_bmsk);
		rpt->pwdb_s80 = (s8)(((tmp & MASKBYTE1) >> 8) - 256);
		rpt->pwdb_s40 = (s8)(((tmp & MASKBYTE2) >> 16) - 256);
		break;
	case CHANNEL_WIDTH_40:
		/*52A/52B/52C/51B has hw bug of pwdb-FB is 0 when bw=40M*/
		if ((bb->ic_type == BB_RTL8852A) ||
		    (bb->ic_type == BB_RTL8852B) ||
		    (bb->ic_type == BB_RTL8852C) ||
		    (bb->ic_type == BB_RTL8851B)) {
			if ((rpt->pwdb_p20 == (s8)(EDCCA_PWDB_EXCLU_TX)) ||
			    (rpt->pwdb_s20 == (s8)(EDCCA_PWDB_EXCLU_TX))) {
				rpt->pwdb_fb = (s8)(EDCCA_PWDB_EXCLU_TX);
			} else {
				tmp = (u32)EDCCA_PWDB_TO_RSSI(rpt->pwdb_p20);
				tmp_linear = halbb_db_2_linear(tmp);
				tmp = (u32)EDCCA_PWDB_TO_RSSI(rpt->pwdb_s20);
				tmp_linear += halbb_db_2_linear(tmp);
				tmp_linear = (tmp_linear + (1 << (FRAC_BITS - 1))) >> FRAC_BITS;
				tmp = halbb_convert_to_db(tmp_linear);
				rpt->pwdb_fb = (s8)(tmp - 110);
			}
		}
		break;
	default:
		break;
	}
}

void halbb_edcca(struct bb_info *bb)
{
	struct bb_edcca_info *bb_edcca = &bb->bb_edcca_i;

	bb_edcca->edcca_mode = bb->phl_com->edcca_mode;

	if (halbb_edcca_abort(bb) == false) { /*show edcca log when edcca is paused*/
		halbb_edcca_thre_calc(bb);
#ifdef HALBB_COMPILE_AP2_SERIES
		halbb_set_collision_th(bb);
#endif
	}

	halbb_edcca_get_result(bb);
	halbb_edcca_log(bb);
}

void halbb_fw_edcca(struct bb_info *bb)
{
	struct bb_edcca_info *bb_edcca = &bb->bb_edcca_i;
	struct rtw_phl_com_t *phl = bb->phl_com;
	struct rtw_hal_com_t *hal = bb->hal_com;
	struct dev_cap_t *dev = &phl->dev_cap;
	u8 band = bb->hal_com->band[bb->bb_phy_idx].cur_chandef.band;
	struct bb_h2c_fw_edcca *fw_edcca_i = &bb->bb_fw_edcca_i;
	u8 cmdlen;
#if defined(BB_8852C_SUPPORT) || defined(BB_8852B_SUPPORT)
	bool ret_val = false;
#endif
	u32 *bb_h2c = (u32 *)fw_edcca_i;
	cmdlen = sizeof(struct bb_h2c_fw_edcca);

	bb_edcca->edcca_mode = phl->edcca_mode;

	BB_DBG(bb, DBG_EDCCA, "notify_switch_band/fw_edcca, edcca_mode=%d\n",
	       bb_edcca->edcca_mode);

	/*8852BP FW needs infomation of band and mode even if edcca is paused*/
	if (bb->ic_sub_type != BB_IC_SUB_TYPE_8852B_8852BP) {
		if (halbb_edcca_abort(bb))
			return;
	}

	/*only for 8852C CAV & 8852BP / 6E Homologation*/
	if (!(((hal->cv == CAV) && (bb->ic_type == BB_RTL8852C)) ||
	      (bb->ic_sub_type == BB_IC_SUB_TYPE_8852B_8852BP)))
		return;

	/* Only for NIC side*/
	if (dev->rfe_type >= 50)
		return;

	/*8852BP FW needs infomation of band and mode even if edcca is normal mode*/
	if ((bb_edcca->edcca_mode == EDCCA_NORMAL_MODE) &&
	    (bb->ic_sub_type != BB_IC_SUB_TYPE_8852B_8852BP)) {
		BB_DBG(bb, DBG_EDCCA, "Normal Mode without FW EDCCA\n");
		return;
	}

	fw_edcca_i->mode = bb_edcca->edcca_mode;
	fw_edcca_i->band = bb->hal_com->band[bb->bb_phy_idx].cur_chandef.band;

	BB_DBG(bb, DBG_EDCCA, "[FW EDCCA H2C] Mode=%d, Band=%d\n", 
				fw_edcca_i->mode, fw_edcca_i->band);
	/*8852BP FW EDCCA don't need edcca th*/
	if (bb->ic_sub_type != BB_IC_SUB_TYPE_8852B_8852BP)
		BB_DBG(bb, DBG_EDCCA, "[FW EDCCA H2C] Adapt-5G_th=-%d, Adapt-2.4G_th=-%d,Carrier-sense_th=-%d\n", 
					fw_edcca_i->pwr_th_5g, fw_edcca_i->pwr_th_2p4, 
					fw_edcca_i->pwr_th_cs);
	
	BB_DBG(bb, DBG_FW_INFO, "[FW][H2C] h2c conent[0]=%x\n", bb_h2c[0]);
	BB_DBG(bb, DBG_FW_INFO, "[FW][H2C] h2c conent[1]=%x\n", bb_h2c[1]);

#if defined(BB_8852C_SUPPORT) || defined(BB_8852B_SUPPORT)
	ret_val = halbb_fill_h2c_cmd(bb, cmdlen, DM_H2C_FW_EDCCA, 
					     HALBB_H2C_DM, bb_h2c);
	if (ret_val == false)
		BB_WARNING(" H2C cmd: FW Tx error!!\n");
#endif
}

void halbb_fw_i(struct bb_info *bb, u8 enable)
{
	struct bb_h2c_fw_edcca *fw_edcca_i = &bb->bb_fw_edcca_i;
	u8 cmdlen;
	bool ret_val = false;

	u32 *bb_h2c = (u32 *)fw_edcca_i;
	cmdlen = sizeof(struct bb_h2c_fw_edcca);

	fw_edcca_i->enable = enable;

	ret_val = halbb_fill_h2c_cmd(bb, cmdlen, DM_H2C_FW_EDCCA, 
					     HALBB_H2C_DM, bb_h2c);

	BB_DBG(bb, DBG_FW_INFO, "[Hsuan][Drv] is_i_only=%d\n", fw_edcca_i->enable);

	if (ret_val == false)
		BB_WARNING(" H2C cmd: FW Tx error!!\n");
}

void halbb_edcca_cnsl_log(struct bb_info *bb, u32 *_used, char *output,
			  u32 *_out_len)
{
	struct bb_edcca_info *bb_edcca = &bb->bb_edcca_i;
	struct edcca_hw_rpt *rpt = &bb_edcca->edcca_rpt;
	struct bb_edcca_cr_info *cr = &bb->bb_edcca_i.bb_edcca_cr_i;
	struct bb_ch_info *ch = &bb->bb_ch_i;
	enum channel_width bw = 0;
	u32 used = *_used;
	u32 out_len = *_out_len;
	u8 edcca_p_th = 0;
	u8 edcca_s_th = 0;
	u8 edcca_diff = 0;
	u8 ppdu_s_th = 0;
	u8 obss_th = 0;
	bool edcca_en = 0;
	char ext_loss[HALBB_MAX_PATH][HALBB_SNPRINT_SIZE];
	u8 i = 0;

	bw = bb->hal_com->band[0].cur_chandef.bw;

	BB_DBG_CNSL(out_len, used, output + used, out_len - used,
		    "Adapt-5G_th=%d(dBm), Adapt-2.4G_th=%d(dBm), Carrier-sense_th=%d(dBm), CBP-6G_th=%d(dBm)\n",
		    bb_edcca->th_h_5g - 128, bb_edcca->th_h_2p4g - 128,
		    bb_edcca->th_h_cs - 128, bb_edcca->th_h_6g - 128);
	BB_DBG_CNSL(out_len, used, output + used, out_len - used,
		    "Mode=%d, th_h=%d(dBm), th_l=%d(dBm)\n",
		    bb_edcca->edcca_mode, bb_edcca->th_h - 128,
		    bb_edcca->th_l - 128);

	halbb_edcca_get_result(bb);

	for (i = 0; i < HALBB_MAX_PATH; i++)
		halbb_print_sign_frac_digit(bb, ch->ext_loss[i], 8, 2,
					    ext_loss[i], HALBB_SNPRINT_SIZE);

	halbb_print_sign_frac_digit(bb, ch->ext_loss_avg, 8, 2, bb->dbg_buf,
				    HALBB_SNPRINT_SIZE);

	if (bb->num_rf_path >= 4) {
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			    "ext_loss{avg,a,b,c,d}={%s,%s,%s,%s,%s} dB, pwrofst=%d dB\n",
			    bb->dbg_buf, ext_loss[0], ext_loss[1], ext_loss[2],
			    ext_loss[3], bb_edcca->pwrofst - 16);
	} else if (bb->num_rf_path >= 2) {
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			    "ext_loss{avg,a,b}={%s,%s,%s} dB, pwrofst=%d dB\n",
			    bb->dbg_buf, ext_loss[0], ext_loss[1],
			    bb_edcca->pwrofst - 16);
	} else {
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			    "ext_loss=%s dB, pwrofst=%d dB\n",
			    ext_loss[0], bb_edcca->pwrofst - 16);
	}

	switch (bw) {
	case CHANNEL_WIDTH_80_80:
	case CHANNEL_WIDTH_160:
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			    "pwdb per20{0,1,2,3,4,5,6,7}={%d,%d,%d,%d,%d,%d,%d,%d}(dBm)\n",
			    rpt->pwdb_0, rpt->pwdb_1, rpt->pwdb_2,
			    rpt->pwdb_3, rpt->pwdb_4, rpt->pwdb_5,
			    rpt->pwdb_6, rpt->pwdb_7);
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			    "path-%d, edcca_flag {FB,p20,s20,s40,s80}={%d,%d,%d,%d,%d}\n",
			    rpt->path, rpt->flag_fb, rpt->flag_p20,
			    rpt->flag_s20, rpt->flag_s40, rpt->flag_s80);
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			    "pwdb {FB,p20,s20,s40,s80}={%d,%d,%d,%d,%d}(dBm)\n",
			    rpt->pwdb_fb, rpt->pwdb_p20, rpt->pwdb_s20,
			    rpt->pwdb_s40, rpt->pwdb_s80);
		break;
	case CHANNEL_WIDTH_80:
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			    "pwdb per20{0,1,2,3}={%d,%d,%d,%d}(dBm)\n",
			    rpt->pwdb_0, rpt->pwdb_1, rpt->pwdb_2, rpt->pwdb_3);
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			    "path-%d, edcca_flag {FB,p20,s20,s40}={%d,%d,%d,%d}\n",
			    rpt->path, rpt->flag_fb, rpt->flag_p20,
			    rpt->flag_s20, rpt->flag_s40);
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			    "pwdb {FB,p20,s20,s40}={%d,%d,%d,%d}(dBm)\n",
			    rpt->pwdb_fb, rpt->pwdb_p20, rpt->pwdb_s20,
			    rpt->pwdb_s40);
		break;
	case CHANNEL_WIDTH_40:
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			    "pwdb per20{0,1}={%d,%d}(dBm)\n",
			    rpt->pwdb_0, rpt->pwdb_1);
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			    "path-%d, edcca_flag {FB,p20,s20}={%d,%d,%d}\n",
			    rpt->path, rpt->flag_fb, rpt->flag_p20,
			    rpt->flag_s20);
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			    "pwdb {FB,p20,s20}={%d,%d,%d}(dBm)\n",
			    rpt->pwdb_fb, rpt->pwdb_p20, rpt->pwdb_s20);
		break;
	case CHANNEL_WIDTH_20:
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			    "pwdb per20{0}={%d}(dBm)\n", rpt->pwdb_0);
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			    "path-%d, edcca_flag {FB,p20}={%d,%d}\n",
			    rpt->path, rpt->flag_fb, rpt->flag_p20);
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			    "pwdb {FB,p20}={%d,%d}(dBm)\n",
			    rpt->pwdb_fb, rpt->pwdb_p20);
		break;
	default:
		break;
	}

	edcca_en = (bool)halbb_get_reg(bb, cr->r_snd_en, cr->r_snd_en_m);
	edcca_diff = (u8)halbb_get_reg(bb, cr->r_dwn_level, cr->r_dwn_level_m);
	edcca_p_th = (u8)halbb_get_reg(bb, cr->r_edcca_level_p,
				       cr->r_edcca_level_p_m);
	edcca_s_th = (u8)halbb_get_reg(bb, cr->r_edcca_level,
				       cr->r_edcca_level_m);
	ppdu_s_th = (u8)halbb_get_reg(bb, cr->r_ppdu_level,
				       cr->r_ppdu_level_m);
	obss_th = (u8)halbb_get_reg(bb, cr->r_obss_level, cr->r_obss_level_m);

	BB_DBG_CNSL(out_len, used, output + used, out_len - used,
		    "r_snd_en=%d, r_dwn_level = %d, r_edcca_level_p = %d(%d dBm)\n",
		    edcca_en, edcca_diff, edcca_p_th, edcca_p_th - 128);
	BB_DBG_CNSL(out_len, used, output + used, out_len - used,
		    "r_edcca_level = %d(%d dBm), r_ppdu_level = %d(%d dBm), r_obss_level = %d(%d dBm)\n",
		    edcca_s_th, edcca_s_th - 128, ppdu_s_th, ppdu_s_th - 128,
		    obss_th, obss_th - 128);
	BB_DBG_CNSL(out_len, used, output + used, out_len - used,
		    "cca_sec = edcca_sec || ppducca_sec\n");
	BB_DBG_CNSL(out_len, used, output + used, out_len - used,
		    "edcca_s20 : r_edcca_level    , ppducca_s20 : max(r_ppdu_level    , r_obss_level    )\n");
	BB_DBG_CNSL(out_len, used, output + used, out_len - used,
		    "edcca_s40 : r_edcca_level + 3, ppducca_s40 : max(r_ppdu_level    , r_obss_level + 3)\n");
	BB_DBG_CNSL(out_len, used, output + used, out_len - used,
		    "edcca_s80 : r_edcca_level + 6, ppducca_s80 : max(r_ppdu_level + 3, r_obss_level + 6)\n");
}

void halbb_edcca_dbg(struct bb_info *bb, char input[][16], u32 *_used,
			      char *output, u32 *_out_len)
{
	struct bb_edcca_info *bb_edcca = &bb->bb_edcca_i;
	struct bb_h2c_fw_edcca *fw_edcca_i = &bb->bb_fw_edcca_i;
	struct rtw_hal_com_t *hal = bb->hal_com;
	char help[] = "-h";
	u32 var[10] = {0};
	u32 used = *_used;
	u32 out_len = *_out_len;

	HALBB_SCAN(input[1], DCMD_DECIMAL, &var[0]);

	if ((_os_strcmp(input[1], help) == 0)) {
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "[EDCCA] Set power threshold(-dBm): {1} {Adapt-5G_th} {Adapt-2.4G_th} {Carrier-sense_th} {CBP-6G_th}\n");
#if defined(BB_8852C_SUPPORT) || defined(BB_8852B_SUPPORT)
		if ((bb->ic_type == BB_RTL8852C) ||
		    (bb->ic_sub_type == BB_IC_SUB_TYPE_8852B_8852BP))
			BB_DBG_CNSL(out_len, used, output + used, out_len - used,
				    "[Manually trigger FW EDCCA] : 2\n");
#endif
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "[EDCCA] Set EDCCA mode: {3} {mode 0:normal mode, 1:Adaptivity, 2: Carrier sense, 3:CBP}\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "[Collision T2R/T2T] Set extra offset(dB): {4} {OFST}\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "Show Power threshold: {100}\n");
	} else if (var[0] == 1) {
		HALBB_SCAN(input[2], DCMD_DECIMAL, &var[1]);
		HALBB_SCAN(input[3], DCMD_DECIMAL, &var[2]);
		HALBB_SCAN(input[4], DCMD_DECIMAL, &var[3]);
		HALBB_SCAN(input[5], DCMD_DECIMAL, &var[4]);

		bb_edcca->th_h_5g = (u8)var[1];
		bb_edcca->th_h_2p4g = (u8)var[2];
		bb_edcca->th_h_cs = (u8)var[3];
		bb_edcca->th_h_6g = (u8)var[4];

		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			    "Set Adapt-5G_th=-%d(dBm), Adapt-2.4G_th=-%d(dBm), Carrier-sense_th=-%d(dBm), CBP-6G_th=-%d(dBm)\n",
			    bb_edcca->th_h_5g, bb_edcca->th_h_2p4g,
			    bb_edcca->th_h_cs, bb_edcca->th_h_6g);

		bb_edcca->th_h_5g = 0 - (bb_edcca->th_h_5g) + 128;
		bb_edcca->th_h_2p4g = 0 - (bb_edcca->th_h_2p4g) + 128;
		bb_edcca->th_h_cs = 0 - (bb_edcca->th_h_cs) + 128;
		bb_edcca->th_h_6g = 0 - (bb_edcca->th_h_6g) + 128;
#if defined(BB_8852C_SUPPORT) || defined(BB_8852B_SUPPORT)
	} else if (var[0] == 2) {
		if (!(((hal->cv == CAV) && (bb->ic_type == BB_RTL8852C)) ||
		      (bb->ic_sub_type == BB_IC_SUB_TYPE_8852B_8852BP)))
			return;

		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			    "[Homologation 6E] FW EDCCA start\n");
		halbb_fw_edcca(bb);
#endif
	} else if (var[0] == 3) {
		HALBB_SCAN(input[2], DCMD_DECIMAL, &var[1]);

		bb_edcca->edcca_mode = (u8)var[1];

		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			    "Set EDCCA mode = %s\n",
			    (bb_edcca->edcca_mode == EDCCA_NORMAL_MODE) ?
			    "Normal mode" : "Adaptivity/Carrier Sense/CBP mode");

		if (!(((hal->cv == CAV) && (bb->ic_type == BB_RTL8852C)) ||
		      (bb->ic_sub_type == BB_IC_SUB_TYPE_8852B_8852BP)))
			return;

#if defined(BB_8852C_SUPPORT) || defined(BB_8852B_SUPPORT)
		if (bb_edcca->edcca_mode == EDCCA_CBP_MODE)
			BB_DBG_CNSL(out_len, used, output + used,
				    out_len - used,
				    "[Homologation 6E] FW EDCCA start\n");
		halbb_fw_edcca(bb);
#endif
	} else if (var[0] == 4) {
		HALBB_SCAN(input[2], DCMD_DECIMAL, &var[1]);

		bb_edcca->colli_ofst = (u8)var[1];

		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			    "Set extra offset(dB)=%d\n", bb_edcca->colli_ofst);
	} else if (var[0] == 100) {
		halbb_edcca_cnsl_log(bb, _used, output, _out_len);
	} else if (var[0] == 101) {
		BB_WARNING("[Hsuan] i-only cmd\n");
		HALBB_SCAN(input[2], DCMD_DECIMAL, &var[1]);
		halbb_fw_i(bb, (u8)var[1]);
	}
	*_used = used;
	*_out_len = out_len;
}

void halbb_edcca_dev_hw_cap(struct bb_info *bb)
{
	struct rtw_hal_com_t *hal = bb->hal_com;

	if (bb->ic_type == BB_RTL8851B) {/* For SingleTone shift 1MHz*/
		hal->dev_hw_cap.edcca_cap.edcca_carrier_sense_th = CARRIER_SENSE - 6;
		hal->dev_hw_cap.edcca_cap.edcca_adap_th_5g = EDCCA_5G;
		hal->dev_hw_cap.edcca_cap.edcca_adap_th_2g = EDCCA_2G;
	} else if (bb->ic_type == BB_RTL8852B) { /*[HALBB-126] for SingleTone shift 1MHz*/
#if (defined(BB_8852B_SUPPORT))
		if (bb->ic_sub_type == BB_IC_SUB_TYPE_8852B_8852BP)
			hal->dev_hw_cap.edcca_cap.edcca_cbp_th_6g = CBP_6G;
#endif
		hal->dev_hw_cap.edcca_cap.edcca_carrier_sense_th = CARRIER_SENSE - 6;
		hal->dev_hw_cap.edcca_cap.edcca_adap_th_5g = EDCCA_5G;
		hal->dev_hw_cap.edcca_cap.edcca_adap_th_2g = EDCCA_2G;
	} else if (bb->ic_type == BB_RTL8852C || bb->ic_type == BB_RTL8922A) {
		hal->dev_hw_cap.edcca_cap.edcca_cbp_th_6g = CBP_6G;
		hal->dev_hw_cap.edcca_cap.edcca_adap_th_5g = EDCCA_5G - 3;
		hal->dev_hw_cap.edcca_cap.edcca_adap_th_2g = EDCCA_2G - 3;
		hal->dev_hw_cap.edcca_cap.edcca_carrier_sense_th = CARRIER_SENSE - 10;
	} else {
		hal->dev_hw_cap.edcca_cap.edcca_carrier_sense_th = CARRIER_SENSE;
		hal->dev_hw_cap.edcca_cap.edcca_adap_th_5g = EDCCA_5G;
		hal->dev_hw_cap.edcca_cap.edcca_adap_th_2g = EDCCA_2G;
	}
}


void halbb_edcca_init(struct bb_info *bb)
{
	struct bb_edcca_info *bb_edcca = &bb->bb_edcca_i;
	struct bb_h2c_fw_edcca *fw_edcca_i = &bb->bb_fw_edcca_i;
	struct bb_edcca_cr_info *cr = &bb->bb_edcca_i.bb_edcca_cr_i;
	struct rtw_phl_com_t *phl = bb->phl_com;

	if(phl_is_mp_mode(bb->phl_com))
		return;

	bb_edcca->edcca_mode = phl->edcca_mode;

	bb_edcca->th_h = EDCCA_MAX;
	bb_edcca->th_l = EDCCA_MAX;
	bb_edcca->th_h_lb = 46;
	bb_edcca->pwrofst = (u8)halbb_get_reg(bb, cr->r_pwrofst,
					      cr->r_pwrofst_m);
#if defined(BB_8922A_SUPPORT)
		if ((bb->ic_type == BB_RTL8922A)) {
			halbb_set_reg_phy0_1(bb, cr->r_obss_level, cr->r_obss_level_m, EDCCA_MAX);
			halbb_set_reg_phy0_1(bb, cr->r_edcca_level_p, cr->r_edcca_level_p_m, EDCCA_MAX);
			halbb_set_reg_phy0_1(bb, cr->r_edcca_level, cr->r_edcca_level_m, EDCCA_MAX);
			halbb_set_reg_phy0_1(bb, cr->r_ppdu_level, cr->r_ppdu_level_m, EDCCA_MAX);
			halbb_set_reg_phy0_1(bb, cr->r_edcca_level_s80_1, cr->r_edcca_level_s80_1_m, EDCCA_MAX);
			halbb_set_reg_phy0_1(bb, cr->r_edcca_level_s80_1_p, cr->r_edcca_level_s80_1_p_m, EDCCA_MAX);
			halbb_set_reg_phy0_1(bb, cr->r_edcca_level_s80_2, cr->r_edcca_level_s80_2_m, EDCCA_MAX);
			halbb_set_reg_phy0_1(bb, cr->r_edcca_level_s80_2_p, cr->r_edcca_level_s80_2_p_m, EDCCA_MAX);
			halbb_set_reg_phy0_1(bb, cr->r_edcca_level_s80_3, cr->r_edcca_level_s80_3_m, EDCCA_MAX);
			halbb_set_reg_phy0_1(bb, cr->r_edcca_level_s80_3_p, cr->r_edcca_level_s80_3_p_m, EDCCA_MAX);
			halbb_set_reg_phy0_1(bb, cr->r_ppdu_level_p, cr->r_ppdu_level_p_m, EDCCA_MAX);
			halbb_set_reg_phy0_1(bb, cr->r_snd_en, cr->r_snd_en_m, false);
			halbb_set_reg_phy0_1(bb, cr->r_snd_en_s80_1, cr->r_snd_en_s80_1_m, false);
			halbb_set_reg_phy0_1(bb, cr->r_snd_en_s80_2, cr->r_snd_en_s80_2_m, false);
			halbb_set_reg_phy0_1(bb, cr->r_snd_en_s80_3, cr->r_snd_en_s80_3_m, false);
		}
#endif

	// EDCCA
	/* 6G only for 52C/52BP*/
#if defined(BB_8852C_SUPPORT) || defined(BB_8852B_SUPPORT) || defined(BB_8922A_SUPPORT)
	if ((bb->ic_type == BB_RTL8852C) || (bb->ic_type == BB_RTL8922A)
		       || (bb->ic_sub_type == BB_IC_SUB_TYPE_8852B_8852BP))
		bb_edcca->th_h_6g = phl->dev_cap.edcca_cap.edcca_cbp_th_6g;
#endif

	bb_edcca->th_h_5g = phl->dev_cap.edcca_cap.edcca_adap_th_5g;
	bb_edcca->th_h_2p4g = phl->dev_cap.edcca_cap.edcca_adap_th_2g;
	bb_edcca->th_h_cs = phl->dev_cap.edcca_cap.edcca_carrier_sense_th;

	// FW EDCCA
	fw_edcca_i->pwr_th_5g = phl->dev_cap.edcca_cap.edcca_adap_th_5g;
	fw_edcca_i->pwr_th_2p4 = phl->dev_cap.edcca_cap.edcca_adap_th_2g;
	fw_edcca_i->pwr_th_cs = phl->dev_cap.edcca_cap.edcca_carrier_sense_th;

	// Collision R2T/T2R
	bb_edcca->colli_ofst = COLLOSION_TH_OFST;
}


void halbb_cr_cfg_edcca_init(struct bb_info *bb)
{
	struct bb_edcca_cr_info *cr = &bb->bb_edcca_i.bb_edcca_cr_i;

	switch (bb->cr_type) {

#ifdef HALBB_COMPILE_AP_SERIES
	case BB_AP:
		cr->r_snd_en = SEG0R_SND_EN_A;
		cr->r_snd_en_m = SEG0R_SND_EN_A_M;
		cr->r_dwn_level = SEG0R_DWN_LVL_A;
		cr->r_dwn_level_m = SEG0R_DWN_LVL_A_M;
		cr->r_edcca_level = SEG0R_EDCCA_LVL_A;
		cr->r_edcca_level_m = SEG0R_EDCCA_LVL_A_M;
		cr->r_edcca_level_p = SEG0R_EDCCA_LVL_P_A;
		cr->r_edcca_level_p_m = SEG0R_EDCCA_LVL_P_A_M;
		cr->r_edcca_rpt_a = EDCCA_IOQ_P0_A_A;
		cr->r_edcca_rpt_a_m = EDCCA_IOQ_P0_A_A_M;
		cr->r_edcca_rpt_b = EDCCA_IOQ_P0_B_A;
		cr->r_edcca_rpt_b_m = EDCCA_IOQ_P0_B_A_M;
		cr->r_edcca_rpt_a_p1 = EDCCA_IOQ_P1_A_A;
		cr->r_edcca_rpt_a_p1_m = EDCCA_IOQ_P1_A_A_M;
		cr->r_edcca_rpt_b_p1 = EDCCA_IOQ_P1_B_A;
		cr->r_edcca_rpt_b_p1_m = EDCCA_IOQ_P1_B_A_M;
		cr->r_edcca_rpt_sel = EDCCA_RPTREG_SEL_P0_A;
		cr->r_edcca_rpt_sel_m = EDCCA_RPTREG_SEL_P0_A_M;
		cr->r_edcca_rpt_sel_p1 = EDCCA_RPTREG_SEL_P1_A;
		cr->r_edcca_rpt_sel_p1_m = EDCCA_RPTREG_SEL_P1_A_M;
		cr->r_ppdu_level = SEG0R_PPDU_LVL_A;
		cr->r_ppdu_level_m = SEG0R_PPDU_LVL_A_M;
		cr->collision_r2t_th = COLLISION_R2T_TH_A;
		cr->collision_r2t_th_m = COLLISION_R2T_TH_A_M;
		cr->collision_t2r_th_mcs0 = COLLISION_T2R_TH_MCS0_A;
		cr->collision_t2r_th_mcs0_m = COLLISION_T2R_TH_MCS0_A_M;
		cr->collision_t2r_th_mcs1 = COLLISION_T2R_TH_MCS1_A;
		cr->collision_t2r_th_mcs1_m = COLLISION_T2R_TH_MCS1_A_M;
		cr->collision_t2r_th_mcs2 = COLLISION_T2R_TH_MCS2_A;
		cr->collision_t2r_th_mcs2_m = COLLISION_T2R_TH_MCS2_A_M;
		cr->collision_t2r_th_mcs3 = COLLISION_T2R_TH_MCS3_A;
		cr->collision_t2r_th_mcs3_m = COLLISION_T2R_TH_MCS3_A_M;
		cr->collision_t2r_th_mcs4 = COLLISION_T2R_TH_MCS4_A;
		cr->collision_t2r_th_mcs4_m = COLLISION_T2R_TH_MCS4_A_M;
		cr->collision_t2r_th_mcs5 = COLLISION_T2R_TH_MCS5_A;
		cr->collision_t2r_th_mcs5_m = COLLISION_T2R_TH_MCS5_A_M;
		cr->collision_t2r_th_mcs6 = COLLISION_T2R_TH_MCS6_A;
		cr->collision_t2r_th_mcs6_m = COLLISION_T2R_TH_MCS6_A_M;
		cr->collision_t2r_th_mcs7 = COLLISION_T2R_TH_MCS7_A;
		cr->collision_t2r_th_mcs7_m = COLLISION_T2R_TH_MCS7_A_M;
		cr->collision_t2r_th_mcs8 = COLLISION_T2R_TH_MCS8_A;
		cr->collision_t2r_th_mcs8_m = COLLISION_T2R_TH_MCS8_A_M;
		cr->collision_t2r_th_mcs9 = COLLISION_T2R_TH_MCS9_A;
		cr->collision_t2r_th_mcs9_m = COLLISION_T2R_TH_MCS9_A_M;
		cr->collision_t2r_th_mcs10 = COLLISION_T2R_TH_MCS10_A;
		cr->collision_t2r_th_mcs10_m = COLLISION_T2R_TH_MCS10_A_M;
		cr->collision_t2r_th_mcs11 = COLLISION_T2R_TH_MCS11_A;
		cr->collision_t2r_th_mcs11_m = COLLISION_T2R_TH_MCS11_A_M;
		cr->collision_t2r_th_cck = COLLISION_T2R_TH_CCK_A;
		cr->collision_t2r_th_cck_m = COLLISION_T2R_TH_CCK_A_M;
		cr->r_obss_level = SEG0R_OBSS_LVL_A;
		cr->r_obss_level_m = SEG0R_OBSS_LVL_A_M;
		cr->r_pwrofst = SEG0R_PWOFST_A;
		cr->r_pwrofst_m = SEG0R_PWOFST_A_M;
		break;
#endif
#ifdef HALBB_COMPILE_CLIENT_SERIES
	case BB_CLIENT:
		cr->r_snd_en = SEG0R_SND_EN_C;
		cr->r_snd_en_m = SEG0R_SND_EN_C_M;
		cr->r_dwn_level = SEG0R_DWN_LVL_C;
		cr->r_dwn_level_m = SEG0R_DWN_LVL_C_M;
		cr->r_edcca_level = SEG0R_EDCCA_LVL_C;
		cr->r_edcca_level_m = SEG0R_EDCCA_LVL_C_M;
		cr->r_edcca_level_p = SEG0R_EDCCA_LVL_P_C;
		cr->r_edcca_level_p_m = SEG0R_EDCCA_LVL_P_C_M;
		cr->r_edcca_rpt_a = EDCCA_IOQ_P0_A_C;
		cr->r_edcca_rpt_a_m = EDCCA_IOQ_P0_A_C_M;
		cr->r_edcca_rpt_b = EDCCA_IOQ_P0_B_C;
		cr->r_edcca_rpt_b_m = EDCCA_IOQ_P0_B_C_M;
		cr->r_edcca_rpt_a_p1 = EDCCA_IOQ_P1_A_C;
		cr->r_edcca_rpt_a_p1_m = EDCCA_IOQ_P1_A_C_M;
		cr->r_edcca_rpt_b_p1 = EDCCA_IOQ_P1_B_C;
		cr->r_edcca_rpt_b_p1_m = EDCCA_IOQ_P1_B_C_M;
		cr->r_edcca_rpt_sel = EDCCA_RPTREG_SEL_P0_C;
		cr->r_edcca_rpt_sel_m = EDCCA_RPTREG_SEL_P0_C_M;
		cr->r_edcca_rpt_sel_p1 = EDCCA_RPTREG_SEL_P1_C;
		cr->r_edcca_rpt_sel_p1_m = EDCCA_RPTREG_SEL_P1_C_M;
		cr->r_ppdu_level = SEG0R_PPDU_LVL_C;
		cr->r_ppdu_level_m = SEG0R_PPDU_LVL_C_M;
		cr->collision_r2t_th = COLLISION_R2T_TH_C;
		cr->collision_r2t_th_m = COLLISION_R2T_TH_C_M;
		cr->collision_t2r_th_mcs0 = COLLISION_T2R_TH_MCS0_C;
		cr->collision_t2r_th_mcs0_m = COLLISION_T2R_TH_MCS0_C_M;
		cr->collision_t2r_th_mcs1 = COLLISION_T2R_TH_MCS1_C;
		cr->collision_t2r_th_mcs1_m = COLLISION_T2R_TH_MCS1_C_M;
		cr->collision_t2r_th_mcs2 = COLLISION_T2R_TH_MCS2_C;
		cr->collision_t2r_th_mcs2_m = COLLISION_T2R_TH_MCS2_C_M;
		cr->collision_t2r_th_mcs3 = COLLISION_T2R_TH_MCS3_C;
		cr->collision_t2r_th_mcs3_m = COLLISION_T2R_TH_MCS3_C_M;
		cr->collision_t2r_th_mcs4 = COLLISION_T2R_TH_MCS4_C;
		cr->collision_t2r_th_mcs4_m = COLLISION_T2R_TH_MCS4_C_M;
		cr->collision_t2r_th_mcs5 = COLLISION_T2R_TH_MCS5_C;
		cr->collision_t2r_th_mcs5_m = COLLISION_T2R_TH_MCS5_C_M;
		cr->collision_t2r_th_mcs6 = COLLISION_T2R_TH_MCS6_C;
		cr->collision_t2r_th_mcs6_m = COLLISION_T2R_TH_MCS6_C_M;
		cr->collision_t2r_th_mcs7 = COLLISION_T2R_TH_MCS7_C;
		cr->collision_t2r_th_mcs7_m = COLLISION_T2R_TH_MCS7_C_M;
		cr->collision_t2r_th_mcs8 = COLLISION_T2R_TH_MCS8_C;
		cr->collision_t2r_th_mcs8_m = COLLISION_T2R_TH_MCS8_C_M;
		cr->collision_t2r_th_mcs9 = COLLISION_T2R_TH_MCS9_C;
		cr->collision_t2r_th_mcs9_m = COLLISION_T2R_TH_MCS9_C_M;
		cr->collision_t2r_th_mcs10 = COLLISION_T2R_TH_MCS10_C;
		cr->collision_t2r_th_mcs10_m = COLLISION_T2R_TH_MCS10_C_M;
		cr->collision_t2r_th_mcs11 = COLLISION_T2R_TH_MCS11_C;
		cr->collision_t2r_th_mcs11_m = COLLISION_T2R_TH_MCS11_C_M;
		cr->collision_t2r_th_cck = COLLISION_T2R_TH_CCK_C;
		cr->collision_t2r_th_cck_m = COLLISION_T2R_TH_CCK_C_M;
		cr->r_obss_level = SEG0R_OBSS_LVL_C;
		cr->r_obss_level_m = SEG0R_OBSS_LVL_C_M;
		cr->r_pwrofst = SEG0R_PWOFST_C;
		cr->r_pwrofst_m = SEG0R_PWOFST_C_M;
		break;
#endif
#ifdef HALBB_COMPILE_AP2_SERIES
	case BB_AP2:
		cr->r_snd_en = SEG0R_SND_EN_A2;
		cr->r_snd_en_m = SEG0R_SND_EN_A2_M;
		cr->r_dwn_level = SEG0R_DWN_LVL_A2;
		cr->r_dwn_level_m = SEG0R_DWN_LVL_A2_M;
		cr->r_edcca_level = SEG0R_EDCCA_LVL_A2;
		cr->r_edcca_level_m = SEG0R_EDCCA_LVL_A2_M;
		cr->r_edcca_level_p = SEG0R_EDCCA_LVL_P_A2;
		cr->r_edcca_level_p_m = SEG0R_EDCCA_LVL_P_A2_M;
		cr->r_edcca_rpt_a = EDCCA_IOQ_P0_A_A2;
		cr->r_edcca_rpt_a_m = EDCCA_IOQ_P0_A_A2_M;
		cr->r_edcca_rpt_b = EDCCA_IOQ_P0_B_A2;
		cr->r_edcca_rpt_b_m = EDCCA_IOQ_P0_B_A2_M;
		cr->r_edcca_rpt_a_p1 = EDCCA_IOQ_P1_A_A2;
		cr->r_edcca_rpt_a_p1_m = EDCCA_IOQ_P1_A_A2_M;
		cr->r_edcca_rpt_b_p1 = EDCCA_IOQ_P1_B_A2;
		cr->r_edcca_rpt_b_p1_m = EDCCA_IOQ_P1_B_A2_M;
		cr->r_edcca_rpt_sel = EDCCA_RPTREG_SEL_P0_A2;
		cr->r_edcca_rpt_sel_m = EDCCA_RPTREG_SEL_P0_A2_M;
		cr->r_edcca_rpt_sel_p1 = EDCCA_RPTREG_SEL_P1_A2;
		cr->r_edcca_rpt_sel_p1_m = EDCCA_RPTREG_SEL_P1_A2_M;
		cr->r_ppdu_level = SEG0R_PPDU_LVL_A2;
		cr->r_ppdu_level_m = SEG0R_PPDU_LVL_A2_M;
		cr->collision_r2t_th = COLLISION_R2T_TH_A2;
		cr->collision_r2t_th_m = COLLISION_R2T_TH_A2_M;
		cr->collision_t2r_th_mcs0 = COLLISION_T2R_TH_MCS0_A2;
		cr->collision_t2r_th_mcs0_m = COLLISION_T2R_TH_MCS0_A2_M;
		cr->collision_t2r_th_mcs1 = COLLISION_T2R_TH_MCS1_A2;
		cr->collision_t2r_th_mcs1_m = COLLISION_T2R_TH_MCS1_A2_M;
		cr->collision_t2r_th_mcs2 = COLLISION_T2R_TH_MCS2_A2;
		cr->collision_t2r_th_mcs2_m = COLLISION_T2R_TH_MCS2_A2_M;
		cr->collision_t2r_th_mcs3 = COLLISION_T2R_TH_MCS3_A2;
		cr->collision_t2r_th_mcs3_m = COLLISION_T2R_TH_MCS3_A2_M;
		cr->collision_t2r_th_mcs4 = COLLISION_T2R_TH_MCS4_A2;
		cr->collision_t2r_th_mcs4_m = COLLISION_T2R_TH_MCS4_A2_M;
		cr->collision_t2r_th_mcs5 = COLLISION_T2R_TH_MCS5_A2;
		cr->collision_t2r_th_mcs5_m = COLLISION_T2R_TH_MCS5_A2_M;
		cr->collision_t2r_th_mcs6 = COLLISION_T2R_TH_MCS6_A2;
		cr->collision_t2r_th_mcs6_m = COLLISION_T2R_TH_MCS6_A2_M;
		cr->collision_t2r_th_mcs7 = COLLISION_T2R_TH_MCS7_A2;
		cr->collision_t2r_th_mcs7_m = COLLISION_T2R_TH_MCS7_A2_M;
		cr->collision_t2r_th_mcs8 = COLLISION_T2R_TH_MCS8_A2;
		cr->collision_t2r_th_mcs8_m = COLLISION_T2R_TH_MCS8_A2_M;
		cr->collision_t2r_th_mcs9 = COLLISION_T2R_TH_MCS9_A2;
		cr->collision_t2r_th_mcs9_m = COLLISION_T2R_TH_MCS9_A2_M;
		cr->collision_t2r_th_mcs10 = COLLISION_T2R_TH_MCS10_A2;
		cr->collision_t2r_th_mcs10_m = COLLISION_T2R_TH_MCS10_A2_M;
		cr->collision_t2r_th_mcs11 = COLLISION_T2R_TH_MCS11_A2;
		cr->collision_t2r_th_mcs11_m = COLLISION_T2R_TH_MCS11_A2_M;
		cr->collision_t2r_th_cck = COLLISION_T2R_TH_CCK_A2;
		cr->collision_t2r_th_cck_m = COLLISION_T2R_TH_CCK_A2_M;
		cr->r_obss_level = SEG0R_OBSS_LVL_A2;
		cr->r_obss_level_m = SEG0R_OBSS_LVL_A2_M;
		cr->r_pwrofst = SEG0R_PWOFST_A2;
		cr->r_pwrofst_m = SEG0R_PWOFST_A2_M;
		break;
#endif

#ifdef HALBB_COMPILE_BE0_SERIES
		case BB_BE0:
		cr->r_snd_en = STFDET_R_SND_EN_BE0;
		cr->r_snd_en_m = STFDET_R_SND_EN_BE0_M;
		cr->r_snd_en_s80_1 = STFDET_S80_1_R_SND_EN_BE0;
		cr->r_snd_en_s80_1_m = STFDET_S80_1_R_SND_EN_BE0_M;
		cr->r_snd_en_s80_2 = STFDET_S80_2_R_SND_EN_BE0;
		cr->r_snd_en_s80_2_m = STFDET_S80_2_R_SND_EN_BE0_M;
		cr->r_snd_en_s80_3 = STFDET_S80_3_R_SND_EN_BE0;
		cr->r_snd_en_s80_3_m = STFDET_S80_3_R_SND_EN_BE0_M;
		cr->r_dwn_level = DWN_LVL_BE0;
		cr->r_dwn_level_m = DWN_LVL_BE0_M;
		cr->r_dwn_level_s80_1 = S80_1_R_DWN_LVL_BE0;
		cr->r_dwn_level_s80_1_m = S80_1_R_DWN_LVL_BE0_M;
		cr->r_dwn_level_s80_2 = S80_2_R_DWN_LVL_BE0;
		cr->r_dwn_level_s80_2_m = S80_2_R_DWN_LVL_BE0_M;
		cr->r_dwn_level_s80_3 = S80_3_R_DWN_LVL_BE0;
		cr->r_dwn_level_s80_3_m = S80_3_R_DWN_LVL_BE0_M;
		cr->r_edcca_level = EDCCA_LVL_BE0;
		cr->r_edcca_level_m = EDCCA_LVL_BE0_M;
		cr->r_edcca_level_p = EDCCA_LVL_P_BE0;
		cr->r_edcca_level_p_m = EDCCA_LVL_P_BE0_M;
		cr->r_edcca_level_s80_1 = S80_1_R_EDCCA_LVL_BE0;
		cr->r_edcca_level_s80_1_m = S80_1_R_EDCCA_LVL_BE0_M;
		cr->r_edcca_level_s80_1_p = S80_1_R_EDCCA_LVL_P_BE0;
		cr->r_edcca_level_s80_1_p_m = S80_1_R_EDCCA_LVL_P_BE0_M;
		cr->r_edcca_level_s80_2 = S80_2_R_EDCCA_LVL_BE0;
		cr->r_edcca_level_s80_2_m = S80_2_R_EDCCA_LVL_BE0_M;
		cr->r_edcca_level_s80_2_p = S80_2_R_EDCCA_LVL_P_BE0;
		cr->r_edcca_level_s80_2_p_m = S80_2_R_EDCCA_LVL_P_BE0_M;
		cr->r_edcca_level_s80_3 = S80_3_R_EDCCA_LVL_BE0;
		cr->r_edcca_level_s80_3_m = S80_3_R_EDCCA_LVL_BE0_M;
		cr->r_edcca_level_s80_3_p = S80_3_R_EDCCA_LVL_P_BE0;
		cr->r_edcca_level_s80_3_p_m = S80_3_R_EDCCA_LVL_P_BE0_M;
		cr->r_edcca_rpt_a = EDCCA_IOQ_P0_A_BE0;
		cr->r_edcca_rpt_a_m = EDCCA_IOQ_P0_A_BE0_M;
		cr->r_edcca_rpt_b = EDCCA_IOQ_P0_B_BE0;
		cr->r_edcca_rpt_b_m = EDCCA_IOQ_P0_B_BE0_M;
		cr->r_edcca_rpt_a_p1 = EDCCA_IOQ_P1_A_BE0;
		cr->r_edcca_rpt_a_p1_m = EDCCA_IOQ_P1_A_BE0_M;
		cr->r_edcca_rpt_b_p1 = EDCCA_IOQ_P1_B_BE0;
		cr->r_edcca_rpt_b_p1_m = EDCCA_IOQ_P1_B_BE0_M;
		cr->r_edcca_rpt_sel = EDCCA_RPTREG_SEL_P0_BE0;
		cr->r_edcca_rpt_sel_m = EDCCA_RPTREG_SEL_P0_BE0_M;
		cr->r_edcca_rpt_sel_p1 = EDCCA_RPTREG_SEL_P1_BE0;
		cr->r_edcca_rpt_sel_p1_m = EDCCA_RPTREG_SEL_P1_BE0_M;
		cr->r_ppdu_level = PPDU_LVL_BE0;
		cr->r_ppdu_level_m = PPDU_LVL_BE0_M;
		cr->r_ppdu_level_p = PPDU_LVL_P_BE0;
		cr->r_ppdu_level_p_m = PPDU_LVL_P_BE0_M;
		cr->collision_r2t_th = COLLISION_R2T_TH_BE0;
		cr->collision_r2t_th_m = COLLISION_R2T_TH_BE0_M;
		cr->collision_t2r_th_mcs0 = COLLISION_T2R_TH_MCS0_BE0;
		cr->collision_t2r_th_mcs0_m = COLLISION_T2R_TH_MCS0_BE0_M;
		cr->collision_t2r_th_mcs1 = COLLISION_T2R_TH_MCS1_BE0;
		cr->collision_t2r_th_mcs1_m = COLLISION_T2R_TH_MCS1_BE0_M;
		cr->collision_t2r_th_mcs2 = COLLISION_T2R_TH_MCS2_BE0;
		cr->collision_t2r_th_mcs2_m = COLLISION_T2R_TH_MCS2_BE0_M;
		cr->collision_t2r_th_mcs3 = COLLISION_T2R_TH_MCS3_BE0;
		cr->collision_t2r_th_mcs3_m = COLLISION_T2R_TH_MCS3_BE0_M;
		cr->collision_t2r_th_mcs4 = COLLISION_T2R_TH_MCS4_BE0;
		cr->collision_t2r_th_mcs4_m = COLLISION_T2R_TH_MCS4_BE0_M;
		cr->collision_t2r_th_mcs5 = COLLISION_T2R_TH_MCS5_BE0;
		cr->collision_t2r_th_mcs5_m = COLLISION_T2R_TH_MCS5_BE0_M;
		cr->collision_t2r_th_mcs6 = COLLISION_T2R_TH_MCS6_BE0;
		cr->collision_t2r_th_mcs6_m = COLLISION_T2R_TH_MCS6_BE0_M;
		cr->collision_t2r_th_mcs7 = COLLISION_T2R_TH_MCS7_BE0;
		cr->collision_t2r_th_mcs7_m = COLLISION_T2R_TH_MCS7_BE0_M;
		cr->collision_t2r_th_mcs8 = COLLISION_T2R_TH_MCS8_BE0;
		cr->collision_t2r_th_mcs8_m = COLLISION_T2R_TH_MCS8_BE0_M;
		cr->collision_t2r_th_mcs9 = COLLISION_T2R_TH_MCS9_BE0;
		cr->collision_t2r_th_mcs9_m = COLLISION_T2R_TH_MCS9_BE0_M;
		cr->collision_t2r_th_mcs10 = COLLISION_T2R_TH_MCS10_BE0;
		cr->collision_t2r_th_mcs10_m = COLLISION_T2R_TH_MCS10_BE0_M;
		cr->collision_t2r_th_mcs11 = COLLISION_T2R_TH_MCS11_BE0;
		cr->collision_t2r_th_mcs11_m = COLLISION_T2R_TH_MCS11_BE0_M;
		cr->collision_t2r_th_cck = COLLISION_T2R_TH_CCK_BE0;
		cr->collision_t2r_th_cck_m = COLLISION_T2R_TH_CCK_BE0_M;
		cr->r_obss_level = OBSS_LVL_BE0; // except p20
		cr->r_obss_level_m = OBSS_LVL_BE0_M; // except p20
		cr->r_pwrofst = STFDET_R_PWOFST_BE0; // only for s80_0
		cr->r_pwrofst_m = STFDET_R_PWOFST_BE0_M; // only for s80_0
		break;
	
#endif

	default:
		BB_WARNING("[%s] BBCR Hook FAIL!\n", __func__);
		if (bb->bb_dbg_i.cr_fake_init_hook_en) {
			BB_TRACE("[%s] BBCR fake init\n", __func__);
			halbb_cr_hook_fake_init(bb, (u32 *)cr, (sizeof(struct bb_edcca_cr_info) >> 2));
		}
		break;
	}

	if (bb->bb_dbg_i.cr_init_hook_recorder_en) {
		BB_TRACE("[%s] BBCR Hook dump\n", __func__);
		halbb_cr_hook_init_dump(bb, (u32 *)cr, (sizeof(struct bb_edcca_cr_info) >> 2));
	}
}
#endif

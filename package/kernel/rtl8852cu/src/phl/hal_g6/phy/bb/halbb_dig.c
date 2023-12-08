/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2021, Realtek Semiconductor Corp. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice, this
 *     list of conditions and the following disclaimer.
 *
 *   * Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   * Neither the name of the Realtek nor the names of its contributors may
 *     be used to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "halbb_precomp.h"

#ifdef HALBB_DIG_SUPPORT

#ifdef BB_8852A_2_SUPPORT
u8 halbb_lna_idx_by_rssi(struct bb_info *bb, u8 rssi)
{
	struct bb_dig_info *bb_dig = &bb->bb_dig_i;
	struct bb_dig_op_unit *bb_dig_u = bb_dig->p_cur_dig_unit;
	u8 lna_idx = LNA_IDX_MAX;

	if (rssi < bb_dig_u->dig_op_para.igi_rssi_th[0])
		lna_idx = 6;
	else if (rssi < bb_dig_u->dig_op_para.igi_rssi_th[1])
		lna_idx = 5;
	else if (rssi < bb_dig_u->dig_op_para.igi_rssi_th[2])
		lna_idx = 4;
	else if (rssi < bb_dig_u->dig_op_para.igi_rssi_th[3])
		lna_idx = 3;
	else if (rssi < bb_dig_u->dig_op_para.igi_rssi_th[4])
		lna_idx = 2;
	else
		lna_idx = 1;

	return lna_idx;
}

u8 halbb_tia_idx_by_rssi(struct bb_info *bb, u8 rssi)
{
	struct bb_dig_info *bb_dig = &bb->bb_dig_i;
	struct bb_dig_op_unit *bb_dig_u = bb_dig->p_cur_dig_unit;
	u8 tia_idx = TIA_IDX_MAX;

	if (rssi < bb_dig_u->dig_op_para.igi_rssi_th[0])
		tia_idx = 1;
	else
		tia_idx = 0;

	return tia_idx;
}

u8 halbb_rxb_idx_by_rssi(struct bb_info *bb,
			 struct agc_gaincode_set *set, u8 rssi)
{
	struct bb_dig_info *bb_dig = &bb->bb_dig_i;
	u8 rxb_idx = RXB_IDX_MAX;
	s8 lna_gain = bb_dig->lna_gain[set->lna_idx];
	s8 tia_gain = bb_dig->tia_gain[set->tia_idx];
	s32 rxb_idx_tmp = RXB_IDX_MAX;
	s32 wb_rssi = rssi + lna_gain + tia_gain;

	rxb_idx_tmp = (bb_dig->ib_pkpwr - bb_dig->ib_pbk + 110) - wb_rssi + 10;

	if (rxb_idx_tmp > RXB_IDX_MAX)
		rxb_idx = RXB_IDX_MAX;
	else if (rxb_idx_tmp < RXB_IDX_MIN)
		rxb_idx = RXB_IDX_MIN;
	else
		rxb_idx = (u8)rxb_idx_tmp;

	BB_DIG_DBG(bb, DIG_DBG_LV2, "wb_rssi=%03d, rxb_idx_tmp=%03d\n",
		   wb_rssi, rxb_idx_tmp);

	return rxb_idx;
}


void halbb_dig_set_igi_cr_8852a(struct bb_info *bb, const struct agc_gaincode_set set)
{
	if (bb->ic_type != BB_RTL8852A)
		return;

	if (bb->hal_com->dbcc_en) {
		if (bb->bb_phy_idx == HW_PHY_0)
			halbb_set_igi_8852a_2(bb, set.lna_idx, set.tia_idx,
					      set.rxb_idx, RF_PATH_A);
		else
			halbb_set_igi_8852a_2(bb, set.lna_idx, set.tia_idx,
					      set.rxb_idx, RF_PATH_B);
	} else {
		halbb_set_igi_8852a_2(bb, set.lna_idx, set.tia_idx,
				      set.rxb_idx, RF_PATH_A);
		halbb_set_igi_8852a_2(bb, set.lna_idx, set.tia_idx,
				      set.rxb_idx, RF_PATH_B);
	}

	BB_DIG_DBG(bb, DIG_DBG_LV1, "Set (lna,tia,rxb)=((%d,%d,%02d))\n",
		   set.lna_idx, set.tia_idx, set.rxb_idx);
}

void halbb_dig_agc_update_8852a(struct bb_info *bb, struct bb_dig_op_para_unit *para)
{
	struct bb_dig_info *bb_dig = &bb->bb_dig_i;
	u8 igi_rssi_th_ifem[IGI_RSSI_TH_NUM] = {68, 84, 90, 98, 104};
	u8 igi_rssi_th_efem[IGI_RSSI_TH_NUM] = {68, 84, 90, 98, 104};
	u8 *igi_rssi_th;


	if (bb->ic_type != BB_RTL8852A)
		return;

	/* gain para update */
	bb_dig->lna_gain = bb_dig->lna_gain_g;
	bb_dig->tia_gain = bb_dig->tia_gain_g;

	/* igi rssi th update */
	switch (bb->phl_com->dev_cap.rfe_type) {
	case 51:
	case 52:
	case 53:
	case 54:
		igi_rssi_th = igi_rssi_th_efem;
		break;
	default:
		igi_rssi_th = igi_rssi_th_ifem;
	}

	halbb_mem_cpy(bb, &para->igi_rssi_th, igi_rssi_th, sizeof(u8) * IGI_RSSI_TH_NUM);

	BB_DIG_DBG(bb, DIG_DBG_LV1, "Sigi_rssi_th[4:0] = %d,%d,%d,%d,%d\n",
		   para->igi_rssi_th[4], para->igi_rssi_th[3],
		   para->igi_rssi_th[2], para->igi_rssi_th[1],
		   para->igi_rssi_th[0]);
}

#if 0
void halbb_dig_write_igi_8852a(struct bb_info *bb)
{
	struct bb_dig_info *bb_dig = &bb->bb_dig_i;
	struct bb_dig_op_unit *bb_dig_u = bb_dig->p_cur_dig_unit;

	halbb_dig_set_igi_cr_8852a(bb, bb_dig_u->cur_gaincode);
}
#endif

void halbb_gaincode_by_rssi_8852a(struct bb_info *bb,
			    struct agc_gaincode_set *set, u8 rssi) {
	struct bb_dig_info *bb_dig = &bb->bb_dig_i;

	if (bb->ic_type != BB_RTL8852A)
		return;

	set->lna_idx = halbb_lna_idx_by_rssi(bb, rssi);
	set->tia_idx = halbb_tia_idx_by_rssi(bb, rssi);
	set->rxb_idx = halbb_rxb_idx_by_rssi(bb, set, rssi);

	BB_DIG_DBG(bb, DIG_DBG_LV1, "final_rssi=%03d, (lna,tia,rab)=(%d,%d,%02d)\n",
	       rssi, set->lna_idx, set->tia_idx, set->rxb_idx);
}

bool halbb_dig_gaincode_update_en_8852a(struct bb_info *bb)
{
	struct bb_dig_info *bb_dig = &bb->bb_dig_i;
	struct bb_dig_op_unit *bb_dig_u = bb_dig->p_cur_dig_unit;
	struct rtw_hw_band *hw_band = &bb->hal_com->band[bb->bb_phy_idx];

	if ((bb->ic_type != BB_RTL8852A) || (bb->hal_com->cv >= CCV))
		return false;

	if (hw_band->cur_chandef.band != BAND_ON_24G)
		return false;

	halbb_gaincode_by_rssi_8852a(bb, &bb_dig_u->cur_gaincode, bb_dig_u->igi_fa_rssi);

	return true;
}

#endif

#ifdef HALBB_DIG_DAMPING_CHK
void halbb_dig_recorder_reset(struct bb_info *bb)
{
	struct bb_dig_info *dig = &bb->bb_dig_i;
	struct bb_dig_record_info *dig_rc = &dig->bb_dig_record_i;

	BB_DBG(bb, DBG_DIG,  "%s ======>\n", __func__);

	halbb_mem_set(bb, dig_rc, 0, sizeof(struct bb_dig_record_info));
}

void halbb_dig_recorder(struct bb_info *bb, u8 igi_curr, u32 fa_metrics)
{
	struct bb_dig_info *dig = &bb->bb_dig_i;
	struct bb_dig_record_info *dig_rc = &dig->bb_dig_record_i;
	u8 igi_pre = dig_rc->igi_history[0];
	u8 igi_up = 0;

//	if (!bb->bb_link_i.is_linked)
//		return;

	BB_DBG(bb, DBG_DIG,  "%s ======>\n", __func__);

#if 0
	if (bb->bb_link_i.first_connect) {
		BB_DBG(bb, DBG_DIG,  "first_connect\n");
		halbb_dig_recorder_reset(bb);
		dig_rc->igi_history[0] = igi_curr;
		dig_rc->fa_history[0] = fa_metrics;
		return;
	}
#endif

	igi_pre = dig_rc->igi_history[0];
	igi_up = (igi_curr > igi_pre) ? 1 : 0;
	dig_rc->igi_bitmap = (dig_rc->igi_bitmap << 1) | igi_up;

	dig_rc->igi_history[5] = dig_rc->igi_history[4];
	dig_rc->igi_history[4] = dig_rc->igi_history[3];
	dig_rc->igi_history[3] = dig_rc->igi_history[2];
	dig_rc->igi_history[2] = dig_rc->igi_history[1];
	dig_rc->igi_history[1] = dig_rc->igi_history[0];
	dig_rc->igi_history[0] = igi_curr;

	dig_rc->fa_history[5] = dig_rc->fa_history[4];
	dig_rc->fa_history[4] = dig_rc->fa_history[3];
	dig_rc->fa_history[3] = dig_rc->fa_history[2];
	dig_rc->fa_history[2] = dig_rc->fa_history[1];
	dig_rc->fa_history[1] = dig_rc->fa_history[0];
	dig_rc->fa_history[0] = fa_metrics;

	BB_DBG(bb, DBG_DIG, "igi_history[5:0] = {%02d, %02d, %02d, %02d, %02d, %02d}\n",
		dig_rc->igi_history[5], dig_rc->igi_history[4], dig_rc->igi_history[3], dig_rc->igi_history[2],
		dig_rc->igi_history[1], dig_rc->igi_history[0]);
	BB_DBG(bb, DBG_DIG, "fa_history[5:0]  = {%02d, %02d, %02d, %02d, %02d, %02d}\n",
	       dig_rc->fa_history[5], dig_rc->fa_history[4], dig_rc->fa_history[3], dig_rc->fa_history[2],
	       dig_rc->fa_history[1], dig_rc->fa_history[0]);
	BB_DBG(bb, DBG_DIG, "igi_bitmap[5:0]=0x%x{ %d,  %d,  %d,  %d,  %d,  %d}\n",
	       dig_rc->igi_bitmap,
	       (dig_rc->igi_bitmap & BIT5) >> 5,
	       (dig_rc->igi_bitmap & BIT4) >> 4,
	       (dig_rc->igi_bitmap & BIT3) >> 3,
	       (dig_rc->igi_bitmap & BIT2) >> 2,
	       (dig_rc->igi_bitmap & BIT1) >> 1,
	       dig_rc->igi_bitmap & BIT0);
}

void halbb_dig_damping_chk(struct bb_info *bb)
{
	struct bb_dig_info *dig = &bb->bb_dig_i;
	struct bb_dig_record_info *dig_rc = &dig->bb_dig_record_i;
	struct bb_dig_op_unit *bb_dig_u = dig->p_cur_dig_unit;
	u8 diff1 = 0, diff2 = 0;
	u32 fa_low_th = bb_dig_u->dig_op_para.fa_th[0];
	u32 fa_high_th = bb_dig_u->dig_op_para.fa_th[1];
	u32 fa_high_th2 = bb_dig_u->dig_op_para.fa_th[2];
	u32 fa_high_th3 = bb_dig_u->dig_op_para.fa_th[3];
	bool fa_pattern_match = false;
	u32 time_tmp = 0;

	if (!bb->bb_link_i.is_linked)
		return;

	BB_DBG(bb, DBG_DIG, "%s ======>\n", __func__);

	/*@== Release Damping ================================================*/
	if (dig_rc->damping_lock_en) {
		BB_DBG(bb, DBG_DIG,
		       "[Damping Limit!] limit_time=%d, phydm_sys_up_time=%d\n",
		       dig_rc->limit_time, bb->bb_sys_up_time);

		time_tmp = dig_rc->limit_time + DIG_LIMIT_PERIOD;

		if (DIFF_2(bb->bb_ch_i.rssi_min, dig_rc->limit_rssi) > dig->rls_rssi_diff_th ||
		    time_tmp < bb->bb_sys_up_time) {
			dig_rc->damping_lock_en = false;
			BB_DBG(bb, DBG_DIG, "[Rls] rssi=%d.%d, limit_rssi=%d.%d, th=%d.%d\n",
			       bb->bb_ch_i.rssi_min >> 1,
			       (bb->bb_ch_i.rssi_min & 1) * 5,
			       dig_rc->limit_rssi >> 1,
			       (dig_rc->limit_rssi & 1) * 5,
			       dig->rls_rssi_diff_th >> 1,
			       (dig->rls_rssi_diff_th & 1) * 5);
		}
		return;
	}

	/*@== Damping Pattern Check===========================================*/
	BB_DBG(bb, DBG_DIG, "fa_th{H2, H2, H1, L}= {%d,%d,%d,%d}\n",
	       fa_high_th3, fa_high_th2, fa_high_th, fa_low_th);

	if ((dig_rc->igi_bitmap & 0xf) == 0x5) {
		BB_DBG(bb, DBG_DIG, "[Type:0] map=0x5\n");
	/*@ 4b'0101
	* IGI:[3]down(0x24)->[2]up(0x26)->[1]down(0x24)->[0]up(0x26)->[new](Lock @ 0x26)
	* FA: [3] >high1   ->[2] <low   ->[1] >high1   ->[0] <low   ->[new]   <low
	*
	* IGI:[3]down(0x24)->[2]up(0x28)->[1]down(0x24)->[0]up(0x28)->[new](Lock @ 0x28)
	* FA: [3] >high2   ->[2] <low   ->[1] >high2   ->[0] <low   ->[new]   <low
	*/
		if (dig_rc->igi_history[0] > dig_rc->igi_history[1])
			diff1 = dig_rc->igi_history[0] - dig_rc->igi_history[1];

		if (dig_rc->igi_history[2] > dig_rc->igi_history[3])
			diff2 = dig_rc->igi_history[2] - dig_rc->igi_history[3];

		if (dig_rc->fa_history[0] < fa_low_th &&
		    dig_rc->fa_history[1] >= fa_high_th &&
		    dig_rc->fa_history[2] < fa_low_th &&
		    dig_rc->fa_history[3] >= fa_high_th) {
		    /*@Check each fa element*/
			fa_pattern_match = true;
		}

	} else if ((dig_rc->igi_bitmap & 0x1f) == 0x9) {
		BB_DBG(bb, DBG_DIG, "[Type:1] map=0x9\n");
	/*@ 5b'01001
	* IGI:[3]up(0x28)->[2]down(0x26)->[1]down(0x24)->[0]up(0x28)->[new](Lock @ 0x28)
	* FA: [3]  <low  ->[2] <low     ->[1] >high2   ->[0] <low   ->[new]  <low
	*/
		if (dig_rc->igi_history[0] > dig_rc->igi_history[1])
			diff1 = dig_rc->igi_history[0] - dig_rc->igi_history[1];

		if (dig_rc->igi_history[3] > dig_rc->igi_history[4])
			diff2 = dig_rc->igi_history[3] - dig_rc->igi_history[4];

		if (dig_rc->fa_history[0] < fa_low_th &&
		    dig_rc->fa_history[1] >= fa_high_th2 &&
		    dig_rc->fa_history[2] < fa_low_th &&
		    dig_rc->fa_history[3] < fa_low_th) {
		    /*@Check each fa element*/
			fa_pattern_match = true;
		}
	} else if ((dig_rc->igi_bitmap & 0x3f) == 0x11) {
		BB_DBG(bb, DBG_DIG, "[Type:2] map=0x11\n");
	/*@ 6b'010001
	* IGI:[4]up(0x28)->[3]down(0x26)->[2]down(0x24)->[1]down(0x22)->[0]up(0x28)->[new](Lock @ 0x28)
	* FA: [4]  <low  ->[3] <low     ->[2] <low     ->[1] >high3   ->[0] <low   ->[new]  <low
	*/
		if (dig_rc->igi_history[0] > dig_rc->igi_history[1])
			diff1 = dig_rc->igi_history[0] - dig_rc->igi_history[1];

		if (dig_rc->igi_history[4] > dig_rc->igi_history[5])
			diff2 = dig_rc->igi_history[4] - dig_rc->igi_history[5];

		if (dig_rc->fa_history[0] < fa_low_th &&
		    dig_rc->fa_history[1] >= fa_high_th3 &&
		    dig_rc->fa_history[2] < fa_low_th &&
		    dig_rc->fa_history[3] < fa_low_th &&
		    dig_rc->fa_history[4] < fa_low_th) {
		    /*@Check each fa element*/
			fa_pattern_match = true;
		}
	}

	if (diff1 >= 2 && diff2 >= 2 && fa_pattern_match) {
		dig_rc->damping_lock_en = true;
		dig_rc->damping_limit_val = dig_rc->igi_history[0];
		dig_rc->limit_time = bb->bb_sys_up_time;
		dig_rc->limit_rssi = bb->bb_ch_i.rssi_min;

		BB_DBG(bb, DBG_DIG,
		       "[Start damping_limit!] IGI_dyn_min=0x%x, limit_time=%d, limit_rssi=%d\n",
		       dig_rc->damping_limit_val,
		       dig_rc->limit_time, dig_rc->limit_rssi >> 1);
	}
	BB_DBG(bb, DBG_DIG,  "[lock_en=%d] ptrn_match=%d, diff1=%d, diff2=%d\n",
	       dig_rc->damping_lock_en, fa_pattern_match, diff1, diff2);
}

void halbb_dig_damping_chk_init(struct bb_info *bb)
{
	struct bb_dig_info *dig = &bb->bb_dig_i;
	struct bb_dig_record_info *dig_rc = &dig->bb_dig_record_i;

	halbb_dig_recorder_reset(bb);
	dig->dig_dl_en = true;
	dig->rls_rssi_diff_th = 6; /*6 >> 1 = 3dB*/
}
#endif

u8 halbb_get_lna_idx(struct bb_info *bb, enum rf_path path)
{
	struct bb_dig_info *bb_dig = &bb->bb_dig_i;
	const struct bb_dig_cr_info *cr = &bb_dig->bb_dig_cr_i;
	u8 lna_idx = LNA_IDX_MAX;

	/*lna initial gain index*/
	switch (path) {
	case RF_PATH_A:
		lna_idx = (u8)halbb_get_reg(bb, cr->path0_lna_init_idx,
					    cr->path0_lna_init_idx_m);
		break;
	case RF_PATH_B:
		lna_idx = (u8)halbb_get_reg(bb, cr->path1_lna_init_idx,
					    cr->path1_lna_init_idx_m);
		break;
	default:
		break;
	}

	return lna_idx;
}

u8 halbb_get_tia_idx(struct bb_info *bb, enum rf_path path)
{
	struct bb_dig_info *bb_dig = &bb->bb_dig_i;
	const struct bb_dig_cr_info *cr = &bb_dig->bb_dig_cr_i;
	u8 tia_idx = TIA_IDX_MAX;

	switch (path) {
	case RF_PATH_A:
		tia_idx = (u8)halbb_get_reg(bb, cr->path0_tia_init_idx,
					    cr->path0_tia_init_idx_m);
		break;
	case RF_PATH_B:
		tia_idx = (u8)halbb_get_reg(bb, cr->path1_tia_init_idx,
					    cr->path1_tia_init_idx_m);
		break;
	default:
		break;
	}

	return tia_idx;
}

u8 halbb_get_rxb_idx(struct bb_info *bb, enum rf_path path)
{
	struct bb_dig_info *bb_dig = &bb->bb_dig_i;
	const struct bb_dig_cr_info *cr = &bb_dig->bb_dig_cr_i;
	u8 rxb_idx = RXB_IDX_MAX;

	switch (path) {
	case RF_PATH_A:
		rxb_idx = (u8)halbb_get_reg(bb, cr->path0_rxb_init_idx,
					    cr->path0_rxb_init_idx_m);
		break;
	case RF_PATH_B:
		rxb_idx = (u8)halbb_get_reg(bb, cr->path1_rxb_init_idx,
					    cr->path1_rxb_init_idx_m);
		break;
	default:
		break;
	}

	return rxb_idx;
}

u8 halbb_igi_by_edcca(struct bb_info *bb, u8 igi)
{
#ifdef HALBB_EDCCA_SUPPORT
	struct bb_dig_info *bb_dig = &bb->bb_dig_i;
	const u8 margin = IGI_EDCCA_GAP_LIMIT - 18; /* -128(dBm)+110(RSSI) */
	u8 bound = bb->bb_edcca_i.th_h;

	bound = margin > (EDCCA_MAX - bound) ? EDCCA_MAX : (bound + margin);
	if (igi > bound) {
		igi = bound;
		BB_DIG_DBG(bb, DIG_DBG_LV0, "EDCCA th_h = %d, IGI upper clamp to %d.\n",
			   bb->bb_edcca_i.th_h, bound);
	}
#endif
	return igi;
}

void halbb_dig_noisy_lv_decision(struct bb_info *bb)
{
	struct bb_dig_info *bb_dig = &bb->bb_dig_i;
	struct bb_dig_op_unit *bb_dig_u = bb_dig->p_cur_dig_unit;
	struct bb_dig_op_para_unit *para = &bb_dig_u->dig_op_para;
	u16 fa_ratio = bb_dig_u->fa_r_avg;

	if (bb_dig_u->fa_valid_state_cnt == 0) {
		bb_dig_u->cur_noisy_lv = DIG_NOISY_LV1;
		BB_DIG_DBG(bb, DIG_DBG_LV1, "fa_ratio N/A, set noisy_lv=%d\n", DIG_NOISY_LV1);
		return;
	}

	if (fa_ratio < para->fa_th[0])
		bb_dig_u->cur_noisy_lv = DIG_NOISY_LV0;
	else if (fa_ratio < para->fa_th[1])
		bb_dig_u->cur_noisy_lv = DIG_NOISY_LV1;
	else if (fa_ratio < para->fa_th[2])
		bb_dig_u->cur_noisy_lv = DIG_NOISY_LV2;
	else if (fa_ratio < para->fa_th[3])
		bb_dig_u->cur_noisy_lv = DIG_NOISY_LV3;
	else
		bb_dig_u->cur_noisy_lv = DIG_NOISY_LV_MAX;

	BB_DIG_DBG(bb, DIG_DBG_LV1, "fa_th: [+6 (%d) +4 (%d) +2 (%d) 0 (%d) -2 ]\n",
		   para->fa_th[3], para->fa_th[2], para->fa_th[1], para->fa_th[0]);

	BB_DIG_DBG(bb, DIG_DBG_LV1, "fa_avg=%d, noisy_lv=%d\n",
		   bb_dig_u->fa_r_avg, bb_dig_u->cur_noisy_lv);

	bb_dig_u->fa_valid_state_cnt = 0;
}

s8 halbb_dig_ofst_by_fa(struct bb_info *bb, u16 fa)
{
	struct bb_dig_info *bb_dig = &bb->bb_dig_i;
	struct bb_dig_op_para_unit *para = &bb_dig->p_cur_dig_unit->dig_op_para;
	//enum dig_noisy_level noisy_lv;
	s8 ofst = 0;

	if (fa < para->fa_th[0]) {
		//noisy_lv = DIG_NOISY_LV0;
		ofst = -2;
	} else if (fa < para->fa_th[1]) {
		//noisy_lv = DIG_NOISY_LV1;
		ofst = 0;
	} else if (fa < para->fa_th[2]) {
		//noisy_lv = DIG_NOISY_LV2;
		ofst = 2;
	} else if (fa < para->fa_th[3]) {
		//noisy_lv = DIG_NOISY_LV3;
		ofst = 4;
	} else {
		//noisy_lv = DIG_NOISY_LV_MAX;
		ofst = 6;
	}

	BB_DIG_DBG(bb, DIG_DBG_LV1, "fa_th: [+6 (%d) +4 (%d) +2 (%d) 0 (%d) -2 ]\n",
		   para->fa_th[3], para->fa_th[2], para->fa_th[1], para->fa_th[0]);

	BB_DIG_DBG(bb, DIG_DBG_LV1, "fa=%d, ofst=%d\n", fa, ofst);

	return ofst;
}

u8 halbb_dig_igi_by_ofst(struct bb_info *bb, u8 igi_pre, s8 ofst)
{
	struct bb_dig_info *dig = &bb->bb_dig_i;
	struct bb_dig_op_unit *dig_u = dig->p_cur_dig_unit;
	#ifdef HALBB_DIG_DAMPING_CHK
	struct bb_dig_record_info *dig_rc = &dig->bb_dig_record_i;
	#endif
	u8 joint_max, joint_min;
	u8 dyn_min, dyn_max;
	u8 igi_new;

	igi_new = (u8)((s8)igi_pre + ofst);
	dyn_min = SUBTRACT_TO_0(dig->igi_rssi, 10);
	dyn_max = dyn_min + IGI_OFFSET_MAX;

	#ifdef HALBB_DIG_DAMPING_CHK
	/*@Limit Dyn min by damping*/
	if (dig->dig_dl_en &&
	    dig_rc->damping_lock_en &&
	    dyn_min < dig_rc->damping_limit_val) {
		BB_DBG(bb, DBG_DIG, "[Limit by Damping] Dyn_min=0x%x -> 0x%x\n",
		       dyn_min, dig_rc->damping_limit_val);

		dyn_min = dig_rc->damping_limit_val;
	}
	#endif

	joint_max = MIN_2(dyn_max, dig_u->abs_igi_max);
	joint_min = MAX_2(dyn_min, dig_u->abs_igi_min);

	if (joint_max >= joint_min) {
	/*Check IGI exceed the max/in boundary or not*/
		if (igi_new > joint_max)
			igi_new = joint_max;
		else if (igi_new < joint_min)
			igi_new = joint_min;
	} else {
		igi_new = joint_max;
	}

	BB_DIG_DBG(bb, DIG_DBG_LV0,
		   "rssi=%02d, dyn(max,min)=(%d,%d), abs(max,min)=(%d,%d), Joint(max,min)=(%d,%d), igi=%d\n",
		   dig->igi_rssi,
		   dyn_max, dyn_min,
		   dig_u->abs_igi_max, dig_u->abs_igi_min,
		   joint_max, joint_min,
		   igi_new);

	return igi_new;
}

void halbb_dig_igi_ofst_by_env(struct bb_info *bb)
{
	struct bb_dig_info *bb_dig = &bb->bb_dig_i;
	struct bb_dig_op_unit *bb_dig_u = bb_dig->p_cur_dig_unit;
	u8 fa_rssi_ofst_pre = bb_dig_u->fa_rssi_ofst;

	switch (bb_dig_u->cur_noisy_lv) {
	case DIG_NOISY_LV0:
		if (bb_dig_u->fa_rssi_ofst < 2)
			bb_dig_u->fa_rssi_ofst = 0;
		else
			bb_dig_u->fa_rssi_ofst -= 2;
		break;
	case DIG_NOISY_LV2:
		bb_dig_u->fa_rssi_ofst += 2;
		break;
	case DIG_NOISY_LV3:
		bb_dig_u->fa_rssi_ofst += 4;
		break;
	case DIG_NOISY_LV_MAX:
		bb_dig_u->fa_rssi_ofst += 6;
		break;
	default:
		break;
	}

	if (bb_dig_u->fa_rssi_ofst > IGI_OFFSET_MAX)
		bb_dig_u->fa_rssi_ofst = IGI_OFFSET_MAX;

	BB_DIG_DBG(bb, DIG_DBG_LV0, "[noisy_lv=%d] ofst: %d -> %d (max: %d)\n",
		   bb_dig_u->cur_noisy_lv, fa_rssi_ofst_pre, bb_dig_u->fa_rssi_ofst, IGI_OFFSET_MAX);
}

u8 halbb_dig_igi_bound_decision(struct bb_info *bb)
{
	struct bb_dig_info *bb_dig = &bb->bb_dig_i;
	struct bb_dig_op_unit *bb_dig_u = bb_dig->p_cur_dig_unit;
	#ifdef HALBB_DIG_DAMPING_CHK
	struct bb_dig_record_info *dig_rc = &bb_dig->bb_dig_record_i;
	#endif
	u8 tmp_max_bound = bb_dig_u->abs_igi_max;
	u8 tmp_min_bound = bb_dig_u->abs_igi_min;
	u8 igi_new;

	if (bb_dig->igi_rssi < 10)
		bb_dig_u->dyn_igi_min = 0;
	else
		bb_dig_u->dyn_igi_min = bb_dig->igi_rssi - 10;

	bb_dig_u->dyn_igi_max = bb_dig_u->dyn_igi_min + IGI_OFFSET_MAX;
	igi_new = bb_dig_u->dyn_igi_min + bb_dig_u->fa_rssi_ofst;

	#ifdef HALBB_DIG_DAMPING_CHK
	/*@Limit Dyn min by damping*/
	if (bb_dig->dig_dl_en &&
	    dig_rc->damping_lock_en &&
	    bb_dig_u->dyn_igi_min < dig_rc->damping_limit_val) {
		BB_DBG(bb, DBG_DIG,
			  "[Limit by Damping] Dig_dyn_min=0x%x -> 0x%x\n",
			  bb_dig_u->dyn_igi_min, dig_rc->damping_limit_val);

		bb_dig_u->dyn_igi_min = dig_rc->damping_limit_val;
	}
	#endif

	tmp_max_bound = MIN_2(bb_dig_u->dyn_igi_max, tmp_max_bound);
	tmp_min_bound = MAX_2(bb_dig_u->dyn_igi_min, tmp_min_bound);

	if (igi_new > tmp_max_bound)
		igi_new = tmp_max_bound;
	else if (igi_new < tmp_min_bound)
		igi_new = tmp_min_bound;

	BB_DIG_DBG(bb, DIG_DBG_LV0, "rssi_min=%02d, dyn(max,min)=(%d,%d), abs(max,min)=(%d,%d), igi=%d\n",
		   bb_dig->igi_rssi,
		   bb_dig_u->dyn_igi_max, bb_dig_u->dyn_igi_min,
		   bb_dig_u->abs_igi_max, bb_dig_u->abs_igi_min,
		   igi_new);

	return igi_new;
}

bool halbb_dig_fahm_trig(struct bb_info *bb, u16 mntr_time)
{
	struct bb_dig_info *bb_dig = &bb->bb_dig_i;
	bool is_trig_success = false;
#ifdef HALBB_ENV_MNTR_SUPPORT
	struct fahm_trig_report rpt = {0};

	/* trigger fahm_rpt */
	bb_dig->fahm_para_i.fahm_mntr_time = mntr_time - 15;
	if (halbb_fahm_trigger(bb, &bb_dig->fahm_para_i, &rpt)) {
		BB_DIG_DBG(bb, DIG_DBG_LV2,
			   "fahm trigger ok, timestamp %d, mntr_time %d ms.\n",
			   rpt.fahm_rpt_stamp,
			   bb_dig->fahm_para_i.fahm_mntr_time);
		bb_dig->fahm_timestamp = rpt.fahm_rpt_stamp;
		bb_dig->fahm_is_triggered = true;
		is_trig_success = true;
	} else {
		BB_DIG_DBG(bb, DIG_DBG_LV1, "fahm trigger fail.\n");
	}
#endif
	return is_trig_success;
}

bool halbb_dig_fahm_latch(struct bb_info *bb)
{
	struct bb_dig_info *dig = &bb->bb_dig_i;
	struct bb_dig_op_unit *dig_u = dig->p_cur_dig_unit;
#ifdef HALBB_ENV_MNTR_SUPPORT
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;
	struct fahm_report rpt = {0};
	bool fahm_rpt_result = false;

	if (env->fahm_manual_ctrl) {
		BB_DIG_DBG(bb, DIG_DBG_LV1, "FAHM in manual ctrl\n");
		return false;
	}

	fahm_rpt_result = halbb_fahm_result(bb, &rpt);

	/* get fahm_rpt and accumulate */
	if (!dig->fahm_is_triggered) {
		BB_DIG_DBG(bb, DIG_DBG_LV1, "[Latch Err] DIG not trigger.\n");
		return false;
	} else {
		dig->fahm_is_triggered = false;
	}

	if (fahm_rpt_result == false) {
		BB_DIG_DBG(bb, DIG_DBG_LV1, "[Latch Err] FA rpt not valid.\n");
		return false;
	}

	if (dig->fahm_timestamp != rpt.fahm_rpt_stamp) {
		BB_DIG_DBG(bb, DIG_DBG_LV1, "[Stamp Err] %d, mine: %d.\n",
			   rpt.fahm_rpt_stamp, dig->fahm_timestamp);
		return false;
	}

	dig_u->fa_r_acc += rpt.fahm_permil;
	dig_u->fa_valid_state_cnt++;

	BB_DIG_DBG(bb, DIG_DBG_LV2, "[FA] CCK + OFDM = ALL(%d)\n",
		   rpt.fahm_permil);

	BB_DIG_DBG(bb, DIG_DBG_LV2, "[FA] acc: %d, cnt: %d\n",
		   dig_u->fa_r_acc, dig_u->fa_valid_state_cnt);

#endif
	return true;
}

void halbb_sdagc_follow_pagc_config(struct bb_info *bb, bool set_en)
{
	struct bb_dig_info *bb_dig = &bb->bb_dig_i;
	const struct bb_dig_cr_info *cr = &bb_dig->bb_dig_cr_i;
	u32 val = (set_en) ? 1 : 0;
	u8 i = 0;

	if (bb_dig->p_cur_dig_unit->sdagc_follow_pagc_en == set_en)
		return;

	bb_dig->p_cur_dig_unit->sdagc_follow_pagc_en = set_en;

	halbb_set_reg_cmn(bb, cr->path0_p20_follow_by_pagcugc_en_a,
			  cr->path0_p20_follow_by_pagcugc_en_a_m, val, bb->bb_phy_idx);
	halbb_set_reg_cmn(bb, cr->path0_s20_follow_by_pagcugc_en_a,
			  cr->path0_s20_follow_by_pagcugc_en_a_m, val, bb->bb_phy_idx);
	halbb_set_reg_cmn(bb, cr->path1_p20_follow_by_pagcugc_en_a,
			  cr->path1_p20_follow_by_pagcugc_en_a_m, val, bb->bb_phy_idx);
	halbb_set_reg_cmn(bb, cr->path1_s20_follow_by_pagcugc_en_a,
			  cr->path1_s20_follow_by_pagcugc_en_a_m, val, bb->bb_phy_idx);

	BB_DIG_DBG(bb, DIG_DBG_LV1, "sdagc_follow_pagc=%d\n", val);
}

void halbb_dyn_pd_th_cck(struct bb_info *bb, u8 rssi, bool set_en)
{
	struct bb_dig_info *bb_dig = &bb->bb_dig_i;
	struct bb_dig_op_unit *bb_dig_u = bb_dig->p_cur_dig_unit;
	const struct bb_dig_cr_info *cr = &bb_dig->bb_dig_cr_i;
	u8 pd_dyn_max = bb_dig->igi_rssi + 5; /* PD_low upper bound */
	u8 margin = bb_dig_u->pd_low_th_ofst; /* backoff of CCA ability */
	u8 phy = bb->bb_phy_idx == HW_PHY_1 ? 1 : 0;
	enum channel_width cbw = bb->hal_com->band[phy].cur_chandef.bw;

	BB_DIG_DBG(bb, DIG_DBG_LV1, "%s ======>\n", __func__);

	rssi = MIN_2(rssi, pd_dyn_max);
	rssi -= MIN_2(rssi, margin);

	if(!set_en) {
		halbb_set_pd_lower_bound_cck(bb, RSSI_MIN, cbw, bb->bb_phy_idx);
		BB_DIG_DBG(bb, DIG_DBG_LV0, "Dynamic CCK PD th dsiabled\n");
	} else {
		if(!halbb_set_pd_lower_bound_cck(bb, RSSI_MAX - rssi, cbw,
		   bb->bb_phy_idx))
			BB_DIG_DBG(bb, DIG_DBG_LV0, "CCK PD th set warning.\n");
		else
			BB_DIG_DBG(bb, DIG_DBG_LV1, "dyn_max=%d, backoff=%d, pd_th=%d(-%ddBm)\n",
				   pd_dyn_max, margin, rssi, RSSI_MAX - rssi);
	}
}

void halbb_dyn_pd_th_ofdm(struct bb_info *bb, u8 rssi, bool set_en)
{
	struct bb_dig_info *bb_dig = &bb->bb_dig_i;
	struct bb_dig_op_unit *bb_dig_u = bb_dig->p_cur_dig_unit;
	const struct bb_dig_cr_info *cr = &bb_dig->bb_dig_cr_i;
	u8 pd_dyn_max = bb_dig->igi_rssi + 5; /* PD_low upper bound */
	u8 margin = bb_dig_u->pd_low_th_ofst; /* backoff of CCA ability */
	u8 phy = bb->bb_phy_idx == HW_PHY_1 ? 1 : 0;
	enum channel_width cbw = bb->hal_com->band[phy].cur_chandef.bw;

	BB_DIG_DBG(bb, DIG_DBG_LV1, "%s ======>\n", __func__);

	rssi = MIN_2(rssi, pd_dyn_max);
	rssi -= MIN_2(rssi, margin);

	if(!set_en) {
		halbb_set_pd_lower_bound(bb, RSSI_MIN, cbw, bb->bb_phy_idx);
		BB_DIG_DBG(bb, DIG_DBG_LV0, "Dynamic PD th dsiabled\n");
	} else {
		if(!halbb_set_pd_lower_bound(bb, RSSI_MAX - rssi, cbw,
		   bb->bb_phy_idx))
			BB_DIG_DBG(bb, DIG_DBG_LV0, "PD th set warning.\n");
		else
			BB_DIG_DBG(bb, DIG_DBG_LV1, "pd_dyn_max=%d, backoff=%d, pd_th_eq_rssi=%d(-%ddBm)\n",
				   pd_dyn_max, margin, rssi, RSSI_MAX - rssi);
	}
}

void halbb_dig_mode_update(struct bb_info *bb, enum dig_op_mode mode, enum phl_phy_idx phy_idx)
{
	struct bb_dig_info *bb_dig = &bb->bb_dig_i;

	switch (mode) {
	case DIG_ORIGIN:
	case DIG_SIMPLE:
#ifdef HALBB_DIG_TDMA_SUPPORT
	case DIG_TDMA:
	case DIG_TDMA_ADV:
#endif
		bb_dig->dig_mode = mode;
		break;
	default:
		bb_dig->dig_mode = DIG_ORIGIN;
	}

	BB_DIG_DBG(bb, DIG_DBG_LV0, "Set DIG op mode %d\n", bb_dig->dig_mode);
}

void halbb_dig_gain_para_init(struct bb_info *bb)
{
	struct bb_dig_info *bb_dig = &bb->bb_dig_i;
	struct bb_dig_cr_info *cr = &bb_dig->bb_dig_cr_i;
	s8 *gain_arr = NULL;
	u32 tmp_val, i = 0;

	tmp_val = halbb_get_reg_cmn(bb, cr->path0_ib_pkpwr,
				    cr->path0_ib_pkpwr_m, bb->bb_phy_idx);
	bb_dig->ib_pkpwr = (s8)halbb_cnvrt_2_sign(tmp_val >> 2, 8);

	bb_dig->ib_pbk = (u8)halbb_get_reg_cmn(bb, cr->path0_ib_pbk,
					       cr->path0_ib_pbk_m,
					       bb->bb_phy_idx);

	/*=== [2G Gain Table] =================*/
	/*LNA*/
	gain_arr = bb_dig->lna_gain_g;
	tmp_val = halbb_get_reg_cmn(bb, cr->path0_lna_err_g0_g,
				    cr->path0_lna_err_g0_g_m, bb->bb_phy_idx);
	gain_arr[0] = (s8)halbb_cnvrt_2_sign(tmp_val >> 2, 4) + LNA0_GAIN;
	tmp_val = halbb_get_reg_cmn(bb, cr->path0_lna_err_g1_g,
				    cr->path0_lna_err_g1_g_m, bb->bb_phy_idx);
	gain_arr[1] = (s8)halbb_cnvrt_2_sign(tmp_val >> 2, 4) + LNA1_GAIN;
	tmp_val = halbb_get_reg_cmn(bb, cr->path0_lna_err_g2_g,
				    cr->path0_lna_err_g2_g_m, bb->bb_phy_idx);
	gain_arr[2] = (s8)halbb_cnvrt_2_sign(tmp_val >> 2, 4) + LNA2_GAIN;
	tmp_val = halbb_get_reg_cmn(bb, cr->path0_lna_err_g3_g,
				    cr->path0_lna_err_g3_g_m, bb->bb_phy_idx);
	gain_arr[3] = (s8)halbb_cnvrt_2_sign(tmp_val >> 2, 4) + LNA3_GAIN;
	tmp_val = halbb_get_reg_cmn(bb, cr->path0_lna_err_g4_g,
				    cr->path0_lna_err_g4_g_m, bb->bb_phy_idx);
	gain_arr[4] = (s8)halbb_cnvrt_2_sign(tmp_val >> 2, 4) + LNA4_GAIN;
	tmp_val = halbb_get_reg_cmn(bb, cr->path0_lna_err_g5_g,
				    cr->path0_lna_err_g5_g_m, bb->bb_phy_idx);
	gain_arr[5] = (s8)halbb_cnvrt_2_sign(tmp_val >> 2, 4) + LNA5_GAIN;
	tmp_val = halbb_get_reg_cmn(bb, cr->path0_lna_err_g6_g,
				    cr->path0_lna_err_g6_g_m, bb->bb_phy_idx);
	gain_arr[6] = (s8)halbb_cnvrt_2_sign(tmp_val >> 2, 4) + LNA6_GAIN;

	/*TIA*/
	gain_arr = bb_dig->tia_gain_g;
	tmp_val = halbb_get_reg_cmn(bb, cr->path0_tia_err_g0_g,
				    cr->path0_tia_err_g0_g_m, bb->bb_phy_idx);
	gain_arr[0] = (s8)halbb_cnvrt_2_sign(tmp_val >> 2, 4) + TIA0_GAIN_G;
	tmp_val = halbb_get_reg_cmn(bb, cr->path0_tia_err_g1_g,
				    cr->path0_tia_err_g1_g_m, bb->bb_phy_idx);
	gain_arr[1] = (s8)halbb_cnvrt_2_sign(tmp_val >> 2, 4) + TIA1_GAIN_G;

	/*=== [5G Gain Table] =================*/
	/*LNA*/
	gain_arr = bb_dig->lna_gain_a;
	tmp_val = halbb_get_reg_cmn(bb, cr->path0_lna_err_g0_a,
				    cr->path0_lna_err_g0_a_m, bb->bb_phy_idx);
	gain_arr[0] = (s8)halbb_cnvrt_2_sign(tmp_val >> 2, 4) + LNA0_GAIN;
	tmp_val = halbb_get_reg_cmn(bb, cr->path0_lna_err_g1_a,
				    cr->path0_lna_err_g1_a_m, bb->bb_phy_idx);
	gain_arr[1] = (s8)halbb_cnvrt_2_sign(tmp_val >> 2, 4) + LNA1_GAIN;
	tmp_val = halbb_get_reg_cmn(bb, cr->path0_lna_err_g2_a,
				    cr->path0_lna_err_g2_a_m, bb->bb_phy_idx);
	gain_arr[2] = (s8)halbb_cnvrt_2_sign(tmp_val >> 2, 4) + LNA2_GAIN;
	tmp_val = halbb_get_reg_cmn(bb, cr->path0_lna_err_g3_a,
				    cr->path0_lna_err_g3_a_m, bb->bb_phy_idx);
	gain_arr[3] = (s8)halbb_cnvrt_2_sign(tmp_val >> 2, 4) + LNA3_GAIN;
	tmp_val = halbb_get_reg_cmn(bb, cr->path0_lna_err_g4_a,
				    cr->path0_lna_err_g4_a_m, bb->bb_phy_idx);
	gain_arr[4] = (s8)halbb_cnvrt_2_sign(tmp_val >> 2, 4) + LNA4_GAIN;
	tmp_val = halbb_get_reg_cmn(bb, cr->path0_lna_err_g5_a,
				    cr->path0_lna_err_g5_a_m, bb->bb_phy_idx);
	gain_arr[5] = (s8)halbb_cnvrt_2_sign(tmp_val >> 2, 4) + LNA5_GAIN;
	tmp_val = halbb_get_reg_cmn(bb, cr->path0_lna_err_g6_a,
				    cr->path0_lna_err_g6_a_m, bb->bb_phy_idx);
	gain_arr[6] = (s8)halbb_cnvrt_2_sign(tmp_val >> 2, 4) + LNA6_GAIN;

	/*TIA*/
	gain_arr = bb_dig->tia_gain_a;
	tmp_val = halbb_get_reg_cmn(bb, cr->path0_tia_err_g0_a,
				    cr->path0_tia_err_g0_a_m, bb->bb_phy_idx);
	gain_arr[0] = (s8)halbb_cnvrt_2_sign(tmp_val >> 2, 4) + TIA0_GAIN_A;
	tmp_val = halbb_get_reg_cmn(bb, cr->path0_tia_err_g1_a,
				    cr->path0_tia_err_g1_a_m, bb->bb_phy_idx);
	gain_arr[1] = (s8)halbb_cnvrt_2_sign(tmp_val >> 2, 4) + TIA1_GAIN_A;

	BB_DIG_DBG(bb, DIG_DBG_LV0, "ib_pkpwr=%d, ib_pbk=%d\n",
		   bb_dig->ib_pkpwr, bb_dig->ib_pbk);

	for (i = 0; i < BB_LNA_SIZE; i++)
		BB_DIG_DBG(bb, DIG_DBG_LV0, "lna_gain_g[%d]=%d\n",
			   i, bb_dig->lna_gain_g[i]);
	for (i = 0; i < BB_TIA_SIZE; i++)
		BB_DIG_DBG(bb, DIG_DBG_LV0, "tia_gain_g[%d]=%d\n",
			   i, bb_dig->tia_gain_g[i]);
	for (i = 0; i < BB_LNA_SIZE; i++)
		BB_DIG_DBG(bb, DIG_DBG_LV0, "lna_gain_a[%d]=%d\n",
			   i, bb_dig->lna_gain_a[i]);
	for (i = 0; i < BB_TIA_SIZE; i++)
		BB_DIG_DBG(bb, DIG_DBG_LV0, "tia_gain_a[%d]=%d\n",
			   i, bb_dig->tia_gain_a[i]);

	/*lna initial gain index*/
	if (bb->hal_com->dbcc_en && (bb->bb_phy_idx == HW_PHY_1)) {
		bb_dig->max_gaincode.lna_idx = halbb_get_lna_idx(bb, RF_PATH_B);
		bb_dig->max_gaincode.tia_idx = halbb_get_tia_idx(bb, RF_PATH_B);
		bb_dig->max_gaincode.rxb_idx = halbb_get_rxb_idx(bb, RF_PATH_B);

	} else {
		bb_dig->max_gaincode.lna_idx = halbb_get_lna_idx(bb, RF_PATH_A);
		bb_dig->max_gaincode.tia_idx = halbb_get_tia_idx(bb, RF_PATH_A);
		bb_dig->max_gaincode.rxb_idx = halbb_get_rxb_idx(bb, RF_PATH_A);
	}
	BB_DIG_DBG(bb, DIG_DBG_LV0, "Read max gaincode = (%d,%d,%2d)\n",
		   bb_dig->max_gaincode.lna_idx, bb_dig->max_gaincode.tia_idx,
		   bb_dig->max_gaincode.rxb_idx);
}


/* fa_cnt 2000/4000/5000/8000 */
static const u16 fa_th_no_link[FA_TH_NUM] = {196, 352, 440, 528};
static const u16 fa_th_linked[FA_TH_NUM] = {4, 8, 12, 16};

void halbb_dig_para_update(struct bb_info *bb)
{
	struct bb_dig_info *bb_dig = &bb->bb_dig_i;
	struct bb_link_info *bb_link = &bb->bb_link_i;
	struct bb_dig_op_para_unit *para_dst;

	BB_DIG_DBG(bb, DIG_DBG_LV1, "%s ======>\n", __func__);

	/* IGI and PT_low enable control */
	para_dst = &bb_dig->dig_state_h_i.dig_op_para;
	para_dst->dyn_pd_th_en = true;

#ifdef BB_8852A_2_SUPPORT
	if (bb->ic_type == BB_RTL8852A)
		halbb_dig_agc_update_8852a(bb, para_dst);
#endif

	/* fa th update */
	if (!bb->bb_link_i.is_linked)
		halbb_mem_cpy(bb, &para_dst->fa_th, fa_th_no_link,
			      sizeof(u16) * FA_TH_NUM);
	else
		halbb_mem_cpy(bb, &para_dst->fa_th, fa_th_linked,
			      sizeof(u16) * FA_TH_NUM);

	BB_DIG_DBG(bb, DIG_DBG_LV1, "is_linked=%d, fa_th[3:0] = %d,%d,%d,%d\n",
		   bb->bb_link_i.is_linked,
		   para_dst->fa_th[3], para_dst->fa_th[2], para_dst->fa_th[1],
		   para_dst->fa_th[0]);

#ifdef HALBB_DIG_TDMA_SUPPORT
	para_dst = &bb_dig->dig_state_l_i.dig_op_para;
	halbb_mem_cpy(bb, para_dst, &bb_dig->dig_state_h_i.dig_op_para,
		      sizeof(struct bb_dig_op_para_unit));
#endif
}

void halbb_dig_fa_info_update(struct bb_info *bb)
{
	struct bb_dig_info *bb_dig = &bb->bb_dig_i;
	struct bb_dig_op_unit *bb_dig_u = bb_dig->p_cur_dig_unit;

	/* average from env_mntr_rpt */
	bb_dig_u->fa_r_avg = HALBB_DIV(bb_dig_u->fa_r_acc, bb_dig_u->fa_valid_state_cnt);
	bb_dig_u->fa_r_acc = 0;
}

void halbb_dig_op_unit_para_reset_h(struct bb_info *bb)
{
	struct bb_dig_info *bb_dig = &bb->bb_dig_i;
	struct bb_dig_op_unit *unit_cur = &bb_dig->dig_state_h_i;
	u8 i = 0;

	unit_cur->cur_gaincode = bb_dig->max_gaincode;
	unit_cur->force_gaincode = bb_dig->max_gaincode;
	unit_cur->abs_igi_max = IGI_MAX_PERFORMANCE_MODE;
	unit_cur->abs_igi_min = 0xc;
	unit_cur->pd_low_th_ofst = 16;
#ifdef HALBB_DIG_TDMA_SUPPORT
	unit_cur->state_identifier = DIG_TDMA_HIGH;
#endif
}

#ifdef HALBB_DIG_TDMA_SUPPORT
void halbb_dig_op_unit_para_reset_l(struct bb_info *bb)
{
	struct bb_dig_info *bb_dig = &bb->bb_dig_i;
	struct bb_dig_op_unit *unit_cur = &bb_dig->dig_state_l_i;
	u8 i = 0;

	unit_cur->cur_gaincode = bb_dig->max_gaincode;
	unit_cur->force_gaincode = bb_dig->max_gaincode;
	unit_cur->abs_igi_max = 0x26;
	unit_cur->abs_igi_min = 0xc;
	unit_cur->pd_low_th_ofst = 16;
	unit_cur->state_identifier = DIG_TDMA_LOW;
}
#endif

void halbb_dig_fahm_para_init(struct bb_info *bb)
{
	struct bb_dig_info *dig = &bb->bb_dig_i;
#ifdef HALBB_ENV_MNTR_SUPPORT
	struct fahm_para_info *para = &dig->fahm_para_i;

	para->fahm_rac_lv = RAC_LV_2;
	para->fahm_mntr_time = DIG_CCX_WD_TRIGTIME;

	para->fahm_app = FAHM_DIG;
	para->fahm_numer_opt = FAHM_INCLU_FA;
	para->fahm_denom_opt = FAHM_INCLU_CRC_ERR;
#endif
	dig->fahm_is_triggered = false;
}

void halbb_dig_para_reset(struct bb_info *bb)
{
	struct bb_dig_info *bb_dig = &bb->bb_dig_i;

	bb_dig->pre_dig_mode = bb_dig->dig_mode;
	bb_dig->p_cur_dig_unit = &bb_dig->dig_state_h_i;
	bb_dig->need_update = false;
	halbb_dig_op_unit_para_reset_h(bb); /* dig_op_unit para reset */
#ifdef HALBB_DIG_TDMA_SUPPORT
	halbb_dig_op_unit_para_reset_l(bb); /* dig_op_unit para reset */
	bb_dig->gaincode_update_en = false;
	bb_dig->tdma_passed_time_acc = 0;
	bb_dig->tdma_timestamp_cur = 0;
	bb_dig->tdma_timestamp_pre = bb_dig->tdma_timestamp_cur;
#endif
#ifdef HALBB_ENV_MNTR_SUPPORT
	bb_dig->fahm_timestamp = 0;
	bb_dig->fahm_is_triggered = false;
#endif
}

void halbb_dig_reset(struct bb_info *bb)
{
	struct bb_dig_info *bb_dig = &bb->bb_dig_i;

	BB_DIG_DBG(bb, DIG_DBG_LV0, "[%s]=========>\n", __func__);

	halbb_mem_set(bb, &bb_dig->dig_fa_i, 0, sizeof(struct bb_dig_fa_info));
	halbb_dig_para_reset(bb);

#ifdef HALBB_FW_OFLD_SUPPORT
	if (bb->bb_cmn_hooker->skip_io_init_en && !bb_dig->init_dig_cr_success)
		return;
#endif

#ifdef BB_8852A_2_SUPPORT
	halbb_dig_set_igi_cr_8852a(bb, bb_dig->max_gaincode);
#endif
	halbb_dyn_pd_th_ofdm(bb, IGI_NOLINK, false);
	halbb_dyn_pd_th_cck(bb, IGI_NOLINK, false);
	halbb_sdagc_follow_pagc_config(bb, false);
}

void halbb_dig_init_io_en(struct bb_info *bb)
{
	struct bb_dig_info *dig = &bb->bb_dig_i;
	u8 igi_new;

	BB_DBG(bb, DBG_DIG, "[%s]=========>\n", __func__);

	halbb_dig_gain_para_init(bb);
	halbb_dig_para_update(bb);
	igi_new = halbb_dig_igi_by_ofst(bb, dig->igi_rssi, 0);
	halbb_dig_cfg_bbcr(bb, igi_new);

	dig->init_dig_cr_success = true;
}

void halbb_dig_init(struct bb_info *bb)
{
	struct bb_dig_info *bb_dig = &bb->bb_dig_i;

	if(phl_is_mp_mode(bb->phl_com))
		return;

	BB_DIG_DBG(bb, DIG_DBG_LV0, "[%s]=========>\n", __func__);

	/* DIG sub-DM configurations */
	halbb_dig_mode_update(bb, DIG_ORIGIN, bb->bb_phy_idx);
	bb_dig->igi_rssi = IGI_NOLINK - 10; /*init IGI state*/
	bb_dig->igi_pause_cnt = 0;
	bb_dig->le_igi_ofst = 0;
	bb_dig->dbg_lv = DIG_DBG_LV2;
	bb_dig->dig_state_h_i.state_num_lmt = 3;
	bb_dig->dig_state_h_i.sdagc_follow_pagc_en = false;
#ifdef HALBB_DIG_TDMA_SUPPORT
	bb_dig->dig_state_l_i.state_num_lmt = 1;
	bb_dig->dig_state_l_i.sdagc_follow_pagc_en = false;
	bb_dig->dig_timer_i.cb_time = 50;
#endif
	halbb_dig_fahm_para_init(bb);
#ifdef HALBB_DIG_DAMPING_CHK
	halbb_dig_damping_chk_init(bb);
#endif
	halbb_dig_reset(bb);

#ifdef HALBB_FW_OFLD_SUPPORT
	BB_DBG(bb, DBG_DIG, "[%s][phy=%d]skip_io_init_en = %d\n",
	       __func__, bb->bb_phy_idx, bb->bb_cmn_hooker->skip_io_init_en);

	if (bb->bb_cmn_hooker->skip_io_init_en) {
		bb_dig->init_dig_cr_success = false;
		return;
	}
#endif
	halbb_dig_init_io_en(bb);
}

bool halbb_dig_abort(struct bb_info *bb)
{
	struct bb_dig_info *dig = &bb->bb_dig_i;

	/* support_ability */
	if (!(bb->support_ability & BB_DIG)) {
		BB_DIG_DBG(bb, DIG_DBG_LV0, "%s ======> DISABLED\n", __func__);
		return true;
	}

	if (bb->pause_ability & BB_DIG) {
		dig->igi_pause_cnt++;
		BB_DIG_DBG(bb, DIG_DBG_LV0, "Return: Pause DIG in LV=%d, cnt=%d\n",
		       bb->pause_lv_table.lv_dig,
		       dig->igi_pause_cnt);

		return true;
	}

	if (dig->igi_pause_cnt) {
		dig->igi_pause_cnt = 0;
		BB_DIG_DBG(bb, DIG_DBG_LV0, "Skip 1 time after pause DIG\n");
		return true;
	}

	return false;
}

void halbb_dig_cfg_bbcr(struct bb_info *bb, u8 igi_new)
{
	struct bb_dig_info *dig = &bb->bb_dig_i;
	struct bb_dig_op_unit *bb_dig_u = dig->p_cur_dig_unit;
	struct bb_dig_op_unit *dig_u = &dig->dig_state_h_i;
	struct bb_dig_op_para_unit *para = &dig_u->dig_op_para;

	if (igi_new == dig_u->igi_fa_rssi) {
		BB_DIG_DBG(bb, DIG_DBG_LV0, "[IGI][hold_cnt:%d] %d\n", dig->dig_hold_cnt, igi_new);
		dig->dig_hold_cnt++;
		return;
	} else {
		BB_DIG_DBG(bb, DIG_DBG_LV0, "[IGI] %d -> %d\n", dig_u->igi_fa_rssi, igi_new);
		dig->dig_hold_cnt = 0;
	}

	dig_u->igi_fa_rssi = igi_new;

#ifdef BB_8852A_2_SUPPORT
	/* IGI decision */
	if (halbb_dig_gaincode_update_en_8852a(bb))
		halbb_dig_set_igi_cr_8852a(bb, bb_dig_u->cur_gaincode);
#endif

	/* Dynamic PD lower bound */
	halbb_dyn_pd_th_ofdm(bb, igi_new, para->dyn_pd_th_en);

	/* Dynamic CCK PD TH */
	halbb_dyn_pd_th_cck(bb, igi_new, para->dyn_pd_th_en);

	/* Dynamic sync-dagc follow pagc*/
	if (para->dyn_pd_th_en && igi_new > dig->igi_rssi)
		halbb_sdagc_follow_pagc_config(bb, true);
	else
		halbb_sdagc_follow_pagc_config(bb, false);
}

void halbb_dig_lps(struct bb_info *bb)
{
	struct bb_dig_info *bb_dig = &bb->bb_dig_i;
	struct bb_dig_op_unit *bb_dig_u = bb_dig->p_cur_dig_unit;
	struct bb_link_info *bb_link = &bb->bb_link_i;
	s16 final_rssi = 0;

	BB_DIG_DBG(bb, DIG_DBG_LV0, "%s ======>\n", __func__);

	/* Update igi_rssi */
	bb_dig->igi_rssi = (bb_link->is_linked) ? (bb->bb_ch_i.rssi_min >> 1) : IGI_NOLINK;

	final_rssi = MIN_2(bb_dig->igi_rssi + bb_dig->le_igi_ofst, RSSI_MAX);
	final_rssi = SUBTRACT_TO_0(final_rssi, 10);

	BB_DIG_DBG(bb, DIG_DBG_LV0, "rssi=%03d, le_ofst=(%03d)\n",
		   bb_dig->igi_rssi, bb_dig->le_igi_ofst);

	halbb_dig_cfg_bbcr(bb, (u8)final_rssi);
}

void halbb_dig_simple(struct bb_info *bb, u8 rssi_ofst)
{
	struct bb_dig_info *bb_dig = &bb->bb_dig_i;
	struct bb_dig_op_unit *bb_dig_u = bb_dig->p_cur_dig_unit;
	struct bb_link_info *bb_link = &bb->bb_link_i;
	s16 final_rssi = 0;
	u8 bound = 0;
	u8 phy = bb->bb_phy_idx == HW_PHY_1 ? 1 : 0;
	enum channel_width cbw = bb->hal_com->band[phy].cur_chandef.bw;

	BB_DBG(bb, DBG_DIG, "%s ======>\n", __func__);
	/* Simple DIG for control PD LB only*/
	/* Update igi_rssi */
	bb_dig->igi_rssi = (bb_link->is_linked) ? (bb->bb_ch_i.rssi_min >> 1) : IGI_NOLINK;

	/*final_rssi = MIN_2(bb_dig->igi_rssi + bb_dig->le_igi_ofst, RSSI_MAX);*/
	/* Remove 10 dB offset */
	final_rssi = MIN_2(bb_dig->igi_rssi, RSSI_MAX);
	final_rssi = SUBTRACT_TO_0(final_rssi, rssi_ofst);
	bound = 110 - final_rssi;

	BB_DBG(bb, DBG_DIG, "rssi=%03d, rssi_ofst=(%03d), bound = %03d\n",
		   bb_dig->igi_rssi, rssi_ofst, bound);

	/* Dynamic PD lower bound */
	halbb_set_pd_lower_bound(bb, bound, cbw, phy);
	/*halbb_dyn_pd_th(bb, (u8)final_rssi, true);*/
}

void halbb_dig(struct bb_info *bb)
{
	struct bb_dig_info *dig = &bb->bb_dig_i;
	struct bb_dig_op_unit *dig_u = &dig->dig_state_h_i;
	struct bb_dig_op_para_unit *para = &dig_u->dig_op_para;
	struct bb_link_info *bb_link = &bb->bb_link_i;
	u16 fa_avg;
	u8 igi_new, igi_pre = dig_u->igi_fa_rssi;
	s8 ofst;

	BB_DIG_DBG(bb, DIG_DBG_LV0, "%s ======>\n", __func__);

#ifdef HALBB_FW_OFLD_SUPPORT
	if (bb->bb_cmn_hooker->skip_io_init_en) {
		if (!dig->init_dig_cr_success) {
			halbb_dig_init_io_en(bb);
			BB_DBG(bb, DBG_DIG, "init_dig_cr_success = %d\n",
			       dig->init_dig_cr_success);
			return;
		}
	}
#endif

	dig->need_update |= (bb_link->first_connect | bb_link->first_disconnect);

	if (halbb_dig_abort(bb))
		return;

	/* Update igi_rssi */
	dig->igi_rssi = (bb_link->is_linked) ? (bb->bb_ch_i.rssi_min >> 1) : IGI_NOLINK;
	BB_DIG_DBG(bb, DIG_DBG_LV0, "link=%d, rssi=%d\n", bb_link->is_linked, dig->igi_rssi);

	if (dig->need_update) {
		halbb_dig_para_update(bb);
		#ifdef HALBB_DIG_DAMPING_CHK
		halbb_dig_recorder_reset(bb);
		#endif

		BB_DIG_DBG(bb, DIG_DBG_LV0, "Connect/Disconnect\n");

		igi_new = halbb_dig_igi_by_ofst(bb, dig->igi_rssi, 0);
		halbb_dig_cfg_bbcr(bb, igi_new);
		dig->need_update = false;
		return;
	}

#ifdef HALBB_DIG_TDMA_SUPPORT
	if (dig->dig_mode == DIG_TDMA && bb_link->is_linked) {
		halbb_dig_timercheck_watchdog(bb);
		return;
	}

	dig->p_cur_dig_unit = &dig->dig_state_h_i;
#endif

	if (dig->dig_mode == DIG_SIMPLE) {
		halbb_dig_lps(bb);
		return;
	}

	/* FA info handling */
	if (!halbb_dig_fahm_latch(bb)) {
		BB_DIG_DBG(bb, DIG_DBG_LV0, "FAHM Get Rpt Fail\n");
		goto DIG_END;
	}

	fa_avg = HALBB_DIV(dig_u->fa_r_acc, dig_u->fa_valid_state_cnt);
	dig_u->fa_r_acc = 0;
	dig_u->fa_valid_state_cnt = 0;

#ifdef HALBB_DIG_DAMPING_CHK
	/*Record IGI History*/
	halbb_dig_recorder(bb, igi_pre, fa_avg);
	/*DIG Damping Check*/
	halbb_dig_damping_chk(bb);
#endif
	ofst = halbb_dig_ofst_by_fa(bb, fa_avg);
	igi_new = halbb_dig_igi_by_ofst(bb, igi_pre, ofst);
	halbb_dig_cfg_bbcr(bb, igi_new);

DIG_END:
	if (!halbb_dig_fahm_trig(bb, DIG_CCX_WD_TRIGTIME))
		BB_DIG_DBG(bb, DIG_DBG_LV0, "FAHM Trig Fail\n");
}

#ifdef HALBB_DIG_TDMA_SUPPORT
void halbb_tdma_dig(struct bb_info *bb) {
	struct bb_dig_info *bb_dig = &bb->bb_dig_i;
	struct bb_dig_op_unit *bb_dig_u = bb_dig->p_cur_dig_unit;
	bool fahm_op_status = false;
	bool igi_update_en_h = false, igi_update_en_l = false;

	bb_dig->tdma_timestamp_cur++;
	bb_dig->tdma_passed_time_acc += (u16)bb_dig->dig_timer_i.cb_time;

	BB_DIG_DBG(bb, DIG_DBG_LV2, "[IN]state_cnt=%d, state_lmt=%d\n",
		   bb_dig_u->passed_state_cnt, bb_dig_u->state_num_lmt);

	/* FA info handling */
	fahm_op_status = halbb_dig_fahm_latch(bb);

	/* Two seconds periodic procedure */
	if (bb_dig->tdma_passed_time_acc >= WACHDOG_PERIOD_IN_MS) {
		BB_DIG_DBG(bb, DIG_DBG_LV0, "Two seconds reached.\n");

		BB_DIG_DBG(bb, DIG_DBG_LV0, "[TDMA-H]============>\n");
		bb_dig->p_cur_dig_unit = &bb_dig->dig_state_h_i;
		/* FA info handling */
		halbb_dig_fa_info_update(bb);
		/* Noisy level decision */
		halbb_dig_noisy_lv_decision(bb);
		halbb_dig_igi_ofst_by_env(bb);
		/* IGI and boundary decision */
		bb_dig_u->igi_fa_rssi = halbb_dig_igi_bound_decision(bb);
#ifdef BB_8852A_2_SUPPORT
		/* IGI decision */
		igi_update_en_h = halbb_dig_gaincode_update_en_8852a(bb);
#endif

		BB_DIG_DBG(bb, DIG_DBG_LV0, "[TDMA-L]============>\n");
		bb_dig->p_cur_dig_unit = &bb_dig->dig_state_l_i;
		/* FA info handling */
		halbb_dig_fa_info_update(bb);
		/* Noisy level decision */
		halbb_dig_noisy_lv_decision(bb);
		halbb_dig_igi_ofst_by_env(bb);
		/* IGI and boundary decision */
		bb_dig_u->igi_fa_rssi = halbb_dig_igi_bound_decision(bb);
#ifdef BB_8852A_2_SUPPORT
		/* IGI decision */
		igi_update_en_l = halbb_dig_gaincode_update_en_8852a(bb);
#endif
		bb_dig->gaincode_update_en = igi_update_en_h | igi_update_en_l;
		bb_dig->p_cur_dig_unit = bb_dig_u;
		bb_dig->tdma_passed_time_acc = 0;
	}

	/* TDMA state transition */
	if (++bb_dig_u->passed_state_cnt >= bb_dig_u->state_num_lmt) {
		BB_DIG_DBG(bb, DIG_DBG_LV2, "[OUT]state_cnt=%d, state_lmt=%d\n",
			   bb_dig_u->passed_state_cnt,
			   bb_dig_u->state_num_lmt);
		bb_dig_u->passed_state_cnt = 0;
		switch (bb_dig_u->state_identifier) {
		case DIG_TDMA_LOW:
			bb_dig->p_cur_dig_unit = &bb_dig->dig_state_h_i;
			BB_DIG_DBG(bb, DIG_DBG_LV1, "[TDMA-L]->[TDMA-H].\n");
			break;
		case DIG_TDMA_HIGH:
			bb_dig->p_cur_dig_unit = &bb_dig->dig_state_l_i;
			BB_DIG_DBG(bb, DIG_DBG_LV1, "[TDMA-H]->[TDMA-L].\n");
			break;
		default:
			break;
		}
		bb_dig_u = bb_dig->p_cur_dig_unit;
#ifdef BB_8852A_2_SUPPORT
		/* To set lna, tia, rxbb index */
		if (bb_dig->gaincode_update_en)
			halbb_dig_set_igi_cr_8852a(bb, bb_dig_u->cur_gaincode);
#endif
		/* Dynamic PD lower bound */
		halbb_dyn_pd_th_ofdm(bb, bb_dig_u->igi_fa_rssi,
				     bb_dig_u->dig_op_para.dyn_pd_th_en);

		/* Dynamic CCK PD TH */
		halbb_dyn_pd_th_cck(bb, bb_dig_u->igi_fa_rssi,
				    bb_dig_u->dig_op_para.dyn_pd_th_en);

		/* Dynamic sync-dagc follow pagc*/
		if (bb_dig_u->dig_op_para.dyn_pd_th_en &&
		    (bb_dig_u->igi_fa_rssi > bb_dig->igi_rssi))
			halbb_sdagc_follow_pagc_config(bb, true);
		else
			halbb_sdagc_follow_pagc_config(bb, false);
	}

	fahm_op_status = halbb_dig_fahm_trig(bb, (u16)bb_dig->dig_timer_i.cb_time);
}

void halbb_dig_timercheck_watchdog(struct bb_info *bb)
{
	struct bb_dig_info *bb_dig = &bb->bb_dig_i;

	if (bb_dig->tdma_timestamp_cur == bb_dig->tdma_timestamp_pre) {
		BB_DIG_DBG(bb, DIG_DBG_LV0, "DIG TDMA timer check FAIL. Restart.\n");
		halbb_dig_reset(bb);
		halbb_cfg_timers(bb, BB_SET_TIMER, &bb->bb_dig_i.dig_timer_i);
	}
	bb_dig->tdma_timestamp_pre = bb_dig->tdma_timestamp_cur;
}

void halbb_tdmadig_io_en(struct bb_info *bb)
{
	struct bb_link_info *bb_link = &bb->bb_link_i;

	if (halbb_dig_abort(bb)) {
		return;
	}

	if (!bb_link->is_linked) {
		BB_DIG_DBG(bb, DIG_DBG_LV0, "is_linked=%d, one_entry_only=%d\n",
			   bb_link->is_linked, bb_link->is_one_entry_only);
		return;
	}

	if ((bb->bb_dig_i.dig_mode != DIG_TDMA) &&
	    (bb->bb_dig_i.dig_mode != DIG_TDMA_ADV))
	    return;

	halbb_tdma_dig(bb);
	halbb_cfg_timers(bb, BB_SET_TIMER, &bb->bb_dig_i.dig_timer_i);
}

void halbb_tdmadig_callback(void *context)
{
	struct bb_info *bb = (struct bb_info *)context;
	struct halbb_timer_info *timer = &bb->bb_dig_i.dig_timer_i;

	BB_DIG_DBG(bb, DIG_DBG_LV0, "[%s]===>\n", __func__);

	timer->timer_state = BB_TIMER_IDLE;

	if (bb->phl_com->hci_type == RTW_HCI_PCIE)
		halbb_tdmadig_io_en(bb);
	else
		rtw_hal_cmd_notify(bb->phl_com, MSG_EVT_NOTIFY_BB, (void *)(&timer->event_idx), bb->bb_phy_idx);
}

void halbb_dig_timer_init(struct bb_info *bb)
{
	struct halbb_timer_info *timer = &bb->bb_dig_i.dig_timer_i;

	BB_DBG(bb, DBG_INIT, "[%s]\n", __func__);

	timer->event_idx = BB_EVENT_TIMER_DIG;
	timer->timer_state = BB_TIMER_IDLE;

	halbb_init_timer(bb, &timer->timer_list, halbb_tdmadig_callback, bb, "halbb_dig_timer");
}

#endif /*#ifdef HALBB_DIG_TDMA_SUPPORT*/

void halbb_set_dig_pause_val(struct bb_info *bb, u32 *buf, u8 val_len)
{
	struct bb_dig_info *bb_dig = &bb->bb_dig_i;
	struct bb_dig_op_unit *dig_u = bb_dig->p_cur_dig_unit;
#ifdef BB_8852A_2_SUPPORT
	struct agc_gaincode_set gaincode = bb_dig->max_gaincode;
#endif
	u8 phy = bb->bb_phy_idx == HW_PHY_1 ? 1 : 0;
	enum channel_width cbw = bb->hal_com->band[phy].cur_chandef.bw;
	u8 target_pwr, margin = dig_u->pd_low_th_ofst;

	if (val_len != DIG_PAUSE_INFO_SIZE) {
		BB_DIG_DBG(bb, DIG_DBG_LV0, "[Error][DIG]Need val_len=%d\n",
			   DIG_PAUSE_INFO_SIZE);
		return;
	}
	BB_DIG_DBG(bb, DIG_DBG_LV0, "[%s] Pd=-%d dB\n", __func__, buf[0]);

#ifdef BB_8852A_2_SUPPORT
	/* write igi or keep max gaincode */
	if (bb->ic_type == BB_RTL8852A) {
		if ((buf[1] == PAUSE_OFDM_CCK) && (bb->hal_com->cv < CCV)) {
			BB_DIG_DBG(bb, DIG_DBG_LV0, "[52A] igi_en=1\n");
			halbb_gaincode_by_rssi_8852a(bb, &gaincode, RSSI_MAX - (u8)buf[0]);
		}
		halbb_dig_set_igi_cr_8852a(bb, gaincode);
	}
#endif

	/* write pd lower bound anyway */
	target_pwr = MIN_2((u8)buf[0] + margin, RSSI_MAX);
	halbb_set_pd_lower_bound(bb, target_pwr, cbw, bb->bb_phy_idx);
	if (buf[1] == PAUSE_OFDM)
		target_pwr = 0;
	halbb_set_pd_lower_bound_cck(bb, target_pwr, cbw, bb->bb_phy_idx);
}

void* halbb_get_dig_fa_statistic(struct bb_info *bb)
{
	struct bb_dig_info *bb_dig = &bb->bb_dig_i;

	return &bb_dig->dig_fa_i;
}

void halbb_dig_dbg(struct bb_info *bb, char input[][16], u32 *_used,
		   char *output, u32 *_out_len)
{
	struct bb_dig_info *bb_dig = &bb->bb_dig_i;
	struct bb_dig_op_unit *bb_dig_u = &bb_dig->dig_state_h_i;
#ifdef BB_8852A_2_SUPPORT
	struct agc_gaincode_set set_tmp;
#endif
	u32 var[10] = {0};
	u8 i = 0;

	if (_os_strcmp(input[1], "-h") == 0) {
#ifdef HALBB_DIG_TDMA_SUPPORT
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "DIG-H state\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "{0} {dig op mode = %d(0:DIG,1:TDMA,2:A-TDMA,3:low IO)}\n",
			    bb_dig->dig_mode);
#else
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "{0} {dig op mode = %d(0:DIG,3:low IO)}\n",
			    bb_dig->dig_mode);
#endif
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "{1} {fix_lna = %d} {fix_tia = %d} {fix_rxb = %d}\n",
			    bb_dig_u->force_gaincode.lna_idx,
			    bb_dig_u->force_gaincode.tia_idx,
			    bb_dig_u->force_gaincode.rxb_idx);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "{2: get current IGI} {path(0/1)}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "{3: show rssi threshold of IGI}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "{4: modify rssi threshold} {TH idx(0-4)} {value}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "{5: show fa ratio vs. noisy level.}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "{6: modify fa ratio threshold} {TH idx(0-3)} {value}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "{7: dyn PD low bound en = %d} {0:disable, 1:enable}\n",
			    bb_dig_u->dig_op_para.dyn_pd_th_en);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "{8: dyn PD low bound backoff = %d} {value}\n",
			    bb_dig_u->pd_low_th_ofst);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "{9: reset rssi, fa_ratio TH}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "{10: show AGC table}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "{11: Update gain parameters}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "{12: simple mode IGI offset = (%d)} {0:-, 1:+} {val}\n",
			    bb_dig->le_igi_ofst);
#ifdef HALBB_DIG_TDMA_SUPPORT
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "{13: tdma timer = %d ms} {val(ms)}\n",
			    bb_dig->dig_timer_i.cb_time);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "{14: state limit (L,H) = (%d,%d)} {0:L,1:H} {state num}\n",
			    bb_dig->dig_state_l_i.state_num_lmt,
			    bb_dig->dig_state_h_i.state_num_lmt);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "{15} {0:L,1:H} {fix_lna = %d} {fix_tia = %d} {fix_rxb = %d}\n",
			    bb_dig_u->force_gaincode.lna_idx,
			    bb_dig_u->force_gaincode.tia_idx,
			    bb_dig_u->force_gaincode.rxb_idx);
#endif
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "{20: dbg level = %d} {0/1/2}\n",
			    bb_dig->dbg_lv);
#ifdef HALBB_DIG_DAMPING_CHK
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "damping en {val}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "damping th {s(8,1)}\n");
#endif
		return;
	}

#ifdef HALBB_DIG_DAMPING_CHK
	if (_os_strcmp(input[1], "damping") == 0) {
		HALBB_SCAN(input[3], DCMD_DECIMAL, &var[0]);
		if (_os_strcmp(input[2], "en") == 0) {
			bb_dig->dig_dl_en = (bool)var[0];

			if (!bb_dig->dig_dl_en)
				halbb_dig_recorder_reset(bb);

			BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
				    "dig_dl_en = %d\n", bb_dig->dig_dl_en);
		} else if (_os_strcmp(input[2], "th") == 0) {
			bb_dig->rls_rssi_diff_th = (u8)var[0];
			BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
				    "rls_rssi_diff_th = %d.%d\n",
				    var[0] >> 1, (var[0] & 1) * 5);
		}
		return;
	}
#endif

	HALBB_SCAN(input[1], DCMD_DECIMAL, &var[0]);
	if (var[0] == 0) {
		HALBB_SCAN(input[2], DCMD_DECIMAL, &var[1]);
		halbb_dig_mode_update(bb, (enum dig_op_mode)var[1], bb->bb_phy_idx);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "Set DIG op mode = %d\n", bb_dig->dig_mode);
	} else if (var[0] == 1) {
		HALBB_SCAN(input[2], DCMD_DECIMAL, &var[1]);
		HALBB_SCAN(input[3], DCMD_DECIMAL, &var[2]);
		HALBB_SCAN(input[4], DCMD_DECIMAL, &var[3]);

		bb_dig_u->force_gaincode.lna_idx = (u8)var[1];
		bb_dig_u->force_gaincode.tia_idx = (u8)var[2];
		bb_dig_u->force_gaincode.rxb_idx = (u8)var[3];

		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			 "IGI: fix(lna,tia,rxb)=(%d,%d,%d)\n",
			 bb_dig_u->force_gaincode.lna_idx,
			 bb_dig_u->force_gaincode.tia_idx,
			 bb_dig_u->force_gaincode.rxb_idx);
#ifdef BB_8852A_2_SUPPORT
		halbb_dig_set_igi_cr_8852a(bb, bb_dig_u->force_gaincode);
#endif
	} else if (var[0] == 2) {
		HALBB_SCAN(input[2], DCMD_DECIMAL, &var[1]);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			 "current(lna,tia,rxb)=((%d,%d,%d))\n",
			 halbb_get_lna_idx(bb,(enum rf_path)var[1]),
			 halbb_get_tia_idx(bb,(enum rf_path)var[1]),
			 halbb_get_rxb_idx(bb,(enum rf_path)var[1]));
	} else if (var[0] == 3) {
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "rssi TH: -----%03d-----%03d-----%03d-----%03d-----%03d-----\n",
			    bb_dig_u->dig_op_para.igi_rssi_th[0],
			    bb_dig_u->dig_op_para.igi_rssi_th[1],
			    bb_dig_u->dig_op_para.igi_rssi_th[2],
			    bb_dig_u->dig_op_para.igi_rssi_th[3],
			    bb_dig_u->dig_op_para.igi_rssi_th[4]);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "lna idx: --%1d--|||--%1d--|||--%1d--|||--%1d--|||--%1d--|||--%1d--\n",
			    6,5,4,3,2,1);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "tia idx: --%1d--|||--%1d--|||--%1d--|||--%1d--|||--%1d--|||--%1d--\n",
			    1,0,0,0,0,0);
	} else if (var[0] == 4) {
		HALBB_SCAN(input[2], DCMD_DECIMAL, &var[1]);
		HALBB_SCAN(input[3], DCMD_DECIMAL, &var[2]);
		if (var[1] < 5) {
			bb_dig_u->dig_op_para.igi_rssi_th[var[1]] = (u8)var[2];
			BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "Set rssi_TH[%d]=%d\n", var[1],
			    bb_dig_u->dig_op_para.igi_rssi_th[var[1]]);
		}
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "rssi TH: -----%03d-----%03d-----%03d-----%03d-----%03d-----\n",
			    bb_dig_u->dig_op_para.igi_rssi_th[0],
			    bb_dig_u->dig_op_para.igi_rssi_th[1],
			    bb_dig_u->dig_op_para.igi_rssi_th[2],
			    bb_dig_u->dig_op_para.igi_rssi_th[3],
			    bb_dig_u->dig_op_para.igi_rssi_th[4]);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "lna idx: --%1d--|||--%1d--|||--%1d--|||--%1d--|||--%1d--|||--%1d--\n",
			    6,5,4,3,2,1);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "tia idx: --%1d--|||--%1d--|||--%1d--|||--%1d--|||--%1d--|||--%1d--\n",
			    1,0,0,0,0,0);
	} else if (var[0] == 5) {
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "fa ratio TH: -----%03d-----%03d-----%03d-----%03d-----\n",
			    bb_dig_u->dig_op_para.fa_th[0],
			    bb_dig_u->dig_op_para.fa_th[1],
			    bb_dig_u->dig_op_para.fa_th[2],
			    bb_dig_u->dig_op_para.fa_th[3]);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "noisy level: --%1d--|||--%1d--|||--%1d--|||--%1d--|||-%s-\n",
			    0,1,2,3,"MAX");
	} else if (var[0] == 6) {
		HALBB_SCAN(input[2], DCMD_DECIMAL, &var[1]);
		HALBB_SCAN(input[3], DCMD_DECIMAL, &var[2]);
		if (var[1] < 4) {
			bb_dig_u->dig_op_para.fa_th[var[1]] = (u16)var[2];
			BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "Set FA ratio TH[%d]=%d\n", var[1],
			    bb_dig_u->dig_op_para.fa_th[var[1]]);
		}
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "fa ratio TH: -----%03d-----%03d-----%03d-----%03d-----\n",
			    bb_dig_u->dig_op_para.fa_th[0],
			    bb_dig_u->dig_op_para.fa_th[1],
			    bb_dig_u->dig_op_para.fa_th[2],
			    bb_dig_u->dig_op_para.fa_th[3]);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "noisy level: --%1d--|||--%1d--|||--%1d--|||--%1d--|||-%s-\n",
			    0,1,2,3,"MAX");
	} else if (var[0] == 7) {
		HALBB_SCAN(input[2], DCMD_DECIMAL, &var[1]);
		bb_dig_u->dig_op_para.dyn_pd_th_en = (bool)var[1];
		if (bb_dig_u->dig_op_para.dyn_pd_th_en)
			BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
				    "Enabled dyn PD low bound\n");
		else
			BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
				    "Disabled dyn PD low bound\n");
	} else if (var[0] == 8) {
		HALBB_SCAN(input[2], DCMD_DECIMAL, &var[1]);
		bb_dig_u->pd_low_th_ofst = (u8)var[1];
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "Set dyn PD low bound backoff=%d\n",
			    bb_dig_u->pd_low_th_ofst);
	} else if (var[0] == 9) {
		halbb_dig_reset(bb);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "Reset state machine parameters\n");
#ifdef BB_8852A_2_SUPPORT
	} else if (var[0] == 10) {
		for (i = RSSI_MIN; i <= RSSI_MAX; i++)
			halbb_gaincode_by_rssi_8852a(bb, &set_tmp, i);
#endif
	} else if (var[0] == 11) {
		halbb_dig_gain_para_init(bb);
	} else if (var[0] == 12) {
		HALBB_SCAN(input[2], DCMD_DECIMAL, &var[1]);
		HALBB_SCAN(input[3], DCMD_DECIMAL, &var[2]);
		if (var[1] == 0)
			bb_dig->le_igi_ofst = -(s8)var[2];
		else if (var[1] == 1)
			bb_dig->le_igi_ofst = (s8)var[2];
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "simple mode IGI offset set to (%d)\n",
		    bb_dig->le_igi_ofst);
#ifdef HALBB_DIG_TDMA_SUPPORT
	} else if (var[0] == 13) {
		HALBB_SCAN(input[2], DCMD_DECIMAL, &var[1]);
			bb_dig->dig_timer_i.cb_time = (u16)var[1];
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "tdma timer set to %d ms\n",
		    bb_dig->dig_timer_i.cb_time);
	} else if (var[0] == 14) {
		HALBB_SCAN(input[2], DCMD_DECIMAL, &var[1]);
		HALBB_SCAN(input[3], DCMD_DECIMAL, &var[2]);
		if (var[1] == 0)
			bb_dig_u = &bb_dig->dig_state_l_i;
		else if (var[1] == 1)
			bb_dig_u = &bb_dig->dig_state_h_i;
		bb_dig_u->state_num_lmt = (u8)var[2];
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "set TDMA-%d state limit to %d\n",
			    bb_dig_u->state_identifier,
			    bb_dig_u->state_num_lmt);
	} else if (var[0] == 15) {
		HALBB_SCAN(input[2], DCMD_DECIMAL, &var[1]);
		HALBB_SCAN(input[3], DCMD_DECIMAL, &var[2]);
		HALBB_SCAN(input[4], DCMD_DECIMAL, &var[3]);
		HALBB_SCAN(input[5], DCMD_DECIMAL, &var[4]);
		HALBB_SCAN(input[6], DCMD_DECIMAL, &var[5]);
		if (var[1] == 0)
			bb_dig_u = &bb_dig->dig_state_l_i;
		else if (var[1] == 1)
			bb_dig_u = &bb_dig->dig_state_h_i;

		bb_dig_u->force_gaincode.lna_idx = (u8)var[2];
		bb_dig_u->force_gaincode.tia_idx = (u8)var[3];
		bb_dig_u->force_gaincode.rxb_idx = (u8)var[4];

		bb_dig_u = &bb_dig->dig_state_l_i;
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			 "[TDMA-L]IGI: fix(lna,tia,rxb)=(%d,%d,%d)\n",
			 bb_dig_u->force_gaincode.lna_idx,
			 bb_dig_u->force_gaincode.tia_idx,
			 bb_dig_u->force_gaincode.rxb_idx);
		bb_dig_u = &bb_dig->dig_state_h_i;
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			 "[TDMA-H]IGI: fix(lna,tia,rxb)=(%d,%d,%d)\n",
			 bb_dig_u->force_gaincode.lna_idx,
			 bb_dig_u->force_gaincode.tia_idx,
			 bb_dig_u->force_gaincode.rxb_idx);
#endif
	} else if (var[0] == 20) {
		HALBB_SCAN(input[2], DCMD_DECIMAL, &var[1]);
		if ((enum dig_dbg_level)var[1] <= DIG_DBG_LV2)
			bb_dig->dbg_lv = (enum dig_dbg_level)var[1];
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "dbg level set to lv%d\n",
		    bb_dig->dbg_lv);
	} else {
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "Set Err\n");
	}
}

void halbb_cr_cfg_dig_init(struct bb_info *bb)
{
	struct bb_dig_info *bb_dig = &bb->bb_dig_i;
	struct bb_dig_cr_info *cr = &bb_dig->bb_dig_cr_i;

	switch (bb->cr_type) {
#ifdef HALBB_COMPILE_AP_SERIES
	case BB_AP:
		cr->path0_ib_pbk = PATH0_R_IB_PBK_A;
		cr->path0_ib_pbk_m = PATH0_R_IB_PBK_A_M;
		cr->path0_ib_pkpwr = PATH0_R_IB_PKPW_A;
		cr->path0_ib_pkpwr_m = PATH0_R_IB_PKPW_A_M;
		cr->path1_ib_pbk = PATH1_R_IB_PBK_A;
		cr->path1_ib_pbk_m = PATH1_R_IB_PBK_A_M;
		cr->path1_ib_pkpwr = PATH1_R_IB_PKPW_A;
		cr->path1_ib_pkpwr_m = PATH1_R_IB_PKPW_A_M;
		cr->path0_lna_init_idx = PATH0_R_LNA_INIT_IDX_A;
		cr->path0_lna_init_idx_m = PATH0_R_LNA_INIT_IDX_A_M;
		cr->path1_lna_init_idx = PATH1_R_LNA_INIT_IDX_A;
		cr->path1_lna_init_idx_m = PATH1_R_LNA_INIT_IDX_A_M;
		cr->path0_tia_init_idx = PATH0_R_TIA_INIT_IDX_A;
		cr->path0_tia_init_idx_m = PATH0_R_TIA_INIT_IDX_A_M;
		cr->path1_tia_init_idx = PATH1_R_TIA_INIT_IDX_A;
		cr->path1_tia_init_idx_m = PATH1_R_TIA_INIT_IDX_A_M;
		cr->path0_rxb_init_idx = PATH0_R_RXB_INIT_IDX_A;
		cr->path0_rxb_init_idx_m = PATH0_R_RXB_INIT_IDX_A_M;
		cr->path1_rxb_init_idx = PATH1_R_RXB_INIT_IDX_A;
		cr->path1_rxb_init_idx_m = PATH1_R_RXB_INIT_IDX_A_M;
		cr->seg0r_pd_spatial_reuse_en_a = SEG0R_PD_SPATIAL_REUSE_EN_A;
		cr->seg0r_pd_spatial_reuse_en_a_m = SEG0R_PD_SPATIAL_REUSE_EN_A_M;
		cr->seg0r_pd_lower_bound_a = SEG0R_PD_LOWER_BOUND_A;
		cr->seg0r_pd_lower_bound_a_m = SEG0R_PD_LOWER_BOUND_A_M;
		cr->cca_rssi_lmt_en_a = R1B_RX_CCA_RSSI_LMT_EN_A;
		cr->cca_rssi_lmt_en_a_m = R1B_RX_CCA_RSSI_LMT_EN_A_M;
		cr->rssi_nocca_low_th_a = R1B_RX_RSSI_NOCCA_LOW_TH_A;
		cr->rssi_nocca_low_th_a_m = R1B_RX_RSSI_NOCCA_LOW_TH_A_M;
		cr->path0_p20_follow_by_pagcugc_en_a = PATH0_P20_R_FOLLOW_BY_PAGCUGC_EN_A;
		cr->path0_p20_follow_by_pagcugc_en_a_m = PATH0_P20_R_FOLLOW_BY_PAGCUGC_EN_A_M;
		cr->path0_s20_follow_by_pagcugc_en_a = PATH0_S20_R_FOLLOW_BY_PAGCUGC_EN_A;
		cr->path0_s20_follow_by_pagcugc_en_a_m = PATH0_S20_R_FOLLOW_BY_PAGCUGC_EN_A_M;
		cr->path1_p20_follow_by_pagcugc_en_a = PATH1_P20_R_FOLLOW_BY_PAGCUGC_EN_A;
		cr->path1_p20_follow_by_pagcugc_en_a_m = PATH1_P20_R_FOLLOW_BY_PAGCUGC_EN_A_M;
		cr->path1_s20_follow_by_pagcugc_en_a = PATH1_S20_R_FOLLOW_BY_PAGCUGC_EN_A;
		cr->path1_s20_follow_by_pagcugc_en_a_m = PATH1_S20_R_FOLLOW_BY_PAGCUGC_EN_A_M;
		cr->path0_lna_err_g0_a = PATH0_R_LNA_ERR_G0_A_A;
		cr->path0_lna_err_g0_a_m = PATH0_R_LNA_ERR_G0_A_A_M;
		cr->path0_lna_err_g0_g = PATH0_R_LNA_ERR_G0_G_A;
		cr->path0_lna_err_g0_g_m = PATH0_R_LNA_ERR_G0_G_A_M;
		cr->path0_lna_err_g1_a = PATH0_R_LNA_ERR_G1_A_A;
		cr->path0_lna_err_g1_a_m = PATH0_R_LNA_ERR_G1_A_A_M;
		cr->path0_lna_err_g1_g = PATH0_R_LNA_ERR_G1_G_A;
		cr->path0_lna_err_g1_g_m = PATH0_R_LNA_ERR_G1_G_A_M;
		cr->path0_lna_err_g2_a = PATH0_R_LNA_ERR_G2_A_A;
		cr->path0_lna_err_g2_a_m = PATH0_R_LNA_ERR_G2_A_A_M;
		cr->path0_lna_err_g2_g = PATH0_R_LNA_ERR_G2_G_A;
		cr->path0_lna_err_g2_g_m = PATH0_R_LNA_ERR_G2_G_A_M;
		cr->path0_lna_err_g3_a = PATH0_R_LNA_ERR_G3_A_A;
		cr->path0_lna_err_g3_a_m = PATH0_R_LNA_ERR_G3_A_A_M;
		cr->path0_lna_err_g3_g = PATH0_R_LNA_ERR_G3_G_A;
		cr->path0_lna_err_g3_g_m = PATH0_R_LNA_ERR_G3_G_A_M;
		cr->path0_lna_err_g4_a = PATH0_R_LNA_ERR_G4_A_A;
		cr->path0_lna_err_g4_a_m = PATH0_R_LNA_ERR_G4_A_A_M;
		cr->path0_lna_err_g4_g = PATH0_R_LNA_ERR_G4_G_A;
		cr->path0_lna_err_g4_g_m = PATH0_R_LNA_ERR_G4_G_A_M;
		cr->path0_lna_err_g5_a = PATH0_R_LNA_ERR_G5_A_A;
		cr->path0_lna_err_g5_a_m = PATH0_R_LNA_ERR_G5_A_A_M;
		cr->path0_lna_err_g5_g = PATH0_R_LNA_ERR_G5_G_A;
		cr->path0_lna_err_g5_g_m = PATH0_R_LNA_ERR_G5_G_A_M;
		cr->path0_lna_err_g6_a = PATH0_R_LNA_ERR_G6_A_A;
		cr->path0_lna_err_g6_a_m = PATH0_R_LNA_ERR_G6_A_A_M;
		cr->path0_lna_err_g6_g = PATH0_R_LNA_ERR_G6_G_A;
		cr->path0_lna_err_g6_g_m = PATH0_R_LNA_ERR_G6_G_A_M;
		cr->path0_tia_err_g0_a = PATH0_R_TIA_ERR_G0_A_A;
		cr->path0_tia_err_g0_a_m = PATH0_R_TIA_ERR_G0_A_A_M;
		cr->path0_tia_err_g0_g = PATH0_R_TIA_ERR_G0_G_A;
		cr->path0_tia_err_g0_g_m = PATH0_R_TIA_ERR_G0_G_A_M;
		cr->path0_tia_err_g1_a = PATH0_R_TIA_ERR_G1_A_A;
		cr->path0_tia_err_g1_a_m = PATH0_R_TIA_ERR_G1_A_A_M;
		cr->path0_tia_err_g1_g = PATH0_R_TIA_ERR_G1_G_A;
		cr->path0_tia_err_g1_g_m = PATH0_R_TIA_ERR_G1_G_A_M;
		cr->path1_lna_err_g0_a = PATH1_R_LNA_ERR_G0_A_A;
		cr->path1_lna_err_g0_a_m = PATH1_R_LNA_ERR_G0_A_A_M;
		cr->path1_lna_err_g0_g = PATH1_R_LNA_ERR_G0_G_A;
		cr->path1_lna_err_g0_g_m = PATH1_R_LNA_ERR_G0_G_A_M;
		cr->path1_lna_err_g1_a = PATH1_R_LNA_ERR_G1_A_A;
		cr->path1_lna_err_g1_a_m = PATH1_R_LNA_ERR_G1_A_A_M;
		cr->path1_lna_err_g1_g = PATH1_R_LNA_ERR_G1_G_A;
		cr->path1_lna_err_g1_g_m = PATH1_R_LNA_ERR_G1_G_A_M;
		cr->path1_lna_err_g2_a = PATH1_R_LNA_ERR_G2_A_A;
		cr->path1_lna_err_g2_a_m = PATH1_R_LNA_ERR_G2_A_A_M;
		cr->path1_lna_err_g2_g = PATH1_R_LNA_ERR_G2_G_A;
		cr->path1_lna_err_g2_g_m = PATH1_R_LNA_ERR_G2_G_A_M;
		cr->path1_lna_err_g3_a = PATH1_R_LNA_ERR_G3_A_A;
		cr->path1_lna_err_g3_a_m = PATH1_R_LNA_ERR_G3_A_A_M;
		cr->path1_lna_err_g3_g = PATH1_R_LNA_ERR_G3_G_A;
		cr->path1_lna_err_g3_g_m = PATH1_R_LNA_ERR_G3_G_A_M;
		cr->path1_lna_err_g4_a = PATH1_R_LNA_ERR_G4_A_A;
		cr->path1_lna_err_g4_a_m = PATH1_R_LNA_ERR_G4_A_A_M;
		cr->path1_lna_err_g4_g = PATH1_R_LNA_ERR_G4_G_A;
		cr->path1_lna_err_g4_g_m = PATH1_R_LNA_ERR_G4_G_A_M;
		cr->path1_lna_err_g5_a = PATH1_R_LNA_ERR_G5_A_A;
		cr->path1_lna_err_g5_a_m = PATH1_R_LNA_ERR_G5_A_A_M;
		cr->path1_lna_err_g5_g = PATH1_R_LNA_ERR_G5_G_A;
		cr->path1_lna_err_g5_g_m = PATH1_R_LNA_ERR_G5_G_A_M;
		cr->path1_lna_err_g6_a = PATH1_R_LNA_ERR_G6_A_A;
		cr->path1_lna_err_g6_a_m = PATH1_R_LNA_ERR_G6_A_A_M;
		cr->path1_lna_err_g6_g = PATH1_R_LNA_ERR_G6_G_A;
		cr->path1_lna_err_g6_g_m = PATH1_R_LNA_ERR_G6_G_A_M;
		cr->path1_tia_err_g0_a = PATH1_R_TIA_ERR_G0_A_A;
		cr->path1_tia_err_g0_a_m = PATH1_R_TIA_ERR_G0_A_A_M;
		cr->path1_tia_err_g0_g = PATH1_R_TIA_ERR_G0_G_A;
		cr->path1_tia_err_g0_g_m = PATH1_R_TIA_ERR_G0_G_A_M;
		cr->path1_tia_err_g1_a = PATH1_R_TIA_ERR_G1_A_A;
		cr->path1_tia_err_g1_a_m = PATH1_R_TIA_ERR_G1_A_A_M;
		cr->path1_tia_err_g1_g = PATH1_R_TIA_ERR_G1_G_A;
		cr->path1_tia_err_g1_g_m = PATH1_R_TIA_ERR_G1_G_A_M;
		break;
#endif
#ifdef HALBB_COMPILE_CLIENT_SERIES
	case BB_CLIENT:
		cr->path0_lna_init_idx = PATH0_R_LNA_IDX_INIT_C;
		cr->path0_lna_init_idx_m = PATH0_R_LNA_IDX_INIT_C_M;
		cr->path1_lna_init_idx = PATH1_R_LNA_IDX_INIT_C;
		cr->path1_lna_init_idx_m = PATH1_R_LNA_IDX_INIT_C_M;
		cr->path0_tia_init_idx = PATH0_R_TIA_IDX_INIT_C;
		cr->path0_tia_init_idx_m = PATH0_R_TIA_IDX_INIT_C_M;
		cr->path1_tia_init_idx = PATH1_R_TIA_IDX_INIT_C;
		cr->path1_tia_init_idx_m = PATH1_R_TIA_IDX_INIT_C_M;
		cr->path0_rxb_init_idx = PATH0_R_RXIDX_INIT_C;
		cr->path0_rxb_init_idx_m = PATH0_R_RXIDX_INIT_C_M;
		cr->path1_rxb_init_idx = PATH1_R_RXIDX_INIT_C;
		cr->path1_rxb_init_idx_m = PATH1_R_RXIDX_INIT_C_M;
		cr->seg0r_pd_spatial_reuse_en_a = SEG0R_PD_SPATIAL_REUSE_EN_C;
		cr->seg0r_pd_spatial_reuse_en_a_m = SEG0R_PD_SPATIAL_REUSE_EN_C_M;
		cr->seg0r_pd_lower_bound_a = SEG0R_PD_LOWER_BOUND_C;
		cr->seg0r_pd_lower_bound_a_m = SEG0R_PD_LOWER_BOUND_C_M;
		cr->cca_rssi_lmt_en_a = R1B_RX_CCA_RSSI_LMT_EN_C;
		cr->cca_rssi_lmt_en_a_m = R1B_RX_CCA_RSSI_LMT_EN_C_M;
		cr->rssi_nocca_low_th_a = R1B_RX_RSSI_NOCCA_LOW_TH_C;
		cr->rssi_nocca_low_th_a_m = R1B_RX_RSSI_NOCCA_LOW_TH_C_M;
#if 0
		cr->path0_ib_pbk = PATH0_R_IB_PBK_C;
		cr->path0_ib_pbk_m = PATH0_R_IB_PBK_C_M;
		cr->path0_ib_pkpwr = PATH0_R_IB_PKPW_C;
		cr->path0_ib_pkpwr_m = PATH0_R_IB_PKPW_C_M;
		cr->path1_ib_pbk = PATH1_R_IB_PBK_C;
		cr->path1_ib_pbk_m = PATH1_R_IB_PBK_C_M;
		cr->path1_ib_pkpwr = PATH1_R_IB_PKPW_C;
		cr->path1_ib_pkpwr_m = PATH1_R_IB_PKPW_C_M;
		cr->path0_lna_init_idx = PATH0_R_LNA_INIT_IDX_C;
		cr->path0_lna_init_idx_m = PATH0_R_LNA_INIT_IDX_C_M;
		cr->path1_lna_init_idx = PATH1_R_LNA_INIT_IDX_C;
		cr->path1_lna_init_idx_m = PATH1_R_LNA_INIT_IDX_C_M;
		cr->path0_tia_init_idx = PATH0_R_TIA_INIT_IDX_C;
		cr->path0_tia_init_idx_m = PATH0_R_TIA_INIT_IDX_C_M;
		cr->path1_tia_init_idx = PATH1_R_TIA_INIT_IDX_C;
		cr->path1_tia_init_idx_m = PATH1_R_TIA_INIT_IDX_C_M;
		cr->path0_rxb_init_idx = PATH0_R_RXB_INIT_IDX_C;
		cr->path0_rxb_init_idx_m = PATH0_R_RXB_INIT_IDX_C_M;
		cr->path1_rxb_init_idx = PATH1_R_RXB_INIT_IDX_C;
		cr->path1_rxb_init_idx_m = PATH1_R_RXB_INIT_IDX_C_M;
		cr->seg0r_pd_spatial_reuse_en_a = SEG0R_PD_SPATIAL_REUSE_EN_C;
		cr->seg0r_pd_spatial_reuse_en_a_m = SEG0R_PD_SPATIAL_REUSE_EN_C_M;
		cr->seg0r_pd_lower_bound_a = SEG0R_PD_LOWER_BOUND_C;
		cr->seg0r_pd_lower_bound_a_m = SEG0R_PD_LOWER_BOUND_C_M;
		cr->path0_p20_follow_by_pagcugc_en_a = PATH0_P20_R_FOLLOW_BY_PAGCUGC_EN_C;
		cr->path0_p20_follow_by_pagcugc_en_a_m = PATH0_P20_R_FOLLOW_BY_PAGCUGC_EN_C_M;
		cr->path0_s20_follow_by_pagcugc_en_a = PATH0_S20_R_FOLLOW_BY_PAGCUGC_EN_C;
		cr->path0_s20_follow_by_pagcugc_en_a_m = PATH0_S20_R_FOLLOW_BY_PAGCUGC_EN_C_M;
		cr->path1_p20_follow_by_pagcugc_en_a = PATH1_P20_R_FOLLOW_BY_PAGCUGC_EN_C;
		cr->path1_p20_follow_by_pagcugc_en_a_m = PATH1_P20_R_FOLLOW_BY_PAGCUGC_EN_C_M;
		cr->path1_s20_follow_by_pagcugc_en_a = PATH1_S20_R_FOLLOW_BY_PAGCUGC_EN_C;
		cr->path1_s20_follow_by_pagcugc_en_a_m = PATH1_S20_R_FOLLOW_BY_PAGCUGC_EN_C_M;
		cr->path0_lna_err_g0_a = PATH0_R_LNA_ERR_G0_C_C;
		cr->path0_lna_err_g0_a_m = PATH0_R_LNA_ERR_G0_C_C_M;
		cr->path0_lna_err_g0_g = PATH0_R_LNA_ERR_G0_G_C;
		cr->path0_lna_err_g0_g_m = PATH0_R_LNA_ERR_G0_G_C_M;
		cr->path0_lna_err_g1_a = PATH0_R_LNA_ERR_G1_C_C;
		cr->path0_lna_err_g1_a_m = PATH0_R_LNA_ERR_G1_C_C_M;
		cr->path0_lna_err_g1_g = PATH0_R_LNA_ERR_G1_G_C;
		cr->path0_lna_err_g1_g_m = PATH0_R_LNA_ERR_G1_G_C_M;
		cr->path0_lna_err_g2_a = PATH0_R_LNA_ERR_G2_C_C;
		cr->path0_lna_err_g2_a_m = PATH0_R_LNA_ERR_G2_C_C_M;
		cr->path0_lna_err_g2_g = PATH0_R_LNA_ERR_G2_G_C;
		cr->path0_lna_err_g2_g_m = PATH0_R_LNA_ERR_G2_G_C_M;
		cr->path0_lna_err_g3_a = PATH0_R_LNA_ERR_G3_C_C;
		cr->path0_lna_err_g3_a_m = PATH0_R_LNA_ERR_G3_C_C_M;
		cr->path0_lna_err_g3_g = PATH0_R_LNA_ERR_G3_G_C;
		cr->path0_lna_err_g3_g_m = PATH0_R_LNA_ERR_G3_G_C_M;
		cr->path0_lna_err_g4_a = PATH0_R_LNA_ERR_G4_C_C;
		cr->path0_lna_err_g4_a_m = PATH0_R_LNA_ERR_G4_C_C_M;
		cr->path0_lna_err_g4_g = PATH0_R_LNA_ERR_G4_G_C;
		cr->path0_lna_err_g4_g_m = PATH0_R_LNA_ERR_G4_G_C_M;
		cr->path0_lna_err_g5_a = PATH0_R_LNA_ERR_G5_C_C;
		cr->path0_lna_err_g5_a_m = PATH0_R_LNA_ERR_G5_C_C_M;
		cr->path0_lna_err_g5_g = PATH0_R_LNA_ERR_G5_G_C;
		cr->path0_lna_err_g5_g_m = PATH0_R_LNA_ERR_G5_G_C_M;
		cr->path0_lna_err_g6_a = PATH0_R_LNA_ERR_G6_C_C;
		cr->path0_lna_err_g6_a_m = PATH0_R_LNA_ERR_G6_C_C_M;
		cr->path0_lna_err_g6_g = PATH0_R_LNA_ERR_G6_G_C;
		cr->path0_lna_err_g6_g_m = PATH0_R_LNA_ERR_G6_G_C_M;
		cr->path0_tia_err_g0_a = PATH0_R_TIA_ERR_G0_C_C;
		cr->path0_tia_err_g0_a_m = PATH0_R_TIA_ERR_G0_C_C_M;
		cr->path0_tia_err_g0_g = PATH0_R_TIA_ERR_G0_G_C;
		cr->path0_tia_err_g0_g_m = PATH0_R_TIA_ERR_G0_G_C_M;
		cr->path0_tia_err_g1_a = PATH0_R_TIA_ERR_G1_C_C;
		cr->path0_tia_err_g1_a_m = PATH0_R_TIA_ERR_G1_C_C_M;
		cr->path0_tia_err_g1_g = PATH0_R_TIA_ERR_G1_G_C;
		cr->path0_tia_err_g1_g_m = PATH0_R_TIA_ERR_G1_G_C_M;
		cr->path1_lna_err_g0_a = PATH1_R_LNA_ERR_G0_C_C;
		cr->path1_lna_err_g0_a_m = PATH1_R_LNA_ERR_G0_C_C_M;
		cr->path1_lna_err_g0_g = PATH1_R_LNA_ERR_G0_G_C;
		cr->path1_lna_err_g0_g_m = PATH1_R_LNA_ERR_G0_G_C_M;
		cr->path1_lna_err_g1_a = PATH1_R_LNA_ERR_G1_C_C;
		cr->path1_lna_err_g1_a_m = PATH1_R_LNA_ERR_G1_C_C_M;
		cr->path1_lna_err_g1_g = PATH1_R_LNA_ERR_G1_G_C;
		cr->path1_lna_err_g1_g_m = PATH1_R_LNA_ERR_G1_G_C_M;
		cr->path1_lna_err_g2_a = PATH1_R_LNA_ERR_G2_C_C;
		cr->path1_lna_err_g2_a_m = PATH1_R_LNA_ERR_G2_C_C_M;
		cr->path1_lna_err_g2_g = PATH1_R_LNA_ERR_G2_G_C;
		cr->path1_lna_err_g2_g_m = PATH1_R_LNA_ERR_G2_G_C_M;
		cr->path1_lna_err_g3_a = PATH1_R_LNA_ERR_G3_C_C;
		cr->path1_lna_err_g3_a_m = PATH1_R_LNA_ERR_G3_C_C_M;
		cr->path1_lna_err_g3_g = PATH1_R_LNA_ERR_G3_G_C;
		cr->path1_lna_err_g3_g_m = PATH1_R_LNA_ERR_G3_G_C_M;
		cr->path1_lna_err_g4_a = PATH1_R_LNA_ERR_G4_C_C;
		cr->path1_lna_err_g4_a_m = PATH1_R_LNA_ERR_G4_C_C_M;
		cr->path1_lna_err_g4_g = PATH1_R_LNA_ERR_G4_G_C;
		cr->path1_lna_err_g4_g_m = PATH1_R_LNA_ERR_G4_G_C_M;
		cr->path1_lna_err_g5_a = PATH1_R_LNA_ERR_G5_C_C;
		cr->path1_lna_err_g5_a_m = PATH1_R_LNA_ERR_G5_C_C_M;
		cr->path1_lna_err_g5_g = PATH1_R_LNA_ERR_G5_G_C;
		cr->path1_lna_err_g5_g_m = PATH1_R_LNA_ERR_G5_G_C_M;
		cr->path1_lna_err_g6_a = PATH1_R_LNA_ERR_G6_C_C;
		cr->path1_lna_err_g6_a_m = PATH1_R_LNA_ERR_G6_C_C_M;
		cr->path1_lna_err_g6_g = PATH1_R_LNA_ERR_G6_G_C;
		cr->path1_lna_err_g6_g_m = PATH1_R_LNA_ERR_G6_G_C_M;
		cr->path1_tia_err_g0_a = PATH1_R_TIA_ERR_G0_C_C;
		cr->path1_tia_err_g0_a_m = PATH1_R_TIA_ERR_G0_C_C_M;
		cr->path1_tia_err_g0_g = PATH1_R_TIA_ERR_G0_G_C;
		cr->path1_tia_err_g0_g_m = PATH1_R_TIA_ERR_G0_G_C_M;
		cr->path1_tia_err_g1_a = PATH1_R_TIA_ERR_G1_C_C;
		cr->path1_tia_err_g1_a_m = PATH1_R_TIA_ERR_G1_C_C_M;
		cr->path1_tia_err_g1_g = PATH1_R_TIA_ERR_G1_G_C;
		cr->path1_tia_err_g1_g_m = PATH1_R_TIA_ERR_G1_G_C_M;
#endif
		break;
#endif
#ifdef HALBB_COMPILE_AP2_SERIES
	case BB_AP2:
		cr->path0_lna_init_idx = PATH0_R_LNA_IDX_INIT_A2;
		cr->path0_lna_init_idx_m = PATH0_R_LNA_IDX_INIT_A2_M;
		cr->path1_lna_init_idx = PATH1_R_LNA_IDX_INIT_A2;
		cr->path1_lna_init_idx_m = PATH1_R_LNA_IDX_INIT_A2_M;
		cr->path0_tia_init_idx = PATH0_R_TIA_IDX_INIT_A2;
		cr->path0_tia_init_idx_m = PATH0_R_TIA_IDX_INIT_A2_M;
		cr->path1_tia_init_idx = PATH1_R_TIA_IDX_INIT_A2;
		cr->path1_tia_init_idx_m = PATH1_R_TIA_IDX_INIT_A2_M;
		cr->path0_rxb_init_idx = PATH0_R_RXIDX_INIT_A2;
		cr->path0_rxb_init_idx_m = PATH0_R_RXIDX_INIT_A2_M;
		cr->path1_rxb_init_idx = PATH1_R_RXIDX_INIT_A2;
		cr->path1_rxb_init_idx_m = PATH1_R_RXIDX_INIT_A2_M;
		cr->seg0r_pd_spatial_reuse_en_a = SEG0R_PD_SPATIAL_REUSE_EN_A2;
		cr->seg0r_pd_spatial_reuse_en_a_m = SEG0R_PD_SPATIAL_REUSE_EN_A2_M;
		cr->seg0r_pd_lower_bound_a = SEG0R_PD_LOWER_BOUND_A2;
		cr->seg0r_pd_lower_bound_a_m = SEG0R_PD_LOWER_BOUND_A2_M;
		cr->path0_p20_follow_by_pagcugc_en_a = PATH0_P20_R_FOLLOW_BY_PAGCUGC_EN_A2;
		cr->path0_p20_follow_by_pagcugc_en_a_m = PATH0_P20_R_FOLLOW_BY_PAGCUGC_EN_A2_M;
		cr->path0_s20_follow_by_pagcugc_en_a = PATH0_S20_R_FOLLOW_BY_PAGCUGC_EN_A2;
		cr->path0_s20_follow_by_pagcugc_en_a_m = PATH0_S20_R_FOLLOW_BY_PAGCUGC_EN_A2_M;
		cr->path1_p20_follow_by_pagcugc_en_a = PATH1_P20_R_FOLLOW_BY_PAGCUGC_EN_A2;
		cr->path1_p20_follow_by_pagcugc_en_a_m = PATH1_P20_R_FOLLOW_BY_PAGCUGC_EN_A2_M;
		cr->path1_s20_follow_by_pagcugc_en_a = PATH1_S20_R_FOLLOW_BY_PAGCUGC_EN_A2;
		cr->path1_s20_follow_by_pagcugc_en_a_m = PATH1_S20_R_FOLLOW_BY_PAGCUGC_EN_A2_M;
		cr->cca_rssi_lmt_en_a = CCA_RSSI_LMT_EN_A2;
		cr->cca_rssi_lmt_en_a_m = CCA_RSSI_LMT_EN_A2_M;
		cr->rssi_nocca_low_th_a = RSSI_NOCCA_LOW_TH_A2;
		cr->rssi_nocca_low_th_a_m = RSSI_NOCCA_LOW_TH_A2_M;
		cr->path0_dig_mode_en_a = PATH0_R_DIG_MODE_EN_A2;
		cr->path0_dig_mode_en_a_m = PATH0_R_DIG_MODE_EN_A2_M;
		cr->path0_igi_for_dig_a = PATH0_R_IGI_FOR_DIG_A2;
		cr->path0_igi_for_dig_a_m = PATH0_R_IGI_FOR_DIG_A2_M;
		cr->path0_backoff_wb_gain_a = PATH0_R_BACKOFF_WB_GAIN_A2;
		cr->path0_backoff_wb_gain_a_m = PATH0_R_BACKOFF_WB_GAIN_A2_M;
		cr->path1_dig_mode_en_a = PATH1_R_DIG_MODE_EN_A2;
		cr->path1_dig_mode_en_a_m = PATH1_R_DIG_MODE_EN_A2_M;
		cr->path1_igi_for_dig_a = PATH1_R_IGI_FOR_DIG_A2;
		cr->path1_igi_for_dig_a_m = PATH1_R_IGI_FOR_DIG_A2_M;
		cr->path1_backoff_wb_gain_a = PATH1_R_BACKOFF_WB_GAIN_A2;
		cr->path1_backoff_wb_gain_a_m = PATH1_R_BACKOFF_WB_GAIN_A2_M;
		break;
#endif
#ifdef HALBB_COMPILE_BE0_SERIES
	case BB_BE0:
		cr->path0_lna_init_idx = PATH0_R_LNA_IDX_INIT_BE0;
		cr->path0_lna_init_idx_m = PATH0_R_LNA_IDX_INIT_BE0_M;
		cr->path1_lna_init_idx = PATH1_R_LNA_IDX_INIT_BE0;
		cr->path1_lna_init_idx_m = PATH1_R_LNA_IDX_INIT_BE0_M;
		cr->path0_tia_init_idx = PATH0_R_TIA_IDX_INIT_BE0;
		cr->path0_tia_init_idx_m = PATH0_R_TIA_IDX_INIT_BE0_M;
		cr->path1_tia_init_idx = PATH1_R_TIA_IDX_INIT_BE0;
		cr->path1_tia_init_idx_m = PATH1_R_TIA_IDX_INIT_BE0_M;
		cr->path0_rxb_init_idx = PATH0_R_RXIDX_INIT_BE0;
		cr->path0_rxb_init_idx_m = PATH0_R_RXIDX_INIT_BE0_M;
		cr->path1_rxb_init_idx = PATH1_R_RXIDX_INIT_BE0;
		cr->path1_rxb_init_idx_m = PATH1_R_RXIDX_INIT_BE0_M;
		cr->seg0r_pd_spatial_reuse_en_a = SEG0R_PD_SPATIAL_REUSE_EN_BE0;
		cr->seg0r_pd_spatial_reuse_en_a_m = SEG0R_PD_SPATIAL_REUSE_EN_BE0_M;
		cr->seg0r_pd_lower_bound_a = SEG0R_PD_LOWER_BOUND_BE0;
		cr->seg0r_pd_lower_bound_a_m = SEG0R_PD_LOWER_BOUND_BE0_M;
		cr->path0_p20_follow_by_pagcugc_en_a = PATH0_P20_R_FOLLOW_BY_PAGCUGC_EN_BE0;
		cr->path0_p20_follow_by_pagcugc_en_a_m = PATH0_P20_R_FOLLOW_BY_PAGCUGC_EN_BE0_M;
		cr->path0_s20_follow_by_pagcugc_en_a = PATH0_S20_R_FOLLOW_BY_PAGCUGC_EN_BE0;
		cr->path0_s20_follow_by_pagcugc_en_a_m = PATH0_S20_R_FOLLOW_BY_PAGCUGC_EN_BE0_M;
		cr->path1_p20_follow_by_pagcugc_en_a = PATH1_P20_R_FOLLOW_BY_PAGCUGC_EN_BE0;
		cr->path1_p20_follow_by_pagcugc_en_a_m = PATH1_P20_R_FOLLOW_BY_PAGCUGC_EN_BE0_M;
		cr->path1_s20_follow_by_pagcugc_en_a = PATH1_S20_R_FOLLOW_BY_PAGCUGC_EN_BE0;
		cr->path1_s20_follow_by_pagcugc_en_a_m = PATH1_S20_R_FOLLOW_BY_PAGCUGC_EN_BE0_M;
		cr->cca_rssi_lmt_en_a = CCA_RSSI_LMT_EN_BE0;
		cr->cca_rssi_lmt_en_a_m = CCA_RSSI_LMT_EN_BE0_M;
		cr->rssi_nocca_low_th_a = RSSI_NOCCA_LOW_TH_BE0;
		cr->rssi_nocca_low_th_a_m = RSSI_NOCCA_LOW_TH_BE0_M;
		cr->path0_dig_mode_en_a = PATH0_R_DIG_MODE_EN_BE0;
		cr->path0_dig_mode_en_a_m = PATH0_R_DIG_MODE_EN_BE0_M;
		cr->path0_igi_for_dig_a = PATH0_R_IGI_FOR_DIG_BE0;
		cr->path0_igi_for_dig_a_m = PATH0_R_IGI_FOR_DIG_BE0_M;
		cr->path0_backoff_wb_gain_a = PATH0_R_BACKOFF_WB_GAIN_BE0;
		cr->path0_backoff_wb_gain_a_m = PATH0_R_BACKOFF_WB_GAIN_BE0_M;
		cr->path1_dig_mode_en_a = PATH1_R_DIG_MODE_EN_BE0;
		cr->path1_dig_mode_en_a_m = PATH1_R_DIG_MODE_EN_BE0_M;
		cr->path1_igi_for_dig_a = PATH1_R_IGI_FOR_DIG_BE0;
		cr->path1_igi_for_dig_a_m = PATH1_R_IGI_FOR_DIG_BE0_M;
		cr->path1_backoff_wb_gain_a = PATH1_R_BACKOFF_WB_GAIN_BE0;
		cr->path1_backoff_wb_gain_a_m = PATH1_R_BACKOFF_WB_GAIN_BE0_M;
		break;

#endif

	default:
		BB_WARNING("[%s] BBCR Hook FAIL!\n", __func__);
		if (bb->bb_dbg_i.cr_fake_init_hook_en) {
			BB_TRACE("[%s] BBCR fake init\n", __func__);
			halbb_cr_hook_fake_init(bb, (u32 *)cr, (sizeof(struct bb_dig_cr_info) >> 2));
		}
		break;
	}

	if (bb->bb_dbg_i.cr_init_hook_recorder_en) {
		BB_TRACE("[%s] BBCR Hook dump\n", __func__);
		halbb_cr_hook_init_dump(bb, (u32 *)cr, (sizeof(struct bb_dig_cr_info) >> 2));
	}
}
#endif
#ifdef HALBB_DIG_MCC_SUPPORT
void Halbb_init_mccdm(struct bb_info *bb)
{
	struct halbb_mcc_dm *mcc_dm = &bb->mcc_dm;
	u8 i = 0;

	BB_DBG(bb, DBG_DIG, "[%s]=========>\n", __func__);

	mcc_dm->softap_macid = INVALID_INIT_VAL;

	for (i = 0; i < MCC_BAND_NUM; i++) {
		mcc_dm->mcc_reg_id[i] = INVALID_INIT_VAL;
		mcc_dm->mcc_dm_reg[i] = 0;
		mcc_dm->mcc_dm_val[i][0] = 0;
		mcc_dm->mcc_dm_val[i][1] = 0;
	}
}

u32 halbb_c2h_mccdm_check(struct bb_info *bb, u16 len, u8 *c2h)
{
	bool fw_mccdm_en = false;

	if (!c2h) {
		BB_WARNING("Error fw mcc dig c2h failed!!\n");
		return _FAIL;
	}

	fw_mccdm_en = (bool)c2h[0];
	BB_DBG(bb, DBG_DIG, "FW MCC DIG : %s\n", fw_mccdm_en ? "true" : "false");

	return _SUCCESS;
}

void halbb_mccdm_h2ccmd_rst(struct bb_info *bb)
{
	struct mcc_h2c *mcc_cfg;
	bool ret_val = false;
	u8 cmdlen = sizeof(struct mcc_h2c);
	u32 *bb_h2c;

	mcc_cfg = hal_mem_alloc(bb->hal_com, cmdlen);

	if (!mcc_cfg) {
		BB_WARNING(" Error mcc_cfg allocat failed!!\n");
		return;
	}
	halbb_mem_set(bb, mcc_cfg, 0, cmdlen);
	bb_h2c = (u32*) mcc_cfg;
	//u8 h2c_mcc[H2C_MAX_LENGTH];

	/* RST MCC */
	mcc_cfg->mcc_dm_en = 0;
	mcc_cfg->reg_cnt = 0;
	mcc_cfg->mcc_set = 0;
	ret_val = halbb_fill_h2c_cmd(bb, cmdlen, DM_H2C_FW_MCC,
					   HALBB_H2C_DM, bb_h2c);
	BB_DBG(bb, DBG_DIG, "MCC H2C RST\n");

	if (mcc_cfg)
		hal_mem_free(bb->hal_com, mcc_cfg, cmdlen);
}

void Halbb_mccdm_h2c_handler(struct bb_info *bb)
{
	struct halbb_mcc_dm *mcc_dm = &bb->mcc_dm;
	struct mcc_h2c_reg_content *reg_cont;
	struct mcc_h2c *mcc_cfg;
	bool ret_val = false;
	u8 cmdlen = sizeof(struct mcc_h2c);
	u8 i;
	u8 regid;
	u8 ch_idx;
	u8 reg_cnt;
	u32 *bb_h2c;

	mcc_cfg = hal_mem_alloc(bb->hal_com, cmdlen);

	if (!mcc_cfg) {
		BB_WARNING(" Error mcc_cfg allocat failed!!\n");
		return;
	}
	bb_h2c = (u32*) mcc_cfg;

	if (mcc_dm->mcc_rf_ch[0].center_ch == INVALID_INIT_VAL &&
	    mcc_dm->mcc_rf_ch[1].center_ch == INVALID_INIT_VAL) {
		BB_DBG(bb, DBG_DIG, "MCC channel Error\n");
		mcc_cfg->mcc_dm_en = 0;
		mcc_cfg->reg_cnt = 0;
		mcc_cfg->mcc_set = 0;
		ret_val = halbb_fill_h2c_cmd(bb, cmdlen, DM_H2C_FW_MCC,
					     HALBB_H2C_DM, bb_h2c);
		if (mcc_cfg)
			hal_mem_free(bb->hal_com, mcc_cfg, cmdlen);
		return;
	}

	/* Set Channel number, reg, and val*/
	for (ch_idx = 0; ch_idx < MCC_BAND_NUM; ch_idx++) {
		halbb_mem_set(bb, mcc_cfg, 0, cmdlen);
		reg_cnt = 0;
		mcc_cfg->mcc_dm_en = 1;
		mcc_cfg->mcc_ch_idx = ch_idx;
		mcc_cfg->mcc_set = 1;
		mcc_cfg->phy0_en = 1;
		mcc_cfg->phy1_en = 0;
		mcc_cfg->ch_lsb = (u8)mcc_dm->mcc_rf_ch[ch_idx].center_ch;
		mcc_cfg->ch_msb = (u8)mcc_dm->mcc_rf_ch[ch_idx].band;
		for (i = 0; i < NUM_MAX_IGI_CNT; i++) {
			regid = mcc_dm->mcc_reg_id[i];
			if (regid == INVALID_INIT_VAL)
				break;
			reg_cont = &mcc_cfg->mcc_reg_content[i];
			reg_cont->addr_lsb = (u8)mcc_dm->mcc_dm_reg[i];
			reg_cont->addr_msb = (u8)(mcc_dm->mcc_dm_reg[i] >> 8);
			reg_cont->bmask_lsb = (u8)(mcc_dm->mcc_dm_mask[i]);
			reg_cont->bmask_msb = (u8)(mcc_dm->mcc_dm_mask[i] >> 8);
			reg_cont->val_lsb = (u8)(mcc_dm->mcc_dm_val[i][ch_idx]);
			reg_cont->val_msb = (u8)(mcc_dm->mcc_dm_val[i][ch_idx] >> 8);
			reg_cnt++;
		}
		mcc_cfg->reg_cnt = reg_cnt;
		BB_DBG(bb, DBG_DIG, "MCC H2C SetCH: 0x%x 0x%x 0x%x\n",
		       bb_h2c[0], bb_h2c[1], bb_h2c[2]);
		ret_val = halbb_fill_h2c_cmd(bb, cmdlen, DM_H2C_FW_MCC,
					     HALBB_H2C_DM, bb_h2c);
	}

	if (mcc_cfg)
		hal_mem_free(bb->hal_com, mcc_cfg, cmdlen);
}

void halbb_mccdm_ctrl(struct bb_info *bb)
{
	struct halbb_mcc_dm *mcc_dm = &bb->mcc_dm;
	u32 val[2] = {0};

	BB_DBG(bb, DBG_DIG, "MCC status: %x\n", mcc_dm->mcc_status_en);
	/*MCC stage no change*/
	if (mcc_dm->mcc_status_en == mcc_dm->mcc_pre_status_en)
		return;

	/*Not in MCC stage*/
	if (mcc_dm->mcc_status_en != 0) {
		/* Disable normal DIG */
		halbb_pause_func(bb, F_DIG, HALBB_PAUSE_NO_SET, HALBB_PAUSE_LV_2,
				 2, val, bb->bb_phy_idx);
	}
	if (mcc_dm->mcc_status_en == 0 && mcc_dm->mcc_pre_status_en != 0) {
		Halbb_init_mccdm(bb);
		halbb_mccdm_h2ccmd_rst(bb);
		/* Enable normal DIG */
		halbb_pause_func(bb, F_DIG, HALBB_RESUME, HALBB_PAUSE_LV_2, 2,
				 val, bb->bb_phy_idx);
	}

	mcc_dm->mcc_pre_status_en = mcc_dm->mcc_status_en;
}

void halbb_fill_mcccmd(struct bb_info *bb, u8 regid, u16 reg_add, u16 mask,
		       u8 band, u16 val)
{
	struct halbb_mcc_dm *mcc_dm = &bb->mcc_dm;

	mcc_dm->mcc_reg_id[regid] = regid;
	mcc_dm->mcc_dm_reg[regid] = reg_add;
	mcc_dm->mcc_dm_mask[regid] = mask;
	mcc_dm->mcc_dm_val[regid][band] = val;
}

void halbb_mccdm_igi_rst(struct bb_info *bb, u8 clr_port)
{
	struct halbb_mcc_dm *mcc_dm = &bb->mcc_dm;

	mcc_dm->mcc_dm_val[0][clr_port] = PD_IDX_MIN; //-102dBm
	//mcc_dm->mcc_dm_val[1][clr_port] = 0xff;
}

#if 0
void halbb_mcc_igi_chk(struct bb_info *bb)
{
	struct halbb_mcc_dm *mcc_dm = &bb->mcc_dm;

	if (mcc_dm->mcc_dm_val[0][0] == 0xffff &&
	    mcc_dm->mcc_dm_val[0][1] == 0xffff) {
		mcc_dm->mcc_dm_reg[0] = 0xffff;
		mcc_dm->mcc_reg_id[0] = 0xff;
	}

	if (mcc_dm->mcc_dm_val[1][0] == 0xffff &&
	    mcc_dm->mcc_dm_val[1][1] == 0xffff) {
		mcc_dm->mcc_dm_reg[1] = 0xffff;
		mcc_dm->mcc_reg_id[1] = 0xff;
	}
}
#endif

u8 halbb_mccdm_pd_lower_bound_cal(struct bb_info *bb, u8 bound,
				      enum channel_width bw)
{
	/*
	Range of bound value:
	BW20: 95~33
	BW40: 92~30
	BW80: 89~27
	*/
	u8 bw_attenuation = 0;
	u8 subband_filter_atteniation = 7;
	u8 bound_idx = 0;

	if (bound == 0) {
		BB_DBG(bb, DBG_DIG,
		       "[PD Bound] Set Boundary to default!\n");
		return 0;
	}

	if (bw == CHANNEL_WIDTH_20) {
		bw_attenuation = 0;
	} else if (bw == CHANNEL_WIDTH_40) {
		bw_attenuation = 3;
	} else if (bw == CHANNEL_WIDTH_80) {
		bw_attenuation = 6;
	} else {
		BB_DBG(bb, DBG_DIG,
		       "[PD Bound] Only support BW20/40/80 !\n");
		return 0;
	}

	bound += (bw_attenuation + subband_filter_atteniation);
	// If Boundary dbm is odd, set it to even number
	bound = bound % 2 ? bound + 1 : bound;

	if (bound < 40) {
		BB_DBG(bb, DBG_DIG,
		       "[PD Bound] Threshold too high, set to highest level!\n");
		bound = 40;
	}

	if (bound > 102) {
		BB_DBG(bb, DBG_DIG,
		       "[PD Bound] Threshold too low, disable PD lower bound function!\n");
		bound = 102;
	}

	bound_idx =  (102 - bound) >> 1;

	return bound_idx;
}

void halbb_mccdm_pd_cal(struct bb_info *bb)
{
	struct halbb_mcc_dm *mcc_dm = &bb->mcc_dm;
	struct bb_dig_info *bb_dig = &bb->bb_dig_i;
	struct bb_dig_op_unit *bb_dig_u = bb_dig->p_cur_dig_unit;
	struct bb_dig_cr_info *cr = &bb_dig->bb_dig_cr_i;
	u8 shift = 10;
	u8 igi_val;
	u8 pd_val;
	u8 i;
	u16 mask0;
	u16 reg0;
	enum channel_width cbw = CHANNEL_WIDTH_20;

	for (i = 0; i < MCC_BAND_NUM; i++) {
		igi_val = mcc_dm->rssi_min[i] >> 1;
		igi_val = SUBTRACT_TO_0(igi_val, shift);
		igi_val = MIN_2(igi_val, IGI_MAX_PERFORMANCE_MODE);
		igi_val = MAX_2(igi_val, 0xc);
		igi_val -= MIN_2(igi_val, bb_dig_u->pd_low_th_ofst);

		cbw = mcc_dm->mcc_rf_ch[i].bw;

		pd_val = halbb_mccdm_pd_lower_bound_cal(bb, RSSI_MAX - igi_val,
							cbw);

		reg0 = (u16)cr->seg0r_pd_lower_bound_a;
		mask0 = (u16)cr->seg0r_pd_lower_bound_a_m;

		halbb_fill_mcccmd(bb, 0, reg0, mask0, i, (u16)pd_val);

		if (mcc_dm->sta_cnt[i] == 0)
			halbb_mccdm_igi_rst(bb, i);
	}

	BB_DBG(bb, DBG_DIG, "STA cnt %d %d, RSSI_min: %d %d, BW: %d %d, MCC_pd_idx: %d %d\n",
	       mcc_dm->sta_cnt[0], mcc_dm->sta_cnt[1],
	       mcc_dm->rssi_min[0] >> 1, mcc_dm->rssi_min[1] >> 1,
	       mcc_dm->mcc_rf_ch[0].bw, mcc_dm->mcc_rf_ch[1].bw,
	       mcc_dm->mcc_dm_val[0][0], mcc_dm->mcc_dm_val[0][1]);
}

void halbb_mccdm_switch(struct bb_info *bb)
{
	struct halbb_mcc_dm *mcc_dm = &bb->mcc_dm;

	if (!(bb->ic_type & HALBB_DIG_MCC_SUPPORT_IC)) {
		BB_DBG(bb, DBG_DIG, "IC type is not supported\n");
		return;
	}

	halbb_mccdm_ctrl(bb);

	if (mcc_dm->mcc_status_en == 0)
		return;
	BB_DBG(bb, DBG_DIG, "<====== %s ======>\n", __func__);

	/* Set IGI*/
	halbb_mccdm_pd_cal(bb);

	/* Set H2C Cmd*/
	Halbb_mccdm_h2c_handler(bb);
}
#endif

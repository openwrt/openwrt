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

#ifdef HALBB_ENV_MNTR_SUPPORT

u16 halbb_ccx_get_ratio(struct bb_info *bb, u16 rpt, u16 score)
{
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;
	u32 numer = 0;
	u16 ratio = 0;
	u16 ret = 0;

	if (rpt == env->ccx_period) {
		ret = score;
	} else {
		numer = rpt * score + (env->ccx_period >> 1);
		ratio = (u16)HALBB_DIV(numer, env->ccx_period);
		ret = (ratio == score) ? (score - 1) : ratio;
	}

	return ret;
}

void halbb_ccx_ms_2_period_unit(struct bb_info *bb, u16 time_ms, u32 *period,
				u32 *unit_idx)
{
	if (time_ms >= 2097)
		time_ms = 2097;

	if (time_ms < 263)
		*unit_idx = CCX_04_US;
	else if (time_ms < 525)
		*unit_idx = CCX_08_US;
	else if (time_ms < 1049)
		*unit_idx = CCX_16_US;
	else
		*unit_idx = CCX_32_US;

	*period = (u32)((time_ms * MS_TO_4US_RATIO) >> *unit_idx);

	BB_DBG(bb, DBG_ENV_MNTR, "[Trigger Time] period:%d, unit_idx:%d\n",
		 period[0], unit_idx[0]);
}

u32 halbb_ccx_idx_cnt_2_us(struct bb_info *bb, u16 idx_cnt)
{
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;
	u32 time_us = 0;

	time_us = (u32)(idx_cnt << (2 + env->ccx_unit_idx));

	return time_us;
}

u16 halbb_ccx_us_2_idx_cnt(struct bb_info *bb, u32 time_us)
{
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;
	u16 idx_cnt = 0;

	idx_cnt = (u16)(time_us >> (2 + env->ccx_unit_idx));

	return idx_cnt;
}

void halbb_ccx_top_setting_init(struct bb_info *bb)
{
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;
	struct bb_env_mntr_cr_info *cr = &env->bb_env_mntr_cr_i;

	env->ccx_manual_ctrl = false;
	env->ccx_ongoing = false;
	env->ccx_rac_lv = RAC_RELEASE;
	env->ccx_rpt_stamp = 0;
	env->ccx_period = 0;
	env->ccx_unit_idx = CCX_32_US;
	env->ccx_trigger_time = 0;
	env->ccx_edcca_opt = 0;
	env->ccx_ext_loss_update = false;
	env->ccx_ext_loss_pre = 0;
	env->ccx_ext_loss = 0;

	halbb_set_reg_curr_phy(bb, cr->ccx_en, cr->ccx_en_m, 1);
	halbb_set_reg_curr_phy(bb, cr->ccx_trig_opt, cr->ccx_trig_opt_m, 0);
	halbb_set_reg_curr_phy(bb, cr->ccx_edcca_opt, cr->ccx_edcca_opt_m, 0);
	halbb_set_reg_curr_phy(bb, cr->ccx_source_sel, cr->ccx_source_sel_m,
			       4); /*dccl output, full bandwidth*/
}

void halbb_ccx_racing_release(struct bb_info *bb, u8 func_sel)
{
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;

	BB_DBG(bb, DBG_ENV_MNTR, "[%s]===>\n", __func__);
	BB_DBG(bb, DBG_ENV_MNTR, "lv:(%d)->(0)\n", env->ccx_rac_lv);

	env->ccx_ongoing = false;
	env->ccx_rac_lv = RAC_RELEASE;
	env->clm_app = CLM_INIT;
	if (func_sel & NHM_SEL)
		env->nhm_app = NHM_INIT;

	env->ifs_clm_app = IFS_CLM_INIT;
	env->edcca_clm_app = EDCCA_CLM_INIT;
}

u8 halbb_ccx_racing_ctrl(struct bb_info *bb, enum halbb_racing_lv rac_lv)
{
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;
	u8 set_result = HALBB_SET_SUCCESS;

	if (rac_lv >= RAC_MAX_NUM) {
		BB_DBG(bb, DBG_ENV_MNTR, "[WARNING] Wrong LV=%d\n", rac_lv);
		return HALBB_SET_FAIL;
	}

	BB_DBG(bb, DBG_ENV_MNTR, "ccx_ongoing=%d, lv:(%d)->(%d)\n",
	       env->ccx_ongoing, env->ccx_rac_lv, rac_lv);

	if (env->ccx_ongoing) {
		if (rac_lv <= env->ccx_rac_lv)
			set_result = HALBB_SET_FAIL;
		else
			env->ccx_ongoing = false;
	}

	if (set_result)
		env->ccx_rac_lv = rac_lv;

	BB_DBG(bb, DBG_ENV_MNTR, "ccx racing success=%d\n", set_result);

	return set_result;
}

void halbb_ccx_trigger(struct bb_info *bb, u8 func_sel)
{
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;
	struct bb_env_mntr_cr_info *cr = &env->bb_env_mntr_cr_i;

	BB_DBG(bb, DBG_ENV_MNTR, "[%s]===>\n", __func__);

	/*Due to IFS_CLM clock gating : [HALBB-58]*/
	halbb_set_reg_curr_phy(bb, cr->ifs_clm_clr, cr->ifs_clm_clr_m, false);
	halbb_set_reg_curr_phy(bb, cr->clm_en, cr->clm_en_m, false);
	if (func_sel & NHM_SEL)
		halbb_set_reg_curr_phy(bb, cr->nhm_en, cr->nhm_en_m, false);

	halbb_set_reg_curr_phy(bb, cr->edcca_clm_en, cr->edcca_clm_en_m, false);
	halbb_set_reg_curr_phy(bb, cr->ifs_clm_clr, cr->ifs_clm_clr_m, true);
	halbb_set_reg_curr_phy(bb, cr->clm_en, cr->clm_en_m, true);
	if (func_sel & NHM_SEL)
		halbb_set_reg_curr_phy(bb, cr->nhm_en, cr->nhm_en_m, true);

	halbb_set_reg_curr_phy(bb, cr->edcca_clm_en, cr->edcca_clm_en_m, true);

	env->ccx_trigger_time = bb->bb_sys_up_time;
	env->ccx_rpt_stamp++;
	env->ccx_ongoing = true;
}

void halbb_ccx_edcca_opt_set(struct bb_info *bb, enum ccx_edcca_opt_sc_idx sc)
{
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;
	struct bb_env_mntr_cr_info *cr = &env->bb_env_mntr_cr_i;
	u8 pri_ch = 0;
	u8 central_ch = 0;
	enum channel_width bw = 0;
	u8 pri_ch_idx = 0;
	u8 pri_sb_idx = 0;
	u8 edcca_opt = 0;

	BB_DBG(bb, DBG_ENV_MNTR, "[%s]===>\n", __func__);

	pri_ch = bb->hal_com->band[bb->bb_phy_idx].cur_chandef.chan;
	central_ch = bb->hal_com->band[bb->bb_phy_idx].cur_chandef.center_ch;
	bw = bb->hal_com->band[bb->bb_phy_idx].cur_chandef.bw;

	if (bb->ic_type & BB_IC_AX_SERIES) {
		/*==== [search pri_ch idx] ====*/
		if (central_ch <= 14) {
			// === 2G === //
			switch (bw) {
			case CHANNEL_WIDTH_20:
				break;
			case CHANNEL_WIDTH_40:
				pri_ch_idx = pri_ch > central_ch ? 1 : 2;
				break;
			default:
				break;
			}
		} else {
			// === 5G === //
			switch (bw) {
			case CHANNEL_WIDTH_20:
				break;
			case CHANNEL_WIDTH_40:
			case CHANNEL_WIDTH_80:
				if (pri_ch > central_ch)
					pri_ch_idx = (pri_ch - central_ch) >> 1;
				else
					pri_ch_idx = ((central_ch - pri_ch) >> 1) + 1;
				break;
			default:
				break;
			}
		}

		BB_DBG(bb, DBG_ENV_MNTR,
		       "sc_idx=%d, pri_ch=%d, cen_ch=%d, bw=%d, pri_ch_idx=%d\n",
		       sc, pri_ch, central_ch, bw, pri_ch_idx);

		/*s20edcca[0] = p0*/
		/*s20edcca[1] = s1*/
		/*s20edcca[2] = s2*/
		/*s20edcca[3] = s3*/
		/*sc_idx => 4|2|1|3*/
		/*4 : s20edcca[2] | s20edcca[3] | s20edcca[1] | s20edcca[0] = s2|s3|s1|p0*/
		/*2 : s20edcca[3] | s20edcca[2] | s20edcca[0] | s20edcca[1] = s3|s2|p0|s1*/
		/*1 : s20edcca[1] | s20edcca[0] | s20edcca[2] | s20edcca[3] = s1|p0|s2|s3*/
		/*3 : s20edcca[0] | s20edcca[1] | s20edcca[3] | s20edcca[2] = p0|s1|s3|s2*/
		/*others : 0=s20edcca[0]=p20, othters=0*/
		switch (pri_ch_idx) {
		case 4: /* edcca_opt[3] | edcca_opt[2] | edcca_opt[1] | edcca_opt[0] = s2|s3|s1|p0*/
			if (sc == CCX_EDCCA_P0)
				edcca_opt = 0;
			else if (sc == CCX_EDCCA_S1)
				edcca_opt = 1;
			else if (sc == CCX_EDCCA_S2)
				edcca_opt = 3;
			else if (sc == CCX_EDCCA_S3)
				edcca_opt = 2;
			break;
		case 2: /* edcca_opt[3] | edcca_opt[2] | edcca_opt[1] | edcca_opt[0] = s3|s2|p0|s1*/
			if (sc == CCX_EDCCA_P0)
				edcca_opt = 1;
			else if (sc == CCX_EDCCA_S1)
				edcca_opt = 0;
			else if (sc == CCX_EDCCA_S2)
				edcca_opt = 2;
			else if (sc == CCX_EDCCA_S3)
				edcca_opt = 3;
			break;
		case 1: /* edcca_opt[3] | edcca_opt[2] | edcca_opt[1] | edcca_opt[0] = s1|p0|s2|s3*/
			if (sc == CCX_EDCCA_P0)
				edcca_opt = 2;
			else if (sc == CCX_EDCCA_S1)
				edcca_opt = 3;
			else if (sc == CCX_EDCCA_S2)
				edcca_opt = 1;
			else if (sc == CCX_EDCCA_S3)
				edcca_opt = 0;
			break;
		case 3: /* edcca_opt[3] | edcca_opt[2] | edcca_opt[1] | edcca_opt[0] = p0|s1|s3|s2*/
			if (sc == CCX_EDCCA_P0)
				edcca_opt = 3;
			else if (sc == CCX_EDCCA_S1)
				edcca_opt = 2;
			else if (sc == CCX_EDCCA_S2)
				edcca_opt = 0;
			else if (sc == CCX_EDCCA_S3)
				edcca_opt = 1;
			break;
		default:
			edcca_opt = 0; /*s20edcca[0] = p20*/
			break;
		}

#ifdef HALBB_DBCC_SUPPORT
		/*phy1 is seg1 in edcca module*/
		if (bb->bb_phy_idx == HW_PHY_1)
			edcca_opt += 4; /*seg0:0~3, seg1:4~7*/
#endif
	} else { /*11BE*/
		pri_sb_idx = halbb_get_prim_sb(bb, central_ch, pri_ch, bw);
		BB_DBG(bb, DBG_ENV_MNTR,
		       "sc_idx=%d, pri_ch=%d, cen_ch=%d, bw=%d, pri_sb_idx=%d\n",
		       sc, pri_ch, central_ch, bw, pri_sb_idx);

		if (bb->ic_type == BB_RTL8922A) {
			/*s20edcca[0] = sb_idx(0)*/
			/*s20edcca[1] = sb_idx(1)*/
			/*s20edcca[2] = sb_idx(2)*/
			/*s20edcca[3] = sb_idx(3)*/
			/*sb_idx => 0|1|2|3*/
			/*0 : s20edcca[2] | s20edcca[3] | s20edcca[1] | s20edcca[0] = s3|s2|s1|p0*/
			/*1 : s20edcca[3] | s20edcca[2] | s20edcca[0] | s20edcca[1] = s3|s2|s1|p0*/
			/*2 : s20edcca[1] | s20edcca[0] | s20edcca[2] | s20edcca[3] = s2|s3|p0|s1*/
			/*3 : s20edcca[0] | s20edcca[1] | s20edcca[3] | s20edcca[2] = s2|s3|p0|s1*/
			/*others : 0=s20edcca[0]=sb_idx(0), othters=0*/
			switch (pri_sb_idx) {
			case 0: /* edcca_opt[3] | edcca_opt[2] | edcca_opt[1] | edcca_opt[0] = s3|s2|s1|p0*/
				if (sc == CCX_EDCCA_P0)
					edcca_opt = 0;
				else if (sc == CCX_EDCCA_S1)
					edcca_opt = 1;
				else if (sc == CCX_EDCCA_S2)
					edcca_opt = 2;
				else if (sc == CCX_EDCCA_S3)
					edcca_opt = 3;
				break;
			case 1: /* edcca_opt[3] | edcca_opt[2] | edcca_opt[1] | edcca_opt[0] = s3|s2|s1|p0*/
				if (sc == CCX_EDCCA_P0)
					edcca_opt = 0;
				else if (sc == CCX_EDCCA_S1)
					edcca_opt = 1;
				else if (sc == CCX_EDCCA_S2)
					edcca_opt = 2;
				else if (sc == CCX_EDCCA_S3)
					edcca_opt = 3;
				break;
			case 2: /* edcca_opt[3] | edcca_opt[2] | edcca_opt[1] | edcca_opt[0] = s2|s3|p0|s1*/
				if (sc == CCX_EDCCA_P0)
					edcca_opt = 1;
				else if (sc == CCX_EDCCA_S1)
					edcca_opt = 0;
				else if (sc == CCX_EDCCA_S2)
					edcca_opt = 3;
				else if (sc == CCX_EDCCA_S3)
					edcca_opt = 2;
				break;
			case 3: /* edcca_opt[3] | edcca_opt[2] | edcca_opt[1] | edcca_opt[0] = s2|s3|p0|s1*/
				if (sc == CCX_EDCCA_P0)
					edcca_opt = 1;
				else if (sc == CCX_EDCCA_S1)
					edcca_opt = 0;
				else if (sc == CCX_EDCCA_S2)
					edcca_opt = 3;
				else if (sc == CCX_EDCCA_S3)
					edcca_opt = 2;
				break;
			default:
				edcca_opt = 0;/*s20edcca[0]=sb_idx(0)*/
				break;
			}
		} else { /*other 11BE*/
			/*s20edcca[i] = sb_idx(i)*/
			switch (pri_sb_idx % 4) {
			case 0:
				if (sc == CCX_EDCCA_P0)
					edcca_opt = 0;
				else if (sc == CCX_EDCCA_S1)
					edcca_opt = 1;
				else if (sc == CCX_EDCCA_S2)
					edcca_opt = 3;
				else if (sc == CCX_EDCCA_S3)
					edcca_opt = 2;
				break;
			case 1:
				if (sc == CCX_EDCCA_P0)
					edcca_opt = 1;
				else if (sc == CCX_EDCCA_S1)
					edcca_opt = 0;
				else if (sc == CCX_EDCCA_S2)
					edcca_opt = 2;
				else if (sc == CCX_EDCCA_S3)
					edcca_opt = 3;
				break;
			case 2:
				if (sc == CCX_EDCCA_P0)
					edcca_opt = 2;
				else if (sc == CCX_EDCCA_S1)
					edcca_opt = 3;
				else if (sc == CCX_EDCCA_S2)
					edcca_opt = 1;
				else if (sc == CCX_EDCCA_S3)
					edcca_opt = 0;
				break;
			case 3:
				if (sc == CCX_EDCCA_P0)
					edcca_opt = 3;
				else if (sc == CCX_EDCCA_S1)
					edcca_opt = 2;
				else if (sc == CCX_EDCCA_S2)
					edcca_opt = 0;
				else if (sc == CCX_EDCCA_S3)
					edcca_opt = 1;
				break;
			default:
				break;
			}
			edcca_opt += 4 * (pri_sb_idx % 4);
		}
	}

	if (env->ccx_edcca_opt != edcca_opt) {
		halbb_set_reg_curr_phy(bb, cr->ccx_edcca_opt,
				       cr->ccx_edcca_opt_m, edcca_opt);
		BB_DBG(bb, DBG_ENV_MNTR,
		       "Update EDCCA_OPT ((%d)) -> ((%d))\n",
		       env->ccx_edcca_opt, edcca_opt);
		env->ccx_edcca_opt = edcca_opt;
	}
}

void halbb_ccx_ext_loss_update(struct bb_info *bb)
{
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;
	struct bb_ch_info *ch = &bb->bb_ch_i;
	u8 i = 0;
	char ext_loss[HALBB_MAX_PATH][HALBB_SNPRINT_SIZE];
	char ccx_ext_loss[HALBB_SNPRINT_SIZE];

	if (((bb->ic_type != BB_RTL8852A) && (bb->ic_type != BB_RTL8852B) &&
	    (bb->ic_type != BB_RTL8852C) && (bb->ic_type != BB_RTL8192XB) &&
	    (bb->ic_type != BB_RTL8851B))) {
		env->ccx_ext_loss_update = false;
		return;
	}

	env->ccx_ext_loss = (ch->ext_loss_avg + 1) / 2;

	if (env->ccx_ext_loss != env->ccx_ext_loss_pre)
		env->ccx_ext_loss_update = true;
	else
		env->ccx_ext_loss_update = false;
	env->ccx_ext_loss_pre = env->ccx_ext_loss;

	for (i = 0; i < HALBB_MAX_PATH; i++)
		halbb_print_sign_frac_digit(bb, ch->ext_loss[i], 8, 2,
					    ext_loss[i], HALBB_SNPRINT_SIZE);

	halbb_print_sign_frac_digit(bb, ch->ext_loss_avg, 8, 2, bb->dbg_buf,
				    HALBB_SNPRINT_SIZE);
	halbb_print_sign_frac_digit(bb, env->ccx_ext_loss, 8, 1, ccx_ext_loss,
				    HALBB_SNPRINT_SIZE);

	if (bb->num_rf_path >= 4) {
		BB_DBG(bb, DBG_ENV_MNTR,
		       "ext_loss{avg, a, b, c, d} = {%s, %s, %s, %s, %s} dB, ccx_ext_loss = %s dB, ccx_ext_loss_update = %d\n",
		       bb->dbg_buf, ext_loss[0], ext_loss[1], ext_loss[2],
		       ext_loss[3], ccx_ext_loss, env->ccx_ext_loss_update);
	} else if (bb->num_rf_path >= 2) {
		BB_DBG(bb, DBG_ENV_MNTR,
		       "ext_loss{avg, a, b} = {%s, %s, %s} dB, ccx_ext_loss = %s dB, ccx_ext_loss_update = %d\n",
		       bb->dbg_buf, ext_loss[0], ext_loss[1], ccx_ext_loss,
		       env->ccx_ext_loss_update);
	} else {
		BB_DBG(bb, DBG_ENV_MNTR, "ext_loss=%s dB, ccx_ext_loss = %s dB, ccx_ext_loss_update = %d\n",
		       ext_loss[0], ccx_ext_loss, env->ccx_ext_loss_update);
	}
}

#ifdef NHM_SUPPORT

void halbb_nhm_cal_wgt(struct bb_info *bb)
{
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;
	u8 i = 0;

	for (i = 0; i < NHM_RPT_NUM; i++) {
		if (i == 0)
			env->nhm_wgt[i] = (u8)(MAX_2(env->nhm_th[i] - 2, 0));
		else if (i == (NHM_RPT_NUM - 1))
			env->nhm_wgt[i] = (u8)(env->nhm_th[i - 1] + 2);
		else
			env->nhm_wgt[i] = (u8)((env->nhm_th[i - 1] +
						env->nhm_th[i]) >> 1);
	}

	if ((bb->ic_type == BB_RTL8852A) || (bb->ic_type == BB_RTL8852B) ||
	    (bb->ic_type == BB_RTL8852C)) {
		if (env->nhm_th[NHM_TH_NUM - 1] == RSSI_2_NHM_TH(NHM_WA_TH)) {
			env->nhm_wgt[NHM_RPT_NUM - 1] = (u8)(env->nhm_th[NHM_TH_NUM - 2] + 2);
			env->nhm_wgt[NHM_RPT_NUM - 2] = env->nhm_wgt[NHM_RPT_NUM - 1];
		}
	}

	BB_DBG(bb, DBG_ENV_MNTR,
	       "Update NHM_wgt(H->L)[%d %d %d %d %d %d %d %d %d %d %d %d]\n",
	       env->nhm_wgt[11], env->nhm_wgt[10], env->nhm_wgt[9],
	       env->nhm_wgt[8], env->nhm_wgt[7], env->nhm_wgt[6],
	       env->nhm_wgt[5], env->nhm_wgt[4], env->nhm_wgt[3],
	       env->nhm_wgt[2], env->nhm_wgt[1], env->nhm_wgt[0]);
}

u8 halbb_nhm_cal_wgt_avg(struct bb_info *bb, u8 start_i, u8 end_i, u16 n_sum,
			 u8 frac_bit_num)
{
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;
	u8 i = 0;
	u32 tmp = 0;
	u8 wgt_avg = 0;
	u8 nhm_valid = 0;

	BB_DBG(bb, DBG_ENV_MNTR, "[%s]===>\n", __func__);

	if (n_sum == 0) {
		BB_DBG(bb, DBG_ENV_MNTR,
			  "result_sum = 0, don't need to update\n");
		return 0;
	} else if (end_i > NHM_RPT_NUM - 1) {
		BB_DBG(bb, DBG_ENV_MNTR,
			  "[WARNING]nhm_end_idx is larger than 11!!\n");
		return 0;
	}

	for (i = start_i; i <= end_i; i++)
		tmp += env->nhm_sw_result[i] * env->nhm_wgt[i];

	if (frac_bit_num == 1) /*u(8.1)*/
		wgt_avg = (u8)HALBB_DIV(tmp, n_sum);
	else /*u(8.0)*/
		wgt_avg = (u8)(NHM_TH_2_RSSI(HALBB_DIV(tmp, n_sum)));

	nhm_valid = (u8)halbb_ccx_get_ratio(bb, n_sum, 100);
	BB_DBG(bb, DBG_ENV_MNTR,
	       "valid: ((%d)) percent, wgt_avg(RSSI)=((%d))\n",
	       nhm_valid, wgt_avg);

	return wgt_avg;
}

u16 halbb_nhm_exclu_noise_figure(struct bb_info *bb)
{
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;
	struct rtw_hw_band *hw_band = &bb->hal_com->band[bb->bb_phy_idx];
	u8 first_idx = 255;
	u8 second_idx = 255;
	u16 non_noise_f = 0;
	u8 noise_f_th = 0;
	u8 i = 0;
	u8 bw = 0;

	bw = bb->hal_com->band[0].cur_chandef.bw;
	non_noise_f = env->nhm_result_sum;

	/*search first & second cluster*/
	for (i = 0; i < NHM_RPT_NUM; i++) {
		if (env->nhm_rpt[i]) {
			if (first_idx == 255) {
				first_idx = i;
			} else if (second_idx == 255) {
				second_idx = i;
				break;
			}
		}
	}

	if ((bw == CHANNEL_WIDTH_160) || (bw == CHANNEL_WIDTH_80_80))
		noise_f_th = NHM_160M_NOISE_F_TH;
	else
		noise_f_th = NHM_NOISE_F_TH;

	/*exclude first cluster under noise_f_th*/
	for (i = 0; i < NHM_RPT_NUM; i++) {
		if ((i < (first_idx + 3)) && (env->nhm_wgt[i] <= noise_f_th))
			non_noise_f -= env->nhm_sw_result[i];
	}

	/*exclude first_idx and second_idx above -80dBm for 52A 2.4G DIG issue*/
	if ((bb->ic_type == BB_RTL8852A) &&
	    (hw_band->cur_chandef.band == BAND_ON_24G)) {
		if ((first_idx != 255) &&
		    (env->nhm_wgt[first_idx] > NHM_NOISE_F_TH))
			non_noise_f -= env->nhm_sw_result[first_idx];

		if ((second_idx != 255) &&
		    (env->nhm_wgt[second_idx] > NHM_NOISE_F_TH))
			non_noise_f -= env->nhm_sw_result[second_idx];
	}

	BB_DBG(bb, DBG_ENV_MNTR,
	       "cal non_noise_f: 1st_i=%d, 2nd_i=%d, non_noise_f=%d\n",
	       first_idx, second_idx, non_noise_f);

	return non_noise_f;
}

void halbb_nhm_get_utility(struct bb_info *bb)
{
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;
	struct bb_edcca_info *bb_edcca = &bb->bb_edcca_i;
	struct edcca_hw_rpt *edcca_r = &bb_edcca->edcca_rpt;
	u16 non_noise_f = 0;
	u8 i = 0;

	halbb_nhm_cal_wgt(bb);

	for (i = 0; i < NHM_RPT_NUM; i++) {
		if (((bb->ic_type == BB_RTL8852A) ||
		    (bb->ic_type == BB_RTL8852B) ||
		    (bb->ic_type == BB_RTL8852C)) &&
		    (i == (NHM_RPT_NUM - 1))) {
			env->nhm_sw_result[0] += env->nhm_result[i];
			env->nhm_sw_result[i] = 0;
		} else {
			env->nhm_sw_result[i] = env->nhm_result[i];
		}
	}

	BB_DBG(bb, DBG_ENV_MNTR,
	       "NHM sw result[%d](H->L)[%d %d %d %d %d %d %d %d %d %d %d %d]\n",
	       env->ccx_rpt_stamp, env->nhm_sw_result[11],
	       env->nhm_sw_result[10], env->nhm_sw_result[9],
	       env->nhm_sw_result[8], env->nhm_sw_result[7],
	       env->nhm_sw_result[6], env->nhm_sw_result[5],
	       env->nhm_sw_result[4], env->nhm_sw_result[3],
	       env->nhm_sw_result[2], env->nhm_sw_result[1],
	       env->nhm_sw_result[0]);

	for (i = 0; i < NHM_RPT_NUM; i++)
		env->nhm_rpt[i] = (u8)halbb_ccx_get_ratio(bb,
				  env->nhm_sw_result[i], 100);

	/*exclude nhm_r[0] above noise_f_th or first cluster under noise_f_th*/
	non_noise_f = halbb_nhm_exclu_noise_figure(bb);
	env->nhm_ratio = (u8)halbb_ccx_get_ratio(bb, non_noise_f, 100);
	env->nhm_tx_ratio = (u8)halbb_ccx_get_ratio(bb, env->nhm_tx_cnt, 100);
	env->nhm_cca_ratio = (u8)halbb_ccx_get_ratio(bb, env->nhm_cca_cnt, 100);
	env->nhm_idle_ratio = (u8)halbb_ccx_get_ratio(bb, env->nhm_idle_cnt,
						      100);
	env->nhm_pwr = halbb_nhm_cal_wgt_avg(bb, 0, NHM_RPT_NUM - 1,
					     env->nhm_result_sum, 0);
	env->nhm_pwr_0p5 = halbb_nhm_cal_wgt_avg(bb, 0, NHM_RPT_NUM - 1,
						 env->nhm_result_sum, 1);

	if (bb->ic_type == BB_RTL8852A) {
		if ((edcca_r->pwdb_fb != (s8)(EDCCA_PWDB_EXCLU_TX)) &&
		    (EDCCA_PWDB_TO_RSSI(edcca_r->pwdb_fb) <= NHM_WA_PWR))
			env->edcca_noise_bg = EDCCA_PWDB_TO_RSSI(edcca_r->pwdb_fb);

		BB_DBG(bb, DBG_ENV_MNTR, "edcca_noise_bg, nhm_pwr = {%d, %d}\n",
		       env->edcca_noise_bg, env->nhm_pwr);

		if (env->nhm_pwr <= NHM_WA_PWR) {
			env->nhm_pwr = env->edcca_noise_bg;
			env->nhm_pwr_0p5 = (u8)(env->nhm_pwr << 1);
		}
	}

	BB_DBG(bb, DBG_ENV_MNTR, "cnt ratio{cca, tx, idle} = {%d, %d, %d}\n",
	       env->nhm_cca_ratio, env->nhm_tx_ratio, env->nhm_idle_ratio);

	BB_DBG(bb, DBG_ENV_MNTR,
	       "nhm_ratio=%d, nhm_pwr=%d, nhm_pwr_0p5=%d.%d\n",
	       env->nhm_ratio, env->nhm_pwr, env->nhm_pwr_0p5 >> 1,
	       5 * (env->nhm_pwr_0p5 & 0x1));
}

bool halbb_nhm_get_fw_result_h2c(struct bb_info *bb)
{
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;
	u8 env_h2c_content[4];
	u16 cmdlen = 2;
	u32 *bb_h2c = (u32 *)env_h2c_content;
	bool h2c_ret_val = true;

	BB_DBG(bb, DBG_ENV_MNTR, "[%s]===>\n", __func__);

	if (env->nhm_app != NHM_FW_CTRL) {
		BB_DBG(bb, DBG_ENV_MNTR, "NHM is controlled by driver!\n");
		return HALBB_SET_FAIL;
	}

	env_h2c_content[0] = (u8)NHM_SEL;
	env_h2c_content[1] = (u8)bb->bb_phy_idx;
	env_h2c_content[2] = 0; /*rsvd*/
	env_h2c_content[3] = 0; /*rsvd*/

	h2c_ret_val = halbb_fill_h2c_cmd(bb, cmdlen, DM_H2C_FW_ENV_MNTR,
					 HALBB_H2C_DM, bb_h2c);

	BB_DBG(bb, DBG_ENV_MNTR, "ENV_h2c: %x\n", bb_h2c[0]);

	if (h2c_ret_val == false)
		BB_WARNING("ENV H2C cmd error!!\n");

	return true;
}

void halbb_nhm_get_fw_result_c2h(struct bb_info *bb_0, u8 *c2h)
{
	struct bb_info *bb = bb_0;
	struct bb_env_mntr_info *env = NULL;
	struct bb_c2h_nhm_info *env_c2h;
	struct env_mntr_rpt *env_bg = NULL;
	enum phl_phy_idx phy_idx;

	BB_DBG(bb, DBG_ENV_MNTR, "[%s]===>\n", __func__);

	env_c2h = (struct bb_c2h_nhm_info *)c2h;
	phy_idx = (enum phl_phy_idx)env_c2h->phy_idx;

#ifdef HALBB_DBCC_SUPPORT
	HALBB_GET_PHY_PTR(bb_0, bb, phy_idx);
	BB_DBG(bb, DBG_ENV_MNTR, "[%s] phy_idx=%d\n", __func__, bb->bb_phy_idx);
#endif
	env = &bb->bb_env_mntr_i;
	env_bg = &env->env_mntr_rpt_bg;

	if (env->nhm_app != NHM_FW_CTRL) {
		BB_DBG(bb, DBG_ENV_MNTR, "NHM is controlled by driver!\n");
		return;
	}

	halbb_mem_cpy(bb, &env->nhm_rpt[0], &env_c2h->nhm_rpt[0], NHM_RPT_NUM);
	halbb_mem_cpy(bb, &env_bg->nhm_rpt[0], &env_c2h->nhm_rpt[0], NHM_RPT_NUM);

	env->nhm_tx_ratio = env_c2h->nhm_tx_ratio;
	env->nhm_cca_ratio = env_c2h->nhm_cca_ratio;
	env->nhm_idle_ratio = env_c2h->nhm_idle_ratio;
	env->nhm_ratio = env_c2h->nhm_ratio;
	env->nhm_pwr_0p5 = env_c2h->nhm_pwr_0p5;
	env->nhm_pwr = (u8)(env_c2h->nhm_pwr_0p5 >> 1);

	env_bg->nhm_tx_ratio = env_c2h->nhm_tx_ratio;
	env_bg->nhm_cca_ratio = env_c2h->nhm_cca_ratio;
	env_bg->nhm_idle_ratio = env_c2h->nhm_idle_ratio;
	env_bg->nhm_ratio = env_c2h->nhm_ratio;
	env_bg->nhm_pwr_0p5 = env_c2h->nhm_pwr_0p5;
	env_bg->nhm_pwr = (u8)(env_c2h->nhm_pwr_0p5 >> 1);

	BB_DBG(bb, DBG_ENV_MNTR,
	       "NHM fw rpt(H->L)[%d %d %d %d %d %d %d %d %d %d %d %d]\n",
	       env->nhm_rpt[11], env->nhm_rpt[10], env->nhm_rpt[9],
	       env->nhm_rpt[8], env->nhm_rpt[7], env->nhm_rpt[6],
	       env->nhm_rpt[5], env->nhm_rpt[4], env->nhm_rpt[3],
	       env->nhm_rpt[2], env->nhm_rpt[1], env->nhm_rpt[0]);

	BB_DBG(bb, DBG_ENV_MNTR, "cnt ratio{cca, tx, idle} = {%d, %d, %d}\n",
	       env->nhm_cca_ratio, env->nhm_tx_ratio, env->nhm_idle_ratio);

	BB_DBG(bb, DBG_ENV_MNTR,
	       "nhm_ratio=%d, nhm_pwr=%d, nhm_pwr_0p5=%d.%d\n",
	       env->nhm_ratio, env->nhm_pwr, env->nhm_pwr_0p5 >> 1,
	       5 * (env->nhm_pwr_0p5 & 0x1));
}

bool halbb_nhm_get_result(struct bb_info *bb)
{
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;
	struct bb_env_mntr_cr_info *cr = &env->bb_env_mntr_cr_i;
	u8 i = 0;
	u32 result_sum_tmp = 0;

	BB_DBG(bb, DBG_ENV_MNTR, "[%s]===>\n", __func__);

	if (env->nhm_app == NHM_FW_CTRL) {
		BB_DBG(bb, DBG_ENV_MNTR, "NHM is controlled by FW!\n");
		return HALBB_SET_FAIL;
	}

	if (!(halbb_get_reg_curr_phy(bb, cr->nhm_rdy, cr->nhm_rdy_m))) {
		BB_DBG(bb, DBG_ENV_MNTR, "Get NHM report Fail\n");
		return false;
	}

	env->nhm_result[0] = (u16)halbb_get_reg_curr_phy(bb, cr->nhm_cnt0,
							 cr->nhm_cnt0_m);
	env->nhm_result[1] = (u16)halbb_get_reg_curr_phy(bb, cr->nhm_cnt1,
							 cr->nhm_cnt1_m);
	env->nhm_result[2] = (u16)halbb_get_reg_curr_phy(bb, cr->nhm_cnt2,
							 cr->nhm_cnt2_m);
	env->nhm_result[3] = (u16)halbb_get_reg_curr_phy(bb, cr->nhm_cnt3,
							 cr->nhm_cnt3_m);
	env->nhm_result[4] = (u16)halbb_get_reg_curr_phy(bb, cr->nhm_cnt4,
							 cr->nhm_cnt4_m);
	env->nhm_result[5] = (u16)halbb_get_reg_curr_phy(bb, cr->nhm_cnt5,
							 cr->nhm_cnt5_m);
	env->nhm_result[6] = (u16)halbb_get_reg_curr_phy(bb, cr->nhm_cnt6,
							 cr->nhm_cnt6_m);
	env->nhm_result[7] = (u16)halbb_get_reg_curr_phy(bb, cr->nhm_cnt7,
							 cr->nhm_cnt7_m);
	env->nhm_result[8] = (u16)halbb_get_reg_curr_phy(bb, cr->nhm_cnt8,
							 cr->nhm_cnt8_m);
	env->nhm_result[9] = (u16)halbb_get_reg_curr_phy(bb, cr->nhm_cnt9,
							 cr->nhm_cnt9_m);
	env->nhm_result[10] = (u16)halbb_get_reg_curr_phy(bb, cr->nhm_cnt10,
							  cr->nhm_cnt10_m);
	env->nhm_result[11] = (u16)halbb_get_reg_curr_phy(bb, cr->nhm_cnt11,
							  cr->nhm_cnt11_m);

	for (i = 0; i < NHM_RPT_NUM; i++)
		result_sum_tmp += (u32)env->nhm_result[i];

	env->nhm_result_sum = (u16)result_sum_tmp;
	BB_DBG(bb, DBG_ENV_MNTR, "nhm_result_sum=%d\n", env->nhm_result_sum);

	/*Get NHM cnt*/
	env->nhm_cca_cnt = (u16)halbb_get_reg_curr_phy(bb, cr->nhm_cca_cnt,
						       cr->nhm_cca_cnt_m);
	env->nhm_tx_cnt = (u16)halbb_get_reg_curr_phy(bb, cr->nhm_tx_cnt,
						      cr->nhm_tx_cnt_m);
	env->nhm_idle_cnt = (u16)halbb_get_reg_curr_phy(bb, cr->nhm_idle_cnt,
							cr->nhm_idle_cnt_m);

	BB_DBG(bb, DBG_ENV_MNTR, "cnt{cca, tx, idle} = {%d, %d, %d}\n",
	       env->nhm_cca_cnt, env->nhm_tx_cnt, env->nhm_idle_cnt);

	BB_DBG(bb, DBG_ENV_MNTR,
	       "NHM hw result[%d](H->L)[%d %d %d %d %d %d %d %d %d %d %d %d]\n",
	       env->ccx_rpt_stamp, env->nhm_result[11], env->nhm_result[10],
	       env->nhm_result[9], env->nhm_result[8], env->nhm_result[7],
	       env->nhm_result[6], env->nhm_result[5], env->nhm_result[4],
	       env->nhm_result[3], env->nhm_result[2], env->nhm_result[1],
	       env->nhm_result[0]);

	halbb_nhm_get_utility(bb);

	return true;
}

void halbb_nhm_set_th_reg(struct bb_info *bb)
{
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;
	struct bb_env_mntr_cr_info *cr = &env->bb_env_mntr_cr_i;

	BB_DBG(bb, DBG_ENV_MNTR, "[%s]===>\n", __func__);

	if (((bb->ic_type == BB_RTL8852A) || (bb->ic_type == BB_RTL8852B) ||
	    (bb->ic_type == BB_RTL8852C) || (bb->ic_type == BB_RTL8192XB) ||
		(bb->ic_type == BB_RTL8851B))) {
		/*Set NHM threshold*/ /*Unit: RSSI U(8,1)*/
		halbb_set_reg_curr_phy(bb, cr->nhm_th0, cr->nhm_th0_m,
			env->nhm_th[0]-env->ccx_ext_loss);
		halbb_set_reg_curr_phy(bb, cr->nhm_th1, cr->nhm_th1_m,
			env->nhm_th[1]-env->ccx_ext_loss);
		halbb_set_reg_curr_phy(bb, cr->nhm_th2, cr->nhm_th2_m,
			env->nhm_th[2]-env->ccx_ext_loss);
		halbb_set_reg_curr_phy(bb, cr->nhm_th3, cr->nhm_th3_m,
			env->nhm_th[3]-env->ccx_ext_loss);
		halbb_set_reg_curr_phy(bb, cr->nhm_th4, cr->nhm_th4_m,
			env->nhm_th[4]-env->ccx_ext_loss);
		halbb_set_reg_curr_phy(bb, cr->nhm_th5, cr->nhm_th5_m,
			env->nhm_th[5]-env->ccx_ext_loss);
		halbb_set_reg_curr_phy(bb, cr->nhm_th6, cr->nhm_th6_m,
			env->nhm_th[6]-env->ccx_ext_loss);
		halbb_set_reg_curr_phy(bb, cr->nhm_th7, cr->nhm_th7_m,
			env->nhm_th[7]-env->ccx_ext_loss);
		halbb_set_reg_curr_phy(bb, cr->nhm_th8, cr->nhm_th8_m,
			env->nhm_th[8]-env->ccx_ext_loss);
		halbb_set_reg_curr_phy(bb, cr->nhm_th9, cr->nhm_th9_m,
			env->nhm_th[9]-env->ccx_ext_loss);
		if ((bb->ic_type == BB_RTL8852A) || (bb->ic_type == BB_RTL8852B) ||
			(bb->ic_type == BB_RTL8852C))
			halbb_set_reg_curr_phy(bb, cr->nhm_th10, cr->nhm_th10_m,
				RSSI_2_NHM_TH(NHM_WA_TH));
		else
			halbb_set_reg_curr_phy(bb, cr->nhm_th10, cr->nhm_th10_m,
				env->nhm_th[10]-env->ccx_ext_loss);
	} else {
		/*Set NHM threshold*/ /*Unit: RSSI U(8,1)*/
		halbb_set_reg_curr_phy(bb, cr->nhm_th0, cr->nhm_th0_m, env->nhm_th[0]);
		halbb_set_reg_curr_phy(bb, cr->nhm_th1, cr->nhm_th1_m, env->nhm_th[1]);
		halbb_set_reg_curr_phy(bb, cr->nhm_th2, cr->nhm_th2_m, env->nhm_th[2]);
		halbb_set_reg_curr_phy(bb, cr->nhm_th3, cr->nhm_th3_m, env->nhm_th[3]);
		halbb_set_reg_curr_phy(bb, cr->nhm_th4, cr->nhm_th4_m, env->nhm_th[4]);
		halbb_set_reg_curr_phy(bb, cr->nhm_th5, cr->nhm_th5_m, env->nhm_th[5]);
		halbb_set_reg_curr_phy(bb, cr->nhm_th6, cr->nhm_th6_m, env->nhm_th[6]);
		halbb_set_reg_curr_phy(bb, cr->nhm_th7, cr->nhm_th7_m, env->nhm_th[7]);
		halbb_set_reg_curr_phy(bb, cr->nhm_th8, cr->nhm_th8_m, env->nhm_th[8]);
		halbb_set_reg_curr_phy(bb, cr->nhm_th9, cr->nhm_th9_m, env->nhm_th[9]);
		halbb_set_reg_curr_phy(bb, cr->nhm_th10, cr->nhm_th10_m,
				       env->nhm_th[10]);
	}

	BB_DBG(bb, DBG_ENV_MNTR,
	       "Update NHM_th[H->L]=[%d %d %d %d %d %d %d %d %d %d %d]\n",
	       env->nhm_th[10], env->nhm_th[9], env->nhm_th[8], env->nhm_th[7],
	       env->nhm_th[6], env->nhm_th[5], env->nhm_th[4], env->nhm_th[3],
	       env->nhm_th[2], env->nhm_th[1], env->nhm_th[0]);
}

bool
halbb_nhm_th_update_chk(struct bb_info *bb, struct ccx_para_info *para,
			u8 *nhm_th)
{
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;
	bool is_app_change = (env->nhm_app == para->nhm_app) ? false : true;
	bool is_ccx_ext_loss_update = env->ccx_ext_loss_update;
	bool is_update = (is_app_change || is_ccx_ext_loss_update);
	u8 nhm_th_11k[NHM_TH_NUM] = {18, 21, 24, 27, 30, 35, 40, 45, 50, 55,
				     60}; /*Unit RSSI*/
	u8 i = 0;
	u8 th_ofst = 3;
	u8 th0 = 0;

	BB_DBG(bb, DBG_ENV_MNTR, "nhm_App=%d, is_ccx_ext_loss_update=%d\n",
		para->nhm_app, is_ccx_ext_loss_update);

	if (!is_update)
		goto CHK_NHM_UPDATE_FINISHED;

	switch (para->nhm_app) {
	case NHM_INIT:
	case NHM_BACKGROUND: /* IEEE 11K*/
	case NHM_DBG_11K:
	case NHM_ACS:
		is_update = true;
		for (i = 0; i < NHM_TH_NUM; i++) {
			if (((bb->ic_type == BB_RTL8852A) ||
			    (bb->ic_type == BB_RTL8852B) ||
			    (bb->ic_type == BB_RTL8852C)) &&
			    (i == (NHM_TH_NUM - 1)))
				nhm_th[i] = RSSI_2_NHM_TH(NHM_WA_TH);
			else
				nhm_th[i] = RSSI_2_NHM_TH(nhm_th_11k[i]);
		}
		break;
	case NHM_DBG_MANUAL:
		is_update = true;
		th_ofst = para->nhm_manual_th_ofst;
		if ((bb->ic_type == BB_RTL8852A) ||
		    (bb->ic_type == BB_RTL8852B) ||
		    (bb->ic_type == BB_RTL8852C)) {
			/*nhm_th[0] upper bound is 109 - 10 * th_ofst*/
			th0 = MIN_2(para->nhm_manual_th0,
				    NHM_WA_TH - th_ofst * (NHM_TH_NUM - 1));
			for (i = 0; i < NHM_TH_NUM; i++) {
				if (i == (NHM_TH_NUM - 1))
					nhm_th[i] = RSSI_2_NHM_TH(NHM_WA_TH);
				else
					nhm_th[i] = RSSI_2_NHM_TH(th0 +
								  (th_ofst *
								   i));
			}
		} else {
			/*nhm_th[0] upper bound is 127 - 10 * th_ofst*/
			th0 = MIN_2(para->nhm_manual_th0,
				    127 - th_ofst * (NHM_TH_NUM - 1));
			for (i = 0; i < NHM_TH_NUM; i++)
				nhm_th[i] = RSSI_2_NHM_TH(th0 + (th_ofst * i));
		}
		break;
	default:
		break;
	}

CHK_NHM_UPDATE_FINISHED:
	if (!is_update)
		BB_DBG(bb, DBG_ENV_MNTR, "No need to update NHM_TH\n");

	return is_update;
}

bool halbb_nhm_set(struct bb_info *bb, struct ccx_para_info *para)
{
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;
	struct bb_link_info *link = &bb->bb_link_i;
	struct bb_env_mntr_cr_info *cr = &env->bb_env_mntr_cr_i;
	enum channel_width bw = bb->hal_com->band[bb->bb_phy_idx].cur_chandef.bw;
	u16 mntr_time = 0;
	u32 period = 65535;
	u32 unit_idx = 0;
	u8 nhm_th[NHM_TH_NUM] = {0};

	BB_DBG(bb, DBG_ENV_MNTR, "[%s]===>\n", __func__);

	if (para->mntr_time == 0) {
		BB_DBG(bb, DBG_ENV_MNTR, "[WARNING] MNTR_TIME is 0\n");
		return HALBB_SET_FAIL;
	}

	if (para->nhm_app == NHM_DBG_MANUAL && para->nhm_manual_th_ofst == 0) {
		BB_DBG(bb, DBG_ENV_MNTR,
		       "[WARNING] th_ofst is 0 when nhm_app = manual\n");
		return HALBB_SET_FAIL;
	}

	if (para->nhm_app == NHM_DBG_MANUAL && para->nhm_manual_th_ofst > 12) {
		BB_DBG(bb, DBG_ENV_MNTR,
		       "[WARNING] th_ofst is larger than 12 when nhm_app = manual\n");
		return HALBB_SET_FAIL;
	}

	if (env->nhm_app == NHM_FW_CTRL) {
		BB_DBG(bb, DBG_ENV_MNTR, "NHM is controlled by FW!\n");
		return HALBB_SET_FAIL;
	}

	if (halbb_ccx_racing_ctrl(bb, para->rac_lv) == HALBB_SET_FAIL)
		return HALBB_SET_FAIL;

	BB_DBG(bb, DBG_ENV_MNTR, "mntr_time=%d ms, nhm_incld_cca=%d\n",
	       para->mntr_time, para->nhm_incld_cca);

	if ((bb->ic_type != BB_RTL8852A) && (bb->ic_type != BB_RTL8852B) &&
	    (bb->ic_type != BB_RTL8851B))
		BB_DBG(bb, DBG_ENV_MNTR,
		       "nhm_nav_en=%d, nhm_rssi_th_en=%d, nhm_rssi_th=%d\n",
		       para->nhm_nav_en, para->nhm_rssi_th_en,
		       para->nhm_rssi_th);

	/*Set unit & period*/
	if (bw == CHANNEL_WIDTH_10)
		mntr_time = para->mntr_time >> 1;
	else if (bw == CHANNEL_WIDTH_5)
		mntr_time = para->mntr_time >> 2;
	else
		mntr_time = para->mntr_time;

	if (mntr_time != env->nhm_mntr_time) {
		halbb_ccx_ms_2_period_unit(bb, mntr_time, &period, &unit_idx);
		halbb_set_reg_curr_phy(bb, cr->nhm_period, cr->nhm_period_m,
				       period);
		halbb_set_reg_curr_phy(bb, cr->nhm_unit_idx, cr->nhm_unit_idx_m,
				       unit_idx);

		BB_DBG(bb, DBG_ENV_MNTR, "Update NHM time ((%d)) -> ((%d)), bw = %d\n",
		       env->nhm_mntr_time, mntr_time, bw);

		env->nhm_mntr_time = mntr_time;
		env->ccx_period = (u16)period;
		env->ccx_unit_idx = (u8)unit_idx;
	}

	/*Set include cca*/
	if (para->nhm_incld_cca != env->nhm_include_cca) {
		halbb_set_reg_curr_phy(bb, cr->nhm_inclu_cca,
				       cr->nhm_inclu_cca_m,
				       para->nhm_incld_cca);

		BB_DBG(bb, DBG_ENV_MNTR,
		       "Update NHM include cca ((%d)) -> ((%d))\n",
		       env->nhm_include_cca, para->nhm_incld_cca);

		env->nhm_include_cca = para->nhm_incld_cca;
	}

	/*Set NHM threshold*/
	if (halbb_nhm_th_update_chk(bb, para, &nhm_th[0])) {
		env->nhm_app = para->nhm_app;
		halbb_mem_cpy(bb, &env->nhm_th[0], &nhm_th, NHM_TH_NUM);

		/*Set NHM th*/
		halbb_nhm_set_th_reg(bb);
	}

	if ((bb->ic_type != BB_RTL8852A) && (bb->ic_type != BB_RTL8852B) &&
	    (bb->ic_type != BB_RTL8851B)) {
		/*Set nav_en*/
		if (para->nhm_nav_en != env->nhm_nav_en) {
			halbb_set_reg_curr_phy(bb, cr->nhm_nav_en,
					       cr->nhm_nav_en_m,
					       para->nhm_nav_en);
			BB_DBG(bb, DBG_ENV_MNTR,
			       "Update NHM nav en ((%d)) -> ((%d))\n",
			       env->nhm_nav_en, para->nhm_nav_en);
			env->nhm_nav_en = para->nhm_nav_en;
		}

		/*Set rssi_th_en*/
		if (para->nhm_rssi_th_en != env->nhm_rssi_th_en) {
			halbb_set_reg_curr_phy(bb, cr->nhm_rssi_th_en,
					       cr->nhm_rssi_th_en_m,
					       para->nhm_rssi_th_en);
			BB_DBG(bb, DBG_ENV_MNTR,
			       "Update NHM rssi_th en ((%d)) -> ((%d))\n",
			       env->nhm_rssi_th_en, para->nhm_rssi_th_en);
			env->nhm_rssi_th_en = para->nhm_rssi_th_en;
		}

		/*Set rssi_th*/
		if (para->nhm_rssi_th != env->nhm_rssi_th) {
			halbb_set_reg_curr_phy(bb, cr->nhm_rssi_th,
					       cr->nhm_rssi_th_m,
					       para->nhm_rssi_th);
			BB_DBG(bb, DBG_ENV_MNTR,
			       "Update NHM rssi_th ((%d)) -> ((%d))\n",
			       env->nhm_rssi_th, para->nhm_rssi_th);
			env->nhm_rssi_th = para->nhm_rssi_th;
		}
	}

	return HALBB_SET_SUCCESS;
}

void halbb_nhm_init(struct bb_info *bb)
{
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;
	struct bb_env_mntr_cr_info *cr = &env->bb_env_mntr_cr_i;
	u8 i = 0;
	u8 nhm_th_11k[NHM_TH_NUM] = {18, 21, 24, 27, 30, 35, 40, 45, 50, 55,
				     60}; /*Unit RSSI*/
	u8 nhm_th[NHM_TH_NUM] = {0};

	BB_DBG(bb, DBG_ENV_MNTR, "[%s]===>\n", __func__);

	env->nhm_app = NHM_INIT;
	env->nhm_include_cca = NHM_CCA_INIT;
	if ((bb->ic_type != BB_RTL8852A) && (bb->ic_type != BB_RTL8852B)) {
		env->nhm_nav_en = NHM_NAV_EN_INIT;
		env->nhm_rssi_th_en = NHM_RSSI_TH_EN_INIT;
		env->nhm_rssi_th = 0;
	}
	env->nhm_mntr_time = 0;

	/*Set NHM threshold*/
	for (i = 0; i < NHM_TH_NUM; i++) {
		if (((bb->ic_type == BB_RTL8852A) ||
		    (bb->ic_type == BB_RTL8852B) ||
		    (bb->ic_type == BB_RTL8852C)) &&
		    (i == (NHM_TH_NUM - 1)))
			nhm_th[i] = RSSI_2_NHM_TH(NHM_WA_TH);
		else
			nhm_th[i] = RSSI_2_NHM_TH(nhm_th_11k[i]);
	}
	halbb_mem_cpy(bb, &env->nhm_th[0], &nhm_th, NHM_TH_NUM);

	/*if r_nhm_en = 0, nhm report will always be 0.*/
	halbb_set_reg_curr_phy(bb, cr->nhm_en, cr->nhm_en_m, true);

	/*r_nhm_pwdb_method_sel[0]=1 : select max path*/
	/*r_nhm_pwdb_method_sel[1] is dummy*/
	halbb_set_reg_curr_phy(bb, cr->nhm_method_sel, cr->nhm_method_sel_m, 1);
}

void halbb_nhm_dbg(struct bb_info *bb, char input[][16], u32 *_used,
		   char *output, u32 *_out_len)
{
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;
	struct ccx_para_info para = {0};
	u32 var[10] = {0};
	u8 i = 0;
	u8 end_i = 0;

	HALBB_SCAN(input[1], DCMD_DECIMAL, &var[0]);

	if ((_os_strcmp(input[1], "-h") == 0)) {
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "NHM Get Result: {100}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "Basic-Trigger(11k/1900ms): {1}\n");
		if ((bb->ic_type == BB_RTL8852A) ||
		    (bb->ic_type == BB_RTL8852B) ||
		    (bb->ic_type == BB_RTL8851B)) {
			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used,
				    "Adv-Trigger(11k): {2} {0~2097ms} {Include CCA}\n");
			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used,
				    "Adv-Trigger(Manual): {3} {0~2097ms} {Inclu CCA} {th[0]} {th_ofst:1~12}\n");
		} else {
			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used,
				    "Adv-Trigger(11k): {2} {0~2097ms} {Include CCA} {nav_en} {rssi_th_en} {rssi_th}\n");
			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used,
				    "Adv-Trigger(Manual): {3} {0~2097ms} {Inclu CCA} {th[0]} {th_ofst:1~12} {nav_en} {rssi_th_en} {rssi_th}\n");
		}
	} else if (var[0] == 100) { /*Get NHM results*/
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "ccx_rpt_stamp=%d, ccx_period=%d\n",
			    env->ccx_rpt_stamp, env->ccx_period);

		if (halbb_nhm_get_result(bb)) {
			if ((bb->ic_type == BB_RTL8852A) ||
			    (bb->ic_type == BB_RTL8852B) ||
			    (bb->ic_type == BB_RTL8852C))
				end_i = NHM_RPT_NUM - 1;
			else
				end_i = NHM_RPT_NUM;
			for (i = 0; i < end_i; i++)
				BB_DBG_CNSL(*_out_len, *_used, output + *_used,
					    *_out_len - *_used,
					    "nhm_sw_result[%d] = %d (%d percent)\n",
					    i, env->nhm_sw_result[i],
					    env->nhm_rpt[i]);

			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used,
				    "nhm_cca_cnt = %d (%d percent)\n",
				    env->nhm_cca_cnt, env->nhm_cca_ratio);

			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used,
				    "nhm_tx_cnt = %d (%d percent)\n",
				    env->nhm_tx_cnt, env->nhm_tx_ratio);

			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used,
				    "nhm_idle_cnt = %d (%d percent)\n",
				    env->nhm_idle_cnt, env->nhm_idle_ratio);

			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used,
				    "nhm_ratio=%d, nhm_pwr=%d, nhm_pwr_0p5=%d.%d\n",
				    env->nhm_ratio, env->nhm_pwr,
				    env->nhm_pwr_0p5 >> 1,
				    5 * (env->nhm_pwr_0p5 & 0x1));
		} else {
			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used, "Get NHM result Fail\n");
		}

		if (env->nhm_app != NHM_FW_CTRL) {
			halbb_ccx_racing_release(bb, NHM_SEL);
			env->ccx_manual_ctrl = false;
		} else {
			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used,
				    "NHM is controlled by FW!\n");
		}
	} else { /*NMH trigger*/
		if (env->nhm_app != NHM_FW_CTRL) {
			env->ccx_manual_ctrl = true;
		} else {
			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used,
				    "NHM is controlled by FW!\n");
		}
		for (i = 1; i < 9; i++) {
			if (input[i + 1])
				HALBB_SCAN(input[i + 1], DCMD_DECIMAL, &var[i]);
		}

		if (var[0] == 1) {
			para.nhm_app = NHM_DBG_11K;
			para.mntr_time = 1900;
			para.nhm_incld_cca = NHM_EXCLUDE_CCA;
			if ((bb->ic_type != BB_RTL8852A) &&
			    (bb->ic_type != BB_RTL8852B) &&
			    (bb->ic_type != BB_RTL8851B)) {
				para.nhm_nav_en = NHM_NAV_EN_DISABLED;
				para.nhm_rssi_th_en = NHM_RSSI_TH_EN_DISABLED;
			}
		} else if (var[0] == 2) {
			para.nhm_app = NHM_DBG_11K;
			para.mntr_time = (u16)var[1];
			para.nhm_incld_cca = (enum nhm_opt_cca_all)var[2];
			if ((bb->ic_type != BB_RTL8852A) &&
			    (bb->ic_type != BB_RTL8852B) &&
			    (bb->ic_type != BB_RTL8851B)) {
				para.nhm_nav_en = (enum nhm_opt_nav_en)var[3];
				para.nhm_rssi_th_en = (enum nhm_opt_rssi_th_en)var[4];
				para.nhm_rssi_th = (u8)var[5];
			}
		} else if (var[0] == 3) {
			para.nhm_app = NHM_DBG_MANUAL;
			para.mntr_time = (u16)var[1];
			para.nhm_incld_cca = (enum nhm_opt_cca_all)var[2];
			para.nhm_manual_th0 = (u8)var[3];
			para.nhm_manual_th_ofst = (u8)var[4];
			if ((bb->ic_type != BB_RTL8852A) &&
			    (bb->ic_type != BB_RTL8852B) &&
			    (bb->ic_type != BB_RTL8851B)) {
				para.nhm_nav_en = (enum nhm_opt_nav_en)var[5];
				para.nhm_rssi_th_en = (enum nhm_opt_rssi_th_en)var[6];
				para.nhm_rssi_th = (u8)var[7];
			}

			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used,
				    "manual_th_ofst=%d, manaul_th0=%d\n",
				    para.nhm_manual_th_ofst,
				    para.nhm_manual_th0);
		}

		para.rac_lv = RAC_LV_4;

		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "app=%d, lv=%d, time=%d ms, inclu_cca=%d\n",
			    para.nhm_app, para.rac_lv, para.mntr_time,
			    para.nhm_incld_cca);

		if ((bb->ic_type != BB_RTL8852A) &&
		    (bb->ic_type != BB_RTL8852B) &&
		    (bb->ic_type != BB_RTL8851B))
			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used,
				    "nav_en=%d, rssi_th_en=%d, rssi_th=%d\n",
				    para.nhm_nav_en, para.nhm_rssi_th_en,
				    para.nhm_rssi_th);

		if (halbb_nhm_set(bb, &para) == HALBB_SET_SUCCESS) {
			halbb_ccx_trigger(bb, NHM_SEL);

			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used, "ccx_rpt_stamp=%d\n",
				    env->ccx_rpt_stamp);

			for (i = 0; i < NHM_TH_NUM; i++)
				BB_DBG_CNSL(*_out_len, *_used, output + *_used,
					    *_out_len - *_used,
					    "NHM_th[%d] RSSI = %d\n", i,
					    NHM_TH_2_RSSI(env->nhm_th[i]));
		} else {
			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used, "NHM mntr set fail!\n");
		}
	}
}

#endif /*#ifdef NHM_SUPPORT*/
#ifdef CLM_SUPPORT

void halbb_clm_get_utility(struct bb_info *bb)
{
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;

	env->clm_ratio = (u8)halbb_ccx_get_ratio(bb, env->clm_result, 100);
}

bool
halbb_clm_get_result(struct bb_info *bb)
{
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;
	struct bb_env_mntr_cr_info *cr = &env->bb_env_mntr_cr_i;

	if (!(halbb_get_reg_curr_phy(bb, cr->clm_rdy, cr->clm_rdy_m))) {
		BB_DBG(bb, DBG_ENV_MNTR, "Get CLM report Fail\n");
		return false;
	}

	env->clm_result = (u16)halbb_get_reg_curr_phy(bb, cr->clm_cnt,
						      cr->clm_cnt_m);

	BB_DBG(bb, DBG_ENV_MNTR, "CLM result = %d\n", env->clm_result);

	halbb_clm_get_utility(bb);

	return true;
}

bool halbb_clm_set(struct bb_info *bb, struct ccx_para_info *para)
{
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;
	struct bb_env_mntr_cr_info *cr = &env->bb_env_mntr_cr_i;
	enum channel_width bw = bb->hal_com->band[bb->bb_phy_idx].cur_chandef.bw;
	u16 mntr_time = 0;
	u32 period = 0;
	u32 unit_idx = 0;

	BB_DBG(bb, DBG_ENV_MNTR, "[%s]===>\n", __func__);

	if (para->mntr_time == 0) {
		BB_DBG(bb, DBG_ENV_MNTR, "[WARNING] MNTR_TIME is 0\n");
		return HALBB_SET_FAIL;
	}

	if (halbb_ccx_racing_ctrl(bb, para->rac_lv) == HALBB_SET_FAIL)
		return HALBB_SET_FAIL;

	BB_DBG(bb, DBG_ENV_MNTR, "mntr_time=%d ms, clm_input opt = %d\n",
	       para->mntr_time, para->clm_input_opt);

	if ((bb->ic_type != BB_RTL8852A) && (bb->ic_type != BB_RTL8852B) &&
	    (bb->ic_type != BB_RTL8851B))
		BB_DBG(bb, DBG_ENV_MNTR,
		       "clm_nav_en=%d, clm_rssi_th_en=%d, clm_rssi_th=%d\n",
		       para->clm_nav_en, para->clm_rssi_th_en,para->clm_rssi_th);

	/*Set unit & period*/
	if (bw == CHANNEL_WIDTH_10)
		mntr_time = para->mntr_time >> 1;
	else if (bw == CHANNEL_WIDTH_5)
		mntr_time = para->mntr_time >> 2;
	else
		mntr_time = para->mntr_time;

	if (mntr_time != env->clm_mntr_time) {
		halbb_ccx_ms_2_period_unit(bb, mntr_time, &period, &unit_idx);
		halbb_set_reg_curr_phy(bb, cr->clm_period, cr->clm_period_m,
				       period);
		halbb_set_reg_curr_phy(bb, cr->clm_unit_idx, cr->clm_unit_idx_m,
				       unit_idx);

		BB_DBG(bb, DBG_ENV_MNTR, "Update CLM time ((%d)) -> ((%d)), bw = %d\n",
		       env->clm_mntr_time, mntr_time, bw);

		env->clm_mntr_time = mntr_time;
		env->ccx_period = (u16)period;
		env->ccx_unit_idx = (u8)unit_idx;
	}

	/*Set input option*/
	if (para->clm_input_opt != env->clm_input_opt) {
		halbb_set_reg_curr_phy(bb, cr->clm_opt, cr->clm_opt_m,
				     para->clm_input_opt);

		BB_DBG(bb, DBG_ENV_MNTR,
		       "Update CLM input opt ((%d)) -> ((%d))\n",
		       env->clm_input_opt, para->clm_input_opt);

		env->clm_input_opt = para->clm_input_opt;
	}

	if ((bb->ic_type != BB_RTL8852A) && (bb->ic_type != BB_RTL8852B) &&
	    (bb->ic_type != BB_RTL8851B)) {
		/*Set nav_en*/
		if (para->clm_nav_en != env->clm_nav_en) {
			halbb_set_reg_curr_phy(bb, cr->clm_nav_en,
					       cr->clm_nav_en_m,
					       para->clm_nav_en);
			BB_DBG(bb, DBG_ENV_MNTR,
			       "Update CLM nav en ((%d)) -> ((%d))\n",
			       env->clm_nav_en, para->clm_nav_en);
			env->clm_nav_en = para->clm_nav_en;
		}

		/*Set rssi_th_en*/
		if (para->clm_rssi_th_en != env->clm_rssi_th_en) {
			halbb_set_reg_curr_phy(bb, cr->clm_rssi_th_en,
					       cr->clm_rssi_th_en_m,
					       para->clm_rssi_th_en);
			BB_DBG(bb, DBG_ENV_MNTR,
			       "Update CLM rssi_th en ((%d)) -> ((%d))\n",
			       env->clm_rssi_th_en, para->clm_rssi_th_en);
			env->clm_rssi_th_en = para->clm_rssi_th_en;
		}

		/*Set rssi_th*/
		if (para->clm_rssi_th != env->clm_rssi_th) {
			halbb_set_reg_curr_phy(bb, cr->clm_rssi_th,
					       cr->clm_rssi_th_m,
					       para->clm_rssi_th);
			BB_DBG(bb, DBG_ENV_MNTR,
			       "Update CLM rssi_th ((%d)) -> ((%d))\n",
			       env->clm_rssi_th, para->clm_rssi_th);
			env->clm_rssi_th = para->clm_rssi_th;
		}
	}

	env->clm_app = para->clm_app;

	return HALBB_SET_SUCCESS;
}

void halbb_clm_init(struct bb_info *bb)
{
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;
	struct bb_env_mntr_cr_info *cr = &env->bb_env_mntr_cr_i;

	BB_DBG(bb, DBG_ENV_MNTR, "[%s]===>\n", __func__);
	env->clm_app = CLM_INIT;
	if (bb->ic_type & BB_IC_AX_SERIES)
		env->clm_input_opt = CLM_CCA_INIT;
	else
		env->clm_input_opt = BE_CLM_CCA_INIT;

	if ((bb->ic_type != BB_RTL8852A) && (bb->ic_type != BB_RTL8852B) &&
	    (bb->ic_type != BB_RTL8851B)) {
		env->clm_nav_en = CLM_NAV_EN_INIT;
		env->clm_rssi_th_en = CLM_RSSI_TH_EN_INIT;
		env->clm_rssi_th = 0;
	}
	env->clm_mntr_time = 0;

	halbb_set_reg_curr_phy(bb, cr->clm_en, cr->clm_en_m, true);
}

void halbb_clm_set_dbg_sel(struct bb_info *bb, u8 dbg_sel)
{
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;
	struct bb_env_mntr_cr_info *cr = &env->bb_env_mntr_cr_i;

	/*r_clm_from_dbg_sel[5](0xa04[25]) is dummy*/
	halbb_set_reg_curr_phy(bb, cr->clm_dbg_sel, cr->clm_dbg_sel_m, dbg_sel);
}


void halbb_clm_dbg(struct bb_info *bb, char input[][16], u32 *_used,
		   char *output, u32 *_out_len)
{
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;
	struct ccx_para_info para = {0};
	char help[] = "-h";
	u32 var[10] = {0};
	u8 i = 0;

	for (i = 0; i < 7; i++) {
		if (input[i + 1])
			HALBB_SCAN(input[i + 1], DCMD_DECIMAL, &var[i]);
	}

	if ((_os_strcmp(input[1], help) == 0)) {
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "CLM Get Result: {100}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "CLM Basic-Trigger(1900ms): {1}\n");
		if ((bb->ic_type == BB_RTL8852A) ||
		    (bb->ic_type == BB_RTL8852B) ||
		    (bb->ic_type == BB_RTL8851B))
			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used,
				    "CLM Adv-Trigger: {2} {0~2097ms} {input}\n");
		else
			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used,
				    "CLM Adv-Trigger: {2} {0~2097ms} {input} {nav_en} {rssi_th_en} {rssi_th}\n");

		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "CLM set dbg_sel: {3} {bit:0~31}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "=============Notes=============>\n");
		if (bb->ic_type & BB_IC_AX_SERIES)
			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				*_out_len - *_used,
				"CLM input : 0(p20)/1(s20)/2(s40)/3(s80)/4(dbg)/5(txon_cca)/6(s20_s40_s80)/7(s20_s40_s80_p20)\n");
		else
			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				*_out_len - *_used,
				"CLM input : 0(p20)/1(s20)/2(s40)/3(s80)/4(s160)/5(dbg)/6(txon_cca)/7(s20_s40_s80_s160)/8(s20_s40_s80_s160_p20)\n");
	} else if (var[0] == 100) { /*Get CLM results */
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "ccx_rpt_stamp=%d, ccx_period=%d\n",
			    env->ccx_rpt_stamp, env->ccx_period);

		if (halbb_clm_get_result(bb)) {
			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used,
				    "clm_result = %d (%d percent)\n",
				    env->clm_result, env->clm_ratio);
		} else {
			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used, "Get CLM_rpt Fail\n");
		}

		halbb_ccx_racing_release(bb, CLM_SEL);
		env->ccx_manual_ctrl = false;
	} else if (var[0] == 3) { /* Set dbg_bit_sel */
		halbb_clm_set_dbg_sel(bb, (u8)var[1]);
	} else { /* Set & trigger CLM */
		env->ccx_manual_ctrl = true;

		if (var[0] == 1) {
			para.mntr_time = 1900;
			if (bb->ic_type & BB_IC_AX_SERIES)
				para.clm_input_opt = CLM_CCA_S80_S40_S20;
			else
				para.clm_input_opt = BE_CLM_CCA_S160_S80_S40_S20;

			if ((bb->ic_type != BB_RTL8852A) &&
			    (bb->ic_type != BB_RTL8852B) &&
			    (bb->ic_type != BB_RTL8851B)) {
				para.clm_nav_en = CLM_NAV_EN_DISABLED;
				para.clm_rssi_th_en = CLM_RSSI_TH_EN_DISABLED;
			}
		} else if (var[0] == 2) {
			para.mntr_time = (u16)var[1];
			if (bb->ic_type & BB_IC_AX_SERIES)
				para.clm_input_opt = (enum clm_opt_input)var[2];
			else
				para.clm_input_opt = (enum be_clm_opt_input)var[2];

			if ((bb->ic_type != BB_RTL8852A) &&
			    (bb->ic_type != BB_RTL8852B) &&
			    (bb->ic_type != BB_RTL8851B)) {
				para.clm_nav_en = (enum clm_opt_nav_en)var[3];
				para.clm_rssi_th_en = (enum clm_opt_rssi_th_en)var[4];
				para.clm_rssi_th = (u8)var[5];
			}
		}

		para.clm_app = CLM_DBG;
		para.rac_lv = RAC_LV_4;

		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "app=%d, lv=%d, time=%d ms, input_opt=%d\n",
			    para.clm_app, para.rac_lv, para.mntr_time,
			    para.clm_input_opt);

		if ((bb->ic_type != BB_RTL8852A) &&
		    (bb->ic_type != BB_RTL8852B) &&
		    (bb->ic_type != BB_RTL8851B))
			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used,
				    "nav_en=%d, rssi_th_en=%d, rssi_th=%d\n",
				    para.clm_nav_en, para.clm_rssi_th_en,
				    para.clm_rssi_th);

		if (halbb_clm_set(bb, &para) == HALBB_SET_SUCCESS) {
			halbb_ccx_trigger(bb, CLM_SEL);

			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used, "ccx_rpt_stamp=%d\n",
				    env->ccx_rpt_stamp);
		} else {
			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used, "CLM mntr set fail!\n");
		}
	}
}

#endif /*#ifdef CLM_SUPPORT*/

#ifdef IFS_CLM_SUPPORT

void halbb_ifs_clm_get_utility(struct bb_info *bb)
{
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;
	u8 i = 0;
	u32 numer = 0;
	u16 denom = 0;
	u8 ifs_clm_num = 0;

	if (bb->ic_type & BB_IC_AX_SERIES)
		ifs_clm_num = IFS_CLM_NUM;
	else
		ifs_clm_num = BE_IFS_CLM_NUM;

	env->ifs_clm_tx_ratio = (u8)halbb_ccx_get_ratio(bb, env->ifs_clm_tx,
							100);
	env->ifs_clm_edcca_excl_cca_ratio = (u8)halbb_ccx_get_ratio(bb,
					    env->ifs_clm_edcca_excl_cca, 100);
	env->ifs_clm_cck_fa_ratio = (u8)halbb_ccx_get_ratio(bb,
				    env->ifs_clm_cckfa, 100);
	env->ifs_clm_ofdm_fa_ratio = (u8)halbb_ccx_get_ratio(bb,
				     env->ifs_clm_ofdmfa, 100);
	env->ifs_clm_cck_cca_excl_fa_ratio = (u8)halbb_ccx_get_ratio(bb,
					     env->ifs_clm_cckcca_excl_fa, 100);
	env->ifs_clm_ofdm_cca_excl_fa_ratio = (u8)halbb_ccx_get_ratio(bb,
					      env->ifs_clm_ofdmcca_excl_fa,
					      100);
	env->ifs_clm_cck_fa_permil = halbb_ccx_get_ratio(bb, env->ifs_clm_cckfa,
							 1000);
	env->ifs_clm_ofdm_fa_permil = halbb_ccx_get_ratio(bb,
				      env->ifs_clm_ofdmfa, 1000);

	for (i = 0; i < ifs_clm_num; i++) {
		if ((env->ifs_clm_his[i] > 127) && (bb->ic_type == BB_RTL8852A))
			env->ifs_clm_ifs_avg[i] = ENV_MNTR_FAIL_DWORD;
		else
			env->ifs_clm_ifs_avg[i] = halbb_ccx_idx_cnt_2_us(bb,
						  env->ifs_clm_avg[i]);

		numer = halbb_ccx_idx_cnt_2_us(bb, env->ifs_clm_cca[i]) +
					       (env->ifs_clm_his[i] >> 1);
		denom = env->ifs_clm_his[i];
		env->ifs_clm_cca_avg[i] = HALBB_DIV(numer, denom);
	}

	BB_DBG(bb, DBG_ENV_MNTR,
	       "IFS-CLM ratio {Tx, EDCCA_exclu_cca} = {%d, %d}\n",
	       env->ifs_clm_tx_ratio, env->ifs_clm_edcca_excl_cca_ratio);
	BB_DBG(bb, DBG_ENV_MNTR,
	       "IFS-CLM FA ratio {CCK, OFDM} = {%d, %d}\n",
	       env->ifs_clm_cck_fa_ratio, env->ifs_clm_ofdm_fa_ratio);
	BB_DBG(bb, DBG_ENV_MNTR,
	       "IFS-CLM FA permil {CCK, OFDM} = {%d, %d}\n",
	       env->ifs_clm_cck_fa_permil, env->ifs_clm_ofdm_fa_permil);
	BB_DBG(bb, DBG_ENV_MNTR,
	       "IFS-CLM CCA_exclu_FA ratio {CCK, OFDM} = {%d, %d}\n",
	       env->ifs_clm_cck_cca_excl_fa_ratio,
	       env->ifs_clm_ofdm_cca_excl_fa_ratio);

	BB_DBG(bb, DBG_ENV_MNTR, "Time:[his, ifs_avg(us), cca_avg(us)]\n");
	for (i = 0; i < ifs_clm_num; i++)
		BB_DBG(bb, DBG_ENV_MNTR, "T%d:[%d, %d, %d]\n", i + 1,
		       env->ifs_clm_his[i], env->ifs_clm_ifs_avg[i],
		       env->ifs_clm_cca_avg[i]);
}

bool
halbb_ifs_clm_get_result(struct bb_info *bb)
{
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;
	struct bb_env_mntr_cr_info *cr = &env->bb_env_mntr_cr_i;
	u8 i = 0;
	u8 ifs_clm_num = 0;

	if (bb->ic_type & BB_IC_AX_SERIES)
		ifs_clm_num = IFS_CLM_NUM;
	else
		ifs_clm_num = BE_IFS_CLM_NUM;

	BB_DBG(bb, DBG_ENV_MNTR, "[%s]===>\n", __func__);

	if (!(halbb_get_reg_curr_phy(bb, cr->ifs_clm_rdy, cr->ifs_clm_rdy_m))) {
		BB_DBG(bb, DBG_ENV_MNTR, "Get IFS_CLM report Fail\n");
		return false;
	}

	/*CLM result*/
	env->ifs_clm_tx = (u16)halbb_get_reg_curr_phy(bb, cr->ifs_clm_tx_cnt,
						      cr->ifs_clm_tx_cnt_m);
	env->ifs_clm_edcca_excl_cca = (u16)halbb_get_reg_curr_phy(bb,
								  cr->ifs_clm_edcca_exclu_cca,
								  cr->ifs_clm_edcca_exclu_cca_m);
	env->ifs_clm_cckcca_excl_fa = (u16)halbb_get_reg_curr_phy(bb,
								  cr->ifs_clm_cckcca_exclu_fa,
								  cr->ifs_clm_cckcca_exclu_fa_m);
	env->ifs_clm_ofdmcca_excl_fa = (u16)halbb_get_reg_curr_phy(bb,
								   cr->ifs_clm_ofdmcca_exclu_fa,
								   cr->ifs_clm_ofdmcca_exclu_fa_m);
	env->ifs_clm_cckfa = (u16)halbb_get_reg_curr_phy(bb, cr->ifs_clm_cck_fa,
							 cr->ifs_clm_cck_fa_m);
	env->ifs_clm_ofdmfa = (u16)halbb_get_reg_curr_phy(bb,
							  cr->ifs_clm_ofdm_fa,
							  cr->ifs_clm_ofdm_fa_m);

	/* IFS result */
	env->ifs_clm_his[0] = (u16)halbb_get_reg_curr_phy(bb,
							  cr->ifs_clm_t1_his,
							  cr->ifs_clm_t1_his_m);
	env->ifs_clm_his[1] = (u16)halbb_get_reg_curr_phy(bb,
							  cr->ifs_clm_t2_his,
							  cr->ifs_clm_t2_his_m);
	env->ifs_clm_his[2] = (u16)halbb_get_reg_curr_phy(bb,
							  cr->ifs_clm_t3_his,
							  cr->ifs_clm_t3_his_m);
	env->ifs_clm_his[3] = (u16)halbb_get_reg_curr_phy(bb,
							  cr->ifs_clm_t4_his,
							  cr->ifs_clm_t4_his_m);

	env->ifs_clm_avg[0] = (u16)halbb_get_reg_curr_phy(bb,
							  cr->ifs_clm_t1_avg,
							  cr->ifs_clm_t1_avg_m);
	env->ifs_clm_avg[1] = (u16)halbb_get_reg_curr_phy(bb,
							  cr->ifs_clm_t2_avg,
							  cr->ifs_clm_t2_avg_m);
	env->ifs_clm_avg[2] = (u16)halbb_get_reg_curr_phy(bb,
							  cr->ifs_clm_t3_avg,
							  cr->ifs_clm_t3_avg_m);
	env->ifs_clm_avg[3] = (u16)halbb_get_reg_curr_phy(bb,
							  cr->ifs_clm_t4_avg,
							  cr->ifs_clm_t4_avg_m);

	env->ifs_clm_cca[0] = (u16)halbb_get_reg_curr_phy(bb,
							  cr->ifs_clm_t1_cca,
							  cr->ifs_clm_t1_cca_m);
	env->ifs_clm_cca[1] = (u16)halbb_get_reg_curr_phy(bb,
							  cr->ifs_clm_t2_cca,
							  cr->ifs_clm_t2_cca_m);
	env->ifs_clm_cca[2] = (u16)halbb_get_reg_curr_phy(bb,
							  cr->ifs_clm_t3_cca,
							  cr->ifs_clm_t3_cca_m);
	env->ifs_clm_cca[3] = (u16)halbb_get_reg_curr_phy(bb,
							  cr->ifs_clm_t4_cca,
							  cr->ifs_clm_t4_cca_m);
	env->ifs_clm_total_ifs = (u16)halbb_get_reg_curr_phy(bb,
							     cr->ifs_total_cnt,
							     cr->ifs_total_cnt_m);

	if (bb->ic_type & BB_IC_BE_SERIES) {
		env->ifs_clm_his[4] = (u16)halbb_get_reg_curr_phy(bb,
								  cr->ifs_clm_t5_his,
								  cr->ifs_clm_t5_his_m);
		env->ifs_clm_avg[4] = (u16)halbb_get_reg_curr_phy(bb,
								  cr->ifs_clm_t5_avg,
								  cr->ifs_clm_t5_avg_m);
		env->ifs_clm_cca[4] = (u16)halbb_get_reg_curr_phy(bb,
								  cr->ifs_clm_t5_cca,
								  cr->ifs_clm_t5_cca_m);
	}

	/*Print Result*/
	BB_DBG(bb, DBG_ENV_MNTR, "IFS-CLM total_ifs = %d\n",
	       env->ifs_clm_total_ifs);
	BB_DBG(bb, DBG_ENV_MNTR, "IFS-CLM {Tx, EDCCA_exclu_cca} = {%d, %d}\n",
	       env->ifs_clm_tx, env->ifs_clm_edcca_excl_cca);
	BB_DBG(bb, DBG_ENV_MNTR, "IFS-CLM FA {CCK, OFDM} = {%d, %d}\n",
	       env->ifs_clm_cckfa, env->ifs_clm_ofdmfa);
	BB_DBG(bb, DBG_ENV_MNTR,
	       "IFS-CLM CCA_exclu_FA {CCK, OFDM} = {%d, %d}\n",
	       env->ifs_clm_cckcca_excl_fa, env->ifs_clm_ofdmcca_excl_fa);

	BB_DBG(bb, DBG_ENV_MNTR, "Time:[his, avg, cca]\n");
	for (i = 0; i < ifs_clm_num; i++)
		BB_DBG(bb, DBG_ENV_MNTR, "T%d:[%d, %d, %d]\n", i + 1,
		       env->ifs_clm_his[i], env->ifs_clm_avg[i],
		       env->ifs_clm_cca[i]);

	halbb_ifs_clm_get_utility(bb);

	return true;
}

void halbb_ifs_clm_set_th_reg(struct bb_info *bb)
{
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;
	struct bb_env_mntr_cr_info *cr = &env->bb_env_mntr_cr_i;
	u8 i = 0;
	u8 ifs_clm_num = 0;

	if (bb->ic_type & BB_IC_AX_SERIES)
		ifs_clm_num = IFS_CLM_NUM;
	else
		ifs_clm_num = BE_IFS_CLM_NUM;

	BB_DBG(bb, DBG_ENV_MNTR, "[%s]===>\n", __func__);
	/*Set IFS for en/th_low/th_high T1~T4*/
	halbb_set_reg_curr_phy(bb, cr->ifs_t1_th_l, cr->ifs_t1_th_l_m,
			       env->ifs_clm_th_l[0]);
	halbb_set_reg_curr_phy(bb, cr->ifs_t2_th_l, cr->ifs_t2_th_l_m,
			       env->ifs_clm_th_l[1]);
	halbb_set_reg_curr_phy(bb, cr->ifs_t3_th_l, cr->ifs_t3_th_l_m,
			       env->ifs_clm_th_l[2]);
	halbb_set_reg_curr_phy(bb, cr->ifs_t4_th_l, cr->ifs_t4_th_l_m,
			       env->ifs_clm_th_l[3]);

	halbb_set_reg_curr_phy(bb, cr->ifs_t1_th_h, cr->ifs_t1_th_h_m,
			       env->ifs_clm_th_h[0]);
	halbb_set_reg_curr_phy(bb, cr->ifs_t2_th_h, cr->ifs_t2_th_h_m,
			       env->ifs_clm_th_h[1]);
	halbb_set_reg_curr_phy(bb, cr->ifs_t3_th_h, cr->ifs_t3_th_h_m,
			       env->ifs_clm_th_h[2]);
	halbb_set_reg_curr_phy(bb, cr->ifs_t4_th_h, cr->ifs_t4_th_h_m,
			       env->ifs_clm_th_h[3]);

	/*Set IFS for en/th_low/th_high T5*/
	if (bb->ic_type & BB_IC_BE_SERIES) {
		halbb_set_reg_curr_phy(bb, cr->ifs_t5_th_l, cr->ifs_t5_th_l_m,
				       env->ifs_clm_th_l[4]);
		halbb_set_reg_curr_phy(bb, cr->ifs_t5_th_h, cr->ifs_t5_th_h_m,
				       env->ifs_clm_th_h[4]);
	}

	for (i = 0; i < ifs_clm_num; i++)
		BB_DBG(bb, DBG_ENV_MNTR,
		       "Update IFS_T%d_th{low, high} : {%d, %d}\n",
		       i + 1, env->ifs_clm_th_l[i], env->ifs_clm_th_h[i]);
}

bool halbb_ifs_clm_th_update_chk(struct bb_info *bb, struct ccx_para_info *para,
				 u16 *ifs_th_l, u16 *ifs_th_h)
{
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;
	bool is_app_change = (env->ifs_clm_app == para->ifs_clm_app) ? false : true;
	bool is_update = false;
	u8 i = 0;
	u32 ifs_th0_us = 0;
	u32 ifs_th_times = 0;
	u32 ifs_th_h_us[BE_IFS_CLM_NUM] = {0};
	u8 ifs_clm_num = 0;

	if (bb->ic_type & BB_IC_AX_SERIES)
		ifs_clm_num = IFS_CLM_NUM;
	else
		ifs_clm_num = BE_IFS_CLM_NUM;

	BB_DBG(bb, DBG_ENV_MNTR, "ifs_clm_App=%d\n", para->ifs_clm_app);

	if (!is_app_change)
		goto CHK_IFS_UPDATE_FINISHED;

	is_update = true;

	switch (para->ifs_clm_app) {
	case IFS_CLM_INIT:
	case IFS_CLM_BACKGROUND:
	case IFS_CLM_ACS:
	case IFS_CLM_DBG:
		/*ifs_th_h_us = {16384    , 4096     , 1024    , 256    , 64}*/
		/*ifs_th_l_us = {4096+unit, 1024+unit, 256+unit, 64+unit, 0}*/
		ifs_th0_us = 64;
		ifs_th_times = 4;

		break;
	case IFS_CLM_DBG_MANUAL:
		/*ifs_th_h_us[0] = ifs_th0_us*/
		/*ifs_th_h_us[i] = ifs_th_h_us[i - 1] * ifs_th_times, i = 1 ~ 3*/
		/*ifs_th_l_us[0] = 0*/
		/*ifs_th_l_us[i] = ifs_th_h_us[i - 1] + unit, i = 1 ~ 3*/
		ifs_th0_us = para->ifs_clm_manual_th0;
		ifs_th_times = para->ifs_clm_manual_th_times;

		break;
	default:
		break;
	}

	for (i = 0; i < ifs_clm_num; i++) {
		ifs_th_l[i] = (i == 0) ? 0 : ifs_th_h[i - 1] + 1;
		ifs_th_h_us[i] = (i == 0) ? ifs_th0_us : ifs_th_h_us[i - 1] *
				 ifs_th_times;
		ifs_th_h[i] = halbb_ccx_us_2_idx_cnt(bb, ifs_th_h_us[i]);
	}

CHK_IFS_UPDATE_FINISHED:
	if (!is_update)
		BB_DBG(bb, DBG_ENV_MNTR, "No need to update IFS_TH\n");

	return is_update;
}

bool halbb_ifs_clm_set(struct bb_info *bb, struct ccx_para_info *para)
{
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;
	struct bb_env_mntr_cr_info *cr = &env->bb_env_mntr_cr_i;
	enum channel_width bw = bb->hal_com->band[bb->bb_phy_idx].cur_chandef.bw;
	u16 mntr_time = 0;
	u32 period = 0;
	u32 unit_idx = 0;
	u16 ccx_mntr_time = 0;
	u16 ifs_th_l[BE_IFS_CLM_NUM] = {0};
	u16 ifs_th_h[BE_IFS_CLM_NUM] = {0};
	u8 i = 0;
	u8 ifs_clm_num = 0;

	if (bb->ic_type & BB_IC_AX_SERIES)
		ifs_clm_num = IFS_CLM_NUM;
	else
		ifs_clm_num = BE_IFS_CLM_NUM;

	BB_DBG(bb, DBG_ENV_MNTR, "[%s]===>\n", __func__);

	if (para->mntr_time == 0) {
		BB_DBG(bb, DBG_ENV_MNTR, "[WARNING] MNTR_TIME is 0\n");
		return HALBB_SET_FAIL;
	}

	if (halbb_ccx_racing_ctrl(bb, para->rac_lv) == HALBB_SET_FAIL)
		return HALBB_SET_FAIL;

	BB_DBG(bb, DBG_ENV_MNTR, "mntr_time=%d ms\n", para->mntr_time);

	/*Set unit & period*/
	if (bw == CHANNEL_WIDTH_10)
		mntr_time = para->mntr_time >> 1;
	else if (bw == CHANNEL_WIDTH_5)
		mntr_time = para->mntr_time >> 2;
	else
		mntr_time = para->mntr_time;

	if (mntr_time != env->ifs_clm_mntr_time) {
		halbb_ccx_ms_2_period_unit(bb, mntr_time, &period, &unit_idx);
		halbb_set_reg_curr_phy(bb, cr->ifs_clm_period,
				       cr->ifs_clm_period_m, period);
		halbb_set_reg_curr_phy(bb, cr->ifs_clm_unit_idx,
				       cr->ifs_clm_unit_idx_m, unit_idx);

		BB_DBG(bb, DBG_ENV_MNTR,
		       "Update IFS-CLM time ((%d)) -> ((%d)), bw = %d\n",
		       env->ifs_clm_mntr_time, mntr_time, bw);

		env->ifs_clm_mntr_time = mntr_time;
		env->ccx_period = (u16)period;
		env->ccx_unit_idx = (u8)unit_idx;
	}

	/*Set IFS CLM threshold*/
	if (halbb_ifs_clm_th_update_chk(bb, para, &ifs_th_l[0], &ifs_th_h[0])) {
		env->ifs_clm_app = para->ifs_clm_app;
		for (i = 0; i < ifs_clm_num; i++) {
			env->ifs_clm_th_l[i] = ifs_th_l[i];
			env->ifs_clm_th_h[i] = ifs_th_h[i];
		}

		halbb_ifs_clm_set_th_reg(bb);
	}

	return HALBB_SET_SUCCESS;
}

void halbb_ifs_clm_init(struct bb_info *bb)
{
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;
	struct bb_env_mntr_cr_info *cr = &env->bb_env_mntr_cr_i;

	BB_DBG(bb, DBG_ENV_MNTR, "[%s]===>\n", __func__);

	env->ifs_clm_app = IFS_CLM_INIT;
	env->ifs_clm_mntr_time = 0;

	/*if r_IFS_collect_en = 0, ifs_clm ready bit will always be 0.*/
	halbb_set_reg_curr_phy(bb, cr->ifs_clm_en, cr->ifs_clm_en_m, true);

	/*Enable IFS cnt*/
	halbb_set_reg_curr_phy(bb, cr->ifs_t1_en, cr->ifs_t1_en_m, true);
	halbb_set_reg_curr_phy(bb, cr->ifs_t2_en, cr->ifs_t2_en_m, true);
	halbb_set_reg_curr_phy(bb, cr->ifs_t3_en, cr->ifs_t3_en_m, true);
	halbb_set_reg_curr_phy(bb, cr->ifs_t4_en, cr->ifs_t4_en_m, true);

	if (bb->ic_type & BB_IC_BE_SERIES) {
		halbb_set_reg_curr_phy(bb, cr->ifs_t5_en, cr->ifs_t5_en_m,
				       true);
		/*enable cckcca/ofdmcca*/
		halbb_set_reg_curr_phy(bb, cr->ifs_clm_cca_opt,
				       cr->ifs_clm_cca_opt_m, 0x3);
	}
}

void halbb_ifs_clm_dbg(struct bb_info *bb, char input[][16], u32 *_used,
		       char *output, u32 *_out_len)
{
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;
	struct ccx_para_info para = {0};
	char help[] = "-h";
	u32 var[10] = {0};
	u8 i = 0;
	u8 ifs_clm_num = 0;

	if (bb->ic_type & BB_IC_AX_SERIES)
		ifs_clm_num = IFS_CLM_NUM;
	else
		ifs_clm_num = BE_IFS_CLM_NUM;

	for (i = 0; i < 5; i++) {
		if (input[i + 1])
			HALBB_SCAN(input[i + 1], DCMD_DECIMAL, &var[i]);
	}

	if ((_os_strcmp(input[1], help) == 0)) {
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "IFS-CLM Get Result: {100}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "IFS-CLM Basic-Trigger 1900ms: {1}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "IFS-CLM Adv-Trigger: {2} {0~2097ms}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "IFS-CLM Adv-Trigger(manual): {3} {0~2097ms} {ifs_th0(us)} {ifs_th_times}\n");
	} else if (var[0] == 100) { /*Get IFS_CLM results*/
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "ccx_rpt_stamp=%d, ccx_period=%d\n",
			    env->ccx_rpt_stamp, env->ccx_period);

		if (halbb_ifs_clm_get_result(bb)) {
			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used,
				    "IFS_CLM Tx cnt = %d (%d percent)\n",
				    env->ifs_clm_tx, env->ifs_clm_tx_ratio);

			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used,
				    "IFS_CLM EDCCA_excl_cca cnt = %d (%d percent)\n",
				    env->ifs_clm_edcca_excl_cca,
				    env->ifs_clm_edcca_excl_cca_ratio);

			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used,
				    "IFS_CLM CCK FA cnt = %d (%d percent/%d permil)\n",
				    env->ifs_clm_cckfa,
				    env->ifs_clm_cck_fa_ratio,
				    env->ifs_clm_cck_fa_permil);

			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used,
				    "IFS_CLM OFDM FA cnt = %d (%d percent/%d permil)\n",
				    env->ifs_clm_ofdmfa,
				    env->ifs_clm_ofdm_fa_ratio,
				    env->ifs_clm_ofdm_fa_permil);

			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used,
				    "IFS_CLM CCK CCA_excl_fa cnt = %d (%d percent)\n",
				    env->ifs_clm_cckcca_excl_fa,
				    env->ifs_clm_cck_cca_excl_fa_ratio);

			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used,
				    "IFS_CLM OFDM CCA_excl_fa cnt = %d (%d percent)\n",
				    env->ifs_clm_ofdmcca_excl_fa,
				    env->ifs_clm_ofdm_cca_excl_fa_ratio);

			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used, "IFS_total cnt = %d\n",
				    env->ifs_clm_total_ifs);

			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used,
				    "Time(us):[his, ifs_avg(us), cca_avg(us)]\n");
			for (i = 0; i < ifs_clm_num; i++)
				BB_DBG_CNSL(*_out_len, *_used, output + *_used,
					    *_out_len - *_used,
					    "T%d(%d ~ %d):[%d, %d, %d]\n",
					    i + 1, halbb_ccx_idx_cnt_2_us(bb,
					    env->ifs_clm_th_l[i]),
					    halbb_ccx_idx_cnt_2_us(bb,
					    env->ifs_clm_th_h[i]),
					    env->ifs_clm_his[i],
					    env->ifs_clm_ifs_avg[i],
					    env->ifs_clm_cca_avg[i]);
		} else {
			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used, "Get ICS-CLM_rpt Fail\n");
		}

		halbb_ccx_racing_release(bb, IFS_CLM_SEL);
		env->ccx_manual_ctrl = false;
	} else { /*IFS_CLM trigger*/
		env->ccx_manual_ctrl = true;

		if (var[0] == 1) {
			para.ifs_clm_app = IFS_CLM_DBG;
			para.mntr_time = 1900;
			para.ifs_clm_manual_th0 = 0;
			para.ifs_clm_manual_th_times = 0;
		} else if (var[0] == 2) {
			para.ifs_clm_app = IFS_CLM_DBG;
			para.mntr_time = (u16)var[1];
			para.ifs_clm_manual_th0 = 0;
			para.ifs_clm_manual_th_times = 0;
		} else {
			para.ifs_clm_app = IFS_CLM_DBG_MANUAL;
			para.mntr_time = (u16)var[1];
			para.ifs_clm_manual_th0 = (u32)var[2];
			para.ifs_clm_manual_th_times = (u32)var[3];
		}

		para.rac_lv = RAC_LV_4;

		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "app=%d, lv=%d, time=%d ms, manual_th0=%d(us), manual_th_times=%d\n",
			    para.ifs_clm_app, para.rac_lv, para.mntr_time,
			    para.ifs_clm_manual_th0,
			    para.ifs_clm_manual_th_times);

		if (halbb_ifs_clm_set(bb, &para) == HALBB_SET_SUCCESS) {
			halbb_ccx_trigger(bb, IFS_CLM_SEL);

			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used, "ccx_rpt_stamp=%d\n",
				    env->ccx_rpt_stamp);

			for (i = 0; i < ifs_clm_num; i++)
				BB_DBG_CNSL(*_out_len, *_used, output + *_used,
					    *_out_len - *_used,
					    "IFS_T%d_th(us){low, high} : {%d, %d}\n",
					    i + 1,halbb_ccx_idx_cnt_2_us(bb,
					    env->ifs_clm_th_l[i]),
					    halbb_ccx_idx_cnt_2_us(bb,
					    env->ifs_clm_th_h[i]));
		} else {
			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used, "IFS_CLM mntr set fail!\n");
		}
	}
}
#endif

#ifdef FAHM_SUPPORT
u16 halbb_fahm_get_ratio(struct bb_info *bb, u16 rpt, u16 score)
{
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;
	u32 numer = 0;
	u16 ratio = 0;
	u16 ret = 0;

	if (rpt == env->fahm_period) {
		ret = score;
	} else {
		numer = rpt * score + (env->fahm_period >> 1);
		ratio = (u16)HALBB_DIV(numer, env->fahm_period);
		ret = (ratio == score) ? (score - 1) : ratio;
	}

	return ret;
}

void halbb_fahm_racing_release(struct bb_info *bb)
{
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;

	BB_DBG(bb, DBG_ENV_MNTR, "[%s]===>\n", __func__);
	BB_DBG(bb, DBG_ENV_MNTR, "lv:(%d)->(0)\n", env->fahm_rac_lv);

	env->fahm_ongoing = false;
	env->fahm_rac_lv = RAC_RELEASE;
	env->fahm_app = FAHM_INIT;
}

u8 halbb_fahm_racing_ctrl(struct bb_info *bb, enum halbb_racing_lv rac_lv)
{
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;
	u8 set_result = HALBB_SET_SUCCESS;

	if (rac_lv >= RAC_MAX_NUM) {
		BB_DBG(bb, DBG_ENV_MNTR, "[WARNING] Wrong LV=%d\n", rac_lv);
		return HALBB_SET_FAIL;
	}

	BB_DBG(bb, DBG_ENV_MNTR, "fahm_ongoing=%d, lv:(%d)->(%d)\n",
	       env->fahm_ongoing, env->fahm_rac_lv, rac_lv);

	if (env->fahm_ongoing) {
		if (rac_lv <= env->fahm_rac_lv)
			set_result = HALBB_SET_FAIL;
		else
			env->fahm_ongoing = false;
	}

	if (set_result)
		env->fahm_rac_lv = rac_lv;

	BB_DBG(bb, DBG_ENV_MNTR, "fahm racing success=%d\n", set_result);
	return set_result;
}

void halbb_fahm_hw_trigger(struct bb_info *bb)
{
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;
	struct bb_env_mntr_cr_info *cr = &env->bb_env_mntr_cr_i;

	BB_DBG(bb, DBG_ENV_MNTR, "[%s]===>\n", __func__);

	halbb_set_reg_curr_phy(bb, cr->fahm_en, cr->fahm_en_m, false);
	halbb_set_reg_curr_phy(bb, cr->fahm_en, cr->fahm_en_m, true);

	env->fahm_trigger_time = bb->bb_sys_up_time;
	env->fahm_rpt_stamp++;
	env->fahm_ongoing = true;
}

void halbb_fahm_cal_wgt(struct bb_info *bb)
{
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;
	u8 i = 0;

	for (i = 0; i < FAHM_RPT_NUM; i++) {
		if (i == 0)
			env->fahm_wgt[i] = (u8)(MAX_2(env->fahm_th[i] - 2, 0));
		else if (i == (FAHM_RPT_NUM - 1))
			env->fahm_wgt[i] = (u8)(env->fahm_th[i - 1] + 2);
		else
			env->fahm_wgt[i] = (u8)((env->fahm_th[i - 1] +
					   env->fahm_th[i]) >> 1);
	}

	if ((bb->ic_type == BB_RTL8852A) || (bb->ic_type == BB_RTL8852B) ||
	    (bb->ic_type == BB_RTL8852C)) {
		if (env->fahm_th[FAHM_TH_NUM - 1] == RSSI_2_FAHM_TH(FAHM_WA_TH)) {
			env->fahm_wgt[FAHM_RPT_NUM - 1] = (u8)(env->fahm_th[FAHM_TH_NUM - 2] + 2);
			env->fahm_wgt[FAHM_RPT_NUM - 2] = env->fahm_wgt[FAHM_RPT_NUM - 1];
		}
	}

	BB_DBG(bb, DBG_ENV_MNTR,
	       "Update FAHM_wgt(H->L)[%d %d %d %d %d %d %d %d %d %d %d %d]\n",
	       env->fahm_wgt[11], env->fahm_wgt[10], env->fahm_wgt[9],
	       env->fahm_wgt[8], env->fahm_wgt[7], env->fahm_wgt[6],
	       env->fahm_wgt[5], env->fahm_wgt[4], env->fahm_wgt[3],
	       env->fahm_wgt[2], env->fahm_wgt[1], env->fahm_wgt[0]);
}

u8 halbb_fahm_cal_wgt_avg(struct bb_info *bb, u8 start_i, u8 end_i, u16 n_sum,
		        u8 frac_bit_num)
{
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;
	u8 i = 0;
	u32 tmp = 0;
	u8 wgt_avg = 0;
	u8 fahm_valid = 0;

	BB_DBG(bb, DBG_ENV_MNTR, "[%s]===>\n", __func__);

	if (n_sum == 0) {
		BB_DBG(bb, DBG_ENV_MNTR,
			  "fahm_rpt_sum = 0, don't need to update noise\n");
		return 0;
	} else if (end_i > FAHM_RPT_NUM - 1) {
		BB_DBG(bb, DBG_ENV_MNTR,
			  "[WARNING]fahm_rpt_end_idx is larger than 11!!\n");
		return 0;
	}

	for (i = start_i; i <= end_i; i++)
		tmp += env->fahm_sw_result[i] * env->fahm_wgt[i];

	if (frac_bit_num == 1) /*u(8.1)*/
		wgt_avg = (u8)HALBB_DIV(tmp, n_sum);
	else /*u(8.0)*/
		wgt_avg = (u8)(FAHM_TH_2_RSSI(HALBB_DIV(tmp, n_sum)));

	fahm_valid = (u8)halbb_fahm_get_ratio(bb, n_sum, 100);
	BB_DBG(bb, DBG_ENV_MNTR,
	       "valid: ((%d)) percent, wgt_avg(RSSI)=((%d))\n",
	       fahm_valid, wgt_avg);

	return wgt_avg;
}


void halbb_fahm_get_utility(struct bb_info *bb)
{
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;
	u8 i = 0;

	halbb_fahm_cal_wgt(bb);

	for (i = 0; i < FAHM_RPT_NUM; i++) {
		if (((bb->ic_type == BB_RTL8852A) ||
		    (bb->ic_type == BB_RTL8852B) ||
		    (bb->ic_type == BB_RTL8852C)) &&
		    (i == (FAHM_RPT_NUM - 1))) {
			env->fahm_sw_result[0] += env->fahm_result[i];
			env->fahm_sw_result[i] = 0;
		} else {
			env->fahm_sw_result[i] = env->fahm_result[i];
		}
	}

	BB_DBG(bb, DBG_ENV_MNTR,
	       "FAHM sw result[%d](H->L)[%d %d %d %d %d %d %d %d %d %d %d %d]\n",
	       env->fahm_rpt_stamp, env->fahm_sw_result[11],
	       env->fahm_sw_result[10], env->fahm_sw_result[9],
	       env->fahm_sw_result[8], env->fahm_sw_result[7],
	       env->fahm_sw_result[6], env->fahm_sw_result[5],
	       env->fahm_sw_result[4], env->fahm_sw_result[3],
	       env->fahm_sw_result[2], env->fahm_sw_result[1],
	       env->fahm_sw_result[0]);

	env->fahm_ratio = (u8)halbb_fahm_get_ratio(bb, env->fahm_result_sum,
			 			   100);
	env->fahm_denom_ratio = (u8)halbb_fahm_get_ratio(bb,
							 env->fahm_denom_result,
							 100);
	env->fahm_permil = (u16)halbb_fahm_get_ratio(bb, env->fahm_result_sum,
						     1000);
	env->fahm_denom_permil = (u16)halbb_fahm_get_ratio(bb,
							   env->fahm_denom_result,
							   1000);
	env->fahm_pwr = halbb_fahm_cal_wgt_avg(bb, 0, FAHM_RPT_NUM - 1,
					       env->fahm_result_sum, 0);
	env->fahm_pwr_0p5 = halbb_fahm_cal_wgt_avg(bb, 0, FAHM_RPT_NUM - 1,
						   env->fahm_result_sum, 1);

	for (i = 0; i < FAHM_RPT_NUM; i++)
		env->fahm_rpt[i] = (u8)halbb_fahm_get_ratio(bb,
				   env->fahm_sw_result[i], 100);

	BB_DBG(bb, DBG_ENV_MNTR, "fahm_pwr=%d, fahm_pwr_0p5=%d.%d\n",
	       env->fahm_pwr, env->fahm_pwr_0p5 >> 1, 5 * (env->fahm_pwr_0p5 &
							   0x1));
	BB_DBG(bb, DBG_ENV_MNTR, "fahm_ratio=%d, fahm_permil=%d\n",
	       env->fahm_ratio, env->fahm_permil);
	BB_DBG(bb, DBG_ENV_MNTR, "fahm_denom_ratio=%d, fahm_denom_permil=%d\n",
	       env->fahm_denom_ratio, env->fahm_denom_permil);
}

bool halbb_fahm_get_result(struct bb_info *bb)
{
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;
	struct bb_env_mntr_cr_info *cr = &env->bb_env_mntr_cr_i;
	u8 i = 0;
	u32 result_sum_tmp = 0;

	BB_DBG(bb, DBG_ENV_MNTR, "[%s]===>\n", __func__);

	if (!(halbb_get_reg_curr_phy(bb, cr->fahm_rdy, cr->fahm_rdy_m))) {
		BB_DBG(bb, DBG_ENV_MNTR, "Get FAHM report Fail\n");
		return false;
	}

	env->fahm_result[0] = (u16)halbb_get_reg_curr_phy(bb, cr->fahm_cnt0,
							  cr->fahm_cnt0_m);
	env->fahm_result[1] = (u16)halbb_get_reg_curr_phy(bb, cr->fahm_cnt1,
							  cr->fahm_cnt1_m);
	env->fahm_result[2] = (u16)halbb_get_reg_curr_phy(bb, cr->fahm_cnt2,
							  cr->fahm_cnt2_m);
	env->fahm_result[3] = (u16)halbb_get_reg_curr_phy(bb, cr->fahm_cnt3,
							  cr->fahm_cnt3_m);
	env->fahm_result[4] = (u16)halbb_get_reg_curr_phy(bb, cr->fahm_cnt4,
							  cr->fahm_cnt4_m);
	env->fahm_result[5] = (u16)halbb_get_reg_curr_phy(bb, cr->fahm_cnt5,
							  cr->fahm_cnt5_m);
	env->fahm_result[6] = (u16)halbb_get_reg_curr_phy(bb, cr->fahm_cnt6,
							  cr->fahm_cnt6_m);
	env->fahm_result[7] = (u16)halbb_get_reg_curr_phy(bb, cr->fahm_cnt7,
							  cr->fahm_cnt7_m);
	env->fahm_result[8] = (u16)halbb_get_reg_curr_phy(bb, cr->fahm_cnt8,
							  cr->fahm_cnt8_m);
	env->fahm_result[9] = (u16)halbb_get_reg_curr_phy(bb, cr->fahm_cnt9,
							  cr->fahm_cnt9_m);
	env->fahm_result[10] = (u16)halbb_get_reg_curr_phy(bb, cr->fahm_cnt10,
							   cr->fahm_cnt10_m);
	env->fahm_result[11] = (u16)halbb_get_reg_curr_phy(bb, cr->fahm_cnt11,
							   cr->fahm_cnt11_m);

	for (i = 0; i < FAHM_RPT_NUM; i++)
		result_sum_tmp += (u32)env->fahm_result[i];

	env->fahm_result_sum = (u16)result_sum_tmp;
	BB_DBG(bb, DBG_ENV_MNTR, "fahm_result_sum=%d\n", env->fahm_result_sum);

	/*Get FAHM denominator*/
	env->fahm_denom_result = (u16)halbb_get_reg_curr_phy(bb,
							     cr->fahm_denom_cnt,
							     cr->fahm_denom_cnt_m);
	BB_DBG(bb, DBG_ENV_MNTR, "fahm_denominator result = %d\n",
	       env->fahm_denom_result);

	BB_DBG(bb, DBG_ENV_MNTR,
	       "FAHM hw result[%d](H->L)[%d %d %d %d %d %d %d %d %d %d %d %d]\n",
	       env->fahm_rpt_stamp, env->fahm_result[11], env->fahm_result[10],
	       env->fahm_result[9], env->fahm_result[8], env->fahm_result[7],
	       env->fahm_result[6], env->fahm_result[5], env->fahm_result[4],
	       env->fahm_result[3], env->fahm_result[2], env->fahm_result[1],
	       env->fahm_result[0]);

	halbb_fahm_get_utility(bb);

	return true;
}

void halbb_fahm_set_th_reg(struct bb_info *bb)
{
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;
	struct bb_env_mntr_cr_info *cr = &env->bb_env_mntr_cr_i;

	BB_DBG(bb, DBG_ENV_MNTR, "[%s]===>\n", __func__);

	if (((bb->ic_type == BB_RTL8852A) || (bb->ic_type == BB_RTL8852B) ||
	    (bb->ic_type == BB_RTL8852C) || (bb->ic_type == BB_RTL8192XB) ||
		(bb->ic_type == BB_RTL8851B))) {
		/*Set FAHM threshold*/ /*Unit: RSSI U(8,1)*/
		halbb_set_reg_curr_phy(bb, cr->fahm_th0, cr->fahm_th0_m,
				       env->fahm_th[0]-env->ccx_ext_loss);
		halbb_set_reg_curr_phy(bb, cr->fahm_th1, cr->fahm_th1_m,
				       env->fahm_th[1]-env->ccx_ext_loss);
		halbb_set_reg_curr_phy(bb, cr->fahm_th2, cr->fahm_th2_m,
				       env->fahm_th[2]-env->ccx_ext_loss);
		halbb_set_reg_curr_phy(bb, cr->fahm_th3, cr->fahm_th3_m,
				       env->fahm_th[3]-env->ccx_ext_loss);
		halbb_set_reg_curr_phy(bb, cr->fahm_th4, cr->fahm_th4_m,
				       env->fahm_th[4]-env->ccx_ext_loss);
		halbb_set_reg_curr_phy(bb, cr->fahm_th5, cr->fahm_th5_m,
				       env->fahm_th[5]-env->ccx_ext_loss);
		halbb_set_reg_curr_phy(bb, cr->fahm_th6, cr->fahm_th6_m,
				       env->fahm_th[6]-env->ccx_ext_loss);
		halbb_set_reg_curr_phy(bb, cr->fahm_th7, cr->fahm_th7_m,
				       env->fahm_th[7]-env->ccx_ext_loss);
		halbb_set_reg_curr_phy(bb, cr->fahm_th8, cr->fahm_th8_m,
				       env->fahm_th[8]-env->ccx_ext_loss);
		halbb_set_reg_curr_phy(bb, cr->fahm_th9, cr->fahm_th9_m,
				       env->fahm_th[9]-env->ccx_ext_loss);
		if ((bb->ic_type == BB_RTL8852A) || (bb->ic_type == BB_RTL8852B) ||
			(bb->ic_type == BB_RTL8852C))
			halbb_set_reg_curr_phy(bb, cr->fahm_th10, cr->fahm_th10_m,
					RSSI_2_FAHM_TH(FAHM_WA_TH));
		else
			halbb_set_reg_curr_phy(bb, cr->fahm_th10, cr->fahm_th10_m,
				       env->fahm_th[10]-env->ccx_ext_loss);
	} else {
		/*Set FAHM threshold*/ /*Unit: RSSI U(8,1)*/
		halbb_set_reg_curr_phy(bb, cr->fahm_th0, cr->fahm_th0_m,
				       env->fahm_th[0]);
		halbb_set_reg_curr_phy(bb, cr->fahm_th1, cr->fahm_th1_m,
				       env->fahm_th[1]);
		halbb_set_reg_curr_phy(bb, cr->fahm_th2, cr->fahm_th2_m,
				       env->fahm_th[2]);
		halbb_set_reg_curr_phy(bb, cr->fahm_th3, cr->fahm_th3_m,
				       env->fahm_th[3]);
		halbb_set_reg_curr_phy(bb, cr->fahm_th4, cr->fahm_th4_m,
				       env->fahm_th[4]);
		halbb_set_reg_curr_phy(bb, cr->fahm_th5, cr->fahm_th5_m,
				       env->fahm_th[5]);
		halbb_set_reg_curr_phy(bb, cr->fahm_th6, cr->fahm_th6_m,
				       env->fahm_th[6]);
		halbb_set_reg_curr_phy(bb, cr->fahm_th7, cr->fahm_th7_m,
				       env->fahm_th[7]);
		halbb_set_reg_curr_phy(bb, cr->fahm_th8, cr->fahm_th8_m,
				       env->fahm_th[8]);
		halbb_set_reg_curr_phy(bb, cr->fahm_th9, cr->fahm_th9_m,
				       env->fahm_th[9]);
		halbb_set_reg_curr_phy(bb, cr->fahm_th10, cr->fahm_th10_m,
				       env->fahm_th[10]);
	}

	BB_DBG(bb, DBG_ENV_MNTR,
	       "Update FAHM_th[H->L]=[%d %d %d %d %d %d %d %d %d %d %d]\n",
	       env->fahm_th[10], env->fahm_th[9], env->fahm_th[8],
	       env->fahm_th[7], env->fahm_th[6], env->fahm_th[5],
	       env->fahm_th[4], env->fahm_th[3], env->fahm_th[2],
	       env->fahm_th[1], env->fahm_th[0]);
}

bool
halbb_fahm_th_update_chk(struct bb_info *bb, struct fahm_para_info *para,
			 u8 *fahm_th)
{
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;
	bool is_app_change = (env->fahm_app == para->fahm_app) ? false : true;
	bool is_ccx_ext_loss_update = env->ccx_ext_loss_update;
	bool is_update = (is_app_change || is_ccx_ext_loss_update);
	u8 fahm_th_11k[FAHM_TH_NUM] = {18, 21, 24, 27, 30, 35, 40, 45, 50, 55,
				       60}; /*Unit RSSI*/
	u8 i = 0;
	u8 th_ofst = 3;
	u8 th0 = 0;

	BB_DBG(bb, DBG_ENV_MNTR, "fahm_App=%d, is_ccx_ext_loss_update=%d\n",
		para->fahm_app, is_ccx_ext_loss_update);

	if (!is_update)
		goto CHK_FAHM_UPDATE_FINISHED;

	switch (para->fahm_app) {
	case FAHM_INIT:
	case FAHM_BACKGROUND: /* IEEE 11K*/
	case FAHM_DBG_11K:
	case FAHM_DIG:
		is_update = true;
		for (i = 0; i < FAHM_TH_NUM; i++) {
			if (((bb->ic_type == BB_RTL8852A) ||
			    (bb->ic_type == BB_RTL8852B) ||
			    (bb->ic_type == BB_RTL8852C)) &&
			    (i == (FAHM_TH_NUM - 1)))
				fahm_th[i] = RSSI_2_FAHM_TH(FAHM_WA_TH);
			else
				fahm_th[i] = RSSI_2_FAHM_TH(fahm_th_11k[i]);
		}
		break;
	case FAHM_DBG_MANUAL:
		is_update = true;
		th_ofst = para->fahm_manual_th_ofst;
		if ((bb->ic_type == BB_RTL8852A) ||
		    (bb->ic_type == BB_RTL8852B) ||
		    (bb->ic_type == BB_RTL8852C)) {
			/*fahm_th[0] upper bound is 109 - 10 * th_ofst*/
			th0 = MIN_2(para->fahm_manual_th0,
				    FAHM_WA_TH - th_ofst * (FAHM_TH_NUM - 1));
			for (i = 0; i < FAHM_TH_NUM; i++) {
				if (i == (FAHM_TH_NUM - 1))
					fahm_th[i] = RSSI_2_FAHM_TH(FAHM_WA_TH);
				else
					fahm_th[i] = RSSI_2_FAHM_TH(th0 +
								    (th_ofst *
								     i));
			}
		} else {
			/*fahm_th[0] upper bound is 127 - 10 * th_ofst*/
			th0 = MIN_2(para->fahm_manual_th0,
				    127 - th_ofst * (FAHM_TH_NUM - 1));
			for (i = 0; i < FAHM_TH_NUM; i++)
				fahm_th[i] = RSSI_2_FAHM_TH(th0 + (th_ofst *
								   i));
		}
		break;
	default:
		break;
	}

CHK_FAHM_UPDATE_FINISHED:
	if (!is_update)
		BB_DBG(bb, DBG_ENV_MNTR, "No need to update FAHM_TH\n");

	return is_update;
}

bool halbb_fahm_set(struct bb_info *bb, struct fahm_para_info *para)
{
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;
	struct bb_link_info *link = &bb->bb_link_i;
	struct bb_env_mntr_cr_info *cr = &env->bb_env_mntr_cr_i;
	enum channel_width bw = bb->hal_com->band[bb->bb_phy_idx].cur_chandef.bw;
	u16 mntr_time = 0;
	u32 period = 65535;
	u32 unit_idx = 0;
	u8 fahm_th[FAHM_TH_NUM] = {0};

	BB_DBG(bb, DBG_ENV_MNTR, "[%s]===>\n", __func__);

	if (para->fahm_mntr_time == 0) {
		BB_DBG(bb, DBG_ENV_MNTR, "[WARNING] fahm_MNTR_TIME is 0\n");
		return HALBB_SET_FAIL;
	}

	if (para->fahm_app == FAHM_DBG_MANUAL &&
	    para->fahm_manual_th_ofst == 0) {
		BB_DBG(bb, DBG_ENV_MNTR,
		       "[WARNING] th_ofst is 0 when fahm_app = manual\n");
		return HALBB_SET_FAIL;
	}

	if (para->fahm_app == FAHM_DBG_MANUAL &&
	    para->fahm_manual_th_ofst > 12) {
		BB_DBG(bb, DBG_ENV_MNTR,
		       "[WARNING] th_ofst is larger than 12 when fahm_app = manual\n");
		return HALBB_SET_FAIL;
	}

	if (halbb_fahm_racing_ctrl(bb, para->fahm_rac_lv) == HALBB_SET_FAIL)
		return HALBB_SET_FAIL;

	BB_DBG(bb, DBG_ENV_MNTR,
	       "fahm_numer_opt=%d, fahm_denum_opt=%d, fahm_mntr_time=%d ms\n",
	       para->fahm_numer_opt, para->fahm_denom_opt,
	       para->fahm_mntr_time);

	/*Set unit & period*/
	if (bw == CHANNEL_WIDTH_10)
		mntr_time = para->fahm_mntr_time >> 1;
	else if (bw == CHANNEL_WIDTH_5)
		mntr_time = para->fahm_mntr_time >> 2;
	else
		mntr_time = para->fahm_mntr_time;

	if (mntr_time != env->fahm_mntr_time) {
		halbb_ccx_ms_2_period_unit(bb, mntr_time, &period, &unit_idx);
		halbb_set_reg_curr_phy(bb, cr->fahm_period, cr->fahm_period_m,
				       period);
		halbb_set_reg_curr_phy(bb, cr->fahm_unit_idx,
				       cr->fahm_unit_idx_m, unit_idx);

		BB_DBG(bb, DBG_ENV_MNTR, "Update FAHM time ((%d)) -> ((%d)), bw = %d\n",
		       env->fahm_mntr_time, mntr_time, bw);

		env->fahm_mntr_time = mntr_time;
		env->fahm_period = (u16)period;
	}

	/*Set numerator_opt*/
	if (para->fahm_numer_opt != env->fahm_numer_opt) {
		halbb_set_reg_curr_phy(bb, cr->fahm_numer_opt,
				       cr->fahm_numer_opt_m,
				       para->fahm_numer_opt);

		BB_DBG(bb, DBG_ENV_MNTR,
		       "Update FAHM numer_opt ((%d)) -> ((%d))\n",
		       env->fahm_numer_opt, para->fahm_numer_opt);

		env->fahm_numer_opt = para->fahm_numer_opt;
	}

	/*Set denominator_opt*/
	if (para->fahm_denom_opt != env->fahm_denom_opt) {
		halbb_set_reg_curr_phy(bb, cr->fahm_denom_opt,
				     cr->fahm_denom_opt_m,
				     para->fahm_denom_opt);

		BB_DBG(bb, DBG_ENV_MNTR,
		       "Update FAHM denom_opt ((%d)) -> ((%d))\n",
		       env->fahm_denom_opt, para->fahm_denom_opt);

		env->fahm_denom_opt = para->fahm_denom_opt;
	}

	/*Set FAHM threshold*/
	if (halbb_fahm_th_update_chk(bb, para, &fahm_th[0])) {
		env->fahm_app = para->fahm_app;
		halbb_mem_cpy(bb, &env->fahm_th[0], &fahm_th, FAHM_TH_NUM);

		/*Set FAHM th*/
		halbb_fahm_set_th_reg(bb);
	}

	return HALBB_SET_SUCCESS;
}

void halbb_fahm_get_bg_result(struct bb_info *bb, struct fahm_report *bg_rpt)
{
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;

	halbb_mem_cpy(bb, bg_rpt, &env->fahm_report_bg,
		      sizeof(struct fahm_report));
}

void halbb_fahm_get_bg_setting(struct bb_info *bb,
			       struct fahm_para_info *bg_para)
{
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;

	halbb_mem_cpy(bb, bg_para, &env->fahm_para_info_bg,
		      sizeof(struct fahm_para_info));
}

bool halbb_fahm_trigger(struct bb_info *bb, struct fahm_para_info *para,
			    struct fahm_trig_report *trig_rpt)
{
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;
	bool trigger_result = false;

	env = &bb->bb_env_mntr_i;

	BB_DBG(bb, DBG_ENV_MNTR, "[%s] ======>\n", __func__);

	halbb_ccx_ext_loss_update(bb);

	if (halbb_fahm_set(bb, para))
		trigger_result = true;

	if (trigger_result)
		halbb_fahm_hw_trigger(bb);

	trig_rpt->fahm_rpt_stamp = env->fahm_rpt_stamp;

	BB_DBG(bb, DBG_ENV_MNTR, "fahm_rpt_stamp=%d, trigger_result=%d\n",
	       trig_rpt->fahm_rpt_stamp, trigger_result);

	/*update bg structure*/
	if ((env->fahm_app == FAHM_BACKGROUND) || (env->fahm_app == FAHM_DIG))
		halbb_mem_cpy(bb, &env->fahm_para_info_bg, para,
			      sizeof(struct fahm_para_info));

	return trigger_result;
}

bool halbb_fahm_result(struct bb_info *bb, struct fahm_report *rpt)
{
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;
	u8 i = 0;

	BB_DBG(bb, DBG_ENV_MNTR, "[%s] ======>\n", __func__);

	/*Get FAHM result*/
	if (halbb_fahm_get_result(bb)) {
		rpt->fahm_ratio = env->fahm_ratio;
		rpt->fahm_denom_ratio = env->fahm_denom_ratio;
		rpt->fahm_permil = env->fahm_permil;
		rpt->fahm_denom_permil = env->fahm_denom_permil;
		rpt->fahm_pwr = env->fahm_pwr;
		rpt->fahm_pwr_0p5= env->fahm_pwr_0p5;
		rpt->fahm_rpt_result = true;
		halbb_mem_cpy(bb, &rpt->fahm_rpt[0], &env->fahm_rpt[0], 
			      FAHM_RPT_NUM);
	} else {
		rpt->fahm_ratio = ENV_MNTR_FAIL_BYTE;
		rpt->fahm_denom_ratio = ENV_MNTR_FAIL_BYTE;
		rpt->fahm_permil = ENV_MNTR_FAIL_WORD;
		rpt->fahm_denom_permil = ENV_MNTR_FAIL_WORD;
		rpt->fahm_pwr = ENV_MNTR_FAIL_BYTE;
		rpt->fahm_pwr_0p5 = ENV_MNTR_FAIL_BYTE;
		for (i = 0; i < FAHM_RPT_NUM; i++)
			rpt->fahm_rpt[i] = ENV_MNTR_FAIL_BYTE;
	}

	rpt->fahm_rpt_stamp = env->fahm_rpt_stamp;

	BB_DBG(bb, DBG_ENV_MNTR, "fahm_rpt_stamp=%d, fahm_rpt_result=%d\n",
	       rpt->fahm_rpt_stamp, rpt->fahm_rpt_result);

	/*update bg structure*/
	if ((env->fahm_app == FAHM_BACKGROUND) || (env->fahm_app == FAHM_DIG))
		halbb_mem_cpy(bb, &env->fahm_report_bg, rpt,
			      sizeof(struct fahm_report));

	halbb_fahm_racing_release(bb);

	return rpt->fahm_rpt_result;
}

void halbb_fahm_init(struct bb_info *bb)
{
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;
	struct bb_env_mntr_cr_info *cr = &env->bb_env_mntr_cr_i;
	u8 i = 0;
	u8 fahm_th_11k[FAHM_TH_NUM] = {18, 21, 24, 27, 30, 35, 40, 45, 50, 55,
				       60}; /*Unit RSSI*/
	u8 fahm_th[FAHM_TH_NUM] = {0};

	BB_DBG(bb, DBG_ENV_MNTR, "[%s]===>\n", __func__);
	env->fahm_manual_ctrl = false;
	env->fahm_ongoing = false;
	env->fahm_rac_lv = RAC_RELEASE;
	env->fahm_rpt_stamp = 0;
	env->fahm_period = 0;
	env->fahm_trigger_time = 0;

	env->fahm_app = FAHM_INIT;
	env->fahm_numer_opt = 0;
	env->fahm_denom_opt = 0;
	env->fahm_mntr_time = 0;

	/*Set FAHM threshold*/
	for (i = 0; i < FAHM_TH_NUM; i++) {
		if (((bb->ic_type == BB_RTL8852A) ||
		    (bb->ic_type == BB_RTL8852B) ||
		    (bb->ic_type == BB_RTL8852C)) &&
		    (i == (NHM_TH_NUM - 1)))
			fahm_th[i] = RSSI_2_FAHM_TH(FAHM_WA_TH);
		else
			fahm_th[i] = RSSI_2_FAHM_TH(fahm_th_11k[i]);
	}
	halbb_mem_cpy(bb, &env->fahm_th[0], &fahm_th, FAHM_TH_NUM);

	halbb_set_reg_curr_phy(bb, cr->fahm_en, cr->fahm_en_m, true);

	/*r_fahm_en_ofdm = r_fahm_en_cck = 1, or fahm report will be 0.*/
	halbb_set_reg_curr_phy(bb, cr->fahm_ofdm_en, cr->fahm_ofdm_en_m, true);
	halbb_set_reg_curr_phy(bb, cr->fahm_cck_en, cr->fahm_cck_en_m, true);

	/*r_fahm_pwdb_sel = 1:select max path*/
	halbb_set_reg_curr_phy(bb, cr->fahm_method_sel, cr->fahm_method_sel_m,
			     0x1);

	/*r_fahm_dis_count_each_mpdu = 1, or fa report will abnormal*/
	halbb_set_reg_curr_phy(bb, cr->fahm_dis_count_each_mpdu,
			     cr->fahm_dis_count_each_mpdu_m, true);
}

void halbb_fahm_dbg(struct bb_info *bb, char input[][16], u32 *_used,
		    char *output, u32 *_out_len)
{
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;
	struct fahm_para_info para;
	u32 var[10] = {0};
	u8 i = 0;
	u8 end_i = 0;

	HALBB_SCAN(input[1], DCMD_DECIMAL, &var[0]);

	if ((_os_strcmp(input[1], "-h") == 0)) {
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "FAHM Get Result: {100}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "Basic-Trigger(11k/1900ms): {1}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "Adv-Trigger(11k): {2} {0~2097ms} {numer_opt} {denom_opt}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "Adv-Trigger(Manual): {3} {0~2097ms} {numer_opt} {denom_opt} {th[0]} {th_ofst:1~12}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "numer_opt/denom_opt: {BIT 0/1/2} = {FA/CRC32_OK/CRC32_ERR}\n");
	} else if (var[0] == 100) { /*Get FAHM results*/
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "fahm_rpt_stamp=%d, fahm_period=%d\n",
			    env->fahm_rpt_stamp, env->fahm_period);

		if (halbb_fahm_get_result(bb)) {
			if ((bb->ic_type == BB_RTL8852A) ||
			    (bb->ic_type == BB_RTL8852B) ||
			    (bb->ic_type == BB_RTL8852C))
				end_i = FAHM_RPT_NUM - 1;
			else
				end_i = FAHM_RPT_NUM;
			for (i = 0; i < end_i; i++)
				BB_DBG_CNSL(*_out_len, *_used, output + *_used,
					    *_out_len - *_used,
					    "fahm_sw_result[%d] = %d (%d percent)\n",
					    i, env->fahm_sw_result[i],
					    env->fahm_rpt[i]);

			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used,
				    "fahm_pwr=%d, fahm_pwr_0p5=%d.%d\n",
				    env->fahm_pwr, env->fahm_pwr_0p5 >> 1,
				    5 * (env->fahm_pwr_0p5 & 0x1));

			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used,
				    "fahm_numerator result = %d (%d percent/%d permil)\n",
				    env->fahm_result_sum, env->fahm_ratio,
				    env->fahm_permil);

			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used,
				    "fahm_denominator result = %d (%d percent/%d permil)\n",
				    env->fahm_denom_result,
				    env->fahm_denom_ratio,
				    env->fahm_denom_permil);
		} else {
			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used, "Get FAHM_rpt Fail\n");
		}

		halbb_fahm_racing_release(bb);
		env->fahm_manual_ctrl = false;
	} else { /*FAMH trigger*/
		env->fahm_manual_ctrl = true;

		for (i = 1; i < 9; i++) {
			if (input[i + 1])
				HALBB_SCAN(input[i + 1], DCMD_DECIMAL, &var[i]);
		}

		if (var[0] == 1) {
			para.fahm_app = FAHM_DBG_11K;
			para.fahm_mntr_time = 1900;
			para.fahm_numer_opt = FAHM_INCLU_FA;
			para.fahm_denom_opt = FAHM_INCLU_CRC_ERR;
		} else if (var[0] == 2) {
			para.fahm_app = FAHM_DBG_11K;
			para.fahm_mntr_time = (u16)var[1];
			para.fahm_numer_opt = (u8)var[2];
			para.fahm_denom_opt = (u8)var[3];
		} else if (var[0] == 3) {
			para.fahm_app = FAHM_DBG_MANUAL;
			para.fahm_mntr_time = (u16)var[1];
			para.fahm_numer_opt = (u8)var[2];
			para.fahm_denom_opt = (u8)var[3];
			para.fahm_manual_th0 = (u8)var[4];
			para.fahm_manual_th_ofst = (u8)var[5];
			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used,
				    "manual_th_ofst=%d, manaul_th_0=%d\n",
				    para.fahm_manual_th_ofst,
				    para.fahm_manual_th0);
		}

		para.fahm_rac_lv = RAC_LV_4;

		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "app=%d, lv=%d, time=%d ms, numer_opt=%d, denom_opt=%d\n",
			    para.fahm_app, para.fahm_rac_lv,
			    para.fahm_mntr_time, para.fahm_numer_opt,
			    para.fahm_denom_opt);

		if (halbb_fahm_set(bb, &para) == HALBB_SET_SUCCESS) {
			halbb_fahm_hw_trigger(bb);

			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used, "fahm_rpt_stamp=%d\n",
				    env->fahm_rpt_stamp);

			for (i = 0; i < FAHM_TH_NUM; i++) {
				BB_DBG_CNSL(*_out_len, *_used, output + *_used,
					    *_out_len - *_used,
					    "FAHM_th[%d] RSSI = %d\n", i,
					    FAHM_TH_2_RSSI(env->fahm_th[i]));
			}
		} else {
			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used, "FAHM mntr set fail!\n");
		}
	}
}

#endif
#ifdef EDCCA_CLM_SUPPORT

void halbb_edcca_clm_get_utility(struct bb_info *bb)
{
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;

	env->edcca_clm_ratio = (u8)halbb_ccx_get_ratio(bb,
						       env->edcca_clm_result,
						       100);
}

bool
halbb_edcca_clm_get_result(struct bb_info *bb)
{
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;
	struct bb_env_mntr_cr_info *cr = &env->bb_env_mntr_cr_i;

	if (!(halbb_get_reg_curr_phy(bb, cr->edcca_clm_rdy,
				     cr->edcca_clm_rdy_m))) {
		BB_DBG(bb, DBG_ENV_MNTR, "Get EDCCA_CLM report Fail\n");
		return false;
	}

	env->edcca_clm_result = (u16)halbb_get_reg_curr_phy(bb,
							    cr->edcca_clm_cnt,
							    cr->edcca_clm_cnt_m);

	BB_DBG(bb, DBG_ENV_MNTR, "EDCCA_CLM result = %d\n",
	       env->edcca_clm_result);

	halbb_edcca_clm_get_utility(bb);

	return true;
}

bool halbb_edcca_clm_set(struct bb_info *bb, struct ccx_para_info *para)
{
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;
	struct bb_env_mntr_cr_info *cr = &env->bb_env_mntr_cr_i;
	enum channel_width bw = bb->hal_com->band[bb->bb_phy_idx].cur_chandef.bw;
	u16 mntr_time = 0;
	u32 period = 0;
	u32 unit_idx = 0;

	if (para->mntr_time == 0) {
		BB_DBG(bb, DBG_ENV_MNTR, "[WARNING] MNTR_TIME is 0\n");
		return HALBB_SET_FAIL;
	}

	if (halbb_ccx_racing_ctrl(bb, para->rac_lv) == HALBB_SET_FAIL)
		return HALBB_SET_FAIL;

	/*Set unit & period*/
	if (bw == CHANNEL_WIDTH_10)
		mntr_time = para->mntr_time >> 1;
	else if (bw == CHANNEL_WIDTH_5)
		mntr_time = para->mntr_time >> 2;
	else
		mntr_time = para->mntr_time;

	if (mntr_time != env->edcca_clm_mntr_time) {
		halbb_ccx_ms_2_period_unit(bb, mntr_time, &period, &unit_idx);
		halbb_set_reg_curr_phy(bb, cr->edcca_clm_period,
				       cr->edcca_clm_period_m, period);
		halbb_set_reg_curr_phy(bb, cr->edcca_clm_unit_idx,
				       cr->edcca_clm_unit_idx_m, unit_idx);

		BB_DBG(bb, DBG_ENV_MNTR,
		       "Update EDCCA-CLM time ((%d)) -> ((%d)), bw = %d\n",
		       env->edcca_clm_mntr_time, mntr_time, bw);

		env->edcca_clm_mntr_time = mntr_time;
		env->ccx_period = (u16)period;
		env->ccx_unit_idx = (u8)unit_idx;
	}

	halbb_ccx_edcca_opt_set(bb, para->ccx_edcca_opt_sc_idx);

	env->edcca_clm_app = para->edcca_clm_app;

	return HALBB_SET_SUCCESS;
}

void halbb_edcca_clm_init(struct bb_info *bb)
{
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;
	struct bb_env_mntr_cr_info *cr = &env->bb_env_mntr_cr_i;

	BB_DBG(bb, DBG_ENV_MNTR, "[%s]===>\n", __func__);
	env->edcca_clm_app = EDCCA_CLM_INIT;
	env->edcca_clm_mntr_time = 0;

	halbb_set_reg_curr_phy(bb, cr->edcca_clm_en, cr->edcca_clm_en_m, true);
}

void halbb_edcca_clm_dbg(struct bb_info *bb, char input[][16], u32 *_used,
			 char *output, u32 *_out_len)
{
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;
	char help[] = "-h";
	u32 var[10] = {0};
	struct ccx_para_info para = {0};
	u8 i = 0;

	for (i = 0; i < 5; i++) {
		if (input[i + 1])
			HALBB_SCAN(input[i + 1], DCMD_DECIMAL, &var[i]);
	}

	if ((_os_strcmp(input[1], help) == 0)) {
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "EDCCA-CLM Get Result: {100}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "EDCCA-CLM Basic-Trigger(1900ms): {1}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "EDCCA-CLM Adv-Trigger: {2} {0~2097ms} {CCX_edcca_opt}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "edcca_opt: 0~3:p0/s1/s2/s3\n");
	} else if (var[0] == 100) { /*Get EDCCA-CLM results */
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "ccx_rpt_stamp=%d, ccx_period=%d\n",
			    env->ccx_rpt_stamp, env->ccx_period);

		if (halbb_edcca_clm_get_result(bb)) {
			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used,
				    "edcca_clm_result = %d (%d percent)\n",
				    env->edcca_clm_result,
				    env->edcca_clm_ratio);
		} else {
			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used,
				    "Get EDCCA_CLM_rpt Fail\n");
		}

		halbb_ccx_racing_release(bb, EDCCA_CLM_SEL);
		env->ccx_manual_ctrl = false;
	} else { /* Set & trigger CLM */
		env->ccx_manual_ctrl = true;

		if (var[0] == 1) {
			para.mntr_time = 1900;
			para.ccx_edcca_opt_sc_idx = CCX_EDCCA_P0;
		} else if (var[0] == 2) {
			para.mntr_time = (u16)var[1];
			para.ccx_edcca_opt_sc_idx = (enum ccx_edcca_opt_sc_idx)var[2];
		}

		para.edcca_clm_app = EDCCA_CLM_DBG;
		para.rac_lv = RAC_LV_4;

		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "app=%d, lv=%d, time=%d ms, edcca_opt=%d\n",
			    para.edcca_clm_app, para.rac_lv, para.mntr_time,
			    para.ccx_edcca_opt_sc_idx);

		if (halbb_edcca_clm_set(bb, &para) == HALBB_SET_SUCCESS) {
			halbb_ccx_trigger(bb, EDCCA_CLM_SEL);

			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used, "ccx_rpt_stamp=%d\n",
				    env->ccx_rpt_stamp);
		} else {
			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used,
				    "EDCCA_CLM mntr set fail!\n");
		}
	}
}

#endif
u32 halbb_env_mntr_get_fw_result_c2h(struct bb_info *bb, u8 *c2h)
{
	if (!c2h) {
		BB_WARNING("Error fw env_mntr c2h failed!!\n");
		return false;
	}

	if (c2h[0] == NHM_SEL)
		halbb_nhm_get_fw_result_c2h(bb, c2h + 1);

	return true;
}

bool
halbb_env_mntr_init_app_chk(struct bb_info *bb)
{
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;
	bool chk_result = HALBB_SET_FAIL;

	if ((env->clm_app == CLM_INIT) &&
	    ((env->nhm_app == NHM_INIT) || (env->nhm_app == NHM_FW_CTRL)) &&
	    (env->ifs_clm_app == IFS_CLM_INIT) &&
	    (env->edcca_clm_app == EDCCA_CLM_INIT))
		chk_result = HALBB_SET_SUCCESS;

	return chk_result;
}

bool
halbb_env_mntr_bg_app_chk(struct bb_info *bb)
{
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;
	bool chk_result = HALBB_SET_FAIL;

	if ((env->clm_app == CLM_BACKGROUND) &&
	    ((env->nhm_app == NHM_BACKGROUND) ||
	     (env->nhm_app == NHM_FW_CTRL) ||
	     (env->nhm_app == NHM_INIT)) &&
	    (env->ifs_clm_app == IFS_CLM_BACKGROUND) &&
	    (env->edcca_clm_app == EDCCA_CLM_BACKGROUND))
		chk_result = HALBB_SET_SUCCESS;

	return chk_result;
}

void halbb_env_mntr_cmn_log(struct bb_info *bb)
{
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;
	u8 i = 0;

	if (!(bb->cmn_dbg_msg_component & BB_BASIC_DBG_02_ENVMNTR)) {
		BB_DBG(bb, DBG_CMN, "Disabled\n");
		return;
	}

	if ((env->ccx_watchdog_result & CCX_SEL_EXCLU_NHM_FAHM) == CCX_FAIL) {
		BB_DBG(bb, DBG_CMN, "[CLM/IFS_CLM/EDCCA_CLM]Env_mntr get CCX result failed!\n");
		BB_DBG(bb, DBG_CMN,
		       "APP:{CLM, IFS_CLM, EDCCA} = {%d, %d, %d}\n",
		       env->clm_app, env->ifs_clm_app, env->edcca_clm_app);
	} else {
		BB_DBG(bb, DBG_CMN,
		       "[CLM/IFS_CLM/EDCCA_CLM]{Tx, CCA_sec, EDCCA_p20, EDCCA_exclu_CCA} = {%d, %d, %d, %d} %%\n",
		       env->ifs_clm_tx_ratio, env->clm_ratio,
		       env->edcca_clm_ratio, env->ifs_clm_edcca_excl_cca_ratio);
		BB_DBG(bb, DBG_CMN,
		       "FA{CCK, OFDM}={%d, %d} %%, CCA_exclu_FA{CCK, OFDM}={%d, %d} %%\n",
		       env->ifs_clm_cck_fa_ratio, env->ifs_clm_ofdm_fa_ratio,
		       env->ifs_clm_cck_cca_excl_fa_ratio,
		       env->ifs_clm_ofdm_cca_excl_fa_ratio);
	}

	if (((env->ccx_watchdog_result & NHM_SEL) == CCX_FAIL) &&
	    (env->nhm_app != NHM_FW_CTRL)) {
		BB_DBG(bb, DBG_CMN,
		       "[NHM]Env_mntr get NHM result failed! NHM APP=%d\n",
		       env->nhm_app);
	} else {
		BB_DBG(bb, DBG_CMN,
		       "[NHM]{Tx, Idle, CCA_p20} = {%d, %d, %d} %%, nhm_ratio = %d %%\n",
		       env->nhm_tx_ratio, env->nhm_idle_ratio,
		       env->nhm_cca_ratio, env->nhm_ratio);
		if ((bb->ic_type == BB_RTL8852A) ||
		    (bb->ic_type == BB_RTL8852B) ||
		    (bb->ic_type == BB_RTL8852C)) {
			BB_DBG(bb, DBG_CMN,
			       "%-18s[%.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d]\n",
			       "  Th", NHM_TH_2_RSSI(env->nhm_th[9]),
			       NHM_TH_2_RSSI(env->nhm_th[8]),
			       NHM_TH_2_RSSI(env->nhm_th[7]),
			       NHM_TH_2_RSSI(env->nhm_th[6]),
			       NHM_TH_2_RSSI(env->nhm_th[5]),
			       NHM_TH_2_RSSI(env->nhm_th[4]),
			       NHM_TH_2_RSSI(env->nhm_th[3]),
			       NHM_TH_2_RSSI(env->nhm_th[2]),
			       NHM_TH_2_RSSI(env->nhm_th[1]),
			       NHM_TH_2_RSSI(env->nhm_th[0]));
			BB_DBG(bb, DBG_CMN,
			       "[NHM]  (pwr:%02d.%d)[%.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d]\n",
			       env->nhm_pwr, 5 * (env->nhm_pwr_0p5 & 0x1),
			       env->nhm_rpt[10], env->nhm_rpt[9],
			       env->nhm_rpt[8], env->nhm_rpt[7],
			       env->nhm_rpt[6], env->nhm_rpt[5],
			       env->nhm_rpt[4], env->nhm_rpt[3],
			       env->nhm_rpt[2], env->nhm_rpt[1],
			       env->nhm_rpt[0]);
		} else {
			BB_DBG(bb, DBG_CMN,
			       "%-18s[%.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d]\n",
			       "  Th", NHM_TH_2_RSSI(env->nhm_th[10]),
			       NHM_TH_2_RSSI(env->nhm_th[9]),
			       NHM_TH_2_RSSI(env->nhm_th[8]),
			       NHM_TH_2_RSSI(env->nhm_th[7]),
			       NHM_TH_2_RSSI(env->nhm_th[6]),
			       NHM_TH_2_RSSI(env->nhm_th[5]),
			       NHM_TH_2_RSSI(env->nhm_th[4]),
			       NHM_TH_2_RSSI(env->nhm_th[3]),
			       NHM_TH_2_RSSI(env->nhm_th[2]),
			       NHM_TH_2_RSSI(env->nhm_th[1]),
			       NHM_TH_2_RSSI(env->nhm_th[0]));
			BB_DBG(bb, DBG_CMN,
			       "[NHM]  (pwr:%02d.%d)[%.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d]\n",
			       env->nhm_pwr, 5 * (env->nhm_pwr_0p5 & 0x1),
			       env->nhm_rpt[11], env->nhm_rpt[10],
			       env->nhm_rpt[9], env->nhm_rpt[8],
			       env->nhm_rpt[7], env->nhm_rpt[6],
			       env->nhm_rpt[5], env->nhm_rpt[4],
			       env->nhm_rpt[3], env->nhm_rpt[2],
			       env->nhm_rpt[1], env->nhm_rpt[0]);
		}
	}

	if ((env->fahm_watchdog_result == false) && (env->fahm_app != FAHM_DIG)) {
		BB_DBG(bb, DBG_CMN,
		       "[FAHM]Env_mntr get FAHM result failed! FAHM APP=%d\n",
		       env->fahm_app);
		return;
	}
	BB_DBG(bb, DBG_CMN, "[FAHM]{FA, CRC_err} = {%d, %d} %%\n",
	       env->fahm_ratio, env->fahm_denom_ratio);
	if ((bb->ic_type == BB_RTL8852A) || (bb->ic_type == BB_RTL8852B) ||
	    (bb->ic_type == BB_RTL8852C)) {
		BB_DBG(bb, DBG_CMN,
		       "%-18s[%.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d]\n",
		       "  Th", FAHM_TH_2_RSSI(env->fahm_th[9]),
		       FAHM_TH_2_RSSI(env->fahm_th[8]),
		       FAHM_TH_2_RSSI(env->fahm_th[7]),
		       FAHM_TH_2_RSSI(env->fahm_th[6]),
		       FAHM_TH_2_RSSI(env->fahm_th[5]),
		       FAHM_TH_2_RSSI(env->fahm_th[4]),
		       FAHM_TH_2_RSSI(env->fahm_th[3]),
		       FAHM_TH_2_RSSI(env->fahm_th[2]),
		       FAHM_TH_2_RSSI(env->fahm_th[1]),
		       FAHM_TH_2_RSSI(env->fahm_th[0]));
		BB_DBG(bb, DBG_CMN,
		       "[FAHM] (pwr:%02d.%d)[%.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d]\n",
		       env->fahm_pwr, 5 * (env->fahm_pwr_0p5 & 0x1),
		       env->fahm_rpt[10], env->fahm_rpt[9], env->fahm_rpt[8],
		       env->fahm_rpt[7], env->fahm_rpt[6], env->fahm_rpt[5],
		       env->fahm_rpt[4], env->fahm_rpt[3], env->fahm_rpt[2],
		       env->fahm_rpt[1], env->fahm_rpt[0]);
	} else {
		BB_DBG(bb, DBG_CMN,
		       "%-18s[%.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d]\n",
		       "  Th", FAHM_TH_2_RSSI(env->fahm_th[10]),
		       FAHM_TH_2_RSSI(env->fahm_th[9]),
		       FAHM_TH_2_RSSI(env->fahm_th[8]),
		       FAHM_TH_2_RSSI(env->fahm_th[7]),
		       FAHM_TH_2_RSSI(env->fahm_th[6]),
		       FAHM_TH_2_RSSI(env->fahm_th[5]),
		       FAHM_TH_2_RSSI(env->fahm_th[4]),
		       FAHM_TH_2_RSSI(env->fahm_th[3]),
		       FAHM_TH_2_RSSI(env->fahm_th[2]),
		       FAHM_TH_2_RSSI(env->fahm_th[1]),
		       FAHM_TH_2_RSSI(env->fahm_th[0]));
		BB_DBG(bb, DBG_CMN,
		       "[FAHM] (pwr:%02d.%d)[%.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d]\n",
		       env->fahm_pwr, 5 * (env->fahm_pwr_0p5 & 0x1),
		       env->fahm_rpt[11], env->fahm_rpt[10], env->fahm_rpt[9],
		       env->fahm_rpt[8], env->fahm_rpt[7], env->fahm_rpt[6],
		       env->fahm_rpt[5], env->fahm_rpt[4], env->fahm_rpt[3],
		       env->fahm_rpt[2], env->fahm_rpt[1], env->fahm_rpt[0]);
	}
}

void halbb_env_mntr_log(struct bb_info *bb, u32 dbg_comp)
{
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;
	u8 i = 0;
	u8 ifs_clm_num = 0;

	if (bb->ic_type & BB_IC_AX_SERIES)
		ifs_clm_num = IFS_CLM_NUM;
	else
		ifs_clm_num = BE_IFS_CLM_NUM;

	if (bb->bb_watchdog_mode != BB_WATCHDOG_NORMAL)
		return;

	if (dbg_comp == DBG_CMN) {
		halbb_env_mntr_cmn_log(bb);
		return;
	}

	if ((env->ccx_watchdog_result & CCX_SEL_EXCLU_NHM_FAHM) == CCX_FAIL) {
		BB_DBG(bb, DBG_ENV_MNTR, "[CLM/IFS_CLM/EDCCA_CLM]Env_mntr get CCX result failed!\n");
		BB_DBG(bb, DBG_ENV_MNTR,
		       "APP:{CLM, IFS_CLM, EDCCA} = {%d, %d, %d}\n",
		       env->clm_app, env->ifs_clm_app, env->edcca_clm_app);
	} else {
		BB_DBG(bb, DBG_ENV_MNTR,
		       "[CLM/IFS_CLM/EDCCA_CLM]{Tx, CCA_sec, EDCCA_p20, EDCCA_exclu_CCA} = {%d, %d, %d, %d} %%\n",
		       env->ifs_clm_tx_ratio, env->clm_ratio,
		       env->edcca_clm_ratio, env->ifs_clm_edcca_excl_cca_ratio);
		BB_DBG(bb, DBG_ENV_MNTR, "FA{CCK, OFDM}={%d, %d} %%, CCA_exclu_FA{CCK, OFDM}={%d, %d} %%\n",
		       env->ifs_clm_cck_fa_ratio, env->ifs_clm_ofdm_fa_ratio,
		       env->ifs_clm_cck_cca_excl_fa_ratio,
		       env->ifs_clm_ofdm_cca_excl_fa_ratio);
		BB_DBG(bb, DBG_ENV_MNTR,
		       "[IFS] Time(us):[his, ifs_avg(us), cca_avg(us)], total cnt=%d\n",
		       env->ifs_clm_total_ifs);
		for (i = 0; i < ifs_clm_num; i++)
			BB_DBG(bb, DBG_ENV_MNTR,
			       " *[%d](%04d~%04d):[%03d,     %04d,     %04d]\n",
			       i + 1,
			       halbb_ccx_idx_cnt_2_us(bb, env->ifs_clm_th_l[i]),
			       halbb_ccx_idx_cnt_2_us(bb, env->ifs_clm_th_h[i]),
			       env->ifs_clm_his[i], env->ifs_clm_ifs_avg[i],
			       env->ifs_clm_cca_avg[i]);
	}

	if (((env->ccx_watchdog_result & NHM_SEL) == CCX_FAIL) &&
	    (env->nhm_app != NHM_FW_CTRL)) {
		BB_DBG(bb, DBG_ENV_MNTR,
		       "[NHM]Env_mntr get NHM result failed! NHM APP=%d\n",
		       env->nhm_app);
	} else {
		BB_DBG(bb, DBG_ENV_MNTR,
		       "[NHM]{Tx, Idle, CCA_p20} = {%d, %d, %d} %%, nhm_ratio = %d %%\n",
		       env->nhm_tx_ratio, env->nhm_idle_ratio,
		       env->nhm_cca_ratio, env->nhm_ratio);
		if ((bb->ic_type == BB_RTL8852A) ||
		    (bb->ic_type == BB_RTL8852B) ||
		    (bb->ic_type == BB_RTL8852C)) {
			BB_DBG(bb, DBG_ENV_MNTR,
			       "%-18s[%.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d]\n",
			       "  Th", NHM_TH_2_RSSI(env->nhm_th[9]),
			       NHM_TH_2_RSSI(env->nhm_th[8]),
			       NHM_TH_2_RSSI(env->nhm_th[7]),
			       NHM_TH_2_RSSI(env->nhm_th[6]),
			       NHM_TH_2_RSSI(env->nhm_th[5]),
			       NHM_TH_2_RSSI(env->nhm_th[4]),
			       NHM_TH_2_RSSI(env->nhm_th[3]),
			       NHM_TH_2_RSSI(env->nhm_th[2]),
			       NHM_TH_2_RSSI(env->nhm_th[1]),
			       NHM_TH_2_RSSI(env->nhm_th[0]));
			BB_DBG(bb, DBG_ENV_MNTR,
			       "[NHM]  (pwr:%02d.%d)[%.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d]\n",
			       env->nhm_pwr, 5 * (env->nhm_pwr_0p5 & 0x1),
			       env->nhm_rpt[10], env->nhm_rpt[9],
			       env->nhm_rpt[8], env->nhm_rpt[7],
			       env->nhm_rpt[6], env->nhm_rpt[5],
			       env->nhm_rpt[4], env->nhm_rpt[3],
			       env->nhm_rpt[2], env->nhm_rpt[1],
			       env->nhm_rpt[0]);
		} else {
			BB_DBG(bb, DBG_ENV_MNTR,
			       "%-18s[%.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d]\n",
			       "  Th", NHM_TH_2_RSSI(env->nhm_th[10]),
			       NHM_TH_2_RSSI(env->nhm_th[9]),
			       NHM_TH_2_RSSI(env->nhm_th[8]),
			       NHM_TH_2_RSSI(env->nhm_th[7]),
			       NHM_TH_2_RSSI(env->nhm_th[6]),
			       NHM_TH_2_RSSI(env->nhm_th[5]),
			       NHM_TH_2_RSSI(env->nhm_th[4]),
			       NHM_TH_2_RSSI(env->nhm_th[3]),
			       NHM_TH_2_RSSI(env->nhm_th[2]),
			       NHM_TH_2_RSSI(env->nhm_th[1]),
			       NHM_TH_2_RSSI(env->nhm_th[0]));
			BB_DBG(bb, DBG_ENV_MNTR,
			       "[NHM]  (pwr:%02d.%d)[%.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d]\n",
			       env->nhm_pwr, 5 * (env->nhm_pwr_0p5 & 0x1),
			       env->nhm_rpt[11], env->nhm_rpt[10],
			       env->nhm_rpt[9], env->nhm_rpt[8],
			       env->nhm_rpt[7], env->nhm_rpt[6],
			       env->nhm_rpt[5], env->nhm_rpt[4],
			       env->nhm_rpt[3], env->nhm_rpt[2],
			       env->nhm_rpt[1], env->nhm_rpt[0]);
		}
	}

	if ((env->fahm_watchdog_result == false) && (env->fahm_app != FAHM_DIG)) {
		BB_DBG(bb, DBG_ENV_MNTR,
		       "[FAHM]Env_mntr get FAHM result failed! FAHM APP=%d\n",
		       env->fahm_app);
		return;
	}
	BB_DBG(bb, DBG_ENV_MNTR, "[FAHM]{FA, CRC_err} = {%d, %d} %%\n",
	       env->fahm_ratio, env->fahm_denom_ratio);
	if ((bb->ic_type == BB_RTL8852A) || (bb->ic_type == BB_RTL8852B) ||
	    (bb->ic_type == BB_RTL8852C)) {
		BB_DBG(bb, DBG_ENV_MNTR,
		       "%-18s[%.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d]\n",
		       "  Th", FAHM_TH_2_RSSI(env->fahm_th[9]),
		       FAHM_TH_2_RSSI(env->fahm_th[8]),
		       FAHM_TH_2_RSSI(env->fahm_th[7]),
		       FAHM_TH_2_RSSI(env->fahm_th[6]),
		       FAHM_TH_2_RSSI(env->fahm_th[5]),
		       FAHM_TH_2_RSSI(env->fahm_th[4]),
		       FAHM_TH_2_RSSI(env->fahm_th[3]),
		       FAHM_TH_2_RSSI(env->fahm_th[2]),
		       FAHM_TH_2_RSSI(env->fahm_th[1]),
		       FAHM_TH_2_RSSI(env->fahm_th[0]));
		BB_DBG(bb, DBG_ENV_MNTR,
		       "[FAHM] (pwr:%02d.%d)[%.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d]\n",
		       env->fahm_pwr, 5 * (env->fahm_pwr_0p5 & 0x1),
		       env->fahm_rpt[10], env->fahm_rpt[9], env->fahm_rpt[8],
		       env->fahm_rpt[7], env->fahm_rpt[6], env->fahm_rpt[5],
		       env->fahm_rpt[4], env->fahm_rpt[3], env->fahm_rpt[2],
		       env->fahm_rpt[1], env->fahm_rpt[0]);
	} else {
		BB_DBG(bb, DBG_ENV_MNTR,
		       "%-18s[%.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d]\n",
		       "  Th", FAHM_TH_2_RSSI(env->fahm_th[10]),
		       FAHM_TH_2_RSSI(env->fahm_th[9]),
		       FAHM_TH_2_RSSI(env->fahm_th[8]),
		       FAHM_TH_2_RSSI(env->fahm_th[7]),
		       FAHM_TH_2_RSSI(env->fahm_th[6]),
		       FAHM_TH_2_RSSI(env->fahm_th[5]),
		       FAHM_TH_2_RSSI(env->fahm_th[4]),
		       FAHM_TH_2_RSSI(env->fahm_th[3]),
		       FAHM_TH_2_RSSI(env->fahm_th[2]),
		       FAHM_TH_2_RSSI(env->fahm_th[1]),
		       FAHM_TH_2_RSSI(env->fahm_th[0]));
		BB_DBG(bb, DBG_ENV_MNTR,
		       "[FAHM] (pwr:%02d.%d)[%.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d]\n",
		       env->fahm_pwr, 5 * (env->fahm_pwr_0p5 & 0x1),
		       env->fahm_rpt[11], env->fahm_rpt[10], env->fahm_rpt[9],
		       env->fahm_rpt[8], env->fahm_rpt[7], env->fahm_rpt[6],
		       env->fahm_rpt[5], env->fahm_rpt[4], env->fahm_rpt[3],
		       env->fahm_rpt[2], env->fahm_rpt[1], env->fahm_rpt[0]);
	}
}

void halbb_env_mntr_get_bg_result(struct bb_info *bb_0,
				  struct env_mntr_rpt *bg_rpt,
				  enum phl_phy_idx phy_idx)
{
	struct bb_info *bb = bb_0;
	struct bb_env_mntr_info *env = NULL;

#ifdef HALBB_DBCC_SUPPORT
	HALBB_GET_PHY_PTR(bb_0, bb, phy_idx);
	BB_DBG(bb, DBG_ENV_MNTR, "[%s] phy_idx=%d\n", __func__, bb->bb_phy_idx);
#endif

	env = &bb->bb_env_mntr_i;

	halbb_mem_cpy(bb, bg_rpt, &env->env_mntr_rpt_bg,
		      sizeof(struct env_mntr_rpt));
}

void halbb_env_mntr_get_bg_setting(struct bb_info *bb_0,
				   struct ccx_para_info *bg_para,
				   enum phl_phy_idx phy_idx)
{
	struct bb_info *bb = bb_0;
	struct bb_env_mntr_info *env = NULL;

#ifdef HALBB_DBCC_SUPPORT
	HALBB_GET_PHY_PTR(bb_0, bb, phy_idx);
	BB_DBG(bb, DBG_ENV_MNTR, "[%s] phy_idx=%d\n", __func__, bb->bb_phy_idx);
#endif

	env = &bb->bb_env_mntr_i;

	halbb_mem_cpy(bb, bg_para, &env->ccx_para_info_bg,
		      sizeof(struct ccx_para_info));
}

u8 halbb_env_mntr_trigger(struct bb_info *bb_0, struct ccx_para_info *para,
		        struct env_trig_rpt *trig_rpt, enum phl_phy_idx phy_idx)
{
	struct bb_info *bb = bb_0;
	struct bb_env_mntr_info *env = NULL;
	u8 trigger_result = CCX_FAIL;

#ifdef HALBB_DBCC_SUPPORT
	HALBB_GET_PHY_PTR(bb_0, bb, phy_idx);
	BB_DBG(bb, DBG_ENV_MNTR, "[%s] phy_idx=%d\n", __func__, bb->bb_phy_idx);
#endif
	env = &bb->bb_env_mntr_i;

	BB_DBG(bb, DBG_ENV_MNTR, "[%s] ======>\n", __func__);

	halbb_ccx_ext_loss_update(bb);

	if (halbb_clm_set(bb, para))
		trigger_result |= CLM_SUCCESS;

	if (halbb_nhm_set(bb, para))
		trigger_result |= NHM_SUCCESS;

	if (halbb_ifs_clm_set(bb, para))
		trigger_result |= IFS_CLM_SUCCESS;

	if (halbb_edcca_clm_set(bb, para))
		trigger_result |= EDCCA_CLM_SUCCESS;

	if (trigger_result)
		halbb_ccx_trigger(bb, trigger_result);

	trig_rpt->ccx_rpt_stamp = env->ccx_rpt_stamp;

	/*update bg structure*/
	if (halbb_env_mntr_bg_app_chk(bb))
		halbb_mem_cpy(bb, &env->ccx_para_info_bg, para,
			      sizeof(struct ccx_para_info));

	BB_DBG(bb, DBG_ENV_MNTR, "ccx_rpt_stamp=%d, trigger_result=0x%x\n",
	       trig_rpt->ccx_rpt_stamp, trigger_result);

	return trigger_result;
}

u8 halbb_env_mntr_result(struct bb_info *bb_0, struct env_mntr_rpt *rpt,
		       enum phl_phy_idx phy_idx)
{
	struct bb_info *bb = bb_0;
	struct bb_env_mntr_info *env = NULL;
	u8 i = 0;
	u8 ifs_clm_num = 0;

	if (bb->ic_type & BB_IC_AX_SERIES)
		ifs_clm_num = IFS_CLM_NUM;
	else
		ifs_clm_num = BE_IFS_CLM_NUM;

#ifdef HALBB_DBCC_SUPPORT
	HALBB_GET_PHY_PTR(bb_0, bb, phy_idx);
	BB_DBG(bb, DBG_ENV_MNTR, "[%s] phy_idx=%d\n", __func__, bb->bb_phy_idx);
#endif
	env = &bb->bb_env_mntr_i;

	BB_DBG(bb, DBG_ENV_MNTR, "[%s] ======>\n", __func__);

	rpt->ccx_rpt_result = CCX_FAIL;

	/*Get CLM result*/
	if (halbb_clm_get_result(bb)) {
		rpt->clm_ratio = env->clm_ratio;
		rpt->clm_result = env->clm_result;
		rpt->ccx_rpt_result |= CLM_SUCCESS;
	} else {
		rpt->clm_ratio = ENV_MNTR_FAIL_BYTE;
		rpt->clm_result = ENV_MNTR_FAIL_WORD;
	}

	/*Get NHM result*/
	if (halbb_nhm_get_result(bb)) {
		rpt->nhm_ratio = env->nhm_ratio;
		rpt->nhm_tx_ratio = env->nhm_tx_ratio;
		rpt->nhm_cca_ratio = env->nhm_cca_ratio;
		rpt->nhm_idle_ratio = env->nhm_idle_ratio;
		rpt->nhm_tx_cnt = env->nhm_tx_cnt;
		rpt->nhm_cca_cnt = env->nhm_cca_cnt;
		rpt->nhm_idle_cnt = env->nhm_idle_cnt;
		rpt->nhm_pwr = env->nhm_pwr;
		rpt->nhm_pwr_0p5 = env->nhm_pwr_0p5;
		rpt->ccx_rpt_result |= NHM_SUCCESS;
		halbb_mem_cpy(bb, &rpt->nhm_rpt[0], &env->nhm_rpt[0],
			      NHM_RPT_NUM);
	} else if (env->nhm_app != NHM_FW_CTRL) {
		rpt->nhm_ratio = ENV_MNTR_FAIL_BYTE;
		rpt->nhm_tx_ratio = ENV_MNTR_FAIL_BYTE;
		rpt->nhm_cca_ratio = ENV_MNTR_FAIL_BYTE;
		rpt->nhm_idle_ratio = ENV_MNTR_FAIL_BYTE;
		rpt->nhm_pwr = ENV_MNTR_FAIL_BYTE;
		rpt->nhm_pwr_0p5 = ENV_MNTR_FAIL_BYTE;
		for (i = 0; i < NHM_RPT_NUM; i++)
			rpt->nhm_rpt[i] = ENV_MNTR_FAIL_BYTE;
	}

	/*Get IFS_CLM result*/
	if (halbb_ifs_clm_get_result(bb)) {
		rpt->ifs_clm_tx_ratio = env->ifs_clm_tx_ratio;
		rpt->ifs_clm_edcca_excl_cca_ratio = env->ifs_clm_edcca_excl_cca_ratio;
		rpt->ifs_clm_cck_fa_ratio = env->ifs_clm_cck_fa_ratio;
		rpt->ifs_clm_ofdm_fa_ratio = env->ifs_clm_ofdm_fa_ratio;
		rpt->ifs_clm_cck_cca_excl_fa_ratio = env->ifs_clm_cck_cca_excl_fa_ratio;
		rpt->ifs_clm_ofdm_cca_excl_fa_ratio = env->ifs_clm_ofdm_cca_excl_fa_ratio;
		rpt->ifs_clm_cck_fa_permil = env->ifs_clm_cck_fa_permil;
		rpt->ifs_clm_ofdm_fa_permil = env->ifs_clm_ofdm_fa_permil;
		rpt->ifs_clm_total_ifs = env->ifs_clm_total_ifs;
		for (i = 0; i < ifs_clm_num; i++) {
			rpt->ifs_clm_his[i] = env->ifs_clm_his[i];
			rpt->ifs_clm_ifs_avg[i] = env->ifs_clm_ifs_avg[i];
			rpt->ifs_clm_cca_avg[i] = env->ifs_clm_cca_avg[i];
		}
		rpt->ccx_rpt_result |= IFS_CLM_SUCCESS;
	} else {
		rpt->ifs_clm_tx_ratio = ENV_MNTR_FAIL_BYTE;
		rpt->ifs_clm_edcca_excl_cca_ratio = ENV_MNTR_FAIL_BYTE;
		rpt->ifs_clm_cck_fa_ratio = ENV_MNTR_FAIL_BYTE;
		rpt->ifs_clm_ofdm_fa_ratio = ENV_MNTR_FAIL_BYTE;
		rpt->ifs_clm_cck_cca_excl_fa_ratio = ENV_MNTR_FAIL_BYTE;
		rpt->ifs_clm_ofdm_cca_excl_fa_ratio = ENV_MNTR_FAIL_BYTE;
		rpt->ifs_clm_cck_fa_permil = ENV_MNTR_FAIL_WORD;
		rpt->ifs_clm_ofdm_fa_permil = ENV_MNTR_FAIL_WORD;
		rpt->ifs_clm_total_ifs = ENV_MNTR_FAIL_WORD;
		for (i = 0; i < ifs_clm_num; i++) {
			rpt->ifs_clm_his[i] = ENV_MNTR_FAIL_WORD;
			rpt->ifs_clm_ifs_avg[i] = ENV_MNTR_FAIL_DWORD;
			rpt->ifs_clm_cca_avg[i] = ENV_MNTR_FAIL_DWORD;
		}
	}

	/*Get EDCCA_CLM result*/
	if (halbb_edcca_clm_get_result(bb)) {
		rpt->edcca_clm_ratio = env->edcca_clm_ratio;
		rpt->ccx_rpt_result |= EDCCA_CLM_SUCCESS;
	} else {
		rpt->edcca_clm_ratio = ENV_MNTR_FAIL_BYTE;
	}

	rpt->ccx_rpt_stamp = env->ccx_rpt_stamp;

	BB_DBG(bb, DBG_ENV_MNTR, "ccx_rpt_stamp=%d, ccx_rpt_result=0x%x\n",
	       rpt->ccx_rpt_stamp, rpt->ccx_rpt_result);

	/*update bg structure*/
	if (halbb_env_mntr_bg_app_chk(bb)) {
		if (env->nhm_app == NHM_FW_CTRL)
			halbb_nhm_get_fw_result_h2c(bb);

		halbb_mem_cpy(bb, &env->env_mntr_rpt_bg, rpt,
			      sizeof(struct env_mntr_rpt));
	}

	halbb_ccx_racing_release(bb, rpt->ccx_rpt_result);

	return rpt->ccx_rpt_result;
}

void halbb_env_mntr_fw_ctrl(struct bb_info *bb, u8 func_sel, u8 pause_type)
{
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;

	BB_DBG(bb, DBG_ENV_MNTR, "[%s], func_sel=%d, pause_type=%d\n",
	       __func__, func_sel, pause_type);

	if (func_sel == NHM_SEL) {
		if (pause_type == HALBB_PAUSE_NO_SET) {
			env->nhm_app = NHM_FW_CTRL;
			env->nhm_include_cca = NHM_CCA_INIT;
			env->nhm_mntr_time = 0;
			if ((bb->ic_type != BB_RTL8852A) &&
			    (bb->ic_type != BB_RTL8852B)) {
				env->nhm_nav_en = NHM_NAV_EN_INIT;
				env->nhm_rssi_th_en = NHM_RSSI_TH_EN_INIT;
				env->nhm_rssi_th = 0;
			}
		} else if (pause_type == HALBB_RESUME_NO_RECOVERY) {
			env->nhm_app = NHM_INIT;
		}
	}
}

/*Environment Monitor*/
bool
halbb_env_mntr_watchdog_chk(struct bb_info *bb, bool is_chk_fahm)
{
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;
	u32 sys_return_time = 0;
	bool chk_result = HALBB_SET_FAIL;

	if (is_chk_fahm) {
		sys_return_time = env->fahm_trigger_time + MAX_ENV_MNTR_TIME;
		if ((env->fahm_app != FAHM_BACKGROUND) &&
		    (env->fahm_app != FAHM_INIT) &&
		    (sys_return_time > bb->bb_sys_up_time)) {
			BB_DBG(bb, DBG_ENV_MNTR, "FAHM_app = %d\n",
			       env->fahm_app);
			BB_DBG(bb, DBG_ENV_MNTR,
			       "fahm_trigger_time=%d, fahm_sys_time=%d\n",
			       env->fahm_trigger_time, bb->bb_sys_up_time);
		} else {
			chk_result = HALBB_SET_SUCCESS;
		}
	} else {
		sys_return_time = env->ccx_trigger_time + MAX_ENV_MNTR_TIME;
		if ((!halbb_env_mntr_bg_app_chk(bb)) &&
		    (!halbb_env_mntr_init_app_chk(bb)) &&
		    (sys_return_time > bb->bb_sys_up_time)) {
			BB_DBG(bb, DBG_ENV_MNTR,
			       "APP:{CLM, NHM, IFS_CLM, EDCCA} = {%d, %d, %d, %d}\n",
			       env->clm_app, env->nhm_app, env->ifs_clm_app,
			       env->edcca_clm_app);
			BB_DBG(bb, DBG_ENV_MNTR,
			       "trigger_time=%d, sys_time=%d\n",
			       env->ccx_trigger_time, bb->bb_sys_up_time);
		} else {
			chk_result = HALBB_SET_SUCCESS;
		}
	}
	return chk_result;
}

void halbb_idle_time_pwr_physts(struct bb_info *bb, struct physts_rxd *desc,
				bool is_cck_rate)
{
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;
	struct bb_physts_info	*physts = &bb->bb_physts_i;
	u16 idle_pwr = 0; /*u(8,1)*/

	if (is_cck_rate)
		idle_pwr = physts->bb_physts_rslt_0_i.avg_idle_noise_pwr_cck;
	else 
		idle_pwr = physts->bb_physts_rslt_1_i.avg_idle_noise_pwr;

	BB_DBG(bb, DBG_PHY_STS, "cck=%d, idle_pwr=%d.%d, nhm_pwr=%d\n",
	       is_cck_rate, idle_pwr >> 1, (idle_pwr & 1) * 5, env->nhm_pwr);

	if (idle_pwr > (env->nhm_pwr + 10) && env->nhm_pwr != 0)
		return;

	if (env->idle_pwr_physts != 0)
		env->idle_pwr_physts = MA_ACC(env->idle_pwr_physts, idle_pwr, 2,
					      RSSI_MA_L);
	else 
		env->idle_pwr_physts = idle_pwr << RSSI_MA_L;

	BB_DBG(bb, DBG_PHY_STS, "idle_pwr_physts=%d (%d.%03d)\n",
	       env->idle_pwr_physts, env->idle_pwr_physts >> 3,
	       (env->idle_pwr_physts & 0x7) * 125);
}

void halbb_env_mntr(struct bb_info *bb)
{
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;
	struct ccx_para_info para = {0};
	struct env_mntr_rpt rpt = {0};
	struct env_trig_rpt trig_rpt = {0};
	struct fahm_para_info fahm_para = {0};
	struct fahm_report fahm_rpt = {0};
	struct fahm_trig_report fahm_trig_rpt = {0};
	u8 chk_result = CCX_FAIL;
	bool fahm_chk_result = false;

	BB_DBG(bb, DBG_ENV_MNTR, "[%s]===>\n", __func__);

	env->ccx_watchdog_result = CCX_FAIL;
	env->fahm_watchdog_result = false;

	if (!(bb->support_ability & BB_ENVMNTR))
		return;

	if (env->ccx_manual_ctrl) {
		BB_DBG(bb, DBG_ENV_MNTR, "CCX in manual ctrl\n");
	} else if (halbb_env_mntr_watchdog_chk(bb, false)) {
		/*get result*/
		env->ccx_watchdog_result = halbb_env_mntr_result(bb, &rpt,
								 bb->bb_phy_idx);

		/*set parameter*/
		para.mntr_time = 1900;
		para.rac_lv = RAC_LV_1;
		para.ccx_edcca_opt_sc_idx = CCX_EDCCA_P0;

		para.clm_app = CLM_BACKGROUND;
		if (bb->ic_type & BB_IC_AX_SERIES)
			para.clm_input_opt = CLM_CCA_S80_S40_S20;
		else
			para.clm_input_opt = BE_CLM_CCA_S160_S80_S40_S20;

		para.nhm_app = NHM_BACKGROUND;
		para.nhm_incld_cca = NHM_EXCLUDE_CCA;

		para.ifs_clm_app = IFS_CLM_BACKGROUND;

		para.edcca_clm_app = EDCCA_CLM_BACKGROUND;

		if ((bb->ic_type != BB_RTL8852A) &&
		    (bb->ic_type != BB_RTL8852B) &&
		    (bb->ic_type != BB_RTL8851B)) {
			para.clm_nav_en = CLM_NAV_EN_DISABLED;
			para.clm_rssi_th_en = CLM_RSSI_TH_EN_DISABLED;

			para.nhm_nav_en = NHM_NAV_EN_DISABLED;
			para.nhm_rssi_th_en = NHM_RSSI_TH_EN_DISABLED;
		}

		chk_result = halbb_env_mntr_trigger(bb, &para, &trig_rpt,
						    bb->bb_phy_idx);
		BB_DBG(bb, DBG_ENV_MNTR,
		       "get_result=0x%x, chk_result:0x%x\n",
		       env->ccx_watchdog_result, chk_result);
	}

	if (env->fahm_manual_ctrl) {
		BB_DBG(bb, DBG_ENV_MNTR, "FAHM in manual ctrl\n");
	} else if (halbb_env_mntr_watchdog_chk(bb, true)) {
		/*get result*/
		env->fahm_watchdog_result = halbb_fahm_result(bb, &fahm_rpt);

		/*set parameter*/
		fahm_para.fahm_mntr_time = 1900;
		fahm_para.fahm_rac_lv = RAC_LV_1;

		fahm_para.fahm_app = FAHM_BACKGROUND;
		fahm_para.fahm_numer_opt = FAHM_INCLU_FA;
		fahm_para.fahm_denom_opt = FAHM_INCLU_CRC_ERR;

		fahm_chk_result = halbb_fahm_trigger(bb, &fahm_para,
						     &fahm_trig_rpt);

		BB_DBG(bb, DBG_ENV_MNTR,
		       "fahm_get_result=%d, fahm_chk_result:%d\n",
		       env->fahm_watchdog_result, fahm_chk_result);
	}

	BB_DBG(bb, DBG_ENV_MNTR, "CCX Summary:\n");
	halbb_env_mntr_log(bb, DBG_ENV_MNTR);
}

void halbb_env_mntr_init(struct bb_info *bb)
{
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;

	BB_DBG(bb, DBG_ENV_MNTR, "[%s]===>\n", __func__);

	halbb_ccx_top_setting_init(bb);
	halbb_clm_init(bb);
	halbb_nhm_init(bb);
	halbb_ifs_clm_init(bb);
	halbb_fahm_init(bb);
	halbb_edcca_clm_init(bb);

	#ifdef HALBB_INIT_FW_NHM_EN
	halbb_env_mntr_fw_ctrl(bb, NHM_SEL, HALBB_PAUSE_NO_SET);
	#endif

	env->idle_pwr_physts= 0;
}

void halbb_env_mntr_dbg_show_rpt(struct bb_info *bb, u32 *_used, char *output,
				 u32 *_out_len, struct env_mntr_rpt *rpt,
				 struct fahm_report *fahm_rpt, bool is_show_rpt,
				 bool is_show_nhm_rpt, bool is_show_fahm_rpt,
				 bool is_show_extloss)
{
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;
	struct bb_ch_info *ch = &bb->bb_ch_i;
	u8 i = 0;
	char ext_loss[HALBB_MAX_PATH][HALBB_SNPRINT_SIZE];
	char ccx_ext_loss[HALBB_SNPRINT_SIZE];
	u8 ifs_clm_num = 0;

	if (bb->ic_type & BB_IC_AX_SERIES)
		ifs_clm_num = IFS_CLM_NUM;
	else
		ifs_clm_num = BE_IFS_CLM_NUM;

	if (is_show_rpt) {
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "[CLM/IFS_CLM/EDCCA_CLM rpt]ccx_rpt_stamp = %d\n", rpt->ccx_rpt_stamp);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "{Tx, CCA_sec, EDCCA_p20, EDCCA_exclu_CCA} = {%d, %d, %d, %d} %%\n",
			    rpt->ifs_clm_tx_ratio, rpt->clm_ratio,
			    rpt->edcca_clm_ratio,
			    rpt->ifs_clm_edcca_excl_cca_ratio);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "FA{CCK, OFDM}={%d, %d} %%, CCA_exclu_FA{CCK, OFDM}={%d, %d} %%\n",
			    rpt->ifs_clm_cck_fa_ratio,
			    rpt->ifs_clm_ofdm_fa_ratio,
			    rpt->ifs_clm_cck_cca_excl_fa_ratio,
			    rpt->ifs_clm_ofdm_cca_excl_fa_ratio);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "[IFS] Time(us):[his, ifs_avg(us), cca_avg(us)], total cnt=%d\n",
			    rpt->ifs_clm_total_ifs);
		for (i = 0; i < ifs_clm_num; i++)
			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used,
				    " *[%d](%04d~%04d):[%03d,     %04d,     %04d]\n",
				    i + 1,
				    halbb_ccx_idx_cnt_2_us(bb,
							   env->ifs_clm_th_l[i]),
				    halbb_ccx_idx_cnt_2_us(bb,
							   env->ifs_clm_th_h[i]),
				    rpt->ifs_clm_his[i], rpt->ifs_clm_ifs_avg[i],
				    rpt->ifs_clm_cca_avg[i]);
	}
	if (is_show_nhm_rpt) {
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "[NHM]{Tx, Idle, CCA_p20} = {%d, %d, %d} %%, nhm_ratio = %d %%\n",
			    rpt->nhm_tx_ratio, rpt->nhm_idle_ratio,
			    rpt->nhm_cca_ratio, rpt->nhm_ratio);
		if ((bb->ic_type == BB_RTL8852A) ||
		    (bb->ic_type == BB_RTL8852B) ||
		    (bb->ic_type == BB_RTL8852C)) {
			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used,
				    "%-18s[%.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d]\n",
				    "  Th", NHM_TH_2_RSSI(env->nhm_th[9]),
				    NHM_TH_2_RSSI(env->nhm_th[8]),
				    NHM_TH_2_RSSI(env->nhm_th[7]),
				    NHM_TH_2_RSSI(env->nhm_th[6]),
				    NHM_TH_2_RSSI(env->nhm_th[5]),
				    NHM_TH_2_RSSI(env->nhm_th[4]),
				    NHM_TH_2_RSSI(env->nhm_th[3]),
				    NHM_TH_2_RSSI(env->nhm_th[2]),
				    NHM_TH_2_RSSI(env->nhm_th[1]),
				    NHM_TH_2_RSSI(env->nhm_th[0]));
			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used,
				    "[NHM]  (pwr:%02d.%d)[%.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d]\n",
				    rpt->nhm_pwr, 5 * (env->nhm_pwr_0p5 & 0x1),
				    rpt->nhm_rpt[10], rpt->nhm_rpt[9],
				    rpt->nhm_rpt[8], rpt->nhm_rpt[7],
				    rpt->nhm_rpt[6], rpt->nhm_rpt[5],
				    rpt->nhm_rpt[4], rpt->nhm_rpt[3],
				    rpt->nhm_rpt[2], rpt->nhm_rpt[1],
				    rpt->nhm_rpt[0]);
		} else {
			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used,
				    "%-18s[%.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d]\n",
				    "  Th", NHM_TH_2_RSSI(env->nhm_th[10]),
				    NHM_TH_2_RSSI(env->nhm_th[9]),
				    NHM_TH_2_RSSI(env->nhm_th[8]),
				    NHM_TH_2_RSSI(env->nhm_th[7]),
				    NHM_TH_2_RSSI(env->nhm_th[6]),
				    NHM_TH_2_RSSI(env->nhm_th[5]),
				    NHM_TH_2_RSSI(env->nhm_th[4]),
				    NHM_TH_2_RSSI(env->nhm_th[3]),
				    NHM_TH_2_RSSI(env->nhm_th[2]),
				    NHM_TH_2_RSSI(env->nhm_th[1]),
				    NHM_TH_2_RSSI(env->nhm_th[0]));
			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used,
				    "[NHM]  (pwr:%02d.%d)[%.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d]\n",
				    rpt->nhm_pwr, 5 * (env->nhm_pwr_0p5 & 0x1),
				    rpt->nhm_rpt[11], rpt->nhm_rpt[10],
				    rpt->nhm_rpt[9], rpt->nhm_rpt[8],
				    rpt->nhm_rpt[7], rpt->nhm_rpt[6],
				    rpt->nhm_rpt[5], rpt->nhm_rpt[4],
				    rpt->nhm_rpt[3], rpt->nhm_rpt[2],
				    rpt->nhm_rpt[1], rpt->nhm_rpt[0]);
		}
	}
	if (is_show_fahm_rpt) {
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "[FAHM]fahm_rpt_stamp = %d\n", fahm_rpt->fahm_rpt_stamp);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "{FA, CRC_err} = {%d, %d} %%\n",
			    fahm_rpt->fahm_ratio, fahm_rpt->fahm_denom_ratio);
		if ((bb->ic_type == BB_RTL8852A) ||
		    (bb->ic_type == BB_RTL8852B) ||
		    (bb->ic_type == BB_RTL8852C)) {
			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used,
				    "%-18s[%.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d]\n",
				    "  Th", FAHM_TH_2_RSSI(env->fahm_th[9]),
				    FAHM_TH_2_RSSI(env->fahm_th[8]),
				    FAHM_TH_2_RSSI(env->fahm_th[7]),
				    FAHM_TH_2_RSSI(env->fahm_th[6]),
				    FAHM_TH_2_RSSI(env->fahm_th[5]),
				    FAHM_TH_2_RSSI(env->fahm_th[4]),
				    FAHM_TH_2_RSSI(env->fahm_th[3]),
				    FAHM_TH_2_RSSI(env->fahm_th[2]),
				    FAHM_TH_2_RSSI(env->fahm_th[1]),
				    FAHM_TH_2_RSSI(env->fahm_th[0]));
			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used,
				    "[FAHM] (pwr:%02d.%d)[%.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d]\n",
				    fahm_rpt->fahm_pwr,
				    5 * (env->fahm_pwr_0p5 & 0x1),
				    fahm_rpt->fahm_rpt[10],
				    fahm_rpt->fahm_rpt[9],
				    fahm_rpt->fahm_rpt[8],
				    fahm_rpt->fahm_rpt[7],
				    fahm_rpt->fahm_rpt[6],
				    fahm_rpt->fahm_rpt[5],
				    fahm_rpt->fahm_rpt[4],
				    fahm_rpt->fahm_rpt[3],
				    fahm_rpt->fahm_rpt[2],
				    fahm_rpt->fahm_rpt[1],
				    fahm_rpt->fahm_rpt[0]);
		} else {
			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used,
				    "%-18s[%.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d]\n",
				    "  Th", FAHM_TH_2_RSSI(env->fahm_th[10]),
				    FAHM_TH_2_RSSI(env->fahm_th[9]),
				    FAHM_TH_2_RSSI(env->fahm_th[8]),
				    FAHM_TH_2_RSSI(env->fahm_th[7]),
				    FAHM_TH_2_RSSI(env->fahm_th[6]),
				    FAHM_TH_2_RSSI(env->fahm_th[5]),
				    FAHM_TH_2_RSSI(env->fahm_th[4]),
				    FAHM_TH_2_RSSI(env->fahm_th[3]),
				    FAHM_TH_2_RSSI(env->fahm_th[2]),
				    FAHM_TH_2_RSSI(env->fahm_th[1]),
				    FAHM_TH_2_RSSI(env->fahm_th[0]));
			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used,
				    "[FAHM] (pwr:%02d.%d)[%.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d]\n",
				    fahm_rpt->fahm_pwr,
				    5 * (env->fahm_pwr_0p5 & 0x1),
				    fahm_rpt->fahm_rpt[11],
				    fahm_rpt->fahm_rpt[10],
				    fahm_rpt->fahm_rpt[9],
				    fahm_rpt->fahm_rpt[8],
				    fahm_rpt->fahm_rpt[7],
				    fahm_rpt->fahm_rpt[6],
				    fahm_rpt->fahm_rpt[5],
				    fahm_rpt->fahm_rpt[4],
				    fahm_rpt->fahm_rpt[3],
				    fahm_rpt->fahm_rpt[2],
				    fahm_rpt->fahm_rpt[1],
				    fahm_rpt->fahm_rpt[0]);
		}
	}

	if (is_show_extloss) {
		for (i = 0; i < HALBB_MAX_PATH; i++)
			halbb_print_sign_frac_digit(bb, ch->ext_loss[i],
						    8, 2, ext_loss[i],
						    HALBB_SNPRINT_SIZE);

		halbb_print_sign_frac_digit(bb, ch->ext_loss_avg, 8, 2,
					    bb->dbg_buf, HALBB_SNPRINT_SIZE);
		halbb_print_sign_frac_digit(bb, env->ccx_ext_loss, 8, 1,
					    ccx_ext_loss, HALBB_SNPRINT_SIZE);

		if (bb->num_rf_path >= 4) {
			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used,
				    "ext_loss{avg, a, b, c, d} = {%s, %s, %s, %s, %s} dB, ccx_ext_loss = %s dB\n",
				    bb->dbg_buf, ext_loss[0], ext_loss[1],
				    ext_loss[2], ext_loss[3], ccx_ext_loss);
		} else if (bb->num_rf_path >= 2) {
			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used,
				    "ext_loss{avg, a, b} = {%s, %s, %s} dB, ccx_ext_loss = %s dB\n",
				    bb->dbg_buf, ext_loss[0], ext_loss[1],
				    ccx_ext_loss);
		} else {
			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used,
				    "ext_loss=%s dB, ccx_ext_loss = %s dB\n",
				    ext_loss[0], ccx_ext_loss);
		}
	}
}

void halbb_env_mntr_dbg_bg_result(struct bb_info *bb, u32 *_used, char *output,
				  u32 *_out_len)
{
	struct env_mntr_rpt rpt = {0};
	struct fahm_report fahm_rpt = {0};
	bool is_show_rpt = false;
	bool is_show_nhm_rpt = false;
	bool is_show_fahm_rpt = false;
	bool is_show_extloss = false;

	halbb_env_mntr_get_bg_result(bb, &rpt, bb->bb_phy_idx);
	halbb_fahm_get_bg_result(bb, &fahm_rpt);
	is_show_rpt = true;
	is_show_nhm_rpt = true;
	is_show_fahm_rpt = true;
	is_show_extloss = true;

	halbb_env_mntr_dbg_show_rpt(bb, _used, output, _out_len, &rpt,
				    &fahm_rpt, is_show_rpt, is_show_nhm_rpt,
				    is_show_fahm_rpt, is_show_extloss);
}

void halbb_env_mntr_dbg_bg_para(struct bb_info *bb, u32 *_used, char *output,
				u32 *_out_len)
{
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;
	struct ccx_para_info para = {0};
	struct fahm_para_info fahm_para = {0};

	halbb_env_mntr_get_bg_setting(bb, &para, HW_PHY_0);
	halbb_fahm_get_bg_setting(bb, &fahm_para);
	/*bg_para*/
	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "=== Background parameters ===\n");
	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "rac_lv = %d, edcca_opt_sc_idx = %d, mntr_time = %d\n",
		    para.rac_lv, para.ccx_edcca_opt_sc_idx,
		    para.mntr_time);
	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "fahm_rac_lv = %d, fahm_mntr_time = %d\n",
		    fahm_para.fahm_rac_lv, fahm_para.fahm_mntr_time);
	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "APP:{CLM, NHM, FAHM, IFS_CLM, EDCCA} = {%d, %d, %d, %d, %d}\n",
		    para.clm_app, para.nhm_app, fahm_para.fahm_app,
		    para.ifs_clm_app, para.edcca_clm_app);
	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "clm_input_opt = %d, nhm_inclu_cca = %d\n",
		    para.clm_input_opt, para.nhm_incld_cca);
	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "fahm_numer_opt = %d, fahm_denom_opt = %d\n",
		    fahm_para.fahm_numer_opt, fahm_para.fahm_denom_opt);
	/*curr_para*/
	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "=== Current parameters ===\n");
	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "rac_lv = %d, edcca_opt = %d\n",
		    env->ccx_rac_lv, env->ccx_edcca_opt);
	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "clm_mntr_time = %d, nhm_mntr_time = %d\n",
		    env->clm_mntr_time, env->nhm_mntr_time);
	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "ifs_clm_mntr_time = %d, edcca_clm_mntr_time = %d\n",
		    env->ifs_clm_mntr_time, env->edcca_clm_mntr_time);
	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "fahm_rac_lv = %d, fahm_mntr_time = %d\n",
		    env->fahm_rac_lv, env->fahm_mntr_time);
	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "APP:{CLM, NHM, FAHM, IFS_CLM, EDCCA} = {%d, %d, %d, %d, %d}\n",
		    env->clm_app, env->nhm_app, env->fahm_app,
		    env->ifs_clm_app, env->edcca_clm_app);
	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "clm_input_opt = %d, nhm_inclu_cca = %d\n",
		    env->clm_input_opt, env->nhm_include_cca);
	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "fahm_numer_opt = %d, fahm_denom_opt = %d\n",
		    env->fahm_numer_opt, env->fahm_denom_opt);
}

void halbb_env_mntr_dbg_trigger(struct bb_info *bb, u32 *_used, char *output,
				  u32 *_out_len, bool is_adv_trig,
				  u16 mntr_time)
{
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;
	struct ccx_para_info para = {0};
	struct env_trig_rpt trig_rpt = {0};
	struct fahm_para_info fahm_para = {0};
	struct fahm_trig_report fahm_trig_rpt = {0};
	u8 set_result = CCX_FAIL;
	bool fahm_set_result = false;

	env->ccx_manual_ctrl = true;
	env->fahm_manual_ctrl = true;

	if (is_adv_trig == false) {
		para.mntr_time = 1900;
		fahm_para.fahm_mntr_time = 1900;
	} else {
		para.mntr_time = mntr_time;
		fahm_para.fahm_mntr_time = mntr_time;
	}

	para.rac_lv = RAC_LV_4;
	fahm_para.fahm_rac_lv = RAC_LV_4;
	para.ccx_edcca_opt_sc_idx = CCX_EDCCA_P0;

	/*clm para*/
	para.clm_app = CLM_DBG;
	if (bb->ic_type & BB_IC_AX_SERIES)
		para.clm_input_opt = CLM_CCA_S80_S40_S20;
	else
		para.clm_input_opt = BE_CLM_CCA_S160_S80_S40_S20;

	/*nhm para*/
	para.nhm_app = NHM_DBG_11K;
	para.nhm_incld_cca = NHM_EXCLUDE_CCA;

	/*fahm para*/
	fahm_para.fahm_app = FAHM_DBG_11K;
	fahm_para.fahm_numer_opt = FAHM_INCLU_FA;
	fahm_para.fahm_denom_opt = FAHM_INCLU_CRC_ERR;

	/*ifs_clm para*/
	para.ifs_clm_app = IFS_CLM_DBG;

	/*edcca_clm para*/
	para.edcca_clm_app = EDCCA_CLM_DBG;

	set_result = halbb_env_mntr_trigger(bb, &para, &trig_rpt,
		    bb->bb_phy_idx);

	fahm_set_result = halbb_fahm_trigger(bb, &fahm_para,
					     &fahm_trig_rpt);

	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "Set Result=0x%x, ccx_rpt_stamp=%d\n",
		    set_result, trig_rpt.ccx_rpt_stamp);

	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "fahm set Result=%d, fahm_rpt_stamp=%d\n",
		    fahm_set_result, fahm_trig_rpt.fahm_rpt_stamp);
}

void halbb_env_mntr_dbg_get_result(struct bb_info *bb, u32 *_used, char *output,
				  u32 *_out_len)
{
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;
	struct env_mntr_rpt rpt = {0};
	struct fahm_report fahm_rpt = {0};
	u8 set_result = CCX_FAIL;
	bool fahm_set_result = false;
	bool is_show_rpt = false;
	bool is_show_nhm_rpt = false;
	bool is_show_fahm_rpt = false;
	bool is_show_extloss = false;

	set_result = halbb_env_mntr_result(bb, &rpt, bb->bb_phy_idx);
	fahm_set_result = halbb_fahm_result(bb, &fahm_rpt);

	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "CCX get Result=0x%x, ccx_rpt_stamp=%d\n",
		    set_result, rpt.ccx_rpt_stamp);

	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "fahm get Result=%d, fahm_rpt_stamp=%d\n",
		    fahm_set_result, env->fahm_rpt_stamp);

	if (set_result & CCX_SEL_EXCLU_NHM_FAHM)
		is_show_rpt = true;
	else
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used, "Get CCX_rpt all Fail\n");

	if (set_result & NHM_SEL)
		is_show_nhm_rpt = true;
	else
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used, "Get NHM_rpt Fail\n");

	if (fahm_set_result)
		is_show_fahm_rpt = true;
	else
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used, "Get FAHM_rpt Fail\n");

	if (set_result || fahm_set_result)
		is_show_extloss = true;

	env->ccx_manual_ctrl = false;
	env->fahm_manual_ctrl = false;

	halbb_env_mntr_dbg_show_rpt(bb, _used, output, _out_len, &rpt,
				    &fahm_rpt, is_show_rpt, is_show_nhm_rpt,
				    is_show_fahm_rpt, is_show_extloss);
}

void halbb_env_mntr_dbg(struct bb_info *bb, char input[][16], u32 *_used,
			char *output, u32 *_out_len)
{
	char help[] = "-h";
	u32 var[10] = {0};
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;
	u8 i = 0;
	bool is_fw_ctrl = false;
	u8 func_sel = 0;

	for (i = 0; i < 3; i++) {
		if (input[i + 1])
			HALBB_SCAN(input[i + 1], DCMD_DECIMAL, &var[i]);
	}

	if ((_os_strcmp(input[1], help) == 0)) {
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "Basic-Trigger(1900ms): {1}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "Adv-Trigger: {2} {0~2097ms}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "FW-ctrl: {3} {en} {func_sel, bit0~bit3:nhm/clm/ifs_clm/edcca_clm}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "Get Result: {100}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "Get Background Result: {101}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "Get Background parameter: {102}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "Show noise {noise}\n");
	} else if (var[0] == 100) { /* Get results */
		halbb_env_mntr_dbg_get_result(bb, _used, output, _out_len);
	} else if (var[0] == 101) { /* Get bg results */
		halbb_env_mntr_dbg_bg_result(bb, _used, output, _out_len);
	} else if (var[0] == 102) { /* Get bg paramters */
		halbb_env_mntr_dbg_bg_para(bb, _used, output, _out_len);
	} else if (_os_strcmp(input[1], "noise") == 0) {
		/*This command is *_used for customers, do not modify it arbitrarily*/
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "noise=%d(dBm)\n", env->nhm_pwr - 110);
	} else if (var[0] == 3) { /* fw-ctrl */
		is_fw_ctrl = (bool)var[1];
		func_sel = (u8)var[2];
		if (is_fw_ctrl)
			halbb_env_mntr_fw_ctrl(bb, func_sel, HALBB_PAUSE_NO_SET);
		else
			halbb_env_mntr_fw_ctrl(bb, func_sel, HALBB_RESUME_NO_RECOVERY);

		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "func_sel=%d, ccx_fw_ctrl=%d\n",
			    func_sel, is_fw_ctrl);
	} else if (var[0] == 1) {
		halbb_env_mntr_dbg_trigger(bb, _used, output, _out_len,
					   false, (u16)var[1]);
	} else if (var[0] == 2) {
		halbb_env_mntr_dbg_trigger(bb, _used, output, _out_len, true,
					   (u16)var[1]);
	}
}

u8 halbb_env_mntr_get_802_11_k_rsni(struct bb_info *bb, s8 rcpi, s8 anpi)
{
	u8 rsni = 0;
	u8 signal = 0;
	u8 sig_to_rsni[13] = {0, 8, 15, 20, 24, 27, 30, 32, 35, 37, 39, 41, 43};

	/*rcpi = signal + noise + interference = rssi*/
	/*anpi = noise + interferecne = nhm*/
	/*signal = rcpi - anpi*/

	/*rsni = 2*(10*log10((rcpi_lin/anpi_lin)-1)+10), unit = 0.5dB*/
	/*rcpi_lin/anpi_lin=10^((rcpi_dB-anpi_db)/10)*/
	/*rsni is approximated as 2*((rcpi_db-anpi_db)+10) when signal >= 13*/

	if (rcpi <= anpi)
		signal = 0;
	else if (rcpi - anpi >= 117)
		signal = 117;
	else
		signal = rcpi - anpi;

	if (signal < 13)
		rsni = sig_to_rsni[signal];
	else
		rsni = 2 * (signal + 10);

	return rsni;
}

void halbb_cr_cfg_env_mntr_init(struct bb_info *bb)
{
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;
	struct bb_env_mntr_cr_info *cr = &env->bb_env_mntr_cr_i;

	switch (bb->cr_type) {

	#ifdef HALBB_COMPILE_AP_SERIES
	case BB_AP:
		cr->ccx_en = CCX_EN_A;
		cr->ccx_en_m = CCX_EN_A_M;
		cr->ccx_trig_opt = CCX_TRIG_OPT_A;
		cr->ccx_trig_opt_m = CCX_TRIG_OPT_A_M;
		cr->ccx_trig = MEASUREMENT_TRIG_A;
		cr->ccx_trig_m = MEASUREMENT_TRIG_A_M;
		cr->ccx_edcca_opt = CCX_EDCCA_OPT_A;
		cr->ccx_edcca_opt_m = CCX_EDCCA_OPT_A_M;
		cr->ccx_source_sel = CCX_SOURCE_SEL_A;
		cr->ccx_source_sel_m = CCX_SOURCE_SEL_A_M;
		cr->clm_unit_idx = CLM_COUNTER_UNIT_A;
		cr->clm_unit_idx_m = CLM_COUNTER_UNIT_A_M;
		cr->clm_en = CLM_EN_A;
		cr->clm_en_m = CLM_EN_A_M;
		cr->clm_opt = CLM_CCA_OPT_A;
		cr->clm_opt_m = CLM_CCA_OPT_A_M;
		cr->clm_period = CLM_PERIOD_A;
		cr->clm_period_m = CLM_PERIOD_A_M;
		cr->clm_dbg_sel = CLM_FROM_DBG_SEL_A;
		cr->clm_dbg_sel_m = CLM_FROM_DBG_SEL_A_M;
		cr->clm_cnt = RO_CLM_RESULT_A;
		cr->clm_cnt_m = RO_CLM_RESULT_A_M;
		cr->clm_rdy = RO_CLM_RDY_A;
		cr->clm_rdy_m = RO_CLM_RDY_A_M;
		cr->edcca_clm_period = CLM_EDCCA_PERIOD_A;
		cr->edcca_clm_period_m = CLM_EDCCA_PERIOD_A_M;
		cr->edcca_clm_unit_idx = CLM_EDCCA_COUNTER_UNIT_A;
		cr->edcca_clm_unit_idx_m = CLM_EDCCA_COUNTER_UNIT_A_M;
		cr->edcca_clm_en = CLM_EDCCA_EN_A;
		cr->edcca_clm_en_m = CLM_EDCCA_EN_A_M;
		cr->edcca_clm_cnt = RO_CLM_EDCCA_RESULT_A;
		cr->edcca_clm_cnt_m = RO_CLM_EDCCA_RESULT_A_M;
		cr->edcca_clm_rdy = RO_CLM_EDCCA_RDY_A;
		cr->edcca_clm_rdy_m = RO_CLM_EDCCA_RDY_A_M;
		cr->nhm_en = NHM_EN_A;
		cr->nhm_en_m = NHM_EN_A_M;
		cr->nhm_method_sel = NHM_PWDB_METHOD_SEL_A;
		cr->nhm_method_sel_m = NHM_PWDB_METHOD_SEL_A_M;
		cr->nhm_period = NHM_PERIOD_A;
		cr->nhm_period_m = NHM_PERIOD_A_M;
		cr->nhm_unit_idx = NHM_COUNTER_UNIT_A;
		cr->nhm_unit_idx_m = NHM_COUNTER_UNIT_A_M;
		cr->nhm_inclu_cca = NHM_IGNORE_CCA_A;
		cr->nhm_inclu_cca_m = NHM_IGNORE_CCA_A_M;
		cr->nhm_th0 = NHM_TH0_A;
		cr->nhm_th0_m = NHM_TH0_A_M;
		cr->nhm_th1 = NHM_TH1_A;
		cr->nhm_th1_m = NHM_TH1_A_M;
		cr->nhm_th2 = NHM_TH2_A;
		cr->nhm_th2_m = NHM_TH2_A_M;
		cr->nhm_th3 = NHM_TH3_A;
		cr->nhm_th3_m = NHM_TH3_A_M;
		cr->nhm_th4 = NHM_TH4_A;
		cr->nhm_th4_m = NHM_TH4_A_M;
		cr->nhm_th5 = NHM_TH5_A;
		cr->nhm_th5_m = NHM_TH5_A_M;
		cr->nhm_th6 = NHM_TH6_A;
		cr->nhm_th6_m = NHM_TH6_A_M;
		cr->nhm_th7 = NHM_TH7_A;
		cr->nhm_th7_m = NHM_TH7_A_M;
		cr->nhm_th8 = NHM_TH8_A;
		cr->nhm_th8_m = NHM_TH8_A_M;
		cr->nhm_th9 = NHM_TH9_A;
		cr->nhm_th9_m = NHM_TH9_A_M;
		cr->nhm_th10 = NHM_TH10_A;
		cr->nhm_th10_m = NHM_TH10_A_M;
		cr->nhm_cnt0 = NHM_CNT0_A;
		cr->nhm_cnt0_m = NHM_CNT0_A_M;
		cr->nhm_cnt1 = NHM_CNT1_A;
		cr->nhm_cnt1_m = NHM_CNT1_A_M;
		cr->nhm_cnt2 = NHM_CNT2_A;
		cr->nhm_cnt2_m = NHM_CNT2_A_M;
		cr->nhm_cnt3 = NHM_CNT3_A;
		cr->nhm_cnt3_m = NHM_CNT3_A_M;
		cr->nhm_cnt4 = NHM_CNT4_A;
		cr->nhm_cnt4_m = NHM_CNT4_A_M;
		cr->nhm_cnt5 = NHM_CNT5_A;
		cr->nhm_cnt5_m = NHM_CNT5_A_M;
		cr->nhm_cnt6 = NHM_CNT6_A;
		cr->nhm_cnt6_m = NHM_CNT6_A_M;
		cr->nhm_cnt7 = NHM_CNT7_A;
		cr->nhm_cnt7_m = NHM_CNT7_A_M;
		cr->nhm_cnt8 = NHM_CNT8_A;
		cr->nhm_cnt8_m = NHM_CNT8_A_M;
		cr->nhm_cnt9 = NHM_CNT9_A;
		cr->nhm_cnt9_m = NHM_CNT9_A_M;
		cr->nhm_cnt10 = NHM_CNT10_A;
		cr->nhm_cnt10_m = NHM_CNT10_A_M;
		cr->nhm_cnt11 = NHM_CNT11_A;
		cr->nhm_cnt11_m = NHM_CNT11_A_M;
		cr->nhm_cca_cnt = NHM_CCA_CNT_A;
		cr->nhm_cca_cnt_m = NHM_CCA_CNT_A_M;
		cr->nhm_tx_cnt = NHM_TXON_CNT_A;
		cr->nhm_tx_cnt_m = NHM_TXON_CNT_A_M;
		cr->nhm_idle_cnt = NHM_IDLE_CNT_A;
		cr->nhm_idle_cnt_m = NHM_IDLE_CNT_A_M;
		cr->nhm_rdy = NHM_RDY_A;
		cr->nhm_rdy_m = NHM_RDY_A_M;
		cr->fahm_en = FAHM_EN_A;
		cr->fahm_en_m = FAHM_EN_A_M;
		cr->fahm_ofdm_en = FAHM_EN_OFDM_A;
		cr->fahm_ofdm_en_m = FAHM_EN_OFDM_A_M;
		cr->fahm_cck_en = FAHM_EN_CCK_A;
		cr->fahm_cck_en_m = FAHM_EN_CCK_A_M;
		cr->fahm_numer_opt = FAHM_NUM_CANDIDATE_A;
		cr->fahm_numer_opt_m = FAHM_NUM_CANDIDATE_A_M;
		cr->fahm_denom_opt = FAHM_DEN_CANDIDATE_A;
		cr->fahm_denom_opt_m = FAHM_DEN_CANDIDATE_A_M;
		cr->fahm_period = FAHM_PERIOD_A;
		cr->fahm_period_m = FAHM_PERIOD_A_M;
		cr->fahm_unit_idx = FAHM_COUNTER_UNIT_A;
		cr->fahm_unit_idx_m = FAHM_COUNTER_UNIT_A_M;
		cr->fahm_method_sel = FAHM_PWDB_SEL_A;
		cr->fahm_method_sel_m = FAHM_PWDB_SEL_A_M;
		cr->fahm_th0 = FAHM_TH0_A;
		cr->fahm_th0_m = FAHM_TH0_A_M;
		cr->fahm_th1 = FAHM_TH1_A;
		cr->fahm_th1_m = FAHM_TH1_A_M;
		cr->fahm_th2 = FAHM_TH2_A;
		cr->fahm_th2_m = FAHM_TH2_A_M;
		cr->fahm_th3 = FAHM_TH3_A;
		cr->fahm_th3_m = FAHM_TH3_A_M;
		cr->fahm_th4 = FAHM_TH4_A;
		cr->fahm_th4_m = FAHM_TH4_A_M;
		cr->fahm_th5 = FAHM_TH5_A;
		cr->fahm_th5_m = FAHM_TH5_A_M;
		cr->fahm_th6 = FAHM_TH6_A;
		cr->fahm_th6_m = FAHM_TH6_A_M;
		cr->fahm_th7 = FAHM_TH7_A;
		cr->fahm_th7_m = FAHM_TH7_A_M;
		cr->fahm_th8 = FAHM_TH8_A;
		cr->fahm_th8_m = FAHM_TH8_A_M;
		cr->fahm_th9 = FAHM_TH9_A;
		cr->fahm_th9_m = FAHM_TH9_A_M;
		cr->fahm_th10 = FAHM_TH10_A;
		cr->fahm_th10_m = FAHM_TH10_A_M;
		cr->fahm_dis_count_each_mpdu = FAHM_DIS_COUNT_EACH_MPDU_A;
		cr->fahm_dis_count_each_mpdu_m = FAHM_DIS_COUNT_EACH_MPDU_A_M;
		cr->fahm_cnt0 = RO_FAHM_NUM0_A;
		cr->fahm_cnt0_m = RO_FAHM_NUM0_A_M;
		cr->fahm_cnt1 = RO_FAHM_NUM1_A;
		cr->fahm_cnt1_m = RO_FAHM_NUM1_A_M;
		cr->fahm_cnt2 = RO_FAHM_NUM2_A;
		cr->fahm_cnt2_m = RO_FAHM_NUM2_A_M;
		cr->fahm_cnt3 = RO_FAHM_NUM3_A;
		cr->fahm_cnt3_m = RO_FAHM_NUM3_A_M;
		cr->fahm_cnt4 = RO_FAHM_NUM4_A;
		cr->fahm_cnt4_m = RO_FAHM_NUM4_A_M;
		cr->fahm_cnt5 = RO_FAHM_NUM5_A;
		cr->fahm_cnt5_m = RO_FAHM_NUM5_A_M;
		cr->fahm_cnt6 = RO_FAHM_NUM6_A;
		cr->fahm_cnt6_m = RO_FAHM_NUM6_A_M;
		cr->fahm_cnt7 = RO_FAHM_NUM7_A;
		cr->fahm_cnt7_m = RO_FAHM_NUM7_A_M;
		cr->fahm_cnt8 = RO_FAHM_NUM8_A;
		cr->fahm_cnt8_m = RO_FAHM_NUM8_A_M;
		cr->fahm_cnt9 = RO_FAHM_NUM9_A;
		cr->fahm_cnt9_m = RO_FAHM_NUM9_A_M;
		cr->fahm_cnt10 = RO_FAHM_NUM10_A;
		cr->fahm_cnt10_m = RO_FAHM_NUM10_A_M;
		cr->fahm_cnt11 = RO_FAHM_NUM11_A;
		cr->fahm_cnt11_m = RO_FAHM_NUM11_A_M;
		cr->fahm_denom_cnt = RO_FAHM_DEN_A;
		cr->fahm_denom_cnt_m = RO_FAHM_DEN_A_M;
		cr->fahm_rdy = RO_FAHM_RDY_A;
		cr->fahm_rdy_m = RO_FAHM_RDY_A_M;
		cr->ifs_clm_en = IFS_COLLECT_EN_A;
		cr->ifs_clm_en_m = IFS_COLLECT_EN_A_M;
		cr->ifs_clm_clr = IFS_COUNTER_CLR_A;
		cr->ifs_clm_clr_m = IFS_COUNTER_CLR_A_M;
		cr->ifs_clm_period = IFS_COLLECT_TOTAL_TIME_A;
		cr->ifs_clm_period_m = IFS_COLLECT_TOTAL_TIME_A_M;
		cr->ifs_clm_unit_idx = IFS_COUNTER_UNIT_A;
		cr->ifs_clm_unit_idx_m = IFS_COUNTER_UNIT_A_M;
		cr->ifs_t1_en = IFS_T1_EN_A;
		cr->ifs_t1_en_m = IFS_T1_EN_A_M;
		cr->ifs_t2_en = IFS_T2_EN_A;
		cr->ifs_t2_en_m = IFS_T2_EN_A_M;
		cr->ifs_t3_en = IFS_T3_EN_A;
		cr->ifs_t3_en_m = IFS_T3_EN_A_M;
		cr->ifs_t4_en = IFS_T4_EN_A;
		cr->ifs_t4_en_m = IFS_T4_EN_A_M;
		cr->ifs_t1_th_l = IFS_T1_TH_LOW_A;
		cr->ifs_t1_th_l_m = IFS_T1_TH_LOW_A_M;
		cr->ifs_t2_th_l = IFS_T2_TH_LOW_A;
		cr->ifs_t2_th_l_m = IFS_T2_TH_LOW_A_M;
		cr->ifs_t3_th_l = IFS_T3_TH_LOW_A;
		cr->ifs_t3_th_l_m = IFS_T3_TH_LOW_A_M;
		cr->ifs_t4_th_l = IFS_T4_TH_LOW_A;
		cr->ifs_t4_th_l_m = IFS_T4_TH_LOW_A_M;
		cr->ifs_t1_th_h = IFS_T1_TH_HIGH_A;
		cr->ifs_t1_th_h_m = IFS_T1_TH_HIGH_A_M;
		cr->ifs_t2_th_h = IFS_T2_TH_HIGH_A;
		cr->ifs_t2_th_h_m = IFS_T2_TH_HIGH_A_M;
		cr->ifs_t3_th_h = IFS_T3_TH_HIGH_A;
		cr->ifs_t3_th_h_m = IFS_T3_TH_HIGH_A_M;
		cr->ifs_t4_th_h = IFS_T4_TH_HIGH_A;
		cr->ifs_t4_th_h_m = IFS_T4_TH_HIGH_A_M;
		cr->ifs_clm_tx_cnt = IFSCNT_CNT_TX_A;
		cr->ifs_clm_tx_cnt_m = IFSCNT_CNT_TX_A_M;
		cr->ifs_clm_edcca_exclu_cca = IFSCNT_CNT_EDCCA_EXCLUDE_CCA_FA_A;
		cr->ifs_clm_edcca_exclu_cca_m = IFSCNT_CNT_EDCCA_EXCLUDE_CCA_FA_A_M;
		cr->ifs_clm_cckcca_exclu_fa = IFSCNT_CNT_CCKCCA_EXCLUDE_FA_A;
		cr->ifs_clm_cckcca_exclu_fa_m = IFSCNT_CNT_CCKCCA_EXCLUDE_FA_A_M;
		cr->ifs_clm_ofdmcca_exclu_fa = IFSCNT_CNT_OFDMCCA_EXCLUDE_FA_A;
		cr->ifs_clm_ofdmcca_exclu_fa_m = IFSCNT_CNT_OFDMCCA_EXCLUDE_FA_A_M;
		cr->ifs_clm_cck_fa = IFSCNT_CNT_CCKFA_A;
		cr->ifs_clm_cck_fa_m = IFSCNT_CNT_CCKFA_A_M;
		cr->ifs_clm_ofdm_fa = IFSCNT_CNT_OFDMFA_A;
		cr->ifs_clm_ofdm_fa_m = IFSCNT_CNT_OFDMFA_A_M;
		cr->ifs_clm_t1_his = IFS_T1_HIS_A;
		cr->ifs_clm_t1_his_m = IFS_T1_HIS_A_M;
		cr->ifs_clm_t2_his = IFS_T2_HIS_A;
		cr->ifs_clm_t2_his_m = IFS_T2_HIS_A_M;
		cr->ifs_clm_t3_his = IFS_T3_HIS_A;
		cr->ifs_clm_t3_his_m = IFS_T3_HIS_A_M;
		cr->ifs_clm_t4_his = IFS_T4_HIS_A;
		cr->ifs_clm_t4_his_m = IFS_T4_HIS_A_M;
		cr->ifs_clm_t1_avg = IFS_T1_AVG_A;
		cr->ifs_clm_t1_avg_m = IFS_T1_AVG_A_M;
		cr->ifs_clm_t2_avg = IFS_T2_AVG_A;
		cr->ifs_clm_t2_avg_m = IFS_T2_AVG_A_M;
		cr->ifs_clm_t3_avg = IFS_T3_AVG_A;
		cr->ifs_clm_t3_avg_m = IFS_T3_AVG_A_M;
		cr->ifs_clm_t4_avg = IFS_T4_AVG_A;
		cr->ifs_clm_t4_avg_m = IFS_T4_AVG_A_M;
		cr->ifs_clm_t1_cca = IFS_T1_CLM_A;
		cr->ifs_clm_t1_cca_m = IFS_T1_CLM_A_M;
		cr->ifs_clm_t2_cca = IFS_T2_CLM_A;
		cr->ifs_clm_t2_cca_m = IFS_T2_CLM_A_M;
		cr->ifs_clm_t3_cca = IFS_T3_CLM_A;
		cr->ifs_clm_t3_cca_m = IFS_T3_CLM_A_M;
		cr->ifs_clm_t4_cca = IFS_T4_CLM_A;
		cr->ifs_clm_t4_cca_m = IFS_T4_CLM_A_M;
		cr->ifs_total_cnt = IFS_TOTAL_A;
		cr->ifs_total_cnt_m = IFS_TOTAL_A_M;
		cr->ifs_clm_rdy = IFSCNT_DONE_A;
		cr->ifs_clm_rdy_m = IFSCNT_DONE_A_M;
		break;

	#endif
	#ifdef HALBB_COMPILE_CLIENT_SERIES
	case BB_CLIENT:
		cr->ccx_en = CCX_EN_C;
		cr->ccx_en_m = CCX_EN_C_M;
		cr->ccx_trig_opt = CCX_TRIG_OPT_C;
		cr->ccx_trig_opt_m = CCX_TRIG_OPT_C_M;
		cr->ccx_trig = MEASUREMENT_TRIG_C;
		cr->ccx_trig_m = MEASUREMENT_TRIG_C_M;
		cr->ccx_edcca_opt = CCX_EDCCA_OPT_C;
		cr->ccx_edcca_opt_m = CCX_EDCCA_OPT_C_M;
		cr->ccx_source_sel = CCX_SOURCE_SEL_C;
		cr->ccx_source_sel_m = CCX_SOURCE_SEL_C_M;
		cr->clm_unit_idx = CLM_COUNTER_UNIT_C;
		cr->clm_unit_idx_m = CLM_COUNTER_UNIT_C_M;
		cr->clm_en = CLM_EN_C;
		cr->clm_en_m = CLM_EN_C_M;
		cr->clm_opt = CLM_CCA_OPT_C;
		cr->clm_opt_m = CLM_CCA_OPT_C_M;
		cr->clm_period = CLM_PERIOD_C;
		cr->clm_period_m = CLM_PERIOD_C_M;
		cr->clm_dbg_sel = CLM_FROM_DBG_SEL_C;
		cr->clm_dbg_sel_m = CLM_FROM_DBG_SEL_C_M;
		cr->clm_cnt = RO_CLM_RESULT_C;
		cr->clm_cnt_m = RO_CLM_RESULT_C_M;
		cr->clm_rdy = RO_CLM_RDY_C;
		cr->clm_rdy_m = RO_CLM_RDY_C_M;
		cr->edcca_clm_period = CLM_EDCCA_PERIOD_C;
		cr->edcca_clm_period_m = CLM_EDCCA_PERIOD_C_M;
		cr->edcca_clm_unit_idx = CLM_EDCCA_COUNTER_UNIT_C;
		cr->edcca_clm_unit_idx_m = CLM_EDCCA_COUNTER_UNIT_C_M;
		cr->edcca_clm_en = CLM_EDCCA_EN_C;
		cr->edcca_clm_en_m = CLM_EDCCA_EN_C_M;
		cr->edcca_clm_cnt = RO_CLM_EDCCA_RESULT_C;
		cr->edcca_clm_cnt_m = RO_CLM_EDCCA_RESULT_C_M;
		cr->edcca_clm_rdy = RO_CLM_EDCCA_RDY_C;
		cr->edcca_clm_rdy_m = RO_CLM_EDCCA_RDY_C_M;
		cr->nhm_en = NHM_EN_C;
		cr->nhm_en_m = NHM_EN_C_M;
		cr->nhm_method_sel = NHM_PWDB_METHOD_SEL_C;
		cr->nhm_method_sel_m = NHM_PWDB_METHOD_SEL_C_M;
		cr->nhm_period = NHM_PERIOD_C;
		cr->nhm_period_m = NHM_PERIOD_C_M;
		cr->nhm_unit_idx = NHM_COUNTER_UNIT_C;
		cr->nhm_unit_idx_m = NHM_COUNTER_UNIT_C_M;
		cr->nhm_inclu_cca = NHM_IGNORE_CCA_C;
		cr->nhm_inclu_cca_m = NHM_IGNORE_CCA_C_M;
		cr->nhm_th0 = NHM_TH0_C;
		cr->nhm_th0_m = NHM_TH0_C_M;
		cr->nhm_th1 = NHM_TH1_C;
		cr->nhm_th1_m = NHM_TH1_C_M;
		cr->nhm_th2 = NHM_TH2_C;
		cr->nhm_th2_m = NHM_TH2_C_M;
		cr->nhm_th3 = NHM_TH3_C;
		cr->nhm_th3_m = NHM_TH3_C_M;
		cr->nhm_th4 = NHM_TH4_C;
		cr->nhm_th4_m = NHM_TH4_C_M;
		cr->nhm_th5 = NHM_TH5_C;
		cr->nhm_th5_m = NHM_TH5_C_M;
		cr->nhm_th6 = NHM_TH6_C;
		cr->nhm_th6_m = NHM_TH6_C_M;
		cr->nhm_th7 = NHM_TH7_C;
		cr->nhm_th7_m = NHM_TH7_C_M;
		cr->nhm_th8 = NHM_TH8_C;
		cr->nhm_th8_m = NHM_TH8_C_M;
		cr->nhm_th9 = NHM_TH9_C;
		cr->nhm_th9_m = NHM_TH9_C_M;
		cr->nhm_th10 = NHM_TH10_C;
		cr->nhm_th10_m = NHM_TH10_C_M;
		cr->nhm_cnt0 = NHM_CNT0_C;
		cr->nhm_cnt0_m = NHM_CNT0_C_M;
		cr->nhm_cnt1 = NHM_CNT1_C;
		cr->nhm_cnt1_m = NHM_CNT1_C_M;
		cr->nhm_cnt2 = NHM_CNT2_C;
		cr->nhm_cnt2_m = NHM_CNT2_C_M;
		cr->nhm_cnt3 = NHM_CNT3_C;
		cr->nhm_cnt3_m = NHM_CNT3_C_M;
		cr->nhm_cnt4 = NHM_CNT4_C;
		cr->nhm_cnt4_m = NHM_CNT4_C_M;
		cr->nhm_cnt5 = NHM_CNT5_C;
		cr->nhm_cnt5_m = NHM_CNT5_C_M;
		cr->nhm_cnt6 = NHM_CNT6_C;
		cr->nhm_cnt6_m = NHM_CNT6_C_M;
		cr->nhm_cnt7 = NHM_CNT7_C;
		cr->nhm_cnt7_m = NHM_CNT7_C_M;
		cr->nhm_cnt8 = NHM_CNT8_C;
		cr->nhm_cnt8_m = NHM_CNT8_C_M;
		cr->nhm_cnt9 = NHM_CNT9_C;
		cr->nhm_cnt9_m = NHM_CNT9_C_M;
		cr->nhm_cnt10 = NHM_CNT10_C;
		cr->nhm_cnt10_m = NHM_CNT10_C_M;
		cr->nhm_cnt11 = NHM_CNT11_C;
		cr->nhm_cnt11_m = NHM_CNT11_C_M;
		cr->nhm_cca_cnt = NHM_CCA_CNT_C;
		cr->nhm_cca_cnt_m = NHM_CCA_CNT_C_M;
		cr->nhm_tx_cnt = NHM_TXON_CNT_C;
		cr->nhm_tx_cnt_m = NHM_TXON_CNT_C_M;
		cr->nhm_idle_cnt = NHM_IDLE_CNT_C;
		cr->nhm_idle_cnt_m = NHM_IDLE_CNT_C_M;
		cr->nhm_rdy = NHM_RDY_C;
		cr->nhm_rdy_m = NHM_RDY_C_M;
		cr->fahm_en = FAHM_EN_C;
		cr->fahm_en_m = FAHM_EN_C_M;
		cr->fahm_ofdm_en = FAHM_EN_OFDM_C;
		cr->fahm_ofdm_en_m = FAHM_EN_OFDM_C_M;
		cr->fahm_cck_en = FAHM_EN_CCK_C;
		cr->fahm_cck_en_m = FAHM_EN_CCK_C_M;
		cr->fahm_numer_opt = FAHM_NUM_CANDIDATE_C;
		cr->fahm_numer_opt_m = FAHM_NUM_CANDIDATE_C_M;
		cr->fahm_denom_opt = FAHM_DEN_CANDIDATE_C;
		cr->fahm_denom_opt_m = FAHM_DEN_CANDIDATE_C_M;
		cr->fahm_period = FAHM_PERIOD_C;
		cr->fahm_period_m = FAHM_PERIOD_C_M;
		cr->fahm_unit_idx = FAHM_COUNTER_UNIT_C;
		cr->fahm_unit_idx_m = FAHM_COUNTER_UNIT_C_M;
		cr->fahm_method_sel = FAHM_PWDB_SEL_C;
		cr->fahm_method_sel_m = FAHM_PWDB_SEL_C_M;
		cr->fahm_th0 = FAHM_TH0_C;
		cr->fahm_th0_m = FAHM_TH0_C_M;
		cr->fahm_th1 = FAHM_TH1_C;
		cr->fahm_th1_m = FAHM_TH1_C_M;
		cr->fahm_th2 = FAHM_TH2_C;
		cr->fahm_th2_m = FAHM_TH2_C_M;
		cr->fahm_th3 = FAHM_TH3_C;
		cr->fahm_th3_m = FAHM_TH3_C_M;
		cr->fahm_th4 = FAHM_TH4_C;
		cr->fahm_th4_m = FAHM_TH4_C_M;
		cr->fahm_th5 = FAHM_TH5_C;
		cr->fahm_th5_m = FAHM_TH5_C_M;
		cr->fahm_th6 = FAHM_TH6_C;
		cr->fahm_th6_m = FAHM_TH6_C_M;
		cr->fahm_th7 = FAHM_TH7_C;
		cr->fahm_th7_m = FAHM_TH7_C_M;
		cr->fahm_th8 = FAHM_TH8_C;
		cr->fahm_th8_m = FAHM_TH8_C_M;
		cr->fahm_th9 = FAHM_TH9_C;
		cr->fahm_th9_m = FAHM_TH9_C_M;
		cr->fahm_th10 = FAHM_TH10_C;
		cr->fahm_th10_m = FAHM_TH10_C_M;
		cr->fahm_dis_count_each_mpdu = FAHM_DIS_COUNT_EACH_MPDU_C;
		cr->fahm_dis_count_each_mpdu_m = FAHM_DIS_COUNT_EACH_MPDU_C_M;
		cr->fahm_cnt0 = RO_FAHM_NUM0_C;
		cr->fahm_cnt0_m = RO_FAHM_NUM0_C_M;
		cr->fahm_cnt1 = RO_FAHM_NUM1_C;
		cr->fahm_cnt1_m = RO_FAHM_NUM1_C_M;
		cr->fahm_cnt2 = RO_FAHM_NUM2_C;
		cr->fahm_cnt2_m = RO_FAHM_NUM2_C_M;
		cr->fahm_cnt3 = RO_FAHM_NUM3_C;
		cr->fahm_cnt3_m = RO_FAHM_NUM3_C_M;
		cr->fahm_cnt4 = RO_FAHM_NUM4_C;
		cr->fahm_cnt4_m = RO_FAHM_NUM4_C_M;
		cr->fahm_cnt5 = RO_FAHM_NUM5_C;
		cr->fahm_cnt5_m = RO_FAHM_NUM5_C_M;
		cr->fahm_cnt6 = RO_FAHM_NUM6_C;
		cr->fahm_cnt6_m = RO_FAHM_NUM6_C_M;
		cr->fahm_cnt7 = RO_FAHM_NUM7_C;
		cr->fahm_cnt7_m = RO_FAHM_NUM7_C_M;
		cr->fahm_cnt8 = RO_FAHM_NUM8_C;
		cr->fahm_cnt8_m = RO_FAHM_NUM8_C_M;
		cr->fahm_cnt9 = RO_FAHM_NUM9_C;
		cr->fahm_cnt9_m = RO_FAHM_NUM9_C_M;
		cr->fahm_cnt10 = RO_FAHM_NUM10_C;
		cr->fahm_cnt10_m = RO_FAHM_NUM10_C_M;
		cr->fahm_cnt11 = RO_FAHM_NUM11_C;
		cr->fahm_cnt11_m = RO_FAHM_NUM11_C_M;
		cr->fahm_denom_cnt = RO_FAHM_DEN_C;
		cr->fahm_denom_cnt_m = RO_FAHM_DEN_C_M;
		cr->fahm_rdy = RO_FAHM_RDY_C;
		cr->fahm_rdy_m = RO_FAHM_RDY_C_M;
		cr->ifs_clm_en = IFS_COLLECT_EN_C;
		cr->ifs_clm_en_m = IFS_COLLECT_EN_C_M;
		cr->ifs_clm_clr = IFS_COUNTER_CLR_C;
		cr->ifs_clm_clr_m = IFS_COUNTER_CLR_C_M;
		cr->ifs_clm_period = IFS_COLLECT_TOTAL_TIME_C;
		cr->ifs_clm_period_m = IFS_COLLECT_TOTAL_TIME_C_M;
		cr->ifs_clm_unit_idx = IFS_COUNTER_UNIT_C;
		cr->ifs_clm_unit_idx_m = IFS_COUNTER_UNIT_C_M;
		cr->ifs_t1_en = IFS_T1_EN_C;
		cr->ifs_t1_en_m = IFS_T1_EN_C_M;
		cr->ifs_t2_en = IFS_T2_EN_C;
		cr->ifs_t2_en_m = IFS_T2_EN_C_M;
		cr->ifs_t3_en = IFS_T3_EN_C;
		cr->ifs_t3_en_m = IFS_T3_EN_C_M;
		cr->ifs_t4_en = IFS_T4_EN_C;
		cr->ifs_t4_en_m = IFS_T4_EN_C_M;
		cr->ifs_t1_th_l = IFS_T1_TH_LOW_C;
		cr->ifs_t1_th_l_m = IFS_T1_TH_LOW_C_M;
		cr->ifs_t2_th_l = IFS_T2_TH_LOW_C;
		cr->ifs_t2_th_l_m = IFS_T2_TH_LOW_C_M;
		cr->ifs_t3_th_l = IFS_T3_TH_LOW_C;
		cr->ifs_t3_th_l_m = IFS_T3_TH_LOW_C_M;
		cr->ifs_t4_th_l = IFS_T4_TH_LOW_C;
		cr->ifs_t4_th_l_m = IFS_T4_TH_LOW_C_M;
		cr->ifs_t1_th_h = IFS_T1_TH_HIGH_C;
		cr->ifs_t1_th_h_m = IFS_T1_TH_HIGH_C_M;
		cr->ifs_t2_th_h = IFS_T2_TH_HIGH_C;
		cr->ifs_t2_th_h_m = IFS_T2_TH_HIGH_C_M;
		cr->ifs_t3_th_h = IFS_T3_TH_HIGH_C;
		cr->ifs_t3_th_h_m = IFS_T3_TH_HIGH_C_M;
		cr->ifs_t4_th_h = IFS_T4_TH_HIGH_C;
		cr->ifs_t4_th_h_m = IFS_T4_TH_HIGH_C_M;
		cr->ifs_clm_tx_cnt = IFSCNT_CNT_TX_C;
		cr->ifs_clm_tx_cnt_m = IFSCNT_CNT_TX_C_M;
		cr->ifs_clm_edcca_exclu_cca = IFSCNT_CNT_EDCCA_EXCLUDE_CCA_FA_C;
		cr->ifs_clm_edcca_exclu_cca_m = IFSCNT_CNT_EDCCA_EXCLUDE_CCA_FA_C_M;
		cr->ifs_clm_cckcca_exclu_fa = IFSCNT_CNT_CCKCCA_EXCLUDE_FA_C;
		cr->ifs_clm_cckcca_exclu_fa_m = IFSCNT_CNT_CCKCCA_EXCLUDE_FA_C_M;
		cr->ifs_clm_ofdmcca_exclu_fa = IFSCNT_CNT_OFDMCCA_EXCLUDE_FA_C;
		cr->ifs_clm_ofdmcca_exclu_fa_m = IFSCNT_CNT_OFDMCCA_EXCLUDE_FA_C_M;
		cr->ifs_clm_cck_fa = IFSCNT_CNT_CCKFA_C;
		cr->ifs_clm_cck_fa_m = IFSCNT_CNT_CCKFA_C_M;
		cr->ifs_clm_ofdm_fa = IFSCNT_CNT_OFDMFA_C;
		cr->ifs_clm_ofdm_fa_m = IFSCNT_CNT_OFDMFA_C_M;
		cr->ifs_clm_t1_his = IFS_T1_HIS_C;
		cr->ifs_clm_t1_his_m = IFS_T1_HIS_C_M;
		cr->ifs_clm_t2_his = IFS_T2_HIS_C;
		cr->ifs_clm_t2_his_m = IFS_T2_HIS_C_M;
		cr->ifs_clm_t3_his = IFS_T3_HIS_C;
		cr->ifs_clm_t3_his_m = IFS_T3_HIS_C_M;
		cr->ifs_clm_t4_his = IFS_T4_HIS_C;
		cr->ifs_clm_t4_his_m = IFS_T4_HIS_C_M;
		cr->ifs_clm_t1_avg = IFS_T1_AVG_C;
		cr->ifs_clm_t1_avg_m = IFS_T1_AVG_C_M;
		cr->ifs_clm_t2_avg = IFS_T2_AVG_C;
		cr->ifs_clm_t2_avg_m = IFS_T2_AVG_C_M;
		cr->ifs_clm_t3_avg = IFS_T3_AVG_C;
		cr->ifs_clm_t3_avg_m = IFS_T3_AVG_C_M;
		cr->ifs_clm_t4_avg = IFS_T4_AVG_C;
		cr->ifs_clm_t4_avg_m = IFS_T4_AVG_C_M;
		cr->ifs_clm_t1_cca = IFS_T1_CLM_C;
		cr->ifs_clm_t1_cca_m = IFS_T1_CLM_C_M;
		cr->ifs_clm_t2_cca = IFS_T2_CLM_C;
		cr->ifs_clm_t2_cca_m = IFS_T2_CLM_C_M;
		cr->ifs_clm_t3_cca = IFS_T3_CLM_C;
		cr->ifs_clm_t3_cca_m = IFS_T3_CLM_C_M;
		cr->ifs_clm_t4_cca = IFS_T4_CLM_C;
		cr->ifs_clm_t4_cca_m = IFS_T4_CLM_C_M;
		cr->ifs_total_cnt = IFS_TOTAL_C;
		cr->ifs_total_cnt_m = IFS_TOTAL_C_M;
		cr->ifs_clm_rdy = IFSCNT_DONE_C;
		cr->ifs_clm_rdy_m = IFSCNT_DONE_C_M;
		break;
	#endif

	#ifdef HALBB_COMPILE_AP2_SERIES
	case BB_AP2:
		cr->ccx_en = CCX_EN_A2;
		cr->ccx_en_m = CCX_EN_A2_M;
		cr->ccx_trig_opt = CCX_TRIG_OPT_A2;
		cr->ccx_trig_opt_m = CCX_TRIG_OPT_A2_M;
		cr->ccx_trig = MEASUREMENT_TRIG_A2;
		cr->ccx_trig_m = MEASUREMENT_TRIG_A2_M;
		cr->ccx_edcca_opt = CCX_EDCCA_OPT_A2;
		cr->ccx_edcca_opt_m = CCX_EDCCA_OPT_A2_M;
		cr->ccx_source_sel = CCX_SOURCE_SEL_A2;
		cr->ccx_source_sel_m = CCX_SOURCE_SEL_A2_M;
		cr->clm_unit_idx = CLM_COUNTER_UNIT_A2;
		cr->clm_unit_idx_m = CLM_COUNTER_UNIT_A2_M;
		cr->clm_en = CLM_EN_A2;
		cr->clm_en_m = CLM_EN_A2_M;
		cr->clm_opt = CLM_CCA_OPT_A2;
		cr->clm_opt_m = CLM_CCA_OPT_A2_M;
		cr->clm_period = CLM_PERIOD_A2;
		cr->clm_period_m = CLM_PERIOD_A2_M;
		cr->clm_dbg_sel = CLM_FROM_DBG_SEL_A2;
		cr->clm_dbg_sel_m = CLM_FROM_DBG_SEL_A2_M;
		cr->clm_nav_en = CLM_NAV_EN_A2;
		cr->clm_nav_en_m = CLM_NAV_EN_A2_M;
		cr->clm_rssi_th_en = CLM_RSSI_TH_EN_A2;
		cr->clm_rssi_th_en_m = CLM_RSSI_TH_EN_A2_M;
		cr->clm_rssi_th = CLM_RSSI_TH_A2;
		cr->clm_rssi_th_m = CLM_RSSI_TH_A2_M;
		cr->clm_cnt = RO_CLM_RESULT_A2;
		cr->clm_cnt_m = RO_CLM_RESULT_A2_M;
		cr->clm_rdy = RO_CLM_RDY_A2;
		cr->clm_rdy_m = RO_CLM_RDY_A2_M;
		cr->edcca_clm_period = CLM_EDCCA_PERIOD_A2;
		cr->edcca_clm_period_m = CLM_EDCCA_PERIOD_A2_M;
		cr->edcca_clm_unit_idx = CLM_EDCCA_COUNTER_UNIT_A2;
		cr->edcca_clm_unit_idx_m = CLM_EDCCA_COUNTER_UNIT_A2_M;
		cr->edcca_clm_en = CLM_EDCCA_EN_A2;
		cr->edcca_clm_en_m = CLM_EDCCA_EN_A2_M;
		cr->edcca_clm_cnt = RO_CLM_EDCCA_RESULT_A2;
		cr->edcca_clm_cnt_m = RO_CLM_EDCCA_RESULT_A2_M;
		cr->edcca_clm_rdy = RO_CLM_EDCCA_RDY_A2;
		cr->edcca_clm_rdy_m = RO_CLM_EDCCA_RDY_A2_M;
		cr->nhm_en = NHM_EN_A2;
		cr->nhm_en_m = NHM_EN_A2_M;
		cr->nhm_method_sel = NHM_PWDB_METHOD_SEL_A2;
		cr->nhm_method_sel_m = NHM_PWDB_METHOD_SEL_A2_M;
		cr->nhm_period = NHM_PERIOD_A2;
		cr->nhm_period_m = NHM_PERIOD_A2_M;
		cr->nhm_unit_idx = NHM_COUNTER_UNIT_A2;
		cr->nhm_unit_idx_m = NHM_COUNTER_UNIT_A2_M;
		cr->nhm_inclu_cca = NHM_IGNORE_CCA_A2;
		cr->nhm_inclu_cca_m = NHM_IGNORE_CCA_A2_M;
		cr->nhm_nav_en = NHM_NAV_EN_A2;
		cr->nhm_nav_en_m = NHM_NAV_EN_A2_M;
		cr->nhm_rssi_th_en = NHM_RSSI_TH_EN_A2;
		cr->nhm_rssi_th_en_m = NHM_RSSI_TH_EN_A2_M;
		cr->nhm_rssi_th = NHM_RSSI_TH_A2;
		cr->nhm_rssi_th_m = NHM_RSSI_TH_A2_M;
		cr->nhm_th0 = NHM_TH0_A2;
		cr->nhm_th0_m = NHM_TH0_A2_M;
		cr->nhm_th1 = NHM_TH1_A2;
		cr->nhm_th1_m = NHM_TH1_A2_M;
		cr->nhm_th2 = NHM_TH2_A2;
		cr->nhm_th2_m = NHM_TH2_A2_M;
		cr->nhm_th3 = NHM_TH3_A2;
		cr->nhm_th3_m = NHM_TH3_A2_M;
		cr->nhm_th4 = NHM_TH4_A2;
		cr->nhm_th4_m = NHM_TH4_A2_M;
		cr->nhm_th5 = NHM_TH5_A2;
		cr->nhm_th5_m = NHM_TH5_A2_M;
		cr->nhm_th6 = NHM_TH6_A2;
		cr->nhm_th6_m = NHM_TH6_A2_M;
		cr->nhm_th7 = NHM_TH7_A2;
		cr->nhm_th7_m = NHM_TH7_A2_M;
		cr->nhm_th8 = NHM_TH8_A2;
		cr->nhm_th8_m = NHM_TH8_A2_M;
		cr->nhm_th9 = NHM_TH9_A2;
		cr->nhm_th9_m = NHM_TH9_A2_M;
		cr->nhm_th10 = NHM_TH10_A2;
		cr->nhm_th10_m = NHM_TH10_A2_M;
		cr->nhm_cnt0 = NHM_CNT0_A2;
		cr->nhm_cnt0_m = NHM_CNT0_A2_M;
		cr->nhm_cnt1 = NHM_CNT1_A2;
		cr->nhm_cnt1_m = NHM_CNT1_A2_M;
		cr->nhm_cnt2 = NHM_CNT2_A2;
		cr->nhm_cnt2_m = NHM_CNT2_A2_M;
		cr->nhm_cnt3 = NHM_CNT3_A2;
		cr->nhm_cnt3_m = NHM_CNT3_A2_M;
		cr->nhm_cnt4 = NHM_CNT4_A2;
		cr->nhm_cnt4_m = NHM_CNT4_A2_M;
		cr->nhm_cnt5 = NHM_CNT5_A2;
		cr->nhm_cnt5_m = NHM_CNT5_A2_M;
		cr->nhm_cnt6 = NHM_CNT6_A2;
		cr->nhm_cnt6_m = NHM_CNT6_A2_M;
		cr->nhm_cnt7 = NHM_CNT7_A2;
		cr->nhm_cnt7_m = NHM_CNT7_A2_M;
		cr->nhm_cnt8 = NHM_CNT8_A2;
		cr->nhm_cnt8_m = NHM_CNT8_A2_M;
		cr->nhm_cnt9 = NHM_CNT9_A2;
		cr->nhm_cnt9_m = NHM_CNT9_A2_M;
		cr->nhm_cnt10 = NHM_CNT10_A2;
		cr->nhm_cnt10_m = NHM_CNT10_A2_M;
		cr->nhm_cnt11 = NHM_CNT11_A2;
		cr->nhm_cnt11_m = NHM_CNT11_A2_M;
		cr->nhm_cca_cnt = NHM_CCA_CNT_A2;
		cr->nhm_cca_cnt_m = NHM_CCA_CNT_A2_M;
		cr->nhm_tx_cnt = NHM_TXON_CNT_A2;
		cr->nhm_tx_cnt_m = NHM_TXON_CNT_A2_M;
		cr->nhm_idle_cnt = NHM_IDLE_CNT_A2;
		cr->nhm_idle_cnt_m = NHM_IDLE_CNT_A2_M;
		cr->nhm_rdy = NHM_RDY_A2;
		cr->nhm_rdy_m = NHM_RDY_A2_M;
		cr->fahm_en = FAHM_EN_A2;
		cr->fahm_en_m = FAHM_EN_A2_M;
		cr->fahm_ofdm_en = FAHM_EN_OFDM_A2;
		cr->fahm_ofdm_en_m = FAHM_EN_OFDM_A2_M;
		cr->fahm_cck_en = FAHM_EN_CCK_A2;
		cr->fahm_cck_en_m = FAHM_EN_CCK_A2_M;
		cr->fahm_numer_opt = FAHM_NUM_CANDIDATE_A2;
		cr->fahm_numer_opt_m = FAHM_NUM_CANDIDATE_A2_M;
		cr->fahm_denom_opt = FAHM_DEN_CANDIDATE_A2;
		cr->fahm_denom_opt_m = FAHM_DEN_CANDIDATE_A2_M;
		cr->fahm_period = FAHM_PERIOD_A2;
		cr->fahm_period_m = FAHM_PERIOD_A2_M;
		cr->fahm_unit_idx = FAHM_COUNTER_UNIT_A2;
		cr->fahm_unit_idx_m = FAHM_COUNTER_UNIT_A2_M;
		cr->fahm_method_sel = FAHM_PWDB_SEL_A2;
		cr->fahm_method_sel_m = FAHM_PWDB_SEL_A2_M;
		cr->fahm_th0 = FAHM_TH0_A2;
		cr->fahm_th0_m = FAHM_TH0_A2_M;
		cr->fahm_th1 = FAHM_TH1_A2;
		cr->fahm_th1_m = FAHM_TH1_A2_M;
		cr->fahm_th2 = FAHM_TH2_A2;
		cr->fahm_th2_m = FAHM_TH2_A2_M;
		cr->fahm_th3 = FAHM_TH3_A2;
		cr->fahm_th3_m = FAHM_TH3_A2_M;
		cr->fahm_th4 = FAHM_TH4_A2;
		cr->fahm_th4_m = FAHM_TH4_A2_M;
		cr->fahm_th5 = FAHM_TH5_A2;
		cr->fahm_th5_m = FAHM_TH5_A2_M;
		cr->fahm_th6 = FAHM_TH6_A2;
		cr->fahm_th6_m = FAHM_TH6_A2_M;
		cr->fahm_th7 = FAHM_TH7_A2;
		cr->fahm_th7_m = FAHM_TH7_A2_M;
		cr->fahm_th8 = FAHM_TH8_A2;
		cr->fahm_th8_m = FAHM_TH8_A2_M;
		cr->fahm_th9 = FAHM_TH9_A2;
		cr->fahm_th9_m = FAHM_TH9_A2_M;
		cr->fahm_th10 = FAHM_TH10_A2;
		cr->fahm_th10_m = FAHM_TH10_A2_M;
		cr->fahm_dis_count_each_mpdu = FAHM_DIS_COUNT_EACH_MPDU_A2;
		cr->fahm_dis_count_each_mpdu_m = FAHM_DIS_COUNT_EACH_MPDU_A2_M;
		cr->fahm_cnt0 = RO_FAHM_NUM0_A2;
		cr->fahm_cnt0_m = RO_FAHM_NUM0_A2_M;
		cr->fahm_cnt1 = RO_FAHM_NUM1_A2;
		cr->fahm_cnt1_m = RO_FAHM_NUM1_A2_M;
		cr->fahm_cnt2 = RO_FAHM_NUM2_A2;
		cr->fahm_cnt2_m = RO_FAHM_NUM2_A2_M;
		cr->fahm_cnt3 = RO_FAHM_NUM3_A2;
		cr->fahm_cnt3_m = RO_FAHM_NUM3_A2_M;
		cr->fahm_cnt4 = RO_FAHM_NUM4_A2;
		cr->fahm_cnt4_m = RO_FAHM_NUM4_A2_M;
		cr->fahm_cnt5 = RO_FAHM_NUM5_A2;
		cr->fahm_cnt5_m = RO_FAHM_NUM5_A2_M;
		cr->fahm_cnt6 = RO_FAHM_NUM6_A2;
		cr->fahm_cnt6_m = RO_FAHM_NUM6_A2_M;
		cr->fahm_cnt7 = RO_FAHM_NUM7_A2;
		cr->fahm_cnt7_m = RO_FAHM_NUM7_A2_M;
		cr->fahm_cnt8 = RO_FAHM_NUM8_A2;
		cr->fahm_cnt8_m = RO_FAHM_NUM8_A2_M;
		cr->fahm_cnt9 = RO_FAHM_NUM9_A2;
		cr->fahm_cnt9_m = RO_FAHM_NUM9_A2_M;
		cr->fahm_cnt10 = RO_FAHM_NUM10_A2;
		cr->fahm_cnt10_m = RO_FAHM_NUM10_A2_M;
		cr->fahm_cnt11 = RO_FAHM_NUM11_A2;
		cr->fahm_cnt11_m = RO_FAHM_NUM11_A2_M;
		cr->fahm_denom_cnt = RO_FAHM_DEN_A2;
		cr->fahm_denom_cnt_m = RO_FAHM_DEN_A2_M;
		cr->fahm_rdy = RO_FAHM_RDY_A2;
		cr->fahm_rdy_m = RO_FAHM_RDY_A2_M;
		cr->ifs_clm_en = IFS_COLLECT_EN_A2;
		cr->ifs_clm_en_m = IFS_COLLECT_EN_A2_M;
		cr->ifs_clm_clr = IFS_COUNTER_CLR_A2;
		cr->ifs_clm_clr_m = IFS_COUNTER_CLR_A2_M;
		cr->ifs_clm_period = IFS_COLLECT_TOTAL_TIME_A2;
		cr->ifs_clm_period_m = IFS_COLLECT_TOTAL_TIME_A2_M;
		cr->ifs_clm_unit_idx = IFS_COUNTER_UNIT_A2;
		cr->ifs_clm_unit_idx_m = IFS_COUNTER_UNIT_A2_M;
		cr->ifs_t1_en = IFS_T1_EN_A2;
		cr->ifs_t1_en_m = IFS_T1_EN_A2_M;
		cr->ifs_t2_en = IFS_T2_EN_A2;
		cr->ifs_t2_en_m = IFS_T2_EN_A2_M;
		cr->ifs_t3_en = IFS_T3_EN_A2;
		cr->ifs_t3_en_m = IFS_T3_EN_A2_M;
		cr->ifs_t4_en = IFS_T4_EN_A2;
		cr->ifs_t4_en_m = IFS_T4_EN_A2_M;
		cr->ifs_t1_th_l = IFS_T1_TH_LOW_A2;
		cr->ifs_t1_th_l_m = IFS_T1_TH_LOW_A2_M;
		cr->ifs_t2_th_l = IFS_T2_TH_LOW_A2;
		cr->ifs_t2_th_l_m = IFS_T2_TH_LOW_A2_M;
		cr->ifs_t3_th_l = IFS_T3_TH_LOW_A2;
		cr->ifs_t3_th_l_m = IFS_T3_TH_LOW_A2_M;
		cr->ifs_t4_th_l = IFS_T4_TH_LOW_A2;
		cr->ifs_t4_th_l_m = IFS_T4_TH_LOW_A2_M;
		cr->ifs_t1_th_h = IFS_T1_TH_HIGH_A2;
		cr->ifs_t1_th_h_m = IFS_T1_TH_HIGH_A2_M;
		cr->ifs_t2_th_h = IFS_T2_TH_HIGH_A2;
		cr->ifs_t2_th_h_m = IFS_T2_TH_HIGH_A2_M;
		cr->ifs_t3_th_h = IFS_T3_TH_HIGH_A2;
		cr->ifs_t3_th_h_m = IFS_T3_TH_HIGH_A2_M;
		cr->ifs_t4_th_h = IFS_T4_TH_HIGH_A2;
		cr->ifs_t4_th_h_m = IFS_T4_TH_HIGH_A2_M;
		cr->ifs_clm_tx_cnt = IFSCNT_CNT_TX_A2;
		cr->ifs_clm_tx_cnt_m = IFSCNT_CNT_TX_A2_M;
		cr->ifs_clm_edcca_exclu_cca = IFSCNT_CNT_EDCCA_EXCLUDE_CCA_FA_A2;
		cr->ifs_clm_edcca_exclu_cca_m = IFSCNT_CNT_EDCCA_EXCLUDE_CCA_FA_A2_M;
		cr->ifs_clm_cckcca_exclu_fa = IFSCNT_CNT_CCKCCA_EXCLUDE_FA_A2;
		cr->ifs_clm_cckcca_exclu_fa_m = IFSCNT_CNT_CCKCCA_EXCLUDE_FA_A2_M;
		cr->ifs_clm_ofdmcca_exclu_fa = IFSCNT_CNT_OFDMCCA_EXCLUDE_FA_A2;
		cr->ifs_clm_ofdmcca_exclu_fa_m = IFSCNT_CNT_OFDMCCA_EXCLUDE_FA_A2_M;
		cr->ifs_clm_cck_fa = IFSCNT_CNT_CCKFA_A2;
		cr->ifs_clm_cck_fa_m = IFSCNT_CNT_CCKFA_A2_M;
		cr->ifs_clm_ofdm_fa = IFSCNT_CNT_OFDMFA_A2;
		cr->ifs_clm_ofdm_fa_m = IFSCNT_CNT_OFDMFA_A2_M;
		cr->ifs_clm_t1_his = IFS_T1_HIS_A2;
		cr->ifs_clm_t1_his_m = IFS_T1_HIS_A2_M;
		cr->ifs_clm_t2_his = IFS_T2_HIS_A2;
		cr->ifs_clm_t2_his_m = IFS_T2_HIS_A2_M;
		cr->ifs_clm_t3_his = IFS_T3_HIS_A2;
		cr->ifs_clm_t3_his_m = IFS_T3_HIS_A2_M;
		cr->ifs_clm_t4_his = IFS_T4_HIS_A2;
		cr->ifs_clm_t4_his_m = IFS_T4_HIS_A2_M;
		cr->ifs_clm_t1_avg = IFS_T1_AVG_A2;
		cr->ifs_clm_t1_avg_m = IFS_T1_AVG_A2_M;
		cr->ifs_clm_t2_avg = IFS_T2_AVG_A2;
		cr->ifs_clm_t2_avg_m = IFS_T2_AVG_A2_M;
		cr->ifs_clm_t3_avg = IFS_T3_AVG_A2;
		cr->ifs_clm_t3_avg_m = IFS_T3_AVG_A2_M;
		cr->ifs_clm_t4_avg = IFS_T4_AVG_A2;
		cr->ifs_clm_t4_avg_m = IFS_T4_AVG_A2_M;
		cr->ifs_clm_t1_cca = IFS_T1_CLM_A2;
		cr->ifs_clm_t1_cca_m = IFS_T1_CLM_A2_M;
		cr->ifs_clm_t2_cca = IFS_T2_CLM_A2;
		cr->ifs_clm_t2_cca_m = IFS_T2_CLM_A2_M;
		cr->ifs_clm_t3_cca = IFS_T3_CLM_A2;
		cr->ifs_clm_t3_cca_m = IFS_T3_CLM_A2_M;
		cr->ifs_clm_t4_cca = IFS_T4_CLM_A2;
		cr->ifs_clm_t4_cca_m = IFS_T4_CLM_A2_M;
		cr->ifs_total_cnt = IFS_TOTAL_A2;
		cr->ifs_total_cnt_m = IFS_TOTAL_A2_M;
		cr->ifs_clm_rdy = IFSCNT_DONE_A2;
		cr->ifs_clm_rdy_m = IFSCNT_DONE_A2_M;
		break;
	#endif

	#ifdef HALBB_COMPILE_BE0_SERIES
	case BB_BE0:
		cr->ccx_en = CCX_EN_BE0;
		cr->ccx_en_m = CCX_EN_BE0_M;
		cr->ccx_trig_opt = CCX_TRIG_OPT_BE0;
		cr->ccx_trig_opt_m = CCX_TRIG_OPT_BE0_M;
		cr->ccx_trig = MEASUREMENT_TRIG_BE0;
		cr->ccx_trig_m = MEASUREMENT_TRIG_BE0_M;
		cr->ccx_edcca_opt = CCX_EDCCA_OPT_BE0;
		cr->ccx_edcca_opt_m = CCX_EDCCA_OPT_BE0_M;
		cr->ccx_source_sel = RXINT_R_CCX_SOURCE_SEL_BE0; // different naming
		cr->ccx_source_sel_m = RXINT_R_CCX_SOURCE_SEL_BE0_M; // different naming
		cr->clm_unit_idx = CLM_COUNTER_UNIT_BE0;
		cr->clm_unit_idx_m = CLM_COUNTER_UNIT_BE0_M;
		cr->clm_en = CLM_EN_BE0;
		cr->clm_en_m = CLM_EN_BE0_M;
		cr->clm_opt = CLM_CCA_OPT_BE0;
		cr->clm_opt_m = CLM_CCA_OPT_BE0_M;
		cr->clm_period = CLM_PERIOD_BE0;
		cr->clm_period_m = CLM_PERIOD_BE0_M;
		cr->clm_dbg_sel = CLM_FROM_DBG_SEL_BE0;
		cr->clm_dbg_sel_m = CLM_FROM_DBG_SEL_BE0_M;
		cr->clm_nav_en = CLM_NAV_EN_BE0;
		cr->clm_nav_en_m = CLM_NAV_EN_BE0_M;
		cr->clm_rssi_th_en = CLM_RSSI_TH_EN_BE0;
		cr->clm_rssi_th_en_m = CLM_RSSI_TH_EN_BE0_M;
		cr->clm_rssi_th = CLM_RSSI_TH_BE0;
		cr->clm_rssi_th_m = CLM_RSSI_TH_BE0_M;
		cr->clm_cnt = RO_CLM_RESULT_BE0;
		cr->clm_cnt_m = RO_CLM_RESULT_BE0_M;
		cr->clm_rdy = RO_CLM_RDY_BE0;
		cr->clm_rdy_m = RO_CLM_RDY_BE0_M;
		cr->edcca_clm_period = CLM_EDCCA_PERIOD_BE0;
		cr->edcca_clm_period_m = CLM_EDCCA_PERIOD_BE0_M;
		cr->edcca_clm_unit_idx = CLM_EDCCA_COUNTER_UNIT_BE0;
		cr->edcca_clm_unit_idx_m = CLM_EDCCA_COUNTER_UNIT_BE0_M;
		cr->edcca_clm_en = CLM_EDCCA_EN_BE0;
		cr->edcca_clm_en_m = CLM_EDCCA_EN_BE0_M;
		cr->edcca_clm_cnt = RO_CLM_EDCCA_RESULT_BE0;
		cr->edcca_clm_cnt_m = RO_CLM_EDCCA_RESULT_BE0_M;
		cr->edcca_clm_rdy = RO_CLM_EDCCA_RDY_BE0;
		cr->edcca_clm_rdy_m = RO_CLM_EDCCA_RDY_BE0_M;
		cr->nhm_en = NHM_EN_BE0;
		cr->nhm_en_m = NHM_EN_BE0_M;
		cr->nhm_method_sel = NHM_PWDB_METHOD_SEL_BE0;
		cr->nhm_method_sel_m = NHM_PWDB_METHOD_SEL_BE0_M;
		cr->nhm_period = NHM_PERIOD_BE0;
		cr->nhm_period_m = NHM_PERIOD_BE0_M;
		cr->nhm_unit_idx = NHM_COUNTER_UNIT_BE0;
		cr->nhm_unit_idx_m = NHM_COUNTER_UNIT_BE0_M;
		cr->nhm_inclu_cca = NHM_IGNORE_CCA_BE0;
		cr->nhm_inclu_cca_m = NHM_IGNORE_CCA_BE0_M;
		cr->nhm_nav_en = NHM_NAV_EN_BE0;
		cr->nhm_nav_en_m = NHM_NAV_EN_BE0_M;
		cr->nhm_rssi_th_en = NHM_RSSI_TH_EN_BE0;
		cr->nhm_rssi_th_en_m = NHM_RSSI_TH_EN_BE0_M;
		cr->nhm_rssi_th = NHM_RSSI_TH_BE0;
		cr->nhm_rssi_th_m = NHM_RSSI_TH_BE0_M;
		cr->nhm_th0 = NHM_TH0_BE0;
		cr->nhm_th0_m = NHM_TH0_BE0_M;
		cr->nhm_th1 = NHM_TH1_BE0;
		cr->nhm_th1_m = NHM_TH1_BE0_M;
		cr->nhm_th2 = NHM_TH2_BE0;
		cr->nhm_th2_m = NHM_TH2_BE0_M;
		cr->nhm_th3 = NHM_TH3_BE0;
		cr->nhm_th3_m = NHM_TH3_BE0_M;
		cr->nhm_th4 = NHM_TH4_BE0;
		cr->nhm_th4_m = NHM_TH4_BE0_M;
		cr->nhm_th5 = NHM_TH5_BE0;
		cr->nhm_th5_m = NHM_TH5_BE0_M;
		cr->nhm_th6 = NHM_TH6_BE0;
		cr->nhm_th6_m = NHM_TH6_BE0_M;
		cr->nhm_th7 = NHM_TH7_BE0;
		cr->nhm_th7_m = NHM_TH7_BE0_M;
		cr->nhm_th8 = NHM_TH8_BE0;
		cr->nhm_th8_m = NHM_TH8_BE0_M;
		cr->nhm_th9 = NHM_TH9_BE0;
		cr->nhm_th9_m = NHM_TH9_BE0_M;
		cr->nhm_th10 = NHM_TH10_BE0;
		cr->nhm_th10_m = NHM_TH10_BE0_M;
		cr->nhm_cnt0 = NHM_CNT0_BE0;
		cr->nhm_cnt0_m = NHM_CNT0_BE0_M;
		cr->nhm_cnt1 = NHM_CNT1_BE0;
		cr->nhm_cnt1_m = NHM_CNT1_BE0_M;
		cr->nhm_cnt2 = NHM_CNT2_BE0;
		cr->nhm_cnt2_m = NHM_CNT2_BE0_M;
		cr->nhm_cnt3 = NHM_CNT3_BE0;
		cr->nhm_cnt3_m = NHM_CNT3_BE0_M;
		cr->nhm_cnt4 = NHM_CNT4_BE0;
		cr->nhm_cnt4_m = NHM_CNT4_BE0_M;
		cr->nhm_cnt5 = NHM_CNT5_BE0;
		cr->nhm_cnt5_m = NHM_CNT5_BE0_M;
		cr->nhm_cnt6 = NHM_CNT6_BE0;
		cr->nhm_cnt6_m = NHM_CNT6_BE0_M;
		cr->nhm_cnt7 = NHM_CNT7_BE0;
		cr->nhm_cnt7_m = NHM_CNT7_BE0_M;
		cr->nhm_cnt8 = NHM_CNT8_BE0;
		cr->nhm_cnt8_m = NHM_CNT8_BE0_M;
		cr->nhm_cnt9 = NHM_CNT9_BE0;
		cr->nhm_cnt9_m = NHM_CNT9_BE0_M;
		cr->nhm_cnt10 = NHM_CNT10_BE0;
		cr->nhm_cnt10_m = NHM_CNT10_BE0_M;
		cr->nhm_cnt11 = NHM_CNT11_BE0;
		cr->nhm_cnt11_m = NHM_CNT11_BE0_M;
		cr->nhm_cca_cnt = NHM_CCA_CNT_BE0;
		cr->nhm_cca_cnt_m = NHM_CCA_CNT_BE0_M;
		cr->nhm_tx_cnt = NHM_TXON_CNT_BE0;
		cr->nhm_tx_cnt_m = NHM_TXON_CNT_BE0_M;
		cr->nhm_idle_cnt = NHM_IDLE_CNT_BE0;
		cr->nhm_idle_cnt_m = NHM_IDLE_CNT_BE0_M;
		cr->nhm_rdy = NHM_RDY_BE0;
		cr->nhm_rdy_m = NHM_RDY_BE0_M;
		cr->fahm_en = FAHM_EN_BE0;
		cr->fahm_en_m = FAHM_EN_BE0_M;
		cr->fahm_ofdm_en = FAHM_EN_OFDM_BE0;
		cr->fahm_ofdm_en_m = FAHM_EN_OFDM_BE0_M;
		cr->fahm_cck_en = FAHM_EN_CCK_BE0;
		cr->fahm_cck_en_m = FAHM_EN_CCK_BE0_M;
		cr->fahm_numer_opt = FAHM_NUM_CANDIDATE_BE0;
		cr->fahm_numer_opt_m = FAHM_NUM_CANDIDATE_BE0_M;
		cr->fahm_denom_opt = FAHM_DEN_CANDIDATE_BE0;
		cr->fahm_denom_opt_m = FAHM_DEN_CANDIDATE_BE0_M;
		cr->fahm_period = FAHM_PERIOD_BE0;
		cr->fahm_period_m = FAHM_PERIOD_BE0_M;
		cr->fahm_unit_idx = FAHM_COUNTER_UNIT_BE0;
		cr->fahm_unit_idx_m = FAHM_COUNTER_UNIT_BE0_M;
		cr->fahm_method_sel = FAHM_PWDB_SEL_BE0;
		cr->fahm_method_sel_m = FAHM_PWDB_SEL_BE0_M;
		cr->fahm_th0 = FAHM_TH0_BE0;
		cr->fahm_th0_m = FAHM_TH0_BE0_M;
		cr->fahm_th1 = FAHM_TH1_BE0;
		cr->fahm_th1_m = FAHM_TH1_BE0_M;
		cr->fahm_th2 = FAHM_TH2_BE0;
		cr->fahm_th2_m = FAHM_TH2_BE0_M;
		cr->fahm_th3 = FAHM_TH3_BE0;
		cr->fahm_th3_m = FAHM_TH3_BE0_M;
		cr->fahm_th4 = FAHM_TH4_BE0;
		cr->fahm_th4_m = FAHM_TH4_BE0_M;
		cr->fahm_th5 = FAHM_TH5_BE0;
		cr->fahm_th5_m = FAHM_TH5_BE0_M;
		cr->fahm_th6 = FAHM_TH6_BE0;
		cr->fahm_th6_m = FAHM_TH6_BE0_M;
		cr->fahm_th7 = FAHM_TH7_BE0;
		cr->fahm_th7_m = FAHM_TH7_BE0_M;
		cr->fahm_th8 = FAHM_TH8_BE0;
		cr->fahm_th8_m = FAHM_TH8_BE0_M;
		cr->fahm_th9 = FAHM_TH9_BE0;
		cr->fahm_th9_m = FAHM_TH9_BE0_M;
		cr->fahm_th10 = FAHM_TH10_BE0;
		cr->fahm_th10_m = FAHM_TH10_BE0_M;
		cr->fahm_dis_count_each_mpdu = FAHM_DIS_COUNT_EACH_MPDU_BE0;
		cr->fahm_dis_count_each_mpdu_m = FAHM_DIS_COUNT_EACH_MPDU_BE0_M;
		cr->fahm_cnt0 = RO_FAHM_NUM0_BE0;
		cr->fahm_cnt0_m = RO_FAHM_NUM0_BE0_M;
		cr->fahm_cnt1 = RO_FAHM_NUM1_BE0;
		cr->fahm_cnt1_m = RO_FAHM_NUM1_BE0_M;
		cr->fahm_cnt2 = RO_FAHM_NUM2_BE0;
		cr->fahm_cnt2_m = RO_FAHM_NUM2_BE0_M;
		cr->fahm_cnt3 = RO_FAHM_NUM3_BE0;
		cr->fahm_cnt3_m = RO_FAHM_NUM3_BE0_M;
		cr->fahm_cnt4 = RO_FAHM_NUM4_BE0;
		cr->fahm_cnt4_m = RO_FAHM_NUM4_BE0_M;
		cr->fahm_cnt5 = RO_FAHM_NUM5_BE0;
		cr->fahm_cnt5_m = RO_FAHM_NUM5_BE0_M;
		cr->fahm_cnt6 = RO_FAHM_NUM6_BE0;
		cr->fahm_cnt6_m = RO_FAHM_NUM6_BE0_M;
		cr->fahm_cnt7 = RO_FAHM_NUM7_BE0;
		cr->fahm_cnt7_m = RO_FAHM_NUM7_BE0_M;
		cr->fahm_cnt8 = RO_FAHM_NUM8_BE0;
		cr->fahm_cnt8_m = RO_FAHM_NUM8_BE0_M;
		cr->fahm_cnt9 = RO_FAHM_NUM9_BE0;
		cr->fahm_cnt9_m = RO_FAHM_NUM9_BE0_M;
		cr->fahm_cnt10 = RO_FAHM_NUM10_BE0;
		cr->fahm_cnt10_m = RO_FAHM_NUM10_BE0_M;
		cr->fahm_cnt11 = RO_FAHM_NUM11_BE0;
		cr->fahm_cnt11_m = RO_FAHM_NUM11_BE0_M;
		cr->fahm_denom_cnt = RO_FAHM_DEN_BE0;
		cr->fahm_denom_cnt_m = RO_FAHM_DEN_BE0_M;
		cr->fahm_rdy = RO_FAHM_RDY_BE0;
		cr->fahm_rdy_m = RO_FAHM_RDY_BE0_M;
		cr->ifs_clm_en = IFS_COLLECT_EN_BE0;
		cr->ifs_clm_en_m = IFS_COLLECT_EN_BE0_M;
		cr->ifs_clm_clr = IFS_COUNTER_CLR_BE0;
		cr->ifs_clm_clr_m = IFS_COUNTER_CLR_BE0_M;
		cr->ifs_clm_period = IFS_COLLECT_TOTAL_TIME_BE0;
		cr->ifs_clm_period_m = IFS_COLLECT_TOTAL_TIME_BE0_M;
		cr->ifs_clm_unit_idx = IFS_COUNTER_UNIT_BE0;
		cr->ifs_clm_unit_idx_m = IFS_COUNTER_UNIT_BE0_M;
		cr->ifs_t1_en = IFS_T1_EN_BE0;
		cr->ifs_t1_en_m = IFS_T1_EN_BE0_M;
		cr->ifs_t2_en = IFS_T2_EN_BE0;
		cr->ifs_t2_en_m = IFS_T2_EN_BE0_M;
		cr->ifs_t3_en = IFS_T3_EN_BE0;
		cr->ifs_t3_en_m = IFS_T3_EN_BE0_M;
		cr->ifs_t4_en = IFS_T4_EN_BE0;
		cr->ifs_t4_en_m = IFS_T4_EN_BE0_M;
		cr->ifs_t5_en = IFS_T5_EN_BE0;
		cr->ifs_t5_en_m = IFS_T5_EN_BE0_M;
		cr->ifs_t1_th_l = IFS_T1_TH_LOW_BE0;
		cr->ifs_t1_th_l_m = IFS_T1_TH_LOW_BE0_M;
		cr->ifs_t2_th_l = IFS_T2_TH_LOW_BE0;
		cr->ifs_t2_th_l_m = IFS_T2_TH_LOW_BE0_M;
		cr->ifs_t3_th_l = IFS_T3_TH_LOW_BE0;
		cr->ifs_t3_th_l_m = IFS_T3_TH_LOW_BE0_M;
		cr->ifs_t4_th_l = IFS_T4_TH_LOW_BE0;
		cr->ifs_t4_th_l_m = IFS_T4_TH_LOW_BE0_M;
		cr->ifs_t5_th_l = IFS_T5_TH_LOW_BE0;
		cr->ifs_t5_th_l_m = IFS_T5_TH_LOW_BE0_M;
		cr->ifs_t1_th_h = IFS_T1_TH_HIGH_BE0;
		cr->ifs_t1_th_h_m = IFS_T1_TH_HIGH_BE0_M;
		cr->ifs_t2_th_h = IFS_T2_TH_HIGH_BE0;
		cr->ifs_t2_th_h_m = IFS_T2_TH_HIGH_BE0_M;
		cr->ifs_t3_th_h = IFS_T3_TH_HIGH_BE0;
		cr->ifs_t3_th_h_m = IFS_T3_TH_HIGH_BE0_M;
		cr->ifs_t4_th_h = IFS_T4_TH_HIGH_BE0;
		cr->ifs_t4_th_h_m = IFS_T4_TH_HIGH_BE0_M;
		cr->ifs_t5_th_h = IFS_T5_TH_HIGH_BE0;
		cr->ifs_t5_th_h_m = IFS_T5_TH_HIGH_BE0_M;
		cr->ifs_clm_tx_cnt = IFSCNT_CNT_TX_BE0;
		cr->ifs_clm_tx_cnt_m = IFSCNT_CNT_TX_BE0_M;
		cr->ifs_clm_edcca_exclu_cca = IFSCNT_CNT_EDCCA_EXCLUDE_CCA_FA_BE0;
		cr->ifs_clm_edcca_exclu_cca_m = IFSCNT_CNT_EDCCA_EXCLUDE_CCA_FA_BE0_M;
		cr->ifs_clm_cckcca_exclu_fa = IFSCNT_CNT_CCKCCA_EXCLUDE_FA_BE0;
		cr->ifs_clm_cckcca_exclu_fa_m = IFSCNT_CNT_CCKCCA_EXCLUDE_FA_BE0_M;
		cr->ifs_clm_ofdmcca_exclu_fa = IFSCNT_CNT_OFDMCCA_EXCLUDE_FA_BE0;
		cr->ifs_clm_ofdmcca_exclu_fa_m = IFSCNT_CNT_OFDMCCA_EXCLUDE_FA_BE0_M;
		cr->ifs_clm_cck_fa = IFSCNT_CNT_CCKFA_BE0;
		cr->ifs_clm_cck_fa_m = IFSCNT_CNT_CCKFA_BE0_M;
		cr->ifs_clm_ofdm_fa = IFSCNT_CNT_OFDMFA_BE0;
		cr->ifs_clm_ofdm_fa_m = IFSCNT_CNT_OFDMFA_BE0_M;
		cr->ifs_clm_t1_his = IFS_T1_HIS_BE0;
		cr->ifs_clm_t1_his_m = IFS_T1_HIS_BE0_M;
		cr->ifs_clm_t2_his = IFS_T2_HIS_BE0;
		cr->ifs_clm_t2_his_m = IFS_T2_HIS_BE0_M;
		cr->ifs_clm_t3_his = IFS_T3_HIS_BE0;
		cr->ifs_clm_t3_his_m = IFS_T3_HIS_BE0_M;
		cr->ifs_clm_t4_his = IFS_T4_HIS_BE0;
		cr->ifs_clm_t4_his_m = IFS_T4_HIS_BE0_M;
		cr->ifs_clm_t5_his = IFS_T5_HIS_BAND0_BE0;
		cr->ifs_clm_t5_his_m = IFS_T5_HIS_BAND0_BE0_M;
		cr->ifs_clm_t1_avg = IFS_T1_AVG_BE0;
		cr->ifs_clm_t1_avg_m = IFS_T1_AVG_BE0_M;
		cr->ifs_clm_t2_avg = IFS_T2_AVG_BE0;
		cr->ifs_clm_t2_avg_m = IFS_T2_AVG_BE0_M;
		cr->ifs_clm_t3_avg = IFS_T3_AVG_BE0;
		cr->ifs_clm_t3_avg_m = IFS_T3_AVG_BE0_M;
		cr->ifs_clm_t4_avg = IFS_T4_AVG_BE0;
		cr->ifs_clm_t4_avg_m = IFS_T4_AVG_BE0_M;
		cr->ifs_clm_t5_avg = IFS_T5_AVG_BAND0_BE0;
		cr->ifs_clm_t5_avg_m = IFS_T5_AVG_BAND0_BE0_M;
		cr->ifs_clm_t1_cca = IFS_T1_CLM_BE0;
		cr->ifs_clm_t1_cca_m = IFS_T1_CLM_BE0_M;
		cr->ifs_clm_t2_cca = IFS_T2_CLM_BE0;
		cr->ifs_clm_t2_cca_m = IFS_T2_CLM_BE0_M;
		cr->ifs_clm_t3_cca = IFS_T3_CLM_BE0;
		cr->ifs_clm_t3_cca_m = IFS_T3_CLM_BE0_M;
		cr->ifs_clm_t4_cca = IFS_T4_CLM_BE0;
		cr->ifs_clm_t4_cca_m = IFS_T4_CLM_BE0_M;
		cr->ifs_clm_t5_cca = IFS_T5_CLM_BAND0_BE0;
		cr->ifs_clm_t5_cca_m = IFS_T5_CLM_BAND0_BE0_M;
		cr->ifs_total_cnt = IFS_TOTAL_BE0;
		cr->ifs_total_cnt_m = IFS_TOTAL_BE0_M;
		cr->ifs_clm_rdy = IFSCNT_DONE_BE0;
		cr->ifs_clm_rdy_m = IFSCNT_DONE_BE0_M;
		cr->ifs_clm_cca_opt = IFS_CCA_OPT_BE0;
		cr->ifs_clm_cca_opt_m = IFS_CCA_OPT_BE0_M;

		break;

	#endif

	default:
		BB_WARNING("[%s] BBCR Hook FAIL!\n", __func__);
		if (bb->bb_dbg_i.cr_fake_init_hook_en) {
			BB_TRACE("[%s] BBCR fake init\n", __func__);
			halbb_cr_hook_fake_init(bb, (u32 *)cr, (sizeof(struct bb_env_mntr_cr_info) >> 2));
		}
		break;
	}

	if (bb->bb_dbg_i.cr_init_hook_recorder_en) {
		BB_TRACE("[%s] BBCR Hook dump\n", __func__);
		halbb_cr_hook_init_dump(bb, (u32 *)cr, (sizeof(struct bb_env_mntr_cr_info) >> 2));
	}
}


#endif


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

#ifdef HALBB_CFO_TRK_SUPPORT

#ifdef BB_DYN_CFO_TRK_LOP

void halbb_dyn_cfo_trk_loop_en(struct bb_info *bb, bool en)
{
	bb->bb_cfo_trk_i.bb_dyn_cfo_trk_lop_i.dyn_cfo_trk_loop_en = en;
}

void halbb_cfo_trk_loop_cr_cfg(struct bb_info *bb, enum bb_dctl_state_t state)
{
	struct bb_cfo_trk_info *cfo_trk = &bb->bb_cfo_trk_i;
	struct bb_dyn_cfo_trk_lop_info *dctl = &cfo_trk->bb_dyn_cfo_trk_lop_i;
	struct bb_cfo_trk_lop_cr_info  *cr;

	if (state >= DCTL_NUM)
		return;

	if (state == dctl->dyn_cfo_trk_loop_state) {
		dctl->dctl_hold_cnt++;
		BB_DBG(bb, DBG_IC_API, "hold_cnt = %d", dctl->dctl_hold_cnt);
		return;
	}

	dctl->dyn_cfo_trk_loop_state = state;

	dctl->dctl_hold_cnt = 0;
	cr = &dctl->bb_cfo_trk_lop_cr_i[state];

	halbb_set_reg(bb, 0x4404, 0x7C00, cr->dctl_data); /*8852a CR*/
	halbb_set_reg(bb, 0x440c, 0x7C00, cr->dctl_pilot);

	BB_DBG(bb, DBG_IC_API, "dctl_data = 0x%x, dctl_pilot = 0x%x", cr->dctl_data, cr->dctl_pilot);
}

void halbb_dyn_cfo_trk_loop(struct bb_info *bb)
{
	struct bb_cfo_trk_info *cfo_trk = &bb->bb_cfo_trk_i;
	struct bb_dyn_cfo_trk_lop_info *dctl = &cfo_trk->bb_dyn_cfo_trk_lop_i;
	struct bb_link_info *link = &bb->bb_link_i;
	struct rtw_phl_stainfo_t *sta;
	u16 snr_tmp = 0;

	if (!dctl->dyn_cfo_trk_loop_en) {
		BB_DBG(bb, DBG_IC_API, "dyn_cfo_trk_loop_en = %d",
		       dctl->dyn_cfo_trk_loop_en);
		return;
	}

	if (!link->is_linked)
		return;

	if (!link->is_one_entry_only)
		return;

	sta = bb->phl_sta_info[bb->bb_link_i.one_entry_macid];

	if (!sta)
		return;

	snr_tmp = sta->hal_sta->rssi_stat.snr_ma;
	BB_DBG(bb, DBG_IC_API, "macid=%d, SNR = %s",
	       bb->bb_link_i.one_entry_macid,
	       halbb_print_sign_frac_digit2(bb, snr_tmp, 16, 4));

	BB_DBG(bb, DBG_IC_API, "snr_th{L, H} = {%d, %d}",
	       dctl->dctl_snr_th_l >> RSSI_MA_H, dctl->dctl_snr_th_h >> RSSI_MA_H);

	if (snr_tmp >= dctl->dctl_snr_th_h)
		halbb_cfo_trk_loop_cr_cfg(bb, DCTL_LINK);
	else if (snr_tmp <= dctl->dctl_snr_th_l)
		halbb_cfo_trk_loop_cr_cfg(bb, DCTL_SNR);
}

void halbb_dyn_cfo_trk_loop_init(struct bb_info *bb)
{
	struct bb_cfo_trk_info *cfo_trk = &bb->bb_cfo_trk_i;
	struct bb_dyn_cfo_trk_lop_info *dctl = &cfo_trk->bb_dyn_cfo_trk_lop_i;

	dctl->dyn_cfo_trk_loop_en = false;
	dctl->dyn_cfo_trk_loop_state = DCTL_SNR;
	dctl->dctl_snr_th_l = (u16)5 << RSSI_MA_H;
	dctl->dctl_snr_th_h = (u16)7 << RSSI_MA_H;

	dctl->bb_cfo_trk_lop_cr_i[DCTL_SNR].dctl_data = 0x1f;
	dctl->bb_cfo_trk_lop_cr_i[DCTL_SNR].dctl_pilot = 0x7;

	dctl->bb_cfo_trk_lop_cr_i[DCTL_LINK].dctl_data = 0x1f;
	dctl->bb_cfo_trk_lop_cr_i[DCTL_LINK].dctl_pilot = 0x9;
}

#endif

void halbb_digital_cfo_comp(struct bb_info *bb, s32 curr_cfo)
{
	switch (bb->ic_type) {

	#ifdef BB_8852A_2_SUPPORT
	case BB_RTL8852A:
		halbb_digital_cfo_comp_8852a_2(bb, curr_cfo);

		break;
	#endif

	#ifdef BB_8852B_SUPPORT
	case BB_RTL8852B:
		halbb_digital_cfo_comp_8852b(bb, curr_cfo);

		break;
	#endif

	#ifdef BB_8852C_SUPPORT
	case BB_RTL8852C:
		halbb_digital_cfo_comp_8852c(bb, curr_cfo);

		break;
	#endif

	#ifdef BB_8192XB_SUPPORT
	case BB_RTL8192XB:
		halbb_digital_cfo_comp_8192xb(bb, curr_cfo);

		break;
	#endif

	#ifdef BB_8851B_SUPPORT
	case BB_RTL8851B:
		halbb_digital_cfo_comp_8851b(bb, curr_cfo);

		break;
	#endif

	default:
		break;
	}
}

void halbb_digital_cfo_comp_init(struct bb_info *bb)
{
	struct bb_cfo_trk_info *cfo_trk = &bb->bb_cfo_trk_i;
	struct bb_cfo_trk_cr_info *cr = &bb->bb_cfo_trk_i.bb_cfo_trk_cr_i;

	// 0x4494[29] Whether the memory of r_cfo_comp_312p5khz is valid
	halbb_set_reg(bb, cr->r_cfo_comp_seg0_vld, cr->r_cfo_comp_seg0_vld_m, 1);

	// 0x4490[27:24] r_cfo_weighting
	halbb_set_reg(bb, cr->r_cfo_wgting, cr->r_cfo_wgting_m, 8);

	/* 0xD248 */
	/* All scenario set CFO comp.*/
	rtw_hal_mac_write_msk_pwr_reg(bb->hal_com, 0, 0xd248, 0x7, 0);
}

void halbb_cfo_trk_reset(struct bb_info *bb)
{
	struct bb_cfo_trk_info *bb_cfo_trk = &bb->bb_cfo_trk_i;

	BB_DBG(bb, DBG_CFO_TRK, "%s ======>\n", __func__);

	bb_cfo_trk->is_adjust = false;

	if (bb_cfo_trk->crystal_cap > bb_cfo_trk->def_x_cap) {
		halbb_set_crystal_cap(bb, bb_cfo_trk->crystal_cap - 1);
	} else if (bb_cfo_trk->crystal_cap < bb_cfo_trk->def_x_cap) {
		halbb_set_crystal_cap(bb, bb_cfo_trk->crystal_cap + 1);
	}
	BB_DBG(bb, DBG_CFO_TRK, "(0x%x) approach to dflt_val=(0x%x)\n",
	       bb_cfo_trk->crystal_cap, bb_cfo_trk->def_x_cap);
}

#ifdef HALBB_CFO_DAMPING_CHK
void halbb_cfo_recorder(struct bb_info *bb, u8 step_curr, bool is_positive)
{
	struct bb_cfo_trk_info *bb_cfo_trk = &bb->bb_cfo_trk_i;
	struct bb_cfo_rc_info *bb_cfo_rc = &bb_cfo_trk->bb_cfo_rc_i;
	u8 step_pre = bb_cfo_rc->step_history[0];
	u8 step_up = 0;

	BB_DBG(bb, DBG_CFO_TRK,  "%s ======>\n", __func__);

	step_pre = bb_cfo_rc->step_history[0];
	step_up = is_positive ? 1 : 0;
	bb_cfo_rc->step_bitmap = (bb_cfo_rc->step_bitmap << 1) | step_up;

	bb_cfo_rc->step_history[5] = bb_cfo_rc->step_history[4];
	bb_cfo_rc->step_history[4] = bb_cfo_rc->step_history[3];
	bb_cfo_rc->step_history[3] = bb_cfo_rc->step_history[2];
	bb_cfo_rc->step_history[2] = bb_cfo_rc->step_history[1];
	bb_cfo_rc->step_history[1] = bb_cfo_rc->step_history[0];
	bb_cfo_rc->step_history[0] = step_curr;

	BB_DBG(bb, DBG_CFO_TRK, "step_history[5:0] = {%02d, %02d, %02d, %02d, %02d, %02d}\n",
		bb_cfo_rc->step_history[5], bb_cfo_rc->step_history[4], bb_cfo_rc->step_history[3], bb_cfo_rc->step_history[2],
		bb_cfo_rc->step_history[1], bb_cfo_rc->step_history[0]);
	BB_DBG(bb, DBG_CFO_TRK, "step_bitmap[5:0]=0x%x{ %d,  %d,  %d,  %d,  %d,  %d}\n",
	       bb_cfo_rc->step_bitmap,
	       (bb_cfo_rc->step_bitmap & BIT5) >> 5,
	       (bb_cfo_rc->step_bitmap & BIT4) >> 4,
	       (bb_cfo_rc->step_bitmap & BIT3) >> 3,
	       (bb_cfo_rc->step_bitmap & BIT2) >> 2,
	       (bb_cfo_rc->step_bitmap & BIT1) >> 1,
	       bb_cfo_rc->step_bitmap & BIT0);
}

void halbb_cfo_damping_chk(struct bb_info *bb)
{
	struct bb_cfo_trk_info *bb_cfo_trk = &bb->bb_cfo_trk_i;
	struct bb_cfo_rc_info *bb_cfo_rc = &bb_cfo_trk->bb_cfo_rc_i;
	u8 diff1 = 0, diff2 = 0;
	bool step_pattern_match = false;
	u32 time_tmp = 0;

	if (!bb->bb_link_i.is_linked)
		return;

	BB_DBG(bb, DBG_CFO_TRK, "%s ======>\n", __func__);

	/*@== Release Damping ================================================*/
	if (bb_cfo_rc->damping_lock_en) {
		BB_DBG(bb, DBG_CFO_TRK,
		       "[Damping Limit!] limit_time=%d, phydm_sys_up_time=%d\n",
		       bb_cfo_rc->limit_time, bb->bb_sys_up_time);

		time_tmp = bb_cfo_rc->limit_time + CFO_LIMIT_PERIOD;

		if (time_tmp < bb->bb_sys_up_time) {
			bb_cfo_rc->damping_lock_en = false;
		} else {
			halbb_cfo_trk_reset(bb); /*xcap to default value */
		}

		return;
	}

	if (bb_cfo_rc->force_damping_step) {
		BB_DBG(bb, DBG_CFO_TRK,
		       "[Force damping step!] limit_time=%d, phydm_sys_up_time=%d\n",
		       bb_cfo_rc->limit_time, bb->bb_sys_up_time);

		time_tmp = bb_cfo_rc->limit_time + CFO_LIMIT_PERIOD;

		if (time_tmp < bb->bb_sys_up_time)
			bb_cfo_rc->force_damping_step = false;

		return;
	}

	/*@== Damping Pattern Check===========================================*/
	if ((bb_cfo_rc->step_bitmap & 0xf) == 0x5) {
		BB_DBG(bb, DBG_CFO_TRK, "[Type:0] map=0x5\n");
	/*@ 4b'0101
	*/
		if (bb_cfo_rc->step_history[0] > bb_cfo_rc->step_history[1]) {
			diff1 = bb_cfo_rc->step_history[0] - bb_cfo_rc->step_history[1];
		}

		if (bb_cfo_rc->step_history[2] > bb_cfo_rc->step_history[3]) {
			diff2 = bb_cfo_rc->step_history[2] - bb_cfo_rc->step_history[3];
		}

			step_pattern_match = true;
	}

	if (diff1 >= 2 && diff2 >= 2 && step_pattern_match) {
		bb_cfo_rc->damping_lock_en = true;
		bb_cfo_rc->limit_time = bb->bb_sys_up_time;
		BB_DBG(bb, DBG_CFO_TRK, "[CFO][Start damping_limit!] limit_time=%d\n",
			   bb_cfo_rc->limit_time);
	} else if (bb_cfo_rc->step_history[0] >= 3 && bb_cfo_rc->step_history[1] >= 3
		   && bb_cfo_rc->step_history[2] >= 3 && bb_cfo_rc->step_history[3] >= 3
		   && step_pattern_match) {
		bb_cfo_rc->force_damping_step = true;
		bb_cfo_rc->limit_time = bb->bb_sys_up_time;
		BB_DBG(bb, DBG_CFO_TRK, "[CFO][Force damping step!] limit_time=%d\n",
			   bb_cfo_rc->limit_time);
	}

	BB_DBG(bb, DBG_CFO_TRK,  "[lock_en=%d] ptrn_match=%d, diff1=%d, diff2=%d\n",
	       bb_cfo_rc->damping_lock_en, step_pattern_match, diff1, diff2);

	BB_DBG(bb, DBG_CFO_TRK,  "[force_en=%d] ptrn_match=%d\n",
	       bb_cfo_rc->force_damping_step, step_pattern_match);
}

void halbb_cfo_damping_chk_init(struct bb_info *bb)
{
	struct bb_cfo_trk_info *bb_cfo_trk = &bb->bb_cfo_trk_i;
	struct bb_cfo_rc_info *bb_cfo_rc = &bb_cfo_trk->bb_cfo_rc_i;

	BB_DBG(bb, DBG_CFO_TRK,  "%s ======>\n", __func__);
	halbb_mem_set(bb, bb_cfo_rc, 0, sizeof(struct bb_cfo_rc_info));
}
#endif

void halbb_cfo_diver_init(struct bb_info *bb)
{
	struct bb_cfo_trk_info *bb_cfo_trk = &bb->bb_cfo_trk_i;
	struct bb_cfo_diver_info *bb_cfo_div = &bb_cfo_trk->bb_cfo_div_i;

	BB_DBG(bb, DBG_CFO_TRK,  "%s ======>\n", __func__);
	halbb_mem_set(bb, bb_cfo_div, 0, sizeof(struct bb_cfo_diver_info));
}

void halbb_cfo_acc_io_en(struct bb_info *bb)
{
	struct bb_cfo_trk_info *cfo_trk = &bb->bb_cfo_trk_i;

	BB_DBG(bb, DBG_CFO_TRK, "[%s]===>\n", __func__);

	if (!cfo_trk->cfo_trig_by_timer_en)
		return;

	halbb_cfo_dm(bb);
	halbb_cfg_timers(bb, BB_SET_TIMER, &bb->bb_cfo_trk_i.cfo_timer_i);
}

void halbb_cfo_acc_callback(void *context)
{
	struct bb_info *bb = (struct bb_info *)context;
	struct bb_cfo_trk_info *cfo_trk = &bb->bb_cfo_trk_i;
	struct halbb_timer_info *timer = &cfo_trk->cfo_timer_i;

	BB_DBG(bb, DBG_CFO_TRK, "[%s]===>\n", __func__);

	timer->timer_state = BB_TIMER_IDLE;

	if (bb->phl_com->hci_type == RTW_HCI_PCIE)
		halbb_cfo_acc_io_en(bb);
	else
		rtw_hal_cmd_notify(bb->phl_com, MSG_EVT_NOTIFY_BB, (void *)(&timer->event_idx), bb->bb_phy_idx);
}

void halbb_cfo_acc_timer_init(struct bb_info *bb)
{
	struct halbb_timer_info *timer = &bb->bb_cfo_trk_i.cfo_timer_i;

	BB_DBG(bb, DBG_INIT, "[%s]\n", __func__);

	timer->event_idx = BB_EVENT_TIMER_CFO;
	timer->timer_state = BB_TIMER_IDLE;

	halbb_init_timer(bb, &timer->timer_list, halbb_cfo_acc_callback, bb, "halbb_cfo_timer");
}

void halbb_cfo_trk_init(struct bb_info *bb)
{
	struct bb_cfo_trk_info *bb_cfo_trk = &bb->bb_cfo_trk_i;
	struct rtw_phl_com_t *phl = bb->phl_com;
	struct dev_cap_t *dev = &phl->dev_cap;

	BB_DBG(bb, DBG_CFO_TRK, "[%s]=========>\n", __func__);

	/* Init crystal cap from efuse */
	bb_cfo_trk->def_x_cap = bb->phl_com->dev_cap.xcap & 0x7f;
	bb_cfo_trk->crystal_cap = bb_cfo_trk->def_x_cap;
	bb_cfo_trk->x_cap_ub = ((bb_cfo_trk->def_x_cap + CFO_VALID_BOUNDARY) > 0x7f)
				 ? 0x7f : (bb_cfo_trk->def_x_cap + CFO_VALID_BOUNDARY);
	bb_cfo_trk->x_cap_lb = ((bb_cfo_trk->def_x_cap - CFO_VALID_BOUNDARY) < 0x1)
				 ? 0x1 : (bb_cfo_trk->def_x_cap - CFO_VALID_BOUNDARY);
	bb_cfo_trk->is_adjust = false;

	bb_cfo_trk->x_cap_ofst = 0;
	bb_cfo_trk->no_pkt_cnt = 0;

	bb_cfo_trk->cfo_th[0] = CFO_TRK_TH_1 << 2;
	bb_cfo_trk->cfo_th[1] = CFO_TRK_TH_2 << 2;
	bb_cfo_trk->cfo_th[2] = CFO_TRK_TH_3 << 2;
	bb_cfo_trk->cfo_th[3] = CFO_TRK_TH_4 << 2;

	bb_cfo_trk->step[0] = CFO_STEP_1;
	bb_cfo_trk->step[1] = CFO_STEP_2;
	bb_cfo_trk->step[2] = CFO_STEP_3;
	bb_cfo_trk->step[3] = CFO_STEP_4;

	bb_cfo_trk->cfo_th_en = CFO_TRK_ENABLE_TH << 2;
	bb_cfo_trk->cfo_th_stop = CFO_TRK_STOP_TH << 2;

	bb_cfo_trk->sw_comp_fine_tune = CFO_SW_COMP_FINE_TUNE << 2;

	bb_cfo_trk->multi_sta_cfo_mode = TP_BASED_AVG_MODE;
	bb_cfo_trk->man_cfo_tol = false;
	BB_DBG(bb, DBG_CFO_TRK, "Default xcap=0x%x\n", bb_cfo_trk->def_x_cap);
	BB_DBG(bb, DBG_CFO_TRK, "xcap upper_bound=0x%x\n", bb_cfo_trk->x_cap_ub);
	BB_DBG(bb, DBG_CFO_TRK, "xcap lower_bound=0x%x\n", bb_cfo_trk->x_cap_lb);

	// For manually fine tune digital cfo
	halbb_set_reg_cmn(bb, 0x4264, 0x00000003, 1, bb->bb_phy_idx);
	bb_cfo_trk->tb_tx_comp_cfo_th = DIGI_CFO_COMP_LIMIT << 2;
	halbb_digital_cfo_comp_init(bb);

	bb_cfo_trk->cfo_period_cnt = CFO_PERIOD_CNT;
	bb_cfo_trk->cfo_timer_i.cb_time = CFO_COMP_PERIOD;
	bb_cfo_trk->cfo_trig_by_timer_en = false;

	bb_cfo_trk->cfo_tf_cnt_th = CFO_TF_CNT_TH;
	bb_cfo_trk->cfo_tf_cnt_pre = 0;
	bb_cfo_trk->bb_cfo_trk_acc_mode = CFO_ACC_MODE_1;

	bb_cfo_trk->bb_cfo_trk_state = CFO_STATE_0;
	bb_cfo_trk->bb_cfo_trk_cnt = 0;

	if (bb->ic_type & (BB_RTL8852A | BB_RTL8852C | BB_RTL8192XB))
		bb_cfo_trk->cfo_src = CFO_SRC_PREAMBLE;
	else
		bb_cfo_trk->cfo_src = CFO_SRC_FD;

	// For NIC only, to speed up sw CFO compensation
	bb_cfo_trk->cfo_dyn_acc_en = (dev->rfe_type < 50) ? true : false;
	bb_cfo_trk->cfo_trk_by_data_en = false;

#ifdef HALBB_CFO_DAMPING_CHK
	halbb_cfo_damping_chk_init(bb);
#endif
	halbb_cfo_diver_init(bb);
}

void halbb_set_crystal_cap(struct bb_info *bb, u8 crystal_cap)
{
	struct bb_cfo_trk_info *bb_cfo_trk = &bb->bb_cfo_trk_i;
	struct rtw_hal_com_t *hal = bb->hal_com;
	u32 sc_xi_val = 0, sc_xo_val = 0;

	if (bb_cfo_trk->crystal_cap == crystal_cap)
		return;

	if (crystal_cap > 0x7F) {
		BB_DBG(bb, DBG_CFO_TRK, "crystal_cap(0x%x) > 0x7F\n", crystal_cap);
		crystal_cap = 0x7F;
	}

	rtw_hal_mac_set_xcap(hal, SC_XO, (u32)crystal_cap & 0x7F);
	rtw_hal_mac_set_xcap(hal, SC_XI, (u32)crystal_cap & 0x7F);

	rtw_hal_mac_get_xcap(hal, SC_XO, &sc_xo_val);
	rtw_hal_mac_get_xcap(hal, SC_XI, &sc_xi_val);
	
	BB_DBG(bb, DBG_CFO_TRK, "Set sc_xi/xo= {0x%x, 0x%x}\n", sc_xi_val, sc_xo_val);

	bb_cfo_trk->crystal_cap = (u8)sc_xi_val;

	bb_cfo_trk->x_cap_ofst = (s8)DIFF_2(bb_cfo_trk->crystal_cap, bb_cfo_trk->def_x_cap);
	if (bb_cfo_trk->crystal_cap < bb_cfo_trk->def_x_cap)
		bb_cfo_trk->x_cap_ofst = bb_cfo_trk->x_cap_ofst * (-1);
}

void halbb_crystal_cap_adjust(struct bb_info *bb, s32 curr_cfo)
{
	struct bb_cfo_trk_info *bb_cfo_trk = &bb->bb_cfo_trk_i;
	struct bb_cfo_rc_info *bb_cfo_rc = &bb_cfo_trk->bb_cfo_rc_i;
	struct bb_path_info *bb_path = &bb->bb_path_i;
	//enum bb_mlo_mode_info mode = MLO_0_PLUS_2;
	u8 x_cap = bb_cfo_trk->crystal_cap;
	u8 step = 0;
	s32 cfo_abs = ABS_32(curr_cfo);
	bool is_positive = IS_GREATER(curr_cfo, 0);

	BB_DBG(bb, DBG_CFO_TRK, "[CFO_DBG] %s ======>\n", __func__);

	if (!bb_cfo_trk->is_adjust) {
		/* If cfo_avg > th, enable tracking */
		if (cfo_abs > bb_cfo_trk->cfo_th_en)
			bb_cfo_trk->is_adjust = true;
	} else {
		if (cfo_abs <= bb_cfo_trk->cfo_th_stop)
			bb_cfo_trk->is_adjust = false;
	}

	if (!bb_cfo_trk->is_adjust) {
		BB_DBG(bb, DBG_CFO_TRK, "Stop Tracking\n");
		/*halbb_digital_cfo_comp(bb);*/
		return;
	}

	/*Adjust Crystal Cap. */
	if (cfo_abs > bb_cfo_trk->cfo_th[3])
		step = bb_cfo_trk->step[3];
	else if (cfo_abs > bb_cfo_trk->cfo_th[2])
		step = bb_cfo_trk->step[2];
	else if (cfo_abs > bb_cfo_trk->cfo_th[1])
		step = bb_cfo_trk->step[1];
	else if (cfo_abs > bb_cfo_trk->cfo_th[0])
		step = bb_cfo_trk->step[0];
	else
		return;

#ifdef HALBB_CFO_DAMPING_CHK
	/*Record step History*/
	halbb_cfo_recorder(bb, step, is_positive);
	/*CFO Damping Check*/
	halbb_cfo_damping_chk(bb);
	if (bb_cfo_rc->damping_lock_en) {
		BB_DBG(bb, DBG_CFO_TRK, "cfo damping\n");
		return;
	}
	if (bb_cfo_rc->force_damping_step)
		step = 1;
#endif

#if 0
#if defined(BB_8922A_SUPPORT) && defined(HALBB_DBCC_SUPPORT)
	/*Avoid overshooting issue when 1+1 MLO, TBD*/
	if (bb->ic_type == BB_RTL8922A && mode == MLO_1_PLUS_1 && step >= bb_cfo_trk->step[1]) {
		BB_DBG(bb, DBG_CFO_TRK, "MLO = %d, decrease step from %d to %d\n",
		       mode, step, step >> 1);
		step >>= 1;
	}
#endif
#endif
	if (is_positive)
		x_cap += step;
	else if (x_cap > step)
		x_cap -= step;

	BB_DBG(bb, DBG_CFO_TRK, "TH[en, stop]={%d, %d}, TH[3:0]={%d, %d, %d, %d}\n",
	       bb_cfo_trk->cfo_th_en >> 2, bb_cfo_trk->cfo_th_stop >> 2,
	       bb_cfo_trk->cfo_th[3] >> 2, bb_cfo_trk->cfo_th[2] >> 2,
	       bb_cfo_trk->cfo_th[1] >> 2, bb_cfo_trk->cfo_th[0] >> 2);

	BB_DBG(bb, DBG_CFO_TRK, "step=%s%d\n",
	       (is_positive) ? "+" : "-", step);

	halbb_set_crystal_cap(bb, x_cap);
}

s32 halbb_avg_cfo_calc(struct bb_info *bb)
{
	struct bb_cfo_trk_info *bb_cfo_trk = &bb->bb_cfo_trk_i;
	struct bb_link_info *bb_link = &bb->bb_link_i;
	struct rtw_phl_stainfo_t *sta;
	struct rtw_cfo_info *cfo_t = NULL;
	s32 cfo_all_avg = 0;
	u16 macid;

	sta = bb->phl_sta_info[bb_link->one_entry_macid];

	if (!sta)
		return bb_cfo_trk->cfo_avg_pre;

	macid = sta->macid;
	cfo_t = &sta->hal_sta->cfo_stat;

	BB_DBG(bb, DBG_CFO_TRK, "one-entry-only macid=%d\n", macid);

	cfo_all_avg = HALBB_DIV(cfo_t->cfo_tail, (s32)cfo_t->cfo_cnt);

	BB_DBG(bb, DBG_CFO_TRK, "Total cfo=(%d), pkt_cnt=(%d), avg_cfo=(%d)\n",
	       cfo_t->cfo_tail >> 2, cfo_t->cfo_cnt, cfo_all_avg >> 2);
	return cfo_all_avg;
}

s32 halbb_multi_sta_avg_cfo_calc(struct bb_info *bb)
{
	struct bb_cfo_trk_info *bb_cfo_trk = &bb->bb_cfo_trk_i;
	struct rtw_hal_com_t *hal = bb->hal_com;
	struct rtw_phl_com_t *phl = bb->phl_com;
	struct dev_cap_t *dev = &phl->dev_cap;
	u8 band = bb->hal_com->band[0].cur_chandef.band;
	struct bb_link_info *bb_link = &bb->bb_link_i;
	struct rtw_phl_stainfo_t *sta;
	struct rtw_cfo_info *cfo_t = NULL;
	s32 target_cfo = 0;
	s32 cfo_khz_all = 0;
	s32 cfo_khz_all_tp_wgt = 0;
	s32 cfo_avg = 0;
	s32 max_cfo_lb= 0x80000000;
	s32 min_cfo_ub = 0x7fffffff;
	u16 cfo_cnt_all = 0;
	u8 active_entry_cnt = 0, sta_cnt = 0;
	u32 tp_all = 0;
	u16 active_entry = 0;
	u8 i;
	u8 cfo_tol = 0;
	u16 macid;

	BB_DBG(bb, DBG_CFO_TRK, "Multi entry cfo_trk\n");

	if (!bb_cfo_trk->man_cfo_tol) {
		if (band == BAND_ON_24G)
			bb_cfo_trk->sta_cfo_tolerance = STA_CFO_TOLERANCE_2G;
		else
			bb_cfo_trk->sta_cfo_tolerance = STA_CFO_TOLERANCE_5G;
	}

	BB_DBG(bb, DBG_CFO_TRK, "Default multi-sta cfo_trk tolerance=%d\n",
						 bb_cfo_trk->sta_cfo_tolerance);

	/* There are two extra multi-sta strategies remaining as references*/
#if 0
	if (bb_cfo_trk->multi_sta_cfo_mode == PKT_BASED_AVG_MODE) {
		/* Method-1: Centroid pkt based cfo tracking: Compute average cfo from all sta PPDUs */
		/* Just like one entry only method*/
		BB_DBG(bb, DBG_CFO_TRK, "Pkt based average mode\n");

		for (i = 0; i < PHL_MAX_STA_NUM; i++) {

			if (!bb->sta_exist[i])
				continue;

			if (bb_cfo_trk->cfo_cnt[i] == 0)
				continue;

			//BB_DBG(bb, DBG_CFO_TRK, "Macid=%d\n", i);
			cfo_khz_all += bb_cfo_trk->cfo_tail[i];
			cfo_cnt_all += bb_cfo_trk->cfo_cnt[i];
			cfo_avg = HALBB_DIV(cfo_khz_all, (s32)cfo_cnt_all);

			BB_DBG(bb, DBG_CFO_TRK, "s(32,2) Multi-sta total cfo=(%d), pkt_cnt=(%d), avg_cfo=(%d)\n",
					cfo_khz_all, cfo_cnt_all, cfo_avg);

			target_cfo = cfo_avg;
		}
	} else if (bb_cfo_trk->multi_sta_cfo_mode == ENTRY_BASED_AVG_MODE) {
		/* Method-2: Entry based cfo tracking: Compute average cfo of ertries */
		BB_DBG(bb, DBG_CFO_TRK, "Entry based average mode\n");

		for (i = 0; i < PHL_MAX_STA_NUM; i++) {

			if (!bb->sta_exist[i])
				continue;

			if (bb_cfo_trk->cfo_cnt[i] == 0)
				continue;

			active_entry |= BIT(i);
			bb_cfo_trk->cfo_avg[i] = HALBB_DIV(bb_cfo_trk->cfo_tail[i],
				                      (s32)bb_cfo_trk->cfo_cnt[i]);
			cfo_khz_all += bb_cfo_trk->cfo_avg[i];

			BB_DBG(bb, DBG_CFO_TRK, "Macid=%d, cfo_avg=%d\n",
				                     i, bb_cfo_trk->cfo_avg[i]);
		}

		/* Average of all entries */
		sta_cnt = (u8) halbb_ones_num_in_bitmap(active_entry, sizeof(active_entry) * 8);
		cfo_avg = HALBB_DIV(cfo_khz_all, (s32)sta_cnt);

		BB_DBG(bb, DBG_CFO_TRK, "s(32,2) Multi-sta cfo_acc=(%d), entry_cnt=(%d), avg_cfo=(%d)\n",
					cfo_khz_all, sta_cnt, cfo_avg);

		target_cfo = cfo_avg;

	}
#endif

	/* Method-3: Tp based cfo tracking: With Tp_wgt */
	BB_DBG(bb, DBG_CFO_TRK, "Throughput based average mode\n");
	cfo_tol = bb_cfo_trk->sta_cfo_tolerance;

	for (i = 0; i < PHL_MAX_STA_NUM; i++) {

		if (!bb->sta_exist[i])
			continue;

		sta = bb->phl_sta_info[i];
		macid = sta->macid;

		if (!is_sta_active(sta))
			continue;

		if ((dev->rfe_type >= 50) && (sta->macid == 0))
			continue;

		cfo_t = &sta->hal_sta->cfo_stat;

		sta_cnt++;

		if (cfo_t->cfo_cnt != 0) {
			cfo_t->cfo_avg =
				      HALBB_DIV(cfo_t->cfo_tail, (s32)cfo_t->cfo_cnt);
			active_entry_cnt++;
		} else { /* Linked, but no pkts received*/
			cfo_t->cfo_avg = cfo_t->pre_cfo_avg;
		}

		/* Calculate the cfo torlence window */
		if ((cfo_t->cfo_avg - cfo_tol) > max_cfo_lb)
			max_cfo_lb = cfo_t->cfo_avg - cfo_tol;
		if ((cfo_t->cfo_avg + cfo_tol) < min_cfo_ub)
			min_cfo_ub = cfo_t->cfo_avg + cfo_tol;

		cfo_khz_all += cfo_t->cfo_avg;

		/* Acc throuhgput of all entries */
		tp_all += cfo_t->tp;

		/* Multiple tp_wgt first*/
		cfo_khz_all_tp_wgt += cfo_t->cfo_avg * cfo_t->tp;

		BB_DBG(bb, DBG_CFO_TRK, "[%d] Macid=%d, cfo_avg=%d, tp=%d\n",
			  i, macid, cfo_t->cfo_avg, cfo_t->tp);

		cfo_t->pre_cfo_avg = cfo_t->cfo_avg;

		if (sta_cnt >= bb->hal_com->assoc_sta_cnt)
			break;
	}

	/* Average of all entries with tp_wgt */
	BB_DBG(bb, DBG_CFO_TRK, "Assoc. sta cnt(%d)\n", sta_cnt);
	BB_DBG(bb, DBG_CFO_TRK, "Active sta cnt(%d)\n", active_entry_cnt);

	/* Div. tp_all to normalize wgt*/
	cfo_avg =  HALBB_DIV(cfo_khz_all_tp_wgt, (s32)tp_all);

	BB_DBG(bb, DBG_CFO_TRK, "s(32,2) Multi-sta cfo with tp_wgt=(%d), avg_cfo=(%d)\n",
					   cfo_khz_all_tp_wgt, cfo_avg);

	/* The target cfo need inside the cfo torlence window*/
	BB_DBG(bb, DBG_CFO_TRK, "max_cfo_lb=%d, min_cfo_ub=%d\n",
					        max_cfo_lb, min_cfo_ub);

	if (max_cfo_lb <= min_cfo_ub) {
		BB_DBG(bb, DBG_CFO_TRK, "cfo torlence win. size = %d\n",
					       min_cfo_ub - max_cfo_lb);
		if (cfo_avg < max_cfo_lb) {
			BB_DBG(bb, DBG_CFO_TRK, "cfo_avg < win_lb\n");
			target_cfo = max_cfo_lb;
		} else if (cfo_avg > min_cfo_ub) {
			BB_DBG(bb, DBG_CFO_TRK, "cfo_avg > win_ub\n");
			target_cfo = min_cfo_ub;
		} else {
			target_cfo = cfo_avg;
		}
	} else { /* No intersection of multi-sta cfo torlence, avg. of all cfo of entries */
		BB_DBG(bb, DBG_CFO_TRK, "No intersection of cfo torlence windows\n");
		target_cfo = HALBB_DIV(cfo_khz_all, (s32)sta_cnt);
	}

	BB_DBG(bb, DBG_CFO_TRK, "s(32,2) Final target cfo=(%d)\n", target_cfo);

	return target_cfo;
}

void halbb_set_cfo_pause_val(struct bb_info *bb, u32 *val_buf, u8 val_len)
{
	if (val_len != 1) {
		BB_DBG(bb, DBG_CFO_TRK, "[Error][CFO]Need val_len=1\n");
		return;
	}
	BB_DBG(bb, DBG_CFO_TRK, "[%s] len=%d, val[0]=0x%x\n", __func__, val_len, val_buf[0]);

	halbb_set_crystal_cap(bb, (u8)(val_buf[0] & 0xff));
}

void
halbb_cfo_counter_rst(struct bb_info *bb)
{
	struct bb_cfo_trk_info *bb_cfo_trk = &bb->bb_cfo_trk_i;
	struct rtw_phl_stainfo_t *sta;
	struct rtw_cfo_info *cfo_t = NULL;
	u8 i, sta_cnt = 0;

	for (i = 0; i < PHL_MAX_STA_NUM; i++) {

		if (!bb->sta_exist[i])
			continue;

		sta = bb->phl_sta_info[i];

		if (!is_sta_active(sta))
			continue;
		
		cfo_t = &sta->hal_sta->cfo_stat;

		cfo_t->cfo_tail = 0;
		cfo_t->cfo_cnt = 0;
		cfo_t->cfo_avg = 0;
		cfo_t->tp = 0;

		sta_cnt++;
		if (sta_cnt >= bb->hal_com->assoc_sta_cnt)
			break;
	}

	bb_cfo_trk->cfo_pkt_cnt = 0;
}

bool
halbb_cfo_trk_abort(struct bb_info *bb)
{
	struct bb_cfo_trk_info *cfo_trk = &bb->bb_cfo_trk_i;
	s32 cfo_avg = 0;

	/* support_ability */
	if (!(bb->support_ability & BB_CFO_TRK)) {
		BB_DBG(bb, DBG_CFO_TRK, "[%s] DISABLED\n", __func__);
		halbb_cfo_trk_reset(bb);
		return true;
	}

	if (bb->pause_ability & BB_CFO_TRK) {
		cfo_avg = halbb_avg_cfo_calc(bb);
		halbb_print_sign_frac_digit(bb, cfo_avg, 32, 2, bb->dbg_buf, HALBB_SNPRINT_SIZE);
		
		BB_DBG(bb, DBG_CFO_TRK, "Return: Pause CFO_TRK in LV=%d\n",
		       bb->pause_lv_table.lv_cfo);
		
		BB_DBG(bb, DBG_CFO_TRK, "Xcap=0x%x, cfo_avg=%s\n",
		       cfo_trk->crystal_cap, bb->dbg_buf);

		halbb_cfo_counter_rst(bb);
		return true;
	}

	return false;
}

void halbb_cfo_trk(struct bb_info *bb, s32 curr_cfo)
{
	struct bb_cfo_trk_info *bb_cfo_trk = &bb->bb_cfo_trk_i;
	struct bb_link_info *bb_link = &bb->bb_link_i;
	struct rtw_phl_com_t *phl = bb->phl_com;
	struct dev_cap_t *dev = &phl->dev_cap;

	if (halbb_cfo_trk_abort(bb))
		return;

	BB_DBG(bb, DBG_CFO_TRK, "[%s]\n", __func__);

	if (!bb_link->is_linked) {
		BB_DBG(bb, DBG_CFO_TRK, "is_linked=%d\n",bb_link->is_linked);

		halbb_cfo_trk_reset(bb); /*xcap to default value */
		return;
	}

	/* To avoid NIC soft-AP going to multi-sta cfo until AP side is verified for a long time*/
	if ((!bb_link->is_one_entry_only) && (dev->rfe_type < 50)) {
		BB_DBG(bb, DBG_CFO_TRK, "rfe_type=%d, is_one_entry_only=%d\n",
				     dev->rfe_type, bb_link->is_one_entry_only);

		halbb_cfo_trk_reset(bb); /*xcap to default value */
		return;
	}

	if (bb_link->first_connect)
		return;

	if (curr_cfo == 0) {
		BB_DBG(bb, DBG_CFO_TRK, "curr_cfo=0\n");
		return;
	}

	halbb_crystal_cap_adjust(bb, curr_cfo); /*Decide xcap need to adjust or not */
}

void halbb_cfo_ul_ofdma_acc_enable(struct bb_info *bb)
{
	struct bb_cfo_trk_info *bb_cfo_trk = &bb->bb_cfo_trk_i;

	BB_DBG(bb, DBG_CFO_TRK, "[%s]\n", __func__);
	bb_cfo_trk->bb_cfo_trk_acc_mode = CFO_ACC_MODE_1;
}

/*need to call before cfo_trk_init for embedded system*/
void halbb_cfo_ul_ofdma_acc_disable(struct bb_info *bb)
{
	struct bb_cfo_trk_info *bb_cfo_trk = &bb->bb_cfo_trk_i;

	BB_DBG(bb, DBG_CFO_TRK, "[%s]\n", __func__);
	if (bb==NULL)
	{
		BB_DBG(bb, DBG_CFO_TRK, "cfo_ul_ofdma_acc_disable fail !\n");
		return;
	}
	bb_cfo_trk->bb_cfo_trk_acc_mode = CFO_ACC_MODE_0;
}

bool halbb_cfo_acc_mode_en(struct bb_info *bb)
{
	struct bb_cfo_trk_info *cfo_trk = &bb->bb_cfo_trk_i;
	struct bb_link_info *link = &bb->bb_link_i;
	struct rtw_phl_com_t *phl = bb->phl_com;
	struct rtw_hal_com_t *hal = bb->hal_com;
	struct dev_cap_t *dev = &phl->dev_cap;
	struct rtw_phl_stainfo_t *sta;
	u8 sta_cnt = 0;
	u32 i = 0, cfo_tf_cnt = 0, cfo_tf_cnt_cur = 0;
	bool is_ul_ofdma = false;

	if (!cfo_trk->cfo_dyn_acc_en)
		return false;

	for (i = 0; i < PHL_MAX_STA_NUM; i++) {
		if (!bb->sta_exist[i])
			continue;
		sta = bb->phl_sta_info[i];
		if (!is_sta_active(sta))
			continue;
		sta_cnt++;
		cfo_tf_cnt_cur += sta->stats.rx_tf_cnt;
		BB_DBG(bb, DBG_CFO_TRK, "[%d] macid=%d\n", i, sta->macid);
		if (sta_cnt >= hal->assoc_sta_cnt)
			break;
	}

	cfo_tf_cnt = SUBTRACT_TO_0(cfo_tf_cnt_cur,cfo_trk->cfo_tf_cnt_pre);

	if (cfo_tf_cnt > cfo_trk->cfo_tf_cnt_th)
		is_ul_ofdma = true;

	// Check TP, switch compensation period
	switch (cfo_trk->bb_cfo_trk_state) {
	case CFO_STATE_0:
		if (link->total_tp >= CFO_TP_UPPER) {
			cfo_trk->bb_cfo_trk_state = CFO_STATE_1;
			cfo_trk->cfo_trig_by_timer_en = true;
			// cfo_trk speed up
			//cfo_trk->cfo_timer_i.cb_time = CFO_COMP_PERIOD;
			halbb_cfo_acc_io_en(bb);
		}
		break;

	case CFO_STATE_1:
		if (cfo_trk->bb_cfo_trk_cnt >= cfo_trk->cfo_period_cnt) {
			if (cfo_trk->bb_cfo_trk_acc_mode==CFO_ACC_MODE_0) {
				cfo_trk->cfo_trig_by_timer_en = false;
				cfo_trk->bb_cfo_trk_state = CFO_STATE_2;
			}
			else if (!is_ul_ofdma) {
				cfo_trk->cfo_trig_by_timer_en = false;
				cfo_trk->bb_cfo_trk_state = CFO_STATE_2;
			}
		}
		else
			cfo_trk->bb_cfo_trk_cnt++;

		if (link->total_tp <= CFO_TP_LOWER) {
			cfo_trk->bb_cfo_trk_state = CFO_STATE_0;
			cfo_trk->bb_cfo_trk_cnt = 0;
			cfo_trk->cfo_trig_by_timer_en = false;
		}
		break;

	case CFO_STATE_2:
		if ((cfo_trk->bb_cfo_trk_acc_mode==CFO_ACC_MODE_1) && (is_ul_ofdma)) {
			cfo_trk->bb_cfo_trk_state = CFO_STATE_1;
			cfo_trk->cfo_trig_by_timer_en = true;
			halbb_cfo_acc_io_en(bb);
		}

		if (link->total_tp <= CFO_TP_LOWER) {
			cfo_trk->bb_cfo_trk_state = CFO_STATE_0;
			cfo_trk->bb_cfo_trk_cnt = 0;
			cfo_trk->cfo_trig_by_timer_en = false;
		}
		break;

	default:
		cfo_trk->bb_cfo_trk_state = CFO_STATE_0;
		cfo_trk->bb_cfo_trk_cnt = 0;
		break;
	}
	BB_DBG(bb, DBG_CFO_TRK, "[CFO_COMP] WD, total_tp = %d, cfo_trk_state = %d, timer_en = %d, trk_cnt = %d, is_ul_ofdma = %d\n",
		link->total_tp, cfo_trk->bb_cfo_trk_state, cfo_trk->cfo_trig_by_timer_en, cfo_trk->bb_cfo_trk_cnt, is_ul_ofdma);
	BB_DBG(bb, DBG_CFO_TRK, "[CFO_COMP] WD, cfo_trk_acc_mode = %d, cfo_tf_cnt_cur = %d, cfo_tf_cnt_pre = %d\n",
		cfo_trk->bb_cfo_trk_acc_mode, cfo_tf_cnt_cur, cfo_trk->cfo_tf_cnt_pre);
	BB_DBG(bb, DBG_CFO_TRK, "[CFO_COMP] WD, cfo_tf_cnt = %d, cfo_tf_cnt_th = %d \n",
		cfo_tf_cnt, cfo_trk->cfo_tf_cnt_th);

	cfo_trk->cfo_tf_cnt_pre = cfo_tf_cnt_cur;
	return cfo_trk->cfo_trig_by_timer_en;
}

void halbb_cfo_dm(struct bb_info *bb)
{
	struct bb_cfo_trk_info *cfo_trk = &bb->bb_cfo_trk_i;
	struct bb_cfo_diver_info *cfo_div = &cfo_trk->bb_cfo_div_i;
	struct bb_link_info *bb_link = &bb->bb_link_i;
	struct rtw_phl_com_t *phl = bb->phl_com;
	struct dev_cap_t *dev = &phl->dev_cap;
	bool x_cap_update = false;
	u8 pre_x_cap = cfo_trk->crystal_cap;
	s32 new_cfo = 0;
	u32 time_tmp;

	BB_DBG(bb, DBG_CFO_TRK, "[%s]\n", __func__);

#if 0
	rtw_hal_rf_xtal_tracking_offset(bb->hal_com, &xtal_th_ofst);
	rtw_hal_rf_get_thermal(bb->hal_com, 0, &thermal);

	BB_DBG(bb, DBG_CFO_TRK, "xtal_th_ofst=%d, thermal=%d\n", xtal_th_ofst, thermal);
#endif

	/* No new packet */
	if (cfo_trk->cfo_pkt_cnt == 0) {
		cfo_trk->no_pkt_cnt++;
		BB_DBG(bb, DBG_CFO_TRK, "Pkt cnt doesn't change\n");
		
		if (cfo_trk->no_pkt_cnt >= NO_PKT_RETURN_TH) {
			BB_DBG(bb, DBG_CFO_TRK, "bb_cfo_trk->no_pkt_cnt=%d\n",
			       cfo_trk->no_pkt_cnt);

			halbb_cfo_trk_reset(bb); /*xcap to default value */
		}
		return;
	}

	cfo_trk->no_pkt_cnt = 0;

	/* Xcap tend to diverge to extreme value */

#ifdef HALBB_DBCC_SUPPORT
if (!bb->hal_com->dbcc_en) //WA for DBCC test
#endif
{
	if (cfo_div->divergence_lock_en) {
		BB_DBG(bb, DBG_CFO_TRK,
		       "[Divergence lock!] limit_time=%d, phydm_sys_up_time=%d\n",
		       cfo_div->limit_time, bb->bb_sys_up_time);

		time_tmp = cfo_div->limit_time + CFO_LIMIT_PERIOD;

		if (time_tmp < bb->bb_sys_up_time) {
			cfo_div->divergence_lock_en = false;
		} else {
			halbb_cfo_trk_reset(bb); /*xcap to default value */
		}

		return;
	}

	BB_DBG(bb, DBG_CFO_TRK, "crystal_cap=%d; x_cap_ub=%d, x_cap_lb=%d\n",
	       cfo_trk->crystal_cap, cfo_trk->x_cap_ub, cfo_trk->x_cap_lb);

	if ((cfo_trk->crystal_cap >= cfo_trk->x_cap_ub) ||
				(cfo_trk->crystal_cap <= cfo_trk->x_cap_lb)) {
		BB_DBG(bb, DBG_CFO_TRK, "Xcap diverge to extreme value\n");
		cfo_div->divergence_lock_en = true;
		cfo_div->limit_time = bb->bb_sys_up_time;
		BB_DBG(bb, DBG_CFO_TRK, "[CFO][Div_force] limit_time=%d\n",
			   cfo_div->limit_time);
		halbb_cfo_trk_reset(bb);
		return;
	}
}

	if (bb_link->is_linked) {
		if (bb_link->is_one_entry_only) {
			new_cfo = halbb_avg_cfo_calc(bb);
		} else {
			/* Multi-sta cfo tracking -> calc. centroid cfo first*/
			new_cfo = halbb_multi_sta_avg_cfo_calc(bb);
		}
	}

	halbb_cfo_trk(bb, new_cfo);

	cfo_trk->cfo_avg_pre = new_cfo;

	halbb_print_sign_frac_digit(bb, cfo_trk->cfo_avg_pre, 32, 2, bb->dbg_buf, HALBB_SNPRINT_SIZE);
	BB_DBG(bb, DBG_CFO_TRK, "cfo_avg=(%s) Khz\n", bb->dbg_buf);

	BB_DBG(bb, DBG_CFO_TRK, "X_cap {Default:0x%x} {Curr: 0x%x -> 0x%x}, x_cap_ofst=%d step\n",
	       cfo_trk->def_x_cap, pre_x_cap, cfo_trk->crystal_cap,
	       cfo_trk->x_cap_ofst);

	x_cap_update =  (cfo_trk->crystal_cap == pre_x_cap) ? false : true;

	BB_DBG(bb, DBG_CFO_TRK, "Xcap_up=%d\n", x_cap_update);

	if (x_cap_update) {
		if (IS_GREATER(new_cfo, 0))
			new_cfo -= cfo_trk->sw_comp_fine_tune;
		else
			new_cfo += cfo_trk->sw_comp_fine_tune;
	}
	halbb_digital_cfo_comp(bb, new_cfo);

	halbb_cfo_counter_rst(bb);
}

void halbb_cfo_watchdog(struct bb_info *bb)
{
	BB_DBG(bb, DBG_CFO_TRK, "[%s] bb_phy_idx=%d\n", __func__, bb->bb_phy_idx);

#ifdef HALBB_DBCC_SUPPORT
	if (!bb->hal_com->dbcc_en)
#endif
	{
		if (halbb_cfo_acc_mode_en(bb))
			return;
	}

	BB_DBG(bb, DBG_CFO_TRK, "[%s]\n", __func__);
	halbb_cfo_dm(bb);
	BB_DBG(bb, DBG_CFO_TRK, "\n");
}

void halbb_parsing_cfo(struct bb_info *bb, u32 physts_bitmap,
		       struct physts_rxd *desc)
{
	struct bb_physts_info	*physts = &bb->bb_physts_i;
	struct bb_cfo_trk_info *bb_cfo_trk = &bb->bb_cfo_trk_i;
	struct bb_rate_info	*rate_info = &bb->bb_cmn_rpt_i.bb_rate_i;
	struct dev_cap_t *dev = &bb->phl_com->dev_cap;
	struct rtw_phl_stainfo_t *sta;
	struct rtw_cfo_info *cfo_t = NULL;
	s16 cfo;
	u8 fw_rate_idx = rate_info->fw_rate_idx;
	u8 bb_macid;

	if (!(physts_bitmap & BIT(IE01_CMN_OFDM) &&
	    physts->bb_physts_rslt_hdr_i.ie_map_type >= LEGACY_OFDM_PKT))
		return;

	if (bb_cfo_trk->cfo_trk_by_data_en) {
		if (!desc->user_i[0].is_data)
			return;
	}

	if (bb_cfo_trk->cfo_src == CFO_SRC_FD)
		cfo = physts->bb_physts_rslt_1_i.cfo_avg;
	else
		cfo = physts->bb_physts_rslt_1_i.cfo_pab_avg;

	if (desc->macid_su > PHL_MAX_STA_NUM)
		BB_WARNING("[%s] macid_su=%d\n", __func__, desc->macid_su);

	bb_macid = *(bb->phl2bb_macid_table + desc->macid_su);

	if (bb_macid > PHL_MAX_STA_NUM)
		BB_WARNING("[%s] bb_macid=%d\n", __func__, bb_macid);

	sta = *(bb->phl_sta_info + bb_macid);

	if (!is_sta_active(sta))
		return;

	if (sta->macid > PHL_MAX_STA_NUM)
		return;

	if (!sta->hal_sta)
		return;

	cfo_t = &sta->hal_sta->cfo_stat;

	if ((dev->rfe_type >= 50) && (bb_macid == 0)) /* No need to cnt AP Rx boardcast pkt*/
		return;

	/*  CFO info. of all path from phy-status have been averaged */
	cfo_t->cfo_tail += cfo;
	cfo_t->cfo_cnt++;

	bb_cfo_trk->cfo_pkt_cnt++;

	/*BB_DBG(bb, DBG_CFO_TRK, "cfo_cnt[%d]=%d, all_cfo_cnt=%d\n", desc->macid_su, cfo_t->cfo_cnt, bb_cfo_trk->cfo_pkt_cnt);*/

	/* Calcute throughput from rx rate idx*/
	if (rate_info->mode == BB_HE_MODE) {
		/* HE[3.2] = VHT[LGI]x1.25*/
		cfo_t->tp +=
		((bb_phy_rate_table[fw_rate_idx - MAX_RATE_VHT - MAX_RATE_HT] << 2)
		+ bb_phy_rate_table[fw_rate_idx - MAX_RATE_VHT - MAX_RATE_HT]) >> 2;
	} else if (rate_info->mode == BB_VHT_MODE) {
		cfo_t->tp += bb_phy_rate_table[fw_rate_idx - MAX_RATE_HT];
	} else {
		cfo_t->tp +=  bb_phy_rate_table[fw_rate_idx];
	}
}


void halbb_cfo_trk_dbg(struct bb_info *bb, char input[][16], u32 *_used,
			      char *output, u32 *_out_len)
{
	struct bb_cfo_trk_info *bb_cfo_trk = &bb->bb_cfo_trk_i;
	u32 var[10] = {0};
	bool	timer_en_pre;

	if (_os_strcmp(input[1], "-h") == 0) {
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "src {0:fd, 1:preamble}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "data_only {en}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "dyn_acc {en}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "set Xcap: {1} {val}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "show Xcap: {100}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "th {en, stop, 0~3} {s(8,2)}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "tb_comp {s(8,2)}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "sw_comp {Xcap_enable_th (kHz)} {Xcap_stop_th (kHz)} {sw_comp_fine_tune (kHz)}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "acc {en} {ms} {period_cnt}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "acc_mode {0: disable_ul_ofmda_acc, 1: enable_ul_ofdma_acc} {tf_cnt_th}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "set_mode {multi-sta cfo_trk mode - 0:Pkts averaged mode, 1: Entry averaged mode, 2: TP based mode}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "cfo_tol {manually adjust hypothetical sta_cfo_tolerance in decimal kHz}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "cfo_step {step[0]} {step[1]} {step[2]} {step[3]}\n");
		return;
	}

	HALBB_SCAN(input[1], DCMD_DECIMAL, &var[0]);
	if (var[0] == 1) {
		HALBB_SCAN(input[2], DCMD_HEX, &var[1]);
		halbb_set_crystal_cap(bb, (u8)var[1]);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "Set X_cap=0x%x\n", bb_cfo_trk->crystal_cap);
	} else if (var[0] == 100) {
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "X_cap=0x%x\n", bb_cfo_trk->crystal_cap);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "Xcap_enable_th = %d (kHz)\n", bb_cfo_trk->cfo_th_en >> 2);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "Xcap_stop_th = %d (kHz)\n", bb_cfo_trk->cfo_th_stop >> 2);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "sw_comp_fine_tune = %d (kHz)\n", bb_cfo_trk->sw_comp_fine_tune >> 2);

	} else if (_os_strcmp(input[1], "data_only") == 0) {
		HALBB_SCAN(input[2], DCMD_DECIMAL, &var[0]);
		
		bb_cfo_trk->cfo_trk_by_data_en = (bool)var[0];
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "cfo_trk_by_data_en: %d\n", bb_cfo_trk->cfo_trk_by_data_en);
	} else if (_os_strcmp(input[1], "dyn_acc") == 0) {
		HALBB_SCAN(input[2], DCMD_DECIMAL, &var[0]);
		
		bb_cfo_trk->cfo_dyn_acc_en = (bool)var[0];
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "cfo_dyn_acc_en: %d\n", bb_cfo_trk->cfo_dyn_acc_en);
	} else if (_os_strcmp(input[1], "src") == 0) {
		HALBB_SCAN(input[2], DCMD_DECIMAL, &var[0]);

		bb_cfo_trk->cfo_src = (enum bb_cfo_trk_src_t)var[0];
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "cfo_src: %s\n",
			    ((bb_cfo_trk->cfo_src == CFO_SRC_FD) ? "FD" : "PAB"));
	} else if (_os_strcmp(input[1], "acc") == 0) {
		HALBB_SCAN(input[2], DCMD_DECIMAL, &var[0]);
		HALBB_SCAN(input[3], DCMD_DECIMAL, &var[1]);
		HALBB_SCAN(input[4], DCMD_DECIMAL, &var[2]);

		timer_en_pre = bb_cfo_trk->cfo_trig_by_timer_en;
		bb_cfo_trk->cfo_trig_by_timer_en = (bool)var[0];

		if (var[1] > 2000)
			bb_cfo_trk->cfo_timer_i.cb_time = 2000;
		else if (var[1] < 5)
			bb_cfo_trk->cfo_timer_i.cb_time = 5;
		else
			bb_cfo_trk->cfo_timer_i.cb_time = (u16)var[1];

		bb_cfo_trk->cfo_period_cnt = (bool)var[2];
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "timer=%d ms, period_cnt=%d cnt, trig_by_timer_en=%d\n",
			    bb_cfo_trk->cfo_timer_i.cb_time, bb_cfo_trk->cfo_period_cnt, bb_cfo_trk->cfo_trig_by_timer_en);

		if (!timer_en_pre && bb_cfo_trk->cfo_trig_by_timer_en) {
			BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "[Start callback]\n");
			halbb_cfo_acc_io_en(bb);
		}
	}else if (_os_strcmp(input[1], "acc_mode") == 0) {
		HALBB_SCAN(input[2], DCMD_DECIMAL, &var[0]);
		HALBB_SCAN(input[3], DCMD_DECIMAL, &var[1]);

		bb_cfo_trk->bb_cfo_trk_acc_mode= (enum bb_cfo_trk_acc_mode_t)var[0] ;
		bb_cfo_trk->cfo_tf_cnt_th= var[1] ;

		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "cfo_trk_acc_mode = %d, cfo_tf_cnt_th = %d\n",
			    bb_cfo_trk->bb_cfo_trk_acc_mode,bb_cfo_trk->cfo_tf_cnt_th);
	} else if (_os_strcmp(input[1], "th") == 0) {

		HALBB_SCAN(input[3], DCMD_DECIMAL, &var[1]);

		if (_os_strcmp(input[2], "en") == 0) {
			bb_cfo_trk->cfo_th_en = (u8)var[1];
		} else if (_os_strcmp(input[2], "stop") == 0) {
			bb_cfo_trk->cfo_th_stop = (u8)var[1];
		} else {
			HALBB_SCAN(input[2], DCMD_DECIMAL, &var[0]);
			if (var[0] < CFO_TRK_TH_SIZE)
				bb_cfo_trk->cfo_th[var[0]] = (u8)var[1];
			else
				BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
					    "Set Err\n");
		}
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "TH[en, stop]={%d, %d}, TH[3:0]={%d, %d, %d, %d}\n",
			    bb_cfo_trk->cfo_th_en, bb_cfo_trk->cfo_th_stop,
		            bb_cfo_trk->cfo_th[3], bb_cfo_trk->cfo_th[2],
		            bb_cfo_trk->cfo_th[1], bb_cfo_trk->cfo_th[0]);

	} else if (_os_strcmp(input[1], "tb_comp") == 0) {
		HALBB_SCAN(input[2], DCMD_DECIMAL, &var[0]);

		bb_cfo_trk->tb_tx_comp_cfo_th = (u8)var[0];

		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "tb_tx_comp_cfo_th = %d\n",
			    bb_cfo_trk->tb_tx_comp_cfo_th);
	} else if (_os_strcmp(input[1], "sw_comp") == 0) {
		HALBB_SCAN(input[2], DCMD_DECIMAL, &var[0]);
		HALBB_SCAN(input[3], DCMD_DECIMAL, &var[1]);
		HALBB_SCAN(input[4], DCMD_DECIMAL, &var[2]);

		bb_cfo_trk->cfo_th_en = (u8)var[0] << 2;
		bb_cfo_trk->cfo_th_stop = (u8)var[1] << 2;
		bb_cfo_trk->sw_comp_fine_tune = (u8)var[2] << 2;

		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "xcap_enable_th = %d,xcap_stop_th = %d,sw_comp_fine_tune = %d\n",
			    bb_cfo_trk->cfo_th_en >> 2,bb_cfo_trk->cfo_th_stop >> 2, bb_cfo_trk->sw_comp_fine_tune >> 2);
	} else if (_os_strcmp(input[1], "set_mode") == 0) {
		HALBB_SCAN(input[2], DCMD_DECIMAL, &var[0]);

		bb_cfo_trk->multi_sta_cfo_mode= (u8)var[0];

		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "multi_sta_cfo_mode = %d\n",
			    bb_cfo_trk->multi_sta_cfo_mode);
	} else if (_os_strcmp(input[1], "cfo_tol") == 0) {
		HALBB_SCAN(input[2], DCMD_DECIMAL, &var[0]);
		bb_cfo_trk->man_cfo_tol= true;
		bb_cfo_trk->sta_cfo_tolerance = (u8)var[0];

		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "manually adjust hypothetical sta_cfo_tolerance = %d\n",
			    bb_cfo_trk->sta_cfo_tolerance);
	} else if (_os_strcmp(input[1], "cfo_step") == 0) {
		HALBB_SCAN(input[2], DCMD_DECIMAL, &var[0]);
		HALBB_SCAN(input[3], DCMD_DECIMAL, &var[1]);
		HALBB_SCAN(input[4], DCMD_DECIMAL, &var[2]);
		HALBB_SCAN(input[5], DCMD_DECIMAL, &var[3]);

		bb_cfo_trk->step[0] = (u8)var[0];
		bb_cfo_trk->step[1] = (u8)var[1];
		bb_cfo_trk->step[2] = (u8)var[2];
		bb_cfo_trk->step[3] = (u8)var[3];

		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "cfo_step: step[0]=%d; step[1]=%d; step[2]=%d; step[3]=%d\n",
			    bb_cfo_trk->step[0], bb_cfo_trk->step[1],
			    bb_cfo_trk->step[2], bb_cfo_trk->step[3]);
	}
}

void halbb_cr_cfg_cfo_trk_init(struct bb_info *bb)
{
	struct bb_cfo_trk_cr_info *cr = &bb->bb_cfo_trk_i.bb_cfo_trk_cr_i;

	switch (bb->cr_type) {

	#ifdef HALBB_COMPILE_AP_SERIES
	case BB_AP:
		cr->r_cfo_comp_seg0_312p5khz = CFO_COMP_SEG0_312P5KHZ_0_A;
		cr->r_cfo_comp_seg0_312p5khz_m = CFO_COMP_SEG0_312P5KHZ_0_A_M;
		cr->r_cfo_comp_seg0_vld = CFO_COMP_SEG0_VLD_0_A;
		cr->r_cfo_comp_seg0_vld_m = CFO_COMP_SEG0_VLD_0_A_M;
		cr->r_cfo_wgting = CFO_WGTING_A;
		cr->r_cfo_wgting_m = CFO_WGTING_A_M;

		break;

	#endif
	#ifdef HALBB_COMPILE_CLIENT_SERIES
	case BB_CLIENT:
		#ifdef BB_8852B_SUPPORT
		if (bb->ic_type == BB_RTL8852B) {
			cr->r_cfo_comp_seg0_312p5khz = CFO_COMP_SEG0_312P5KHZ_0_C;
			cr->r_cfo_comp_seg0_312p5khz_m = CFO_COMP_SEG0_312P5KHZ_0_C_M;
			cr->r_cfo_comp_seg0_312p5khz_1 = CFO_COMP_SEG0_312P5KHZ_1_C;
			cr->r_cfo_comp_seg0_312p5khz_1_m = CFO_COMP_SEG0_312P5KHZ_1_C_M;
			cr->r_cfo_comp_seg0_312p5khz_2 = CFO_COMP_SEG0_312P5KHZ_2_C;
			cr->r_cfo_comp_seg0_312p5khz_2_m = CFO_COMP_SEG0_312P5KHZ_2_C_M;
			cr->r_cfo_comp_seg0_312p5khz_3 = CFO_COMP_SEG0_312P5KHZ_3_C;
			cr->r_cfo_comp_seg0_312p5khz_3_m = CFO_COMP_SEG0_312P5KHZ_3_C_M;
		}
		#endif
		#ifdef BB_8851B_SUPPORT
		if (bb->ic_type == BB_RTL8851B) {
			cr->r_cfo_comp_seg0_312p5khz = CFO_COMP_SEG0_312P5KHZ_0_C_51B_PATCH;
			cr->r_cfo_comp_seg0_312p5khz_m = CFO_COMP_SEG0_312P5KHZ_0_C_51B_PATCH_M;
			cr->r_cfo_comp_seg0_312p5khz_1 = CFO_COMP_SEG0_312P5KHZ_1_C_51B_PATCH;
			cr->r_cfo_comp_seg0_312p5khz_1_m = CFO_COMP_SEG0_312P5KHZ_1_C_51B_PATCH_M;
			cr->r_cfo_comp_seg0_312p5khz_2 = CFO_COMP_SEG0_312P5KHZ_2_C_51B_PATCH;
			cr->r_cfo_comp_seg0_312p5khz_2_m = CFO_COMP_SEG0_312P5KHZ_2_C_51B_PATCH_M;
			cr->r_cfo_comp_seg0_312p5khz_3 = CFO_COMP_SEG0_312P5KHZ_3_C_51B_PATCH;
			cr->r_cfo_comp_seg0_312p5khz_3_m = CFO_COMP_SEG0_312P5KHZ_3_C_51B_PATCH_M;
		}
		#endif
		cr->r_cfo_comp_seg0_vld = CFO_COMP_SEG0_VLD_0_C;
		cr->r_cfo_comp_seg0_vld_m = CFO_COMP_SEG0_VLD_0_C_M;
		cr->r_cfo_wgting = CFO_WGTING_C;
		cr->r_cfo_wgting_m = CFO_WGTING_C_M;

		break;
	#endif

	#ifdef HALBB_COMPILE_AP2_SERIES
	case BB_AP2:
		cr->r_cfo_comp_seg0_312p5khz = CFO_COMP_SEG0_312P5KHZ_0_A2;
		cr->r_cfo_comp_seg0_312p5khz_m = CFO_COMP_SEG0_312P5KHZ_0_A2_M;
		cr->r_cfo_comp_seg0_vld = CFO_COMP_SEG0_VLD_0_A2;
		cr->r_cfo_comp_seg0_vld_m = CFO_COMP_SEG0_VLD_0_A2_M;
		cr->r_cfo_wgting = CFO_WGTING_A2;
		cr->r_cfo_wgting_m = CFO_WGTING_A2_M;

		break;

	#endif

	#ifdef HALBB_COMPILE_BE0_SERIES
	case BB_BE0:
		cr->r_cfo_comp_seg0_312p5khz = CFO_COMP_SEG0_312P5KHZ_0_BE0;
		cr->r_cfo_comp_seg0_312p5khz_m = CFO_COMP_SEG0_312P5KHZ_0_BE0_M;
		cr->r_cfo_comp_seg0_vld = CFO_COMP_SEG0_VLD_0_BE0;
		cr->r_cfo_comp_seg0_vld_m = CFO_COMP_SEG0_VLD_0_BE0_M;
		cr->r_cfo_wgting = CFO_WGTING_BE0;
		cr->r_cfo_wgting_m = CFO_WGTING_BE0_M;

		break;

	#endif
	
	default:
		BB_WARNING("[%s] BBCR Hook FAIL!\n", __func__);
		if (bb->bb_dbg_i.cr_fake_init_hook_en) {
			BB_TRACE("[%s] BBCR fake init\n", __func__);
			halbb_cr_hook_fake_init(bb, (u32 *)cr, (sizeof(struct bb_cfo_trk_cr_info) >> 2));
		}
		break;
	}

	if (bb->bb_dbg_i.cr_init_hook_recorder_en) {
		BB_TRACE("[%s] BBCR Hook dump\n", __func__);
		halbb_cr_hook_init_dump(bb, (u32 *)cr, (sizeof(struct bb_cfo_trk_cr_info) >> 2));
	}
}

#endif

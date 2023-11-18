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

#ifdef HALBB_ANT_DIV_SUPPORT

void halbb_antdiv_reset_training_stat(struct bb_info *bb)
{
	struct bb_antdiv_info *bb_ant_div = &bb->bb_ant_div_i;
	struct bb_antdiv_rate_info *bb_rate_i = &bb_ant_div->bb_rate_i;
	struct bb_antdiv_evm_info *bb_evm_i = &bb_ant_div->bb_evm_i;
	struct bb_antdiv_rssi_info *bb_rssi_i = &bb_ant_div->bb_rssi_i;
	struct bb_antdiv_cn_info *bb_cn_i = &bb_ant_div->bb_cn_i;

	halbb_mem_set(bb, bb_rate_i, 0, sizeof(struct bb_antdiv_rate_info));
	halbb_mem_set(bb, bb_cn_i, 0, sizeof(struct bb_antdiv_cn_info));
	halbb_mem_set(bb, bb_evm_i, 0, sizeof(struct bb_antdiv_evm_info));
	halbb_mem_set(bb, bb_rssi_i, 0, sizeof(struct bb_antdiv_rssi_info));

}

void halbb_antdiv_reset(struct bb_info *bb)
{
	struct bb_antdiv_info *bb_ant_div = &bb->bb_ant_div_i;
	struct bb_antdiv_rate_info *bb_rate_i = &bb_ant_div->bb_rate_i;
	struct bb_antdiv_evm_info *bb_evm_i = &bb_ant_div->bb_evm_i;

	/* Reset stat */
	halbb_antdiv_reset_training_stat(bb);
}

void halbb_antdiv_reg_init(struct bb_info *bb)
{
	struct bb_antdiv_info *bb_ant_div = &bb->bb_ant_div_i;
	struct bb_link_info *bb_link = &bb->bb_link_i;
	struct bb_antdiv_cr_info *cr = &bb->bb_ant_div_i.bb_antdiv_cr_i;

	/* dis r_ant_train_en */
	halbb_set_reg_cmn(bb, cr->path0_r_ant_train_en, cr->path0_r_ant_train_en_m, 0x0, HW_PHY_0);

	/* force r_tx_ant_sel instead of from FW CMAC table */
	halbb_set_reg_cmn(bb, cr->path0_r_tx_ant_sel, cr->path0_r_tx_ant_sel_m, 0x0, HW_PHY_0);

	/* r_trsw_tx_extend = 0us */
	halbb_set_reg_cmn(bb, 0x728, 0xf, 0x0, HW_PHY_0);
	/* dis r_hw_antsw_dis_by_gnt_bt */
	halbb_set_reg_cmn(bb, 0x728, BIT(12), 0x0, HW_PHY_0);


	/* dis r_bt_force_en */
	halbb_set_reg_cmn(bb, cr->path0_r_bt_force_antidx_en, cr->path0_r_bt_force_antidx_en_m, 0x0, HW_PHY_0);

	/* r_rfsw_ctrl_antenna (Antenna mapping) */
	halbb_set_reg_cmn(bb, cr->path0_r_rfsw_ant_31_0, 0xFFFF, 0x0100, HW_PHY_0);

	/* dis r_BB_SEL_BTG_TRX_S */
	halbb_set_reg_cmn(bb, cr->path0_r_antsel, BIT(21), 0x1, HW_PHY_0);

	/* "antsel" is controlled by HWs*/
	halbb_set_reg_cmn(bb, cr->path0_r_antsel, BIT(16), 0x1, HW_PHY_0);
	/* r_ANT_DIV_SW_2G_S, 2G "CS/CG switching" is controlled by HWs */
	halbb_set_reg_cmn(bb, cr->path0_r_antsel, BIT(23), 0x0, HW_PHY_0);
	/* r_ANT_DIV_SW_5G_S, 5G "CS/CG switching" is controlled by HWs */
	halbb_set_reg_cmn(bb, cr->path0_r_antsel, BIT(25), 0x0, HW_PHY_0);
	if (bb->ic_type == BB_RTL8192XB && bb->bb_api_i.band == BAND_ON_5G) {
		/* 32BR one pin control switch */
		halbb_gpio_setting(bb, 0, BB_PATH_A, false, ANTSEL_0);
	}
	else if (bb->ic_type == BB_RTL8851B) {
		/*RFE control, GPIO set by halrf*/
		/*51B two pin control swtich*/ /*set BB RFE control pin*/
		//halbb_gpio_setting(bb, 16, BB_PATH_A, true, ANTSEL_0);
		//halbb_gpio_setting(bb, 17, BB_PATH_A, false, ANTSEL_0);
		/*Set MAC GPIO*/
		//rtw_hal_mac_set_gpio_func(bb->hal_com, RTW_MAC_GPIO_WL_RFE_CTRL, 16);
		//rtw_hal_mac_set_gpio_func(bb->hal_com, RTW_MAC_GPIO_WL_RFE_CTRL, 17);
	}
}

void halbb_antdiv_init(struct bb_info *bb)
{
	struct bb_antdiv_info *bb_ant_div = &bb->bb_ant_div_i;
	struct bb_link_info *bb_link = &bb->bb_link_i;
	struct rtw_phl_com_t *phl = bb->phl_com;
	struct dev_cap_t *dev = &phl->dev_cap;

	BB_DBG(bb, DBG_ANT_DIV, "%s ======>\n", __func__);

	if ((bb->support_ability & BB_ANT_DIV) ||(phl_is_mp_mode(bb->phl_com) &&
		((bb->ic_type == BB_RTL8851B) && ((dev->rfe_type % 3) == 2)))) {
		/* HW reg. init to set mux & ctrler for antdiv */
		halbb_antdiv_reg_init(bb);
	} else {
		BB_DBG(bb, DBG_ANT_DIV, "Early return - Not support antenna diversity\n");
		return;
	}

	/* Mode setting*/
	bb_ant_div->antdiv_mode = AUTO_ANT;
	bb_ant_div->pre_antdiv_mode = AUTO_ANT;
	bb_ant_div->antdiv_method = EVM_BASED_ANTDIV;
	bb_ant_div->evm_decision_method = EVM_LINEAR_AVG;

	/* Algorithm parameter setting */
	bb_ant_div->tp_diff_th_high = ANTDIV_DEC_TP_HIGH;
	bb_ant_div->tp_diff_th_low = ANTDIV_DEC_TP_LOW;
	bb_ant_div->rssi_diff_th = ANTDIV_DEC_RSSI;
	bb_ant_div->fixant_rssi_diff = ANTDIV_FIXANT_RSSI;
	bb_ant_div->tp_lb = TP_LOWER_BOUND;

	if (bb->ic_type & BB_IC_1SS) {
		bb_ant_div->antdiv_period = ANTDIV_PERIOD_1SS;
		bb_ant_div->antdiv_train_num = ANTDIV_TRAINING_NUM_1SS;
		bb_ant_div->antdiv_delay = ANTDIV_DELAY_1SS;
		bb_ant_div->antdiv_intvl = ANTDIV_INTVL_1SS;
		bb_ant_div->tp_decision_method = TP_MAX_DOMINATION;
		bb_ant_div->evm_diff_th = ANTDIV_DEC_EVM_1SS;
	}
	else {
		bb_ant_div->antdiv_period = ANTDIV_PERIOD;
		bb_ant_div->antdiv_train_num = ANTDIV_TRAINING_NUM;
		bb_ant_div->antdiv_delay = ANTDIV_DELAY;
		bb_ant_div->antdiv_intvl = ANTDIV_INTVL;
		bb_ant_div->tp_decision_method = TP_HIGHEST_DOMINATION;
		bb_ant_div->evm_diff_th = ANTDIV_DEC_EVM;
	}

	/* variable init */
	bb_ant_div->antdiv_wd_cnt = 0;
	bb_ant_div->antdiv_training_state_cnt = 0;
	bb_ant_div->get_stats = false;
	bb_ant_div->antdiv_use_ctrl_frame = true;
	bb_ant_div->antdiv_notrain_num = ANTDIV_NOTRAINING_NUM;
	bb_ant_div->antdiv_notrain_cnt = bb_ant_div->antdiv_notrain_num;

	bb_ant_div->target_ant = ANTDIV_INIT;
	bb_ant_div->target_ant_evm = ANTDIV_INIT;
	bb_ant_div->target_ant_tp = ANTDIV_INIT;
	bb_ant_div->target_ant_rssi =  ANTDIV_INIT;
	bb_ant_div->training_ant = ANTDIV_INIT;
	bb_ant_div->pre_target_ant = MAIN_ANT;

	halbb_antdiv_reset(bb);

	BB_DBG(bb, DBG_INIT, "Init ant_diversity timer");
}

void halbb_antdiv_set_ant(struct bb_info *bb, u8 ant)
{
	struct bb_antdiv_info *bb_ant_div = &bb->bb_ant_div_i;
	struct bb_link_info *bb_link = &bb->bb_link_i;
	struct bb_antdiv_cr_info *cr = &bb->bb_ant_div_i.bb_antdiv_cr_i;
	struct rtw_hal_com_t *hal = bb->hal_com;
	u8 band = bb->hal_com->band[0].cur_chandef.band;
	u8 default_ant, optional_ant;

	if ((bb_ant_div->pre_target_ant != ant) || (bb_ant_div->training_ant != ant) ) {
		BB_DBG(bb, DBG_ANT_DIV, "Set Antenna =%s\n",
			  (ant == MAIN_ANT) ? "MAIN_ANT" : "AUX_ANT");
		if(ant == MAIN_ANT) {
			default_ant = ANT1_2G;
			optional_ant = ANT2_2G;
		} else {
			default_ant = ANT2_2G;
			optional_ant = ANT1_2G;
		}

		/* Original Rx antenna */
		//halbb_set_reg_cmn(bb, cr->path0_r_antsel, 0x20000, default_ant, HW_PHY_0);
		halbb_set_reg_cmn(bb, cr->path0_r_antsel, 0xf0, default_ant, HW_PHY_0);
		/* Alternative Rx antenna */
		halbb_set_reg_cmn(bb, cr->path0_r_antsel, 0xf00, optional_ant, HW_PHY_0);
		/* Tx antenna, same as orig. rx ant. */
		halbb_set_reg_cmn(bb, cr->path0_r_antsel, 0xf000, default_ant, HW_PHY_0);
		
		/*#ifdef BB_8852A_2_SUPPORT
			if (bb->ic_type == BB_RTL8852A)
				rtw_hal_rf_rx_ant(hal, ant);
		#endif*/
	} else {
		BB_DBG(bb, DBG_ANT_DIV, "Stay in Ori-ant\n");
	}
}

void halbb_antdiv_get_rssi(struct bb_info *bb)
{
	struct bb_cmn_rpt_info	*cmn_rpt = &bb->bb_cmn_rpt_i;
	struct bb_pkt_cnt_su_info *pkt_cnt = &cmn_rpt->bb_pkt_cnt_su_i;
	struct bb_antdiv_info *bb_ant_div = &bb->bb_ant_div_i;
	struct bb_antdiv_rssi_info *rssi = &bb_ant_div->bb_rssi_i;
	struct bb_rate_info *rate_i = &cmn_rpt->bb_rate_i;

	rssi->rssi_cck_avg = (u8)HALBB_DIV(rssi->rssi_cck_avg_acc, rssi->pkt_cnt_cck);
	rssi->rssi_ofdm_avg = (u8)HALBB_DIV(rssi->rssi_ofdm_avg_acc, rssi->pkt_cnt_ofdm);
	rssi->rssi_t_avg = (u8)HALBB_DIV(rssi->rssi_t_avg_acc, rssi->pkt_cnt_t);

	rssi->main_rssi_cck_avg = (u8)HALBB_DIV(rssi->main_rssi_cck_avg_acc, rssi->main_pkt_cnt_cck);
	rssi->main_rssi_ofdm_avg = (u8)HALBB_DIV(rssi->main_rssi_ofdm_avg_acc, rssi->main_pkt_cnt_ofdm);
	rssi->main_rssi_t_avg = (u8)HALBB_DIV(rssi->main_rssi_t_avg_acc, rssi->main_pkt_cnt_t);

	rssi->aux_rssi_cck_avg = (u8)HALBB_DIV(rssi->aux_rssi_cck_avg_acc, rssi->aux_pkt_cnt_cck);
	rssi->aux_rssi_ofdm_avg = (u8)HALBB_DIV(rssi->aux_rssi_ofdm_avg_acc, rssi->aux_pkt_cnt_ofdm);
	rssi->aux_rssi_t_avg = (u8)HALBB_DIV(rssi->aux_rssi_t_avg_acc, rssi->aux_pkt_cnt_t);

	if ((rssi->pkt_cnt_t >= rssi->pkt_cnt_cck) && (rssi->pkt_cnt_t >= rssi->pkt_cnt_ofdm))
		rssi->rssi_final = rssi->rssi_t_avg;
	else if ((rssi->pkt_cnt_ofdm >= rssi->pkt_cnt_cck) && (rssi->pkt_cnt_ofdm >= rssi->pkt_cnt_t))
		rssi->rssi_final = rssi->rssi_ofdm_avg;
	else
		rssi->rssi_final = rssi->rssi_cck_avg;

	if (rssi->main_pkt_cnt_t || rssi->aux_pkt_cnt_t) {
		rssi->main_rssi_final =  rssi->main_rssi_t_avg;
		rssi->aux_rssi_final = rssi->aux_rssi_t_avg;
	} else if (rssi->main_pkt_cnt_ofdm ||rssi->aux_pkt_cnt_ofdm ) {
		rssi->main_rssi_final = rssi->main_rssi_ofdm_avg;
		rssi->aux_rssi_final = rssi->aux_rssi_ofdm_avg;
	} else {
		rssi->main_rssi_final =rssi->main_rssi_cck_avg;
		rssi->aux_rssi_final = rssi->aux_rssi_cck_avg;
	}

/*
	if ((rssi->main_pkt_cnt_t >= rssi->main_pkt_cnt_cck) && (rssi->main_pkt_cnt_t >= rssi->main_pkt_cnt_ofdm))
		rssi->main_rssi_final = rssi->main_rssi_t_avg;
	else if ((rssi->main_pkt_cnt_ofdm >= rssi->main_pkt_cnt_cck) && (rssi->main_pkt_cnt_ofdm >= rssi->main_pkt_cnt_t))
		rssi->main_rssi_final = rssi->main_rssi_ofdm_avg;
	else
		rssi->main_rssi_final = rssi->main_rssi_cck_avg;

	if ((rssi->aux_pkt_cnt_t >= rssi->aux_pkt_cnt_cck) && (rssi->aux_pkt_cnt_t >= rssi->aux_pkt_cnt_ofdm))
		rssi->aux_rssi_final = rssi->aux_rssi_t_avg;
	else if ((rssi->aux_pkt_cnt_ofdm >= rssi->aux_pkt_cnt_cck) && (rssi->aux_pkt_cnt_ofdm >= rssi->aux_pkt_cnt_t))
		rssi->aux_rssi_final = rssi->aux_rssi_ofdm_avg;
	else
		rssi->aux_rssi_final = rssi->aux_rssi_cck_avg; */
}

void halbb_antdiv_get_rssi_target_ant(struct bb_info *bb)
{
	struct bb_antdiv_info *bb_ant_div = &bb->bb_ant_div_i;
	struct bb_antdiv_rssi_info *rssi = &bb_ant_div->bb_rssi_i;
	u8 main_rssi_final, aux_rssi_final, target_ant_rssi;
	bool no_change_flag = false;

	halbb_antdiv_get_rssi(bb);

	main_rssi_final = rssi->main_rssi_final >> 1;
	aux_rssi_final = rssi->aux_rssi_final >> 1;
	rssi->rssi_diff = DIFF_2(main_rssi_final , aux_rssi_final);

	/*prevent pkt_cnt=0 causing rssi=0*/
	/*if ((rssi->main_pkt_cnt_cck == 0 && rssi->main_pkt_cnt_ofdm == 0 && rssi->main_pkt_cnt_t == 0) ||
		(rssi->aux_pkt_cnt_cck == 0 && rssi->aux_pkt_cnt_ofdm == 0 && rssi->aux_pkt_cnt_t == 0)) {
		target_ant_rssi = bb_ant_div->pre_target_ant;
		no_change_flag= true;
	}*/
	if (main_rssi_final == aux_rssi_final) {
		target_ant_rssi = bb_ant_div->pre_target_ant;
		no_change_flag= true;
	}
	else if (main_rssi_final == 0 || aux_rssi_final == 0) {
		target_ant_rssi = bb_ant_div->pre_target_ant;
		//no_change_flag= true;
	}
	else if (rssi->rssi_diff < bb_ant_div->rssi_diff_th) {
		target_ant_rssi = bb_ant_div->pre_target_ant;
		no_change_flag= true;
	}
	else if (main_rssi_final > aux_rssi_final)
		target_ant_rssi = MAIN_ANT;
	else
		target_ant_rssi = AUX_ANT;

	rssi->no_change_flag = no_change_flag;
	bb_ant_div->target_ant_rssi = target_ant_rssi;

	BB_DBG(bb, DBG_ANT_DIV, "Main RSSI = [%d], AUX RSSI = [%d]\n", main_rssi_final, aux_rssi_final);
	BB_DBG(bb, DBG_ANT_DIV, "Main[cck_cnt, ofdm_cnt, t_cnt] = [%d, %d, %d]\n", rssi->main_pkt_cnt_cck, rssi->main_pkt_cnt_ofdm, rssi->main_pkt_cnt_t);
	BB_DBG(bb, DBG_ANT_DIV, "AUX[cck_cnt, ofdm_cnt, t_cnt] = [%d, %d, %d]\n", rssi->aux_pkt_cnt_cck, rssi->aux_pkt_cnt_ofdm, rssi->aux_pkt_cnt_t);
	BB_DBG(bb, DBG_ANT_DIV, "RSSI DIFF = [%d], TH = [%d]\n", rssi->rssi_diff, bb_ant_div->rssi_diff_th);
	BB_DBG(bb, DBG_ANT_DIV, "RSSI based TargetAnt= [%s]\n", 
		(bb_ant_div->target_ant_rssi == MAIN_ANT) ? "MAIN_ANT" : "AUX_ANT");
}

void halbb_antdiv_get_cn_target_ant(struct bb_info *bb)
{
	struct bb_antdiv_info *bb_ant_div = &bb->bb_ant_div_i;
	struct bb_link_info *bb_link = &bb->bb_link_i;
	struct bb_cmn_rpt_info	*cmn_rpt = &bb->bb_cmn_rpt_i;
	struct bb_rate_info *rate_i = &cmn_rpt->bb_rate_i;
	struct bb_antdiv_cn_info *bb_cn_i = &bb_ant_div->bb_cn_i;
	struct bb_antdiv_rate_info *bb_rate_i = &bb_ant_div->bb_rate_i;
	u32 main_cn, aux_cn;
	u8 target_ant_cn;

	/* CN */
	main_cn =  (u8)HALBB_DIV(bb_cn_i->main_cn_avg_acc, (bb_rate_i->main_pkt_cnt_t + bb_rate_i->main_pkt_cnt_ofdm));
	aux_cn =  (u8)HALBB_DIV(bb_cn_i->aux_cn_avg_acc, (bb_rate_i->aux_pkt_cnt_t+ bb_rate_i->aux_pkt_cnt_ofdm));

	if (aux_cn == 0)
		target_ant_cn = MAIN_ANT;
	else if (main_cn == 0)
		target_ant_cn = AUX_ANT;
	else
		target_ant_cn = (main_cn == aux_cn) ? (bb_ant_div->pre_target_ant) : ((main_cn >= aux_cn) ? AUX_ANT : MAIN_ANT);

	BB_DBG(bb, DBG_ANT_DIV, "%-9s (%02d.%03d)\n", "[Main-Ant CN_avg]",
	       (main_cn >> 1),
	       halbb_show_fraction_num(main_cn & 0x1, 1));

	BB_DBG(bb, DBG_ANT_DIV, "%-9s (%02d.%03d)\n", "[Aux-Ant CN_avg]",
	       (aux_cn >> 1),
	       halbb_show_fraction_num(aux_cn & 0x1, 1));

	bb_ant_div->target_ant_cn = target_ant_cn;
	BB_DBG(bb, DBG_ANT_DIV, "CN based TargetAnt= [%s]\n", (bb_ant_div->target_ant_cn == MAIN_ANT) ? "MAIN_ANT" : "AUX_ANT");
}


void halbb_antdiv_get_highest_mcs(struct bb_info *bb)
{
	struct bb_antdiv_info *bb_ant_div = &bb->bb_ant_div_i;
	struct bb_link_info *bb_link = &bb->bb_link_i;
	struct bb_cmn_rpt_info	*cmn_rpt = &bb->bb_cmn_rpt_i;
	struct bb_rate_info *rate_i = &cmn_rpt->bb_rate_i;
	struct bb_antdiv_rate_info *bb_rate_i = &bb_ant_div->bb_rate_i;

	u16 main_max_cnt = 1;
	u16 aux_max_cnt = 1;
	u16 main_max_idx = 0;
	u16 aux_max_idx = 0;
	u8 ss_ofst = 0;
	u8 i;

	BB_DBG(bb, DBG_ANT_DIV, "*Main-Ant CCK    cnt:{%d, %d, %d, %d}\n",
	       bb_rate_i->main_pkt_cnt_legacy[0], bb_rate_i->main_pkt_cnt_legacy[1],
	       bb_rate_i->main_pkt_cnt_legacy[2], bb_rate_i->main_pkt_cnt_legacy[3]);

	BB_DBG(bb, DBG_ANT_DIV, "*Aux-Ant CCK    cnt:{%d, %d, %d, %d}\n",
	       bb_rate_i->aux_pkt_cnt_legacy[0], bb_rate_i->aux_pkt_cnt_legacy[1],
	       bb_rate_i->aux_pkt_cnt_legacy[2], bb_rate_i->aux_pkt_cnt_legacy[3]);

	BB_DBG(bb, DBG_ANT_DIV, "*Main-Ant OFDM    cnt:{%d, %d, %d, %d, %d, %d, %d, %d}\n",
	       bb_rate_i->main_pkt_cnt_legacy[4], bb_rate_i->main_pkt_cnt_legacy[5],
	       bb_rate_i->main_pkt_cnt_legacy[6], bb_rate_i->main_pkt_cnt_legacy[7],
	       bb_rate_i->main_pkt_cnt_legacy[8], bb_rate_i->main_pkt_cnt_legacy[9],
	       bb_rate_i->main_pkt_cnt_legacy[10], bb_rate_i->main_pkt_cnt_legacy[11]);

	BB_DBG(bb, DBG_ANT_DIV, "*Aux-Ant OFDM    cnt:{%d, %d, %d, %d, %d, %d, %d, %d}\n",
	       bb_rate_i->aux_pkt_cnt_legacy[4], bb_rate_i->aux_pkt_cnt_legacy[5],
	       bb_rate_i->aux_pkt_cnt_legacy[6], bb_rate_i->aux_pkt_cnt_legacy[7],
	       bb_rate_i->aux_pkt_cnt_legacy[8], bb_rate_i->aux_pkt_cnt_legacy[9],
	       bb_rate_i->aux_pkt_cnt_legacy[10], bb_rate_i->aux_pkt_cnt_legacy[11]);

	if ((bb_rate_i->main_he_pkt_not_zero == true) || (bb_rate_i->aux_he_pkt_not_zero == true)) {
		for (i = 0; i < HE_RATE_NUM; i++) {

			if (bb_ant_div->tp_decision_method == TP_MAX_DOMINATION) {
				if (bb_rate_i->main_pkt_cnt_he[i] >= main_max_cnt) {
					main_max_cnt = bb_rate_i->main_pkt_cnt_he[i];
					main_max_idx = i;
				}
				if (bb_rate_i->aux_pkt_cnt_he[i] >= aux_max_cnt) {
					aux_max_cnt = bb_rate_i->aux_pkt_cnt_he[i];
					aux_max_idx = i;
				}
			} else if (bb_ant_div->tp_decision_method == TP_HIGHEST_DOMINATION) {
				if (bb_rate_i->main_pkt_cnt_he[i] > 0) {
					main_max_cnt = bb_rate_i->main_pkt_cnt_he[i];
					main_max_idx = i;
				}
				if (bb_rate_i->aux_pkt_cnt_he[i] > 0) {
					aux_max_cnt = bb_rate_i->aux_pkt_cnt_he[i];
					aux_max_idx = i;
				}
			}
		}

		for (i = 0; i < bb->num_rf_path; i++) {
			ss_ofst = HE_VHT_NUM_MCS * i;

			BB_DBG(bb, DBG_ANT_DIV,
				  "*Main-Ant HE %d-SS cnt:{%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d}\n",
				  (i + 1),
				  bb_rate_i->main_pkt_cnt_he[ss_ofst + 0],
				  bb_rate_i->main_pkt_cnt_he[ss_ofst + 1],
				  bb_rate_i->main_pkt_cnt_he[ss_ofst + 2],
				  bb_rate_i->main_pkt_cnt_he[ss_ofst + 3],
				  bb_rate_i->main_pkt_cnt_he[ss_ofst + 4],
				  bb_rate_i->main_pkt_cnt_he[ss_ofst + 5],
				  bb_rate_i->main_pkt_cnt_he[ss_ofst + 6],
				  bb_rate_i->main_pkt_cnt_he[ss_ofst + 7],
				  bb_rate_i->main_pkt_cnt_he[ss_ofst + 8],
				  bb_rate_i->main_pkt_cnt_he[ss_ofst + 9],
				  bb_rate_i->main_pkt_cnt_he[ss_ofst + 10],
				  bb_rate_i->main_pkt_cnt_he[ss_ofst + 11]);

			BB_DBG(bb, DBG_ANT_DIV,
				  "*Aux-Ant HE %d-SS cnt:{%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d}\n",
				  (i + 1),
				  bb_rate_i->aux_pkt_cnt_he[ss_ofst + 0],
				  bb_rate_i->aux_pkt_cnt_he[ss_ofst + 1],
				  bb_rate_i->aux_pkt_cnt_he[ss_ofst + 2],
				  bb_rate_i->aux_pkt_cnt_he[ss_ofst + 3],
				  bb_rate_i->aux_pkt_cnt_he[ss_ofst + 4],
				  bb_rate_i->aux_pkt_cnt_he[ss_ofst + 5],
				  bb_rate_i->aux_pkt_cnt_he[ss_ofst + 6],
				  bb_rate_i->aux_pkt_cnt_he[ss_ofst + 7],
				  bb_rate_i->aux_pkt_cnt_he[ss_ofst + 8],
				  bb_rate_i->aux_pkt_cnt_he[ss_ofst + 9],
				  bb_rate_i->aux_pkt_cnt_he[ss_ofst + 10],
				  bb_rate_i->aux_pkt_cnt_he[ss_ofst + 11]);

		}

	} else if ((bb_rate_i->main_vht_pkt_not_zero == true) || (bb_rate_i->aux_vht_pkt_not_zero == true)) {

		for (i = 0; i < VHT_RATE_NUM; i++) {

			if (bb_ant_div->tp_decision_method == TP_MAX_DOMINATION) {
				if (bb_rate_i->main_pkt_cnt_vht[i] >= main_max_cnt) {
					main_max_cnt = bb_rate_i->main_pkt_cnt_vht[i];
					main_max_idx = i;
				}
				if (bb_rate_i->aux_pkt_cnt_vht[i] >= aux_max_cnt) {
					aux_max_cnt = bb_rate_i->aux_pkt_cnt_vht[i];
					aux_max_idx = i;
				}
			} else if (bb_ant_div->tp_decision_method == TP_HIGHEST_DOMINATION) {
				if (bb_rate_i->main_pkt_cnt_vht[i] > 0) {
					main_max_cnt = bb_rate_i->main_pkt_cnt_vht[i];
					main_max_idx = i;
				}
				if (bb_rate_i->aux_pkt_cnt_vht[i] > 0) {
					aux_max_cnt = bb_rate_i->aux_pkt_cnt_vht[i];
					aux_max_idx = i;
				}
			}
		}


		for (i = 0; i < bb->num_rf_path; i++) {
			ss_ofst = HE_VHT_NUM_MCS * i;

			BB_DBG(bb, DBG_ANT_DIV,
				  "*Main-Ant VHT %d-SS cnt:{%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d}\n",
				  (i + 1),
				  bb_rate_i->main_pkt_cnt_vht[ss_ofst + 0],
				  bb_rate_i->main_pkt_cnt_vht[ss_ofst + 1],
				  bb_rate_i->main_pkt_cnt_vht[ss_ofst + 2],
				  bb_rate_i->main_pkt_cnt_vht[ss_ofst + 3],
				  bb_rate_i->main_pkt_cnt_vht[ss_ofst + 4],
				  bb_rate_i->main_pkt_cnt_vht[ss_ofst + 5],
				  bb_rate_i->main_pkt_cnt_vht[ss_ofst + 6],
				  bb_rate_i->main_pkt_cnt_vht[ss_ofst + 7],
				  bb_rate_i->main_pkt_cnt_vht[ss_ofst + 8],
				  bb_rate_i->main_pkt_cnt_vht[ss_ofst + 9],
				  bb_rate_i->main_pkt_cnt_vht[ss_ofst + 10],
				  bb_rate_i->main_pkt_cnt_vht[ss_ofst + 11]);

			BB_DBG(bb, DBG_ANT_DIV,
				  "*Aux-Ant VHT %d-SS cnt:{%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d}\n",
				  (i + 1),
				  bb_rate_i->aux_pkt_cnt_vht[ss_ofst + 0],
				  bb_rate_i->aux_pkt_cnt_vht[ss_ofst + 1],
				  bb_rate_i->aux_pkt_cnt_vht[ss_ofst + 2],
				  bb_rate_i->aux_pkt_cnt_vht[ss_ofst + 3],
				  bb_rate_i->aux_pkt_cnt_vht[ss_ofst + 4],
				  bb_rate_i->aux_pkt_cnt_vht[ss_ofst + 5],
				  bb_rate_i->aux_pkt_cnt_vht[ss_ofst + 6],
				  bb_rate_i->aux_pkt_cnt_vht[ss_ofst + 7],
				  bb_rate_i->aux_pkt_cnt_vht[ss_ofst + 8],
				  bb_rate_i->aux_pkt_cnt_vht[ss_ofst + 9],
				  bb_rate_i->aux_pkt_cnt_vht[ss_ofst + 10],
				  bb_rate_i->aux_pkt_cnt_vht[ss_ofst + 11]);
		}

	} else if ((bb_rate_i->main_ht_pkt_not_zero == true) || (bb_rate_i->aux_ht_pkt_not_zero == true)) {
		for (i = 0; i < HT_RATE_NUM; i++) {

			if (bb_ant_div->tp_decision_method == TP_MAX_DOMINATION) {
				if (bb_rate_i->main_pkt_cnt_ht[i] >= main_max_cnt) {
					main_max_cnt = bb_rate_i->main_pkt_cnt_ht[i];
					main_max_idx = i;
				}
				if (bb_rate_i->aux_pkt_cnt_ht[i] >= aux_max_cnt) {
					aux_max_cnt = bb_rate_i->aux_pkt_cnt_ht[i];
					aux_max_idx = i;
				}
			} else if (bb_ant_div->tp_decision_method == TP_HIGHEST_DOMINATION) {
				if (bb_rate_i->main_pkt_cnt_ht[i] > 0) {
					main_max_cnt = bb_rate_i->main_pkt_cnt_ht[i];
					main_max_idx = i;
				}
				if (bb_rate_i->aux_pkt_cnt_ht[i] > 0) {
					aux_max_cnt = bb_rate_i->aux_pkt_cnt_ht[i];
					aux_max_idx = i;
				}
			}
		}

		for (i = 0; i < bb->num_rf_path; i++) {
			ss_ofst = (i << 3);

			BB_DBG(bb, DBG_ANT_DIV,
			       "*HT%02d:%02d cnt:{%d, %d, %d, %d, %d, %d, %d, %d}\n",
			       (ss_ofst), (ss_ofst + 7),
			       bb_rate_i->main_pkt_cnt_ht[ss_ofst + 0],
			       bb_rate_i->main_pkt_cnt_ht[ss_ofst + 1],
			       bb_rate_i->main_pkt_cnt_ht[ss_ofst + 2],
			       bb_rate_i->main_pkt_cnt_ht[ss_ofst + 3],
			       bb_rate_i->main_pkt_cnt_ht[ss_ofst + 4],
			       bb_rate_i->main_pkt_cnt_ht[ss_ofst + 5],
			       bb_rate_i->main_pkt_cnt_ht[ss_ofst + 6],
			       bb_rate_i->main_pkt_cnt_ht[ss_ofst + 7]);

			BB_DBG(bb, DBG_ANT_DIV,
			       "*HT%02d:%02d cnt:{%d, %d, %d, %d, %d, %d, %d, %d}\n",
			       (ss_ofst), (ss_ofst + 7),
			       bb_rate_i->aux_pkt_cnt_ht[ss_ofst + 0],
			       bb_rate_i->aux_pkt_cnt_ht[ss_ofst + 1],
			       bb_rate_i->aux_pkt_cnt_ht[ss_ofst + 2],
			       bb_rate_i->aux_pkt_cnt_ht[ss_ofst + 3],
			       bb_rate_i->aux_pkt_cnt_ht[ss_ofst + 4],
			       bb_rate_i->aux_pkt_cnt_ht[ss_ofst + 5],
			       bb_rate_i->aux_pkt_cnt_ht[ss_ofst + 6],
			       bb_rate_i->aux_pkt_cnt_ht[ss_ofst + 7]);
		}

	}

	/* Compute all throughput*/
	if (bb_ant_div->tp_decision_method == TP_AVG_DOMINATION) {
		BB_DBG(bb, DBG_ANT_DIV, "Main_tp = %lld, Aux_tp = %lld\n", 
					 bb_rate_i->main_tp, bb_rate_i->aux_tp);

		if (bb_rate_i->main_tp > bb_rate_i->aux_tp) {
			bb_ant_div->target_ant_tp = MAIN_ANT;
			bb_rate_i->tp_diff = bb_rate_i->main_tp - bb_rate_i->aux_tp;
		} else if (bb_rate_i->main_tp < bb_rate_i->aux_tp) {
			bb_ant_div->target_ant_tp = AUX_ANT;
			bb_rate_i->tp_diff = bb_rate_i->aux_tp - bb_rate_i->main_tp;
		} else {
			bb_ant_div->target_ant_tp = bb_ant_div->pre_target_ant;
			bb_rate_i->no_change_flag = true;
			BB_DBG(bb, DBG_ANT_DIV, "TP based TargetAnt= Pre-TargetAnt\n");
		}

		BB_DBG(bb, DBG_ANT_DIV, "MCS based TargetAnt= [%s]\n", 
		(bb_ant_div->target_ant_tp == MAIN_ANT) ? "MAIN_ANT" : "AUX_ANT");
		BB_DBG(bb, DBG_ANT_DIV, "TP Confidence= [%lld]\n", bb_rate_i->tp_diff);

		return;
	}

	bb_rate_i->main_max_cnt = main_max_cnt;
	bb_rate_i->main_max_idx = main_max_idx;
	bb_rate_i->aux_max_cnt = aux_max_cnt;
	bb_rate_i->aux_max_idx = aux_max_idx;

	/* Decision Tput based target ant using MCS rate instead of phy data rate */
	if ((main_max_cnt > bb_ant_div->tp_lb) || (aux_max_cnt > bb_ant_div->tp_lb)) {
		if (main_max_idx > aux_max_idx) {
			bb_ant_div->target_ant_tp = MAIN_ANT;
			bb_rate_i->tp_diff = 100;
		} else if (main_max_idx < aux_max_idx) {
			bb_ant_div->target_ant_tp = AUX_ANT;
			bb_rate_i->tp_diff = 100;
		} else {
			if (main_max_cnt > aux_max_cnt)
				bb_ant_div->target_ant_tp = MAIN_ANT;
			else if (main_max_cnt < aux_max_cnt)
				bb_ant_div->target_ant_tp = AUX_ANT;
			else {
				bb_ant_div->target_ant_tp = bb_ant_div->pre_target_ant;
				bb_rate_i->no_change_flag = true;
			}
			/* Calc. TP confidence*/
			bb_rate_i->tp_diff = DIFF_2(main_max_cnt, aux_max_cnt);
		}
	} else {
		bb_ant_div->target_ant_tp = bb_ant_div->pre_target_ant;
		bb_rate_i->no_change_flag = true;
		BB_DBG(bb, DBG_ANT_DIV, "MCS based TargetAnt= Pre-TargetAnt\n");
	}

	if (bb_rate_i->no_change_flag == true)
		bb_rate_i->tp_diff = 0;

	if (bb_rate_i->tp_diff > 100)
		bb_rate_i->tp_diff = 100;

	BB_DBG(bb, DBG_ANT_DIV, "MCS based TargetAnt= [%s]\n", 
		(bb_ant_div->target_ant_tp == MAIN_ANT) ? "MAIN_ANT" : "AUX_ANT");
	BB_DBG(bb, DBG_ANT_DIV, "TP Confidence= [%lld]\n", bb_rate_i->tp_diff);
}

void halbb_antdiv_get_evm_target_ant(struct bb_info *bb)
{
	struct bb_antdiv_info *bb_ant_div = &bb->bb_ant_div_i;
	struct bb_link_info *bb_link = &bb->bb_link_i;
	struct bb_cmn_rpt_info	*cmn_rpt = &bb->bb_cmn_rpt_i;
	struct bb_rate_info *rate_i = &cmn_rpt->bb_rate_i;
	struct bb_antdiv_evm_info *bb_evm_i = &bb_ant_div->bb_evm_i;
	struct bb_antdiv_rate_info *bb_rate_i = &bb_ant_div->bb_rate_i;
	u8 main_2ss_evm_min = 0;
	u8 aux_2ss_evm_min = 0;
	//u8 main_2ss_evm_avg, aux_2ss_evm_avg;
	u8 main_1ss_evm = 0;
	u8 aux_1ss_evm = 0;
	u8 target_ant_evm_1ss = 0, target_ant_evm_2ss = 0;
	u8 decision_evm_ss = 0;
	u8 evm_diff_1ss = 0, evm_diff_2ss = 0;
	bool no_change_flag_1ss = false;
	bool no_change_flag_2ss = false;

	/* 1ss EVM */
	if (bb_ant_div->evm_decision_method == EVM_LINEAR_AVG) {
		/* Modify db to linear (*10)*/
		bb_evm_i->main_evm_1ss = HALBB_DIV(bb_evm_i->main_evm_1ss, 10);
		bb_evm_i->aux_evm_1ss = HALBB_DIV(bb_evm_i->aux_evm_1ss, 10);

		main_1ss_evm = (u8)halbb_convert_to_db((u64)HALBB_DIV(bb_evm_i->main_evm_1ss, 
			(bb_rate_i->main_pkt_cnt_1ss + bb_rate_i->main_pkt_cnt_ofdm)));
		aux_1ss_evm = (u8)halbb_convert_to_db((u64)HALBB_DIV(bb_evm_i->aux_evm_1ss, 
			(bb_rate_i->aux_pkt_cnt_1ss + bb_rate_i->aux_pkt_cnt_ofdm)));

		main_1ss_evm = main_1ss_evm << 2;
		aux_1ss_evm = aux_1ss_evm << 2;
	} else {
		main_1ss_evm =  (u8)HALBB_DIV(bb_evm_i->main_evm_1ss, 
			(bb_rate_i->main_pkt_cnt_1ss + bb_rate_i->main_pkt_cnt_ofdm));
		aux_1ss_evm =  (u8)HALBB_DIV(bb_evm_i->aux_evm_1ss, 
			(bb_rate_i->aux_pkt_cnt_1ss + bb_rate_i->aux_pkt_cnt_ofdm));
	}

	target_ant_evm_1ss = (main_1ss_evm == aux_1ss_evm) ? 
	(bb_ant_div->pre_target_ant) : ((main_1ss_evm >= aux_1ss_evm) ? MAIN_ANT : AUX_ANT);

	if (main_1ss_evm == aux_1ss_evm) {
		target_ant_evm_1ss = bb_ant_div->pre_target_ant;
		no_change_flag_1ss = true;
	} else if (main_1ss_evm > aux_1ss_evm) {
		target_ant_evm_1ss = MAIN_ANT;
	} else {
		target_ant_evm_1ss = AUX_ANT;
	}

	/* Calc. EVM confindece*/
	evm_diff_1ss = DIFF_2(main_1ss_evm, aux_1ss_evm);

	BB_DBG(bb, DBG_ANT_DIV, "%-9s (%02d.%03d)\n", "[Main-Ant 1ss-EVM_avg]",
	       (main_1ss_evm >> 2),
	       halbb_show_fraction_num(main_1ss_evm & 0x3, 2));

	BB_DBG(bb, DBG_ANT_DIV, "%-9s (%02d.%03d)\n", "[Aux-Ant 1ss-EVM_avg]",
	       (aux_1ss_evm >> 2),
	       halbb_show_fraction_num(aux_1ss_evm & 0x3, 2));

	/* 2ss EVM */
	if ((bb_rate_i->main_pkt_cnt_2ss + bb_rate_i->aux_pkt_cnt_2ss) != 0) {
		if (bb_ant_div->evm_decision_method == EVM_LINEAR_AVG) {

			bb_evm_i->main_evm_min_acc = HALBB_DIV(bb_evm_i->main_evm_min_acc, 10);
			bb_evm_i->aux_evm_min_acc = HALBB_DIV(bb_evm_i->aux_evm_min_acc, 10);

			main_2ss_evm_min = (u8)halbb_convert_to_db((u64)HALBB_DIV(bb_evm_i->main_evm_min_acc,
							      bb_rate_i->main_pkt_cnt_2ss));
			aux_2ss_evm_min = (u8)halbb_convert_to_db((u64)HALBB_DIV(bb_evm_i->aux_evm_min_acc,
							      bb_rate_i->aux_pkt_cnt_2ss));

			main_2ss_evm_min = main_2ss_evm_min << 2;
			aux_2ss_evm_min = aux_2ss_evm_min << 2;

		} else {
			main_2ss_evm_min = (u8)HALBB_DIV(bb_evm_i->main_evm_min_acc,
				bb_rate_i->main_pkt_cnt_2ss);
			aux_2ss_evm_min = (u8)HALBB_DIV(bb_evm_i->aux_evm_min_acc,
				bb_rate_i->aux_pkt_cnt_2ss);
		}

		if (main_2ss_evm_min == aux_2ss_evm_min) {
			target_ant_evm_2ss = bb_ant_div->pre_target_ant;
			no_change_flag_2ss = true;
		} else if (main_2ss_evm_min > aux_2ss_evm_min) {
			target_ant_evm_2ss = MAIN_ANT;
		} else {
			target_ant_evm_2ss = AUX_ANT;
		}

		/* Calc. EVM confindece*/
		evm_diff_2ss = DIFF_2(main_2ss_evm_min, aux_2ss_evm_min);
	}

	/*-----For Debug-----*/
	BB_DBG(bb, DBG_ANT_DIV, "%-9s (%02d.%03d) (%d)\n", "[Main-Ant 2ss-EVM_avg]",
	       (main_2ss_evm_min >> 2),
	       halbb_show_fraction_num(main_2ss_evm_min & 0x3, 2),main_2ss_evm_min);

	BB_DBG(bb, DBG_ANT_DIV, "%-9s (%02d.%03d) (%d)\n", "[Aux-Ant 2ss-EVM_avg]",
	       (aux_2ss_evm_min >> 2),
	       halbb_show_fraction_num(aux_2ss_evm_min & 0x3, 2), aux_2ss_evm_min);
	/*-------------------*/

	if ((bb_rate_i->main_pkt_cnt_2ss + bb_rate_i->aux_pkt_cnt_2ss) != 0) {
		decision_evm_ss = 2;
		bb_evm_i->evm_diff = evm_diff_2ss;
		bb_ant_div->target_ant_evm = target_ant_evm_2ss;
		bb_evm_i->no_change_flag = no_change_flag_2ss;
	} else {
		decision_evm_ss = 1;
		bb_evm_i->evm_diff = evm_diff_1ss;
		bb_ant_div->target_ant_evm = target_ant_evm_1ss;
		bb_evm_i->no_change_flag = no_change_flag_1ss;
	}

	if (bb_evm_i->no_change_flag == true) {
		bb_evm_i->evm_diff = 0;
		BB_DBG(bb, DBG_ANT_DIV, "EVM based TargetAnt= Pre-TargetAnt\n");
	}

	if (bb_evm_i->evm_diff > 100)
		bb_evm_i->evm_diff = 100;

	BB_DBG(bb, DBG_ANT_DIV, "%d-ss EVM based TargetAnt= [%s]\n", 
	decision_evm_ss, (bb_ant_div->target_ant_evm == MAIN_ANT) ? "MAIN_ANT" : "AUX_ANT");
	BB_DBG(bb, DBG_ANT_DIV, "EVM Confidence= [%d]\n", bb_evm_i->evm_diff);
}

void halbb_antdiv_training_state(struct bb_info *bb)
{
	struct bb_antdiv_info *bb_ant_div = &bb->bb_ant_div_i;
	struct bb_link_info *bb_link = &bb->bb_link_i;
	u8 next_ant;

	if (bb_ant_div->antdiv_training_state_cnt == 0) {
		bb_ant_div->get_stats = false;

		halbb_antdiv_reset_training_stat(bb);
		bb_ant_div->training_ant = bb_ant_div->pre_target_ant;

		next_ant = (bb_ant_div->training_ant == MAIN_ANT) ? MAIN_ANT : AUX_ANT;
		BB_DBG(bb, DBG_ANT_DIV, "Next training ant =%s\n",
			  (next_ant == MAIN_ANT) ? "MAIN_ANT" : "AUX_ANT");
		halbb_antdiv_set_ant(bb, next_ant);

		bb_ant_div->training_ant = next_ant;
		//bb_ant_div->pre_target_ant = next_ant;

		bb_ant_div->antdiv_training_state_cnt++;

		BB_DBG(bb, DBG_ANT_DIV, "%s Statistics Interval=%d ms\n",
			((next_ant == MAIN_ANT) ? "MAIN_ANT" : "AUX_ANT"), bb_ant_div->antdiv_intvl);
		bb_ant_div->get_stats = true;
		bb_ant_div->antdiv_timer_i.cb_time= bb_ant_div->antdiv_intvl;
		halbb_cfg_timers(bb, BB_SET_TIMER, &bb_ant_div->antdiv_timer_i);

	} else if ((bb_ant_div->antdiv_training_state_cnt % 2) == 0) {
		bb_ant_div->antdiv_training_state_cnt++;

		next_ant = bb_ant_div->training_ant;

		BB_DBG(bb, DBG_ANT_DIV, "%s Statistics Interval=%d ms\n",
			((next_ant == MAIN_ANT) ? "MAIN_ANT" : "AUX_ANT"), bb_ant_div->antdiv_intvl);

		bb_ant_div->get_stats = true;
		bb_ant_div->antdiv_timer_i.cb_time = bb_ant_div->antdiv_intvl;
		halbb_cfg_timers(bb, BB_SET_TIMER, &bb_ant_div->antdiv_timer_i);

	} else if ((bb_ant_div->antdiv_training_state_cnt % 2) != 0) {
		bb_ant_div->get_stats = false;

		next_ant = (bb_ant_div->training_ant == MAIN_ANT) ? AUX_ANT : MAIN_ANT;
		BB_DBG(bb, DBG_ANT_DIV, "Next training ant =%s\n",
			  (next_ant == MAIN_ANT) ? "MAIN_ANT" : "AUX_ANT");
		halbb_antdiv_set_ant(bb, next_ant);

		bb_ant_div->training_ant = next_ant;

		bb_ant_div->antdiv_training_state_cnt++;

		BB_DBG(bb, DBG_ANT_DIV, "%s Delay=%d ms\n",
			((next_ant == MAIN_ANT) ? "MAIN_ANT" : "AUX_ANT"), bb_ant_div->antdiv_delay);
		bb_ant_div->antdiv_timer_i.cb_time = bb_ant_div->antdiv_delay;
		halbb_cfg_timers(bb, BB_SET_TIMER, &bb_ant_div->antdiv_timer_i);
	}
}

void halbb_antdiv_decision_state(struct bb_info *bb)
{
	struct bb_antdiv_info *bb_ant_div = &bb->bb_ant_div_i;
	struct bb_link_info *bb_link = &bb->bb_link_i;
	struct bb_cmn_rpt_info	*cmn_rpt = &bb->bb_cmn_rpt_i;
	struct bb_rate_info *rate_i = &cmn_rpt->bb_rate_i;
	struct bb_antdiv_rate_info *bb_rate_i = &bb_ant_div->bb_rate_i;
	struct bb_antdiv_evm_info *bb_evm_i = &bb_ant_div->bb_evm_i;

	BB_DBG(bb, DBG_ANT_DIV, "[Decisoin state]\n");

	bb_ant_div->get_stats = false;
	bb_ant_div->antdiv_training_state_cnt = 0;
	/* Check highest MCS idx of main & aux antenna*/
	halbb_antdiv_get_highest_mcs(bb);

	/* EVM based antenna diversity */
	halbb_antdiv_get_evm_target_ant(bb);

	/* CN based antenna diversity*/
	//halbb_antdiv_get_cn_target_ant(bb);

	/* TP based antenna diversity */
	//halbb_antdiv_get_tp_target_ant(bb);

	/* Final Decision */
	if ((bb_rate_i->main_cnt_all + bb_rate_i->aux_cnt_all) == 0) {
		BB_DBG(bb, DBG_ANT_DIV, "Not enough count, remain previous antenna\n");
		bb_ant_div->target_ant = bb_ant_div->pre_target_ant;
		BB_DBG(bb, DBG_ANT_DIV, "Make decision again ASAP\n");
		bb_ant_div->antdiv_wd_cnt = bb_ant_div->antdiv_period;
		return;
	}

	if (bb_rate_i->no_change_flag && bb_evm_i->no_change_flag) {
		BB_DBG(bb, DBG_ANT_DIV, "No Decision, remain previous antenna\n");
		bb_ant_div->target_ant = bb_ant_div->pre_target_ant;
		BB_DBG(bb, DBG_ANT_DIV, "Make decision again ASAP\n");
		bb_ant_div->antdiv_wd_cnt = bb_ant_div->antdiv_period;
		return;
	}

	if ((bb_ant_div->target_ant_evm == bb_ant_div->target_ant_tp) &&
		(!bb_rate_i->no_change_flag) && (!bb_evm_i->no_change_flag)) {
		BB_DBG(bb, DBG_ANT_DIV, "Decision confidence is enough\n");
		bb_ant_div->target_ant = bb_ant_div->target_ant_evm;
	} else {
		if (bb_rate_i->tp_diff >= bb_ant_div->tp_diff_th_high) {
			BB_DBG(bb, DBG_ANT_DIV, "TP confidence is %lld > %d, Decided by MCS based\n"
			,bb_rate_i->tp_diff, bb_ant_div->tp_diff_th_low);
			bb_ant_div->target_ant = bb_ant_div->target_ant_tp;
		} else if (bb_evm_i->evm_diff >= bb_ant_div->evm_diff_th) {
			BB_DBG(bb, DBG_ANT_DIV, "EVM confidence is more than %d, Decided by EVM based\n"
						,bb_ant_div->evm_diff_th);
			bb_ant_div->target_ant = bb_ant_div->target_ant_evm;
		} else if (bb_rate_i->tp_diff >= bb_ant_div->tp_diff_th_low) {
			BB_DBG(bb, DBG_ANT_DIV, "TP confidence is %lld > %d, Decided by MCS based\n"
			,bb_rate_i->tp_diff, bb_ant_div->tp_diff_th_low);
			bb_ant_div->target_ant = bb_ant_div->target_ant_tp;
		} else {
			BB_DBG(bb, DBG_ANT_DIV, "Decision confidence is not enough, remain previous antenna\n");
			bb_ant_div->target_ant = bb_ant_div->pre_target_ant;

			BB_DBG(bb, DBG_ANT_DIV, "Make decision again ASAP\n");
			bb_ant_div->antdiv_wd_cnt = bb_ant_div->antdiv_period;
		}
	}

	BB_DBG(bb, DBG_ANT_DIV, "TargetAnt= [%s]\n", (bb_ant_div->target_ant == MAIN_ANT) ? "MAIN_ANT" : "AUX_ANT");

}

void halbb_antdiv_1ss_decision_state(struct bb_info * bb)
{
	struct bb_antdiv_info *bb_ant_div = &bb->bb_ant_div_i;
	struct bb_antdiv_rssi_info *bb_rssi_i = &bb_ant_div->bb_rssi_i;
	struct bb_antdiv_rate_info *bb_rate_i = &bb_ant_div->bb_rate_i;
	struct bb_antdiv_evm_info *bb_evm_i = &bb_ant_div->bb_evm_i;

	BB_DBG(bb, DBG_ANT_DIV, "%s ======>\n", __func__);

	bb_ant_div->get_stats = false;
	bb_ant_div->antdiv_training_state_cnt = 0;

	/* Check highest MCS idx of main & aux antenna*/
	halbb_antdiv_get_highest_mcs(bb);

	/* EVM based antenna diversity */
	halbb_antdiv_get_evm_target_ant(bb);

	/* RSSI based antenna diversity*/
	halbb_antdiv_get_rssi_target_ant(bb);

	/* Final Decision */
	if ((bb_rate_i->main_cnt_all + bb_rate_i->aux_cnt_all) == 0) {
		BB_DBG(bb, DBG_ANT_DIV, "Not enough count, remain previous antenna\n");
		bb_ant_div->target_ant = bb_ant_div->pre_target_ant;
		BB_DBG(bb, DBG_ANT_DIV, "Make decision again ASAP\n");
		bb_ant_div->antdiv_wd_cnt = bb_ant_div->antdiv_period;
		bb_ant_div->antdiv_notrain_cnt = bb_ant_div->antdiv_notrain_num;
		return;
	}

	if (bb_rate_i->no_change_flag && bb_evm_i->no_change_flag && bb_rssi_i->no_change_flag) {
		BB_DBG(bb, DBG_ANT_DIV, "No Decision, remain previous antenna\n");
		bb_ant_div->target_ant = bb_ant_div->pre_target_ant;
		BB_DBG(bb, DBG_ANT_DIV, "Make decision again ASAP\n");
		bb_ant_div->antdiv_wd_cnt = bb_ant_div->antdiv_period;
		bb_ant_div->antdiv_notrain_cnt = bb_ant_div->antdiv_notrain_num;
		return;
	}

	if ((bb_ant_div->target_ant_rssi == bb_ant_div->target_ant_tp) &&
		(bb_ant_div->target_ant_rssi == bb_ant_div->target_ant_evm) &&
		(!bb_rate_i->no_change_flag) && (!bb_rssi_i->no_change_flag) && (!bb_evm_i->no_change_flag)) {
		BB_DBG(bb, DBG_ANT_DIV, "Decision confidence is enough\n");
		bb_ant_div->target_ant = bb_ant_div->target_ant_rssi;
	} else {
		if (bb_rate_i->tp_diff >= bb_ant_div->tp_diff_th_high) {
			BB_DBG(bb, DBG_ANT_DIV, "TP confidence is %lld >= %d, Decided by MCS based\n"
			,bb_rate_i->tp_diff, bb_ant_div->tp_diff_th_high);
			bb_ant_div->target_ant = bb_ant_div->target_ant_tp;
		} else if (bb_evm_i->evm_diff >= bb_ant_div->evm_diff_th) {
			BB_DBG(bb, DBG_ANT_DIV, "EVM confidence is more than %d, Decided by EVM based\n"
			,bb_ant_div->evm_diff_th);
			bb_ant_div->target_ant = bb_ant_div->target_ant_evm;
		} else if (bb_rate_i->tp_diff >= bb_ant_div->tp_diff_th_low) {
			BB_DBG(bb, DBG_ANT_DIV, "TP confidence is %lld > %d, Decided by MCS based\n"
			,bb_rate_i->tp_diff, bb_ant_div->tp_diff_th_low);
			bb_ant_div->target_ant = bb_ant_div->target_ant_tp;
		} else if (!bb_rssi_i->no_change_flag) {
			BB_DBG(bb, DBG_ANT_DIV, "Decided by RSSI based!\n");
			bb_ant_div->target_ant = bb_ant_div->target_ant_rssi;
		} else {
			BB_DBG(bb, DBG_ANT_DIV, "Decision confidence is not enough, remain previous antenna\n");
			bb_ant_div->target_ant = bb_ant_div->pre_target_ant;
			BB_DBG(bb, DBG_ANT_DIV, "Make decision again ASAP\n");
			bb_ant_div->antdiv_wd_cnt = bb_ant_div->antdiv_period;
			bb_ant_div->antdiv_notrain_cnt = bb_ant_div->antdiv_notrain_num;
			return;
		}
	}

	if (bb_ant_div->target_ant != bb_ant_div->pre_target_ant) {
		bb_ant_div->antdiv_wd_cnt = bb_ant_div->antdiv_period;
		bb_ant_div->antdiv_notrain_cnt = bb_ant_div->antdiv_notrain_num;
	}
	else { /*if ant. doesn't change, no training for 2 x antdiv_notrain_cnt_th sec.*/
		bb_ant_div->antdiv_wd_cnt = bb_ant_div->antdiv_period;
		bb_ant_div->antdiv_notrain_cnt = 0;
		BB_DBG(bb, DBG_ANT_DIV, "Ant no change, no training for 2 x antdiv_notrain_cnt_th sec.\n");
	}
	BB_DBG(bb, DBG_ANT_DIV, "TargetAnt= [%s]\n", (bb_ant_div->target_ant == MAIN_ANT) ? "MAIN_ANT" : "AUX_ANT");
}

void halbb_evm_based_antdiv(struct bb_info *bb)
{
	struct bb_antdiv_info *bb_ant_div = &bb->bb_ant_div_i;
	struct bb_link_info *bb_link = &bb->bb_link_i;
	struct bb_cmn_rpt_info	*cmn_rpt = &bb->bb_cmn_rpt_i;
	struct bb_rssi_su_avg_info *avg = &cmn_rpt->bb_rssi_su_avg_i;
	struct bb_rate_info *rate_i = &cmn_rpt->bb_rate_i;

	/* Main function */
	if (bb_ant_div->antdiv_training_state_cnt <= ((bb_ant_div->antdiv_train_num << 1) - 2)) {
		BB_DBG(bb, DBG_ANT_DIV, "Ant_diversity training state = %d \n",
					bb_ant_div->antdiv_training_state_cnt);
		halbb_antdiv_training_state(bb);
		return;
	} else {
		if (bb->ic_type & BB_IC_1SS)
			halbb_antdiv_1ss_decision_state(bb);
		else
			halbb_antdiv_decision_state(bb);
	}

	/* Set new target antenna */

	BB_DBG(bb, DBG_ANT_DIV, "Ant_diversity done\n");
	halbb_antdiv_set_ant(bb, bb_ant_div->target_ant);

	/* Re-assign to next step reqired variable */
	bb_ant_div->target_ant_rssi = bb_ant_div->target_ant;
	bb_ant_div->target_ant_evm = bb_ant_div->target_ant;
	bb_ant_div->target_ant_tp = bb_ant_div->target_ant;
	bb_ant_div->target_ant_cn = bb_ant_div->target_ant;
	bb_ant_div->pre_target_ant = bb_ant_div->target_ant;

}

void halbb_antenna_diversity(struct bb_info *bb)
{
	struct bb_antdiv_info *bb_ant_div = &bb->bb_ant_div_i;
	struct bb_link_info *bb_link = &bb->bb_link_i;
	struct bb_antdiv_rssi_info *rssi_stat = &bb_ant_div->bb_rssi_i;
	struct rtw_phl_com_t *phl = bb->phl_com;
	struct dev_cap_t *dev = &phl->dev_cap;
	u8 rssi, rssi_pre;

	BB_DBG(bb, DBG_ANT_DIV, "%s ======>\n", __func__);

	BB_DBG(bb, DBG_ANT_DIV, "RFE_TYPE = %d\n",dev->rfe_type);

	/* Early return */
	if(phl_is_mp_mode(bb->phl_com)) {
		BB_DBG(bb, DBG_ANT_DIV, "Early return - MP mode\n");
		return;
	}

	if (!(bb->support_ability & BB_ANT_DIV)) {
		BB_DBG(bb, DBG_ANT_DIV, "Early return - Not support antenna diversity\n");
		return;
	}
	BB_DBG(bb, DBG_CMN, "WARNING!! ANT_DIV is enable now\n");

	if (!(bb_link->is_linked) || !(bb_link->is_one_entry_only)) {
		BB_DBG(bb, DBG_ANT_DIV, "Early return - is_linked=%d, one_entry_only=%d\n",
			  bb_link->is_linked, bb_link->is_one_entry_only);
		return;
	}

	if (bb_link->at_least_one_bfee) {
		BB_DBG(bb, DBG_ANT_DIV, "Early return - at least one macid rx bfee=%d\n",
		       bb_link->at_least_one_bfee);
		return;
	}

	halbb_antdiv_get_rssi(bb);
	rssi = rssi_stat->rssi_final >> 1;
	rssi_pre = bb_ant_div->rssi_pre >> 1;
	BB_DBG(bb, DBG_ANT_DIV, "RSSI=%d, RSSI_pre=%d, FIX_ANT_RSSI_DIFF_th=%d\n", rssi, rssi_pre, bb_ant_div->fixant_rssi_diff);
	BB_DBG(bb, DBG_ANT_DIV, "[cck_cnt, ofdm_cnt, t_cnt] = [%d, %d, %d]\n", rssi_stat->pkt_cnt_cck, rssi_stat->pkt_cnt_ofdm, rssi_stat->pkt_cnt_t);
	bb_ant_div->rssi_pre =  rssi_stat->rssi_final;
	halbb_mem_set(bb, rssi_stat, 0, sizeof(struct bb_antdiv_rssi_info));

	/*if (rssi < ANTDIV_RSSI_TH_HIGH) {
		BB_DBG(bb, DBG_ANT_DIV, "RSSI=%d is too low to do Ant_diveristy\n", rssi);
		halbb_antdiv_set_ant(bb, MAIN_ANT);
		return;
	}*/

	/* Fixed-antenna diversity mode */
	if (bb_ant_div->antdiv_mode != AUTO_ANT) {
		BB_DBG(bb, DBG_ANT_DIV, "Fix Antenna at (( %s ))\n",
			  (bb_ant_div->antdiv_mode == FIX_MAIN_ANT) ? "MAIN" : "AUX");

		if (bb_ant_div->antdiv_mode != bb_ant_div->pre_antdiv_mode) {

			if (bb_ant_div->antdiv_mode == FIX_MAIN_ANT) {
				halbb_antdiv_set_ant(bb, MAIN_ANT);
			}
			else if (bb_ant_div->antdiv_mode == FIX_AUX_ANT) {
				halbb_antdiv_set_ant(bb, AUX_ANT);
			}
		}
		bb_ant_div->pre_antdiv_mode = bb_ant_div->antdiv_mode;
		return;
	}

	if (bb->ic_type & BB_IC_1SS) {
		if (DIFF_2(rssi, rssi_pre) >= bb_ant_div->fixant_rssi_diff)
			bb_ant_div->antdiv_notrain_cnt = bb_ant_div->antdiv_notrain_num;
		if (bb_ant_div->antdiv_notrain_cnt < bb_ant_div->antdiv_notrain_num) {
			bb_ant_div->antdiv_notrain_cnt++;
			BB_DBG(bb, DBG_ANT_DIV, "notrain_num = %d, notrain_cnt = %d\n",
				bb_ant_div->antdiv_notrain_num, bb_ant_div->antdiv_notrain_cnt);
			return;
		}
		else
			bb_ant_div->antdiv_notrain_cnt = bb_ant_div->antdiv_notrain_num;
	}

	if (bb_link->one_entry_tp_active_occur) { // if TP > th., do antdiv per watchdog
		BB_DBG(bb, DBG_ANT_DIV, "TP occur, do antdv immediately\n");
		bb_ant_div->antdiv_wd_cnt = bb_ant_div->antdiv_period;
	}
	if (bb_ant_div->antdiv_wd_cnt < bb_ant_div->antdiv_period) { // else, do antdiv after specific watchdog
		BB_DBG(bb, DBG_ANT_DIV, "Ant-div period = %d, watchdog count = %d\n",
			bb_ant_div->antdiv_period, bb_ant_div->antdiv_wd_cnt);
		bb_ant_div->antdiv_wd_cnt++;
		return;
	} else {
		bb_ant_div->antdiv_wd_cnt = 0;
	}

	/* Main section of EVM/TP based antenna diversity*/
	BB_DBG(bb, DBG_ANT_DIV, "Ant_diversity start\n");
	halbb_evm_based_antdiv(bb);
}

void halbb_antdiv_get_rssi_stat(struct bb_info *bb)
{
	struct bb_physts_rslt_hdr_info	*psts_h = &bb->bb_physts_i.bb_physts_rslt_hdr_i;
	struct bb_cmn_rpt_info	*cmn_rpt = &bb->bb_cmn_rpt_i;
	struct bb_antdiv_info *bb_ant_div = &bb->bb_ant_div_i;
	struct bb_antdiv_rssi_info *rssi = &bb_ant_div->bb_rssi_i;
	struct bb_rate_info *rate_i = &cmn_rpt->bb_rate_i;

	if (rate_i->mode == BB_LEGACY_MODE) {
		if (cmn_rpt->is_cck_rate) {
			rssi->rssi_cck_avg_acc += psts_h->rssi_avg;
			rssi->pkt_cnt_cck++;

		} else {
			rssi->rssi_ofdm_avg_acc += psts_h->rssi_avg;
			rssi->pkt_cnt_ofdm++;

		}
	} else {
		rssi->rssi_t_avg_acc+= psts_h->rssi_avg;
		rssi->pkt_cnt_t++;
	}

	/* Only get main/aux rssi stats @ training period */
	if (!bb_ant_div->get_stats)
		return;

	if (bb_ant_div->training_ant == MAIN_ANT) {
		if (rate_i->mode == BB_LEGACY_MODE) {
			if (cmn_rpt->is_cck_rate) {
				rssi->main_rssi_cck_avg_acc += psts_h->rssi_avg;
				rssi->main_pkt_cnt_cck++;

			} else {
				rssi->main_rssi_ofdm_avg_acc += psts_h->rssi_avg;
				rssi->main_pkt_cnt_ofdm++;

			}
		} else {
			rssi->main_rssi_t_avg_acc+= psts_h->rssi_avg;
			rssi->main_pkt_cnt_t++;
		}
	} else if(bb_ant_div->training_ant == AUX_ANT) { 
		if (rate_i->mode == BB_LEGACY_MODE) {
			if (cmn_rpt->is_cck_rate) {
				rssi->aux_rssi_cck_avg_acc += psts_h->rssi_avg;
				rssi->aux_pkt_cnt_cck++;

			} else {
				rssi->aux_rssi_ofdm_avg_acc += psts_h->rssi_avg;
				rssi->aux_pkt_cnt_ofdm++;

			}
		} else {
			rssi->aux_rssi_t_avg_acc+= psts_h->rssi_avg;
			rssi->aux_pkt_cnt_t++;
		}
	}
}

void halbb_antdiv_get_evm_stat(struct bb_info *bb)
{
	struct bb_physts_rslt_1_info	*psts_1 = &bb->bb_physts_i.bb_physts_rslt_1_i;
	struct bb_cmn_rpt_info	*cmn_rpt = &bb->bb_cmn_rpt_i;
	struct bb_rate_info *rate_i = &cmn_rpt->bb_rate_i;

	struct bb_antdiv_info *bb_ant_div = &bb->bb_ant_div_i;
	struct bb_antdiv_evm_info *bb_evm_i = &bb_ant_div->bb_evm_i;
	struct bb_link_info *bb_link = &bb->bb_link_i;

	/* Only get stats @ training period */
	if (!bb_ant_div->get_stats)
		return;

	/* No EVM info. @ cck rate */
	if (cmn_rpt->is_cck_rate)
		return;

	if (bb_ant_div->evm_decision_method == EVM_LINEAR_AVG) {
		if(bb_ant_div->training_ant == MAIN_ANT) {
			if (rate_i->ss == 1) {
				bb_evm_i->main_evm_1ss += (u32)halbb_db_2_linear((psts_1->evm_min >> 2));
			} else {
				bb_evm_i->main_evm_min_acc += (u32)halbb_db_2_linear((psts_1->evm_min>> 2));

				bb_evm_i->main_evm_max_acc += (u32)halbb_db_2_linear((psts_1->evm_max >> 2));
			}
		} else if(bb_ant_div->training_ant == AUX_ANT) {
			if (rate_i->ss == 1) {
				bb_evm_i->aux_evm_1ss += (u32)halbb_db_2_linear((psts_1->evm_min >> 2));
			} else {
				bb_evm_i->aux_evm_min_acc += (u32)halbb_db_2_linear((psts_1->evm_min >> 2));

				bb_evm_i->aux_evm_max_acc += (u32)halbb_db_2_linear((psts_1->evm_max >> 2));
			}
		}
	} else {
		if(bb_ant_div->training_ant == MAIN_ANT) {
			if (rate_i->ss == 1) {
				bb_evm_i->main_evm_1ss += psts_1->evm_min;
			} else {
				bb_evm_i->main_evm_min_acc += psts_1->evm_min;

				bb_evm_i->main_evm_max_acc += psts_1->evm_max;
			}
		} else if(bb_ant_div->training_ant == AUX_ANT) {
			if (rate_i->ss == 1) {
				bb_evm_i->aux_evm_1ss += psts_1->evm_min;
			} else {
				bb_evm_i->aux_evm_min_acc += psts_1->evm_min;

				bb_evm_i->aux_evm_max_acc += psts_1->evm_max;
			}
		}
	}
}

void halbb_antdiv_get_cn_stat(struct bb_info *bb)
{
	struct bb_physts_rslt_1_info	*psts_1 = &bb->bb_physts_i.bb_physts_rslt_1_i;
	struct bb_cmn_rpt_info	*cmn_rpt = &bb->bb_cmn_rpt_i;
	struct bb_rate_info *rate_i = &cmn_rpt->bb_rate_i;

	struct bb_antdiv_info *bb_ant_div = &bb->bb_ant_div_i;
	struct bb_antdiv_cn_info *bb_cn_i = &bb_ant_div->bb_cn_i;
	struct bb_link_info *bb_link = &bb->bb_link_i;

	/* Only get stats @ training period */
	if (!bb_ant_div->get_stats)
		return;

	/* Data frame only */
	#if 0
	if (bb_ant_div->antdiv_use_ctrl_frame) {
		if (!bb_link->is_match_bssid)
			return;
	}
	#endif

	/* No CN info. @ cck rate */
	if (cmn_rpt->is_cck_rate)
		return;

	if (rate_i->ss == 0)
		return;

	if(bb_ant_div->training_ant == MAIN_ANT) {
		bb_cn_i->main_cn_avg_acc += psts_1->cn_avg;
	} else if(bb_ant_div->training_ant == AUX_ANT) {
		bb_cn_i->aux_cn_avg_acc += psts_1->cn_avg;
	}
}

void halbb_antdiv_get_rate_stat(struct bb_info *bb)
{
	struct bb_cmn_rpt_info	*cmn_rpt = &bb->bb_cmn_rpt_i;
	//struct bb_pkt_cnt_su_info *pkt_cnt = &cmn_rpt->bb_pkt_cnt_su_i;
	struct bb_rate_info *rate_i = &cmn_rpt->bb_rate_i;

	struct bb_antdiv_info *bb_ant_div = &bb->bb_ant_div_i;
	struct bb_antdiv_rate_info *bb_rate_i = &bb_ant_div->bb_rate_i;
	struct bb_link_info *bb_link = &bb->bb_link_i;
	u8 ofst = rate_i->idx;

	/* Only get stats @ training period */
	if (!bb_ant_div->get_stats)
		return;

	/* Data frame only */
	#if 0
	if (bb_ant_div->antdiv_use_ctrl_frame) {
		if (!bb_link->is_match_bssid)
			return;
	}
	#endif

	//BB_DBG(bb, DBG_ANT_DIV, "Rate mode= %d\n", rate_i->mode);
	//BB_DBG(bb, DBG_ANT_DIV, "Training antenna= %d\n", bb_ant_div->training_ant);

	if(bb_ant_div->training_ant == MAIN_ANT) {
		/* pkt_cnt acc */
		//BB_DBG(bb, DBG_ANT_DIV, "MAIN_ANT rate!\n");
		bb_rate_i->main_cnt_all++;
		if (rate_i->mode == BB_LEGACY_MODE) {
			if (cmn_rpt->is_cck_rate)
				bb_rate_i->main_pkt_cnt_cck++;
			else
				bb_rate_i->main_pkt_cnt_ofdm++;
		} else {
			bb_rate_i->main_pkt_cnt_t++;
			if (rate_i->ss == 1)
				bb_rate_i->main_pkt_cnt_1ss++;
			else if (rate_i->ss == 2)
				bb_rate_i->main_pkt_cnt_2ss++;
		}

		/* rate cnt acc*/
		if (rate_i->mode == BB_LEGACY_MODE) {
			bb_rate_i->main_pkt_cnt_legacy[ofst]++;
			return;
		}


		if (rate_i->ss >= 2 && rate_i->mode >= BB_VHT_MODE)
			ofst += (HE_VHT_NUM_MCS * (rate_i->ss - 1));

		if (rate_i->mode == BB_HT_MODE) {
			bb_rate_i->main_ht_pkt_not_zero = true;
			ofst = NOT_GREATER(ofst, HT_RATE_NUM - 1);
			bb_rate_i->main_pkt_cnt_ht[ofst]++;
			/* shift ofst due to mismatch of HT/VHT rate num*/
			ofst += ((ofst << 3) >> 2);
			bb_rate_i->main_tp += 
				      bb_phy_rate_table[ofst + LEGACY_RATE_NUM];
		} else if (rate_i->mode == BB_VHT_MODE) {
			bb_rate_i->main_vht_pkt_not_zero = true;
			ofst = NOT_GREATER(ofst, VHT_RATE_NUM - 1);
			bb_rate_i->main_pkt_cnt_vht[ofst]++;
			bb_rate_i->main_tp += 
				      bb_phy_rate_table[ofst + LEGACY_RATE_NUM];
		} else if (rate_i->mode == BB_HE_MODE) {
			bb_rate_i->main_he_pkt_not_zero = true;
			ofst = NOT_GREATER(ofst, HE_RATE_NUM - 1);
			bb_rate_i->main_pkt_cnt_he[ofst]++;
			bb_rate_i->main_tp += 
				      bb_phy_rate_table[ofst + LEGACY_RATE_NUM];
		}

	} else if(bb_ant_div->training_ant == AUX_ANT) {
		bb_rate_i->aux_cnt_all++;
		//BB_DBG(bb, DBG_ANT_DIV, "Aux_ANT rate!\n");
		/* pkt_cnt acc */
		if (rate_i->mode == BB_LEGACY_MODE) {
			if (cmn_rpt->is_cck_rate)
				bb_rate_i->aux_pkt_cnt_cck++;
			else
				bb_rate_i->aux_pkt_cnt_ofdm++;
		} else {
			bb_rate_i->aux_pkt_cnt_t++;
			if (rate_i->ss == 1)
				bb_rate_i->aux_pkt_cnt_1ss++;
			else if (rate_i->ss == 2)
				bb_rate_i->aux_pkt_cnt_2ss++;
		}

		if (rate_i->mode == BB_LEGACY_MODE) {
			bb_rate_i->aux_pkt_cnt_legacy[ofst]++;
			return;
		}

		if (rate_i->ss >= 2 && rate_i->mode >= BB_VHT_MODE)
			ofst += (HE_VHT_NUM_MCS * (rate_i->ss - 1));

		if (rate_i->mode == BB_HT_MODE) {
			bb_rate_i->aux_ht_pkt_not_zero = true;
			ofst = NOT_GREATER(ofst, HT_RATE_NUM - 1);
			bb_rate_i->aux_pkt_cnt_ht[ofst]++;
			/* shift ofst due to mismatch of HT/VHT rate num*/
			ofst += ((ofst >> 3) << 2);
			bb_rate_i->aux_tp += 
				      bb_phy_rate_table[ofst + LEGACY_RATE_NUM];
		} else if (rate_i->mode == BB_VHT_MODE) {
			bb_rate_i->aux_vht_pkt_not_zero = true;
			ofst = NOT_GREATER(ofst, VHT_RATE_NUM - 1);
			bb_rate_i->aux_pkt_cnt_vht[ofst]++;
			bb_rate_i->aux_tp += 
				      bb_phy_rate_table[ofst + LEGACY_RATE_NUM];
		} else if (rate_i->mode == BB_HE_MODE) {
			bb_rate_i->aux_he_pkt_not_zero = true;
			ofst = NOT_GREATER(ofst, HE_RATE_NUM - 1);
			bb_rate_i->aux_pkt_cnt_he[ofst]++;
			bb_rate_i->aux_tp += 
				      bb_phy_rate_table[ofst + LEGACY_RATE_NUM];
		}
	}
}

void halbb_antdiv_phy_sts(struct bb_info *bb, u32 physts_bitmap,
		       struct physts_rxd *desc) {

	struct bb_physts_info	*physts = &bb->bb_physts_i;
	struct dev_cap_t *dev = &bb->phl_com->dev_cap;
	struct rtw_phl_stainfo_t *sta;
	struct rtw_cfo_info *cfo_t = NULL;
	u8 bb_macid;

	halbb_antdiv_get_rssi_stat(bb);

	if (!(physts_bitmap & BIT(IE01_CMN_OFDM) &&
	    physts->bb_physts_rslt_hdr_i.ie_map_type >= LEGACY_OFDM_PKT))
		return;

	if (desc->macid_su > PHL_MAX_STA_NUM)
		BB_WARNING("[%s] macid_su=%d\n", __func__, desc->macid_su);

	bb_macid = bb->phl2bb_macid_table[desc->macid_su];

	if (bb_macid > PHL_MAX_STA_NUM)
		BB_WARNING("[%s] bb_macid=%d\n", __func__, bb_macid);

	sta = bb->phl_sta_info[bb_macid];

	if (!is_sta_active(sta))
		return;

	if (sta->macid > PHL_MAX_STA_NUM)
		return;

	if (!sta->hal_sta)
		return;

	if ((dev->rfe_type >= 50) && (bb_macid == 0)) /* No need to cnt AP Rx boardcast pkt*/
		return;

	halbb_antdiv_get_rate_stat(bb);
	halbb_antdiv_get_evm_stat(bb);
	halbb_antdiv_get_cn_stat(bb);
}

void halbb_antdiv_dbg(struct bb_info *bb, char input[][16], u32 *_used,
			      char *output, u32 *_out_len)
{
	struct bb_antdiv_info *bb_ant_div = &bb->bb_ant_div_i;
	struct bb_antdiv_cr_info *cr = &bb->bb_ant_div_i.bb_antdiv_cr_i;

	char help[] = "-h";
	u32 var[10] = {0};
	u32 used = *_used;
	u32 out_len = *_out_len;

	if ((_os_strcmp(input[1], help) == 0)) {
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "Init Antenna Diversity: {0}: halbb_antdiv_init.\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "Set Antenna Diversity Mode: {1} {0}: Auto-mode {1}: Fix main ant. {2}: Fix aux ant.\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "Set Antenna Diversity Method: {2} {0}: EVM based {1}: CN based\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "Set training period: {3} {num of watchdog} (How much watchdog to do one ant-div)\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "Set training state number: {4} {num}\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "Set training interval: {5} {ms} (how long for one training state)\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "Set training delay: {6} {ms} (how long to wait RA stable)\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "Set decision threshold: {7} {TP high th.} {TP low th.} {EVM th.} {RSSI th.}\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "TP lower bound: {8} {th.}\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "TP decision method: {9} {0}: Max cnt domination {1}: highest rate domination {2}: Avg. TP domination\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "EVM decision method: {10} {0}:Linear avg. {1}: dB avg.\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "Set notrain number: {11} {num}\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "Set fix_ant_rssi_diff_th: {12} {th}\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "Show all parameter: {100}\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "[ONLY DEBUG] {101} 0x586c[16]={0,1}\n");
	} else {
		HALBB_SCAN(input[1], DCMD_DECIMAL, &var[0]);

		if(var[0] == 0) {
			HALBB_SCAN(input[2], DCMD_DECIMAL, &var[1]);
			halbb_antdiv_init(bb);
			BB_DBG_CNSL(out_len, used, output + used, out_len - used, "Init antdiv !\n");
		} else if (var[0] == 1) {
			HALBB_SCAN(input[2], DCMD_DECIMAL, &var[1]);
			bb_ant_div->antdiv_mode = (u8)var[1];
			BB_DBG_CNSL(out_len, used, output + used, out_len - used,
				 "Ant-Div Mode=%d\n", bb_ant_div->antdiv_mode);
			if (bb_ant_div->antdiv_mode == FIX_MAIN_ANT) {
				halbb_antdiv_set_ant(bb, MAIN_ANT);
				bb_ant_div->pre_target_ant = MAIN_ANT;
			} else if (bb_ant_div->antdiv_mode == FIX_AUX_ANT) {
				halbb_antdiv_set_ant(bb, AUX_ANT);
				bb_ant_div->pre_target_ant = AUX_ANT;
			} else if (bb_ant_div->antdiv_mode == AUTO_ANT)
				halbb_antdiv_set_ant(bb, bb_ant_div->pre_target_ant);
		} else if (var[0] == 2) {
			HALBB_SCAN(input[2], DCMD_DECIMAL, &var[1]);
			bb_ant_div->antdiv_method = var[1];
			BB_DBG_CNSL(out_len, used, output + used, out_len - used,
				 "Ant-Div training method=%d\n", bb_ant_div->antdiv_method);
		} else if (var[0] == 3) {
			HALBB_SCAN(input[2], DCMD_DECIMAL, &var[1]);
			bb_ant_div->antdiv_period = (u8)var[1];
			BB_DBG_CNSL(out_len, used, output + used, out_len - used,
				 "Ant-Div period=%d watchdog\n", bb_ant_div->antdiv_period);
		} else if (var[0] == 4) {
			HALBB_SCAN(input[2], DCMD_DECIMAL, &var[1]);
			bb_ant_div->antdiv_train_num = var[1];
			BB_DBG_CNSL(out_len, used, output + used, out_len - used,
				 "Ant-Div training state num=%d\n", bb_ant_div->antdiv_train_num);
		} else if (var[0] == 5) {
			HALBB_SCAN(input[2], DCMD_DECIMAL, &var[1]);
			bb_ant_div->antdiv_intvl = var[1];
			BB_DBG_CNSL(out_len, used, output + used, out_len - used,
				 "Ant-Div training interval=%d\n", bb_ant_div->antdiv_intvl);
		} else if (var[0] == 6) {
			HALBB_SCAN(input[2], DCMD_DECIMAL, &var[1]);
			bb_ant_div->antdiv_delay = var[1];
			BB_DBG_CNSL(out_len, used, output + used, out_len - used,
				 "Ant-Div training delay=%d\n", bb_ant_div->antdiv_delay);
		} else if (var[0] == 7) {
			HALBB_SCAN(input[2], DCMD_DECIMAL, &var[1]);
			HALBB_SCAN(input[3], DCMD_DECIMAL, &var[2]);
			HALBB_SCAN(input[4], DCMD_DECIMAL, &var[3]);
			HALBB_SCAN(input[5], DCMD_DECIMAL, &var[4]);
			bb_ant_div->tp_diff_th_high = (u16)var[1];
			bb_ant_div->tp_diff_th_low = (u16)var[2];
			bb_ant_div->evm_diff_th = (u8)var[3];
			bb_ant_div->rssi_diff_th = (u8)var[4];

			BB_DBG_CNSL(out_len, used, output + used, out_len - used,
				 "Decision threshold: {TP high = %d} {TP low = %d} {EVM = %d} {RSSI = %d}\n",
				 bb_ant_div->tp_diff_th_high,
				 bb_ant_div->tp_diff_th_low,
				 bb_ant_div->evm_diff_th,
				 bb_ant_div->rssi_diff_th);
		} else if (var[0] == 8) {
			HALBB_SCAN(input[2], DCMD_DECIMAL, &var[1]);
			bb_ant_div->tp_lb = (u8)var[1];
			BB_DBG_CNSL(out_len, used, output + used, out_len - used,
				 "TP lower bound=%d\n", bb_ant_div->tp_lb);
		} else if (var[0] == 9) {
			HALBB_SCAN(input[2], DCMD_DECIMAL, &var[1]);
			bb_ant_div->tp_decision_method = (u8)var[1];
			BB_DBG(bb, DBG_ANT_DIV, "TP decision method=(( %d ))\n",
						bb_ant_div->tp_decision_method);
		} else if (var[0] == 10) {
			HALBB_SCAN(input[2], DCMD_DECIMAL, &var[1]);
			bb_ant_div->evm_decision_method = (u8)var[1];
			BB_DBG(bb, DBG_ANT_DIV, "EVM decision method=(( %d ))\n",
						bb_ant_div->evm_decision_method);
		} else if (var[0] == 11) {
			HALBB_SCAN(input[2], DCMD_DECIMAL, &var[1]);
			bb_ant_div->antdiv_notrain_num= (u8)var[1];
			BB_DBG_CNSL(out_len, used, output + used, out_len - used,
				 "No train Num =%d\n", bb_ant_div->antdiv_notrain_num);
			bb_ant_div->antdiv_notrain_cnt = bb_ant_div->antdiv_notrain_num;
		} else if (var[0] == 12) {
			HALBB_SCAN(input[2], DCMD_DECIMAL, &var[1]);
			bb_ant_div->fixant_rssi_diff = (u8)var[1];
			BB_DBG_CNSL(out_len, used, output + used, out_len - used,
				 "fixant_rssi_diff_th =%d\n", bb_ant_div->fixant_rssi_diff);
		} else if (var[0] == 100) {
			BB_DBG_CNSL(out_len, used, output + used, out_len - used,
				 "Ant-Div Mode = {%d}\n", bb_ant_div->antdiv_mode);
			BB_DBG_CNSL(out_len, used, output + used, out_len - used,
				 "Ant-Div training method = {%d}\n", bb_ant_div->antdiv_method);
			BB_DBG_CNSL(out_len, used, output + used, out_len - used,
				 "TP decision method = {%d}\n", bb_ant_div->tp_decision_method);
			BB_DBG_CNSL(out_len, used, output + used, out_len - used,
				 "EVM decision method = {%d}\n", bb_ant_div->evm_decision_method);
			BB_DBG_CNSL(out_len, used, output + used, out_len - used,
				 "Ant-Div period = {#%d-watchdog}\n", bb_ant_div->antdiv_period);
			BB_DBG_CNSL(out_len, used, output + used, out_len - used,
				 "Ant-Div training state num = {%d}\n", bb_ant_div->antdiv_train_num);
			BB_DBG_CNSL(out_len, used, output + used, out_len - used,
				 "Ant-Div training interval = {%d}\n", bb_ant_div->antdiv_intvl);
			BB_DBG_CNSL(out_len, used, output + used, out_len - used,
				 "Ant-Div training delay = {%d}\n", bb_ant_div->antdiv_delay);
			BB_DBG_CNSL(out_len, used, output + used, out_len - used,
				 "No traing Num = {%d}\n", bb_ant_div->antdiv_notrain_num);
			BB_DBG_CNSL(out_len, used, output + used, out_len - used,
				 "Fix ant rssi diff th = {%d}\n", bb_ant_div->fixant_rssi_diff);
			BB_DBG_CNSL(out_len, used, output + used, out_len - used,
				 "Decision threshold: {TP high = %d} {TP low = %d} {EVM = %d} {RSSI = %d}\n",
				 bb_ant_div->tp_diff_th_high,
				 bb_ant_div->tp_diff_th_low,
				 bb_ant_div->evm_diff_th,
				 bb_ant_div->rssi_diff_th);
			BB_DBG_CNSL(out_len, used, output + used, out_len - used,
				 "TP lower bound = {%d}\n", bb_ant_div->tp_lb);
		} else if (var[0] == 101) {
			HALBB_SCAN(input[2], DCMD_DECIMAL, &var[1]);
			halbb_set_reg_cmn(bb, cr->path0_r_antsel, BIT(16), var[1], HW_PHY_0);
			BB_DBG_CNSL(out_len, used, output + used, out_len - used,
				 "[ONLY DEBUG] 0x1586c[16] = %d\n", var[1]);

		}
	}

	*_used = used;
	*_out_len = out_len;
}

void halbb_antdiv_io_en(struct bb_info *bb)
{
	halbb_evm_based_antdiv(bb);
}

void halbb_antdiv_callback(void *context)
{
	struct bb_info *bb = (struct bb_info *)context;
	struct halbb_timer_info *timer = &bb->bb_ant_div_i.antdiv_timer_i;

	BB_DBG(bb, DBG_ANT_DIV, "[%s]===>\n", __func__);
	timer->timer_state = BB_TIMER_IDLE;

	if (bb->phl_com->hci_type == RTW_HCI_PCIE)
		halbb_antdiv_io_en(bb);
	else
		rtw_hal_cmd_notify(bb->phl_com, MSG_EVT_NOTIFY_BB, (void *)(&timer->event_idx), bb->bb_phy_idx);
}

void halbb_antdiv_timer_init(struct bb_info *bb)
{
	struct halbb_timer_info *timer = &bb->bb_ant_div_i.antdiv_timer_i;

	BB_DBG(bb, DBG_INIT, "[%s]\n", __func__);

	timer->event_idx = BB_EVENT_TIMER_ANTDIV;
	timer->timer_state = BB_TIMER_IDLE;

	halbb_init_timer(bb, &timer->timer_list, halbb_antdiv_callback, bb, "halbb_antdiv_timer");
}

void halbb_cr_cfg_antdiv_init(struct bb_info *bb)
{
	struct bb_antdiv_cr_info *cr = &bb->bb_ant_div_i.bb_antdiv_cr_i;

	switch (bb->cr_type) {
	#ifdef HALBB_COMPILE_AP_SERIES
	case BB_AP:
		cr->path0_r_ant_train_en = PATH0_R_ANT_TRAIN_EN_A;
		cr->path0_r_ant_train_en_m = PATH0_R_ANT_TRAIN_EN_A_M;
		cr->path0_r_tx_ant_sel = PATH0_R_TX_ANT_SEL_A;
		cr->path0_r_tx_ant_sel_m = PATH0_R_TX_ANT_SEL_A_M;
		cr->path0_r_rfe_buf_en = PATH0_R_RFE_BUF_EN_A;
		cr->path0_r_rfe_buf_en_m = PATH0_R_RFE_BUF_EN_A_M;
		cr->path0_r_lnaon_agc = PATH0_R_LNAON_AGC_A;
		cr->path0_r_lnaon_agc_m = PATH0_R_LNAON_AGC_A_M;
		cr->path0_r_trsw_bit_bt = PATH0_R_TRSW_BIT_BT_A;
		cr->path0_r_trsw_bit_bt_m = PATH0_R_TRSW_BIT_BT_A_M;
		cr->path0_r_trsw_s = PATH0_R_TRSW_S_A;
		cr->path0_r_trsw_s_m = PATH0_R_TRSW_S_A_M;
		cr->path0_r_trsw_o = PATH0_R_TRSW_O_A;
		cr->path0_r_trsw_o_m = PATH0_R_TRSW_O_A_M;
		cr->path0_r_trswb_o = PATH0_R_TRSWB_O_A;
		cr->path0_r_trswb_o_m = PATH0_R_TRSWB_O_A_M;
		cr->path0_r_bt_force_antidx = PATH0_R_BT_FORCE_ANTIDX_A;
		cr->path0_r_bt_force_antidx_m = PATH0_R_BT_FORCE_ANTIDX_A_M;
		cr->path0_r_bt_force_antidx_en = PATH0_R_BT_FORCE_ANTIDX_EN_A;
		cr->path0_r_bt_force_antidx_en_m = PATH0_R_BT_FORCE_ANTIDX_EN_A_M;
		cr->path0_r_ant_module_rfe_opt = PATH0_R_ANT_MODULE_RFE_OPT_A;
		cr->path0_r_ant_module_rfe_opt_m = PATH0_R_ANT_MODULE_RFE_OPT_A_M;
		cr->path0_r_rfsw_tr = PATH0_R_RFSW_TR_A;
		cr->path0_r_rfsw_tr_m = PATH0_R_RFSW_TR_A_M;
		cr->path0_r_antsel = PATH0_R_ANTSEL_A;
		cr->path0_r_antsel_m = PATH0_R_ANTSEL_A_M;
		cr->path0_r_rfsw_ant_31_0 = PATH0_R_RFSW_ANT_31_0__A;
		cr->path0_r_rfsw_ant_31_0_m = PATH0_R_RFSW_ANT_31_0__A_M;
		cr->path0_r_rfsw_ant_63_32 = PATH0_R_RFSW_ANT_63_32__A;
		cr->path0_r_rfsw_ant_63_32_m = PATH0_R_RFSW_ANT_63_32__A_M;
		cr->path0_r_rfsw_ant_95_64 = PATH0_R_RFSW_ANT_95_64__A;
		cr->path0_r_rfsw_ant_95_64_m = PATH0_R_RFSW_ANT_95_64__A_M;
		cr->path0_r_rfsw_ant_127_96 = PATH0_R_RFSW_ANT_127_96__A;
		cr->path0_r_rfsw_ant_127_96_m = PATH0_R_RFSW_ANT_127_96__A_M;
		break;

	#endif
	#ifdef HALBB_COMPILE_CLIENT_SERIES
	case BB_CLIENT:
		cr->path0_r_ant_train_en = PATH0_R_ANT_TRAIN_EN_C;
		cr->path0_r_ant_train_en_m = PATH0_R_ANT_TRAIN_EN_C_M;
		cr->path0_r_tx_ant_sel = PATH0_R_TX_ANT_SEL_C;
		cr->path0_r_tx_ant_sel_m = PATH0_R_TX_ANT_SEL_C_M;
		cr->path0_r_rfe_buf_en = PATH0_R_RFE_BUF_EN_C;
		cr->path0_r_rfe_buf_en_m = PATH0_R_RFE_BUF_EN_C_M;
		cr->path0_r_lnaon_agc = PATH0_R_LNAON_AGC_C;
		cr->path0_r_lnaon_agc_m = PATH0_R_LNAON_AGC_C_M;
		cr->path0_r_trsw_bit_bt = PATH0_R_TRSW_BIT_BT_C;
		cr->path0_r_trsw_bit_bt_m = PATH0_R_TRSW_BIT_BT_C_M;
		cr->path0_r_trsw_s = PATH0_R_TRSW_S_C;
		cr->path0_r_trsw_s_m = PATH0_R_TRSW_S_C_M;
		cr->path0_r_trsw_o = PATH0_R_TRSW_O_C;
		cr->path0_r_trsw_o_m = PATH0_R_TRSW_O_C_M;
		cr->path0_r_trswb_o = PATH0_R_TRSWB_O_C;
		cr->path0_r_trswb_o_m = PATH0_R_TRSWB_O_C_M;
		cr->path0_r_bt_force_antidx = PATH0_R_BT_FORCE_ANTIDX_C;
		cr->path0_r_bt_force_antidx_m = PATH0_R_BT_FORCE_ANTIDX_C_M;
		cr->path0_r_bt_force_antidx_en = PATH0_R_BT_FORCE_ANTIDX_EN_C;
		cr->path0_r_bt_force_antidx_en_m = PATH0_R_BT_FORCE_ANTIDX_EN_C_M;
		cr->path0_r_ant_module_rfe_opt = PATH0_R_ANT_MODULE_RFE_OPT_C;
		cr->path0_r_ant_module_rfe_opt_m = PATH0_R_ANT_MODULE_RFE_OPT_C_M;
		cr->path0_r_rfsw_tr = PATH0_R_RFSW_TR_C;
		cr->path0_r_rfsw_tr_m = PATH0_R_RFSW_TR_C_M;
		cr->path0_r_antsel = PATH0_R_ANTSEL_C;
		cr->path0_r_antsel_m = PATH0_R_ANTSEL_C_M;
		cr->path0_r_rfsw_ant_31_0 = PATH0_R_RFSW_ANT_31_0__C;
		cr->path0_r_rfsw_ant_31_0_m = PATH0_R_RFSW_ANT_31_0__C_M;
		cr->path0_r_rfsw_ant_63_32 = PATH0_R_RFSW_ANT_63_32__C;
		cr->path0_r_rfsw_ant_63_32_m = PATH0_R_RFSW_ANT_63_32__C_M;
		cr->path0_r_rfsw_ant_95_64 = PATH0_R_RFSW_ANT_95_64__C;
		cr->path0_r_rfsw_ant_95_64_m = PATH0_R_RFSW_ANT_95_64__C_M;
		cr->path0_r_rfsw_ant_127_96 = PATH0_R_RFSW_ANT_127_96__C;
		cr->path0_r_rfsw_ant_127_96_m = PATH0_R_RFSW_ANT_127_96__C_M;
		break;
	#endif
	#ifdef HALBB_COMPILE_AP2_SERIES
	case BB_AP2:
		cr->path0_r_ant_train_en = PATH0_R_ANT_TRAIN_EN_A2;
		cr->path0_r_ant_train_en_m = PATH0_R_ANT_TRAIN_EN_A2_M;
		cr->path0_r_tx_ant_sel = PATH0_R_TX_ANT_SEL_A2;
		cr->path0_r_tx_ant_sel_m = PATH0_R_TX_ANT_SEL_A2_M;
		cr->path0_r_rfe_buf_en = PATH0_R_RFE_BUF_EN_A2;
		cr->path0_r_rfe_buf_en_m = PATH0_R_RFE_BUF_EN_A2_M;
		cr->path0_r_lnaon_agc = PATH0_R_LNAON_AGC_A2;
		cr->path0_r_lnaon_agc_m = PATH0_R_LNAON_AGC_A2_M;
		cr->path0_r_trsw_bit_bt = PATH0_R_TRSW_BIT_BT_A2;
		cr->path0_r_trsw_bit_bt_m = PATH0_R_TRSW_BIT_BT_A2_M;
		cr->path0_r_trsw_s = PATH0_R_TRSW_S_A2;
		cr->path0_r_trsw_s_m = PATH0_R_TRSW_S_A2_M;
		cr->path0_r_trsw_o = PATH0_R_TRSW_O_A2;
		cr->path0_r_trsw_o_m = PATH0_R_TRSW_O_A2_M;
		cr->path0_r_trswb_o = PATH0_R_TRSWB_O_A2;
		cr->path0_r_trswb_o_m = PATH0_R_TRSWB_O_A2_M;
		cr->path0_r_bt_force_antidx = PATH0_R_BT_FORCE_ANTIDX_A2;
		cr->path0_r_bt_force_antidx_m = PATH0_R_BT_FORCE_ANTIDX_A2_M;
		cr->path0_r_bt_force_antidx_en = PATH0_R_BT_FORCE_ANTIDX_EN_A2;
		cr->path0_r_bt_force_antidx_en_m = PATH0_R_BT_FORCE_ANTIDX_EN_A2_M;
		cr->path0_r_ant_module_rfe_opt = PATH0_R_ANT_MODULE_RFE_OPT_A2;
		cr->path0_r_ant_module_rfe_opt_m = PATH0_R_ANT_MODULE_RFE_OPT_A2_M;
		cr->path0_r_rfsw_tr = PATH0_R_RFSW_TR_A2;
		cr->path0_r_rfsw_tr_m = PATH0_R_RFSW_TR_A2_M;
		cr->path0_r_antsel = PATH0_R_ANTSEL_A2;
		cr->path0_r_antsel_m = PATH0_R_ANTSEL_A2_M;
		cr->path0_r_rfsw_ant_31_0 = PATH0_R_RFSW_ANT_31_0__A2;
		cr->path0_r_rfsw_ant_31_0_m = PATH0_R_RFSW_ANT_31_0__A2_M;
		cr->path0_r_rfsw_ant_63_32 = PATH0_R_RFSW_ANT_63_32__A2;
		cr->path0_r_rfsw_ant_63_32_m = PATH0_R_RFSW_ANT_63_32__A2_M;
		cr->path0_r_rfsw_ant_95_64 = PATH0_R_RFSW_ANT_95_64__A2;
		cr->path0_r_rfsw_ant_95_64_m = PATH0_R_RFSW_ANT_95_64__A2_M;
		cr->path0_r_rfsw_ant_127_96 = PATH0_R_RFSW_ANT_127_96__A2;
		cr->path0_r_rfsw_ant_127_96_m = PATH0_R_RFSW_ANT_127_96__A2_M;
		break;

	#endif
	#ifdef HALBB_COMPILE_BE0_SERIES
	case BB_BE0:
		cr->path0_r_ant_train_en = PATH0_R_ANT_TRAIN_EN_BE0;
		cr->path0_r_ant_train_en_m = PATH0_R_ANT_TRAIN_EN_BE0_M;
		cr->path0_r_tx_ant_sel = PATH0_R_TX_ANT_SEL_BE0;
		cr->path0_r_tx_ant_sel_m = PATH0_R_TX_ANT_SEL_BE0_M;
		cr->path0_r_rfe_buf_en = PATH0_R_RFE_BUF_EN_BE0;
		cr->path0_r_rfe_buf_en_m = PATH0_R_RFE_BUF_EN_BE0_M;
		cr->path0_r_lnaon_agc = PATH0_R_LNAON_AGC_BE0;
		cr->path0_r_lnaon_agc_m = PATH0_R_LNAON_AGC_BE0_M;
		cr->path0_r_trsw_bit_bt = PATH0_R_TRSW_BIT_BT_BE0;
		cr->path0_r_trsw_bit_bt_m = PATH0_R_TRSW_BIT_BT_BE0_M;
		cr->path0_r_trsw_s = PATH0_R_TRSW_S_BE0;
		cr->path0_r_trsw_s_m = PATH0_R_TRSW_S_BE0_M;
		cr->path0_r_trsw_o = PATH0_R_TRSW_O_BE0;
		cr->path0_r_trsw_o_m = PATH0_R_TRSW_O_BE0_M;
		cr->path0_r_trswb_o = PATH0_R_TRSWB_O_BE0;
		cr->path0_r_trswb_o_m = PATH0_R_TRSWB_O_BE0_M;
		cr->path0_r_bt_force_antidx = PATH0_R_BT_FORCE_ANTIDX_BE0;
		cr->path0_r_bt_force_antidx_m = PATH0_R_BT_FORCE_ANTIDX_BE0_M;
		cr->path0_r_bt_force_antidx_en = PATH0_R_BT_FORCE_ANTIDX_EN_BE0;
		cr->path0_r_bt_force_antidx_en_m = PATH0_R_BT_FORCE_ANTIDX_EN_BE0_M;
		cr->path0_r_ant_module_rfe_opt = PATH0_R_ANT_MODULE_RFE_OPT_BE0;
		cr->path0_r_ant_module_rfe_opt_m = PATH0_R_ANT_MODULE_RFE_OPT_BE0_M;
		cr->path0_r_rfsw_tr = PATH0_R_RFSW_TR_BE0;
		cr->path0_r_rfsw_tr_m = PATH0_R_RFSW_TR_BE0_M;
		cr->path0_r_antsel = PATH0_R_ANTSEL_BE0;
		cr->path0_r_antsel_m = PATH0_R_ANTSEL_BE0_M;
		cr->path0_r_rfsw_ant_31_0 = PATH0_R_RFSW_ANT_31_0__BE0;
		cr->path0_r_rfsw_ant_31_0_m = PATH0_R_RFSW_ANT_31_0__BE0_M;
		cr->path0_r_rfsw_ant_63_32 = PATH0_R_RFSW_ANT_63_32__BE0;
		cr->path0_r_rfsw_ant_63_32_m = PATH0_R_RFSW_ANT_63_32__BE0_M;
		cr->path0_r_rfsw_ant_95_64 = PATH0_R_RFSW_ANT_95_64__BE0;
		cr->path0_r_rfsw_ant_95_64_m = PATH0_R_RFSW_ANT_95_64__BE0_M;
		cr->path0_r_rfsw_ant_127_96 = PATH0_R_RFSW_ANT_127_96__BE0;
		cr->path0_r_rfsw_ant_127_96_m = PATH0_R_RFSW_ANT_127_96__BE0_M;
		break;

	#endif


	default:
		BB_WARNING("[%s] BBCR Hook FAIL!\n", __func__);
		if (bb->bb_dbg_i.cr_fake_init_hook_en) {
			BB_TRACE("[%s] BBCR fake init\n", __func__);
			halbb_cr_hook_fake_init(bb, (u32 *)cr, (sizeof(struct bb_antdiv_cr_info) >> 2));
		}
		break;
	}

	if (bb->bb_dbg_i.cr_init_hook_recorder_en) {
		BB_TRACE("[%s] BBCR Hook dump\n", __func__);
		halbb_cr_hook_init_dump(bb, (u32 *)cr, (sizeof(struct bb_antdiv_cr_info) >> 2));
	}
}


#endif

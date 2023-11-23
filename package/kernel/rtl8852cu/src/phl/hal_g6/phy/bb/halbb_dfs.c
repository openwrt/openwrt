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

#ifdef HALBB_DFS_SUPPORT
void halbb_dfs(struct bb_info *bb)
{
	struct bb_dfs_info *bb_dfs = &bb->bb_dfs_i;

	if (!(bb->support_ability & BB_DFS))
		return;

	if (bb_dfs->dfs_dyn_setting_en)
		halbb_dfs_dyn_setting(bb);
}

void halbb_dfs_init(struct bb_info *bb)
{
	struct bb_dfs_info *bb_dfs = &bb->bb_dfs_i;
	struct bb_dfs_cr_info* cr = &bb_dfs->bb_dfs_cr_i;

	BB_DBG(bb, DBG_DFS, "[%s]===>\n", __func__);

	/*DFS Parameter Initialization*/
	bb_dfs->dfs_rgn_domain = bb->phl_com->dfs_info.region_domain;
	halbb_dfs_rgn_dmn_dflt_cnfg(bb);
	bb_dfs->chrp_obsrv_flag = false;
	bb_dfs->dfs_sw_trgr_mode = false;
	bb_dfs->dfs_dbg_mode = false;
	bb_dfs->dfs_dyn_setting_en = true;
	bb_dfs->dbg_dyn_prnt_en = false;
	bb_dfs->dbg_hwdet_prnt_en = false;
	bb_dfs->dbg_swdet_prnt_en = false;
	bb_dfs->dbg_trivil_prnt_en = false;
	bb_dfs->dbg_brk_prnt_en = false;

	bb_dfs->is_mic_w53 = false;
	bb_dfs->is_mic_w56 = false;
	bb_dfs->ppb_prcnt = DFS_PPB_IDLE_PRCNT;
	bb_dfs->pw_factor = PW_FTR_IDLE;
	bb_dfs->pri_factor = PRI_FTR_IDLE;
	bb_dfs->fk_dfs_num_th = 5;
	bb_dfs->dfs_tp_th = 2;
	bb_dfs->dfs_idle_prd_th = 35;

	bb_dfs->dfs_fa_th = 20;
	bb_dfs->dfs_nhm_th = 1;
	bb_dfs->dfs_n_cnfd_lvl_th = 255;

	bb_dfs->dfs_dyn_aci_en = true;
	bb_dfs->dfs_aci_adaptv_th0 = 3;
	bb_dfs->dfs_aci_adaptv_th1 = 0;

	bb_dfs->ACI2SIG_db = -128;
	bb_dfs->dfs_aci_is_read = true;
	bb_dfs->no_aci_rpt_cnt = 0;
	bb_dfs->no_aci_rpt_th = 5;

	bb_dfs->In_CAC_Flag = false;
	bb_dfs->first_dyn_set_flag = true;
	bb_dfs->dyn_reset_flag = true;

	bb_dfs->pw_diff_th = 5;
	bb_dfs->pri_diff_th = 10;
	bb_dfs->pw_lng_chrp_diff_th = 20;
	bb_dfs->lng_rdr_cnt = 0;
	bb_dfs->lng_rdr_cnt_sg1 = 0;
	bb_dfs->lng_rdr_cnt_pre = 0;
	bb_dfs->lng_rdr_cnt_sg1_pre = 0;
	bb_dfs->chrp_rdr_cnt = 0;
	bb_dfs->invalid_lng_pulse_th = 4;

	bb_dfs->rpt_rdr_cnt = 0;
	bb_dfs->pri_mask_th = 3;

	bb_dfs->detect_state = DFS_Normal_State;
	bb_dfs->adap_detect_cnt_init = DFS_ADPTV_CNT1;
	bb_dfs->adap_detect_cnt_add = DFS_ADPTV_CNT2;
	bb_dfs->adap_detect_cnt_th = DFS_ADPTV_CNT_TH;
	bb_dfs->adap_detect_cnt = 0;
	bb_dfs->adap_detect_cnt_all = 0;
	bb_dfs->adap_detect_brk_en = false;

	if (bb_dfs->dfs_rgn_domain == 3)
		bb_dfs->pw_max_th = 660;  // ETSI max pw = 30us, 1.2*30/0.2 , but sometimes decrease detection rate
	else
		bb_dfs->pw_max_th = 660;  // lng pulse max pw = 110us

	#ifdef HALBB_TW_DFS_SERIES
		bb_dfs->rpt_sg_history = 0;
		bb_dfs->rpt_sg_history_all = 0;
	#else
		bb_dfs->rpt_sg_history = 1;
		bb_dfs->rpt_sg_history_all = 1;
	#endif

	bb_dfs->dfs_backup_l2h_val = 0;
	bb_dfs->dfs_mask_l2h_val = 0XFA;
}

void halbb_mac_cfg_dfs_rpt(struct bb_info *bb, bool rpt_en)
{
	struct rtw_hal_com_t *hal_com = bb->hal_com;
	struct hal_mac_dfs_rpt_cfg conf = {
		.rpt_en = rpt_en,
		.rpt_num_th = 0, /* 0:29 , 1:61 , 2:93 , 3:125 */
		.rpt_en_to = rpt_en,
		.rpt_to = 3, /* 1:20ms , 2:40ms , 3:80ms */
	};

	if ((bb->ic_type == BB_RTL8852A) ||(bb->ic_type == BB_RTL8852B))
		conf.rpt_num_th = 1; /* MAC_AX_DFS_TH_61 */

	rtw_hal_mac_cfg_dfs_rpt(hal_com, &conf);
}

void halbb_radar_detect_reset(struct bb_info *bb)
{
	struct bb_dfs_info *bb_dfs = &bb->bb_dfs_i;

	struct bb_dfs_cr_info *cr = &bb_dfs->bb_dfs_cr_i;

#ifdef HALBB_DBCC_SUPPORT
	if (bb->hal_com->dbcc_en) {
		if (bb->bb_phy_idx == HW_PHY_0) {
			halbb_mac_cfg_dfs_rpt(bb, false);
			halbb_set_reg_cmn(bb, cr->dfs_en, cr->dfs_en_m, 0, HW_PHY_0);
			halbb_mac_cfg_dfs_rpt(bb, true);
			halbb_set_reg_cmn(bb, cr->dfs_en, cr->dfs_en_m, 1, HW_PHY_0);
		}
		else
		{
			halbb_mac_cfg_dfs_rpt(bb, false);
			halbb_set_reg_cmn(bb, cr->dfs_en_p1, cr->dfs_en_p1_m, 0, HW_PHY_1);
			halbb_mac_cfg_dfs_rpt(bb, true);
			halbb_set_reg_cmn(bb, cr->dfs_en_p1, cr->dfs_en_p1_m, 1, HW_PHY_1);
		}
	}
	else
#endif
	{
		halbb_mac_cfg_dfs_rpt(bb, false);
		halbb_set_reg_cmn(bb, cr->dfs_en, cr->dfs_en_m, 0, HW_PHY_0);
		halbb_mac_cfg_dfs_rpt(bb, true);
		halbb_set_reg_cmn(bb, cr->dfs_en, cr->dfs_en_m, 1, HW_PHY_0);
	}
}

void halbb_radar_detect_disable(struct bb_info *bb)
{
	struct bb_dfs_info *bb_dfs = &bb->bb_dfs_i;
	struct bb_dfs_cr_info *cr = &bb_dfs->bb_dfs_cr_i;

#ifdef HALBB_DBCC_SUPPORT
	if (bb->hal_com->dbcc_en) {
		if (bb->bb_phy_idx == HW_PHY_0) {
			halbb_mac_cfg_dfs_rpt(bb, false);
			halbb_set_reg_cmn(bb, cr->dfs_en, cr->dfs_en_m, 0, HW_PHY_0);
		}
		else
		{
			halbb_mac_cfg_dfs_rpt(bb, false);
			halbb_set_reg_cmn(bb, cr->dfs_en_p1, cr->dfs_en_p1_m, 0, HW_PHY_1);
		}
	}
	else
#endif
	{
		halbb_mac_cfg_dfs_rpt(bb, false);
		halbb_set_reg_cmn(bb, cr->dfs_en, cr->dfs_en_m, 0, HW_PHY_0);
	}
}

void halbb_radar_detect_enable(struct bb_info *bb)
{
	struct bb_dfs_info *bb_dfs = &bb->bb_dfs_i;
	struct bb_dfs_cr_info *cr = &bb_dfs->bb_dfs_cr_i;

#ifdef HALBB_DBCC_SUPPORT
	if (bb->hal_com->dbcc_en) {
		if (bb->bb_phy_idx == HW_PHY_0) {
			halbb_mac_cfg_dfs_rpt(bb, true);
			halbb_set_reg_cmn(bb, cr->dfs_en, cr->dfs_en_m, 1, HW_PHY_0);
		}
		else
		{
			halbb_mac_cfg_dfs_rpt(bb, true);
			halbb_set_reg_cmn(bb, cr->dfs_en_p1, cr->dfs_en_p1_m, 1, HW_PHY_1);
		}
	}
	else
#endif
	{
		halbb_mac_cfg_dfs_rpt(bb, true);
		halbb_set_reg_cmn(bb, cr->dfs_en, cr->dfs_en_m, 1, HW_PHY_0);
	}
}

void halbb_dfs_enable_cac_flag(struct bb_info* bb)
{
	bb->bb_dfs_i.In_CAC_Flag = true;
}

void halbb_dfs_disable_cac_flag(struct bb_info* bb)
{
	bb->bb_dfs_i.In_CAC_Flag = false;
}

void halbb_set_dfs_st_l2h(struct bb_info* bb, bool mask_dfs_en)
{
	struct bb_dfs_info* bb_dfs = &bb->bb_dfs_i;
	struct bb_dfs_cr_info* cr = &bb_dfs->bb_dfs_cr_i;

	if (mask_dfs_en) {
		//backup original L2H val and set L2H to mask val
		bb_dfs->dfs_backup_l2h_val = halbb_get_reg(bb, cr->dfs_l2h_th, cr->dfs_l2h_th_m);
		halbb_set_reg(bb, cr->dfs_l2h_th, cr->dfs_l2h_th_m, bb_dfs->dfs_mask_l2h_val);//cr->dfs_l2h_th is s(8,0)
	}
	else {
		//restore L2H to default val
		if (bb_dfs->dfs_backup_l2h_val != 0) {
			halbb_set_reg(bb, cr->dfs_l2h_th, cr->dfs_l2h_th_m, bb_dfs->dfs_backup_l2h_val);
		}
		else {
			BB_DBG(bb, DBG_DFS, "[%s]===>\n", __func__);
			BB_DBG(bb, DBG_DFS, "Fail to restore L2H to default val due to dfs_backup_l2h_val is 0!\n");
		}
	}
		
}

void halbb_dfs_change_dmn(struct bb_info *bb, u8 ch, u8 bw)
{
	struct bb_dfs_info *bb_dfs = &bb->bb_dfs_i;
	bool is_w53_band = false, is_w56_band = false;

	bb_dfs->dfs_rgn_domain = bb->phl_com->dfs_info.region_domain;
	halbb_dfs_rgn_dmn_dflt_cnfg(bb);
	if ((bw == CHANNEL_WIDTH_160) && (ch >= 36) && (ch <= 48))
		is_w53_band = true;
	else if ((ch >= 52) && (ch <= 64))
		is_w53_band = true;
	else if ((ch >= 100) && (ch <= 144))
		is_w56_band = true;

	if (bb_dfs->dfs_rgn_domain == DFS_REGD_JAP)
		halbb_dfs_rgn_dmn_cnfg_by_ch(bb, is_w53_band, is_w56_band);
}

bool halbb_is_dfs_band(struct bb_info *bb, u8 ch, u8 bw)
{
	struct bb_dfs_info *bb_dfs = &bb->bb_dfs_i;
	bool is_w53_band = false, is_w56_band = false;


	if ((bw == CHANNEL_WIDTH_160) && (ch >= 36) && (ch <= 48))
		is_w53_band = true;
	else if ((ch >= 52) && (ch <= 64))
		is_w53_band = true;
	else if ((ch >= 100) && (ch <= 144))
		is_w56_band = true;

	if (bb_dfs->dfs_rgn_domain == DFS_REGD_JAP)
		halbb_dfs_rgn_dmn_cnfg_by_ch(bb, is_w53_band, is_w56_band);

	if ((is_w53_band) || (is_w56_band))
		return true;
	else
		return false;
}

void halbb_dfs_rgn_dmn_dflt_cnfg(struct bb_info *bb)
{
	struct bb_dfs_info *bb_dfs = &bb->bb_dfs_i;
	u8 i;

	/* PW unit: 200ns ; PRI unit: 25us */
	if (bb_dfs->dfs_rgn_domain == DFS_REGD_FCC) {
		/*Type {1,2,3,4,6,0,L}*/
		u8 pw_min_fcc_tab[DFS_RDR_TYP_NUM] = {5,5,30,55,5,5,250,0};
		u16 pw_max_fcc_tab[DFS_RDR_TYP_NUM] = {5,25,50,100,5,5,500,1000};
		u8 pri_min_fcc_tab[DFS_RDR_TYP_NUM] = {20,6,8,8,13,57,40,0};
		u8 pri_max_fcc_tab[DFS_RDR_TYP_NUM] = {123,10,20,20,14,58,80,0};
		u8 ppb_fcc_tab[DFS_RDR_TYP_NUM] = {18,23,16,12,15,18,15,255};

		bb_dfs->l_rdr_exst_flag = true;
		for (i = 0; i < DFS_RDR_TYP_NUM ; i++) {
			bb_dfs->pw_min_tab[i] = pw_min_fcc_tab[i];
			bb_dfs->pw_max_tab[i] = pw_max_fcc_tab[i];
			bb_dfs->pri_min_tab[i] = pri_min_fcc_tab[i];
			bb_dfs->pri_max_tab[i] = pri_max_fcc_tab[i];
			bb_dfs->ppb_tab[i] = ppb_fcc_tab[i];
		}
	}
	if (bb_dfs->dfs_rgn_domain == DFS_REGD_ETSI) {
		/*Type {1,2,3,4,5,6,R,CN_R}*/
		/*reduce ppb of Type1 from 10 to 9 in order to increase detection rate*/
		/*increase type3 pw max to 30us due to SRRC spec*/
		u8 pw_min_etsi_tab[DFS_RDR_TYP_NUM] = {2,2,2,100,2,2,5,5};
		u16 pw_max_etsi_tab[DFS_RDR_TYP_NUM] = {25,75,150,150,10,10,5,5};
		u8 pri_min_etsi_tab[DFS_RDR_TYP_NUM] = {40,25,10,10,100,33,57,40};
		u8 pri_max_etsi_tab[DFS_RDR_TYP_NUM] = {200,200,18,20,134,100,58,40};
		u8 ppb_etsi_tab[DFS_RDR_TYP_NUM] = {9,15,25,20,20,30,18,18};
		/*
		etsi 302 Type {1,2,3,4,5,X,L}
		u8 pw_min_etsi2_tab[DFS_RDR_TYP_NUM] =  {5,5,50,5,5,0,100,0};
		u16 pw_max_etsi2_tab[DFS_RDR_TYP_NUM] = {5,25,75,75,75,1000,150,1000};
		u8 pri_min_etsi2_tab[DFS_RDR_TYP_NUM] = {53,40,40,25,10,0,10,0};
		u8 pri_max_etsi2_tab[DFS_RDR_TYP_NUM] = {54,200,200,34,18,0,20,0};
		u8 ppb_etsi2_tab[DFS_RDR_TYP_NUM] = {15,10,15,15,25,255,20,255};
		*/
		bb_dfs->l_rdr_exst_flag = false;
		for (i = 0; i < DFS_RDR_TYP_NUM ; i++) {
			bb_dfs->pw_min_tab[i] = pw_min_etsi_tab[i];
			bb_dfs->pw_max_tab[i] = pw_max_etsi_tab[i];
			bb_dfs->pri_min_tab[i] = pri_min_etsi_tab[i];
			bb_dfs->pri_max_tab[i] = pri_max_etsi_tab[i];
			bb_dfs->ppb_tab[i] = ppb_etsi_tab[i];
		}
	}
	if (bb_dfs->dfs_rgn_domain == DFS_REGD_KCC) {
		/* reduce ppb of Type 3 from 70 to 20 due to buffer size */
		u8 pw_min_kcc_tab[DFS_RDR_TYP_NUM] = { 5,5,2,0,0,0,0,0 };
		u16 pw_max_kcc_tab[DFS_RDR_TYP_NUM] = { 5,5,75,1000,1000,1000,1000,1000 };
		u8 pri_min_kcc_tab[DFS_RDR_TYP_NUM] = { 57,22,121,0,0,0,0,0 };
		u8 pri_max_kcc_tab[DFS_RDR_TYP_NUM] = { 58,23,122,0,0,0,0,0 };
		u8 ppb_kcc_tab[DFS_RDR_TYP_NUM] = { 18,10,20,255,255,255,255,255 };

		bb_dfs->l_rdr_exst_flag = false;
		for (i = 0; i < DFS_RDR_TYP_NUM; i++) {
			bb_dfs->pw_min_tab[i] = pw_min_kcc_tab[i];
			bb_dfs->pw_max_tab[i] = pw_max_kcc_tab[i];
			bb_dfs->pri_min_tab[i] = pri_min_kcc_tab[i];
			bb_dfs->pri_max_tab[i] = pri_max_kcc_tab[i];
			bb_dfs->ppb_tab[i] = ppb_kcc_tab[i];
		}
	}
}

void halbb_dfs_rgn_dmn_cnfg_by_ch(struct bb_info *bb, bool w53_band,
				  bool w56_band)
{
	struct bb_dfs_info *bb_dfs = &bb->bb_dfs_i;
	u8 i;

	/* PW unit: 200ns ; PRI unit: 25us */
	/*Type {1,2,3,4,5,6,7,8}*/
	/*Type3.4 T1 = 70~120us, Type5.6.7.8 T1 =50~240us*/
	/*Type3.4 W2 = 20~110us, Type5.6.7.8 W2 = 30~32us*/
	/*pri_min = pri_min -( max(T1)+max(W2) ), pri_max = pri_max - ( min(T1)+min(W2) )*/
	u8 pw_min_mic_w53_tab[DFS_RDR_TYP_NUM] = {2,2,2,2,2,2,2,2};
	u16 pw_max_mic_w53_tab[DFS_RDR_TYP_NUM] = {25,75,25,75,8,8,8,8};
	u8 pri_min_mic_w53_tab[DFS_RDR_TYP_NUM] = {40,25,30,15,24,32,34,43};
	u8 pri_max_mic_w53_tab[DFS_RDR_TYP_NUM] = {200,200,196,196,32,39,41,51};
	u8 ppb_mic_w53_tab[DFS_RDR_TYP_NUM] = {10,15,22,22,30,25,24,20};

	/*Type {1,2,3,4,5,6,L,8}*/
	u8 pw_min_mic_w56_tab[DFS_RDR_TYP_NUM] = {2,5,10,5,30,55,250,5};
	u16 pw_max_mic_w56_tab[DFS_RDR_TYP_NUM] = {3,5,10,25,50,100,500,5};
	u8 pri_min_mic_w56_tab[DFS_RDR_TYP_NUM] = {55,57,160,6,8,8,40,13};
	u8 pri_max_mic_w56_tab[DFS_RDR_TYP_NUM] = {56,58,160,10,20,20,80,14};
	u8 ppb_mic_w56_tab[DFS_RDR_TYP_NUM] = {18,18,18,23,16,12,15,18};

	if (bb_dfs->dfs_rgn_domain == DFS_REGD_JAP) {
		if (w53_band) {
			bb_dfs->is_mic_w53 = true;
			bb_dfs->is_mic_w56 = false;
			bb_dfs->l_rdr_exst_flag = false;
			for (i = 0; i < DFS_RDR_TYP_NUM ; i++) {
				bb_dfs->pw_min_tab[i] = pw_min_mic_w53_tab[i];
				bb_dfs->pw_max_tab[i] = pw_max_mic_w53_tab[i];
				bb_dfs->pri_min_tab[i] = pri_min_mic_w53_tab[i];
				bb_dfs->pri_max_tab[i] = pri_max_mic_w53_tab[i];
				bb_dfs->ppb_tab[i] = ppb_mic_w53_tab[i];
			}
		} else if (w56_band) {
			bb_dfs->is_mic_w53 = false;
			bb_dfs->is_mic_w56 = true;
			bb_dfs->l_rdr_exst_flag = true;
			for (i = 0; i < DFS_RDR_TYP_NUM ; i++) {
				bb_dfs->pw_min_tab[i] = pw_min_mic_w56_tab[i];
				bb_dfs->pw_max_tab[i] = pw_max_mic_w56_tab[i];
				bb_dfs->pri_min_tab[i] = pri_min_mic_w56_tab[i];
				bb_dfs->pri_max_tab[i] = pri_max_mic_w56_tab[i];
				bb_dfs->ppb_tab[i] = ppb_mic_w56_tab[i];
			}
		}
	}
}

void halbb_radar_chrp_mntr(struct bb_info *bb, bool chrp_flag, bool is_sg1)
{
	struct bb_dfs_info *bb_dfs = &bb->bb_dfs_i;
	u8 i = 0;

	if (is_sg1) {// Seg1 of TW DFS
		if (bb->bb_sys_up_time - bb_dfs->chrp_srt_t_sg1 >= DFS_FCC_LP_LNGTH) {
			bb_dfs->chrp_obsrv_flag_sg1 = false;
			bb_dfs->chrp_srt_t_sg1 = 0;
			//bb_dfs->chrp_cnt = 0;
			bb_dfs->lng_rdr_cnt_sg1 = 0;
		}
		if ((chrp_flag) && !(bb_dfs->chrp_obsrv_flag_sg1)) {
			bb_dfs->chrp_srt_t_sg1 = bb->bb_sys_up_time;
			bb_dfs->chrp_obsrv_flag_sg1 = true;
		}
	} else {
		if (bb->bb_sys_up_time - bb_dfs->chrp_srt_t >= DFS_FCC_LP_LNGTH) {
			bb_dfs->chrp_obsrv_flag = false;
			bb_dfs->chrp_srt_t = 0;
			//bb_dfs->chrp_cnt = 0;
			bb_dfs->lng_rdr_cnt = 0;
		}
		if ((chrp_flag) && !(bb_dfs->chrp_obsrv_flag)) {
			bb_dfs->chrp_srt_t = bb->bb_sys_up_time;
			bb_dfs->chrp_obsrv_flag = true;
		}
	}
}

void halbb_radar_seq_inspctn(struct bb_info *bb, u16 dfs_rpt_idx,
				u8 c_num, u8 p_num, bool is_sg1, u8 c_seg, u8 p_seg)
{
	struct bb_dfs_info *bb_dfs = &bb->bb_dfs_i;

#ifdef HALBB_TW_DFS_SERIES

	if (dfs_rpt_idx != 0 && ( bb_dfs->rpt_sg_history_all & BIT(is_sg1) )) {
		if (p_num == DFS_MAX_SEQ_NUM) {
			if (c_num != 0 && c_seg == p_seg) {
				if (is_sg1)
					bb_dfs->n_seq_flag_sg1 = true;
				else
					bb_dfs->n_seq_flag = true;
			}
		} else {
			if (ABS_8(c_num - p_num) > 1 && c_seg == p_seg) {
				if (is_sg1)
					bb_dfs->n_seq_flag_sg1 = true;
				else
					bb_dfs->n_seq_flag = true;
			}
		}
	} else {
		bb_dfs->rpt_sg_history_all = bb_dfs->rpt_sg_history_all | BIT(is_sg1);
	}
	if (is_sg1)
		bb_dfs->lst_seq_num_sg1 = c_num;
	else
		bb_dfs->lst_seq_num = c_num;

	bb_dfs->lst_seg_idx = c_seg;
#else

	if (dfs_rpt_idx != 0) {
		if (p_num == DFS_MAX_SEQ_NUM) {
			if (c_num != 0)
				bb_dfs->n_seq_flag = true;
		} else {
			if (ABS_8(c_num - p_num) > 1)
				bb_dfs->n_seq_flag = true;
		}
	}

	bb_dfs->lst_seq_num = c_num;
	bb_dfs->lst_seg_idx = c_seg;
#endif
}

void halbb_radar_ptrn_cmprn(struct bb_info *bb, u16 dfs_rpt_idx,
				u8 pri, u16 pw, bool chrp_flag, bool is_sg1)
{
	struct bb_dfs_info *bb_dfs = &bb->bb_dfs_i;
	u8 j = 0, pw_lbd = 0, pri_lbd = 0, pri_ubd = 0;
	u16 pw_ubd = 0;

	bb_dfs->rpt_sg_history = bb_dfs->rpt_sg_history | BIT(is_sg1);

	if (bb_dfs->l_rdr_exst_flag)
		halbb_radar_chrp_mntr(bb, chrp_flag, is_sg1);

	/*record the min.max pw of chirp pulse and chirp pulse cnt*/
	if (chrp_flag) {
		if (is_sg1) {
			bb_dfs->chrp_rdr_cnt_sg1++;
			if (pw > bb_dfs->max_pw_chrp_sg1)
				bb_dfs->max_pw_chrp_sg1 = pw;
			if (pw < bb_dfs->min_pw_chrp_sg1)
				bb_dfs->min_pw_chrp_sg1 = pw;
		}
		else {
			bb_dfs->chrp_rdr_cnt++;
			if (pw > bb_dfs->max_pw_chrp)
				bb_dfs->max_pw_chrp = pw;
			if (pw < bb_dfs->min_pw_chrp)
				bb_dfs->min_pw_chrp = pw;
		}
	}

	for (j = 0; j < DFS_RDR_TYP_NUM ; j++) {
		bb_dfs->ppb_typ_th[j] = ((bb_dfs->ppb_tab[j] * bb_dfs->ppb_prcnt) >> 3);
		pw_lbd = MIN_2(bb_dfs->pw_min_tab[j] - 1, (bb_dfs->pw_min_tab[j] * (8 - bb_dfs->pw_factor) >> 3));
	//	pw_lbd = (pw_lbd <= 1) ? 2 : pw_lbd;
		pw_ubd = MAX_2(bb_dfs->pw_max_tab[j] + 1, (bb_dfs->pw_max_tab[j] * (8 + bb_dfs->pw_factor) >> 3));
		pri_lbd = (bb_dfs->pri_min_tab[j] * (8 - bb_dfs->pri_factor) >> 3);
		if ((bb_dfs->pri_max_tab[j] * (8 + bb_dfs->pri_factor) >> 3) <= 0xDC)
			pri_ubd = (bb_dfs->pri_max_tab[j] * (8 + bb_dfs->pri_factor) >> 3);
		else
			pri_ubd = 0xDC;

		if ((j == DFS_L_RDR_IDX) && (bb_dfs->l_rdr_exst_flag)) {
			if (dfs_rpt_idx == 0 || !(bb_dfs->rpt_sg_history & BIT(is_sg1))) {
				if ((pw_lbd <= pw) && (pw_ubd >= pw) && chrp_flag) {  // first rpt doesn't consider pri because it's not real
					if (is_sg1) {
						bb_dfs->lng_rdr_cnt_sg1++;
						if (pw > bb_dfs->max_pw_lng_sg1)
							bb_dfs->max_pw_lng_sg1 = pw;
						if (pw < bb_dfs->min_pw_lng_sg1)
							bb_dfs->min_pw_lng_sg1 = pw;
					}
					else {
						bb_dfs->lng_rdr_cnt++;
						if (pw > bb_dfs->max_pw_lng)
							bb_dfs->max_pw_lng = pw;
						if (pw < bb_dfs->min_pw_lng)
							bb_dfs->min_pw_lng = pw;
					}
				}
			}
			else {
				if ((pw_lbd <= pw) && (pw_ubd >= pw) && (pri_lbd <= pri) && (pri_ubd >= pri) && chrp_flag) {
					if (is_sg1) {
						bb_dfs->lng_rdr_cnt_sg1++;
						if (pw > bb_dfs->max_pw_lng_sg1)
							bb_dfs->max_pw_lng_sg1 = pw;
						if (pw < bb_dfs->min_pw_lng_sg1)
							bb_dfs->min_pw_lng_sg1 = pw;
					}
					else {
						bb_dfs->lng_rdr_cnt++;
						if (pw > bb_dfs->max_pw_lng)
							bb_dfs->max_pw_lng = pw;
						if (pw < bb_dfs->min_pw_lng)
							bb_dfs->min_pw_lng = pw;
					}
				}
			}
		}
		else {
			if ((bb_dfs->dfs_rgn_domain == DFS_REGD_ETSI) && (j == DFS_SPCL_RDR_IDX_ETSI)) {  // ETSI Type4 is chirp
				if ((pw_lbd <= pw) && (pw_ubd >= pw) && (pri_lbd <= pri) && (pri_ubd >= pri) && chrp_flag) {
					if (is_sg1) {
						bb_dfs->srt_rdr_cnt_sg1[j]++;
						if (pw > bb_dfs->max_pw_shrt_sg1[j])
							bb_dfs->max_pw_shrt_sg1[j] = pw;
						if (pw < bb_dfs->min_pw_shrt_sg1[j])
							bb_dfs->min_pw_shrt_sg1[j] = pw;
						if (!(dfs_rpt_idx == 0) && (bb_dfs->rpt_sg_history & BIT(is_sg1))) {
							if (pri > bb_dfs->max_pri_shrt_sg1[j])
								bb_dfs->max_pri_shrt_sg1[j] = pri;
							if (pri < bb_dfs->min_pri_shrt_sg1[j])
								bb_dfs->min_pri_shrt_sg1[j] = pri;
						}
					}
					else {
						bb_dfs->srt_rdr_cnt[j]++;
						if (pw > bb_dfs->max_pw_shrt[j])
							bb_dfs->max_pw_shrt[j] = pw;
						if (pw < bb_dfs->min_pw_shrt[j])
							bb_dfs->min_pw_shrt[j] = pw;
						if (!(dfs_rpt_idx == 0) && (bb_dfs->rpt_sg_history & BIT(is_sg1))) {
							if (pri > bb_dfs->max_pri_shrt[j])
								bb_dfs->max_pri_shrt[j] = pri;
							if (pri < bb_dfs->min_pri_shrt[j])
								bb_dfs->min_pri_shrt[j] = pri;
						}
					}
				}
			}
			else if ((bb_dfs->dfs_rgn_domain == DFS_REGD_ETSI) && (j == 4 || j==5)) { // ETSI TYPE5.6 multi pri
				if ((pw_lbd <= pw) && (pw_ubd >= pw) && (pri_lbd <= pri) && (pri_ubd >= pri) && !(chrp_flag)) {
					if (is_sg1) {
						bb_dfs->srt_rdr_cnt_sg1[j]++;
						if (pw > bb_dfs->max_pw_shrt_sg1[j])
							bb_dfs->max_pw_shrt_sg1[j] = pw;
						if (pw < bb_dfs->min_pw_shrt_sg1[j])
							bb_dfs->min_pw_shrt_sg1[j] = pw;
					}
					else {
						bb_dfs->srt_rdr_cnt[j]++;
						if (pw > bb_dfs->max_pw_shrt[j])
							bb_dfs->max_pw_shrt[j] = pw;
						if (pw < bb_dfs->min_pw_shrt[j])
							bb_dfs->min_pw_shrt[j] = pw;
					}
				}
			}
			else if ((pw_lbd <= pw) && (pw_ubd >= pw) && (pri_lbd <= pri) && (pri_ubd >= pri) && !(chrp_flag)) {
				if (is_sg1) {
					bb_dfs->srt_rdr_cnt_sg1[j]++;
					if (pw > bb_dfs->max_pw_shrt_sg1[j])
						bb_dfs->max_pw_shrt_sg1[j] = pw;
					if (pw < bb_dfs->min_pw_shrt_sg1[j])
						bb_dfs->min_pw_shrt_sg1[j] = pw;
					if (!(dfs_rpt_idx == 0) && (bb_dfs->rpt_sg_history & BIT(is_sg1))) {
						if (pri > bb_dfs->max_pri_shrt_sg1[j])
							bb_dfs->max_pri_shrt_sg1[j] = pri;
						if (pri < bb_dfs->min_pri_shrt_sg1[j])
							bb_dfs->min_pri_shrt_sg1[j] = pri;
					}
				}
				else {
					bb_dfs->srt_rdr_cnt[j]++;
					if (pw > bb_dfs->max_pw_shrt[j])
						bb_dfs->max_pw_shrt[j] = pw;
					if (pw < bb_dfs->min_pw_shrt[j])
						bb_dfs->min_pw_shrt[j] = pw;
					if (!(dfs_rpt_idx == 0) && (bb_dfs->rpt_sg_history & BIT(is_sg1))) {
						if (pri > bb_dfs->max_pri_shrt[j])
							bb_dfs->max_pri_shrt[j] = pri;
						if (pri < bb_dfs->min_pri_shrt[j])
							bb_dfs->min_pri_shrt[j] = pri;
					}
				}
			}
		}
		if (dfs_rpt_idx == 0) {
			if (bb_dfs->dbg_hwdet_prnt_en)
				BB_DBG(bb, DBG_DFS, "Type %d: [pw_lbd-pw_ubd], [pri_lbd-pri_ubd], [ppb_thd] = [%d-%d], [%d-%d], [%d]\n",
				(j + 1), pw_lbd, pw_ubd, pri_lbd, pri_ubd, bb_dfs->ppb_typ_th[j]);
		}
		bb_dfs->pw_lbd[j] = pw_lbd;
		bb_dfs->pw_ubd[j] = pw_ubd;
		bb_dfs->pri_lbd[j] = pri_lbd;
		bb_dfs->pri_ubd[j] = pri_ubd;
	}
}

void halbb_radar_info_processing(struct bb_info *bb,
				 struct hal_dfs_rpt *rpt, u16 dfs_rpt_idx)
{
	struct bb_dfs_info *bb_dfs = &bb->bb_dfs_i;
	struct bb_rdr_info *dfs_rdr_info = (struct bb_rdr_info *)rpt->dfs_ptr;
	struct bb_dfs_cr_info* cr = &bb_dfs->bb_dfs_cr_i;

	u8 i, pri = 0, cur_seq_num = 0, pre_seq_num = 0;
	u8 cur_seg_idx = 0, pre_seg_idx = 0;// For TW DFS
	u16 pw = 0;
	bool chrp_flag = false;
	bool is_sg1 = false;

#ifdef HALBB_TW_DFS_SERIES
	if (bb_dfs->is_tw_en) {
		pw = (dfs_rdr_info->rdr_info_sg0_pw_m << 4) | (dfs_rdr_info->rdr_info_sg0_pw_l);
		//Seg0 of TW DFS
		if (pw != 0) {
			cur_seq_num = dfs_rdr_info->rdr_info_sg0_seq;
			pre_seq_num = bb_dfs->lst_seq_num;
			cur_seg_idx = 0;
			pre_seg_idx = bb_dfs->lst_seg_idx;
			pri = (dfs_rdr_info->rdr_info_sg0_pri_m << 4) |	(dfs_rdr_info->rdr_info_sg0_pri_l);
			chrp_flag = dfs_rdr_info->rdr_info_sg0_chirp_flag;
			is_sg1 = false;
		}
		//Seg1 of TW DFS
		else {
			cur_seq_num = dfs_rdr_info->rdr_info_sg1_seq;
			pre_seq_num = bb_dfs->lst_seq_num_sg1;
			cur_seg_idx = 1;
			pre_seg_idx = bb_dfs->lst_seg_idx;
			pw = (dfs_rdr_info->rdr_info_sg1_pw_m << 7) | (dfs_rdr_info->rdr_info_sg1_pw_l);
			pri = (dfs_rdr_info->rdr_info_sg1_pri_m << 7) |	(dfs_rdr_info->rdr_info_sg1_pri_l);
			chrp_flag = dfs_rdr_info->rdr_info_sg1_chirp_flag;
			is_sg1 = true;
		}
	}
	else {
		cur_seq_num = dfs_rdr_info->rdr_info_sg0_seq;
		pre_seq_num = bb_dfs->lst_seq_num;
		pw = (dfs_rdr_info->rdr_info_sg0_pw_m << 4) | (dfs_rdr_info->rdr_info_sg0_pw_l);
		pri = (dfs_rdr_info->rdr_info_sg0_pri_m << 4) | (dfs_rdr_info->rdr_info_sg0_pri_l);
		chrp_flag = dfs_rdr_info->rdr_info_sg0_chirp_flag;
		is_sg1 = false;
	}
#else
	cur_seq_num = dfs_rdr_info->rdr_info_seq;
	pre_seq_num = bb_dfs->lst_seq_num;
	cur_seg_idx = 0;
	pre_seg_idx = 0;
	if (rpt->phy_idx == HW_PHY_0) {
		pw = (dfs_rdr_info->rdr_info_sg0_pw_m << 7) |
		     (dfs_rdr_info->rdr_info_sg0_pw_l);
		pri = (dfs_rdr_info->rdr_info_sg0_pri_m << 7) |
		      (dfs_rdr_info->rdr_info_sg0_pri_l);
		chrp_flag = dfs_rdr_info->rdr_info_sg0_chirp_flag;
	} else if (rpt->phy_idx == HW_PHY_1) {
		pw = (dfs_rdr_info->rdr_info_sg1_pw_m << 4) |
		     (dfs_rdr_info->rdr_info_sg1_pw_l);
		pri = (dfs_rdr_info->rdr_info_sg1_pri_m << 4) |
		      (dfs_rdr_info->rdr_info_sg1_pri_l);
		chrp_flag = dfs_rdr_info->rdr_info_sg1_chirp_flag;
	}
#endif
	rpt->dfs_ptr += DFS_RPT_LENGTH;
	bb_dfs->seg_rpt_all[bb_dfs->rpt_rdr_cnt] = is_sg1;
	bb_dfs->seq_num_rpt_all[bb_dfs->rpt_rdr_cnt] = cur_seq_num;

	halbb_radar_seq_inspctn(bb, dfs_rpt_idx, cur_seq_num, pre_seq_num, is_sg1, cur_seg_idx, pre_seg_idx);

#ifdef HALBB_TW_DFS_SERIES   // 160M ch36~64 seg0 report is non real radar
	if (bb_dfs->is_tw_en && bb_dfs->bypass_seg0) {
		if (!is_sg1)
			return;
	}
#endif

	/* mask pri < th dfs report because it may generate lots of rpts*/
	if (pri >= bb_dfs->pri_mask_th) {
		bb_dfs->pw_rpt[bb_dfs->rpt_rdr_cnt] = pw;
		bb_dfs->pri_rpt[bb_dfs->rpt_rdr_cnt] = pri;
		bb_dfs->chrp_rpt[bb_dfs->rpt_rdr_cnt] = chrp_flag;
		bb_dfs->seg_rpt[bb_dfs->rpt_rdr_cnt] = is_sg1;
		bb_dfs->seq_num_rpt[bb_dfs->rpt_rdr_cnt] = cur_seq_num;
		bb_dfs->rpt_rdr_cnt ++;
	}
}

bool halbb_radar_detect(struct bb_info *bb, struct hal_dfs_rpt *dfs_rpt)
{
	struct bb_dfs_info *bb_dfs = &bb->bb_dfs_i;
	struct bb_link_info *link = &bb->bb_link_i;
	bool rdr_detected = false, rdr_detected_sg1 = false;
	u8 bw = bb->hal_com->band[bb->bb_phy_idx].cur_chandef.bw;
	u8 chan = bb->hal_com->band[bb->bb_phy_idx].cur_chandef.chan;
	u16 i = 0, pw_diff_chrp = 0, pw_diff_lng = 0;
	u16 pw_diff_chrp_sg1 = 0, pw_diff_lng_sg1 = 0, pw_max = 0, pw_max_sg1 = 0;

	bb_dfs->min_pw_lng = 65535;
	bb_dfs->min_pw_chrp = 65535;
	bb_dfs->max_pw_lng = 0;
	bb_dfs->max_pw_chrp = 0;
	bb_dfs->chrp_rdr_cnt = 0;

	bb_dfs->min_pw_lng_sg1 = 65535;
	bb_dfs->min_pw_chrp_sg1 = 65535;
	bb_dfs->max_pw_lng_sg1 = 0;
	bb_dfs->max_pw_chrp_sg1 = 0;
	bb_dfs->chrp_rdr_cnt_sg1 = 0;

	for (i = 0; i < DFS_RDR_TYP_NUM ; i++) {
		bb_dfs->min_pw_shrt[i] = 65535;
		bb_dfs->min_pri_shrt[i] = 65535;

		bb_dfs->max_pw_shrt[i] = 0;
		bb_dfs->max_pri_shrt[i] = 0;

		bb_dfs->pw_diff_shrt[i] = 0;
		bb_dfs->pri_diff_shrt[i] = 0;

		bb_dfs->min_pw_shrt_sg1[i] = 65535;
		bb_dfs->min_pri_shrt_sg1[i] = 65535;

		bb_dfs->max_pw_shrt_sg1[i] = 0;
		bb_dfs->max_pri_shrt_sg1[i] = 0;

		bb_dfs->pw_diff_shrt_sg1[i] = 0;
		bb_dfs->pri_diff_shrt_sg1[i] = 0;
	}

	/* SW Trigger Mode */
	if (bb_dfs->dfs_sw_trgr_mode) {
		rdr_detected = true;
		BB_DBG(bb, DBG_DFS, "[HALBB] Radar SW-Trigger Mode!\n");
		return rdr_detected;
	}
	if (!(bb->support_ability & BB_DFS)) {
		BB_DBG(bb, DBG_DFS, "Not support DFS function!\n");
		return false;
	}

	/* DFS Info Parsing/Processing*/
	for (i = 0; i < (dfs_rpt->dfs_num) ; i++)
		halbb_radar_info_processing(bb, dfs_rpt, i);

	if (!(bb_dfs->l_rdr_exst_flag) || bb_dfs->rpt_rdr_cnt == 0) {
		/* Check Fake DFS rpt */
		if (bb_dfs->rpt_rdr_cnt < bb_dfs->fk_dfs_num_th) {
			if (bb_dfs->dbg_trivil_prnt_en)
				BB_DBG(bb, DBG_DFS, "Non-existent form of DFS!\n");
			goto DETECTING_END;
		}
	}

	if (bb_dfs->dbg_hwdet_prnt_en) {
		BB_DBG(bb, DBG_DFS, "\n");
		BB_DBG(bb, DBG_DFS, "[%s]===>\n", __func__);
		BB_DBG(bb, DBG_DFS, "DFS Region Domain = %d, BW = %d, Channel = %d\n",
		       bb_dfs->dfs_rgn_domain, bw, chan);
		BB_DBG(bb, DBG_DFS, "phy_idx = %d, dfs_num = %d, rpt_num = %d\n",
		       dfs_rpt->phy_idx, dfs_rpt->dfs_num, bb_dfs->rpt_rdr_cnt);
		BB_DBG(bb, DBG_DFS, "is_link = %d, is_CAC = %d, is_idle = %d\n",
		       link->is_linked, bb_dfs->In_CAC_Flag, bb_dfs->idle_flag);
		BB_DBG(bb, DBG_DFS, "pw_factor =  %d, pri_factor = %d, ppb_prcnt = %d\n",
			bb_dfs->pw_factor,bb_dfs->pri_factor,bb_dfs->ppb_prcnt);
	}

       /* DFS radar comparsion*/
	for (i = 0; i < (bb_dfs->rpt_rdr_cnt) ; i++) {
		halbb_radar_ptrn_cmprn(bb, i, bb_dfs->pri_rpt[i], bb_dfs->pw_rpt[i], bb_dfs->chrp_rpt[i], bb_dfs->seg_rpt[i]);
		if (bb_dfs->dbg_hwdet_prnt_en) {
			BB_DBG(bb, DBG_DFS, "DFS_RPT: [pw, pri, c_flag, is_sg1] = [%d, %d, %d, %d]\n",
				bb_dfs->pw_rpt[i], bb_dfs->pri_rpt[i], bb_dfs->chrp_rpt[i], bb_dfs->seg_rpt[i]);
		}
	}

	/* Monitor Time and valid radar counter show*/
	if (bb_dfs->dbg_hwdet_prnt_en) {
		if (bb_dfs->l_rdr_exst_flag) {
			if (bb_dfs->is_tw_en) {
				BB_DBG(bb, DBG_DFS, "[mntr_prd, sys_t, chrp_srt_t]: [%d, %d, %d]\n",
	     	 			(bb->bb_sys_up_time - bb_dfs->chrp_srt_t),
	       			bb->bb_sys_up_time, bb_dfs->chrp_srt_t);
				BB_DBG(bb, DBG_DFS, "[mntr_prd, sys_t, chrp_srt_t_sg1]: [%d, %d, %d]\n",
	     	 			(bb->bb_sys_up_time - bb_dfs->chrp_srt_t_sg1),
	       			bb->bb_sys_up_time, bb_dfs->chrp_srt_t_sg1);
			}
			else
				BB_DBG(bb, DBG_DFS, "[mntr_prd, sys_t, chrp_srt_t]: [%d, %d, %d]\n",
	     	 			(bb->bb_sys_up_time - bb_dfs->chrp_srt_t),
	       			bb->bb_sys_up_time, bb_dfs->chrp_srt_t);
		}
		if (bb_dfs->is_tw_en) {
			BB_DBG(bb, DBG_DFS, "==================seg 0==================\n");
			BB_DBG(bb, DBG_DFS, "lng_rdr_cnt = %d\n", bb_dfs->lng_rdr_cnt);
			BB_DBG(bb, DBG_DFS, "srt_rdr_cnt = [%d, %d, %d, %d, %d, %d, %d, %d]\n",
				bb_dfs->srt_rdr_cnt[0], bb_dfs->srt_rdr_cnt[1],
				bb_dfs->srt_rdr_cnt[2], bb_dfs->srt_rdr_cnt[3],
				bb_dfs->srt_rdr_cnt[4], bb_dfs->srt_rdr_cnt[5],
				bb_dfs->srt_rdr_cnt[6], bb_dfs->srt_rdr_cnt[7]);
			BB_DBG(bb, DBG_DFS, "==================seg 1==================\n");
			BB_DBG(bb, DBG_DFS, "lng_rdr_cnt_sg1 = %d\n", bb_dfs->lng_rdr_cnt_sg1);
			BB_DBG(bb, DBG_DFS, "srt_rdr_cnt_sg1 = [%d, %d, %d, %d, %d, %d, %d, %d]\n",
				bb_dfs->srt_rdr_cnt_sg1[0], bb_dfs->srt_rdr_cnt_sg1[1],
				bb_dfs->srt_rdr_cnt_sg1[2], bb_dfs->srt_rdr_cnt_sg1[3],
				bb_dfs->srt_rdr_cnt_sg1[4], bb_dfs->srt_rdr_cnt_sg1[5],
				bb_dfs->srt_rdr_cnt_sg1[6], bb_dfs->srt_rdr_cnt_sg1[7]);
			BB_DBG(bb, DBG_DFS, "\n");
		}
		else {
			BB_DBG(bb, DBG_DFS, "\n");
			BB_DBG(bb, DBG_DFS, "lng_rdr_cnt = %d\n", bb_dfs->lng_rdr_cnt);
			BB_DBG(bb, DBG_DFS, "srt_rdr_cnt = [%d, %d, %d, %d, %d, %d, %d, %d]\n",
				bb_dfs->srt_rdr_cnt[0], bb_dfs->srt_rdr_cnt[1],
				bb_dfs->srt_rdr_cnt[2], bb_dfs->srt_rdr_cnt[3],
				bb_dfs->srt_rdr_cnt[4], bb_dfs->srt_rdr_cnt[5],
				bb_dfs->srt_rdr_cnt[6], bb_dfs->srt_rdr_cnt[7]);
			BB_DBG(bb, DBG_DFS, "\n");
		}
	}

	/* PW Diff calculation */
	for (i = 0; i < DFS_RDR_TYP_NUM ; i++) {
		bb_dfs->pw_diff_shrt[i] = bb_dfs->max_pw_shrt[i] - bb_dfs->min_pw_shrt[i];
		bb_dfs->pri_diff_shrt[i] = bb_dfs->max_pri_shrt[i] - bb_dfs->min_pri_shrt[i];
		if (bb_dfs->dbg_hwdet_prnt_en)
			BB_DBG(bb, DBG_DFS, "short type %d : [pw_diff,pri_diff] = [%d,%d] \n", (i + 1), bb_dfs->pw_diff_shrt[i], bb_dfs->pri_diff_shrt[i]);
		if (bb_dfs->is_tw_en) {
			bb_dfs->pw_diff_shrt_sg1[i] = bb_dfs->max_pw_shrt_sg1[i] - bb_dfs->min_pw_shrt_sg1[i];
			bb_dfs->pri_diff_shrt_sg1[i] = bb_dfs->max_pri_shrt_sg1[i] - bb_dfs->min_pri_shrt_sg1[i];
			if (bb_dfs->dbg_hwdet_prnt_en)
				BB_DBG(bb, DBG_DFS, "short type %d : [pw_diff_sg1,pri_diff_sg1] = [%d,%d] \n", (i + 1), bb_dfs->pw_diff_shrt_sg1[i], bb_dfs->pri_diff_shrt_sg1[i]);
		}
	}
	pw_diff_chrp = bb_dfs->max_pw_chrp - bb_dfs->min_pw_chrp;
	if (bb_dfs->dbg_hwdet_prnt_en)
		BB_DBG(bb, DBG_DFS, "chrp pulse : [pw_diff] = [%d] \n",pw_diff_chrp);
	if (bb_dfs->is_tw_en) {
		pw_diff_chrp_sg1 = bb_dfs->max_pw_chrp_sg1 - bb_dfs->min_pw_chrp_sg1;
		if (bb_dfs->dbg_hwdet_prnt_en)
			BB_DBG(bb, DBG_DFS, "chrp pulse : [pw_diff_sg1] = [%d] \n",pw_diff_chrp_sg1);
	}

	/*MAX PW*/
	for (i = 0; i < dfs_rpt->dfs_num ; i++) {
		if (bb_dfs->is_tw_en) {
			if (bb_dfs->seg_rpt[i])
				pw_max_sg1 = MAX_2(pw_max_sg1, bb_dfs->pw_rpt[i]);
			else
				pw_max = MAX_2(pw_max, bb_dfs->pw_rpt[i]);
		}
		else {
			pw_max = MAX_2(pw_max, bb_dfs->pw_rpt[i]);
		}
	}
	if (bb_dfs->dbg_hwdet_prnt_en)
		BB_DBG(bb, DBG_DFS, "[pw_max,pw_max_sg1,pw_max_th] = [%d,%d,%d] \n", pw_max, pw_max_sg1, bb_dfs->pw_max_th);

	/*lng pulse cnt reset*/
	if (bb_dfs->l_rdr_exst_flag) {
		pw_diff_lng = bb_dfs->max_pw_lng - bb_dfs->min_pw_lng;
		if (bb_dfs->dbg_hwdet_prnt_en)
			BB_DBG(bb, DBG_DFS, "long pulse : [pw_diff] = [%d] \n",pw_diff_lng);
		if (pw_diff_lng > bb_dfs->pw_lng_chrp_diff_th) {
			bb_dfs->lng_rdr_cnt = 0;          // reset lng_rdr cnt
			if (bb_dfs->dbg_hwdet_prnt_en || bb_dfs->dbg_brk_prnt_en)
				BB_DBG(bb, DBG_DFS, "long type PW Diff BRK, reset lng_rdr cnt!\n");
		}
		if (pw_diff_chrp > bb_dfs->pw_lng_chrp_diff_th) {
			bb_dfs->lng_rdr_cnt = 0;          // reset lng_rdr cnt
			if (bb_dfs->dbg_hwdet_prnt_en || bb_dfs->dbg_brk_prnt_en)
				BB_DBG(bb, DBG_DFS, "chrp type PW Diff BRK, reset lng_rdr cnt!\n");
		}
		if (pw_max > bb_dfs->pw_max_th) {
			bb_dfs->lng_rdr_cnt = 0;          // reset lng_rdr cnt
			if (bb_dfs->dbg_hwdet_prnt_en || bb_dfs->dbg_brk_prnt_en)
				BB_DBG(bb, DBG_DFS, "pw_max BRK, reset lng_rdr cnt!\n");
		}
		if (bb_dfs->chrp_rdr_cnt > bb_dfs->invalid_lng_pulse_th) {   // max PPB=3 in lng rdr (plus one as margin)
			bb_dfs->lng_rdr_cnt = 0;          // reset lng_rdr cnt
			if (bb_dfs->dbg_hwdet_prnt_en || bb_dfs->dbg_brk_prnt_en)
				BB_DBG(bb, DBG_DFS, "Invalid long pulse cnt BRK, reset lng_rdr cnt!\n");
		}
		if (bb_dfs->is_tw_en) {
			pw_diff_lng_sg1 = bb_dfs->max_pw_lng_sg1 - bb_dfs->min_pw_lng_sg1;
			if (bb_dfs->dbg_hwdet_prnt_en)
				BB_DBG(bb, DBG_DFS, "long pulse : [pw_diff_sg1] = [%d] \n",pw_diff_lng_sg1);
			if (pw_diff_lng_sg1 > bb_dfs->pw_lng_chrp_diff_th) {
				bb_dfs->lng_rdr_cnt_sg1 = 0;          // reset lng_rdr cnt
				if (bb_dfs->dbg_hwdet_prnt_en || bb_dfs->dbg_brk_prnt_en)
					BB_DBG(bb, DBG_DFS, "long type PW Diff BRK, reset lng_rdr cnt_sg1!\n");
			}
			if (pw_diff_chrp_sg1 > bb_dfs->pw_lng_chrp_diff_th) {
				bb_dfs->lng_rdr_cnt_sg1 = 0;          // reset lng_rdr cnt
				if (bb_dfs->dbg_hwdet_prnt_en || bb_dfs->dbg_brk_prnt_en)
					BB_DBG(bb, DBG_DFS, "chrp type PW Diff BRK, reset lng_rdr cnt_sg1!\n");
			}
			if (pw_max_sg1 > bb_dfs->pw_max_th) {
				bb_dfs->lng_rdr_cnt_sg1 = 0;          // reset lng_rdr cnt
				if (bb_dfs->dbg_hwdet_prnt_en || bb_dfs->dbg_brk_prnt_en)
					BB_DBG(bb, DBG_DFS, "pw_max_sg1 BRK, reset lng_rdr cnt_sg1!\n");
			}
			if (bb_dfs->chrp_rdr_cnt_sg1 > bb_dfs->invalid_lng_pulse_th) {   // max PPB=3 in lng rdr (plus one as margin)
				bb_dfs->lng_rdr_cnt_sg1 = 0;          // reset lng_rdr cnt
				if (bb_dfs->dbg_hwdet_prnt_en || bb_dfs->dbg_brk_prnt_en)
					BB_DBG(bb, DBG_DFS, "Invalid long pulse cnt BRK, reset lng_rdr cnt_sg1!\n");
			}
		}
	}

	if (bb_dfs->dbg_hwdet_prnt_en) {
		BB_DBG(bb, DBG_DFS, "[pw_diff_th,pw_lng_chrp_diff_th,pri_diff_th] = [%d,%d,%d] \n",
			bb_dfs->pw_diff_th,bb_dfs->pw_lng_chrp_diff_th,bb_dfs->pri_diff_th);
		BB_DBG(bb, DBG_DFS, "[adap_cnt,adap_cnt_all,adap_cnt_th] =[%d,%d,%d]\n",
			bb_dfs->adap_detect_cnt, bb_dfs->adap_detect_cnt_all, bb_dfs->adap_detect_cnt_th);
	}

	/* Check if DFS matching cnts exceed ppb th*/
	for (i = 0; i < DFS_RDR_TYP_NUM ; i++) {
		if ((i == DFS_L_RDR_IDX) && (bb_dfs->l_rdr_exst_flag)) {
			if ((bb_dfs->lng_rdr_cnt >= bb_dfs->ppb_typ_th[i])) {
				rdr_detected = true;
				if (bb_dfs->dbg_hwdet_prnt_en) {
					BB_DBG(bb, DBG_DFS, "Long Rdr Appeared!\n");
					BB_DBG(bb, DBG_DFS, "Long Rdr reaches threshold (ppb_th:%d)!\n",bb_dfs->ppb_typ_th[i]);
				}
			}
		} else {
			if (bb_dfs->srt_rdr_cnt[i] >= bb_dfs->ppb_typ_th[i]) {
				 rdr_detected = true;
				if (bb_dfs->dbg_hwdet_prnt_en) {
					BB_DBG(bb, DBG_DFS, "Rdr Type %d reaches threshold (ppb_th:%d)!\n",
							(i+1), bb_dfs->ppb_typ_th[i]);
				}
				 if (bb_dfs->pw_diff_shrt[i] > bb_dfs->pw_diff_th) {
				 	rdr_detected = false;
					if (bb_dfs->dbg_hwdet_prnt_en || bb_dfs->dbg_brk_prnt_en)
						BB_DBG(bb, DBG_DFS, "Short type %d PW Diff BRK, pw_diff_shrt = %d\n",(i+1),bb_dfs->pw_diff_shrt[i]);
				}
				if (bb_dfs->pri_diff_shrt[i] > bb_dfs->pri_diff_th) {
					rdr_detected = false;
					if (bb_dfs->dbg_hwdet_prnt_en || bb_dfs->dbg_brk_prnt_en)
						BB_DBG(bb, DBG_DFS, "Short type %d PRI Diff BRK, pri_diff_shrt = %d\n",(i+1),bb_dfs->pri_diff_shrt[i]);
				}
			}
		}
	}

	/*Drop the detected RDR to prevent FRD*/
	if (rdr_detected) {
		if (pw_max > bb_dfs->pw_max_th) {
			rdr_detected = false;
			if (bb_dfs->dbg_hwdet_prnt_en || bb_dfs->dbg_brk_prnt_en)
				BB_DBG(bb, DBG_DFS, "RDR drop by PW_MAX BRK !\n");
		}
		if (pw_diff_chrp > bb_dfs->pw_lng_chrp_diff_th) {
			rdr_detected = false;
			if (bb_dfs->dbg_hwdet_prnt_en || bb_dfs->dbg_brk_prnt_en)
				BB_DBG(bb, DBG_DFS, "RDR drop by Chrp PW Diff BRK !\n");
		}
		if ((bb_dfs->adap_detect_cnt_all > bb_dfs->adap_detect_cnt_th) && (bb_dfs->adap_detect_brk_en)) {
			rdr_detected = false;
			if (bb_dfs->dbg_hwdet_prnt_en || bb_dfs->dbg_brk_prnt_en)
				BB_DBG(bb, DBG_DFS, "RDR drop by ENV_2 !\n");
		} else if (bb_dfs->detect_state) {
			rdr_detected = false;
			if (bb_dfs->dbg_hwdet_prnt_en || bb_dfs->dbg_brk_prnt_en)
				BB_DBG(bb, DBG_DFS, "RDR drop by ENV !\n");
		}
		if (bb_dfs->n_seq_flag) {
			rdr_detected = false;
			if (bb_dfs->dbg_hwdet_prnt_en || bb_dfs->dbg_brk_prnt_en) {
				BB_DBG(bb, DBG_DFS, "Non-sequential DFS Dropped!\n");
				for (i = 0; i < (dfs_rpt->dfs_num) ; i++) {
					BB_DBG(bb, DBG_DFS, "[seq_num, is_sg1] = [%d, %d]\n",
						 bb_dfs->seq_num_rpt_all[i], bb_dfs->seg_rpt_all[i]);
				}
			}
		}
		if (bb_dfs->n_cnfd_flag) {
			rdr_detected = false;
			if (bb_dfs->dbg_hwdet_prnt_en || bb_dfs->dbg_brk_prnt_en)
				BB_DBG(bb, DBG_DFS, "Non-confidential DFS Blocked!\n");
		}
	}

	/* Check if DFS matching cnts exceed ppb th for TW DFS*/
	if (bb_dfs->is_tw_en) {
		for (i = 0; i < DFS_RDR_TYP_NUM ; i++) {
			if ((i == DFS_L_RDR_IDX) && (bb_dfs->l_rdr_exst_flag))  {
				if ((bb_dfs->lng_rdr_cnt_sg1 >= bb_dfs->ppb_typ_th[i])) {
					rdr_detected_sg1 = true;
					if (bb_dfs->dbg_hwdet_prnt_en) {
						BB_DBG(bb, DBG_DFS, "seg1: Long Rdr Appeared!\n");
						BB_DBG(bb, DBG_DFS, "seg1: Long Rdr reaches threshold (ppb_th:%d)!\n",bb_dfs->ppb_typ_th[i]);
					}
				}
			} else {
				if (bb_dfs->srt_rdr_cnt_sg1[i] >= bb_dfs->ppb_typ_th[i]) {
					 rdr_detected_sg1 = true;
					if (bb_dfs->dbg_hwdet_prnt_en) {
						BB_DBG(bb, DBG_DFS, "seg1: Rdr Type %d reaches threshold (ppb_th:%d)!\n",
								(i+1), bb_dfs->ppb_typ_th[i]);
					}
					 if (bb_dfs->pw_diff_shrt_sg1[i] > bb_dfs->pw_diff_th) {
					 	rdr_detected_sg1 = false;
						if (bb_dfs->dbg_hwdet_prnt_en || bb_dfs->dbg_brk_prnt_en)
							BB_DBG(bb, DBG_DFS, "seg1: Short type %d PW Diff BRK, pw_diff_shrt_sg1 = %d\n",(i+1),bb_dfs->pw_diff_shrt_sg1[i]);
					}
					if (bb_dfs->pri_diff_shrt_sg1[i] > bb_dfs->pri_diff_th) {
						rdr_detected_sg1 = false;
						if (bb_dfs->dbg_hwdet_prnt_en || bb_dfs->dbg_brk_prnt_en)
							BB_DBG(bb, DBG_DFS, "seg1: Short type %d PRI Diff BRK, pri_diff_shrt_sg1 = %d\n",(i+1),bb_dfs->pri_diff_shrt_sg1[i]);
					}
				}
			}
		}
	}

	/*Drop the detected RDR to prevent FRD for TW DFS*/
	if (bb_dfs->is_tw_en) {
		if (rdr_detected_sg1) {
			if (pw_max_sg1 > bb_dfs->pw_max_th) {
				rdr_detected_sg1 = false;
				if (bb_dfs->dbg_hwdet_prnt_en || bb_dfs->dbg_brk_prnt_en)
					BB_DBG(bb, DBG_DFS, "seg1: RDR drop by PW_MAX BRK !\n");
			}
			if (pw_diff_chrp_sg1 > bb_dfs->pw_lng_chrp_diff_th) {
				rdr_detected_sg1 = false;
				if (bb_dfs->dbg_hwdet_prnt_en || bb_dfs->dbg_brk_prnt_en)
					BB_DBG(bb, DBG_DFS, "seg1: RDR drop by Chrp PW Diff BRK !\n");
			}
			if ((bb_dfs->adap_detect_cnt_all > bb_dfs->adap_detect_cnt_th) && (bb_dfs->adap_detect_brk_en)) {
				rdr_detected_sg1 = false;
				if (bb_dfs->dbg_hwdet_prnt_en || bb_dfs->dbg_brk_prnt_en)
					BB_DBG(bb, DBG_DFS, "seg1: RDR drop by ENV_2 !\n");
			} else if (bb_dfs->detect_state) {
				rdr_detected_sg1 = false;
				if (bb_dfs->dbg_hwdet_prnt_en || bb_dfs->dbg_brk_prnt_en)
					BB_DBG(bb, DBG_DFS, "seg1: RDR drop by ENV !\n");
			}
			if (bb_dfs->n_seq_flag_sg1) {
				rdr_detected_sg1 = false;
				if (bb_dfs->dbg_hwdet_prnt_en || bb_dfs->dbg_brk_prnt_en) {
					BB_DBG(bb, DBG_DFS, "seg1: Non-sequential DFS Dropped!\n");
					for (i = 0; i < (dfs_rpt->dfs_num) ; i++) {
						BB_DBG(bb, DBG_DFS, "[seq_num, is_sg1] = [%d, %d]\n",
							 bb_dfs->seq_num_rpt_all[i], bb_dfs->seg_rpt_all[i]);
					}
				}
			}
			if (bb_dfs->n_cnfd_flag) {
				rdr_detected_sg1= false;
				if (bb_dfs->dbg_hwdet_prnt_en || bb_dfs->dbg_brk_prnt_en)
					BB_DBG(bb, DBG_DFS, "seg1: Non-confidential DFS Blocked!\n");
			}
		}
	}

	if (bb_dfs->dbg_hwdet_prnt_en)
		bb_dfs->dbg_swdet_prnt_en = false;

	/* Debug Mode */
	if (rdr_detected) {
		if (bb_dfs->dbg_swdet_prnt_en) {
			BB_DBG(bb, DBG_DFS, "[%s]===>\n", __func__);
			BB_DBG(bb, DBG_DFS, "phy_idx = %d, dfs_num = %d, rpt_num = %d\n", dfs_rpt->phy_idx, dfs_rpt->dfs_num, bb_dfs->rpt_rdr_cnt);
			BB_DBG(bb, DBG_DFS, "pw_factor =  %d, pri_factor = %d, ppb_prcnt = %d, idle_flag = %d\n",
				bb_dfs->pw_factor,bb_dfs->pri_factor,bb_dfs->ppb_prcnt, bb_dfs->idle_flag);
			for (i = 0; i < DFS_RDR_TYP_NUM ; i++) {
				BB_DBG(bb, DBG_DFS, "Type %d: [pw_lbd-pw_ubd], [pri_lbd-pri_ubd], [ppb_thd] = [%d-%d], [%d-%d], [%d]\n",
				      (i+1), bb_dfs->pw_lbd[i],
				      bb_dfs->pw_ubd[i], bb_dfs->pri_lbd[i],
				      bb_dfs->pri_ubd[i], bb_dfs->ppb_typ_th[i]);
			}
			for (i = 0; i < bb_dfs->rpt_rdr_cnt ; i++) {
				BB_DBG(bb, DBG_DFS, "DFS_RPT %d: [pw, pri, c_flag, is_sg1] = [%d, %d, %d, %d]\n",
			              (i + 1), bb_dfs->pw_rpt[i], bb_dfs->pri_rpt[i],
				       bb_dfs->chrp_rpt[i], bb_dfs->seg_rpt[i]);
			}
			BB_DBG(bb, DBG_DFS, "lng_rdr_cnt = %d\n", bb_dfs->lng_rdr_cnt);
			BB_DBG(bb, DBG_DFS, "srt_rdr_cnt = [%d, %d, %d, %d, %d, %d, %d, %d]\n",
			       bb_dfs->srt_rdr_cnt[0], bb_dfs->srt_rdr_cnt[1],
			       bb_dfs->srt_rdr_cnt[2], bb_dfs->srt_rdr_cnt[3],
			       bb_dfs->srt_rdr_cnt[4], bb_dfs->srt_rdr_cnt[5],
			       bb_dfs->srt_rdr_cnt[6], bb_dfs->srt_rdr_cnt[7]);
			BB_DBG(bb, DBG_DFS, "pw_diff_th = %d, pw_lng_chrp_diff_th=%d, pri_diff_th =%d, pw_max_th =%d\n",
				bb_dfs->pw_diff_th, bb_dfs->pw_lng_chrp_diff_th, bb_dfs->pri_diff_th, bb_dfs->pw_max_th);
			BB_DBG(bb, DBG_DFS, "adap_cnt = %d, adap_cnt_all = %d\n",
				bb_dfs->adap_detect_cnt, bb_dfs->adap_detect_cnt_all);
		}
		/* Reset Long radar Counter */
		bb_dfs->lng_rdr_cnt = 0;
		if (bb_dfs->dfs_dbg_mode) {
			rdr_detected = false;
			BB_DBG(bb, DBG_DFS, "Radar is detected in DFS debug mode!\n");
		}
		else
			BB_DBG(bb, DBG_DFS, "Radar is detected in DFS general mode!\n");
	}

	/* Debug Mode for TW DFS*/
	if (bb_dfs->is_tw_en) {
		if (rdr_detected_sg1) {
			if (bb_dfs->dbg_swdet_prnt_en) {
				BB_DBG(bb, DBG_DFS, "[%s]===>\n", __func__);
				BB_DBG(bb, DBG_DFS, "=================== seg 1 ===================>\n");
				BB_DBG(bb, DBG_DFS, "phy_idx = %d, dfs_num = %d, rpt_num = %d\n", dfs_rpt->phy_idx, dfs_rpt->dfs_num, bb_dfs->rpt_rdr_cnt);
				BB_DBG(bb, DBG_DFS, "pw_factor =  %d, pri_factor = %d, ppb_prcnt = %d, idle_flag = %d\n",
					bb_dfs->pw_factor,bb_dfs->pri_factor,bb_dfs->ppb_prcnt, bb_dfs->idle_flag);
				for (i = 0; i < DFS_RDR_TYP_NUM; i++) {
					BB_DBG(bb, DBG_DFS, "Type %d: [pw_lbd-pw_ubd], [pri_lbd-pri_ubd], [ppb_thd] = [%d-%d], [%d-%d], [%d]\n",
				      (i+1), bb_dfs->pw_lbd[i],
				      bb_dfs->pw_ubd[i], bb_dfs->pri_lbd[i],
				      bb_dfs->pri_ubd[i], bb_dfs->ppb_typ_th[i]);
				}
				for (i = 0; i < bb_dfs->rpt_rdr_cnt; i++) {
					BB_DBG(bb, DBG_DFS, "DFS_RPT %d: [pw, pri, c_flag, is_sg1] = [%d, %d, %d, %d]\n",
						(i + 1), bb_dfs->pw_rpt[i], bb_dfs->pri_rpt[i],
						bb_dfs->chrp_rpt[i], bb_dfs->seg_rpt[i]);
				}
				BB_DBG(bb, DBG_DFS, "lng_rdr_cnt_sg1 = %d\n", bb_dfs->lng_rdr_cnt_sg1);
				BB_DBG(bb, DBG_DFS, "srt_rdr_cnt_sg1 = [%d, %d, %d, %d, %d, %d, %d, %d]\n",
					bb_dfs->srt_rdr_cnt_sg1[0], bb_dfs->srt_rdr_cnt_sg1[1],
					bb_dfs->srt_rdr_cnt_sg1[2], bb_dfs->srt_rdr_cnt_sg1[3],
					bb_dfs->srt_rdr_cnt_sg1[4], bb_dfs->srt_rdr_cnt_sg1[5],
					bb_dfs->srt_rdr_cnt_sg1[6], bb_dfs->srt_rdr_cnt_sg1[7]);
				BB_DBG(bb, DBG_DFS, "pw_diff_th = %d, pw_lng_chrp_diff_th=%d, pri_diff_th =%d, pw_max_th =%d\n",
					bb_dfs->pw_diff_th, bb_dfs->pw_lng_chrp_diff_th, bb_dfs->pri_diff_th, bb_dfs->pw_max_th);
				BB_DBG(bb, DBG_DFS, "adap_cnt = %d, adap_cnt_all = %d\n",
					bb_dfs->adap_detect_cnt, bb_dfs->adap_detect_cnt_all);
			}
			/* Reset Long radar Counter */
			bb_dfs->lng_rdr_cnt_sg1 = 0;
			if (bb_dfs->dfs_dbg_mode) {
				rdr_detected_sg1 = false;
				BB_DBG(bb, DBG_DFS, "seg1:Radar is detected in DFS debug mode!\n");
			}
			else
				BB_DBG(bb, DBG_DFS, "seg1:Radar is detected in DFS general mode!\n");
		}
	}

DETECTING_END:
	/* Reset SW Counter/Flag */
	bb_dfs->n_seq_flag = false;
	bb_dfs->n_seq_flag_sg1 = false;
	bb_dfs->n_cnfd_flag = false;
	bb_dfs->n_cnfd_lvl = 0;
	bb_dfs->rpt_rdr_cnt = 0;
	for (i = 0; i < DFS_RDR_TYP_NUM; i++) {
		bb_dfs->srt_rdr_cnt[i] = 0;
		bb_dfs->srt_rdr_cnt_sg1[i] = 0;
	}
	for (i = 0; i < dfs_rpt->dfs_num ; i++) {
		bb_dfs->pw_rpt[i] = 0;
		bb_dfs->pri_rpt[i] = 0;
		bb_dfs->chrp_rpt[i] = 0;
		bb_dfs->seg_rpt[i] = 0;
		bb_dfs->seg_rpt_all[i] = 0;
		bb_dfs->seq_num_rpt[i] = 0;
		bb_dfs->seq_num_rpt_all[i] = 0;
	}
	//reset rpt sg memory
	#ifdef HALBB_TW_DFS_SERIES
		bb_dfs->rpt_sg_history = 0;
		bb_dfs->rpt_sg_history_all = 0;
	#else
		bb_dfs->rpt_sg_history = 1;
		bb_dfs->rpt_sg_history_all = 1;
	#endif
	//store lng_rdr_cnt for invalid check
	bb_dfs->lng_rdr_cnt_pre = bb_dfs->lng_rdr_cnt;
	bb_dfs->lng_rdr_cnt_sg1_pre = bb_dfs->lng_rdr_cnt_sg1;

	return (rdr_detected || rdr_detected_sg1);
}

void halbb_parsing_aci2sig(struct bb_info* bb, u32 physts_bitmap)
{
	struct bb_physts_info* physts = &bb->bb_physts_i;
	struct bb_physts_rslt_24_info *psts_24 = &physts->bb_physts_rslt_24_i;
	struct bb_dfs_info* bb_dfs = &bb->bb_dfs_i;

	if (!(physts_bitmap & BIT(IE24_DBG_OFDM_TD_PATH_A)))
		return;

	if (physts->bb_physts_rslt_hdr_i.ie_map_type >= CCK_BRK && physts->bb_physts_rslt_hdr_i.ie_map_type <= VHT_MU)
		return;

	if (physts->bb_physts_rslt_hdr_i.ie_map_type >= RSVD_9 && physts->bb_physts_rslt_hdr_i.ie_map_type <= CCK_PKT)
		return;

	bb_dfs->ACI2SIG_db = (s8)halbb_cnvrt_2_sign(psts_24->aci2sig_db, 7);

	switch (bb->hal_com->band[bb->bb_phy_idx].cur_chandef.bw) {
	case CHANNEL_WIDTH_20:
		bb_dfs->ACI2SIG_db = bb_dfs->ACI2SIG_db - 30;
		//BB_DBG(bb, DBG_DFS, "cur bw : 20\n");
		break;
	case CHANNEL_WIDTH_40:
		bb_dfs->ACI2SIG_db = bb_dfs->ACI2SIG_db - 40;
		//BB_DBG(bb, DBG_DFS, "cur bw : 40\n");
		break;
	case CHANNEL_WIDTH_80:
	case CHANNEL_WIDTH_160:
	case CHANNEL_WIDTH_80_80:
		bb_dfs->ACI2SIG_db = bb_dfs->ACI2SIG_db - 50;
		//BB_DBG(bb, DBG_DFS, "cur bw : 80\n");
		break;
	default:
		bb_dfs->ACI2SIG_db = bb_dfs->ACI2SIG_db - 50;
		//BB_DBG(bb, DBG_DFS, "cur bw : NaN\n");
		break;
	}

	bb_dfs->dfs_aci_is_read = false;
}

void halbb_dfs_dyn_setting(struct bb_info *bb)
{
	struct bb_dfs_info *bb_dfs = &bb->bb_dfs_i;
	struct bb_link_info *link = &bb->bb_link_i;
	struct bb_env_mntr_info *env_mntr = &bb->bb_env_mntr_i;
	struct bb_dfs_cr_info* cr = &bb_dfs->bb_dfs_cr_i;
	u8 bw = bb->hal_com->band[bb->bb_phy_idx].cur_chandef.bw;
	u8 chan = bb->hal_com->band[bb->bb_phy_idx].cur_chandef.chan;
	u8 i;
#ifdef HALBB_STATISTICS_SUPPORT
	struct bb_stat_info *stat = &bb->bb_stat_i;
	struct bb_fa_info *fa = &stat->bb_fa_i;
#endif

#ifdef HALBB_PHYSTS_PARSING_SUPPORT
	struct bb_physts_info	*physts = &bb->bb_physts_i;

	if (bb->phl_com->dev_state & RTW_DEV_IN_DFS_CAC_PERIOD)
		bb_dfs->In_CAC_Flag = true;
	else
		bb_dfs->In_CAC_Flag = false;

	if (bb_dfs->In_CAC_Flag || !link->is_linked)
		bb->bb_physts_i.dfs_phy_sts_privilege = true;
	else
		bb->bb_physts_i.dfs_phy_sts_privilege = false;

	if (bb_dfs->dbg_dyn_prnt_en) {
		BB_DBG(bb, DBG_DFS, "[bb_sys_up_time/show_all_pkt/CAC_flag]: [%d,%d,%d]\n",
			bb->bb_sys_up_time,bb->bb_physts_i.show_phy_sts_all_pkt,bb_dfs->In_CAC_Flag);
	}

	if (bb_dfs->dfs_aci_is_read == false) {
		bb_dfs->dfs_aci_is_read = true;
		bb_dfs->no_aci_rpt_cnt = 0;
		/*brk ppdu sts can be filtered by mac when B_AX_PPDU_STAT_RPT_ADDR(0xce40[4] for 52B/ 0xce40[6] for 52C) = 1*/
		if ((bb->ic_type == BB_RTL8852B) || (bb->ic_type == BB_RTL8851B))
			halbb_physts_brk_fail_rpt_en(bb, true, bb->bb_phy_idx);
		else
			halbb_physts_brk_fail_rpt_en(bb, false, bb->bb_phy_idx);
		if (bb_dfs->dbg_dyn_prnt_en)
			BB_DBG(bb, DBG_DFS, "aci2sig_db is updated\n");
	}
	else {
		bb_dfs->ACI2SIG_db = -128;
		bb_dfs->no_aci_rpt_cnt += 1;
		if (bb_dfs->no_aci_rpt_cnt > bb_dfs->no_aci_rpt_th)
			halbb_physts_brk_fail_rpt_en(bb, true, bb->bb_phy_idx);
		if (bb_dfs->dbg_dyn_prnt_en)
			BB_DBG(bb, DBG_DFS, "aci2sig_db isn't updated, and ACI2SIG_db is set as -128\n");
	}

	/* when channel is switching, there is a short moment channel would be set to 0 */
	if (bb->hal_com->band[bb->bb_phy_idx].cur_chandef.chan == 0) {
		bb_dfs->ACI2SIG_db = -128;
		if (bb_dfs->dbg_dyn_prnt_en)
			BB_DBG(bb, DBG_DFS, "cur ch is Invalid, and ACI2SIG_db is set as -128\n");
	}

#endif
	// aviod down up into Adptive right away
	if (bb_dfs->first_dyn_set_flag) {
		bb_dfs->first_dyn_set_flag = false;
		if (bb_dfs->dbg_dyn_prnt_en)
			BB_DBG(bb, DBG_DFS, "First dyn setting !\n");
		goto DETECTING_END;
	}
	// first set dbgmode reset cnt
	if (bb_dfs->dfs_dbg_mode && bb_dfs->dyn_reset_flag) {
		bb_dfs->adap_detect_cnt = 0;
		bb_dfs->adap_detect_cnt_all = 0;
		bb_dfs->detect_state = DFS_Normal_State;
		bb_dfs->dyn_reset_flag = false;
	}

	/*if (bb_dfs->dfs_dbg_mode)
		bb_dfs->adap_detect_brk_en = false;
	else
		bb_dfs->adap_detect_brk_en = true; */

	if (bb_dfs->dfs_dyn_aci_en == true) {
		if (bb_dfs->dfs_dbg_mode == false) {
			switch (bb->hal_com->band[bb->bb_phy_idx].cur_chandef.bw) {
			case CHANNEL_WIDTH_20:
				bb_dfs->dfs_aci_adaptv_th0 = 12;
				bb_dfs->dfs_aci_adaptv_th1 = 9;
				break;
			case CHANNEL_WIDTH_40:
			case CHANNEL_WIDTH_80:
			case CHANNEL_WIDTH_160:
			case CHANNEL_WIDTH_80_80:
			default:
				bb_dfs->dfs_aci_adaptv_th0 = 5;
				bb_dfs->dfs_aci_adaptv_th1 = 2;
				break;
			}
		}
		else {
			switch (bb->hal_com->band[bb->bb_phy_idx].cur_chandef.bw) {
			case CHANNEL_WIDTH_20:
				bb_dfs->dfs_aci_adaptv_th0 = 15;
				bb_dfs->dfs_aci_adaptv_th1 = 12;
				break;
			case CHANNEL_WIDTH_40:
			case CHANNEL_WIDTH_80:
			case CHANNEL_WIDTH_160:
			case CHANNEL_WIDTH_80_80:
			default:
				bb_dfs->dfs_aci_adaptv_th0 = 6;
				bb_dfs->dfs_aci_adaptv_th1 = 3;
				break;
			}
		}
	}

#ifdef HALBB_TW_DFS_SERIES

	switch (bb->hal_com->band[bb->bb_phy_idx].cur_chandef.bw) {
	case CHANNEL_WIDTH_20:
	case CHANNEL_WIDTH_40:
		bb_dfs->is_tw_en = false;
		bb_dfs->bypass_seg0 = false;
		break;
	case CHANNEL_WIDTH_80:
		bb_dfs->is_tw_en = (bool)halbb_get_reg_cmn(bb, cr->tw_dfs_en, cr->tw_dfs_en_m, bb->bb_phy_idx);
		bb_dfs->bypass_seg0 = false;
		break;
	case CHANNEL_WIDTH_160:
	case CHANNEL_WIDTH_80_80:
		bb_dfs->is_tw_en = true;
		if ((chan >= 36) && (chan <=64))
			bb_dfs->bypass_seg0 = true;
		else
			bb_dfs->bypass_seg0 = false;
		break;
	default:
		bb_dfs->is_tw_en = false;
		bb_dfs->bypass_seg0 = false;
		break;
	}
#endif

	if (link->total_tp < bb_dfs->dfs_tp_th) {
		bb_dfs->idle_flag = true;
		bb_dfs->pw_diff_th = 12;
		bb_dfs->pw_lng_chrp_diff_th = 18;
		bb_dfs->pri_diff_th = 5;
		bb_dfs->pw_factor = PW_FTR_IDLE;
		bb_dfs->pri_factor = PRI_FTR_IDLE;
		bb_dfs->ppb_prcnt = DFS_PPB_IDLE_PRCNT;
	}
	else {
		bb_dfs->idle_flag = false;
		bb_dfs->pw_diff_th = 20;
		bb_dfs->pw_lng_chrp_diff_th = 20;
		bb_dfs->pri_diff_th = 120;
		bb_dfs->pw_factor = PW_FTR;
		bb_dfs->pri_factor = PRI_FTR;
		bb_dfs->ppb_prcnt = DFS_PPB_PRCNT;
	}

	/* Add ACI info for supressing false detection */
	switch (bb_dfs->detect_state) {
	case DFS_Normal_State:
		if (bb_dfs->ACI2SIG_db > bb_dfs->dfs_aci_adaptv_th0) {
			/* block DFS_ADPTV_CNT1 X 2 sec if ACI is detected in DFS_Normal_State */
			bb_dfs->adap_detect_cnt = bb_dfs->adap_detect_cnt_init;
			bb_dfs->adap_detect_cnt_all += 1;
			bb_dfs->detect_state = DFS_Adaptive_State;
			if (bb_dfs->dbg_dyn_prnt_en) {
				BB_DBG(bb, DBG_DFS, "[DFS Status] ACI triggers Adaptive State\n");
				BB_DBG(bb, DBG_DFS, "[DFS Status] Adaptive State : [%d]\n", bb_dfs->adap_detect_cnt);
			}
		}
		else if ((env_mntr->nhm_idle_ratio < bb_dfs->dfs_idle_prd_th && env_mntr->nhm_idle_ratio > 0) ||
			 (fa->cnt_fail_all > bb_dfs->dfs_fa_th) ||
			 (env_mntr->nhm_ratio > bb_dfs->dfs_nhm_th && env_mntr->nhm_ratio != 100)) {
			bb_dfs->adap_detect_cnt = bb_dfs->adap_detect_cnt_init;
			bb_dfs->adap_detect_cnt_all += 1;
			bb_dfs->detect_state = DFS_Adaptive_State;
			if (bb_dfs->dbg_dyn_prnt_en) {
				BB_DBG(bb, DBG_DFS, "[DFS Status] NHM/FA triggers Adaptive State\n");
				BB_DBG(bb, DBG_DFS, "[DFS Status] Adaptive State : [%d]\n", bb_dfs->adap_detect_cnt);
			}
		}
		else {
			if (bb_dfs->dbg_dyn_prnt_en)
				BB_DBG(bb, DBG_DFS, "[DFS Status] Normal State\n");
		}
		if (bb_dfs->dbg_dyn_prnt_en)
			BB_DBG(bb, DBG_DFS, "[DFS Status] Adap_detect_cnt_all = [%d]\n", bb_dfs->adap_detect_cnt_all);

		break;

	case DFS_Adaptive_State:
		bb_dfs->adap_detect_cnt = SUBTRACT_TO_0(bb_dfs->adap_detect_cnt,1);
		/* add  DFS_ADPTV_CNT2 X 2 sec again if ACI is detected in DFS_Adaptive_State */
		if (bb_dfs->ACI2SIG_db > bb_dfs->dfs_aci_adaptv_th1) {
			bb_dfs->adap_detect_cnt = MIN_2(255, bb_dfs->adap_detect_cnt + bb_dfs->adap_detect_cnt_add);
			bb_dfs->adap_detect_cnt_all = MIN_2(255, bb_dfs->adap_detect_cnt_all+1);
			if (bb_dfs->dbg_dyn_prnt_en) {
				BB_DBG(bb, DBG_DFS, "[DFS Status] ACI triggers Adaptive State again\n");
				BB_DBG(bb, DBG_DFS, "[DFS Status] Adaptive State : [%d]\n", bb_dfs->adap_detect_cnt);
			}
		}
		else if ((env_mntr->nhm_idle_ratio < bb_dfs->dfs_idle_prd_th && env_mntr->nhm_idle_ratio > 0) ||
			(fa->cnt_fail_all > bb_dfs->dfs_fa_th) ||
			(env_mntr->nhm_ratio > bb_dfs->dfs_nhm_th && env_mntr->nhm_ratio != 100)) {
			bb_dfs->adap_detect_cnt = MIN_2(255, bb_dfs->adap_detect_cnt + bb_dfs->adap_detect_cnt_add);
			bb_dfs->adap_detect_cnt_all = MIN_2(255, bb_dfs->adap_detect_cnt_all+1);
			if (bb_dfs->dbg_dyn_prnt_en) {
				BB_DBG(bb, DBG_DFS, "[DFS Status] NHM/FA triggers Adaptive State again\n");
				BB_DBG(bb, DBG_DFS, "[DFS Status] Adaptive State : [%d]\n", bb_dfs->adap_detect_cnt);
			}
		}

		if (bb_dfs->adap_detect_cnt == 0) {
			bb_dfs->detect_state = DFS_Normal_State;
			if (bb_dfs->dbg_dyn_prnt_en)
				BB_DBG(bb, DBG_DFS, "[DFS Status] Normal State\n");
		}
		if (bb_dfs->dbg_dyn_prnt_en)
			BB_DBG(bb, DBG_DFS, "[DFS Status] Adap_detect_cnt_all = [%d]\n", bb_dfs->adap_detect_cnt_all);

		break;

	default:
		bb_dfs->detect_state = DFS_Normal_State;
		bb_dfs->adap_detect_cnt = 0;

		break;

	}

	DETECTING_END:
		if (bb_dfs->dbg_dyn_prnt_en) {
			BB_DBG(bb, DBG_DFS, "[Is_link / T_TP / I_RTO / FA_CNT / N_RTO] = [%d, %d, %d, %d, %d]\n",
	       		link->is_linked, link->total_tp, env_mntr->nhm_idle_ratio,
	       		fa->cnt_fail_all, env_mntr->nhm_ratio);
			BB_DBG(bb, DBG_DFS, "[ACI2SIG_db] = [%d]\n", bb_dfs->ACI2SIG_db);
			BB_DBG(bb, DBG_DFS, "DFS Region Domain = %d, BW = %d, Channel = %d\n",
		        bb_dfs->dfs_rgn_domain, bw, chan);
			BB_DBG(bb, DBG_DFS, "\n");
		}
}

void halbb_dfs_debug(struct bb_info *bb, char input[][16], u32 *_used,
			 char *output, u32 *_out_len)
{
	struct bb_dfs_info *bb_dfs = &bb->bb_dfs_i;
	char help[] = "-h";
	u32 var[10] = {0};
	u32 used = *_used;
	u32 out_len = *_out_len;
	u8 i = 0;

	HALBB_SCAN(input[1], DCMD_DECIMAL, &var[0]);

	if ((_os_strcmp(input[1], help) == 0)) {
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "{1} Set DFS_SW_TRGR_MODE => {0}: Disable, {1}: Enable\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "{2} Set DFS_DBG_MODE => {0}: Disable, {1}: Enable\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "{3} Set PRNT LEVEL => \n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "---{1} Set DBG_HWDET_PRINT => {0}: Disable, {1}: Enable\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "---{2} Set DBG_SWDET_PRINT => {0}: Disable, {1}: Enable\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "---{3} Set DBG_BRK_PRINT => {0}: Disable, {1}: Enable\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "---{4} Set DBG_DYN_PRINT => {0}: Disable, {1}: Enable\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "---{5} Set DBG_TRIVIL_PRINT => {0}: Disable, {1}: Enable\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "{4} Set DYN_SETTING_EN => {0}: Disable, {1}: Enable\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "{5} Set Adap_detect_brk_EN => {0}: Disable, {1}: Enable\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "{6} Set Detection Parameter => \n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "---{1} Set the threshold of fake DFS number => {Num}\n");
		/*
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "---{2} Set the threshold of chirp number => {Num}\n");
		*/
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "---{3} Set the threshold of ppb percent => {Percent: 1-8}\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "---{4} Set the threshold of DFS_TP Threshold => {Mbps}\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "---{5} Set the threshold of DFS_Idle_Period Threshold => {Percent: 0-100}\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "---{6} Set the threshold of DFS_FA Threshold => {Num}\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "---{7} Set the threshold of DFS_NHM Threshold => {Percent: 0-100}\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "---{8} Set the threshold of DFS_N_CNFD_Level Threshold => {Num}\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "---{9} Reset aci_disable_detect_cnt\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "---{10} Set the threshold of dfs_aci_adaptv_th0 => {Num}\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "---{11} Set the threshold of dfs_aci_adaptv_th1 => {Num}\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "---{12} Set dfs_dyn_aci_en => {0}: Disable, {1}: Enable\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "---{13} Set the threshold of pw_diff_th => {Num}\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "---{14} Set the threshold of pri_diff_th => {Num}\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "---{15} Set the threshold of no_aci_rpt_th => {Num}\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "---{16} Set the threshold of PW_FTR => {Percent: 1-8}\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "---{17} Set the threshold of PRI_FTR => {Percent: 1-8}\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "---{18} Set the threshold of adap_detect_cnt_init => {Num}\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "---{19} Set the threshold of adap_detect_cnt_add => {Num}\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "---{20} Set the threshold of adap_detect_cnt_th => {Num}\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "---{21} Set the threshold of pw_max_th => {Num}\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "---{22} Set the threshold of pw_lng_chrp_th => {Num}\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "---{23} Set the threshold of invalid_lng_pulse_th => {Num}\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "---{24} Set the threshold of pri_mask_th => {Num}\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "---{25} Set bypass_seg0_en => {0}: Disable, {1}: Enable\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "{7} Set SPEC pw/pri/ppb thd => \n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "---{1} Set pw[i+1] thd=> {i} {LB} {UB}\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "---{2} Set pri[i+1] thd => {i} {LB} {UB}\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "---{3} Set ppb[i+1] thd => {i} {PPB}\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "{8} Set DFS MASK L2H Value s(8,0) => {HEX}\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "{100} Show all parameter\n");
	} else if (var[0] == 100) {
		BB_DBG_CNSL(out_len, used, output + used, out_len - used, "DFS Region Domain: %s\n",
			   (bb_dfs->dfs_rgn_domain > 1) ?
			   (bb_dfs->dfs_rgn_domain > 2) ?
			   "ETSI": "MIC" : "FCC");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used, "DFS_SW_TRGR_MODE = %d\n",
			    bb_dfs->dfs_sw_trgr_mode);
		BB_DBG_CNSL(out_len, used, output + used, out_len - used, "DFS_DBG_MODE = %d\n",
			    bb_dfs->dfs_dbg_mode);
		BB_DBG_CNSL(out_len, used, output + used, out_len - used, "DBG_HWDET_PRINT = %d\n",
			    bb_dfs->dbg_hwdet_prnt_en);
		BB_DBG_CNSL(out_len, used, output + used, out_len - used, "DBG_SWDET_PRINT = %d\n",
			    bb_dfs->dbg_swdet_prnt_en);
		BB_DBG_CNSL(out_len, used, output + used, out_len - used, "DBG_BRK_PRINT = %d\n",
			    bb_dfs->dbg_trivil_prnt_en);
		BB_DBG_CNSL(out_len, used, output + used, out_len - used, "DBG_DYN_PRINT = %d\n",
			    bb_dfs->dbg_dyn_prnt_en);
		BB_DBG_CNSL(out_len, used, output + used, out_len - used, "DBG_TRIVIL_PRINT = %d\n",
			    bb_dfs->dbg_trivil_prnt_en);
		BB_DBG_CNSL(out_len, used, output + used, out_len - used, "DYN_SETTING_EN = %d\n",
			    bb_dfs->dfs_dyn_setting_en);
		BB_DBG_CNSL(out_len, used, output + used, out_len - used, "Adap_detect_brk_EN = %d\n",
			    bb_dfs->adap_detect_brk_en);
		BB_DBG_CNSL(out_len, used, output + used, out_len - used, "Fake DFS Num Threshold = %d\n",
			    bb_dfs->fk_dfs_num_th);
		BB_DBG_CNSL(out_len, used, output + used, out_len - used, "DFS MASK L2H Value (dBm) = %d\n",
			    bb_dfs->dfs_mask_l2h_val >= 128 ? bb_dfs->dfs_mask_l2h_val-256 : bb_dfs->dfs_mask_l2h_val);
		/*
		BB_DBG_CNSL(out_len, used, output + used, out_len - used, "Chirp Number = %d\n",
			    bb_dfs->chrp_th);
		*/
		BB_DBG_CNSL(out_len, used, output + used, out_len - used, "PPB Percent = %d\n",
			    bb_dfs->ppb_prcnt);
		BB_DBG_CNSL(out_len, used, output + used, out_len - used, "DFS_TP Threshold = %d\n",
			    bb_dfs->dfs_tp_th);
		BB_DBG_CNSL(out_len, used, output + used, out_len - used, "DFS_Idle_Period Threshold = %d\n",
			    bb_dfs->dfs_idle_prd_th);
		BB_DBG_CNSL(out_len, used, output + used, out_len - used, "DFS_FA Threshold = %d\n",
			    bb_dfs->dfs_fa_th);
		BB_DBG_CNSL(out_len, used, output + used, out_len - used, "DFS_NHM Threshold = %d\n",
			    bb_dfs->dfs_nhm_th);
		BB_DBG_CNSL(out_len, used, output + used, out_len - used, "DFS_N_CNFD_Level Threshold = %d\n",
			    bb_dfs->dfs_n_cnfd_lvl_th);
		BB_DBG_CNSL(out_len, used, output + used, out_len - used, "detect_state = %d\n",
			    bb_dfs->detect_state);
		BB_DBG_CNSL(out_len, used, output + used, out_len - used, "dfs_aci_adaptv_th0 = %d\n",
			    bb_dfs->dfs_aci_adaptv_th0);
		BB_DBG_CNSL(out_len, used, output + used, out_len - used, "dfs_aci_adaptv_th1 = %d\n",
			    bb_dfs->dfs_aci_adaptv_th1);
		BB_DBG_CNSL(out_len, used, output + used, out_len - used, "dfs_dyn_aci_en = %d\n",
			    bb_dfs->dfs_dyn_aci_en);
		BB_DBG_CNSL(out_len, used, output + used, out_len - used, "pw_diff_th = %d\n",
			    bb_dfs->pw_diff_th);
		BB_DBG_CNSL(out_len, used, output + used, out_len - used, "pri_diff_th = %d\n",
				bb_dfs->pri_diff_th);
		BB_DBG_CNSL(out_len, used, output + used, out_len - used, "no_aci_rpt_th = %d\n",
				bb_dfs->no_aci_rpt_th);
		BB_DBG_CNSL(out_len, used, output + used, out_len - used, "PW_FTR = %d\n",
				bb_dfs->pw_factor);
		BB_DBG_CNSL(out_len, used, output + used, out_len - used, "PRI_FTR = %d\n",
				bb_dfs->pri_factor);
		BB_DBG_CNSL(out_len, used, output + used, out_len - used, "adap_detect_cnt_init = %d\n",
				bb_dfs->adap_detect_cnt_init);
		BB_DBG_CNSL(out_len, used, output + used, out_len - used, "adap_detect_cnt_add = %d\n",
				bb_dfs->adap_detect_cnt_add);
		BB_DBG_CNSL(out_len, used, output + used, out_len - used, "adap_detect_cnt_th = %d\n",
				bb_dfs->adap_detect_cnt_th);
		BB_DBG_CNSL(out_len, used, output + used, out_len - used, "pw_max_th = %d\n",
				bb_dfs->pw_max_th);
		BB_DBG_CNSL(out_len, used, output + used, out_len - used, "pw_lng_chrp_diff_th = %d\n",
				bb_dfs->pw_lng_chrp_diff_th);
		BB_DBG_CNSL(out_len, used, output + used, out_len - used, "invalid_lng_pulse_th = %d\n",
				bb_dfs->invalid_lng_pulse_th);
		BB_DBG_CNSL(out_len, used, output + used, out_len - used, "pri_mask_th = %d\n",
				bb_dfs->pri_mask_th);
		BB_DBG_CNSL(out_len, used, output + used, out_len - used, "bypass_seg0 = %d\n",
				bb_dfs->bypass_seg0);
		BB_DBG_CNSL(out_len, used, output + used, out_len - used, "SPEC TH : \n");
		for (i = 0; i < DFS_RDR_TYP_NUM ; i++) {
			BB_DBG_CNSL(out_len, used, output + used, out_len - used, "Type%d : pw = [%d-%d], pri = [%d-%d], ppb = %d\n",
				i+1,bb_dfs->pw_min_tab[i],bb_dfs->pw_max_tab[i],
				bb_dfs->pri_min_tab[i],bb_dfs->pri_max_tab[i],bb_dfs->ppb_tab[i]);
		}
		BB_DBG_CNSL(out_len, used, output + used, out_len - used, "Modify TH : \n");
		for (i = 0; i < DFS_RDR_TYP_NUM ; i++) {
			BB_DBG_CNSL(out_len, used, output + used, out_len - used, "Type%d : pw = [%d-%d], pri = [%d-%d], ppb = %d\n",
				i+1,bb_dfs->pw_lbd[i],bb_dfs->pw_ubd[i],
				bb_dfs->pri_lbd[i],bb_dfs->pri_ubd[i],bb_dfs->ppb_typ_th[i]);
		}
	} else {
		if (var[0] == 1) {
			HALBB_SCAN(input[2], DCMD_DECIMAL, &var[1]);
			bb_dfs->dfs_sw_trgr_mode = (bool)var[1];
			BB_DBG_CNSL(out_len, used, output + used,
				    out_len - used, "DFS_SW_TRGR_MODE = %d\n",
				    bb_dfs->dfs_sw_trgr_mode);
		} else if (var[0] == 2) {
			HALBB_SCAN(input[2], DCMD_DECIMAL, &var[1]);
			bb_dfs->dfs_dbg_mode = (bool)var[1];
			BB_DBG_CNSL(out_len, used, output + used,
				    out_len - used, "DFS_DBG_MODE = %d\n",
				    bb_dfs->dfs_dbg_mode);
		} else if (var[0] == 3) {
			HALBB_SCAN(input[2], DCMD_DECIMAL, &var[1]);
			if (var[1] == 1) {
				HALBB_SCAN(input[3], DCMD_DECIMAL, &var[2]);
				bb_dfs->dbg_hwdet_prnt_en = (bool)var[2];
				BB_DBG_CNSL(out_len, used, output + used,
					    out_len - used, "DBG_HWDET_PRINT = %d\n",
					    bb_dfs->dbg_hwdet_prnt_en);
			} else if (var[1] == 2) {
				HALBB_SCAN(input[3], DCMD_DECIMAL, &var[2]);
				bb_dfs->dbg_swdet_prnt_en = (bool)var[2];
				BB_DBG_CNSL(out_len, used, output + used,
					    out_len - used, "DBG_SWDET_PRINT = %d\n",
					    bb_dfs->dbg_swdet_prnt_en);
			} else if (var[1] == 3) {
				HALBB_SCAN(input[3], DCMD_DECIMAL, &var[2]);
				bb_dfs->dbg_brk_prnt_en = (bool)var[2];
				BB_DBG_CNSL(out_len, used, output + used,
					    out_len - used, "DBG_BRK_PRINT = %d\n",
					    bb_dfs->dbg_brk_prnt_en);
			} else if (var[1] == 4) {
				HALBB_SCAN(input[3], DCMD_DECIMAL, &var[2]);
				bb_dfs->dbg_dyn_prnt_en = (bool)var[2];
				BB_DBG_CNSL(out_len, used, output + used,
					    out_len - used, "DBG_DYN_PRINT = %d\n",
					    bb_dfs->dbg_dyn_prnt_en);
			} else if (var[1] == 5) {
				HALBB_SCAN(input[3], DCMD_DECIMAL, &var[2]);
				bb_dfs->dbg_trivil_prnt_en = (bool)var[2];
				BB_DBG_CNSL(out_len, used, output + used,
					    out_len - used, "DBG_TRIVIL_PRINT = %d\n",
					    bb_dfs->dbg_trivil_prnt_en);
			}
		} else if (var[0] == 4) {
			HALBB_SCAN(input[2], DCMD_DECIMAL, &var[1]);
			bb_dfs->dfs_dyn_setting_en = (bool)var[1];
			BB_DBG_CNSL(out_len, used, output + used,
				    out_len - used, "DYN_SETTING_EN = %d\n",
				    bb_dfs->dfs_dyn_setting_en);
		} else if (var[0] == 5) {
			HALBB_SCAN(input[2], DCMD_DECIMAL, &var[1]);
			bb_dfs->adap_detect_brk_en = (bool)var[1];
			BB_DBG_CNSL(out_len, used, output + used,
				    out_len - used, "Adap_detect_brk_EN = %d\n",
				    bb_dfs->adap_detect_brk_en);
		} else if (var[0] == 6) {
			HALBB_SCAN(input[2], DCMD_DECIMAL, &var[1]);
			if (var[1] == 1) {
				HALBB_SCAN(input[3], DCMD_DECIMAL, &var[2]);
				bb_dfs->fk_dfs_num_th = (u8)var[2];
				BB_DBG_CNSL(out_len, used, output + used,
					    out_len - used, "fk_dfs_num_th = %d\n",
					    bb_dfs->fk_dfs_num_th);
			} else if (var[1] == 2) {
			/*
				HALBB_SCAN(input[3], DCMD_DECIMAL, &var[2]);
				bb_dfs->chrp_th= (u8)var[2];
				BB_DBG_CNSL(out_len, used, output + used,
					    out_len - used, "chrp_th = %d\n",
					    bb_dfs->chrp_th);
			*/
			} else if (var[1] == 3) {
				HALBB_SCAN(input[3], DCMD_DECIMAL, &var[2]);
				bb_dfs->ppb_prcnt = (u8)var[2];
				BB_DBG_CNSL(out_len, used, output + used,
					    out_len - used, "ppb_prcnt = %d\n",
					    bb_dfs->ppb_prcnt);
			} else if (var[1] == 4) {
				HALBB_SCAN(input[3], DCMD_DECIMAL, &var[2]);
				bb_dfs->dfs_tp_th = (u8)var[2];
				BB_DBG_CNSL(out_len, used, output + used,
					    out_len - used, "dfs_tp_th = %d\n",
					    bb_dfs->dfs_tp_th);
			} else if (var[1] == 5) {
				HALBB_SCAN(input[3], DCMD_DECIMAL, &var[2]);
				bb_dfs->dfs_idle_prd_th = (u8)var[2];
				BB_DBG_CNSL(out_len, used, output + used,
					    out_len - used, "dfs_idle_prd_th = %d\n",
					    bb_dfs->dfs_idle_prd_th);
			} else if (var[1] == 6) {
				HALBB_SCAN(input[3], DCMD_DECIMAL, &var[2]);
				bb_dfs->dfs_fa_th= (u16)var[2];
				BB_DBG_CNSL(out_len, used, output + used,
					    out_len - used, "dfs_fa_th = %d\n",
					    bb_dfs->dfs_fa_th);
			} else if (var[1] == 7) {
				HALBB_SCAN(input[3], DCMD_DECIMAL, &var[2]);
				bb_dfs->dfs_nhm_th= (u8)var[2];
				BB_DBG_CNSL(out_len, used, output + used,
					    out_len - used, "dfs_nhm_th = %d\n",
					    bb_dfs->dfs_nhm_th);
			} else if (var[1] == 8) {
				HALBB_SCAN(input[3], DCMD_DECIMAL, &var[2]);
				bb_dfs->dfs_n_cnfd_lvl_th= (u8)var[2];
				BB_DBG_CNSL(out_len, used, output + used,
					    out_len - used, "dfs_n_cnfd_lvl_th = %d\n",
					    bb_dfs->dfs_n_cnfd_lvl_th);
			} else if (var[1] == 9) {
				bb_dfs->adap_detect_cnt = 0;
				bb_dfs->adap_detect_cnt_all =0;
				bb_dfs->detect_state = DFS_Normal_State;
				BB_DBG_CNSL(out_len, used, output + used,
					    out_len - used, "reset aci_disable_detect_cnt\n");
			} else if (var[1] == 10) {
				HALBB_SCAN(input[3], DCMD_DECIMAL, &var[2]);
				bb_dfs->dfs_aci_adaptv_th0= (u8)var[2];
				BB_DBG_CNSL(out_len, used, output + used,
					    out_len - used, "dfs_aci_adaptv_th0 = %d\n",
					    bb_dfs->dfs_aci_adaptv_th0);
			} else if (var[1] == 11) {
				HALBB_SCAN(input[3], DCMD_DECIMAL, &var[2]);
				bb_dfs->dfs_aci_adaptv_th1= (u8)var[2];
				BB_DBG_CNSL(out_len, used, output + used,
					    out_len - used, "dfs_aci_adaptv_th1 = %d\n",
					    bb_dfs->dfs_aci_adaptv_th1);
			} else if (var[1] == 12) {
				HALBB_SCAN(input[3], DCMD_DECIMAL, &var[2]);
				bb_dfs->dfs_dyn_aci_en = (bool)var[2];
				BB_DBG_CNSL(out_len, used, output + used,
					    out_len - used, "dfs_dyn_aci_en = %d\n",
					    bb_dfs->dfs_dyn_aci_en);
			} else if (var[1] == 13) {
				HALBB_SCAN(input[3], DCMD_DECIMAL, &var[2]);
				bb_dfs->pw_diff_th= (u16)var[2];
				BB_DBG_CNSL(out_len, used, output + used,
					    out_len - used, "pw_diff_th = %d\n",
					    bb_dfs->pw_diff_th);
			} else if (var[1] == 14) {
				HALBB_SCAN(input[3], DCMD_DECIMAL, &var[2]);
				bb_dfs->pri_diff_th = (u16)var[2];
				BB_DBG_CNSL(out_len, used, output + used,
					out_len - used, "pri_diff_th = %d\n",
					bb_dfs->pri_diff_th);
			} else if (var[1] == 15) {
				HALBB_SCAN(input[3], DCMD_DECIMAL, &var[2]);
				bb_dfs->no_aci_rpt_th = (u8)var[2];
				BB_DBG_CNSL(out_len, used, output + used,
					out_len - used, "no_aci_rpt_th = %d\n",
					bb_dfs->no_aci_rpt_th);
			} else if (var[1] == 16) {
				HALBB_SCAN(input[3], DCMD_DECIMAL, &var[2]);
				bb_dfs->pw_factor = (u8)var[2];
				BB_DBG_CNSL(out_len, used, output + used,
					out_len - used, "PW_FTR = %d\n",
					bb_dfs->pw_factor);
			} else if (var[1] == 17) {
				HALBB_SCAN(input[3], DCMD_DECIMAL, &var[2]);
				bb_dfs->pri_factor = (u8)var[2];
				BB_DBG_CNSL(out_len, used, output + used,
					out_len - used, "PRI_FTR = %d\n",
					bb_dfs->pri_factor);
			} else if (var[1] == 18) {
				HALBB_SCAN(input[3], DCMD_DECIMAL, &var[2]);
				bb_dfs->adap_detect_cnt_init= (u8)var[2];
				BB_DBG_CNSL(out_len, used, output + used,
					out_len - used, "adap_detct_cnt_init = %d\n",
					bb_dfs->adap_detect_cnt_init);
			} else if (var[1] == 19) {
				HALBB_SCAN(input[3], DCMD_DECIMAL, &var[2]);
				bb_dfs->adap_detect_cnt_add = (u8)var[2];
				BB_DBG_CNSL(out_len, used, output + used,
					out_len - used, "adap_detct_cnt_add = %d\n",
					bb_dfs->adap_detect_cnt_add);
			} else if (var[1] == 20) {
				HALBB_SCAN(input[3], DCMD_DECIMAL, &var[2]);
				bb_dfs->adap_detect_cnt_th= (u8)var[2];
				BB_DBG_CNSL(out_len, used, output + used,
					out_len - used, "adap_detct_cnt_th = %d\n",
					bb_dfs->adap_detect_cnt_th);
			}else if (var[1] == 21) {
				HALBB_SCAN(input[3], DCMD_DECIMAL, &var[2]);
				bb_dfs->pw_max_th = (u16)var[2];
				BB_DBG_CNSL(out_len, used, output + used,
					out_len - used, "pw_max_th = %d\n",
					bb_dfs->pw_max_th);
			}else if (var[1] == 22) {
				HALBB_SCAN(input[3], DCMD_DECIMAL, &var[2]);
				bb_dfs->pw_lng_chrp_diff_th = (u16)var[2];
				BB_DBG_CNSL(out_len, used, output + used,
					out_len - used, "pw_lng_chrp_diff_th = %d\n",
					bb_dfs->pw_lng_chrp_diff_th);
			}else if (var[1] == 23) {
				HALBB_SCAN(input[3], DCMD_DECIMAL, &var[2]);
				bb_dfs->invalid_lng_pulse_th= (u16)var[2];
				BB_DBG_CNSL(out_len, used, output + used,
					out_len - used, "invalid_lng_pulse_th = %d\n",
					bb_dfs->invalid_lng_pulse_th);
			}else if (var[1] == 24) {
				HALBB_SCAN(input[3], DCMD_DECIMAL, &var[2]);
				bb_dfs->pri_mask_th= (u8)var[2];
				BB_DBG_CNSL(out_len, used, output + used,
					out_len - used, "pri_mask_th = %d\n",
					bb_dfs->pri_mask_th);
			}else if (var[1] == 25) {
			HALBB_SCAN(input[3], DCMD_DECIMAL, &var[2]);
			bb_dfs->bypass_seg0= (bool)var[2];
			BB_DBG_CNSL(out_len, used, output + used,
				out_len - used, "bypass_seg0 = %d\n",
				bb_dfs->bypass_seg0);
			}
		} else if (var[0] == 7) {
			HALBB_SCAN(input[2], DCMD_DECIMAL, &var[1]);
			if (var[1] == 1) {
				HALBB_SCAN(input[3], DCMD_DECIMAL, &var[2]);
				HALBB_SCAN(input[4], DCMD_DECIMAL, &var[3]);
				HALBB_SCAN(input[5], DCMD_DECIMAL, &var[4]);
				i = (u8)var[2];
				bb_dfs->pw_min_tab[i]= (u8)var[3];
				bb_dfs->pw_max_tab[i]= (u16)var[4];
				BB_DBG_CNSL(out_len, used, output + used,
					    out_len - used, "Type%d: pw = [%d-%d]\n",
					    i+1,bb_dfs->pw_min_tab[i],bb_dfs->pw_max_tab[i]);
			} else if (var[1] == 2) {
				HALBB_SCAN(input[3], DCMD_DECIMAL, &var[2]);
				HALBB_SCAN(input[4], DCMD_DECIMAL, &var[3]);
				HALBB_SCAN(input[5], DCMD_DECIMAL, &var[4]);
				i = (u8)var[2];
				bb_dfs->pri_min_tab[i]= (u8)var[3];
				bb_dfs->pri_max_tab[i]= (u8)var[4];
				BB_DBG_CNSL(out_len, used, output + used,
					    out_len - used, "Type%d: pri = [%d-%d]\n",
					    i+1,bb_dfs->pri_min_tab[i],bb_dfs->pri_max_tab[i]);
			} else if (var[1] == 3) {
				HALBB_SCAN(input[3], DCMD_DECIMAL, &var[2]);
				HALBB_SCAN(input[4], DCMD_DECIMAL, &var[3]);
				i = (u8)var[2];
				bb_dfs->ppb_tab[i]= (u8)var[3];
				BB_DBG_CNSL(out_len, used, output + used,
					    out_len - used, "Type%d: ppb = %d\n",
					    i+1,bb_dfs->ppb_tab[i]);
			}
		} else if (var[0] == 8) {
			HALBB_SCAN(input[2], DCMD_DECIMAL, &var[1]);
			bb_dfs->dfs_mask_l2h_val = (u8)var[1];
			BB_DBG_CNSL(out_len, used, output + used,
				    out_len - used, "DFS MASK L2H Value = %d\n",
				    bb_dfs->dfs_mask_l2h_val);
		}
	}

	*_used = used;
	*_out_len = out_len;
}

void halbb_cr_cfg_dfs_init(struct bb_info *bb)
{
	struct bb_dfs_info *bb_dfs = &bb->bb_dfs_i;
	struct bb_dfs_cr_info *cr = &bb_dfs->bb_dfs_cr_i;

	switch (bb->cr_type) {

#ifdef HALBB_COMPILE_AP_SERIES
	case BB_AP:
		cr->dfs_en = DFS_EN_A;
		cr->dfs_en_m = DFS_EN_A_M;
		break;
#endif

#ifdef HALBB_COMPILE_AP2_SERIES
	case BB_AP2:
		cr->dfs_en = DFS_EN_A2;
		cr->dfs_en_m = DFS_EN_A2_M;
#ifdef HALBB_TW_DFS_SERIES
		cr->tw_dfs_en = TW_DFS_EN_A2;
		cr->tw_dfs_en_m = TW_DFS_EN_A2_M;
#endif
#ifdef HALBB_DBCC_SUPPORT
		cr->dfs_en_p1 = DFS_EN_A2_P1;
		cr->dfs_en_p1_m = DFS_EN_A2_P1_M;
		cr->dfs_l2h_th = DFS_L2H_TH;
		cr->dfs_l2h_th_m = DFS_L2H_TH_M;
#endif
		break;

#endif

#ifdef HALBB_COMPILE_CLIENT_SERIES
	case BB_CLIENT:
		cr->dfs_en = DFS_EN_C;
		cr->dfs_en_m = DFS_EN_C_M;
		break;
#endif
#ifdef HALBB_COMPILE_BE0_SERIES
	case BB_BE0:
		cr->dfs_en = DFS_EN_BE0;
		cr->dfs_en_m = DFS_EN_BE0_M;
		break;
#endif
	default:
		BB_WARNING("[%s] BBCR Hook FAIL!\n", __func__);
		if (bb->bb_dbg_i.cr_fake_init_hook_en) {
			BB_TRACE("[%s] BBCR fake init\n", __func__);
			halbb_cr_hook_fake_init(bb, (u32 *)cr, (sizeof(struct bb_dfs_cr_info) >> 2));
		}
		break;
	}

	if (bb->bb_dbg_i.cr_init_hook_recorder_en) {
		BB_TRACE("[%s] BBCR Hook dump\n", __func__);
		halbb_cr_hook_init_dump(bb, (u32 *)cr, (sizeof(struct bb_dfs_cr_info) >> 2));
	}
}
#endif

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
#ifdef HALBB_DBCC_SUPPORT

void halbb_media_status_delete_phy1(struct bb_info *bb_in)
{
	struct bb_info *bb_0;
	struct bb_info *bb_1;
	struct rtw_hal_com_t *hal = bb_in->hal_com;
	struct rtw_phl_stainfo_t *phl_sta_i;
	u8 i = 0, sta_cnt = 0;

	BB_DBG(bb_in, DBG_DBCC, "[%s]\n", __func__);

	HALBB_GET_PHY_PTR(bb_in, bb_0, HW_PHY_0);
	HALBB_GET_PHY_PTR(bb_in, bb_1, HW_PHY_1);

	for (i = 0; i < PHL_MAX_STA_NUM; i++) {

		phl_sta_i = bb_in->phl_sta_info[i];

		if (!phl_sta_i) {
			BB_WARNING("[%s] NULL\n", __func__);
			return;
		}

		BB_DBG(bb_in, DBG_DBCC, "[phy-%d] MACID=%d\n",
		       phl_sta_i->rlink->hw_band, phl_sta_i->macid);

		if (phl_sta_i->rlink->hw_band == HW_BAND_0) {
			if (bb_0->sta_exist[i]) {
				BB_WARNING("[%s] err_macid=%d\n", __func__, i);
			}
		} else { /* HW_BAND_1 */
			if (bb_1->sta_exist[i]) {
				bb_0->sta_exist[i] = true;
				bb_1->sta_exist[i] = false;
			}
		}

		sta_cnt++;

		if (sta_cnt >= hal->assoc_sta_cnt)
			break;
	}

	bb_0->bb_sta_cnt = sta_cnt;
	bb_1->bb_sta_cnt = 0;
}

void halbb_dbcc_band_switch_notify(struct bb_info *bb)
{
	struct bb_info *bb_0;
	struct bb_info *bb_1;
	struct rtw_hal_com_t *hal = bb->hal_com;
	u32 val_tmp;

	BB_DBG(bb, DBG_DBCC, "[%s]\n", __func__);

	HALBB_GET_PHY_PTR(bb, bb_0, HW_PHY_0);
	HALBB_GET_PHY_PTR(bb, bb_1, HW_PHY_1);

	halbb_media_status_delete_phy1(bb);

	BB_DBG(bb, DBG_DBCC, 
	       "[%s] PHY[0] Fc_CH = %03d/%03d, is_2g=%d\n", __func__, 
	       bb_0->hal_com->band[0].cur_chandef.center_ch,
	       bb_0->bb_ch_i.fc_ch_idx, bb_0->bb_ch_i.is_2g);

	BB_DBG(bb, DBG_DBCC, 
	       "PHY[1] Fc_CH = %03d/%03d, is_2g=%d\n",
	       bb_1->hal_com->band[1].cur_chandef.center_ch,
	       bb_1->bb_ch_i.fc_ch_idx,
	       bb_1->bb_ch_i.is_2g);

	BB_DBG(bb, DBG_DBCC, 
	       "DBCC_en=%d/%d, DBCC_sup=%d,cck_blk_en=%d, cck_phy_map=%d\n",
	       bb->hal_com->dbcc_en, bb->bb_cmn_hooker->bb_dbcc_en,
	       bb->phl_com->dev_cap.dbcc_sup,
	       bb->bb_cmn_hooker->cck_blk_en, bb->bb_cmn_hooker->cck_phy_map);

	val_tmp = halbb_get_reg(bb, 0x4970, 0x3);

	BB_DBG(bb, DBG_DBCC,  "0x4970[1:0]=0x%x\n", val_tmp);

	val_tmp = halbb_get_reg(bb, 0x2344, BIT31);

	BB_DBG(bb, DBG_DBCC,  "0x2344[31] Disable_CCK=0x%x \n", val_tmp);

	BB_DBG(bb, DBG_DBCC, "[Drv STA_cnt] all(%d) = phy0(%d) + phy1(%d)\n",
	       hal->assoc_sta_cnt,
	       bb->hal_com->band[0].assoc_sta_cnt,
	       bb->hal_com->band[1].assoc_sta_cnt);

	BB_DBG(bb, DBG_DBCC, "[BB STA_cnt]  all(%d) = phy%d(%d) + phy%d(%d)\n",
	       bb_0->bb_sta_cnt + bb_1->bb_sta_cnt,
	       bb_0->bb_phy_idx, bb_0->bb_sta_cnt,
	       bb_1->bb_phy_idx, bb_1->bb_sta_cnt);

	BB_DBG(bb, DBG_DBCC, "[RSSI_min]    phy%d(%d) + phy%d(%d)\n\n",
	       bb_0->bb_phy_idx, bb_0->bb_ch_i.rssi_min >> 1,
	       bb_1->bb_phy_idx, bb_1->bb_ch_i.rssi_min >> 1);
}

enum bb_path halbb_get_cur_phy_valid_path(struct bb_info *bb)
{
	enum bb_path valid_path = BB_PATH_AB;

	switch (bb->ic_type) {

	case BB_RTL8852A:
	case BB_RTL8852C:
		if (bb->hal_com->dbcc_en) {
			 if (bb->bb_phy_idx == HW_PHY_1)
			 	valid_path = BB_PATH_B;
			 else
			 	valid_path = BB_PATH_A;
		} else {
			valid_path = BB_PATH_AB;
		}

		break;

	default:
		break;
	}
	return valid_path;
}

bool halbb_ctrl_dbcc(struct bb_info *bb, bool dbcc_enable)
{
	if (!bb->bb_cmn_hooker->ic_dbcc_support)
		return false;

	switch (bb->ic_type) {

	#ifdef BB_8852A_2_SUPPORT
	case BB_RTL8852A:
		halbb_ctrl_dbcc_8852a_2(bb, dbcc_enable);
		break;
	#endif
	#ifdef BB_8852C_SUPPORT
	case BB_RTL8852C:
		halbb_ctrl_dbcc_8852c(bb, dbcc_enable);
		break;
	#endif
	default:
		break;
	}
	return true;
}

bool halbb_cfg_dbcc_cck_phy_map(struct bb_info *bb, enum phl_phy_idx cck_phy_map)
{
	if (!bb->bb_cmn_hooker->ic_dbcc_support)
		return false;

	BB_DBG(bb, DBG_DBCC, "[%s] cck_phy_map=%d\n", __func__, cck_phy_map);

	switch (bb->ic_type) {

	#ifdef BB_8852A_2_SUPPORT
	case BB_RTL8852A:
		halbb_cfg_dbcc_cck_phy_map_8852a_2(bb, bb->hal_com->dbcc_en, cck_phy_map);
		break;
	#endif
	#ifdef BB_8852C_SUPPORT
	case BB_RTL8852C:
		halbb_cfg_dbcc_cck_phy_map_8852c(bb, bb->hal_com->dbcc_en, cck_phy_map);
		break;
	#endif

	default:
		break;
	}
	return true;
}

void halbb_cfo_trk_joint_phy_dec(struct bb_info *bb_in)
{
	struct bb_info *bb_0, *bb_1;
	struct rtw_phl_stainfo_t *phl_sta_i;
	enum phl_phy_idx phy_idx_tmp = HW_PHY_0;
	u8 i;
	u8 wmode_max_phy0 = 0, wmode_max_phy1 = 0, wmode_tmp = 0;
	u32 val = 0;

	BB_DBG(bb_in, DBG_DBCC, "[%s]\n", __func__);

	if (!bb_in->hal_com->dbcc_en)
		return;

	if (bb_in->bb_phy_idx == HW_PHY_0) {
		bb_0 = bb_in;
		HALBB_GET_PHY_PTR(bb_in, bb_1, HW_PHY_1);
	} else {
		HALBB_GET_PHY_PTR(bb_in, bb_0, HW_PHY_0);
		bb_1 = bb_in;
	}

	for (i = 0; i < PHL_MAX_STA_NUM; i++) {
		if (bb_0->sta_exist[i]) {
			phl_sta_i = bb_0->phl_sta_info[i];
			phy_idx_tmp = bb_0->bb_phy_idx;
		}
		#ifdef HALBB_DBCC_SUPPORT
		else if (bb_1->sta_exist[i]) {
			phl_sta_i = bb_1->phl_sta_info[i];
			phy_idx_tmp = bb_1->bb_phy_idx;
		}
		#endif
		else {
			continue;
		}

		if (!is_sta_active(phl_sta_i))
			continue;

		BB_DBG(bb_in, DBG_DBCC,
		       "MACID[%d], phy_idx=%d, Mode=%s%s%s%s%s\n",
		       i, phy_idx_tmp,
		       (phl_sta_i->wmode & WLAN_MD_11B) ? "B" : " ",
		       (phl_sta_i->wmode & (WLAN_MD_11G | WLAN_MD_11A)) ? "G" : " ",
		       (phl_sta_i->wmode & WLAN_MD_11N) ? "N" : " ",
		       (phl_sta_i->wmode & WLAN_MD_11AC) ? "AC" : " ",
		       (phl_sta_i->wmode & WLAN_MD_11AX) ? "AX" : " ");

		wmode_tmp = phl_sta_i->wmode & 0xf8;
		if (phy_idx_tmp == HW_PHY_0) {
			if (wmode_tmp > wmode_max_phy0)
				wmode_max_phy0 = wmode_tmp;

			BB_DBG(bb_in, DBG_DBCC, "wmode_max[0] = 0x%x\n", wmode_max_phy0);
		}

		if (phy_idx_tmp == HW_PHY_1) {
			if (wmode_tmp > wmode_max_phy1)
				wmode_max_phy1 = wmode_tmp;

			BB_DBG(bb_in, DBG_DBCC, "wmode_max[1] = 0x%x\n", wmode_max_phy1);
		}
	}

	/*MLO usage for BE series can still enable cfo tracking due to same connectivity source*/
#if defined(BB_8922A_SUPPORT)
	if (bb_in->ic_type == BB_RTL8922A)
		return;
#endif

	if (wmode_max_phy1 > wmode_max_phy0) {
		BB_DBG(bb_in, DBG_DBCC, "Disable Phy[0] CFO_TRK\n");
		halbb_pause_func(bb_0, F_CFO_TRK, HALBB_PAUSE_NO_SET, HALBB_PAUSE_LV_2, 1, &val, HW_PHY_0);
		halbb_pause_func(bb_1, F_CFO_TRK, HALBB_RESUME_NO_RECOVERY, HALBB_PAUSE_LV_2, 1, &val, HW_PHY_1);

	} else if (wmode_max_phy0 > wmode_max_phy1) {
		BB_DBG(bb_in, DBG_DBCC, "Disable Phy[1] CFO_TRK\n");
		halbb_pause_func(bb_0, F_CFO_TRK, HALBB_RESUME_NO_RECOVERY, HALBB_PAUSE_LV_2, 1, &val, HW_PHY_0);
		halbb_pause_func(bb_1, F_CFO_TRK, HALBB_PAUSE_NO_SET, HALBB_PAUSE_LV_2, 1, &val, HW_PHY_1);
	} else {
		BB_DBG(bb_in, DBG_DBCC, "Disable Phy[0 & 1] CFO_TRK\n");
		halbb_pause_func(bb_0, F_CFO_TRK, HALBB_PAUSE_NO_SET, HALBB_PAUSE_LV_2, 1, &val, HW_PHY_0);
		halbb_pause_func(bb_1, F_CFO_TRK, HALBB_PAUSE_NO_SET, HALBB_PAUSE_LV_2, 1, &val, HW_PHY_1);
	}
}

bool halbb_cfg_dbcc(struct bb_info *bb, struct bb_dbcc_cfg_info *cfg)
{
	struct bb_dbcc_info	*dbcc = &bb->bb_dbcc_i;
	u32 val = 0;

	if (!bb->bb_cmn_hooker->ic_dbcc_support)
		return false;

	BB_DBG(bb, DBG_DBCC, "[%s] en=%d, cck_phy_map=%d\n", __func__,
	       cfg->dbcc_en, cfg->cck_phy_map);

	switch (bb->ic_type) {

	#ifdef BB_8852A_2_SUPPORT
	case BB_RTL8852A:
		halbb_cfg_dbcc_8852a_2(bb, cfg);
		break;
	#endif
	#ifdef BB_8852C_SUPPORT
	case BB_RTL8852C:
		halbb_cfg_dbcc_8852c(bb, cfg);
		break;
	#endif
	default:
		break;
	}

#ifdef HALBB_FW_OFLD_SUPPORT
	halbb_fwofld_bitmap_en(bb, true, FW_OFLD_PHY_1_DM_INIT);
#endif
	
	if (cfg->dbcc_en) {
		halbb_dm_init_phy1(bb);

		BB_DBG(bb, DBG_DBCC, "Pause PHY 1 CFO_TRK\n");
		//halbb_pause_func(bb, F_CFO_TRK, HALBB_PAUSE_NO_SET, HALBB_PAUSE_LV_2, 1, &val, HW_PHY_0);
		halbb_pause_func(bb->bb_phy_hooker, F_CFO_TRK, HALBB_PAUSE_NO_SET, HALBB_PAUSE_LV_2, 1, &val, HW_PHY_1);
	} else {
		BB_DBG(bb, DBG_DBCC, "Resume PHY 0/1 CFO_TRK\n");
		halbb_pause_func(bb, F_CFO_TRK, HALBB_RESUME_NO_RECOVERY, HALBB_PAUSE_LV_2, 1, &val, HW_PHY_0);
		halbb_pause_func(bb->bb_phy_hooker, F_CFO_TRK, HALBB_RESUME_NO_RECOVERY, HALBB_PAUSE_LV_2, 1, &val, HW_PHY_1);
	}

#ifdef HALBB_FW_OFLD_SUPPORT
	halbb_fwofld_bitmap_en(bb, false, FW_OFLD_PHY_1_DM_INIT);
#endif

	return true;
}

struct bb_info *halbb_get_curr_bb_pointer(struct bb_info *bb,
					  enum phl_phy_idx phy_idx)
{
	if (bb->phl_com->dev_cap.dbcc_sup && phy_idx != bb->bb_phy_idx && bb->bb_phy_hooker) {
		BB_DBG(bb, DBG_DBCC, "[CVRT][en:%d] phy_idx (%d) -> (%d)\n",
		       bb->phl_com->dev_cap.dbcc_sup, bb->bb_phy_idx, phy_idx);
		return bb->bb_phy_hooker;
	} else {
		BB_DBG(bb, DBG_DBCC, "[STAY][en:%d] phy_idx (%d) -> (%d)\n",
		       bb->phl_com->dev_cap.dbcc_sup, bb->bb_phy_idx, phy_idx);
		return bb;
	}
}

u32
halbb_buffer_init_phy1(struct bb_info *bb_0)
{
	struct bb_info *bb_1 = NULL;

	BB_DBG(bb_0, DBG_INIT, "[%s]\n", __func__);

	if (!bb_0) {
		BB_WARNING("[%s]*bb_phy_0 = NULL\n", __func__);
		return RTW_HAL_STATUS_BB_INIT_FAILURE;
	}

	bb_1 = halbb_mem_alloc(bb_0, sizeof(struct bb_info));

	if (!bb_1) {
		BB_WARNING("[%s]*bb_phy_1 = NULL\n", __func__);
		return RTW_HAL_STATUS_BB_INIT_FAILURE;
	}
	/*Hook Link*/
	bb_0->bb_phy_hooker = bb_1;
	bb_1->bb_phy_hooker = bb_0;

	bb_1->bb_phy_idx = HW_PHY_1;
	bb_1->bb_cmn_hooker = bb_0->bb_cmn_hooker;

	bb_1->phl_com = bb_0->phl_com;/*shared memory for all components*/
	bb_1->hal_com = bb_0->hal_com;/*shared memory for phl and hal*/

	halbb_hw_init(bb_1);

#if 0
	BB_DBG(bb_0, DBG_DBCC, "phy_idx[0,1]={%d, %d}\n", bb_0->bb_phy_idx, bb_1->bb_phy_idx);
	BB_DBG(bb_1, DBG_DBCC, "phy_idx[0,1]={%d, %d}\n", bb_0->bb_phy_idx, bb_1->bb_phy_idx);

	BB_DBG(bb_0, DBG_DBCC, "phy_idx[0,1]={%d, %d}\n", bb_1->bb_phy_hooker->bb_phy_idx, bb_0->bb_phy_hooker->bb_phy_idx);
	BB_DBG(bb_1, DBG_DBCC, "phy_idx[0,1]={%d, %d}\n", bb_1->bb_phy_hooker->bb_phy_idx, bb_0->bb_phy_hooker->bb_phy_idx);
#endif

	return RTW_HAL_STATUS_SUCCESS;
}

void halbb_dbcc_early_init(struct bb_info *bb)
{
	bool drv_fw_ofld = false, drv_dbcc_fw_ofld = false;
	bool halbb_fw_ofld = false;
	bool halbb_dbcc_fw_ofld = false;
	bool halbb_normal_fw_ofld = false;

	BB_DBG(bb, DBG_DBCC, "IC_dbcc_support=%d\n", bb->bb_cmn_hooker->ic_dbcc_support);

	#ifdef CONFIG_FW_IO_OFLD_SUPPORT
	drv_fw_ofld = true;
	#endif

	#ifdef CONFIG_FW_DBCC_OFLD_SUPPORT
	drv_dbcc_fw_ofld = true;
	#endif

	#ifdef HALBB_FW_OFLD_SUPPORT
	halbb_fw_ofld = true;
	#endif

	#ifdef HALBB_FW_NORMAL_OFLD_SUPPORT
	halbb_normal_fw_ofld = true;
	#endif

	#ifdef HALBB_FW_DBCC_OFLD_SUPPORT
	halbb_dbcc_fw_ofld = true;
	#endif

	BB_DBG(bb, DBG_DBCC, "[Drv]   drv_fw_ofld =%d, drv_dbcc_fw_ofld=%d\n",
	       drv_fw_ofld, drv_dbcc_fw_ofld);
	BB_DBG(bb, DBG_DBCC, "[HALBB] halbb_fw_ofld=%d, halbb_normal_fw_ofld=%d, halbb_dbcc_fw_ofld=%d\n",
	       halbb_fw_ofld, halbb_normal_fw_ofld, halbb_dbcc_fw_ofld);
}


void halbb_dbcc_dbg(struct bb_info *bb, char input[][16], u32 *_used,
		       char *output, u32 *_out_len)
{
	struct bb_dbcc_cfg_info cfg;
	struct bb_info *bb_out;
	u32 val[10] = {0};
	u16 i = 0;

	if (_os_strcmp(input[1], "-h") == 0) {
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			 "phy {0/1}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			 "init\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			 "cck_map {dbcc_en} {cck_phy_map}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			 "ofld {fwofld_sup_bitmap}\n");
		return;
	}

	if (_os_strcmp(input[1], "phy") == 0) {
		HALBB_SCAN(input[2], DCMD_DECIMAL, &val[0]);
		if (val[0] == 1)
			bb->bb_cmn_hooker->bb_echo_cmd_i.echo_phy_idx = HW_PHY_1;
		else
			bb->bb_cmn_hooker->bb_echo_cmd_i.echo_phy_idx = HW_PHY_0;

		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "echo cmd convert to phy-%d mode\n",
			    bb->bb_cmn_hooker->bb_echo_cmd_i.echo_phy_idx);
	} else if (_os_strcmp(input[1], "cck_map") == 0) {
		HALBB_SCAN(input[2], DCMD_DECIMAL, &val[0]);
		HALBB_SCAN(input[3], DCMD_DECIMAL, &val[1]);

		cfg.dbcc_en = (bool)val[0];
		cfg.cck_phy_map = (enum phl_phy_idx)val[1];

		halbb_cfg_dbcc(bb, &cfg);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "dbcc_en=%d, cck_map=%d\n", cfg.dbcc_en, cfg.cck_phy_map);
#if 0
	} else if (_os_strcmp(input[1], "test1") == 0) {
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "[TEST_1]\n");
		
		HALBB_SCAN(input[2], DCMD_DECIMAL, &val[0]);

		BB_DBG(bb, DBG_DBCC, "[IN]bb->bb_phy_idx=%d, echo_phy_idx=%d\n",
		       bb->bb_phy_idx,
		       bb->bb_cmn_hooker->bb_echo_cmd_i.echo_phy_idx);

		HALBB_GET_PHY_PTR(bb, bb_out, (enum phl_phy_idx)val[0]);

		if (bb_out) {
			BB_DBG(bb, DBG_DBCC, "[OUT]bb_out->bb_phy_idx=%d, echo_phy_idx=%d\n",
			       bb_out->bb_phy_idx,
			       bb_out->bb_cmn_hooker->bb_echo_cmd_i.echo_phy_idx);
		} else {
			BB_DBG(bb, DBG_DBCC, "bb_out=NULL\n");
		}
#endif
	} else if (_os_strcmp(input[1], "test2") == 0) {
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "[TEST_2][DBG]phy_idx={%d}\n", bb->bb_phy_idx);

		BB_DBG(bb, DBG_DBCC, "[IN]phy_idx={%d}\n", bb->bb_phy_idx);
		BB_DBG(bb, DBG_DBCC, "phy0->phy1\n");
		HALBB_GET_PHY_PTR(bb, bb_out, HW_PHY_1);
		BB_DBG(bb, DBG_DBCC, "[OUT]phy_idx={%d}\n", bb_out->bb_phy_idx);

		bb = bb_out;
		BB_DBG(bb, DBG_DBCC, "[IN]phy_idx={%d}\n", bb->bb_phy_idx);
		BB_DBG(bb, DBG_DBCC, "phy1->phy1\n");
		HALBB_GET_PHY_PTR(bb, bb_out, HW_PHY_1); /*phy1->phy1*/
		BB_DBG(bb, DBG_DBCC, "[OUT]phy_idx={%d}\n", bb_out->bb_phy_idx);

		bb = bb_out;
		BB_DBG(bb, DBG_DBCC, "[IN]phy_idx={%d}\n", bb->bb_phy_idx);
		BB_DBG(bb, DBG_DBCC, "phy1->phy0\n");
		HALBB_GET_PHY_PTR(bb, bb_out, HW_PHY_0); /*phy1->phy0*/
		BB_DBG(bb, DBG_DBCC, "[OUT]phy_idx={%d}\n", bb_out->bb_phy_idx);

		bb = bb_out;
		BB_DBG(bb, DBG_DBCC, "[IN]phy_idx={%d}\n", bb->bb_phy_idx);
		BB_DBG(bb, DBG_DBCC, "phy0->phy0\n");
		HALBB_GET_PHY_PTR(bb, bb_out, HW_PHY_0); /*phy0->phy0*/
		BB_DBG(bb, DBG_DBCC, "[OUT]phy_idx={%d}\n", bb_out->bb_phy_idx);

		bb = bb_out;
		BB_DBG(bb, DBG_DBCC, "[IN]phy_idx={%d}\n", bb->bb_phy_idx);
		BB_DBG(bb, DBG_DBCC, "phy0->phy1\n");
		HALBB_GET_PHY_PTR(bb, bb_out, HW_PHY_1); /*phy0->phy1*/
		BB_DBG(bb, DBG_DBCC, "[OUT]phy_idx={%d}\n", bb_out->bb_phy_idx);
	} else if (_os_strcmp(input[1], "test3") == 0) {
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "[TEST_3][DBG]phy_idx={%d}\n", bb->bb_phy_idx);

		BB_DBG(bb, DBG_DBCC, "phy0->phy1\n");
		bb = halbb_get_curr_bb_pointer(bb, HW_PHY_1); /*phy0->phy1*/
		BB_DBG(bb, DBG_DBCC, "phy1->phy1\n");
		bb = halbb_get_curr_bb_pointer(bb, HW_PHY_1); /*phy1->phy1*/
		BB_DBG(bb, DBG_DBCC, "phy1->phy0\n");
		bb = halbb_get_curr_bb_pointer(bb, HW_PHY_0); /*phy1->phy0*/
		BB_DBG(bb, DBG_DBCC, "phy0->phy0\n");
		bb = halbb_get_curr_bb_pointer(bb, HW_PHY_0); /*phy0->phy0*/
		BB_DBG(bb, DBG_DBCC, "phy0->phy1\n");
		bb = halbb_get_curr_bb_pointer(bb, HW_PHY_1); /*phy0->phy1*/
	}
}

#endif




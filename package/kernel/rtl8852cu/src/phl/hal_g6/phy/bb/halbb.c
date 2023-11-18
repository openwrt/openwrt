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

void halbb_supportability_dbg(struct bb_info *bb, char input[][16], u32 *_used,
			     char *output, u32 *_out_len)
{
	u32 val[10] = {0};
	u64 pre_support_ability, one = 1;
	u64 comp = 0;
	u32 used = *_used;
	u32 out_len = *_out_len;
	u8 i;

	for (i = 0; i < 5; i++) {
		if (input[i + 1])
			HALBB_SCAN(input[i + 1], DCMD_DECIMAL, &val[i]);
	}

	pre_support_ability = bb->support_ability;
	comp = bb->support_ability;

	BB_DBG_CNSL(out_len, used, output + used, out_len - used,
		 "\n================================\n");

	if (val[0] == 100) {
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "[Supportability] Selection\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "================================\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "00. (( %s ))RA\n",
			 ((comp & BB_RA) ? ("V") : (".")));
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "01. (( %s ))FA_CNT\n",
			 ((comp & BB_FA_CNT) ? ("V") : (".")));
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "02. (( %s ))RSSI_MNTR\n",
			 ((comp & HALBB_FUN_RSVD_2) ? ("V") : (".")));
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "03. (( %s ))DFS\n",
			 ((comp & BB_DFS) ? ("V") : (".")));
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "04. (( %s ))EDCCA\n",
			 ((comp & BB_EDCCA) ? ("V") : (".")));
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "05. (( %s ))ENV_MNTR\n",
			 ((comp & BB_ENVMNTR) ? ("V") : (".")));
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "06. (( %s ))CFO_TRK\n",
			 ((comp & BB_CFO_TRK) ? ("V") : (".")));
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "07. (( %s ))PWR_CTRL\n",
			 ((comp & BB_PWR_CTRL) ? ("V") : (".")));
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "10. (( %s ))ANT_DIV\n",
			 ((comp & DBG_ANT_DIV) ? ("V") : (".")));
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "11. (( %s ))DIG\n",
			 ((comp & BB_DIG) ? ("V") : (".")));
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "12. (( %s ))PATH_DIV\n",
			 ((comp & DBG_PATH_DIV) ? ("V") : (".")));
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "13. (( %s ))UL_TB_CTRL\n",
			 ((comp & BB_UL_TB_CTRL) ? ("V") : (".")));

		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "31. (( %s ))Dyn CSI RSP\n",
			 ((comp & BB_DCR) ? ("V") : (".")));
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "================================\n");

	} else if (val[0] == 101) {
		bb->support_ability = 0;
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "Disable all support_ability components\n");
	} else {
		if (val[1] == 1) { /* @enable */
			bb->support_ability |= (one << val[0]);
		} else if (val[1] == 2) {/* @disable */
			bb->support_ability &= ~(one << val[0]);
		} else {
			BB_DBG_CNSL(out_len, used, output + used, out_len - used,
				 "[Warning!!!]  1:enable,  2:disable\n");
		}
	}
	BB_DBG_CNSL(out_len, used, output + used, out_len - used,
		 "pre-supportability = 0x%llx\n", pre_support_ability);
	BB_DBG_CNSL(out_len, used, output + used, out_len - used,
		 "Cur-supportability = 0x%llx\n", bb->support_ability);
	BB_DBG_CNSL(out_len, used, output + used, out_len - used,
		 "================================\n");

	*_used = used;
	*_out_len = out_len;
}

bool halbb_sta_info_init(struct bb_info *bb,
			 struct rtw_phl_stainfo_t *phl_sta_info)
{
	struct bb_sta_info *bb_sta;

	if (!bb) {
		BB_WARNING("[%s]*bb = NULL\n", __func__);
		return false;
	}

	if (!phl_sta_info)
		return false;

	if (!phl_sta_info->hal_sta)
		return false;

	bb_sta = halbb_mem_alloc(bb, sizeof(struct bb_sta_info));

	if (!bb_sta) {
		BB_WARNING("*bb_sta = NULL\n");
		return RTW_HAL_STATUS_BB_INIT_FAILURE;
	}
	phl_sta_info->hal_sta->bb_sta = (void *)bb_sta;

	return true;
}

bool halbb_sta_info_deinit(struct bb_info *bb,
			   struct rtw_phl_stainfo_t *phl_sta_info)
{
	if (!bb) {
		BB_WARNING("*bb = NULL\n");
		return false;
	}

	BB_DBG(bb, DBG_CONNECT, "[%s]\n", __func__);

	if (!phl_sta_info)
		return false;

	if (!phl_sta_info->hal_sta)
		return false;

	if (!phl_sta_info->hal_sta->bb_sta)
		return false;

	halbb_mem_free(bb, phl_sta_info->hal_sta->bb_sta, sizeof(struct bb_sta_info));
	return true;
}

bool halbb_sta_info_add_entry(struct bb_info *bb,
				      struct rtw_phl_stainfo_t *phl_sta_info)
{
	#ifdef HALBB_DBCC_SUPPORT
	struct bb_info *bb_1;
	#endif

	if (!bb) {
		BB_WARNING("[%s]*bb = NULL\n", __func__);
		return false;
	}

	BB_DBG(bb, DBG_CONNECT, "[%s]\n", __func__);

	if (!phl_sta_info)
		return false;

	if ((phl_sta_info->macid) >= PHL_MAX_STA_NUM)
		return false;

	BB_DBG(bb, DBG_CONNECT, "[%s] macid=%d\n", __func__, phl_sta_info->macid);

	bb->phl2bb_macid_table[phl_sta_info->macid] = (u8)phl_sta_info->macid;
	bb->phl_sta_info[phl_sta_info->macid] = phl_sta_info;

	#ifdef HALBB_DBCC_SUPPORT
	HALBB_GET_PHY_PTR(bb, bb_1, HW_PHY_1);
	bb_1->phl2bb_macid_table[phl_sta_info->macid] = (u8)phl_sta_info->macid;
	bb_1->phl_sta_info[phl_sta_info->macid] = phl_sta_info;
	#endif

	return true;
}

bool halbb_sta_info_delete_entry(struct bb_info *bb,
					struct rtw_phl_stainfo_t *phl_sta_info)
{
	#ifdef HALBB_DBCC_SUPPORT
	struct bb_info *bb_1;
	#endif

	BB_DBG(bb, DBG_CONNECT, "[%s]\n", __func__);

	if (!bb) {
		BB_WARNING("*bb = NULL\n");
		return false;
	}

	if (!phl_sta_info)
		return false;

	if ((phl_sta_info->macid) >= PHL_MAX_STA_NUM)
		return false;

	if (!phl_sta_info->hal_sta)
			return false;

	if (!phl_sta_info->hal_sta->bb_sta)
		return false;

	BB_DBG(bb, DBG_CONNECT, "[%s] macid=%d\n", __func__, phl_sta_info->macid);

	bb->sta_exist[phl_sta_info->macid] = false;
	bb->phl_sta_info[phl_sta_info->macid] = NULL;

	#ifdef HALBB_DBCC_SUPPORT
	HALBB_GET_PHY_PTR(bb, bb_1, HW_PHY_1);
	bb_1->sta_exist[phl_sta_info->macid] = false;
	bb_1->phl_sta_info[phl_sta_info->macid] = NULL;
	#endif

	return true;
}

void halbb_media_status_update(struct bb_info *bb_0,
			       struct rtw_phl_stainfo_t *phl_sta_info,
			       bool is_connected)
{
	struct bb_info *bb = bb_0;

#ifdef HALBB_DBCC_SUPPORT
	if (phl_sta_info->rlink->hw_band == HW_BAND_0) {
		bb_0->sta_exist[phl_sta_info->macid] = is_connected;
	} else if (phl_sta_info->rlink->hw_band == HW_BAND_1) {
		HALBB_GET_PHY_PTR(bb_0, bb, HW_PHY_1);
		bb->sta_exist[phl_sta_info->macid] = is_connected;
	} else {
		BB_WARNING("[%s]\n", __func__);
	}

	BB_DBG(bb, DBG_CONNECT, "[%s]phy_idx=%d, macid=%d, connect=%d\n", __func__,
	       phl_sta_info->rlink->hw_band, phl_sta_info->macid, is_connected);

	halbb_cfo_trk_joint_phy_dec(bb);
#else
	bb->sta_exist[phl_sta_info->macid] = is_connected;
#endif

	/*Reset MA RSSI*/
	if (!is_connected) {
		phl_sta_info->hal_sta->rssi_stat.rssi = 0;
		phl_sta_info->hal_sta->rssi_stat.rssi_ma = 0;
		phl_sta_info->hal_sta->rssi_stat.rssi_ma_path[0] = 0;
		phl_sta_info->hal_sta->rssi_stat.rssi_ma_path[1] = 0;
		phl_sta_info->hal_sta->rssi_stat.rssi_ma_path[2] = 0;
		phl_sta_info->hal_sta->rssi_stat.rssi_ma_path[3] = 0;
		phl_sta_info->hal_sta->rssi_stat.pkt_cnt_data = 0;
		phl_sta_info->hal_sta->rssi_stat.rssi_bcn = 0;
		phl_sta_info->hal_sta->rssi_stat.rssi_bcn_ma = 0;
		phl_sta_info->hal_sta->rssi_stat.rssi_bcn_ma_path[0] = 0;
		phl_sta_info->hal_sta->rssi_stat.rssi_bcn_ma_path[1] = 0;
		phl_sta_info->hal_sta->rssi_stat.rssi_bcn_ma_path[2] = 0;
		phl_sta_info->hal_sta->rssi_stat.rssi_bcn_ma_path[3] = 0;
		phl_sta_info->hal_sta->rssi_stat.pkt_cnt_bcn = 0;
		phl_sta_info->hal_sta->rssi_stat.rssi_ofdm = 0;
		phl_sta_info->hal_sta->rssi_stat.rssi_cck = 0;
		phl_sta_info->hal_sta->rssi_stat.snr_ma = 0;

		if (bb->bb_sta_cnt > 0)
			bb->bb_sta_cnt--;
	} else {
		phl_sta_info->hal_sta->rssi_stat.ma_factor = RSSI_MA_L;
		phl_sta_info->hal_sta->rssi_stat.ma_factor_bcn = RSSI_MA_L;
		bb->bb_sta_cnt++;
		#ifdef HALBB_UL_TB_CTRL_SUPPORT
		halbb_ul_tb_chk(bb);
		#endif
	}
}

void halbb_sta_info_dbg(struct bb_info *bb, char input[][16], u32 *_used,
			char *output, u32 *_out_len)
{
	struct rtw_hal_com_t *hal = bb->hal_com;
	struct rtw_phl_stainfo_t *phl_sta;
	struct rtw_rssi_info *rssi_t = NULL;
	struct rtw_ra_sta_info	*ra;
	char dbg_buf[HALBB_SNPRINT_SIZE] = {0};
	u32 val[10] = {0};
	u32 tmp = 0;
	u16 curr_tx_rt = 0;
	u8 i = 0, j = 0;
	enum phl_phy_idx phy_idx = HW_PHY_0;
	#if 0 /*wait for phl mu ra declaration*/
	struct rtw_phl_com_t *phl = bb->phl_com;
	struct dev_cap_t *dev = &phl->dev_cap;
	u8 u1_muidx = 0;
	#endif
#ifdef HALBB_DBCC_SUPPORT
	struct bb_info *bb_other;
	u32 val_tmp;
#endif

	if (_os_strcmp(input[1], "-h") == 0) {
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			 "all\n");
		return;
	}

	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "PHY[0] Fc_CH = %03d/%03d, is_2g=%d\n",
		    bb->hal_com->band[0].cur_chandef.center_ch,
		    bb->bb_ch_i.fc_ch_idx,
		    bb->bb_ch_i.is_2g);

#ifdef HALBB_DBCC_SUPPORT
	if (bb->bb_phy_idx == HW_PHY_0) {
		HALBB_GET_PHY_PTR(bb, bb_other, HW_PHY_1);
	} else {
		HALBB_GET_PHY_PTR(bb, bb_other, HW_PHY_0);
	}

	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "PHY[1] Fc_CH = %03d/%03d, is_2g=%d\n",
		    bb_other->hal_com->band[1].cur_chandef.center_ch,
		    bb_other->bb_ch_i.fc_ch_idx,
		    bb_other->bb_ch_i.is_2g);

	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "DBCC_en=%d/%d, DBCC_sup=%d,cck_blk_en=%d, cck_phy_map=%d\n",
		    bb->hal_com->dbcc_en, bb->bb_cmn_hooker->bb_dbcc_en,
		    bb->phl_com->dev_cap.dbcc_sup,
		    bb->bb_cmn_hooker->cck_blk_en, bb->bb_cmn_hooker->cck_phy_map);
	val_tmp = halbb_get_reg(bb, 0x4970, 0x3);
	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "0x4970[1:0]=0x%x\n", val_tmp);

	val_tmp = halbb_get_reg(bb, 0x2344, BIT31);
	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "0x2344[31] Disable_CCK=0x%x \n", val_tmp);

	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "[Drv STA_cnt] all(%d) = phy0(%d) + phy1(%d)\n",
		    hal->assoc_sta_cnt,
		    bb->hal_com->band[0].assoc_sta_cnt,
		    bb->hal_com->band[1].assoc_sta_cnt);
	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "[BB STA_cnt]  all(%d) = phy%d(%d) + phy%d(%d)\n",
		    bb->bb_sta_cnt + bb_other->bb_sta_cnt,
		    bb->bb_phy_idx, bb->bb_sta_cnt,
		    bb_other->bb_phy_idx, bb_other->bb_sta_cnt);
	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "[RSSI_min]    phy%d(%d) + phy%d(%d)\n\n",
		    bb->bb_phy_idx, bb->bb_ch_i.rssi_min >> 1,
		    bb_other->bb_phy_idx, bb_other->bb_ch_i.rssi_min >> 1);
#else
	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "Drv STA_cnt(all)=%d\n\n", hal->assoc_sta_cnt);
	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "[RSSI_min]  %d\n\n",
		    bb->bb_ch_i.rssi_min >> 1);
#endif
	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "======================================\n");

	if (_os_strcmp(input[1], "all") == 0) {
		for (i = 0; i < PHL_MAX_STA_NUM; i++) {
			if (bb->sta_exist[i]) {
				phl_sta = bb->phl_sta_info[i];
				phy_idx = bb->bb_phy_idx;
			}
			#ifdef HALBB_DBCC_SUPPORT
			else if (bb_other->sta_exist[i]) {
				phl_sta = bb_other->phl_sta_info[i];
				phy_idx = bb_other->bb_phy_idx;
			}
			#endif
			else {
				continue;
			}

			if (!is_sta_active(phl_sta))
				continue;

			BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
				    "==[ID:%d/%d][Phy:%d/%d] Active=%d==\n\n",
				    i, phl_sta->macid, phy_idx, phl_sta->rlink->hw_band, phl_sta->active);
			rssi_t = &phl_sta->hal_sta->rssi_stat;

			halbb_print_sign_frac_digit(bb, rssi_t->rssi_ma, 16, 5, dbg_buf, HALBB_SNPRINT_SIZE_S);
			BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
				    "  [Data] rssi_avg=%s, MA=1/%02d\n",
				    dbg_buf, 1 << rssi_t->ma_factor);

			for (j = 0; j < HALBB_MAX_PATH; j++) {
				halbb_print_sign_frac_digit(bb, rssi_t->rssi_ma_path[j], 16, 5, dbg_buf, HALBB_SNPRINT_SIZE_S);
				BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
					    "         rssi[%d]= %s\n", j, dbg_buf);
			}

			halbb_print_sign_frac_digit(bb, rssi_t->rssi_bcn_ma, 16, 5, dbg_buf, HALBB_SNPRINT_SIZE_S);
			BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
				    "  [Bcn]  rssi_avg=%s, MA=1/%02d\n",
				    dbg_buf, 1 << rssi_t->ma_factor_bcn);


			for (j = 0; j < HALBB_MAX_PATH; j++) {
				halbb_print_sign_frac_digit(bb, rssi_t->rssi_bcn_ma_path[j], 16, 5, dbg_buf, HALBB_SNPRINT_SIZE_S);
				BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
					    "       rssi[%d]= %s\n", j, dbg_buf);
			}

			BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
				    "  rssi_cck=%02d.%d, rssi_ofdm=%02d.%d\n",
				    rssi_t->rssi_cck >> 1, (rssi_t->rssi_cck & 1) * 5,
				    rssi_t->rssi_ofdm >> 1, (rssi_t->rssi_ofdm & 1) * 5);

			halbb_print_sign_frac_digit(bb, rssi_t->snr_ma, 16, 4, dbg_buf, HALBB_SNPRINT_SIZE_S);
			BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
				    "  SNR_avg=%s dB\n", dbg_buf);
			halbb_print_sign_frac_digit(bb, rssi_t->snr_ma_path[0], 16, 4, dbg_buf, HALBB_SNPRINT_SIZE_S);
			BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
				    "    *SNR[0]=%s dB\n", dbg_buf);
			halbb_print_sign_frac_digit(bb, rssi_t->snr_ma_path[1], 16, 4, dbg_buf, HALBB_SNPRINT_SIZE_S);
			BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
				    "    *SNR[1]=%s dB\n\n", dbg_buf);

			ra = &phl_sta->hal_sta->ra_info;
			if (bb->ic_type & BB_IC_AX_SERIES)
				curr_tx_rt = (u16)(ra->rpt_rt_i.mcs_ss_idx & 0x7f) | ((u16)(ra->rpt_rt_i.mode & 0x3) << 7);
			else
				curr_tx_rt = (u16)(ra->rpt_rt_i.mcs_ss_idx & 0xff) | ((u16)(ra->rpt_rt_i.mode & 0xf) << 8);

			halbb_print_rate_2_buff(bb, curr_tx_rt, ra->rpt_rt_i.gi_ltf, bb->dbg_buf, HALBB_SNPRINT_SIZE);
			BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
				    "  [Tx] Rate:(%s) (0x%x)\n",
				    bb->dbg_buf, curr_tx_rt);
			BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
				    "  [Tx] PER=(%d), BW=(%d)\n",
				    ra->curr_retry_ratio, (20 << ra->rpt_rt_i.bw));

			#if 0 /*wait for phl mu ra declaration*/
			if (dev->rfe_type >= 50) {
				for (j = 0; j < MAX_MU_STA_NUM - 1; j++) {
					if (ra->mu_active[j] == false)
						continue;

					curr_tx_rt = (u16)(ra->rpt_rt_i.mu_mcs_ss_idx[j]) | ((u16)(ra->rpt_rt_i.mu_mode[j]) << 7);
					u1_muidx = (j >= ra->mu_idx) ? j + 1 : j;

					halbb_print_rate_2_buff(bb, curr_tx_rt, ra->rpt_rt_i.mu_gi_ltf[j], bb->dbg_buf, HALBB_SNPRINT_SIZE);
					BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
						    "MU : muidx{u0,u1} = {%d, %d}, Tx_Rate=%s (0x%x-%d), PER=(%d), TXBW=(%d)\n",
						    ra->mu_idx, u1_muidx,
						    bb->dbg_buf, curr_tx_rt,
						    ra->rpt_rt_i.mu_gi_ltf[j],
						    ra->mu_curr_retry_ratio[j],
						    (20 << ra->rpt_rt_i.mu_bw[j]));
				}
			}
			#endif

			BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
				    "======================================\n");

		}
	}
}

void halbb_traffic_load_decision(struct bb_info *bb)
{
	struct rtw_stats *stat = &bb->phl_com->phl_stats;
	struct bb_link_info *link = &bb->bb_link_i;
	u32 max_tp; /*Mbps*/

	/*@---TP & Trafic-load caln---*/
	link->tx_tp = KB_2_MB(stat->tx_tp_kbits);
	link->rx_tp = KB_2_MB(stat->rx_tp_kbits);
	link->total_tp = link->tx_tp + link->rx_tp;
	max_tp = MAX_2(link->tx_tp, link->rx_tp); /*Mbps*/

	BB_DBG(bb, DBG_COMMON_FLOW,
		"byte_uni{tx,rx}={%llu,%llu}, byte_total{tx,rx}={%llu,%llu}\n",
		stat->tx_byte_uni, stat->rx_byte_uni,
		stat->tx_byte_total, stat->rx_byte_total);

	BB_DBG(bb, DBG_COMMON_FLOW,
		"TP_kbit{tx,rx}={%d,%d}, TP_MA{tx,rx}={%d,%d}\n",
		stat->tx_tp_kbits, stat->rx_tp_kbits,
		stat->tx_moving_average_tp, stat->rx_moving_average_tp);

	/*@[Calculate TX/RX state]*/
	if (link->tx_tp > (link->rx_tp << 1))
		link->txrx_state_all = BB_TX_STATE;
	else if (link->rx_tp > (link->tx_tp << 1))
		link->txrx_state_all = BB_RX_STATE;
	else
		link->txrx_state_all = BB_BI_DIR_STATE;

	/*@[Traffic load decision]*/
	link->traffic_load_pre = link->traffic_load;

	if (max_tp > 20) {
		link->traffic_load = TRAFFIC_HIGH;
	} else if (max_tp > 5) {
		link->traffic_load = TRAFFIC_MID;
	} else if (max_tp > 1) {
		link->traffic_load = TRAFFIC_LOW;
	} else if (stat->tx_tp_kbits > 100 || stat->rx_tp_kbits > 100) { /*100Kb*/
		link->traffic_load = TRAFFIC_ULTRA_LOW;
	} else {
		link->traffic_load = TRAFFIC_NO_TP;
	}

	/*@[Calculate consecutive idlel time]*/
	if (link->traffic_load == TRAFFIC_NO_TP)
		link->consecutive_idle_time = 0;
	else
		link->consecutive_idle_time += HALBB_WATCHDOG_PERIOD;
}

void halbb_cmn_info_self_reset(struct bb_info *bb) {

	struct bb_link_info *link = &bb->bb_link_i;

	bb->bb_ch_i.rssi_max = 0;
	bb->bb_ch_i.rssi_min = 0;

	link->is_one_entry_only = false;
	link->one_entry_macid = 0;
	link->one_entry_tp = 0;
	link->one_entry_tp_active_occur = false;
	link->one_entry_tp_pre = 0;
	link->num_linked_client_pre = 0;
	link->num_active_client_pre = 0;
	link->num_linked_client = 0;
	link->num_active_client = 0;
}

u8 halbb_get_rssi_min(struct bb_info *bb)
{
	struct rtw_hal_com_t *hal = bb->hal_com;
	struct rtw_phl_stainfo_t *sta;
	struct rtw_rssi_info *sta_rssi = NULL;
	u8 sta_cnt = 0;
	u8 rssi_min = 0xff, rssi_curr = 0;
	u32 i = 0;

	if (hal->assoc_sta_cnt == 0) {
		BB_WARNING("[%s] assoc_sta_cnt=0\n", __func__);
		return 0;
	}

	for (i = 0; i < PHL_MAX_STA_NUM; i++) {
		if (!bb->sta_exist[i])
			continue;

		sta = bb->phl_sta_info[i];

		if (!is_sta_active(sta))
			continue;

		BB_DBG(bb, DBG_COMMON_FLOW, "[%d] macid=%d\n", i, sta->macid);

		sta_cnt++;

		sta_rssi = &sta->hal_sta->rssi_stat;

		if (sta_rssi->rssi != 0)
			rssi_curr = sta_rssi->rssi;
		else
			rssi_curr = sta_rssi->rssi_bcn;

		// Fix RSSI min = 0
		if (rssi_curr == 0)
			rssi_curr = sta_rssi->assoc_rssi << 1;

		/*[RSSI min]*/
		if (rssi_curr <= rssi_min) {
			rssi_min = rssi_curr;
		}

		BB_DBG(bb, DBG_COMMON_FLOW,
		       "rssi_min = %d", rssi_min);

		if (sta_cnt >= hal->assoc_sta_cnt)
			break;
	}

	if (sta_cnt == 0) {
		BB_WARNING("[%s] sta_cnt=0\n", __func__);
		return 0;
	}

	return rssi_min;
}

void halbb_cmn_info_self_update(struct bb_info *bb)
{
	struct rtw_hal_com_t *hal = bb->hal_com;
	struct bb_link_info *link = &bb->bb_link_i;
	struct rtw_phl_com_t *phl = bb->phl_com;
	struct dev_cap_t *dev = &phl->dev_cap;
	struct rtw_phl_stainfo_t *sta;
	struct rtw_rssi_info *sta_rssi = NULL;
	struct bb_ch_info *ch = &bb->bb_ch_i;
#ifdef HALBB_DIG_MCC_SUPPORT
	struct halbb_mcc_dm *mcc_dm = &bb->mcc_dm;
	u8 mcc_rssi_min[MCC_BAND_NUM], mcc_sta_cnt[MCC_BAND_NUM];
	u8 j = 0, band_idx = MCC_BAND_NUM, role_ch = 0;
#endif
	u8 sta_cnt = 0, num_active_client = 0;
	u8 rssi_min = 0xff, rssi_max = 0, rssi_curr = 0;
	u16 wlan_mode_all = 0;
	u32 i = 0, one_entry_macid_tmp = 0;
	u32 trx_tp = 0;
	u32 tp_diff = 0;
	u8 per_phy_sta_cnt = 0;

#ifdef HALBB_DBCC_SUPPORT
	if (bb->hal_com->dbcc_en)
		per_phy_sta_cnt = hal->band[bb->bb_phy_idx].assoc_sta_cnt;
	else
#endif
		per_phy_sta_cnt = hal->assoc_sta_cnt;

	/*[Link Status Check]*/
	link->is_linked = (per_phy_sta_cnt != 0) ? true : false;
	link->first_connect = link->is_linked && !link->is_linked_pre;
	link->first_disconnect = !link->is_linked && link->is_linked_pre;
	link->is_linked_pre = link->is_linked;

	BB_DBG(bb, DBG_COMMON_FLOW, "phy_idx=%d, is_linked = %d, 1st_connect=%d, 1st_disconnect=%d, assoc_sta_cnt=%d\n",
	       bb->bb_phy_idx, link->is_linked, link->first_connect,
	       link->first_disconnect, per_phy_sta_cnt);

	/*[Traffic load information]*/
	halbb_traffic_load_decision(bb);

	link->rx_rate_plurality = halbb_get_plurality_rx_rate_su(bb);
	link->rx_rate_plurality_mu = halbb_get_plurality_rx_rate_mu(bb);

	if (!link->is_linked) {
		if (link->first_disconnect)
			halbb_cmn_info_self_reset(bb);

		return;
	}
#ifdef HALBB_DIG_MCC_SUPPORT
	if (mcc_dm->mcc_status_en) {
		for (i = 0; i < MCC_BAND_NUM; i++) {
			mcc_rssi_min[i] = rssi_min;
			mcc_sta_cnt[i] = 0;
		}
	}
#endif
	ch->pre_rssi_min = ch->rssi_min;

	for (i = 0; i < PHL_MAX_STA_NUM; i++) {
		if (!bb->sta_exist[i])
			continue;

		sta = bb->phl_sta_info[i];

		if (!is_sta_active(sta))
			continue;

		if ((dev->rfe_type >= 50) && (sta->macid == 0))
			continue;

		BB_DBG(bb, DBG_COMMON_FLOW, "[%d] macid=%d\n", i, sta->macid);

		sta_cnt++;

		if (sta_cnt == 1)
			one_entry_macid_tmp = i;

		trx_tp = KB_2_MB(sta->stats.tx_tp_kbits +
				 sta->stats.rx_tp_kbits); /*Mbit*/

		sta_rssi = &sta->hal_sta->rssi_stat;

		if (bb->bb_watchdog_mode != BB_WATCHDOG_NORMAL) {
			rssi_curr = sta_rssi->rssi_bcn;
		} else {
			if (sta_rssi->rssi == 0 && sta_rssi->rssi_bcn != 0)
				rssi_curr = sta_rssi->rssi_bcn;
			else
				rssi_curr = sta_rssi->rssi;
		}

		// Fix RSSI min = 0
		if (rssi_curr == 0)
			rssi_curr = sta_rssi->assoc_rssi << 1;

		if (sta_rssi->pkt_cnt_data > 100)
			sta_rssi->ma_factor = RSSI_MA_H;
		else if (sta_rssi->pkt_cnt_data > 20)
			sta_rssi->ma_factor = RSSI_MA_M;
		else if (sta_rssi->pkt_cnt_data > 5)
			sta_rssi->ma_factor = RSSI_MA_L;
		else
			sta_rssi->ma_factor = RSSI_MA_UL;

		if (sta_rssi->pkt_cnt_bcn > 5)
			sta_rssi->ma_factor_bcn = RSSI_MA_L;
		else
			sta_rssi->ma_factor_bcn = RSSI_MA_UL;

		BB_DBG(bb, DBG_COMMON_FLOW,
		       "pkt_cnt_data=%d, pkt_cnt_bcn=%d, ma_factor=%d, ma_factor_bcn=%d\n",
		       sta_rssi->pkt_cnt_data, sta_rssi->pkt_cnt_bcn,
		       sta_rssi->ma_factor, sta_rssi->ma_factor_bcn);

		sta_rssi->pkt_cnt_data = 0;
		sta_rssi->pkt_cnt_bcn = 0;

		BB_DBG(bb, DBG_COMMON_FLOW,
		       "new rssi u(8,1) = %d",
		       sta_rssi->rssi);

		/*[RSSI min]*/
		if (rssi_curr <= rssi_min) {
			ch->rssi_min = rssi_curr;
			ch->rssi_min_macid = sta->macid;
			rssi_min = rssi_curr;
			ch->int_rssi_min = SUBTRACT_TO_0(ch->rssi_min,
							 ch->ext_loss_avg / 2);
		}
		/*[RSSI max]*/
		if (rssi_curr >= rssi_max) {
			ch->rssi_max = rssi_curr;
			ch->rssi_max_macid = sta->macid;
			rssi_max = rssi_curr;
			ch->int_rssi_max = SUBTRACT_TO_0(ch->rssi_max,
							 ch->ext_loss_avg / 2);
		}

		//BB_DBG(bb, DBG_COMMON_FLOW, "TP: TRX=%d Mb/sec\n", trx_tp);

		//BB_DBG(bb, DBG_COMMON_FLOW, "TP: TX=%d, RX=%d, kb/sec\n",
		//       sta->stats.tx_tp_kbits, sta->stats.rx_tp_kbits);

		BB_DBG(bb, DBG_COMMON_FLOW,
		       "rssi_min = %d, rssi_max = %d", rssi_min, rssi_max);
#ifdef HALBB_DIG_MCC_SUPPORT
		if (mcc_dm->mcc_status_en) {
			if (i == mcc_dm->softap_macid)
				continue;

			band_idx = MCC_BAND_NUM;
			role_ch = sta->rlink->chandef.center_ch;

			for (j = 0; j < MCC_BAND_NUM; j++) {
				if (mcc_dm->mcc_rf_ch[j].center_ch == role_ch) {
					band_idx = j;
					break;
				}
			}

			if (band_idx == MCC_BAND_NUM) {
				BB_WARNING("%s, band_idx = %d", __func__,
					   band_idx);
				continue;
			}

			if (rssi_curr <= mcc_rssi_min[band_idx])
				mcc_rssi_min[band_idx] = rssi_curr;

			mcc_sta_cnt[band_idx]++;
		}
#endif
		if (trx_tp > ACTIVE_TP_THRESHOLD)
			num_active_client++;

		wlan_mode_all |= (u16)sta->wmode;

		if (sta_cnt >= per_phy_sta_cnt)
			break;
	}
#ifdef HALBB_DIG_MCC_SUPPORT
	if (mcc_dm->mcc_status_en) {
		for (i = 0; i < MCC_BAND_NUM; i++) {
			mcc_dm->rssi_min[i] = mcc_rssi_min[i];
			mcc_dm->sta_cnt[i] = mcc_sta_cnt[i];
		}
	}
#endif
	link->wlan_mode_bitmap = wlan_mode_all;

	link->is_one_entry_only = (per_phy_sta_cnt == 1) ? true : false;

	if (link->is_one_entry_only) {
		link->one_entry_macid = one_entry_macid_tmp;
		link->one_entry_tp = trx_tp;
		link->one_entry_tp_active_occur = false;

		//BB_DBG(bb, DBG_COMMON_FLOW, "one_entry_tp=((%d)), one_entry_tp_pre=((%d))\n",
		//	  link->one_entry_tp, link->one_entry_tp_pre);

		if (link->one_entry_tp > link->one_entry_tp_pre &&
		    link->one_entry_tp_pre <= 2) {
			tp_diff = link->one_entry_tp - link->one_entry_tp_pre;

			if (tp_diff > link->tp_active_th)
				link->one_entry_tp_active_occur = true;
		}

		link->one_entry_tp_pre = link->one_entry_tp;
	}

	link->num_linked_client_pre = link->num_linked_client;
	link->num_active_client_pre = link->num_active_client;
	link->num_linked_client = sta_cnt;
	link->num_active_client = num_active_client;
}

void halbb_watchdog_reset(struct bb_info *bb)
{

}

void halbb_update_hal_info(struct bb_info *bb)
{
	struct rtw_hal_com_t *hal = bb->hal_com;

	hal->trx_stat.rx_rate_plurality = bb->bb_link_i.rx_rate_plurality;
}

void halbb_store_data(struct bb_info *bb)
{
	halbb_cmn_info_rpt_store_data(bb);
}

void halbb_reset(struct bb_info *bb)
{
	if (bb->bb_cmn_hooker->bb_cmn_dbg_i.cmn_log_2_cnsl_en)
		return;

	if (bb->bb_cmn_hooker->bb_cmn_dbg_i.cmn_log_2_drv_statistic_en)
		return;

	halbb_store_data(bb);
	#ifdef HALBB_STATISTICS_SUPPORT
	halbb_statistics_reset(bb);
	#endif
	halbb_cmn_info_rpt_reset(bb);
}

void halbb_watchdog_normal(struct bb_info *bb, enum phl_phy_idx phy_idx)
{
	halbb_cmn_info_self_update(bb);
	halbb_ic_hw_setting(bb);
	#ifdef HALBB_ENV_MNTR_SUPPORT
	halbb_env_mntr(bb);
	#endif
	#ifdef HALBB_DIG_SUPPORT
	halbb_dig(bb);
	#endif
	#ifdef HALBB_STATISTICS_SUPPORT
	//halbb_statistics(bb);
	halbb_pmac_statistics(bb);
	#endif
	halbb_basic_dbg_message(bb);
	halbb_physts_watchdog(bb);

	if (!bb->adv_bb_dm_en) {
		BB_DBG(bb, DBG_COMMON_FLOW, "Disable adv halbb dm\n");
		halbb_reset(bb);
		return;
	}

	#ifdef HALBB_EDCCA_SUPPORT
	halbb_edcca(bb);
	#endif
	#ifdef HALBB_DFS_SUPPORT
	halbb_dfs(bb);
	#endif
	#ifdef HALBB_CFO_TRK_SUPPORT
	halbb_cfo_watchdog(bb);
	#endif
	#ifdef HALBB_UL_TB_CTRL_SUPPORT
	halbb_ul_tb_ctrl(bb);
	#endif
	#ifdef HALBB_RA_SUPPORT
	halbb_ra_watchdog(bb);
	#endif
	#ifdef HALBB_PWR_CTRL_SUPPORT
	halbb_pwr_ctrl(bb);
	#endif
	#ifdef HALBB_LA_MODE_SUPPORT
	halbb_la_re_trig_watchdog(bb);
	#endif
	#ifdef HALBB_ANT_DIV_SUPPORT
	halbb_antenna_diversity(bb);
	#endif
	#ifdef HALBB_PATH_DIV_SUPPORT
	halbb_path_diversity(bb);
	#endif
	halbb_update_hal_info(bb);
	#ifdef HALBB_DIG_MCC_SUPPORT
	halbb_mccdm_switch(bb);
	#endif
	/*[Rest all counter]*/
	halbb_reset(bb);
}

void halbb_watchdog_low_io(struct bb_info *bb, enum phl_phy_idx phy_idx)
{
	halbb_cmn_info_self_update(bb);
	halbb_ic_hw_setting_low_io(bb);
	halbb_basic_dbg_message(bb);
	#ifdef HALBB_DIG_SUPPORT
	halbb_dig_lps(bb);
	#endif

	#ifdef HALBB_RA_SUPPORT
	halbb_ra_watchdog(bb);
	#endif

	#if 0//def HALBB_EDCCA_SUPPORT
	halbb_edcca(bb);
	#endif

	#if 0//def HALBB_DFS_SUPPORT
	halbb_dfs(bb);
	#endif
	/*[Rest all counter]*/
	halbb_reset(bb);
}

void halbb_watchdog_non_io(struct bb_info *bb, enum phl_phy_idx phy_idx)
{
	halbb_cmn_info_self_update(bb);
	halbb_ic_hw_setting_non_io(bb);
	halbb_basic_dbg_message(bb);
	/*[Rest all counter]*/
	halbb_reset(bb);
}

void halbb_watchdog_mp(struct bb_info *bb, enum phl_phy_idx phy_idx)
{
	halbb_cmn_info_self_update(bb);
	halbb_basic_dbg_message(bb);
	halbb_physts_watchdog(bb);
	/*[Rest all counter]*/
	halbb_reset(bb);
}

void halbb_watchdog_dbcc(struct bb_info *bb)
{
#ifdef HALBB_DBCC_SUPPORT
	BB_DBG(bb, DBG_COMMON_FLOW, "[%s] phy_idx=%d\n", __func__,
	       bb->bb_phy_idx);

	halbb_cmn_info_self_update(bb);
	halbb_ic_hw_setting_dbcc(bb);

#ifdef HALBB_ENV_MNTR_SUPPORT
	halbb_env_mntr(bb);
#endif
#ifdef HALBB_DIG_SUPPORT
	halbb_dig_lps(bb);

#endif
#ifdef HALBB_STATISTICS_SUPPORT
	halbb_pmac_statistics(bb);
#endif
	halbb_basic_dbg_message(bb);

	halbb_physts_watchdog(bb);

#ifdef HALBB_CFO_TRK_SUPPORT
	halbb_cfo_watchdog(bb);
#endif
#ifdef HALBB_RA_SUPPORT
	halbb_ra_watchdog(bb);
#endif
	/*[Rest all counter]*/
	halbb_reset(bb);
#endif
}

void halbb_watchdog_per_phy(struct bb_info *bb_0, enum bb_watchdog_mode_t mode, enum phl_phy_idx phy_idx)
{
	struct bb_info *bb = bb_0;
	bool is_mp_mode = false;

#ifdef HALBB_DBCC_SUPPORT
	HALBB_GET_PHY_PTR(bb_0, bb, phy_idx);
	BB_DBG(bb, DBG_COMMON_FLOW, "[%s] phy_idx=%d\n", __func__, bb->bb_phy_idx);
#endif

	bb->bb_sys_up_time += BB_WATCH_DOG_PERIOD;

	if ((bb->bb_sys_up_time % bb->bb_watchdog_period) != 0)
		return;

	if (bb->bb_dbg_i.cr_recorder_en)
		BB_TRACE("[%s] up_time:%d \n", __func__, bb->bb_sys_up_time);

	bb->bb_watchdog_mode = mode;
	BB_DBG(bb, DBG_COMMON_FLOW, "BB_watchdog_mode = %s\n",
	       ((mode == BB_WATCHDOG_NORMAL) ? "Normal IO" :
	       ((mode == BB_WATCHDOG_LOW_IO) ? "Low IO" : "Non IO")));

	/*=== [HALBB Watchdog] ===============================================*/
	is_mp_mode = (bool)phl_is_mp_mode(bb->phl_com);

	if (is_mp_mode) {
		BB_DBG(bb, DBG_COMMON_FLOW, "MP mode=%d", bb->phl_com->drv_mode);

		if (!bb->is_mp_mode_pre)
			halbb_cvrt_2_mp(bb);

		halbb_watchdog_mp(bb, phy_idx);
#ifdef HALBB_DBCC_SUPPORT
	} else if (bb->hal_com->dbcc_en && bb->phl_com->dev_cap.dbcc_sup) {
		BB_DBG(bb, DBG_COMMON_FLOW, "dbcc_en=%d\n", bb->hal_com->dbcc_en);
		if (mode == BB_WATCHDOG_NORMAL)
			halbb_watchdog_dbcc(bb);
#endif
	} else {
		if (mode == BB_WATCHDOG_NORMAL) {
			halbb_watchdog_normal(bb, phy_idx);
		} else if (mode == BB_WATCHDOG_LOW_IO) {
			halbb_watchdog_low_io(bb, phy_idx);
		} else { /*if (mode == BB_WATCHDOG_NON_IO)*/
			halbb_watchdog_non_io(bb, phy_idx);
		}
	}

	bb->is_mp_mode_pre = is_mp_mode;

	if (bb->bb_dbg_i.cr_recorder_en)
		BB_TRACE("[%s] end\n", __func__);

	halbb_print_devider(bb, BB_DEVIDER_LEN_32, true, bb->dbg_component);
}

void halbb_watchdog(struct bb_info *bb_0, enum bb_watchdog_mode_t mode, enum phl_phy_idx phy_idx)
{
	halbb_watchdog_per_phy(bb_0, mode, HW_PHY_0);

#ifdef HALBB_DBCC_SUPPORT
	if (bb_0->hal_com->dbcc_en && bb_0->phl_com->dev_cap.dbcc_sup) {
		if (bb_0->hal_com->band[1].cur_chandef.center_ch == 0) {
			BB_WARNING("[%s] phy=1, fc=0\n", __func__);
			return;
		}

		halbb_watchdog_per_phy(bb_0, mode, HW_PHY_1);
	}
#endif
}

void halbb_new_entry_connect(struct bb_info *bb)
{
	struct bb_dig_info *dig = &bb->bb_dig_i;
	u8 igi_new;
	u8 rssi_min_new = halbb_get_rssi_min(bb);

	BB_DIG_DBG(bb, DIG_DBG_LV0, "%s ======>\n", __func__);
	if (rssi_min_new == 0)
		return;

	BB_DIG_DBG(bb, DIG_DBG_LV0, "rssi_min_new=%d\n", rssi_min_new << 1);
	/* Update igi_rssi */
	dig->igi_rssi = rssi_min_new >> 1;
	igi_new = halbb_dig_igi_by_ofst(bb, dig->igi_rssi, 0);
	halbb_dig_cfg_bbcr(bb, igi_new);
	/*Update EDCCA threshold*/
	halbb_edcca_thre_calc(bb);
}

void halbb_bb_cmd_notify(struct bb_info *bb_0, void *bb_cmd, enum phl_phy_idx phy_idx)
{
	struct bb_info *bb = bb_0;
	enum halbb_event_idx_t event_idx = *((enum halbb_event_idx_t *)bb_cmd);

#ifdef HALBB_DBCC_SUPPORT
	HALBB_GET_PHY_PTR(bb_0, bb, phy_idx);
	BB_DBG(bb, DBG_COMMON_FLOW, "[%s] phy_idx=%d\n", __func__, bb->bb_phy_idx);
#endif

	BB_DBG(bb, DBG_COMMON_FLOW, "[%s][phy_idx=%d] event_idx=%d\n", __func__, phy_idx, event_idx);

	if (event_idx == BB_EVENT_TIMER_DIG) {
		#ifdef HALBB_DIG_TDMA_SUPPORT
		halbb_tdmadig_io_en(bb);
		#endif
	} else if (event_idx == BB_EVENT_TIMER_CFO) {
		#ifdef HALBB_CFO_TRK_SUPPORT
		halbb_cfo_acc_io_en(bb);
		#endif
	} else if (event_idx == BB_EVENT_TIMER_DTR) {
		#ifdef HALBB_DYN_DTR_SUPPORT
			halbb_dtr_acc_io_en(bb);
		#endif
	} else if (event_idx == BB_EVENT_TIMER_ANTDIV) {
		#ifdef HALBB_ANT_DIV_SUPPORT
		halbb_antdiv_io_en(bb);
		#endif
	} else if (event_idx == BB_EVENT_TIMER_TDMA_CR) {
		#ifdef HALBB_TDMA_CR_SUPPORT
		halbb_tdma_cr_sel_io_en(bb);
		#endif
	} else if (event_idx == BB_EVENT_TIMER_LA) {
		#ifdef HALBB_LA_MODE_SUPPORT
		halbb_la_io_en(bb);
		#endif
	} else {
		BB_WARNING("[%s] event_idx=%d\n", __func__, event_idx);
	}
}

u8 halbb_wifi_event_notify(struct bb_info *bb_0, enum phl_msg_evt_id event, enum phl_phy_idx phy_idx)
{
	struct bb_info *bb = bb_0;
	struct rtw_hw_band *hw_band = &bb->hal_com->band[phy_idx];
	u8 pause_result = 0;
	u32 val[5] = {0};
	char val_char = '0';

#ifdef HALBB_DBCC_SUPPORT
	HALBB_GET_PHY_PTR(bb_0, bb, phy_idx);
	BB_DBG(bb, DBG_COMMON_FLOW, "[%s] phy_idx=%d\n", __func__, bb->bb_phy_idx);
#endif

	BB_DBG(bb, DBG_COMMON_FLOW, "[%s] event=%d\n", __func__, event);

	if (event == MSG_EVT_SCAN_START || event == MSG_EVT_CONNECT_START) {
		val[0] = 90;
		if (hw_band->cur_chandef.band == BAND_ON_24G)
			val[1] = PAUSE_OFDM_CCK;
		else
			val[1] = PAUSE_OFDM;
		pause_result = halbb_pause_func(bb, F_DIG, HALBB_PAUSE, HALBB_PAUSE_LV_2, 2, val, bb->bb_phy_idx);
		halbb_edcca_event_nofity(bb, HALBB_PAUSE);
	} else if (event == MSG_EVT_SCAN_END) {
		pause_result = halbb_pause_func(bb, F_DIG, HALBB_RESUME, HALBB_PAUSE_LV_2, 2, val, bb->bb_phy_idx);
		halbb_edcca_event_nofity(bb, HALBB_RESUME);
	} else if (event == MSG_EVT_CONNECT_END) {
		pause_result = halbb_pause_func(bb, F_DIG, HALBB_RESUME_NO_RECOVERY, HALBB_PAUSE_LV_2, 2, val, bb->bb_phy_idx);
		halbb_edcca_event_nofity(bb, HALBB_RESUME_NO_RECOVERY);
		halbb_new_entry_connect(bb);
		if (hw_band->cur_chandef.band == BAND_ON_24G)
			halbb_set_crc32_cnt3_format(bb, STATE_CTS);
		else
			halbb_set_crc32_cnt3_format(bb, STATE_BEACON);
	} else if (event == MSG_EVT_DBG_RX_DUMP || event == MSG_EVT_DBG_TX_DUMP) {
		halbb_dump_bb_reg(bb, &val[0], &val_char, &val[0], false, FRC_DUMP_ALL);
		halbb_dump_bb_reg(bb, &val[0], &val_char, &val[0], false, FRC_DUMP_ALL);
	}
	#ifdef HALBB_DBCC_SUPPORT
	else if (event == MSG_EVT_DBCC_DISABLE) {
		halbb_dbcc_band_switch_notify(bb);
	}
	#endif
	#ifdef HALBB_FW_OFLD_SUPPORT
	bb->bb_phl_evt = event;
	#endif

	return pause_result;
}

u8 halbb_pause_func(struct bb_info *bb, enum habb_fun_t pause_func,
		    enum halbb_pause_type pause_type,
		    enum halbb_pause_lv_type lv,
		    u8 val_lehgth,
		    u32 *val_buf, enum phl_phy_idx phy_idx)
{
	struct bb_func_hooker_info *func_t = &bb->bb_cmn_hooker->bb_func_hooker_i;
	s8 *pause_lv_pre = &bb->u8_dummy;
	u32 *bkp_val = &bb->u32_dummy;
	u32 ori_val[5] = {0};
	u32 pause_func_bitmap = (u32)BIT(pause_func);
	u8 i = 0;
	u8 pause_result = PAUSE_FAIL;

	BB_DBG(bb, DBG_DBG_API, "[%s][%s] LV=%d, Len=%d\n", __func__,
		((pause_type == HALBB_PAUSE) ? "Pause" :
		((pause_type == HALBB_RESUME) ? "Resume" :
		((pause_type == HALBB_PAUSE_NO_SET) ? "Pause no_set" : "Resume_no_recovery"))),
		lv, val_lehgth);

	if (lv >= HALBB_PAUSE_MAX_NUM) {
		BB_WARNING("[%s] LV=%d\n", __func__, lv);
		return PAUSE_FAIL;
	}

	if (pause_func == F_FA_CNT) {
		BB_DBG(bb, DBG_DBG_API, "[FA_cnt]\n");

		pause_lv_pre = &bb->pause_lv_table.lv_fa_cnt;
	}
#ifdef HALBB_CFO_TRK_SUPPORT
	else if (pause_func == F_CFO_TRK) {
		BB_DBG(bb, DBG_DBG_API, "[CFO]\n");

		if (val_lehgth > 1) {
			BB_WARNING("CFO length > 1\n");
			return PAUSE_FAIL;
		}

		ori_val[0] = (u32)(bb->bb_cfo_trk_i.crystal_cap); //which value?
		pause_lv_pre = &bb->pause_lv_table.lv_cfo;
		bkp_val = (u32 *)(&bb->bb_cfo_trk_i.rvrt_val);
		/*@function pointer hook*/
		func_t->pause_bb_dm_handler = halbb_set_cfo_pause_val;
	}
#endif
#ifdef HALBB_DIG_SUPPORT
	else if (pause_func == F_DIG) {
		BB_DBG(bb, DBG_DBG_API, "[DIG]\n");

		if (val_lehgth > DIG_PAUSE_INFO_SIZE) {
			BB_WARNING("DIG length > %d\n", DIG_PAUSE_INFO_SIZE);
			return PAUSE_FAIL;
		}
		/* {equivalent_rssi, en_pause_by_igi, en_pause_by_pd_low} */
		ori_val[0] = (u32)(RSSI_MAX - bb->bb_dig_i.p_cur_dig_unit->igi_fa_rssi);
		ori_val[1] = val_buf[1];
		pause_lv_pre = &bb->pause_lv_table.lv_dig;
		bkp_val = (u32 *)(&bb->bb_dig_i.rvrt_val);
		/*@function pointer hook*/
		func_t->pause_bb_dm_handler = halbb_set_dig_pause_val;
	}
#endif
#ifdef HALBB_EDCCA_SUPPORT
	else if (pause_func == F_EDCCA) {
		BB_DBG(bb, DBG_DBG_API, "[EDCCA]\n");

		if (val_lehgth > 1) {
			BB_WARNING("EDCCA length > 1\n");
			return PAUSE_FAIL;
		}
		ori_val[0] = (u32)(bb->bb_edcca_i.th_h);
		pause_lv_pre = &bb->pause_lv_table.lv_edcca;
		bkp_val = (u32 *)(&bb->bb_edcca_i.rvrt_val);
		/*@function pointer hook*/
		func_t->pause_bb_dm_handler = halbb_set_edcca_pause_val;
	}
#endif
	else {
		BB_WARNING("Error func idx\n");
		return PAUSE_FAIL;
	}

	BB_DBG(bb, DBG_DBG_API, "Pause_LV{new , pre} = {%d ,%d}\n",
	       lv, pause_lv_pre[0]);

	if (pause_type == HALBB_PAUSE || pause_type == HALBB_PAUSE_NO_SET) {
		if (lv <= *pause_lv_pre) {
			BB_DBG(bb, DBG_DBG_API, "[PAUSE FAIL] Pre_LV >= Curr_LV\n");
			return PAUSE_FAIL;
		}

		if (!(bb->pause_ability & pause_func_bitmap)) {
			for (i = 0; i < val_lehgth; i++)
				bkp_val[i] = ori_val[i];
		}

		bb->pause_ability |= pause_func_bitmap;
		BB_DBG(bb, DBG_DBG_API, "pause_ability=0x%x\n", bb->pause_ability);

		if (pause_type == HALBB_PAUSE) {
			for (i = 0; i < val_lehgth; i++)
				BB_DBG(bb, DBG_DBG_API,
				       "[PAUSE SUCCESS] val_idx[%d]{New, Ori}={0x%x, 0x%x}\n",
				       i, val_buf[i], bkp_val[i]);

			func_t->pause_bb_dm_handler(bb, val_buf, val_lehgth);
		} else {
			for (i = 0; i < val_lehgth; i++)
				BB_DBG(bb, DBG_DBG_API,
				       "[PAUSE NO Set: SUCCESS] val_idx[%d]{Ori}={0x%x}\n",
				       i, bkp_val[i]);
		}

		*pause_lv_pre = lv;
		pause_result = PAUSE_SUCCESS;
	} else if (pause_type == HALBB_RESUME) {
		if (lv < *pause_lv_pre) {
			BB_DBG(bb, DBG_DBG_API, "[Resume FAIL] Pre_LV >= Curr_LV\n");
			return PAUSE_FAIL;
		}

		if ((bb->pause_ability & pause_func_bitmap) == 0) {
			BB_DBG(bb, DBG_DBG_API, "[RESUME] No Need to Revert\n");
			return PAUSE_SUCCESS;
		}

		bb->pause_ability &= ~pause_func_bitmap;
		BB_DBG(bb, DBG_DBG_API, "pause_ability=0x%x\n", bb->pause_ability);

		*pause_lv_pre = HALBB_PAUSE_RELEASE;

		for (i = 0; i < val_lehgth; i++) {
			BB_DBG(bb, DBG_DBG_API,
			       "[RESUME] val_idx[%d]={0x%x}\n", i, bkp_val[i]);
		}

		func_t->pause_bb_dm_handler(bb, bkp_val, val_lehgth);

		pause_result = PAUSE_SUCCESS;
	} else if (pause_type == HALBB_RESUME_NO_RECOVERY) {
		if (lv < *pause_lv_pre) {
			BB_DBG(bb, DBG_DBG_API, "[Resume FAIL] Pre_LV >= Curr_LV\n");
			return PAUSE_FAIL;
		}

		if ((bb->pause_ability & pause_func_bitmap) == 0) {
			BB_DBG(bb, DBG_DBG_API, "[RESUME] No Need to Revert\n");
			return PAUSE_SUCCESS;
		}

		bb->pause_ability &= ~pause_func_bitmap;
		BB_DBG(bb, DBG_DBG_API, "pause_ability=0x%x\n", bb->pause_ability);

		*pause_lv_pre = HALBB_PAUSE_RELEASE;

		pause_result = PAUSE_SUCCESS;
	} else {
		BB_WARNING("error pause_type\n");
		pause_result = PAUSE_FAIL;
	}

	return pause_result;
}

void halbb_pause_func_dbg(struct bb_info *bb, char input[][16], u32 *_used,
			  char *output, u32 *_out_len)
{
	u32 val[10] = {0};
	u32 i;
	u8 len = 0;
	u32 buf[5] = {0};
	u8 pause_result = 0;
	enum halbb_pause_type type = 0;
	enum halbb_pause_lv_type lv = 0;
	u8 halbb_ary_size = bb->bb_cmn_hooker->bb_dm_number;
	enum habb_fun_t id = F_DEFAULT;

	/* ==== [Help] ====]*/
	if (_os_strcmp(input[1], "-h") == 0) {
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "{Func} {p:pause, pn:pause_no_set, r:Resume, rnc: Resume_no_recov} {lv:0~3} Val[0],...,Val[5]\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "{dig} {p/pn/r} {lv} {Pwr(|dBm|),hex} {0:apply to ofdm, 1:apply to cck and ofdm}\n");
		for (i = 0; i < halbb_ary_size; i++)
			BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
				    "*%s\n", halbb_func_i[i].name);
		return;
	}
	/* ==== [Function] ====]*/
	for (i = 0; i < halbb_ary_size; i++) {
		if (_os_strcmp(halbb_func_i[i].name, input[1]) == 0) {
			id = halbb_func_i[i].id;

			BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
				    "[%s]===>\n", halbb_func_i[i].name);
			break;
		}
	}

	if (i == halbb_ary_size) {
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "Func not found!\n");
		return;
	}
	/* ==== [Type] ====]*/
	if (_os_strcmp(input[2], "p") == 0) {
		type = HALBB_PAUSE;
	} else if (_os_strcmp(input[2], "pn") == 0) {
		type = HALBB_PAUSE_NO_SET;
	} else if (_os_strcmp(input[2], "r") == 0) {
		type = HALBB_RESUME;
	} else if (_os_strcmp(input[2], "rnc") == 0) {
		type = HALBB_RESUME_NO_RECOVERY;
	} else {
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "Set Err\n");
		return;
	}

	for (i = 1; i < 10; i++) {
		HALBB_SCAN(input[i + 1], DCMD_HEX, &val[i]);
	}

	lv = (enum halbb_pause_lv_type)val[2];

	for (i = 0; i < 5; i++)
		buf[i] = val[3 + i];

	if (id == F_CFO_TRK) {
		len = 1;
	} else if (id == F_DIG) {
		len = DIG_PAUSE_INFO_SIZE;
	} else if (id == F_EDCCA) {
		len = 1;
	} else {
		return;
	}

	if (len) {
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "{%s in lv=%d}, pause_val = {%d, %d}\n",
			   ((type == HALBB_PAUSE) ? "Pause" :
			   ((type == HALBB_RESUME) ? "Resume" : "Pause no set")),
			   lv, val[3], val[4]);

		pause_result = halbb_pause_func(bb, id, type, lv, len, buf, bb->bb_phy_idx);
	}

	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "Set %s\n", (pause_result) ? "Success" : "Fail");
}

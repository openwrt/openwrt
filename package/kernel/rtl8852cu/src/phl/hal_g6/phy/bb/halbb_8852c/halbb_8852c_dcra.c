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
#include "../halbb_precomp.h"

#ifdef BB_8852C_SUPPORT
#ifdef HALBB_DCRA_EN

void halbb_upt_mcs_table(struct bb_info *bb, struct rtw_phl_stainfo_t *phl_sta_i, struct rtw_xpltfm_mcs_table *mcs_table)
{
	struct bb_category_info bb_category[RTW_XPKT_CAT_MAX];
	
	//get original MCS of L0~L3
	halbb_category_init(bb, bb_category);
	//get tx report of L0~L3
	halbb_mcs_info_parsing(bb, bb_category, phl_sta_i, mcs_table);
	//Parse TX report to reconstruct TX scenario in last burst
	halbb_tx_report_parsing(bb, bb_category, phl_sta_i);
	halbb_rate_decision(bb, bb_category, mcs_table);
	halbb_modify_timer(bb, bb_category, phl_sta_i);
	BB_DBG(bb, DBG_RA, "Final table: 0x%x, 0x%x, 0x%x, 0x%x\n", 
		   mcs_table->policy[0].rate, mcs_table->policy[1].rate,
		   mcs_table->policy[2].rate, mcs_table->policy[3].rate);
	return;
}

void halbb_category_init(struct bb_info *bb, struct bb_category_info *category)
{
	u8 idx = 0;

	for (idx = 0; idx < RTW_XPKT_CAT_MAX; idx++) {
		category[idx].total = 0;
		category[idx].pkt_ok_num= 0;
		category[idx].rate_down_flag= 0;
		category[idx].report_highest_success_mcs = 0;
		category[idx].report_lowest_fail_mcs = 0x88;
	}
	category[0].absolute_highest_mcs = L0_HIGHEST_MCS;
	category[1].absolute_highest_mcs = L1_HIGHEST_MCS;
	category[2].absolute_highest_mcs = L2_HIGHEST_MCS;
	category[3].absolute_highest_mcs = L3_HIGHEST_MCS;
	BB_DBG(bb, DBG_RA, "[Init]Cat[0]Highest_mcs: 0x%x, Cat[1]Highest_mcs: 0x%x\n",
		   category[0].absolute_highest_mcs, category[1].absolute_highest_mcs);
	BB_DBG(bb, DBG_RA, "[Init]Cat[2]Highest_mcs: 0x%x, Cat[3]Highest_mcs: 0x%x\n",
		   category[2].absolute_highest_mcs, category[3].absolute_highest_mcs);
	return;
}

void halbb_mcs_info_parsing(struct bb_info *bb, struct bb_category_info *category, struct rtw_phl_stainfo_t *phl_sta_i, struct rtw_xpltfm_mcs_table *mcs_table)
{
	struct rtw_xpltfm_mcs_table table = {0};
	struct rtw_xpltfm_sta_info *sta = phl_sta_i->xpltfm_sta;
	u8 idx = 0;
	
	for (idx = 0; idx < RTW_XPKT_CAT_MAX; idx++) {
		table = sta->mcs_table;
		mcs_table->policy[idx].rate = table.policy[idx].rate;
		mcs_table->policy[idx].boost_pwr= table.policy[idx].boost_pwr;
		category[idx].initial_rate = table.policy[idx].rate;
		category[idx].boost_pwr = table.policy[idx].boost_pwr;
		category[idx].rate_up_cnt = sta->rate_up_cnt[idx];
		BB_DBG(bb, DBG_RA, "Original_Table: Cat[%d]: 0x%x, Pwr: %d, RU_cnt: %d\n",
			   idx, category[idx].initial_rate, category[idx].boost_pwr, category[idx].rate_up_cnt);
	}
	return;
}

void halbb_tx_report_parsing(struct bb_info *bb, struct bb_category_info *category, struct rtw_phl_stainfo_t *phl_sta_i)
{
	struct rtw_xpltfm_tx_report tx_report = {0};
	u8 idx = 0;
	u8 length_cat = 0;

	for (idx = 0; idx < RTW_XPLTFM_MAX_TX_RPT_CNT; idx++) {
		tx_report = phl_sta_i->xpltfm_sta->report[idx];
		if (tx_report.valid == 0)
			break;
		length_cat = tx_report.length_cat;
		if (tx_report.init_rate == tx_report.final_rate && tx_report.pkt_ok_num) {
			category[length_cat].pkt_ok_num++;
			if (tx_report.init_rate > category[length_cat].report_highest_success_mcs)
				category[length_cat].report_highest_success_mcs = tx_report.init_rate;
		} else {
			if (tx_report.init_rate < category[length_cat].report_lowest_fail_mcs)
				category[length_cat].report_lowest_fail_mcs = tx_report.init_rate;
		}
		category[tx_report.length_cat].total++;
		BB_DBG(bb, DBG_RA, "[Tx_rpt]Length_cat: %d, Total: %d,Ok: %d\n",
			   length_cat, category[length_cat].total, category[length_cat].pkt_ok_num);
		BB_DBG(bb, DBG_RA, "[Tx_rpt]Highest_success_mcs: 0x%x, Lowest_fail_mcs: 0x%x\n",
			   category[length_cat].report_highest_success_mcs,
			   category[length_cat].report_lowest_fail_mcs);
	}
	return;
}


void halbb_rate_decision(struct bb_info *bb, struct bb_category_info *category, struct rtw_xpltfm_mcs_table *mcs_table)
{
	u8 idx = 0;

	for (idx = 0; idx < RTW_XPKT_CAT_MAX; idx++) {
		if (category[idx].total > category[idx].pkt_ok_num) {
			//Rate down
			BB_DBG(bb, DBG_RA, "[RD]Rate down!\n");
			halbb_rate_down(bb, category, mcs_table, idx);
		} else if (category[idx].rate_up_cnt == RATE_UP_TH) {
			//Rate up
			BB_DBG(bb, DBG_RA, "[RU]Rate up, RU cnt: %d, RU th: %d\n",
				   category[idx].rate_up_cnt, RATE_UP_TH);
			halbb_rate_up(bb, category, mcs_table, idx);
		} else {
			//Rate stay
			if (category[idx].rate_down_flag)
				continue;
			if ((category[idx].total == 0 && category[idx].rate_up_cnt)
				|| (category[idx].initial_rate <= category[idx].report_highest_success_mcs))
				category[idx].rate_up_cnt++;
			BB_DBG(bb, DBG_RA, "[RS]Cat: %d, Rate: 0x%x, RU cnt: %d\n",
				   idx, category[idx].initial_rate, category[idx].rate_up_cnt);
		}
	}
	return;
}

void halbb_rate_down(struct bb_info *bb, struct bb_category_info *category, struct rtw_xpltfm_mcs_table *mcs_table, u8 length_category)
{
	u8 cur_idx = length_category;
	u8 i = 0;
	u8 j = 0;

	if (category[cur_idx].initial_rate == RTW_DATA_RATE_MCS0) {
		BB_DBG(bb, DBG_RA, "[RD]Already lowest rate, Cat: %d\n", cur_idx);
	} else {
		mcs_table->policy[cur_idx].rate = category[cur_idx].report_lowest_fail_mcs - 1;
		BB_DBG(bb, DBG_RA, "[Normal_RD]Category: 0x%x, Rate: 0x%x, Pwr: %d\n",
			   cur_idx, mcs_table->policy[cur_idx].rate, mcs_table->policy[cur_idx].boost_pwr);
	}
	category[cur_idx].rate_up_cnt = 0;
	i = cur_idx + 1;
	while (i < RTW_XPKT_CAT_MAX) {
		if (category[i].rate_down_flag) {
			BB_DBG(bb, DBG_RA, "[C2_RD][No_Rate_Down]Already rate down once!Cat: %d\n", i);
			i++;
			continue;
		}
		if (mcs_table->policy[i].rate == RTW_DATA_RATE_MCS0) {
			BB_DBG(bb, DBG_RA, "[C2_RD][Lowest_rate]Cat: %d, Rate: 0x%x, Pwr: %d\n",
			i, mcs_table->policy[i].rate, mcs_table->policy[i].boost_pwr);
		} else {
			mcs_table->policy[i].rate--;
			BB_DBG(bb, DBG_RA, "[C2_RD][Rate_Down]Cat: %d, Rate: 0x%x, Pwr: %d\n",
				   i, mcs_table->policy[i].rate, mcs_table->policy[i].boost_pwr);
		}
		category[i].rate_up_cnt = 0;
		category[i].rate_down_flag = 1;
		i++;
	}

	for (j = cur_idx; j > 0; j--) {
		if (mcs_table->policy[j].rate < mcs_table->policy[j - 1].rate) {
			mcs_table->policy[j - 1].rate = mcs_table->policy[j].rate;
			category[j - 1].rate_up_cnt = 0;
			BB_DBG(bb, DBG_RA, "[C1_RD]Cat: %d, Rate: 0x%x, Pwr: %d\n",
				   j - 1, mcs_table->policy[j - 1].rate, mcs_table->policy[j - 1].boost_pwr);
		}
	}
	return;

}
void halbb_rate_up(struct bb_info *bb, struct bb_category_info *category, struct rtw_xpltfm_mcs_table *mcs_table, u8 length_category)
{
	u8 cur_idx = length_category;
	u8 i = 0;
	category[cur_idx].rate_up_cnt = 0;

	if (category[cur_idx].initial_rate <= category[cur_idx].report_highest_success_mcs
		|| category[cur_idx].total == 0) {
		if (mcs_table->policy[cur_idx].rate < category[cur_idx].absolute_highest_mcs) {
			mcs_table->policy[cur_idx].rate++;
			BB_DBG(bb, DBG_RA, "[Normal_RU]Cat: %d, Rate: 0x%x, Pwr: %d\n",
				   cur_idx, mcs_table->policy[cur_idx].rate, mcs_table->policy[cur_idx].boost_pwr);
		}
		else {
			BB_DBG(bb, DBG_RA, "[No_RU][Highest_Rate]Cat: %d, Rate: 0x%x, Pwr: %d\n",
				   cur_idx, mcs_table->policy[cur_idx].rate, mcs_table->policy[cur_idx].boost_pwr);
			return;
		}
	}

	for (i = cur_idx; i < RTW_XPKT_CAT_MAX - 1; i++) {
		if (mcs_table->policy[i].rate > mcs_table->policy[i + 1].rate) {
			mcs_table->policy[i + 1].rate = mcs_table->policy[i].rate;
			category[i + 1].rate_up_cnt = 0;
			BB_DBG(bb, DBG_RA, "[C1_RU][Rate_up]Category: %d, Rate: 0x%x, Pwr: %d\n",
				   i + 1, mcs_table->policy[i + 1].rate, mcs_table->policy[i + 1].boost_pwr);
		}
	}
	return;
}

void halbb_modify_timer(struct bb_info *bb, struct bb_category_info *category, struct rtw_phl_stainfo_t *phl_sta_i)
{
	u8 i = 0;
	struct rtw_xpltfm_sta_info *sta = phl_sta_i->xpltfm_sta;
	for (i = 0; i < RTW_XPKT_CAT_MAX; i++) {
		sta->rate_up_cnt[i] = category[i].rate_up_cnt;
		BB_DBG(bb, DBG_RA, "[Update Timer & Rate_up_cnt]Category: 0x%x, Rate_up_cnt: %d\n",
			   i, sta->rate_up_cnt[i]);
	}
	return;
}

u8 halbb_arfr_table(struct bb_info *bb, u16 init_rate, u8 switch_table)
{
	u8 table = 0;
	u16 mcs = init_rate & 0xf;
	if (mcs == MCS0 || mcs == MCS1 || mcs == MCS2 || mcs == MCS5 || mcs == MCS8)
		table = 1;
	BB_DBG(bb, DBG_RA, "Return arfr table: Initial rate: 0x%x, MCS: %d, Table: %d\n",
		   init_rate, mcs, table);
	return table;
}

#endif
#endif

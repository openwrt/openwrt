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
#ifndef _HALBB_XBOX_RA_H_
#define _HALBB_XBOX_RA_H_
#ifdef BB_8852C_SUPPORT
#ifdef HALBB_DCRA_EN

/*@--------------------------[Define] ---------------------------------------*/

#define RATE_UP_TH 6

enum highest_mcs {
	L0_HIGHEST_MCS = 0x82,
	L1_HIGHEST_MCS = 0x83,
	L2_HIGHEST_MCS = 0x86,
	L3_HIGHEST_MCS = 0x87
};

enum power_setting {
	power_boost = 0x1E,
	non_power_boost = 0x14
};

enum msft_rate {
	MCS0 = 0x0,
	MCS1 = 0x1,
	MCS2 = 0x2,
	MCS3 = 0x3,
	MCS4 = 0x4,
	MCS5 = 0x5,
	MCS6 = 0x6,
	MCS7 = 0x7,
	MCS8 = 0x8,
	MCS9 = 0x9
};

/*@--------------------------[Structure]-------------------------------------*/

struct bb_category_info {
	u8 total;
	u8 pkt_ok_num;
	u8 boost_pwr: 4;
	u8 rate_down_flag: 4;
	u8 rate_up_cnt;
	u16 initial_rate;
	u16 absolute_highest_mcs;
	u16 report_highest_success_mcs;
	u16 report_lowest_fail_mcs;
	u32 start_time;
};

/*@--------------------------[Prptotype]-------------------------------------*/

void halbb_upt_mcs_table(struct bb_info *bb, struct rtw_phl_stainfo_t *phl_sta_i, struct rtw_xpltfm_mcs_table *mcs_table);
void halbb_category_init(bb, bb_category);
void halbb_mcs_info_parsing(struct bb_info *bb, struct bb_category_info *category, struct rtw_phl_stainfo_t *phl_sta_i, struct rtw_xpltfm_mcs_table *mcs_table);
void halbb_tx_report_parsing(struct bb_info *bb, struct bb_category_info *category, struct rtw_phl_stainfo_t *phl_sta_i);
void halbb_rate_decision(struct bb_info *bb, struct bb_category_info *category, struct rtw_xpltfm_mcs_table *mcs_table);
void halbb_rate_down(struct bb_info *bb, struct bb_category_info *category, struct rtw_xpltfm_mcs_table *mcs_table, u8 length_category);
void halbb_rate_up(struct bb_info *bb, struct bb_category_info *category, struct rtw_xpltfm_mcs_table *mcs_table, u8 length_category);
void halbb_modify_timer(struct bb_info *bb, struct bb_category_info *category, struct rtw_phl_stainfo_t *phl_sta_i);
u8 halbb_arfr_table(struct bb_info *bb, u16 init_rate, u8 switch_table);

#endif
#endif
#endif

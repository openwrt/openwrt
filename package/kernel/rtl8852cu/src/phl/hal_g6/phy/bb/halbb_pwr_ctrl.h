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
#ifndef __HALBB_PWR_CTRL_H__
#define __HALBB_PWR_CTRL_H__

/*@--------------------------[Define] ---------------------------------------*/
#define REG_PWRMACID_OFST	0x0016c
#define MACREG_PWRMACID_CR	0x0D36c
#define HALBB_PWR_STATE_NUM	3
#define DTP_FLOOR_UP_GAP 3
#define TX_HP_LV_0 0
#define TX_HP_LV_1 1
#define TX_HP_LV_2 2
#define TX_HP_LV_3 3
#define TX_HP_LV_UNCHANGE	4
/*#if (DM_ODM_SUPPORT_TYPE == ODM_AP)*/
#define TX_PWR_TH_LVL3 87
#define TX_PWR_TH_LVL2 78
#define TX_PWR_TH_LVL1 70
/*#elif (DM_ODM_SUPPORT_TYPE == ODM_WIN)
#define TX_PWR_TH_LVL3 90
#define TX_PWR_TH_LVL2 85
#define TX_PWR_TH_LVL1 80
#endif*/
#define TX_PWR_LVL3 6	/*3dBm*/
#define TX_PWR_LVL2 12	/*6dBm*/
#define TX_PWR_LVL1 20	/*10dBm*/

#define TSSI_CFG_NUM 4
#define TSSI_SBW_NUM 15

/*txdiff Table size*/
#define MCS_SIZE_TAB 12 /*MCS0~MCS11(12)*/
#define TABLE_SIZE_TAB 4 /*tbl0~tbl3 for each mcs idx*/
/*@--------------------------[Enum]------------------------------------------*/
/*@--------------------------[Structure]-------------------------------------*/
/* @ Dynamic CCA TH part */
struct bb_macidcca_info {
	bool cca_th_en;
	s8 cca_th;
};

struct bb_dyncca_info {
	/* [] */
	bool dyncca_en;
	u8 dyncca_offset;
	u8 dyncca_offset_n;
	/**/
	struct bb_macidcca_info macidcca_i[PHL_MAX_STA_NUM];
};

/* @ Power Ctrl part */
struct bb_dtp_info {
	/*u8	dyn_tx_power;
	u8	last_tx_power;*/
	u8	dyn_tx_pwr_lvl:4;
	u8	last_pwr_lvl:4;

	s8	pwr_val[2]; /* S(8,1) */
	bool	en_pwr[2];
};

struct bb_tssi_info {
	u32 tssi_dbw_table[TSSI_CFG_NUM][TSSI_SBW_NUM];
};

struct bb_pwr_ctrl_info {
	u8 pwr;

	/* [] */
	u8 enhance_pwr_th[HALBB_PWR_STATE_NUM];
	u8 set_pwr_th[HALBB_PWR_STATE_NUM];
	s8 pwr_lv_dbm[HALBB_PWR_STATE_NUM];
	/**/
	struct bb_dtp_info dtp_i[PHL_MAX_STA_NUM];
	struct bb_tssi_info tssi_i;
};

struct bb_info;
/*@--------------------------[Prptotype]-------------------------------------*/
#ifdef HALBB_PWR_CTRL_SUPPORT
void halbb_pwr_ctrl(struct bb_info *bb);
void halbb_pwr_ctrl_init(struct bb_info *bb);
void halbb_txdiff_tbl_init(struct bb_info* bb);
void halbb_set_pwr_macid_idx(struct bb_info *bb, u16 macid, s8 pwr, bool pwr_en, u8 idx);
void halbb_pwr_ctrl_dbg(struct bb_info *bb, char input[][16], u32 *_used,
		   char *output, u32 *_out_len);
#endif
void halbb_macid_ctrl_init(struct bb_info *bb);
void halbb_tpu_mac_cr_init(struct bb_info *bb, enum phl_phy_idx phy_idx);
void halbb_tssi_ctrl_mac_cr_init(struct bb_info *bb, enum phl_phy_idx phy_idx);
void halbb_tssi_ctrl_set_dbw_table(struct bb_info *bb);
void halbb_tssi_ctrl_set_fast_mode_cfg(struct bb_info *bb, enum phl_band_idx band,
						enum tssi_bandedge_cfg bandedge_cfg);
void halbb_pwr_dbg(struct bb_info *bb, char input[][16], u32 *_used,
		   char *output, u32 *_out_len);
void halbb_tmac_force_tx_pwr_switch_on(struct bb_info *bb, enum phl_phy_idx phy_idx);
void halbb_pwr_lim_by_macid_init_and_cca_pwr_th_init(struct bb_info *bb);

#endif
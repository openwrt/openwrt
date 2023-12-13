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

#ifndef _HALBB_PMAC_SETTING_EX_H_
#define _HALBB_PMAC_SETTING_EX_H_
#include "halbb_ic_hw_info.h"
#include "halbb_pmac_setting.h"

/*  ============================================================
 			   structure
    ============================================================
*/
struct halbb_pmac_info {
	u8 en_pmac_tx:1; /*0: PMAC Tx Off 1: PMAC Tx On */
	u8 is_cck:1;
	u8 mode:3; /*1: Packet TX 3:Continuous TX */
	u8 rsvd:3;
	u16 tx_cnt;
	u16 period; // unit=50ns
	u16 tx_time; // us
	u8 duty_cycle; // for fw trig. tx used
	bool cck_lbk_en;
};

struct bb_h2c_fw_tx_setting {
	u8 pkt_cnt[2];
	bool tx_en;
	u8 tx_type;
	u8 tx_period[4];
	u8 tx_time[4];
	u8 duty_cycle[4];
};

struct bb_c2h_fw_tx_rpt {
	bool tx_done;
};


/*  ============================================================
 		     Function Prototype
    ============================================================
*/

struct bb_info;

void halbb_set_pmac_tx(struct bb_info *bb, struct halbb_pmac_info *tx_info,
		       enum phl_phy_idx phy_idx);

void halbb_set_tmac_tx(struct bb_info *bb, enum phl_phy_idx phy_idx);

bool halbb_cfg_lbk(struct bb_info *bb, bool lbk_en, bool is_dgt_lbk,
		   enum rf_path tx_path, enum rf_path rx_path,
		   enum channel_width bw, enum phl_phy_idx phy_idx);

bool halbb_cfg_lbk_cck(struct bb_info *bb, bool lbk_en, bool is_dgt_lbk,
		       enum rf_path tx_path, enum rf_path rx_path,
		       enum channel_width bw, enum phl_phy_idx phy_idx);

bool halbb_set_txpwr_dbm(struct bb_info *bb, s16 pwr_dbm,
			 enum phl_phy_idx phy_idx);

s16 halbb_get_txpwr_dbm(struct bb_info *bb, enum phl_phy_idx phy_idx);

s16 halbb_get_txinfo_txpwr_dbm(struct bb_info *bb);

bool halbb_set_cck_txpwr_idx(struct bb_info *bb, u16 pwr_idx,
			     enum rf_path tx_path);

u16 halbb_get_cck_txpwr_idx(struct bb_info *bb, enum rf_path tx_path);

s16 halbb_get_cck_ref_dbm(struct bb_info *bb, enum rf_path tx_path);
bool halbb_set_vht_mu_user_idx(struct bb_info *bb, bool en, u8 idx,
			       enum phl_phy_idx phy_idx);
bool halbb_set_ofdm_txpwr_idx(struct bb_info *bb, u16 pwr_idx,
			      enum rf_path tx_path);

u16 halbb_get_ofdm_txpwr_idx(struct bb_info *bb, enum rf_path tx_path);

s16 halbb_get_ofdm_ref_dbm(struct bb_info *bb, enum rf_path tx_path);

bool halbb_chk_tx_idle(struct bb_info *bb, enum phl_phy_idx phy_idx);

void halbb_dpd_bypass(struct bb_info *bb, bool pdp_bypass,
		      enum phl_phy_idx phy_idx);

void halbb_backup_info(struct bb_info *bb, enum phl_phy_idx phy_idx);

void halbb_restore_info(struct bb_info *bb, enum phl_phy_idx phy_idx);

enum rtw_hal_status halbb_set_txsc(struct bb_info *bb, u8 txsc,
				   enum phl_phy_idx phy_idx);

enum rtw_hal_status halbb_set_txsb(struct bb_info *bb, u8 txsb,
				   enum phl_phy_idx phy_idx);

bool halbb_set_bss_color(struct bb_info *bb, u8 bss_color, 
			 enum phl_phy_idx phy_idx);

bool halbb_set_sta_id(struct bb_info *bb, u16 sta_id, enum phl_phy_idx phy_idx);

#endif

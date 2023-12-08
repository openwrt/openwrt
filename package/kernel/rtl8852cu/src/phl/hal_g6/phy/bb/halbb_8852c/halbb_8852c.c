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
#include "../halbb_precomp.h"

#ifdef BB_8852C_SUPPORT

bool halbb_chk_pkg_valid_8852c(struct bb_info *bb, u8 bb_ver, u8 rf_ver)
{
	bool valid = true;

#if 0
	if (bb_ver >= X && rf_ver >= Y)
		valid = true;
	else if (bb_ver < X && rf_ver < Y)
		valid = true;
	else
		valid = false;
#endif

	if (!valid) {
		/*halbb_set_reg(bb, 0x1c3c, (BIT(0) | BIT(1)), 0x0);*/
		BB_WARNING("[%s] Pkg_ver{bb, rf}={%d, %d} disable all BB block\n",
			 __func__, bb_ver, rf_ver);
	}

	return valid;
}

bool halbb_chk_tx_idle_8852c(struct bb_info *bb, enum phl_phy_idx phy_idx)
{
	u8 tx_state = 0;
	bool idle = false;
	u32 dbg_port = 0x30002;

	BB_DBG(bb, DBG_PHY_CONFIG, "<====== %s ======>\n", __func__);

	if (phy_idx == HW_PHY_1)
		dbg_port = 0x30005;

	if (halbb_bb_dbg_port_racing(bb, DBGPORT_PRI_3)) {
		halbb_dbg_port_sel(bb, (u16)(dbg_port & 0xffff),
				   (u8)((dbg_port & 0xff0000) >> 16), 0x0, 0x1);
		BB_DBG(bb, DBG_PHY_CONFIG,
		       "*Set dbg_port=(0x%x)\n", dbg_port);
	} else {
		dbg_port = halbb_get_bb_dbg_port_idx(bb);
		BB_DBG(bb, DBG_PHY_CONFIG,
		       "[Set dbg_port fail!] Curr-DbgPort=0x%x\n", dbg_port);

		return false;
	}

	/* Release DBG port */
	halbb_release_bb_dbg_port(bb);

	tx_state = (u8)(halbb_get_bb_dbg_port_val(bb) & 0x3f);

	if (tx_state == 0)
		idle = true;
	else
		idle = false;

	return idle;
}

void halbb_stop_pmac_tx_8852c(struct bb_info *bb,
			      struct halbb_pmac_info *tx_info,
			      enum phl_phy_idx phy_idx)
{
	if (tx_info->is_cck) { // CCK
		if (tx_info->mode == CONT_TX) {
			halbb_set_reg(bb, 0x231c, BIT(1), 1);
			halbb_set_reg(bb, 0x2318, BIT(8), 0);
			halbb_set_reg(bb, 0x231c, BIT(21), 0);
			halbb_set_reg(bb, 0x231c, BIT(1), 0);
		} else if (tx_info->mode == PKTS_TX) {
			halbb_set_reg_cmn(bb, 0x9c4, BIT(4), 0, phy_idx);
		} else if (tx_info->mode == CCK_CARRIER_SIPPRESSION_TX) {
			halbb_set_reg_cmn(bb, 0x9c4, BIT(4), 0, phy_idx);
			/*Carrier Suppress Tx*/
			halbb_set_reg(bb, 0x2318, BIT(9), 0);
			/*Enable scrambler at payload part*/
			halbb_set_reg(bb, 0x231c, BIT(7), 0);
		}
	} else { // OFDM
		if (tx_info->mode == CONT_TX)
			halbb_set_reg_cmn(bb, 0x9c4, BIT(0), 0, phy_idx);
		else if (tx_info->mode == PKTS_TX)
			halbb_set_reg_cmn(bb, 0x9c4, BIT(4), 0, phy_idx);
	}
}

void halbb_start_pmac_tx_8852c(struct bb_info *bb,
			       struct halbb_pmac_info *tx_info,
			       enum halbb_pmac_mode mode, u32 pkt_cnt,u16 period,
			       enum phl_phy_idx phy_idx)
{
	if (mode == CONT_TX) {
		if (tx_info->is_cck) {
			halbb_set_reg(bb, 0x2318, BIT(8), 1);
			halbb_set_reg(bb, 0x231c, BIT(21), 1);
		} else {
			halbb_set_reg_cmn(bb, 0x9c4, BIT(0), 1, phy_idx);
		}
	} else if (mode == PKTS_TX) {
		/*Tx_N_PACKET_EN */
		halbb_set_reg_cmn(bb, 0x9c4, BIT(4), 1, phy_idx);
		/*Tx_N_PERIOD */
		halbb_set_reg_cmn(bb, 0x9c4, 0xffffff00, period, phy_idx);
		/*Tx_N_PACKET */
		halbb_set_reg_cmn(bb, 0x9c8, 0xffffffff, pkt_cnt, phy_idx);
	} else if (mode == CCK_CARRIER_SIPPRESSION_TX) {
		if (tx_info->is_cck) {
			/*Carrier Suppress Tx*/
			halbb_set_reg(bb, 0x2318, BIT(9), 1);
			/*Disable scrambler at payload part*/
			halbb_set_reg(bb, 0x231c, BIT(7), 1);
		} else {
			return;
		}
		/*Tx_N_PACKET_EN */
		halbb_set_reg_cmn(bb, 0x9c4, BIT(4), 1, phy_idx);
		/*Tx_N_PERIOD */
		halbb_set_reg_cmn(bb, 0x9c4, 0xffffff00, period, phy_idx);
		/*Tx_N_PACKET */
		halbb_set_reg_cmn(bb, 0x9c8, 0xffffffff, pkt_cnt, phy_idx);
	}
	/*Tx_EN */
	halbb_set_reg_cmn(bb, 0x9c0, BIT(0), 1, phy_idx);
	halbb_set_reg_cmn(bb, 0x9c0, BIT(0), 0, phy_idx);
}

void halbb_set_pmac_tx_8852c(struct bb_info *bb_0, struct halbb_pmac_info *tx_info,
			     enum phl_phy_idx phy_idx)
{
	struct bb_info *bb = bb_0;

#ifdef HALBB_DBCC_SUPPORT
	HALBB_GET_PHY_PTR(bb_0, bb, phy_idx);
#endif

	BB_DBG(bb, DBG_PHY_CONFIG, "<====== %s ======>\n", __func__);

	if (!tx_info->en_pmac_tx) {
		halbb_stop_pmac_tx_8852c(bb, tx_info, phy_idx);
		/* PD hit enable */
		halbb_set_reg_cmn(bb, 0xc3c, BIT(9), 0, phy_idx);
		if (bb->bb_ch_i.is_2g)
        		halbb_set_reg(bb, 0x2320, BIT(0), 0);
		return;
	}
	/*Turn on PMAC */
	/* Tx */
	halbb_set_reg_cmn(bb, 0x0980, BIT(0), 1, phy_idx);
	/* Rx */
	halbb_set_reg_cmn(bb, 0x0980, BIT(16), 1, phy_idx);
	halbb_set_reg_cmn(bb, 0x0988, 0x3f, 0x3f, phy_idx);

	/* PD hit enable */
	halbb_set_reg_cmn(bb, 0x704, BIT(1), 0, phy_idx);
	halbb_set_reg_cmn(bb, 0xc3c, BIT(9), 1, phy_idx);
	if (tx_info->cck_lbk_en) {
		halbb_set_reg(bb, 0x2320, BIT(0), 0);
	} else {
		if (bb->bb_ch_i.is_2g)
        		halbb_set_reg(bb, 0x2320, BIT(0), 1);
	}
	halbb_set_reg_cmn(bb, 0x704, BIT(1), 1, phy_idx);

	halbb_start_pmac_tx_8852c(bb, tx_info, tx_info->mode, tx_info->tx_cnt,
		       tx_info->period, phy_idx);

	BB_DBG(bb, DBG_PHY_CONFIG, "[PMAC Tx] cck_lbk_en=%d\n", tx_info->cck_lbk_en);
}

void halbb_set_tmac_tx_8852c(struct bb_info *bb, enum phl_phy_idx phy_idx)
{
	/* To do: 0x0d80[16] [25] / 0x0d88[5:0] Should be set to default value in parameter package*/
	/* Turn on TMAC */
	halbb_set_reg_cmn(bb, 0x0980, BIT(0), 0, phy_idx);
	halbb_set_reg_cmn(bb, 0x0980, BIT(16), 0, phy_idx);
	halbb_set_reg_cmn(bb, 0x0988, 0xfff, 0, phy_idx);
	halbb_set_reg_cmn(bb, 0x0994, 0xf0, 0, phy_idx);
	// PDP bypass from TMAC
	halbb_set_reg_cmn(bb, 0x09a4, BIT(10), 0, phy_idx);
	// TMAC Tx path
	halbb_set_reg_cmn(bb, 0x09a4, 0x1c, 0, phy_idx);
	// TMAC Tx power
	halbb_set_reg_cmn(bb, 0x09a4, BIT(16), 0, phy_idx);
	// TMAC Tx OFDM triangular shaping filter
	halbb_set_reg_cmn(bb, 0x09a4, BIT(31), 0, phy_idx);
}

void halbb_dpd_bypass_8852c(struct bb_info *bb, bool pdp_bypass,
			      enum phl_phy_idx phy_idx)
{
	halbb_set_reg_cmn(bb, 0x09a4, BIT(10), 1, phy_idx);
	halbb_set_reg_cmn(bb, 0x45b8, BIT(16), pdp_bypass, phy_idx);
}

void halbb_ic_hw_setting_init_8852c(struct bb_info *bb)
{
	BB_DBG(bb, DBG_PHY_CONFIG, "<====== %s ======>\n", __func__);
	//Set dbcc_80p80_sel_evm_rpt_en
	halbb_set_reg(bb, 0xa10, BIT(0), 1);
	halbb_set_reg(bb, 0x2a10, BIT(0), 1);
}

void halbb_ic_hw_setting_8852c(struct bb_info *bb)
{
	bool btg_en;

	BB_DBG(bb, DBG_PHY_CONFIG, "<====== %s ======>\n", __func__);

	btg_en = (bb->hal_com->band[0].cur_chandef.band == BAND_ON_24G) &&
		  ((bb->rx_path == RF_PATH_B) || (bb->rx_path == RF_PATH_AB)) ? true : false;

	if (btg_en && bb->bb_link_i.is_linked && (bb->bb_ch_i.rssi_min < (75 << 1))) {// if rssi < -35 dbm
		halbb_set_reg(bb, 0x4aa4, BIT(18), 0x0);
		BB_DBG(bb, DBG_PHY_CONFIG, "[BT] BTG enable, Is linked\n");
		BB_DBG(bb, DBG_PHY_CONFIG, "[BT][RSSI] rssi_min=0x%x\n", bb->bb_ch_i.rssi_min >> 1);
	} else {
		halbb_set_reg(bb, 0x4aa4, BIT(18), 0x1);
		BB_DBG(bb, DBG_PHY_CONFIG, "[BT] btg_en=%d, is_linked=%d\n", btg_en, bb->bb_link_i.is_linked);
		BB_DBG(bb, DBG_PHY_CONFIG, "[BT][RSSI] rssi_min=0x%x\n", bb->bb_ch_i.rssi_min >> 1);
	}
}

bool halbb_set_pd_lower_bound_8852c(struct bb_info *bb, u8 bound,
				      enum channel_width bw,
				      enum phl_phy_idx phy_idx)
{
	/* 
	Range of bound value: 
	BW20: 95~33
	BW40: 92~30
	BW80: 89~27
	*/
	u8 bw_attenuation = 0;
	u8 subband_filter_atteniation = 7;
	u8 bound_idx = 0;
	bool rpt = true;

	BB_DBG(bb, DBG_PHY_CONFIG, "<====== %s ======>\n", __func__);

	if (bound == 0) {
		halbb_set_reg_cmn(bb, 0x481C, BIT(29), 0, phy_idx);
		BB_DBG(bb, DBG_PHY_CONFIG,
		       "[PD Bound] Set Boundary to default!\n");
		return true;
	}

	bb->bb_cmn_backup_i.cur_pd_lower_bound = bound;

	if (bw == CHANNEL_WIDTH_20) {
		bw_attenuation = 0;
	} else if (bw == CHANNEL_WIDTH_40) {
		bw_attenuation = 3;
	} else if (bw == CHANNEL_WIDTH_80) {
		bw_attenuation = 6;
	} else if (bw == CHANNEL_WIDTH_160) {
		bw_attenuation = 9;
	} else {
		BB_DBG(bb, DBG_PHY_CONFIG,
		       "[PD Bound] Only support BW20/40/80/160 !\n");
		return false;
	}

	bound += (bw_attenuation + subband_filter_atteniation);
	// If Boundary dbm is odd, set it to even number
	bound = bound % 2 ? bound + 1 : bound;

	if (bound < 40) {
		BB_DBG(bb, DBG_PHY_CONFIG,
		       "[PD Bound] Threshold too high, set to highest level!\n");
		bound = 40;
		rpt = false;
	}

	if (bound > 102) {
		BB_DBG(bb, DBG_PHY_CONFIG,
		       "[PD Bound] Threshold too low, disable PD lower bound function!\n");
		halbb_set_reg_cmn(bb, 0x481C, BIT(29), 0, phy_idx);
		return true;
	}

	bound_idx =  (102 - bound) >> 1;

	halbb_set_reg_cmn(bb, 0x481C, 0x7c0, bound_idx, phy_idx);
	halbb_set_reg_cmn(bb, 0x481C, BIT(29), 1, phy_idx);

	BB_DBG(bb, DBG_PHY_CONFIG, "[PD Bound] Set Boundary Success!\n");

	return rpt;
}

bool halbb_set_pd_lower_bound_cck_8852c(struct bb_info *bb, u8 bound,
				      enum channel_width bw,
				      enum phl_phy_idx phy_idx)
{
	u8 bw_attenuation = 0;
	u8 subband_filter_atteniation = 5;
	s8 bound_tmp = 0;

	BB_DBG(bb, DBG_PHY_CONFIG, "<====== %s ======>\n", __func__);

	if (bound == 0) {
		halbb_set_reg_cmn(bb, 0x4b74, BIT(30), 0, phy_idx);
		BB_DBG(bb, DBG_PHY_CONFIG,
			"[PD Bound] Set Boundary to default!\n");
		return true;
	}

	if (bw == CHANNEL_WIDTH_20) {
		bw_attenuation = 0;
	} else if (bw == CHANNEL_WIDTH_40) {
		bw_attenuation = 3;
	} else if (bw == CHANNEL_WIDTH_80) {
		bw_attenuation = 6;
	} else if (bw == CHANNEL_WIDTH_160) {
		bw_attenuation = 9;
	} else {
		BB_DBG(bb, DBG_PHY_CONFIG,
			"[PD Bound] Only support BW20/40/80/160 !\n");
		return false;
	}

	bound += (bw_attenuation + subband_filter_atteniation);
	bound_tmp = (-1) * MIN_2(bound, 128);

	halbb_set_reg_cmn(bb, 0x4b64, 0xff000000, bound_tmp, phy_idx);
	halbb_set_reg_cmn(bb, 0x4b64, 0xff0000, 0x7f, phy_idx);
	halbb_set_reg_cmn(bb, 0x4b74, BIT(30), 1, phy_idx);

	BB_DBG(bb, DBG_PHY_CONFIG, "[PD Bound] Set CCK Boundary Success!\n");

	return true;
}

u8 halbb_querry_pd_lower_bound_8852c(struct bb_info *bb, bool get_en_info, enum phl_phy_idx phy_idx)
{
	u8 tmp;

	BB_DBG(bb, DBG_PHY_CONFIG, "<====== %s ======>\n", __func__);

	if (get_en_info)
		tmp = (u8)halbb_get_reg_cmn(bb, 0x481C, BIT(29), phy_idx);
	else
		tmp = bb->bb_cmn_backup_i.cur_pd_lower_bound;

	return tmp;
}

void halbb_pop_en_8852c(struct bb_info *bb, bool en, enum phl_phy_idx phy_idx)
{
	BB_DBG(bb, DBG_PHY_CONFIG, "<====== %s ======>\n", __func__);

	if (en) {
		halbb_set_reg_cmn(bb, 0x4798, BIT(8), 1, phy_idx);

		halbb_set_reg_cmn(bb, 0xc54, 0x6, 0, phy_idx);
		halbb_set_reg_cmn(bb, 0xc68, 0xff, 0x50, phy_idx);
		halbb_set_reg_cmn(bb, 0x4794, 0xF8000, 0xc, phy_idx);
		halbb_set_reg_cmn(bb, 0x4794, 0x1F00000, 8, phy_idx);
		halbb_set_reg_cmn(bb, 0x4798, BIT(9), 0, phy_idx);
	} else {
		halbb_set_reg_cmn(bb, 0x4798, BIT(8), 0, phy_idx);
	}
}

bool halbb_querry_pop_en_8852c(struct bb_info *bb, enum phl_phy_idx phy_idx)
{
	bool en;

	BB_DBG(bb, DBG_PHY_CONFIG, "<====== %s ======>\n", __func__);

	en = (bool)halbb_get_reg_cmn(bb, 0x4798, BIT(8), phy_idx);

	return en;
}

u16 halbb_get_per_8852c(struct bb_info *bb, enum phl_phy_idx phy_idx)
{
	u16 per;
	u32 crc_ok, crc_err, brk_cnt;
	u32 numer, denomer;

	BB_DBG(bb, DBG_PHY_CONFIG, "<====== %s ======>\n", __func__);

	crc_ok = halbb_mp_get_rx_crc_ok(bb, phy_idx);
	crc_err = halbb_mp_get_rx_crc_err(bb, phy_idx);
	brk_cnt = halbb_get_reg_cmn(bb, 0x1a08, MASKHWORD, phy_idx);

	if ((crc_ok > 0xffff) || (crc_err > 0xffff)) {
		BB_DBG(bb, DBG_DBG_API, "[PER] Value > Brk cnt upper bound!\n");
		return 0xffff;
	}

	denomer = crc_ok + crc_err + brk_cnt;
	numer = crc_err + brk_cnt + (denomer >> 1);
	per = (u16)HALBB_DIV(numer * 100, denomer);

	return per;
}

u8 halbb_get_losel_8852c(struct bb_info *bb)
{
	u8 tmp = 0;

	BB_DBG(bb, DBG_PHY_CONFIG, "<====== %s ======>\n", __func__);

	tmp = (u8)halbb_get_reg(bb, 0x35c, BIT(11) | BIT(10));
	return tmp;
}

#endif

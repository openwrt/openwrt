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
#include "halbb_precomp.h"
#ifdef HALBB_RA_SUPPORT

const u16 bb_phy_rate_table[] = {
	/*CCK*/
	1, 2, 5, 11,
	/*OFDM*/
	6, 9, 12, 18, 24, 36, 48, 54,
	/*HT/VHT-1ss LGI*/
	6, 13, 19, 26, 39, 52, 58, 65, 78, 87, 98, 108,
	/*HT/VHT-2ss LGI*/
	13, 26, 39, 52, 78, 104, 117, 130, 156, 173, 195, 217,
	/*HT/VHT-3ss LGI*/
	19, 39, 58, 78, 117, 156, 175, 195, 234, 260, 293, 325,
	/*HT/VHT-4ss LGI*/
	26, 52, 78, 104, 156, 208, 234, 260, 312, 347, 390, 433,
}; /*HE[3.2] = VHT[LGI] * 1.125*/

bool halbb_is_he_rate(struct bb_info *bb, u16 rate)
{
	return ((((rate & 0x1ff) >= BB_HE_1SS_MCS0) &&
		((rate & 0x1ff) <= BB_HE_1SS_MCS(11))) ||
		(((rate & 0x1ff) >= BB_HE_2SS_MCS0) &&
		((rate & 0x1ff) <= BB_HE_2SS_MCS(11))) ||
		(((rate & 0x1ff) >= BB_HE_3SS_MCS0) &&
		((rate & 0x1ff) <= BB_HE_3SS_MCS(11))) ||
		(((rate & 0x1ff) >= BB_HE_4SS_MCS0) &&
		((rate & 0x1ff) <= BB_HE_4SS_MCS(11)))) ? true : false;
}

bool halbb_is_vht_rate(struct bb_info *bb, u16 rate)
{
	return ((((rate & 0x1ff) >= BB_VHT_1SS_MCS0) &&
		((rate & 0x1ff) <= BB_VHT_1SS_MCS(9))) ||
		(((rate & 0x1ff) >= BB_VHT_2SS_MCS0) &&
		((rate & 0x1ff) <= BB_VHT_2SS_MCS(9))) ||
		(((rate & 0x1ff) >= BB_VHT_3SS_MCS0) &&
		((rate & 0x1ff) <= BB_VHT_3SS_MCS(9))) ||
		(((rate & 0x1ff) >= BB_VHT_4SS_MCS0) &&
		((rate & 0x1ff) <= BB_VHT_4SS_MCS(9)))) ? true : false;
}

bool halbb_is_ht_rate(struct bb_info *bb, u16 rate)
{
	return (((rate & 0x1ff) >= BB_HT_MCS0) &&
		((rate & 0x1ff) <= BB_HT_MCS(31))) ? true : false;
}

bool halbb_is_ofdm_rate(struct bb_info *bb, u16 rate)
{
	return (((rate & 0x1ff) >= BB_06M) &&
		((rate & 0x1ff) <= BB_54M)) ? true : false;
}

bool halbb_is_cck_rate(struct bb_info *bb, u16 rate)
{
	return ((rate & 0x1ff) <= BB_11M) ? true : false;
}

u8 halbb_legacy_rate_2_spec_rate(struct bb_info *bb, u16 rate)
{
	u8 rate_idx = 0x0;
	u8 legacy_spec_rate_t[8] = {BB_SPEC_RATE_6M, BB_SPEC_RATE_9M,
				    BB_SPEC_RATE_12M, BB_SPEC_RATE_18M,
				    BB_SPEC_RATE_24M, BB_SPEC_RATE_36M,
				    BB_SPEC_RATE_48M, BB_SPEC_RATE_54M};

	rate_idx = rate - BB_06M;
	return legacy_spec_rate_t[rate_idx];
}

u64 halbb_mgnt_2_rate_mask(u8 rate)
{
	u64 ret = 0;

	/*exclude BSS basic bit*/
	rate &= 0x7f;

	/*a/b/g mode only, there is no requirement for n/ac/ax mode*/
	if (rate > 108)
		return ret;

	/*unit:0.5Mbps*/
	switch (rate) {
	case 2:
		ret = BIT(BB_01M);
		break;
	case 4:
		ret = BIT(BB_02M);
		break;
	case 11:
		ret = BIT(BB_05M);
		break;
	case 22:
		ret = BIT(BB_11M);
		break;
	case 12:
		ret = BIT(BB_06M);
		break;
	case 18:
		ret = BIT(BB_09M);
		break;
	case 24:
		ret = BIT(BB_12M);
		break;
	case 36:
		ret = BIT(BB_18M);
		break;
	case 48:
		ret = BIT(BB_24M);
		break;
	case 72:
		ret = BIT(BB_36M);
		break;
	case 96:
		ret = BIT(BB_48M);
		break;
	case 108:
		ret = BIT(BB_54M);
		break;
	default:
		break;
	}

	return ret;
}

u8 halbb_mcs_ss_to_fw_rate_idx(u8 mode, u8 mcs, u8 ss)
{
	u8 fw_rate_idx = 0;

	if (mode == 3) {
		fw_rate_idx = RATE_HE1SS_MCS0 + (ss - 1) * 12; // MAX HE MCS is MCS11 (Totally, 12 MCS index)
	} else if (mode == 2) {
		fw_rate_idx = RATE_VHT1SS_MCS0 + (ss - 1) * 10; // MAX VHT MCS is MCS9 (Totally, 10 MCS index)
	} else if (mode == 1) { // HT
		fw_rate_idx = RATE_HT_MCS0 + (ss - 1) * 8;
	} else {
		fw_rate_idx = 0;
	}

	return fw_rate_idx;
}

void halbb_rate_idx_parsor(struct bb_info *bb, u16 rate_idx, enum rtw_gi_ltf gi_ltf, struct bb_rate_info *ra_i)
{
	ra_i->rate_idx_all = rate_idx | (((u16)gi_ltf & 0xf) << 12);
	ra_i->rate_idx = rate_idx;
	ra_i->gi_ltf = gi_ltf;
	if (bb->bb_80211spec == BB_AX_IC)
		ra_i->mode = (enum bb_mode_type)((rate_idx & 0x180) >> 7);
	else
		ra_i->mode = (enum bb_mode_type)((rate_idx & 0xf00) >> 8);

	if (ra_i->mode == BB_LEGACY_MODE) {
		ra_i->ss = 1;
		ra_i->idx = rate_idx & 0x1f;
	} else if (ra_i->mode == BB_HT_MODE) {
		ra_i->ss = ((rate_idx & 0x18) >> 3) + 1;
		ra_i->idx = rate_idx & 0x1f;
	} else if (ra_i->mode == BB_EHT_MODE) {
		ra_i->ss = ((rate_idx & 0xe0) >> 5) + 1;
		ra_i->idx = rate_idx & 0x1f;
	} else { /*VHT,HE*/
		if (bb->ic_type & BB_IC_AX_SERIES)
			ra_i->ss = ((rate_idx & 0x70) >> 4) + 1;
		else
			ra_i->ss = ((rate_idx & 0xe0) >> 5) + 1;

		ra_i->idx = rate_idx & 0xf;
	}

	/* Transfer to fw used rate_idx*/
	if (ra_i->mode == BB_LEGACY_MODE) {
		ra_i->fw_rate_idx = ra_i->idx;
		return;
	}

	ra_i->fw_rate_idx = halbb_mcs_ss_to_fw_rate_idx(ra_i->mode, ra_i->idx, ra_i->ss);

	return;
}

u8 halbb_rate_2_rate_digit(struct bb_info *bb, u16 rate)
{
	u8 legacy_table[12] = {1, 2, 5, 11, 6, 9, 12, 18, 24, 36, 48, 54};
	u8 rate_digit = 0;

	if (rate >= BB_HE_8SS_MCS0)
		rate_digit = (rate - BB_HE_8SS_MCS0);
	else if (rate >= BB_HE_7SS_MCS0)
		rate_digit = (rate - BB_HE_7SS_MCS0);
	else if (rate >= BB_HE_6SS_MCS0)
		rate_digit = (rate - BB_HE_6SS_MCS0);
	else if (rate >= BB_HE_5SS_MCS0)
		rate_digit = (rate - BB_HE_5SS_MCS0);
	else if (rate >= BB_HE_4SS_MCS0)
		rate_digit = (rate - BB_HE_4SS_MCS0);
	else if (rate >= BB_HE_3SS_MCS0)
		rate_digit = (rate - BB_HE_3SS_MCS0);
	else if (rate >= BB_HE_2SS_MCS0)
		rate_digit = (rate - BB_HE_2SS_MCS0);
	else if (rate >= BB_HE_1SS_MCS0)
		rate_digit = (rate - BB_HE_1SS_MCS0);
	else if (rate >= BB_VHT_8SS_MCS0)
		rate_digit = (rate - BB_VHT_8SS_MCS0);
	else if (rate >= BB_VHT_7SS_MCS0)
		rate_digit = (rate - BB_VHT_7SS_MCS0);
	else if (rate >= BB_VHT_6SS_MCS0)
		rate_digit = (rate - BB_VHT_6SS_MCS0);
	else if (rate >= BB_VHT_5SS_MCS0)
		rate_digit = (rate - BB_VHT_5SS_MCS0);
	else if (rate >= BB_VHT_4SS_MCS0)
		rate_digit = (rate - BB_VHT_4SS_MCS0);
	else if (rate >= BB_VHT_3SS_MCS0)
		rate_digit = (rate - BB_VHT_3SS_MCS0);
	else if (rate >= BB_VHT_2SS_MCS0)
		rate_digit = (rate - BB_VHT_2SS_MCS0);
	else if (rate >= BB_VHT_1SS_MCS0)
		rate_digit = (rate - BB_VHT_1SS_MCS0);
	else if (rate >= BB_HT_MCS0)
		rate_digit = (rate - BB_HT_MCS0);
	else if (rate <= BB_54M)
		rate_digit = legacy_table[rate];

	return rate_digit;
}

/*u8 halbb_get_rx_stream_num(struct bb_info *bb, enum rf_type type)
{
	u8 rx_num = 1;

	if (type == RF_1T1R)
		rx_num = 1;
	else if (type == RF_2T2R || type == RF_1T2R)
		rx_num = 2;
	else if (type == RF_3T3R || type == RF_2T3R)
		rx_num = 3;
	else if (type == RF_4T4R || type == RF_3T4R || type == RF_2T4R)
		rx_num = 4;
	else
		BB_WARNING("%s\n", __func__);

	return rx_num;
}*/

u8 halbb_rate_type_2_num_ss(struct bb_info *bb, enum halbb_rate_type type)
{
	u8 num_ss = 1;

	switch (type) {
	case BB_CCK:
	case BB_OFDM:
	case BB_1SS:
		num_ss = 1;
		break;
	case BB_2SS:
		num_ss = 2;
		break;
	case BB_3SS:
		num_ss = 3;
		break;
	case BB_4SS:
		num_ss = 4;
		break;
	default:
		break;
	}

	return num_ss;
}

u8 halbb_rate_to_num_ss(struct bb_info *bb, u16 rate)
{
	u8 num_ss = 1;

	if (rate <= BB_54M)
		num_ss = 1;
	else if (rate <= BB_HT_MCS(31))
		num_ss = ((rate - BB_HT_MCS0) >> 3) + 1;
	else if (rate <= BB_VHT_1SS_MCS(9))
		num_ss = 1;
	else if (rate <= BB_VHT_2SS_MCS(9))
		num_ss = 2;
	else if (rate <= BB_VHT_3SS_MCS(9))
		num_ss = 3;
	else if (rate <= BB_VHT_4SS_MCS(9))
		num_ss = 4;
	else if (rate <= BB_VHT_5SS_MCS(9))
		num_ss = 5;
	else if (rate <= BB_VHT_6SS_MCS(9))
		num_ss = 6;
	else if (rate <= BB_VHT_7SS_MCS(9))
		num_ss = 7;
	else if (rate <= BB_VHT_8SS_MCS(9))
		num_ss = 8;
	else if (rate <= BB_HE_1SS_MCS(11))
		num_ss = 1;
	else if (rate <= BB_HE_2SS_MCS(11))
		num_ss = 2;
	else if (rate <= BB_HE_3SS_MCS(11))
		num_ss = 3;
	else if (rate <= BB_HE_4SS_MCS(11))
		num_ss = 4;
	else if (rate <= BB_HE_5SS_MCS(11))
		num_ss = 5;
	else if (rate <= BB_HE_6SS_MCS(11))
		num_ss = 6;
	else if (rate <= BB_HE_7SS_MCS(11))
		num_ss = 7;
	else if (rate <= BB_HE_8SS_MCS(11))
		num_ss = 8;

	return num_ss;
}

u8 halbb_ss_mcs_2_mcs_ss_idx(struct bb_info *bb, enum bb_mode_type mode, u8 ss, u8 mcs_idx)
{
	u8 mcs_ss_idx = 0;

	if (mode == BB_LEGACY_MODE) {
		mcs_ss_idx = mcs_idx;
	} else if (mode == BB_HT_MODE) {
		mcs_ss_idx = (mcs_idx & 0x7) | (ss << 3);
	} else if ((mode == BB_VHT_MODE) || (mode == BB_HE_MODE)) {
		if (bb->ic_type & BB_IC_AX_SERIES)
			mcs_ss_idx = (mcs_idx & 0xf) | (ss << 4);
		else
			mcs_ss_idx = (mcs_idx & 0xf) | (ss << 5);
	} else if (mode == BB_EHT_MODE) {
		mcs_ss_idx = (mcs_idx & 0x1f) | (ss << 5);
	}

	return mcs_ss_idx;
}

u16 halbb_gen_rate_idx(struct bb_info *bb, enum bb_mode_type mode, u8 ss, u8 idx)
{
	u16 rate_idx = 0;

	if (bb->bb_80211spec == BB_BE_IC) {
		if (mode == BB_EHT_MODE) {
			rate_idx = BB_BE_EHT_MCS(ss, idx);
		} else if (mode == BB_HE_MODE) {
			rate_idx = BB_BE_HE_MCS(ss, idx);
		} else if (mode == BB_VHT_MODE) {
			rate_idx = BB_BE_VHT_MCS(ss, idx);
		} else if (mode == BB_HT_MODE) {
			rate_idx = BB_BE_HT_MCS(idx);
		}
	} else {
		if (mode == BB_HE_MODE) {
			rate_idx = BB_HE_MCS(ss, idx);
		} else if (mode == BB_VHT_MODE) {
			rate_idx = BB_VHT_MCS(ss, idx);
		} else if (mode == BB_HT_MODE) {
			rate_idx = BB_HT_MCS(idx);
		} else {
			rate_idx = idx;
		}
	}
#if 0
	BB_DBG(bb, DBG_BIT17, "rate_idx=0x%x, mode=%d, ss=%d, idx=%d\n", rate_idx, mode, ss, idx);

	/*RX Rate*/
	halbb_print_rate_2_buff(bb, rate_idx, RTW_GILTF_LGI_4XHE32, bb->dbg_buf, HALBB_SNPRINT_SIZE);
	BB_DBG(bb, DBG_BIT17, "Rate:%s\n", bb->dbg_buf);
#endif
	return rate_idx;
}

void halbb_print_rate_2_buff(struct bb_info *bb, u16 rate_idx, enum rtw_gi_ltf gi_ltf, char *buf, u16 buf_size)
{
	struct bb_rate_info rate;
	char *ss = NULL;
	char *mode = NULL;
	char *gi = NULL;

	halbb_rate_idx_parsor(bb, rate_idx, gi_ltf, &rate);

	if (rate.mode == BB_HE_MODE)
		mode = "HE ";
	else if (rate.mode == BB_VHT_MODE)
		mode = "VHT ";
	else if (rate.mode == BB_HT_MODE)
		mode = "HT";
	else if (rate.mode == BB_EHT_MODE)
		mode = "EHT";
	else
		mode = "";

	if (rate.ss == 4)
		ss = "4";
	else if (rate.ss == 3)
		ss = "3";
	else if (rate.ss == 2)
		ss = "2";
	else
		ss = "1";

	if ((rate.mode == BB_HE_MODE) || (rate.mode == BB_EHT_MODE)) {
		if (rate.gi_ltf == RTW_GILTF_LGI_4XHE32)
			gi = "[4X32]";
		else if (rate.gi_ltf == RTW_GILTF_SGI_4XHE08)
			gi = "[4X08]";
		else if (rate.gi_ltf == RTW_GILTF_2XHE16)
			gi = "[2X16]";
		else if (rate.gi_ltf == RTW_GILTF_2XHE08)
			gi = "[2X08]";
		else if (rate.gi_ltf == RTW_GILTF_1XHE16)
			gi = "[1X16]";
		else
			gi = "[1X08]";
	} else if (rate.mode >= BB_HT_MODE) {
		if (rate.gi_ltf == RTW_GILTF_SGI_4XHE08)
			gi = "[sgi]";
		else
			gi = "";
	} else {
		gi = "";
	}

	/* BB_SNPRINTF wait driver porting */
	_os_snprintf(buf, buf_size, "(%s%s%s%s%d%s%s)",
		     mode,
		     (rate.mode >= BB_VHT_MODE) ? ss : "",
		     (rate.mode >= BB_VHT_MODE) ? "-ss " : "",
		     (rate.rate_idx >= BB_HT_MCS0) ? "MCS" : "",
		     (rate.rate_idx >= BB_HT_MCS0) ? rate.idx : bb_phy_rate_table[rate.idx],
		     gi,
		     (rate.rate_idx < BB_HT_MCS0) ? "M" : "");
}

enum bb_qam_type halbb_get_qam_order(struct bb_info *bb, u16 rate_idx)
{
	u16 tmp_idx = rate_idx;
	enum bb_qam_type qam_order = BB_QAM_BPSK;
	enum bb_qam_type qam[10] = {BB_QAM_BPSK, BB_QAM_QPSK,
				    BB_QAM_QPSK, BB_QAM_16QAM,
				    BB_QAM_16QAM, BB_QAM_64QAM,
				    BB_QAM_64QAM, BB_QAM_64QAM,
				    BB_QAM_256QAM, BB_QAM_256QAM};

	if (rate_idx <= BB_11M)
		return BB_QAM_CCK;

	if ((rate_idx >= BB_VHT_MCS(1, 0)) && (rate_idx <= BB_VHT_MCS(4, 9))) {
		if (rate_idx >= BB_VHT_MCS(4, 0))
			tmp_idx -= BB_VHT_MCS(4, 0);
		else if (rate_idx >= BB_VHT_MCS(3, 0))
			tmp_idx -= BB_VHT_MCS(3, 0);
		else if (rate_idx >= BB_VHT_MCS(2, 0))
			tmp_idx -= BB_VHT_MCS(2, 0);
		else
			tmp_idx -= BB_VHT_MCS(1, 0);

		qam_order = qam[tmp_idx];
	} else if ((rate_idx >= BB_HT_MCS(0)) && (rate_idx <= BB_HT_MCS(31))) {
		if (rate_idx >= BB_HT_MCS(24))
			tmp_idx -= BB_HT_MCS(24);
		else if (rate_idx >= BB_HT_MCS(16))
			tmp_idx -= BB_HT_MCS(16);
		else if (rate_idx >= BB_HT_MCS(8))
			tmp_idx -= BB_HT_MCS(8);
		else
			tmp_idx -= BB_HT_MCS(0);

		qam_order = qam[tmp_idx];
	} else {
		if ((rate_idx > BB_06M) && (rate_idx <= BB_54M)) {
			tmp_idx -= BB_06M;
			qam_order = qam[tmp_idx - 1];
		} else { /* OFDM 6M & all other undefine rate*/
			qam_order = BB_QAM_BPSK;
		}
	}
	return qam_order;
}

u8 halbb_rate_order_compute(struct bb_info *bb, u16 rate_idx)
{
	u16 rate_order = rate_idx & 0x7f;

	rate_idx &= 0x7f;

	if (rate_idx >= BB_VHT_MCS(4, 0))
		rate_order -= BB_VHT_MCS(4, 0);
	else if (rate_idx >= BB_VHT_MCS(3, 0))
		rate_order -= BB_VHT_MCS(3, 0);
	else if (rate_idx >= BB_VHT_MCS(2, 0))
		rate_order -= BB_VHT_MCS(2, 0);
	else if (rate_idx >= BB_VHT_MCS(1, 0))
		rate_order -= BB_VHT_MCS(1, 0);
	else if (rate_idx >= BB_HT_MCS(24))
		rate_order -= BB_HT_MCS(24);
	else if (rate_idx >= BB_HT_MCS(16))
		rate_order -= BB_HT_MCS(16);
	else if (rate_idx >= BB_HT_MCS(8))
		rate_order -= BB_HT_MCS(8);
	else if (rate_idx >= BB_HT_MCS(0))
		rate_order -= BB_HT_MCS(0);
	else if (rate_idx >= BB_06M)
		rate_order -= BB_06M;
	else
		rate_order -= BB_01M;

	if (rate_idx >= BB_HT_MCS(0))
		rate_order++;

	return (u8)rate_order;
}

u8 halbb_init_ra_by_rssi(struct bb_info *bb, u8 rssi_assoc)
{
	u8 init_ra_lv = 0;

	if (rssi_assoc > 50)
		init_ra_lv = 1;
	else if (rssi_assoc > 30)
		init_ra_lv = 2;
	else if (rssi_assoc > 1)
		init_ra_lv = 3;
	else
		init_ra_lv = 0;

	return init_ra_lv;
}

bool halbb_set_csi_rate(struct bb_info *bb, struct rtw_phl_stainfo_t *phl_sta_i)
{
	u8 macid;
	union bb_h2c_ra_cmn_info *ra_cmn;
	struct bb_h2c_ra_cfg_info *ra_cfg;
	struct rtw_hal_stainfo_t *hal_sta_i;
	struct rtw_ra_sta_info * ra_sta_i;

	if (!phl_sta_i || !bb)
		return false;
	macid = (u8)(phl_sta_i->macid);
	hal_sta_i = phl_sta_i->hal_sta;
	if (!hal_sta_i)
		return false;
	ra_cmn = &bb->bb_cmn_hooker->bb_ra_i[macid].ra_cfg;
	ra_cfg = &ra_cmn->bb_h2c_ra_info;
	ra_sta_i = &(hal_sta_i->ra_info);
	if (!ra_sta_i)
		return false;
	/*if ((!ra_sta_i->ra_csi_rate_en) && (!ra_sta_i->fixed_csi_rate_en))
		return false;*/
	/* Set csi rate ctrl enable */
	ra_cfg->ramask[7] |= BIT(7);
	ra_cfg->ra_csi_rate_en = ra_sta_i->ra_csi_rate_en;
	ra_cfg->fixed_csi_rate_en = ra_sta_i->fixed_csi_rate_en;
	ra_cfg->cr_tbl_sel = bb->hal_com->csi_para_ctrl_sel;
	ra_cfg->band_num = phl_sta_i->rlink->hw_band;
	ra_cfg->fixed_csi_rate_l = halbb_ss_mcs_2_mcs_ss_idx(bb,
							     ra_sta_i->csi_rate.mode,
							     ra_sta_i->csi_rate.ss,
							     ra_sta_i->csi_rate.mcs_idx);

	BB_DBG(bb, DBG_RA,
	       "ra_csi_rate_en=%d, fixed_csi_rate_en=%d, cr_tbl_sel=%d, band_num=%d, fixed_csi_rate_l=%x\n",
	       ra_cfg->ra_csi_rate_en, ra_cfg->fixed_csi_rate_en,
	       ra_cfg->cr_tbl_sel, ra_cfg->band_num, ra_cfg->fixed_csi_rate_l);

	return true;
}

u8 halbb_rssi_lv_dec(struct bb_info *bb, u8 rssi, u8 ratr_state)
{
	/*@MCS0 ~ MCS4 , VHT1SS MCS0 ~ MCS4 , G 6M~24M*/
	/*u8 rssi_lv_t[RA_FLOOR_TABLE_SIZE] = {20, 34, 38, 42, 46, 50, 100};*/
	u8 rssi_lv_t[RA_FLOOR_TABLE_SIZE] = {30, 44, 48, 52, 56, 60, 100};
	/*@ RSSI definition changed in AX*/
	u8 new_rssi_lv = 0;
	u8 i;

	BB_DBG(bb, DBG_RA,
	       "curr RA level=(%d), Table_ori=[%d, %d, %d, %d, %d, %d]\n",
	       ratr_state, rssi_lv_t[0], rssi_lv_t[1], rssi_lv_t[2],
	       rssi_lv_t[3], rssi_lv_t[4], rssi_lv_t[5]);
	for (i = 0; i < RA_FLOOR_TABLE_SIZE; i++) {
		if (i >= (ratr_state))
			rssi_lv_t[i] += RA_FLOOR_UP_GAP;
	}
	BB_DBG(bb, DBG_RA,
	       "RSSI=(%d), Table_mod=[%d, %d, %d, %d, %d, %d]\n", rssi,
	       rssi_lv_t[0], rssi_lv_t[1], rssi_lv_t[2], rssi_lv_t[3],
	       rssi_lv_t[4], rssi_lv_t[5]);
	for (i = 0; i < RA_FLOOR_TABLE_SIZE; i++) {
		if (rssi < rssi_lv_t[i]) {
			new_rssi_lv = i;
			break;
		}
	}
	return new_rssi_lv;
}

u64 halbb_ramask_by_rssi(struct bb_info *bb, u8 rssi_lv, u64 ramask)
{
	u64 ra_mask_bitmap = ramask;

	if (rssi_lv == 0)
		ra_mask_bitmap &= 0xffffffffffffffff;
	else if (rssi_lv == 1)
		ra_mask_bitmap &= 0xfffffffffffffff0;
	else if (rssi_lv == 2)
		ra_mask_bitmap &= 0xffffffffffffefe0;
	else if (rssi_lv == 3)
		ra_mask_bitmap &= 0xffffffffffffcfc0;
	else if (rssi_lv == 4)
		ra_mask_bitmap &= 0xffffffffffff8f80;
	else if (rssi_lv >= 5)
		ra_mask_bitmap &= 0xffffffffffff0f00;

	/*Avoid empty HT/VHT/HE ramask when HT/VHT/HE mode is enabled*/
	if ((ra_mask_bitmap >> 12) == 0x0) {
		ra_mask_bitmap |= (ramask & 0xfffffffffffff000);
		BB_DBG(bb, DBG_RA,
		       "Empty HT/VHT/HE ramask! Bypass HT/VHT/HE ramask_by_rssi\n");
	}

	/*Avoid empty legacy ramask after foolproof of HT/VHT/HE mode*/
	if (ra_mask_bitmap == 0x0) {
		ra_mask_bitmap |= (ramask & 0xfff);
		BB_DBG(bb, DBG_RA,
		       "Empty ramask! Bypass a/b/g ramask_by_rssi\n");
	}

	return ra_mask_bitmap;
}

u64 halbb_ramask_mod(struct bb_info *bb, u8 macid, u64 ramask, u8 rssi, u8 mode,
	u8 nss, u64 *ramask_h)
{
	struct bb_ra_info *bb_ra = &bb->bb_cmn_hooker->bb_ra_i[macid];
	u64 mod_mask = ramask;
	u64 mod_mask_rssi = ramask;
	u8 new_rssi_lv = 0;
	u8 wifi_mode = RA_non_ht;

	if (mode == CCK_SUPPORT) { /* B mode */
		mod_mask &= RAMASK_B;
		wifi_mode = RA_CCK;
		BB_DBG(bb, DBG_RA, "RA mask B mode\n");
	} else if (mode == OFDM_SUPPORT) { /* AG mode */
		mod_mask &= RAMASK_AG;
		wifi_mode = RA_non_ht;
		BB_DBG(bb, DBG_RA, "RA mask A mode\n");
	} else if (mode == (CCK_SUPPORT|OFDM_SUPPORT)) {
		/* BG mode */
		mod_mask &= RAMASK_BG;
		wifi_mode = RA_non_ht;
		BB_DBG(bb, DBG_RA, "RA mask 2.4G BG mode\n");
	} else if (mode == (CCK_SUPPORT|OFDM_SUPPORT|HT_SUPPORT)) {
		/* 2G N mode */
		mod_mask &= RAMASK_HT_2G;
		wifi_mode = RA_HT;
		BB_DBG(bb, DBG_RA, "RA mask 2.4G HT mode\n");
	} else if (mode == (OFDM_SUPPORT|HT_SUPPORT)) {
		/* 5G N mode */
		mod_mask &= RAMASK_HT_5G;
		wifi_mode = RA_HT;
		BB_DBG(bb, DBG_RA, "RA mask 5G HT mode\n");
	} else if (mode == (CCK_SUPPORT|OFDM_SUPPORT|VHT_SUPPORT_TX)) {
		/* 2G AC mode */
		mod_mask &= RAMASK_VHT_2G;
		wifi_mode = RA_VHT;
		BB_DBG(bb, DBG_RA, "RA mask 2.4G VHT mode\n");
	} else if (mode == (OFDM_SUPPORT|VHT_SUPPORT_TX)) {
		/* 5G AC mode */
		mod_mask &= RAMASK_VHT_5G;
		wifi_mode = RA_VHT;
		BB_DBG(bb, DBG_RA, "RA mask 5G VHT mode\n");
	} else if (mode == (CCK_SUPPORT|OFDM_SUPPORT|HE_SUPPORT)) {
		/* 2G AX mode */
		mod_mask &= RAMASK_HE_2G;
		wifi_mode = RA_HE;
		BB_DBG(bb, DBG_RA, "RA mask 2.4G HE mode\n");
	} else if (mode == (OFDM_SUPPORT|HE_SUPPORT)) {
		/* 5G AX mode */
		mod_mask &= RAMASK_HE_5G;
		wifi_mode = RA_HE;
		BB_DBG(bb, DBG_RA, "RA mask 5G HE mode\n");
	} else if (mode == (CCK_SUPPORT|OFDM_SUPPORT|EHT_SUPPORT)) {
		/* 2G BE mode */
		mod_mask &= RAMASK_EHT_2G_L;
		*ramask_h &= RAMASK_EHT_H;
		wifi_mode = RA_EHT;
		BB_DBG(bb, DBG_RA, "RA mask 2.4G BE mode\n");
	} else if (mode == (OFDM_SUPPORT|EHT_SUPPORT)) {
		/* 5G BE mode */
		mod_mask &= RAMASK_EHT_5G_L;
		*ramask_h &= RAMASK_EHT_H;
		wifi_mode = RA_EHT;
		BB_DBG(bb, DBG_RA, "RA mask 5G BE mode\n");
	} else {
		BB_WARNING("[%s] Mode=%x\n", __func__, mode);
	}
	BB_DBG(bb, DBG_RA, "RA mask SS NUM : %d\n", nss);
	if (wifi_mode == RA_HT) {
		switch (nss) {
		case RA_1SS_MODE:
			mod_mask &= RAMASK_1SS_HT;
			break;
		case RA_2SS_MODE:
			mod_mask &= RAMASK_2SS_HT;
			break;
		case RA_3SS_MODE:
			mod_mask &= RAMASK_3SS_HT;
			break;
		case RA_4SS_MODE:
			mod_mask &= RAMASK_4SS_HT;
			break;
		default:
			mod_mask &= RAMASK_1SS_HT;
			break;
		}
	} else if (wifi_mode == RA_VHT) {
		switch (nss) {
		case RA_1SS_MODE:
			mod_mask &= RAMASK_1SS_VHT;
			break;
		case RA_2SS_MODE:
			mod_mask &= RAMASK_2SS_VHT;
			break;
		case RA_3SS_MODE:
			mod_mask &= RAMASK_3SS_VHT;
			break;
		case RA_4SS_MODE:
			mod_mask &= RAMASK_4SS_VHT;
			break;
		default:
			mod_mask &= RAMASK_1SS_VHT;
			break;
		}
	} else if (wifi_mode == RA_HE) {
		switch (nss) {
		case RA_1SS_MODE:
			mod_mask &= RAMASK_1SS_HE;
			break;
		case RA_2SS_MODE:
			mod_mask &= RAMASK_2SS_HE;
			break;
		case RA_3SS_MODE:
			mod_mask &= RAMASK_3SS_HE;
			break;
		case RA_4SS_MODE:
			mod_mask &= RAMASK_4SS_HE;
			break;
		default:
			mod_mask &= RAMASK_1SS_HE;
			break;
		}
	} else if (wifi_mode == RA_EHT) {
		switch (nss) {
		case RA_1SS_MODE:
			mod_mask &= RAMASK_1SS_EHT;
			*ramask_h = 0;
			break;
		case RA_2SS_MODE:
			mod_mask &= RAMASK_2SS_EHT;
			*ramask_h = 0;
			break;
		case RA_3SS_MODE:
			mod_mask &= RAMASK_3SS_EHT;
			*ramask_h = 0;
			break;
		case RA_4SS_MODE:
			mod_mask &= RAMASK_4SS_EHT_L;
			*ramask_h &= RAMASK_4SS_EHT_H;
			break;
		default:
			mod_mask &= RAMASK_1SS_EHT;
			*ramask_h = 0;
			break;
		}
	} else {
		BB_DBG(bb, DBG_RA, "RA mask non-ht mode\n");
	}
	BB_DBG(bb, DBG_RA, "RA mask modify : %llx\n", mod_mask);
	new_rssi_lv = halbb_rssi_lv_dec(bb, rssi, bb_ra->rssi_lv);

	if (bb_ra->rssi_lv != new_rssi_lv)
		bb_ra->rssi_lv = new_rssi_lv;

	if (wifi_mode != RA_CCK) {
		mod_mask_rssi = halbb_ramask_by_rssi(bb, new_rssi_lv, mod_mask);
		//BB_DBG(bb, DBG_RA, "RA mask modify by rssi : 0x%016llx\n", mod_mask_rssi);
	} else {
		mod_mask_rssi = mod_mask;
	}
	return mod_mask_rssi;
}

void rtw_halbb_mudbg(struct bb_info *bb, u8 type, u8 mu_entry, u8 macid, 
			   bool en_256q, bool en_1024q)
{
	struct bb_h2c_mu_cfg mucfg = {0};
	u32 *bb_h2c = (u32 *)&mucfg;
	u8 cmdlen = sizeof(mucfg);

	mucfg.cmd_type = type;
	mucfg.entry = mu_entry;
	mucfg.macid = macid;
	halbb_fill_h2c_cmd(bb, cmdlen, RA_H2C_MUCFG, HALBB_H2C_RA, bb_h2c);
}

u64 halbb_gen_abg_mask(struct bb_info *bb, struct rtw_phl_stainfo_t *phl_sta_i)
{
	u64 abg_mask = 0;
	u8 i;
	struct protocol_cap_t *asoc_cap_i;

	if (!phl_sta_i)
		return 0;

	asoc_cap_i = &phl_sta_i->asoc_cap;

	BB_DBG(bb, DBG_RA,
	       "supported rates(L->H) = [%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x]\n",
	       asoc_cap_i->supported_rates[0], asoc_cap_i->supported_rates[1],
	       asoc_cap_i->supported_rates[2], asoc_cap_i->supported_rates[3],
	       asoc_cap_i->supported_rates[4], asoc_cap_i->supported_rates[5],
	       asoc_cap_i->supported_rates[6], asoc_cap_i->supported_rates[7],
	       asoc_cap_i->supported_rates[8], asoc_cap_i->supported_rates[9],
	       asoc_cap_i->supported_rates[10], asoc_cap_i->supported_rates[11]);
	for (i = 0; i < MAX_ABG_RATE_NUM; i++) {
		if (asoc_cap_i->supported_rates[i] == 0x0)
			continue;

		abg_mask |= halbb_mgnt_2_rate_mask(asoc_cap_i->supported_rates[i]);
	}
	BB_DBG(bb, DBG_RA, "gen_abgmask: 0x%llx\n", abg_mask);

	return abg_mask;
}

u64 halbb_gen_vht_mask(struct bb_info *bb,
				struct rtw_phl_stainfo_t *phl_sta_i)
{
	u8 vht_cap[2] = {0};
	u8 tmp_cap = 0;
	u8 cap_ss;
	u64 tmp_mask_nss = 0;
	u8 i;
	struct protocol_cap_t *asoc_cap_i;

	/*@Becareful RA use our "Tx" capability which means the capability of their "Rx"*/
	if (!phl_sta_i)
		return 0;
	asoc_cap_i = &phl_sta_i->asoc_cap;
	vht_cap[0] = asoc_cap_i->vht_rx_mcs[0];
	vht_cap[1] = asoc_cap_i->vht_rx_mcs[1];
	BB_DBG(bb, DBG_RA, "%s : vhtcap:%x %x\n", __func__, vht_cap[0], vht_cap[1]);
	for (i = 0; i < MAX_NSS_VHT; i++) {
		if (i == 0)
			tmp_cap = vht_cap[0];
		else if (i == 4)
			tmp_cap = vht_cap[1];

		cap_ss = tmp_cap & 0x03;
		tmp_cap = tmp_cap >> 2;
		if (cap_ss == 0)
			tmp_mask_nss |= ((u64)0xff << (i * 12));
		else if (cap_ss == 1)
			tmp_mask_nss |= ((u64)0x1ff << (i * 12));
		else if (cap_ss == 2)
			tmp_mask_nss |= ((u64)0x3ff << (i * 12));

		BB_DBG(bb, DBG_RA, "gen_vhtmask:cap%x, ss%x, hemask: 0x%llx\n",
		       cap_ss, i, tmp_mask_nss);
	}
	return tmp_mask_nss;
}

u64 halbb_gen_ht_mask(struct bb_info *bb,
				struct rtw_phl_stainfo_t *phl_sta_i)
{
	u8 ht_cap[4] = {0};
	u64 cap_ss;
	u64 tmp_mask_nss = 0;
	u8 i;
	struct protocol_cap_t *asoc_cap_i;

	/*@Becareful RA use our "Tx" capability which means the capability of their "Rx"*/
	if (!phl_sta_i)
		return 0;
	asoc_cap_i = &phl_sta_i->asoc_cap;
	for (i = 0; i < MAX_NSS_HT; i++)/* can use pointer after merge code*/
		ht_cap[i] = asoc_cap_i->ht_rx_mcs[i];
	BB_DBG(bb, DBG_RA, "%s : htcap: %x %x\n", __func__, ht_cap[0], ht_cap[1]);
	for (i = 0; i < MAX_NSS_HT; i++) {
		cap_ss = (u64)ht_cap[i];
		tmp_mask_nss = tmp_mask_nss | (cap_ss << (i * 12));
		BB_DBG(bb, DBG_RA, "gen_htmask:cap%llx, ss%x, htmask: 0x%llx\n", cap_ss, i, tmp_mask_nss);
	}

	return tmp_mask_nss;
}

u64 halbb_gen_he_mask(struct bb_info *bb,
				struct rtw_phl_stainfo_t *phl_sta_i, enum channel_width bw)
{
	u8 he_cap[2] = {0};
	u8 tmp_cap = 0;
	u8 cap_ss;
	u64 tmp_mask_nss = 0;
	u8 i;
	struct protocol_cap_t *asoc_cap_i;
	/*@Becareful RA use our "Tx" capability which means the capability of their "Rx"*/
	/*@In HE cap, mcs is correspond to channel bw"*/

	if (!phl_sta_i)
		return 0;
	asoc_cap_i = &phl_sta_i->asoc_cap;
	if (bw == CHANNEL_WIDTH_80_80) {
		he_cap[0] = asoc_cap_i->he_rx_mcs[4];
		he_cap[1] = asoc_cap_i->he_rx_mcs[5];
	} else if (bw == CHANNEL_WIDTH_160) {
		he_cap[0] = asoc_cap_i->he_rx_mcs[2];
		he_cap[1] = asoc_cap_i->he_rx_mcs[3];
	} else {
		he_cap[0] = asoc_cap_i->he_rx_mcs[0];
		he_cap[1] = asoc_cap_i->he_rx_mcs[1];
	}
	BB_DBG(bb, DBG_RA, "%s: hecap:%x %x\n", __func__, he_cap[0], he_cap[1]);
	for (i = 0; i < MAX_NSS_HE; i++) {
		if (i == 0)
			tmp_cap = he_cap[0];
		else if (i == 4)
			tmp_cap = he_cap[1];
		cap_ss = tmp_cap & 0x03;
		tmp_cap = tmp_cap >> 2;
		if (cap_ss == 0)
			tmp_mask_nss |= ((u64)0xff << (i * 12));
		else if (cap_ss == 1)
			tmp_mask_nss |= ((u64)0x3ff << (i * 12));
		else if (cap_ss == 2)
			tmp_mask_nss |= ((u64)0xfff << (i * 12));

		BB_DBG(bb, DBG_RA, "gen_hemask:cap%x, ss%x, hemask: 0x%llx\n",
		       cap_ss, i, tmp_mask_nss);
	}
	return tmp_mask_nss;
}

#ifdef BB_8922A_DVLP_SPF
u64 halbb_gen_eht_mask(struct bb_info *bb,
				struct rtw_phl_stainfo_t *phl_sta_i, enum channel_width bw)
{
	u8 eht_cap[4] = {0};
	u8 tmp_cap = 0;
	u8 rfe_idx = bb->hal_com->dev_hw_cap.rfe_type;
	u32 cap_mcs[4] = {0xff, 0x3ff, 0xfff, 0x3fff};
	u64 tmp_mask_nss = 0;
	u8 i,j;
	struct protocol_cap_t *asoc_cap_i;
	/*@Becareful RA use our "Tx" capability which means the capability of their "Rx"*/
	/*@In EHT cap, mcs is correspond to channel bw"*/

	if (!phl_sta_i)
		return 0;
	asoc_cap_i = &phl_sta_i->asoc_cap;
	if (rfe_idx > 50 && bw == CHANNEL_WIDTH_20) {
		eht_cap[0] = asoc_cap_i->eht_mcs[0];
		eht_cap[1] = asoc_cap_i->eht_mcs[1];
		eht_cap[2] = asoc_cap_i->eht_mcs[2];
		eht_cap[3] = asoc_cap_i->eht_mcs[3];
	} else if (bw <= CHANNEL_WIDTH_80) {
		eht_cap[1] = asoc_cap_i->he_rx_mcs[4];
		eht_cap[2] = asoc_cap_i->he_rx_mcs[5];
		eht_cap[3] = asoc_cap_i->he_rx_mcs[6];
	} else if (bw == CHANNEL_WIDTH_160 || bw == CHANNEL_WIDTH_80_80) {
		eht_cap[1] = asoc_cap_i->eht_mcs[7];
		eht_cap[2] = asoc_cap_i->eht_mcs[8];
		eht_cap[3] = asoc_cap_i->eht_mcs[9];
	} else {
		eht_cap[1] = asoc_cap_i->eht_mcs[10];
		eht_cap[2] = asoc_cap_i->eht_mcs[11];
		eht_cap[3] = asoc_cap_i->eht_mcs[12];
	}
	BB_DBG(bb, DBG_RA, "%s: ehtcap:%x %x %x %x\n", __func__,
		   eht_cap[0], eht_cap[1], eht_cap[2], eht_cap[3]);

	for (i = 0; i < MAX_CAP_EHT; i++) {
		tmp_cap = (eht_cap[i] >> 4) & 0xf;
		for (j = 0; j < tmp_cap; j++) {
			tmp_mask_nss |= (cap_mcs[i] << (j * 16));
			BB_DBG(bb, DBG_RA, "gen_ehtmask:cap: %x, ss: %x, ehtmask: 0x%llx\n",
				   cap_mcs[i], j, tmp_mask_nss);
		}
	}
	return tmp_mask_nss;
}

bool halbb_is_eht_rate_wifi7(struct bb_info *bb, u16 rate)
{
	return ((((rate & 0xfff) >= BE_BB_EHT_1SS_MCS0) &&
		((rate & 0xfff) <= BB_EHT_1SS_MCS(15))) ||
		(((rate & 0xfff) >= BE_BB_EHT_2SS_MCS0) &&
		((rate & 0xfff) <= BB_EHT_2SS_MCS(15))) ||
		(((rate & 0xfff) >= BE_BB_EHT_3SS_MCS0) &&
		((rate & 0xfff) <= BB_EHT_3SS_MCS(15))) ||
		(((rate & 0xfff) >= BE_BB_EHT_4SS_MCS0) &&
		((rate & 0xfff) <= BB_EHT_4SS_MCS(15)))) ? true : false;
}

bool halbb_is_he_rate_wifi7(struct bb_info *bb, u16 rate)
{
	return ((((rate & 0xfff) >= BE_BB_HE_1SS_MCS0) &&
		((rate & 0xfff) <= BB_HE_1SS_MCS(11))) ||
		(((rate & 0xfff) >= BE_BB_HE_2SS_MCS0) &&
		((rate & 0xfff) <= BB_HE_2SS_MCS(11))) ||
		(((rate & 0xfff) >= BE_BB_HE_3SS_MCS0) &&
		((rate & 0xfff) <= BB_HE_3SS_MCS(11))) ||
		(((rate & 0xfff) >= BE_BB_HE_4SS_MCS0) &&
		((rate & 0xfff) <= BB_HE_4SS_MCS(11)))) ? true : false;
}

bool halbb_is_vht_rate_wifi7(struct bb_info *bb, u16 rate)
{
	return ((((rate & 0xfff) >= BE_BB_VHT_1SS_MCS0) &&
		((rate & 0xfff) <= BB_VHT_1SS_MCS(9))) ||
		(((rate & 0xfff) >= BE_BB_VHT_2SS_MCS0) &&
		((rate & 0xfff) <= BB_VHT_2SS_MCS(9))) ||
		(((rate & 0xfff) >= BE_BB_VHT_3SS_MCS0) &&
		((rate & 0xfff) <= BB_VHT_3SS_MCS(9))) ||
		(((rate & 0xfff) >= BE_BB_VHT_4SS_MCS0) &&
		((rate & 0xfff) <= BB_VHT_4SS_MCS(9)))) ? true : false;
}

bool halbb_is_ht_rate_wifi7(struct bb_info *bb, u16 rate)
{
	return (((rate & 0xfff) >= BE_BB_HT_MCS0) &&
		((rate & 0xfff) <= BB_HT_MCS(31))) ? true : false;
}

#endif

bool halbb_chk_bw_under_20(struct bb_info *bb, u8 bw)
{
	bool ret_val = true;

	switch (bw) {
	case CHANNEL_WIDTH_5:
	case CHANNEL_WIDTH_10:
	case CHANNEL_WIDTH_20:
		ret_val = true;
		break;
	case CHANNEL_WIDTH_40:
	case CHANNEL_WIDTH_80:
	case CHANNEL_WIDTH_80_80:
	case CHANNEL_WIDTH_160:
		ret_val = false;
		break;
	default:
		ret_val = false;
		break;
	}
	return ret_val;
}

bool halbb_chk_bw_under_40(struct bb_info *bb, u8 bw)
{
	bool ret_val = true;

	switch (bw) {
	case CHANNEL_WIDTH_5:
	case CHANNEL_WIDTH_10:
	case CHANNEL_WIDTH_20:
	case CHANNEL_WIDTH_40:
		ret_val = true;
		break;
	case CHANNEL_WIDTH_80:
	case CHANNEL_WIDTH_80_80:
	case CHANNEL_WIDTH_160:
		ret_val = false;
		break;
	default:
		ret_val = false;
		break;
	}
	return ret_val;
}

bool halbb_hw_bw_mode_chk(struct bb_info *bb, u8 bw, u8 hw_mode)
{
	bool ret_val = true;

	switch (hw_mode) {
	case CCK_SUPPORT:
	case OFDM_SUPPORT:
	case (CCK_SUPPORT | OFDM_SUPPORT):
		ret_val = halbb_chk_bw_under_20(bb, bw);
		break;
	case HT_SUPPORT:
	case (HT_SUPPORT | CCK_SUPPORT):
	case (HT_SUPPORT | OFDM_SUPPORT):
	case (HT_SUPPORT | OFDM_SUPPORT | CCK_SUPPORT):
		ret_val = halbb_chk_bw_under_40(bb, bw);
		break;
	case VHT_SUPPORT_TX:
	case (VHT_SUPPORT_TX | CCK_SUPPORT):
	case (VHT_SUPPORT_TX | OFDM_SUPPORT):
	case (VHT_SUPPORT_TX | OFDM_SUPPORT | CCK_SUPPORT):
	case HE_SUPPORT:
	case (HE_SUPPORT | CCK_SUPPORT):
	case (HE_SUPPORT | OFDM_SUPPORT):
	case (HE_SUPPORT | OFDM_SUPPORT | CCK_SUPPORT):
		ret_val = true;
		break;
	default:
		ret_val = true;
		break;
	}
	if (!ret_val)
		BB_WARNING("[%s]\n", __func__);
	return ret_val;
}

u8 halbb_hw_bw_mapping(struct bb_info *bb, u8 bw, u8 hw_mode)
{
	u8 hw_bw_map = CHANNEL_WIDTH_20;
	bool ret_val;

	if (bw <= CHANNEL_WIDTH_80)
		hw_bw_map = bw;
	else if (bw == CHANNEL_WIDTH_160 || bw == CHANNEL_WIDTH_80_80)
		hw_bw_map = CHANNEL_WIDTH_160;
#ifdef BB_8922A_DVLP_SPF
	else if (bw == CHANNEL_WIDTH_320)
		hw_bw_map = CHANNEL_WIDTH_320;
#endif
	else
		hw_bw_map = CHANNEL_WIDTH_20;
	ret_val = halbb_hw_bw_mode_chk(bb, bw, hw_mode);
	return hw_bw_map;
}

u8 halbb_hw_mode_mapping(struct bb_info *bb, u8 wifi_mode)
{
	u8 hw_mode_map = 0;
	/* Driver wifi mode mapping */

	if (wifi_mode & WLAN_MD_11B) /*11B*/
		hw_mode_map |= CCK_SUPPORT;
	if ((wifi_mode & WLAN_MD_11A) || (wifi_mode & WLAN_MD_11G)) /*11G, 11A*/
		hw_mode_map |= OFDM_SUPPORT;

	/* To prevent unnecessary mode from driver, causing confusing ra mask selection after then*/
#ifdef BB_8922A_DVLP_SPF
	if (wifi_mode & WLAN_MD_11BE) /*11BE*/
		hw_mode_map |= EHT_SUPPORT;
	else if (wifi_mode & WLAN_MD_11AX) /*11AX*/
#else
	if (wifi_mode & WLAN_MD_11AX) /*11AX*/
#endif
		hw_mode_map |= HE_SUPPORT;
	else if (wifi_mode & WLAN_MD_11AC) /*11AC*/
		hw_mode_map |= VHT_SUPPORT_TX;
	else if (wifi_mode & WLAN_MD_11N) /* 11_N*/
		hw_mode_map |= HT_SUPPORT;

	if (hw_mode_map == 0)
		BB_WARNING("[%s] hw_mode_map == 0\n", __func__);

	BB_DBG(bb, DBG_RA, "drv_wifi_mode(0x%x) -> bb_wifi_mode(0x%x)\n", wifi_mode, hw_mode_map);
	return hw_mode_map;
}

bool halbb_ac_n_sgi_chk(struct bb_info *bb, u8 hw_mode, bool en_sgi)
{
	bool sgi_chk = en_sgi;

	/* Driver wifi mode mapping */
	if (hw_mode & HE_SUPPORT) {
		sgi_chk = false;
		BB_DBG(bb, DBG_RA, "HE mode sgi is not used!\n");
	}
	return sgi_chk;
}

u8 halbb_ax_giltf_chk(struct bb_info *bb, u8 hw_mode, u8 giltf_cap)
{
	u8 giltf_chk = giltf_cap;
	
	/* Driver wifi mode mapping */
	if (!(hw_mode & HE_SUPPORT)) {
		giltf_chk = 0;
		BB_DBG(bb, DBG_RA, "In non-HE mode gi_ltf is not used!\n");
	}
	return giltf_chk;
}

bool halbb_ldpc_chk(struct bb_info *bb,  struct rtw_phl_stainfo_t *phl_sta_i, u8 hw_mode)
{
	struct protocol_cap_t *asoc_cap_i;
	bool ldpc_en = false;

	/* Driver wifi mode mapping */
	if (!phl_sta_i)
		return false;
	asoc_cap_i = &phl_sta_i->asoc_cap;
	if (hw_mode & HE_SUPPORT) {
		ldpc_en |= asoc_cap_i->he_ldpc;
		BB_DBG(bb, DBG_RA, "Enable HE LDPC\n");
	} else if (hw_mode&VHT_SUPPORT_TX) {
		ldpc_en |= asoc_cap_i->vht_ldpc;
		BB_DBG(bb, DBG_RA, "Enable VHT LDPC\n");
	} else if  (hw_mode&HT_SUPPORT) {
		ldpc_en |= asoc_cap_i->ht_ldpc;
		BB_DBG(bb, DBG_RA, "Enable HT LDPC\n");
	}
	return ldpc_en;
}

u8 halbb_nss_mapping(struct bb_info *bb,  u8 nss)
{
	u8 mapping_nss = 0;

	if (nss != 0)
		mapping_nss = nss - 1;
	/* Driver tx_nss mapping */
	if (mapping_nss > (bb->phl_com->phy_cap[bb->bb_phy_idx].tx_path_num - 1))
		mapping_nss = bb->phl_com->phy_cap[bb->bb_phy_idx].tx_path_num - 1;
	return mapping_nss;
}

bool halbb_stbc_mapping(struct bb_info *bb,  struct rtw_phl_stainfo_t *phl_sta_i, u8 hw_mode)
{
	struct protocol_cap_t *asoc_cap_i;
	bool stbc_en = false;

	/* Driver wifi mode mapping */
	if (!phl_sta_i)
		return false;
	asoc_cap_i = &phl_sta_i->asoc_cap;
	if (hw_mode & HE_SUPPORT) {
		if (asoc_cap_i->stbc_he_rx != 0)
			stbc_en = true;
		BB_DBG(bb, DBG_RA, "HE STBC %d\n", stbc_en);
	} else if (hw_mode & VHT_SUPPORT_TX) {
		if (asoc_cap_i->stbc_vht_rx != 0)
			stbc_en = true;
		BB_DBG(bb, DBG_RA, "VHT STBC %d\n", stbc_en);
	} else if  (hw_mode & HT_SUPPORT) {
		if (asoc_cap_i->stbc_ht_rx != 0)
			stbc_en = true;
		BB_DBG(bb, DBG_RA, "HT STBC %d\n", stbc_en);
	}
	return stbc_en;
}

bool halbb_sgi_chk(struct bb_info *bb,  struct rtw_phl_stainfo_t *phl_sta_i, u8 hw_bw)
{
	struct protocol_cap_t *asoc_cap_i;
	bool sgi_en = false;

	/* Driver wifi mode mapping */
	if (!phl_sta_i)
		return false;
	asoc_cap_i = &phl_sta_i->asoc_cap;
	if (hw_bw == CHANNEL_WIDTH_20) {
		sgi_en = asoc_cap_i->sgi_20;
		BB_DBG(bb, DBG_RA, "Enable 20M SGI\n");
	} else if (hw_bw == CHANNEL_WIDTH_40) {
		sgi_en = asoc_cap_i->sgi_40;
		BB_DBG(bb, DBG_RA, "Enable 40M SGI\n");
	} else if  (hw_bw == CHANNEL_WIDTH_80) {
		sgi_en = asoc_cap_i->sgi_80;
		BB_DBG(bb, DBG_RA, "Enable 80M SGI\n");
	} else if  (hw_bw == CHANNEL_WIDTH_160) {
		sgi_en = asoc_cap_i->sgi_160;
		BB_DBG(bb, DBG_RA, "Enable 160M SGI\n");
	}
	return sgi_en;
}

void halbb_ramask_trans(struct bb_info *bb, u8 macid, u64 mask, u64 mask_h)
{
	union bb_h2c_ra_cmn_info *ra_cmn = &bb->bb_cmn_hooker->bb_ra_i[macid].ra_cfg;
	struct bb_h2c_ra_cfg_info *ra_cfg = &ra_cmn->bb_h2c_ra_info;

	ra_cfg->ramask[0] = (u8)(mask & 0x00000000000000ff);
	ra_cfg->ramask[1] = (u8)((mask & 0x000000000000ff00)>>8);
	ra_cfg->ramask[2] = (u8)((mask & 0x0000000000ff0000)>>16);
	ra_cfg->ramask[3] = (u8)((mask & 0x00000000ff000000)>>24);
	ra_cfg->ramask[4] = (u8)((mask & 0x000000ff00000000)>>32);
	ra_cfg->ramask[5] = (u8)((mask & 0x0000ff0000000000)>>40);
	ra_cfg->ramask[6] = (u8)((mask & 0x00ff000000000000)>>48);
	ra_cfg->ramask[7] = (u8)((mask & 0xff00000000000000)>>56);
	if (bb->ic_type & BB_IC_BE_SERIES) {
		ra_cmn->bb_h2c_ra_info_wifi7.ramask[8] = (u8)(mask_h & 0x00000000000000ff);
		ra_cmn->bb_h2c_ra_info_wifi7.ramask[9] = (u8)((mask_h & 0x000000000000ff00)>>8);
		ra_cmn->bb_h2c_ra_info_wifi7.ramask[10] = (u8)((mask_h & 0x0000000000ff0000)>>16);
		ra_cmn->bb_h2c_ra_info_wifi7.ramask[11] = (u8)((mask_h & 0x00000000ff000000)>>24);
	}
}

u8 halbb_get_opt_giltf(struct bb_info *bb, u8 assoc_giltf)
{
	u8 i =0;
	u8 opt_gi_ltf = 0;

	if (assoc_giltf & BIT(1)) /* cap. for 4x0.8*/
		opt_gi_ltf |= BIT(BB_OPT_GILTF_4XHE08);
	else if (assoc_giltf & BIT(5)) /* cap. for 1x0.8*/
		opt_gi_ltf |= BIT(BB_OPT_GILTF_1XHE08);

	BB_DBG(bb, DBG_RA, "Ass GILTF=%x,opt GILTF=%x\n", assoc_giltf, opt_gi_ltf);
	return opt_gi_ltf;
}

u8 halbb_giltf_trans(struct bb_info *bb, u8 assoc_giltf, u8 cal_giltf)
{
	u8 i =0;

	BB_DBG(bb, DBG_RA, "Ass GILTF=%x,Cal GILTF=%x\n", assoc_giltf, cal_giltf);
	if (cal_giltf == RTW_GILTF_LGI_4XHE32 && (assoc_giltf & BIT(0)))
		return cal_giltf;
	else if (cal_giltf == RTW_GILTF_SGI_4XHE08 && (assoc_giltf & BIT(1)))
		return cal_giltf;
	else if (cal_giltf == RTW_GILTF_2XHE16 && (assoc_giltf & BIT(2)))
		return cal_giltf;
	else if (cal_giltf == RTW_GILTF_2XHE08 && (assoc_giltf & BIT(3)))
		return cal_giltf;
	else if (cal_giltf == RTW_GILTF_1XHE16 && (assoc_giltf & BIT(4)))
		return cal_giltf;
	else if (cal_giltf == RTW_GILTF_1XHE08 && (assoc_giltf & BIT(5)))
		return cal_giltf;
	if (assoc_giltf & BIT(3))
		return RTW_GILTF_2XHE08;
	else if (assoc_giltf & BIT(2))
		return RTW_GILTF_2XHE16;
	else if (assoc_giltf & BIT(1))
		return RTW_GILTF_SGI_4XHE08;
	else if (assoc_giltf & BIT(5))
		return RTW_GILTF_1XHE08;
	else if (assoc_giltf & BIT(4))
		return RTW_GILTF_1XHE16;
	else
		return RTW_GILTF_LGI_4XHE32;
}

bool rtw_halbb_dft_mask(struct bb_info *bb_0,
				struct rtw_phl_stainfo_t *phl_sta_i)
{
	struct bb_info *bb = bb_0;
	u8 mode = 0; /* connect to phl->assoc*/
	u8 hw_md;
	u64 init_mask = 0;
	u64 get_mask = 0;
	u32 mask0, mask1, mask2 = 0;
	enum channel_width bw;
	struct rtw_hal_stainfo_t *hal_sta_i;
	struct protocol_cap_t *asoc_cap_i;

#if 1
	if (!phl_sta_i || !bb) {
		BB_WARNING("[%s][0] phl_sta_i is NULL!!!\n", __func__);
		return false;
	}
	hal_sta_i = phl_sta_i->hal_sta;

	if (!hal_sta_i) {
		BB_WARNING("[%s][1] hal_sta_i is NULL!!!\n", __func__);
		return false;
	}

	if (phl_sta_i->rlink->hw_band == HW_BAND_0) {
		BB_DBG(bb, DBG_RA, "[0] MACID=%d, phy_idx=%d\n", phl_sta_i->macid, bb->bb_phy_idx);
	}	
	#ifdef HALBB_DBCC_SUPPORT
	else if (phl_sta_i->rlink->hw_band == HW_BAND_1) {
		HALBB_GET_PHY_PTR(bb_0, bb, HW_PHY_1);
		BB_DBG(bb, DBG_RA, "[1] MACID=%d, phy_idx=%d\n", phl_sta_i->macid, bb->bb_phy_idx);
	}
	#endif
	else {
		BB_WARNING("[%s]MACID not exist\n", __func__);
		return false;
	}

	BB_DBG(bb, DBG_RA, "====>%s\n", __func__);
#else
	if (!phl_sta_i || !bb) {
		BB_WARNING("[%s][1]\n", __func__);
		return false;
	}
	hal_sta_i = phl_sta_i->hal_sta;
	if (!hal_sta_i) {
		BB_WARNING("[%s][2]\n", __func__);
		return false;
	}
#endif
	asoc_cap_i = &phl_sta_i->asoc_cap;
	mode = phl_sta_i->wmode;
	bw = phl_sta_i->chandef.bw;
	hw_md = halbb_hw_mode_mapping(bb, mode);
	BB_DBG(bb, DBG_RA, "Gen Dftmask: mode = %x, hw_md = %x\n", mode, hw_md);

	if (hw_md & (CCK_SUPPORT | OFDM_SUPPORT))
		init_mask = halbb_gen_abg_mask(bb, phl_sta_i);

	if (init_mask == 0) {
		if (hw_md & CCK_SUPPORT) {
			init_mask |= 0x0000000f;
			BB_DBG(bb, DBG_RA,
			       "[%s]abg mask is null!, set b mask=0xf\n", __func__);
		}

		if (hw_md & OFDM_SUPPORT) {
			init_mask |= 0x00000ff0;
			BB_DBG(bb, DBG_RA,
			       "[%s]abg mask is null!, set ag mask=0xff\n", __func__);
		}
	}
#ifdef BB_8922A_DVLP_SPF
	if (hw_md & EHT_SUPPORT) {
		get_mask = halbb_gen_eht_mask(bb, phl_sta_i, bw);
		mask2 = (get_mask >> 52) & 0xfff;
		hal_sta_i->ra_info.cur_ra_mask_h= mask2;
	} else if (hw_md & HE_SUPPORT)
#else 
	if (hw_md & HE_SUPPORT)
#endif
		get_mask = halbb_gen_he_mask(bb, phl_sta_i, bw);
	else if (hw_md & VHT_SUPPORT_TX)
		get_mask = halbb_gen_vht_mask(bb, phl_sta_i);
	else if (hw_md & HT_SUPPORT)
		get_mask = halbb_gen_ht_mask(bb, phl_sta_i);
	else
		get_mask = 0;
	init_mask |= (get_mask << 12);
	mask0 = (u32)(init_mask & 0xffffffff);
	mask1 = (u32)((init_mask >> 32) & 0xffffffff);
	if (init_mask != 0) {
		hal_sta_i->ra_info.cur_ra_mask = init_mask;
#ifdef BB_8922A_DVLP_SPF
		hal_sta_i->ra_info.cur_ra_mask_h = mask2;
#endif
		BB_DBG(bb, DBG_RA, "Default mask = %x %x %x\n", mask0, mask1, mask2);
		return true;
	} else {
		BB_WARNING("[%s]Error default mask, it should not zero\n", __func__);
		return false;
	}
}

u8 rtw_halbb_arfr_trans(struct bb_info *bb,
	struct rtw_phl_stainfo_t *phl_sta_i)
{
	struct rtw_hal_stainfo_t *hal_sta_i;
	u8 mode;
	u8 arfr_ret = 0x0;

	if (!phl_sta_i)
		return false;
	hal_sta_i = phl_sta_i->hal_sta;
	if (!hal_sta_i)
		return false;
	mode = phl_sta_i->wmode;
	if (mode & WLAN_MD_11B) /*11B*/
		arfr_ret |= CCK_SUPPORT;
	if ((mode & WLAN_MD_11A)||(mode & WLAN_MD_11G)) /*11G, 11A*/
		arfr_ret |= OFDM_SUPPORT;
	if (mode & WLAN_MD_11N) /* 11_N*/
		arfr_ret |= HT_SUPPORT;
	if (mode & WLAN_MD_11AC) /*11AC*/
		arfr_ret |= VHT_SUPPORT_TX;
	return arfr_ret;
	/*if (mode|WLAN_MD_11AX ) // 11AX usually can use arfr
		hw_mode_map |= HE_SUPPORT;*/
}

bool halbb_ra_cfg_chk(struct bb_info *bb, u8 *mode, u8 *tx_nss, bool *tx_stbc)
{
	bool rpt = true;

	if (bb->hal_com->dbcc_en && bb->num_rf_path == 2) {
		if (*tx_stbc) {
			BB_WARNING("[%s] stbc_en=%d", __func__, tx_stbc[0]);
			*tx_stbc = false;
			rpt = false;
		}

		if (*tx_nss == 1) { /*2SS*/
			BB_WARNING("[%s] mapping_nss=%d", __func__, tx_nss[0]);
			*tx_nss = 0;
			rpt = false;
		}

		if (*mode == 0) {
			BB_WARNING("[%s] mode=%d", __func__, mode[0]);
			rpt = false;
		}
	}
	return rpt;
}

bool rtw_halbb_raregistered(struct bb_info *bb_0, struct rtw_phl_stainfo_t *phl_sta_i)
{
	struct bb_info *bb = bb_0;
	/*struct rtw_phl_stainfo_t *phl_sta_i =  bb->phl_sta_info[macid];*/
	u8 macid;
	union bb_h2c_ra_cmn_info *ra_cmn;
	struct bb_h2c_ra_cfg_info *ra_cfg;
	struct rtw_hal_stainfo_t *hal_sta_i;
	struct rtw_wifi_role_link_t *rlink = phl_sta_i->rlink;
	bool tx_ldpc;
	bool tx_stbc;
	bool ret_val = false;
	u8 tx_nss;
	//u8 rssi;
	struct protocol_cap_t *asoc_cap_i;
	u8 rssi_assoc ;
	u8 mode;
	bool en_sgi = false;
	u8 giltf_cap = 0;
	u8 init_lv;
	/* Need to mapping with driver wifi mode*/
	u32 *bb_h2c;
	u8 cmdlen = sizeof(union bb_h2c_ra_cmn_info);
	u64 mod_mask, mod_mask_h = 0;
	u8 rfe_idx = bb->phl_com->dev_cap.rfe_type;

	if (!phl_sta_i || !bb) {
		BB_WARNING("[%s][0] phl_sta_i is NULL!!!\n", __func__);
		return false;
	}
	macid = (u8)(phl_sta_i->macid);
	hal_sta_i = phl_sta_i->hal_sta;

	if (!hal_sta_i) {
		BB_WARNING("[%s][1] hal_sta_i is NULL!!!\n", __func__);
		return false;
	}

	if (phl_sta_i->rlink->hw_band == HW_BAND_0) {
		BB_DBG(bb, DBG_RA, "[0] MACID=%d, phy_idx=%d\n", macid, bb->bb_phy_idx);
	}	
	#ifdef HALBB_DBCC_SUPPORT
	else if (phl_sta_i->rlink->hw_band == HW_BAND_1) {
		HALBB_GET_PHY_PTR(bb_0, bb, HW_PHY_1);
		BB_DBG(bb, DBG_RA, "[1] MACID=%d, phy_idx=%d\n", macid, bb->bb_phy_idx);
	}
	#endif
	else {
		BB_WARNING("[%s]MACID not exist\n", __func__);
		return false;
	}

	ra_cmn = &bb->bb_cmn_hooker->bb_ra_i[macid].ra_cfg;
	ra_cfg = &ra_cmn->bb_h2c_ra_info;
	bb_h2c = ra_cmn->val;

	BB_DBG(bb, DBG_RA, "====>%s\n", __func__);

	asoc_cap_i = &phl_sta_i->asoc_cap;
	/*@ use assoc rssi to init ra, only use in ra register, it is an integer using U(8,0)*/
	rssi_assoc = (u8)(hal_sta_i->rssi_stat.assoc_rssi);
	//rssi = (u8)(hal_sta_i->rssi_stat.rssi) >> 1; //unused now
	//mode = phl_sta_i->wmode;
	init_lv = halbb_init_ra_by_rssi(bb, rssi_assoc);

	BB_DBG(bb, DBG_RA, "Phy_idx=%d, RSSI_LV=%d, rssi_assoc=%d\n", bb->bb_phy_idx, init_lv, rssi_assoc);

	/*@Becareful RA use our "Tx" capability which means the capability of their "Rx"*/
	tx_nss = halbb_nss_mapping(bb, asoc_cap_i->nss_rx);
	if (asoc_cap_i->dcm_max_const_rx)
		ra_cfg->dcm_cap = 1;
	else
		ra_cfg->dcm_cap = 0;

	BB_DBG(bb, DBG_RA,
	       "Mode=%s%s%s%s%s\n",
	       (phl_sta_i->wmode & WLAN_MD_11B) ? "B" : " ",
	       (phl_sta_i->wmode & (WLAN_MD_11G | WLAN_MD_11A)) ? "G" : " ",
	       (phl_sta_i->wmode & WLAN_MD_11N) ? "N" : " ",
	       (phl_sta_i->wmode & WLAN_MD_11AC) ? "AC" : " ",
	       (phl_sta_i->wmode & WLAN_MD_11AX) ? "AX" : " ");

	mode = halbb_hw_mode_mapping(bb, phl_sta_i->wmode);

	/* ONLY need to get the optional gi-ltf combination for H2C FW*/
	/* bit(0)=4x0.8, bit(1)=1x0.8 -> different definition from the drver giltf*/
	giltf_cap = halbb_get_opt_giltf(bb, asoc_cap_i->ltf_gi);
	ra_cfg->giltf_cap = halbb_ax_giltf_chk(bb, mode, giltf_cap);
	ra_cfg->bw_cap = halbb_hw_bw_mapping(bb, phl_sta_i->chandef.bw, mode);

	/* giltf assigned by phl/halbb or trained by FW*/
	if (hal_sta_i->ra_info.fix_giltf_en == false) { /*giltf from halbb*/
		#if 0
		if (bb->ic_type & (BB_RTL8852A | BB_RTL8852B | BB_RTL8852C)) { /*Disable FW train GI_LTF*/
			ra_cfg->fix_giltf_en = true;
			ra_cfg->fix_giltf = RTW_GILTF_2XHE08;
			/* gi_ltf is decided by delay spread. This is not ready.
			if (is_giltf_decided_by_delay_sp)
				ra_cfg->fix_giltf = halbb_ra_giltf_ctrl(bb, macid, delay_sp, asoc_cap_i->ltf_gi);
			*/
		} else { /*Enable FW train GI_LTF*/
			ra_cfg->fix_giltf_en = false;
		}
		#endif
		/*Disable FW train GI_LTF*/
		ra_cfg->fix_giltf_en = true;
		if ((ra_cfg->giltf_cap & BIT(0)) && (bb->ic_type & BB_RTL8852C)
			&& (ra_cfg->bw_cap == CHANNEL_WIDTH_160) && (rfe_idx < 50))
			ra_cfg->fix_giltf = RTW_GILTF_SGI_4XHE08;
		else
			ra_cfg->fix_giltf = RTW_GILTF_2XHE08;
	} else { /*giltf from phl*/
		ra_cfg->fix_giltf = halbb_giltf_trans(bb, asoc_cap_i->ltf_gi, hal_sta_i->ra_info.cal_giltf);
	}
	BB_DBG(bb, DBG_RA, "fix_giltf_en(phl)=%d, fix_giltf_en(Final)=%d, fix_giltf=%d\n",
	       hal_sta_i->ra_info.fix_giltf_en, ra_cfg->fix_giltf_en, ra_cfg->fix_giltf);

	ra_cfg->is_dis_ra = hal_sta_i->ra_info.dis_ra;
	mod_mask = hal_sta_i->ra_info.cur_ra_mask;
#ifdef BB_8922A_DVLP_SPF
	mod_mask_h = hal_sta_i->ra_info.cur_ra_mask_h;
#endif
	BB_DBG(bb, DBG_RA, "ra mask from phl = %llx, %llx\n", mod_mask, mod_mask_h);
	ra_cfg->er_cap = asoc_cap_i->er_su;
	ra_cfg->partial_bw_su_er = asoc_cap_i->partial_bw_su_er;
	tx_stbc = halbb_stbc_mapping(bb, phl_sta_i, mode);
	ra_cfg->upd_all= true;
	ra_cfg->upd_bw_nss_mask= false;
	ra_cfg->upd_mask= false;

	if (!(bb->ic_type == BB_IC_BE_SERIES)
		&& !(halbb_ra_cfg_chk(bb, &mode, &tx_nss, &tx_stbc))) {
		BB_WARNING("[%s] mode=%d, tx_nss=%d, tx_stbc=%d\n", __func__, mode, tx_nss, tx_stbc);
		return false;
	}
	if (mode == 0) {
		BB_WARNING("[%s] mode=%d\n", __func__, mode);
		return false;
	}
	tx_ldpc = halbb_ldpc_chk(bb, phl_sta_i, mode);
	if (bb->ic_type == BB_IC_BE_SERIES) {
		ra_cmn->bb_h2c_ra_info_wifi7.mode_ctrl_eht= mode;
		ra_cmn->bb_h2c_ra_info_wifi7.bw_cap_eht= halbb_hw_bw_mapping(bb, phl_sta_i->chandef.bw, mode);
		en_sgi = halbb_sgi_chk(bb, phl_sta_i, ra_cmn->bb_h2c_ra_info_wifi7.bw_cap_eht);
	}
	ra_cfg->mode_ctrl = mode;
	en_sgi = halbb_sgi_chk(bb, phl_sta_i, ra_cfg->bw_cap);
	ra_cfg->macid = macid;
	ra_cfg->init_rate_lv = init_lv;
	ra_cfg->en_sgi = halbb_ac_n_sgi_chk(bb, mode, en_sgi);
	ra_cfg->ldpc_cap = tx_ldpc;
	ra_cfg->stbc_cap = tx_stbc;
	ra_cfg->ss_num = tx_nss;
	ra_cfg->band = rlink->chandef.band;
	if (bb->ic_type == BB_RTL8192XB)
		ra_cfg->is_new_bb_ra_dbgreg = true;
	else
		ra_cfg->is_new_bb_ra_dbgreg = false;
	/*@ modify ra mask by assoc rssi*/
	mod_mask = halbb_ramask_mod(bb, macid, mod_mask, rssi_assoc, mode, tx_nss, &mod_mask_h);
	halbb_ramask_trans(bb, macid, mod_mask, mod_mask_h);
	ret_val = halbb_set_csi_rate(bb, phl_sta_i);
	if (!ret_val)
		return ret_val;
	BB_DBG(bb, DBG_RA, "RA Register=>In: Dis_ra=%x, MD=%x, BW=%x, macid=%x, band=%d\n",
		   hal_sta_i->ra_info.dis_ra, mode, phl_sta_i->chandef.bw, macid, rlink->chandef.band);
	BB_DBG(bb, DBG_RA, "RA Register=>In: DCM =%x, ER=%x, Par_ER=%x, in_rt=%x, upd_a=%x, sgi=%x, ldpc=%x, stbc=%x\n",
		   ra_cfg->dcm_cap, ra_cfg->er_cap, ra_cfg->partial_bw_su_er, init_lv, ra_cfg->upd_all,
		   en_sgi, tx_ldpc, tx_stbc);
	BB_DBG(bb, DBG_RA, "RA Register=>In: SS=%x, GILTF_cap=%x, upd_bnm=%x, upd_m=%x, mask=%llx\n",
		   tx_nss, giltf_cap, ra_cfg->upd_bw_nss_mask, ra_cfg->upd_mask, mod_mask);
	BB_DBG(bb, DBG_RA, "RA Register=>Out racfg: dis%x bw%x md%x mid%x\n", ra_cfg->is_dis_ra,
		   ra_cfg->bw_cap, ra_cfg->mode_ctrl, ra_cfg->macid);
	BB_DBG(bb, DBG_RA, "RA Register=>Out h2cp: %x %x %x %x %x %x\n", bb_h2c[0], bb_h2c[1],
		   bb_h2c[2], bb_h2c[3], bb_h2c[4], bb_h2c[5]);
	ret_val = halbb_fill_h2c_cmd(bb, cmdlen, RA_H2C_MACIDCFG, HALBB_H2C_RA, bb_h2c);
	return ret_val;
}

bool rtw_halbb_ra_deregistered(struct bb_info *bb,
	struct rtw_phl_stainfo_t *phl_sta_i)
{
	return true;
}

bool rtw_halbb_raupdate(struct bb_info *bb_0,
	struct rtw_phl_stainfo_t *phl_sta_i)
{
	/* Update only change bw, nss, ramask */
	struct bb_info *bb = bb_0;
	u8 macid;
	union bb_h2c_ra_cmn_info *ra_cmn;
	struct bb_h2c_ra_cfg_info *ra_cfg;
	struct rtw_hal_stainfo_t *hal_sta_i;
	struct protocol_cap_t *asoc_cap_i;
	u8 tx_nss;
	u8 rssi;
	u8 init_lv = 0;
	u8 mode = 0;
	u8 giltf_cap = 0;
	bool ret_val = false;
	/* Need to mapping with driver wifi mode*/
	u32 *bb_h2c;
	u8 cmdlen = sizeof(union bb_h2c_ra_cmn_info);
	u64 mod_mask, mod_mask_h = 0;
	u8 rfe_idx = bb->phl_com->dev_cap.rfe_type;

#if 1
	if (!phl_sta_i || !bb) {
		BB_WARNING("[%s][0] phl_sta_i is NULL!!!\n", __func__);
		return false;
	}
	macid = (u8)(phl_sta_i->macid);
	hal_sta_i = phl_sta_i->hal_sta;

	if (!hal_sta_i) {
		BB_WARNING("[%s][1] hal_sta_i is NULL!!!\n", __func__);
		return false;
	}

	if (phl_sta_i->rlink->hw_band == HW_BAND_0) {
		BB_DBG(bb, DBG_RA, "[0] MACID=%d, phy_idx=%d\n", macid, bb->bb_phy_idx);
	}	
	#ifdef HALBB_DBCC_SUPPORT
	else if (phl_sta_i->rlink->hw_band == HW_BAND_1) {
		HALBB_GET_PHY_PTR(bb_0, bb, HW_PHY_1);
		BB_DBG(bb, DBG_RA, "[1] MACID=%d, phy_idx=%d\n", macid, bb->bb_phy_idx);
	}
	#endif
	else {
		BB_WARNING("[%s]MACID not exist\n", __func__);
		return false;
	}

	ra_cmn = &bb->bb_cmn_hooker->bb_ra_i[macid].ra_cfg;
	ra_cfg = &ra_cmn->bb_h2c_ra_info;
	bb_h2c = ra_cmn->val;

	BB_DBG(bb, DBG_RA, "====>%s\n", __func__);
#else
	BB_DBG(bb, DBG_RA, "====>%s\n", __func__);
	if (!phl_sta_i || !bb) {
		BB_WARNING("[%s][1] Pointer is NULL!!!\n", __func__);
		return ret_val;
	}
	macid = (u8) (phl_sta_i->macid);
	hal_sta_i = phl_sta_i->hal_sta;
	if (!hal_sta_i) {
		BB_WARNING("[%s][2] Pointer is NULL!!!\n", __func__);
		return ret_val;
	}
	ra_cfg = &bb->bb_cmn_hooker->bb_ra_i[macid].ra_cfg;
	mode = ra_cfg->mode_ctrl;
	bb_h2c = (u32 *) ra_cfg;
#endif

	asoc_cap_i = &phl_sta_i->asoc_cap;
	rssi = hal_sta_i->rssi_stat.rssi >> 1;
	/*@Becareful RA use our "Tx" capability which means the capability of their "Rx"*/
	tx_nss = halbb_nss_mapping(bb, asoc_cap_i->nss_rx);
	ra_cfg->is_dis_ra = hal_sta_i->ra_info.dis_ra;
	mod_mask = hal_sta_i->ra_info.cur_ra_mask;
#ifdef BB_8922A_DVLP_SPF
	mod_mask_h = hal_sta_i->ra_info.cur_ra_mask_h;
#endif
	BB_DBG(bb, DBG_RA, "ra mask from phl = %llx, %llx\n", mod_mask, mod_mask_h);
	ra_cfg->upd_all= false;
	ra_cfg->upd_bw_nss_mask= true;
	ra_cfg->upd_mask= false;
	mode = ra_cfg->mode_ctrl;

	if (mode == 0) {
		BB_WARNING("[%s] mode=%d\n", __func__, mode);
		return false;
	}

	ra_cfg->mode_ctrl = mode;

	/* ONLY need to get the optional gi-ltf combination for H2C FW*/
	/* bit(0)=4x0.8, bit(1)=1x0.8 -> different definition from the drver giltf*/
	giltf_cap = halbb_get_opt_giltf(bb, asoc_cap_i->ltf_gi);
	ra_cfg->giltf_cap = halbb_ax_giltf_chk(bb, mode, giltf_cap);
	if (bb->ic_type == BB_IC_BE_SERIES)
		ra_cmn->bb_h2c_ra_info_wifi7.bw_cap_eht = halbb_hw_bw_mapping(bb, phl_sta_i->chandef.bw, mode);
	else
		ra_cfg->bw_cap = halbb_hw_bw_mapping(bb, phl_sta_i->chandef.bw, mode);

	/* giltf assigned by phl/halbb or trained by FW*/
	if (hal_sta_i->ra_info.fix_giltf_en == false) { /*giltf from halbb*/
		#if 0
		if (bb->ic_type & (BB_RTL8852A | BB_RTL8852B | BB_RTL8852C)) { /*Disable FW train GI_LTF*/
			ra_cfg->fix_giltf_en = true;
			ra_cfg->fix_giltf = RTW_GILTF_2XHE08;
			/* gi_ltf is decided by delay spread. This is not ready.
			if (is_giltf_decided_by_delay_sp)
				ra_cfg->fix_giltf = halbb_ra_giltf_ctrl(bb, macid, delay_sp, asoc_cap_i->ltf_gi);
			*/
		} else { /*Enable FW train GI_LTF*/
			ra_cfg->fix_giltf_en = false;
		}
		#endif
		/*Disable FW train GI_LTF*/
		ra_cfg->fix_giltf_en = true;
		if ((ra_cfg->giltf_cap & BIT(0)) && (bb->ic_type & BB_RTL8852C)
			&& (ra_cfg->bw_cap == CHANNEL_WIDTH_160) && (rfe_idx < 50))
			ra_cfg->fix_giltf = RTW_GILTF_SGI_4XHE08;
		else
			ra_cfg->fix_giltf = RTW_GILTF_2XHE08;
	} else { /*giltf from phl*/
		ra_cfg->fix_giltf = halbb_giltf_trans(bb, asoc_cap_i->ltf_gi, hal_sta_i->ra_info.cal_giltf);
	}
	BB_DBG(bb, DBG_RA, "fix_giltf_en(phl)=%d, fix_giltf_en(Final)=%d, fix_giltf=%d\n",
	       hal_sta_i->ra_info.fix_giltf_en, ra_cfg->fix_giltf_en, ra_cfg->fix_giltf);

	ra_cfg->init_rate_lv = 0;
	ra_cfg->ss_num = tx_nss;
	mod_mask = halbb_ramask_mod(bb, macid, mod_mask, rssi, mode, tx_nss, &mod_mask_h);
	halbb_ramask_trans(bb, macid, mod_mask, mod_mask_h);
	BB_DBG(bb, DBG_RA, "RA Register=>Out h2cp: %x %x %x %x %x %x\n", bb_h2c[0], bb_h2c[1],
		   bb_h2c[2], bb_h2c[3], bb_h2c[4], bb_h2c[5]);
	ret_val = halbb_fill_h2c_cmd(bb, cmdlen, RA_H2C_MACIDCFG, HALBB_H2C_RA, bb_h2c);
	return ret_val;
}

bool halbb_ra_update_mask_watchdog(struct bb_info *bb, struct rtw_phl_stainfo_t *phl_sta_i)
{
	u8 macid;
	union bb_h2c_ra_cmn_info *ra_cmn;
	struct bb_h2c_ra_cfg_info *ra_cfg;
	struct rtw_hal_stainfo_t *hal_sta_i;
	struct protocol_cap_t *asoc_cap_i;
	u8 tx_nss;
	u8 rssi;
	u8 init_lv = 0;
	u8 mode = 0;
	u8 giltf_cap = 0;
	bool ret_val = false;
	/* Need to mapping with driver wifi mode*/
	u32 *bb_h2c;
	u8 cmdlen = sizeof(union bb_h2c_ra_cmn_info);
	u64 mod_mask, mod_mask_h = 0;
	u8 rfe_idx = bb->phl_com->dev_cap.rfe_type;

	macid = (u8)(phl_sta_i->macid);
	hal_sta_i = phl_sta_i->hal_sta;

	ra_cmn = &bb->bb_cmn_hooker->bb_ra_i[macid].ra_cfg;
	ra_cfg = &ra_cmn->bb_h2c_ra_info;
	mode = ra_cfg->mode_ctrl;
	bb_h2c = ra_cmn->val;
	asoc_cap_i = &phl_sta_i->asoc_cap;
	rssi = hal_sta_i->rssi_stat.rssi >> 1;
	/*@Becareful RA use our "Tx" capability which means the capability of their "Rx"*/
	tx_nss = halbb_nss_mapping(bb, asoc_cap_i->nss_rx);
	ra_cfg->is_dis_ra = hal_sta_i->ra_info.dis_ra;
	mod_mask = hal_sta_i->ra_info.cur_ra_mask;
#ifdef BB_8922A_DVLP_SPF
	mode = ra_cmn->bb_h2c_ra_info_wifi7.mode_ctrl_eht;
	mod_mask_h = hal_sta_i->ra_info.cur_ra_mask_h;
#endif
	BB_DBG(bb, DBG_RA, "ra mask from phl = %llx, %llx\n", mod_mask, mod_mask_h);
	ra_cfg->upd_all= false;
	ra_cfg->upd_bw_nss_mask= false;
	ra_cfg->upd_mask= true;
	/* while ra mask is updated, gi_ltf can also be update */
	if (mode == 0) {
		BB_DBG(bb, DBG_RA, "mode = %d\n", mode);
		return ret_val;
	}
	ra_cfg->init_rate_lv = 0;
	/* ONLY need to get the optional gi-ltf combination for H2C FW*/
	/* bit(0)=4x0.8, bit(1)=1x0.8 -> different definition from the drver giltf*/
	giltf_cap = halbb_get_opt_giltf(bb, asoc_cap_i->ltf_gi);
	ra_cfg->giltf_cap = halbb_ax_giltf_chk(bb, mode, giltf_cap);
	if (bb->ic_type == BB_IC_BE_SERIES)
		ra_cmn->bb_h2c_ra_info_wifi7.bw_cap_eht = halbb_hw_bw_mapping(bb, phl_sta_i->chandef.bw, mode);
	else
		ra_cfg->bw_cap = halbb_hw_bw_mapping(bb, phl_sta_i->chandef.bw, mode);

	/* giltf assigned by phl/halbb or trained by FW*/
	if (hal_sta_i->ra_info.fix_giltf_en == false) { /*giltf from halbb*/
		#if 0
		if (bb->ic_type & (BB_RTL8852A | BB_RTL8852B | BB_RTL8852C)) { /*Disable FW train GI_LTF*/
			ra_cfg->fix_giltf_en = true;
			ra_cfg->fix_giltf = RTW_GILTF_2XHE08;
			/* gi_ltf is decided by delay spread. This is not ready.
			if (is_giltf_decided_by_delay_sp)
				ra_cfg->fix_giltf = halbb_ra_giltf_ctrl(bb, macid, delay_sp, asoc_cap_i->ltf_gi);
			*/
		} else { /*Enable FW train GI_LTF*/
			ra_cfg->fix_giltf_en = false;
		}
		#endif
		/*Disable FW train GI_LTF*/
		ra_cfg->fix_giltf_en = true;
		if ((ra_cfg->giltf_cap & BIT(0)) && (bb->ic_type & BB_RTL8852C)
			&& (ra_cfg->bw_cap == CHANNEL_WIDTH_160) && (rfe_idx < 50))
			ra_cfg->fix_giltf = RTW_GILTF_SGI_4XHE08;
		else
			ra_cfg->fix_giltf = RTW_GILTF_2XHE08;
	} else { /*giltf from phl*/
		ra_cfg->fix_giltf = halbb_giltf_trans(bb, asoc_cap_i->ltf_gi, hal_sta_i->ra_info.cal_giltf);
	}
	BB_DBG(bb, DBG_RA, "fix_giltf_en(phl)=%d, fix_giltf_en(Final)=%d, fix_giltf=%d\n",
	       hal_sta_i->ra_info.fix_giltf_en, ra_cfg->fix_giltf_en, ra_cfg->fix_giltf);

	mod_mask = halbb_ramask_mod(bb, macid, mod_mask, rssi, mode, tx_nss, &mod_mask_h);
	halbb_ramask_trans(bb, macid, mod_mask, mod_mask_h);
	BB_DBG(bb, DBG_RA, "RA Register=>Out h2cp: %x %x %x %x %x %x\n", bb_h2c[0], bb_h2c[1],
		   bb_h2c[2], bb_h2c[3], bb_h2c[4], bb_h2c[5]);
	ret_val = halbb_fill_h2c_cmd(bb, cmdlen, RA_H2C_MACIDCFG, HALBB_H2C_RA, bb_h2c);
	return ret_val;
}

u32 halbb_get_fw_ra_rpt(struct bb_info *bb, u16 len, u8 *c2h)
{
#ifdef HALBB_DBCC_SUPPORT
	struct bb_info *bb_1 = NULL;
#endif
	u16 macid_rpt;
	struct rtw_hal_stainfo_t *hal_sta_i;
	struct rtw_phl_stainfo_t *phl_sta_i;
	struct rtw_rate_info *rt_i;
	struct halbb_ra_rpt_info *ra_rpt_i;
	struct bb_rate_info rate;
	u16 rate_idx = 0;
	#if 0 /*wait for phl mu ra declaration*/
	u8 u1_muidx = 0;
	u8 i = 0;
	#endif

	if (!c2h)
		return 0;
	ra_rpt_i = (struct halbb_ra_rpt_info *)c2h;
	macid_rpt = ra_rpt_i->rpt_macid_l + (ra_rpt_i->rpt_macid_m << 8);
	if (macid_rpt >= PHL_MAX_STA_NUM) {
		BB_WARNING("[%s][1]Error macid = %d!!\n", __func__, macid_rpt);
		return 0;
	}

	if (bb->sta_exist[macid_rpt]) {
		phl_sta_i = bb->phl_sta_info[(u8)macid_rpt];
	} else {
#ifdef HALBB_DBCC_SUPPORT
		HALBB_GET_PHY_PTR(bb, bb_1, HW_PHY_1);
		if (bb_1->sta_exist[macid_rpt]) {
			phl_sta_i = bb_1->phl_sta_info[(u8)macid_rpt];
		} else
#endif
		{
			BB_WARNING("[%s][2]Error macid = %d!!\n", __func__, macid_rpt);
			return 0;
		}
	}

	if (!phl_sta_i) {
		BB_WARNING("[%s][3] phl_sta==NULL, Wrong C2H RA macid !!\n", __func__);
		return 0;
	}
	hal_sta_i = phl_sta_i->hal_sta;
	if (!hal_sta_i) {
		BB_WARNING("[%s][4] hal_sta==NULL, Wrong C2H RA macid !!\n", __func__);
		return 0;
	}

	rt_i = &hal_sta_i->ra_info.rpt_rt_i;
	if (bb->ic_type & BB_IC_AX_SERIES)
		rate_idx = (u16)(ra_rpt_i->rpt_mcs_nss & 0x7f) | ((u16)(ra_rpt_i->rpt_md_sel & 0x3) << 7);
	else
		rate_idx = (u16)(ra_rpt_i->rpt_mcs_nss & 0xff) | ((u16)(ra_rpt_i->rpt_md_sel & 0xf) << 8);

	halbb_rate_idx_parsor(bb, rate_idx, ra_rpt_i->rpt_gi_ltf, &rate);

	if (ra_rpt_i->is_mu) {
		#if 0 /*wait for phl mu ra declaration*/
		hal_sta_i->ra_info.mu_idx = ra_rpt_i->u0_muidx;

		u1_muidx = (ra_rpt_i->u1_muidx >= ra_rpt_i->u0_muidx) ?
			   ra_rpt_i->u1_muidx + 1 : ra_rpt_i->u1_muidx;

		if (hal_sta_i->ra_info.mu_active[ra_rpt_i->u1_muidx] == false) {
			for (i = 0; i < PHL_MAX_STA_NUM; i++) {
				if (i == (u8)macid_rpt)
					continue;
				if (!bb->sta_exist[i])
					continue;
				if (!is_sta_active(bb->phl_sta_info[i]))
					continue;

				if (bb->phl_sta_info[i]->hal_sta->ra_info.mu_idx == u1_muidx)
					hal_sta_i->ra_info.mu_active[ra_rpt_i->u1_muidx] = true;
			}
		}

		if (hal_sta_i->ra_info.mu_active[ra_rpt_i->u1_muidx]) {
			hal_sta_i->ra_info.mu_curr_retry_ratio[ra_rpt_i->u1_muidx] = ra_rpt_i->retry_ratio;
			rt_i->mu_mcs_ss_idx[ra_rpt_i->u1_muidx] = ra_rpt_i->rpt_mcs_nss;
			rt_i->mu_gi_ltf[ra_rpt_i->u1_muidx] = ra_rpt_i->rpt_gi_ltf;
			rt_i->mu_mode[ra_rpt_i->u1_muidx] = ra_rpt_i->rpt_md_sel;
			rt_i->mu_mcs_idx[ra_rpt_i->u1_muidx] = rate.idx;
			rt_i->mu_ss[ra_rpt_i->u1_muidx] = rate.ss - 1;
			rt_i->mu_bw[ra_rpt_i->u1_muidx] = ra_rpt_i->rpt_bw;

			BB_DBG(bb, DBG_RA,
			       "MU RA RPT: su_macid = %d, muidx{u0,u1} = {%d,%d}, mu_mode = %d, mu_giltf = %x, mu_mcs_nss = %x, mu_mcs_idx = %x, mu_ss = %x\n",
			       macid_rpt, ra_rpt_i->u0_muidx, u1_muidx,
			       rt_i->mu_mode[ra_rpt_i->u1_muidx],
			       rt_i->mu_gi_ltf[ra_rpt_i->u1_muidx],
			       rt_i->mu_mcs_ss_idx[ra_rpt_i->u1_muidx],
			       rt_i->mu_mcs_idx[ra_rpt_i->u1_muidx],
			       rt_i->mu_ss[ra_rpt_i->u1_muidx]);
		}
		#endif
	} else {
		hal_sta_i->ra_info.curr_retry_ratio = ra_rpt_i->retry_ratio;
		rt_i->mcs_ss_idx = ra_rpt_i->rpt_mcs_nss;
		rt_i->gi_ltf = ra_rpt_i->rpt_gi_ltf;
		rt_i->mode = ra_rpt_i->rpt_md_sel;
		rt_i->mcs_idx = rate.idx;
		rt_i->ss = rate.ss - 1;
		rt_i->bw = ra_rpt_i->rpt_bw;
		if (ra_rpt_i->rpt_md_sel == BB_HE_MODE)
			rt_i->is_actrl = ((ra_rpt_i->rpt_mcs_nss & 0xf) <= 2) ? false : true;
		else
			rt_i->is_actrl = false;

		BB_DBG(bb, DBG_RA,
		       "SU RA RPT: macid = %d, mode = %d, giltf = %x, mcs_nss = %x, mcs_idx = %x, ss = %x\n",
		       macid_rpt, rt_i->mode, rt_i->gi_ltf, rt_i->mcs_ss_idx, rt_i->mcs_idx, rt_i->ss);
	}

	return 0;
}

bool rtw_halbb_query_txsts(struct bb_info *bb, u16 macid0, u16 macid1)
{
	u8 content[4];
	u32 *bb_h2c = (u32 *)content;
	bool ret_val;
	u16 cmdlen = 4;

	BB_DBG(bb, DBG_RA, "====> QuerryTxSts : macid = %d %d\n", macid0, macid1);
	content[0] = (u8)(macid0& 0xff);
	content[1] = (u8)((macid0>>8)& 0xff);
	content[2] = (u8)(macid1& 0xff);
	content[3] = (u8)((macid1>>8)& 0xff);
	ret_val = halbb_fill_h2c_cmd(bb, cmdlen, RA_H2C_GET_TXSTS, HALBB_H2C_RA, bb_h2c);
	if (!ret_val)
		BB_WARNING("[%s] Error H2C cmd in querry txsts !!\n", __func__);
	return ret_val;
}

void halbb_drv_cmac_rpt_parsing(struct bb_info *bb, u8 *rpt)
{
	struct bb_fw_cmac_rpt_info *c_rpt;
	u16 rpt_len, i;
	u8 *rpt_tmp = rpt;

	if (!rpt)
		return;
	c_rpt = (struct bb_fw_cmac_rpt_info *)rpt;
	rpt_len = sizeof(struct bb_fw_cmac_rpt_info);
	if (rpt_len % 4) {
		return;
	}
	BB_DBG(bb, DBG_RA,
	       "[%d] try_rate=%d, fix_rate=0x%x, final{rate, giltf}={0x%x,0x%x}, data_bw = 0x%x\n",
	       c_rpt->macid, c_rpt->try_rate, c_rpt->fix_rate,
	       c_rpt->final_rate, c_rpt->final_gi_ltf, c_rpt->data_bw);
	for (i = 0; i < rpt_len; i++) {
		BB_DBG(bb, DBG_RA, "[%d] 0x%x\n", i, ((u32*)rpt)[0]);
		rpt_tmp += 4;
	}
}

u32 halbb_get_txsts_rpt(struct bb_info *bb, u16 len, u8 *c2h)
{
#ifdef HALBB_DBCC_SUPPORT
	struct bb_info *bb_1 = NULL;
#endif
	u16 macid_rpt;
	u16 macid_rpt_1;
	u8 i = 0;
	struct rtw_hal_stainfo_t *hal_sta_i;
	struct rtw_phl_stainfo_t *phl_sta_i;
	struct rtw_ra_sta_info *ra_sta_i;
	struct halbb_txsts_info *txsts_i;
	u16 tx_total;
	u16 tx_ok[4];
	u16 tx_retry[4];
	u8 null_cnt = 0;
	u8 j = 0;

	if (!c2h)
		return 0;

	for (j = 0; j <= 1; j++) {
		c2h = (j) ? c2h : (c2h + 32); /*txsts_info is 32 bytes*/

		txsts_i = (struct halbb_txsts_info *)c2h;
		macid_rpt = txsts_i->rpt_macid_l + (txsts_i->rpt_macid_m << 8);

		if (j == 0)
			macid_rpt_1 = macid_rpt;

		if (macid_rpt >= PHL_MAX_STA_NUM) {
			BB_DBG(bb, DBG_RA,
			       "[%s][1] macid = %d, Error macid !!\n",
			       __func__, macid_rpt);
			null_cnt++;
			continue;
		}

		if (bb->sta_exist[macid_rpt]) {
			phl_sta_i = bb->phl_sta_info[(u8)macid_rpt];
		} else {
#ifdef HALBB_DBCC_SUPPORT
			HALBB_GET_PHY_PTR(bb, bb_1, HW_PHY_1);
			if (bb_1->sta_exist[macid_rpt]) {
				phl_sta_i = bb_1->phl_sta_info[(u8)macid_rpt];
			} else
#endif
			{
				BB_DBG(bb, DBG_RA,
				       "[%s][2]Error macid = %d!!\n",
				       __func__, macid_rpt);
				null_cnt++;
				continue;
			}
		}

		if (phl_sta_i == NULL) {
			BB_DBG(bb, DBG_RA,
			       "[%s][3] phl_sta == NULL, Error macid = %d!!\n",
			       __func__, macid_rpt);
			null_cnt++;
			continue;
		}
		hal_sta_i = phl_sta_i->hal_sta;
		if (hal_sta_i == NULL) {
			BB_DBG(bb, DBG_RA,
			       "[%s][3] hal_sta == NULL, Error macid = %d!!\n",
			       __func__, macid_rpt);
			null_cnt++;
			continue;
		}
		ra_sta_i = &hal_sta_i->ra_info;
		tx_ok[0] = txsts_i->tx_ok_be_l + (txsts_i->tx_ok_be_m << 8);
		tx_ok[1] = txsts_i->tx_ok_bk_l + (txsts_i->tx_ok_bk_m << 8);
		tx_ok[2] = txsts_i->tx_ok_vi_l + (txsts_i->tx_ok_vi_m << 8);
		tx_ok[3] = txsts_i->tx_ok_vo_l + (txsts_i->tx_ok_vo_m << 8);
		tx_retry[0] = txsts_i->tx_retry_be_l + (txsts_i->tx_retry_be_m << 8);
		tx_retry[1] = txsts_i->tx_retry_bk_l + (txsts_i->tx_retry_bk_m << 8);
		tx_retry[2] = txsts_i->tx_retry_vi_l + (txsts_i->tx_retry_vi_m << 8);
		tx_retry[3] = txsts_i->tx_retry_vo_l + (txsts_i->tx_retry_vo_m << 8);
		for (i = 0; i <= 3; i++) {
			if ((0xffffffff - ra_sta_i->tx_ok_cnt[i]) > (tx_ok[i]))
				ra_sta_i->tx_ok_cnt[i] += tx_ok[i];
			if ((0xffffffff - ra_sta_i->tx_retry_cnt[i]) > (tx_retry[i]))
				ra_sta_i->tx_retry_cnt[i] += tx_retry[i];
			BB_DBG(bb, DBG_RA, "TxOk[%d] = %d, TxRty[%d] = %d\n", i, tx_ok[i], i, tx_retry[i]);
		}
		tx_total = txsts_i->tx_total_l + (txsts_i->tx_total_m << 8);
		if ((0xffffffff - ra_sta_i->tx_total_cnt) > tx_total)
			ra_sta_i->tx_total_cnt += tx_total;
		BB_DBG(bb, DBG_RA,
		       "====> GetTxSts : macid = %d, TxTotal = %d\n",
		       macid_rpt, tx_total);
	}

	if (null_cnt > 1)
		BB_WARNING("====> GetTxStS : both macid sta is null, macid = {%d,%d}\n",
			   macid_rpt_1, macid_rpt);

	return 0;
}

void halbb_ra_rssi_setting_watchdog(struct bb_info *bb)
{
	u8 macid = 0;
	u8 i = 0, sta_cnt = 0;
	u16 cmdlen;
	bool ret_val = true;
	struct rtw_ra_sta_info *bb_ra;
	struct rtw_hal_stainfo_t *hal_sta_i;
	union bb_h2c_ra_rssi_info *ra_rssi;
	struct bb_h2c_rssi_setting *rssi_i;
	struct bb_link_info *bb_link = &bb->bb_link_i;
	u16 rssi_len = 0;
	u32 *bb_h2c;
	u8 rssi_a = 0, rssi_b = 0, bcn_rssi_a = 0, bcn_rssi_b = 0;

	rssi_len = sizeof(struct bb_h2c_rssi_setting) * PHL_MAX_STA_NUM;
	ra_rssi = hal_mem_alloc(bb->hal_com, rssi_len);
	rssi_i = &ra_rssi->bb_h2c_ra_rssi;
	if (!rssi_i) {
		BB_WARNING("[%s] Error RSSI allocat failed!!\n", __func__);
		return;
	}
	bb_h2c = ra_rssi->val;
	cmdlen = rssi_len;
	for (i = 0; i < PHL_MAX_STA_NUM; i++) {
		if (!bb->sta_exist[i] || !bb->phl_sta_info[i])
			continue;

		hal_sta_i = bb->phl_sta_info[i]->hal_sta;
		if (!hal_sta_i)
			continue;

		bb_ra = &hal_sta_i->ra_info;
		if (!(bb_ra->ra_registered))
			continue;
		
		macid = (u8)(bb->phl_sta_info[i]->macid);
		BB_DBG(bb, DBG_RA, "Add BB rssi info[%d], macid=%d\n", i, macid);
		/* Need modify for Nss > 2 */
		rssi_a = (hal_sta_i->rssi_stat.rssi_ma_path[0] >> 5) & 0x7f;
		bcn_rssi_a = (hal_sta_i->rssi_stat.rssi_bcn_ma_path[0] >> 5) & 0x7f;
		//if (!bb->hal_com->dbcc_en) 
		{
			rssi_b = (hal_sta_i->rssi_stat.rssi_ma_path[1] >> 5) & 0x7f;
			bcn_rssi_b = (hal_sta_i->rssi_stat.rssi_bcn_ma_path[1] >> 5) & 0x7f;
		}
		rssi_i[sta_cnt].macid = macid;
		rssi_i[sta_cnt].rssi_a = rssi_a | BIT(7);
		rssi_i[sta_cnt].rssi_b = rssi_b;
		rssi_i[sta_cnt].bcn_rssi_a = bcn_rssi_a | BIT(7);
		rssi_i[sta_cnt].bcn_rssi_b = bcn_rssi_b;
		rssi_i[sta_cnt].drv_ractrl = 0; /* RSVD */
		rssi_i[sta_cnt].is_fixed_rate = bb_ra->fixed_rt_en;
		rssi_i[sta_cnt].fixed_rate = bb_ra->fixed_rt_i.mcs_ss_idx;
		rssi_i[sta_cnt].fixed_rate_md = bb_ra->fixed_rt_i.mode;
		rssi_i[sta_cnt].fixed_giltf = bb_ra->fixed_rt_i.gi_ltf;
		rssi_i[sta_cnt].fixed_bw = bb_ra->fixed_rt_i.bw;
		if (bb->ic_type & BB_IC_BE_SERIES) {
			ra_rssi[sta_cnt].bb_h2c_ra_rssi_wifi7.fixed_rate_M = ((u8)bb_ra->fixed_rt_i.mcs_ss_idx >> 7) & 0x1;
			ra_rssi[sta_cnt].bb_h2c_ra_rssi_wifi7.fixed_bw_M = ((u8)bb_ra->fixed_rt_i.bw >> 2) & 0x1;
			ra_rssi[sta_cnt].bb_h2c_ra_rssi_wifi7.fixed_rate_md_M = ((u8)bb_ra->fixed_rt_i.mode >>2) & 0x1;
		}
		sta_cnt++;

		BB_DBG(bb, DBG_RA, "bcn_rssi{a,b}={%d,%d}, data_rssi{a,b}={%d,%d}\n",
		       bcn_rssi_a, bcn_rssi_b, rssi_a, rssi_b);

		if (sta_cnt == bb_link->num_linked_client)
			break;
	}

	if (sta_cnt == STA_NUM_RSSI_CMD) {
		/* Fill endcmd = 1 for last sta */
		rssi_i[sta_cnt - 1].endcmd = 1;
		ret_val = halbb_fill_h2c_cmd(bb, cmdlen, RA_H2C_RSSISETTING,
				   HALBB_H2C_RA, bb_h2c);
		BB_DBG(bb, DBG_RA, "sta_cnt=%d, RSSI cmd end 1\n", sta_cnt);
	} else if ((sta_cnt > 0) && (sta_cnt < STA_NUM_RSSI_CMD)) {
		/* Fill endcmd = 1 for last sta */
		rssi_i[sta_cnt - 1].endcmd = 1;
		rssi_i[sta_cnt].macid = 0xff;
		ret_val = halbb_fill_h2c_cmd(bb, cmdlen, RA_H2C_RSSISETTING,
				   HALBB_H2C_RA, bb_h2c);
		BB_DBG(bb, DBG_RA, "sta_cnt=%d, RSSI cmd end 2\n", sta_cnt);
	}

	BB_DBG(bb, DBG_RA, "RSSI=>h2c[0~3]: %x %x %x %x\n", bb_h2c[0], bb_h2c[1],
	       bb_h2c[2], bb_h2c[3]);

	if (ret_val == false)
		BB_WARNING("[%s] H2C cmd error!!\n", __func__);

	if (rssi_i)
		hal_mem_free(bb->hal_com, rssi_i, rssi_len);
}

void halbb_ra_giltf_ctrl(struct bb_info *bb, u8 macid, u8 delay_sp, u8 assoc_giltf)
{
	//struct bb_ra_info *bb_ra = &bb->bb_cmn_hooker->bb_ra_i[macid];
	//struct bb_h2c_ra_cfg_info *ra_cfg = &bb->bb_cmn_hooker->bb_ra_i[macid].ra_cfg;
	//enum rtw_gi_ltf giltf = RTW_GILTF_LGI_4XHE32;

	//if (!bb)
	//	return;

	/*This dhould be decision before ra mask h2c*/
	/* GI LTF decision algorithm is needed*/
	/*if (delay_sp)
		giltf = RTW_GILTF_LGI_4XHE32;
	else
		giltf = RTW_GILTF_LGI_4XHE32;
	ra_cfg->giltf = giltf;*/
}

bool halbb_ra_bfer_chk(struct bb_info *bb, struct rtw_phl_stainfo_t *phl_sta_i)
{
	struct protocol_cap_t *asoc_cap = &phl_sta_i->asoc_cap;
	struct protocol_cap_t *self_cap = &phl_sta_i->rlink->protocol_cap;
	bool bfer_chk = false;

	BB_DBG(bb, DBG_RA, "====>%s\n", __func__);

	if ((phl_sta_i->wmode & WLAN_MD_11AX) &&
	    ((asoc_cap->he_su_bfme && self_cap->he_su_bfmr) ||
	     (asoc_cap->he_mu_bfme && self_cap->he_mu_bfmr))) {
		/*AX bfer chk*/
		bfer_chk = true;
	} else if ((phl_sta_i->wmode & WLAN_MD_11AC) &&
		   ((asoc_cap->vht_su_bfme && self_cap->vht_su_bfmr) ||
		    (asoc_cap->vht_mu_bfme && self_cap->vht_mu_bfmr))) {
		/*AC bfer chk*/
		bfer_chk = true;
	} else if ((phl_sta_i->wmode & WLAN_MD_11N) &&
		   (asoc_cap->ht_su_bfme && self_cap->ht_su_bfmr)) {
		/*N bfer chk*/
		bfer_chk = true;
	}

	BB_DBG(bb, DBG_RA,
		"assoc bfee : {he_su, he_mu, vht_su, vht_mu, ht_su}={%d,%d,%d,%d,%d}\n",
		asoc_cap->he_su_bfme, asoc_cap->he_mu_bfme,
		asoc_cap->vht_su_bfme, asoc_cap->vht_mu_bfme,
		asoc_cap->ht_su_bfme);

	BB_DBG(bb, DBG_RA,
		"self bfer : {he_su, he_mu, vht_su, vht_mu, ht_su}={%d,%d,%d,%d,%d}\n",
		self_cap->he_su_bfmr, self_cap->he_mu_bfmr,
		self_cap->vht_su_bfmr, self_cap->vht_mu_bfmr,
		self_cap->ht_su_bfmr);

	return bfer_chk;
}

bool halbb_ra_bfee_chk(struct bb_info *bb, struct rtw_phl_stainfo_t *phl_sta_i)
{
	struct protocol_cap_t *asoc_cap = &phl_sta_i->asoc_cap;
	struct protocol_cap_t *self_cap = &phl_sta_i->rlink->protocol_cap;
	bool bfee_chk = false;

	BB_DBG(bb, DBG_RA, "====>%s\n", __func__);


	if ((phl_sta_i->wmode & WLAN_MD_11AX) &&
	    ((asoc_cap->he_su_bfmr && self_cap->he_su_bfme) ||
	     (asoc_cap->he_mu_bfmr && self_cap->he_mu_bfme))) {
		/*AX bfee chk*/
		bfee_chk = true;
	} else if ((phl_sta_i->wmode & WLAN_MD_11AC) &&
		   ((asoc_cap->vht_su_bfmr && self_cap->vht_su_bfme) ||
		    (asoc_cap->vht_mu_bfmr && self_cap->vht_mu_bfme))) {
		/*AC bfee chk*/
		bfee_chk = true;
	} else if ((phl_sta_i->wmode & WLAN_MD_11N) &&
		   (asoc_cap->ht_su_bfmr && self_cap->ht_su_bfme)) {
		/*N bfee chk*/
		bfee_chk = true;
	}

	BB_DBG(bb, DBG_RA,
		"assoc bfer : {he_su, he_mu, vht_su, vht_mu, ht_su}={%d,%d,%d,%d,%d}\n",
		asoc_cap->he_su_bfmr, asoc_cap->he_mu_bfmr,
		asoc_cap->vht_su_bfmr, asoc_cap->vht_mu_bfmr,
		asoc_cap->ht_su_bfmr);

	BB_DBG(bb, DBG_RA,
		"self bfee : {he_su, he_mu, vht_su, vht_mu, ht_su}={%d,%d,%d,%d,%d}\n",
		self_cap->he_su_bfme, self_cap->he_mu_bfme,
		self_cap->vht_su_bfme, self_cap->vht_mu_bfme,
		self_cap->ht_su_bfme);

	return bfee_chk;
}

void halbb_ra_watchdog(struct bb_info *bb)
{
	u8 i = 0, sta_cnt = 0, macid = 0;
	struct bb_link_info *bb_link = &bb->bb_link_i;

	if (bb == NULL)
		return;
#if 0
	if (!(bb->support_ability & BB_RA))
		return;
#endif
	halbb_ra_rssi_setting_watchdog(bb);

	bb_link->at_least_one_bfee = false;
	bb_link->at_least_one_bfer = false;

	for (i = 0; i < PHL_MAX_STA_NUM; i++) {
		if (!(bb->sta_exist[i]))
			continue;

		if (!(bb->phl_sta_info[i]) || !bb) {
			BB_WARNING("[%s][1] Pointer is NULL!!!\n", __func__);
			continue;
		}

		if (!(bb->phl_sta_info[i]->hal_sta)) {
			BB_WARNING("[%s][2] Pointer is NULL!!!\n", __func__);
			continue;
		}

		if (!(bb->phl_sta_info[i]->hal_sta->ra_info.ra_registered))
			continue;

		macid = (u8)(bb->phl_sta_info[i]->macid);
		BB_DBG(bb, DBG_RA, "====>ra update mask[%d], macid=%d\n", i, macid);
		if (!halbb_ra_update_mask_watchdog(bb, bb->phl_sta_info[i])) {
			BB_DBG(bb, DBG_RA, "Update Fail\n");
		}

		BB_DBG(bb, DBG_RA,
		       "i=%d,macid=%d,Mode=%s%s%s%s%s\n", i, macid,
		       (bb->phl_sta_info[i]->wmode & WLAN_MD_11B) ? "B" : " ",
		       (bb->phl_sta_info[i]->wmode & (WLAN_MD_11G | WLAN_MD_11A)) ? "G" : " ",
		       (bb->phl_sta_info[i]->wmode & WLAN_MD_11N) ? "N" : " ",
		       (bb->phl_sta_info[i]->wmode & WLAN_MD_11AC) ? "AC" : " ",
		       (bb->phl_sta_info[i]->wmode & WLAN_MD_11AX) ? "AX" : " ");

		if (halbb_ra_bfee_chk(bb, bb->phl_sta_info[i]))
			bb_link->at_least_one_bfee = true;

		if (halbb_ra_bfer_chk(bb, bb->phl_sta_info[i]))
			bb_link->at_least_one_bfer = true;

		sta_cnt++;
		if (sta_cnt == bb_link->num_linked_client)
			break;
	}

	BB_DBG(bb, DBG_RA,
	       "at_least_one_bfee=%d, at_least_one_bfer=%d\n",
	       bb_link->at_least_one_bfee, bb_link->at_least_one_bfer);
}

void halbb_ra_init(struct bb_info *bb)
{
	struct bb_ra_info *bb_ra = NULL;
	u8 macid = 0;

	for (macid = 0; macid < PHL_MAX_STA_NUM; macid ++) {
		bb_ra = &bb->bb_cmn_hooker->bb_ra_i[macid];
		if (!bb_ra)
			halbb_mem_set(bb, bb_ra, 0, sizeof (struct bb_ra_info));
	}
}

u8 rtw_halbb_rts_rate(struct bb_info *bb, u16 tx_rate, bool is_erp_prot)
{

	u8 rts_ini_rate = RTW_DATA_RATE_OFDM6;

	if (is_erp_prot) { /* use CCK rate as RTS */
		rts_ini_rate = RTW_DATA_RATE_CCK1;
	} else {
		switch (tx_rate) {
		case RTW_DATA_RATE_VHT_NSS4_MCS9:
		case RTW_DATA_RATE_VHT_NSS4_MCS8:
		case RTW_DATA_RATE_VHT_NSS4_MCS7:
		case RTW_DATA_RATE_VHT_NSS4_MCS6:
		case RTW_DATA_RATE_VHT_NSS4_MCS5:
		case RTW_DATA_RATE_VHT_NSS4_MCS4:
		case RTW_DATA_RATE_VHT_NSS4_MCS3:
		case RTW_DATA_RATE_VHT_NSS3_MCS9:
		case RTW_DATA_RATE_VHT_NSS3_MCS8:
		case RTW_DATA_RATE_VHT_NSS3_MCS7:
		case RTW_DATA_RATE_VHT_NSS3_MCS6:
		case RTW_DATA_RATE_VHT_NSS3_MCS5:
		case RTW_DATA_RATE_VHT_NSS3_MCS4:
		case RTW_DATA_RATE_VHT_NSS3_MCS3:
		case RTW_DATA_RATE_VHT_NSS2_MCS9:
		case RTW_DATA_RATE_VHT_NSS2_MCS8:
		case RTW_DATA_RATE_VHT_NSS2_MCS7:
		case RTW_DATA_RATE_VHT_NSS2_MCS6:
		case RTW_DATA_RATE_VHT_NSS2_MCS5:
		case RTW_DATA_RATE_VHT_NSS2_MCS4:
		case RTW_DATA_RATE_VHT_NSS2_MCS3:
		case RTW_DATA_RATE_VHT_NSS1_MCS9:
		case RTW_DATA_RATE_VHT_NSS1_MCS8:
		case RTW_DATA_RATE_VHT_NSS1_MCS7:
		case RTW_DATA_RATE_VHT_NSS1_MCS6:
		case RTW_DATA_RATE_VHT_NSS1_MCS5:
		case RTW_DATA_RATE_VHT_NSS1_MCS4:
		case RTW_DATA_RATE_VHT_NSS1_MCS3:
		case RTW_DATA_RATE_MCS31:
		case RTW_DATA_RATE_MCS30:
		case RTW_DATA_RATE_MCS29:
		case RTW_DATA_RATE_MCS28:
		case RTW_DATA_RATE_MCS27:
		case RTW_DATA_RATE_MCS23:
		case RTW_DATA_RATE_MCS22:
		case RTW_DATA_RATE_MCS21:
		case RTW_DATA_RATE_MCS20:
		case RTW_DATA_RATE_MCS19:
		case RTW_DATA_RATE_MCS15:
		case RTW_DATA_RATE_MCS14:
		case RTW_DATA_RATE_MCS13:
		case RTW_DATA_RATE_MCS12:
		case RTW_DATA_RATE_MCS11:
		case RTW_DATA_RATE_MCS7:
		case RTW_DATA_RATE_MCS6:
		case RTW_DATA_RATE_MCS5:
		case RTW_DATA_RATE_MCS4:
		case RTW_DATA_RATE_MCS3:
		case RTW_DATA_RATE_OFDM54:
		case RTW_DATA_RATE_OFDM48:
		case RTW_DATA_RATE_OFDM36:
		case RTW_DATA_RATE_OFDM24:
			rts_ini_rate = RTW_DATA_RATE_OFDM24;
			break;
		case RTW_DATA_RATE_VHT_NSS4_MCS2:
		case RTW_DATA_RATE_VHT_NSS4_MCS1:
		case RTW_DATA_RATE_VHT_NSS3_MCS2:
		case RTW_DATA_RATE_VHT_NSS3_MCS1:
		case RTW_DATA_RATE_VHT_NSS2_MCS2:
		case RTW_DATA_RATE_VHT_NSS2_MCS1:
		case RTW_DATA_RATE_VHT_NSS1_MCS2:
		case RTW_DATA_RATE_VHT_NSS1_MCS1:
		case RTW_DATA_RATE_MCS26:
		case RTW_DATA_RATE_MCS25:
		case RTW_DATA_RATE_MCS18:
		case RTW_DATA_RATE_MCS17:
		case RTW_DATA_RATE_MCS10:
		case RTW_DATA_RATE_MCS9:
		case RTW_DATA_RATE_MCS2:
		case RTW_DATA_RATE_MCS1:
		case RTW_DATA_RATE_OFDM18:
		case RTW_DATA_RATE_OFDM12:
			rts_ini_rate = RTW_DATA_RATE_OFDM12;
			break;
		case RTW_DATA_RATE_VHT_NSS4_MCS0:
		case RTW_DATA_RATE_VHT_NSS3_MCS0:
		case RTW_DATA_RATE_VHT_NSS2_MCS0:
		case RTW_DATA_RATE_VHT_NSS1_MCS0:
		case RTW_DATA_RATE_MCS24:
		case RTW_DATA_RATE_MCS16:
		case RTW_DATA_RATE_MCS8:
		case RTW_DATA_RATE_MCS0:
		case RTW_DATA_RATE_OFDM9:
		case RTW_DATA_RATE_OFDM6:
			rts_ini_rate = RTW_DATA_RATE_OFDM6;
			break;
		case RTW_DATA_RATE_CCK11:
		case RTW_DATA_RATE_CCK5_5:
		case RTW_DATA_RATE_CCK2:
		case RTW_DATA_RATE_CCK1:
			rts_ini_rate = RTW_DATA_RATE_CCK1;
			break;
		default:
			rts_ini_rate = RTW_DATA_RATE_OFDM6;
			break;
		}
	}

	if (bb->hal_com->band[0].cur_chandef.band == BAND_ON_5G) {
		if (rts_ini_rate < RTW_DATA_RATE_OFDM6)
			rts_ini_rate = RTW_DATA_RATE_OFDM6;
	}
	return rts_ini_rate;
}

void halbb_ra_shift_darf_tc(struct bb_info *bb, bool enable, u8 *init_fb_cnt)
{
	struct bb_h2c_ra_shift_dafc_tc darf_tc_h2c;
	u32 *bb_h2c = (u32 *) &darf_tc_h2c;
	u8 cmdlen = sizeof(struct bb_h2c_ra_shift_dafc_tc);
	bool ret_val = false;
	u8 i = 0;

	darf_tc_h2c.enable = (u8)enable;

	for (i = 0; i < 24; i++) { /*1ss MCS0 ~ 2ss MCS11*/
		if (init_fb_cnt[i] == 0) /*the range of init_fb_cnt is 1~8*/
			darf_tc_h2c.init_fb_cnt[i] = 4; /*default setting => 0x1123_1000*/
		else
			darf_tc_h2c.init_fb_cnt[i] = init_fb_cnt[i];
	}

	BB_DBG(bb, DBG_RA,
	       "RA shift darf_tc H2C: %x %x %x %x %x %x %x\n",
	       bb_h2c[0], bb_h2c[1], bb_h2c[2], bb_h2c[3], bb_h2c[4],
	       bb_h2c[5], bb_h2c[6]);

	ret_val = halbb_fill_h2c_cmd(bb, cmdlen, RA_H2C_RA_SHIFT_DARF_TC, HALBB_H2C_RA, bb_h2c);
}

void halbb_get_ra_dbgreg(struct bb_info *bb)
{
	struct rtw_phl_com_t *phl = bb->phl_com;
	struct dev_cap_t *dev = &phl->dev_cap;
	struct bb_dbg_info *dbg = &bb->bb_dbg_i;
	struct bb_ra_dbgreg *dbgreg = &dbg->ra_dbgreg_i;

	dbgreg->cmac_tbl_id0 = halbb_get_reg(bb, 0x160, MASKDWORD);
	dbgreg->cmac_tbl_id1 = halbb_get_reg(bb, 0x164, MASKDWORD);
	dbgreg->per = halbb_get_reg(bb, 0x168, MASKBYTE3);
	dbgreg->rdr = halbb_get_reg(bb, 0x174, MASKBYTE0);
	dbgreg->r4 = halbb_get_reg(bb, 0x174, MASKBYTE1);
	if ((bb->ic_type != BB_RTL8852A) && (bb->ic_type != BB_RTL8852B) &&
	    (bb->ic_type != BB_RTL8851B)) {
		dbgreg->cls = halbb_get_reg(bb, 0x2444, MASKBYTE0);
	}
	dbgreg->rate_up_lmt_cnt = halbb_get_reg(bb, 0x168, MASKBYTE0);
	dbgreg->per_ma = halbb_get_reg(bb, 0x16c, MASKLWORD);
	dbgreg->var = halbb_get_reg(bb, 0x16c, MASKHWORD);
	dbgreg->d_o_n = halbb_get_reg(bb, 0x170, MASKLWORD);
	dbgreg->d_o_p = halbb_get_reg(bb, 0x170, MASKHWORD);
	dbgreg->rd_th = halbb_get_reg(bb, 0x168, MASKBYTE1);
	dbgreg->ru_th = halbb_get_reg(bb, 0x168, MASKBYTE2);
	dbgreg->try_per = halbb_get_reg(bb, 0x178, MASKBYTE2);
	dbgreg->try_rdr = halbb_get_reg(bb, 0x184, MASKBYTE3);
	dbgreg->try_r4 = halbb_get_reg(bb, 0x188, MASKBYTE0);
	dbgreg->txrpt_tot = halbb_get_reg(bb, 0x178, MASKBYTE3);
	dbgreg->ra_timer = halbb_get_reg(bb, 0x178, MASKBYTE1);
	dbgreg->tot_disra_trying_return = halbb_get_reg(bb, 0x174, MASKBYTE2);
	dbgreg->r4_return = halbb_get_reg(bb, 0x174, MASKBYTE3);
	dbgreg->ra_mask_h = halbb_get_reg(bb, 0x180, MASKDWORD);
	dbgreg->ra_mask_l = halbb_get_reg(bb, 0x17c, MASKDWORD);
	dbgreg->highest_rate = halbb_get_reg(bb, 0x184, MASKBYTE0);
	dbgreg->lowest_rate = halbb_get_reg(bb, 0x184, MASKBYTE1);
	dbgreg->upd_all_h2c_0 = halbb_get_reg(bb, 0x1d0, MASKDWORD);
	dbgreg->upd_all_h2c_1 = halbb_get_reg(bb, 0x1d4, MASKDWORD);
	dbgreg->upd_all_h2c_2 = halbb_get_reg(bb, 0x1d8, MASKDWORD);
	dbgreg->upd_all_h2c_3 = halbb_get_reg(bb, 0x1dc, MASKDWORD);
	if ((bb->ic_type == BB_RTL8852A) || (bb->ic_type == BB_RTL8852B)) {
		dbgreg->dyn_stbc = halbb_get_reg(bb, 0x1ec, MASKBYTE3);
	}
	/*MU MIMO RA*/
	if (dev->rfe_type >= 50) {
		dbgreg->mu_mcs = halbb_get_reg(bb, 0x1a0, MASKDWORD);
		dbgreg->mu_id_lowest_rate = halbb_get_reg(bb, 0x1a4, MASKDWORD);
		dbgreg->mu_rd_ru_th = halbb_get_reg(bb, 0x1a8, MASKDWORD);
		dbgreg->mu_per = halbb_get_reg(bb, 0x1ac, MASKDWORD);
	}
}

void halbb_ra_dbg(struct bb_info *bb, char input[][16], u32 *_used,
			 char *output, u32 *_out_len)
{
	struct rtw_phl_stainfo_t *phl_sta = NULL;
	char help[] = "-h";
	u32 val[10] = {0};
	u32 used = *_used;
	u32 out_len = *_out_len;
	u8 i;
	bool ret_val = false;
	struct rtw_ra_sta_info *bb_ra;
	struct rtw_hal_stainfo_t *hal_sta_i;
	//struct bb_h2c_rssi_setting *rssi_i;
	u8 rssi_a = 0, rssi_b = 0, bcn_rssi_a = 0, bcn_rssi_b = 0, cmdlen = 0;
	u8 h2c_ra_cls[4];
	u16 rssi_len = 0;
	u32 *bb_h2c = NULL;
	struct bb_h2c_ra_adjust ra_th_i;
	struct bb_h2c_ra_mask ra_mask_i;
	struct bb_h2c_ra_d_o_timer ra_d_o_timer_i;
	union bb_h2c_ra_rssi_info ra_rssi = {0};
	struct bb_h2c_rssi_setting *rssi_i = &ra_rssi.bb_h2c_ra_rssi;

	if (_os_strcmp(input[1], help) == 0) {
		//BB_DBG_CNSL(out_len, used, output + used, out_len - used,
		//	 "{Test ra mode}: [ra] [100] [macid] [mode 0: fixed rate, 1:fixed macidcfg]\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "{Fix rate} [ra] [1] [macid] [mode] [giltf] [is_mu/ss_mcs(dec)] [bw])\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "{Auto rate}: [ra] [2] [macid] [X] [X] [is_mu/X(dec)] [X]\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "=============Notes=============>\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "[mode]: 0:(legacy), 1:(HT), 2:(VHT), 3:(HE)\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "[giltf]: 0: (4xHE-LTF 3.2usGI, LGI), 1: (4xHE-LTF 0.8usGI, SGI), 2: (2xHE-LTF 1.6usGI)\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "[giltf]: 3: (2xHE-LTF 0.8usGI), 4: (1xHE-LTF 1.6usGI), 5: (1xHE-LTF 0.8usGI)\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "[is_mu/ss_mcs]: (Bitmap format) [6]: is_mu [5:4]:Nss(0=1SS, 1=2SS) [3:0]:MCS\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "==============================>\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "{Drvier shift rate up/down threshold}: [ra] [3] [macid] [0: Increase th. (Tend to RU) 1: Decrease th (Tend to RD)] [percent]\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "{Manually adjust RA mask}: [ra] [4] [macid] [0: dis. manual adj. RA mask; 1: en. manual adj. RA mask] [0: mask; 1: reveal] [rate_mode] [ss_mcs]\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "{d_o_timer}: [ra] [5] [macid] [en] [timer (FW default 20)]\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "{cls head/tail}: [ra] [6] [en]\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "{debug bb reg}: [ra] [dbgreg]\n");
		//BB_DBG_CNSL(out_len, used, output + used, out_len - used,
		//	 "{Fix rate & ra mask}: ra (3 [macid] [mode] [giltf] [ss_mcs] [mask1] [mask0])}\n");
		goto out;
	}
	for (i = 0; i < 8; i++) {
		if (input[i + 1])
			HALBB_SCAN(input[i + 1], DCMD_DECIMAL, &val[i]);
	}
	//rssi_len = sizeof(struct bb_h2c_rssi_setting);
	//rssi_i = hal_mem_alloc(bb->hal_com, rssi_len);

	if (0) { /* Test RA mode */
		/*
		bb_h2c = (u32 *) &ra_cfg;
		ra_cfg.bw_cap = 0;
		ra_cfg.mode_ctrl = (u8)val[2];
		ra_cfg.is_dis_ra= false;
		ra_cfg.macid = (u8)val[1];
		ra_cfg.stbc_cap = false;
		ra_cfg.ldpc_cap = false;
		ra_cfg.en_sgi = false;
		ra_cfg.upd_all = true;
		ra_cfg.init_rate_lv = 1;
		ra_cfg.er_cap = false;
		ra_cfg.dcm_cap = false;
		ra_cfg.upd_mask = false;
		ra_cfg.upd_bw_nss_mask = false;
		ra_cfg.giltf = 0;
		ra_cfg.ss_num = 1;
		for (i = 0; i < 8; i++)
			ra_cfg.ramask[i] = 0xff;
		BB_DBG(bb, DBG_RA, "RA Register: %x %x %x\n", bb_h2c[0], bb_h2c[1],
		       bb_h2c[2]);
		ret_val = halbb_fill_h2c_cmd(bb, cmdlen, RA_H2C_MACIDCFG, HALBB_H2C_RA, bb_h2c);
		*/
	} else if (val[0] == 1) {
		bb_h2c = ra_rssi.val;
		cmdlen = sizeof(union bb_h2c_ra_rssi_info);

		phl_sta = bb->phl_sta_info[(u8)val[1]];

		if (!phl_sta) {
			BB_WARNING("[%s][1] macid=%d\n", __func__, val[1]);
			return;
		}

		hal_sta_i = phl_sta->hal_sta;

		bb_ra = &hal_sta_i->ra_info;
		if (bb_ra->ra_registered) {
			BB_DBG(bb, DBG_RA, "RA fix rate macid=[%d]\n", (u8)val[1]);
			/* Need modify for Nss > 2 */
			rssi_a = (hal_sta_i->rssi_stat.rssi_ma_path[0] >> 5) & 0x7f;
			bcn_rssi_a = (hal_sta_i->rssi_stat.rssi_bcn_ma_path[0] >> 5) & 0x7f;
			if (!bb->hal_com->dbcc_en) {
				rssi_b = (hal_sta_i->rssi_stat.rssi_ma_path[1] >> 5) & 0x7f;
				bcn_rssi_b = (hal_sta_i->rssi_stat.rssi_bcn_ma_path[1] >> 5) & 0x7f;
			}
			rssi_i->macid = (u8)val[1];
			rssi_i->rssi_a = rssi_a | BIT(7);
			rssi_i->rssi_b = rssi_b;
			rssi_i->bcn_rssi_a = bcn_rssi_a | BIT(7);
			rssi_i->bcn_rssi_b = bcn_rssi_b;
			rssi_i->is_fixed_rate = true;
			rssi_i->fixed_rate = (u8)val[4];
			rssi_i->fixed_giltf = (u8)val[3];
			rssi_i->fixed_bw = (u8)val[5];
			rssi_i->fixed_rate_md = (u8)val[2];
			rssi_i->endcmd = 1;
			if (bb->ic_type & BB_IC_BE_SERIES) {
				ra_rssi.bb_h2c_ra_rssi_wifi7.fixed_rate_M = ((u8)val[4] >> 7) & 0x1;
				ra_rssi.bb_h2c_ra_rssi_wifi7.fixed_bw_M = ((u8)val[5] >> 2) & 0x1;
				ra_rssi.bb_h2c_ra_rssi_wifi7.fixed_rate_md_M = ((u8)val[2] >> 2) & 0x1;
				bb_ra->fixed_rt_i.mcs_ss_idx = (ra_rssi.bb_h2c_ra_rssi_wifi7.fixed_rate_M << 7) | rssi_i->fixed_rate;
				bb_ra->fixed_rt_i.bw = (ra_rssi.bb_h2c_ra_rssi_wifi7.fixed_bw_M << 2) | rssi_i->fixed_bw;
				bb_ra->fixed_rt_i.mode = (ra_rssi.bb_h2c_ra_rssi_wifi7.fixed_rate_md_M << 2) | rssi_i->fixed_rate_md;
			} else {
				bb_ra->fixed_rt_i.mcs_ss_idx = rssi_i->fixed_rate;
				bb_ra->fixed_rt_i.bw = rssi_i->fixed_bw;
				bb_ra->fixed_rt_i.mode = rssi_i->fixed_rate_md;
			}
			bb_ra->fixed_rt_en = rssi_i->is_fixed_rate;
			bb_ra->fixed_rt_i.gi_ltf = rssi_i->fixed_giltf;
			BB_DBG(bb, DBG_RA, "RA fix rate H2C: %x %x\n", bb_h2c[0], bb_h2c[1]);

			ret_val = halbb_fill_h2c_cmd(bb, cmdlen, RA_H2C_RSSISETTING, HALBB_H2C_RA, bb_h2c);
		} else {
			BB_DBG(bb, DBG_RA, "No Link ! RA rssi cmd fail!\n");
		}
	} else if (val[0] == 2) {
		bb_h2c = ra_rssi.val;
		cmdlen = sizeof(union bb_h2c_ra_rssi_info);

		phl_sta = bb->phl_sta_info[(u8)val[1]];

		if (!phl_sta) {
			BB_WARNING("[%s][2] macid=%d\n", __func__, val[1]);
			return;
		}

		hal_sta_i = phl_sta->hal_sta;
		bb_ra = &hal_sta_i->ra_info;
		if (bb_ra->ra_registered) {
			BB_DBG(bb, DBG_RA, "RA auto rate macid=[%d]\n", (u8)val[1]);
			/* Need modify for Nss > 2 */
			rssi_a = (hal_sta_i->rssi_stat.rssi_ma_path[0] >> 5) & 0x7f;
			bcn_rssi_a = (hal_sta_i->rssi_stat.rssi_bcn_ma_path[0] >> 5) & 0x7f;
			if (!bb->hal_com->dbcc_en) {
				rssi_b = (hal_sta_i->rssi_stat.rssi_ma_path[1] >> 5) & 0x7f;
				bcn_rssi_b = (hal_sta_i->rssi_stat.rssi_bcn_ma_path[1] >> 5) & 0x7f;
		}
		rssi_i->macid = (u8)val[1];
		rssi_i->rssi_a = rssi_a | BIT(7);
		rssi_i->rssi_b = rssi_b;
		rssi_i->bcn_rssi_a = bcn_rssi_a | BIT(7);
		rssi_i->bcn_rssi_b = bcn_rssi_b;
		rssi_i->is_fixed_rate = false;
		rssi_i->endcmd = 1;
		bb_ra->fixed_rt_en = rssi_i->is_fixed_rate;
		BB_DBG(bb, DBG_RA, "RA auto rate H2C: %x %x\n", bb_h2c[0], bb_h2c[1]);

		ret_val = halbb_fill_h2c_cmd(bb, cmdlen, RA_H2C_RSSISETTING, HALBB_H2C_RA, bb_h2c);
		} else {
			BB_DBG(bb, DBG_RA, "No Link ! RA rssi cmd fail!\n");
		}
	} else if (val[0] == 3) {
		bb_h2c = (u32 *) &ra_th_i;
		cmdlen = sizeof(struct bb_h2c_ra_adjust);

		hal_sta_i = bb->phl_sta_info[(u8)val[1]]->hal_sta;
		bb_ra = &hal_sta_i->ra_info;
		if (bb_ra->ra_registered) {
			BB_DBG(bb, DBG_RA, "RA adjust th. macid=[%d]\n", (u8)val[1]);
			ra_th_i.macid = (u8)val[1];
			ra_th_i.drv_shift_en = (u8)val[2] & 0x01;
			ra_th_i.drv_shift_value= (u8)val[3] & 0x7f;

			BB_DBG(bb, DBG_RA, "RA adjust %s th =[%d]\n",
			ra_th_i.drv_shift_en == 0x1 ? "RD": "RU", ra_th_i.drv_shift_value);

			BB_DBG(bb, DBG_RA, "RA adjust th H2C: %x %x\n", bb_h2c[0], bb_h2c[1]);

			ret_val = halbb_fill_h2c_cmd(bb, cmdlen, RA_H2C_RA_ADJUST, HALBB_H2C_RA, bb_h2c);
		} else {
			BB_DBG(bb, DBG_RA, "No Link ! RA rssi cmd fail!\n");
		}
	} else if (val[0] == 4) {
		bb_h2c = (u32 *) &ra_mask_i;
		cmdlen = sizeof(struct bb_h2c_ra_mask);

		hal_sta_i = bb->phl_sta_info[(u8)val[1]]->hal_sta;
		bb_ra = &hal_sta_i->ra_info;

		if (bb_ra->ra_registered) {
			BB_DBG(bb, DBG_RA, "macid=[%d]\n", (u8)val[1]);

			ra_mask_i.macid = (u8)val[1];
			ra_mask_i.is_manual_adjust_ra_mask = (u8)val[2] & 0x01;
			ra_mask_i.mask_or_reveal = (u8)val[3] & 0x01;
			ra_mask_i.mask_rate_md= (u8)val[4];
			ra_mask_i.mask_rate= (u8)val[5];

			BB_DBG(bb, DBG_RA, "Manual adjust RA mask = %d\n",
			       ra_mask_i.is_manual_adjust_ra_mask);
			BB_DBG(bb, DBG_RA, "Adjust mode=%d, rate(d')=%d\n",
			       ra_mask_i.mask_rate_md, ra_mask_i.mask_rate);
			BB_DBG(bb, DBG_RA, "[%s] this rate in RA mask\n",
			       ra_mask_i.mask_or_reveal ? "Reveal": "Mask");
			BB_DBG(bb, DBG_RA, "RA adjust th H2C: %x %x\n",
			       bb_h2c[0], bb_h2c[1]);

			ret_val = halbb_fill_h2c_cmd(bb, cmdlen, RA_H2C_ADJUST_RA_MASK, HALBB_H2C_RA, bb_h2c);
		} else {
			BB_DBG(bb, DBG_RA, "No Link ! RA rssi cmd fail!\n");
		}
	} else if (val[0] == 5) {
		bb_h2c = (u32 *) &ra_d_o_timer_i;
		cmdlen = sizeof(struct bb_h2c_ra_d_o_timer);

		hal_sta_i = bb->phl_sta_info[(u8)val[1]]->hal_sta;
		bb_ra = &hal_sta_i->ra_info;
		if (bb_ra->ra_registered) {
			BB_DBG(bb, DBG_RA, "RA d_o_timer macid=[%d]\n", (u8)val[1]);
			ra_d_o_timer_i.macid = (u8)val[1];
			ra_d_o_timer_i.d_o_timer_en = (u8)val[2] & 0x01;
			ra_d_o_timer_i.d_o_timer_value= (u8)val[3] & 0x7f;

			BB_DBG(bb, DBG_RA, "RA d_o_timer {en,value} = {%d,%d}\n",
			ra_d_o_timer_i.d_o_timer_en , ra_d_o_timer_i.d_o_timer_value);

			BB_DBG(bb, DBG_RA, "RA d_o_timer H2C: %x\n", bb_h2c[0]);

			ret_val = halbb_fill_h2c_cmd(bb, cmdlen, RA_H2C_RA_D_O_TIMER, HALBB_H2C_RA, bb_h2c);
		} else {
			BB_DBG(bb, DBG_RA, "No Link ! RA d_o_timer cmd fail!\n");
		}
	}  else if (val[0] == 6) {
		bb_h2c = (u32 *)h2c_ra_cls;
		cmdlen = 1;

		h2c_ra_cls[0] = (u8)(val[1] & 0x01); /*ra_cls_en*/
		h2c_ra_cls[1] = 0; /*rsvd*/
		h2c_ra_cls[2] = 0; /*rsvd*/
		h2c_ra_cls[3] = 0; /*rsvd*/

		BB_DBG(bb, DBG_RA, "RA cls head/tail en=%d\n",
		h2c_ra_cls[0]);

		BB_DBG(bb, DBG_RA, "RA cls head/tail H2C: %x\n", bb_h2c[0]);

		ret_val = halbb_fill_h2c_cmd(bb, cmdlen, RA_H2C_RA_CLS, HALBB_H2C_RA, bb_h2c);
	} else if (_os_strcmp(input[1], "dbgreg") == 0) {
		halbb_get_ra_dbgreg(bb);
		halbb_ra_dbgreg_cnsl(bb, _used, output, _out_len);
	}
	//if (rssi_i)
	//	hal_mem_free(bb->hal_com, rssi_i, rssi_len);
out:
	*_used = used;
	*_out_len = out_len;
}

#endif



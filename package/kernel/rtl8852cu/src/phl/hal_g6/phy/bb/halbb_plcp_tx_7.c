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
#include "halbb_plcp_gen.h"
//#include "halbb_plcp_tx.h"
//#include "halbb_he_sigb_gen.h"
#ifdef HALBB_PMAC_TX_SUPPORT

const u8 ru_alloc_b1_7_tbl_7[RU_SIZE_NUM][2] = {{0, 37}, //RU26
					      {0, 16}, //RU52
					      {0, 8},  //RU106
					      {0, 4},  //RU242
					      {0, 2},  //RU484
					      {0, 1},  //RU996
					      {1, 1},  //RU996x2
					      {0, 0},  //HESIGB
					      {2, 0},  //RU996x4
					      {0, 12}, //RU52+26
					      {0, 8},  //RU106_26
					      {0, 4},  //RU484_242
					      {1, 4},  //RU996_484
					      {1, 8},  //RU996_484_242
					      {2, 0},  //RU996x2_484
					      {2, 0},  //RU996x3
					      {2, 0}}; //RU996x3_484

u8 halbb_set_crc8_7(struct bb_info *bb, unsigned char in[], u8 len)
{
	u16 i = 0;
	u8 reg0 = 1;
	u8 reg1 = 1;
	u8 reg2 = 1;
	u8 reg3 = 1;
	u8 reg4 = 1;
	u8 reg5 = 1;
	u8 reg6 = 1;
	u8 reg7 = 1;
	u8 bit_in = 0;
	u8 out = 0;

	for (i = 0; i < len; i++) {
		bit_in = in[i] ^ reg7;
		reg7 = reg6;
		reg6 = reg5;
		reg5 = reg4;
		reg4 = reg3;
		reg3 = reg2;
		reg2 = bit_in ^ reg1;
		reg1 = bit_in ^ reg0;
		reg0 = bit_in;
	}
	out = (reg0 << 7) | (reg1 << 6) | (reg2 << 5) | (reg3 << 4) |
		  (reg4 << 3) | (reg5 << 2) | (reg6 << 1) | reg7;
	return ~out;

}

void halbb_ppdu_var_type_cfg_7(struct bb_info *bb_0, struct halbb_plcp_info *in,
			     enum phl_phy_idx phy_idx)
{
	struct bb_info *bb = bb_0;
	u8 ppdu_type = 0, ppdu_var = 0;
	struct bb_plcp_cr_info *cr = &bb->bb_plcp_i.bb_plcp_cr_i;

#ifdef HALBB_DBCC_SUPPORT
	HALBB_GET_PHY_PTR(bb_0, bb, phy_idx);
#endif

	switch (in->ppdu_type) {
	case B_MODE_FMT:
		ppdu_type = in->long_preamble_en ? 0 : 1;
		ppdu_var = 0;
		break;
	case LEGACY_FMT: // Not support (Duplication + puncture)
		ppdu_type = 2;
		ppdu_var = in->dbw > DBW20 ? 1 : 0;
		break;
	case HT_MF_FMT:
		ppdu_type = 3;
		ppdu_var = in->ndp_en ? 1 : 0;
		break;
	case VHT_FMT:
		ppdu_type = 5;
		ppdu_var = in->ndp_en ? 1 : 0;
		break;
	case HE_SU_FMT: // Not support (NDP + puncture) & (ranging NDP)
		ppdu_type = 7;
		ppdu_var = in->ndp_en ? 1 : 0;
		break;
	case HE_ER_SU_FMT:
		ppdu_type = 8;
		ppdu_var = 0;
		break;
	case HE_TB_FMT: // Not support (ranging NDP)
		ppdu_type = 10;
		ppdu_var = in->ndp_en ? 1 : 0;
		break;
	case EHT_MU_SU_FMT:
		ppdu_type = 11;
		if ((in->punc_pattern == 0xff) || (in->punc_pattern == 0xf)) // Full bw
			ppdu_var = in->ndp_en ? 4 : 0;
		else // With puncture
			ppdu_var = in->ndp_en ? 7 : 5;
		break;
	case EHT_MU_ERSU_FMT:
		ppdu_type = 11;
		ppdu_var = 1;
		break;
	case EHT_TB_FMT:
		ppdu_type = 12;
		ppdu_var = 0;
		break;
	default:
		BB_DBG(bb, DBG_BIT14, "[%s] Invalid/Unsupported PPDU type = %d\n",
		       __func__, in->ppdu_type);
		break;
	}
	BB_DBG(bb, DBG_BIT14, "[%s] PPDU type=%d, {type, var}={%d, %d}\n",
	       __func__,  in->ppdu_type, ppdu_type, ppdu_var);

	halbb_set_reg_cmn(bb, cr->ppdu_type, cr->ppdu_type_m, ppdu_type, phy_idx);
	halbb_set_reg_cmn(bb, cr->ppdu_var, cr->ppdu_var_m, ppdu_var, phy_idx);
}

u32 halbb_ru_occupied_sub20_he_7(struct bb_info *bb, struct halbb_plcp_info *in)
{
	u32 out = 0;

	switch (in->usr[0].ru_size) {
	case RU26:
		if (in->usr[0].ru_idx <= 9)
			out = 0x1;
		else if (in->usr[0].ru_idx == 10)
			out = in->dbw >= DBW80 ? 0x3 : 0x2;
		else if (in->usr[0].ru_idx <= 18)
			out = 0x2;
		else if (in->usr[0].ru_idx == 19)
			out = 0x6;
		else if (in->usr[0].ru_idx <= 27)
			out = 0x4;
		else if (in->usr[0].ru_idx == 28)
			out = 0xc;
		else if (in->usr[0].ru_idx <= 37)
			out = 0x8;
		else if (in->usr[0].ru_idx <= 46)
			out = 0x10;
		else if (in->usr[0].ru_idx == 47)
			out = 0x30;
		else if (in->usr[0].ru_idx <= 55)
			out = 0x20;
		else if (in->usr[0].ru_idx == 56)
			out = 0x60;
		else if (in->usr[0].ru_idx <= 64)
			out = 0x40;
		else if (in->usr[0].ru_idx == 65)
			out = 0xc0;
		else
			out = 0x80;
		break;
	case RU52:
		if (in->usr[0].ru_idx <= 4)
			out = 0x1;
		else if (in->usr[0].ru_idx == 5)
			out = in->dbw >= DBW80 ? 0x3 : 0x2;
		else if (in->usr[0].ru_idx <= 8)
			out = 0x2;
		else if (in->usr[0].ru_idx <= 11)
			out = 0x4;
		else if (in->usr[0].ru_idx == 12)
			out = 0xc;
		else if (in->usr[0].ru_idx <= 16)
			out = 0x8;
		else if (in->usr[0].ru_idx <= 20)
			out = 0x10;
		else if (in->usr[0].ru_idx == 21)
			out = 0x30;
		else if (in->usr[0].ru_idx <= 24)
			out = 0x20;
		else if (in->usr[0].ru_idx <= 27)
			out = 0x40;
		else if (in->usr[0].ru_idx == 28)
			out = 0xc0;
		else
			out = 0x80;
		break;
	case RU106:
		if (in->usr[0].ru_idx <= 2)
			out = 0x1;
		else if (in->usr[0].ru_idx == 3)
			out = in->dbw >= DBW80 ? 0x3 : 0x2;
		else if (in->usr[0].ru_idx == 4)
			out = 0x2;
		else if (in->usr[0].ru_idx == 5)
			out = 0x4;
		else if (in->usr[0].ru_idx == 6)
			out = 0xc;
		else if (in->usr[0].ru_idx <= 8)
			out = 0x8;
		else if (in->usr[0].ru_idx <= 10)
			out = 0x10;
		else if (in->usr[0].ru_idx == 11)
			out = 0x30;
		else if (in->usr[0].ru_idx == 12)
			out = 0x20;
		else if (in->usr[0].ru_idx == 13)
			out = 0x40;
		else if (in->usr[0].ru_idx == 14)
			out = 0xc0;
		else
			out = 0x80;
		break;
	case RU242:
		if (in->usr[0].ru_idx == 1)
			out = 0x1;
		else if (in->usr[0].ru_idx == 2)
			out = in->dbw >= DBW80 ? 0x3 : 0x2;
		else if (in->usr[0].ru_idx == 3)
			out = 0xc;
		else if (in->usr[0].ru_idx == 4)
			out = 0x8;
		else if (in->usr[0].ru_idx == 5)
			out = 0x10;
		else if (in->usr[0].ru_idx == 6)
			out = 0x30;
		else if (in->usr[0].ru_idx == 7)
			out = 0xc0;
		else
			out = 0x80;
		break;
	case RU484:
		if (in->usr[0].ru_idx == 1)
			out = 0x3;
		else if (in->usr[0].ru_idx == 2)
			out = 0xc;
		else if (in->usr[0].ru_idx == 3)
			out = 0x30;
		else
			out = 0xc0;
		break;
	case RU996:
		out = in->usr[0].ru_idx == 1 ? 0xf : 0xf0;
		break;
	case RU996X2:
		out = 0xff;
		break;
	default:
		break;
	}
	return out;
}

u32 halbb_ru_occupied_sub20_eht_7(struct bb_info *bb, struct halbb_plcp_info *in)
{
	u32 output = 0;
	u8 ch20_with_data_dbw80 = 0, ch20_with_data_dbw160 = 0, nsub_80 = 4, mru_idx = 0, n_x1_flag = 0, n_x1 = 0;
	u8 size_idx = 0, num_b1_7 = 0;
	u8 ru_484_242_tbl[4] = {0xE, 0xD, 0xB, 0x7}; // {4b'1110, 4b'1101, 4b'1011, 4b'0111}
	u8 ru_996_484_tbl[4] = {0xFC, 0xF3, 0xCF, 0x3F}; // {8b'11111100, 8b'11110011, 8b'11001111, 8b'00111111}
	u16 ru_996x2_484_tbl[12] = {0xFFC, 0xFF3, 0xFCF, 0xF3F, 0xCFF, 0x3FF,
				    0xFFC0, 0xFF30, 0xFCF0, 0xF3F0, 0xCFF0, 0x3FF0};

	size_idx = ru_alloc_b1_7_tbl_7[in->usr[0].ru_size][0];
	num_b1_7 = ru_alloc_b1_7_tbl_7[in->usr[0].ru_size][1];

	n_x1_flag = size_idx;
	mru_idx = size_idx == 2 ? (u8)in->usr[0].ru_idx : (u8)(halbb_mod(in->usr[0].ru_idx -1, num_b1_7) + 1);
	n_x1 = size_idx == 2 ? 0 : (u8)(in->usr[0].ru_idx -1) / num_b1_7;

	if (n_x1_flag == 0) {
		switch (in->usr[0].ru_size) {
		case RU26:
			if (mru_idx <= 9)
				ch20_with_data_dbw80 = 0x1;
			else if (mru_idx == 10)
				ch20_with_data_dbw80 = in->dbw >= DBW80 ? 0x3 : 0x2;
			else if (mru_idx <= 18)
				ch20_with_data_dbw80 = 0x2;
			else if (mru_idx == 19)
				ch20_with_data_dbw80 = 0x6;
			else if (mru_idx <= 27)
				ch20_with_data_dbw80 = 0x4;
			else if (mru_idx == 28)
				ch20_with_data_dbw80 = 0xc;
			else if (mru_idx <= 37)
				ch20_with_data_dbw80 = 0x8;
			else if (mru_idx <= 46)
				ch20_with_data_dbw80 = 0x10;
			else if (mru_idx == 47)
				ch20_with_data_dbw80 = 0x30;
			else if (mru_idx <= 55)
				ch20_with_data_dbw80 = 0x20;
			else if (mru_idx == 56)
				ch20_with_data_dbw80 = 0x60;
			else if (mru_idx <= 64)
				ch20_with_data_dbw80 = 0x40;
			else if (mru_idx == 65)
				ch20_with_data_dbw80 = 0xc0;
			else
				ch20_with_data_dbw80 = 0x80;
			break;
		case RU52:
			ch20_with_data_dbw80 = 1 << ((mru_idx - 1) / 4);
			break;
		case RU106:
			ch20_with_data_dbw80 = 1 << ((mru_idx - 1) / 2);
			break;
		case RU242:
			ch20_with_data_dbw80 = 1 << (mru_idx - 1);
			break;
		case RU484:
			ch20_with_data_dbw80 = 0x3 << ((mru_idx - 1) * 2);
			break;
		case RU996:
			ch20_with_data_dbw80 = 0xf;
			break;
		case RU52_26:
			ch20_with_data_dbw80 = 1 << ((mru_idx - 1) / 3);
			break;
		case RU106_26:
			ch20_with_data_dbw80 = 1 << ((mru_idx - 1) / 2);
			break;
		case RU484_242:
			ch20_with_data_dbw80 = ru_484_242_tbl[mru_idx - 1];
			break;
		default:
			break;
		}
		output = ch20_with_data_dbw80 << (nsub_80 * n_x1);
	} else if (n_x1_flag == 1) {
		switch (in->usr[0].ru_size) {
		case RU996X2:
			ch20_with_data_dbw160 = 0xff;
			break;
		case RU996_484:
			ch20_with_data_dbw160 = ru_996_484_tbl[mru_idx - 1];
			break;
		case RU996_484_242:
			ch20_with_data_dbw160 = 0xff ^ (1 << (mru_idx - 1));
			break;
		default:
			break;
		}
		output = ch20_with_data_dbw160 << (nsub_80 * n_x1 * 2);
	} else if (n_x1_flag == 2) {
		switch (in->usr[0].ru_size) {
		case RU996X2_484:
			output = ru_996x2_484_tbl[mru_idx - 1];
			break;
		case RU996X3:
			output = 0xffff ^ (0xf << (4 * (mru_idx - 1)));
			break;
		case RU996X3_484:
			output = 0xffff ^ (0x3 << (2 * (mru_idx - 1)));
			break;
		case RU996X4:
			output = 0xffff;
			break;
		default:
			break;
		}
	}
	return output;
}

u8 halbb_eht_sig_gi_ltf_tbl_7(struct bb_info *bb, u8 gi, u8 ltf)
{
	u8 out = 0xff;

	BB_DBG(bb, DBG_BIT14, "[EHT SIG] gi=%d, ltf=%d\n", gi, ltf);

	if (gi == 1 && ltf == 1) // 2x_0.8
		out = 0;
	else if (gi == 1 && ltf == 2) //2x_1.6
		out = 1;
	else if (gi == 2 && ltf == 1) //4x_0.8
		out = 2;
	else if (gi == 2 && ltf == 3) //4x_3.2
		out = 3;
	else
		BB_WARNING("Invalid GI_LTF for EHT-SIG!!\n");

	return out;
}

u32 halbb_cfg_ch20_with_data_7(struct bb_info *bb, struct halbb_plcp_info *in)
{
	u32 ch20_with_data = 0;

	if (in->ppdu_type == HE_TB_FMT) {
		ch20_with_data = halbb_ru_occupied_sub20_he_7(bb, in);
	} else if ((in->ppdu_type == EHT_MU_RU_FMT) || (in->ppdu_type == EHT_TB_FMT)) {
		ch20_with_data = halbb_ru_occupied_sub20_eht_7(bb, in);
	} else {
		switch (in->dbw) {
			case 0:
				ch20_with_data = 0x1;
				break;
			case 1:
				ch20_with_data = 0x3;
				break;
			case 2:
				ch20_with_data = 0xf;
				break;
			case 3:
				ch20_with_data = 0xff;
				break;
			default:
				break;
		}
	}

	return ch20_with_data;
}

void halbb_ppdu_type_comp_mode_trans_7(struct bb_info *bb, bool ul_dl_flag,
				     enum packet_format_t ppdu_type,
				     struct plcp_tx_pre_fec_padding_setting_out_t *out_plcp)
{
	if (ul_dl_flag == 0)
		out_plcp->ppdu_type_comp_mode = ppdu_type == EHT_MU_RU_FMT ? 0 : 1;
	else
		out_plcp->ppdu_type_comp_mode = ppdu_type == EHT_TB_FMT ? 0 : 1;
}

bool halbb_punc_ch_info_2_ru_size_idx_7(struct bb_info *bb, u8 user_idx, enum plcp_dbw dbw,
				      u8 ppdu_type_comp_mode, u8 punc_pattern,
				      struct plcp_tx_pre_fec_padding_setting_out_t *out_plcp)
{
	// This is for EHT_MU_SU
	u8 i = 0;
	bool pattern_chk = false;
	const u8 dbw80_punc_pattern[5] = {0xf, 0x7, 0xb, 0xd, 0xe};
	const u8 dbw160_punc_pattern[13] = {0xff, 0x7f, 0xbf, 0xdf, 0xef, 0xf7, 0xfb, 0xfd, 0xfe,
					    0x3f, 0xcf, 0xf3, 0xfc};
	const u8 dbw320_punc_pattern[25] = {0xff, 0x7f, 0xbf, 0xdf, 0xef, 0xf7, 0xfb, 0xfd, 0xfe,
					    0x3f, 0xcf, 0xf3, 0xfc,
					    0x1f, 0x2f, 0x37, 0x3b, 0x3d, 0x3e, 0x7c, 0xbc, 0xdc, 0xec, 0xf4, 0xf8};

	BB_DBG(bb, DBG_BIT14,
	       "[EHT punc_ch_info] dbw=%d, ppdu_type_comp_mode=%d, punc_pattern=%d\n",
	       dbw, ppdu_type_comp_mode, punc_pattern);

	switch (ppdu_type_comp_mode){
	case  0:
		break;
	case 1:
	case 2:
		switch (dbw) {
		case DBW20:
			out_plcp->usr[user_idx].ru_idx = 1;
			out_plcp->usr[user_idx].ru_size = RU242;
			out_plcp->punc_ch_info = 0;
			pattern_chk = true;
			break;
		case DBW40:
			out_plcp->usr[user_idx].ru_idx = 1;
			out_plcp->usr[user_idx].ru_size = RU484;
			out_plcp->punc_ch_info = 0;
			pattern_chk = true;
			break;
		case DBW80:
			for (i = 0; i < 5; i++) {
				if (punc_pattern == dbw80_punc_pattern[i]) {
					out_plcp->punc_ch_info = i;
					pattern_chk = true;
				}
			}
			out_plcp->usr[user_idx].ru_idx = out_plcp->punc_ch_info == 0 ? 1 : out_plcp->punc_ch_info;
			out_plcp->usr[user_idx].ru_size = out_plcp->punc_ch_info == 0 ? RU996 : RU484_242;
			break;
		case DBW160:
			for (i = 0; i < 13; i++) {
				if (punc_pattern == dbw160_punc_pattern[i]) {
					out_plcp->punc_ch_info = i;
					pattern_chk = true;
				}
			}
			if (out_plcp->punc_ch_info == 0) {
				out_plcp->usr[user_idx].ru_idx = 1;
				out_plcp->usr[user_idx].ru_size = RU996X2;
			} else if (out_plcp->punc_ch_info <= 8) {
				out_plcp->usr[user_idx].ru_idx = out_plcp->punc_ch_info;
				out_plcp->usr[user_idx].ru_size = RU996_484_242;
			} else if (out_plcp->punc_ch_info <= 12) {
				out_plcp->usr[user_idx].ru_idx = out_plcp->punc_ch_info - 8;
				out_plcp->usr[user_idx].ru_size = RU996_484;
			} else {
				BB_DBG(bb, DBG_BIT14, "[EHT punc_ch_info] Invalid puncturing pattern for BW160!\n");
			}
			break;
		case DBW320:
			for (i = 0; i < 25; i++) {
				if (punc_pattern == dbw320_punc_pattern[i]) {
					out_plcp->punc_ch_info = i;
					pattern_chk = true;
				}
			}
			if (out_plcp->punc_ch_info == 0) {
				out_plcp->usr[user_idx].ru_idx = 1;
				out_plcp->usr[user_idx].ru_size = RU996X4;
			} else if (out_plcp->punc_ch_info <= 8) {
				out_plcp->usr[user_idx].ru_idx = out_plcp->punc_ch_info;
				out_plcp->usr[user_idx].ru_size = RU996X3_484;
			} else if (out_plcp->punc_ch_info <= 12) {
				out_plcp->usr[user_idx].ru_idx = out_plcp->punc_ch_info - 8;
				out_plcp->usr[user_idx].ru_size = RU996X3;
			} else if (out_plcp->punc_ch_info <= 18) {
				out_plcp->usr[user_idx].ru_idx = out_plcp->punc_ch_info - 6;
				out_plcp->usr[user_idx].ru_size = RU996X2_484;
			} else if (out_plcp->punc_ch_info <= 24) {
				out_plcp->usr[user_idx].ru_idx = out_plcp->punc_ch_info - 18;
				out_plcp->usr[user_idx].ru_size = RU996X2_484;
			} else {
				BB_DBG(bb, DBG_BIT14, "[EHT punc_ch_info] Invalid puncturing pattern for BW320!\n");
			}
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}

	BB_DBG(bb, DBG_BIT14, "[EHT punc_ch_info][User%d] ru_idx=%d, ru_size=%d\n",
	       user_idx, out_plcp->usr[user_idx].ru_idx, out_plcp->usr[user_idx].ru_size);

	return pattern_chk;
}

u32 halbb_ru_size_id_2_ru_alloc_7(struct bb_info *bb, u8 ru_id, u8 ru_size, u8 prim_sb, enum plcp_dbw dbw)
{
	u32 ru_alloc = 0;
//	out_plcp->usr[0].ru_idx = 1;
//	out_plcp->usr[0].ru_size = RU242;
	const u8 BW80_RU_id_max_table[12] = {37, 16, 8, 4, 2, 1, 0, 0, 0, 12, 8, 4};
	const u8 RU_allocation_offset_table[17] = {0, 37, 53, 61, 65, 67, 68, 0, 69, 70, 82, 90, 94, 96, 100, 104, 105};
	const u8 RU996x2_484_allocation_offset_table_specific[12] = {0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3};
	u8 seg_p160_p80 = 0, seg_p160_s80 = 0, seg_s160_l80 = 0, seg_s160_u80 = 0;
	bool B_0 = false, PS160 = false;
	u8 loc_80 = 0, BW80_RU_id = 0;
	bool B0_RU996_484[8] = {0, 0, 1, 1, 0, 0, 1, 1};
	bool B0_RU996_484_242[16] = {0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1};
	bool PS160_RU996x2[2] = {0, 1};
	bool B0_RU996x2_484[12] = {0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1};
	bool PS160_RU996x2_484[12] = {0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1};
	bool B0_RU996x3[4] = {0, 1, 0, 1};
	bool PS160_RU996x3[4] = {0, 0, 1, 1};
	bool B0_RU996x3_484[8] = {0, 0, 1, 1, 0, 0, 1, 1};
	bool PS160_RU996x3_484[8] = {0, 0, 0, 0, 1, 1, 1, 1};

	if (dbw <= DBW80) {
		if (!((ru_size <= RU996) || (ru_size >= RU52_26 && ru_size <= RU484_242)))
			BB_WARNING("Illegal ru_size in BW80 !!");
		PS160 = 0;
		B_0 = 0;
		ru_alloc = PS160 * 256 + (RU_allocation_offset_table[ru_size] + ru_id - 1) * 2 + B_0;
	} else if (dbw == DBW160) {
		PS160 = 0;

		if (ru_size == RU996_484) {
			if (!(ru_id > 0 && ru_id <= 4))
				BB_WARNING("Illegal ru_id when ru_size = RU996_484 in BW160 !!");
			B_0 = B0_RU996_484[ru_id - 1];
			ru_alloc = PS160 * 256 + (RU_allocation_offset_table[ru_size] + (ru_id % 2 == 0 ? 1 : 0)) * 2 + B_0;
		} else if (ru_size == RU996_484_242) {
			if (!(ru_id > 0 && ru_id <= 8))
				BB_WARNING("Illegal ru_id when ru_size = RU996_484_242 in BW160 !!");
			B_0 = B0_RU996_484_242[ru_id - 1];
			ru_alloc = PS160 * 256 + (RU_allocation_offset_table[ru_size] + (ru_id > 4 ? ru_id - 4 : ru_id) - 1) * 2 + B_0;
		} else if (ru_size == RU996X2) {
			if (!(ru_id == 1))
				BB_WARNING("Illegal ru_id when ru_size = RU996x2 in BW160 !!");
			B_0 = 1;
			ru_alloc = PS160 * 256 + (RU_allocation_offset_table[ru_size] + ru_id - 1) * 2 + B_0;
		} else { // RU_size <= RU996
			if (prim_sb < 4) {
				seg_p160_p80 = 0;
				seg_p160_s80 = 1;
			} else if (prim_sb < 8) {
				seg_p160_p80 = 1;
				seg_p160_s80 = 0;
			} else {
				BB_WARNING("prim_sb is illegal !!");
			}

			// Determine which 80 it lacates
			if (!(ru_size <= RU484_242))
				BB_WARNING("Illegal ru_size in BW160 !!");

			loc_80 = (ru_id % BW80_RU_id_max_table[ru_size] == 0) ? (ru_id / BW80_RU_id_max_table[ru_size] - 1) : (ru_id / BW80_RU_id_max_table[ru_size]);

			if (!(loc_80 >= 0 && loc_80 <= 1))
				BB_WARNING("loc_80 is illegal in BW160 !!");

			if (loc_80 == seg_p160_s80)
				B_0 = 1;
			else
				B_0 = 0;

			BW80_RU_id = ru_id - loc_80 * BW80_RU_id_max_table[ru_size];
			ru_alloc = PS160 * 256 + (RU_allocation_offset_table[ru_size] + BW80_RU_id - 1) * 2 + B_0;
		}
	} else if (dbw == DBW320) {
		if (ru_size == RU996_484) {
			if (!(ru_id > 0 && ru_id <= 8))
				BB_WARNING("Illegal ru_id when ru_size = RU996_484 in BW320 !!");

			B_0 = B0_RU996_484[ru_id - 1];

			if (prim_sb >= 8) { //[S160][P160]
				if (ru_id >= 5)
					PS160 = 0;
				else
					PS160 = 1;
			} else {//[P160][S160]
				if (ru_id >= 5)
					PS160 = 1;
				else
					PS160 = 0;
			}

			ru_alloc = PS160 * 256 + (RU_allocation_offset_table[ru_size] + ((ru_id % 2) == 0 ? 1 : 0)) * 2 + B_0;
		} else if (ru_size == RU996_484_242) {
			if (!(ru_id > 0 && ru_id <= 16))
				BB_WARNING("Illegal ru_id when ru_size = RU996_484_242 in BW320 !!");

			B_0 = B0_RU996_484_242[ru_id - 1];

			if (prim_sb >= 8) { //[S160][P160]
				if (ru_id >= 9)
					PS160 = 0;
				else
					PS160 = 1;
			} else { //[P160][S160]
				if (ru_id >= 9)
					PS160 = 1;
				else
					PS160 = 0;
			}

			ru_alloc = PS160 * 256 + (RU_allocation_offset_table[ru_size] + (ru_id > 4 ? ru_id - 4 : ru_id) - 1) * 2 + B_0;
		} else if (ru_size == RU996X2) {
			if (!(ru_id > 0 && ru_id <= 2))
				BB_WARNING("Illegal ru_id when ru_size = RU996x2 in BW320 !!");

			B_0 = 1;
			PS160 = PS160_RU996x2[ru_id - 1];

			ru_alloc = PS160 * 256 + (RU_allocation_offset_table[ru_size]) * 2 + B_0;
		} else if (ru_size == RU996X2_484) {
			if (!(ru_id > 0 && ru_id <= 12))
				BB_WARNING("Illegal ru_id when ru_size = RU996x2_484 in BW320 !!");

			B_0 = B0_RU996x2_484[ru_id - 1];
			PS160 = PS160_RU996x2_484[ru_id - 1];

			ru_alloc = PS160 * 256 + (RU_allocation_offset_table[ru_size] + RU996x2_484_allocation_offset_table_specific[ru_id - 1]) * 2 + B_0;
		} else if (ru_size == RU996X3) {
			if (!(ru_id >0 && ru_id <= 4))
				BB_WARNING("Illegal ru_id when ru_size = RU996x3 in BW320 !!");

			B_0 = B0_RU996x3[ru_id - 1];
			PS160 = PS160_RU996x3[ru_id - 1];

			ru_alloc = PS160 * 256 + (RU_allocation_offset_table[ru_size]) * 2 + B_0;
		} else if (ru_size == RU996X3_484) {
			if (!(ru_id > 0 && ru_id <= 8))
				BB_WARNING("Illegal ru_id when ru_size = RU996x3_484 in BW320 !!");

			B_0 = B0_RU996x3_484[ru_id - 1];
			PS160 = PS160_RU996x3_484[ru_id - 1];

			ru_alloc = PS160 * 256 + (RU_allocation_offset_table[ru_size] + (ru_id % 2 == 0 ? 1 : 0)) * 2 + B_0;
		} else if (ru_size == RU996X4) {
			if (!(ru_id == 1))
				BB_WARNING("Illegal ru_id when ru_size = RU996x4 in BW320 !!");

			B_0 = 1;
			PS160 = 1;

			ru_alloc = PS160 * 256 + (RU_allocation_offset_table[ru_size] + (ru_id % 2 == 0 ? 1 : 0)) * 2 + B_0;
		} else {// ru_size <= RU996
			// Determine seg_p160_p80, seg_p160_s80. seg_s160_l80, seg_s160_u80 by prim_sb
			if (prim_sb < 4) {
				seg_p160_p80 = 0;
				seg_p160_s80 = 1;
				seg_s160_l80 = 2;
				seg_s160_u80 = 3;
			} else if (prim_sb < 8) {
				seg_p160_p80 = 1;
				seg_p160_s80 = 0;
				seg_s160_l80 = 2;
				seg_s160_u80 = 3;
			} else if (prim_sb < 12) {
				seg_p160_p80 = 2;
				seg_p160_s80 = 3;
				seg_s160_l80 = 0;
				seg_s160_u80 = 1;
			} else if (prim_sb < 16) {
				seg_p160_p80 = 3;
				seg_p160_s80 = 2;
				seg_s160_l80 = 0;
				seg_s160_u80 = 1;
			} else {
				BB_WARNING("prim_sb is illegal !!");
			}

			if (!(ru_size <= RU484_242))
				BB_WARNING("Illegal ru_size in BW320 !!");

			loc_80 = ru_id % BW80_RU_id_max_table[ru_size] == 0 ? (ru_id / BW80_RU_id_max_table[ru_size] - 1) : (ru_id / BW80_RU_id_max_table[ru_size]);

			if (!(loc_80 >= 0 && loc_80 <= 3))
				BB_WARNING("loc_80 is illegal in BW320 !!");

			if (loc_80 == seg_p160_p80 || loc_80 == seg_p160_s80)
				PS160 = 0;
			else
				PS160 = 1;

			if (loc_80 == seg_p160_s80 || loc_80 == seg_s160_u80)
				B_0 = 1;
			else
				B_0 = 0;

			BW80_RU_id = ru_id - loc_80 * BW80_RU_id_max_table[ru_size];
			ru_alloc = PS160 * 256 + (RU_allocation_offset_table[ru_size] + BW80_RU_id - 1) * 2 + B_0;
		}
	}

	return ru_alloc;
}

void halbb_he_sigb_7(struct bb_info *bb, struct halbb_plcp_info *in,
		   struct plcp_tx_pre_fec_padding_setting_in_t *in_plcp,
		   struct plcp_tx_pre_fec_padding_setting_out_t *out,
		   enum phl_phy_idx phy_idx)
{
	struct bb_h2c_ehtsig_sigb *he_sigb = &bb->bb_h2c_ehtsig_sigb_i;
	u16 i = 0;
	u16 cmdlen;
	bool ret_val = false;
	u32 *bb_h2c = (u32 *)he_sigb;
	bool he_sigb_valid = false;
	bool he_sigb_pol = false;
	u16 he_n_sigb_sym = 0;
	u32 ru_alloc = 0;

	struct bb_plcp_cr_info *cr = &bb->bb_plcp_i.bb_plcp_cr_i;

	u32 hesigb_ehtsig_cr[80] = {cr->hesigb_ehtsig_0, cr->hesigb_ehtsig_1,
				    cr->hesigb_ehtsig_2, cr->hesigb_ehtsig_3,
				    cr->hesigb_ehtsig_4, cr->hesigb_ehtsig_5,
				    cr->hesigb_ehtsig_6, cr->hesigb_ehtsig_7,
				    cr->hesigb_ehtsig_8, cr->hesigb_ehtsig_9,
				    cr->hesigb_ehtsig_10, cr->hesigb_ehtsig_11,
				    cr->hesigb_ehtsig_12, cr->hesigb_ehtsig_13,
				    cr->hesigb_ehtsig_14, cr->hesigb_ehtsig_15,
				    cr->hesigb_ehtsig_16, cr->hesigb_ehtsig_17,
				    cr->hesigb_ehtsig_18, cr->hesigb_ehtsig_19,
				    cr->hesigb_ehtsig_20, cr->hesigb_ehtsig_21,
				    cr->hesigb_ehtsig_22, cr->hesigb_ehtsig_23,
				    cr->hesigb_ehtsig_24, cr->hesigb_ehtsig_25,
				    cr->hesigb_ehtsig_26, cr->hesigb_ehtsig_27,
				    cr->hesigb_ehtsig_28, cr->hesigb_ehtsig_29,
				    cr->hesigb_ehtsig_30, cr->hesigb_ehtsig_31,
				    cr->hesigb_ehtsig_32, cr->hesigb_ehtsig_33,
				    cr->hesigb_ehtsig_34, cr->hesigb_ehtsig_35,
				    cr->hesigb_ehtsig_36, cr->hesigb_ehtsig_37,
				    cr->hesigb_ehtsig_38, cr->hesigb_ehtsig_39,
				    cr->hesigb_ehtsig_40, cr->hesigb_ehtsig_41,
				    cr->hesigb_ehtsig_42, cr->hesigb_ehtsig_43,
				    cr->hesigb_ehtsig_44, cr->hesigb_ehtsig_45,
				    cr->hesigb_ehtsig_46, cr->hesigb_ehtsig_47,
				    cr->hesigb_ehtsig_48, cr->hesigb_ehtsig_49,
				    cr->hesigb_ehtsig_50, cr->hesigb_ehtsig_51,
				    cr->hesigb_ehtsig_52, cr->hesigb_ehtsig_53,
				    cr->hesigb_ehtsig_54, cr->hesigb_ehtsig_55,
				    cr->hesigb_ehtsig_56, cr->hesigb_ehtsig_57,
				    cr->hesigb_ehtsig_58, cr->hesigb_ehtsig_59,
				    cr->hesigb_ehtsig_60, cr->hesigb_ehtsig_61,
				    cr->hesigb_ehtsig_62, cr->hesigb_ehtsig_63,
				    cr->hesigb_ehtsig_64, cr->hesigb_ehtsig_65,
				    cr->hesigb_ehtsig_66, cr->hesigb_ehtsig_67,
				    cr->hesigb_ehtsig_68, cr->hesigb_ehtsig_69,
				    cr->hesigb_ehtsig_70, cr->hesigb_ehtsig_71,
				    cr->hesigb_ehtsig_72, cr->hesigb_ehtsig_73,
				    cr->hesigb_ehtsig_74, cr->hesigb_ehtsig_75,
				    cr->hesigb_ehtsig_76, cr->hesigb_ehtsig_77,
				    cr->hesigb_ehtsig_78, cr->hesigb_ehtsig_79};

	for (i = 0; i < 80; i++)
		halbb_set_reg(bb, hesigb_ehtsig_cr[i], MASKDWORD, 0);

	if (phy_idx == HW_PHY_0) {
		for (i = 0; i < 80; i++) {
			he_sigb->ehtsig_sigb_cr[i].address_0 = (u8)(hesigb_ehtsig_cr[i] & 0xff);
			he_sigb->ehtsig_sigb_cr[i].address_1 = (u8)((hesigb_ehtsig_cr[i] & 0xff00) >> 8);
			he_sigb->ehtsig_sigb_cr[i].address_2 = (u8)((hesigb_ehtsig_cr[i] & 0xff0000) >> 16);
			he_sigb->ehtsig_sigb_cr[i].address_3 = (u8)((hesigb_ehtsig_cr[i] & 0xff000000) >> 24);
		}
	}

	cmdlen = sizeof(struct bb_h2c_ehtsig_sigb);

	he_sigb->ehtsig_sigb = false;

	he_sigb->ehtsig_sigb_i.ppdu_bw = (u8)in->dbw;
	he_sigb->ehtsig_sigb_i.sta_list_num = (u8)in->n_user;

	for (i = 0; i < in->n_user; i++) {
		ru_alloc = halbb_ru_size_id_2_ru_alloc_7(bb, (u8)out->usr[i].ru_idx,
							 (u8)out->usr[i].ru_size,
							 (u8)in->txsb,
							 (enum plcp_dbw)in->dbw);
		he_sigb->ehtsig_sigb_i.usr_info[i].coding = (u8)in->usr[i].fec;
		he_sigb->ehtsig_sigb_i.usr_info[i].nsts = (u8)(in->usr[i].nss << in->stbc) - 1;
		he_sigb->ehtsig_sigb_i.usr_info[i].ru_position = (u8)in->usr[i].ru_alloc;
		he_sigb->ehtsig_sigb_i.usr_info[i].dcm = (u8)in->usr[i].dcm;
		he_sigb->ehtsig_sigb_i.usr_info[i].mcs = (u8)in->usr[i].mcs;
	}

	ret_val = halbb_fill_h2c_cmd(bb, cmdlen, DM_H2C_FW_EHTSIG_SIGB,
				     HALBB_H2C_DM, bb_h2c);

	BB_DBG(bb, DBG_PHY_CONFIG, "[SIGB] {ppdu_bw, sta_list_num}={%d,%d}\n", he_sigb->ehtsig_sigb_i.ppdu_bw, he_sigb->ehtsig_sigb_i.sta_list_num);
	BB_DBG(bb, DBG_PHY_CONFIG, "[SIGB] {coding,nsts,ru_position,dcm,mcs}={%d,%d,%d,%d,%d}\n",
		he_sigb->ehtsig_sigb_i.usr_info[0].coding,
		he_sigb->ehtsig_sigb_i.usr_info[0].nsts,
		he_sigb->ehtsig_sigb_i.usr_info[0].ru_position,
		he_sigb->ehtsig_sigb_i.usr_info[0].dcm,
		he_sigb->ehtsig_sigb_i.usr_info[0].mcs);
	/*
	BB_DBG(bb, DBG_PHY_CONFIG, "[SIGB] User0 NSS = %d\n", in->usr[0].nss);
	BB_DBG(bb, DBG_PHY_CONFIG, "[SIGB] User0 MCS = %d\n", in->usr[0].mcs);
	BB_DBG(bb, DBG_PHY_CONFIG, "[SIGB] User0 AID = %d\n", in->usr[0].aid);
	BB_DBG(bb, DBG_PHY_CONFIG, "[SIGB] User0 ru position = %d\n", he_sigb->dl_rua_out.dl_output_sta_list[0].ru_position);
	BB_DBG(bb, DBG_PHY_CONFIG, "[SIGB] User0 dropping flag = %d\n", he_sigb->dl_rua_out.dl_output_sta_list[0].dropping_flag);
	BB_DBG(bb, DBG_PHY_CONFIG, "[SIGB] SIGB Addr = 0x%x, Mask = 0x%x\n",
		he_sigb->n_sym_sigb_ch1[0].address, he_sigb->n_sym_sigb_ch1[0].bitmask);
	*/

	for (i = 0; i < 500; i++) {
		halbb_delay_us(bb, 10);
		he_sigb_pol = (bool)halbb_get_reg(bb, 0x1e0, BIT(16));
		if (he_sigb_pol) {
			he_sigb_valid = (bool)halbb_get_reg(bb, 0x1e0, BIT(8));
			he_n_sigb_sym = (u16)halbb_get_reg(bb, 0x1e0, 0x3f);
			in_plcp->n_hesigb_sym = he_n_sigb_sym;
			break;
		}
	}
}

void halbb_eht_sig_7(struct bb_info *bb, struct halbb_plcp_info *in,
		   struct plcp_tx_pre_fec_padding_setting_in_t *in_plcp,
		   struct plcp_tx_pre_fec_padding_setting_out_t *out,
		   enum phl_phy_idx phy_idx)
{
	struct bb_h2c_ehtsig_sigb *eht_sig = &bb->bb_h2c_ehtsig_sigb_i;
	u16 i = 0;
	u16 cmdlen;
	bool ret_val = false;
	u32 *bb_h2c = (u32 *)eht_sig;
	bool eht_sig_valid = false;
	bool eht_sig_pol = false;
	u16 eht_sig_n_sym = 0;
	u8 nss_max = 0, txsb_para = 0;
	u32 ru_alloc = 0;
	u32 ch20_with_data = halbb_ru_occupied_sub20_eht_7(bb, in);
	struct bb_plcp_cr_info *cr = &bb->bb_plcp_i.bb_plcp_cr_i;

	u32 hesigb_ehtsig_cr[80] = {cr->hesigb_ehtsig_0, cr->hesigb_ehtsig_1,
				    cr->hesigb_ehtsig_2, cr->hesigb_ehtsig_3,
				    cr->hesigb_ehtsig_4, cr->hesigb_ehtsig_5,
				    cr->hesigb_ehtsig_6, cr->hesigb_ehtsig_7,
				    cr->hesigb_ehtsig_8, cr->hesigb_ehtsig_9,
				    cr->hesigb_ehtsig_10, cr->hesigb_ehtsig_11,
				    cr->hesigb_ehtsig_12, cr->hesigb_ehtsig_13,
				    cr->hesigb_ehtsig_14, cr->hesigb_ehtsig_15,
				    cr->hesigb_ehtsig_16, cr->hesigb_ehtsig_17,
				    cr->hesigb_ehtsig_18, cr->hesigb_ehtsig_19,
				    cr->hesigb_ehtsig_20, cr->hesigb_ehtsig_21,
				    cr->hesigb_ehtsig_22, cr->hesigb_ehtsig_23,
				    cr->hesigb_ehtsig_24, cr->hesigb_ehtsig_25,
				    cr->hesigb_ehtsig_26, cr->hesigb_ehtsig_27,
				    cr->hesigb_ehtsig_28, cr->hesigb_ehtsig_29,
				    cr->hesigb_ehtsig_30, cr->hesigb_ehtsig_31,
				    cr->hesigb_ehtsig_32, cr->hesigb_ehtsig_33,
				    cr->hesigb_ehtsig_34, cr->hesigb_ehtsig_35,
				    cr->hesigb_ehtsig_36, cr->hesigb_ehtsig_37,
				    cr->hesigb_ehtsig_38, cr->hesigb_ehtsig_39,
				    cr->hesigb_ehtsig_40, cr->hesigb_ehtsig_41,
				    cr->hesigb_ehtsig_42, cr->hesigb_ehtsig_43,
				    cr->hesigb_ehtsig_44, cr->hesigb_ehtsig_45,
				    cr->hesigb_ehtsig_46, cr->hesigb_ehtsig_47,
				    cr->hesigb_ehtsig_48, cr->hesigb_ehtsig_49,
				    cr->hesigb_ehtsig_50, cr->hesigb_ehtsig_51,
				    cr->hesigb_ehtsig_52, cr->hesigb_ehtsig_53,
				    cr->hesigb_ehtsig_54, cr->hesigb_ehtsig_55,
				    cr->hesigb_ehtsig_56, cr->hesigb_ehtsig_57,
				    cr->hesigb_ehtsig_58, cr->hesigb_ehtsig_59,
				    cr->hesigb_ehtsig_60, cr->hesigb_ehtsig_61,
				    cr->hesigb_ehtsig_62, cr->hesigb_ehtsig_63,
				    cr->hesigb_ehtsig_64, cr->hesigb_ehtsig_65,
				    cr->hesigb_ehtsig_66, cr->hesigb_ehtsig_67,
				    cr->hesigb_ehtsig_68, cr->hesigb_ehtsig_69,
				    cr->hesigb_ehtsig_70, cr->hesigb_ehtsig_71,
				    cr->hesigb_ehtsig_72, cr->hesigb_ehtsig_73,
				    cr->hesigb_ehtsig_74, cr->hesigb_ehtsig_75,
				    cr->hesigb_ehtsig_76, cr->hesigb_ehtsig_77,
				    cr->hesigb_ehtsig_78, cr->hesigb_ehtsig_79};

	if (in->ppdu_type >= EHT_TB_FMT)
		return;

	for (i = 0; i < 80; i++)
		halbb_set_reg(bb, hesigb_ehtsig_cr[i], MASKDWORD, 0);

	if (phy_idx == HW_PHY_0) {
		for (i = 0; i < 80; i++) {
			eht_sig->ehtsig_sigb_cr[i].address_0 = (u8)(hesigb_ehtsig_cr[i] & 0xff);
			eht_sig->ehtsig_sigb_cr[i].address_1 = (u8)((hesigb_ehtsig_cr[i] & 0xff00) >> 8);
			eht_sig->ehtsig_sigb_cr[i].address_2 = (u8)((hesigb_ehtsig_cr[i] & 0xff0000) >> 16);
			eht_sig->ehtsig_sigb_cr[i].address_3 = (u8)((hesigb_ehtsig_cr[i] & 0xff000000) >> 24);
		}
	}

	cmdlen = sizeof(struct bb_h2c_ehtsig_sigb);

	eht_sig->ehtsig_sigb = true;
	eht_sig->ehtsig_sigb_mcs = (u8)in->eht_mcs_sig;

	eht_sig->ehtsig_sigb_i.ppdu_bw = (u8)in->dbw;
	eht_sig->ehtsig_sigb_i.sta_list_num = (u8)in->n_user;

	txsb_para = (u8)(in->txsb * BIT(in->dbw));

	/*-----------------------------------------------------------------------*/
	eht_sig->ehtsig_sigb_i.ch20_with_data_0 = (u8)(ch20_with_data & 0xff);
	eht_sig->ehtsig_sigb_i.ch20_with_data_1 = (u8)((ch20_with_data & 0xff00) >> 8);

	// Apply valid TXSB to prevent EHT-SIG error.
	if ((in->punc_pattern != 0xf) && (in->punc_pattern != 0xff)) {
		// Puncture
		if (in->dbw == DBW80)
			txsb_para = (in->punc_pattern & 0x8) ? 0 : 2;
		else if (in->dbw == DBW160)
			txsb_para = (in->punc_pattern & 0x80) ? 0 : 4;
		else if (in->dbw == DBW320)
			txsb_para = (in->punc_pattern & 0x80) ? 0 : 8 +
				    ((in->punc_pattern & 0x8) ? 0 : 2);
		else
			txsb_para = 0;
	} else {
		txsb_para = (u8)(in->txsb * BIT(in->dbw));
	}
	eht_sig->ehtsig_sigb_i.pri_txsb = txsb_para;
	eht_sig->ehtsig_sigb_i.ul_dl = (u8)in->ul_flag;
	eht_sig->ehtsig_sigb_i.ppdu_type_comp_mode = (u8)out->ppdu_type_comp_mode;
	eht_sig->ehtsig_sigb_i.usig_spat_reuse = (u8)in->ul_srp1;
	eht_sig->ehtsig_sigb_i.usig_ltf_symb = nss_max == 1 ? 1 : (u8)(halbb_ceil (nss_max, 2) * 2);
	eht_sig->ehtsig_sigb_i.usig_nss = 0;
	eht_sig->ehtsig_sigb_i.usig_bf = 0;
	eht_sig->ehtsig_sigb_i.usig_spat_gi_ltf = halbb_eht_sig_gi_ltf_tbl_7(bb, (u8)in->gi, (u8)in->he_ltf_type);
	eht_sig->ehtsig_sigb_i.usig_disregard_ndp = 1;
	eht_sig->ehtsig_sigb_i.usig_ldpc_extra_symb_seg = (u8)out->ldpc_extra;
	eht_sig->ehtsig_sigb_i.usig_prefec = (u8)out->pre_fec_padding_factor;
	eht_sig->ehtsig_sigb_i.usig_pe_disambiguity = (u8)out->disamb;
	eht_sig->ehtsig_sigb_i.usig_disregard = 1;

	//for (i = 0; i < in->n_user; i++) {
	for (i = 0; i < 1; i++) {
		ru_alloc = halbb_ru_size_id_2_ru_alloc_7(bb, (u8)out->usr[i].ru_idx,
							 (u8)out->usr[i].ru_size,
							 (u8)in->txsb,
							 (enum plcp_dbw)in->dbw);

		eht_sig->ehtsig_sigb_i.usr_info[i].coding = (u8)in->usr[i].fec;
		eht_sig->ehtsig_sigb_i.usr_info[i].nsts = (u8)(in->usr[i].nss << in->stbc) - 1;
		eht_sig->ehtsig_sigb_i.usr_info[i].ru_position = (u8)(ru_alloc & 0xff);
		eht_sig->ehtsig_sigb_i.usr_info[i].dcm = (u8)in->usr[i].dcm;
		eht_sig->ehtsig_sigb_i.usr_info[i].mcs = (u8)in->usr[i].mcs;
		eht_sig->ehtsig_sigb_i.usr_info[i].ps160 = (ru_alloc & BIT(8)) >> 8;
	}

	ret_val = halbb_fill_h2c_cmd(bb, cmdlen, DM_H2C_FW_EHTSIG_SIGB,
				     HALBB_H2C_DM, bb_h2c);

	BB_DBG(bb, DBG_PHY_CONFIG,
	       "[EHT-SIG] {ppdu_bw, sta_list_num}={%d,%d}\n",
	       eht_sig->ehtsig_sigb_i.ppdu_bw,
	       eht_sig->ehtsig_sigb_i.sta_list_num);
	BB_DBG(bb, DBG_PHY_CONFIG,
	       "[EHT-SIG] {coding,nsts,ru_position,dcm,mcs}={%d,%d,%d,%d,%d,%d}\n",
	       eht_sig->ehtsig_sigb_i.usr_info[0].coding,
	       eht_sig->ehtsig_sigb_i.usr_info[0].nsts,
	       eht_sig->ehtsig_sigb_i.usr_info[0].ru_position,
	       eht_sig->ehtsig_sigb_i.usr_info[0].dcm,
	       eht_sig->ehtsig_sigb_i.usr_info[0].mcs,
	       eht_sig->ehtsig_sigb_i.usr_info[0].ps160);

	for (i = 0; i < 500; i++) {
		halbb_delay_us(bb, 10);
		eht_sig_pol = (bool)halbb_get_reg(bb, 0x1e0, BIT(16));
		if (eht_sig_pol) {
			eht_sig_valid = (bool)halbb_get_reg(bb, 0x1e0, BIT(8));
			eht_sig_n_sym = (u16)halbb_get_reg(bb, 0x1e0, 0x3f);
			// Shared para. with n_ehtsig_sym
			in_plcp->n_hesigb_sym = eht_sig_n_sym;
			break;
		}
	}
}

bool halbb_ru_info_init_7(struct bb_info *bb, struct halbb_plcp_info *in,
			struct plcp_tx_pre_fec_padding_setting_in_t *in_plcp,
			struct plcp_tx_pre_fec_padding_setting_out_t *out,
			enum phl_phy_idx phy_idx)
{
	// Only supports 1 user temporarily
	u8 i = 0;

	halbb_ppdu_type_comp_mode_trans_7(bb, (bool)in->ul_flag, in->ppdu_type, out);

	for (i = 0; i < 1; i++) {
		if ((in->ppdu_type == HE_MU_FMT) ||
		    (in->ppdu_type == HE_TB_FMT) ||
		    (in->ppdu_type == EHT_MU_RU_FMT) ||
		    (in->ppdu_type == EHT_TB_FMT)) {
			out->usr[i].ru_size = in->usr[i].ru_size;
			out->usr[i].ru_idx = in->usr[i].ru_idx;
		} else {
			if (!halbb_punc_ch_info_2_ru_size_idx_7(bb, i, in->dbw, (u8)out->ppdu_type_comp_mode, (u8)in->punc_pattern, out))
				return false;
		}
	}

	return true;
}

void halbb_plcp_gen_init_7(struct bb_info *bb, struct halbb_plcp_info *in,
			 struct plcp_tx_pre_fec_padding_setting_in_t *in_plcp)
{
	u32 i = 0;

	//Outer Input
	in->source_gen_mode = 2;
	in->locked_clk = 1;
	in->dyn_bw = 0;
	in->ndp_en = 0;
	in->doppler = 0;
	in->ht_l_len = 0;
	in->preamble_puncture = 0;
	in->he_sigb_compress_en = 1;
	in->ul_flag = 0;
	in->bss_color= 10;
	in->sr = 0;
	in->beamchange_en = 1;
	in->ul_srp1 = in->ppdu_type >= EHT_MU_ERSU_FMT ? 0xf : 0;
	in->ul_srp2 = in->ppdu_type >= EHT_MU_ERSU_FMT ? 0xf : 0;
	in->ul_srp3 = 0;
	in->ul_srp4 = 0;
	in->group_id = 63;
	in->txop = 127;
	in->nominal_t_pe = 2;
	in->ness = 0;
	in->tb_rsvd = 0;
	in->vht_txop_not_allowed = 0;

	// Test
#if 0
	if (in->ppdu_type >= EHT_MU_SU_FMT) {
		in->gi = 1; // Force 0.8
		in->he_ltf_type = 1; // Force 2x
	}
#endif
	for (i = 0; i < in->n_user; i++) {
		in->usr[i].mpdu_len = 0;
		in->usr[i].n_mpdu = 0;
		in->usr[i].txbf = 0;
	}

	//PLCP Input
	in_plcp->format_idx = (u8)in->ppdu_type;
	in_plcp->stbc = (u8)in->stbc;
	in_plcp->he_dcm_sigb = (u8)in->he_dcm_sigb;
	in_plcp->doppler_mode = (u8)in->doppler;
	in_plcp->he_mcs_sigb = (u16)in->he_mcs_sigb;
	in_plcp->nominal_t_pe = (u16)in->nominal_t_pe;
	in_plcp->dbw = (u8)in->dbw;
	in_plcp->gi = (u8)in->gi;
	in_plcp->ltf_type = (u8)in->he_ltf_type;
	in_plcp->ness = (u16)in->ness;
	in_plcp->mode_idx = (u8)in->mode;
	in_plcp->max_tx_time_0p4us = in->max_tx_time_0p4us;
	in_plcp->n_user = in->n_user;
	in_plcp->ndp = in->ndp_en;
	in_plcp->he_er_u106ru_en = in->he_er_u106ru_en;
	in_plcp->tb_l_len = in->tb_l_len;
	in_plcp->tb_ru_tot_sts_max = in->tb_ru_tot_sts_max;
	in_plcp->tb_disam = in->tb_disam;
	in_plcp->tb_ldpc_extra = in->tb_ldpc_extra;
	in_plcp->tb_pre_fec_padding_factor = in->tb_pre_fec_padding_factor;
	in_plcp->ht_l_len = in->ht_l_len;
	for (i = 0; i < in_plcp->n_user; i++) {
		in_plcp->usr[i].nss = (u8)in->usr[i].nss;
		in_plcp->usr[i].fec = (u8)in->usr[i].fec;
		in_plcp->usr[i].apep = in->usr[i].apep;
		in_plcp->usr[i].dcm = (bool)in->usr[i].dcm;
		in_plcp->usr[i].mcs = (u8)in->usr[i].mcs;
		in_plcp->usr[i].mpdu_length_byte = (u16)in->usr[i].mpdu_len;
		in_plcp->usr[i].n_mpdu = in->usr[i].n_mpdu;
		if (in->ppdu_type >= EHT_MU_SU_FMT)
			in_plcp->usr[i].dcm = (in->usr[i].mcs == 14 || in->usr[i].mcs == 15) ? true : false;
	}
}

void halbb_plcp_lsig_7(struct bb_info *bb, struct halbb_plcp_info *in,
		     struct plcp_tx_pre_fec_padding_setting_out_t *out_plcp,
		     enum phl_phy_idx phy_idx)
{
	bool parity = 0;
	u8 lsig_rate = 0;
	u32 lsig_bits = 0;
	u32 lsig = 0;
	u8 i = 0;
	struct bb_plcp_cr_info *cr = &bb->bb_plcp_i.bb_plcp_cr_i;

	BB_DBG(bb, DBG_BIT14, "<====== %s ======>\n", __func__);

	if (in->ppdu_type == LEGACY_FMT) {
		switch (in->usr[0].mcs) {
			case 0:
				lsig_rate = 11;
				break;
			case 1:
				lsig_rate = 15;
				break;
			case 2:
				lsig_rate = 10;
				break;
			case 3:
				lsig_rate = 14;
				break;
			case 4:
				lsig_rate = 9;
				break;
			case 5:
				lsig_rate = 13;
				break;
			case 6:
				lsig_rate = 8;
				break;
			case 7:
				lsig_rate = 12;
				break;
			default:
				break;
		}
	} else {
		lsig_rate = 11;
	}
	lsig_bits = ((out_plcp->l_len) << 5) + lsig_rate;
	for (i = 0; i < 17; i++)
		parity ^= (lsig_bits >> i) % 2;

	halbb_set_bit(0, 4, lsig_rate, &lsig);
	halbb_set_bit(4, 1, 0, &lsig);// rsvd //
	halbb_set_bit(5, 12, out_plcp->l_len, &lsig);
	halbb_set_bit(17, 1, parity, &lsig);
	halbb_set_bit(18, 6, 0, &lsig);
	/*=== Write CR ===*/
	halbb_set_reg_cmn(bb, cr->lsig, cr->lsig_m, lsig, phy_idx);
}

void halbb_plcp_siga_7(struct bb_info *bb, struct halbb_plcp_info *in,
		     struct plcp_tx_pre_fec_padding_setting_out_t *out_plcp,
	     	     enum phl_phy_idx phy_idx)
{
	u32 siga1 = 0;
	u32 siga2 = 0;
	unsigned char siga_bits[64] = {0};
	u8 crc8_out;
	u8 crc4_out = 0;
	u8 i = 0;
	u8 n_he_ltf[8] = { 0, 1, 1, 2, 2, 3, 3, 4 };
	struct bb_plcp_cr_info *cr = &bb->bb_plcp_i.bb_plcp_cr_i;

	BB_DBG(bb, DBG_BIT14, "<====== %s ======>\n", __func__);

	if ((in->ppdu_type == HE_SU_FMT) || (in->ppdu_type == HE_ER_SU_FMT)) { // HE_SU SIG-A //
		/*=== SIG-A1 ===*/
		halbb_set_bit(0, 1, 1, &siga1);
		halbb_set_bit(1, 1, in->beamchange_en, &siga1);
		halbb_set_bit(2, 1, in->ul_flag, &siga1);
		halbb_set_bit(3, 4, in->usr[0].mcs, &siga1);
		if ((in->gi == 1) && (in->he_ltf_type == 2))
			halbb_set_bit(7, 1, 1, &siga1);
		else
			halbb_set_bit(7, 1, out_plcp->usr[0].dcm, &siga1);
		halbb_set_bit(8, 6, in->bss_color, &siga1);
		halbb_set_bit(14, 1, 1, &siga1); // rsvd //
		halbb_set_bit(15, 4, in->sr, &siga1);
		if (in->ppdu_type == HE_ER_SU_FMT) {
			if (in->he_er_u106ru_en)
				halbb_set_bit(19, 2, 1, &siga1);
			else
				halbb_set_bit(19, 2, 0, &siga1);
		} else {
			halbb_set_bit(19, 2, in->dbw, &siga1);
		}
		if (out_plcp->gi == 1 && in->he_ltf_type == 0)
			halbb_set_bit(21, 2, 0, &siga1);// he_ltf_type & GI
		else if (out_plcp->gi == 1 && in->he_ltf_type == 1)
			halbb_set_bit(21, 2, 1, &siga1);
		else if (out_plcp->gi == 2 && in->he_ltf_type == 1)
			halbb_set_bit(21, 2, 2, &siga1);
		else if ((out_plcp->gi == 1 && in->he_ltf_type == 2) || (out_plcp->gi == 3 && in->he_ltf_type == 2))
			halbb_set_bit(21, 2, 3, &siga1);
		halbb_set_bit(23, 3, out_plcp->usr[0].nsts-1, &siga1);//NSTS & Midamble// doppler //???????????????
		/*=== SIG-A2 ===*/
		halbb_set_bit(0, 7, in->txop, &siga2);
		halbb_set_bit(7, 1, out_plcp->usr[0].fec, &siga2);
		if (out_plcp->usr[0].fec == 0)
			halbb_set_bit(8, 1, 1, &siga2);
		else
			halbb_set_bit(8, 1, out_plcp->ldpc_extra, &siga2);
		if ((in->gi == 1) && (in->he_ltf_type == 2))
			halbb_set_bit(9, 1, 1, &siga2);
		else
			halbb_set_bit(9, 1, out_plcp->stbc, &siga2);
		halbb_set_bit(10, 1, 0, &siga2);//Beamformed? //
		halbb_set_bit(11, 2, out_plcp->pre_fec_padding_factor, &siga2);
		halbb_set_bit(13, 1, out_plcp->disamb, &siga2);
		halbb_set_bit(14, 1, 1, &siga2); // rsvd //
		halbb_set_bit(15, 1, out_plcp->doppler_en, &siga2);
		//CRC4//
		//--- Set HESIG1 ---
		for(i = 0; i < 26; i++)
			siga_bits[i] = ( siga1 >> i ) & 0x1 ;
		//--- Set HESIG2 ---
		for(i = 0; i < 16; i++)
			siga_bits[i + 26] = ( siga2 >> i ) & 0x1 ;
		crc8_out = halbb_set_crc8_7(bb, siga_bits, 42);
		crc4_out = crc8_out & 0xf;
		halbb_set_bit(16, 4, crc4_out, &siga2);
		halbb_set_bit(20, 6, 0, &siga2);
	}
	else if (in->ppdu_type == HE_MU_FMT) { // HE MU SIG-A //
		/*=== SIG-A1 ===*/
		halbb_set_bit(0, 1, in->ul_flag, &siga1);
		halbb_set_bit(1, 3, in->he_mcs_sigb, &siga1);
		halbb_set_bit(4, 1, in->he_dcm_sigb, &siga1);
		halbb_set_bit(5, 6, in->bss_color, &siga1);
		halbb_set_bit(11, 4, in->sr, &siga1);
		halbb_set_bit(15, 3, in->dbw, &siga1); // Bandwidth = DBW
		halbb_set_bit(18, 4, out_plcp->n_sym_hesigb, &siga1);
		halbb_set_bit(22, 1, 0, &siga1);
		if (in->he_ltf_type == 2 && out_plcp->gi == 1)
			halbb_set_bit(23, 2, 0, &siga1);// he_ltf_type & GI
		else if (in->he_ltf_type == 1 && out_plcp->gi == 1)
			halbb_set_bit(23, 2, 1, &siga1);
		else if (in->he_ltf_type == 1 && out_plcp->gi == 2)
			halbb_set_bit(23, 2, 2, &siga1);
		else if (in->he_ltf_type == 2 && out_plcp->gi == 3)
			halbb_set_bit(23, 2, 3, &siga1);
		halbb_set_bit(25, 1, out_plcp->doppler_en, &siga1);
		/*=== SIG-A2 ===*/
		halbb_set_bit(0, 7, in->txop, &siga2);
		halbb_set_bit(7, 1, 1, &siga2); //rsvd
		halbb_set_bit(8, 3, n_he_ltf[out_plcp->n_ltf], &siga2);//N_LTF & Midamble// doppler
		halbb_set_bit(11, 1, out_plcp->ldpc_extra, &siga2);// LDPC extra symbpl seg  ldpc_extra 
		halbb_set_bit(12, 1, out_plcp->stbc, &siga2);
		halbb_set_bit(13, 2, out_plcp->pre_fec_padding_factor, &siga2);
		halbb_set_bit(15, 1, out_plcp->disamb, &siga2);
		//CRC4//
		//--- Set HESIG1 ---
		for(i = 0; i < 26; i++)
			siga_bits[i] = ( siga1 >> i ) & 0x1 ;
		//--- Set HESIG2 ---
		for(i = 0; i < 16; i++)
			siga_bits[i + 26] = ( siga2 >> i ) & 0x1 ;
		crc8_out = halbb_set_crc8_7(bb, siga_bits, 42);
		crc4_out = crc8_out & 0xf;
		halbb_set_bit(16, 4, crc4_out, &siga2);
		halbb_set_bit(20, 6, 0, &siga2);
		halbb_set_bit(20, 6, 0, &siga2);
	}
	else if (in->ppdu_type == HE_TB_FMT) { // HE_TB SIG-A //
		/*=== SIG-A1 ===*/
		halbb_set_bit(0, 1, 0, &siga1);
		halbb_set_bit(1, 6, in->bss_color, &siga1);
		halbb_set_bit(7, 4, in->ul_srp1, &siga1);
		halbb_set_bit(11, 4, in->ul_srp2, &siga1);
		halbb_set_bit(15, 4, in->ul_srp3, &siga1);
		halbb_set_bit(19, 4, in->ul_srp4, &siga1);
		halbb_set_bit(23, 1, 1, &siga1); // rsvd //
		halbb_set_bit(24, 2, in->dbw, &siga1);
		/*=== SIG-A2 ===*/
		halbb_set_bit(0, 7, in->txop, &siga2);
		halbb_set_bit(7, 9, in->tb_rsvd, &siga2);
		//CRC4//
		//--- Set HESIG1 ---
		for(i = 0; i < 26; i++)
			siga_bits[i] = ( siga1 >> i ) & 0x1 ;
		//--- Set HESIG2 ---
		for(i = 0; i < 16; i++)
			siga_bits[i + 26] = ( siga2 >> i ) & 0x1 ;
		crc8_out = halbb_set_crc8_7(bb, siga_bits, 42);
		crc4_out = crc8_out & 0xf;
		halbb_set_bit(16, 4, crc4_out, &siga2);
		halbb_set_bit(20, 6, 0, &siga2);
	}
	else if (in->ppdu_type == VHT_FMT) {// VHT SIG-A //
		/*=== SIG-A1 ===*/
		halbb_set_bit(0, 2, in->dbw, &siga1);
		halbb_set_bit(2, 1, 1, &siga1); // rsvd //
		halbb_set_bit(3, 1, out_plcp->stbc, &siga1);
		halbb_set_bit(4, 6, in->group_id, &siga1);
		halbb_set_bit(10, 3, out_plcp->usr[0].nsts-1, &siga1); // NSS //
		halbb_set_bit(13, 9, in->usr[0].aid, &siga1); // AID //
		halbb_set_bit(22, 1, in->vht_txop_not_allowed, &siga1);
		halbb_set_bit(23, 1, 1, &siga1);
		/*=== SIG-A2 ===*/
		if (out_plcp->gi == 0)
			halbb_set_bit(0, 1, 1, &siga2); // Short GI //
		else
			halbb_set_bit(0, 1, 0, &siga2);
		halbb_set_bit(1, 1, out_plcp->disamb, &siga2);
		halbb_set_bit(2, 1, out_plcp->usr[0].fec, &siga2);
		halbb_set_bit(3, 1, out_plcp->ldpc_extra, &siga2);
		halbb_set_bit(4, 4, in->usr[0].mcs, &siga2);
		halbb_set_bit(8, 1, 0, &siga2);//Beamformed? //
		halbb_set_bit(9, 1, 1, &siga2);// rsvd //
		//CRC8//
		 //--- Set HTSIG1 ---
		  for(i = 0; i < 24; i++)
		    siga_bits[i] = ( siga1 >> i ) & 0x1 ;

		  for(i = 0; i < 10; i++)
		    siga_bits[i+24] = ( siga2 >> i ) & 0x1 ;
		crc8_out = halbb_set_crc8_7(bb, siga_bits, 34);
		halbb_set_bit(10, 8, crc8_out, &siga2);
		halbb_set_bit(18, 6, 0, &siga2);
	}
	else if (in->ppdu_type == HT_MF_FMT) {// HT_MF SIG-A //
		/*=== SIG-A1 ===*/
		halbb_set_bit(0, 7, in->usr[0].mcs, &siga1);
		halbb_set_bit(7, 1, in->dbw, &siga1);
		halbb_set_bit(8, 16, out_plcp->usr[0].apep_len, &siga1);
		/*=== SIG-A2 ===*/
		halbb_set_bit(0, 1, 1, &siga2);
		halbb_set_bit(1, 1, ~in->ndp_en, &siga2);
		halbb_set_bit(2, 1, 1, &siga2);
		halbb_set_bit(3, 1, out_plcp->usr[0].n_mpdu > 1 ? 1 : 0, &siga2);
		halbb_set_bit(4, 2, out_plcp->usr[0].nsts - out_plcp->usr[0].nss, &siga2);
		halbb_set_bit(6, 1, out_plcp->usr[0].fec, &siga2);
		if (out_plcp->gi == 0)
			halbb_set_bit(7, 1, 1, &siga2);
		else
			halbb_set_bit(7, 1, 0, &siga2);
		halbb_set_bit(8, 2, in->ness, &siga2);
		//CRC8//
		//--- Set HTSIG1 ---
		for(i = 0; i < 24; i++)
			siga_bits[i] = ( siga1 >> i ) & 0x1 ;
		//--- Set HTSIG2 ---
		for(i = 0; i < 10; i++)
			siga_bits[i+24] = ( siga2 >> i ) & 0x1 ;
		crc8_out = halbb_set_crc8_7(bb, siga_bits, 34);
		halbb_set_bit(10, 8, crc8_out, &siga2);
		halbb_set_bit(18, 6, 0, &siga2);
	}
	/*=== Write CR ===*/
	halbb_set_reg_cmn(bb, cr->siga1, cr->siga1_m, siga1, phy_idx);
	halbb_set_reg_cmn(bb, cr->siga2, cr->siga2_m, siga2, phy_idx);
}

void halbb_plcp_usig_7(struct bb_info *bb, struct halbb_plcp_info *in,
		     struct plcp_tx_pre_fec_padding_setting_out_t *out_plcp,
	     	     enum phl_phy_idx phy_idx)
{
	u32 usig1 = 0, usig1_val = 0;
	u32 usig2 = 0, usig2_val = 0;
	unsigned char siga_bits[64] = {0};
	u8 crc8_out;
	u8 crc4_out = 0;
	u8 eht_sig_mcs = 0;
	u8 i = 0;
	u8 n_he_ltf[8] = { 0, 1, 1, 2, 2, 3, 3, 4 };
	struct bb_plcp_cr_info *cr = &bb->bb_plcp_i.bb_plcp_cr_i;

	BB_DBG(bb, DBG_BIT14, "<====== %s ======>\n", __func__);

	if (in->eht_mcs_sig == 0)
		eht_sig_mcs = 0;
	else if (in->eht_mcs_sig == 1)
		eht_sig_mcs = 1;
	else if (in->eht_mcs_sig == 3)
		eht_sig_mcs = 2;
	else if (in->eht_mcs_sig == 15)
		eht_sig_mcs = 3;
	else
		BB_WARNING("Invalid EHT-SIG MCS !!");

	if ((in->ppdu_type >= EHT_MU_SU_FMT) && (in->ppdu_type <= EHT_MU_RU_FMT)) {
		/*=== U-SIG-1 ===*/
		halbb_set_bit(0, 3, 0, &usig1);
		halbb_set_bit(3, 3, in->dbw, &usig1);
		halbb_set_bit(6, 1, in->ul_flag, &usig1);
		halbb_set_bit(7, 6, in->bss_color, &usig1);
		halbb_set_bit(13, 7, in->txop, &usig1);
		halbb_set_bit(20, 5, 0x1f, &usig1);
		halbb_set_bit(25, 1, 1, &usig1);
		/*=== U-SIG-2 ===*/
		halbb_set_bit(0, 2, out_plcp->ppdu_type_comp_mode, &usig2);
		halbb_set_bit(2, 1, 1, &usig2);
		halbb_set_bit(3, 5, out_plcp->punc_ch_info, &usig2);
		halbb_set_bit(8, 1, 1, &usig2);
		halbb_set_bit(9, 2, eht_sig_mcs, &usig2);
		halbb_set_bit(11, 5, out_plcp->n_sym_ehtsig, &usig2);
		//CRC4//
		//--- Set U-SIG-1 ---
		for(i = 0; i < 26; i++)
			siga_bits[i] = (usig1 >> i) & 0x1 ;
		//--- Set U-SIG-2 ---
		for(i = 0; i < 16; i++)
			siga_bits[i + 26] = (usig2 >> i) & 0x1 ;
		crc8_out = halbb_set_crc8_7(bb, siga_bits, 42);
		crc4_out = crc8_out & 0xf;
		halbb_set_bit(16, 4, crc4_out, &usig2);
		halbb_set_bit(20, 6, 0, &usig2);
	} else if (in->ppdu_type >= EHT_TB_FMT) {
		/*=== U-SIG-1 ===*/
		halbb_set_bit(0, 3, 0, &usig1);
		halbb_set_bit(3, 3, in->dbw, &usig1);
		halbb_set_bit(6, 1, in->ul_flag, &usig1);
		halbb_set_bit(7, 6, in->bss_color, &usig1);
		halbb_set_bit(13, 7, in->txop, &usig1);
		halbb_set_bit(20, 6, 0, &usig1);
		/*=== U-SIG-2 ===*/
		halbb_set_bit(0, 2, out_plcp->ppdu_type_comp_mode, &usig2);
		halbb_set_bit(2, 1, 0, &usig2);
		halbb_set_bit(3, 4, in->ul_srp1, &usig2);
		halbb_set_bit(7, 4, in->ul_srp2, &usig2);
		halbb_set_bit(11, 5, 0, &usig2);
		//CRC4//
		//--- Set U-SIG-1 ---
		for(i = 0; i < 26; i++)
			siga_bits[i] = (usig1 >> i) & 0x1 ;
		//--- Set U-SIG-2 ---
		for(i = 0; i < 16; i++)
			siga_bits[i + 26] = (usig2 >> i) & 0x1 ;
		crc8_out = halbb_set_crc8_7(bb, siga_bits, 42);
		crc4_out = crc8_out & 0xf;
		halbb_set_bit(16, 4, crc4_out, &usig2);
		halbb_set_bit(20, 6, 0, &usig2);
	} else if (in->ppdu_type == EHT_MU_ERSU_FMT) {
		/*=== U-SIG-1 ===*/
		halbb_set_bit(0, 3, 0, &usig1);
		halbb_set_bit(3, 3, in->dbw, &usig1);
		halbb_set_bit(6, 1, in->ul_flag, &usig1);
		halbb_set_bit(7, 6, in->bss_color, &usig1);
		halbb_set_bit(13, 7, in->txop, &usig1);
		halbb_set_bit(20, 6, 0x3f, &usig1);
		/*=== U-SIG-2 ===*/
		halbb_set_bit(0, 16, 0xffff, &usig2);
		halbb_set_bit(2, 1, 0, &usig2);
		halbb_set_bit(3, 4, in->ul_srp1, &usig2);
		halbb_set_bit(7, 4, in->ul_srp2, &usig2);
		halbb_set_bit(11, 5, 0, &usig2);
		//CRC4//
		//--- Set U-SIG-1 ---
		for(i = 0; i < 26; i++)
			siga_bits[i] = (usig1 >> i) & 0x1 ;
		//--- Set U-SIG-2 ---
		for(i = 0; i < 16; i++)
			siga_bits[i + 26] = (usig2 >> i) & 0x1 ;
		crc8_out = halbb_set_crc8_7(bb, siga_bits, 42);
		crc4_out = crc8_out & 0xf;
		halbb_set_bit(16, 4, crc4_out, &usig2);
		halbb_set_bit(20, 6, 0, &usig2);
	}
	usig1_val = ((usig2 & 0x3f) << 26)| usig1;
	usig2_val = usig2 >> 6;
	/*=== Write CR ===*/
	halbb_set_reg_cmn(bb, cr->usig_0, cr->usig_0_m, usig1_val, phy_idx);
	halbb_set_reg_cmn(bb, cr->usig_1, cr->usig_1_m, usig2_val, phy_idx);
}

void halbb_cfg_txinfo_7(struct bb_info *bb, struct halbb_plcp_info *in,
		      struct plcp_tx_pre_fec_padding_setting_out_t *out_plcp,
	     	      enum phl_phy_idx phy_idx)
{
	u32 ch20_with_data = 0;
	u8 i = 0;

	struct bb_plcp_cr_info *cr = &bb->bb_plcp_i.bb_plcp_cr_i;

	BB_DBG(bb, DBG_BIT14, "<====== %s ======>\n", __func__);

	halbb_set_reg_cmn(bb, cr->cfo_comp, cr->cfo_comp_m, 7, phy_idx);
	halbb_set_reg_cmn(bb, cr->obw_cts2self_dup_type, cr->obw_cts2self_dup_type_m, 0, phy_idx);
	halbb_set_reg_cmn(bb, cr->txcmd_txtp, cr->txcmd_txtp_m, 0, phy_idx);
	halbb_set_reg_cmn(bb, cr->ul_cqi_rpt_tri, cr->ul_cqi_rpt_tri_m, 0, phy_idx);
	halbb_set_reg_cmn(bb, cr->rf_fixed_gain_en, cr->rf_fixed_gain_en_m, 0, phy_idx);
	halbb_set_reg_cmn(bb, cr->rf_gain_idx, cr->rf_gain_idx_m, 0, phy_idx);
	halbb_set_reg_cmn(bb, cr->cca_pw_th_en, cr->cca_pw_th_en_m, 0, phy_idx);
	halbb_set_reg_cmn(bb, cr->cca_pw_th, cr->cca_pw_th_m, 0, phy_idx);
	halbb_set_reg_cmn(bb, cr->ant_sel_a, cr->ant_sel_a_m, 0, phy_idx);
	halbb_set_reg_cmn(bb, cr->ant_sel_b, cr->ant_sel_b_m, 0, phy_idx);
	halbb_set_reg_cmn(bb, cr->ant_sel_c, cr->ant_sel_c_m, 0, phy_idx);
	halbb_set_reg_cmn(bb, cr->ant_sel_d, cr->ant_sel_d_m, 0, phy_idx);
	halbb_set_reg_cmn(bb, cr->dbw_idx, cr->dbw_idx_m, in->dbw, phy_idx);
	halbb_set_reg_cmn(bb, cr->txsb, cr->txsb_m, in->txsb, phy_idx);
	halbb_set_reg_cmn(bb, cr->source_gen_mode_idx, cr->source_gen_mode_idx_m, in->source_gen_mode, phy_idx);
	halbb_set_reg_cmn(bb, cr->tssi_ru_size, cr->tssi_ru_size_m, out_plcp->usr[0].ru_size, phy_idx);

	ch20_with_data = halbb_cfg_ch20_with_data_7(bb, in);
	halbb_set_reg_cmn(bb, cr->ch20_with_data, cr->ch20_with_data_m, ch20_with_data, phy_idx);

	if (in->ppdu_type == B_MODE_FMT)
		halbb_set_reg_cmn(bb, cr->n_usr, cr->n_usr_m, in->n_user, phy_idx);
	else
		halbb_set_reg_cmn(bb, cr->n_usr, cr->n_usr_m, out_plcp->n_usr, phy_idx);
}

void halbb_cfg_txctrl_7(struct bb_info *bb, struct halbb_plcp_info *in,
		      struct plcp_tx_pre_fec_padding_setting_out_t *out_plcp,
	     	      enum phl_phy_idx phy_idx)//Add random value
{
	u8 i = 0;

	struct bb_plcp_cr_info *cr = &bb->bb_plcp_i.bb_plcp_cr_i;

	u32 pw_boost_fac[4] = {cr->usr0_pw_boost_fctr_db, cr->usr1_pw_boost_fctr_db,
						   cr->usr2_pw_boost_fctr_db, cr->usr3_pw_boost_fctr_db};
	u32 pw_boost_fac_m[4] = {cr->usr0_pw_boost_fctr_db_m ,cr->usr1_pw_boost_fctr_db_m,
						     cr->usr2_pw_boost_fctr_db_m, cr->usr3_pw_boost_fctr_db_m};
	u32 dcm_en[4] = {cr->usr0_dcm_en, cr->usr1_dcm_en, cr->usr2_dcm_en,
					 cr->usr3_dcm_en};
	u32 dcm_en_m[4] = {cr->usr0_dcm_en_m, cr->usr1_dcm_en_m, cr->usr2_dcm_en_m,
					   cr->usr3_dcm_en_m};
  	u32 mcs[4] = {cr->usr0_mcs, cr->usr1_mcs, cr->usr2_mcs, cr->usr3_mcs};
	u32 mcs_m[4] = {cr->usr0_mcs_m, cr->usr1_mcs_m, cr->usr2_mcs_m, cr->usr3_mcs_m};
	u32 fec[4] = {cr->usr0_fec_type, cr->usr1_fec_type, cr->usr2_fec_type, cr->usr3_fec_type};
	u32 fec_m[4] = {cr->usr0_fec_type_m, cr->usr1_fec_type_m, cr->usr2_fec_type_m, cr->usr3_fec_type_m};
	u32 n_sts[4] = {cr->usr0_n_sts, cr->usr1_n_sts, cr->usr2_n_sts, cr->usr3_n_sts};
	u32 n_sts_m[4] = {cr->usr0_n_sts_m, cr->usr1_n_sts_m, cr->usr2_n_sts_m, cr->usr3_n_sts_m};
	u32 n_sts_ru_tot[4] = {cr->usr0_n_sts_ru_tot, cr->usr1_n_sts_ru_tot,
						   cr->usr2_n_sts_ru_tot, cr->usr3_n_sts_ru_tot};
	u32 n_sts_ru_tot_m[4] = {cr->usr0_n_sts_ru_tot_m, cr->usr1_n_sts_ru_tot_m,
							 cr->usr2_n_sts_ru_tot_m, cr->usr3_n_sts_ru_tot_m};
	u32 precoding_mode_idx[4] = {cr->usr0_precoding_mode_idx, cr->usr1_precoding_mode_idx,
				     cr->usr2_precoding_mode_idx, cr->usr3_precoding_mode_idx};
	u32 precoding_mode_idx_m[4] = {cr->usr0_precoding_mode_idx_m, cr->usr1_precoding_mode_idx_m,
				       cr->usr2_precoding_mode_idx_m, cr->usr3_precoding_mode_idx_m};
	u32 csi_buf_id[4] = {cr->usr0_csi_buf_id, cr->usr1_csi_buf_id, cr->usr2_csi_buf_id,
						 cr->usr3_csi_buf_id};
	u32 csi_buf_id_m[4] = {cr->usr0_csi_buf_id_m, cr->usr1_csi_buf_id_m,
						   cr->usr2_csi_buf_id_m, cr->usr3_csi_buf_id_m};
	u32 strt_sts[4] = {cr->usr0_strt_sts, cr->usr1_strt_sts, cr->usr2_strt_sts,
					   cr->usr3_strt_sts};
	u32 strt_sts_m[4] = {cr->usr0_strt_sts_m, cr->usr1_strt_sts_m, cr->usr2_strt_sts_m,
					     cr->usr3_strt_sts_m};
	u32 ru_id[4] = {cr->usr0_ru_id, cr->usr1_ru_id, cr->usr2_ru_id, cr->usr3_ru_id};
	u32 ru_id_m[4] = {cr->usr0_ru_id_m, cr->usr1_ru_id_m, cr->usr2_ru_id_m, cr->usr3_ru_id_m};
	u32 ru_size[4] = {cr->usr0_ru_size, cr->usr1_ru_size, cr->usr2_ru_size, cr->usr3_ru_size};
	u32 ru_size_m[4] = {cr->usr0_ru_size_m, cr->usr1_ru_size_m, cr->usr2_ru_size_m, cr->usr3_ru_size_m};

	BB_DBG(bb, DBG_BIT14, "<====== %s ======>\n", __func__);

	// 	Default value //
	//	When HE_TB NDP, it's valid; o.w., it's RSVD and set to 1'b0
	halbb_set_reg_cmn(bb, cr->feedback_status, cr->feedback_status_m, 0, phy_idx);
	//	it's RSVD except HE PPDU and set to 1'b0 when it's RSVD  0: disable MU-MIMO-LTF-Mode, 1: enable MU-MIMO-LTF-Mode
	halbb_set_reg_cmn(bb, cr->mumimo_ltf_mode_en, cr->mumimo_ltf_mode_en_m, 0, phy_idx);

	// 	usr value //
	// U_ID
	halbb_set_reg_cmn(bb, cr->usr0_u_id, cr->usr0_u_id_m, 0, phy_idx);
	halbb_set_reg_cmn(bb, cr->usr1_u_id, cr->usr1_u_id_m, 1, phy_idx);
	halbb_set_reg_cmn(bb, cr->usr2_u_id, cr->usr2_u_id_m, 2, phy_idx);
	halbb_set_reg_cmn(bb, cr->usr3_u_id, cr->usr3_u_id_m, 3, phy_idx);

	//  Input Interface //
	//	When HE_MU, whether to apply DCM is HE-SIGB or not; o.w., it's RSVD and set to 1'b0
	if (in->ppdu_type != HE_MU_FMT)
		halbb_set_reg_cmn(bb, cr->he_sigb_dcm_en, cr->he_sigb_dcm_en_m, 0, phy_idx);
	else
		halbb_set_reg_cmn(bb, cr->he_sigb_dcm_en, cr->he_sigb_dcm_en_m, in->he_dcm_sigb, phy_idx);//0: disable, 1:enable

	//	HE_SIGB / EHT_SIG MCS
	if (in->ppdu_type == HE_MU_FMT)
		halbb_set_reg_cmn(bb, cr->he_sigb_ehtsig_mcs, cr->he_sigb_ehtsig_mcs_m, in->he_mcs_sigb, phy_idx);
	else if (in->ppdu_type >= EHT_MU_SU_FMT)
		halbb_set_reg_cmn(bb, cr->he_sigb_ehtsig_mcs, cr->he_sigb_ehtsig_mcs_m, in->eht_mcs_sig, phy_idx);
	else
		halbb_set_reg_cmn(bb, cr->he_sigb_ehtsig_mcs, cr->he_sigb_ehtsig_mcs_m, 0, phy_idx);
	//	When HE_SU or HE_ER_SU, it means whether to apply beam_change or not; o.w. it's RSVD and set to 1'b1 for OFDM and set to 1'b0 for b_mode.
	if (in->ppdu_type == B_MODE_FMT)
		halbb_set_reg_cmn(bb, cr->beam_change_en, cr->beam_change_en_m, 0, phy_idx);
	else
		halbb_set_reg_cmn(bb, cr->beam_change_en, cr->beam_change_en_m, in->beamchange_en, phy_idx);

	//	The number of LTF. The definition is on the right-hand side. it's RSVD when b_mode and Legacy. When it's RSVD, it shall be set to 3'b0.
	if (in->ppdu_type == B_MODE_FMT || in->ppdu_type == LEGACY_FMT)
		halbb_set_reg_cmn(bb, cr->n_ltf, cr->n_ltf_m, 0, phy_idx);
	else
		halbb_set_reg_cmn(bb, cr->n_ltf, cr->n_ltf_m, out_plcp->n_ltf, phy_idx);

	//	0: LTF_type1x, 1: LTF_type2x, 2: LTF_type4x, 3:RSVD
	if (in->ppdu_type < HE_SU_FMT)
		halbb_set_reg_cmn(bb, cr->ltf_type, cr->ltf_type_m, 0, phy_idx);
	else
		halbb_set_reg_cmn(bb, cr->ltf_type, cr->ltf_type_m, in->he_ltf_type, phy_idx);

	//	0: GI_0p4us, 1: GI_0p8us, 2:GI_1p6us, 3:GI_3p2us
	if (in->ppdu_type == B_MODE_FMT)
		halbb_set_reg_cmn(bb, cr->gi_type, cr->gi_type_m, 0, phy_idx);
	else
		halbb_set_reg_cmn(bb, cr->gi_type, cr->gi_type_m, out_plcp->gi, phy_idx);

	//	it's RSVD except HE PPDU when Doppler=enable. When it's RSVD, it shall be set to 1'b0
	if (!((in->ppdu_type > VHT_FMT) && out_plcp->doppler_en))
		halbb_set_reg_cmn(bb, cr->midamble_mode, cr->midamble_mode_m, 0, phy_idx);
	else
		halbb_set_reg_cmn(bb, cr->midamble_mode, cr->midamble_mode_m, out_plcp->midamble, phy_idx);

	//	it's RSVD expect HE PPDU. It shall be set to 1'b0
	if (in->ppdu_type < HE_SU_FMT)
		halbb_set_reg_cmn(bb, cr->doppler_en, cr->doppler_en_m, 0, phy_idx);
	else
		halbb_set_reg_cmn(bb, cr->doppler_en, cr->doppler_en_m, out_plcp->doppler_en, phy_idx);

	//	It's RSVD when b_mode and Legacy, and shall be set to 1'b0. For 8852A, STBC only support NSS * 2 = NSTS
	if (in->ppdu_type == B_MODE_FMT || in->ppdu_type == LEGACY_FMT)
		halbb_set_reg_cmn(bb, cr->stbc_en, cr->stbc_en_m, 0, phy_idx);
	else
		halbb_set_reg_cmn(bb, cr->stbc_en, cr->stbc_en_m, out_plcp->stbc, phy_idx);

	// 	usr0 value //
	//	The power boost factor applied in corresponding RU in pwr. S(5,2)
	// Initialize
	for (i = 0; i < 4; i++) {
		halbb_set_reg_cmn(bb, pw_boost_fac[i], pw_boost_fac_m[i], 0, phy_idx);
		halbb_set_reg_cmn(bb, dcm_en[i], dcm_en_m[i], 0, phy_idx);
		halbb_set_reg_cmn(bb, mcs[i], mcs_m[i], 0, phy_idx);
		halbb_set_reg_cmn(bb, fec[i], fec_m[i], 0, phy_idx);
		halbb_set_reg_cmn(bb, n_sts[i], n_sts_m[i], 0, phy_idx);
		halbb_set_reg_cmn(bb, n_sts_ru_tot[i], n_sts_ru_tot_m[i], 0, phy_idx);
		// Txbf
		halbb_set_reg_cmn(bb, precoding_mode_idx[i], precoding_mode_idx_m[i], 0, phy_idx);
		// CSI buf_id
		halbb_set_reg_cmn(bb, csi_buf_id[i], csi_buf_id_m[i], 0, phy_idx);
		// Strt sts
		halbb_set_reg_cmn(bb, strt_sts[i], strt_sts_m[i], 0, phy_idx);
	}

	for (i = 0; i < in->n_user; i++) {
		halbb_set_reg_cmn(bb, pw_boost_fac[i], pw_boost_fac_m[i], in->usr[i].pwr_boost_db, phy_idx);

		//	Whether the user applies DCM; it's RSVD when STBC or MU-MIMO
		if (!out_plcp->stbc) // if (!STBC)
			halbb_set_reg_cmn(bb, dcm_en[i], dcm_en_m[i], out_plcp->usr[i].dcm, phy_idx);

		//	The modulation and coding scheme applied to the user.For 8852A, HT(0~31), VHT/HE(0~11), OFDM(0~8), bmode(0~3); otherwise, it's RSVD
		halbb_set_reg_cmn(bb, mcs[i], mcs_m[i], in->usr[i].mcs, phy_idx);

		//	0: BCC, 1:LDPC
		if (in->ppdu_type == B_MODE_FMT)
			halbb_set_reg_cmn(bb, fec[i], fec_m[i], 0, phy_idx);
		else
			halbb_set_reg_cmn(bb, fec[i], fec_m[i], out_plcp->usr[i].fec, phy_idx);

		//	The number of space-time-stream
		halbb_set_reg_cmn(bb, n_sts[i], n_sts_m[i], out_plcp->usr[i].nsts - 1, phy_idx);

		//	N_STS_RU_total - 1
		halbb_set_reg_cmn(bb, n_sts_ru_tot[i], n_sts_ru_tot_m[i], out_plcp->usr[i].nsts - 1, phy_idx);

		//	ru_id / ru_size
		halbb_set_reg_cmn(bb, ru_id[i], ru_id_m[i], out_plcp->usr[i].ru_idx, phy_idx);
		halbb_set_reg_cmn(bb, ru_size[i], ru_size_m[i], out_plcp->usr[i].ru_size, phy_idx);
	}

	//	it's RSVD except HE PPDU and shall be set to 2'b0
	if (in->ppdu_type < HE_SU_FMT)
		halbb_set_reg_cmn(bb, cr->pre_fec_fctr, cr->pre_fec_fctr_m, 0, phy_idx);
	else
		halbb_set_reg_cmn(bb, cr->pre_fec_fctr, cr->pre_fec_fctr_m, out_plcp->pre_fec_padding_factor, phy_idx);

	//	it's RSVD except HE-PPDU and shall be set to 2'b0. it means the duration for packet extension field
	if (in->ppdu_type < HE_SU_FMT)
		halbb_set_reg_cmn(bb, cr->pkt_ext_idx, cr->pkt_ext_idx_m, 0, phy_idx);
	else
		halbb_set_reg_cmn(bb, cr->pkt_ext_idx, cr->pkt_ext_idx_m, out_plcp->t_pe, phy_idx);

	//	0: without LDPC extra, 1: with LDPC extra
	halbb_set_reg_cmn(bb, cr->ldpc_extr, cr->ldpc_extr_m, out_plcp->ldpc_extra, phy_idx);

	//	The number of data symbols in HE-SIGB/EHT-SIG.
	if (in->ppdu_type == HE_MU_FMT)
		halbb_set_reg_cmn(bb, cr->n_sym_hesigb_ehtsig, cr->n_sym_hesigb_ehtsig_m, out_plcp->n_sym_hesigb, phy_idx);
	else if (in->ppdu_type >= EHT_MU_SU_FMT)
		halbb_set_reg_cmn(bb, cr->n_sym_hesigb_ehtsig, cr->n_sym_hesigb_ehtsig_m, out_plcp->n_sym_ehtsig, phy_idx);
	else
		halbb_set_reg_cmn(bb, cr->n_sym_hesigb_ehtsig, cr->n_sym_hesigb_ehtsig_m, 0, phy_idx);
	//	It means the number of data symbols in data_field, which the number of midamble symbols are excluded
	halbb_set_reg_cmn(bb, cr->n_sym, cr->n_sym_m, out_plcp->n_sym, phy_idx);
}

void halbb_plcp_delimiter_7(struct bb_info *bb, struct halbb_plcp_info *in,
			  struct plcp_tx_pre_fec_padding_setting_out_t *out_plcp,
	     		  enum phl_phy_idx phy_idx) //Add random value
{
	u8 crc8_out = 0;
	u16 tmp = 0;
	u32 delimiter = 0;
	unsigned char delimiter_crc[32] = {0};
	u8 i = 0;
	u8 j = 0;

	struct bb_plcp_cr_info *cr = &bb->bb_plcp_i.bb_plcp_cr_i;

	u32 delmter[4] = {cr->usr0_delmter, cr->usr1_delmter, cr->usr2_delmter,
					  cr->usr3_delmter};
	u32 delmter_m[4] = {cr->usr0_delmter_m, cr->usr1_delmter_m,
						cr->usr2_delmter_m, cr->usr3_delmter_m};
	u32 mpdu_len[4] = {cr->usr0_mdpu_len_byte, cr->usr1_mdpu_len_byte,
					   cr->usr2_mdpu_len_byte, cr->usr3_mdpu_len_byte};
	u32 mpdu_len_m[4] = {cr->usr0_mdpu_len_byte_m, cr->usr1_mdpu_len_byte_m,
					     cr->usr2_mdpu_len_byte_m, cr->usr3_mdpu_len_byte_m};
	u32 n_mpdu[4] = {cr->usr0_n_mpdu, cr->usr1_n_mpdu, cr->usr2_n_mpdu,
					 cr->usr3_n_mpdu};
	u32 n_mpdu_m[4] = {cr->usr0_n_mpdu_m, cr->usr1_n_mpdu_m, cr->usr2_n_mpdu_m,
					   cr->usr3_n_mpdu_m};
	u32 eof_padding_len[4] = {cr->usr0_eof_padding_len, cr->usr1_eof_padding_len,
							  cr->usr2_eof_padding_len, cr->usr3_eof_padding_len};
	u32 eof_padding_len_m[4] = {cr->usr0_eof_padding_len_m, cr->usr1_eof_padding_len_m,
							    cr->usr2_eof_padding_len_m, cr->usr3_eof_padding_len_m};
 	u32 init_seed[4] = {cr->usr0_init_seed, cr->usr1_init_seed,
						cr->usr2_init_seed, cr->usr3_init_seed};
	u32 init_seed_m[4] = {cr->usr0_init_seed_m, cr->usr1_init_seed_m,
						  cr->usr2_init_seed_m, cr->usr3_init_seed_m};

	BB_DBG(bb, DBG_BIT14, "<====== %s ======>\n", __func__);

	// Initialize
	for (i = 0; i < 4; i++) {
		halbb_set_reg_cmn(bb, delmter[i], delmter_m[i], 0, phy_idx);
		halbb_set_reg_cmn(bb, mpdu_len[i], mpdu_len_m[i], 0, phy_idx);
		halbb_set_reg_cmn(bb, n_mpdu[i], n_mpdu_m[i], 0, phy_idx);
		halbb_set_reg_cmn(bb, eof_padding_len[i], eof_padding_len_m[i], 0, phy_idx);
		halbb_set_reg_cmn(bb, init_seed[i], init_seed_m[i], 0, phy_idx);
	}

	for (i = 0; i < in->n_user; i++) {
		//=== [Delimiter] ===//
		if (out_plcp->usr[i].n_mpdu == 1)
			halbb_set_bit(0, 1, 1, &delimiter);
		else
			halbb_set_bit(0, 1, 0, &delimiter);
		halbb_set_bit(1, 1, 0, &delimiter); //rsvd
		halbb_set_bit(2, 2, out_plcp->usr[i].mpdu_length_byte >> 12, &delimiter);
		tmp = out_plcp->usr[i].mpdu_length_byte & 0xfff;
		halbb_set_bit(4, 12, tmp, &delimiter);
		//CRC8//
		//--- Set Delimiter ---
		for(j = 0; j < 16; j++)
			delimiter_crc[j] = ( delimiter >> j ) & 0x1 ;
		crc8_out = halbb_set_crc8_7(bb, delimiter_crc, 16);
		halbb_set_bit(16, 8, crc8_out, &delimiter);
		halbb_set_bit(24, 8, 0x4e, &delimiter); // MSB [01001110] LSB
		/*=== Write CR ===*/
		halbb_set_reg_cmn(bb, delmter[i], delmter_m[i], delimiter, phy_idx);
		//=== [MPDU Length] ===//
		halbb_set_reg_cmn(bb, mpdu_len[i], mpdu_len_m[i], out_plcp->usr[i].mpdu_length_byte, phy_idx);
		//=== [N_MPDU] ===//
		halbb_set_reg_cmn(bb, n_mpdu[i], n_mpdu_m[i], out_plcp->usr[i].n_mpdu, phy_idx);
		//=== [EOF Padding Length] ===//
		halbb_set_reg_cmn(bb, eof_padding_len[i], eof_padding_len_m[i], out_plcp->usr[i].eof_padding_length * 8, phy_idx);
		//=== [Init seed] ===//
		halbb_set_reg_cmn(bb, init_seed[i], init_seed_m[i], in->usr[i].random_init_seed, phy_idx);
	}
}

void halbb_cfg_cck_7(struct bb_info *bb, struct halbb_plcp_info *in, enum phl_phy_idx phy_idx)
{
	struct bb_plcp_cr_info *cr = &bb->bb_plcp_i.bb_plcp_cr_i;

	// === 11b_tx_pmac_psdu_byte === //
	halbb_set_reg_cmn(bb, cr->usr0_mdpu_len_byte, cr->usr0_mdpu_len_byte_m, in->usr[0].apep, phy_idx);
	// === 11b_tx_pmac_psdu_rate === //
	halbb_set_reg(bb, cr->b_rate_idx, cr->b_rate_idx_m, in->usr[0].mcs);
	// === 11b_tx_pmac_service_bit2 === //
	halbb_set_reg(bb, cr->b_locked_clk_en, cr->b_locked_clk_en_m, 1);
	// === 11b_tx_pmac_carrier_suppress_tx === //
	halbb_set_reg(bb, cr->b_carrier_suppress_tx, cr->b_carrier_suppress_tx_m, 0);
}

void halbb_vht_sigb_7(struct bb_info *bb, struct halbb_plcp_info *in,
		    struct plcp_tx_pre_fec_padding_setting_out_t *out_plcp,
	     	    enum phl_phy_idx phy_idx)
{
	// VHT SU
	u8 crc8_out = 0;
	u8 scrambler_seed = 0;
	u32 vht_sigb = 0;
	unsigned char sigb[32] = {0};
	u8 i = 0;

	struct bb_plcp_cr_info *cr = &bb->bb_plcp_i.bb_plcp_cr_i;

	u32 vht_sigb_cr[4] = {cr->vht_sigb0, cr->vht_sigb1, cr->vht_sigb2,
						  cr->vht_sigb3};
	u32 vht_sigb_cr_m[4] = {cr->vht_sigb0_m, cr->vht_sigb1_m, cr->vht_sigb2_m,
							cr->vht_sigb3_m};
	u32 service[4] = {cr->usr0_service, cr->usr1_service, cr->usr2_service,
					  cr->usr3_service};
	u32 service_m[4] = {cr->usr0_service_m, cr->usr1_service_m,
						cr->usr2_service_m,	cr->usr3_service_m};

	// Initialize
	for (i = 0; i < 4; i++) {
		halbb_set_reg_cmn(bb, vht_sigb_cr[i], vht_sigb_cr_m[i], 0, phy_idx);
		halbb_set_reg_cmn(bb, service[i], service_m[i], 0, phy_idx);
	}
	switch (in->dbw) {//0:BW20, 1:BW40, 2:BW80, 3:BW160/BW80+80
		case 0:
			halbb_set_bit(0, 17, halbb_ceil(out_plcp->usr[0].apep_len, 4), &vht_sigb);
			halbb_set_bit(17, 3, 0x7, &vht_sigb);
			//--- Set VHT SigB ---
			for(i = 0; i < 20; i++)
				sigb[i] = ( vht_sigb >> i ) & 0x1 ;
			crc8_out = halbb_set_crc8_7(bb, sigb, 20);
			halbb_set_bit(20, 6, 0x0, &vht_sigb);
			break;
		case 1:
			halbb_set_bit(0, 19, halbb_ceil(out_plcp->usr[0].apep_len, 4), &vht_sigb);
			halbb_set_bit(19, 2, 0x3, &vht_sigb);
			//--- Set VHT SigB ---
			for(i = 0; i < 21; i++)
				sigb[i] = ( vht_sigb >> i ) & 0x1 ;
			crc8_out = halbb_set_crc8_7(bb, sigb, 21);
			halbb_set_bit(21, 6, 0x0, &vht_sigb);
			break;
		case 2:
		case 3:
			halbb_set_bit(0, 21, halbb_ceil(out_plcp->usr[0].apep_len, 4), &vht_sigb);
			halbb_set_bit(21, 2, 0x3, &vht_sigb);
			//--- Set VHT SigB ---
			for(i = 0; i < 23; i++)
				sigb[i] = ( vht_sigb >> i ) & 0x1 ;
			crc8_out = halbb_set_crc8_7(bb, sigb, 23);
			halbb_set_bit(23, 6, 0x0, &vht_sigb);
			break;
		default:
			break;
	}
	//=== [Service] ===//
	scrambler_seed = in->usr[0].scrambler_seed & 0x7f;

	halbb_set_reg_cmn(bb, service[0], service_m[0], (crc8_out << 8) + scrambler_seed, phy_idx);
	halbb_set_reg_cmn(bb, vht_sigb_cr[0], vht_sigb_cr_m[0], vht_sigb, phy_idx);
}

void halbb_service_7(struct bb_info *bb, struct halbb_plcp_info *in,
	     	   enum phl_phy_idx phy_idx)
{
	u8 i = 0;
	u32 scrambler_seed[4] = {0};

	struct bb_plcp_cr_info *cr = &bb->bb_plcp_i.bb_plcp_cr_i;

	u32 service[4] = {cr->usr0_service, cr->usr1_service, cr->usr2_service,
					  cr->usr3_service};
	u32 service_m[4] = {cr->usr0_service_m, cr->usr1_service_m,
						cr->usr2_service_m,	cr->usr3_service_m};

	for (i = 0; i < 4; i++)
		halbb_set_reg_cmn(bb, service[i], service_m[i], 0, phy_idx);
	for(i = 0; i < in->n_user; i++) {
		//=== [Service] ===//
		if (in->ppdu_type < EHT_MU_SU_FMT)
			scrambler_seed[i] = in->usr[i].scrambler_seed & 0x7f;
		else
			scrambler_seed[i] = in->usr[i].scrambler_seed & 0x7ff;
		halbb_set_reg_cmn(bb, service[i], service_m[i], scrambler_seed[i], phy_idx);
	}
}

#if 0
void halbb_show_input(struct bb_info *bb, struct halbb_plcp_info *in)
{
	u8 user_num = 0;

	BB_DBG(bb, DBG_BIT14, "<====== %s ======>\n", __func__);
	BB_DBG(bb, DBG_BIT14, "source_gen_mode=%d\n", in->source_gen_mode);
	BB_DBG(bb, DBG_BIT14, "locked_clk=%d\n", in->locked_clk);
	BB_DBG(bb, DBG_BIT14, "dyn_bw=%d\n", in->dyn_bw);
	BB_DBG(bb, DBG_BIT14, "ndp_en=%d\n", in->ndp_en);
	BB_DBG(bb, DBG_BIT14, "long_preamble_en=%d\n", in->long_preamble_en);
	BB_DBG(bb, DBG_BIT14, "stbc=%d\n", in->stbc);
	BB_DBG(bb, DBG_BIT14, "gi=%d\n", in->gi);
	BB_DBG(bb, DBG_BIT14, "tb_l_len=%d\n", in->tb_l_len);
	BB_DBG(bb, DBG_BIT14, "tb_ru_tot_sts_max=%d\n", in->tb_ru_tot_sts_max);
	BB_DBG(bb, DBG_BIT14, "vht_txop_not_allowed=%d\n", in->vht_txop_not_allowed);
	BB_DBG(bb, DBG_BIT14, "tb_disam=%d\n", in->tb_disam);
	BB_DBG(bb, DBG_BIT14, "doppler=%d\n", in->doppler);
	BB_DBG(bb, DBG_BIT14, "he_ltf_type=%d\n", in->he_ltf_type);
	BB_DBG(bb, DBG_BIT14, "ht_l_len=%d\n", in->ht_l_len);
	BB_DBG(bb, DBG_BIT14, "preamble_puncture=%d\n", in->preamble_puncture);
	BB_DBG(bb, DBG_BIT14, "he_mcs_sigb=%d\n", in->he_mcs_sigb);
	BB_DBG(bb, DBG_BIT14, "he_dcm_sigb=%d\n", in->he_dcm_sigb);
	BB_DBG(bb, DBG_BIT14, "he_sigb_compress_en=%d\n", in->he_sigb_compress_en);
	BB_DBG(bb, DBG_BIT14, "max_tx_time_0p4us=%d\n", in->max_tx_time_0p4us);
	BB_DBG(bb, DBG_BIT14, "ul_flag=%d\n", in->ul_flag);
	BB_DBG(bb, DBG_BIT14, "tb_ldpc_extra=%d\n", in->tb_ldpc_extra);
	BB_DBG(bb, DBG_BIT14, "bss_color=%d\n", in->bss_color);
	BB_DBG(bb, DBG_BIT14, "sr=%d\n", in->sr);
	BB_DBG(bb, DBG_BIT14, "beamchange_en=%d\n", in->beamchange_en);
	BB_DBG(bb, DBG_BIT14, "he_er_u106ru_en=%d\n", in->he_er_u106ru_en);
	BB_DBG(bb, DBG_BIT14, "ul_srp1=%d\n", in->ul_srp1);
	BB_DBG(bb, DBG_BIT14, "ul_srp2=%d\n", in->ul_srp2);
	BB_DBG(bb, DBG_BIT14, "ul_srp3=%d\n", in->ul_srp3);
	BB_DBG(bb, DBG_BIT14, "ul_srp4=%d\n", in->ul_srp4);
	BB_DBG(bb, DBG_BIT14, "mode=%d\n", in->mode);
	BB_DBG(bb, DBG_BIT14, "group_id=%d\n", in->group_id);
	BB_DBG(bb, DBG_BIT14, "ppdu_type=%d\n", in->ppdu_type);
	BB_DBG(bb, DBG_BIT14, "txop=%d\n", in->txop);
	BB_DBG(bb, DBG_BIT14, "tb_strt_sts=%d\n", in->tb_strt_sts);
	BB_DBG(bb, DBG_BIT14, "tb_pre_fec_padding_factor=%d\n", in->tb_pre_fec_padding_factor);
	BB_DBG(bb, DBG_BIT14, "cbw=%d\n", in->cbw);
	BB_DBG(bb, DBG_BIT14, "txsc=%d\n", in->txsc);
	BB_DBG(bb, DBG_BIT14, "tb_mumimo_mode_en=%d\n", in->tb_mumimo_mode_en);
	BB_DBG(bb, DBG_BIT14, "dbw=%d\n", in->dbw);
	BB_DBG(bb, DBG_BIT14, "nominal_t_pe=%d\n", in->nominal_t_pe);
	BB_DBG(bb, DBG_BIT14, "ness=%d\n", in->ness);
	BB_DBG(bb, DBG_BIT14, "n_user=%d\n", in->n_user);
	BB_DBG(bb, DBG_BIT14, "tb_rsvd=%d\n", in->tb_rsvd);
	BB_DBG(bb, DBG_BIT14, "punc_pattern=%d\n", in->punc_pattern);
	BB_DBG(bb, DBG_BIT14, "eht_mcs_sig=%d\n", in->eht_mcs_sig);
	BB_DBG(bb, DBG_BIT14, "txsb=%d\n", in->txsb);
	halbb_print_devider(bb, BB_DEVIDER_LEN_16, true, FRC_PRINT_LINE);
	for (user_num = 0; user_num < N_USER; user_num++) {
		BB_DBG(bb, DBG_BIT14, "[user%d]mcs=%d\n", user_num, in->usr[user_num].mcs);
		BB_DBG(bb, DBG_BIT14, "[user%d]mpdu_len=%d\n", user_num, in->usr[user_num].mpdu_len);
		BB_DBG(bb, DBG_BIT14, "[user%d]n_mpdu=%d\n", user_num, in->usr[user_num].n_mpdu);
		BB_DBG(bb, DBG_BIT14, "[user%d]fec=%d\n", user_num, in->usr[user_num].fec);
		BB_DBG(bb, DBG_BIT14, "[user%d]dcm=%d\n", user_num, in->usr[user_num].dcm);
		BB_DBG(bb, DBG_BIT14, "[user%d]aid=%d\n", user_num, in->usr[user_num].aid);
		BB_DBG(bb, DBG_BIT14, "[user%d]scrambler_seed=%d\n", user_num, in->usr[user_num].scrambler_seed);
		BB_DBG(bb, DBG_BIT14, "[user%d]random_init_seed=%d\n", user_num, in->usr[user_num].random_init_seed);
		BB_DBG(bb, DBG_BIT14, "[user%d]apep=%d\n", user_num, in->usr[user_num].apep);
		BB_DBG(bb, DBG_BIT14, "[user%d]ru_alloc=%d\n", user_num, in->usr[user_num].ru_alloc);
		BB_DBG(bb, DBG_BIT14, "[user%d]nss=%d\n", user_num, in->usr[user_num].nss);
		BB_DBG(bb, DBG_BIT14, "[user%d]txbf=%d\n", user_num, in->usr[user_num].txbf);
		BB_DBG(bb, DBG_BIT14, "[user%d]pwr_boost_db=%d\n", user_num, in->usr[user_num].pwr_boost_db);
		BB_DBG(bb, DBG_BIT14, "[user%d]ru_size=%d\n", user_num, in->usr[user_num].ru_size);
		BB_DBG(bb, DBG_BIT14, "[user%d]ru_idx=%d\n", user_num, in->usr[user_num].ru_idx);
		halbb_print_devider(bb, BB_DEVIDER_LEN_16, true, FRC_PRINT_LINE);
	}
}
#endif

enum plcp_sts halbb_plcp_gen_7(struct bb_info *bb, struct halbb_plcp_info *in,
		    struct usr_plcp_gen_in *user, enum phl_phy_idx phy_idx)
{
	struct bb_plcp_cr_info *cr = &bb->bb_plcp_i.bb_plcp_cr_i;
	enum plcp_sts tmp = PLCP_SUCCESS;
	struct plcp_tx_pre_fec_padding_setting_in_t in_plcp;
	struct plcp_tx_pre_fec_padding_setting_out_t out;
	//struct _bb_result he_result;

	halbb_mem_cpy(bb, in->usr, user, 4*sizeof(struct usr_plcp_gen_in));

	BB_DBG(bb, DBG_BIT14, "<====== %s ======>\n", __func__);

	halbb_plcp_gen_init_7(bb, in, &in_plcp);

#if 0
	halbb_show_input(bb, in);
#endif
	if (!halbb_ru_info_init_7(bb, in, &in_plcp, &out, phy_idx))
		return SPEC_INVALID;

	halbb_ppdu_var_type_cfg_7(bb, in, phy_idx);

	// CCK
	if (in->ppdu_type == B_MODE_FMT) {
		halbb_cfg_cck_7(bb, in, phy_idx);
		if ((in->usr[0].mcs == 0) && (in->long_preamble_en == 0))
			tmp = CCK_INVALID;
	} else {
		if (in->ppdu_type >= EHT_MU_SU_FMT) {
			// Calculate user specific field & EHT_SIG_Nsym (with incorrect common field para.)
			halbb_eht_sig_7(bb, in, &in_plcp, &out, phy_idx);
			// Calculate PLCP header with EHT_SIG_Nsym
			tmp = halbb_tx_plcp_cal(bb, &in_plcp, &out);
			// Overwrite common field & CRC with correct para. such as pre_fec_padding...
			halbb_eht_sig_7(bb, in, &in_plcp, &out, phy_idx);
		} else {
			// PLCP calculation
			tmp = halbb_tx_plcp_cal(bb, &in_plcp, &out);
		}
		// VHT SIG-B
		if (in->ppdu_type == VHT_FMT)
			halbb_vht_sigb_7(bb, in, &out, phy_idx);
		else
			halbb_service_7(bb, in, phy_idx);
		// L-SIG
		halbb_plcp_lsig_7(bb, in, &out, phy_idx);
		if (in->ppdu_type >= EHT_MU_SU_FMT)// U-SIG
			halbb_plcp_usig_7(bb, in, &out, phy_idx);
		else if (in->ppdu_type > LEGACY_FMT)// SIG-A
			halbb_plcp_siga_7(bb, in, &out, phy_idx);

		// Tx Ctrl Info
		halbb_cfg_txctrl_7(bb, in, &out, phy_idx);
		// Delimiter
		halbb_plcp_delimiter_7(bb, in, &out, phy_idx);
	}
	// Tx Info
	halbb_cfg_txinfo_7(bb, in, &out, phy_idx);

	return tmp;
}


void halbb_cr_cfg_plcp_init_7(struct bb_info *bb)
{
	struct bb_plcp_info *plcp_info = &bb->bb_plcp_i;
	struct bb_plcp_cr_info *cr = &plcp_info->bb_plcp_cr_i;

	switch (bb->cr_type) {

	#ifdef HALBB_COMPILE_BE0_SERIES
	case BB_BE0:
		cr->b_header_0 = R1B_TX_PMAC_HEADER_0_BE0;
		cr->b_header_0_m = R1B_TX_PMAC_HEADER_0_BE0_M;
		cr->b_header_1 = R1B_TX_PMAC_HEADER_1_BE0;
		cr->b_header_1_m = R1B_TX_PMAC_HEADER_1_BE0_M;
		cr->b_header_2 = R1B_TX_PMAC_HEADER_2_BE0;
		cr->b_header_2_m = R1B_TX_PMAC_HEADER_2_BE0_M;
		cr->b_header_3 = R1B_TX_PMAC_HEADER_3_BE0;
		cr->b_header_3_m = R1B_TX_PMAC_HEADER_3_BE0_M;
		cr->b_header_4 = R1B_TX_PMAC_HEADER_4_BE0;
		cr->b_header_4_m = R1B_TX_PMAC_HEADER_4_BE0_M;
		cr->b_header_5 = R1B_TX_PMAC_HEADER_5_BE0;
		cr->b_header_5_m = R1B_TX_PMAC_HEADER_5_BE0_M;
		cr->b_carrier_suppress_tx = R1B_TX_PMAC_CARRIER_SUPPRESS_TX_BE0;
		cr->b_carrier_suppress_tx_m = R1B_TX_PMAC_CARRIER_SUPPRESS_TX_BE0_M;
		cr->b_rate_idx = BMODE_RATE_IDX_BE0;
		cr->b_rate_idx_m = BMODE_RATE_IDX_BE0_M;
		cr->b_locked_clk_en = BMODE_LOCKED_CLK_EN_BE0;
		cr->b_locked_clk_en_m = BMODE_LOCKED_CLK_EN_BE0_M;
		// hesigb_ehtsig
		cr->hesigb_ehtsig_0	= TXD_HESIGB_EHTSIG_0_BE0;
		cr->hesigb_ehtsig_0_m   = TXD_HESIGB_EHTSIG_0_BE0_M;
		cr->hesigb_ehtsig_1     = TXD_HESIGB_EHTSIG_1_BE0;
		cr->hesigb_ehtsig_1_m   = TXD_HESIGB_EHTSIG_1_BE0_M;
		cr->hesigb_ehtsig_2     = TXD_HESIGB_EHTSIG_2_BE0;
		cr->hesigb_ehtsig_2_m   = TXD_HESIGB_EHTSIG_2_BE0_M;
		cr->hesigb_ehtsig_3     = TXD_HESIGB_EHTSIG_3_BE0;
		cr->hesigb_ehtsig_3_m   = TXD_HESIGB_EHTSIG_3_BE0_M;
		cr->hesigb_ehtsig_4     = TXD_HESIGB_EHTSIG_4_BE0;
		cr->hesigb_ehtsig_4_m   = TXD_HESIGB_EHTSIG_4_BE0_M;
		cr->hesigb_ehtsig_5     = TXD_HESIGB_EHTSIG_5_BE0;
		cr->hesigb_ehtsig_5_m   = TXD_HESIGB_EHTSIG_5_BE0_M;
		cr->hesigb_ehtsig_6     = TXD_HESIGB_EHTSIG_6_BE0;
		cr->hesigb_ehtsig_6_m   = TXD_HESIGB_EHTSIG_6_BE0_M;
		cr->hesigb_ehtsig_7     = TXD_HESIGB_EHTSIG_7_BE0;
		cr->hesigb_ehtsig_7_m   = TXD_HESIGB_EHTSIG_7_BE0_M;
		cr->hesigb_ehtsig_8     = TXD_HESIGB_EHTSIG_8_BE0;
		cr->hesigb_ehtsig_8_m   = TXD_HESIGB_EHTSIG_8_BE0_M;
		cr->hesigb_ehtsig_9     = TXD_HESIGB_EHTSIG_9_BE0;
		cr->hesigb_ehtsig_9_m   = TXD_HESIGB_EHTSIG_9_BE0_M;
		cr->hesigb_ehtsig_10    = TXD_HESIGB_EHTSIG_10_BE0;
		cr->hesigb_ehtsig_10_m  = TXD_HESIGB_EHTSIG_10_BE0_M;
		cr->hesigb_ehtsig_11    = TXD_HESIGB_EHTSIG_11_BE0;
		cr->hesigb_ehtsig_11_m  = TXD_HESIGB_EHTSIG_11_BE0_M;
		cr->hesigb_ehtsig_12    = TXD_HESIGB_EHTSIG_12_BE0;
		cr->hesigb_ehtsig_12_m  = TXD_HESIGB_EHTSIG_12_BE0_M;
		cr->hesigb_ehtsig_13    = TXD_HESIGB_EHTSIG_13_BE0;
		cr->hesigb_ehtsig_13_m  = TXD_HESIGB_EHTSIG_13_BE0_M;
		cr->hesigb_ehtsig_14    = TXD_HESIGB_EHTSIG_14_BE0;
		cr->hesigb_ehtsig_14_m  = TXD_HESIGB_EHTSIG_14_BE0_M;
		cr->hesigb_ehtsig_15    = TXD_HESIGB_EHTSIG_15_BE0;
		cr->hesigb_ehtsig_15_m  = TXD_HESIGB_EHTSIG_15_BE0_M;
		cr->hesigb_ehtsig_16    = TXD_HESIGB_EHTSIG_16_BE0;
		cr->hesigb_ehtsig_16_m  = TXD_HESIGB_EHTSIG_16_BE0_M;
		cr->hesigb_ehtsig_17    = TXD_HESIGB_EHTSIG_17_BE0;
		cr->hesigb_ehtsig_17_m  = TXD_HESIGB_EHTSIG_17_BE0_M;
		cr->hesigb_ehtsig_18    = TXD_HESIGB_EHTSIG_18_BE0;
		cr->hesigb_ehtsig_18_m  = TXD_HESIGB_EHTSIG_18_BE0_M;
		cr->hesigb_ehtsig_19    = TXD_HESIGB_EHTSIG_19_BE0;
		cr->hesigb_ehtsig_19_m  = TXD_HESIGB_EHTSIG_19_BE0_M;
		cr->hesigb_ehtsig_20    = TXD_HESIGB_EHTSIG_20_BE0;
		cr->hesigb_ehtsig_20_m  = TXD_HESIGB_EHTSIG_20_BE0_M;
		cr->hesigb_ehtsig_21    = TXD_HESIGB_EHTSIG_21_BE0;
		cr->hesigb_ehtsig_21_m  = TXD_HESIGB_EHTSIG_21_BE0_M;
		cr->hesigb_ehtsig_22    = TXD_HESIGB_EHTSIG_22_BE0;
		cr->hesigb_ehtsig_22_m  = TXD_HESIGB_EHTSIG_22_BE0_M;
		cr->hesigb_ehtsig_23    = TXD_HESIGB_EHTSIG_23_BE0;
		cr->hesigb_ehtsig_23_m  = TXD_HESIGB_EHTSIG_23_BE0_M;
		cr->hesigb_ehtsig_24    = TXD_HESIGB_EHTSIG_24_BE0;
		cr->hesigb_ehtsig_24_m  = TXD_HESIGB_EHTSIG_24_BE0_M;
		cr->hesigb_ehtsig_25    = TXD_HESIGB_EHTSIG_25_BE0;
		cr->hesigb_ehtsig_25_m  = TXD_HESIGB_EHTSIG_25_BE0_M;
		cr->hesigb_ehtsig_26    = TXD_HESIGB_EHTSIG_26_BE0;
		cr->hesigb_ehtsig_26_m  = TXD_HESIGB_EHTSIG_26_BE0_M;
		cr->hesigb_ehtsig_27    = TXD_HESIGB_EHTSIG_27_BE0;
		cr->hesigb_ehtsig_27_m  = TXD_HESIGB_EHTSIG_27_BE0_M;
		cr->hesigb_ehtsig_28    = TXD_HESIGB_EHTSIG_28_BE0;
		cr->hesigb_ehtsig_28_m  = TXD_HESIGB_EHTSIG_28_BE0_M;
		cr->hesigb_ehtsig_29    = TXD_HESIGB_EHTSIG_29_BE0;
		cr->hesigb_ehtsig_29_m  = TXD_HESIGB_EHTSIG_29_BE0_M;
		cr->hesigb_ehtsig_30    = TXD_HESIGB_EHTSIG_30_BE0;
		cr->hesigb_ehtsig_30_m  = TXD_HESIGB_EHTSIG_30_BE0_M;
		cr->hesigb_ehtsig_31    = TXD_HESIGB_EHTSIG_31_BE0;
		cr->hesigb_ehtsig_31_m  = TXD_HESIGB_EHTSIG_31_BE0_M;
		cr->hesigb_ehtsig_32    = TXD_HESIGB_EHTSIG_32_BE0;
		cr->hesigb_ehtsig_32_m  = TXD_HESIGB_EHTSIG_32_BE0_M;
		cr->hesigb_ehtsig_33    = TXD_HESIGB_EHTSIG_33_BE0;
		cr->hesigb_ehtsig_33_m  = TXD_HESIGB_EHTSIG_33_BE0_M;
		cr->hesigb_ehtsig_34    = TXD_HESIGB_EHTSIG_34_BE0;
		cr->hesigb_ehtsig_34_m  = TXD_HESIGB_EHTSIG_34_BE0_M;
		cr->hesigb_ehtsig_35    = TXD_HESIGB_EHTSIG_35_BE0;
		cr->hesigb_ehtsig_35_m  = TXD_HESIGB_EHTSIG_35_BE0_M;
		cr->hesigb_ehtsig_36    = TXD_HESIGB_EHTSIG_36_BE0;
		cr->hesigb_ehtsig_36_m  = TXD_HESIGB_EHTSIG_36_BE0_M;
		cr->hesigb_ehtsig_37    = TXD_HESIGB_EHTSIG_37_BE0;
		cr->hesigb_ehtsig_37_m  = TXD_HESIGB_EHTSIG_37_BE0_M;
		cr->hesigb_ehtsig_38    = TXD_HESIGB_EHTSIG_38_BE0;
		cr->hesigb_ehtsig_38_m  = TXD_HESIGB_EHTSIG_38_BE0_M;
		cr->hesigb_ehtsig_39    = TXD_HESIGB_EHTSIG_39_BE0;
		cr->hesigb_ehtsig_39_m  = TXD_HESIGB_EHTSIG_39_BE0_M;
		cr->hesigb_ehtsig_40    = TXD_HESIGB_EHTSIG_40_BE0;
		cr->hesigb_ehtsig_40_m  = TXD_HESIGB_EHTSIG_40_BE0_M;
		cr->hesigb_ehtsig_41    = TXD_HESIGB_EHTSIG_41_BE0;
		cr->hesigb_ehtsig_41_m  = TXD_HESIGB_EHTSIG_41_BE0_M;
		cr->hesigb_ehtsig_42    = TXD_HESIGB_EHTSIG_42_BE0;
		cr->hesigb_ehtsig_42_m  = TXD_HESIGB_EHTSIG_42_BE0_M;
		cr->hesigb_ehtsig_43    = TXD_HESIGB_EHTSIG_43_BE0;
		cr->hesigb_ehtsig_43_m  = TXD_HESIGB_EHTSIG_43_BE0_M;
		cr->hesigb_ehtsig_44    = TXD_HESIGB_EHTSIG_44_BE0;
		cr->hesigb_ehtsig_44_m  = TXD_HESIGB_EHTSIG_44_BE0_M;
		cr->hesigb_ehtsig_45    = TXD_HESIGB_EHTSIG_45_BE0;
		cr->hesigb_ehtsig_45_m  = TXD_HESIGB_EHTSIG_45_BE0_M;
		cr->hesigb_ehtsig_46    = TXD_HESIGB_EHTSIG_46_BE0;
		cr->hesigb_ehtsig_46_m  = TXD_HESIGB_EHTSIG_46_BE0_M;
		cr->hesigb_ehtsig_47    = TXD_HESIGB_EHTSIG_47_BE0;
		cr->hesigb_ehtsig_47_m  = TXD_HESIGB_EHTSIG_47_BE0_M;
		cr->hesigb_ehtsig_48    = TXD_HESIGB_EHTSIG_48_BE0;
		cr->hesigb_ehtsig_48_m  = TXD_HESIGB_EHTSIG_48_BE0_M;
		cr->hesigb_ehtsig_49    = TXD_HESIGB_EHTSIG_49_BE0;
		cr->hesigb_ehtsig_49_m  = TXD_HESIGB_EHTSIG_49_BE0_M;
		cr->hesigb_ehtsig_50    = TXD_HESIGB_EHTSIG_50_BE0;
		cr->hesigb_ehtsig_50_m  = TXD_HESIGB_EHTSIG_50_BE0_M;
		cr->hesigb_ehtsig_51    = TXD_HESIGB_EHTSIG_51_BE0;
		cr->hesigb_ehtsig_51_m  = TXD_HESIGB_EHTSIG_51_BE0_M;
		cr->hesigb_ehtsig_52    = TXD_HESIGB_EHTSIG_52_BE0;
		cr->hesigb_ehtsig_52_m  = TXD_HESIGB_EHTSIG_52_BE0_M;
		cr->hesigb_ehtsig_53    = TXD_HESIGB_EHTSIG_53_BE0;
		cr->hesigb_ehtsig_53_m  = TXD_HESIGB_EHTSIG_53_BE0_M;
		cr->hesigb_ehtsig_54    = TXD_HESIGB_EHTSIG_54_BE0;
		cr->hesigb_ehtsig_54_m  = TXD_HESIGB_EHTSIG_54_BE0_M;
		cr->hesigb_ehtsig_55    = TXD_HESIGB_EHTSIG_55_BE0;
		cr->hesigb_ehtsig_55_m  = TXD_HESIGB_EHTSIG_55_BE0_M;
		cr->hesigb_ehtsig_56    = TXD_HESIGB_EHTSIG_56_BE0;
		cr->hesigb_ehtsig_56_m  = TXD_HESIGB_EHTSIG_56_BE0_M;
		cr->hesigb_ehtsig_57    = TXD_HESIGB_EHTSIG_57_BE0;
		cr->hesigb_ehtsig_57_m  = TXD_HESIGB_EHTSIG_57_BE0_M;
		cr->hesigb_ehtsig_58    = TXD_HESIGB_EHTSIG_58_BE0;
		cr->hesigb_ehtsig_58_m  = TXD_HESIGB_EHTSIG_58_BE0_M;
		cr->hesigb_ehtsig_59    = TXD_HESIGB_EHTSIG_59_BE0;
		cr->hesigb_ehtsig_59_m  = TXD_HESIGB_EHTSIG_59_BE0_M;
		cr->hesigb_ehtsig_60    = TXD_HESIGB_EHTSIG_60_BE0;
		cr->hesigb_ehtsig_60_m  = TXD_HESIGB_EHTSIG_60_BE0_M;
		cr->hesigb_ehtsig_61    = TXD_HESIGB_EHTSIG_61_BE0;
		cr->hesigb_ehtsig_61_m  = TXD_HESIGB_EHTSIG_61_BE0_M;
		cr->hesigb_ehtsig_62    = TXD_HESIGB_EHTSIG_62_BE0;
		cr->hesigb_ehtsig_62_m  = TXD_HESIGB_EHTSIG_62_BE0_M;
		cr->hesigb_ehtsig_63    = TXD_HESIGB_EHTSIG_63_BE0;
		cr->hesigb_ehtsig_63_m  = TXD_HESIGB_EHTSIG_63_BE0_M;
		cr->hesigb_ehtsig_64    = TXD_HESIGB_EHTSIG_64_BE0;
		cr->hesigb_ehtsig_64_m  = TXD_HESIGB_EHTSIG_64_BE0_M;
		cr->hesigb_ehtsig_65    = TXD_HESIGB_EHTSIG_65_BE0;
		cr->hesigb_ehtsig_65_m  = TXD_HESIGB_EHTSIG_65_BE0_M;
		cr->hesigb_ehtsig_66    = TXD_HESIGB_EHTSIG_66_BE0;
		cr->hesigb_ehtsig_66_m  = TXD_HESIGB_EHTSIG_66_BE0_M;
		cr->hesigb_ehtsig_67    = TXD_HESIGB_EHTSIG_67_BE0;
		cr->hesigb_ehtsig_67_m  = TXD_HESIGB_EHTSIG_67_BE0_M;
		cr->hesigb_ehtsig_68    = TXD_HESIGB_EHTSIG_68_BE0;
		cr->hesigb_ehtsig_68_m  = TXD_HESIGB_EHTSIG_68_BE0_M;
		cr->hesigb_ehtsig_69    = TXD_HESIGB_EHTSIG_69_BE0;
		cr->hesigb_ehtsig_69_m  = TXD_HESIGB_EHTSIG_69_BE0_M;
		cr->hesigb_ehtsig_70    = TXD_HESIGB_EHTSIG_70_BE0;
		cr->hesigb_ehtsig_70_m  = TXD_HESIGB_EHTSIG_70_BE0_M;
		cr->hesigb_ehtsig_71    = TXD_HESIGB_EHTSIG_71_BE0;
		cr->hesigb_ehtsig_71_m  = TXD_HESIGB_EHTSIG_71_BE0_M;
		cr->hesigb_ehtsig_72    = TXD_HESIGB_EHTSIG_72_BE0;
		cr->hesigb_ehtsig_72_m  = TXD_HESIGB_EHTSIG_72_BE0_M;
		cr->hesigb_ehtsig_73    = TXD_HESIGB_EHTSIG_73_BE0;
		cr->hesigb_ehtsig_73_m  = TXD_HESIGB_EHTSIG_73_BE0_M;
		cr->hesigb_ehtsig_74    = TXD_HESIGB_EHTSIG_74_BE0;
		cr->hesigb_ehtsig_74_m  = TXD_HESIGB_EHTSIG_74_BE0_M;
		cr->hesigb_ehtsig_75    = TXD_HESIGB_EHTSIG_75_BE0;
		cr->hesigb_ehtsig_75_m  = TXD_HESIGB_EHTSIG_75_BE0_M;
		cr->hesigb_ehtsig_76    = TXD_HESIGB_EHTSIG_76_BE0;
		cr->hesigb_ehtsig_76_m  = TXD_HESIGB_EHTSIG_76_BE0_M;
		cr->hesigb_ehtsig_77    = TXD_HESIGB_EHTSIG_77_BE0;
		cr->hesigb_ehtsig_77_m  = TXD_HESIGB_EHTSIG_77_BE0_M;
		cr->hesigb_ehtsig_78    = TXD_HESIGB_EHTSIG_78_BE0;
		cr->hesigb_ehtsig_78_m  = TXD_HESIGB_EHTSIG_78_BE0_M;
		cr->hesigb_ehtsig_79    = TXD_HESIGB_EHTSIG_79_BE0;
		cr->hesigb_ehtsig_79_m  = TXD_HESIGB_EHTSIG_79_BE0_M;
		cr->usr0_delmter = USER0_DELMTER_BE0;
		cr->usr0_delmter_m = USER0_DELMTER_BE0_M;
		cr->usr0_eof_padding_len = USER0_EOF_PADDING_LEN_BE0;
		cr->usr0_eof_padding_len_m = USER0_EOF_PADDING_LEN_BE0_M;
		cr->usr0_init_seed = USER0_INIT_SEED_BE0;
		cr->usr0_init_seed_m = USER0_INIT_SEED_BE0_M;
		cr->usr1_delmter = USER1_DELMTER_BE0;
		cr->usr1_delmter_m = USER1_DELMTER_BE0_M;
		cr->usr1_eof_padding_len = USER1_EOF_PADDING_LEN_BE0;
		cr->usr1_eof_padding_len_m = USER1_EOF_PADDING_LEN_BE0_M;
		cr->usr1_init_seed = USER1_INIT_SEED_BE0;
		cr->usr1_init_seed_m = USER1_INIT_SEED_BE0_M;
		cr->usr2_delmter = USER2_DELMTER_BE0;
		cr->usr2_delmter_m = USER2_DELMTER_BE0_M;
		cr->usr2_eof_padding_len = USER2_EOF_PADDING_LEN_BE0;
		cr->usr2_eof_padding_len_m = USER2_EOF_PADDING_LEN_BE0_M;
		cr->usr2_init_seed = USER2_INIT_SEED_BE0;
		cr->usr2_init_seed_m = USER2_INIT_SEED_BE0_M;
		cr->usr3_delmter = USER3_DELMTER_BE0;
		cr->usr3_delmter_m = USER3_DELMTER_BE0_M;
		cr->usr3_eof_padding_len = USER3_EOF_PADDING_LEN_BE0;
		cr->usr3_eof_padding_len_m = USER3_EOF_PADDING_LEN_BE0_M;
		cr->usr3_init_seed = USER3_INIT_SEED_BE0;
		cr->usr3_init_seed_m = USER3_INIT_SEED_BE0_M;
		cr->vht_sigb0 = TXD_VHT_SIGB0_BE0;
		cr->vht_sigb0_m	= TXD_VHT_SIGB0_BE0_M;
		cr->vht_sigb1 = TXD_VHT_SIGB1_BE0;
		cr->vht_sigb1_m	= TXD_VHT_SIGB1_BE0_M;
		cr->vht_sigb2 = TXD_VHT_SIGB2_BE0;
		cr->vht_sigb2_m	= TXD_VHT_SIGB2_BE0_M;
		// ============ [Add] ============ //
		cr->he_sigb_ehtsig_mcs = INTF_R_TXCOMCT_HESIGB_EHTSIG_MCS_BE0;
		cr->he_sigb_ehtsig_mcs_m = INTF_R_TXCOMCT_HESIGB_EHTSIG_MCS_BE0_M;
		// =============================== //
		cr->vht_sigb3 = TXD_VHT_SIGB3_BE0;
		cr->vht_sigb3_m = TXD_VHT_SIGB3_BE0_M;
		cr->n_ltf = INTF_R_TXCOMCT_N_LTF_BE0;
		cr->n_ltf_m = INTF_R_TXCOMCT_N_LTF_BE0_M;
		cr->siga1 = TXD_SIGA1_BE0;
		cr->siga1_m = TXD_SIGA1_BE0_M;
		cr->siga2 = TXD_SIGA2_BE0;
		cr->siga2_m = TXD_SIGA2_BE0_M;
		cr->lsig = TXD_LSIG_BE0;
		cr->lsig_m = TXD_LSIG_BE0_M;
		cr->cca_pw_th = INTF_R_T2RCT_CCA_PW_TH_BE0;
		cr->cca_pw_th_m	= INTF_R_T2RCT_CCA_PW_TH_BE0_M;
		cr->n_sym = INTF_R_TXTIMCT_N_SYM_BE0;
		cr->n_sym_m = INTF_R_TXTIMCT_N_SYM_BE0_M;
		cr->usr0_service = USER0_SERVICE_BE0;
		cr->usr0_service_m = USER0_SERVICE_BE0_M;
		cr->usr1_service = USER1_SERVICE_BE0;
		cr->usr1_service_m = USER1_SERVICE_BE0_M;
		cr->usr2_service = USER2_SERVICE_BE0;
		cr->usr2_service_m = USER2_SERVICE_BE0_M;
		cr->usr3_service = USER3_SERVICE_BE0;
		cr->usr3_service_m = USER3_SERVICE_BE0_M;
		cr->usr0_mdpu_len_byte = USER0_MDPU_LEN_BYTE_BE0;
		cr->usr0_mdpu_len_byte_m = USER0_MDPU_LEN_BYTE_BE0_M;
		cr->usr1_mdpu_len_byte = USER1_MDPU_LEN_BYTE_BE0;
		cr->usr1_mdpu_len_byte_m = USER1_MDPU_LEN_BYTE_BE0_M;
		cr->obw_cts2self_dup_type = INTF_R_TXINFO_OBW_CTS2SELF_DUP_TYPE_BE0;
		cr->obw_cts2self_dup_type_m = INTF_R_TXINFO_OBW_CTS2SELF_DUP_TYPE_BE0_M;
		cr->usr2_mdpu_len_byte = USER2_MDPU_LEN_BYTE_BE0;
		cr->usr2_mdpu_len_byte_m = USER2_MDPU_LEN_BYTE_BE0_M;
		cr->usr3_mdpu_len_byte = USER3_MDPU_LEN_BYTE_BE0;
		cr->usr3_mdpu_len_byte_m = USER3_MDPU_LEN_BYTE_BE0_M;
		cr->usr0_csi_buf_id = MIMOCT_CSI_BUF_ID_0_BE0;
		cr->usr0_csi_buf_id_m = MIMOCT_CSI_BUF_ID_0_BE0_M;
		cr->usr1_csi_buf_id = MIMOCT_CSI_BUF_ID_1_BE0;
		cr->usr1_csi_buf_id_m = MIMOCT_CSI_BUF_ID_1_BE0_M;
		cr->rf_gain_idx	= INTF_R_T2RCT_RF_GAIN_IDX_BE0;
		cr->rf_gain_idx_m = INTF_R_T2RCT_RF_GAIN_IDX_BE0_M;
		cr->usr2_csi_buf_id = MIMOCT_CSI_BUF_ID_2_BE0;
		cr->usr2_csi_buf_id_m = MIMOCT_CSI_BUF_ID_2_BE0_M;
		cr->usr3_csi_buf_id = MIMOCT_CSI_BUF_ID_3_BE0;
		cr->usr3_csi_buf_id_m = MIMOCT_CSI_BUF_ID_3_BE0_M;
		cr->usr0_n_mpdu	= USER0_N_MPDU_BE0;
		cr->usr0_n_mpdu_m = USER0_N_MPDU_BE0_M;
		cr->usr1_n_mpdu	= USER1_N_MPDU_BE0;
		cr->usr1_n_mpdu_m = USER1_N_MPDU_BE0_M;
		cr->usr2_n_mpdu	= USER2_N_MPDU_BE0;
		cr->usr2_n_mpdu_m = USER2_N_MPDU_BE0_M;
		cr->usr0_pw_boost_fctr_db = INTF_R_TXUSRCT_PW_BOOST_FCTR_DB_0_BE0;
		cr->usr0_pw_boost_fctr_db_m = INTF_R_TXUSRCT_PW_BOOST_FCTR_DB_0_BE0_M;
		cr->usr3_n_mpdu = USER3_N_MPDU_BE0;
		cr->usr3_n_mpdu_m = USER3_N_MPDU_BE0_M;
		cr->ch20_with_data = INTF_R_TXINFO_CH20_WITH_DATA_BE0;
		cr->ch20_with_data_m = INTF_R_TXINFO_CH20_WITH_DATA_BE0_M;
		cr->n_usr = INTF_R_TXINFO_USR_CNT_BE0;
		cr->n_usr_m = INTF_R_TXINFO_USR_CNT_BE0_M;
		cr->txcmd_txtp = INTF_R_TXINFO_TXCMD_TXTP_BE0;
		cr->txcmd_txtp_m = INTF_R_TXINFO_TXCMD_TXTP_BE0_M;
		// ============ [Add] ============ //
		cr->usr0_ru_id = INTF_R_TXUSRCT_RU_ID_0_BE0;
		cr->usr0_ru_id_m = INTF_R_TXUSRCT_RU_ID_0_BE0_M;
		cr->usr0_ru_size = INTF_R_TXUSRCT_RU_SIZE_0_BE0;
		cr->usr0_ru_size_m = INTF_R_TXUSRCT_RU_SIZE_0_BE0_M;
		// =============================== //
		cr->usr0_u_id = INTF_R_TXUSRCT_U_ID_0_BE0;
		cr->usr0_u_id_m	= INTF_R_TXUSRCT_U_ID_0_BE0_M;
		// ============ [Add] ============ //
		cr->usr1_ru_id = INTF_R_TXUSRCT_RU_ID_1_BE0;
		cr->usr1_ru_id_m = INTF_R_TXUSRCT_RU_ID_1_BE0_M;
		cr->usr1_ru_size = INTF_R_TXUSRCT_RU_SIZE_1_BE0;
		cr->usr1_ru_size_m = INTF_R_TXUSRCT_RU_SIZE_1_BE0_M;
		// =============================== //
		cr->usr1_u_id = INTF_R_TXUSRCT_U_ID_1_BE0;
		cr->usr1_u_id_m	= INTF_R_TXUSRCT_U_ID_1_BE0_M;
		// ============ [Add] ============ //
		cr->usr2_ru_id = INTF_R_TXUSRCT_RU_ID_2_BE0;
		cr->usr2_ru_id_m = INTF_R_TXUSRCT_RU_ID_2_BE0_M;
		cr->usr2_ru_size = INTF_R_TXUSRCT_RU_SIZE_2_BE0;
		cr->usr2_ru_size_m = INTF_R_TXUSRCT_RU_SIZE_2_BE0_M;
		// =============================== //
		cr->usr2_u_id = INTF_R_TXUSRCT_U_ID_2_BE0;
		cr->usr2_u_id_m	= INTF_R_TXUSRCT_U_ID_2_BE0_M;
		// ============ [Add] ============ //
		cr->usr3_ru_id = INTF_R_TXUSRCT_RU_ID_3_BE0;
		cr->usr3_ru_id_m = INTF_R_TXUSRCT_RU_ID_3_BE0_M;
		cr->usr3_ru_size = INTF_R_TXUSRCT_RU_SIZE_3_BE0;
		cr->usr3_ru_size_m = INTF_R_TXUSRCT_RU_SIZE_3_BE0_M;
		// =============================== //
		cr->usr3_u_id = INTF_R_TXUSRCT_U_ID_3_BE0;
		cr->usr3_u_id_m	= INTF_R_TXUSRCT_U_ID_3_BE0_M;
		// ============ [Add] ============ //
		cr->n_sym_hesigb_ehtsig = INTF_R_TXTIMCT_N_SYM_HESIGB_EHTSIG_BE0;
		cr->n_sym_hesigb_ehtsig_m = INTF_R_TXTIMCT_N_SYM_HESIGB_EHTSIG_BE0_M;
		// =============================== //
		cr->usr0_mcs = INTF_R_TXUSRCT_MCS_0_BE0;
		cr->usr0_mcs_m	= INTF_R_TXUSRCT_MCS_0_BE0_M;
		cr->usr1_mcs = INTF_R_TXUSRCT_MCS_1_BE0;
		cr->usr1_mcs_m	= INTF_R_TXUSRCT_MCS_1_BE0_M;
		cr->usr2_mcs = INTF_R_TXUSRCT_MCS_2_BE0;
		cr->usr2_mcs_m = INTF_R_TXUSRCT_MCS_2_BE0_M;
		cr->usr3_mcs = INTF_R_TXUSRCT_MCS_3_BE0;
		cr->usr3_mcs_m = INTF_R_TXUSRCT_MCS_3_BE0_M;
		cr->usr1_pw_boost_fctr_db = INTF_R_TXUSRCT_PW_BOOST_FCTR_DB_1_BE0;
		cr->usr1_pw_boost_fctr_db_m = INTF_R_TXUSRCT_PW_BOOST_FCTR_DB_1_BE0_M;
		cr->usr2_pw_boost_fctr_db = INTF_R_TXUSRCT_PW_BOOST_FCTR_DB_2_BE0;
		cr->usr2_pw_boost_fctr_db_m = INTF_R_TXUSRCT_PW_BOOST_FCTR_DB_2_BE0_M;
		cr->usr3_pw_boost_fctr_db = INTF_R_TXUSRCT_PW_BOOST_FCTR_DB_3_BE0;
		cr->usr3_pw_boost_fctr_db_m = INTF_R_TXUSRCT_PW_BOOST_FCTR_DB_3_BE0_M;
		cr->ppdu_type = INTF_R_TXINFO_PPDU_TYPE_BE0;
		cr->ppdu_type_m	= INTF_R_TXINFO_PPDU_TYPE_BE0_M;
		// ============ [Add] ============ //
		cr->txsb = INTF_R_TXINFO_TXSB_BE0;
		cr->txsb_m = INTF_R_TXINFO_TXSB_BE0_M;
		// =============================== //
		cr->cfo_comp = INTF_R_TXINFO_CFO_COMP_BE0;
		cr->cfo_comp_m = INTF_R_TXINFO_CFO_COMP_BE0_M;
		cr->pkt_ext_idx = INTF_R_TXTIMCT_PKT_EXT_IDX_BE0;
		cr->pkt_ext_idx_m = INTF_R_TXTIMCT_PKT_EXT_IDX_BE0_M;
		cr->usr0_n_sts = INTF_R_TXUSRCT_N_STS_0_BE0;
		cr->usr0_n_sts_m = INTF_R_TXUSRCT_N_STS_0_BE0_M;
		cr->usr0_n_sts_ru_tot = INTF_R_TXUSRCT_N_STS_RU_TOT_0_BE0;
		cr->usr0_n_sts_ru_tot_m = INTF_R_TXUSRCT_N_STS_RU_TOT_0_BE0_M;
		cr->usr0_strt_sts = INTF_R_TXUSRCT_STRT_STS_0_BE0;
		cr->usr0_strt_sts_m = INTF_R_TXUSRCT_STRT_STS_0_BE0_M;
		cr->usr1_n_sts = INTF_R_TXUSRCT_N_STS_1_BE0;
		cr->usr1_n_sts_m = INTF_R_TXUSRCT_N_STS_1_BE0_M;
		cr->usr1_n_sts_ru_tot = INTF_R_TXUSRCT_N_STS_RU_TOT_1_BE0;
		cr->usr1_n_sts_ru_tot_m = INTF_R_TXUSRCT_N_STS_RU_TOT_1_BE0_M;
		cr->usr1_strt_sts = INTF_R_TXUSRCT_STRT_STS_1_BE0;
		cr->usr1_strt_sts_m = INTF_R_TXUSRCT_STRT_STS_1_BE0_M;
		cr->usr2_n_sts = INTF_R_TXUSRCT_N_STS_2_BE0;
		cr->usr2_n_sts_m = INTF_R_TXUSRCT_N_STS_2_BE0_M;
		cr->usr2_n_sts_ru_tot = INTF_R_TXUSRCT_N_STS_RU_TOT_2_BE0;
		cr->usr2_n_sts_ru_tot_m	= INTF_R_TXUSRCT_N_STS_RU_TOT_2_BE0_M;
		cr->usr2_strt_sts = INTF_R_TXUSRCT_STRT_STS_2_BE0;
		cr->usr2_strt_sts_m = INTF_R_TXUSRCT_STRT_STS_2_BE0_M;
		cr->usr3_n_sts = INTF_R_TXUSRCT_N_STS_3_BE0;
		cr->usr3_n_sts_m = INTF_R_TXUSRCT_N_STS_3_BE0_M;
		cr->usr3_n_sts_ru_tot = INTF_R_TXUSRCT_N_STS_RU_TOT_3_BE0;
		cr->usr3_n_sts_ru_tot_m	= INTF_R_TXUSRCT_N_STS_RU_TOT_3_BE0_M;
		cr->usr3_strt_sts = INTF_R_TXUSRCT_STRT_STS_3_BE0;
		cr->usr3_strt_sts_m = INTF_R_TXUSRCT_STRT_STS_3_BE0_M;
		cr->source_gen_mode_idx	= SOURCE_GEN_MODE_IDX_BE0;
		cr->source_gen_mode_idx_m = SOURCE_GEN_MODE_IDX_BE0_M;
		cr->gi_type = INTF_R_TXCOMCT_GI_TYPE_BE0;
		cr->gi_type_m = INTF_R_TXCOMCT_GI_TYPE_BE0_M;
		cr->ltf_type = INTF_R_TXCOMCT_LTF_TYPE_BE0;
		cr->ltf_type_m = INTF_R_TXCOMCT_LTF_TYPE_BE0_M;
		cr->dbw_idx = INTF_R_TXINFO_DBW_IDX_BE0;
		cr->dbw_idx_m = INTF_R_TXINFO_DBW_IDX_BE0_M;
		cr->pre_fec_fctr = INTF_R_TXTIMCT_PRE_FEC_FCTR_BE0;
		cr->pre_fec_fctr_m = INTF_R_TXTIMCT_PRE_FEC_FCTR_BE0_M;
		cr->beam_change_en = INTF_R_TXCOMCT_BEAM_CHANGE_EN_BE0;
		cr->beam_change_en_m = INTF_R_TXCOMCT_BEAM_CHANGE_EN_BE0_M;
		cr->doppler_en = INTF_R_TXCOMCT_DOPPLER_EN_BE0;
		cr->doppler_en_m = INTF_R_TXCOMCT_DOPPLER_EN_BE0_M;
		cr->feedback_status = INTF_R_TXCOMCT_FEEDBACK_STATUS_BE0;
		cr->feedback_status_m = INTF_R_TXCOMCT_FEEDBACK_STATUS_BE0_M;
		cr->he_sigb_dcm_en = INTF_R_TXCOMCT_HE_SIGB_DCM_EN_BE0;
		cr->he_sigb_dcm_en_m = INTF_R_TXCOMCT_HE_SIGB_DCM_EN_BE0_M;
		cr->midamble_mode = INTF_R_TXCOMCT_MIDAMBLE_MODE_BE0;
		cr->midamble_mode_m = INTF_R_TXCOMCT_MIDAMBLE_MODE_BE0_M;
		cr->mumimo_ltf_mode_en = INTF_R_TXCOMCT_MUMIMO_LTF_MODE_EN_BE0;
		cr->mumimo_ltf_mode_en_m = INTF_R_TXCOMCT_MUMIMO_LTF_MODE_EN_BE0_M;
		cr->stbc_en = INTF_R_TXCOMCT_STBC_EN_BE0;
		cr->stbc_en_m = INTF_R_TXCOMCT_STBC_EN_BE0_M;
		cr->ant_sel_a = INTF_R_TXINFO_ANTIDX_ANT_SEL_A_BE0;
		cr->ant_sel_a_m	= INTF_R_TXINFO_ANTIDX_ANT_SEL_A_BE0_M;
		cr->ant_sel_b = INTF_R_TXINFO_ANTIDX_ANT_SEL_B_BE0;
		cr->ant_sel_b_m	= INTF_R_TXINFO_ANTIDX_ANT_SEL_B_BE0_M;
		cr->ant_sel_c = INTF_R_TXINFO_ANTIDX_ANT_SEL_C_BE0;
		cr->ant_sel_c_m	= INTF_R_TXINFO_ANTIDX_ANT_SEL_C_BE0_M;
		cr->ant_sel_d = INTF_R_TXINFO_ANTIDX_ANT_SEL_D_BE0;
		cr->ant_sel_d_m	= INTF_R_TXINFO_ANTIDX_ANT_SEL_D_BE0_M;
		cr->cca_pw_th_en = INTF_R_T2RCT_CCA_PW_TH_EN_BE0;
		cr->cca_pw_th_en_m = INTF_R_T2RCT_CCA_PW_TH_EN_BE0_M;
		cr->rf_fixed_gain_en = INTF_R_T2RCT_RF_FIXED_GAIN_EN_BE0;
		cr->rf_fixed_gain_en_m = INTF_R_T2RCT_RF_FIXED_GAIN_EN_BE0_M;
		cr->ul_cqi_rpt_tri = RXINT_R_TBCOMCT_UL_CQI_SND_EN_BE0;
		cr->ul_cqi_rpt_tri_m = RXINT_R_TBCOMCT_UL_CQI_SND_EN_BE0_M;
		cr->ldpc_extr = INTF_R_TXTIMCT_LDPC_EXTR_BE0;
		cr->ldpc_extr_m	= INTF_R_TXTIMCT_LDPC_EXTR_BE0_M;
		cr->usr0_dcm_en	= INTF_R_TXUSRCT_DCM_EN_0_BE0;
		cr->usr0_dcm_en_m = INTF_R_TXUSRCT_DCM_EN_0_BE0_M;
		cr->usr0_fec_type = INTF_R_TXUSRCT_FEC_TYPE_0_BE0;
		cr->usr0_fec_type_m = INTF_R_TXUSRCT_FEC_TYPE_0_BE0_M;
		cr->usr1_dcm_en	= INTF_R_TXUSRCT_DCM_EN_1_BE0;
		cr->usr1_dcm_en_m = INTF_R_TXUSRCT_DCM_EN_1_BE0_M;
		cr->usr1_fec_type = INTF_R_TXUSRCT_FEC_TYPE_1_BE0;
		cr->usr1_fec_type_m = INTF_R_TXUSRCT_FEC_TYPE_1_BE0_M;
		cr->usr2_dcm_en	= INTF_R_TXUSRCT_DCM_EN_2_BE0;
		cr->usr2_dcm_en_m = INTF_R_TXUSRCT_DCM_EN_2_BE0_M;
		cr->usr2_fec_type = INTF_R_TXUSRCT_FEC_TYPE_2_BE0;
		cr->usr2_fec_type_m = INTF_R_TXUSRCT_FEC_TYPE_2_BE0_M;
		cr->usr3_dcm_en = INTF_R_TXUSRCT_DCM_EN_3_BE0;
		cr->usr3_dcm_en_m = INTF_R_TXUSRCT_DCM_EN_3_BE0_M;
		cr->usr3_fec_type = INTF_R_TXUSRCT_FEC_TYPE_3_BE0;
		cr->usr3_fec_type_m = INTF_R_TXUSRCT_FEC_TYPE_3_BE0_M;
		// ============ [Add] ============ //
		cr->usr0_precoding_mode_idx = INTF_R_TXUSRCT_PRECODING_MODE_IDX_0_BE0;
		cr->usr0_precoding_mode_idx_m = INTF_R_TXUSRCT_PRECODING_MODE_IDX_0_BE0_M;
		cr->usr1_precoding_mode_idx = INTF_R_TXUSRCT_PRECODING_MODE_IDX_1_BE0;
		cr->usr1_precoding_mode_idx_m = INTF_R_TXUSRCT_PRECODING_MODE_IDX_1_BE0_M;
		cr->usr2_precoding_mode_idx = INTF_R_TXUSRCT_PRECODING_MODE_IDX_2_BE0;
		cr->usr2_precoding_mode_idx_m = INTF_R_TXUSRCT_PRECODING_MODE_IDX_2_BE0_M;
		cr->usr3_precoding_mode_idx = INTF_R_TXUSRCT_PRECODING_MODE_IDX_3_BE0;
		cr->usr3_precoding_mode_idx_m = INTF_R_TXUSRCT_PRECODING_MODE_IDX_3_BE0_M;
		cr->ppdu_var = INTF_R_TXINFO_PPDU_VAR_BE0;
		cr->ppdu_var_m = INTF_R_TXINFO_PPDU_VAR_BE0_M;
		cr->usig_0 = TXD_USIG_0_BE0;
		cr->usig_0_m = TXD_USIG_0_BE0_M;
		cr->usig_1 = TXD_USIG_1_BE0;
		cr->usig_1_m = TXD_USIG_1_BE0_M;
		cr->tssi_ru_size = INTF_R_TXINFO_TSSICT_RU_SIZE_BE0;
		cr->tssi_ru_size_m = INTF_R_TXINFO_TSSICT_RU_SIZE_BE0_M;
		// =============================== //
		break;
	#endif

	default:
		#if 0
		BB_WARNING("[%s] BBCR Hook FAIL!\n", __func__);
		if (bb->bb_dbg_i.cr_fake_init_hook_en) {
			BB_TRACE("[%s] BBCR fake init\n", __func__);
			halbb_cr_hook_fake_init(bb, (u32 *)cr, (sizeof(struct bb_plcp_cr_info) >> 2));
		}
		#endif
		break;
	}

#if 0
	if (bb->bb_dbg_i.cr_init_hook_recorder_en) {
		BB_TRACE("[%s] BBCR Hook dump\n", __func__);
		halbb_cr_hook_init_dump(bb, (u32 *)cr, (sizeof(struct bb_plcp_cr_info) >> 2));
	}
#endif
}

#else

enum plcp_sts halbb_plcp_gen_7(struct bb_info *bb, struct halbb_plcp_info *in,
		    struct usr_plcp_gen_in *user, enum phl_phy_idx phy_idx)
{
	return SPEC_INVALID;
}

#endif

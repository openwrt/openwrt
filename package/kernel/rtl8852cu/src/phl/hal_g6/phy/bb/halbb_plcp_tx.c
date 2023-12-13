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
#if 0 // BE_IC_TYPE
const u8 ru_alloc_b1_7_tbl[RU_SIZE_NUM][2] = {{0, 37}, //RU26
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
#endif
u8 halbb_set_crc8(struct bb_info *bb, unsigned char in[], u8 len)
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

void halbb_ic_cfg(struct bb_info *bb, struct halbb_plcp_info *in,
		  struct bb_plcp_cr_info *cr, enum phl_phy_idx phy_idx)
{
	u16 i = 0;
	u8 ht_mcs = 0xff;
	u16 pw_a_cr = 0;
	u16 pw_b_cr = 0;
	u32 table_len = 0;
	u32 *pwr_comp_1ss_52c = NULL;
	u32 *pwr_comp_2ss_52c = NULL;

	// Triangular shaping
	if (bb->ic_type == BB_RTL8192XB || bb->ic_type == BB_RTL8852C) {
		if (in->ppdu_type == HT_MF_FMT)
			ht_mcs = in->usr[0].mcs % 8;
		if (in->ppdu_type != B_MODE_FMT) {
			if (bb->dyn_pmac_tri_en) {
				if (in->usr[0].mcs <= 7 || ht_mcs <= 7) {
					halbb_set_reg_cmn(bb, cr->tx_tri_idx, cr->tx_tri_idx_m, bb->pmac_tri_idx, phy_idx);
					halbb_set_reg_cmn(bb, cr->tx_tri_pw_ofst, cr->tx_tri_pw_ofst_m, bb->pmac_pwr_ofst, phy_idx);
				} else {
					halbb_set_reg_cmn(bb, cr->tx_tri_idx, cr->tx_tri_idx_m, 0, phy_idx);
					halbb_set_reg_cmn(bb, cr->tx_tri_pw_ofst, cr->tx_tri_pw_ofst_m, 0, phy_idx);
				}
			} else {
				if (bb->pmac_tri_en) {
					halbb_set_reg_cmn(bb, cr->tx_tri_idx, cr->tx_tri_idx_m, bb->pmac_tri_idx, phy_idx);
					halbb_set_reg_cmn(bb, cr->tx_tri_pw_ofst, cr->tx_tri_pw_ofst_m, bb->pmac_pwr_ofst, phy_idx);
				} else {
					halbb_set_reg_cmn(bb, cr->tx_tri_idx, cr->tx_tri_idx_m, 0, phy_idx);
					halbb_set_reg_cmn(bb, cr->tx_tri_pw_ofst, cr->tx_tri_pw_ofst_m, 0, phy_idx);
				}
			}
		} else {
			halbb_set_reg_cmn(bb, cr->tx_tri_idx, cr->tx_tri_idx_m, 0, phy_idx);
			halbb_set_reg_cmn(bb, cr->tx_tri_pw_ofst, cr->tx_tri_pw_ofst_m, 0, phy_idx);
		}
	}

	// Digital comp
	if (bb->ic_type == BB_RTL8192XB) {
		pw_a_cr = 0x5428;
		pw_b_cr = 0x7428;
		table_len = sizeof(pwr_comp_1ss_32b)/(sizeof(u32) * 2);
		if (bb->pwr_comp_en  && bb->phl_com->dev_cap.rfe_type <= 50) {
			if (in->usr[0].nss == 1 && bb->bb_api_i.band == BAND_ON_5G) {
				for (i = 0; i < table_len; i ++) {
					halbb_set_reg_cmn(bb, pw_a_cr, MASKDWORD, pwr_comp_1ss_32b[i][0], phy_idx);
					halbb_set_reg_cmn(bb, pw_b_cr, MASKDWORD, pwr_comp_1ss_32b[i][1], phy_idx);
					pw_a_cr += 0x4;
					pw_b_cr += 0x4;
				}
			} else if (in->usr[0].nss == 2 && bb->bb_api_i.band == BAND_ON_5G) {
				for (i = 0; i < table_len; i ++) {
					halbb_set_reg_cmn(bb, pw_a_cr, MASKDWORD, pwr_comp_2ss_32b[i][0], phy_idx);
					halbb_set_reg_cmn(bb, pw_b_cr, MASKDWORD, pwr_comp_2ss_32b[i][1], phy_idx);
					pw_a_cr += 0x4;
					pw_b_cr += 0x4;
				}
			} else {
				for (i = 0; i < table_len; i ++) {
					halbb_set_reg_cmn(bb, pw_a_cr, MASKDWORD, pwr_comp_92xb[i][0], phy_idx);
					halbb_set_reg_cmn(bb, pw_b_cr, MASKDWORD, pwr_comp_92xb[i][1], phy_idx);
					pw_a_cr += 0x4;
					pw_b_cr += 0x4;
				}
			}
		} else {
			for (i = 0; i < table_len; i ++) {
				halbb_set_reg_cmn(bb, pw_a_cr, MASKDWORD, 0, phy_idx);
				halbb_set_reg_cmn(bb, pw_b_cr, MASKDWORD, 0, phy_idx);
				pw_a_cr += 0x4;
				pw_b_cr += 0x4;
			}
		}
	} else if (bb->ic_type == BB_RTL8852C) {
		pw_a_cr = 0x5428;
		pw_b_cr = 0x7428;
		table_len = sizeof(pwr_comp_1ss_8852c)/sizeof(u32);
		pwr_comp_1ss_52c = bb->phl_com->dev_cap.rfe_type <= 50 ?
				   (u32 *)pwr_comp_1ss_8852c :
				   (u32 *)pwr_comp_1ss_efem_8852c;
		pwr_comp_2ss_52c = bb->phl_com->dev_cap.rfe_type <= 50 ?
				   (u32 *)pwr_comp_2ss_8852c :
				   (u32 *)pwr_comp_2ss_efem_8852c;
		if (bb->pwr_comp_en && bb->phl_com->dev_cap.rfe_type <= 50) {// eFEM disable digital comp !!
			if (in->usr[0].nss == 1) {
				while (i < table_len) {
					halbb_set_reg_cmn(bb, pw_a_cr, MASKDWORD, pwr_comp_1ss_52c[i], phy_idx);
					halbb_set_reg_cmn(bb, pw_b_cr, MASKDWORD, pwr_comp_1ss_52c[i + 1], phy_idx);
					pw_a_cr += 0x4;
					pw_b_cr += 0x4;
					i += 2;
				}
			} else {
				while (i < table_len) {
					halbb_set_reg_cmn(bb, pw_a_cr, MASKDWORD, pwr_comp_2ss_52c[i], phy_idx);
					halbb_set_reg_cmn(bb, pw_b_cr, MASKDWORD, pwr_comp_2ss_52c[i + 1], phy_idx);
					pw_a_cr += 0x4;
					pw_b_cr += 0x4;
					i += 2;
				}
			}
		} else {
			while (i < table_len) {
				halbb_set_reg_cmn(bb, pw_a_cr, MASKDWORD, 0, phy_idx);
				halbb_set_reg_cmn(bb, pw_b_cr, MASKDWORD, 0, phy_idx);
				pw_a_cr += 0x4;
				pw_b_cr += 0x4;
				i += 2;
			}
		}
	}
}
#if 0 // BE_IC_TYPE
void halbb_ppdu_var_type_cfg(struct bb_info *bb_0, struct halbb_plcp_info *in,
			     enum phl_phy_idx phy_idx)
{
	struct bb_info *bb = bb_0;
	u8 ppdu_type = 0, ppdu_var = 0;

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
		ppdu_var = bb->bb_api_i.bw >= in->dbw ? 1 : 0;
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
		if ((in->punc_pattern != 0xff) || (in->punc_pattern != 0xf)) // With puncture
			ppdu_var = in->ndp_en ? 7 : 5;
		else
			ppdu_var = in->ndp_en ? 4 : 0;
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
}

u32 halbb_ru_occupied_sub20_he(struct bb_info *bb, struct halbb_plcp_info *in)
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

u32 halbb_ru_occupied_sub20_eht(struct bb_info *bb, struct halbb_plcp_info *in)
{
	u32 output = 0;
	u8 ch20_with_data_dbw80 = 0, ch20_with_data_dbw160 = 0, nsub_80 = 4, mru_idx = 0, n_x1_flag = 0, n_x1 = 0;
	u8 size_idx = 0, num_b1_7 = 0;
	u8 ru_484_242_tbl[4] = {0xE, 0xD, 0xB, 0x7}; // {4b'1110, 4b'1101, 4b'1011, 4b'0111}
	u8 ru_996_484_tbl[4] = {0xFC, 0xF3, 0xCF, 0x3F}; // {8b'11111100, 8b'11110011, 8b'11001111, 8b'00111111}
	u16 ru_996x2_484_tbl[12] = {0xFFC, 0xFF3, 0xFCF, 0xF3F, 0xCFF, 0x3FF,
				    0xFFC0, 0xFF30, 0xFCF0, 0xF3F0, 0xCFF0, 0x3FF0};

	size_idx = ru_alloc_b1_7_tbl[in->usr[0].ru_size][0];
	num_b1_7 = ru_alloc_b1_7_tbl[in->usr[0].ru_size][1];

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

u8 halbb_eht_sig_gi_ltf_tbl(struct bb_info *bb, u8 gi, u8 ltf)
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
#endif
u32 halbb_cfg_ch20_with_data(struct bb_info *bb, struct halbb_plcp_info *in)
{
	u32 ch20_with_data = 0;
#if 0 // BE_IC_TYPE
	if (bb->ic_type >= BB_RTL8922A) { // [BE]
		if (in->ppdu_type == HE_TB_FMT)
			ch20_with_data = halbb_ru_occupied_sub20_he(bb, in);
		else if ((in->ppdu_type == EHT_MU_RU_FMT) || (in->ppdu_type == EHT_TB_FMT))
			ch20_with_data = halbb_ru_occupied_sub20_eht(bb, in);
		else {
			switch (in->dbw) {
				case 0:
					ch20_with_data = ((bb->ic_type == BB_RTL8852A) || (bb->ic_type == BB_RTL8852B) || (bb->ic_type == BB_RTL8851B)) ? 0x80 : 0x1;
					break;
				case 1:
					ch20_with_data = ((bb->ic_type == BB_RTL8852A) || (bb->ic_type == BB_RTL8852B) || (bb->ic_type == BB_RTL8851B)) ? 0xc0 : 0x3;
					break;
				case 2:
					ch20_with_data = ((bb->ic_type == BB_RTL8852A) || (bb->ic_type == BB_RTL8852B) || (bb->ic_type == BB_RTL8851B)) ? 0xf0 : 0xf;
					break;
				case 3:
					ch20_with_data = 0xff;
					break;
				default:
					break;
			}
		}
	} else { //[AX]
#endif
		if (in->ppdu_type == HE_TB_FMT) {
			switch (in->dbw) {
			case 0:
				if (((in->usr[0].ru_alloc >> 1) <= 8) || ((in->usr[0].ru_alloc >> 1) >= 37 && (in->usr[0].ru_alloc >> 1) <= 40)
					|| (in->usr[0].ru_alloc >> 1) == 53 || (in->usr[0].ru_alloc >> 1) == 54 || (in->usr[0].ru_alloc >> 1) == 61)
					ch20_with_data = ((bb->ic_type == BB_RTL8852A) || (bb->ic_type == BB_RTL8852B) || (bb->ic_type == BB_RTL8851B)) ? 0x80 : 0x1;
				break;

			case 1:
				if (((in->usr[0].ru_alloc >> 1) <= 8) || ((in->usr[0].ru_alloc >> 1) >= 37 && (in->usr[0].ru_alloc >> 1) <= 40)
					|| (in->usr[0].ru_alloc >> 1) == 53 || (in->usr[0].ru_alloc >> 1) == 54 || (in->usr[0].ru_alloc >> 1) == 61)
					ch20_with_data = ((bb->ic_type == BB_RTL8852A) || (bb->ic_type == BB_RTL8852B) || (bb->ic_type == BB_RTL8851B)) ? 0x80 : 0x1;

				else if (((in->usr[0].ru_alloc >> 1) >= 9 && (in->usr[0].ru_alloc >> 1) <= 17) || ((in->usr[0].ru_alloc >> 1) >= 41 && (in->usr[0].ru_alloc >> 1) <= 44)
					|| (in->usr[0].ru_alloc >> 1) == 55 || (in->usr[0].ru_alloc >> 1) == 56 || (in->usr[0].ru_alloc >> 1) == 62)
					ch20_with_data = ((bb->ic_type == BB_RTL8852A) || (bb->ic_type == BB_RTL8852B) || (bb->ic_type == BB_RTL8851B)) ? 0x40 : 0x2;

				else if ((in->usr[0].ru_alloc >> 1) == 65)
					ch20_with_data = ((bb->ic_type == BB_RTL8852A) || (bb->ic_type == BB_RTL8852B) || (bb->ic_type == BB_RTL8851B)) ? 0xc0 : 0x3;

				break;

			case 2:
				if (((in->usr[0].ru_alloc >> 1) <= 8) || ((in->usr[0].ru_alloc >> 1) >= 37 && (in->usr[0].ru_alloc >> 1) <= 40)
					|| (in->usr[0].ru_alloc >> 1) == 53 || (in->usr[0].ru_alloc >> 1) == 54 || (in->usr[0].ru_alloc >> 1) == 61)
					ch20_with_data = ((bb->ic_type == BB_RTL8852A) || (bb->ic_type == BB_RTL8852B) || (bb->ic_type == BB_RTL8851B)) ? 0x80 : 0x1;

				else if (((in->usr[0].ru_alloc >> 1) >= 10 && (in->usr[0].ru_alloc >> 1) <= 17) || ((in->usr[0].ru_alloc >> 1) >= 42 && (in->usr[0].ru_alloc >> 1) <= 44)
					|| (in->usr[0].ru_alloc >> 1) == 56)
					ch20_with_data = ((bb->ic_type == BB_RTL8852A) || (bb->ic_type == BB_RTL8852B) || (bb->ic_type == BB_RTL8851B)) ? 0x40 : 0x2;

				else if ((in->usr[0].ru_alloc >> 1) == 9 || (in->usr[0].ru_alloc >> 1) == 41 || (in->usr[0].ru_alloc >> 1) == 55 || (in->usr[0].ru_alloc >> 1) == 62 || (in->usr[0].ru_alloc >> 1) == 65)
					ch20_with_data = ((bb->ic_type == BB_RTL8852A) || (bb->ic_type == BB_RTL8852B) || (bb->ic_type == BB_RTL8851B)) ? 0xc0 : 0x3;

				else if (((in->usr[0].ru_alloc >> 1) >= 19 && (in->usr[0].ru_alloc >> 1) <= 26) || ((in->usr[0].ru_alloc >> 1) >= 45 && (in->usr[0].ru_alloc >> 1) <= 47)
					|| (in->usr[0].ru_alloc >> 1) == 57)
					ch20_with_data = ((bb->ic_type == BB_RTL8852A) || (bb->ic_type == BB_RTL8852B) || (bb->ic_type == BB_RTL8851B)) ? 0x20 : 0x4;

				else if (((in->usr[0].ru_alloc >> 1) >= 28 && (in->usr[0].ru_alloc >> 1) <= 36) || ((in->usr[0].ru_alloc >> 1) >= 49 && (in->usr[0].ru_alloc >> 1) <= 52)
					|| (in->usr[0].ru_alloc >> 1) == 59 || (in->usr[0].ru_alloc >> 1) == 60 || (in->usr[0].ru_alloc >> 1) == 64)
					ch20_with_data = ((bb->ic_type == BB_RTL8852A) || (bb->ic_type == BB_RTL8852B) || (bb->ic_type == BB_RTL8851B)) ? 0x10 : 0x8;

				else if ((in->usr[0].ru_alloc >> 1) == 27 || (in->usr[0].ru_alloc >> 1) == 48 || (in->usr[0].ru_alloc >> 1) == 58 || (in->usr[0].ru_alloc >> 1) == 63 || (in->usr[0].ru_alloc >> 1) == 66)
					ch20_with_data = ((bb->ic_type == BB_RTL8852A) || (bb->ic_type == BB_RTL8852B) || (bb->ic_type == BB_RTL8851B)) ? 0x30 : 0xc;

				else if ((in->usr[0].ru_alloc >> 1) == 18)
					ch20_with_data = ((bb->ic_type == BB_RTL8852A) || (bb->ic_type == BB_RTL8852B) || (bb->ic_type == BB_RTL8851B)) ? 0x60 : 0x6;

				else if ((in->usr[0].ru_alloc >> 1) == 67)
					ch20_with_data = ((bb->ic_type == BB_RTL8852A) || (bb->ic_type == BB_RTL8852B) || (bb->ic_type == BB_RTL8851B)) ? 0xf0 : 0xf;

				break;
			case 3:
				if (in->usr[0].ru_alloc & BIT(0)) {
					if (((in->usr[0].ru_alloc >> 1) <= 8) || ((in->usr[0].ru_alloc >> 1) >= 37 && (in->usr[0].ru_alloc >> 1) <= 40)
						|| (in->usr[0].ru_alloc >> 1) == 53 || (in->usr[0].ru_alloc >> 1) == 54 || (in->usr[0].ru_alloc >> 1) == 61)
						ch20_with_data = 0x10;
					else if (((in->usr[0].ru_alloc >> 1) >= 10 && (in->usr[0].ru_alloc >> 1) <= 17) || ((in->usr[0].ru_alloc >> 1) >= 42 && (in->usr[0].ru_alloc >> 1) <= 44)
						|| (in->usr[0].ru_alloc >> 1) == 56)
						ch20_with_data = 0x20;
					else if ((in->usr[0].ru_alloc >> 1) == 9 || (in->usr[0].ru_alloc >> 1) == 41 || (in->usr[0].ru_alloc >> 1) == 55 || (in->usr[0].ru_alloc >> 1) == 62 || (in->usr[0].ru_alloc >> 1) == 65)
						ch20_with_data = 0x30;
					else if (((in->usr[0].ru_alloc >> 1) >= 19 && (in->usr[0].ru_alloc >> 1) <= 26) || ((in->usr[0].ru_alloc >> 1) >= 45 && (in->usr[0].ru_alloc >> 1) <= 47)
						|| (in->usr[0].ru_alloc >> 1) == 57)
						ch20_with_data = 0x40;
					else if ((in->usr[0].ru_alloc >> 1) == 18)
						ch20_with_data = 0x60;
					else if (((in->usr[0].ru_alloc >> 1) >= 28 && (in->usr[0].ru_alloc >> 1) <= 36) || ((in->usr[0].ru_alloc >> 1) >= 49 && (in->usr[0].ru_alloc >> 1) <= 52)
						|| (in->usr[0].ru_alloc >> 1) == 59 || (in->usr[0].ru_alloc >> 1) == 60 || (in->usr[0].ru_alloc >> 1) == 64)
						ch20_with_data = 0x80;
					else if ((in->usr[0].ru_alloc >> 1) == 27 || (in->usr[0].ru_alloc >> 1) == 48 || (in->usr[0].ru_alloc >> 1) == 58 || (in->usr[0].ru_alloc >> 1) == 63 || (in->usr[0].ru_alloc >> 1) == 66)
						ch20_with_data = 0xc0;
					else if ((in->usr[0].ru_alloc >> 1) == 67)
						ch20_with_data = 0xf0;
					else if ((in->usr[0].ru_alloc >> 1) == 68)
						ch20_with_data = 0xff;
				} else {
					if (((in->usr[0].ru_alloc >> 1) <= 8) || ((in->usr[0].ru_alloc >> 1) >= 37 && (in->usr[0].ru_alloc >> 1) <= 40)
						|| (in->usr[0].ru_alloc >> 1) == 53 || (in->usr[0].ru_alloc >> 1) == 54 || (in->usr[0].ru_alloc >> 1) == 61)
						ch20_with_data = 0x1;
					else if (((in->usr[0].ru_alloc >> 1) >= 10 && (in->usr[0].ru_alloc >> 1) <= 17) || ((in->usr[0].ru_alloc >> 1) >= 42 && (in->usr[0].ru_alloc >> 1) <= 44)
						|| (in->usr[0].ru_alloc >> 1) == 56)
						ch20_with_data = 0x2;
					else if ((in->usr[0].ru_alloc >> 1) == 9 || (in->usr[0].ru_alloc >> 1) == 41 || (in->usr[0].ru_alloc >> 1) == 55 || (in->usr[0].ru_alloc >> 1) == 62 || (in->usr[0].ru_alloc >> 1) == 65)
						ch20_with_data = 0x3;
					else if (((in->usr[0].ru_alloc >> 1) >= 19 && (in->usr[0].ru_alloc >> 1) <= 26) || ((in->usr[0].ru_alloc >> 1) >= 45 && (in->usr[0].ru_alloc >> 1) <= 47)
						|| (in->usr[0].ru_alloc >> 1) == 57)
						ch20_with_data = 0x4;
					else if ((in->usr[0].ru_alloc >> 1) == 18)
						ch20_with_data = 0x6;
					else if (((in->usr[0].ru_alloc >> 1) >= 28 && (in->usr[0].ru_alloc >> 1) <= 36) || ((in->usr[0].ru_alloc >> 1) >= 49 && (in->usr[0].ru_alloc >> 1) <= 52)
						|| (in->usr[0].ru_alloc >> 1) == 59 || (in->usr[0].ru_alloc >> 1) == 60 || (in->usr[0].ru_alloc >> 1) == 64)
						ch20_with_data = 0x8;
					else if ((in->usr[0].ru_alloc >> 1) == 27 || (in->usr[0].ru_alloc >> 1) == 48 || (in->usr[0].ru_alloc >> 1) == 58 || (in->usr[0].ru_alloc >> 1) == 63 || (in->usr[0].ru_alloc >> 1) == 66)
						ch20_with_data = 0xc;
					else if ((in->usr[0].ru_alloc >> 1) == 67)
						ch20_with_data = 0xf;
					else if ((in->usr[0].ru_alloc >> 1) == 68)
						ch20_with_data = 0xff;
				}
				break;
			default:
				break;
			}
		} else {
			switch (in->dbw) {
				case 0:
					ch20_with_data = ((bb->ic_type == BB_RTL8852A) || (bb->ic_type == BB_RTL8852B) || (bb->ic_type == BB_RTL8851B)) ? 0x80 : 0x1;
					break;
				case 1:
					ch20_with_data = ((bb->ic_type == BB_RTL8852A) || (bb->ic_type == BB_RTL8852B) || (bb->ic_type == BB_RTL8851B)) ? 0xc0 : 0x3;
					break;
				case 2:
					ch20_with_data = ((bb->ic_type == BB_RTL8852A) || (bb->ic_type == BB_RTL8852B) || (bb->ic_type == BB_RTL8851B)) ? 0xf0 : 0xf;
					break;
				case 3:
					ch20_with_data = 0xff;
					break;
				default:
					break;
			}
		}
#if 0 // BE_IC_TYPE
	}
#endif
	return ch20_with_data;
}
#if 0 // BE_IC_TYPE
void halbb_ppdu_type_comp_mode_trans(struct bb_info *bb, bool ul_dl_flag,
				     enum packet_format_t ppdu_type,
				     struct plcp_tx_pre_fec_padding_setting_out_t *out_plcp)
{
	if (ul_dl_flag == 0)
		out_plcp->ppdu_type_comp_mode = ppdu_type == EHT_MU_RU_FMT ? 0 : 2;
	else
		out_plcp->ppdu_type_comp_mode = ppdu_type == EHT_TB_FMT ? 0 : 1;
}

bool halbb_punc_ch_info_2_ru_size_idx(struct bb_info *bb, enum plcp_dbw dbw,
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
			out_plcp->usr[0].ru_idx = 1;
			out_plcp->usr[0].ru_size = RU242;
			out_plcp->punc_ch_info = 0;
			break;
		case DBW40:
			out_plcp->usr[0].ru_idx = 1;
			out_plcp->usr[0].ru_size = RU484;
			out_plcp->punc_ch_info = 0;
			break;
		case DBW80:
			for (i = 0; i < 5; i++) {
				if (punc_pattern == dbw80_punc_pattern[i]) {
					out_plcp->punc_ch_info = i;
					pattern_chk = true;
				}
			}
			out_plcp->usr[0].ru_idx = out_plcp->punc_ch_info == 0 ? 1 : out_plcp->punc_ch_info;
			out_plcp->usr[0].ru_size = out_plcp->punc_ch_info == 0 ? RU996 : RU484_242;
			break;
		case DBW160:
			for (i = 0; i < 13; i++) {
				if (punc_pattern == dbw160_punc_pattern[i]) {
					out_plcp->punc_ch_info = i;
					pattern_chk = true;
				}
			}
			if (out_plcp->punc_ch_info == 0) {
				out_plcp->usr[0].ru_idx = 1;
				out_plcp->usr[0].ru_size = RU996X2;
			} else if (out_plcp->punc_ch_info <= 8) {
				out_plcp->usr[0].ru_idx = out_plcp->punc_ch_info;
				out_plcp->usr[0].ru_size = RU996_484_242;
			} else if (out_plcp->punc_ch_info <= 12) {
				out_plcp->usr[0].ru_idx = out_plcp->punc_ch_info - 8;
				out_plcp->usr[0].ru_size = RU996_484;
			} else {
				BB_DBG(bb, DBG_BIT14, "[EHT punc_ch_info] Invalid puncturing pattern for BW160!\n");
			}
			break;
		case DBW320:
			for (i = 0; i < 13; i++) {
				if (punc_pattern == dbw160_punc_pattern[i]) {
					out_plcp->punc_ch_info = i;
					pattern_chk = true;
				}
			}
			if (out_plcp->punc_ch_info == 0) {
				out_plcp->usr[0].ru_idx = 1;
				out_plcp->usr[0].ru_size = RU996X2;
			} else if (out_plcp->punc_ch_info <= 8) {
				out_plcp->usr[0].ru_idx = out_plcp->punc_ch_info;
				out_plcp->usr[0].ru_size = RU996X3_484;
			} else if (out_plcp->punc_ch_info <= 12) {
				out_plcp->usr[0].ru_idx = out_plcp->punc_ch_info - 8;
				out_plcp->usr[0].ru_size = RU996X3;
			} else if (out_plcp->punc_ch_info <= 18) {
				out_plcp->usr[0].ru_idx = out_plcp->punc_ch_info - 6;
				out_plcp->usr[0].ru_size = RU996X2_484;
			} else if (out_plcp->punc_ch_info <= 24) {
				out_plcp->usr[0].ru_idx = out_plcp->punc_ch_info - 18;
				out_plcp->usr[0].ru_size = RU996X2_484;
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

	BB_DBG(bb, DBG_BIT14, "[EHT punc_ch_info] ru_idx=%d, ru_size=%d\n",
	       out_plcp->usr[0].ru_idx, out_plcp->usr[0].ru_size);

	return pattern_chk;
}

u32 halbb_ru_size_id_2_ru_alloc(struct bb_info *bb, u8 ru_id, u8 ru_size, u8 prim_sb, enum plcp_dbw dbw)
{
	u32 ru_alloc = 0;
//	out_plcp->usr[0].ru_idx = 1;
//	out_plcp->usr[0].ru_size = RU242;
	const u8 BW80_RU_id_max_table[12] = {37, 16, 8, 4, 2, 1, 0, 0, 0, 12, 8, 4};
	const u8 RU_allocation_offset_table[17] = {0, 37, 53, 61, 65, 67, 68, 0, 69, 70, 82, 90, 94, 96, 100, 104, 105};
	const u8 RU996x2_484_allocation_offset_table_specific[12] = {0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3};
	u8 seg_p160_p80, seg_p160_s80, seg_s160_l80, seg_s160_u80;
	bool B_0, PS160;
	u8 loc_80, BW80_RU_id;
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
		if ((ru_size <= RU996) || (ru_size >= RU52_26 && ru_size <= RU484_242))
			BB_WARNING("Illegal ru_size in BW80 !!");
		PS160 = 0;
		B_0 = 0;
		ru_alloc = PS160 * 256 + (RU_allocation_offset_table[ru_size] + ru_id - 1) * 2 + B_0;
	} else if (dbw == DBW160) {
		PS160 = 0;

		if (ru_size == RU996_484) {
			if (ru_id > 0 && ru_id <= 4)
				BB_WARNING("Illegal ru_id when ru_size = RU996_484 in BW160 !!");
			B_0 = B0_RU996_484[ru_id - 1];
			ru_alloc = PS160 * 256 + (RU_allocation_offset_table[ru_size] + (ru_id % 2 == 0 ? 1 : 0)) * 2 + B_0;
		} else if (ru_size == RU996_484_242) {
			if (ru_id > 0 && ru_id <= 8)
				BB_WARNING("Illegal ru_id when ru_size = RU996_484_242 in BW160 !!");
			B_0 = B0_RU996_484_242[ru_id - 1];
			ru_alloc = PS160 * 256 + (RU_allocation_offset_table[ru_size] + (ru_id > 4 ? ru_id - 4 : ru_id) - 1) * 2 + B_0;
		} else if (ru_size == RU996X2) {
			if (ru_id == 1)
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
			if (ru_size <= RU484_242)
				BB_WARNING("Illegal ru_size in BW160 !!");

			loc_80 = (ru_id % BW80_RU_id_max_table[ru_size] == 0) ? (ru_id / BW80_RU_id_max_table[ru_size] - 1) : (ru_id / BW80_RU_id_max_table[ru_size]);

			if (loc_80 >= 0 && loc_80 <= 1)
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
			if (ru_id > 0 && ru_id <= 8)
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
			if (ru_id > 0 && ru_id <= 16)
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
			if (ru_id > 0 && ru_id <= 2)
				BB_WARNING("Illegal ru_id when ru_size = RU996x2 in BW320 !!");

			B_0 = 1;
			PS160 = PS160_RU996x2[ru_id - 1];

			ru_alloc = PS160 * 256 + (RU_allocation_offset_table[ru_size]) * 2 + B_0;
		} else if (ru_size == RU996X2_484) {
			if (ru_id > 0 && ru_id <= 12)
				BB_WARNING("Illegal ru_id when ru_size = RU996x2_484 in BW320 !!");

			B_0 = B0_RU996x2_484[ru_id - 1];
			PS160 = PS160_RU996x2_484[ru_id - 1];

			ru_alloc = PS160 * 256 + (RU_allocation_offset_table[ru_size] + RU996x2_484_allocation_offset_table_specific[ru_id - 1]) * 2 + B_0;
		} else if (ru_size == RU996X3) {
			if (ru_id >0 && ru_id <= 4)
				BB_WARNING("Illegal ru_id when ru_size = RU996x3 in BW320 !!");

			B_0 = B0_RU996x3[ru_id - 1];
			PS160 = PS160_RU996x3[ru_id - 1];

			ru_alloc = PS160 * 256 + (RU_allocation_offset_table[ru_size]) * 2 + B_0;
		} else if (ru_size == RU996X3_484) {
			if (ru_id > 0 && ru_id <= 8)
				BB_WARNING("Illegal ru_id when ru_size = RU996x3_484 in BW320 !!");

			B_0 = B0_RU996x3_484[ru_id - 1];
			PS160 = PS160_RU996x3_484[ru_id - 1];

			ru_alloc = PS160 * 256 + (RU_allocation_offset_table[ru_size] + (ru_id % 2 == 0 ? 1 : 0)) * 2 + B_0;
		} else if (ru_size == RU996X4) {
			if (ru_id == 1)
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

			if (ru_size <= RU484_242)
				BB_WARNING("Illegal ru_size in BW320 !!");

			loc_80 = ru_id % BW80_RU_id_max_table[ru_size] == 0 ? (ru_id / BW80_RU_id_max_table[ru_size] - 1) : (ru_id / BW80_RU_id_max_table[ru_size]);

			if (loc_80 >= 0 && loc_80 <= 3)
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
#endif
void halbb_he_sigb(struct bb_info *bb, struct halbb_plcp_info *in,
		   struct plcp_tx_pre_fec_padding_setting_in_t *in_plcp,
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

	struct bb_plcp_cr_info *cr = &bb->bb_plcp_i.bb_plcp_cr_i;

	u32 n_sym_sigb_ch1_phy0[16] = {cr->he_sigb_ch1_0, cr->he_sigb_ch1_1,
				       cr->he_sigb_ch1_2, cr->he_sigb_ch1_3,
				       cr->he_sigb_ch1_4, cr->he_sigb_ch1_5,
				       cr->he_sigb_ch1_6, cr->he_sigb_ch1_7,
				       cr->he_sigb_ch1_8, cr->he_sigb_ch1_9,
				       cr->he_sigb_ch1_10, cr->he_sigb_ch1_11,
				       cr->he_sigb_ch1_12, cr->he_sigb_ch1_13,
				       cr->he_sigb_ch1_14, cr->he_sigb_ch1_15};
	u32 n_sym_sigb_ch2_phy0[16] = {cr->he_sigb_ch2_0, cr->he_sigb_ch2_1,
				       cr->he_sigb_ch2_2, cr->he_sigb_ch2_3,
				       cr->he_sigb_ch2_4, cr->he_sigb_ch2_5,
				       cr->he_sigb_ch2_6, cr->he_sigb_ch2_7,
				       cr->he_sigb_ch2_8, cr->he_sigb_ch2_9,
				       cr->he_sigb_ch2_10, cr->he_sigb_ch2_11,
				       cr->he_sigb_ch2_12, cr->he_sigb_ch2_13,
				       cr->he_sigb_ch2_14, cr->he_sigb_ch2_15};

	for (i = 0; i < 16; i++) {
		halbb_set_reg(bb, n_sym_sigb_ch1_phy0[i], MASKDWORD, 0);
		halbb_set_reg(bb, n_sym_sigb_ch2_phy0[i], MASKDWORD, 0);
	}

	if (phy_idx == HW_PHY_0) {
		for (i = 0; i < 16; i++) {
			he_sigb->n_sym_sig_ch1[i].address_0 = (u8)(n_sym_sigb_ch1_phy0[i] & 0xff);
			he_sigb->n_sym_sig_ch1[i].address_1 = (u8)((n_sym_sigb_ch1_phy0[i] & 0xff00) >> 8);
			he_sigb->n_sym_sig_ch1[i].address_2 = (u8)((n_sym_sigb_ch1_phy0[i] & 0xff0000) >> 16);
			he_sigb->n_sym_sig_ch1[i].address_3 = (u8)((n_sym_sigb_ch1_phy0[i] & 0xff000000) >> 24);

			he_sigb->n_sym_sig_ch2[i].address_0 = (u8)(n_sym_sigb_ch2_phy0[i] & 0xff);
			he_sigb->n_sym_sig_ch2[i].address_1 = (u8)((n_sym_sigb_ch2_phy0[i] & 0xff00) >> 8);
			he_sigb->n_sym_sig_ch2[i].address_2 = (u8)((n_sym_sigb_ch2_phy0[i] & 0xff0000) >> 16);
			he_sigb->n_sym_sig_ch2[i].address_3 = (u8)((n_sym_sigb_ch2_phy0[i] & 0xff000000) >> 24);
		}
	} /*else {
		for (i = 0; i < 16; i++) {
			he_sigb->n_sym_sigb_ch1[i] = n_sym_sigb_ch1_phy1[i];
			he_sigb->n_sym_sigb_ch2[i] = n_sym_sigb_ch2_phy1[i];
		}
	}*/

	cmdlen = sizeof(struct bb_h2c_ehtsig_sigb);

	he_sigb->ehtsig_sigb = false;

	he_sigb->ehtsig_sigb_i.ppdu_bw = (u8)in->dbw;
	he_sigb->ehtsig_sigb_i.sta_list_num = (u8)in->n_user;

	for (i = 0; i < in->n_user; i++) {
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
		he_sigb_pol = (bool)halbb_get_reg(bb, 0xfc, BIT(16));
		if (he_sigb_pol) {
			he_sigb_valid = (bool)halbb_get_reg(bb, 0xfc, BIT(8));
			he_n_sigb_sym = (u16)halbb_get_reg(bb, 0xfc, 0x3f);
			in_plcp->n_hesigb_sym = he_n_sigb_sym;
			break;
		}
	}
}
#if 0 // BE_IC_TYPE
void halbb_eht_sig(struct bb_info *bb, struct halbb_plcp_info *in,
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
	u8 nss_max = 0;
	u32 ru_alloc = 0;
	u32 ch20_with_data = halbb_ru_occupied_sub20_eht(bb, in);
	struct bb_plcp_cr_info *cr = &bb->bb_plcp_i.bb_plcp_cr_i;

	// [TBD] EHT-SIG CR need to be modified based on CR document
	u32 n_sym_sig_ch1_phy0[16] = {cr->he_sigb_ch1_0, cr->he_sigb_ch1_1,
				       cr->he_sigb_ch1_2, cr->he_sigb_ch1_3,
				       cr->he_sigb_ch1_4, cr->he_sigb_ch1_5,
				       cr->he_sigb_ch1_6, cr->he_sigb_ch1_7,
				       cr->he_sigb_ch1_8, cr->he_sigb_ch1_9,
				       cr->he_sigb_ch1_10, cr->he_sigb_ch1_11,
				       cr->he_sigb_ch1_12, cr->he_sigb_ch1_13,
				       cr->he_sigb_ch1_14, cr->he_sigb_ch1_15};
	u32 n_sym_sig_ch2_phy0[16] = {cr->he_sigb_ch2_0, cr->he_sigb_ch2_1,
				       cr->he_sigb_ch2_2, cr->he_sigb_ch2_3,
				       cr->he_sigb_ch2_4, cr->he_sigb_ch2_5,
				       cr->he_sigb_ch2_6, cr->he_sigb_ch2_7,
				       cr->he_sigb_ch2_8, cr->he_sigb_ch2_9,
				       cr->he_sigb_ch2_10, cr->he_sigb_ch2_11,
				       cr->he_sigb_ch2_12, cr->he_sigb_ch2_13,
				       cr->he_sigb_ch2_14, cr->he_sigb_ch2_15};

	if (in->ppdu_type >= EHT_TB_FMT)
		return;

	for (i = 0; i < 16; i++) {
		halbb_set_reg(bb, n_sym_sig_ch1_phy0[i], MASKDWORD, 0);
		halbb_set_reg(bb, n_sym_sig_ch2_phy0[i], MASKDWORD, 0);
	}

	if (phy_idx == HW_PHY_0) {
		for (i = 0; i < 16; i++) {
			eht_sig->n_sym_sig_ch1[i].address_0 = (u8)(n_sym_sig_ch1_phy0[i] & 0xff);
			eht_sig->n_sym_sig_ch1[i].address_1 = (u8)((n_sym_sig_ch1_phy0[i] & 0xff00) >> 8);
			eht_sig->n_sym_sig_ch1[i].address_2 = (u8)((n_sym_sig_ch1_phy0[i] & 0xff0000) >> 16);
			eht_sig->n_sym_sig_ch1[i].address_3 = (u8)((n_sym_sig_ch1_phy0[i] & 0xff000000) >> 24);

			eht_sig->n_sym_sig_ch2[i].address_0 = (u8)(n_sym_sig_ch2_phy0[i] & 0xff);
			eht_sig->n_sym_sig_ch2[i].address_1 = (u8)((n_sym_sig_ch2_phy0[i] & 0xff00) >> 8);
			eht_sig->n_sym_sig_ch2[i].address_2 = (u8)((n_sym_sig_ch2_phy0[i] & 0xff0000) >> 16);
			eht_sig->n_sym_sig_ch2[i].address_3 = (u8)((n_sym_sig_ch2_phy0[i] & 0xff000000) >> 24);
		}
	} /*else {
		for (i = 0; i < 16; i++) {
			he_sigb->n_sym_sigb_ch1[i] = n_sym_sigb_ch1_phy1[i];
			he_sigb->n_sym_sigb_ch2[i] = n_sym_sigb_ch2_phy1[i];
		}
	}*/

	cmdlen = sizeof(struct bb_h2c_ehtsig_sigb);

	eht_sig->ehtsig_sigb = true;

	eht_sig->ehtsig_sigb_i.ppdu_bw = (u8)in->dbw;
	eht_sig->ehtsig_sigb_i.sta_list_num = (u8)in->n_user;
	/*-----------------------------------------------------------------------*/
	eht_sig->ehtsig_sigb_i.ch20_with_data_0 = (u8)(ch20_with_data & 0xff);
	eht_sig->ehtsig_sigb_i.ch20_with_data_1 = (u8)((ch20_with_data & 0xff00) >> 8);
	eht_sig->ehtsig_sigb_i.pri_txsb = bb->bb_plcp_i.prim_sb;
	eht_sig->ehtsig_sigb_i.ul_dl = (u8)in->ul_flag;
	eht_sig->ehtsig_sigb_i.ppdu_type_comp_mode = (u8)out->ppdu_type_comp_mode;
	eht_sig->ehtsig_sigb_i.usig_spat_reuse = (u8)in->ul_srp1;
	eht_sig->ehtsig_sigb_i.usig_ltf_symb = nss_max == 1 ? 1 : (u8)(halbb_ceil (nss_max, 2) * 2);
	eht_sig->ehtsig_sigb_i.usig_nss = 0;
	eht_sig->ehtsig_sigb_i.usig_bf = 0;
	eht_sig->ehtsig_sigb_i.usig_spat_gi_ltf = halbb_eht_sig_gi_ltf_tbl(bb, (u8)in->gi, (u8)in->he_ltf_type);
	eht_sig->ehtsig_sigb_i.usig_disregard_ndp = 1;
	eht_sig->ehtsig_sigb_i.usig_ldpc_extra_symb_seg = (u8)out->ldpc_extra;
	eht_sig->ehtsig_sigb_i.usig_prefec = (u8)out->pre_fec_padding_factor;
	eht_sig->ehtsig_sigb_i.usig_pe_disambiguity = (u8)out->disamb;
	eht_sig->ehtsig_sigb_i.usig_disregard = 1;

	for (i = 0; i < in->n_user; i++) {
		eht_sig->ehtsig_sigb_i.usr_info[i].coding = (u8)in->usr[i].fec;
		eht_sig->ehtsig_sigb_i.usr_info[i].nsts = (u8)(in->usr[i].nss << in->stbc) - 1;
		eht_sig->ehtsig_sigb_i.usr_info[i].ru_position = (u8)in->usr[i].ru_alloc;
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
		eht_sig_pol = (bool)halbb_get_reg(bb, 0xfc, BIT(16));
		if (eht_sig_pol) {
			eht_sig_valid = (bool)halbb_get_reg(bb, 0xfc, BIT(8));
			eht_sig_n_sym = (u16)halbb_get_reg(bb, 0xfc, 0x3f);
			// Shared para. with n_ehtsig_sym
			in_plcp->n_hesigb_sym = eht_sig_n_sym;
			break;
		}
	}
}
#endif
bool halbb_ru_info_init(struct bb_info *bb, struct halbb_plcp_info *in,
			struct plcp_tx_pre_fec_padding_setting_in_t *in_plcp,
			struct plcp_tx_pre_fec_padding_setting_out_t *out,
			enum phl_phy_idx phy_idx)
{
	u8 i = 0;
	bool invalid_chk = false;
#if 0 // BE_IC_TYPE
	if (bb->ic_type >= BB_RTL8922A) { // [BE]
		halbb_ppdu_type_comp_mode_trans(bb, (bool)in->ul_flag, in->ppdu_type, out);
		invalid_chk = halbb_punc_ch_info_2_ru_size_idx(bb, in->dbw, (u8)out->ppdu_type_comp_mode, (u8)in->punc_pattern, out);
		for (i = 0; i < in->n_user; i++)
			in_plcp->usr[i].ru_size_idx = in->ppdu_type >= EHT_MU_SU_FMT ? (enum ru_sizes_list) out->usr[i].ru_size : (enum ru_sizes_list) in->usr[i].ru_size;
	} else { // [AX]
#endif
		if (in->ppdu_type == HE_SU_FMT) { //HE_SU
			if (in->dbw == 0)
				in->usr[0].ru_alloc = 122;
			else if (in->dbw == 1)
				in->usr[0].ru_alloc = 130;
			else if (in->dbw == 2)
				in->usr[0].ru_alloc = 134;
			else
				in->usr[0].ru_alloc = 137;
		} else if (in->ppdu_type == HE_ER_SU_FMT) { //HE_ER_SU
			if (in->he_er_u106ru_en)
				in->usr[0].ru_alloc = 108;
			else
				in->usr[0].ru_alloc = 122;
		}

		if (in->ppdu_type == HE_TB_FMT) {
			in->n_user = 1;
			// === Set ru_size_idx === //
			if((in->usr[0].ru_alloc >> 1) < 37)
				in_plcp->usr[0].ru_size_idx = 0;
			else if((in->usr[0].ru_alloc >> 1) < 53)
				in_plcp->usr[0].ru_size_idx = 1;
			else if((in->usr[0].ru_alloc >> 1) < 61)
				in_plcp->usr[0].ru_size_idx = 2;
			else if((in->usr[0].ru_alloc >> 1) < 65)
				in_plcp->usr[0].ru_size_idx = 3;
			else if((in->usr[0].ru_alloc >> 1) < 67)
				in_plcp->usr[0].ru_size_idx = 4;
			else
				in_plcp->usr[0].ru_size_idx = 5;
		}
		invalid_chk = true;
#if 0 // BE_IC_TYPE
	}
#endif
	// HE SIG-B
	if (in->ppdu_type == HE_MU_FMT) {
		halbb_he_sigb(bb, in, in_plcp, phy_idx);

		for (i = 0; i < in->n_user; i++) {
			// === Set ru_size_idx === //
			if((in->usr[i].ru_alloc >> 1) < 37)
				in_plcp->usr[i].ru_size_idx = 0;
			else if((in->usr[i].ru_alloc >> 1) < 53)
				in_plcp->usr[i].ru_size_idx = 1;
			else if((in->usr[i].ru_alloc >> 1) < 61)
				in_plcp->usr[i].ru_size_idx = 2;
			else if((in->usr[i].ru_alloc >> 1) < 65)
				in_plcp->usr[i].ru_size_idx = 3;
			else if((in->usr[i].ru_alloc >> 1) < 67)
				in_plcp->usr[i].ru_size_idx = 4;
			else
				in_plcp->usr[i].ru_size_idx = 5;
			BB_DBG(bb, DBG_BIT14, "[SIGB] User%d RU_alloc = %d\n", i, in->usr[i].ru_alloc);
		}
	}

	return invalid_chk;
}

void halbb_plcp_gen_init(struct bb_info *bb, struct halbb_plcp_info *in,
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

	for (i = 0; i < in->n_user; i++) {
		in->usr[i].mpdu_len = 0;
		in->usr[i].n_mpdu = 0;
		in->usr[i].txbf = 0;
		in->usr[i].scrambler_seed = 0x81;
		in->usr[i].random_init_seed = (bb->ic_type == BB_RTL8852A) ? 0x49 : 0x4b;
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
	}
}

void halbb_plcp_lsig(struct bb_info *bb, struct halbb_plcp_info *in,
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

void halbb_plcp_siga(struct bb_info *bb, struct halbb_plcp_info *in,
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
#if 0 // BE_IC_TYPE
	if ((in->ppdu_type >= EHT_MU_SU_FMT) && (in->ppdu_type <= EHT_MU_RU_FMT)) {
		/*=== U-SIG-1 ===*/
		halbb_set_bit(0, 3, 0, &siga1);
		halbb_set_bit(3, 3, in->dbw, &siga1);
		halbb_set_bit(6, 1, in->ul_flag, &siga1);
		halbb_set_bit(7, 6, in->bss_color, &siga1);
		halbb_set_bit(13, 7, in->txop, &siga1);
		halbb_set_bit(20, 5, 0x1f, &siga1);
		halbb_set_bit(25, 1, 1, &siga1);
		/*=== U-SIG-2 ===*/
		halbb_set_bit(0, 2, out_plcp->ppdu_type_comp_mode, &siga2);
		halbb_set_bit(2, 1, 1, &siga2);
		halbb_set_bit(3, 5, out_plcp->punc_ch_info, &siga2);
		halbb_set_bit(8, 1, 1, &siga2);
		halbb_set_bit(9, 2, in->eht_sig_mcs, &siga2);
		halbb_set_bit(11, 5, out_plcp->n_sym_ehtsig, &siga2);
		//CRC4//
		//--- Set U-SIG-1 ---
		for(i = 0; i < 26; i++)
			siga_bits[i] = (siga1 >> i) & 0x1 ;
		//--- Set U-SIG-2 ---
		for(i = 0; i < 16; i++)
			siga_bits[i + 26] = (siga2 >> i) & 0x1 ;
		crc8_out = halbb_set_crc8(bb, siga_bits, 42);
		crc4_out = crc8_out & 0xf;
		halbb_set_bit(16, 4, crc4_out, &siga2);
		halbb_set_bit(20, 6, 0, &siga2);
	} else if (in->ppdu_type >= EHT_TB_FMT) {
		/*=== U-SIG-1 ===*/
		halbb_set_bit(0, 3, 0, &siga1);
		halbb_set_bit(3, 3, in->dbw, &siga1);
		halbb_set_bit(6, 1, in->ul_flag, &siga1);
		halbb_set_bit(7, 6, in->bss_color, &siga1);
		halbb_set_bit(13, 7, in->txop, &siga1);
		halbb_set_bit(20, 6, 0, &siga1);
		/*=== U-SIG-2 ===*/
		halbb_set_bit(0, 2, out_plcp->ppdu_type_comp_mode, &siga2);
		halbb_set_bit(2, 1, 0, &siga2);
		halbb_set_bit(3, 4, in->ul_srp1, &siga2);
		halbb_set_bit(7, 4, in->ul_srp2, &siga2);
		halbb_set_bit(11, 5, 0, &siga2);
		//CRC4//
		//--- Set U-SIG-1 ---
		for(i = 0; i < 26; i++)
			siga_bits[i] = (siga1 >> i) & 0x1 ;
		//--- Set U-SIG-2 ---
		for(i = 0; i < 16; i++)
			siga_bits[i + 26] = (siga2 >> i) & 0x1 ;
		crc8_out = halbb_set_crc8(bb, siga_bits, 42);
		crc4_out = crc8_out & 0xf;
		halbb_set_bit(16, 4, crc4_out, &siga2);
		halbb_set_bit(20, 6, 0, &siga2);
	} else if (in->ppdu_type == EHT_MU_ERSU_FMT) {
		/*=== U-SIG-1 ===*/
		halbb_set_bit(0, 3, 0, &siga1);
		halbb_set_bit(3, 3, in->dbw, &siga1);
		halbb_set_bit(6, 1, in->ul_flag, &siga1);
		halbb_set_bit(7, 6, in->bss_color, &siga1);
		halbb_set_bit(13, 7, in->txop, &siga1);
		halbb_set_bit(20, 6, 0x3f, &siga1);
		/*=== U-SIG-2 ===*/
		halbb_set_bit(0, 16, 0xffff, &siga2);
		halbb_set_bit(2, 1, 0, &siga2);
		halbb_set_bit(3, 4, in->ul_srp1, &siga2);
		halbb_set_bit(7, 4, in->ul_srp2, &siga2);
		halbb_set_bit(11, 5, 0, &siga2);
		//CRC4//
		//--- Set U-SIG-1 ---
		for(i = 0; i < 26; i++)
			siga_bits[i] = (siga1 >> i) & 0x1 ;
		//--- Set U-SIG-2 ---
		for(i = 0; i < 16; i++)
			siga_bits[i + 26] = (siga2 >> i) & 0x1 ;
		crc8_out = halbb_set_crc8(bb, siga_bits, 42);
		crc4_out = crc8_out & 0xf;
		halbb_set_bit(16, 4, crc4_out, &siga2);
		halbb_set_bit(20, 6, 0, &siga2);
	} else if ((in->ppdu_type == HE_SU_FMT) || (in->ppdu_type == HE_ER_SU_FMT)) { // HE_SU SIG-A //
#else
		if ((in->ppdu_type == HE_SU_FMT) || (in->ppdu_type == HE_ER_SU_FMT)) { // HE_SU SIG-A //
#endif
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
		crc8_out = halbb_set_crc8(bb, siga_bits, 42);
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
		crc8_out = halbb_set_crc8(bb, siga_bits, 42);
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
		crc8_out = halbb_set_crc8(bb, siga_bits, 42);
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
		crc8_out = halbb_set_crc8(bb, siga_bits, 34);
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
		crc8_out = halbb_set_crc8(bb, siga_bits, 34);
		halbb_set_bit(10, 8, crc8_out, &siga2);
		halbb_set_bit(18, 6, 0, &siga2);
	}
	/*=== Write CR ===*/
	halbb_set_reg_cmn(bb, cr->siga1, cr->siga1_m, siga1, phy_idx);
	halbb_set_reg_cmn(bb, cr->siga2, cr->siga2_m, siga2, phy_idx);
}


void halbb_cfg_txinfo(struct bb_info *bb, struct halbb_plcp_info *in,
		      struct plcp_tx_pre_fec_padding_setting_out_t *out_plcp,
	     	      enum phl_phy_idx phy_idx)
{
	u8 txinfo_ppdu = 0;
	u32 ch20_with_data = 0;
	u8 i = 0;
	u32 max_mcs = 0;

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
	halbb_set_reg_cmn(bb, cr->txsc, cr->txsc_m, in->txsc, phy_idx);
	halbb_set_reg_cmn(bb, cr->source_gen_mode_idx, cr->source_gen_mode_idx_m, in->source_gen_mode, phy_idx);

	ch20_with_data = halbb_cfg_ch20_with_data(bb, in);
	halbb_set_reg_cmn(bb, cr->ch20_with_data, cr->ch20_with_data_m, ch20_with_data, phy_idx);

	switch(in->ppdu_type) {
		case B_MODE_FMT: //CCK
			if (in->long_preamble_en)
				txinfo_ppdu = 0;
			else
				txinfo_ppdu = 1;
			break;
		case LEGACY_FMT: //Legacy
			txinfo_ppdu = 2;
			break;
		case HT_MF_FMT: //HT_MF
			txinfo_ppdu = 3;
			break;
		case HT_GF_FMT: //HT_GF
			txinfo_ppdu = 4;
			break;
		case VHT_FMT: //VHT
			txinfo_ppdu = 5;
			break;
		case HE_SU_FMT: //HE_SU
			txinfo_ppdu = 7;
			break;
		case HE_ER_SU_FMT: //HE_ER_SU
			txinfo_ppdu = 8;
			break;
		case HE_MU_FMT: //HE_MU
			txinfo_ppdu = 9;
			break;
		case HE_TB_FMT: //HE_TB
			txinfo_ppdu = 10;
			break;
		default:
			break;
	}
	halbb_set_reg_cmn(bb, cr->ppdu_type, cr->ppdu_type_m, txinfo_ppdu, phy_idx);
	if (in->ppdu_type == B_MODE_FMT)
		halbb_set_reg_cmn(bb, cr->n_usr, cr->n_usr_m, in->n_user, phy_idx);
	else
		halbb_set_reg_cmn(bb, cr->n_usr, cr->n_usr_m, out_plcp->n_usr, phy_idx);

	if ((bb->ic_type != BB_RTL8852A) && (bb->ic_type != BB_RTL8852B) &&
	    (bb->ic_type != BB_RTL8851B)) {
		if (in->ppdu_type == HT_MF_FMT) {
			max_mcs = in->usr[0].mcs % 8;
		} else if (in->ppdu_type == HE_MU_FMT) {
			max_mcs = in->usr[0].mcs;
			for (i = 1; i < N_USER; i++)
				max_mcs = in->usr[i].mcs > max_mcs ? in->usr[i].mcs : max_mcs;
		} else {
			max_mcs = in->usr[0].mcs;
		}
		halbb_set_reg_cmn(bb, cr->max_mcs, cr->max_mcs_m, max_mcs, phy_idx);
	}
}

void halbb_cfg_txctrl(struct bb_info *bb, struct halbb_plcp_info *in,
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
	u32 ru_alloc[4] = {cr->usr0_ru_alloc, cr->usr1_ru_alloc, cr->usr2_ru_alloc,
					   cr->usr3_ru_alloc};
	u32 ru_alloc_m[4] = {cr->usr0_ru_alloc_m, cr->usr1_ru_alloc_m, cr->usr2_ru_alloc_m,
					     cr->usr3_ru_alloc_m};
	u32 txbf_en[4] = {cr->usr0_txbf_en, cr->usr1_txbf_en, cr->usr2_txbf_en,
					  cr->usr3_txbf_en};
	u32 txbf_en_m[4] = {cr->usr0_txbf_en_m, cr->usr1_txbf_en_m, cr->usr2_txbf_en_m,
					    cr->usr3_txbf_en_m};
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

	BB_DBG(bb, DBG_BIT14, "<====== %s ======>\n", __func__);

	// 	Default value //
	//	When HE_TB NDP, it's valid; o.w., it's RSVD and set to 1'b0
	halbb_set_reg_cmn(bb, cr->feedback_status, cr->feedback_status_m, 0, phy_idx);
	//	Whether this PPDU contains data field or not. 0: with data field, 1:without data field 
	halbb_set_reg_cmn(bb, cr->ndp, cr->ndp_m, 0, phy_idx);
	//	it's RSVD except HE PPDU and set to 1'b0 when it's RSVD  0: disable MU-MIMO-LTF-Mode, 1: enable MU-MIMO-LTF-Mode
	halbb_set_reg_cmn(bb, cr->mumimo_ltf_mode_en, cr->mumimo_ltf_mode_en_m, 0, phy_idx);
	//	it's RSVD except VHT_MU and HE_MU. When it's RSVD, it shall be set to 1'b0   0: non-full-bandwidth-MU-MIMO, 1: full-bandwidth-MU-MIMO
	halbb_set_reg_cmn(bb, cr->fb_mumimo_en, cr->fb_mumimo_en_m, 0, phy_idx);

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

	//	When HE_MU, the MCS for HE-SIGB or not; o.w., it's RSVD and set to 3'b0
	if (in->ppdu_type != HE_MU_FMT)
		halbb_set_reg_cmn(bb, cr->he_sigb_mcs, cr->he_sigb_mcs_m, 0, phy_idx);
	else
		halbb_set_reg_cmn(bb, cr->he_sigb_mcs, cr->he_sigb_mcs_m, in->he_mcs_sigb, phy_idx);

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
		halbb_set_reg_cmn(bb, ru_alloc[i], ru_alloc_m[i], 0, phy_idx);
		// Txbf
		if ((bb->ic_type == BB_RTL8852A) ||
		    (bb->ic_type == BB_RTL8852B) ||
		    (bb->ic_type == BB_RTL8851B))
			halbb_set_reg_cmn(bb, txbf_en[i], txbf_en_m[i], 0, phy_idx);
		else
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

		//	For all PPDU excepts HE_SU, HE_ER_SU, HE_MU, HE_TB, are RSVD and shall be set to 8'b0
		if (in->ppdu_type < HE_SU_FMT)
			halbb_set_reg_cmn(bb, ru_alloc[i], ru_alloc_m[i], 0, phy_idx);
		else
			halbb_set_reg_cmn(bb, ru_alloc[i], ru_alloc_m[i], in->usr[i].ru_alloc, phy_idx);
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

	//	The number of data symbols in HE-SIGB. It is RSVD except HE_MU and shall be set to 6'b0
	if (in->ppdu_type != HE_MU_FMT)
		halbb_set_reg_cmn(bb, cr->n_sym_hesigb, cr->n_sym_hesigb_m, 0, phy_idx);
	else
		halbb_set_reg_cmn(bb, cr->n_sym_hesigb, cr->n_sym_hesigb_m, out_plcp->n_sym_hesigb, phy_idx);

	//	It means the number of data symbols in data_field, which the number of midamble symbols are excluded
	halbb_set_reg_cmn(bb, cr->n_sym, cr->n_sym_m, out_plcp->n_sym, phy_idx);
}

void halbb_plcp_delimiter(struct bb_info *bb, struct halbb_plcp_info *in,
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
		crc8_out = halbb_set_crc8(bb, delimiter_crc, 16);
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

void halbb_cfg_cck(struct bb_info *bb, struct halbb_plcp_info *in, enum phl_phy_idx phy_idx)
{
	struct bb_plcp_cr_info *cr = &bb->bb_plcp_i.bb_plcp_cr_i;


	if ((bb->ic_type == BB_RTL8852A) || (bb->ic_type == BB_RTL8852B) ||
	    (bb->ic_type == BB_RTL8851B)) {
		// === 11b_tx_pmac_psdu_byte === //
		halbb_set_reg(bb, cr->b_psdu_byte, cr->b_psdu_byte_m, in->usr[0].apep);
		// === 11b_tx_pmac_psdu_type === //
		halbb_set_reg(bb, cr->b_ppdu_type, cr->b_ppdu_type_m, ~in->long_preamble_en);
		// === 11b_tx_pmac_psdu_rate === //
		halbb_set_reg(bb, cr->b_psdu_rate, cr->b_psdu_rate_m, in->usr[0].mcs);
		// === 11b_tx_pmac_service_bit2 === //
		halbb_set_reg(bb, cr->b_service_bit2, cr->b_service_bit2_m, 1);
		// === 11b_tx_pmac_psdu_header === //
		halbb_set_reg(bb, cr->b_header_0, cr->b_header_0_m, 0x3020100);
		halbb_set_reg(bb, cr->b_header_1, cr->b_header_1_m, 0x7060504);
		halbb_set_reg(bb, cr->b_header_2, cr->b_header_2_m, 0xb0a0908);
		halbb_set_reg(bb, cr->b_header_3, cr->b_header_3_m, 0xf0e0d0c);
		halbb_set_reg(bb, cr->b_header_4, cr->b_header_4_m, 0x13121110);
		halbb_set_reg(bb, cr->b_header_5, cr->b_header_5_m, 0x17161514);
	} else {
		// === 11b_tx_pmac_psdu_byte === //
		halbb_set_reg_cmn(bb, cr->usr0_mdpu_len_byte, cr->usr0_mdpu_len_byte_m, in->usr[0].apep, phy_idx);
		// === 11b_tx_pmac_psdu_type === //
		halbb_set_reg_cmn(bb, cr->ppdu_type, cr->ppdu_type_m, ~in->long_preamble_en, phy_idx);
		// === 11b_tx_pmac_psdu_rate === //
		halbb_set_reg(bb, cr->b_rate_idx, cr->b_rate_idx_m, in->usr[0].mcs);
		// === 11b_tx_pmac_service_bit2 === //
		halbb_set_reg(bb, cr->b_locked_clk_en, cr->b_locked_clk_en_m, 1);
		// === 11b_rx_cca_trig_dly === //
		//halbb_set_reg(bb, 0x2324, 0x3C00000, 0);
		// === cca_rssi_limit_en === //
		//halbb_set_reg(bb, 0x4b74, BIT(30), 0);
		// === 11b_rx_force_ant_cca_en === //
		//halbb_set_reg(bb, 0x2324, BIT(26), 0);
	}
	// === 11b_tx_pmac_carrier_suppress_tx === //
	halbb_set_reg(bb, cr->b_carrier_suppress_tx, cr->b_carrier_suppress_tx_m, 0);
}

void halbb_vht_sigb(struct bb_info *bb, struct halbb_plcp_info *in,
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
			crc8_out = halbb_set_crc8(bb, sigb, 20);
			halbb_set_bit(20, 6, 0x0, &vht_sigb);
			break;
		case 1:
			halbb_set_bit(0, 19, halbb_ceil(out_plcp->usr[0].apep_len, 4), &vht_sigb);
			halbb_set_bit(19, 2, 0x3, &vht_sigb);
			//--- Set VHT SigB ---
			for(i = 0; i < 21; i++)
				sigb[i] = ( vht_sigb >> i ) & 0x1 ;
			crc8_out = halbb_set_crc8(bb, sigb, 21);
			halbb_set_bit(21, 6, 0x0, &vht_sigb);
			break;
		case 2:
		case 3:
			halbb_set_bit(0, 21, halbb_ceil(out_plcp->usr[0].apep_len, 4), &vht_sigb);
			halbb_set_bit(21, 2, 0x3, &vht_sigb);
			//--- Set VHT SigB ---
			for(i = 0; i < 23; i++)
				sigb[i] = ( vht_sigb >> i ) & 0x1 ;
			crc8_out = halbb_set_crc8(bb, sigb, 23);
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

void halbb_service(struct bb_info *bb, struct halbb_plcp_info *in,
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

enum plcp_sts halbb_plcp_gen_6(struct bb_info *bb, struct halbb_plcp_info *in,
		    struct usr_plcp_gen_in *user, enum phl_phy_idx phy_idx)
{
	struct bb_plcp_cr_info *cr = &bb->bb_plcp_i.bb_plcp_cr_i;
	enum plcp_sts tmp = PLCP_SUCCESS;
	struct plcp_tx_pre_fec_padding_setting_in_t in_plcp;
	struct plcp_tx_pre_fec_padding_setting_out_t out;
	//struct _bb_result he_result;
#ifdef HALBB_FW_OFLD_SUPPORT
	halbb_fwofld_cfgcr_start(bb);
#endif
	halbb_mem_cpy(bb, in->usr, user, 4*sizeof(struct usr_plcp_gen_in));

	BB_DBG(bb, DBG_BIT14, "<====== %s ======>\n", __func__);

	halbb_plcp_gen_init(bb, in, &in_plcp);

	if (!halbb_ru_info_init(bb, in, &in_plcp, &out, phy_idx))
		return SPEC_INVALID;
#if 0 // BE_IC_TYPE
	if (bb->ic_type >= BB_RTL8922A)
		halbb_ppdu_var_type_cfg(bb, in, phy_idx);
#endif
	// CCK
	if (in->ppdu_type == B_MODE_FMT) {
		halbb_cfg_cck(bb, in, phy_idx);
		if ((in->usr[0].mcs == 0) && (in->long_preamble_en == 0))
			tmp = CCK_INVALID;
	} else {
#if 0 // BE_IC_TYPE
		if (in->ppdu_type >= EHT_MU_SU_FMT) {
			// Calculate user specific field & EHT_SIG_Nsym (with incorrect common field para.)
			halbb_eht_sig(bb, in, &in_plcp, &out, phy_idx);
			// Calculate PLCP header with EHT_SIG_Nsym
			tmp = halbb_tx_plcp_cal(bb, &in_plcp, &out);
			// Overwrite common field & CRC with correct para. such as pre_fec_padding...
			halbb_eht_sig(bb, in, &in_plcp, &out, phy_idx);
		} else {
#endif			// PLCP calculation
			tmp = halbb_tx_plcp_cal(bb, &in_plcp, &out);
#if 0 // BE_IC_TYPE
		}
#endif		// VHT SIG-B
		if (in->ppdu_type == VHT_FMT)
			halbb_vht_sigb(bb, in, &out, phy_idx);
		else
			halbb_service(bb, in, phy_idx);
		// L-SIG
		halbb_plcp_lsig(bb, in, &out, phy_idx);
		// SIG-A
		if (in->ppdu_type > LEGACY_FMT)
			halbb_plcp_siga(bb, in, &out, phy_idx);
		// Tx Ctrl Info
		halbb_cfg_txctrl(bb, in, &out, phy_idx);
		// Delimiter
		halbb_plcp_delimiter(bb, in, &out, phy_idx);
	}
	// Tx Info
	halbb_cfg_txinfo(bb, in, &out, phy_idx);

	// IC config
	halbb_ic_cfg(bb, in, cr, phy_idx);
#ifdef HALBB_FW_OFLD_SUPPORT
	halbb_fwofld_cfgcr_end(bb);
#endif
// ===================================== //
	return tmp;
}

enum plcp_sts halbb_plcp_gen(struct bb_info *bb, struct halbb_plcp_info *in,
		    struct usr_plcp_gen_in *user, enum phl_phy_idx phy_idx)
{
	enum plcp_sts rpt = SPEC_INVALID;

	switch (bb->bb_80211spec) {
	case BB_AX_IC:
		rpt = halbb_plcp_gen_6(bb, in, user, phy_idx);
		break;
	case BB_BE_IC:
		rpt = halbb_plcp_gen_7(bb, in, user, phy_idx);
		break;
	default:
		break;
	}

	return rpt;
}

void halbb_cr_cfg_plcp_init(struct bb_info *bb)
{
	struct bb_plcp_info *plcp_info = &bb->bb_plcp_i;
	struct bb_plcp_cr_info *cr = &plcp_info->bb_plcp_cr_i;

	switch (bb->cr_type) {

	#ifdef HALBB_COMPILE_AP_SERIES
	case BB_AP:
		cr->b_header_0 = R1B_TX_PMAC_HEADER_0_A;
		cr->b_header_0_m = R1B_TX_PMAC_HEADER_0_A_M;	
		cr->b_header_1 = R1B_TX_PMAC_HEADER_1_A;
		cr->b_header_1_m = R1B_TX_PMAC_HEADER_1_A_M;	
		cr->b_header_2 = R1B_TX_PMAC_HEADER_2_A;
		cr->b_header_2_m = R1B_TX_PMAC_HEADER_2_A_M;	
		cr->b_header_3 = R1B_TX_PMAC_HEADER_3_A;
		cr->b_header_3_m = R1B_TX_PMAC_HEADER_3_A_M;
		cr->b_header_4 = R1B_TX_PMAC_HEADER_4_A;
		cr->b_header_4_m = R1B_TX_PMAC_HEADER_4_A_M;	
		cr->b_header_5 = R1B_TX_PMAC_HEADER_5_A;
		cr->b_header_5_m = R1B_TX_PMAC_HEADER_5_A_M;	
		cr->b_psdu_byte = R1B_TX_PMAC_PSDU_BYTE_A;	
		cr->b_psdu_byte_m = R1B_TX_PMAC_PSDU_BYTE_A_M;
		cr->b_carrier_suppress_tx = R1B_TX_PMAC_CARRIER_SUPPRESS_TX_A;	
		cr->b_carrier_suppress_tx_m = R1B_TX_PMAC_CARRIER_SUPPRESS_TX_A_M;
		cr->b_ppdu_type	= R1B_TX_PMAC_PPDU_TYPE_A;	
		cr->b_ppdu_type_m = R1B_TX_PMAC_PPDU_TYPE_A_M;	
		cr->b_psdu_rate	= R1B_TX_PMAC_PSDU_RATE_A;	
		cr->b_psdu_rate_m = R1B_TX_PMAC_PSDU_RATE_A_M;	
		cr->b_service_bit2 = R1B_TX_PMAC_SERVICE_BIT2_A;	
		cr->b_service_bit2_m = R1B_TX_PMAC_SERVICE_BIT2_A_M;	
		cr->he_sigb_ch1_0 = TXD_HE_SIGB_CH1_0_A;		
		cr->he_sigb_ch1_0_m = TXD_HE_SIGB_CH1_0_A_M;	
		cr->he_sigb_ch1_1 = TXD_HE_SIGB_CH1_1_A;		
		cr->he_sigb_ch1_1_m = TXD_HE_SIGB_CH1_1_A_M;	
		cr->he_sigb_ch1_10 = TXD_HE_SIGB_CH1_10_A;		
		cr->he_sigb_ch1_10_m = TXD_HE_SIGB_CH1_10_A_M;
		cr->he_sigb_ch1_11 = TXD_HE_SIGB_CH1_11_A;		
		cr->he_sigb_ch1_11_m = TXD_HE_SIGB_CH1_11_A_M;
		cr->he_sigb_ch1_12 = TXD_HE_SIGB_CH1_12_A;	
		cr->he_sigb_ch1_12_m = TXD_HE_SIGB_CH1_12_A_M;
		cr->he_sigb_ch1_13 = TXD_HE_SIGB_CH1_13_A;	
		cr->he_sigb_ch1_13_m = TXD_HE_SIGB_CH1_13_A_M;
		cr->he_sigb_ch1_14 = TXD_HE_SIGB_CH1_14_A;	
		cr->he_sigb_ch1_14_m = TXD_HE_SIGB_CH1_14_A_M;
		cr->he_sigb_ch1_15 = TXD_HE_SIGB_CH1_15_A;		
		cr->he_sigb_ch1_15_m = TXD_HE_SIGB_CH1_15_A_M;
		cr->he_sigb_ch1_2 = TXD_HE_SIGB_CH1_2_A;	
		cr->he_sigb_ch1_2_m = TXD_HE_SIGB_CH1_2_A_M;
		cr->he_sigb_ch1_3 = TXD_HE_SIGB_CH1_3_A;	
		cr->he_sigb_ch1_3_m = TXD_HE_SIGB_CH1_3_A_M;
		cr->he_sigb_ch1_4 = TXD_HE_SIGB_CH1_4_A;	
		cr->he_sigb_ch1_4_m = TXD_HE_SIGB_CH1_4_A_M;
		cr->he_sigb_ch1_5 = TXD_HE_SIGB_CH1_5_A;	
		cr->he_sigb_ch1_5_m = TXD_HE_SIGB_CH1_5_A_M;
		cr->he_sigb_ch1_6 = TXD_HE_SIGB_CH1_6_A;	
		cr->he_sigb_ch1_6_m = TXD_HE_SIGB_CH1_6_A_M;
		cr->he_sigb_ch1_7 = TXD_HE_SIGB_CH1_7_A;	
		cr->he_sigb_ch1_7_m = TXD_HE_SIGB_CH1_7_A_M;
		cr->he_sigb_ch1_8 = TXD_HE_SIGB_CH1_8_A;	
		cr->he_sigb_ch1_8_m = TXD_HE_SIGB_CH1_8_A_M;
		cr->he_sigb_ch1_9 = TXD_HE_SIGB_CH1_9_A;	
		cr->he_sigb_ch1_9_m = TXD_HE_SIGB_CH1_9_A_M;
		cr->he_sigb_ch2_0 = TXD_HE_SIGB_CH2_0_A;	
		cr->he_sigb_ch2_0_m = TXD_HE_SIGB_CH2_0_A_M;
		cr->he_sigb_ch2_1 = TXD_HE_SIGB_CH2_1_A;	
		cr->he_sigb_ch2_1_m = TXD_HE_SIGB_CH2_1_A_M;
		cr->he_sigb_ch2_10 = TXD_HE_SIGB_CH2_10_A;	
		cr->he_sigb_ch2_10_m = TXD_HE_SIGB_CH2_10_A_M;
		cr->he_sigb_ch2_11 = TXD_HE_SIGB_CH2_11_A;
		cr->he_sigb_ch2_11_m = TXD_HE_SIGB_CH2_11_A_M;
		cr->he_sigb_ch2_12 = TXD_HE_SIGB_CH2_12_A;
		cr->he_sigb_ch2_12_m = TXD_HE_SIGB_CH2_12_A_M;
		cr->he_sigb_ch2_13 = TXD_HE_SIGB_CH2_13_A;
		cr->he_sigb_ch2_13_m = TXD_HE_SIGB_CH2_13_A_M;
		cr->he_sigb_ch2_14 = TXD_HE_SIGB_CH2_14_A;	
		cr->he_sigb_ch2_14_m = TXD_HE_SIGB_CH2_14_A_M;
		cr->he_sigb_ch2_15 = TXD_HE_SIGB_CH2_15_A;		
		cr->he_sigb_ch2_15_m = TXD_HE_SIGB_CH2_15_A_M;
		cr->he_sigb_ch2_2 = TXD_HE_SIGB_CH2_2_A;	
		cr->he_sigb_ch2_2_m = TXD_HE_SIGB_CH2_2_A_M;	
		cr->he_sigb_ch2_3 = TXD_HE_SIGB_CH2_3_A;	
		cr->he_sigb_ch2_3_m = TXD_HE_SIGB_CH2_3_A_M;
		cr->he_sigb_ch2_4 = TXD_HE_SIGB_CH2_4_A;	
		cr->he_sigb_ch2_4_m = TXD_HE_SIGB_CH2_4_A_M;
		cr->he_sigb_ch2_5 = TXD_HE_SIGB_CH2_5_A;
		cr->he_sigb_ch2_5_m = TXD_HE_SIGB_CH2_5_A_M;
		cr->he_sigb_ch2_6 = TXD_HE_SIGB_CH2_6_A;
		cr->he_sigb_ch2_6_m = TXD_HE_SIGB_CH2_6_A_M;
		cr->he_sigb_ch2_7 = TXD_HE_SIGB_CH2_7_A;		
		cr->he_sigb_ch2_7_m = TXD_HE_SIGB_CH2_7_A_M;
		cr->he_sigb_ch2_8 = TXD_HE_SIGB_CH2_8_A;
		cr->he_sigb_ch2_8_m = TXD_HE_SIGB_CH2_8_A_M;
		cr->he_sigb_ch2_9 = TXD_HE_SIGB_CH2_9_A;
		cr->he_sigb_ch2_9_m = TXD_HE_SIGB_CH2_9_A_M;
		cr->usr0_delmter = USER0_DELMTER_A;	
		cr->usr0_delmter_m = USER0_DELMTER_A_M;	
		cr->usr0_eof_padding_len = USER0_EOF_PADDING_LEN_A;
		cr->usr0_eof_padding_len_m = USER0_EOF_PADDING_LEN_A_M;
		cr->usr0_init_seed = USER0_INIT_SEED_A;		
		cr->usr0_init_seed_m = USER0_INIT_SEED_A_M;	
		cr->usr1_delmter = USER1_DELMTER_A;	
		cr->usr1_delmter_m = USER1_DELMTER_A_M;
		cr->usr1_eof_padding_len = USER1_EOF_PADDING_LEN_A;	
		cr->usr1_eof_padding_len_m = USER1_EOF_PADDING_LEN_A_M;
		cr->usr1_init_seed = USER1_INIT_SEED_A;
		cr->usr1_init_seed_m = USER1_INIT_SEED_A_M;
		cr->usr2_delmter = USER2_DELMTER_A;
		cr->usr2_delmter_m = USER2_DELMTER_A_M;
		cr->usr2_eof_padding_len = USER2_EOF_PADDING_LEN_A;	
		cr->usr2_eof_padding_len_m = USER2_EOF_PADDING_LEN_A_M;
		cr->usr2_init_seed = USER2_INIT_SEED_A;
		cr->usr2_init_seed_m = USER2_INIT_SEED_A_M;
		cr->usr3_delmter = USER3_DELMTER_A;	
		cr->usr3_delmter_m = USER3_DELMTER_A_M;
		cr->usr3_eof_padding_len = USER3_EOF_PADDING_LEN_A;
		cr->usr3_eof_padding_len_m = USER3_EOF_PADDING_LEN_A_M;
		cr->usr3_init_seed = USER3_INIT_SEED_A;
		cr->usr3_init_seed_m = USER3_INIT_SEED_A_M;
		cr->vht_sigb0 = TXD_VHT_SIGB0_A;	
		cr->vht_sigb0_m	= TXD_VHT_SIGB0_A_M;
		cr->vht_sigb1 = TXD_VHT_SIGB1_A;	
		cr->vht_sigb1_m	= TXD_VHT_SIGB1_A_M;
		cr->vht_sigb2 = TXD_VHT_SIGB2_A;	
		cr->vht_sigb2_m	= TXD_VHT_SIGB2_A_M;
		cr->he_sigb_mcs = TXCOMCT_HE_SIGB_MCS_A;
		cr->he_sigb_mcs_m = TXCOMCT_HE_SIGB_MCS_A_M;
		cr->vht_sigb3 = TXD_VHT_SIGB3_A;
		cr->vht_sigb3_m = TXD_VHT_SIGB3_A_M;
		cr->n_ltf = TXCOMCT_N_LTF_A;
		cr->n_ltf_m = TXCOMCT_N_LTF_A_M;
		cr->siga1 = TXD_SIGA1_A;	
		cr->siga1_m = TXD_SIGA1_A_M;
		cr->siga2 = TXD_SIGA2_A;	
		cr->siga2_m = TXD_SIGA2_A_M;
		cr->lsig = TXD_LSIG_A;	
		cr->lsig_m = TXD_LSIG_A_M;
		cr->cca_pw_th = TXINFO_CCA_PW_TH_A;
		cr->cca_pw_th_m	= TXINFO_CCA_PW_TH_A_M;
		cr->n_sym = TXTIMCT_N_SYM_A;	
		cr->n_sym_m = TXTIMCT_N_SYM_A_M;
		cr->usr0_service = USER0_SERVICE_A;		
		cr->usr0_service_m = USER0_SERVICE_A_M;	
		cr->usr1_service = USER1_SERVICE_A;		
		cr->usr1_service_m = USER1_SERVICE_A_M;	
		cr->usr2_service = USER2_SERVICE_A;		
		cr->usr2_service_m = USER2_SERVICE_A_M;
		cr->usr3_service = USER3_SERVICE_A;	
		cr->usr3_service_m = USER3_SERVICE_A_M;	
		cr->usr0_mdpu_len_byte = USER0_MDPU_LEN_BYTE_A;
		cr->usr0_mdpu_len_byte_m = USER0_MDPU_LEN_BYTE_A_M;	
		cr->usr1_mdpu_len_byte = USER1_MDPU_LEN_BYTE_A;	
		cr->usr1_mdpu_len_byte_m = USER1_MDPU_LEN_BYTE_A_M;	
		cr->obw_cts2self_dup_type = TXINFO_OBW_CTS2SELF_DUP_TYPE_A;	
		cr->obw_cts2self_dup_type_m = TXINFO_OBW_CTS2SELF_DUP_TYPE_A_M;		
		cr->usr2_mdpu_len_byte = USER2_MDPU_LEN_BYTE_A;
		cr->usr2_mdpu_len_byte_m = USER2_MDPU_LEN_BYTE_A_M;
		cr->usr3_mdpu_len_byte = USER3_MDPU_LEN_BYTE_A;
		cr->usr3_mdpu_len_byte_m = USER3_MDPU_LEN_BYTE_A_M;
		cr->usr0_csi_buf_id = TXUSRCT0_CSI_BUF_ID_A;
		cr->usr0_csi_buf_id_m = TXUSRCT0_CSI_BUF_ID_A_M;
		cr->usr1_csi_buf_id = TXUSRCT1_CSI_BUF_ID_A;
		cr->usr1_csi_buf_id_m = TXUSRCT1_CSI_BUF_ID_A_M;
		cr->rf_gain_idx	= TXINFO_RF_GAIN_IDX_A;	
		cr->rf_gain_idx_m = TXINFO_RF_GAIN_IDX_A_M;
		cr->usr2_csi_buf_id = TXUSRCT2_CSI_BUF_ID_A;
		cr->usr2_csi_buf_id_m = TXUSRCT2_CSI_BUF_ID_A_M;
		cr->usr3_csi_buf_id = TXUSRCT3_CSI_BUF_ID_A;
		cr->usr3_csi_buf_id_m = TXUSRCT3_CSI_BUF_ID_A_M;
		cr->usr0_n_mpdu	= USER0_N_MPDU_A;	
		cr->usr0_n_mpdu_m = USER0_N_MPDU_A_M;	
		cr->usr1_n_mpdu	= USER1_N_MPDU_A;	
		cr->usr1_n_mpdu_m = USER1_N_MPDU_A_M;	
		cr->usr2_n_mpdu	= USER2_N_MPDU_A;	
		cr->usr2_n_mpdu_m = USER2_N_MPDU_A_M;
		cr->usr0_pw_boost_fctr_db = TXUSRCT0_PW_BOOST_FCTR_DB_A;	
		cr->usr0_pw_boost_fctr_db_m = TXUSRCT0_PW_BOOST_FCTR_DB_A_M;	
		cr->usr3_n_mpdu = USER3_N_MPDU_A;
		cr->usr3_n_mpdu_m = USER3_N_MPDU_A_M;		
		cr->ch20_with_data = TXINFO_CH20_WITH_DATA_A;	
		cr->ch20_with_data_m = TXINFO_CH20_WITH_DATA_A_M;
		cr->n_usr = TXINFO_N_USR_A;	
		cr->n_usr_m = TXINFO_N_USR_A_M;	
		cr->txcmd_txtp = TXINFO_TXCMD_TXTP_A;
		cr->txcmd_txtp_m = TXINFO_TXCMD_TXTP_A_M;
		cr->usr0_ru_alloc = TXUSRCT0_RU_ALLOC_A;	
		cr->usr0_ru_alloc_m = TXUSRCT0_RU_ALLOC_A_M;
		cr->usr0_u_id = TXUSRCT0_U_ID_A;
		cr->usr0_u_id_m	= TXUSRCT0_U_ID_A_M;
		cr->usr1_ru_alloc = TXUSRCT1_RU_ALLOC_A;	
		cr->usr1_ru_alloc_m = TXUSRCT1_RU_ALLOC_A_M;
		cr->usr1_u_id = TXUSRCT1_U_ID_A;		
		cr->usr1_u_id_m	= TXUSRCT1_U_ID_A_M;
		cr->usr2_ru_alloc = TXUSRCT2_RU_ALLOC_A;
		cr->usr2_ru_alloc_m = TXUSRCT2_RU_ALLOC_A_M;
		cr->usr2_u_id = TXUSRCT2_U_ID_A;
		cr->usr2_u_id_m	= TXUSRCT2_U_ID_A_M;
		cr->usr3_ru_alloc = TXUSRCT3_RU_ALLOC_A;
		cr->usr3_ru_alloc_m = TXUSRCT3_RU_ALLOC_A_M;
		cr->usr3_u_id = TXUSRCT3_U_ID_A;
		cr->usr3_u_id_m	= TXUSRCT3_U_ID_A_M;
		cr->n_sym_hesigb = TXTIMCT_N_SYM_HESIGB_A;	
		cr->n_sym_hesigb_m = TXTIMCT_N_SYM_HESIGB_A_M;	
		cr->usr0_mcs = TXUSRCT0_MCS_A;
		cr->usr0_mcs_m	= TXUSRCT0_MCS_A_M;
		cr->usr1_mcs = TXUSRCT1_MCS_A;
		cr->usr1_mcs_m	= TXUSRCT1_MCS_A_M;
		cr->usr2_mcs = TXUSRCT2_MCS_A;
		cr->usr2_mcs_m = TXUSRCT2_MCS_A_M;
		cr->usr3_mcs = TXUSRCT3_MCS_A;
		cr->usr3_mcs_m = TXUSRCT3_MCS_A_M;
		cr->usr1_pw_boost_fctr_db = TXUSRCT1_PW_BOOST_FCTR_DB_A;
		cr->usr1_pw_boost_fctr_db_m = TXUSRCT1_PW_BOOST_FCTR_DB_A_M;
		cr->usr2_pw_boost_fctr_db = TXUSRCT2_PW_BOOST_FCTR_DB_A;
		cr->usr2_pw_boost_fctr_db_m = TXUSRCT2_PW_BOOST_FCTR_DB_A_M;
		cr->usr3_pw_boost_fctr_db = TXUSRCT3_PW_BOOST_FCTR_DB_A;
		cr->usr3_pw_boost_fctr_db_m = TXUSRCT3_PW_BOOST_FCTR_DB_A_M;
		cr->ppdu_type = TXINFO_PPDU_TYPE_A;
		cr->ppdu_type_m	= TXINFO_PPDU_TYPE_A_M;
		cr->txsc = TXINFO_TXSC_A;	
		cr->txsc_m = TXINFO_TXSC_A_M;
		cr->cfo_comp = TXINFO_CFO_COMP_A;
		cr->cfo_comp_m = TXINFO_CFO_COMP_A_M;
		cr->pkt_ext_idx = TXTIMCT_PKT_EXT_IDX_A;
		cr->pkt_ext_idx_m = TXTIMCT_PKT_EXT_IDX_A_M;	
		cr->usr0_n_sts = TXUSRCT0_N_STS_A;	
		cr->usr0_n_sts_m = TXUSRCT0_N_STS_A_M;
		cr->usr0_n_sts_ru_tot = TXUSRCT0_N_STS_RU_TOT_A;
		cr->usr0_n_sts_ru_tot_m = TXUSRCT0_N_STS_RU_TOT_A_M;
		cr->usr0_strt_sts = TXUSRCT0_STRT_STS_A;
		cr->usr0_strt_sts_m = TXUSRCT0_STRT_STS_A_M;
		cr->usr1_n_sts = TXUSRCT1_N_STS_A;
		cr->usr1_n_sts_m = TXUSRCT1_N_STS_A_M;
		cr->usr1_n_sts_ru_tot = TXUSRCT1_N_STS_RU_TOT_A;
		cr->usr1_n_sts_ru_tot_m = TXUSRCT1_N_STS_RU_TOT_A_M;
		cr->usr1_strt_sts = TXUSRCT1_STRT_STS_A;	
		cr->usr1_strt_sts_m = TXUSRCT1_STRT_STS_A_M;
		cr->usr2_n_sts = TXUSRCT2_N_STS_A;
		cr->usr2_n_sts_m = TXUSRCT2_N_STS_A_M;
		cr->usr2_n_sts_ru_tot = TXUSRCT2_N_STS_RU_TOT_A;
		cr->usr2_n_sts_ru_tot_m	= TXUSRCT2_N_STS_RU_TOT_A_M;
		cr->usr2_strt_sts = TXUSRCT2_STRT_STS_A;
		cr->usr2_strt_sts_m = TXUSRCT2_STRT_STS_A_M;
		cr->usr3_n_sts = TXUSRCT3_N_STS_A;
		cr->usr3_n_sts_m = TXUSRCT3_N_STS_A_M;
		cr->usr3_n_sts_ru_tot = TXUSRCT3_N_STS_RU_TOT_A;
		cr->usr3_n_sts_ru_tot_m	= TXUSRCT3_N_STS_RU_TOT_A_M;
		cr->usr3_strt_sts = TXUSRCT3_STRT_STS_A;	
		cr->usr3_strt_sts_m = TXUSRCT3_STRT_STS_A_M;
		cr->source_gen_mode_idx	= SOURCE_GEN_MODE_IDX_A;
		cr->source_gen_mode_idx_m = SOURCE_GEN_MODE_IDX_A_M;
		cr->gi_type = TXCOMCT_GI_TYPE_A;
		cr->gi_type_m = TXCOMCT_GI_TYPE_A_M;	
		cr->ltf_type = TXCOMCT_LTF_TYPE_A;
		cr->ltf_type_m = TXCOMCT_LTF_TYPE_A_M;
		cr->dbw_idx = TXINFO_DBW_IDX_A;
		cr->dbw_idx_m = TXINFO_DBW_IDX_A_M;
		cr->pre_fec_fctr = TXTIMCT_PRE_FEC_FCTR_A;
		cr->pre_fec_fctr_m = TXTIMCT_PRE_FEC_FCTR_A_M;
		cr->beam_change_en = TXCOMCT_BEAM_CHANGE_EN_A;
		cr->beam_change_en_m = TXCOMCT_BEAM_CHANGE_EN_A_M;
		cr->doppler_en = TXCOMCT_DOPPLER_EN_A;
		cr->doppler_en_m = TXCOMCT_DOPPLER_EN_A_M;
		cr->fb_mumimo_en = TXCOMCT_FB_MUMIMO_EN_A;
		cr->fb_mumimo_en_m = TXCOMCT_FB_MUMIMO_EN_A_M;
		cr->feedback_status = TXCOMCT_FEEDBACK_STATUS_A;
		cr->feedback_status_m = TXCOMCT_FEEDBACK_STATUS_A_M;	
		cr->he_sigb_dcm_en = TXCOMCT_HE_SIGB_DCM_EN_A;
		cr->he_sigb_dcm_en_m = TXCOMCT_HE_SIGB_DCM_EN_A_M;
		cr->midamble_mode = TXCOMCT_MIDAMBLE_MODE_A;
		cr->midamble_mode_m = TXCOMCT_MIDAMBLE_MODE_A_M;
		cr->mumimo_ltf_mode_en = TXCOMCT_MUMIMO_LTF_MODE_EN_A;
		cr->mumimo_ltf_mode_en_m = TXCOMCT_MUMIMO_LTF_MODE_EN_A_M;
		cr->ndp = TXCOMCT_NDP_A;
		cr->ndp_m = TXCOMCT_NDP_A_M;	
		cr->stbc_en = TXCOMCT_STBC_EN_A;
		cr->stbc_en_m = TXCOMCT_STBC_EN_A_M;
		cr->ant_sel_a = TXINFO_ANT_SEL_A_A;
		cr->ant_sel_a_m	= TXINFO_ANT_SEL_A_A_M;
		cr->ant_sel_b = TXINFO_ANT_SEL_B_A;
		cr->ant_sel_b_m	= TXINFO_ANT_SEL_B_A_M;
		cr->ant_sel_c = TXINFO_ANT_SEL_C_A;
		cr->ant_sel_c_m	= TXINFO_ANT_SEL_C_A_M;
		cr->ant_sel_d = TXINFO_ANT_SEL_D_A;
		cr->ant_sel_d_m	= TXINFO_ANT_SEL_D_A_M;
		cr->cca_pw_th_en = TXINFO_CCA_PW_TH_EN_A;
		cr->cca_pw_th_en_m = TXINFO_CCA_PW_TH_EN_A_M;
		cr->rf_fixed_gain_en = TXINFO_RF_FIXED_GAIN_EN_A;
		cr->rf_fixed_gain_en_m = TXINFO_RF_FIXED_GAIN_EN_A_M;
		cr->ul_cqi_rpt_tri = TXINFO_UL_CQI_RPT_TRI_A;	
		cr->ul_cqi_rpt_tri_m = TXINFO_UL_CQI_RPT_TRI_A_M;
		cr->ldpc_extr = TXTIMCT_LDPC_EXTR_A;	
		cr->ldpc_extr_m	= TXTIMCT_LDPC_EXTR_A_M;
		cr->usr0_dcm_en	= TXUSRCT0_DCM_EN_A;
		cr->usr0_dcm_en_m = TXUSRCT0_DCM_EN_A_M;	
		cr->usr0_fec_type = TXUSRCT0_FEC_TYPE_A;	
		cr->usr0_fec_type_m = TXUSRCT0_FEC_TYPE_A_M;	
		cr->usr0_txbf_en = TXUSRCT0_TXBF_EN_A;		
		cr->usr0_txbf_en_m = TXUSRCT0_TXBF_EN_A_M;
		cr->usr1_dcm_en	= TXUSRCT1_DCM_EN_A;
		cr->usr1_dcm_en_m = TXUSRCT1_DCM_EN_A_M;	
		cr->usr1_fec_type = TXUSRCT1_FEC_TYPE_A;	
		cr->usr1_fec_type_m = TXUSRCT1_FEC_TYPE_A_M;
		cr->usr1_txbf_en = TXUSRCT1_TXBF_EN_A;	
		cr->usr1_txbf_en_m = TXUSRCT1_TXBF_EN_A_M;
		cr->usr2_dcm_en	= TXUSRCT2_DCM_EN_A;
		cr->usr2_dcm_en_m = TXUSRCT2_DCM_EN_A_M;		
		cr->usr2_fec_type = TXUSRCT2_FEC_TYPE_A;		
		cr->usr2_fec_type_m = TXUSRCT2_FEC_TYPE_A_M;	
		cr->usr2_txbf_en = TXUSRCT2_TXBF_EN_A;	
		cr->usr2_txbf_en_m = TXUSRCT2_TXBF_EN_A_M;	
		cr->usr3_dcm_en = TXUSRCT3_DCM_EN_A;
		cr->usr3_dcm_en_m = TXUSRCT3_DCM_EN_A_M;	
		cr->usr3_fec_type = TXUSRCT3_FEC_TYPE_A;	
		cr->usr3_fec_type_m = TXUSRCT3_FEC_TYPE_A_M;
		cr->usr3_txbf_en = TXUSRCT3_TXBF_EN_A;	
		cr->usr3_txbf_en_m = TXUSRCT3_TXBF_EN_A_M;
		break;

	#endif
	#ifdef HALBB_COMPILE_AP2_SERIES
	case BB_AP2:
		cr->b_header_0 = R1B_TX_PMAC_HEADER_0_A2;
		cr->b_header_0_m = R1B_TX_PMAC_HEADER_0_A2_M;	
		cr->b_header_1 = R1B_TX_PMAC_HEADER_1_A2;
		cr->b_header_1_m = R1B_TX_PMAC_HEADER_1_A2_M;	
		cr->b_header_2 = R1B_TX_PMAC_HEADER_2_A2;
		cr->b_header_2_m = R1B_TX_PMAC_HEADER_2_A2_M;	
		cr->b_header_3 = R1B_TX_PMAC_HEADER_3_A2;
		cr->b_header_3_m = R1B_TX_PMAC_HEADER_3_A2_M;
		cr->b_header_4 = R1B_TX_PMAC_HEADER_4_A2;
		cr->b_header_4_m = R1B_TX_PMAC_HEADER_4_A2_M;	
		cr->b_header_5 = R1B_TX_PMAC_HEADER_5_A2;
		cr->b_header_5_m = R1B_TX_PMAC_HEADER_5_A2_M;	
		cr->b_carrier_suppress_tx = R1B_TX_PMAC_CARRIER_SUPPRESS_TX_A2;	
		cr->b_carrier_suppress_tx_m = R1B_TX_PMAC_CARRIER_SUPPRESS_TX_A2_M;
		cr->b_rate_idx = BMODE_RATE_IDX_A2;
		cr->b_rate_idx_m = BMODE_RATE_IDX_A2_M;
		cr->b_locked_clk_en = BMODE_LOCKED_CLK_EN_A2;
		cr->b_locked_clk_en_m = BMODE_LOCKED_CLK_EN_A2_M;
		cr->he_sigb_ch1_0 = TXD_HE_SIGB_CH1_0_A2;		
		cr->he_sigb_ch1_0_m = TXD_HE_SIGB_CH1_0_A2_M;	
		cr->he_sigb_ch1_1 = TXD_HE_SIGB_CH1_1_A2;		
		cr->he_sigb_ch1_1_m = TXD_HE_SIGB_CH1_1_A2_M;	
		cr->he_sigb_ch1_10 = TXD_HE_SIGB_CH1_10_A2;		
		cr->he_sigb_ch1_10_m = TXD_HE_SIGB_CH1_10_A2_M;
		cr->he_sigb_ch1_11 = TXD_HE_SIGB_CH1_11_A2;		
		cr->he_sigb_ch1_11_m = TXD_HE_SIGB_CH1_11_A2_M;
		cr->he_sigb_ch1_12 = TXD_HE_SIGB_CH1_12_A2;	
		cr->he_sigb_ch1_12_m = TXD_HE_SIGB_CH1_12_A2_M;
		cr->he_sigb_ch1_13 = TXD_HE_SIGB_CH1_13_A2;	
		cr->he_sigb_ch1_13_m = TXD_HE_SIGB_CH1_13_A2_M;
		cr->he_sigb_ch1_14 = TXD_HE_SIGB_CH1_14_A2;	
		cr->he_sigb_ch1_14_m = TXD_HE_SIGB_CH1_14_A2_M;
		cr->he_sigb_ch1_15 = TXD_HE_SIGB_CH1_15_A2;		
		cr->he_sigb_ch1_15_m = TXD_HE_SIGB_CH1_15_A2_M;
		cr->he_sigb_ch1_2 = TXD_HE_SIGB_CH1_2_A2;	
		cr->he_sigb_ch1_2_m = TXD_HE_SIGB_CH1_2_A2_M;
		cr->he_sigb_ch1_3 = TXD_HE_SIGB_CH1_3_A2;	
		cr->he_sigb_ch1_3_m = TXD_HE_SIGB_CH1_3_A2_M;
		cr->he_sigb_ch1_4 = TXD_HE_SIGB_CH1_4_A2;	
		cr->he_sigb_ch1_4_m = TXD_HE_SIGB_CH1_4_A2_M;
		cr->he_sigb_ch1_5 = TXD_HE_SIGB_CH1_5_A2;	
		cr->he_sigb_ch1_5_m = TXD_HE_SIGB_CH1_5_A2_M;
		cr->he_sigb_ch1_6 = TXD_HE_SIGB_CH1_6_A2;	
		cr->he_sigb_ch1_6_m = TXD_HE_SIGB_CH1_6_A2_M;
		cr->he_sigb_ch1_7 = TXD_HE_SIGB_CH1_7_A2;	
		cr->he_sigb_ch1_7_m = TXD_HE_SIGB_CH1_7_A2_M;
		cr->he_sigb_ch1_8 = TXD_HE_SIGB_CH1_8_A2;	
		cr->he_sigb_ch1_8_m = TXD_HE_SIGB_CH1_8_A2_M;
		cr->he_sigb_ch1_9 = TXD_HE_SIGB_CH1_9_A2;	
		cr->he_sigb_ch1_9_m = TXD_HE_SIGB_CH1_9_A2_M;
		cr->he_sigb_ch2_0 = TXD_HE_SIGB_CH2_0_A2;	
		cr->he_sigb_ch2_0_m = TXD_HE_SIGB_CH2_0_A2_M;
		cr->he_sigb_ch2_1 = TXD_HE_SIGB_CH2_1_A2;	
		cr->he_sigb_ch2_1_m = TXD_HE_SIGB_CH2_1_A2_M;
		cr->he_sigb_ch2_10 = TXD_HE_SIGB_CH2_10_A2;	
		cr->he_sigb_ch2_10_m = TXD_HE_SIGB_CH2_10_A2_M;
		cr->he_sigb_ch2_11 = TXD_HE_SIGB_CH2_11_A2;
		cr->he_sigb_ch2_11_m = TXD_HE_SIGB_CH2_11_A2_M;
		cr->he_sigb_ch2_12 = TXD_HE_SIGB_CH2_12_A2;
		cr->he_sigb_ch2_12_m = TXD_HE_SIGB_CH2_12_A2_M;
		cr->he_sigb_ch2_13 = TXD_HE_SIGB_CH2_13_A2;
		cr->he_sigb_ch2_13_m = TXD_HE_SIGB_CH2_13_A2_M;
		cr->he_sigb_ch2_14 = TXD_HE_SIGB_CH2_14_A2;	
		cr->he_sigb_ch2_14_m = TXD_HE_SIGB_CH2_14_A2_M;
		cr->he_sigb_ch2_15 = TXD_HE_SIGB_CH2_15_A2;		
		cr->he_sigb_ch2_15_m = TXD_HE_SIGB_CH2_15_A2_M;
		cr->he_sigb_ch2_2 = TXD_HE_SIGB_CH2_2_A2;	
		cr->he_sigb_ch2_2_m = TXD_HE_SIGB_CH2_2_A2_M;	
		cr->he_sigb_ch2_3 = TXD_HE_SIGB_CH2_3_A2;	
		cr->he_sigb_ch2_3_m = TXD_HE_SIGB_CH2_3_A2_M;
		cr->he_sigb_ch2_4 = TXD_HE_SIGB_CH2_4_A2;	
		cr->he_sigb_ch2_4_m = TXD_HE_SIGB_CH2_4_A2_M;
		cr->he_sigb_ch2_5 = TXD_HE_SIGB_CH2_5_A2;
		cr->he_sigb_ch2_5_m = TXD_HE_SIGB_CH2_5_A2_M;
		cr->he_sigb_ch2_6 = TXD_HE_SIGB_CH2_6_A2;
		cr->he_sigb_ch2_6_m = TXD_HE_SIGB_CH2_6_A2_M;
		cr->he_sigb_ch2_7 = TXD_HE_SIGB_CH2_7_A2;		
		cr->he_sigb_ch2_7_m = TXD_HE_SIGB_CH2_7_A2_M;
		cr->he_sigb_ch2_8 = TXD_HE_SIGB_CH2_8_A2;
		cr->he_sigb_ch2_8_m = TXD_HE_SIGB_CH2_8_A2_M;
		cr->he_sigb_ch2_9 = TXD_HE_SIGB_CH2_9_A2;
		cr->he_sigb_ch2_9_m = TXD_HE_SIGB_CH2_9_A2_M;
		cr->usr0_delmter = USER0_DELMTER_A2;	
		cr->usr0_delmter_m = USER0_DELMTER_A2_M;	
		cr->usr0_eof_padding_len = USER0_EOF_PADDING_LEN_A2;
		cr->usr0_eof_padding_len_m = USER0_EOF_PADDING_LEN_A2_M;
		cr->usr0_init_seed = USER0_INIT_SEED_A2;		
		cr->usr0_init_seed_m = USER0_INIT_SEED_A2_M;	
		cr->usr1_delmter = USER1_DELMTER_A2;	
		cr->usr1_delmter_m = USER1_DELMTER_A2_M;
		cr->usr1_eof_padding_len = USER1_EOF_PADDING_LEN_A2;	
		cr->usr1_eof_padding_len_m = USER1_EOF_PADDING_LEN_A2_M;
		cr->usr1_init_seed = USER1_INIT_SEED_A2;
		cr->usr1_init_seed_m = USER1_INIT_SEED_A2_M;
		cr->usr2_delmter = USER2_DELMTER_A2;
		cr->usr2_delmter_m = USER2_DELMTER_A2_M;
		cr->usr2_eof_padding_len = USER2_EOF_PADDING_LEN_A2;	
		cr->usr2_eof_padding_len_m = USER2_EOF_PADDING_LEN_A2_M;
		cr->usr2_init_seed = USER2_INIT_SEED_A2;
		cr->usr2_init_seed_m = USER2_INIT_SEED_A2_M;
		cr->usr3_delmter = USER3_DELMTER_A2;	
		cr->usr3_delmter_m = USER3_DELMTER_A2_M;
		cr->usr3_eof_padding_len = USER3_EOF_PADDING_LEN_A2;
		cr->usr3_eof_padding_len_m = USER3_EOF_PADDING_LEN_A2_M;
		cr->usr3_init_seed = USER3_INIT_SEED_A2;
		cr->usr3_init_seed_m = USER3_INIT_SEED_A2_M;
		cr->vht_sigb0 = TXD_VHT_SIGB0_A2;	
		cr->vht_sigb0_m	= TXD_VHT_SIGB0_A2_M;
		cr->vht_sigb1 = TXD_VHT_SIGB1_A2;	
		cr->vht_sigb1_m	= TXD_VHT_SIGB1_A2_M;
		cr->vht_sigb2 = TXD_VHT_SIGB2_A2;	
		cr->vht_sigb2_m	= TXD_VHT_SIGB2_A2_M;
		cr->he_sigb_mcs = TXCOMCT_HE_SIGB_MCS_A2;
		cr->he_sigb_mcs_m = TXCOMCT_HE_SIGB_MCS_A2_M;
		cr->vht_sigb3 = TXD_VHT_SIGB3_A2;
		cr->vht_sigb3_m = TXD_VHT_SIGB3_A2_M;
		cr->n_ltf = TXCOMCT_N_LTF_A2;
		cr->n_ltf_m = TXCOMCT_N_LTF_A2_M;
		cr->siga1 = TXD_SIGA1_A2;	
		cr->siga1_m = TXD_SIGA1_A2_M;
		cr->siga2 = TXD_SIGA2_A2;	
		cr->siga2_m = TXD_SIGA2_A2_M;
		cr->lsig = TXD_LSIG_A2;	
		cr->lsig_m = TXD_LSIG_A2_M;
		cr->cca_pw_th = TXINFO_CCA_PW_TH_A2;
		cr->cca_pw_th_m	= TXINFO_CCA_PW_TH_A2_M;
		cr->n_sym = TXTIMCT_N_SYM_A2;	
		cr->n_sym_m = TXTIMCT_N_SYM_A2_M;
		cr->usr0_service = USER0_SERVICE_A2;		
		cr->usr0_service_m = USER0_SERVICE_A2_M;	
		cr->usr1_service = USER1_SERVICE_A2;		
		cr->usr1_service_m = USER1_SERVICE_A2_M;	
		cr->usr2_service = USER2_SERVICE_A2;		
		cr->usr2_service_m = USER2_SERVICE_A2_M;
		cr->usr3_service = USER3_SERVICE_A2;	
		cr->usr3_service_m = USER3_SERVICE_A2_M;	
		cr->usr0_mdpu_len_byte = USER0_MDPU_LEN_BYTE_A2;
		cr->usr0_mdpu_len_byte_m = USER0_MDPU_LEN_BYTE_A2_M;	
		cr->usr1_mdpu_len_byte = USER1_MDPU_LEN_BYTE_A2;	
		cr->usr1_mdpu_len_byte_m = USER1_MDPU_LEN_BYTE_A2_M;	
		cr->obw_cts2self_dup_type = TXINFO_OBW_CTS2SELF_DUP_TYPE_A2;	
		cr->obw_cts2self_dup_type_m = TXINFO_OBW_CTS2SELF_DUP_TYPE_A2_M;		
		cr->usr2_mdpu_len_byte = USER2_MDPU_LEN_BYTE_A2;
		cr->usr2_mdpu_len_byte_m = USER2_MDPU_LEN_BYTE_A2_M;
		cr->usr3_mdpu_len_byte = USER3_MDPU_LEN_BYTE_A2;
		cr->usr3_mdpu_len_byte_m = USER3_MDPU_LEN_BYTE_A2_M;
		cr->usr0_csi_buf_id = TXUSRCT0_CSI_BUF_ID_A2;
		cr->usr0_csi_buf_id_m = TXUSRCT0_CSI_BUF_ID_A2_M;
		cr->usr1_csi_buf_id = TXUSRCT1_CSI_BUF_ID_A2;
		cr->usr1_csi_buf_id_m = TXUSRCT1_CSI_BUF_ID_A2_M;
		cr->rf_gain_idx	= TXINFO_RF_GAIN_IDX_A2;	
		cr->rf_gain_idx_m = TXINFO_RF_GAIN_IDX_A2_M;
		cr->usr2_csi_buf_id = TXUSRCT2_CSI_BUF_ID_A2;
		cr->usr2_csi_buf_id_m = TXUSRCT2_CSI_BUF_ID_A2_M;
		cr->usr3_csi_buf_id = TXUSRCT3_CSI_BUF_ID_A2;
		cr->usr3_csi_buf_id_m = TXUSRCT3_CSI_BUF_ID_A2_M;
		cr->usr0_n_mpdu	= USER0_N_MPDU_A2;	
		cr->usr0_n_mpdu_m = USER0_N_MPDU_A2_M;	
		cr->usr1_n_mpdu	= USER1_N_MPDU_A2;	
		cr->usr1_n_mpdu_m = USER1_N_MPDU_A2_M;	
		cr->usr2_n_mpdu	= USER2_N_MPDU_A2;	
		cr->usr2_n_mpdu_m = USER2_N_MPDU_A2_M;
		cr->usr0_pw_boost_fctr_db = TXUSRCT0_PW_BOOST_FCTR_DB_A2;	
		cr->usr0_pw_boost_fctr_db_m = TXUSRCT0_PW_BOOST_FCTR_DB_A2_M;	
		cr->usr3_n_mpdu = USER3_N_MPDU_A2;
		cr->usr3_n_mpdu_m = USER3_N_MPDU_A2_M;		
		cr->ch20_with_data = TXINFO_CH20_WITH_DATA_A2;	
		cr->ch20_with_data_m = TXINFO_CH20_WITH_DATA_A2_M;
		cr->n_usr = TXINFO_N_USR_A2;	
		cr->n_usr_m = TXINFO_N_USR_A2_M;	
		cr->txcmd_txtp = TXINFO_TXCMD_TXTP_A2;
		cr->txcmd_txtp_m = TXINFO_TXCMD_TXTP_A2_M;
		cr->usr0_ru_alloc = TXUSRCT0_RU_ALLOC_A2;	
		cr->usr0_ru_alloc_m = TXUSRCT0_RU_ALLOC_A2_M;
		cr->usr0_u_id = TXUSRCT0_U_ID_A2;
		cr->usr0_u_id_m	= TXUSRCT0_U_ID_A2_M;
		cr->usr1_ru_alloc = TXUSRCT1_RU_ALLOC_A2;	
		cr->usr1_ru_alloc_m = TXUSRCT1_RU_ALLOC_A2_M;
		cr->usr1_u_id = TXUSRCT1_U_ID_A2;		
		cr->usr1_u_id_m	= TXUSRCT1_U_ID_A2_M;
		cr->usr2_ru_alloc = TXUSRCT2_RU_ALLOC_A2;
		cr->usr2_ru_alloc_m = TXUSRCT2_RU_ALLOC_A2_M;
		cr->usr2_u_id = TXUSRCT2_U_ID_A2;
		cr->usr2_u_id_m	= TXUSRCT2_U_ID_A2_M;
		cr->usr3_ru_alloc = TXUSRCT3_RU_ALLOC_A2;
		cr->usr3_ru_alloc_m = TXUSRCT3_RU_ALLOC_A2_M;
		cr->usr3_u_id = TXUSRCT3_U_ID_A2;
		cr->usr3_u_id_m	= TXUSRCT3_U_ID_A2_M;
		cr->n_sym_hesigb = TXTIMCT_N_SYM_HESIGB_A2;	
		cr->n_sym_hesigb_m = TXTIMCT_N_SYM_HESIGB_A2_M;	
		cr->usr0_mcs = TXUSRCT0_MCS_A2;
		cr->usr0_mcs_m	= TXUSRCT0_MCS_A2_M;
		cr->usr1_mcs = TXUSRCT1_MCS_A2;
		cr->usr1_mcs_m	= TXUSRCT1_MCS_A2_M;
		cr->usr2_mcs = TXUSRCT2_MCS_A2;
		cr->usr2_mcs_m = TXUSRCT2_MCS_A2_M;
		cr->usr3_mcs = TXUSRCT3_MCS_A2;
		cr->usr3_mcs_m = TXUSRCT3_MCS_A2_M;
		cr->usr1_pw_boost_fctr_db = TXUSRCT1_PW_BOOST_FCTR_DB_A2;
		cr->usr1_pw_boost_fctr_db_m = TXUSRCT1_PW_BOOST_FCTR_DB_A2_M;
		cr->usr2_pw_boost_fctr_db = TXUSRCT2_PW_BOOST_FCTR_DB_A2;
		cr->usr2_pw_boost_fctr_db_m = TXUSRCT2_PW_BOOST_FCTR_DB_A2_M;
		cr->usr3_pw_boost_fctr_db = TXUSRCT3_PW_BOOST_FCTR_DB_A2;
		cr->usr3_pw_boost_fctr_db_m = TXUSRCT3_PW_BOOST_FCTR_DB_A2_M;
		cr->ppdu_type = TXINFO_PPDU_TYPE_A2;
		cr->ppdu_type_m	= TXINFO_PPDU_TYPE_A2_M;
		cr->txsc = TXINFO_TXSC_A2;	
		cr->txsc_m = TXINFO_TXSC_A2_M;
		cr->cfo_comp = TXINFO_CFO_COMP_A2;
		cr->cfo_comp_m = TXINFO_CFO_COMP_A2_M;
		cr->pkt_ext_idx = TXTIMCT_PKT_EXT_IDX_A2;
		cr->pkt_ext_idx_m = TXTIMCT_PKT_EXT_IDX_A2_M;	
		cr->usr0_n_sts = TXUSRCT0_N_STS_A2;	
		cr->usr0_n_sts_m = TXUSRCT0_N_STS_A2_M;
		cr->usr0_n_sts_ru_tot = TXUSRCT0_N_STS_RU_TOT_A2;
		cr->usr0_n_sts_ru_tot_m = TXUSRCT0_N_STS_RU_TOT_A2_M;
		cr->usr0_strt_sts = TXUSRCT0_STRT_STS_A2;
		cr->usr0_strt_sts_m = TXUSRCT0_STRT_STS_A2_M;
		cr->usr1_n_sts = TXUSRCT1_N_STS_A2;
		cr->usr1_n_sts_m = TXUSRCT1_N_STS_A2_M;
		cr->usr1_n_sts_ru_tot = TXUSRCT1_N_STS_RU_TOT_A2;
		cr->usr1_n_sts_ru_tot_m = TXUSRCT1_N_STS_RU_TOT_A2_M;
		cr->usr1_strt_sts = TXUSRCT1_STRT_STS_A2;	
		cr->usr1_strt_sts_m = TXUSRCT1_STRT_STS_A2_M;
		cr->usr2_n_sts = TXUSRCT2_N_STS_A2;
		cr->usr2_n_sts_m = TXUSRCT2_N_STS_A2_M;
		cr->usr2_n_sts_ru_tot = TXUSRCT2_N_STS_RU_TOT_A2;
		cr->usr2_n_sts_ru_tot_m	= TXUSRCT2_N_STS_RU_TOT_A2_M;
		cr->usr2_strt_sts = TXUSRCT2_STRT_STS_A2;
		cr->usr2_strt_sts_m = TXUSRCT2_STRT_STS_A2_M;
		cr->usr3_n_sts = TXUSRCT3_N_STS_A2;
		cr->usr3_n_sts_m = TXUSRCT3_N_STS_A2_M;
		cr->usr3_n_sts_ru_tot = TXUSRCT3_N_STS_RU_TOT_A2;
		cr->usr3_n_sts_ru_tot_m	= TXUSRCT3_N_STS_RU_TOT_A2_M;
		cr->usr3_strt_sts = TXUSRCT3_STRT_STS_A2;	
		cr->usr3_strt_sts_m = TXUSRCT3_STRT_STS_A2_M;
		cr->source_gen_mode_idx	= SOURCE_GEN_MODE_IDX_A2;
		cr->source_gen_mode_idx_m = SOURCE_GEN_MODE_IDX_A2_M;
		cr->gi_type = TXCOMCT_GI_TYPE_A2;
		cr->gi_type_m = TXCOMCT_GI_TYPE_A2_M;	
		cr->ltf_type = TXCOMCT_LTF_TYPE_A2;
		cr->ltf_type_m = TXCOMCT_LTF_TYPE_A2_M;
		cr->dbw_idx = TXINFO_DBW_IDX_A2;
		cr->dbw_idx_m = TXINFO_DBW_IDX_A2_M;
		cr->pre_fec_fctr = TXTIMCT_PRE_FEC_FCTR_A2;
		cr->pre_fec_fctr_m = TXTIMCT_PRE_FEC_FCTR_A2_M;
		cr->beam_change_en = TXCOMCT_BEAM_CHANGE_EN_A2;
		cr->beam_change_en_m = TXCOMCT_BEAM_CHANGE_EN_A2_M;
		cr->doppler_en = TXCOMCT_DOPPLER_EN_A2;
		cr->doppler_en_m = TXCOMCT_DOPPLER_EN_A2_M;
		cr->fb_mumimo_en = TXCOMCT_FB_MUMIMO_EN_A2;
		cr->fb_mumimo_en_m = TXCOMCT_FB_MUMIMO_EN_A2_M;
		cr->feedback_status = TXCOMCT_FEEDBACK_STATUS_A2;
		cr->feedback_status_m = TXCOMCT_FEEDBACK_STATUS_A2_M;	
		cr->he_sigb_dcm_en = TXCOMCT_HE_SIGB_DCM_EN_A2;
		cr->he_sigb_dcm_en_m = TXCOMCT_HE_SIGB_DCM_EN_A2_M;
		cr->midamble_mode = TXCOMCT_MIDAMBLE_MODE_A2;
		cr->midamble_mode_m = TXCOMCT_MIDAMBLE_MODE_A2_M;
		cr->mumimo_ltf_mode_en = TXCOMCT_MUMIMO_LTF_MODE_EN_A2;
		cr->mumimo_ltf_mode_en_m = TXCOMCT_MUMIMO_LTF_MODE_EN_A2_M;
		cr->ndp = TXCOMCT_NDP_A2;
		cr->ndp_m = TXCOMCT_NDP_A2_M;	
		cr->stbc_en = TXCOMCT_STBC_EN_A2;
		cr->stbc_en_m = TXCOMCT_STBC_EN_A2_M;
		cr->ant_sel_a = TXINFO_ANT_SEL_A_A2;
		cr->ant_sel_a_m	= TXINFO_ANT_SEL_A_A2_M;
		cr->ant_sel_b = TXINFO_ANT_SEL_B_A2;
		cr->ant_sel_b_m	= TXINFO_ANT_SEL_B_A2_M;
		cr->ant_sel_c = TXINFO_ANT_SEL_C_A2;
		cr->ant_sel_c_m	= TXINFO_ANT_SEL_C_A2_M;
		cr->ant_sel_d = TXINFO_ANT_SEL_D_A2;
		cr->ant_sel_d_m	= TXINFO_ANT_SEL_D_A2_M;
		cr->cca_pw_th_en = TXINFO_CCA_PW_TH_EN_A2;
		cr->cca_pw_th_en_m = TXINFO_CCA_PW_TH_EN_A2_M;
		cr->rf_fixed_gain_en = TXINFO_RF_FIXED_GAIN_EN_A2;
		cr->rf_fixed_gain_en_m = TXINFO_RF_FIXED_GAIN_EN_A2_M;
		cr->ul_cqi_rpt_tri = TXINFO_UL_CQI_RPT_TRI_A2;	
		cr->ul_cqi_rpt_tri_m = TXINFO_UL_CQI_RPT_TRI_A2_M;
		cr->ldpc_extr = TXTIMCT_LDPC_EXTR_A2;	
		cr->ldpc_extr_m	= TXTIMCT_LDPC_EXTR_A2_M;
		cr->usr0_dcm_en	= TXUSRCT0_DCM_EN_A2;
		cr->usr0_dcm_en_m = TXUSRCT0_DCM_EN_A2_M;	
		cr->usr0_fec_type = TXUSRCT0_FEC_TYPE_A2;	
		cr->usr0_fec_type_m = TXUSRCT0_FEC_TYPE_A2_M;	
		cr->usr0_precoding_mode_idx = TXUSRCT0_PRECODING_MODE_IDX_A2;		
		cr->usr0_precoding_mode_idx_m = TXUSRCT0_PRECODING_MODE_IDX_A2;
		cr->usr1_dcm_en	= TXUSRCT1_DCM_EN_A2;
		cr->usr1_dcm_en_m = TXUSRCT1_DCM_EN_A2_M;	
		cr->usr1_fec_type = TXUSRCT1_FEC_TYPE_A2;	
		cr->usr1_fec_type_m = TXUSRCT1_FEC_TYPE_A2_M;
		cr->usr1_precoding_mode_idx = TXUSRCT1_PRECODING_MODE_IDX_A2;	
		cr->usr1_precoding_mode_idx_m = TXUSRCT1_PRECODING_MODE_IDX_A2;
		cr->usr2_dcm_en	= TXUSRCT2_DCM_EN_A2;
		cr->usr2_dcm_en_m = TXUSRCT2_DCM_EN_A2_M;		
		cr->usr2_fec_type = TXUSRCT2_FEC_TYPE_A2;		
		cr->usr2_fec_type_m = TXUSRCT2_FEC_TYPE_A2_M;	
		cr->usr2_precoding_mode_idx = TXUSRCT2_PRECODING_MODE_IDX_A2;	
		cr->usr2_precoding_mode_idx_m = TXUSRCT2_PRECODING_MODE_IDX_A2;	
		cr->usr3_dcm_en = TXUSRCT3_DCM_EN_A2;
		cr->usr3_dcm_en_m = TXUSRCT3_DCM_EN_A2_M;	
		cr->usr3_fec_type = TXUSRCT3_FEC_TYPE_A2;	
		cr->usr3_fec_type_m = TXUSRCT3_FEC_TYPE_A2_M;
		cr->usr3_precoding_mode_idx = TXUSRCT3_PRECODING_MODE_IDX_A2;	
		cr->usr3_precoding_mode_idx_m = TXUSRCT3_PRECODING_MODE_IDX_A2;
		cr->max_mcs = TXINFO_MAX_MCS_A2;
		cr->max_mcs_m = TXINFO_MAX_MCS_A2_M;
		cr->tx_tri_idx = TXINFO_TX_BANDEDGE_A2;
		cr->tx_tri_idx_m = TXINFO_TX_BANDEDGE_A2_M;
		cr->tx_tri_pw_ofst = TXINFO_PW_OFST_SEG0_DB_A2;
		cr->tx_tri_pw_ofst_m = TXINFO_PW_OFST_SEG0_DB_A2_M;
		break;

	#endif
	#ifdef HALBB_COMPILE_CLIENT_SERIES
	case BB_CLIENT:
		cr->b_header_0 = R1B_TX_PMAC_HEADER_0_C;
		cr->b_header_0_m = R1B_TX_PMAC_HEADER_0_C_M;	
		cr->b_header_1 = R1B_TX_PMAC_HEADER_1_C;
		cr->b_header_1_m = R1B_TX_PMAC_HEADER_1_C_M;	
		cr->b_header_2 = R1B_TX_PMAC_HEADER_2_C;
		cr->b_header_2_m = R1B_TX_PMAC_HEADER_2_C_M;	
		cr->b_header_3 = R1B_TX_PMAC_HEADER_3_C;
		cr->b_header_3_m = R1B_TX_PMAC_HEADER_3_C_M;
		cr->b_header_4 = R1B_TX_PMAC_HEADER_4_C;
		cr->b_header_4_m = R1B_TX_PMAC_HEADER_4_C_M;	
		cr->b_header_5 = R1B_TX_PMAC_HEADER_5_C;
		cr->b_header_5_m = R1B_TX_PMAC_HEADER_5_C_M;	
		cr->b_psdu_byte = R1B_TX_PMAC_PSDU_BYTE_C;	
		cr->b_psdu_byte_m = R1B_TX_PMAC_PSDU_BYTE_C_M;
		cr->b_carrier_suppress_tx = R1B_TX_PMAC_CARRIER_SUPPRESS_TX_C;	
		cr->b_carrier_suppress_tx_m = R1B_TX_PMAC_CARRIER_SUPPRESS_TX_C_M;
		cr->b_ppdu_type	= R1B_TX_PMAC_PPDU_TYPE_C;	
		cr->b_ppdu_type_m = R1B_TX_PMAC_PPDU_TYPE_C_M;	
		cr->b_psdu_rate	= R1B_TX_PMAC_PSDU_RATE_C;	
		cr->b_psdu_rate_m = R1B_TX_PMAC_PSDU_RATE_C_M;	
		cr->b_service_bit2 = R1B_TX_PMAC_SERVICE_BIT2_C;	
		cr->b_service_bit2_m = R1B_TX_PMAC_SERVICE_BIT2_C_M;	
		cr->he_sigb_ch1_0 = TXD_HE_SIGB_CH1_0_C;		
		cr->he_sigb_ch1_0_m = TXD_HE_SIGB_CH1_0_C_M;	
		cr->he_sigb_ch1_1 = TXD_HE_SIGB_CH1_1_C;		
		cr->he_sigb_ch1_1_m = TXD_HE_SIGB_CH1_1_C_M;	
		cr->he_sigb_ch1_10 = TXD_HE_SIGB_CH1_10_C;		
		cr->he_sigb_ch1_10_m = TXD_HE_SIGB_CH1_10_C_M;
		cr->he_sigb_ch1_11 = TXD_HE_SIGB_CH1_11_C;		
		cr->he_sigb_ch1_11_m = TXD_HE_SIGB_CH1_11_C_M;
		cr->he_sigb_ch1_12 = TXD_HE_SIGB_CH1_12_C;	
		cr->he_sigb_ch1_12_m = TXD_HE_SIGB_CH1_12_C_M;
		cr->he_sigb_ch1_13 = TXD_HE_SIGB_CH1_13_C;	
		cr->he_sigb_ch1_13_m = TXD_HE_SIGB_CH1_13_C_M;
		cr->he_sigb_ch1_14 = TXD_HE_SIGB_CH1_14_C;	
		cr->he_sigb_ch1_14_m = TXD_HE_SIGB_CH1_14_C_M;
		cr->he_sigb_ch1_15 = TXD_HE_SIGB_CH1_15_C;		
		cr->he_sigb_ch1_15_m = TXD_HE_SIGB_CH1_15_C_M;
		cr->he_sigb_ch1_2 = TXD_HE_SIGB_CH1_2_C;	
		cr->he_sigb_ch1_2_m = TXD_HE_SIGB_CH1_2_C_M;
		cr->he_sigb_ch1_3 = TXD_HE_SIGB_CH1_3_C;	
		cr->he_sigb_ch1_3_m = TXD_HE_SIGB_CH1_3_C_M;
		cr->he_sigb_ch1_4 = TXD_HE_SIGB_CH1_4_C;	
		cr->he_sigb_ch1_4_m = TXD_HE_SIGB_CH1_4_C_M;
		cr->he_sigb_ch1_5 = TXD_HE_SIGB_CH1_5_C;	
		cr->he_sigb_ch1_5_m = TXD_HE_SIGB_CH1_5_C_M;
		cr->he_sigb_ch1_6 = TXD_HE_SIGB_CH1_6_C;	
		cr->he_sigb_ch1_6_m = TXD_HE_SIGB_CH1_6_C_M;
		cr->he_sigb_ch1_7 = TXD_HE_SIGB_CH1_7_C;	
		cr->he_sigb_ch1_7_m = TXD_HE_SIGB_CH1_7_C_M;
		cr->he_sigb_ch1_8 = TXD_HE_SIGB_CH1_8_C;	
		cr->he_sigb_ch1_8_m = TXD_HE_SIGB_CH1_8_C_M;
		cr->he_sigb_ch1_9 = TXD_HE_SIGB_CH1_9_C;	
		cr->he_sigb_ch1_9_m = TXD_HE_SIGB_CH1_9_C_M;
		cr->he_sigb_ch2_0 = TXD_HE_SIGB_CH2_0_C;	
		cr->he_sigb_ch2_0_m = TXD_HE_SIGB_CH2_0_C_M;
		cr->he_sigb_ch2_1 = TXD_HE_SIGB_CH2_1_C;	
		cr->he_sigb_ch2_1_m = TXD_HE_SIGB_CH2_1_C_M;
		cr->he_sigb_ch2_10 = TXD_HE_SIGB_CH2_10_C;	
		cr->he_sigb_ch2_10_m = TXD_HE_SIGB_CH2_10_C_M;
		cr->he_sigb_ch2_11 = TXD_HE_SIGB_CH2_11_C;
		cr->he_sigb_ch2_11_m = TXD_HE_SIGB_CH2_11_C_M;
		cr->he_sigb_ch2_12 = TXD_HE_SIGB_CH2_12_C;
		cr->he_sigb_ch2_12_m = TXD_HE_SIGB_CH2_12_C_M;
		cr->he_sigb_ch2_13 = TXD_HE_SIGB_CH2_13_C;
		cr->he_sigb_ch2_13_m = TXD_HE_SIGB_CH2_13_C_M;
		cr->he_sigb_ch2_14 = TXD_HE_SIGB_CH2_14_C;	
		cr->he_sigb_ch2_14_m = TXD_HE_SIGB_CH2_14_C_M;
		cr->he_sigb_ch2_15 = TXD_HE_SIGB_CH2_15_C;		
		cr->he_sigb_ch2_15_m = TXD_HE_SIGB_CH2_15_C_M;
		cr->he_sigb_ch2_2 = TXD_HE_SIGB_CH2_2_C;	
		cr->he_sigb_ch2_2_m = TXD_HE_SIGB_CH2_2_C_M;	
		cr->he_sigb_ch2_3 = TXD_HE_SIGB_CH2_3_C;	
		cr->he_sigb_ch2_3_m = TXD_HE_SIGB_CH2_3_C_M;
		cr->he_sigb_ch2_4 = TXD_HE_SIGB_CH2_4_C;	
		cr->he_sigb_ch2_4_m = TXD_HE_SIGB_CH2_4_C_M;
		cr->he_sigb_ch2_5 = TXD_HE_SIGB_CH2_5_C;
		cr->he_sigb_ch2_5_m = TXD_HE_SIGB_CH2_5_C_M;
		cr->he_sigb_ch2_6 = TXD_HE_SIGB_CH2_6_C;
		cr->he_sigb_ch2_6_m = TXD_HE_SIGB_CH2_6_C_M;
		cr->he_sigb_ch2_7 = TXD_HE_SIGB_CH2_7_C;		
		cr->he_sigb_ch2_7_m = TXD_HE_SIGB_CH2_7_C_M;
		cr->he_sigb_ch2_8 = TXD_HE_SIGB_CH2_8_C;
		cr->he_sigb_ch2_8_m = TXD_HE_SIGB_CH2_8_C_M;
		cr->he_sigb_ch2_9 = TXD_HE_SIGB_CH2_9_C;
		cr->he_sigb_ch2_9_m = TXD_HE_SIGB_CH2_9_C_M;
		cr->usr0_delmter = USER0_DELMTER_C;	
		cr->usr0_delmter_m = USER0_DELMTER_C_M;	
		cr->usr0_eof_padding_len = USER0_EOF_PADDING_LEN_C;
		cr->usr0_eof_padding_len_m = USER0_EOF_PADDING_LEN_C_M;
		cr->usr0_init_seed = USER0_INIT_SEED_C;		
		cr->usr0_init_seed_m = USER0_INIT_SEED_C_M;	
		cr->usr1_delmter = USER1_DELMTER_C;	
		cr->usr1_delmter_m = USER1_DELMTER_C_M;
		cr->usr1_eof_padding_len = USER1_EOF_PADDING_LEN_C;	
		cr->usr1_eof_padding_len_m = USER1_EOF_PADDING_LEN_C_M;
		cr->usr1_init_seed = USER1_INIT_SEED_C;
		cr->usr1_init_seed_m = USER1_INIT_SEED_C_M;
		cr->usr2_delmter = USER2_DELMTER_C;
		cr->usr2_delmter_m = USER2_DELMTER_C_M;
		cr->usr2_eof_padding_len = USER2_EOF_PADDING_LEN_C;	
		cr->usr2_eof_padding_len_m = USER2_EOF_PADDING_LEN_C_M;
		cr->usr2_init_seed = USER2_INIT_SEED_C;
		cr->usr2_init_seed_m = USER2_INIT_SEED_C_M;
		cr->usr3_delmter = USER3_DELMTER_C;	
		cr->usr3_delmter_m = USER3_DELMTER_C_M;
		cr->usr3_eof_padding_len = USER3_EOF_PADDING_LEN_C;
		cr->usr3_eof_padding_len_m = USER3_EOF_PADDING_LEN_C_M;
		cr->usr3_init_seed = USER3_INIT_SEED_C;
		cr->usr3_init_seed_m = USER3_INIT_SEED_C_M;
		cr->vht_sigb0 = TXD_VHT_SIGB0_C;	
		cr->vht_sigb0_m	= TXD_VHT_SIGB0_C_M;
		cr->vht_sigb1 = TXD_VHT_SIGB1_C;	
		cr->vht_sigb1_m	= TXD_VHT_SIGB1_C_M;
		cr->vht_sigb2 = TXD_VHT_SIGB2_C;	
		cr->vht_sigb2_m	= TXD_VHT_SIGB2_C_M;
		cr->he_sigb_mcs = TXCOMCT_HE_SIGB_MCS_C;
		cr->he_sigb_mcs_m = TXCOMCT_HE_SIGB_MCS_C_M;
		cr->vht_sigb3 = TXD_VHT_SIGB3_C;
		cr->vht_sigb3_m = TXD_VHT_SIGB3_C_M;
		cr->n_ltf = TXCOMCT_N_LTF_C;
		cr->n_ltf_m = TXCOMCT_N_LTF_C_M;
		cr->siga1 = TXD_SIGA1_C;	
		cr->siga1_m = TXD_SIGA1_C_M;
		cr->siga2 = TXD_SIGA2_C;	
		cr->siga2_m = TXD_SIGA2_C_M;
		cr->lsig = TXD_LSIG_C;	
		cr->lsig_m = TXD_LSIG_C_M;
		cr->cca_pw_th = TXINFO_CCA_PW_TH_C;
		cr->cca_pw_th_m	= TXINFO_CCA_PW_TH_C_M;
		cr->n_sym = TXTIMCT_N_SYM_C;	
		cr->n_sym_m = TXTIMCT_N_SYM_C_M;
		cr->usr0_service = USER0_SERVICE_C;		
		cr->usr0_service_m = USER0_SERVICE_C_M;	
		cr->usr1_service = USER1_SERVICE_C;		
		cr->usr1_service_m = USER1_SERVICE_C_M;	
		cr->usr2_service = USER2_SERVICE_C;		
		cr->usr2_service_m = USER2_SERVICE_C_M;
		cr->usr3_service = USER3_SERVICE_C;	
		cr->usr3_service_m = USER3_SERVICE_C_M;	
		cr->usr0_mdpu_len_byte = USER0_MDPU_LEN_BYTE_C;
		cr->usr0_mdpu_len_byte_m = USER0_MDPU_LEN_BYTE_C_M;	
		cr->usr1_mdpu_len_byte = USER1_MDPU_LEN_BYTE_C;	
		cr->usr1_mdpu_len_byte_m = USER1_MDPU_LEN_BYTE_C_M;	
		cr->obw_cts2self_dup_type = TXINFO_OBW_CTS2SELF_DUP_TYPE_C;	
		cr->obw_cts2self_dup_type_m = TXINFO_OBW_CTS2SELF_DUP_TYPE_C_M;		
		cr->usr2_mdpu_len_byte = USER2_MDPU_LEN_BYTE_C;
		cr->usr2_mdpu_len_byte_m = USER2_MDPU_LEN_BYTE_C_M;
		cr->usr3_mdpu_len_byte = USER3_MDPU_LEN_BYTE_C;
		cr->usr3_mdpu_len_byte_m = USER3_MDPU_LEN_BYTE_C_M;
		cr->usr0_csi_buf_id = TXUSRCT0_CSI_BUF_ID_C;
		cr->usr0_csi_buf_id_m = TXUSRCT0_CSI_BUF_ID_C_M;
		cr->usr1_csi_buf_id = TXUSRCT1_CSI_BUF_ID_C;
		cr->usr1_csi_buf_id_m = TXUSRCT1_CSI_BUF_ID_C_M;
		cr->rf_gain_idx	= TXINFO_RF_GAIN_IDX_C;	
		cr->rf_gain_idx_m = TXINFO_RF_GAIN_IDX_C_M;
		cr->usr2_csi_buf_id = TXUSRCT2_CSI_BUF_ID_C;
		cr->usr2_csi_buf_id_m = TXUSRCT2_CSI_BUF_ID_C_M;
		cr->usr3_csi_buf_id = TXUSRCT3_CSI_BUF_ID_C;
		cr->usr3_csi_buf_id_m = TXUSRCT3_CSI_BUF_ID_C_M;
		cr->usr0_n_mpdu	= USER0_N_MPDU_C;	
		cr->usr0_n_mpdu_m = USER0_N_MPDU_C_M;	
		cr->usr1_n_mpdu	= USER1_N_MPDU_C;	
		cr->usr1_n_mpdu_m = USER1_N_MPDU_C_M;	
		cr->usr2_n_mpdu	= USER2_N_MPDU_C;	
		cr->usr2_n_mpdu_m = USER2_N_MPDU_C_M;
		cr->usr0_pw_boost_fctr_db = TXUSRCT0_PW_BOOST_FCTR_DB_C;	
		cr->usr0_pw_boost_fctr_db_m = TXUSRCT0_PW_BOOST_FCTR_DB_C_M;	
		cr->usr3_n_mpdu = USER3_N_MPDU_C;
		cr->usr3_n_mpdu_m = USER3_N_MPDU_C_M;		
		cr->ch20_with_data = TXINFO_CH20_WITH_DATA_C;	
		cr->ch20_with_data_m = TXINFO_CH20_WITH_DATA_C_M;
		cr->n_usr = TXINFO_N_USR_C;	
		cr->n_usr_m = TXINFO_N_USR_C_M;	
		cr->txcmd_txtp = TXINFO_TXCMD_TXTP_C;
		cr->txcmd_txtp_m = TXINFO_TXCMD_TXTP_C_M;
		cr->usr0_ru_alloc = TXUSRCT0_RU_ALLOC_C;	
		cr->usr0_ru_alloc_m = TXUSRCT0_RU_ALLOC_C_M;
		cr->usr0_u_id = TXUSRCT0_U_ID_C;
		cr->usr0_u_id_m	= TXUSRCT0_U_ID_C_M;
		cr->usr1_ru_alloc = TXUSRCT1_RU_ALLOC_C;	
		cr->usr1_ru_alloc_m = TXUSRCT1_RU_ALLOC_C_M;
		cr->usr1_u_id = TXUSRCT1_U_ID_C;		
		cr->usr1_u_id_m	= TXUSRCT1_U_ID_C_M;
		cr->usr2_ru_alloc = TXUSRCT2_RU_ALLOC_C;
		cr->usr2_ru_alloc_m = TXUSRCT2_RU_ALLOC_C_M;
		cr->usr2_u_id = TXUSRCT2_U_ID_C;
		cr->usr2_u_id_m	= TXUSRCT2_U_ID_C_M;
		cr->usr3_ru_alloc = TXUSRCT3_RU_ALLOC_C;
		cr->usr3_ru_alloc_m = TXUSRCT3_RU_ALLOC_C_M;
		cr->usr3_u_id = TXUSRCT3_U_ID_C;
		cr->usr3_u_id_m	= TXUSRCT3_U_ID_C_M;
		cr->n_sym_hesigb = TXTIMCT_N_SYM_HESIGB_C;	
		cr->n_sym_hesigb_m = TXTIMCT_N_SYM_HESIGB_C_M;	
		cr->usr0_mcs = TXUSRCT0_MCS_C;
		cr->usr0_mcs_m	= TXUSRCT0_MCS_C_M;
		cr->usr1_mcs = TXUSRCT1_MCS_C;
		cr->usr1_mcs_m	= TXUSRCT1_MCS_C_M;
		cr->usr2_mcs = TXUSRCT2_MCS_C;
		cr->usr2_mcs_m = TXUSRCT2_MCS_C_M;
		cr->usr3_mcs = TXUSRCT3_MCS_C;
		cr->usr3_mcs_m = TXUSRCT3_MCS_C_M;
		cr->usr1_pw_boost_fctr_db = TXUSRCT1_PW_BOOST_FCTR_DB_C;
		cr->usr1_pw_boost_fctr_db_m = TXUSRCT1_PW_BOOST_FCTR_DB_C_M;
		cr->usr2_pw_boost_fctr_db = TXUSRCT2_PW_BOOST_FCTR_DB_C;
		cr->usr2_pw_boost_fctr_db_m = TXUSRCT2_PW_BOOST_FCTR_DB_C_M;
		cr->usr3_pw_boost_fctr_db = TXUSRCT3_PW_BOOST_FCTR_DB_C;
		cr->usr3_pw_boost_fctr_db_m = TXUSRCT3_PW_BOOST_FCTR_DB_C_M;
		cr->ppdu_type = TXINFO_PPDU_TYPE_C;
		cr->ppdu_type_m	= TXINFO_PPDU_TYPE_C_M;
		cr->txsc = TXINFO_TXSC_C;	
		cr->txsc_m = TXINFO_TXSC_C_M;
		cr->cfo_comp = TXINFO_CFO_COMP_C;
		cr->cfo_comp_m = TXINFO_CFO_COMP_C_M;
		cr->pkt_ext_idx = TXTIMCT_PKT_EXT_IDX_C;
		cr->pkt_ext_idx_m = TXTIMCT_PKT_EXT_IDX_C_M;	
		cr->usr0_n_sts = TXUSRCT0_N_STS_C;	
		cr->usr0_n_sts_m = TXUSRCT0_N_STS_C_M;
		cr->usr0_n_sts_ru_tot = TXUSRCT0_N_STS_RU_TOT_C;
		cr->usr0_n_sts_ru_tot_m = TXUSRCT0_N_STS_RU_TOT_C_M;
		cr->usr0_strt_sts = TXUSRCT0_STRT_STS_C;
		cr->usr0_strt_sts_m = TXUSRCT0_STRT_STS_C_M;
		cr->usr1_n_sts = TXUSRCT1_N_STS_C;
		cr->usr1_n_sts_m = TXUSRCT1_N_STS_C_M;
		cr->usr1_n_sts_ru_tot = TXUSRCT1_N_STS_RU_TOT_C;
		cr->usr1_n_sts_ru_tot_m = TXUSRCT1_N_STS_RU_TOT_C_M;
		cr->usr1_strt_sts = TXUSRCT1_STRT_STS_C;	
		cr->usr1_strt_sts_m = TXUSRCT1_STRT_STS_C_M;
		cr->usr2_n_sts = TXUSRCT2_N_STS_C;
		cr->usr2_n_sts_m = TXUSRCT2_N_STS_C_M;
		cr->usr2_n_sts_ru_tot = TXUSRCT2_N_STS_RU_TOT_C;
		cr->usr2_n_sts_ru_tot_m	= TXUSRCT2_N_STS_RU_TOT_C_M;
		cr->usr2_strt_sts = TXUSRCT2_STRT_STS_C;
		cr->usr2_strt_sts_m = TXUSRCT2_STRT_STS_C_M;
		cr->usr3_n_sts = TXUSRCT3_N_STS_C;
		cr->usr3_n_sts_m = TXUSRCT3_N_STS_C_M;
		cr->usr3_n_sts_ru_tot = TXUSRCT3_N_STS_RU_TOT_C;
		cr->usr3_n_sts_ru_tot_m	= TXUSRCT3_N_STS_RU_TOT_C_M;
		cr->usr3_strt_sts = TXUSRCT3_STRT_STS_C;	
		cr->usr3_strt_sts_m = TXUSRCT3_STRT_STS_C_M;
		cr->source_gen_mode_idx	= SOURCE_GEN_MODE_IDX_C;
		cr->source_gen_mode_idx_m = SOURCE_GEN_MODE_IDX_C_M;
		cr->gi_type = TXCOMCT_GI_TYPE_C;
		cr->gi_type_m = TXCOMCT_GI_TYPE_C_M;	
		cr->ltf_type = TXCOMCT_LTF_TYPE_C;
		cr->ltf_type_m = TXCOMCT_LTF_TYPE_C_M;
		cr->dbw_idx = TXINFO_DBW_IDX_C;
		cr->dbw_idx_m = TXINFO_DBW_IDX_C_M;
		cr->pre_fec_fctr = TXTIMCT_PRE_FEC_FCTR_C;
		cr->pre_fec_fctr_m = TXTIMCT_PRE_FEC_FCTR_C_M;
		cr->beam_change_en = TXCOMCT_BEAM_CHANGE_EN_C;
		cr->beam_change_en_m = TXCOMCT_BEAM_CHANGE_EN_C_M;
		cr->doppler_en = TXCOMCT_DOPPLER_EN_C;
		cr->doppler_en_m = TXCOMCT_DOPPLER_EN_C_M;
		cr->fb_mumimo_en = TXCOMCT_FB_MUMIMO_EN_C;
		cr->fb_mumimo_en_m = TXCOMCT_FB_MUMIMO_EN_C_M;
		cr->feedback_status = TXCOMCT_FEEDBACK_STATUS_C;
		cr->feedback_status_m = TXCOMCT_FEEDBACK_STATUS_C_M;	
		cr->he_sigb_dcm_en = TXCOMCT_HE_SIGB_DCM_EN_C;
		cr->he_sigb_dcm_en_m = TXCOMCT_HE_SIGB_DCM_EN_C_M;
		cr->midamble_mode = TXCOMCT_MIDAMBLE_MODE_C;
		cr->midamble_mode_m = TXCOMCT_MIDAMBLE_MODE_C_M;
		cr->mumimo_ltf_mode_en = TXCOMCT_MUMIMO_LTF_MODE_EN_C;
		cr->mumimo_ltf_mode_en_m = TXCOMCT_MUMIMO_LTF_MODE_EN_C_M;
		cr->ndp = TXCOMCT_NDP_C;
		cr->ndp_m = TXCOMCT_NDP_C_M;	
		cr->stbc_en = TXCOMCT_STBC_EN_C;
		cr->stbc_en_m = TXCOMCT_STBC_EN_C_M;
		cr->ant_sel_a = TXINFO_ANT_SEL_A_C;
		cr->ant_sel_a_m	= TXINFO_ANT_SEL_A_C_M;
		cr->ant_sel_b = TXINFO_ANT_SEL_B_C;
		cr->ant_sel_b_m	= TXINFO_ANT_SEL_B_C_M;
		cr->ant_sel_c = TXINFO_ANT_SEL_C_C;
		cr->ant_sel_c_m	= TXINFO_ANT_SEL_C_C_M;
		cr->ant_sel_d = TXINFO_ANT_SEL_D_C;
		cr->ant_sel_d_m	= TXINFO_ANT_SEL_D_C_M;
		cr->cca_pw_th_en = TXINFO_CCA_PW_TH_EN_C;
		cr->cca_pw_th_en_m = TXINFO_CCA_PW_TH_EN_C_M;
		cr->rf_fixed_gain_en = TXINFO_RF_FIXED_GAIN_EN_C;
		cr->rf_fixed_gain_en_m = TXINFO_RF_FIXED_GAIN_EN_C_M;
		cr->ul_cqi_rpt_tri = TXINFO_UL_CQI_RPT_TRI_C;	
		cr->ul_cqi_rpt_tri_m = TXINFO_UL_CQI_RPT_TRI_C_M;
		cr->ldpc_extr = TXTIMCT_LDPC_EXTR_C;	
		cr->ldpc_extr_m	= TXTIMCT_LDPC_EXTR_C_M;
		cr->usr0_dcm_en	= TXUSRCT0_DCM_EN_C;
		cr->usr0_dcm_en_m = TXUSRCT0_DCM_EN_C_M;	
		cr->usr0_fec_type = TXUSRCT0_FEC_TYPE_C;	
		cr->usr0_fec_type_m = TXUSRCT0_FEC_TYPE_C_M;	
		cr->usr0_txbf_en = TXUSRCT0_TXBF_EN_C;		
		cr->usr0_txbf_en_m = TXUSRCT0_TXBF_EN_C_M;
		cr->usr1_dcm_en	= TXUSRCT1_DCM_EN_C;
		cr->usr1_dcm_en_m = TXUSRCT1_DCM_EN_C_M;	
		cr->usr1_fec_type = TXUSRCT1_FEC_TYPE_C;	
		cr->usr1_fec_type_m = TXUSRCT1_FEC_TYPE_C_M;
		cr->usr1_txbf_en = TXUSRCT1_TXBF_EN_C;	
		cr->usr1_txbf_en_m = TXUSRCT1_TXBF_EN_C_M;
		cr->usr2_dcm_en	= TXUSRCT2_DCM_EN_C;
		cr->usr2_dcm_en_m = TXUSRCT2_DCM_EN_C_M;		
		cr->usr2_fec_type = TXUSRCT2_FEC_TYPE_C;		
		cr->usr2_fec_type_m = TXUSRCT2_FEC_TYPE_C_M;	
		cr->usr2_txbf_en = TXUSRCT2_TXBF_EN_C;	
		cr->usr2_txbf_en_m = TXUSRCT2_TXBF_EN_C_M;	
		cr->usr3_dcm_en = TXUSRCT3_DCM_EN_C;
		cr->usr3_dcm_en_m = TXUSRCT3_DCM_EN_C_M;	
		cr->usr3_fec_type = TXUSRCT3_FEC_TYPE_C;	
		cr->usr3_fec_type_m = TXUSRCT3_FEC_TYPE_C_M;
		cr->usr3_txbf_en = TXUSRCT3_TXBF_EN_C;	
		cr->usr3_txbf_en_m = TXUSRCT3_TXBF_EN_C_M;
		break;
	#endif

	default:
		BB_WARNING("[%s] BBCR Hook FAIL!\n", __func__);
		if (bb->bb_dbg_i.cr_fake_init_hook_en) {
			BB_TRACE("[%s] BBCR fake init\n", __func__);
			halbb_cr_hook_fake_init(bb, (u32 *)cr, (sizeof(struct bb_plcp_cr_info) >> 2));
		}
		break;
	}

	if (bb->bb_dbg_i.cr_init_hook_recorder_en) {
		BB_TRACE("[%s] BBCR Hook dump\n", __func__);
		halbb_cr_hook_init_dump(bb, (u32 *)cr, (sizeof(struct bb_plcp_cr_info) >> 2));
	}
}

void halbb_plcp_init(struct bb_info *bb)
{
	bb->dyn_pmac_tri_en = true;
	bb->pmac_tri_en = false;
	bb->pwr_comp_en = true;
}
#else

enum plcp_sts halbb_plcp_gen(struct bb_info *bb, struct halbb_plcp_info *in,
		    struct usr_plcp_gen_in *user, enum phl_phy_idx phy_idx)
{
	return SPEC_INVALID;
}

#endif

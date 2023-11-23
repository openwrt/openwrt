/******************************************************************************
 *
 * Copyright(c) 2007 - 2017  Realtek Corporation.
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

void halbb_cfg_rf_reg_8852c(struct bb_info *bb, u32 addr, u32 data,
			     enum rf_path rf_path, u32 reg_addr)
{
}

void halbb_cfg_rf_radio_a_8852c(struct bb_info *bb, u32 addr, u32 data)
{
}

void halbb_cfg_rf_radio_b_8852c(struct bb_info *bb, u32 addr, u32 data)
{
}

void halbb_cfg_bb_phy_8852c(struct bb_info *bb, u32 addr, u32 data,
			    enum phl_phy_idx phy_idx)
{
#ifdef HALBB_DBCC_SUPPORT
	u32 ofst = 0;
#endif

	if (addr == 0xfe) {
		halbb_delay_ms(bb, 50);
		BB_DBG(bb, DBG_INIT, "Delay 50 ms\n");
	} else if (addr == 0xfd) {
		halbb_delay_ms(bb, 5);
		BB_DBG(bb, DBG_INIT, "Delay 5 ms\n");
	} else if (addr == 0xfc) {
		halbb_delay_ms(bb, 1);
		BB_DBG(bb, DBG_INIT, "Delay 1 ms\n");
	} else if (addr == 0xfb) {
		halbb_delay_us(bb, 50);
		BB_DBG(bb, DBG_INIT, "Delay 50 us\n");
	} else if (addr == 0xfa) {
		halbb_delay_us(bb, 5);
		BB_DBG(bb, DBG_INIT, "Delay 5 us\n");
	} else if (addr == 0xf9) {
		halbb_delay_us(bb, 1);
		BB_DBG(bb, DBG_INIT, "Delay 1 us\n");
	} else {
		#ifdef HALBB_DBCC_SUPPORT
		if ((bb->hal_com->dbcc_en || bb->bb_dbg_i.cr_dbg_mode_en) &&
		    phy_idx == HW_PHY_1) {
			ofst = halbb_phy0_to_phy1_ofst(bb, addr, phy_idx);
			if (ofst == 0)
				return;
			addr += ofst;
		} else {
			phy_idx = HW_PHY_0;
		}
		//if (phy_idx == HW_PHY_1)
		//	BB_DBG(bb, DBG_DBCC, "[REG][%d]0x%04X = 0x%08X\n", phy_idx, addr, data);
		#else
		BB_DBG(bb, DBG_INIT, "[REG]0x%04X = 0x%08X\n", addr, data);
		#endif

		halbb_set_reg(bb, addr, MASKDWORD, data);
	}
}

void halbb_cfg_bb_gain_8852c(struct bb_info *bb, u32 addr, u32 data)
{
	struct bb_gain_info *gain = &bb->bb_gain_i;
	u8 cfg_type = (u8)((addr & 0xff000000) >> 24);
	enum bb_band_t band_idx = (enum bb_band_t)((addr & 0xff0000) >> 16);
	u8 path = (u8)((addr & 0xff00) >> 8);
	u8 type;
	u8 i = 0;

	if (band_idx >= BB_GAIN_BAND_NUM)
		return;

	if (path >= HALBB_MAX_PATH)
		return;

	if (addr == 0xfe) {
		halbb_delay_ms(bb, 50);
		BB_DBG(bb, DBG_INIT, "Delay 50 ms\n");
	} else if (addr == 0xfd) {
		halbb_delay_ms(bb, 5);
		BB_DBG(bb, DBG_INIT, "Delay 5 ms\n");
	} else if (addr == 0xfc) {
		halbb_delay_ms(bb, 1);
		BB_DBG(bb, DBG_INIT, "Delay 1 ms\n");
	} else if (addr == 0xfb) {
		halbb_delay_us(bb, 50);
		BB_DBG(bb, DBG_INIT, "Delay 50 us\n");
	} else if (addr == 0xfa) {
		halbb_delay_us(bb, 5);
		BB_DBG(bb, DBG_INIT, "Delay 5 us\n");
	} else if (addr == 0xf9) {
		halbb_delay_us(bb, 1);
		BB_DBG(bb, DBG_INIT, "Delay 1 us\n");
	} else if (cfg_type ==0) { /*GAIN ERROR*/
		type = (u8)(addr & 0xff);
		if (type == 0) {
			for (i = 0; i < 4; i++)
				gain->lna_gain[band_idx][path][i] = (data >> (8 * i)) & 0xff;
		} else if (type == 1) {
			for (i = 0; i < 3; i++)
				gain->lna_gain[band_idx][path][4 + i] = (data >> (8 * i)) & 0xff;
		} else if (type == 2) {
			for (i = 0; i < 2; i++)
				gain->tia_gain[band_idx][path][i] = (data >> (8 * i)) & 0xff;
		}
	} else if (cfg_type == 1) { /*RPL Offset*/
		halbb_cfg_bb_rpl_ofst(bb, band_idx, path, addr, data);
	} else if (cfg_type ==2) { /*Bypass Mode GAIN ERROR*/
		type = (u8)(addr & 0xff);
		if (type == 0) {
			for (i = 0; i < 4; i++)
				gain->lna_gain_bypass[band_idx][path][i] = (data >> (8 * i)) & 0xff;
		} else if (type == 1) {
			for (i = 0; i < 3; i++)
				gain->lna_gain_bypass[band_idx][path][4 + i] = (data >> (8 * i)) & 0xff;
		}
	} else if (cfg_type == 3) { /*op1dB values*/
		type = (u8)(addr & 0xff);
		if (type == 0) {
			for (i = 0; i < 4; i++)
				gain->lna_op1db[band_idx][path][i] = (data >> (8 * i)) & 0xff;
		} else if (type == 1) {
			for (i = 0; i < 3; i++)
				gain->lna_op1db[band_idx][path][4 + i] = (data >> (8 * i)) & 0xff;
		} else if (type == 2) {
			for (i = 0; i < 4; i++)
				gain->tia_lna_op1db[band_idx][path][i] = (data >> (8 * i)) & 0xff;
		} else if (type == 3) {
			for (i = 0; i < 4; i++)
				gain->tia_lna_op1db[band_idx][path][4 + i] = (data >> (8 * i)) & 0xff;
		}
	} else if (cfg_type == 4) { /*WB_GIDX & GS IDX*/
		type = (u8)(addr & 0xff);
		if (type == 0)
			gain->wb_gidx_elna[band_idx][path] = data;
		else if (type == 1)
			for (i = 0; i < 8; i++)
				gain->wb_gidx_lna_tia[band_idx][path][i] = (data >> (4 * i)) & 0x7;
		else if (type == 2)
			for (i = 0; i < 8; i++)
				gain->wb_gidx_lna_tia[band_idx][path][i + 8] = (data >> (4 * i)) & 0x7;
		else if (type == 3)
			gain->gs_idx[band_idx][path][0] = data;
		else if (type == 4)
			gain->gs_idx[band_idx][path][1] = data;
		else if (type == 5)
			for (i = 0; i < 2; i++)
				gain->g_elna[band_idx][path][i] = (data >> (8 * i)) & 0xff;
	} else {
		BB_WARNING("cfg_type=%d\n", cfg_type);
	}
}

#endif

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

#ifdef HALBB_FW_OFLD_SUPPORT
bool halbb_check_fw_ofld(struct bb_info *bb)
{
	bool ret = bb->phl_com->dev_cap.fw_cap.offload_cap & BIT0;

	BB_DBG(bb, DBG_FW_INFO, "[Offload_cap CHK] FW ofld ret = %d\n", (u8)ret);
	return ret;
}


bool halbb_dbcc_check_fw_ofld(struct bb_info *bb)
{
	bool dbcc_fw_ofld_en = false;

#ifdef HALBB_FW_DBCC_OFLD_SUPPORT
	dbcc_fw_ofld_en = true;
#endif

	BB_DBG(bb, DBG_FW_INFO, "[DBCC Flag CHK]bb_fwofld_sup_bitmap=0x%x, dbcc_fw_ofld_en = %d\n",
	       bb->bb_cmn_hooker->bb_fwofld_sup_bitmap, dbcc_fw_ofld_en);

	return dbcc_fw_ofld_en;
}

void halbb_fwofld_bitmap_en(struct bb_info *bb, bool en, enum fw_ofld_type app)
{
	u32 bb_fwofld_end_time = 0, bb_fwofld_time = 0;

	BB_DBG(bb, DBG_FW_INFO, "[%s] en_opt=%d, app= BIT(%d)\n", __func__, en, app);

	if (!en && bb->bb_cmn_hooker->bb_fwofld_in_progress) {

		halbb_fw_set_reg(bb, 0x1a24, 0xff, 0, 1);
		bb->bb_cmn_hooker->bb_fwofld_in_progress = false;

		BB_DBG(bb, DBG_FW_INFO, "[en:0]fwofld_in_progress= %d\n",
		       bb->bb_cmn_hooker->bb_fwofld_in_progress);

		bb_fwofld_end_time = _os_get_cur_time_ms();

		bb_fwofld_time = DIFF_2(bb_fwofld_end_time, bb->bb_cmn_hooker->bb_fwofld_start_time);

		BB_DBG(bb, DBG_FW_INFO, "[Duration time] %d ms = %d(start) -%d(end)\n",
		       bb_fwofld_time, bb->bb_cmn_hooker->bb_fwofld_start_time, bb_fwofld_end_time);
		return;
	}

	/*=== HALBB Support CHK ==============================================*/
	if (!(BIT(app) & bb->bb_cmn_hooker->bb_fwofld_sup_bitmap)){
		bb->bb_cmn_hooker->bb_fwofld_in_progress = false;
		BB_DBG(bb, DBG_FW_INFO, "NOT Support, Curr_bitmap=%d\n",
		       bb->bb_cmn_hooker->bb_fwofld_sup_bitmap);
		return;
	}
#if 0
	/*=== PHL support CHK ================================================*/
	if (app == FW_OFLD_PHY_0_CR_INIT || app == FW_OFLD_BB_API || app == FW_OFLD_DM_INIT) {
		if (halbb_check_fw_ofld(bb))
			fw_ofld_en = true;
		else
			fw_ofld_en = false;
			
	} else if (app == FW_OFLD_PHY_1_CR_INIT || app == FW_OFLD_PHY_1_DM_INIT || app == FW_OFLD_DBCC_API) {
		if (halbb_dbcc_check_fw_ofld(bb))
			fw_ofld_en = true;
		else
			fw_ofld_en = false;
	}
#endif
	bb->bb_cmn_hooker->bb_fwofld_start_time = _os_get_cur_time_ms();
	BB_DBG(bb, DBG_FW_INFO, "Start time: %d\n",
	       bb->bb_cmn_hooker->bb_fwofld_start_time);

	bb->bb_cmn_hooker->bb_fwofld_in_progress = true;

	BB_DBG(bb, DBG_FW_INFO, "[en:1]fwofld_in_progress= %d\n",
	       bb->bb_cmn_hooker->bb_fwofld_in_progress);
}

void halbb_fwofld_bitmap_init(struct bb_info *bb)
{
	bb->bb_cmn_hooker->bb_fwofld_sup_bitmap =
					#ifdef HALBB_FW_NORMAL_OFLD_SUPPORT
						  BIT(FW_OFLD_PHY_0_CR_INIT) |
						  BIT(FW_OFLD_DM_INIT) |
						  BIT(FW_OFLD_BB_API) |
					#endif
					#ifdef HALBB_FW_DBCC_OFLD_SUPPORT
						  BIT(FW_OFLD_PHY_1_CR_INIT) |
						  BIT(FW_OFLD_PHY_1_DM_INIT) |
						  BIT(FW_OFLD_DBCC_API) |
					#endif
						  0;

	BB_DBG(bb, DBG_FW_INFO, "[%s] bb_fwofld_sup_bitmap = 0x%x\n", __func__,
	       bb->bb_cmn_hooker->bb_fwofld_sup_bitmap);
}

bool halbb_fw_delay(struct bb_info *bb, u32 val)
{
/* halbb_set_reg */
	struct rtw_mac_cmd cmd;
	u32 ret = false;

	cmd.type = RTW_MAC_DELAY_OFLD;
	cmd.lc = 0;
	cmd.value = val; /*delay us*/

	ret = rtw_hal_mac_add_cmd_ofld(bb->hal_com, &cmd);

	BB_DBG(bb, DBG_FW_INFO, "FW ofld delay:%x\n", val);
	if (ret) {
		BB_WARNING("IO offload fail: %d\n", ret);
		return false;
	}
	else {
		return true;
	}
}

bool halbb_fw_set_reg(struct bb_info *bb, u32 addr, u32 mask, u32 val, u8 lc)
{
	struct rtw_mac_cmd cmd;
	u32 ret = false;

	cmd.src = RTW_MAC_BB_CMD_OFLD;
	cmd.type = RTW_MAC_WRITE_OFLD;
	cmd.lc = lc;
	cmd.offset = (u16)addr;
	cmd.value = val;
	cmd.mask = mask;

	ret = rtw_hal_mac_add_cmd_ofld(bb->hal_com, &cmd);
	BB_DBG(bb, DBG_FW_INFO, "FW ofld addr:%x, val:%x, msk:%x\n", addr, val, mask);

	if (ret) {
		BB_WARNING("[%s] IO offload fail: %d\n", __func__, ret);
		return false;
	}
	return true;
}

bool halbb_fw_set_rfreg(struct bb_info *bb, enum rtw_mac_rf_path rf_path, u32 addr, u32 mask, u32 val, u8 lc)
{
/* halbb_set_rf_reg */
	struct rtw_mac_cmd cmd;
	u32 ret;

	cmd.src = RTW_MAC_RF_CMD_OFLD;
	cmd.type = RTW_MAC_WRITE_OFLD;
	cmd.rf_path = rf_path;
	cmd.lc = lc;
	cmd.offset = (u16)addr;
	cmd.value = val;
	cmd.mask = mask;
	ret = rtw_hal_mac_add_cmd_ofld(bb->hal_com, &cmd);
	BB_DBG(bb, DBG_FW_INFO, "FW ofld addr:%x, val:%x, msk:%x\n", addr, val, mask);
	if (ret) {
		BB_WARNING("IO offload fail: %d\n", ret);
		return false;
	}
	else {
		return true;
	}
	
}

bool halbb_fw_set_reg_cmn(struct bb_info *bb, u32 addr, 
			            u32 mask, u32 val, enum phl_phy_idx phy_idx, u8 lc)
{
	bool ret = true;
	u32 val_mod = val;

	addr += halbb_phy0_to_phy1_ofst(bb, addr, phy_idx);

	ret = halbb_fw_set_reg(bb, addr, mask, val_mod, lc);
	return ret;
}

void halbb_fwofld_cfgcr_start(struct bb_info *bb)
{
	bb->bb_cmn_hooker->bbcr_fwofld_state = 1;
}

void halbb_fwofld_cfgcr_end(struct bb_info *bb)
{
	bool ret = true;

	bb->bb_cmn_hooker->bbcr_fwofld_state = 0;

	ret = halbb_fw_set_reg(bb, 0x1a24, 0xff, 0, 1);
}

#ifdef BB_8852A_2_SUPPORT
bool halbb_fwcfg_bb_phy_8852a_2(struct bb_info *bb, u32 addr, u32 data,
			    enum phl_phy_idx phy_idx)
{
#ifdef HALBB_DBCC_SUPPORT
	u32 ofst = 0;
#endif
	bool ret = true;

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
				return true;
			addr += ofst;
		} else {
			phy_idx = HW_PHY_0;
		}
		#endif

		/*FWOFLD in init BB reg flow */
		if (halbb_check_fw_ofld(bb)) {
			ret &= halbb_fw_set_reg(bb, addr, MASKDWORD, data, 0);
			BB_DBG(bb, DBG_INIT, "[REG FWOFLD]0x%04X = 0x%08X, ret = %d\n", addr, data, (u8)ret);
		}
		else {
			halbb_set_reg(bb, addr, MASKDWORD, data);
			#ifdef HALBB_DBCC_SUPPORT
			BB_DBG(bb, DBG_INIT, "[REG][%d]0x%04X = 0x%08X\n", phy_idx, addr, data);
			#else
			BB_DBG(bb, DBG_INIT, "[REG]0x%04X = 0x%08X\n", addr, data);
			#endif
		}

	}
	return ret;
}

bool halbb_fwofld_cck_en_8852a_2(struct bb_info *bb, bool cck_en,
			     enum phl_phy_idx phy_idx)
{
	bool ret = true;

	if (cck_en) {
		ret &= halbb_fw_set_reg(bb, 0x2344, BIT(31), 0, 0);
	} else {
		ret &= halbb_fw_set_reg(bb, 0x2344, BIT(31), 1, 0);
	}
	BB_DBG(bb, DBG_FW_INFO, "[%s]cck_en=%d PHY%d\n", __func__, cck_en, phy_idx);
	return ret;
}

bool halbb_fwofld_btg_8852a_2(struct bb_info *bb, bool btg)
{
	struct rtw_phl_com_t *phl = bb->phl_com;
	struct dev_cap_t *dev = &phl->dev_cap;
	bool ret = true;

	if (dev->rfe_type >= 50)
		return true;
	BB_DBG(bb, DBG_PHY_CONFIG, "<====== %s ======>\n", __func__);
	if (btg) {
		// Path A
		ret &= halbb_fw_set_reg(bb, 0x466c, BIT(18) | BIT(17), 0x1, 0);
		// Path B
		ret &= halbb_fw_set_reg(bb, 0x4740, BIT(18) | BIT(17), 0x3, 0);
		BB_DBG(bb, DBG_PHY_CONFIG, "[BT] Apply BTG Setting\n");
		// Apply Grant BT by TMAC Setting
		ret &= halbb_fw_set_reg(bb, 0x980, 0x1e0000, 0x0, 0);
		BB_DBG(bb, DBG_PHY_CONFIG, "[BT] Apply Grant BT by TMAC Setting\n");
		// Add BT share
		ret &= halbb_fw_set_reg(bb, 0x4978, BIT(14), 0x1, 0);
		ret &= halbb_fw_set_reg(bb, 0x4974, 0x3c00000, 0x2, 0);
		ret &= halbb_fw_set_reg(bb, 0x441c, BIT(31), 0x1, 0);
	} else {
		// Path A
		ret &= halbb_fw_set_reg(bb, 0x466c, BIT(18) | BIT(17), 0x0, 0);
		// Path B
		ret &= halbb_fw_set_reg(bb, 0x4740, BIT(18) | BIT(17), 0x0, 0);
		BB_DBG(bb, DBG_PHY_CONFIG, "[BT] Disable BTG Setting\n");
		// Ignore Grant BT by PMAC Setting
		ret &= halbb_fw_set_reg(bb, 0x980, 0x1e0000, 0xf, 0);
		ret &= halbb_fw_set_reg(bb, 0x980, 0x3c000000, 0x4, 0);
		BB_DBG(bb, DBG_PHY_CONFIG, "[BT] Ignore Grant BT by PMAC Setting\n");
		// Reset BT share
		ret &= halbb_fw_set_reg(bb, 0x4978, BIT(14), 0x0, 0);
		ret &= halbb_fw_set_reg(bb, 0x4974, 0x3c00000, 0x0, 0);
		ret &= halbb_fw_set_reg(bb, 0x441c, BIT(31), 0x0, 0);
	}
	return ret;
}

bool halbb_fw_5m_mask_8852a_2(struct bb_info *bb, u8 pri_ch, enum channel_width bw)
{
	bool mask_5m_low = false;
	bool mask_5m_en = false;
	bool ret = true;

	switch (bw) {
		case CHANNEL_WIDTH_40:
			/* Prich=1 : Mask 5M High
			   Prich=2 : Mask 5M Low */
			mask_5m_en = true;
			mask_5m_low = pri_ch == 2 ? true : false;
			break;
		case CHANNEL_WIDTH_80:
			/* Prich=3 : Mask 5M High
			   Prich=4 : Mask 5M Low 
			   Else    : Mask 5M Disable */
			mask_5m_en = ((pri_ch == 3) || (pri_ch == 4)) ? true : false;
			mask_5m_low = pri_ch == 4 ? true : false;
			break;
		default:
			mask_5m_en = false;
			break;
	}

	BB_DBG(bb, DBG_PHY_CONFIG, "[5M Mask] pri_ch = %d, bw = %d", pri_ch, bw);

	if (!mask_5m_en) {
		ret &= halbb_fw_set_reg(bb, 0x46b0, BIT(12), 0x0, 0);
		ret &= halbb_fw_set_reg(bb, 0x4784, BIT(12), 0x0, 0);
	} else {
		if (mask_5m_low) {
			ret &= halbb_fw_set_reg(bb, 0x46b0, 0x3f, 0x4, 0);
			ret &= halbb_fw_set_reg(bb, 0x46b0, BIT(12) | BIT(8) | BIT(6), 0x5, 0);
			ret &= halbb_fw_set_reg(bb, 0x4784, 0x3f, 0x4, 0);
			ret &= halbb_fw_set_reg(bb, 0x4784, BIT(12) | BIT(8) | BIT(6), 0x5, 0);
		} else {
			ret &= halbb_fw_set_reg(bb, 0x46b0, 0x3f, 0x4, 0);
			ret &= halbb_fw_set_reg(bb, 0x46b0, BIT(12) | BIT(8) | BIT(6), 0x6, 0);
			ret &= halbb_fw_set_reg(bb, 0x4784, 0x3f, 0x4, 0);
			ret &= halbb_fw_set_reg(bb, 0x4784, BIT(12) | BIT(8) | BIT(6), 0x6, 0);
		}
	}
	return ret;
}

bool halbb_fw_set_rf_reg_8852a_2(struct bb_info *bb, enum rf_path path,
			      u32 reg_addr, u32 bit_mask, u32 data, u8 lc)
{
/* halbb_write_rf_reg_8852a_2 */
	bool ret = true;
	u32 direct_addr = 0;
	u32 offset_write_rf[2] = {0xc000, 0xd000};

	/*==== Error handling ====*/
	if (path > RF_PATH_B) {
		BB_WARNING("[%s] Unsupported path (%d)\n", __func__, path);
		return false;
	}

	/*==== Calculate offset ====*/
	reg_addr &= 0xff;
	direct_addr = offset_write_rf[path] + (reg_addr << 2);

	/*==== RF register only has 20bits ====*/
	bit_mask &= RFREGOFFSETMASK;
	halbb_fw_delay(bb, 1);
	/*==== Write RF register directly ====*/
	ret = halbb_fw_set_reg(bb, direct_addr, bit_mask, data, lc);

	/*halbb_delay_us(bb, 1);*/
	halbb_fw_delay(bb, 1);

	BB_DBG(bb, DBG_FW_INFO, "FW OFLD RF-%d 0x%x = 0x%x , bit mask = 0x%x, ret = %d\n",
	       path, reg_addr, data, bit_mask, (u8)ret);

	return ret;
}

bool halbb_fw_ofld_rf_reg_8852a_2(struct bb_info *bb, enum rf_path path,
			      u32 reg_addr, u32 bit_mask, u32 data, u8 lc)
{
/* halbb_write_rf_reg_8852a_2 */
	bool ret = true;

	/*==== Error handling ====*/
	if (path > RF_PATH_B) {
		BB_WARNING("[%s] Unsupported path (%d)\n", __func__, path);
		return false;
	}

	/*==== Calculate offset ====*/

	/*==== RF register only has 20bits ====*/
	bit_mask &= RFREGOFFSETMASK;
	halbb_fw_delay(bb, 1);
	/*==== Write RF register directly ====*/
	ret = halbb_fw_set_rfreg(bb, path, reg_addr, bit_mask, data, lc);
	halbb_fw_delay(bb, 1);

	BB_DBG(bb, DBG_FW_INFO, "FW OFLD RF-%d 0x%x = 0x%x , bit mask = 0x%x, ret = %d\n",
	       path, reg_addr, data, bit_mask, (u8)ret);

	return ret;
}

bool halbb_fw_set_efuse_8852a_2(struct bb_info *bb, u8 central_ch, enum rf_path path, enum phl_phy_idx phy_idx)
{
	u8 band;
	bool ret = true;
	u8 upper_bound = 60; // S(7,4): 3.75
	u8 lower_bound = 64; // S(7,4): -4
	s32 hidden_efuse = 0, normal_efuse = 0, normal_efuse_cck = 0;
	s32 tmp = 0;
	u32 efuse_5g[BB_PATH_MAX_8852A] = {0x4624, 0x46f8};
	u32 efuse_5g_mask = 0x07e00000;
	u32 efuse_2g[BB_PATH_MAX_8852A] = {0x4628, 0x46fc};
	u32 efuse_2g_mask = 0x0000003f;

	BB_DBG(bb, DBG_PHY_CONFIG, "<====== %s ======>\n", __func__);
	
	// 2G Band: (0)
	// 5G Band: (1):Low, (2): Mid, (3):High
	if (central_ch >= 0 && central_ch <= 14)
		band = 0;
	else if (central_ch >= 36 && central_ch <= 64)
		band = 1;
	else if (central_ch >= 100 && central_ch <= 144)
		band = 2;
	else if (central_ch >= 149 && central_ch <= 177)
		band = 3;
	else
		band = 0;

	// === [Set hidden efuse] === //
	if (bb->bb_efuse_i.hidden_efuse_check) {
		for (path = RF_PATH_A; path < BB_PATH_MAX_8852A; path++) {
			if (central_ch >= 0 && central_ch <= 14) {
				hidden_efuse = (bb->bb_efuse_i.gain_cg[path][band] << 2);
				ret &= halbb_fw_set_reg(bb, efuse_2g[path], efuse_2g_mask, (hidden_efuse & 0x3f), 0);
			} else {
				hidden_efuse = (bb->bb_efuse_i.gain_cg[path][band] << 2);
				ret &= halbb_fw_set_reg(bb, efuse_5g[path], efuse_5g_mask, (hidden_efuse & 0x3f), 0);
			}
		}
		BB_DBG(bb, DBG_INIT, "[Efuse][FWOFLD] Hidden efuse dynamic setting!!\n");
	} else {
		BB_DBG(bb, DBG_INIT, "[Efuse][FWOFLD] Values of hidden efuse are all 0xff, bypass dynamic setting!!\n");
	}	

	// === [Set normal efuse] === //
	if (bb->bb_efuse_i.normal_efuse_check) {
		BB_DBG(bb, DBG_DBCC, "[%s] bb->rx_path=%d , phy_idx=%d, dbcc_en=%d\n", __func__, bb->rx_path, phy_idx, bb->hal_com->dbcc_en);
		if ((bb->rx_path == RF_PATH_A) || (bb->rx_path == RF_PATH_AB)) {
			normal_efuse = bb->bb_efuse_i.gain_offset[RF_PATH_A][band + 1];
			normal_efuse_cck = bb->bb_efuse_i.gain_offset[RF_PATH_A][0];
		} else if (bb->rx_path == RF_PATH_B) {
			normal_efuse = bb->bb_efuse_i.gain_offset[RF_PATH_B][band + 1];
			normal_efuse_cck = bb->bb_efuse_i.gain_offset[RF_PATH_B][0];
		}
		normal_efuse *= (-1);
		normal_efuse_cck *= (-1);

		// OFDM normal efuse
		if (normal_efuse > 3) {
			tmp = (normal_efuse << 4) + (bb->bb_efuse_i.efuse_ofst[HW_PHY_0] << 2) - upper_bound;
			ret &= halbb_fw_set_reg_cmn(bb, 0x494c, 0xf8000000, ((tmp >> 2) & 0x1f), phy_idx, 0);
			ret &= halbb_fw_set_reg_cmn(bb, 0x4964, 0xfe00000, (tmp & 0x7f), phy_idx, 0);
			// Set efuse
			ret &= halbb_fw_set_reg_cmn(bb, 0x4960, 0xfe00000, (upper_bound & 0x7f), phy_idx, 0);
			ret &= halbb_fw_set_reg_cmn(bb, 0x4964, 0x7f, (upper_bound & 0x7f), phy_idx, 0);
			ret &= halbb_fw_set_reg_cmn(bb, 0x4964, 0x3f80, (upper_bound & 0x7f), phy_idx, 0);
		} else if (normal_efuse < -4) {
			tmp = (normal_efuse << 4) + (bb->bb_efuse_i.efuse_ofst[HW_PHY_0] << 2) + lower_bound;
			// r_1_rpl_bias_comp
			ret &= halbb_fw_set_reg_cmn(bb, 0x494c, 0xf8000000, ((tmp >> 2) & 0x1f), phy_idx, 0);
			// r_tb_rssi_bias_comp
			ret &= halbb_fw_set_reg_cmn(bb, 0x4964, 0xfe00000, (tmp & 0x7f), phy_idx, 0);
			// Set efuse
			ret &= halbb_fw_set_reg_cmn(bb, 0x4960, 0xfe00000, (lower_bound & 0x7f), phy_idx, 0);
			ret &= halbb_fw_set_reg_cmn(bb, 0x4964, 0x7f, (lower_bound & 0x7f), phy_idx, 0);
			ret &= halbb_fw_set_reg_cmn(bb, 0x4964, 0x3f80, (lower_bound & 0x7f), phy_idx, 0);
		} else {
			ret &= halbb_fw_set_reg_cmn(bb, 0x494c, 0xf8000000, (bb->bb_efuse_i.efuse_ofst[HW_PHY_0] & 0x1f), phy_idx, 0);
			ret &= halbb_fw_set_reg_cmn(bb, 0x4964, 0xfe00000, (bb->bb_efuse_i.efuse_ofst_tb[HW_PHY_0] & 0x7f), phy_idx, 0);
			// Set efuse
			ret &= halbb_fw_set_reg_cmn(bb, 0x4960, 0xfe00000, ((normal_efuse << 4) & 0x7f), phy_idx, 0);
			ret &= halbb_fw_set_reg_cmn(bb, 0x4964, 0x7f, ((normal_efuse << 4) & 0x7f), phy_idx, 0);
			ret &= halbb_fw_set_reg_cmn(bb, 0x4964, 0x3f80, ((normal_efuse << 4) & 0x7f), phy_idx, 0);
		}

		// CCK normal efuse
		if (band == 0) {
			tmp = normal_efuse_cck << 3;
			ret &= halbb_fw_set_reg(bb, 0x23ac, 0x7f, (tmp & 0x7f), 0);
		}

		BB_DBG(bb, DBG_INIT, "[Efuse][FWOFLD] Normal efuse dynamic setting!!\n");
	} else {
		BB_DBG(bb, DBG_INIT, "[Efuse][FWOFLD] Values of normal efuse are all 0xff, bypass dynamic setting!!\n");
	}
	return ret;
}
bool halbb_fw_set_gain_error_8852a_2(struct bb_info *bb, u8 central_ch)
{
	bool ret = true;
	u8 band;
	u8 path = 0, lna_idx = 0, tia_idx = 0;
	s32 tmp = 0;
	u32 lna_err_a_5g[BB_PATH_MAX_8852A][7] = {{0x462c, 0x462c, 0x4630,
						   0x4634, 0x4634, 0x4638,
						   0x4638}, {0x4700, 0x4700,
						   0x4704, 0x4708, 0x4708,
						   0x470c, 0x470c}};
	u32 lna_err_a_5g_mask[7] = {0x00000fc0, 0x3f000000, 0x0003f000,
				    0x0000003f, 0x00fc0000, 0x00000fc0,
				    0x3f000000};
	u32 lna_err_a_2g[BB_PATH_MAX_8852A][7] = {{0x462c, 0x4630, 0x4630,
						   0x4634, 0x4634, 0x4638,
						   0x463c}, {0x4700, 0x4704,
						   0x4704, 0x4708, 0x4708,
						   0x470c, 0x4710}};
	u32 lna_err_a_2g_mask[7] = {0x0003f000, 0x0000003f, 0x00fc0000,
				    0x00000fc0, 0x3f000000, 0x0003f000,
				    0x0000003f};
	u32 tia_err_a_5g[BB_PATH_MAX_8852A][2] = {{0x4640, 0x4644}, {0x4714,
						   0x4718}};
	u32 tia_err_a_5g_mask[2] = {0x0003f000, 0x0000003f};
	u32 tia_err_a_2g[BB_PATH_MAX_8852A][2] = {{0x4640, 0x4644}, {0x4714,
						   0x4718}};
	u32 tia_err_a_2g_mask[2] = {0x00fc0000, 0x00000fc0};

	BB_DBG(bb, DBG_PHY_CONFIG, "<====== %s ======>\n", __func__);

	// 2G Band: (0)
	// 5G Band: (1):Low, (2): Mid, (3):High
	if (central_ch >= 0 && central_ch <= 14)
		band = 0;
	else if (central_ch >= 36 && central_ch <= 64)
		band = 1;
	else if (central_ch >= 100 && central_ch <= 144)
		band = 2;
	else if (central_ch >= 149 && central_ch <= 177)
		band = 3;
	else
		band = 0;

	if (central_ch >= 0 && central_ch <= 14) {
		for (path = RF_PATH_A; path < BB_PATH_MAX_8852A; path++) {
			// Set 2G LNA Gain Err
			for (lna_idx = 0; lna_idx < 7; lna_idx++) {
				tmp = bb->bb_gain_i.lna_gain[band][path][lna_idx];

				ret &= halbb_fw_set_reg(bb, lna_err_a_2g[path][lna_idx],
					      lna_err_a_2g_mask[lna_idx], (tmp & 0x3f), 0);
				
			}
			// Set 2G TIA Gain Err
			for (tia_idx = 0; tia_idx < 2; tia_idx++) {
				tmp = bb->bb_gain_i.tia_gain[band][path][tia_idx];
				ret &= halbb_fw_set_reg(bb, tia_err_a_2g[path][tia_idx],
					      tia_err_a_2g_mask[tia_idx], (tmp & 0x3f), 0);
			}
		}
	} else {
		for (path = RF_PATH_A; path < BB_PATH_MAX_8852A; path++) {
			// Set 5G LNA Gain Err
			for (lna_idx = 0; lna_idx < 7; lna_idx++) {
				tmp = bb->bb_gain_i.lna_gain[band][path][lna_idx];

				ret &= halbb_fw_set_reg(bb, lna_err_a_5g[path][lna_idx],
					      lna_err_a_5g_mask[lna_idx], (tmp & 0x3f), 0);
			}
			// Set 5G TIA Gain Err
			for (tia_idx = 0; tia_idx < 2; tia_idx++) {
				tmp = bb->bb_gain_i.tia_gain[band][path][tia_idx];
				ret &= halbb_fw_set_reg(bb, tia_err_a_5g[path][tia_idx],
					      tia_err_a_5g_mask[tia_idx], (tmp & 0x3f), 0);
			}
		}
	}
	return ret;
}
bool halbb_fwofld_sco_cck_8852a_2(struct bb_info *bb, u8 pri_ch)
{
	u32 sco_barker_threshold[14] = {0x1cfea, 0x1d0e1, 0x1d1d7, 0x1d2cd,
					0x1d3c3, 0x1d4b9, 0x1d5b0, 0x1d6a6,
					0x1d79c, 0x1d892, 0x1d988, 0x1da7f,
					0x1db75, 0x1ddc4};
	u32 sco_cck_threshold[14] = {0x27de3, 0x27f35, 0x28088, 0x281da,
				     0x2832d, 0x2847f, 0x285d2, 0x28724,
				     0x28877, 0x289c9, 0x28b1c, 0x28c6e,
				     0x28dc1, 0x290ed};
	bool rpt = true;

	if (pri_ch > 14) {
		BB_DBG(bb, DBG_PHY_CONFIG, "[CCK SCO Fail]");
		/*Return true because its not FW offload fail*/
		return true;
	}

	rpt &= halbb_fw_set_reg(bb, 0x23b0, 0x7ffff, sco_barker_threshold[pri_ch - 1], 0);
	rpt &= halbb_fw_set_reg(bb, 0x23b4, 0x7ffff, sco_cck_threshold[pri_ch - 1], 0);
	BB_DBG(bb, DBG_PHY_CONFIG, "[CCK SCO Success]");
	return rpt;
}

bool halbb_fwofld_rf_ch_8852a_2(struct bb_info *bb, u8 central_ch, enum rf_path path,
			    bool *is_2g_ch, u32 *rf_reg18)
{
	//u32 rf_reg18 = 0;
	bool ret = true;

	/*rf_reg18 = halbb_read_rf_reg_8852a_2(bb, path, 0x18, RFREGOFFSETMASK);*/
	/*==== [Error handling] ====*/
	if (*rf_reg18 == INVALID_RF_DATA) {
		BB_WARNING("Invalid RF_0x18 for Path-%d\n", path);
		return false;
	}
	*is_2g_ch = (central_ch <= 14) ? true : false;
	/*==== [Set RF Reg 0x18] ====*/
	*rf_reg18 &= ~0x303ff; /*[17:16],[9:8],[7:0]*/
	*rf_reg18 |= central_ch; /* Channel*/
	/*==== [5G Setting] ====*/
	if (!*is_2g_ch)
		*rf_reg18 |= (BIT(16) | BIT(8));
	/*ret &= halbb_fw_set_rf_reg_8852a_2(bb, path, 0x18, RFREGOFFSETMASK, *rf_reg18);*/

	BB_DBG(bb, DBG_PHY_CONFIG, "[Success][ch_setting] CH: %d for Path-%d\n",
	       central_ch, path);
	return ret;
}

bool halbb_fwofld_ch_8852a_2(struct bb_info *bb, u8 central_ch,
			 enum phl_phy_idx phy_idx, u32 *path0_rf18, u32 *path1_rf18)
{
	u8 sco_comp;
	bool is_2g_ch = false;
	bool ret = true;

	if (bb->is_disable_phy_api) {
		BB_DBG(bb, DBG_PHY_CONFIG, "[%s] Disable PHY API\n", __func__);
		return true;
	}
	/*==== Error handling ====*/
	if ((central_ch > 14 && central_ch < 36) ||
	    (central_ch > 64 && central_ch < 100) ||
	    (central_ch > 144 && central_ch < 149) ||
	    central_ch > 177 ) {
		BB_WARNING("Invalid CH:%d for PHY%d\n", central_ch,
			   phy_idx);
		return false;
	}

	if (phy_idx == HW_PHY_0) {
		/*============== [Path A] ==============*/
		ret &= halbb_fwofld_rf_ch_8852a_2(bb, central_ch, RF_PATH_A, &is_2g_ch, path0_rf18);
		//------------- [Mode Sel - Path A] ------------//
		if (is_2g_ch)
			ret &= halbb_fw_set_reg_cmn(bb, 0x4644, BIT(31) | BIT(30), 1,
					  phy_idx, 0);
		else
			ret &= halbb_fw_set_reg_cmn(bb, 0x4644, BIT(31) | BIT(30), 0,
					  phy_idx, 0);

		/*============== [Path B] ==============*/
		if (!bb->hal_com->dbcc_en) {
			ret &= halbb_fwofld_rf_ch_8852a_2(bb, central_ch, RF_PATH_B,
					       &is_2g_ch, path1_rf18);
			//------------- [Mode Sel - Path B] ------------//
			if (is_2g_ch)
				ret &= halbb_fw_set_reg_cmn(bb, 0x4718, BIT(31) | BIT(30),
						  1, phy_idx, 0);
			else
				ret &= halbb_fw_set_reg_cmn(bb, 0x4718, BIT(31) | BIT(30),
						  0, phy_idx, 0);
		} else { /*==== [Phy0 config at 2/5G] ====*/
			if (is_2g_ch)
				ret &= halbb_fw_set_reg(bb, 0x4970, BIT(1), 0, 0);
			else
				ret &= halbb_fw_set_reg(bb, 0x4970, BIT(1), 1, 0);
		}
		/*==== [SCO compensate fc setting] ====*/
		sco_comp = halbb_sco_mapping_8852a_2(bb, central_ch);
		ret &= halbb_fw_set_reg_cmn(bb, 0x4974, 0x7f, sco_comp, phy_idx, 0);
	} else {
		/*============== [Path B] ==============*/
		ret &= halbb_fwofld_rf_ch_8852a_2(bb, central_ch, RF_PATH_B, &is_2g_ch, path0_rf18);

		//------------- [Mode Sel - Path B] ------------//
		if (is_2g_ch)
			ret &= halbb_fw_set_reg_cmn(bb, 0x4718, BIT(31) | BIT(30), 1,
					  phy_idx, 0);
		else
			ret &= halbb_fw_set_reg_cmn(bb, 0x4718, BIT(31) | BIT(30), 0,
					  phy_idx, 0);

		/*==== [SCO compensate fc setting] ====*/
		sco_comp = halbb_sco_mapping_8852a_2(bb, central_ch);
		ret &= halbb_fw_set_reg_cmn(bb, 0x4974, 0x7f, sco_comp, phy_idx, 0);
	}

	/* === Bandedge ===*/
	if (is_2g_ch)
		ret &= halbb_fw_set_reg_cmn(bb, 0x4498, BIT(30), 1, phy_idx, 0);
	else
		ret &= halbb_fw_set_reg_cmn(bb, 0x4498, BIT(30), 0, phy_idx, 0);
	/* === CCK Parameters === */
	if (central_ch == 14) {
		ret &= halbb_fw_set_reg(bb, 0x2300, 0xffffff, 0x3b13ff, 0);
		ret &= halbb_fw_set_reg(bb, 0x2304, 0xffffff, 0x1c42de, 0);
		ret &= halbb_fw_set_reg(bb, 0x2308, 0xffffff, 0xfdb0ad, 0);
		ret &= halbb_fw_set_reg(bb, 0x230c, 0xffffff, 0xf60f6e, 0);
		ret &= halbb_fw_set_reg(bb, 0x2310, 0xffffff, 0xfd8f92, 0);
		ret &= halbb_fw_set_reg(bb, 0x2314, 0xffffff, 0x2d011, 0);
		ret &= halbb_fw_set_reg(bb, 0x2318, 0xffffff, 0x1c02c, 0);
		ret &= halbb_fw_set_reg(bb, 0x231c, 0xffffff, 0xfff00a, 0);
	} else {	
		ret &= halbb_fw_set_reg(bb, 0x2300, 0xffffff, 0x3d23ff, 0);
		ret &= halbb_fw_set_reg(bb, 0x2304, 0xffffff, 0x29b354, 0);
		ret &= halbb_fw_set_reg(bb, 0x2308, 0xffffff, 0xfc1c8, 0);
		ret &= halbb_fw_set_reg(bb, 0x230c, 0xffffff, 0xfdb053, 0);
		ret &= halbb_fw_set_reg(bb, 0x2310, 0xffffff, 0xf86f9a, 0);
		ret &= halbb_fw_set_reg(bb, 0x2314, 0xffffff, 0xfaef92, 0);
		ret &= halbb_fw_set_reg(bb, 0x2318, 0xffffff, 0xfe5fcc, 0);
		ret &= halbb_fw_set_reg(bb, 0x231c, 0xffffff, 0xffdff5, 0);
	}
	/* === Set Gain Error === */
	ret &= halbb_fw_set_gain_error_8852a_2(bb, central_ch);
	/* === Set Efuse === */
	BB_DBG(bb, DBG_DBCC, "[%s] bb->rx_path=%d , phy_idx=%d, dbcc_en=%d\n", __func__, bb->rx_path, phy_idx, bb->hal_com->dbcc_en);
	ret &= halbb_fw_set_efuse_8852a_2(bb, central_ch, bb->rx_path, phy_idx);

	/* === Set Ch idx report in phy-sts === */
	/* write for last cmd*/
	ret &= halbb_fw_set_reg_cmn(bb, 0x0734, 0x0ff0000, central_ch, phy_idx, 0);

	BB_DBG(bb, DBG_PHY_CONFIG, "[Switch CH] CH: %d for PHY%d, ret = %d\n",
	       central_ch, phy_idx, (u8)ret);
	return ret;
}

bool halbb_fw_bw_setting_8852a_2(struct bb_info *bb, enum channel_width bw,
			    enum rf_path path, u32 *rf_reg18)
{
	bool ret = true;
	/*u32 rf_reg18 = 0;*/
	u32 adc_sel[2] = {0x12d0, 0x32d0};
	u32 wbadc_sel[2] = {0x12ec, 0x32ec};

	/*rf_reg18 = halbb_read_rf_reg_8852a_2(bb, path, 0x18, RFREGOFFSETMASK);*/
	/*==== [Error handling] ====*/
	if (*rf_reg18 == INVALID_RF_DATA) {
		BB_WARNING("Invalid RF_0x18 for Path-%d\n", path);
		return false;
	}
	*rf_reg18 &= ~(BIT(11) | BIT(10));
	/*==== [Switch bandwidth] ====*/
	switch (bw) {
	case CHANNEL_WIDTH_5:
	case CHANNEL_WIDTH_10:
	case CHANNEL_WIDTH_20:
		if (bw == CHANNEL_WIDTH_5) {
			/*ADC clock = 20M & WB ADC clock = 40M for BW5 */
			ret &= halbb_fw_set_reg(bb, adc_sel[path], 0x6000, 0x1, 0);
			ret &= halbb_fw_set_reg(bb, wbadc_sel[path], 0x30, 0x0, 0);
		} else if (bw == CHANNEL_WIDTH_10) {
			/*ADC clock = 40M & WB ADC clock = 80M for BW10 */
			ret &= halbb_fw_set_reg(bb, adc_sel[path], 0x6000, 0x2, 0);
			ret &= halbb_fw_set_reg(bb, wbadc_sel[path], 0x30, 0x1, 0);
		} else if (bw == CHANNEL_WIDTH_20) {
			/*ADC clock = 80M & WB ADC clock = 160M for BW20 */
			ret &= halbb_fw_set_reg(bb, adc_sel[path], 0x6000, 0x0, 0);
			ret &= halbb_fw_set_reg(bb, wbadc_sel[path], 0x30, 0x2, 0);
		}

		/*RF bandwidth */
		*rf_reg18 |= (BIT(11) | BIT(10));

		break;
	case CHANNEL_WIDTH_40:
		/*ADC clock = 80M & WB ADC clock = 160M for BW40 */
		ret &= halbb_fw_set_reg(bb, adc_sel[path], 0x6000, 0x0, 0);
		ret &= halbb_fw_set_reg(bb, wbadc_sel[path], 0x30, 0x2, 0);

		/*RF bandwidth */
		*rf_reg18 |= BIT(11);

		break;
	case CHANNEL_WIDTH_80:
		/*ADC clock = 160M & WB ADC clock = 160M for BW40 */
		ret &= halbb_fw_set_reg(bb, adc_sel[path], 0x6000, 0x0, 0);
		ret &= halbb_fw_set_reg(bb, wbadc_sel[path], 0x30, 0x2, 0);

		/*RF bandwidth */
		*rf_reg18 |= BIT(10);

		break;
	default:
		BB_WARNING("Fail to set ADC\n");
	}

	/*==== [Write RF register] ====*/
	/*ret &= halbb_fw_set_rf_reg_8852a_2(bb, path, 0x18, RFREGOFFSETMASK, rf_reg18);*/
	BB_DBG(bb, DBG_PHY_CONFIG,
	       "[bw_setting] ADC setting for Path-%d\n, ret = %d", path, (u8)ret);
	return ret;
}

bool halbb_fwofld_bw_8852a_2(struct bb_info *bb, u8 pri_ch, enum channel_width bw,
			 enum phl_phy_idx phy_idx, u32 *path0_rf18, u32 *path1_rf18)
{
	bool ret = true;

	if (bb->is_disable_phy_api) {
		BB_DBG(bb, DBG_PHY_CONFIG, "[%s] Disable PHY API\n", __func__);
		return true;
	}

	/*==== Error handling ====*/
	if (bw >= CHANNEL_WIDTH_MAX || (bw == CHANNEL_WIDTH_40 && pri_ch > 2) ||
	    (bw == CHANNEL_WIDTH_80 && pri_ch > 4)) {
		BB_WARNING("Fail to switch bw(bw:%d, pri ch:%d)\n", bw,
			   pri_ch);
		return true;
	}

	/*==== Switch bandwidth ====*/
	switch (bw) {
	case CHANNEL_WIDTH_5:
	case CHANNEL_WIDTH_10:
	case CHANNEL_WIDTH_20:
		if (bw == CHANNEL_WIDTH_5) {
			/*RF_BW:[31:30]=0x0 */
			ret &= halbb_fw_set_reg_cmn(bb, 0x4974, 0xC0000000, 0x0,
					  phy_idx, 0);
			/*small BW:[13:12]=0x1 */
			ret &= halbb_fw_set_reg_cmn(bb, 0x4978, 0x3000, 0x1, phy_idx, 0);
			/*Pri ch:[11:8]=0x0 */
			ret &= halbb_fw_set_reg_cmn(bb, 0x4978, 0xf00, 0x0, phy_idx, 0);
		} else if (bw == CHANNEL_WIDTH_10) {
			/*RF_BW:[31:30]=0x0 */
			ret &= halbb_fw_set_reg_cmn(bb, 0x4974, 0xC0000000, 0x0,
					  phy_idx, 0);
			/*small BW:[13:12]=0x2 */
			ret &= halbb_fw_set_reg_cmn(bb, 0x4978, 0x3000, 0x2, phy_idx, 0);
			/*Pri ch:[11:8]=0x0 */
			ret &= halbb_fw_set_reg_cmn(bb, 0x4978, 0xf00, 0x0, phy_idx, 0);
		} else if (bw == CHANNEL_WIDTH_20) {
			/*RF_BW:[31:30]=0x0 */
			ret &= halbb_fw_set_reg_cmn(bb, 0x4974, 0xC0000000, 0x0,
					  phy_idx, 0);
			/*small BW:[13:12]=0x0 */
			ret &= halbb_fw_set_reg_cmn(bb, 0x4978, 0x3000, 0x0, phy_idx, 0);
			/*Pri ch:[11:8]=0x0 */
			ret &= halbb_fw_set_reg_cmn(bb, 0x4978, 0xf00, 0x0, phy_idx, 0);
		}

		break;
	case CHANNEL_WIDTH_40:
		/*RF_BW:[31:30]=0x1 */
		ret &= halbb_fw_set_reg_cmn(bb, 0x4974, 0xC0000000, 0x1, phy_idx, 0);
		/*small BW:[13:12]=0x0 */
		ret &= halbb_fw_set_reg_cmn(bb, 0x4978, 0x3000, 0x0, phy_idx, 0);
		/*Pri ch:[11:8] */
		ret &= halbb_fw_set_reg_cmn(bb, 0x4978, 0xf00, pri_ch, phy_idx, 0);
		/*CCK primary channel */
		if (pri_ch == 1)
			ret &= halbb_fw_set_reg(bb, 0x237c, BIT(0), 1, 0);
		else
			ret &= halbb_fw_set_reg(bb, 0x237c, BIT(0), 0, 0);

		break;
	case CHANNEL_WIDTH_80:
		/*RF_BW:[31:30]=0x2 */
		ret &= halbb_fw_set_reg_cmn(bb, 0x4974, 0xC0000000, 0x2, phy_idx, 0);
		/*small BW:[13:12]=0x0 */
		ret &= halbb_fw_set_reg_cmn(bb, 0x4978, 0x3000, 0x0, phy_idx, 0);
		/*Pri ch:[11:8] */
		ret &= halbb_fw_set_reg_cmn(bb, 0x4978, 0xf00, pri_ch, phy_idx, 0);

		break;
	default:
		BB_WARNING("Fail to switch bw (bw:%d, pri ch:%d)\n", bw,
			   pri_ch);
	}

	if (phy_idx == HW_PHY_0) {
		/*============== [Path A] ==============*/
		ret &= halbb_fw_bw_setting_8852a_2(bb, bw, RF_PATH_A, path0_rf18);
		/*============== [Path B] ==============*/
		if (!bb->hal_com->dbcc_en)
			ret &= halbb_fw_bw_setting_8852a_2(bb, bw, RF_PATH_B, path1_rf18);
	} else {
		/*============== [Path B] ==============*/
		ret &= halbb_fw_bw_setting_8852a_2(bb, bw, RF_PATH_B, path1_rf18);
	}

	BB_DBG(bb, DBG_PHY_CONFIG,
		  "[Switch BW] BW: %d for PHY%d\n, ret = %d", bw, phy_idx, (u8)ret);

	return ret;
}

bool halbb_fwofld_bw_ch_8852a_2(struct bb_info *bb, u8 pri_ch, u8 central_ch,
			    enum channel_width bw, enum phl_phy_idx phy_idx)
{
	bool rpt = true;
	bool cck_en = false;
	bool is_2g_ch = false;
	u8 pri_ch_idx = 0;
	u32 path0_rf18 = 0;
	u32 path1_rf18 = 0;

	/*==== [Set pri_ch idx] ====*/
	if (central_ch <= 14) {
		// === 2G === //
		switch (bw) {
		case CHANNEL_WIDTH_20:
			break;

		case CHANNEL_WIDTH_40:
			pri_ch_idx = pri_ch > central_ch ? 1 : 2;
			break;

		default:
			break;
		}

		/*==== [CCK SCO Compesate] ====*/
		rpt &= halbb_fwofld_sco_cck_8852a_2(bb, pri_ch);

		cck_en = true;
		is_2g_ch = true;
	} else {
		// === 5G === //
		switch (bw) {
		case CHANNEL_WIDTH_20:
			break;

		case CHANNEL_WIDTH_40:
		case CHANNEL_WIDTH_80:
			if (pri_ch > central_ch)
				pri_ch_idx = (pri_ch - central_ch) >> 1;
			else
				pri_ch_idx = ((central_ch - pri_ch) >> 1) + 1;
			break;

		default:
			break;
		}
		cck_en = false;
		is_2g_ch = false;
	}
	#if 0
	if (!bb->hal_com->dbcc_en) {
		/*============== [Path A] ==============*/
		path0_rf18 = halbb_read_rf_reg_8852a_2(bb, RF_PATH_A, 0x18, RFREGOFFSETMASK);
		/*============== [Path B] ==============*/
		path1_rf18 = halbb_read_rf_reg_8852a_2(bb, RF_PATH_B, 0x18, RFREGOFFSETMASK);
	} else {
		if (phy_idx == HW_PHY_0) {
			/*============== [Path A] ==============*/
			path0_rf18 = halbb_read_rf_reg_8852a_2(bb, RF_PATH_A, 0x18, RFREGOFFSETMASK);
		}
		else {
			/*============== [Path B] ==============*/
			path1_rf18 = halbb_read_rf_reg_8852a_2(bb, RF_PATH_B, 0x18, RFREGOFFSETMASK);
		}
	}
	#else
	/* ignore rf read flow and use rf io offload with bit 0~11, 16~17 mask for RF write */
	path0_rf18 = 0;
	path1_rf18 = 0;
	#endif
	/*BB_WARNING("RF a/b = %x , %x", path0_rf18, path1_rf18);*/
	/*==== [Switch BW] ====*/
	rpt &= halbb_fwofld_bw_8852a_2(bb, pri_ch_idx, bw, phy_idx, &path0_rf18, &path1_rf18);
	/*BB_WARNING("SwBW : RF a/b = %x , %x", path0_rf18, path1_rf18);*/
	/*==== [Switch CH] ====*/
	rpt &= halbb_fwofld_ch_8852a_2(bb, central_ch, phy_idx, &path0_rf18, &path1_rf18);
	/*BB_WARNING("SwCH : RF a/b = %x , %x", path0_rf18, path1_rf18);*/
	#if 0
	if (!bb->hal_com->dbcc_en) {
		/*============== [Path A] ==============*/
		rpt &= halbb_fw_set_rf_reg_8852a_2(bb, RF_PATH_A, 0x18, RFREGOFFSETMASK, path0_rf18, 0);
		/*============== [Path B] ==============*/
		rpt &= halbb_fw_set_rf_reg_8852a_2(bb, RF_PATH_B, 0x18, RFREGOFFSETMASK, path1_rf18, 0);
	} else {
		if (phy_idx == HW_PHY_0) {
			/*============== [Path A] ==============*/
			rpt &= halbb_fw_set_rf_reg_8852a_2(bb, RF_PATH_A, 0x18, RFREGOFFSETMASK, path0_rf18, 0);
		} else {
			/*============== [Path B] ==============*/
			rpt &= halbb_fw_set_rf_reg_8852a_2(bb, RF_PATH_B, 0x18, RFREGOFFSETMASK, path1_rf18, 0);
		}
	}
	#else
	if (!bb->hal_com->dbcc_en) {
		/*============== [Path A] ==============*/
		rpt &= halbb_fw_ofld_rf_reg_8852a_2(bb, RF_PATH_A, 0x18, RF18REGMASK, path0_rf18, 0);
		/*============== [Path B] ==============*/
		rpt &= halbb_fw_ofld_rf_reg_8852a_2(bb, RF_PATH_B, 0x18, RF18REGMASK, path1_rf18, 0);
	} else {
		if (phy_idx == HW_PHY_0) {
			/*============== [Path A] ==============*/
			rpt &= halbb_fw_ofld_rf_reg_8852a_2(bb, RF_PATH_A, 0x18, RF18REGMASK, path0_rf18, 0);
		} else {
			/*============== [Path B] ==============*/
			rpt &= halbb_fw_ofld_rf_reg_8852a_2(bb, RF_PATH_B, 0x18, RF18REGMASK, path1_rf18, 0);
		}
	}
	#endif
	/*==== [CCK Enable / Disable] ====*/
	rpt &= halbb_fwofld_cck_en_8852a_2(bb, cck_en, phy_idx);
	/*==== [Spur elimination] ====*/
	if (central_ch == 153) {
		rpt &= halbb_fw_set_reg(bb, 0x469c, 0xfff, 0x210, 0);
		rpt &= halbb_fw_set_reg(bb, 0x4770, 0xfff, 0x210, 0);
		rpt &= halbb_fw_set_reg(bb, 0x42ac, 0xfff, 0x7c0, 0);
		rpt &= halbb_fw_set_reg(bb, 0x469c, BIT(12), 0x1, 0);
		rpt &= halbb_fw_set_reg(bb, 0x4770, BIT(12), 0x1, 0);
		rpt &= halbb_fw_set_reg(bb, 0x42c4, BIT(23), 0x1, 0);
	} else if (central_ch == 151) {
		rpt &= halbb_fw_set_reg(bb, 0x469c, 0xfff, 0x210, 0);
		rpt &= halbb_fw_set_reg(bb, 0x4770, 0xfff, 0x210, 0);
		rpt &= halbb_fw_set_reg(bb, 0x42ac, 0xfff, 0x40, 0);
		rpt &= halbb_fw_set_reg(bb, 0x469c, BIT(12), 0x1, 0);
		rpt &= halbb_fw_set_reg(bb, 0x4770, BIT(12), 0x1, 0);
		rpt &= halbb_fw_set_reg(bb, 0x42c4, BIT(23), 0x1, 0);
	} else if (central_ch == 155) {
		rpt &= halbb_fw_set_reg(bb, 0x469c, 0xfff, 0x2d0, 0);
		rpt &= halbb_fw_set_reg(bb, 0x4770, 0xfff, 0x2d0, 0);
		rpt &= halbb_fw_set_reg(bb, 0x42ac, 0xfff, 0x740, 0);
		rpt &= halbb_fw_set_reg(bb, 0x469c, BIT(12), 0x1, 0);
		rpt &= halbb_fw_set_reg(bb, 0x4770, BIT(12), 0x1, 0);
		rpt &= halbb_fw_set_reg(bb, 0x42c4, BIT(23), 0x1, 0);
	} else {
		rpt &= halbb_fw_set_reg(bb, 0x469c, BIT(12), 0x0, 0);
		rpt &= halbb_fw_set_reg(bb, 0x4770, BIT(12), 0x0, 0);
		rpt &= halbb_fw_set_reg(bb, 0x42c4, BIT(23), 0x0, 0);
	}

	BB_DBG(bb, DBG_DBCC, "[%s] bb->rx_path=%d , phy_idx=%d, dbcc_en=%d\n", __func__, bb->rx_path, phy_idx, bb->hal_com->dbcc_en);
	if (is_2g_ch && ((bb->rx_path == RF_PATH_B) || (bb->rx_path == RF_PATH_AB)))
		rpt &=halbb_fwofld_btg_8852a_2(bb, true);
	else
		rpt &=halbb_fwofld_btg_8852a_2(bb, false);

	/* Dynamic 5M Mask Setting */
	rpt &=halbb_fw_5m_mask_8852a_2(bb, pri_ch, bw);

	/*==== [TSSI reset] ====*/
	if (!bb->hal_com->dbcc_en) {
		// Path A
		rpt &= halbb_fw_set_reg(bb, 0x58dc, BIT(31) | BIT(30), 0x1, 0);
		rpt &= halbb_fw_set_reg(bb, 0x58dc, BIT(31) | BIT(30), 0x3, 0);
		// Path B
		rpt &= halbb_fw_set_reg(bb, 0x78dc, BIT(31) | BIT(30), 0x1, 0);
		rpt &= halbb_fw_set_reg(bb, 0x78dc, BIT(31) | BIT(30), 0x3, 0);
	} else {
		if (phy_idx == HW_PHY_0) {
			// Path A
			rpt &= halbb_fw_set_reg(bb, 0x58dc, BIT(31) | BIT(30), 0x1, 0);
			rpt &= halbb_fw_set_reg(bb, 0x58dc, BIT(31) | BIT(30), 0x3, 0);
		} else {
			// Path B
			rpt &= halbb_fw_set_reg(bb, 0x78dc, BIT(31) | BIT(30), 0x1, 0);
			rpt &= halbb_fw_set_reg(bb, 0x78dc, BIT(31) | BIT(30), 0x3, 0);
		}
	}

	return rpt;
}

void halbb_fwofld_stop_pmac_tx_8852a_2(struct bb_info *bb,
			      struct halbb_pmac_info *tx_info,
			      enum phl_phy_idx phy_idx)
{
	if (tx_info->is_cck) { // CCK
		if (tx_info->mode == CONT_TX) {
			halbb_fw_set_reg(bb, 0x2300, BIT(26), 1, 0);
			halbb_fw_set_reg(bb, 0x2338, BIT(17), 0, 0);
			halbb_fw_set_reg(bb, 0x2300, BIT(28), 0, 0);
			halbb_fw_set_reg(bb, 0x2300, BIT(26), 0, 0);
		} else if (tx_info->mode == PKTS_TX) {
			halbb_fw_set_reg_cmn(bb, 0x9c4, BIT(4), 0, phy_idx, 0);
		} else if (tx_info->mode == CCK_CARRIER_SIPPRESSION_TX) {
			halbb_fw_set_reg_cmn(bb, 0x9c4, BIT(4), 0, phy_idx, 0);
			/*Carrier Suppress Tx*/
			halbb_fw_set_reg(bb, 0x2338, BIT(18), 0, 0);
			/*Enable scrambler at payload part*/
			halbb_fw_set_reg(bb, 0x2304, BIT(26), 0, 0);
		}
	} else { // OFDM
		if (tx_info->mode == CONT_TX)
			halbb_fw_set_reg_cmn(bb, 0x9c4, BIT(0), 0, phy_idx, 0);
		else if (tx_info->mode == PKTS_TX)
			halbb_fw_set_reg_cmn(bb, 0x9c4, BIT(4), 0, phy_idx, 0);
	}
}

void halbb_fwofld_start_pmac_tx_8852a_2(struct bb_info *bb,
			       struct halbb_pmac_info *tx_info,
			       enum halbb_pmac_mode mode, u32 pkt_cnt,u16 period,
			       enum phl_phy_idx phy_idx)
{
	if (mode == CONT_TX) {
		if (tx_info->is_cck) {
			halbb_fw_set_reg(bb, 0x2338, BIT(17), 1, 0);
			halbb_fw_set_reg(bb, 0x2300, BIT(28), 0, 0);
		} else {
			halbb_fw_set_reg_cmn(bb, 0x9c4, BIT(0), 1, phy_idx, 0);
		}
	} else if (mode == PKTS_TX) {
		/*Tx_N_PACKET_EN */
		halbb_fw_set_reg_cmn(bb, 0x9c4, BIT(4), 1, phy_idx, 0);
		/*Tx_N_PERIOD */
		halbb_fw_set_reg_cmn(bb, 0x9c4, 0xffffff00, period, phy_idx, 0);
		/*Tx_N_PACKET */
		halbb_fw_set_reg_cmn(bb, 0x9c8, 0xffffffff, pkt_cnt, phy_idx, 0);
	} else if (mode == CCK_CARRIER_SIPPRESSION_TX) {
		if (tx_info->is_cck) {
			/*Carrier Suppress Tx*/
			halbb_fw_set_reg(bb, 0x2338, BIT(18), 1, 0);
			/*Disable scrambler at payload part*/
			halbb_fw_set_reg(bb, 0x2304, BIT(26), 1, 0);
		} else {
			return;
		}
		/*Tx_N_PACKET_EN */
		halbb_fw_set_reg_cmn(bb, 0x9c4, BIT(4), 1, phy_idx, 0);
		/*Tx_N_PERIOD */
		halbb_fw_set_reg_cmn(bb, 0x9c4, 0xffffff00, period, phy_idx, 0);
		/*Tx_N_PACKET */
		halbb_fw_set_reg_cmn(bb, 0x9c8, 0xffffffff, pkt_cnt, phy_idx, 0);
	}
	/*Tx_EN */
	halbb_fw_set_reg_cmn(bb, 0x9c0, BIT(0), 1, phy_idx, 0);
	halbb_fw_set_reg_cmn(bb, 0x9c0, BIT(0), 0, phy_idx, 1);
}

void halbb_fwofld_set_pmac_tx_8852a_2(struct bb_info *bb, struct halbb_pmac_info *tx_info,
			     enum phl_phy_idx phy_idx)
{
	BB_DBG(bb, DBG_PHY_CONFIG, "<====== %s ======>\n", __func__);

	if (!tx_info->en_pmac_tx) {
		halbb_fwofld_stop_pmac_tx_8852a_2(bb, tx_info, phy_idx);
		/* PD hit enable */
		halbb_fw_set_reg_cmn(bb, 0xc3c, BIT(9), 0, phy_idx, 0);
		if (bb->hal_com->band[phy_idx].cur_chandef.band == BAND_ON_24G)
			halbb_fw_set_reg(bb, 0x2344, BIT(31), 0, 1);
		return;
	}
	/*Turn on PMAC */
	/* Tx */
	halbb_fw_set_reg_cmn(bb, 0x0980, BIT(0), 1, phy_idx, 0);
	/* Rx */
	halbb_fw_set_reg_cmn(bb, 0x0980, BIT(16), 1, phy_idx, 0);
	halbb_fw_set_reg_cmn(bb, 0x0988, 0x3f, 0x3f, phy_idx, 0);

	/* PD hit enable */
	halbb_fw_set_reg_cmn(bb, 0x704, BIT(1), 0, phy_idx, 0);
	halbb_fw_set_reg_cmn(bb, 0xc3c, BIT(9), 1, phy_idx, 0);
	halbb_fw_set_reg(bb, 0x2344, BIT(31), 1, 0);
	halbb_fw_set_reg_cmn(bb, 0x704, BIT(1), 1, phy_idx, 0);

	halbb_fwofld_start_pmac_tx_8852a_2(bb, tx_info, tx_info->mode, tx_info->tx_cnt,
		       tx_info->period, phy_idx);
}
#endif
void halbb_fw_ofld_dbg(struct bb_info *bb, char input[][16], u32 *_used,
		       char *output, u32 *_out_len)
{
	u32 val[10] = {0};
	u8 i, cmd_size;

	if (_os_strcmp(input[1], "-h") == 0) {
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "show\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "bitmap {val(hex)}\n");
		return;
	}


	if (_os_strcmp(input[1], "show") == 0) {

		cmd_size = sizeof(halbb_fw_ofld_info_i) / sizeof(struct fw_cmd_info);
		
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			 "BB FW OFLD cmd ==>\n");

		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "FW_OFLD_bitmap=0x%x\n",
			    bb->bb_cmn_hooker->bb_fwofld_sup_bitmap);

		for (i = 0; i < cmd_size; i++)
			BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
				    "  %-5d: (%s) BIT(%02d) %s \n", i,
				    ((BIT(halbb_fw_ofld_info_i[i].id) & bb->bb_cmn_hooker->bb_fwofld_sup_bitmap) ? "V" : "."),
				    halbb_fw_ofld_info_i[i].id, halbb_fw_ofld_info_i[i].name);

	}

	if (_os_strcmp(input[1], "bitmap") == 0) {
		HALBB_SCAN(input[2], DCMD_HEX, &val[0]);
		bb->bb_cmn_hooker->bb_fwofld_sup_bitmap = val[0];
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "FW_OFLD_bitmap=0x%x\n",
			    bb->bb_cmn_hooker->bb_fwofld_sup_bitmap);
	}
}
#endif

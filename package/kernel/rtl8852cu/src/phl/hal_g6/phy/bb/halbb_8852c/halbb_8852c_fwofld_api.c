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
#ifdef HALBB_FW_OFLD_SUPPORT
bool halbb_fwcfg_bb_phy_8852c(struct bb_info *bb, u32 addr, u32 data,
			    enum phl_phy_idx phy_idx)
{
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

void halbb_fwofld_edcca_per20_bitmap_sifs_8852c(struct bb_info *bb,
						enum channel_width bw,
						enum phl_phy_idx phy_idx)
{
	if (bw == CHANNEL_WIDTH_20) {
		halbb_fw_set_reg_cmn(bb, 0x0c9c, 0xFF000, 0xff, phy_idx, 0);
		halbb_fw_set_reg_cmn(bb, 0x0cA0, 0xFF000, 0, phy_idx, 0);
	} else {
		halbb_fw_set_reg_cmn(bb, 0x0c9c, 0xFF000, 0, phy_idx, 0);
		halbb_fw_set_reg_cmn(bb, 0x0cA0, 0xFF000, 0, phy_idx, 0);
	}
}

void halbb_fwofld_bb_reset_8852c(struct bb_info *bb, enum phl_phy_idx phy_idx)
{
	BB_DBG(bb, DBG_DBG_API, "%s\n", __func__);

	//Protest HW-SI
	halbb_fw_set_reg_cmn(bb, 0x1200, BIT(28) | BIT(29) | BIT(30), 0x7, phy_idx, 0);
	halbb_fw_set_reg_cmn(bb, 0x3200, BIT(28) | BIT(29) | BIT(30), 0x7, phy_idx, 0);
	halbb_delay_us(bb, 1);
	// === [BB reset] === //
	halbb_fw_set_reg_cmn(bb, 0x704, BIT(1), 1, phy_idx, 0);
	halbb_fw_set_reg_cmn(bb, 0x704, BIT(1), 0, phy_idx, 0);

	halbb_fw_set_reg_cmn(bb, 0x1200, BIT(28) | BIT(29) | BIT(30), 0x0, phy_idx, 0);
	halbb_fw_set_reg_cmn(bb, 0x3200, BIT(28) | BIT(29) | BIT(30), 0x0, phy_idx, 0);
	halbb_fw_set_reg_cmn(bb, 0x704, BIT(1), 1, phy_idx, 0);
}

void halbb_fwofld_dfs_en_8852c(struct bb_info *bb, bool en)
{
	BB_DBG(bb, DBG_DBG_API, "%s\n", __func__);

	if (en)
		halbb_fw_set_reg(bb, 0x0, BIT(31), 1, 0);
	else
		halbb_fw_set_reg(bb, 0x0, BIT(31), 0, 0);
}

void halbb_fwofld_adc_en_8852c(struct bb_info *bb, bool en)
{
	BB_DBG(bb, DBG_DBG_API, "%s\n", __func__);

	if (en)
		halbb_fw_set_reg(bb, 0x20fc, 0xff000000, 0x0, 0);
	else
		halbb_fw_set_reg(bb, 0x20fc, 0xff000000, 0xf, 0);
}

void halbb_fwofld_tssi_cont_en_8852c(struct bb_info *bb, bool en, enum rf_path path)
{
	u32 tssi_trk_man[2] = {0x5818, 0x7818};

	BB_DBG(bb, DBG_DBG_API, "%s\n", __func__);

	if (en) {
		halbb_fw_set_reg(bb, tssi_trk_man[path], BIT(30), 0x0, 0);
		if (bb->hal_com->dbcc_en && (path == RF_PATH_B))
			rtw_hal_rf_tssi_scan_ch(bb->hal_com, HW_PHY_1, path);
		else
			rtw_hal_rf_tssi_scan_ch(bb->hal_com, HW_PHY_0, path);
	} else {
		halbb_fw_set_reg(bb, tssi_trk_man[path], BIT(30), 0x1, 0);
	}
}
void halbb_fwofld_bb_reset_all_8852c(struct bb_info *bb, enum phl_phy_idx phy_idx)
{
	BB_DBG(bb, DBG_DBG_API, "%s\n", __func__);

	//Protest SW-SI 
	halbb_fw_set_reg_cmn(bb, 0x1200, BIT(28) | BIT(29) | BIT(30), 0x7, phy_idx, 0);
	halbb_fw_set_reg_cmn(bb, 0x3200, BIT(28) | BIT(29) | BIT(30), 0x7, phy_idx, 0);
	halbb_fw_delay(bb, 1);
	// === [BB reset] === //
	halbb_fw_set_reg_cmn(bb, 0x704, BIT(1), 1, phy_idx, 0);
	halbb_fw_set_reg_cmn(bb, 0x704, BIT(1), 0, phy_idx, 0);
	
	halbb_fw_set_reg_cmn(bb, 0x1200, BIT(28) | BIT(29) | BIT(30), 0x0, phy_idx, 0);
	halbb_fw_set_reg_cmn(bb, 0x3200, BIT(28) | BIT(29) | BIT(30), 0x0, phy_idx, 0);
	halbb_fw_set_reg_cmn(bb, 0x704, BIT(1), 1, phy_idx, 0);
}

void halbb_fwofld_bb_reset_en_8852c(struct bb_info *bb, bool en, enum phl_phy_idx phy_idx)
{
	BB_DBG(bb, DBG_DBG_API, "%s\n", __func__);

	if (en) {
		halbb_fw_set_reg_cmn(bb, 0x1200, BIT(28) | BIT(29) | BIT(30), 0x0, phy_idx, 0);
		halbb_fw_set_reg_cmn(bb, 0x3200, BIT(28) | BIT(29) | BIT(30), 0x0, phy_idx, 0);
		halbb_fw_set_reg_cmn(bb, 0x704, BIT(1), 1, phy_idx, 0);
		//PD Enable
		if(bb->hal_com->band[0].cur_chandef.band == BAND_ON_24G)
			halbb_fw_set_reg(bb,0x2320, BIT(0), 0x0, 0);
		halbb_fw_set_reg(bb,0xc3c, BIT(9), 0x0, 0);
	} else {
		//PD Disable
		halbb_fw_set_reg(bb,0x2320, BIT(0), 0x1, 0);
		halbb_fw_set_reg(bb,0xc3c, BIT(9), 0x1, 0);
		//Protest SW-SI 
		halbb_fw_set_reg_cmn(bb, 0x1200, BIT(28) | BIT(29) | BIT(30), 0x7, phy_idx, 0);
		halbb_fw_set_reg_cmn(bb, 0x3200, BIT(28) | BIT(29) | BIT(30), 0x7, phy_idx, 0);
		halbb_fw_delay(bb, 1);
		halbb_fw_set_reg_cmn(bb, 0x704, BIT(1), 0, phy_idx, 0);
	}
}
#if 0
u32 halbb_read_rf_reg_8852c_a(struct bb_info *bb, enum rf_path path,
			      u32 reg_addr, u32 bit_mask)
{
	u8 path_tmp=0;
	u32 i = 0, j = 0, readback_value = INVALID_RF_DATA, r_reg = 0;
	bool is_r_busy = true, is_w_busy = true, is_r_done = false;

	BB_DBG(bb, DBG_PHY_CONFIG, "<====== %s ======>\n", __func__);

	/*==== Error handling ====*/
	while (is_w_busy || is_r_busy) {
		is_w_busy = (bool)halbb_get_reg(bb, 0x174c, BIT(24));
		is_r_busy = (bool)halbb_get_reg(bb, 0x174c, BIT(25));
		halbb_delay_us(bb, 1);
		/*BB_WARNING("[%s] is_w_busy = %d, is_r_busy = %d\n",
				__func__, is_w_busy, is_r_busy);*/
		i++;
		if (i > 30)
			break;
	}
	if (is_w_busy || is_r_busy) {
		BB_WARNING("[%s] is_w_busy = (%d), is_r_busy = (%d)\n",
			   __func__, is_w_busy, is_r_busy);
		return INVALID_RF_DATA;
	}

	
	if (path > RF_PATH_B) {
		BB_WARNING("[%s] Unsupported path (%d)\n", __func__, path);
		return INVALID_RF_DATA;
	}

	/*==== Calculate offset ====*/
	path_tmp = (u8)path & 0x7;
	reg_addr &= 0xff;

	/*==== RF register only has 20bits ====*/
	bit_mask &= RFREGOFFSETMASK;

	r_reg = (path_tmp << 8 | reg_addr) & 0x7ff;
	halbb_set_reg(bb, 0x378, 0x7ff, r_reg);
	halbb_delay_us(bb, 2);

	
	/*==== Read RF register ====*/
	while (!is_r_done) {
		is_r_done = (bool)halbb_get_reg(bb, 0x174c, BIT(26));
		halbb_delay_us(bb, 1);
		j++;
		if (j > 30)
			break;
	}
	if (is_r_done) {
		readback_value = halbb_get_reg(bb, 0x174c, bit_mask);
	} else {
		BB_WARNING("[%s] is_r_done = (%d)\n", __func__, is_r_done);
		return INVALID_RF_DATA;
	}
	BB_DBG(bb, DBG_PHY_CONFIG, "A die RF-%d 0x%x = 0x%x, bit mask = 0x%x, i=%x, j =%x\n",
	       path_tmp, reg_addr, readback_value, bit_mask,i,j);
	return readback_value;
}

u32 halbb_read_rf_reg_8852c_d(struct bb_info *bb, enum rf_path path,
			      u32 reg_addr, u32 bit_mask)
{
	u32 readback_value = 0, direct_addr = 0;
	u32 offset_read_rf[2] = {0xe000, 0xf000};

	BB_DBG(bb, DBG_PHY_CONFIG, "<====== %s ======>\n", __func__);

	/*==== Error handling ====*/
	if (path > RF_PATH_B) {
		BB_WARNING("[%s] Unsupported path (%d)\n", __func__, path);
		return INVALID_RF_DATA;
	}

	/*==== Calculate offset ====*/
	reg_addr &= 0xff;
	direct_addr = offset_read_rf[path] + (reg_addr << 2);

	/*==== RF register only has 20bits ====*/
	bit_mask &= RFREGOFFSETMASK;

	/*==== Read RF register directly ====*/
	readback_value = halbb_get_reg(bb, direct_addr, bit_mask);
	BB_DBG(bb, DBG_PHY_CONFIG, "D die RF-%d 0x100%x = 0x%x, bit mask = 0x%x\n",
	       path, reg_addr, readback_value, bit_mask);
	return readback_value;
}

u32 halbb_read_rf_reg_8852c(struct bb_info *bb, enum rf_path path, u32 reg_addr,
			    u32 bit_mask)
{
	u32 readback_value = INVALID_RF_DATA;
	enum rtw_dv_sel ad_sel = (enum rtw_dv_sel)((reg_addr & 0x10000) >> 16);

	BB_DBG(bb, DBG_PHY_CONFIG, "<====== %s ======>\n", __func__);

	/*==== Error handling ====*/
	if (path > RF_PATH_B) {
		BB_WARNING("[%s] Unsupported path (%d)\n", __func__, path);
		return INVALID_RF_DATA;
	}

	if (ad_sel == DAV) {
		readback_value = halbb_read_rf_reg_8852c_a(bb, path, reg_addr,
				 bit_mask);
		/*BB_DBG(bb, DBG_PHY_CONFIG, "A-die RF-%d 0x%x = 0x%x, bit mask = 0x%x\n",
		       path, reg_addr, readback_value, bit_mask);*/
	} else if (ad_sel == DDV) {
		readback_value = halbb_read_rf_reg_8852c_d(bb, path, reg_addr,
				 bit_mask);
		/*BB_DBG(bb, DBG_PHY_CONFIG, "D-die RF-%d 0x%x = 0x%x, bit mask = 0x%x\n",
		       path, reg_addr, readback_value, bit_mask);*/
	} else {
		BB_DBG(bb, DBG_PHY_CONFIG, "Fail Read RF RF-%d 0x%x = 0x%x, bit mask = 0x%x\n",
		       path, reg_addr, readback_value, bit_mask);
		return INVALID_RF_DATA;
	}
	return readback_value;
}
bool halbb_write_rf_reg_8852c_a(struct bb_info *bb, enum rf_path path,
				u32 reg_addr, u32 bit_mask, u32 data)
{
	u8 path_tmp = 0, b_msk_en = 0, bit_shift = 0;
	u32 i =0, w_reg = 0;
	bool is_r_busy = true, is_w_busy = true;

	BB_DBG(bb, DBG_PHY_CONFIG, "<====== %s ======>\n", __func__);

	/*==== Error handling ====*/
	while (is_w_busy || is_r_busy) {
		is_w_busy = (bool)halbb_get_reg(bb, 0x174c, BIT(24));
		is_r_busy = (bool)halbb_get_reg(bb, 0x174c, BIT(25));
		halbb_delay_us(bb, 1);
		/*BB_WARNING("[%s] is_w_busy = %d, is_r_busy = %d\n",
				__func__, is_w_busy, is_r_busy);*/
		i++;
		if (i > 30)
			break;
	}
	if (is_w_busy || is_r_busy) {
		BB_WARNING("[%s] is_w_busy = (%d), is_r_busy = (%d)\n",
			   __func__, is_w_busy, is_r_busy);
		return false;
	}
	if (path > RF_PATH_B) {
		BB_WARNING("[%s] Unsupported path (%d)\n", __func__, path);
		return false;
	}

	/*==== Calculate offset ====*/
	path_tmp = (u8)path & 0x7;
	reg_addr &= 0xff;
	
	/*==== RF register only has 20bits ====*/
	data &= RFREGOFFSETMASK;
	bit_mask &= RFREGOFFSETMASK;

	/*==== Check if mask needed  ====*/
	if (bit_mask != RFREGOFFSETMASK) {
		b_msk_en = 1;
		halbb_set_reg(bb, 0x374, RFREGOFFSETMASK, bit_mask);
		for (bit_shift = 0; bit_shift <= 19; bit_shift++) {
			if ((bit_mask >> bit_shift) & 0x1)
				break;
		}
		data = (data << bit_shift) & RFREGOFFSETMASK;
	}

	w_reg = b_msk_en << 31 | path_tmp << 28 | reg_addr << 20 | data;

	/*==== Write RF register  ====*/
	halbb_set_reg(bb, 0x370, MASKDWORD, w_reg);
	//halbb_delay_us(bb, 5);

	BB_DBG(bb, DBG_PHY_CONFIG, "A die RF-%d 0x%x = 0x%x , bit mask = 0x%x, i=%x\n",
	       path_tmp, reg_addr, data, bit_mask,i);

	return true;
}

bool halbb_write_rf_reg_8852c_d(struct bb_info *bb, enum rf_path path,
				u32 reg_addr, u32 bit_mask, u32 data)
{
	u32 direct_addr = 0;
	u32 offset_write_rf[2] = {0xe000, 0xf000};

	BB_DBG(bb, DBG_PHY_CONFIG, "<====== %s ======>\n", __func__);

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

	/*==== Write RF register directly ====*/
	halbb_set_reg(bb, direct_addr, bit_mask, data);

	halbb_delay_us(bb, 1);

	BB_DBG(bb, DBG_PHY_CONFIG, "D die RF-%d 0x%x = 0x%x , bit mask = 0x%x\n",
	       path, reg_addr, data, bit_mask);

	return true;
}

bool halbb_write_rf_reg_8852c(struct bb_info *bb, enum rf_path path,
			      u32 reg_addr, u32 bit_mask, u32 data)
{
	u8 path_tmp = 0, b_msk_en = 0;
	u32 w_reg = 0;
	bool rpt = true;
	enum rtw_dv_sel ad_sel = (enum rtw_dv_sel)((reg_addr & 0x10000) >> 16);

	BB_DBG(bb, DBG_PHY_CONFIG, "<====== %s ======>\n", __func__);

	/*==== Error handling ====*/
	if (path > RF_PATH_B) {
		BB_WARNING("[%s] Unsupported path (%d)\n", __func__, path);
		return false;
	}

	if (ad_sel == DAV) {
		rpt = halbb_write_rf_reg_8852c_a(bb, path, reg_addr, bit_mask,
		      data);
		/*BB_DBG(bb, DBG_PHY_CONFIG, "A-die RF-%d 0x%x = 0x%x , bit mask = 0x%x\n",
		       path, reg_addr, data, bit_mask);*/
	} else if (ad_sel == DDV) {
		rpt = halbb_write_rf_reg_8852c_d(bb, path, reg_addr, bit_mask,
		      data);
		/*BB_DBG(bb, DBG_PHY_CONFIG, "D-die RF-%d 0x%x = 0x%x , bit mask = 0x%x\n",
		       path, reg_addr, data, bit_mask);*/
	} else {
		rpt = false;
		BB_DBG(bb, DBG_PHY_CONFIG, "Fail Write RF-%d 0x%x = 0x%x , bit mask = 0x%x\n",
		       path, reg_addr, data, bit_mask);
	}

	return rpt;
}
#endif
void halbb_fwofld_5m_mask_8852c(struct bb_info *bb, u8 pri_ch, enum channel_width bw,
			   enum phl_phy_idx phy_idx)
{
	bool mask_5m_low = false;
	bool mask_5m_en = false;

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
		halbb_fw_set_reg(bb, 0x4c4c, BIT(12), 0x0, 0);
		halbb_fw_set_reg(bb, 0x4d10, BIT(12), 0x0, 0);
		halbb_fw_set_reg_cmn(bb, 0x4450, BIT(24), 0x0, phy_idx, 0);
	} else {
		if (mask_5m_low) {
			halbb_fw_set_reg(bb, 0x4c4c, 0x3f, 0x4, 0);
			halbb_fw_set_reg(bb, 0x4c4c, BIT(12), 0x1, 0);
			halbb_fw_set_reg(bb, 0x4c4c, BIT(8), 0x0, 0);
			halbb_fw_set_reg(bb, 0x4c4c, BIT(6), 0x1, 0);
			halbb_fw_set_reg(bb, 0x4d10, 0x3f, 0x4, 0);
			halbb_fw_set_reg(bb, 0x4d10, BIT(12), 0x1, 0);
			halbb_fw_set_reg(bb, 0x4d10, BIT(8), 0x0, 0);
			halbb_fw_set_reg(bb, 0x4d10, BIT(6), 0x1, 0);
		} else {
			halbb_fw_set_reg(bb, 0x4c4c, 0x3f, 0x4, 0);
			halbb_fw_set_reg(bb, 0x4c4c, BIT(12), 0x1, 0);
			halbb_fw_set_reg(bb, 0x4c4c, BIT(8), 0x1, 0);
			halbb_fw_set_reg(bb, 0x4c4c, BIT(6), 0x0, 0);
			halbb_fw_set_reg(bb, 0x4d10, 0x3f, 0x4, 0);
			halbb_fw_set_reg(bb, 0x4d10, BIT(12), 0x1, 0);
			halbb_fw_set_reg(bb, 0x4d10, BIT(8), 0x1, 0);
			halbb_fw_set_reg(bb, 0x4d10, BIT(6), 0x0, 0);
		}
		halbb_fw_set_reg_cmn(bb, 0x4450, BIT(24), 0x1, phy_idx, 0);
	}
}

u16 halbb_fwofld_sco_mapping_8852c(struct bb_info *bb, enum band_type band,  u8 central_ch)
{
	u16 central_freq = 0;

	if (band == BAND_ON_6G) {
		central_freq = 5955 + (central_ch - 1) * 5;
	} else if (band == BAND_ON_5G) {
		central_freq = 5180 + (central_ch - 36) * 5;
	} else { // band == BAND_ON_2G
		if (central_ch == 14)
			central_freq = 2484;
		else
			central_freq = 2412 + (central_ch - 1) * 5;
	}

	return central_freq;
}

bool halbb_fwofld_ctrl_sco_cck_8852c(struct bb_info *bb, u8 pri_ch)
{
	u32 sco_barker_threshold[14] = {0x1fe4f, 0x1ff5e, 0x2006c, 0x2017b,
					0x2028a, 0x20399, 0x204a8, 0x205b6,
					0x206c5, 0x207d4, 0x208e3, 0x209f2,
					0x20b00, 0x20d8a};
	u32 sco_cck_threshold[14] = {0x2bdac, 0x2bf21, 0x2c095, 0x2c209,
				     0x2c37e, 0x2c4f2, 0x2c666, 0x2c7db,
				     0x2c94f, 0x2cac3, 0x2cc38, 0x2cdac,
				     0x2cf21, 0x2d29e};

	if (pri_ch > 14) {
		BB_DBG(bb, DBG_PHY_CONFIG, "[CCK SCO Fail]");
		return false;
	}

	halbb_fw_set_reg(bb, 0x4a1c, 0x7ffff, sco_barker_threshold[pri_ch - 1], 0);
	halbb_fw_set_reg(bb, 0x4a20, 0x7ffff, sco_cck_threshold[pri_ch - 1], 0);
	BB_DBG(bb, DBG_PHY_CONFIG, "[CCK SCO Success]");
	return true;
}

void halbb_fwofld_ctrl_btg_8852c(struct bb_info *bb, bool btg)
{
	struct rtw_phl_com_t *phl = bb->phl_com;
	struct dev_cap_t *dev = &phl->dev_cap;
	bool s1_2g_band = false;

	if (dev->rfe_type >= 50)
		return;

	BB_DBG(bb, DBG_PHY_CONFIG, "<====== %s ======>\n", __func__);

	if (btg) {
		// Path A
		halbb_fw_set_reg(bb, 0x4738, BIT(19), 0x1, 0);
		halbb_fw_set_reg(bb, 0x4738, BIT(22), 0x0, 0);
		// Path B
		halbb_fw_set_reg(bb, 0x476c, 0xFF000000, 0x20, 0);
		halbb_fw_set_reg(bb, 0x4778, 0xFF, 0x30, 0);
		halbb_fw_set_reg(bb, 0x4aa4, BIT(19), 0x1, 0);
		halbb_fw_set_reg(bb, 0x4aa4, BIT(22), 0x1, 0);
		BB_DBG(bb, DBG_PHY_CONFIG, "[BT] Apply BTG Setting\n");
		// Apply Grant BT by TMAC Setting
		halbb_fw_set_reg(bb, 0x980, 0x1e0000, 0x0, 0);
		BB_DBG(bb, DBG_PHY_CONFIG, "[BT] Apply Grant BT by TMAC Setting\n");
		// Add BT share
		halbb_fw_set_reg(bb, 0x4978, BIT(14), 0x1, 0);
		halbb_fw_set_reg(bb, 0x4974, 0x3c00000, 0x2, 0);
		/* To avoid abnormal 1R CCA without BT, set rtl only 0xc6c[21] = 0x1 */
		halbb_fw_set_reg(bb, 0x441c, BIT(31), 0x1, 0);
		halbb_fw_set_reg(bb, 0xc6c, BIT(21), 0x1, 0);
	} else {
		// Path A
		halbb_fw_set_reg(bb, 0x4738, BIT(19), 0x0, 0);
		halbb_fw_set_reg(bb, 0x4738, BIT(22), 0x0, 0);
		// Path B
		halbb_fw_set_reg(bb, 0x476c, 0xFF000000, 0x1a, 0);
		halbb_fw_set_reg(bb, 0x4778, 0xFF, 0x2a, 0);
		halbb_fw_set_reg(bb, 0x4aa4, BIT(19), 0x0, 0);
		halbb_fw_set_reg(bb, 0x4aa4, BIT(22), 0x0, 0);
		BB_DBG(bb, DBG_PHY_CONFIG, "[BT] Disable BTG Setting\n");
		// Ignore Grant BT by PMAC Setting
		halbb_fw_set_reg(bb, 0x980, 0x1e0000, 0xf, 0);
		halbb_fw_set_reg(bb, 0x980, 0x3c000000, 0x4, 0);
		BB_DBG(bb, DBG_PHY_CONFIG, "[BT] Ignore Grant BT by PMAC Setting\n");
		// Reset BT share
		halbb_fw_set_reg(bb, 0x4978, BIT(14), 0x0, 0);
		halbb_fw_set_reg(bb, 0x4974, 0x3c00000, 0x0, 0);
		/* To avoid abnormal 1R CCA without BT, set rtl only 0xc6c[21] = 0x1 */
		halbb_fw_set_reg(bb, 0x441c, BIT(31), 0x0, 0);
		halbb_fw_set_reg(bb, 0xc6c, BIT(21), 0x0, 0);
	}
}

void halbb_fwofld_ctrl_btc_preagc_8852c(struct bb_info *bb, bool bt_en)
{
	BB_DBG(bb, DBG_PHY_CONFIG, "<====== %s ======>\n", __func__);

	if (bt_en) {
		// DFIR Corner
		halbb_fw_set_reg(bb, 0x4c00, BIT(1) | BIT(0), 0x3, 0);
		halbb_fw_set_reg(bb, 0x4cc4, BIT(1) | BIT(0), 0x3, 0);

		// BT trakcing always on
		halbb_fw_set_reg(bb, 0x4ad4, MASKDWORD, 0xf, 0);
		halbb_fw_set_reg(bb, 0x4ae0, MASKDWORD, 0xf, 0);

		// LNA6_OP1dB
		halbb_fw_set_reg(bb, 0x4688, MASKBYTE3, 0x80, 0);
		halbb_fw_set_reg(bb, 0x476C, MASKBYTE3, 0x80, 0);

		// LNA6_TIA0_1_OP1dB
		halbb_fw_set_reg(bb, 0x4694, MASKBYTE0, 0x80, 0);
		halbb_fw_set_reg(bb, 0x4694, MASKBYTE1, 0x80, 0);
		halbb_fw_set_reg(bb, 0x4778, MASKBYTE0, 0x80, 0);
		halbb_fw_set_reg(bb, 0x4778, MASKBYTE1, 0x80, 0);

		// LNA, TIA, ADC backoff at BT TX
		halbb_fw_set_reg(bb, 0x4ae4, 0xffffff, 0x780D1E, 0);
		halbb_fw_set_reg(bb, 0x4aec, 0xffffff, 0x780D1E, 0);

		// IBADC backoff
		halbb_fw_set_reg(bb, 0x469c, 0xfc000000, 0x34, 0);
		halbb_fw_set_reg(bb, 0x49f0, 0xfc000000, 0x34, 0);
	} else {
		// DFIR Corner
		halbb_fw_set_reg(bb, 0x4c00, BIT(1) | BIT(0), 0x0, 0);
		halbb_fw_set_reg(bb, 0x4cc4, BIT(1) | BIT(0), 0x0, 0);

		// BT trakcing always on
		halbb_fw_set_reg(bb, 0x4ad4, MASKDWORD, 0x60, 0);
		halbb_fw_set_reg(bb, 0x4ae0, MASKDWORD, 0x60, 0);

		// LNA6_OP1dB
		halbb_fw_set_reg(bb, 0x4688, MASKBYTE3, 0x1a, 0);
		halbb_fw_set_reg(bb, 0x476C, MASKBYTE3, 0x1a, 0);

		// LNA6_TIA0_1_OP1dB
		halbb_fw_set_reg(bb, 0x4694, MASKBYTE0, 0x2a, 0);
		halbb_fw_set_reg(bb, 0x4694, MASKBYTE1, 0x2a, 0);
		halbb_fw_set_reg(bb, 0x4778, MASKBYTE0, 0x2a, 0);
		halbb_fw_set_reg(bb, 0x4778, MASKBYTE1, 0x2a, 0);

		// LNA, TIA, ADC backoff at BT TX
		halbb_fw_set_reg(bb, 0x4ae4, 0xffffff, 0x79E99E, 0);
		halbb_fw_set_reg(bb, 0x4aec, 0xffffff, 0x79E99E, 0);

		// IBADC backoff
		halbb_fw_set_reg(bb, 0x469c, 0xfc000000, 0x26, 0);
		halbb_fw_set_reg(bb, 0x49f0, 0xfc000000, 0x26, 0);
	}
}
bool halbb_fwofld_bw_setting_8852c(struct bb_info *bb, enum channel_width bw,
			    enum rf_path path)
{
	u32 adc_sel[2] = {0xC0EC, 0xC1EC};
	u32 wbadc_sel[2] = {0xC0E4, 0xC1E4};
	u32 adc_op5_bw_sel[2] = {0xc0d8, 0xc1d8};

	/*==== [Switch bandwidth] ====*/
	switch (bw) {
	case CHANNEL_WIDTH_5:
	case CHANNEL_WIDTH_10:
		if (bw == CHANNEL_WIDTH_5) {
			/*ADC clock = 20M & WB ADC clock = 40M for BW5 */
			halbb_fw_set_reg(bb, adc_sel[path], 0x6000, 0x1, 0);
			halbb_fw_set_reg(bb, wbadc_sel[path], 0x30, 0x0, 0);
		} else if (bw == CHANNEL_WIDTH_10) {
			/*ADC clock = 40M & WB ADC clock = 80M for BW10 */
			halbb_fw_set_reg(bb, adc_sel[path], 0x6000, 0x2, 0);
			halbb_fw_set_reg(bb, wbadc_sel[path], 0x30, 0x1, 0);
		}

		break;
	case CHANNEL_WIDTH_20:/*ADC clock = 80M & WB ADC clock = 160M for BW20 */
	case CHANNEL_WIDTH_40:/*ADC clock = 80M & WB ADC clock = 160M for BW40 */
	case CHANNEL_WIDTH_80:/*ADC clock = 160M & WB ADC clock = 160M for BW80 */
	case CHANNEL_WIDTH_160:/*ADC clock = 160M & WB ADC clock = 160M for BW160 */
		halbb_fw_set_reg(bb, adc_sel[path], 0x6000, 0x0, 0);
		halbb_fw_set_reg(bb, wbadc_sel[path], 0x30, 0x2, 0);
		/*op5_current*/
		if ((bw == CHANNEL_WIDTH_80) || (bw == CHANNEL_WIDTH_160))
			halbb_fw_set_reg(bb, adc_op5_bw_sel[path], 0x1E0, 0x6, 0);
		else
			halbb_fw_set_reg(bb, adc_op5_bw_sel[path], 0x1E0, 0xF, 0);

		break;
	default:
		BB_WARNING("Fail to set ADC\n");
	}

	/*==== [Write RF register] ====*/
	// RF_18 R/W already move to RF API //
	BB_DBG(bb, DBG_PHY_CONFIG,
	       "[Success][bw_setting] ADC setting for Path-%d\n", path);
	return true;
}

bool halbb_fwofld_ctrl_bw_8852c(struct bb_info *bb, u8 pri_ch, enum channel_width bw,
			 enum phl_phy_idx phy_idx)
{
	BB_DBG(bb, DBG_PHY_CONFIG, "<====== %s ======>\n", __func__);

	if (bb->is_disable_phy_api) {
		BB_DBG(bb, DBG_PHY_CONFIG, "[%s] Disable PHY API\n", __func__);
		return true;
	}

	/*==== Error handling ====*/
	if (bw >= CHANNEL_WIDTH_MAX || (bw == CHANNEL_WIDTH_40 && pri_ch > 2) ||
	    (bw == CHANNEL_WIDTH_80 && pri_ch > 4) || (bw == CHANNEL_WIDTH_160
	    && pri_ch > 8)) {
		BB_WARNING("Fail to switch bw(bw:%d, pri ch:%d)\n", bw,
			   pri_ch);
		return false;
	}

	/*==== Switch bandwidth ====*/
	switch (bw) {
	case CHANNEL_WIDTH_5:
	case CHANNEL_WIDTH_10:
	case CHANNEL_WIDTH_20:
		if (bw == CHANNEL_WIDTH_5) {
			/*RF_BW:[31:30]=0x0 */
			halbb_fw_set_reg_cmn(bb, 0x4974, 0xC0000000, 0x0, phy_idx, 0);
			/*small BW:[13:12]=0x1 */
			halbb_fw_set_reg_cmn(bb, 0x4978, 0x3000, 0x1, phy_idx, 0);
			/*Pri ch:[11:8]=0x0 */
			halbb_fw_set_reg_cmn(bb, 0x4978, 0xf00, 0x0, phy_idx, 0);
			/*ACI Detect:[16]=0x0 */
			halbb_fw_set_reg_cmn(bb, 0x4738, 0x10000, 0x0, phy_idx, 0);
			halbb_fw_set_reg_cmn(bb, 0x4AA4, 0x10000, 0x0, phy_idx, 0);
		} else if (bw == CHANNEL_WIDTH_10) {
			/*RF_BW:[31:30]=0x0 */
			halbb_fw_set_reg_cmn(bb, 0x4974, 0xC0000000, 0x0, phy_idx, 0);
			/*small BW:[13:12]=0x2 */
			halbb_fw_set_reg_cmn(bb, 0x4978, 0x3000, 0x2, phy_idx, 0);
			/*Pri ch:[11:8]=0x0 */
			halbb_fw_set_reg_cmn(bb, 0x4978, 0xf00, 0x0, phy_idx, 0);
			/*ACI Detect:[16]=0x0 */
			halbb_fw_set_reg_cmn(bb, 0x4738, 0x10000, 0x0, phy_idx, 0);
			halbb_fw_set_reg_cmn(bb, 0x4AA4, 0x10000, 0x0, phy_idx, 0);
		} else if (bw == CHANNEL_WIDTH_20) {
			/*RF_BW:[31:30]=0x0 */
			halbb_fw_set_reg_cmn(bb, 0x4974, 0xC0000000, 0x0, phy_idx, 0);
			/*small BW:[13:12]=0x0 */
			halbb_fw_set_reg_cmn(bb, 0x4978, 0x3000, 0x0, phy_idx, 0);
			/*Pri ch:[11:8]=0x0 */
			halbb_fw_set_reg_cmn(bb, 0x4978, 0xf00, 0x0, phy_idx, 0);
			/*ACI Detect:[16]=0x1 */
			halbb_fw_set_reg_cmn(bb, 0x4738, 0x10000, 0x1, phy_idx, 0);
			halbb_fw_set_reg_cmn(bb, 0x4AA4, 0x10000, 0x1, phy_idx, 0);
		}
		// Set ADC
		halbb_fw_set_reg(bb, 0xC0D4, BIT(27) | BIT(26), 0x3, 0);
		halbb_fw_set_reg(bb, 0xC1D4, BIT(27) | BIT(26), 0x3, 0);
		halbb_fw_set_reg(bb, 0xC0D8, 0x1e0, 0xf, 0);
		halbb_fw_set_reg(bb, 0xC1D8, 0x1e0, 0xf, 0);

		break;
	case CHANNEL_WIDTH_40:
		/*RF_BW:[31:30]=0x1 */
		halbb_fw_set_reg_cmn(bb, 0x4974, 0xC0000000, 0x1, phy_idx, 0);
		/*small BW:[13:12]=0x0 */
		halbb_fw_set_reg_cmn(bb, 0x4978, 0x3000, 0x0, phy_idx, 0);
		/*Pri ch:[11:8] */
		halbb_fw_set_reg_cmn(bb, 0x4978, 0xf00, pri_ch, phy_idx, 0);
		// Set ADC
		halbb_fw_set_reg(bb, 0xC0D4, BIT(27) | BIT(26), 0x3, 0);
		halbb_fw_set_reg(bb, 0xC1D4, BIT(27) | BIT(26), 0x3, 0);
		halbb_fw_set_reg(bb, 0xC0D8, 0x1e0, 0xf, 0);
		halbb_fw_set_reg(bb, 0xC1D8, 0x1e0, 0xf, 0);

		break;
	case CHANNEL_WIDTH_80:
		/*RF_BW:[31:30]=0x2 */
		halbb_fw_set_reg_cmn(bb, 0x4974, 0xC0000000, 0x2, phy_idx, 0);
		/*small BW:[13:12]=0x0 */
		halbb_fw_set_reg_cmn(bb, 0x4978, 0x3000, 0x0, phy_idx, 0);
		/*Pri ch:[11:8] */
		halbb_fw_set_reg_cmn(bb, 0x4978, 0xf00, pri_ch, phy_idx, 0);
		// Set ADC
		halbb_fw_set_reg(bb, 0xC0D4, BIT(27) | BIT(26), 0x2, 0);
		halbb_fw_set_reg(bb, 0xC1D4, BIT(27) | BIT(26), 0x2, 0);
		halbb_fw_set_reg(bb, 0xC0D8, 0x1e0, 0xd, 0);
		halbb_fw_set_reg(bb, 0xC1D8, 0x1e0, 0xd, 0);

		break;
	case CHANNEL_WIDTH_160:
		/*RF_BW:[31:30]=0x2 */
		halbb_fw_set_reg_cmn(bb, 0x4974, 0xC0000000, 0x3, phy_idx, 0);
		/*small BW:[13:12]=0x0 */
		halbb_fw_set_reg_cmn(bb, 0x4978, 0x3000, 0x0, phy_idx, 0);
		/*Pri ch:[11:8] */
		halbb_fw_set_reg_cmn(bb, 0x4978, 0xf00, pri_ch, phy_idx, 0);
		// Set ADC
		halbb_fw_set_reg(bb, 0xC0D4, BIT(27) | BIT(26), 0x1, 0);
		halbb_fw_set_reg(bb, 0xC1D4, BIT(27) | BIT(26), 0x1, 0);
		halbb_fw_set_reg(bb, 0xC0D8, 0x1e0, 0xb, 0);
		halbb_fw_set_reg(bb, 0xC1D8, 0x1e0, 0xb, 0);

		break;
	default:
		BB_WARNING("Fail to switch bw (bw:%d, pri ch:%d)\n", bw,
			   pri_ch);
	}

	/*==== r_Rx_BW40_2xFFT_en setting ====*/
	if (bw == CHANNEL_WIDTH_40) {
		halbb_fw_set_reg_cmn(bb, 0x4e30, BIT(26), 1, phy_idx, 0);
		halbb_fw_set_reg_cmn(bb, 0x4424, BIT(2), 1, phy_idx, 0);
	} else {
		halbb_fw_set_reg_cmn(bb, 0x4e30, BIT(26), 0, phy_idx, 0);
		halbb_fw_set_reg_cmn(bb, 0x4424, BIT(2), 0, phy_idx, 0);
	}

	if (phy_idx == HW_PHY_0) {
		/*============== [Path A] ==============*/
		halbb_fwofld_bw_setting_8852c(bb, bw, RF_PATH_A);
		/*============== [Path B] ==============*/
		if (!bb->hal_com->dbcc_en)
			halbb_fwofld_bw_setting_8852c(bb, bw, RF_PATH_B);
	} else {
		/*============== [Path B] ==============*/
		halbb_fwofld_bw_setting_8852c(bb, bw, RF_PATH_B);
	}

	halbb_fwofld_edcca_per20_bitmap_sifs_8852c(bb, bw, phy_idx);

	BB_DBG(bb, DBG_PHY_CONFIG,
		  "[Switch BW Success] BW: %d for PHY%d\n", bw, phy_idx);

	return true;
}

bool halbb_fwofld_ctrl_ch_8852c(struct bb_info *bb, u8 central_ch, enum band_type band,
			 enum phl_phy_idx phy_idx)
{
	u8 sco;
	u16 central_freq;
	u8 ch_idx_encoded = 0;

	BB_DBG(bb, DBG_PHY_CONFIG, "<====== %s ======>\n", __func__);

	if (bb->is_disable_phy_api) {
		BB_DBG(bb, DBG_PHY_CONFIG, "[%s] Disable PHY API\n", __func__);
		return true;
	}
	/*==== Error handling ====*/
	if (band != BAND_ON_6G) {
		if ((central_ch > 14 && central_ch < 36) ||
			(central_ch > 64 && central_ch < 100) ||
			(central_ch > 144 && central_ch < 149) ||
			central_ch > 177 ||
			central_ch== 0) {
			BB_WARNING("Invalid CH:%d for PHY%d\n", central_ch,
				phy_idx);
			return false;
			}
	} else {
		if (central_ch > 253) {
			BB_WARNING("Invalid 6G CH:%d for PHY%d\n", central_ch,
				   phy_idx);
			return false;
		}
	}

	if (phy_idx == HW_PHY_0) {
		/*============== [Path A] ==============*/
		//------------- [Set Gain Error] ------------//
		halbb_fwofld_set_gain_error_8852c(bb, central_ch, band, RF_PATH_A);
		//------------- [Set efuse] ------------//
		halbb_fwofld_set_hidden_efuse_8852c(bb, central_ch, band, RF_PATH_A);
		halbb_fwofld_set_normal_efuse_8852c(bb, central_ch, band, RF_PATH_A);
		//------------- [Mode Sel - Path A] ------------//
		if (band == BAND_ON_24G)
			halbb_fw_set_reg_cmn(bb, 0x4738, BIT(17), 1, phy_idx, 0);
		else
			halbb_fw_set_reg_cmn(bb, 0x4738, BIT(17), 0, phy_idx, 0);

		/*============== [Path B] ==============*/
		if (!bb->hal_com->dbcc_en) {
			//------------- [Set Gain Error] ------------//
			halbb_fwofld_set_gain_error_8852c(bb, central_ch, band, RF_PATH_B);
			//------------- [Set efuse] ------------//
			halbb_fwofld_set_hidden_efuse_8852c(bb, central_ch, band, RF_PATH_B);
			halbb_fwofld_set_normal_efuse_8852c(bb, central_ch, band, RF_PATH_B);
			//------------- [Mode Sel - Path B] ------------//
			if (band == BAND_ON_24G)
				halbb_fw_set_reg_cmn(bb, 0x4aa4, BIT(17), 1, phy_idx, 0);
			else
				halbb_fw_set_reg_cmn(bb, 0x4aa4, BIT(17), 0, phy_idx, 0);
		}
		/*==== [SCO compensate fc setting] ====*/
		central_freq = halbb_fwofld_sco_mapping_8852c(bb, band, central_ch);
		halbb_fw_set_reg_cmn(bb, 0x4e30, 0x1fff, central_freq, phy_idx, 0);

		/*=== SCO compensate : (BIT(0) << 18) / central_ch ===*/
		sco = (u8)HALBB_DIV((BIT(0) << 18) + (central_freq / 2), central_freq);
		halbb_fw_set_reg_cmn(bb, 0x4974, 0x7f, sco, phy_idx, 0);
	} else {
		/*============== [Path B] ==============*/
		//------------- [Set Gain Error] ------------//
		halbb_fwofld_set_gain_error_8852c(bb, central_ch, band, RF_PATH_B);
		//------------- [Set efuse] ------------//
		halbb_fwofld_set_hidden_efuse_8852c(bb, central_ch, band, RF_PATH_B);
		halbb_fwofld_set_normal_efuse_8852c(bb, central_ch, band, RF_PATH_B);
		//------------- [Mode Sel - Path B] ------------//
		if (band == BAND_ON_24G)
			halbb_fw_set_reg_cmn(bb, 0x4aa4, BIT(17), 1, phy_idx, 0);
		else
			halbb_fw_set_reg_cmn(bb, 0x4aa4, BIT(17), 0, phy_idx, 0);

		/*==== [SCO compensate fc setting] ====*/
		central_freq = halbb_fwofld_sco_mapping_8852c(bb, band, central_ch);
		halbb_fw_set_reg_cmn(bb, 0x4e30, 0x1fff, central_freq, phy_idx, 0);

		/*=== SCO compensate : (BIT(0) << 18) / central_ch ===*/
		sco = (u8)HALBB_DIV((BIT(0) << 18) + (central_freq / 2), central_freq);
		halbb_fw_set_reg_cmn(bb, 0x4974, 0x7f, sco, phy_idx, 0);
	}

	/* === CCK Parameters === */
	if (band == BAND_ON_24G) {
		if (central_ch == 14) {
			halbb_fw_set_reg(bb, 0x45bc, 0xffffff, 0x3b13ff, 0);
			halbb_fw_set_reg(bb, 0x45cc, 0xffffff, 0x1c42de, 0);
			halbb_fw_set_reg(bb, 0x45d0, 0xffffff, 0xfdb0ad, 0);
			halbb_fw_set_reg(bb, 0x45d4, 0xffffff, 0xf60f6e, 0);
			halbb_fw_set_reg(bb, 0x45d8, 0xffffff, 0xfd8f92, 0);
			halbb_fw_set_reg(bb, 0x45c0, 0xffffff, 0x2d011, 0);
			halbb_fw_set_reg(bb, 0x45c4, 0xffffff, 0x1c02c, 0);
			halbb_fw_set_reg(bb, 0x45c8, 0xffffff, 0xfff00a, 0);
		} else {
			halbb_fw_set_reg(bb, 0x45bc, 0xffffff, 0x3d23ff, 0);
			halbb_fw_set_reg(bb, 0x45cc, 0xffffff, 0x29b354, 0);
			halbb_fw_set_reg(bb, 0x45d0, 0xffffff, 0xfc1c8, 0);
			halbb_fw_set_reg(bb, 0x45d4, 0xffffff, 0xfdb053, 0);
			halbb_fw_set_reg(bb, 0x45d8, 0xffffff, 0xf86f9a, 0);
			halbb_fw_set_reg(bb, 0x45c0, 0xffffff, 0xfaef92, 0);
			halbb_fw_set_reg(bb, 0x45c4, 0xffffff, 0xfe5fcc, 0);
			halbb_fw_set_reg(bb, 0x45c8, 0xffffff, 0xffdff5, 0);
		}
		/* === Set RXSC RPL Comp === */
		//halbb_fwofld_set_rxsc_rpl_comp_8852c(bb, central_ch);
	}

	/* === Set Ch idx report in phy-sts === */
	halbb_ch_idx_encode(bb, central_ch, band, &ch_idx_encoded);
	halbb_fw_set_reg_cmn(bb, 0x0734, 0x0ff0000, ch_idx_encoded, phy_idx, 0);

	BB_DBG(bb, DBG_PHY_CONFIG, "[Switch CH Success] CH: %d for PHY%d\n",
	       central_ch, phy_idx);
	return true;
}
void halbb_fwofld_ctrl_cck_blk_en_8852c(struct bb_info *bb, bool cck_blk_en,
				 enum phl_phy_idx phy_idx)
{
	if (cck_blk_en) {
		halbb_fw_set_reg_cmn(bb, 0x700, BIT(5), 1, phy_idx, 0);
		halbb_fw_set_reg_cmn(bb, 0xc80, BIT(31), 0, phy_idx, 0);
	} else {
		halbb_fw_set_reg_cmn(bb, 0x700, BIT(5), 0, phy_idx, 0);
		halbb_fw_set_reg_cmn(bb, 0xc80, BIT(31), 1, phy_idx, 0);
	}
}

void halbb_fwofld_ctrl_cck_en_8852c(struct bb_info *bb, bool cck_en,
			     enum phl_phy_idx phy_idx)
{
	struct bb_info *bb_phy0 = bb;
#ifdef HALBB_DBCC_SUPPORT
	struct bb_info *bb_phy1 = bb;

	HALBB_GET_PHY_PTR(bb, bb_phy0, HW_PHY_0);

	if (bb->hal_com->dbcc_en &&
	    bb->phl_com->dev_cap.dbcc_sup &&
	    (bb->bb_ch_i.is_2g || (bb->bb_phy_hooker && bb->bb_phy_hooker->bb_ch_i.is_2g))) {
		cck_en = true;
		BB_DBG(bb, DBG_DBCC, "Frc DBCC cck_en ===> 1\n");
	}
#endif
	halbb_fwofld_ctrl_cck_blk_en_8852c(bb, bb_phy0->bb_ch_i.is_2g ? 1 : 0, HW_PHY_0);
#ifdef HALBB_DBCC_SUPPORT
	if (bb->hal_com->dbcc_en) {
		HALBB_GET_PHY_PTR(bb, bb_phy1, HW_PHY_1);
		halbb_fwofld_ctrl_cck_blk_en_8852c(bb, bb_phy1->bb_ch_i.is_2g ? 1 : 0, HW_PHY_1);
	}

	bb->bb_cmn_hooker->cck_blk_en = cck_en;

#endif
	if (cck_en)
		halbb_fw_set_reg(bb, 0x2320, BIT(0), 0, 0);
	else
		halbb_fw_set_reg(bb, 0x2320, BIT(0), 1, 0);
	// if (cck_en) {
	// 	//halbb_set_reg(bb, 0x2300, BIT(27), 0);
	// 	halbb_fw_set_reg(bb, 0x700, BIT(5), 1, 0);
	// 	halbb_fw_set_reg(bb, 0x2320, BIT(0), 0, 0);
	// 	halbb_fw_set_reg_cmn(bb, 0xc80, BIT(31), 0, phy_idx, 0);
	// } else {
	// 	//halbb_set_reg(bb, 0x2300, BIT(27), 1);
	// 	halbb_fw_set_reg(bb, 0x700, BIT(5), 0, 0);
	// 	halbb_fw_set_reg(bb, 0x2320, BIT(0), 1, 0);
	// 	halbb_fw_set_reg_cmn(bb, 0xc80, BIT(31), 1, phy_idx, 0);
	// }
	// BB_DBG(bb, DBG_DBCC, "[%s]cck_en=%d PHY%d\n", __func__, cck_en, phy_idx);
}

bool halbb_fwofld_ctrl_bw_ch_8852c(struct bb_info *bb_0, u8 pri_ch, u8 central_ch,
			    enum channel_width bw, enum band_type band,
			    enum phl_phy_idx phy_idx)
{
	struct bb_info *bb = bb_0;
	bool rpt = true;
	bool cck_en = false;
	bool is_2g_ch;
	bool s1_2g_band = false;
	u8 pri_ch_idx = 0;

#ifdef HALBB_DBCC_SUPPORT
	HALBB_GET_PHY_PTR(bb_0, bb, phy_idx);
#endif
	is_2g_ch = (band == BAND_ON_24G) ? true : false;

	bb->bb_ch_i.is_2g = is_2g_ch;
	bb->bb_ch_i.fc_ch_idx = central_ch;

	/*==== [Set pri_ch idx] ====*/
	if (is_2g_ch) {
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
		rpt &= halbb_fwofld_ctrl_sco_cck_8852c(bb, pri_ch);

		cck_en = true;
	} else {
		// === 5G === //
		switch (bw) {
		case CHANNEL_WIDTH_20:
			break;

		case CHANNEL_WIDTH_40:
		case CHANNEL_WIDTH_80:
		case CHANNEL_WIDTH_160:
			if (pri_ch > central_ch)
				pri_ch_idx = (pri_ch - central_ch) >> 1;
			else
				pri_ch_idx = ((central_ch - pri_ch) >> 1) + 1;
			break;

		default:
			break;
		}
		cck_en = false;
	}

	/*==== [Switch CH] ====*/
	rpt &= halbb_fwofld_ctrl_ch_8852c(bb, central_ch, band, phy_idx);
	/*==== [Switch BW] ====*/
	rpt &= halbb_fwofld_ctrl_bw_8852c(bb, pri_ch_idx, bw, phy_idx);
	/*==== [CCK Enable / Disable] ====*/
	halbb_fwofld_ctrl_cck_en_8852c(bb, cck_en, phy_idx);

	/*==== [Spur Suppression] ====*/
	if (bb->phl_com->dev_cap.rfe_type <= 50) { // ==> iFEM only !!
		halbb_fwofld_csi_tone_idx(bb, central_ch, bw, band, phy_idx);
		if (phy_idx == HW_PHY_0) {
			if ((bw == CHANNEL_WIDTH_160) && (pri_ch_idx == 2 || pri_ch_idx == 7)) {
				/*============== [Path A] ==============*/
				halbb_fw_set_reg(bb, 0x4c14, 0x1fff, 0xe7f, 0);
				halbb_fw_set_reg(bb, 0x4c14, BIT(12), 0x1, 0);
				halbb_fw_set_reg(bb, 0x4c20, 0x1fff, 0xe7f, 0);
				halbb_fw_set_reg(bb, 0x4c20, BIT(12), 0x1, 0);
				/*============== [Path B] ==============*/
				if (!bb->hal_com->dbcc_en) {
					halbb_fw_set_reg(bb, 0x4cd8, 0x1fff, 0xe7f, 0);
					halbb_fw_set_reg(bb, 0x4cd8, BIT(12), 0x1, 0);
					halbb_fw_set_reg(bb, 0x4ce4, 0x1fff, 0xe7f, 0);
					halbb_fw_set_reg(bb, 0x4ce4, BIT(12), 0x1, 0);
				}
			} else if ((bw == CHANNEL_WIDTH_160) && (pri_ch_idx == 1 || pri_ch_idx == 8)) {
				/*============== [Path A] ==============*/
				halbb_fw_set_reg(bb, 0x4c14, 0x1fff, 0x280, 0);
				halbb_fw_set_reg(bb, 0x4c14, BIT(12), 0x1, 0);
				halbb_fw_set_reg(bb, 0x4c20, 0x1fff, 0x280, 0);
				halbb_fw_set_reg(bb, 0x4c20, BIT(12), 0x1, 0);
				/*============== [Path B] ==============*/
				if (!bb->hal_com->dbcc_en) {
					halbb_fw_set_reg(bb, 0x4cd8, 0x1fff, 0x280, 0);
					halbb_fw_set_reg(bb, 0x4cd8, BIT(12), 0x1, 0);
					halbb_fw_set_reg(bb, 0x4ce4, 0x1fff, 0x280, 0);
					halbb_fw_set_reg(bb, 0x4ce4, BIT(12), 0x1, 0);
				}
			} else {
				/*============== [Path A] ==============*/
				halbb_fwofld_nbi_tone_idx(bb, central_ch, pri_ch, bw, band, RF_PATH_A);
				/*============== [Path B] ==============*/
				if (!bb->hal_com->dbcc_en)
					halbb_fwofld_nbi_tone_idx(bb, central_ch, pri_ch, bw, band, RF_PATH_B);
			}
		} else {
			/*============== [Path B] ==============*/
			if ((bw == CHANNEL_WIDTH_160) && (pri_ch_idx == 2 || pri_ch_idx == 7)) {
				halbb_fw_set_reg(bb, 0x4cd8, 0x1fff, 0xe7f, 0);
				halbb_fw_set_reg(bb, 0x4cd8, BIT(12), 0x1, 0);
				halbb_fw_set_reg(bb, 0x4ce4, 0x1fff, 0xe7f, 0);
				halbb_fw_set_reg(bb, 0x4ce4, BIT(12), 0x1, 0);
			} else if ((bw == CHANNEL_WIDTH_160) && (pri_ch_idx == 1 || pri_ch_idx == 8)) {
				halbb_fw_set_reg(bb, 0x4cd8, 0x1fff, 0x280, 0);
				halbb_fw_set_reg(bb, 0x4cd8, BIT(12), 0x1, 0);
				halbb_fw_set_reg(bb, 0x4ce4, 0x1fff, 0x280, 0);
				halbb_fw_set_reg(bb, 0x4ce4, BIT(12), 0x1, 0);
			} else {
				halbb_fwofld_nbi_tone_idx(bb, central_ch, pri_ch, bw, band, RF_PATH_B);
			}
		}
	}

	/*==== [PD Boost en] ====*/
	if ((bw == CHANNEL_WIDTH_160) && (pri_ch_idx != 1 && pri_ch_idx != 2 && pri_ch_idx != 7 && pri_ch_idx != 8))
		halbb_fw_set_reg_cmn(bb, 0x49e8, BIT(7), 0x1, phy_idx, 0);
	else
		halbb_fw_set_reg_cmn(bb, 0x49e8, BIT(7), 0x0, phy_idx, 0);

	/*==== [Bw160 Ru_alloc Fixed Item] ====*/
	if ((bw == CHANNEL_WIDTH_160) && (bb->hal_com->cv != CAV)) {
		if (phl_is_mp_mode(bb->phl_com)) {
			rtw_hal_mac_write_msk_pwr_reg(bb->hal_com, (u8)phy_idx, 0xD848, BIT(28), 0);
			if (pri_ch > central_ch) {
				halbb_fw_set_reg(bb, 0x2410, (phy_idx == HW_PHY_0) ? BIT(13) : BIT(14), 1, 0);
				halbb_fw_set_reg_cmn(bb, 0x9d8, BIT(26), 1, phy_idx, 0);
			} else {
				halbb_fw_set_reg(bb, 0x2410, (phy_idx == HW_PHY_0) ? BIT(13) : BIT(14), 0, 0);
				halbb_fw_set_reg_cmn(bb, 0x9d8, BIT(26), 0, phy_idx, 0);
			}
		} else {
			halbb_fw_set_reg_cmn(bb, 0x9d8, BIT(26), 0, phy_idx, 0);
			if (pri_ch > central_ch) {
				halbb_fw_set_reg(bb, 0x2410, (phy_idx == HW_PHY_0) ? BIT(13) : BIT(14), 1, 0);
				rtw_hal_mac_write_msk_pwr_reg(bb->hal_com, (u8)phy_idx, 0xD848, BIT(28), 1);
			} else {
				halbb_fw_set_reg(bb, 0x2410, (phy_idx == HW_PHY_0) ? BIT(13) : BIT(14), 0, 0);
				rtw_hal_mac_write_msk_pwr_reg(bb->hal_com, (u8)phy_idx, 0xD848, BIT(28), 0);
			}
		}
	}

	/*==== [Disable CDD EVM check] ====*/
	if (band == BAND_ON_6G && bw == CHANNEL_WIDTH_160)
		halbb_fw_set_reg_cmn(bb, 0x46c0, BIT(0), 0, phy_idx, 0);
	else
		halbb_fw_set_reg_cmn(bb, 0x46c0, BIT(0), 1, phy_idx, 0);

	BB_DBG(bb, DBG_DBCC, "[%s] bb->rx_path=%d , phy_idx=%d, dbcc_en=%d\n", __func__, bb->rx_path, phy_idx, bb->hal_com->dbcc_en);

	/*==== [BTG Ctrl] ====*/
	if (is_2g_ch && ((bb->rx_path == RF_PATH_B) || (bb->rx_path == RF_PATH_AB)))
		halbb_fwofld_ctrl_btg_8852c(bb, true);
	else
		halbb_fwofld_ctrl_btg_8852c(bb, false);

	/* Dynamic 5M Mask Setting */
	halbb_fwofld_5m_mask_8852c(bb, pri_ch, bw, phy_idx);

	/*==== [TSSI reset] ====*/
	if (!bb->hal_com->dbcc_en) {
		// Path A
		halbb_fw_set_reg(bb, 0x58dc, BIT(31) | BIT(30), 0x1, 0);
		halbb_fw_set_reg(bb, 0x58dc, BIT(31) | BIT(30), 0x3, 0);
		// Path B
		halbb_fw_set_reg(bb, 0x78dc, BIT(31) | BIT(30), 0x1, 0);
		halbb_fw_set_reg(bb, 0x78dc, BIT(31) | BIT(30), 0x3, 0);

		// Path A
		halbb_fw_set_reg(bb, 0x5818, BIT(30), 0x1, 0);
		halbb_fw_set_reg(bb, 0x7818, BIT(30), 0x1, 0);
		//bbreset
		halbb_fw_set_reg(bb, 0x5818, BIT(30), 0x0, 0);
		halbb_fw_set_reg(bb, 0x7818, BIT(30), 0x0, 0);

	} else {
		if (phy_idx == HW_PHY_0) {
			// Path A
			halbb_fw_set_reg(bb, 0x58dc, BIT(31) | BIT(30), 0x1, 0);
			halbb_fw_set_reg(bb, 0x58dc, BIT(31) | BIT(30), 0x3, 0);
		} else {
			// Path B
			halbb_fw_set_reg(bb, 0x78dc, BIT(31) | BIT(30), 0x1, 0);
			halbb_fw_set_reg(bb, 0x78dc, BIT(31) | BIT(30), 0x3, 0);
		}
	}

	/*==== [BB reset] ====*/
	halbb_fwofld_bb_reset_all_8852c(bb, phy_idx);

	return rpt;
}

u8 halbb_fwofld_band_determine_8852c(struct bb_info *bb, u8 central_ch,
				     enum band_type band_type,
				     bool is_normal_efuse)
{
	u8 band = 0, ofdm_tbl_ofst = 1;

	if (band_type == BAND_ON_24G) {
		// 2G Band: (0)
		band = 0;
	} else if (band_type == BAND_ON_5G) {
		// 5G Band: (1):Low, (2): Mid, (3):High
		if (central_ch >= 36 && central_ch <= 64)
			band = 1;
		else if (central_ch >= 100 && central_ch <= 144)
			band = 2;
		else if (central_ch >= 149 && central_ch <= 177)
			band = 3;
		else
			band = 1;
	} else {
		/* 6G Band:
		/  (4):BW160_0, (5):BW160_1   --> Low
		/  (6):BW160_2, (7):BW160_3   --> Mid
		/  (8):BW160_4, (9):BW160_5   --> High
		/  (10):BW160_6, (11):BW160_7 --> Ultra-high
		*/
		if (central_ch >= 1 && central_ch <= 29)
			band = 4;
		else if (central_ch >= 33 && central_ch <= 61)
			band = 5;
		else if (central_ch >= 65 && central_ch <= 93)
			band = 6;
		else if (central_ch >= 97 && central_ch <= 125)
			band = 7;
		else if (central_ch >= 129 && central_ch <= 157)
			band = 8;
		else if (central_ch >= 161 && central_ch <= 189)
			band = 9;
		else if (central_ch >= 193 && central_ch <= 221)
			band = 10;
		else if (central_ch >= 225 && central_ch <= 253)
			band = 11;
		else
			band = 4;
	}

	if (is_normal_efuse)
		band += ofdm_tbl_ofst;

	return band;
}

void halbb_fwofld_ext_loss_avg_update_8852c(struct bb_info *bb,
				     enum band_type band_type)
{
	struct bb_ch_info *ch = &bb->bb_ch_i;
	struct bb_edcca_info *bb_edcca = &bb->bb_edcca_i;
	struct bb_edcca_cr_info *cr = &bb->bb_edcca_i.bb_edcca_cr_i;
	u64 tmp_linear = 0;

	if (ch->ext_loss[0] == ch->ext_loss[1]) {
		ch->ext_loss_avg = ch->ext_loss[0];
	} else {
		/* avg(a_db, b_db) = 10*log10((10^(a_db/10)+10^(b_db/10))/2) =*/
		/* 10*log10((10^((a_db - b_db)/10)+1)/2)+b_db*/
		/* a_db > b_db => a_db - b_db > 0*/
		tmp_linear = halbb_db_2_linear((u32)DIFF_2(ch->ext_loss[0], ch->ext_loss[1])) + 1;
		tmp_linear = tmp_linear >> 1;
		tmp_linear = (tmp_linear + (1 << (FRAC_BITS - 1))) >> FRAC_BITS;
		ch->ext_loss_avg = (s8)halbb_convert_to_db(tmp_linear);
		ch->ext_loss_avg += MIN_2(ch->ext_loss[0], ch->ext_loss[1]);
	}

	/*edcca loss compensation*/
	if (band_type == BAND_ON_6G) /*bypass 6G for CBP test*/
		bb_edcca->pwrofst = EDCCA_PWROFST_DEFAULT;
	else
		bb_edcca->pwrofst = MAX_2(((ch->ext_loss_avg + 2) / 4) + 16,
					  EDCCA_PWROFST_DEFAULT);

	halbb_fw_set_reg(bb, cr->r_pwrofst, cr->r_pwrofst_m, bb_edcca->pwrofst, 0);

	BB_DBG(bb, DBG_PHY_CONFIG,
	       "[Ext loss] path{avg, a, b}={%d, %d, %d}(0.25dB), edcca_pwrofst = %d dB\n",
	       ch->ext_loss_avg, bb->bb_ch_i.ext_loss[RF_PATH_A],
	       bb->bb_ch_i.ext_loss[RF_PATH_B], bb_edcca->pwrofst - 16);
}

void halbb_fwofld_set_hidden_efuse_8852c(struct bb_info *bb, u8 central_ch, enum band_type band_type, enum rf_path path)
{
	u8 band;
	s32 hidden_efuse = 0;
	s8 gain = 0;
	u32 gain_err_addr[2] = {0x4ACC, 0x4AD8};

	BB_DBG(bb, DBG_PHY_CONFIG, "<====== %s ======>\n", __func__);

	band = halbb_fwofld_band_determine_8852c(bb, central_ch, band_type, false);

	// === [Set hidden efuse] === //
	if (bb->bb_efuse_i.hidden_efuse_check) {
		gain = bb->phl_com->dev_cap.rfe_type > 50 ? bb->bb_efuse_i.gain_cs[path][band] : bb->bb_efuse_i.gain_cg[path][band];
		hidden_efuse = (s32)(gain << 2);
		halbb_fw_set_reg(bb, gain_err_addr[path], 0xff, (hidden_efuse & 0x3f), 0);
		BB_DBG(bb, DBG_PHY_CONFIG, "[Efuse][S%d] Hidden efuse = 0x%x; addr=0x%x, bitmap=0xff\n", path, gain, gain_err_addr[path]);
	} else {
		BB_DBG(bb, DBG_PHY_CONFIG, "[Efuse] Values of hidden efuse are all 0xff, bypass dynamic setting!!\n");
	}
}

void halbb_fwofld_set_normal_efuse_8852c(struct bb_info *bb, u8 central_ch, enum band_type band_type, enum rf_path path)
{
	u8 band;
	s32 normal_efuse = 0, normal_efuse_cck = 0, normal_efuse_offset_cck = 0;
	s32 tmp = 0;
	u32 rssi_ofst_addr[2] = {0x4694, 0x4778};
	u32 rpl_bitmap[2] = {0xff00, 0xff0000};
	u32 rpl_tb_bitmap[2] = {0xff, 0xff00};
	enum phl_phy_idx phy_idx = HW_PHY_0;

	band = halbb_fwofld_band_determine_8852c(bb, central_ch, band_type, true);
	// 6G band normal efuse not yet ready
	// band = band_type == BAND_ON_6G ? 1 : band;

	// === [Set normal efuse] === //
	if (bb->bb_efuse_i.normal_efuse_check) {
		// CCK normal efuse
		if (bb->hal_com->dbcc_en && (path == RF_PATH_B))
			phy_idx = HW_PHY_1;

		if (band_type == BAND_ON_24G) {
			normal_efuse_cck = bb->bb_efuse_i.gain_offset[path][0];
			normal_efuse_offset_cck = bb->bb_efuse_i.efuse_ofst[phy_idx];

			normal_efuse_cck *= (-1);
			// r_11b_rx_rpl_offset
			tmp = (normal_efuse_cck << 3) + (normal_efuse_offset_cck >> 1);
			halbb_fw_set_reg(bb, 0x2340, 0x7f00, (tmp & 0x7f), 0);
			// r_11b_rx_rssi_offset
			// tmp = normal_efuse_cck + (normal_efuse_offset_cck >> 4);
			// halbb_fw_set_reg(bb, 0x232c, 0xff00000, (tmp & 0xff), 0);
		}

		// OFDM normal efuse
		normal_efuse = bb->bb_efuse_i.gain_offset[path][band];

		normal_efuse *= (-1);

		// r_g_offset
		tmp = (normal_efuse << 2) + (bb->bb_efuse_i.efuse_ofst[phy_idx] >> 2);
		tmp *= (-1);
		halbb_fw_set_reg(bb, rssi_ofst_addr[path], 0xff0000, (tmp & 0xff), 0);

		BB_DBG(bb, DBG_PHY_CONFIG,
			"[Normal Efuse][Phy%d] ========== Path=%d ==========\n",
			phy_idx, path);
		BB_DBG(bb, DBG_PHY_CONFIG,
			"[Normal Efuse] normal_efuse=0x%x, efuse_ofst0x%x\n",
			normal_efuse, bb->bb_efuse_i.efuse_ofst[phy_idx]);
		BB_DBG(bb, DBG_PHY_CONFIG,
			"[RSSI] addr=0x%x, bitmask=0xff0000, val=0x%x\n",
			rssi_ofst_addr[path], tmp);

		// r_1_rpl_bias_comp
		tmp = (normal_efuse << 4);
		halbb_fw_set_reg_cmn(bb, 0x4e0c, rpl_bitmap[path], (tmp & 0xff), phy_idx, 0);

		BB_DBG(bb, DBG_PHY_CONFIG,
			"[RPL] addr=0x4e0c, bitmask=0x%x, val=0x%x\n",
			rpl_bitmap[path], tmp);

		// r_tb_rssi_bias_comp
		halbb_fw_set_reg_cmn(bb, 0x4e2c, rpl_tb_bitmap[path], (tmp & 0xff), phy_idx, 0);

		BB_DBG(bb, DBG_PHY_CONFIG,
			"[Normal TB Efuse] addr=0x4e2c, bitmask=0x%x, val=0x%x\n",
			rpl_tb_bitmap[path], tmp);
		/*ext_loss*/
		bb->bb_ch_i.ext_loss[path] = (s8)((normal_efuse << 2) +
						  (bb->bb_efuse_i.efuse_ofst[phy_idx] >> 2));
	} else {
		/*ext_loss*/
		bb->bb_ch_i.ext_loss[path] = 0;
		BB_DBG(bb, DBG_PHY_CONFIG, "[Efuse] Values of normal efuse are all 0xff, bypass dynamic setting!!\n");
	}

	halbb_fwofld_ext_loss_avg_update_8852c(bb, band_type);
}

void halbb_fwofld_set_gain_cr_init_8852c(struct bb_info *bb)
{
	/* This function is to set 2G Rx gain k */
	struct bb_gain_info *gain = &bb->bb_gain_i;
	u8 lna_idx = 0, tia_idx = 0;
	s32 tmp = 0;
	enum rf_path path = RF_PATH_A;
	u32 lna_gain_g[BB_PATH_MAX_8852C][7] = {{0x4678, 0x4678, 0x467C,
						   0x467C, 0x467C, 0x467C,
						   0x4680}, {0x475C, 0x475C,
						   0x4760, 0x4760, 0x4760,
						   0x4760, 0x4764}};
	u32 lna_gain_mask[7] = {0x00ff0000, 0xff000000, 0x000000ff,
				    0x0000ff00, 0x00ff0000, 0xff000000,
				    0x000000ff};
	u32 tia_gain_g[BB_PATH_MAX_8852C][2] = {{0x4680, 0x4680}, {0x4764,
						   0x4764}};
	u32 tia_gain_mask[2] = {0x00ff0000, 0xff000000};
	u32 lna_gain_g_bypass[BB_PATH_MAX_8852C][7] = {{0x4BB8, 0x4BBC, 0x4BBC,
						   0x4BBC, 0x4BBC, 0x4BC0,
						   0x4BC0}, {0x4C7C, 0x4C80,
						   0x4C80, 0x4C80, 0x4C80,
						   0x4C84, 0x4C84}};
	u32 lna_gain_g_bypass_mask[7] = {0xff000000, 0xff, 0xff00, 0xff0000, 0xff000000,
				       0xff, 0xff00};

	for (path = RF_PATH_A; path < BB_PATH_MAX_8852C; path++) {
		// Set 2G LNA value
		for (lna_idx = 0; lna_idx < 7; lna_idx++) {
			tmp = gain->lna_gain[0][path][lna_idx];
			halbb_fw_set_reg(bb, lna_gain_g[path][lna_idx], lna_gain_mask[lna_idx], tmp, 0);
			BB_DBG(bb, DBG_PHY_CONFIG, "lna_gain_g[%d][%d]=0x%x, lna_gain_mask[%d]=0x%x, val=%d\n", path, lna_idx, lna_gain_g[path][lna_idx], lna_idx, lna_gain_mask[lna_idx], tmp);
			tmp = gain->lna_gain_bypass[0][path][lna_idx];
			halbb_fw_set_reg(bb, lna_gain_g_bypass[path][lna_idx], lna_gain_g_bypass_mask[lna_idx], tmp, 0);
		}
		// Set 2G TIA value
		for (tia_idx = 0; tia_idx < 2; tia_idx++) {
			tmp = gain->tia_gain[0][path][tia_idx];
			halbb_fw_set_reg(bb, tia_gain_g[path][tia_idx], tia_gain_mask[tia_idx], tmp, 0);
		}
	}
}

void halbb_fwofld_set_gain_error_8852c(struct bb_info *bb, u8 central_ch,
				       enum band_type band_type,
				       enum rf_path path)
{
	struct bb_gain_info *gain = &bb->bb_gain_i;

	u8 band;
	u8 lna_idx = 0, tia_idx = 0,  i = 0;
	s32 tmp = 0;
	u32 wb_gidx_tmp = 0;
	// u32 lna_gain_g[BB_PATH_MAX_8852C][7] = {{0x4678, 0x4678, 0x467C,
	// 					   0x467C, 0x467C, 0x467C,
	// 					   0x4680}, {0x475C, 0x475C,
	// 					   0x4760, 0x4760, 0x4760,
	// 					   0x4760, 0x4764}};
	u32 lna_gain_a[BB_PATH_MAX_8852C][7] = {{0x45DC, 0x45DC, 0x4660,
						   0x4660, 0x4660, 0x4660,
						   0x4664}, {0x4740, 0x4740,
						   0x4744, 0x4744, 0x4744,
						   0x4744, 0x4748}};
	u32 lna_gain_mask[7] = {0x00ff0000, 0xff000000, 0x000000ff,
				    0x0000ff00, 0x00ff0000, 0xff000000,
				    0x000000ff};
	u32 lna_gain_a_bypass[BB_PATH_MAX_8852C][7] = {{0x4BB4, 0x4BB4, 0x4BB4,
						   0x4BB4, 0x4BB8, 0x4BB8,
						   0x4BB8}, {0x4C78, 0x4C78,
						   0x4C78, 0x4C78, 0x4C7C,
						   0x4C7C, 0x4C7C}};
	u32 lna_gain_a_bypass_mask[7] = {0xff, 0xff00, 0xff0000, 0xff000000, 0xff,
				       0xff00, 0xff0000};
	// u32 lna_gain_g_bypass[BB_PATH_MAX_8852C][7] = {{0x4BB8, 0x4BBC, 0x4BBC,
	// 					   0x4BBC, 0x4BBC, 0x4BC0,
	// 					   0x4BC0}, {0x4C7C, 0x4C80,
	// 					   0x4C80, 0x4C80, 0x4C80,
	// 					   0x4C84, 0x4C84}};
	// u32 lna_gain_g_bypass_mask[7] = {0xff000000, 0xff, 0xff00, 0xff0000, 0xff000000,
	// 			       0xff, 0xff00};
	// u32 tia_gain_g[BB_PATH_MAX_8852C][2] = {{0x4680, 0x4680}, {0x4764,
	// 					   0x4764}};
	u32 tia_gain_a[BB_PATH_MAX_8852C][2] = {{0x4664, 0x4664}, {0x4748,
						   0x4748}};
	u32 tia_gain_mask[2] = {0x00ff0000, 0xff000000};

	u32 lna_op1db_a[BB_PATH_MAX_8852C][7] = {{0x4668, 0x4668, 0x4668,
						 0x4668, 0x466c, 0x466c,
						 0x466c}, {0x474c, 0x474c,
						 0x474c, 0x474c, 0x4750,
						 0x4750, 0x4750}};
	u32 tia_lna_op1db_a[BB_PATH_MAX_8852C][7] = {{0x4670, 0x4670, 0x4670,
						     0x4670, 0x4674, 0x4674,
						     0x4674}, {0x4754, 0x4754,
						     0x4754, 0x4754, 0x4758,
						     0x4758, 0x4758}};
	u32 tia0_lna6_op1db_a[BB_PATH_MAX_8852C] = {0x4674, 0x4758};
	u32 op1db_a_mask[7] = {0xff, 0xff00, 0xff0000, 0xff000000, 0xff, 0xff00,
			       0xff0000};
	u32 lna_op1db_g[BB_PATH_MAX_8852C][7] = {{0x4684, 0x4684, 0x4684,
						 0x4688, 0x4688, 0x4688,
						 0x4688}, {0x4768, 0x4768,
						 0x4768, 0x476C, 0x476C,
						 0x476C, 0x476C}};
	u32 tia_lna_op1db_g[BB_PATH_MAX_8852C][7] = {{0x468C, 0x468C, 0x4690,
						     0x4690, 0x4690, 0x4690,
						     0x4694}, {0x4770, 0x4770,
						     0x4774, 0x4774, 0x4774,
						     0x4774, 0x4778}};
	u32 tia0_lna6_op1db_g[BB_PATH_MAX_8852C] = {0x4694, 0x4778};
	u32 op1db_g_mask[7] = {0xff00, 0xff0000, 0xff000000, 0xff, 0xff00,
			       0xff0000, 0xff000000};
	u32 tia_lna_op1db_g_mask[7] = {0xff0000, 0xff000000, 0xff, 0xff00,
				       0xff0000, 0xff000000, 0xff};

	u32 wb_gidx_elna[BB_PATH_MAX_8852C] = {0x4734, 0x4AA0};
	u32 wb_gidx_lna_tia[BB_PATH_MAX_8852C][4] = {{0x46A0, 0x46BC, 0x4720, 0x4724},
				     {0x49F4, 0x4A70, 0x4A74, 0x4A78}};
	u32 gs_idx[BB_PATH_MAX_8852C][2] = {{0x46A8, 0x46AC}, {0x4A5C, 0x4A60}};
	u32 g_elna[BB_PATH_MAX_8852C] = {0x45DC, 0x4740};

	BB_DBG(bb, DBG_PHY_CONFIG, "<====== %s ======>\n", __func__);

	if (band_type == BAND_ON_24G)
		return;

	band = halbb_fwofld_band_determine_8852c(bb, central_ch, band_type, false);
	// Seperated in 4 bands, with 2 BW160 each, choose gain offset of lower BW160
	if (band_type == BAND_ON_6G)
		band = (band + 4) / 2;

	BB_DBG(bb, DBG_PHY_CONFIG, "[%s] central_ch=%d, band_type=%d, path=%d, band=%d\n", __func__, central_ch, band_type, path, band);

	for (lna_idx = 0; lna_idx < 7; lna_idx++) {
		// Set LNA value
		tmp = gain->lna_gain[band][path][lna_idx];
		halbb_fw_set_reg(bb, lna_gain_a[path][lna_idx], lna_gain_mask[lna_idx], tmp, 0);
		BB_DBG(bb, DBG_PHY_CONFIG, "lna_gain_a[%d][%d]=0x%x, lna_gain_mask[%d]=0x%x, val=%d\n", path, lna_idx, lna_gain_a[path][lna_idx], lna_idx, lna_gain_mask[lna_idx], tmp);
		tmp = gain->lna_gain_bypass[band][path][lna_idx];
		halbb_fw_set_reg(bb, lna_gain_a_bypass[path][lna_idx], lna_gain_a_bypass_mask[lna_idx], tmp, 0);
		// Set op1dB value
		tmp = gain->lna_gain[band][path][lna_idx];
		halbb_fw_set_reg(bb, lna_gain_a[path][lna_idx], lna_gain_mask[lna_idx], tmp, 0);
		tmp = gain->lna_gain_bypass[band][path][lna_idx];
		halbb_fw_set_reg(bb, lna_gain_a_bypass[path][lna_idx], lna_gain_a_bypass_mask[lna_idx], tmp, 0);
	
	}
	// Set TIA0_LNA6 op1dB value
	tmp = gain->tia_lna_op1db[band][path][7];
	halbb_fw_set_reg(bb, tia0_lna6_op1db_a[path], 0xff000000, tmp, 0);

	// Set TIA value
	for (tia_idx = 0; tia_idx < 2; tia_idx++) {
		tmp = gain->tia_gain[band][path][tia_idx];
		halbb_fw_set_reg(bb, tia_gain_a[path][tia_idx], tia_gain_mask[tia_idx], tmp, 0);
	}

	// Set wb_gidx
	if (bb->phl_com->dev_cap.rfe_type >= 51) {
		// wb_gidx_elna
		halbb_fw_set_reg(bb, wb_gidx_elna[path], 0x3fffc0, gain->wb_gidx_elna[band][path], 0);
		// wb_gidx_lna_tia
		wb_gidx_tmp = gain->wb_gidx_lna_tia[band][path][0];
		halbb_fw_set_reg(bb, wb_gidx_lna_tia[path][0], 0xE0000000, wb_gidx_tmp, 0);

		wb_gidx_tmp = (gain->wb_gidx_lna_tia[band][path][2] << 3) | gain->wb_gidx_lna_tia[band][path][1];
		halbb_fw_set_reg(bb, wb_gidx_lna_tia[path][1], 0x3F000000, wb_gidx_tmp, 0);

		wb_gidx_tmp = 0;
		for (i = 0; i < 10; i++)
			wb_gidx_tmp |= (gain->wb_gidx_lna_tia[band][path][i + 3] << (3 * i));
		halbb_fw_set_reg(bb, wb_gidx_lna_tia[path][2], 0x3FFFFFFF, wb_gidx_tmp, 0);

		wb_gidx_tmp = (gain->wb_gidx_lna_tia[band][path][15] << 6) |
				(gain->wb_gidx_lna_tia[band][path][14] << 3) |
				gain->wb_gidx_lna_tia[band][path][13];
		halbb_fw_set_reg(bb, wb_gidx_lna_tia[path][3], 0x1FF, wb_gidx_tmp, 0);
		// gs_idx
		wb_gidx_tmp = gain->gs_idx[band][path][0] & 0xffff;
		halbb_fw_set_reg(bb, gs_idx[path][0], 0x7FFF8000, wb_gidx_tmp, 0);

		wb_gidx_tmp = (gain->gs_idx[band][path][1] << 16) |
				((gain->gs_idx[band][path][0] & 0xffff0000) >> 16);
		halbb_fw_set_reg(bb, gs_idx[path][1], 0x0FFFFFFF, wb_gidx_tmp, 0);
		// g_elna
		wb_gidx_tmp = (gain->g_elna[band][path][1] << 8) | gain->g_elna[band][path][0];
		halbb_fw_set_reg(bb, g_elna[path], 0xFFFF, wb_gidx_tmp, 0);
	}
}

void halbb_fwofld_set_rxsc_rpl_comp_8852c(struct bb_info *bb, u8 central_ch)
{
	struct bb_gain_info* gain = &bb->bb_gain_i;
	u8 band;
	u8 path = 0;
	u8 i = 0;
	u8 rxsc = 0;
	s8 ofst = 0;
	s8 bw20_avg = 0;
	s8 bw40_avg = 0, bw40_avg_1 = 0, bw40_avg_2 = 0;
	s8 bw80_avg = 0;
	s8 bw80_avg_1 = 0, bw80_avg_2 = 0, bw80_avg_3 = 0, bw80_avg_4 = 0;
	s8 bw80_avg_9 = 0, bw80_avg_10 = 0;
	u32 tmp_val1 = 0, tmp_val2 = 0, tmp_val3 = 0;
	u32 tmp_val1_tb = 0, tmp_val2_tb = 0, tmp_val3_tb = 0, tmp_val4_tb = 0;


	BB_DBG(bb, DBG_PHY_CONFIG, "<====== %s ======>\n", __func__);

	if (central_ch >= 0 && central_ch <= 14) {
		band = 0;
	}
	else if (central_ch >= 36 && central_ch <= 64) {
		band = 1;
	}
	else if (central_ch >= 100 && central_ch <= 144) {
		band = 2;
	}
	else if (central_ch >= 149 && central_ch <= 177) {
		band = 3;
	}
	else {
		band = 0;
	}
	//20M RPL
	bw20_avg = (gain->rpl_ofst_20[band][RF_PATH_A] +
		gain->rpl_ofst_20[band][RF_PATH_B]) >> 1;
	tmp_val1 |= ((u32)bw20_avg & 0xff);
	tmp_val1_tb |= ((u32)bw20_avg & 0xff);
	//40M RPL
	bw40_avg = (gain->rpl_ofst_40[band][RF_PATH_A][0] +
		gain->rpl_ofst_40[band][RF_PATH_B][0]) >> 1;
	tmp_val1 |= (((u32)bw40_avg & 0xff) << 8);
	tmp_val2_tb |= ((u32)bw40_avg & 0xff);

	bw40_avg_1 = (gain->rpl_ofst_40[band][RF_PATH_A][1] +
		gain->rpl_ofst_40[band][RF_PATH_B][1]) >> 1;
	tmp_val1 |= (((u32)bw40_avg_1 & 0xff) << 16);
	tmp_val2_tb |= (((u32)bw40_avg_1 & 0xff) << 8);

	bw40_avg_2 = (gain->rpl_ofst_40[band][RF_PATH_A][2] +
		gain->rpl_ofst_40[band][RF_PATH_B][2]) >> 1;
	tmp_val1 |= (((u32)bw40_avg_2 & 0xff) << 24);
	tmp_val2_tb |= (((u32)bw40_avg_2 & 0xff) << 16);
	//80M RPL
	bw80_avg = (gain->rpl_ofst_80[band][RF_PATH_A][0] +
		gain->rpl_ofst_80[band][RF_PATH_B][0]) >> 1;
	tmp_val2 |= (u32)(bw80_avg & 0xff);
	tmp_val2_tb |= (((u32)bw80_avg & 0xff) << 24);

	bw80_avg_1 = (gain->rpl_ofst_80[band][RF_PATH_A][1] +
		gain->rpl_ofst_80[band][RF_PATH_B][1]) >> 1;
	tmp_val2 |= (((u32)bw80_avg_1 & 0xff) << 8);
	tmp_val3_tb |= ((u32)bw80_avg_1 & 0xff);

	bw80_avg_10 = (gain->rpl_ofst_80[band][RF_PATH_A][10] +
		gain->rpl_ofst_80[band][RF_PATH_B][10]) >> 1;
	tmp_val2 |= (((u32)bw80_avg_10 & 0xff) << 16);
	tmp_val3_tb |= (((u32)bw80_avg_10 & 0xff) << 8);

	bw80_avg_2 = (gain->rpl_ofst_80[band][RF_PATH_A][2] +
		gain->rpl_ofst_80[band][RF_PATH_B][2]) >> 1;
	tmp_val2 |= (((u32)bw80_avg_2 & 0xff) << 24);
	tmp_val3_tb |= (((u32)bw80_avg_2 & 0xff) << 16);

	bw80_avg_3 = (gain->rpl_ofst_80[band][RF_PATH_A][3] +
		gain->rpl_ofst_80[band][RF_PATH_B][3]) >> 1;
	tmp_val3 |= ((u32)bw80_avg_3 & 0xff);
	tmp_val3_tb |= (((u32)bw80_avg_3 & 0xff) << 24);

	bw80_avg_4 = (gain->rpl_ofst_80[band][RF_PATH_A][4] +
		gain->rpl_ofst_80[band][RF_PATH_B][4]) >> 1;
	tmp_val3 |= (((u32)bw80_avg_4 & 0xff) << 8);
	tmp_val4_tb |= ((u32)bw80_avg_4 & 0xff);

	bw80_avg_9 = (gain->rpl_ofst_80[band][RF_PATH_A][9] +
		gain->rpl_ofst_80[band][RF_PATH_B][9]) >> 1;
	tmp_val3 |= (((u32)bw80_avg_9 & 0xff) << 16);
	tmp_val4_tb |= (((u32)bw80_avg_9 & 0xff) << 8);

	BB_DBG(bb, DBG_PHY_CONFIG, "[20M RPL] gain ofst = 0x%2x\n",
		bw20_avg & 0xff);
	BB_DBG(bb, DBG_PHY_CONFIG, "[40M RPL] gain ofst = 0x%2x, 0x%2x, 0x%2x\n",
		bw40_avg & 0xff, bw40_avg_1 & 0xff, bw40_avg_2 & 0xff);
	BB_DBG(bb, DBG_PHY_CONFIG, "[80M RPL] gain ofst = 0x%2x, 0x%2x, 0x%2x, 0x%2x, 0x%2x, 0x%2x, 0x%2x\n",
		bw80_avg & 0xff, bw80_avg_1 & 0xff, bw80_avg_2 & 0xff, bw80_avg_3 & 0xff, bw80_avg_4 & 0xff, bw80_avg_9 & 0xff, bw80_avg_10 & 0xff);
	BB_DBG(bb, DBG_PHY_CONFIG, "tmp1 = 0x%x, tmp2 = 0x%x, tmp3 = 0x%x\n",
		tmp_val1, tmp_val2, tmp_val3);
	BB_DBG(bb, DBG_PHY_CONFIG, "tmp1_tb = 0x%x, tmp2_tb = 0x%x, tmp3_tb = 0x%x, tmp4_tb = 0x%x\n",
		tmp_val1_tb, tmp_val2_tb, tmp_val3_tb, tmp_val4_tb);

	// Common
	halbb_fw_set_reg(bb, 0x4e00, MASKDWORD, tmp_val1, 0);
	halbb_fw_set_reg(bb, 0x4e04, MASKDWORD, tmp_val2, 0);
	halbb_fw_set_reg(bb, 0x4e08, 0xffffff, tmp_val3, 0);
	// TB
	halbb_fw_set_reg(bb, 0x4e1c, 0xff000000, tmp_val1_tb, 0);
	halbb_fw_set_reg(bb, 0x4e20, MASKDWORD, tmp_val2_tb, 0);
	halbb_fw_set_reg(bb, 0x4e24, MASKDWORD, tmp_val3_tb, 0);
	halbb_fw_set_reg(bb, 0x4e28, 0xffff, tmp_val4_tb, 0);
}
#endif
#endif


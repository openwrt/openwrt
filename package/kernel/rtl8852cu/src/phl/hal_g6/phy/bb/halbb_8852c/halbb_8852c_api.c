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

u8 halbb_ex_cn_report_8852c(struct bb_info *bb)
{
	struct bb_cmn_rpt_info	*cmn_rpt = &bb->bb_cmn_rpt_i;
	struct bb_pkt_cnt_su_info *pkt_cnt = &cmn_rpt->bb_pkt_cnt_su_i;
	struct bb_physts_acc_info *acc = &cmn_rpt->bb_physts_acc_i;
	struct bb_physts_avg_info *avg = &cmn_rpt->bb_physts_avg_i;
	u8 cn_report = 0;

	// halbb_set_reg(bb, 0x429c, BIT(23), 0x1);
	avg->cn_avg = (u8)HALBB_DIV(acc->cn_avg_acc, pkt_cnt->pkt_cnt_2ss);
	cn_report = (avg->cn_avg >> 1);
	BB_DBG(bb, DBG_DBG_API, "[CN_avg] = %d\n", (avg->cn_avg >> 1));
	return cn_report;

}

u8 halbb_ex_evm_1ss_report_8852c(struct bb_info *bb)
{
	struct bb_cmn_rpt_info	*cmn_rpt = &bb->bb_cmn_rpt_i;
	struct bb_pkt_cnt_su_info *pkt_cnt = &cmn_rpt->bb_pkt_cnt_su_i;
	struct bb_physts_acc_info *acc = &cmn_rpt->bb_physts_acc_i;
	struct bb_physts_avg_info *avg = &cmn_rpt->bb_physts_avg_i;
	u8 evm_1ss_report = 0;

	avg->evm_1ss = (u8)HALBB_DIV(acc->evm_1ss, (pkt_cnt->pkt_cnt_1ss + pkt_cnt->pkt_cnt_ofdm));

	evm_1ss_report = (avg->evm_1ss >> 2);

	BB_DBG(bb, DBG_DBG_API, "[EVM_1ss] = %d\n", evm_1ss_report);


	return evm_1ss_report;
}

u8 halbb_ex_evm_max_report_8852c(struct bb_info *bb)
{
	struct bb_cmn_rpt_info	*cmn_rpt = &bb->bb_cmn_rpt_i;
	struct bb_pkt_cnt_su_info *pkt_cnt = &cmn_rpt->bb_pkt_cnt_su_i;
	struct bb_physts_acc_info *acc = &cmn_rpt->bb_physts_acc_i;
	struct bb_physts_avg_info *avg = &cmn_rpt->bb_physts_avg_i;
	u8 evm_max_report = 0;

	avg->evm_max = (u8)HALBB_DIV(acc->evm_max_acc, pkt_cnt->pkt_cnt_2ss);

	evm_max_report = (avg->evm_max >> 2);

	BB_DBG(bb, DBG_DBG_API, "[EVM_max] = %d\n", evm_max_report);

	return evm_max_report;
}

u8 halbb_ex_evm_min_report_8852c(struct bb_info *bb)
{
	struct bb_cmn_rpt_info	*cmn_rpt = &bb->bb_cmn_rpt_i;
	struct bb_pkt_cnt_su_info *pkt_cnt = &cmn_rpt->bb_pkt_cnt_su_i;
	struct bb_physts_acc_info *acc = &cmn_rpt->bb_physts_acc_i;
	struct bb_physts_avg_info *avg = &cmn_rpt->bb_physts_avg_i;
	u8 evm_min_report = 0;

	avg->evm_min = (u8)HALBB_DIV(acc->evm_min_acc, pkt_cnt->pkt_cnt_2ss);

	evm_min_report = (avg->evm_min >> 2);

	BB_DBG(bb, DBG_DBG_API, "[EVM_min] = %d\n", evm_min_report);

	return evm_min_report;
}

bool halbb_set_vht_mu_user_idx_8852c(struct bb_info *bb, bool en, u8 idx,
				     enum phl_phy_idx phy_idx) {

	if (idx >= 4)
		return false;

	halbb_set_reg_cmn(bb, 0x0994, BIT5, en, phy_idx);
	halbb_set_reg_cmn(bb, 0x0994, BIT13 | BIT12, idx, phy_idx);

	return true;
}

void halbb_edcca_per20_bitmap_sifs_8852c(struct bb_info *bb,
					 enum channel_width bw,
					 enum phl_phy_idx phy_idx)
{
	if (bw == CHANNEL_WIDTH_20) {
		halbb_set_reg_cmn(bb, 0x0c9c, 0xFF000, 0xff, phy_idx);
		halbb_set_reg_cmn(bb, 0x0cA0, 0xFF000, 0, phy_idx);
	} else {
		halbb_set_reg_cmn(bb, 0x0c9c, 0xFF000, 0, phy_idx);
		halbb_set_reg_cmn(bb, 0x0cA0, 0xFF000, 0, phy_idx);
	}
}

bool halbb_set_pwr_ul_tb_ofst_8852c(struct bb_info *bb,
				      s8 pw_ofst, enum phl_phy_idx phy_idx)
{
	s8 pw_ofst_2tx = 0;
	u8 i;
	s8 val_1tx[4] = {0};
	s8 val_2tx[4] = {0};

	/*S(8,2) for 8852C*/
	if (pw_ofst < -32 || pw_ofst > 31) {
		BB_WARNING("[%s] ofst=%d\n", __func__, pw_ofst);
		return false;
	}

	/*ECO en*/
	rtw_hal_mac_write_msk_pwr_reg(bb->hal_com, (u8)phy_idx, 0xD288, BIT31, 1);

	for (i = 0; i < 4; i++) {
		val_1tx[i] = pw_ofst << 2;

		pw_ofst_2tx = pw_ofst - 3;
		if (pw_ofst_2tx < -32)
			pw_ofst_2tx = -32;
		val_2tx[i] = pw_ofst_2tx << 2;

		if (pw_ofst < -32)
			pw_ofst = -32;
	}

	BB_DBG(bb, DBG_DBG_API, "val_1tx=0x%x, 0x%x, 0x%x, 0x%x\n",
	       val_1tx[3], val_1tx[2], val_1tx[1], val_1tx[0]);
	BB_DBG(bb, DBG_DBG_API, "val_2tx=0x%x, 0x%x, 0x%x, 0x%x\n",
	       val_2tx[3], val_2tx[2], val_2tx[1], val_2tx[0]);

	for (i = 0; i < 4; i++) {
		/*1 TX*/
		rtw_hal_mac_write_msk_pwr_reg(bb->hal_com, (u8)phy_idx, 0xD28c, 0xff << (8 * i), val_1tx[i]);
		/*2 TX*/
		rtw_hal_mac_write_msk_pwr_reg(bb->hal_com, (u8)phy_idx, 0xD290, 0xff << (8 * i), val_2tx[i]);
	}

	return true;
}

bool halbb_lps_info_8852c(struct bb_info *bb, u16 mac_id)
{
/*
	struct bb_h2c_lps_info_8852c *lps_info;
	u32 *bb_h2c;
	u8 cmdlen = sizeof(struct bb_h2c_lps_info_8852c);
	u16 lps_info_len = 0;
	bool ret_val = true, h2c_done = false;
	u8 i = 0;

	lps_info = hal_mem_alloc(bb->hal_com, cmdlen);

	bb_h2c = (u32 *)lps_info;

	lps_info->central_ch = bb->phl_sta_info[mac_id]->chandef.center_ch;
	lps_info->pri_ch = bb->phl_sta_info[mac_id]->chandef.chan;
	lps_info->bw = (u8)bb->phl_sta_info[mac_id]->chandef.bw;
	lps_info->band_type = (u8)bb->phl_sta_info[mac_id]->chandef.band;

	ret_val = halbb_fill_h2c_cmd(bb, cmdlen, DM_H2C_FW_LPS_INFO,
				     HALBB_H2C_DM, bb_h2c);

	hal_mem_free(bb->hal_com, lps_info, cmdlen);

	BB_DBG(bb, DBG_FW_INFO, "LPS info=>h2c start\n");

	while (!h2c_done) {
		h2c_done = (bool)halbb_get_reg(bb, 0x1e0, BIT(0));
		halbb_delay_us(bb, 50);
		i++;
		if (i > 100) {
			ret_val = false;
			break;
		}
	}

	BB_DBG(bb, DBG_FW_INFO, "LPS info=>h2c[0]: %x, h2c_done: %d\n", bb_h2c[0], h2c_done);

	// Return h2c_done flag to default
	halbb_set_reg(bb, 0x1e0, BIT(0), 0);

	BB_DBG(bb, DBG_FW_INFO, "LPS info=>h2c[0]: %x\n", bb_h2c[0]);

	return ret_val;
*/
	return true;
}

void halbb_digital_cfo_comp_8852c(struct bb_info *bb, s32 curr_cfo)
{
	struct bb_cfo_trk_cr_info *cr = &bb->bb_cfo_trk_i.bb_cfo_trk_cr_i;
	struct bb_link_info *bb_link = &bb->bb_link_i;
	s32 cfo_avg_312; /*in unit of sub-carrier spacing*/
	s32 digital_cfo_comp_offset;
	bool is_positive = IS_GREATER(curr_cfo, 0);

	if (!bb_link->is_linked) {
		BB_DBG(bb, DBG_CFO_TRK, "[%s] is_linked=%d\n", __func__,
		       bb_link->is_linked);
		return;
	}

	if (curr_cfo == 0) {
		BB_DBG(bb, DBG_CFO_TRK, "curr_cfo=0\n");
		return;
	}

	BB_DBG(bb, DBG_CFO_TRK, "[%s]\n", __func__);

	// For manually fine tune digital cfo
	digital_cfo_comp_offset = halbb_get_reg(bb, 0x4264, 0x000000FF);

	/*CR{S(14,13} = (CFO_avg{S(12,2)} << 11) / 312.5*/
	/*CR = X(KHz) << 11 / 312.5(KHz) = X << 12 / 625 ~= X*(4000/625) = (X*32)/5 = (X << 5)/5 */
	if(is_positive) {
		cfo_avg_312 = HALBB_DIV(curr_cfo << 5, 5) + digital_cfo_comp_offset;
	} else {
		cfo_avg_312 = HALBB_DIV(curr_cfo << 5, 5) - digital_cfo_comp_offset;
	}

	BB_DBG(bb, DBG_CFO_TRK, "[52C] cfo_avg_312 = %d\n", cfo_avg_312);

	halbb_print_sign_frac_digit(bb, curr_cfo, 32, 2, bb->dbg_buf, HALBB_SNPRINT_SIZE);
	BB_DBG(bb, DBG_CFO_TRK, "[CFO_DBG] [Digital Comp] cfo: %s KHz\n", bb->dbg_buf);

	halbb_print_sign_frac_digit(bb, cfo_avg_312, 32, 13, bb->dbg_buf, HALBB_SNPRINT_SIZE);
	BB_DBG(bb, DBG_CFO_TRK, "[CFO_DBG] cfo_avg_312: %s * 312.5KHz\n", bb->dbg_buf);

	halbb_set_reg(bb, cr->r_cfo_comp_seg0_312p5khz, cr->r_cfo_comp_seg0_312p5khz_m, cfo_avg_312);
}

void halbb_tx_triangular_shap_cfg_8852c(struct bb_info *bb, u8 shape_idx,
					  enum phl_phy_idx phy_idx) {

	if(phl_is_mp_mode(bb->phl_com)) {
		halbb_set_reg(bb, 0x9a4, BIT(31), 0x1);
        	bb->pmac_tri_idx = shape_idx;

		if (shape_idx == TSSI_BANDEDGE_FLAT)
			bb->pmac_pwr_ofst = 0;
		else if (shape_idx == TSSI_BANDEDGE_LOW)
			bb->pmac_pwr_ofst = 0x10;
		else if (shape_idx == TSSI_BANDEDGE_MID)
			bb->pmac_pwr_ofst = 0x10;
		else if (shape_idx == TSSI_BANDEDGE_HIGH)
			bb->pmac_pwr_ofst = 0x10;
        }
	halbb_tssi_ctrl_set_bandedge_cfg(bb, phy_idx,
					 (enum tssi_bandedge_cfg)shape_idx);

	if (bb->phl_com->dev_cap.rfe_type >= 50) {
		// RF power compensate at txagc
		halbb_set_reg(bb, 0x58F8, BIT(31), 0x0);
		halbb_set_reg(bb, 0x78F8, BIT(31), 0x0);
	} else {
		// RF power compensate at TSSI offset
		halbb_set_reg(bb, 0x58F8, BIT(31), 0x1);
		halbb_set_reg(bb, 0x78F8, BIT(31), 0x1);
	}
}

void halbb_set_digital_pwr_comp_8852c(struct bb_info *bb, bool en, enum phl_phy_idx phy_idx)
{
	u16 i = 0;
	u16 pw_a_cr = 0;
	u16 pw_b_cr = 0;
	u32 table_len = sizeof(pwr_comp_1ss_8852c)/sizeof(u32);
	u32 *pwr_comp_2ss_52c = NULL;

	pw_a_cr = 0x5428;
	pw_b_cr = 0x7428;

	pwr_comp_2ss_52c = bb->phl_com->dev_cap.rfe_type <= 50 ?
			   (u32 *)pwr_comp_2ss_8852c : (u32 *)pwr_comp_2ss_efem_8852c;

	if (en) {
		while (i < table_len) {
			halbb_set_reg_cmn(bb, pw_a_cr, MASKDWORD, pwr_comp_2ss_52c[i], phy_idx);
			halbb_set_reg_cmn(bb, pw_b_cr, MASKDWORD, pwr_comp_2ss_52c[i + 1], phy_idx);
			pw_a_cr += 0x4;
			pw_b_cr += 0x4;
			i += 2;
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

void halbb_tx_dfir_shap_cck_8852c(struct bb_info *bb, u8 ch, u8 shape_idx,
				  enum phl_phy_idx phy_idx) {

	u32 para_flat[8] = {0x003D23FF, 0x0029B354, 0x000FC1C8, 0x00FDB053,
			    0x00F86F9A, 0x00FAEF92, 0x00FE5FCC, 0x00FFDFF5};
	u32 para_sharp[8] = {0x003D83FF, 0x002C636A, 0x0013F204, 0x00008090,
			     0x00F87FB0, 0x00F99F83, 0x00FDBFBA, 0x00003FF5};
	u32 para_sharp_14[8] = {0x003B13FF, 0x001C42DE, 0x00FDB0AD, 0x00F60F6E,
			        0x00FD8F92, 0x0002D011, 0x0001C02C, 0x00FFF00A};
	u32 filter_addr[8] = {0x45BC, 0x45CC, 0x45D0, 0x45D4, 0x45D8, 0x45C0,
			      0x45C4, 0x45C8};
	u32 *para = NULL;
	u8 i = 0;

	BB_DBG(bb, DBG_DBG_API, "[%s] ch=%d, shape_idx=%d\n", __func__, ch, shape_idx);

	if (ch > 14)
		return;

	if (ch == 14) {
		para = para_sharp_14;
	} else {
		if (shape_idx == 0) {
		/*flat CH1~14*/
			para = para_flat;
		} else {
		/*Sharp( b mode tx dfir)*/
			para = para_sharp;
		}
	}

	for (i = 0; i < 8; i++) {
		halbb_set_reg(bb, filter_addr[i], 0xffffff, para[i]);
		BB_DBG(bb, DBG_DBG_API, "Reg0x%08x = 0x%08x\n", filter_addr[i], para[i]);
	}
}

void halbb_bb_reset_8852c(struct bb_info *bb, enum phl_phy_idx phy_idx)
{
	BB_DBG(bb, DBG_DBG_API, "%s\n", __func__);
}

void halbb_bb_reset_all_8852c(struct bb_info* bb, enum phl_phy_idx phy_idx)
{
	BB_DBG(bb, DBG_DBG_API, "%s\n", __func__);

	//Protest HW-SI
	halbb_set_reg_cmn(bb, 0x1200, BIT(28) | BIT(29) | BIT(30), 0x7, phy_idx);
	halbb_set_reg_cmn(bb, 0x3200, BIT(28) | BIT(29) | BIT(30), 0x7, phy_idx);
	halbb_delay_us(bb, 1);
	// === [BB reset] === //
	halbb_set_reg_cmn(bb, 0x704, BIT(1), 1, phy_idx);
	halbb_set_reg_cmn(bb, 0x704, BIT(1), 0, phy_idx);

	halbb_set_reg_cmn(bb, 0x1200, BIT(28) | BIT(29) | BIT(30), 0x0, phy_idx);
	halbb_set_reg_cmn(bb, 0x3200, BIT(28) | BIT(29) | BIT(30), 0x0, phy_idx);
	halbb_set_reg_cmn(bb, 0x704, BIT(1), 1, phy_idx);
}

void halbb_bb_reset_en_8852c(struct bb_info *bb, bool en, enum phl_phy_idx phy_idx)
{
	bool path_0_en = false, path_1_en = false;

	BB_DBG(bb, DBG_DBG_API, "%s\n", __func__);

	if (bb->hal_com->dbcc_en) {
		if (phy_idx == HW_PHY_0)
			path_0_en = true;
		else
			path_1_en = true;
	} else {
		path_0_en = true;
		path_1_en = true;
	}

	if (en) {
		if (path_0_en)
			halbb_set_reg_cmn(bb, 0x1200, 0x70000000, 0x0, phy_idx); /*path-A-Domain*/
		if (path_1_en)
			halbb_set_reg_cmn(bb, 0x3200, 0x70000000, 0x0, phy_idx); /*path-B-Domain*/

		halbb_set_reg_cmn(bb, 0x704, BIT(1), 1, phy_idx);
		//PD Enable
		if (bb->hal_com->band[phy_idx].cur_chandef.band == BAND_ON_24G)
			halbb_set_reg(bb, 0x2320, BIT(0), 0x0);

		halbb_set_reg_cmn(bb, 0xc3c, BIT(9), 0x0, phy_idx);
	} else {
		//PD Disable
		if (bb->hal_com->band[phy_idx].cur_chandef.band == BAND_ON_24G)
			halbb_set_reg(bb, 0x2320, BIT(0), 0x1);

		halbb_set_reg_cmn(bb, 0xc3c, BIT(9), 0x1, phy_idx);
		//Protest SW-SI
		if (path_0_en)
			halbb_set_reg_cmn(bb, 0x1200, 0x70000000, 0x7, phy_idx); /*path-A-Domain*/
		if (path_1_en)
			halbb_set_reg_cmn(bb, 0x3200, 0x70000000, 0x7, phy_idx); /*path-B-Domain*/

		halbb_delay_us(bb, 1);
		halbb_set_reg_cmn(bb, 0x704, BIT(1), 0, phy_idx);
	}
}

void halbb_fix_rx_htmcs0_8852c(struct bb_info *bb, bool en, enum phl_phy_idx phy_idx)
{
	BB_DBG(bb, DBG_DBG_API, "%s\n", __func__);

	if(!phl_is_mp_mode(bb->phl_com))
		return;

	if (en) {
		/*Disable CCK CCA*/
		halbb_set_reg(bb, 0x2320, BIT(0), 1);
		/*Break packet format != HT*/
		halbb_set_reg(bb, 0x49a4, 0xf, 0xf);
		halbb_set_reg(bb, 0x49a4, 0x60, 0x3);
		/*Break MCS rate > MCS0*/
		halbb_set_reg(bb, 0x0d00, MASKBYTE0, 0x0);
		halbb_set_reg(bb, 0x0d00, MASKBYTE2, 0x0);
	} else {
		halbb_set_reg(bb, 0x2320, BIT(0), 0);
		halbb_set_reg(bb, 0x49a4, 0xf, 0);
		halbb_set_reg(bb, 0x49a4, 0x60, 0);
		halbb_set_reg(bb, 0x0d00, MASKBYTE0, 0x77);
		halbb_set_reg(bb, 0x0d00, MASKBYTE2, 0x77);
	}
}

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

	if (path > RF_PATH_B || path < RF_PATH_A) {
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
	if (path >= RF_PATH_A && path <= RF_PATH_B)
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
	if (path > RF_PATH_B || path < RF_PATH_A) {
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
	if (path > RF_PATH_B || path < RF_PATH_A) {
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
	if (path > RF_PATH_B || path < RF_PATH_A) {
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
	if (path > RF_PATH_B || path < RF_PATH_A) {
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

void halbb_dfs_en_8852c(struct bb_info *bb, bool en)
{
	BB_DBG(bb, DBG_DBG_API, "%s\n", __func__);

	if (en)
		halbb_set_reg(bb, 0x0, BIT(31), 1);
	else
		halbb_set_reg(bb, 0x0, BIT(31), 0);
}

void halbb_adc_en_8852c(struct bb_info *bb, bool en, enum phl_phy_idx phy_idx)
{
	u32 val_tmp = 0;

	BB_DBG(bb, DBG_DBG_API, "%s\n", __func__);

	val_tmp = halbb_get_reg(bb, 0x20fc, 0xff000000);

	if (en) {
		if (phy_idx == HW_PHY_0) /*mapping to path-0*/
			val_tmp &= ~0x1;
		else
			val_tmp &= ~0x2; /*mapping to path-1*/
	} else {
		if (phy_idx == HW_PHY_0) /*mapping to path-0*/
			val_tmp |= 0x1;
		else
			val_tmp |= 0x2; /*mapping to path-1*/
	}

	halbb_set_reg(bb, 0x20fc, 0xff000000, val_tmp);
}

bool halbb_adc_cfg_8852c(struct bb_info *bb, enum channel_width bw,
			 enum rf_path path)
{
	u32 idac2[2] = {0xC0D4, 0xC1D4};
	u32 idac2_1[2] = {0xC0D4, 0xC1D4};
	u32 adc_sample_td[2] = {0xC0D4, 0xC1D4};
	u32 adc_op5_bw_sel[2] = {0xC0D8, 0xC1D8};
	u32 rck_offset[2] = {0xC0C4, 0xC1C4};
	u32 rck_reset_count[2] = {0xC0E8, 0xC1E8};
	u32 wbadc_sel[2] = {0xC0E4, 0xC1E4};
	u32 rx_adc_clk[2] = {0x12A0, 0x32A0};
	u32 decim_filter[2] = {0xC0EC, 0xC1EC};
	u32 adc_rst_cycle[2] = {0xC0EC, 0xC1EC};

	BB_DBG(bb, DBG_DBG_API, "%s\n", __func__);

	halbb_set_reg(bb, idac2[path], 0x780, 0x8);
	halbb_set_reg(bb, rck_reset_count[path], 0xFFFF0000, 0x9);
	halbb_set_reg(bb, wbadc_sel[path], 0x30, 0x2);
	halbb_set_reg(bb, rx_adc_clk[path], 0xFF800000, 0x49);
	halbb_set_reg(bb, decim_filter[path], 0x6000, 0x0);

	switch (bw) {
	case CHANNEL_WIDTH_5:
	case CHANNEL_WIDTH_10:
	case CHANNEL_WIDTH_20:
	case CHANNEL_WIDTH_40:/*ADC clock = 80M & WB ADC clock = 160M */
		halbb_set_reg(bb, idac2_1[path], 0x7800, 0x2);
		halbb_set_reg(bb, adc_sample_td[path], 0xC000000, 0x3);
		halbb_set_reg(bb, adc_op5_bw_sel[path], 0x1E0, 0xf);
		halbb_set_reg(bb, rck_offset[path], 0x3E0000, 0x0);
		if (bb->phl_com->dev_cap.rfe_type >= 51)
			halbb_set_reg(bb, adc_rst_cycle[path], 0xFF0000, 0x2);
		else
			halbb_set_reg(bb, adc_rst_cycle[path], 0xFF0000, 0x3);
		break;
	case CHANNEL_WIDTH_80:/*ADC clock = 160M & WB ADC clock = 160M */
		halbb_set_reg(bb, idac2_1[path], 0x7800, 0x2);
		halbb_set_reg(bb, adc_sample_td[path], 0xC000000, 0x2);
		halbb_set_reg(bb, adc_op5_bw_sel[path], 0x1E0, 0x8);
		halbb_set_reg(bb, rck_offset[path], 0x3E0000, 0x0);
		halbb_set_reg(bb, adc_rst_cycle[path], 0xFF0000, 0x3);
		break;
	case CHANNEL_WIDTH_160:/*ADC clock = 160M & WB ADC clock = 160M */
		halbb_set_reg(bb, idac2_1[path], 0x7800, 0x0);
		halbb_set_reg(bb, adc_sample_td[path], 0xC000000, 0x2);
		halbb_set_reg(bb, adc_op5_bw_sel[path], 0x1E0, 0x4);
		halbb_set_reg(bb, rck_offset[path], 0x3E0000, 0x6);
		halbb_set_reg(bb, adc_rst_cycle[path], 0xFF0000, 0x3);
		break;
	default:
		BB_WARNING("Fail to set ADC\n");
		return false;
	}

	return true;
}

void halbb_tssi_cont_en_8852c(struct bb_info *bb, bool en, enum rf_path path)
{
	u32 tssi_trk_man[2] = {0x5818, 0x7818};

	BB_DBG(bb, DBG_DBG_API, "%s\n", __func__);

	if (en) {
		halbb_set_reg(bb, tssi_trk_man[path], BIT(30), 0x0);
		if (bb->hal_com->dbcc_en && (path == RF_PATH_B))
			rtw_hal_rf_tssi_scan_ch(bb->hal_com, HW_PHY_1, path);
		else
			rtw_hal_rf_tssi_scan_ch(bb->hal_com, HW_PHY_0, path);
	} else {
		halbb_set_reg(bb, tssi_trk_man[path], BIT(30), 0x1);
	}
}

void halbb_5m_mask_8852c(struct bb_info *bb, u8 pri_ch_idx, enum channel_width bw,
			   enum phl_phy_idx phy_idx)
{
	bool mask_5m_low = false;
	bool mask_5m_en = false;

	switch (bw) {
		case CHANNEL_WIDTH_40:
			/* Prich=1 : Mask 5M High
			   Prich=2 : Mask 5M Low */
			mask_5m_en = true;
			mask_5m_low = pri_ch_idx == 2 ? true : false;
			break;
		case CHANNEL_WIDTH_80:
			/* Prich=3 : Mask 5M High
			   Prich=4 : Mask 5M Low
			   Else    : Mask 5M Disable */
			mask_5m_en = ((pri_ch_idx == 3) || (pri_ch_idx == 4)) ? true : false;
			mask_5m_low = pri_ch_idx == 4 ? true : false;
			break;
		default:
			mask_5m_en = false;
			break;
	}

	BB_DBG(bb, DBG_PHY_CONFIG, "[5M Mask] pri_ch_idx = %d, bw = %d", pri_ch_idx, bw);

	if (!mask_5m_en) {
		halbb_set_reg(bb, 0x4c4c, BIT(12), 0x0);
		halbb_set_reg(bb, 0x4d10, BIT(12), 0x0);
		halbb_set_reg_cmn(bb, 0x4450, BIT(24), 0x0, phy_idx);
	} else {
		if (mask_5m_low) {
			halbb_set_reg(bb, 0x4c4c, 0x3f, 0x4);
			halbb_set_reg(bb, 0x4c4c, BIT(12), 0x1);
			halbb_set_reg(bb, 0x4c4c, BIT(8), 0x0);
			halbb_set_reg(bb, 0x4c4c, BIT(6), 0x1);
			halbb_set_reg(bb, 0x4d10, 0x3f, 0x4);
			halbb_set_reg(bb, 0x4d10, BIT(12), 0x1);
			halbb_set_reg(bb, 0x4d10, BIT(8), 0x0);
			halbb_set_reg(bb, 0x4d10, BIT(6), 0x1);
		} else {
			halbb_set_reg(bb, 0x4c4c, 0x3f, 0x4);
			halbb_set_reg(bb, 0x4c4c, BIT(12), 0x1);
			halbb_set_reg(bb, 0x4c4c, BIT(8), 0x1);
			halbb_set_reg(bb, 0x4c4c, BIT(6), 0x0);
			halbb_set_reg(bb, 0x4d10, 0x3f, 0x4);
			halbb_set_reg(bb, 0x4d10, BIT(12), 0x1);
			halbb_set_reg(bb, 0x4d10, BIT(8), 0x1);
			halbb_set_reg(bb, 0x4d10, BIT(6), 0x0);
		}
		halbb_set_reg_cmn(bb, 0x4450, BIT(24), 0x1, phy_idx);
	}
}

bool halbb_ctrl_sco_cck_8852c(struct bb_info *bb, u8 pri_ch)
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

	halbb_set_reg(bb, 0x4a1c, 0x7ffff, sco_barker_threshold[pri_ch - 1]);
	halbb_set_reg(bb, 0x4a20, 0x7ffff, sco_cck_threshold[pri_ch - 1]);
	BB_DBG(bb, DBG_PHY_CONFIG, "[CCK SCO Success]");
	return true;
}

void halbb_ctrl_btg_8852c(struct bb_info *bb, bool btg)
{
	struct rtw_phl_com_t *phl = bb->phl_com;
	struct dev_cap_t *dev = &phl->dev_cap;
	bool s1_2g_band = false;

	if (dev->rfe_type >= 50)
		return;

	BB_DBG(bb, DBG_PHY_CONFIG, "<====== %s ======>\n", __func__);

	if (btg) {
		// Path A
		halbb_set_reg(bb, 0x4738, BIT(19), 0x1);
		halbb_set_reg(bb, 0x4738, BIT(22), 0x0);
		// Path B
		halbb_set_reg(bb, 0x476c, 0xFF000000, 0x20);
		halbb_set_reg(bb, 0x4778, 0xFF, 0x30);
		halbb_set_reg(bb, 0x4aa4, BIT(19), 0x1);
		halbb_set_reg(bb, 0x4aa4, BIT(22), 0x1);
		BB_DBG(bb, DBG_PHY_CONFIG, "[BT] Apply BTG Setting\n");
		// Apply Grant BT by TMAC Setting
		halbb_set_reg(bb, 0x980, 0x1e0000, 0x0);
		BB_DBG(bb, DBG_PHY_CONFIG, "[BT] Apply Grant BT by TMAC Setting\n");
		// Add BT share
		halbb_set_reg(bb, 0x4978, BIT(14), 0x1);
		halbb_set_reg(bb, 0x4974, 0x3c00000, 0x2);
		/* To avoid abnormal 1R CCA without BT, set rtl only 0xc6c[21] = 0x1 */
		halbb_set_reg(bb, 0x441c, BIT(31), 0x1);
		halbb_set_reg(bb, 0xc6c, BIT(21), 0x1);
	} else {
		// Path A
		halbb_set_reg(bb, 0x4738, BIT(19), 0x0);
		halbb_set_reg(bb, 0x4738, BIT(22), 0x0);
		// Path B
		halbb_set_reg(bb, 0x476c, 0xFF000000, 0x1a);
		halbb_set_reg(bb, 0x4778, 0xFF, 0x2a);
		halbb_set_reg(bb, 0x4aa4, BIT(19), 0x0);
		halbb_set_reg(bb, 0x4aa4, BIT(22), 0x0);
		BB_DBG(bb, DBG_PHY_CONFIG, "[BT] Disable BTG Setting\n");
		// Ignore Grant BT by PMAC Setting
		halbb_set_reg(bb, 0x980, 0x1e0000, 0xf);
		halbb_set_reg(bb, 0x980, 0x3c000000, 0x4);
		BB_DBG(bb, DBG_PHY_CONFIG, "[BT] Ignore Grant BT by PMAC Setting\n");
		// Reset BT share
		halbb_set_reg(bb, 0x4978, BIT(14), 0x0);
		halbb_set_reg(bb, 0x4974, 0x3c00000, 0x0);
		/* To avoid abnormal 1R CCA without BT, set rtl only 0xc6c[21] = 0x1 */
		halbb_set_reg(bb, 0x441c, BIT(31), 0x0);
		halbb_set_reg(bb, 0xc6c, BIT(21), 0x0);
	}
}
bool halbb_spur_location_for_CSI_8852c(struct bb_info *bb, u8 central_ch,
				       enum channel_width bw, enum band_type band,
				       u32 *intf)
{
	bool rpt = true;

	if (band == BAND_ON_6G) {
		if (bb->phl_com->dev_cap.rfe_type == 5) {
			if (central_ch == 9)
				*intf = 5993;
			else if (central_ch == 57)
				*intf = 6233;
			else if (central_ch == 105)
				*intf = 6473;
			else if (central_ch == 153)
				*intf = 6713;
			else if (central_ch == 201)
				*intf = 6953;
			else if (central_ch == 249)
				*intf = 7193;
			else
				rpt = false;
		} else {
			rpt = false;
		}
	} else {
		rpt = false;
	}
	return rpt;
}
bool halbb_spur_location_8852c(struct bb_info *bb, u8 central_ch,
			       enum channel_width bw, enum band_type band,
			       u32 *intf)
{
        bool rpt = true;

        if (band == BAND_ON_24G) {
                if ((bw == CHANNEL_WIDTH_20) && ((central_ch >= 5) &&
                    (central_ch <= 8)))
                        *intf = 2440;
                else if ((bw == CHANNEL_WIDTH_20) && (central_ch == 13))
                        *intf = 2480;
                else if ((bw == CHANNEL_WIDTH_40) && ((central_ch >= 3) &&
                         (central_ch <= 10)))
                        *intf = 2440;
                else
                        return false;
        } else if (band == BAND_ON_5G) {
                if ((central_ch == 151) || (central_ch == 153) ||
                    (central_ch == 155) || (central_ch == 163))
                        *intf = 5760;
                else
                        rpt = false;
        } else if (band == BAND_ON_6G) {
		if (bb->phl_com->dev_cap.rfe_type == 5) {
			if (central_ch == 7)
				*intf = 5960;
			else if (central_ch == 23)
				*intf = 6040;
			else if (central_ch == 39)
				*intf = 6120;
			else if (central_ch == 55)
				*intf = 6200;
			else if (central_ch == 71)
				*intf = 6280;
			else if (central_ch == 87)
				*intf = 6360;
			else if (central_ch == 103)
				*intf = 6440;
			else if (central_ch == 119)
				*intf = 6520;
			else if (central_ch == 135)
				*intf = 6600;
			else if (central_ch == 151)
				*intf = 6680;
			else if (central_ch == 167)
				*intf = 6760;
			else if (central_ch == 183)
				*intf = 6840;
			else if ((central_ch == 195) || (central_ch == 197) ||
			   (central_ch == 207) || (central_ch == 199))
				*intf = 6920;
			else
				rpt = false;
		} else {
			if ((central_ch == 195) || (central_ch == 197) ||
			   (central_ch == 207) || (central_ch == 199))
				*intf = 6920;
			else
				rpt = false;
		}

        } else {
                rpt = false;
        }
        return rpt;
}

bool halbb_ctrl_bw_8852c(struct bb_info *bb, u8 pri_ch_idx, enum channel_width bw,
			 enum phl_phy_idx phy_idx)
{
	BB_DBG(bb, DBG_PHY_CONFIG, "<====== %s ======>\n", __func__);

	if (bb->is_disable_phy_api) {
		BB_DBG(bb, DBG_PHY_CONFIG, "[%s] Disable PHY API\n", __func__);
		return true;
	}

	/*==== Error handling ====*/
	if (bw >= CHANNEL_WIDTH_MAX || (bw == CHANNEL_WIDTH_40 && pri_ch_idx > 2) ||
	    (bw == CHANNEL_WIDTH_80 && pri_ch_idx > 4) || (bw == CHANNEL_WIDTH_160
	    && pri_ch_idx > 8)) {
		BB_WARNING("Fail to switch bw(bw:%d, pri ch:%d)\n", bw,
			   pri_ch_idx);
		return false;
	}

	/*==== Switch bandwidth ====*/
	switch (bw) {
	case CHANNEL_WIDTH_5:
	case CHANNEL_WIDTH_10:
	case CHANNEL_WIDTH_20:
		if (bw == CHANNEL_WIDTH_5) {
			/*RF_BW:[31:30]=0x0 */
			halbb_set_reg_cmn(bb, 0x4974, 0xC0000000, 0x0,
					  phy_idx);
			/*small BW:[13:12]=0x1 */
			halbb_set_reg_cmn(bb, 0x4978, 0x3000, 0x1, phy_idx);
			/*Pri ch:[11:8]=0x0 */
			halbb_set_reg_cmn(bb, 0x4978, 0xf00, 0x0, phy_idx);
			/*ACI Detect:[16]=0x0 */
			halbb_set_reg_cmn(bb, 0x4738, 0x10000, 0x0, phy_idx);
			halbb_set_reg_cmn(bb, 0x4AA4, 0x10000, 0x0, phy_idx);
		} else if (bw == CHANNEL_WIDTH_10) {
			/*RF_BW:[31:30]=0x0 */
			halbb_set_reg_cmn(bb, 0x4974, 0xC0000000, 0x0,
					  phy_idx);
			/*small BW:[13:12]=0x2 */
			halbb_set_reg_cmn(bb, 0x4978, 0x3000, 0x2, phy_idx);
			/*Pri ch:[11:8]=0x0 */
			halbb_set_reg_cmn(bb, 0x4978, 0xf00, 0x0, phy_idx);
			/*ACI Detect:[16]=0x0 */
			halbb_set_reg_cmn(bb, 0x4738, 0x10000, 0x0, phy_idx);
			halbb_set_reg_cmn(bb, 0x4AA4, 0x10000, 0x0, phy_idx);
		} else if (bw == CHANNEL_WIDTH_20) {
			/*RF_BW:[31:30]=0x0 */
			halbb_set_reg_cmn(bb, 0x4974, 0xC0000000, 0x0,
					  phy_idx);
			/*small BW:[13:12]=0x0 */
			halbb_set_reg_cmn(bb, 0x4978, 0x3000, 0x0, phy_idx);
			/*Pri ch:[11:8]=0x0 */
			halbb_set_reg_cmn(bb, 0x4978, 0xf00, 0x0, phy_idx);
			/*ACI Detect:[16]=0x1 */
			halbb_set_reg_cmn(bb, 0x4738, 0x10000, 0x1, phy_idx);
			halbb_set_reg_cmn(bb, 0x4AA4, 0x10000, 0x1, phy_idx);
		}
		break;
	case CHANNEL_WIDTH_40:
		/*RF_BW:[31:30]=0x1 */
		halbb_set_reg_cmn(bb, 0x4974, 0xC0000000, 0x1, phy_idx);
		/*small BW:[13:12]=0x0 */
		halbb_set_reg_cmn(bb, 0x4978, 0x3000, 0x0, phy_idx);
		/*Pri ch:[11:8] */
		halbb_set_reg_cmn(bb, 0x4978, 0xf00, pri_ch_idx, phy_idx);
		break;
	case CHANNEL_WIDTH_80:
		/*RF_BW:[31:30]=0x2 */
		halbb_set_reg_cmn(bb, 0x4974, 0xC0000000, 0x2, phy_idx);
		/*small BW:[13:12]=0x0 */
		halbb_set_reg_cmn(bb, 0x4978, 0x3000, 0x0, phy_idx);
		/*Pri ch:[11:8] */
		halbb_set_reg_cmn(bb, 0x4978, 0xf00, pri_ch_idx, phy_idx);
		break;
	case CHANNEL_WIDTH_160:
		/*RF_BW:[31:30]=0x2 */
		halbb_set_reg_cmn(bb, 0x4974, 0xC0000000, 0x3, phy_idx);
		/*small BW:[13:12]=0x0 */
		halbb_set_reg_cmn(bb, 0x4978, 0x3000, 0x0, phy_idx);
		/*Pri ch:[11:8] */
		halbb_set_reg_cmn(bb, 0x4978, 0xf00, pri_ch_idx, phy_idx);
		break;
	default:
		BB_WARNING("Fail to switch bw (bw:%d, pri ch:%d)\n", bw,
			   pri_ch_idx);
	}

	/*==== r_Rx_BW40_2xFFT_en setting ====*/
	if (bw == CHANNEL_WIDTH_40) {
		halbb_set_reg_cmn(bb, 0x4e30, BIT(26), 1, phy_idx);
		halbb_set_reg_cmn(bb, 0x4424, BIT(2), 1, phy_idx);
	} else {
		halbb_set_reg_cmn(bb, 0x4e30, BIT(26), 0, phy_idx);
		halbb_set_reg_cmn(bb, 0x4424, BIT(2), 0, phy_idx);
	}

	if (phy_idx == HW_PHY_0) {
		/*============== [Path A] ==============*/
		halbb_adc_cfg_8852c(bb, bw, RF_PATH_A);
		/*============== [Path B] ==============*/
		if (!bb->hal_com->dbcc_en)
			halbb_adc_cfg_8852c(bb, bw, RF_PATH_B);
	} else {
		/*============== [Path B] ==============*/
		halbb_adc_cfg_8852c(bb, bw, RF_PATH_B);
	}

	halbb_edcca_per20_bitmap_sifs_8852c(bb, bw, phy_idx);

	BB_DBG(bb, DBG_PHY_CONFIG,
		  "[Switch BW Success] BW: %d for PHY%d\n", bw, phy_idx);

	return true;
}

bool halbb_ctrl_ch_8852c(struct bb_info *bb, u8 central_ch, enum band_type band,
			   enum phl_phy_idx phy_idx)
{
	u8 sco;
	u16 central_freq;
	u8 ch_idx_encoded = 0;

	BB_DBG(bb, DBG_DBCC, "[%s] central_ch=%d, band=%d, phy_idx=%d, dbcc_en=%d\n",
		__func__, central_ch, band, phy_idx, bb->hal_com->dbcc_en);

	if (bb->is_disable_phy_api) {
		BB_DBG(bb, DBG_PHY_CONFIG, "[%s] Disable PHY API\n", __func__);
		return true;
	}
	/*==== Error handling ====*/
	if (band != BAND_ON_6G) {
		if ((central_ch > 14 && central_ch < 36) ||
		    (central_ch > 64 && central_ch < 100) ||
		    (central_ch > 144 && central_ch < 149) ||
		    central_ch > 177) {
			BB_WARNING("Invalid 2/5G CH:%d for PHY%d\n", central_ch,
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
		halbb_set_gain_error_8852c(bb, central_ch, band, RF_PATH_A);
		//------------- [Set efuse] ------------//
		halbb_set_hidden_efuse_8852c(bb, central_ch, band, RF_PATH_A);
		halbb_set_normal_efuse_8852c(bb, central_ch, band, RF_PATH_A);
		//------------- [Mode Sel - Path A] ------------//
		if (band == BAND_ON_24G)
			halbb_set_reg(bb, 0x4738, BIT(17), 1);
		else
			halbb_set_reg(bb, 0x4738, BIT(17), 0);

		/*============== [Path B] ==============*/
		if (!bb->hal_com->dbcc_en) {
			//------------- [Set Gain Error] ------------//
			halbb_set_gain_error_8852c(bb, central_ch, band, RF_PATH_B);
			//------------- [Set efuse] ------------//
			halbb_set_hidden_efuse_8852c(bb, central_ch, band, RF_PATH_B);
			halbb_set_normal_efuse_8852c(bb, central_ch, band, RF_PATH_B);
			//------------- [Mode Sel - Path B] ------------//
			if (band == BAND_ON_24G)
				halbb_set_reg(bb, 0x4aa4, BIT(17), 1);
			else
				halbb_set_reg(bb, 0x4aa4, BIT(17), 0);
		}
		/*==== [SCO compensate fc setting] ====*/
		central_freq = halbb_fc_mapping(bb, band, central_ch);
		halbb_set_reg_cmn(bb, 0x4e30, 0x1fff, central_freq, phy_idx);

		/*=== SCO compensate : (BIT(0) << 18) / central_ch ===*/
		sco = (u8)HALBB_DIV((BIT(0) << 18) + (central_freq / 2), central_freq);
		halbb_set_reg_cmn(bb, 0x4974, 0x7f, sco, phy_idx);
	} else {
		/*============== [Path B] ==============*/
		//------------- [Set Gain Error] ------------//
		halbb_set_gain_error_8852c(bb, central_ch, band, RF_PATH_B);
		//------------- [Set efuse] ------------//
		halbb_set_hidden_efuse_8852c(bb, central_ch, band, RF_PATH_B);
		halbb_set_normal_efuse_8852c(bb, central_ch, band, RF_PATH_B);
		//------------- [Mode Sel - Path B] ------------//
		if (band == BAND_ON_24G)
			halbb_set_reg(bb, 0x4aa4, BIT(17), 1);
		else
			halbb_set_reg(bb, 0x4aa4, BIT(17), 0);

		/*==== [SCO compensate fc setting] ====*/
		central_freq = halbb_fc_mapping(bb, band, central_ch);
		halbb_set_reg_cmn(bb, 0x4e30, 0x1fff, central_freq, phy_idx);

		/*=== SCO compensate : (BIT(0) << 18) / central_ch ===*/
		sco = (u8)HALBB_DIV((BIT(0) << 18) + (central_freq / 2), central_freq);
		halbb_set_reg_cmn(bb, 0x4974, 0x7f, sco, phy_idx);
	}

	if (band == BAND_ON_24G) {
		/* === CCK Parameters === */
		if (central_ch == 14) {
			halbb_set_reg(bb, 0x45bc, 0xffffff, 0x3b13ff);
			halbb_set_reg(bb, 0x45cc, 0xffffff, 0x1c42de);
			halbb_set_reg(bb, 0x45d0, 0xffffff, 0xfdb0ad);
			halbb_set_reg(bb, 0x45d4, 0xffffff, 0xf60f6e);
			halbb_set_reg(bb, 0x45d8, 0xffffff, 0xfd8f92);
			halbb_set_reg(bb, 0x45c0, 0xffffff, 0x2d011);
			halbb_set_reg(bb, 0x45c4, 0xffffff, 0x1c02c);
			halbb_set_reg(bb, 0x45c8, 0xffffff, 0xfff00a);
		} else {
			halbb_set_reg(bb, 0x45bc, 0xffffff, 0x3d23ff);
			halbb_set_reg(bb, 0x45cc, 0xffffff, 0x29b354);
			halbb_set_reg(bb, 0x45d0, 0xffffff, 0xfc1c8);
			halbb_set_reg(bb, 0x45d4, 0xffffff, 0xfdb053);
			halbb_set_reg(bb, 0x45d8, 0xffffff, 0xf86f9a);
			halbb_set_reg(bb, 0x45c0, 0xffffff, 0xfaef92);
			halbb_set_reg(bb, 0x45c4, 0xffffff, 0xfe5fcc);
			halbb_set_reg(bb, 0x45c8, 0xffffff, 0xffdff5);
		}
		/* === Set RXSC RPL Comp === */
		//halbb_set_rxsc_rpl_comp_8852c(bb, central_ch);
	}
	/* === Set Ch idx report in phy-sts === */
	halbb_ch_idx_encode(bb, central_ch, band, &ch_idx_encoded);
	halbb_set_reg_cmn(bb, 0x0734, 0x0ff0000, ch_idx_encoded, phy_idx);

	BB_DBG(bb, DBG_PHY_CONFIG, "[Switch CH Success] CH: %d for PHY%d\n",
	       central_ch, phy_idx);
	return true;
}

bool halbb_query_cck_en_8852c(struct bb_info *bb, enum phl_phy_idx phy_idx,
			      u8 func_idx)
{
	bool cck_en;

	cck_en = (bool)halbb_get_reg(bb, 0x2320, BIT(0));

	BB_DBG(bb, DBG_DBCC, "[%s]cck_en=%d PHY%d, func_idx=%d\n", __func__, cck_en, phy_idx, func_idx);

	return cck_en;
}

void halbb_ctrl_cck_blk_en_8852c(struct bb_info *bb, bool cck_blk_en,
				 enum phl_phy_idx phy_idx)
{
	if (cck_blk_en) {
		halbb_set_reg_cmn(bb, 0x700, BIT(5), 1, phy_idx);
		halbb_set_reg_cmn(bb, 0xc80, BIT(31), 0, phy_idx);
	} else {
		halbb_set_reg_cmn(bb, 0x700, BIT(5), 0, phy_idx);
		halbb_set_reg_cmn(bb, 0xc80, BIT(31), 1, phy_idx);
	}
}

void halbb_ctrl_cck_en_8852c(struct bb_info *bb, bool cck_en,
			     enum phl_phy_idx phy_idx, u8 func_idx)
{
	struct bb_info *bb_phy0 = bb;
#ifdef HALBB_DBCC_SUPPORT
	struct bb_info *bb_phy1 = bb;
	u32 cck_en_ori = (halbb_get_reg(bb, 0x2320, BIT(0))) ? false : true;

	HALBB_GET_PHY_PTR(bb, bb_phy0, HW_PHY_0);

	BB_DBG(bb, DBG_DBCC, "[%s]cck_en=%d->%d PHY%d, func_idx=%d\n",
	       __func__, cck_en_ori, cck_en, phy_idx, func_idx);

	if (bb->hal_com->dbcc_en &&
	    bb->phl_com->dev_cap.dbcc_sup &&
	    (bb->bb_ch_i.is_2g || (bb->bb_phy_hooker && bb->bb_phy_hooker->bb_ch_i.is_2g))) {
		cck_en = true;
		BB_DBG(bb, DBG_DBCC, "Frc DBCC cck_en ===> 1\n");
	}
#endif
	halbb_ctrl_cck_blk_en_8852c(bb, bb_phy0->bb_ch_i.is_2g ? 1 : 0, HW_PHY_0);
#ifdef HALBB_DBCC_SUPPORT
	if (bb->hal_com->dbcc_en) {
		HALBB_GET_PHY_PTR(bb, bb_phy1, HW_PHY_1);
		halbb_ctrl_cck_blk_en_8852c(bb, bb_phy1->bb_ch_i.is_2g ? 1 : 0, HW_PHY_1);
	}

	bb->bb_cmn_hooker->cck_blk_en = cck_en;

	if (cck_en_ori == cck_en)
		return;
#endif
	if (cck_en)
		halbb_set_reg(bb, 0x2320, BIT(0), 0);
	else
		halbb_set_reg(bb, 0x2320, BIT(0), 1);
}

bool halbb_ctrl_bw_ch_8852c(struct bb_info *bb_0, u8 pri_ch, u8 central_ch,
				 enum channel_width bw, enum band_type band,
				 enum phl_phy_idx phy_idx)
{
	struct bb_info *bb = bb_0;
	bool rpt = true;
	bool cck_en = false;
	bool is_2g_ch;
	bool s1_2g_band = false;
	u8 pri_ch_idx = 0;
	u32 intf;

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
		rpt &= halbb_ctrl_sco_cck_8852c(bb, pri_ch);

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
	rpt &= halbb_ctrl_ch_8852c(bb, central_ch, band, phy_idx);
	/*==== [Switch BW] ====*/
	rpt &= halbb_ctrl_bw_8852c(bb, pri_ch_idx, bw, phy_idx);
	/*==== [CCK Enable / Disable] ====*/
#if 0//def HALBB_DBCC_SUPPORT
	if (bb->hal_com->dbcc_en &&
	    bb->phl_com->dev_cap.dbcc_sup &&
	    (bb->bb_ch_i.is_2g || bb->bb_phy_hooker->bb_ch_i.is_2g)) {
		cck_en = true;
	}
	bb->bb_cmn_hooker->cck_blk_en = cck_en;
#else
	halbb_ctrl_cck_en_8852c(bb, cck_en, phy_idx, 0);
#endif

	/*==== [Spur Suppression] ====*/
	if (bb->phl_com->dev_cap.rfe_type <= 50) { // ==> iFEM only !!
		halbb_csi_tone_idx(bb, central_ch, bw, band, phy_idx);
		if (phy_idx == HW_PHY_0) {
			if ((bw == CHANNEL_WIDTH_160) && (pri_ch_idx == 2 || pri_ch_idx == 7)) {
				/*============== [Path A] ==============*/
				halbb_set_reg(bb, 0x4c14, 0x1fff, 0xe7f);
				halbb_set_reg(bb, 0x4c14, BIT(12), 0x1);
				halbb_set_reg(bb, 0x4c20, 0x1fff, 0xe7f);
				halbb_set_reg(bb, 0x4c20, BIT(12), 0x1);
				/*============== [Path B] ==============*/
				if (!bb->hal_com->dbcc_en) {
					halbb_set_reg(bb, 0x4cd8, 0x1fff, 0xe7f);
					halbb_set_reg(bb, 0x4cd8, BIT(12), 0x1);
					halbb_set_reg(bb, 0x4ce4, 0x1fff, 0xe7f);
					halbb_set_reg(bb, 0x4ce4, BIT(12), 0x1);
				}
			} else if ((bw == CHANNEL_WIDTH_160) && (pri_ch_idx == 1 || pri_ch_idx == 8)) {
				/*============== [Path A] ==============*/
				halbb_set_reg(bb, 0x4c14, 0x1fff, 0x280);
				halbb_set_reg(bb, 0x4c14, BIT(12), 0x1);
				halbb_set_reg(bb, 0x4c20, 0x1fff, 0x280);
				halbb_set_reg(bb, 0x4c20, BIT(12), 0x1);
				/*============== [Path B] ==============*/
				if (!bb->hal_com->dbcc_en) {
					halbb_set_reg(bb, 0x4cd8, 0x1fff, 0x280);
					halbb_set_reg(bb, 0x4cd8, BIT(12), 0x1);
					halbb_set_reg(bb, 0x4ce4, 0x1fff, 0x280);
					halbb_set_reg(bb, 0x4ce4, BIT(12), 0x1);
				}
			} else {
				/*============== [Path A] ==============*/
				halbb_nbi_tone_idx(bb, central_ch, pri_ch, bw, band, RF_PATH_A);
				/*============== [Path B] ==============*/
				if (!bb->hal_com->dbcc_en)
					halbb_nbi_tone_idx(bb, central_ch, pri_ch, bw, band, RF_PATH_B);
			}
		} else {
			/*============== [Path B] ==============*/
			if ((bw == CHANNEL_WIDTH_160) && (pri_ch_idx == 2 || pri_ch_idx == 7)) {
				halbb_set_reg(bb, 0x4cd8, 0x1fff, 0xe7f);
				halbb_set_reg(bb, 0x4cd8, BIT(12), 0x1);
				halbb_set_reg(bb, 0x4ce4, 0x1fff, 0xe7f);
				halbb_set_reg(bb, 0x4ce4, BIT(12), 0x1);
			} else if ((bw == CHANNEL_WIDTH_160) && (pri_ch_idx == 1 || pri_ch_idx == 8)) {
				halbb_set_reg(bb, 0x4cd8, 0x1fff, 0x280);
				halbb_set_reg(bb, 0x4cd8, BIT(12), 0x1);
				halbb_set_reg(bb, 0x4ce4, 0x1fff, 0x280);
				halbb_set_reg(bb, 0x4ce4, BIT(12), 0x1);
			} else {
				halbb_nbi_tone_idx(bb, central_ch, pri_ch, bw, band, RF_PATH_B);
			}
		}
		if((bw == CHANNEL_WIDTH_80) && (band == BAND_ON_6G) &&
		    halbb_spur_location_8852c(bb, central_ch, bw, band, &intf) &&
		    (bb->phl_com->dev_cap.rfe_type == 5))
			halbb_set_reg(bb, 0x47ac, 0x1F, 0x12);
		else
			halbb_set_reg(bb, 0x47ac, 0x1F, 0x11);

		if((bw == CHANNEL_WIDTH_20) && (band == BAND_ON_6G) &&
		    halbb_spur_location_for_CSI_8852c(bb, central_ch, bw, band, &intf) &&
		    (bb->phl_com->dev_cap.rfe_type == 5)) {
			halbb_set_reg(bb, 0x42c4, BIT(24), 0x1);
			halbb_set_reg(bb, 0x42ac, 0x3ff800, 0x7ea);
		    } else {
			halbb_set_reg(bb, 0x42c4, BIT(24), 0x0);
		    }
	}

	/*==== [PD Boost en] ====*/
	if ((bw == CHANNEL_WIDTH_160) && (pri_ch_idx != 1 && pri_ch_idx != 2 && pri_ch_idx != 7 && pri_ch_idx != 8))
		halbb_set_reg_cmn(bb, 0x49e8, BIT(7), 0x1, phy_idx);
	else
		halbb_set_reg_cmn(bb, 0x49e8, BIT(7), 0x0, phy_idx);

	/*==== [Bw160 Ru_alloc Fixed Item] ====*/
	if ((bw == CHANNEL_WIDTH_160) && (bb->hal_com->cv != CAV)) {
		if (phl_is_mp_mode(bb->phl_com)) {
			rtw_hal_mac_write_msk_pwr_reg(bb->hal_com, (u8)phy_idx, 0xD848, BIT(28), 0);
			if (pri_ch > central_ch) {
				halbb_set_reg(bb, 0x2410, (phy_idx == HW_PHY_0) ? BIT(13) : BIT(14), 1);
				halbb_set_reg_cmn(bb, 0x9d8, BIT(26), 1, phy_idx);
			} else {
				halbb_set_reg(bb, 0x2410, (phy_idx == HW_PHY_0) ? BIT(13) : BIT(14), 0);
				halbb_set_reg_cmn(bb, 0x9d8, BIT(26), 0, phy_idx);
			}
		} else {
			halbb_set_reg_cmn(bb, 0x9d8, BIT(26), 0, phy_idx);
			if (pri_ch > central_ch) {
				halbb_set_reg(bb, 0x2410, (phy_idx == HW_PHY_0) ? BIT(13) : BIT(14), 1);
				rtw_hal_mac_write_msk_pwr_reg(bb->hal_com, (u8)phy_idx, 0xD848, BIT(28), 1);
			} else {
				halbb_set_reg(bb, 0x2410, (phy_idx == HW_PHY_0) ? BIT(13) : BIT(14), 0);
				rtw_hal_mac_write_msk_pwr_reg(bb->hal_com, (u8)phy_idx, 0xD848, BIT(28), 0);
			}
		}
	}

	/*==== [Disable CDD EVM check] ====*/
	if (band == BAND_ON_6G && bw == CHANNEL_WIDTH_160)
		halbb_set_reg_cmn(bb, 0x46c0, BIT(0), 0, phy_idx);
	else
		halbb_set_reg_cmn(bb, 0x46c0, BIT(0), 1, phy_idx);

	BB_DBG(bb, DBG_DBCC, "[%s] bb->rx_path=%d , phy_idx=%d, dbcc_en=%d\n", __func__, bb->rx_path, phy_idx, bb->hal_com->dbcc_en);

	if (is_2g_ch && ((bb->rx_path == RF_PATH_B) || (bb->rx_path == RF_PATH_AB)))
		halbb_ctrl_btg_8852c(bb, true);
	else
		halbb_ctrl_btg_8852c(bb, false);

	if (is_2g_ch)
		halbb_ctrl_btc_preagc_8852c(bb, bb->bt_en);
	else
		halbb_ctrl_btc_preagc_8852c(bb, false);

	/* Dynamic 5M Mask Setting */
	halbb_5m_mask_8852c(bb, pri_ch_idx, bw, phy_idx);

	/*==== [TSSI reset] ====*/
	if (!bb->hal_com->dbcc_en) {
		// Path A
		halbb_set_reg(bb, 0x58dc, BIT(31) | BIT(30), 0x1);
		halbb_set_reg(bb, 0x58dc, BIT(31) | BIT(30), 0x3);
		// Path B
		halbb_set_reg(bb, 0x78dc, BIT(31) | BIT(30), 0x1);
		halbb_set_reg(bb, 0x78dc, BIT(31) | BIT(30), 0x3);


		// Path A
		halbb_set_reg(bb, 0x5818, BIT(30), 0x1);
		halbb_set_reg(bb, 0x7818, BIT(30), 0x1);
		//bbreset
		halbb_set_reg(bb, 0x5818, BIT(30), 0x0);
		halbb_set_reg(bb, 0x7818, BIT(30), 0x0);
	} else {
		if (phy_idx == HW_PHY_0) {
			// Path A
			halbb_set_reg(bb, 0x58dc, BIT(31) | BIT(30), 0x1);
			halbb_set_reg(bb, 0x58dc, BIT(31) | BIT(30), 0x3);
		} else {
			// Path B
			halbb_set_reg(bb, 0x78dc, BIT(31) | BIT(30), 0x1);
			halbb_set_reg(bb, 0x78dc, BIT(31) | BIT(30), 0x3);
		}
	}

	/*==== [BB reset] ====*/
	halbb_bb_reset_all_8852c(bb, phy_idx);

	return rpt;
}

void halbb_1r_cca_cr_cfg_8852c(struct bb_info *bb, enum rf_path rx_path)
{
	if (rx_path == RF_PATH_A) {
		/*path-A CCA only*/
		halbb_set_reg(bb, 0x32b8, 0x1E0000, 1);
		halbb_set_reg(bb, 0x4974, 0x3C000, 1);
		halbb_set_reg(bb, 0x4444, BIT(31), 1);
	} else if (rx_path == RF_PATH_B) {
		/*path-B CCA only*/
		halbb_set_reg(bb, 0x32b8, 0x1E0000, 0);
		halbb_set_reg(bb, 0x4974, 0x3C000, 2);
		halbb_set_reg(bb, 0x4444, BIT(31), 1);
	} else {
		/*path-AB CCA*/
		halbb_set_reg(bb, 0x32b8, 0x1E0000, 0);
		halbb_set_reg(bb, 0x4974, 0x3C000, 3);
		halbb_set_reg(bb, 0x4444, BIT(31), 0);
	}
}

bool halbb_ctrl_rx_path_8852c(struct bb_info *bb_in, enum rf_path rx_path, bool dbcc_en)
{
	struct bb_info *bb = bb_in;
	struct bb_info *bb_0;

#ifdef HALBB_DBCC_SUPPORT

	if (dbcc_en) {
		HALBB_GET_PHY_PTR(bb_in, bb_0, HW_PHY_0);

		bb_0->rx_path = RF_PATH_A;
		BB_DBG(bb, DBG_DBCC, "[%s][1] Update bb_0->rx_path=%d, dbcc_en=%d\n",
			__func__, bb_0->rx_path, dbcc_en);

		if (bb_0->bb_phy_hooker) {
			bb_0->bb_phy_hooker->rx_path = RF_PATH_B;
			BB_DBG(bb, DBG_DBCC, "[%s][1] Update bb_0->bb_cmn_hooker->rx_path=%d\n",
			__func__, bb_0->bb_phy_hooker->rx_path);
		}

		halbb_set_reg(bb, 0x4978, 0xf, 0x1);
		halbb_set_reg(bb, 0x6978, 0xf, 0x2);
		/*==== [PHY0] 1rcca ====*/
		halbb_set_reg(bb, 0x4974, 0x3C000, 1);
		halbb_set_reg(bb, 0x4974, 0x3C0000, 1);
		/*==== [PHY1] 1rcca ====*/
		halbb_set_reg(bb, 0x6974, 0x3C000, 2);
		halbb_set_reg(bb, 0x6974, 0x3C0000, 2);
		/*==== [PHY0] Rx HT nss_limit / mcs_limit ====*/
		halbb_set_reg(bb, 0xd18, BIT(9) | BIT(8), 0);
		halbb_set_reg(bb, 0xd18, BIT(22) | BIT(21), 0);
		/*==== [PHY0] Rx HE n_user_max / tb_max_nss ====*/
		halbb_set_reg(bb, 0xd80, 0x3fc0, HE_N_USER_MAX_8852C);
		halbb_set_reg(bb, 0xd80, BIT(16) | BIT(15) | BIT(14), 0);
		halbb_set_reg(bb, 0xd80, BIT(25) | BIT(24) | BIT(23), 0);
		/*==== [PHY1] Rx HT nss_limit / mcs_limit ====*/
		halbb_set_reg(bb, 0x2d18, BIT(9) | BIT(8), 0);
		halbb_set_reg(bb, 0x2d18, BIT(22) | BIT(21), 0);
		/*==== [PHY1] Rx HE n_user_max / tb_max_nss ====*/
		halbb_set_reg(bb, 0x2d80, 0x3fc0, 1);
		halbb_set_reg(bb, 0x2d80, BIT(16) | BIT(15) | BIT(14), 0);
		halbb_set_reg(bb, 0x2d80, BIT(25) | BIT(24) | BIT(23), 0);
	} else
#endif
	{
		bb->rx_path = rx_path;
		BB_DBG(bb, DBG_DBCC, "[%s][2] Update bb->rx_path=%d, dbcc_en=%d\n", __func__, bb->rx_path, dbcc_en);

		if (rx_path == RF_PATH_A) {
			halbb_set_reg(bb, 0x4978, 0xf, 0x1);
			/*==== 1rcca ====*/
			halbb_set_reg(bb, 0x4974, 0x3C000, 1);
			halbb_set_reg(bb, 0x4974, 0x3C0000, 1);
			/*==== Rx HT nss_limit / mcs_limit ====*/
			halbb_set_reg(bb, 0xd18, BIT(9) | BIT(8), 0);
			halbb_set_reg(bb, 0xd18, BIT(22) | BIT(21), 0);
			/*==== Rx HE n_user_max / tb_max_nss ====*/
			halbb_set_reg(bb, 0xd80, 0x3fc0, HE_N_USER_MAX_8852C);
			halbb_set_reg(bb, 0xd80, BIT(16) | BIT(15) | BIT(14),
				      0);
			halbb_set_reg(bb, 0xd80, BIT(25) | BIT(24) | BIT(23),
				      0);
		} else if (rx_path == RF_PATH_B) {
			halbb_set_reg(bb, 0x4978, 0xf, 0x2);
			/*==== 1rcca ====*/
			halbb_set_reg(bb, 0x4974, 0x3C000, 0x2);
			halbb_set_reg(bb, 0x4974, 0x3C0000, 0x2);
			/*==== Rx HT nss_limit / mcs_limit ====*/
			halbb_set_reg(bb, 0xd18, BIT(9) | BIT(8), 0);
			halbb_set_reg(bb, 0xd18, BIT(22) | BIT(21), 0);
			/*==== Rx HE n_user_max / tb_max_nss ====*/
			halbb_set_reg(bb, 0xd80, 0x3fc0, HE_N_USER_MAX_8852C);
			halbb_set_reg(bb, 0xd80, BIT(16) | BIT(15) | BIT(14),
				      0);
			halbb_set_reg(bb, 0xd80, BIT(25) | BIT(24) | BIT(23),
				      0);
		} else if (rx_path == RF_PATH_AB) {
			halbb_set_reg(bb, 0x4978, 0xf, 0x3);
			/*==== 1rcca ====*/
			halbb_set_reg(bb, 0x4974, 0x3C000, 0x3);
			halbb_set_reg(bb, 0x4974, 0x3C0000, 0x3);
			/*==== Rx HT nss_limit / mcs_limit ====*/
			halbb_set_reg(bb, 0xd18, BIT(9) | BIT(8), 1);
			halbb_set_reg(bb, 0xd18, BIT(22) | BIT(21), 1);
			/*==== Rx HE n_user_max / tb_max_nss ====*/
			halbb_set_reg(bb, 0xd80, 0x3fc0, HE_N_USER_MAX_8852C);
			halbb_set_reg(bb, 0xd80, BIT(16) | BIT(15) | BIT(14),
				      1);
			halbb_set_reg(bb, 0xd80, BIT(25) | BIT(24) | BIT(23),
				      1);
		}
	}

	/* === [BTG setting] === */
	if ((bb->hal_com->band[0].cur_chandef.band == BAND_ON_24G) && ((rx_path == RF_PATH_B) || (rx_path == RF_PATH_AB)))
		halbb_ctrl_btg_8852c(bb, true);
	else
		halbb_ctrl_btg_8852c(bb, false);

	/*==== [TSSI reset] ====*/
	if (rx_path == RF_PATH_A) {
		halbb_set_reg(bb, 0x58dc, BIT(31) | BIT(30), 0x1);
		halbb_set_reg(bb, 0x58dc, BIT(31) | BIT(30), 0x3);
	} else {
		halbb_set_reg(bb, 0x78dc, BIT(31) | BIT(30), 0x1);
		halbb_set_reg(bb, 0x78dc, BIT(31) | BIT(30), 0x3);
	}

	BB_DBG(bb, DBG_PHY_CONFIG, "[Rx Success]RX_en=%x\n", rx_path);
	return true;

}

bool halbb_ctrl_tx_path_pmac_8852c(struct bb_info *bb, enum rf_path tx_path,
				   bool dbcc_en)
{
	BB_DBG(bb, DBG_PHY_CONFIG, "<====== %s ======>\n", __func__);
	/*P-MAC TX path control in MP driver*/
	/*No effect in True-MAC used by normal driver*/

	bb->tx_path = tx_path;

	/*==== [P-MAC] Path & Path_map Enable ====*/
	if (!dbcc_en) {
		halbb_set_reg_cmn(bb, 0x09a4, 0x1c, 0x7, HW_PHY_0);
		if (tx_path == RF_PATH_A) {
			halbb_set_reg(bb, 0x458C, 0xf0000000, 0x1);
			halbb_set_reg(bb, 0x45B4, 0x1e0000, 0x0);
			// TSSI reset
			halbb_set_reg(bb, 0x58dc, BIT(31) | BIT(30), 0x1);
			halbb_set_reg(bb, 0x58dc, BIT(31) | BIT(30), 0x3);
		} else if (tx_path == RF_PATH_B) {
			halbb_set_reg(bb, 0x458C, 0xf0000000, 0x2);
			halbb_set_reg(bb, 0x45B4, 0x1e0000, 0x0);
			// TSSI reset
			halbb_set_reg(bb, 0x78dc, BIT(31) | BIT(30), 0x1);
			halbb_set_reg(bb, 0x78dc, BIT(31) | BIT(30), 0x3);
		} else if (tx_path == RF_PATH_AB) {
			halbb_set_reg(bb, 0x458C, 0xf0000000, 0x3);
			halbb_set_reg(bb, 0x45B4, 0x1e0000, 0x4);
			// TSSI reset
			halbb_set_reg(bb, 0x58dc, BIT(31) | BIT(30), 0x1);
			halbb_set_reg(bb, 0x58dc, BIT(31) | BIT(30), 0x3);
			halbb_set_reg(bb, 0x78dc, BIT(31) | BIT(30), 0x1);
			halbb_set_reg(bb, 0x78dc, BIT(31) | BIT(30), 0x3);
		} else {
			BB_WARNING("Invalid Tx Path\n");
			return false;
		}
	} else {
		halbb_set_reg_cmn(bb, 0x09a4, 0x1c, 0x7, HW_PHY_0);
		halbb_set_reg_cmn(bb, 0x458C, 0xf0000000, 0x1, HW_PHY_0);
		halbb_set_reg_cmn(bb, 0x45B4, 0x1e0000, 0x0, HW_PHY_0);
		halbb_set_reg_cmn(bb, 0x09a4, 0x1c, 0x7, HW_PHY_1);
		halbb_set_reg_cmn(bb, 0x458C, 0xf0000000, 0x2, HW_PHY_1);
		halbb_set_reg_cmn(bb, 0x45B4, 0x1e0000, 0x4, HW_PHY_1);
		// TSSI reset
		halbb_set_reg(bb, 0x58dc, BIT(31) | BIT(30), 0x1);
		halbb_set_reg(bb, 0x58dc, BIT(31) | BIT(30), 0x3);
		halbb_set_reg(bb, 0x78dc, BIT(31) | BIT(30), 0x1);
		halbb_set_reg(bb, 0x78dc, BIT(31) | BIT(30), 0x3);
	}

	return true;
}

bool halbb_ctrl_tx_path_tmac_8852c(struct bb_info *bb, enum rf_path tx_path_1sts,
				   bool dbcc_en)
{
	// Need to Add MP flag for Tx_path API since Normal Drv will also call this function
	// ==== [T-MAC] Path & Path_map Enable ==== //
	u32 path_com_cr_array[24] = {0xD800, 0x00000000,
				     0xD804, 0x00000000,
				     0xD808, 0x00000000,
				     0xD80C, 0x49249249,
				     0xD810, 0x1C9C9C49,
				     0xD814, 0x39393939,
				     0xD818, 0x39393939,
				     0xD81C, 0x39393939,
				     0xD820, 0x00003939,
				     0xD824, 0x000007C0,
				     0xD828, 0xE0000000,
				     0xD82C, 0x00000000};

	u32 base;
	u32 offset;
	u8 cr_size = sizeof(path_com_cr_array)/sizeof(u32);
	u8 i = 0;
	struct rtw_hal_com_t *hal_com = bb->hal_com;

	base = 0xDC00;
	for (offset = 0; offset <= 4 * 127; offset += 4) {
		rtw_hal_mac_set_pwr_reg(hal_com, (u8)HW_BAND_0, base + offset, 0x0);
	}

	if (!hal_com->dbcc_en) {
		// 0x9a4[2] tx path_en source: 0 for tmac, 1 for pmac
		// 0x9a4[3] tx path_map_a source: 0 for tmac, 1 for pmac
		// 0x9a4[4] tx path_map_b source: 0 for tmac, 1 for pmac
		halbb_set_reg_cmn(bb, 0x09a4, 0x1c, 0x0, HW_PHY_0); /*T-MAC TX*/

		if (tx_path_1sts == RF_PATH_A) {
			// path_a enable, path_a connect ifft0
			path_com_cr_array[1] = 0x08889880; // 0xD800
			path_com_cr_array[3] = 0x13111111; // 0xD804
			path_com_cr_array[5] = 0x01209313; // 0xD808
			path_com_cr_array[15] = 0x39393939; // 0xD81C
			path_com_cr_array[17] = 0x00003939; // 0xD820
		} else if (tx_path_1sts == RF_PATH_B) {
			// path_b enable, path_b connect ifft0
			path_com_cr_array[1] = 0x11111900; // 0xD800
			path_com_cr_array[3] = 0x23222222; // 0xD804
			path_com_cr_array[5] = 0x01209323; // 0xD808
			path_com_cr_array[15] = 0x39383939; // 0xD81C
			path_com_cr_array[17] = 0x00003938; // 0xD820
		} else if (tx_path_1sts == RF_PATH_AB) {
			// path_a enable, path_a connect ifft0, path_b enable, path_b connect ifft1
			path_com_cr_array[1] = 0x19999980; // 0xD800
			path_com_cr_array[3] = 0x33333333; // 0xD804
			path_com_cr_array[5] = 0x01209333; // 0xD808
			path_com_cr_array[15] = 0x39393939; // 0xD81C
			path_com_cr_array[17] = 0x00003939; // 0xD820
		} else {
			BB_WARNING("[Invalid Tx Path]1sts Tx Path: %d\n", tx_path_1sts);
		}

		for (i = 0; i < cr_size; i += 2) {
			BB_DBG(bb, DBG_INIT, "0x%x = 0x%x\n", path_com_cr_array[i], path_com_cr_array[i+1]);
			rtw_hal_mac_set_pwr_reg(hal_com, (u8)HW_BAND_0, path_com_cr_array[i], path_com_cr_array[i+1]);
		}

		// TSSI reset
		if (tx_path_1sts == RF_PATH_A) {
			halbb_set_reg(bb, 0x58dc, BIT(31) | BIT(30), 0x1);
			halbb_set_reg(bb, 0x58dc, BIT(31) | BIT(30), 0x3);
		} else if (tx_path_1sts == RF_PATH_B) {
			halbb_set_reg(bb, 0x78dc, BIT(31) | BIT(30), 0x1);
			halbb_set_reg(bb, 0x78dc, BIT(31) | BIT(30), 0x3);
		} else if (tx_path_1sts == RF_PATH_AB) {
			halbb_set_reg(bb, 0x58dc, BIT(31) | BIT(30), 0x1);
			halbb_set_reg(bb, 0x58dc, BIT(31) | BIT(30), 0x3);
			halbb_set_reg(bb, 0x78dc, BIT(31) | BIT(30), 0x1);
			halbb_set_reg(bb, 0x78dc, BIT(31) | BIT(30), 0x3);
		}
	} else {
		// MAC0 and PHY0 ===============================================

		// 0x9a4[2] tx path_en source: 0 for tmac, 1 for pmac
		// 0x9a4[3] tx path_map_a source: 0 for tmac, 1 for pmac
		// 0x9a4[4] tx path_map_b source: 0 for tmac, 1 for pmac
		halbb_set_reg_cmn(bb, 0x09a4, 0x1c, 0x0, HW_PHY_0);

		// path_a enable, path_a connect ifft0
		path_com_cr_array[1] = 0x08888880;  // 0xD800
		path_com_cr_array[3] = 0x11111111;  // 0xD804
		path_com_cr_array[5] = 0x00009111;  // 0xD808
		path_com_cr_array[7] = 0x09249249;  // 0xD80C
		path_com_cr_array[9] = 0x00000049;  // 0xD810
		path_com_cr_array[11] = 0x00000000; // 0xD814
		path_com_cr_array[13] = 0x00000000; // 0xD818
		path_com_cr_array[15] = 0x00000000; // 0xD81C
		path_com_cr_array[17] = 0x00000000; // 0xD820
		path_com_cr_array[19] = 0x00300000; // 0xD824
		path_com_cr_array[21] = 0xE0000000; // 0xD828
		path_com_cr_array[23] = 0x00000000; // 0xD82C

		for (i = 0; i < cr_size; i += 2) {
			BB_DBG(bb, DBG_INIT, "0x%x = 0x%x\n", path_com_cr_array[i], path_com_cr_array[i+1]);
			rtw_hal_mac_set_pwr_reg(hal_com, (u8)HW_BAND_0, path_com_cr_array[i], path_com_cr_array[i+1]);
		}

		// MAC1 and PHY1 ===============================================

		// 0x9a4[2] tx path_en source: 0 for tmac, 1 for pmac
		// 0x9a4[3] tx path_map_a source: 0 for tmac, 1 for pmac
		// 0x9a4[4] tx path_map_b source: 0 for tmac, 1 for pmac
		halbb_set_reg_cmn(bb, 0x09a4, 0x1c, 0x0, HW_PHY_1);

		// path_b enable, path_b connect ifft0
		path_com_cr_array[1] = 0x11111100;  // 0xD800
		path_com_cr_array[3] = 0x22222222;  // 0xD804
		path_com_cr_array[5] = 0x00009222;  // 0xD808
		path_com_cr_array[7] = 0x09249249;  // 0xD80C
		path_com_cr_array[9] = 0x00000049;  // 0xD810
		path_com_cr_array[11] = 0x00000000; // 0xD814
		path_com_cr_array[13] = 0x00000000; // 0xD818
		path_com_cr_array[15] = 0x00000000; // 0xD81C
		path_com_cr_array[17] = 0x00000000; // 0xD820
		path_com_cr_array[19] = 0x00300000; // 0xD824
		path_com_cr_array[21] = 0xE0000000; // 0xD828
		path_com_cr_array[23] = 0x00000000; // 0xD82C

		for (i = 0; i < cr_size; i += 2) {
			BB_DBG(bb, DBG_INIT, "0x%x = 0x%x\n", path_com_cr_array[i], path_com_cr_array[i+1]);
			rtw_hal_mac_set_pwr_reg(hal_com, (u8)HW_BAND_1, path_com_cr_array[i], path_com_cr_array[i+1]);
		}

		// TSSI reset
		halbb_set_reg(bb, 0x58dc, BIT(31) | BIT(30), 0x1);
		halbb_set_reg(bb, 0x58dc, BIT(31) | BIT(30), 0x3);
		halbb_set_reg(bb, 0x78dc, BIT(31) | BIT(30), 0x1);
		halbb_set_reg(bb, 0x78dc, BIT(31) | BIT(30), 0x3);
	}

	return true;
}

bool halbb_ctrl_tx_path_8852c(struct bb_info *bb, enum rf_path tx_path)
{
	bool rpt = false;

	BB_DBG(bb, DBG_PHY_CONFIG, "<====== %s ======>\n", __func__);

	if (phl_is_mp_mode(bb->phl_com) && !phl_mp_is_tmac_mode(bb->phl_com))
		rpt = halbb_ctrl_tx_path_pmac_8852c(bb, tx_path, bb->hal_com->dbcc_en);
	else
		rpt = halbb_ctrl_tx_path_tmac_8852c(bb, tx_path, bb->hal_com->dbcc_en);

	return rpt;
}

void halbb_tssi_ctrl_set_dbw_table_8852c(struct bb_info *bb)
{
	struct bb_pwr_ctrl_info *pwr_ctrl_i = &bb->bb_pwr_ctrl_i;
	struct bb_tssi_info *tssi_i = &pwr_ctrl_i->tssi_i;
	u32 tssi_dbw_table[4][15] = {
		/*20, 40_0, 40_1, 80_0, 80_1, 80_2, 80_3, 160_0, 160_1, 160_2, 160_3, 160_4, 160_5, 160_6, 160_7*/
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
	};

	halbb_mem_cpy(bb, tssi_i->tssi_dbw_table, tssi_dbw_table, sizeof(tssi_dbw_table));
}

u32 halbb_phy0_to_phy1_ofst_8852c(struct bb_info *bb, u32 addr)
{
	u32 phy_page = addr >> 8;
	u32 ofst = 0;

	if ((phy_page >= 0x40) && (phy_page <= 0x4f))
		return 0x2000;

	switch (phy_page) {
	case 0x6:
	case 0x7:
	case 0x8:
	case 0x9:
	case 0xa:
	case 0xb:
	case 0xc:
	case 0xd:
	case 0x19:
	case 0x1a:
	case 0x1b:
		ofst = 0x2000;
		break;
	default:
		ofst = 0; /*phy1 or warning case*/
		break;
	}

	return ofst;
}

void halbb_gpio_ctrl_dump_8852c(struct bb_info *bb)
{
	u32 rfe_cr_table[] = {0x0334, 0x0338, 0x033c,
			      0x5868, 0x5894, 0x7868, 0x7894,
			      0x5890, 0x7890, 0x5880, 0x5884, 0x7880, 0x7884};
	u8 i = 0;
	u8 rfe_cr_len = sizeof(rfe_cr_table) / sizeof(u32);
	u32 cr_tmp, val;
	BB_DBG(bb, DBG_DBG_API, "[%s] ==================> \n", __func__);
	for (i = 0; i < rfe_cr_len; i++) {
		cr_tmp = rfe_cr_table[i];
		val = halbb_get_reg(bb, cr_tmp, MASKDWORD);
		BB_DBG(bb, DBG_DBG_API, "Reg[0x%04x] = 0x%08x\n", cr_tmp, val);
	}
}

void halbb_gpio_rfm_8852c(struct bb_info *bb, enum bb_path path,
			  enum bb_rfe_src_sel src, bool dis_tx_gnt_wl,
			  bool active_tx_opt, bool act_bt_en, u8 rfm_output_val)
{
	u32 path_cr_base = 0;
	u32 cr_tmp = 0;
	u32 val_tmp = 0;
	u32 mask = 0xff, mask_ofst = 0;

	BB_DBG(bb, DBG_DBG_API,
	       "[src:%d]{dis_tx_gnt_wl:%d, active_tx_opt:%d, act_bt_en:%d] = 0x%x\n",
	       src, dis_tx_gnt_wl, active_tx_opt, act_bt_en, rfm_output_val);

	if (src == PAPE_RFM)
		mask_ofst = 0;
	else if (src == TRSW_RFM)
		mask_ofst = 8;
	else if (src == LNAON_RFM)
		mask_ofst = 16;
	else
		return; /*invalid case*/

	if (path == BB_PATH_A) {
		path_cr_base = 0x5800;
	} else {
		path_cr_base = 0x7800;
	}

	cr_tmp = path_cr_base | 0x94;
	val_tmp = (u8)dis_tx_gnt_wl << 7 | (u8)active_tx_opt << 6 |
		  (u8)act_bt_en << 5 | rfm_output_val;
	mask = 0xff << mask_ofst;

	halbb_set_reg(bb, cr_tmp, mask, val_tmp);

	BB_DBG(bb, DBG_DBG_API, "0x%x[0x%x]=%d\n",cr_tmp, mask, val_tmp);
}

void halbb_gpio_trsw_table_8852c(struct bb_info *bb, enum bb_path path,
				 bool tx_path_en, bool trsw_tx,
				 bool trsw_rx, bool trsw, bool trsw_b)
{
	u32 path_cr_base = 0;
	u32 cr_tmp = 0;
	u32 val_tmp = 0;
	u32 mask_ofst = 16;

	BB_DBG(bb, DBG_DBG_API,
	       "[path %d]{tx_path_en:%d, trsw_tx:%d, trsw_rx:%d] = {trsw:%d, trsw_b:%d}\n",
	       path, tx_path_en, trsw_tx, trsw_rx, trsw, trsw_b);

	if (path == BB_PATH_A) {
		path_cr_base = 0x5800;
	} else {
		path_cr_base = 0x7800;
	}

	cr_tmp = path_cr_base | 0x68;
	mask_ofst += ((u8)tx_path_en << 2 | (u8)trsw_tx << 1 | (u8)trsw_rx) << 1;
	val_tmp = (u8)trsw << 1 | (u8)trsw_b;

	halbb_set_reg(bb, cr_tmp, 0x3 << mask_ofst, val_tmp);

	BB_DBG(bb, DBG_DBG_API, "0x%x[0x%x]=%d\n",cr_tmp, mask_ofst, val_tmp);
}

void halbb_gpio_setting_8852c(struct bb_info *bb, u8 gpio_idx,
			      enum bb_path path, bool inv,
			      enum bb_rfe_src_sel src)
{
	u8 path_sel = 0;
	u32 path_cr_base = 0;
	u32 cr_tmp = 0;
	u32 mask_tmp = 0;

	BB_DBG(bb, DBG_DBG_API, "%s\n", __func__);
	BB_DBG(bb, DBG_DBG_API, "gpio_idx[%d]:{Path=%d}{inv=%d}{src=%d}\n",
	       gpio_idx, path, inv, src);

	if (gpio_idx > 31 || path >= BB_PATH_C) {
		BB_WARNING("Wrong gpio_idx=%d,path=%d}\n", gpio_idx, path);
		return;
	}

	if (path == BB_PATH_A) {
		path_sel = 0;
		path_cr_base = 0x5800;
	} else {
		path_sel = 1;
		path_cr_base = 0x7800;
	}

	/*[Path_mux_sel]*/
	if (gpio_idx <= 15) {
		cr_tmp = 0x33c;
		mask_tmp = ((u32)0x3) << (gpio_idx << 1);
	} else {
		cr_tmp = 0x340;
		mask_tmp = ((u32)0x3) << ((gpio_idx - 16) << 1);
	}

	halbb_set_reg(bb, cr_tmp, mask_tmp, path_sel);
	BB_DBG(bb, DBG_DBG_API, "Path: 0x%x[0x%x]=%d\n",
	       cr_tmp, mask_tmp, path_sel);

	/*[inv]*/
	halbb_set_reg(bb, (path_cr_base | 0x90), BIT(gpio_idx), (u32)inv);
	BB_DBG(bb, DBG_DBG_API, "Inv: 0x%x[0x%x]=%d\n",
	       (path_cr_base | 0x90), (u32)BIT(gpio_idx), inv);

	/*[Output Source Signal]*/
	cr_tmp = path_cr_base | 0x80 | ((gpio_idx >> 1) & 0xfc);
	mask_tmp = ((u32)0xf) << ((gpio_idx % 8) << 2);
	halbb_set_reg(bb, cr_tmp, mask_tmp, (u32)src);
	BB_DBG(bb, DBG_DBG_API, "src: 0x%x[0x%x]=%d\n", cr_tmp, mask_tmp, src);
}

void halbb_gpio_setting_all_8852c(struct bb_info *bb, u8 rfe_idx)
{

	if (rfe_idx == 51) {
		halbb_gpio_setting_8852c(bb, 10, BB_PATH_A, true, TRSW_RFM);
		halbb_gpio_setting_8852c(bb, 7, BB_PATH_A, false, LNAON_RFM);
		halbb_gpio_setting_8852c(bb, 6, BB_PATH_A, false, PAPE_RFM);

		halbb_gpio_setting_8852c(bb, 1, BB_PATH_B, true, TRSW_RFM);
		halbb_gpio_setting_8852c(bb, 2, BB_PATH_B, false, LNAON_RFM);
		halbb_gpio_setting_8852c(bb, 3, BB_PATH_B, false, PAPE_RFM);

	} else if (rfe_idx == 52) {
		halbb_gpio_setting_8852c(bb, 10, BB_PATH_A, true, TRSW_RFM);
		halbb_gpio_setting_8852c(bb, 7, BB_PATH_A, false, LNAON_RFM);
		halbb_gpio_setting_8852c(bb, 6, BB_PATH_A, false, PAPE_RFM);

		halbb_gpio_setting_8852c(bb, 1, BB_PATH_B, true, TRSW_RFM);
		halbb_gpio_setting_8852c(bb, 2, BB_PATH_B, false, LNAON_RFM);
		halbb_gpio_setting_8852c(bb, 3, BB_PATH_B, false, PAPE_RFM);

	} else if (rfe_idx == 63) {
		halbb_gpio_setting_8852c(bb, 10, BB_PATH_A, true, TRSW_RFM);
		halbb_gpio_setting_8852c(bb, 7, BB_PATH_A, true, LNAON_RFM);
		halbb_gpio_setting_8852c(bb, 6, BB_PATH_A, false, PAPE_RFM);

		halbb_gpio_setting_8852c(bb, 1, BB_PATH_B, true, TRSW_RFM);
		halbb_gpio_setting_8852c(bb, 2, BB_PATH_B, true, LNAON_RFM);
		halbb_gpio_setting_8852c(bb, 3, BB_PATH_B, false, PAPE_RFM);

	} else if (rfe_idx == 64) {
		halbb_gpio_setting_8852c(bb, 10, BB_PATH_A, true, TRSW_RFM);
		halbb_gpio_setting_8852c(bb, 7, BB_PATH_A, true, LNAON_RFM);
		halbb_gpio_setting_8852c(bb, 6, BB_PATH_A, false, PAPE_RFM);

		halbb_gpio_setting_8852c(bb, 1, BB_PATH_B, true, TRSW_RFM);
		halbb_gpio_setting_8852c(bb, 2, BB_PATH_B, true, LNAON_RFM);
		halbb_gpio_setting_8852c(bb, 3, BB_PATH_B, false, PAPE_RFM);
	} else {
		BB_DBG(bb, DBG_DBG_API, "Not Support RFE=%d\n", rfe_idx);
	}

}

void halbb_gpio_setting_init_8852c(struct bb_info *bb)
{
	u32 path_cr_base = 0;
	u8 i = 0;
	struct rtw_phl_com_t *phl = bb->phl_com;
	struct dev_cap_t *dev = &phl->dev_cap;
	u8 rfe_idx = dev->rfe_type;

	/*[Delay]*/
	for (i = 0; i < 2; i++) {
		if (i == 0) {
			path_cr_base = 0x5800;
		} else {
			path_cr_base = 0x7800;
		}
		halbb_set_reg(bb, (path_cr_base | 0x68), BIT(1), 1); /*	r_tx_ant_sel*/
		halbb_set_reg(bb, (path_cr_base | 0x68), BIT(2), 0);
		halbb_set_reg(bb, (path_cr_base | 0x68), 0xe0, 0);/*convert to TRSW look-up-table*/
		halbb_set_reg(bb, (path_cr_base | 0x80), MASKDWORD, 0x77777777);
		halbb_set_reg(bb, (path_cr_base | 0x84), MASKDWORD, 0x77777777);
		halbb_set_reg(bb, (path_cr_base | 0x94), BIT(24), 1); /* TRSW from rfm_TRSW instead of TRSW table*/
	}

	halbb_set_reg(bb, 0x334, MASKDWORD, 0xffffffff); /*output mode[31:0]*/
	halbb_set_reg(bb, 0x338, MASKDWORD, 0);	/*non dbg_gpio mode[31:0]*/
	halbb_set_reg(bb, 0x33c, MASKDWORD, 0);	/*path_sel[15:0]*/
	halbb_set_reg(bb, 0x340, MASKDWORD, 0);	/*path_sel[16:31]*/

	/*[TRSW Table]*/
	halbb_gpio_trsw_table_8852c(bb, BB_PATH_A, 0, 0, 0, 0, 1);
	halbb_gpio_trsw_table_8852c(bb, BB_PATH_A, 0, 0, 1, 1, 0);
	halbb_gpio_trsw_table_8852c(bb, BB_PATH_A, 0, 1, 0, 1, 0);
	halbb_gpio_trsw_table_8852c(bb, BB_PATH_A, 0, 1, 1, 1, 0);
	halbb_gpio_trsw_table_8852c(bb, BB_PATH_A, 1, 0, 0, 0, 1);
	halbb_gpio_trsw_table_8852c(bb, BB_PATH_A, 1, 0, 1, 1, 0);
	halbb_gpio_trsw_table_8852c(bb, BB_PATH_A, 1, 1, 0, 1, 0);
	halbb_gpio_trsw_table_8852c(bb, BB_PATH_A, 1, 1, 1, 1, 0);

	halbb_gpio_trsw_table_8852c(bb, BB_PATH_B, 0, 0, 0, 0, 1);
	halbb_gpio_trsw_table_8852c(bb, BB_PATH_B, 0, 0, 1, 1, 0);
	halbb_gpio_trsw_table_8852c(bb, BB_PATH_B, 0, 1, 0, 1, 0);
	halbb_gpio_trsw_table_8852c(bb, BB_PATH_B, 0, 1, 1, 1, 0);
	halbb_gpio_trsw_table_8852c(bb, BB_PATH_B, 1, 0, 0, 0, 1);
	halbb_gpio_trsw_table_8852c(bb, BB_PATH_B, 1, 0, 1, 1, 0);
	halbb_gpio_trsw_table_8852c(bb, BB_PATH_B, 1, 1, 0, 1, 0);
	halbb_gpio_trsw_table_8852c(bb, BB_PATH_B, 1, 1, 1, 1, 0);

	halbb_gpio_rfm_8852c(bb, BB_PATH_A, PAPE_RFM, 0, 0, 0, 0x4);
	halbb_gpio_rfm_8852c(bb, BB_PATH_A, TRSW_RFM, 0, 0, 0, 0x6);
	halbb_gpio_rfm_8852c(bb, BB_PATH_B, PAPE_RFM, 0, 0, 0, 0x4);
	halbb_gpio_rfm_8852c(bb, BB_PATH_B, TRSW_RFM, 0, 0, 0, 0x6);

	if (rfe_idx == 63 || rfe_idx == 64) {
		/* LNA inv=true, standby mode of rfm_LNAON = 0x1*/
		halbb_gpio_rfm_8852c(bb, BB_PATH_A, LNAON_RFM, 0, 0, 0, 0xA);
		halbb_gpio_rfm_8852c(bb, BB_PATH_B, LNAON_RFM, 0, 0, 0, 0xA);
	} else {
		/* LNA inv=false, standby mode of rfm_LNAON = 0x0*/
		halbb_gpio_rfm_8852c(bb, BB_PATH_A, LNAON_RFM, 0, 0, 0, 0x8);
		halbb_gpio_rfm_8852c(bb, BB_PATH_B, LNAON_RFM, 0, 0, 0, 0x8);
	}
}


void halbb_ctrl_rf_mode_8852c(struct bb_info *bb,  enum phl_rf_mode mode)
{
	if (mode == RF_MODE_STANDBY) {
		halbb_set_reg(bb, 0x12ac, 0xfffffff0, 0x1111111);
		halbb_set_reg(bb, 0x12b0, 0xfff, 0x111);
		halbb_set_reg(bb, 0x32ac, 0xfffffff0, 0x1111111);
		halbb_set_reg(bb, 0x32b0, 0xfff, 0x111);
	} else if (mode == RF_MODE_SHUTDOWN) {
		halbb_set_reg(bb, 0x12ac, 0xfffffff0, 0x0);
		halbb_set_reg(bb, 0x12b0, 0xfff, 0x0);
		halbb_set_reg(bb, 0x32ac, 0xfffffff0, 0x0);
		halbb_set_reg(bb, 0x32b0, 0xfff, 0x0);
	} else {
		halbb_set_reg(bb, 0x12ac, 0xfffffff0, 0x1233312);
		halbb_set_reg(bb, 0x12b0, 0xfff, 0x333);
		halbb_set_reg(bb, 0x32ac, 0xfffffff0, 0x1233312);
		halbb_set_reg(bb, 0x32b0, 0xfff, 0x333);
	}
	BB_DBG(bb, DBG_PHY_CONFIG, "[RF Mode] Mode = %d", mode);
}

u16 halbb_cfg_cmac_tx_ant_8852c(struct bb_info *bb, enum rf_path tx_path)
{
	// Return CMAC [OFST 20] Tx settings //
	/* [19:16] path_en[3:0] ||
	|| [21:20] map_a[1:0]   ||
	|| [23:22] map_b[1:0]   ||
	|| [25:24] map_c[1:0]   ||
	|| [27:26] map_d[1:0]   ||
	|| [28] ant_sel_a[0]    ||
	|| [29] ant_sel_b[0]    ||
	|| [30] ant_sel_c[0]    ||
	|| [31] ant_sel_d[0]    */
	u16 cmac_tx_info = 0;

	if (tx_path == RF_PATH_A) {
		cmac_tx_info = 0x1;
	} else if (tx_path == RF_PATH_B) {
		cmac_tx_info = 0x2;
	} else if (tx_path == RF_PATH_AB) {
		cmac_tx_info = 0x43;
	} else {
		cmac_tx_info = 0xffff;
		BB_WARNING("Invalid Tx Path: %d\n", tx_path);
	}
	return cmac_tx_info;

}

void halbb_ctrl_trx_path_8852c(struct bb_info *bb, enum rf_path tx_path,
			       u8 tx_nss, enum rf_path rx_path, u8 rx_nss)
{
	/*normal driver use only*/
	// Rx Config
	halbb_ctrl_rx_path_8852c(bb, rx_path, bb->hal_com->dbcc_en);

	if ((rx_nss > 2) || (tx_nss > 2)) {
		BB_WARNING("[Invalid Nss]Tx Nss: %d, Rx Nss: %d\n", tx_nss,
			   rx_nss);
		return;
	}
	if ((tx_nss == 2) && ((tx_path == RF_PATH_A) || (tx_path == RF_PATH_B))) {
		BB_WARNING("[Invalid Tx Nss and Path]Tx Nss: %d, Tx Path: %d\n", tx_nss, tx_path);
		return;
	}

	if (rx_nss == 1) {
		/*==== [PHY0] Rx HT nss_limit / mcs_limit ====*/
		halbb_set_reg(bb, 0xd18, BIT(9) | BIT(8), 0);
		halbb_set_reg(bb, 0xd18, BIT(22) | BIT(21), 0);
		/*==== [PHY0] Rx HE n_user_max / tb_max_nss ====*/
		halbb_set_reg(bb, 0xd80, BIT(16) | BIT(15) | BIT(14), 0);
		halbb_set_reg(bb, 0xd80, BIT(25) | BIT(24) | BIT(23), 0);
	} else {
		/*==== [PHY0] Rx HT nss_limit / mcs_limit ====*/
		halbb_set_reg(bb, 0xd18, BIT(9) | BIT(8), 1);
		halbb_set_reg(bb, 0xd18, BIT(22) | BIT(21), 1);
		/*==== [PHY0] Rx HE n_user_max / tb_max_nss ====*/
		halbb_set_reg(bb, 0xd80, BIT(16) | BIT(15) | BIT(14), 1);
		halbb_set_reg(bb, 0xd80, BIT(25) | BIT(24) | BIT(23), 1);
	}

	// Tx Config
	halbb_ctrl_tx_path_8852c(bb, tx_path);
}

void halbb_tssi_bb_reset_8852c(struct bb_info *bb)
{

}

#ifdef HALBB_DBCC_SUPPORT
void halbb_ctrl_dbcc_8852c(struct bb_info *bb, bool dbcc_en)
{
	bb->bb_cmn_hooker->bb_dbcc_en = dbcc_en;

	if (dbcc_en)
		halbb_set_reg(bb, 0x4970, BIT(0), 1);
	else
		halbb_set_reg(bb, 0x4970, BIT(0), 0);
	BB_DBG(bb, DBG_PHY_CONFIG, "[DBCC Enable = %d]\n", dbcc_en);
}

void halbb_cfg_dbcc_cck_phy_map_8852c(struct bb_info *bb, bool dbcc_en,
				      enum phl_phy_idx cck_phy_map)
{
	BB_DBG(bb, DBG_DBCC, "[%s] cck_phy_map=%d\n", __func__, cck_phy_map);

	bb->bb_cmn_hooker->cck_phy_map = cck_phy_map;

	if (dbcc_en) {
		if (cck_phy_map == HW_PHY_1)
			halbb_set_reg(bb, 0x4970, BIT(1), 1);
		else
			halbb_set_reg(bb, 0x4970, BIT(1), 0);
	} else {
		halbb_set_reg(bb, 0x4970, BIT(1), 0);
	}

	BB_DBG(bb, DBG_PHY_CONFIG, "[DBCC cfg] cck_phy_map = %d\n", cck_phy_map);
}

void halbb_cfg_dbcc_8852c(struct bb_info *bb, struct bb_dbcc_cfg_info *cfg)
{
	BB_DBG(bb, DBG_DBCC, "[%s] en=%d, cck_phy_map=%d\n", __func__,
	       cfg->dbcc_en, cfg->cck_phy_map);

	#ifdef HALBB_FW_OFLD_SUPPORT
	halbb_fwofld_bitmap_en(bb, true, FW_OFLD_DBCC_API);
	#endif
	// === DBCC En === //
	halbb_ctrl_dbcc_8852c(bb, cfg->dbcc_en);
	halbb_cfg_dbcc_cck_phy_map_8852c(bb, cfg->dbcc_en, cfg->cck_phy_map);
	halbb_ctrl_rx_path_8852c(bb, RF_PATH_AB, cfg->dbcc_en);
	halbb_ctrl_tx_path_tmac_8852c(bb, RF_PATH_AB, cfg->dbcc_en);

	#ifdef HALBB_FW_OFLD_SUPPORT
	halbb_fwofld_bitmap_en(bb, false, FW_OFLD_DBCC_API);
	#endif

	if (cfg->dbcc_en) {
		// === LO Select === //
		halbb_set_reg(bb, 0x35c, BIT(11) | BIT(10), 3);
		// === SYN 1 enable === //
		rtw_hal_rf_syn_config(bb->hal_com, 1, HW_PHY_0, RF_PATH_B, true); // Phy_idx don't care
	} else {
		// === LO Select === //
		halbb_set_reg(bb, 0x35c, BIT(11) | BIT(10), 1);
		// === SYN 1 disable === //
		rtw_hal_rf_syn_config(bb->hal_com, 1, HW_PHY_0, RF_PATH_B, false); // Phy_idx don't care
	}
	BB_DBG(bb, DBG_PHY_CONFIG, "[DBCC cfg] Status = %d\n", cfg->dbcc_en);
}
#endif

void halbb_ctrl_rx_cca_8852c(struct bb_info *bb, bool cca_en, enum phl_phy_idx phy_idx)
{
	bool band_2g;

	band_2g = (bb->hal_com->band[phy_idx].cur_chandef.band == BAND_ON_24G) ?
		  true : false;

	if (cca_en) {
		halbb_set_reg_cmn(bb, 0xc3c, BIT(9), 0, phy_idx);
		if (band_2g)
			halbb_ctrl_cck_en_8852c(bb, true, phy_idx, 2);
	} else {
		halbb_set_reg_cmn(bb, 0xc3c, BIT(9), 1, phy_idx);
		if (band_2g)
			halbb_ctrl_cck_en_8852c(bb, false, phy_idx, 2);
		// TSSI pause
		halbb_set_reg(bb, 0x5818, BIT(30), 1);
		halbb_set_reg(bb, 0x7818, BIT(30), 1);
		// BB reset
		halbb_bb_reset_en_8852c(bb, false, phy_idx);
		halbb_bb_reset_en_8852c(bb, true, phy_idx);
		// TSSI pause disable
		halbb_set_reg(bb, 0x5818, BIT(30), 0);
		halbb_set_reg(bb, 0x7818, BIT(30), 0);
		// Delay 1us
		halbb_delay_us(bb, 1);
	}
	BB_DBG(bb, DBG_PHY_CONFIG, "[Rx CCA] CCA_EN = %d\n", cca_en);
}



void halbb_ctrl_ofdm_en_8852c(struct bb_info *bb, bool ofdm_en,
			      enum phl_phy_idx phy_idx)
{
	if (ofdm_en)
		halbb_set_reg_cmn(bb, 0x700, BIT(4), 1, phy_idx);
	else
		halbb_set_reg_cmn(bb, 0x700, BIT(4), 0, phy_idx);

	BB_DBG(bb, DBG_PHY_CONFIG, "[OFDM Enable for PHY%d]\n", phy_idx);
}



// =================== [Power Module] =================== //
bool halbb_set_txpwr_dbm_8852c(struct bb_info *bb, s16 power_dbm,
			       enum phl_phy_idx phy_idx)
{
	bool tmp = false;

	power_dbm &= 0x1ff;
	halbb_set_reg_cmn(bb, 0x09a4, BIT(16), 1, phy_idx);
	halbb_set_reg_cmn(bb, 0x4594, 0x7fc00000, power_dbm, phy_idx);
	BB_DBG(bb, DBG_PHY_CONFIG,
	       "[Success] Set Tx pwr(dBm) for [PHY-%d] : %d\n", phy_idx,
	       power_dbm);
	tmp = true;
	return tmp;
}

s16 halbb_get_txpwr_dbm_8852c(struct bb_info *bb, enum phl_phy_idx phy_idx)
{
	u32 txpwr_dbm;
	s16 output;

	BB_DBG(bb, DBG_PHY_CONFIG, "<====== %s ======>\n", __func__);

	txpwr_dbm = halbb_get_reg_cmn(bb, 0x4594, 0x7fc00000, phy_idx);
	output = (s16)halbb_cnvrt_2_sign(txpwr_dbm, 9);
	BB_DBG(bb, DBG_PHY_CONFIG,
	       "[Success] Get Tx pwr(dBm) for [PHY-%d] : %d\n", phy_idx,
	       output);
	return output;
}

s16 halbb_get_txinfo_txpwr_dbm_8852c(struct bb_info *bb)
{
	u32 txpwr_dbm;
	s16 output;

	BB_DBG(bb, DBG_PHY_CONFIG, "<====== %s ======>\n", __func__);

	txpwr_dbm = halbb_get_reg(bb, 0x1804, 0x7FC0000);
	output = (s16)halbb_cnvrt_2_sign(txpwr_dbm, 9);
	BB_DBG(bb, DBG_PHY_CONFIG,
	       "[Success] Get TxInfo pwr(dBm) : %d\n", output);
	return output;
}

bool halbb_set_cck_txpwr_idx_8852c(struct bb_info *bb, u16 power_idx,
				   enum rf_path tx_path)
{
	u32 pwr_idx_addr[2] = {0x5808, 0x7808};

	/*==== Power index Check ====*/
	if ((power_idx & ~0x1ff) != 0) {
		BB_WARNING("Power Idx: %x\n", power_idx);
		return false;
	}
	/*==== Tx Path Check ====*/
	if (tx_path > RF_PATH_B || tx_path < RF_PATH_A) {
		BB_WARNING("Invalid Tx Path for CCK Txpwr_idx setting (52A)\n");
		return false;
	}
	halbb_set_reg(bb, pwr_idx_addr[tx_path], 0x3fe00, power_idx);

	BB_DBG(bb, DBG_PHY_CONFIG,
	       "[Success] [CCK] Set Tx pwr idx for [Path-%d] : %x\n",
	       tx_path, power_idx);
	return true;
}

u16 halbb_get_cck_txpwr_idx_8852c(struct bb_info *bb, enum rf_path tx_path)
{
	u16 cck_pwr_idx;
	u32 pwr_idx_addr[2] = {0x5808, 0x7808};

	BB_DBG(bb, DBG_PHY_CONFIG, "<====== %s ======>\n", __func__);

	cck_pwr_idx = (u16)halbb_get_reg(bb, pwr_idx_addr[tx_path], 0x3fe00);
	BB_DBG(bb, DBG_PHY_CONFIG,
	       "[Success] [CCK] Get Tx pwr idx for [Path-%d] : %x\n",
	       tx_path, cck_pwr_idx);
	return cck_pwr_idx;
}

s16 halbb_get_cck_ref_dbm_8852c(struct bb_info *bb, enum rf_path tx_path)
{
	u32 cck_ref_dbm;
	u32 pwr_ref_addr[2] = {0x5808, 0x7808};
	s16 output;

	BB_DBG(bb, DBG_PHY_CONFIG, "<====== %s ======>\n", __func__);

	cck_ref_dbm = halbb_get_reg(bb, pwr_ref_addr[tx_path], 0x1ff);
	output = (s16)halbb_cnvrt_2_sign(cck_ref_dbm, 9);
	BB_DBG(bb, DBG_PHY_CONFIG,
	       "[Success] [CCK] Get Tx ref pwr(dBm) for [Path-%d] : %d\n",
	       tx_path, output);
	return output;
}

bool halbb_set_ofdm_txpwr_idx_8852c(struct bb_info *bb, u16 power_idx,
				    enum rf_path tx_path)
{
	u32 pwr_idx_addr[2] = {0x5804, 0x7804};

	/*==== Power index Check ====*/
	if ((power_idx & ~0x1ff) != 0) {
		BB_WARNING("Power Idx: %x\n", power_idx);
		return false;
	}
	/*==== Tx Path Check ====*/
	if (tx_path > RF_PATH_B || tx_path < RF_PATH_A) {
		BB_WARNING("Invalid Tx Path for CCK Txpwr_idx setting (52A)\n");
		return false;
	}

	halbb_set_reg(bb, pwr_idx_addr[tx_path], 0x3fe00, power_idx);

	BB_DBG(bb, DBG_PHY_CONFIG,
	       "[Success] [OFDM] Set Tx pwr idx for [Path-%d] : %x\n",
	       tx_path, power_idx);
	return true;
}

u16 halbb_get_ofdm_txpwr_idx_8852c(struct bb_info *bb, enum rf_path tx_path)
{
	u16 ofdm_pwr_idx;
	u32 pwr_idx_addr[2] = {0x5804, 0x7804};

	BB_DBG(bb, DBG_PHY_CONFIG, "<====== %s ======>\n", __func__);

	ofdm_pwr_idx = (u16)halbb_get_reg(bb, pwr_idx_addr[tx_path], 0x3fe00);
	BB_DBG(bb, DBG_PHY_CONFIG,
	       "[Success] [OFDM] Get Tx pwr idx for [Path-%d] : %x\n",
	       tx_path, ofdm_pwr_idx);
	return ofdm_pwr_idx;
}

s16 halbb_get_ofdm_ref_dbm_8852c(struct bb_info *bb, enum rf_path tx_path)
{
	u32 ofdm_ref_dbm;
	u32 pwr_ref_addr[2] = {0x5804, 0x7804};
	s16 output;

	BB_DBG(bb, DBG_PHY_CONFIG, "<====== %s ======>\n", __func__);

	ofdm_ref_dbm = halbb_get_reg(bb, pwr_ref_addr[tx_path], 0x1ff);
	output = (s16)halbb_cnvrt_2_sign(ofdm_ref_dbm, 9);
	BB_DBG(bb, DBG_PHY_CONFIG,
	       "[Success] [OFDM] Get Tx ref pwr(dBm) for [Path-%d] : %d\n",
	       tx_path, output);
	return output;
}

void halbb_reset_bb_hw_cnt_8852c(struct bb_info *bb)
{

	/*@ Reset all counter*/
	halbb_set_reg_phy0_1(bb, 0x730, BIT(0), 1);
	halbb_set_reg_phy0_1(bb, 0x730, BIT(0), 0);

}

void halbb_backup_info_8852c(struct bb_info *bb, enum phl_phy_idx phy_idx)
{
	/*==== This Backup info is for RF TSSI calibration =====*/
	bb->bb_cmn_backup_i.cur_tx_path = (u8)halbb_get_reg_cmn(bb, 0x458c, 0xf0000000, phy_idx);
	bb->bb_cmn_backup_i.cur_rx_path = (u8)halbb_get_reg_cmn(bb, 0x4978, 0xf, phy_idx);
	bb->bb_cmn_backup_i.cur_tx_pwr = halbb_get_txpwr_dbm_8852c(bb, phy_idx);

	BB_DBG(bb, DBG_PHY_CONFIG, "[Backup Info] [PHY%d] Tx path = %x\n", phy_idx, bb->bb_cmn_backup_i.cur_tx_path);
	BB_DBG(bb, DBG_PHY_CONFIG, "[Backup Info] [PHY%d] Tx pwr = %x\n", phy_idx, (u16)bb->bb_cmn_backup_i.cur_tx_pwr);
}

void halbb_restore_info_8852c(struct bb_info *bb, enum phl_phy_idx phy_idx)
{
	u8 tx_nss;
	enum rf_path rx_path;
	enum rf_path tx_path;

	if (bb->bb_cmn_backup_i.cur_rx_path == 1)
		rx_path = RF_PATH_A;
	else if (bb->bb_cmn_backup_i.cur_rx_path == 2)
		rx_path = RF_PATH_B;
	else
		rx_path = RF_PATH_AB;

	if (bb->bb_cmn_backup_i.cur_tx_path == 1)
		tx_path = RF_PATH_A;
	else if (bb->bb_cmn_backup_i.cur_tx_path == 2)
		tx_path = RF_PATH_B;
	else
		tx_path = RF_PATH_AB;
	/*==== This Restore info is for RF TSSI calibration =====*/
	halbb_set_txpwr_dbm_8852c(bb, bb->bb_cmn_backup_i.cur_tx_pwr, phy_idx);

	tx_nss = (bb->bb_cmn_backup_i.cur_tx_path == 0x3) ? 2 : 1;

	halbb_ctrl_tx_path_8852c(bb, tx_path);
	halbb_ctrl_rx_path_8852c(bb, rx_path, bb->hal_com->dbcc_en);

	BB_DBG(bb, DBG_PHY_CONFIG,
	       "[Restore Info] [PHY%d] Tx path=%x, Rx path=%x, is_mp=%d\n",
	       phy_idx, bb->bb_cmn_backup_i.cur_tx_path,
	       bb->bb_cmn_backup_i.cur_rx_path, phl_is_mp_mode(bb->phl_com));
}

void halbb_lbk_comm_8852c(struct bb_info *bb, bool lbk_en, bool is_dgt_lbk,
			  enum rf_path tx_path)
{
	if (!lbk_en) {
		halbb_set_reg(bb, 0x20ec, 0x7, 0);
		// AFE exit dbg mode
		halbb_set_reg(bb, 0x12b8, BIT(30), 0);
		halbb_set_reg(bb, 0x32b8, BIT(30), 0);
		//gothrough_trx_iqkdpk enable
		halbb_set_reg(bb, 0x5864, BIT(28) | BIT(27), 0x3);
		halbb_set_reg(bb, 0x7864, BIT(28) | BIT(27), 0x3);
		// Enable POP
		halbb_set_reg(bb, 0x4798, BIT(8), 1);
		halbb_set_reg(bb, 0x6798, BIT(8), 1);
		BB_DBG(bb, DBG_PHY_CONFIG, "[LBK] Disable\n");
	} else {
		//gothrough_trx_iqkdpk disable
		halbb_set_reg(bb, 0x5864, BIT(28) | BIT(27), 0);
		halbb_set_reg(bb, 0x7864, BIT(28) | BIT(27), 0);
		// Disable POP
		halbb_set_reg(bb, 0x4798, BIT(8), 0);
		halbb_set_reg(bb, 0x6798, BIT(8), 0);
		if (!is_dgt_lbk) {
			// AFE enter dbg mode for AFE rst
			halbb_set_reg(bb, 0x12b8, BIT(30), 1);
			halbb_set_reg(bb, 0x32b8, BIT(30), 1);
			halbb_set_reg(bb, 0x30c, 0xff000000, 0xff); //ADC reset
			halbb_set_reg(bb, 0x30c, 0xff000000, 0xf3);
			halbb_set_reg(bb, 0x32c, 0xffff0000, 0x30); //filter reset
			halbb_set_reg(bb, 0x32c, 0xffff0000, 0x70);
			BB_DBG(bb, DBG_PHY_CONFIG, "[LBK] AFE LBK setting!\n");
		} else {
			// AFE exit dbg mode for BB rst
			halbb_set_reg(bb, 0x12b8, BIT(30), 0);
			halbb_set_reg(bb, 0x32b8, BIT(30), 0);
			BB_DBG(bb, DBG_PHY_CONFIG, "[LBK] BB LBK setting!\n");
		}
	}
}

bool halbb_cfg_lbk_8852c(struct bb_info *bb, bool lbk_en, bool is_dgt_lbk,
			 enum rf_path tx_path, enum rf_path rx_path,
			 enum channel_width bw, enum phl_phy_idx phy_idx)
{
	u8 lbk_path_tmp = 0;

	BB_DBG(bb, DBG_PHY_CONFIG, "<====== %s ======>\n", __func__);

	// Error handling
	if ((tx_path == rx_path) || (tx_path > RF_PATH_B) || (rx_path > RF_PATH_B))
		return false;

	halbb_lbk_comm_8852c(bb, lbk_en, is_dgt_lbk, tx_path);

	if (!lbk_en)
		return true;
	/*
	[r_lbk_sel]
	4 :  BB pathB Tx to pathA Rx
	5 :  BB pathA Tx to pathB Rx
	6 :  AFE pathB Tx to pathA Rx
	7 :  AFE pathA Tx to pathB Rx
	*/
	if (tx_path == RF_PATH_A) {
		lbk_path_tmp = is_dgt_lbk ? 5 : 7;
		halbb_set_reg(bb, 0x20ec, 0x7, lbk_path_tmp);
		BB_DBG(bb, DBG_PHY_CONFIG, "[LBK] PathA Tx to PathB Rx\n");
	} else {
		lbk_path_tmp = is_dgt_lbk ? 4 : 6;
		halbb_set_reg(bb, 0x20ec, 0x7, lbk_path_tmp);
		BB_DBG(bb, DBG_PHY_CONFIG, "[LBK] PathB Tx to PathA Rx\n");
	}

	return true;
}

bool halbb_cfg_lbk_cck_8852c(struct bb_info *bb, bool lbk_en, bool is_dgt_lbk,
			     enum rf_path tx_path, enum rf_path rx_path,
			     enum channel_width bw, enum phl_phy_idx phy_idx)
{
	u8 lbk_path_tmp = 0;

	BB_DBG(bb, DBG_PHY_CONFIG, "<====== %s ======>\n", __func__);

	// Error handling
	if ((tx_path != rx_path) || (tx_path > RF_PATH_B) || (rx_path > RF_PATH_B))
		return false;

	halbb_lbk_comm_8852c(bb, lbk_en, is_dgt_lbk, tx_path);

	if (!lbk_en) {
		// Enable OFDM CCA
		halbb_set_reg_cmn(bb, 0xc3c, BIT(9), 0, phy_idx);
		return true;
	}
	/*
	[r_lbk_sel]
	1 :  BB pathA Tx to pathA Rx / pathB Tx to pathB Rx
	*/
	halbb_set_reg(bb, 0x20ec, 0x7, 0x1);

	// Disable OFDM CCA
	halbb_set_reg_cmn(bb, 0xc3c, BIT(9), 1, phy_idx);

	return true;
}

bool halbb_set_txsc_8852c(struct bb_info *bb, u8 txsc, enum phl_phy_idx phy_idx)
{
	/*==== txsc Check ====*/
	if ((txsc & ~0xf) != 0) {
		BB_WARNING("TXSC: %x\n", txsc);
		return false;
	}
	halbb_set_reg_cmn(bb, 0x45ac, 0x7800000, txsc, phy_idx);
	BB_DBG(bb, DBG_PHY_CONFIG,
	       "[Success] [P-MAC] TXSC for [PHY-%d] : %x\n", phy_idx,
	       txsc);
	return true;
}

void halbb_ctrl_btc_preagc_8852c(struct bb_info *bb, bool bt_en)
{
	BB_DBG(bb, DBG_PHY_CONFIG, "<====== %s ======>\n", __func__);

	if (bt_en) {
		// DFIR Corner
		halbb_set_reg(bb, 0x4c00, BIT(1) | BIT(0), 0x3);
		halbb_set_reg(bb, 0x4cc4, BIT(1) | BIT(0), 0x3);

		// BT trakcing always on
		halbb_set_reg(bb, 0x4ad4, MASKDWORD, 0xf);
		halbb_set_reg(bb, 0x4ae0, MASKDWORD, 0xf);

		// LNA6_OP1dB
		halbb_set_reg(bb, 0x4688, MASKBYTE3, 0x80);
		halbb_set_reg(bb, 0x476C, MASKBYTE3, 0x80);

		// LNA6_TIA0_1_OP1dB
		halbb_set_reg(bb, 0x4694, MASKBYTE0, 0x80);
		halbb_set_reg(bb, 0x4694, MASKBYTE1, 0x80);
		halbb_set_reg(bb, 0x4778, MASKBYTE0, 0x80);
		halbb_set_reg(bb, 0x4778, MASKBYTE1, 0x80);

		// LNA, TIA, ADC backoff at BT TX
		halbb_set_reg(bb, 0x4ae4, 0xffffff, 0x780D1E);
		halbb_set_reg(bb, 0x4aec, 0xffffff, 0x780D1E);

		// IBADC backoff
		halbb_set_reg(bb, 0x469c, 0xfc000000, 0x34);
		halbb_set_reg(bb, 0x49f0, 0xfc000000, 0x34);

	} else {
		// DFIR Corner
		halbb_set_reg(bb, 0x4c00, BIT(1) | BIT(0), 0x0);
		halbb_set_reg(bb, 0x4cc4, BIT(1) | BIT(0), 0x0);

		// BT trakcing always on
		halbb_set_reg(bb, 0x4ad4, MASKDWORD, 0x60);
		halbb_set_reg(bb, 0x4ae0, MASKDWORD, 0x60);

		// LNA6_OP1dB
		halbb_set_reg(bb, 0x4688, MASKBYTE3, 0x1a);
		halbb_set_reg(bb, 0x476C, MASKBYTE3, 0x20);

		// LNA6_TIA0_1_OP1dB
		halbb_set_reg(bb, 0x4694, MASKBYTE0, 0x2a);
		halbb_set_reg(bb, 0x4694, MASKBYTE1, 0x2a);
		halbb_set_reg(bb, 0x4778, MASKBYTE0, 0x30);
		halbb_set_reg(bb, 0x4778, MASKBYTE1, 0x2a);

		// LNA, TIA, ADC backoff at BT TX
		halbb_set_reg(bb, 0x4ae4, 0xffffff, 0x79E99E);
		halbb_set_reg(bb, 0x4aec, 0xffffff, 0x79E99E);

		// IBADC backoff
		halbb_set_reg(bb, 0x469c, 0xfc000000, 0x26);
		halbb_set_reg(bb, 0x49f0, 0xfc000000, 0x26);
	}
}

bool halbb_set_bss_color_8852c(struct bb_info *bb, u8 bss_color,
			       enum phl_phy_idx phy_idx)
{
	/*==== BSS color Check ====*/
	if ((bss_color & ~0x3f) != 0) {
		BB_WARNING("BSS color: %x\n", bss_color);
		return false;
	}
	//=== [Enable BSS color mapping] ===//
	halbb_set_reg_cmn(bb, 0x43ac, BIT(28), 0x1, phy_idx);
	halbb_set_reg_cmn(bb, 0x43ac, 0xfc00000, bss_color, phy_idx);
	BB_DBG(bb, DBG_PHY_CONFIG,
	       "[Success] [P-MAC] BSS Color for [PHY-%d] : %x\n", phy_idx,
	       bss_color);
	return true;
}

bool halbb_set_sta_id_8852c(struct bb_info *bb, u16 sta_id,
			    enum phl_phy_idx phy_idx)
{
	/*==== Station ID Check ====*/
	if ((sta_id & ~0x7ff) != 0) {
		BB_WARNING("Station ID: %x\n", sta_id);
		return false;
	}
	//=== [Set Station ID] ===//
	halbb_set_reg_cmn(bb, 0x43ac, 0x3ff800, sta_id, phy_idx);
	BB_DBG(bb, DBG_PHY_CONFIG,
	       "[Success] [P-MAC] Station ID for [PHY-%d] : %x\n", phy_idx,
	       sta_id);

	return true;
}

void halbb_set_igi_8852c(struct bb_info *bb, u8 lna_idx, bool tia_idx,
			u8 rxbb_idx, enum rf_path path)
{
	u8 lna = 0;
	bool tia = 0;
	u8 rxbb = 0;

	u32 lna_addr[2] = {0x472c, 0x4a80};
	u32 tia_addr[2] = {0x473c, 0x4aa8};
	u32 rxbb_addr[2] = {0x46a8, 0x4a5c};

	BB_DBG(bb, DBG_PHY_CONFIG, "<====== %s ======>\n", __func__);

	halbb_set_reg(bb, lna_addr[path], BIT(26) | BIT(25) | BIT(24), lna_idx);
	halbb_set_reg(bb, tia_addr[path], BIT(9), tia_idx);
	halbb_set_reg(bb, rxbb_addr[path], 0x7c00, rxbb_idx);

	lna = (u8)halbb_get_reg(bb, lna_addr[path], BIT(26) | BIT(25) | BIT(24));
	tia = (bool)halbb_get_reg(bb, tia_addr[path], BIT(9));
	rxbb = (u8)halbb_get_reg(bb, rxbb_addr[path], 0x7c00);

	BB_DBG(bb, DBG_PHY_CONFIG, "[IGI] LNA for [Path-%d] : %d\n", path, lna);
	BB_DBG(bb, DBG_PHY_CONFIG, "[IGI] TIA for [Path-%d] : %d\n", path, tia);
	BB_DBG(bb, DBG_PHY_CONFIG, "[IGI] RxBB for [Path-%d] : %d\n", path, rxbb);
}

void halbb_set_tx_pow_per_path_lmt_8852c(struct bb_info *bb, s16 pwr_lmt_a, s16 pwr_lmt_b)
{
	halbb_set_reg(bb, 0x56CC, 0x1ff, pwr_lmt_a); /*s(9,2) dB*/
	halbb_set_reg(bb, 0x76CC, 0x1ff, pwr_lmt_b); /*s(9,2) dB*/
}

void halbb_set_tx_pow_ref_8852c(struct bb_info *bb, s16 pw_dbm_ofdm, /*s(9,2)*/
				  s16 pw_dbm_cck, s8 ofst,
				  u8 base_cw_0db, u16 tssi_16dBm_cw,
				  u16 *ofdm_cw, u16 *cck_cw,
				  enum phl_phy_idx phy_idx)
{
	s16 rf_pw_cw = 0;
	u32 pw_cw = 0;
	u32 val_ofdm = 0, val_cck = 0;
	s16 pw_s10_3 = 0;
	u32 tssi_ofst_cw = 0;
	bool cfg_path_a = true, cfg_path_b = true;

	/*==== [OFDM] =======================================================*/
	pw_s10_3 = (pw_dbm_ofdm * 2) + (s16)(ofst) + (s16)(base_cw_0db * 8);


	pw_cw = pw_s10_3;
	rf_pw_cw = (pw_s10_3 & 0x1F8) >> 3;

	if (rf_pw_cw > 63)
		pw_cw = (63 << 3) | (pw_s10_3 & 0x7); /*upper bound (+24dBm)*/
	else if (rf_pw_cw < 15)
		pw_cw = (15 << 3) | (pw_s10_3 & 0x7); /*lower bound (-24dBm)*/

	/* ===[Set TSSI Offset]===*/
	/*
	172 = 300 -  (55 - 39) * 8;
	tssi_ofst_cw = tssi_16dBm_cw -  (tx_pow_16dBm_ref_cw - tx_pow_ref_cw) * 8;
	             = tssi_16dBm_cw + tx_pow_ref * 8  - tx_pow_16dBm_ref * 8
	*/
	tssi_ofst_cw = (u32)((s16)tssi_16dBm_cw + (pw_dbm_ofdm * 2) - (16 * 8));

	BB_DBG(bb, DBG_DBG_API, "[OFDM]tssi_ofst_cw=%d, rf_cw=0x%x, bb_cw=0x%x\n", tssi_ofst_cw, pw_cw >> 3, pw_cw & 0x7);

	*ofdm_cw = (u16)pw_cw;
	val_ofdm = tssi_ofst_cw << 18 | pw_cw << 9 | (u32)(pw_dbm_ofdm & 0x1ff);

	/*==== [CCK] =========================================================*/
	pw_s10_3 = (pw_dbm_cck * 2) + (s16)(ofst) + (s16)(base_cw_0db * 8);

	pw_cw = pw_s10_3;
	rf_pw_cw = (pw_s10_3 & 0x1F8) >> 3;

	if (rf_pw_cw > 63)
		pw_cw = (63 << 3) | (pw_s10_3 & 0x7); /*upper bound (+24dBm)*/
	else if (rf_pw_cw < 15)
		pw_cw = (15 << 3) | (pw_s10_3 & 0x7); /*lower bound (-24dBm)*/

	/* ===[Set TSSI Offset]===*/
	/*
	172 = 300 -  (55 - 39) * 8;
	tssi_ofst_cw = tssi_16dBm_cw -  (tx_pow_16dBm_ref_cw - tx_pow_ref_cw) * 8;
	             = tssi_16dBm_cw + tx_pow_ref * 8  - tx_pow_16dBm_ref * 8
	*/
	tssi_ofst_cw = (u32)((s16)tssi_16dBm_cw + (pw_dbm_cck * 2) - (16 * 8));

	BB_DBG(bb, DBG_DBG_API, "[CCK] tssi_ofst_cw=%d, rf_cw=0x%x, bb_cw=0x%x\n", tssi_ofst_cw, pw_cw >> 3, pw_cw & 0x7);

	*cck_cw = (u16)pw_cw;
	val_cck = tssi_ofst_cw << 18 | pw_cw << 9 | (u32)(pw_dbm_cck & 0x1ff);

	/*==== [SET BB CR] ===================================================*/
#ifdef HALBB_DBCC_SUPPORT
	if (bb->hal_com->dbcc_en) {
		if (phy_idx == HW_PHY_0)
			cfg_path_b = false; /*path-A only for phy-0, no need to set path-B*/
		else if (phy_idx == HW_PHY_1)
			cfg_path_a = false; /*path-B only for phy-1, no need to set path-A*/
	}
#endif
	/*path-A*/
	if (cfg_path_a) {
		halbb_set_reg(bb, 0x5804, 0x7FFFFFF, val_ofdm);
		halbb_set_reg(bb, 0x5808, 0x7FFFFFF, val_cck);
	}
	/*path-B*/
	if (cfg_path_b) {
		halbb_set_reg(bb, 0x7804, 0x7FFFFFF, val_ofdm);
		halbb_set_reg(bb, 0x7808, 0x7FFFFFF, val_cck);
	}
}

void halbb_dump_bb_reg_8852c(struct bb_info *bb, u32 *_used, char *output,
			     u32 *_out_len, bool dump_2_buff, enum bb_frc_phy_dump_reg frc_phy_dump)
{
	u32 i = 0, addr = 0, addr_1 = 0;
	u32 cr_start = 0, cr_end = 0;
	u32 dump_cr_table[][2] = {{0x0000, 0x04FC},
				  {0x0600, 0x0DFC},
				  {0x1000, 0x10FC},
				  {0x1200, 0x13FC},
				  {0x1700, 0x20FC},
				  {0x2200, 0x24FC},
				  {0x2600, 0x2DFC},
				  {0x3000, 0x30FC},
				  {0x3200, 0x33FC},
				  {0x3900, 0x4FFC},
				  {0x5600, 0x56FC},
				  {0x5800, 0x6FFC},
				  {0x7600, 0x76FC},
				  {0x7800, 0x7FFC},
				  {0xC000, 0xC2FC}};
	u32 bb_wrapper_table[][2] = {{0xd200, 0xd6e8},
				     {0xd800, 0xd958},
				     {0xdb00, 0xdcfc}};
	u32 table_len = sizeof(dump_cr_table)/(sizeof(u32) * 2);
	u32 bb_wrapper_len = sizeof(bb_wrapper_table)/(sizeof(u32) * 2);
	u32 reg_val = 0;

	BB_TRACE1(bb, "BBCR: dump all ==>\n");
	BB_TRACE1(bb, "table_len=%d\n", table_len);

	for (i = 0; i < table_len; i ++) {
		if (dump_2_buff)
			BB_DBG_VAST(*_out_len, *_used, output + *_used, *_out_len - *_used,
				    "[%02d] Reg[0x%04x ~ 0x%04x]\n\n",
				    i, dump_cr_table[i][0], dump_cr_table[i][1]);
		else
			BB_TRACE1(bb, "[%02d] Reg[0x%04x ~ 0x%04x]\n\n",
				 i, dump_cr_table[i][0], dump_cr_table[i][1]);
	}

	for (i = 0; i < table_len; i ++) {
		cr_start = dump_cr_table[i][0];
		cr_end = dump_cr_table[i][1];
		for (addr = cr_start; addr <= cr_end; addr += 4) {
			if (frc_phy_dump == FRC_DUMP_ALL)
				reg_val = halbb_get_reg(bb, addr, MASKDWORD);
			else
				reg_val = halbb_get_reg_cmn(bb, addr, MASKDWORD, (enum phl_phy_idx)frc_phy_dump);

			if (dump_2_buff)
				BB_DBG_VAST(*_out_len, *_used, output + *_used, *_out_len - *_used,
					    "0x%04x 0x%08x\n", addr, reg_val);
			else
				BB_TRACE1(bb, "0x%04x 0x%08x\n", addr, reg_val);
		}
	}

	BB_TRACE1(bb, "BB Wrapper[0]\n ======================================>");

	for (i = 0; i < bb_wrapper_len; i ++) {
		cr_start = bb_wrapper_table[i][0];
		cr_end = bb_wrapper_table[i][1];
		for (addr = cr_start; addr <= cr_end; addr += 4) {
			rtw_hal_mac_get_pwr_reg(bb->hal_com, 0, addr, &reg_val);

			if (dump_2_buff)
				BB_DBG_VAST(*_out_len, *_used, output + *_used, *_out_len - *_used,
					    "0x%04x 0x%08x\n", addr, reg_val);
			else
				BB_TRACE1(bb, "0x%04x 0x%08x\n", addr, reg_val);
		}
	}

#ifdef HALBB_DBCC_SUPPORT
	BB_TRACE1(bb, "BB Wapper[1]\n ======================================>");
	BB_TRACE1(bb, "dbcc_en=%d\n", bb->hal_com->dbcc_en);

	if (bb->phl_com->dev_cap.dbcc_sup) {
		for (i = 0; i < bb_wrapper_len; i ++) {
			cr_start = bb_wrapper_table[i][0];
			cr_end = bb_wrapper_table[i][1];
			for (addr = cr_start; addr <= cr_end; addr += 4) {
				rtw_hal_mac_get_pwr_reg(bb->hal_com, 1, addr, &reg_val);
				addr_1 = addr | BIT13;
				if (dump_2_buff)
					BB_DBG_VAST(*_out_len, *_used, output + *_used, *_out_len - *_used,
						    "0x%04x 0x%08x\n", addr_1, reg_val);
				else
					BB_TRACE1(bb, "0x%04x 0x%08x\n", addr_1, reg_val);
			}
		}
	}
#endif
}

void halbb_dbgport_dump_all_8852c(struct bb_info *bb, u32 *_used, char *output,
				    u32 *_out_len)
{
	const u32 dump_dbgport_table[][3] = {
					{DBGPORT_IP_TD, 0x001, 0x026},
					{DBGPORT_IP_TD, 0x200, 0x2ff},
					{DBGPORT_IP_TD, 0xb01, 0xb27},
					{DBGPORT_IP_RX_INNER, 0x0, 0x29},
					{DBGPORT_IP_TX_INNER, 0x0, 0x8},
					{DBGPORT_IP_OUTER, 0x0, 0xaa},
					{DBGPORT_IP_OUTER, 0xc0, 0xc4},
					{DBGPORT_IP_INTF, 0x0, 0x40},
					{DBGPORT_IP_CCK, 0x0, 0x3e},
					{DBGPORT_IP_BF, 0x0, 0x59},
					{DBGPORT_IP_RX_OUTER, 0x00, 0x63},
					{DBGPORT_IP_RX_OUTER, 0x90, 0x98},
					{DBGPORT_IP_RX_OUTER, 0xc0, 0xc3},
					{DBGPORT_IP_RX_OUTER, 0xe0, 0xe3}};
	u32 table_len;
	u32 dp = 0; /*debug port value*/
	u8 i;
	u32 j;
	u32 dbg_start = 0, dbg_end = 0;

	table_len = sizeof(dump_dbgport_table) / (sizeof(u32) * 3);

	BB_DBG_VAST(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "BB DBG Port: dump all ==>\n");

	for (i = 0; i < table_len; i++) {
		BB_DBG_VAST(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "[%02d][IP=%02d] Dbgport[0x%03x ~ 0x%03x]\n",
			    i, dump_dbgport_table[i][0],
			    dump_dbgport_table[i][1], dump_dbgport_table[i][2]);
	}

	for (i = 0; i < table_len; i++) {
		halbb_set_bb_dbg_port_ip(bb, dump_dbgport_table[i][0]);
		dbg_start = dump_dbgport_table[i][1];
		dbg_end = dump_dbgport_table[i][2];
		for (j = dbg_start; j <= dbg_end; j ++) {
			halbb_set_bb_dbg_port(bb, j);
			dp = halbb_get_bb_dbg_port_val(bb);
			BB_DBG_VAST(*_out_len, *_used, output + *_used, *_out_len - *_used,
				    "[0x%02x, 0x%03x] = 0x%08x\n",
				    dump_dbgport_table[i][0], j, dp);
		}
	}
	halbb_release_bb_dbg_port(bb);
}

void halbb_physts_brk_fail_pkt_rpt_8852c(struct bb_info *bb, bool enable,
					   enum phl_phy_idx phy_idx)
{
	u32 val32 = (enable) ? 0 : 0x3;

	halbb_set_reg_cmn(bb, 0x0738, 0xC, val32, phy_idx);
}

bool halbb_rf_write_bb_reg_8852c(struct bb_info *bb, u32 addr, u32 mask, u32 data)
{
	u32 page = (addr & 0xff00) >> 8;

	if (page != 0x0c && page != 0x20 && page != 0x2c &&
	    page != 0x58 && page != 0x78 &&
	    addr != 0x0700 && addr != 0x12a0 && addr != 0x12b8 &&
	    addr != 0x2320 && addr != 0x2700 &&
	    addr != 0x32a0 && addr != 0x32b8) {
		return false;
	}

	halbb_set_reg(bb, addr, mask, data);
	return true;
}

void halbb_pre_agc_en_8852c(struct bb_info *bb, bool enable)
{
	u8 en = 0;

	en = (enable == true) ? 1 : 0;
	halbb_set_reg(bb, 0x4730, BIT(31), en);
	halbb_set_reg(bb, 0x4A9C, BIT(31), en);
	BB_DBG(bb, DBG_DBG_API, "PreAGC en: 0x4670[20]=(0x%x)\n", en);
}

void halbb_agc_fix_gain_8852c(struct bb_info *bb, bool enable, u8 lna_idx,
			      u8 tia_idx, u8 rxbb_idx, enum rf_path path)
{
	u32 agc_en_addr[2] = {0x4730, 0x4A9C};

	if ((lna_idx > 6) || (tia_idx > 1) || (rxbb_idx > 31)) {
		BB_DBG(bb, DBG_IC_API, "[AGC][Path%d] Invalid IGI idx {%d, %d, %d}!!\n",
		       path, lna_idx, tia_idx, rxbb_idx);
		return;
	}

	halbb_set_reg(bb, agc_en_addr[path], BIT(31), enable);
	halbb_set_igi_8852c(bb, lna_idx, tia_idx, rxbb_idx, path);

	BB_DBG(bb, DBG_IC_API,
	       "[AGC][Path%d] {Fix_en, lna, tia, rxbb}={%d, %d, %d, %d}\n",
		path, enable, lna_idx, tia_idx, rxbb_idx);
}

void halbb_agc_tia_shrink_8852c(struct bb_info *bb, bool shrink_en,
				bool shrink_init, enum rf_path path)
{
	u32 shrink_addr[2] = {0x473C, 0x4AA8};

	if ((shrink_en == true) && (shrink_init == false))
		BB_DBG(bb, DBG_IC_API,
		       "[AGC][Path%d] TIA Shrink setting is set to default!! {en, init}={%d, %d}\n",
		       path, shrink_en, shrink_init);
	else
		BB_DBG(bb, DBG_IC_API,
		       "[AGC][Path%d] TIA Shrink config!! {en, init}={%d, %d}\n",
		       path, shrink_en, shrink_init);

	halbb_set_reg(bb, shrink_addr[path], BIT(11), shrink_en);
	halbb_set_reg(bb, shrink_addr[path], BIT(12), shrink_init);
}

void halbb_agc_step_en_8852c(struct bb_info *bb, bool pre_pd_agc_en,
			     bool linear_agc_en, bool post_pd_agc_en,
			     bool nlgc_agc_en, enum rf_path path)
{
	u32 agc_addr[2] = {0x473C, 0x4AA8};

	if (pre_pd_agc_en && linear_agc_en && post_pd_agc_en && nlgc_agc_en)
		BB_DBG(bb, DBG_IC_API,
		       "[AGC][Path%d] AGC step setting is set to default!\n",
		       path);
	else
		BB_DBG(bb, DBG_IC_API,
		       "[AGC][Path%d] AGC step config!! {pre_pd, linear, post_pd, nlgc}={%d, %d, %d, %d}\n",
		       path, pre_pd_agc_en, linear_agc_en, post_pd_agc_en,
		       nlgc_agc_en);

	halbb_set_reg(bb, agc_addr[path], BIT(6), pre_pd_agc_en);
	halbb_set_reg(bb, agc_addr[path], BIT(0), linear_agc_en);
	halbb_set_reg(bb, agc_addr[path], BIT(5), post_pd_agc_en);
	halbb_set_reg(bb, agc_addr[path], BIT(2), nlgc_agc_en);
}

s8 halbb_efuse_exchange_8852c(struct bb_info *bb, u8 value,
				enum efuse_bit_mask mask)
{
	s8 tmp = 0;

	if (mask == LOW_MASK) {
		tmp = value & 0xf;

		if (tmp & BIT(3))
			tmp = tmp | 0xf0;
	} else {
		tmp = (value & 0xf0) >> 4;

		if (tmp & BIT(3))
			tmp = tmp | 0xf0;
	}

	return tmp;
}

u8 halbb_band_determine_8852c(struct bb_info *bb, u8 central_ch,
			      enum band_type band_type, bool is_normal_efuse)
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

void halbb_ext_loss_avg_update_8852c(struct bb_info *bb,
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

	halbb_set_reg(bb, cr->r_pwrofst, cr->r_pwrofst_m, bb_edcca->pwrofst);

	BB_DBG(bb, DBG_PHY_CONFIG,
	       "[Ext loss] path{avg, a, b}={%d, %d, %d}(0.25dB), edcca_pwrofst = %d dB\n",
	       ch->ext_loss_avg, bb->bb_ch_i.ext_loss[RF_PATH_A],
	       bb->bb_ch_i.ext_loss[RF_PATH_B], bb_edcca->pwrofst - 16);
}

void halbb_get_normal_efuse_init_8852c(struct bb_info *bb)
{
	struct bb_efuse_info *gain = &bb->bb_efuse_i;
	u8 tmp, tmp_ofst;
	u32 check_tmp = 0, i, j;

	BB_DBG(bb, DBG_PHY_CONFIG, "<====== %s ======>\n", __func__);

	tmp_ofst = (u8)halbb_get_reg(bb, 0x4df0, 0xff); // PHY0
	bb->bb_efuse_i.efuse_ofst[HW_PHY_0] = (s8)halbb_cnvrt_2_sign(tmp_ofst, 8);
	tmp_ofst = (u8)halbb_get_reg(bb, 0x6df0, 0xff); // Phy1
	bb->bb_efuse_i.efuse_ofst[HW_PHY_1] = (s8)halbb_cnvrt_2_sign(tmp_ofst, 8);
	tmp_ofst = (u8)halbb_get_reg(bb, 0x4e0c, 0xff000000); // PHY0 TB
	bb->bb_efuse_i.efuse_ofst_tb[HW_PHY_0] = (s8)halbb_cnvrt_2_sign(tmp_ofst, 8);
	tmp_ofst = (u8)halbb_get_reg(bb, 0x6e0c, 0xff000000); // Phy1 TB
	bb->bb_efuse_i.efuse_ofst_tb[HW_PHY_1] = (s8)halbb_cnvrt_2_sign(tmp_ofst, 8);

	halbb_efuse_get_info(bb, EFUSE_INFO_RF_RX_GAIN_K_A_2G_CCK, &tmp, 1);
	gain->gain_offset[RF_PATH_A][0] = halbb_efuse_exchange_8852c(bb, tmp, HIGH_MASK);
	gain->gain_offset[RF_PATH_B][0] = halbb_efuse_exchange_8852c(bb, tmp, LOW_MASK);

	halbb_efuse_get_info(bb, EFUSE_INFO_RF_RX_GAIN_K_A_2G_OFMD, &tmp, 1);
	gain->gain_offset[RF_PATH_A][1] = halbb_efuse_exchange_8852c(bb, tmp, HIGH_MASK);
	gain->gain_offset[RF_PATH_B][1] = halbb_efuse_exchange_8852c(bb, tmp, LOW_MASK);

	halbb_efuse_get_info(bb, EFUSE_INFO_RF_RX_GAIN_K_A_5GL, &tmp, 1);
	gain->gain_offset[RF_PATH_A][2] = halbb_efuse_exchange_8852c(bb, tmp, HIGH_MASK);
	gain->gain_offset[RF_PATH_B][2] = halbb_efuse_exchange_8852c(bb, tmp, LOW_MASK);

	halbb_efuse_get_info(bb, EFUSE_INFO_RF_RX_GAIN_K_A_5GM, &tmp, 1);
	gain->gain_offset[RF_PATH_A][3] = halbb_efuse_exchange_8852c(bb, tmp, HIGH_MASK);
	gain->gain_offset[RF_PATH_B][3] = halbb_efuse_exchange_8852c(bb, tmp, LOW_MASK);

	halbb_efuse_get_info(bb, EFUSE_INFO_RF_RX_GAIN_K_A_5GH, &tmp, 1);
	gain->gain_offset[RF_PATH_A][4] = halbb_efuse_exchange_8852c(bb, tmp, HIGH_MASK);
	gain->gain_offset[RF_PATH_B][4] = halbb_efuse_exchange_8852c(bb, tmp, LOW_MASK);

	halbb_efuse_get_info(bb, EFUSE_INFO_RF_RX_GAIN_K_A_6GL1, &tmp, 1);
	gain->gain_offset[RF_PATH_A][5] = halbb_efuse_exchange_8852c(bb, tmp, HIGH_MASK);
	gain->gain_offset[RF_PATH_B][5] = halbb_efuse_exchange_8852c(bb, tmp, LOW_MASK);

	halbb_efuse_get_info(bb, EFUSE_INFO_RF_RX_GAIN_K_A_6GL2, &tmp, 1);
	gain->gain_offset[RF_PATH_A][6] = halbb_efuse_exchange_8852c(bb, tmp, HIGH_MASK);
	gain->gain_offset[RF_PATH_B][6] = halbb_efuse_exchange_8852c(bb, tmp, LOW_MASK);

	halbb_efuse_get_info(bb, EFUSE_INFO_RF_RX_GAIN_K_A_6GL3, &tmp, 1);
	gain->gain_offset[RF_PATH_A][7] = halbb_efuse_exchange_8852c(bb, tmp, HIGH_MASK);
	gain->gain_offset[RF_PATH_B][7] = halbb_efuse_exchange_8852c(bb, tmp, LOW_MASK);

	halbb_efuse_get_info(bb, EFUSE_INFO_RF_RX_GAIN_K_A_6GM1, &tmp, 1);
	gain->gain_offset[RF_PATH_A][8] = halbb_efuse_exchange_8852c(bb, tmp, HIGH_MASK);
	gain->gain_offset[RF_PATH_B][8] = halbb_efuse_exchange_8852c(bb, tmp, LOW_MASK);

	halbb_efuse_get_info(bb, EFUSE_INFO_RF_RX_GAIN_K_A_6GM2, &tmp, 1);
	gain->gain_offset[RF_PATH_A][9] = halbb_efuse_exchange_8852c(bb, tmp, HIGH_MASK);
	gain->gain_offset[RF_PATH_B][9] = halbb_efuse_exchange_8852c(bb, tmp, LOW_MASK);

	halbb_efuse_get_info(bb, EFUSE_INFO_RF_RX_GAIN_K_A_6GM3, &tmp, 1);
	gain->gain_offset[RF_PATH_A][10] = halbb_efuse_exchange_8852c(bb, tmp, HIGH_MASK);
	gain->gain_offset[RF_PATH_B][10] = halbb_efuse_exchange_8852c(bb, tmp, LOW_MASK);

	halbb_efuse_get_info(bb, EFUSE_INFO_RF_RX_GAIN_K_A_6GH1, &tmp, 1);
	gain->gain_offset[RF_PATH_A][11] = halbb_efuse_exchange_8852c(bb, tmp, HIGH_MASK);
	gain->gain_offset[RF_PATH_B][11] = halbb_efuse_exchange_8852c(bb, tmp, LOW_MASK);

	halbb_efuse_get_info(bb, EFUSE_INFO_RF_RX_GAIN_K_A_6GH2, &tmp, 1);
	gain->gain_offset[RF_PATH_A][12] = halbb_efuse_exchange_8852c(bb, tmp, HIGH_MASK);
	gain->gain_offset[RF_PATH_B][12] = halbb_efuse_exchange_8852c(bb, tmp, LOW_MASK);

	for (i = 0; i < HALBB_MAX_PATH; i++) {
		for (j = 0; j < (BB_BAND_NUM_MAX + 1); j++) {
			BB_DBG(bb, DBG_INIT, "[Efuse]gain->gain_offset[%d][%d]=0x%x\n", i, j, gain->gain_offset[i][j]);
			if ((gain->gain_offset[i][j] & 0xf) == 0xf)
				check_tmp++;
		}
	}

	BB_DBG(bb, DBG_INIT, "[Efuse]check_tmp = %d\n", check_tmp);
	BB_DBG(bb, DBG_INIT, "[Efuse]HALBB_MAX_PATH * (BB_BAND_NUM_MAX + 1) = %d\n", HALBB_MAX_PATH * (BB_BAND_NUM_MAX + 1));

	if (check_tmp == HALBB_MAX_PATH * (BB_BAND_NUM_MAX + 1))
		bb->bb_efuse_i.normal_efuse_check = false;
	else
		bb->bb_efuse_i.normal_efuse_check = true;
	BB_DBG(bb, DBG_INIT, "[Efuse]normal_efuse_check=%d\n", bb->bb_efuse_i.normal_efuse_check);
/*
	BB_DBG(bb, DBG_INIT,
	       "[Efuse][Gain 2G][CCK] Path-A: %d, Path-B: %d\n",
	       gain->gain_offset[RF_PATH_A][0], gain->gain_offset[RF_PATH_B][0]);
	BB_DBG(bb, DBG_INIT,
	       "[Efuse][Gain 2G][OFDM] Path-A: %d, Path-B: %d\n",
	       gain->gain_offset[RF_PATH_A][1], gain->gain_offset[RF_PATH_B][1]);
	BB_DBG(bb, DBG_INIT,
	       "[Efuse][Gain 5GL] Path-A: %d, Path-B: %d\n",
	       gain->gain_offset[RF_PATH_A][2], gain->gain_offset[RF_PATH_B][2]);
	BB_DBG(bb, DBG_INIT,
	       "[Efuse][Gain 5GM] Path-A: %d, Path-B: %d\n",
	       gain->gain_offset[RF_PATH_A][3], gain->gain_offset[RF_PATH_B][3]);
	BB_DBG(bb, DBG_INIT,
	       "[Efuse][Gain 5GH] Path-A: %d, Path-B: %d\n",
	       gain->gain_offset[RF_PATH_A][4], gain->gain_offset[RF_PATH_B][4]);
*/
}


void halbb_get_hidden_efuse_init_8852c(struct bb_info *bb)
{
	struct bb_efuse_info *gain = &bb->bb_efuse_i;
	u32 check_tmp = 0, i;
	enum rf_path path = RF_PATH_A;
	u32 hidden_efuse_addr[BB_PATH_MAX_8852C][12] = {{GAIN_HIDE_EFUSE_A_2G_8852C,
							GAIN_HIDE_EFUSE_A_5GL_8852C,
							GAIN_HIDE_EFUSE_A_5GM_8852C,
							GAIN_HIDE_EFUSE_A_5GH_8852C,
							GAIN_HIDE_EFUSE_A_6GH_0_8852C,
							GAIN_HIDE_EFUSE_A_6GH_1_8852C,
							GAIN_HIDE_EFUSE_A_6GH_2_8852C,
							GAIN_HIDE_EFUSE_A_6GH_3_8852C,
							GAIN_HIDE_EFUSE_A_6GH_4_8852C,
							GAIN_HIDE_EFUSE_A_6GH_5_8852C,
							GAIN_HIDE_EFUSE_A_6GH_6_8852C,
							GAIN_HIDE_EFUSE_A_6GH_7_8852C},
							{GAIN_HIDE_EFUSE_B_2G_8852C,
							GAIN_HIDE_EFUSE_B_5GL_8852C,
							GAIN_HIDE_EFUSE_B_5GM_8852C,
							GAIN_HIDE_EFUSE_B_5GH_8852C,
							GAIN_HIDE_EFUSE_B_6GH_0_8852C,
							GAIN_HIDE_EFUSE_B_6GH_1_8852C,
							GAIN_HIDE_EFUSE_B_6GH_2_8852C,
							GAIN_HIDE_EFUSE_B_6GH_3_8852C,
							GAIN_HIDE_EFUSE_B_6GH_4_8852C,
							GAIN_HIDE_EFUSE_B_6GH_5_8852C,
							GAIN_HIDE_EFUSE_B_6GH_6_8852C,
							GAIN_HIDE_EFUSE_B_6GH_7_8852C}};

	BB_DBG(bb, DBG_PHY_CONFIG, "<====== %s ======>\n", __func__);

	halbb_phy_efuse_get_info(bb, GAIN_HIDE_EFUSE_START_ADDR_8852C, BB_HIDE_EFUSE_SIZE, bb->bb_efuse_i.hidden_efuse);

	for (path = RF_PATH_A; path < BB_PATH_MAX_8852C; path++) {
		for (i = 0; i < BB_BAND_NUM_MAX; i++) {
			gain->gain_cs[path][i] = halbb_efuse_exchange_8852c(bb, bb->bb_efuse_i.hidden_efuse[hidden_efuse_addr[path][i] - GAIN_HIDE_EFUSE_START_ADDR_8852C], HIGH_MASK);
			gain->gain_cg[path][i] = halbb_efuse_exchange_8852c(bb, bb->bb_efuse_i.hidden_efuse[hidden_efuse_addr[path][i] - GAIN_HIDE_EFUSE_START_ADDR_8852C], LOW_MASK);
			BB_DBG(bb, DBG_INIT, "[Hidden Efuse] gain_cs/gain_cg[path][band_idx]\n");
			BB_DBG(bb, DBG_INIT, "[Hidden Efuse] gain_cs[%d][%d]: %d, gain_cg[%d][%d]: %d\n", path, i, gain->gain_cs[path][i], path, i, gain->gain_cg[path][i]);
		}
	}

	for (path = RF_PATH_A; path < BB_PATH_MAX_8852C; path++) {
		for (i = 0; i < BB_BAND_NUM_MAX; i++) {
			BB_DBG(bb, DBG_INIT, "[Efuse]gain->gain_cs[%d][%d]=0x%x\n", path, i, gain->gain_cg[path][i]);
			if (((gain->gain_cg[path][i] & 0xf) == 0xf) && ((gain->gain_cs[path][i] & 0xf) == 0xf))
				check_tmp++;
		}
	}

	BB_DBG(bb, DBG_INIT, "[Efuse]check_tmp = %d\n", check_tmp);
	BB_DBG(bb, DBG_INIT, "[Efuse]HALBB_MAX_PATH * BB_BAND_NUM_MAX = %d\n", HALBB_MAX_PATH * BB_BAND_NUM_MAX);

	if (check_tmp == HALBB_MAX_PATH * BB_BAND_NUM_MAX)
		bb->bb_efuse_i.hidden_efuse_check = false;
	else
		bb->bb_efuse_i.hidden_efuse_check = true;
	BB_DBG(bb, DBG_INIT, "[Efuse]bb->bb_efuse_i.hidden_efuse_check=%d\n", bb->bb_efuse_i.hidden_efuse_check);
}

void halbb_set_hidden_efuse_8852c(struct bb_info *bb, u8 central_ch, enum band_type band_type, enum rf_path path)
{
	u8 band;
	s32 hidden_efuse = 0;
	s8 gain = 0;
	u32 gain_err_addr[2] = {0x4BC4, 0x4C88};

	BB_DBG(bb, DBG_PHY_CONFIG, "<====== %s ======>\n", __func__);

	band = halbb_band_determine_8852c(bb, central_ch, band_type, false);

	// === [Set hidden efuse] === //
	if (bb->bb_efuse_i.hidden_efuse_check) {
		gain = bb->phl_com->dev_cap.rfe_type > 50 ? bb->bb_efuse_i.gain_cs[path][band] : bb->bb_efuse_i.gain_cg[path][band];
		hidden_efuse = (s32)(gain << 2);
		halbb_set_reg(bb, gain_err_addr[path], MASKBYTE1, (hidden_efuse & 0xff));
		BB_DBG(bb, DBG_PHY_CONFIG, "[Efuse][S%d] Hidden efuse = 0x%x; addr=0x%x, bitmap=0xff\n", path, gain, gain_err_addr[path]);
	} else {
		BB_DBG(bb, DBG_PHY_CONFIG, "[Efuse] Values of hidden efuse are all 0xff, bypass dynamic setting!!\n");
	}
}

void halbb_set_normal_efuse_8852c(struct bb_info *bb, u8 central_ch, enum band_type band_type, enum rf_path path)
{
	u8 band;
	s32 normal_efuse = 0, normal_efuse_cck = 0, normal_efuse_offset_cck = 0;
	s32 tmp = 0;
	u32 rssi_ofst_addr[2] = {0x4694, 0x4778};
	u32 rpl_bitmap[2] = {0xff00, 0xff0000};
	u32 rpl_tb_bitmap[2] = {0xff, 0xff00};
	enum phl_phy_idx phy_idx = HW_PHY_0;

	band = halbb_band_determine_8852c(bb, central_ch, band_type, true);

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
			halbb_set_reg(bb, 0x2340, 0x7f00, (tmp & 0x7f));
		}

		// OFDM normal efuse
		normal_efuse = bb->bb_efuse_i.gain_offset[path][band];

		normal_efuse *= (-1);

		// r_g_offset
		tmp = (normal_efuse << 2) + (bb->bb_efuse_i.efuse_ofst[phy_idx] >> 2);
		tmp *= (-1);
		halbb_set_reg(bb, rssi_ofst_addr[path], 0xff0000, (tmp & 0xff));

		BB_DBG(bb, DBG_PHY_CONFIG,
			"[Normal Efuse][Phy%d] ========== Path=%d ==========\n",
			phy_idx, path);
		BB_DBG(bb, DBG_PHY_CONFIG,
			"[Normal Efuse] normal_efuse=0x%x, efuse_ofst0x%x\n",
			normal_efuse, bb->bb_efuse_i.efuse_ofst[phy_idx]);
		BB_DBG(bb, DBG_PHY_CONFIG,
			"[RSSI] addr=0x%x, bitmask=0xff0000, val=0x%x\n",
			rssi_ofst_addr[path], tmp);

		// r_l_rpl
		tmp = (normal_efuse << 4);
		halbb_set_reg_cmn(bb, 0x4e0c, rpl_bitmap[path], (tmp & 0xff), phy_idx);

		BB_DBG(bb, DBG_PHY_CONFIG,
			"[RPL] addr=0x4e0c, bitmask=0x%x, val=0x%x\n",
			rpl_bitmap[path], tmp);

		// r_tb_rssi_bias_comp
		halbb_set_reg_cmn(bb, 0x4e2c, rpl_tb_bitmap[path], (tmp & 0xff), phy_idx);

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

	halbb_ext_loss_avg_update_8852c(bb, band_type);
}

void halbb_set_gain_cr_init_8852c(struct bb_info *bb)
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
			halbb_set_reg(bb, lna_gain_g[path][lna_idx], lna_gain_mask[lna_idx], tmp);
			BB_DBG(bb, DBG_PHY_CONFIG, "lna_gain_g[%d][%d]=0x%x, lna_gain_mask[%d]=0x%x, val=%d\n", path, lna_idx, lna_gain_g[path][lna_idx], lna_idx, lna_gain_mask[lna_idx], tmp);
			tmp = gain->lna_gain_bypass[0][path][lna_idx];
			halbb_set_reg(bb, lna_gain_g_bypass[path][lna_idx], lna_gain_g_bypass_mask[lna_idx], tmp);
		}
		// Set 2G TIA value
		for (tia_idx = 0; tia_idx < 2; tia_idx++) {
			tmp = gain->tia_gain[0][path][tia_idx];
			halbb_set_reg(bb, tia_gain_g[path][tia_idx], tia_gain_mask[tia_idx], tmp);
		}
	}
}

void halbb_set_gain_error_8852c(struct bb_info *bb, u8 central_ch, enum band_type band_type, enum rf_path path)
{
	/* This function is the dynamic mechanism of 5G Rx gain k */
	struct bb_gain_info *gain = &bb->bb_gain_i;

	u8 band;
	u8 lna_idx = 0, tia_idx = 0, i = 0 ;
	s32 tmp = 0;
	u32 wb_gidx_tmp = 0;
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

	if (band_type == BAND_ON_24G)
		return;

	band = halbb_band_determine_8852c(bb, central_ch, band_type, false);
	// Seperated in 4 bands, with 2 BW160 each, choose gain offset of lower BW160
	if (band_type == BAND_ON_6G)
		band = (band + 4) / 2;

	BB_DBG(bb, DBG_PHY_CONFIG, "[%s] central_ch=%d, band_type=%d, path=%d, band=%d\n", __func__, central_ch, band_type, path, band);

	for (lna_idx = 0; lna_idx < 7; lna_idx++) {
		// Set LNA value
		tmp = gain->lna_gain[band][path][lna_idx];
		halbb_set_reg(bb, lna_gain_a[path][lna_idx], lna_gain_mask[lna_idx], tmp);
		BB_DBG(bb, DBG_PHY_CONFIG, "lna_gain_a[%d][%d]=0x%x, lna_gain_mask[%d]=0x%x, val=%d\n", path, lna_idx, lna_gain_a[path][lna_idx], lna_idx, lna_gain_mask[lna_idx], tmp);
		tmp = gain->lna_gain_bypass[band][path][lna_idx];
		halbb_set_reg(bb, lna_gain_a_bypass[path][lna_idx], lna_gain_a_bypass_mask[lna_idx], tmp);
		// Set op1dB value
		tmp = gain->lna_op1db[band][path][lna_idx];
		halbb_set_reg(bb, lna_op1db_a[path][lna_idx], op1db_a_mask[lna_idx], tmp);
		tmp = gain->tia_lna_op1db[band][path][lna_idx];
		halbb_set_reg(bb, tia_lna_op1db_a[path][lna_idx], op1db_a_mask[lna_idx], tmp);
	}

	// Set TIA0_LNA6 op1dB value
	tmp = gain->tia_lna_op1db[band][path][7];
	halbb_set_reg(bb, tia0_lna6_op1db_a[path], 0xff000000, tmp);

	// Set TIA value
	for (tia_idx = 0; tia_idx < 2; tia_idx++) {
		tmp = gain->tia_gain[band][path][tia_idx];
		halbb_set_reg(bb, tia_gain_a[path][tia_idx], tia_gain_mask[tia_idx], tmp);
	}

	// Set wb_gidx
	if (bb->phl_com->dev_cap.rfe_type >= 51) {
		// wb_gidx_elna
		halbb_set_reg(bb, wb_gidx_elna[path], 0x3fffc0, gain->wb_gidx_elna[band][path]);
		// wb_gidx_lna_tia
		wb_gidx_tmp = gain->wb_gidx_lna_tia[band][path][0];
		halbb_set_reg(bb, wb_gidx_lna_tia[path][0], 0xE0000000, wb_gidx_tmp);

		wb_gidx_tmp = (gain->wb_gidx_lna_tia[band][path][2] << 3) | gain->wb_gidx_lna_tia[band][path][1];
		halbb_set_reg(bb, wb_gidx_lna_tia[path][1], 0x3F000000, wb_gidx_tmp);

		wb_gidx_tmp = 0;
		for (i = 0; i < 10; i++)
			wb_gidx_tmp |= (gain->wb_gidx_lna_tia[band][path][i + 3] << (3 * i));
		halbb_set_reg(bb, wb_gidx_lna_tia[path][2], 0x3FFFFFFF, wb_gidx_tmp);

		wb_gidx_tmp = (gain->wb_gidx_lna_tia[band][path][15] << 6) |
				(gain->wb_gidx_lna_tia[band][path][14] << 3) |
				gain->wb_gidx_lna_tia[band][path][13];
		halbb_set_reg(bb, wb_gidx_lna_tia[path][3], 0x1FF, wb_gidx_tmp);
		// gs_idx
		wb_gidx_tmp = gain->gs_idx[band][path][0] & 0xffff;
		halbb_set_reg(bb, gs_idx[path][0], 0x7FFF8000, wb_gidx_tmp);

		wb_gidx_tmp = (gain->gs_idx[band][path][1] << 16) |
				((gain->gs_idx[band][path][0] & 0xffff0000) >> 16);
		halbb_set_reg(bb, gs_idx[path][1], 0x0FFFFFFF, wb_gidx_tmp);
		// g_elna
		wb_gidx_tmp = (gain->g_elna[band][path][1] << 8) | gain->g_elna[band][path][0];
		halbb_set_reg(bb, g_elna[path], 0xFFFF, wb_gidx_tmp);
	}
}

void halbb_set_rxsc_rpl_comp_8852c(struct bb_info* bb, u8 central_ch)
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
	halbb_set_reg(bb, 0x4e00, MASKDWORD, tmp_val1);
	halbb_set_reg(bb, 0x4e04, MASKDWORD, tmp_val2);
	halbb_set_reg(bb, 0x4e08, 0xffffff, tmp_val3);
	// TB
	halbb_set_reg(bb, 0x4e1c, 0xff000000, tmp_val1_tb);
	halbb_set_reg(bb, 0x4e20, MASKDWORD, tmp_val2_tb);
	halbb_set_reg(bb, 0x4e24, MASKDWORD, tmp_val3_tb);
	halbb_set_reg(bb, 0x4e28, 0xffff, tmp_val4_tb);
}

void halbb_normal_efuse_verify_8852c(struct bb_info *bb, s8 rx_gain_offset,
				       enum rf_path rx_path,
				       enum phl_phy_idx phy_idx)
{
	s32 normal_efuse = 0;
	s32 tmp = 0;
	u32 rssi_ofst_addr[2] = {0x4694, 0x4778};
	u32 rpl_bitmap[2] = {0xff00, 0xff0000};
	u32 rpl_tb_bitmap[2] = {0xff, 0xff00};

	BB_DBG(bb, DBG_PHY_CONFIG, "<====== %s ======>\n", __func__);

	BB_DBG(bb, DBG_PHY_CONFIG, "[Normal Efuse] rx_gain_offset = %d\n",
		rx_gain_offset);

	rx_gain_offset *= (-1);

	BB_DBG(bb, DBG_PHY_CONFIG, "[Normal Efuse] gain_offset=0x%x, path=%d, phy_idx=%d\n",
	       rx_gain_offset, rx_path, phy_idx);
	// === [Set normal efuse] === //
	// r_g_offset
	tmp = (rx_gain_offset << 2) + bb->bb_efuse_i.efuse_ofst[phy_idx];
	halbb_set_reg(bb, rssi_ofst_addr[rx_path], 0xff0000, (tmp & 0xff));
	BB_DBG(bb, DBG_PHY_CONFIG,
	       "[Normal Efuse][RSSI] addr=0x%x, bitmask=0xff0000, val=0x%x\n",
	       rssi_ofst_addr[rx_path], tmp);
	// r_1_rpl_bias_comp
	tmp = (rx_gain_offset << 4);
	halbb_set_reg_cmn(bb, 0x4e0c, rpl_bitmap[rx_path], (tmp & 0xff), phy_idx);
	BB_DBG(bb, DBG_PHY_CONFIG,
	       "[Normal Efuse][RPL] addr=0x4e0c, bitmask=0x%x, val=0x%x\n",
	       rpl_bitmap[rx_path], tmp);
	// r_tb_rssi_bias_comp
	tmp = (rx_gain_offset << 4);
	halbb_set_reg_cmn(bb, 0x4e2c, rpl_tb_bitmap[rx_path], (tmp & 0xff), phy_idx);
	BB_DBG(bb, DBG_PHY_CONFIG,
	       "[Normal TB Efuse] addr=0x4e2c, bitmask=0x%x, val=0x%x\n",
	       rpl_tb_bitmap[rx_path], tmp);
}

void halbb_normal_efuse_verify_cck_8852c(struct bb_info *bb, s8 rx_gain_offset,
					   enum rf_path rx_path,
					   enum phl_phy_idx phy_idx)
{
	s32 tmp = 0;

	BB_DBG(bb, DBG_PHY_CONFIG, "<====== %s ======>\n", __func__);

	rx_gain_offset *= (-1);
	BB_DBG(bb, DBG_PHY_CONFIG, "[Normal Efuse] gain_offset=0x%x, path=%d, phy_idx=%d\n",
	       rx_gain_offset, rx_path,phy_idx);
	// r_11b_rx_rssi_offset
	tmp = rx_gain_offset + (bb->bb_efuse_i.efuse_ofst[phy_idx] >> 4);
	halbb_set_reg(bb, 0x232c, 0xff00000, (tmp & 0xff));
	BB_DBG(bb, DBG_PHY_CONFIG,
	       "[Normal CCK Efuse] addr=0x232c, bitmask=0xff00000, val=0x%x\n",
	       tmp);
	// r_11b_rx_rpl_offset
	tmp = (rx_gain_offset << 3) + (bb->bb_efuse_i.efuse_ofst[phy_idx] >> 1);
	halbb_set_reg(bb, 0x2340, 0x7f00, (tmp & 0x7f));
	BB_DBG(bb, DBG_PHY_CONFIG,
	       "[Normal CCK Efuse] addr=0x2340, bitmask=0x7f00, val=0x%x\n",
	       tmp);
}

void halbb_show_gain_err_8852c(struct bb_info *bb, char input[][16],
			       u32 *_used, char *output, u32 *_out_len)
{
	u32 val[10] = {0};
	u32 used = *_used;
	u32 out_len = *_out_len;
	u8 lna_idx = 0, tia_idx = 0;
	u32 tmp = 0;
	u8 path;
	u32 lna_gain_g[BB_PATH_MAX_8852C][7] = {{0x4678, 0x4678, 0x467C,
						   0x467C, 0x467C, 0x467C,
						   0x4680}, {0x475C, 0x475C,
						   0x4760, 0x4760, 0x4760,
						   0x4760, 0x4764}};
	u32 lna_gain_a[BB_PATH_MAX_8852C][7] = {{0x45DC, 0x45DC, 0x4660,
						   0x4660, 0x4660, 0x4660,
						   0x4664}, {0x4740, 0x4740,
						   0x4744, 0x4744, 0x4744,
						   0x4744, 0x4748}};
	u32 lna_gain_mask[7] = {0x00ff0000, 0xff000000, 0x000000ff,
				    0x0000ff00, 0x00ff0000, 0xff000000,
				    0x000000ff};
	u32 tia_gain_g[BB_PATH_MAX_8852C][2] = {{0x4680, 0x4680}, {0x4764,
						   0x4764}};
	u32 tia_gain_a[BB_PATH_MAX_8852C][2] = {{0x4664, 0x4664}, {0x4748,
						   0x4748}};
	u32 tia_gain_mask[2] = {0x00ff0000, 0xff000000};

	HALBB_SCAN(input[3], DCMD_DECIMAL, &val[0]);
	if (val[0] > 1) {
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
				"Invalid Path index!\n");
		return;
	}

	path = (u8)val[0];

	if (_os_strcmp(input[2], "amode") == 0) {
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len -
			    *_used, "===[A mode][Path-%d]===\n", path);
		for (lna_idx = 0; lna_idx < 7; lna_idx++) {
			tmp = (s8)halbb_get_reg(bb, lna_gain_a[path][lna_idx],
						lna_gain_mask[lna_idx]);
			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used,
				    "[LNA]Addr=0x%x, Bit mask=0x%x, Value=%d\n",
				    lna_gain_a[path][lna_idx],
				    lna_gain_mask[lna_idx], tmp);
		}
		for (tia_idx = 0; tia_idx < 2; tia_idx++) {
			tmp = (s8)halbb_get_reg(bb, tia_gain_a[path][tia_idx],
						tia_gain_mask[tia_idx]);
			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used,
				    "[TIA]Addr=0x%x, Bit mask=0x%x, Value=%d\n",
				    tia_gain_a[path][tia_idx],
				    tia_gain_mask[tia_idx], tmp);
		}
	} else if (_os_strcmp(input[2], "gmode") == 0) {
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len -
			    *_used, "===[G mode][Path-%d]===\n", path);
		for (lna_idx = 0; lna_idx < 7; lna_idx++) {
			tmp = (s8)halbb_get_reg(bb, lna_gain_g[path][lna_idx],
						lna_gain_mask[lna_idx]);
			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used,
				    "[LNA]Addr=0x%x, Bit mask=0x%x, Value=%d\n",
				    lna_gain_g[path][lna_idx],
				    lna_gain_mask[lna_idx], tmp);
		}
		for (tia_idx = 0; tia_idx < 2; tia_idx++) {
			tmp = (s8)halbb_get_reg(bb, tia_gain_g[path][tia_idx],
						tia_gain_mask[tia_idx]);
			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used,
				    "[TIA]Addr=0x%x, Bit mask=0x%x, Value=%d\n",
				    tia_gain_g[path][tia_idx],
				    tia_gain_mask[tia_idx], tmp);
		}
	} else {
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len -
			    *_used,
			    "Please set correct mode {amode / gmode}\n");
	}
}

void halbb_rx_op1db_dbg_8852c(struct bb_info *bb, char input[][16],
			      u32 *_used, char *output, u32 *_out_len)
{
	struct bb_gain_info *gain = &bb->bb_gain_i;
	u32 val[10] = {0};
	u32 used = *_used;
	u32 out_len = *_out_len;
	u8 lna_idx = 0, path;
	s8 tmp = 0;
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

	HALBB_SCAN(input[3], DCMD_DECIMAL, &val[0]);
	if (val[0] > 1) {
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
				"Invalid Path index!\n");
		return;
	}

	path = (u8)val[0];

	if (_os_strcmp(input[2], "amode") == 0) {
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len -
			    *_used, "===[A mode][Path-%d]===\n", path);
		for (lna_idx = 0; lna_idx < 7; lna_idx++) {
			tmp = (s8)halbb_get_reg(bb, lna_op1db_a[path][lna_idx],
						op1db_a_mask[lna_idx]);
			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used,
				    "[LNA_op1db]Addr=0x%x, Bit mask=0x%x, Value=%d\n",
				    lna_op1db_a[path][lna_idx],
				    op1db_a_mask[lna_idx], tmp);
		}
		for (lna_idx = 0; lna_idx < 7; lna_idx++) {
			tmp = (s8)halbb_get_reg(bb,
						tia_lna_op1db_a[path][lna_idx],
						op1db_a_mask[lna_idx]);
			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used,
				    "[TIA_LNA_op1db]Addr=0x%x, Bit mask=0x%x, Value=%d\n",
				    tia_lna_op1db_a[path][lna_idx],
				    op1db_a_mask[lna_idx], tmp);
		}
		tmp = (s8)halbb_get_reg(bb,
					tia0_lna6_op1db_a[path], 0xff000000);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len -
			    *_used,
			    "[TIA_LNA_op1db]Addr=0x%x, Bit mask=0xff000000, Value=%d\n",
			    tia0_lna6_op1db_a[path], tmp);
	} else if (_os_strcmp(input[2], "gmode") == 0) {
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len -
			    *_used, "===[G mode][Path-%d]===\n", path);
		for (lna_idx = 0; lna_idx < 7; lna_idx++) {
			tmp = (s8)halbb_get_reg(bb, lna_op1db_g[path][lna_idx],
						op1db_g_mask[lna_idx]);
			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used,
				    "[LNA_op1db]Addr=0x%x, Bit mask=0x%x, Value=%d\n",
				    lna_op1db_g[path][lna_idx],
				    op1db_g_mask[lna_idx], tmp);
		}
		for (lna_idx = 0; lna_idx < 7; lna_idx++) {
			tmp = (s8)halbb_get_reg(bb,
						tia_lna_op1db_g[path][lna_idx],
						tia_lna_op1db_g_mask[lna_idx]);
			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used,
				    "[TIA_LNA_op1db]Addr=0x%x, Bit mask=0x%x, Value=%d\n",
				    tia_lna_op1db_g[path][lna_idx],
				    tia_lna_op1db_g_mask[lna_idx], tmp);
		}
		tmp = (s8)halbb_get_reg(bb, tia0_lna6_op1db_g[path], 0xff00);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len -
			    *_used,
			    "[TIA_LNA_op1db]Addr=0x%x, Bit mask=0xff00, Value=%d\n",
			    tia0_lna6_op1db_g[path], tmp);
	} else {
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len -
			    *_used,
			    "Please set correct mode {amode / gmode}\n");
	}
}

void halbb_clk_en_8852c(struct bb_info *bb, bool en, enum phl_phy_idx phy_idx)
{
	if (en) {
		halbb_set_reg_cmn(bb, 0x704, BIT(1), 1, phy_idx);
		halbb_set_reg(bb, 0x0, BIT(2), 1);
	} else {
		halbb_set_reg_cmn(bb, 0x704, BIT(1), 0, phy_idx);
		halbb_set_reg(bb, 0x0, BIT(2), 0);
	}
}

void halbb_pwr_en_8852c(struct bb_info *bb, bool en)
{
	if (en) {
		rtw_hal_mac_write_msk_pwr_reg(bb->hal_com, HW_PHY_0, 0x20, 0x3ff, 0x3ff);
		rtw_hal_mac_write_msk_pwr_reg(bb->hal_com, HW_PHY_0, 0x80, BIT(7), 0);
	} else {
		rtw_hal_mac_write_msk_pwr_reg(bb->hal_com, HW_PHY_0, 0x20, 0x3ff, 0x0);
		rtw_hal_mac_write_msk_pwr_reg(bb->hal_com, HW_PHY_0, 0x80, BIT(7), 1);
	}
}
#endif

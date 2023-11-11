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
#include "../halrf_precomp.h"
#ifdef RF_8852C_SUPPORT

/*8852C DPK ver:0x10 20221123*/

void _dpk_bkup_kip_8852c(
	struct rf_info *rf,
	u32 *reg,
	u32 reg_bkup[][DPK_KIP_REG_NUM_8852C],
	u8 path)
{
	u8 i;

	for (i = 0; i < DPK_KIP_REG_NUM_8852C; i++) {
		reg_bkup[path][i] = halrf_rreg(rf, reg[i] + (path << 8), MASKDWORD);
		if (DPK_REG_DBG)
			RF_DBG(rf, DBG_RF_DPK, "[DPK] Backup 0x%x = %x\n", reg[i]+ (path << 8), reg_bkup[path][i]);
	}
}

void _dpk_bkup_bb_8852c(
	struct rf_info *rf,
	u32 *reg,
	u32 reg_bkup[DPK_BB_REG_NUM_8852C])
{
	u8 i;

	for (i = 0; i < DPK_BB_REG_NUM_8852C; i++) {
		reg_bkup[i] = halrf_rreg(rf, reg[i], MASKDWORD);
		if (DPK_REG_DBG)
			RF_DBG(rf, DBG_RF_DPK, "[DPK] Backup 0x%x = %x\n", reg[i], reg_bkup[i]);
	}
}

void _dpk_bkup_rf_8852c(
	struct rf_info *rf,
	u32 *rf_reg,
	u32 rf_bkup[][DPK_RF_REG_NUM_8852C],
	u8 path)
{
	u8 i;

	for (i = 0; i < DPK_RF_REG_NUM_8852C; i++) {
		rf_bkup[path][i] = halrf_rrf(rf, path, rf_reg[i], MASKRF);
		if (DPK_REG_DBG)
			RF_DBG(rf, DBG_RF_DPK, "[DPK] Backup RF S%d 0x%x = %x\n",
				path, rf_reg[i], rf_bkup[path][i]);
	}
}

void _dpk_reload_kip_8852c(
	struct rf_info *rf,
	u32 *reg,
	u32 reg_bkup[][DPK_KIP_REG_NUM_8852C],
	u8 path) 
{
	u8 i;

	for (i = 0; i < DPK_KIP_REG_NUM_8852C; i++) {
		halrf_wreg(rf, reg[i] + (path << 8), MASKDWORD, reg_bkup[path][i]);
		if (DPK_REG_DBG)
			RF_DBG(rf, DBG_RF_DPK, "[DPK] Reload 0x%x = %x\n", reg[i] + (path << 8),
				   reg_bkup[path][i]);
	}
}

void _dpk_reload_bb_8852c(
	struct rf_info *rf,
	u32 *reg,
	u32 reg_bkup[DPK_BB_REG_NUM_8852C]) 
{
	u8 i;

	for (i = 0; i < DPK_BB_REG_NUM_8852C; i++) {
		halrf_wreg(rf, reg[i], MASKDWORD, reg_bkup[i]);
		if (DPK_REG_DBG)
			RF_DBG(rf, DBG_RF_DPK, "[DPK] Reload 0x%x = %x\n", reg[i],
				   reg_bkup[i]);
	}
}

void _dpk_reload_rf_8852c(
	struct rf_info *rf,
	u32 *rf_reg,
	u32 rf_bkup[][DPK_RF_REG_NUM_8852C],
	u8 path)
{
	u8 i;

	for (i = 0; i < DPK_RF_REG_NUM_8852C; i++) {
		halrf_wrf(rf, path, rf_reg[i], MASKRF, rf_bkup[path][i]);
		if (DPK_REG_DBG)
			RF_DBG(rf, DBG_RF_DPK, "[DPK] Reload RF S%d 0x%x = %x\n",
				path, rf_reg[i], rf_bkup[path][i]);
	}
}

u8 _dpk_one_shot_8852c(
	struct rf_info *rf,
	enum phl_phy_idx phy,
	enum rf_path path,
	enum dpk_id id)
{
	u8 phy_map;
	u16 dpk_cmd = 0x0;
	//u32 r_bff8 = 0x0, r_80fc = 0x0, cnt1 = 0, cnt2 = 0;

	phy_map = (BIT(phy) << 4) | BIT(path);

	dpk_cmd = (u16)((id << 8) | (0x19 + path * 0x12));
#if 0
	halrf_btc_rfk_ntfy(rf, phy_map, RF_BTC_DPK, RFK_ONESHOT_START);
#endif
#if 0
	halrf_wreg(rf, 0x8000, MASKDWORD, dpk_cmd);

	r_bff8 = halrf_rreg(rf, 0xbff8, MASKBYTE0);
	while (r_bff8 != 0x55 && cnt1 < 2000) {
		halrf_delay_us(rf, 10);
		r_bff8 = halrf_rreg(rf, 0xbff8, MASKBYTE0);
		cnt1++;
	}

	halrf_delay_us(rf, 1);

	r_80fc = halrf_rreg(rf, 0x80fc, MASKLWORD);
	while (r_80fc != 0x8000 && cnt2 < 2000) {
		halrf_delay_us(rf, 1);
		r_80fc = halrf_rreg(rf, 0x80fc, MASKLWORD);
		cnt2++;
	}

	halrf_wreg(rf, 0x8010, MASKBYTE0, 0x0);
#endif
	halrf_do_one_shot_8852c(rf, path, 0x8000, MASKDWORD, dpk_cmd);
#if 0
	halrf_btc_rfk_ntfy(rf, phy_map, RF_BTC_DPK, RFK_ONESHOT_STOP);
#endif
	RF_DBG(rf, DBG_RF_DPK, "[DPK] one-shot for %s = 0x%04x (cnt:%d/%d)\n",
	       id == 0x28 ? "KIP_PRESET" : (id == 0x29 ? "DPK_TXAGC" :
	       (id == 0x2a ? "DPK_RXAGC" : (id == 0x2b ? "SYNC" :
	       	(id == 0x2c ? "GAIN_LOSS" : (id == 0x2d ? "MDPK_IDL" :
		(id == 0x2f ? "DPK_GAIN_NORM" : (id == 0x31 ? "KIP_RESOTRE" :
		(id == 0x6 ? "LBK_RXIQK" : "Unknown id")))))))), dpk_cmd,
			rf->nctl_ck_times[0], rf->nctl_ck_times[1]);

	if (rf->nctl_ck_times[0] == 2000 || rf->nctl_ck_times[1] == 2000) {
		RF_DBG(rf, DBG_RF_DPK, "[DPK] one-shot over 20ms!!!!\n");
		return 1;
	} else
		return 0;
}

void _dpk_init_8852c(
	struct rf_info *rf,
	enum rf_path path)
{
	struct halrf_dpk_info *dpk = &rf->dpk;

	u8 kidx = dpk->cur_idx[path];

	dpk->bp[path][kidx].path_ok = 0;
	dpk->ov_flag[path] = 0;
}

void _dpk_information_8852c(
	struct rf_info *rf,
	enum phl_phy_idx phy,
	enum rf_path path)
{
	struct halrf_dpk_info *dpk = &rf->dpk;

	u8 kidx = dpk->cur_idx[path];

	dpk->bp[path][kidx].band = rf->hal_com->band[phy].cur_chandef.band;
	dpk->bp[path][kidx].ch = rf->hal_com->band[phy].cur_chandef.center_ch;
	dpk->bp[path][kidx].bw = rf->hal_com->band[phy].cur_chandef.bw;

	RF_DBG(rf, DBG_RF_DPK, "[DPK] S%d[%d] (PHY%d): TSSI %s/ DBCC %s/ %s/ CH%d/ %s\n",
	       path, dpk->cur_idx[path], phy, rf->is_tssi_mode[path] ? "on" : "off",
	       rf->hal_com->dbcc_en ? "on" : "off",
	       dpk->bp[path][kidx].band == 0 ? "2G" : (dpk->bp[path][kidx].band == 1 ? "5G" : "6G"),
	       dpk->bp[path][kidx].ch,
	       dpk->bp[path][kidx].bw == 0 ? "20M" : (dpk->bp[path][kidx].bw == 1 ? "40M" :
	       (dpk->bp[path][kidx].bw == 2 ? "80M" : "160M")));
}

void _dpk_rxagc_onoff_8852c(
	struct rf_info *rf,
	enum rf_path path,
	bool turn_on)
{
	if (path == RF_PATH_A)
		halrf_wreg(rf, 0x4730, BIT(31), turn_on);
	else
		halrf_wreg(rf, 0x4a9c, BIT(31), turn_on);

	RF_DBG(rf, DBG_RF_DPK, "[DPK] S%d RXAGC is %s\n", path,
		turn_on ? "turn_on" : "turn_off");
}

void _dpk_bb_afe_setting_8852c(
	struct rf_info *rf,
	enum rf_path path)
{
	/*1. Keep ADC_fifo reset*/
	halrf_wreg(rf, 0x20fc, BIT(16 + path), 0x1);
	halrf_wreg(rf, 0x20fc, BIT(20 + path), 0x0);
	halrf_wreg(rf, 0x20fc, BIT(24 + path), 0x1);
	halrf_wreg(rf, 0x20fc, BIT(28 + path), 0x0);
	/*2. BB for IQK DBG mode*/
	halrf_wreg(rf, 0x5670 + (path << 13), MASKDWORD, 0xd801dffd); /*bit13 for gapk_offset*/
	/*3.Set DAC clk*/
	halrf_txck_force_8852c(rf, path, true, DAC_960M);
	/*4. Set ADC clk*/
	halrf_rxck_force_8852c(rf, path, true, ADC_1920M);
	//halrf_wreg(rf, 0x5670 + (path << 13), BIT(30) | BIT(29), 0x2);

	halrf_wreg(rf, 0xc0d4 + (path << 8), BIT(27) | BIT(26), 0x1);
	halrf_wreg(rf, 0xc0d8 + (path << 8), 0x000001E0, 0xb); /*[8:5]*/
	halrf_wreg(rf, 0x12b8 + (path << 13), BIT(30), 0x1);
	halrf_wreg(rf, 0x030c, MASKBYTE3, 0x1f);
	halrf_wreg(rf, 0x030c, MASKBYTE3, 0x13);
	halrf_wreg(rf, 0x032c, MASKHWORD, 0x0001);
	halrf_wreg(rf, 0x032c, MASKHWORD, 0x0041);
	/*5. ADDA fifo rst*/
	halrf_wreg(rf, 0x20fc, BIT(20 + path), 0x1);
	halrf_wreg(rf, 0x20fc, BIT(28 + path), 0x1);

	RF_DBG(rf, DBG_RF_DPK, "[DPK] S%d BB/AFE setting\n", path);
}

void _dpk_bb_afe_restore_8852c(
	struct rf_info *rf,
	enum rf_path path)
{
	halrf_wreg(rf, 0x12b8 + (path << 13), BIT(30), 0x0);
	halrf_wreg(rf, 0x20fc, BIT(16 + path), 0x1);
	halrf_wreg(rf, 0x20fc, BIT(20 + path), 0x0);
	halrf_wreg(rf, 0x20fc, BIT(24 + path), 0x1);
	halrf_wreg(rf, 0x20fc, BIT(28 + path), 0x0);
	halrf_wreg(rf, 0x5670 + (path << 13), MASKDWORD, 0x00000000);
	halrf_wreg(rf, 0x12a0 + (path << 13), 0x000FF000, 0x00); /*[19:12]*/
	halrf_wreg(rf, 0x20fc, BIT(16 + path), 0x0);
	halrf_wreg(rf, 0x20fc, BIT(24 + path), 0x0);

	RF_DBG(rf, DBG_RF_DPK, "[DPK] S%d BB/AFE restore\n", path);
}

void _dpk_tssi_pause_8852c(
	struct rf_info *rf,
	enum rf_path path,
	bool is_pause)
{
	halrf_wreg(rf, 0x5818 + (path << 13), BIT(30), is_pause);

	RF_DBG(rf, DBG_RF_DPK, "[DPK] S%d TSSI %s\n", path,
	       is_pause ? "pause" : "resume");
}

void _dpk_tpg_sel_8852c(
	struct rf_info *rf,
	enum rf_path path,
	u8 kidx)
{
	struct halrf_dpk_info *dpk = &rf->dpk;

	if (dpk->bp[path][kidx].bw == CHANNEL_WIDTH_160) {
		halrf_wreg(rf, 0x806c, BIT(2) | BIT (1), 0x3);
		halrf_wreg(rf, 0x8068, MASKDWORD, 0x0180ff30);
	} else if (dpk->bp[path][kidx].bw == CHANNEL_WIDTH_80) {
		halrf_wreg(rf, 0x806c, BIT(2) | BIT (1), 0x0);
		halrf_wreg(rf, 0x8068, MASKDWORD, 0xffe0fa00);
	} else if (dpk->bp[path][kidx].bw == CHANNEL_WIDTH_40) {
		halrf_wreg(rf, 0x806c, BIT(2) | BIT (1), 0x2);
		halrf_wreg(rf, 0x8068, MASKDWORD, 0xff4009e0);
	} else {
		halrf_wreg(rf, 0x806c, BIT(2) | BIT (1), 0x1);
		halrf_wreg(rf, 0x8068, MASKDWORD, 0xf9f007d0);
	}

	RF_DBG(rf, DBG_RF_DPK, "[DPK] TPG Select for %s\n",
	       dpk->bp[path][kidx].bw == CHANNEL_WIDTH_160 ? "160M" :
	       (dpk->bp[path][kidx].bw == CHANNEL_WIDTH_80 ? "80M" : 
	       (dpk->bp[path][kidx].bw == CHANNEL_WIDTH_40 ? "40M" : "20M")));
}

void _dpk_txpwr_bb_force_8852c(
	struct rf_info *rf,
	enum rf_path path,
	bool is_force)
{
	halrf_wreg(rf, 0x56cc + (path << 13), BIT(28), is_force); /*txpwr_bb_force_on*/
	halrf_wreg(rf, 0x580c + (path << 13), BIT(15), is_force); /*txpwr_bb_force_rdy*/

	RF_DBG(rf, DBG_RF_DPK, "[DPK] S%d txpwr_bb_force %s\n", path, is_force ? "on" : "off");
}

void _dpk_kip_pwr_clk_onoff_8852c(
	struct rf_info *rf,
	bool turn_on)
{
	if (turn_on) {
		halrf_wreg(rf, 0x8008, MASKDWORD, 0x00000080); /*cip power on*/
		halrf_wreg(rf, 0x8088, MASKDWORD, 0x807f030a); /*320M*/
	} else {
		halrf_wreg(rf, 0x8008, MASKDWORD, 0x00000000);
		halrf_wreg(rf, 0x8088, MASKDWORD, 0x80000000);
		halrf_wreg(rf, 0x80f4, BIT(18), 0x1);
	}
	//RF_DBG(rf, DBG_RF_DPK, "[DPK] KIP Power/CLK is %s\n", turn_on ? "turn_on" : "turn_off");
}

void _dpk_kip_control_rfc_8852c(
	struct rf_info *rf,
	enum rf_path path,
	bool ctrl_by_kip)
{
	halrf_wreg(rf, 0x5670 + (path << 13), BIT(1), ctrl_by_kip); /*KIP control RFC*/

	RF_DBG(rf, DBG_RF_DPK, "[DPK] RFC is controlled by %s\n", ctrl_by_kip ? "KIP" : "BB");
}

void _dpk_kip_preset_8852c(
	struct rf_info *rf,
	enum phl_phy_idx phy,
	enum rf_path path,
	u8 kidx)
{
	struct halrf_dpk_info *dpk = &rf->dpk;

	/*cip power on*/
	//halrf_wreg(rf, 0x8008, MASKDWORD, 0x00000080);	
	/*320M*/
	//halrf_wreg(rf, 0x8088, MASKDWORD, 0x807f030a);

	/*must before kip control RFC*/
	if (dpk->bp[path][kidx].band == BAND_ON_24G)
		halrf_wreg(rf, 0x8078, 0x000FFFFF, 0x50121 | BIT(rf->hal_com->dbcc_en)); /*[19:0]*/
	else
		halrf_wreg(rf, 0x8078, 0x000FFFFF, 0x50101 | BIT(rf->hal_com->dbcc_en));
	//RF_DBG(rf, DBG_RF_DPK, "[DPK] 0x8078 = 0x%x\n", halrf_rreg(rf, 0x8078, MASKDWORD));

	if (rf->hal_com->cv == CAV)
		halrf_wreg(rf, 0x81bc + (path << 8) + (kidx << 2), 0x00003F00, 0x01); /*[13:8] thermal slope*/
	else
		halrf_wreg(rf, 0x81bc + (path << 8) + (kidx << 2), 0x00003F00, 0x0c); /*[13:8] thermal slope*/
	_dpk_kip_control_rfc_8852c(rf, path, true);
	halrf_wreg(rf, 0x8104 + (path << 8), BIT(8), kidx); /*ch0/ch1 selection*/

	//halrf_wreg(rf, 0x8078, 0x000FFFFF, 0x50121); /*[19:0]*/

	//halrf_wreg(rf, 0x8010, MASKBYTE0, 0x0);

	_dpk_one_shot_8852c(rf, phy, path, D_KIP_PRESET);
}

void _dpk_kip_restore_8852c(
	struct rf_info *rf,
	enum phl_phy_idx phy,
	enum rf_path path)
{
	_dpk_one_shot_8852c(rf, phy, path, D_KIP_RESTORE);

	/*cip power on*/
	//halrf_wreg(rf, 0x8008, MASKDWORD, 0x00000000);
	/*CFIR CLK restore*/
	//halrf_wreg(rf, 0x8088, MASKDWORD, 0x80000000);

	_dpk_kip_control_rfc_8852c(rf, path, false);
	_dpk_txpwr_bb_force_8852c(rf, path, false);
#if 0
	if (rf->hal_com->cv > 0x0) /*hw txagc_offset*/
		halrf_wreg(rf, 0x81c8 + (path << 8), BIT(15), 0x1);
#endif
	RF_DBG(rf, DBG_RF_DPK, "[DPK] S%d restore KIP\n", path);
}

u8 _dpk_dbm_convert_8852c(
	struct rf_info *rf,
	u8 dbm)
{
	u8 txagc_cw;

	txagc_cw = (dbm - 16) * 4 + 0x40; /*0.25dB/step*/

	RF_DBG(rf, DBG_RF_DPK, "[DPK] convert %ddBm to 0x%x\n", dbm, txagc_cw);

	return txagc_cw;
}

void _dpk_read_rxsram_8852c(
	struct rf_info *rf)
{
	u32 addr;

	halrf_wreg(rf, 0x80e8, BIT(7), 0x1);	/*web_iqrx*/
	halrf_wreg(rf, 0x8074, BIT(31), 0x1);	/*rxsram_ctrl_sel*/
	halrf_wreg(rf, 0x80d4, MASKDWORD, 0x00020000);	/*rpt_sel*/

	for (addr = 0; addr < 0x200; addr++) {
		halrf_wreg(rf, 0x80d8, MASKDWORD, 0x00010000 | addr);
		RF_DBG(rf, DBG_RF_DPK, "[DPK] RXSRAM[%03d] = 0x%07x\n", addr,
			halrf_rreg(rf, 0x80fc, MASKDWORD));
	}
	halrf_wreg(rf, 0x80e8, BIT(7), 0x0);	/*web_iqrx*/
	halrf_wreg(rf, 0x8074, BIT(31), 0x0);	/*rxsram_ctrl_sel*/
}

void _dpk_rf_reg_query_8852c(
	struct rf_info *rf,
	enum rf_path path,
	u32 reg)
{
#if 1
	u32 ori_ctrl = halrf_rreg(rf, 0x5670 + (path << 13), BIT(1));

	_dpk_kip_control_rfc_8852c(rf, path, false);

	RF_DBG(rf, DBG_RF_DPK, "[DPK] RF 0x%x = 0x%x\n", reg,
		halrf_rrf(rf, path, reg, MASKRF));

	_dpk_kip_control_rfc_8852c(rf, path, (bool)ori_ctrl);
#endif
}

void _dpk_kset_query_8852c(
	struct rf_info *rf,
	enum rf_path path)
{
	struct halrf_dpk_info *dpk = &rf->dpk;

#ifdef HALRF_CONFIG_FW_IO_OFLD_SUPPORT
	dpk->cur_k_set = 2;
#else
	halrf_wreg(rf, 0x81d4 + (path << 8), 0x003F0000, 0x10);	/*rpt_sel*/

	dpk->cur_k_set = (u8)(halrf_rreg(rf, 0x81fc + (path << 8), 0xE0000000) - 1); /*[31:29]*/
#endif
	/*RF_DBG(rf, DBG_RF_DPK, "[DPK] cur k_set = %d\n", dpk->cur_k_set);*/
}

void _dpk_para_query_8852c(
	struct rf_info *rf,
	enum rf_path path,
	u8 kidx)
{
	/*[31:26]:t-meter, [25:16]:txagc_bb, [15:7]:txagc, [6:0]:gs*/
	struct halrf_dpk_info *dpk = &rf->dpk;

	u32 reg[2][4] = {{0x8190, 0x8194, 0x8198, 0x81a4},
			 {0x81a8, 0x81c4, 0x81c8, 0x81e8}};
	u32 para;

	para = halrf_rreg(rf, reg[kidx][dpk->cur_k_set] + (path << 8), MASKDWORD);

	dpk->bp[path][kidx].txagc_dpk = (para >> 10) & 0x3f;
	dpk->bp[path][kidx].ther_dpk = (para >> 26) & 0x3f;

	RF_DBG(rf, DBG_RF_DPK, "[DPK] thermal/ txagc_RF (K%d) = 0x%x/ 0x%x\n",
		dpk->cur_k_set, dpk->bp[path][kidx].ther_dpk, dpk->bp[path][kidx].txagc_dpk);
#if 0
	if (kidx == 0) { /*CH0*/
		RF_DBG(rf, DBG_RF_DPK, "[DPK] CH0_K0= 0x%x\n",
			halrf_rreg(rf, 0x8190 + (path << 8), MASKDWORD));
		RF_DBG(rf, DBG_RF_DPK, "[DPK] CH0_K1= 0x%x\n",
			halrf_rreg(rf, 0x8194 + (path << 8), MASKDWORD));
		RF_DBG(rf, DBG_RF_DPK, "[DPK] CH0_K2= 0x%x\n",
			halrf_rreg(rf, 0x8198 + (path << 8), MASKDWORD));
		RF_DBG(rf, DBG_RF_DPK, "[DPK] CH0_K3= 0x%x\n",
			halrf_rreg(rf, 0x81a4 + (path << 8), MASKDWORD));
	} else { /*CH1*/
		RF_DBG(rf, DBG_RF_DPK, "[DPK] CH0_K0= 0x%x\n",
			halrf_rreg(rf, 0x81a8 + (path << 8), MASKDWORD));
		RF_DBG(rf, DBG_RF_DPK, "[DPK] CH0_K1= 0x%x\n",
			halrf_rreg(rf, 0x81c4 + (path << 8), MASKDWORD));
		RF_DBG(rf, DBG_RF_DPK, "[DPK] CH0_K2= 0x%x\n",
			halrf_rreg(rf, 0x81c8 + (path << 8), MASKDWORD));
		RF_DBG(rf, DBG_RF_DPK, "[DPK] CH0_K3= 0x%x\n",
			halrf_rreg(rf, 0x81e8 + (path << 8), MASKDWORD));
	}
#endif
}

bool _dpk_sync_check_8852c(
	struct rf_info *rf,
	enum rf_path path,
	u8 kidx)
{
	struct halrf_dpk_info *dpk = &rf->dpk;

	u16 dc_i, dc_q;
	u8 corr_val, corr_idx, rxbb;
	u32 corr, dc;

	halrf_wreg(rf, 0x80d4, 0x003F0000, 0x0);	/*rpt_sel*/

	corr = halrf_rreg(rf, 0x80fc, MASKDWORD);
	corr_idx = corr & 0xff;
	corr_val = (corr & 0xff00) >> 8;

	dpk->corr_idx[path][kidx] = corr_idx;
	dpk->corr_val[path][kidx] = corr_val;

	halrf_wreg(rf, 0x80d4, 0x003F0000, 0x9);	/*rpt_sel*/

	dc = halrf_rreg(rf, 0x80fc, MASKDWORD);
	dc_i = (dc & 0x0fff0000) >> 16; /*[27:16]*/
	dc_q = dc & 0x00000fff; /*[11:0]*/

	if (dc_i >> 11 == 1)
		dc_i = 0x1000 - dc_i;
	if (dc_q >> 11 == 1)
		dc_q = 0x1000 - dc_q;

	RF_DBG(rf, DBG_RF_DPK, "[DPK] S%d Corr_idx/ Corr_val /DC I/Q, = %d / %d / %d / %d\n",
		path, corr_idx, corr_val, dc_i, dc_q);

	dpk->dc_i[path][kidx] = dc_i;
	dpk->dc_q[path][kidx] = dc_q;

	halrf_wreg(rf, 0x80d4, 0x003F0000, 0x8);	/*rpt_sel*/

	rxbb = (u8)halrf_rreg(rf, 0x80fc, 0x0000003F);	/*[5:0]*/

	//_dpk_rf_reg_query_8852c(rf, path, 0x00);

	halrf_wreg(rf, 0x80d4, 0x003F0000, 0x31);	/*rpt_sel*/

	dpk->rxbb_ov[path] = (u8)halrf_rreg(rf, 0x80fc, BIT(8));

	RF_DBG(rf, DBG_RF_DPK, "[DPK] S%d RXBB/ RXAGC_done /RXBB_ovlmt = %d / %d / %d\n",
		path, rxbb, halrf_rreg(rf, 0x80fc, BIT(0)), dpk->rxbb_ov[path]);

	if ((dc_i > 200) || (dc_q > 200) || (corr_val < 170))
		return true;
	else
		return false;
}

void _dpk_kip_set_txagc_8852c(
	struct rf_info *rf,
	enum phl_phy_idx phy,
	enum rf_path path,
	u8 dbm,
	bool set_from_bb)
{	
	if (set_from_bb) {
		if (dbm >= 24)
			dbm = 24;
		else if (dbm <= 7)
			dbm = 7;
		RF_DBG(rf, DBG_RF_DPK, "[DPK] set S%d txagc to %ddBm\n", path, dbm);
		halrf_wreg(rf, 0x56cc + (path << 13), 0x0FF80000, dbm << 2); /*[27:19]*/
	}
	_dpk_one_shot_8852c(rf, phy, path, D_TXAGC);
	_dpk_kset_query_8852c(rf, path);
	//_dpk_rf_reg_query_8852c(rf, path, 0x11);
}

void _dpk_kip_set_rxagc_8852c(
	struct rf_info *rf,
	enum phl_phy_idx phy,
	enum rf_path path,
	u8 kidx)
{
	_dpk_kip_control_rfc_8852c(rf, path, false);
	halrf_wreg(rf, 0x8078, 0x000FFFFF, halrf_rrf(rf, path, 0x00, MASKRF)); /*[19:0]*/
	_dpk_kip_control_rfc_8852c(rf, path, true);

	_dpk_one_shot_8852c(rf, phy, path, D_RXAGC);
#if 0
	halrf_wreg(rf, 0x80d4, 0x000F0000, 0x8);
	RF_DBG(rf, DBG_RF_DPK, "[DPK] set RXBB = 0x%x\n",
		halrf_rreg(rf, 0x80fc, 0x0000001F));
#endif
}

void _dpk_lbk_rxiqk_8852c(
	struct rf_info *rf,
	enum phl_phy_idx phy,
	enum rf_path path)
{
	u8 cur_rxbb;
	u32 rf_11, reg_81cc;
	
	halrf_wreg(rf, 0x81a0 + (path << 8), BIT(7), 0x1); /*Rx-IQC switch to MDPK-mode*/
	halrf_wreg(rf, 0x8074, BIT(31), 0x1); /*RxSRAM_ctrl_sel 0:MDPK; 1:IQK*/

	_dpk_kip_control_rfc_8852c(rf, path, false);
	/*RF setting*/
	cur_rxbb = (u8)halrf_rrf(rf, path, 0x00, MASKRFRXBB);
	rf_11 = halrf_rrf(rf, path, 0x11, MASKRF);
	reg_81cc = halrf_rreg(rf, 0x81cc + (path << 8), BIT(13) | BIT(12));
	
	halrf_wrf(rf, path, 0x11, BIT(1) | BIT(0), 0x0);
	halrf_wrf(rf, path, 0x11, BIT(6) | BIT(5) | BIT(4), 0x3);
	halrf_wrf(rf, path, 0x11, 0x1F000, 0xd); /*[16:12]*/
	halrf_wrf(rf, path, 0x00, MASKRFRXBB, 0x1f); /*[9:5]*/

	halrf_wreg(rf, 0x81cc + (path << 8), 0x0000003F, 0x12); /*[5:0]*/
	halrf_wreg(rf, 0x81cc + (path << 8), BIT(13) | BIT(12), 0x3);

	_dpk_kip_control_rfc_8852c(rf, path, true);

	halrf_wreg(rf, 0x802c, MASKDWORD, 0x00250025); /*Rx_tone_idx=0x025 (9.25MHz)*/

	_dpk_one_shot_8852c(rf, phy, path, LBK_RXIQK);

#ifndef HALRF_CONFIG_FW_IO_OFLD_SUPPORT
	RF_DBG(rf, DBG_RF_DPK, "[DPK] S%d LBK RXIQC = 0x%x\n", path,
		halrf_rreg(rf, 0x813c + (path << 8), MASKDWORD));
#endif
	/*restore*/
	_dpk_kip_control_rfc_8852c(rf, path, false);

	halrf_wrf(rf, path, 0x11, MASKRF, rf_11);
	halrf_wrf(rf, path, 0x00, MASKRFRXBB, cur_rxbb);
	halrf_wreg(rf, 0x81cc + (path << 8), BIT(13) | BIT(12), reg_81cc);

	halrf_wreg(rf, 0x8074, BIT(31), 0x0); /*RxSRAM_ctrl_sel 0:MDPK; 1:IQK*/
	halrf_wreg(rf, 0x80d0, BIT(21) | BIT(20), 0x0);
	halrf_wreg(rf, 0x81dc + (path << 8), BIT(1), 0x1); /*auto*/

	_dpk_kip_control_rfc_8852c(rf, path, true);
}

void _dpk_get_thermal_8852c(struct rf_info *rf, u8 kidx, enum rf_path path)
{
	struct halrf_dpk_info *dpk = &rf->dpk;

	u32 reg[2][4] = {{0x8190, 0x8194, 0x8198, 0x81a4},
			 {0x81a8, 0x81c4, 0x81c8, 0x81e8}};

	dpk->bp[path][kidx].ther_dpk = halrf_get_thermal_8852c(rf, path);

	halrf_wreg(rf, reg[kidx][dpk->cur_k_set] + (path << 8), 0xFC000000,
			dpk->bp[path][kidx].ther_dpk); /*[31:26]*/

	RF_DBG(rf, DBG_RF_DPK, "[DPK] thermal@DPK (by driver)= 0x%x\n", dpk->bp[path][kidx].ther_dpk);
}

void _dpk_rf_setting_8852c(
	struct rf_info *rf,
	enum rf_path path,
	u8 kidx)
{
	struct halrf_dpk_info *dpk = &rf->dpk;

	if (dpk->bp[path][kidx].band == BAND_ON_24G) { /*2G*/
		halrf_wrf(rf, path, 0x00, MASKRF, 0x50121 | BIT(rf->hal_com->dbcc_en));
		halrf_wrf(rf, path, 0x10000, MASKRFMODE, RF_DPK);
		/*att*/
		halrf_wrf(rf, path, 0x83, 0x00007, 0x2); /*[2:0] ATT 0~7 : -24~-36 dB*/
		halrf_wrf(rf, path, 0x83, 0x000F0, 0x4); /*[7:4] R1  0~15: -23~-1 dB*/
		/*TIA*/
		halrf_wrf(rf, path, 0xdf, BIT(12), 0x1);
		halrf_wrf(rf, path, 0x9e, BIT(8), 0x1); /*0:26dB, 1:20dB*/
#if 0
		RF_DBG(rf, DBG_RF_DPK, "[DPK] RF 0x0/0x83/0x9e/0x1a/0xdf/0x1001a = 0x%x/ 0x%x/ 0x%x/ 0x%x/ 0x%x/ 0x%x\n",
		       halrf_rrf(rf, path, 0x00, MASKRF),
		       halrf_rrf(rf, path, 0x83, MASKRF),
		       halrf_rrf(rf, path, 0x9e, MASKRF),
		       halrf_rrf(rf, path, 0x1a, MASKRF),
		       halrf_rrf(rf, path, 0xdf, MASKRF),
		       halrf_rrf(rf, path, 0x1001a, MASKRF));
#endif
	} else { /*5G /6G*/
		halrf_wrf(rf, path, 0x00, MASKRF, 0x50101 | BIT(rf->hal_com->dbcc_en));
		halrf_wrf(rf, path, 0x10000, MASKRFMODE, RF_DPK);
#if 0
		halrf_wrf(rf, path, 0x97, 0x00F00, 0x1); /*[11:8]*/
		halrf_wrf(rf, path, 0xa3, 0xF0000, 0xf); /*[19:16]*/
#else
		if (dpk->bp[path][kidx].band == BAND_ON_6G &&
			dpk->bp[path][kidx].ch >= 161)
			halrf_wrf(rf, path, 0x97, 0x00F00, 0x8); /*[11:8]*/
#endif
		halrf_wrf(rf, path, 0xa3, 0xF0000, 0xd); /*[19:16]*/
		halrf_wrf(rf, path, 0x5f, 0x0000F, 0x8); /*[3:0] TX_IQGEN_LDO*/

		/*switch + att*/
		halrf_wrf(rf, path, 0x8c, 0x0000F, 0x0); /*[3:0] ATT 0/1/2 : -38/-32/-26 dB*/
		halrf_wrf(rf, path, 0x98, 0x0F000, 0x3); /*[15:12] R1  1/2/3/4 : -12/-5/-6/0 dB*/
		/*TIA*/
		halrf_wrf(rf, path, 0xdf, BIT(12), 0x1);
		halrf_wrf(rf, path, 0x9e, BIT(8), 0x1); /*0:26dB, 1:20dB*/

		if (dpk->bp[path][kidx].bw == CHANNEL_WIDTH_160)
			halrf_wrf(rf, path, 0x8f, BIT(6) | BIT(5), 0x0); /*RXBB BW Extend*/
#if 0
		RF_DBG(rf, DBG_RF_DPK, "[DPK] RF 0x0/0x8c/0x8f/0x97/0x98/0x9e/0x1a/0xa3/0xdf/0x1001a = 0x%x/ 0x%x/ 0x%x/ 0x%x/ 0x%x/ 0x%x/ 0x%x/ 0x%x/ 0x%x/ 0x%x\n",
		       halrf_rrf(rf, path, 0x00, MASKRF),
		       halrf_rrf(rf, path, 0x8c, MASKRF),
		       halrf_rrf(rf, path, 0x8f, MASKRF),
		       halrf_rrf(rf, path, 0x97, MASKRF),
		       halrf_rrf(rf, path, 0x98, MASKRF),
		       halrf_rrf(rf, path, 0x9e, MASKRF),
		       halrf_rrf(rf, path, 0x1a, MASKRF),
		       halrf_rrf(rf, path, 0xa3, MASKRF),
		       halrf_rrf(rf, path, 0xdf, MASKRF),
		       halrf_rrf(rf, path, 0x1001a, MASKRF));
#endif
	}
	/*debug rtxbw*/
	//halrf_wrf(rf, path, 0xde, BIT(2), 0x1);
	/*txbb filter*/
	//halrf_wrf(rf, path, 0x1a, BIT(14) | BIT(13) | BIT(12), dpk->bp[path][kidx].bw + 1);
	/*rxbb filter*/
	//halrf_wrf(rf, path, 0x1a, BIT(11) | BIT(10), 0x0);
}

void _dpk_bypass_rxiqc_8852c(
	struct rf_info *rf,
	enum rf_path path)
{
	halrf_wreg(rf, 0x81a0 + (path << 8), BIT(7), 0x1);
	halrf_wreg(rf, 0x813c + (path << 8), MASKDWORD, 0x40000002);
	RF_DBG(rf, DBG_RF_DPK, "[DPK] Bypass RXIQC\n");
}

u16 _dpk_dgain_read_8852c(
	struct rf_info *rf)
{
	u16 dgain = 0x0;

	halrf_wreg(rf, 0x80d4, 0x003F0000, 0x0);	/*rpt_sel*/

	dgain = (u16)halrf_rreg(rf, 0x80fc, 0x0FFF0000);	/*[27:16]*/

	RF_DBG(rf, DBG_RF_DPK, "[DPK] DGain = 0x%x\n", dgain);

	return dgain;
}

s8 _dpk_dgain_mapping_8852c(
	struct rf_info *rf,
	u16 dgain)
{
	u16 bnd[15] = {0xbf1, 0xaa5, 0x97d, 0x875, 0x789,
			0x6b7, 0x5fc, 0x556, 0x4c1, 0x43d,
			0x3c7, 0x35e, 0x2ac, 0x262, 0x220};
	s8 offset = 0;

	if (dgain >= bnd[0])
		offset = 0x6;
	else if ((bnd[0] > dgain) && (dgain >= bnd[1]))
		offset = 0x6;
	else if ((bnd[1] > dgain) && (dgain >= bnd[2]))
		offset = 0x5;
	else if ((bnd[2] > dgain) && (dgain >= bnd[3]))
		offset = 0x4;
	else if ((bnd[3] > dgain) && (dgain >= bnd[4]))
		offset = 0x3;
	else if ((bnd[4] > dgain) && (dgain >= bnd[5]))
		offset = 0x2;
	else if ((bnd[5] > dgain) && (dgain >= bnd[6]))
		offset = 0x1;
	else if ((bnd[6] > dgain) && (dgain >= bnd[7]))
		offset = 0x0;
	else if ((bnd[7] > dgain) && (dgain >= bnd[8]))
		offset = 0xff;
	else if ((bnd[8] > dgain) && (dgain >= bnd[9]))
		offset = 0xfe;
	else if ((bnd[9] > dgain) && (dgain >= bnd[10]))
		offset = 0xfd;
	else if ((bnd[10] > dgain) && (dgain >= bnd[11]))
		offset = 0xfc;
	else if ((bnd[11] > dgain) && (dgain >= bnd[12]))
		offset = 0xfb;
	else if ((bnd[12] > dgain) && (dgain >= bnd[13]))
		offset = 0xfa;
	else if ((bnd[13] > dgain) && (dgain >= bnd[14]))
		offset = 0xf9;
	else if (bnd[14] > dgain)
		offset = 0xf8;
	else
		offset = 0x0;

	//RF_DBG(rf, DBG_RF_DPK, "[DPK] DGain offset = %d\n", offset);

	return offset;
}

u8 _dpk_pas_check_8852c(
	struct rf_info *rf)
{
	u8 fail = 0;

	halrf_wreg(rf, 0x80d4, MASKBYTE2, 0x06); /*0x80d6, ctrl_out_Kpack*/
	halrf_wreg(rf, 0x80bc, BIT(14), 0x0);	/*query status*/
	halrf_wreg(rf, 0x80c0, MASKBYTE2, 0x08);

	halrf_wreg(rf, 0x80c0, MASKBYTE3, 0x00); /*0x80C3*/
	if (halrf_rreg(rf, 0x80fc, MASKHWORD) == 0x0800) {
		fail = 1;
		RF_DBG(rf, DBG_RF_DPK, "[DPK] PAS check Fail!!\n");
	}
	
	return fail;
}

u8 _dpk_gainloss_read_8852c(
	struct rf_info *rf)
{
	u8 result;

	halrf_wreg(rf, 0x80d4, 0x003F0000, 0x6);	/*rpt_sel*/
	halrf_wreg(rf, 0x80bc, BIT(14), 0x1);		/*query status*/

	result = (u8)halrf_rreg(rf, 0x80fc, 0x000000F0); /*[7:4]*/

	RF_DBG(rf, DBG_RF_DPK, "[DPK] tmp GL = %d\n", result);

	return result;
}

u8 _dpk_gainloss_8852c(
	struct rf_info *rf,
	enum phl_phy_idx phy,
	enum rf_path path,
	u8 kidx)
{
	_dpk_one_shot_8852c(rf, phy, path, D_GAIN_LOSS);
	_dpk_kip_set_txagc_8852c(rf, phy, path, 0xff, false); /*set new tx power*/

	/*for k3 hw_bug*/
	halrf_wreg(rf, 0x81f0 + (path << 8), 0x0003FFFF, 0xf078); /*[17:0]*/
	halrf_wreg(rf, 0x81f0 + (path << 8), 0xF0000000, 0x0); /*[31:28]*/

	return _dpk_gainloss_read_8852c(rf);
}

u8 _dpk_pas_read_8852c(
	struct rf_info *rf,
	u8 is_check)
{
	u8 i;
	u32 tmp_val, val1_i = 0, val1_q = 0, val2_i = 0, val2_q = 0;

	halrf_wreg(rf, 0x80d4, MASKBYTE2, 0x06); /*0x80d6, ctrl_out_Kpack*/
	halrf_wreg(rf, 0x80bc, BIT(14), 0x0);	/*query status*/
	halrf_wreg(rf, 0x80c0, MASKBYTE2, 0x08);

	halrf_write_fwofld_end(rf); 	/*FW Offload End*/

	if (is_check) {
		halrf_wreg(rf, 0x80c0, MASKBYTE3, 0x00);
		tmp_val = halrf_rreg(rf, 0x80fc, MASKDWORD);
		val1_i = (tmp_val & MASKHWORD) >> 16;
		if (val1_i >= 0x800)
			val1_i = 0x1000 - val1_i;
		val1_q = tmp_val & MASKLWORD;
		if (val1_q >= 0x800)
			val1_q = 0x1000 - val1_q;
		halrf_wreg(rf, 0x80c0, MASKBYTE3, 0x1f);
		tmp_val = halrf_rreg(rf, 0x80fc, MASKDWORD);
		val2_i = (tmp_val & MASKHWORD) >> 16;
		if (val2_i >= 0x800)
			val2_i = 0x1000 - val2_i;
		val2_q = tmp_val & MASKLWORD;
		if (val2_q >= 0x800)
			val2_q = 0x1000 - val2_q;

		if ((val2_i * val2_i + val2_q * val2_q) != 0) /*to avoid BSOD issue*/
			RF_DBG(rf, DBG_RF_DPK, "[DPK] PAS_delta = 0x%x\n",
				(val1_i * val1_i + val1_q * val1_q) / 
				(val2_i * val2_i + val2_q * val2_q));
	} else {
		for (i = 0; i < 32; i++) {
			halrf_wreg(rf, 0x80c0, MASKBYTE3, i); /*0x80C3*/
			RF_DBG(rf, DBG_RF_DPK, "[DPK] PAS_Read[%02d]= 0x%08x\n", i,
				   halrf_rreg(rf, 0x80fc, MASKDWORD));
		}
	}

	halrf_write_fwofld_start(rf);	/*FW Offload Start*/

	if ((val1_i * val1_i + val1_q * val1_q) < (val2_i * val2_i + val2_q * val2_q))
		return 2;
	else if ((val1_i * val1_i + val1_q * val1_q) >= ((val2_i * val2_i + val2_q * val2_q) * 8 / 5))
		return 1;
	else
		return 0;
}

u8 _dpk_agc_8852c(
	struct rf_info *rf,
	enum phl_phy_idx phy,
	enum rf_path path,
	u8 kidx,
	u8 init_xdbm,
	u8 loss_only)
{
	struct halrf_dpk_info *dpk = &rf->dpk;

	u8 i = 0, tmp_dbm = init_xdbm, tmp_gl_idx = 0;
	u8 tmp_rxbb = 0;
	u8 goout = 0, agc_cnt = 0;
	//s8 offset = 0;
	u16 dgain = 0;
	bool is_fail = false;
	
	do {
		switch (i) {
		case 0: /*SYNC and Dgain*/
			_dpk_kip_set_rxagc_8852c(rf, phy, path, kidx);

			if (agc_cnt == 0) {
				if(dpk->bp[path][kidx].band == BAND_ON_24G)
					_dpk_bypass_rxiqc_8852c(rf, path);
				else
					_dpk_lbk_rxiqk_8852c(rf, phy, path);
			}

			if (DPK_RXSRAM_DBG_8852C)
				_dpk_read_rxsram_8852c(rf);

			halrf_write_fwofld_end(rf); 	/*FW Offload End*/

			is_fail = _dpk_sync_check_8852c(rf, path, kidx);

			if (is_fail) {
				halrf_write_fwofld_start(rf);	/*FW Offload Start*/
				goout = 1;
				break;
			}

			dgain = _dpk_dgain_read_8852c(rf);

			if (dgain > 0x5fc || dgain < 0x556) {
				_dpk_one_shot_8852c(rf, phy, path, D_SYNC);
				dgain = _dpk_dgain_read_8852c(rf);
			}

			halrf_write_fwofld_start(rf);	/*FW Offload Start*/

			i = 1;
			break;

		case 1: /*GAIN_LOSS and idx*/
			tmp_gl_idx = _dpk_gainloss_8852c(rf, phy, path, kidx);
			/*_dpk_pas_read_8852c(rf, false);*/

			if ((_dpk_pas_read_8852c(rf, true) == 2) && (tmp_gl_idx > 0))
				i = 3;
			else if ((tmp_gl_idx == 0 && _dpk_pas_read_8852c(rf, true) == 1) || tmp_gl_idx >= 7)
				i = 2; /*GL > criterion*/
			else if (tmp_gl_idx == 0)
				i = 3; /*GL < criterion*/
			else 
				i = 4;
			break;

		case 2: /*GL > criterion*/
			if (tmp_dbm <= 7) {
				goout = 1;
				RF_DBG(rf, DBG_RF_DPK, "[DPK] Txagc@lower bound!!\n");
			} else {
				if (tmp_dbm - 3 <= 7)
					tmp_dbm = 7;
				else
				tmp_dbm = tmp_dbm - 3;
				_dpk_kip_set_txagc_8852c(rf, phy, path, tmp_dbm, true);
			}
			i = 0;
			agc_cnt++;
			break;

		case 3:	/*GL < criterion*/
			if (tmp_dbm >= 24) {
				goout = 1;
				RF_DBG(rf, DBG_RF_DPK, "[DPK] Txagc@upper bound!!\n");
			} else {
				if (tmp_dbm + 2 >= 24)
					tmp_dbm = 24;
				else
				tmp_dbm = tmp_dbm + 2;
				_dpk_kip_set_txagc_8852c(rf, phy, path, tmp_dbm, true);
			}
			i = 0;
			agc_cnt++;
			break;

		case 4:
			if ((rf->phl_com->dev_cap.rfe_type == 5) && (dpk->bp[path][kidx].band != BAND_ON_24G))
				_dpk_kip_set_txagc_8852c(rf, phy, path, tmp_dbm - tmp_gl_idx - 3, true);

			_dpk_kip_control_rfc_8852c(rf, path, false);
			tmp_rxbb = (u8)halrf_rrf(rf, path, 0x00, MASKRFRXBB);
			if (tmp_rxbb + tmp_gl_idx > 0x1f)
				tmp_rxbb = 0x1f;
			 else
				tmp_rxbb = tmp_rxbb + tmp_gl_idx;

			 halrf_wrf(rf, path, 0x00, MASKRFRXBB, tmp_rxbb);
			
			RF_DBG(rf, DBG_RF_DPK, "[DPK] Adjust RXBB (%+d) = 0x%x\n",
				tmp_gl_idx, tmp_rxbb);
			_dpk_kip_control_rfc_8852c(rf, path, true);
			goout = 1;
			break;
		default:
			goout = 1;
			break;
		}	
	} while (!goout && (agc_cnt < 6));

	return is_fail;
}

void _dpk_set_mdpd_para_8852c(
	struct rf_info *rf,
	enum rf_path path,
	u8 order)
{
	struct halrf_dpk_info *dpk = &rf->dpk;

	switch (order) {
	case 0: /*(5,3,1)*/
		halrf_wreg(rf, 0x80a0, BIT(1) | BIT(0), 0x0);
		halrf_wreg(rf, 0x809c, BIT(10) | BIT(9), 0x2);
		halrf_wreg(rf, 0x80a0, 0x00001F00, 0x4); /*[12:8] phase normalize tap*/
		halrf_wreg(rf, 0x8070, 0x70000000, 0x1); /*[30:28] tx_delay_man*/
		dpk->dpk_order[path] = 0x6;
		break;

	case 1: /*(5,3,0)*/
		halrf_wreg(rf, 0x80a0, BIT(1) | BIT(0), 0x1);
		halrf_wreg(rf, 0x809c, BIT(10) | BIT(9), 0x1);
		halrf_wreg(rf, 0x80a0, 0x00001F00, 0x0); /*[12:8] phase normalize tap*/
		halrf_wreg(rf, 0x8070, 0x70000000, 0x0); /*[30:28] tx_delay_man*/
		dpk->dpk_order[path] = 0x2;
		break;

	case 2: /*(5,0,0)*/
		halrf_wreg(rf, 0x80a0, BIT(1) | BIT(0), 0x2);
		halrf_wreg(rf, 0x809c, BIT(10) | BIT(9), 0x0);
		halrf_wreg(rf, 0x80a0, 0x00001F00, 0x0); /*[12:8] phase normalize tap*/
		halrf_wreg(rf, 0x8070, 0x70000000, 0x0); /*[30:28] tx_delay_man*/
		dpk->dpk_order[path] = 0x0;
		break;

	case 3: /*(7,3,1)*/
		halrf_wreg(rf, 0x80a0, BIT(1) | BIT(0), 0x3);
		halrf_wreg(rf, 0x809c, BIT(10) | BIT(9), 0x3);
		halrf_wreg(rf, 0x80a0, 0x00001F00, 0x4); /*[12:8] phase normalize tap*/
		halrf_wreg(rf, 0x8070, 0x70000000, 0x1); /*[30:28] tx_delay_man*/
		dpk->dpk_order[path] = 0x1;
		break;
	default:
		RF_DBG(rf, DBG_RF_DPK, "[DPK] Wrong MDPD order!!(0x%x)\n", order);
		break;
	}
	RF_DBG(rf, DBG_RF_DPK, "[DPK] Set %s for IDL\n", order == 0x0 ? "(5,3,1)" :
		(order == 0x1 ? "(5,3,0)" : (order == 0x2 ? "(5,0,0)" : "(7,3,1)")));
}

void _dpk_coef_read_8852c(
	struct rf_info *rf,
	enum rf_path path,
	u8 kidx)
{
	struct halrf_dpk_info *dpk = &rf->dpk;

	u32 reg, reg_start, reg_stop;

	halrf_wreg(rf, 0x81d8 + (path << 8), MASKDWORD, 0x00010000);

	reg_start = 0xa500 + kidx * 0x1c0 + path * 0x400 + dpk->cur_k_set * 0x70;

	reg_stop = reg_start + 0x70;

	RF_DBG(rf, DBG_RF_DPK, "[DPK] ===== [Coef of S%d_CH%d_K%d] =====\n",
		path, kidx, dpk->cur_k_set);

	for (reg = reg_start; reg < reg_stop ; reg += 4) {
		RF_DBG(rf, DBG_RF_DPK, "[DPK][coef_r] 0x%x = 0x%08x\n", reg,
			   halrf_rreg(rf, reg, MASKDWORD));
	}
	halrf_wreg(rf, 0x81d8 + (path << 8), MASKDWORD, 0x00000000);
}

void _dpk_idl_mpa_8852c(
	struct rf_info *rf,
	enum phl_phy_idx phy,
	enum rf_path path,
	u8 kidx)
{
	struct halrf_dpk_info *dpk = &rf->dpk;
	u8 cnt = 0;
	u16 cnt1;

	halrf_wreg(rf, 0x80a0, BIT(16), 0x1); /*Phase Normalize enable*/

	if (dpk->bp[path][kidx].bw == CHANNEL_WIDTH_5 ||
		 dpk->bp[path][kidx].bw == CHANNEL_WIDTH_10 ||
		 dpk->bp[path][kidx].bw == CHANNEL_WIDTH_20)
		_dpk_set_mdpd_para_8852c(rf, path, 0x2);	/*5,0,0*/
	else if(dpk->bp[path][kidx].bw == CHANNEL_WIDTH_40 ||
		dpk->bp[path][kidx].bw == CHANNEL_WIDTH_80)
		_dpk_set_mdpd_para_8852c(rf, path, 0x1);	/*5,3,0*/
	else
		_dpk_set_mdpd_para_8852c(rf, path, 0x0);	/*5,3,1*/
#if 0
	if (halrf_rreg(rf, 0x80ec, BIT(8)) == 0x1)
		_dpk_set_mdpd_para_8852c(rf, path, 0x2); /*5,0,0*/
	else if (halrf_rreg(rf, 0x80ec, BIT(9)) == 0x1)
		_dpk_set_mdpd_para_8852c(rf, path, 0x1); /*5,3,0*/
	else
	_dpk_set_mdpd_para_8852c(rf, path, 0x0); /*5,3,1*/
#endif
	halrf_wreg(rf, 0x809c, BIT(8), 0x0);

	for (cnt1 = 0; cnt1 < 1000; cnt1++)
		halrf_delay_us(rf, 1);

	_dpk_one_shot_8852c(rf, phy, path, D_MDPK_IDL);

	halrf_wreg(rf, 0x80d4, 0x003F0000, 0x0);	/*rpt_sel*/
	//RF_DBG(rf, DBG_RF_DPK, "[DPK] timing_sync = 0x%x\n", halrf_rreg(rf, 0x80fc, MASKDWORD));

	halrf_write_fwofld_end(rf); 	/*FW Offload End*/

	dpk->dpk_sync[path] = halrf_rreg(rf, 0x80fc, MASKDWORD);

	/*wa for ov issue*/
	halrf_wreg(rf, 0x80d4, 0x003F0000, 0xf);	/*rpt_sel*/
	dpk->ov_flag[path] = (u8)halrf_rreg(rf, 0x80fc, BIT(30));

	halrf_write_fwofld_start(rf);	/*FW Offload Start*/

	while (dpk->ov_flag[path] == 0x1 && cnt < 5) {
		RF_DBG(rf, DBG_RF_DPK, "[DPK] ReK due to MDPK ov!!!\n");
		_dpk_one_shot_8852c(rf, phy, path, D_MDPK_IDL);
		halrf_wreg(rf, 0x80d4, 0x003F0000, 0xf);	/*rpt_sel*/
		dpk->ov_flag[path] = (u8)halrf_rreg(rf, 0x80fc, BIT(30));
		cnt++;
		dpk->rek_cnt[path][0]++;
	}

	if (dpk->ov_flag[path]) {
		_dpk_set_mdpd_para_8852c(rf, path, 0x2);	/*5,0,0*/
		_dpk_one_shot_8852c(rf, phy, path, D_MDPK_IDL);
	}

	//_dpk_coef_read_8852c(rf, path, kidx);
}

u8 _dpk_order_convert_8852c(
	struct rf_info *rf)
{
	u8 val;

	switch (halrf_rreg(rf, 0x80a0, BIT(1) | BIT(0))) {
	case 0: /*(5,3,1)*/
		val = 0x6;
		break;

	case 1: /*(5,3,0)*/
		val = 0x2;
		break;

	case 2: /*(5,0,0)*/
		val = 0x0;
		break;

	case 3: /*(7,3,1)*/
		val = 0x7;
		break;
	default:
		val = 0xff;
		break;
	}
		
	/*0x80a0 [1:0] = 0x0 => 0x81bc[26:25] = 0x6   //(5,3,1)*/
	/*0x80a0 [1:0] = 0x1 => 0x81bc[26:25] = 0x2   //(5,3,0)*/
	/*0x80a0 [1:0] = 0x2 => 0x81bc[26:25] = 0x0   //(5,0,0)*/
	/*0x80a0 [1:0] = 0x3 => 0x81bc[26:25] = 0x7   //(7,3,1)*/

	RF_DBG(rf, DBG_RF_DPK, "[DPK] convert MDPD order to 0x%x\n", val);

	return val;
}

void _dpk_gs_defalut_8852c(
	struct rf_info *rf,
	enum rf_path path,
	u8 kidx)
{
	struct halrf_dpk_info *dpk = &rf->dpk;

	u32 reg[2][4] = {{0x8190, 0x8194, 0x8198, 0x81a4},
			 {0x81a8, 0x81c4, 0x81c8, 0x81e8}};

	halrf_wreg(rf, reg[kidx][dpk->cur_k_set] + (path << 8), 0x0000007F, 0x5b);
#if 0
	/*CH0*/
	halrf_wreg(rf, 0x8190 + (path << 8), 0x0000007F, 0x5b); /*[6:0], K0*/
	halrf_wreg(rf, 0x8194 + (path << 8), 0x0000007F, 0x5b); /*[6:0], K1*/
	halrf_wreg(rf, 0x8198 + (path << 8), 0x0000007F, 0x5b); /*[6:0], K2*/
	halrf_wreg(rf, 0x81a4 + (path << 8), 0x0000007F, 0x5b); /*[6:0], K3*/
	/*CH1*/
	halrf_wreg(rf, 0x81a8 + (path << 8), 0x0000007F, 0x5b); /*[6:0], K0*/
	halrf_wreg(rf, 0x81c4 + (path << 8), 0x0000007F, 0x5b); /*[6:0], K1*/
	halrf_wreg(rf, 0x81c8 + (path << 8), 0x0000007F, 0x5b); /*[6:0], K2*/
	halrf_wreg(rf, 0x81e8 + (path << 8), 0x0000007F, 0x5b); /*[6:0], K3*/
#endif
}

void _dpk_gain_normalize_8852c(
	struct rf_info *rf,
	enum phl_phy_idx phy,
	enum rf_path path,
	u8 kidx,
	bool is_execute)
{
	struct halrf_dpk_info *dpk = &rf->dpk;

	u32 reg[2][4] = {{0x8190, 0x8194, 0x8198, 0x81a4},
			 {0x81a8, 0x81c4, 0x81c8, 0x81e8}};

	if (is_execute) {
		halrf_wreg(rf, 0x819c + (path << 8), 0x000003FF, 0x200); /*[9:0] pow_cal_start*/
		halrf_wreg(rf, 0x819c + (path << 8), BIT(17) | BIT(16), 0x3); /*pow_cal_len*/

		_dpk_one_shot_8852c(rf, phy, path, D_GAIN_NORM);
	} else
		halrf_wreg(rf, reg[kidx][dpk->cur_k_set] + (path << 8), 0x0000007F, 0x5b);
#ifndef HALRF_CONFIG_FW_IO_OFLD_SUPPORT
	dpk->bp[path][kidx].gs = (u8)halrf_rreg(rf, reg[kidx][dpk->cur_k_set] + (path << 8), 0x0000007F);
#else
	dpk->bp[path][kidx].gs = 0x5b;
#endif
#if 0
	/*CH0*/
	halrf_wreg(rf, 0x8190 + (path << 8), 0x0000007F, 0x5b); /*[6:0], K0*/
	halrf_wreg(rf, 0x8194 + (path << 8), 0x0000007F, 0x5b); /*[6:0], K1*/
	halrf_wreg(rf, 0x8198 + (path << 8), 0x0000007F, 0x5b); /*[6:0], K2*/
	halrf_wreg(rf, 0x81a4 + (path << 8), 0x0000007F, 0x5b); /*[6:0], K3*/
	/*CH1*/
	halrf_wreg(rf, 0x81a8 + (path << 8), 0x0000007F, 0x5b); /*[6:0], K0*/
	halrf_wreg(rf, 0x81c4 + (path << 8), 0x0000007F, 0x5b); /*[6:0], K1*/
	halrf_wreg(rf, 0x81c8 + (path << 8), 0x0000007F, 0x5b); /*[6:0], K2*/
	halrf_wreg(rf, 0x81e8 + (path << 8), 0x0000007F, 0x5b); /*[6:0], K3*/
#endif
}

void _dpk_on_8852c(
	struct rf_info *rf,
	enum phl_phy_idx phy,
	enum rf_path path,
	u8 kidx)
{
	struct halrf_dpk_info *dpk = &rf->dpk;

	u32 reg[2][4] = {{0x8190, 0x8194, 0x8198, 0x81a4},
			 {0x81a8, 0x81c4, 0x81c8, 0x81e8}};

	/*coef load*/
	halrf_wreg(rf, 0x81dc + (path << 8), BIT(16), 0x1);
	halrf_wreg(rf, 0x81dc + (path << 8), BIT(16), 0x0);

	halrf_wreg(rf, 0x81bc + (path << 8) + (kidx << 2),
			BIT(26) | BIT(25) | BIT(24), dpk->dpk_order[path]);

	dpk->bp[path][kidx].path_ok = dpk->bp[path][kidx].path_ok | BIT(dpk->cur_k_set);

	RF_DBG(rf, DBG_RF_DPK, "[DPK] S%d[%d] path_ok = 0x%x\n", path, kidx,
		dpk->bp[path][kidx].path_ok);

	/*MDPD enable, [31:28] = [k3,k2,k1,k0]*/
	halrf_wreg(rf, 0x81bc + (path << 8) + (kidx << 2), 0xf0000000,
			dpk->bp[path][kidx].path_ok);

	_dpk_gain_normalize_8852c(rf, phy, path, kidx, false);
	//_dpk_para_query_8852c(rf, path, kidx);

	if ((rf->phl_com->dev_cap.rfe_type == 5) && (dpk->bp[path][kidx].band != BAND_ON_24G)) {
		dpk->bp[path][kidx].txagc_dpk = dpk->bp[path][kidx].txagc_dpk - 3;
		halrf_wreg(rf, reg[kidx][dpk->cur_k_set] + (path << 8), 0x0000FC00, dpk->bp[path][kidx].txagc_dpk); /*[15:10]*/
	}
	
}

bool _dpk_reload_check_8852c(
	struct rf_info *rf,
	enum phl_phy_idx phy,
	enum rf_path path)
{
	struct halrf_dpk_info *dpk = &rf->dpk;

	bool is_reload = false;
	u8 idx, cur_band, cur_ch;

	cur_band = rf->hal_com->band[phy].cur_chandef.band;
	cur_ch = rf->hal_com->band[phy].cur_chandef.center_ch;

	for (idx = 0; idx < DPK_BKUP_NUM; idx++) {
		if ((cur_band == dpk->bp[path][idx].band) && (cur_ch == dpk->bp[path][idx].ch)) {
			halrf_wreg(rf, 0x8104 + (path << 8), BIT(8), idx);
			dpk->cur_idx[path] = idx;
			is_reload = true;
			RF_DBG(rf, DBG_RF_DPK, "[DPK] reload S%d[%d] success\n", path, idx);
		}
	}

	return is_reload;
}

bool _dpk_main_8852c(
	struct rf_info *rf,
	enum phl_phy_idx phy,
	enum rf_path path)
{
	struct halrf_dpk_info *dpk = &rf->dpk;

	u8 init_xdbm = 15;
	u8 kidx = dpk->cur_idx[path];
	bool is_fail = false;

	_dpk_kip_control_rfc_8852c(rf, path, false);
	halrf_rf_direct_cntrl_8852c(rf, path, false); /*switch control to direct write*/
	halrf_wrf(rf, path, 0x10005, MASKRF, 0x03ffd); /*only keep BB control TX_POWER*/

	_dpk_rf_setting_8852c(rf, path, kidx);
	halrf_rx_dck_toggle_8852c(rf, path);

	_dpk_kip_pwr_clk_onoff_8852c(rf, true);
	_dpk_kip_preset_8852c(rf, phy, path, kidx);
	_dpk_txpwr_bb_force_8852c(rf, path, true);
	_dpk_kip_set_txagc_8852c(rf, phy, path, init_xdbm, true);
	_dpk_tpg_sel_8852c(rf, path, kidx);
#if 0
	is_fail = _dpk_kip_set_rxagc_8852c(rf, phy, path, kidx);

	if (DPK_RXSRAM_DBG)
		_dpk_read_rxsram_8852c(rf);

	if (is_fail)
		goto _error;

	_dpk_dgain_read_8852c(rf);
	_dpk_bypass_rxiqc_8852c(rf, path);
	_dpk_gainloss_8852c(rf, phy, path, kidx);
	_dpk_para_query_8852c(rf, path, kidx);
#else
	is_fail = _dpk_agc_8852c(rf, phy, path, kidx, init_xdbm, false);

	if (is_fail)
		goto _error;
#endif
	/*_dpk_pas_read_8852c(rf, false);*/
	//_dpk_get_thermal_8852c(rf, kidx, path);

	_dpk_idl_mpa_8852c(rf, phy, path, kidx);
	_dpk_para_query_8852c(rf, path, kidx);

	//_dpk_kip_control_rfc_8852c(rf, path, false);
	//_dpk_get_thermal_8852c(rf, kidx, path);
#if 0
	_dpk_coef_read_8852c(rf, path, kidx, gain);
#endif
	_dpk_on_8852c(rf, phy, path, kidx);
_error:
	_dpk_kip_control_rfc_8852c(rf, path, false);
	halrf_wrf(rf, path, 0x00, MASKRFMODE, RF_RX);

	RF_DBG(rf, DBG_RF_DPK, "[DPK] S%d[%d]_K%d %s\n", path, kidx,
		dpk->cur_k_set, is_fail ? "need Check" : "is Success");

	dpk->dpk_cal_cnt++;

	if(!is_fail)
		dpk->dpk_ok_cnt++;

	return is_fail;
}

void _dpk_cal_select_8852c(
	struct rf_info *rf,
	bool force,
	enum phl_phy_idx phy,
	u8 kpath)
{
	struct halrf_dpk_info *dpk = &rf->dpk;

	u32 kip_bkup[DPK_RF_PATH_MAX_8852C][DPK_KIP_REG_NUM_8852C] = {{0}};
	//u32 bb_bkup[DPK_BB_REG_NUM_8852C] = {0};
	u32 rf_bkup[DPK_RF_PATH_MAX_8852C][DPK_RF_REG_NUM_8852C] = {{0}};

	u32 kip_reg[] = {0x813c, 0x8124, 0x8120, 0xc0d4, 0xc0d8};
	//u32 bb_reg[] = {0x2344, 0x5800, 0x7800};
	u32 rf_reg[DPK_RF_REG_NUM_8852C] = {0xdf, 0x5f, 0x8f, 0x97, 0xa3, 0x5, 0x10005};

	u8 path;
	bool is_fail = true;
	bool reloaded[DPK_RF_PATH_MAX_8852C] = {false};

	if ((!phl_is_mp_mode(rf->phl_com)) && DPK_RELOAD_EN_8852C) {
		for (path = 0; path < DPK_RF_PATH_MAX_8852C; path++) {
			reloaded[path] = _dpk_reload_check_8852c(rf, phy, path);
			if ((reloaded[path] == false) && (dpk->bp[path][0].ch != 0))
				dpk->cur_idx[path] = !dpk->cur_idx[path];
			else
				halrf_dpk_onoff_8852c(rf, path, false);
		}
	} else {
		for (path = 0; path < DPK_RF_PATH_MAX_8852C; path++)
			dpk->cur_idx[path] = 0;
	}

	for (path = 0; path < DPK_RF_PATH_MAX_8852C; path++) {
		if (kpath & BIT(path)) {
			_dpk_bkup_kip_8852c(rf, kip_reg, kip_bkup, path);
			_dpk_bkup_rf_8852c(rf, rf_reg, rf_bkup, path);
			_dpk_information_8852c(rf, phy, path);
			_dpk_init_8852c(rf, path);
			if (rf->is_tssi_mode[path])
				_dpk_tssi_pause_8852c(rf, path, true);
		}
	}

	halrf_write_fwofld_start(rf);	/*FW Offload Start*/

	for (path = 0; path < DPK_RF_PATH_MAX_8852C; path++) {
		if (kpath & BIT(path)) {
			RF_DBG(rf, DBG_RF_DPK, "[DPK] ========= S%d[%d] DPK Start =========\n", path, dpk->cur_idx[path]);		
			_dpk_rxagc_onoff_8852c(rf, path, false);
			halrf_drf_direct_cntrl_8852c(rf, path, false);
			_dpk_bb_afe_setting_8852c(rf, path);
			is_fail = _dpk_main_8852c(rf, phy, path);
			halrf_dpk_onoff_8852c(rf, path, is_fail);
		}
	}

	for (path = 0; path < DPK_RF_PATH_MAX_8852C; path++) {
		if (kpath & BIT(path)) {
			_dpk_kip_restore_8852c(rf, phy, path);
			_dpk_reload_kip_8852c(rf, kip_reg, kip_bkup, path);
			_dpk_reload_rf_8852c(rf, rf_reg, rf_bkup, path);
			_dpk_bb_afe_restore_8852c(rf, path);
			_dpk_rxagc_onoff_8852c(rf, path, true);
			if (rf->is_tssi_mode[path])
				_dpk_tssi_pause_8852c(rf, path, false);
		}
	}
	_dpk_kip_pwr_clk_onoff_8852c(rf, false);

	halrf_write_fwofld_end(rf); 	/*FW Offload End*/
}

u8 _dpk_bypass_check_8852c(
	struct rf_info *rf,
	enum phl_phy_idx phy)
{
	struct halrf_fem_info *fem = &rf->fem;

	u8 result;

	if (rf->hal_com->cv == CAV && (rf->hal_com->band[phy].cur_chandef.band != BAND_ON_24G)) {
		RF_DBG(rf, DBG_RF_DPK, "[DPK] Skip DPK due to CAV & not 2G!!\n");
		result = 1;
	} else if (fem->epa_2g && (rf->hal_com->band[phy].cur_chandef.band == BAND_ON_24G)) {
		RF_DBG(rf, DBG_RF_DPK, "[DPK] Skip DPK due to 2G_ext_PA exist!!\n");
		result = 1;
	} else if (fem->epa_5g && (rf->hal_com->band[phy].cur_chandef.band == BAND_ON_5G)) {
		RF_DBG(rf, DBG_RF_DPK, "[DPK] Skip DPK due to 5G_ext_PA exist!!\n");
		result = 1;
	} else if (fem->epa_6g && (rf->hal_com->band[phy].cur_chandef.band == BAND_ON_6G)) {
		RF_DBG(rf, DBG_RF_DPK, "[DPK] Skip DPK due to 6G_ext_PA exist!!\n");
		result = 1;
	} else if (rf->phl_com->dev_cap.rfe_type > 50) {
		result = 1;
	} else
		result = 0;

	return result;
}

void _dpk_force_bypass_8852c(
	struct rf_info *rf,
	enum phl_phy_idx phy)
{
	u8 path, kpath;

	kpath = halrf_kpath_8852c(rf, phy);

	for (path = 0; path < DPK_RF_PATH_MAX_8852C; path++) {
		if (kpath & BIT(path))
			halrf_dpk_onoff_8852c(rf, path, true);
	}
}

void halrf_dpk_8852c(
	struct rf_info *rf,
	enum phl_phy_idx phy,
	bool force)
{
	struct halrf_rx_dck_info *rx_dck = &rf->rx_dck;

	RF_DBG(rf, DBG_RF_DPK, "[DPK] ****** DPK Start (Ver: 0x%x, Cv: %d, RF_para: %d) ******\n",
		DPK_VER_8852C, rf->hal_com->cv, halrf_get_radio_ver_from_reg(rf));

	RF_DBG(rf, DBG_RF_DPK, "[DPK] Driver mode = %d\n", rf->phl_com->drv_mode);
#if 1
	if (_dpk_bypass_check_8852c(rf, phy))
		_dpk_force_bypass_8852c(rf, phy);
	else
		_dpk_cal_select_8852c(rf, force, phy, halrf_kpath_8852c(rf, phy));

	if (rx_dck->is_auto_res) /*if auto DCK enabled*/
		halrf_rx_dck_8852c(rf, phy, false);
#else
	//_dpk_information_8852c(rf, 0, RF_PATH_A);
	//halrf_drf_direct_cntrl_8852c(rf, RF_PATH_A, false);
	//_dpk_bb_afe_setting_8852c(rf, RF_PATH_A);
	
	//_dpk_main_8852c(rf, phy, RF_PATH_A);
	//halrf_delay_us(rf, 100);
	halrf_rf_direct_cntrl_8852c(rf, RF_PATH_A, false); /*switch control to direct write*/
	//halrf_delay_us(rf, 100);
	//halrf_wrf(rf, RF_PATH_A, 0x10005, MASKRF, 0x03ffd); /*only keep BB control TX_POWER*/
	_dpk_rf_setting_8852c(rf, RF_PATH_A, 0);
	//halrf_delay_us(rf, 70);
	halrf_set_rx_dck_8852c(rf, phy, RF_PATH_A, false);
	//_dpk_kip_pwr_clk_onoff_8852c(rf, true);
	//_dpk_kip_preset_8852c(rf, phy, RF_PATH_A, 0);
	//_dpk_txpwr_bb_force_8852c(rf, RF_PATH_A, true);
	//_dpk_kip_set_txagc_8852c(rf, phy, RF_PATH_A, 21, true);
	//_dpk_tpg_sel_8852c(rf, RF_PATH_A, 0);
	//_dpk_kip_set_rxagc_8852c(rf, phy, RF_PATH_A, 0);


#endif
}

void halrf_dpk_onoff_8852c(
	struct rf_info *rf,
	enum rf_path path,
	bool off)
{
	struct halrf_dpk_info *dpk = &rf->dpk;
	bool off_reverse;
	u8 val, kidx = dpk->cur_idx[path];

	if (off)
		off_reverse = false;
	else
		off_reverse = true;

	val = dpk->is_dpk_enable * off_reverse * dpk->bp[path][kidx].path_ok;

	/*MDPD enable, [31:28] = [k3,k2,k1,k0]*/
	halrf_wreg(rf, 0x81bc + (path << 8) + (kidx << 2), 0xf0000000, val);

	RF_DBG(rf, DBG_RF_DPK, "[DPK] S%d[%d] DPK %s !!!\n", path, kidx,
		   (val == 0) ? "disable" : "enable");
}

void halrf_dpk_track_8852c(
	struct rf_info *rf)
{
#if 1 
	struct halrf_dpk_info *dpk = &rf->dpk;

	u8 path, kidx;
	u8 txagc_rf;
	s8 txagc_bb, txagc_bb_tp, txagc_ofst;
	u8 cur_ther;
	s16 pwsf_tssi_ofst;
	s8 delta_ther = 0;
	//u8 tx_sf_addr;
	//s8 slope;

	if ((rf->dbg_component & DBG_RF_DPK_TRACK) == 0x0 && rf->hal_com->cv != CAV)
		return;

	for (path = 0; path < DPK_RF_PATH_MAX_8852C; path++) {

		kidx = dpk->cur_idx[path];

		RF_DBG(rf, DBG_RF_DPK_TRACK,
		       "[DPK_TRK] ================[S%d[%d] (CH %d)]================\n",
		       path, kidx, dpk->bp[path][kidx].ch);

		/*rpt from BB*/
		txagc_rf = (u8)halrf_rreg(rf, 0x1c60 + (path << 13), 0x0000003f); /*[5:0]*/
		txagc_bb = (s8)halrf_rreg(rf, 0x1c60 + (path << 13), MASKBYTE2); /*[23:16]*/
		txagc_bb_tp = (u8)halrf_rreg(rf, 0x1ca0 + (path << 13), 0xFF000000); /*[31:24]*/

		/*rpt from KIP*/
		halrf_wreg(rf, 0x81d4 + (path << 8), 0x003F0000, 0xf);	/*rpt_sel*/
		cur_ther = (u8)halrf_rreg(rf, 0x81fc + (path << 8), 0x0000003F); /*[5:0]*/
		txagc_ofst = (s8)halrf_rreg(rf, 0x81fc + (path << 8), 0x0000FF00); /*[15:8]*/
		pwsf_tssi_ofst = (s16)halrf_rreg(rf, 0x81fc + (path << 8), 0x1FFF0000); /*[28:16]*/

		if (pwsf_tssi_ofst >= 0x1000)
			pwsf_tssi_ofst |= 0xE000;

		//halrf_wreg(rf, 0x81d4 + (path << 8), 0x003F0000, 0x11);	/*rpt_sel*/
		//tx_sf_addr = (u8)halrf_rreg(rf, 0x81fc + (path << 8), 0x000000FF); /*[7:0]*/

		//slope = (s8)halrf_rreg(rf, 0x81bc + (path << 8) + (kidx << 2), 0x00003F00); /*[13:8]*/

		delta_ther = cur_ther - dpk->bp[path][kidx].ther_dpk;
		
		delta_ther = delta_ther * 1 / 2; /*2.4G*/

		RF_DBG(rf, DBG_RF_DPK_TRACK, "[DPK_TRK] extra delta_ther = %d (0x%x / 0x%x@k)\n",
			delta_ther, cur_ther, dpk->bp[path][kidx].ther_dpk);

		RF_DBG(rf, DBG_RF_DPK_TRACK, "[DPK_TRK] delta_txagc = %d (0x%x / 0x%x@k)\n",
			txagc_rf - dpk->bp[path][kidx].txagc_dpk,
			txagc_rf, dpk->bp[path][kidx].txagc_dpk);

		RF_DBG(rf, DBG_RF_DPK_TRACK, "[DPK_TRK] txagc_offset / pwsf_tssi_ofst = 0x%x / %+d\n",
			txagc_ofst, pwsf_tssi_ofst);

		RF_DBG(rf, DBG_RF_DPK_TRACK, "[DPK_TRK] txagc_bb_tp / txagc_bb = 0x%x / 0x%x\n",
			txagc_bb_tp, txagc_bb);

		if (rf->rfk_is_processing != true && halrf_rreg(rf, 0x80f4, BIT(29)) == 0x0 && txagc_rf != 0 && rf->hal_com->cv == CAV) {
			RF_DBG(rf, DBG_RF_DPK_TRACK, "[DPK_TRK] New pwsf = 0x%x\n", 0x78 - delta_ther);

			halrf_wreg(rf, 0x81b4 + (path << 8) + (kidx << 2), 0x07FC0000, 0x78 - delta_ther); /*[26:18] k2*/
		}
	}
#endif
}

#endif

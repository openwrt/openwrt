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
#include "../halrf_precomp.h"
#ifdef RF_8852C_SUPPORT

void _txgapk_backup_bb_registers_8852c(
	struct rf_info *rf,
	u32 *reg,
	u32 *reg_bkup,
	u8 reg_num)
{
	u8 i;

	for (i = 0; i < reg_num; i++) {
		reg_bkup[i] = halrf_rreg(rf, reg[i], MASKDWORD);
		
		RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK] Backup BB 0x%x = %x\n", reg[i], reg_bkup[i]);
	}
}

void _txgapk_backup_kip_8852c(
	struct rf_info *rf,
	u32 *reg,
	u32 reg_bkup[][TXGAPK_KIP_REG_NUM_8852C],
	u8 path,
	u8 reg_num)
{
	u8 i;

	for (i = 0; i < reg_num; i++) {
		reg_bkup[path][i] = halrf_rreg(rf, reg[i] + (path << 8), MASKDWORD);
		
		RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK] Backup kip 0x%x = %x\n", reg[i]+ (path << 8), reg_bkup[path][i]);
	}
}


void _txgapk_bkup_rf_8852c(
	struct rf_info *rf,
	u32 *rf_reg,
	u32 rf_bkup[][TXGAPK_RF_REG_NUM_8852C],
	u8 path,
	u8 reg_num)
{
	u8 i;

	for (i = 0; i < reg_num; i++) {
		rf_bkup[path][i] = halrf_rrf(rf, path, rf_reg[i], MASKRF);
		
		RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK] Backup RF S%d 0x%x = %x\n",
			path, rf_reg[i], rf_bkup[path][i]);
	}
}
	



void _txgapk_reload_bb_registers_8852c(
	struct rf_info *rf,
	u32 *reg,
	u32 *reg_backup,
	u8 reg_num)
{
	u8 i;

	for (i = 0; i < reg_num; i++) {
		halrf_wreg(rf, reg[i], MASKDWORD, reg_backup[i]);

		RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK] Reload BB 0x%x = 0x%x\n",
		       reg[i], reg_backup[i]);
	}
}


void _txgapk_reload_rf_8852c(
	struct rf_info *rf,
	u32 *rf_reg,
	u32 rf_bkup[][TXGAPK_RF_REG_NUM_8852C],
	u8 path,
	u8 reg_num)
{
	u8 i;

	for (i = 0; i < reg_num; i++) {
		halrf_wrf(rf, path, rf_reg[i], MASKRF, rf_bkup[path][i]);
		
			RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK] Reload RF S%d 0x%x = %x\n",
				path, rf_reg[i], rf_bkup[path][i]);
	}
}


void _txgapk_reload_kip_8852c(
	struct rf_info *rf,
	u32 *reg,
	u32 reg_bkup[][TXGAPK_KIP_REG_NUM_8852C],
	u8 path,
	u8 reg_num) 
{
	u8 i;

	for (i = 0; i < reg_num; i++) {
		halrf_wreg(rf, reg[i] + (path << 8), MASKDWORD, reg_bkup[path][i]);
		
		RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK] Reload KIP 0x%x = %x\n", reg[i] + (path << 8),
			reg_bkup[path][i]);
	}
}



void _halrf_txgapk_rxagc_onoff_8852c(
	struct rf_info *rf,
	enum rf_path path,
	bool turn_on)
{
	if (path == RF_PATH_A)
		halrf_wreg(rf, 0x4730, BIT(31), turn_on);
	else
		halrf_wreg(rf, 0x4a9c, BIT(31), turn_on);

	RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]======> S%d RXAGC is %s\n", path,
		turn_on ? "turn_on" : "turn_off");
}

void _halrf_txgapk_bb_afe_by_mode_8852c(struct rf_info *rf,
					enum phl_phy_idx phy, enum rf_path path, bool is_dbcc)
{
	RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]======> %s\n", __func__);

	if (!is_dbcc) {
		/* nodbcc */
		halrf_wrf(rf, path, 0x10005, 0x00001, 0x0);
		if (path == RF_PATH_A) {
			//1.Keep ADC_fifo reset until rxck is rdy
			//ADDA fifo rst //od unparsing
			halrf_wreg(rf, 0x20fc, 0x00010000, 0x1);
			halrf_wreg(rf, 0x20fc, 0x00100000, 0x0);
			halrf_wreg(rf, 0x20fc, 0x01000000, 0x1);
			halrf_wreg(rf, 0x20fc, 0x10000000, 0x0);

			//2.BB for IQK DBG mode 
			//IQK control RFC
			halrf_wreg(rf, 0x5670, MASKDWORD, 0xf801fffd);

			//3. Set DAC clk (Option)
			// DAC_960Mhz | 0x1 
			#if 0
			halrf_wreg(rf, 0x5670, 0x00004000, 0x1);
			halrf_wreg(rf, 0x12a0, 0x00008000, 0x1);
				
			//DAC CLK 960MHz
			halrf_wreg(rf, 0x5670, 0x80000000, 0x1);
			halrf_wreg(rf, 0x12a0, 0x00007000, 0x7);
			#else
			halrf_txck_force_8852c(rf, path, true, DAC_960M);
			#endif

			//4. Set ADC clk&upd_adc (Option)
			//ADC CLK 320MHz
			#if 0
			halrf_wreg(rf, 0x5670, 0x00002000, 0x1);
			halrf_wreg(rf, 0x12a0, 0x00080000, 0x1);
			
			halrf_wreg(rf, 0x12a0, 0x00070000, 0x3);
			halrf_wreg(rf, 0x5670, 0x60000000, 0x2);
			#else
			halrf_rxck_force_8852c(rf, path, true, ADC_1920M);
			#endif
		

			//ADC320M, s0
			halrf_wreg(rf, 0xc0d4, 0x0c000000, 0x1);
			halrf_wreg(rf, 0xc0d8, 0x000001e0, 0xb);

			// AFE debug mode
			halrf_wreg(rf, 0x12b8, 0x40000000, 0x1);

			//DAC ON  AD ON //By¡@KS
			// Analog Part ADC rst
			halrf_wreg(rf, 0x030c, 0xff000000, 0x1f);
			halrf_wreg(rf, 0x030c, 0xff000000, 0x13);

			// Digital Part ADC rst
			halrf_wreg(rf, 0x032c, 0xffff0000, 0x0001);
			halrf_wreg(rf, 0x032c, 0xffff0000, 0x0041);

			//5. ADDA fifo rst
			halrf_wreg(rf, 0x20fc, 0x00100000, 0x1);
			halrf_wreg(rf, 0x20fc, 0x10000000, 0x1);
		} else if (path == RF_PATH_B) {			
			halrf_wreg(rf, 0x20fc, 0x00020000, 0x1);
			halrf_wreg(rf, 0x20fc, 0x00200000, 0x0);
			halrf_wreg(rf, 0x20fc, 0x02000000, 0x1);
			halrf_wreg(rf, 0x20fc, 0x20000000, 0x0);
			halrf_wreg(rf, 0x7670, MASKDWORD, 0xf801fffd);

			//3. Set DAC clk (Option)
			#if 0
			halrf_wreg(rf, 0x7670, 0x00004000, 0x1);
			halrf_wreg(rf, 0x32a0, 0x00008000, 0x1);
			
			halrf_wreg(rf, 0x7670, 0x80000000, 0x1);
			halrf_wreg(rf, 0x32a0, 0x00007000, 0x7);
			#else
			halrf_txck_force_8852c(rf, path, true, DAC_960M);
			#endif


			//4. Set ADC clk&upd_adc (Option)
			//ADC CLK 320MHz
			#if 0
			halrf_wreg(rf, 0x32a0, 0x00080000, 0x1);
			halrf_wreg(rf, 0x32a0, 0x00070000, 0x3);
			halrf_wreg(rf, 0x7670, 0x60000000, 0x2);
			#else
			halrf_rxck_force_8852c(rf, path,true, ADC_1920M);
			#endif

			
			halrf_wreg(rf, 0xc1d4, 0x0c000000, 0x1);
			halrf_wreg(rf, 0xc1d8, 0x000001e0, 0xb);
			halrf_wreg(rf, 0x32b8, 0x40000000, 0x1);
			halrf_wreg(rf, 0x030c, 0xff000000, 0x1f);
			halrf_wreg(rf, 0x030c, 0xff000000, 0x13);
			halrf_wreg(rf, 0x032c, 0xffff0000, 0x0001);
			halrf_wreg(rf, 0x032c, 0xffff0000, 0x0041);
			halrf_wreg(rf, 0x20fc, 0x00200000, 0x1);
			halrf_wreg(rf, 0x20fc, 0x20000000, 0x1);
		}				
	} else {
		if (phy == HW_PHY_0) {
			/* dbcc phy0 path 0 */
			//TBD
		} else if (phy == HW_PHY_1) {
			/* dbcc phy1 path 1 */
			//TBD
		}
	}
		
}




void _halrf_txgapk_iqk_preset_by_mode_8852c(struct rf_info *rf,
					enum phl_phy_idx phy, enum rf_path path, bool is_dbcc)
{
	RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]======> %s\n", __func__);

	if (!is_dbcc) {
		/* nodbcc */
		/* A-Die BB_Direct_SEL */
		halrf_wrf(rf, path, 0x5, 0x00001, 0x0);

		// [7]cip_power_on
		halrf_wreg(rf, 0x8008, MASKDWORD, 0x00000080);

		//cip_rate_dbg=dpk_rate_dbg=CLK320M 
		halrf_wreg(rf, 0x8088, MASKDWORD, 0x81ff010a);
	} else {
		/* dbcc */
		//TBD
		//if (phy == HW_PHY_0)		
		//else if (phy == HW_PHY_1)
			
	}
	
}


void _halrf_txgapk_clk_setting_dac960mhz_by_mode_8852c
	(struct rf_info *rf, enum phl_phy_idx phy, enum rf_path path, bool is_dbcc)
{
	RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]======> %s\n", __func__);

	if (!is_dbcc) {
		/* nodbcc */
		halrf_wreg(rf, 0x12b8, 0x40000000, 0x1);
		halrf_wreg(rf, 0x32b8, 0x40000000, 0x1);
		halrf_delay_us(rf, 1);
		
		halrf_wreg(rf, 0x030c, 0xff000000, 0x1f);
		halrf_delay_us(rf, 1);
		
		halrf_wreg(rf, 0x030c, 0xff000000, 0x13);
		halrf_wreg(rf, 0x032c, 0xffff0000, 0x0001);
		halrf_delay_us(rf, 1);
		
		halrf_wreg(rf, 0x032c, 0xffff0000, 0x0041);
		
	} else {
		/* dbcc */
		if (phy == HW_PHY_0)
			halrf_wreg(rf, 0x8120, MASKDWORD, 0xce000a08);
		else if (phy == HW_PHY_1)
			halrf_wreg(rf, 0x8220, MASKDWORD, 0xce000a08);

	}
	
}


void _halrf_txgapk_rf_dpk_2g_8852c
	(struct rf_info *rf, enum phl_phy_idx phy, enum rf_path path)
{
	u32 dpk_path_att_r1[2] = {0xe, 0x3};

	RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]======> %s\n", __func__);

	// RF mode & RxBB
	halrf_wrf(rf, path, 0x00, 0xf0000, 0x5);
	halrf_wrf(rf, path, 0x00, 0x003e0, 0x1f);

	// TIA 
	halrf_wrf(rf, path, 0xdf, 0x01000, 0x1);
	halrf_wrf(rf, path, 0x9e, 0x00100, 0x0); // 0:26dB, 1:20dB

	// DPK path attenuator
	halrf_wrf(rf, path, 0x83, 0x00007, 0x1); //ATT 0~7 : -24~-36 dB
	halrf_wrf(rf, path, 0x83, 0x000f0, dpk_path_att_r1[path]); //R1  0~15: -23~-1 dB
}

void _halrf_txgapk_rf_dpk_5g_8852c
	(struct rf_info *rf, enum phl_phy_idx phy, enum rf_path path)
{
	u8 rfe_type = rf->phl_com->dev_cap.rfe_type;
	
	RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]======> %s\n", __func__);

	
	// RF mode & RxBB
	halrf_wrf(rf, path, 0x00, 0xf0000, 0x5);

	if (rfe_type <50) //NIC
		halrf_wrf(rf, path, 0x00, 0x003e0, 0x1f);
	else if (rfe_type == 50) //ifem
		halrf_wrf(rf, path, 0x00, 0x003e0, 0x12);
	else //efem
		halrf_wrf(rf, path, 0x00, 0x003e0, 0x12);

	//debug
	RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]RXBB RF0x0[9:5] = 0x%x\n", halrf_rrf(rf, path, 0x00, 0x003e0));
	

	// TIA 
	halrf_wrf(rf, path, 0xdf, 0x01000, 0x1);
	halrf_wrf(rf, path, 0x9e, 0x00100, 0x0); // 0:26dB, 1:20dB

	//DPK path attenuator
	halrf_wrf(rf, path, 0x8c, 0x0000f, 0x2); //ATT 0/1/2 : -38/-32/-26 dB
	halrf_wrf(rf, path, 0x98, 0x0f000, 0x3); //R1  1/2/3/4 : -12/-5/-6/0 dB
}


void _halrf_txgapk_rf_dpk_8852c
	(struct rf_info *rf, enum phl_phy_idx phy, enum rf_path path)
{
	//u8 channel = rf->hal_com->band[phy].cur_chandef.center_ch;
	u32 band = rf->hal_com->band[phy].cur_chandef.band;
	
	//if (channel >= 1 && channel <= 14)
	if (band == BAND_ON_24G)
		_halrf_txgapk_rf_dpk_2g_8852c(rf, phy, path);
	else
		_halrf_txgapk_rf_dpk_5g_8852c(rf, phy, path);
}


void _halrf_txgapk_rf_rxdck_2g_8852c
	(struct rf_info *rf, enum phl_phy_idx phy, enum rf_path path)
{
	//u8 i = 0;

	RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]======> %s\n", __func__);
		
	halrf_wrf(rf, path, 0x92, 0x00001, 0x0);
	halrf_wrf(rf, path, 0x92, 0x00001, 0x1);

	#if 0
	for (i = 0; i < 100; i++)
		halrf_delay_us(rf, 1);
	#else
	halrf_delay_us(rf, 100);
	#endif
	
	halrf_wrf(rf, path, 0x92, 0x00001, 0x0);
}

void _halrf_txgapk_rf_rxdck_5g_8852c
	(struct rf_info *rf, enum phl_phy_idx phy, enum rf_path path)
{
	//u8 i = 0;

	RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]======> %s\n", __func__);

	
	halrf_wrf(rf, path, 0x92, 0x00001, 0x0);
	halrf_wrf(rf, path, 0x92, 0x00001, 0x1);

	#if 0
	for (i = 0; i < 100; i++)
		halrf_delay_us(rf, 1);
	#else
	halrf_delay_us(rf, 100);
	#endif
	
	halrf_wrf(rf, path, 0x92, 0x00001, 0x0);
}


void _halrf_txgapk_rf_rxdck_8852c
	(struct rf_info *rf, enum phl_phy_idx phy, enum rf_path path)
{
	//u8 channel = rf->hal_com->band[phy].cur_chandef.center_ch;
	u32 band = rf->hal_com->band[phy].cur_chandef.band;
	
	//if (channel >= 1 && channel <= 14)
	if (band == BAND_ON_24G)
		_halrf_txgapk_rf_rxdck_2g_8852c(rf, phy, path);
	else
		_halrf_txgapk_rf_rxdck_5g_8852c(rf, phy, path);
}


void _halrf_txgapk_track_table_nctl_2g_8852c
	(struct rf_info *rf, enum phl_phy_idx phy, enum rf_path path, u32 gain_stage)
{
	struct halrf_gapk_info *txgapk_info = &rf->gapk;
	
	u32 i;
	u32 d[17] = {0}; 
	//u32 ta[17] = {0};

#ifndef HALRF_CONFIG_FW_IO_OFLD_SUPPORT
	u32 rf_tmp = 0;
#endif

	u32 calcu_ta[17] = {0};
	
	u32 itqt[2] = {0x81cc, 0x82cc};


	u32 iqk_ctrl_rfc_addr[2] = {0x5670, 0x7670};

	u32 process_id1[2] = {0x00001019, 0x00001029};
	u32 process_id2[2] = {0x00001519, 0x00001529};

	u32 gapk_on_tbl_setting[2] = {0x8158, 0x8258};

	
	RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]======> %s, gain_stage = 0x%x\n", __func__, gain_stage);

	halrf_wreg(rf, 0x80e4, 0x0000003f, 0x01); //set PA_GAPK_INDEX[5:0] when PAD GapK
	
	halrf_wreg(rf, 0x801c, 0x000e0000, 0x2); //Rx_P_avg

	halrf_wreg(rf, 0x80e0, 0x000001f0, 0x07); //Send flag to ITQT controller
	
	halrf_wreg(rf, 0x80e0, 0x0000f000, 0x0); //gapk_on_table_setting
	halrf_wreg(rf, 0x8038, 0x003f0000, 0x2d); //ItQt=100_100


	//halrf_wreg(rf, gapk_on_tbl_setting[path], 0x001fffff, 0x001554);
	halrf_wreg(rf, gapk_on_tbl_setting[path], 0x001fffff, gain_stage);
	
	//for debug
	//halrf_rrf(rf, path, 0x1005e, MASKRF);

	halrf_wreg(rf, iqk_ctrl_rfc_addr[path], 0x00000002, 0x1); //IQK cotrol	
	halrf_wreg(rf, itqt[path], 0x0000003f, 0x12);  // ItQt
	halrf_wreg(rf, 0x802c, 0x0fff0000, 0x009); //rx_tone_idx

#if 0
	halrf_before_one_shot_enable_8852c(rf);
	halrf_wreg(rf, 0x8000, MASKDWORD, cal_path_step1[path]); //set cal_path, process id 
	halrf_one_shot_nctl_done_check_8852c(rf, path);	
	//halrf_wreg(rf, 0x8010, 0x000000ff, 0x00);
#else
	txgapk_info->is_txgapk_ok = halrf_do_one_shot_8852c(rf, path, 0x8000, MASKDWORD, process_id1[path]);

	txgapk_info->txgapk_chk_cnt[path][TXGAPK_TRACK][0] = rf->nctl_ck_times[0]; 
	txgapk_info->txgapk_chk_cnt[path][TXGAPK_TRACK][1] = rf->nctl_ck_times[1];
#endif

	
	
	halrf_wreg(rf, itqt[path], 0x0000003f, 0x3f); // ItQt

#if 0	
	halrf_before_one_shot_enable_8852c(rf);
	halrf_wreg(rf, 0x8000, MASKDWORD, cal_path_step2[path]);
	halrf_one_shot_nctl_done_check_8852c(rf, path);
	//halrf_wreg(rf, 0x8010, 0x000000ff, 0x00);
#else
	txgapk_info->is_txgapk_ok = halrf_do_one_shot_8852c(rf, path, 0x8000, MASKDWORD, process_id2[path]);
#endif

		
	halrf_wreg(rf, iqk_ctrl_rfc_addr[path], 0x00000002, 0x0); //IQK cotrol RFC
	halrf_wreg(rf, 0x80e0, 0x00000001, 0x0); //Psd_Gapk_en
	//  ========END : Do Track GapK =====


#if TXGAPK_DBG
	// For debug
	RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]======> rf0x1005c [17:11] = 0x%x\n", halrf_rrf(rf, path, 0x1005c, 0x3f800));
	RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]======> rf0x1005c [5:0] = 0x%x\n",halrf_rrf(rf, path, 0x1005c, 0x0003f) );
	RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]======> rf0x1005e [17:12] = 0x%x\n", halrf_rrf(rf, path, 0x1005e, 0x3f000));
	RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]======> rf0x1005e [5:0] = 0x%x\n", halrf_rrf(rf, path, 0x1005e, 0x0003f));
	RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]======> rf0x11 [1:0] = 0x%x\n", halrf_rrf(rf, path, 0x11, 0x00003));
	RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]======> rf0x11 [6:4] = 0x%x\n", halrf_rrf(rf, path, 0x11, 0x00070));
	RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]======> rf0x11 [16:12] = 0x%x\n", halrf_rrf(rf, path, 0x11, 0x1f000));
#endif	


#ifndef HALRF_CONFIG_FW_IO_OFLD_SUPPORT

	//gapk_report
	halrf_wreg(rf, 0x80d4, MASKDWORD, 0x00130000);
	
	halrf_wreg(rf, 0x80e4, 0x00000f00, 0x3);
#if 0
	d[0] = halrf_rreg(rf, 0x80fc, 0x0000007f);
	d[1] = halrf_rreg(rf, 0x80fc, 0x00003f80);
	d[2] = halrf_rreg(rf, 0x80fc, 0x001fc000);
	d[3] = halrf_rreg(rf, 0x80fc, 0x0fe00000);
#else
	rf_tmp = halrf_rreg(rf, 0x80fc, 0x0fffffff);
	d[0] = rf_tmp & 0x0000007f;
	d[1] = rf_tmp & 0x00003f80;
	d[2] = rf_tmp & 0x001fc000;
	d[3] = rf_tmp & 0x0fe00000;

	d[1] = d[1] >> 7;
	d[2] = d[2] >> 14;
	d[3] = d[3] >> 21;
#endif

	halrf_wreg(rf, 0x80e4, 0x00000f00, 0x4);
#if 0
	d[4] = halrf_rreg(rf, 0x80fc, 0x0000007f);
	d[5] = halrf_rreg(rf, 0x80fc, 0x00003f80);
	d[6] = halrf_rreg(rf, 0x80fc, 0x001fc000);
	d[7] = halrf_rreg(rf, 0x80fc, 0x0fe00000);
#else
	rf_tmp = halrf_rreg(rf, 0x80fc, 0x0fffffff);
	d[4] = rf_tmp & 0x0000007f;
	d[5] = rf_tmp & 0x00003f80;
	d[6] = rf_tmp & 0x001fc000;
	d[7] = rf_tmp & 0x0fe00000;

	d[5] = d[5] >> 7;
	d[6] = d[6] >> 14;
	d[7] = d[7] >> 21;
#endif

	halrf_wreg(rf, 0x80e4, 0x00000f00, 0x5);
#if 0
	d[8] = halrf_rreg(rf, 0x80fc, 0x0000007f);
	d[9] = halrf_rreg(rf, 0x80fc, 0x00003f80);
	d[10] = halrf_rreg(rf, 0x80fc, 0x001fc000);
	d[11] = halrf_rreg(rf, 0x80fc, 0x0fe00000);
#else
	rf_tmp = halrf_rreg(rf, 0x80fc, 0x0fffffff);
	d[8] = rf_tmp & 0x0000007f;
	d[9] = rf_tmp & 0x00003f80;
	d[10] = rf_tmp & 0x001fc000;
	d[11] = rf_tmp & 0x0fe00000;

	d[9] = d[9] >> 7;
	d[10] = d[10] >> 14;
	d[11] = d[11] >> 21;
#endif

	halrf_wreg(rf, 0x80e4, 0x00000f00, 0x6);
#if 0
	d[12] = halrf_rreg(rf, 0x80fc, 0x0000007f);
	d[13] = halrf_rreg(rf, 0x80fc, 0x00003f80);
	d[14] = halrf_rreg(rf, 0x80fc, 0x001fc000);
	d[15] = halrf_rreg(rf, 0x80fc, 0x0fe00000);
#else
	rf_tmp = halrf_rreg(rf, 0x80fc, 0x0fffffff);
	d[12] = rf_tmp & 0x0000007f;
	d[13] = rf_tmp & 0x00003f80;
	d[14] = rf_tmp & 0x001fc000;
	d[15] = rf_tmp & 0x0fe00000;

	d[13] = d[13] >> 7;
	d[14] = d[14] >> 14;
	d[15] = d[15] >> 21;
#endif

	halrf_wreg(rf, 0x80e4, 0x00000f00, 0x7);
	d[16] = halrf_rreg(rf, 0x80fc, 0x0000007f);

#if 0
	halrf_wreg(rf, 0x80e4, 0x00000f00, 0x9);
	ta[0] = halrf_rreg(rf, 0x80fc, 0x000000ff);
	ta[1] = halrf_rreg(rf, 0x80fc, 0x0000ff00);
	ta[2] = halrf_rreg(rf, 0x80fc, 0x00ff0000);
	ta[3] = halrf_rreg(rf, 0x80fc, 0xff000000);

	
	halrf_wreg(rf, 0x80e4, 0x00000f00, 0xa);
	ta[4] = halrf_rreg(rf, 0x80fc, 0x000000ff);
	ta[5] = halrf_rreg(rf, 0x80fc, 0x0000ff00);
	ta[6] = halrf_rreg(rf, 0x80fc, 0x00ff0000);
	ta[7] = halrf_rreg(rf, 0x80fc, 0xff000000);

	
	halrf_wreg(rf, 0x80e4, 0x00000f00, 0xb);
	ta[8] = halrf_rreg(rf, 0x80fc, 0x000000ff);
	ta[9] = halrf_rreg(rf, 0x80fc, 0x0000ff00);
	ta[10] = halrf_rreg(rf, 0x80fc, 0x00ff0000);
	ta[11] = halrf_rreg(rf, 0x80fc, 0xff000000);

	
	halrf_wreg(rf, 0x80e4, 0x00000f00, 0xc);
	ta[12] = halrf_rreg(rf, 0x80fc, 0x000000ff);
	ta[13] = halrf_rreg(rf, 0x80fc, 0x0000ff00);
	ta[14] = halrf_rreg(rf, 0x80fc, 0x00ff0000);
	ta[15] = halrf_rreg(rf, 0x80fc, 0xff000000);

	
	halrf_wreg(rf, 0x80e4, 0x00000f00, 0xd);
	ta[16] = halrf_rreg(rf, 0x80fc, 0x000000ff);
#endif
	// Get The Value --> Ta 6bit
	halrf_wreg(rf, 0x80e4, 0x00000f00, 0x9);
#if 0
	calcu_ta[0] = halrf_rreg(rf, 0x80fc, 0x0000007e);
	calcu_ta[1] = halrf_rreg(rf, 0x80fc, 0x00007e00);
	calcu_ta[2] = halrf_rreg(rf, 0x80fc, 0x007e0000);
	calcu_ta[3] = halrf_rreg(rf, 0x80fc, 0x7e000000);
#else
	rf_tmp = halrf_rreg(rf, 0x80fc, 0x7e7e7e7f) & 0x7e7e7e7e;
	calcu_ta[0] = rf_tmp & (0x0000007e);
	calcu_ta[1] = rf_tmp & (0x00007e00);
	calcu_ta[2] = rf_tmp & (0x007e0000);
	calcu_ta[3] = rf_tmp & (0x7e000000);

	
	calcu_ta[0] = calcu_ta[0] >> 1;
	calcu_ta[1] = calcu_ta[1] >> 9;
	calcu_ta[2] = calcu_ta[2] >> 17;
	calcu_ta[3] = calcu_ta[3] >> 25;
#endif
	halrf_wreg(rf, 0x80e4, 0x00000f00, 0xa);
#if 0
	calcu_ta[4] = halrf_rreg(rf, 0x80fc, 0x0000007e);
	calcu_ta[5] = halrf_rreg(rf, 0x80fc, 0x00007e00);
	calcu_ta[6] = halrf_rreg(rf, 0x80fc, 0x007e0000);
	calcu_ta[7] = halrf_rreg(rf, 0x80fc, 0x7e000000);
#else
	rf_tmp = halrf_rreg(rf, 0x80fc, 0x7e7e7e7f) & 0x7e7e7e7e;
	calcu_ta[4] = rf_tmp & (0x0000007e);
	calcu_ta[5] = rf_tmp & (0x00007e00);
	calcu_ta[6] = rf_tmp & (0x007e0000);
	calcu_ta[7] = rf_tmp & (0x7e000000);	
		
	calcu_ta[4] = calcu_ta[4] >> 1;
	calcu_ta[5] = calcu_ta[5] >> 9;
	calcu_ta[6] = calcu_ta[6] >> 17;
	calcu_ta[7] = calcu_ta[7] >> 25;
#endif		
	
	halrf_wreg(rf, 0x80e4, 0x00000f00, 0xb);
#if 0
	calcu_ta[8] = halrf_rreg(rf, 0x80fc, 0x0000007e);
	calcu_ta[9] = halrf_rreg(rf, 0x80fc, 0x00007e00);
	calcu_ta[10] = halrf_rreg(rf, 0x80fc, 0x007e0000);
	calcu_ta[11] = halrf_rreg(rf, 0x80fc, 0x7e000000);
#else
	rf_tmp = halrf_rreg(rf, 0x80fc, 0x7e7e7e7f) & 0x7e7e7e7e;
	calcu_ta[8] = rf_tmp & (0x0000007e);
	calcu_ta[9] = rf_tmp & (0x00007e00);
	calcu_ta[10] = rf_tmp & (0x007e0000);
	calcu_ta[11] = rf_tmp & (0x7e000000);
			
	calcu_ta[8] = calcu_ta[8] >> 1;
	calcu_ta[9] = calcu_ta[9] >> 9;
	calcu_ta[10] = calcu_ta[10] >> 17;
	calcu_ta[11] = calcu_ta[11] >> 25;
#endif

	halrf_wreg(rf, 0x80e4, 0x00000f00, 0xc);
#if 0
	calcu_ta[12] = halrf_rreg(rf, 0x80fc, 0x0000007e);
	calcu_ta[13] = halrf_rreg(rf, 0x80fc, 0x00007e00);
	calcu_ta[14] = halrf_rreg(rf, 0x80fc, 0x007e0000);
	calcu_ta[15] = halrf_rreg(rf, 0x80fc, 0x7e000000);
#else
	rf_tmp = halrf_rreg(rf, 0x80fc, 0x7e7e7e7f) & 0x7e7e7e7e;
	calcu_ta[12] = rf_tmp & (0x0000007e);
	calcu_ta[13] = rf_tmp & (0x00007e00);
	calcu_ta[14] = rf_tmp & (0x007e0000);
	calcu_ta[15] = rf_tmp & (0x7e000000);
	
		
	calcu_ta[12] = calcu_ta[12] >> 1;
	calcu_ta[13] = calcu_ta[13] >> 9;
	calcu_ta[14] = calcu_ta[14] >> 17;
	calcu_ta[15] = calcu_ta[15] >> 25;
#endif

	halrf_wreg(rf, 0x80e4, 0x00000f00, 0xd);
	calcu_ta[16] = halrf_rreg(rf, 0x80fc, 0x0000007e);

#endif




#if TXGAPK_DBG	 
	halrf_wreg(rf, 0x80e4, 0x00000f00, 0x2);
	RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]======> 0x80fc [6:0] = 0x%x\n", halrf_rreg(rf, 0x80fc, 0x0000007f));
	 
	halrf_wreg(rf, 0x80e4, 0x00000f00, 0x0);
	RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]======> 0x80fc = 0x%x\n", halrf_rreg(rf, 0x80fc, MASKDWORD));
	 
	halrf_wreg(rf, 0x80e4, 0x00000f00, 0x2);
	RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]======> 0x80fc [22:16] = 0x%x\n", halrf_rreg(rf, 0x80fc, 0x007f0000));
	 
	halrf_wreg(rf, 0x80e4, 0x00000f00, 0x1);
	RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]======>psd_pwr1 0x80fc = 0x%x\n", halrf_rreg(rf, 0x80fc, MASKDWORD));
	 
	halrf_wreg(rf, 0x80e4, 0x00000f00, 0xf);
	RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]======>gain_out 0x80fc = 0x%x\n", halrf_rreg(rf, 0x80fc, MASKDWORD));

	// continuous single tone
	halrf_wreg(rf, itqt[path], MASKDWORD, 0x24); //ItQt
	halrf_wreg(rf, 0x80d0, 0x00100000, 0x1); //tst_iqk2set
#endif
//=========================
	for (i = 0; i < 17; i++) {
		if (d[i] & BIT(6))
			txgapk_info->track_d[path][i] = (s32)(d[i] | 0xffffff80);
		else
			txgapk_info->track_d[path][i] = (s32)(d[i]);

		/*RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]	track	d[%d][%d]=0x%x\n",
			path, i, txgapk_info->track_d[path][i]);*/
		RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]	track	d[%d][%d]=0x%x\n",
			path, i, d[i]);
	}

#if 0
	for (i = 0; i < 17; i++) {
		if (ta[i] & BIT(7))
			txgapk_info->track_ta[path][i] = (s32)(ta[i] | 0xffffff00);
		else
			txgapk_info->track_ta[path][i] = (s32)(ta[i]);
		
		/*RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]	track	ta[%d][%d]=0x%x\n",
			path, i, txgapk_info->track_ta[path][i]);*/
		
		RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]	track	ta[%d][%d]=0x%x\n",
			path, i, ta[i]);
	}
#endif

	for (i = 0; i < 17; i++) {
		//6bit
		if (calcu_ta[i] & BIT(5))
			txgapk_info->track_ta[path][i] = (s32)(calcu_ta[i] | 0xffffffc0);
		else
			txgapk_info->track_ta[path][i] = (s32)(calcu_ta[i]);
			
		/*RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK] track	calcu_ta[%d][%d]=0x%x\n",
			path, i, txgapk_info->track_ta[path][i]);*/
		RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK] track	calcu_ta[%d][%d]=0x%x\n",
			path, i, calcu_ta[i]);
	}



}


void _halrf_txgapk_track_table_nctl_5g_8852c
	(struct rf_info *rf, enum phl_phy_idx phy, enum rf_path path, u32 gain_stage)
{
	struct halrf_gapk_info *txgapk_info = &rf->gapk;
	u8 rfe_type = rf->phl_com->dev_cap.rfe_type;

	u32 i;
	u32 d[17] = {0};
	//u32 ta[17] = {0};
	u32 calcu_ta[17] = {0};
#ifndef HALRF_CONFIG_FW_IO_OFLD_SUPPORT
	u32 rf_tmp = 0;
#endif

	
	u32 itqt[2] = {0x81cc, 0x82cc};
	
	u32 iqk_ctrl_rfc_addr[2] = {0x5670, 0x7670};
	
	u32 process_id1[2] = {0x00001019, 0x00001029};
	u32 process_id2[2] = {0x00001519, 0x00001529};

	u32 gapk_on_tbl_setting[2] = {0x8158, 0x8258};

	RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]======> %s, gain_stage = 0x%x\n", __func__, gain_stage);


	halrf_wreg(rf, 0x80e4, 0x0000003f, 0x01); //set PA_GAPK_INDEX
	halrf_wreg(rf, 0x801c, 0x000e0000, 0x2); //Rx_P_avg
	halrf_wreg(rf, 0x80e0, 0x000001f0, 0x07); //Send flag to ITQT controller
	halrf_wreg(rf, 0x80e0, 0x0000f000, 0x0); //Gmode 11g 


	if (rfe_type <= 50) //ifem
		//halrf_wreg(rf, 0x8038, 0x003f0000, 0x2d); //52B GapK 2nd ItQt
		halrf_wreg(rf, 0x8038, 0x003f0000, 0x36); //52B GapK 2nd ItQt
	else //efem
		halrf_wreg(rf, 0x8038, 0x003f0000, 0x24); //52B GapK 2nd ItQt

	//debug
	RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]trk 0x8038[21:16] = 0x%x\n", halrf_rreg(rf, 0x8038, 0x003f0000));
	
	halrf_wreg(rf, gapk_on_tbl_setting[path], 0x001fffff, gain_stage); //Amode

	//for debug
	//halrf_rrf(rf, path, 0x1005e, MASKRF);
	
	halrf_wreg(rf, iqk_ctrl_rfc_addr[path], 0x00000002, 0x1); //IQK cotrol RFC


	if (rfe_type <= 50) //ifem
		//	halrf_wreg(rf, itqt[path], 0x0000003f, 0x12); // ItQt
		halrf_wreg(rf, itqt[path], 0x0000003f, 0x24); // ItQt
	else //efem
		halrf_wreg(rf, itqt[path], 0x0000003f, 0x09); // ItQt

	
	//debug
	RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]trk 0x%x[5:0] = 0x%x\n", itqt[path], halrf_rreg(rf, itqt[path], 0x0000003f));
		
	halrf_wreg(rf, 0x802c, 0x0fff0000, 0x009); //rx_tone_idx

#if 0
	halrf_before_one_shot_enable_8852c(rf);
	halrf_wreg(rf, 0x8000, MASKDWORD, cal_path_step1[path]); //set cal_path, process id 
	halrf_one_shot_nctl_done_check_8852c(rf, path);
	//halrf_wreg(rf, 0x8010, 0x000000ff, 0x00);
#else
	txgapk_info->is_txgapk_ok = halrf_do_one_shot_8852c(rf, path, 0x8000, MASKDWORD, process_id1[path]);

	txgapk_info->txgapk_chk_cnt[path][TXGAPK_TRACK][0] = rf->nctl_ck_times[0]; 
	txgapk_info->txgapk_chk_cnt[path][TXGAPK_TRACK][1] = rf->nctl_ck_times[1];
#endif


	halrf_wreg(rf, itqt[path], 0x0000003f, 0x3f); // ItQt

#if 0	
	halrf_before_one_shot_enable_8852c(rf);
	halrf_wreg(rf, 0x8000, MASKDWORD, cal_path_step2[path]); //set cal_path, process id 
	halrf_one_shot_nctl_done_check_8852c(rf, path);
	//halrf_wreg(rf, 0x8010, 0x000000ff, 0x00);
#else
	txgapk_info->is_txgapk_ok = halrf_do_one_shot_8852c(rf, path, 0x8000, MASKDWORD, process_id2[path]);
#endif
	
	halrf_wreg(rf, iqk_ctrl_rfc_addr[path], 0x00000002, 0x0); //IQK cotrol RFC
	halrf_wreg(rf, 0x80e0, 0x00000001, 0x0); //=Psd_Gapk_en
	//  ========END : Do Track GapK =====

#if TXGAPK_DBG
	// ==== check performance
	RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]======> rf0x1005c [17:11] = 0x%x\n", halrf_rrf(rf, path, 0x1005c, 0x3f800));
	RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]======> rf0x1005c [5:0] = 0x%x\n", halrf_rrf(rf, path, 0x1005c, 0x0003f));
	RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]======> rf0x1005e [17:12] = 0x%x\n", halrf_rrf(rf, path, 0x1005e, 0x3f000));
	RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]======> rf0x1005e [5:0] = 0x%x\n", halrf_rrf(rf, path, 0x1005e, 0x0003f));
	RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]======> rf0x11 [1:0] = 0x%x\n", halrf_rrf(rf, path, 0x11, 0x00003));
	RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]======> rf0x11 [6:4] = 0x%x\n", halrf_rrf(rf, path, 0x11, 0x00070));
	RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]======> rf0x11 [16:12] = 0x%x\n", halrf_rrf(rf, path, 0x11, 0x1f000));
#endif


#ifndef HALRF_CONFIG_FW_IO_OFLD_SUPPORT

	//read report 
	halrf_wreg(rf, 0x80d4, MASKDWORD, 0x00130000);
	
	halrf_wreg(rf, 0x80e4, 0x00000f00, 0x3);
#if 0
	d[0] = halrf_rreg(rf, 0x80fc, 0x0000007f);
	d[1] = halrf_rreg(rf, 0x80fc, 0x00003f80);
	d[2] = halrf_rreg(rf, 0x80fc, 0x001fc000);
	d[3] = halrf_rreg(rf, 0x80fc, 0x0fe00000);
#else
	rf_tmp = halrf_rreg(rf, 0x80fc, 0x0fffffff);
	d[0] = rf_tmp & (0x0000007f);
	d[1] = rf_tmp & (0x00003f80);
	d[2] = rf_tmp & (0x001fc000);
	d[3] = rf_tmp & (0x0fe00000);

	d[1] = d[1] >> 7;
	d[2] = d[2] >> 14;
	d[3] = d[3] >> 21;
#endif

	halrf_wreg(rf, 0x80e4, 0x00000f00, 0x4);
#if 0
	d[4] = halrf_rreg(rf, 0x80fc, 0x0000007f);
	d[5] = halrf_rreg(rf, 0x80fc, 0x00003f80);
	d[6] = halrf_rreg(rf, 0x80fc, 0x001fc000);
	d[7] = halrf_rreg(rf, 0x80fc, 0x0fe00000);
#else
	rf_tmp = halrf_rreg(rf, 0x80fc, 0x0fffffff);
	d[4] = rf_tmp & (0x0000007f);
	d[5] = rf_tmp & (0x00003f80);
	d[6] = rf_tmp & (0x001fc000);
	d[7] = rf_tmp & (0x0fe00000);

	d[5] = d[5] >> 7;
	d[6] = d[6] >> 14;
	d[7] = d[7] >> 21;
#endif

	halrf_wreg(rf, 0x80e4, 0x00000f00, 0x5);
#if 0
	d[8] = halrf_rreg(rf, 0x80fc, 0x0000007f);
	d[9] = halrf_rreg(rf, 0x80fc, 0x00003f80);
	d[10] = halrf_rreg(rf, 0x80fc, 0x001fc000);
	d[11] = halrf_rreg(rf, 0x80fc, 0x0fe00000);
#else
	rf_tmp = halrf_rreg(rf, 0x80fc, 0x0fffffff);
	d[8] = rf_tmp & (0x0000007f);
	d[9] = rf_tmp & (0x00003f80);
	d[10] = rf_tmp & (0x001fc000);
	d[11] = rf_tmp & (0x0fe00000);

	d[9] = d[9] >> 7;
	d[10] = d[10] >> 14;
	d[11] = d[11] >> 21;
#endif

	halrf_wreg(rf, 0x80e4, 0x00000f00, 0x6);
#if 0
	d[12] = halrf_rreg(rf, 0x80fc, 0x0000007f);
	d[13] = halrf_rreg(rf, 0x80fc, 0x00003f80);
	d[14] = halrf_rreg(rf, 0x80fc, 0x001fc000);
	d[15] = halrf_rreg(rf, 0x80fc, 0x0fe00000);
#else
	rf_tmp = halrf_rreg(rf, 0x80fc, 0x0fffffff);
	d[12] = rf_tmp & (0x0000007f);
	d[13] = rf_tmp & (0x00003f80);
	d[14] = rf_tmp & (0x001fc000);
	d[15] = rf_tmp & (0x0fe00000);

	d[13] = d[13] >> 7;
	d[14] = d[14] >> 14;
	d[15] = d[15] >> 21;
#endif

	halrf_wreg(rf, 0x80e4, 0x00000f00, 0x7);
	d[16] = halrf_rreg(rf, 0x80fc, 0x0000007f);


#if 0
	halrf_wreg(rf, 0x80e4, 0x00000f00, 0x9);
	ta[0] = halrf_rreg(rf, 0x80fc, 0x000000ff);
	ta[1] = halrf_rreg(rf, 0x80fc, 0x0000ff00);
	ta[2] = halrf_rreg(rf, 0x80fc, 0x00ff0000);
	ta[3] = halrf_rreg(rf, 0x80fc, 0xff000000);

	halrf_wreg(rf, 0x80e4, 0x00000f00, 0xa);
	ta[4] = halrf_rreg(rf, 0x80fc, 0x000000ff);
	ta[5] = halrf_rreg(rf, 0x80fc, 0x0000ff00);
	ta[6] = halrf_rreg(rf, 0x80fc, 0x00ff0000);
	ta[7] = halrf_rreg(rf, 0x80fc, 0xff000000);

	halrf_wreg(rf, 0x80e4, 0x00000f00, 0xb);
	ta[8] = halrf_rreg(rf, 0x80fc, 0x000000ff);
	ta[9] = halrf_rreg(rf, 0x80fc, 0x0000ff00);
	ta[10] = halrf_rreg(rf, 0x80fc, 0x00ff0000);
	ta[11] = halrf_rreg(rf, 0x80fc, 0xff000000);

	halrf_wreg(rf, 0x80e4, 0x00000f00, 0xc);
	ta[12] = halrf_rreg(rf, 0x80fc, 0x000000ff);
	ta[13] = halrf_rreg(rf, 0x80fc, 0x0000ff00);
	ta[14] = halrf_rreg(rf, 0x80fc, 0x00ff0000);
	ta[15] = halrf_rreg(rf, 0x80fc, 0xff000000);

	halrf_wreg(rf, 0x80e4, 0x00000f00, 0xd);
	ta[16] = halrf_rreg(rf, 0x80fc, 0x000000ff);
#endif

	// Get The Value --> Ta 6bit
	halrf_wreg(rf, 0x80e4, 0x00000f00, 0x9);
#if 0
	calcu_ta[0] = halrf_rreg(rf, 0x80fc, 0x0000007e);
	calcu_ta[1] = halrf_rreg(rf, 0x80fc, 0x00007e00);
	calcu_ta[2] = halrf_rreg(rf, 0x80fc, 0x007e0000);
	calcu_ta[3] = halrf_rreg(rf, 0x80fc, 0x7e000000);
#else
	rf_tmp = halrf_rreg(rf, 0x80fc, 0x7e7e7e7f) & 0x7e7e7e7e;
	calcu_ta[0] = rf_tmp & (0x0000007e);
	calcu_ta[1] = rf_tmp & (0x00007e00);
	calcu_ta[2] = rf_tmp & (0x007e0000);
	calcu_ta[3] = rf_tmp & (0x7e000000);

	
	calcu_ta[0] = calcu_ta[0] >> 1;
	calcu_ta[1] = calcu_ta[1] >> 9;
	calcu_ta[2] = calcu_ta[2] >> 17;
	calcu_ta[3] = calcu_ta[3] >> 25;
#endif
	halrf_wreg(rf, 0x80e4, 0x00000f00, 0xa);	
#if 0
	calcu_ta[4] = halrf_rreg(rf, 0x80fc, 0x0000007e);
	calcu_ta[5] = halrf_rreg(rf, 0x80fc, 0x00007e00);
	calcu_ta[6] = halrf_rreg(rf, 0x80fc, 0x007e0000);
	calcu_ta[7] = halrf_rreg(rf, 0x80fc, 0x7e000000);
#else
	rf_tmp = halrf_rreg(rf, 0x80fc, 0x7e7e7e7f) & 0x7e7e7e7e;
	calcu_ta[4] = rf_tmp & (0x0000007e);
	calcu_ta[5] = rf_tmp & (0x00007e00);
	calcu_ta[6] = rf_tmp & (0x007e0000);
	calcu_ta[7] = rf_tmp & (0x7e000000);
	
		
	calcu_ta[4] = calcu_ta[4] >> 1;
	calcu_ta[5] = calcu_ta[5] >> 9;
	calcu_ta[6] = calcu_ta[6] >> 17;
	calcu_ta[7] = calcu_ta[7] >> 25;
#endif		
	halrf_wreg(rf, 0x80e4, 0x00000f00, 0xb);
#if 0
	calcu_ta[8] = halrf_rreg(rf, 0x80fc, 0x0000007e);
	calcu_ta[9] = halrf_rreg(rf, 0x80fc, 0x00007e00);
	calcu_ta[10] = halrf_rreg(rf, 0x80fc, 0x007e0000);
	calcu_ta[11] = halrf_rreg(rf, 0x80fc, 0x7e000000);
#else
	rf_tmp = halrf_rreg(rf, 0x80fc, 0x7e7e7e7f) & 0x7e7e7e7e;
	calcu_ta[8] = rf_tmp & (0x0000007e);
	calcu_ta[9] = rf_tmp & (0x00007e00);
	calcu_ta[10] = rf_tmp & (0x007e0000);
	calcu_ta[11] = rf_tmp & (0x7e000000);
			
	calcu_ta[8] = calcu_ta[8] >> 1;
	calcu_ta[9] = calcu_ta[9] >> 9;
	calcu_ta[10] = calcu_ta[10] >> 17;
	calcu_ta[11] = calcu_ta[11] >> 25;
#endif
	
	halrf_wreg(rf, 0x80e4, 0x00000f00, 0xc);
#if 0
	calcu_ta[12] = halrf_rreg(rf, 0x80fc, 0x0000007e);
	calcu_ta[13] = halrf_rreg(rf, 0x80fc, 0x00007e00);
	calcu_ta[14] = halrf_rreg(rf, 0x80fc, 0x007e0000);
	calcu_ta[15] = halrf_rreg(rf, 0x80fc, 0x7e000000);
#else
	rf_tmp = halrf_rreg(rf, 0x80fc, 0x7e7e7e7f) & 0x7e7e7e7e;
	calcu_ta[12] = rf_tmp & (0x0000007e);
	calcu_ta[13] = rf_tmp & (0x00007e00);
	calcu_ta[14] = rf_tmp & (0x007e0000);
	calcu_ta[15] = rf_tmp & (0x7e000000);
		
	calcu_ta[12] = calcu_ta[12] >> 1;
	calcu_ta[13] = calcu_ta[13] >> 9;
	calcu_ta[14] = calcu_ta[14] >> 17;
	calcu_ta[15] = calcu_ta[15] >> 25;
#endif	
	halrf_wreg(rf, 0x80e4, 0x00000f00, 0xd);
	calcu_ta[16] = halrf_rreg(rf, 0x80fc, 0x0000007e);

#endif


#if 0	 
	halrf_wreg(rf, 0x80e4, 0x00000f00, 0x2);
	 = halrf_rreg(rf, 0x80fc, 0x0000007f);
	halrf_wreg(rf, 0x80e4, 0x00000f00, 0x0);
	 = halrf_rreg(rf, 0x80fc, MASKDWORD);
	halrf_wreg(rf, 0x80e4, 0x00000f00, 0x2);
	 = halrf_rreg(rf, 0x80fc, 0x007f0000);
	halrf_wreg(rf, 0x80e4, 0x00000f00, 0x1);
	 = halrf_rreg(rf, 0x80fc, MASKDWORD);
	halrf_wreg(rf, 0x80e4, 0x00000f00, 0xf);
	 = halrf_rreg(rf, 0x80fc, MASKDWORD);
	
	// continuous single tone
	halrf_wreg(rf, itqt[path], MASKDWORD, 0x24); //ItQt
	halrf_wreg(rf, 0x80d0, 0x00100000, 0x1); //tst_iqk2set
#endif	
	//============================

	for (i = 0; i < 17; i++) {
		if (d[i] & BIT(6))
			txgapk_info->track_d[path][i] = (s32)(d[i] | 0xffffff80);
		else
			txgapk_info->track_d[path][i] = (s32)(d[i]);

		/*RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]	track	d[%d][%d]=0x%x\n",
			path, i, txgapk_info->track_d[path][i]);*/

		RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]	track	d[%d][%d]=0x%x\n",
			path, i, d[i]);	
	}

#if 0
	for (i = 0; i < 17; i++) {
		if (ta[i] & BIT(7))
			txgapk_info->track_ta[path][i] = (s32)(ta[i] | 0xffffff00);
		else
			txgapk_info->track_ta[path][i] = (s32)(ta[i]);
		
		/*RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]	track	ta[%d][%d]=0x%x\n",
			path, i, txgapk_info->track_ta[path][i]);*/
		RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]	track	ta[%d][%d]=0x%x\n",
			path, i, ta[i]);
	}
#endif
	for (i = 0; i < 17; i++) {
		//6bit
		if (calcu_ta[i] & BIT(5))
			txgapk_info->track_ta[path][i] = (s32)(calcu_ta[i] | 0xffffffc0);
		else
			txgapk_info->track_ta[path][i] = (s32)(calcu_ta[i]);
		
		/*RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]	track	calcu_ta[%d][%d]=0x%x\n",
			path, i, txgapk_info->track_ta[path][i]);*/
		RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]	track	calcu_ta[%d][%d]=0x%x\n",
			path, i, calcu_ta[i]);
	}


}


void _halrf_txgapk_track_table_nctl_8852c
	(struct rf_info *rf, enum phl_phy_idx phy, enum rf_path path)
{
	u8 channel = rf->hal_com->band[phy].cur_chandef.center_ch;
	u32 band = rf->hal_com->band[phy].cur_chandef.band;
	u32 group_of_6g = 0;
	u8 rfe_type = rf->phl_com->dev_cap.rfe_type;


	u32 gain_stage_2g = 0x1554;
	//u32 gain_stage_5g = 0x11554;
	u32 gain_stage_5g[3] = {0x6aa8, 0xd550, 0xd550};
	u32 gain_stage_5g_efem = 0x2aa8;
	
	//u32 gain_stage_6g[4] = {0x10aaa, 0x11554, 0x11554, 0x10aaa}; //LB MB HB UHB
	u32 gain_stage_6g[2][4] = {{0xd4d0, 0xd550, 0xd4d0, 0x6a68}, 
							    {0xd4d0, 0xd550, 0xd250, 0x6964}}; //LB MB HB UHB


	group_of_6g = halrf_rrf(rf, path, 0x17, 0x6000);
	
	if (rfe_type > 50) { //efem
		if (band == BAND_ON_24G) //TBD
			_halrf_txgapk_track_table_nctl_2g_8852c(rf, phy, path, gain_stage_2g);
		else
			_halrf_txgapk_track_table_nctl_5g_8852c(rf, phy, path, gain_stage_5g_efem);
	} else {
	//if (channel >= 1 && channel <= 14)
		if (band == BAND_ON_24G)
			_halrf_txgapk_track_table_nctl_2g_8852c(rf, phy, path, gain_stage_2g);
		else if (band == BAND_ON_5G) {
			if (channel >= 36 && channel <= 64)
				_halrf_txgapk_track_table_nctl_5g_8852c(rf, phy, path, gain_stage_5g[0]);
			else if (channel >= 100 && channel <= 144)
				_halrf_txgapk_track_table_nctl_5g_8852c(rf, phy, path, gain_stage_5g[1]);
			else if (channel >= 149 && channel <= 177)
				_halrf_txgapk_track_table_nctl_5g_8852c(rf, phy, path, gain_stage_5g[2]);
		} else {
			_halrf_txgapk_track_table_nctl_5g_8852c(rf, phy, path, gain_stage_6g[path][group_of_6g]);
		}					
	}
}



void _halrf_txgapk_power_table_nctl_2g_8852c
	(struct rf_info *rf, enum phl_phy_idx phy, enum rf_path path, u32 gain_stage)
{
	struct halrf_gapk_info *txgapk_info = &rf->gapk;
	/* u8 rfe_type = rf->phl_com->dev_cap.rfe_type; */
	u32 i;
	u32 d[17] = {0};
	//u32 ta[17] = {0};
	u32 calcu_ta[17] = {0};

#ifndef HALRF_CONFIG_FW_IO_OFLD_SUPPORT
	u32 rf_tmp = 0;
#endif


	u32 itqt[2] = {0x81cc, 0x82cc};
	
	u32 iqk_ctrl_rfc_addr[2] = {0x5670, 0x7670};
	u32 gapk_on_tbl_setting[2] = {0x8170, 0x8270};
	
	u32 process_id1[2] = {0x00001119, 0x00001129};
	u32 process_id2[2] = {0x00001619, 0x00001629};


	RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]======> %s, gain_stage = 0x%x\n", __func__, gain_stage);

	halrf_wreg(rf, 0x80e0, 0x000001f0, 0x07); //Send flag to ITQT controller
	halrf_wreg(rf, 0x8038, 0x003f0000, 0x24); // 2nd ItQt
	

	//halrf_wreg(rf, gapk_on_tbl_setting[path], 0x001fffff, 0x000540); //gapk_on_table1_setting
	halrf_wreg(rf, gapk_on_tbl_setting[path], 0x001fffff, gain_stage); //gapk_on_table1_setting
	

	halrf_wreg(rf, iqk_ctrl_rfc_addr[path], 0x00000002, 0x1); //IQK cotrol RFC
	halrf_wreg(rf, itqt[path], MASKDWORD, 0x12); // ItQt
	halrf_wreg(rf, 0x802c, 0x0fff0000, 0x009); //rx_tone_idx

	#if 0
	halrf_before_one_shot_enable_8852c(rf);
	halrf_wreg(rf, 0x8000, MASKDWORD, cal_path_step1[path]);//set cal_path, process id
	halrf_one_shot_nctl_done_check_8852c(rf, path);
	//halrf_wreg(rf, 0x8010, 0x000000ff, 0x00);
	#else 
	txgapk_info->is_txgapk_ok = halrf_do_one_shot_8852c(rf, path, 0x8000, MASKDWORD, process_id1[path]);

	txgapk_info->txgapk_chk_cnt[path][TXGAPK_PWR][0] = rf->nctl_ck_times[0]; 
	txgapk_info->txgapk_chk_cnt[path][TXGAPK_PWR][1] = rf->nctl_ck_times[1];
	#endif


	halrf_wreg(rf, itqt[path], 0x0000003f, 0x3f);  // ItQt

	#if 0
	halrf_before_one_shot_enable_8852c(rf);
	halrf_wreg(rf, 0x8000, MASKDWORD, cal_path_step2[path]);
	halrf_one_shot_nctl_done_check_8852c(rf, path); //set cal_path, process id
	//halrf_wreg(rf, 0x8010, 0x000000ff, 0x00);
	#else
	txgapk_info->is_txgapk_ok = halrf_do_one_shot_8852c(rf, path, 0x8000, MASKDWORD, process_id2[path]);
	#endif

	
	halrf_wreg(rf, iqk_ctrl_rfc_addr[path], 0x00000002, 0x0); //path0, IQK cotrol RFC
	halrf_wreg(rf, 0x801c, 0x000e0000, 0x0); //Rx_P_avg
	//  ========END : Do PA GapK =====

#ifndef HALRF_CONFIG_FW_IO_OFLD_SUPPORT

	// ===== Read GapK Results, Bcut resolution = 0.0625 dB =====
	halrf_wreg(rf, 0x80d4, MASKDWORD, 0x00130000); //gapk_report
	
	halrf_wreg(rf, 0x80e4, 0x00000f00, 0x3);
#if 0
	d[0] = halrf_rreg(rf, 0x80fc, 0x0000007f);
	d[1] = halrf_rreg(rf, 0x80fc, 0x00003f80);
	d[2] = halrf_rreg(rf, 0x80fc, 0x001fc000);
	d[3] = halrf_rreg(rf, 0x80fc, 0x0fe00000);
#else
	rf_tmp = halrf_rreg(rf, 0x80fc, 0x0fffffff);
	d[0] = rf_tmp & (0x0000007f);
	d[1] = rf_tmp & (0x00003f80);
	d[2] = rf_tmp & (0x001fc000);
	d[3] = rf_tmp & (0x0fe00000);

	d[1] = d[1] >> 7;
	d[2] = d[2] >> 14;
	d[3] = d[3] >> 21;
#endif

	halrf_wreg(rf, 0x80e4, 0x00000f00, 0x4);
#if 0
	d[4] = halrf_rreg(rf, 0x80fc, 0x0000007f);
	d[5] = halrf_rreg(rf, 0x80fc, 0x00003f80);
	d[6] = halrf_rreg(rf, 0x80fc, 0x001fc000);
	d[7] = halrf_rreg(rf, 0x80fc, 0x0fe00000);
#else
	rf_tmp = halrf_rreg(rf, 0x80fc, 0x0fffffff);
	d[4] = rf_tmp & (0x0000007f);
	d[5] = rf_tmp & (0x00003f80);
	d[6] = rf_tmp & (0x001fc000);
	d[7] = rf_tmp & (0x0fe00000);

	d[5] = d[5] >> 7;
	d[6] = d[6] >> 14;
	d[7] = d[7] >> 21;
#endif

	halrf_wreg(rf, 0x80e4, 0x00000f00, 0x5);
#if 0
	d[8] = halrf_rreg(rf, 0x80fc, 0x0000007f);
	d[9] = halrf_rreg(rf, 0x80fc, 0x00003f80);
	d[10] = halrf_rreg(rf, 0x80fc, 0x001fc000);
	d[11] = halrf_rreg(rf, 0x80fc, 0x0fe00000);
#else
	rf_tmp = halrf_rreg(rf, 0x80fc, 0x0fffffff);
	d[8] = rf_tmp & (0x0000007f);
	d[9] = rf_tmp & (0x00003f80);
	d[10] = rf_tmp & (0x001fc000);
	d[11] = rf_tmp & (0x0fe00000);

	d[9] = d[9] >> 7;
	d[10] = d[10] >> 14;
	d[11] = d[11] >> 21;
#endif

	halrf_wreg(rf, 0x80e4, 0x00000f00, 0x6);
#if 0
	d[12] = halrf_rreg(rf, 0x80fc, 0x0000007f);
	d[13] = halrf_rreg(rf, 0x80fc, 0x00003f80);
	d[14] = halrf_rreg(rf, 0x80fc, 0x001fc000);
	d[15] = halrf_rreg(rf, 0x80fc, 0x0fe00000);
#else
	rf_tmp = halrf_rreg(rf, 0x80fc, 0x0fffffff);
	d[12] = rf_tmp & (0x0000007f);
	d[13] = rf_tmp & (0x00003f80);
	d[14] = rf_tmp & (0x001fc000);
	d[15] = rf_tmp & (0x0fe00000);

	d[13] = d[13] >> 7;
	d[14] = d[14] >> 14;
	d[15] = d[15] >> 21;
#endif

	halrf_wreg(rf, 0x80e4, 0x00000f00, 0x7);
	d[16] = halrf_rreg(rf, 0x80fc, 0x0000007f);

#if 0	 
	halrf_wreg(rf, 0x80e4, 0x00000f00, 0x9);
	ta[0] = halrf_rreg(rf, 0x80fc, 0x000000ff);
	ta[1] = halrf_rreg(rf, 0x80fc, 0x0000ff00);
	ta[2] = halrf_rreg(rf, 0x80fc, 0x00ff0000);
	ta[3] = halrf_rreg(rf, 0x80fc, 0xff000000);
	halrf_wreg(rf, 0x80e4, 0x00000f00, 0xa);
	ta[4] = halrf_rreg(rf, 0x80fc, 0x000000ff);
	ta[5] = halrf_rreg(rf, 0x80fc, 0x0000ff00);
	ta[6] = halrf_rreg(rf, 0x80fc, 0x00ff0000);
	ta[7] = halrf_rreg(rf, 0x80fc, 0xff000000);
	halrf_wreg(rf, 0x80e4, 0x00000f00, 0xb);
	ta[8] = halrf_rreg(rf, 0x80fc, 0x000000ff);
	ta[9] = halrf_rreg(rf, 0x80fc, 0x0000ff00);
	ta[10] = halrf_rreg(rf, 0x80fc, 0x00ff0000);
	ta[11] = halrf_rreg(rf, 0x80fc, 0xff000000);
	halrf_wreg(rf, 0x80e4, 0x00000f00, 0xc);
	ta[12] = halrf_rreg(rf, 0x80fc, 0x000000ff);
	ta[13] = halrf_rreg(rf, 0x80fc, 0x0000ff00);
	ta[14] = halrf_rreg(rf, 0x80fc, 0x00ff0000);
	ta[15] = halrf_rreg(rf, 0x80fc, 0xff000000);
	halrf_wreg(rf, 0x80e4, 0x00000f00, 0xd);
	ta[16] = halrf_rreg(rf, 0x80fc, 0x000000ff);
#endif
	// Get The Value --> Ta 6bit
	halrf_wreg(rf, 0x80e4, 0x00000f00, 0x9);
#if 0
	calcu_ta[0] = halrf_rreg(rf, 0x80fc, 0x0000007e);
	calcu_ta[1] = halrf_rreg(rf, 0x80fc, 0x00007e00);
	calcu_ta[2] = halrf_rreg(rf, 0x80fc, 0x007e0000);
	calcu_ta[3] = halrf_rreg(rf, 0x80fc, 0x7e000000);
#else
	rf_tmp = halrf_rreg(rf, 0x80fc, 0x7e7e7e7f) & 0x7e7e7e7e;
	calcu_ta[0] = rf_tmp & (0x0000007e);
	calcu_ta[1] = rf_tmp & (0x00007e00);
	calcu_ta[2] = rf_tmp & (0x007e0000);
	calcu_ta[3] = rf_tmp & (0x7e000000);

	
	calcu_ta[0] = calcu_ta[0] >> 1;
	calcu_ta[1] = calcu_ta[1] >> 9;
	calcu_ta[2] = calcu_ta[2] >> 17;
	calcu_ta[3] = calcu_ta[3] >> 25;
#endif
	halrf_wreg(rf, 0x80e4, 0x00000f00, 0xa);
#if 0
	calcu_ta[4] = halrf_rreg(rf, 0x80fc, 0x0000007e);
	calcu_ta[5] = halrf_rreg(rf, 0x80fc, 0x00007e00);
	calcu_ta[6] = halrf_rreg(rf, 0x80fc, 0x007e0000);
	calcu_ta[7] = halrf_rreg(rf, 0x80fc, 0x7e000000);
#else
	rf_tmp = halrf_rreg(rf, 0x80fc, 0x7e7e7e7f) & 0x7e7e7e7e;
	calcu_ta[4] = rf_tmp & (0x0000007e);
	calcu_ta[5] = rf_tmp & (0x00007e00);
	calcu_ta[6] = rf_tmp & (0x007e0000);
	calcu_ta[7] = rf_tmp & (0x7e000000);
	
		
	calcu_ta[4] = calcu_ta[4] >> 1;
	calcu_ta[5] = calcu_ta[5] >> 9;
	calcu_ta[6] = calcu_ta[6] >> 17;
	calcu_ta[7] = calcu_ta[7] >> 25;
#endif
	halrf_wreg(rf, 0x80e4, 0x00000f00, 0xb);
#if 0
	calcu_ta[8] = halrf_rreg(rf, 0x80fc, 0x0000007e);
	calcu_ta[9] = halrf_rreg(rf, 0x80fc, 0x00007e00);
	calcu_ta[10] = halrf_rreg(rf, 0x80fc, 0x007e0000);
	calcu_ta[11] = halrf_rreg(rf, 0x80fc, 0x7e000000);
#else
	rf_tmp = halrf_rreg(rf, 0x80fc, 0x7e7e7e7f) & 0x7e7e7e7e;
	calcu_ta[8] = rf_tmp & (0x0000007e);
	calcu_ta[9] = rf_tmp & (0x00007e00);
	calcu_ta[10] = rf_tmp & (0x007e0000);
	calcu_ta[11] = rf_tmp & (0x7e000000);
				
	calcu_ta[8] = calcu_ta[8] >> 1;
	calcu_ta[9] = calcu_ta[9] >> 9;
	calcu_ta[10] = calcu_ta[10] >> 17;
	calcu_ta[11] = calcu_ta[11] >> 25;
#endif
	halrf_wreg(rf, 0x80e4, 0x00000f00, 0xc);
#if 0
	calcu_ta[12] = halrf_rreg(rf, 0x80fc, 0x0000007e);
	calcu_ta[13] = halrf_rreg(rf, 0x80fc, 0x00007e00);
	calcu_ta[14] = halrf_rreg(rf, 0x80fc, 0x007e0000);
	calcu_ta[15] = halrf_rreg(rf, 0x80fc, 0x7e000000);
#else
	rf_tmp = halrf_rreg(rf, 0x80fc, 0x7e7e7e7f) & 0x7e7e7e7e;
	calcu_ta[12] = rf_tmp & (0x0000007e);
	calcu_ta[13] = rf_tmp & (0x00007e00);
	calcu_ta[14] = rf_tmp & (0x007e0000);
	calcu_ta[15] = rf_tmp & (0x7e000000);
	
		
	calcu_ta[12] = calcu_ta[12] >> 1;
	calcu_ta[13] = calcu_ta[13] >> 9;
	calcu_ta[14] = calcu_ta[14] >> 17;
	calcu_ta[15] = calcu_ta[15] >> 25;
#endif
	halrf_wreg(rf, 0x80e4, 0x00000f00, 0xd);
	calcu_ta[16] = halrf_rreg(rf, 0x80fc, 0x0000007e);

#endif


#if 0	
	halrf_wreg(rf, 0x80e4, 0x00000f00, 0x2);
	 = halrf_rreg(rf, 0x80fc, 0x0000007f);
	halrf_wreg(rf, 0x80e4, 0x00000f00, 0x0);
	 = halrf_rreg(rf, 0x80fc, MASKDWORD);
	halrf_wreg(rf, 0x80e4, 0x00000f00, 0x2);
	 = halrf_rreg(rf, 0x80fc, 0x007f0000);
	halrf_wreg(rf, 0x80e4, 0x00000f00, 0x1);
	 = halrf_rreg(rf, 0x80fc, MASKDWORD);
	halrf_wreg(rf, 0x80e4, 0x00000f00, 0xf);
	 = halrf_rreg(rf, 0x80fc, MASKDWORD);

	halrf_wreg(rf, itqt[path], MASKDWORD, 0x1b); //ItQt
	halrf_wreg(rf, 0x80d0, 0x00100000, 0x1); //tst_iqk2set
#endif

	
	//========================
	for (i = 0; i < 17; i++) {
		if (d[i] & BIT(6))
			txgapk_info->power_d[path][i] = (s32)(d[i] | 0xffffff80);
		else
			txgapk_info->power_d[path][i] = (s32)(d[i]);

		/*RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]	power	d[%d][%d]=0x%x\n",
			path, i, txgapk_info->power_d[path][i]);*/
		RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]	power	d[%d][%d]=0x%x\n",
			path, i, d[i]);
	}

#if 0
	for (i = 0; i < 17; i++) {
		if (ta[i] & BIT(7))
			txgapk_info->power_ta[path][i] = (s32)(ta[i] | 0xffffff00);
		else
			txgapk_info->power_ta[path][i] = (s32)(ta[i]);
		
		/*RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]	power	ta[%d][%d]=0x%x\n",
			path, i, txgapk_info->power_ta[path][i]);*/
		RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]	power	ta[%d][%d]=0x%x\n",
			path, i, ta[i]);
	}
#endif

	for (i = 0; i < 17; i++) {
		//6bit
		if (calcu_ta[i] & BIT(5))
			txgapk_info->power_ta[path][i] = (s32)(calcu_ta[i] | 0xffffffc0);
		else
			txgapk_info->power_ta[path][i] = (s32)(calcu_ta[i]);
		
		/*RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]	power	calcu_ta[%d][%d]=0x%x\n",
			path, i, txgapk_info->power_ta[path][i]);*/
		RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]	power	calcu_ta[%d][%d]=0x%x\n",
			path, i, calcu_ta[i]);
	}


}


void _halrf_txgapk_power_table_nctl_5g_8852c
	(struct rf_info *rf, enum phl_phy_idx phy, enum rf_path path, u32 gain_stage)
{
	struct halrf_gapk_info *txgapk_info = &rf->gapk;
	u8 rfe_type = rf->phl_com->dev_cap.rfe_type;

	u32 i;
	u32 d[17] = {0};
	//u32 ta[17] = {0};
	u32 calcu_ta[17] = {0};

#ifndef HALRF_CONFIG_FW_IO_OFLD_SUPPORT
	u32 rf_tmp = 0;
#endif


	
	u32 itqt[2] = {0x81cc, 0x82cc};

	u32 gapk_on_tbl_setting[2] = {0x8170, 0x8270};
	u32 iqk_ctrl_rfc_addr[2] = {0x5670, 0x7670};
	
		
	u32 process_id1[2] = {0x00001119, 0x00001129};
	u32 process_id2[2] = {0x00001619, 0x00001629};
	

	RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]======> %s, gain_stage = 0x%x\n", __func__, gain_stage);

	halrf_wreg(rf, 0x80e0, 0x000001f0, 0x07); //Send flag to ITQT controller


	if (rfe_type <= 50) //ifem
		halrf_wreg(rf, 0x8038, 0x003f0000, 0x24); //GapK 2nd ItQt
	else //efem
		halrf_wreg(rf, 0x8038, 0x003f0000, 0x09); //GapK 2nd ItQt

	//debug
	RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]pwr 0x8038[21:16] = 0x%x\n", halrf_rreg(rf, 0x8038, 0x003f0000));
	

	//halrf_wreg(rf, gapk_on_tbl_setting[path], 0x001fffff, 0x000000); //gapk_on_table0_setting
	halrf_wreg(rf, gapk_on_tbl_setting[path], 0x001fffff, gain_stage); //gapk_on_table0_setting
	
	halrf_wreg(rf, iqk_ctrl_rfc_addr[path], 0x00000002, 0x1); //IQK cotrol RFC

	if (rfe_type <= 50) //ifem
		halrf_wreg(rf, itqt[path], MASKDWORD, 0x12); // ItQt
	else //efem
		halrf_wreg(rf, itqt[path], MASKDWORD, 0x09); // ItQt


	//debug
	RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]pwr 0x%x[5:0] = 0x%x\n", itqt[path], halrf_rreg(rf, itqt[path], 0x0000003f));

	halrf_wreg(rf, 0x802c, 0x0fff0000, 0x009); //rx_tone_idx 

	#if 0
	halrf_before_one_shot_enable_8852c(rf);
	halrf_wreg(rf, 0x8000, MASKDWORD, cal_path_step1[path]); //set cal_path, process id 
	halrf_one_shot_nctl_done_check_8852c(rf, path);
	//halrf_wreg(rf, 0x8010, 0x000000ff, 0x00);
	#else
	txgapk_info->is_txgapk_ok = halrf_do_one_shot_8852c(rf, path, 0x8000, MASKDWORD, process_id1[path]);

	txgapk_info->txgapk_chk_cnt[path][TXGAPK_PWR][0] = rf->nctl_ck_times[0]; 
	txgapk_info->txgapk_chk_cnt[path][TXGAPK_PWR][1] = rf->nctl_ck_times[1];
	#endif

		
	halrf_wreg(rf, itqt[path], 0x0000003f, 0x3f); // ItQt

	#if 0
	halrf_before_one_shot_enable_8852c(rf);
	halrf_wreg(rf, 0x8000, MASKDWORD, cal_path_step2[path]); //set cal_path, process id 
	halrf_one_shot_nctl_done_check_8852c(rf, path);
	//halrf_wreg(rf, 0x8010, 0x000000ff, 0x00);
	#else
	txgapk_info->is_txgapk_ok = halrf_do_one_shot_8852c(rf, path, 0x8000, MASKDWORD, process_id2[path]);
	#endif

	
	halrf_wreg(rf, iqk_ctrl_rfc_addr[path], 0x00000002, 0x0); //IQK cotrol RFC
	halrf_wreg(rf, 0x801c, 0x000e0000, 0x0); //Rx_P_avg
	//	========END : Do PA GapK =====


#ifndef HALRF_CONFIG_FW_IO_OFLD_SUPPORT

	// ===== Read GapK Results, Bcut resolution = 0.0625 dB =====
	halrf_wreg(rf, 0x80d4, MASKDWORD, 0x00130000); //gapk_report

	halrf_wreg(rf, 0x80e4, 0x00000f00, 0x3);
#if 0
	d[0] = halrf_rreg(rf, 0x80fc, 0x0000007f);
	d[1] = halrf_rreg(rf, 0x80fc, 0x00003f80);
	d[2] = halrf_rreg(rf, 0x80fc, 0x001fc000);
	d[3] = halrf_rreg(rf, 0x80fc, 0x0fe00000);
#else
	rf_tmp = halrf_rreg(rf, 0x80fc, 0x0fffffff);
	d[0] = rf_tmp & (0x0000007f);
	d[1] = rf_tmp & (0x00003f80);
	d[2] = rf_tmp & (0x001fc000);
	d[3] = rf_tmp & (0x0fe00000);

	d[1] = d[1] >> 7;
	d[2] = d[2] >> 14;
	d[3] = d[3] >> 21;
#endif

	halrf_wreg(rf, 0x80e4, 0x00000f00, 0x4);
#if 0
	d[4] = halrf_rreg(rf, 0x80fc, 0x0000007f);
	d[5] = halrf_rreg(rf, 0x80fc, 0x00003f80);
	d[6] = halrf_rreg(rf, 0x80fc, 0x001fc000);
	d[7] = halrf_rreg(rf, 0x80fc, 0x0fe00000);
#else
	rf_tmp = halrf_rreg(rf, 0x80fc, 0x0fffffff);
	d[4] = rf_tmp & (0x0000007f);
	d[5] = rf_tmp & (0x00003f80);
	d[6] = rf_tmp & (0x001fc000);
	d[7] = rf_tmp & (0x0fe00000);

	d[5] = d[5] >> 7;
	d[6] = d[6] >> 14;
	d[7] = d[7] >> 21;
#endif

	halrf_wreg(rf, 0x80e4, 0x00000f00, 0x5);
#if 0
	d[8] = halrf_rreg(rf, 0x80fc, 0x0000007f);
	d[9] = halrf_rreg(rf, 0x80fc, 0x00003f80);
	d[10] = halrf_rreg(rf, 0x80fc, 0x001fc000);
	d[11] = halrf_rreg(rf, 0x80fc, 0x0fe00000);
#else
	rf_tmp = halrf_rreg(rf, 0x80fc, 0x0fffffff);
	d[8] = rf_tmp & (0x0000007f);
	d[9] = rf_tmp & (0x00003f80);
	d[10] = rf_tmp & (0x001fc000);
	d[11] = rf_tmp & (0x0fe00000);

	d[9] = d[9] >> 7;
	d[10] = d[10] >> 14;
	d[11] = d[11] >> 21;	
#endif

	halrf_wreg(rf, 0x80e4, 0x00000f00, 0x6);
#if 0
	d[12] = halrf_rreg(rf, 0x80fc, 0x0000007f);
	d[13] = halrf_rreg(rf, 0x80fc, 0x00003f80);
	d[14] = halrf_rreg(rf, 0x80fc, 0x001fc000);
	d[15] = halrf_rreg(rf, 0x80fc, 0x0fe00000);
#else
	rf_tmp = halrf_rreg(rf, 0x80fc, 0x0fffffff);
	d[12] = rf_tmp & (0x0000007f);
	d[13] = rf_tmp & (0x00003f80);
	d[14] = rf_tmp & (0x001fc000);
	d[15] = rf_tmp & (0x0fe00000);

	d[13] = d[13] >> 7;
	d[14] = d[14] >> 14;
	d[15] = d[15] >> 21;
#endif

	halrf_wreg(rf, 0x80e4, 0x00000f00, 0x7);
	d[16] = halrf_rreg(rf, 0x80fc, 0x0000007f);

#if 0
	halrf_wreg(rf, 0x80e4, 0x00000f00, 0x9);
	ta[0] = halrf_rreg(rf, 0x80fc, 0x000000ff);
	ta[1] = halrf_rreg(rf, 0x80fc, 0x0000ff00);
	ta[2] = halrf_rreg(rf, 0x80fc, 0x00ff0000);
	ta[3] = halrf_rreg(rf, 0x80fc, 0xff000000);

	halrf_wreg(rf, 0x80e4, 0x00000f00, 0xa);
	ta[4] = halrf_rreg(rf, 0x80fc, 0x000000ff);
	ta[5] = halrf_rreg(rf, 0x80fc, 0x0000ff00);
	ta[6] = halrf_rreg(rf, 0x80fc, 0x00ff0000);
	ta[7] = halrf_rreg(rf, 0x80fc, 0xff000000);

	halrf_wreg(rf, 0x80e4, 0x00000f00, 0xb);
	ta[8] = halrf_rreg(rf, 0x80fc, 0x000000ff);
	ta[9] = halrf_rreg(rf, 0x80fc, 0x0000ff00);
	ta[10] = halrf_rreg(rf, 0x80fc, 0x00ff0000);
	ta[11] = halrf_rreg(rf, 0x80fc, 0xff000000);

	halrf_wreg(rf, 0x80e4, 0x00000f00, 0xc);
	ta[12] = halrf_rreg(rf, 0x80fc, 0x000000ff);
	ta[13] = halrf_rreg(rf, 0x80fc, 0x0000ff00);
	ta[14] = halrf_rreg(rf, 0x80fc, 0x00ff0000);
	ta[15] = halrf_rreg(rf, 0x80fc, 0xff000000);

	halrf_wreg(rf, 0x80e4, 0x00000f00, 0xd);
	ta[16] = halrf_rreg(rf, 0x80fc, 0x000000ff);
#endif

	// Get The Value --> Ta 6bit
	halrf_wreg(rf, 0x80e4, 0x00000f00, 0x9);
#if 0
	calcu_ta[0] = halrf_rreg(rf, 0x80fc, 0x0000007e);
	calcu_ta[1] = halrf_rreg(rf, 0x80fc, 0x00007e00);
	calcu_ta[2] = halrf_rreg(rf, 0x80fc, 0x007e0000);
	calcu_ta[3] = halrf_rreg(rf, 0x80fc, 0x7e000000);
#else
	rf_tmp = halrf_rreg(rf, 0x80fc, 0x7e7e7e7f) & 0x7e7e7e7e;
	calcu_ta[0] = rf_tmp & (0x0000007e);
	calcu_ta[1] = rf_tmp & (0x00007e00);
	calcu_ta[2] = rf_tmp & (0x007e0000);
	calcu_ta[3] = rf_tmp & (0x7e000000);

	
	calcu_ta[0] = calcu_ta[0] >> 1;
	calcu_ta[1] = calcu_ta[1] >> 9;
	calcu_ta[2] = calcu_ta[2] >> 17;
	calcu_ta[3] = calcu_ta[3] >> 25;
#endif
	halrf_wreg(rf, 0x80e4, 0x00000f00, 0xa);
#if 0
	calcu_ta[4] = halrf_rreg(rf, 0x80fc, 0x0000007e);
	calcu_ta[5] = halrf_rreg(rf, 0x80fc, 0x00007e00);
	calcu_ta[6] = halrf_rreg(rf, 0x80fc, 0x007e0000);
	calcu_ta[7] = halrf_rreg(rf, 0x80fc, 0x7e000000);
#else
	rf_tmp = halrf_rreg(rf, 0x80fc, 0x7e7e7e7f) & 0x7e7e7e7e;
	calcu_ta[4] = rf_tmp & (0x0000007e);
	calcu_ta[5] = rf_tmp & (0x00007e00);
	calcu_ta[6] = rf_tmp & (0x007e0000);
	calcu_ta[7] = rf_tmp & (0x7e000000);
	
		
	calcu_ta[4] = calcu_ta[4] >> 1;
	calcu_ta[5] = calcu_ta[5] >> 9;
	calcu_ta[6] = calcu_ta[6] >> 17;
	calcu_ta[7] = calcu_ta[7] >> 25;
#endif

		
	halrf_wreg(rf, 0x80e4, 0x00000f00, 0xb);
#if 0
	calcu_ta[8] = halrf_rreg(rf, 0x80fc, 0x0000007e);
	calcu_ta[9] = halrf_rreg(rf, 0x80fc, 0x00007e00);
	calcu_ta[10] = halrf_rreg(rf, 0x80fc, 0x007e0000);
	calcu_ta[11] = halrf_rreg(rf, 0x80fc, 0x7e000000);
#else
	rf_tmp = halrf_rreg(rf, 0x80fc, 0x7e7e7e7f) & 0x7e7e7e7e;
	calcu_ta[8] = rf_tmp & (0x0000007e);
	calcu_ta[9] = rf_tmp & (0x00007e00);
	calcu_ta[10] = rf_tmp & (0x007e0000);
	calcu_ta[11] = rf_tmp & (0x7e000000);
			
	calcu_ta[8] = calcu_ta[8] >> 1;
	calcu_ta[9] = calcu_ta[9] >> 9;
	calcu_ta[10] = calcu_ta[10] >> 17;
	calcu_ta[11] = calcu_ta[11] >> 25;
#endif
	
	halrf_wreg(rf, 0x80e4, 0x00000f00, 0xc);
#if 0
	calcu_ta[12] = halrf_rreg(rf, 0x80fc, 0x0000007e);
	calcu_ta[13] = halrf_rreg(rf, 0x80fc, 0x00007e00);
	calcu_ta[14] = halrf_rreg(rf, 0x80fc, 0x007e0000);
	calcu_ta[15] = halrf_rreg(rf, 0x80fc, 0x7e000000);
#else
	rf_tmp = halrf_rreg(rf, 0x80fc, 0x7e7e7e7f) & 0x7e7e7e7e;
	calcu_ta[12] = rf_tmp & (0x0000007e);
	calcu_ta[13] = rf_tmp & (0x00007e00);
	calcu_ta[14] = rf_tmp & (0x007e0000);
	calcu_ta[15] = rf_tmp & (0x7e000000);
	
		
	calcu_ta[12] = calcu_ta[12] >> 1;
	calcu_ta[13] = calcu_ta[13] >> 9;
	calcu_ta[14] = calcu_ta[14] >> 17;
	calcu_ta[15] = calcu_ta[15] >> 25;
#endif
	halrf_wreg(rf, 0x80e4, 0x00000f00, 0xd);
	calcu_ta[16] = halrf_rreg(rf, 0x80fc, 0x0000007e);

#endif


#if 0	 
	halrf_wreg(rf, 0x80e4, 0x00000f00, 0x2);
	 = halrf_rreg(rf, 0x80fc, 0x0000007f);
	halrf_wreg(rf, 0x80e4, 0x00000f00, 0x0);
	 = halrf_rreg(rf, 0x80fc, MASKDWORD);
	halrf_wreg(rf, 0x80e4, 0x00000f00, 0x2);
	 = halrf_rreg(rf, 0x80fc, 0x007f0000);
	halrf_wreg(rf, 0x80e4, 0x00000f00, 0x1);
	 = halrf_rreg(rf, 0x80fc, MASKDWORD);
	halrf_wreg(rf, 0x80e4, 0x00000f00, 0xf);
	 = halrf_rreg(rf, 0x80fc, MASKDWORD);


	halrf_wreg(rf, itqt[path], MASKDWORD, 0x1b);
	halrf_wreg(rf, 0x80d0, 0x00100000, 0x1);
#endif	
	//==========
	for (i = 0; i < 17; i++) {
		if (d[i] & BIT(6))
			txgapk_info->power_d[path][i] = (s32)(d[i] | 0xffffff80);
		else
			txgapk_info->power_d[path][i] = (s32)(d[i]);

		/*RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]	power	d[%d][%d]=0x%x\n",
			path, i, txgapk_info->power_d[path][i]);*/
		RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]	power	d[%d][%d]=0x%x\n",
			path, i, d[i]);
	}

#if 0
	for (i = 0; i < 17; i++) {
		if (ta[i] & BIT(7))
			txgapk_info->power_ta[path][i] = (s32)(ta[i] | 0xffffff00);
		else
			txgapk_info->power_ta[path][i] = (s32)(ta[i]);
		
		/*RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]	power	ta[%d][%d]=0x%x\n",
			path, i, txgapk_info->power_ta[path][i]);*/
		RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]	power	ta[%d][%d]=0x%x\n",
			path, i, ta[i]);
	}
#endif

	for (i = 0; i < 17; i++) {
		//6bit
		if (calcu_ta[i] & BIT(5))
			txgapk_info->power_ta[path][i] = (s32)(calcu_ta[i] | 0xffffffc0);
		else
			txgapk_info->power_ta[path][i] = (s32)(calcu_ta[i]);
		
		/*RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]	power	calcu_ta[%d][%d]=0x%x\n",
			path, i, txgapk_info->power_ta[path][i]);*/
		RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]	power	calcu_ta[%d][%d]=0x%x\n",
			path, i, calcu_ta[i]);
	}

}


void _halrf_txgapk_power_table_nctl_8852c
	(struct rf_info *rf, enum phl_phy_idx phy, enum rf_path path)
{
	//u8 channel = rf->hal_com->band[phy].cur_chandef.center_ch;
	u32 band = rf->hal_com->band[phy].cur_chandef.band;
	u32 gain_stage_2g = 0x540 ;
	u32 gain_stage_5g = 0x0;
	u32 gain_stage_6g = 0x0;

	//if (channel >= 1 && channel <= 14)
	if (band == BAND_ON_24G)
		_halrf_txgapk_power_table_nctl_2g_8852c(rf, phy, path, gain_stage_2g);
	else if (band == BAND_ON_5G)
		_halrf_txgapk_power_table_nctl_5g_8852c(rf, phy, path, gain_stage_5g);
	else
		_halrf_txgapk_power_table_nctl_5g_8852c(rf, phy, path, gain_stage_6g);	

}



void _halrf_txgapk_iqk_bk_reg_by_mode_8852c
	(struct rf_info *rf, enum phl_phy_idx phy, enum rf_path path, bool is_dbcc)
{
	struct halrf_gapk_info *txgapk_info = &rf->gapk;
	u32 process_id[2] = {0x1219, 0x1229};
	
	RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]======> %s\n", __func__);
	
	if (!is_dbcc) {
		/* no dbcc */

#if 0
		halrf_before_one_shot_enable_8852c(rf);
		halrf_wreg(rf, 0x8000, MASKDWORD, path_setting[path]);

		halrf_one_shot_nctl_done_check_8852c(rf, path);
#else
		txgapk_info->is_txgapk_ok = halrf_do_one_shot_8852c(rf, path, 0x8000, MASKDWORD, process_id[path]);
#endif
		txgapk_info->txgapk_chk_cnt[path][TXGAPK_IQKBK][0] = rf->nctl_ck_times[0]; 
		txgapk_info->txgapk_chk_cnt[path][TXGAPK_IQKBK][1] = rf->nctl_ck_times[1];
		
		halrf_wreg(rf, 0x8008, MASKDWORD, 0x00000000);
		halrf_wreg(rf, 0x8088, MASKDWORD, 0x80000000);
		
	} else {
		/* dbcc */
		if (phy == HW_PHY_0) {
			//TBD
		} else if (phy == HW_PHY_1) {
			//TBD
		}
	}
		
	halrf_wrf(rf, path, 0xef, 0x00004, 0x0);
	halrf_wrf(rf, path, 0x0, 0xf0000, 0x3);	
	halrf_wrf(rf, path, 0x5, 0x00001, 0x1);
	
	
}



void _halrf_txgapk_restore_iqk_kmod_bb_8852c
	(struct rf_info *rf, enum phl_phy_idx phy, enum rf_path path, bool is_dbcc)
{
	RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]======> %s\n", __func__);

	if (!is_dbcc) {
		/* no dbcc */
		if (path == RF_PATH_A) {
			halrf_wreg(rf, 0x12b8, 0x40000000, 0x0);
			halrf_wreg(rf, 0x20fc, 0x00010000, 0x1);
			halrf_wreg(rf, 0x20fc, 0x00100000, 0x0);
			halrf_wreg(rf, 0x20fc, 0x01000000, 0x1);
			halrf_wreg(rf, 0x20fc, 0x10000000, 0x0);
			halrf_wreg(rf, 0x5670, MASKDWORD, 0x00000000);
			halrf_wreg(rf, 0x12a0, 0x000ff000, 0x00);
			halrf_wreg(rf, 0x20fc, 0x00010000, 0x0);
			halrf_wreg(rf, 0x20fc, 0x01000000, 0x0);
		} else if (path == RF_PATH_B) {
			halrf_wreg(rf, 0x32b8, 0x40000000, 0x0);
			halrf_wreg(rf, 0x20fc, 0x00020000, 0x1);
			halrf_wreg(rf, 0x20fc, 0x00200000, 0x0);
			halrf_wreg(rf, 0x20fc, 0x02000000, 0x1);
			halrf_wreg(rf, 0x20fc, 0x20000000, 0x0);
			halrf_wreg(rf, 0x7670, MASKDWORD, 0x00000000);
			halrf_wreg(rf, 0x32a0, 0x000ff000, 0x00);
			halrf_wreg(rf, 0x20fc, 0x00020000, 0x0);
			halrf_wreg(rf, 0x20fc, 0x02000000, 0x0);
		}
			
	} else {
		/* dbcc */
		if (phy == HW_PHY_0) {
			//TBD
		} else if (phy == HW_PHY_1) {
			//TBD
		}
	}
	
}


void _halrf_txgapk_restore_iqk_kmod_drfc_8852c
	(struct rf_info *rf, enum phl_phy_idx phy, enum rf_path path, bool is_dbcc)
{
	RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]======> %s\n", __func__);
	
	halrf_wrf(rf, path, 0x10005, 0x00001, 0x1);
}


void _halrf_txgapk_main_8852c(struct rf_info *rf,
					enum phl_phy_idx phy, enum rf_path path, bool is_dbcc)
{
	/*_halrf_txgapk_iqk_dpk_init_reg_8852c(rf, phy, RF_PATH_A);*/
	/*_halrf_txgapk_nctl_8852c(rf, phy, RF_PATH_A);*/

	#if 0 //it doesn't need to write default
	_halrf_txgapk_write_track_table_default_8852c(rf, phy, path);
	_halrf_txgapk_write_power_table_default_8852c(rf, phy, path);
	#endif

	_halrf_txgapk_bb_afe_by_mode_8852c(rf, phy, path, false);
	
	_halrf_txgapk_iqk_preset_by_mode_8852c(rf, phy, path, false);

	_halrf_txgapk_rf_dpk_8852c(rf, phy, path);
	
	_halrf_txgapk_rf_rxdck_8852c(rf, phy, path);

	_halrf_txgapk_track_table_nctl_8852c(rf, phy, path);
	//it doesn't need to write table
	

	halrf_write_fwofld_end(rf); 	/*FW Offload End*/
	halrf_write_fwofld_start(rf);

	_halrf_txgapk_power_table_nctl_8852c(rf, phy, path);
	//it doesn't need to write table

	_halrf_txgapk_iqk_bk_reg_by_mode_8852c(rf, phy, path, false);
	
	_halrf_txgapk_restore_iqk_kmod_bb_8852c(rf, phy, path, false);
	
	_halrf_txgapk_restore_iqk_kmod_drfc_8852c(rf, phy, path, false);
	
}

void _halrf_do_reset_tbl_txgapk_8852c(struct rf_info *rf,
					enum phl_phy_idx phy)
{
//	struct halrf_gapk_info *txgapk_info = &rf->gapk;
	u8 path;
	u32 band = rf->hal_com->band[phy].cur_chandef.band;
	
	u32 kip_bkup[TXGAPK_RF_PATH_MAX_8852C][TXGAPK_KIP_REG_NUM_8852C] = {{0}};
	//u32 bb_bkup[TXGAPK_BB_REG_NUM_8852C] = {0};
	u32 rf_bkup[TXGAPK_RF_PATH_MAX_8852C][TXGAPK_RF_REG_NUM_8852C] = {{0}};


	u32 kip_reg[] = {0x813c, 0x8124, 0x8120, 0xc0d4, 0xc0d8};
	//u32 bb_reg[] = {0x2344, 0xc0d4, 0xc0d8, 0xc1d4, 0xc1d8};
	u32 rf_reg[] = {0x5, 0x10005, 0xdf};


	RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]======> %s   phy=%d, table_idx = %d, band = %d, ch = %d\n", __func__, phy, rf->gapk.txgapk_table_idx, band, rf->hal_com->band[phy].cur_chandef.center_ch);

	//_txgapk_backup_bb_registers_8852c(rf, bb_reg, bb_bkup, TXGAPK_BB_REG_NUM_8852C);

	for (path = 0; path < TXGAPK_RF_PATH_MAX_8852C; path++) {	
		_txgapk_backup_kip_8852c(rf, kip_reg, kip_bkup, path, TXGAPK_KIP_REG_NUM_8852C);
		_txgapk_bkup_rf_8852c(rf, rf_reg, rf_bkup, path, TXGAPK_RF_REG_NUM_8852C);
	}				

	for (path = 0; path < TXGAPK_RF_PATH_MAX_8852C; path++) {
		RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]======> reset table index = %d\n", halrf_rrf(rf, path, 0x18, 0x80000));

		//follow dpk flow
		_halrf_txgapk_rxagc_onoff_8852c(rf, path, false);
	
		//_halrf_txgapk_main(rf, phy, path, false);
		//====== main =======
		_halrf_txgapk_bb_afe_by_mode_8852c(rf, phy, path, false);	
		_halrf_txgapk_iqk_preset_by_mode_8852c(rf, phy, path, false);
		_halrf_txgapk_rf_dpk_8852c(rf, phy, path);
		_halrf_txgapk_rf_rxdck_8852c(rf, phy, path);	

		//_halrf_txgapk_track_table_nctl_8852c(rf, phy, path);
		//_halrf_txgapk_power_table_nctl_8852c(rf, phy, path);

		if (band == BAND_ON_24G) {
			_halrf_txgapk_track_table_nctl_2g_8852c(rf, phy, path, 0x0);
			_halrf_txgapk_power_table_nctl_2g_8852c(rf, phy, path, 0x0);
		} else {
			_halrf_txgapk_track_table_nctl_5g_8852c(rf, phy, path, 0x0);
			_halrf_txgapk_power_table_nctl_5g_8852c(rf, phy, path, 0x0);
		}
						

		_halrf_txgapk_iqk_bk_reg_by_mode_8852c(rf, phy, path, false);		
		_halrf_txgapk_restore_iqk_kmod_bb_8852c(rf, phy, path, false);
		
		_halrf_txgapk_restore_iqk_kmod_drfc_8852c(rf, phy, path, false);
		//====== end =======
	}

	
	for (path = 0; path < TXGAPK_RF_PATH_MAX_8852C; path++) {
		_txgapk_reload_kip_8852c(rf, kip_reg, kip_bkup, path, TXGAPK_KIP_REG_NUM_8852C);
		_txgapk_reload_rf_8852c(rf, rf_reg, rf_bkup, path, TXGAPK_RF_REG_NUM_8852C);

		//follow dpk flow
		_halrf_txgapk_rxagc_onoff_8852c(rf, path, true);
	}

	//_txgapk_reload_bb_registers_8852c(rf, bb_reg, bb_bkup, TXGAPK_BB_REG_NUM_8852C);
}



					
void _halrf_do_non_dbcc_txgapk_8852c(struct rf_info *rf,
					enum phl_phy_idx phy)
{
	struct halrf_gapk_info *txgapk_info = &rf->gapk;
	u8 path;
	
	u32 kip_bkup[TXGAPK_RF_PATH_MAX_8852C][TXGAPK_KIP_REG_NUM_8852C] = {{0}};
	//u32 bb_bkup[TXGAPK_BB_REG_NUM_8852C] = {0};
	u32 rf_bkup[TXGAPK_RF_PATH_MAX_8852C][TXGAPK_RF_REG_NUM_8852C] = {{0}};


	u32 kip_reg[] = {0x813c, 0x8124, 0x8120, 0xc0d4, 0xc0d8};
	//u32 bb_reg[] = {0x2344, 0xc0d4, 0xc0d8, 0xc1d4, 0xc1d8};
	u32 rf_reg[] = {0x0, 0x5, 0x10005, 0xdf};


	RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]======> %s   phy=%d, table_idx = %d\n", __func__, phy,  txgapk_info->txgapk_table_idx);

	//_txgapk_backup_bb_registers_8852c(rf, bb_reg, bb_bkup, TXGAPK_BB_REG_NUM_8852C);

	for (path = 0; path < TXGAPK_RF_PATH_MAX_8852C; path++) {	
		_txgapk_backup_kip_8852c(rf, kip_reg, kip_bkup, path, TXGAPK_KIP_REG_NUM_8852C);
		_txgapk_bkup_rf_8852c(rf, rf_reg, rf_bkup, path, TXGAPK_RF_REG_NUM_8852C);
	}				

	halrf_write_fwofld_start(rf);	/*FW Offload Start*/

	for (path = 0; path < TXGAPK_RF_PATH_MAX_8852C; path++) {
		halrf_wrf(rf, path, 0x18, 0x80000, txgapk_info->txgapk_table_idx);
		halrf_wrf(rf, path, 0x10018, 0x80000, txgapk_info->txgapk_table_idx);

		//follow dpk flow
		_halrf_txgapk_rxagc_onoff_8852c(rf, path, false);
	
		_halrf_txgapk_main_8852c(rf, phy, path, false);
	}

	//TBD Handle K fail
	
	for (path = 0; path < TXGAPK_RF_PATH_MAX_8852C; path++) {
		_txgapk_reload_kip_8852c(rf, kip_reg, kip_bkup, path, TXGAPK_KIP_REG_NUM_8852C);
		_txgapk_reload_rf_8852c(rf, rf_reg, rf_bkup, path, TXGAPK_RF_REG_NUM_8852C);

		//follow dpk flow
		_halrf_txgapk_rxagc_onoff_8852c(rf, path, true);
	}
	halrf_write_fwofld_end(rf); 	/*FW Offload End*/

	//_txgapk_reload_bb_registers_8852c(rf, bb_reg, bb_bkup, TXGAPK_BB_REG_NUM_8852C);
}


void _halrf_do_dbcc_txgapk_8852c(struct rf_info *rf,
					enum phl_phy_idx phy)
{
	enum rf_path path = 0;
	
	if (phy == HW_PHY_0)
		path = RF_PATH_A;
	else if (phy == HW_PHY_1)
		path = RF_PATH_B;

	
	//DBCC use the same table
	/* 0:table_0, 1:table_1 */
	halrf_wrf(rf, path, 0x18, 0x80000, 0);
	halrf_wrf(rf, path, 0x10018, 0x80000, 0);
}
void _halrf_txgapk_get_ch_info_8852c(struct rf_info *rf, enum phl_phy_idx phy)
{
	struct halrf_gapk_info *txgapk_info = &rf->gapk;
	struct halrf_mcc_info *mcc_info = &rf->mcc_info;
	
	u8 idx = 0;
//	u8 get_empty_table = false;

	RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]======> %s \n", __func__);

#if 0
	for  (idx = 0;  idx < 2; idx++) {
		if (txgapk_info->txgapk_mcc_ch[idx] == 0) {
			get_empty_table = true;
			break;
		}
	}
	//RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK] (1)  idx = %x\n", idx);

	if (false == get_empty_table) {
		idx = txgapk_info->txgapk_table_idx + 1;
		if (idx > 1) {
			idx = 0;
		}		
		//RF_DBG(rf, DBG_RF_IQK, "[IQK]we will replace iqk table index(%d), !!!!! \n", idx);
	}	
	//RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK] (2)  idx = %x\n", idx);
#endif

	idx = mcc_info->table_idx;
	txgapk_info->txgapk_table_idx =  idx;

	txgapk_info->txgapk_mcc_ch[idx] = mcc_info->ch[idx];

	if (mcc_info->ch[idx] != rf->hal_com->band[phy].cur_chandef.center_ch)
		RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]======> warning!!!! mcc_info = %d, curr_ch=%d \n",
		mcc_info->ch[idx], rf->hal_com->band[phy].cur_chandef.center_ch);

	txgapk_info->ch[0] = rf->hal_com->band[phy].cur_chandef.center_ch;	
	
}


void  _halrf_sel_hw_table_txgapk_8852c(struct rf_info *rf, bool is_hw) 
{
	RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]======> %s \n", __func__);

	halrf_wrf(rf, RF_PATH_A, 0x10055, 0x80000, is_hw);
	halrf_wrf(rf, RF_PATH_B, 0x10055, 0x80000, is_hw);


}

void halrf_do_txgapk_8852c(struct rf_info *rf,
					enum phl_phy_idx phy)
{
#if 0
	u8 rfe_type = rf->phl_com->dev_cap.rfe_type;
	
	//0x10055[19] SW gapk table selection
	//move to initial
	//_halrf_sel_hw_table_txgapk_8852c(rf, phy, false); //use sw control

	//For AP only, rfe_type >50 do txgapk	
	if (rfe_type < 50) { //NOT efem
		RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]======> %s rfe_type = %d,Not efem or ifem, skip txgapk process\n", __func__, rfe_type);
		return;
	}
#endif

	_halrf_txgapk_get_ch_info_8852c(rf, phy);
	
	
	RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]======> %s, ref_type=%d, phy=%d, dbcc_en = %d, band = %d, ch = %d\n", __func__, rf->phl_com->dev_cap.rfe_type, phy, rf->hal_com->dbcc_en, 
		rf->hal_com->band[phy].cur_chandef.band, rf->hal_com->band[phy].cur_chandef.center_ch);
	RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]======> version = 0x%x\n", TXGAPK_VER_8852C); 
	RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]======> before GapK process, 0x%x= 0x%x\n", 0x8010, halrf_rreg(rf, 0x8010, MASKDWORD));
	
	
	if (rf->hal_com->dbcc_en)
		_halrf_do_dbcc_txgapk_8852c(rf, phy);
	else
		_halrf_do_non_dbcc_txgapk_8852c(rf, phy);
	
		
	RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]======> After GapK process, 0x%x= 0x%x\n", 0x8010, halrf_rreg(rf, 0x8010, MASKDWORD));
}


void halrf_txgapk_init_8852c(struct rf_info *rf) 
{	
	struct halrf_gapk_info *txgapk_info = &rf->gapk;


	if(!txgapk_info->is_gapk_init) {
		RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]======> %s \n", __func__);
		txgapk_info->is_gapk_init = true;

		_halrf_sel_hw_table_txgapk_8852c(rf, false); //use sw control			
	}
}

#if 0
void halrf_txgapk_enable_8852c
	(struct rf_info *rf, enum phl_phy_idx phy)
{
	u8 i;
	RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]======> %s   phy=%d\n", __func__, phy);

	for (i = RF_PATH_A; i < 2; i++) {
		_halrf_txgapk_write_track_table_8852c(rf, phy, i);
		_halrf_txgapk_write_power_table_8852c(rf, phy, i);
	}
}
#endif


void halrf_txgapk_write_table_default_8852c
	(struct rf_info *rf, enum phl_phy_idx phy)
{
	RF_DBG(rf, DBG_RF_TXGAPK, "[TXGAPK]======> %s   phy=%d\n", __func__, phy);
	
	_halrf_do_reset_tbl_txgapk_8852c(rf, phy);
}

#endif

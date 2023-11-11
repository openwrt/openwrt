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

#ifndef __iram_func__
#define __iram_func__
#endif

__iram_func__
static void _iqk_read_fft_8852c(struct rf_info *rf, u8 path)
{
#if 0
	u8 i = 0x0;
	u32 fft[6] = {0x0};

	RF_DBG(rf, DBG_RF_IQK, "[IQK]===>%s\n", __func__);
	halrf_wreg(rf, 0x80d4, MASKDWORD, 0x00160000);
	fft[0] = halrf_rreg(rf, 0x80fc, MASKDWORD);
	halrf_wreg(rf, 0x80d4, MASKDWORD, 0x00170000);
	fft[1] = halrf_rreg(rf, 0x80fc, MASKDWORD);
	halrf_wreg(rf, 0x80d4, MASKDWORD, 0x00180000);
	fft[2] = halrf_rreg(rf, 0x80fc, MASKDWORD);
	halrf_wreg(rf, 0x80d4, MASKDWORD, 0x00190000);
	fft[3] = halrf_rreg(rf, 0x80fc, MASKDWORD);
	halrf_wreg(rf, 0x80d4, MASKDWORD, 0x001a0000);
	fft[4] = halrf_rreg(rf, 0x80fc, MASKDWORD);
	halrf_wreg(rf, 0x80d4, MASKDWORD, 0x001b0000);
	fft[5] = halrf_rreg(rf, 0x80fc, MASKDWORD);
	for(i =0; i< 6; i++)
		RF_DBG(rf, DBG_RF_IQK, "[IQK]S%x,fft[%x]= %x\n", path, i, fft[i]);
#endif
	return;
}

__iram_func__
static void _iqk_read_xym_8852c(struct rf_info *rf, u8 path)
{

	u8 i = 0x0;
//	u32 tmp = 0x0;
	u32 bk8138 = 0x0;

	RF_DBG(rf, DBG_RF_IQK, "[IQK]===>%s\n", __func__);
	bk8138 = halrf_rreg(rf, 0x8138 + (path << 8), MASKDWORD);
	halrf_wreg(rf, 0x8000, 0x00000006, path);
	halrf_wreg(rf, 0x801c, 0x000000ff, 0x1);

	for (i = 0x0; i < 0x18; i++) {
		halrf_wreg(rf, 0x8014, MASKDWORD, 0x000000c0 + i);
		halrf_wreg(rf, 0x8014, MASKDWORD, 0x00000000);
	//	tmp = halrf_rreg(rf, 0x8138 + (path << 8), MASKDWORD);
		RF_DBG(rf, DBG_RF_IQK, "[IQK]S%x, 0x8%x38 = %x\n", path, 1 << path, halrf_rreg(rf, 0x8138 + (path << 8), MASKDWORD));
		halrf_delay_us(rf, 1);
	}

	halrf_wreg(rf, 0x801c, MASKDWORD, 0xa210bc00);
	halrf_wreg(rf, 0x8014, MASKDWORD, 0x80010100);
	halrf_wreg(rf, 0x8138 + (path << 8), MASKDWORD, bk8138);
	halrf_delay_us(rf, 1);

	return;
}


__iram_func__
static void _iqk_read_txcfir_8852c(struct rf_info *rf, u8 path, u8 group)
{

	struct halrf_iqk_info *iqk_info = &rf->iqk;
	u8 idx = 0x0;
//	u32 tmp = 0x0;
	u8 dbcc_idx = 0x0;
	u32 dbcc_diff = 0x0;

	//RF_DBG(rf, DBG_RF_IQK, "[IQK]===>%s\n", __func__);
	RF_DBG(rf, DBG_RF_IQK, "[IQK] -----S%x, DBCC%x, TxCFIR%x-----\n", path, iqk_info->iqk_table_idx[path], group);

	dbcc_idx = iqk_info->iqk_table_idx[path];
	if(dbcc_idx == 0)
		dbcc_diff = 0;
	else
		dbcc_diff = 0x110;
	
	halrf_wreg(rf, 0x81d8 + (path << 8), MASKDWORD, 0x00000001);

	switch (group) {
	case 0:
		for (idx = 0; idx < 0x0d; idx++) {
	//		tmp = halrf_rreg(rf, 0x9500 + (path << 11) + (idx << 2) + dbcc_diff, MASKDWORD);
			RF_DBG(rf, DBG_RF_IQK, "[IQK] %x = %x\n", 0x9500 + (path << 11) + (idx << 2) + dbcc_diff, halrf_rreg(rf, 0x9500 + (path << 11) + (idx << 2) + dbcc_diff, MASKDWORD));
		}
		break;
	case 1:
		for (idx = 0; idx < 0x0d; idx++) {
	//		tmp = halrf_rreg(rf, 0x9544 + (path << 11) + (idx << 2) + dbcc_diff, MASKDWORD);
			RF_DBG(rf, DBG_RF_IQK, "[IQK] %x = %x\n", 0x9544 + (path << 11) + (idx << 2) + dbcc_diff, halrf_rreg(rf, 0x9544 + (path << 11) + (idx << 2) + dbcc_diff, MASKDWORD));
		}
		break;
	case 2:
		for (idx = 0; idx < 0x0d; idx++) {
	//		tmp = halrf_rreg(rf, 0x9588 + (path << 11) + (idx << 2) + dbcc_diff, MASKDWORD);
			RF_DBG(rf, DBG_RF_IQK, "[IQK] %x = %x\n", 0x9588 + (path << 11) + (idx << 2) + dbcc_diff, halrf_rreg(rf, 0x9588 + (path << 11) + (idx << 2) + dbcc_diff, MASKDWORD));
		}
		break;
	case 3:
		for (idx = 0; idx < 0x0d; idx++) {
	//		tmp = halrf_rreg(rf, 0x95cc + (path << 11) + (idx << 2) + dbcc_diff, MASKDWORD);
			RF_DBG(rf, DBG_RF_IQK, "[IQK] %x = %x\n", 0x95cc + (path << 11) + (idx << 2) + dbcc_diff, halrf_rreg(rf, 0x95cc + (path << 11) + (idx << 2) + dbcc_diff, MASKDWORD));
		}
		break;
	default:
		break;
	}
	halrf_wreg(rf, 0x81d8 + (path << 8), MASKDWORD, 0x00000000);
	return;
}

__iram_func__
static void _iqk_read_rxcfir_8852c(struct rf_info *rf, u8 path, u8 group)
{

	struct halrf_iqk_info *iqk_info = &rf->iqk;
	u8 idx = 0x0;
//	u32 tmp = 0x0;
	u8 dbcc_idx = 0x0;
	u32 dbcc_diff = 0x0;

	//RF_DBG(rf, DBG_RF_IQK, "[IQK]===>%s\n", __func__);
	RF_DBG(rf, DBG_RF_IQK, "[IQK] -----S%x, DBCC%x, RxCFIR%x-----\n", path, iqk_info->iqk_table_idx[path], group);

	dbcc_idx = iqk_info->iqk_table_idx[path];
	if(dbcc_idx == 0)
		dbcc_diff = 0;
	else
		dbcc_diff = 0x220;
	
	halrf_wreg(rf, 0x81d8 + (path << 8), MASKDWORD, 0x00000001);

	switch (group) {
	case 0:
		for (idx = 0; idx < 0x0d; idx++) {
		//	tmp = halrf_rreg(rf, 0x9720 + (path << 11) + (idx << 2) + dbcc_diff, MASKDWORD);
			RF_DBG(rf, DBG_RF_IQK, "[IQK] %x = %x\n", 0x9720 + (path << 11) + (idx << 2) + dbcc_diff, halrf_rreg(rf, 0x9720 + (path << 11) + (idx << 2) + dbcc_diff, MASKDWORD));
		}
		break;
	case 1:
		for (idx = 0; idx < 0x0d; idx++) {
		//	tmp = halrf_rreg(rf, 0x9764 + (path << 11) + (idx << 2) + dbcc_diff, MASKDWORD);
			RF_DBG(rf, DBG_RF_IQK, "[IQK] %x = %x\n", 0x9764 + (path << 11) + (idx << 2) + dbcc_diff, halrf_rreg(rf, 0x9764 + (path << 11) + (idx << 2) + dbcc_diff, MASKDWORD));
		}
		break;
	case 2:
		for (idx = 0; idx < 0x0d; idx++) {
		//	tmp = halrf_rreg(rf, 0x97a8 + (path << 11) + (idx << 2) + dbcc_diff, MASKDWORD);
			RF_DBG(rf, DBG_RF_IQK, "[IQK] %x = %x\n", 0x97a8 + (path << 11) + (idx << 2) + dbcc_diff, halrf_rreg(rf, 0x97a8 + (path << 11) + (idx << 2) + dbcc_diff, MASKDWORD));
		}
		break;
	case 3:
		for (idx = 0; idx < 0x0d; idx++) {
		//	tmp = halrf_rreg(rf, 0x97ec + (path << 11) + (idx << 2) + dbcc_diff, MASKDWORD);
			RF_DBG(rf, DBG_RF_IQK, "[IQK] %x = %x\n", 0x97ec + (path << 11) + (idx << 2) + dbcc_diff,  halrf_rreg(rf, 0x97ec + (path << 11) + (idx << 2) + dbcc_diff, MASKDWORD));
		}
		break;
	default:
		break;
	}
	halrf_wreg(rf, 0x81d8 + (path << 8), MASKDWORD, 0x00000000);
	return;
}

__iram_func__
static bool _iqk_check_nbiqc_8852c(struct rf_info *rf, enum phl_phy_idx phy_idx,  u8 ktype, u8 path)
{
	u32 x = 0;
	u32 y = 0;

	RF_DBG(rf, DBG_RF_IQK, "[IQK]===>%s\n", __func__);

	if(ktype == ID_NBTXK) {
		x = halrf_rreg(rf, 0x8138 + (path << 8), 0xfff00000); 
		y = halrf_rreg(rf, 0x8138 + (path << 8), 0x000fff00);
	} else {
		x = halrf_rreg(rf, 0x813c + (path << 8), 0xfff00000); 
		y = halrf_rreg(rf, 0x813c + (path << 8), 0x000fff00);
	}
	
	
	x = HALRF_ABS(x, 1024);
	if (y > 0x800) 
		y = (0x1000 - y);

	RF_DBG(rf, DBG_RF_IQK, "[IQK] x = %d, y = %d\n", x, y);

	if ( (x > 150) ||(y > 150))
		return true;
	else
		return false;
}

__iram_func__
static void _iqk_sram_8852c(struct rf_info *rf, u8 path)
{

//	u32 tmp = 0x0;
	u32 i = 0x0;

	RF_DBG(rf, DBG_RF_IQK, "[IQK]===>%s\n", __func__);
	halrf_wreg(rf, 0x80d4, MASKDWORD, 0x00020000);
	halrf_wreg(rf, 0x8074, MASKDWORD, 0x80000000);
	halrf_wreg(rf, 0x80e8, MASKDWORD, 0x00000080);
	halrf_wreg(rf, 0x80d8, MASKDWORD, 0x00010000);
	halrf_wreg(rf, 0x80d8, MASKDWORD, 0x00010000);


	for (i = 0; i <= 0x27f; i++) {
		halrf_wreg(rf, 0x80d8, MASKDWORD, 0x00010000 + i);
	//	tmp = halrf_rreg(rf, 0x80fc, 0x0fff0000);
		RF_DBG(rf, DBG_RF_IQK, "[IQK]0x%x\n", halrf_rreg(rf, 0x80fc, 0x0fff0000));
	}

	for (i = 0; i <= 0x27f; i++) {
		halrf_wreg(rf, 0x80d8, MASKDWORD, 0x00010000 + i);
	//	tmp = halrf_rreg(rf, 0x80fc, 0x00000fff);
		RF_DBG(rf, DBG_RF_IQK, "[IQK]0x%x\n", halrf_rreg(rf, 0x80fc, 0x00000fff));
	}
	halrf_wreg(rf, 0x80e8, MASKDWORD, 0x00000000);
	halrf_wreg(rf, 0x80d8, MASKDWORD, 0x00000000);


	return;
}

__iram_func__
static void _iqk_rxk_setting_8852c(struct rf_info *rf, u8 path)
{
	struct halrf_iqk_info *iqk_info = &rf->iqk;
	//u32 tmp = 0x0;
	
	RF_DBG(rf, DBG_RF_IQK, "[IQK]===>%s\n", __func__);
	if(path == RF_PATH_A)
		halrf_wreg(rf, 0x20fc, 0xffff0000, 0x0101);
	else
		halrf_wreg(rf, 0x20fc, 0xffff0000, 0x0202);
	switch (iqk_info->iqk_bw[path]) {
	case CHANNEL_WIDTH_20:
	case CHANNEL_WIDTH_40:		
		RF_DBG(rf, DBG_RF_IQK, "[IQK]05_BW2040_DAC_off_ADC080\n");
		halrf_wreg(rf, 0x5670 + (path << 13), 0x00002000, 0x1);
		halrf_rxck_force_8852c(rf, path, true, ADC_480M);
		halrf_wreg(rf, 0x5670 + (path << 13), 0x60000000, 0x0);
//		halrf_wreg(rf, 0xc0d4 + (path << 8), 0x0c000000, 0x3);
//		halrf_wreg(rf, 0xc0d8 + (path << 8), 0x000001e0, 0xf);
		halrf_wrf(rf, path, 0x8f, 0x01000, 0x1);
		halrf_wreg(rf, 0x12b8 + (path << 13), 0x40000000, 0x1);
		break;
	case CHANNEL_WIDTH_80:
		RF_DBG(rf, DBG_RF_IQK, "[IQK]05_BW80_DAC_off_ADC160\n");
		halrf_wreg(rf, 0x5670 + (path << 13), 0x00002000, 0x1);
		halrf_rxck_force_8852c(rf, path, true, ADC_960M);
		halrf_wreg(rf, 0x5670 + (path << 13), 0x60000000, 0x1);
//		halrf_wreg(rf, 0xc0d4 + (path << 8), 0x0c000000, 0x2);
//		halrf_wreg(rf, 0xc0d8 + (path << 8), 0x000001e0, 0xd);
		halrf_wrf(rf, path, 0x8f, 0x01000, 0x1);
		halrf_wreg(rf, 0x12b8 + (path << 13), 0x40000000, 0x1);
	break;
	case CHANNEL_WIDTH_160:
		RF_DBG(rf, DBG_RF_IQK, "[IQK]05_BW160_DAC_off_ADC320\n");
		halrf_wreg(rf, 0x5670 + (path << 13), 0x00002000, 0x1);
		halrf_rxck_force_8852c(rf, path, true, ADC_1920M);
		halrf_wreg(rf, 0x5670 + (path << 13), 0x60000000, 0x2);
//		halrf_wreg(rf, 0xc0d4 + (path << 8), 0x0c000000, 0x1);
//		halrf_wreg(rf, 0xc0d8 + (path << 8), 0x000001e0, 0xb);
		halrf_wrf(rf, path, 0x8f, 0x01000, 0x1);
		halrf_wreg(rf, 0x12b8 + (path << 13), 0x40000000, 0x1);
		break;
	default:
		break;
	}
	halrf_wreg(rf, 0x030c, 0xff000000, 0x0f);
	halrf_wreg(rf, 0x030c, 0xff000000, 0x03);
	halrf_wreg(rf, 0x032c, 0xffff0000, 0x0001);
	halrf_wreg(rf, 0x032c, 0xffff0000, 0x0041);
	if(path == RF_PATH_A)
		halrf_wreg(rf, 0x20fc, 0xffff0000, 0x1101);
	else
		halrf_wreg(rf, 0x20fc, 0xffff0000, 0x2202);
	return;
}

__iram_func__
static bool _iqk_check_cal_8852c(struct rf_info *rf, u8 path, u8 ktype)
{

	//struct halrf_iqk_info *iqk_info = &rf->iqk;
	bool notready = true, fail = true;
	u32 delay_count = 0x0, tmp;

	//RF_DBG(rf, DBG_RF_IQK, "[IQK]===>%s\n", __func__);
	delay_count = 0x0;
	while (notready) {
		if (halrf_rreg(rf, 0xbff8, MASKBYTE0) == 0x55) {
			halrf_delay_us(rf, 1);
			notready = false;
		} else {
			halrf_delay_us(rf, 1);
			delay_count++;
		}
		if (delay_count > 8200) {
			fail = true;
			RF_DBG(rf, DBG_RF_IQK, "[IQK]NCTL1 IQK timeout!!!\n");
			break;
		}
	}
	
	while (notready) {
		if (halrf_rreg(rf, 0x80fc, 0x0000ffff) == 0x8000) {
			halrf_delay_us(rf, 1);
			notready = false;
		} else {
			halrf_delay_us(rf, 1);
			delay_count++;
		}
		if (delay_count > 200) {
			fail = true;
			RF_DBG(rf, DBG_RF_IQK, "[IQK]NCTL2 IQK timeout!!!\n");
			break;
		}
	}

	halrf_delay_us(rf, 10);
	if (!notready)
		fail = (bool)halrf_rreg(rf, 0x8008, BIT(26));
	halrf_wreg(rf, 0x8010, MASKBYTE0, 0x0);

	//DBG_LOG_SERIOUS(DBGMSG_RF, DBG_WARNING, "[IQK]%x\n", delay_count);

	RF_DBG(rf, DBG_RF_IQK, "[IQK]S%x, cnt= %d\n", path, delay_count);
	tmp = halrf_rreg(rf, 0x8008, MASKDWORD);
	RF_DBG(rf, DBG_RF_IQK, "[IQK]S%x, type= %x, 0x8008 = 0x%x \n", path, ktype, tmp);

	return fail;
}

__iram_func__
static bool _iqk_one_shot_8852c(struct rf_info *rf, enum phl_phy_idx phy_idx,
				 u8 path, u8 ktype)
{

	struct halrf_iqk_info *iqk_info = &rf->iqk;
	bool fail = false;
	u32 iqk_cmd = 0x0;
	u8 phy_map;
	u32 addr_rfc_ctl = 0x0;

	phy_map = (BIT(phy_idx) << 4) | BIT(path);

	addr_rfc_ctl = 0x5670 + (path << 13);

	//RF_DBG(rf, DBG_RF_IQK, "[IQK]===>%s\n", __func__);
	//halrf_btc_rfk_ntfy(rf, phy_map, RF_BTC_IQK, RFK_ONESHOT_START);
	switch (ktype) {
	case ID_TXAGC:
		//RF_DBG(rf, DBG_RF_IQK, "[IQK]============ S%d TXAGC ============\n", path);
		iqk_cmd = 0x008 | (1 << (4 + path)) | (path << 1);
		break;
	case ID_A_FLoK_coarse:
		//RF_DBG(rf, DBG_RF_IQK, "[IQK]============ S%d ID_FLoK_coarse ============\n", path);
		halrf_wreg(rf, addr_rfc_ctl, 0x00000002, 0x1);
		iqk_cmd = 0x008 | (1 << (4 + path));
		break;	
	case ID_G_FLoK_coarse:
		//RF_DBG(rf, DBG_RF_IQK, "[IQK]============ S%d ID_FLoK_coarse ============\n", path);
		halrf_wreg(rf, addr_rfc_ctl, 0x00000002, 0x1);
		iqk_cmd = 0x108 | (1 << (4 + path));
		break;
	case ID_A_FLoK_fine:
		//RF_DBG(rf, DBG_RF_IQK, "[IQK]============ S%d ID_FLoK_fine ============\n", path);
		halrf_wreg(rf, addr_rfc_ctl, 0x00000002, 0x1);
		iqk_cmd = 0x508 | (1 << (4 + path));
		break;	
	case ID_G_FLoK_fine:
		//RF_DBG(rf, DBG_RF_IQK, "[IQK]============ S%d ID_FLoK_fine ============\n", path);
		halrf_wreg(rf, addr_rfc_ctl, 0x00000002, 0x1);
		iqk_cmd = 0x208 | (1 << (4 + path));
		break;
	case ID_FLOK_vbuffer:
		//RF_DBG(rf, DBG_RF_IQK, "[IQK]============ S%d ID_FLoK_fine ============\n", path);
		halrf_wreg(rf, addr_rfc_ctl, 0x00000002, 0x1);
		iqk_cmd = 0x308 | (1 << (4 + path));
		break;
	case ID_TXK:
		//RF_DBG(rf, DBG_RF_IQK, "[IQK]============ S%d ID_TXK ============\n", path);
		halrf_wreg(rf, addr_rfc_ctl, 0x00000002, 0x0);
		iqk_cmd = 0x008 | (1 << (4 + path )) | (((0x8 + iqk_info->iqk_bw[path] ) & 0xf) << 8);
		break;
	case ID_RXAGC:
		//RF_DBG(rf, DBG_RF_IQK, "[IQK]============ S%d ID_RXAGC ============\n", path);
		iqk_cmd = 0x508 | (1 << (4 + path)) | (path << 1);
		break;
	case ID_RXK:
		//RF_DBG(rf, DBG_RF_IQK, "[IQK]============ S%d ID_RXK ============\n", path);
		halrf_wreg(rf, addr_rfc_ctl, 0x00000002, 0x1);
		iqk_cmd = 0x008 | (1 << (path + 4)) | (((0xc + iqk_info->iqk_bw[path]) & 0xf) << 8);
		break;
	case ID_NBTXK:
		halrf_wreg(rf, addr_rfc_ctl, 0x00000002, 0x0);
		iqk_cmd = 0x408 | (1 << (4 + path));
		break;
	case ID_NBRXK:
		halrf_wreg(rf, addr_rfc_ctl, 0x00000002, 0x1);
		iqk_cmd = 0x608 | (1 << (4 + path));
		break;
	default:
		return false;
		break;
	}
	RF_DBG(rf, DBG_RF_IQK, "[IQK] CMD_ID = %x, addr_rfc_ctl =%x\n",  iqk_cmd + 1, addr_rfc_ctl);
	halrf_wreg(rf, 0x8000, MASKDWORD, iqk_cmd + 1);
	//halrf_delay_us(rf, 1);

	halrf_delay_us(rf, 1);
	fail = _iqk_check_cal_8852c(rf, path, ktype);

	switch(ktype ) {
	case ID_TXK:
		if (iqk_info->iqk_xym_en == true)
			_iqk_read_xym_8852c(rf, path);
		if (iqk_info->iqk_cfir_en == true) {
			_iqk_read_txcfir_8852c(rf, path, 0x1);
			_iqk_read_txcfir_8852c(rf, path, 0x2);
			_iqk_read_txcfir_8852c(rf, path, 0x3);
		}
		break;
	case ID_RXK:		
		if (iqk_info->iqk_sram_en == true)
			_iqk_sram_8852c(rf, path);
		if (iqk_info->iqk_xym_en == true)
			_iqk_read_xym_8852c(rf, path);
		if (iqk_info->iqk_cfir_en == true) {
			_iqk_read_rxcfir_8852c(rf, path, 0x0);
			_iqk_read_rxcfir_8852c(rf, path, 0x1);
			_iqk_read_rxcfir_8852c(rf, path, 0x2);
			_iqk_read_rxcfir_8852c(rf, path, 0x3);
		}
		break;
	default:
		break;
	}
	//8. IQK cotrol RFC
	halrf_wreg(rf, addr_rfc_ctl, 0x00000002, 0x0);

	//halrf_btc_rfk_ntfy(rf, phy_map, RF_BTC_IQK, RFK_ONESHOT_STOP);

	return fail;
}

__iram_func__
static bool _rxk_group_sel_8852c(struct rf_info *rf, enum phl_phy_idx phy_idx,
				  u8 path)
{

	struct halrf_iqk_info *iqk_info = &rf->iqk;
	u8 gp = 0x0;
	
	u32 a6_idxrxgain[4] = {0x190, 0x196, 0x290, 0x316};
	u32 a6_idxattc2[4] = {0x00, 0x00, 0x00, 0x00};
	u32 a_idxrxgain[4] = {0x190, 0x198, 0x310, 0x318};
	//u32 a_idxattc2[4] = {0x00, 0x01, 0x00, 0x01};	
	u32 a_idxattc2[4] = {0x00, 0x00, 0x00, 0x00};	
	u32 g_idxrxgain[4] = {0x252, 0x26c, 0x350, 0x360};
	u32 g_idxattc2[4] = {0x00, 0x07, 0x00, 0x3};
	bool fail = false;
	bool kfail = false;
	u32 tmp = 0;
	u32 bkrf0 = 0;

	RF_DBG(rf, DBG_RF_IQK, "[IQK]===>%s\n", __func__);
	bkrf0 = halrf_rrf(rf, path, 0x00, 0x0c000);
	if (path == RF_PATH_B) {		
		RF_DBG(rf, DBG_RF_IQK, "[IQK]WA for PATH B\n");
		halrf_wrf(rf, RF_PATH_B, 0xdc, 0x00300, 0x3);
		 tmp = halrf_rrf(rf, RF_PATH_B, 0x17, 0x00c00);
		halrf_wrf(rf, RF_PATH_B, 0x1f, 0x30000, tmp);
		tmp = halrf_rrf(rf, RF_PATH_B, 0x17, 0x003ff);
		halrf_wrf(rf, RF_PATH_B, 0x1f, 0x003ff, tmp);
		
		RF_DBG(rf, DBG_RF_IQK, "[IQK]S0 RF0x18 = %x\n", halrf_rrf(rf, RF_PATH_A, 0x18, 0xfffff));
		RF_DBG(rf, DBG_RF_IQK, "[IQK]S1 RF0x17 = %x\n", halrf_rrf(rf, RF_PATH_B, 0x17, 0xfffff));
		RF_DBG(rf, DBG_RF_IQK, "[IQK]S1 RF0x1f = %x\n", halrf_rrf(rf, RF_PATH_B, 0x1f, 0xfffff));
	}

	switch (iqk_info->iqk_band[path]) {
	case BAND_ON_24G:
		halrf_wrf(rf, path, 0x00, 0xf0000, 0xc);
		halrf_wrf(rf, path, 0x00, 0x0c000, 0x0);
		halrf_wrf(rf, path, 0x84, 0xf0000, 0x9);
	break;
	case BAND_ON_5G:
		halrf_wrf(rf, path, 0x00, 0xf0000, 0xc);
		halrf_wrf(rf, path, 0x00, 0x0c000, 0x0);
		//halrf_wrf(rf, path, 0x89, 0x0000f, 0x9);
		halrf_wrf(rf, path, 0x89, 0x0000f, 0x8);
	break;	
	case BAND_ON_6G:
		halrf_wrf(rf, path, 0x00, 0xf0000, 0xc);
		halrf_wrf(rf, path, 0x00, 0x0c000, 0x0);
		halrf_wrf(rf, path, 0x89, 0x0000f, 0x9);
	break;
	default:
	break;
	}
	halrf_delay_us(rf, 10);
	for (gp = 0; gp < 0x4; gp++) {
		switch (iqk_info->iqk_band[path]) {
		case BAND_ON_24G:
			halrf_wrf(rf, path, 0x00, 0x03ff0, g_idxrxgain[gp]);
			halrf_wrf(rf, path, 0x83, 0x0f000, g_idxattc2[gp]);
		break;
		case BAND_ON_5G:
			halrf_wrf(rf, path, 0x00, 0x03ff0, a_idxrxgain[gp]);
			halrf_wrf(rf, path, 0x8c, 0x000f0, a_idxattc2[gp]);
		break;		
		case BAND_ON_6G:
			halrf_wrf(rf, path, 0x00, 0x03ff0, a6_idxrxgain[gp]);
			halrf_wrf(rf, path, 0x8c, 0x000f0, a6_idxattc2[gp]);
		break;
		default:
		break;
		}
		halrf_wreg(rf, 0x8154 + (path << 8), 0x00000100, 0x1);
		halrf_wreg(rf, 0x8154 + (path << 8), 0x00000010, 0x0);
		halrf_wreg(rf, 0x8154 + (path << 8), 0x00000007, gp);
		fail = _iqk_one_shot_8852c(rf, phy_idx, path, ID_RXK);
		halrf_wreg(rf, 0x9fe0, BIT(16 + gp + path * 4), fail);
		kfail = kfail | fail;
	}

	if (path == RF_PATH_B)
		halrf_wrf(rf, path, 0xdc, 0x00300, 0x0);
	halrf_wrf(rf, path, 0x00, 0x0c000, bkrf0);

	if (kfail) {
		iqk_info->nb_rxcfir[path] = 0x40000002;
		//halrf_wreg(rf, 0x8124 + (path << 8), 0x0000000f, 0x0);
		iqk_info->is_wb_rxiqk[path] = false;
	} else {
		iqk_info->nb_rxcfir[path] = 0x40000000;
		//halrf_wreg(rf, 0x8124 + (path << 8), 0x0000000f, 0x5);
		iqk_info->is_wb_rxiqk[path] = true;
	}
/*
	tmp = halrf_rreg(rf, 0x813c + (path << 8), MASKDWORD);
	RF_DBG(rf, DBG_RF_IQK, "[IQK]S%x, 0x8%x3c = 0x%x\n", path, 1 << path, tmp);
*/
	return kfail;
}

__iram_func__
static bool _iqk_nbrxk_8852c(struct rf_info *rf, enum phl_phy_idx phy_idx,
			      u8 path)
{
	struct halrf_iqk_info *iqk_info = &rf->iqk;
	u8 gp = 0x0;
	// 52C RX Gain Setting	
	u32 a6_idxrxgain[4] = {0x190, 0x196, 0x290, 0x316};
	u32 a6_idxattc2[4] = {0x00, 0x00, 0x00, 0x00};
	u32 a_idxrxgain[4] = {0x190, 0x198, 0x310, 0x318};
	u32 a_idxattc2[4] = {0x00, 0x01, 0x00, 0x01};
	u32 g_idxrxgain[4] = {0x252, 0x26c, 0x350, 0x360};
	u32 g_idxattc2[4] = {0x00, 0x07, 0x00, 0x3};
	bool fail = false;
	bool kfail = false;
	u32 tmp = 0x0;
	bool notready = true;
	u32 delay_count = 0;
	u32 bkrf0 = 0;

	//u32 pwr_sig = 0x0, pwr_img = 0x0, rximr = 0x0;
	RF_DBG(rf, DBG_RF_IQK, "[IQK]===>%s\n", __func__);

	bkrf0 = halrf_rrf(rf, path, 0x00, 0x0c000);
	if (path == RF_PATH_B) {		
		RF_DBG(rf, DBG_RF_IQK, "[IQK]WA for PATH B\n");
		halrf_wrf(rf, RF_PATH_B, 0xdc, 0x00300, 0x3);
		 tmp = halrf_rrf(rf, RF_PATH_B, 0x17, 0x00c00);
		halrf_wrf(rf, RF_PATH_B, 0x1f, 0x30000, tmp);
		tmp = halrf_rrf(rf, RF_PATH_B, 0x17, 0x003ff);
		halrf_wrf(rf, RF_PATH_B, 0x1f, 0x003ff, tmp);

#ifndef HALRF_CONFIG_FW_IO_OFLD_SUPPORT /*NOT USB*/
		RF_DBG(rf, DBG_RF_IQK, "[IQK]S0 RF0x18 = %x\n", halrf_rrf(rf, RF_PATH_A, 0x18, 0xfffff));
		RF_DBG(rf, DBG_RF_IQK, "[IQK]S1 RF0x17 = %x\n", halrf_rrf(rf, RF_PATH_B, 0x17, 0xfffff));
		RF_DBG(rf, DBG_RF_IQK, "[IQK]S1 RF0x1f = %x\n", halrf_rrf(rf, RF_PATH_B, 0x1f, 0xfffff));
#endif
	}
	gp =0x3;

	switch (iqk_info->iqk_band[path]) {
	case BAND_ON_24G:
		halrf_wrf(rf, path, 0x00, 0xf0000, 0xc);
		halrf_wrf(rf, path, 0x00, 0x0c000, 0x0);
		halrf_wrf(rf, path, 0x84, 0xf0000, 0x9);
		halrf_wrf(rf, path, 0x00, 0x03ff0, g_idxrxgain[gp]);
		halrf_wrf(rf, path, 0x83, 0x0f000, g_idxattc2[gp]);
	break;
	case BAND_ON_5G:
		halrf_wrf(rf, path, 0x00, 0xf0000, 0xc);
		halrf_wrf(rf, path, 0x00, 0x0c000, 0x0);
		halrf_wrf(rf, path, 0x89, 0x0000f, 0x9);
		halrf_wrf(rf, path, 0x00, 0x03ff0, a_idxrxgain[gp]);
		halrf_wrf(rf, path, 0x8c, 0x000f0, a_idxattc2[gp]);
	break;	
	case BAND_ON_6G:
		halrf_wrf(rf, path, 0x00, 0xf0000, 0xc);
		halrf_wrf(rf, path, 0x00, 0x0c000, 0x0);
		halrf_wrf(rf, path, 0x89, 0x0000f, 0x9);
		halrf_wrf(rf, path, 0x00, 0x03ff0, a6_idxrxgain[gp]);
		halrf_wrf(rf, path, 0x8c, 0x000f0, a6_idxattc2[gp]);
	break;
	default:
		break;
	}	

	halrf_delay_us(rf, 100); 
	halrf_wreg(rf, 0x8154 + (path << 8), 0x00000100, 0x1);
	halrf_wreg(rf, 0x8154 + (path << 8), 0x00000010, 0x0);
	halrf_wreg(rf, 0x8154 + (path << 8), 0x00000007, gp);	
	halrf_wreg(rf, 0x802c, 0x0fff0000, 0x011);
	halrf_wrf(rf, path, 0x1e, 0x0003f, 0x13);
	halrf_delay_us(rf, 60);
	while (notready) {
		if (halrf_rrf(rf, path, 0x1e, 0x40000) == 0x1) {
			halrf_delay_us(rf, 1);
			notready = false;
		} else {
			halrf_delay_us(rf, 1);
			delay_count++;
		}
		if (delay_count > 200) {
			notready = false;
			fail = true;
			RF_DBG(rf, DBG_RF_IQK, "[IQK]IQPLL timeout!!!\n");
			break;
		}
	}
	RF_DBG(rf, DBG_RF_IQK, "[IQK]S%x, 0x1e = %x\n", path, halrf_rrf(rf, path, 0x1e, MASKDWORD));

	fail = _iqk_one_shot_8852c(rf, phy_idx, path, ID_NBRXK);
	kfail = _iqk_check_nbiqc_8852c(rf, phy_idx, ID_NBRXK, path);

	if (!kfail) {
		iqk_info->nb_rxcfir[path] = halrf_rreg(rf, 0x813c + (path << 8), MASKDWORD) | 0x2;
	} else {
		iqk_info->nb_rxcfir[path] = 0x40000002;
	}
	if (path == RF_PATH_B)
		halrf_wrf(rf, path, 0xdc, 0x00300, 0x0);
	halrf_wrf(rf, path, 0x00, 0x0c000, bkrf0);

	iqk_info->is_wb_rxiqk[path] = false;

	RF_DBG(rf, DBG_RF_IQK, "[IQK]S%x, result = %x, 0x8%x3c = 0x%x\n", path, fail, 1 << path, halrf_rreg(rf, 0x813c + (path << 8), MASKDWORD));
	return kfail;
}

__iram_func__
static bool _txk_group_sel_8852c(struct rf_info *rf, enum phl_phy_idx phy_idx,
				  u8 path)
{

	struct halrf_iqk_info *iqk_info = &rf->iqk;
	bool fail = false;
//	bool kfail = false;
	u8 gp = 0x0;
	
	u32 a6_power_range[1] = {0x0};
	u32 a6_track_range[1] = {0x3};
	u32 a6_gain_bb[1] = {0x12};
	u32 a6_itqt[1] = {0x12};

	u32 a_power_range[3] = {0x0, 0x0, 0x0};
	u32 a_track_range[3] = {0x3, 0x6, 0x6};
	u32 a_gain_bb[3] = {0x12, 0x0a, 0x0f};
	u32 a_itqt[3] = {0x12, 0x12, 0x12};
	
	u32 g_power_range[3] = {0x0, 0x0, 0x0};
	u32 g_track_range[3] = {0x5, 0x6, 0x6};
	u32 g_gain_bb[3] = {0x0e, 0x0a, 0x0e};
	u32 g_itqt[3] = { 0x12, 0x12, 0x12};

	//RF_DBG(rf, DBG_RF_IQK, "[IQK]===>%s\n", __func__);

	if (iqk_info->iqk_band[path] == BAND_ON_6G) {
		for (gp = 0x0; gp < 0x1; gp++) {
			switch (iqk_info->iqk_band[path]) {
			case BAND_ON_6G:
				halrf_wrf(rf, path, 0x11, 0x00003, a6_power_range[gp]);
				halrf_wrf(rf, path, 0x11, 0x00070, a6_track_range[gp]);
				halrf_wrf(rf, path, 0x11, 0x1f000, a6_gain_bb[gp]);
				halrf_wreg(rf, 0x81cc + (path << 8), MASKDWORD, a6_itqt[gp]);
			break;
			default:
				break;
			}
			halrf_wreg(rf, 0x8154 + (path << 8), 0x00000100, 0x1); //man_sel_cfir_lut
			halrf_wreg(rf, 0x8154 + (path << 8), 0x00000010, 0x1); //TX=0x1 or RX=0x0
			halrf_wreg(rf, 0x8154 + (path << 8), 0x00000004, 0x0); //force to zero
			halrf_wreg(rf, 0x8154 + (path << 8), 0x00000003, gp + 1);//remapping as {idx_rfgain, idx_txbb}	
			halrf_wreg(rf, 0x8010, 0x000000ff, 0x00);
			fail = _iqk_one_shot_8852c(rf, phy_idx, path, ID_TXK);
			halrf_wreg(rf, 0x9fe0, BIT(8 + gp + path * 4), fail);
			}
	} else {
		for (gp = 0x0; gp < 0x3; gp++) {
			switch (iqk_info->iqk_band[path]) {
			case BAND_ON_24G:
				halrf_wrf(rf, path, 0x11, 0x00003, g_power_range[gp]);
				halrf_wrf(rf, path, 0x11, 0x00070, g_track_range[gp]);
				halrf_wrf(rf, path, 0x11, 0x1f000, g_gain_bb[gp]);
				halrf_wreg(rf, 0x81cc + (path << 8), MASKDWORD, g_itqt[gp]);
				break;
			case BAND_ON_5G:
				halrf_wrf(rf, path, 0x11, 0x00003, a_power_range[gp]);
				halrf_wrf(rf, path, 0x11, 0x00070, a_track_range[gp]);
				halrf_wrf(rf, path, 0x11, 0x1f000, a_gain_bb[gp]);
				halrf_wreg(rf, 0x81cc + (path << 8), MASKDWORD, a_itqt[gp]);
				break;
			default:
				break;
			}
			halrf_wreg(rf, 0x8154 + (path << 8), 0x00000100, 0x1); //man_sel_cfir_lut
			halrf_wreg(rf, 0x8154 + (path << 8), 0x00000010, 0x1); //TX=0x1 or RX=0x0
			halrf_wreg(rf, 0x8154 + (path << 8), 0x00000004, 0x0); //force to zero
			halrf_wreg(rf, 0x8154 + (path << 8), 0x00000003, gp + 1);//remapping as {idx_rfgain, idx_txbb}	
			halrf_wreg(rf, 0x8010, 0x000000ff, 0x00);
			fail = _iqk_one_shot_8852c(rf, phy_idx, path, ID_TXK);
			halrf_wreg(rf, 0x9fe0, BIT(8 + gp + path * 4), fail);
			}
		}

		iqk_info->is_wb_txiqk[path] = true;

	RF_DBG(rf, DBG_RF_IQK, "[IQK]S%x, 0x8%x38 = 0x%x\n", path, 1 << path, halrf_rreg(rf, 0x8138 + (path << 8), MASKDWORD));

	return fail;
}

__iram_func__
static bool _iqk_nbtxk_8852c(struct rf_info *rf, enum phl_phy_idx phy_idx,
			      u8 path)
{
	struct halrf_iqk_info *iqk_info = &rf->iqk;
	bool fail = false;
	bool kfail = false;
	u8 gp = 0x0;
	u8 thermal = 0;
	u32 a6_power_range[1] = {0x0};
	u32 a6_track_range[1] = {0x3};
	u32 a6_gain_bb[1] = {0x12};
	u32 a6_itqt[1] = {0x12};

	u32 a_power_range[3] = {0x0, 0x0, 0x0};
	u32 a_track_range[3] = {0x3, 0x6, 0x6};
	u32 a_gain_bb[3] = {0x12, 0x0a, 0x0f};
	u32 a_itqt[3] = {0x12, 0x12, 0x12};
	
	u32 g_power_range[3] = {0x0, 0x0, 0x0};
	u32 g_track_range[3] = {0x5, 0x6, 0x6};
	u32 g_gain_bb[3] = {0x0e, 0x0a, 0x0e};
	u32 g_itqt[3] = { 0x12, 0x12, 0x12};

	RF_DBG(rf, DBG_RF_IQK, "[IQK]===>%s\n", __func__);
	
	if (iqk_info->iqk_band[path] == BAND_ON_6G) {
		for (gp = 0x0; gp < 0x1; gp++) {
			switch (iqk_info->iqk_band[path]) {
			case BAND_ON_6G:
				halrf_wrf(rf, path, 0x11, 0x00003, a6_power_range[gp]);
				halrf_wrf(rf, path, 0x11, 0x00070, a6_track_range[gp]);
				halrf_wrf(rf, path, 0x11, 0x1f000, a6_gain_bb[gp]);
				halrf_wreg(rf, 0x81cc + (path << 8), MASKDWORD, a6_itqt[gp]);
			break;
			default:
				break;
			}
			halrf_wreg(rf, 0x8154 + (path << 8), 0x00000100, 0x1); //man_sel_cfir_lut
			halrf_wreg(rf, 0x8154 + (path << 8), 0x00000010, 0x1); //TX=0x1 or RX=0x0
			halrf_wreg(rf, 0x8154 + (path << 8), 0x00000004, 0x0); //force to zero
			halrf_wreg(rf, 0x8154 + (path << 8), 0x00000003, gp + 1);//remapping as {idx_rfgain, idx_txbb}	
			halrf_wreg(rf, 0x8010, 0x000000ff, 0x00);
			if(thermal < 0x15)
				halrf_wreg(rf, 0x81cc + (path << 8), MASKDWORD, 0x1b);
			
			fail = _iqk_one_shot_8852c(rf, phy_idx, path, ID_NBTXK);
			halrf_wreg(rf, 0x9fe0, BIT(8 + gp + path * 4), fail);
			kfail = kfail | fail;
			}
	} else {
			gp = 0x2;
	 		switch (iqk_info->iqk_band[path]) {
			case BAND_ON_24G:
				halrf_wrf(rf, path, 0x11, 0x00003, g_power_range[gp]);
				halrf_wrf(rf, path, 0x11, 0x00070, g_track_range[gp]);
				halrf_wrf(rf, path, 0x11, 0x1f000, g_gain_bb[gp]);
				halrf_wreg(rf, 0x81cc + (path << 8), MASKDWORD, g_itqt[gp]);
				break;
			case BAND_ON_5G:
				halrf_wrf(rf, path, 0x11, 0x00003, a_power_range[gp]);
				halrf_wrf(rf, path, 0x11, 0x00070, a_track_range[gp]);
				halrf_wrf(rf, path, 0x11, 0x1f000, a_gain_bb[gp]);
				halrf_wreg(rf, 0x81cc + (path << 8), MASKDWORD, a_itqt[gp]);
				break;
			default:
				break;
			}
			halrf_wreg(rf, 0x8154 + (path << 8), 0x00000100, 0x1); //man_sel_cfir_lut
			halrf_wreg(rf, 0x8154 + (path << 8), 0x00000010, 0x1); //TX=0x1 or RX=0x0
			halrf_wreg(rf, 0x8154 + (path << 8), 0x00000004, 0x0); //force to zero
			halrf_wreg(rf, 0x8154 + (path << 8), 0x00000003, gp + 1);//remapping as {idx_rfgain, idx_txbb}	
			halrf_wreg(rf, 0x8010, 0x000000ff, 0x00);			
			if(thermal < 0x15)
				halrf_wreg(rf, 0x81cc + (path << 8), MASKDWORD, 0x1b);
			
			kfail = _iqk_one_shot_8852c(rf, phy_idx, path, ID_NBTXK);

	 	}

		if (!kfail) {
			iqk_info->nb_txcfir[path] = halrf_rreg(rf, 0x8138 + (path << 8), MASKDWORD)  | 0x2;
		} else {
			iqk_info->nb_txcfir[path] = 0x40000002;
		}
		RF_DBG(rf, DBG_RF_IQK, "[IQK]S%x, 0x8%x38 = 0x%x\n", path, 1 << path, halrf_rreg(rf, 0x8138 + (path << 8), MASKDWORD));
		iqk_info->is_wb_txiqk[path] = false;
	return kfail;
}

__iram_func__
static void _lok_res_table_8852c(struct rf_info *rf, u8 path, u8 ibias)
{

	struct halrf_iqk_info *iqk_info = &rf->iqk;

	RF_DBG(rf, DBG_RF_IQK, "[IQK]===>%s\n", __func__);
	RF_DBG(rf, DBG_RF_IQK, "[IQK]S%x, ibias = %x\n", path, ibias);
	halrf_wrf(rf, path, 0xef, 0x00002, 0x1);
	if (iqk_info->iqk_band[path] == BAND_ON_24G)
		halrf_wrf(rf, path, 0x30, 0x08000, 0x0);
	else
		halrf_wrf(rf, path, 0x30, 0x08000, 0x1);

	halrf_wrf(rf, path, 0x30, 0x00038, ibias);

#ifndef HALRF_CONFIG_FW_IO_OFLD_SUPPORT /*NOT USB*/
	RF_DBG(rf, DBG_RF_IQK, "[IQK]S%x, 0x30 = 0x%x\n", path, halrf_rrf(rf, path, 0x30, 0xfffff));
#endif

	halrf_wrf(rf, path, 0xef, 0x00002, 0x0);
	return;
}

__iram_func__
static bool _lok_finetune_check_8852c(struct rf_info *rf, u8 path)
{
	struct halrf_iqk_info *iqk_info = &rf->iqk;
	bool is_fail = false, is_fail1 = false,  is_fail2 = false;
	u32 temp = 0x0;
	u32 core_i = 0x0;
	u32 core_q = 0x0;
	u8 ch = 0x0;
	u32 vbuff_i = 0x0;
	u32 vbuff_q = 0x0;

	//RF_DBG(rf, DBG_RF_IQK, "[IQK]===>%s\n", __func__);
	
	temp = halrf_rrf(rf, path, 0x5c, MASKRF);

	//temp = halrf_rrf(rf, path, 0x08, MASKRF);
	core_i = (temp & 0xfc000) >> 14;
	core_q = (temp & 0x03f00) >> 8;
	ch = iqk_info->iqk_table_idx[path];

	if (core_i  < 0x3 || core_i  > 0x3c || core_q < 0x3 || core_q > 0x3c) 
		is_fail1 = true;
	else 
		is_fail1 = false;

	iqk_info->lok_idac[ch][path] = temp;

#ifndef HALRF_CONFIG_FW_IO_OFLD_SUPPORT	/*NOT USB*/
	RF_DBG(rf, DBG_RF_IQK, "[IQK]S%x, core_i = 0x%x, core_q = 0x%x\n", path, core_i, core_q);
	RF_DBG(rf, DBG_RF_IQK, "[IQK]S%x, 0x5c[19:14] = 0x%x, 0x5c[13:8] = 0x%x\n", path, halrf_rrf(rf, path, 0x5c, 0xfc000), halrf_rrf(rf, path, 0x5c, 0x03f00));
	RF_DBG(rf, DBG_RF_IQK, "[IQK]S%x, 0x08[19:14] = 0x%x, 0x08[09:4] = 0x%x\n", path, halrf_rrf(rf, path, 0x08, 0xfc000), halrf_rrf(rf, path, 0x08, 0x003f0));
	RF_DBG(rf, DBG_RF_IQK, "[IQK]S%x, lok_idac[%x][%x] = 0x%x\n", path, ch, path, iqk_info->lok_idac[ch][path]);
#endif

	temp = halrf_rrf(rf, path, 0x0a, MASKRF);
	vbuff_i = (temp & 0xfc000) >> 14;
	vbuff_q = (temp & 0x003f0) >> 4;

	if (vbuff_i  < 0x2 || vbuff_i  > 0x3d || vbuff_q < 0x2 || vbuff_q > 0x3d) {
		is_fail2 = true;
	} else {	
		is_fail2 = false;
	}
	iqk_info->lok_vbuf[ch][path] = temp;

	is_fail = is_fail1 |  is_fail2;
	
	RF_DBG(rf, DBG_RF_IQK, "[IQK]S%x, lok_vbuf[%x][%x] = 0x%x\n", path, ch, path, iqk_info->lok_vbuf[ch][path]);
	is_fail = is_fail1;

	return is_fail;
}

__iram_func__
static bool _iqk_lok_2g_bygain_8852c(struct rf_info *rf, enum phl_phy_idx phy_idx,
			    u8 path)
{
//	struct halrf_iqk_info *iqk_info = &rf->iqk;
	bool isfail;

	RF_DBG(rf, DBG_RF_IQK, "[IQK]===>%s\n", __func__);
	halrf_wrf(rf, path, 0x11, 0x00003, 0x0);
	halrf_wrf(rf, path, 0x11, 0x00070, 0x6);
	halrf_wrf(rf, path, 0x11, 0x1f000, 0x6);
	halrf_wreg(rf, 0x5670 + (path << 13), 0x00000002, 0x1);
	halrf_wreg(rf, 0x81cc + (path << 8), 0x0000003f, 0x09);
	halrf_wreg(rf, 0x802c, 0x00000fff, 0x021);
	halrf_wreg(rf, 0x8000, MASKDWORD, 0x00000109 | (1 << (4 + path)));
	isfail = _iqk_check_cal_8852c(rf, path, lok);
	halrf_wreg(rf, 0x8010, 0x000000ff, 0x00);
	halrf_wreg(rf, 0x5670 + (path << 13), 0x00000002, 0x0);

	halrf_wrf(rf, path, 0x11, 0x1f000, 0x12);
	halrf_wreg(rf, 0x5670 + (path << 13), 0x00000002, 0x1);
	halrf_wreg(rf, 0x81cc + (path << 8), 0x0000003f, 0x1b);
	halrf_wreg(rf, 0x8000, MASKDWORD, 0x00000309 | (1 << (4 + path)));
	isfail = _iqk_check_cal_8852c(rf, path, lok);
	halrf_wreg(rf, 0x8010, 0x000000ff, 0x00);
	halrf_wreg(rf, 0x5670 + (path << 13), 0x00000002, 0x0);

	halrf_wrf(rf, path, 0x11, 0x1f000, 0x6);
	halrf_wreg(rf, 0x5670 + (path << 13), 0x00000002, 0x1);
	halrf_wreg(rf, 0x81cc + (path << 8), 0x0000003f, 0x09);
	halrf_wreg(rf, 0x8000, MASKDWORD, 0x00000209 | (1 << (4 + path)));
	isfail = _iqk_check_cal_8852c(rf, path, lok);
	halrf_wreg(rf, 0x8010, 0x000000ff, 0x00);
	halrf_wreg(rf, 0x5670 + (path << 13), 0x00000002, 0x0);

	halrf_wrf(rf, path, 0x11, 0x1f000, 0x12);
	halrf_wreg(rf, 0x5670 + (path << 13), 0x00000002, 0x1);
	halrf_wreg(rf, 0x81cc + (path << 8), 0x0000003f, 0x1b);
	halrf_wreg(rf, 0x8000, MASKDWORD, 0x00000309 | (1 << (4 + path)));
	isfail = _iqk_check_cal_8852c(rf, path, lok);
	halrf_wreg(rf, 0x8010, 0x000000ff, 0x00);

	halrf_wreg(rf, 0x5670 + (path << 13), 0x00000002, 0x0);

	return isfail;

}

__iram_func__
static bool _iqk_lok_5g_bygain_8852c(struct rf_info *rf, enum phl_phy_idx phy_idx,
			    u8 path)
{
//	struct halrf_iqk_info *iqk_info = &rf->iqk;
	bool isfail;
	u8 i, j, k, n, m;
	u32 rf_reg08[4] = {0x00, 0x00, 0x00, 0x00};
	u32 a_track_idx[3] = {0x00, 0x04, 0x06};
	u32 a_txbb_L[7] = {0x00, 0x04, 0x08, 0x0c, 0x0e, 0x10, 0x12};
	u32 a_txbb_H[3] = {0x14, 0x16 ,0x17};

	RF_DBG(rf, DBG_RF_IQK, "[IQK]===>%s\n", __func__);
	halrf_wrf(rf, path, 0x11, 0x00003, 0x0);
	halrf_wrf(rf, path, 0x11, 0x00070, 0x6);
	halrf_wrf(rf, path, 0x11, 0x1f000, 0x12);
//Tracking = 6
	halrf_wreg(rf, 0x5670 + (path << 13), 0x00000002, 0x1);
	halrf_wreg(rf, 0x81cc + (path << 8), 0x0000003f, 0x1b);
	halrf_wreg(rf, 0x802c, 0x00000fff, 0x021);
	halrf_wreg(rf, 0x8000, MASKDWORD, 0x00000709 | (1 << (4 + path)));
	isfail = _iqk_check_cal_8852c(rf, path, lok);
	halrf_wreg(rf, 0x8010, 0x000000ff, 0x00);
	halrf_wreg(rf, 0x5670 + (path << 13), 0x00000002, 0x0);

	halrf_wrf(rf, path, 0x11, 0x1f000, 0x06);
	halrf_wreg(rf, 0x5670 + (path << 13), 0x00000002, 0x1);
	halrf_wreg(rf, 0x81cc + (path << 8), 0x0000003f, 0x09);
	halrf_wreg(rf, 0x8000, MASKDWORD, 0x00000509 | (1 << (4 + path)));
	isfail = _iqk_check_cal_8852c(rf, path, lok);
	halrf_wreg(rf, 0x8010, 0x000000ff, 0x00);
	halrf_wreg(rf, 0x5670 + (path << 13), 0x00000002, 0x0);

	halrf_wrf(rf, path, 0x11, 0x1f000, 0x12);
	halrf_wreg(rf, 0x5670 + (path << 13), 0x00000002, 0x1);
	halrf_wreg(rf, 0x81cc + (path << 8), 0x0000003f, 0x1b);
	halrf_wreg(rf, 0x8000, MASKDWORD, 0x00000309 | (1 << (4 + path)));
	isfail = _iqk_check_cal_8852c(rf, path, lok);
	halrf_wreg(rf, 0x8010, 0x000000ff, 0x00);
	halrf_wreg(rf, 0x5670 + (path << 13), 0x00000002, 0x0);
	rf_reg08[0]= halrf_rrf(rf, path, 0x8, MASKRF); //2
	RF_DBG(rf, DBG_RF_IQK, "[IQK]S%x, rf_reg08[0] = %x, 0x08[19:14] = 0x%x, 0x08[09:4] = 0x%x\n", path, rf_reg08[0], halrf_rrf(rf, path, 0x08, 0xfc000), halrf_rrf(rf, path, 0x08, 0x003f0));

	halrf_wrf(rf, path, 0x11, 0x1f000, 0x15);
	halrf_wreg(rf, 0x5670 + (path << 13), 0x00000002, 0x1);
	halrf_wreg(rf, 0x81cc + (path << 8), 0x0000003f, 0x1b);
	halrf_wreg(rf, 0x8000, MASKDWORD, 0x00000509 | (1 << (4 + path)));
	isfail = _iqk_check_cal_8852c(rf, path, lok);
	halrf_wreg(rf, 0x8010, 0x000000ff, 0x00);
	halrf_wreg(rf, 0x5670 + (path << 13), 0x00000002, 0x0);
	rf_reg08[1] = halrf_rrf(rf, path, 0x8, MASKRF);
	RF_DBG(rf, DBG_RF_IQK, "[IQK]S%x, rf_reg08[1] = %x, 0x08[19:14] = 0x%x, 0x08[09:4] = 0x%x\n", path, rf_reg08[1], halrf_rrf(rf, path, 0x08, 0xfc000), halrf_rrf(rf, path, 0x08, 0x003f0));

	halrf_wrf(rf, path, 0x11, 0x00070, 0x7);
	halrf_wreg(rf, 0x5670 + (path << 13), 0x00000002, 0x1);
	halrf_wreg(rf, 0x81cc + (path << 8), 0x0000003f, 0x1b);
	halrf_wreg(rf, 0x8000, MASKDWORD, 0x00000509 | (1 << (4 + path)));
	isfail = _iqk_check_cal_8852c(rf, path, lok);
	halrf_wreg(rf, 0x8010, 0x000000ff, 0x00);
	halrf_wreg(rf, 0x5670 + (path << 13), 0x00000002, 0x0);
	rf_reg08[3] = halrf_rrf(rf, path, 0x8, MASKRF);
	RF_DBG(rf, DBG_RF_IQK, "[IQK]S%x, rf_reg08[3] = %x, 0x08[19:14] = 0x%x, 0x08[09:4] = 0x%x\n", path, rf_reg08[3], halrf_rrf(rf, path, 0x08, 0xfc000), halrf_rrf(rf, path, 0x08, 0x003f0));

	halrf_wrf(rf, path, 0x11, 0x1f000, 0x06);
	halrf_wreg(rf, 0x5670 + (path << 13), 0x00000002, 0x1);
	halrf_wreg(rf, 0x81cc + (path << 8), 0x0000003f, 0x09);
	halrf_wreg(rf, 0x8000, MASKDWORD, 0x00000509 | (1 << (4 + path)));
	isfail = _iqk_check_cal_8852c(rf, path, lok);
	halrf_wreg(rf, 0x8010, 0x000000ff, 0x00);
	halrf_wreg(rf, 0x5670 + (path << 13), 0x00000002, 0x0);
	rf_reg08[2] = halrf_rrf(rf, path, 0x8, MASKRF);
	RF_DBG(rf, DBG_RF_IQK, "[IQK]S%x, rf_reg08[2] = %x, 0x08[19:14] = 0x%x, 0x08[09:4] = 0x%x\n", path, rf_reg08[2], halrf_rrf(rf, path, 0x08, 0xfc000), halrf_rrf(rf, path, 0x08, 0x003f0));

	halrf_wrf(rf, path, 0x55, 0x00006, 0x3);
	for(i = 0; i < 0x3; i++){
		halrf_wrf(rf, path, 0x11, 0x00070, a_track_idx[i]);
		for(j = 0; j < 0x7; j++){
			halrf_wrf(rf, path, 0x11, 0x1f000, a_txbb_L[j]);
			halrf_wrf(rf, path, 0x08, 0xfffff, rf_reg08[0]);
			RF_DBG(rf, DBG_RF_IQK, "[IQK]S%x, [i, j]=[%x, %x],0x11[12:15]=%x, 0x8[0] = %x\n", path, i, j, halrf_rrf(rf, path, 0x11, 0x1f000), halrf_rrf(rf, path, 0x08, 0xfffff));
		}		
		for(k = 0; k < 0x3; k++){
			halrf_wrf(rf, path, 0x11, 0x1f000, a_txbb_H[k]);
			halrf_wrf(rf, path, 0x08, 0xfffff, rf_reg08[1]);			
			RF_DBG(rf, DBG_RF_IQK, "[IQK]S%x, [i, k]=[%x, %x],0x11[12:15]=%x, 0x8[1] = %x\n", path, i, k, halrf_rrf(rf, path, 0x11, 0x1f000), halrf_rrf(rf, path, 0x08, 0xfffff));
		}
	}
	halrf_wrf(rf, path, 0x11, 0x00070, 0x7);
	for(n = 0; n < 0x7; n++){
		halrf_wrf(rf, path, 0x11, 0x1f000, a_txbb_L[n]);
		halrf_wrf(rf, path, 0x08, 0xfffff, rf_reg08[2]);		
		RF_DBG(rf, DBG_RF_IQK, "[IQK]S%x, [n]=[%x],0x11[12:15]=%x, 0x8[2] = %x\n", path, n, halrf_rrf(rf, path, 0x11, 0x1f000), halrf_rrf(rf, path, 0x08, 0xfffff));
	}
	for(m = 0; m < 0x3; m++){
		halrf_wrf(rf, path, 0x11, 0x1f000, a_txbb_H[m]);
		halrf_wrf(rf, path, 0x08, 0xfffff, rf_reg08[3]);		
		RF_DBG(rf, DBG_RF_IQK, "[IQK]S%x, [m]=[%x],0x11[12:15]=%x, 0x8[3] = %x\n", path, m, halrf_rrf(rf, path, 0x11, 0x1f000), halrf_rrf(rf, path, 0x08, 0xfffff));
	}
	return isfail;

}

__iram_func__
static bool _iqk_lok_6g_bygain_8852c(struct rf_info *rf, enum phl_phy_idx phy_idx,
			    u8 path)
{
//	struct halrf_iqk_info *iqk_info = &rf->iqk;
	bool isfail;

	RF_DBG(rf, DBG_RF_IQK, "[IQK]===>%s\n", __func__);
	halrf_wrf(rf, path, 0x11, 0x00003, 0x0);
	halrf_wrf(rf, path, 0x11, 0x00070, 0x6);
	halrf_wrf(rf, path, 0x11, 0x1f000, 0x12);
	halrf_wreg(rf, 0x5670 + (path << 13), 0x00000002, 0x1);
	halrf_wreg(rf, 0x81cc + (path << 8), 0x0000003f, 0x1b);
	halrf_wreg(rf, 0x802c, 0x00000fff, 0x021);
	halrf_wreg(rf, 0x8000, MASKDWORD, 0x00000709 | (1 << (4 + path)));
	isfail = _iqk_check_cal_8852c(rf, path, lok);
	halrf_wreg(rf, 0x8010, 0x000000ff, 0x00);

	halrf_wreg(rf, 0x5670 + (path << 13), 0x00000002, 0x0);
	halrf_wrf(rf, path, 0x11, 0x1f000, 0x06);
	halrf_wreg(rf, 0x5670 + (path << 13), 0x00000002, 0x1);
	halrf_wreg(rf, 0x81cc + (path << 8), 0x0000003f, 0x09);
	halrf_wreg(rf, 0x8000, MASKDWORD, 0x00000509 | (1 << (4 + path)));
	isfail = _iqk_check_cal_8852c(rf, path, lok);
	halrf_wreg(rf, 0x8010, 0x000000ff, 0x00);

	halrf_wreg(rf, 0x5670 + (path << 13), 0x00000002, 0x0);
	halrf_wrf(rf, path, 0x11, 0x1f000, 0x12);
	halrf_wreg(rf, 0x5670 + (path << 13), 0x00000002, 0x1);
	halrf_wreg(rf, 0x81cc + (path << 8), 0x0000003f, 0x1b);
	halrf_wreg(rf, 0x8000, MASKDWORD, 0x00000309 | (1 << (4 + path)));
	isfail = _iqk_check_cal_8852c(rf, path, lok);
	halrf_wreg(rf, 0x8010, 0x000000ff, 0x00);

	halrf_wreg(rf, 0x5670 + (path << 13), 0x00000002, 0x0);

	return isfail;

}

__iram_func__
static bool _iqk_lok_8852c(struct rf_info *rf, enum phl_phy_idx phy_idx,
			    u8 path)
{
	struct halrf_iqk_info *iqk_info = &rf->iqk;
	bool isfail;

	//RF_DBG(rf, DBG_RF_IQK, "[IQK]===>%s\n", __func__);

	switch (iqk_info->iqk_band[path]) {
	case BAND_ON_24G:
		isfail = _iqk_lok_2g_bygain_8852c(rf, phy_idx, path);
		break;
	case BAND_ON_5G:		
		isfail = _iqk_lok_5g_bygain_8852c(rf, phy_idx, path);
		break;
	case BAND_ON_6G:		
		isfail = _iqk_lok_6g_bygain_8852c(rf, phy_idx, path);
		break;
	default:
		break;
	}

	halrf_wrf(rf, path, 0xef, 0x00004, 0x0);
	isfail = _lok_finetune_check_8852c(rf, path);

	return isfail;
}

__iram_func__
static void _iqk_txk_setting_8852c(struct rf_info *rf, u8 path)
{
	struct halrf_iqk_info *iqk_info = &rf->iqk;

	//TX init gain setting
	/*0/1:G/A*/
	RF_DBG(rf, DBG_RF_IQK, "[IQK]===>%s\n", __func__);
	switch (iqk_info->iqk_band[path]) {
	case BAND_ON_24G:
		//03_G_S0_TxLOK_52C_reg
		halrf_wrf(rf, path, 0x51, 0x80000, 0x0);
		halrf_wrf(rf, path, 0x51, 0x00800, 0x0);
		halrf_wrf(rf, path, 0x52, 0x00800, 0x1);
		halrf_wrf(rf, path, 0x65, 0xf0000, 0xf);
		halrf_wrf(rf, path, 0x55, 0x0001f, 0x0);
		halrf_wrf(rf, path, 0xef, 0x00004, 0x1);
		halrf_wrf(rf, path, 0x00, 0xfffff, 0x403e0 | iqk_info->syn1to2 );
		halrf_delay_us(rf, 10);
		halrf_wrf(rf, path, 0x11, 0x00003, 0x0);
		halrf_wrf(rf, path, 0x11, 0x00070, 0x6);
		break;
	case BAND_ON_5G:
		//RF_DBG(rf, DBG_RF_IQK, "[IQK]===>%s\n", __func__);
		//03_A_S0_TxLOK_52C_reg
		halrf_wrf(rf, path, 0x64, 0x38000, 0x0);
		halrf_wrf(rf, path, 0x7f, 0x00080, 0x1);
		halrf_wrf(rf, path, 0x65, 0xf0000, 0xf);
		halrf_wrf(rf, path, 0x55, 0x0001f, 0x0);
		halrf_wrf(rf, path, 0xef, 0x00004, 0x1);
		halrf_wrf(rf, path, 0x00, 0xfffff, 0x403e0 | iqk_info->syn1to2);
		halrf_delay_us(rf, 10);
		halrf_wrf(rf, path, 0x11, 0x00003, 0x0);
		halrf_wrf(rf, path, 0x11, 0x00070, 0x6);
		break;
	case BAND_ON_6G:
		//RF_DBG(rf, DBG_RF_IQK, "[IQK]===>%s\n", __func__);
		//03_6g_S0_TxLOK_52C_reg
		halrf_wrf(rf, path, 0x64, 0x38000, 0x0);
		halrf_wrf(rf, path, 0x7f, 0x00080, 0x1);
		halrf_wrf(rf, path, 0x65, 0xf0000, 0xf);
		halrf_wrf(rf, path, 0x55, 0x0001f, 0x0);
		halrf_wrf(rf, path, 0xef, 0x00004, 0x1);
		halrf_wrf(rf, path, 0x00, 0xfffff, 0x403e0  | iqk_info->syn1to2);		
		halrf_delay_us(rf, 10);
		halrf_wrf(rf, path, 0x11, 0x00003, 0x0);
		halrf_wrf(rf, path, 0x11, 0x00070, 0x6);
		break;
	default:
		break;
	}

#ifndef HALRF_CONFIG_FW_IO_OFLD_SUPPORT	/*NOT USB*/
	RF_DBG(rf, DBG_RF_IQK, "[IQK]S%x, rf%x = 0x%x\n", path, path, halrf_rrf(rf, path, 0x00, MASKRF));
#endif

	return;
}

__iram_func__
static void _iqk_txclk_setting_8852c(struct rf_info *rf, u8 path)
{
#if 0
	//RF_DBG(rf, DBG_RF_IQK, "[IQK]===>%s\n", __func__);
	//04_CLK_Setting_TxIQK_DAC960MHz_nonDBCC_PHY0_path01_reg
	halrf_wreg(rf, 0x12b8, 0x40000000, 0x1);
	halrf_wreg(rf, 0x32b8, 0x40000000, 0x1);
	halrf_delay_us(rf, 1);
	halrf_wreg(rf, 0x030c, 0xff000000, 0x1f);
	halrf_delay_us(rf, 1);
	halrf_wreg(rf, 0x030c, 0xff000000, 0x13);
	halrf_wreg(rf, 0x032c, 0xffff0000, 0x0001);
	halrf_delay_us(rf, 1);
	halrf_wreg(rf, 0x032c, 0xffff0000, 0x0041);
#endif
	return;
}

__iram_func__
static void _iqk_info_iqk_8852c(struct rf_info *rf, enum phl_phy_idx phy_idx,
				 u8 path)
{

	struct halrf_iqk_info *iqk_info = &rf->iqk;
	u32 tmp = 0x0;
	bool flag = 0x0;

	//RF_DBG(rf, DBG_RF_IQK, "[IQK]===>%s\n", __func__);
	//iqk_info->thermal[path] = halrf_get_thermal_8852c(rf, path);	
	iqk_info->thermal_rek_en = false;
	flag = iqk_info->lok_cor_fail[0][path];
	halrf_wreg(rf, 0x9fe0, BIT(0) << (path * 4), flag);
	flag = iqk_info->lok_fin_fail[0][path];
	halrf_wreg(rf, 0x9fe0, BIT(1) << (path * 4), flag);
	flag = iqk_info->iqk_tx_fail[0][path];
	halrf_wreg(rf, 0x9fe0, BIT(2) << (path * 4), flag);
	flag = iqk_info->iqk_rx_fail[0][path];
	halrf_wreg(rf, 0x9fe0, BIT(3) << (path * 4), flag);

	tmp = halrf_rreg(rf, 0x8124 + (path << 8), MASKDWORD);
	iqk_info->bp_iqkenable[path] = tmp;
	tmp = halrf_rreg(rf, 0x8138 + (path << 8), MASKDWORD);
	iqk_info->bp_txkresult[path] = tmp;
	tmp = halrf_rreg(rf, 0x813c + (path << 8), MASKDWORD);
	iqk_info->bp_rxkresult[path] = tmp;

	halrf_wreg(rf, 0x9fe8, 0x0000ff00, (u8)iqk_info->iqk_times);

	tmp = halrf_rreg(rf, 0x9fe0, 0x0000000f << (path * 4));
	if (tmp != 0x0) {
		iqk_info->iqk_fail_cnt++;
		//RF_WARNING("S%x, IQK Fail 0x9fe0 = %x\n", path, tmp);
	}
	
	halrf_wreg(rf, 0x9fe8, 0x00ff0000 << (path * 4), iqk_info->iqk_fail_cnt);

	return;

}

__iram_func__
static void _iqk_disable_rxagc_8852c(struct rf_info *rf,u8 path, u8 en_rxgac)
{
	RF_DBG(rf, DBG_RF_IQK, "===> %s\n", __func__);
	if(path == RF_PATH_A)
		halrf_wreg(rf, 0x4730, BIT(31), en_rxgac);
	else
		halrf_wreg(rf, 0x4a9c, BIT(31), en_rxgac);
}

__iram_func__
static void _iqk_by_path_8852c(struct rf_info *rf, enum phl_phy_idx phy_idx,
				u8 path)
{
	struct halrf_iqk_info *iqk_info = &rf->iqk;
	bool lok_is_fail = false;
	u8 i = 0x0;
	u8 ibias = 0;
	u8 thermal = 0;
	bool nbiqk_en = false;
	u32 afe_rst_en = 0;
	
	nbiqk_en = iqk_info->is_nbiqk;
	thermal = halrf_get_thermal_8852c(rf, path);
	afe_rst_en = halrf_rreg(rf, 0xc0e8 + (path << 8), BIT(6));
	if(thermal < 0x14){
		iqk_info->is_nbiqk = true;
		halrf_wreg(rf, 0xc0e8 + (path << 8), BIT(6), 0x1);
		RF_DBG(rf, DBG_RF_IQK, "[IQK]thermal = %x, nbiqk enable\n", thermal);
	} else {
		RF_DBG(rf, DBG_RF_IQK, "[IQK]thermal = %x\n", thermal);
	}
#if 1
	//LOK
	if (iqk_info->iqk_band[path] == BAND_ON_24G)
		ibias = 0x2;
	else
		ibias = 0x3;

	for (i = 0; i < 3; i++) {		
		_lok_res_table_8852c(rf, path, ibias + i*2);
		_iqk_txk_setting_8852c(rf, path);
		lok_is_fail = _iqk_lok_8852c(rf, phy_idx, path);
		if (!lok_is_fail)			
			break;
	}
	
	//TXK	
	if (iqk_info->is_nbiqk) {
		iqk_info->iqk_tx_fail[0][path] =
			_iqk_nbtxk_8852c(rf, phy_idx, path);
	} else {
		iqk_info->iqk_tx_fail[0][path] =
			_txk_group_sel_8852c(rf, phy_idx, path);
	}
	//RX
	_iqk_rxk_setting_8852c(rf, path);
	if (iqk_info->is_nbiqk) {
		iqk_info->iqk_rx_fail[0][path] =
			_iqk_nbrxk_8852c(rf, phy_idx, path);
	} else {
		iqk_info->iqk_rx_fail[0][path] =
			_rxk_group_sel_8852c(rf, phy_idx, path);
	}
#endif
#ifndef HALRF_CONFIG_FW_IO_OFLD_SUPPORT /*NOT USB*/
	_iqk_info_iqk_8852c(rf, phy_idx, path);
#endif
	if(iqk_info->is_nbiqk) {
		halrf_wreg(rf, 0x8138 + (path << 8), MASKDWORD, iqk_info->nb_txcfir[path]);
		halrf_wreg(rf, 0x813c + (path << 8), MASKDWORD, iqk_info->nb_rxcfir[path]);
	} else {
		halrf_wreg(rf, 0x8138 + (path << 8), MASKDWORD, 0x40000000);
		halrf_wreg(rf, 0x813c + (path << 8), MASKDWORD, 0x40000000);
	}
	iqk_info->is_nbiqk = nbiqk_en;
	
	halrf_wreg(rf, 0xc0e8 + (path << 8), BIT(6), afe_rst_en);

	return;
}

__iram_func__
void iqk_backup_rf0_8852c(
	struct rf_info *rf, u8 path,
	u32 backup_rf0[rf_reg_num_8852c],
	u32 backup_rf_reg0[rf_reg_num_8852c])
{
	u8 i;

	if(path != RF_PATH_A)
		return;
	RF_DBG(rf, DBG_RF_IQK, "[IQK]===>%s\n", __func__);
	for (i = 0; i < rf_reg_num_8852c; i++) {
		backup_rf0[i] = halrf_rrf(rf, RF_PATH_A, backup_rf_reg0[i], MASKRF);		
		//RF_DBG(rf, DBG_RF_IQK, "[IQK]bk rf0, %x = %x\n", backup_rf_reg0[i], backup_rf0[i]);
	}	
	return;
}

__iram_func__
void iqk_backup_rf1_8852c(
	struct rf_info *rf, u8 path,
	u32 backup_rf1[rf_reg_num_8852c],
	u32 backup_rf_reg1[rf_reg_num_8852c])
{
	u8 i;

	if(path != RF_PATH_B)
		return;

	//DBG_LOG_SERIOUS(DBGMSG_RF, DBG_WARNING, "[IQK] 06 \n");
	for (i = 0; i < rf_reg_num_8852c; i++) {
		backup_rf1[i] = halrf_rrf(rf, RF_PATH_B, backup_rf_reg1[i], MASKRF);		
		//RF_DBG(rf, DBG_RF_IQK, "[IQK]bk rf1, %x = %x\n", backup_rf_reg1[i], backup_rf1[i]);
	}	
	return;
}

__iram_func__
void iqk_restore_rf0_8852c(
	struct rf_info *rf, u8 path,
	u32 backup_rf0[rf_reg_num_8852c],
	u32 backup_rf_reg0[rf_reg_num_8852c])
{
	u32 i = 0;
	
	if(path != RF_PATH_A)
		return;

	RF_DBG(rf, DBG_RF_IQK, "[IQK]===>%s\n", __func__);	
	//DBG_LOG_SERIOUS(DBGMSG_RF, DBG_WARNING, "[IQK] 13 \n");
	for (i = 0; i < rf_reg_num_8852c; i++) {
		halrf_wrf(rf, RF_PATH_A, backup_rf_reg0[i], MASKRF, backup_rf0[i]);
		//RF_DBG(rf, DBG_RF_IQK, "[IQK]restore rf0, 0x%x = 0x%x\n", backup_rf_reg0[i], halrf_rrf(rf, 0x0, backup_rf_reg0[i], MASKRF));
	}
	return;
}

__iram_func__
void iqk_restore_rf1_8852c(
	struct rf_info *rf, u8 path,
	u32 backup_rf1[rf_reg_num_8852c],
	u32 backup_rf_reg1[rf_reg_num_8852c])
{
	u32 i;
	
	if(path != RF_PATH_B)
		return;

	RF_DBG(rf, DBG_RF_IQK, "[IQK]===>%s\n", __func__);
	for (i = 0; i < rf_reg_num_8852c; i++) {
		halrf_wrf(rf, RF_PATH_B, backup_rf_reg1[i], MASKRF, backup_rf1[i]);		
		//RF_DBG(rf, DBG_RF_IQK, "[IQK]restore rf S%d = %x, value = %x\n", path, backup_rf_reg[path][i], halrf_rrf(rf, path, backup_rf_reg[path][i], MASKRF));
	}
	return;
}


__iram_func__
void iqk_set_info_8852c(struct rf_info *rf, enum phl_phy_idx phy_idx,
				 u8 path)
{

	struct halrf_iqk_info *iqk_info = &rf->iqk;
	u32 tmp = 0x0;
	bool flag = 0x0;

	//RF_DBG(rf, DBG_RF_IQK, "[IQK]===>%s\n", __func__);
	iqk_info->thermal[path] = halrf_get_thermal_8852c(rf, path);	
	iqk_info->thermal_rek_en = false;

	flag = (bool) halrf_rreg(rf, 0x9fe0, BIT(0) << (path * 4));
	iqk_info->lok_cor_fail[0][path] = flag;

	flag = (bool) halrf_rreg(rf, 0x9fe0, BIT(1) << (path * 4));
	iqk_info->lok_fin_fail[0][path] = flag;

	flag = (bool) halrf_rreg(rf, 0x9fe0, BIT(2) << (path * 4));
	iqk_info->iqk_tx_fail[0][path] = flag;

	flag = (bool) halrf_rreg(rf, 0x9fe0, BIT(3) << (path * 4));
	iqk_info->iqk_rx_fail[0][path] = flag;
	
	tmp = halrf_rreg(rf, 0x8124 + (path << 8), MASKDWORD);
	iqk_info->bp_iqkenable[path] = tmp;

	tmp = halrf_rreg(rf, 0x8138 + (path << 8), MASKDWORD);
	iqk_info->bp_txkresult[path] = tmp;

	tmp = halrf_rreg(rf, 0x813c + (path << 8), MASKDWORD);
	iqk_info->bp_rxkresult[path] = tmp;

	iqk_info->iqk_times = (u8) halrf_rreg(rf, 0x9fe8, 0x0000ff00);
	iqk_info->iqk_fail_cnt = halrf_rreg(rf, 0x9fe8, 0x00ff0000 << (path * 4));
	return;
}


__iram_func__
bool iqk_mcc_page_sel_8852c(struct rf_info *rf, enum phl_phy_idx phy, u8 path)
{
#if 0
	struct halrf_iqk_info *iqk_info = &rf->iqk;
	bool flag = false;

	if (rf->hal_com->band[phy].cur_chandef.center_ch ==  iqk_info->iqk_mcc_ch[0][path]) {
		halrf_wreg(rf, 0x8104 + (path << 8), 0x00000001, 0x0);
		halrf_wreg(rf, 0x8154 + (path << 8), 0x00000004, 0x0);
		_iqk_mcc_load_lok_8852c(rf, phy, path, iqk_info->lok_idac[0][path]);
		flag = true;
	} else if (rf->hal_com->band[phy].cur_chandef.center_ch == iqk_info->iqk_mcc_ch[1][path]) {
		halrf_wreg(rf, 0x8104 + (path << 8), 0x00000001, 0x1);
		halrf_wreg(rf, 0x8154 + (path << 8), 0x00000004, 0x1);
		_iqk_mcc_load_lok_8852c(rf, phy, path, iqk_info->lok_idac[1][path]);
		flag = true;
	} else
		flag = false;
#endif	
	return false;
}

__iram_func__
void iqk_get_ch_info_8852c(struct rf_info *rf, enum phl_phy_idx phy, u8 path)
{
	struct halrf_iqk_info *iqk_info = &rf->iqk;
	struct halrf_mcc_info *mcc_info = &rf->mcc_info;
	u32 reg_rf18 = 0x0;
	u8 ver;
	u8 idx = 0;

	idx = mcc_info->table_idx;
	//RF_DBG(rf, DBG_RF_IQK, "[IQK]===>%s\n", __func__);
	RF_DBG(rf, DBG_RF_IQK, "[IQK]Test Ver 0x%x\n", 0x13);
	reg_rf18 = halrf_rrf(rf, path, 0x18, MASKRF);
	iqk_info->iqk_band[path] = rf->hal_com->band[phy].cur_chandef.band;
	iqk_info->iqk_bw[path] = rf->hal_com->band[phy].cur_chandef.bw;
	iqk_info->iqk_ch[path] = rf->hal_com->band[phy].cur_chandef.center_ch;	
	iqk_info->iqk_mcc_ch[idx][path] = rf->hal_com->band[phy].cur_chandef.center_ch;
	iqk_info->iqk_table_idx[path] =  idx;
	if (!rf->hal_com->dbcc_en)
		iqk_info->syn1to2 = 0x1;
	else
		iqk_info->syn1to2 = 0x3;

	RF_DBG(rf, DBG_RF_IQK, "[IQK]S%d (PHY%d): / DBCC %s/ %s/ CH%d/ %s\n",
		   path, phy,  rf->hal_com->dbcc_en ? "on" : "off",
		   iqk_info->iqk_band[path]  == 0 ? "2G" : (iqk_info->iqk_band[path]  == 1 ? "5G" : "6G"),
		   iqk_info->iqk_ch[path] ,
		   iqk_info->iqk_bw[path] == 0 ? "20M" : (iqk_info->iqk_bw[path] == 1 ? "40M" : "80M"));	
	RF_DBG(rf, DBG_RF_IQK, "[IQK]times = 0x%x, ch =%x\n", iqk_info->iqk_times , idx);	
	RF_DBG(rf, DBG_RF_IQK, "[IQK]iqk_mcc_ch[%x][%x] = 0x%x\n",  (u8)idx, (u8)path, iqk_info->iqk_mcc_ch[idx][path]);
	RF_DBG(rf, DBG_RF_IQK, "[IQK]S%x, 0x18= 0x%x, idx = %x, iqk_info->syn1to2 =%x\n", path, reg_rf18, idx, iqk_info->syn1to2);
	RF_DBG(rf, DBG_RF_IQK, "[IQK]S%x, thermal = %x\n", path, halrf_get_thermal_8852c(rf, path));

	halrf_wreg(rf, 0x9fe0, 0xff000000, iqk_version_8852c);
	//2G5G6G = 0/1/2
	halrf_wreg(rf, 0x9fe4, 0x000f << (path * 16), (u8)iqk_info->iqk_band[path]);
	//20/40/80 = 0/1/2
	halrf_wreg(rf, 0x9fe4, 0x00f0 << (path * 16), (u8)iqk_info->iqk_bw[path]);
	halrf_wreg(rf, 0x9fe4, 0xff00 << (path * 16), (u8)iqk_info->iqk_ch[path]); 

	ver = (u8) halrf_get_8852c_nctl_reg_ver();
	halrf_wreg(rf, 0x9fe8, 0x000000ff, ver);
	
	return;
}

__iram_func__
void halrf_iqk_reload_8852c(struct rf_info *rf, u8 path)
{
	struct halrf_iqk_info *iqk_info = &rf->iqk;
	u32 tmp;
	//RF_DBG(rf, DBG_RF_IQK, "[IQK]===>%s\n", __func__);
	iqk_info->reload_cnt++;
	tmp = iqk_info->bp_iqkenable[path];
	halrf_wreg(rf, 0x8124 + (path << 8), MASKDWORD, tmp);
	tmp = iqk_info->bp_txkresult[path];
	halrf_wreg(rf, 0x8138 + (path << 8), MASKDWORD, tmp);
	tmp = iqk_info->bp_rxkresult[path];
	halrf_wreg(rf, 0x813c + (path << 8), MASKDWORD, tmp);
	return;
}

__iram_func__
void iqk_start_iqk_8852c(struct rf_info *rf, enum phl_phy_idx phy_idx, u8 path)
{

	//RF_DBG(rf, DBG_RF_IQK, "[IQK]===>%s\n", __func__);
	_iqk_by_path_8852c(rf, phy_idx, path);
	return;
}

__iram_func__
void iqk_restore_8852c(struct rf_info *rf, u8 path)
{
//	struct halrf_iqk_info *iqk_info = &rf->iqk;
	bool fail;
	//98_S0_IQK_Reg_Restore_52C_reg
	halrf_wreg(rf, 0x8000, MASKDWORD, 0x00001219 + (path << 4));
	halrf_delay_us(rf, 200);
	fail = _iqk_check_cal_8852c(rf, path, 0x12);
	
	halrf_wreg(rf, 0x8010, 0x000000ff, 0x00);
	halrf_wreg(rf, 0x8008, MASKDWORD, 0x00000000);
	halrf_wreg(rf, 0x8088, MASKDWORD, 0x80000000);
	
	halrf_wrf(rf, path, 0xef, 0x00004, 0x0);
	halrf_wrf(rf, path, 0x00, 0xf0000, 0x3);
	halrf_wrf(rf, path, 0x05, 0x00001, 0x1);

	return;	
}

__iram_func__
void iqk_afebb_restore_8852c(struct rf_info *rf, enum phl_phy_idx phy_idx, u8 path)
{
	RF_DBG(rf, DBG_RF_IQK, "===> %s\n", __func__);

	switch(path) {
		case RF_PATH_A:	
			//99a_restore_iqk_kmod_bb_path0_KS			
			halrf_wreg(rf, 0x12b8, 0x40000000, 0x0);
			halrf_wreg(rf, 0x20fc, 0x00010000, 0x1);
			halrf_wreg(rf, 0x20fc, 0x00100000, 0x0);
			halrf_wreg(rf, 0x20fc, 0x01000000, 0x1);
			halrf_wreg(rf, 0x20fc, 0x10000000, 0x0);
			halrf_wreg(rf, 0x5670, MASKDWORD, 0x00000000);
			halrf_wreg(rf, 0x12a0, 0x000ff000, 0x00);
			halrf_wreg(rf, 0x20fc, 0x00010000, 0x0);
			halrf_wreg(rf, 0x20fc, 0x01000000, 0x0);
			//99b_restore_iqk_kmod_DRFC_path0			
			halrf_wrf(rf, RF_PATH_A, 0x10005, 0x00001, 0x1);
		break;
		case RF_PATH_B:			
			//99a_restore_iqk_kmod_bb_path1_KS_reg
			halrf_wreg(rf, 0x32b8, 0x40000000, 0x0);
			halrf_wreg(rf, 0x20fc, 0x00020000, 0x1);
			halrf_wreg(rf, 0x20fc, 0x00200000, 0x0);
			halrf_wreg(rf, 0x20fc, 0x02000000, 0x1);
			halrf_wreg(rf, 0x20fc, 0x20000000, 0x0);
			halrf_wreg(rf, 0x7670, MASKDWORD, 0x00000000);
			halrf_wreg(rf, 0x32a0, 0x000ff000, 0x00);
			halrf_wreg(rf, 0x20fc, 0x00020000, 0x0);
			halrf_wreg(rf, 0x20fc, 0x02000000, 0x0);
			//99b_restore_iqk_kmod_DRFC_path1
			halrf_wrf(rf, RF_PATH_B, 0x10005, 0x00001, 0x1);
		break;
		default:			
			break;
	}
	
	//enable rxgac
	_iqk_disable_rxagc_8852c(rf, path, 0x1);

	return;
}

__iram_func__
void iqk_preset_8852c(struct rf_info *rf, u8 path)
{
	struct halrf_iqk_info *iqk_info = &rf->iqk;
	u8 idx = 0;
	
	//ch = ((iqk_info->iqk_times /2) % 2) & 0x1;
	RF_DBG(rf, DBG_RF_IQK, "[IQK]===>%s\n", __func__);
	//02_IQK_Preset_path0_20210804
	//02_IQK_Preset_path1_20210804

	idx = iqk_info->iqk_table_idx[path];	
	//idx = 0;
	
	halrf_wreg(rf, 0x8104 + (path << 8), 0x00000001, idx);
	halrf_wreg(rf, 0x8154 + (path << 8), 0x00000008, idx);

	if (iqk_info->iqk_band[path] == BAND_ON_6G) { 
		if(idx == 0x0)
			halrf_wreg(rf, 0x810c + (path << 8), MASKDWORD, 0x11111111);
		else
			halrf_wreg(rf, 0x810c + (path << 8), MASKDWORD, 0x11111111);
	} else {
		if(idx == 0x0)
			halrf_wreg(rf, 0x810c + (path << 8), MASKDWORD, 0x33112211);
		else
			halrf_wreg(rf, 0x810c + (path << 8), MASKDWORD, 0x33112211);
	}
	halrf_wrf(rf, path, 0x5, 0x00001, 0x0);
	//halrf_wrf(rf, path, 0x10005, 0x00001, 0x0);
	halrf_wreg(rf, 0x8008, MASKDWORD, 0x00000080);
	halrf_wreg(rf, 0x8088, MASKDWORD, 0x81ff010a);


#ifndef HALRF_CONFIG_FW_IO_OFLD_SUPPORT	/*NOT USB*/
	RF_DBG(rf, DBG_RF_IQK, "[IQK](1)S%x, 0x8%x04 = 0x%x\n", path, 1 << path, halrf_rreg(rf, 0x8104 + (path << 8), MASKDWORD));
	RF_DBG(rf, DBG_RF_IQK, "[IQK](1)S%x, 0x8%x54 = 0x%x\n", path, 1 << path, halrf_rreg(rf, 0x8154 + (path << 8), MASKDWORD));
	RF_DBG(rf, DBG_RF_IQK, "[IQK](1)S%x, 0x8%x0c = 0x%x\n", path, 1 << path, halrf_rreg(rf, 0x810c + (path << 8), MASKDWORD));
	RF_DBG(rf, DBG_RF_IQK, "[IQK](1)S%x, 0xc%xe8 = 0x%x\n", path, path, halrf_rreg(rf, 0xc0e8 + (path << 8), MASKDWORD));
#endif
	return;
}

__iram_func__
void iqk_macbb_setting_8852c(struct rf_info *rf, enum phl_phy_idx phy_idx, u8 path)
{
	RF_DBG(rf, DBG_RF_IQK, "[IQK]===> %s\n", __func__);

	//01_BB_AFE_forDPK_S0_20210820
	halrf_wrf(rf, path, 0x10005, 0x00001, 0x0);
	halrf_wreg(rf, 0x20fc, BIT(16) << path, 0x1);
	halrf_wreg(rf, 0x20fc, BIT(20) << path, 0x0);
	halrf_wreg(rf, 0x20fc, BIT(24) << path, 0x1);
	halrf_wreg(rf, 0x20fc, BIT(28) << path, 0x0);
	//disable rxgac
	_iqk_disable_rxagc_8852c(rf, path, 0x0);
	halrf_wreg(rf, 0x5670 | (path << 13), MASKDWORD, 0xf801fffd);
	halrf_wreg(rf, 0x5670 | (path << 13), 0x00004000, 0x1);
	halrf_wreg(rf, 0x5670 | (path << 13), 0x80000000, 0x1);
	halrf_txck_force_8852c(rf, path, true, DAC_960M);
	halrf_wreg(rf, 0x5670 | (path << 13), 0x00002000, 0x1);
	halrf_rxck_force_8852c(rf, path, true, ADC_1920M);
	halrf_wreg(rf, 0x5670 | (path << 13), 0x60000000, 0x2);
	halrf_wreg(rf, 0xc0d4 | (path << 8), 0x0c000000, 0x1);
	halrf_wreg(rf, 0xc0d8 | (path << 8), 0x000001e0, 0xb);
	halrf_wreg(rf, 0x12b8 | (path << 13), 0x40000000, 0x1);
	halrf_wreg(rf, 0x030c, 0xff000000, 0x1f);
	halrf_wreg(rf, 0x030c, 0xff000000, 0x13);
	halrf_wreg(rf, 0x032c, 0xffff0000, 0x0001);
	halrf_wreg(rf, 0x032c, 0xffff0000, 0x0041);
	halrf_wreg(rf, 0x20fc, BIT(20) << path, 0x1);
	halrf_wreg(rf, 0x20fc, BIT(28) << path, 0x1);

	return;
}

__iram_func__
void halrf_iqk_toneleakage_8852c(struct rf_info *rf, u8 path)
{
	struct halrf_iqk_info *iqk_info = &rf->iqk;
	u32 a6_power_range[3] = {0x0, 0x0, 0x0};
	u32 a6_track_range[3] = {0x6, 0x7, 0x7};
	u32 a6_gain_bb[3] = {0x12, 0x09, 0x0e};
	u32 a6_itqt[3] = {0x12, 0x12, 0x12};

	u32 a_power_range[3] = {0x0, 0x0, 0x0};
	u32 a_track_range[3] = {0x5, 0x6, 0x7};
	u32 a_gain_bb[3] = {0x12, 0x09, 0x0e};
	u32 a_itqt[3] = {0x12, 0x12, 0x12};
	
	u32 g_power_range[3] = {0x0, 0x0, 0x0};
	u32 g_track_range[3] = {0x5, 0x6, 0x6};
	u32 g_gain_bb[3] = {0x0e, 0x0a, 0x0e};
	u32 g_itqt[3] = { 0x12, 0x12, 0x12};
	u8 gp = 0x0;

	//RF_DBG(rf, DBG_RF_IQK, "[IQK]===>%s\n", __func__);		

	iqk_get_ch_info_8852c(rf, HW_PHY_0, path);
	iqk_macbb_setting_8852c(rf, HW_PHY_0, path);	
	iqk_preset_8852c(rf, path);

	gp = 0x1;
	switch (iqk_info->iqk_band[path]) {
	case BAND_ON_24G:
		halrf_wrf(rf, path, 0x11, 0x00003, g_power_range[gp]);
		halrf_wrf(rf, path, 0x11, 0x00070, g_track_range[gp]);
		halrf_wrf(rf, path, 0x11, 0x1f000, g_gain_bb[gp]);
		halrf_wreg(rf, 0x81cc + (path << 8), MASKDWORD, g_itqt[gp]);
		break;
	case BAND_ON_5G:
		halrf_wrf(rf, path, 0x11, 0x00003, a_power_range[gp]);
		halrf_wrf(rf, path, 0x11, 0x00070, a_track_range[gp]);
		halrf_wrf(rf, path, 0x11, 0x1f000, a_gain_bb[gp]);
		halrf_wreg(rf, 0x81cc + (path << 8), MASKDWORD, a_itqt[gp]);
		break;
	case BAND_ON_6G:
		halrf_wrf(rf, path, 0x11, 0x00003, a6_power_range[gp]);
		halrf_wrf(rf, path, 0x11, 0x00070, a6_track_range[gp]);
		halrf_wrf(rf, path, 0x11, 0x1f000, a6_gain_bb[gp]);
		halrf_wreg(rf, 0x81cc + (path << 8), MASKDWORD, a6_itqt[gp]);
	break;
	default:
		break;
	}

	halrf_wrf(rf, path, 0x0, 0xf0000, 0x2);
	halrf_wreg(rf, 0x8074, 0xff000000, 0x80);
	halrf_wreg(rf, 0x80d0, MASKDWORD, 0x00300000);
	halrf_wreg(rf, 0x8120  + (path << 8), MASKDWORD, 0xc2000a08);
	halrf_wreg(rf, 0x8000, 0x00000006, path);
	halrf_wrf(rf, path, 0x10001, 0x0003f, 0x37);
	halrf_wreg(rf, 0x8034, 0x00000030, 0x2);
	halrf_wreg(rf, 0x8038, 0x00000100, 0x1);
	halrf_wreg(rf, 0x8034, 0xff000000, 0x11);
	halrf_wreg(rf, 0x5670 |path << 13, 0x00000002, 0x1);
	halrf_wreg(rf, 0x8014, 0x10000000, 0x1);
	halrf_wreg(rf, 0x8014, 0x10000000, 0x0);
	halrf_delay_us(rf, 100);
	halrf_wreg(rf, 0x5670 |path << 13, 0x00000002, 0x0);
	halrf_wreg(rf, 0x8018, 0x70000000, 0x2);
	halrf_wreg(rf, 0x802c, 0x00000fff, 0x00b);
	halrf_wreg(rf, 0x8034, 0x00000001, 0x1);
	halrf_wreg(rf, 0x8034, 0x00000001, 0x0);

	return;
}

__iram_func__
void halrf_iqk_dbcc_8852c(struct rf_info *rf, u8 path)
{
#if 0
	bool bkdbcc = false;	
	u8 phy_idx = 0x0;
	
	bkdbcc = rf->hal_com->dbcc_en;
	rf->hal_com->dbcc_en = true;

	if (path == 0x0)
		phy_idx = HW_PHY_0;
	else
		phy_idx = HW_PHY_1;

	//iqk_mcc_page_sel_8852c(rf,phy_idx, path);		
	iqk_get_ch_info_8852c(rf,phy_idx, path);
	iqk_macbb_setting_8852c(rf, phy_idx, path);
	iqk_preset_8852c(rf, path);
	iqk_start_iqk_8852c(rf, phy_idx, path);
	iqk_restore_8852c(rf, path);
	iqk_afebb_restore_8852c(rf, phy_idx, path);	
	rf->hal_com->dbcc_en = bkdbcc;
#endif
	return;
}

__iram_func__
void halrf_iqk_onoff_8852c(struct rf_info *rf, bool is_enable)
{
	struct halrf_iqk_info *iqk_info = &rf->iqk;

	if (is_enable) {
		halrf_wreg(rf, 0x8124, 0xffffffff, 0x01010101); /*ch0*/
		halrf_wreg(rf, 0x8224, 0xffffffff, 0x01010101); /*ch1*/
		iqk_info->is_iqk_enable = true;
	} else {
		halrf_wreg(rf, 0x8124, 0x0000ffff, 0x00000000); /*ch0*/
		halrf_wreg(rf, 0x8224, 0x0000ffff, 0x00000000); /*ch1*/
		iqk_info->is_iqk_enable = false;
	}
	RF_DBG(rf, DBG_RF_IQK, "[IQK] IQK enable :  %s !!!\n",
	       iqk_info->is_iqk_enable ? "enable" : "disable");
	return;
}

__iram_func__
void halrf_iqk_tx_bypass_8852c(struct rf_info *rf, u8 path)
{
	if (path == RF_PATH_A) { //path A
		/*ch0*/
		halrf_wreg(rf, 0x8124, MASKDWORD, 0x0);
		halrf_wreg(rf, 0x8138, MASKDWORD, 0x40000002);
	} else {
		/*ch1*/
		halrf_wreg(rf, 0x8224, MASKDWORD, 0x0);
		halrf_wreg(rf, 0x8238, MASKDWORD, 0x40000002);
	}
	return;
}

__iram_func__
void halrf_iqk_rx_bypass_8852c(struct rf_info *rf, u8 path)
{
	if (path == RF_PATH_A) { //path A
		/*ch0*/
		halrf_wreg(rf, 0x8124, MASKDWORD, 0x0);
		halrf_wreg(rf, 0x813c, MASKDWORD, 0x40000002);
	} else {
		/*ch1*/
		halrf_wreg(rf, 0x8224, MASKDWORD, 0x0);
		halrf_wreg(rf, 0x823c, MASKDWORD, 0x40000002);
	}
	return;
}

__iram_func__
void halrf_iqk_lok_bypass_8852c(struct rf_info *rf, u8 path)
{
	halrf_wrf(rf, path, 0xdf, 0x00004, 0x1);
	halrf_wrf(rf, path, 0x58, MASKRF, 0x84220);
	return;
}

__iram_func__
void halrf_nbiqk_enable_8852c(struct rf_info *rf, bool nbiqk_en)
{
	struct halrf_iqk_info *iqk_info = &rf->iqk;

	if (nbiqk_en) {
		iqk_info->is_nbiqk = true;
	} else {
		iqk_info->is_nbiqk = false;
	}

	return;
}

__iram_func__
void halrf_iqk_xym_enable_8852c(struct rf_info *rf, bool iqk_xym_en)
{
	struct halrf_iqk_info *iqk_info = &rf->iqk;

	if (iqk_xym_en) {
		iqk_info->iqk_xym_en = true;
	} else {
		iqk_info->iqk_xym_en = false;
	}

	return;
}

__iram_func__
void halrf_iqk_fft_enable_8852c(struct rf_info *rf, bool iqk_fft_en)
{
	struct halrf_iqk_info *iqk_info = &rf->iqk;

	if (iqk_fft_en) {
		iqk_info->iqk_fft_en = true;
	} else {
		iqk_info->iqk_fft_en = false;
	}

	return;
}

__iram_func__
void halrf_iqk_sram_enable_8852c(struct rf_info *rf, bool iqk_sram_en)
{
	struct halrf_iqk_info *iqk_info = &rf->iqk;

	if (iqk_sram_en) {
		iqk_info->iqk_sram_en = true;
	} else {
		iqk_info->iqk_sram_en = false;
	}

	return;
}

__iram_func__
void halrf_iqk_cfir_enable_8852c(struct rf_info *rf, bool iqk_cfir_en)
{
	struct halrf_iqk_info *iqk_info = &rf->iqk;

	if (iqk_cfir_en) {
		iqk_info->iqk_cfir_en = true;
	} else {
		iqk_info->iqk_cfir_en = false;
	}
	return;
}

__iram_func__
void halrf_iqk_track_8852c(
	struct rf_info *rf)
{
	struct halrf_iqk_info *iqk_info = &rf->iqk;
	u8 path =0x0, i = 0x0;
	u8 cur_ther = 0x0, ther_avg_cnt = 0;
	u32 ther_avg[2] = {0};

	/*only check path 0 */
	for (path = 0; path < 1; path++) {
		cur_ther = halrf_get_thermal_8852c(rf, path);
		iqk_info->ther_avg[path][iqk_info->ther_avg_idx] = cur_ther;		
		/*Average times */
		ther_avg_cnt = 0;
		for (i = 0; i < 0x4; i++) {
			if (iqk_info->ther_avg[path][i]) {
				ther_avg[path] += iqk_info->ther_avg[path][i];
				ther_avg_cnt++;
#if 0
				RF_DBG(rf, DBG_RF_IQK,
						 "[IQK] thermal avg[%d] = %d\n", i,
						   iqk_info->ther_avg[path][i]);
#endif
			}
		}
		/*Calculate Average ThermalValue after average enough times*/
		if (ther_avg_cnt) {
			cur_ther = (u8)(ther_avg[path] / ther_avg_cnt);
		}
		if (HALRF_ABS(cur_ther, iqk_info->thermal[path] ) > IQK_THR_ReK) {
			iqk_info->thermal_rek_en = true;
		}
		else {
			iqk_info->thermal_rek_en = false;
		}
		//RF_DBG(rf, DBG_RF_IQK, "[IQK]S%x, iqk_ther =%d, ther_now = %d\n", path, iqk->thermal[path], cur_ther);
	}
	iqk_info->ther_avg_idx++;
	if (iqk_info->ther_avg_idx == 0x4)
		iqk_info->ther_avg_idx = 0;
	return;
}

__iram_func__
bool halrf_iqk_get_ther_rek_8852c(struct rf_info *rf ) {
	struct halrf_iqk_info *iqk_info = &rf->iqk;

	return iqk_info->thermal_rek_en;
}

__iram_func__
u8 halrf_iqk_get_mcc_ch0_8852c(struct rf_info *rf ) {
	
	struct halrf_iqk_info *iqk_info = &rf->iqk;

	return iqk_info->iqk_mcc_ch[0][0];
}

__iram_func__
u8 halrf_iqk_get_mcc_ch1_8852c(struct rf_info *rf ) {
	
	struct halrf_iqk_info *iqk_info = &rf->iqk;

	return iqk_info->iqk_mcc_ch[1][0];
}

__iram_func__
bool halrf_check_fwiqk_done_8852c(struct rf_info *rf)
{
	u32 counter = 0x0;
	bool flag = false;
	bool isfail = false;
#if 1
	while (1) {
		if (halrf_rreg(rf, 0xbff8, MASKBYTE0) == 0xaa  || counter > 3000) {
			if(halrf_rreg(rf, 0x8010, MASKBYTE0) == 0xaa) {
				flag = true;
				break;
			}
		}
		counter++;
		halrf_delay_us(rf, 10);
	};
#else
	for(counter = 0; counter < 6000; counter++)
		halrf_delay_us(rf, 10);
#endif
	if (counter < 10)
		isfail = true; 
	else
		isfail = false; 
		
	if(flag) {
		RF_DBG(rf, DBG_RF_IQK, "[IQK] Load FW Done, counter = %d!!\n", counter);
	} else {
		RF_DBG(rf, DBG_RF_IQK, "[IQK] Load FW Fail, counter = %d!!\n", counter);	
		halrf_iqk_tx_bypass_8852c(rf, RF_PATH_A);
		halrf_iqk_tx_bypass_8852c(rf, RF_PATH_B);
		halrf_iqk_rx_bypass_8852c(rf, RF_PATH_A);
		halrf_iqk_rx_bypass_8852c(rf, RF_PATH_B);
		halrf_iqk_lok_bypass_8852c(rf, RF_PATH_A);		
		halrf_iqk_lok_bypass_8852c(rf, RF_PATH_B);
	}
	halrf_wreg(rf, 0x8010, 0x000000ff,0x0);
	return isfail;
}

__iram_func__
void halrf_enable_fw_iqk_8852c(struct rf_info *rf, bool is_fw_iqk)
{
	struct halrf_iqk_info *iqk_info = &rf->iqk;

	if (is_fw_iqk) {
		iqk_info->is_fw_iqk = true;
	} else {
		iqk_info->is_fw_iqk = false;
	}
	return;
}

__iram_func__ 
u8 halrf_get_iqk_times_8852c(struct rf_info *rf) {
	u8 times  = 0x0;

	times = (u8) halrf_rreg(rf, 0x9fe8, 0x0000ff00);
	return times;
}

__iram_func__ 
u32 halrf_get_iqk_ver_8852c(void)
{
	return iqk_version_8852c;
}

__iram_func__
void iqk_init_8852c(struct rf_info *rf)
{
	struct halrf_iqk_info *iqk_info = &rf->iqk;
	u8 idx, path;
	halrf_wreg(rf, 0x9fe0, MASKDWORD, 0x0);	
	if (!iqk_info->is_iqk_init) {
		RF_DBG(rf, DBG_RF_IQK, "[IQK]===>%s\n", __func__);
		iqk_info->is_iqk_init = true;
		iqk_info->is_nbiqk = false;
		iqk_info->iqk_fft_en = false;
		iqk_info->iqk_sram_en = false;
		iqk_info->iqk_cfir_en = false;
		iqk_info->iqk_xym_en = false;
		iqk_info->thermal_rek_en = false;
		iqk_info->ther_avg_idx = 0x0;
		iqk_info->iqk_times = 0x0;
		/*channel/path/TRX(TX:0, RX:1) */
		iqk_info->iqk_times = 0x0;
		iqk_info->is_fw_iqk = false;
		for (idx = 0; idx < 2; idx++) { //channel		
			iqk_info->iqk_channel[idx] = 0x0;
			for (path = 0; path < ss_8852c; path++) {//path
				iqk_info->lok_cor_fail[idx][path] = false;
				iqk_info->lok_fin_fail[idx][path] = false;
				iqk_info->iqk_tx_fail[idx][path] = false;
				iqk_info->iqk_rx_fail[idx][path] = false;
				iqk_info->iqk_mcc_ch[idx][path] = 0x0;
				iqk_info->iqk_table_idx[path] = 0x0;
				iqk_info->is_wb_txiqk[path] = true;
				iqk_info->is_wb_rxiqk[path] = true;
			}
		}
	}
	return;
}
u8 halrf_iqk_get_rxevm_8852c(struct rf_info *rf)
{
	u8 rxevm = 0x0;
#if 1
	halrf_wreg(rf, 0x738, 0x00000010, 0x1);
	halrf_wreg(rf, 0x750, 0xffffffff, 0x300002);
	halrf_wreg(rf, 0x754, 0xffffffff, 0x300002);
	halrf_wreg(rf, 0x758, 0xffffffff, 0x300002);
	halrf_wreg(rf, 0x75c, 0xffffffff, 0x300002);
	halrf_wreg(rf, 0x760, 0xffffffff, 0x300002);
	halrf_wreg(rf, 0x764, 0xffffffff, 0x300002);
	halrf_wreg(rf, 0x768, 0xffffffff, 0x300002);
	halrf_wreg(rf, 0x76c, 0xffffffff, 0x300002);
	halrf_wreg(rf, 0x770, 0xffffffff, 0x300002);
	halrf_wreg(rf, 0x774, 0xffffffff, 0x300002);
	halrf_wreg(rf, 0x738, 0x000000c0, 0x1);
	halrf_wreg(rf, 0x738, 0x00000020, 0x0);
	halrf_wreg(rf, 0x738, 0x00000020, 0x1);
	halrf_wreg(rf, 0x738, 0x0000ff00, 0xf);
	rxevm = (u8) (halrf_rreg(rf, 0x1af0, 0xff000000)) /4;
	halrf_wreg(rf, 0x738, 0x00000020, 0x0);
#else 
	rxevm = rtw_hal_bb_get_rxevm_single_user(rf->hal_com, HW_PHY_0, 0, 1);
#endif
	return rxevm;
}

u32 halrf_iqk_get_rximr_8852c(struct rf_info *rf, u8 path, u32 idx)
{
	u32 rximr =0x0;
	u32 tone_idx = 0x0;
	u32 main_idx = 0x0;
	//u32 rf0 = 0x0;
	u32 pwr_sig = 0x0, pwr_img = 0x0;
	u32 reg_0x800c = 0x0, reg_0x8018 = 0x0, reg_0x801c = 0x0, reg_0x81cc = 0x0, reg_0x82cc = 0x0;
	u32 tmp =0x0;
	
	reg_0x800c = halrf_rreg(rf, 0x800c, MASKDWORD);
	reg_0x8018 = halrf_rreg(rf, 0x8018, MASKDWORD);
	reg_0x801c = halrf_rreg(rf, 0x801c, MASKDWORD); 
	reg_0x81cc = halrf_rreg(rf, 0x81cc, MASKDWORD); 
	reg_0x82cc = halrf_rreg(rf, 0x82cc, MASKDWORD); 

	//tone_idx = idx * 320MHz/80MHz
	if (idx <100) {		
		tone_idx = idx * 4;
		main_idx = tone_idx & 0xfff;
	} else {
		tone_idx = (idx -1000) *4;
		main_idx = (0x2000-tone_idx) & 0xfff;
	}
	RF_DBG(rf, DBG_RF_IQK, "[IQK][IMR]S%x, idx = 0x%x, tone_idx = 0x%x, main_idx = 0x%x\n", path, idx, tone_idx, main_idx);

	iqk_macbb_setting_8852c(rf, HW_PHY_0, path);
	iqk_preset_8852c(rf, path);	
	_iqk_rxk_setting_8852c(rf, path);

	// 06a_S0_IQKPSD_Apply_RxCFIR_52C
	halrf_wreg(rf, 0x8154 + (path << 8), 0x00000100, 0x0);
	halrf_wreg(rf, 0x8140 + (path << 8), 0x00000100, 0x1);
	tmp = halrf_rrf(rf, path, 0x00, 0x03800);
	halrf_wreg(rf, 0x8144 + (path << 8), 0x00070000, tmp);
	tmp= halrf_rrf(rf, path, 0x00, 0x003e0);
	halrf_wreg(rf, 0x8144 + (path << 8), 0x1f000000, tmp);
	halrf_wreg(rf, 0x81dc + (path << 8), MASKDWORD, 0x00000001);
	halrf_wreg(rf, 0x81dc + (path << 8), MASKDWORD, 0x00000000);

	halrf_wrf(rf, path, 0x0, 0xf0000, 0x3);
	halrf_wreg(rf, 0x8000, MASKDWORD, 0x00000008);
	halrf_wreg(rf, 0x800c, MASKDWORD, 0x00000c00);
	halrf_wreg(rf, 0x8018, 0x70000000, 0x4);
	halrf_wreg(rf, 0x80d0, MASKDWORD, 0x00300000);
	halrf_wreg(rf, 0x81cc+ (path << 8), 0x0000003f, 0x3f);
#if 0	
	halrf_wreg(rf, 0x8140 + (path << 8), 0x00000100, 0x1);
	rf0 = halrf_rrf(rf, path, 0x00, 0x03800);
	halrf_wreg(rf, 0x8144 + (path << 8), 0x00070000, rf0);
	rf0  = halrf_rrf(rf, path, 0x00, 0x003e0);
	halrf_wreg(rf, 0x8144 + (path << 8), 0x1f000000, rf0);
	halrf_wreg(rf, 0x81dc + (path << 8), MASKDWORD, 0x00000001);
	halrf_wreg(rf, 0x81dc + (path << 8), MASKDWORD, 0x00000000);
#endif
	halrf_wreg(rf, 0x802c, 0x0fff0000, main_idx);
	halrf_wreg(rf, 0x8034, 0x00000001, 0x1);
	halrf_wreg(rf, 0x8034, 0x00000001, 0x0);
	halrf_delay_us(rf, 100);

	halrf_wreg(rf, 0x80d4, MASKDWORD, 0x002d0000);
	pwr_sig = halrf_rreg(rf, 0x80fc, 0x007f0000);
	halrf_wreg(rf, 0x80d4, MASKDWORD, 0x002e0000);
	pwr_sig = (pwr_sig << 25) + (halrf_rreg(rf, 0x80fc, MASKDWORD) >> 7);

	halrf_wreg(rf, 0x80d4, MASKDWORD, 0x002d0000);
	pwr_img = halrf_rreg(rf, 0x80fc, 0x0000007f);	
	halrf_wreg(rf, 0x80d4, MASKDWORD, 0x002f0000);
	pwr_img = (pwr_img << 25) + (halrf_rreg(rf, 0x80fc, MASKDWORD) >> 7);	

	rximr = (u32) (pwr_sig / pwr_img);
	RF_DBG(rf, DBG_RF_IQK, "[IQK][IMR]S%x, %x, %d,%d, %d\n", path, halrf_rrf(rf, path, 0x0, MASKRF),pwr_sig, pwr_img, rximr);

	halrf_wreg(rf, 0x800c, MASKDWORD, reg_0x800c);
	halrf_wreg(rf, 0x8018, MASKDWORD, reg_0x8018);
	halrf_wreg(rf, 0x801c, MASKDWORD, reg_0x801c);
	halrf_wreg(rf, 0x81cc, MASKDWORD, reg_0x81cc);
	halrf_wreg(rf, 0x82cc, MASKDWORD, reg_0x82cc);

	return rximr;
}

void halrf_lok_backup_dbcc_8852c(struct rf_info *rf, enum phl_phy_idx phy_idx)
{
	struct halrf_iqk_info *iqk_info = &rf->iqk;
	u8 kpath, i;

	RF_DBG(rf, DBG_RF_RFK, "[DBCC]===>%s\n", __func__);
	kpath = halrf_kpath(rf, phy_idx);
	for (i = 0; i < 2; i++) {
		if (kpath & BIT(i)) {
			iqk_info->lok_0x58[i] = halrf_rrf(rf, i, 0x58, MASKRF);
			iqk_info->lok_0x5c[i] = halrf_rrf(rf, i, 0x5c, MASKRF);
			iqk_info->lok_0x7c[i] = halrf_rrf(rf, i, 0x7c, MASKRF);
		}
	}
}

void halrf_lok_reload_dbcc_8852c(struct rf_info *rf, enum phl_phy_idx phy_idx)
{
	struct halrf_iqk_info *iqk_info = &rf->iqk;
	u8 kpath, i;

	RF_DBG(rf, DBG_RF_RFK, "[DBCC]===>%s\n", __func__);
	kpath = halrf_kpath(rf, phy_idx);
	for (i = 0; i < 2; i++) {
		if (kpath & BIT(i)) {
			halrf_wrf(rf, i, 0xdf, BIT(2), 0x1);
			halrf_wrf(rf, i, 0x58, MASKRF, iqk_info->lok_0x58[i]);
			halrf_wrf(rf, i, 0x5c, MASKRF, iqk_info->lok_0x5c[i]);
			halrf_wrf(rf, i, 0x7c, MASKRF, iqk_info->lok_0x7c[i]);
		}
	}
}

#endif



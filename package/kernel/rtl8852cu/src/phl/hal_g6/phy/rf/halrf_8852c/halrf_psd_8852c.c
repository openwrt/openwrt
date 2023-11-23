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
#include "../halrf_precomp.h"

#ifdef RF_8852C_SUPPORT

void _halrf_psd_backup_bb_registers_8852c(
	struct rf_info *rf,
	enum phl_phy_idx phy,
	u32 *reg,
	u32 *reg_backup,
	u32 reg_num)
{
	u32 i;

	for (i = 0; i < reg_num; i++) {
		reg_backup[i] = halrf_rreg(rf, reg[i], MASKDWORD);

		RF_DBG(rf, DBG_RF_PSD, "[IQK_PSD] Backup BB 0x%08x = 0x%08x\n",
		       reg[i], reg_backup[i]);
	}
}

void _halrf_psd_reload_bb_registers_8852c(
	struct rf_info *rf,
	enum phl_phy_idx phy,
	u32 *reg,
	u32 *reg_backup,
	u32 reg_num)

{
	u32 i;

	for (i = 0; i < reg_num; i++) {
		halrf_wreg(rf, reg[i], MASKDWORD, reg_backup[i]);

		RF_DBG(rf, DBG_RF_PSD, "[IQK_PSD] Reload BB 0x%08x = 0x%08x\n",
		       reg[i], reg_backup[i]);
	}
}

void _halrf_psd_bkup_rf_8852c(
	struct rf_info *rf,
	u32 *rf_reg,
	u8 path)
{
	struct halrf_psd_data *psd_info = &rf->psd;
	u8 i;

	for (i = 0; i < PSD_RF_REG_NUM_8852C; i++) {
		psd_info->rf_bkup[path][i] = halrf_rrf(rf, path, rf_reg[i], MASKRF);

		RF_DBG(rf, DBG_RF_PSD, "[IQK_PSD] Backup RF S%d 0x%x = %x\n",
			path, rf_reg[i], psd_info->rf_bkup[path][i]);
	}
}

void _halrf_psd_reload_rf_8852c(
	struct rf_info *rf,
	u32 *rf_reg,
	u8 path)
{
	struct halrf_psd_data *psd_info = &rf->psd;
	u8 i;

	for (i = 0; i < PSD_RF_REG_NUM_8852C; i++) {
		halrf_wrf(rf, path, rf_reg[i], MASKRF, psd_info->rf_bkup[path][i]);

		RF_DBG(rf, DBG_RF_PSD, "[IQK_PSD] Reload RF S%d 0x%x = %x\n",
			path, rf_reg[i], psd_info->rf_bkup[path][i]);
	}
}

void _halrf_psd_set_dac_off_adc_8852c(struct rf_info *rf, enum phl_phy_idx phy)

{
	struct halrf_psd_data *psd_info = &rf->psd;
	u8 bw = rf->hal_com->band[phy].cur_chandef.bw;

	RF_DBG(rf, DBG_RF_PSD, "======> %s   phy=%d   bw=%d   psd_info->path=%d\n",
		__func__, phy, bw, psd_info->path);

	if (psd_info->path == RF_PATH_A) {
		if (bw == CHANNEL_WIDTH_20) {
			/*05_BW20_DAC_off_ADC080_and_RxCFIR_path0_KS*/
			halrf_wreg(rf, 0x20fc, 0xffff0000, 0x0101);
			halrf_wreg(rf, 0x5670, 0x00002000, 0x1);
			halrf_wreg(rf, 0x12a0, 0x00080000, 0x1);
			halrf_wreg(rf, 0x12a0, 0x00070000, 0x1);
			halrf_wreg(rf, 0x5670, 0x60000000, 0x0);
			halrf_wreg(rf, 0xc0d4, 0x0c000000, 0x3);
			halrf_wreg(rf, 0xc0d8, 0x000001e0, 0xf);
			halrf_wrf(rf, RF_PATH_A, 0x8f, 0x01000, 0x1);
			halrf_wreg(rf, 0x12b8, 0x40000000, 0x1);
			halrf_wreg(rf, 0x32b8, 0x40000000, 0x1);
			halrf_wreg(rf, 0x030c, 0xff000000, 0xef);
			halrf_wreg(rf, 0x030c, 0xff000000, 0xe3);
			halrf_wreg(rf, 0x032c, 0xffff0000, 0x8009);
			halrf_wreg(rf, 0x032c, 0xffff0000, 0x8049);
			halrf_wreg(rf, 0x20fc, 0xffff0000, 0x1101);
			halrf_wreg(rf, 0x8120, MASKDWORD, 0x40000a08);
			halrf_wreg(rf, 0x8128, 0x000000ff, 0x84);
		} else if (bw == CHANNEL_WIDTH_40) {
			/*05_BW40_DAC_off_ADC080_and_RxCFIR_path0_KS*/
			halrf_wreg(rf, 0x20fc, 0xffff0000, 0x0101);
			halrf_wreg(rf, 0x5670, 0x00002000, 0x1);
			halrf_wreg(rf, 0x12a0, 0x00080000, 0x1);
			halrf_wreg(rf, 0x12a0, 0x00070000, 0x1);
			halrf_wreg(rf, 0x5670, 0x60000000, 0x0);
			halrf_wreg(rf, 0xc0d4, 0x0c000000, 0x3);
			halrf_wreg(rf, 0xc0d8, 0x000001e0, 0xf);
			halrf_wrf(rf, RF_PATH_A, 0x8f, 0x01000, 0x1);
			halrf_wreg(rf, 0x12b8, 0x40000000, 0x1);
			halrf_wreg(rf, 0x32b8, 0x40000000, 0x1);
			halrf_wreg(rf, 0x030c, 0xff000000, 0xef);
			halrf_wreg(rf, 0x030c, 0xff000000, 0xe3);
			halrf_wreg(rf, 0x032c, 0xffff0000, 0x8009);
			halrf_wreg(rf, 0x032c, 0xffff0000, 0x8049);
			halrf_wreg(rf, 0x20fc, 0xffff0000, 0x1101);
			halrf_wreg(rf, 0x8120, MASKDWORD, 0x40000a08);
			halrf_wreg(rf, 0x8128, 0x000000ff, 0x85);
		} else if (bw == CHANNEL_WIDTH_80) {
			/*05_BW80_DAC_off_ADC160_and_RxCFIR_path0_KS*/
			halrf_wreg(rf, 0x20fc, 0xffff0000, 0x0101);
			halrf_wreg(rf, 0x5670, 0x00002000, 0x1);
			halrf_wreg(rf, 0x12a0, 0x00080000, 0x1);
			halrf_wreg(rf, 0x12a0, 0x00070000, 0x2);
			halrf_wreg(rf, 0x5670, 0x60000000, 0x1);
			halrf_wreg(rf, 0xc0d4, 0x0c000000, 0x2);
			halrf_wreg(rf, 0xc0d8, 0x000001e0, 0xd);
			halrf_wrf(rf, RF_PATH_A, 0x8f, 0x01000, 0x1);
			halrf_wreg(rf, 0x12b8, 0x40000000, 0x1);
			halrf_wreg(rf, 0x32b8, 0x40000000, 0x1);
			halrf_wreg(rf, 0x030c, 0xff000000, 0xef);
			halrf_wreg(rf, 0x030c, 0xff000000, 0xe3);
			halrf_wreg(rf, 0x032c, 0xffff0000, 0x8009);
			halrf_wreg(rf, 0x032c, 0xffff0000, 0x8049);
			halrf_wreg(rf, 0x20fc, 0xffff0000, 0x1101);
			halrf_wreg(rf, 0x8120, MASKDWORD, 0x41000a08);
			halrf_wreg(rf, 0x8128, 0x000000ff, 0x86);
		}  else if (bw == CHANNEL_WIDTH_160) {
			/*05_BW160_DAC_off_ADC320_and_RxCFIR_path0_KS*/
			halrf_wreg(rf, 0x20fc, 0xffff0000, 0x0101);
			halrf_wreg(rf, 0x5670, 0x00002000, 0x1);
			halrf_wreg(rf, 0x12a0, 0x00080000, 0x1);
			halrf_wreg(rf, 0x12a0, 0x00070000, 0x3);
			halrf_wreg(rf, 0x5670, 0x60000000, 0x2);
			halrf_wreg(rf, 0xc0d4, 0x0c000000, 0x1);
			halrf_wreg(rf, 0xc0d8, 0x000001e0, 0xb);
			halrf_wrf(rf, RF_PATH_A, 0x8f, 0x01000, 0x1);
			halrf_wreg(rf, 0x12b8, 0x40000000, 0x1);
			halrf_wreg(rf, 0x32b8, 0x40000000, 0x1);
			halrf_wreg(rf, 0x030c, 0xff000000, 0xef);
			halrf_wreg(rf, 0x030c, 0xff000000, 0xe3);
			halrf_wreg(rf, 0x032c, 0xffff0000, 0x8009);
			halrf_wreg(rf, 0x032c, 0xffff0000, 0x8049);
			halrf_wreg(rf, 0x20fc, 0xffff0000, 0x1101);
			halrf_wreg(rf, 0x8120, MASKDWORD, 0x42000a08);
			halrf_wreg(rf, 0x8128, 0x000000ff, 0x87);
		}
	} else {
		if (bw == CHANNEL_WIDTH_20) {
			/*05_BW20_DAC_off_ADC080_and_RxCFIR_path1_KS*/
			halrf_wreg(rf, 0x20fc, 0xffff0000, 0x0202);
			halrf_wreg(rf, 0x7670, 0x00002000, 0x1);
			halrf_wreg(rf, 0x32a0, 0x00080000, 0x1);
			halrf_wreg(rf, 0x32a0, 0x00070000, 0x1);
			halrf_wreg(rf, 0x7670, 0x60000000, 0x0);
			halrf_wreg(rf, 0xc1d4, 0x0c000000, 0x3);
			halrf_wreg(rf, 0xc1d8, 0x000001e0, 0xf);
			halrf_wrf(rf, RF_PATH_B, 0x8f, 0x01000, 0x1);
			halrf_wreg(rf, 0x12b8, 0x40000000, 0x1);
			halrf_wreg(rf, 0x32b8, 0x40000000, 0x1);
			halrf_wreg(rf, 0x030c, 0xff000000, 0xef);
			halrf_wreg(rf, 0x030c, 0xff000000, 0xe3);
			halrf_wreg(rf, 0x032c, 0xffff0000, 0x8009);
			halrf_wreg(rf, 0x032c, 0xffff0000, 0x8049);
			halrf_wreg(rf, 0x20fc, 0xffff0000, 0x2202);
			halrf_wreg(rf, 0x8220, MASKDWORD, 0x40000a08);
			halrf_wreg(rf, 0x8228, 0x000000ff, 0x84);
		} else if (bw == CHANNEL_WIDTH_40) {
			/*05_BW40_DAC_off_ADC080_and_RxCFIR_path1_KS*/
			halrf_wreg(rf, 0x20fc, 0xffff0000, 0x0202);
			halrf_wreg(rf, 0x7670, 0x00002000, 0x1);
			halrf_wreg(rf, 0x32a0, 0x00080000, 0x1);
			halrf_wreg(rf, 0x32a0, 0x00070000, 0x1);
			halrf_wreg(rf, 0x7670, 0x60000000, 0x0);
			halrf_wreg(rf, 0xc1d4, 0x0c000000, 0x3);
			halrf_wreg(rf, 0xc1d8, 0x000001e0, 0xf);
			halrf_wrf(rf, RF_PATH_B, 0x8f, 0x01000, 0x1);
			halrf_wreg(rf, 0x12b8, 0x40000000, 0x1);
			halrf_wreg(rf, 0x32b8, 0x40000000, 0x1);
			halrf_wreg(rf, 0x030c, 0xff000000, 0xef);
			halrf_wreg(rf, 0x030c, 0xff000000, 0xe3);
			halrf_wreg(rf, 0x032c, 0xffff0000, 0x8009);
			halrf_wreg(rf, 0x032c, 0xffff0000, 0x8049);
			halrf_wreg(rf, 0x20fc, 0xffff0000, 0x2202);
			halrf_wreg(rf, 0x8220, MASKDWORD, 0x40000a08);
			halrf_wreg(rf, 0x8228, 0x000000ff, 0x85);
		} else if (bw == CHANNEL_WIDTH_80) {
			/*05_BW80_DAC_off_ADC160_and_RxCFIR_path1_KS*/
			halrf_wreg(rf, 0x20fc, 0xffff0000, 0x0202);
			halrf_wreg(rf, 0x7670, 0x00002000, 0x1);
			halrf_wreg(rf, 0x32a0, 0x00080000, 0x1);
			halrf_wreg(rf, 0x32a0, 0x00070000, 0x2);
			halrf_wreg(rf, 0x7670, 0x60000000, 0x1);
			halrf_wreg(rf, 0xc1d4, 0x0c000000, 0x2);
			halrf_wreg(rf, 0xc1d8, 0x000001e0, 0xd);
			halrf_wrf(rf, RF_PATH_B, 0x8f, 0x01000, 0x1);
			halrf_wreg(rf, 0x12b8, 0x40000000, 0x1);
			halrf_wreg(rf, 0x32b8, 0x40000000, 0x1);
			halrf_wreg(rf, 0x030c, 0xff000000, 0xef);
			halrf_wreg(rf, 0x030c, 0xff000000, 0xe3);
			halrf_wreg(rf, 0x032c, 0xffff0000, 0x8009);
			halrf_wreg(rf, 0x032c, 0xffff0000, 0x8049);
			halrf_wreg(rf, 0x20fc, 0xffff0000, 0x2202);
			halrf_wreg(rf, 0x8220, MASKDWORD, 0x41000a08);
			halrf_wreg(rf, 0x8228, 0x000000ff, 0x86);
		}  else if (bw == CHANNEL_WIDTH_160) {
			/*05_BW160_DAC_off_ADC320_and_RxCFIR_path1_KS*/
			halrf_wreg(rf, 0x20fc, 0xffff0000, 0x0202);
			halrf_wreg(rf, 0x7670, 0x00002000, 0x1);
			halrf_wreg(rf, 0x32a0, 0x00080000, 0x1);
			halrf_wreg(rf, 0x32a0, 0x00070000, 0x3);
			halrf_wreg(rf, 0x7670, 0x60000000, 0x2);
			halrf_wreg(rf, 0xc1d4, 0x0c000000, 0x1);
			halrf_wreg(rf, 0xc1d8, 0x000001e0, 0xb);
			halrf_wrf(rf, RF_PATH_B, 0x8f, 0x01000, 0x1);
			halrf_wreg(rf, 0x12b8, 0x40000000, 0x1);
			halrf_wreg(rf, 0x32b8, 0x40000000, 0x1);
			halrf_wreg(rf, 0x030c, 0xff000000, 0xef);
			halrf_wreg(rf, 0x030c, 0xff000000, 0xe3);
			halrf_wreg(rf, 0x032c, 0xffff0000, 0x8009);
			halrf_wreg(rf, 0x032c, 0xffff0000, 0x8049);
			halrf_wreg(rf, 0x20fc, 0xffff0000, 0x2202);
			halrf_wreg(rf, 0x8220, MASKDWORD, 0x42000a08);
			halrf_wreg(rf, 0x8228, 0x000000ff, 0x87);
		}
	}
}

void halrf_psd_init_8852c(struct rf_info *rf, enum phl_phy_idx phy,
			u8 path, u8 iq_path, u32 avg, u32 fft)
{
	struct halrf_psd_data *psd_info = &rf->psd;
	u32 reg_tmp;
	u32 bb_reg[PSD_BACKUP_NUM_8852C] = {
			0x20fc, 0x5670, 0x7670, 0x12a0, 0x32a0,
			0x8080, 0x8008,	0x8088, 0x80d0, 0x8074,
			0x81dc, 0x82dc, 0xc0d4, 0xc1d4, 0xc0d8,
			0xc1d8, 0x12b8, 0x32b8, 0x030c, 0x032c,
			0x8120, 0x8220, 0x8128, 0x8228, 0x8154,
			0x8254, 0x8140, 0x8240,	0x8144, 0x8244,
			0x801c, 0x8000, 0x800c, 0x8018, 0x81cc,
			0x82cc, 0x802c, 0x8034, 0x80d4, 0x80fc};

	u32 rf_reg[PSD_RF_REG_NUM_8852C] = {0x0, 0x5, 0x8f, 0x10000, 0x10005};

	RF_DBG(rf, DBG_RF_PSD, "======> %s   phy=%d   path=%d   iq_path=%d   avg=%d   fft=%d\n",
		__func__, phy,  path, iq_path, avg, fft);

	if (psd_info->psd_progress == 0) {
		_halrf_psd_backup_bb_registers_8852c(rf, phy, bb_reg,
				psd_info->psd_reg_backup, PSD_BACKUP_NUM_8852C);

		_halrf_psd_bkup_rf_8852c(rf, rf_reg, RF_PATH_A);
		_halrf_psd_bkup_rf_8852c(rf, rf_reg, RF_PATH_B);
	}

	psd_info->psd_progress = 1;

	psd_info->path = path;
	psd_info->iq_path = iq_path;
	psd_info->avg = avg;
	psd_info->fft = fft;

	/*01a_set_iqk_kmod_DRFC_path0*/
	halrf_wrf(rf, path, 0x10005, 0x00001, 0x0);

	/*01b_set_iqk_kmod_bb_path0*/
	if (path == RF_PATH_A) {
		halrf_wreg(rf, 0x20fc, 0x00010000, 0x1);
		halrf_wreg(rf, 0x20fc, 0x00100000, 0x0);
		halrf_wreg(rf, 0x20fc, 0x01000000, 0x1);
		halrf_wreg(rf, 0x20fc, 0x10000000, 0x0);
		halrf_wreg(rf, 0x5670, MASKDWORD, 0xf801fffd);
		halrf_wreg(rf, 0x5670, 0x00004000, 0x1);
		halrf_wreg(rf, 0x12a0, 0x00008000, 0x1);
		halrf_wreg(rf, 0x5670, 0x80000000, 0x1);
		halrf_wreg(rf, 0x12a0, 0x00007000, 0x7);
		halrf_wreg(rf, 0x5670, 0x00002000, 0x1);
		halrf_wreg(rf, 0x12a0, 0x00080000, 0x1);
		halrf_wreg(rf, 0x12a0, 0x00070000, 0x3);
		halrf_wreg(rf, 0x5670, 0x60000000, 0x2);
		halrf_wreg(rf, 0x20fc, 0x00100000, 0x1);
		halrf_wreg(rf, 0x20fc, 0x10000000, 0x1);
	} else {	/*01b_set_iqk_kmod_bb_path1*/
		halrf_wreg(rf, 0x20fc, 0x00020000, 0x1);
		halrf_wreg(rf, 0x20fc, 0x00200000, 0x0);
		halrf_wreg(rf, 0x20fc, 0x02000000, 0x1);
		halrf_wreg(rf, 0x20fc, 0x20000000, 0x0);
		halrf_wreg(rf, 0x7670, MASKDWORD, 0xf801fffd);
		halrf_wreg(rf, 0x7670, 0x00004000, 0x1);
		halrf_wreg(rf, 0x32a0, 0x00008000, 0x1);
		halrf_wreg(rf, 0x7670, 0x80000000, 0x1);
		halrf_wreg(rf, 0x32a0, 0x00007000, 0x7);
		halrf_wreg(rf, 0x7670, 0x00002000, 0x1);
		halrf_wreg(rf, 0x32a0, 0x00080000, 0x1);
		halrf_wreg(rf, 0x32a0, 0x00070000, 0x3);
		halrf_wreg(rf, 0x7670, 0x60000000, 0x2);
		halrf_wreg(rf, 0x20fc, 0x00200000, 0x1);
		halrf_wreg(rf, 0x20fc, 0x20000000, 0x1);
	}

	/*02_IQKPSD_Preset_path0*/
	if (path == RF_PATH_A) {
		halrf_wreg(rf, 0x8080, MASKDWORD, 0x00000006);
		halrf_wrf(rf, path, 0x5, 0x00001, 0x0);
		halrf_wreg(rf, 0x8008, MASKDWORD, 0x00000080);
		halrf_wreg(rf, 0x8088, MASKDWORD, 0x81ff010a);
		halrf_wreg(rf, 0x80d0, MASKDWORD, 0x00200000);
		halrf_wreg(rf, 0x8074, MASKDWORD, 0x80000000);
		halrf_wreg(rf, 0x81dc, MASKDWORD, 0x00000000);
	} else {	/*02_IQKPSD_Preset_path1*/
		halrf_wreg(rf, 0x8080, MASKDWORD, 0x00000006);
		halrf_wrf(rf, path, 0x5, 0x00001, 0x0);
		halrf_wreg(rf, 0x8008, MASKDWORD, 0x00000080);
		halrf_wreg(rf, 0x8088, MASKDWORD, 0x81ff010a);
		halrf_wreg(rf, 0x80d0, MASKDWORD, 0x00200000);
		halrf_wreg(rf, 0x8074, MASKDWORD, 0x80000000);
		halrf_wreg(rf, 0x82dc, MASKDWORD, 0x00000000);
	}

	_halrf_psd_set_dac_off_adc_8852c(rf, phy);

	/*06a_S0_IQKPSD_Apply_RxCFIR_52C*/
	if (path == RF_PATH_A) {
		halrf_wreg(rf, 0x8154, 0x00000100, 0x0);
		halrf_wreg(rf, 0x8140, 0x00000100, 0x1);
		reg_tmp = halrf_rrf(rf, path, 0x00, 0x03800);
		halrf_wreg(rf, 0x8144, 0x00070000, reg_tmp);
		reg_tmp = halrf_rrf(rf, path, 0x00, 0x003e0);
		halrf_wreg(rf, 0x8144, 0x1f000000, reg_tmp);
		halrf_wreg(rf, 0x81dc, MASKDWORD, 0x00000001);
		halrf_wreg(rf, 0x81dc, MASKDWORD, 0x00000000);
	} else {	/*06a_S1_IQKPSD_Apply_RxCFIR_52C*/
		halrf_wreg(rf, 0x8254, 0x00000100, 0x0);
		halrf_wreg(rf, 0x8240, 0x00000100, 0x1);
		reg_tmp = halrf_rrf(rf, path, 0x00, 0x03800);
		halrf_wreg(rf, 0x8244, 0x00070000, reg_tmp);
		reg_tmp = halrf_rrf(rf, path, 0x00, 0x003e0);
		halrf_wreg(rf, 0x8244, 0x1f000000, reg_tmp);
		halrf_wreg(rf, 0x82dc, MASKDWORD, 0x00000001);
		halrf_wreg(rf, 0x82dc, MASKDWORD, 0x00000000);
	}

	/*windowing*/

	if (fft == 160)
		halrf_wreg(rf, 0x801c, 0x00003000, 0x0);
	else if (fft == 320)
		halrf_wreg(rf, 0x801c, 0x00003000, 0x1);
	else if (fft == 640)
		halrf_wreg(rf, 0x801c, 0x00003000, 0x2);
	else /*1280*/
		halrf_wreg(rf, 0x801c, 0x00003000, 0x3);

	if (avg == 1)
		halrf_wreg(rf, 0x801c, 0x000e0000, 0x0);
	else if (avg == 2)
		halrf_wreg(rf, 0x801c, 0x000e0000, 0x1);
	else if (avg == 4)
		halrf_wreg(rf, 0x801c, 0x000e0000, 0x2);
	else if (avg == 8)
		halrf_wreg(rf, 0x801c, 0x000e0000, 0x3);
	else if (avg == 16)
		halrf_wreg(rf, 0x801c, 0x000e0000, 0x4);
	else if (avg == 64)
		halrf_wreg(rf, 0x801c, 0x000e0000, 0x6);
	else if (avg == 128)
		halrf_wreg(rf, 0x801c, 0x000e0000, 0x7);
	else	/*32*/
		halrf_wreg(rf, 0x801c, 0x000e0000, 0x5);

	/*IQ_ALL_OFF = 0x3*/
	/*ONLY_I_ON = 0x2*/
	/*ONLY_Q_ON = 0x1*/
	/*IQ_ALL_ON = 0x0*/
	halrf_wreg(rf, 0x801c, 0x00000006, iq_path);

	/*06b_S0_IQKPSD_52C*/
	if (path == RF_PATH_A) {
		halrf_wrf(rf, RF_PATH_A, 0x0, 0xf0000, 0x3);
		halrf_wreg(rf, 0x8000, 0xffffffff, 0x00000008);
	} else {
		halrf_wrf(rf, RF_PATH_B, 0x0, 0xf0000, 0x3);
		halrf_wreg(rf, 0x8000, 0xffffffff, 0x0000000a);
	}

	halrf_wreg(rf, 0x800c, MASKDWORD, 0x00000c00);
	halrf_wreg(rf, 0x8018, 0x70000000, 0x4);
	halrf_wreg(rf, 0x80d0, MASKDWORD, 0x00300000);

	if (path == RF_PATH_A)
		halrf_wreg(rf, 0x81cc, 0x0000003f, 0x3f);
	else
		halrf_wreg(rf, 0x82cc, 0x0000003f, 0x3f);

}

void halrf_psd_restore_8852c(struct rf_info *rf, enum phl_phy_idx phy)
{
	struct halrf_psd_data *psd_info = &rf->psd;

	u32 bb_reg[PSD_BACKUP_NUM_8852C] = {
			0x20fc, 0x5670, 0x7670, 0x12a0, 0x32a0,
			0x8080, 0x8008,	0x8088, 0x80d0, 0x8074,
			0x81dc, 0x82dc, 0xc0d4, 0xc1d4, 0xc0d8,
			0xc1d8, 0x12b8, 0x32b8, 0x030c, 0x032c,
			0x8120, 0x8220, 0x8128, 0x8228, 0x8154,
			0x8254, 0x8140, 0x8240,	0x8144, 0x8244,
			0x801c, 0x8000, 0x800c, 0x8018, 0x81cc,
			0x82cc, 0x802c, 0x8034, 0x80d4, 0x80fc};

	u32 rf_reg[PSD_RF_REG_NUM_8852C] = {0x0, 0x5, 0x8f, 0x10000, 0x10005};

	RF_DBG(rf, DBG_RF_PSD, "======> %s   phy=%d   psd_info->path=%d\n", __func__, phy, psd_info->path);

	/*98_S0_IQKPSD_Reg_Restore_52C*/
	if (psd_info->path == RF_PATH_A) {
		halrf_wreg(rf, 0x8080, MASKDWORD, 0x00000002);
		halrf_delay_ms(rf, 1);
		halrf_wreg(rf, 0x8074, 0xff000000, 0x00);
		halrf_wreg(rf, 0x80d0, 0x00ff0000, 0x00);
		halrf_wreg(rf, 0x80e0, 0x00000001, 0x0);
		halrf_wreg(rf, 0x8120, MASKDWORD, 0x10010000);
		halrf_wreg(rf, 0x8128, 0x000000ff, 0x04);
		halrf_wreg(rf, 0x8140, 0x0000ff00, 0x00);
		halrf_wreg(rf, 0x8154, 0x0000ff00, 0x00);
		halrf_wreg(rf, 0x81dc, 0x000000ff, 0x02);
		halrf_wreg(rf, 0x8008, MASKDWORD, 0x00000000);
		halrf_wreg(rf, 0x8088, MASKDWORD, 0x80000000);
		/*halrf_wrf(rf, RF_PATH_A, 0xef, 0x00004, 0x0);*/
		/*halrf_wrf(rf, RF_PATH_A, 0x0, 0xf0000, 0x3);*/
		/*halrf_wrf(rf, RF_PATH_A, 0x5, 0x00001, 0x1);*/
	} else {	/*98_S1_IQKPSD_Reg_Restore_52C*/
		halrf_wreg(rf, 0x8080, MASKDWORD, 0x00000002);
		halrf_delay_ms(rf, 1);
		halrf_wreg(rf, 0x8074, 0xff000000, 0x00);
		halrf_wreg(rf, 0x80d0, 0x00ff0000, 0x00);
		halrf_wreg(rf, 0x80e0, 0x00000001, 0x0);
		halrf_wreg(rf, 0x8220, MASKDWORD, 0x10010000);
		halrf_wreg(rf, 0x8228, 0x000000ff, 0x04);
		halrf_wreg(rf, 0x8240, 0x0000ff00, 0x00);
		halrf_wreg(rf, 0x8254, 0x0000ff00, 0x00);
		halrf_wreg(rf, 0x82dc, 0x000000ff, 0x02);
		halrf_wreg(rf, 0x8008, MASKDWORD, 0x00000000);
		halrf_wreg(rf, 0x8088, MASKDWORD, 0x80000000);
		/*halrf_wrf(rf, RF_PATH_B, 0xef, 0x00004, 0x0);*/
		/*halrf_wrf(rf, RF_PATH_B, 0x0, 0xf0000, 0x3);*/
		/*halrf_wrf(rf, RF_PATH_B, 0x5, 0x00001, 0x1);*/
	}

	/*99a_restore_iqk_kmod_bb_path0_KS*/
	if (psd_info->path == RF_PATH_A) {
		halrf_wreg(rf, 0x12b8, 0x40000000, 0x0);
		halrf_wreg(rf, 0x20fc, 0x00010000, 0x1);
		halrf_wreg(rf, 0x20fc, 0x00100000, 0x0);
		halrf_wreg(rf, 0x20fc, 0x01000000, 0x1);
		halrf_wreg(rf, 0x20fc, 0x10000000, 0x0);
		halrf_wreg(rf, 0x5670, MASKDWORD, 0x00000000);
		halrf_wreg(rf, 0x12a0, 0x000ff000, 0x00);
		halrf_wreg(rf, 0x20fc, 0x00010000, 0x0);
		halrf_wreg(rf, 0x20fc, 0x01000000, 0x0);
	} else {	/*99a_restore_iqk_kmod_bb_path1_KS*/
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

#if 0
	/*99b_restore_iqk_kmod_DRFC_path0*/
	if (psd_info->path == RF_PATH_A)
		halrf_wrf(rf, RF_PATH_A, 0x10005, 0x00001, 0x1);
	else	/*99b_restore_iqk_kmod_DRFC_path1*/
		halrf_wrf(rf, RF_PATH_B, 0x10005, 0x00001, 0x1);
#endif

	if (psd_info->psd_progress == 1) {
		_halrf_psd_reload_bb_registers_8852c(rf, phy, bb_reg,
			psd_info->psd_reg_backup, PSD_BACKUP_NUM_8852C);
		_halrf_psd_reload_rf_8852c(rf, rf_reg, RF_PATH_A);
		_halrf_psd_reload_rf_8852c(rf, rf_reg, RF_PATH_B);
	}

	psd_info->psd_progress = 0;
}

u32 halrf_psd_get_point_data_8852c(struct rf_info *rf,
			enum phl_phy_idx phy, s32 point)
{
	u32 val_tmp, val, data;

	halrf_wreg(rf, 0x802c, 0x0fff0000, (point & 0xfff));
	halrf_wreg(rf, 0x8034, 0x00000001, 0x1);
	halrf_wreg(rf, 0x8034, 0x00000001, 0x0);
	halrf_delay_us(rf, 500);

	halrf_wreg(rf, 0x80d4, 0xffffffff, 0x002d0000);
	val_tmp = halrf_rreg(rf, 0x80fc, 0x007f0000);
	halrf_wreg(rf, 0x80d4, 0xffffffff, 0x002e0000);
	val = halrf_rreg(rf, 0x80fc, 0xffffffff);

	data = (val_tmp << 25) | (val >> 7);

	return data;
}

void halrf_psd_query_8852c(struct rf_info *rf, enum phl_phy_idx phy,
			u32 point, u32 start_point, u32 stop_point, u32 *outbuf)
{
	struct halrf_psd_data *psd_info = &rf->psd;
	u32 i = 0, j = 0;
	s32 point_temp = 0;

	RF_DBG(rf, DBG_RF_PSD, "======> %s phy=%d point=%d start_point=%d stop_point=%d\n",
		__func__, phy, point, start_point, stop_point);

#ifdef PHL_PLATFORM_AP
	if (psd_info->psd_result_running == 2) {
		for (i = 0; i < PSD_RF_DATA_NUM; i++)
			outbuf[i] = psd_info->psd_data[i];

		RF_DBG(rf, DBG_RF_PSD, "======> %s PSD End !!!\n", __func__);

		psd_info->psd_result_running = 0;
		return;
	}
#else
	if (psd_info->psd_result_running == 1) {
		RF_DBG(rf, DBG_RF_PSD, "======> %s PSD Running Return !!!\n", __func__);
		return;
	}
	psd_info->psd_result_running = 1;
#endif

	for (i = 0; i < PSD_RF_DATA_NUM; i++)
		psd_info->psd_data[i] = 0;

	i = start_point;
	while (i < stop_point) {
		point_temp = i - point;

		psd_info->psd_data[j] = halrf_psd_get_point_data_8852c(rf, phy, point_temp);

		i++;
		j++;
	}

	RF_DBG(rf, DBG_RF_PSD, "PSD Point = Start:%d   End:%d\n",
		start_point - point, point_temp);

	for (i = 0; i < PSD_RF_DATA_NUM; i = i + 10) {
		RF_DBG(rf, DBG_RF_PSD,
			"%d   %d   %d   %d   %d   %d   %d   %d   %d   %d\n",
			psd_info->psd_data[i],
			psd_info->psd_data[i + 1],
			psd_info->psd_data[i + 2],
			psd_info->psd_data[i + 3],
			psd_info->psd_data[i + 4],
			psd_info->psd_data[i + 5],
			psd_info->psd_data[i + 6],
			psd_info->psd_data[i + 7],
			psd_info->psd_data[i + 8],
			psd_info->psd_data[i + 9]);
	}

	for (i = 0; i < PSD_RF_DATA_NUM; i++)
		outbuf[i] = psd_info->psd_data[i];

	RF_DBG(rf, DBG_RF_PSD, "======> %s PSD End !!!\n", __func__);

#ifdef PHL_PLATFORM_AP
	psd_info->psd_result_running = 2;
#else
	psd_info->psd_result_running = 0;
#endif
}

#endif	/*RF_8852C_SUPPORT*/

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

#define t_avg 100


void halrf_afe_init_8852c(struct rf_info *rf)
{
#if 0
	halrf_wmac32(rf, 0x8040, 0xf);
	halrf_wreg(rf, 0xc0d4, MASKDWORD, 0x4486888c);
	halrf_wreg(rf, 0xc0d8, MASKDWORD, 0xc6ba10e0);
	halrf_wreg(rf, 0xc0dc, MASKDWORD, 0x30c52868);
	halrf_wreg(rf, 0xc0e0, MASKDWORD, 0x05008128);
	halrf_wreg(rf, 0xc0e4, MASKDWORD, 0x0000272b);
	halrf_wreg(rf, 0xc1d4, MASKDWORD, 0x4486888c);
	halrf_wreg(rf, 0xc1d8, MASKDWORD, 0xc6ba10e0);
	halrf_wreg(rf, 0xc1dc, MASKDWORD, 0x30c52868);
	halrf_wreg(rf, 0xc1e0, MASKDWORD, 0x05008128);
	halrf_wreg(rf, 0xc1e4, MASKDWORD, 0x0000272b);
#endif
}
void halrf_dack_init_8852c(struct rf_info *rf)
{

}

void halrf_dack_reset_8852c(struct rf_info *rf, enum rf_path path)
{
	if (path == RF_PATH_A) {
		halrf_wreg(rf, 0xc000, BIT(17), 0x0);
		halrf_wreg(rf, 0xc000, BIT(17), 0x1);
	} else {
		halrf_wreg(rf, 0xc100, BIT(17), 0x0);
		halrf_wreg(rf, 0xc100, BIT(17), 0x1);
	}
}

void halrf_drck_8852c(struct rf_info *rf)
{
	u32 c;
	u32 rck_d;

	RF_DBG(rf, DBG_RF_DACK, "[DACK]Ddie RCK start!!!\n");	
	halrf_wreg(rf, 0xc0c4, BIT(6), 0x1);
	c = 0;

#ifdef HALRF_CONFIG_FW_IO_OFLD_SUPPORT
	if (!halrf_polling_bb(rf, 0xc0c8, BIT(3), 1, 5000)) {
		RF_DBG(rf, DBG_RF_FW, "[IO OFLD][DACK] %s 0xc0c8[3] == 1 Timeout !!!\n", __func__);
	
	} else { //Polling ok
		halrf_delay_us(rf, 10);
		RF_DBG(rf, DBG_RF_FW, "[IO OFLD][DACK] %s polling 0xc0c8[3] == 1 in IO queue OK!!!\n", __func__);
	}	
#else
	while (halrf_rreg(rf, 0xc0c8, BIT(3)) == 0) {
		c++;
		halrf_delay_us(rf, 1);
		if (c > 10000) {
			RF_DBG(rf, DBG_RF_DACK, "[DACK]DRCK timeout\n");
			break;
		}
	}
#endif
		

	halrf_wreg(rf, 0xc0c4, BIT(6), 0x0);
	halrf_wreg(rf, 0xc094, BIT(9), 0x1);
	halrf_delay_us(rf, 1);
	halrf_wreg(rf, 0xc094, BIT(9), 0x0);



	/*manual write for LPS*/
	//rck_d = halrf_rreg(rf, 0xc0c8, 0xf8000);
	rck_d = halrf_rreg(rf, 0xc0c8, 0x7c00);	
	/*RCK_SEL=0*/
	halrf_wreg(rf, 0xc0c4, BIT(9), 0x0);
	halrf_wreg(rf, 0xc0c4, 0x1f, rck_d);
	RF_DBG(rf, DBG_RF_DACK, "[DACK]0xc0c4 = 0x%x\n", halrf_rreg(rf, 0xc0c4, MASKDWORD));
}

void halrf_addck_backup_8852c(struct rf_info *rf)
{
	struct halrf_dack_info *dack = &rf->dack;

	halrf_wreg(rf, 0xc0f4, 0x300, 0x0);
	dack->addck_d[0][0] = (u16)halrf_rreg(rf, 0xc0fc,0xffc00) ;
	dack->addck_d[0][1] = (u16)halrf_rreg(rf, 0xc0fc,0x003ff) ;

	halrf_wreg(rf, 0xc1f4, 0x300, 0x0);
	dack->addck_d[1][0] = (u16)halrf_rreg(rf, 0xc1fc,0xffc00) ;
	dack->addck_d[1][1] = (u16)halrf_rreg(rf, 0xc1fc,0x003ff) ;
}

void halrf_addck_reload_8852c(struct rf_info *rf)
{
	struct halrf_dack_info *dack = &rf->dack;
	/*S0*/
	halrf_wreg(rf, 0xc0f8, 0x0ffc0000, dack->addck_d[0][0]);
	halrf_wreg(rf, 0xc0f8, 0x0003ff00, dack->addck_d[0][1]);
	/*manual*/
	halrf_wreg(rf, 0xc0f8, 0x30000000, 0x3);
	/*S1*/
	halrf_wreg(rf, 0xc1f8, 0x0ffc0000, dack->addck_d[1][0]);
	halrf_wreg(rf, 0xc1f8, 0x0003ff00, dack->addck_d[1][1]);
	/*manual*/
	halrf_wreg(rf, 0xc1f8, 0x30000000, 0x3);
}


void halrf_dack_reload_by_path_8852c(struct rf_info *rf, u8 path, u8 index)
{
	struct halrf_dack_info *dack = &rf->dack;
	u32 temp = 0 , temp_offset, temp_reg;
	u8 i;
	u32 idx_offset, path_offset;

	if (index ==0)
		idx_offset = 0;
	else
		idx_offset = 0x14;

	if (path == RF_PATH_A)
		path_offset = 0;
	else
		path_offset = 0x28;

	temp_offset = idx_offset + path_offset;

	/*--FW Offload Start*/
#ifdef HALRF_CONFIG_FW_IO_OFLD_SUPPORT
	halrf_write_fwofld_start(rf);		/*FW Offload Start*/
#endif
	/*FW Offload Start--*/	

	halrf_wreg(rf, 0xc004, BIT(17), 0x1);
	halrf_wreg(rf, 0xc024, BIT(17), 0x1);
	halrf_wreg(rf, 0xc104, BIT(17), 0x1);
	halrf_wreg(rf, 0xc124, BIT(17), 0x1);

	/*--FW Offload End*/
#ifdef HALRF_CONFIG_FW_IO_OFLD_SUPPORT
	halrf_write_fwofld_end(rf); 	/*FW Offload End*/
#endif
	/*FW Offload End--*/

	/*msbk_d: 15/14/13/12*/
	temp = 0x0;
	for (i = 0; i < 4; i++) {
		temp |= dack->msbk_d[path][index][i+12] << (i * 8);
	}
	temp_reg = 0xc200 + temp_offset;
	halrf_w32(rf, temp_reg, temp);
	RF_DBG(rf, DBG_RF_DACK, "[DACK]0x%x=0x%x\n", temp_reg, halrf_rreg(rf, temp_reg, MASKDWORD));
	/*msbk_d: 11/10/9/8*/
	temp = 0x0;
	for (i = 0; i < 4; i++) {
		temp |= dack->msbk_d[path][index][i+8] << (i * 8);
	}
	temp_reg = 0xc204 + temp_offset;
	halrf_w32(rf, temp_reg, temp);
	RF_DBG(rf, DBG_RF_DACK, "[DACK]0x%x=0x%x\n", temp_reg, halrf_rreg(rf, temp_reg, MASKDWORD));
	/*msbk_d: 7/6/5/4*/
	temp = 0x0;
	for (i = 0; i < 4; i++) {
		temp |= dack->msbk_d[path][index][i+4] << (i * 8);
	}
	temp_reg = 0xc208 + temp_offset;
	halrf_w32(rf, temp_reg, temp);
	RF_DBG(rf, DBG_RF_DACK, "[DACK]0x%x=0x%x\n", temp_reg, halrf_rreg(rf, temp_reg, MASKDWORD));
	/*msbk_d: 3/2/1/0*/
	temp = 0x0;
	for (i = 0; i < 4; i++) {
		temp |= dack->msbk_d[path][index][i] << (i * 8);
	}
	temp_reg = 0xc20c + temp_offset;
	halrf_w32(rf, temp_reg, temp);
	RF_DBG(rf, DBG_RF_DACK, "[DACK]0x%x=0x%x\n", temp_reg, halrf_rreg(rf,temp_reg, MASKDWORD));
	/*dadak_d/biask_d*/
	temp = 0x0;
	temp =   (dack->biask_d[path][index] << 22) |
		 (dack->dadck_d[path][index] << 14);
	temp_reg = 0xc210 + temp_offset;
	halrf_w32(rf, temp_reg, temp);
	RF_DBG(rf, DBG_RF_DACK, "[DACK]0x%x=0x%x\n", temp_reg, halrf_rreg(rf,temp_reg, MASKDWORD));
	/*enable DACK result from reg */
	halrf_wreg(rf, 0xc210 + temp_offset, BIT(0), 0x1);
}


void halrf_dack_reload_8852c(struct rf_info *rf, enum rf_path path)
{
	u8 i;

	for (i = 0; i < 2; i++)
		halrf_dack_reload_by_path_8852c(rf, path, i);
	/*DACK result  1 : from reg_table ; 0 : from circuit Calibration*/
}
void halrf_dack_backup_s0_8852c(struct rf_info *rf)
{
	struct halrf_dack_info *dack = &rf->dack;
	u8 i;

	halrf_wreg(rf, 0x12b8, BIT(30), 0x1);
	/*MSBK*/
#if 0
	for (i = 0; i < 0x10; i++) {
		/*S0*/
		halrf_wreg(rf, 0xc000, 0x1e, i);
		temp = (u8)halrf_rreg(rf, 0xc04c, 0x7fc0);
		RF_DBG(rf, DBG_RF_DACK, "[DACK]0xc04c %d = 0x%x\n",
		i, temp);
	}

	for (i = 0; i < 0x10; i++) {
		/*S0*/
		halrf_wreg(rf, 0xc020, 0x1e, i);
		temp = (u8)halrf_rreg(rf, 0xc070, 0x7fc0);
		RF_DBG(rf, DBG_RF_DACK, "[DACK]0xc070 %d = 0x%x\n",
		i, temp);
	}
#endif
	for (i = 0; i < 0x10; i++) {
		halrf_wreg(rf, 0xc000, 0x1e, i);
		dack->msbk_d[0][0][i] = (u8)halrf_rreg(rf, 0xc05c, 0xff000000);
		halrf_wreg(rf, 0xc020, 0x1e, i);
		dack->msbk_d[0][1][i] = (u8)halrf_rreg(rf, 0xc080, 0xff000000);
	}


	/*biasK*/
	dack->biask_d[0][0] = (u16)halrf_rreg(rf, 0xc048, 0xffc);
	dack->biask_d[0][1] = (u16)halrf_rreg(rf, 0xc06c, 0xffc);
	/*DADCK*/
	dack->dadck_d[0][0] = (u8)halrf_rreg(rf, 0xc060, 0xff000000);
	dack->dadck_d[0][1] = (u8)halrf_rreg(rf, 0xc084, 0xff000000);
}

void halrf_dack_backup_s1_8852c(struct rf_info *rf)
{
	struct halrf_dack_info *dack = &rf->dack;
	u8 i;

	halrf_wreg(rf, 0x32b8, BIT(30), 0x1);
	/*MSBK*/
#if 0
	for (i = 0; i < 0x10; i++) {
		/*S0*/
		halrf_wreg(rf, 0xc100, 0x1e, i);
		temp = (u8)halrf_rreg(rf, 0xc14c, 0x7fc0);
		RF_DBG(rf, DBG_RF_DACK, "[DACK]0xc14c %d = 0x%x\n",
		i, temp);
	}

	for (i = 0; i < 0x10; i++) {
		/*S0*/
		halrf_wreg(rf, 0xc120, 0x1e, i);
		temp = (u8)halrf_rreg(rf, 0xc170, 0x7fc0);
		RF_DBG(rf, DBG_RF_DACK, "[DACK]0xc170 %d = 0x%x\n",
		i, temp);
	}
#endif

	for (i = 0; i < 0x10; i++) {
		halrf_wreg(rf, 0xc100, 0x1e, i);
		dack->msbk_d[1][0][i] = (u8)halrf_rreg(rf, 0xc15c, 0xff000000);
		halrf_wreg(rf, 0xc120, 0x1e, i);
		dack->msbk_d[1][1][i] = (u8)halrf_rreg(rf, 0xc180, 0xff000000);
	}
	/*biasK*/
	dack->biask_d[1][0] = (u16)halrf_rreg(rf, 0xc148, 0xffc);
	dack->biask_d[1][1] = (u16)halrf_rreg(rf, 0xc16c, 0xffc);
	/*DADCK*/
	dack->dadck_d[1][0] = (u8)halrf_rreg(rf, 0xc160, 0xff000000);
	dack->dadck_d[1][1] = (u8)halrf_rreg(rf, 0xc184, 0xff000000);
}


void halrf_check_addc_8852c(struct rf_info *rf, enum rf_path path)
{
	u32 temp, dc_re, dc_im;
	u32 i, m, p, t;
	u32 re[t_avg], im[t_avg];
#if 1
	halrf_wreg(rf, 0x20f4, BIT(24), 0x0);
	halrf_wreg(rf, 0x20f8, 0x80000000, 0x1);
	halrf_wreg(rf, 0x20f0, 0xff0000, 0x1);
	halrf_wreg(rf, 0x20f0, 0xf00, 0x2);
	halrf_wreg(rf, 0x20f0, 0xf, 0x0);
	if (path  == RF_PATH_A)
		halrf_wreg(rf, 0x20f0, 0xc0, 0x2);
	else
		halrf_wreg(rf, 0x20f0, 0xc0, 0x3);
	for (i = 0; i < t_avg; i++) {
		temp = halrf_rreg(rf, 0x1730, 0xffffffff);
		re[i] = (temp & 0xfff000) >> 12;
		im[i] = temp & 0xfff;
//		RF_DBG(rf, DBG_RF_DACK, "[DACK]S%d,re[i]= 0x%x,im[i] =0x%x\n",
//		path, re[i], im[i]);
	}
#else
	for (i = 0; i < t_avg; i++) {
		if (path  == RF_PATH_A)
			temp = halrf_rreg(rf, 0x1c8c, MASKDWORD);
		else
			temp = halrf_rreg(rf, 0x3c8c, MASKDWORD);

		re[i] = (temp & 0xfff000) >> 12;
		im[i] = temp & 0xfff;
//		RF_DBG(rf, DBG_RF_DACK, "[DACK]S%d,re[i]= 0x%x,im[i] =0x%x\n",
//		path, re[i], im[i]);
	}

#endif
	m = 0;
	p = 0;
	for (i = 0; i < t_avg; i++) {
		if (re[i] > 0x800)
			m = (0x1000 - re[i]) + m;
		else
			p = re[i] + p;
	}

	if (p > m) {
		t = p - m;
		t = t / t_avg;
	} else {
		t = m - p;
		t = t / t_avg;
		if (t != 0x0)
			t = 0x1000 - t;
	}
	dc_re = t;

	m = 0;
	p = 0;
	for (i = 0; i < t_avg; i++) {
		if (im[i] > 0x800)
			m = (0x1000 - im[i]) + m;
		else
			p = im[i] + p;
	}

	if (p > m) {
		t = p - m;
		t = t / t_avg;
	} else {
		t = m - p;
		t = t / t_avg;
		if (t != 0x0)
			t = 0x1000 - t;
	}
	dc_im = t;

	RF_DBG(rf, DBG_RF_DACK, "[DACK]S%d,dc_re = 0x%x,dc_im =0x%x\n",
		path, dc_re, dc_im);
}

void halrf_addck_8852c(struct rf_info *rf)
{
	struct halrf_dack_info *dack = &rf->dack;
	u32 c;
	/*S0*/


	/*0.set one_shot_k_mode*/
	halrf_wreg(rf, 0xc0f4, BIT(2), 0x1);
	/*1.Enable on_shot_k_en*/
	halrf_wreg(rf, 0xc0f4, BIT(4), 0x1);
	halrf_wreg(rf, 0xc0f4, BIT(4), 0x0);
	halrf_delay_us(rf, 1);
	/*check if cal done*/
	halrf_wreg(rf, 0xc0f4, 0x300, 0x1);

	c = 0;

#ifdef HALRF_CONFIG_FW_IO_OFLD_SUPPORT
	if (!halrf_polling_bb(rf, 0xc0fc, BIT(0), 1, 5000)) {
		RF_DBG(rf, DBG_RF_FW, "[IO OFLD][DACK] %s 0xc0fc[0] == 1 Timeout !!!\n", __func__);
		
	} else { //Polling ok
		halrf_delay_us(rf, 10);
		dack->addck_timeout[0] = false;
		RF_DBG(rf, DBG_RF_FW, "[IO OFLD][DACK] %s polling 0xc0fc[0] == 1 in IO queue OK!!!\n", __func__);
	}	
#else	
	while (halrf_rreg(rf, 0xc0fc, BIT(0)) == 0) {
		c++;
		halrf_delay_us(rf, 1);
		if (c > 10000) {
			RF_DBG(rf, DBG_RF_DACK, "[DACK]S0 ADDCK timeout\n");
			dack->addck_timeout[0] = true;
			break;
		}
	}
#endif
	
	RF_DBG(rf, DBG_RF_DACK, "[DACK]ADDCK c = %d\n", c);

	
	/*Disable one_shot_k mode*/
	halrf_wreg(rf, 0xc0f4, BIT(2), 0x0);

	/*S1*/
	/*0.set one_shot_k_mode*/
	halrf_wreg(rf, 0xc1f4, BIT(2), 0x1);
	/*1.Enable on_shot_k_en*/
	halrf_wreg(rf, 0xc1f4, BIT(4), 0x1);
	halrf_wreg(rf, 0xc1f4, BIT(4), 0x0);
	halrf_delay_us(rf, 1);
	/*check if cal done*/
	halrf_wreg(rf, 0xc1f4, 0x300, 0x1);


	c = 0;


#ifdef HALRF_CONFIG_FW_IO_OFLD_SUPPORT
	if (!halrf_polling_bb(rf, 0xc1fc, BIT(0), 1, 5000)) {
		RF_DBG(rf, DBG_RF_FW, "[IO OFLD][DACK] %s 0xc1fc[0] == 1 Timeout !!!\n", __func__);
		
	} else { //Polling ok
		halrf_delay_us(rf, 10);
		RF_DBG(rf, DBG_RF_FW, "[IO OFLD][DACK] %s polling 0xc1fc[0] == 1 in IO queue OK!!!\n", __func__);
	}	
#else	
	while (halrf_rreg(rf, 0xc1fc, BIT(0)) == 0) {
		c++;
		halrf_delay_us(rf, 1);
		if (c > 10000) {
			RF_DBG(rf, DBG_RF_DACK, "[DACK]S1 ADDCK timeout\n");
			dack->addck_timeout[0] = true;
			break;
		}
	}
#endif

	RF_DBG(rf, DBG_RF_DACK, "[DACK]ADDCK c = %d\n", c);
	/*Disable one_shot_k mode*/
	halrf_wreg(rf, 0xc1f4, BIT(2), 0x0);
}

void halrf_check_dadc_8852c(struct rf_info *rf, enum rf_path path)
{
	halrf_wreg(rf, 0x032c, BIT(30), 0x0);
	halrf_wreg(rf, 0x030c, 0x0f000000, 0xf);
	halrf_wreg(rf, 0x030c, 0x0f000000, 0x3);
	halrf_wreg(rf, 0x032c, BIT(16), 0x0);
	if (path == RF_PATH_A) {
		halrf_wreg(rf, 0x12dc, BIT(0), 0x1);
		halrf_wreg(rf, 0x12e8, BIT(2), 0x1);
		halrf_wrf(rf, RF_PATH_A, 0x8f, BIT(13), 0x1);
	} else {
		halrf_wreg(rf, 0x32dc, BIT(0), 0x1);
		halrf_wreg(rf, 0x32e8, BIT(2), 0x1);
		halrf_wrf(rf, RF_PATH_B, 0x8f, BIT(13), 0x1);
	}
	halrf_check_addc_8852c(rf, path);
	if (path == RF_PATH_A) {
		halrf_wreg(rf, 0x12dc, BIT(0), 0x0);
		halrf_wreg(rf, 0x12e8, BIT(2), 0x0);
		halrf_wrf(rf, RF_PATH_A, 0x8f, BIT(13), 0x0);
	} else {
		halrf_wreg(rf, 0x32dc, BIT(0), 0x0);
		halrf_wreg(rf, 0x32e8, BIT(2), 0x0);
		halrf_wrf(rf, RF_PATH_B, 0x8f, BIT(13), 0x0);
	}
	halrf_wreg(rf, 0x032c, BIT(16), 0x1);
}

void halrf_dack_8852c_s0(struct rf_info *rf)
{
	struct halrf_dack_info *dack = &rf->dack;
	u32 c = 0;

#ifdef HALRF_CONFIG_FW_IO_OFLD_SUPPORT /*--FW Offload Start*/
	halrf_write_fwofld_start(rf);		
#endif /*FW Offload Start--*/
	/*step 1: Set clk to 160MHz for calibration  */
	halrf_txck_force_8852c(rf, RF_PATH_A, true, DAC_160M);
	/*step 2: DAC & clk enable */
	halrf_wreg(rf, 0x12b8, BIT(30), 0x1);
	halrf_wreg(rf, 0x030c, BIT(28), 0x1);
	halrf_wreg(rf, 0x032c, 0x80000000, 0x0);

	halrf_wreg(rf, 0xc004, 0xfff00000, 0x30);
	halrf_wreg(rf, 0xc024, 0xfff00000, 0x30);

	halrf_dack_reset_8852c(rf, RF_PATH_A);

	/*step 3: Enable DACK*/
	halrf_wreg(rf, 0xc004, BIT(0), 0x1);
	halrf_delay_us(rf, 1);
#ifdef HALRF_CONFIG_FW_IO_OFLD_SUPPORT /*--FW Offload End*/
	halrf_write_fwofld_end(rf); 	/*FW Offload End*/
#endif /*FW Offload End--*/


	c = 0x0;
	while ((halrf_rreg(rf, 0xc040, BIT(31)) == 0) || (halrf_rreg(rf, 0xc064, BIT(31)) == 0) ||
		(halrf_rreg(rf, 0xc05c, BIT(2)) == 0) || (halrf_rreg(rf, 0xc080, BIT(2)) == 0)) {
		c++;
		halrf_delay_us(rf, 1);
		if (c > 10000) {
			RF_DBG(rf, DBG_RF_DACK, "[DACK]S0 DACK timeout\n");
			dack->msbk_timeout[0] = true;
			break;
		}
	}
	RF_DBG(rf, DBG_RF_DACK, "[DACK]DACK c = %d\n", c);

#ifdef HALRF_CONFIG_FW_IO_OFLD_SUPPORT /*--FW Offload Start*/
	halrf_write_fwofld_start(rf);		
#endif /*FW Offload Start--*/
	/*step 4: disableDACK */
	halrf_wreg(rf, 0xc004, BIT(0), 0x0);
	/*step 5: Set clk to 960MHz for normal mode */
	halrf_txck_force_8852c(rf, RF_PATH_A, false, DAC_960M);
#ifdef HALRF_CONFIG_FW_IO_OFLD_SUPPORT /*--FW Offload End*/
	halrf_write_fwofld_end(rf); 	/*FW Offload End*/
#endif /*FW Offload End--*/	

	
	RF_DBG(rf, DBG_RF_DACK, "[DACK]after S0 DADCK\n");
	/*halrf_check_dadc_8852c(rf, RF_PATH_A);*/

	/*backup here*/
	halrf_dack_backup_s0_8852c(rf);
	halrf_dack_reload_8852c(rf, RF_PATH_A);
	/*step 6: Set DAC & clk to normal mode */
	halrf_wreg(rf, 0x12b8, BIT(30), 0x0);
}

void halrf_dack_8852c_s1(struct rf_info *rf)
{
	struct halrf_dack_info *dack = &rf->dack;
	u32 c = 0;

	/*--FW Offload Start*/
#ifdef HALRF_CONFIG_FW_IO_OFLD_SUPPORT
	halrf_write_fwofld_start(rf);		/*FW Offload Start*/
#endif
	/*FW Offload Start--*/	
			
	/*step 1: Set clk to 160MHz for calibration  */
	halrf_txck_force_8852c(rf, RF_PATH_B, true, DAC_160M);
	/*step 2: DAC & clk enable */
	halrf_wreg(rf, 0x32b8, BIT(30), 0x1);
	halrf_wreg(rf, 0x030c, BIT(28), 0x1);
	halrf_wreg(rf, 0x032c, 0x80000000, 0x0);

	halrf_wreg(rf, 0xc104, 0xfff00000, 0x30);
	halrf_wreg(rf, 0xc124, 0xfff00000, 0x30);

	halrf_dack_reset_8852c(rf, RF_PATH_B);

	/*step 3: Enable DACK*/
	halrf_wreg(rf, 0xc104, BIT(0), 0x1);
	halrf_delay_us(rf, 1);

	/*--FW Offload End*/
#ifdef HALRF_CONFIG_FW_IO_OFLD_SUPPORT
	halrf_write_fwofld_end(rf); 	/*FW Offload End*/
#endif
	/*FW Offload End--*/

	c = 0x0;
	while ((halrf_rreg(rf, 0xc140, BIT(31)) == 0) || (halrf_rreg(rf, 0xc164, BIT(31)) == 0) ||
		(halrf_rreg(rf, 0xc15c, BIT(2)) == 0) || (halrf_rreg(rf, 0xc180, BIT(2)) == 0)) {
		c++;
		halrf_delay_us(rf, 1);
		if (c > 10000) {
			RF_DBG(rf, DBG_RF_DACK, "[DACK]S1 DACK timeout\n");
			dack->msbk_timeout[0] = true;
			break;
		}
	}
	RF_DBG(rf, DBG_RF_DACK, "[DACK]DACK c = %d\n", c);

#ifdef HALRF_CONFIG_FW_IO_OFLD_SUPPORT /*--FW Offload Start*/
	halrf_write_fwofld_start(rf);		
#endif /*FW Offload Start--*/
	/*step 4: disableDACK */
	halrf_wreg(rf, 0xc104, BIT(0), 0x0);
	/*step 5: Set clk to 960MHz for normal mode */
	halrf_txck_force_8852c(rf, RF_PATH_B, false, DAC_960M);
#ifdef HALRF_CONFIG_FW_IO_OFLD_SUPPORT /*--FW Offload End*/
	halrf_write_fwofld_end(rf); 	/*FW Offload End*/
#endif /*FW Offload End--*/


	RF_DBG(rf, DBG_RF_DACK, "[DACK]after S0 DADCK\n");
	/*halrf_check_dadc_8852c(rf, RF_PATH_A);*/

	/*backup here*/
	halrf_dack_backup_s1_8852c(rf);
	halrf_dack_reload_8852c(rf, RF_PATH_B);
	/*step 6: Set DAC & clk to normal mode */
	halrf_wreg(rf, 0x32b8, BIT(30), 0x0);
}

void halrf_dack_8852c(struct rf_info *rf)
{
	halrf_dack_8852c_s0(rf);
	halrf_dack_8852c_s1(rf);
}

void halrf_dack_dump_8852c(struct rf_info *rf)
{
	struct halrf_dack_info *dack = &rf->dack;
	u8 i;
	u8 t;

	RF_DBG(rf, DBG_RF_DACK, "[DACK]S0 ADC_DCK ic = 0x%x, qc = 0x%x\n",
	                    dack->addck_d[0][0], dack->addck_d[0][1] );
	RF_DBG(rf, DBG_RF_DACK, "[DACK]S1 ADC_DCK ic = 0x%x, qc = 0x%x\n",
	                    dack->addck_d[1][0], dack->addck_d[1][1] );
	RF_DBG(rf, DBG_RF_DACK, "[DACK]S0 DAC_DCK ic = 0x%x, qc = 0x%x\n",
	       dack->dadck_d[0][0], dack->dadck_d[0][1] );
	RF_DBG(rf, DBG_RF_DACK, "[DACK]S1 DAC_DCK ic = 0x%x, qc = 0x%x\n",
	       dack->dadck_d[1][0], dack->dadck_d[1][1] );

	RF_DBG(rf, DBG_RF_DACK, "[DACK]S0 biask ic = 0x%x, qc = 0x%x\n",
	       dack->biask_d[0][0], dack->biask_d[0][1] );
	RF_DBG(rf, DBG_RF_DACK, "[DACK]S1 biask ic = 0x%x, qc = 0x%x\n",
	       dack->biask_d[1][0], dack->biask_d[1][1] );

	RF_DBG(rf, DBG_RF_DACK, "[DACK]S0 MSBK ic:\n");
	for (i = 0; i < 0x10; i++) {
		t = dack->msbk_d[0][0][i];
		RF_DBG(rf, DBG_RF_DACK, "[DACK]0x%x\n", t);
	}
	RF_DBG(rf, DBG_RF_DACK, "[DACK]S0 MSBK qc:\n");
	for (i = 0; i < 0x10; i++) {
		t = dack->msbk_d[0][1][i];
		RF_DBG(rf, DBG_RF_DACK, "[DACK]0x%x\n", t);
	}
	RF_DBG(rf, DBG_RF_DACK, "[DACK]S1 MSBK ic:\n");
	for (i = 0; i < 0x10; i++) {
		t = dack->msbk_d[1][0][i];
		RF_DBG(rf, DBG_RF_DACK, "[DACK]0x%x\n", t);
	}
	RF_DBG(rf, DBG_RF_DACK, "[DACK]S1 MSBK qc:\n");
	for (i = 0; i < 0x10; i++) {
		t = dack->msbk_d[1][1][i];
		RF_DBG(rf, DBG_RF_DACK, "[DACK]0x%x\n", t);
	}
}

void halrf_dac_cal_8852c(struct rf_info *rf, bool force)
{
	struct halrf_dack_info *dack = &rf->dack;
	u32 rf0_0, rf1_0;
	u8 phy_map;

	phy_map = (BIT(HW_PHY_0) << 4) | RF_AB;
#if 0
	if (dack->dack_en) {
		if (!force) {
			halrf_dack_reload_8852a(rf);
			RF_DBG(rf, DBG_RF_DACK, "[DACK]reload dack value\n");
			return;
		}
	} else {
		dack->dack_en = true;
	}
#endif
	dack->dack_done = false;
	RF_DBG(rf, DBG_RF_DACK, "[DACK]DACK 0x1\n");
	RF_DBG(rf, DBG_RF_DACK, "[DACK]DACK start!!!\n");	
	rf0_0 = halrf_rrf(rf,RF_PATH_A, 0x0, MASKRF);
	rf1_0 = halrf_rrf(rf,RF_PATH_B, 0x0, MASKRF);
	
	halrf_afe_init_8852c(rf);

	
#ifdef HALRF_CONFIG_FW_IO_OFLD_SUPPORT /*--FW Offload Start*/
	halrf_write_fwofld_start(rf);		
#endif /*FW Offload Start--*/

	halrf_drck_8852c(rf);
	halrf_wrf(rf, RF_PATH_A, 0x5, BIT(0), 0x0);
	halrf_wrf(rf, RF_PATH_B, 0x5, BIT(0), 0x0);
	halrf_wrf(rf, RF_PATH_A, 0x0, MASKRF, 0x337e1);
	halrf_wrf(rf, RF_PATH_B, 0x0, MASKRF, 0x337e1);

#ifdef HALRF_CONFIG_FW_IO_OFLD_SUPPORT /*--FW Offload End*/
	halrf_write_fwofld_end(rf); 	/*FW Offload End*/
#endif /*FW Offload End--*/

	

	halrf_btc_rfk_ntfy(rf, phy_map, RF_BTC_DACK, RFK_ONESHOT_START);

#ifdef HALRF_CONFIG_FW_IO_OFLD_SUPPORT /*--FW Offload Start*/
	halrf_write_fwofld_start(rf);		
#endif /*FW Offload Start--*/
	halrf_addck_8852c(rf);
#ifdef HALRF_CONFIG_FW_IO_OFLD_SUPPORT /*--FW Offload End*/
	halrf_write_fwofld_end(rf); 	/*FW Offload End*/
#endif /*FW Offload End--*/


	halrf_btc_rfk_ntfy(rf, phy_map, RF_BTC_DACK, RFK_ONESHOT_STOP);
	halrf_addck_backup_8852c(rf);

#ifdef HALRF_CONFIG_FW_IO_OFLD_SUPPORT /*--FW Offload Start*/
	halrf_write_fwofld_start(rf);		
#endif /*FW Offload Start--*/
	halrf_addck_reload_8852c(rf);
	halrf_wrf(rf, RF_PATH_A, 0x1, MASKRF, 0x0);
	halrf_wrf(rf, RF_PATH_B, 0x1, MASKRF, 0x0);
#ifdef HALRF_CONFIG_FW_IO_OFLD_SUPPORT /*--FW Offload End*/
	halrf_write_fwofld_end(rf); 	/*FW Offload End*/
#endif /*FW Offload End--*/	

	halrf_btc_rfk_ntfy(rf, phy_map, RF_BTC_DACK, RFK_ONESHOT_START);
	halrf_dack_8852c(rf);
	halrf_btc_rfk_ntfy(rf, phy_map, RF_BTC_DACK, RFK_ONESHOT_STOP);
	halrf_dack_dump_8852c(rf);
	dack->dack_done = true;

	/*--FW Offload Start*/
#ifdef HALRF_CONFIG_FW_IO_OFLD_SUPPORT
	halrf_write_fwofld_start(rf);		/*FW Offload Start*/
#endif
	/*FW Offload Start--*/		
	halrf_wrf(rf, RF_PATH_A, 0x0, MASKRF, rf0_0);
	halrf_wrf(rf, RF_PATH_B, 0x0, MASKRF, rf1_0);
	halrf_wrf(rf, RF_PATH_A, 0x5, BIT(0), 0x1);
	halrf_wrf(rf, RF_PATH_B, 0x5, BIT(0), 0x1);
	/*--FW Offload End*/
#ifdef HALRF_CONFIG_FW_IO_OFLD_SUPPORT
	halrf_write_fwofld_end(rf);		/*FW Offload End*/
#endif
	/*FW Offload End--*/

	dack->dack_cnt++;
	RF_DBG(rf, DBG_RF_DACK, "[DACK]DACK finish!!!\n");
}

bool halrf_dack_reg_check_fail_8852c(struct rf_info *rf)
{
#if 1
	struct halrf_dack_info *dack = &rf->dack;
	u16 addck_d[2][2]; 
	u8 msbk_d[2][2][16];
	u8 dadck_d[2][2];
	u16 biask_d[2][2];	
	u32 i, j, p;
	bool fail = false;

	halrf_wreg(rf, 0x12b8, BIT(30), 0x1);
	halrf_wreg(rf, 0x32b8, BIT(30), 0x1);

	
	addck_d[0][0] = (u16)halrf_rreg(rf, 0xc0fc, 0xffc00);
	addck_d[0][1] = (u16)halrf_rreg(rf, 0xc0fc, 0x003ff);
	addck_d[1][0] =(u16) halrf_rreg(rf, 0xc1fc, 0xffc00);
	addck_d[1][1] = (u16)halrf_rreg(rf, 0xc1fc, 0x003ff);

	biask_d[0][0] = (u16)halrf_rreg(rf, 0xc048, 0xffc);
	biask_d[0][1] = (u16)halrf_rreg(rf, 0xc06c, 0xffc);
	biask_d[1][0] = (u16)halrf_rreg(rf, 0xc148, 0xffc);
	biask_d[1][1] = (u16)halrf_rreg(rf, 0xc16c, 0xffc);

	dadck_d[0][0] = (u8)halrf_rreg(rf, 0xc060, 0xff000000);
	dadck_d[0][1] = (u8)halrf_rreg(rf, 0xc084, 0xff000000);
	dadck_d[0][0] = (u8)halrf_rreg(rf, 0xc160, 0xff000000);
	dadck_d[0][1] = (u8)halrf_rreg(rf, 0xc184, 0xff000000);

	for (i = 0; i < 0x10; i++) {
		/*S0*/
		halrf_wreg(rf, 0xc000, 0x1e, i);
		msbk_d[0][0][i] = (u8)halrf_rreg(rf, 0xc05c, 0xff000000);
		halrf_wreg(rf, 0xc020, 0x1e, i);
		msbk_d[0][1][i] = (u8)halrf_rreg(rf, 0xc080, 0xff000000);
	}

	for (i = 0; i < 0x10; i++) {
		/*S1*/
		halrf_wreg(rf, 0xc100, 0x1e, i);
		dack->msbk_d[1][0][i] = (u8)halrf_rreg(rf, 0xc15c, 0xff000000);
		halrf_wreg(rf, 0xc120, 0x1e, i);
		dack->msbk_d[1][1][i] = (u8)halrf_rreg(rf, 0xc180, 0xff000000);
	}

	halrf_wreg(rf, 0x12b8, BIT(30), 0x0);
	halrf_wreg(rf, 0x32b8, BIT(30), 0x0);

	for (p = 0; p < 2; p++) {
		for (i = 0;i < 2; i++) {
			for (j = 0; j < 16; j ++) {
				if (msbk_d[p][i][j] != dack->msbk_d[p][i][j]) {
					RF_DBG(rf, DBG_RF_RFK, "[RFK]msbk_d[%d][%d][%d] mismatch\n", p, i, j);
					fail = true;
				}
			}
		}
	}

	for (p =0; p < 2; p++) {
		for (i = 0; i < 2; i++) {
			if (biask_d[p][i] != dack->biask_d[p][i]) {
				RF_DBG(rf, DBG_RF_RFK, "[RFK]biask_d[%d][%d] mismatch\n", p, i);
				fail = true;
			}
		}
	}


	for (p =0; p < 2; p++) {
		for (i = 0; i < 2; i++) {
			if (addck_d[p][i] != dack->addck_d[p][i]) {
				RF_DBG(rf, DBG_RF_RFK, "[RFK]addck_d[%d][%d] mismatch\n", p, i);
				fail = true;
			}
		}
	}

	return fail;
#endif
}
#endif


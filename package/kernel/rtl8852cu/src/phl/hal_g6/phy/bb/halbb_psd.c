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

#ifdef HALBB_PSD_SUPPORT
static const u8 psd_result_cali_tone_8821[7] = {21, 28, 33, 93, 98, 105, 127};
static const u8 psd_result_cali_val_8821[7] = {67, 69, 71, 72, 71, 69, 67};

void halbb_psd_igi_lv(struct bb_info *bb, enum igi_lv_sel igi_lv)
{
	struct bb_psd_info *psd = &bb->bb_cmn_hooker->bb_psd_i;
	/*struct agc_gaincode_set *set;*/

	if (igi_lv == IGI_DEFAULT) {
		psd->igi_pin = 28;
		halbb_set_igi(bb, psd->lna_bkp, psd->tia_bkp, psd->rxbb_bkp, RF_PATH_A);
		halbb_set_igi(bb, psd->lna_bkp_b, psd->tia_bkp_b, psd->rxbb_bkp_b, RF_PATH_B);
	} else if (igi_lv == IGI_LEVEL1) {/*@ target Pin=-75dBm*/
		psd->igi_pin = 35;
		halbb_set_igi(bb, 0x6, 0x1, 0x1f, RF_PATH_A);
		halbb_set_igi(bb, 0x6, 0x1, 0x1f, RF_PATH_B);
	} else if (igi_lv == IGI_LEVEL2) {/*@ target Pin=-60dBm*/
		psd->igi_pin = 50;
		halbb_set_igi(bb, 0x6, 0x1, 0x10, RF_PATH_A);
		halbb_set_igi(bb, 0x6, 0x1, 0x10, RF_PATH_B);
	} else if (igi_lv == IGI_LEVEL3) {/*@ target Pin=-40dBm*/
		psd->igi_pin = 70;
		halbb_set_igi(bb, 0x5, 0x0, 0xa, RF_PATH_A);
		halbb_set_igi(bb, 0x5, 0x0, 0xa, RF_PATH_B);
	} else if (igi_lv == IGI_LEVEL4) {/*@ target Pin=-30dBm*/
		psd->igi_pin = 80;
		halbb_set_igi(bb, 0x4, 0x0, 0x8, RF_PATH_A);
		halbb_set_igi(bb, 0x4, 0x0, 0x8, RF_PATH_B);
	} else if (igi_lv == IGI_LEVEL5) {/*@ target Pin=-20dBm*/
		psd->igi_pin = 90;
		halbb_set_igi(bb, 0x2, 0x0, 0xe, RF_PATH_A);
		halbb_set_igi(bb, 0x2, 0x0, 0xe, RF_PATH_B);
	} else if (igi_lv == IGI_LEVEL6) {/*@ target Pin=-10dBm*/
		psd->igi_pin = 100;
		halbb_set_igi(bb, 0x1, 0x0, 0xc, RF_PATH_A);
		halbb_set_igi(bb, 0x1, 0x0, 0xc, RF_PATH_B);
	} else if (igi_lv == IGI_LEVEL7) {/*@ target Pin=0dBm*/
		psd->igi_pin = 110;
		halbb_set_igi(bb, 0x0, 0x0, 0xb, RF_PATH_A);
		halbb_set_igi(bb, 0x0, 0x0, 0xb, RF_PATH_B);
	}
}

u32 halbb_get_psd_data(struct bb_info *bb, u32 psd_tone_idx, u32 igi_pin)
{
	struct bb_psd_info *psd = &bb->bb_cmn_hooker->bb_psd_i;
	struct bb_psd_cr_info *cr = &psd->bb_psd_cr_i;
	u8 i = 0;
	u8 j = 0;
	u32 psd_report = 0;
	bool psd_rdy = false;

	halbb_set_reg(bb, cr->psd_fft_idx, cr->psd_fft_idx_m, psd_tone_idx & 0x7ff);
	/*PSD trigger start*/
	halbb_set_reg(bb, cr->psd_en, cr->psd_en_m, 1);
	halbb_delay_us(bb, 10 << (psd->fft_point >> 7));

	/*Get PSD Report*/
	do { /*Polling time always use 100ms, when it exceed 2s, break loop*/
		psd_rdy = (u8)halbb_get_reg(bb, cr->psd_ok_flag, cr->psd_ok_flag_m);

		if (!psd_rdy) {
			halbb_delay_us(bb, 100);
			i++;
			continue;
		} else {
			/*BB_DBG(bb, DBG_DBG_API, "psd_rdy = %d\n", psd_rdy);*/
			psd_report = halbb_get_reg(bb, cr->psd_pw, cr->psd_pw_m);/*ABS value*/
			/*
			psd_report = psd_report >> 19; // U(25,19)
			psd_report = 2*halbb_convert_to_db((u64)psd_report) //+ igi_pin;
			*/
			break;
		}
		psd_report = 0;
	} while (i < 20);

	/*PSD trigger stop*/
	halbb_set_reg(bb, cr->psd_en, cr->psd_en_m, 0);

	do { /*Polling time always use 100ms, when it exceed 2s, break loop*/
		psd_rdy = (u8)halbb_get_reg(bb, cr->psd_ok_flag, cr->psd_ok_flag_m);

		if (psd_rdy) {
			/*BB_TRACE("psd_rdy2 = %d, wait %d * 100us for release \n", psd_rdy, (j + 1));*/
			halbb_delay_us(bb, 100);
			j++;
			continue;
		} else {

			break;
		}
	} while (j < 20);

	return psd_report;
}

void halbb_psd_para_setting(struct bb_info *bb, u8 sw_avg_time, u8 hw_avg_time,
			    u8 i_q_setting, u16 fft_point, u8 path,
			    u8 psd_input, u8 pri_ch, u8 channel, u8 bw, enum band_type band)
{
	struct bb_psd_info *psd = &bb->bb_cmn_hooker->bb_psd_i;
	struct bb_psd_cr_info *cr = &psd->bb_psd_cr_i;
	u8 n_dft_idx = 0;

	psd->fft_point_pre = psd->fft_point;
	psd->fft_point = fft_point;

	if (sw_avg_time == 0)
		sw_avg_time = 1;

	psd->sw_avg_time = sw_avg_time;
	psd->hw_avg_time = hw_avg_time;
	psd->i_q_setting = i_q_setting;
	psd->path = path;
	psd->input = psd_input;
	psd->pri_ch = pri_ch;
	psd->fc_ch = channel;
	psd->bw = bw;
	psd->band = band;

	if (fft_point == 128)
		n_dft_idx = 3;
	else if (fft_point == 256)
		n_dft_idx = 2;
	else if (fft_point == 512)
		n_dft_idx = 1;
	else if (fft_point == 1024)
		n_dft_idx = 0;

	halbb_set_reg(bb, cr->psd_n_dft, cr->psd_n_dft_m, n_dft_idx);
	halbb_set_reg(bb, cr->psd_iq_sel, cr->psd_iq_sel_m, i_q_setting);
	halbb_set_reg(bb, cr->psd_l_avg, cr->psd_l_avg_m, hw_avg_time);
	halbb_set_reg(bb, cr->psd_path_sel, cr->psd_path_sel_m, path);
	halbb_set_reg(bb, cr->psd_source_sel, cr->psd_source_sel_m, psd_input);
}

bool halbb_psd_alloc_buff(struct bb_info *bb)
{
	struct bb_psd_info *psd = &bb->bb_cmn_hooker->bb_psd_i;
	u32 psd_len;
	
	psd_len = psd->fft_point * sizeof(psd_len);

	if (!psd->rpt) {
		psd->rpt = halbb_mem_alloc(bb, psd_len);
	} else if (psd->rpt && psd->fft_point != psd->fft_point_pre) {
		halbb_mem_free(bb, psd->rpt, sizeof(psd->rpt));
		psd->rpt = halbb_mem_alloc(bb, psd_len);
	}

	if (!psd->rpt)
		return false;
	else
		return true;
}

void halbb_get_gain_index(struct bb_info *bb)
{
	struct bb_psd_info *psd = &bb->bb_cmn_hooker->bb_psd_i;

	psd->lna_bkp = halbb_get_lna_idx(bb, RF_PATH_A);
	psd->lna_bkp_b = halbb_get_lna_idx(bb, RF_PATH_B);
	psd->tia_bkp = halbb_get_tia_idx(bb, RF_PATH_A);
	psd->tia_bkp_b = halbb_get_tia_idx(bb, RF_PATH_B);
	psd->rxbb_bkp = halbb_get_rxb_idx(bb, RF_PATH_A);
	psd->rxbb_bkp_b = halbb_get_rxb_idx(bb, RF_PATH_B);

}

u8 halbb_psd(struct bb_info *bb, enum igi_lv_sel igi_lv, u16 start_point,
	     u16 stop_point)
{
	struct bb_psd_info *psd = &bb->bb_cmn_hooker->bb_psd_i;
	struct bb_api_info *api = &bb->bb_api_i;
	struct bb_psd_cr_info *cr = &psd->bb_psd_cr_i;
	struct rtw_hal_com_t *hal_com = bb->hal_com;
	u32 i = 0, mod_tone_idx = 0;
	u32 t = 0;
	u16 fft_max_half_bw = 0;
	u16 fail_cnt = 0;
	u16 valid_cnt = 0;
	u8 psd_fc_ch = psd->fc_ch;
	enum band_type band = psd->band;
	u8 bw = psd->bw;
	u32 psd_result_tmp = 0;
	u32 psd_result_total = 0;
	u32 psd_result = 0;
	u8 psd_result_cali_tone[7] = {0};
	u8 psd_result_cali_val[7] = {0};
	u8 noise_idx = 0;
	u8 set_result = 0;
	u8 psd_pri_ch = psd->pri_ch;
	u8 central_ch_seg1 = 0;
	u8 fc_ch_bk = api->central_ch;
	enum band_type band_bk = api->band;
	u8 bw_bk = api->bw;
	u8 pri_ch_bk = api->pri_ch_idx;

	if (bb->hal_com->chip_id == CHIP_WIFI6_8852A) {
		halbb_mem_cpy(bb, psd_result_cali_tone,
			      psd_result_cali_tone_8821, 7);
		halbb_mem_cpy(bb, psd_result_cali_val,
			      psd_result_cali_val_8821, 7);
	}

	psd->psd_in_progress = 1;
	halbb_set_reg(bb, cr->psd_clk_en, cr->psd_clk_en_m, 1);

	BB_TRACE(" PSD_Start =>\n");
	/* @[Get default gain value]*/
	halbb_get_gain_index(bb);
	/* @[Stop pre-AGC]*/
	/* @[Set High IGI level & make it can't CCA]*/
	halbb_pre_agc_en(bb, false);
	halbb_psd_igi_lv(bb, IGI_LEVEL7);

	halbb_delay_us(bb, 10);
	BB_TRACE(" sw_avg_time = %d , hw_avg_time = %d , IQ = %d , fft_point = %d , path = %d , input = %d , ch = %d , BW = %d, Band = %d\n",
		 psd->sw_avg_time, psd->hw_avg_time, psd->i_q_setting,
		 psd->fft_point, psd->path, psd->input, psd_fc_ch, bw, band);

	if (halbb_stop_ic_trx(bb, HALBB_SET) == HALBB_SET_FAIL) {
		/*@[resume pre-AGC and resume IGI level]*/
		halbb_pre_agc_en(bb, true);
		halbb_psd_igi_lv(bb, IGI_DEFAULT);
		return HALBB_SET_FAIL;
	}

	/* @[Set IGI level]*/
	halbb_psd_igi_lv(bb, igi_lv);

	/* @[Backup RF Reg]*/
	psd->rf_0x18_bkp = halbb_read_rf_reg(bb, RF_PATH_A, 0x18,
						RFREGOFFSETMASK);
	psd->rf_0x18_bkp_b = halbb_read_rf_reg(bb, RF_PATH_B, 0x18,
						  RFREGOFFSETMASK);

	if (psd->input == 0) {
		psd->psd_bw = 40;
	} else if (psd->input == 1){
		if (bw == CHANNEL_WIDTH_20 || bw == CHANNEL_WIDTH_40)
			psd->psd_bw = 40;
		else
			psd->psd_bw = 80;
	} else if ((psd->input == 2) || (psd->input == 3)) {
		if (bw == CHANNEL_WIDTH_20 || bw == CHANNEL_WIDTH_40)
			psd->psd_bw = 80;
		else
			psd->psd_bw = 160;
	}
	BB_DBG(bb, DBG_DBG_API, " PSD_BW = %d\n", psd->psd_bw);


	/* Set RF fc*/
	if(halbb_ctrl_bw_ch(bb, psd_pri_ch, psd_fc_ch, central_ch_seg1, band, bw, HW_PHY_0) == false) {
		BB_DBG(bb, DBG_DBG_API, "halbb_ctrl_bw_ch PHY0 fail!\n");
		return HALBB_SET_FAIL;
	}
	if(halbb_ctrl_bw_ch(bb, psd_pri_ch, psd_fc_ch, central_ch_seg1, band, bw, HW_PHY_1) == false) {
		BB_DBG(bb, DBG_DBG_API, "halbb_ctrl_bw_ch PHY1 fail!\n");
		return HALBB_SET_FAIL;
	}

	if(rtw_hal_rf_set_ch_bw(hal_com, HW_PHY_0, psd_fc_ch, band, bw) != RTW_HAL_STATUS_SUCCESS) {
		BB_DBG(bb, DBG_DBG_API, "rtw_hal_rf_set_ch_bw PHY0 fail!\n");
		return HALBB_SET_FAIL;
	}
	if(rtw_hal_rf_set_ch_bw(hal_com, HW_PHY_1, psd_fc_ch, band, bw) != RTW_HAL_STATUS_SUCCESS) {
		BB_DBG(bb, DBG_DBG_API, "rtw_hal_rf_set_ch_bw PHY01 fail!\n");
		return HALBB_SET_FAIL;
	}
	if(rtw_hal_rf_chl_rfk_trigger(hal_com, HW_PHY_0, true) != RTW_HAL_STATUS_SUCCESS) {
		BB_DBG(bb, DBG_DBG_API, "rtw_hal_rf_chl_rfk_trigger PHY0 fail!\n");
		return HALBB_SET_FAIL;
	}
	if(rtw_hal_rf_chl_rfk_trigger(hal_com, HW_PHY_1, true) != RTW_HAL_STATUS_SUCCESS) {
		BB_DBG(bb, DBG_DBG_API, "rtw_hal_rf_chl_rfk_trigger PHY1 fail!\n");
		return HALBB_SET_FAIL;
	}

	BB_DBG(bb, DBG_DBG_API, "RF0x18=((0x%x))\n",
	       halbb_read_rf_reg(bb, RF_PATH_A, 0x18, RFREGOFFSETMASK));
	/*check IQK setting*/
	BB_DBG(bb, DBG_DBG_API, "0x813c=((0x%x)), 0x823c=((0x%x))\n",
	       halbb_get_reg(bb, 0x813c, MASKDWORD), halbb_get_reg(bb, 0x823c, MASKDWORD));
	BB_DBG(bb, DBG_DBG_API, "0x8124=((0x%x)), 0x8224=((0x%x))\n",
	       halbb_get_reg(bb, 0x8124, MASKDWORD), halbb_get_reg(bb, 0x8224, MASKDWORD));

	halbb_delay_us(bb, 10);

	if (stop_point > (psd->fft_point - 1))
		stop_point = (psd->fft_point - 1);

	if (start_point > (psd->fft_point - 1))
		start_point = (psd->fft_point - 1);

	if (start_point > stop_point)
		stop_point = start_point;

	for (i = start_point; i <= stop_point; i++) {
		
		fft_max_half_bw = (psd->fft_point) >> 1;

		if (i < fft_max_half_bw)
			mod_tone_idx = i + fft_max_half_bw;
		else
			mod_tone_idx = i - fft_max_half_bw;

		psd_result_tmp = 0;
		psd_result_total = 0;
		fail_cnt = 0;
		for (t = 0; t < psd->sw_avg_time; t++) {
			psd_result_tmp = halbb_get_psd_data(bb, mod_tone_idx,
							    psd->igi_pin);
			if (psd_result_tmp == 0)
				fail_cnt ++;
			psd_result_total += psd_result_tmp;
		}
		if (psd->sw_avg_time > fail_cnt)
			valid_cnt = psd->sw_avg_time - fail_cnt;
		else
			valid_cnt = psd->sw_avg_time;
		psd_result = (psd_result_total / valid_cnt) -
			     psd->psd_pwr_common_offset;
		psd->rpt[i] = psd_result;

		if (psd->fft_point == 128 && psd->noise_k_en) {
			if (i > psd_result_cali_tone[noise_idx])
				noise_idx++;

			if (noise_idx > 6)
				noise_idx = 6;

			if (psd_result >= psd_result_cali_val[noise_idx])
				psd_result = psd_result -
					     psd_result_cali_val[noise_idx];
			else
				psd_result = 0;

			psd->psd_result[i] = psd_result;
		}

		BB_DBG(bb, DBG_DBG_API, "[%-4d] N_cali = %d, PSD = %d\n",
		       mod_tone_idx, psd_result_cali_val[noise_idx],
		       psd_result);
	}

	/*@[Revert Reg]*/
	set_result = halbb_stop_ic_trx(bb, HALBB_REVERT);

	halbb_ctrl_bw_ch(bb, pri_ch_bk, fc_ch_bk, central_ch_seg1, band_bk, bw_bk, HW_PHY_0);
	halbb_ctrl_bw_ch(bb, pri_ch_bk, fc_ch_bk, central_ch_seg1, band_bk, bw_bk, HW_PHY_1);
	rtw_hal_rf_set_ch_bw(hal_com, HW_PHY_0, fc_ch_bk, band_bk, bw_bk);
	rtw_hal_rf_set_ch_bw(hal_com, HW_PHY_1, fc_ch_bk, band_bk, bw_bk);

	BB_DBG(bb, DBG_DBG_API, "Restore RF0x18=((0x%x))\n",
	       halbb_read_rf_reg(bb, RF_PATH_A, 0x18, RFREGOFFSETMASK));

	BB_DBG(bb, DBG_DBG_API, " PSD_finish\n\n");

	/*@[Resume pre-AGC and IGI level]*/
	halbb_pre_agc_en(bb, true);
	halbb_psd_igi_lv(bb, IGI_DEFAULT);

	psd->psd_in_progress = 0;

	return HALBB_SET_SUCCESS;
}

void halbb_psd_dbg(struct bb_info *bb, char input[][16], u32 *_used,
		   char *output, u32 *_out_len)
{
	struct bb_psd_info *psd = &bb->bb_cmn_hooker->bb_psd_i;
	char help[] = "-h";
	u32 var1[11] = {0};
	u32 used = *_used;
	u32 out_len = *_out_len;
	u8 i = 0;

	HALBB_SCAN(input[1], DCMD_DECIMAL, &var1[0]);

	if ((_os_strcmp(input[1], help) == 0)) {
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			    "{0} {sw_avg} {hw_avg 0:3} {0:I,1:Q,2:IQ} {fft_point: 128/256/512/1024}\n{path_sel 0~3} {0: dout_sub20_0_40M(E), 1: ccx_in(C), 2: dout_cfir(B), 3: rxdib_in(A)} {pri_CH} {fc_CH} {BW 20/40/80: 0/1/2} {band 2G/5G/6G: 0/1/2}\n\n");

		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			    "{1} show all psd results\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			    "{2} {IGI_lv(0:7)} {start_point} {stop_point}\n");

	} else if (var1[0] == 0) {

		for (i = 1; i <= 10; i++) {
			if (input[i + 1])
				HALBB_SCAN(input[i + 1], DCMD_DECIMAL,
					   &var1[i]);
		}
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			    "sw_avg_time=((%d)), hw_avg_time=((%d)), IQ=((%d)), fft=((%d)), path=((%d)), input =((%d)), pri_ch=((%d)), fc_ch=((%d)), bw=((%d)), band=((%d))\n",
			    var1[1], var1[2], var1[3], var1[4], var1[5],
			    var1[6], (u8)var1[7], (u8)var1[8], (u8)var1[9], (enum band_type)var1[10]);

		halbb_psd_para_setting(bb, (u8)var1[1], (u8)var1[2],
				       (u8)var1[3], (u16)var1[4], (u8)var1[5],
				       (u8)var1[6], (u8)var1[7], (u8)var1[8], (u8)var1[9], (enum band_type)var1[10]);

	} else if (var1[0] == 1) {
		HALBB_SCAN(input[2], DCMD_DECIMAL, &var1[1]);
		HALBB_SCAN(input[3], DCMD_DECIMAL, &var1[2]);
		HALBB_SCAN(input[4], DCMD_DECIMAL, &var1[3]);
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			    "IGI_lv=((0x%x)), start_point=((%d)), stop_point=((%d))\n",
			    var1[1], var1[2], var1[3]);

		bb->dbg_component |= DBG_DBG_API;
		
		if (!halbb_psd_alloc_buff(bb)) {
			BB_DBG_CNSL(out_len, used, output + used,
				    out_len - used, "memory alloc FAIL\n");
			return;
		}

		if (halbb_psd(bb, IGI_LEVEL4, 0, psd->fft_point) == HALBB_SET_FAIL)
			BB_DBG_CNSL(out_len, used, output + used,
				    out_len - used, "PSD_SET_FAIL\n");

		bb->dbg_component &= ~(DBG_DBG_API);
	} else if (var1[0] == 2) {
		HALBB_SCAN(input[2], DCMD_DECIMAL, &var1[1]);
		HALBB_SCAN(input[3], DCMD_DECIMAL, &var1[2]);
		HALBB_SCAN(input[4], DCMD_DECIMAL, &var1[3]);
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			    "IGI_lv=((0x%x)), start_point=((%d)), stop_point=((%d))\n",
			    var1[1], var1[2], var1[3]);

		bb->dbg_component |= DBG_DBG_API;
		
		if (!halbb_psd_alloc_buff(bb)) {
			BB_DBG_CNSL(out_len, used, output + used,
				    out_len - used, "memory alloc FAIL\n");
			return;
		}

		if (halbb_psd(bb, (enum igi_lv_sel)var1[1], (u16)var1[2],
		    (u16)var1[3]) == HALBB_SET_FAIL)
			BB_DBG_CNSL(out_len, used, output + used,
				    out_len - used, "PSD_SET_FAIL\n");

		bb->dbg_component &= ~(DBG_DBG_API);
	}

	*_used = used;
	*_out_len = out_len;
}

void halbb_psd_init(struct bb_info *bb)
{
	struct bb_psd_info *psd = &bb->bb_cmn_hooker->bb_psd_i;

	BB_DBG(bb, DBG_DBG_API, "[%s]\n", __func__);
	psd->psd_in_progress = false;
	psd->psd_pwr_common_offset = 0;
	psd->fft_point_pre = 0;
	halbb_psd_para_setting(bb, 1, 3, 2, 512, 0, 3, 36, 42, 2, 1);
}

void halbb_psd_deinit(struct bb_info *bb)
{
	struct bb_psd_info *psd = &bb->bb_cmn_hooker->bb_psd_i;

	if (psd->rpt)
		halbb_mem_free(bb, psd->rpt, sizeof(psd->rpt));
}

bool halbb_get_psd_result(struct bb_info *bb, u32 *psd_data, u16 *psd_len)
{
	struct bb_psd_info *psd = &bb->bb_cmn_hooker->bb_psd_i;

	if (!psd->rpt)
		return false;

	psd_data = psd->rpt;
	*psd_len = psd->fft_point;
	return true;
}

void halbb_cr_cfg_psd_init(struct bb_info *bb)
{
	struct bb_psd_cr_info *cr = &bb->bb_cmn_hooker->bb_psd_i.bb_psd_cr_i;

	switch (bb->cr_type) {

	#ifdef HALBB_COMPILE_AP_SERIES
	case BB_AP:
		cr->psd_clk_en = PSD_CKEN_A;
		cr->psd_clk_en_m = PSD_CKEN_A_M;
		cr->psd_pw = PSD_PW_A;
		cr->psd_pw_m = PSD_PW_A_M;
		cr->psd_ok_flag = PSD_OK_FLAG_A;
		cr->psd_ok_flag_m = PSD_OK_FLAG_A_M;
		cr->psd_fft_idx = PSD_FFT_IDX_A;
		cr->psd_fft_idx_m = PSD_FFT_IDX_A_M;
		cr->psd_iq_sel = PSD_IQ_SEL_A;
		cr->psd_iq_sel_m = PSD_IQ_SEL_A_M;
		cr->psd_l_avg = PSD_L_AVG_A;
		cr->psd_l_avg_m = PSD_L_AVG_A_M;
		cr->psd_n_dft = PSD_N_DFT_A;
		cr->psd_n_dft_m = PSD_N_DFT_A_M;
		cr->psd_path_sel = PSD_IN_PATH_SEL_A;
		cr->psd_path_sel_m = PSD_IN_PATH_SEL_A_M;
		cr->psd_source_sel = PSD_IN_SOURCE_SEL_A;
		cr->psd_source_sel_m = PSD_IN_SOURCE_SEL_A_M;
		cr->psd_en = PSD_ENABLE_A;
		cr->psd_en_m = PSD_ENABLE_A_M;
		break;

	#endif
	#ifdef HALBB_COMPILE_CLIENT_SERIES
	case BB_CLIENT:
		cr->psd_clk_en = PSD_CKEN_C;
		cr->psd_clk_en_m = PSD_CKEN_C_M;
		cr->psd_pw = PSD_PW_C;
		cr->psd_pw_m = PSD_PW_C_M;
		cr->psd_ok_flag = PSD_OK_FLAG_C;
		cr->psd_ok_flag_m = PSD_OK_FLAG_C_M;
		cr->psd_fft_idx = PSD_FFT_IDX_C;
		cr->psd_fft_idx_m = PSD_FFT_IDX_C_M;
		cr->psd_iq_sel = PSD_IQ_SEL_C;
		cr->psd_iq_sel_m = PSD_IQ_SEL_C_M;
		cr->psd_l_avg = PSD_L_AVG_C;
		cr->psd_l_avg_m = PSD_L_AVG_C_M;
		cr->psd_n_dft = PSD_N_DFT_C;
		cr->psd_n_dft_m = PSD_N_DFT_C_M;
		cr->psd_path_sel = PSD_IN_PATH_SEL_C;
		cr->psd_path_sel_m = PSD_IN_PATH_SEL_C_M;
		cr->psd_source_sel = PSD_IN_SOURCE_SEL_C;
		cr->psd_source_sel_m = PSD_IN_SOURCE_SEL_C_M;
		cr->psd_en = PSD_ENABLE_C;
		cr->psd_en_m = PSD_ENABLE_C_M;
		break;
	#endif
	#ifdef HALBB_COMPILE_AP2_SERIES
	case BB_AP2:
		cr->psd_clk_en = PSD_CKEN_A2;
		cr->psd_clk_en_m = PSD_CKEN_A2_M;
		cr->psd_pw = PSD_PW_A2;
		cr->psd_pw_m = PSD_PW_A2_M;
		cr->psd_ok_flag = PSD_OK_FLAG_A2;
		cr->psd_ok_flag_m = PSD_OK_FLAG_A2_M;
		cr->psd_fft_idx = PSD_FFT_IDX_A2;
		cr->psd_fft_idx_m = PSD_FFT_IDX_A2_M;
		cr->psd_iq_sel = PSD_IQ_SEL_A2;
		cr->psd_iq_sel_m = PSD_IQ_SEL_A2_M;
		cr->psd_l_avg = PSD_L_AVG_A2;
		cr->psd_l_avg_m = PSD_L_AVG_A2_M;
		cr->psd_n_dft = PSD_N_DFT_A2;
		cr->psd_n_dft_m = PSD_N_DFT_A2_M;
		cr->psd_path_sel = PSD_IN_PATH_SEL_A2;
		cr->psd_path_sel_m = PSD_IN_PATH_SEL_A2_M;
		cr->psd_source_sel = PSD_IN_SOURCE_SEL_A2;
		cr->psd_source_sel_m = PSD_IN_SOURCE_SEL_A2_M;
		cr->psd_en = PSD_ENABLE_A2;
		cr->psd_en_m = PSD_ENABLE_A2_M;
		break;
	#endif
	#ifdef HALBB_COMPILE_BE0_SERIES
	case BB_BE0:
		cr->psd_clk_en = PSD_CKEN_BE0;
		cr->psd_clk_en_m = PSD_CKEN_BE0_M;
		cr->psd_pw = PSD_PW_BE0;
		cr->psd_pw_m = PSD_PW_BE0_M;
		cr->psd_ok_flag = PSD_OK_FLAG_BE0;
		cr->psd_ok_flag_m = PSD_OK_FLAG_BE0_M;
		cr->psd_fft_idx = PSD_FFT_IDX_BE0;
		cr->psd_fft_idx_m = PSD_FFT_IDX_BE0_M;
		cr->psd_iq_sel = PSD_IQ_SEL_BE0;
		cr->psd_iq_sel_m = PSD_IQ_SEL_BE0_M;
		cr->psd_l_avg = PSD_L_AVG_BE0;
		cr->psd_l_avg_m = PSD_L_AVG_BE0_M;
		cr->psd_n_dft = PSD_N_DFT_BE0;
		cr->psd_n_dft_m = PSD_N_DFT_BE0_M;
		cr->psd_path_sel = PSD_IN_PATH_SEL_BE0;
		cr->psd_path_sel_m = PSD_IN_PATH_SEL_BE0_M;
		cr->psd_source_sel = PSD_IN_SOURCE_SEL_BE0;
		cr->psd_source_sel_m = PSD_IN_SOURCE_SEL_BE0_M;
		cr->psd_en = PSD_ENABLE_BE0;
		cr->psd_en_m = PSD_ENABLE_BE0_M;
		break;
	#endif

	default:
		BB_WARNING("[%s] BBCR Hook FAIL!\n", __func__);
		if (bb->bb_dbg_i.cr_fake_init_hook_en) {
			BB_TRACE("[%s] BBCR fake init\n", __func__);
			halbb_cr_hook_fake_init(bb, (u32 *)cr, (sizeof(struct bb_psd_cr_info) >> 2));
		}
		break;
	}

	if (bb->bb_dbg_i.cr_init_hook_recorder_en) {
		BB_TRACE("[%s] BBCR Hook dump\n", __func__);
		halbb_cr_hook_init_dump(bb, (u32 *)cr, (sizeof(struct bb_psd_cr_info) >> 2));
	}
}
#endif

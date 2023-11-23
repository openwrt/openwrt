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

#ifdef HALBB_CH_INFO_SUPPORT

static u8 grp_num_tab[4] = {1, 2, 4, 16};

void halbb_ch_trig_select(struct bb_info *bb, u8 event)
{
	struct bb_ch_info_physts_info *ch_physts = &bb->bb_ch_rpt_i.bb_ch_info_physts_i;

 	if (event == CH_RPT_TRIG_ONCE) {
		ch_physts->ch_info_event = CH_RPT_TRIG_ONCE;
		ch_physts->ch_info_state = CH_RPT_START_TO_WAIT;
	}	
	else if (event == CH_RPT_TRIG_ALWAYS) {
		ch_physts->ch_info_event = CH_RPT_TRIG_ALWAYS;
		ch_physts->ch_info_state = CH_RPT_ALWAYS_ON;
	}
	else {
		BB_DBG(bb, DBG_CH_INFO, "Trig_event(%d): Error Event \n",
		       event);
		return;
	}
	
	BB_DBG(bb, DBG_CH_INFO, "Trig_event = %d\n", ch_physts->ch_info_event);
}

bool halbb_ch_info_valid_chk_8852a(struct bb_info *bb, struct physts_rxd *desc)
{
#if defined(BB_8852A_2_SUPPORT)
	struct bb_physts_info	*physts = &bb->bb_physts_i;
	struct bb_physts_rslt_8_info *psts_8 = &physts->bb_physts_rslt_8_i;
	struct bb_ch_rpt_info *ch_rpt = &bb->bb_ch_rpt_i;
	struct bb_ch_info_physts_info *ch_physts = &ch_rpt->bb_ch_info_physts_i;
	u16 err_len = 0;

	if (bb->ic_type != BB_RTL8852A)
		return true;
	
	/* [bug_idx=2&5] To avoid rxsc!= ch_info. -> partial ch_info. = 0*/
	if (!(psts_8->is_ch_info_len_valid && psts_8->rxsc == 0)) {
		BB_DBG(bb, DBG_CH_INFO, "valid=%d, rxsc = %d\n",
		       psts_8->is_ch_info_len_valid, psts_8->rxsc);
		return false;
	}

	/* [bug_idx=1] To avoid HE40M error ch_len */
	if (physts->bb_physts_rslt_1_i.bw_idx == CHANNEL_WIDTH_40 &&
	    desc->data_rate >= BB_HE_1SS_MCS0) {
		err_len = 62 * ch_rpt->bb_ch_rpt_size_i.per_tone_ch_rpt_size;
	
		if (ch_physts->ch_info_len != err_len) {
			BB_DBG(bb, DBG_CH_INFO, "ch_info_len(%d) != err_len(%d)\n",
			       ch_physts->ch_info_len, err_len);
			return false;
		}
	
		ch_physts->ch_info_len = 32 * ch_rpt->bb_ch_rpt_size_i.per_tone_ch_rpt_size;
	}

	/* To avoid HE20M error ch_len */
	if (physts->bb_physts_rslt_1_i.bw_idx == CHANNEL_WIDTH_20 &&
	    desc->data_rate >= BB_HE_1SS_MCS0) {
		err_len = 64 * ch_rpt->bb_ch_rpt_size_i.per_tone_ch_rpt_size;
	
		if (ch_physts->ch_info_len != err_len) {
			BB_DBG(bb, DBG_CH_INFO, "ch_info_len(%d) != err_len(%d)\n",
			       ch_physts->ch_info_len, err_len);
			return false;
		}
	
		ch_physts->ch_info_len = 16 * ch_rpt->bb_ch_rpt_size_i.per_tone_ch_rpt_size;
	}
#endif
	return true;
}

void halbb_ch_info_cr_dump(struct bb_info *bb)
{
	struct bb_ch_rpt_info *ch_rpt = &bb->bb_ch_rpt_i;
	struct bb_ch_info_cr_info *cr = &ch_rpt->bb_ch_info_cr_i;
	struct bb_ch_info_cr_cfg_info *cfg = &ch_rpt->bb_ch_info_cr_cfg_i;
	struct bb_ch_info_cr_cfg_info *cur_cfg = &ch_rpt->bb_ch_info_cur_cr_cfg_i;
	u32 cr_table[4];
	u8 cr_len = sizeof(cr_table) / sizeof(u32);

	cr_table[0] = cr->ch_info_en_0;
	cr_table[1] = cr->ele_bitmap;
	cr_table[2] = cr->ch_info_type;
	cr_table[3] = cr->seg_len;

	halbb_cr_table_dump(bb, cr_table, cr_len);

	BB_TRACE("src=%d, cmprs=%d, grp_num/he=%d/%d\n",
		 cur_cfg->ch_i_data_src, cur_cfg->ch_i_cmprs,
		 cur_cfg->ch_i_grp_num, cur_cfg->ch_i_grp_num_he);
	BB_TRACE("blk_start/end_=%d/%d, bitmap=0x%x, type=%d, seg_len=%d,\n",
		 cur_cfg->ch_i_blk_start_idx, cur_cfg->ch_i_blk_end_idx,
		 cur_cfg->ch_i_ele_bitmap, cur_cfg->ch_i_type, cur_cfg->ch_i_seg_len);
}

void halbb_ch_info_physts_get_buf(struct bb_info *bb, u8 *rpt_buf,
				 struct bb_ch_rpt_hdr_info *hdr,
				 struct bb_phy_info_rpt *phy_info,
				 struct bb_ch_info_drv_rpt *drv)
{
	struct bb_ch_rpt_info *ch_rpt = &bb->bb_ch_rpt_i;
	struct bb_ch_info_physts_info *ch_physts = &ch_rpt->bb_ch_info_physts_i;
	struct bb_ch_info_raw_info *buf = &ch_rpt->bb_ch_info_raw_i;

	drv->get_ch_rpt_success = ch_physts->get_ch_rpt_success;
	drv->seg_idx_curr = 0;
	drv->raw_data_len = ch_physts->ch_info_len;
	rpt_buf = (u8*)buf->octet;
}

void halbb_ch_info_print(struct bb_info *bb, char input[][16], u32 *_used,
			 char *output, u32 *_out_len)
{
	struct bb_ch_rpt_info *ch_rpt = &bb->bb_ch_rpt_i;
	struct bb_ch_rpt_size_info *size = &ch_rpt->bb_ch_rpt_size_i;
	struct bb_ch_info_physts_info *ch_physts = &ch_rpt->bb_ch_info_physts_i;
	struct bb_ch_info_cr_cfg_info *cfg = &ch_rpt->bb_ch_info_cr_cfg_i;
	struct bb_ch_info_raw_info *buf = &ch_rpt->bb_ch_info_raw_i;
	u16 *rpt_tmp_16 = NULL;
	u8 *rpt_tmp_8 = NULL;
	u16 tone_num = 0;
	u8 i, j, k;
	u32 rpt_idx = 0;
	#if defined(BB_8852A_2_SUPPORT)
	u16 shift_tone = 0;
	#endif

	tone_num = HALBB_DIV(ch_physts->ch_info_len, size->per_tone_ch_rpt_size);
	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "[CH-Info Rpt][bitmap=0x%x] len:%d, per_tone_size=%d, tone_num=%d, bw=%d M\n",
		    ch_physts->bitmap_type_rpt,
		    ch_physts->ch_info_len, size->per_tone_ch_rpt_size,
		    tone_num, 20 << bb->hal_com->band[0].cur_chandef.bw);

	if (!ch_rpt->print_en)
		return;

	if (!buf->octet)
		return;

	if (ch_physts->ch_info_len == 0)
		return;
		
	if (buf->ch_info_buf_len < ch_physts->ch_info_len)
		return;

	/*===[CR Setting]===================================================*/
	BB_TRACE("[CR Setting]\n");
	halbb_ch_info_cr_dump(bb);
	BB_TRACE("per_tone_size=%d bit, tone_num=%d\n",
		 size->per_tone_ch_rpt_size, tone_num);
	BB_TRACE("Data=%d Byte, grp=1/%d, H:[%dx%d]\n",
		 1 << cfg->ch_i_cmprs, grp_num_tab[cfg->ch_i_grp_num],
		 size->n_r, size->n_c);
	halbb_print_devider(bb, BB_DEVIDER_LEN_32, true, FRC_PRINT_LINE);

	/*===[phy-sts rpt]===================================================*/
	BB_TRACE("[phy-sts rpt]\n");

	halbb_print_rate_2_buff(bb, ch_physts->data_rate, ch_physts->gi_ltf,
				bb->dbg_buf, HALBB_SNPRINT_SIZE);

	BB_TRACE("RxRate:%s (0x%x), gi_ltf=%d\n",
		 bb->dbg_buf, ch_physts->data_rate, ch_physts->gi_ltf);

	BB_TRACE("bitmap_type=0x%x, RF_BW=%d M, rxsc=%d, n_rx=%d, n_sts=%d\n",
		 ch_physts->bitmap_type_rpt,
		 20 << bb->hal_com->band[0].cur_chandef.bw,
		 ch_physts->rxsc, ch_physts->n_rx, ch_physts->n_sts);

	BB_TRACE("len=%d, evm_1/2={%d.%02d,%d.%02d}, noise_pwr=%d.%d, valid=%d\n",
		 ch_physts->ch_info_len,
		 ch_physts->evm_1_sts >> 2,
		 halbb_show_fraction_num(ch_physts->evm_1_sts & 0x3, 2),
		 ch_physts->evm_2_sts >> 2,
		 halbb_show_fraction_num(ch_physts->evm_2_sts & 0x3, 2),
		 ch_physts->avg_idle_noise_pwr >> 1,
		 halbb_show_fraction_num(ch_physts->avg_idle_noise_pwr & 0x1, 1),
		 ch_physts->is_ch_info_len_valid);
	halbb_print_devider(bb, BB_DEVIDER_LEN_32, true, FRC_PRINT_LINE);
	/*===[Raw data]=====================================================*/
	BB_TRACE("[CH-Info Raw data]\n");
	halbb_print_buff_64(bb, (u8*)buf->octet, ch_physts->ch_info_len);
	halbb_print_devider(bb, BB_DEVIDER_LEN_32, true, FRC_PRINT_LINE);
	#if defined(BB_8852A_2_SUPPORT)
	if (bb->ic_type == BB_RTL8852A && size->data_byte == 1) {
		BB_TRACE("[RXD-N Shifted raw data]\n");
		if (size->n_c == 1 && size->n_r == 1 && ch_physts->data_rate <= BB_54M
					&& ((cfg->ch_i_grp_num == 1) || (cfg->ch_i_grp_num == 2))) {
			halbb_print_buff_64(bb, (u8*)buf->octet + ch_physts->ch_info_len + 2, CH_INFO_RXD_LEN);
		} else {
			halbb_print_buff_64(bb, (u8*)buf->octet + ch_physts->ch_info_len -
											CH_INFO_RXD_LEN + 2, CH_INFO_RXD_LEN);
		}
		halbb_print_devider(bb, BB_DEVIDER_LEN_32, true, FRC_PRINT_LINE);
	}
	#endif
	/*===[Per Tone IQ data Parsing]======================================*/
	BB_TRACE("[Per Tone data]\n");

	if (size->data_byte == 2) {
		BB_TRACE("S(16,12)\n");
		rpt_tmp_16 = (u16 *)buf->octet;
	} else {
		BB_TRACE("S(8,4)\n");
		rpt_tmp_8 = (u8 *)buf->octet;
		#if defined(BB_8852A_2_SUPPORT)
		if (bb->ic_type == BB_RTL8852A &&
		    size->n_c == 1 && size->n_r == 1 && ch_physts->data_rate <= BB_54M
					&& ((cfg->ch_i_grp_num == 1) || (cfg->ch_i_grp_num == 2))) {
			shift_tone = tone_num - 2;
		} else {
			shift_tone = tone_num - (4/(size->n_c << (size->n_r - 1)));
		}
		#endif
	}		

	for (i = 0; i < tone_num; i++) {
		BB_TRACE("[Tone_idx=%d]\n", i);

		#if defined(BB_8852A_2_SUPPORT)
		/* 8bit IQ ch_info would shift 2 byte at tone-1, and no-shift at rxd-N */
		if (bb->ic_type == BB_RTL8852A && size->data_byte == 1 && i == shift_tone) {
			if (size->n_c == 1 && size->n_r == 1 && ch_physts->data_rate <= BB_54M
					&& ((cfg->ch_i_grp_num == 1) || (cfg->ch_i_grp_num == 2))) {
				rpt_tmp_8 = (u8*)buf->octet + ch_physts->ch_info_len + 2;
			} else {
				rpt_tmp_8 = (u8*)buf->octet + ch_physts->ch_info_len - CH_INFO_RXD_LEN + 2;
			}
			rpt_idx = 0;
		}
		#endif

		for (j = 0; j < size->n_c; j++) {
			for (k = 0; k < size->n_r; k++) {
				if ((size->data_byte == 2) && (rpt_tmp_16 != NULL)) {
					BB_TRACE("H_%d%d: 0x%04x + 0x%04x i\n",
						 j, k, rpt_tmp_16[rpt_idx + 1], rpt_tmp_16[rpt_idx]);
				} else {
					BB_TRACE("H_%d%d: 0x%02x + 0x%02x i\n",
						 j, k, rpt_tmp_8[rpt_idx + 1], rpt_tmp_8[rpt_idx]);
				}
				rpt_idx += 2; /*I & Q*/
			}
		}
		halbb_print_devider(bb, BB_DEVIDER_LEN_16, false, FRC_PRINT_LINE);
	}
}

bool halbb_ch_info_wait_from_physts(struct bb_info *bb, u32 dly, u32 dly_max,
				    u16 bitmap, bool valid_rpt_only)
{
	struct bb_ch_rpt_info *ch_rpt = &bb->bb_ch_rpt_i;
	struct bb_ch_info_physts_info *ch_physts = &ch_rpt->bb_ch_info_physts_i;
	struct bb_ch_info_cr_cfg_info *cfg = &ch_rpt->bb_ch_info_cr_cfg_i;
	bool rpt_success = false;
	u32 wait_time = 0;

	BB_DBG(bb, DBG_CH_INFO, "dly=%d, dly_max=%d ms\n", dly, dly_max);

	if (bitmap & BIT(LEGACY_OFDM_PKT))
		cfg->ch_i_type = BB_CH_LEGACY_CH;
	else
		cfg->ch_i_type = BB_CH_MIMO_CH;

	if (bb->ic_type & (BB_RTL8852B | BB_RTL8852C | BB_RTL8192XB |
			   BB_RTL8851B))
		cfg->ch_i_data_src = 1; /*HW bug, or Ch-info length may only 1/2 */

	if (!halbb_cfg_ch_info_cr(bb, cfg))
		return false;

	if (ch_physts->ch_info_event == CH_RPT_TRIG_ALWAYS)		
		return false;


	ch_physts->ch_info_state = CH_RPT_START_TO_WAIT;
	
	ch_physts->valid_ch_info_only_en = valid_rpt_only;
	halbb_ch_info_physts_en(bb, true, bitmap, bb->bb_phy_idx);
	//halbb_delay_ms(bb, 10);

	while (wait_time <= dly_max) {
		//Delay for get physts
		BB_DBG(bb, DBG_CH_INFO, "wait=%d ms\n", wait_time);
		halbb_delay_ms(bb, dly);
		wait_time += dly;

		if (ch_physts->ch_info_state == CH_RPT_GETTED) {
			rpt_success = true;
			break;
		}
	}
	halbb_ch_info_physts_en(bb, false, bitmap, bb->bb_phy_idx);
	ch_physts->get_ch_rpt_success = rpt_success;

	return rpt_success;
}

bool halbb_ch_info_chk_cr_valid(struct bb_info *bb, struct bb_ch_info_cr_cfg_info *cfg)
{
	struct bb_ch_rpt_info *ch_rpt = &bb->bb_ch_rpt_i;
	struct bb_ch_rpt_size_info *rpt_size = &ch_rpt->bb_ch_rpt_size_i;
	u8 ch_matrix_nr[4];
	u8 i = 0, j = 0;
	u8 nc = 1, nr = 1;
	u8 msb_bit = 0;
	u8 mask_tmp = 0;
	u16 per_tone_size = 0;

	/*{Data_bit}*/
	if (cfg->ch_i_cmprs == 0)
		rpt_size->data_byte = 1;
	else
		rpt_size->data_byte = 2;

	BB_DBG(bb, DBG_CH_INFO, "ch_i_ele_bitmap = 0x%x\n", cfg->ch_i_ele_bitmap);

	/*{Nc}*/
	for (i = 0; i < 4; i++) {
		ch_matrix_nr[i] = (cfg->ch_i_ele_bitmap >> (8 * i)) & 0xff;
		if (i == 0 && ch_matrix_nr[i] == 0) {
			BB_WARNING("ch_matrix_nr[0] = 0\n");
			return false;
		} else if (i >= 1) {
			if (ch_matrix_nr[i] != 0) {
				nc++;
				if (ch_matrix_nr[i] != ch_matrix_nr[i - 1]) {
				    BB_WARNING("matrix_nr[%d]/[%d]=0x%x/0x%x\n",
					       i - 1, i, ch_matrix_nr[i-1], ch_matrix_nr[i]);
					return false;
				}
			}
		}
	}

	/*{Nr}*/
	for (i = 0; i < 8; i++) {
		if (ch_matrix_nr[0] & BIT(i))
			msb_bit = i;
	}
	nr = msb_bit + 1;

	mask_tmp = (u8)halbb_gen_mask_from_0(nr);
	if (ch_matrix_nr[0] != mask_tmp) {
		BB_WARNING("ch_matrix_nr[0]=0x%x, mask_tmp = 0x%x\n",
			   ch_matrix_nr[0], mask_tmp);
		return false;
	}

	rpt_size->n_c = nc;
	rpt_size->n_r = nr;
	BB_DBG(bb, DBG_CH_INFO, "Nr x Nc: [%d x %d]\n", nr, nc);

	/*
	Length = {Data_bit} * {I,Q} * {Nc * Nr} * {N_tone(BW) / group_num}
	       = {8 or 16} * 2 * {1'number in ele_bitmap} * {N_tone} / {1/2/4/16}
	*/
	per_tone_size = rpt_size->data_byte * 2 * nc * nr;
	rpt_size->per_tone_ch_rpt_size = per_tone_size;
	BB_DBG(bb, DBG_CH_INFO, "per_tone_size = %d\n", per_tone_size);

	if (cfg->ch_i_grp_num >= CH_DESI_OPT_NUM || cfg->ch_i_grp_num_he >= CH_DESI_OPT_NUM)
		return false;

	for (i = 0; i < CH_INFO_BW_NUM; i++) {
		rpt_size->ch_info_tone_num_lgcy[i] = rpt_size->tone_num_lgcy[i][cfg->ch_i_grp_num];
		rpt_size->ch_info_tone_num[i] = rpt_size->tone_num[i][cfg->ch_i_grp_num];
		rpt_size->ch_info_tone_num_he[i] = rpt_size->tone_num_he[i][cfg->ch_i_grp_num];
		BB_DBG(bb, DBG_CH_INFO, "tone_num/he = %d, %d\n",
		       rpt_size->ch_info_tone_num[i], rpt_size->ch_info_tone_num_he[i]);

		rpt_size->ch_info_rpt_len[i] = per_tone_size * rpt_size->ch_info_tone_num[i];
		rpt_size->ch_info_rpt_len_he[i] = per_tone_size * rpt_size->ch_info_tone_num_he[i];
	}
	return true;
}

void halbb_ch_info_size_query(struct bb_info *bb,
			     struct bb_ch_rpt_size_info *exp_rpt_size,enum phl_phy_idx phy_idx)
{
	struct bb_ch_rpt_size_info *rpt_size = &bb->bb_ch_rpt_i.bb_ch_rpt_size_i;
	struct bb_physts_info *physts = &bb->bb_physts_i;
	u16 tmp_size = 0;
	u8 i = 0;

	BB_DBG(bb, DBG_CH_INFO, "[%s] ====>\n", __func__);

	for (i = 0; i < CH_INFO_BW_NUM; i++) {
		tmp_size = physts->physts_rpt_len_byte[LEGACY_OFDM_PKT] + rpt_size->ch_info_rpt_len[i];
		rpt_size->valid_size_physts_lgcy[i] = (tmp_size < 1024) ? true : false;

		tmp_size = physts->physts_rpt_len_byte[HT_PKT] + rpt_size->ch_info_rpt_len[i];
		rpt_size->valid_size_physts_ht[i] = (tmp_size < 1024) ? true : false;

		tmp_size = physts->physts_rpt_len_byte[VHT_PKT] + rpt_size->ch_info_rpt_len[i];
		rpt_size->valid_size_physts_vht[i] = (tmp_size < 1024) ? true : false;

		tmp_size = physts->physts_rpt_len_byte[HE_PKT] + rpt_size->ch_info_rpt_len_he[i];
		rpt_size->valid_size_physts_he[i] = (tmp_size < 1024) ? true : false;
		BB_DBG(bb, DBG_CH_INFO, "[BW=%d M]\n", 20 << i);

		BB_DBG(bb, DBG_CH_INFO, "  *[LGCY] (valid %d): [size_physts(%d) + (CSI_rpt_len %d)] < 1024 Byte\n",
		      rpt_size->valid_size_physts_lgcy[i], physts->physts_rpt_len_byte[LEGACY_OFDM_PKT], rpt_size->ch_info_rpt_len[i]);
		BB_DBG(bb, DBG_CH_INFO, "  *[HT  ] (valid %d): [size_physts(%d) + (CSI_rpt_len %d)] < 1024 Byte\n",
		      rpt_size->valid_size_physts_ht[i], physts->physts_rpt_len_byte[HT_PKT], rpt_size->ch_info_rpt_len[i]);
		BB_DBG(bb, DBG_CH_INFO, "  *[VHT ] (valid %d): [size_physts(%d) + (CSI_rpt_len %d)] < 1024 Byte\n",
		      rpt_size->valid_size_physts_vht[i], physts->physts_rpt_len_byte[VHT_PKT], rpt_size->ch_info_rpt_len[i]);
		BB_DBG(bb, DBG_CH_INFO, "  *[HE  ] (valid %d): [size_physts(%d) + (CSI_rpt_len %d)] < 1024 Byte\n",
		      rpt_size->valid_size_physts_he[i], physts->physts_rpt_len_byte[HE_PKT], rpt_size->ch_info_rpt_len_he[i]);
	}

	halbb_mem_cpy(bb, exp_rpt_size, rpt_size, sizeof(struct bb_ch_rpt_size_info));
}

void halbb_ch_info_cfg_mu_buff_cr(struct bb_info *bb, bool en)
{
	struct bb_ch_rpt_info *ch_rpt = &bb->bb_ch_rpt_i;
	struct bb_ch_info_cr_cfg_info *cfg = &ch_rpt->bb_ch_info_cr_cfg_i;
	struct bb_ch_info_cr_cfg_info *cur_cfg = &ch_rpt->bb_ch_info_cur_cr_cfg_i;
	struct bb_ch_info_cr_info *cr = &ch_rpt->bb_ch_info_cr_i;
	u32 val_32 = 0;

	BB_DBG(bb, DBG_CH_INFO, "[%s], en=%d\n", __func__, en);
	if (en) {
		if (bb->ic_type & (BB_RTL8852B | BB_RTL8851B)) {
			cfg->ch_i_blk_start_idx = 1;
			cfg->ch_i_blk_end_idx = 2;
		} else {
			cfg->ch_i_blk_start_idx = 1;
			cfg->ch_i_blk_end_idx = 10;
		}
	} else {
		cfg->ch_i_blk_start_idx = 0;
		cfg->ch_i_blk_end_idx = 0;
	}

	val_32 = (cfg->ch_i_blk_end_idx << 4) | cfg->ch_i_blk_start_idx;

	cur_cfg->ch_i_blk_start_idx= cfg->ch_i_blk_start_idx; /*1~10*/
	cur_cfg->ch_i_blk_end_idx= cfg->ch_i_blk_end_idx; /*1~10*/

	halbb_set_reg(bb, cr->ch_info_en_0, 0xff00, val_32);

	BB_DBG(bb, DBG_CH_INFO, "blk_start/end_=%d/%d\n",
	       cur_cfg->ch_i_blk_start_idx, cur_cfg->ch_i_blk_end_idx);
}

bool halbb_cfg_ch_info_cr(struct bb_info *bb, struct bb_ch_info_cr_cfg_info *cfg)
{
	struct bb_ch_rpt_info *ch_rpt = &bb->bb_ch_rpt_i;
	struct bb_ch_info_cr_cfg_info *cur_cfg = &ch_rpt->bb_ch_info_cur_cr_cfg_i;
	struct bb_ch_info_cr_info *cr = &bb->bb_ch_rpt_i.bb_ch_info_cr_i;	
	u32 val_32;

	BB_DBG(bb, DBG_CH_INFO, "src=%d, cmprs=%d, grp_num/he=%d/%d\n",
	       cfg->ch_i_data_src, cfg->ch_i_cmprs,
	       cfg->ch_i_grp_num, cfg->ch_i_grp_num_he);
	BB_DBG(bb, DBG_CH_INFO, "bitmap=0x%x, type=%d, seg_len=%d\n",
	       cfg->ch_i_ele_bitmap, cfg->ch_i_type, cfg->ch_i_seg_len);

	if (!halbb_ch_info_chk_cr_valid(bb, cfg)) {
		BB_DBG(bb, DBG_CH_INFO, "[%s] invalid\n", __func__);
		return false;
	}

	cur_cfg->ch_i_data_src= cfg->ch_i_data_src; /*0~1: CH-estimation, CH-smoothing*/
	cur_cfg->ch_i_cmprs= cfg->ch_i_cmprs; /*0~1: 8/16 bit, 0:S(8,4), 1:S(16,12)*/
	cur_cfg->ch_i_grp_num= cfg->ch_i_grp_num; /*[Lgcy/HT/VHT] 0~3: decimation to 1/1, 1/2, 1/4, 1/16*/
	cur_cfg->ch_i_grp_num_he= cfg->ch_i_grp_num_he; /*[HE]0~3: decimation to 1/1, 1/2, 1/4, 1/16*/
	cur_cfg->ch_i_ele_bitmap= cfg->ch_i_ele_bitmap; /*Channel matrix size, ex: 0x303:2X2, 0x1: 1X1*/
	cur_cfg->ch_i_type= cfg->ch_i_type; /*0~1: Legacy-CH, MIMO-CH*/
	cur_cfg->ch_i_seg_len= cfg->ch_i_seg_len; /*0~3: 12/28/60/124 (8byte)*/

	/*CH-info Common Settings*/
	val_32 = (cfg->ch_i_data_src & 0x1) |
		 ((cfg->ch_i_cmprs & 0x1) << 1) |
		 ((cfg->ch_i_grp_num & 0x3) << 2) |
		 ((cfg->ch_i_grp_num_he & 0x3) << 4);

	halbb_set_reg(bb, cr->ch_info_en_0, 0xfc, val_32);
	halbb_set_reg(bb, cr->ele_bitmap, MASKDWORD, cfg->ch_i_ele_bitmap);
	halbb_set_reg(bb, cr->ch_info_type, cr->ch_info_type_m, cfg->ch_i_type);
	halbb_set_reg(bb, cr->seg_len, cr->seg_len_m, cfg->ch_i_seg_len);

	return true;
}

void halbb_ch_info_physts_en(struct bb_info *bb, bool en,
			     u16 bitmap, enum phl_phy_idx phy_idx)
{
	struct bb_ch_rpt_info *ch_rpt = &bb->bb_ch_rpt_i;
	struct bb_ch_info_cr_info *cr = &ch_rpt->bb_ch_info_cr_i;
	u32 val_32 = 1;
	u16 i = 0;

	BB_DBG(bb, DBG_CH_INFO, "[%s] en=%d, bitmap=0x%x\n", __func__, en, bitmap);

	if (en) {
		ch_rpt->ch_info_data_mode |= CH_INFO_FROM_PHY_STS;
	} else {
		ch_rpt->ch_info_data_mode &= ~CH_INFO_FROM_PHY_STS;
		if (ch_rpt->ch_info_data_mode == 0)
			val_32 = 0;
	}

	if (en)
		halbb_ch_info_buf_alloc(bb);

	BB_DBG(bb, DBG_CH_INFO, "Data_mode=%d\n", ch_rpt->ch_info_data_mode);

	if (phy_idx == HW_PHY_0) {
		halbb_set_reg(bb, cr->ch_info_en_0, 0x3, 1);
	} else {
		halbb_set_reg(bb, cr->ch_info_en_0, 0x3, 2);
	}

	/*Phy-sts IE 8 Enable*/
	for (i=0; i < PHYSTS_BITMAP_NUM; i++) {
		if (bitmap & BIT(i))
			halbb_physts_ie_bitmap_en(bb, i, IE08_FTR_CH, en);
	}
}

void halbb_ch_info_status_en(struct bb_info *bb, bool en, enum phl_phy_idx phy_idx)
{
	struct bb_ch_rpt_info *ch_rpt = &bb->bb_ch_rpt_i;
	struct bb_ch_info_cr_info *cr = &ch_rpt->bb_ch_info_cr_i;
	//u32 val_32 = 1;

	BB_DBG(bb, DBG_CH_INFO, "[%s] en=%d\n", __func__, en);

	if (en) {
		ch_rpt->ch_info_data_mode |= CH_INFO_FROM_CH_STS;
	} else {
		ch_rpt->ch_info_data_mode &= ~CH_INFO_FROM_CH_STS;
		//if (ch_rpt->ch_info_data_mode == 0)
		//	val_32 = 0;
	}

	BB_DBG(bb, DBG_CH_INFO, "Data_mode=%d\n", ch_rpt->ch_info_data_mode);

	halbb_ch_info_cfg_mu_buff_cr(bb, en);

	if (phy_idx == HW_PHY_0) {
		halbb_set_reg(bb, cr->ch_info_en_0, 0x3, 1);
	} else {
		halbb_set_reg(bb, cr->ch_info_en_0, 0x3, 2);
	}
}

void halbb_ch_info_self_test(struct bb_info *bb)
{
	struct bb_ch_rpt_info *ch_rpt = &bb->bb_ch_rpt_i;
	struct bb_ch_info_cr_cfg_info *cfg = &ch_rpt->bb_ch_info_cr_cfg_i;
	struct bb_ch_rpt_size_info size;
	u8 grp_num_tab[4] = {1, 2, 4, 16};
	u8 i = 0;

	BB_DBG(bb, DBG_CH_INFO, "[%s]ch_rpt = %d\n", __func__, ch_rpt->seg_idx_pre);
	halbb_ch_info_size_query(bb, &size, bb->bb_phy_idx);

	BB_TRACE("Ch-info Rpt Len ========>\n");
	BB_TRACE("Data=%d Byte, grp=1/%d, H:[%dx%d] (0x%x), per_tone_len=%d\n",
		 1 << cfg->ch_i_cmprs, grp_num_tab[cfg->ch_i_grp_num],
		 size.n_r, size.n_c,
		 cfg->ch_i_ele_bitmap,
		 size.per_tone_ch_rpt_size); 
	BB_TRACE("[Lgcy/HT/VHT]\n");
	for (i = 0; i < CH_INFO_BW_NUM; i++) {
		BB_TRACE("  [%03dM] Len=%04d Byte, tone_num=%03d\n",
			 (20 << i), size.ch_info_rpt_len[i],
			 size.ch_info_tone_num[i]);
	}
	BB_TRACE("[HE]\n");
	for (i = 0; i < CH_INFO_BW_NUM; i++) {
		BB_TRACE("  [%03dM] Len=%04d Byte, tone_num=%03d\n",
			 (20 << i), size.ch_info_rpt_len_he[i],
			 size.ch_info_tone_num_he[i]);
	}
}

enum bb_ch_info_t halbb_ch_info_get_data(struct bb_info *bb, struct physts_rxd *desc, u8 *addr, u32 len,
					struct bb_ch_rpt_hdr_info *hdr,
					struct bb_phy_info_rpt *phy_info,
					struct bb_ch_info_drv_rpt *drv)
{
	struct bb_ch_rpt_info *ch_rpt = &bb->bb_ch_rpt_i;
	enum bb_ch_info_t csi_rpt = BB_CH_INFO_FAIL;
	u8 seq;
	u8 rxsc = 0;

	if (!addr || len == 0)
		return BB_CH_INFO_FAIL;

	/*=== [BB CSI HDR] ==================================================*/
	halbb_mem_cpy(bb, hdr, addr, ch_rpt->ch_rpt_hdr_len);
	drv->raw_data_len = len - ch_rpt->ch_rpt_hdr_len; /*ex: (12 * 8 = 96) - 8 = 88*/
	addr += ch_rpt->ch_rpt_hdr_len;
	seq = hdr->seq_num;

	drv->seg_idx_curr = seq;
	

	if (hdr->set_valid == 0) {
		BB_DBG(bb, DBG_CH_INFO, "[FAIL][1] valid=%d\n", hdr->set_valid);
		return BB_CH_INFO_FAIL;
	}

	if (ch_rpt->skip_ch_info) {
		if (seq == 0) {
			ch_rpt->skip_ch_info = false;
		} else {
			BB_DBG(bb, DBG_CH_INFO, "[FAIL][2] seq_num=%d\n", seq);
			return BB_CH_INFO_FAIL;
		}
	}

	if (seq != 0 && ((seq - ch_rpt->seg_idx_pre) != 1)) {
		BB_DBG(bb, DBG_CH_INFO, "[FAIL][3] seq_num=%d, seg_idx_pre=%d\n", seq, ch_rpt->seg_idx_pre);
		return BB_CH_INFO_FAIL;
	}

	if (hdr->total_seg_num == 0 || hdr->total_len_l == 0) {
		BB_DBG(bb, DBG_CH_INFO, "[FAIL][4] total_seq=%d, total_len=%d\n",
		       hdr->total_seg_num, hdr->total_len_l);
		return BB_CH_INFO_FAIL;
	}

	ch_rpt->seg_idx_pre = seq;

	BB_DBG(bb, DBG_CH_INFO, "[CSI Hdr][seg_%d: %d/%d] skip_en=%d\n",
	       seq, seq + 1, hdr->total_seg_num, ch_rpt->skip_ch_info);
	BB_DBG(bb, DBG_CH_INFO, "  *total_l=%d, pkt_end=%d, valid=%d, seg_size=%d\n",
	       hdr->total_len_l, hdr->is_pkt_end, hdr->set_valid, hdr->segment_size);
	BB_DBG(bb, DBG_CH_INFO, "  *evm_0=%d, noise_pw=%d, n_rx=%d, n_sts=%d\n",
	       hdr->sts0_evm, hdr->avg_noise_pow, hdr->n_rx, hdr->n_sts);

	/*=== [BB SEG-0 HDR] ==================================================*/
	if (seq == 0) {
		halbb_mem_cpy(bb, phy_info, addr, ch_rpt->phy_info_len);
		addr += ch_rpt->phy_info_len;
		drv->raw_data_len -= ch_rpt->phy_info_len;  /*ex: 88 - 8 = 80*/

		ch_rpt->csi_raw_data_total_len = (hdr->total_len_m << 16 | hdr->total_len_l) 
						 - ch_rpt->ch_rpt_hdr_len - ch_rpt->phy_info_len; /*@320 - 16 = 304*/
		rxsc = (u8)phy_info->rxsc;
		halbb_ch_info_modify_ack_rxsc(bb, desc, ch_rpt->csi_raw_data_total_len, &rxsc);
		phy_info->rxsc = rxsc & 0xf;
		
		BB_DBG(bb, DBG_CH_INFO, "  *csi_raw_data_total_len=%d\n", ch_rpt->csi_raw_data_total_len);

		BB_DBG(bb, DBG_CH_INFO, "[SEG_0 Hdr] len: %d(csi_data) = %d(all) - %d(csi_hdr) - %d(seg0_hdr)\n",
	               drv->raw_data_len, len, ch_rpt->ch_rpt_hdr_len, ch_rpt->phy_info_len); /*ex: 96 - 8 - 8 = 80*/
		BB_DBG(bb, DBG_CH_INFO, "  *RSSI= (%d){%d, %d}, evm={%d, %d} , rxsc= %d\n",
		       phy_info->rssi_avg >> 1, phy_info->rssi[0] >> 1, phy_info->rssi[1] >> 1,
		       hdr->sts0_evm, (phy_info->sts1_evm_m << 4 | phy_info->sts1_evm_l), phy_info->rxsc);

		ch_rpt->total_len_remnant = ch_rpt->csi_raw_data_total_len;
	} else { /*SEG = 1,2,3...,N-1*/
		BB_DBG(bb, DBG_CH_INFO, "[SEG_%d Hdr] len: %d(csi_data) = %d(all) - %d(csi_hdr)\n",
	               seq, drv->raw_data_len, len, ch_rpt->ch_rpt_hdr_len); /*ex: 96 - 8 = 88*/
	}

	BB_DBG(bb, DBG_CH_INFO, "remnant_pre=%d\n", ch_rpt->total_len_remnant);

	if (hdr->is_pkt_end) {
		BB_DBG(bb, DBG_CH_INFO, "[SEG Last] len_remnant=%d\n", ch_rpt->total_len_remnant);

		if (ch_rpt->total_len_remnant <= drv->raw_data_len) {
			csi_rpt = BB_CH_INFO_LAST_SEG;
			BB_DBG(bb, DBG_CH_INFO, "LAST_SEG success\n");
		}
	} else {
		ch_rpt->total_len_remnant -= drv->raw_data_len;

		BB_DBG(bb, DBG_CH_INFO, "remnant_post=%d\n", ch_rpt->total_len_remnant);
		csi_rpt = BB_CH_INFO_SUCCESS;
	}

	BB_DBG(bb, DBG_CH_INFO, "[SEG Last] Copy Data seg=%d\n", seq);

	if (ch_rpt->print_en)
		halbb_print_buff_64(bb, addr, (u16)drv->raw_data_len);
	
	return csi_rpt;
}

enum bb_ch_info_t halbb_ch_info_parsing(struct bb_info *bb, u8 *addr, u32 len,
					struct physts_rxd *desc,
					u8 *rpt_buf,
					struct bb_ch_rpt_hdr_info *hdr,
					struct bb_phy_info_rpt *phy_info,
					struct bb_ch_info_drv_rpt *drv)
{
	struct bb_ch_rpt_info *ch_rpt = &bb->bb_ch_rpt_i;
	enum bb_ch_info_t rpt;

	BB_DBG(bb, DBG_CH_INFO, "[%s] skip_en=%d\n", __func__, ch_rpt->skip_ch_info);

	halbb_print_rate_2_buff(bb, desc->data_rate, desc->gi_ltf, bb->dbg_buf, HALBB_SNPRINT_SIZE);
	BB_DBG(bb, DBG_CH_INFO,"Rate= %s (0x%x-%x), macid_su=%d\n",
	         bb->dbg_buf, desc->data_rate, desc->gi_ltf, desc->macid_su);

	rpt = halbb_ch_info_get_data(bb, desc, addr, len, hdr, phy_info, drv);

	if (rpt == BB_CH_INFO_FAIL) {
		ch_rpt->skip_ch_info = true;
		drv->get_ch_rpt_success = false;
	} else if (rpt == BB_CH_INFO_LAST_SEG) {
		if (ch_rpt->seg_idx_pre == 0)
			/* shift ch_rpt_hdr_len & phy_info_len for SEG 0 */
			halbb_mem_cpy(bb, rpt_buf, (addr + ch_rpt->ch_rpt_hdr_len + ch_rpt->phy_info_len), drv->raw_data_len);
		else
			/* SEG = N-1 (LAST SEG) */
			halbb_mem_cpy(bb, rpt_buf, (addr + ch_rpt->ch_rpt_hdr_len), drv->raw_data_len);
		drv->get_ch_rpt_success = true;
	} else {
		/* shift ch_rpt_hdr_len & phy_info_len for SEG 0 */
		if (ch_rpt->seg_idx_pre == 0) {
			halbb_mem_cpy(bb, rpt_buf, (addr + ch_rpt->ch_rpt_hdr_len + ch_rpt->phy_info_len), drv->raw_data_len);
		/* shift ch_rpt_hdr_len for SEG 1~N-2 */
		} else {
			halbb_mem_cpy(bb, rpt_buf, (addr + ch_rpt->ch_rpt_hdr_len), drv->raw_data_len);
		}
		drv->get_ch_rpt_success = false;
	}

	return rpt;
}

void halbb_ch_info_bbcr_init(struct bb_info *bb)
{
	struct bb_ch_rpt_info *ch_rpt = &bb->bb_ch_rpt_i;
	struct bb_ch_info_cr_cfg_info *cfg = &ch_rpt->bb_ch_info_cr_cfg_i;

	BB_DBG(bb, DBG_CH_INFO, "[%s]\n", __func__);

	cfg->ch_i_data_src = 0;
	cfg->ch_i_cmprs = 1;
	cfg->ch_i_grp_num = 3;
	cfg->ch_i_grp_num_he = 3;
	cfg->ch_i_blk_start_idx = 0;
	cfg->ch_i_blk_end_idx = 0;
	cfg->ch_i_ele_bitmap = 0x303; /*Nr X Nc: 2 X 2*/
	cfg->ch_i_type = BB_CH_MIMO_CH;
	cfg->ch_i_seg_len = 0;
	halbb_cfg_ch_info_cr(bb, cfg);
}

void halbb_ch_info_init(struct bb_info *bb)
{
	struct bb_ch_rpt_info *ch_rpt = &bb->bb_ch_rpt_i;
	struct bb_ch_rpt_size_info *rpt_size = &ch_rpt->bb_ch_rpt_size_i;
	struct bb_ch_info_physts_info *csi_physts = &bb->bb_ch_rpt_i.bb_ch_info_physts_i;
	struct bb_physts_info *physts = &bb->bb_physts_i;
	struct bb_ch_info_raw_info *buf = &bb->bb_ch_rpt_i.bb_ch_info_raw_i;
	struct bb_ch_rpt_size_info *size = &ch_rpt->bb_ch_rpt_size_i;
	u8 i = 0;
	struct bb_ch_rpt_size_info ch_rpt_size_info;

	u16 tone_num_lgcy[CH_INFO_BW_NUM][CH_DESI_OPT_NUM] =
							{{52, 26, 14, 4}, /*20M*/
							 {104, 52, 28, 8}, /*40M*/
							 {208, 104, 56, 16}, /*80M*/
							 {416, 208, 112, 32}}; /*160M TBD*/
	u16 tone_num[CH_INFO_BW_NUM][CH_DESI_OPT_NUM] = {{56, 28, 14, 4}, /*20M*/
							 {114, 58, 30, 8}, /*40M*/
							 {242, 122, 62, 16}, /*80M*/
							 {484, 244, 124, 32}}; /*160M TBD*/
	u16 tone_num_he[CH_INFO_BW_NUM][CH_DESI_OPT_NUM]=
							{{242, 122, 62, 16}, /*20M*/
							 {484, 242, 122, 32}, /*40M*/
							 {996, 498, 250, 64}, /*80M*/
							 {1992, 996, 500, 128}}; /*160M TBD*/

	#if defined(BB_8852A_2_SUPPORT)
	if (bb->ic_type == BB_RTL8852A) {
		tone_num_lgcy[1][0] = 106;
		tone_num_lgcy[1][1] = 54;
		tone_num_lgcy[2][0] = 212;
		tone_num_lgcy[2][1] = 108;

		tone_num_he[0][0] = 244;
		tone_num_he[1][0] = 488;
		tone_num_he[1][1] = 244;
		tone_num_he[1][3] = 62;
	}
	#endif

	halbb_mem_cpy(bb, size->tone_num_lgcy, tone_num_lgcy, CH_INFO_BW_NUM * CH_DESI_OPT_NUM * sizeof(u16));
	halbb_mem_cpy(bb, size->tone_num, tone_num, CH_INFO_BW_NUM * CH_DESI_OPT_NUM * sizeof(u16));
	halbb_mem_cpy(bb, size->tone_num_he, tone_num_he, CH_INFO_BW_NUM * CH_DESI_OPT_NUM * sizeof(u16));

	halbb_ch_info_bbcr_init(bb);
	ch_rpt->ch_rpt_hdr_len = sizeof(struct bb_ch_rpt_hdr_info); /*8852A: 8Byte*/
	ch_rpt->phy_info_len = sizeof(struct bb_phy_info_rpt); /*8852A: 8Byte*/
	ch_rpt->skip_ch_info = true;
	ch_rpt->print_en = false;
	ch_rpt->seg_idx_pre = 0;

	//Init Parameter
	buf->ch_info_buf_len = 1024;
	csi_physts->bitmap_type_auto_en = true;
	csi_physts->ch_info_event = CH_RPT_TRIG_ONCE ;
	ch_rpt->bb_ch_info_physts_i.valid_ch_info_only_en = false;
	ch_rpt->bb_ch_info_physts_i.filter_rxsc_en = false;

	BB_DBG(bb, DBG_CH_INFO, "[%s] hdr_len = %d, phy_info_len=%d\n", __func__, ch_rpt->ch_rpt_hdr_len, ch_rpt->phy_info_len);

	physts->physts_rpt_len_byte[LEGACY_OFDM_PKT] = 100;
	physts->physts_rpt_len_byte[HT_PKT] = 150;
	physts->physts_rpt_len_byte[VHT_PKT] = 150;
	physts->physts_rpt_len_byte[HE_PKT] = 250;

	for (i = 0; i < CH_INFO_BW_NUM; i++) {
		rpt_size->valid_size_physts_lgcy[i] = false;
		rpt_size->valid_size_physts_ht[i] = false;
		rpt_size->valid_size_physts_vht[i] = false;
		rpt_size->valid_size_physts_he[i] = false;
	}

	halbb_ch_info_size_query(bb, &ch_rpt_size_info, bb->bb_phy_idx);
}

void halbb_ch_info_print_buf(struct bb_info *bb)
{
	struct bb_ch_rpt_info *ch_rpt = &bb->bb_ch_rpt_i;
	u16 i = 0;
	u8 *buf_tmp;

	if (!ch_rpt->test_buf)
		return;

	buf_tmp = ch_rpt->test_buf;
	BB_DBG(bb, DBG_CH_INFO, "----------------------------\n");
	for (i = 0; i < ((TEST_CH_INFO_MAX_SEG * TEST_CH_SEG_LEN) >> 3); i++) {
			BB_DBG(bb, DBG_CH_INFO, "[%02d] 0x%016llx\n", i, ((u64*)buf_tmp)[0]);
			buf_tmp += 8;

			if (i == 9 || i == 20 || i == 31 || i == 37 || i == 42)
				BB_DBG(bb, DBG_CH_INFO, "----------------------------\n");
	}
	BB_DBG(bb, DBG_CH_INFO, "----------------------------\n");
}

void halbb_ch_info_buf_rls(struct bb_info *bb)
{
	struct bb_ch_info_raw_info *buf = &bb->bb_ch_rpt_i.bb_ch_info_raw_i;

	if (!buf->octet)
		return;

	BB_DBG(bb, DBG_CH_INFO,"[%s]\n", __func__);

	halbb_mem_free(bb, buf->octet, buf->ch_info_buf_len);
	buf->octet = 0;
}

bool halbb_ch_info_buf_alloc(struct bb_info *bb)
{
	struct bb_ch_info_raw_info *buf = &bb->bb_ch_rpt_i.bb_ch_info_raw_i;

	if (buf->octet) {
		halbb_mem_set(bb, buf->octet, 0, buf->ch_info_buf_len);
		return true;
	}

	BB_DBG(bb, DBG_CH_INFO,"[%s]\n", __func__);

	buf->octet = (s16 *)halbb_mem_alloc(bb, buf->ch_info_buf_len);

	if (!buf->octet)
		return false;
	
	return true;
}

void halbb_ch_info_reset(struct bb_info *bb)
{
	struct bb_ch_rpt_info *ch_rpt = &bb->bb_ch_rpt_i;

	if (ch_rpt->test_buf)
		halbb_mem_set(bb, ch_rpt->test_buf, 0, (TEST_CH_INFO_MAX_SEG * TEST_CH_SEG_LEN));
}

void halbb_ch_info_deinit(struct bb_info *bb)
{
	struct bb_ch_rpt_info *ch_rpt = &bb->bb_ch_rpt_i;

	halbb_ch_info_buf_rls(bb);
	if (ch_rpt->test_buf)
		halbb_mem_free(bb, ch_rpt->test_buf, (TEST_CH_INFO_MAX_SEG * TEST_CH_SEG_LEN));
}

void halbb_ch_info_dbg(struct bb_info *bb, char input[][16], u32 *_used,
		       char *output, u32 *_out_len)
{
	struct bb_ch_rpt_info *ch_rpt = &bb->bb_ch_rpt_i;
	struct bb_ch_info_cr_cfg_info *cfg = &ch_rpt->bb_ch_info_cr_cfg_i;
	struct bb_ch_rpt_size_info *size = &ch_rpt->bb_ch_rpt_size_i;
	struct bb_ch_info_physts_info *ch_physts = &ch_rpt->bb_ch_info_physts_i;
	struct bb_pkt_cnt_su_store_info *store = &bb->bb_cmn_rpt_i.bb_pkt_cnt_su_store_i;
	struct bb_ch_rpt_hdr_info ch_rpt_hdr = {0};
	struct bb_phy_info_rpt phy_rpt_in;
	struct bb_ch_rpt_hdr_info hdr; /*output*/
	struct bb_phy_info_rpt phy_rpt; /*output*/
	struct bb_ch_info_drv_rpt drv_rpt; /*output*/
	enum bb_ch_info_t rpt; /*output*/
	struct physts_rxd rxdesc = {0};
	u16 bitmap_type = 0; /*bitmap for physts type*/
	bool get_ch_info_success;
	u8 seg_idx;
	u8 *addr, *addr_tmp, *addr_ori;
	u32 len = TEST_CH_SEG_LEN; /*Byte*/
	u32 val[11] = {0};
	u16 i = 0;

	if (_os_strcmp(input[1], "-h") == 0) {

		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "=== [Rich Mode(CH-info-sts)] =================\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "trig_chinfo en {val}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "trig_chinfo print {val}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "{self_test}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "{print}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "{rst}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "{test} {seg_idx:0~3}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "cfg blk {start_idx} {end_idx}\n\n");

		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "=== [Light Mode(Phy-sts)] ====================\n");

		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "trig_physts {print_date_en} {valid_rpt_only_en}\n\n");

		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "=== [General] ================================\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "cr_dump\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "cfg show\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "cfg para0 {bitmap-0: auto, 6~7: HE/VHT_MU, 12~15: Lgcy/HT/VHT/HE}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "cfg para1 {0~1: 1~2 Byte} {grp: 0~3:1/2/4/16} {H_map(Hex)}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "cfg para2 {seg_len}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "cfg para3 {0:LS, 1:CS}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "cfg para4 {0:Trigger Once, 1:Trigger Always}\n\n");		
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "filter rxsc {en} {rxsc_idx}\n\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "==============================================\n");
		return;
	}

	if (_os_strcmp(input[1], "self_test") == 0) {
		halbb_ch_info_self_test(bb);
	} else if (_os_strcmp(input[1], "print") == 0) {
		halbb_ch_info_print_buf(bb);
	} else if (_os_strcmp(input[1], "rst") == 0) {
		halbb_ch_info_reset(bb);
	} else if (_os_strcmp(input[1], "cr_dump") == 0) {
		halbb_ch_info_cr_dump(bb);
	} else if (_os_strcmp(input[1], "filter") == 0) {
		if (_os_strcmp(input[2], "rxsc") == 0) {
			HALBB_SCAN(input[3], DCMD_DECIMAL, &val[0]);
			HALBB_SCAN(input[4], DCMD_DECIMAL, &val[1]);
			ch_physts->filter_rxsc_en = (bool)val[0];
			ch_physts->filter_rxsc_tgrt_idx = (u8)val[1];
			BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
				    "filter rxsc_en=%d, rxsc_tgrt_idx=%d\n",
				    ch_physts->filter_rxsc_en, ch_physts->filter_rxsc_tgrt_idx);
		}
	} else if (_os_strcmp(input[1], "trig_chinfo") == 0) {
		HALBB_SCAN(input[3], DCMD_DECIMAL, &val[0]);
		if (_os_strcmp(input[2], "en") == 0) {
			BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
				    "trig_chinfo_en=%d\n", val[0]);

			halbb_ch_info_status_en(bb, (bool)val[0], bb->bb_phy_idx);
		} else if (_os_strcmp(input[2], "print") == 0) {
			ch_rpt->print_en = (bool)val[0];
			BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
				    "[CH-info] print_raw_data_en=%d\n", ch_rpt->print_en);
		} else {
			BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
				    "Err\n");
		}
	} else if (_os_strcmp(input[1], "trig_physts") == 0) {
		HALBB_SCAN(input[2], DCMD_DECIMAL, &val[0]);
		HALBB_SCAN(input[3], DCMD_HEX, &val[1]);
		ch_rpt->print_en = (bool)val[0];

		//if (!halbb_ch_info_buf_alloc(bb)) {
		//	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		//		    "Buff alloc fail\n");
		//	return;
		//}

		if (ch_physts->bitmap_type_auto_en) {
			if (store->he_pkt_not_zero)
				bitmap_type = bitmap_type | BIT(HE_PKT);
			else if (store->vht_pkt_not_zero)
				bitmap_type = bitmap_type | BIT(VHT_PKT);
			else if (store->ht_pkt_not_zero)
				bitmap_type = bitmap_type | BIT(HT_PKT);
			else
				bitmap_type = bitmap_type | BIT(LEGACY_OFDM_PKT);
		} else {
			bitmap_type |= BIT(ch_physts->force_bitmap_type);
		}

		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "Auto Type=%d\n", ch_physts->bitmap_type_auto_en);

		get_ch_info_success = halbb_ch_info_wait_from_physts(bb, 100, 1000, bitmap_type, (bool)val[1]);

		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "Trigger Type[bitmap=0x%x], BW=%d\n",
			    bitmap_type, 20 << bb->hal_com->band[0].cur_chandef.bw);

		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "Get CH-info success=%d\n", get_ch_info_success);

		if (!get_ch_info_success)
			return;

		halbb_ch_info_print(bb, input, _used, output, _out_len);

	} else if (_os_strcmp(input[1], "cfg") == 0) {

		for (i = 0; i <= 4; i++)
			HALBB_SCAN(input[3 + i], DCMD_DECIMAL, &val[i]);

		if (_os_strcmp(input[2], "show") == 0) {
			BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
				    "src=%d, cmprs=%d, grp_num/he=%d/%d\n",
				    cfg->ch_i_data_src, cfg->ch_i_cmprs,
				    cfg->ch_i_grp_num, cfg->ch_i_grp_num_he);
			BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
				    "blk_start/end_=%d/%d, bitmap=0x%x, type=%d, seg_len=%d\n",
				    cfg->ch_i_blk_start_idx, cfg->ch_i_blk_end_idx,
				    cfg->ch_i_ele_bitmap, cfg->ch_i_type, cfg->ch_i_seg_len);
			return;
		} else if (_os_strcmp(input[2], "para0") == 0) {
			if (val[0] == 0) {
				ch_physts->bitmap_type_auto_en = true;
				return;
			}
			ch_physts->bitmap_type_auto_en = false;
			ch_physts->force_bitmap_type = (u16) val[0];

			BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
   				    "force_bitmap_type=0x%x\n", ch_physts->force_bitmap_type);
		} else if (_os_strcmp(input[2], "para1") == 0) {
			HALBB_SCAN(input[5], DCMD_HEX, &val[2]);

			if (val[0] > 1)
				val[0] = 1;

			cfg->ch_i_cmprs = (bool)val[0];

			if (val[1] > 3)
				val[1] = 3;

			cfg->ch_i_grp_num = (u8)val[1];
			cfg->ch_i_grp_num_he = (u8)val[1];
			
			cfg->ch_i_ele_bitmap = val[2];

		} else if (_os_strcmp(input[2], "para2") == 0) {
			cfg->ch_i_seg_len = (u8)val[0];
			BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
				    "seg_len=%d\n",
				    cfg->ch_i_seg_len);
		} else if (_os_strcmp(input[2], "para3") == 0) {
			cfg->ch_i_data_src = (bool)val[0];
			BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
				    "src=%d\n", cfg->ch_i_data_src);
		} else if (_os_strcmp(input[2], "para4") == 0) {
			halbb_ch_trig_select (bb, (u8)val[0]);
			BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
				    "Trigger Event=%d\n", ch_physts->ch_info_event);	
		} else if (_os_strcmp(input[2], "blk") == 0) {
			cfg->ch_i_blk_start_idx = (u8)val[0];
			cfg->ch_i_blk_end_idx = (u8)val[1];
			BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
				    "blk_start/end_=%d/%d\n",
				    cfg->ch_i_blk_start_idx, cfg->ch_i_blk_end_idx);
		} else {
			BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
				    "Set Err\n");
			return;
		}

		if (!halbb_cfg_ch_info_cr(bb, cfg)) {
			BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
				    "Cfg Fail\n");
			return;
		}
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
				    "Ch-info Rpt Len ========>\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
				    "Data=%d Byte, grp=1/%d, H:[%dx%d] (0x%x), per_tone_len=%d\n",
				    1 << cfg->ch_i_cmprs, grp_num_tab[cfg->ch_i_grp_num],
				    size->n_r, size->n_c,
				    cfg->ch_i_ele_bitmap,
				    size->per_tone_ch_rpt_size);		    
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
				    "[Lgcy/HT/VHT]\n");
		for (i = 0; i < CH_INFO_BW_NUM; i++) {
			BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
				    "  [%03dM] Len=%04d Byte, tone_num=%03d\n",
				    (20 << i), size->ch_info_rpt_len[i],
				    size->ch_info_tone_num[i]);
		}
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
				    "[HE]\n");
		for (i = 0; i < CH_INFO_BW_NUM; i++) {
			BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
				    "  [%03dM] Len=%04d Byte, tone_num=%03d\n",
				    (20 << i), size->ch_info_rpt_len_he[i],
				    size->ch_info_tone_num_he[i]);
		}
	} else if (_os_strcmp(input[1], "test") == 0) {
		halbb_mem_set(bb, &phy_rpt_in,0,sizeof(phy_rpt_in));
		halbb_mem_set(bb, &hdr,0,sizeof(hdr));
		halbb_mem_set(bb, &phy_rpt,0,sizeof(phy_rpt));
		halbb_mem_set(bb, &drv_rpt,0,sizeof(drv_rpt));

		HALBB_SCAN(input[2], DCMD_DECIMAL, &val[0]);
		seg_idx = (u8)val[0];

		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "[SEG:%d] Input\n", seg_idx);

		/*Rpt Buff*/
		if (!ch_rpt->test_buf)
			ch_rpt->test_buf = (u8 *)halbb_mem_alloc(bb, (TEST_CH_INFO_MAX_SEG * TEST_CH_SEG_LEN));

		/*Data Buff*/
		addr = (u8 *)halbb_mem_alloc(bb, len);
		addr_ori = addr;

		if (!addr)
			return;

		/*HDR Info*/
		/*seg_0:[2(hdr) + 10(data)] + seg_1:11(data) + seg_2:11(data) + seg_3:6(data) = 40, 40*8=320*/
		ch_rpt_hdr.total_len_l = 320;
		ch_rpt_hdr.total_seg_num = TEST_CH_INFO_MAX_SEG;
		ch_rpt_hdr.set_valid = 1;
		ch_rpt_hdr.segment_size = TEST_CH_SEG_LEN >> 3;
		ch_rpt_hdr.seq_num= seg_idx;
		
		if (seg_idx == TEST_CH_INFO_MAX_SEG - 1) {
			ch_rpt_hdr.is_pkt_end = 1;
		} else {
			ch_rpt_hdr.is_pkt_end = 0;
		}

		halbb_mem_cpy(bb, addr, &ch_rpt_hdr, ch_rpt->ch_rpt_hdr_len);
		addr += ch_rpt->ch_rpt_hdr_len;

		if (seg_idx == 0) {
			phy_rpt_in.rssi[0] = 50;
			phy_rpt_in.rssi[1] = 70;
			phy_rpt_in.rssi_avg = 60;
			phy_rpt_in.rsvd_0 = 0;
			phy_rpt_in.rsvd_1 = 0;
			phy_rpt_in.rsvd_2 = 0;

			halbb_mem_cpy(bb, addr, &phy_rpt_in, ch_rpt->phy_info_len);
			addr += ch_rpt->phy_info_len;
			ch_rpt->test_buf_curr = ch_rpt->test_buf;
		}

		if (seg_idx == TEST_CH_INFO_MAX_SEG - 1) {
		/*SEG3 (LAST)*/
			for (i = 0; i < (TEST_CH_SEG_LEN - 8 - 40); i++) {
				*addr = (u8)i;
				addr++;
			}
		} else if (seg_idx == 0) {
		/*SEG 0*/
			for (i = 0; i < (TEST_CH_SEG_LEN - 16); i++) {
				*addr = (u8)i;
				addr++;
			}
		} else {
		/*SEG 1,2*/
			for (i = 0; i < (TEST_CH_SEG_LEN - 8); i++) {
				*addr = (u8)i;
				addr++;
			}
		}

		addr_tmp = addr_ori;
		for (i = 0; i < (TEST_CH_SEG_LEN >> 3); i++) {
			BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
				    "[%02d] 0x%016llx\n", i, *(u64 *)addr_tmp);
			addr_tmp += 8;
		}

		rpt = halbb_ch_info_parsing(bb, addr_ori, len, &rxdesc, ch_rpt->test_buf_curr, &hdr, &phy_rpt, &drv_rpt);

		ch_rpt->raw_data_len_acc += drv_rpt.raw_data_len;
		ch_rpt->test_buf_curr += drv_rpt.raw_data_len;

		if (rpt == BB_CH_INFO_SUCCESS) {
			BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
				    "[Result] Success\n");
		} else {
			if (rpt == BB_CH_INFO_LAST_SEG) {
				BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
					    "[Result] Success, Last Seg\n");
				halbb_ch_info_print_buf(bb);

			} else {
				BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
					    "[Result] Fail\n");
			}
			halbb_ch_info_reset(bb);
			ch_rpt->raw_data_len_acc = 0;
			ch_rpt->test_buf_curr = ch_rpt->test_buf;
		}
			
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			 "[ACC] remnant=%d, raw_data_len_acc=%d\n",
			ch_rpt->total_len_remnant, ch_rpt->raw_data_len_acc);

		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			 "[SEG] all_len=%d = Hdr_len + raw_data_len=%d\n",
			 len, drv_rpt.raw_data_len);

		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			 "[SEG] total_len=%d, seg_size=%d, end=%d, sts0_evm=%d, seq_num=%d\n",
			 hdr.total_len_l, hdr.segment_size, hdr.is_pkt_end, hdr.sts0_evm, hdr.seq_num);

		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			 "[IN]  SEG[%d]: rssi{0,1,avg}={%02d,%02d,%02d}, sts1_evm=%d, rsvd{0,1,2}={%d,%d,%d}\n",
			 ch_rpt_hdr.seq_num,
			 phy_rpt_in.rssi[0], phy_rpt_in.rssi[1],
			 phy_rpt_in.rssi_avg, (phy_rpt_in.sts1_evm_m << 4 | phy_rpt_in.sts1_evm_l) ,
			 phy_rpt_in.rsvd_0, phy_rpt_in.rsvd_1,
			 phy_rpt_in.rsvd_2);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			 "[OUT] SEG[%d]: rssi{0,1,avg}={%02d,%02d,%02d}, sts1_evm=%d, rsvd{0,1,2}={%d,%d,%d}\n\n",
			 drv_rpt.seg_idx_curr,
			 phy_rpt.rssi[0], phy_rpt.rssi[1],
			 phy_rpt.rssi_avg, (phy_rpt.sts1_evm_m << 4 | phy_rpt.sts1_evm_l),
			 phy_rpt.rsvd_0, phy_rpt.rsvd_1,
			 phy_rpt.rsvd_2);

		halbb_mem_free(bb, addr_ori, len);
	} else {
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "Set Err\n");
	}
}

void halbb_cr_cfg_ch_info_init(struct bb_info *bb)
{
	struct bb_ch_rpt_info *ch_rpt = &bb->bb_ch_rpt_i;
	struct bb_ch_info_cr_info *cr = &ch_rpt->bb_ch_info_cr_i;
	
	switch (bb->cr_type) {

	#ifdef HALBB_COMPILE_AP_SERIES
	case BB_AP:
		cr->ch_info_en_0 = INTF_R_CH_INFO_EN_P0_A;
		cr->ele_bitmap = INTF_R_ELE_BITMAP_A;
		cr->ch_info_type = CH_INFO_TYPE_A;
		cr->ch_info_type_m = CH_INFO_TYPE_A_M;
		cr->seg_len = CH_INFO_SEG_LEN_A;
		cr->seg_len_m = CH_INFO_SEG_LEN_A_M;
		break;
	#endif
	#ifdef HALBB_COMPILE_AP2_SERIES
	case BB_AP2:
		cr->ch_info_en_0 = INTF_R_CH_INFO_EN_P0_A2;
		cr->ele_bitmap = INTF_R_ELE_BITMAP_A2;
		cr->ch_info_type = CH_INFO_TYPE_A2;
		cr->ch_info_type_m = CH_INFO_TYPE_A2_M;
		cr->seg_len = CH_INFO_SEG_LEN_A2;
		cr->seg_len_m = CH_INFO_SEG_LEN_A2_M;
		cr->ch_info_off_powersaving = RX_IN_HBUF_CKEN_P0_A2;
		break;
	#endif
	#ifdef HALBB_COMPILE_CLIENT_SERIES
	case BB_CLIENT:
		cr->ch_info_en_0 = INTF_R_CH_INFO_EN_P0_C;
		cr->ele_bitmap = INTF_R_ELE_BITMAP_C;
		cr->ch_info_type = CH_INFO_TYPE_C;
		cr->ch_info_type_m = CH_INFO_TYPE_C_M;
		cr->seg_len = CH_INFO_SEG_LEN_C;
		cr->seg_len_m = CH_INFO_SEG_LEN_C_M;
		break;
	#endif

	default:
		BB_WARNING("[%s] BBCR Hook FAIL!\n", __func__);
		if (bb->bb_dbg_i.cr_fake_init_hook_en) {
			BB_TRACE("[%s] BBCR fake init\n", __func__);
			halbb_cr_hook_fake_init(bb, (u32 *)cr, (sizeof(struct bb_ch_info_cr_info) >> 2));
		}
		break;
	}

	if (bb->bb_dbg_i.cr_init_hook_recorder_en) {
		BB_TRACE("[%s] BBCR Hook dump\n", __func__);
		halbb_cr_hook_init_dump(bb, (u32 *)cr, (sizeof(struct bb_ch_info_cr_info) >> 2));
	}
}

void halbb_ch_info_close_powersaving(struct bb_info *bb, bool en,
			 enum phl_phy_idx phy_idx)
{
	struct bb_ch_rpt_info *ch_rpt = &bb->bb_ch_rpt_i;
	struct bb_ch_info_cr_info *cr = &ch_rpt->bb_ch_info_cr_i;
	#if (defined(BB_8852C_SUPPORT) || defined(BB_8192XB_SUPPORT))
	if ((bb->ic_type == BB_RTL8852C) || (bb->ic_type == BB_RTL8192XB)){
		if (en) {
			if (phy_idx == HW_PHY_0) {
				halbb_set_reg(bb, cr->ch_info_off_powersaving, 0x1, 1);
			} else {
				halbb_set_reg(bb, cr->ch_info_off_powersaving, 0x2, 1);
			}
		} else {
			if (phy_idx == HW_PHY_0) {
				halbb_set_reg(bb, cr->ch_info_off_powersaving, 0x1, 0);
			} else {
				halbb_set_reg(bb, cr->ch_info_off_powersaving, 0x2, 0);
			}
		}
	}
	#endif
	BB_DBG(bb, DBG_CH_INFO, "[%s], en=%d\n", __func__, en);
}

void halbb_ch_info_modify_ack_rxsc(struct bb_info *bb, struct physts_rxd *desc, u32 len,  u8 *rxsc)
{
	struct bb_ch_rpt_info *ch_rpt = &bb->bb_ch_rpt_i;
	struct bb_ch_rpt_size_info *size = &ch_rpt->bb_ch_rpt_size_i;
	struct bb_ch_info_cr_cfg_info *cur_cfg = &ch_rpt->bb_ch_info_cur_cr_cfg_i;
	u16 calc_tone_num = 0;
	u8 bwidx = 0;
	enum channel_width cbw = bb->hal_com->band[0].cur_chandef.bw;
	
	if (!(desc->data_rate <= BB_24M && desc->data_rate >= BB_06M))
		return;

	calc_tone_num = (u16)HALBB_DIV(len, size->per_tone_ch_rpt_size);
	bwidx = (u8)HALBB_DIV(calc_tone_num, size->tone_num_lgcy[0][cur_cfg->ch_i_grp_num]);
	BB_DBG(bb, DBG_CH_INFO, "[%s], table=%d, rxsc=%d\n",
			__func__, size->tone_num_lgcy[0][cur_cfg->ch_i_grp_num], *rxsc);
	BB_DBG(bb, DBG_CH_INFO, "[%s], per_tone_ch_rpt_size=%d, calc_tone_num=%d, bwidx=%d, cbw=%d\n",
			__func__, size->per_tone_ch_rpt_size, calc_tone_num, bwidx, cbw);
	if (bwidx == 1 || bwidx == 2 || bwidx == 4 || bwidx == 8){
		if (bwidx == BIT(cbw)) {
			*rxsc = RXSC_FULLBW;
		} else if (bwidx == BW_CAP_80M) {
			*rxsc = RXSC_DUPILCATE_80;
		} else if (bwidx == BW_CAP_40M) {
			*rxsc = RXSC_DUPILCATE_40;
		} else {
			*rxsc = RXSC_20;
		}
	} else {
		BB_WARNING("error for bwidx\n");
	}
	BB_DBG(bb, DBG_CH_INFO, "[%s], modify_rxsc=%d, data_rate=%d\n", __func__, *rxsc, desc->data_rate);
}

u8 halbb_ch_info_ack_verify(struct bb_info *bb, u16 *addr, u8 datasize, u16 len)
{
        //datasize: 1, 1byte S(8,4); 2, 2byte S(16,12)
        //verify only for 0x303 (2x2 CH)
	u32 i;
	u32 h11h22, h12h21, chdiff;
	s32 utility = 0;
	u16 tone_num = len/(datasize*2*4); // I+Q, 2x2 CH
	u16 *addr_16;
	u8 *addr_8;
	bool iscablelink = false;
	
	if (datasize == 1) {
		addr_8 = (u8 *)addr;
		for (i = 0; i < tone_num; i++) {
			h11h22 = ((u8)ABS_8(addr_8[i*8+0]))+((u8)ABS_8(addr_8[i*8+1]))
				+((u8)ABS_8(addr_8[i*8+6]))+((u8)ABS_8(addr_8[i*8+7]));
			h12h21 =((u8)ABS_8(addr_8[i*8+2]))+((u8)ABS_8(addr_8[i*8+3]))
				+((u8)ABS_8(addr_8[i*8+4]))+((u8)ABS_8(addr_8[i*8+5]));
			
			chdiff = DIFF_2(h11h22, h12h21);
			BB_DBG(bb, DBG_CH_INFO, "CH Info Verify 1byte: H11H22 = %d, H12H21= %d ,Diff= %d\n", h11h22, h12h21, chdiff);
			
			if (chdiff > 40)
				utility += 2;
			else if (chdiff > 25)
				utility += 1;
			else if (chdiff > 20)
				utility -= 1;
			else
				utility -= 3;
		}
	} else if (datasize == 2) {
		addr_16 = (u16 *)addr;
		for (i = 0; i < tone_num; i++) {
			h11h22 = ((u16)ABS_16(addr_16[i*8+0]))+((u16)ABS_16(addr_16[i*8+1]))
				+((u16)ABS_16(addr_16[i*8+6]))+((u16)ABS_16(addr_16[i*8+7]));
			h12h21 =((u16)ABS_16(addr_16[i*8+2]))+((u16)ABS_16(addr_16[i*8+3]))
				+((u16)ABS_16(addr_16[i*8+4]))+((u16)ABS_16(addr_16[i*8+5]));
		
			chdiff = DIFF_2(h11h22, h12h21);
			BB_DBG(bb, DBG_CH_INFO, "CH Info Verify 2byte: H11H22 = %d, H12H21= %d ,Diff= %d\n", h11h22, h12h21, chdiff);

			if (chdiff > 10000)
				utility += 2;
			else if (chdiff > 7000)
				utility += 1;
			else if (chdiff > 5000)
				utility -= 1;
			else
				utility -= 3;
		}
	}

	if (utility > 0)
		iscablelink = true;

	return iscablelink;
}

#endif

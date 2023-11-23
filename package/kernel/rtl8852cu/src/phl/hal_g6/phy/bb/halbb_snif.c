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
#ifdef HALBB_SNIF_SUPPORT
static const u8 he_sigb_n_user [256] = {
	9, 8, 8, 7, 8, 7, 7, 6,
	8, 7, 7, 6, 7, 6, 5, 3,
	4, 5, 6, 7, 8, 9, 10, 3,
	4, 5, 6, 7, 8, 9, 10, 6,
	7, 8, 9,10, 11, 12, 13, 5,
	6, 7, 8, 9, 10, 11, 12, 5,
	6, 7, 8, 9, 10, 11, 12, 4,
	5, 6, 7, 8, 9, 10, 11, 6,
	7, 8, 9, 10, 11, 12, 13, 5,
	6, 7, 8, 9, 10, 11, 12, 5,
	6, 7, 8, 9, 10, 11, 12, 4,
	5, 6, 7, 8, 9, 10, 11, 2,
	3, 4, 5, 6, 7, 8, 9,10,
	11, 12, 13, 14, 15 ,16, 17, 4,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 3,
	4, 5, 6, 7, 8, 9,10, 11,
	12, 13, 14, 15, 16, 17, 18, 19,
	20, 21, 22, 23, 24, 25, 26, 27,
	28, 29, 30, 31, 32, 33, 34, 35,
	36, 37, 38, 39, 40, 41, 42, 43,
	44, 45, 46, 47, 48, 49, 50, 51,
	52, 53, 54, 55, 56, 57, 58, 59,
	60, 61, 62, 63, 64, 65, 66, 1,
	2, 3, 4, 5, 6, 7, 8, 1,
	2, 3, 4, 5, 6, 7, 8, 1,
	2, 3, 4, 5, 6, 7, 8, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0};

void halbb_sniffer_rpt_reset(struct bb_info *bb)
{
	struct bb_snif_info *snif = &bb->bb_cmn_hooker->bb_snif_i;

	snif->snif_l_sig_len = 0;
	snif->snif_sig_a1_len = 0;
	snif->snif_sig_a2_len = 0;
	snif->snif_sig_b_len = 0;
}

void halbb_sniffer_mode_en(struct bb_info *bb, bool en)
{
	struct bb_snif_info *snif = &bb->bb_cmn_hooker->bb_snif_i;
	struct bb_physts_info *physts = &bb->bb_physts_i;
	u32 u32_tmp = 0;
	u32 snif_ie_bitmap = BIT(IE09_FTR_PLCP_0) | BIT(IE10_FTR_PLCP_EXT);
	u8 i = 0;

	snif->snif_mode_en = en;

	/*Enable IE Pages 9 & 10*/
	for (i = 0; i < PHYSTS_BITMAP_NUM; i++) {
		u32_tmp = halbb_physts_ie_bitmap_get(bb, i) & ~snif_ie_bitmap;

		if (i >= HE_MU) {
			if (en) {
				if (i == HE_MU || i == VHT_MU) {
					u32_tmp |= snif_ie_bitmap;
				} else {
					u32_tmp |= BIT(IE09_FTR_PLCP_0);
				}
			}

			halbb_physts_ie_bitmap_set(bb, i, u32_tmp);
			u32_tmp = halbb_physts_ie_bitmap_get(bb, i);
			physts->bitmap_type[i] = u32_tmp;

			BB_DBG(bb, DBG_SNIFFER, "[IE:%02d] bit_map=0x%08x\n",
			       i, physts->bitmap_type[i]);
		}
	}
}

bool halbb_sniffer_phy_sts_ie_09(struct bb_info *bb)
{
	struct bb_snif_info *snif = &bb->bb_cmn_hooker->bb_snif_i;
	struct bb_physts_info	*physts = &bb->bb_physts_i;
	struct bb_physts_rslt_9_info *psts_9 = &physts->bb_physts_rslt_9_i;
	struct bb_cmn_rpt_info	*cmn_rpt = &bb->bb_cmn_rpt_i;

	//BB_DBG(bb, DBG_SNIFFER, "[IE:09] LSIG=0x%06x, SIGA1=0x%08x, SIGA2=0x%08x\n",
	//       psts_9->l_sig, psts_9->sig_a1, psts_9->sig_a2);

	snif->snif_l_sig = &psts_9->l_sig;
	snif->snif_sig_a1 = &psts_9->sig_a1;
	snif->snif_sig_a2 = &psts_9->sig_a2;

	snif->snif_l_sig_len = 17;

	if (cmn_rpt->bb_rate_i.mode == BB_LEGACY_MODE) {
		snif->snif_sig_a1_len = 0;
		snif->snif_sig_a2_len = 0;
	} else if (cmn_rpt->bb_rate_i.mode == BB_HT_MODE ||
		   cmn_rpt->bb_rate_i.mode == BB_VHT_MODE) {
		
		snif->snif_sig_a1_len = 24; /*SIG_A1[23:0]*/
		snif->snif_sig_a2_len = 10; /*SIG_A2[9:0]*/

	} else if (cmn_rpt->bb_rate_i.mode == BB_HE_MODE) {
		snif->snif_sig_a1_len = 26; /*SIG_A1[25:0]*/
		snif->snif_sig_a2_len = 16; /*SIG_A2[15:0]*/
	} else { /*Lgcy*/
		BB_WARNING("[%s] \n", __func__);
		return false;
	}
	return true;
}

bool halbb_sniffer_phy_sts_ie_10(struct bb_info *bb)
{
	struct bb_snif_info *snif = &bb->bb_cmn_hooker->bb_snif_i;
	struct plcp_hdr_vht_sig_b_info *vht_sig_b = &snif->plcp_hdr_vht_sig_b_i;
	struct bb_physts_info	*physts = &bb->bb_physts_i;
	struct bb_physts_rslt_hdr_info	*psts_h = &physts->bb_physts_rslt_hdr_i;
	struct bb_physts_rslt_1_info	*psts_1 = &physts->bb_physts_rslt_1_i;
	struct bb_physts_rslt_10_info	*psts_10 = &physts->bb_physts_rslt_10_i;
	struct bb_cmn_rpt_info	*cmn_rpt = &bb->bb_cmn_rpt_i;
	u32 sigb_tmp = 0;
	u16 sig_b_len_tmp = 0;
	u32 sig_b_len = 0, vht_mcs = 0; 

	BB_DBG(bb, DBG_SNIFFER, "[IE:10] snif_sig_b_len=%d\n", snif->snif_sig_b_len);

	if (cmn_rpt->bb_rate_i.mode == BB_VHT_MODE ||
	    cmn_rpt->bb_rate_i.mode == BB_HE_MODE) {
		snif->snif_sig_b = (u32 *)psts_10->sigb_raw_data_bits_addr;
		snif->snif_sig_b_len = psts_10->sigb_len;

		sigb_tmp = *snif->snif_sig_b;
		sig_b_len_tmp = snif->snif_sig_b_len;
	} else {
		BB_WARNING("[%s] rate_mode=%d\n", __func__, cmn_rpt->bb_rate_i.mode);
		return false;
	}

	if (cmn_rpt->bb_rate_i.mode == BB_VHT_MODE) {
		if (psts_h->ie_map_type == VHT_MU) {
			if (psts_1->bw_idx == CHANNEL_WIDTH_20) {
				vht_sig_b->sigb_len_l = sigb_tmp & 0xffff;
				vht_sig_b->vht_mcs = (sigb_tmp >> 16) & 0xf;
			} else if (psts_1->bw_idx == CHANNEL_WIDTH_40) {
				vht_sig_b->sigb_len_l = sigb_tmp & 0x1ffff;
				vht_sig_b->vht_mcs = (sigb_tmp >> 17) & 0xf;
			} else { /*80, 160, 80+80*/
				vht_sig_b->sigb_len_l = sigb_tmp & 0x7ffff;
				vht_sig_b->vht_mcs = (sigb_tmp >> 19) & 0xf;
			}
		} else { /*su*/
			vht_sig_b->vht_mcs = 0xff;
			if (psts_1->bw_idx == CHANNEL_WIDTH_20) {
				vht_sig_b->sigb_len_l = sigb_tmp & 0x1ffff;
			} else if (psts_1->bw_idx == CHANNEL_WIDTH_40) {
				vht_sig_b->sigb_len_l = sigb_tmp & 0x7ffff;
			} else { /*80, 160, 80+80*/
				vht_sig_b->sigb_len_l = sigb_tmp & 0x1fffff;
			}
		}
		
	} else { /*HE*/
		snif->snif_sig_b_len = 0;
	}

	return true;
}

bool halbb_sniffer_phy_sts(struct bb_info *bb, struct physts_result *rpt,
			   u32 physts_bitmap, struct physts_rxd *desc)
{
	struct bb_snif_info *snif = &bb->bb_cmn_hooker->bb_snif_i;
	struct bb_physts_info	*physts = &bb->bb_physts_i;
	struct bb_physts_rslt_hdr_info	*psts_h = &physts->bb_physts_rslt_hdr_i;
	struct bb_physts_rslt_9_info *psts_9 = &physts->bb_physts_rslt_9_i;
	struct bb_cmn_rpt_info	*cmn_rpt = &bb->bb_cmn_rpt_i;
	bool rpt_valid = true;

	if (!snif->snif_mode_en)
		return false;

	if (physts->bb_physts_rslt_hdr_i.ie_map_type <= DL_MU_SPOOFING)
		return false;

	if (psts_h->ie_map_type == CCK_PKT)
		return false;

	if ((physts_bitmap & (BIT(IE09_FTR_PLCP_0) | BIT(IE10_FTR_PLCP_EXT))) == 0)
		return false;

	halbb_print_rate_2_buff(bb, desc->data_rate, desc->gi_ltf, bb->dbg_buf, HALBB_SNPRINT_SIZE);

	BB_DBG(bb, DBG_SNIFFER, "[%d][%s] bitmap=0x%08x, Rate= %s (0x%x-%x), rate_mode=%d\n",
	       psts_h->ie_map_type, 
	       bb_physts_bitmap_type_t[psts_h->ie_map_type],
	       physts_bitmap,
	       bb->dbg_buf, desc->data_rate, desc->gi_ltf,
	       cmn_rpt->bb_rate_i.mode);

	halbb_sniffer_rpt_reset(bb);
	snif->snif_rate = desc->data_rate;
	snif->snif_rate_mode = (u8)cmn_rpt->bb_rate_i.mode;
	snif->snif_ie_bitmap_type = psts_h->ie_map_type;	

	if (physts_bitmap & BIT(IE09_FTR_PLCP_0)) {
		rpt_valid &= halbb_sniffer_phy_sts_ie_09(bb);
		BB_DBG(bb, DBG_SNIFFER, "[IE:09] LSIG=0x%06x(%d), SIGA1=0x%08x(%d), SIGA2=0x%08x(%d)\n",
		       (snif->snif_l_sig)[0], snif->snif_l_sig_len,
		       (snif->snif_sig_a1)[0], snif->snif_sig_a1_len,
		       (snif->snif_sig_a2)[0], snif->snif_sig_a2_len);
	}

	if (physts_bitmap & BIT(IE10_FTR_PLCP_EXT)) {
		rpt_valid &= halbb_sniffer_phy_sts_ie_10(bb);
	}

	rpt->bb_snif_i = snif;

	BB_DBG(bb, DBG_SNIFFER, "valid=%d\n", rpt_valid);
	return rpt_valid;
}

void halbb_sniffer_phy_sts_init(struct bb_info *bb)
{
	struct bb_snif_info *snif = &bb->bb_cmn_hooker->bb_snif_i;

	BB_DBG(bb, DBG_SNIFFER, "[%s] \n", __func__);
}

void halbb_snif_dbg(struct bb_info *bb, char input[][16], u32 *_used,
		    char *output, u32 *_out_len)
{
	u32 val[10] = {0};

	if (_os_strcmp(input[1], "-h") == 0) {
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "en {val}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "ofdma_1 {bss_color} {sta_id}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "vht_mu {PMAC_Ctrl_user_idx_en} {user_idx}\n");
		return;
	}

	if (_os_strcmp(input[1], "en") == 0) {
		HALBB_SCAN(input[2], DCMD_HEX, &val[0]);
		halbb_sniffer_mode_en(bb, (bool)val[0]);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "Sniffer Mode en = %d\n", val[0]);
		return;
	} else if (_os_strcmp(input[1], "ofdma_1") == 0) {
		HALBB_SCAN(input[2], DCMD_HEX, &val[0]);
		HALBB_SCAN(input[3], DCMD_HEX, &val[1]);

		halbb_set_bss_color(bb, (u8)val[0], bb->bb_phy_idx);
		halbb_set_sta_id(bb, (u16)val[1], bb->bb_phy_idx);

		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "Set bss_color=%d, sta_id = %d\n", val[0], val[1]);
	} else if (_os_strcmp(input[1], "vht_mu") == 0) {
		HALBB_SCAN(input[2], DCMD_DECIMAL, &val[0]);
		HALBB_SCAN(input[3], DCMD_DECIMAL, &val[1]);

		halbb_set_vht_mu_user_idx(bb, (bool)val[0], (u8)val[1], bb->bb_phy_idx);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "Set vht_mu PMAC_Ctrl_en=%d user_idx= %d\n", val[0], val[1]);
	} else {
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "Err\n");
	}
}

#endif

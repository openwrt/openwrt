/******************************************************************************
 *
 * Copyright(c) 2007 - 2021 Realtek Corporation.
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

#ifndef _HALBB_SPUR_SUPPRESS_H_
#define _HALBB_SPUR_SUPPRESS_H_

/*@--------------------------[Define]-------------------------------------*/
#define CARRIER_SPACING_312_5 312500 // 312.5 kHz
#define CARRIER_SPACING_78_125 78125 // 78.125 kHz
#define MAX_TONE_NUM 2048

struct bb_spur_cr_info {
	u32 seg0_set1_csi_tone_idx;
	u32 seg0_set1_csi_tone_idx_m;
	u32 seg0_set1_csi_en;
	u32 seg0_set1_csi_en_m;
	u32 path0_notch_nbi_idx;
	u32 path0_notch_nbi_idx_m;
	u32 path0_notch_nbi_frac_idx;
	u32 path0_notch_nbi_frac_idx_m;
	u32 path0_notch_nbi_en;
	u32 path0_notch_nbi_en_m;
	u32 path1_notch_nbi_idx;
	u32 path1_notch_nbi_idx_m;
	u32 path1_notch_nbi_frac_idx;
	u32 path1_notch_nbi_frac_idx_m;
	u32 path1_notch_nbi_en;
	u32 path1_notch_nbi_en_m;
	u32 path0_notch2_nbi_idx;
	u32 path0_notch2_nbi_idx_m;
	u32 path0_notch2_nbi_frac_idx;
	u32 path0_notch2_nbi_frac_idx_m;
	u32 path0_notch2_nbi_en;
	u32 path0_notch2_nbi_en_m;
	u32 path1_notch2_nbi_idx;
	u32 path1_notch2_nbi_idx_m;
	u32 path1_notch2_nbi_frac_idx;
	u32 path1_notch2_nbi_frac_idx_m;
	u32 path1_notch2_nbi_en;
	u32 path1_notch2_nbi_en_m;
};
struct bb_spur_info {
	struct bb_spur_cr_info	bb_spur_cr_i;
};

void halbb_csi_tone_idx(struct bb_info *bb, u8 central_ch,
                        enum channel_width bw, enum band_type band,
                        enum phl_phy_idx phy_idx);
void halbb_nbi_tone_idx(struct bb_info *bb, u8 central_ch, u8 pri_ch,
                        enum channel_width bw, enum band_type band,
                        enum rf_path path);
void halbb_cr_cfg_spur_init(struct bb_info *bb);
void halbb_spur_dbg(struct bb_info *bb, char input[][16], u32 *_used,
                    char *output, u32 *_out_len);
void halbb_fwofld_csi_tone_idx(struct bb_info *bb, u8 central_ch,
                        enum channel_width bw, enum band_type band,
                        enum phl_phy_idx phy_idx);
void halbb_fwofld_nbi_tone_idx(struct bb_info *bb, u8 central_ch, u8 pri_ch,
                        enum channel_width bw, enum band_type band,
                        enum rf_path path);
#endif


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

#include "halbb_precomp.h"

bool halbb_spur_location(struct bb_info *bb, u8 central_ch,
                         enum channel_width bw, enum band_type band, u32 *intf)
{
	bool rpt = false;

	switch (bb->ic_type) {

	#ifdef BB_8852A_2_SUPPORT
	case BB_RTL8852A:
		break;
	#endif

	#ifdef BB_8852B_SUPPORT
	case BB_RTL8852B:
		break;
	#endif

	#ifdef BB_8852C_SUPPORT
	case BB_RTL8852C:
		rpt = halbb_spur_location_8852c(bb, central_ch, bw, band, intf);
		break;
	#endif

	#ifdef BB_8192XB_SUPPORT
	case BB_RTL8192XB:
		break;
	#endif

	default:
		break;
	}

	return rpt;
}

bool halbb_spur_location_for_CSI(struct bb_info *bb, u8 central_ch,
                         enum channel_width bw, enum band_type band, u32 *intf)
{
	bool rpt = false;

	switch (bb->ic_type) {

	#ifdef BB_8852C_SUPPORT
	case BB_RTL8852C:
                rpt = halbb_spur_location_for_CSI_8852c(bb, central_ch, bw, band, intf);
		break;
	#endif

	default:
		break;
	}

	return rpt;
}


void halbb_csi_tone_idx(struct bb_info *bb, u8 central_ch,
                        enum channel_width bw, enum band_type band,
                        enum phl_phy_idx phy_idx)
{
        /* CSI tone index:  [-1024] --- [0] --- [1023] */
        struct bb_spur_cr_info *cr = &bb->bb_cmn_hooker->bb_spur_i.bb_spur_cr_i;
        u32 fc, intf, intf_csi ; //Central freq. (MHz) & Spur freq. (MHz)
        s32 f_diff, csi_idx;
        bool spur_chk;

        if ((bb->ic_type == BB_RTL8852A) || (bb->ic_type == BB_RTL8852B) || (bb->ic_type == BB_RTL8851B)) {
                BB_DBG(bb, DBG_DBG_API, "[Spur] IC not support!\n");
                return;
        }
        
        halbb_spur_location_for_CSI(bb, central_ch, bw, band, &intf_csi);

        if ((bw == CHANNEL_WIDTH_80) && (band == BAND_ON_6G) && (bb->phl_com->dev_cap.rfe_type == 5))
                spur_chk = false;
        else
                spur_chk = halbb_spur_location(bb, central_ch, bw, band, &intf);

        if (spur_chk) {
                BB_DBG(bb, DBG_DBG_API, "[Spur] Enable CSI\n");
                fc = (u32)halbb_fc_mapping(bb, band, central_ch);
                //Enable CSI
                f_diff = (intf - fc) * 1000000;
                csi_idx = (f_diff + CARRIER_SPACING_78_125 / 2) / CARRIER_SPACING_78_125;
                // Floor of negative number
                if ((f_diff < 0) && (((f_diff + CARRIER_SPACING_78_125 / 2) % CARRIER_SPACING_78_125) != 0))
                        csi_idx -= 1;

                csi_idx = (csi_idx + MAX_TONE_NUM) % MAX_TONE_NUM;

                halbb_set_reg_cmn(bb, cr->seg0_set1_csi_tone_idx, cr->seg0_set1_csi_tone_idx_m, csi_idx, phy_idx);
                halbb_set_reg_cmn(bb, cr->seg0_set1_csi_en, cr->seg0_set1_csi_en_m, 1, phy_idx);

                BB_DBG(bb, DBG_DBG_API, "[Spur] fc=%d, intf=%d, f_diff=%d (MHz)\n",
	               fc, intf, (f_diff / 1000000));
                BB_DBG(bb, DBG_DBG_API, "[Spur] csi_idx=%d\n", csi_idx);
        } else {
                BB_DBG(bb, DBG_DBG_API, "[Spur] Disable CSI\n");
                // Disable CSI
                halbb_set_reg_cmn(bb, cr->seg0_set1_csi_en, cr->seg0_set1_csi_en_m, 0, phy_idx);
        }
}

void halbb_nbi_tone_idx(struct bb_info *bb, u8 central_ch, u8 pri_ch,
                        enum channel_width bw, enum band_type band,
                        enum rf_path path)
{
        /* NBI tone index:
        @  Bw20:                    [0] - [32] - [63]
        @  Bw40:             [-64] ------- [0] ------- [63]
        @  Bw80:  [-128] ----------------- [0] ----------------- [128]
        @  Bw160: [-128] ----------------- [0] ----------------- [128] (Upper 80) +
        @         [-128] ----------------- [0] ----------------- [128] (Lower 80)
        */
       struct bb_spur_cr_info *cr = &bb->bb_cmn_hooker->bb_spur_i.bb_spur_cr_i;
        u16 tone_para = 0;
        u32 fc, intf; //Central freq. (MHz) & Spur freq. (MHz)
        s32 f_diff, nbi_idx, nbi_idx_tmp = 0, nbi_frac_idx, nbi_frac_tmp = 0;
        // CR
        u32 notch1_nbi_idx[2] = {cr->path0_notch_nbi_idx, cr->path1_notch_nbi_idx};
        u32 notch1_nbi_idx_m[2] = {cr->path0_notch_nbi_idx_m, cr->path1_notch_nbi_idx_m};
        u32 notch1_nbi_frac_idx[2] = {cr->path0_notch_nbi_frac_idx, cr->path1_notch_nbi_frac_idx};
        u32 notch1_nbi_frac_idx_m[2] = {cr->path0_notch_nbi_frac_idx_m, cr->path1_notch_nbi_frac_idx_m};
        u32 notch1_nbi_en[2] = {cr->path0_notch_nbi_en, cr->path1_notch_nbi_en};
        u32 notch1_nbi_en_m[2] = {cr->path0_notch_nbi_en_m, cr->path1_notch_nbi_en_m};
        u32 notch2_nbi_idx[2] = {cr->path0_notch2_nbi_idx, cr->path1_notch2_nbi_idx};
        u32 notch2_nbi_idx_m[2] = {cr->path0_notch2_nbi_idx_m, cr->path1_notch2_nbi_idx_m};
        u32 notch2_nbi_frac_idx[2] = {cr->path0_notch2_nbi_frac_idx, cr->path1_notch2_nbi_frac_idx};
        u32 notch2_nbi_frac_idx_m[2] = {cr->path0_notch2_nbi_frac_idx_m, cr->path1_notch2_nbi_frac_idx_m};
        u32 notch2_nbi_en[2] = {cr->path0_notch2_nbi_en, cr->path1_notch2_nbi_en};
        u32 notch2_nbi_en_m[2] = {cr->path0_notch2_nbi_en_m, cr->path1_notch2_nbi_en_m};
        bool spur_chk, notch2_chk = false;

        if (path >= HALBB_MAX_PATH || path < RF_PATH_A)
                return;

        if ((bb->ic_type == BB_RTL8852A) || (bb->ic_type == BB_RTL8852B) || (bb->ic_type == BB_RTL8851B)) {
                BB_DBG(bb, DBG_DBG_API, "[Spur] IC not support!\n");
                return;
        }

        if ((bw == CHANNEL_WIDTH_80) && (band == BAND_ON_6G) && (bb->phl_com->dev_cap.rfe_type == 5)) {
                if (path == RF_PATH_B)
                        spur_chk = false;
                else
                        spur_chk = halbb_spur_location(bb, central_ch, bw, band, &intf);
        } else {
                spur_chk = halbb_spur_location(bb, central_ch, bw, band, &intf);
        }

        if (spur_chk) {
                BB_DBG(bb, DBG_DBG_API, "[Spur] Enable NBI\n");
                fc = (u32)halbb_fc_mapping(bb, band, central_ch);
                //Enable CSI
                if (bw == CHANNEL_WIDTH_160) {
                        fc = (intf > fc) ? fc + 40 : fc - 40;
                        if (((fc > intf) && (central_ch < pri_ch)) ||
                            ((fc < intf) && (central_ch > pri_ch)))
                                notch2_chk = true;
                }

                f_diff = (intf - fc) * 1000000;
                // NBI idx
                nbi_idx = HALBB_DIV(f_diff, CARRIER_SPACING_312_5);
                // Floor of negative number
                if ((f_diff < 0) && ((f_diff % CARRIER_SPACING_312_5) != 0)) {
                        nbi_idx -= 1;
                        nbi_frac_tmp = (f_diff / CARRIER_SPACING_312_5) - 1;
                } else {
                        nbi_frac_tmp = (f_diff / CARRIER_SPACING_312_5);
                }

                if (bw == CHANNEL_WIDTH_20) {
                        nbi_idx =  (nbi_idx + 32) % 64;
                } else {
                        tone_para = (bw == CHANNEL_WIDTH_40) ? 128 : 256;
                        nbi_idx =  (nbi_idx + tone_para) % tone_para;
                }
                // NBI fraction idx
                nbi_frac_idx = f_diff - CARRIER_SPACING_312_5 * nbi_frac_tmp;
                nbi_frac_idx = HALBB_DIV_ROUND(nbi_frac_idx, CARRIER_SPACING_78_125);

                if ((bw == CHANNEL_WIDTH_160) && notch2_chk) {
                        halbb_set_reg(bb, notch2_nbi_idx[path], notch2_nbi_idx_m[path], nbi_idx);
                        halbb_set_reg(bb, notch2_nbi_frac_idx[path], notch2_nbi_frac_idx_m[path], nbi_frac_idx);
                        halbb_set_reg(bb, notch2_nbi_en[path], notch2_nbi_en_m[path], 0);
                        halbb_set_reg(bb, notch2_nbi_en[path], notch2_nbi_en_m[path], 1);
                        halbb_set_reg(bb, notch1_nbi_en[path], notch1_nbi_en_m[path], 0);
                } else {
                        halbb_set_reg(bb, notch1_nbi_idx[path], notch1_nbi_idx_m[path], nbi_idx);
                        halbb_set_reg(bb, notch1_nbi_frac_idx[path], notch1_nbi_frac_idx_m[path], nbi_frac_idx);
                        halbb_set_reg(bb, notch1_nbi_en[path], notch1_nbi_en_m[path], 0);
                        halbb_set_reg(bb, notch1_nbi_en[path], notch1_nbi_en_m[path], 1);
                        halbb_set_reg(bb, notch2_nbi_en[path], notch2_nbi_en_m[path], 0);
                }
                BB_DBG(bb, DBG_DBG_API, "[Spur] fc=%d, intf=%d, f_diff=%d (MHz)\n",
	               fc, intf, (f_diff / 1000000));
                BB_DBG(bb, DBG_DBG_API, "[Spur] nbi_idx=%d, nbi_frac_idx=%d\n",
	               nbi_idx, nbi_frac_idx);
        } else {
                BB_DBG(bb, DBG_DBG_API, "[Spur] Disable NBI\n");
                // Disable NBI
                halbb_set_reg(bb, notch1_nbi_en[path], notch1_nbi_en_m[path], 0);
                halbb_set_reg(bb, notch2_nbi_en[path], notch2_nbi_en_m[path], 0);
        }
}

#ifdef HALBB_FW_OFLD_SUPPORT
        void halbb_fwofld_csi_tone_idx(struct bb_info *bb, u8 central_ch,
                                enum channel_width bw, enum band_type band,
                                enum phl_phy_idx phy_idx)
        {
                /* CSI tone index:  [-1024] --- [0] --- [1023] */
                struct bb_spur_cr_info *cr = &bb->bb_cmn_hooker->bb_spur_i.bb_spur_cr_i;
                u32 fc, intf; //Central freq. (MHz) & Spur freq. (MHz)
                s32 f_diff, csi_idx;
                bool spur_chk;

                if ((bb->ic_type == BB_RTL8852A) || (bb->ic_type == BB_RTL8852B) || (bb->ic_type == BB_RTL8851B)) {
                        BB_DBG(bb, DBG_DBG_API, "[Spur] IC not support!\n");
                        return;
                }

                spur_chk = halbb_spur_location(bb, central_ch, bw, band, &intf);

                if (spur_chk) {
                        BB_DBG(bb, DBG_DBG_API, "[Spur] Enable CSI\n");
                        fc = (u32)halbb_fc_mapping(bb, band, central_ch);
                        //Enable CSI
                        f_diff = (intf - fc) * 1000000;
                        csi_idx = (f_diff + CARRIER_SPACING_78_125 / 2) / CARRIER_SPACING_78_125;
                        // Floor of negative number
                        if ((f_diff < 0) && (((f_diff + CARRIER_SPACING_78_125 / 2) % CARRIER_SPACING_78_125) != 0))
                                csi_idx -= 1;

                        csi_idx = (csi_idx + MAX_TONE_NUM) % MAX_TONE_NUM;

                        halbb_fw_set_reg_cmn(bb, cr->seg0_set1_csi_tone_idx, cr->seg0_set1_csi_tone_idx_m, csi_idx, phy_idx, 0);
                        halbb_fw_set_reg_cmn(bb, cr->seg0_set1_csi_en, cr->seg0_set1_csi_en_m, 1, phy_idx, 0);

                        BB_DBG(bb, DBG_DBG_API, "[Spur] fc=%d, intf=%d, f_diff=%d (MHz)\n",
                        fc, intf, (f_diff / 1000000));
                        BB_DBG(bb, DBG_DBG_API, "[Spur] csi_idx=%d\n", csi_idx);
                } else {
                        BB_DBG(bb, DBG_DBG_API, "[Spur] Disable CSI\n");
                        // Disable CSI
                        halbb_fw_set_reg_cmn(bb, cr->seg0_set1_csi_en, cr->seg0_set1_csi_en_m, 0, phy_idx, 0);
                }
        }

        void halbb_fwofld_nbi_tone_idx(struct bb_info *bb, u8 central_ch, u8 pri_ch,
                                enum channel_width bw, enum band_type band,
                                enum rf_path path)
        {
                /* NBI tone index:
                @  Bw20:                    [0] - [32] - [63]
                @  Bw40:             [-64] ------- [0] ------- [63]
                @  Bw80:  [-128] ----------------- [0] ----------------- [128]
                @  Bw160: [-128] ----------------- [0] ----------------- [128] (Upper 80) +
                @         [-128] ----------------- [0] ----------------- [128] (Lower 80)
                */
        struct bb_spur_cr_info *cr = &bb->bb_cmn_hooker->bb_spur_i.bb_spur_cr_i;
                u16 tone_para = 0;
                u32 fc, intf; //Central freq. (MHz) & Spur freq. (MHz)
                s32 f_diff, nbi_idx, nbi_idx_tmp = 0, nbi_frac_idx, nbi_frac_tmp = 0;
                // CR
                u32 notch1_nbi_idx[2] = {cr->path0_notch_nbi_idx, cr->path1_notch_nbi_idx};
                u32 notch1_nbi_idx_m[2] = {cr->path0_notch_nbi_idx_m, cr->path1_notch_nbi_idx_m};
                u32 notch1_nbi_frac_idx[2] = {cr->path0_notch_nbi_frac_idx, cr->path1_notch_nbi_frac_idx};
                u32 notch1_nbi_frac_idx_m[2] = {cr->path0_notch_nbi_frac_idx_m, cr->path1_notch_nbi_frac_idx_m};
                u32 notch1_nbi_en[2] = {cr->path0_notch_nbi_en, cr->path1_notch_nbi_en};
                u32 notch1_nbi_en_m[2] = {cr->path0_notch_nbi_en_m, cr->path1_notch_nbi_en_m};
                u32 notch2_nbi_idx[2] = {cr->path0_notch2_nbi_idx, cr->path1_notch2_nbi_idx};
                u32 notch2_nbi_idx_m[2] = {cr->path0_notch2_nbi_idx_m, cr->path1_notch2_nbi_idx_m};
                u32 notch2_nbi_frac_idx[2] = {cr->path0_notch2_nbi_frac_idx, cr->path1_notch2_nbi_frac_idx};
                u32 notch2_nbi_frac_idx_m[2] = {cr->path0_notch2_nbi_frac_idx_m, cr->path1_notch2_nbi_frac_idx_m};
                u32 notch2_nbi_en[2] = {cr->path0_notch2_nbi_en, cr->path1_notch2_nbi_en};
                u32 notch2_nbi_en_m[2] = {cr->path0_notch2_nbi_en_m, cr->path1_notch2_nbi_en_m};
                bool spur_chk, notch2_chk = false;

                if (path >= HALBB_MAX_PATH || path < RF_PATH_A)
                        return;

                if ((bb->ic_type == BB_RTL8852A) || (bb->ic_type == BB_RTL8852B) || (bb->ic_type == BB_RTL8851B)) {
                        BB_DBG(bb, DBG_DBG_API, "[Spur] IC not support!\n");
                        return;
                }

                spur_chk = halbb_spur_location(bb, central_ch, bw, band, &intf);

                if (spur_chk) {
                        BB_DBG(bb, DBG_DBG_API, "[Spur] Enable NBI\n");
                        fc = (u32)halbb_fc_mapping(bb, band, central_ch);
                        //Enable CSI
                        if (bw == CHANNEL_WIDTH_160) {
                                fc = (intf > fc) ? fc + 40 : fc - 40;
                                if (((fc > intf) && (central_ch < pri_ch)) ||
                                ((fc < intf) && (central_ch > pri_ch)))
                                        notch2_chk = true;
                        }

                        f_diff = (intf - fc) * 1000000;
                        // NBI idx
                        nbi_idx = HALBB_DIV(f_diff, CARRIER_SPACING_312_5);
                        // Floor of negative number
                        if ((f_diff < 0) && ((f_diff % CARRIER_SPACING_312_5) != 0)) {
                                nbi_idx -= 1;
                                nbi_frac_tmp = (f_diff / CARRIER_SPACING_312_5) - 1;
                        } else {
                                nbi_frac_tmp = (f_diff / CARRIER_SPACING_312_5);
                        }

                        if (bw == CHANNEL_WIDTH_20) {
                                nbi_idx =  (nbi_idx + 32) % 64;
                        } else {
                                tone_para = (bw == CHANNEL_WIDTH_40) ? 128 : 256;
                                nbi_idx =  (nbi_idx + tone_para) % tone_para;
                        }
                        // NBI fraction idx
                        nbi_frac_idx = f_diff - CARRIER_SPACING_312_5 * nbi_frac_tmp;
                        nbi_frac_idx = HALBB_DIV_ROUND(nbi_frac_idx, CARRIER_SPACING_78_125);

                        if ((bw == CHANNEL_WIDTH_160) && notch2_chk) {
                                halbb_fw_set_reg(bb, notch2_nbi_idx[path], notch2_nbi_idx_m[path], nbi_idx, 0);
                                halbb_fw_set_reg(bb, notch2_nbi_frac_idx[path], notch2_nbi_frac_idx_m[path], nbi_frac_idx, 0);
                                halbb_fw_set_reg(bb, notch2_nbi_en[path], notch2_nbi_en_m[path], 0, 0);
                                halbb_fw_set_reg(bb, notch2_nbi_en[path], notch2_nbi_en_m[path], 1, 0);
                                halbb_fw_set_reg(bb, notch1_nbi_en[path], notch1_nbi_en_m[path], 0, 0);
                        } else {
                                halbb_fw_set_reg(bb, notch1_nbi_idx[path], notch1_nbi_idx_m[path], nbi_idx, 0);
                                halbb_fw_set_reg(bb, notch1_nbi_frac_idx[path], notch1_nbi_frac_idx_m[path], nbi_frac_idx, 0);
                                halbb_fw_set_reg(bb, notch1_nbi_en[path], notch1_nbi_en_m[path], 0, 0);
                                halbb_fw_set_reg(bb, notch1_nbi_en[path], notch1_nbi_en_m[path], 1, 0);
                                halbb_fw_set_reg(bb, notch2_nbi_en[path], notch2_nbi_en_m[path], 0, 0);
                        }
                        BB_DBG(bb, DBG_DBG_API, "[Spur] fc=%d, intf=%d, f_diff=%d (MHz)\n",
                        fc, intf, (f_diff / 1000000));
                        BB_DBG(bb, DBG_DBG_API, "[Spur] nbi_idx=%d, nbi_frac_idx=%d\n",
                        nbi_idx, nbi_frac_idx);
                } else {
                        BB_DBG(bb, DBG_DBG_API, "[Spur] Disable NBI\n");
                        // Disable NBI
                        halbb_fw_set_reg(bb, notch1_nbi_en[path], notch1_nbi_en_m[path], 0, 0);
                        halbb_fw_set_reg(bb, notch2_nbi_en[path], notch2_nbi_en_m[path], 0, 0);
                }
        }
#endif

void halbb_spur_dbg(struct bb_info *bb, char input[][16], u32 *_used,
                    char *output, u32 *_out_len)
{
	u32 val[10] = {0};
	u32 used = *_used;
	u32 out_len = *_out_len;
	u8 tmp = 0, i = 0;
	u32 j = 0;

	if (_os_strcmp(input[1], "-h") == 0) {
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "nbi_en ({central_ch} {pri_ch} {bw} {band} {path})\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "csi_en ({central_ch} {bw} {band} {phy_idx})\n");
	} else if (_os_strcmp(input[1], "nbi_en") == 0) {
		HALBB_SCAN(input[2], DCMD_DECIMAL, &val[0]);
		HALBB_SCAN(input[3], DCMD_DECIMAL, &val[1]);
		HALBB_SCAN(input[4], DCMD_DECIMAL, &val[2]);
		HALBB_SCAN(input[5], DCMD_DECIMAL, &val[3]);
                HALBB_SCAN(input[6], DCMD_DECIMAL, &val[4]);
                halbb_nbi_tone_idx(bb, (u8)val[0], (u8)val[1],
                                   (enum channel_width)val[2],
                                   (enum band_type)val[3],
                                   (enum rf_path)val[4]);
	}  else if (_os_strcmp(input[1], "csi_en") == 0) {
		HALBB_SCAN(input[2], DCMD_DECIMAL, &val[0]);
		HALBB_SCAN(input[3], DCMD_DECIMAL, &val[1]);
		HALBB_SCAN(input[4], DCMD_DECIMAL, &val[2]);
		HALBB_SCAN(input[5], DCMD_DECIMAL, &val[3]);
                halbb_csi_tone_idx(bb, (u8)val[0], (enum channel_width)val[1],
                                   (enum band_type)val[2],
                                   (enum phl_phy_idx)val[3]);
	}

	*_used = used;
	*_out_len = out_len;
}

void halbb_cr_cfg_spur_init(struct bb_info *bb)
{
	struct bb_spur_cr_info *cr = &bb->bb_cmn_hooker->bb_spur_i.bb_spur_cr_i;

	switch (bb->cr_type) {

	#ifdef HALBB_COMPILE_AP_SERIES
	case BB_AP:
		cr->seg0_set1_csi_tone_idx = SEG0_SET1_CSI_WGT_TONE_IDX_A;
                cr->seg0_set1_csi_tone_idx_m = SEG0_SET1_CSI_WGT_TONE_IDX_A_M;
                cr->seg0_set1_csi_en = SEG0_SET1_CSI_WGT_EN_A;
                cr->seg0_set1_csi_en_m = SEG0_SET1_CSI_WGT_EN_A_M;
                cr->path0_notch_nbi_idx = PATH0_R_NBI_IDX_A;
                cr->path0_notch_nbi_idx_m = PATH0_R_NBI_IDX_A_M;
                cr->path0_notch_nbi_frac_idx = PATH0_R_NBI_FRAC_IDX_A;
                cr->path0_notch_nbi_frac_idx_m = PATH0_R_NBI_FRAC_IDX_A_M;
                cr->path0_notch_nbi_en = PATH0_R_NBI_NOTCH_EN_A;
                cr->path0_notch_nbi_en_m = PATH0_R_NBI_NOTCH_EN_A_M;
                cr->path1_notch_nbi_idx = PATH1_R_NBI_IDX_A;
                cr->path1_notch_nbi_idx_m = PATH1_R_NBI_IDX_A_M;
                cr->path1_notch_nbi_frac_idx = PATH1_R_NBI_FRAC_IDX_A;
                cr->path1_notch_nbi_frac_idx_m = PATH1_R_NBI_FRAC_IDX_A_M;
                cr->path1_notch_nbi_en = PATH1_R_NBI_NOTCH_EN_A;
                cr->path1_notch_nbi_en_m = PATH1_R_NBI_NOTCH_EN_A_M;
		break;

	#endif
	#ifdef HALBB_COMPILE_AP2_SERIES
	case BB_AP2:
                cr->seg0_set1_csi_tone_idx = SEG0_SET1_CSI_WGT_TONE_IDX_A2;
                cr->seg0_set1_csi_tone_idx_m = SEG0_SET1_CSI_WGT_TONE_IDX_A2_M;
                cr->seg0_set1_csi_en = SEG0_SET1_CSI_WGT_EN_A2;
                cr->seg0_set1_csi_en_m = SEG0_SET1_CSI_WGT_EN_A2_M;
                cr->path0_notch_nbi_idx = PATH0_NOTCH_R_NBI_IDX_A2;
                cr->path0_notch_nbi_idx_m = PATH0_NOTCH_R_NBI_IDX_A2_M;
                cr->path0_notch_nbi_frac_idx = PATH0_NOTCH_R_NBI_FRAC_IDX_A2;
                cr->path0_notch_nbi_frac_idx_m = PATH0_NOTCH_R_NBI_FRAC_IDX_A2_M;
                cr->path0_notch_nbi_en = PATH0_NOTCH_R_NBI_NOTCH_EN_A2;
                cr->path0_notch_nbi_en_m = PATH0_NOTCH_R_NBI_NOTCH_EN_A2_M;
                cr->path1_notch_nbi_idx = PATH1_NOTCH_R_NBI_IDX_A2;
                cr->path1_notch_nbi_idx_m = PATH1_NOTCH_R_NBI_IDX_A2_M;
                cr->path1_notch_nbi_frac_idx = PATH1_NOTCH_R_NBI_FRAC_IDX_A2;
                cr->path1_notch_nbi_frac_idx_m = PATH1_NOTCH_R_NBI_FRAC_IDX_A2_M;
                cr->path1_notch_nbi_en = PATH1_NOTCH_R_NBI_NOTCH_EN_A2;
                cr->path1_notch_nbi_en_m = PATH1_NOTCH_R_NBI_NOTCH_EN_A2_M;
                cr->path0_notch2_nbi_idx = PATH0_NOTCH2_NOTCH_R_NBI_IDX_A2;
                cr->path0_notch2_nbi_idx_m = PATH0_NOTCH2_NOTCH_R_NBI_IDX_A2_M;
                cr->path0_notch2_nbi_frac_idx = PATH0_NOTCH2_NOTCH_R_NBI_FRAC_IDX_A2;
                cr->path0_notch2_nbi_frac_idx_m = PATH0_NOTCH2_NOTCH_R_NBI_FRAC_IDX_A2_M;
                cr->path0_notch2_nbi_en = PATH0_NOTCH2_NOTCH_R_NBI_NOTCH_EN_A2;
                cr->path0_notch2_nbi_en_m = PATH0_NOTCH2_NOTCH_R_NBI_NOTCH_EN_A2_M;
                cr->path1_notch2_nbi_idx = PATH1_NOTCH2_NOTCH_R_NBI_IDX_A2;
                cr->path1_notch2_nbi_idx_m = PATH1_NOTCH2_NOTCH_R_NBI_IDX_A2_M;
                cr->path1_notch2_nbi_frac_idx = PATH1_NOTCH2_NOTCH_R_NBI_FRAC_IDX_A2;
                cr->path1_notch2_nbi_frac_idx_m = PATH1_NOTCH2_NOTCH_R_NBI_FRAC_IDX_A2_M;
                cr->path1_notch2_nbi_en = PATH1_NOTCH2_NOTCH_R_NBI_NOTCH_EN_A2;
                cr->path1_notch2_nbi_en_m = PATH1_NOTCH2_NOTCH_R_NBI_NOTCH_EN_A2_M;
		break;
	#endif
	#ifdef HALBB_COMPILE_CLIENT_SERIES
	case BB_CLIENT:
		cr->seg0_set1_csi_tone_idx = SEG0_SET1_CSI_WGT_TONE_IDX_C;
                cr->seg0_set1_csi_tone_idx_m = SEG0_SET1_CSI_WGT_TONE_IDX_C_M;
                cr->seg0_set1_csi_en = SEG0_SET1_CSI_WGT_EN_C;
                cr->seg0_set1_csi_en_m = SEG0_SET1_CSI_WGT_EN_C_M;
                cr->path0_notch_nbi_idx = PATH0_R_NBI_IDX_C;
                cr->path0_notch_nbi_idx_m = PATH0_R_NBI_IDX_C_M;
                cr->path0_notch_nbi_frac_idx = PATH0_R_NBI_FRAC_IDX_C;
                cr->path0_notch_nbi_frac_idx_m = PATH0_R_NBI_FRAC_IDX_C_M;
                cr->path0_notch_nbi_en = PATH0_R_NBI_NOTCH_EN_C;
                cr->path0_notch_nbi_en_m = PATH0_R_NBI_NOTCH_EN_C_M;
                cr->path1_notch_nbi_idx = PATH1_R_NBI_IDX_C;
                cr->path1_notch_nbi_idx_m = PATH1_R_NBI_IDX_C_M;
                cr->path1_notch_nbi_frac_idx = PATH1_R_NBI_FRAC_IDX_C;
                cr->path1_notch_nbi_frac_idx_m = PATH1_R_NBI_FRAC_IDX_C_M;
                cr->path1_notch_nbi_en = PATH1_R_NBI_NOTCH_EN_C;
                cr->path1_notch_nbi_en_m = PATH1_R_NBI_NOTCH_EN_C_M;
		break;
	#endif

	default:
		BB_WARNING("[%s] BBCR Hook FAIL!\n", __func__);
		if (bb->bb_dbg_i.cr_fake_init_hook_en) {
			BB_TRACE("[%s] BBCR fake init\n", __func__);
			halbb_cr_hook_fake_init(bb, (u32 *)cr, (sizeof(struct bb_spur_cr_info) >> 2));
		}
		break;
	}

	if (bb->bb_dbg_i.cr_init_hook_recorder_en) {
		BB_TRACE("[%s] BBCR Hook dump\n", __func__);
		halbb_cr_hook_init_dump(bb, (u32 *)cr, (sizeof(struct bb_spur_cr_info) >> 2));
	}
}
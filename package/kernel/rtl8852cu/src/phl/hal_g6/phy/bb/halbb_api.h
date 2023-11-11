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
#ifndef _HALBB_API_H_
#define _HALBB_API_H_

#include "halbb_ic_hw_info.h"
#include "halbb_dbcc_ex.h"

/*@--------------------------[Define] ---------------------------------------*/
#define IGI_2_RSSI(igi)		(igi - 10)

#define FUNC_ENABLE 1
#define FUNC_DISABLE 2
/*@--------------------------[Enum]------------------------------------------*/
enum bb_rfe_src_sel {
	PAPE_RFM		= 0,
	GNT_BT_INV		= 1,
	LNA0N			= 2,
	LNAON_RFM		= 3,
	TRSW_RFM		= 4,
	TRSW_RFM_B		= 5,
	GNT_BT			= 6,
	ZERO			= 7,
	ANTSEL_0		= 8,
	ANTSEL_1		= 9,
	ANTSEL_2		= 0xa,
	ANTSEL_3		= 0xb,
	ANTSEL_4		= 0xc,
	ANTSEL_5		= 0xd,
	ANTSEL_6		= 0xe,
	ANTSEL_7		= 0xf
};

enum bb_mlo {
	MLO_OFF_BB0,
	MLO_OFF_BB1,
	MLO_ON
};
/*@--------------------------[Structure]-------------------------------------*/
struct bb_api_info {
	u32 rxiqc_reg1; /*N-mode: for pathA REG0xc14*/
	u32 rxiqc_reg2; /*N-mode: for pathB REG0xc1c*/
	u8 tx_queue_bitmap; /*REG0x520[23:16]*/
	u8 ccktx_path;
	u8 pri_ch_idx;
	u8 central_ch;
	u8 bw;
	enum band_type band;
};
/*@--------------------------[Prptotype]-------------------------------------*/
struct bb_info;
u8 halbb_ch_2_band(struct bb_info *bb, u8 fc_ch);
u16 halbb_get_csi_buf_idx(struct bb_info *bb, u8 buf_idx, u8 txsc_idx);

void halbb_reset_bb_phy(struct bb_info *bb, enum phl_phy_idx phy_idx);
void halbb_gpio_setting_init(struct bb_info *bb);
void halbb_pre_agc_en(struct bb_info *bb, bool enable);
void halbb_set_gain_error(struct bb_info *bb, u8 central_ch, enum band_type band, enum rf_path path);
u8 halbb_stop_ic_trx(struct bb_info *bb, u8 set_type);
void halbb_ic_api_dbg(struct bb_info *bb, char input[][16], u32 *_used,
			char *output, u32 *_out_len);
u16 halbb_fc_mapping(struct bb_info *bb, enum band_type band, u8 central_ch);
u8 halbb_get_prim_sb (struct bb_info *bb, u8 central_ch, u8 pri_ch, enum channel_width bw);
void halbb_ic_hw_setting_non_io(struct bb_info *bb);
void halbb_ic_hw_setting_low_io(struct bb_info *bb);
void halbb_ic_hw_setting_dbcc(struct bb_info *bb);
void halbb_ic_hw_setting(struct bb_info *bb);
void halbb_ic_hw_setting_dbg(struct bb_info *bb, char input[][16], 
			     u32 *_used, char *output, u32 *_out_len);
#endif

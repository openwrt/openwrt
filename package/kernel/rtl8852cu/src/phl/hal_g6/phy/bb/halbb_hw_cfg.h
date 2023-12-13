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
#ifndef __HALBB_HW_CFG_H__
#define __HALBB_HW_CFG_H__

/*@--------------------------[Define] ---------------------------------------*/

#define BB_RXSC_NUM_40		9 /*SC:0,1~8*/
#define BB_RXSC_NUM_80		13 /*SC:0,1~8,9~12*/
#define BB_RXSC_NUM_160		15 /*SC:0,1~8,9~12,13~14*/
#define BB_RXSC_START_IDX_FULL	0
#define BB_RXSC_START_IDX_20	1
#define BB_RXSC_START_IDX_20_1	5
#define BB_RXSC_START_IDX_40	9
#define BB_RXSC_START_IDX_80	13

#define BB_BAND_NUM_MAX		12 /*2G:1, 5G:3, 6G:8*/
#define BB_HIDE_EFUSE_SIZE	55

//#define BB_GAIN_BAND_NUM	4
/*@--------------------------[Enum]------------------------------------------*/
enum bb_band_t {
	BB_BAND_2G	= 0,
	BB_BAND_5G_L	= 1,
	BB_BAND_5G_M	= 2,
	BB_BAND_5G_H	= 3,
	BB_BAND_6G_L	= 4,
	BB_BAND_6G_M	= 5,
	BB_BAND_6G_H	= 6,
	BB_BAND_6G_UH	= 7,
	BB_GAIN_BAND_NUM	= 8
};
/*@--------------------------[Structure]-------------------------------------*/

/*@--------------------------[Prptotype]-------------------------------------*/
struct bb_info;
void halbb_cfg_bb_rpl_ofst(struct bb_info *bb, enum bb_band_t band, u8 path, u32 addr, u32 data);
bool halbb_init_cr_default(struct bb_info *bb, bool is_form_folder, u32 folder_len,
			   u32 *folder_array, enum phl_phy_idx phy_idx);
bool halbb_init_gain_table(struct bb_info *bb, bool is_form_folder, u32 folder_len,
				 u32 *folder_array, enum phl_phy_idx phy_idx);
void halbb_rx_gain_table_dbg(struct bb_info *bb, char input[][16], 
			     u32 *_used, char *output, u32 *_out_len);
void halbb_rx_op1db_table_dbg(struct bb_info *bb, char input[][16], 
			      u32 *_used, char *output, u32 *_out_len);
#endif
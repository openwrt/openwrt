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
#ifndef __HALBB_RUA_TBL_H__
#define __HALBB_RUA_TBL_H__

#ifdef HALBB_RUA_SUPPORT
/*@--------------------------[Define] ---------------------------------------*/
/*[IO Reg]*/


/*@--------------------------[Enum]------------------------------------------*/

enum rua_tbl_sel {
	DL_RU_GP_TBL = 0x0,
	UL_RU_GP_TBL = 0x1,
	RU_STA_INFO = 0x2,
	DL_RU_FIX_TBL = 0x3,
	UL_RU_FIX_TBL = 0x4,
	BA_INFO_TBL = 0x5
};

enum rua_cfg_sel {
	SW_GRP_HDL = 0x0,
	DL_MACID_CFG = 0x1,
	UL_MACID_CFG = 0x2,
	CSI_INFO_CFG = 0x3,
	CQI_CFG = 0x4,
	BB_INFO_CFG = 0x5,
	PWR_TBL_init = 0x6
};


/*@--------------------------[Structure]-------------------------------------*/

/*@--------------------------[Prptotype]-------------------------------------*/
struct bb_info;
void halbb_rua_tbl_init(struct bb_info *bb);
void halbb_rua_tbl_dbg(struct bb_info *bb, char input[][16], u32 *_used,
		char *output, u32 *_out_len);
void halbb_trxpath_info_upd(struct bb_info *bb, enum rf_path tx_path,
		enum rf_path rx_path);
#endif /* HALBB_RUA_SUPPORT */

#endif

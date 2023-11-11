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
#ifndef __HALBB_DIG_EX_H__
#define __HALBB_DIG_EX_H__

/*@--------------------------[Define] ---------------------------------------*/
#define DIG_PAUSE_INFO_SIZE	2
/*@--------------------------[Enum]------------------------------------------*/
enum dig_op_mode {
	DIG_ORIGIN	= 0,
#ifdef HALBB_DIG_TDMA_SUPPORT
	DIG_TDMA	= 1,
	DIG_TDMA_ADV	= 2,
#endif
	DIG_SIMPLE	= 3,
	DIG_NONE
};

enum dig_pause_case {
	PAUSE_OFDM = 0,
	PAUSE_OFDM_CCK = 1
};
/*@--------------------------[Structure]-------------------------------------*/
struct bb_info;

/*@--------------------------[Prptotype]-------------------------------------*/
void halbb_dig_mode_update(struct bb_info *bb, enum dig_op_mode mode, enum phl_phy_idx phy_idx);
void halbb_dig_simple(struct bb_info *bb, u8 rssi_ofst);
#endif

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
#ifndef __HALBB_SNIF_H__
#define __HALBB_SNIF_H__


/*@--------------------------[Define] ---------------------------------------*/

/*@--------------------------[Enum]------------------------------------------*/

/*@--------------------------[Structure]-------------------------------------*/

struct bb_info;
/*@--------------------------[Prptotype]-------------------------------------*/
bool halbb_sniffer_phy_sts(struct bb_info *bb, struct physts_result *rpt,
			   u32 physts_bitmap, struct physts_rxd *desc);
void halbb_sniffer_phy_sts_init(struct bb_info *bb);
void halbb_snif_dbg(struct bb_info *bb, char input[][16], u32 *_used,
		    char *output, u32 *_out_len);
#endif
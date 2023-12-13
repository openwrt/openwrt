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
#ifndef __HALBB_DBCC_H__
#define __HALBB_DBCC_H__
#ifdef HALBB_DBCC_SUPPORT
/*@--------------------------[Define] ---------------------------------------*/
	#define	HALBB_GET_PHY_PTR(bb, bb_out, phy_idx)\
	do {								\
		if ((bb->bb_phy_hooker) && (phy_idx != bb->bb_phy_idx) && (bb->phl_com->dev_cap.dbcc_sup))	\
				bb_out = bb->bb_phy_hooker;		\
		else							\
				bb_out = bb;				\
	} while (0)

/*@--------------------------[Enum]------------------------------------------*/

/*@--------------------------[Structure]-------------------------------------*/
struct bb_dbcc_info {
	bool tmp_val;
};

/*@--------------------------[Prptotype]-------------------------------------*/
struct bb_info;
void halbb_cfo_trk_joint_phy_dec(struct bb_info *bb);
enum bb_path halbb_get_cur_phy_valid_path(struct bb_info *bb);
void halbb_dbcc_early_init(struct bb_info *bb);
void halbb_dbcc_dbg(struct bb_info *bb, char input[][16], u32 *_used,
		       char *output, u32 *_out_len);
struct bb_info *halbb_get_curr_bb_pointer(struct bb_info *bb,
					  enum phl_phy_idx phy_idx);
u32 halbb_buffer_init_phy1(struct bb_info *bb_phy_0);
#endif
#endif
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
#ifndef __HALBB_FWOFLD_H__
#define __HALBB_FWOFLD_H__

/*@--------------------------[Define] ---------------------------------------*/
#define RF18REGMASK 0x30fff  /* BW: bit10,11, Ch: bit17,16,9~0 */
/*@--------------------------[Enum]------------------------------------------*/
enum fw_ofld_type {
	FW_OFLD_PHY_0_CR_INIT	= 0,
	FW_OFLD_DM_INIT		= 1,
	FW_OFLD_BB_API		= 2,
	FW_OFLD_PHY_1_CR_INIT	= 8,
	FW_OFLD_PHY_1_DM_INIT	= 9,
	FW_OFLD_DBCC_API	= 10
};
/*@--------------------------[Structure]-------------------------------------*/

struct bb_info;

struct fw_cmd_info {
	char name[16];
	u16 id;
};

static const struct fw_cmd_info halbb_fw_ofld_info_i[] = {
	{"phy0_cr_init",FW_OFLD_PHY_0_CR_INIT}, /*@do not move this element to other position*/
	{"dm_init", FW_OFLD_DM_INIT},
	{"bb_api",	FW_OFLD_BB_API}, /*@do not move this element to other position*/
	{"phy1_cr_init", FW_OFLD_PHY_1_CR_INIT},
	{"phy1_dm_init", FW_OFLD_PHY_1_DM_INIT},
	{"dbcc_cfg",	FW_OFLD_DBCC_API}
};
/*@--------------------------[Prptotype]-------------------------------------*/
#ifdef HALBB_FW_OFLD_SUPPORT
void halbb_fwofld_bitmap_en(struct bb_info *bb, bool en, enum fw_ofld_type app);
void halbb_fwofld_bitmap_init(struct bb_info *bb);
bool halbb_fw_set_reg(struct bb_info *bb, u32 addr, u32 mask, u32 val, u8 lc);
bool halbb_fw_set_reg_cmn(struct bb_info *bb, u32 addr, u32 mask, u32 val,
			  enum phl_phy_idx phy_idx, u8 lc);
bool halbb_fwcfg_bb_phy_8852a_2(struct bb_info *bb, u32 addr, u32 data,
			    enum phl_phy_idx phy_idx);
bool halbb_fwofld_bw_ch_8852a_2(struct bb_info *bb, u8 pri_ch, u8 central_ch,
			    enum channel_width bw, enum phl_phy_idx phy_idx);
void halbb_fwofld_set_pmac_tx_8852a_2(struct bb_info *bb,
			     struct halbb_pmac_info *tx_info,
			     enum phl_phy_idx phy_idx);
void halbb_fw_ofld_dbg(struct bb_info *bb, char input[][16], u32 *_used,
		       char *output, u32 *_out_len);
#endif
#endif

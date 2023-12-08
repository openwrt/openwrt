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

void halbb_mp_bt_cfg(struct bb_info *bb, bool bt_connect)
{
	struct bb_rpt_cr_info *cr = &bb->bb_rpt_i.bb_rpt_cr_i;
	u8 lna_val = 0, lna_ori = 0, lna_gain_ofst = 0;

	BB_DBG(bb, DBG_PHY_CONFIG, "<====== %s ======>\n", __func__);

	if (bb->ic_type == BB_RTL8852A) {
		BB_DBG(bb, DBG_PHY_CONFIG, "IC not support !!\n");
		return;
	}

	if (bb->ic_type == BB_RTL8852C)
		lna_gain_ofst = 12;
	else if (bb->ic_sub_type == BB_IC_SUB_TYPE_8852B_8852BP)
		lna_gain_ofst = 8;

	lna_ori = (u8)halbb_get_reg(bb, cr->path1_g_lna6, cr->path1_g_lna6_m);

	if (bt_connect) {
		lna_val = (lna_ori <= lna_gain_ofst) ? 0 : lna_ori - lna_gain_ofst;
		halbb_set_reg(bb, cr->path1_g_lna6, cr->path1_g_lna6_m, lna_val);
	} else {
		lna_val = ((lna_ori + lna_gain_ofst) >= 0xff) ? 0xff : lna_ori + lna_gain_ofst;
		halbb_set_reg(bb, cr->path1_g_lna6, cr->path1_g_lna6_m, lna_val);
	}

	BB_DBG(bb, DBG_PHY_CONFIG, "BT connect:%d, lna_val:0x%x\n", bt_connect, lna_val);
}

u16 halbb_mp_get_tx_ok(struct bb_info *bb, u32 rate_index,
			enum phl_phy_idx phy_idx)
{
	u32 tx_ok;

	struct bb_rpt_cr_info *cr = &bb->bb_rpt_i.bb_rpt_cr_i;

	if (halbb_is_cck_rate(bb, (u16)rate_index))
		tx_ok = halbb_get_reg(bb, cr->cnt_ccktxon, cr->cnt_ccktxon_m);
	else
		tx_ok = halbb_get_reg_cmn(bb, cr->cnt_ofdmtxon, cr->cnt_ofdmtxon_m, phy_idx);
	return (u16)tx_ok;
}

u32 halbb_mp_get_rx_crc_ok(struct bb_info *bb, enum phl_phy_idx phy_idx)
{
	u32 cck_ok = 0, ofdm_ok = 0, ht_ok = 0, vht_ok = 0, he_ok = 0;
	u32 crc_ok;

	struct bb_rpt_cr_info *cr = &bb->bb_rpt_i.bb_rpt_cr_i;

	if (phy_idx == HW_PHY_0)
		cck_ok = halbb_get_reg(bb, cr->cnt_cck_crc32ok_p0, cr->cnt_cck_crc32ok_p0_m);
	else
		cck_ok = halbb_get_reg(bb, cr->cnt_cck_crc32ok_p1, cr->cnt_cck_crc32ok_p1_m);

	ofdm_ok = halbb_get_reg_cmn(bb, cr->cnt_l_crc_ok, cr->cnt_l_crc_ok_m, phy_idx);
	ht_ok = halbb_get_reg_cmn(bb, cr->cnt_ht_crc_ok, cr->cnt_ht_crc_ok_m, phy_idx);
	vht_ok = halbb_get_reg_cmn(bb, cr->cnt_vht_crc_ok, cr->cnt_vht_crc_ok_m, phy_idx);
	he_ok = halbb_get_reg_cmn(bb, cr->cnt_he_crc_ok, cr->cnt_he_crc_ok_m, phy_idx);

	crc_ok = cck_ok + ofdm_ok + ht_ok + vht_ok + he_ok;

	// === [Reset cnt] === //
	if (crc_ok >= 65535) {
		halbb_mp_reset_cnt(bb);
		crc_ok = 0;
	}

	return crc_ok;
}

u32 halbb_mp_get_rx_crc_err(struct bb_info *bb, enum phl_phy_idx phy_idx)
{
	u32 cck_err = 0, ofdm_err = 0, ht_err = 0, vht_err = 0, he_err = 0;
	u32 crc_err;

	struct bb_rpt_cr_info *cr = &bb->bb_rpt_i.bb_rpt_cr_i;

	if (phy_idx == HW_PHY_0)
		cck_err = halbb_get_reg(bb, cr->cnt_cck_crc32fail_p0, cr->cnt_cck_crc32fail_p0_m);
	else
		cck_err = halbb_get_reg(bb, cr->cnt_cck_crc32fail_p1, cr->cnt_cck_crc32fail_p1_m);

	ofdm_err = halbb_get_reg_cmn(bb, cr->cnt_l_crc_err, cr->cnt_l_crc_err_m, phy_idx);
	ht_err = halbb_get_reg_cmn(bb, cr->cnt_ht_crc_err, cr->cnt_ht_crc_err_m, phy_idx);
	vht_err = halbb_get_reg_cmn(bb, cr->cnt_vht_crc_err, cr->cnt_vht_crc_err_m, phy_idx);
	he_err = halbb_get_reg_cmn(bb, cr->cnt_he_crc_err, cr->cnt_he_crc_err_m, phy_idx);

	crc_err = cck_err + ofdm_err + ht_err + vht_err + he_err;

	// === [Reset cnt] === //
	if (crc_err == 65535) {
		halbb_mp_reset_cnt(bb);
		crc_err = 0;
	}

	return crc_err;
}

void halbb_mp_cnt_reset(struct bb_info *bb)
{
	struct bb_rpt_cr_info *cr = &bb->bb_rpt_i.bb_rpt_cr_i;

	halbb_set_reg_cmn(bb, cr->rst_all_cnt, cr->rst_all_cnt_m, 1, bb->bb_phy_idx);
	halbb_set_reg_cmn(bb, cr->rst_all_cnt, cr->rst_all_cnt_m, 0, bb->bb_phy_idx);
}

void halbb_mp_reset_cnt(struct bb_info *bb)
{
	struct bb_rpt_cr_info *cr = &bb->bb_rpt_i.bb_rpt_cr_i;

	// PHY0 cnt reset
	halbb_set_reg_cmn(bb, cr->rst_all_cnt, cr->rst_all_cnt_m, 1, HW_PHY_0);
	halbb_set_reg_cmn(bb, cr->rst_all_cnt, cr->rst_all_cnt_m, 0, HW_PHY_0);
	// PHY1 cnt reset
	halbb_set_reg_cmn(bb, cr->rst_all_cnt, cr->rst_all_cnt_m, 1, HW_PHY_1);
	halbb_set_reg_cmn(bb, cr->rst_all_cnt, cr->rst_all_cnt_m, 0, HW_PHY_1);
}

void halbb_mp_psts_setting(struct bb_info *bb, u32 ie_bitmap_setting)
{
	struct bb_physts_info	*physts = &bb->bb_physts_i;
	struct bb_rpt_cr_info *cr = &bb->bb_rpt_i.bb_rpt_cr_i;

	BB_DBG(bb, DBG_PHY_CONFIG, "<====== %s ======>\n", __func__);

	if (ie_bitmap_setting & BIT(IE00_CMN_CCK)) {
		halbb_set_reg(bb, cr->phy_sts_bitmap_he_mu, cr->phy_sts_bitmap_he_mu_m, BIT(IE00_CMN_CCK));
		halbb_set_reg(bb, cr->phy_sts_bitmap_vht_mu, cr->phy_sts_bitmap_vht_mu_m, BIT(IE00_CMN_CCK));
		halbb_set_reg(bb, cr->phy_sts_bitmap_cck, cr->phy_sts_bitmap_cck_m, BIT(IE00_CMN_CCK));
		halbb_set_reg(bb, cr->phy_sts_bitmap_legacy, cr->phy_sts_bitmap_legacy_m, BIT(IE00_CMN_CCK));
		halbb_set_reg(bb, cr->phy_sts_bitmap_ht, cr->phy_sts_bitmap_ht_m, BIT(IE00_CMN_CCK));
		halbb_set_reg(bb, cr->phy_sts_bitmap_vht, cr->phy_sts_bitmap_vht_m, BIT(IE00_CMN_CCK));
		halbb_set_reg(bb, cr->phy_sts_bitmap_he, cr->phy_sts_bitmap_he_m, BIT(IE00_CMN_CCK));
	}
	if (ie_bitmap_setting & BIT(IE01_CMN_OFDM)) {
		halbb_set_reg(bb, cr->phy_sts_bitmap_he_mu, cr->phy_sts_bitmap_he_mu_m, BIT(IE01_CMN_OFDM));
		halbb_set_reg(bb, cr->phy_sts_bitmap_vht_mu, cr->phy_sts_bitmap_vht_mu_m, BIT(IE01_CMN_OFDM));
		halbb_set_reg(bb, cr->phy_sts_bitmap_cck, cr->phy_sts_bitmap_cck_m, BIT(IE01_CMN_OFDM));
		halbb_set_reg(bb, cr->phy_sts_bitmap_legacy, cr->phy_sts_bitmap_legacy_m, BIT(IE01_CMN_OFDM));
		halbb_set_reg(bb, cr->phy_sts_bitmap_ht, cr->phy_sts_bitmap_ht_m, BIT(IE01_CMN_OFDM));
		halbb_set_reg(bb, cr->phy_sts_bitmap_vht, cr->phy_sts_bitmap_vht_m, BIT(IE01_CMN_OFDM));
		halbb_set_reg(bb, cr->phy_sts_bitmap_he, cr->phy_sts_bitmap_he_m, BIT(IE01_CMN_OFDM));
	}

	BB_DBG(bb, DBG_PHY_CONFIG, "[MP] physts ie bitmap setting : 0x%08x\n", ie_bitmap_setting);


}

void
halbb_mp_get_psts_ie_bitmap(struct bb_info *bb, struct bb_mp_psts *bb_mp_physts)
{
	struct bb_physts_info	*physts = &bb->bb_physts_i;
	u32 *bitmap_ptr = &physts->physts_bitmap_recv;
	u32 *mp_bitmap_ptr = &bb_mp_physts->ie_bitmap;

	BB_DBG(bb, DBG_PHY_CONFIG, "<====== %s ======>\n", __func__);

	mp_bitmap_ptr = bitmap_ptr;

}


void
halbb_mp_get_psts_ie_00(struct bb_info *bb, struct bb_mp_psts *bb_mp_physts)
{
	struct bb_physts_info	*physts = &bb->bb_physts_i;
	struct bb_physts_rslt_0_info *psts_0 = &physts->bb_physts_rslt_0_i;
	struct mp_physts_rslt_0 *mp_psts_0 = &bb_mp_physts->mp_physts_rslt_0_i;

	BB_DBG(bb, DBG_PHY_CONFIG, "<====== %s ======>\n", __func__);

	mp_psts_0 = (struct mp_physts_rslt_0 *)psts_0;

}

void
halbb_mp_get_psts_ie_01(struct bb_info *bb, struct bb_mp_psts *bb_mp_physts)
{
	struct bb_physts_info	*physts = &bb->bb_physts_i;
	struct bb_physts_rslt_1_info *psts_1 = &physts->bb_physts_rslt_1_i;
	//struct bb_mp_psts *mp_physts = &bb->bb_mp_psts_i;
	struct mp_physts_rslt_1 *mp_psts_1 = &bb_mp_physts->mp_physts_rslt_1_i;

	BB_DBG(bb, DBG_PHY_CONFIG, "<====== %s ======>\n", __func__);

	mp_psts_1 = (struct mp_physts_rslt_1 *)psts_1;

}

void
halbb_mp_get_psts(struct bb_info *bb , struct bb_mp_psts *bb_mp_physts)
{
	struct bb_physts_info	*physts = &bb->bb_physts_i;
	struct bb_physts_rslt_hdr_info	*psts_h = &physts->bb_physts_rslt_hdr_i;


	BB_DBG(bb, DBG_PHY_CONFIG, "<====== %s ======>\n", __func__);

	// IE bitmap info //
	halbb_mp_get_psts_ie_bitmap(bb , bb_mp_physts);

	// detail info of per IE //
	if (bb_mp_physts->ie_bitmap & BIT(IE00_CMN_CCK)) {
		halbb_mp_get_psts_ie_00(bb , bb_mp_physts);
	}
	if (bb_mp_physts->ie_bitmap & BIT(IE01_CMN_OFDM)) {
		halbb_mp_get_psts_ie_01(bb , bb_mp_physts);
	}
}

void halbb_keeper_cond(struct bb_info *bb, bool keeper_en, u8 keeper_trig_cond,
		       u8 dbg_sel, enum phl_phy_idx phy_idx)
{
	struct bb_rpt_cr_info *cr = &bb->bb_rpt_i.bb_rpt_cr_i;

	halbb_set_reg_cmn(bb, cr->sts_keeper_en, cr->sts_keeper_en_m, keeper_en,
			  phy_idx); //0x0738[4]
	halbb_set_reg_cmn(bb, cr->sts_keeper_trig_cond,
			  cr->sts_keeper_trig_cond_m, keeper_trig_cond, phy_idx); //0x0738[7:6]
	halbb_set_reg_cmn(bb, cr->sts_dbg_sel, cr->sts_dbg_sel_m, dbg_sel,
			  phy_idx); //0x0738[30:28]
}

void halbb_dbg_port_sel(struct bb_info *bb, u16 dbg_port_sel, u8 dbg_port_ip_sel,
			bool dbg_port_ref_clk_en, bool dbg_port_en)
{
	struct bb_rpt_cr_info *cr = &bb->bb_rpt_i.bb_rpt_cr_i;

	halbb_set_reg(bb, cr->dbg_port_sel, 0xfff, dbg_port_sel); // 0x20f0[11:0]
	halbb_set_reg(bb, cr->dbg_port_ip_sel, cr->dbg_port_ip_sel_m,
		      dbg_port_ip_sel); // 0x20f0[23:16]
	halbb_set_reg(bb, cr->dbg_port_ref_clk_en, cr->dbg_port_ref_clk_en_m,
		      dbg_port_ref_clk_en); // 0x20f4[24]
	halbb_set_reg(bb, cr->dbg_port_en, cr->dbg_port_en_m, dbg_port_en); // 0x20f8[31]
	
}

u8 halbb_mp_get_rxevm(struct bb_info *bb, u8 user, u8 strm, bool is_seg_0)
{
#if 0
	// Note: Only supports 2SS ! //
	struct bb_rpt_cr_info *cr = &bb->bb_rpt_i.bb_rpt_cr_i;
	enum phl_phy_idx phy_idx = bb->bb_phy_idx;
	u8 rxevm;

	bool is_cck;
	u8 i = 0;
	u32 rxevm_tmp = 0;
	u32 mask_0[4] = {0xff, 0xff00, 0xff0000, 0xff000000};
	u8 addr_ofst[2] = {0, 20};
	u32 dbg_port = 0;
	u16 tmp = 0;
	u32 bitmap[7] = {cr->phy_sts_bitmap_he_mu, cr->phy_sts_bitmap_vht_mu,
			 cr->phy_sts_bitmap_trigbase, cr->phy_sts_bitmap_legacy,
			 cr->phy_sts_bitmap_ht, cr->phy_sts_bitmap_vht,
			 cr->phy_sts_bitmap_he};
	u32 bitmap_m[7] = {cr->phy_sts_bitmap_he_mu_m,
			   cr->phy_sts_bitmap_vht_mu_m,
			   cr->phy_sts_bitmap_trigbase_m,
			   cr->phy_sts_bitmap_legacy_m, cr->phy_sts_bitmap_ht_m,
			   cr->phy_sts_bitmap_vht_m, cr->phy_sts_bitmap_he_m};
	u32 bitmap_restore[7] = {0};

	//=== Backup Bitmap value ===//
	for (i = 0; i < 7; i++)
		bitmap_restore[i] = halbb_get_reg_cmn(bb, bitmap[i], bitmap_m[i], phy_idx);
	//=== Set Bitmap ===//
	for (i = 0; i < 7; i++)
		halbb_set_reg_cmn(bb, bitmap[i], bitmap_m[i], 0x300002, phy_idx);

	// Set keeper condition //
	halbb_keeper_cond(bb, true, 0x1, 0x2, phy_idx);

	// DBG port polling //
	if (halbb_bb_dbg_port_racing(bb, DBGPORT_PRI_3)) {
		tmp = (phy_idx == HW_PHY_0 ? 0x700 : 0x701);
		halbb_dbg_port_sel(bb, tmp, 0x1, 0x0, 0x1);
	} else {
		dbg_port = halbb_get_bb_dbg_port_idx(bb);
		BB_TRACE("[Set dbg_port fail!] Curr-DbgPort=0x%x\n", dbg_port);
		return bb->rxevm;
	}

	if (halbb_get_reg(bb, 0x40b0, BIT(4)) == 0) { // Only use [4], Total [31:0]
		BB_WARNING("No CRC_OK in Keeper!\n");
		//=== Restore Bitmap value ===//
		for (i = 0; i < 7; i++)
			halbb_set_reg_cmn(bb, bitmap[i], bitmap_m[i], bitmap_restore[i], phy_idx);
		return bb->rxevm;
	}

	halbb_set_reg_cmn(bb, cr->sts_keeper_read, cr->sts_keeper_read_m, 1, phy_idx);

	halbb_delay_us(bb, 2);

	if (halbb_get_reg(bb, cr->dbg32_d, BIT(5)) == 1) {
		bb->rxevm.rxevm_seg[phy_idx].rxevm_valid = true;
		// Determine CCK pkt
		halbb_set_reg_cmn(bb, cr->sts_keeper_addr, cr->sts_keeper_addr_m, 1, phy_idx);
		is_cck = (halbb_get_reg_cmn(bb, cr->sts_keeper_data, 0xffff00, phy_idx) == 11) ? true : false; // Only use [23:8], Total [31:0]

		// CCK
		if (is_cck) {
			halbb_set_reg_cmn(bb, cr->sts_keeper_addr, cr->sts_keeper_addr_m, 3, phy_idx);
			rxevm_tmp = halbb_get_reg_cmn(bb, cr->sts_keeper_data, 0xff00, phy_idx); // Only use [15:8], Total [31:0]
			halbb_set_reg_cmn(bb, cr->sts_keeper_read, cr->sts_keeper_read_m, 0, phy_idx);
			bb->rxevm.rxevm_seg[0].rxevm_user[0].rxevm_ss_0 = (u8)rxevm_tmp;
			return bb->rxevm;
		}

		switch (user) {
		case 0:
			halbb_set_reg_cmn(bb, cr->sts_keeper_addr, cr->sts_keeper_addr_m, 15 + addr_ofst[is_seg_0], phy_idx);
			rxevm = halbb_get_reg_cmn(bb, cr->sts_keeper_data, 0xffff00, phy_idx);
			break;
		case 1:
			
			break;
		case 2:
			
			break;
		case 3:
			
			break;
		default:
			break;
		}

		for (i = 0; i < 2; i++) {
			
			// Usr0
			halbb_set_reg_cmn(bb, cr->sts_keeper_addr, cr->sts_keeper_addr_m, 15 + addr_ofst[i], phy_idx);
			rxevm_tmp = halbb_get_reg_cmn(bb, cr->sts_keeper_data, 0xffff00, phy_idx); // Only use [23:8], Total [31:0]
			bb->rxevm.rxevm_seg[i].rxevm_user[0].rxevm_ss_0 = (rxevm_tmp & 0xff);
			bb->rxevm.rxevm_seg[i].rxevm_user[0].rxevm_ss_1 = ((rxevm_tmp & 0xff00) >> 8);
			// Usr1
			halbb_set_reg_cmn(bb, cr->sts_keeper_addr, cr->sts_keeper_addr_m, 19 + addr_ofst[i], phy_idx);
			bb->rxevm.rxevm_seg[i].rxevm_user[1].rxevm_ss_0 = (u8)halbb_get_reg_cmn(bb, cr->sts_keeper_data, 0xff000000, phy_idx); // Only use [31:24], Total [31:0]
			halbb_set_reg_cmn(bb, cr->sts_keeper_addr, cr->sts_keeper_addr_m, 18 + addr_ofst[i], phy_idx);
			bb->rxevm.rxevm_seg[i].rxevm_user[1].rxevm_ss_1 = (u8)halbb_get_reg_cmn(bb, cr->sts_keeper_data, 0xff, phy_idx); // Only use [7:0], Total [31:0]
			// Usr2
			halbb_set_reg_cmn(bb, cr->sts_keeper_addr, cr->sts_keeper_addr_m, 22 + addr_ofst[i], phy_idx);
			rxevm_tmp = halbb_get_reg_cmn(bb, cr->sts_keeper_data, 0xffff00, phy_idx); // Only use [23:8], Total [31:0]
			bb->rxevm.rxevm_seg[i].rxevm_user[2].rxevm_ss_0 = (rxevm_tmp & 0xff);
			bb->rxevm.rxevm_seg[i].rxevm_user[2].rxevm_ss_1 = ((rxevm_tmp & 0xff00) >> 8);
			// Usr3
			halbb_set_reg_cmn(bb, cr->sts_keeper_addr, cr->sts_keeper_addr_m, 26 + addr_ofst[i], phy_idx);
			bb->rxevm.rxevm_seg[i].rxevm_user[3].rxevm_ss_0 = (u8)halbb_get_reg_cmn(bb, cr->sts_keeper_data, 0xff000000, phy_idx); // Only use [31:24], Total [31:0]
			halbb_set_reg_cmn(bb, cr->sts_keeper_addr, cr->sts_keeper_addr_m, 29 + addr_ofst[i], phy_idx);
			bb->rxevm.rxevm_seg[i].rxevm_user[3].rxevm_ss_1 = (u8)halbb_get_reg_cmn(bb, cr->sts_keeper_data, 0xff, phy_idx); // Only use [7:0], Total [31:0]
		}
	} else {
		bb->rxevm.rxevm_seg[phy_idx].rxevm_valid = false;
	}
	halbb_set_reg_cmn(bb, cr->sts_keeper_read, cr->sts_keeper_read_m, 0, phy_idx);

	halbb_release_bb_dbg_port(bb);

	//=== Restore Bitmap value ===//
	for (i = 0; i < 7; i++)
		halbb_set_reg_cmn(bb, bitmap[i], bitmap_m[i], bitmap_restore[i], phy_idx);

	return bb->rxevm;
#elif 0
	u8 rx_evm;
	u8 mode;
	u32 user_mask[4] = {0xff000000, 0xff0000, 0xff00, 0xff};

	struct bb_rpt_cr_info *cr = &bb->bb_rpt_i.bb_rpt_cr_i;

	BB_DBG(bb, DBG_PHY_CONFIG, "<====== %s ======>\n", __func__);
	/*==== Error handling ====*/
	if ((user >= 4) || (strm >= 4)) {
		BB_WARNING("Invalid User or Stream\n");
		return false;
	}
	/*=== [All tone] ===*/
	halbb_set_reg(bb, cr->rpt_tone_evm_idx, 0x3ff, 0); // Only use [9:0], Total [10:0]
	halbb_set_reg_cmn(bb, cr->rpt_tone_evm_idx, 0x3ff, 0, HW_PHY_1);
	/*=== [Switch MUX] ===*/
	halbb_set_reg(bb, cr->dbg_port_ref_clk_en, cr->dbg_port_ref_clk_en_m, 0);
	halbb_set_reg(bb, cr->dbg_port_en, cr->dbg_port_en_m, 1);
	halbb_set_reg(bb, cr->dbg_port_ip_sel, 0x1f, 2); // Only use [4:0], Total [7:0]
	halbb_set_reg(bb, cr->dbg_port_sel, 0xf0, 1); // Only use [7:4], Total [15:0]

	mode = user + 1 + rxevm_table * 8;

	halbb_set_reg(bb, cr->dbg_port_sel, 0xf, mode); // Only use [3:0], Total [15:0]

	rx_evm = (u8)halbb_get_reg(bb, cr->dbg32_d, user_mask[strm]);
	return rx_evm;
#else
	return 0;
#endif
}

struct rxevm_physts halbb_mp_get_rxevm_physts(struct bb_info *bb,
					      enum phl_phy_idx phy_idx)
{
	// Note: Only supports 2SS ! //
	struct bb_rpt_cr_info *cr = &bb->bb_rpt_i.bb_rpt_cr_i;

	bool is_cck;
	u8 i = 0;
	u32 rxevm_tmp = 0;
	u8 addr_ofst[2] = {0, 20};
	u32 dbg_port = 0;
	u16 tmp = 0;
	u32 bitmap[7] = {cr->phy_sts_bitmap_he_mu, cr->phy_sts_bitmap_vht_mu,
			 cr->phy_sts_bitmap_trigbase, cr->phy_sts_bitmap_legacy,
			 cr->phy_sts_bitmap_ht, cr->phy_sts_bitmap_vht,
			 cr->phy_sts_bitmap_he};
	u32 bitmap_m[7] = {cr->phy_sts_bitmap_he_mu_m,
			   cr->phy_sts_bitmap_vht_mu_m,
			   cr->phy_sts_bitmap_trigbase_m,
			   cr->phy_sts_bitmap_legacy_m, cr->phy_sts_bitmap_ht_m,
			   cr->phy_sts_bitmap_vht_m, cr->phy_sts_bitmap_he_m};
	u32 bitmap_restore[7] = {0};

	BB_DBG(bb, DBG_PHY_CONFIG, "[Rxevm] halbb_mp_get_rxevm_physts==========================>\n");

	//=== Backup Bitmap value ===//
	for (i = 0; i < 7; i++)
		bitmap_restore[i] = halbb_get_reg_cmn(bb, bitmap[i], bitmap_m[i], phy_idx);
	//=== Set Bitmap ===//
	for (i = 0; i < 7; i++)
		halbb_set_reg_cmn(bb, bitmap[i], bitmap_m[i], 0x300002, phy_idx);

	// Set keeper condition //
	halbb_keeper_cond(bb, true, 0x1, 0x2, phy_idx);

	// DBG port polling //
	if (halbb_bb_dbg_port_racing(bb, DBGPORT_PRI_3)) {
		tmp = (phy_idx == HW_PHY_0 ? 0x700 : 0x701);
		halbb_dbg_port_sel(bb, tmp, 0x1, 0x0, 0x1);
	} else {
		dbg_port = halbb_get_bb_dbg_port_idx(bb);
		BB_TRACE("[Set dbg_port fail!] Curr-DbgPort=0x%x\n", dbg_port);
		return bb->rxevm;
	}

	// Config user0
	halbb_set_reg_cmn(bb, cr->sts_user_sel, cr->sts_user_sel_m, 0, phy_idx);

	halbb_set_reg_cmn(bb, cr->sts_keeper_read, cr->sts_keeper_read_m, 1, phy_idx);

	halbb_delay_us(bb, 2);

	if (halbb_get_reg(bb, cr->dbg32_d, BIT(5)) == 1) {
		// Determine CCK pkt
		halbb_set_reg_cmn(bb, cr->sts_keeper_addr, cr->sts_keeper_addr_m, 1, phy_idx);
		is_cck = (halbb_get_reg_cmn(bb, cr->sts_keeper_data, 0xf, phy_idx) == 11) ? true : false;

		// CCK
		if (is_cck) {
			halbb_set_reg_cmn(bb, cr->sts_keeper_addr, cr->sts_keeper_addr_m, 5, phy_idx);
			rxevm_tmp = halbb_get_reg_cmn(bb, cr->sts_keeper_data, 0xff00, phy_idx); // Only use [15:8], Total [31:0]
			bb->rxevm.rxevm_seg[0].rxevm_user[0].rxevm_ss_0 = (u8)rxevm_tmp;
			bb->rxevm.rxevm_seg[0].rxevm_user[0].rxevm_ss_1 = 0;
			BB_DBG(bb, DBG_PHY_CONFIG, "[Rxevm][CCK][Phy-%d] rxevm_ss_0=0x%x\n", phy_idx, bb->rxevm.rxevm_seg[0].rxevm_user[0].rxevm_ss_0);
			BB_DBG(bb, DBG_PHY_CONFIG, "[Rxevm][CCK][Phy-%d] rxevm_ss_1=0x%x\n", phy_idx, bb->rxevm.rxevm_seg[0].rxevm_user[0].rxevm_ss_1);
		} else {
			if ((bb->ic_type == BB_RTL8852A) || (bb->ic_type == BB_RTL8852B) || (bb->ic_type == BB_RTL8851B)) {
				for (i = 0; i < 2; i++) {
					// Usr0
					halbb_set_reg_cmn(bb, cr->sts_keeper_addr, cr->sts_keeper_addr_m, 15 + addr_ofst[i], phy_idx);
					rxevm_tmp = halbb_get_reg_cmn(bb, cr->sts_keeper_data, 0xffff00, phy_idx); // Only use [23:8], Total [31:0]
					bb->rxevm.rxevm_seg[i].rxevm_user[0].rxevm_ss_0 = (rxevm_tmp & 0xff);
					bb->rxevm.rxevm_seg[i].rxevm_user[0].rxevm_ss_1 = ((rxevm_tmp & 0xff00) >> 8);

					// Usr1
					halbb_set_reg_cmn(bb, cr->sts_keeper_addr, cr->sts_keeper_addr_m, 19 + addr_ofst[i], phy_idx);
					bb->rxevm.rxevm_seg[i].rxevm_user[1].rxevm_ss_0 = (u8)halbb_get_reg_cmn(bb, cr->sts_keeper_data, 0xff000000, phy_idx); // Only use [31:24], Total [31:0]
					halbb_set_reg_cmn(bb, cr->sts_keeper_addr, cr->sts_keeper_addr_m, 18 + addr_ofst[i], phy_idx);
					bb->rxevm.rxevm_seg[i].rxevm_user[1].rxevm_ss_1 = (u8)halbb_get_reg_cmn(bb, cr->sts_keeper_data, 0xff, phy_idx); // Only use [7:0], Total [31:0]
					// Usr2
					halbb_set_reg_cmn(bb, cr->sts_keeper_addr, cr->sts_keeper_addr_m, 22 + addr_ofst[i], phy_idx);
					rxevm_tmp = halbb_get_reg_cmn(bb, cr->sts_keeper_data, 0xffff00, phy_idx); // Only use [23:8], Total [31:0]
					bb->rxevm.rxevm_seg[i].rxevm_user[2].rxevm_ss_0 = (rxevm_tmp & 0xff);
					bb->rxevm.rxevm_seg[i].rxevm_user[2].rxevm_ss_1 = ((rxevm_tmp & 0xff00) >> 8);
					// Usr3
					halbb_set_reg_cmn(bb, cr->sts_keeper_addr, cr->sts_keeper_addr_m, 26 + addr_ofst[i], phy_idx);
					bb->rxevm.rxevm_seg[i].rxevm_user[3].rxevm_ss_0 = (u8)halbb_get_reg_cmn(bb, cr->sts_keeper_data, 0xff000000, phy_idx); // Only use [31:24], Total [31:0]
					halbb_set_reg_cmn(bb, cr->sts_keeper_addr, cr->sts_keeper_addr_m, 29 + addr_ofst[i], phy_idx);
					bb->rxevm.rxevm_seg[i].rxevm_user[3].rxevm_ss_1 = (u8)halbb_get_reg_cmn(bb, cr->sts_keeper_data, 0xff, phy_idx); // Only use [7:0], Total [31:0]
				}
			} else {
				// Usr0
				halbb_set_reg_cmn(bb, cr->sts_keeper_addr, cr->sts_keeper_addr_m, 15, phy_idx);
				bb->rxevm.rxevm_seg[0].rxevm_user[0].rxevm_ss_0 = (u8)halbb_get_reg_cmn(bb, cr->sts_keeper_data, 0xff000000, phy_idx);
				halbb_set_reg_cmn(bb, cr->sts_keeper_addr, cr->sts_keeper_addr_m, 19, phy_idx);
				bb->rxevm.rxevm_seg[0].rxevm_user[0].rxevm_ss_1 = (u8)halbb_get_reg_cmn(bb, cr->sts_keeper_data, 0xff000000, phy_idx);
				BB_DBG(bb, DBG_PHY_CONFIG, "[Rxevm] [Phy-%d] rxevm_ss_0=0x%x\n", phy_idx, bb->rxevm.rxevm_seg[0].rxevm_user[0].rxevm_ss_0);
				BB_DBG(bb, DBG_PHY_CONFIG, "[Rxevm] [Phy-%d] rxevm_ss_1=0x%x\n", phy_idx, bb->rxevm.rxevm_seg[0].rxevm_user[0].rxevm_ss_1);
			}
		}
		 bb->bb_cmn_backup_i.last_rxevm_rpt = bb->rxevm;
	} else {
		BB_DBG(bb, DBG_PHY_CONFIG, "[Rxevm] No crc_ok\n");
		bb->rxevm = bb->bb_cmn_backup_i.last_rxevm_rpt;
	}
	halbb_set_reg_cmn(bb, cr->sts_keeper_read, cr->sts_keeper_read_m, 0, phy_idx);

	halbb_release_bb_dbg_port(bb);

	//=== Restore Bitmap value ===//
	//for (i = 0; i < 7; i++)
	//	halbb_set_reg_cmn(bb, bitmap[i], bitmap_m[i], bitmap_restore[i], phy_idx);

	return bb->rxevm;
}
/*
u16 halbb_mp_get_pwdb_diff(struct bb_info *bb, enum rf_path path)
{
	u16 pwdb_diff;
	u32 rpt_addr[4] = {0x4048, 0x4060, 0x4078, 0x4090};

	BB_DBG(bb, DBG_PHY_CONFIG, "<====== %s ======>\n", __func__);

	if (path > RF_PATH_D) {
		BB_WARNING("Invalid Path\n");
		return false;
	}

	pwdb_diff = (u16)halbb_get_reg(bb, rpt_addr[path], 0xff800);
	return pwdb_diff;

}
*/
u8 halbb_mp_get_rssi(struct bb_info *bb, enum rf_path path)
{
#if 0
	struct bb_rpt_cr_info *cr = &bb->bb_rpt_i.bb_rpt_cr_i;
	u32 rpt_mask_ofst[4] = {0xff, 0xff00, 0xff0000, 0xff000000};
	u32 dbg_port = 0;
	u8 rssi;

	// Set keeper condition //
	halbb_keeper_cond(bb, true, 0x1, 0x2, HW_PHY_0);

	// DBG port polling //
	if (halbb_bb_dbg_port_racing(bb, DBGPORT_PRI_3)) {
		halbb_dbg_port_sel(bb, 0x700, 0x1, 0x0, 0x1);
	} else {
		dbg_port = halbb_get_bb_dbg_port_idx(bb);
		BB_TRACE("[Set dbg_port fail!] Curr-DbgPort=0x%x\n", dbg_port);
		return 0xff;
	}

	// Set to Read State //
	halbb_set_reg_cmn(bb, cr->sts_keeper_read, cr->sts_keeper_read_m, 1, HW_PHY_0);

	halbb_delay_us(bb, 2);

	// Polling machanism that determines if read state is successfully set //
	if (halbb_get_reg(bb, cr->dbg32_d, BIT(5)) == 1) {
		halbb_set_reg_cmn(bb, cr->sts_keeper_addr, cr->sts_keeper_addr_m, 0, HW_PHY_0);
		rssi = (u8)halbb_get_reg_cmn(bb, cr->sts_keeper_data, rpt_mask_ofst[path], HW_PHY_0); // Only use [23:15], Total [31:0]
		bb->bb_cmn_backup_i.last_rpl = rssi;
	} else {
		rssi = bb->bb_cmn_backup_i.last_rpl;
	}

	halbb_set_reg_cmn(bb, cr->sts_keeper_read, cr->sts_keeper_read_m, 0, HW_PHY_0);

	halbb_release_bb_dbg_port(bb);

	return rssi;
#else
	u8 rssi;
	u32 dbg_port = 0;

	// RSSI_FD
	struct bb_rpt_cr_info *cr = &bb->bb_rpt_i.bb_rpt_cr_i;

	// Phy0 / 1
	halbb_set_reg_cmn(bb, cr->sts_keeper_en, cr->sts_keeper_en_m, 1, HW_PHY_0);
	halbb_set_reg_cmn(bb, cr->sts_keeper_trig_cond, cr->sts_keeper_trig_cond_m, 1, HW_PHY_0);
	halbb_set_reg_cmn(bb, cr->sts_dbg_sel, cr->sts_dbg_sel_m, 2, HW_PHY_0);

	if (halbb_bb_dbg_port_racing(bb, DBGPORT_PRI_3)) {
		halbb_set_reg(bb, cr->dbg_port_ref_clk_en, cr->dbg_port_ref_clk_en_m, 0);
		halbb_set_reg(bb, cr->dbg_port_en, cr->dbg_port_en_m, 1);
		halbb_set_reg(bb, cr->dbg_port_ip_sel, cr->dbg_port_ip_sel_m, 1);
		halbb_set_reg(bb, cr->dbg_port_sel, 0xf00, 7); // Only use [11:8], Total [15:0]

		halbb_set_reg(bb, cr->dbg_port_sel, 0xff, 0);	// Only use [7:0], Total [15:0]
	} else {
		dbg_port = halbb_get_bb_dbg_port_idx(bb);
		BB_TRACE("[Set dbg_port fail!] Curr-DbgPort=0x%x\n", dbg_port);
		return bb->bb_cmn_backup_i.last_rpl;
	}

	halbb_set_reg_cmn(bb, cr->sts_keeper_read, cr->sts_keeper_read_m, 1, HW_PHY_0);

	halbb_delay_us(bb, 2);

	if (halbb_get_reg(bb, cr->dbg32_d, BIT(5)) == 1) {
		halbb_set_reg_cmn(bb, cr->sts_keeper_addr, cr->sts_keeper_addr_m, 3, HW_PHY_0);
		rssi = (u8)halbb_get_reg_cmn(bb, cr->sts_keeper_data, 0xff00, HW_PHY_0); // Only use [23:8], Total [31:0]
		bb->bb_cmn_backup_i.last_rpl = rssi;
	} else {
		rssi = bb->bb_cmn_backup_i.last_rpl;
	}

	halbb_set_reg_cmn(bb, cr->sts_keeper_read, cr->sts_keeper_read_m, 0, HW_PHY_0);

	halbb_release_bb_dbg_port(bb);

	return rssi;
#endif
}

s32 halbb_rssi_cal(struct bb_info *bb, u8 rssi_0, u8 rssi_1, bool is_higher_rssi_path, enum phl_phy_idx phy_idx)
{
	u8 rssi_diff = rssi_0 - rssi_1;
	s32 rssi_cal;
	u64 alpha_tmp;

	BB_DBG(bb, DBG_PHY_CONFIG, "<====== %s ======>\n", __func__);

	if (rssi_diff == 0)
		return 0;

	rssi_diff = rssi_diff % 2 ? (rssi_diff + 1) >> 1 : rssi_diff >> 1;

	alpha_tmp = halbb_db_2_linear((u32) rssi_diff);

	alpha_tmp = (alpha_tmp >> 2) % 2 ? (alpha_tmp >> FRAC_BITS) + 1 : alpha_tmp >> FRAC_BITS;

	if (is_higher_rssi_path)
		rssi_cal = halbb_convert_to_db(alpha_tmp) -
			   halbb_convert_to_db(alpha_tmp + 1) + 3;
	else
		rssi_cal = 3 - halbb_convert_to_db(alpha_tmp + 1);

	return rssi_cal;
}

struct rssi_physts halbb_get_mp_rssi_physts(struct bb_info *bb, enum rf_path path, enum phl_phy_idx phy_idx)
{
	// RSSI_FD: This function is used for MP UI report, which is RPL value
	struct bb_rpt_cr_info *cr = &bb->bb_rpt_i.bb_rpt_cr_i;
	struct bb_physts_info *physts = &bb->bb_physts_i;
	struct bb_physts_rslt_hdr_info *psts_h = &physts->bb_physts_rslt_hdr_i;
	struct bb_efuse_info efuse = bb->bb_efuse_i;
	struct rssi_physts rssi_rpt;
	bool rssi_0_high = psts_h->rssi[0] > psts_h->rssi[1] ? true : false;
	s32 efuse_tmp = 0;
	u8 band = 0;
	u8 central_ch = bb->hal_com->band[phy_idx].cur_chandef.center_ch;
	u8 RPL = 0;
	u32 rssi = 0;
	u32 dbg_port = 0;
	s32 cal_tmp[2] = {0};
	u16 tmp = 0;

	halbb_mem_set(bb, &rssi_rpt,0,sizeof(rssi_rpt));

	BB_DBG(bb, DBG_PHY_CONFIG, "<====== %s ======>\n", __func__);

	// 2G Band: (0)
	// 5G Band: (1):Low, (2): Mid, (3):High
	if (central_ch >= 0 && central_ch <= 14)
		band = 0;
	else if (central_ch >= 36 && central_ch <= 64)
		band = 1;
	else if (central_ch >= 100 && central_ch <= 144)
		band = 2;
	else if (central_ch >= 149 && central_ch <= 177)
		band = 3;
	else
		band = 0;

	efuse_tmp = efuse.gain_offset[RF_PATH_A][band] - efuse.gain_offset[RF_PATH_B][band];

	// Phy0 / 1
	halbb_set_reg_cmn(bb, cr->sts_keeper_en, cr->sts_keeper_en_m, 1, phy_idx);
	halbb_set_reg_cmn(bb, cr->sts_keeper_trig_cond, cr->sts_keeper_trig_cond_m, 1, phy_idx);
	halbb_set_reg_cmn(bb, cr->sts_dbg_sel, cr->sts_dbg_sel_m, 2, phy_idx);

	if (halbb_bb_dbg_port_racing(bb, DBGPORT_PRI_3)) {
		tmp = (phy_idx == HW_PHY_0 ? 0x700 : 0x701);
		halbb_dbg_port_sel(bb, tmp, 0x1, 0x0, 0x1);
	} else {
		dbg_port = halbb_get_bb_dbg_port_idx(bb);
		BB_TRACE("[Set dbg_port fail!] Curr-DbgPort=0x%x\n", dbg_port);
		return bb->bb_cmn_backup_i.last_rssi_rpt;
	}

	halbb_set_reg_cmn(bb, cr->sts_keeper_read, cr->sts_keeper_read_m, 1, phy_idx);

	halbb_delay_us(bb, 2);

	#if 1
		if (halbb_get_reg(bb, cr->dbg32_d, BIT(5)) == 1) {
			halbb_set_reg_cmn(bb, cr->sts_keeper_addr, cr->sts_keeper_addr_m, 3, phy_idx);
			RPL = (u8)halbb_get_reg_cmn(bb, cr->sts_keeper_data, 0xff00, phy_idx); // Only use [23:8], Total [31:0]
			bb->bb_cmn_backup_i.last_rpl = RPL;
		} else {
			RPL = bb->bb_cmn_backup_i.last_rpl;
		}
	#else
		if (halbb_get_reg(bb, cr->dbg32_d, BIT(5)) != 1) {
			halbb_release_bb_dbg_port(bb);
			return bb->bb_cmn_backup_i.last_rssi_rpt;
		}
	#endif

	halbb_set_reg_cmn(bb, cr->sts_keeper_read, cr->sts_keeper_read_m, 0, phy_idx);

	halbb_release_bb_dbg_port(bb);

	#if 1
		if (path == RF_PATH_A) {
			rssi_rpt.rssi_seg[0].rssi[0] = (s32)(RPL - 220);
			rssi_rpt.rssi_seg[0].rssi[1] = 0;
		} else if (path == RF_PATH_B) {
			rssi_rpt.rssi_seg[0].rssi[0] = 0;
			rssi_rpt.rssi_seg[0].rssi[1] = (s32)(RPL - 220);
		} else {
			if (rssi_0_high) {
				if (psts_h->rssi[0] - psts_h->rssi[1] > 12) {
					rssi_rpt.rssi_seg[0].rssi[0] = (s32)(RPL - 220 + 6);
					rssi_rpt.rssi_seg[0].rssi[1] = (s32)(psts_h->rssi[1] - 220);
				} else {
					cal_tmp[0] = halbb_rssi_cal(bb, psts_h->rssi[0], psts_h->rssi[1], true, phy_idx);
					cal_tmp[1] = halbb_rssi_cal(bb, psts_h->rssi[0], psts_h->rssi[1], false, phy_idx);
					rssi_rpt.rssi_seg[0].rssi[0] = (s32)((RPL - 220) + (cal_tmp[0] << 1));
					rssi_rpt.rssi_seg[0].rssi[1] = (s32)((RPL - 220) + (efuse_tmp << 1) + (cal_tmp[1] << 1));
				}
			} else {
				if (psts_h->rssi[1] - psts_h->rssi[0] > 12) {
					rssi_rpt.rssi_seg[0].rssi[0] = (s32)(psts_h->rssi[0] - 220);
					rssi_rpt.rssi_seg[0].rssi[1] = (s32)(RPL - 220 + 6);
				} else {
					cal_tmp[0] = halbb_rssi_cal(bb, psts_h->rssi[1], psts_h->rssi[0], false, phy_idx);
					cal_tmp[1] = halbb_rssi_cal(bb, psts_h->rssi[1], psts_h->rssi[0], true, phy_idx);
					rssi_rpt.rssi_seg[0].rssi[0] = (s32)((RPL - 220) + (cal_tmp[0] << 1));
					rssi_rpt.rssi_seg[0].rssi[1] = (s32)((RPL - 220) + (efuse_tmp << 1) + (cal_tmp[1] << 1));
				}
			}
		}
	#else
		#if 0
		rssi_rpt.rssi_seg[0].rssi[0] = (s32)((rssi & 0xff) - 220);
		rssi_rpt.rssi_seg[0].rssi[1] = (s32)(((rssi & 0xff00) >> 8) - 220);
		rssi_rpt.rssi_seg[0].rssi[2] = (s32)(((rssi & 0xff0000) >> 16) - 220);
		rssi_rpt.rssi_seg[0].rssi[3] = (s32)(((rssi & 0xff000000) >> 24) - 220);
		#endif
		rssi = (u32)halbb_mp_get_rpl(bb, RF_PATH_A, phy_idx);
		rssi_rpt.rssi_seg[0].rssi[0] = (s32)((rssi & 0xff) - 220);
		rssi = (u32)halbb_mp_get_rpl(bb, RF_PATH_B, phy_idx);
		rssi_rpt.rssi_seg[0].rssi[1] = (s32)((rssi & 0xff) - 220);
		BB_DBG(bb, DBG_PHY_CONFIG, "[RSSI] [Phy-%d] RSSI[0]=0x%x\n", phy_idx, rssi_rpt.rssi_seg[0].rssi[0]);
		BB_DBG(bb, DBG_PHY_CONFIG, "[RSSI] [Phy-%d] RSSI[1]=0x%x\n", phy_idx, rssi_rpt.rssi_seg[0].rssi[1]);

		bb->bb_cmn_backup_i.last_rssi_rpt = rssi_rpt;
	#endif

	return rssi_rpt;
}

u16 halbb_mp_get_rpl(struct bb_info *bb, enum rf_path path, enum phl_phy_idx phy_idx)
{
	struct bb_rpt_cr_info *cr = &bb->bb_rpt_i.bb_rpt_cr_i;
	u16 rpl = 0;
	u32 rpl_m[2] = {0x1fff, 0x3ffe000};

	// Error hadling
	if (path > RF_PATH_D || path < RF_PATH_A) {
		BB_WARNING("Invalid Path!\n");
		return 0xffff;
	}

	if (bb->ic_type == BB_RTL8852C)
		rpl = (u16)halbb_get_reg_cmn(bb, cr->pw_dbm_rx0, *(rpl_m + path), phy_idx);
	else
		rpl = 0xffff;

	return rpl;
}

u32 halbb_mp_get_dc_lvl(struct bb_info *bb, enum rf_path path, bool i_ch, enum phl_phy_idx phy_idx)
{
	struct bb_rpt_cr_info *cr = &bb->bb_rpt_i.bb_rpt_cr_i;
	u16 path_tmp[2] = {0xb12, 0xb22};
	u32 tmp = 0;
	u32 dbg_port = 0;

	// Error hadling
	if (path > RF_PATH_D || path < RF_PATH_A) {
		BB_WARNING("Invalid Path!\n");
		return 0xffff;
	}

	// DBG port polling //
	if (halbb_bb_dbg_port_racing(bb, DBGPORT_PRI_3)) {
		halbb_dbg_port_sel(bb, *(path_tmp + path), 0x1, 0x0, 0x1);
	} else {
		dbg_port = halbb_get_bb_dbg_port_idx(bb);
		BB_TRACE("[Set dbg_port fail!] Curr-DbgPort=0x%x\n", dbg_port);
		return 0xffffffff;
	}

	// Release DBG port //
	halbb_release_bb_dbg_port(bb);

	if (i_ch)
		tmp = halbb_get_reg(bb, cr->dbg32_d, 0xfff); // I channel
	else
		tmp = halbb_get_reg(bb, cr->dbg32_d, 0xfff000); // Q channel

	return tmp;
}

u16 halbb_mp_get_pwdbm(struct bb_info *bb, enum rf_path path, enum phl_phy_idx phy_idx)
{
	struct bb_rpt_cr_info *cr = &bb->bb_rpt_i.bb_rpt_cr_i;
	u16 pwdbm;

	// Error hadling
	if (path > RF_PATH_D) {
		BB_WARNING("Invalid Path!\n");
		return 0xffff;
	}

	if (path == RF_PATH_A)
		pwdbm = (u16)halbb_get_reg(bb, cr->path0_rssi_at_agc_rdy,
					   cr->path0_rssi_at_agc_rdy_m);
	else if (path == RF_PATH_B)
		pwdbm = (u16)halbb_get_reg(bb, cr->path1_rssi_at_agc_rdy,
					   cr->path1_rssi_at_agc_rdy_m);
	else
		pwdbm = 0xffff;

	return pwdbm;
}

u16 halbb_mp_get_cfo(struct bb_info *bb, enum phl_phy_idx phy_idx)
{
	struct bb_rpt_cr_info *cr = &bb->bb_rpt_i.bb_rpt_cr_i;
	u32 dbg_port = 0;
	u16 cfo = 0;
	u16 tmp = 0;

	// Set keeper condition //
	halbb_keeper_cond(bb, true, 0x1, 0x2, HW_PHY_0);

	// DBG port polling //
	if (halbb_bb_dbg_port_racing(bb, DBGPORT_PRI_3)) {
		tmp = (phy_idx == HW_PHY_0 ? 0x700 : 0x701);
		halbb_dbg_port_sel(bb, tmp, 0x1, 0x0, 0x1);
	} else {
		dbg_port = halbb_get_bb_dbg_port_idx(bb);
		BB_TRACE("[Set dbg_port fail!] Curr-DbgPort=0x%x\n", dbg_port);
		return 0xffff;
	}

	// Set to Read State //
	halbb_set_reg_cmn(bb, cr->sts_keeper_read, cr->sts_keeper_read_m, 1, HW_PHY_0);

	halbb_delay_us(bb, 2);

	// Polling machanism that determines if read state is successfully set  //
	if (halbb_get_reg(bb, cr->dbg32_d, BIT(5)) == 1) {
		halbb_set_reg_cmn(bb, cr->sts_keeper_addr, cr->sts_keeper_addr_m, 2, HW_PHY_0); // 0x738[15:18]
		cfo = (u8)halbb_get_reg_cmn(bb, cr->sts_keeper_data, 0xfff00000, HW_PHY_0); // Only use [31:20], Total [31:0]
		bb->bb_cmn_backup_i.last_cfo = cfo;
	} else {
		cfo = bb->bb_cmn_backup_i.last_cfo;
	}

	// Set to Write State //
	halbb_set_reg_cmn(bb, cr->sts_keeper_read, cr->sts_keeper_read_m, 0, HW_PHY_0);

	// Release DBG port //
	halbb_release_bb_dbg_port(bb);

	return cfo;
}

void halbb_cvrt_2_mp(struct bb_info *bb)
{
	BB_DBG(bb, DBG_COMMON_FLOW, "[%s]", __func__);
	
	bb->bb_link_i.is_linked = true;
	halbb_physts_cvrt_2_mp(bb);
}

void halbb_mp_dbg(struct bb_info *bb, char input[][16], u32 *_used,
		  char *output, u32 *_out_len)
{
	u32 val[10] = {0};
	u32 used = *_used;
	u32 out_len = *_out_len;
	struct rxevm_physts rxevm;
	halbb_mem_set(bb, &rxevm,0,sizeof(rxevm));

	if (_os_strcmp(input[1], "-h") == 0) {
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "EVM({phy_idx (0~1)})\n");

	} else if (_os_strcmp(input[1], "evm") == 0) {
		HALBB_SCAN(input[2], DCMD_DECIMAL, &val[0]);

		rxevm = halbb_mp_get_rxevm_physts(bb, (enum phl_phy_idx)val[0]);

		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			    "[MP] [User 0] [SS0] RXEVM = -%d\n",
			    rxevm.rxevm_seg[0].rxevm_user[0].rxevm_ss_0 >> 2);
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			    "[MP] [User 0] [SS1] RXEVM = -%d\n",
			    rxevm.rxevm_seg[0].rxevm_user[0].rxevm_ss_1 >> 2);

	}

	*_used = used;
	*_out_len = out_len;
}

void halbb_cr_cfg_mp_init(struct bb_info *bb)
{
	struct bb_rpt_info *rpt_info = &bb->bb_rpt_i;
	struct bb_rpt_cr_info *cr = &rpt_info->bb_rpt_cr_i;

	switch (bb->cr_type) {

	#ifdef HALBB_COMPILE_AP_SERIES
	case BB_AP:
		cr->cnt_ccktxon = CNT_CCKTXON_A;
		cr->cnt_ccktxon_m = CNT_CCKTXON_A_M;
		cr->cnt_ofdmtxon = CNT_OFDMTXON_A;
		cr->cnt_ofdmtxon_m = CNT_OFDMTXON_A_M;
		cr->cnt_cck_crc32ok_p0 = CNT_CCK_CRC32OK_P0_A;
		cr->cnt_cck_crc32ok_p0_m = CNT_CCK_CRC32OK_P0_A_M;
		cr->cnt_cck_crc32ok_p1 = CNT_CCK_CRC32OK_P1_A;
		cr->cnt_cck_crc32ok_p1_m = CNT_CCK_CRC32OK_P1_A_M;
		cr->cnt_l_crc_ok = CNT_L_CRC_OK_A;
		cr->cnt_l_crc_ok_m = CNT_L_CRC_OK_A_M;
		cr->cnt_ht_crc_ok = CNT_HT_CRC_OK_A;
		cr->cnt_ht_crc_ok_m = CNT_HT_CRC_OK_A_M;
		cr->cnt_vht_crc_ok = CNT_VHT_CRC_OK_A;
		cr->cnt_vht_crc_ok_m = CNT_VHT_CRC_OK_A_M;
		cr->cnt_he_crc_ok = CNT_HE_CRC_OK_A;
		cr->cnt_he_crc_ok_m = CNT_HE_CRC_OK_A_M;
		cr->cnt_cck_crc32fail_p0 = CNT_CCK_CRC32FAIL_P0_A;
		cr->cnt_cck_crc32fail_p0_m = CNT_CCK_CRC32FAIL_P0_A_M;
		cr->cnt_cck_crc32fail_p1 = CNT_CCK_CRC32FAIL_P1_A;
		cr->cnt_cck_crc32fail_p1_m = CNT_CCK_CRC32FAIL_P1_A_M;
		cr->cnt_l_crc_err = CNT_L_CRC_ERR_A;
		cr->cnt_l_crc_err_m = CNT_L_CRC_ERR_A_M;
		cr->cnt_ht_crc_err = CNT_HT_CRC_ERR_A;
		cr->cnt_ht_crc_err_m = CNT_HT_CRC_ERR_A_M;
		cr->cnt_vht_crc_err = CNT_VHT_CRC_ERR_A;
		cr->cnt_vht_crc_err_m = CNT_VHT_CRC_ERR_A_M;
		cr->cnt_he_crc_err = CNT_HE_CRC_ERR_A;
		cr->cnt_he_crc_err_m = CNT_HE_CRC_ERR_A_M;
		cr->rst_all_cnt = RST_ALL_CNT_A;
		cr->rst_all_cnt_m = RST_ALL_CNT_A_M;
		cr->phy_sts_bitmap_he_mu = PHY_STS_BITMAP_HE_MU_A;
		cr->phy_sts_bitmap_he_mu_m = PHY_STS_BITMAP_HE_MU_A_M;
		cr->phy_sts_bitmap_vht_mu = PHY_STS_BITMAP_VHT_MU_A;
		cr->phy_sts_bitmap_vht_mu_m = PHY_STS_BITMAP_VHT_MU_A_M;
		cr->phy_sts_bitmap_cck = PHY_STS_BITMAP_CCK_A;
		cr->phy_sts_bitmap_cck_m = PHY_STS_BITMAP_CCK_A_M;
		cr->phy_sts_bitmap_legacy = PHY_STS_BITMAP_LEGACY_A;
		cr->phy_sts_bitmap_legacy_m = PHY_STS_BITMAP_LEGACY_A_M;
		cr->phy_sts_bitmap_ht = PHY_STS_BITMAP_HT_A;
		cr->phy_sts_bitmap_ht_m = PHY_STS_BITMAP_HT_A_M;
		cr->phy_sts_bitmap_vht = PHY_STS_BITMAP_VHT_A;
		cr->phy_sts_bitmap_vht_m = PHY_STS_BITMAP_VHT_A_M;
		cr->phy_sts_bitmap_he = PHY_STS_BITMAP_HE_A;
		cr->phy_sts_bitmap_he_m = PHY_STS_BITMAP_HE_A_M;
		cr->rpt_tone_evm_idx = RPT_TONE_EVM_IDX_A;
		cr->rpt_tone_evm_idx_m = RPT_TONE_EVM_IDX_A_M;
		cr->dbg_port_ref_clk_en = DBG_PORT_REF_CLK_EN_A;
		cr->dbg_port_ref_clk_en_m = DBG_PORT_REF_CLK_EN_A_M;
		cr->dbg_port_en = DBG_PORT_EN_A;
		cr->dbg_port_en_m = DBG_PORT_EN_A_M;
		cr->dbg_port_ip_sel = DBG_PORT_IP_SEL_A;
		cr->dbg_port_ip_sel_m = DBG_PORT_IP_SEL_A_M;
		cr->dbg_port_sel = DBG_PORT_SEL_A;
		cr->dbg_port_sel_m = DBG_PORT_SEL_A_M;
		cr->dbg32_d = DBG32_D_A;
		cr->dbg32_d_m = DBG32_D_A_M;
		cr->phy_sts_bitmap_trigbase = PHY_STS_BITMAP_TRIGBASE_A;
		cr->phy_sts_bitmap_trigbase_m = PHY_STS_BITMAP_TRIGBASE_A_M;
		cr->sts_keeper_en = STS_KEEPER_EN_A;
		cr->sts_keeper_en_m = STS_KEEPER_EN_A_M;
		cr->sts_keeper_trig_cond = STS_KEEPER_TRIG_COND_A;
		cr->sts_keeper_trig_cond_m = STS_KEEPER_TRIG_COND_A_M;
		cr->sts_dbg_sel = STS_DBG_SEL_A;
		cr->sts_dbg_sel_m = STS_DBG_SEL_A_M;
		cr->sts_keeper_read = STS_KEEPER_READ_A;
		cr->sts_keeper_read_m = STS_KEEPER_READ_A_M;
		cr->sts_keeper_addr = STS_KEEPER_ADDR_A;
		cr->sts_keeper_addr_m = STS_KEEPER_ADDR_A_M;
		cr->sts_keeper_data = STS_KEEPER_DATA_A;
		cr->sts_keeper_data_m = STS_KEEPER_DATA_A_M;
		cr->sts_user_sel = STS_USER_SEL_A;
		cr->sts_user_sel_m = STS_USER_SEL_A_M;
		break;

	#endif
	#ifdef HALBB_COMPILE_AP2_SERIES
	case BB_AP2:
		cr->cnt_ccktxon = CNT_CCKTXON_A2;
		cr->cnt_ccktxon_m = CNT_CCKTXON_A2_M;
		cr->cnt_ofdmtxon = CNT_OFDMTXON_A2;
		cr->cnt_ofdmtxon_m = CNT_OFDMTXON_A2_M;
		cr->cnt_cck_crc32ok_p0 = CNT_CCK_CRC32OK_P0_A2;
		cr->cnt_cck_crc32ok_p0_m = CNT_CCK_CRC32OK_P0_A2_M;
		cr->cnt_cck_crc32ok_p1 = CNT_CCK_CRC32OK_P1_A2;
		cr->cnt_cck_crc32ok_p1_m = CNT_CCK_CRC32OK_P1_A2_M;
		cr->cnt_l_crc_ok = CNT_L_CRC_OK_A2;
		cr->cnt_l_crc_ok_m = CNT_L_CRC_OK_A2_M;
		cr->cnt_ht_crc_ok = CNT_HT_CRC_OK_A2;
		cr->cnt_ht_crc_ok_m = CNT_HT_CRC_OK_A2_M;
		cr->cnt_vht_crc_ok = CNT_VHT_CRC_OK_A2;
		cr->cnt_vht_crc_ok_m = CNT_VHT_CRC_OK_A2_M;
		cr->cnt_he_crc_ok = CNT_HE_CRC_OK_A2;
		cr->cnt_he_crc_ok_m = CNT_HE_CRC_OK_A2_M;
		cr->cnt_cck_crc32fail_p0 = CNT_CCK_CRC32FAIL_P0_A2;
		cr->cnt_cck_crc32fail_p0_m = CNT_CCK_CRC32FAIL_P0_A2_M;
		cr->cnt_cck_crc32fail_p1 = CNT_CCK_CRC32FAIL_P1_A2;
		cr->cnt_cck_crc32fail_p1_m = CNT_CCK_CRC32FAIL_P1_A2_M;
		cr->cnt_l_crc_err = CNT_L_CRC_ERR_A2;
		cr->cnt_l_crc_err_m = CNT_L_CRC_ERR_A2_M;
		cr->cnt_ht_crc_err = CNT_HT_CRC_ERR_A2;
		cr->cnt_ht_crc_err_m = CNT_HT_CRC_ERR_A2_M;
		cr->cnt_vht_crc_err = CNT_VHT_CRC_ERR_A2;
		cr->cnt_vht_crc_err_m = CNT_VHT_CRC_ERR_A2_M;
		cr->cnt_he_crc_err = CNT_HE_CRC_ERR_A2;
		cr->cnt_he_crc_err_m = CNT_HE_CRC_ERR_A2_M;
		cr->rst_all_cnt = RST_ALL_CNT_A2;
		cr->rst_all_cnt_m = RST_ALL_CNT_A2_M;
		cr->phy_sts_bitmap_he_mu = PHY_STS_BITMAP_HE_MU_A2;
		cr->phy_sts_bitmap_he_mu_m = PHY_STS_BITMAP_HE_MU_A2_M;
		cr->phy_sts_bitmap_vht_mu = PHY_STS_BITMAP_VHT_MU_A2;
		cr->phy_sts_bitmap_vht_mu_m = PHY_STS_BITMAP_VHT_MU_A2_M;
		cr->phy_sts_bitmap_cck = PHY_STS_BITMAP_CCK_A2;
		cr->phy_sts_bitmap_cck_m = PHY_STS_BITMAP_CCK_A2_M;
		cr->phy_sts_bitmap_legacy = PHY_STS_BITMAP_LEGACY_A2;
		cr->phy_sts_bitmap_legacy_m = PHY_STS_BITMAP_LEGACY_A2_M;
		cr->phy_sts_bitmap_ht = PHY_STS_BITMAP_HT_A2;
		cr->phy_sts_bitmap_ht_m = PHY_STS_BITMAP_HT_A2_M;
		cr->phy_sts_bitmap_vht = PHY_STS_BITMAP_VHT_A2;
		cr->phy_sts_bitmap_vht_m = PHY_STS_BITMAP_VHT_A2_M;
		cr->phy_sts_bitmap_he = PHY_STS_BITMAP_HE_A2;
		cr->phy_sts_bitmap_he_m = PHY_STS_BITMAP_HE_A2_M;
		cr->rpt_tone_evm_idx = RPT_TONE_EVM_IDX_A2;
		cr->rpt_tone_evm_idx_m = RPT_TONE_EVM_IDX_A2_M;
		cr->dbg_port_ref_clk_en = DBG_PORT_REF_CLK_EN_A2;
		cr->dbg_port_ref_clk_en_m = DBG_PORT_REF_CLK_EN_A2_M;
		cr->dbg_port_en = DBG_PORT_EN_A2;
		cr->dbg_port_en_m = DBG_PORT_EN_A2_M;
		cr->dbg_port_ip_sel = DBG_PORT_IP_SEL_A2;
		cr->dbg_port_ip_sel_m = DBG_PORT_IP_SEL_A2_M;
		cr->dbg_port_sel = DBG_PORT_SEL_A2;
		cr->dbg_port_sel_m = DBG_PORT_SEL_A2_M;
		cr->dbg32_d = DBG32_D_A2;
		cr->dbg32_d_m = DBG32_D_A2_M;
		cr->phy_sts_bitmap_trigbase = PHY_STS_BITMAP_TRIGBASE_A2;
		cr->phy_sts_bitmap_trigbase_m = PHY_STS_BITMAP_TRIGBASE_A2_M;
		cr->sts_keeper_en = STS_KEEPER_EN_A2;
		cr->sts_keeper_en_m = STS_KEEPER_EN_A2_M;
		cr->sts_keeper_trig_cond = STS_KEEPER_TRIG_COND_A2;
		cr->sts_keeper_trig_cond_m = STS_KEEPER_TRIG_COND_A2_M;
		cr->sts_dbg_sel = STS_DBG_SEL_A2;
		cr->sts_dbg_sel_m = STS_DBG_SEL_A2_M;
		cr->sts_keeper_read = STS_KEEPER_READ_A2;
		cr->sts_keeper_read_m = STS_KEEPER_READ_A2_M;
		cr->sts_keeper_addr = STS_KEEPER_ADDR_A2;
		cr->sts_keeper_addr_m = STS_KEEPER_ADDR_A2_M;
		cr->sts_keeper_data = STS_KEEPER_DATA_A2;
		cr->sts_keeper_data_m = STS_KEEPER_DATA_A2_M;
		cr->path0_rssi_at_agc_rdy = PATH0_RSSI_AT_AGC_RDY_A2;
		cr->path0_rssi_at_agc_rdy_m = PATH0_RSSI_AT_AGC_RDY_A2_M;
		cr->path1_rssi_at_agc_rdy = PATH1_RSSI_AT_AGC_RDY_A2;
		cr->path1_rssi_at_agc_rdy_m = PATH1_RSSI_AT_AGC_RDY_A2_M;
		cr->sts_user_sel = STS_USER_SEL_A2;
		cr->sts_user_sel_m = STS_USER_SEL_A2_M;
		cr->path1_g_lna6 = PATH1_R_G_G_LNA6_A2;
		cr->path1_g_lna6_m = PATH1_R_G_G_LNA6_A2_M;
		break;

	#endif
	#ifdef HALBB_COMPILE_CLIENT_SERIES
	case BB_CLIENT:
		cr->cnt_ccktxon = CNT_CCKTXON_C;
		cr->cnt_ccktxon_m = CNT_CCKTXON_C_M;
		cr->cnt_ofdmtxon = CNT_OFDMTXON_C;
		cr->cnt_ofdmtxon_m = CNT_OFDMTXON_C_M;
		cr->cnt_cck_crc32ok_p0 = CNT_CCK_CRC32OK_P0_C;
		cr->cnt_cck_crc32ok_p0_m = CNT_CCK_CRC32OK_P0_C_M;
		cr->cnt_cck_crc32ok_p1 = CNT_CCK_CRC32OK_P1_C;
		cr->cnt_cck_crc32ok_p1_m = CNT_CCK_CRC32OK_P1_C_M;
		cr->cnt_l_crc_ok = CNT_L_CRC_OK_C;
		cr->cnt_l_crc_ok_m = CNT_L_CRC_OK_C_M;
		cr->cnt_ht_crc_ok = CNT_HT_CRC_OK_C;
		cr->cnt_ht_crc_ok_m = CNT_HT_CRC_OK_C_M;
		cr->cnt_vht_crc_ok = CNT_VHT_CRC_OK_C;
		cr->cnt_vht_crc_ok_m = CNT_VHT_CRC_OK_C_M;
		cr->cnt_he_crc_ok = CNT_HE_CRC_OK_C;
		cr->cnt_he_crc_ok_m = CNT_HE_CRC_OK_C_M;
		cr->cnt_cck_crc32fail_p0 = CNT_CCK_CRC32FAIL_P0_C;
		cr->cnt_cck_crc32fail_p0_m = CNT_CCK_CRC32FAIL_P0_C_M;
		cr->cnt_cck_crc32fail_p1 = CNT_CCK_CRC32FAIL_P1_C;
		cr->cnt_cck_crc32fail_p1_m = CNT_CCK_CRC32FAIL_P1_C_M;
		cr->cnt_l_crc_err = CNT_L_CRC_ERR_C;
		cr->cnt_l_crc_err_m = CNT_L_CRC_ERR_C_M;
		cr->cnt_ht_crc_err = CNT_HT_CRC_ERR_C;
		cr->cnt_ht_crc_err_m = CNT_HT_CRC_ERR_C_M;
		cr->cnt_vht_crc_err = CNT_VHT_CRC_ERR_C;
		cr->cnt_vht_crc_err_m = CNT_VHT_CRC_ERR_C_M;
		cr->cnt_he_crc_err = CNT_HE_CRC_ERR_C;
		cr->cnt_he_crc_err_m = CNT_HE_CRC_ERR_C_M;
		cr->rst_all_cnt = RST_ALL_CNT_C;
		cr->rst_all_cnt_m = RST_ALL_CNT_C_M;
		cr->phy_sts_bitmap_he_mu = PHY_STS_BITMAP_HE_MU_C;
		cr->phy_sts_bitmap_he_mu_m = PHY_STS_BITMAP_HE_MU_C_M;
		cr->phy_sts_bitmap_vht_mu = PHY_STS_BITMAP_VHT_MU_C;
		cr->phy_sts_bitmap_vht_mu_m = PHY_STS_BITMAP_VHT_MU_C_M;
		cr->phy_sts_bitmap_cck = PHY_STS_BITMAP_CCK_C;
		cr->phy_sts_bitmap_cck_m = PHY_STS_BITMAP_CCK_C_M;
		cr->phy_sts_bitmap_legacy = PHY_STS_BITMAP_LEGACY_C;
		cr->phy_sts_bitmap_legacy_m = PHY_STS_BITMAP_LEGACY_C_M;
		cr->phy_sts_bitmap_ht = PHY_STS_BITMAP_HT_C;
		cr->phy_sts_bitmap_ht_m = PHY_STS_BITMAP_HT_C_M;
		cr->phy_sts_bitmap_vht = PHY_STS_BITMAP_VHT_C;
		cr->phy_sts_bitmap_vht_m = PHY_STS_BITMAP_VHT_C_M;
		cr->phy_sts_bitmap_he = PHY_STS_BITMAP_HE_C;
		cr->phy_sts_bitmap_he_m = PHY_STS_BITMAP_HE_C_M;
		cr->rpt_tone_evm_idx = RPT_TONE_EVM_IDX_C;
		cr->rpt_tone_evm_idx_m = RPT_TONE_EVM_IDX_C_M;
		cr->dbg_port_ref_clk_en = DBG_PORT_REF_CLK_EN_C;
		cr->dbg_port_ref_clk_en_m = DBG_PORT_REF_CLK_EN_C_M;
		cr->dbg_port_en = DBG_PORT_EN_C;
		cr->dbg_port_en_m = DBG_PORT_EN_C_M;
		cr->dbg_port_ip_sel = DBG_PORT_IP_SEL_C;
		cr->dbg_port_ip_sel_m = DBG_PORT_IP_SEL_C_M;
		cr->dbg_port_sel = DBG_PORT_SEL_C;
		cr->dbg_port_sel_m = DBG_PORT_SEL_C_M;
		cr->dbg32_d = DBG32_D_C;
		cr->dbg32_d_m = DBG32_D_C_M;
		cr->phy_sts_bitmap_trigbase = PHY_STS_BITMAP_TRIGBASE_C;
		cr->phy_sts_bitmap_trigbase_m = PHY_STS_BITMAP_TRIGBASE_C_M;
		cr->sts_keeper_en = STS_KEEPER_EN_C;
		cr->sts_keeper_en_m = STS_KEEPER_EN_C_M;
		cr->sts_keeper_trig_cond = STS_KEEPER_TRIG_COND_C;
		cr->sts_keeper_trig_cond_m = STS_KEEPER_TRIG_COND_C_M;
		cr->sts_dbg_sel = STS_DBG_SEL_C;
		cr->sts_dbg_sel_m = STS_DBG_SEL_C_M;
		cr->sts_keeper_read = STS_KEEPER_READ_C;
		cr->sts_keeper_read_m = STS_KEEPER_READ_C_M;
		cr->sts_keeper_addr = STS_KEEPER_ADDR_C;
		cr->sts_keeper_addr_m = STS_KEEPER_ADDR_C_M;
		cr->sts_keeper_data = STS_KEEPER_DATA_C;
		cr->sts_keeper_data_m = STS_KEEPER_DATA_C_M;
		cr->pw_dbm_rx0 = P0_L_TOT_PW_DBM_RX0_C;
		cr->pw_dbm_rx0_m = P0_L_TOT_PW_DBM_RX0_C_M;
		cr->sts_user_sel = STS_USER_SEL_C;
		cr->sts_user_sel_m = STS_USER_SEL_C_M;
		cr->path1_g_lna6 = PATH1_R_G_G_LNA6_C;
		cr->path1_g_lna6_m = PATH1_R_G_G_LNA6_C_M;
		break;
	#endif

	#ifdef HALBB_COMPILE_BE0_SERIES
	case BB_BE0:
		cr->cnt_ccktxon = CNT_CCKTXON_BE0;
		cr->cnt_ccktxon_m = CNT_CCKTXON_BE0_M;
		cr->cnt_ofdmtxon = CNT_OFDMTXON_BE0;
		cr->cnt_ofdmtxon_m = CNT_OFDMTXON_BE0_M;
		cr->cnt_cck_crc32ok_p0 = CNT_CCK_CRC32OK_P0_BE0;
		cr->cnt_cck_crc32ok_p0_m = CNT_CCK_CRC32OK_P0_BE0_M;
		cr->cnt_cck_crc32ok_p1 = CNT_CCK_CRC32OK_P1_BE0;
		cr->cnt_cck_crc32ok_p1_m = CNT_CCK_CRC32OK_P1_BE0_M;
		cr->cnt_l_crc_ok = CNT_L_CRC_OK_BE0;
		cr->cnt_l_crc_ok_m = CNT_L_CRC_OK_BE0_M;
		cr->cnt_ht_crc_ok = CNT_HT_CRC_OK_BE0;
		cr->cnt_ht_crc_ok_m = CNT_HT_CRC_OK_BE0_M;
		cr->cnt_vht_crc_ok = CNT_VHT_CRC_OK_BE0;
		cr->cnt_vht_crc_ok_m = CNT_VHT_CRC_OK_BE0_M;
		cr->cnt_he_crc_ok = CNT_HE_CRC_OK_BE0;
		cr->cnt_he_crc_ok_m = CNT_HE_CRC_OK_BE0_M;
		cr->cnt_cck_crc32fail_p0 = CNT_CCK_CRC32FAIL_P0_BE0;
		cr->cnt_cck_crc32fail_p0_m = CNT_CCK_CRC32FAIL_P0_BE0_M;
		cr->cnt_cck_crc32fail_p1 = CNT_CCK_CRC32FAIL_P1_BE0;
		cr->cnt_cck_crc32fail_p1_m = CNT_CCK_CRC32FAIL_P1_BE0_M;
		cr->cnt_l_crc_err = CNT_L_CRC_ERR_BE0;
		cr->cnt_l_crc_err_m = CNT_L_CRC_ERR_BE0_M;
		cr->cnt_ht_crc_err = CNT_HT_CRC_ERR_BE0;
		cr->cnt_ht_crc_err_m = CNT_HT_CRC_ERR_BE0_M;
		cr->cnt_vht_crc_err = CNT_VHT_CRC_ERR_BE0;
		cr->cnt_vht_crc_err_m = CNT_VHT_CRC_ERR_BE0_M;
		cr->cnt_he_crc_err = CNT_HE_CRC_ERR_BE0;
		cr->cnt_he_crc_err_m = CNT_HE_CRC_ERR_BE0_M;
		cr->rst_all_cnt = RST_ALL_CNT_BE0;
		cr->rst_all_cnt_m = RST_ALL_CNT_BE0_M;
		cr->phy_sts_bitmap_he_mu = PHY_STS_BITMAP_HE_MU_BE0;
		cr->phy_sts_bitmap_he_mu_m = PHY_STS_BITMAP_HE_MU_BE0_M;
		cr->phy_sts_bitmap_vht_mu = PHY_STS_BITMAP_VHT_MU_BE0;
		cr->phy_sts_bitmap_vht_mu_m = PHY_STS_BITMAP_VHT_MU_BE0_M;
		cr->phy_sts_bitmap_cck = PHY_STS_BITMAP_CCK_BE0;
		cr->phy_sts_bitmap_cck_m = PHY_STS_BITMAP_CCK_BE0_M;
		cr->phy_sts_bitmap_legacy = PHY_STS_BITMAP_LEGACY_BE0;
		cr->phy_sts_bitmap_legacy_m = PHY_STS_BITMAP_LEGACY_BE0_M;
		cr->phy_sts_bitmap_ht = PHY_STS_BITMAP_HT_BE0;
		cr->phy_sts_bitmap_ht_m = PHY_STS_BITMAP_HT_BE0_M;
		cr->phy_sts_bitmap_vht = PHY_STS_BITMAP_VHT_BE0;
		cr->phy_sts_bitmap_vht_m = PHY_STS_BITMAP_VHT_BE0_M;
		cr->phy_sts_bitmap_he = PHY_STS_BITMAP_HE_BE0;
		cr->phy_sts_bitmap_he_m = PHY_STS_BITMAP_HE_BE0_M;
		cr->rpt_tone_evm_idx = RXINT_R_RPT_TONE_EVM_IDX_BE0;
		cr->rpt_tone_evm_idx_m = RXINT_R_RPT_TONE_EVM_IDX_BE0_M;
		cr->dbg_port_ref_clk_en = DBG_PORT_REF_CLK_EN_BE0;
		cr->dbg_port_ref_clk_en_m = DBG_PORT_REF_CLK_EN_BE0_M;
		cr->dbg_port_en = DBG_PORT_EN_BE0;
		cr->dbg_port_en_m = DBG_PORT_EN_BE0_M;
		cr->dbg_port_ip_sel = DBG_PORT_IP_SEL_BE0;
		cr->dbg_port_ip_sel_m = DBG_PORT_IP_SEL_BE0_M;
		cr->dbg_port_sel = DBG_PORT_SEL_BE0;
		cr->dbg_port_sel_m = DBG_PORT_SEL_BE0_M;
		cr->dbg32_d = DBG32_D_BE0;
		cr->dbg32_d_m = DBG32_D_BE0_M;
		cr->phy_sts_bitmap_trigbase = PHY_STS_BITMAP_TRIGBASE_BE0;
		cr->phy_sts_bitmap_trigbase_m = PHY_STS_BITMAP_TRIGBASE_BE0_M;
		cr->sts_keeper_en = STS_KEEPER_EN_BE0;
		cr->sts_keeper_en_m = STS_KEEPER_EN_BE0_M;
		cr->sts_keeper_trig_cond = STS_KEEPER_TRIG_COND_BE0;
		cr->sts_keeper_trig_cond_m = STS_KEEPER_TRIG_COND_BE0_M;
		cr->sts_dbg_sel = STS_DBG_SEL_BE0;
		cr->sts_dbg_sel_m = STS_DBG_SEL_BE0_M;
		cr->sts_keeper_read = STS_KEEPER_READ_BE0;
		cr->sts_keeper_read_m = STS_KEEPER_READ_BE0_M;
		cr->sts_keeper_addr = STS_KEEPER_ADDR_BE0;
		cr->sts_keeper_addr_m = STS_KEEPER_ADDR_BE0_M;
		cr->sts_keeper_data = STS_KEEPER_DATA_BE0;
		cr->sts_keeper_data_m = STS_KEEPER_DATA_BE0_M;
		cr->path0_rssi_at_agc_rdy = PATH0_RSSI_AT_AGC_RDY_BE0;
		cr->path0_rssi_at_agc_rdy_m = PATH0_RSSI_AT_AGC_RDY_BE0_M;
		cr->path1_rssi_at_agc_rdy = PATH1_RSSI_AT_AGC_RDY_BE0;
		cr->path1_rssi_at_agc_rdy_m = PATH1_RSSI_AT_AGC_RDY_BE0_M;
		cr->sts_user_sel = STS_USER_SEL_BE0;
		cr->sts_user_sel_m = STS_USER_SEL_BE0_M;
		cr->path1_g_lna6 = PATH1_R_G_G_LNA6_BE0;
		cr->path1_g_lna6_m = PATH1_R_G_G_LNA6_BE0_M;
		break;
	#endif

	default:
		BB_WARNING("[%s] BBCR Hook FAIL!\n", __func__);
		if (bb->bb_dbg_i.cr_fake_init_hook_en) {
			BB_TRACE("[%s] BBCR fake init\n", __func__);
			halbb_cr_hook_fake_init(bb, (u32 *)cr, (sizeof(struct bb_rpt_cr_info) >> 2));
		}
		break;
	}

	if (bb->bb_dbg_i.cr_init_hook_recorder_en) {
		BB_TRACE("[%s] BBCR Hook dump\n", __func__);
		halbb_cr_hook_init_dump(bb, (u32 *)cr, (sizeof(struct bb_rpt_cr_info) >> 2));
	}
}



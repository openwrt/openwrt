/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation. All rights reserved.
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
 ******************************************************************************/

#include "addr_cam.h"

struct mac_ax_mc_table {
	u8 valid;
	struct mac_ax_multicast_info mc;
	struct mac_ax_role_info role;
};

#define MAC_AX_MAX_MC_ENTRY 32
static struct mac_ax_mc_table mc_role[MAC_AX_MAX_MC_ENTRY];

#define MAC_AX_NO_HIT_IDX 0xFF

static u8 get_set_bits_of_msk(u8 msk)
{
	u8 set_bits;

	if (msk == 0)
		return 0;
	set_bits = msk & (msk - 1);
	if (set_bits == 0)
		return 1;
	return get_set_bits_of_msk(set_bits) + 1;
}

u32 find_avail_addr_cam_entry(struct mac_ax_adapter *adapter,
			      struct mac_ax_role_info *info)
{
	u16 i;

	for (i = 0; i < adapter->hw_info->macid_num; i++) {
		if (!mac_role_srch_by_addr_cam(adapter, i))
			break;
	}
	if (i == adapter->hw_info->macid_num)
		return MACADDRCAMFL;
	info->a_info.addr_cam_idx = (u8)i;

	return MACSUCCESS;
}

u32 find_avail_bssid_cam_entry(struct mac_ax_adapter *adapter,
			       struct mac_ax_role_info *info)
{
	u8 i;
	struct mac_role_tbl *role;
	u8 maddr_cmp_len = ETH_ALEN;

	info->a_info.bssid_cam_idx = adapter->hw_info->bssid_num;
	info->b_info.bssid_cam_idx = adapter->hw_info->bssid_num;

	if (info->a_info.mask_sel == MAC_AX_BSSID_MSK)
		maddr_cmp_len = get_set_bits_of_msk(info->a_info.addr_mask);

	for (i = 0; i < adapter->hw_info->bssid_num; i++) {
		role = mac_role_srch_by_bssid(adapter, i);
		if (role &&
		    !PLTFM_MEMCMP(info->bssid,
				  role->info.bssid, maddr_cmp_len) &&
		    info->band == role->info.band) {
			info->a_info.bssid_cam_idx =
				role->info.a_info.bssid_cam_idx;
			if (maddr_cmp_len < ETH_ALEN) {
				PLTFM_MEMCPY(info->b_info.bssid,
					     role->info.b_info.bssid,
					     ETH_ALEN);
			}
			break;
		} else if (!role) {
			if (info->a_info.bssid_cam_idx ==
			    adapter->hw_info->bssid_num) {
				info->a_info.bssid_cam_idx = i;
			}
		}
	}

	if (info->a_info.bssid_cam_idx == adapter->hw_info->bssid_num)
		return MACBSSIDCAMFL;
	info->b_info.bssid_cam_idx = info->a_info.bssid_cam_idx;

	return MACSUCCESS;
}

u32 init_addr_cam_info(struct mac_ax_adapter *adapter,
		       struct mac_ax_role_info *info,
		       struct fwcmd_addrcam_info *fw_addrcam)
{
	u32 ret;

	ret = find_avail_addr_cam_entry(adapter, info);
	if (ret)
		return ret;

	ret = find_avail_bssid_cam_entry(adapter, info);
	if (ret)
		return ret;

	fill_addr_cam_info(adapter, info, fw_addrcam);
	fill_bssid_cam_info(adapter, info, fw_addrcam);

	return MACSUCCESS;
}

u32 change_addr_cam_info(struct mac_ax_adapter *adapter,
			 struct mac_ax_role_info *info,
			 struct fwcmd_addrcam_info *fw_addrcam)
{
	u32 ret;

	ret = find_avail_bssid_cam_entry(adapter, info);
	if (ret)
		return ret;

	fill_addr_cam_info(adapter, info, fw_addrcam);
	fill_bssid_cam_info(adapter, info, fw_addrcam);

	return MACSUCCESS;
}

u32 fill_addr_cam_info(struct mac_ax_adapter *adapter,
		       struct mac_ax_role_info *info,
		       struct fwcmd_addrcam_info *fw_addrcam)
{
	struct mac_ax_addr_cam_info a_info;
	u8 i;
	u8 sma_hash = 0x00;
	u8 tma_hash = 0x00;
	u8 maddr_cmp_len;

	maddr_cmp_len = get_set_bits_of_msk(info->a_info.addr_mask);

	switch (info->a_info.mask_sel) {
	case MAC_AX_SMA_MSK:
		for (i = 0; i < maddr_cmp_len; i++)
			sma_hash ^= info->a_info.sma[i];
		for (i = 0; i < ETH_ALEN; i++)
			tma_hash ^= info->a_info.tma[i];
		break;
	case MAC_AX_TMA_MSK:
		for (i = 0; i < ETH_ALEN; i++)
			sma_hash ^= info->a_info.sma[i];
		for (i = 0; i < maddr_cmp_len; i++)
			tma_hash ^= info->a_info.tma[i];
		break;
	case MAC_AX_NO_MSK:
	case MAC_AX_BSSID_MSK:
	default:
		for (i = 0; i < ETH_ALEN; i++)
			sma_hash ^= info->a_info.sma[i];
		for (i = 0; i < ETH_ALEN; i++)
			tma_hash ^= info->a_info.tma[i];
		break;
	}

	a_info = info->a_info;

	fw_addrcam->dword1 =
	  cpu_to_le32(SET_WORD(a_info.addr_cam_idx,
			       FWCMD_H2C_ADDRCAM_INFO_IDX) |
		      SET_WORD(a_info.offset, FWCMD_H2C_ADDRCAM_INFO_OFFSET) |
		      SET_WORD(a_info.len, FWCMD_H2C_ADDRCAM_INFO_LEN));

	fw_addrcam->dword2 =
	  cpu_to_le32(((a_info.valid) ? FWCMD_H2C_ADDRCAM_INFO_VALID : 0) |
	   SET_WORD(a_info.net_type, FWCMD_H2C_ADDRCAM_INFO_NET_TYPE) |
	   SET_WORD(a_info.bcn_hit_cond, FWCMD_H2C_ADDRCAM_INFO_BCN_HIT_COND) |
	   SET_WORD(a_info.hit_rule, FWCMD_H2C_ADDRCAM_INFO_HIT_RULE) |
	   ((a_info.bb_sel) ? FWCMD_H2C_ADDRCAM_INFO_BB_SEL : 0) |
	   SET_WORD(a_info.addr_mask, FWCMD_H2C_ADDRCAM_INFO_ADDR_MASK) |
	   SET_WORD(a_info.mask_sel, FWCMD_H2C_ADDRCAM_INFO_MASK_SEL) |
	   SET_WORD(sma_hash, FWCMD_H2C_ADDRCAM_INFO_SMA_HASH) |
	   SET_WORD(tma_hash, FWCMD_H2C_ADDRCAM_INFO_TMA_HASH));

	fw_addrcam->dword3 =
	  cpu_to_le32(SET_WORD(a_info.bssid_cam_idx,
			       FWCMD_H2C_ADDRCAM_INFO_BSSID_CAM_IDX) |
	   ((a_info.is_mul_ent) ? FWCMD_H2C_ADDRCAM_INFO_IS_MUL_ENT : 0));

	fw_addrcam->dword4 =
	  cpu_to_le32(SET_WORD(a_info.sma[0], FWCMD_H2C_ADDRCAM_INFO_SMA0) |
	   SET_WORD(a_info.sma[1], FWCMD_H2C_ADDRCAM_INFO_SMA1) |
	   SET_WORD(a_info.sma[2], FWCMD_H2C_ADDRCAM_INFO_SMA2) |
	   SET_WORD(a_info.sma[3], FWCMD_H2C_ADDRCAM_INFO_SMA3));

	fw_addrcam->dword5 =
	  cpu_to_le32(SET_WORD(a_info.sma[4], FWCMD_H2C_ADDRCAM_INFO_SMA4) |
	   SET_WORD(a_info.sma[5], FWCMD_H2C_ADDRCAM_INFO_SMA5) |
	   SET_WORD(a_info.tma[0], FWCMD_H2C_ADDRCAM_INFO_TMA0) |
	   SET_WORD(a_info.tma[1], FWCMD_H2C_ADDRCAM_INFO_TMA1));

	fw_addrcam->dword6 =
	  cpu_to_le32(SET_WORD(a_info.tma[2], FWCMD_H2C_ADDRCAM_INFO_TMA2) |
	   SET_WORD(a_info.tma[3], FWCMD_H2C_ADDRCAM_INFO_TMA3) |
	   SET_WORD(a_info.tma[4], FWCMD_H2C_ADDRCAM_INFO_TMA4) |
	   SET_WORD(a_info.tma[5], FWCMD_H2C_ADDRCAM_INFO_TMA5));

	// dword7 rsvd

	fw_addrcam->dword8 =
	  cpu_to_le32(SET_WORD(a_info.macid, FWCMD_H2C_ADDRCAM_INFO_MACID) |
	   SET_WORD(a_info.port_int, FWCMD_H2C_ADDRCAM_INFO_PORT_INT) |
	   SET_WORD(a_info.tsf_sync, FWCMD_H2C_ADDRCAM_INFO_TSF_SYNC) |
	   ((a_info.tf_trs) ? FWCMD_H2C_ADDRCAM_INFO_TF_TRS : 0) |
	   ((a_info.lsig_txop) ? FWCMD_H2C_ADDRCAM_INFO_LSIG_TXOP : 0) |
	   SET_WORD(a_info.tgt_ind, FWCMD_H2C_ADDRCAM_INFO_TGT_IND) |
	   SET_WORD(a_info.frm_tgt_ind, FWCMD_H2C_ADDRCAM_INFO_FRM_TGT_IND));

	fw_addrcam->dword9 =
	  cpu_to_le32((a_info.aid12 & 0xfff) |
	   ((a_info.wol_pattern) ? FWCMD_H2C_ADDRCAM_INFO_WOL_PATTERN : 0) |
	   ((a_info.wol_uc) ? FWCMD_H2C_ADDRCAM_INFO_WOL_UC : 0) |
	   ((a_info.wol_magic) ? FWCMD_H2C_ADDRCAM_INFO_WOL_MAGIC : 0) |
	   ((a_info.wapi) ? FWCMD_H2C_ADDRCAM_INFO_WAPI : 0) |
	   SET_WORD(a_info.sec_ent_mode, FWCMD_H2C_ADDRCAM_INFO_SEC_ENT_MODE) |
	   SET_WORD(a_info.sec_ent_keyid[0],
		    FWCMD_H2C_ADDRCAM_INFO_SEC_ENT0_KEYID) |
	   SET_WORD(a_info.sec_ent_keyid[1],
		    FWCMD_H2C_ADDRCAM_INFO_SEC_ENT1_KEYID) |
	   SET_WORD(a_info.sec_ent_keyid[2],
		    FWCMD_H2C_ADDRCAM_INFO_SEC_ENT2_KEYID) |
	   SET_WORD(a_info.sec_ent_keyid[3],
		    FWCMD_H2C_ADDRCAM_INFO_SEC_ENT3_KEYID) |
	   SET_WORD(a_info.sec_ent_keyid[4],
		    FWCMD_H2C_ADDRCAM_INFO_SEC_ENT4_KEYID) |
	   SET_WORD(a_info.sec_ent_keyid[5],
		    FWCMD_H2C_ADDRCAM_INFO_SEC_ENT5_KEYID) |
	   SET_WORD(a_info.sec_ent_keyid[6],
		    FWCMD_H2C_ADDRCAM_INFO_SEC_ENT6_KEYID));

	fw_addrcam->dword10 =
	  cpu_to_le32(SET_WORD(a_info.sec_ent_valid,
			       FWCMD_H2C_ADDRCAM_INFO_SEC_ENT_VALID) |
	   SET_WORD(a_info.sec_ent[0], FWCMD_H2C_ADDRCAM_INFO_SEC_ENT0) |
	   SET_WORD(a_info.sec_ent[1], FWCMD_H2C_ADDRCAM_INFO_SEC_ENT1) |
	   SET_WORD(a_info.sec_ent[2], FWCMD_H2C_ADDRCAM_INFO_SEC_ENT2));

	fw_addrcam->dword11 =
	  cpu_to_le32(SET_WORD(a_info.sec_ent[3],
			       FWCMD_H2C_ADDRCAM_INFO_SEC_ENT3) |
	   SET_WORD(a_info.sec_ent[4], FWCMD_H2C_ADDRCAM_INFO_SEC_ENT4) |
	   SET_WORD(a_info.sec_ent[5], FWCMD_H2C_ADDRCAM_INFO_SEC_ENT5) |
	   SET_WORD(a_info.sec_ent[6], FWCMD_H2C_ADDRCAM_INFO_SEC_ENT6));

	return MACSUCCESS;
}

u32 fill_bssid_cam_info(struct mac_ax_adapter *adapter,
			struct mac_ax_role_info *role_info,
			struct fwcmd_addrcam_info *fw_addrcam)
{
	struct mac_ax_bssid_cam_info b_info = role_info->b_info;
	u8 msk = role_info->mask_sel == MAC_AX_BSSID_MSK ?
		 role_info->addr_mask : MAC_AX_MSK_NONE;

	fw_addrcam->dword12 =
	  cpu_to_le32(SET_WORD(b_info.bssid_cam_idx,
			       FWCMD_H2C_ADDRCAM_INFO_B_IDX) |
		      SET_WORD(b_info.offset, FWCMD_H2C_ADDRCAM_INFO_B_OFFSET) |
		      SET_WORD(b_info.len, FWCMD_H2C_ADDRCAM_INFO_B_LEN));

	fw_addrcam->dword13 =
	  cpu_to_le32(((b_info.valid) ? FWCMD_H2C_ADDRCAM_INFO_B_VALID : 0) |
	   SET_WORD(msk, FWCMD_H2C_ADDRCAM_INFO_B_MSK) |
	   ((b_info.bb_sel) ? FWCMD_H2C_ADDRCAM_INFO_B_BB_SEL : 0) |
	   SET_WORD(b_info.bss_color, FWCMD_H2C_ADDRCAM_INFO_BSS_COLOR) |
	   SET_WORD(b_info.bssid[0], FWCMD_H2C_ADDRCAM_INFO_BSSID0) |
	   SET_WORD(b_info.bssid[1], FWCMD_H2C_ADDRCAM_INFO_BSSID1));

	fw_addrcam->dword14 =
	  cpu_to_le32(SET_WORD(b_info.bssid[2], FWCMD_H2C_ADDRCAM_INFO_BSSID2) |
	   SET_WORD(b_info.bssid[3], FWCMD_H2C_ADDRCAM_INFO_BSSID3) |
	   SET_WORD(b_info.bssid[4], FWCMD_H2C_ADDRCAM_INFO_BSSID4) |
	   SET_WORD(b_info.bssid[5], FWCMD_H2C_ADDRCAM_INFO_BSSID5));

	return MACSUCCESS;
}

u32 mac_upd_addr_cam(struct mac_ax_adapter *adapter,
		     struct mac_ax_role_info *info,
		     enum mac_ax_role_opmode op)
{
	u32 tbl[21];
	u32 ret;
	u32 i;
	u8 *buf;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	struct fwcmd_addrcam_info *fwcmd_tbl;
	u8 ctlinfo_aidx_off;

	if (adapter->sm.fwdl == MAC_AX_FWDL_INIT_RDY) {
		h2cb = h2cb_alloc(adapter, H2CB_CLASS_DATA);
		if (!h2cb)
			return MACNPTR;
		buf = h2cb_put(h2cb, sizeof(struct fwcmd_addrcam_info));
		if (!buf) {
			ret = MACNOBUF;
			goto FWOFLD_END;
		}
		fwcmd_tbl = (struct fwcmd_addrcam_info *)buf;

		if (op == CHG)
			ret = change_addr_cam_info(adapter, info, fwcmd_tbl);
		else
			ret = init_addr_cam_info(adapter, info, fwcmd_tbl);
		if (ret)
			goto FWOFLD_END;
		// dword 0

		ret = h2c_pkt_set_hdr(adapter,
				      h2cb,
				      FWCMD_TYPE_H2C,
				      FWCMD_H2C_CAT_MAC,
				      FWCMD_H2C_CL_ADDR_CAM_UPDATE,
				      FWCMD_H2C_FUNC_ADDRCAM_INFO,
				      0,
				      1);
		if (ret)
			goto FWOFLD_END;

		// Return MACSUCCESS if h2c aggregation is enabled and enqueued successfully.
		// The H2C shall be sent by mac_h2c_agg_tx.
		ret = h2c_agg_enqueue(adapter, h2cb);
		if (ret == MACSUCCESS)
			return MACSUCCESS;

		ret = h2c_pkt_build_txd(adapter, h2cb);
		if (ret)
			goto FWOFLD_END;
		#if MAC_AX_PHL_H2C
		ret = PLTFM_TX(h2cb);
		#else
		ret = PLTFM_TX(h2cb->data, h2cb->len);
		#endif

FWOFLD_END:
		h2cb_free(adapter, h2cb);
		if (!ret)
			h2c_end_flow(adapter);

		return ret;
	}
	if (op == CHG)
		ret = change_addr_cam_info(adapter,
					   info,
					   (struct fwcmd_addrcam_info *)
					   tbl);
	else
		ret = init_addr_cam_info(adapter, info,
					 (struct fwcmd_addrcam_info *)
					 tbl);
	if (ret)
		return ret;
	// Indirect write addr cam
	for (i = 0; i < (u32)((info->a_info.len)) / 4; i++)
		mac_sram_dbg_write(adapter, (info->a_info.addr_cam_idx *
					     info->a_info.len) + (i * 4),
				   le32_to_cpu(tbl[i + 2]), ADDR_CAM_SEL);

	// Indirect write BSSID cam
	for (i = 0; i < (u32)((info->b_info.len)) / 4; i++)
		mac_sram_dbg_write(adapter, (info->b_info.bssid_cam_idx *
					     info->b_info.len) + (i * 4),
				   le32_to_cpu(tbl[i + 13]), BSSID_CAM_SEL);

	// Indirect write cmac table addr cam idx
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A))
		ctlinfo_aidx_off = 0x18;
	else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
		 is_chip_id(adapter, MAC_AX_CHIP_ID_8851B))
		ctlinfo_aidx_off = 0x18;
	else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
		 is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
		 is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
		 is_chip_id(adapter, MAC_AX_CHIP_ID_8852D))
		ctlinfo_aidx_off = 0x17;
	else
		ctlinfo_aidx_off = 0xFF;
	PLTFM_MSG_WARN("%s ind access cmac tbl start\n", __func__);
	PLTFM_MUTEX_LOCK(&adapter->hw_info->ind_access_lock);
	adapter->hw_info->ind_aces_cnt++;
	MAC_REG_W8(R_AX_INDIR_ACCESS_ENTRY +
		   info->macid * CCTL_INFO_SIZE + ctlinfo_aidx_off,
		   info->a_info.addr_cam_idx);
	adapter->hw_info->ind_aces_cnt--;
	PLTFM_MUTEX_UNLOCK(&adapter->hw_info->ind_access_lock);
	PLTFM_MSG_WARN("%s ind access cmac tbl end\n", __func__);

	return MACSUCCESS;
}

static void addr_cam_opt_2_uint(struct mac_ax_adapter *adapter,
				struct mac_ax_addrcam_ctrl_t *opt,
				u32 *val32)
{
	*val32 = ((opt->addrcam_en) ? B_AX_ADDR_CAM_EN : 0) |
		 ((opt->srch_per_mpdu) ? B_AX_ADDR_CAM_SRCH_PERPKT : 0) |
		 ((opt->a2_bit0_cmp_en) ? B_AX_ADDR_CAM_A2_B0_CHK : 0) |
		 //opt->clr_all_content) ? B_AX_ADDR_CAM_CLR: 0) |
		 SET_WORD(opt->srch_time_lmt, B_AX_ADDR_CAM_CMPLIMT) |
		 SET_WORD(opt->srch_range_lmt, B_AX_ADDR_CAM_RANGE);
}

static void addr_cam_dis_opt_2_uint(struct mac_ax_adapter *adapter,
				    struct mac_ax_addrcam_dis_ctrl_t *opt,
				    u32 *val32)
{
	*val32 = SET_WORD(opt->def_hit_idx, B_AX_ADDR_CAM_DIS_IDX) |
		 ((opt->def_hit_result) ? B_AX_ADDR_CAM_DIS_CAM_HIT : 0) |
		 ((opt->def_a1_hit_result) ? B_AX_ADDR_CAM_DIS_A1_HIT : 0) |
		 ((opt->def_a2_hit_result) ? B_AX_ADDR_CAM_DIS_A2_HIT : 0) |
		 ((opt->def_a3_hit_result) ? B_AX_ADDR_CAM_DIS_A3_HIT : 0) |
		 SET_WORD(opt->def_port, B_AX_ADDR_CAM_DIS_PORT) |
		 SET_WORD(opt->def_sec_idx, B_AX_ADDR_CAM_DIS_SEC_IDX) |
		 SET_WORD(opt->def_macid, B_AX_ADDR_CAM_DIS_MACID);
}

u32 mac_get_cfg_addr_cam(struct mac_ax_adapter *adapter,
			 struct mac_ax_addrcam_ctrl_t *opt,
			 enum mac_ax_band band)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32;

	if (!opt) {
		PLTFM_MSG_ERR("[ERR]%s opt is null\n", __func__);
		return MACNPTR;
	}

	val32 = check_mac_en(adapter, band, MAC_AX_CMAC_SEL);
	if (val32 != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]%s CMAC%d not enable\n", __func__, band);
		return val32;
	}

	val32 = MAC_REG_R32((band == MAC_AX_BAND_1) ?
		R_AX_ADDR_CAM_CTRL_C1 : R_AX_ADDR_CAM_CTRL);
	opt->addrcam_en = ((val32 & B_AX_ADDR_CAM_EN) != 0);
	opt->srch_per_mpdu = ((val32 & B_AX_ADDR_CAM_SRCH_PERPKT) != 0);
	opt->a2_bit0_cmp_en = ((val32 & B_AX_ADDR_CAM_A2_B0_CHK) != 0);
	opt->srch_time_lmt = GET_FIELD(val32, B_AX_ADDR_CAM_CMPLIMT);
	opt->srch_range_lmt = GET_FIELD(val32, B_AX_ADDR_CAM_RANGE);

	return MACSUCCESS;
}

u32 mac_get_cfg_addr_cam_dis(struct mac_ax_adapter *adapter,
			     struct mac_ax_addrcam_dis_ctrl_t *opt,
			     enum mac_ax_band band)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32;

	if (!opt) {
		PLTFM_MSG_ERR("[ERR]%s opt is null\n", __func__);
		return MACNPTR;
	}

	val32 = check_mac_en(adapter, band, MAC_AX_CMAC_SEL);
	if (val32 != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]%s CMAC%d not enable\n", __func__, band);
		return val32;
	}

	val32 = MAC_REG_R32((band == MAC_AX_BAND_1) ?
			     R_AX_ADDR_CAM_DIS_INFO_C1 :
			     R_AX_ADDR_CAM_DIS_INFO);

	opt->def_hit_idx = GET_FIELD(val32, B_AX_ADDR_CAM_DIS_IDX);
	opt->def_hit_result = ((val32 & B_AX_ADDR_CAM_DIS_CAM_HIT) != 0);
	opt->def_a1_hit_result = ((val32 & B_AX_ADDR_CAM_DIS_A1_HIT) != 0);
	opt->def_a2_hit_result = ((val32 & B_AX_ADDR_CAM_DIS_A2_HIT) != 0);
	opt->def_a3_hit_result = ((val32 & B_AX_ADDR_CAM_DIS_A3_HIT) != 0);
	opt->def_port = GET_FIELD(val32, B_AX_ADDR_CAM_DIS_PORT);
	opt->def_sec_idx = GET_FIELD(val32, B_AX_ADDR_CAM_DIS_SEC_IDX);
	opt->def_macid = GET_FIELD(val32, B_AX_ADDR_CAM_DIS_MACID);

	return MACSUCCESS;
}

u32 mac_cfg_addr_cam(struct mac_ax_adapter *adapter,
		     struct mac_ax_addrcam_ctrl_t *ctl_opt,
		     struct mac_ax_addrcam_ctrl_t *ctl_msk,
		     enum mac_ax_band band)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	struct mac_ax_addrcam_ctrl_t opt = {0};
	u32 ctl_opt_val;
	u32 ctl_msk_val;
	u32 opt_val;
	u32 reg;
	u32 cnt;
	u32 ret = MACSUCCESS;

	ret = check_mac_en(adapter, band, MAC_AX_CMAC_SEL);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]%s CMAC%d not enable\n", __func__, band);
		return ret;
	}

	reg = (band == MAC_AX_BAND_1) ?
	      R_AX_ADDR_CAM_CTRL_C1 : R_AX_ADDR_CAM_CTRL;

	mac_get_cfg_addr_cam(adapter, &opt, band);

	addr_cam_opt_2_uint(adapter, ctl_opt, &ctl_opt_val);
	addr_cam_opt_2_uint(adapter, ctl_msk, &ctl_msk_val);
	addr_cam_opt_2_uint(adapter, &opt, &opt_val);

	opt_val = (ctl_opt_val & ctl_msk_val) |
		  (~(~ctl_opt_val & ctl_msk_val) & opt_val);

	MAC_REG_W32(reg, opt_val);

	if (ctl_opt->clr_all_content & ctl_msk->clr_all_content) {
		opt_val |= B_AX_ADDR_CAM_CLR;
		MAC_REG_W32(reg, opt_val);
		cnt = TRXCFG_WAIT_CNT;
		while (cnt--) {
			if (!(MAC_REG_R16(reg) & B_AX_ADDR_CAM_CLR))
				break;
			PLTFM_DELAY_US(TRXCFG_WAIT_US);
		}
		if (!++cnt) {
			PLTFM_MSG_ERR("[ERR]ADDR_CAM reset\n");
			return MACPOLLTO;
		}
	}

	return MACSUCCESS;
}

u32 mac_cfg_addr_cam_dis(struct mac_ax_adapter *adapter,
			 struct mac_ax_addrcam_dis_ctrl_t *ctl_opt,
			 struct mac_ax_addrcam_dis_ctrl_t *ctl_msk,
			 enum mac_ax_band band)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	struct mac_ax_addrcam_dis_ctrl_t opt = {0};
	u32 ctl_opt_val;
	u32 ctl_msk_val;
	u32 opt_val;
	u32 reg;
	u32 ret = MACSUCCESS;

	ret = check_mac_en(adapter, band, MAC_AX_CMAC_SEL);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]%s CMAC%d not enable\n", __func__, band);
		return ret;
	}

	reg = (band == MAC_AX_BAND_1) ?
	      R_AX_ADDR_CAM_DIS_INFO_C1 : R_AX_ADDR_CAM_DIS_INFO;

	mac_get_cfg_addr_cam_dis(adapter, &opt, band);

	addr_cam_dis_opt_2_uint(adapter, ctl_opt, &ctl_opt_val);
	addr_cam_dis_opt_2_uint(adapter, ctl_msk, &ctl_msk_val);
	addr_cam_dis_opt_2_uint(adapter, &opt, &opt_val);

	opt_val = (ctl_opt_val & ctl_msk_val) |
		  (~(~ctl_opt_val & ctl_msk_val) & opt_val);

	MAC_REG_W32(reg, opt_val);

	return MACSUCCESS;
}

u32 addr_cam_init(struct mac_ax_adapter *adapter,
		  enum mac_ax_band band)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32, reg;
	u32 cnt = TRXCFG_WAIT_CNT;
	u32 ret;

	ret = check_mac_en(adapter, band, MAC_AX_CMAC_SEL);
	if (ret != MACSUCCESS)
		return ret;

	reg = band == MAC_AX_BAND_1 ?
		R_AX_ADDR_CAM_CTRL_C1 : R_AX_ADDR_CAM_CTRL;
	val32 = MAC_REG_R32(reg);
	val32 |= (SET_WORD(ADDR_CAM_SERCH_RANGE, B_AX_ADDR_CAM_RANGE) |
		  B_AX_ADDR_CAM_EN);
	if (band == MAC_AX_BAND_0)
		val32 |= B_AX_ADDR_CAM_CLR;
	MAC_REG_W32(reg, val32);

	cnt = TRXCFG_WAIT_CNT;
	while (cnt--) {
		if (!(MAC_REG_R16(band == MAC_AX_BAND_1 ?
		    R_AX_ADDR_CAM_CTRL_C1 : R_AX_ADDR_CAM_CTRL)
		    & B_AX_ADDR_CAM_CLR))
			break;
		PLTFM_DELAY_US(TRXCFG_WAIT_US);
	}
	if (!++cnt) {
		PLTFM_MSG_ERR("[ERR]ADDR_CAM reset\n");
		return MACPOLLTO;
	}
	return MACSUCCESS;
}

static u32 _set_mac_resp_ack(struct mac_ax_adapter *adapter, u32 ack, u8 band)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u8 val8;
	u32 val32;
	u32 offset_ctrl = band ? R_AX_ADDR_CAM_CTRL_C1 : R_AX_ADDR_CAM_CTRL;
	u32 offset_hit = band ? R_AX_ADDR_CAM_DIS_INFO_C1 :
		R_AX_ADDR_CAM_DIS_INFO;

	if (ack) {
		val8 = MAC_REG_R8(offset_ctrl);
		MAC_REG_W8(offset_ctrl, val8 | B_AX_ADDR_CAM_EN);
	} else {
		val8 = MAC_REG_R8(offset_ctrl);
		MAC_REG_W8(offset_ctrl, val8 & ~B_AX_ADDR_CAM_EN);
		val32 = MAC_REG_R32(offset_hit);
		val32 = SET_CLR_WORD(val32, MAC_AX_NO_HIT_IDX,
				     B_AX_ADDR_CAM_DIS_IDX);
		val32 &= ~(B_AX_ADDR_CAM_DIS_CAM_HIT |
			   B_AX_ADDR_CAM_DIS_A1_HIT);
		MAC_REG_W32(offset_hit, val32);
	}

	return MACSUCCESS;
}

#if MAC_AX_FW_REG_OFLD
u32 set_mac_do_resp_ack_ofld(struct mac_ax_adapter *adapter)
{
	u32 ret;
	u8 cmac1_en;

	cmac1_en = check_mac_en(adapter, 1, MAC_AX_CMAC_SEL) == MACSUCCESS ?
			1 : 0;

	ret = MAC_REG_W_OFLD(R_AX_ADDR_CAM_CTRL, B_AX_ADDR_CAM_EN,
			     1, cmac1_en ? 0 : 1);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("%s: config fail\n", __func__);
		return ret;
	}
	if (cmac1_en) {
		ret = MAC_REG_W_OFLD(R_AX_ADDR_CAM_CTRL_C1,
				     B_AX_ADDR_CAM_EN,
				     1, 1);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("%s: config fail\n", __func__);
			return ret;
		}
	}

	return MACSUCCESS;
}

u32 set_mac_not_resp_ack_ofld(struct mac_ax_adapter *adapter)
{
	u32 ret, msk;
	u8 cmac1_en;

	cmac1_en = check_mac_en(adapter, 1, MAC_AX_CMAC_SEL) == MACSUCCESS ?
			1 : 0;

	/* set 0xCE34[0] = 0 */
	ret = MAC_REG_W_OFLD(R_AX_ADDR_CAM_CTRL, B_AX_ADDR_CAM_EN,
			     0, 0);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("%s: config fail\n", __func__);
		return ret;
	}

	/* set 0xCE38[9:0] = 0xFF */
	msk = (B_AX_ADDR_CAM_DIS_IDX_MSK << B_AX_ADDR_CAM_DIS_IDX_SH) |
		B_AX_ADDR_CAM_DIS_A1_HIT |
		B_AX_ADDR_CAM_DIS_CAM_HIT;
	ret = MAC_REG_W_OFLD(R_AX_ADDR_CAM_DIS_INFO, msk,
			     MAC_AX_NO_HIT_IDX, cmac1_en ? 0 : 1);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("%s: config fail\n", __func__);
		return ret;
	}

	if (cmac1_en) {
		ret = MAC_REG_W_OFLD(R_AX_ADDR_CAM_CTRL_C1, B_AX_ADDR_CAM_EN,
				     0, 0);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("%s: config fail\n", __func__);
			return ret;
		}

		ret = MAC_REG_W_OFLD(R_AX_ADDR_CAM_DIS_INFO_C1, msk,
				     MAC_AX_NO_HIT_IDX, 1);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("%s: config fail\n", __func__);
			return ret;
		}
	}

	return MACSUCCESS;
}

u32 set_mac_resp_ack_ofld(struct mac_ax_adapter *adapter, u32 ack)
{
	u32 ret;

	if (ack)
		ret = set_mac_do_resp_ack_ofld(adapter);
	else
		ret = set_mac_not_resp_ack_ofld(adapter);

	return ret;
}
#endif

u32 set_mac_resp_ack(struct mac_ax_adapter *adapter, u32 *ack)
{
	u32 ret;
#if MAC_AX_FW_REG_OFLD
	if (adapter->sm.fwdl == MAC_AX_FWDL_INIT_RDY)
		return ret = set_mac_resp_ack_ofld(adapter, *ack);
#endif

	ret = _set_mac_resp_ack(adapter, *ack, 0);
	if (ret == MACSUCCESS &&
	    check_mac_en(adapter, 1, MAC_AX_CMAC_SEL) == MACSUCCESS)
		ret = _set_mac_resp_ack(adapter, *ack, 1);

	return ret;
}

u32 get_mac_resp_ack(struct mac_ax_adapter *adapter, u32 *ack)
{
#define MAC_AX_ACK_CMAC1_SH 1
#define MAC_AX_ACK_CMAC0_SH 0
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u8 cmac0 = 0, cmac1 = 0;

	cmac0 = !!(MAC_REG_R8(R_AX_ADDR_CAM_CTRL) & B_AX_ADDR_CAM_EN);
	if (check_mac_en(adapter, 1, MAC_AX_CMAC_SEL) == MACSUCCESS)
		cmac1 = !!(MAC_REG_R8(R_AX_ADDR_CAM_CTRL_C1) &
			   B_AX_ADDR_CAM_EN);

	*ack = cmac0 << MAC_AX_ACK_CMAC0_SH | cmac1 << MAC_AX_ACK_CMAC1_SH;

	return MACSUCCESS;
}

u8 get_addr_cam_size(struct mac_ax_adapter *adapter)
{
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B))
		return ADDR_CAM_ENT_LONG_SIZE;
	else
		return ADDR_CAM_ENT_SHORT_SIZE;
}

struct mac_ax_mc_table *
get_avalible_mc_entry(struct mac_ax_adapter *adapter,
		      struct mac_ax_multicast_info *info)
{
	struct mac_ax_mc_table *mc_entry = mc_role;
	u8 i;

	for (i = 0; i < MAC_AX_MAX_MC_ENTRY; i++) {
		if (!PLTFM_MEMCMP(&mc_entry->mc, info, sizeof(*info)) &&
		    mc_entry->valid == 1) {
			PLTFM_MSG_ERR("duplicated multicast info\n");
			return NULL;
		}

		if (mc_entry->valid == 0)
			return mc_entry;
		mc_entry++;
	}

	return NULL;
}

struct mac_ax_mc_table *
get_record_mc_entry(struct mac_ax_adapter *adapter,
		    struct mac_ax_multicast_info *info)
{
	struct mac_ax_mc_table *mc_entry = mc_role;
	u8 i;

	for (i = 0; i < MAC_AX_MAX_MC_ENTRY; i++) {
		if (!PLTFM_MEMCMP(&mc_entry->mc, info, sizeof(*info)) &&
		    mc_entry->valid == 1) {
			return mc_entry;
		}
		mc_entry++;
	}

	return NULL;
}

u8 get_avalible_mc_entry_macid(struct mac_ax_adapter *adapter)
{
	struct mac_ax_mc_table *mc_entry = mc_role;
	u8 i, macid = adapter->hw_info->macid_num - 1;

	for (i = 0; i < MAC_AX_MAX_MC_ENTRY; i++) {
		if (mc_entry->valid == 1 && mc_entry->role.macid <= macid)
			macid = mc_entry->role.macid - 1;
		mc_entry++;
	}

	return macid;
}

static u32 mac_add_multicast(struct mac_ax_adapter *adapter,
			     struct mac_ax_multicast_info *info)
{
	struct mac_ax_mc_table *mc_entry;
	struct mac_ax_role_info *role;
	u32 ret;

	mc_entry = get_avalible_mc_entry(adapter, info);
	if (!mc_entry) {
		PLTFM_MSG_ERR("%s: fails to get avalible mc\n", __func__);
		return MACNPTR;
	}

	PLTFM_MEMCPY(&mc_entry->mc, info, sizeof(mc_entry->mc));

	role = &mc_entry->role;
	PLTFM_MEMSET(role, 0, sizeof(*role));
	role->upd_mode = MAC_AX_ROLE_CREATE;
	role->opmode = MAC_AX_ROLE_DISCONN;
	role->macid = get_avalible_mc_entry_macid(adapter);
	role->mask_sel = MAC_AX_SMA_MSK;
	role->addr_mask = MAC_AX_MSK_NONE;
	PLTFM_MEMCPY(role->self_mac, info->mc_addr, 6);
	PLTFM_MEMCPY(role->target_mac, info->bssid, 6);
	PLTFM_MEMCPY(role->bssid, info->bssid, 6);
	role->is_mul_ent = 1;

	ret = mac_add_role(adapter, role);
	if (ret) {
		PLTFM_MSG_ERR("%s: add role fail(%d)\n", __func__, ret);
		return ret;
	}
	mc_entry->valid = 1;

	return MACSUCCESS;
}

static u32 mac_del_multicast(struct mac_ax_adapter *adapter,
			     struct mac_ax_multicast_info *info)
{
	struct mac_ax_mc_table *mc_entry;
	u32 ret;

	mc_entry = get_record_mc_entry(adapter, info);
	if (!mc_entry) {
		PLTFM_MSG_ERR("%s: fails to get record mc\n", __func__);
		return MACNPTR;
	}
	ret = mac_remove_role(adapter, mc_entry->role.macid);
	if (ret) {
		PLTFM_MSG_ERR("%s: remove role fail(%d)\n", __func__, ret);
		return ret;
	}
	mc_entry->valid = 0;

	return MACSUCCESS;
}

u32 mac_pre_proc_mc_info(struct mac_ax_multicast_info *info)
{
	u8 i;

	for (i = 0; i < 6; i++) {
		if (!(info->mc_msk & (1 << i)))
			info->mc_addr[i] = 0;
	}

	return 0;
}

u32 mac_cfg_multicast(struct mac_ax_adapter *adapter, u8 add,
		      struct mac_ax_multicast_info *info)
{
	u32 ret;

	mac_pre_proc_mc_info(info);

	if (add)
		ret = mac_add_multicast(adapter, info);
	else
		ret = mac_del_multicast(adapter, info);

	return ret;
}

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

#include "tblupd_8852c.h"
#if MAC_AX_8852C_SUPPORT

u32 mac_f2p_test_cmd_8852c(struct mac_ax_adapter *adapter,
			   struct mac_ax_f2p_test_para_v1 *info,
			   struct mac_ax_f2p_wd *f2pwd,
			   struct mac_ax_f2p_tx_cmd_v1 *ptxcmd,
			   u8 *psigb_addr)
{
	u32 ret = 0;
	u8 *buf;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	struct fwcmd_test_para_v1 *tbl;
	struct mac_ax_tf_user_para *user;
	struct mac_ax_tf_depend_user_para *dep_user;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_LONG_DATA);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, sizeof(struct fwcmd_test_para_v1));
	if (!buf) {
		ret = MACNOBUF;
		goto fail;
	}

	PLTFM_MEMSET(buf, 0, sizeof(struct fwcmd_test_para_v1));

	tbl = (struct fwcmd_test_para_v1 *)buf;

	tbl->dword0 =
	cpu_to_le32(SET_WORD(info->tf_pkt.ul_bw, FWCMD_F2PTEST_ULBW) |
		    SET_WORD(info->tf_pkt.gi_ltf, FWCMD_F2PTEST_GILTF) |
		    SET_WORD(info->tf_pkt.num_he_ltf, FWCMD_F2PTEST_NUMLTF) |
		    SET_WORD(info->tf_pkt.ul_stbc, FWCMD_F2PTEST_ULSTBC) |
		    SET_WORD(info->tf_pkt.doppler, FWCMD_F2PTEST_DPLR) |
		    SET_WORD(info->tf_pkt.ap_tx_power, FWCMD_F2PTEST_TXPWR) |
		    SET_WORD(info->tf_pkt.user_num, FWCMD_F2PTEST_USERNUM) |
		    SET_WORD(info->tf_pkt.pktnum, FWCMD_F2PTEST_PKTNUM) |
		    SET_WORD(info->tf_pkt.pri20_bitmap, FWCMD_F2PTEST_BITMAP));

	user = &info->tf_pkt.user[0];
	tbl->dword1 =
	cpu_to_le32(SET_WORD(user->aid12, FWCMD_F2PTEST_AID12) |
		    SET_WORD(user->ul_mcs, FWCMD_F2PTEST_ULMCS) |
		    SET_WORD(user->macid, FWCMD_F2PTEST_MACID) |
		    SET_WORD(user->ru_pos, FWCMD_F2PTEST_RUPOS));

	tbl->dword2 =
	cpu_to_le32(SET_WORD(user->ul_fec_code, FWCMD_F2PTEST_ULFEC) |
		    SET_WORD(user->ul_dcm, FWCMD_F2PTEST_ULDCM) |
		    SET_WORD(user->ss_alloc, FWCMD_F2PTEST_SS_ALLOC) |
		    SET_WORD(user->ul_tgt_rssi, FWCMD_F2PTEST_UL_TGTRSSI));

	user = &info->tf_pkt.user[1];
	tbl->dword3 =
	cpu_to_le32(SET_WORD(user->aid12, FWCMD_F2PTEST_AID12) |
		    SET_WORD(user->ul_mcs, FWCMD_F2PTEST_ULMCS) |
		    SET_WORD(user->macid, FWCMD_F2PTEST_MACID) |
		    SET_WORD(user->ru_pos, FWCMD_F2PTEST_RUPOS));

	tbl->dword4 =
	cpu_to_le32(SET_WORD(user->ul_fec_code, FWCMD_F2PTEST_ULFEC) |
		    SET_WORD(user->ul_dcm, FWCMD_F2PTEST_ULDCM) |
		    SET_WORD(user->ss_alloc, FWCMD_F2PTEST_SS_ALLOC) |
		    SET_WORD(user->ul_tgt_rssi, FWCMD_F2PTEST_UL_TGTRSSI));

	user = &info->tf_pkt.user[2];
	tbl->dword5 =
	cpu_to_le32(SET_WORD(user->aid12, FWCMD_F2PTEST_AID12) |
		    SET_WORD(user->ul_mcs, FWCMD_F2PTEST_ULMCS) |
		    SET_WORD(user->macid, FWCMD_F2PTEST_MACID) |
		    SET_WORD(user->ru_pos, FWCMD_F2PTEST_RUPOS));

	tbl->dword6 =
	cpu_to_le32(SET_WORD(user->ul_fec_code, FWCMD_F2PTEST_ULFEC) |
		    SET_WORD(user->ul_dcm, FWCMD_F2PTEST_ULDCM) |
		    SET_WORD(user->ss_alloc, FWCMD_F2PTEST_SS_ALLOC) |
		    SET_WORD(user->ul_tgt_rssi, FWCMD_F2PTEST_UL_TGTRSSI));

	user = &info->tf_pkt.user[3];
	tbl->dword7 =
	cpu_to_le32(SET_WORD(user->aid12, FWCMD_F2PTEST_AID12) |
		    SET_WORD(user->ul_mcs, FWCMD_F2PTEST_ULMCS) |
		    SET_WORD(user->macid, FWCMD_F2PTEST_MACID) |
		    SET_WORD(user->ru_pos, FWCMD_F2PTEST_RUPOS));

	tbl->dword8 =
	cpu_to_le32(SET_WORD(user->ul_fec_code, FWCMD_F2PTEST_ULFEC) |
		    SET_WORD(user->ul_dcm, FWCMD_F2PTEST_ULDCM) |
		    SET_WORD(user->ss_alloc, FWCMD_F2PTEST_SS_ALLOC) |
		    SET_WORD(user->ul_tgt_rssi, FWCMD_F2PTEST_UL_TGTRSSI));

	user = &info->tf_pkt.user[4];
	tbl->dword9 =
	cpu_to_le32(SET_WORD(user->aid12, FWCMD_F2PTEST_AID12) |
		    SET_WORD(user->ul_mcs, FWCMD_F2PTEST_ULMCS) |
		    SET_WORD(user->macid, FWCMD_F2PTEST_MACID) |
		    SET_WORD(user->ru_pos, FWCMD_F2PTEST_RUPOS));

	tbl->dword10 =
	cpu_to_le32(SET_WORD(user->ul_fec_code, FWCMD_F2PTEST_ULFEC) |
		    SET_WORD(user->ul_dcm, FWCMD_F2PTEST_ULDCM) |
		    SET_WORD(user->ss_alloc, FWCMD_F2PTEST_SS_ALLOC) |
		    SET_WORD(user->ul_tgt_rssi, FWCMD_F2PTEST_UL_TGTRSSI));

	user = &info->tf_pkt.user[5];
	tbl->dword11 =
	cpu_to_le32(SET_WORD(user->aid12, FWCMD_F2PTEST_AID12) |
		    SET_WORD(user->ul_mcs, FWCMD_F2PTEST_ULMCS) |
		    SET_WORD(user->macid, FWCMD_F2PTEST_MACID) |
		    SET_WORD(user->ru_pos, FWCMD_F2PTEST_RUPOS));

	tbl->dword12 =
	cpu_to_le32(SET_WORD(user->ul_fec_code, FWCMD_F2PTEST_ULFEC) |
		    SET_WORD(user->ul_dcm, FWCMD_F2PTEST_ULDCM) |
		    SET_WORD(user->ss_alloc, FWCMD_F2PTEST_SS_ALLOC) |
		    SET_WORD(user->ul_tgt_rssi, FWCMD_F2PTEST_UL_TGTRSSI));

	user = &info->tf_pkt.user[6];
	tbl->dword13 =
	cpu_to_le32(SET_WORD(user->aid12, FWCMD_F2PTEST_AID12) |
		    SET_WORD(user->ul_mcs, FWCMD_F2PTEST_ULMCS) |
		    SET_WORD(user->macid, FWCMD_F2PTEST_MACID) |
		    SET_WORD(user->ru_pos, FWCMD_F2PTEST_RUPOS));

	tbl->dword14 =
	cpu_to_le32(SET_WORD(user->ul_fec_code, FWCMD_F2PTEST_ULFEC) |
		    SET_WORD(user->ul_dcm, FWCMD_F2PTEST_ULDCM) |
		    SET_WORD(user->ss_alloc, FWCMD_F2PTEST_SS_ALLOC) |
		    SET_WORD(user->ul_tgt_rssi, FWCMD_F2PTEST_UL_TGTRSSI));

	user = &info->tf_pkt.user[7];
	tbl->dword15 =
	cpu_to_le32(SET_WORD(user->aid12, FWCMD_F2PTEST_AID12) |
		    SET_WORD(user->ul_mcs, FWCMD_F2PTEST_ULMCS) |
		    SET_WORD(user->macid, FWCMD_F2PTEST_MACID) |
		    SET_WORD(user->ru_pos, FWCMD_F2PTEST_RUPOS));

	tbl->dword16 =
	cpu_to_le32(SET_WORD(user->ul_fec_code, FWCMD_F2PTEST_ULFEC) |
		    SET_WORD(user->ul_dcm, FWCMD_F2PTEST_ULDCM) |
		    SET_WORD(user->ss_alloc, FWCMD_F2PTEST_SS_ALLOC) |
		    SET_WORD(user->ul_tgt_rssi, FWCMD_F2PTEST_UL_TGTRSSI));

	dep_user = &info->tf_pkt.dep_user[0];
	tbl->dword17_0 =
	cpu_to_le32(SET_WORD(dep_user->pref_AC, FWCMD_F2PTEST_PREF_AC));

	dep_user = &info->tf_pkt.dep_user[1];
	tbl->dword17_1 =
	cpu_to_le32(SET_WORD(dep_user->pref_AC, FWCMD_F2PTEST_PREF_AC));

	dep_user = &info->tf_pkt.dep_user[2];
	tbl->dword17_2 =
	cpu_to_le32(SET_WORD(dep_user->pref_AC, FWCMD_F2PTEST_PREF_AC));

	dep_user = &info->tf_pkt.dep_user[3];
	tbl->dword17_3 =
	cpu_to_le32(SET_WORD(dep_user->pref_AC, FWCMD_F2PTEST_PREF_AC));

	dep_user = &info->tf_pkt.dep_user[4];
	tbl->dword18_0 =
	cpu_to_le32(SET_WORD(dep_user->pref_AC, FWCMD_F2PTEST_PREF_AC));

	dep_user = &info->tf_pkt.dep_user[5];
	tbl->dword18_1 =
	cpu_to_le32(SET_WORD(dep_user->pref_AC, FWCMD_F2PTEST_PREF_AC));

	dep_user = &info->tf_pkt.dep_user[6];
	tbl->dword18_2 =
	cpu_to_le32(SET_WORD(dep_user->pref_AC, FWCMD_F2PTEST_PREF_AC));

	dep_user = &info->tf_pkt.dep_user[7];
	tbl->dword18_3 =
	cpu_to_le32(SET_WORD(dep_user->pref_AC, FWCMD_F2PTEST_PREF_AC));

	tbl->dword19 =
	cpu_to_le32(SET_WORD(info->tf_wd.datarate, FWCMD_F2PTEST_DATARATE) |
		    SET_WORD(info->tf_wd.mulport_id, FWCMD_F2PTEST_MULPORT) |
		    SET_WORD(info->tf_wd.pwr_ofset, FWCMD_F2PTEST_PWR_OFSET) |
		    SET_WORD(info->mode, FWCMD_F2PTEST_MODE) |
		    SET_WORD(info->frexch_type, FWCMD_F2PTEST_TYPE) |
		    SET_WORD(info->sigb_len, FWCMD_F2PTEST_SIGB_LEN));

	PLTFM_MEMCPY(&tbl->dword20, (u8 *)f2pwd, sizeof(struct mac_ax_f2p_wd));
	PLTFM_MEMCPY(&tbl->dword22, (u8 *)ptxcmd, sizeof(struct mac_ax_f2p_tx_cmd_v1));

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C,
			      FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_FR_EXCHG,
			      FWCMD_H2C_FUNC_F2P_TEST,
			      0,
			      0);
	if (ret)
		goto fail;

	ret = h2c_pkt_build_txd(adapter, h2cb);
	if (ret)
		goto fail;

	#if MAC_AX_PHL_H2C
	ret = PLTFM_TX(h2cb);
	#else
	ret = PLTFM_TX(h2cb->data, h2cb->len);
	#endif
	if (ret)
		goto fail;

	h2cb_free(adapter, h2cb);

	return MACSUCCESS;
fail:
	h2cb_free(adapter, h2cb);

	return ret;
}

u32 mac_upd_dctl_info_8852c(struct mac_ax_adapter *adapter,
			    struct mac_ax_dctl_info *info,
			    struct mac_ax_dctl_info *mask,
			    u8 macid, u8 operation)
{
	u32 ret = 0;
	u8 *buf;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	struct fwcmd_dctlinfo_ud_v1 *tbl;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_DATA);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, sizeof(struct fwcmd_dctlinfo_ud_v1));
	if (!buf) {
		ret = MACNOBUF;
		goto fail;
	}

	tbl = (struct fwcmd_dctlinfo_ud_v1 *)buf;
	tbl->dword0 =
	cpu_to_le32(SET_WORD(macid, FWCMD_H2C_DCTLINFO_UD_V1_MACID) |
		    (operation ? FWCMD_H2C_DCTLINFO_UD_V1_OP : 0));

	tbl->dword1 =
	cpu_to_le32(SET_WORD(info->qos_field_h, FWCMD_H2C_DCTRL_QOS_FIELD_H) |
		    SET_WORD(info->hw_exseq_macid, FWCMD_H2C_DCTRL_HW_EXSEQ_MACID) |
		    (info->qos_field_h_en ? FWCMD_H2C_DCTRL_V1_QOS_DATA : 0) | // qos_data
		    SET_WORD(info->aes_iv_l, FWCMD_H2C_DCTRL_AES_IV_L));

	tbl->dword2 =
	cpu_to_le32(SET_WORD(info->aes_iv_h, FWCMD_H2C_DCTRL_AES_IV_H));

	tbl->dword3 =
	cpu_to_le32(SET_WORD(info->seq0, FWCMD_H2C_DCTRL_SEQ0) |
		    SET_WORD(info->seq1, FWCMD_H2C_DCTRL_SEQ1) |
		    SET_WORD(info->amsdu_max_length, FWCMD_H2C_DCTRL_AMSDU_MAX_LEN) |
		    (info->sta_amsdu_en ? FWCMD_H2C_DCTRL_STA_AMSDU_EN : 0) |
		    (info->chksum_offload_en ? FWCMD_H2C_DCTRL_CHKSUM_OFLD_EN : 0) |
		    (info->with_llc ? FWCMD_H2C_DCTRL_WITH_LLC : 0));

	tbl->dword4 =
	cpu_to_le32(SET_WORD(info->seq2, FWCMD_H2C_DCTRL_SEQ2) |
		    SET_WORD(info->seq3, FWCMD_H2C_DCTRL_SEQ3) |
		    SET_WORD(info->tgt_ind, FWCMD_H2C_DCTRL_V1_TGT_IND) |
		    (info->tgt_ind_en ? FWCMD_H2C_DCTRL_V1_TGT_IND_EN : 0) |
		    SET_WORD(info->htc_lb, FWCMD_H2C_DCTRL_V1_HTC_LB));

	tbl->dword5 =
	cpu_to_le32(SET_WORD(info->mhdr_len, FWCMD_H2C_DCTRL_V1_MHDR_LEN) |
		    (info->vlan_tag_valid ? FWCMD_H2C_DCTRL_V1_VLAN_TAG_VALID : 0) |
		    SET_WORD(info->vlan_tag_sel, FWCMD_H2C_DCTRL_V1_VLAN_TAG_SEL) |
		    (info->htc_order ? FWCMD_H2C_DCTRL_V1_HTC_ORDER : 0) |
		    SET_WORD(info->sec_key_id, FWCMD_H2C_DCTRL_V1_SEC_KEY_ID) |
		    (info->wapi ? FWCMD_H2C_DCTRL_V1_WAPI : 0) |
		    SET_WORD(info->sec_ent_mode,
			     FWCMD_H2C_DCTRL_V1_SEC_ENT_MODE) |
		    SET_WORD(info->sec_ent0_keyid,
			     FWCMD_H2C_DCTRL_V1_SEC_ENT0_KEYID) |
		    SET_WORD(info->sec_ent1_keyid,
			     FWCMD_H2C_DCTRL_V1_SEC_ENT1_KEYID) |
		    SET_WORD(info->sec_ent2_keyid,
			     FWCMD_H2C_DCTRL_V1_SEC_ENT2_KEYID) |
		    SET_WORD(info->sec_ent3_keyid,
			     FWCMD_H2C_DCTRL_V1_SEC_ENT3_KEYID) |
		    SET_WORD(info->sec_ent4_keyid,
			     FWCMD_H2C_DCTRL_V1_SEC_ENT4_KEYID) |
		    SET_WORD(info->sec_ent5_keyid,
			     FWCMD_H2C_DCTRL_V1_SEC_ENT5_KEYID) |
		    SET_WORD(info->sec_ent6_keyid,
			     FWCMD_H2C_DCTRL_V1_SEC_ENT6_KEYID));

	tbl->dword6 =
	cpu_to_le32(SET_WORD(info->sec_ent_valid,
			     FWCMD_H2C_DCTRL_V1_SEC_ENT_VALID) |
		    SET_WORD(info->sec_ent0, FWCMD_H2C_DCTRL_V1_SEC_ENT0) |
		    SET_WORD(info->sec_ent1, FWCMD_H2C_DCTRL_V1_SEC_ENT1) |
		    SET_WORD(info->sec_ent2, FWCMD_H2C_DCTRL_V1_SEC_ENT2));

	tbl->dword7 =
	cpu_to_le32(SET_WORD(info->sec_ent3, FWCMD_H2C_DCTRL_V1_SEC_ENT3) |
		    SET_WORD(info->sec_ent4, FWCMD_H2C_DCTRL_V1_SEC_ENT4) |
		    SET_WORD(info->sec_ent5, FWCMD_H2C_DCTRL_V1_SEC_ENT5) |
		    SET_WORD(info->sec_ent6, FWCMD_H2C_DCTRL_V1_SEC_ENT6));

	tbl->dword9 =
	cpu_to_le32(SET_WORD(mask->qos_field_h, FWCMD_H2C_DCTRL_QOS_FIELD_H) |
		    SET_WORD(mask->hw_exseq_macid, FWCMD_H2C_DCTRL_HW_EXSEQ_MACID) |
		    (mask->qos_field_h_en ? FWCMD_H2C_DCTRL_V1_QOS_DATA : 0) | // qos_data
		    SET_WORD(mask->aes_iv_l, FWCMD_H2C_DCTRL_AES_IV_L));

	tbl->dword10 =
	cpu_to_le32(SET_WORD(mask->aes_iv_h, FWCMD_H2C_DCTRL_AES_IV_H));

	tbl->dword11 =
	cpu_to_le32(SET_WORD(mask->seq0, FWCMD_H2C_DCTRL_SEQ0) |
		    SET_WORD(mask->seq1, FWCMD_H2C_DCTRL_SEQ1) |
		    SET_WORD(mask->amsdu_max_length, FWCMD_H2C_DCTRL_AMSDU_MAX_LEN) |
		    (mask->sta_amsdu_en ? FWCMD_H2C_DCTRL_STA_AMSDU_EN : 0) |
		    (mask->chksum_offload_en ? FWCMD_H2C_DCTRL_CHKSUM_OFLD_EN : 0) |
		    (mask->with_llc ? FWCMD_H2C_DCTRL_WITH_LLC : 0));

	tbl->dword12 =
	cpu_to_le32(SET_WORD(mask->seq2, FWCMD_H2C_DCTRL_SEQ2) |
		    SET_WORD(mask->seq3, FWCMD_H2C_DCTRL_SEQ3) |
		    SET_WORD(mask->tgt_ind, FWCMD_H2C_DCTRL_V1_TGT_IND) |
		    (mask->tgt_ind_en ? FWCMD_H2C_DCTRL_V1_TGT_IND_EN : 0) |
		    SET_WORD(mask->htc_lb, FWCMD_H2C_DCTRL_V1_HTC_LB));

	tbl->dword13 =
	cpu_to_le32(SET_WORD(mask->mhdr_len, FWCMD_H2C_DCTRL_V1_MHDR_LEN) |
		    (mask->vlan_tag_valid ? FWCMD_H2C_DCTRL_V1_VLAN_TAG_VALID : 0) |
		    SET_WORD(mask->vlan_tag_sel, FWCMD_H2C_DCTRL_V1_VLAN_TAG_SEL) |
		    (mask->htc_order ? FWCMD_H2C_DCTRL_V1_HTC_ORDER : 0) |
		    SET_WORD(mask->sec_key_id, FWCMD_H2C_DCTRL_V1_SEC_KEY_ID) |
		    (mask->wapi ? FWCMD_H2C_DCTRL_V1_WAPI : 0) |
		    SET_WORD(mask->sec_ent_mode,
			     FWCMD_H2C_DCTRL_V1_SEC_ENT_MODE) |
		    SET_WORD(mask->sec_ent0_keyid,
			     FWCMD_H2C_DCTRL_V1_SEC_ENT0_KEYID) |
		    SET_WORD(mask->sec_ent1_keyid,
			     FWCMD_H2C_DCTRL_V1_SEC_ENT1_KEYID) |
		    SET_WORD(mask->sec_ent2_keyid,
			     FWCMD_H2C_DCTRL_V1_SEC_ENT2_KEYID) |
		    SET_WORD(mask->sec_ent3_keyid,
			     FWCMD_H2C_DCTRL_V1_SEC_ENT3_KEYID) |
		    SET_WORD(mask->sec_ent4_keyid,
			     FWCMD_H2C_DCTRL_V1_SEC_ENT4_KEYID) |
		    SET_WORD(mask->sec_ent5_keyid,
			     FWCMD_H2C_DCTRL_V1_SEC_ENT5_KEYID) |
		    SET_WORD(mask->sec_ent6_keyid,
			     FWCMD_H2C_DCTRL_V1_SEC_ENT6_KEYID));

	tbl->dword14 =
	cpu_to_le32(SET_WORD(mask->sec_ent_valid,
			     FWCMD_H2C_DCTRL_V1_SEC_ENT_VALID) |
		    SET_WORD(mask->sec_ent0, FWCMD_H2C_DCTRL_V1_SEC_ENT0) |
		    SET_WORD(mask->sec_ent1, FWCMD_H2C_DCTRL_V1_SEC_ENT1) |
		    SET_WORD(mask->sec_ent2, FWCMD_H2C_DCTRL_V1_SEC_ENT2));

	tbl->dword15 =
	cpu_to_le32(SET_WORD(mask->sec_ent3, FWCMD_H2C_DCTRL_V1_SEC_ENT3) |
		    SET_WORD(mask->sec_ent4, FWCMD_H2C_DCTRL_V1_SEC_ENT4) |
		    SET_WORD(mask->sec_ent5, FWCMD_H2C_DCTRL_V1_SEC_ENT5) |
		    SET_WORD(mask->sec_ent6, FWCMD_H2C_DCTRL_V1_SEC_ENT6));

	if (adapter->sm.fwdl == MAC_AX_FWDL_INIT_RDY) {
		ret = h2c_pkt_set_hdr(adapter, h2cb,
				      FWCMD_TYPE_H2C,
				      FWCMD_H2C_CAT_MAC,
				      FWCMD_H2C_CL_FR_EXCHG,
				      FWCMD_H2C_FUNC_DCTLINFO_UD_V1,
				      0,
				      1);
		if (ret)
			goto fail;

		ret = h2c_agg_enqueue(adapter, h2cb);
		if (!ret)
			return MACSUCCESS;

		ret = h2c_pkt_build_txd(adapter, h2cb);
		if (ret)
			goto fail;

		#if MAC_AX_PHL_H2C
		ret = PLTFM_TX(h2cb);
		#else
		ret = PLTFM_TX(h2cb->data, h2cb->len);
		#endif
		if (ret)
			goto fail;
	} else {
#if MAC_AX_FEATURE_DBGPKG
		if (operation) {
			dctl_info_debug_write_8852c
			(adapter, (struct fwcmd_dctlinfo_ud_v1 *)buf, macid);
		}
#else
		return MACFWNONRDY;
#endif
	}
	h2cb_free(adapter, h2cb);

	return MACSUCCESS;
fail:
	h2cb_free(adapter, h2cb);

	return ret;
}

void _set_role_cctrl_v1_8852c(struct mac_ax_adapter *adapter,
			      struct rtw_hal_mac_ax_cctl_info *info,
			      struct rtw_hal_mac_ax_cctl_info *mask,
			      struct rtw_hal_mac_ax_cctl_info *cctrl)
{
	cctrl->datarate = (cctrl->datarate & ~mask->datarate) |
			(mask->datarate & info->datarate);
	cctrl->force_txop = (cctrl->force_txop & ~mask->force_txop) |
			(mask->force_txop & info->force_txop);
	cctrl->data_bw = (cctrl->data_bw & ~mask->data_bw) |
			(mask->data_bw & info->data_bw);
	cctrl->data_gi_ltf = (cctrl->data_gi_ltf & ~mask->data_gi_ltf) |
			(mask->data_gi_ltf & info->data_gi_ltf);
	cctrl->darf_tc_index = (cctrl->darf_tc_index & ~mask->darf_tc_index) |
			(mask->darf_tc_index & info->darf_tc_index);
	cctrl->arfr_ctrl = (cctrl->arfr_ctrl & ~mask->arfr_ctrl) |
			(mask->arfr_ctrl & info->arfr_ctrl);
	cctrl->acq_rpt_en = (cctrl->acq_rpt_en & ~mask->acq_rpt_en) |
			(mask->acq_rpt_en & info->acq_rpt_en);
	cctrl->mgq_rpt_en = (cctrl->mgq_rpt_en & ~mask->mgq_rpt_en) |
			(mask->mgq_rpt_en & info->mgq_rpt_en);
	cctrl->ulq_rpt_en = (cctrl->ulq_rpt_en & ~mask->ulq_rpt_en) |
			(mask->ulq_rpt_en & info->ulq_rpt_en);
	cctrl->twtq_rpt_en = (cctrl->twtq_rpt_en & ~mask->twtq_rpt_en) |
			(mask->twtq_rpt_en & info->twtq_rpt_en);
	cctrl->disrtsfb = (cctrl->disrtsfb & ~mask->disrtsfb) |
			(mask->disrtsfb & info->disrtsfb);
	cctrl->disdatafb = (cctrl->disdatafb & ~mask->disdatafb) |
			(mask->disdatafb & info->disdatafb);
	cctrl->tryrate = (cctrl->tryrate & ~mask->tryrate) |
			(mask->tryrate & info->tryrate);
	cctrl->ampdu_density = (cctrl->ampdu_density & ~mask->ampdu_density) |
			(mask->ampdu_density & info->ampdu_density);

	cctrl->data_rty_lowest_rate =
			(cctrl->data_rty_lowest_rate &
			 ~mask->data_rty_lowest_rate) |
			(mask->data_rty_lowest_rate &
			 info->data_rty_lowest_rate);
	cctrl->ampdu_time_sel = (cctrl->ampdu_time_sel &
				 ~mask->ampdu_time_sel) |
			(mask->ampdu_time_sel & info->ampdu_time_sel);
	cctrl->ampdu_len_sel = (cctrl->ampdu_len_sel & ~mask->ampdu_len_sel) |
			(mask->ampdu_len_sel & info->ampdu_len_sel);
	cctrl->rts_txcnt_lmt_sel =
			(cctrl->rts_txcnt_lmt_sel & ~mask->rts_txcnt_lmt_sel) |
			(mask->rts_txcnt_lmt_sel & info->rts_txcnt_lmt_sel);
	cctrl->rts_txcnt_lmt = (cctrl->rts_txcnt_lmt & ~mask->rts_txcnt_lmt) |
			(mask->rts_txcnt_lmt & info->rts_txcnt_lmt);
	cctrl->rtsrate = (cctrl->rtsrate & ~mask->rtsrate) |
			(mask->rtsrate & info->rtsrate);
	cctrl->vcs_stbc = (cctrl->vcs_stbc & ~mask->vcs_stbc) |
			(mask->vcs_stbc & info->vcs_stbc);
	cctrl->rts_rty_lowest_rate =
			(cctrl->rts_rty_lowest_rate &
			 ~mask->rts_rty_lowest_rate) |
			(mask->rts_rty_lowest_rate & info->rts_rty_lowest_rate);

	cctrl->data_tx_cnt_lmt =
			(cctrl->data_tx_cnt_lmt & ~mask->data_tx_cnt_lmt) |
			(mask->data_tx_cnt_lmt & info->data_tx_cnt_lmt);
	cctrl->data_txcnt_lmt_sel =
			(cctrl->data_txcnt_lmt_sel &
			 ~mask->data_txcnt_lmt_sel) |
			(mask->data_txcnt_lmt_sel & info->data_txcnt_lmt_sel);
	cctrl->max_agg_num_sel = (cctrl->max_agg_num_sel &
				  ~mask->max_agg_num_sel) |
			(mask->max_agg_num_sel & info->max_agg_num_sel);
	cctrl->rts_en = (cctrl->rts_en & ~mask->rts_en) |
			(mask->rts_en & info->rts_en);
	cctrl->cts2self_en = (cctrl->cts2self_en & ~mask->cts2self_en) |
			(mask->cts2self_en & info->cts2self_en);
	cctrl->cca_rts = (cctrl->cca_rts & ~mask->cca_rts) |
			(mask->cca_rts & info->cca_rts);
	cctrl->hw_rts_en = (cctrl->hw_rts_en & ~mask->hw_rts_en) |
			(mask->hw_rts_en & info->hw_rts_en);
	cctrl->rts_drop_data_mode =
			(cctrl->rts_drop_data_mode &
			 ~mask->rts_drop_data_mode) |
			(mask->rts_drop_data_mode & info->rts_drop_data_mode);
	cctrl->preld_en = (cctrl->preld_en & ~mask->preld_en) |
			(mask->preld_en & info->preld_en);
	cctrl->ampdu_max_len = (cctrl->ampdu_max_len & ~mask->ampdu_max_len) |
			(mask->ampdu_max_len & info->ampdu_max_len);
	cctrl->ul_mu_dis = (cctrl->ul_mu_dis & ~mask->ul_mu_dis) |
			(mask->ul_mu_dis & info->ul_mu_dis);
	cctrl->ampdu_max_time = (cctrl->ampdu_max_time &
				 ~mask->ampdu_max_time) |
			(mask->ampdu_max_time & info->ampdu_max_time);

	cctrl->max_agg_num = (cctrl->max_agg_num & ~mask->max_agg_num) |
			(mask->max_agg_num & info->max_agg_num);
	cctrl->ba_bmap = (cctrl->ba_bmap & ~mask->ba_bmap) |
			(mask->ba_bmap & info->ba_bmap);
	cctrl->vo_lftime_sel = (cctrl->vo_lftime_sel & ~mask->vo_lftime_sel) |
			(mask->vo_lftime_sel & info->vo_lftime_sel);
	cctrl->vi_lftime_sel = (cctrl->vi_lftime_sel & ~mask->vi_lftime_sel) |
			(mask->vi_lftime_sel & info->vi_lftime_sel);
	cctrl->be_lftime_sel = (cctrl->be_lftime_sel & ~mask->be_lftime_sel) |
			(mask->be_lftime_sel & info->be_lftime_sel);
	cctrl->bk_lftime_sel = (cctrl->bk_lftime_sel & ~mask->bk_lftime_sel) |
			(mask->bk_lftime_sel & info->bk_lftime_sel);

	cctrl->multi_port_id = (cctrl->multi_port_id & ~mask->multi_port_id) |
			(mask->multi_port_id & info->multi_port_id);
	cctrl->bmc = (cctrl->multi_port_id & ~mask->bmc) |
			(mask->multi_port_id & info->bmc);
	cctrl->mbssid = (cctrl->mbssid & ~mask->mbssid) |
			(mask->mbssid & info->mbssid);
	cctrl->data_dcm = (cctrl->data_dcm & ~mask->data_dcm) |
			(mask->data_dcm & info->data_dcm);
	cctrl->data_er = (cctrl->data_er & ~mask->data_er) |
			(mask->data_er & info->data_er);
	cctrl->data_ldpc = (cctrl->data_ldpc & ~mask->data_ldpc) |
			(mask->data_ldpc & info->data_ldpc);
	cctrl->data_stbc = (cctrl->data_stbc & ~mask->data_stbc) |
			(mask->data_stbc & info->data_stbc);
	cctrl->a_ctrl_bqr = (cctrl->a_ctrl_bqr & ~mask->a_ctrl_bqr) |
			(mask->a_ctrl_bqr & info->a_ctrl_bqr);
	cctrl->a_ctrl_uph = (cctrl->a_ctrl_uph & ~mask->a_ctrl_uph) |
			(mask->a_ctrl_uph & info->a_ctrl_uph);
	cctrl->a_ctrl_bsr = (cctrl->a_ctrl_bsr & ~mask->a_ctrl_bsr) |
			(mask->a_ctrl_bsr & info->a_ctrl_bsr);
	cctrl->a_ctrl_cas = (cctrl->a_ctrl_cas & ~mask->a_ctrl_cas) |
			(mask->a_ctrl_cas & info->a_ctrl_cas);
	cctrl->data_bw_er = (cctrl->data_bw_er & ~mask->data_bw_er) |
			(mask->data_bw_er & info->data_bw_er);
	cctrl->lsig_txop_en = (cctrl->lsig_txop_en & ~mask->lsig_txop_en) |
			(mask->lsig_txop_en & info->lsig_txop_en);
	cctrl->ctrl_cnt_vld = (cctrl->ctrl_cnt_vld & ~mask->ctrl_cnt_vld) |
			(mask->ctrl_cnt_vld & info->ctrl_cnt_vld);
	cctrl->ctrl_cnt = (cctrl->ctrl_cnt & ~mask->ctrl_cnt) |
			(mask->ctrl_cnt & info->ctrl_cnt);

	cctrl->resp_ref_rate = (cctrl->resp_ref_rate & ~mask->resp_ref_rate) |
			(mask->resp_ref_rate & info->resp_ref_rate);
	cctrl->all_ack_support =
			(cctrl->all_ack_support & ~mask->all_ack_support) |
			(mask->all_ack_support & info->all_ack_support);
	cctrl->bsr_queue_size_format =
			(cctrl->bsr_queue_size_format &
			 ~mask->bsr_queue_size_format) |
			(mask->bsr_queue_size_format &
			 info->bsr_queue_size_format);
	cctrl->bsr_om_upd_en = (cctrl->bsr_om_upd_en & ~mask->bsr_om_upd_en) |
			(mask->bsr_om_upd_en & info->bsr_om_upd_en);
	cctrl->macid_fwd_idc = (cctrl->macid_fwd_idc & ~mask->macid_fwd_idc) |
			(mask->macid_fwd_idc & info->macid_fwd_idc);
	cctrl->addr_cam_index = (cctrl->addr_cam_index &
				 ~mask->addr_cam_index) |
			(mask->addr_cam_index & info->addr_cam_index);

	cctrl->nominal_pkt_padding =
			(cctrl->nominal_pkt_padding &
			 ~mask->nominal_pkt_padding) |
			(mask->nominal_pkt_padding & info->nominal_pkt_padding);
	cctrl->nominal_pkt_padding40 =
			(cctrl->nominal_pkt_padding40 &
			 ~mask->nominal_pkt_padding40) |
			(mask->nominal_pkt_padding40 &
			 info->nominal_pkt_padding40);
	cctrl->nominal_pkt_padding80 =
			(cctrl->nominal_pkt_padding80 &
			 ~mask->nominal_pkt_padding80) |
			(mask->nominal_pkt_padding80 &
			 info->nominal_pkt_padding80);
	cctrl->nominal_pkt_padding160 =
			(cctrl->nominal_pkt_padding160 &
			 ~mask->nominal_pkt_padding160) |
			(mask->nominal_pkt_padding160 &
			 info->nominal_pkt_padding160);
	cctrl->paid = (cctrl->paid & ~mask->paid) |
			(mask->paid & info->paid);
	cctrl->uldl = (cctrl->uldl & ~mask->uldl) |
			(mask->uldl & info->uldl);
	cctrl->doppler_ctrl = (cctrl->doppler_ctrl & ~mask->doppler_ctrl) |
			(mask->doppler_ctrl & info->doppler_ctrl);
	cctrl->txpwr_tolerence =
			(cctrl->txpwr_tolerence & ~mask->txpwr_tolerence) |
			(mask->txpwr_tolerence & info->txpwr_tolerence);

	cctrl->nc = (cctrl->nc & ~mask->nc) |
			(mask->nc & info->nc);
	cctrl->nr = (cctrl->nr & ~mask->nr) |
			(mask->nr & info->nr);
	cctrl->ng = (cctrl->ng & ~mask->ng) |
			(mask->ng & info->ng);
	cctrl->cb = (cctrl->cb & ~mask->cb) |
			(mask->cb & info->cb);
	cctrl->cs = (cctrl->cs & ~mask->cs) |
			(mask->cs & info->cs);
	cctrl->csi_txbf_en = (cctrl->csi_txbf_en & ~mask->csi_txbf_en) |
			(mask->csi_txbf_en & info->csi_txbf_en);
	cctrl->csi_stbc_en = (cctrl->csi_stbc_en & ~mask->csi_stbc_en) |
			(mask->csi_stbc_en & info->csi_stbc_en);
	cctrl->csi_ldpc_en = (cctrl->csi_ldpc_en & ~mask->csi_ldpc_en) |
			(mask->csi_ldpc_en & info->csi_ldpc_en);
	cctrl->csi_para_en = (cctrl->csi_para_en & ~mask->csi_para_en) |
			(mask->csi_para_en & info->csi_para_en);
	cctrl->csi_fix_rate = (cctrl->csi_fix_rate & ~mask->csi_fix_rate) |
			(mask->csi_fix_rate & info->csi_fix_rate);
	cctrl->csi_bw = (cctrl->csi_bw & ~mask->csi_bw) |
			(mask->csi_bw & info->csi_bw);
}

void mac_upd_role_cctrl_v1_8852c(struct mac_ax_adapter *adapter,
				 struct rtw_hal_mac_ax_cctl_info *info,
				 struct rtw_hal_mac_ax_cctl_info *mask, u8 macid)
{
	struct mac_role_tbl *role;

	role = mac_role_srch(adapter, macid);
	if (!role) {
		PLTFM_MSG_ERR("role search failed\n");
		return;
	}
	_set_role_cctrl_v1_8852c(adapter, info, mask, &role->info.c_info);
}

u32 mac_upd_cctl_info_8852c(struct mac_ax_adapter *adapter,
			    struct rtw_hal_mac_ax_cctl_info *info,
			    struct rtw_hal_mac_ax_cctl_info *mask,
			    u8 macid, u8 operation)
{
	u32 ret = 0;
	u8 *buf;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	struct fwcmd_cctlinfo_ud *tbl;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_DATA);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, sizeof(struct fwcmd_cctlinfo_ud));
	if (!buf) {
		ret = MACNOBUF;
		goto fail;
	}

	tbl = (struct fwcmd_cctlinfo_ud *)buf;
	tbl->dword0 =
	cpu_to_le32(SET_WORD(macid, FWCMD_H2C_CCTLINFO_UD_MACID) |
		    (operation ? FWCMD_H2C_CCTLINFO_UD_OP : 0));

	tbl->dword1 =
	cpu_to_le32(SET_WORD(info->datarate, FWCMD_H2C_CCTRL_DATARATE) |
		    (info->force_txop ? FWCMD_H2C_CCTRL_FORCE_TXOP : 0) |
		    SET_WORD(info->data_bw, FWCMD_H2C_CCTRL_DATA_BW) |
		    SET_WORD(info->data_gi_ltf, FWCMD_H2C_CCTRL_DATA_GI_LTF) |
		    (info->darf_tc_index ? FWCMD_H2C_CCTRL_DARF_TC_INDEX : 0) |
		    SET_WORD(info->arfr_ctrl, FWCMD_H2C_CCTRL_ARFR_CTRL) |
		    (info->acq_rpt_en ? FWCMD_H2C_CCTRL_ACQ_RPT_EN : 0) |
		    (info->mgq_rpt_en ? FWCMD_H2C_CCTRL_MGQ_RPT_EN : 0) |
		    (info->ulq_rpt_en ? FWCMD_H2C_CCTRL_ULQ_RPT_EN : 0) |
		    (info->twtq_rpt_en ? FWCMD_H2C_CCTRL_TWTQ_RPT_EN : 0) |
		    (info->disrtsfb ? FWCMD_H2C_CCTRL_DISRTSFB : 0) |
		    (info->disdatafb ? FWCMD_H2C_CCTRL_DISDATAFB : 0) |
		    (info->tryrate ? FWCMD_H2C_CCTRL_TRYRATE : 0) |
		    SET_WORD(info->ampdu_density, FWCMD_H2C_CCTRL_AMPDU_DENSITY));

	if (info->rtsrate >= MAC_AX_VHT_NSS1_MCS0) // for Fool-proof mechanism
		info->rtsrate = MAC_AX_OFDM6;

	tbl->dword2 =
	cpu_to_le32(SET_WORD(info->data_rty_lowest_rate,
			     FWCMD_H2C_CCTRL_DATA_RTY_LOWEST_RATE) |
		    (info->ampdu_time_sel ? FWCMD_H2C_CCTRL_AMPDU_TIME_SEL : 0) |
		    (info->ampdu_len_sel ? FWCMD_H2C_CCTRL_AMPDU_LEN_SEL : 0) |
		    (info->rts_txcnt_lmt_sel ? FWCMD_H2C_CCTRL_RTS_TXCNT_LMT_SEL :
		     0) |
		    SET_WORD(info->rts_txcnt_lmt, FWCMD_H2C_CCTRL_RTS_TXCNT_LMT) |
		    SET_WORD(info->rtsrate, FWCMD_H2C_CCTRL_RTSRATE) |
		    (info->vcs_stbc ? FWCMD_H2C_CCTRL_VCS_STBC : 0) |
		    SET_WORD(info->rts_rty_lowest_rate,
			     FWCMD_H2C_CCTRL_RTS_RTY_LOWEST_RATE));

	tbl->dword3 =
	cpu_to_le32(SET_WORD(info->data_tx_cnt_lmt,
			     FWCMD_H2C_CCTRL_DATA_TX_CNT_LMT) |
		    (info->data_txcnt_lmt_sel ? FWCMD_H2C_CCTRL_DATA_TXCNT_LMT_SEL :
		     0) |
		    (info->max_agg_num_sel ? FWCMD_H2C_CCTRL_MAX_AGG_NUM_SEL : 0) |
		    (info->rts_en ? FWCMD_H2C_CCTRL_RTS_EN : 0) |
		    (info->cts2self_en ? FWCMD_H2C_CCTRL_CTS2SELF_EN : 0) |
		    SET_WORD(info->cca_rts, FWCMD_H2C_CCTRL_CCA_RTS) |
		    (info->hw_rts_en ? FWCMD_H2C_CCTRL_HW_RTS_EN : 0) |
		    SET_WORD(info->rts_drop_data_mode,
			     FWCMD_H2C_CCTRL_RTS_DROP_DATA_MODE) |
			(info->preld_en ? FWCMD_H2C_CCTRL_V1_PRELD_EN : 0) |
		    SET_WORD(info->ampdu_max_len, FWCMD_H2C_CCTRL_AMPDU_MAX_LEN) |
		    (info->ul_mu_dis ? FWCMD_H2C_CCTRL_UL_MU_DIS : 0) |
		    SET_WORD(info->ampdu_max_time, FWCMD_H2C_CCTRL_AMPDU_MAX_TIME));

	if (info->max_agg_num > 0)
		info->max_agg_num -= 1;

	tbl->dword4 =
	cpu_to_le32(SET_WORD(info->max_agg_num, FWCMD_H2C_CCTRL_MAX_AGG_NUM) |
		    SET_WORD(info->ba_bmap, FWCMD_H2C_CCTRL_BA_BMAP) |
		    SET_WORD(info->vo_lftime_sel, FWCMD_H2C_CCTRL_VO_LFTIME_SEL) |
		    SET_WORD(info->vi_lftime_sel, FWCMD_H2C_CCTRL_VI_LFTIME_SEL) |
		    SET_WORD(info->be_lftime_sel, FWCMD_H2C_CCTRL_BE_LFTIME_SEL) |
		    SET_WORD(info->bk_lftime_sel, FWCMD_H2C_CCTRL_BK_LFTIME_SEL));

	tbl->dword5 =
	cpu_to_le32(SET_WORD(info->multi_port_id, FWCMD_H2C_CCTRL_MULTI_PORT_ID) |
		    (info->bmc ? FWCMD_H2C_CCTRL_BMC : 0) |
		    SET_WORD(info->mbssid, FWCMD_H2C_CCTRL_MBSSID) |
		    (info->data_dcm ? FWCMD_H2C_CCTRL_DATA_DCM : 0) |
		    (info->data_er ? FWCMD_H2C_CCTRL_DATA_ER : 0) |
		    (info->data_ldpc ? FWCMD_H2C_CCTRL_DATA_LDPC : 0) |
		    (info->data_stbc ? FWCMD_H2C_CCTRL_DATA_STBC : 0) |
		    (info->a_ctrl_bqr ? FWCMD_H2C_CCTRL_A_CTRL_BQR : 0) |
		    (info->a_ctrl_uph ? FWCMD_H2C_CCTRL_A_CTRL_UPH : 0) |
		    (info->a_ctrl_bsr ? FWCMD_H2C_CCTRL_A_CTRL_BSR : 0) |
		    (info->a_ctrl_cas ? FWCMD_H2C_CCTRL_A_CTRL_CAS : 0) |
		    (info->data_bw_er ? FWCMD_H2C_CCTRL_DATA_BW_ER : 0) |
		    (info->lsig_txop_en ? FWCMD_H2C_CCTRL_LSIG_TXOP_EN : 0) |
		    (info->ctrl_cnt_vld ? FWCMD_H2C_CCTRL_CTRL_CNT_VLD : 0) |
		    SET_WORD(info->ctrl_cnt, FWCMD_H2C_CCTRL_CTRL_CNT));

	tbl->dword6 =
	cpu_to_le32(SET_WORD(info->resp_ref_rate, FWCMD_H2C_CCTRL_RESP_REF_RATE) |
		    (info->all_ack_support ? FWCMD_H2C_CCTRL_ALL_ACK_SUPPORT : 0) |
		    (info->bsr_queue_size_format ?
		     FWCMD_H2C_CCTRL_BSR_QUEUE_SIZE_FORMAT : 0) |
			(info->bsr_om_upd_en ? FWCMD_H2C_CCTRL_V1_BSR_OM_UPD_EN : 0) |
			(info->macid_fwd_idc ? FWCMD_H2C_CCTRL_V1_MACID_FWD_IDC : 0) |
		    SET_WORD(info->addr_cam_index, FWCMD_H2C_CCTRL_V1_ADDR_CAM_INDEX));

	tbl->dword7 =
	cpu_to_le32(SET_WORD(info->nominal_pkt_padding,
			     FWCMD_H2C_CCTRL_V1_NOMINAL_PKT_PADDING20) |
		    SET_WORD(info->nominal_pkt_padding40,
			     FWCMD_H2C_CCTRL_V1_NOMINAL_PKT_PADDING40) |
		    SET_WORD(info->nominal_pkt_padding80,
			     FWCMD_H2C_CCTRL_V1_NOMINAL_PKT_PADDING80) |
		    SET_WORD(info->nominal_pkt_padding160,
			     FWCMD_H2C_CCTRL_V1_NOMINAL_PKT_PADDING160) |
		    SET_WORD(info->paid, FWCMD_H2C_CCTRL_PAID) |
		    (info->uldl ? FWCMD_H2C_CCTRL_ULDL : 0) |
		    SET_WORD(info->doppler_ctrl, FWCMD_H2C_CCTRL_DOPPLER_CTRL) |
		    SET_WORD(info->txpwr_tolerence, FWCMD_H2C_CCTRL_V1_TXPWR_TOLERENCE));

	tbl->dword8 =
	cpu_to_le32(SET_WORD(info->nc, FWCMD_H2C_CCTRL_NC) |
		    SET_WORD(info->nr, FWCMD_H2C_CCTRL_NR) |
		    SET_WORD(info->ng, FWCMD_H2C_CCTRL_NG) |
		    SET_WORD(info->cb, FWCMD_H2C_CCTRL_CB) |
		    SET_WORD(info->cs, FWCMD_H2C_CCTRL_CS) |
		    (info->csi_txbf_en ? FWCMD_H2C_CCTRL_CSI_TXBF_EN : 0) |
		    (info->csi_stbc_en ? FWCMD_H2C_CCTRL_CSI_STBC_EN : 0) |
		    (info->csi_ldpc_en ? FWCMD_H2C_CCTRL_CSI_LDPC_EN : 0) |
		    (info->csi_para_en ? FWCMD_H2C_CCTRL_CSI_PARA_EN : 0) |
		    SET_WORD(info->csi_fix_rate, FWCMD_H2C_CCTRL_CSI_FIX_RATE) |
		    SET_WORD(info->csi_bw, FWCMD_H2C_CCTRL_V1_CSI_BW));

	tbl->dword9 =
	cpu_to_le32(SET_WORD(mask->datarate, FWCMD_H2C_CCTRL_DATARATE) |
		    (mask->force_txop ? FWCMD_H2C_CCTRL_FORCE_TXOP : 0) |
		    SET_WORD(mask->data_bw, FWCMD_H2C_CCTRL_DATA_BW) |
		    SET_WORD(mask->data_gi_ltf, FWCMD_H2C_CCTRL_DATA_GI_LTF) |
		    (mask->darf_tc_index ? FWCMD_H2C_CCTRL_DARF_TC_INDEX : 0) |
		    SET_WORD(mask->arfr_ctrl, FWCMD_H2C_CCTRL_ARFR_CTRL) |
		    (mask->acq_rpt_en ? FWCMD_H2C_CCTRL_ACQ_RPT_EN : 0) |
		    (mask->mgq_rpt_en ? FWCMD_H2C_CCTRL_MGQ_RPT_EN : 0) |
		    (mask->ulq_rpt_en ? FWCMD_H2C_CCTRL_ULQ_RPT_EN : 0) |
		    (mask->twtq_rpt_en ? FWCMD_H2C_CCTRL_TWTQ_RPT_EN : 0) |
		    (mask->disrtsfb ? FWCMD_H2C_CCTRL_DISRTSFB : 0) |
		    (mask->disdatafb ? FWCMD_H2C_CCTRL_DISDATAFB : 0) |
		    (mask->tryrate ? FWCMD_H2C_CCTRL_TRYRATE : 0) |
		    SET_WORD(mask->ampdu_density, FWCMD_H2C_CCTRL_AMPDU_DENSITY));

	tbl->dword10 =
	cpu_to_le32(SET_WORD(mask->data_rty_lowest_rate,
			     FWCMD_H2C_CCTRL_DATA_RTY_LOWEST_RATE) |
		    (mask->ampdu_time_sel ? FWCMD_H2C_CCTRL_AMPDU_TIME_SEL : 0) |
		    (mask->ampdu_len_sel ? FWCMD_H2C_CCTRL_AMPDU_LEN_SEL : 0) |
		    (mask->rts_txcnt_lmt_sel ? FWCMD_H2C_CCTRL_RTS_TXCNT_LMT_SEL :
		     0) |
		    SET_WORD(mask->rts_txcnt_lmt, FWCMD_H2C_CCTRL_RTS_TXCNT_LMT) |
		    SET_WORD(mask->rtsrate, FWCMD_H2C_CCTRL_RTSRATE) |
		    (mask->vcs_stbc ? FWCMD_H2C_CCTRL_VCS_STBC : 0) |
		    SET_WORD(mask->rts_rty_lowest_rate,
			     FWCMD_H2C_CCTRL_RTS_RTY_LOWEST_RATE));

	tbl->dword11 =
	cpu_to_le32(SET_WORD(mask->data_tx_cnt_lmt,
			     FWCMD_H2C_CCTRL_DATA_TX_CNT_LMT) |
		    (mask->data_txcnt_lmt_sel ? FWCMD_H2C_CCTRL_DATA_TXCNT_LMT_SEL :
		     0) |
		    (mask->max_agg_num_sel ? FWCMD_H2C_CCTRL_MAX_AGG_NUM_SEL : 0) |
		    (mask->rts_en ? FWCMD_H2C_CCTRL_RTS_EN : 0) |
		    (mask->cts2self_en ? FWCMD_H2C_CCTRL_CTS2SELF_EN : 0) |
		    SET_WORD(mask->cca_rts, FWCMD_H2C_CCTRL_CCA_RTS) |
		    (mask->hw_rts_en ? FWCMD_H2C_CCTRL_HW_RTS_EN : 0) |
		    SET_WORD(mask->rts_drop_data_mode,
			     FWCMD_H2C_CCTRL_RTS_DROP_DATA_MODE) |
			(mask->preld_en ? FWCMD_H2C_CCTRL_V1_PRELD_EN : 0) |
		    SET_WORD(mask->ampdu_max_len, FWCMD_H2C_CCTRL_AMPDU_MAX_LEN) |
		    (mask->ul_mu_dis ? FWCMD_H2C_CCTRL_UL_MU_DIS : 0) |
		    SET_WORD(mask->ampdu_max_time, FWCMD_H2C_CCTRL_AMPDU_MAX_TIME));

	tbl->dword12 =
	cpu_to_le32(SET_WORD(mask->max_agg_num, FWCMD_H2C_CCTRL_MAX_AGG_NUM) |
		    SET_WORD(mask->ba_bmap, FWCMD_H2C_CCTRL_BA_BMAP) |
		    SET_WORD(mask->vo_lftime_sel, FWCMD_H2C_CCTRL_VO_LFTIME_SEL) |
		    SET_WORD(mask->vi_lftime_sel, FWCMD_H2C_CCTRL_VI_LFTIME_SEL) |
		    SET_WORD(mask->be_lftime_sel, FWCMD_H2C_CCTRL_BE_LFTIME_SEL) |
		    SET_WORD(mask->bk_lftime_sel, FWCMD_H2C_CCTRL_BK_LFTIME_SEL));

	tbl->dword13 =
	cpu_to_le32(SET_WORD(mask->multi_port_id, FWCMD_H2C_CCTRL_MULTI_PORT_ID) |
		    (mask->bmc ? FWCMD_H2C_CCTRL_BMC : 0) |
		    SET_WORD(mask->mbssid, FWCMD_H2C_CCTRL_MBSSID) |
		    (mask->data_dcm ? FWCMD_H2C_CCTRL_DATA_DCM : 0) |
		    (mask->data_er ? FWCMD_H2C_CCTRL_DATA_ER : 0) |
		    (mask->data_ldpc ? FWCMD_H2C_CCTRL_DATA_LDPC : 0) |
		    (mask->data_stbc ? FWCMD_H2C_CCTRL_DATA_STBC : 0) |
		    (mask->a_ctrl_bqr ? FWCMD_H2C_CCTRL_A_CTRL_BQR : 0) |
		    (mask->a_ctrl_uph ? FWCMD_H2C_CCTRL_A_CTRL_UPH : 0) |
		    (mask->a_ctrl_bsr ? FWCMD_H2C_CCTRL_A_CTRL_BSR : 0) |
		    (mask->a_ctrl_cas ? FWCMD_H2C_CCTRL_A_CTRL_CAS : 0) |
		    (mask->data_bw_er ? FWCMD_H2C_CCTRL_DATA_BW_ER : 0) |
		    (mask->lsig_txop_en ? FWCMD_H2C_CCTRL_LSIG_TXOP_EN : 0) |
		    (mask->ctrl_cnt_vld ? FWCMD_H2C_CCTRL_CTRL_CNT_VLD : 0) |
		    SET_WORD(mask->ctrl_cnt, FWCMD_H2C_CCTRL_CTRL_CNT));

	tbl->dword14 =
	cpu_to_le32(SET_WORD(mask->resp_ref_rate, FWCMD_H2C_CCTRL_RESP_REF_RATE) |
		    (mask->all_ack_support ? FWCMD_H2C_CCTRL_ALL_ACK_SUPPORT : 0) |
		    (mask->bsr_queue_size_format ?
		     FWCMD_H2C_CCTRL_BSR_QUEUE_SIZE_FORMAT : 0) |
			(mask->bsr_om_upd_en ? FWCMD_H2C_CCTRL_V1_BSR_OM_UPD_EN : 0) |
			(mask->macid_fwd_idc ? FWCMD_H2C_CCTRL_V1_MACID_FWD_IDC : 0) |
		    SET_WORD(mask->addr_cam_index, FWCMD_H2C_CCTRL_V1_ADDR_CAM_INDEX));

	tbl->dword15 =
	cpu_to_le32(SET_WORD(mask->nominal_pkt_padding,
			     FWCMD_H2C_CCTRL_V1_NOMINAL_PKT_PADDING20) |
		    SET_WORD(mask->nominal_pkt_padding40,
			     FWCMD_H2C_CCTRL_V1_NOMINAL_PKT_PADDING40) |
		    SET_WORD(mask->nominal_pkt_padding80,
			     FWCMD_H2C_CCTRL_V1_NOMINAL_PKT_PADDING80) |
		    SET_WORD(mask->nominal_pkt_padding160,
			     FWCMD_H2C_CCTRL_V1_NOMINAL_PKT_PADDING160) |
		    SET_WORD(mask->paid, FWCMD_H2C_CCTRL_PAID) |
		    (mask->uldl ? FWCMD_H2C_CCTRL_ULDL : 0) |
		    SET_WORD(mask->doppler_ctrl, FWCMD_H2C_CCTRL_DOPPLER_CTRL) |
		    SET_WORD(mask->txpwr_tolerence, FWCMD_H2C_CCTRL_V1_TXPWR_TOLERENCE));

	tbl->dword16 =
	cpu_to_le32(SET_WORD(mask->nc, FWCMD_H2C_CCTRL_NC) |
		    SET_WORD(mask->nr, FWCMD_H2C_CCTRL_NR) |
		    SET_WORD(mask->ng, FWCMD_H2C_CCTRL_NG) |
		    SET_WORD(mask->cb, FWCMD_H2C_CCTRL_CB) |
		    SET_WORD(mask->cs, FWCMD_H2C_CCTRL_CS) |
		    (mask->csi_txbf_en ? FWCMD_H2C_CCTRL_CSI_TXBF_EN : 0) |
		    (mask->csi_stbc_en ? FWCMD_H2C_CCTRL_CSI_STBC_EN : 0) |
		    (mask->csi_ldpc_en ? FWCMD_H2C_CCTRL_CSI_LDPC_EN : 0) |
		    (mask->csi_para_en ? FWCMD_H2C_CCTRL_CSI_PARA_EN : 0) |
		    SET_WORD(mask->csi_fix_rate, FWCMD_H2C_CCTRL_CSI_FIX_RATE) |
		    SET_WORD(mask->csi_bw, FWCMD_H2C_CCTRL_V1_CSI_BW));

	if (adapter->sm.fwdl == MAC_AX_FWDL_INIT_RDY) {
		ret = h2c_pkt_set_hdr(adapter, h2cb,
				      FWCMD_TYPE_H2C,
				      FWCMD_H2C_CAT_MAC,
				      FWCMD_H2C_CL_FR_EXCHG,
				      FWCMD_H2C_FUNC_CCTLINFO_UD,
				      0,
				      1);
		if (ret)
			goto fail;

		// Return MACSUCCESS if h2c aggregation is enabled and enqueued successfully.
		// The H2C shall be sent by mac_h2c_agg_tx.
		ret = h2c_agg_enqueue(adapter, h2cb);
		if (ret == MACSUCCESS)
			return MACSUCCESS;

		ret = h2c_pkt_build_txd(adapter, h2cb);
		if (ret)
			goto fail;

		#if MAC_AX_PHL_H2C
		ret = PLTFM_TX(h2cb);
		#else
		ret = PLTFM_TX(h2cb->data, h2cb->len);
		#endif
		if (ret)
			goto fail;
	} else {
#if MAC_AX_FEATURE_DBGPKG
		if (operation)
			cctl_info_debug_write_8852c
			(adapter, (struct fwcmd_cctlinfo_ud *)buf, macid);
#else
		return MACFWNONRDY;
#endif
	}

	h2cb_free(adapter, h2cb);

	mac_upd_role_cctrl_v1_8852c(adapter, info, mask, macid);

	return MACSUCCESS;
fail:
	h2cb_free(adapter, h2cb);

	return ret;
}

u32 mac_init_cctl_info_8852c(struct mac_ax_adapter *adapter, u8 macid)
{
	struct mac_ax_ops *mops = adapter_to_mac_ops(adapter);
	struct rtw_hal_mac_ax_cctl_info info = {0x0};
	struct rtw_hal_mac_ax_cctl_info mask;

	// dword0
	info.datarate = MAC_AX_OFDM6;
	// dword1
	info.data_rty_lowest_rate = MAC_AX_OFDM6;
	info.rtsrate = MAC_AX_OFDM48;
	info.rts_rty_lowest_rate = MAC_AX_OFDM6;
	// dword5
	info.resp_ref_rate = MAC_AX_OFDM54;
	// dword6
	info.nominal_pkt_padding = CCTRL_NOMINAL_PKT_PADDING_16;
	info.nominal_pkt_padding40 = CCTRL_NOMINAL_PKT_PADDING_16;
	info.nominal_pkt_padding80 = CCTRL_NOMINAL_PKT_PADDING_16;
	info.nominal_pkt_padding160 = CCTRL_NOMINAL_PKT_PADDING_16;
	// dword7
	info.nc = CCTRL_NC;
	info.nr = CCTRL_NR;
	info.cb = CCTRL_CB;
	info.csi_para_en = 0x1;
	info.csi_fix_rate = MAC_AX_OFDM54;

	PLTFM_MEMSET(&mask, 0xFF, sizeof(struct rtw_hal_mac_ax_cctl_info));

	return mops->upd_cctl_info(adapter, &info, &mask, macid, 1);
}

#if MAC_AX_FEATURE_DBGPKG
u32 cctl_info_debug_write_8852c(struct mac_ax_adapter *adapter,
				struct fwcmd_cctlinfo_ud *tbl,
				u8 macid)
{
	u32 val;
	u32 *data = &tbl->dword1, *msk = &tbl->dword9;
	u8 i;

	for (i = 0; i < (CCTL_INFO_SIZE >> 2); i++) {
		val = mac_sram_dbg_read(adapter, macid * CCTL_INFO_SIZE + i * 4,
					CMAC_TBL_SEL);
		val = (val & ~(*(msk + i))) | ((*(data + i)) & (*(msk + i)));
		mac_sram_dbg_write(adapter, macid * CCTL_INFO_SIZE + i * 4, val,
				   CMAC_TBL_SEL);
	}

	return MACSUCCESS;
}

u32 dctl_info_debug_write_8852c(struct mac_ax_adapter *adapter,
				struct fwcmd_dctlinfo_ud_v1 *tbl,
				u8 macid)
{
	mac_sram_dbg_write(adapter, macid * DCTL_INFO_SIZE_V1, tbl->dword1,
			   DMAC_TBL_SEL);
	mac_sram_dbg_write(adapter, macid * DCTL_INFO_SIZE_V1 + 4, tbl->dword2,
			   DMAC_TBL_SEL);
	mac_sram_dbg_write(adapter, macid * DCTL_INFO_SIZE_V1 + 8, tbl->dword3,
			   DMAC_TBL_SEL);
	mac_sram_dbg_write(adapter, macid * DCTL_INFO_SIZE_V1 + 12, tbl->dword4,
			   DMAC_TBL_SEL);
	mac_sram_dbg_write(adapter, macid * DCTL_INFO_SIZE_V1 + 16, tbl->dword5,
			   DMAC_TBL_SEL);
	mac_sram_dbg_write(adapter, macid * DCTL_INFO_SIZE_V1 + 20, tbl->dword6,
			   DMAC_TBL_SEL);
	mac_sram_dbg_write(adapter, macid * DCTL_INFO_SIZE_V1 + 24, tbl->dword7,
			   DMAC_TBL_SEL);

	return MACSUCCESS;
}

u32 mac_ss_dl_grp_upd_8852c(struct mac_ax_adapter *adapter,
			    struct mac_ax_ss_dl_grp_upd *info)
{
	u32 ret = 0;
	u8 *buf;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	struct fwcmd_dl_grp_upd_v2 *tbl;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_CMD);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, sizeof(struct fwcmd_dl_grp_upd_v2));
	if (!buf) {
		ret = MACNOBUF;
		goto fail;
	}

	tbl = (struct fwcmd_dl_grp_upd_v2 *)buf;
	tbl->dword0 =
	cpu_to_le32(SET_WORD(info->grp_valid, FWCMD_H2C_DL_GRP_UPD_V2_GRP_VALID) |
		    SET_WORD(info->grp_id, FWCMD_H2C_DL_GRP_UPD_V2_GRP_ID) |
		    SET_WORD(info->is_hwgrp, FWCMD_H2C_DL_GRP_UPD_V2_IS_HWGRP) |
		    SET_WORD(info->mru, FWCMD_H2C_DL_GRP_UPD_V2_MRU));

	tbl->dword1 =
	cpu_to_le32(SET_WORD(info->macid_u0, FWCMD_H2C_DL_GRP_UPD_V2_MACID_U0) |
			SET_WORD(info->macid_u1, FWCMD_H2C_DL_GRP_UPD_V2_MACID_U1) |
			SET_WORD(info->macid_u2, FWCMD_H2C_DL_GRP_UPD_V2_MACID_U2) |
			SET_WORD(info->macid_u3, FWCMD_H2C_DL_GRP_UPD_V2_MACID_U3));

	tbl->dword2 =
	cpu_to_le32(SET_WORD(info->macid_u4, FWCMD_H2C_DL_GRP_UPD_V2_MACID_U4) |
			SET_WORD(info->macid_u5, FWCMD_H2C_DL_GRP_UPD_V2_MACID_U5) |
			SET_WORD(info->macid_u6, FWCMD_H2C_DL_GRP_UPD_V2_MACID_U6) |
			SET_WORD(info->macid_u7, FWCMD_H2C_DL_GRP_UPD_V2_MACID_U7));

	tbl->dword3 =
	cpu_to_le32(SET_WORD(info->ac_bitmap_u0, FWCMD_H2C_DL_GRP_UPD_V2_AC_BITMAP_U0) |
			SET_WORD(info->ac_bitmap_u1, FWCMD_H2C_DL_GRP_UPD_V2_AC_BITMAP_U1) |
			SET_WORD(info->ac_bitmap_u2, FWCMD_H2C_DL_GRP_UPD_V2_AC_BITMAP_U2) |
			SET_WORD(info->ac_bitmap_u3, FWCMD_H2C_DL_GRP_UPD_V2_AC_BITMAP_U3));

	tbl->dword4 =
	cpu_to_le32(SET_WORD(info->ac_bitmap_u4, FWCMD_H2C_DL_GRP_UPD_V2_AC_BITMAP_U4) |
			SET_WORD(info->ac_bitmap_u5, FWCMD_H2C_DL_GRP_UPD_V2_AC_BITMAP_U5) |
			SET_WORD(info->ac_bitmap_u6, FWCMD_H2C_DL_GRP_UPD_V2_AC_BITMAP_U6) |
			SET_WORD(info->ac_bitmap_u7, FWCMD_H2C_DL_GRP_UPD_V2_AC_BITMAP_U7));

	tbl->dword5 =
	cpu_to_le32(SET_WORD(info->next_protecttype, FWCMD_H2C_DL_GRP_UPD_V2_NEXT_PROTECTTYPE) |
			SET_WORD(info->next_rsptype, FWCMD_H2C_DL_GRP_UPD_V2_NEXT_RSPTYPE) |
			SET_WORD(info->w_idx, FWCMD_H2C_DL_GRP_UPD_V2_W_IDX));

	tbl->dword6 =
	cpu_to_le32(SET_WORD(info->dcm_u0, FWCMD_H2C_DL_GRP_UPD_V2_DCM_U0) |
			SET_WORD(info->dcm_u1, FWCMD_H2C_DL_GRP_UPD_V2_DCM_U1) |
			SET_WORD(info->dcm_u2, FWCMD_H2C_DL_GRP_UPD_V2_DCM_U2) |
			SET_WORD(info->dcm_u3, FWCMD_H2C_DL_GRP_UPD_V2_DCM_U3));

	tbl->dword7 =
	cpu_to_le32(SET_WORD(info->dcm_u4, FWCMD_H2C_DL_GRP_UPD_V2_DCM_U4) |
			SET_WORD(info->dcm_u5, FWCMD_H2C_DL_GRP_UPD_V2_DCM_U5) |
			SET_WORD(info->dcm_u6, FWCMD_H2C_DL_GRP_UPD_V2_DCM_U6) |
			SET_WORD(info->dcm_u7, FWCMD_H2C_DL_GRP_UPD_V2_DCM_U7));

	tbl->dword8 =
	cpu_to_le32(SET_WORD(info->ss_u0, FWCMD_H2C_DL_GRP_UPD_V2_SS_U0) |
			SET_WORD(info->ss_u1, FWCMD_H2C_DL_GRP_UPD_V2_SS_U1) |
			SET_WORD(info->ss_u2, FWCMD_H2C_DL_GRP_UPD_V2_SS_U2) |
			SET_WORD(info->ss_u3, FWCMD_H2C_DL_GRP_UPD_V2_SS_U3));

	tbl->dword9 =
	cpu_to_le32(SET_WORD(info->ss_u4, FWCMD_H2C_DL_GRP_UPD_V2_SS_U4) |
			SET_WORD(info->ss_u5, FWCMD_H2C_DL_GRP_UPD_V2_SS_U5) |
			SET_WORD(info->ss_u6, FWCMD_H2C_DL_GRP_UPD_V2_SS_U6) |
			SET_WORD(info->ss_u7, FWCMD_H2C_DL_GRP_UPD_V2_SS_U7));

	tbl->dword10 =
	cpu_to_le32(SET_WORD(info->rate_idx_u0, FWCMD_H2C_DL_GRP_UPD_V2_RATE_IDX_U0) |
			SET_WORD(info->rate_idx_u1, FWCMD_H2C_DL_GRP_UPD_V2_RATE_IDX_U1) |
			SET_WORD(info->rate_idx_u2, FWCMD_H2C_DL_GRP_UPD_V2_RATE_IDX_U2) |
			SET_WORD(info->rate_idx_u3, FWCMD_H2C_DL_GRP_UPD_V2_RATE_IDX_U3));

	tbl->dword11 =
	cpu_to_le32(SET_WORD(info->rate_idx_u4, FWCMD_H2C_DL_GRP_UPD_V2_RATE_IDX_U4) |
			SET_WORD(info->rate_idx_u5, FWCMD_H2C_DL_GRP_UPD_V2_RATE_IDX_U5) |
			SET_WORD(info->rate_idx_u6, FWCMD_H2C_DL_GRP_UPD_V2_RATE_IDX_U6) |
			SET_WORD(info->rate_idx_u7, FWCMD_H2C_DL_GRP_UPD_V2_RATE_IDX_U7));

	tbl->dword12 =
	cpu_to_le32(SET_WORD(info->trigger_len, FWCMD_H2C_DL_GRP_UPD_V2_TRIGGER_LEN) |
			SET_WORD(info->trigger_ul_threshold,
				 FWCMD_H2C_DL_GRP_UPD_V2_TRIGGER_UL_THRESHOLD));

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C,
			      FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_MEDIA_RPT,
			      FWCMD_H2C_FUNC_DL_GRP_UPD,
			      0,
			      1);
	if (ret)
		goto fail;

	ret = h2c_pkt_build_txd(adapter, h2cb);
	if (ret)
		goto fail;

	#if MAC_AX_PHL_H2C
	ret = PLTFM_TX(h2cb);
	#else
	ret = PLTFM_TX(h2cb->data, h2cb->len);
	#endif
	if (ret)
		goto fail;

	h2cb_free(adapter, h2cb);

	return MACSUCCESS;
fail:
	h2cb_free(adapter, h2cb);

	return ret;
}

#endif
#endif /* #if MAC_AX_8852C_SUPPORT */

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

#include "tblupd.h"

u32 mac_upd_mudecision_para(struct mac_ax_adapter *adapter,
			    struct mac_ax_mudecision_para *info)
{
	u32 ret = 0;
	u8 *buf;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	struct fwcmd_mude_para_tblud *tbl;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_CMD);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, sizeof(struct fwcmd_mude_para_tblud));
	if (!buf) {
		ret = MACNOBUF;
		goto fail;
	}

	tbl = (struct fwcmd_mude_para_tblud *)buf;
	tbl->dword0 =
	cpu_to_le32((info->tbl_hdr.rw ? FWCMD_H2C_TBLUD_R_W : 0) |
		    SET_WORD(info->tbl_hdr.idx, FWCMD_H2C_TBLUD_MACID_GROUP) |
		    SET_WORD(info->tbl_hdr.offset, FWCMD_H2C_TBLUD_OFFSET) |
		    SET_WORD(info->tbl_hdr.len, FWCMD_H2C_TBLUD_LENGTH) |
		    (info->tbl_hdr.type ? FWCMD_H2C_TBLUD_TYPE : 0) |
		    SET_WORD(CLASS_MUDECISION_PARA,
			     FWCMD_H2C_TBLUD_TABLE_CLASS));

	tbl->dword1 =
	cpu_to_le32(SET_WORD(info->mu_thold,
			     FWCMD_H2C_MUDECISION_PARA_MUINFO_THOLD) |
		    (info->bypass_thold ?
		     FWCMD_H2C_MUDECISION_PARA_BYPASS_THOLD : 0) |
		    (info->bypass_tp ?
		     FWCMD_H2C_MUDECISION_PARA_BYPASS_TP : 0));

	 tbl->dword2 =
	 cpu_to_le32(SET_WORD(info->init_rate,
			      FWCMD_H2C_MUDECISION_PARA_INIT_RATE) |
		     SET_WORD(info->retry_th,
			      FWCMD_H2C_MUDECISION_PARA_RETRY_TH));

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C,
			      FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_FR_EXCHG,
			      FWCMD_H2C_FUNC_TBLUD,
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

u32 mac_upd_ul_fixinfo(struct mac_ax_adapter *adapter,
		       struct rtw_phl_ax_ul_fixinfo *info)
{
	u32 ret = 0;
	u8 *buf;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	u32 *dword;
	u16 h2c_size = (5 + RTW_PHL_MAX_RU_NUM / 2 + 2 + RTW_PHL_MAX_RU_NUM * 2) * sizeof(u32);
	u8 i;

	struct rtw_phl_ul_macid_info *sta_info;
	struct rtw_phl_ul_macid_info *sta_info_2;
	struct rtw_phl_ax_ulru_out_sta_ent *sta_ent;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_DATA);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, h2c_size);
	if (!buf) {
		ret = MACNOBUF;
		goto fail;
	}

	dword = (u32 *)buf;

	(*dword++) =
	cpu_to_le32((info->tbl_hdr.rw ? FWCMD_H2C_TBLUD_R_W : 0) |
		    SET_WORD(info->tbl_hdr.idx, FWCMD_H2C_TBLUD_MACID_GROUP) |
		    SET_WORD(info->tbl_hdr.offset, FWCMD_H2C_TBLUD_OFFSET) |
		    SET_WORD(info->tbl_hdr.len, FWCMD_H2C_TBLUD_LENGTH) |
		    (info->tbl_hdr.type ? FWCMD_H2C_TBLUD_TYPE : 0) |
		    SET_WORD(CLASS_UL_FIXINFO,
			     FWCMD_H2C_TBLUD_TABLE_CLASS));

	(*dword++) =
	cpu_to_le32(SET_WORD(info->cfg.mode, FWCMD_H2C_UL_FIXINFO_CFG_MODE) |
		    SET_WORD(info->cfg.interval,
			     FWCMD_H2C_UL_FIXINFO_CFG_INTERVAL) |
		    SET_WORD(info->cfg.bsr_thold,
			     FWCMD_H2C_UL_FIXINFO_CFG_BSR_THOLD) |
		    SET_WORD(info->cfg.storemode,
			     FWCMD_H2C_UL_FIXINFO_CFG_STOREMODE));

	(*dword++) =
	cpu_to_le32(SET_WORD(info->ndpa_dur,
			     FWCMD_H2C_UL_FIXINFO_ULINFO_NDPA_DUR) |
		    SET_WORD(info->tf_type,
			     FWCMD_H2C_UL_FIXINFO_ULINFO_TF_TYPE) |
		    (info->sig_ta_pkten ?
		     FWCMD_H2C_UL_FIXINFO_ULINFO_SIGEN : 0) |
		    SET_WORD(info->sig_ta_pktsc,
			     FWCMD_H2C_UL_FIXINFO_ULINFO_SIGSC) |
		    (info->murts_flag ?
		     FWCMD_H2C_UL_FIXINFO_ULINFO_MURTS : 0) |
		    SET_WORD(info->ndpa,
			     FWCMD_H2C_UL_FIXINFO_ULINFO_NDPA) |
		    SET_WORD(info->snd_pkt_sel,
			     FWCMD_H2C_UL_FIXINFO_ULINFO_SNDPKT) |
		    SET_WORD(info->gi_ltf,
			     FWCMD_H2C_UL_FIXINFO_ULINFO_GI_LTF));

	(*dword++) =
	cpu_to_le32(SET_WORD(info->data_rate,
			     FWCMD_H2C_UL_FIXINFO_ULINFO_DATART) |
		    (info->data_er ?
		     FWCMD_H2C_UL_FIXINFO_ULINFO_DATAER : 0) |
		    SET_WORD(info->data_bw,
			     FWCMD_H2C_UL_FIXINFO_ULINFO_DATABW) |
		    SET_WORD(info->data_stbc,
			     FWCMD_H2C_UL_FIXINFO_ULINFO_STBC) |
		    (info->data_ldpc ?
		     FWCMD_H2C_UL_FIXINFO_ULINFO_LDPC : 0) |
		    (info->data_dcm ?
		     FWCMD_H2C_UL_FIXINFO_ULINFO_DATADCM : 0) |
		    SET_WORD(info->apep_len,
			     FWCMD_H2C_UL_FIXINFO_ULINFO_APEPLEN) |
		    (info->more_tf ?
		     FWCMD_H2C_UL_FIXINFO_ULINFO_MORETF : 0) |
		    (info->data_bw_er ?
		     FWCMD_H2C_UL_FIXINFO_ULINFO_DATA_VWER : 0) |
		    (info->istwt ?
		     FWCMD_H2C_UL_FIXINFO_ULINFO_ISTWT : 0) |
		    (info->ul_logo_test ?
		     FWCMD_H2C_UL_FIXINFO_ULINFO_UL_LOGO_TEST : 0));

	(*dword++) =
	cpu_to_le32(SET_WORD(info->multiport_id,
			     FWCMD_H2C_UL_FIXINFO_ULINFO_MULTIPORT) |
		    SET_WORD(info->mbssid,
			     FWCMD_H2C_UL_FIXINFO_ULINFO_MBSSID) |
		    SET_WORD(info->txpwr_mode,
			     FWCMD_H2C_UL_FIXINFO_ULINFO_TXPWR_MODE) |
		    SET_WORD(info->ulfix_usage,
			     FWCMD_H2C_UL_FIXINFO_ULINFO_ULFIX_USAGE) |
		    SET_WORD(info->twtgrp_stanum_sel,
			     FWCMD_H2C_UL_FIXINFO_ULINFO_TWTGRP_STANUM_SEL) |
		    SET_WORD(info->store_idx,
			     FWCMD_H2C_UL_FIXINFO_ULINFO_STORE_IDX));

	for (i = 0; i < RTW_PHL_MAX_RU_NUM; i += 2) {
		sta_info = &info->sta[i];
		sta_info_2 = &info->sta[i + 1];
		(*dword++) =
		cpu_to_le32(SET_WORD(sta_info->macid,
				     FWCMD_H2C_UL_FIXINFO_STA_INFO_MACID_0) |
			    SET_WORD(sta_info->pref_AC,
				     FWCMD_H2C_UL_FIXINFO_STA_INFO_PREF_AC_0) |
			    SET_WORD(sta_info_2->macid,
				     FWCMD_H2C_UL_FIXINFO_STA_INFO_MACID_1) |
			    SET_WORD(sta_info_2->pref_AC,
				     FWCMD_H2C_UL_FIXINFO_STA_INFO_PREF_AC_1));
	}

	(*dword++) =
	cpu_to_le32((info->ulrua.ru2su ?
		     FWCMD_H2C_UL_FIXINFO_ULRUA_RU2SU : 0) |
		    SET_WORD(info->ulrua.ppdu_bw,
			     FWCMD_H2C_UL_FIXINFO_ULRUA_PPDU_BW) |
		    SET_WORD(info->ulrua.gi_ltf,
			     FWCMD_H2C_UL_FIXINFO_ULRUA_GI_LTF) |
		    (info->ulrua.stbc ?
		     FWCMD_H2C_UL_FIXINFO_ULRUA_STBC : 0) |
		    (info->ulrua.doppler ?
		     FWCMD_H2C_UL_FIXINFO_ULRUA_DOPPLER : 0) |
		    SET_WORD(info->ulrua.n_ltf_and_ma,
			     FWCMD_H2C_UL_FIXINFO_ULRUA_LTF_MA) |
		    SET_WORD(info->ulrua.sta_num,
			     FWCMD_H2C_UL_FIXINFO_ULRUA_STANUM) |
		    (info->ulrua.rf_gain_fix ?
		     FWCMD_H2C_UL_FIXINFO_ULRUA_RFGFIX : 0) |
		    SET_WORD(info->ulrua.rf_gain_idx,
			     FWCMD_H2C_UL_FIXINFO_ULRUA_RFGIDX) |
		    SET_WORD(info->ulrua.tb_t_pe_nom,
			     FWCMD_H2C_UL_FIXINFO_ULRUA_TB_NOM));

	(*dword++) =
	cpu_to_le32((info->ulrua.grp_mode ?
		     FWCMD_H2C_UL_FIXINFO_ULRUA_GRP_MODE : 0) |
		    SET_WORD(info->ulrua.grp_id,
			     FWCMD_H2C_UL_FIXINFO_ULRUA_GRP_ID) |
		    (info->ulrua.fix_mode ?
		     FWCMD_H2C_UL_FIXINFO_ULRUA_FIX_MODE : 0));

	for (i = 0; i < RTW_PHL_MAX_RU_NUM; i++) {
		sta_ent = &info->ulrua.sta[i];
		(*dword++) =
		cpu_to_le32((sta_ent->dropping ?
			     FWCMD_H2C_UL_FIXINFO_UL_RUA_STA_ENT_DROP : 0) |
			    SET_WORD(sta_ent->tgt_rssi,
				     FWCMD_H2C_UL_FIXINFO_UL_RUA_STA_ENT_TGT_RSSI) |
			    SET_WORD(sta_ent->mac_id,
				     FWCMD_H2C_UL_FIXINFO_UL_RUA_STA_ENT_MAC_ID) |
			    SET_WORD(sta_ent->ru_pos,
				     FWCMD_H2C_UL_FIXINFO_UL_RUA_STA_ENT_RU_POS) |
			    (sta_ent->coding ?
			     FWCMD_H2C_UL_FIXINFO_UL_RUA_STA_ENT_CODE : 0) |
			    (sta_ent->vip_flag ?
			     FWCMD_H2C_UL_FIXINFO_UL_RUA_STA_ENT_VIP : 0));

		(*dword++) =
		cpu_to_le32(SET_WORD(sta_ent->bsr_length,
				     FWCMD_H2C_UL_FIXINFO_UL_RUA_STA_ENT_BSRLEN) |
			    (sta_ent->rate.dcm ?
			     FWCMD_H2C_UL_FIXINFO_UL_RUA_STA_ENT_DCM : 0) |
			    SET_WORD(sta_ent->rate.ss,
				     FWCMD_H2C_UL_FIXINFO_UL_RUA_STA_ENT_SS) |
			    SET_WORD(sta_ent->rate.mcs,
				     FWCMD_H2C_UL_FIXINFO_UL_RUA_STA_ENT_MCS) |
			    SET_WORD(sta_ent->rpt.rt_tblcol,
				     FWCMD_H2C_UL_FIXINFO_UL_RUA_STA_ENT_RT_TBLCOL) |
			    (sta_ent->rpt.prtl_alloc ?
			     FWCMD_H2C_UL_FIXINFO_UL_RUA_STA_ENT_PRTL_ALLOC : 0) |
			    (sta_ent->rpt.rate_chg ?
			     FWCMD_H2C_UL_FIXINFO_UL_RUA_STA_ENT_RATE_CHG : 0));
	}

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C,
			      FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_FR_EXCHG,
			      FWCMD_H2C_FUNC_TBLUD,
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

u32 mac_f2p_test_cmd(struct mac_ax_adapter *adapter,
		     struct mac_ax_f2p_test_para *info,
		     struct mac_ax_f2p_wd *f2pwd,
		     struct mac_ax_f2p_tx_cmd *ptxcmd,
		     u8 *psigb_addr)
{
	u32 ret = 0;
	u8 *buf;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	struct fwcmd_test_para *tbl;
	struct mac_ax_tf_user_para *user;
	struct mac_ax_tf_depend_user_para *dep_user;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_LONG_DATA);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, sizeof(struct fwcmd_test_para));
	if (!buf) {
		ret = MACNOBUF;
		goto fail;
	}

	PLTFM_MEMSET(buf, 0, sizeof(struct fwcmd_test_para));

	tbl = (struct fwcmd_test_para *)buf;

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

	dep_user = &info->tf_pkt.dep_user[0];
	tbl->byte9 =
	cpu_to_le32(SET_WORD(dep_user->pref_AC, FWCMD_F2PTEST_PREF_AC));

	dep_user = &info->tf_pkt.dep_user[1];
	tbl->byte10 =
	cpu_to_le32(SET_WORD(dep_user->pref_AC, FWCMD_F2PTEST_PREF_AC));

	dep_user = &info->tf_pkt.dep_user[2];
	tbl->byte11 =
	cpu_to_le32(SET_WORD(dep_user->pref_AC, FWCMD_F2PTEST_PREF_AC));

	dep_user = &info->tf_pkt.dep_user[3];
	tbl->byte12 =
	cpu_to_le32(SET_WORD(dep_user->pref_AC, FWCMD_F2PTEST_PREF_AC));

	tbl->dword13 =
	cpu_to_le32(SET_WORD(info->tf_wd.datarate, FWCMD_F2PTEST_DATARATE) |
		    SET_WORD(info->tf_wd.mulport_id, FWCMD_F2PTEST_MULPORT) |
		    SET_WORD(info->tf_wd.pwr_ofset, FWCMD_F2PTEST_PWR_OFSET) |
		    SET_WORD(info->mode, FWCMD_F2PTEST_MODE) |
		    SET_WORD(info->frexch_type, FWCMD_F2PTEST_TYPE) |
		    SET_WORD(info->sigb_len, FWCMD_F2PTEST_SIGB_LEN));

	tbl->dword14 =
	cpu_to_le32(SET_WORD(f2pwd->cmd_qsel, F2P_WD_CMD_QSEL) |
		    (f2pwd->ls ? F2P_WD_LS : 0) |
		    (f2pwd->fs ? F2P_WD_FS : 0) |
		    SET_WORD(f2pwd->total_number, F2P_WD_TOTAL_NUMBER) |
		    SET_WORD(f2pwd->seq, F2P_WD_SEQ) |
		    SET_WORD(f2pwd->length, F2P_WD_LENGTH));

	tbl->dword16 =
	cpu_to_le32(SET_WORD(ptxcmd->cmd_type, F2P_CMD_TYPE) |
		    SET_WORD(ptxcmd->cmd_sub_type, F2P_CMD_SUB_TYPE) |
		    SET_WORD(ptxcmd->dl_user_num, F2P_DL_USER_NUM) |
		    SET_WORD(ptxcmd->bw, F2P_BW) |
		    SET_WORD(ptxcmd->tx_power, F2P_TX_POWER));

	tbl->dword17 =
	cpu_to_le32(SET_WORD(ptxcmd->fw_define, F2P_FW_DEFINE) |
		    SET_WORD(ptxcmd->ss_sel_mode, F2P_SS_SEL_MODE) |
		    SET_WORD(ptxcmd->next_qsel, F2P_NEXT_QSEL) |
		    SET_WORD(ptxcmd->twt_group, F2P_TWT_GROUP) |
		    (ptxcmd->dis_chk_slp ? F2P_DIS_CHK_SLP : 0) |
		    (ptxcmd->ru_mu_2_su ? F2P_RU_MU_2_SU : 0) |
		    SET_WORD(ptxcmd->dl_t_pe, F2P_DL_T_PE));

	tbl->dword18 =
	cpu_to_le32(SET_WORD(ptxcmd->sigb_ch1_len, F2P_SIGB_CH1_LEN) |
		    SET_WORD(ptxcmd->sigb_ch2_len, F2P_SIGB_CH2_LEN) |
		    SET_WORD(ptxcmd->sigb_sym_num, F2P_SIGB_SYM_NUM) |
		    SET_WORD(ptxcmd->sigb_ch2_ofs, F2P_SIGB_CH2_OFS) |
		    (ptxcmd->dis_htp_ack ? F2P_DIS_HTP_ACK : 0) |
		    SET_WORD(ptxcmd->tx_time_ref, F2P_TX_TIME_REF) |
		    SET_WORD(ptxcmd->pri_user_idx, F2P_PRI_USER_IDX));

	tbl->dword19 =
	cpu_to_le32(SET_WORD(ptxcmd->ampdu_max_txtime, F2P_AMPDU_MAX_TXTIME) |
		    SET_WORD(ptxcmd->group_id, F2P_GROUP_ID) |
		    (ptxcmd->twt_chk_en ? F2P_TWT_CHK_EN : 0) |
		    SET_WORD(ptxcmd->twt_port_id, F2P_TWT_PORT_ID));

	tbl->dword20 =
	cpu_to_le32(SET_WORD(ptxcmd->twt_start_time, F2P_TWT_START_TIME));

	tbl->dword21 =
	cpu_to_le32(SET_WORD(ptxcmd->twt_end_time, F2P_TWT_END_TIME));

	tbl->dword22 =
	cpu_to_le32(SET_WORD(ptxcmd->apep_len, F2P_APEP_LEN) |
		    SET_WORD(ptxcmd->tri_pad, F2P_TRI_PAD) |
		    SET_WORD(ptxcmd->ul_t_pe, F2P_UL_T_PE) |
		    SET_WORD(ptxcmd->rf_gain_idx, F2P_RF_GAIN_IDX) |
		    (ptxcmd->fixed_gain_en ? F2P_FIXED_GAIN_EN : 0) |
		    SET_WORD(ptxcmd->ul_gi_ltf, F2P_UL_GI_LTF) |
		    (ptxcmd->ul_doppler ? F2P_UL_DOPPLER : 0) |
		    (ptxcmd->ul_stbc ? F2P_UL_STBC : 0));

	tbl->dword23 =
	cpu_to_le32((ptxcmd->ul_mid_per ? F2P_UL_MID_PER : 0) |
		    (ptxcmd->ul_cqi_rrp_tri ? F2P_UL_CQI_RRP_TRI : 0) |
		    (ptxcmd->sigb_dcm ? F2P_SIGB_DCM : 0) |
		    (ptxcmd->sigb_comp ? F2P_SIGB_COMP : 0) |
		    (ptxcmd->doppler ? F2P_DOPPLER : 0) |
		    (ptxcmd->stbc ? F2P_STBC : 0) |
		    (ptxcmd->mid_per ? F2P_MID_PER : 0) |
		    SET_WORD(ptxcmd->gi_ltf_size, F2P_GI_LTF_SIZE) |
		    SET_WORD(ptxcmd->sigb_mcs, F2P_SIGB_MCS));

	tbl->dword24 =
	cpu_to_le32(SET_WORD(ptxcmd->macid_u0, F2P_MACID_U0) |
		    SET_WORD(ptxcmd->ac_type_u0, F2P_AC_TYPE_U0) |
		    SET_WORD(ptxcmd->mu_sta_pos_u0, F2P_MU_STA_POS_U0) |
		    SET_WORD(ptxcmd->dl_rate_idx_u0, F2P_DL_RATE_IDX_U0) |
		    (ptxcmd->dl_dcm_en_u0 ? F2P_TX_CMD_DL_DCM_EN_U0 : 0) |
		    SET_WORD(ptxcmd->ru_alo_idx_u0, F2P_RU_ALO_IDX_U0) |
		    (ptxcmd->rsvd6 ? BIT(22) : 0));

	tbl->dword25 =
	cpu_to_le32(SET_WORD(ptxcmd->pwr_boost_u0, F2P_PWR_BOOST_U0) |
		    SET_WORD(ptxcmd->agg_bmp_alo_u0, F2P_AGG_BMP_ALO_U0) |
		    SET_WORD(ptxcmd->ampdu_max_txnum_u0, F2P_AMPDU_MAX_NUM_U0) |
		    SET_WORD(ptxcmd->user_define_u0, F2P_USER_DEFINE_U0) |
		    SET_WORD(ptxcmd->user_define_ext_u0, F2P_USER_DEFINE_EXT_U0)
		    );

	tbl->dword26 =
	cpu_to_le32(SET_WORD(ptxcmd->ul_addr_idx_u0, F2P_UL_ADDR_IDX_U0) |
		    (ptxcmd->ul_dcm_u0 ? F2P_UL_DCM_U0 : 0) |
		    (ptxcmd->ul_fec_cod_u0 ? F2P_UL_FEC_COD_U0 : 0) |
		    SET_WORD(ptxcmd->ul_ru_rate_u0, F2P_UL_RU_RATE_U0) |
		    SET_WORD(ptxcmd->ul_ru_alo_idx_u0, F2P_UL_RU_ALO_IDX_U0));

	tbl->dword28 =
	cpu_to_le32(SET_WORD(ptxcmd->macid_u1, F2P_MACID_U1) |
		    SET_WORD(ptxcmd->ac_type_u1, F2P_AC_TYPE_U1) |
		    SET_WORD(ptxcmd->mu_sta_pos_u1, F2P_MU_STA_POS_U1) |
		    SET_WORD(ptxcmd->dl_rate_idx_u1, F2P_DL_RATE_IDX_U1) |
		    (ptxcmd->dl_dcm_en_u1 ? F2P_TX_CMD_DL_DCM_EN_U1 : 0) |
		    SET_WORD(ptxcmd->ru_alo_idx_u1, F2P_RU_ALO_IDX_U1) |
		    (ptxcmd->rsvd10 ? BIT(22) : 0));

	tbl->dword29 =
	cpu_to_le32(SET_WORD(ptxcmd->pwr_boost_u1, F2P_PWR_BOOST_U1) |
		    SET_WORD(ptxcmd->agg_bmp_alo_u1, F2P_AGG_BMP_ALO_U1) |
		    SET_WORD(ptxcmd->ampdu_max_txnum_u1, F2P_AMPDU_MAX_NUM_U1) |
		    SET_WORD(ptxcmd->user_define_u1, F2P_USER_DEFINE_U1) |
		    SET_WORD(ptxcmd->user_define_ext_u1, F2P_USER_DEFINE_EXT_U1)
		    );

	tbl->dword30 =
	cpu_to_le32(SET_WORD(ptxcmd->ul_addr_idx_u1, F2P_UL_ADDR_IDX_U1) |
		    (ptxcmd->ul_dcm_u1 ? F2P_UL_DCM_U1 : 0) |
		    (ptxcmd->ul_fec_cod_u1 ? F2P_UL_FEC_COD_U1 : 0) |
		    SET_WORD(ptxcmd->ul_ru_rate_u1, F2P_UL_RU_RATE_U1) |
		    SET_WORD(ptxcmd->ul_ru_alo_idx_u1, F2P_UL_RU_ALO_IDX_U1));

	tbl->dword32 =
	cpu_to_le32(SET_WORD(ptxcmd->macid_u2, F2P_MACID_U2) |
		    SET_WORD(ptxcmd->ac_type_u2, F2P_AC_TYPE_U2) |
		    SET_WORD(ptxcmd->mu_sta_pos_u2, F2P_MU_STA_POS_U2) |
		    SET_WORD(ptxcmd->dl_rate_idx_u2, F2P_DL_RATE_IDX_U2) |
		    (ptxcmd->dl_dcm_en_u2 ? F2P_TX_CMD_DL_DCM_EN_U2 : 0) |
		    SET_WORD(ptxcmd->ru_alo_idx_u2, F2P_RU_ALO_IDX_U2) |
		    (ptxcmd->rsvd14 ? BIT(22) : 0));

	tbl->dword33 =
	cpu_to_le32(SET_WORD(ptxcmd->pwr_boost_u2, F2P_PWR_BOOST_U2) |
		    SET_WORD(ptxcmd->agg_bmp_alo_u2, F2P_AGG_BMP_ALO_U2) |
		    SET_WORD(ptxcmd->ampdu_max_txnum_u2, F2P_AMPDU_MAX_NUM_U2) |
		    SET_WORD(ptxcmd->user_define_u2, F2P_USER_DEFINE_U2) |
		    SET_WORD(ptxcmd->user_define_ext_u2, F2P_USER_DEFINE_EXT_U2)
		    );

	tbl->dword34 =
	cpu_to_le32(SET_WORD(ptxcmd->ul_addr_idx_u2, F2P_UL_ADDR_IDX_U2) |
		    (ptxcmd->ul_dcm_u2 ? F2P_UL_DCM_U2 : 0) |
		    (ptxcmd->ul_fec_cod_u2 ? F2P_UL_FEC_COD_U2 : 0) |
		    SET_WORD(ptxcmd->ul_ru_rate_u2, F2P_UL_RU_RATE_U2) |
		    SET_WORD(ptxcmd->ul_ru_alo_idx_u2, F2P_UL_RU_ALO_IDX_U2));

	tbl->dword36 =
	cpu_to_le32(SET_WORD(ptxcmd->macid_u3, F2P_MACID_U3) |
		    SET_WORD(ptxcmd->ac_type_u3, F2P_AC_TYPE_U3) |
		    SET_WORD(ptxcmd->mu_sta_pos_u3, F2P_MU_STA_POS_U3) |
		    SET_WORD(ptxcmd->dl_rate_idx_u3, F2P_DL_RATE_IDX_U3) |
		    (ptxcmd->dl_dcm_en_u3 ? F2P_TX_CMD_DL_DCM_EN_U3 : 0) |
		    SET_WORD(ptxcmd->ru_alo_idx_u3, F2P_RU_ALO_IDX_U3) |
		    (ptxcmd->rsvd18 ? BIT(22) : 0));

	tbl->dword37 =
	cpu_to_le32(SET_WORD(ptxcmd->pwr_boost_u3, F2P_PWR_BOOST_U3) |
		    SET_WORD(ptxcmd->agg_bmp_alo_u3, F2P_AGG_BMP_ALO_U3) |
		    SET_WORD(ptxcmd->ampdu_max_txnum_u3, F2P_AMPDU_MAX_NUM_U3) |
		    SET_WORD(ptxcmd->user_define_u3, F2P_USER_DEFINE_U3) |
		    SET_WORD(ptxcmd->user_define_ext_u3, F2P_USER_DEFINE_EXT_U3)
		    );

	tbl->dword38 =
	cpu_to_le32(SET_WORD(ptxcmd->ul_addr_idx_u3, F2P_UL_ADDR_IDX_U3) |
		    (ptxcmd->ul_dcm_u3 ? F2P_UL_DCM_U3 : 0) |
		    (ptxcmd->ul_fec_cod_u3 ? F2P_UL_FEC_COD_U3 : 0) |
		    SET_WORD(ptxcmd->ul_ru_rate_u3, F2P_UL_RU_RATE_U3) |
		    SET_WORD(ptxcmd->ul_ru_alo_idx_u3, F2P_UL_RU_ALO_IDX_U3));

	tbl->dword40 =
	cpu_to_le32(SET_WORD(ptxcmd->pkt_id_0, F2P_PKT_ID_0) |
		    (ptxcmd->valid_0 ? F2P_VALID_0 : 0) |
		    SET_WORD(ptxcmd->ul_user_num_0, F2P_UL_USER_NUM_0));

	tbl->dword41 =
	cpu_to_le32(SET_WORD(ptxcmd->pkt_id_1, F2P_PKT_ID_1) |
		    (ptxcmd->valid_1 ? F2P_VALID_1 : 0) |
		    SET_WORD(ptxcmd->ul_user_num_1, F2P_UL_USER_NUM_1));

	tbl->dword42 =
	cpu_to_le32(SET_WORD(ptxcmd->pkt_id_2, F2P_PKT_ID_2) |
		    (ptxcmd->valid_2 ? F2P_VALID_2 : 0) |
		    SET_WORD(ptxcmd->ul_user_num_2, F2P_UL_USER_NUM_2));

	tbl->dword43 =
	cpu_to_le32(SET_WORD(ptxcmd->pkt_id_3, F2P_PKT_ID_3) |
		    (ptxcmd->valid_3 ? F2P_VALID_3 : 0) |
		    SET_WORD(ptxcmd->ul_user_num_3, F2P_UL_USER_NUM_3));

	tbl->dword44 =
	cpu_to_le32(SET_WORD(ptxcmd->pkt_id_4, F2P_PKT_ID_4) |
		    (ptxcmd->valid_4 ? F2P_VALID_4 : 0) |
		    SET_WORD(ptxcmd->ul_user_num_4, F2P_UL_USER_NUM_4));

	tbl->dword45 =
	cpu_to_le32(SET_WORD(ptxcmd->pkt_id_5, F2P_PKT_ID_5) |
		    (ptxcmd->valid_5 ? F2P_VALID_5 : 0) |
		    SET_WORD(ptxcmd->ul_user_num_5, F2P_UL_USER_NUM_5));

	PLTFM_MEMCPY(tbl->byte46, psigb_addr, info->sigb_len);

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

u32 mac_snd_test_cmd(struct mac_ax_adapter *adapter,
		     u8 *cmd_buf)
{
	return MACSUCCESS;
}

u32 mac_upd_dctl_info(struct mac_ax_adapter *adapter,
		      struct mac_ax_dctl_info *info,
		      struct mac_ax_dctl_info *mask, u8 macid, u8 operation)
{
	u32 ret = 0;
	u8 *buf;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	struct fwcmd_dctlinfo_ud *tbl;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_CMD);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, sizeof(struct fwcmd_dctlinfo_ud));
	if (!buf) {
		ret = MACNOBUF;
		goto fail;
	}

	tbl = (struct fwcmd_dctlinfo_ud *)buf;
	tbl->dword0 =
	cpu_to_le32(SET_WORD(macid, FWCMD_H2C_DCTLINFO_UD_MACID) |
		    (operation ? FWCMD_H2C_DCTLINFO_UD_OP : 0));

	tbl->dword1 =
	cpu_to_le32(SET_WORD(info->qos_field_h, FWCMD_H2C_DCTRL_QOS_FIELD_H) |
		    SET_WORD(info->hw_exseq_macid, FWCMD_H2C_DCTRL_HW_EXSEQ_MACID) |
		    (info->qos_field_h_en ? FWCMD_H2C_DCTRL_QOS_FIELD_H_EN : 0) |
		    SET_WORD(info->aes_iv_l, FWCMD_H2C_DCTRL_AES_IV_L));

	tbl->dword2 =
	cpu_to_le32(SET_WORD(info->aes_iv_h, FWCMD_H2C_DCTRL_AES_IV_H));

	tbl->dword3 =
	cpu_to_le32(SET_WORD(info->seq0, FWCMD_H2C_DCTRL_SEQ0) |
		    SET_WORD(info->seq1, FWCMD_H2C_DCTRL_SEQ1) |
		    SET_WORD(info->amsdu_max_length, FWCMD_H2C_DCTRL_AMSDU_MAX_LEN) |
		    (info->sta_amsdu_en ? FWCMD_H2C_DCTRL_STA_AMSDU_EN : 0) |
		    (info->chksum_offload_en ? FWCMD_H2C_DCTRL_CHKSUM_OFLD_EN : 0) |
		    (info->with_llc ? FWCMD_H2C_DCTRL_WITH_LLC : 0) |
		    (info->sec_hw_enc ? FWCMD_H2C_DCTRL_SEC_HW_ENC : 0));

	tbl->dword4 =
	cpu_to_le32(SET_WORD(info->seq2, FWCMD_H2C_DCTRL_SEQ2) |
		    SET_WORD(info->seq3, FWCMD_H2C_DCTRL_SEQ3) |
		    SET_WORD(info->sec_cam_idx, FWCMD_H2C_DCTRL_SEC_CAM_IDX));

	tbl->dword5 =
	cpu_to_le32(SET_WORD(mask->qos_field_h, FWCMD_H2C_DCTRL_QOS_FIELD_H) |
		    SET_WORD(mask->hw_exseq_macid, FWCMD_H2C_DCTRL_HW_EXSEQ_MACID) |
		    (mask->qos_field_h_en ? FWCMD_H2C_DCTRL_QOS_FIELD_H_EN : 0) |
		    SET_WORD(mask->aes_iv_l, FWCMD_H2C_DCTRL_AES_IV_L));

	tbl->dword6 =
	cpu_to_le32(SET_WORD(mask->aes_iv_h, FWCMD_H2C_DCTRL_AES_IV_H));

	tbl->dword7 =
	cpu_to_le32(SET_WORD(mask->seq0, FWCMD_H2C_DCTRL_SEQ0) |
		    SET_WORD(mask->seq1, FWCMD_H2C_DCTRL_SEQ1) |
		    SET_WORD(mask->amsdu_max_length, FWCMD_H2C_DCTRL_AMSDU_MAX_LEN) |
		    (mask->sta_amsdu_en ? FWCMD_H2C_DCTRL_STA_AMSDU_EN : 0) |
		    (mask->chksum_offload_en ? FWCMD_H2C_DCTRL_CHKSUM_OFLD_EN : 0) |
		    (mask->with_llc ? FWCMD_H2C_DCTRL_WITH_LLC : 0) |
		    (mask->sec_hw_enc ? FWCMD_H2C_DCTRL_SEC_HW_ENC : 0));

	tbl->dword8 =
	cpu_to_le32(SET_WORD(mask->seq2, FWCMD_H2C_DCTRL_SEQ2) |
		    SET_WORD(mask->seq3, FWCMD_H2C_DCTRL_SEQ3) |
		    SET_WORD(mask->sec_cam_idx, FWCMD_H2C_DCTRL_SEC_CAM_IDX));

	if (adapter->sm.fwdl == MAC_AX_FWDL_INIT_RDY) {
		ret = h2c_pkt_set_hdr(adapter, h2cb,
				      FWCMD_TYPE_H2C,
				      FWCMD_H2C_CAT_MAC,
				      FWCMD_H2C_CL_FR_EXCHG,
				      FWCMD_H2C_FUNC_DCTLINFO_UD,
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
			dctl_info_debug_write(adapter, macid,
					      (struct fwcmd_dctlinfo_ud *)buf);
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

u32 mac_upd_shcut_mhdr(struct mac_ax_adapter *adapter,
		       struct mac_ax_shcut_mhdr *info, u8 shcut_camid)
{
	u32 ret = 0;
	u32 i;
	u8 *buf;
	u32 *src, *dest;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	struct fwcmd_shcut_update *tbl;

	if (!(is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
	      is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
	      is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)))
		return MACNOTSUP;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_DATA);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, sizeof(struct fwcmd_shcut_update));
	if (!buf) {
		ret = MACNOBUF;
		goto fail;
	}

	tbl = (struct fwcmd_shcut_update *)buf;
	tbl->dword0 =
	cpu_to_le32(SET_WORD(shcut_camid, FWCMD_H2C_DCTLINFO_UD_MACID));
	src = (u32 *)info;
	dest = (u32 *)(&tbl->dword1);
	for (i = 0; i < (sizeof(struct mac_ax_shcut_mhdr) / 4); i++)
		*(dest++) = *(src++);

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C,
			      FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_FR_EXCHG,
			      FWCMD_H2C_FUNC_SHCUT_UPDATE,
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

void _set_role_cctrl(struct mac_ax_adapter *adapter,
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
	cctrl->sectype = (cctrl->bk_lftime_sel & ~mask->sectype) |
			(mask->bk_lftime_sel & info->sectype);

	cctrl->multi_port_id = (cctrl->multi_port_id & ~mask->multi_port_id) |
			(mask->multi_port_id & info->multi_port_id);
	cctrl->bmc = (cctrl->multi_port_id & ~mask->bmc) |
			(mask->multi_port_id & info->bmc);
	cctrl->mbssid = (cctrl->mbssid & ~mask->mbssid) |
			(mask->mbssid & info->mbssid);
	cctrl->navusehdr = (cctrl->navusehdr & ~mask->navusehdr) |
			(mask->navusehdr & info->navusehdr);
	cctrl->txpwr_mode = (cctrl->txpwr_mode & ~mask->txpwr_mode) |
			(mask->txpwr_mode & info->txpwr_mode);
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
	cctrl->ntx_path_en = (cctrl->ntx_path_en & ~mask->ntx_path_en) |
			(mask->ntx_path_en & info->ntx_path_en);
	cctrl->path_map_a = (cctrl->ntx_path_en & ~mask->path_map_a) |
			(mask->ntx_path_en & info->path_map_a);
	cctrl->path_map_b = (cctrl->path_map_b & ~mask->path_map_b) |
			(mask->path_map_b & info->path_map_b);
	cctrl->path_map_c = (cctrl->path_map_c & ~mask->path_map_c) |
			(mask->path_map_c & info->path_map_c);
	cctrl->path_map_d = (cctrl->path_map_d & ~mask->path_map_d) |
			(mask->path_map_d & info->path_map_d);
	cctrl->antsel_a = (cctrl->antsel_a & ~mask->antsel_a) |
			(mask->antsel_a & info->antsel_a);
	cctrl->antsel_b = (cctrl->antsel_b & ~mask->antsel_b) |
			(mask->antsel_b & info->antsel_b);
	cctrl->antsel_c = (cctrl->antsel_c & ~mask->antsel_c) |
			(mask->antsel_c & info->antsel_c);
	cctrl->antsel_d = (cctrl->antsel_d & ~mask->antsel_d) |
			(mask->antsel_d & info->antsel_d);

	cctrl->addr_cam_index = (cctrl->addr_cam_index &
				 ~mask->addr_cam_index) |
			(mask->addr_cam_index & info->addr_cam_index);
	cctrl->paid = (cctrl->paid & ~mask->paid) |
			(mask->paid & info->paid);
	cctrl->uldl = (cctrl->uldl & ~mask->uldl) |
			(mask->uldl & info->uldl);
	cctrl->doppler_ctrl = (cctrl->doppler_ctrl & ~mask->doppler_ctrl) |
			(mask->doppler_ctrl & info->doppler_ctrl);
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
	cctrl->csi_gi_ltf = (cctrl->csi_gi_ltf & ~mask->csi_gi_ltf) |
			(mask->csi_gi_ltf & info->csi_gi_ltf);
	cctrl->nominal_pkt_padding160 =
			(cctrl->nominal_pkt_padding160 &
			 ~mask->nominal_pkt_padding160) |
			(mask->nominal_pkt_padding160 &
			 info->nominal_pkt_padding160);
	cctrl->csi_bw = (cctrl->csi_bw & ~mask->csi_bw) |
			(mask->csi_bw & info->csi_bw);
}

void mac_upd_role_cctrl(struct mac_ax_adapter *adapter,
			struct rtw_hal_mac_ax_cctl_info *info,
			struct rtw_hal_mac_ax_cctl_info *mask, u8 macid)
{
	struct mac_role_tbl *role;

	role = mac_role_srch(adapter, macid);
	if (!role) {
		PLTFM_MSG_ERR("role search failed\n");
		return;
	}
	_set_role_cctrl(adapter, info, mask, &role->info.c_info);
}

u32 mac_tx_path_map_cfg(struct mac_ax_adapter *adapter, struct hal_txmap_cfg *cfg)
{
	struct mac_ax_ops *mops = adapter_to_mac_ops(adapter);
	struct rtw_hal_mac_ax_cctl_info info;
	struct rtw_hal_mac_ax_cctl_info mask;
	u32 ret;

	PLTFM_MEMSET(&mask, 0, sizeof(struct rtw_hal_mac_ax_cctl_info));
	PLTFM_MEMSET(&info, 0, sizeof(struct rtw_hal_mac_ax_cctl_info));

	info.ntx_path_en = cfg->n_tx_en;
	info.path_map_a = cfg->map_a;
	info.path_map_b = cfg->map_b;
	info.path_map_c = cfg->map_c;
	info.path_map_d = cfg->map_d;

	mask.ntx_path_en = NTX_PATH_EN_MASK;
	mask.path_map_a = PATH_MAP_MASK;
	mask.path_map_b = PATH_MAP_MASK;
	mask.path_map_c = PATH_MAP_MASK;
	mask.path_map_d = PATH_MAP_MASK;

	ret = mops->upd_cctl_info(adapter, &info, &mask, (u8)cfg->macid, TBL_WRITE_OP);

	return MACSUCCESS;
}

u32 mac_upd_cctl_info(struct mac_ax_adapter *adapter,
		      struct rtw_hal_mac_ax_cctl_info *info,
		      struct rtw_hal_mac_ax_cctl_info *mask, u8 macid, u8 operation)
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
	cpu_to_le32(SET_WORD(info->data_tx_cnt_lmt, FWCMD_H2C_CCTRL_DATA_TX_CNT_LMT) |
		    (info->data_txcnt_lmt_sel ? FWCMD_H2C_CCTRL_DATA_TXCNT_LMT_SEL :
		     0) |
		    (info->max_agg_num_sel ? FWCMD_H2C_CCTRL_MAX_AGG_NUM_SEL : 0) |
		    (info->rts_en ? FWCMD_H2C_CCTRL_RTS_EN : 0) |
		    (info->cts2self_en ? FWCMD_H2C_CCTRL_CTS2SELF_EN : 0) |
		    SET_WORD(info->cca_rts, FWCMD_H2C_CCTRL_CCA_RTS) |
		    (info->hw_rts_en ? FWCMD_H2C_CCTRL_HW_RTS_EN : 0) |
		    SET_WORD(info->rts_drop_data_mode,
			     FWCMD_H2C_CCTRL_RTS_DROP_DATA_MODE) |
		    (info->preld_en ? FWCMD_H2C_CCTRL_PRELD_EN : 0) |
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
		    SET_WORD(info->bk_lftime_sel, FWCMD_H2C_CCTRL_BK_LFTIME_SEL) |
		    SET_WORD(info->sectype, FWCMD_H2C_CCTRL_SECTYPE));

	tbl->dword5 =
	cpu_to_le32(SET_WORD(info->multi_port_id, FWCMD_H2C_CCTRL_MULTI_PORT_ID) |
		    (info->bmc ? FWCMD_H2C_CCTRL_BMC : 0) |
		    SET_WORD(info->mbssid, FWCMD_H2C_CCTRL_MBSSID) |
		    (info->navusehdr ? FWCMD_H2C_CCTRL_NAVUSEHDR : 0) |
		    SET_WORD(info->txpwr_mode, FWCMD_H2C_CCTRL_TXPWR_MODE) |
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
		    SET_WORD(info->ntx_path_en, FWCMD_H2C_CCTRL_NTX_PATH_EN) |
		    SET_WORD(info->path_map_a, FWCMD_H2C_CCTRL_PATH_MAP_A) |
		    SET_WORD(info->path_map_b, FWCMD_H2C_CCTRL_PATH_MAP_B) |
		    SET_WORD(info->path_map_c, FWCMD_H2C_CCTRL_PATH_MAP_C) |
		    SET_WORD(info->path_map_d, FWCMD_H2C_CCTRL_PATH_MAP_D) |
		    (info->antsel_a ? FWCMD_H2C_CCTRL_ANTSEL_A : 0) |
		    (info->antsel_b ? FWCMD_H2C_CCTRL_ANTSEL_B : 0) |
		    (info->antsel_c ? FWCMD_H2C_CCTRL_ANTSEL_C : 0) |
		    (info->antsel_d ? FWCMD_H2C_CCTRL_ANTSEL_D : 0));

	tbl->dword7 =
	cpu_to_le32(SET_WORD(info->addr_cam_index, FWCMD_H2C_CCTRL_ADDR_CAM_INDEX) |
		    SET_WORD(info->paid, FWCMD_H2C_CCTRL_PAID) |
		    (info->uldl ? FWCMD_H2C_CCTRL_ULDL : 0) |
		    SET_WORD(info->doppler_ctrl, FWCMD_H2C_CCTRL_DOPPLER_CTRL) |
		    SET_WORD(info->nominal_pkt_padding,
			     FWCMD_H2C_CCTRL_NOMINAL_PKT_PADDING) |
		    SET_WORD(info->nominal_pkt_padding40,
			     FWCMD_H2C_CCTRL_NOMINAL_PKT_PADDING40) |
		    SET_WORD(info->nominal_pkt_padding80,
			     FWCMD_H2C_CCTRL_NOMINAL_PKT_PADDING80) |
		    SET_WORD(info->txpwr_tolerence, FWCMD_H2C_CCTRL_TXPWR_TOLERENCE));

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
		    SET_WORD(info->csi_gi_ltf, FWCMD_H2C_CCTRL_CSI_GI_LTF) |
		    SET_WORD(info->nominal_pkt_padding160,
			     FWCMD_H2C_CCTRL_NOMINAL_PKT_PADDING160) |
		    SET_WORD(info->csi_bw, FWCMD_H2C_CCTRL_CSI_BW));

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
	cpu_to_le32(SET_WORD(mask->data_tx_cnt_lmt, FWCMD_H2C_CCTRL_DATA_TX_CNT_LMT) |
		    (mask->data_txcnt_lmt_sel ? FWCMD_H2C_CCTRL_DATA_TXCNT_LMT_SEL :
		     0) |
		    (mask->max_agg_num_sel ? FWCMD_H2C_CCTRL_MAX_AGG_NUM_SEL : 0) |
		    (mask->rts_en ? FWCMD_H2C_CCTRL_RTS_EN : 0) |
		    (mask->cts2self_en ? FWCMD_H2C_CCTRL_CTS2SELF_EN : 0) |
		    SET_WORD(mask->cca_rts, FWCMD_H2C_CCTRL_CCA_RTS) |
		    (mask->hw_rts_en ? FWCMD_H2C_CCTRL_HW_RTS_EN : 0) |
		    SET_WORD(mask->rts_drop_data_mode,
			     FWCMD_H2C_CCTRL_RTS_DROP_DATA_MODE) |
		    (mask->preld_en ? FWCMD_H2C_CCTRL_PRELD_EN : 0) |
		    SET_WORD(mask->ampdu_max_len, FWCMD_H2C_CCTRL_AMPDU_MAX_LEN) |
		    (mask->ul_mu_dis ? FWCMD_H2C_CCTRL_UL_MU_DIS : 0) |
		    SET_WORD(mask->ampdu_max_time, FWCMD_H2C_CCTRL_AMPDU_MAX_TIME));

	tbl->dword12 =
	cpu_to_le32(SET_WORD(mask->max_agg_num, FWCMD_H2C_CCTRL_MAX_AGG_NUM) |
		    SET_WORD(mask->ba_bmap, FWCMD_H2C_CCTRL_BA_BMAP) |
		    SET_WORD(mask->vo_lftime_sel, FWCMD_H2C_CCTRL_VO_LFTIME_SEL) |
		    SET_WORD(mask->vi_lftime_sel, FWCMD_H2C_CCTRL_VI_LFTIME_SEL) |
		    SET_WORD(mask->be_lftime_sel, FWCMD_H2C_CCTRL_BE_LFTIME_SEL) |
		    SET_WORD(mask->bk_lftime_sel, FWCMD_H2C_CCTRL_BK_LFTIME_SEL) |
		    SET_WORD(mask->sectype, FWCMD_H2C_CCTRL_SECTYPE));

	tbl->dword13 =
	cpu_to_le32(SET_WORD(mask->multi_port_id, FWCMD_H2C_CCTRL_MULTI_PORT_ID) |
		    (mask->bmc ? FWCMD_H2C_CCTRL_BMC : 0) |
		    SET_WORD(mask->mbssid, FWCMD_H2C_CCTRL_MBSSID) |
		    (mask->navusehdr ? FWCMD_H2C_CCTRL_NAVUSEHDR : 0) |
		    SET_WORD(mask->txpwr_mode, FWCMD_H2C_CCTRL_TXPWR_MODE) |
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
		    SET_WORD(mask->ntx_path_en, FWCMD_H2C_CCTRL_NTX_PATH_EN) |
		    SET_WORD(mask->path_map_a, FWCMD_H2C_CCTRL_PATH_MAP_A) |
		    SET_WORD(mask->path_map_b, FWCMD_H2C_CCTRL_PATH_MAP_B) |
		    SET_WORD(mask->path_map_c, FWCMD_H2C_CCTRL_PATH_MAP_C) |
		    SET_WORD(mask->path_map_d, FWCMD_H2C_CCTRL_PATH_MAP_D) |
		    (mask->antsel_a ? FWCMD_H2C_CCTRL_ANTSEL_A : 0) |
		    (mask->antsel_b ? FWCMD_H2C_CCTRL_ANTSEL_B : 0) |
		    (mask->antsel_c ? FWCMD_H2C_CCTRL_ANTSEL_C : 0) |
		    (mask->antsel_d ? FWCMD_H2C_CCTRL_ANTSEL_D : 0));

	tbl->dword15 =
	cpu_to_le32(SET_WORD(mask->addr_cam_index, FWCMD_H2C_CCTRL_ADDR_CAM_INDEX) |
		    SET_WORD(mask->paid, FWCMD_H2C_CCTRL_PAID) |
		    (mask->uldl ? FWCMD_H2C_CCTRL_ULDL : 0) |
		    SET_WORD(mask->doppler_ctrl, FWCMD_H2C_CCTRL_DOPPLER_CTRL) |
		    SET_WORD(mask->nominal_pkt_padding,
			     FWCMD_H2C_CCTRL_NOMINAL_PKT_PADDING) |
		    SET_WORD(mask->nominal_pkt_padding40,
			     FWCMD_H2C_CCTRL_NOMINAL_PKT_PADDING40) |
		    SET_WORD(mask->nominal_pkt_padding80,
			     FWCMD_H2C_CCTRL_NOMINAL_PKT_PADDING80) |
		    SET_WORD(mask->txpwr_tolerence,
			     FWCMD_H2C_CCTRL_TXPWR_TOLERENCE));

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
		    SET_WORD(mask->csi_gi_ltf, FWCMD_H2C_CCTRL_CSI_GI_LTF) |
		    SET_WORD(mask->nominal_pkt_padding160,
			     FWCMD_H2C_CCTRL_NOMINAL_PKT_PADDING160) |
		    SET_WORD(mask->csi_bw, FWCMD_H2C_CCTRL_CSI_BW));

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
			cctl_info_debug_write(adapter, macid,
					      (struct fwcmd_cctlinfo_ud *)buf);
#else
		return MACFWNONRDY;
#endif
	}

	h2cb_free(adapter, h2cb);

	mac_upd_role_cctrl(adapter, info, mask, macid);

	return MACSUCCESS;
fail:
	h2cb_free(adapter, h2cb);

	return ret;
}

u32 mac_set_fixmode_mib(struct mac_ax_adapter *adapter,
			struct mac_ax_fixmode_para *info)
{
	u32 ret = 0;
	u8 *buf;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	struct fwcmd_fixmode_para_tblud *tbl;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_CMD);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, sizeof(struct fwcmd_fixmode_para_tblud));
	if (!buf) {
		ret = MACNOBUF;
		goto fail;
	}

	PLTFM_MEMSET(buf, 0, sizeof(struct fwcmd_fixmode_para_tblud));

	tbl = (struct fwcmd_fixmode_para_tblud *)buf;
	tbl->dword0 =
	cpu_to_le32((info->tbl_hdr.rw ? FWCMD_H2C_TBLUD_R_W : 0) |
		    SET_WORD(info->tbl_hdr.idx, FWCMD_H2C_TBLUD_MACID_GROUP) |
		    SET_WORD(info->tbl_hdr.offset, FWCMD_H2C_TBLUD_OFFSET) |
		    SET_WORD(info->tbl_hdr.len, FWCMD_H2C_TBLUD_LENGTH) |
		    (info->tbl_hdr.type ? FWCMD_H2C_TBLUD_TYPE : 0) |
		    SET_WORD(CLASS_F2P_FIXMODE_PARA,
			     FWCMD_H2C_TBLUD_TABLE_CLASS));

	tbl->dword1 =
	cpu_to_le32((info->force_sumuru_en ?
		     FWCMD_H2C_FIXMODE_PARA_FORCE_SUMURU_EN : 0) |
		    (info->forcesu ?
		     FWCMD_H2C_FIXMODE_PARA_FORCESU : 0) |
		    (info->forcemu ?
		     FWCMD_H2C_FIXMODE_PARA_FORCEMU : 0) |
		    (info->forceru ?
		     FWCMD_H2C_FIXMODE_PARA_FORCERU : 0) |
		    (info->fix_fe_su_en ?
		     FWCMD_H2C_FIXMODE_PARA_FIX_FE_SU_EN : 0) |
		    (info->fix_fe_vhtmu_en ?
		     FWCMD_H2C_FIXMODE_PARA_FIX_FE_VHTMU_EN : 0) |
		    (info->fix_fe_hemu_en ?
		     FWCMD_H2C_FIXMODE_PARA_FIX_FE_HEMU_EN : 0) |
		    (info->fix_fe_heru_en ?
		     FWCMD_H2C_FIXMODE_PARA_FIX_FE_HERU_EN : 0) |
		    (info->fix_fe_ul_en ?
		     FWCMD_H2C_FIXMODE_PARA_FIX_FE_UL_EN : 0) |
		    (info->fix_frame_seq_su ?
		     FWCMD_H2C_FIXMODE_PARA_FIX_FRAME_SEQ_SU : 0) |
		    (info->fix_frame_seq_vhtmu ?
		     FWCMD_H2C_FIXMODE_PARA_FIX_FRAME_SEQ_VHTMU : 0) |
		    (info->fix_frame_seq_hemu ?
		     FWCMD_H2C_FIXMODE_PARA_FIX_FRAME_SEQ_HEMU : 0) |
		    (info->fix_frame_seq_heru ?
		     FWCMD_H2C_FIXMODE_PARA_FIX_FRAME_SEQ_HERU : 0) |
		    (info->fix_frame_seq_ul ?
		     FWCMD_H2C_FIXMODE_PARA_FIX_FRAME_SEQ_UL : 0) |
		    (info->is_dlruhwgrp ?
		     FWCMD_H2C_FIXMODE_PARA_IS_DLRUHWGRP : 0) |
		    (info->is_ulruhwgrp ?
		     FWCMD_H2C_FIXMODE_PARA_IS_ULRUHWGRP : 0) |
		    SET_WORD(info->prot_type_su,
			     FWCMD_H2C_FIXMODE_PARA_PROT_TYPE_SU) |
		    SET_WORD(info->prot_type_vhtmu,
			     FWCMD_H2C_FIXMODE_PARA_PROT_TYPE_VHTMU) |
		    SET_WORD(info->resp_type_vhtmu,
			     FWCMD_H2C_FIXMODE_PARA_RESP_TYPE_VHTMU) |
		    SET_WORD(info->prot_type_hemu,
			     FWCMD_H2C_FIXMODE_PARA_PROT_TYPE_HEMU));

	tbl->dword2 =
	cpu_to_le32(SET_WORD(info->resp_type_hemu,
			     FWCMD_H2C_FIXMODE_PARA_RESP_TYPE_HEMU) |
		    SET_WORD(info->prot_type_heru,
			     FWCMD_H2C_FIXMODE_PARA_PROT_TYPE_HERU) |
		    SET_WORD(info->resp_type_heru,
			     FWCMD_H2C_FIXMODE_PARA_RESP_TYPE_HERU) |
		    SET_WORD(info->ul_prot_type,
			     FWCMD_H2C_FIXMODE_PARA_UL_PROT_TYPE) |
		    SET_WORD(info->rugrpid,
			     FWCMD_H2C_FIXMODE_PARA_RUGRPID) |
		    SET_WORD(info->mugrpid,
			     FWCMD_H2C_FIXMODE_PARA_MUGRPID) |
		    SET_WORD(info->ulgrpid,
			     FWCMD_H2C_FIXMODE_PARA_ULGRPID));

	tbl->dword3 =
	cpu_to_le32((info->fix_txcmdnum_en ?
		     FWCMD_H2C_FIXMODE_PARA_FIX_TXCMDNUM_EN : 0) |
		    (info->force_to_one ?
		    FWCMD_H2C_FIXMODE_PARA_FORCE_TO_ONE : 0));

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C,
			      FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_FR_EXCHG,
			      FWCMD_H2C_FUNC_TBLUD,
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

u32 rst_bacam(struct mac_ax_adapter *adapter, struct rst_bacam_info *info)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32, cnt;

	val32 = MAC_REG_R32(R_AX_RESPBA_CAM_CTRL);
	switch (info->type) {
	case BACAM_RST_ALL:
		val32 = SET_CLR_WORD(val32, S_AX_BACAM_RST_ALL, B_AX_BACAM_RST);
		break;
	case BACAM_RST_ENT:
		val32 = SET_CLR_WORD(val32, info->ent, B_AX_SRC_ENTRY_IDX);
		MAC_REG_W32(R_AX_RESPBA_CAM_CTRL, val32);

		val32 = SET_CLR_WORD(val32, S_AX_BACAM_RST_ENT, B_AX_BACAM_RST);
		break;
	}
	MAC_REG_W32(R_AX_RESPBA_CAM_CTRL, val32);

	cnt = BACAM_RST_DLY_CNT;
	while (cnt) {
		val32 = MAC_REG_R32(R_AX_RESPBA_CAM_CTRL);
		if (GET_FIELD(val32, B_AX_BACAM_RST) == S_AX_BACAM_RST_DONE)
			break;
		PLTFM_DELAY_US(BACAM_RST_DLY_US);
		cnt--;
	}

	if (!cnt) {
		PLTFM_MSG_ERR("[ERR]bacam rst timeout %X\n", val32);
		return MACPOLLTO;
	}

	return MACSUCCESS;
}

u32 mac_bacam_avl_std_entry_idx(struct mac_ax_adapter *adapter,
				struct mac_ax_avl_std_bacam_info *info)
{
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
		PLTFM_MSG_ERR("[ERR] not support in this chip");
		return MACNOBUF;
	}

	#ifdef PHL_FEATURE_AP
		switch (adapter->hw_info->chip_id) {
		case MAC_AX_CHIP_ID_8852C:
			info->min_avl_idx = BACAM_INIT_TMP_ENTRY_NUM_AP_8852C;
			info->max_avl_idx = BACAM_MAX_ENTRY_IDX_8852C;
			break;
		case MAC_AX_CHIP_ID_8192XB:
			info->min_avl_idx = BACAM_INIT_TMP_ENTRY_NUM_AP_8192XB;
			info->max_avl_idx = BACAM_MAX_ENTRY_IDX_8192XB;
			break;
		case MAC_BE_CHIP_ID_1115E:// for Fool-proof mechanism
			info->min_avl_idx = BACAM_MIN_ENTRY_IDX_DEF_1115E;
			info->max_avl_idx = BACAM_MAX_ENTRY_IDX_DEF_1115E;
			return MACSUCCESS;
		default:
			PLTFM_MSG_ERR("[ERR] idx search fail");
			return MACNOBUF;
		}
		#ifdef PHL_FEATURE_NIC // for Fool-proof mechanism in hvtool
			info->min_avl_idx = info->min_avl_idx + BACAM_MAX_RU_SUPPORT_B1_STA;
		#endif
	#else // for NiC mode setting
		switch (adapter->hw_info->chip_id) {
		case MAC_AX_CHIP_ID_8852C:
			info->min_avl_idx = BACAM_INIT_TMP_ENTRY_NUM_STA_8852C;
			info->max_avl_idx = BACAM_MAX_ENTRY_IDX_8852C;
			break;
		case MAC_AX_CHIP_ID_8192XB:
			info->min_avl_idx = BACAM_INIT_TMP_ENTRY_NUM_STA_8192XB;
			info->max_avl_idx = BACAM_MAX_ENTRY_IDX_8192XB;
			break;
		case MAC_BE_CHIP_ID_1115E:// for Fool-proof mechanism
			info->min_avl_idx = BACAM_MIN_ENTRY_IDX_DEF_1115E;
			info->max_avl_idx = BACAM_MAX_ENTRY_IDX_DEF_1115E;
			break;
		default:
			PLTFM_MSG_ERR("[ERR] idx search fail");
			return MACNOBUF;
		}
	#endif
	return MACSUCCESS;
}

u32 mac_bacam_init(struct mac_ax_adapter *adapter)
{
	u32 ret = MACSUCCESS;
	u8 entry_num_b0 = 0, entry_num_b1 = 0, i;
	struct mac_ax_bacam_info info = { 0x0 };
	struct mac_ax_ops *mops = adapter_to_mac_ops(adapter);

	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B) ||
	    is_chip_id(adapter, MAC_BE_CHIP_ID_1115E))
		return ret;

#ifdef PHL_FEATURE_AP
	switch (adapter->hw_info->chip_id) {
	case MAC_AX_CHIP_ID_8852C:
		entry_num_b0 = BACAM_MAX_RU_SUPPORT_B0_AP_8852C;
		entry_num_b1 = BACAM_MAX_RU_SUPPORT_B1_AP_8852C;
		break;
	case MAC_AX_CHIP_ID_8192XB:
		entry_num_b0 = BACAM_MAX_RU_SUPPORT_B0_AP_8192XB;
		entry_num_b1 = BACAM_MAX_RU_SUPPORT_B1_AP_8192XB;
		break;
	default:
		PLTFM_MSG_ERR("[ERR] bacam init fail");
		return MACBADDR;
	}
	#ifdef PHL_FEATURE_NIC // for Fool-proof mechanism in hvtool
		entry_num_b1 = BACAM_MAX_RU_SUPPORT_B1_STA;
	#endif
#else // for NiC mode setting
	entry_num_b0 = BACAM_MAX_RU_SUPPORT_B0_STA;
	entry_num_b1 = BACAM_MAX_RU_SUPPORT_B1_STA;
#endif
	// set band 0 temp entry
	info.band_sel = 0;
	for (i = 0; i < entry_num_b0; i++) {
		info.entry_idx_v1 = i;
		info.uid_value = i;
		ret = mops->bacam_info(adapter, &info);
		if (ret)
			goto fail;
	}
	// set band 1 temp entry
	info.band_sel = 1;
	for (i = 0; i < entry_num_b1; i++) {
		info.entry_idx_v1 = entry_num_b0 + i;
		info.uid_value = i;
		ret = mops->bacam_info(adapter, &info);
		if (ret)
			goto fail;
	}

	return ret;

fail:
	PLTFM_MSG_ERR("[ERR]bacam init fail %d\n", ret);
	return ret;
}

u32 mac_bacam_info(struct mac_ax_adapter *adapter,
		   struct mac_ax_bacam_info *info)
{
	u32 ret = MACSUCCESS;
	u8 *buf;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	struct fwcmd_ba_cam *tbl;
	struct mac_ax_avl_std_bacam_info idx_info = {0x0};
	struct mac_ax_ops *mops = adapter_to_mac_ops(adapter);

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_CMD);
	if (!h2cb) {
		PLTFM_MSG_ERR("[ERR]h2cb_alloc\n");
		return MACNPTR;
	}

	buf = h2cb_put(h2cb, sizeof(struct fwcmd_ba_cam));
	if (!buf) {
		ret = MACNOBUF;
		PLTFM_MSG_ERR("[ERR]h2cb_put %d\n", ret);
		goto fail;
	}

	tbl = (struct fwcmd_ba_cam *)buf;

	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
		tbl->dword0 =
			cpu_to_le32((info->valid ? FWCMD_H2C_BA_CAM_VALID : 0) |
				    (info->init_req ? FWCMD_H2C_BA_CAM_INIT_REQ : 0) |
				    SET_WORD(info->entry_idx, FWCMD_H2C_BA_CAM_ENTRY_IDX) |
				    SET_WORD(info->tid, FWCMD_H2C_BA_CAM_TID) |
				    SET_WORD(info->macid, FWCMD_H2C_BA_CAM_MACID) |
				    SET_WORD(info->bmap_size, FWCMD_H2C_BA_CAM_BMAP_SIZE) |
				    SET_WORD(info->ssn, FWCMD_H2C_BA_CAM_SSN)
				    );
	} else {
		ret = mops->bacam_avl_std_entry_idx(adapter, &idx_info);
		if (ret)
			return ret;
		if (idx_info.max_avl_idx < info->entry_idx_v1) {
			ret = MACNOBUF;
			PLTFM_MSG_ERR("[ERR]out of idx %d\n", ret);
			goto fail;
		}
		tbl->dword0 =
			cpu_to_le32((info->valid ? FWCMD_H2C_BA_CAM_VALID : 0) |
				    (info->init_req ? FWCMD_H2C_BA_CAM_INIT_REQ : 0) |
				    SET_WORD(info->tid, FWCMD_H2C_BA_CAM_TID) |
				    SET_WORD(info->macid, FWCMD_H2C_BA_CAM_MACID) |
				    SET_WORD(info->bmap_size, FWCMD_H2C_BA_CAM_BMAP_SIZE) |
				    SET_WORD(info->ssn, FWCMD_H2C_BA_CAM_SSN)
				    );
		tbl->dword1 =
			cpu_to_le32(SET_WORD(info->uid_value, FWCMD_H2C_BA_CAM_UID_VALUE) |
				    (info->std_entry_en ? FWCMD_H2C_BA_CAM_STD_ENTRY_EN : 0) |
				    (info->band_sel ? FWCMD_H2C_BA_CAM_BAND_SEL : 0) |
				    SET_WORD(info->entry_idx_v1, FWCMD_H2C_BA_CAM_ENTRY_IDX_V1)
				    );
	}

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C,
			      FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_BA_CAM,
			      FWCMD_H2C_FUNC_BA_CAM,
			      0,
			      1);

	if (ret) {
		PLTFM_MSG_ERR("[ERR]h2c_pkt_set_hdr %d\n", ret);
		goto fail;
	}

	ret = h2c_pkt_build_txd(adapter, h2cb);
	if (ret) {
		PLTFM_MSG_ERR("[ERR]h2c_pkt_build_txd %d\n", ret);
		goto fail;
	}

	#if MAC_AX_PHL_H2C
	ret = PLTFM_TX(h2cb);
	#else
	ret = PLTFM_TX(h2cb->data, h2cb->len);
	#endif
	if (ret) {
		PLTFM_MSG_ERR("[ERR]PLTFM_TX %d\n", ret);
		goto fail;
	}

	h2cb_free(adapter, h2cb);

	return MACSUCCESS;
fail:
	h2cb_free(adapter, h2cb);

	return ret;
}

u32 mac_ss_dl_grp_upd(struct mac_ax_adapter *adapter,
		      struct mac_ax_ss_dl_grp_upd *info)
{
	u32 ret = 0;
	u8 *buf;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	struct fwcmd_dl_grp_upd *tbl;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_CMD);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, sizeof(struct fwcmd_dl_grp_upd));
	if (!buf) {
		ret = MACNOBUF;
		goto fail;
	}

	tbl = (struct fwcmd_dl_grp_upd *)buf;
	tbl->dword0 =
	cpu_to_le32((info->grp_valid ? FWCMD_H2C_DL_GRP_UPD_GRP_VALID : 0) |
		    SET_WORD(info->grp_id, FWCMD_H2C_DL_GRP_UPD_GRP_ID) |
		    (info->is_hwgrp ? FWCMD_H2C_DL_GRP_UPD_IS_HWGRP : 0) |
		    SET_WORD(info->macid_u0, FWCMD_H2C_DL_GRP_UPD_MACID_U0) |
		    SET_WORD(info->macid_u1, FWCMD_H2C_DL_GRP_UPD_MACID_U1) |
		    SET_WORD(info->macid_u2, FWCMD_H2C_DL_GRP_UPD_MACID_U2));

	tbl->dword1 =
	cpu_to_le32(SET_WORD(info->macid_u3, FWCMD_H2C_DL_GRP_UPD_MACID_U3) |
		    SET_WORD(info->macid_u4, FWCMD_H2C_DL_GRP_UPD_MACID_U4) |
		    SET_WORD(info->macid_u5, FWCMD_H2C_DL_GRP_UPD_MACID_U5) |
		    SET_WORD(info->macid_u6, FWCMD_H2C_DL_GRP_UPD_MACID_U6));

	tbl->dword2 =
	cpu_to_le32(SET_WORD(info->macid_u7, FWCMD_H2C_DL_GRP_UPD_MACID_U7) |
		    SET_WORD(info->ac_bitmap_u0,
			     FWCMD_H2C_DL_GRP_UPD_AC_BITMAP_U0) |
		    SET_WORD(info->ac_bitmap_u1,
			     FWCMD_H2C_DL_GRP_UPD_AC_BITMAP_U1) |
		    SET_WORD(info->ac_bitmap_u2,
			     FWCMD_H2C_DL_GRP_UPD_AC_BITMAP_U2) |
		    SET_WORD(info->ac_bitmap_u3,
			     FWCMD_H2C_DL_GRP_UPD_AC_BITMAP_U3) |
		    SET_WORD(info->ac_bitmap_u4,
			     FWCMD_H2C_DL_GRP_UPD_AC_BITMAP_U4) |
		    SET_WORD(info->ac_bitmap_u5,
			     FWCMD_H2C_DL_GRP_UPD_AC_BITMAP_U5));

	tbl->dword3 =
	cpu_to_le32(SET_WORD(info->ac_bitmap_u6,
			     FWCMD_H2C_DL_GRP_UPD_AC_BITMAP_U6) |
		    SET_WORD(info->ac_bitmap_u7,
			     FWCMD_H2C_DL_GRP_UPD_AC_BITMAP_U7) |
		    SET_WORD(info->next_protecttype,
			     FWCMD_H2C_DL_GRP_UPD_NEXT_PROTECTTYPE) |
		    SET_WORD(info->next_rsptype,
			     FWCMD_H2C_DL_GRP_UPD_NEXT_RSPTYPE));

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

u32 mac_ss_ul_grp_upd(struct mac_ax_adapter *adapter,
		      struct mac_ax_ss_ul_grp_upd *info)
{
	u32 ret = 0;
	u8 *buf;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	struct fwcmd_ul_grp_upd *tbl;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_CMD);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, sizeof(struct fwcmd_ul_grp_upd));
	if (!buf) {
		ret = MACNOBUF;
		goto fail;
	}

	tbl = (struct fwcmd_ul_grp_upd *)buf;
	tbl->dword0 =
	cpu_to_le32(SET_WORD(info->macid_u0, FWCMD_H2C_UL_GRP_UPD_MACID_U0) |
		    SET_WORD(info->macid_u1, FWCMD_H2C_UL_GRP_UPD_MACID_U1) |
		    SET_WORD(info->grp_bitmap,
			     FWCMD_H2C_UL_GRP_UPD_GRP_BITMAP));

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C,
			      FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_MEDIA_RPT,
			      FWCMD_H2C_FUNC_UL_GRP_UPD,
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

u32 mac_ss_ul_sta_upd(struct mac_ax_adapter *adapter,
		      struct mac_ax_ss_ul_sta_upd *info)
{
	u32 ret = 0;
	u8 *buf;
#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
#else
	struct h2c_buf *h2cb;
#endif
	struct fwcmd_ss_ulsta_upd *tbl;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_CMD);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, sizeof(struct fwcmd_ss_ulsta_upd));
	if (!buf) {
		ret = MACNOBUF;
		goto fail;
	}

	tbl = (struct fwcmd_ss_ulsta_upd *)buf;
	tbl->dword0 =
	cpu_to_le32(SET_WORD(info->mode, FWCMD_H2C_SS_ULSTA_UPD_MODE));

	tbl->dword1 =
	cpu_to_le32(SET_WORD(info->macid[0], FWCMD_H2C_SS_ULSTA_UPD_MACID_U0) |
		    SET_WORD(info->macid[1], FWCMD_H2C_SS_ULSTA_UPD_MACID_U1) |
		    SET_WORD(info->macid[2], FWCMD_H2C_SS_ULSTA_UPD_MACID_U2) |
		    SET_WORD(info->macid[3], FWCMD_H2C_SS_ULSTA_UPD_MACID_U3));

	tbl->dword2 =
	cpu_to_le32(SET_WORD(info->bsr_len[0],
			     FWCMD_H2C_SS_ULSTA_UPD_BSR_LEN_U0) |
		    SET_WORD(info->bsr_len[1],
			     FWCMD_H2C_SS_ULSTA_UPD_BSR_LEN_U1));

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C,
			      FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_FR_EXCHG,
			      FWCMD_H2C_FUNC_SS_ULSTA_UPD,
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

u32 mac_mu_sta_upd(struct mac_ax_adapter *adapter,
		   struct mac_ax_mu_sta_upd *info)
{
	u32 ret = 0;
	u8 *buf;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	struct fwcmd_mu_sta_upd *tbl;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_CMD);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, sizeof(struct fwcmd_mu_sta_upd));
	if (!buf) {
		ret = MACNOBUF;
		goto fail;
	}

	tbl = (struct fwcmd_mu_sta_upd *)buf;
	tbl->dword0 =
	cpu_to_le32(SET_WORD(info->macid, FWCMD_H2C_MU_STA_UPD_MACID) |
		    SET_WORD(info->mu_idx, FWCMD_H2C_MU_STA_UPD_MU_IDX) |
		    SET_WORD(info->prot_rsp_type[0].u.byte_type,
			     FWCMD_H2C_MU_STA_UPD_PROT_RSP_TYPE_0) |
		    SET_WORD(info->prot_rsp_type[1].u.byte_type,
			     FWCMD_H2C_MU_STA_UPD_PROT_RSP_TYPE_1));

	tbl->dword1 =
	cpu_to_le32(SET_WORD(info->prot_rsp_type[2].u.byte_type,
			     FWCMD_H2C_MU_STA_UPD_PROT_RSP_TYPE_2) |
		    SET_WORD(info->prot_rsp_type[3].u.byte_type,
			     FWCMD_H2C_MU_STA_UPD_PROT_RSP_TYPE_3) |
		    SET_WORD(info->prot_rsp_type[4].u.byte_type,
			     FWCMD_H2C_MU_STA_UPD_PROT_RSP_TYPE_4) |
		    SET_WORD(info->mugrp_bitmap,
			     FWCMD_H2C_MU_STA_UPD_MUGRP_BITMAP) |
		    (info->dis_256q ?
			     FWCMD_H2C_MU_STA_UPD_DIS_256Q : 0) |
		    (info->dis_1024q ?
			     FWCMD_H2C_MU_STA_UPD_DIS_1024Q : 0));

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C,
			      FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_MEDIA_RPT,
			      FWCMD_H2C_FUNC_MU_STA_UPD,
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

u32 mac_wlaninfo_get(struct mac_ax_adapter *adapter,
		     struct mac_ax_wlaninfo_get *info)
{
	u32 ret = 0;
	u8 *buf;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	struct fwcmd_wlaninfo_get *tbl;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_CMD);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, sizeof(struct fwcmd_wlaninfo_get));
	if (!buf) {
		ret = MACNOBUF;
		goto fail;
	}

	tbl = (struct fwcmd_wlaninfo_get *)buf;
	tbl->dword0 =
	cpu_to_le32(SET_WORD(info->info_sel, FWCMD_H2C_WLANINFO_GET_INFO_SEL) |
		    SET_WORD(info->argv0, FWCMD_H2C_WLANINFO_GET_ARGV0) |
		    SET_WORD(info->argv1,
			     FWCMD_H2C_WLANINFO_GET_ARGV1) |
		    SET_WORD(info->argv2,
			     FWCMD_H2C_WLANINFO_GET_ARGV2));

	tbl->dword1 =
	cpu_to_le32(SET_WORD(info->argv3,
			     FWCMD_H2C_WLANINFO_GET_ARGV3) |
		    SET_WORD(info->argv4,
			     FWCMD_H2C_WLANINFO_GET_ARGV4) |
		    SET_WORD(info->argv5,
			     FWCMD_H2C_WLANINFO_GET_ARGV5) |
		    SET_WORD(info->argv6,
			     FWCMD_H2C_WLANINFO_GET_ARGV6));

	tbl->dword2 =
	cpu_to_le32(SET_WORD(info->argv7,
			     FWCMD_H2C_WLANINFO_GET_ARGV7));

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C,
			      FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_FR_EXCHG,
			      FWCMD_H2C_FUNC_WLANINFO_GET,
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

u32 mac_dumpwlanc(struct mac_ax_adapter *adapter, struct mac_ax_dumpwlanc *para)
{
	struct mac_ax_wlaninfo_get info;
	u32 ret = 0;

	info.info_sel = DUMPWLANC;
	info.argv0 = para->cmdid;
	info.argv1 = 0;
	info.argv2 = 0;
	info.argv3 = 0;
	info.argv4 = 0;
	info.argv5 = 0;
	info.argv6 = 0;
	info.argv7 = 0;
	ret = mac_wlaninfo_get(adapter, &info);

	return ret;
}

u32 mac_dumpwlans(struct mac_ax_adapter *adapter, struct mac_ax_dumpwlans *para)
{
	struct mac_ax_wlaninfo_get info;
	u32 ret = 0;

	info.info_sel = DUMPWLANS;
	info.argv0 = para->cmdid;
	info.argv1 = para->macid_grp;
	info.argv2 = 0;
	info.argv3 = 0;
	info.argv4 = 0;
	info.argv5 = 0;
	info.argv6 = 0;
	info.argv7 = 0;
	ret = mac_wlaninfo_get(adapter, &info);

	return ret;
}

u32 mac_dumpwland(struct mac_ax_adapter *adapter, struct mac_ax_dumpwland *para)
{
	u32 ret = 0;
	struct mac_ax_wlaninfo_get info;

	info.argv0 = 0;
	info.argv1 = 0;
	info.argv2 = 0;
	info.argv3 = 0;
	info.argv4 = 0;
	info.argv5 = 0;
	info.argv6 = 0;
	info.argv7 = 0;

	info.info_sel = DUMPWLAND;
	info.argv0 = para->cmdid;
	if (para->cmdid == DLDECISION_SU_FORCEMU_FAIL) {
		info.argv1 = para->macid[0];
		info.argv2 = para->macid[1];
		info.argv3 = 0;
		info.argv4 = 0;
		info.argv5 = 0;
		info.argv6 = 0;
		info.argv7 = 0;
	} else if (para->cmdid == DLDECISION_SU_FORCERU_FAIL) {
		info.argv1 = para->grp_type;
		info.argv2 = para->grp_id;
		info.argv3 = para->macid[0];
		info.argv4 = para->macid[1];
		info.argv5 = para->macid[2];
		info.argv6 = para->macid[3];
		info.argv7 = 0;
	} else if (para->cmdid == DLDECISION_SU_MUTXTIME_PASS_MU_NOTSUPPORT) {
		info.argv1 = para->macid[0];
		info.argv2 = para->macid[1];
		info.argv3 = 0;
		info.argv4 = 0;
		info.argv5 = 0;
		info.argv6 = 0;
		info.argv7 = 0;
	} else if (para->cmdid == DLDECISION_SU_MUTXTIME_FAIL_RU_NOTSUPPORT) {
		info.argv1 = para->grp_type;
		info.argv2 = para->grp_id;
		info.argv3 = para->macid[0];
		info.argv4 = para->macid[1];
		info.argv5 = para->macid[2];
		info.argv6 = para->macid[3];
		info.argv7 = 0;
	} else if (para->cmdid == DLDECISION_MU_TPCOMPARE_RST) {
		info.argv1 = para->macid[0];
		info.argv2 = para->macid[1];
		info.argv3 = 0;
		info.argv4 = 0;
		info.argv5 = 0;
		info.argv6 = 0;
		info.argv7 = 0;
	} else if (para->cmdid == DLDECISION_RU_TPCOMPARE_RST) {
		info.argv1 = para->grp_type;
		info.argv2 = para->grp_id;
		info.argv3 = para->macid[0];
		info.argv4 = para->macid[1];
		info.argv5 = para->macid[2];
		info.argv6 = para->macid[3];
		info.argv7 = 0;
	} else if (para->cmdid == DLDECISION_SU_TPCOMPARE_RST) {
		if (para->muru == 0) {
			info.argv1 = para->macid[0];
			info.argv2 = para->macid[1];
			info.argv3 = 0;
			info.argv4 = 0;
			info.argv5 = 0;
			info.argv6 = 0;
			info.argv7 = 0;
		} else {
			info.argv1 = para->grp_type;
			info.argv2 = para->grp_id;
			info.argv3 = para->macid[0];
			info.argv4 = para->macid[1];
			info.argv5 = para->macid[2];
			info.argv6 = para->macid[3];
			info.argv7 = 0;
		}
	}
	ret = mac_wlaninfo_get(adapter, &info);

	return ret;
}

#if MAC_AX_FEATURE_DBGPKG
u32 cctl_info_debug_write(struct mac_ax_adapter *adapter, u8 macid,
			  struct fwcmd_cctlinfo_ud *tbl)
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

u32 dctl_info_debug_write(struct mac_ax_adapter *adapter, u8 macid,
			  struct fwcmd_dctlinfo_ud *tbl)
{
	mac_sram_dbg_write(adapter, macid * DCTL_INFO_SIZE, tbl->dword1,
			   DMAC_TBL_SEL);
	mac_sram_dbg_write(adapter, macid * DCTL_INFO_SIZE + 4, tbl->dword2,
			   DMAC_TBL_SEL);
	mac_sram_dbg_write(adapter, macid * DCTL_INFO_SIZE + 8, tbl->dword3,
			   DMAC_TBL_SEL);
	mac_sram_dbg_write(adapter, macid * DCTL_INFO_SIZE + 12, tbl->dword4,
			   DMAC_TBL_SEL);

	return MACSUCCESS;
}
#endif
u32 mac_fw_status_cmd(struct mac_ax_adapter *adapter,
		      struct mac_ax_fwstatus_payload *info)
{
	u32 ret = 0;
	u32 i;
	u8 *buf;
	u32 *src, *dest;
#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
#else
	struct h2c_buf *h2cb;
#endif
	struct mac_ax_fwstatus_payload *tbl;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_DATA);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, sizeof(struct mac_ax_fwstatus_payload));
	if (!buf) {
		ret = MACNOBUF;
		goto fail;
	}

	tbl = (struct mac_ax_fwstatus_payload *)buf;
	src = (u32 *)info;
	dest = (u32 *)tbl;
	for (i = 0; i < (sizeof(struct mac_ax_fwstatus_payload) / 4); i++)
		*(dest++) = cpu_to_le32(*(src++));

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C,
			      FWCMD_H2C_CAT_TEST,
			      FWCMD_H2C_CL_FW_STATUS_TEST,
			      0,
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

	h2c_end_flow(adapter);

	return MACSUCCESS;
fail:
	h2cb_free(adapter, h2cb);

	return ret;
}

u32 mac_fwc2h_ofdma_sts_parse(struct mac_ax_adapter *adapter,
			      struct mac_ax_fwc2h_sts *fw_c2h_sts,
			      u32 *content)
{
	u32 val;
	u8 i;
	u32 *dl_content = content + sizeof(struct mac_ax_tf_sts) / sizeof(u32);

	if (!fw_c2h_sts || !content) {
		PLTFM_MSG_ERR("[ERR]fwc2h_sts is null\n");
		return MACNPTR;
	}

	val = le32_to_cpu(*(content++));
	fw_c2h_sts->tfsts.user_num =
		GET_FIELD(val, FWCMD_C2H_OFDMA_STS_TFSTS_USER_NUM);
	fw_c2h_sts->tfsts.ru_su_per =
		GET_FIELD(val, FWCMD_C2H_OFDMA_STS_TFSTS_RU_SU_PER);

	for (i = 0; i < UL_PER_STA_DBGINFO_NUM; i++) {
		val = le32_to_cpu(*(content++));
		fw_c2h_sts->tfsts.tf_user_sts[i].macid =
			GET_FIELD(val, FWCMD_C2H_OFDMA_STS_TFSTS_MACID);
		fw_c2h_sts->tfsts.tf_user_sts[i].tb_rate =
			GET_FIELD(val, FWCMD_C2H_OFDMA_STS_TFSTS_TB_RATE);
		fw_c2h_sts->tfsts.tf_user_sts[i].tb_fail_per =
			GET_FIELD(val, FWCMD_C2H_OFDMA_STS_TFSTS_TB_FAIL_PER);
		fw_c2h_sts->tfsts.tf_user_sts[i].avg_tb_rssi =
			GET_FIELD(val, FWCMD_C2H_OFDMA_STS_TFSTS_AVG_TB_RSSI);
		val = le32_to_cpu(*(content++));
		fw_c2h_sts->tfsts.tf_user_sts[i].cca_miss_per =
			GET_FIELD(val, FWCMD_C2H_OFDMA_STS_TFSTS_CCA_MISS_PER);
		fw_c2h_sts->tfsts.tf_user_sts[i].avg_uph =
			GET_FIELD(val, FWCMD_C2H_OFDMA_STS_TFSTS_AVG_UPH);
		fw_c2h_sts->tfsts.tf_user_sts[i].minflag_per =
			GET_FIELD(val, FWCMD_C2H_OFDMA_STS_TFSTS_MINFLAG_PER);
		fw_c2h_sts->tfsts.tf_user_sts[i].avg_tb_evm =
		val = le32_to_cpu(*(content++));
			GET_FIELD(val, FWCMD_C2H_OFDMA_STS_TFSTS_AVG_TB_EVM);
		fw_c2h_sts->tfsts.tf_user_sts[i].tf_num =
			GET_FIELD(val, FWCMD_C2H_OFDMA_STS_TFSTS_TF_NUM);
		val = le32_to_cpu(*(content++));
		fw_c2h_sts->tfsts.tf_user_sts[i].bsr_len =
			GET_FIELD(val, FWCMD_C2H_OFDMA_STS_TFSTS_BSR_LEN);
	}

	content =  dl_content;

	val = le32_to_cpu(*(content++));
	fw_c2h_sts->dlrusts.total_su_ru_ratio =
		GET_FIELD(val, FWCMD_C2H_OFDMA_STS_DLRUSTS_TOTAL_SU_RU_RATIO);
	fw_c2h_sts->dlrusts.total_ru_fail_ratio =
		GET_FIELD(val, FWCMD_C2H_OFDMA_STS_DLRUSTS_TOTAL_RU_FAIL_RATIO);
	fw_c2h_sts->dlrusts.total_su_fail_ratio =
		GET_FIELD(val, FWCMD_C2H_OFDMA_STS_DLRUSTS_TOTAL_SU_FAIL_RATIO);
	fw_c2h_sts->dlrusts.user_num =
		GET_FIELD(val, FWCMD_C2H_OFDMA_STS_DLRUSTS_USER_NUM);

	for (i = 0; i < fw_c2h_sts->dlrusts.user_num; i++) {
		val = le32_to_cpu(*(content++));
		fw_c2h_sts->dlrusts.user_sts[i].macid =
			GET_FIELD(val, FWCMD_C2H_OFDMA_STS_DLRUSTS_MACID);
		fw_c2h_sts->dlrusts.user_sts[i].su_ru_ratio =
			GET_FIELD(val, FWCMD_C2H_OFDMA_STS_DLRUSTS_SU_RU_RATIO);
		fw_c2h_sts->dlrusts.user_sts[i].su_fail_ratio =
			GET_FIELD(val, FWCMD_C2H_OFDMA_STS_DLRUSTS_SU_FAIL);
		fw_c2h_sts->dlrusts.user_sts[i].ru_fail_ratio =
			GET_FIELD(val, FWCMD_C2H_OFDMA_STS_DLRUSTS_RU_FAIL);
		val = le32_to_cpu(*(content++));
		fw_c2h_sts->dlrusts.user_sts[i].ru_avg_agg =
			GET_FIELD(val, FWCMD_C2H_OFDMA_STS_DLRUSTS_AVG_AGG);
		fw_c2h_sts->dlrusts.user_sts[i].NSS =
			GET_FIELD(val, FWCMD_C2H_OFDMA_STS_DLRUSTS_RU_RATE_NSS);
		fw_c2h_sts->dlrusts.user_sts[i].MCS =
			GET_FIELD(val, FWCMD_C2H_OFDMA_STS_DLRUSTS_RU_RATE_MCS);
	}

	return MACSUCCESS;
}

u32 mac_fw_ofdma_sts_en(struct mac_ax_adapter *adapter,
			struct mac_ax_fwsts_para *fwsts_para)
{
	u32 ret = 0;
	u8 *buf;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	u32 *para;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_DATA);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, sizeof(u32));
	if (!buf) {
		ret = MACNOBUF;
		goto fail;
	}

	para = (u32 *)buf;
	(*para) = cpu_to_le32((fwsts_para->en ? FWCMD_H2C_FW_STS_PARA_EN : 0) |
			       SET_WORD(fwsts_para->intvl_ms,
					FWCMD_H2C_FW_STS_PARA_INTVL_MS));

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C,
			      FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_FR_EXCHG,
			      FWCMD_H2C_FUNC_FW_STS_PARA,
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

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

#include "twt.h"

u32 twt_info_init(struct mac_ax_adapter *adapter)
{
	adapter->twt_info =
		(struct mac_ax_twt_info *)PLTFM_MALLOC(TWT_INFO_SIZE);
	adapter->twt_info->err_rec = 0;
	adapter->twt_info->pdbg_info = (u8 *)PLTFM_MALLOC(TWT_DBG_INFO_SIZE);
	PLTFM_MEMSET(adapter->twt_info->pdbg_info, 0, TWT_DBG_INFO_SIZE);

	return MACSUCCESS;
}

u32 twt_info_exit(struct mac_ax_adapter *adapter)
{
	PLTFM_FREE(adapter->twt_info->pdbg_info, TWT_DBG_INFO_SIZE);
	PLTFM_FREE(adapter->twt_info, TWT_INFO_SIZE);

	return MACSUCCESS;
}

u32 mac_twt_info_upd_h2c(struct mac_ax_adapter *adapter,
			 struct mac_ax_twt_para *info)
{
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	struct fwcmd_twtinfo_upd *hdr;
	u32 ret = MACSUCCESS;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_CMD);
	if (!h2cb)
		return MACNPTR;

	hdr = (struct fwcmd_twtinfo_upd *)
		h2cb_put(h2cb, sizeof(struct fwcmd_twtinfo_upd));
	if (!hdr) {
		ret = MACNOBUF;
		goto fail;
	}

	/* port 4 not support */
	if (info->port >= MAC_AX_PORT_4) {
		PLTFM_MSG_ERR("[ERR] twt info upd h2c port %d\n", info->port);
		return MACFUNCINPUT;
	}

	hdr->dword0 =
		cpu_to_le32(SET_WORD(info->nego_tp,
				     FWCMD_H2C_TWTINFO_UPD_NEGOTYPE) |
			    SET_WORD(info->act, FWCMD_H2C_TWTINFO_UPD_ACT) |
			    (info->trig ? FWCMD_H2C_TWTINFO_UPD_TRIGGER : 0) |
			    (info->flow_tp ?
			     FWCMD_H2C_TWTINFO_UPD_FLOWTYPE : 0) |
			    (info->impt ? FWCMD_H2C_TWTINFO_UPD_IMPT : 0) |
			    (info->wake_unit ?
			     FWCMD_H2C_TWTINFO_UPD_WAKEDURUNIT : 0) |
			    (info->rsp_pm ? FWCMD_H2C_TWTINFO_UPD_RSPPM : 0) |
			    (info->proct ? FWCMD_H2C_TWTINFO_UPD_PROT : 0) |
			    SET_WORD(info->flow_id,
				     FWCMD_H2C_TWTINFO_UPD_FLOWID) |
			    SET_WORD(info->id, FWCMD_H2C_TWTINFO_UPD_ID) |
			    (info->band ? FWCMD_H2C_TWTINFO_UPD_BAND : 0) |
			    SET_WORD(info->port, FWCMD_H2C_TWTINFO_UPD_PORT));

	hdr->dword1 =
		cpu_to_le32(SET_WORD(info->wake_exp,
				     FWCMD_H2C_TWTINFO_UPD_WAKE_EXP) |
			    SET_WORD(info->wake_man,
				     FWCMD_H2C_TWTINFO_UPD_WAKE_MAN) |
			    SET_WORD(info->twtulfixmode,
				     FWCMD_H2C_TWTINFO_UPD_ULFIXMODE) |
			    SET_WORD(info->dur,
				     FWCMD_H2C_TWTINFO_UPD_DUR));

	hdr->dword2 =
		cpu_to_le32(SET_WORD(info->trgt_l,
				     FWCMD_H2C_TWTINFO_UPD_TGT_L));

	hdr->dword3 =
		cpu_to_le32(SET_WORD(info->trgt_h,
				     FWCMD_H2C_TWTINFO_UPD_TGT_H));

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C,
			      FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_TWT,
			      FWCMD_H2C_FUNC_TWTINFO_UPD,
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

u32 mac_twt_act_h2c(struct mac_ax_adapter *adapter,
		    struct mac_ax_twtact_para *info)
{
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	struct fwcmd_twt_stansp_upd *hdr;
	u32 ret = MACSUCCESS;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_CMD);
	if (!h2cb)
		return MACNPTR;

	hdr = (struct fwcmd_twt_stansp_upd *)
		h2cb_put(h2cb, sizeof(struct fwcmd_twt_stansp_upd));
	if (!hdr) {
		ret = MACNOBUF;
		goto fail;
	}

	hdr->dword0 =
		cpu_to_le32(SET_WORD(info->macid,
				     FWCMD_H2C_TWT_STANSP_UPD_MACID) |
			    SET_WORD(info->id,
				     FWCMD_H2C_TWT_STANSP_UPD_ID) |
			    SET_WORD(info->act,
				     FWCMD_H2C_TWT_STANSP_UPD_ACT));

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C,
			      FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_TWT,
			      FWCMD_H2C_FUNC_TWT_STANSP_UPD,
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

	h2c_end_flow(adapter);

	return MACSUCCESS;
fail:
	h2cb_free(adapter, h2cb);

	return ret;
}

u32 mac_twt_staanno_h2c(struct mac_ax_adapter *adapter,
			struct mac_ax_twtanno_para *info)
{
	#if MAC_AX_PHL_H2C
		struct rtw_h2c_pkt *h2cb;
	#else
		struct h2c_buf *h2cb;
	#endif
		struct fwcmd_twt_announce_upd *hdr;
		u32 ret = MACSUCCESS;

		h2cb = h2cb_alloc(adapter, H2CB_CLASS_CMD);
		if (!h2cb)
			return MACNPTR;

		hdr = (struct fwcmd_twt_announce_upd *)h2cb_put(h2cb,
			sizeof(struct fwcmd_twt_announce_upd));
		if (!hdr) {
			ret = MACNOBUF;
			goto fail;
		}

		hdr->dword0 =
			cpu_to_le32(SET_WORD(info->macid, FWCMD_H2C_TWT_ANNOUNCE_UPD_MACID));

		ret = h2c_pkt_set_hdr(adapter, h2cb,
				      FWCMD_TYPE_H2C,
				      FWCMD_H2C_CAT_MAC,
				      FWCMD_H2C_CL_TWT,
				      FWCMD_H2C_FUNC_TWT_ANNOUNCE_UPD,
				      1,
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

void mac_twt_wait_anno(struct mac_ax_adapter *adapter,
		       u8 *c2h_content, u8 *upd_addr)
{
	u32 plat_c2h_content = *(u32 *)(c2h_content);
	struct mac_ax_twtanno_c2hpara *para =
		(struct mac_ax_twtanno_c2hpara *)upd_addr;

	para->wait_case = GET_FIELD(plat_c2h_content,
				    FWCMD_C2H_WAIT_ANNOUNCE_WAIT_CASE);
	para->macid0 = GET_FIELD(plat_c2h_content,
				 FWCMD_C2H_WAIT_ANNOUNCE_MACID0);
	para->macid1 = GET_FIELD(plat_c2h_content,
				 FWCMD_C2H_WAIT_ANNOUNCE_MACID1);
	para->macid2 = GET_FIELD(plat_c2h_content,
				 FWCMD_C2H_WAIT_ANNOUNCE_MACID2);
}

u32 mac_get_tsf(struct mac_ax_adapter *adapter, struct mac_ax_port_tsf *tsf)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 reg_l = 0;
	u32 reg_h = 0;
	u32 ret;

	ret = check_mac_en(adapter, tsf->band, MAC_AX_CMAC_SEL);
	if (ret != MACSUCCESS)
		return ret;

	switch (tsf->port) {
	case MAC_AX_PORT_0:
		reg_h = tsf->band == MAC_AX_BAND_0 ?
			R_AX_TSFTR_HIGH_P0 : R_AX_TSFTR_HIGH_P0_C1;
		reg_l = tsf->band == MAC_AX_BAND_0 ?
			R_AX_TSFTR_LOW_P0 : R_AX_TSFTR_LOW_P0_C1;
		break;
	case MAC_AX_PORT_1:
		reg_h = tsf->band == MAC_AX_BAND_0 ?
			R_AX_TSFTR_HIGH_P1 : R_AX_TSFTR_HIGH_P1_C1;
		reg_l = tsf->band == MAC_AX_BAND_0 ?
			R_AX_TSFTR_LOW_P1 : R_AX_TSFTR_LOW_P1_C1;
		break;
	case MAC_AX_PORT_2:
		reg_h = tsf->band == MAC_AX_BAND_0 ?
			R_AX_TSFTR_HIGH_P2 : R_AX_TSFTR_HIGH_P2_C1;
		reg_l = tsf->band == MAC_AX_BAND_0 ?
			R_AX_TSFTR_LOW_P2 : R_AX_TSFTR_LOW_P2_C1;
		break;
	case MAC_AX_PORT_3:
		reg_h = tsf->band == MAC_AX_BAND_0 ?
			R_AX_TSFTR_HIGH_P3 : R_AX_TSFTR_HIGH_P3_C1;
		reg_l = tsf->band == MAC_AX_BAND_0 ?
			R_AX_TSFTR_LOW_P3 : R_AX_TSFTR_LOW_P3_C1;
		break;
	case MAC_AX_PORT_4:
		reg_h = tsf->band == MAC_AX_BAND_0 ?
			R_AX_TSFTR_HIGH_P4 : R_AX_TSFTR_HIGH_P4_C1;
		reg_l = tsf->band == MAC_AX_BAND_0 ?
			R_AX_TSFTR_LOW_P4 : R_AX_TSFTR_LOW_P4_C1;
		break;
	default:
		return MACFUNCINPUT;
	}

	tsf->tsf_h = MAC_REG_R32(reg_h);
	tsf->tsf_l = MAC_REG_R32(reg_l);

	return MACSUCCESS;
}


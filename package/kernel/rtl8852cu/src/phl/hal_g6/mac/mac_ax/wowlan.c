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

#include "wowlan.h"
#include "mac_priv.h"

static u32 wow_bk_status[4] = {0};
static u32 tgt_ind_orig;
static u32 frm_tgt_ind_orig;
static u32 wol_pattern_orig;
static u32 wol_uc_orig;
static u32 wol_magic_orig;
static u8 mdns_v4_multicast_addr[] = {0x01, 0x00, 0x5e, 0x00, 0x00, 0xFB};
static u8 mdns_v6_multicast_addr[] = {0x33, 0x33, 0x00, 0x00, 0x00, 0xFB};
static u8 wsd_v4_multicast_addr[] = {0x01, 0x00, 0x5E, 0x7F, 0xFF, 0xFA};
static u8 wsd_v6_multicast_addr[] = {0x33, 0x33, 0x00, 0x00, 0x00, 0x0C};

static u32 send_h2c_keep_alive(struct mac_ax_adapter *adapter,
			       struct keep_alive *parm)
{
	u8 *buf;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	struct fwcmd_keep_alive *fwcmd_kalive;
	u32 ret = 0;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_CMD);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, sizeof(struct fwcmd_keep_alive));
	if (!buf) {
		ret = MACNOBUF;
		goto fail;
	}

	fwcmd_kalive = (struct fwcmd_keep_alive *)buf;
	fwcmd_kalive->dword0 =
	cpu_to_le32((parm->keepalive_en ?
		     FWCMD_H2C_KEEP_ALIVE_KEEPALIVE_EN : 0) |
		SET_WORD(parm->packet_id, FWCMD_H2C_KEEP_ALIVE_PACKET_ID) |
		SET_WORD(parm->period, FWCMD_H2C_KEEP_ALIVE_PERIOD) |
		SET_WORD(parm->mac_id, FWCMD_H2C_KEEP_ALIVE_MAC_ID));

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C,
			      FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_WOW,
			      FWCMD_H2C_FUNC_KEEP_ALIVE,
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

static u32 send_h2c_disconnect_detect(struct mac_ax_adapter *adapter,
				      struct disconnect_detect *parm)
{
	u8 *buf;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	struct fwcmd_disconnect_detect *fwcmd_disconnect_det;
	u32 ret = 0;
	u32 tmp;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_CMD);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, sizeof(struct fwcmd_disconnect_detect));
	if (!buf) {
		ret = MACNOBUF;
		goto fail;
	}

	fwcmd_disconnect_det = (struct fwcmd_disconnect_detect *)buf;
	fwcmd_disconnect_det->dword0 =
	cpu_to_le32((parm->disconnect_detect_en ?
		     FWCMD_H2C_DISCONNECT_DETECT_DISCONNECT_DETECT_EN : 0) |
	(parm->tryok_bcnfail_count_en ?
	 FWCMD_H2C_DISCONNECT_DETECT_TRYOK_BCNFAIL_COUNT_EN : 0) |
	(parm->disconnect_en ? FWCMD_H2C_DISCONNECT_DETECT_DISCONNECT_EN : 0) |
	SET_WORD(parm->mac_id, FWCMD_H2C_DISCONNECT_DETECT_MAC_ID) |
	SET_WORD(parm->check_period, FWCMD_H2C_DISCONNECT_DETECT_CHECK_PERIOD) |
	SET_WORD(parm->try_pkt_count,
		 FWCMD_H2C_DISCONNECT_DETECT_TRY_PKT_COUNT));

	tmp = SET_WORD(parm->tryok_bcnfail_count_limit,
		       FWCMD_H2C_DISCONNECT_DETECT_TRYOK_BCNFAIL_COUNT_LIMIT);
	fwcmd_disconnect_det->dword1 = cpu_to_le32(tmp);

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C,
			      FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_WOW,
			      FWCMD_H2C_FUNC_DISCONNECT_DETECT,
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

static u32 send_h2c_wow_global(struct mac_ax_adapter *adapter,
			       struct wow_global *parm)
{
	u8 *buf;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	struct fwcmd_wow_global *fwcmd_wow_glo;
	u32 ret = 0;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_DATA);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, sizeof(struct fwcmd_wow_global)
		       + sizeof(struct mac_ax_remotectrl_info_parm_) - 4);
	if (!buf) {
		ret = MACNOBUF;
		goto fail;
	}

	fwcmd_wow_glo = (struct fwcmd_wow_global *)buf;
	fwcmd_wow_glo->dword0 =
	cpu_to_le32((parm->wow_en ? FWCMD_H2C_WOW_GLOBAL_WOW_EN : 0) |
		(parm->drop_all_pkt ? FWCMD_H2C_WOW_GLOBAL_DROP_ALL_PKT : 0) |
		(parm->rx_parse_after_wake ?
		 FWCMD_H2C_WOW_GLOBAL_RX_PARSE_AFTER_WAKE : 0) |
		SET_WORD(parm->mac_id, FWCMD_H2C_WOW_GLOBAL_MAC_ID) |
		SET_WORD(parm->pairwise_sec_algo,
			 FWCMD_H2C_WOW_GLOBAL_PAIRWISE_SEC_ALGO) |
		SET_WORD(parm->group_sec_algo,
			 FWCMD_H2C_WOW_GLOBAL_GROUP_SEC_ALGO));

	//fwcmd_wow_glo->dword1 =
	//	cpu_to_le32(parm->remotectrl_info_content);
	PLTFM_MEMCPY(&fwcmd_wow_glo->dword1, &parm->remotectrl_info_content,
		     sizeof(struct mac_ax_remotectrl_info_parm_));

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C,
			      FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_WOW,
			      FWCMD_H2C_FUNC_WOW_GLOBAL,
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

static u32 send_h2c_gtk_ofld(struct mac_ax_adapter *adapter,
			     struct gtk_ofld *parm)
{
	u8 *buf;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	struct fwcmd_gtk_ofld *fwcmd_gtk_ofl;
	u32 ret = 0;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_DATA);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, sizeof(struct fwcmd_gtk_ofld)
		       + sizeof(struct mac_ax_gtk_info_parm_) - 4);
	if (!buf) {
		ret = MACNOBUF;
		goto fail;
	}

	fwcmd_gtk_ofl = (struct fwcmd_gtk_ofld *)buf;
	fwcmd_gtk_ofl->dword0 =
	cpu_to_le32((parm->gtk_en ? FWCMD_H2C_GTK_OFLD_GTK_EN : 0) |
		(parm->tkip_en ? FWCMD_H2C_GTK_OFLD_TKIP_EN : 0) |
		(parm->ieee80211w_en ? FWCMD_H2C_GTK_OFLD_IEEE80211W_EN : 0) |
		(parm->pairwise_wakeup ?
		 FWCMD_H2C_GTK_OFLD_PAIRWISE_WAKEUP : 0) |
		(parm->norekey_wakeup ?
		 FWCMD_H2C_GTK_OFLD_NOREKEY_WAKEUP : 0) |
		SET_WORD(parm->mac_id, FWCMD_H2C_GTK_OFLD_MAC_ID) |
		SET_WORD(parm->gtk_rsp_id, FWCMD_H2C_GTK_OFLD_GTK_RSP_ID));

	fwcmd_gtk_ofl->dword1 =
	cpu_to_le32(SET_WORD(parm->pmf_sa_query_id, FWCMD_H2C_GTK_OFLD_PMF_SA_QUERY_ID) |
		    SET_WORD(parm->bip_sec_algo, FWCMD_H2C_GTK_OFLD_PMF_BIP_SEC_ALGO) |
		    SET_WORD(parm->algo_akm_suit, FWCMD_H2C_GTK_OFLD_ALGO_AKM_SUIT));

	PLTFM_MEMCPY(&fwcmd_gtk_ofl->dword2, &parm->gtk_info_content,
		     sizeof(struct mac_ax_gtk_info_parm_));

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C,
			      FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_WOW,
			      FWCMD_H2C_FUNC_GTK_OFLD,
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

static u32 send_h2c_arp_ofld(struct mac_ax_adapter *adapter,
			     struct arp_ofld *parm)
{
	u8 *buf;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	struct fwcmd_arp_ofld *fwcmd_arp_ofl;
	u32 ret = 0;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_CMD);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, sizeof(struct fwcmd_arp_ofld));
	if (!buf) {
		ret = MACNOBUF;
		goto fail;
	}

	fwcmd_arp_ofl = (struct fwcmd_arp_ofld *)buf;
	fwcmd_arp_ofl->dword0 =
	cpu_to_le32((parm->arp_en ? FWCMD_H2C_ARP_OFLD_ARP_EN : 0) |
		(parm->arp_action ? FWCMD_H2C_ARP_OFLD_ARP_ACTION : 0) |
		SET_WORD(parm->mac_id, FWCMD_H2C_ARP_OFLD_MAC_ID) |
		SET_WORD(parm->arp_rsp_id, FWCMD_H2C_ARP_OFLD_ARP_RSP_ID));

	fwcmd_arp_ofl->dword1 =
		cpu_to_le32(parm->arp_info_content);

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C,
			      FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_WOW,
			      FWCMD_H2C_FUNC_ARP_OFLD,
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

static u32 send_h2c_ndp_ofld(struct mac_ax_adapter *adapter,
			     struct ndp_ofld *parm)
{
	u8 *buf;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	struct fwcmd_ndp_ofld *fwcmd_ndp_ofl;
	u32 ret = 0;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_DATA);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, sizeof(struct fwcmd_ndp_ofld) + 2 *
		       sizeof(struct mac_ax_ndp_info_parm_) - 4);
	if (!buf) {
		ret = MACNOBUF;
		goto fail;
	}

	fwcmd_ndp_ofl = (struct fwcmd_ndp_ofld *)buf;
	fwcmd_ndp_ofl->dword0 =
	cpu_to_le32((parm->ndp_en ? FWCMD_H2C_NDP_OFLD_NDP_EN : 0) |
		    SET_WORD(parm->mac_id, FWCMD_H2C_NDP_OFLD_MAC_ID) |
		    SET_WORD(parm->na_id, FWCMD_H2C_NDP_OFLD_NA_ID));

	PLTFM_MEMCPY(&fwcmd_ndp_ofl->dword1, &parm->ndp_info_content, 2 *
		     sizeof(struct mac_ax_ndp_info_parm_));

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C,
			      FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_WOW,
			      FWCMD_H2C_FUNC_NDP_OFLD,
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

static u32 send_h2c_realwow(struct mac_ax_adapter *adapter,
			    struct realwow *parm)
{
	u8 *buf;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	struct fwcmd_realwow *fwcmd_realw;
	u32 ret = 0;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_CMD);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, sizeof(struct fwcmd_realwow) +
		       sizeof(struct mac_ax_realwowv2_info_parm_) - 4);
	if (!buf) {
		ret = MACNOBUF;
		goto fail;
	}

	fwcmd_realw = (struct fwcmd_realwow *)buf;
	fwcmd_realw->dword0 =
	cpu_to_le32((parm->realwow_en ? FWCMD_H2C_REALWOW_REALWOW_EN : 0) |
		(parm->auto_wakeup ? FWCMD_H2C_REALWOW_AUTO_WAKEUP : 0) |
		SET_WORD(parm->mac_id, FWCMD_H2C_REALWOW_MAC_ID));

	fwcmd_realw->dword1 =
	cpu_to_le32(SET_WORD(parm->keepalive_id,
			     FWCMD_H2C_REALWOW_KEEPALIVE_ID) |
	SET_WORD(parm->wakeup_pattern_id, FWCMD_H2C_REALWOW_WAKEUP_PATTERN_ID) |
	SET_WORD(parm->ack_pattern_id, FWCMD_H2C_REALWOW_ACK_PATTERN_ID));

	fwcmd_realw->dword2 =
		cpu_to_le32(parm->realwow_info_content);

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C,
			      FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_WOW,
			      FWCMD_H2C_FUNC_REALWOW,
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

static u32 send_h2c_nlo(struct mac_ax_adapter *adapter,
			struct nlo *parm)
{
	u8 *buf;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif

	u32 *h2cb_u32;
	u32 *nlo_parm_u32;
	u32 ret = 0;
	u8 sh;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_LONG_DATA);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, sizeof(struct fwcmd_nlo) +
		       sizeof(struct mac_ax_nlo_networklist_parm_) - 4);
	if (!buf) {
		ret = MACNOBUF;
		goto fail;
	}

	nlo_parm_u32 = &parm->nlo_networklistinfo_content;
	h2cb_u32 = (u32 *)buf;
	*h2cb_u32 = cpu_to_le32((parm->nlo_en ? FWCMD_H2C_NLO_NLO_EN : 0) |
				(parm->nlo_32k_en ? FWCMD_H2C_NLO_NLO_32K_EN : 0) |
				(parm->ignore_cipher_type ? FWCMD_H2C_NLO_IGNORE_CIPHER_TYPE : 0) |
				SET_WORD(parm->mac_id, FWCMD_H2C_NLO_MAC_ID));
	h2cb_u32++;

	*h2cb_u32 = cpu_to_le32(*nlo_parm_u32);
	h2cb_u32++;
	nlo_parm_u32 = parm->nlo_networklistinfo_more;

	for (sh = 0; sh < (sizeof(struct mac_ax_nlo_networklist_parm_) / 4 - 1); sh++)
		*(h2cb_u32 + sh) = cpu_to_le32(*(nlo_parm_u32 + sh));

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C,
			      FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_WOW,
			      FWCMD_H2C_FUNC_NLO,
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

static u32 send_h2c_wakeup_ctrl(struct mac_ax_adapter *adapter,
				struct wakeup_ctrl *parm)
{
	u8 *buf;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	struct fwcmd_wakeup_ctrl *fwcmd_wakeup_ctr;
	u32 ret = 0;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_CMD);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, sizeof(struct fwcmd_wakeup_ctrl));
	if (!buf) {
		ret = MACNOBUF;
		goto fail;
	}

	fwcmd_wakeup_ctr = (struct fwcmd_wakeup_ctrl *)buf;
	fwcmd_wakeup_ctr->dword0 =
	cpu_to_le32((parm->pattern_match_en ?
		     FWCMD_H2C_WAKEUP_CTRL_PATTERN_MATCH_EN : 0) |
	(parm->magic_en ? FWCMD_H2C_WAKEUP_CTRL_MAGIC_EN : 0) |
	(parm->hw_unicast_en ? FWCMD_H2C_WAKEUP_CTRL_HW_UNICAST_EN : 0) |
	(parm->fw_unicast_en ? FWCMD_H2C_WAKEUP_CTRL_FW_UNICAST_EN : 0) |
	(parm->deauth_wakeup ? FWCMD_H2C_WAKEUP_CTRL_DEAUTH_WAKEUP : 0) |
	(parm->rekey_wakeup ? FWCMD_H2C_WAKEUP_CTRL_REKEY_WAKEUP : 0) |
	(parm->eap_wakeup ? FWCMD_H2C_WAKEUP_CTRL_EAP_WAKEUP : 0) |
	(parm->all_data_wakeup ? FWCMD_H2C_WAKEUP_CTRL_ALL_DATA_WAKEUP : 0) |
	SET_WORD(parm->mac_id, FWCMD_H2C_WAKEUP_CTRL_MAC_ID));

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C,
			      FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_WOW,
			      FWCMD_H2C_FUNC_WAKEUP_CTRL,
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

static u32 send_h2c_negative_pattern(struct mac_ax_adapter *adapter,
				     struct negative_pattern *parm)
{
	u8 *buf;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	struct fwcmd_negative_pattern *fwcmd_negative_patter;
	u32 ret = 0;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_CMD);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, sizeof(struct fwcmd_negative_pattern));
	if (!buf) {
		ret = MACNOBUF;
		goto fail;
	}

	fwcmd_negative_patter = (struct fwcmd_negative_pattern *)buf;

	fwcmd_negative_patter->dword0 =
	cpu_to_le32((parm->negative_pattern_en ?
			FWCMD_H2C_NEGATIVE_PATTERN_NEGATIVE_PATTERN_EN : 0) |
	SET_WORD(parm->pattern_count,
		 FWCMD_H2C_NEGATIVE_PATTERN_PATTERN_COUNT) |
	SET_WORD(parm->mac_id, FWCMD_H2C_NEGATIVE_PATTERN_MAC_ID));

	fwcmd_negative_patter->dword1 =
		cpu_to_le32(parm->pattern_content);

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C,
			      FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_WOW,
			      FWCMD_H2C_FUNC_NEGATIVE_PATTERN,
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

u32 mac_cfg_dev2hst_gpio(struct mac_ax_adapter *adapter,
			 struct mac_ax_dev2hst_gpio_info *parm)
{
	u8 *buf;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	struct fwcmd_dev2hst_gpio *fwcmd_dev2hst_gpi;
	u32 ret = 0;

	if (parm->gpio_num > MAC_AX_GPIO15) {
		PLTFM_MSG_ERR("gpio num > 15");
		return MACNOITEM;
	}
	if (parm->toggle_pulse == MAC_AX_DEV2HST_PULSE) {
		if (parm->gpio_pulse_dura == 0) {
			PLTFM_MSG_ERR("gpio pulse duration cant be 0");
			return MACNOITEM;
		}
		if (parm->gpio_pulse_period <= parm->gpio_pulse_dura) {
			PLTFM_MSG_ERR("gpio pulse period can less than duration");
			return MACNOITEM;
		}
		if (!parm->gpio_pulse_nonstop && parm->gpio_pulse_count == 0) {
			PLTFM_MSG_ERR("gpio pulse count cant be 0");
			return MACNOITEM;
		}
	}

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_CMD);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, sizeof(struct fwcmd_dev2hst_gpio));
	if (!buf) {
		ret = MACNOBUF;
		goto fail;
	}
	fwcmd_dev2hst_gpi = (struct fwcmd_dev2hst_gpio *)buf;
	fwcmd_dev2hst_gpi->dword0 =
	cpu_to_le32((parm->dev2hst_gpio_en ? FWCMD_H2C_DEV2HST_GPIO_DEV2HST_GPIO_EN : 0) |
		    (parm->disable_inband ? FWCMD_H2C_DEV2HST_GPIO_DISABLE_INBAND : 0) |
		    (parm->gpio_output_input ? FWCMD_H2C_DEV2HST_GPIO_GPIO_OUTPUT_INPUT : 0) |
		    (parm->gpio_active ? FWCMD_H2C_DEV2HST_GPIO_GPIO_ACTIVE : 0) |
		    (parm->toggle_pulse ? FWCMD_H2C_DEV2HST_GPIO_TOGGLE_PULSE : 0) |
		    (parm->data_pin_wakeup ? FWCMD_H2C_DEV2HST_GPIO_DATA_PIN_WAKEUP : 0) |
		    (parm->data_pin_wakeup ? FWCMD_H2C_DEV2HST_GPIO_DATA_PIN_WAKEUP : 0) |
		    (parm->gpio_pulse_nonstop ? FWCMD_H2C_DEV2HST_GPIO_GPIO_PULSE_NONSTOP : 0) |
		    (parm->gpio_time_unit ? FWCMD_H2C_DEV2HST_GPIO_GPIO_TIME_UNIT : 0) |
		    SET_WORD(parm->gpio_num, FWCMD_H2C_DEV2HST_GPIO_GPIO_NUM) |
		    SET_WORD(parm->gpio_pulse_dura, FWCMD_H2C_DEV2HST_GPIO_GPIO_PULSE_DURATION) |
		    SET_WORD(parm->gpio_pulse_period, FWCMD_H2C_DEV2HST_GPIO_GPIO_PULSE_PERIOD));

	fwcmd_dev2hst_gpi->dword1 =
	cpu_to_le32(SET_WORD(parm->gpio_pulse_count,
			     FWCMD_H2C_DEV2HST_GPIO_GPIO_PULSE_COUNT));

	fwcmd_dev2hst_gpi->dword2 =
	cpu_to_le32(SET_WORD(parm->customer_id,
			     FWCMD_H2C_DEV2HST_GPIO_CUSTOMER_ID));

	fwcmd_dev2hst_gpi->dword3 =
	cpu_to_le32((parm->rsn_a_en ? FWCMD_H2C_DEV2HST_GPIO_RSN_A_EN : 0) |
		    (parm->rsn_a_toggle_pulse ? FWCMD_H2C_DEV2HST_GPIO_RSN_A_TOGGLE_PULSE : 0) |
		    (parm->rsn_a_pulse_nonstop ? FWCMD_H2C_DEV2HST_GPIO_RSN_A_PULSE_NONSTOP : 0) |
		    (parm->rsn_a_time_unit ? FWCMD_H2C_DEV2HST_GPIO_RSN_A_TIME_UNIT : 0));

	fwcmd_dev2hst_gpi->dword4 =
	cpu_to_le32(SET_WORD(parm->rsn_a, FWCMD_H2C_DEV2HST_GPIO_RSN_A) |
		    SET_WORD(parm->rsn_a_pulse_duration,
			     FWCMD_H2C_DEV2HST_GPIO_RSN_A_PULSE_DURATION) |
		    SET_WORD(parm->rsn_a_pulse_period, FWCMD_H2C_DEV2HST_GPIO_RSN_A_PULSE_PERIOD) |
		    SET_WORD(parm->rsn_a_pulse_count, FWCMD_H2C_DEV2HST_GPIO_RSN_A_PULSE_COUNT));

	fwcmd_dev2hst_gpi->dword5 =
	cpu_to_le32((parm->rsn_b_en ? FWCMD_H2C_DEV2HST_GPIO_RSN_B_EN : 0) |
		    (parm->rsn_b_toggle_pulse ? FWCMD_H2C_DEV2HST_GPIO_RSN_B_TOGGLE_PULSE : 0) |
		    (parm->rsn_b_pulse_nonstop ? FWCMD_H2C_DEV2HST_GPIO_RSN_B_PULSE_NONSTOP : 0) |
		    (parm->rsn_b_time_unit ? FWCMD_H2C_DEV2HST_GPIO_RSN_B_TIME_UNIT : 0));

	fwcmd_dev2hst_gpi->dword6 =
	cpu_to_le32(SET_WORD(parm->rsn_b, FWCMD_H2C_DEV2HST_GPIO_RSN_B) |
		    SET_WORD(parm->rsn_b_pulse_duration,
			     FWCMD_H2C_DEV2HST_GPIO_RSN_B_PULSE_DURATION) |
		    SET_WORD(parm->rsn_b_pulse_period, FWCMD_H2C_DEV2HST_GPIO_RSN_B_PULSE_PERIOD) |
		    SET_WORD(parm->rsn_b_pulse_count, FWCMD_H2C_DEV2HST_GPIO_RSN_B_PULSE_COUNT));

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C,
			      FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_WOW,
			      FWCMD_H2C_FUNC_DEV2HST_GPIO,
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
	PLTFM_MSG_TRACE("ok");
	return MACSUCCESS;
fail:
	h2cb_free(adapter, h2cb);
	return ret;
}

static u32 send_h2c_uphy_ctrl(struct mac_ax_adapter *adapter,
			      struct uphy_ctrl *parm)
{
	u8 *buf;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	struct fwcmd_hst2dev_ctrl *fwcmd_hst2dev_ctr;
	u32 ret = 0;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_CMD);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, sizeof(struct fwcmd_hst2dev_ctrl));
	if (!buf) {
		ret = MACNOBUF;
		goto fail;
	}

	fwcmd_hst2dev_ctr = (struct fwcmd_hst2dev_ctrl *)buf;
	fwcmd_hst2dev_ctr->dword0 =
	cpu_to_le32((parm->disable_uphy ?
			FWCMD_H2C_HST2DEV_CTRL_DISABLE_UPHY : 0) |
	SET_WORD(parm->handshake_mode, FWCMD_H2C_HST2DEV_CTRL_HANDSHAKE_MODE) |
	(parm->rise_hst2dev_dis_uphy ? FWCMD_H2C_HST2DEV_CTRL_RISE_HST2DEV_DIS_UPHY
									: 0) |
	(parm->uphy_dis_delay_unit ? FWCMD_H2C_HST2DEV_CTRL_UPHY_DIS_DELAY_UNIT
									: 0) |
	(parm->pdn_as_uphy_dis ? FWCMD_H2C_HST2DEV_CTRL_PDN_AS_UPHY_DIS : 0) |
	(parm->pdn_to_enable_uphy ? FWCMD_H2C_HST2DEV_CTRL_PDN_TO_ENABLE_UPHY
									: 0) |
	SET_WORD(parm->hst2dev_gpio_num, FWCMD_H2C_HST2DEV_CTRL_HST2DEV_GPIO_NUM) |
	SET_WORD(parm->uphy_dis_delay_count,
		 FWCMD_H2C_HST2DEV_CTRL_UPHY_DIS_DELAY_COUNT));

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C,
			      FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_WOW,
			      FWCMD_H2C_FUNC_HST2DEV_CTRL,
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

static u32 send_h2c_wowcam_upd(struct mac_ax_adapter *adapter,
			       struct wowcam_upd *parm)
{
	u8 *buf;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	struct fwcmd_wow_cam_upd *fwcmd_wowcam_upd;
	u32 ret = 0;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_CMD);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, sizeof(struct fwcmd_wow_cam_upd));
	if (!buf) {
		ret = MACNOBUF;
		goto fail;
	}

	fwcmd_wowcam_upd = (struct fwcmd_wow_cam_upd *)buf;
	fwcmd_wowcam_upd->dword0 =
		cpu_to_le32((parm->r_w ? FWCMD_H2C_WOW_CAM_UPD_R_W : 0) |
		SET_WORD(parm->idx, FWCMD_H2C_WOW_CAM_UPD_IDX));

	fwcmd_wowcam_upd->dword1 =
		cpu_to_le32(parm->wkfm1);

	fwcmd_wowcam_upd->dword2 =
		cpu_to_le32(parm->wkfm2);

	fwcmd_wowcam_upd->dword3 =
		cpu_to_le32(parm->wkfm3);

	fwcmd_wowcam_upd->dword4 =
		cpu_to_le32(parm->wkfm4);

	fwcmd_wowcam_upd->dword5 =
		cpu_to_le32(SET_WORD(parm->crc, FWCMD_H2C_WOW_CAM_UPD_CRC) |
		(parm->negative_pattern_match ? FWCMD_H2C_WOW_CAM_UPD_NEGATIVE_PATTERN_MATCH : 0) |
		(parm->skip_mac_hdr ? FWCMD_H2C_WOW_CAM_UPD_SKIP_MAC_HDR : 0) |
		(parm->uc ? FWCMD_H2C_WOW_CAM_UPD_UC : 0) |
		(parm->mc ? FWCMD_H2C_WOW_CAM_UPD_MC : 0) |
		(parm->bc ? FWCMD_H2C_WOW_CAM_UPD_BC : 0) |
		(parm->valid ? FWCMD_H2C_WOW_CAM_UPD_VALID : 0));

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C,
			      FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_WOW,
			      FWCMD_H2C_FUNC_WOW_CAM_UPD,
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

u32 mac_cfg_wow_wake(struct mac_ax_adapter *adapter,
		     u8 macid,
		     struct mac_ax_wow_wake_info *info,
		     struct mac_ax_remotectrl_info_parm_ *content)
{
	u32 ret = 0, i = 0;
	struct wow_global parm1;
	struct wakeup_ctrl parm2;
	struct mac_role_tbl *role;
	struct mac_ax_sec_iv_info sec_iv_info = {{0}};
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);

	if (adapter->sm.fwdl != MAC_AX_FWDL_INIT_RDY)
		return MACNOFW;

	parm2.pattern_match_en = info->pattern_match_en;
	parm2.magic_en = info->magic_en;
	parm2.hw_unicast_en = info->hw_unicast_en;
	parm2.fw_unicast_en = info->fw_unicast_en;
	parm2.deauth_wakeup = info->deauth_wakeup;
	parm2.rekey_wakeup = info->rekey_wakeup;
	parm2.eap_wakeup = info->eap_wakeup;
	parm2.all_data_wakeup = info->all_data_wakeup;
	parm2.mac_id = macid;
	ret = send_h2c_wakeup_ctrl(adapter, &parm2);
	if (ret) {
		PLTFM_MSG_ERR("send h2c wakeup ctrl failed\n");
		return ret;
	}

	parm1.wow_en = info->wow_en;
	parm1.drop_all_pkt = info->drop_all_pkt;
	parm1.rx_parse_after_wake = info->rx_parse_after_wake;
	parm1.mac_id = macid;
	parm1.pairwise_sec_algo = info->pairwise_sec_algo;
	parm1.group_sec_algo = info->group_sec_algo;
	//parm1.remotectrl_info_content =
	//info->remotectrl_info_content;
	if (content)
		PLTFM_MEMCPY(&parm1.remotectrl_info_content,
			     content,
			     sizeof(struct mac_ax_remotectrl_info_parm_));

	if (info->wow_en) {
		role = mac_role_srch(adapter, macid);
		if (role) {
			tgt_ind_orig = role->info.tgt_ind;
			frm_tgt_ind_orig = role->info.frm_tgt_ind;
			wol_pattern_orig = role->info.wol_pattern;
			wol_uc_orig = role->info.wol_uc;
			wol_magic_orig = role->info.wol_magic;
			wow_bk_status[(macid >> 5)] |= BIT(macid & 0x1F);
			role->info.wol_pattern = (u8)parm2.pattern_match_en;
			role->info.wol_uc = info->hw_unicast_en;
			role->info.wol_magic = info->magic_en;
			role->info.upd_mode = MAC_AX_ROLE_INFO_CHANGE;
			sec_iv_info.macid = macid;
			if (content)
				for (i = 0 ; i < 6 ; i++)
					sec_iv_info.ptktxiv[i] =
						content->ptktxiv[i];

			sec_iv_info.opcode = SEC_IV_UPD_TYPE_WRITE;
			ret = p_ops->mac_wowlan_secinfo(adapter, &sec_iv_info);

			ret = mac_change_role(adapter, &role->info);
			if (ret) {
				PLTFM_MSG_ERR("role change failed\n");
				return ret;
			}
		} else {
			PLTFM_MSG_ERR("role search failed\n");
			return MACNOITEM;
		}
	} else {
		sec_iv_info.macid = macid;
		sec_iv_info.opcode = SEC_IV_UPD_TYPE_READ;
		ret = p_ops->mac_wowlan_secinfo(adapter, &sec_iv_info);
		if (ret)
			PLTFM_MSG_ERR("refresh_security_cam_info failed %d\n", ret);
		else
			PLTFM_MSG_TRACE("refresh_security_cam_info success!\n");

		if (wow_bk_status[(macid >> 5)] & BIT(macid & 0x1F)) {
			//restore address cam
			role = mac_role_srch(adapter, macid);
			if (role) {
				role->info.tgt_ind = (u8)tgt_ind_orig;
				role->info.frm_tgt_ind = (u8)frm_tgt_ind_orig;
				role->info.wol_pattern = (u8)wol_pattern_orig;
				role->info.wol_uc = (u8)wol_uc_orig;
				role->info.wol_magic = (u8)wol_magic_orig;
				role->info.upd_mode = MAC_AX_ROLE_INFO_CHANGE;
				ret = mac_change_role(adapter, &role->info);
				if (ret) {
					PLTFM_MSG_ERR("role change failed\n");
					return ret;
				}
			}
			wow_bk_status[(macid >> 5)] &= ~BIT(macid & 0x1F);
		} else {
			PLTFM_MSG_ERR("role search failed\n");
			return MACNOITEM;
		}
	}

	ret = send_h2c_wow_global(adapter, &parm1);
	if (ret)
		PLTFM_MSG_ERR("set wow global failed\n");

	return ret;
}

u32 mac_cfg_disconnect_det(struct mac_ax_adapter *adapter,
			   u8 macid,
			   struct mac_ax_disconnect_det_info *info)
{
	u32 ret = 0;
	struct disconnect_detect parm;

	if (adapter->sm.fwdl != MAC_AX_FWDL_INIT_RDY)
		return MACNOFW;

	parm.disconnect_detect_en = info->disconnect_detect_en;
	parm.tryok_bcnfail_count_en =
	    info->tryok_bcnfail_count_en;
	parm.disconnect_en = info->disconnect_en;
	parm.mac_id = macid;
	parm.check_period = info->check_period;
	parm.try_pkt_count = info->try_pkt_count;
	parm.tryok_bcnfail_count_limit =
	    info->tryok_bcnfail_count_limit;

	ret = send_h2c_disconnect_detect(adapter, &parm);
	if (ret)
		return ret;

	return MACSUCCESS;
}

u32 mac_cfg_keep_alive(struct mac_ax_adapter *adapter,
		       u8 macid,
		       struct mac_ax_keep_alive_info *info)
{
	u32 ret = 0;
	struct keep_alive parm;

	if (adapter->sm.fwdl != MAC_AX_FWDL_INIT_RDY)
		return MACNOFW;

	parm.keepalive_en = info->keepalive_en;
	parm.packet_id = info->packet_id;
	parm.period = info->period;
	parm.mac_id = macid;

	ret = send_h2c_keep_alive(adapter, &parm);
	if (ret)
		return ret;

	return MACSUCCESS;
}

u32 mac_cfg_gtk_ofld(struct mac_ax_adapter *adapter,
		     u8 macid,
		     struct mac_ax_gtk_ofld_info *info,
		     struct mac_ax_gtk_info_parm_ *content)
{
	u32 ret = 0;
	struct gtk_ofld parm;

	if (adapter->sm.fwdl != MAC_AX_FWDL_INIT_RDY)
		return MACNOFW;

	parm.gtk_en = info->gtk_en;
	parm.tkip_en = info->tkip_en;
	parm.ieee80211w_en = info->ieee80211w_en;
	parm.pairwise_wakeup = info->pairwise_wakeup;
	parm.norekey_wakeup = info->norekey_wakeup;
	parm.mac_id = macid;
	parm.gtk_rsp_id = info->gtk_rsp_id;
	parm.pmf_sa_query_id = info->pmf_sa_query_id;
	parm.bip_sec_algo = info->bip_sec_algo;
	parm.algo_akm_suit = info->algo_akm_suit;

	if (content)
		PLTFM_MEMCPY(&parm.gtk_info_content, content,
			     sizeof(struct mac_ax_gtk_info_parm_));

	ret = send_h2c_gtk_ofld(adapter, &parm);
	if (ret)
		return ret;
	return MACSUCCESS;
}

u32 mac_cfg_arp_ofld(struct mac_ax_adapter *adapter,
		     u8 macid,
		     struct mac_ax_arp_ofld_info *info,
		     void *parp_info_content)
{
	u32 ret = 0;
	struct arp_ofld parm;

	if (adapter->sm.fwdl != MAC_AX_FWDL_INIT_RDY)
		return MACNOFW;

	PLTFM_MEMSET(&parm, 0, sizeof(struct arp_ofld));
	parm.arp_en = info->arp_en;
	parm.arp_action = info->arp_action;
	parm.mac_id = macid;
	parm.arp_rsp_id = info->arp_rsp_id;

	//if (parp_info_content)
	//	PLTFM_MEMCPY(&parm.ndp_info_content, parp_info_content,
	//		     sizeof(struct _arp_info_parm_) * 2);

	ret = send_h2c_arp_ofld(adapter, &parm);
	if (ret)
		return ret;
	return MACSUCCESS;
}

u32 mac_cfg_ndp_ofld(struct mac_ax_adapter *adapter,
		     u8 macid,
		     struct mac_ax_ndp_ofld_info *info,
		     struct mac_ax_ndp_info_parm_ *content)
{
	u32 ret = 0;
	struct ndp_ofld parm;

	if (adapter->sm.fwdl != MAC_AX_FWDL_INIT_RDY)
		return MACNOFW;

	PLTFM_MEMSET(&parm, 0, sizeof(struct ndp_ofld));
	parm.ndp_en = info->ndp_en;
	parm.na_id = info->na_id;
	parm.mac_id = macid;

	if (content)
		PLTFM_MEMCPY(&parm.ndp_info_content, content,
			     sizeof(struct mac_ax_ndp_info_parm_) * 2);

	ret = send_h2c_ndp_ofld(adapter, &parm);
	if (ret)
		return ret;

	return MACSUCCESS;
}

u32 mac_cfg_realwow(struct mac_ax_adapter *adapter,
		    u8 macid,
		    struct mac_ax_realwow_info *info,
		    struct mac_ax_realwowv2_info_parm_ *content)
{
	u32 ret = 0;
	struct realwow parm;

	if (adapter->sm.fwdl != MAC_AX_FWDL_INIT_RDY)
		return MACNOFW;

	PLTFM_MEMSET(&parm, 0, sizeof(struct realwow));
	parm.realwow_en = info->realwow_en;
	parm.auto_wakeup = info->auto_wakeup;
	parm.mac_id = macid;
	parm.keepalive_id = info->keepalive_id;
	parm.wakeup_pattern_id = info->wakeup_pattern_id;
	parm.ack_pattern_id = info->ack_pattern_id;
	if (content)
		PLTFM_MEMCPY(&parm.realwow_info_content, content,
			     sizeof(struct mac_ax_realwowv2_info_parm_));

	ret = send_h2c_realwow(adapter, &parm);
	if (ret)
		return ret;
	return MACSUCCESS;
}

u32 mac_cfg_nlo(struct mac_ax_adapter *adapter,
		u8 macid,
		struct mac_ax_nlo_info *info,
		struct mac_ax_nlo_networklist_parm_ *content)
{
	u32 ret = 0;
	struct nlo parm;

	if (adapter->sm.fwdl != MAC_AX_FWDL_INIT_RDY)
		return MACNOFW;

	PLTFM_MEMSET(&parm, 0, sizeof(struct nlo));
	parm.nlo_en = info->nlo_en;
	parm.nlo_32k_en = info->nlo_32k_en;
	parm.ignore_cipher_type = !info->compare_cipher_type;
	parm.mac_id = macid;

	if (content)
		PLTFM_MEMCPY(&parm.nlo_networklistinfo_content,
			     content,
			     sizeof(struct mac_ax_nlo_networklist_parm_));

	ret = send_h2c_nlo(adapter, &parm);
	if (ret)
		return ret;
	return MACSUCCESS;
}

u32 mac_cfg_uphy_ctrl(struct mac_ax_adapter *adapter,
		      struct mac_ax_uphy_ctrl_info *info)
{
	if (adapter->sm.fwdl != MAC_AX_FWDL_INIT_RDY)
		return MACNOFW;

	return MACSUCCESS;
}

u32 mac_cfg_wowcam_upd(struct mac_ax_adapter *adapter,
		       struct mac_ax_wowcam_upd_info *info)
{
	u32 ret = 0;
	struct wowcam_upd parm;

	if (adapter->sm.fwdl != MAC_AX_FWDL_INIT_RDY)
		return MACNOFW;

	PLTFM_MEMSET(&parm, 0, sizeof(struct wowcam_upd));
	parm.r_w = info->r_w;
	parm.idx = info->idx;
	parm.wkfm1 = info->wkfm1;
	parm.wkfm2 = info->wkfm2;
	parm.wkfm3 = info->wkfm3;
	parm.wkfm4 = info->wkfm4;
	parm.crc = info->crc;
	parm.negative_pattern_match = info->negative_pattern_match;
	parm.skip_mac_hdr = info->skip_mac_hdr;
	parm.uc = info->uc;
	parm.mc = info->mc;
	parm.bc = info->bc;
	parm.valid = info->valid;

	ret = send_h2c_wowcam_upd(adapter, &parm);
	if (ret)
		return ret;

	return MACSUCCESS;
}

u32 get_wake_reason(struct mac_ax_adapter *adapter, u8 *wowlan_wake_reason)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);
	struct mac_ax_c2hreg_offset *c2hreg_offset;

	c2hreg_offset = p_ops->get_c2hreg_offset(adapter);
	if (!c2hreg_offset) {
		PLTFM_MSG_ERR("%s: get c2hreg offset fail\n", __func__);
		return MACNPTR;
	}

	*wowlan_wake_reason = MAC_REG_R8(c2hreg_offset->data3 + 3);

	return MACSUCCESS;
}

u32 mac_get_wow_wake_rsn(struct mac_ax_adapter *adapter, u8 *wake_rsn,
			 u8 *reset)
{
	u32 ret = MACSUCCESS;

	ret = get_wake_reason(adapter, wake_rsn);
	if (ret != MACSUCCESS)
		return ret;

	switch (*wake_rsn) {
	case RTW_MAC_WOW_DMAC_ERROR_OCCURRED:
	case RTW_MAC_WOW_EXCEPTION_OCCURRED:
	case RTW_MAC_WOW_L0_TO_L1_ERROR_OCCURRED:
	case RTW_MAC_WOW_ASSERT_OCCURRED:
	case RTW_MAC_WOW_L2_ERROR_OCCURRED:
	case RTW_MAC_WOW_WDT_TIMEOUT_WAKE:
		*reset = 1;
		break;
	default:
		*reset = 0;
		break;
	}

	return MACSUCCESS;
}

u32 mac_cfg_wow_sleep(struct mac_ax_adapter *adapter,
		      u8 sleep)
{
	u32 ret;
	u32 val32;
	u8 dbg_page;
	struct mac_ax_phy_rpt_cfg cfg;
	struct mac_ax_ops *mac_ops = adapter_to_mac_ops(adapter);
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	PLTFM_MEMSET(&cfg, 0, sizeof(struct mac_ax_phy_rpt_cfg));
#if MAC_AX_FW_REG_OFLD
	if (adapter->sm.fwdl == MAC_AX_FWDL_INIT_RDY) {
		if (sleep) {
			ret = redu_wowlan_rx_qta(adapter);
			if (ret != MACSUCCESS) {
				PLTFM_MSG_ERR("[ERR]patch reduce rx qta %d\n", ret);
				return ret;
			}

			cfg.type = MAC_AX_PPDU_STATUS;
			cfg.en = 0;
			ret = mac_ops->cfg_phy_rpt(adapter, &cfg);
			if (ret != MACSUCCESS) {
				PLTFM_MSG_ERR("[ERR]cfg_phy_rpt failed %d\n", ret);
				return ret;
			}

			ret = MAC_REG_W_OFLD(R_AX_RX_FUNCTION_STOP, B_AX_HDR_RX_STOP, 1, 0);
			if (ret)
				return ret;
			ret = MAC_REG_W_OFLD(R_AX_RX_FLTR_OPT, B_AX_SNIFFER_MODE, 0, 0);
			if (ret)
				return ret;
			ret = MAC_REG_W32_OFLD(R_AX_ACTION_FWD0, 0x00000000, 0);
			if (ret)
				return ret;
			ret = MAC_REG_W32_OFLD(R_AX_ACTION_FWD1, 0x00000000, 0);
			if (ret)
				return ret;
			ret = MAC_REG_W32_OFLD(R_AX_TF_FWD, 0x00000000, 0);
			if (ret)
				return ret;
			ret = MAC_REG_W32_OFLD(R_AX_HW_RPT_FWD, 0x00000000, 1);
			if (ret)
				return ret;
		} else {
			ret = restr_wowlan_rx_qta(adapter);
			if (ret != MACSUCCESS) {
				PLTFM_MSG_ERR("[ERR]patch resume rx qta %d\n", ret);
				return ret;
			}

			ret = MAC_REG_W_OFLD(R_AX_RX_FUNCTION_STOP, B_AX_HDR_RX_STOP, 0, 0);
			if (ret)
				return ret;
			ret = MAC_REG_W32_OFLD(R_AX_ACTION_FWD0, TRXCFG_MPDU_PROC_ACT_FRWD, 0);
			if (ret)
				return ret;
			ret = MAC_REG_W32_OFLD(R_AX_TF_FWD, TRXCFG_MPDU_PROC_TF_FRWD, 1);
			if (ret)
				return ret;

			cfg.type = MAC_AX_PPDU_STATUS;
			cfg.en = 1;
			ret = mac_ops->cfg_phy_rpt(adapter, &cfg);
			if (ret != MACSUCCESS) {
				PLTFM_MSG_ERR("[ERR]cfg_phy_rpt failed %d\n", ret);
				return ret;
			}
		}
		return MACSUCCESS;
	}
#endif
	if (sleep) {
		ret = redu_wowlan_rx_qta(adapter);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("[ERR]patch reduce rx qta %d\n", ret);
			return ret;
		}
		val32 = MAC_REG_R32(R_AX_RX_FUNCTION_STOP);
		val32 |= B_AX_HDR_RX_STOP;
		MAC_REG_W32(R_AX_RX_FUNCTION_STOP, val32);
		val32 = MAC_REG_R32(R_AX_RX_FLTR_OPT);
		val32 &= ~B_AX_SNIFFER_MODE;
		MAC_REG_W32(R_AX_RX_FLTR_OPT, val32);

		cfg.type = MAC_AX_PPDU_STATUS;
		cfg.en = 0;
		ret = mac_ops->cfg_phy_rpt(adapter, &cfg);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("[ERR]cfg_phy_rpt failed %d\n", ret);
			return ret;
		}

		MAC_REG_W32(R_AX_ACTION_FWD0, 0x00000000);
		MAC_REG_W32(R_AX_ACTION_FWD1, 0x00000000);
		MAC_REG_W32(R_AX_TF_FWD, 0x00000000);
		MAC_REG_W32(R_AX_HW_RPT_FWD, 0x00000000);
	} else {
		ret = restr_wowlan_rx_qta(adapter);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("[ERR]patch resume rx qta %d\n", ret);
			return ret;
		}
		val32 = MAC_REG_R32(R_AX_RX_FUNCTION_STOP);
		val32 &= ~B_AX_HDR_RX_STOP;
		MAC_REG_W32(R_AX_RX_FUNCTION_STOP, val32);
		val32 = MAC_REG_R32(R_AX_RX_FLTR_OPT);
		MAC_REG_W32(R_AX_RX_FLTR_OPT, val32);

		cfg.type = MAC_AX_PPDU_STATUS;
		cfg.en = 1;
		ret = mac_ops->cfg_phy_rpt(adapter, &cfg);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("[ERR]cfg_phy_rpt failed %d\n", ret);
			return ret;
		}

		MAC_REG_W32(R_AX_ACTION_FWD0, TRXCFG_MPDU_PROC_ACT_FRWD);
		MAC_REG_W32(R_AX_TF_FWD, TRXCFG_MPDU_PROC_TF_FRWD);

		PLTFM_MSG_ERR("[wow] Start to dump PLE debug pages\n");
		for (dbg_page = 0; dbg_page < 4; dbg_page++)
			mac_dump_ple_dbg_page(adapter, dbg_page);
	}

	return MACSUCCESS;
}

u32 mac_get_wow_fw_status(struct mac_ax_adapter *adapter, u8 *status,
			  u8 func_en)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	if (func_en)
		func_en = 1;

	*status = !!((MAC_REG_R8(R_AX_WOW_CTRL) & B_AX_WOW_WOWEN));

	if (func_en == *status)
		*status = 1;
	else
		*status = 0;

	return MACSUCCESS;
}

u32 _mac_request_aoac_report_rx_rdy(struct mac_ax_adapter *adapter)
{
	u32 ret;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	u8 *buf;
	struct fwcmd_aoac_report_req *fwcmd_aoac_rpt_req;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_DATA);

	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, sizeof(struct fwcmd_aoac_report_req));
	if (!buf) {
		ret = MACNOBUF;
		goto fail;
	}

	PLTFM_MSG_ERR("Request aoac_rpt\n");
	val32 = MAC_REG_R32(R_AX_CH12_TXBD_IDX);
	PLTFM_MSG_ERR("CH12_TXBD=%x\n", val32);
	MAC_REG_W32(R_AX_PLE_DBG_FUN_INTF_CTL, 80010002);
	val32 = MAC_REG_R32(R_AX_PLE_DBG_FUN_INTF_DATA);
	PLTFM_MSG_ERR("PLE_C2H=%x\n", val32);
	MAC_REG_W32(R_AX_PLE_DBG_FUN_INTF_CTL, 80010003);
	val32 = MAC_REG_R32(R_AX_PLE_DBG_FUN_INTF_DATA);
	PLTFM_MSG_ERR("PLE_H2C=%x\n", val32);
	val32 = mac_sram_dbg_read(adapter, 0x400, AXIDMA_SEL);
	PLTFM_MSG_ERR("AXI_H2C=%x\n", val32);
	val32 = mac_sram_dbg_read(adapter, 0x420, AXIDMA_SEL);
	PLTFM_MSG_ERR("AXI_C2H=%x\n", val32);
	val32 = MAC_REG_R32(R_AX_RXQ_RXBD_IDX);
	PLTFM_MSG_ERR("RXQ_RXBD=%x\n\n", val32);

	fwcmd_aoac_rpt_req = (struct fwcmd_aoac_report_req *)buf;
	fwcmd_aoac_rpt_req->dword0 =
	cpu_to_le32(FWCMD_H2C_AOAC_REPORT_REQ_RX_READY);

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C, FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_WOW, FWCMD_H2C_FUNC_AOAC_REPORT_REQ,
			      1, 0);
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
	if (ret) {
		PLTFM_MSG_ERR("[ERR]platform tx: %d\n", ret);
		adapter->sm.aoac_rpt = MAC_AX_AOAC_RPT_ERROR;
		goto fail;
	}

	h2cb_free(adapter, h2cb);

	return MACSUCCESS;
fail:
	h2cb_free(adapter, h2cb);

	return ret;
}

u32 _mac_request_aoac_report_rx_not_rdy(struct mac_ax_adapter *adapter)
{
	struct mac_ax_wowlan_info *wow_info = &adapter->wowlan_info;
	struct mac_ax_aoac_report *aoac_rpt = (struct mac_ax_aoac_report *)wow_info->aoac_report;
	struct mac_ax_h2creg_info h2c_info = {0};
	struct mac_ax_c2hreg_poll c2h_poll = {0};
	struct fwcmd_c2hreg *c2h_content = &c2h_poll.c2hreg_cont.c2h_content;
	u32 ret;
	u8 *p_iv;

	h2c_info.id = FWCMD_H2CREG_FUNC_AOAC_RPT_1;
	h2c_info.content_len = sizeof(struct fwcmd_aoac_rpt_1);

	c2h_poll.polling_id = FWCMD_C2HREG_FUNC_AOAC_RPT_1;
	c2h_poll.retry_cnt = WOW_GET_AOAC_RPT_C2H_CNT;
	c2h_poll.retry_wait_us = WOW_GET_AOAC_RPT_C2H_DLY;

	ret = proc_msg_reg(adapter, &h2c_info, &c2h_poll);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("%s: get aoac rpt(%d) fail: %d\n",
			      __func__, FWCMD_C2HREG_FUNC_AOAC_RPT_1, ret);
		return ret;
	}

	aoac_rpt->key_idx = GET_FIELD(c2h_content->dword0,
				      FWCMD_C2HREG_AOAC_RPT_1_KEY_IDX);
	aoac_rpt->rekey_ok = GET_FIELD(c2h_content->dword0,
				       FWCMD_C2HREG_AOAC_RPT_1_REKEY_OK);
	p_iv = (&aoac_rpt->gtk_rx_iv_0[0] + (aoac_rpt->key_idx * IV_LENGTH));
	p_iv[0] = GET_FIELD(c2h_content->dword1,
			    FWCMD_C2HREG_AOAC_RPT_1_IV_0);
	p_iv[1] = GET_FIELD(c2h_content->dword1,
			    FWCMD_C2HREG_AOAC_RPT_1_IV_1);
	p_iv[2] = GET_FIELD(c2h_content->dword1,
			    FWCMD_C2HREG_AOAC_RPT_1_IV_2);
	p_iv[3] = GET_FIELD(c2h_content->dword1,
			    FWCMD_C2HREG_AOAC_RPT_1_IV_3);
	p_iv[4] = GET_FIELD(c2h_content->dword2,
			    FWCMD_C2HREG_AOAC_RPT_1_IV_4);
	p_iv[5] = GET_FIELD(c2h_content->dword2,
			    FWCMD_C2HREG_AOAC_RPT_1_IV_5);
	p_iv[6] = GET_FIELD(c2h_content->dword2,
			    FWCMD_C2HREG_AOAC_RPT_1_IV_6);
	p_iv[7] = GET_FIELD(c2h_content->dword2,
			    FWCMD_C2HREG_AOAC_RPT_1_IV_7);
	aoac_rpt->ptk_rx_iv[0] = GET_FIELD(c2h_content->dword3,
					   FWCMD_C2HREG_AOAC_RPT_1_PKT_IV_0);
	aoac_rpt->ptk_rx_iv[1] = GET_FIELD(c2h_content->dword3,
					   FWCMD_C2HREG_AOAC_RPT_1_PKT_IV_1);
	aoac_rpt->ptk_rx_iv[2] = GET_FIELD(c2h_content->dword3,
					   FWCMD_C2HREG_AOAC_RPT_1_PKT_IV_2);
	aoac_rpt->ptk_rx_iv[3] = GET_FIELD(c2h_content->dword3,
					   FWCMD_C2HREG_AOAC_RPT_1_PKT_IV_3);

	h2c_info.id = FWCMD_H2CREG_FUNC_AOAC_RPT_2;
	h2c_info.content_len = sizeof(struct fwcmd_aoac_rpt_2);

	c2h_poll.polling_id = FWCMD_C2HREG_FUNC_AOAC_RPT_2;
	c2h_poll.retry_cnt = WOW_GET_AOAC_RPT_C2H_CNT;
	c2h_poll.retry_wait_us = WOW_GET_AOAC_RPT_C2H_DLY;
	ret = proc_msg_reg(adapter, &h2c_info, &c2h_poll);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("%s: get aoac rpt(%d) fail: %d\n",
			      __func__, FWCMD_C2HREG_FUNC_AOAC_RPT_2, ret);
		return ret;
	}

	aoac_rpt->ptk_rx_iv[4] = GET_FIELD(c2h_content->dword0,
					   FWCMD_C2HREG_AOAC_RPT_2_PKT_IV_4);
	aoac_rpt->ptk_rx_iv[5] = GET_FIELD(c2h_content->dword0,
					   FWCMD_C2HREG_AOAC_RPT_2_PKT_IV_5);
	aoac_rpt->ptk_rx_iv[6] = GET_FIELD(c2h_content->dword1,
					   FWCMD_C2HREG_AOAC_RPT_2_PKT_IV_6);
	aoac_rpt->ptk_rx_iv[7] = GET_FIELD(c2h_content->dword1,
					   FWCMD_C2HREG_AOAC_RPT_2_PKT_IV_7);
	aoac_rpt->igtk_ipn[0] = GET_FIELD(c2h_content->dword1,
					  FWCMD_C2HREG_AOAC_RPT_2_IGTK_IPN_0);
	aoac_rpt->igtk_ipn[1] = GET_FIELD(c2h_content->dword1,
					  FWCMD_C2HREG_AOAC_RPT_2_IGTK_IPN_1);
	aoac_rpt->igtk_ipn[2] = GET_FIELD(c2h_content->dword2,
					  FWCMD_C2HREG_AOAC_RPT_2_IGTK_IPN_2);
	aoac_rpt->igtk_ipn[3] = GET_FIELD(c2h_content->dword2,
					  FWCMD_C2HREG_AOAC_RPT_2_IGTK_IPN_3);
	aoac_rpt->igtk_ipn[4] = GET_FIELD(c2h_content->dword2,
					  FWCMD_C2HREG_AOAC_RPT_2_IGTK_IPN_4);
	aoac_rpt->igtk_ipn[5] = GET_FIELD(c2h_content->dword2,
					  FWCMD_C2HREG_AOAC_RPT_2_IGTK_IPN_5);
	aoac_rpt->igtk_ipn[6] = GET_FIELD(c2h_content->dword3,
					  FWCMD_C2HREG_AOAC_RPT_2_IGTK_IPN_6);
	aoac_rpt->igtk_ipn[7] = GET_FIELD(c2h_content->dword3,
					  FWCMD_C2HREG_AOAC_RPT_2_IGTK_IPN_7);

	return MACSUCCESS;
}

u32 mac_request_aoac_report(struct mac_ax_adapter *adapter,
			    u8 rx_ready)
{
	u32 ret;
	struct mac_ax_wowlan_info *wow_info = &adapter->wowlan_info;

	if (adapter->sm.aoac_rpt != MAC_AX_AOAC_RPT_IDLE)
		return MACPROCERR;

	if (wow_info->aoac_report) {
		PLTFM_FREE(wow_info->aoac_report,
			   sizeof(struct mac_ax_aoac_report));
	}
	wow_info->aoac_report = (u8 *)PLTFM_MALLOC(sizeof(struct mac_ax_aoac_report));
	if (!wow_info->aoac_report) {
		PLTFM_MSG_ERR("%s: malloc fail\n", __func__);
		return MACBUFALLOC;
	}

	adapter->sm.aoac_rpt = MAC_AX_AOAC_RPT_H2C_SENDING;

	if (rx_ready)
		ret = _mac_request_aoac_report_rx_rdy(adapter);
	else
		ret = _mac_request_aoac_report_rx_not_rdy(adapter);

	return ret;
}

u32 mac_read_aoac_report(struct mac_ax_adapter *adapter,
			 struct mac_ax_aoac_report *rpt_buf, u8 rx_ready)
{
	struct mac_ax_wowlan_info *wow_info = &adapter->wowlan_info;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 ret = MACSUCCESS;
	u8 cnt = 200;
	u32 val32;

	while ((rx_ready) && (adapter->sm.aoac_rpt != MAC_AX_AOAC_RPT_H2C_DONE)) {
		PLTFM_DELAY_MS(1);
		if (--cnt == 0) {
			PLTFM_MSG_ERR("[ERR] read aoac report(%d) fail\n",
				      adapter->sm.aoac_rpt);
			adapter->sm.aoac_rpt = MAC_AX_AOAC_RPT_IDLE;
			val32 = MAC_REG_R32(R_AX_CH12_TXBD_IDX);
			PLTFM_MSG_ERR("CH12_TXBD=%x\n", val32);
			MAC_REG_W32(R_AX_PLE_DBG_FUN_INTF_CTL, 80010002);
			val32 = MAC_REG_R32(R_AX_PLE_DBG_FUN_INTF_DATA);
			PLTFM_MSG_ERR("PLE_C2H=%x\n", val32);
			MAC_REG_W32(R_AX_PLE_DBG_FUN_INTF_CTL, 80010003);
			val32 = MAC_REG_R32(R_AX_PLE_DBG_FUN_INTF_DATA);
			PLTFM_MSG_ERR("PLE_H2C=%x\n", val32);
			val32 = mac_sram_dbg_read(adapter, 0x400, AXIDMA_SEL);
			PLTFM_MSG_ERR("AXI_H2C=%x\n", val32);
			val32 = mac_sram_dbg_read(adapter, 0x420, AXIDMA_SEL);
			PLTFM_MSG_ERR("AXI_C2H=%x\n", val32);
			val32 = MAC_REG_R32(R_AX_RXQ_RXBD_IDX);
			PLTFM_MSG_ERR("RXQ_RXBD=%x\n\n", val32);
			return MACPOLLTO;
		}
	}

	if (wow_info->aoac_report) {
		PLTFM_MEMCPY(rpt_buf, wow_info->aoac_report,
			     sizeof(struct mac_ax_aoac_report));
		PLTFM_FREE(wow_info->aoac_report,
			   sizeof(struct mac_ax_aoac_report));
		wow_info->aoac_report = NULL;
	} else {
		PLTFM_MSG_ERR("[ERR] aoac report memory allocate fail\n");
		ret = MACBUFALLOC;
	}

	adapter->sm.aoac_rpt = MAC_AX_AOAC_RPT_IDLE;

	return ret;
}

u32 mac_check_aoac_report_done(struct mac_ax_adapter *adapter)
{
	PLTFM_MSG_TRACE("[TRACE]%s: curr state: %d\n", __func__,
			adapter->sm.aoac_rpt);

	if (adapter->sm.aoac_rpt == MAC_AX_AOAC_RPT_H2C_DONE)
		return MACSUCCESS;
	else
		return MACPROCBUSY;
}

u32 mac_wow_stop_trx(struct mac_ax_adapter *adapter)
{
	struct mac_ax_h2creg_info h2c_info;
	struct mac_ax_c2hreg_poll c2h_poll;
	u32 ret;

	if (adapter->sm.wow_stoptrx_stat == MAC_AX_WOW_STOPTRX_BUSY) {
		PLTFM_MSG_ERR("[ERR]wow stop trx busy\n");
		return MACPROCERR;
	} else if (adapter->sm.wow_stoptrx_stat == MAC_AX_WOW_STOPTRX_FAIL) {
		PLTFM_MSG_WARN("[WARN]prev wow stop trx fail\n");
	}

	adapter->sm.wow_stoptrx_stat = MAC_AX_WOW_STOPTRX_BUSY;

	h2c_info.id = FWCMD_H2CREG_FUNC_WOW_TRX_STOP;
	h2c_info.content_len = 0;
	h2c_info.h2c_content.dword0 = 0;
	h2c_info.h2c_content.dword1 = 0;
	h2c_info.h2c_content.dword2 = 0;
	h2c_info.h2c_content.dword3 = 0;

	c2h_poll.polling_id = FWCMD_C2HREG_FUNC_WOW_TRX_STOP;
	c2h_poll.retry_cnt = WOW_GET_STOP_TRX_C2H_CNT;
	c2h_poll.retry_wait_us = WOW_GET_STOP_TRX_C2H_DLY;

	ret = proc_msg_reg(adapter, &h2c_info, &c2h_poll);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]wow stoptrx proc msg reg %d\n", ret);
		adapter->sm.wow_stoptrx_stat = MAC_AX_WOW_STOPTRX_FAIL;
		return ret;
	}

	adapter->sm.wow_stoptrx_stat = MAC_AX_WOW_STOPTRX_IDLE;

	return MACSUCCESS;
}

u32 mac_cfg_wow_auto_test(struct mac_ax_adapter *adapter, u8 rxtest)
{
	u32 ret;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	u8 *buf;
	struct fwcmd_wow_auto_test *fwcmd_wow_auto_test;

	if (adapter->sm.fwdl != MAC_AX_FWDL_INIT_RDY) {
		PLTFM_MSG_WARN("%s fw not ready\n", __func__);
		return MACFWNONRDY;
	}

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_DATA);

	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, sizeof(struct fwcmd_wow_auto_test));
	if (!buf) {
		ret = MACNOBUF;
		goto fail;
	}

	fwcmd_wow_auto_test = (struct fwcmd_wow_auto_test *)buf;
	fwcmd_wow_auto_test->dword0 =
	cpu_to_le32((rxtest ? FWCMD_H2C_WOW_AUTO_TEST_RX_TEST : 0));

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C, FWCMD_H2C_CAT_TEST,
			      FWCMD_H2C_CL_WOW_TEST, FWCMD_H2C_FUNC_WOW_AUTO_TEST,
			      0, 0);
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
	if (ret) {
		PLTFM_MSG_ERR("[ERR]platform tx: %d\n", ret);
		adapter->sm.aoac_rpt = MAC_AX_AOAC_RPT_ERROR;
		goto fail;
	}

	h2cb_free(adapter, h2cb);

	return MACSUCCESS;
fail:
	h2cb_free(adapter, h2cb);

	return ret;
}

static void dump_bytes(struct mac_ax_adapter *adapter, u8 *start, u32 size)
{
#if PROXY_MDNS_DUMP
	u32 idx;

	for (idx = 0; idx < size; idx += 4, start += 4) {
		switch (size - idx) {
		case 1:
			PLTFM_MSG_TRACE("- 0x%x: 0x%x\n", idx, *start);
			break;
		case 2:
			PLTFM_MSG_TRACE("- 0x%x: 0x%x 0x%x\n", idx, *start, *(start + 1));
			break;
		case 3:
			PLTFM_MSG_TRACE("- 0x%x: 0x%x 0x%x 0x%x\n", idx,
					*start, *(start + 1), *(start + 2));
			break;
		default:
			PLTFM_MSG_TRACE("- 0x%x: 0x%x 0x%x 0x%x 0x%x\n", idx,
					*start, *(start + 1), *(start + 2), *(start + 3));
			break;
		}
	}
#endif
}

static void mdns_sprintf(struct mac_ax_adapter *adapter, char *content, u8 *in, u32 len)
{
#if PROXY_MDNS_DUMP
	u32 idx;
	u32 write_idx;

	write_idx = 0;
	PLTFM_MEMSET(content, 0, 128);
	content[write_idx++] = '<';
	for (idx = 0; idx < len; idx++) {
		if (in[idx] >= 0x20) {
			content[write_idx++] = in[idx];
		} else {
			content[write_idx++] = '[';
			content[write_idx++] = '0' + (in[idx] >> 4);
			content[write_idx++] = '0' + (in[idx] & 0xff);
			content[write_idx++] = ']';
		}
	}
	content[write_idx++] = '>';
	content[write_idx] = 0;
#endif
}

static void dump_mdns_machine(struct mac_ax_adapter *adapter,
			      struct rtw_hal_mac_proxy_mdns_machine *machine)
{
#if PROXY_MDNS_DUMP

	char p[128];

	mdns_sprintf(adapter, p, machine->name, machine->len);
	PLTFM_MSG_TRACE("[MDNS][Mchn] %s (%d)\n", &p, machine->len);
#endif
}

static void dump_mdns_rsp_hdr(struct mac_ax_adapter *adapter,
			      struct rtw_hal_mac_proxy_mdns_rsp_hdr h)
{
#if PROXY_MDNS_DUMP
	PLTFM_MSG_TRACE("[MDNS] hdr: type (0x%x 0x%x), cf_cls (0x%x 0x%x), ttl (%d), len (%d)\n",
			h.rspTypeB0, h.rspTypeB1,
			h.cache_class_B0, h.cache_class_B1, h.ttl, h.dataLen);
#endif
}

static void dump_mdns_serv(struct mac_ax_adapter *adapter, struct rtw_hal_mac_proxy_mdns_service *s)
{
#if PROXY_MDNS_DUMP
	char p[128];

	mdns_sprintf(adapter, p, s->name, s->name_len);
	PLTFM_MSG_TRACE("[Serv] name %s (%d)\n", p, s->name_len);
	PLTFM_MSG_TRACE("[Serv] =============>\n");
	dump_mdns_rsp_hdr(adapter, s->hdr);
	PLTFM_MSG_TRACE("[Serv] prio (0x%x), weight (0x%x), port (%d)\n",
			s->priority, s->weight, s->port);
	mdns_sprintf(adapter, p, s->target, s->target_len);
	PLTFM_MSG_TRACE("[Serv] target %s (%d), compress (0x%x), hasTxt (%x), txtPktId (%d)\n",
			p, s->target_len, s->compression, s->has_txt, s->txt_pktid);
#endif
}

static void dump_mdns(struct mac_ax_adapter *adapter, struct rtw_hal_mac_proxy_mdns *mdns)
{
#if PROXY_MDNS_DUMP
	u8 idx;
	char p[128];

	PLTFM_MSG_TRACE("\n");
	PLTFM_MSG_TRACE("[MDNS] v4 pkt (%d), v6 pkt (%d), #serv (%d), #machine (%d), macid (%d)\n",
			mdns->ipv4_pktid, mdns->ipv6_pktid,
			mdns->num_supported_services, mdns->num_machine_names, mdns->macid);
	PLTFM_MSG_TRACE("[MDNS] serv0 (%d), serv1 (%d), serv2 (%d), serv3 (%d), serv4 (%d)\n",
			mdns->serv_pktid[0], mdns->serv_pktid[1], mdns->serv_pktid[2],
			mdns->serv_pktid[3], mdns->serv_pktid[4]);
	PLTFM_MSG_TRACE("[MDNS] serv5 (%d), serv6 (%d), serv7 (%d), serv8 (%d), serv9 (%d)\n",
			mdns->serv_pktid[5], mdns->serv_pktid[6], mdns->serv_pktid[7],
			mdns->serv_pktid[8], mdns->serv_pktid[9]);

	PLTFM_MSG_TRACE("\n");
	for (idx = 0; idx < mdns->num_machine_names; idx++) {
		PLTFM_MSG_TRACE("[MDNS][Mchn %d] =====>\n", idx);
		dump_mdns_machine(adapter, &mdns->machines[idx]);
	}

	PLTFM_MSG_TRACE("\n");
	PLTFM_MSG_TRACE("[MDNS][A] =============>\n");
	dump_mdns_rsp_hdr(adapter, mdns->a_rsp.hdr);
	PLTFM_MSG_TRACE("[MDNS][A] %d.%d.%d.%d\n", mdns->a_rsp.ipv4Addr[0],
			mdns->a_rsp.ipv4Addr[1], mdns->a_rsp.ipv4Addr[2], mdns->a_rsp.ipv4Addr[3]);
	dump_bytes(adapter, (u8 *)&mdns->a_rsp, sizeof(mdns->a_rsp));

	PLTFM_MSG_TRACE("\n");
	PLTFM_MSG_TRACE("[MDNS][AAAA] =============>\n");
	dump_mdns_rsp_hdr(adapter, mdns->aaaa_rsp.hdr);
	PLTFM_MSG_TRACE("[MDNS][AAAA] %x:%x:%x:%x:%x:%x:%x:%x:%x:%x:%x:%x:%x:%x:%x:%x\n",
			mdns->aaaa_rsp.ipv6Addr[0], mdns->aaaa_rsp.ipv6Addr[1],
			mdns->aaaa_rsp.ipv6Addr[2], mdns->aaaa_rsp.ipv6Addr[3],
			mdns->aaaa_rsp.ipv6Addr[4], mdns->aaaa_rsp.ipv6Addr[5],
			mdns->aaaa_rsp.ipv6Addr[6], mdns->aaaa_rsp.ipv6Addr[7],
			mdns->aaaa_rsp.ipv6Addr[8], mdns->aaaa_rsp.ipv6Addr[9],
			mdns->aaaa_rsp.ipv6Addr[10], mdns->aaaa_rsp.ipv6Addr[11],
			mdns->aaaa_rsp.ipv6Addr[12], mdns->aaaa_rsp.ipv6Addr[13],
			mdns->aaaa_rsp.ipv6Addr[14], mdns->aaaa_rsp.ipv6Addr[15]);
	dump_bytes(adapter, (u8 *)&mdns->aaaa_rsp, sizeof(mdns->aaaa_rsp));

	PLTFM_MSG_TRACE("\n");
	PLTFM_MSG_TRACE("[MDNS][PTR] =============>\n");
	dump_mdns_rsp_hdr(adapter, mdns->ptr_rsp.hdr);
	mdns_sprintf(adapter, p, mdns->ptr_rsp.domain, mdns->ptr_rsp.hdr.dataLen - 2);
	PLTFM_MSG_TRACE("[MDNS][PTR] domain %s\n", &p);
	dump_bytes(adapter, (u8 *)&mdns->ptr_rsp, sizeof(mdns->ptr_rsp));
#endif
}

static void mdns_rsp_hdr_endian(struct rtw_hal_mac_proxy_mdns_rsp_hdr *hdr)
{
	hdr->ttl = cpu_to_be32(hdr->ttl);
	hdr->dataLen = cpu_to_be16(hdr->dataLen);
}

u32 mac_proxyofld(struct mac_ax_adapter *adapter, struct rtw_hal_mac_proxyofld *pcfg)
{
	u32 ret;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	u8 *buf;
	struct rtw_hal_mac_proxyofld cfg;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	struct mac_ax_multicast_info mc_info = {{0}, {0}};
	u32 val32;

	ret = MACSUCCESS;
	cfg = *pcfg;

	if (adapter->sm.fwdl != MAC_AX_FWDL_INIT_RDY)
		return MACNOFW;
	if (adapter->sm.proxy_st != MAC_AX_PROXY_IDLE)
		return MACPROCERR;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_DATA);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, sizeof(struct fwcmd_proxy));
	if (!buf) {
		h2cb_free(adapter, h2cb);
		return MACNOBUF;
	}

	PLTFM_MEMCPY(buf, &cfg, sizeof(cfg));

	ret = h2c_pkt_set_hdr(adapter, h2cb, FWCMD_TYPE_H2C, FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_PROXY, FWCMD_H2C_FUNC_PROXY, 1, 1);
	if (ret) {
		h2cb_free(adapter, h2cb);
		return ret;
	}

	ret = h2c_pkt_build_txd(adapter, h2cb);
	if (ret) {
		h2cb_free(adapter, h2cb);
		return ret;
	}

	#if MAC_AX_PHL_H2C
	ret = PLTFM_TX(h2cb);
	#else
	ret = PLTFM_TX(h2cb->data, h2cb->len);
	#endif

	if (ret) {
		h2cb_free(adapter, h2cb);
		return ret;
	}
	adapter->sm.proxy_st = MAC_AX_PROXY_SENDING;
	h2cb_free(adapter, h2cb);

	if (cfg.mdns_v4_rsp || cfg.mdns_v4_wake || cfg.mdns_v6_rsp || cfg.mdns_v6_wake) {
		val32 = MAC_REG_R32(R_AX_RX_FLTR_OPT);
		val32 |= B_AX_A_MC_LIST_CAM_MATCH;
		MAC_REG_W32(R_AX_RX_FLTR_OPT, val32);
		if (cfg.mdns_v4_rsp || cfg.mdns_v4_wake) {
			PLTFM_MEMCPY(mc_info.mc_addr, mdns_v4_multicast_addr, 6);
			mc_info.mc_msk = MAC_AX_MSK_NONE;
			mac_cfg_multicast(adapter, 1, &mc_info);
		}
		if (cfg.mdns_v6_rsp || cfg.mdns_v6_wake) {
			PLTFM_MEMCPY(mc_info.mc_addr, mdns_v6_multicast_addr, 6);
			mc_info.mc_msk = MAC_AX_MSK_NONE;
			mac_cfg_multicast(adapter, 1, &mc_info);
		}
	}
	if (cfg.wsd_v4_wake || cfg.wsd_v6_wake) {
		val32 = MAC_REG_R32(R_AX_RX_FLTR_OPT);
		val32 |= B_AX_A_MC_LIST_CAM_MATCH;
		MAC_REG_W32(R_AX_RX_FLTR_OPT, val32);
		if (cfg.wsd_v4_wake) {
			PLTFM_MEMCPY(mc_info.mc_addr, wsd_v4_multicast_addr, 6);
			mc_info.mc_msk = MAC_AX_MSK_NONE;
			mac_cfg_multicast(adapter, 1, &mc_info);
		}
		if (cfg.wsd_v6_wake) {
			PLTFM_MEMCPY(mc_info.mc_addr, wsd_v6_multicast_addr, 6);
			mc_info.mc_msk = MAC_AX_MSK_NONE;
			mac_cfg_multicast(adapter, 1, &mc_info);
		}
	}

	return ret;
}

u32 mac_proxy_mdns(struct mac_ax_adapter *adapter, struct rtw_hal_mac_proxy_mdns *pmdns)
{
	u8 *buf;
	u32 ret;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	u8 idx;
	struct rtw_hal_mac_proxy_mdns mdns;

	ret = MACSUCCESS;
	mdns = *pmdns;

	if (adapter->sm.fwdl != MAC_AX_FWDL_INIT_RDY)
		return MACNOFW;
	if (adapter->sm.proxy_st != MAC_AX_PROXY_IDLE)
		return MACPROCERR;

	PLTFM_MSG_TRACE("[MDNS] =============>\n");
	dump_mdns(adapter, &mdns);
	// dump_bytes(adapter, (u8 *)&mdns, sizeof(struct rtw_hal_mac_proxy_mdns));

	for (idx = 0; idx < RTW_PHL_PROXY_MDNS_MAX_MACHINE_NUM; idx++)
		mdns.machines[idx].len = cpu_to_le32(mdns.machines[idx].len);
	mdns_rsp_hdr_endian(&mdns.a_rsp.hdr);
	mdns_rsp_hdr_endian(&mdns.aaaa_rsp.hdr);
	mdns_rsp_hdr_endian(&mdns.ptr_rsp.hdr);

	// dump_bytes(adapter, (u8 *)&mdns, sizeof(struct rtw_hal_mac_proxy_mdns));

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_LONG_DATA);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, sizeof(struct rtw_hal_mac_proxy_mdns));
	if (!buf) {
		h2cb_free(adapter, h2cb);
		return MACNOBUF;
	}

	PLTFM_MEMCPY(buf, (u8 *)&mdns, sizeof(struct rtw_hal_mac_proxy_mdns));

	ret = h2c_pkt_set_hdr(adapter, h2cb, FWCMD_TYPE_H2C, FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_PROXY, FWCMD_H2C_FUNC_MDNS, 1, 1);
	if (ret) {
		h2cb_free(adapter, h2cb);
		return ret;
	}

	ret = h2c_pkt_build_txd(adapter, h2cb);
	if (ret) {
		h2cb_free(adapter, h2cb);
		return ret;
	}

	#if MAC_AX_PHL_H2C
	ret = PLTFM_TX(h2cb);
	#else
	ret = PLTFM_TX(h2cb->data, h2cb->len);
	#endif

	if (ret) {
		h2cb_free(adapter, h2cb);
		return ret;
	}
	adapter->sm.proxy_st = MAC_AX_PROXY_SENDING;
	h2cb_free(adapter, h2cb);

	return ret;
}

u32 mac_proxy_mdns_serv_pktofld(struct mac_ax_adapter *adapter,
				struct rtw_hal_mac_proxy_mdns_service *pserv, u8 *pktid)
{
	u16 idx;
	u8 *buf;
	u16 len;
	u32 ret;
	struct rtw_hal_mac_proxy_mdns_service serv;

	serv = *pserv;
	ret = MACSUCCESS;
	len = sizeof(struct rtw_hal_mac_proxy_mdns_service) + serv.name_len + serv.target_len;
	len = len - (sizeof(u8 *) * 2) - 1 - 1; //get rid of *name, *target, target_len, txt_id
	buf = (u8 *)PLTFM_MALLOC(len);

	PLTFM_MSG_TRACE("\n");
	PLTFM_MSG_TRACE("[MDNS][Serv] =============>\n");
	dump_mdns_serv(adapter, &serv);

	mdns_rsp_hdr_endian(&serv.hdr);
	serv.priority = cpu_to_be16(serv.priority);
	serv.weight = cpu_to_be16(serv.weight);
	serv.port = cpu_to_be16(serv.port);

	idx = 0;

	buf[idx++] = serv.name_len;

	PLTFM_MEMCPY(buf + idx, serv.name, serv.name_len);
	idx += serv.name_len;

	PLTFM_MEMCPY(buf + idx, &serv.hdr, RTW_PHL_PROXY_MDNS_RSP_HDR_LEN);
	idx += RTW_PHL_PROXY_MDNS_RSP_HDR_LEN;

	PLTFM_MEMCPY(buf + idx, &serv.priority, 2);
	idx += 2;

	PLTFM_MEMCPY(buf + idx, &serv.weight, 2);
	idx += 2;

	PLTFM_MEMCPY(buf + idx, &serv.port, 2);
	idx += 2;

	PLTFM_MEMCPY(buf + idx, serv.target, serv.target_len);
	idx += serv.target_len;

	buf[idx++] = serv.compression;
	buf[idx++] = serv.compression_loc;
	buf[idx++] = serv.has_txt;
	buf[idx++] = serv.txt_pktid;

	dump_bytes(adapter, buf, len);
	ret = mac_add_pkt_ofld(adapter, buf, len, pktid);
	PLTFM_MSG_TRACE("[MDNS][Serv] ret %d, pktid %d\n", ret, *pktid);
	PLTFM_FREE(buf, len);
	return ret;
}

u32 mac_proxy_mdns_txt_pktofld(struct mac_ax_adapter *adapter,
			       struct rtw_hal_mac_proxy_mdns_txt *ptxt, u8 *pktid)
{
	u16 idx;
	u8 *buf;
	u16 len;
	u32 ret;
	struct rtw_hal_mac_proxy_mdns_txt txt;

	txt = *ptxt;
	ret = MACSUCCESS;
	len = sizeof(struct rtw_hal_mac_proxy_mdns_txt) + txt.content_len;
	len = len - sizeof(u16) - sizeof(u8 *);
	buf = (u8 *)PLTFM_MALLOC(len);

	PLTFM_MSG_TRACE("\n");
	PLTFM_MSG_TRACE("[MDNS][Txt] =============>\n");
	dump_mdns_rsp_hdr(adapter, txt.hdr);
	mdns_rsp_hdr_endian(&txt.hdr);

	idx = 0;

	PLTFM_MEMCPY(buf, &txt.hdr, RTW_PHL_PROXY_MDNS_RSP_HDR_LEN);
	idx += RTW_PHL_PROXY_MDNS_RSP_HDR_LEN;

	PLTFM_MEMCPY(buf + idx, txt.content, txt.content_len);

	dump_bytes(adapter, buf, len);
	ret = mac_add_pkt_ofld(adapter, buf, len, pktid);
	PLTFM_MSG_TRACE("[MDNS][Txt] ret %d, pktid %d\n", ret, *pktid);
	PLTFM_FREE(buf, len);
	return ret;
}

u32 mac_proxy_ptcl_pattern(struct mac_ax_adapter *adapter,
			   struct rtw_hal_mac_proxy_ptcl_pattern *cfg)
{
	u8 *buf;
	u32 ret;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	u8 len;
	u32 idx;
	enum h2c_buf_class h2cb_type;
	struct fwcmd_ptcl_pattern *ptcl_pattern_hdr;
	u8 *patterns_head;
	u8 curr_pattern_len;

	len = sizeof(struct fwcmd_ptcl_pattern);
	ret = MACSUCCESS;

	if (adapter->sm.fwdl != MAC_AX_FWDL_INIT_RDY)
		return MACNOFW;
	if (adapter->sm.proxy_st != MAC_AX_PROXY_IDLE)
		return MACPROCERR;

	if (cfg->num_pattern > RTW_PHL_PROXY_PTCL_PATTERN_MAX_NUM)
		return MACCMP;
	for (idx = 0; idx < cfg->num_pattern; idx++) {
		curr_pattern_len = cfg->pattern_len[idx];
		if (curr_pattern_len > RTW_PHL_PROXY_PTCL_PATTERN_MAX_LEN)
			return MACCMP;
		len += (curr_pattern_len + 1);
	}

	if (len <= (H2C_CMD_LEN - FWCMD_HDR_LEN)) {
		h2cb_type = H2CB_CLASS_CMD;
		PLTFM_MSG_TRACE("[PtclPattern] size %d, using CMD Q\n", len);
	} else if (len <= (H2C_DATA_LEN - FWCMD_HDR_LEN)) {
		h2cb_type = H2CB_CLASS_DATA;
		PLTFM_MSG_TRACE("[PtclPattern] size %d, using DATA Q\n", len);
	} else if (len <= (H2C_LONG_DATA_LEN - FWCMD_HDR_LEN)) {
		h2cb_type = H2CB_CLASS_LONG_DATA;
		PLTFM_MSG_TRACE("[PtclPattern] size %d, using LDATA Q\n", len);
	} else {
		PLTFM_MSG_ERR("[PtclPattern] size %d, exceed LDATA Q size, abort\n", len);
		return MACBUFSZ;
	}

	h2cb = h2cb_alloc(adapter, h2cb_type);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, len);
	if (!buf) {
		h2cb_free(adapter, h2cb);
		return MACNOBUF;
	}

	ptcl_pattern_hdr = (struct fwcmd_ptcl_pattern *)buf;
	ptcl_pattern_hdr->dword0 = cpu_to_le32(SET_WORD(cfg->macid, FWCMD_H2C_PTCL_PATTERN_MACID) |
					       SET_WORD(cfg->ptcl, FWCMD_H2C_PTCL_PATTERN_PTCL) |
					       SET_WORD(cfg->num_pattern,
							FWCMD_H2C_PTCL_PATTERN_NUM_PATTERN));
	PLTFM_MSG_TRACE("[PtclPattern] macid (%d), ptcl (%d), n_pattern (%d)\n",
			cfg->macid, cfg->ptcl, cfg->num_pattern);
	patterns_head = buf + sizeof(struct fwcmd_ptcl_pattern);
	for (idx = 0; idx < cfg->num_pattern; idx++) {
		curr_pattern_len = cfg->pattern_len[idx];
		*patterns_head++ = curr_pattern_len;
		PLTFM_MEMCPY(patterns_head, (u8 *)cfg->patterns[idx], curr_pattern_len);
		PLTFM_MSG_TRACE("[PtclPattern] - # %d : len (%d), [%s]",
				idx, curr_pattern_len, (char *)cfg->patterns[idx]);
		patterns_head += curr_pattern_len;
	}

	ret = h2c_pkt_set_hdr(adapter, h2cb, FWCMD_TYPE_H2C, FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_PROXY, FWCMD_H2C_FUNC_PTCL_PATTERN, 1, 1);
	if (ret) {
		h2cb_free(adapter, h2cb);
		return ret;
	}

	ret = h2c_pkt_build_txd(adapter, h2cb);
	if (ret) {
		h2cb_free(adapter, h2cb);
		return ret;
	}

	#if MAC_AX_PHL_H2C
	ret = PLTFM_TX(h2cb);
	#else
	ret = PLTFM_TX(h2cb->data, h2cb->len);
	#endif

	if (ret) {
		h2cb_free(adapter, h2cb);
		return ret;
	}
	adapter->sm.proxy_st = MAC_AX_PROXY_SENDING;
	h2cb_free(adapter, h2cb);

	return ret;
}

u32 mac_check_proxy_done(struct mac_ax_adapter *adapter, u8 *fw_ret)
{
	if (adapter->sm.proxy_st == MAC_AX_PROXY_IDLE) {
		*fw_ret = adapter->sm.proxy_ret;
		return MACSUCCESS;
	}
	return MACPROCBUSY;
}

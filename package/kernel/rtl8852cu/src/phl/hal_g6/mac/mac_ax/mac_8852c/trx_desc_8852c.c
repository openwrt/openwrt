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

#include "trx_desc_8852c.h"
#if MAC_AX_8852C_SUPPORT

#define RXD_RPKT_TYPE_INVALID	0xFF
#define TXD_AC_TYPE_MSK		0x3
#define TXD_TID_IND_SH		2
#define TID_MAX_NUM		8

#define TID_0_QSEL 0
#define TID_1_QSEL 1
#define TID_2_QSEL 1
#define TID_3_QSEL 0
#define TID_4_QSEL 2
#define TID_5_QSEL 2
#define TID_6_QSEL 3
#define TID_7_QSEL 3
#define TID_0_IND 0
#define TID_1_IND 0
#define TID_2_IND 1
#define TID_3_IND 1
#define TID_4_IND 0
#define TID_5_IND 1
#define TID_6_IND 0
#define TID_7_IND 1

#define WP_OFFSET_UNIT_8852C 0x8
#define SEC_HDR_SIZE_4B 0x4
#define SEC_HDR_SIZE_8B 0x8
#define HDR_SIZE_WLAN_MAX 0x24
#define HDR_SIZE_802P3 0xE
#define HDR_SIZE_LLC 0x8
#define HDR_SIZE_AMSDU 0xE

enum wd_info_pkt_type {
	WD_INFO_PKT_NORMAL,

	/* keep last */
	WD_INFO_PKT_LAST,
	WD_INFO_PKT_MAX = WD_INFO_PKT_LAST,
};

static u8 qsel_l[TID_MAX_NUM] = {
	TID_0_QSEL, TID_1_QSEL, TID_2_QSEL, TID_3_QSEL,
	TID_4_QSEL, TID_5_QSEL, TID_6_QSEL, TID_7_QSEL
};

static u8 tid_ind[TID_MAX_NUM] = {
	TID_0_IND, TID_1_IND, TID_2_IND, TID_3_IND,
	TID_4_IND, TID_5_IND, TID_6_IND, TID_7_IND
};

u32 mac_txdesc_len_8852c(struct mac_ax_adapter *adapter,
			 struct rtw_t_meta_data *info)
{
	u32 len;
	enum rtw_packet_type type = info->type;

	switch (type) {
	case RTW_PHL_PKT_TYPE_H2C:
	case RTW_PHL_PKT_TYPE_FWDL:
		len = RXD_SHORT_LEN;
		/* to prevent from USB 512-bytes alignment */
		if (adapter->hw_info->intf == MAC_AX_INTF_USB) {
			if (((info->pktlen + len) &
			     (0x200 - 1)) == 0)
				len = RXD_LONG_LEN;
		}
		break;
	default:
		len = WD_BODY_LEN_V1;
		if (info->wdinfo_en != 0)
			len += WD_INFO_LEN;
		break;
	}

	return len;
}

static u32 txdes_proc_h2c_fwdl_8852c(struct mac_ax_adapter *adapter,
				     struct rtw_t_meta_data *info,
				     u8 *buf, u32 len)
{
	struct rxd_short_t *s_rxd;
	struct rxd_long_t *l_rxd;
	struct mac_ax_ops *ops = adapter_to_mac_ops(adapter);

	if (len != ops->txdesc_len(adapter, info))
		return MACBUFSZ;

	s_rxd = (struct rxd_short_t *)buf;
	s_rxd->dword0 =
		cpu_to_le32(SET_WORD(info->pktlen, AX_RXD_RPKT_LEN) |
			    (info->type == RTW_PHL_PKT_TYPE_FWDL ?
			     SET_WORD(RXD_S_RPKT_TYPE_FWDL, AX_RXD_RPKT_TYPE) :
			     SET_WORD(RXD_S_RPKT_TYPE_H2C, AX_RXD_RPKT_TYPE)));
	s_rxd->dword1 = 0;
	s_rxd->dword2 = 0;
	s_rxd->dword3 = 0;

	if (len == sizeof(struct rxd_long_t)) {
		l_rxd = (struct rxd_long_t *)buf;
		l_rxd->dword0 |= AX_RXD_LONG_RXD;
		l_rxd->dword4 = 0;
		l_rxd->dword5 = 0;
		l_rxd->dword6 = 0;
		l_rxd->dword7 = 0;
	}

	return MACSUCCESS;
}

#if MAC_AX_FEATURE_HV
static u32 txdes_proc_hv_8852c(struct mac_ax_adapter *adapter,
			       struct rtw_t_meta_data *info,
			       struct wd_body_t_v1 *wdb,
			       struct wd_info_t *wdi)
{
	struct hv_txpkt_info *hv_info = (struct hv_txpkt_info *)info->mac_priv;

	wdb->dword0 |= cpu_to_le32((hv_info->chk_en ? AX_TXD_CHK_EN : 0));
	wdi->dword3 |=
		cpu_to_le32((hv_info->null_1 ? AX_TXD_NULL_1 : 0) |
			    (hv_info->null_0 ? AX_TXD_NULL_0 : 0) |
			    (hv_info->tri_frame ? AX_TXD_TRI_FRAME : 0) |
			    (hv_info->ht_data_snd ? AX_TXD_HT_DATA_SND : 0));
	wdi->dword5 |= cpu_to_le32(SET_WORD(hv_info->ndpa_dur, AX_TXD_NDPA_DURATION));

	return MACSUCCESS;
}
#endif

static u32 txdes_proc_data_8852c(struct mac_ax_adapter *adapter,
				 struct rtw_t_meta_data *info, u8 *buf, u32 len)
{
	struct wd_body_t_v1 *wdb;
	struct wd_info_t *wdi;
	u8 wd_info_tmpl[WD_INFO_PKT_MAX][24] = {{0}};
	u32 sec_iv_h;
	u16 sec_iv_l;
	u8 qsel, dbcc_wmm;

	if (len != mac_txdesc_len_8852c(adapter, info))
		return MACBUFSZ;

	if (info->dma_ch > MAC_AX_DATA_CH11) {
		PLTFM_MSG_ERR("[ERR]txd ch: %d\n", info->dma_ch);
		return MACNOITEM;
	}

	if (info->pktlen > AX_TXD_TXPKTSIZE_MSK || !info->pktlen) {
		PLTFM_MSG_ERR("[ERR] illegal txpktsize %d\n", info->pktlen);
		return MACFUNCINPUT;
	}

	wdb = (struct wd_body_t_v1 *)buf;
	wdb->dword0 = 0;
	wdb->dword1 = 0;
	wdb->dword2 = 0;
	wdb->dword3 = 0;
	wdb->dword4 = 0;
	wdb->dword5 = 0;
	wdb->dword6 = 0;
	wdb->dword7 = 0;

	if (adapter->hw_info->intf == MAC_AX_INTF_SDIO)
		wdb->dword0 =
			cpu_to_le32(AX_TXD_STF_MODE);
	else if (adapter->hw_info->intf == MAC_AX_INTF_USB)
		wdb->dword0 =
			cpu_to_le32(AX_TXD_STF_MODE |
				    (info->usb_pkt_ofst ?
				     AX_TXD_PKT_OFFSET : 0));
	else
		wdb->dword0 =
			cpu_to_le32((info->wd_page_size ? AX_TXD_WD_PAGE : 0) |
				    (adapter->dle_info.qta_mode ==
				     MAC_AX_QTA_SCC_STF ||
				     adapter->dle_info.qta_mode ==
				     MAC_AX_QTA_DBCC_STF ?
				     AX_TXD_STF_MODE : 0));

	wdb->dword0 |=
		cpu_to_le32(SET_WORD(info->hw_seq_mode,
				     AX_TXD_EN_HWSEQ_MODE) |
			    SET_WORD(info->hw_ssn_sel,
				     AX_TXD_HW_SSN_SEL) |
			    SET_WORD(info->hdr_len,
				     AX_TXD_HDR_LLC_LEN) |
			    SET_WORD(info->dma_ch, AX_TXD_CH_DMA) |
			    (info->hw_amsdu ? AX_TXD_HWAMSDU : 0) |
			    (info->smh_en ? AX_TXD_SMH_EN : 0) |
			    (info->wdinfo_en ? AX_TXD_WDINFO_EN : 0) |
			    (info->no_ack ? AX_TXD_NO_ACK : 0) |
			    (info->upd_wlan_hdr ? AX_TXD_UPD_WLAN_HDR : 0) |
			    (info->hw_sec_iv ? AX_TXD_HW_SEC_IV : 0) |
			    SET_WORD(info->wp_offset,
				     AX_TXD_WP_OFFSET_V1));

	wdb->dword1 =
		cpu_to_le32(SET_WORD(info->sec_type, AX_TXD_SEC_TYPE) |
			    (info->sw_sec_iv ? AX_TXD_SW_SEC_IV : 0) |
			    SET_WORD(info->reuse_size, AX_TXD_REUSE_SIZE) |
			    SET_WORD(info->reuse_start_num,
				     AX_TXD_REUSE_START_NUM) |
			    SET_WORD(info->addr_info_num,
				     AX_TXD_ADDR_INFO_NUM) |
			    SET_WORD(info->sec_keyid, AX_TXD_SEC_KEYID));

	/* Get bb and qsel from qsel by according MAC ID */
	if (info->macid < DBCC_WMM_LIST_SIZE)
		dbcc_wmm = *(adapter->dbcc_info->dbcc_wmm_list + info->macid);
	else
		dbcc_wmm = MAC_AX_DBCC_WMM_INVALID;

	if (info->dma_ch == MAC_AX_DATA_CH9 || info->dma_ch == MAC_AX_DATA_CH11)
		qsel = info->band ? MAC_AX_HI1_SEL : MAC_AX_HI0_SEL;
	else if (dbcc_wmm != MAC_AX_DBCC_WMM_INVALID)
		qsel = (dbcc_wmm << 2) | qsel_l[info->tid];
	else
		qsel = (info->band << 3) | (info->wmm << 2) | qsel_l[info->tid];
	wdb->dword2 =
		cpu_to_le32(SET_WORD(info->pktlen, AX_TXD_TXPKTSIZE) |
			    SET_WORD(qsel, AX_TXD_QSEL) |
			    (tid_ind[info->tid] ? AX_TXD_TID_IND : 0) |
			    SET_WORD(info->macid, AX_TXD_MACID));

	wdb->dword3 = cpu_to_le32(SET_WORD(info->sw_seq,
					   AX_TXD_WIFI_SEQ) |
				  (info->ampdu_en ? AX_TXD_AGG_EN : 0) |
				  ((info->bk || info->ack_ch_info) ?
				    AX_TXD_BK : 0));

	sec_iv_l = info->iv[0] |
		(info->iv[1] << 8);
	wdb->dword4 = cpu_to_le32(SET_WORD(sec_iv_l, AX_TXD_SEC_IV_L));
	sec_iv_h = info->iv[2] |
		(info->iv[3] << 8) |
		(info->iv[4] << 16) |
		(info->iv[5] << 24);
	wdb->dword5 = cpu_to_le32(SET_WORD(sec_iv_h, AX_TXD_SEC_IV_H));
	wdb->dword7 =
		cpu_to_le32((info->userate_sel ? AX_TXD_USERATE_SEL_V1 : 0) |
			     (info->f_dcm ? AX_TXD_DATA_DCM_V1 : 0) |
			     SET_WORD(info->f_bw, AX_TXD_DATA_BW) |
			     SET_WORD(info->f_gi_ltf, AX_TXD_GI_LTF) |
			     SET_WORD(info->f_rate, AX_TXD_DATARATE));

	wdi = (struct wd_info_t *)wd_info_tmpl[WD_INFO_PKT_NORMAL];
	wdi->dword0 =
		cpu_to_le32((info->data_bw_er ? AX_TXD_DATA_BW_ER : 0) |
			    (info->f_er ? AX_TXD_DATA_ER : 0) |
			    (info->f_stbc ? AX_TXD_DATA_STBC : 0) |
			    (info->f_ldpc ? AX_TXD_DATA_LDPC : 0) |
			    (info->dis_data_rate_fb ? AX_TXD_DISDATAFB : 0) |
			    (info->dis_rts_rate_fb ? AX_TXD_DISRTSFB : 0) |
			    SET_WORD(info->hal_port,
				     AX_TXD_MULTIPORT_ID) |
			    SET_WORD(info->mbssid, AX_TXD_MBSSID) |
			    (info->ack_ch_info ? AX_TXD_ACK_CH_INFO : 0));

	if (info->max_agg_num > 0)
		info->max_agg_num -= 1;

	wdi->dword1 =
		cpu_to_le32(SET_WORD(info->max_agg_num, AX_TXD_MAX_AGG_NUM) |
			    SET_WORD(info->data_tx_cnt_lmt, AX_TXD_DATA_TXCNT_LMT) |
			    (info->data_tx_cnt_lmt_en ?
			     AX_TXD_DATA_TXCNT_LMT_SEL : 0) |
			    (info->bc || info->mc ? AX_TXD_BMC : 0) |
			    (info->nav_use_hdr ? AX_TXD_NAVUSEHDR : 0) |
			    (info->a_ctrl_uph ? AX_TXD_A_CTRL_UPH : 0) |
			    (info->a_ctrl_bsr ? AX_TXD_A_CTRL_BSR : 0) |
			    (info->a_ctrl_cas ? AX_TXD_A_CTRL_CAS : 0) |
			    SET_WORD(info->data_rty_lowest_rate,
				     AX_TXD_DATA_RTY_LOWEST_RATE));
	wdi->dword2 =
		cpu_to_le32(SET_WORD(info->life_time_sel, AX_TXD_LIFETIME_SEL) |
			    (info->sec_hw_enc ? AX_TXD_FORCE_KEY_EN : 0) |
			    SET_WORD(info->sec_cam_idx, AX_TXD_SEC_CAM_IDX) |
			    (info->force_key_en ? AX_TXD_FORCE_KEY_EN : 0) |
			    SET_WORD(info->ampdu_density,
				     AX_TXD_AMPDU_DENSITY));

	wdi->dword3 =
		cpu_to_le32((info->sifs_tx ? AX_TXD_SIFS_TX : 0) |
			    SET_WORD(info->ndpa, AX_TXD_NDPA) |
			    SET_WORD(info->snd_pkt_sel, AX_TXD_SND_PKT_SEL) |
			    (info->rtt_en ? AX_TXD_RTT_EN : 0) |
			    (info->spe_rpt ? AX_TXD_SPE_RPT : 0));

	wdi->dword4 =
		cpu_to_le32((info->rts_en ? AX_TXD_RTS_EN : 0) |
			    (info->cts2self ? AX_TXD_CTS2SELF : 0) |
			    SET_WORD(info->rts_cca_mode, AX_TXD_CCA_RTS) |
			    (info->hw_rts_en ? AX_TXD_HW_RTS_EN : 0) |
			    SET_WORD(info->sw_define, AX_TXD_SW_DEFINE));

	wdi->dword5 = 0;

#if MAC_AX_FEATURE_HV
	txdes_proc_hv_8852c(adapter, info, wdb, wdi);
#endif
	if (info->wdinfo_en != 0)
		PLTFM_MEMCPY(buf + WD_BODY_LEN_V1, (u8 *)wdi, WD_INFO_LEN);

	if (adapter->hw_info->wd_checksum_en)
		mac_wd_checksum_8852c(adapter, info, buf);

	return MACSUCCESS;
}

static u32 txdes_proc_mgnt_8852c(struct mac_ax_adapter *adapter,
				 struct rtw_t_meta_data *info, u8 *buf, u32 len)
{
	struct wd_body_t_v1 *wdb;
	struct wd_info_t *wdi;
	u32 sec_iv_h;
	u16 sec_iv_l;
	u8 wd_info_tmpl[WD_INFO_PKT_MAX][24] = {{0}};

	if (len != mac_txdesc_len_8852c(adapter, info)) {
		PLTFM_MSG_ERR("[ERR] illegal len %d\n", len);
		return MACBUFSZ;
	}

	if (info->pktlen > AX_TXD_TXPKTSIZE_MSK || !info->pktlen) {
		PLTFM_MSG_ERR("[ERR] illegal txpktsize %d\n", info->pktlen);
		return MACFUNCINPUT;
	}

	/* only use ch0 in initial development phase, */
	/* and modify it for normal using later.*/
	/* wd_info is always appended in initial development phase */
	wdb = (struct wd_body_t_v1 *)buf;
	if (adapter->hw_info->intf == MAC_AX_INTF_SDIO)
		wdb->dword0 =
			cpu_to_le32(AX_TXD_STF_MODE);
	else if (adapter->hw_info->intf == MAC_AX_INTF_USB)
		wdb->dword0 =
			cpu_to_le32(AX_TXD_STF_MODE |
				    (info->usb_pkt_ofst ?
				     AX_TXD_PKT_OFFSET : 0));
	else
		wdb->dword0 =
			cpu_to_le32((info->wd_page_size ? AX_TXD_WD_PAGE : 0) |
				    (adapter->dle_info.qta_mode ==
				     MAC_AX_QTA_SCC_STF ||
				     adapter->dle_info.qta_mode ==
				     MAC_AX_QTA_DBCC_STF ?
				     AX_TXD_STF_MODE : 0));

	wdb->dword0 |=
		cpu_to_le32(SET_WORD(info->hw_seq_mode,
				     AX_TXD_EN_HWSEQ_MODE) |
			    SET_WORD(info->hw_ssn_sel,
				     AX_TXD_HW_SSN_SEL) |
			    SET_WORD(info->hdr_len,
				     AX_TXD_HDR_LLC_LEN) |
			    SET_WORD((info->band ?
				       MAC_AX_DMA_B1MG :
				       MAC_AX_DMA_B0MG),
				       AX_TXD_CH_DMA) |
			    (info->hw_amsdu ? AX_TXD_HWAMSDU : 0) |
			    (info->smh_en ? AX_TXD_SMH_EN : 0) |
			    (info->wdinfo_en ? AX_TXD_WDINFO_EN : 0) |
			    (info->no_ack ? AX_TXD_NO_ACK : 0) |
			    (info->upd_wlan_hdr ? AX_TXD_UPD_WLAN_HDR : 0) |
			    (info->hw_sec_iv ? AX_TXD_HW_SEC_IV : 0) |
			    SET_WORD(info->wp_offset, AX_TXD_WP_OFFSET_V1));

	wdb->dword1 =
		cpu_to_le32(SET_WORD(info->sec_type, AX_TXD_SEC_TYPE) |
			    (info->sw_sec_iv ? AX_TXD_SW_SEC_IV : 0) |
			    SET_WORD(info->reuse_size, AX_TXD_REUSE_SIZE) |
			    SET_WORD(info->reuse_start_num,
				     AX_TXD_REUSE_START_NUM) |
			    SET_WORD(info->addr_info_num,
				     AX_TXD_ADDR_INFO_NUM) |
			    SET_WORD(info->sec_keyid, AX_TXD_SEC_KEYID));

	/* Get bb and qsel from qsel by according MAC ID */
	wdb->dword2 =
		cpu_to_le32(SET_WORD(info->pktlen, AX_TXD_TXPKTSIZE) |
				    SET_WORD((info->band ?
				      MAC_AX_MG1_SEL : MAC_AX_MG0_SEL), AX_TXD_QSEL)|
				(tid_ind[info->tid] ? AX_TXD_TID_IND : 0) |
				SET_WORD(info->macid, AX_TXD_MACID));
	wdb->dword3 = cpu_to_le32(SET_WORD(info->sw_seq,
					   AX_TXD_WIFI_SEQ) |
				  ((info->bk || info->ack_ch_info) ?
				    AX_TXD_BK : 0));
	sec_iv_l = info->iv[0] |
		(info->iv[1] << 8);
	wdb->dword4 = cpu_to_le32(SET_WORD(sec_iv_l, AX_TXD_SEC_IV_L));
	sec_iv_h = info->iv[2] |
		(info->iv[3] << 8) |
		(info->iv[4] << 16) |
		(info->iv[5] << 24);
	wdb->dword5 = cpu_to_le32(SET_WORD(sec_iv_h, AX_TXD_SEC_IV_H));
	wdb->dword6 = 0;
	wdb->dword7 =
		cpu_to_le32((info->userate_sel ? AX_TXD_USERATE_SEL_V1 : 0) |
			     (info->f_dcm ? AX_TXD_DATA_DCM_V1 : 0) |
			     SET_WORD(info->f_bw, AX_TXD_DATA_BW) |
			     SET_WORD(info->f_gi_ltf, AX_TXD_GI_LTF) |
			     SET_WORD(info->f_rate, AX_TXD_DATARATE));

	wdi = (struct wd_info_t *)wd_info_tmpl[WD_INFO_PKT_NORMAL];
	wdi->dword0 =
		cpu_to_le32((info->data_bw_er ? AX_TXD_DATA_BW_ER : 0) |
			    (info->f_er ? AX_TXD_DATA_ER : 0) |
			    (info->f_stbc ? AX_TXD_DATA_STBC : 0) |
			    (info->f_ldpc ? AX_TXD_DATA_LDPC : 0) |
			    (info->dis_data_rate_fb ? AX_TXD_DISDATAFB : 0) |
			    (info->dis_rts_rate_fb ? AX_TXD_DISRTSFB : 0) |
			    SET_WORD(info->hal_port,
				     AX_TXD_MULTIPORT_ID) |
			    SET_WORD(info->mbssid, AX_TXD_MBSSID) |
			    /* (datai->rls_to_cpuio ? AX_TXD_RLS_TO_CPUIO : 0) | */
			    (info->ack_ch_info ? AX_TXD_ACK_CH_INFO : 0));

	if (info->max_agg_num > 0)
		info->max_agg_num -= 1;

	wdi->dword1 =
		cpu_to_le32(SET_WORD(info->max_agg_num, AX_TXD_MAX_AGG_NUM) |
			    SET_WORD(info->data_tx_cnt_lmt, AX_TXD_DATA_TXCNT_LMT) |
			    (info->data_tx_cnt_lmt_en ?
			     AX_TXD_DATA_TXCNT_LMT_SEL : 0) |
			    (info->bc || info->mc ? AX_TXD_BMC : 0) |
			    (info->nav_use_hdr ? AX_TXD_NAVUSEHDR : 0));
	wdi->dword2 =
		cpu_to_le32(SET_WORD(info->life_time_sel, AX_TXD_LIFETIME_SEL) |
			    SET_WORD(info->sec_cam_idx, AX_TXD_SEC_CAM_IDX) |
			    (info->force_key_en ? AX_TXD_FORCE_KEY_EN : 0) |
			    SET_WORD(info->ampdu_density,
				     AX_TXD_AMPDU_DENSITY));
	wdi->dword3 =
		cpu_to_le32((info->sifs_tx ? AX_TXD_SIFS_TX : 0) |
			    SET_WORD(info->ndpa, AX_TXD_NDPA) |
			    SET_WORD(info->snd_pkt_sel, AX_TXD_SND_PKT_SEL) |
			    (info->rtt_en ? AX_TXD_RTT_EN : 0) |
			    (info->spe_rpt ? AX_TXD_SPE_RPT : 0));
	wdi->dword4 =
		cpu_to_le32((info->rts_en ? AX_TXD_RTS_EN : 0) |
			    (info->cts2self ? AX_TXD_CTS2SELF : 0) |
			    SET_WORD(info->rts_cca_mode, AX_TXD_CCA_RTS) |
			    (info->hw_rts_en ? AX_TXD_HW_RTS_EN : 0)) |
			    SET_WORD(info->sw_define, AX_TXD_SW_DEFINE);
	wdi->dword5 = 0;

#if MAC_AX_FEATURE_HV
	txdes_proc_hv_8852c(adapter, info, wdb, wdi);
#endif
	if (info->wdinfo_en != 0)
		PLTFM_MEMCPY(buf + WD_BODY_LEN_V1, (u8 *)wdi, WD_INFO_LEN);

	if (adapter->hw_info->wd_checksum_en)
		mac_wd_checksum_8852c(adapter, info, buf);

	return MACSUCCESS;
}

static struct txd_proc_type txdes_proc_mac_8852c[] = {
	{RTW_PHL_PKT_TYPE_H2C, txdes_proc_h2c_fwdl_8852c},
	{RTW_PHL_PKT_TYPE_FWDL, txdes_proc_h2c_fwdl_8852c},
	{RTW_PHL_PKT_TYPE_DATA, txdes_proc_data_8852c},
	{RTW_PHL_PKT_TYPE_MGNT, txdes_proc_mgnt_8852c},
	{RTW_PHL_PKT_TYPE_MAX, NULL},
};

u32 mac_build_txdesc_8852c(struct mac_ax_adapter *adapter,
			   struct rtw_t_meta_data *info, u8 *buf, u32 len)
{
	struct txd_proc_type *proc = txdes_proc_mac_8852c;
	enum rtw_packet_type pkt_type = info->type;
	u32 (*handler)(struct mac_ax_adapter *adapter,
		       struct rtw_t_meta_data *info, u8 *buf, u32 len) = NULL;

	for (; proc->type != RTW_PHL_PKT_TYPE_MAX; proc++) {
		if (pkt_type == proc->type) {
			handler = proc->handler;
			break;
		}
	}

	if (!handler) {
		PLTFM_MSG_ERR("[ERR]null type handler type: %X\n", proc->type);
		return MACNOITEM;
	}

	return handler(adapter, info, buf, len);
}

u32 mac_refill_txdesc_8852c(struct mac_ax_adapter *adapter,
			    struct rtw_t_meta_data *txpkt_info,
			    struct mac_ax_refill_info *mask,
			    struct mac_ax_refill_info *info)
{
	u32 dw0 = ((struct wd_body_t_v1 *)info->pkt)->dword0;
	u32 dw1 = ((struct wd_body_t_v1 *)info->pkt)->dword1;

	if (mask->packet_offset)
		((struct wd_body_t_v1 *)info->pkt)->dword0 =
			dw0 | (info->packet_offset ? AX_TXD_PKT_OFFSET : 0);

	if (mask->agg_num == AX_TXD_DMA_TXAGG_NUM_V1_MSK)
		((struct wd_body_t_v1 *)info->pkt)->dword1 =
			SET_CLR_WORD(dw1, info->agg_num, AX_TXD_DMA_TXAGG_NUM_V1);

	if (adapter->hw_info->wd_checksum_en)
		mac_wd_checksum_8852c(adapter, txpkt_info, info->pkt);

	return MACSUCCESS;
}

static u32 rxdes_parse_comm_8852c(struct mac_ax_adapter *adapter,
				  struct mac_ax_rxpkt_info *info, u8 *buf)
{
	u32 hdr_val = le32_to_cpu(((struct rxd_short_t *)buf)->dword0);

	info->rxdlen = hdr_val & AX_RXD_LONG_RXD ? RXD_LONG_LEN : RXD_SHORT_LEN;
	info->pktsize = GET_FIELD(hdr_val, AX_RXD_RPKT_LEN);
	info->shift = (u8)GET_FIELD(hdr_val, AX_RXD_SHIFT);
	info->drvsize = (u8)GET_FIELD(hdr_val, AX_RXD_DRV_INFO_SIZE);

	return MACSUCCESS;
}

static u32 rxdes_parse_wifi_8852c(struct mac_ax_adapter *adapter,
				  struct mac_ax_rxpkt_info *info, u8 *buf, u32 len)
{
	u32 hdr_val;

	info->type = MAC_AX_PKT_DATA;

	hdr_val = le32_to_cpu(((struct rxd_short_t *)buf)->dword3);
	info->u.data.crc_err = !!(hdr_val & AX_RXD_CRC32_ERR);
	info->u.data.icv_err = !!(hdr_val & AX_RXD_ICV_ERR);

	return MACSUCCESS;
}

static u32 rxdes_parse_c2h_8852c(struct mac_ax_adapter *adapter,
				 struct mac_ax_rxpkt_info *info, u8 *buf, u32 len)
{
	info->type = MAC_AX_PKT_C2H;

	return MACSUCCESS;
}

static u32 rxdes_parse_ch_info_8852c(struct mac_ax_adapter *adapter,
				     struct mac_ax_rxpkt_info *info, u8 *buf, u32 len)
{
	info->type = MAC_AX_PKT_CH_INFO;

	return MACSUCCESS;
}

static u32 rxdes_parse_dfs_8852c(struct mac_ax_adapter *adapter,
				 struct mac_ax_rxpkt_info *info, u8 *buf, u32 len)
{
	info->type = MAC_AX_PKT_DFS;

	return MACSUCCESS;
}

static u32 rxdes_parse_ppdu_8852c(struct mac_ax_adapter *adapter,
				  struct mac_ax_rxpkt_info *info, u8 *buf, u32 len)
{
	u32 hdr_val = le32_to_cpu(((struct rxd_short_t *)buf)->dword0);

	info->type = MAC_AX_PKT_PPDU;
	info->u.ppdu.mac_info = !!(hdr_val & AX_RXD_MAC_INFO_VLD);

	return MACSUCCESS;
}

static struct rxd_parse_type rxdes_parse_mac_8852c[] = {
	{RXD_S_RPKT_TYPE_WIFI, rxdes_parse_wifi_8852c},
	{RXD_S_RPKT_TYPE_C2H, rxdes_parse_c2h_8852c},
	{RXD_S_RPKT_TYPE_PPDU, rxdes_parse_ppdu_8852c},
	{RXD_S_RPKT_TYPE_CH_INFO, rxdes_parse_ch_info_8852c},
	{RXD_S_RPKT_TYPE_DFS_RPT, rxdes_parse_dfs_8852c},
	{RXD_RPKT_TYPE_INVALID, NULL},
};

u32 mac_parse_rxdesc_8852c(struct mac_ax_adapter *adapter,
			   struct mac_ax_rxpkt_info *info, u8 *buf, u32 len)
{
	struct rxd_parse_type *parse = rxdes_parse_mac_8852c;
	u8 rpkt_type;
	u32 hdr_val;
	u32 (*handler)(struct mac_ax_adapter *adapter,
		       struct mac_ax_rxpkt_info *info, u8 *buf, u32 len) = NULL;

	hdr_val = le32_to_cpu(((struct rxd_short_t *)buf)->dword0);
	rpkt_type = (u8)GET_FIELD(hdr_val, AX_RXD_RPKT_TYPE);

	//rxdes_parse_comm_8852c(adapter, info, buf);

	for (; parse->type != RXD_RPKT_TYPE_INVALID; parse++) {
		if (rpkt_type == parse->type) {
			handler = parse->handler;
			break;
		}
	}

	if (!handler) {
		PLTFM_MSG_ERR("[ERR]null type handler type: %X\n", parse->type);
		return MACNOITEM;
	}

	return handler(adapter, info, buf, len);
}

u32 mac_wd_checksum_8852c(struct mac_ax_adapter *adapter,
			  struct rtw_t_meta_data *info, u8 *wddesc)
{
	u16 chksum = 0;
	u32 wddesc_size;
	u16 *data;
	u32 i, dw4;

	if (!wddesc) {
		PLTFM_MSG_ERR("[ERR]null pointer\n");
		return MACNPTR;
	}

	if (adapter->hw_info->wd_checksum_en != 1)
		PLTFM_MSG_TRACE("[TRACE]chksum disable\n");

	dw4 = ((struct wd_body_t_v1 *)wddesc)->dword4;

	((struct wd_body_t_v1 *)wddesc)->dword4 =
		SET_CLR_WORD(dw4, 0x0, AX_TXD_TXDESC_CHECKSUM);

	data = (u16 *)(wddesc);
	/*unit : 4 bytes*/
	wddesc_size = mac_txdesc_len_8852c(adapter, info) >> 2;
	for (i = 0; i < wddesc_size; i++)
		chksum ^= (*(data + 2 * i) ^ *(data + (2 * i + 1)));

	/* *(data + 2 * i) & *(data + (2 * i + 1) have endain issue*/
	/* Process eniadn issue after checksum calculation */
	((struct wd_body_t_v1 *)wddesc)->dword4 =
		SET_CLR_WORD(dw4, (u16)(chksum), AX_TXD_TXDESC_CHECKSUM);
	return MACSUCCESS;
}

u32 mac_patch_rx_rate_8852c(struct mac_ax_adapter *adapter,
			    struct rtw_r_meta_data *info)
{
	return MACSUCCESS;
}

u32 mac_get_wp_offset_8852c(struct mac_ax_adapter *adapter,
			    struct mac_txd_ofld_wp_offset *ofld_conf, u16 *val)
{
	u16 ret_val = 0;

	if (ofld_conf->enc_type == MAC_TXD_OFLD_HW_ENC_CONF_MISSING) {
		PLTFM_MSG_ERR("missing configurations: ENC type\n");
		return MACFUNCINPUT;
	}
	if (ofld_conf->hw_amsdu_type == MAC_TXD_OFLD_HW_AMSDU_CONF_MISSING) {
		PLTFM_MSG_ERR("missing configurations: HW AMSDU type\n");
		return MACFUNCINPUT;
	}
	if (ofld_conf->hw_hdr_conv_type == MAC_TXD_OFLD_HW_HDR_CONV_CONF_MISSING) {
		PLTFM_MSG_ERR("missing configurations: HW HDR CONV type\n");
		return MACFUNCINPUT;
	}

	switch (ofld_conf->enc_type) {
	case MAC_TXD_OFLD_SW_ENC:
	case MAC_TXD_OFLD_HW_ENC_BIP128:
		ret_val = 0;
	break;
	case MAC_TXD_OFLD_HW_ENC_WAPI:
	case MAC_TXD_OFLD_HW_ENC_NONE:
		if (ofld_conf->hw_hdr_conv_type == MAC_TXD_OFLD_HW_HDR_CONV_ETHII_TO_WLAN)
			ret_val += (HDR_SIZE_WLAN_MAX + HDR_SIZE_LLC - HDR_SIZE_802P3);
		else if (ofld_conf->hw_hdr_conv_type == MAC_TXD_OFLD_HW_HDR_CONV_SNAP_TO_WLAN)
			ret_val += (HDR_SIZE_WLAN_MAX - HDR_SIZE_802P3);

		if (ofld_conf->hw_amsdu_type == MAC_TXD_OFLD_HW_AMSDU_ON)
			ret_val += HDR_SIZE_AMSDU;
	break;
	case MAC_TXD_OFLD_HW_ENC_WEP40:
	case MAC_TXD_OFLD_HW_ENC_WEP104:
		ret_val += SEC_HDR_SIZE_4B;
		if (ofld_conf->hw_hdr_conv_type == MAC_TXD_OFLD_HW_HDR_CONV_ETHII_TO_WLAN)
			ret_val += (HDR_SIZE_WLAN_MAX + HDR_SIZE_LLC - HDR_SIZE_802P3);
		else if (ofld_conf->hw_hdr_conv_type == MAC_TXD_OFLD_HW_HDR_CONV_SNAP_TO_WLAN)
			ret_val += (HDR_SIZE_WLAN_MAX - HDR_SIZE_802P3);

		if (ofld_conf->hw_amsdu_type == MAC_TXD_OFLD_HW_AMSDU_ON)
			ret_val += HDR_SIZE_AMSDU;
	break;
	case MAC_TXD_OFLD_HW_ENC_TKIP:
	case MAC_TXD_OFLD_HW_ENC_GCMSMS4:
	case MAC_TXD_OFLD_HW_ENC_CCMP128:
	case MAC_TXD_OFLD_HW_ENC_CCMP256:
	case MAC_TXD_OFLD_HW_ENC_GCMP128:
	case MAC_TXD_OFLD_HW_ENC_GCMP256:
		ret_val += SEC_HDR_SIZE_8B;
		if (ofld_conf->hw_hdr_conv_type == MAC_TXD_OFLD_HW_HDR_CONV_ETHII_TO_WLAN)
			ret_val += (HDR_SIZE_WLAN_MAX + HDR_SIZE_LLC - HDR_SIZE_802P3);
		else if (ofld_conf->hw_hdr_conv_type == MAC_TXD_OFLD_HW_HDR_CONV_SNAP_TO_WLAN)
			ret_val += (HDR_SIZE_WLAN_MAX - HDR_SIZE_802P3);

		if (ofld_conf->hw_amsdu_type == MAC_TXD_OFLD_HW_AMSDU_ON)
			ret_val += HDR_SIZE_AMSDU;
	break;
	default:
		ret_val = 0;
	}

	*val = (ret_val / WP_OFFSET_UNIT_8852C) + (ret_val % WP_OFFSET_UNIT_8852C ? 0x1 : 0x0);

	return MACSUCCESS;
}

#else
u32 mac_build_txdesc_8852c(struct mac_ax_adapter *adapter,
			   struct rtw_t_meta_data *info, u8 *buf, u32 len)
{
	return 0;
}

u32 mac_refill_txdesc_8852c(struct mac_ax_adapter *adapter,
			    struct rtw_t_meta_data *txpkt_info,
			    struct mac_ax_refill_info *mask,
			    struct mac_ax_refill_info *info)
{
	return 0;
}

u32 mac_parse_rxdesc_8852c(struct mac_ax_adapter *adapter,
			   struct mac_ax_rxpkt_info *info, u8 *buf, u32 len)
{
	return 0;
}

u32 mac_txdesc_len_8852c(struct mac_ax_adapter *adapter,
			 struct rtw_t_meta_data *info)
{
	return 0;
}

#endif /* #if MAC_AX_8852C_SUPPORT */

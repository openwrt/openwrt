/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation.
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
 *****************************************************************************/
#define _HAL_STA_C_
#include "hal_headers.h"

void
_hal_sta_rssi_init(struct rtw_phl_stainfo_t *sta)
{
	sta->hal_sta->rssi_stat.assoc_rssi = 0;
	sta->hal_sta->rssi_stat.ma_rssi = 0;
}

static enum rtw_hal_status
_hal_bfee_init(struct hal_info_t *hal_info,
	       struct rtw_phl_stainfo_t *sta)
{
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_FAILURE;
	struct rtw_wifi_role_link_t *rlink = NULL;
	bool enable_bfee = false;

	do {
		if(NULL == sta)
			break;

		rlink = sta->rlink;
		if (NULL == rlink)
			break;

		/*only init BFee when wrole cap's bfee and sta cap 's bfer matched */
		if ((rlink->protocol_cap.he_su_bfme ||
		     rlink->protocol_cap.he_mu_bfme) &&
		    (sta->asoc_cap.he_su_bfmr || sta->asoc_cap.he_mu_bfmr)) {
			enable_bfee = true;
		}

		if ((rlink->protocol_cap.vht_su_bfme ||
		     rlink->protocol_cap.vht_mu_bfme) &&
		    (sta->asoc_cap.vht_su_bfmr || sta->asoc_cap.vht_mu_bfmr)) {
			enable_bfee = true;
		}

		if (rlink->protocol_cap.ht_su_bfme &&
		    sta->asoc_cap.ht_su_bfmr)
			enable_bfee = true;

		if (true == enable_bfee) {
			/* BFee Functions */
			if (RTW_HAL_STATUS_SUCCESS !=
				hal_bf_hw_mac_init_bfee(hal_info, rlink->hw_band))
			{
				PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_,
					  "%s : Init HW MAC BFee Fail\n",
					  __func__);
				break;
			}
			/* BFee CSI parameters*/
			hal_info->hal_com->csi_para_ctrl_sel = false;
			if (RTW_HAL_STATUS_SUCCESS !=
				hal_bf_set_bfee_csi_para(hal_info,
					hal_info->hal_com->csi_para_ctrl_sel,
					sta)) {
				PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_,
					  "%s : Set BFee CSI Para Fail\n",
					  __func__);
				break;
			}
#ifdef RTW_WKARD_DYNAMIC_BFEE_CAP
			/* BB Workaround */
			rtw_hal_bb_dcr_en(hal_info, true);
#endif

			PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_,
				  "%s : Enable HW BFee Function Success\n",
				  __func__);
		}
		hstatus = RTW_HAL_STATUS_SUCCESS;
	} while (0);

	return hstatus;
}

static enum rtw_hal_status
_hal_set_default_cctrl_tbl(struct hal_info_t *hal_info,
				 struct rtw_phl_stainfo_t *sta)
{
	enum rtw_hal_status sts = RTW_HAL_STATUS_FAILURE;
	struct rtw_hal_mac_ax_cctl_info cctrl, cctl_mask;

	_os_mem_set(hal_to_drvpriv(hal_info), &cctrl, 0,
			sizeof(struct rtw_hal_mac_ax_cctl_info));
	_os_mem_set(hal_to_drvpriv(hal_info), &cctl_mask, 0,
			sizeof(struct rtw_hal_mac_ax_cctl_info));
	if (NULL == sta)
		goto out;
	sts = rtw_hal_bb_cfg_config_cmac_tbl(hal_info, sta, &cctrl, &cctl_mask);
	if (sts != RTW_HAL_STATUS_SUCCESS) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_ERR_, "%s: fill cmac table of bb failed)\n",
			__func__);
		goto out;
	}
#ifdef CONFIG_PHL_DEFAULT_MGNT_Q_RPT_EN
	cctrl.mgq_rpt_en = 1;
	cctl_mask.mgq_rpt_en = 1;
#endif
	sts = rtw_hal_cmc_tbl_cfg(hal_info, &cctrl ,&cctl_mask, sta->macid);
out:
	return sts;
}

static enum rtw_hal_status
_hal_update_cctrl_tbl(struct hal_info_t *hal_info,
			    struct rtw_phl_stainfo_t *sta)
{
	struct rtw_wifi_role_t *wrole = sta->wrole;
	struct rtw_wifi_role_link_t *rlink = sta->rlink;
	struct role_link_cap_t *cap = &rlink->cap;
	enum rtw_hal_status sts = RTW_HAL_STATUS_FAILURE;
	struct rtw_hal_mac_ax_cctl_info cctrl, cctl_info_mask;
#ifdef DBG_DBCC_MONITOR_TIME
	u32 start_t = 0;

	phl_fun_monitor_start(&start_t, true, __FUNCTION__);
#endif /* DBG_DBCC_MONITOR_TIME */
	_os_mem_set(hal_to_drvpriv(hal_info), &cctrl, 0, sizeof(struct rtw_hal_mac_ax_cctl_info));
	_os_mem_set(hal_to_drvpriv(hal_info), &cctl_info_mask, 0, sizeof(struct rtw_hal_mac_ax_cctl_info));

	if (NULL == sta)
		goto out;

	sts = rtw_hal_bb_cfg_config_cmac_tbl(hal_info, sta, &cctrl,
						&cctl_info_mask);
	if (sts != RTW_HAL_STATUS_SUCCESS) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_ERR_, "%s: fill cmac table of bb failed)\n",
			__func__);
		goto out;
	}
	/*TODO - update cctrl tab from stainfo*/
	cctrl.disrtsfb = 1;
	cctl_info_mask.disrtsfb = 1;

	cctrl.disdatafb = 1;
	cctl_info_mask.disdatafb = 1;

	/*
	if (!cctrl.disdatafb)
		cctrl.arfr_ctrl = rtw_hal_bb_get_arfr_idx(hal_info, sta);
	*/

	if (cap->rty_lmt_rts == 0xFF) {
		cctrl.rts_txcnt_lmt_sel = 0;
	} else {
		cctrl.rts_txcnt_lmt_sel = 1;
		cctrl.rts_txcnt_lmt = cap->rty_lmt_rts & 0xF;
		cctl_info_mask.rts_txcnt_lmt = 0xF;
	}
	cctl_info_mask.rts_txcnt_lmt_sel = 1;
	cctrl.rts_rty_lowest_rate = (sta->chandef.band == BAND_ON_24G) ? (RTW_DATA_RATE_CCK1) : (RTW_DATA_RATE_OFDM6);
	cctl_info_mask.rts_rty_lowest_rate = 0xF;

	if (cap->rty_lmt == 0xFF) {
		cctrl.data_txcnt_lmt_sel = 0;
	} else {
		cctrl.data_txcnt_lmt_sel = 1;
		cctrl.data_tx_cnt_lmt = cap->rty_lmt & 0x3F;
		cctl_info_mask.data_tx_cnt_lmt = 0x3F;
	}
	cctl_info_mask.data_txcnt_lmt_sel = 1;

	cctrl.max_agg_num_sel = 1;
	cctl_info_mask.max_agg_num_sel = 1;

	cctrl.max_agg_num = sta->asoc_cap.num_ampdu - 1;
	cctl_info_mask.max_agg_num = 0xFF;

	cctrl.ampdu_density = sta->asoc_cap.ampdu_density;
	cctl_info_mask.ampdu_density = 0xF;

	if (cctrl.max_agg_num <= 0x3F)
		cctrl.ba_bmap = 0; /* 64 */
	else if (cctrl.max_agg_num > 0x3F && cctrl.max_agg_num <= 0x7F)
		cctrl.ba_bmap = 1; /* 128*/
	else if ( cctrl.max_agg_num > 0x7F && cctrl.max_agg_num <= 0xFF)
		cctrl.ba_bmap = 2; /* 256 */
	cctl_info_mask.ba_bmap = 0x3;

	if (rtw_phl_role_is_client_category(sta->wrole)) {
		cctrl.uldl = 1;
		cctl_info_mask.uldl = 1;
	} else {
		cctrl.uldl = 0;
		cctl_info_mask.uldl = 1;
	}

	cctrl.multi_port_id = rlink->hw_port;
	cctl_info_mask.multi_port_id = 0x7;

	if (rtw_phl_role_is_ap_category(wrole)) {
		cctrl.data_dcm = 0; /*(sta->asoc_cap.dcm_max_const_rx > 0)*/
		cctl_info_mask.data_dcm = 1;
	}

	cctrl.rts_en = sta->rts_en;
	cctrl.hw_rts_en = sta->hw_rts_en;
	cctrl.cts2self_en = sta->cts2self;
	cctrl.cca_rts = sta->rts_cca_mode;

	cctl_info_mask.hw_rts_en = 0x1;
	cctl_info_mask.rts_en = 0x1;
	cctl_info_mask.cts2self_en = 0x1;
	cctl_info_mask.cca_rts = 0x3;

	if (sta->asoc_cap.pkt_padding == 3) {
		/* follow PPE threshold */
		u8 ppe16 = 0, ppe8 = 0;
		u8 nss = sta->asoc_cap.nss_rx;

		/* bw = 20MHz */
		ppe16 = (sta->asoc_cap.ppe_thr[nss - 1][CHANNEL_WIDTH_20] & 0x7);
		ppe8 = (sta->asoc_cap.ppe_thr[nss - 1][CHANNEL_WIDTH_20]>>3) & 0x7;

		if ((ppe16 != 7) && (ppe8 == 7)) {
			cctrl.nominal_pkt_padding = 2;
			cctl_info_mask.nominal_pkt_padding = 0x3;
		} else if (ppe8 != 7) {
			cctrl.nominal_pkt_padding = 1;
			cctl_info_mask.nominal_pkt_padding = 0x3;
		} else {
			cctrl.nominal_pkt_padding = 0;
			cctl_info_mask.nominal_pkt_padding = 0x3;
		}
		/* bw = 40MHz */
		ppe16 = (sta->asoc_cap.ppe_thr[nss - 1][CHANNEL_WIDTH_40] & 0x7);
		ppe8 = (sta->asoc_cap.ppe_thr[nss - 1][CHANNEL_WIDTH_40]>>3) & 0x7;

		if ((ppe16 != 7) && (ppe8 == 7)) {
			cctrl.nominal_pkt_padding40 = 2;
			cctl_info_mask.nominal_pkt_padding40 = 0x3;
		} else if (ppe8 != 7) {
			cctrl.nominal_pkt_padding40 = 1;
			cctl_info_mask.nominal_pkt_padding40 = 0x3;
		} else {
			cctrl.nominal_pkt_padding40 = 0;
			cctl_info_mask.nominal_pkt_padding40 = 0x3;
		}
		/* bw = 80MHz */
		ppe16 = (sta->asoc_cap.ppe_thr[nss - 1][CHANNEL_WIDTH_80] & 0x7);
		ppe8 = (sta->asoc_cap.ppe_thr[nss - 1][CHANNEL_WIDTH_80]>>3) & 0x7;

		if ((ppe16 != 7) && (ppe8 == 7)) {
			cctrl.nominal_pkt_padding80 = 2;
			cctl_info_mask.nominal_pkt_padding80 = 0x3;
		} else if (ppe8 != 7) {
			cctrl.nominal_pkt_padding80 = 1;
			cctl_info_mask.nominal_pkt_padding80 = 0x3;
		} else {
			cctrl.nominal_pkt_padding80 = 0;
			cctl_info_mask.nominal_pkt_padding80 = 0x3;
		}
		/* bw = 160MHz */
		ppe16 = (sta->asoc_cap.ppe_thr[nss - 1][CHANNEL_WIDTH_160] & 0x7);
		ppe8 = (sta->asoc_cap.ppe_thr[nss - 1][CHANNEL_WIDTH_160]>>3) & 0x7;

		if ((ppe16 != 7) && (ppe8 == 7)) {
			cctrl.nominal_pkt_padding160 = 2;
			cctl_info_mask.nominal_pkt_padding160 = 0x3;
		} else if (ppe8 != 7) {
			cctrl.nominal_pkt_padding160 = 1;
			cctl_info_mask.nominal_pkt_padding160 = 0x3;
		} else {
			cctrl.nominal_pkt_padding160 = 0;
			cctl_info_mask.nominal_pkt_padding160 = 0x3;
		}
	} else {
		cctrl.nominal_pkt_padding = sta->asoc_cap.pkt_padding;
		cctrl.nominal_pkt_padding40 = sta->asoc_cap.pkt_padding;
		cctrl.nominal_pkt_padding80 = sta->asoc_cap.pkt_padding;
		cctrl.nominal_pkt_padding160 = sta->asoc_cap.pkt_padding;
		cctl_info_mask.nominal_pkt_padding = 0x3;
		cctl_info_mask.nominal_pkt_padding40 = 0x3;
		cctl_info_mask.nominal_pkt_padding80 = 0x3;
		cctl_info_mask.nominal_pkt_padding160 = 0x3;
	}

	if (sta->wmode&WLAN_MD_11AX) {
          	/**
                 * bsr_queue_size_format:
                 * 1: buffer status unit is 802.11, HE mode
                 * 0: buffer status unit is 802.11, legacy mode
                 **/
		cctrl.bsr_queue_size_format = 1;
		cctl_info_mask.bsr_queue_size_format = 1;

		if (sta->asoc_cap.he_ldpc)
			cctrl.data_ldpc = 1;
	} else if (sta->wmode & WLAN_MD_11AC) {
		if (sta->asoc_cap.vht_ldpc)
			cctrl.data_ldpc = 1;
	} else if (sta->wmode & WLAN_MD_11N) {
		if (sta->asoc_cap.ht_ldpc)
			cctrl.data_ldpc = 1;
	}
	cctl_info_mask.data_ldpc = 1;

	if (sta->set_rts_init_rate > 0) {
		cctrl.rtsrate = sta->set_rts_init_rate - 1;
		cctl_info_mask.rtsrate = 0x1FF;
	}

	sts = rtw_hal_cmc_tbl_cfg(hal_info, &cctrl, &cctl_info_mask, sta->macid);

out:
#ifdef DBG_DBCC_MONITOR_TIME
	phl_fun_monitor_end(&start_t, __FUNCTION__);
#endif /* DBG_DBCC_MONITOR_TIME */
	return sts;
}

enum rtw_hal_status
rtw_hal_upd_ampdu_cctrl_info(void *hal, struct rtw_phl_stainfo_t *sta)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	enum rtw_hal_status sts = RTW_HAL_STATUS_FAILURE;
	struct rtw_hal_mac_ax_cctl_info cctrl, cctl_info_mask;

	_os_mem_set(hal_to_drvpriv(hal_info), &cctrl, 0, sizeof(struct rtw_hal_mac_ax_cctl_info));
	_os_mem_set(hal_to_drvpriv(hal_info), &cctl_info_mask, 0, sizeof(struct rtw_hal_mac_ax_cctl_info));

	if (NULL == sta)
		goto out;

	cctrl.max_agg_num_sel = 1;
	cctl_info_mask.max_agg_num_sel = 1;

	cctrl.max_agg_num = sta->asoc_cap.num_ampdu - 1;
	cctl_info_mask.max_agg_num = 0xFF;

	if (cctrl.max_agg_num <= 0x3F)
		cctrl.ba_bmap = 0; /* 64 */
	else if (cctrl.max_agg_num > 0x3F && cctrl.max_agg_num <= 0x7F)
		cctrl.ba_bmap = 1; /* 128*/
	else if ( cctrl.max_agg_num > 0x7F && cctrl.max_agg_num <= 0xFF)
		cctrl.ba_bmap = 2; /* 256 */
	cctl_info_mask.ba_bmap = 0x3;

	sts = rtw_hal_cmc_tbl_cfg(hal_info, &cctrl, &cctl_info_mask, sta->macid);

out:
	return sts;
}

enum rtw_hal_status
rtw_hal_cfg_rsc(void *hal, struct rtw_phl_stainfo_t *sta, u8 rsc_cfg)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct rtw_wifi_role_link_t *rlink = sta->rlink;
	enum rtw_hal_status hsts = RTW_HAL_STATUS_FAILURE;

	hsts = rtw_hal_mac_set_rsc_cfg(hal_info->hal_com, rsc_cfg, rlink->hw_band);

	if (RTW_HAL_STATUS_SUCCESS != hsts)
		goto out;
out:
	return hsts;
}



enum rtw_hal_status
rtw_hal_cfg_rrsr_ref_rate_sel(void *hal, struct rtw_phl_stainfo_t *sta, bool ref_rate_sel)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	enum rtw_hal_status hsts = RTW_HAL_STATUS_FAILURE;

	hsts = rtw_hal_mac_set_rrsr_ref_rate_sel(hal_info->hal_com, ref_rate_sel, sta->rlink->hw_band);

	if (RTW_HAL_STATUS_SUCCESS != hsts)
		goto out;
out:
	return hsts;
}


enum rtw_hal_status
rtw_hal_cfg_tx_ampdu(void *hal, struct rtw_phl_stainfo_t *sta)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	enum rtw_hal_status hsts = RTW_HAL_STATUS_FAILURE;

	/* update ampdu configuration */
	if (256 >= sta->asoc_cap.num_ampdu) {
		hsts = rtw_hal_mac_set_hw_ampdu_cfg(hal_info,
		                                    sta->rlink->hw_band,
		                                    sta->asoc_cap.num_ampdu,
		                                    0xA5);
	} else {
		PHL_WARN("%s not consider this case : num_ampdu(%u), please check \n",
			__func__, sta->asoc_cap.num_ampdu);
	}

	if (RTW_HAL_STATUS_SUCCESS != hsts)
		goto out;
out:
	return hsts;
}


static enum rtw_hal_status
_hal_update_dctrl_tbl(struct hal_info_t *hal_info,
		      struct rtw_phl_stainfo_t *sta)
{
	enum rtw_hal_status sts = RTW_HAL_STATUS_FAILURE;
	struct mac_ax_dctl_info dctrl, dctl_info_mask;
#if defined(CONFIG_RTW_TX_HW_AMSDU_HW_MERGE_MODE) || defined(CONFIG_RTW_TX_HW_AMSDU_SW_MERGE_MODE)
	u8 amsdu_max_len = 0;
#endif
#ifdef DBG_DBCC_MONITOR_TIME
	u32 start_t = 0;

	phl_fun_monitor_start(&start_t, true, __FUNCTION__);
#endif /* DBG_DBCC_MONITOR_TIME */
	if (NULL == sta)
		goto out;

	_os_mem_set(hal_to_drvpriv(hal_info), &dctrl, 0, sizeof(struct mac_ax_dctl_info));
	_os_mem_set(hal_to_drvpriv(hal_info), &dctl_info_mask, 0, sizeof(struct mac_ax_dctl_info));

#ifdef CONFIG_RTW_TX_HW_HDR_CONV
	/* mhdr_len: qos = 26, nonqos = 24 */
	sts = rtw_hal_mac_hdr_conv_tx_macid_en(hal_info, sta->macid, 0, 1, 26, 0);
	if (sts != RTW_HAL_STATUS_SUCCESS)
		goto out;

	dctrl.vlan_tag_sel = 0;	/* [TODO] process vlan_tag */
	dctrl.htc_order = 0;	/* [TODO] process htc_order */

	dctl_info_mask.vlan_tag_sel = 0x3;
	dctl_info_mask.htc_order = 0x1;

#if defined(CONFIG_RTW_TX_HW_AMSDU_HW_MERGE_MODE) || defined(CONFIG_RTW_TX_HW_AMSDU_SW_MERGE_MODE)
	amsdu_max_len = (sta->asoc_cap.max_amsdu_len > 0) ? HWAMSDU_MAX_LEN_7920 : HWAMSDU_MAX_LEN_4024;

	sts = rtw_hal_mac_hwamsdu_max_len(hal_info, sta->macid, amsdu_max_len);
	if (sts != RTW_HAL_STATUS_SUCCESS)
		goto out;

	sts = rtw_hal_mac_hwamsdu_macid_en(hal_info, sta->macid, 1);
	if (sts != RTW_HAL_STATUS_SUCCESS)
		goto out;
#endif
#endif

#ifdef CONFIG_PHL_CSUM_OFFLOAD_RX
	dctrl.chksum_offload_en = 1;
	dctl_info_mask.chksum_offload_en = 1;
	dctrl.with_llc = 1;
	dctl_info_mask.with_llc = 1;
#endif /*CONFIG_PHL_CSUM_OFFLOAD_RX*/

	sts = rtw_hal_dmc_tbl_cfg(hal_info, &dctrl, &dctl_info_mask, sta->macid);

out:
#ifdef DBG_DBCC_MONITOR_TIME
	phl_fun_monitor_end(&start_t, __FUNCTION__);
#endif /* DBG_DBCC_MONITOR_TIME */
	return sts;
}

enum rtw_hal_status
_hal_update_ba_cam(struct hal_info_t *hal_info, u8 valid, u16 macid,
                   u8 dialog_token, u16 timeout, u16 start_seq_num, u16 ba_policy,
                   u16 tid, u16 buf_size, u8 camid, u8 band_idx, u8 uid, bool is_std)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;
	void *drv = hal_to_drvpriv(hal_info);
	struct mac_ax_bacam_info ba_cam = {0};
	FUNCIN();
	_os_mem_set(drv, &ba_cam, 0, sizeof(ba_cam));
	ba_cam.valid = valid;
	ba_cam.init_req = 1;
	ba_cam.entry_idx = camid;
	ba_cam.entry_idx_v1 = camid;
	ba_cam.tid = tid;
	ba_cam.macid = (u8)macid;
	ba_cam.std_entry_en = is_std;
	/**
	 * uid_value valid when std_entry_en = 0,
	 * it indicate the mapping of Rx UID in MAC/PHY intf and this temp entry.
	 **/
	ba_cam.uid_value = uid;
	ba_cam.band_sel = band_idx;
	if (buf_size > 64)
		ba_cam.bmap_size = 4;
	else
		ba_cam.bmap_size = 0;
	ba_cam.ssn = start_seq_num;

	PHL_INFO("[BACAM] ba_cam.valid = %d, ba_cam.init_req = %d, ba_cam.entry_idx = %d\n",
			 ba_cam.valid,
			 ba_cam.init_req,
			 ba_cam.entry_idx);
	PHL_INFO("[BACAM] ba_cam.tid = %d, ba_cam.macid = %d, ba_cam.bmap_size = %d\n",
			 ba_cam.tid,
			 ba_cam.macid,
			 ba_cam.bmap_size);
	PHL_INFO("[BACAM] ba_cam.ssn = 0x%X, ba_cam.entry_idx_v1 =%d\n",
			 ba_cam.ssn,
			 ba_cam.entry_idx_v1);
	PHL_INFO("[BACAM] ba_cam.band_sel = %d, ba_cam.uid_value = %d, ba_cam.std_entry_en = %d\n",
			 ba_cam.band_sel,
			 ba_cam.uid_value,
			 ba_cam.std_entry_en);

	hal_status = rtw_hal_bacam_cfg(hal_info, &ba_cam);

	if (RTW_HAL_STATUS_FAILURE == hal_status) {
		PHL_WARN("rtw_hal_bacam_cfg fail 0x%08X\n", hal_status);

	}

	FUNCOUT();
	return hal_status;
}

enum rtw_hal_status
rtw_hal_start_ba_session(void *hal, struct rtw_phl_stainfo_t *sta,
                         u8 dialog_token, u16 timeout,
                         u16 start_seq_num, u16 ba_policy,
                         u16 tid, u16 buf_size)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct rtw_phl_com_t *phl_com = hal_info->phl_com;
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;
	struct hal_spec_t *hal_spec = phl_get_ic_spec(phl_com);
	u8 max_std_entry_num = hal_spec->max_std_entry_num;
	u8 band_idx = sta->rlink->hw_band;
	u8 entry_idx = 0;

	/* Update existing standard entry */
	for (entry_idx = 0; entry_idx < max_std_entry_num; entry_idx++) {
		if (hal_com->ba_ctl.used_map[entry_idx]
		    && sta->macid == hal_com->ba_ctl.mac_id[entry_idx]
		    && (u8)tid == hal_com->ba_ctl.tid[entry_idx]) {
			PHL_INFO("Update existing standard entry(macid=%d, tid=%d)\n",
			         sta->macid, tid);
			hal_status = _hal_update_ba_cam(hal_info, 1, sta->macid, dialog_token,
			                                timeout, start_seq_num, ba_policy, tid,
			                                buf_size, entry_idx, band_idx, 0, true);
			goto out;
		}
	}

	if (hal_com->ba_ctl.count == max_std_entry_num) {
		/*
		 * Transfer existing standard entry to temp entry
		 * when TID is 0(AC_BE) or 5(AC_VI)
		 */
		if (tid == 0 || tid == 5) {
			for (entry_idx = 0; entry_idx < max_std_entry_num; entry_idx++) {
				if (hal_com->ba_ctl.tid[entry_idx] != 0
				    && hal_com->ba_ctl.tid[entry_idx] != 5) {
					PHL_INFO("Transfer existing standard entry to temp entry \
					         (entry_idx=%d, macid=%d, tid=%d).\n",
					         entry_idx, hal_com->ba_ctl.mac_id[entry_idx],
					         hal_com->ba_ctl.tid[entry_idx]);
					hal_status = _hal_update_ba_cam(hal_info, 0, 0, 0, 0,
					                                0, 0, 0, 0, entry_idx, HW_BAND_0, 0, false);
					if (hal_status == RTW_HAL_STATUS_SUCCESS) {
						hal_com->ba_ctl.used_map[entry_idx] = 0;
						hal_com->ba_ctl.tid[entry_idx] = 0xff;
						hal_com->ba_ctl.mac_id[entry_idx] = 0xffff;
						hal_com->ba_ctl.count--;
						break;
					}
				}
			}
		} else {
			PHL_INFO("No available standard entry for tid(%d)\n", tid);
			goto out;
		}
	}

	for (entry_idx = 0; entry_idx < max_std_entry_num; entry_idx++) {
		if (!hal_com->ba_ctl.used_map[entry_idx])
			break;
	}
	if (entry_idx == max_std_entry_num) {
		PHL_WARN("No avail standard entry found but count is(%d)\n",
			hal_com->ba_ctl.count);
		goto out;
	}
	hal_status = _hal_update_ba_cam(hal_info, 1, sta->macid, dialog_token,
	                                timeout, start_seq_num, ba_policy, tid,
	                                buf_size, entry_idx, band_idx, 0, true);
	if (RTW_HAL_STATUS_SUCCESS != hal_status)
		goto out;

	hal_com->ba_ctl.used_map[entry_idx] = 1;
	hal_com->ba_ctl.tid[entry_idx] = (u8)tid;
	hal_com->ba_ctl.mac_id[entry_idx] = sta->macid;
	hal_com->ba_ctl.count++;
out:
	return hal_status;
}

enum rtw_hal_status
rtw_hal_stop_ba_session(void *hal, struct rtw_phl_stainfo_t *sta, u16 tid)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct rtw_phl_com_t *phl_com = hal_info->phl_com;
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;
	struct hal_spec_t *hal_spec = phl_get_ic_spec(phl_com);
	u8 max_std_entry_num = hal_spec->max_std_entry_num;
	u8 entry_idx = 0;

	for (entry_idx = 0; entry_idx < max_std_entry_num; entry_idx++) {
		if (hal_com->ba_ctl.used_map[entry_idx]
		    && sta->macid == hal_com->ba_ctl.mac_id[entry_idx]
		    && (u8)tid == hal_com->ba_ctl.tid[entry_idx]) {
			hal_status = _hal_update_ba_cam(hal_info, 0, 0, 0, 0,
			                                0, 0, 0, 0, entry_idx, HW_BAND_0, 0, false);
			break;
		}
	}
	if (RTW_HAL_STATUS_SUCCESS != hal_status)
		goto out;
	hal_com->ba_ctl.used_map[entry_idx] = 0;
	hal_com->ba_ctl.tid[entry_idx] = 0xff;
	hal_com->ba_ctl.mac_id[entry_idx] = 0xffff;
	hal_com->ba_ctl.count--;
out:
	return hal_status;
}

enum rtw_hal_status
rtw_hal_init_tmp_entry(void *hal, u8 tmp_entry_num, u8 std_entry_num)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;
	u8 entry_idx = std_entry_num;
	u8 uid = 0, cnt = 0;
	u8 b0_tmp_num = tmp_entry_num;

#ifdef CONFIG_DBCC_SUPPORT
	u8 b1_tmp_num = 0;

	if (is_dbcc_sup(hal_info->phl_com)) {
		b1_tmp_num = tmp_entry_num / 2;
		b0_tmp_num = tmp_entry_num - b1_tmp_num;
		if (b1_tmp_num == 0) {
			PHL_ERR("%s: b1_tmp_num = 0, we can't init tmp entry for Band1. It may can't respond to AMPDU!\n",
				__FUNCTION__);
		}
	}
#endif /* CONFIG_DBCC_SUPPORT */

	uid = 0;
	for (cnt = 0; cnt < b0_tmp_num; cnt++) {
		hal_status = _hal_update_ba_cam(hal_info, 1, 0, 0, 0, 0,
		                                0, 0, 0, entry_idx, HW_BAND_0, uid, false);
		if (RTW_HAL_STATUS_SUCCESS != hal_status) {
			PHL_ERR("rtw_hal_init_tmp_entry fail!\n");
			break;
		}
		entry_idx++;
		uid++;
	}

#ifdef CONFIG_DBCC_SUPPORT
	uid = 0;
	for (cnt = 0; cnt < b1_tmp_num; cnt++) {
		hal_status = _hal_update_ba_cam(hal_info, 1, 0, 0, 0, 0,
		                                0, 0, 0, entry_idx, HW_BAND_1, uid, false);
		if (RTW_HAL_STATUS_SUCCESS != hal_status) {
			PHL_ERR("rtw_hal_init_tmp_entry fail!\n");
			break;
		}
		entry_idx++;
		uid++;
	}
#endif /* CONFIG_DBCC_SUPPORT */
	return hal_status;
}

enum rtw_hal_status
rtw_hal_stainfo_init(void *hal, struct rtw_phl_stainfo_t *sta)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;
	void *drv = hal_to_drvpriv(hal_info);

	sta->hal_sta = _os_mem_alloc(drv, sizeof(struct rtw_hal_stainfo_t));
	if (sta->hal_sta == NULL) {
		PHL_ERR("alloc hal_sta failed\n");
		goto error_exit;
	}

#if defined(CONFIG_PHL_RELEASE_RPT_ENABLE) || defined(CONFIG_PCI_HCI)
	/* alloc trx_stats */
	sta->hal_sta->trx_stat.wp_rpt_stats = _os_mem_alloc(drv,
		sizeof(struct rtw_wp_rpt_stats) * RTW_MAX_WP_RPT_AC_NUM(hal_info));

	if (sta->hal_sta->trx_stat.wp_rpt_stats == NULL) {
		PHL_ERR("alloc wp_rpt_stats failed\n");
		goto error_rpt_stats;
	}
#endif
	sta->hal_sta->hw_cfg_tab =
		_os_mem_alloc(drv, sizeof(struct rtw_hw_cfg_tab));
	if (sta->hal_sta->hw_cfg_tab == NULL) {
		PHL_ERR("alloc hw_cfg_tab failed\n");
		goto error_hsta_mem;
	}

	hal_status = rtw_hal_bb_stainfo_init(hal_info, sta);
	if (hal_status != RTW_HAL_STATUS_SUCCESS) {
		PHL_ERR("alloc bb_stainfo failed\n");
		goto error_hw_cfg_tab;
	}
	/* Init lock for tx statistics */
	_os_spinlock_init(drv, &sta->hal_sta->trx_stat.tx_sts_lock);
	/* Init STA RSSI Statistics */
	_hal_sta_rssi_init(sta);

	return hal_status;

error_hw_cfg_tab :
	if (sta->hal_sta->hw_cfg_tab) {
		_os_mem_free(drv, sta->hal_sta->hw_cfg_tab,
				sizeof(struct rtw_hw_cfg_tab));
		sta->hal_sta->hw_cfg_tab = NULL;
	}
error_hsta_mem :
#if defined(CONFIG_PHL_RELEASE_RPT_ENABLE) || defined(CONFIG_PCI_HCI)
	if (sta->hal_sta->trx_stat.wp_rpt_stats) {
		_os_mem_free(drv, sta->hal_sta->trx_stat.wp_rpt_stats,
			     sizeof(struct rtw_wp_rpt_stats) * RTW_MAX_WP_RPT_AC_NUM(hal_info));
		sta->hal_sta->trx_stat.wp_rpt_stats = NULL;
	}

error_rpt_stats :
#endif
	if (sta->hal_sta) {
		_os_mem_free(drv, sta->hal_sta,
				sizeof(struct rtw_hal_stainfo_t));
		sta->hal_sta = NULL;
	}
error_exit :
	return hal_status;
}

enum rtw_hal_status
rtw_hal_stainfo_deinit(void *hal, struct rtw_phl_stainfo_t *sta)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;
	void *drv = hal_to_drvpriv(hal_info);

	if (sta->hal_sta) {
		/* Free lock for tx statistics */
		_os_spinlock_free(drv, &sta->hal_sta->trx_stat.tx_sts_lock);

#if defined(CONFIG_PHL_RELEASE_RPT_ENABLE) || defined(CONFIG_PCI_HCI)
		if (sta->hal_sta->trx_stat.wp_rpt_stats) {
			_os_mem_free(drv, sta->hal_sta->trx_stat.wp_rpt_stats,
				     sizeof(struct rtw_wp_rpt_stats) * RTW_MAX_WP_RPT_AC_NUM(hal_info));
			sta->hal_sta->trx_stat.wp_rpt_stats = NULL;
		}
#endif
		hal_status = rtw_hal_bb_stainfo_deinit(hal_info, sta);
		if (hal_status != RTW_HAL_STATUS_SUCCESS)
			PHL_ERR("bb_stainfo deinit failed\n");

		if (sta->hal_sta->hw_cfg_tab) {
			_os_mem_free(drv, sta->hal_sta->hw_cfg_tab,
					sizeof(struct rtw_hw_cfg_tab));
			sta->hal_sta->hw_cfg_tab = NULL;
		}

		_os_mem_free(drv, sta->hal_sta,
				sizeof(struct rtw_hal_stainfo_t));
		sta->hal_sta = NULL;
	}

	return hal_status;
}
static void _hal_sta_set_default_value(struct hal_info_t *hal_info,
        struct rtw_phl_stainfo_t *sta)
{
	sta->hal_sta->ra_info.ra_registered = false;
}

enum rtw_hal_status
rtw_hal_set_dctrl_tbl_seciv(void *hal,
				struct rtw_phl_stainfo_t *sta, u64 iv)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	enum rtw_hal_status sts = RTW_HAL_STATUS_FAILURE;
	struct mac_ax_dctl_info dctrl = {0}, dctrl_mask = {0};

	if (NULL == sta)
		goto out;

	dctrl.aes_iv_l = (u16)(iv & 0xffffL);
	dctrl.aes_iv_h = (u32)(iv >> 16) & 0xffffffffL;

	dctrl_mask.aes_iv_l = 0xffffL;
	dctrl_mask.aes_iv_h = 0xffffffffL;

	sts = rtw_hal_dmc_tbl_cfg(hal_info, &dctrl ,&dctrl_mask, sta->macid);

out:
	return sts;
}

enum rtw_hal_status
rtw_hal_add_sta_entry(void *hal, struct rtw_phl_stainfo_t *sta)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;

	_hal_sta_set_default_value(hal_info, sta);

	/*add mac address-cam*/
	if (rtw_hal_mac_addr_cam_add_entry(hal_info, sta) !=
					RTW_HAL_STATUS_SUCCESS) {
		PHL_ERR("rtw_hal_mac_addr_cam_add_entry failed\n");
		goto _exit;
	}

	/*update default cmac table*/
	if (_hal_set_default_cctrl_tbl(hal_info, sta) !=
					RTW_HAL_STATUS_SUCCESS) {
		PHL_WARN("_hal_set_default_cctrl_tbl failed\n");
		/* goto _exit; */ /* shall be unmark after header FW is ready */
	}

	if (_hal_update_dctrl_tbl(hal_info, sta) !=
					RTW_HAL_STATUS_SUCCESS) {
		PHL_WARN("_hal_set_default_dctrl_tbl failed\n");
		/* goto _exit; */
	}

	/*add bb stainfo*/
	if (rtw_hal_bb_stainfo_add(hal_info, sta) !=
					RTW_HAL_STATUS_SUCCESS) {
		PHL_ERR("rtw_hal_bb_stainfo_add failed\n");
		goto _err_bbsta_add;
	}
	return RTW_HAL_STATUS_SUCCESS;

_err_bbsta_add:
	rtw_hal_mac_addr_cam_del_entry(hal_info, sta);
_exit:
	return RTW_HAL_STATUS_FAILURE;
}

#define WAIT_DROP_PKT_TIMEOUT 300
void
rtw_hal_disconnect_drop_all_pkt(void *hal, struct rtw_phl_stainfo_t *sta)
{
	struct hal_info_t *hal_i = (struct hal_info_t *)hal;

#ifdef CONFIG_PCI_HCI
	struct rtw_wp_rpt_stats *rpt_stats =
		(struct rtw_wp_rpt_stats *)sta->hal_sta->trx_stat.wp_rpt_stats;
	u32 start_time = _os_get_cur_time_ms();
	u16 cnt = 0;

	while (true) {
		cnt++;
		rtw_hal_mac_data_drop_once(hal_i->hal_com, sta);
		if (0 == rpt_stats->busy_cnt) {
			PHL_INFO("%s: rpt_stats->busy_cnt == 0, loop cnt(%d)\n",
				__func__, cnt);
			break;
		}
		if (phl_get_passing_time_ms(start_time) > WAIT_DROP_PKT_TIMEOUT) {
			PHL_ERR("%s: Fail, wait time > (%d), busy_cnt(%d)\n",
				__func__, WAIT_DROP_PKT_TIMEOUT,
				rpt_stats->busy_cnt);
			break;
		}
		_os_sleep_ms(hal_to_drvpriv(hal_i), 5);
	}
#else
	/* todo: check empty for usb, sdio */
	rtw_hal_mac_data_drop_once(hal_i->hal_com, sta);
#endif
}

enum rtw_hal_status
rtw_hal_restore_sta_entry(struct rtw_phl_com_t* phl_com, void *hal,
                          struct rtw_phl_stainfo_t *sta, bool is_connect)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;
	enum phl_upd_mode mode = PHL_UPD_STA_CON_DISCONN;

	hal_status = rtw_hal_mac_addr_cam_change_entry(hal_info, sta, PHL_UPD_ROLE_FW_RESTORE, is_connect);
	if (hal_status != RTW_HAL_STATUS_SUCCESS) {
		PHL_ERR("rtw_hal_mac_addr_cam_change_entry failed\n");
		return hal_status;
	}

	/* update cmac table */
	if (RTW_HAL_STATUS_SUCCESS != _hal_update_cctrl_tbl(hal_info, sta))
		PHL_WARN("_hal_update_cctrl_tbl failed\n");

	/*update dmac table*/
	if (RTW_HAL_STATUS_SUCCESS != _hal_update_dctrl_tbl(hal_info, sta))
		PHL_WARN("_hal_update_dctrl_tbl failed\n");

	/*change mac address-cam & mac_h2c_join_info*/
	hal_status = rtw_hal_mac_addr_cam_change_entry(hal_info, sta, mode, is_connect);
	if (hal_status != RTW_HAL_STATUS_SUCCESS) {
		PHL_ERR("rtw_hal_mac_addr_cam_change_entry failed\n");
	}

	if (is_connect) {
		rtw_phl_pkt_ofld_null_request(phl_com, sta, NULL);

		hal_status = rtw_hal_cfg_tx_ampdu(hal, sta);
		if (hal_status != RTW_HAL_STATUS_SUCCESS) {
			PHL_ERR("rtw_hal_cfg_tx_ampdu failed\n");
		}

		if (RTW_HAL_STATUS_SUCCESS != _hal_bfee_init(hal_info, sta)) {
			PHL_ERR("_hal_bfee_init Fail!\n");
		}

		if (sta->hal_sta->rssi_stat.assoc_rssi == 0
                #ifdef CONFIG_PHL_TDLS
			/* There is no association frame for TDLS connection */
			&& sta->wrole->type != PHL_RTYPE_TDLS
                #endif
		) {
			PHL_ERR("%s macid:%d assoc_rssi == 0\n", __func__, sta->macid);
			_os_warn_on(1);
		}

		hal_status = rtw_hal_bb_upt_ramask(hal_info, sta);
		if (hal_status != RTW_HAL_STATUS_SUCCESS)
			PHL_ERR("rtw_hal_bb_upt_ramask failed\n");

		hal_status = rtw_hal_bb_ra_register(hal_info, sta);
		if (hal_status != RTW_HAL_STATUS_SUCCESS)
			PHL_ERR("rtw_hal_bb_ra_register failed\n");

		if (sta->wmode & WLAN_MD_11AX) {
			rtw_hal_bb_set_sta_id(hal_info, sta->aid, sta->rlink->hw_band);
			rtw_hal_bb_set_bss_color(hal_info, sta->asoc_cap.bsscolor,
				sta->rlink->hw_band);
			rtw_hal_bb_set_tb_pwr_ofst(hal_info, 0, sta->rlink->hw_band);
		}
		/* reset rssi stat value */
		sta->hal_sta->rssi_stat.ma_rssi_mgnt = 0;
	} else {
		rtw_phl_pkt_ofld_reset_entry(phl_com, sta->macid);

		hal_status = rtw_hal_bb_ra_deregister(hal_info, sta);
		if (hal_status != RTW_HAL_STATUS_SUCCESS)
			PHL_ERR("rtw_hal_bb_ra_deregister failed\n");

		rtw_hal_disconnect_drop_all_pkt(hal_info, sta);
		/* reset drv rssi_stat */
		_hal_sta_rssi_init(sta);
		_hal_sta_set_default_value(hal_info, sta);
	}
	/* reset bb rssi_stat */
	rtw_hal_bb_media_status_update(hal_info, sta, is_connect);

	return hal_status;
}

enum rtw_hal_status
rtw_hal_update_sta_entry(struct rtw_phl_com_t* phl_com, void *hal,
                         struct rtw_phl_stainfo_t *sta, bool is_connect)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;
	enum phl_upd_mode mode = PHL_UPD_STA_CON_DISCONN;

	/* update cmac table */
	if (RTW_HAL_STATUS_SUCCESS != _hal_update_cctrl_tbl(hal_info, sta))
		PHL_WARN("_hal_update_cctrl_tbl failed\n");

	/* update dmac table */
	if (RTW_HAL_STATUS_SUCCESS != _hal_update_dctrl_tbl(hal_info, sta))
		PHL_WARN("_hal_update_dctrl_tbl failed\n");

	/* change mac address-cam & mac_h2c_join_info */
	hal_status = rtw_hal_mac_addr_cam_change_entry(hal_info, sta, mode, is_connect);
	if (hal_status != RTW_HAL_STATUS_SUCCESS) {
		PHL_ERR("rtw_hal_cfg_tx_ampdu failed\n");
	}

	if (is_connect) {
		rtw_phl_pkt_ofld_null_request(phl_com, sta, NULL);

		hal_status = rtw_hal_cfg_tx_ampdu(hal, sta);
		if (hal_status != RTW_HAL_STATUS_SUCCESS) {
			PHL_ERR("rtw_hal_cfg_tx_ampdu failed\n");
		}

		if (RTW_HAL_STATUS_SUCCESS != _hal_bfee_init(hal_info, sta)) {
			PHL_ERR("_hal_bfee_init Fail!\n");
		}

		if (sta->hal_sta->rssi_stat.assoc_rssi == 0
                #ifdef CONFIG_PHL_TDLS
			/* There is no association frame for TDLS connection */
			&& sta->wrole->type != PHL_RTYPE_TDLS
                #endif
		) {
			PHL_ERR("%s macid:%d assoc_rssi == 0\n", __func__, sta->macid);
			_os_warn_on(1);
		}

		hal_status = rtw_hal_bb_upt_ramask(hal_info, sta);
		if (hal_status != RTW_HAL_STATUS_SUCCESS)
			PHL_ERR("rtw_hal_bb_upt_ramask failed\n");

		hal_status = rtw_hal_bb_ra_register(hal_info, sta);
		if (hal_status != RTW_HAL_STATUS_SUCCESS)
			PHL_ERR("rtw_hal_bb_ra_register failed\n");
		hal_info->hal_com->assoc_sta_cnt++;
		hal_info->hal_com->band[sta->rlink->hw_band].assoc_sta_cnt++;

		if (sta->wmode & WLAN_MD_11AX) {
			rtw_hal_bb_set_sta_id(hal_info, sta->aid, sta->rlink->hw_band);
			rtw_hal_bb_set_bss_color(hal_info, sta->asoc_cap.bsscolor,
				sta->rlink->hw_band);
			rtw_hal_bb_set_tb_pwr_ofst(hal_info, 0, sta->rlink->hw_band);
		}
		/* reset rssi stat value */
		sta->hal_sta->rssi_stat.ma_rssi_mgnt = 0;
	} else {
		rtw_phl_pkt_ofld_reset_entry(phl_com, sta->macid);

		hal_status = rtw_hal_bb_ra_deregister(hal_info, sta);
		if (hal_status != RTW_HAL_STATUS_SUCCESS)
			PHL_ERR("rtw_hal_bb_ra_deregister failed\n");

		hal_info->hal_com->assoc_sta_cnt--;
		hal_info->hal_com->band[sta->rlink->hw_band].assoc_sta_cnt--;
		rtw_hal_disconnect_drop_all_pkt(hal_info, sta);
		/* reset drv rssi_stat */
		_hal_sta_rssi_init(sta);
		_hal_sta_set_default_value(hal_info, sta);
	}
	/* reset bb rssi_stat */
	rtw_hal_bb_media_status_update(hal_info, sta, is_connect);
	PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "%s: hal_status(%d), macid(0x%X), is_connect(%d)\n",
		__FUNCTION__, hal_status, sta->macid , is_connect);
	return hal_status;
}

enum rtw_hal_status
rtw_hal_change_sta_entry(void *hal, struct rtw_phl_stainfo_t *sta,
						enum phl_upd_mode mode)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;
	bool is_connect = false;
#ifdef DBG_DBCC_MONITOR_TIME
	u32 start_t = 0;

	phl_fun_monitor_start(&start_t, true, __FUNCTION__);
#endif /* DBG_DBCC_MONITOR_TIME */
	PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "%s: sta->macid(0x%X), mode(%d)\n",
		__FUNCTION__, sta->macid , mode);
	/* update cmac table */
	if (RTW_HAL_STATUS_SUCCESS != _hal_update_cctrl_tbl(hal_info, sta))
		PHL_WARN("_hal_update_cctrl_tbl failed\n");

	/* update dmac table */
	if (RTW_HAL_STATUS_SUCCESS != _hal_update_dctrl_tbl(hal_info, sta))
		PHL_WARN("_hal_update_dctrl_tbl failed\n");

	/* change mac address-cam & mac_h2c_join_info */
	is_connect = (sta->rlink->mstate == MLME_LINKED) ? true : false;
	hal_status = rtw_hal_mac_addr_cam_change_entry(hal_info, sta, mode, is_connect);
	if (hal_status != RTW_HAL_STATUS_SUCCESS)
		PHL_ERR("rtw_hal_mac_addr_cam_change_entry failed\n");

	hal_status = rtw_hal_bb_ra_update(hal_info, sta);
	if (hal_status != RTW_HAL_STATUS_SUCCESS)
		PHL_ERR("rtw_hal_bb_ra_update failed\n");
#ifdef DBG_DBCC_MONITOR_TIME
	phl_fun_monitor_end(&start_t, __FUNCTION__);
#endif /* DBG_DBCC_MONITOR_TIME */
	return hal_status;
}

enum rtw_hal_status
rtw_hal_del_sta_entry(void *hal, struct rtw_phl_stainfo_t *sta)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	_hal_sta_set_default_value(hal_info, sta);

	hal_status = rtw_hal_mac_addr_cam_del_entry(hal_info, sta);
	if (hal_status != RTW_HAL_STATUS_SUCCESS)
		PHL_ERR("mac_addr_cam_del_entry failed\n");

	hal_status = rtw_hal_bb_stainfo_delete(hal_info, sta);
	if (hal_status != RTW_HAL_STATUS_SUCCESS)
		PHL_ERR("bb_stainfo deinit failed\n");

	return hal_status;
}

u8 rtw_hal_get_sta_rssi(struct rtw_phl_stainfo_t *sta)
{
	u8 rssi = (sta->hal_sta->rssi_stat.rssi >> 1);

	return rssi;
}

u8 rtw_hal_get_sta_rssi_bcn(struct rtw_phl_stainfo_t *sta)
{
	u8 rssi = (sta->hal_sta->rssi_stat.rssi_bcn >> 1);

	return rssi;
}

bool rtw_hal_is_sta_linked(void *hal, struct rtw_phl_stainfo_t *sta)
{
	return (sta->hal_sta->ra_info.ra_registered == true) ? true : false;
}

enum rtw_hal_status
rtw_hal_set_edca(void *hal,
                 struct rtw_wifi_role_link_t *rlink,
                 u8 ac,
                 u32 edca)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	enum rtw_hal_status hal_status;

	hal_status = rtw_hal_mac_set_edca(hal_info->hal_com, rlink->hw_band,
					  rlink->hw_wmm, ac, edca);

	return hal_status;
}

enum rtw_hal_status
rtw_hal_cfg_tx_amsdu(void *hal,
		     u8 macid,
		     u8 enable,
		     u8 amsdu_max_len,
		     u8 qos_field_h,
		     u8 qos_field_h_en,
		     u8 mhdr_len,
		     u8 vlan_tag_valid)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	enum rtw_hal_status hsts = RTW_HAL_STATUS_FAILURE;

	/* set mac hdr length / QoS content */
	hsts = rtw_hal_mac_hdr_conv_tx_macid_en(hal_info,
						macid, qos_field_h,
						qos_field_h_en,
						mhdr_len,
						vlan_tag_valid);
	if (RTW_HAL_STATUS_SUCCESS != hsts) {
		PHL_ERR("rtw_hal_mac_hdr_conv_tx_macid_en failed, hsts = %d\n", hsts);
		goto out;
	}

	/* set HWAMSDU max length */
	hsts = rtw_hal_mac_hwamsdu_max_len(hal_info, macid, amsdu_max_len);
	if (RTW_HAL_STATUS_SUCCESS != hsts) {
		PHL_ERR("rtw_hal_mac_hwamsdu_max_len failed, hsts = %d\n", hsts);
		goto out;
	}

	/* enable HWAMSDU by macid */
	hsts = rtw_hal_mac_hwamsdu_macid_en(hal_info, macid, enable);
	if (RTW_HAL_STATUS_SUCCESS != hsts) {
		PHL_ERR("rtw_hal_mac_hwamsdu_macid_en failed, hsts = %d\n", hsts);
		goto out;
	}

	/* Could add more halmac ops and input parameter in this function */

out:
	return hsts;
}


enum rtw_hal_status
rtw_hal_set_sta_rx_sts(struct rtw_phl_stainfo_t *sta, u8 rst,
									struct rtw_r_meta_data *meta)
{
	if (rst) {
		sta->hal_sta->trx_stat.rx_ok_cnt = 0;
		sta->hal_sta->trx_stat.rx_err_cnt = 0;
	} else {
		if (meta->crc32 || meta->icverr)
			sta->hal_sta->trx_stat.rx_err_cnt++;
		else
			sta->hal_sta->trx_stat.rx_ok_cnt++;

		sta->hal_sta->trx_stat.rx_bw = meta->bw;
		sta->hal_sta->trx_stat.rx_rate = meta->rx_rate;
		sta->hal_sta->trx_stat.rx_gi_ltf = meta->rx_gi_ltf;
	}
	/* TODO: rx_rate_plurality */
	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status
rtw_hal_query_rainfo(void *hal, struct rtw_hal_stainfo_t *hal_sta,
		     struct rtw_phl_rainfo *phl_rainfo)
{
	enum rtw_hal_status hal_sts = RTW_HAL_STATUS_FAILURE;

	hal_sts = rtw_hal_bb_query_rainfo(hal, hal_sta, phl_rainfo);


	return hal_sts;
}

/**
 * rtw_hal_query_txsts_rpt() - get txok and tx retry info
 * @hal:		struct hal_info_t *
 * @macid:		indicate the first macid that you want to query.
 * Return rtw_hal_bb_query_txsts_rpt's return value in enum rtw_hal_status type.
 */
enum rtw_hal_status
rtw_hal_query_txsts_rpt(void *hal, u16 macid)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	/*get tx ok and tx retry statistics*/
	if (RTW_HAL_STATUS_SUCCESS != rtw_hal_bb_query_txsts_rpt(hal_info, macid, 0xFFFF))
		return RTW_HAL_STATUS_FAILURE;
	else
		return RTW_HAL_STATUS_SUCCESS;
}

bool
rtw_hal_check_sta_has_busy_wp(struct rtw_phl_stainfo_t *sta)
{
#ifdef CONFIG_PCI_HCI
	struct rtw_wp_rpt_stats *rpt_stats = sta->hal_sta->trx_stat.wp_rpt_stats;
	return (rpt_stats->busy_cnt) ? true : false;
#else
	/*
	* TODO: USB/SDIO not implement busy wp cnt statistic yet,
	* so return false to avoid affect core-layer logic.
	*/
	return false;
#endif
}
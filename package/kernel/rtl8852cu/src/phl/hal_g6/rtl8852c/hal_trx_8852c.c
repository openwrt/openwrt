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
#define _HAL_TRX_8852C_C_
#include "rtl8852c_hal.h"

static void _hal_trx_8852c_dump_rxcnt(struct hal_ppdu_sts *ppdu_sts)
{
	PHL_TRACE(COMP_PHL_PSTS, _PHL_INFO_, "ppdu_sts->rx_cnt_size = 0x%x\n", ppdu_sts->rx_cnt_size);
	PHL_TRACE(COMP_PHL_PSTS, _PHL_INFO_, "HAL_RXCNT_OFDM_OK = 0x%x\n",
			ppdu_sts->rx_cnt.ppdu_cnt[HAL_RXCNT_OFDM_OK]);
	PHL_TRACE(COMP_PHL_PSTS, _PHL_INFO_, "HAL_RXCNT_OFDM_FAIL = 0x%x\n",
			ppdu_sts->rx_cnt.ppdu_cnt[HAL_RXCNT_OFDM_FAIL]);
	PHL_TRACE(COMP_PHL_PSTS, _PHL_INFO_, "HAL_RXCNT_OFDM_FAM = 0x%x\n",
			ppdu_sts->rx_cnt.ppdu_cnt[HAL_RXCNT_OFDM_FAM]);
	PHL_TRACE(COMP_PHL_PSTS, _PHL_INFO_, "HAL_RXCNT_CCK_OK = 0x%x\n",
			ppdu_sts->rx_cnt.ppdu_cnt[HAL_RXCNT_CCK_OK]);
	PHL_TRACE(COMP_PHL_PSTS, _PHL_INFO_, "HAL_RXCNT_CCK_FAIL = 0x%x\n",
			ppdu_sts->rx_cnt.ppdu_cnt[HAL_RXCNT_CCK_FAIL]);
	PHL_TRACE(COMP_PHL_PSTS, _PHL_INFO_, "HAL_RXCNT_CCK_FAM = 0x%x\n",
			ppdu_sts->rx_cnt.ppdu_cnt[HAL_RXCNT_CCK_FAM]);
	PHL_TRACE(COMP_PHL_PSTS, _PHL_INFO_, "HAL_RXCNT_HT_OK = 0x%x\n",
			ppdu_sts->rx_cnt.ppdu_cnt[HAL_RXCNT_HT_OK]);
	PHL_TRACE(COMP_PHL_PSTS, _PHL_INFO_, "HAL_RXCNT_HT_FAIL = 0x%x\n",
			ppdu_sts->rx_cnt.ppdu_cnt[HAL_RXCNT_HT_FAIL]);
	PHL_TRACE(COMP_PHL_PSTS, _PHL_INFO_, "HAL_RXCNT_HT_PPDU = 0x%x\n",
			ppdu_sts->rx_cnt.ppdu_cnt[HAL_RXCNT_HT_PPDU]);
	PHL_TRACE(COMP_PHL_PSTS, _PHL_INFO_, "HAL_RXCNT_HT_FAM = 0x%x\n",
			ppdu_sts->rx_cnt.ppdu_cnt[HAL_RXCNT_HT_FAM]);
	PHL_TRACE(COMP_PHL_PSTS, _PHL_INFO_, "HAL_RXCNT_VHTSU_OK = 0x%x\n",
			ppdu_sts->rx_cnt.ppdu_cnt[HAL_RXCNT_VHTSU_OK]);
	PHL_TRACE(COMP_PHL_PSTS, _PHL_INFO_, "HAL_RXCNT_VHTSU_FAIL = 0x%x\n",
			ppdu_sts->rx_cnt.ppdu_cnt[HAL_RXCNT_VHTSU_FAIL]);
	PHL_TRACE(COMP_PHL_PSTS, _PHL_INFO_, "HAL_RXCNT_VHTSU_PPDU = 0x%x\n",
			ppdu_sts->rx_cnt.ppdu_cnt[HAL_RXCNT_VHTSU_PPDU]);
	PHL_TRACE(COMP_PHL_PSTS, _PHL_INFO_, "HAL_RXCNT_VHTSU_FAM = 0x%x\n",
			ppdu_sts->rx_cnt.ppdu_cnt[HAL_RXCNT_VHTSU_FAM]);
	PHL_TRACE(COMP_PHL_PSTS, _PHL_INFO_, "HAL_RXCNT_VHTMU_OK = 0x%x\n",
			ppdu_sts->rx_cnt.ppdu_cnt[HAL_RXCNT_VHTMU_OK]);
	PHL_TRACE(COMP_PHL_PSTS, _PHL_INFO_, "HAL_RXCNT_VHTMU_FAIL = 0x%x\n",
			ppdu_sts->rx_cnt.ppdu_cnt[HAL_RXCNT_VHTMU_FAIL]);
	PHL_TRACE(COMP_PHL_PSTS, _PHL_INFO_, "HAL_RXCNT_VHTMU_PPDU = 0x%x\n",
			ppdu_sts->rx_cnt.ppdu_cnt[HAL_RXCNT_VHTMU_PPDU]);
	PHL_TRACE(COMP_PHL_PSTS, _PHL_INFO_, "HAL_RXCNT_VHTMU_FAM = 0x%x\n",
			ppdu_sts->rx_cnt.ppdu_cnt[HAL_RXCNT_VHTMU_FAM]);
	PHL_TRACE(COMP_PHL_PSTS, _PHL_INFO_, "HAL_RXCNT_HESU_OK = 0x%x\n",
			ppdu_sts->rx_cnt.ppdu_cnt[HAL_RXCNT_HESU_OK]);
	PHL_TRACE(COMP_PHL_PSTS, _PHL_INFO_, "HAL_RXCNT_HESU_FAIL = 0x%x\n",
			ppdu_sts->rx_cnt.ppdu_cnt[HAL_RXCNT_HESU_FAIL]);
	PHL_TRACE(COMP_PHL_PSTS, _PHL_INFO_, "HAL_RXCNT_HESU_PPDU = 0x%x\n",
			ppdu_sts->rx_cnt.ppdu_cnt[HAL_RXCNT_HESU_PPDU]);
	PHL_TRACE(COMP_PHL_PSTS, _PHL_INFO_, "HAL_RXCNT_HESU_FAM = 0x%x\n",
			ppdu_sts->rx_cnt.ppdu_cnt[HAL_RXCNT_HESU_FAM]);
	PHL_TRACE(COMP_PHL_PSTS, _PHL_INFO_, "HAL_RXCNT_HEMU_OK = 0x%x\n",
			ppdu_sts->rx_cnt.ppdu_cnt[HAL_RXCNT_HEMU_OK]);
	PHL_TRACE(COMP_PHL_PSTS, _PHL_INFO_, "HAL_RXCNT_HEMU_FAIL = 0x%x\n",
			ppdu_sts->rx_cnt.ppdu_cnt[HAL_RXCNT_HEMU_FAIL]);
	PHL_TRACE(COMP_PHL_PSTS, _PHL_INFO_, "HAL_RXCNT_HEMU_PPDU = 0x%x\n",
			ppdu_sts->rx_cnt.ppdu_cnt[HAL_RXCNT_HEMU_PPDU]);
	PHL_TRACE(COMP_PHL_PSTS, _PHL_INFO_, "HAL_RXCNT_HEMU_FAM = 0x%x\n",
			ppdu_sts->rx_cnt.ppdu_cnt[HAL_RXCNT_HEMU_FAM]);
	PHL_TRACE(COMP_PHL_PSTS, _PHL_INFO_, "HAL_RXCNT_HETB_OK = 0x%x\n",
			ppdu_sts->rx_cnt.ppdu_cnt[HAL_RXCNT_HETB_OK]);
	PHL_TRACE(COMP_PHL_PSTS, _PHL_INFO_, "HAL_RXCNT_HETB_FAIL = 0x%x\n",
			ppdu_sts->rx_cnt.ppdu_cnt[HAL_RXCNT_HETB_FAIL]);
	PHL_TRACE(COMP_PHL_PSTS, _PHL_INFO_, "HAL_RXCNT_HETB_PPDU = 0x%x\n",
			ppdu_sts->rx_cnt.ppdu_cnt[HAL_RXCNT_HETB_PPDU]);
	PHL_TRACE(COMP_PHL_PSTS, _PHL_INFO_, "HAL_RXCNT_HETB_FAM = 0x%x\n",
			ppdu_sts->rx_cnt.ppdu_cnt[HAL_RXCNT_HETB_FAM]);

	PHL_TRACE(COMP_PHL_PSTS, _PHL_INFO_, "HAL_RXCNT_FULLDRP = 0x%x\n",
			ppdu_sts->rx_cnt.ppdu_cnt[HAL_RXCNT_FULLDRP]);
	PHL_TRACE(COMP_PHL_PSTS, _PHL_INFO_, "HAL_RXCNT_FULLDRP_PKT = 0x%x\n",
			ppdu_sts->rx_cnt.ppdu_cnt[HAL_RXCNT_FULLDRP_PKT]);
	PHL_TRACE(COMP_PHL_PSTS, _PHL_INFO_, "HAL_RXCNT_PKTFLTR_DRP = 0x%x\n",
			ppdu_sts->rx_cnt.ppdu_cnt[HAL_RXCNT_PKTFLTR_DRP]);
	PHL_TRACE(COMP_PHL_PSTS, _PHL_INFO_, "HAL_RXCNT_RXDMA = 0x%x\n",
			ppdu_sts->rx_cnt.ppdu_cnt[HAL_RXCNT_RXDMA]);
}

static void _hal_dump_rxdesc(u8 *buf, struct rtw_r_meta_data *mdata)
{
	PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "%s ==>\n", __FUNCTION__);

	debug_dump_data(buf, 56, "_hal_dump_rxdesc:: ");

	PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->pktlen = 0x%X\n", mdata->pktlen);
	PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->shift = 0x%X\n", mdata->shift);
	PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->wl_hd_iv_len = 0x%X\n",
					mdata->wl_hd_iv_len);
	PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->bb_sel = 0x%X\n",
					mdata->bb_sel);
	PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->mac_info_vld = 0x%X\n",
					mdata->mac_info_vld);
	PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->rpkt_type = 0x%X\n",
					mdata->rpkt_type);
	PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->drv_info_size = 0x%X\n",
					mdata->drv_info_size);
	PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->long_rxd = 0x%X\n",
					mdata->long_rxd);

	PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->ppdu_type = 0x%X\n",
					mdata->ppdu_type);
	PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->ppdu_cnt = 0x%X\n",
					mdata->ppdu_cnt);
	PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->sr_en = 0x%X\n",
					mdata->sr_en);
	PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->user_id = 0x%X\n",
					mdata->user_id);
	PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->rx_rate = 0x%X\n",
					mdata->rx_rate);
	PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->rx_gi_ltf = 0x%X\n",
					mdata->rx_gi_ltf);
	PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->non_srg_ppdu = 0x%X\n",
					mdata->non_srg_ppdu);
	PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->inter_ppdu = 0x%X\n",
					mdata->inter_ppdu);
	PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->bw = 0x%X\n",
					mdata->bw );

	PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->freerun_cnt = 0x%X\n",
					mdata->freerun_cnt);

	PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->a1_match = 0x%X\n",
					mdata->a1_match);
	PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->sw_dec = 0x%X\n",
					mdata->sw_dec);
	PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->hw_dec = 0x%X\n",
					mdata->hw_dec);
	PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->ampdu = 0x%X\n",
					mdata->ampdu);
	PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->ampdu_end_pkt = 0x%X\n",
					mdata->ampdu_end_pkt);
	PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->amsdu = 0x%X\n",
					mdata->amsdu);
	PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->amsdu_cut = 0x%X\n",
					mdata->amsdu_cut);
	PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->last_msdu = 0x%X\n",
					mdata->last_msdu);
	PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->bypass = 0x%X\n",
					mdata->bypass);
	PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->crc32 = 0x%X\n",
					mdata->crc32);
	PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->icverr = 0x%X\n",
					mdata->icverr);
	PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->magic_wake = 0x%X\n",
					mdata->magic_wake);
	PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->unicast_wake = 0x%X\n",
					mdata->unicast_wake);
	PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->pattern_wake = 0x%X\n",
					mdata->pattern_wake);
	PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->get_ch_info = 0x%X \n",
					mdata->get_ch_info);
	PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->rx_statistics = 0x%X",
					mdata->rx_statistics);

	PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->pattern_idx = 0x%X",
					mdata->pattern_idx);
	PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->target_idc = 0x%X",
					mdata->target_idc);
	PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->chksum_ofld_en = 0x%X",
					mdata->chksum_ofld_en);
	PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->with_llc = 0x%X",
					mdata->with_llc);
	if (mdata->long_rxd == 1) {
		PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->frame_type = 0x%X",
						mdata->frame_type);
		PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->mc = 0x%X",
						mdata->mc);
		PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->bc = 0x%X",
						mdata->bc);
		PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->more_data = 0x%X",
						mdata->more_data);
		PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->more_frag = 0x%X",
						mdata->more_frag);
		PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->pwr_bit = 0x%X",
						mdata->pwr_bit);
		PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->qos = 0x%X",
						mdata->qos);

		PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->tid = 0x%X",
						mdata->tid);
		PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->eosp = 0x%X",
						mdata->eosp);
		PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->htc = 0x%X",
						mdata->htc);
		PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->q_null = 0x%X",
						mdata->q_null);

		PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->seq = 0x%X",
						mdata->seq);
		PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->frag_num = 0x%X",
						mdata->frag_num);

		PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->sec_cam_idx = 0x%X",
						mdata->sec_cam_idx);
		PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->addr_cam = 0x%X",
						mdata->addr_cam);

		PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->macid = 0x%X\n",
						mdata->macid);
		PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->rx_pl_id = 0x%X",
						mdata->rx_pl_id);
		PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->addr_cam_vld = 0x%X",
						mdata->addr_cam_vld);
		PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->addr_fwd_en = 0x%X",
						mdata->addr_fwd_en);
		PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->rx_pl_match = 0x%X",
						mdata->rx_pl_match);

		debug_dump_data(mdata->mac_addr, 6, "mdata->mac_addr = \n");
	}
}

 static enum rtw_hal_status
 hal_handle_chaninfo_8852c(struct hal_info_t *hal)
 {
	return RTW_HAL_STATUS_SUCCESS;
 }

#ifdef PHL_RXSC_AMPDU
static u8 _hal_check_rxsc_rdy_8852c(struct hal_info_t *hal,
	struct rtw_phl_com_t *phl_com, u8 *desc, struct rtw_r_meta_data *mdata)
{
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_FAILURE;
	struct rtw_rxsc_cache_entry *rxsc_entry = &phl_com->rxsc_entry;

	mdata->tid = GET_RX_AX_DESC_TID_8852C(desc);
	mdata->macid = GET_RX_AX_DESC_MACID_8852C(desc);
	mdata->seq = GET_RX_AX_DESC_SEQ_8852C(desc);
	mdata->mc = GET_RX_AX_DESC_MC_8852C(desc);
	mdata->bc = GET_RX_AX_DESC_BC_8852C(desc);
	mdata->htc = GET_RX_AX_DESC_HTC_8852C(desc);

	if (mdata->ampdu &&
		rxsc_entry->cached_rx_macid == mdata->macid &&
		rxsc_entry->cached_rx_ppdu_cnt == mdata->ppdu_cnt &&
		rxsc_entry->cached_rx_tid == mdata->tid &&
		((rxsc_entry->cached_rx_seq + 1) & WL_SEQ_MASK) == mdata->seq
	) {
		mdata->rx_rate = GET_RX_AX_DESC_RX_DATARATE_8852C(desc);
		mdata->amsdu = GET_RX_AX_DESC_AMSDU_8852C(desc);
		mdata->crc32 = GET_RX_AX_DESC_CRC32_8852C(desc);
		mdata->icverr = GET_RX_AX_DESC_ICVERR_8852C(desc);
		mdata->sw_dec = GET_RX_AX_DESC_SW_DEC_8852C(desc);
		mdata->pwr_bit = GET_RX_AX_DESC_PWR_8852C(desc);
		mdata->q_null = GET_RX_AX_DESC_QNULL_8852C(desc);
		mdata->frame_type = GET_RX_AX_DESC_TYPE_8852C(desc);

		rxsc_entry->cached_rx_seq = mdata->seq;
		hstatus = RTW_HAL_STATUS_SUCCESS;
	} else {
		rxsc_entry->cached_rx_macid = PHL_MACID_MAX_NUM;
	}

	return hstatus;
}
#endif

 /**
 * SW Parsing Rx Desc - hal_parsing_rx_wd_8852c
 * description:
 * 	Parsing Rx WiFi Desc by Halmac or SW Manually
 * input:
 * 	hal : hal ic adapter
 * 	desc : pointer of the start of rx desc
 * output:
 * 	pkt : pointer of the start of pkt;
 * 	pkt_len : the pkt length
 * 	rxwd : rx desc
 */
static enum rtw_hal_status
_hal_parsing_rx_wd_8852c(struct hal_info_t *hal,
		struct rtw_phl_com_t *phl_com,
		u8 *desc, struct rtw_r_meta_data *mdata)
{
	/* ToDo: Parse word by word with byte swap once for
	 * each word
	 */
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_FAILURE;
	struct rtw_hal_com_t *hal_com = hal->hal_com;
#ifdef PHL_RXSC_AMPDU
	struct rtw_rxsc_cache_entry *rxsc_entry = &phl_com->rxsc_entry;
#endif

	mdata->pktlen = GET_RX_AX_DESC_PKT_LEN_8852C(desc);
	mdata->shift = GET_RX_AX_DESC_SHIFT_8852C(desc);
	mdata->bb_sel = GET_RX_AX_DESC_BB_SEL_8852C(desc);
	mdata->mac_info_vld = GET_RX_AX_DESC_MAC_INFO_VLD_8852C(desc);
	mdata->rpkt_type = GET_RX_AX_DESC_RPKT_TYPE_8852C(desc);
	mdata->drv_info_size = GET_RX_AX_DESC_DRV_INFO_SIZE_8852C(desc);
	mdata->long_rxd = GET_RX_AX_DESC_LONG_RXD_8852C(desc);

	if ((mdata->rpkt_type == RX_8852C_DESC_PKT_T_WIFI) ||
	    (mdata->rpkt_type == RX_8852C_DESC_PKT_T_PPDU_STATUS)) {
		mdata->wl_hd_iv_len = GET_RX_AX_DESC_HDR_IV_L_8852C(desc);

		mdata->ppdu_type = GET_RX_AX_DESC_PPDU_TYPE_8852C(desc);
		mdata->ppdu_cnt = GET_RX_AX_DESC_PPDU_CNT_8852C(desc);
		mdata->ampdu = GET_RX_AX_DESC_AMPDU_8852C(desc);

#ifdef PHL_RXSC_AMPDU
		if (PHL_MACID_MAX_NUM != rxsc_entry->cached_rx_macid &&
			RX_8852C_DESC_PKT_T_WIFI == mdata->rpkt_type) {

			/* check and get rxsc cache */
			if (_hal_check_rxsc_rdy_8852c(hal, phl_com, desc, mdata) == RTW_HAL_STATUS_SUCCESS)
				return RTW_HAL_STATUS_SUCCESS;
		}
#endif

		mdata->sr_en = GET_RX_AX_DESC_SR_EN_8852C(desc);
		mdata->user_id = GET_RX_AX_DESC_USER_ID_8852C(desc);
		mdata->rx_rate = GET_RX_AX_DESC_RX_DATARATE_8852C(desc);
		mdata->rx_gi_ltf = GET_RX_AX_DESC_RX_GI_LTF_8852C(desc);
		mdata->non_srg_ppdu = GET_RX_AX_DESC_NON_SRG_PPDU_8852C(desc);
		mdata->inter_ppdu = GET_RX_AX_DESC_INTER_PPDU_8852C(desc);
		mdata->phy_rpt_en = GET_RX_AX_DESC_PHY_RPT_EN_8852C(desc);
		mdata->bw = GET_RX_AX_DESC_BW_8852C(desc);

		mdata->freerun_cnt = GET_RX_AX_DESC_FREERUN_CNT_8852C(desc);

		mdata->a1_match = GET_RX_AX_DESC_A1_MATCH_8852C(desc);
		mdata->sw_dec = GET_RX_AX_DESC_SW_DEC_8852C(desc);
		mdata->hw_dec = GET_RX_AX_DESC_HW_DEC_8852C(desc);
		mdata->ampdu_end_pkt = GET_RX_AX_DESC_AMPDU_EDN_PKT_8852C(desc);
		mdata->amsdu = GET_RX_AX_DESC_AMSDU_8852C(desc);
		mdata->amsdu_cut = GET_RX_AX_DESC_AMSDU_CUT_8852C(desc);
		mdata->last_msdu = GET_RX_AX_DESC_LAST_MSDU_8852C(desc);
		mdata->bypass = GET_RX_AX_DESC_BYPASS_8852C(desc);
		mdata->crc32 = GET_RX_AX_DESC_CRC32_8852C(desc);
		mdata->icverr = GET_RX_AX_DESC_ICVERR_8852C(desc);
		mdata->magic_wake = GET_RX_AX_DESC_MAGIC_WAKE_8852C(desc);
		mdata->unicast_wake = GET_RX_AX_DESC_UNICAST_WAKE_8852C(desc);
		mdata->pattern_wake = GET_RX_AX_DESC_PATTERN_WAKE_8852C(desc);
		mdata->get_ch_info = GET_RX_AX_DESC_CH_INFO_8852C(desc);
		mdata->rx_statistics = GET_RX_AX_DESC_STATISTICS_8852C(desc);

		mdata->pattern_idx = GET_RX_AX_DESC_PATTERN_IDX_8852C(desc);
		mdata->target_idc = GET_RX_AX_DESC_TARGET_IDC_8852C(desc);
		mdata->chksum_ofld_en = GET_RX_AX_DESC_CHKSUM_OFFLOAD_8852C(desc);
		mdata->with_llc = GET_RX_AX_DESC_WITH_LLC_8852C(desc);

		if (mdata->long_rxd == 1) {
			mdata->frame_type = GET_RX_AX_DESC_TYPE_8852C(desc);
			mdata->mc = GET_RX_AX_DESC_MC_8852C(desc);
			mdata->bc = GET_RX_AX_DESC_BC_8852C(desc);
			mdata->more_data = GET_RX_AX_DESC_MD_8852C(desc);
			mdata->more_frag = GET_RX_AX_DESC_MF_8852C(desc);
			mdata->pwr_bit = GET_RX_AX_DESC_PWR_8852C(desc);
			mdata->qos = GET_RX_AX_DESC_QOS_8852C(desc);

			mdata->tid = GET_RX_AX_DESC_TID_8852C(desc);
			mdata->eosp = GET_RX_AX_DESC_EOSP_8852C(desc);
			mdata->htc = GET_RX_AX_DESC_HTC_8852C(desc);
			mdata->q_null = GET_RX_AX_DESC_QNULL_8852C(desc);

			mdata->seq = GET_RX_AX_DESC_SEQ_8852C(desc);
			mdata->frag_num = GET_RX_AX_DESC_FRAG_8852C(desc);

			mdata->sec_cam_idx = GET_RX_AX_DESC_CAM_IDX_8852C(desc);
			mdata->addr_cam = GET_RX_AX_DESC_ADDR_CAM_8852C(desc);

			mdata->addr_cam_vld = GET_RX_AX_DESC_CAM_VLD_8852C(desc);
			if (mdata->addr_cam_vld == 0)
				mdata->macid = 0xFF;
			else
				mdata->macid = GET_RX_AX_DESC_MACID_8852C(desc);

			mdata->rx_pl_id = GET_RX_AX_DESC_PL_ID_8852C(desc);

			mdata->addr_fwd_en = GET_RX_AX_DESC_FWD_EN_8852C(desc);
			mdata->rx_pl_match = GET_RX_AX_DESC_PL_MATCH_8852C(desc);

			_os_mem_cpy(hal_com->drv_priv,
				(void*)&mdata->mac_addr, (void*)(desc + 24), MAC_ALEN);
			mdata->sec_type = GET_RX_AX_DESC_SEC_TYPE_8852C(desc);
		}
	}
#ifdef CONFIG_PHL_CHANNEL_INFO
	else if (mdata->rpkt_type == RX_8852C_DESC_PKT_T_CHANNEL_INFO) {
		mdata->freerun_cnt = GET_RX_AX_DESC_FREERUN_CNT_8852C(desc);
		mdata->bw = GET_RX_AX_DESC_BW_8852C(desc);
		mdata->rx_rate = GET_RX_AX_DESC_RX_DATARATE_8852C(desc);
		mdata->a1_match = GET_RX_AX_DESC_A1_MATCH_8852C(desc);
		mdata->get_ch_info = GET_RX_AX_DESC_CH_INFO_8852C(desc);
	}
#endif

	if(mdata->pktlen == 0)
		hstatus = RTW_HAL_STATUS_FAILURE;
	else
		hstatus = RTW_HAL_STATUS_SUCCESS;

	return hstatus;
}

/**
 * hal_parsing_rx_wd_8852c
 * description:
 * 	Parsing Rx WiFi Desc by Halmac or SW Manually
 * input:
 * 	hal : hal ic adapter
 * 	desc : pointer of the start of rx desc
 * output:
 * 	pkt : pointer of the start of pkt;
 * 	pkt_len : the pkt length
 * 	rxwd : rx desc
 */

enum rtw_hal_status
hal_parsing_rx_wd_8852c(struct rtw_phl_com_t *phl_com,
				struct hal_info_t *hal,
				u8 *buf, u8 **pkt, u16 *pkt_len,
				struct rtw_r_meta_data *mdata)
{
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_FAILURE;
	u8 halmac_rx = 0;
	u8 shift = 0;
	u8 *desc = NULL;
	u8 desc_l = 0;

	do {
		if (NULL == buf)
			break;

		if (phl_com->hci_type == RTW_HCI_PCIE)
			desc = buf + RX_BD_INFO_SIZE;
		else
			desc = buf;

		if (!halmac_rx) {
			hstatus = _hal_parsing_rx_wd_8852c(hal, phl_com, desc, mdata);
		} else {
			/* halmac_ax_ops->parse_rxdesc( */
			/* hm_info->halmac_ax_apter, */
			/* rxpkt_info, */
			/* buf, */
			/* len); */
		}


		if (RTW_HAL_STATUS_SUCCESS != hstatus)
			break;
		/* TODO :: Need Double Check*/
		desc_l = mdata->long_rxd ? RX_DESC_L_SIZE_8852C :
						RX_DESC_S_SIZE_8852C;

		shift = (u8)(mdata->shift * 2 +
			     mdata->drv_info_size * RX_DESC_DRV_INFO_UNIT_8852C +
			     desc_l);

		if ((1 == mdata->mac_info_vld) &&
			(RX_8852C_DESC_PKT_T_PPDU_STATUS != mdata->rpkt_type))
			*pkt = desc + shift + RX_PPDU_MAC_INFO_SIZE_8852C;
		else
			*pkt = desc + shift;

		*pkt_len = (u16)mdata->pktlen;

	} while (false);


	//_hal_dump_rxdesc(desc, mdata);

	return hstatus;
}


static void
_hal_rx_wlanhdr_check_8852c(void *drvpriv, void *hdr, struct rtw_r_meta_data *mdata)
{
	/*Check Retry BIT*/
	u8 retry = 0;
	u16 seq_num = 0;
	u8 type = 0;

	type = PHL_GET_80211_HDR_TYPE(hdr);
	retry = PHL_GET_80211_HDR_RETRY(hdr);
	seq_num = PHL_GET_80211_HDR_SEQUENCE(hdr);
	PHL_GET_80211_HDR_ADDRESS2(drvpriv, hdr, mdata->ta);
	if (retry) {
		PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "[TP Debug] RETRY PKT!!!! seq_num = %d \n", seq_num);
	}
	if (type == 9) { /* trigger */
		PHL_TRACE(COMP_PHL_TRIG, _PHL_INFO_, "rx trigger\n");
		PHL_TRACE(COMP_PHL_TRIG, _PHL_INFO_,
			"addr cam hit=%d\n", mdata->addr_cam_vld);
		PHL_TRACE(COMP_PHL_TRIG, _PHL_INFO_,
			"addr cam idx=%d\n", mdata->addr_cam);
		PHL_TRACE(COMP_PHL_TRIG, _PHL_INFO_,
			"macid = %d\n", mdata->macid);
	}
}

static void
_hal_rx_sts_8852c(struct hal_info_t *hal, struct rtw_r_meta_data *meta)
{
	struct rtw_hal_com_t *hal_com = hal->hal_com;
	struct rtw_trx_stat *trx_stat = &hal_com->trx_stat;

	/* todo: shall drop rx in PHL if hw err */
	if (meta->crc32 || meta->icverr)
		trx_stat->rx_err_cnt++;
	else
		trx_stat->rx_ok_cnt++;
}

void
_hal_parsing_rx_drvinfo_8852c(struct rtw_phl_com_t *phl_com,
			      struct hal_info_t *hal,
			      struct rtw_phl_rx_pkt *phl_rx,
			      u8 *buf)
{
	struct rtw_hal_com_t *hal_com = hal->hal_com;
	struct rtw_chan_def *chandef = &(hal_com->band[phl_rx->r.mdata.bb_sel].cur_chandef);
	u8 *desc = NULL;
	u8 *drv_info = NULL;
	struct rtw_recv_pkt *r = &phl_rx->r;

	do {
		if (NULL == buf)
			break;

		if (!r->mdata.ppdu_cnt_chg) {
			/* only parsing drvinfo for first mpdu in AMPDU */
			break;
		}

		if (phl_com->hci_type == RTW_HCI_PCIE)
			desc = buf + RX_BD_INFO_SIZE;
		else
			desc = buf;

		drv_info = desc + (r->mdata.long_rxd ? RX_DESC_L_SIZE_8852C : RX_DESC_S_SIZE_8852C);

		phl_rx->r.phy_info.signal_strength =
				rtw_hal_bb_drv_info_dbm2rssi(
							hal,
							GET_RX_AX_DRV_INFO_RX_PWR_DBM_8852C(drv_info),
							r->mdata.bw);
		phl_rx->r.phy_info.ch_idx =  GET_RX_AX_DRV_INFO_CENTRAL_CH_8852C(drv_info);
#ifdef RTW_WKARD_PHY_RPT_CCK_CH_IDX
		if (phl_rx->r.phy_info.ch_idx == 0) {
			/* CCK rate issue wkard*/
			phl_rx->r.phy_info.ch_idx = chandef->center_ch;
		}
#endif
		phl_rx->r.phy_info.rxsc =  GET_RX_AX_DRV_INFO_RX_SC_8852C(drv_info);
		phl_rx->r.phy_info.is_drvinfo_vld = true;

		PHL_TRACE(COMP_PHL_PSTS, _PHL_DEBUG_,
			  "[8852CE][DrvInfo] DBM %d(signal_strength %d), RxSC %d, Central %d, BW 0x%x\n",
			  GET_RX_AX_DRV_INFO_RX_PWR_DBM_8852C(drv_info),
			  phl_rx->r.phy_info.signal_strength,
			  phl_rx->r.phy_info.rxsc,
			  phl_rx->r.phy_info.ch_idx,
			  r->mdata.bw);

	} while (0);
}

enum rtw_hal_status
hal_handle_rx_buffer_8852c(struct rtw_phl_com_t *phl_com,
				struct hal_info_t *hal,
				u8 *buf, u32 buf_len,
				struct rtw_phl_rx_pkt *phl_rx)
{
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_FAILURE;
	void *drv = hal_to_drvpriv(hal);
	struct rtw_recv_pkt *r = &phl_rx->r;
	struct rtw_pkt_buf_list *pkt = &r->pkt_list[0];
	struct rtw_r_meta_data *mdata = &r->mdata;
#ifdef PHL_RXSC_AMPDU
	struct rtw_rxsc_cache_entry *rxsc_entry = &phl_com->rxsc_entry;
#endif
#ifdef CONFIG_PHL_TEST_SUITE
	struct test_bp_info bp_info;
#endif
	hstatus = hal_parsing_rx_wd_8852c(phl_com, hal, buf,
					&pkt->vir_addr, &pkt->length, mdata);

	if (RTW_HAL_STATUS_SUCCESS != hstatus)
		return hstatus;
	if( (pkt->vir_addr + pkt->length) > (buf + buf_len) )
		return RTW_HAL_STATUS_FAILURE;

	/* hana_todo */
	r->pkt_cnt = 1;

	switch (mdata->rpkt_type) {
	case RX_8852C_DESC_PKT_T_WIFI :
	{
		phl_rx->type = RTW_RX_TYPE_WIFI;
	#ifdef PHL_RXSC_AMPDU /* todo: ppdu status, err ?*/
		if (PHL_MACID_MAX_NUM == rxsc_entry->cached_rx_macid)
	#endif
		{
			_hal_rx_wlanhdr_check_8852c(drv, pkt->vir_addr, mdata);
			hal_rx_ppdu_sts_normal_data(phl_com, pkt->vir_addr, mdata);
		}
		_hal_rx_sts_8852c(hal, mdata);
		if ((1 == mdata->phy_rpt_en) && (0 < mdata->drv_info_size)) {
			/* paring drv info for rssi & central ch & rxsc */
			_hal_parsing_rx_drvinfo_8852c(phl_com, hal, phl_rx, buf);
		}
	}
	break;
	case RX_8852C_DESC_PKT_T_TX_PD_RELEASE_HOST :
	{
		phl_rx->type = RTW_RX_TYPE_TX_WP_RELEASE_HOST;
	}
	break;
	case RX_8852C_DESC_PKT_T_PPDU_STATUS :
	{
		struct hal_ppdu_sts ppdu_sts = {0};
		u8 is_su = 1;

		phl_rx->type = RTW_RX_TYPE_PPDU_STATUS;
		PHL_TRACE(COMP_PHL_PSTS, _PHL_INFO_,
			  "==> RX_8852C_DESC_PKT_T_PPDU_STATUS :band %d ; ppdu_cnt  %d ; freerun_cnt %d \n",
			  mdata->bb_sel, mdata->ppdu_cnt, mdata->freerun_cnt);
		rtw_hal_mac_ax_parse_ppdu_sts(hal, mdata->mac_info_vld,
							pkt->vir_addr, mdata->pktlen,
							(void *)&ppdu_sts,
							(void *)mdata);
		if (ppdu_sts.rx_cnt_size != 0) {
			_hal_trx_8852c_dump_rxcnt(&ppdu_sts);
		}
		if (ppdu_sts.phy_st_size != 0) {
			if((mdata->ppdu_type == RX_8852C_DESC_PPDU_T_VHT_MU)||
				(mdata->ppdu_type == RX_8852C_DESC_PPDU_T_HE_MU)||
				(mdata->ppdu_type == RX_8852C_DESC_PPDU_T_HE_TB)) {
				is_su = 0;
			}
			if(rtw_hal_bb_parse_phy_sts(hal,
						(void *)&ppdu_sts,
						 phl_rx,
						is_su) != RTW_HAL_STATUS_SUCCESS)
				PHL_TRACE(COMP_PHL_PSTS, _PHL_DEBUG_,
					  "rtw_hal_bb_parse_phy_sts fail\n");


			hal_rx_ppdu_sts(phl_com, phl_rx, &ppdu_sts);
#ifdef CONFIG_PHL_TEST_SUITE
			bp_info.type = BP_INFO_TYPE_MP_RX_PHYSTS;
			bp_info.ptr = (void *)(ppdu_sts.phy_st_ptr);
			bp_info.len = ppdu_sts.phy_st_size;
			rtw_phl_test_setup_bp(phl_com, &bp_info, TEST_SUB_MODULE_MP);
#endif
		}
	}
	break;
	case RX_8852C_DESC_PKT_T_DFS_RPT :
	{
		#ifdef CONFIG_PHL_DFS
		struct mac_ax_dfs_rpt dfs_rpt = {0};
		struct hal_dfs_rpt hal_dfs = {0};
		struct phl_msg msg = {0};

		phl_rx->type = RTW_RX_TYPE_DFS_RPT;

		rtw_hal_mac_parse_dfs(hal,pkt->vir_addr, mdata->pktlen, &dfs_rpt);
		#ifdef DBG_PHL_DFS
		PHL_INFO("RX DFS RPT, pkt_len:%d\n", mdata->pktlen);
		PHL_INFO("[DFS] mac-hdr dfs_num:%d\n", dfs_rpt.dfs_num);
		PHL_INFO("[DFS] mac-hdr drop_num:%d\n", dfs_rpt.drop_num);
		PHL_INFO("[DFS] mac-hdr max_cont_drop:%d\n", dfs_rpt.max_cont_drop);
		PHL_INFO("[DFS] mac-hdr total_drop:%d\n", dfs_rpt.total_drop);
		#endif
		hal_dfs.dfs_ptr = dfs_rpt.dfs_ptr;
		hal_dfs.dfs_num = dfs_rpt.dfs_num;
		hal_dfs.phy_idx = 0;

		if (rtw_hal_bb_radar_detect(hal, &hal_dfs)) {
			SET_MSG_MDL_ID_FIELD(msg.msg_id, PHL_MDL_RX);
			SET_MSG_EVT_ID_FIELD(msg.msg_id, MSG_EVT_DFS_RD_IS_DETECTING);
			rtw_phl_msg_hub_hal_send(phl_com, NULL, &msg);

			phl_com->dfs_info.is_radar_detectd = true;
			PHL_INFO("[DFS] radar detected\n");
		}
		#endif
	}
	break;
	case RX_8852C_DESC_PKT_T_CHANNEL_INFO :
	{
		#ifdef  CONFIG_PHL_CHANNEL_INFO
		enum rtw_hal_status status= RTW_HAL_STATUS_SUCCESS;
		u8* buf_addr;
		struct ch_rpt_hdr_info ch_hdr_rpt = {0};
		struct phy_info_rpt phy_rpt = {0};
		struct ch_info_drv_rpt drv_rpt = {0};
		u32 idle_num = CHAN_INFO_PKT_TOTAL;
		struct chan_info_t *chan_info_old = NULL;

		phl_rx->type = RTW_RX_TYPE_CHANNEL_INFO;
		/* Channel Report */
		/* TODO: need to discuss the final csi header format further.*/
		idle_num = rtw_phl_get_chaninfo_idle_number(drv, phl_com);

		if (idle_num == CHAN_INFO_PKT_TOTAL)
			phl_com->chan_info = rtw_phl_query_idle_chaninfo(drv, phl_com);

		if (phl_com->chan_info == NULL) {
			/*hstatus = RTW_HAL_STATUS_SUCCESS is expected*/
			PHL_INFO("channel info packet not avaialbe due to no pakcet handle\n");
			break;
		}
		buf_addr = phl_com->chan_info->chan_info_buffer;
		status = rtw_hal_bb_ch_info_parsing(hal, pkt->vir_addr, mdata,
			buf_addr + phl_com->chan_info->length,
			&ch_hdr_rpt, &phy_rpt, &drv_rpt);

		if (status == RTW_HAL_STATUS_FAILURE)
			phl_com->chan_info->length = 0;
		else
			phl_com->chan_info->length += drv_rpt.raw_data_len;
		/* store phy info if seg#0 is success*/
		if (drv_rpt.seg_idx_curr == 0 && status != RTW_HAL_STATUS_FAILURE)
			_hal_fill_csi_header_phy_info(hal, &(phl_com->chan_info->csi_header),
				&ch_hdr_rpt, &phy_rpt);
		if (status == RTW_HAL_STATUS_BB_CH_INFO_LAST_SEG) {
			/* Fill remain csi header to buffer  */
			_hal_fill_csi_header_remain(hal,
				&(phl_com->chan_info->csi_header), mdata);
			/* push compelete channel info resourecs to busy queue */
			chan_info_old = rtw_phl_recycle_busy_chaninfo(drv, phl_com, phl_com->chan_info);
			if (chan_info_old)
				rtw_phl_enqueue_idle_chaninfo(drv, phl_com, chan_info_old);
			phl_com->chan_info = rtw_phl_query_idle_chaninfo(drv, phl_com);
			if(phl_com->chan_info == NULL)
				PHL_INFO("channel info packet not avaialbe after recycle\n");
		}
		#else
		phl_rx->type = RTW_RX_TYPE_CHANNEL_INFO;
		#endif /* CONFIG_PHL_CHANNEL_INFO */
	}
	break;
	case RX_8852C_DESC_PKT_T_F2P_TX_CMD_RPT :
	{
		/* DL MU Report ; UL OFDMA Trigger Report */
	}
	break;
	case RX_8852C_DESC_PKT_T_C2H :
	{
		struct rtw_c2h_info c = {0};

		phl_rx->type = RTW_RX_TYPE_C2H;
		rtw_hal_mac_parse_c2h(hal, pkt->vir_addr, mdata->pktlen, (void *)&c);

		hal_c2h_post_process(phl_com, hal, (void *)&c);
	}
	break;
	case RX_8852C_DESC_PKT_T_TX_RPT:
	{
		phl_rx->type = RTW_RX_TYPE_TX_RPT;
	}
	break;

	default:
	break;
	}

	return hstatus;
}


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
#define _HAL_TRX_8852CU_C_
#include "../rtl8852c_hal.h"
#include "hal_trx_8852cu.h"


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
	PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->rx_statistics = 0x%X\n",
											mdata->rx_statistics);

	PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->pattern_idx = 0x%X\n",
											mdata->pattern_idx);
	PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->target_idc = 0x%X\n",
											mdata->target_idc);
	PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->chksum_ofld_en = 0x%X\n",
											mdata->chksum_ofld_en);
	PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->with_llc = 0x%X\n",
											mdata->with_llc);


	if (mdata->long_rxd==1)
	{
		PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->frame_type = 0x%X\n",
											mdata->frame_type);
		PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->mc = 0x%X\n",
											mdata->mc);
		PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->bc = 0x%X\n",
											mdata->bc);
		PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->more_data = 0x%X\n",
											mdata->more_data);
		PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->more_frag = 0x%X\n",
											mdata->more_frag);
		PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->pwr_bit = 0x%X\n",
											mdata->pwr_bit);
		PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->qos = 0x%X\n",
											mdata->qos);

		PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->tid = 0x%X\n",
											mdata->tid);
		PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->eosp = 0x%X\n",
											mdata->eosp);
		PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->htc = 0x%X\n",
											mdata->htc);
		PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->q_null = 0x%X\n",
											mdata->q_null);

		PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->seq = 0x%X\n",
											mdata->seq);
		PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->frag_num = 0x%X\n",
											mdata->frag_num);

		PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->sec_cam_idx = 0x%X\n",
											mdata->sec_cam_idx);
		PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->addr_cam = 0x%X\n",
											mdata->addr_cam);

	PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->macid = 0x%X\n\n",
											mdata->macid);
		PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->rx_pl_id = 0x%X\n",
											mdata->rx_pl_id);
		PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->addr_cam_vld = 0x%X\n",
											mdata->addr_cam_vld);
		PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->addr_fwd_en = 0x%X\n",
											mdata->addr_fwd_en);
		PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "mdata->rx_pl_match = 0x%X\n",
											mdata->rx_pl_match);

		debug_dump_data(mdata->mac_addr, 6, "mdata->mac_addr = \n");
	}



}


/**
 * the function will initializing 8852cu specific data and hw configuration
 */
enum rtw_hal_status hal_trx_init_8852cu(struct hal_info_t *hal)
{
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_FAILURE;

	return hstatus;
}
/**
 * the function will deinitializing 8852cu specific data and hw configuration
 */
static void hal_trx_deinit_8852cu(struct hal_info_t *hal)
{
}

static u8 hal_mapping_hw_tx_chnl_8852cu(struct hal_info_t *hal,
			u16 macid, enum rtw_phl_ring_cat cat, u8 band)
{
	u8 dma_ch = 0;

	/* hana_todo, decided by tid only currently,
	   we should consider more situation later */

	if (0 == band) {
		switch (cat) {
		case RTW_PHL_RING_CAT_TID0:/*AC_BE*/
		case RTW_PHL_RING_CAT_TID3:
		case RTW_PHL_RING_CAT_TID6:/*AC_VO*/
		case RTW_PHL_RING_CAT_TID7:
			dma_ch = ACH0_QUEUE_IDX_8852C;
			break;
		case RTW_PHL_RING_CAT_TID1:/*AC_BK*/
		case RTW_PHL_RING_CAT_TID2:
		case RTW_PHL_RING_CAT_TID4:/*AC_VI*/
		case RTW_PHL_RING_CAT_TID5:
			dma_ch = ACH2_QUEUE_IDX_8852C;
			break;
		case RTW_PHL_RING_CAT_MGNT:
		case RTW_PHL_RING_CAT_HIQ:
			dma_ch = MGQ_B0_QUEUE_IDX_8852C;
			break;
		default:
			dma_ch = ACH0_QUEUE_IDX_8852C;
			PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_, "[WARNING]unknown category (%d)\n",
				  cat);
			break;
		}
	} else if (1 == band) {
		switch (cat) {
		case RTW_PHL_RING_CAT_TID0:/*AC_BE*/
		case RTW_PHL_RING_CAT_TID3:
		case RTW_PHL_RING_CAT_TID6:/*AC_VO*/
		case RTW_PHL_RING_CAT_TID7:
			dma_ch = ACH4_QUEUE_IDX_8852C;
			break;
		case RTW_PHL_RING_CAT_TID1:/*AC_BK*/
		case RTW_PHL_RING_CAT_TID2:
		case RTW_PHL_RING_CAT_TID4:/*AC_VI*/
		case RTW_PHL_RING_CAT_TID5:
			dma_ch = ACH6_QUEUE_IDX_8852C;
			break;
		case RTW_PHL_RING_CAT_MGNT:
		case RTW_PHL_RING_CAT_HIQ:
			dma_ch = MGQ_B1_QUEUE_IDX_8852C;
			break;
		default:
			dma_ch = ACH4_QUEUE_IDX_8852C;
			PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_, "[WARNING]unknown category (%d)\n",
				  cat);
			break;
		}
	} else {
		dma_ch = ACH0_QUEUE_IDX_8852C;
		PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_, "[WARNING]unknown band (%d)\n",
			  band);
	}

	return dma_ch;
}

static enum rtw_hal_status hal_query_info_8852cu(struct hal_info_t *hal, u8 info_id, void *value)
{
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_SUCCESS;

	switch (info_id){
		case RTW_HAL_RXDESC_SIZE:
			/* wifi packet(RXD.RPKT_TYPE = 0x0) = 32 bytes, otherwise 16 bytes */
			*((u8 *)value) = RX_DESC_S_SIZE_8852C;
			break;
		default:
			hstatus = RTW_HAL_STATUS_FAILURE;
			break;
	}
	return hstatus;
}

static enum rtw_hal_status hal_pltfm_tx_8852cu(void *hal,
							struct rtw_h2c_pkt *pkt)
{
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_FAILURE;

	return hstatus;
}

u8 hal_get_bulkout_id_8852cu(struct hal_info_t *hal, u8 dma_ch, u8 mode)
{
	return hal_mac_get_bulkout_id(hal, dma_ch, mode);
}

u8 hal_get_max_bulkout_wd_num_8852cu(struct hal_info_t *hal)
{
	return hal_mac_usb_get_max_bulkout_wd_num(hal);
}

u32
hal_get_wd_len_8852cu(struct hal_info_t *hal, struct rtw_xmit_req *tx_req)
{
	return rtw_hal_mac_get_txdesc_len(hal->mac, tx_req);
}

/**
 * the function update wd page, including wd info, wd body, seq info
 * @hal: see struct hal_info_t
 * @phl_pkt_req: see struct rtw_phl_pkt_req
 */
enum rtw_hal_status
hal_fill_wd_8852cu(struct hal_info_t *hal,
                   struct rtw_xmit_req *tx_req,
                   u8 *wd_buf,
                   u32 *wd_len)
{
	return rtw_hal_mac_fill_txdesc(hal->mac, tx_req, wd_buf, wd_len);
}

enum rtw_hal_status
hal_usb_tx_agg_cfg_8852cu(struct hal_info_t *hal, u8* wd_buf, u8 agg_num)
{
	return hal_mac_usb_tx_agg_cfg(hal, wd_buf, agg_num);
}

u8 hal_get_fwcmd_queue_idx_8852cu(void)
{
	return FWCMD_QUEUE_IDX_8852C;
}

static void _hal_show_tx_failure_rsn_8852cu(u8 txsts)
{

	switch (txsts) {

	case TX_STATUS_TX_FAIL_REACH_RTY_LMT:
		PHL_TRACE(COMP_PHL_DBG, _PHL_DEBUG_, "this wp is tx fail (REACH_RTY_LMT)\n");
		break;
	case TX_STATUS_TX_FAIL_LIFETIME_DROP:
		PHL_TRACE(COMP_PHL_DBG, _PHL_DEBUG_, "this wp is tx fail (LIFETIME_DROP)\n");
		break;
	case TX_STATUS_TX_FAIL_MACID_DROP:
		PHL_TRACE(COMP_PHL_DBG, _PHL_DEBUG_, "this wp is tx fail (MACID_DROP)\n");
		break;
	case TX_STATUS_TX_FAIL_SW_DROP:
		PHL_TRACE(COMP_PHL_DBG, _PHL_DEBUG_, "this wp is tx fail (SW_DROP)\n");
		break;
	default:
		PHL_TRACE(COMP_PHL_DBG, _PHL_DEBUG_, "this wp is tx fail (UNKNOWN)\n");
		break;
	}

}

u16 hal_handle_rx_report_8852cu(struct hal_info_t *hal, u8 *rp,
				u16 len, u8 *mac_id, u8 *ac_queue, u8 *txsts)
{
	u8 polluted = false;
	u16 rsize = 0;
	u8 qsel_value = 0;

	do {
		if (len < RX_RP_PACKET_SIZE)
			break;

		*mac_id = (u8)GET_RX_RP_PKT_MAC_ID(rp);
		qsel_value = (u8)GET_RX_RP_PKT_QSEL(rp);
		*txsts = (u8)GET_RX_RP_PKT_TX_STS(rp);
		/* wp_seq useless for usb case, then skip parsing*/
		polluted = (u8)GET_RX_RP_PKT_POLLUTED(rp);

		*ac_queue = qsel_value % RTW_MAX_AC_QUEUE_NUM;

		PHL_TRACE(COMP_PHL_DBG, _PHL_DEBUG_, "Get recycle report: qsel = %d, macid = %d, ac_queue = %d\n",
			qsel_value, *mac_id, *ac_queue);

		if (TX_STATUS_TX_DONE != *txsts) {

			_hal_show_tx_failure_rsn_8852cu(*txsts);

		} else if (true == polluted) {
			PHL_TRACE(COMP_PHL_DBG, _PHL_DEBUG_, "this wp is polluted\n");
			/*todo handle sw retry */
		} else {
			PHL_TRACE(COMP_PHL_DBG, _PHL_DEBUG_, "this wp tx done\n");
		}

		rsize = RX_RP_PACKET_SIZE;
	} while (false);

	return rsize;
}

static struct hal_trx_ops ops= {
	.init = hal_trx_init_8852cu,
	.deinit = hal_trx_deinit_8852cu,
	.map_hw_tx_chnl = hal_mapping_hw_tx_chnl_8852cu,
	.get_bulkout_id = hal_get_bulkout_id_8852cu,
	.hal_get_wd_len = hal_get_wd_len_8852cu,
	.hal_fill_wd = hal_fill_wd_8852cu,
	.handle_rx_buffer = hal_handle_rx_buffer_8852c,
	.query_hal_info = hal_query_info_8852cu,
	.usb_tx_agg_cfg = hal_usb_tx_agg_cfg_8852cu,
	.usb_rx_agg_cfg = hal_usb_rx_agg_cfg,
	.get_fwcmd_queue_idx = hal_get_fwcmd_queue_idx_8852cu,
	.get_max_bulkout_wd_num = hal_get_max_bulkout_wd_num_8852cu,
	.handle_wp_rpt = hal_handle_rx_report_8852cu,
};

u32 hal_hook_trx_ops_8852cu(struct hal_info_t *hal_info)
{
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_FAILURE;

	if (NULL != hal_info) {
		hal_info->trx_ops = &ops;
		hstatus = RTW_HAL_STATUS_SUCCESS;
	}

	return hstatus;
}

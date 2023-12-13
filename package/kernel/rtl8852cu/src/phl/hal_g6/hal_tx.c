/******************************************************************************
 *
 * Copyright(c) 2019 - 2021 Realtek Corporation.
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
#define _HAL_TX_C_
#include "hal_headers.h"
/**
 * this function will be used in read / write pointer mechanism and
 * return the number of available read pointer
 * @rptr: input, the read pointer
 * @wptr: input, the write pointer
 * @bndy: input, the boundary of read / write pointer mechanism
 */
u16 hal_calc_avail_rptr(u16 rptr, u16 wptr, u16 bndy)
{
	u16 avail_rptr = 0;

	if (wptr >= rptr)
		avail_rptr = wptr - rptr;
	else if (rptr > wptr)
		avail_rptr = wptr + (bndy - rptr);

	return avail_rptr;
}


/**
 * this function will be used in read / write pointer mechanism and
 * return the number of available write pointer
 * @rptr: input, the read pointer
 * @wptr: input, the write pointer
 * @bndy: input, the boundary of read / write pointer mechanism
 */
u16 hal_calc_avail_wptr(u16 rptr, u16 wptr, u16 bndy)
{
	u16 avail_wptr = 0;

	if (rptr > wptr)
		avail_wptr = rptr - wptr - 1;
	else if (wptr >= rptr)
		avail_wptr = rptr + (bndy - wptr) - 1;

	return avail_wptr;
}


enum rtw_hal_status
rtw_hal_enable_hwamsdu(void *hal,
			   u8 enable,
			   u8 max_num,
			   u8 en_single_amsdu,
			   u8 en_last_amsdu_padding)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;

	return rtw_hal_mac_enable_hwamsdu(hal_info, enable, max_num, en_single_amsdu, en_last_amsdu_padding);
}

/**
 * rtw_hal_tx_chnl_mapping - query hw tx dma channel mapping to the sw xmit ring
 * identified by macid, tid and band
 * @hal: see struct hal_info_t
 * @macid: input target macid is 0 ~ 127
 * @cat: input target packet category, see enum rtw_phl_ring_cat
 * @band: input target band, 0 for band 0 / 1 for band 1
 *
 * returns the mapping hw tx dma channel
 */
u8 rtw_hal_tx_chnl_mapping(void *hal, u16 macid,
			   enum rtw_phl_ring_cat cat, u8 band)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct hal_trx_ops *trx_ops = hal_info->trx_ops;
	u8 tx_chnl = 0;

	tx_chnl = trx_ops->map_hw_tx_chnl(hal_info, macid, cat, band);

	return tx_chnl;
}

/**
 * rtw_hal_get_fwcmd_queue_idx - get idx of fwcmd queue
 * @hal: see struct hal_info_t
 *
 * returns u8 idx of fwcmd queue
 */
u8 rtw_hal_get_fwcmd_queue_idx(void *hal)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct hal_trx_ops *trx_ops = hal_info->trx_ops;

	return trx_ops->get_fwcmd_queue_idx();
}
void rtw_hal_cfg_txhci(void *hal, u8 en)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;

	PHL_TRACE(COMP_PHL_DBG, _PHL_DEBUG_, "%s : enable %d.\n", __func__, en);

	if (RTW_HAL_STATUS_SUCCESS != rtw_hal_mac_cfg_txhci(hal_info, en))
		PHL_ERR("%s failure \n", __func__);
}



enum rtw_hal_status rtw_hal_chk_allq_empty(void *hal, u8 *empty)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;

	FUNCIN();

	return rtw_hal_mac_chk_allq_empty(hal_info, empty);
}

enum rtw_hal_status
rtw_hal_fill_txdesc(void *hal,
                    struct rtw_xmit_req *treq,
                    u8 *wd_buf,
                    u32 *wd_len)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;

	return rtw_hal_mac_fill_txdesc(hal_info->mac, treq, wd_buf, wd_len);
}


enum rtw_hal_status
rtw_hal_get_hwseq(void *hal, u16 macid, u8 ref_sel, u8 ssn_sel, u16 *hw_seq)
{
	enum rtw_hal_status sts = RTW_HAL_STATUS_SUCCESS;

	sts = rtw_hal_mac_get_hwseq(hal, macid, ref_sel, ssn_sel, hw_seq);

	return sts;
}


enum rtw_hal_status
rtw_hal_poll_hw_tx_done(void *hal)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	enum rtw_hal_status sts = RTW_HAL_STATUS_SUCCESS;

	sts = rtw_hal_mac_poll_hw_tx_done(hal_info);

	return sts;
}

enum rtw_hal_status
rtw_hal_hw_tx_resume(void *hal)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	enum rtw_hal_status sts = RTW_HAL_STATUS_SUCCESS;

	sts = rtw_hal_mac_hw_tx_resume(hal_info);

	return sts;
}

#ifdef CONFIG_PCI_HCI
/**
 * rtw_hal_convert_qsel_to_tid - convert qsel to tid value
 * @hal: see struct hal_info_t
 * @qsel: HW queue selection
 *
 * returns enum RTW_HAL_STATUS
 */
u8 rtw_hal_convert_qsel_to_tid(void *hal, u8 qsel_id, u8 tid_indic)
{
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_SUCCESS;
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct hal_trx_ops *trx_ops = hal_info->trx_ops;

	hstatus = trx_ops->qsel_to_tid(hal_info, qsel_id, tid_indic);

	return hstatus;
}
/**
 * rtw_hal_tx_res_query - query current HW tx resource with specifc dma channel
 * @hal: see struct hal_info_t
 * @dma_ch: the target dma channel
 * @host_idx: current host index of this channel
 * @hw_idx: current hw index of this channel
 *
 * this function returns the number of available tx resource
 * NOTE, input host_idx and hw_idx ptr shall NOT be NULL
 */
u16 rtw_hal_tx_res_query(void *hal, u8 dma_ch, u16 *host_idx, u16 *hw_idx)
{
	enum rtw_hal_status sts = RTW_HAL_STATUS_FAILURE;
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	u16 res_num = 0;

	sts = rtw_hal_mac_tx_res_query(hal_info, dma_ch, host_idx, hw_idx,
	                               &res_num);

	return res_num;
}

enum phl_band_idx rtw_hal_query_txch_hwband(void *hal, u8 dma_ch)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct hal_trx_ops *trx_ops = hal_info->trx_ops;
	enum phl_band_idx band_idx = HW_BAND_0;

	band_idx = trx_ops->query_txch_hwband(dma_ch);

	return band_idx;
}

/**
 * rtw_hal_query_txch_num - query total hw tx dma channels number
 *
 * returns the number of  hw tx dma channel
 */
u8 rtw_hal_query_txch_num(void *hal)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct hal_trx_ops *trx_ops = hal_info->trx_ops;
	u8 ch_num = 0;

	ch_num = trx_ops->query_txch_num();

	return ch_num;
}

enum rtw_hal_status rtw_hal_trx_init(void *hal, u8 *txbd_buf, u8 *rxbd_buf)
{
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_SUCCESS;
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct hal_trx_ops *trx_ops = hal_info->trx_ops;

	hstatus = trx_ops->init(hal_info, txbd_buf, rxbd_buf);

	return hstatus;
}

void rtw_hal_trx_deinit(void *hal)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct hal_trx_ops *trx_ops = hal_info->trx_ops;

	trx_ops->deinit(hal_info);
}

/**
 * rtw_hal_update_wd_page - update wd page for xmit packet
 * @hal: see struct hal_info_t
 * @phl_pkt_req: packet xmit request from phl, see struct rtw_phl_pkt_req
 *
 * returns enum RTW_HAL_STATUS
 */
enum rtw_hal_status rtw_hal_update_wd_page(void *hal, void *phl_pkt_req)
{
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_SUCCESS;
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct hal_trx_ops *trx_ops = hal_info->trx_ops;

	hstatus = trx_ops->update_wd(hal_info, phl_pkt_req);

	return hstatus;
}

/**
 * rtw_hal_update_txbd - update tx bd for xmit packet
 * @hal: see struct hal_info_t
 * @wd: buffer pointer of wd page to fill in txbd
 *
 * returns enum RTW_HAL_STATUS
 * NOTE, this function is PCIe specific function
 */
enum rtw_hal_status rtw_hal_update_txbd(void *hal, void *txbd, void *wd, u8 dma_ch, u16 wd_num)
{
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_SUCCESS;
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct hal_trx_ops *trx_ops = hal_info->trx_ops;

	hstatus = trx_ops->update_txbd(hal, txbd, wd, dma_ch, wd_num);

	return hstatus;
}


/**
 * rtw_hal_update_trigger_txstart - trigger hw to start tx
 * @hal: see struct hal_info_t
 * @txbd: the target txbd to update
 * @dma_ch: the dma channel index of this txbd_ring
 *
 * returns enum RTW_HAL_STATUS
 */
enum rtw_hal_status
rtw_hal_trigger_txstart(void *hal, struct tx_base_desc *txbd, u8 dma_ch)
{
	enum rtw_hal_status sts = RTW_HAL_STATUS_FAILURE;
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;

	sts = rtw_hal_mac_trigger_txstart(hal_info, txbd, dma_ch);

	if (sts != RTW_HAL_STATUS_SUCCESS) {
		PHL_TRACE(COMP_PHL_XMIT, _PHL_INFO_, "%s : trigger tx start fail!\n",
		          __func__);
	}

	return sts;
}

u8 rtw_hal_poll_txdma_idle(void *hal)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct hal_trx_ops *trx_ops = hal_info->trx_ops;

	FUNCIN();

	return trx_ops->poll_txdma_idle(hal_info);
}

#endif /*CONFIG_PCI_HCI*/

#ifdef CONFIG_USB_HCI
enum rtw_hal_status rtw_hal_trx_init(void *hal)
{
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_SUCCESS;
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct hal_trx_ops *trx_ops = hal_info->trx_ops;

	hstatus = trx_ops->init(hal_info);

	return hstatus;
}

void rtw_hal_trx_deinit(void *hal)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct hal_trx_ops *trx_ops = hal_info->trx_ops;

	trx_ops->deinit(hal_info);
}

u8 rtw_hal_get_bulkout_id(void *hal, u8 dma_ch, u8 mode)
{
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_SUCCESS;
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct hal_trx_ops *trx_ops = hal_info->trx_ops;

	hstatus = trx_ops->get_bulkout_id(hal, dma_ch, mode);

	return hstatus;
}

enum rtw_hal_status
	rtw_hal_usb_tx_agg_cfg(void *hal, u8* wd_buf, u8 agg_num)
{
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_SUCCESS;
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct hal_trx_ops *trx_ops = hal_info->trx_ops;

	hstatus = trx_ops->usb_tx_agg_cfg(hal, wd_buf, agg_num);

	return hstatus;
}


u8 rtw_hal_get_max_bulkout_wd_num(void *hal)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct hal_trx_ops *trx_ops = hal_info->trx_ops;

	return trx_ops->get_max_bulkout_wd_num(hal);
}

u16 rtw_hal_get_max_dma_txagg_msk(void *hal)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;

	return hal_mac_usb_get_max_dma_txagg_msk(hal_info);
}

u32 rtw_hal_get_wd_len(void *hal,
				struct rtw_xmit_req *tx_req)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct hal_trx_ops *trx_ops = hal_info->trx_ops;
	u32 wd_len = 0;

	wd_len = trx_ops->hal_get_wd_len(hal_info, tx_req);

	return wd_len;
}

/**
 * rtw_hal_fill_wd - fill wd-info and wd-boddy for xmit packet
 * @hal: see struct hal_info_t
 * @phl_pkt_req: packet xmit request from phl, see struct rtw_phl_pkt_req
 *
 * returns enum RTW_HAL_STATUS
 */
enum rtw_hal_status rtw_hal_fill_wd(void *hal,
				struct rtw_xmit_req *tx_req,
				u8 *wd_buf, u32 *wd_len)
{
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_SUCCESS;
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct hal_trx_ops *trx_ops = hal_info->trx_ops;

#ifdef RTW_WKARD_CCX_RPT_LIMIT_CTRL
	if (tx_req->mdata.spe_rpt) {
		if (tx_req->mdata.data_tx_cnt_lmt_en)
			hal_info->hal_com->spe_pkt_cnt_lmt = tx_req->mdata.data_tx_cnt_lmt;
	}
#endif
	hstatus = trx_ops->hal_fill_wd(hal_info, tx_req, wd_buf, wd_len);

	return hstatus;
}

#endif

#ifdef CONFIG_SDIO_HCI
void rtw_hal_sdio_tx_cfg(void *hal)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;


	rtw_hal_mac_sdio_tx_cfg(hal_info->hal_com);
}

enum rtw_hal_status rtw_hal_sdio_tx(void *hal, u8 dma_ch, u8 *buf, u32 buf_len,
				    u8 agg_count, u16 *pkt_len, u8 *wp_offset)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	u32 txaddr;
	u32 txlen;
	bool ready;


	ready = rtw_hal_mac_sdio_check_tx_allow(hal_info->hal_com, dma_ch,
						buf, buf_len, agg_count,
						pkt_len, wp_offset, &txaddr,
						&txlen);
	if (!ready)
		return RTW_HAL_STATUS_RESOURCE;

	hal_sdio_cmd53_w(hal_info->hal_com, txaddr, txlen, buf);

	return RTW_HAL_STATUS_SUCCESS;
}
#endif /* CONFIG_SDIO_HCI */

enum rtw_hal_status rtw_hal_cfg_hw_cts2self(void *hal, u8 band_sel, u8 enable,
					    u8 non_sec_thr, u8 sec_thr)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct mac_ax_cts2self_cfg mac_hw_cts_cfg;
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_FAILURE;

	if (enable)
		mac_hw_cts_cfg.threshold_sel = MAC_AX_CTS2SELF_BOTH_THRESHOLD;
	else
		mac_hw_cts_cfg.threshold_sel = MAC_AX_CTS2SELF_DISABLE;

	mac_hw_cts_cfg.band_sel = band_sel;
	mac_hw_cts_cfg.non_sec_threshold = non_sec_thr;
	mac_hw_cts_cfg.sec_threshold = sec_thr;

	hstatus = rtw_hal_mac_cfg_hw_cts2slef(hal_info, &mac_hw_cts_cfg);

	return hstatus;
}


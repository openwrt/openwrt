/******************************************************************************
 *
 * Copyright(c) 2019 - 2022 Realtek Corporation.
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
#define _PHL_TX_C_
#include "phl_headers.h"

/**
 * this function will be used in read / write pointer mechanism and
 * return the number of available read pointer
 * @rptr: input, the read pointer
 * @wptr: input, the write pointer
 * @bndy: input, the boundary of read / write pointer mechanism
 */
u16 phl_calc_avail_rptr(u16 rptr, u16 wptr, u16 bndy)
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
u16 phl_calc_avail_wptr(u16 rptr, u16 wptr, u16 bndy)
{
	u16 avail_wptr = 0;

	if (rptr > wptr)
		avail_wptr = rptr - wptr - 1;
	else if (wptr >= rptr)
		avail_wptr = rptr + (bndy - wptr) - 1;

	return avail_wptr;
}

void phl_dump_sorted_ring(_os_list *sorted_ring)
{
	struct phl_ring_status *ring_sts;
	u16 i = 0;

	PHL_TRACE(COMP_PHL_XMIT, _PHL_INFO_, "==dump sorted ring==\n");

	phl_list_for_loop(ring_sts, struct phl_ring_status, sorted_ring,
			       list) {
		i++;
		PHL_TRACE(COMP_PHL_XMIT, _PHL_INFO_, "==ring %d==\n", i);
		PHL_TRACE(COMP_PHL_XMIT, _PHL_INFO_, "ring_sts->req_busy = %d\n",
			  ring_sts->req_busy);
		PHL_TRACE(COMP_PHL_XMIT, _PHL_INFO_, "ring_sts->ring_ptr->cat = %d\n",
			  ring_sts->ring_ptr->cat);
	}
}

void phl_dump_tx_plan(_os_list *sta_list)
{
	struct phl_tx_plan *tx_plan;
	u16 i = 0;

	PHL_TRACE(COMP_PHL_XMIT, _PHL_INFO_, "==dump tx plan==\n");

	phl_list_for_loop(tx_plan, struct phl_tx_plan, sta_list,
			       list) {
		i++;
		PHL_TRACE(COMP_PHL_XMIT, _PHL_INFO_, "==tx plan %d==\n", i);
		PHL_TRACE(COMP_PHL_XMIT, _PHL_INFO_, "tx_plan->sleep = %d\n",
			  tx_plan->sleep);
		PHL_TRACE(COMP_PHL_XMIT, _PHL_INFO_, "tx_plan->has_mgnt = %d\n",
			  tx_plan->has_mgnt);
		PHL_TRACE(COMP_PHL_XMIT, _PHL_INFO_, "tx_plan->has_hiq = %d\n",
			  tx_plan->has_hiq);
		phl_dump_sorted_ring(&tx_plan->sorted_ring);
	}
}

void phl_dump_t_fctrl_result(_os_list *t_fctrl_result)
{
	struct phl_ring_status *ring_sts;
	u16 i = 0;

	PHL_TRACE(COMP_PHL_XMIT, _PHL_INFO_, "==dump tx flow control result==\n");

	phl_list_for_loop(ring_sts, struct phl_ring_status, t_fctrl_result,
						   list) {
		i++;
		PHL_TRACE(COMP_PHL_XMIT, _PHL_INFO_, "==ring %d==\n", i);
		PHL_TRACE(COMP_PHL_XMIT, _PHL_INFO_, "ring_sts->req_busy = %d\n",
			  ring_sts->req_busy);
		PHL_TRACE(COMP_PHL_XMIT, _PHL_INFO_, "ring_sts->ring_ptr->cat = %d\n",
			  ring_sts->ring_ptr->cat);
	}
}

void phl_dump_tx_stats(struct rtw_stats *stats)
{
	PHL_TRACE(COMP_PHL_XMIT, _PHL_DEBUG_,
		  "Dump Tx statistics\n"
		  "tx_byte_uni = %lld\n"
		  "tx_byte_total = %lld\n"
		  "tx_tp_kbits = %d\n"
		  "last_tx_time_ms = %d\n",
		  stats->tx_byte_uni,
		  stats->tx_byte_total,
		  stats->tx_tp_kbits,
		  stats->last_tx_time_ms);
}

void phl_dump_h2c_pool_stats(struct phl_h2c_pkt_pool *h2c_pkt_pool)
{
	PHL_INFO("[h2c_stats] idle cmd %d, idle data %d, idle ldata %d, busy h2c %d.\n",
				h2c_pkt_pool->idle_h2c_pkt_cmd_list.cnt,
				h2c_pkt_pool->idle_h2c_pkt_data_list.cnt,
				h2c_pkt_pool->idle_h2c_pkt_ldata_list.cnt,
				h2c_pkt_pool->busy_h2c_pkt_list.cnt);
}

void phl_reset_tx_stats(struct rtw_stats *stats)
{
	stats->tx_byte_uni = 0;
	stats->tx_byte_total = 0;
	stats->tx_tp_kbits = 0;
	stats->last_tx_time_ms = 0;
	stats->txtp.last_calc_time_ms = 0;
	stats->txtp.last_calc_time_ms = 0;
	stats->tx_traffic.lvl = RTW_TFC_IDLE;
	stats->tx_traffic.sts = 0;
}

const char *phl_tfc_lvl_to_str(u8 lvl)
{
	switch (lvl) {
	case RTW_TFC_IDLE:
		return "IDLE";
	case RTW_TFC_ULTRA_LOW:
		return "ULTRA_LOW";
	case RTW_TFC_LOW:
		return "LOW";
	case RTW_TFC_MID:
		return "MID";
	case RTW_TFC_HIGH:
		return "HIGH";
	default:
		return "-";
	}
}

void
phl_tx_traffic_upd(struct rtw_stats *sts)
{
	u32 tp_k = 0, tp_m = 0;
	enum rtw_tfc_lvl tx_tfc_lvl = RTW_TFC_IDLE;
	tp_k = sts->tx_tp_kbits;
	tp_m = sts->tx_tp_kbits >> 10;

	if (tp_m >= TX_HIGH_TP_THRES_MBPS)
		tx_tfc_lvl = RTW_TFC_HIGH;
	else if (tp_m >= TX_MID_TP_THRES_MBPS)
		tx_tfc_lvl = RTW_TFC_MID;
	else if (tp_m >= TX_LOW_TP_THRES_MBPS)
		tx_tfc_lvl = RTW_TFC_LOW;
	else if (tp_k >= TX_ULTRA_LOW_TP_THRES_KBPS)
		tx_tfc_lvl = RTW_TFC_ULTRA_LOW;
	else
		tx_tfc_lvl = RTW_TFC_IDLE;

	if (sts->tx_traffic.lvl > tx_tfc_lvl) {
		sts->tx_traffic.sts = (TRAFFIC_CHANGED | TRAFFIC_DECREASE);
		sts->tx_traffic.lvl = tx_tfc_lvl;
	} else if (sts->tx_traffic.lvl < tx_tfc_lvl) {
		sts->tx_traffic.sts = (TRAFFIC_CHANGED | TRAFFIC_INCREASE);
		sts->tx_traffic.lvl = tx_tfc_lvl;
	} else if (sts->tx_traffic.sts &
		(TRAFFIC_CHANGED | TRAFFIC_INCREASE | TRAFFIC_DECREASE)) {
		sts->tx_traffic.sts &= ~(TRAFFIC_CHANGED | TRAFFIC_INCREASE |
					 TRAFFIC_DECREASE);
	}
}

void phl_update_tx_stats(struct rtw_stats *stats, struct rtw_xmit_req *tx_req)
{
	u32 diff_t = 0, cur_time = _os_get_cur_time_ms();
	u64 diff_bits = 0;

	stats->last_tx_time_ms = cur_time;
	stats->tx_byte_total += tx_req->total_len;

	stats->txreq_num++;
	if (tx_req->mdata.bc == 0 && tx_req->mdata.mc == 0)
		stats->tx_byte_uni += tx_req->total_len;

	if (0 == stats->txtp.last_calc_time_ms ||
	    0 == stats->txtp.last_calc_bits) {
		stats->txtp.last_calc_time_ms = stats->last_tx_time_ms;
		stats->txtp.last_calc_bits = stats->tx_byte_uni * 8;
	} else {
		if (cur_time >= stats->txtp.last_calc_time_ms) {
			diff_t = cur_time - stats->txtp.last_calc_time_ms;
		} else {
			diff_t = RTW_U32_MAX - stats->txtp.last_calc_time_ms +
				cur_time + 1;
		}
		if (diff_t > TXTP_CALC_DIFF_MS && stats->tx_byte_uni != 0) {
			diff_bits = (stats->tx_byte_uni * 8) -
				stats->txtp.last_calc_bits;
			stats->tx_tp_kbits = (u32)_os_division64(diff_bits,
								 diff_t);
			stats->txtp.last_calc_bits = stats->tx_byte_uni * 8;
			stats->txtp.last_calc_time_ms = cur_time;
		}
	}
}

void phl_tx_statistics(struct phl_info_t *phl_info, struct rtw_xmit_req *tx_req)
{
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct rtw_stats *phl_stats = &phl_com->phl_stats;
	struct rtw_stats *sta_stats = NULL;
	struct rtw_phl_stainfo_t *sta = NULL;
	u16 macid = tx_req->mdata.macid;

	if (!phl_macid_is_valid(phl_info, macid))
		goto dev_stat;

	sta = rtw_phl_get_stainfo_by_macid(phl_info, macid);

	if (NULL == sta)
		goto dev_stat;
	sta_stats = &sta->stats;

	phl_update_tx_stats(sta_stats, tx_req);
dev_stat:
	phl_update_tx_stats(phl_stats, tx_req);
}


static void _phl_free_phl_tring_list(void *phl,
				struct rtw_phl_tring_list *ring_list)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	void *drv_priv = phl_to_drvpriv(phl_info);
	struct rtw_phl_evt_ops *ops = &phl_info->phl_com->evt_ops;
	struct rtw_phl_tx_ring *ring;
	struct rtw_xmit_req *tx_req;
	u16 rptr = 0;
	u8 i = 0;

	if(ring_list==NULL)
		return ;

	for (i = 0; i < MAX_PHL_RING_CAT_NUM; i++) {
		ring = &ring_list->phl_ring[i];
		rptr = (u16)_os_atomic_read(drv_priv, &ring->phl_idx);

		if (ring->entry==NULL) {
			PHL_DBG("tx ring %d entry is empty!\n", i);
			continue;
		}

		while (rptr != ring->core_idx) {
			rptr += 1;
			if (rptr >= MAX_PHL_TX_RING_ENTRY_NUM)
				rptr = 0;
			tx_req = (struct rtw_xmit_req *)ring->entry[rptr];
			if (NULL == tx_req)
				break;
			ops->tx_recycle(drv_priv, tx_req);
		}

		_os_kmem_free(drv_priv, ring->entry, MAX_PHL_TX_RING_ENTRY_NUM * sizeof(u8 *));
		ring->entry = NULL;
	}
	_os_kmem_free(drv_priv, ring_list, sizeof(*ring_list));
}


void _phl_init_tx_plan(struct phl_tx_plan * tx_plan)
{
	INIT_LIST_HEAD(&tx_plan->list);
	tx_plan->sleep = false;
	tx_plan->has_mgnt = false;
	tx_plan->has_hiq = false;
	INIT_LIST_HEAD(&tx_plan->sorted_ring);
}


static struct rtw_phl_tring_list *
_phl_allocate_phl_tring_list(void *phl, u16 macid,
			u8 hw_band, u8 hw_wmm, u8 hw_port)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct rtw_phl_tring_list *phl_tring_list = NULL;
	void *drv_priv = NULL;
	u32 buf_len = 0;
	u8 i = 0, dma_ch = 0;
	u8 fail = 1;
	drv_priv = phl_to_drvpriv(phl_info);

	buf_len = sizeof(struct rtw_phl_tring_list);
	phl_tring_list = (struct rtw_phl_tring_list *)_os_kmem_alloc(drv_priv,
								buf_len);

	if (NULL != phl_tring_list) {
		_os_mem_set(drv_priv, phl_tring_list, 0, buf_len);
		INIT_LIST_HEAD(&phl_tring_list->list);
		phl_tring_list->macid = macid;
		phl_tring_list->band = hw_band;
		phl_tring_list->wmm = hw_wmm;
		phl_tring_list->port = hw_port;
		/*phl_tring_list->mbssid = hw_mbssid*/

		for (i = 0; i < MAX_PHL_RING_CAT_NUM; i++) {
			phl_tring_list->phl_ring[i].cat = i;
			dma_ch = rtw_hal_tx_chnl_mapping(phl_info->hal, macid,
							 i, hw_band);
			phl_tring_list->phl_ring[i].dma_ch = dma_ch;
			phl_tring_list->phl_ring[i].entry = (u8 **)_os_kmem_alloc(drv_priv, MAX_PHL_TX_RING_ENTRY_NUM * sizeof(u8 *));
			if (phl_tring_list->phl_ring[i].entry == NULL) {
				PHL_DBG("cannot malloc %lu bytes, stop!", (unsigned long int)(MAX_PHL_TX_RING_ENTRY_NUM * sizeof(u8 *)));
				goto exit;
			}
		}
		_phl_init_tx_plan(&phl_tring_list->tx_plan);
		fail = 0;
	}
exit:
	if (fail) {
		_phl_free_phl_tring_list(phl_info, phl_tring_list);
		phl_tring_list = NULL;
	}

	return phl_tring_list;
}

enum rtw_phl_status
phl_register_tx_ring(void *phl, u16 macid, u8 hw_band, u8 hw_wmm, u8 hw_port)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	void *drv_priv = phl_to_drvpriv(phl_info);
	struct rtw_phl_tring_list *phl_tring_list = NULL;
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_FAILURE;
	_os_list *ring_list = NULL;

	phl_tring_list = _phl_allocate_phl_tring_list(phl, macid, hw_band, hw_wmm, hw_port);

	if (NULL != phl_tring_list) {
		ring_list = &phl_info->t_ring_list;
		_os_spinlock(drv_priv, &phl_info->t_ring_list_lock, _bh, NULL);
		list_add_tail(&phl_tring_list->list, ring_list);
		_os_spinunlock(drv_priv, &phl_info->t_ring_list_lock, _bh, NULL);

		phl_status = RTW_PHL_STATUS_SUCCESS;
	}

	return phl_status;
}

enum rtw_phl_status phl_deregister_tx_ring(void *phl, u16 macid)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	void *drv_priv = phl_to_drvpriv(phl_info);
	struct rtw_phl_tring_list *phl_tring_list = NULL, *t;
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_FAILURE;
	_os_list *ring_list = NULL;

	ring_list = &phl_info->t_ring_list;

	_os_spinlock(drv_priv, &phl_info->t_ring_list_lock, _bh, NULL);

	phl_list_for_loop_safe(phl_tring_list, t, struct rtw_phl_tring_list,
					ring_list, list) {
		if (macid == phl_tring_list->macid) {
			list_del(&phl_tring_list->list);
			phl_status = RTW_PHL_STATUS_SUCCESS;
			break;
		}
	}

	_os_spinunlock(drv_priv, &phl_info->t_ring_list_lock, _bh, NULL);

	if (RTW_PHL_STATUS_SUCCESS == phl_status) {
		/* defer the free operation to avoid racing with _phl_tx_callback_xxx */
		_os_spinlock(drv_priv, &phl_info->t_ring_free_list_lock, _bh, NULL);
		list_add_tail(&phl_tring_list->list, &phl_info->t_ring_free_list);
		_os_spinunlock(drv_priv, &phl_info->t_ring_free_list_lock, _bh, NULL);
	}

	return phl_status;
}

enum rtw_phl_status
phl_re_register_tx_ring(void *phl, u16 macid, u8 hw_band, u8 hw_wmm, u8 hw_port)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	void *drv_priv = phl_to_drvpriv(phl_info);
	struct rtw_phl_tring_list *phl_tring_list = NULL, *t;
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_FAILURE;
	_os_list *ring_list = NULL;
	u8 i = 0, dma_ch = 0;

	ring_list = &phl_info->t_ring_list;

	_os_spinlock(drv_priv, &phl_info->t_ring_list_lock, _bh, NULL);

	phl_list_for_loop_safe(phl_tring_list, t, struct rtw_phl_tring_list,
					ring_list, list) {
		if (macid == phl_tring_list->macid) {

			phl_tring_list->band = hw_band;
			phl_tring_list->wmm = hw_wmm;
			phl_tring_list->port = hw_port;

			for (i = 0; i < MAX_PHL_RING_CAT_NUM; i++) {
				phl_tring_list->phl_ring[i].cat = i;
				dma_ch = rtw_hal_tx_chnl_mapping(phl_info->hal, macid,
							 i, hw_band);
				phl_tring_list->phl_ring[i].dma_ch = dma_ch;
			}
			phl_status = RTW_PHL_STATUS_SUCCESS;
			break;
		}
	}

	_os_spinunlock(drv_priv, &phl_info->t_ring_list_lock, _bh, NULL);
	return phl_status;
}

void phl_free_deferred_tx_ring(struct phl_info_t *phl_info)
{
	void *drv_priv = phl_to_drvpriv(phl_info);
	struct rtw_phl_tring_list *phl_tring_list = NULL, *t;
	_os_list *ring_list = NULL;

	ring_list = &phl_info->t_ring_free_list;

	_os_spinlock(drv_priv, &phl_info->t_ring_free_list_lock, _bh, NULL);
	if (list_empty(ring_list) == false) {
		phl_list_for_loop_safe(phl_tring_list, t, struct rtw_phl_tring_list,
						ring_list, list) {
			list_del(&phl_tring_list->list);
			_os_spinunlock(drv_priv, &phl_info->t_ring_free_list_lock, _bh, NULL);
			_phl_free_phl_tring_list(phl_info, phl_tring_list);
			_os_spinlock(drv_priv, &phl_info->t_ring_free_list_lock, _bh, NULL);
		}
	}
	_os_spinunlock(drv_priv, &phl_info->t_ring_free_list_lock, _bh, NULL);
}


struct phl_ring_status *phl_alloc_ring_sts(struct phl_info_t *phl_info)
{
	struct phl_ring_sts_pool *ring_sts_pool = phl_info->ring_sts_pool;
	struct phl_ring_status *ring_sts = NULL;

	_os_spinlock(phl_to_drvpriv(phl_info), &ring_sts_pool->idle_lock, _bh, NULL);

	if (false == list_empty(&ring_sts_pool->idle)) {
		ring_sts = list_first_entry(&ring_sts_pool->idle,
					struct phl_ring_status, list);
		list_del(&ring_sts->list);
	}

	_os_spinunlock(phl_to_drvpriv(phl_info), &ring_sts_pool->idle_lock, _bh, NULL);

	return ring_sts;
}

void phl_release_ring_sts(struct phl_info_t *phl_info,
				struct phl_ring_status *ring_sts)
{
	struct phl_ring_sts_pool *ring_sts_pool = phl_info->ring_sts_pool;
	void *drv_priv = NULL;

	drv_priv = phl_to_drvpriv(phl_info);

	_os_spinlock(drv_priv, &ring_sts_pool->idle_lock, _bh, NULL);
	_os_mem_set(drv_priv, ring_sts, 0, sizeof(*ring_sts));
	INIT_LIST_HEAD(&ring_sts->list);
	list_add_tail(&ring_sts->list, &ring_sts_pool->idle);
	_os_spinunlock(drv_priv, &ring_sts_pool->idle_lock, _bh, NULL);
}


void _phl_ring_status_deinit(struct phl_info_t *phl_info)
{
	struct phl_ring_sts_pool *ring_sts_pool = NULL;
	u16 buf_len = 0;
	void *drv_priv = NULL;
	FUNCIN();
	drv_priv = phl_to_drvpriv(phl_info);
	ring_sts_pool = (struct phl_ring_sts_pool *)phl_info->ring_sts_pool;
	if (NULL != ring_sts_pool) {
		buf_len = sizeof(struct phl_ring_sts_pool);
		_os_spinlock_free(drv_priv, &ring_sts_pool->idle_lock);
		_os_spinlock_free(drv_priv, &ring_sts_pool->busy_lock);
		_os_mem_free(drv_priv, ring_sts_pool, buf_len);
	}
	FUNCOUT();
}


enum rtw_phl_status _phl_ring_status_init(struct phl_info_t *phl_info)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	struct phl_ring_sts_pool *ring_sts_pool = NULL;
	struct phl_ring_status *ring_sts = NULL;
	void *drv_priv = NULL;
	u16 buf_len = 0;
	u16 i = 0;
	FUNCIN_WSTS(pstatus);

	drv_priv = phl_to_drvpriv(phl_info);
	buf_len = sizeof(struct phl_ring_sts_pool);
	ring_sts_pool =
			(struct phl_ring_sts_pool *)_os_mem_alloc(drv_priv, buf_len);

	if (NULL != ring_sts_pool) {
		_os_mem_set(drv_priv, ring_sts_pool, 0, buf_len);
		INIT_LIST_HEAD(&ring_sts_pool->idle);
		INIT_LIST_HEAD(&ring_sts_pool->busy);
		_os_spinlock_init(drv_priv, &ring_sts_pool->idle_lock);
		_os_spinlock_init(drv_priv, &ring_sts_pool->busy_lock);

		for (i = 0; i < MAX_PHL_RING_STATUS_NUMBER; i++) {
			ring_sts = &ring_sts_pool->ring_sts[i];
			INIT_LIST_HEAD(&ring_sts->list);
			_os_spinlock(drv_priv,
					(void *)&ring_sts_pool->idle_lock, _bh, NULL);
			list_add_tail(&ring_sts->list, &ring_sts_pool->idle);
			_os_spinunlock(drv_priv,
					(void *)&ring_sts_pool->idle_lock, _bh, NULL);
		}

		phl_info->ring_sts_pool = ring_sts_pool;

		pstatus = RTW_PHL_STATUS_SUCCESS;
	}

	if (RTW_PHL_STATUS_SUCCESS != pstatus)
		_phl_ring_status_deinit(phl_info);
	FUNCOUT_WSTS(pstatus);

	return pstatus;
}

struct phl_ring_status *
_phl_check_ring_status(struct phl_info_t *phl_info,
					struct rtw_phl_tx_ring *ring,
					struct rtw_phl_tring_list *tring_list)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	struct phl_ring_status *ring_sts = NULL;
	u16 avail = 0, rptr = 0;
	void *drv_priv = phl_to_drvpriv(phl_info);

	do {
		rptr = (u16)_os_atomic_read(drv_priv, &ring->phl_idx);

		avail = phl_calc_avail_rptr(rptr, ring->core_idx,
					MAX_PHL_TX_RING_ENTRY_NUM);
		if (0 == avail) {
			ring_sts = NULL;
			pstatus = RTW_PHL_STATUS_SUCCESS;
			break;
		} else {
			ring_sts = phl_alloc_ring_sts(phl_info);
			if (NULL == ring_sts) {
				PHL_ERR("query ring status fail!\n");
				pstatus = RTW_PHL_STATUS_RESOURCE;
				break;
			}
			ring_sts->band = tring_list->band;
			ring_sts->wmm = tring_list->wmm;
			ring_sts->port = tring_list->port;
			/*ring_sts->mbssid = tring_list->mbssid;*/
			ring_sts->req_busy = avail;
			ring_sts->ring_ptr = ring;

			rptr += 1;

			if (rptr >= MAX_PHL_TX_RING_ENTRY_NUM)
				_os_atomic_set(drv_priv, &ring->phl_next_idx, 0);
			else
				_os_atomic_set(drv_priv, &ring->phl_next_idx, rptr);

			pstatus = RTW_PHL_STATUS_SUCCESS;
			break;
		}
	} while (false);

	return ring_sts;
}

void _phl_reset_tx_plan(struct phl_info_t *phl_info,
			 struct phl_tx_plan *tx_plan)
{
	struct phl_ring_status *ring_sts, *t;

	INIT_LIST_HEAD(&tx_plan->list);
        tx_plan->sleep = false;
	tx_plan->has_mgnt = false;
	tx_plan->has_hiq = false;
	phl_list_for_loop_safe(ring_sts, t, struct phl_ring_status,
			       &tx_plan->sorted_ring, list) {
		list_del(&ring_sts->list);
		phl_release_ring_sts(phl_info, ring_sts);
	}
	INIT_LIST_HEAD(&tx_plan->sorted_ring);
}


void _phl_sort_ring_by_tid(struct phl_ring_status *ring_sts,
			   struct phl_tx_plan *tx_plan,
			   enum rtw_phl_ring_cat cat)
{
	struct phl_ring_status *last_sts = NULL;

	if (ring_sts->ring_ptr->cat == RTW_PHL_RING_CAT_TID1) {
		list_add_tail(&ring_sts->list,
			      &tx_plan->sorted_ring);
	} else if (ring_sts->ring_ptr->cat == RTW_PHL_RING_CAT_TID2) {
		if (list_empty(&tx_plan->sorted_ring)) {
			list_add_tail(&ring_sts->list,
				      &tx_plan->sorted_ring);
		} else {
			last_sts = list_last_entry(&tx_plan->sorted_ring,
			                   struct phl_ring_status, list);
			if (RTW_PHL_RING_CAT_TID1 == last_sts->ring_ptr->cat) {
				__list_add(&ring_sts->list,
					   _get_prev(&last_sts->list),
					   &last_sts->list);
			} else {
				list_add_tail(&ring_sts->list,
					      &tx_plan->sorted_ring);
                        }
		}
	} else {
		list_add(&ring_sts->list,
			 &tx_plan->sorted_ring);
		if (RTW_PHL_RING_CAT_MGNT == cat)
			tx_plan->has_mgnt = true;
		else if (RTW_PHL_RING_CAT_HIQ == cat)
			tx_plan->has_hiq = true;
	}

}

void _phl_check_tring_list(struct phl_info_t *phl_info,
			   struct rtw_phl_tring_list *tring_list,
			   _os_list *sta_list)
{
	struct phl_ring_status *ring_sts = NULL;
	struct rtw_phl_tx_ring *ring = NULL;
	struct phl_tx_plan *tx_plan = &tring_list->tx_plan;
	u8 i = 0;

	for (i = 0; i < MAX_PHL_RING_CAT_NUM; i++) {

		ring = &tring_list->phl_ring[i];

		ring_sts = _phl_check_ring_status(phl_info, ring, tring_list);

		if (NULL != ring_sts) {
			_phl_sort_ring_by_tid(ring_sts, tx_plan, i);
		} else {
			continue;
		}
	}
	/* hana_todo: check this macid is sleep or not */
	if (!list_empty(&tx_plan->sorted_ring)) {
		list_add_tail(&tx_plan->list, sta_list);
	}
}

u8 phl_check_xmit_ring_resource(struct phl_info_t *phl_info, _os_list *sta_list)
{
	void *drvpriv = phl_to_drvpriv(phl_info);
	_os_list *tring_list_head = &phl_info->t_ring_list;
	struct rtw_phl_tring_list *tring_list, *t;

	_os_spinlock(drvpriv, &phl_info->t_ring_list_lock, _bh, NULL);
	phl_list_for_loop_safe(tring_list, t, struct rtw_phl_tring_list,
				tring_list_head, list) {
		_phl_check_tring_list(phl_info, tring_list, sta_list);

		 /* use the first phl tx ring */
		if (phl_info->use_onetxring)
			break;
	}

#ifdef SDIO_TX_THREAD
	/**
	* when SDIO_TX_THREAD is enabled,
	* clearing variable "phl_sw_tx_more" in function "phl_tx_sdio_thrd_hdl"
	*/
#else
	_os_atomic_set(drvpriv, &phl_info->phl_sw_tx_more, 0);
#endif
	_os_spinunlock(drvpriv, &phl_info->t_ring_list_lock, _bh, NULL);

	if (true == list_empty(sta_list))
		return false;
	else
		return true;
}

void phl_tx_flow_ctrl(struct phl_info_t *phl_info, _os_list *sta_list)
{
	_os_list *t_fctrl_result = &phl_info->t_fctrl_result;
	_os_list *tid_entry[MAX_PHL_RING_CAT_NUM] = {0};
	struct phl_tx_plan *tx_plan, *tp;
	struct phl_ring_status *ring_sts = NULL, *ts;
	u8 tid = 0;

	_os_mem_set(phl_to_drvpriv(phl_info), tid_entry, 0,
		    sizeof(_os_list *) * MAX_PHL_RING_CAT_NUM);

	phl_list_for_loop_safe(tx_plan, tp, struct phl_tx_plan, sta_list,
			       list) {
		/* drop power saving station */
		if (true == tx_plan->sleep) {
			list_del(&tx_plan->list);
			_phl_reset_tx_plan(phl_info, tx_plan);
			continue;
		}

		if (true == tx_plan->has_hiq) {
			ring_sts = list_first_entry(&tx_plan->sorted_ring,
						struct phl_ring_status, list);
			list_del(&ring_sts->list);
			list_add(&ring_sts->list, t_fctrl_result);
		}

		if (true == tx_plan->has_mgnt) {
			ring_sts = list_first_entry(&tx_plan->sorted_ring,
						struct phl_ring_status, list);
			list_del(&ring_sts->list);
			list_add(&ring_sts->list, t_fctrl_result);
		}

		/* todo: drop station which has reached tx limit */

		phl_list_for_loop_safe(ring_sts, ts, struct phl_ring_status,
				       &tx_plan->sorted_ring, list) {
			list_del(&ring_sts->list);
			tid = rtw_phl_cvt_cat_to_tid(ring_sts->ring_ptr->cat);
			/* todo: drop tid which has reached tx limit */
			/* sw tx cnt limit */
			if (NULL == tid_entry[tid]) {
				list_add_tail(&ring_sts->list, t_fctrl_result);
			} else {
				__list_add(&ring_sts->list, tid_entry[tid],
					   _get_next(tid_entry[tid]));
			}
			tid_entry[tid] = &ring_sts->list;
		}

		/* clear tx plan */
		list_del(&tx_plan->list);
		_phl_reset_tx_plan(phl_info, tx_plan);
	}
}

enum rtw_phl_status phl_register_handler(struct rtw_phl_com_t *phl_com,
					 struct rtw_phl_handler *handler)
{
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_FAILURE;
	_os_tasklet *tasklet = NULL;
	_os_workitem *workitem = NULL;
	void *drv_priv = phlcom_to_drvpriv(phl_com);

	FUNCIN_WSTS(phl_status);

	#ifdef CONFIG_RTW_OS_HANDLER_EXT
	_os_init_handler_ext(drv_priv, handler);
	#endif /* CONFIG_RTW_OS_HANDLER_EXT */

	if (handler->type == RTW_PHL_HANDLER_PRIO_HIGH) {
		tasklet = &handler->os_handler.u.tasklet;
		phl_status = _os_tasklet_init(drv_priv, tasklet,
						handler->callback, handler);
	} else if (handler->type == RTW_PHL_HANDLER_PRIO_LOW) {
		workitem = &handler->os_handler.u.workitem;
		phl_status = _os_workitem_init(drv_priv, workitem,
						handler->callback, workitem);
	} else if (handler->type == RTW_PHL_HANDLER_PRIO_NORMAL) {
		_os_sema_init(drv_priv, &(handler->os_handler.hdlr_sema), 0);
		handler->os_handler.hdlr_created = false;
		phl_status = RTW_PHL_STATUS_SUCCESS;
	} else {
		PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "[WARNING] unknown handle type(%d)\n",
				handler->type);
	}

	if (RTW_PHL_STATUS_SUCCESS != phl_status)
		phl_deregister_handler(phl_com, handler);

	FUNCOUT_WSTS(phl_status);
	return phl_status;
}

enum rtw_phl_status phl_deregister_handler(
	struct rtw_phl_com_t *phl_com, struct rtw_phl_handler *handler)
{
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_FAILURE;
	_os_tasklet *tasklet = NULL;
	_os_workitem *workitem = NULL;
	_os_thread *thread = NULL;
	void *drv_priv = phlcom_to_drvpriv(phl_com);

	FUNCIN_WSTS(phl_status);

	#ifdef CONFIG_RTW_OS_HANDLER_EXT
	_os_deinit_handler_ext(drv_priv, handler);
	#endif /* CONFIG_RTW_OS_HANDLER_EXT */

	if (handler->type == RTW_PHL_HANDLER_PRIO_HIGH) {
		tasklet = &handler->os_handler.u.tasklet;
		phl_status = _os_tasklet_deinit(drv_priv, tasklet);
	} else if (handler->type == RTW_PHL_HANDLER_PRIO_LOW) {
		workitem = &handler->os_handler.u.workitem;
		phl_status = _os_workitem_deinit(drv_priv, workitem);
	} else if (handler->type == RTW_PHL_HANDLER_PRIO_NORMAL) {
		thread = &handler->os_handler.u.thread;
		if (handler->os_handler.hdlr_created == true) {
			_os_thread_stop(drv_priv, thread);
			_os_sema_up(drv_priv, &(handler->os_handler.hdlr_sema));
			phl_status = _os_thread_deinit(drv_priv, thread);
		} else {
			phl_status = RTW_PHL_STATUS_SUCCESS;
		}

		_os_sema_free(drv_priv, &(handler->os_handler.hdlr_sema));
	} else {
		PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "[WARNING] unknown handle type(%d)\n",
				handler->type);
	}

	if (RTW_PHL_STATUS_SUCCESS != phl_status) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_,
			"[WARNING] deregister handler fail (status = 0x%08X)\n",
			phl_status);
	}

	FUNCOUT_WSTS(phl_status);
	return phl_status;
}

static int _phl_thread_handler(void *context)
{
	struct rtw_phl_handler *phl_handler;
	struct phl_info_t *phl_info;
	void *d;


	phl_handler = (struct rtw_phl_handler *)phl_container_of(context,
							struct rtw_phl_handler,
							os_handler);
	phl_info = (struct phl_info_t *)phl_handler->context;
	d = phl_to_drvpriv(phl_info);

	PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "%s start\n", phl_handler->cb_name);

	while (1) {
		_os_sema_down(d, &phl_handler->os_handler.hdlr_sema);

		if (_os_thread_check_stop(d, (_os_thread*)context))
			break;

		phl_handler->callback(context);
	}

	_os_thread_wait_stop(d, (_os_thread*)context);
	_os_sema_free(d, &phl_handler->os_handler.hdlr_sema);

	PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "%s down\n", phl_handler->cb_name);

	return 0;
}

enum rtw_phl_status phl_schedule_handler(
	struct rtw_phl_com_t *phl_com, struct rtw_phl_handler *handler)
{
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_FAILURE;
	_os_tasklet *tasklet = NULL;
	_os_workitem *workitem = NULL;
	_os_thread *thread = NULL;
	void *drv_priv = phlcom_to_drvpriv(phl_com);

	FUNCIN_WSTS(phl_status);

	if (handler->type == RTW_PHL_HANDLER_PRIO_HIGH) {
		tasklet = &handler->os_handler.u.tasklet;
		phl_status = _os_tasklet_schedule(drv_priv, tasklet);
	} else if (handler->type == RTW_PHL_HANDLER_PRIO_LOW) {
		workitem = &handler->os_handler.u.workitem;
		phl_status = _os_workitem_schedule(drv_priv, workitem);
	} else if (handler->type == RTW_PHL_HANDLER_PRIO_NORMAL) {
		thread = &handler->os_handler.u.thread;
		if (handler->os_handler.hdlr_created == false) {
			phl_status = _os_thread_init(drv_priv, thread,
						     _phl_thread_handler,
						     thread, handler->cb_name);
			if (phl_status == RTW_PHL_STATUS_SUCCESS) {
				handler->os_handler.hdlr_created = true;
				_os_thread_schedule(drv_priv, thread);
				_os_sema_up(drv_priv, &(handler->os_handler.hdlr_sema));
			}
		} else {
			_os_sema_up(drv_priv, &(handler->os_handler.hdlr_sema));
			phl_status = RTW_PHL_STATUS_SUCCESS;
		}
	} else {
		PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "[WARNING] unknown handle type(%d)\n",
				handler->type);
	}

	FUNCOUT_WSTS(phl_status);
	return phl_status;
}

static enum rtw_phl_status enqueue_h2c_pkt(
					struct phl_info_t *phl_info,
					struct phl_queue	*pool_list,
					struct rtw_h2c_pkt *h2c_pkt, u8 pos)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	void *drv = phl_to_drvpriv(phl_info);
	_os_spinlockfg sp_flags;


	if (h2c_pkt != NULL) {
		_os_spinlock(drv, &pool_list->lock, _irq, &sp_flags);
		if (_tail == pos)
			list_add_tail(&h2c_pkt->list, &pool_list->queue);
		else if (_first == pos)
			list_add(&h2c_pkt->list, &pool_list->queue);
		pool_list->cnt++;
		_os_spinunlock(drv, &pool_list->lock, _irq, &sp_flags);

		pstatus = RTW_PHL_STATUS_SUCCESS;
	}

	return pstatus;
}

static struct rtw_h2c_pkt *dequeue_h2c_pkt(struct phl_info_t *phl_info,
	struct phl_queue *pool_list)
{
	struct rtw_h2c_pkt *h2c_pkt = NULL;
	void *drv = phl_to_drvpriv(phl_info);
	_os_spinlockfg sp_flags;

	_os_spinlock(drv, &pool_list->lock, _irq, &sp_flags);
	if (list_empty(&pool_list->queue)) {
		h2c_pkt = NULL;
	} else {
		h2c_pkt = list_first_entry(&pool_list->queue, struct rtw_h2c_pkt, list);

		list_del(&h2c_pkt->list);
		pool_list->cnt--;
	}
	_os_spinunlock(drv, &pool_list->lock, _irq, &sp_flags);
	return h2c_pkt;
}

static void _phl_reset_h2c_pkt(struct phl_info_t *phl_info,
							struct rtw_h2c_pkt *h2c_pkt,
							u32 buf_len)
{
	enum rtw_h2c_pkt_type type = h2c_pkt->type;

	_os_mem_set(phl_to_drvpriv(phl_info), h2c_pkt->vir_head, 0, buf_len);
	h2c_pkt->buf_len = buf_len;
	h2c_pkt->id = 0;
	h2c_pkt->host_idx = 0;
	h2c_pkt->data_len = 0;
	h2c_pkt->h2c_seq = 0;

	switch (type) {
	case H2CB_TYPE_CMD:
		h2c_pkt->vir_data = h2c_pkt->vir_head + FWCMD_HDR_LEN + _WD_BODY_LEN;
		h2c_pkt->vir_tail = h2c_pkt->vir_data;
		h2c_pkt->vir_end = h2c_pkt->vir_data + H2C_CMD_LEN;
		break;
	case H2CB_TYPE_DATA:
		h2c_pkt->vir_data = h2c_pkt->vir_head + FWCMD_HDR_LEN + _WD_BODY_LEN;
		h2c_pkt->vir_tail = h2c_pkt->vir_data;
		h2c_pkt->vir_end = h2c_pkt->vir_data + H2C_DATA_LEN;
		break;
	case H2CB_TYPE_LONG_DATA:
		h2c_pkt->vir_data = h2c_pkt->vir_head + FWCMD_HDR_LEN + _WD_BODY_LEN;
		h2c_pkt->vir_tail = h2c_pkt->vir_data;
		h2c_pkt->vir_end = h2c_pkt->vir_data + H2C_LONG_DATA_LEN;
		break;
	case H2CB_TYPE_MAX:
		PHL_TRACE(COMP_PHL_DBG, _PHL_DEBUG_, "_phl_reset_h2c_pkt(): Unsupported case:%d, please check it\n",
				type);
		break;
	default:
		PHL_TRACE(COMP_PHL_DBG, _PHL_DEBUG_, "_phl_reset_h2c_pkt(): Unrecognize case:%d, please check it\n",
				type);
		break;
	}

}

enum rtw_phl_status phl_enqueue_busy_h2c_pkt(struct phl_info_t *phl_info,
				struct rtw_h2c_pkt *h2c_pkt, u8 pos)
{
	struct phl_h2c_pkt_pool *h2c_pkt_pool =
		(struct phl_h2c_pkt_pool *)phl_info->h2c_pool;
	struct phl_queue *queue = &h2c_pkt_pool->busy_h2c_pkt_list;

	return enqueue_h2c_pkt(phl_info, queue, h2c_pkt, pos);
}

enum rtw_phl_status phl_enqueue_idle_h2c_pkt(
				struct phl_info_t *phl_info,
				struct rtw_h2c_pkt *h2c_pkt)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	struct phl_h2c_pkt_pool *h2c_pkt_pool =
		(struct phl_h2c_pkt_pool *)phl_info->h2c_pool;
	struct phl_queue *queue = NULL;
	int *idle_cnt = NULL;
	u32 buf_len = 0;

	if (!h2c_pkt)
		return pstatus;

	switch (h2c_pkt->type) {
	case H2CB_TYPE_CMD:
		buf_len = FWCMD_HDR_LEN + _WD_BODY_LEN + H2C_CMD_LEN;
		queue = &h2c_pkt_pool->idle_h2c_pkt_cmd_list;
		idle_cnt = &h2c_pkt_pool->idle_h2c_pkt_cmd_list.cnt;
		break;
	case H2CB_TYPE_DATA:
		buf_len = FWCMD_HDR_LEN + _WD_BODY_LEN + H2C_DATA_LEN;
		queue = &h2c_pkt_pool->idle_h2c_pkt_data_list;
		idle_cnt = &h2c_pkt_pool->idle_h2c_pkt_data_list.cnt;
		break;
	case H2CB_TYPE_LONG_DATA:
		buf_len = FWCMD_HDR_LEN + _WD_BODY_LEN + H2C_LONG_DATA_LEN;
		queue = &h2c_pkt_pool->idle_h2c_pkt_ldata_list;
		idle_cnt = &h2c_pkt_pool->idle_h2c_pkt_ldata_list.cnt;
		break;
	case H2CB_TYPE_MAX:
		PHL_ERR("%s : cannot find the matching case(%d).\n",
				__func__, h2c_pkt->type);
		break;
	default:
		PHL_ERR("%s : cannot find the matching cases(%d).\n",
				__func__, h2c_pkt->type);
		break;
	}

	_phl_reset_h2c_pkt(phl_info, h2c_pkt, buf_len);

	pstatus = enqueue_h2c_pkt(phl_info, queue, h2c_pkt, _tail);

	PHL_TRACE(COMP_PHL_DBG, _PHL_DEBUG_, "%s : remaining %d (type %d).\n",
			  __func__, *idle_cnt, h2c_pkt->type);

	return pstatus;
}

struct rtw_h2c_pkt *phl_query_busy_h2c_pkt(struct phl_info_t *phl_info)
{
	struct phl_h2c_pkt_pool *h2c_pkt_pool = NULL;
	struct rtw_h2c_pkt *h2c_pkt = NULL;
	struct phl_queue *queue = NULL;

	h2c_pkt_pool = (struct phl_h2c_pkt_pool *)phl_info->h2c_pool;
	queue = &h2c_pkt_pool->busy_h2c_pkt_list;

	h2c_pkt = dequeue_h2c_pkt(phl_info, queue);

	return h2c_pkt;
}

struct rtw_h2c_pkt *phl_query_idle_h2c_pkt(struct phl_info_t *phl_info, u8 type)
{
	struct phl_h2c_pkt_pool *h2c_pkt_pool = NULL;
	struct rtw_h2c_pkt *h2c_pkt = NULL;
	enum rtw_h2c_pkt_type h2c_type = (enum rtw_h2c_pkt_type)type;
	struct phl_queue *queue = NULL;
	int *idle_cnt = NULL;

	h2c_pkt_pool = (struct phl_h2c_pkt_pool *)phl_info->h2c_pool;

	switch (h2c_type) {
	case H2CB_TYPE_CMD:
		queue = &h2c_pkt_pool->idle_h2c_pkt_cmd_list;
		idle_cnt = &h2c_pkt_pool->idle_h2c_pkt_cmd_list.cnt;
		break;
	case H2CB_TYPE_DATA:
		queue = &h2c_pkt_pool->idle_h2c_pkt_data_list;
		idle_cnt = &h2c_pkt_pool->idle_h2c_pkt_data_list.cnt;
		break;
	case H2CB_TYPE_LONG_DATA:
		queue = &h2c_pkt_pool->idle_h2c_pkt_ldata_list;
		idle_cnt = &h2c_pkt_pool->idle_h2c_pkt_ldata_list.cnt;
		break;
	case H2CB_TYPE_MAX:
		PHL_TRACE(COMP_PHL_DBG, _PHL_DEBUG_, "phl_query_idle_h2c_pkt(): Unsupported case:%d, please check it\n",
				h2c_type);
		break;
	default:
		PHL_TRACE(COMP_PHL_DBG, _PHL_DEBUG_, "phl_query_idle_h2c_pkt(): Unrecognize case:%d, please check it\n",
				h2c_type);
		break;
	}
	PHL_TRACE(COMP_PHL_DBG, _PHL_DEBUG_,
		  "phl_query_idle_h2c_pkt => remaining %d (type %d).\n",
		  *idle_cnt, h2c_type);

	h2c_pkt = dequeue_h2c_pkt(phl_info, queue);

	return h2c_pkt;
}

#if 0
static enum rtw_phl_status phl_release_target_h2c_pkt(
					struct phl_info_t *phl_info,
					struct phl_h2c_pkt_pool *h2c_pkt_pool,
					struct rtw_h2c_pkt *h2c_pkt)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;

	if (h2c_pkt_pool != NULL && h2c_pkt != NULL) {
		phl_enqueue_idle_h2c_pkt(phl_info, h2c_pkt);
		pstatus = RTW_PHL_STATUS_SUCCESS;
	}

	return pstatus;
}
#endif

static void _phl_free_h2c_pkt(struct phl_info_t *phl_info,
					struct rtw_h2c_pkt *h2c_pkt_buf)
{
	u16 i = 0;
	struct rtw_h2c_pkt *h2c_pkt = h2c_pkt_buf;
	struct phl_hci_trx_ops *hci_trx_ops = phl_info->hci_trx_ops;

	if (NULL != h2c_pkt) {
		for (i = 0; i < MAX_H2C_PKT_NUM; i++) {
			if (NULL == h2c_pkt->vir_head)
				continue;
			hci_trx_ops->free_h2c_pkt_buf(phl_info, h2c_pkt);
			h2c_pkt->vir_head = NULL;
			h2c_pkt->cache = false;
			h2c_pkt++;
		}

		_os_mem_free(phl_to_drvpriv(phl_info), h2c_pkt_buf,
					sizeof(struct rtw_h2c_pkt) * MAX_H2C_PKT_NUM);
		h2c_pkt_buf = NULL;
	}
}

struct rtw_h2c_pkt *_phl_alloc_h2c_pkt(struct phl_info_t *phl_info,
	struct phl_h2c_pkt_pool *h2c_pool)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	struct phl_hci_trx_ops *hci_trx_ops = phl_info->hci_trx_ops;
	struct rtw_h2c_pkt *h2c_pkt = NULL;
	struct rtw_h2c_pkt *h2c_pkt_root = NULL;
	struct phl_h2c_pkt_pool *h2c_pkt_pool = h2c_pool;
	u32 buf_len = 0;
	int i;

	buf_len = sizeof(struct rtw_h2c_pkt) * MAX_H2C_PKT_NUM;
	h2c_pkt_root = _os_mem_alloc(phl_to_drvpriv(phl_info), buf_len);
	h2c_pkt = h2c_pkt_root;
	if (h2c_pkt != NULL) {
		for (i = 0; i < MAX_H2C_PKT_NUM; i++) {
			h2c_pkt->cache = false;
			buf_len = get_h2c_size_by_range(i);
			hci_trx_ops->alloc_h2c_pkt_buf(phl_info, h2c_pkt, buf_len);

			if (NULL == h2c_pkt->vir_head) {
				pstatus = RTW_PHL_STATUS_RESOURCE;
				break;
			}

			h2c_pkt->buf_len = buf_len;
			h2c_pkt->vir_data = h2c_pkt->vir_head + FWCMD_HDR_LEN + _WD_BODY_LEN;
			h2c_pkt->vir_tail = h2c_pkt->vir_data;
			INIT_LIST_HEAD(&h2c_pkt->list);
			if (i < _H2CB_CMD_QLEN) {
				h2c_pkt->type = H2CB_TYPE_CMD;
				h2c_pkt->vir_end = h2c_pkt->vir_data + H2C_CMD_LEN;
				enqueue_h2c_pkt(phl_info,
					&h2c_pkt_pool->idle_h2c_pkt_cmd_list, h2c_pkt, _tail);
			} else if (i < _H2CB_CMD_QLEN + _H2CB_DATA_QLEN) {
				h2c_pkt->type = H2CB_TYPE_DATA;
				h2c_pkt->vir_end = h2c_pkt->vir_data + H2C_DATA_LEN;
				enqueue_h2c_pkt(phl_info,
					&h2c_pkt_pool->idle_h2c_pkt_data_list, h2c_pkt, _tail);
			} else {
				h2c_pkt->type = H2CB_TYPE_LONG_DATA;
				h2c_pkt->vir_end = h2c_pkt->vir_data + H2C_LONG_DATA_LEN;
				enqueue_h2c_pkt(phl_info,
					&h2c_pkt_pool->idle_h2c_pkt_ldata_list, h2c_pkt, _tail);
			}
			h2c_pkt++;
			pstatus = RTW_PHL_STATUS_SUCCESS;
		}
	}

	if (RTW_PHL_STATUS_SUCCESS != pstatus) {
		_phl_free_h2c_pkt(phl_info, h2c_pkt_root);
		h2c_pkt_root = NULL;
	}

	return h2c_pkt_root;
}

static void _phl_free_h2c_pool(struct phl_info_t *phl_info)
{
	struct phl_h2c_pkt_pool *h2c_pkt_pool = NULL;
	void *drv_priv = phl_to_drvpriv(phl_info);

	FUNCIN();

	h2c_pkt_pool = phl_info->h2c_pool;
	if (NULL != h2c_pkt_pool) {
		h2c_pkt_pool->idle_h2c_pkt_cmd_list.cnt = 0;
		h2c_pkt_pool->idle_h2c_pkt_data_list.cnt = 0;
		h2c_pkt_pool->idle_h2c_pkt_ldata_list.cnt = 0;

		_phl_free_h2c_pkt(phl_info, h2c_pkt_pool->h2c_pkt_buf);
		h2c_pkt_pool->h2c_pkt_buf = NULL;
		_os_spinlock_free(drv_priv,
					&h2c_pkt_pool->idle_h2c_pkt_cmd_list.lock);
		_os_spinlock_free(drv_priv,
					&h2c_pkt_pool->idle_h2c_pkt_data_list.lock);
		_os_spinlock_free(drv_priv,
					&h2c_pkt_pool->idle_h2c_pkt_ldata_list.lock);
		_os_spinlock_free(drv_priv,
					&h2c_pkt_pool->busy_h2c_pkt_list.lock);
		_os_spinlock_free(drv_priv,
					&h2c_pkt_pool->recycle_lock);
		_os_mem_free(phl_to_drvpriv(phl_info), h2c_pkt_pool,
					sizeof(struct phl_h2c_pkt_pool));
	}
	FUNCOUT();
}

enum rtw_phl_status
_phl_alloc_h2c_pool(struct phl_info_t *phl_info)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	struct phl_h2c_pkt_pool *h2c_pkt_pool = NULL;
	struct rtw_h2c_pkt *h2c_pkt_buf = NULL;
	void *drv_priv = NULL;

	FUNCIN_WSTS(pstatus);
	drv_priv = phl_to_drvpriv(phl_info);

	h2c_pkt_pool = _os_mem_alloc(drv_priv, sizeof(struct phl_h2c_pkt_pool));
	if (NULL != h2c_pkt_pool) {

		INIT_LIST_HEAD(&h2c_pkt_pool->idle_h2c_pkt_cmd_list.queue);
		h2c_pkt_pool->idle_h2c_pkt_cmd_list.cnt = 0;

		INIT_LIST_HEAD(&h2c_pkt_pool->idle_h2c_pkt_data_list.queue);
		h2c_pkt_pool->idle_h2c_pkt_data_list.cnt = 0;

		INIT_LIST_HEAD(&h2c_pkt_pool->idle_h2c_pkt_ldata_list.queue);
		h2c_pkt_pool->idle_h2c_pkt_ldata_list.cnt = 0;

		INIT_LIST_HEAD(&h2c_pkt_pool->busy_h2c_pkt_list.queue);
		h2c_pkt_pool->busy_h2c_pkt_list.cnt = 0;

		_os_spinlock_init(drv_priv,
					&h2c_pkt_pool->idle_h2c_pkt_cmd_list.lock);
		_os_spinlock_init(drv_priv,
					&h2c_pkt_pool->idle_h2c_pkt_data_list.lock);
		_os_spinlock_init(drv_priv,
					&h2c_pkt_pool->idle_h2c_pkt_ldata_list.lock);
		_os_spinlock_init(drv_priv,
					&h2c_pkt_pool->busy_h2c_pkt_list.lock);
		_os_spinlock_init(drv_priv,
					&h2c_pkt_pool->recycle_lock);

		h2c_pkt_buf = _phl_alloc_h2c_pkt(phl_info, h2c_pkt_pool);

		if (NULL == h2c_pkt_buf) {
			_os_spinlock_free(drv_priv,
					&h2c_pkt_pool->idle_h2c_pkt_cmd_list.lock);
			_os_spinlock_free(drv_priv,
					&h2c_pkt_pool->idle_h2c_pkt_data_list.lock);
			_os_spinlock_free(drv_priv,
					&h2c_pkt_pool->idle_h2c_pkt_ldata_list.lock);
			_os_spinlock_free(drv_priv,
					&h2c_pkt_pool->busy_h2c_pkt_list.lock);
			_os_spinlock_free(drv_priv,
					&h2c_pkt_pool->recycle_lock);
			_os_mem_free(drv_priv, h2c_pkt_pool, sizeof(struct phl_h2c_pkt_pool));
			h2c_pkt_pool = NULL;
			pstatus = RTW_PHL_STATUS_RESOURCE;
		} else {
			h2c_pkt_pool->h2c_pkt_buf = h2c_pkt_buf;
			pstatus = RTW_PHL_STATUS_SUCCESS;
		}
	}

	if (RTW_PHL_STATUS_SUCCESS == pstatus)
		phl_info->h2c_pool = h2c_pkt_pool;

	FUNCOUT_WSTS(pstatus);

	return pstatus;
}

void
phl_trx_free_handler(void *phl)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct rtw_phl_handler *tx_handler = &phl_info->phl_tx_handler;
	struct rtw_phl_handler *rx_handler = &phl_info->phl_rx_handler;
	struct rtw_phl_handler *event_handler = &phl_info->phl_event_handler;

	FUNCIN();

	phl_deregister_handler(phl_info->phl_com, event_handler);
	phl_deregister_handler(phl_info->phl_com, rx_handler);
	phl_deregister_handler(phl_info->phl_com, tx_handler);

	FUNCOUT();
}

void
phl_trx_free_sw_rsc(void *phl)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_hci_trx_ops *hci_trx_ops = phl_info->hci_trx_ops;
	void *drv_priv = NULL;

	FUNCIN();

	drv_priv = phl_to_drvpriv(phl_info);

	_phl_free_h2c_pool(phl_info);

	hci_trx_ops->hci_trx_deinit(phl_info);

	phl_rx_deinit(phl_info);

	_phl_ring_status_deinit(phl_info);

	_os_spinlock_free(drv_priv, &phl_info->t_ring_list_lock);
	_os_spinlock_free(drv_priv, &phl_info->rx_ring_lock);
	_os_spinlock_free(drv_priv, &phl_info->t_fctrl_result_lock);
	_os_spinlock_free(drv_priv, &phl_info->t_ring_free_list_lock);

	FUNCOUT();
}

enum rtw_phl_status phl_datapath_start_hw(struct phl_info_t *phl_info)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	struct phl_hci_trx_ops *hci_trx_ops = phl_info->hci_trx_ops;

	do {
		pstatus = hci_trx_ops->trx_cfg(phl_info);
		if (RTW_PHL_STATUS_SUCCESS != pstatus)
			break;
		rtw_hal_notification(phl_info->hal, MSG_EVT_DATA_PATH_START, HW_BAND_MAX);
	} while (false);

	return pstatus;
}

void phl_datapath_stop_hw(struct phl_info_t *phl_info)
{
	struct phl_hci_trx_ops *hci_trx_ops = phl_info->hci_trx_ops;

	hci_trx_ops->trx_stop(phl_info);
	rtw_hal_notification(phl_info->hal, MSG_EVT_DATA_PATH_STOP, HW_BAND_MAX);
}

void phl_datapath_reset(struct phl_info_t *phl_info, u8 type)
{
	struct phl_hci_trx_ops *hci_trx_ops = phl_info->hci_trx_ops;

	hci_trx_ops->trx_reset(phl_info, type);
}

enum rtw_phl_status phl_datapath_start_sw(struct phl_info_t *phl_info, enum phl_module_id id)
{
	struct phl_data_ctl_t ctl = {0};

	ctl.id = id;
	ctl.cmd = PHL_DATA_CTL_SW_TX_RESUME;
	if (phl_data_ctrler(phl_info, &ctl, NULL) != RTW_PHL_STATUS_SUCCESS)
		PHL_WARN("%s: tx resume fail!\n", __func__);
	ctl.cmd = PHL_DATA_CTL_SW_RX_RESUME;
	if (phl_data_ctrler(phl_info, &ctl, NULL) != RTW_PHL_STATUS_SUCCESS)
		PHL_WARN("%s: rx resume fail!\n", __func__);

	return RTW_PHL_STATUS_SUCCESS;
}

void phl_datapath_stop_sw(struct phl_info_t *phl_info, enum phl_module_id id)
{
	struct phl_data_ctl_t ctl = {0};

	ctl.id = id;
	ctl.cmd = PHL_DATA_CTL_SW_TX_PAUSE;
	if (phl_data_ctrler(phl_info, &ctl, NULL) != RTW_PHL_STATUS_SUCCESS)
		PHL_WARN("%s: tx pause fail!\n", __func__);
	ctl.cmd = PHL_DATA_CTL_SW_RX_PAUSE;
	if (phl_data_ctrler(phl_info, &ctl, NULL) != RTW_PHL_STATUS_SUCCESS)
		PHL_WARN("%s: rx pause fail!\n", __func__);
}


void phl_datapath_deinit(struct phl_info_t *phl_info)
{
	phl_trx_free_handler(phl_info);
	phl_trx_free_sw_rsc(phl_info);
}

enum rtw_phl_status phl_datapath_init(struct phl_info_t *phl_info)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	struct phl_hci_trx_ops *hci_trx_ops = phl_info->hci_trx_ops;
	struct rtw_phl_handler *event_handler = &phl_info->phl_event_handler;
	void *drv_priv = NULL;
	FUNCIN_WSTS(pstatus);
	drv_priv = phl_to_drvpriv(phl_info);

	do {
		INIT_LIST_HEAD(&phl_info->t_ring_list);
		INIT_LIST_HEAD(&phl_info->t_fctrl_result);
		INIT_LIST_HEAD(&phl_info->t_ring_free_list);
		_os_spinlock_init(drv_priv, &phl_info->t_ring_list_lock);
		_os_spinlock_init(drv_priv, &phl_info->rx_ring_lock);
		_os_spinlock_init(drv_priv, &phl_info->t_fctrl_result_lock);
		_os_spinlock_init(drv_priv, &phl_info->t_ring_free_list_lock);

		event_handler->type = RTW_PHL_HANDLER_PRIO_HIGH;
		event_handler->callback = phl_event_indicator;
		event_handler->context = phl_info;
		event_handler->drv_priv = drv_priv;
		event_handler->status = 0;

		#ifdef CONFIG_RTW_OS_HANDLER_EXT
		event_handler->id = RTW_PHL_EVT_HANDLER;
		#endif /* CONFIG_RTW_OS_HANDLER_EXT */

		pstatus = phl_register_handler(phl_info->phl_com, event_handler);
		if (RTW_PHL_STATUS_SUCCESS != pstatus)
			break;

		pstatus = _phl_ring_status_init(phl_info);
		if (RTW_PHL_STATUS_SUCCESS != pstatus)
			break;

		pstatus = phl_rx_init(phl_info);
		if (RTW_PHL_STATUS_SUCCESS != pstatus)
			break;

		pstatus = hci_trx_ops->hci_trx_init(phl_info);
		if (RTW_PHL_STATUS_SUCCESS != pstatus)
			break;

		/* allocate h2c pkt */
		pstatus = _phl_alloc_h2c_pool(phl_info);
		if (RTW_PHL_STATUS_SUCCESS != pstatus)
			break;

	}while (false);

	if (RTW_PHL_STATUS_SUCCESS != pstatus)
		phl_datapath_deinit(phl_info);

	FUNCOUT_WSTS(pstatus);

	return pstatus;
}

static enum rtw_phl_status
_phl_tx_pwr_notify(void *phl)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_SUCCESS;

#ifdef SDIO_TX_THREAD
	phl_tx_sdio_wake_thrd((struct phl_info_t *)phl);
#else
	pstatus = rtw_phl_tx_req_notify(phl);
#endif

	return pstatus;
}
#ifdef CONFIG_POWER_SAVE
static void _phl_req_pwr_cb(void *priv, struct phl_msg *msg)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)priv;

	if (MSG_MDL_ID_FIELD(msg->msg_id) == PHL_MDL_TX)
		_os_atomic_set(phl_to_drvpriv(phl_info),
		               &phl_info->phl_sw_tx_req_pwr,
		               0);
	else
		_os_atomic_set(phl_to_drvpriv(phl_info),
		               &phl_info->phl_sw_rx_req_pwr,
		               0);

	if (IS_MSG_FAIL(msg->msg_id) || IS_MSG_CANCEL(msg->msg_id)) {
		PHL_WARN("%s(): request power failure.\n", __func__);
		return;
	}

	if (MSG_MDL_ID_FIELD(msg->msg_id) == PHL_MDL_TX)
		_phl_tx_pwr_notify(priv);
	else if (MSG_MDL_ID_FIELD(msg->msg_id) == PHL_MDL_RX)
		rtw_phl_start_rx_process(priv);
}

static void _phl_datapath_req_pwr(struct phl_info_t *phl_info, u8 type)
{
	enum rtw_phl_status psts = RTW_PHL_STATUS_FAILURE;
	struct phl_msg msg = {0};
	struct phl_msg_attribute attr = {0};

	PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_,
	          "%s(): [DATA_CTRL] SW datapath paused by ps module and request power\n",
	          __func__);

	SET_MSG_MDL_ID_FIELD(msg.msg_id, ((type == PHL_CTRL_TX) ? PHL_MDL_TX : PHL_MDL_RX));
	SET_MSG_EVT_ID_FIELD(msg.msg_id, MSG_EVT_TRX_PWR_REQ);

	attr.completion.completion = _phl_req_pwr_cb;
	attr.completion.priv = phl_info;

	/* shall set req_pwr flag first before sending req_pwr msg */
	if (PHL_CTRL_TX == type)
		_os_atomic_set(phl_to_drvpriv(phl_info),
		               &phl_info->phl_sw_tx_req_pwr,
		               1);
	else
		_os_atomic_set(phl_to_drvpriv(phl_info),
		               &phl_info->phl_sw_rx_req_pwr,
		               1);

	psts = phl_disp_eng_send_msg(phl_info, &msg, &attr, NULL);

	if (RTW_PHL_STATUS_SUCCESS != psts) {
		PHL_WARN("%s(): CANNOT send msg to request power.\n", __func__);
		if (PHL_CTRL_TX == type)
			_os_atomic_set(phl_to_drvpriv(phl_info),
			               &phl_info->phl_sw_tx_req_pwr,
			               0);
		else
			_os_atomic_set(phl_to_drvpriv(phl_info),
			               &phl_info->phl_sw_rx_req_pwr,
			               0);
	}
}

static bool _phl_datapath_chk_pwr(struct phl_info_t *phl_info, u8 type)
{
	void *drvpriv = phl_to_drvpriv(phl_info);
	enum data_ctrl_mdl pause_id = 0;
	_os_atomic *trx_more;
	_os_atomic *req_pwr;
	struct rtw_ps_cap_t *ps_cap = &phl_info->phl_com->dev_cap.ps_cap;

	if (type == PHL_CTRL_TX) {
		pause_id = phl_info->pause_tx_id;
		trx_more = &phl_info->phl_sw_tx_more;
		req_pwr = &phl_info->phl_sw_tx_req_pwr;
	} else {
		pause_id = phl_info->pause_rx_id;
		trx_more = &phl_info->phl_sw_rx_more;
		req_pwr = &phl_info->phl_sw_rx_req_pwr;
	}

	if (pause_id & ~(DATA_CTRL_MDL_PS)) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_,
		          "%s(): [DATA_CTRL] SW datapath paused by module(0x%x)\n",
		          __func__,
		          pause_id);
		return false;
	}

	/**
	 * For some ICs, we may pause datapath during the configuration flow of
	 * entering/leaving low power state. In this case, the pause_id will also be
	 * "DATA_CTRL_MDL_PS" but we don't need to send msg "MSG_EVT_TRX_PWR_REQ"
	 * to leave low power state for this situation. Thus we check the capability
	 * "lps_pause_tx" before sending msg "MSG_EVT_TRX_PWR_REQ".
	 */
	if (!ps_cap->lps_pause_tx)
		return false;

	/* only paused by ps module */
	if (true == _os_atomic_read(drvpriv, trx_more) &&
	    false == _os_atomic_read(drvpriv, req_pwr))
		_phl_datapath_req_pwr(phl_info, type);

	return true;
}
#endif
bool phl_datapath_chk_trx_pause(struct phl_info_t *phl_info, u8 type)
{
	void *drvpriv = phl_to_drvpriv(phl_info);
	_os_atomic *sw_sts;

	if (type == PHL_CTRL_TX)
		sw_sts = &phl_info->phl_sw_tx_sts;
	else
		sw_sts = &phl_info->phl_sw_rx_sts;

	if (PHL_TX_STATUS_SW_PAUSE == _os_atomic_read(drvpriv, sw_sts)) {
#ifdef CONFIG_POWER_SAVE
		_phl_datapath_chk_pwr(phl_info, type);
#endif
		return true;
	}

	return false;
}

void rtw_phl_tx_stop(void *phl)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_hci_trx_ops *hci_trx_ops = phl_info->hci_trx_ops;

	/* Pause SW Tx */
	hci_trx_ops->req_tx_stop(phl_info);
}

void rtw_phl_tx_resume(void *phl)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_hci_trx_ops *hci_trx_ops = phl_info->hci_trx_ops;

	/* Resume SW Tx */
	hci_trx_ops->trx_resume(phl_info, PHL_CTRL_TX);
}


enum rtw_phl_status rtw_phl_tx_req_notify(void *phl)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;

	pstatus = phl_schedule_handler(phl_info->phl_com,
					&phl_info->phl_tx_handler);

	return pstatus;
}

struct rtw_phl_tx_ring *
_phl_select_tx_ring(struct phl_info_t *phl_info, struct rtw_xmit_req *tx_req)
{
	struct rtw_phl_tring_list *tring_list, *t;
	_os_list *list_head = &phl_info->t_ring_list;
	struct rtw_phl_tx_ring *ring = NULL;
	u16 macid = tx_req->mdata.macid;

	if (phl_info->use_onetxring) {
		/*just pick the first entry*/
		tring_list = list_first_entry(list_head, struct rtw_phl_tring_list, list);
		ring = &tring_list->phl_ring[tx_req->mdata.cat];
	} else {
		phl_list_for_loop_safe(tring_list, t, struct rtw_phl_tring_list,
					list_head, list) {
			if (macid != tring_list->macid) {
				continue;
			} else {
				ring = &tring_list->phl_ring[tx_req->mdata.cat];
				break;
			}
		}
	}
	return ring;
}

enum rtw_phl_status rtw_phl_add_tx_req(void *phl,
				struct rtw_xmit_req *tx_req)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	struct rtw_phl_tx_ring *ring = NULL;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	void *drv_priv = NULL;
	u16 ring_res = 0, rptr = 0;

	drv_priv = phl_to_drvpriv(phl_info);

	_os_spinlock(drv_priv, &phl_info->t_ring_list_lock, _bh, NULL);
	ring = 	_phl_select_tx_ring(phl_info, tx_req);

	if (NULL != ring) {
		rptr = (u16)_os_atomic_read(drv_priv, &ring->phl_idx);

		ring_res = phl_calc_avail_wptr(rptr, ring->core_idx,
						MAX_PHL_TX_RING_ENTRY_NUM);
		if (ring_res > 0) {
			ring->core_idx =
				(ring->core_idx + 1) % MAX_PHL_TX_RING_ENTRY_NUM;
			ring->entry[ring->core_idx] = (u8 *)tx_req;
			phl_tx_statistics(phl_info, tx_req);
#ifdef CONFIG_PHL_TX_DBG
			if (tx_req->tx_dbg.en_dbg) {
				tx_req->tx_dbg.core_add_tx_t =
						_os_get_cur_time_us();
			}
#endif /* CONFIG_PHL_TX_DBG */
			_os_atomic_set(drv_priv, &phl_info->phl_sw_tx_more, 1);
			pstatus = RTW_PHL_STATUS_SUCCESS;
		} else {
			PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "no ring resource to add new tx request!\n");
			pstatus = RTW_PHL_STATUS_RESOURCE;
		}
	}

	_os_spinunlock(drv_priv, &phl_info->t_ring_list_lock, _bh, NULL);

	return pstatus;
}

u16 rtw_phl_tring_rsc(void *phl, u16 macid, u8 tid)
{
	struct rtw_phl_tring_list *tring_list, *t;
	struct rtw_phl_tx_ring *ring = NULL;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	void *drv_priv = NULL;
	_os_list *list_head = &phl_info->t_ring_list;
	u16 ring_res = 0, rptr = 0;

	drv_priv = phl_to_drvpriv(phl_info);

	phl_list_for_loop_safe(tring_list, t, struct rtw_phl_tring_list,
				list_head, list) {
		if (macid != tring_list->macid) {
			continue;
		} else {
			/* hana_todo check mgnt frame case */
			ring = &tring_list->phl_ring[tid];
			break;
		}
	}

	if (NULL != ring) {
		rptr = (u16)_os_atomic_read(drv_priv, &ring->phl_idx);

		ring_res = phl_calc_avail_rptr(rptr, ring->core_idx,
						MAX_PHL_TX_RING_ENTRY_NUM);

	}

	return ring_res;
}


enum rtw_phl_status phl_indic_pkt_complete(void *phl)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct rtw_evt_info_t *evt_info = &phl_info->phl_com->evt_info;
	void *drv_priv = phl_to_drvpriv(phl_info);

	do {
		_os_spinlock(drv_priv, &evt_info->evt_lock, _bh, NULL);
		evt_info->evt_bitmap |= RTW_PHL_EVT_TX_RECYCLE;
		_os_spinunlock(drv_priv, &evt_info->evt_lock, _bh, NULL);

		pstatus = phl_schedule_handler(phl_info->phl_com,
							&phl_info->phl_event_handler);
	} while (false);

	return pstatus;
}

enum rtw_phl_status rtw_phl_recycle_tx_buf(void *phl, u8 *tx_buf_ptr)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
#ifdef CONFIG_USB_HCI
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_hci_trx_ops *hci_trx_ops = phl_info->hci_trx_ops;

	pstatus = hci_trx_ops->recycle_tx_buf(phl, tx_buf_ptr);

#endif
	return pstatus;
}


static enum rtw_phl_status
_phl_cfg_tx_ampdu(void *phl, struct rtw_phl_stainfo_t *sta)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	enum rtw_hal_status hsts = RTW_HAL_STATUS_FAILURE;

	hsts = rtw_hal_upd_ampdu_cctrl_info(phl_info->hal, sta);
	if (RTW_HAL_STATUS_SUCCESS != hsts)
		goto fail;

	hsts = rtw_hal_cfg_tx_ampdu(phl_info->hal, sta);
	if (RTW_HAL_STATUS_SUCCESS != hsts)
		goto fail;

	return RTW_PHL_STATUS_SUCCESS;

fail:
	return RTW_PHL_STATUS_FAILURE;
}

#ifdef CONFIG_CMD_DISP
struct cmd_amsdu_param {
	u8 macid;
	u8 enable;
	u8 amsdu_max_len;
	u8 qos_field_h;
	u8 qos_field_h_en;
	u8 mhdr_len;
	u8 vlan_tag_valid;
};


struct cmd_hwseq_param {
	u16 macid;
	u8 ref_sel;
	u8 ssn_sel;
	u16 *hw_seq;
};

enum rtw_phl_status
phl_cmd_cfg_ampdu_hdl(struct phl_info_t *phl_info, u8 *param)
{
	struct rtw_phl_stainfo_t *sta = (struct rtw_phl_stainfo_t *)param;

	PHL_INFO(" %s(), sta = %p !\n", __func__, sta);

	return _phl_cfg_tx_ampdu(phl_info, sta);
}

static enum rtw_phl_status
_phl_cfg_tx_amsdu(void *phl, struct cmd_amsdu_param *pamsdu_param)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	enum rtw_hal_status hsts = RTW_HAL_STATUS_FAILURE;

	hsts = rtw_hal_cfg_tx_amsdu(phl_info->hal,
				    pamsdu_param->macid,
				    pamsdu_param->enable,
				    pamsdu_param->amsdu_max_len,
				    pamsdu_param->qos_field_h,
				    pamsdu_param->qos_field_h_en,
				    pamsdu_param->mhdr_len,
				    pamsdu_param->vlan_tag_valid);
	if (RTW_HAL_STATUS_SUCCESS != hsts)
		goto fail;

	return RTW_PHL_STATUS_SUCCESS;

fail:
	return RTW_PHL_STATUS_FAILURE;
}

enum rtw_phl_status
phl_cmd_cfg_amsdu_tx_hdl(struct phl_info_t *phl_info, u8 *param)
{
	struct cmd_amsdu_param *pamsdu_param = (struct cmd_amsdu_param *)param;

	PHL_INFO(" %s(), pamsdu_param = %p !\n", __func__, pamsdu_param);

	return _phl_cfg_tx_amsdu(phl_info, pamsdu_param);
}

static void _phl_cfg_amsdu_tx_done(void *drv_priv, u8 *cmd, u32 cmd_len,
					enum rtw_phl_status status)
{
	if (cmd) {
		_os_kmem_free(drv_priv, cmd, cmd_len);
		cmd = NULL;
		PHL_INFO("%s.....\n", __func__);
	}
}



enum rtw_phl_status
phl_cmd_get_hwseq_hdl(struct phl_info_t *phl_info, u8 *param)
{
	struct cmd_hwseq_param *hwseq_param = (struct cmd_hwseq_param *)param;
	enum rtw_hal_status hsts = RTW_HAL_STATUS_FAILURE;

	hsts = rtw_hal_get_hwseq(phl_info->hal, hwseq_param->macid, hwseq_param->ref_sel,
					hwseq_param->ssn_sel,
					hwseq_param->hw_seq);

	if (RTW_HAL_STATUS_SUCCESS != hsts)
		goto fail;

	return RTW_PHL_STATUS_SUCCESS;

fail:
	return RTW_PHL_STATUS_FAILURE;
}


static void _phl_get_hw_sequence_done(void *drv_priv, u8 *cmd, u32 cmd_len,
					enum rtw_phl_status status)
{
	if (cmd) {
		_os_kmem_free(drv_priv, cmd, cmd_len);
		cmd = NULL;
		PHL_INFO("%s.....\n", __func__);
	}
}

#endif

/**
 * rtw_phl_get_hw_sequence - get hw sequence form CR or DMAC table
 * @phl: see struct phl_info_t
 * @cur_hw_seq: hw sequence value
 * @updated: if hw sequnece successfully updated, let caller know
 * @hw_ssn_mode: ssn mode;
 *                 HW_SSN_MODE_CR : 0
 *                 HW_SSN_MODE_DMAC : 1
 * @hw_ssn_sel: ssn select; currently each CR set or DMAC entry include 4 entry
 *                 HW_SSN_SEL0 : 0
 *                 HW_SSN_SEL1 : 1
 *                 HW_SSN_SEL2 : 2
 *                 HW_SSN_SEL3 : 3
 * returns enum RTW_PHL_STATUS
 */
enum rtw_phl_status
rtw_phl_get_hw_sequence(void *phl,
			u16 *cur_hw_seq,
			u16 macid,
			u8 hw_ssn_mode,
			u8 hw_ssn_sel)
{
	enum rtw_phl_status sts = RTW_PHL_STATUS_FAILURE;

#ifdef CONFIG_CMD_DISP
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	void *drv = phl_to_drvpriv(phl_info);
	u32 param_len = sizeof(struct cmd_hwseq_param);
	struct cmd_hwseq_param *param = _os_kmem_alloc(drv, param_len);

	if (NULL == param) {
		/* alloc fail */
		PHL_ERR("%s : param alloc fail !! \n", __func__);
		return RTW_PHL_STATUS_FAILURE;
	}

	param->macid = macid;
	param->ref_sel = hw_ssn_mode;
	param->ssn_sel = hw_ssn_sel;
	param->hw_seq = cur_hw_seq;

	sts = phl_cmd_enqueue(phl_info,
			HW_BAND_0,
			MSG_EVT_HWSEQ_GET_HW_SEQUENCE,
			(u8 *)param,
			param_len,
			_phl_get_hw_sequence_done,
			PHL_CMD_WAIT,
			0);

	if (is_cmd_failure(sts)) {
		/* Send cmd success, but wait cmd fail*/
		PHL_ERR("%s : command send success but exec fail, sts = %d\n", __func__, sts);
		sts = RTW_PHL_STATUS_FAILURE;
	} else if (sts != RTW_PHL_STATUS_SUCCESS) {
		/* Send cmd fail */
		PHL_ERR("%s : send command fail, sts = %d\n", __func__, sts);
		_os_kmem_free(phl_to_drvpriv(phl_info), param, param_len);
		sts = RTW_PHL_STATUS_CMD_DROP;
	}

#else
	PHL_ERR("%s : CONFIG_CMD_DISP need to be enabled for MSG_EVT_HWSEQ_GET_HW_SEQUENCE !! \n", __func__);
#endif

	return sts;
}

enum rtw_phl_status
rtw_phl_cmd_cfg_amsdu_tx(struct rtw_wifi_role_t *wrole,
	struct rtw_phl_stainfo_t *sta,
	struct rtw_amsdu_tx_param *input_param,
	bool cmd_wait,
	u32 cmd_timeout)
{
	enum rtw_phl_status sts = RTW_PHL_STATUS_FAILURE;

#ifdef CONFIG_CMD_DISP

	struct phl_info_t *phl = wrole->phl_com->phl_priv;
	void *drv = phl_to_drvpriv(phl);
	u32 param_len = sizeof(struct cmd_amsdu_param);
	struct cmd_amsdu_param *param = _os_kmem_alloc(drv, param_len);

	if (param == NULL) {
		PHL_ERR("%s: alloc param failed!\n", __func__);
		return sts;
	}

	param->macid = input_param->macid;
	param->enable = input_param->enable;
	param->amsdu_max_len = input_param->amsdu_max_len;
	param->qos_field_h = input_param->qos_field_h;
	param->qos_field_h_en = input_param->qos_field_h_en;
	param->mhdr_len = input_param->mhdr_len;
	param->vlan_tag_valid = input_param->vlan_tag_valid;

	sts = phl_cmd_enqueue(phl,
	                       sta->rlink->hw_band,
	                       MSG_EVT_CFG_AMSDU_TX,
	                       (u8 *)param,
	                       param_len,
	                       _phl_cfg_amsdu_tx_done,
	                       PHL_CMD_NO_WAIT,
	                       0);

	if (is_cmd_failure(sts)) {
		/* Send cmd success, but wait cmd fail*/
		PHL_ERR("%s : command send success but exec fail, sts = %d\n", __func__, sts);
		sts = RTW_PHL_STATUS_FAILURE;
	} else if (sts != RTW_PHL_STATUS_SUCCESS) {
		/* Send cmd fail */
		PHL_ERR("%s : send command fail, sts = %d\n", __func__, sts);
		_os_kmem_free(drv, param, param_len);
		sts = RTW_PHL_STATUS_CMD_DROP;
	}
#else
	PHL_ERR("%s : CONFIG_CMD_DISP need to be enabled for MSG_EVT_CFG_AMSDU_TX !! \n", __func__);
#endif

	return sts;
}


enum rtw_phl_status
rtw_phl_cmd_cfg_ampdu(void *phl,
			struct rtw_wifi_role_t *wrole,
			struct rtw_phl_stainfo_t *sta,
			enum phl_cmd_type cmd_type,
			u32 cmd_timeout)
{
	enum rtw_phl_status sts = RTW_PHL_STATUS_FAILURE;
#ifdef CONFIG_CMD_DISP
	sts = phl_cmd_enqueue(phl,
	                      sta->rlink->hw_band,
	                      MSG_EVT_CFG_AMPDU,
	                      (u8 *)sta,
	                      0,
	                      NULL,
	                      cmd_type, cmd_timeout);

	if (is_cmd_failure(sts)) {
		/* Send cmd success, but wait cmd fail*/
		sts = RTW_PHL_STATUS_FAILURE;
	} else if (sts != RTW_PHL_STATUS_SUCCESS) {
		/* Send cmd fail */
		sts = RTW_PHL_STATUS_FAILURE;
	}

	return sts;
#else
	PHL_ERR("%s : CONFIG_CMD_DISP need to be enabled for MSG_EVT_CFG_AMPDU !! \n", __func__);

	return sts;
#endif
}


void rtw_phl_cfg_hwamsdu_init(void *phl,
	u8 hwamsdu_en,
	u8 pkt_num,
	u8 single_amsdu_en,
	u8 last_padding_en)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;

	rtw_hal_enable_hwamsdu(phl_info->hal, hwamsdu_en, pkt_num, single_amsdu_en, last_padding_en);
	PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "rtw_phl_cfg_hwamsdu_init(): init ok\n");
}


void
phl_tx_watchdog(struct phl_info_t *phl_info)
{
	struct phl_hci_trx_ops *trx_ops = phl_info->hci_trx_ops;
	struct rtw_stats *phl_stats = &phl_info->phl_com->phl_stats;

	phl_tx_traffic_upd(phl_stats);
	trx_ops->tx_watchdog(phl_info);
}


u8 rtw_phl_cvt_cat_to_tid(enum rtw_phl_ring_cat cat)
{
	u8 tid = 0;

	switch (cat) {
	case RTW_PHL_RING_CAT_TID0:
		tid = 0;
		break;
	case RTW_PHL_RING_CAT_TID1:
		tid = 1;
		break;
	case RTW_PHL_RING_CAT_TID2:
		tid = 2;
		break;
	case RTW_PHL_RING_CAT_TID3:
		tid = 3;
		break;
	case RTW_PHL_RING_CAT_TID4:
		tid = 4;
		break;
	case RTW_PHL_RING_CAT_TID5:
		tid = 5;
		break;
	case RTW_PHL_RING_CAT_TID6:
		tid = 6;
		break;
	case RTW_PHL_RING_CAT_TID7:
		tid = 7;
		break;
	case RTW_PHL_RING_CAT_MGNT:
	case RTW_PHL_RING_CAT_HIQ:
		tid = 0;
		break;
	default:
		PHL_WARN("%s: this category(%d) is invalid to convert to tid\n",
		         __func__, cat);
		break;
	}

	return tid;
}

/* note: this fucntion shall only be used on data frame  */
enum rtw_phl_ring_cat
rtw_phl_cvt_tid_to_cat(u8 tid)
{
	enum rtw_phl_ring_cat cat = RTW_PHL_RING_CAT_TID0;

	switch (tid) {
	case 0:
		cat = RTW_PHL_RING_CAT_TID0;
		break;
	case 1:
		cat = RTW_PHL_RING_CAT_TID1;
		break;
	case 2:
		cat = RTW_PHL_RING_CAT_TID2;
		break;
	case 3:
		cat = RTW_PHL_RING_CAT_TID3;
		break;
	case 4:
		cat = RTW_PHL_RING_CAT_TID4;
		break;
	case 5:
		cat = RTW_PHL_RING_CAT_TID5;
		break;
	case 6:
		cat = RTW_PHL_RING_CAT_TID6;
		break;
	case 7:
		cat = RTW_PHL_RING_CAT_TID7;
		break;
	default:
		PHL_WARN("%s: this tid(%d) is invalid to convert to category\n",
		         __func__, tid);
		break;
	}

	return cat;
}

enum data_ctrl_mdl _phl_get_ctrl_mdl(enum phl_module_id id)
{
	enum data_ctrl_mdl ctrl_mdl = DATA_CTRL_MDL_NONE;

	switch (id) {
	case PHL_MDL_PHY_MGNT:
		ctrl_mdl = DATA_CTRL_MDL_CMD_CTRLER;
		break;
	case PHL_MDL_SER:
		ctrl_mdl = DATA_CTRL_MDL_SER;
		break;
	case PHL_MDL_POWER_MGNT:
		ctrl_mdl = DATA_CTRL_MDL_PS;
		break;
	case PHL_MDL_MRC:
		ctrl_mdl = DATA_CTRL_MDL_MRC;
		break;
	default:
		PHL_WARN("Unknown PHL module(%d) try to control datapath and is skipped!\n",
			 id);
		ctrl_mdl = DATA_CTRL_MDL_NONE;
		break;
	}

	return ctrl_mdl;
}


enum rtw_phl_status
_phl_poll_hw_tx_done(void)
{
	PHL_TRACE(COMP_PHL_DBG, _PHL_ERR_, "[DATA_CTRL] Polling hw tx done is not supported now\n");

	return RTW_PHL_STATUS_FAILURE;
}

enum rtw_phl_status
_phl_hw_tx_resume(void)
{
	PHL_TRACE(COMP_PHL_DBG, _PHL_ERR_, "[DATA_CTRL] Resume hw tx not is supported now\n");

	return RTW_PHL_STATUS_FAILURE;
}

enum rtw_phl_status
_phl_sw_tx_resume(struct phl_info_t *phl_info, struct phl_data_ctl_t *ctl)
{
	enum rtw_phl_status sts = RTW_PHL_STATUS_FAILURE;
	struct phl_hci_trx_ops *ops = phl_info->hci_trx_ops;
	enum data_ctrl_mdl ctrl_mdl = _phl_get_ctrl_mdl(ctl->id);

	if (PHL_TX_STATUS_IDLE ==
	    _os_atomic_read(phl_to_drvpriv(phl_info), &phl_info->phl_sw_tx_sts)) {
	    ops->trx_resume(phl_info, PHL_CTRL_TX);
	    return RTW_PHL_STATUS_SUCCESS;
	}

	if (!TEST_STATUS_FLAG(phl_info->pause_tx_id, ctrl_mdl)) {
		PHL_WARN("[DATA_CTRL] module %d do not pause tx previously (pause_tx_id 0x%x)\n",
		         ctl->id, phl_info->pause_tx_id);
		return sts;
	}

	CLEAR_STATUS_FLAG(phl_info->pause_tx_id, ctrl_mdl);

	if (DATA_CTRL_MDL_NONE != phl_info->pause_tx_id) {
		PHL_WARN("[DATA_CTRL] sw tx is still paused by tx pause id = 0x%x\n",
		         phl_info->pause_tx_id);

		sts = RTW_PHL_STATUS_SUCCESS;
	} else {
		ops->trx_resume(phl_info, PHL_CTRL_TX);

		sts = rtw_phl_tx_req_notify(phl_info);
	}

	return sts;
}

void
_phl_sw_tx_rst(struct phl_info_t *phl_info)
{
	struct phl_hci_trx_ops *ops = phl_info->hci_trx_ops;

	ops->trx_reset(phl_info, PHL_CTRL_TX);
}

enum rtw_phl_status
_phl_sw_tx_pause(struct phl_info_t *phl_info,
                 struct phl_data_ctl_t *ctl,
                 bool rst_sw)
{
	enum rtw_phl_status sts = RTW_PHL_STATUS_FAILURE;
	struct phl_hci_trx_ops *ops = phl_info->hci_trx_ops;
	void *drv = phl_to_drvpriv(phl_info);
	u32 i = 0;
	enum data_ctrl_mdl ctrl_mdl = _phl_get_ctrl_mdl(ctl->id);
#ifdef DBG_DBCC_MONITOR_TIME
	u32 start_t = 0;

	phl_fun_monitor_start(&start_t, true, __FUNCTION__);
#endif /* DBG_DBCC_MONITOR_TIME */
	if (PHL_TX_STATUS_SW_PAUSE ==
	    _os_atomic_read(drv, &phl_info->phl_sw_tx_sts)) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_,
		          "[DATA_CTRL] SW tx has been paused by module(0x%x)\n",
		          phl_info->pause_tx_id);

		SET_STATUS_FLAG(phl_info->pause_tx_id, ctrl_mdl);

		PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_,
		          "[DATA_CTRL] Update pause sw tx id(0x%x) by module(%d)\n",
		          phl_info->pause_tx_id, ctl->id);

		sts = RTW_PHL_STATUS_SUCCESS;
		goto exit;
	}

	if (PHL_TX_STATUS_STOP_INPROGRESS ==
	    _os_atomic_read(drv, &phl_info->phl_sw_tx_sts)) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_,
		          "[DATA_CTRL] SW tx has been requested to pause by module(0x%x)\n",
		          phl_info->pause_tx_id);

		SET_STATUS_FLAG(phl_info->pause_tx_id, ctrl_mdl);

		PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_,
		          "[DATA_CTRL] Update pause sw tx id(0x%x) by module(%d)\n",
		          phl_info->pause_tx_id, ctl->id);

		sts = RTW_PHL_STATUS_SUCCESS;
		goto exit;
	}

	/* requset sw tx to stop */
	ops->req_tx_stop(phl_info);

	/*
	 * notify sw tx one last time
	 * and poll if it receviced the stop request and paused itself
	 */
	if (RTW_PHL_STATUS_SUCCESS == rtw_phl_tx_req_notify(phl_info)) {
		u32 start = _os_get_cur_time_ms();
		u32 cnt = (POLL_SW_TX_PAUSE_MAX_MS * 1000) / 50;

		for (i = 0; (i < cnt) &&
		     (phl_get_passing_time_ms(start) < POLL_SW_TX_PAUSE_MAX_MS);
		     i++) {
			if (true == ops->is_tx_pause(phl_info)) {
				SET_STATUS_FLAG(phl_info->pause_tx_id, ctrl_mdl);
				sts = RTW_PHL_STATUS_SUCCESS;
				break;
			}
			_os_sleep_us(drv, 50);
		}

		if (RTW_PHL_STATUS_SUCCESS != sts) {
			SET_STATUS_FLAG(phl_info->pause_tx_id, ctrl_mdl);
			sts = RTW_PHL_STATUS_CMD_TIMEOUT;
			PHL_TRACE(COMP_PHL_DBG, _PHL_ERR_,
			          "[DATA_CTRL] Module(%d) polling sw tx pause timeout (%d ms)!\n",
			          ctl->id, POLL_SW_TX_PAUSE_MAX_MS);
		} else {
			if (true == rst_sw) {
				PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_,
				          "[DATA_CTRL] Pause Tx with reset is not supported now! requested by module(%d)\n",
				          ctl->id);
			}
		}
	} else {
		PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_, "[DATA_CTRL] Schedule sw tx process fail!\n");
	}
exit:
#ifdef DBG_DBCC_MONITOR_TIME
	phl_fun_monitor_end(&start_t, __FUNCTION__);
#endif /* DBG_DBCC_MONITOR_TIME */
	return sts;
}

enum rtw_phl_status
_phl_poll_hw_rx_done(void)
{
	PHL_TRACE(COMP_PHL_DBG, _PHL_ERR_, "[DATA_CTRL] Polling hw rx done is not supported now\n");

	return RTW_PHL_STATUS_FAILURE;
}

enum rtw_phl_status
_phl_hw_rx_resume(void)
{
	PHL_TRACE(COMP_PHL_DBG, _PHL_ERR_, "[DATA_CTRL] Resume hw rx not is supported now\n");

	return RTW_PHL_STATUS_FAILURE;
}

enum rtw_phl_status
_phl_sw_rx_resume(struct phl_info_t *phl_info, struct phl_data_ctl_t *ctl)
{
	enum rtw_phl_status sts = RTW_PHL_STATUS_FAILURE;
	struct phl_hci_trx_ops *ops = phl_info->hci_trx_ops;
	enum data_ctrl_mdl ctrl_mdl = _phl_get_ctrl_mdl(ctl->id);

	if (PHL_RX_STATUS_IDLE ==
	    _os_atomic_read(phl_to_drvpriv(phl_info), &phl_info->phl_sw_rx_sts)) {
	    ops->trx_resume(phl_info, PHL_CTRL_RX);
	    return RTW_PHL_STATUS_SUCCESS;
	}

	if (!TEST_STATUS_FLAG(phl_info->pause_rx_id, ctrl_mdl)) {
		PHL_WARN("[DATA_CTRL] module %d do not pause rx previously (pause_rx_id 0x%x)\n",
		         ctl->id, phl_info->pause_rx_id);
		return sts;
	}

	CLEAR_STATUS_FLAG(phl_info->pause_rx_id, ctrl_mdl);

	if (DATA_CTRL_MDL_NONE != phl_info->pause_rx_id) {
		PHL_WARN("[DATA_CTRL] sw rx is still paused by rx pause id = 0x%x\n",
		         phl_info->pause_rx_id);

		sts = RTW_PHL_STATUS_SUCCESS;
	} else {
		ops->trx_resume(phl_info, PHL_CTRL_RX);

		sts = rtw_phl_start_rx_process(phl_info);
	}

	return sts;
}

void
_phl_sw_rx_rst(struct phl_info_t *phl_info)
{
	struct phl_hci_trx_ops *ops = phl_info->hci_trx_ops;

	ops->trx_reset(phl_info, PHL_CTRL_RX);
}

enum rtw_phl_status
_phl_sw_rx_pause(struct phl_info_t *phl_info,
                 struct phl_data_ctl_t *ctl,
                 bool rst_sw)
{
	enum rtw_phl_status sts = RTW_PHL_STATUS_FAILURE;
	struct phl_hci_trx_ops *ops = phl_info->hci_trx_ops;
	void *drv = phl_to_drvpriv(phl_info);
	u32 i = 0;
	enum data_ctrl_mdl ctrl_mdl = _phl_get_ctrl_mdl(ctl->id);
#ifdef DBG_DBCC_MONITOR_TIME
	u32 start_t = 0;

	phl_fun_monitor_start(&start_t, true, __FUNCTION__);
#endif /* DBG_DBCC_MONITOR_TIME */
	if (PHL_RX_STATUS_SW_PAUSE ==
	    _os_atomic_read(drv, &phl_info->phl_sw_rx_sts)) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_,
		          "[DATA_CTRL] SW rx has been paused by module(0x%x)\n",
		          phl_info->pause_rx_id);

		SET_STATUS_FLAG(phl_info->pause_rx_id, ctrl_mdl);

		PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_,
		          "[DATA_CTRL] Update pause sw rx id(0x%x) by module(%d)\n",
		          phl_info->pause_rx_id, ctl->id);
		sts = RTW_PHL_STATUS_SUCCESS;
		goto exit;
	}

	if (PHL_RX_STATUS_STOP_INPROGRESS ==
	    _os_atomic_read(drv, &phl_info->phl_sw_rx_sts)) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_,
		          "[DATA_CTRL] SW rx has been requested to pause by module(0x%x)\n",
		          phl_info->pause_rx_id);

		SET_STATUS_FLAG(phl_info->pause_rx_id, ctrl_mdl);

		PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_,
		          "[DATA_CTRL] Update pause sw rx id(0x%x) by module(%d)\n",
		          phl_info->pause_rx_id, ctl->id);
		sts = RTW_PHL_STATUS_SUCCESS;
		goto exit;
	}

	/* requset sw rx to stop */
	ops->req_rx_stop(phl_info);

	/*
	 * notify sw rx one last time
	 * and poll if it receviced the stop request and paused itself
	 */
	if (RTW_PHL_STATUS_SUCCESS == rtw_phl_start_rx_process(phl_info)) {
		u32 start = _os_get_cur_time_ms();
		u32 cnt = (POLL_SW_RX_PAUSE_MAX_MS * 1000) / 50;

		for (i = 0; (i < cnt) &&
		     (phl_get_passing_time_ms(start) < POLL_SW_TX_PAUSE_MAX_MS);
		     i++) {
			if (false == ops->is_rx_pause(phl_info)) {
				_os_sleep_us(drv, 50);
				continue;
			}

			phl_handle_queued_rx(phl_info);

			if (false == rtw_phl_is_phl_rx_idle(phl_info)) {
				PHL_INFO("[DATA_CTRL] Still have running rx pkt\n");
				phl_dump_all_sta_rx_info(phl_info);
				PHL_INFO("[DATA_CTRL] phl_rx_ring stored rx number = %d\n",
				         rtw_phl_query_new_rx_num(phl_info));
			}
			SET_STATUS_FLAG(phl_info->pause_rx_id, ctrl_mdl);
			sts = RTW_PHL_STATUS_SUCCESS;
			break;

		}

		if (RTW_PHL_STATUS_SUCCESS != sts) {
			SET_STATUS_FLAG(phl_info->pause_rx_id, ctrl_mdl);
			sts = RTW_PHL_STATUS_CMD_TIMEOUT;
			PHL_TRACE(COMP_PHL_DBG, _PHL_ERR_,
			          "[DATA_CTRL] Module(%d) polling sw rx pause timeout (%d ms)!\n",
			          ctl->id, POLL_SW_RX_PAUSE_MAX_MS);
		} else {
			if (true == rst_sw) {
				PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_,
				          "[DATA_CTRL] Pause Rx with reset is not supported now! requested by module(%d)\n",
				          ctl->id);
			}
		}
	} else {
		PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_, "[DATA_CTRL] Schedule sw rx process fail!\n");
	}
exit:
#ifdef DBG_DBCC_MONITOR_TIME
	phl_fun_monitor_end(&start_t, __FUNCTION__);
#endif /* DBG_DBCC_MONITOR_TIME */
	return sts;
}

enum rtw_phl_status
_phl_hw_trx_rst_resume(struct phl_info_t *phl_info)
{
	void *drv = phl_to_drvpriv(phl_info);

	if (false == _os_atomic_read(drv, &phl_info->is_hw_trx_pause)) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_, "[DATA_CTRL] HW T/Rx is not paused\n");
	}

	if (rtw_hal_lv1_rcvy(phl_info->hal, RTW_PHL_SER_LV1_SER_RCVY_STEP_2) !=
	    RTW_HAL_STATUS_SUCCESS) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_ERR_, "[DATA_CTRL] Reset and Resume HW T/Rx fail\n");
		return RTW_PHL_STATUS_FAILURE;
	} else {
		_os_atomic_set(drv, &phl_info->is_hw_trx_pause, false);
		return RTW_PHL_STATUS_SUCCESS;
	}
}

enum rtw_phl_status
_phl_hw_trx_pause(struct phl_info_t *phl_info)
{
	void *drv = phl_to_drvpriv(phl_info);

	if (true == _os_atomic_read(drv, &phl_info->is_hw_trx_pause)) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_, "[DATA_CTRL] HW T/Rx is already paused\n");
	}

	if (rtw_hal_lv1_rcvy(phl_info->hal, RTW_PHL_SER_LV1_RCVY_STEP_1) !=
	    RTW_HAL_STATUS_SUCCESS) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_ERR_, "[DATA_CTRL] Pause HW T/Rx fail\n");
		return RTW_PHL_STATUS_FAILURE;
	} else {
		_os_atomic_set(drv, &phl_info->is_hw_trx_pause, true);
		return RTW_PHL_STATUS_SUCCESS;
	}
}

enum rtw_phl_status
_phl_trx_sw_pause(struct phl_info_t *phl_info, struct phl_data_ctl_t *ctl)
{
	enum rtw_phl_status sts = RTW_PHL_STATUS_FAILURE;

	do {
		sts = _phl_sw_tx_pause(phl_info, ctl, false);
		if (RTW_PHL_STATUS_SUCCESS != sts) {
			PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_, "[DATA_CTRL] Pause SW Tx fail in PHL_DATA_CTL_TRX_SW_PAUSE!\n");
			break;
		}

		sts = _phl_sw_rx_pause(phl_info, ctl, false);
		if (RTW_PHL_STATUS_SUCCESS != sts) {
			PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_, "[DATA_CTRL] Pause SW Rx fail in PHL_DATA_CTL_TRX_SW_PAUSE!\n");
			break;
		}
	} while (false);

	return sts;
}

enum rtw_phl_status
_phl_trx_sw_resume(struct phl_info_t *phl_info, struct phl_data_ctl_t *ctl)
{
	enum rtw_phl_status sts = RTW_PHL_STATUS_FAILURE;

	do {
		sts = _phl_sw_tx_resume(phl_info, ctl);
		if (RTW_PHL_STATUS_SUCCESS != sts) {
			PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_, "[DATA_CTRL] Resume SW Tx fail in PHL_DATA_CTL_TRX_SW_RESUME!\n");
			break;
		}

		sts = _phl_sw_rx_resume(phl_info, ctl);
		if (RTW_PHL_STATUS_SUCCESS != sts) {
			PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_, "[DATA_CTRL] Resume SW Rx fail in PHL_DATA_CTL_TRX_SW_RESUME!\n");
			break;
		}
	} while (false);

	return sts;
}

enum rtw_phl_status
_phl_trx_pause_w_rst(struct phl_info_t *phl_info,
                     struct phl_data_ctl_t *ctl,
                     struct phl_msg *msg)
{
	enum rtw_phl_status sts = RTW_PHL_STATUS_FAILURE;
	enum data_ctrl_err_code *err_sts = NULL;

	if (msg->outbuf && msg->outlen == sizeof(*err_sts))
		err_sts = (enum data_ctrl_err_code *)msg->outbuf;

	do {
		sts = _phl_sw_tx_pause(phl_info, ctl, false);
		if (RTW_PHL_STATUS_SUCCESS != sts) {
			if (err_sts) {
				if (RTW_PHL_STATUS_CMD_TIMEOUT == sts)
					*err_sts = CTRL_ERR_SW_TX_PAUSE_POLLTO;
				else
					*err_sts = CTRL_ERR_SW_TX_PAUSE_FAIL;
			}
			PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_, "[DATA_CTRL] Pause SW Tx fail in PHL_DATA_CTL_TRX_PAUSE_W_RST!\n");
			break;
		}

		sts = _phl_hw_trx_pause(phl_info);
		if (RTW_PHL_STATUS_SUCCESS != sts) {
			if (err_sts)
				*err_sts = CTRL_ERR_HW_TRX_PAUSE_FAIL;
			PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_, "[DATA_CTRL] Pause HW T/Rx fail in PHL_DATA_CTL_TRX_PAUSE_W_RST!\n");
			break;
		}

		sts = _phl_sw_rx_pause(phl_info, ctl, false);
		if (RTW_PHL_STATUS_SUCCESS != sts) {
			if (err_sts) {
				if (RTW_PHL_STATUS_CMD_TIMEOUT == sts)
					*err_sts = CTRL_ERR_SW_RX_PAUSE_POLLTO;
				else
					*err_sts = CTRL_ERR_SW_RX_PAUSE_FAIL;
			}
			PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_, "[DATA_CTRL] Pause SW Rx fail in PHL_DATA_CTL_TRX_PAUSE_W_RST!\n");
			break;
		}

		_phl_sw_tx_rst(phl_info);
		_phl_sw_rx_rst(phl_info);
	} while (false);

	return sts;
}

enum rtw_phl_status
_phl_trx_resume_w_rst(struct phl_info_t *phl_info,
                      struct phl_data_ctl_t *ctl,
                      struct phl_msg *msg)
{
	enum rtw_phl_status sts = RTW_PHL_STATUS_FAILURE;
	enum data_ctrl_err_code *err_sts = NULL;

	if (msg->outbuf && msg->outlen == sizeof(*err_sts))
		err_sts = (enum data_ctrl_err_code *)msg->outbuf;

	do {
		sts = _phl_sw_rx_resume(phl_info, ctl);
		if (RTW_PHL_STATUS_SUCCESS != sts) {
			if (err_sts)
				*err_sts = CTRL_ERR_SW_RX_RESUME_FAIL;

			PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_, "[DATA_CTRL] Resume SW Rx fail in PHL_DATA_CTL_TRX_RESUME_W_RST!\n");
			break;
		}

		sts = _phl_hw_trx_rst_resume(phl_info);
		if (RTW_PHL_STATUS_SUCCESS != sts) {
			if (err_sts)
				*err_sts = CTRL_ERR_HW_TRX_RESUME_FAIL;

			PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_, "[DATA_CTRL] Resume HW T/Rx fail in PHL_DATA_CTL_TRX_RESUME_W_RST!\n");
			break;
		}

		sts = _phl_sw_tx_resume(phl_info, ctl);
		if (RTW_PHL_STATUS_SUCCESS != sts) {
			if (err_sts)
				*err_sts = CTRL_ERR_SW_TX_RESUME_FAIL;

			PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_, "[DATA_CTRL] Resume SW Tx fail in PHL_DATA_CTL_TRX_RESUME_W_RST!\n");
			break;
		}
	} while (false);

	return sts;
}

enum rtw_phl_status
phl_data_ctrler(struct phl_info_t *phl_info, struct phl_data_ctl_t *ctl,
		struct phl_msg *msg)
{
	enum rtw_phl_status sts = RTW_PHL_STATUS_FAILURE;

	if (NULL == ctl) {
		PHL_WARN("phl_tx_ctrler(): input ctl is NULL\n");
		return RTW_PHL_STATUS_FAILURE;
	}

	switch (ctl->cmd) {
	case PHL_DATA_CTL_HW_TRX_RST_RESUME:
		sts = _phl_hw_trx_rst_resume(phl_info);
		break;
	case PHL_DATA_CTL_HW_TRX_PAUSE:
		sts = _phl_hw_trx_pause(phl_info);
		break;
	case PHL_DATA_CTL_SW_TX_RESUME:
		sts = _phl_sw_tx_resume(phl_info, ctl);
		break;
	case PHL_DATA_CTL_SW_RX_RESUME:
		sts = _phl_sw_rx_resume(phl_info, ctl);
		break;
	case PHL_DATA_CTL_SW_TX_PAUSE:
		sts = _phl_sw_tx_pause(phl_info, ctl, false);
		break;
	case PHL_DATA_CTL_SW_RX_PAUSE:
		sts = _phl_sw_rx_pause(phl_info, ctl, false);
		break;
	case PHL_DATA_CTL_SW_TX_RESET:
		_phl_sw_tx_rst(phl_info);
		sts = RTW_PHL_STATUS_SUCCESS;
		break;
	case PHL_DATA_CTL_SW_RX_RESET:
		_phl_sw_rx_rst(phl_info);
		sts = RTW_PHL_STATUS_SUCCESS;
		break;
	case PHL_DATA_CTL_TRX_SW_PAUSE:
		sts = _phl_trx_sw_pause(phl_info, ctl);
		break;
	case PHL_DATA_CTL_TRX_SW_RESUME:
		sts = _phl_trx_sw_resume(phl_info, ctl);
		break;
	case PHL_DATA_CTL_TRX_PAUSE_W_RST:
		sts = _phl_trx_pause_w_rst(phl_info, ctl, msg);
		break;
	case PHL_DATA_CTL_TRX_RESUME_W_RST:
		sts = _phl_trx_resume_w_rst(phl_info, ctl, msg);
		break;
	default:
		PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_,
		          "Unknown data control command(%d)!\n", ctl->cmd);
		break;
	}
	return sts;
}

static enum rtw_phl_status
_phl_set_lifetime(void *phl, u8 hw_band, u8 enable, u16 val)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;

	if (rtw_hal_com_set_tx_lifetime(phl_info->hal, hw_band,
				enable, val) == RTW_HAL_STATUS_SUCCESS)
		return RTW_PHL_STATUS_SUCCESS;
	else
		return RTW_PHL_STATUS_FAILURE;
}

#ifdef CONFIG_CMD_DISP
enum rtw_phl_status
phl_cmd_cfg_lifetime_hdl(struct phl_info_t *phl_info, u8 *param)
{
	struct lifetime_ctx *lt_ctx = (struct lifetime_ctx *)param;

	return _phl_set_lifetime((void *)phl_info, lt_ctx->hw_band,
				lt_ctx->en, lt_ctx->val);
}
#endif

enum rtw_phl_status
rtw_phl_cmd_cfg_lifetime(void *phl,
                         struct rtw_wifi_role_link_t *rlink,
                         u8 enable,
                         u16 acq_val,
                         enum phl_cmd_type cmd_type,
                         u32 cmd_timeout)
{
	enum rtw_phl_status sts = RTW_PHL_STATUS_FAILURE;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct lifetime_ctx *lt_ctx = (struct lifetime_ctx *) &phl_info->lt_ctx;

	lt_ctx->hw_band = rlink->hw_band;
	lt_ctx->en = enable;
	lt_ctx->val = acq_val;

#ifdef CONFIG_CMD_DISP
	sts = phl_cmd_enqueue(phl,
	                      rlink->hw_band,
	                      MSG_EVT_LIFETIME_SETUP,
	                      (u8 *)lt_ctx, 0,
	                      NULL,
	                      cmd_type,
	                      cmd_timeout);
	if (is_cmd_failure(sts)) {
		/* Send cmd success, but wait cmd fail*/
		sts = RTW_PHL_STATUS_FAILURE;
	} else if (sts != RTW_PHL_STATUS_SUCCESS) {
		/* Send cmd fail */
		sts = RTW_PHL_STATUS_FAILURE;
	}

	return sts;
#else
	PHL_ERR("%s : CONFIG_CMD_DISP not set for MSG_EVT_LIFETIME_SETUP\n", __func__);
	return sts;
#endif
}

static enum rtw_phl_status
_phl_set_power_offset(void *phl, u8 hw_band, s8 ofst_mode, s8 ofst_bw)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;

	if (rtw_hal_com_set_power_offset(phl_info->hal, hw_band,
				ofst_mode, ofst_bw) == RTW_HAL_STATUS_SUCCESS)
		return RTW_PHL_STATUS_SUCCESS;
	else
		return RTW_PHL_STATUS_FAILURE;
}

#ifdef CONFIG_CMD_DISP
enum rtw_phl_status
phl_cmd_cfg_power_offset_hdl(struct phl_info_t *phl_info, u8 *param)
{
	struct power_offset_ctx *pwr_ctx = (struct power_offset_ctx *)param;

	return _phl_set_power_offset((void *)phl_info, pwr_ctx->hw_band,
				pwr_ctx->ofst_mode, pwr_ctx->ofst_bw);
}
#endif

enum rtw_phl_status
rtw_phl_cmd_cfg_power_offset(void *phl,
                             struct rtw_wifi_role_link_t *rlink,
                             s8 ofst_mode,
                             s8 ofst_bw,
                             enum phl_cmd_type cmd_type,
                             u32 cmd_timeout)
{
	enum rtw_phl_status sts = RTW_PHL_STATUS_FAILURE;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct power_offset_ctx *pwr_ctx =
			(struct power_offset_ctx *)&phl_info->pwr_ofst_ctx;

	pwr_ctx->hw_band = rlink->hw_band;
	pwr_ctx->ofst_mode = ofst_mode;
	pwr_ctx->ofst_bw = ofst_bw;

#ifdef CONFIG_CMD_DISP
	sts = phl_cmd_enqueue(phl,
	                      rlink->hw_band,
	                      MSG_EVT_POWER_OFFSET_SETUP,
	                      (u8 *)pwr_ctx,
	                      0,
	                      NULL,
	                      cmd_type,
	                      cmd_timeout);
	if (is_cmd_failure(sts)) {
		/* Send cmd success, but wait cmd fail*/
		sts = RTW_PHL_STATUS_FAILURE;
	} else if (sts != RTW_PHL_STATUS_SUCCESS) {
		/* Send cmd fail */
		sts = RTW_PHL_STATUS_FAILURE;
	}

	return sts;
#else
	PHL_ERR("%s : CONFIG_CMD_DISP not set for MSG_EVT_POWER_OFFSET_SETUP\n", __func__);
	return sts;
#endif
}

static enum rtw_phl_status
_phl_set_gt3_with_type(void *phl, u8 en, u32 timeout)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	void *hal = phl_info->hal;

	if(rtw_hal_com_set_gt3(hal, en, timeout) == RTW_HAL_STATUS_SUCCESS)
		return RTW_PHL_STATUS_SUCCESS;
	else
		return RTW_PHL_STATUS_FAILURE;
}

static enum rtw_phl_status _phl_set_gt3(void *phl, u8 en, u32 timeout)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct gtimer_ctx *gt_ctx = (struct gtimer_ctx *) &phl_info->gt3_ctx;

	gt_ctx->en = en;
	gt_ctx->timer_type = _GT3_TYPE_SH_TASK;
	gt_ctx->duration = timeout;

	return _phl_set_gt3_with_type(phl, en, timeout);
}

#ifdef CONFIG_CMD_DISP
enum rtw_phl_status
phl_cmd_cfg_gt3_hdl(struct phl_info_t *phl_info, u8 *param)
{
	struct gtimer_ctx *gt_ctx = (struct gtimer_ctx *)param;

	PHL_INFO(" %s(), gt_ctx = %p !\n", __func__, gt_ctx);

	if (0 == gt_ctx->timer_type)
		return _phl_set_gt3((void *)phl_info,
			gt_ctx->en, gt_ctx->duration);
	else
		return _phl_set_gt3_with_type((void *)phl_info,
			gt_ctx->en, gt_ctx->duration);
}
#endif

enum rtw_phl_status
rtw_phl_cfg_gtimer_register(void *phl,
                            struct rtw_wifi_role_link_t *rlink,
                            u8 type,
                            u8 enable,
                            u32 timeout,
                            enum phl_cmd_type cmd_type,
                            u32 cmd_timeout)
{
	enum rtw_phl_status sts = RTW_PHL_STATUS_FAILURE;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct gtimer_ctx *gt_ctx = (struct gtimer_ctx *) &phl_info->gt3_ctx;

	gt_ctx->en = enable;
	gt_ctx->timer_type = type;
	gt_ctx->duration = timeout;

#ifdef CONFIG_CMD_DISP
	sts = phl_cmd_enqueue(phl,
	                      rlink->hw_band,
	                      MSG_EVT_GT3_SETUP,
	                      (u8 *)gt_ctx,
	                      0,
	                      NULL,
	                      cmd_type,
	                      cmd_timeout);
	if (is_cmd_failure(sts)) {
		/* Send cmd success, but wait cmd fail*/
		sts = RTW_PHL_STATUS_FAILURE;
	} else if (sts != RTW_PHL_STATUS_SUCCESS) {
		/* Send cmd fail */
		sts = RTW_PHL_STATUS_FAILURE;
	}

	return sts;
#else
	PHL_ERR("%s : CONFIG_CMD_DISP not set for MSG_EVT_GT3_SETUP\n", __func__);
	return sts;
#endif
}

#ifdef CONFIG_CMD_DISP
enum rtw_phl_status
phl_cmd_cfg_hw_seq_hdl(struct phl_info_t *phl_info, u8 *param)
{
	struct rtw_phl_stainfo_t *sta = (struct rtw_phl_stainfo_t *)param;

	PHL_INFO(" %s(), sta = %p !\n", __func__, sta);

	return rtw_hal_set_dctrl_tbl_seq((void *)phl_info->hal, sta, sta->hw_seq);
}
#endif

enum rtw_phl_status
rtw_phl_cmd_cfg_hw_seq(void *phl,
                       struct rtw_wifi_role_link_t *rlink,
                       struct rtw_phl_stainfo_t *sta,
                       u32 seq,
                       u16 rts_rate,
                       enum phl_cmd_type cmd_type,
                       u32 cmd_timeout)
{
	enum rtw_phl_status sts = RTW_PHL_STATUS_FAILURE;

	if (NULL == sta)
		return RTW_PHL_STATUS_FAILURE;

	sta->hw_seq = seq;
	sta->set_rts_init_rate = rts_rate;

#ifdef CONFIG_CMD_DISP
	sts = phl_cmd_enqueue(phl,
	                      rlink->hw_band,
	                      MSG_EVT_HW_SEQ_SETUP,
	                      (u8 *)sta,
	                      0,
	                      NULL,
	                      cmd_type,
	                      cmd_timeout);
	if (is_cmd_failure(sts)) {
		/* Send cmd success, but wait cmd fail*/
		sts = RTW_PHL_STATUS_FAILURE;
	} else if (sts != RTW_PHL_STATUS_SUCCESS) {
		/* Send cmd fail */
		sts = RTW_PHL_STATUS_FAILURE;
	}

	return sts;
#else
	PHL_ERR("%s : CMD_DISP not set for MSG_EVT_HW_SEQ_SETUP\n", __func__);
	return sts;
#endif
}

#ifdef CONFIG_PCI_HCI
void rtw_phl_get_hw_cnt_tx_fail(void *phl, u32 *tx_fail, u32 *tx_fail_mgmt)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct rtw_hal_com_t *hal_com = rtw_hal_get_halcom(phl_info->hal);
	struct rtw_wp_rpt_stats *rpt_sts = (struct rtw_wp_rpt_stats *)hal_com->trx_stat.wp_rpt_stats;
	u8 txch_num = rtw_hal_query_txch_num(phl_info->hal);
	u8 mgmt_ch = 0;
	u8 i;

	*tx_fail = 0;
	*tx_fail_mgmt = 0;

	if (rpt_sts) {
		for (i = 0; i < txch_num; i++)
			*tx_fail += rpt_sts[i].tx_fail_cnt;

		/* sum up mgmt queue counters of all HW bands */
		for (i = 0; i < HW_BAND_MAX; i++) {
			mgmt_ch = rtw_hal_tx_chnl_mapping(phl_info->hal, 0,
				RTW_PHL_RING_CAT_MGNT, i);
			*tx_fail_mgmt += rpt_sts[mgmt_ch].tx_fail_cnt;
		}
	}
}

void rtw_phl_get_hw_cnt_tx_ok(void *phl, u32 *tx_ok, u32 *tx_ok_mgmt)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct rtw_hal_com_t *hal_com = rtw_hal_get_halcom(phl_info->hal);
	struct rtw_wp_rpt_stats *rpt_sts = (struct rtw_wp_rpt_stats *)hal_com->trx_stat.wp_rpt_stats;
	u8 txch_num = rtw_hal_query_txch_num(phl_info->hal);
	u8 mgmt_ch = 0;
	u8 i;

	*tx_ok = 0;
	*tx_ok_mgmt = 0;

	if (rpt_sts) {
		for(i = 0; i < txch_num; i++) {
			*tx_ok += rpt_sts[i].tx_ok_cnt;
			*tx_ok += rpt_sts[i].delay_tx_ok_cnt;
		}

		/* sum up mgmt queue counters of all HW bands */
		for (i = 0; i < HW_BAND_MAX; i++) {
			mgmt_ch = rtw_hal_tx_chnl_mapping(phl_info->hal, 0,
				RTW_PHL_RING_CAT_MGNT, i);
			*tx_ok_mgmt += rpt_sts[mgmt_ch].tx_ok_cnt;
			*tx_ok_mgmt += rpt_sts[mgmt_ch].delay_tx_ok_cnt;
		}
	}
}
#endif

#ifdef CONFIG_CMD_DISP
static void
_phl_tx_packet_notify_done(void *drv_priv, u8 *cmd, u32 cmd_len,
			enum rtw_phl_status status)
{
	if (cmd && cmd_len){
		_os_kmem_free(drv_priv, cmd, cmd_len);
		cmd = NULL;
		PHL_INFO("%s.....\n", __func__);
	}
}

void rtw_phl_tx_packet_event_notify(void *phl,
	struct rtw_wifi_role_link_t *rlink,
	enum phl_pkt_evt_type pkt_evt)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	enum phl_pkt_evt_type *pkt_evt_type = NULL;
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_FAILURE;

	pkt_evt_type = (enum phl_pkt_evt_type *)_os_kmem_alloc(
		phl_to_drvpriv(phl_info), sizeof(enum phl_pkt_evt_type));
	if (pkt_evt_type == NULL) {
		PHL_ERR("%s: alloc packet cmd fail.\n", __func__);
		return;
	}

	*pkt_evt_type = pkt_evt;

	phl_status = phl_cmd_enqueue(phl_info,
                                     rlink->hw_band,
                                     MSG_EVT_PKT_EVT_NTFY,
                                     (u8 *)pkt_evt_type,
                                     sizeof(enum phl_pkt_evt_type),
                                     _phl_tx_packet_notify_done,
                                     PHL_CMD_NO_WAIT,
                                     0);

	if (is_cmd_failure(phl_status)) {
		/* Send cmd success, but wait cmd fail*/
		PHL_ERR("%s event: %d status: %d\n",
			__func__, pkt_evt, phl_status);
	} else if (phl_status != RTW_PHL_STATUS_SUCCESS) {
		/* Send cmd fail */
		PHL_ERR("%s event: %d status: %d\n",
			__func__, pkt_evt, phl_status);
		_os_kmem_free(phl_to_drvpriv(phl_info), pkt_evt_type,
			sizeof(enum phl_pkt_evt_type));
	}
}

static void
_phl_packet_notify_done(void *drv_priv, u8 *cmd, u32 cmd_len,
			enum rtw_phl_status status)
{
	if (cmd && cmd_len){
		_os_kmem_free(drv_priv, cmd, cmd_len);
		cmd = NULL;
		PHL_INFO("%s.....\n", __func__);
	}
}

void rtw_phl_packet_event_notify(void *phl,
	struct rtw_wifi_role_link_t *rlink,
	enum phl_pkt_evt_type pkt_evt)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	enum phl_pkt_evt_type *pkt_evt_type = NULL;
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_FAILURE;

	pkt_evt_type = (enum phl_pkt_evt_type *)_os_kmem_alloc(
		phl_to_drvpriv(phl_info), sizeof(enum phl_pkt_evt_type));
	if (pkt_evt_type == NULL) {
		PHL_ERR("%s: alloc packet cmd fail.\n", __func__);
		return;
	}

	*pkt_evt_type = pkt_evt;

	phl_status = phl_cmd_enqueue(phl_info,
                                     rlink->hw_band,
                                     MSG_EVT_PKT_EVT_NTFY,
                                     (u8 *)pkt_evt_type,
                                     sizeof(enum phl_pkt_evt_type),
                                     _phl_packet_notify_done,
                                     PHL_CMD_NO_WAIT,
                                     0);

	if (is_cmd_failure(phl_status)) {
		/* Send cmd success, but wait cmd fail*/
		PHL_ERR("%s event: %d status: %d\n",
			__func__, pkt_evt, phl_status);
	} else if (phl_status != RTW_PHL_STATUS_SUCCESS) {
		/* Send cmd fail */
		PHL_ERR("%s event: %d status: %d\n",
			__func__, pkt_evt, phl_status);
		_os_kmem_free(phl_to_drvpriv(phl_info), pkt_evt_type,
			sizeof(enum phl_pkt_evt_type));
	}
}

#define TX_DBG_STATUS_DUMP_INTERVAL 30000 /* ms */
void phl_tx_dbg_status_dump(struct phl_info_t *phl_info, enum phl_band_idx hwband)
{
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_FAILURE;

	static u32 last_dump_t = 0;

	if (phl_get_passing_time_ms(last_dump_t) >= TX_DBG_STATUS_DUMP_INTERVAL) {
		phl_status = phl_cmd_enqueue(phl_info,
					     hwband,
					     MSG_EVT_DBG_TX_DUMP,
					     NULL,
					     0,
					     NULL,
					     PHL_CMD_NO_WAIT,
					     0);
		if (phl_status != RTW_PHL_STATUS_SUCCESS) {
			PHL_TRACE(COMP_PHL_DBG, _PHL_ERR_, "%s: cmd enqueue fail!\n",
				  __func__);
		}
		last_dump_t = _os_get_cur_time_ms();
	}
}

static enum rtw_phl_status
_phl_cfg_hw_cts2self(struct phl_info_t *phl_info,
		     struct rtw_phl_hw_cts2self_cfg *cts_cfg)
{
	enum rtw_hal_status hsts = RTW_HAL_STATUS_FAILURE;

	hsts = rtw_hal_cfg_hw_cts2self(phl_info->hal,
				    cts_cfg->band_sel,
				    cts_cfg->enable,
				    cts_cfg->non_sec_threshold,
				    cts_cfg->sec_threshold);

	if (RTW_HAL_STATUS_SUCCESS != hsts)
		goto fail;

	return RTW_PHL_STATUS_SUCCESS;

fail:
	return RTW_PHL_STATUS_FAILURE;
}

enum rtw_phl_status
phl_cmd_cfg_hw_cts2self_hdl(struct phl_info_t *phl_info, u8 *param)
{
	struct rtw_phl_hw_cts2self_cfg *cts_cfg = (struct rtw_phl_hw_cts2self_cfg *)param;

	PHL_INFO(" %s(), cts_cfg = %p !\n", __func__, cts_cfg);

	return _phl_cfg_hw_cts2self(phl_info, cts_cfg);
}

static void _phl_hw_cts2self_done(void *drv_priv, u8 *cmd, u32 cmd_len, enum rtw_phl_status status)
{
	if (cmd) {
		_os_mem_free(drv_priv, cmd, cmd_len);
		cmd = NULL;
	}
}

enum rtw_phl_status
rtw_phl_hw_cts2self_cfg(void *phl, u8 enable,
			u8 band_sel, u8 non_sec_thr, u8 sec_thr)
{
#ifdef CONFIG_CMD_DISP
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	void *drv_priv = phl_to_drvpriv(phl_info);
	struct rtw_phl_hw_cts2self_cfg *cts_cfg = NULL;
	enum rtw_phl_status psts = RTW_PHL_STATUS_FAILURE;
	u32 cfg_len = sizeof(struct rtw_phl_hw_cts2self_cfg);

	cts_cfg = _os_mem_alloc(drv_priv, cfg_len);
	if (cts_cfg == NULL) {
		PHL_ERR("%s: alloc cts_cfg failed!\n", __func__);
		goto _exit;
	}

	cts_cfg->enable = enable;
	cts_cfg->band_sel = band_sel;
	cts_cfg->non_sec_threshold = non_sec_thr;
	cts_cfg->sec_threshold = sec_thr;

	psts = phl_cmd_enqueue(phl_info,
	                       HW_BAND_0,
	                       MSG_EVT_HW_CTS2SELF,
	                       (u8 *)cts_cfg,
	                       cfg_len,
	                       _phl_hw_cts2self_done,
	                       PHL_CMD_NO_WAIT,
	                       0);

	if (is_cmd_failure(psts)) {
		/* Send cmd success, but wait cmd fail*/
		psts = RTW_PHL_STATUS_FAILURE;
	} else if (psts != RTW_PHL_STATUS_SUCCESS) {
		/* Send cmd fail */
		psts = RTW_PHL_STATUS_FAILURE;
		_os_mem_free(drv_priv, cts_cfg, cfg_len);
	}
_exit:
	return psts;
#else
	PHL_ERR("phl_fsm not support %s\n", __func__);
	return RTW_PHL_STATUS_FAILURE;
#endif /*CONFIG_CMD_DISP*/
}

#endif /* #ifdef CONFIG_CMD_DISP */

bool
rtw_phl_check_sta_has_busy_wp(struct rtw_phl_stainfo_t *sta) {
	return rtw_hal_check_sta_has_busy_wp(sta);
}

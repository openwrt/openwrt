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
#define _PHL_RX_C_
#include "phl_headers.h"


struct rtw_phl_rx_pkt *rtw_phl_query_phl_rx(void *phl)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	void *drv_priv = phl_to_drvpriv(phl_info);
	struct phl_rx_pkt_pool *rx_pkt_pool = NULL;
	struct rtw_phl_rx_pkt *phl_rx = NULL;

	rx_pkt_pool = (struct phl_rx_pkt_pool *)phl_info->rx_pkt_pool;

	_os_spinlock(drv_priv, &rx_pkt_pool->idle_lock, _bh, NULL);

	if (false == list_empty(&rx_pkt_pool->idle)) {
		phl_rx = list_first_entry(&rx_pkt_pool->idle,
					struct rtw_phl_rx_pkt, list);
		list_del(&phl_rx->list);
		rx_pkt_pool->idle_cnt--;
	}

	_os_spinunlock(drv_priv, &rx_pkt_pool->idle_lock, _bh, NULL);

	return phl_rx;
}

u8 rtw_phl_is_phl_rx_idle(struct phl_info_t *phl_info)
{
	struct phl_rx_pkt_pool *rx_pkt_pool = NULL;
	u8 res = false;

	rx_pkt_pool = (struct phl_rx_pkt_pool *)phl_info->rx_pkt_pool;

	_os_spinlock(phl_to_drvpriv(phl_info), &rx_pkt_pool->idle_lock, _bh, NULL);

	if (MAX_PHL_RING_RX_PKT_NUM == rx_pkt_pool->idle_cnt)
		res = true;
	else
		res = false;

	_os_spinunlock(phl_to_drvpriv(phl_info), &rx_pkt_pool->idle_lock, _bh, NULL);

	return res;
}

void phl_dump_rx_stats(struct rtw_stats *stats)
{
	PHL_TRACE(COMP_PHL_XMIT, _PHL_DEBUG_,
		  "Dump Rx statistics\n"
		  "rx_byte_uni = %lld\n"
		  "rx_byte_total = %lld\n"
		  "rx_tp_kbits = %d\n"
		  "last_rx_time_ms = %d\n",
		  stats->rx_byte_uni,
		  stats->rx_byte_total,
		  stats->rx_tp_kbits,
		  stats->last_rx_time_ms);
}

void phl_reset_rx_stats(struct rtw_stats *stats)
{
	stats->rx_byte_uni = 0;
	stats->rx_byte_total = 0;
	stats->rx_tp_kbits = 0;
	stats->last_rx_time_ms = 0;
	stats->rxtp.last_calc_time_ms = 0;
	stats->rxtp.last_calc_time_ms = 0;
	stats->rx_traffic.lvl = RTW_TFC_IDLE;
	stats->rx_traffic.sts = 0;
	stats->rx_tf_cnt = 0;
	stats->pre_rx_tf_cnt = 0;
}

void
phl_rx_traffic_upd(struct rtw_stats *sts)
{
	u32 tp_k = 0, tp_m = 0;
	enum rtw_tfc_lvl rx_tfc_lvl = RTW_TFC_IDLE;
	tp_k = sts->rx_tp_kbits;
	tp_m = sts->rx_tp_kbits >> 10;

	if (tp_m >= RX_HIGH_TP_THRES_MBPS)
		rx_tfc_lvl = RTW_TFC_HIGH;
	else if (tp_m >= RX_MID_TP_THRES_MBPS)
		rx_tfc_lvl = RTW_TFC_MID;
	else if (tp_m >= RX_LOW_TP_THRES_MBPS)
		rx_tfc_lvl = RTW_TFC_LOW;
	else if (tp_k >= RX_ULTRA_LOW_TP_THRES_KBPS)
		rx_tfc_lvl = RTW_TFC_ULTRA_LOW;
	else
		rx_tfc_lvl = RTW_TFC_IDLE;

	if (sts->rx_traffic.lvl > rx_tfc_lvl) {
		sts->rx_traffic.sts = (TRAFFIC_CHANGED | TRAFFIC_DECREASE);
		sts->rx_traffic.lvl = rx_tfc_lvl;
	} else if (sts->rx_traffic.lvl < rx_tfc_lvl) {
		sts->rx_traffic.sts = (TRAFFIC_CHANGED | TRAFFIC_INCREASE);
		sts->rx_traffic.lvl = rx_tfc_lvl;
	} else if (sts->rx_traffic.sts &
		(TRAFFIC_CHANGED | TRAFFIC_INCREASE | TRAFFIC_DECREASE)) {
		sts->rx_traffic.sts &= ~(TRAFFIC_CHANGED | TRAFFIC_INCREASE |
					 TRAFFIC_DECREASE);
	}
}

void phl_update_rx_stats(struct rtw_stats *stats, struct rtw_recv_pkt *rx_pkt)
{
	u32 diff_t = 0, cur_time = _os_get_cur_time_ms();
	u64 diff_bits = 0;

	stats->last_rx_time_ms = cur_time;
	stats->rx_byte_total += rx_pkt->mdata.pktlen;
	if (rx_pkt->mdata.bc == 0 && rx_pkt->mdata.mc == 0)
		stats->rx_byte_uni += rx_pkt->mdata.pktlen;

	if (0 == stats->rxtp.last_calc_time_ms ||
		0 == stats->rxtp.last_calc_bits) {
		stats->rxtp.last_calc_time_ms = stats->last_rx_time_ms;
		stats->rxtp.last_calc_bits = stats->rx_byte_uni * 8;
	} else {
		if (cur_time >= stats->rxtp.last_calc_time_ms) {
			diff_t = cur_time - stats->rxtp.last_calc_time_ms;
		} else {
			diff_t = RTW_U32_MAX - stats->rxtp.last_calc_time_ms +
				cur_time + 1;
		}
		if (diff_t > RXTP_CALC_DIFF_MS && stats->rx_byte_uni != 0) {
			diff_bits = (stats->rx_byte_uni * 8) -
				stats->rxtp.last_calc_bits;
			stats->rx_tp_kbits = (u32)_os_division64(diff_bits,
								 diff_t);
			stats->rxtp.last_calc_bits = stats->rx_byte_uni * 8;
			stats->rxtp.last_calc_time_ms = cur_time;
		}
	}
}

void phl_rx_statistics(struct phl_info_t *phl_info, struct rtw_recv_pkt *rx_pkt)
{
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct rtw_stats *phl_stats = &phl_com->phl_stats;
	struct rtw_stats *sta_stats = NULL;
	struct rtw_phl_stainfo_t *sta = NULL;
	u16 macid = rx_pkt->mdata.macid;

	if (!phl_macid_is_valid(phl_info, macid))
		goto dev_stat;

	sta = rtw_phl_get_stainfo_by_macid(phl_info, macid);

	if (NULL == sta)
		goto dev_stat;
	sta_stats = &sta->stats;

	phl_update_rx_stats(sta_stats, rx_pkt);
dev_stat:
	phl_update_rx_stats(phl_stats, rx_pkt);
}

void phl_release_phl_rx(struct phl_info_t *phl_info,
				struct rtw_phl_rx_pkt *phl_rx)
{
	void *drv_priv = phl_to_drvpriv(phl_info);
	struct phl_rx_pkt_pool *rx_pkt_pool = NULL;

	rx_pkt_pool = (struct phl_rx_pkt_pool *)phl_info->rx_pkt_pool;

	_os_mem_set(phl_to_drvpriv(phl_info), &phl_rx->r, 0, sizeof(phl_rx->r));
	phl_rx->type = RTW_RX_TYPE_MAX;
	phl_rx->rxbuf_ptr = NULL;
	INIT_LIST_HEAD(&phl_rx->list);

	_os_spinlock(drv_priv, &rx_pkt_pool->idle_lock, _bh, NULL);
	list_add_tail(&phl_rx->list, &rx_pkt_pool->idle);
	rx_pkt_pool->idle_cnt++;
	_os_spinunlock(drv_priv, &rx_pkt_pool->idle_lock, _bh, NULL);
}

static void phl_free_recv_pkt_pool(struct phl_info_t *phl_info)
{
	struct phl_rx_pkt_pool *rx_pkt_pool = NULL;
	u32 buf_len = 0;
	FUNCIN();

	rx_pkt_pool = (struct phl_rx_pkt_pool *)phl_info->rx_pkt_pool;
	if (NULL != rx_pkt_pool) {
		_os_spinlock_free(phl_to_drvpriv(phl_info),
					&rx_pkt_pool->idle_lock);
		_os_spinlock_free(phl_to_drvpriv(phl_info),
					&rx_pkt_pool->busy_lock);

		buf_len = sizeof(*rx_pkt_pool);
		_os_mem_free(phl_to_drvpriv(phl_info), rx_pkt_pool, buf_len);
	}

	FUNCOUT();
}

void phl_rx_deinit(struct phl_info_t *phl_info)
{
	/* TODO: rx reorder deinit */

	/* TODO: peer info deinit */

	phl_free_recv_pkt_pool(phl_info);
}


static enum rtw_phl_status phl_alloc_recv_pkt_pool(struct phl_info_t *phl_info)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	struct phl_rx_pkt_pool *rx_pkt_pool = NULL;
	struct rtw_phl_rx_pkt *phl_rx = NULL;
	u32 buf_len = 0, i = 0;
	FUNCIN_WSTS(pstatus);

	buf_len = sizeof(*rx_pkt_pool);
	rx_pkt_pool = _os_mem_alloc(phl_to_drvpriv(phl_info), buf_len);

	if (NULL != rx_pkt_pool) {
		_os_mem_set(phl_to_drvpriv(phl_info), rx_pkt_pool, 0, buf_len);
		INIT_LIST_HEAD(&rx_pkt_pool->idle);
		INIT_LIST_HEAD(&rx_pkt_pool->busy);
		_os_spinlock_init(phl_to_drvpriv(phl_info),
					&rx_pkt_pool->idle_lock);
		_os_spinlock_init(phl_to_drvpriv(phl_info),
					&rx_pkt_pool->busy_lock);
		rx_pkt_pool->idle_cnt = 0;

		for (i = 0; i < MAX_PHL_RING_RX_PKT_NUM; i++) {
			phl_rx = &rx_pkt_pool->phl_rx[i];
			INIT_LIST_HEAD(&phl_rx->list);
			list_add_tail(&phl_rx->list, &rx_pkt_pool->idle);
			rx_pkt_pool->idle_cnt++;
		}

		phl_info->rx_pkt_pool = rx_pkt_pool;

		pstatus = RTW_PHL_STATUS_SUCCESS;
	}

	if (RTW_PHL_STATUS_SUCCESS != pstatus)
		phl_free_recv_pkt_pool(phl_info);
	FUNCOUT_WSTS(pstatus);

	return pstatus;
}

enum rtw_phl_status phl_rx_init(struct phl_info_t *phl_info)
{
	enum rtw_phl_status status;

	/* Allocate rx packet pool */
	status = phl_alloc_recv_pkt_pool(phl_info);
	if (status != RTW_PHL_STATUS_SUCCESS)
		return status;

	/* TODO: Peer info init */


	/* TODO: Rx reorder init */

	return RTW_PHL_STATUS_SUCCESS;
}

void phl_recycle_rx_buf(struct phl_info_t *phl_info,
				struct rtw_phl_rx_pkt *phl_rx)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	struct phl_hci_trx_ops *hci_trx_ops = phl_info->hci_trx_ops;
	struct rtw_rx_buf *rx_buf = NULL;

	do {
		if (NULL == phl_rx) {
			PHL_TRACE(COMP_PHL_RECV, _PHL_WARNING_, "[WARNING]phl_rx is NULL!\n");
			break;
		}

		rx_buf = (struct rtw_rx_buf *)phl_rx->rxbuf_ptr;

		PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "[4] %s:: [%p]\n",
								__FUNCTION__, rx_buf);
		if (phl_rx->rxbuf_ptr) {
			pstatus = hci_trx_ops->recycle_rx_buf(phl_info, rx_buf,
								phl_rx->r.mdata.dma_ch,
								phl_rx->type);
		}
		if (RTW_PHL_STATUS_SUCCESS != pstatus && phl_rx->rxbuf_ptr)
			PHL_TRACE(COMP_PHL_RECV, _PHL_WARNING_, "[WARNING]recycle hci rx buf error!\n");

		phl_release_phl_rx(phl_info, phl_rx);

	} while (false);

}

void _phl_indic_new_rxpkt(struct phl_info_t *phl_info)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_SUCCESS;
	struct rtw_evt_info_t *evt_info = &phl_info->phl_com->evt_info;
	void *drv_priv = phl_to_drvpriv(phl_info);
	FUNCIN_WSTS(pstatus);

	do {
		_os_spinlock(drv_priv, &evt_info->evt_lock, _bh, NULL);
		evt_info->evt_bitmap |= RTW_PHL_EVT_RX;
		_os_spinunlock(drv_priv, &evt_info->evt_lock, _bh, NULL);

		pstatus = phl_schedule_handler(phl_info->phl_com,
							&phl_info->phl_event_handler);
	} while (false);

	if (RTW_PHL_STATUS_SUCCESS != pstatus)
		PHL_TRACE(COMP_PHL_RECV, _PHL_WARNING_, "[WARNING] Trigger rx indic event fail!\n");

	FUNCOUT_WSTS(pstatus);

#ifdef PHL_RX_BATCH_IND
	phl_info->rx_new_pending = 0;
#endif
}

void _phl_record_rx_stats(struct rtw_recv_pkt *recvpkt)
{
	if(NULL == recvpkt)
		return;
	if (recvpkt->tx_sta)
		recvpkt->tx_sta->stats.rx_rate = recvpkt->mdata.rx_rate;
}

enum rtw_phl_status _phl_add_rx_pkt(struct phl_info_t *phl_info,
				    struct rtw_phl_rx_pkt *phl_rx)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	struct rtw_phl_rx_ring *ring = &phl_info->phl_rx_ring;
	struct rtw_recv_pkt *recvpkt = &phl_rx->r;
	u16 ring_res = 0, wptr = 0, rptr = 0;
	void *drv = phl_to_drvpriv(phl_info);

	FUNCIN_WSTS(pstatus);
	_os_spinlock(drv, &phl_info->rx_ring_lock, _bh, NULL);

	if (!ring)
		goto out;

	wptr = (u16)_os_atomic_read(drv, &ring->phl_idx);
	rptr = (u16)_os_atomic_read(drv, &ring->core_idx);

	ring_res = phl_calc_avail_wptr(rptr, wptr, MAX_PHL_RX_RING_ENTRY_NUM);
	PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_,
		"[3] _phl_add_rx_pkt::[Query] phl_idx =%d , core_idx =%d , ring_res =%d\n",
		_os_atomic_read(drv, &ring->phl_idx),
		_os_atomic_read(drv, &ring->core_idx),
		ring_res);
	if (ring_res <= 0) {
		PHL_TRACE(COMP_PHL_RECV, _PHL_INFO_, "no ring resource to add new rx pkt!\n");
		pstatus = RTW_PHL_STATUS_RESOURCE;
		goto out;
	}

	wptr = wptr + 1;
	if (wptr >= MAX_PHL_RX_RING_ENTRY_NUM)
		wptr = 0;

	ring->entry[wptr] = recvpkt;

	if (wptr)
		_os_atomic_inc(drv, &ring->phl_idx);
	else
		_os_atomic_set(drv, &ring->phl_idx, 0);

#ifdef DEBUG_PHL_RX
	phl_info->rx_stats.rx_pkt_core++;
	if (recvpkt->mdata.pktlen == phl_info->cnt_rx_pktsz)
		phl_info->rx_stats.rx_pktsz_core++;
#endif
#ifdef PHL_RX_BATCH_IND
	phl_info->rx_new_pending = 1;
#endif
	pstatus = RTW_PHL_STATUS_SUCCESS;

out:
	_os_spinunlock(drv, &phl_info->rx_ring_lock, _bh, NULL);

	if(pstatus == RTW_PHL_STATUS_SUCCESS)
		_phl_record_rx_stats(recvpkt);

	FUNCOUT_WSTS(pstatus);

	return pstatus;
}

void
phl_sta_ps_enter(struct phl_info_t *phl_info, struct rtw_phl_stainfo_t *sta,
                 struct rtw_wifi_role_t *role)
{
	void *d = phl_to_drvpriv(phl_info);
	enum rtw_hal_status hal_status;
	struct rtw_phl_evt_ops *ops = &phl_info->phl_com->evt_ops;

	_os_atomic_set(d, &sta->ps_sta, 1);

	PHL_TRACE(COMP_PHL_PS, _PHL_INFO_,
	          "STA %02X:%02X:%02X:%02X:%02X:%02X enters PS mode, AID=%u, macid=%u, sta=0x%p\n",
	          sta->mac_addr[0], sta->mac_addr[1], sta->mac_addr[2],
	          sta->mac_addr[3], sta->mac_addr[4], sta->mac_addr[5],
	          sta->aid, sta->macid, sta);

	hal_status = rtw_hal_set_macid_pause(phl_info->hal,
	                                     sta->macid, true);
	if (RTW_HAL_STATUS_SUCCESS != hal_status) {
	        PHL_WARN("%s(): failed to pause macid tx, macid=%u\n",
	                 __FUNCTION__, sta->macid);
	}

	if (ops->ap_ps_sta_ps_change)
		ops->ap_ps_sta_ps_change(d, role->id, sta->mac_addr, true);
}

void
phl_sta_ps_exit(struct phl_info_t *phl_info, struct rtw_phl_stainfo_t *sta,
                struct rtw_wifi_role_t *role)
{
	void *d = phl_to_drvpriv(phl_info);
	enum rtw_hal_status hal_status;
	struct rtw_phl_evt_ops *ops = &phl_info->phl_com->evt_ops;

	PHL_TRACE(COMP_PHL_PS, _PHL_INFO_,
	          "STA %02X:%02X:%02X:%02X:%02X:%02X leaves PS mode, AID=%u, macid=%u, sta=0x%p\n",
	          sta->mac_addr[0], sta->mac_addr[1], sta->mac_addr[2],
	          sta->mac_addr[3], sta->mac_addr[4], sta->mac_addr[5],
	          sta->aid, sta->macid, sta);

	_os_atomic_set(d, &sta->ps_sta, 0);

	hal_status = rtw_hal_set_macid_pause(phl_info->hal,
	                                     sta->macid, false);
	if (RTW_HAL_STATUS_SUCCESS != hal_status) {
	        PHL_WARN("%s(): failed to resume macid tx, macid=%u\n",
	                 __FUNCTION__, sta->macid);
	}

	if (ops->ap_ps_sta_ps_change)
		ops->ap_ps_sta_ps_change(d, role->id, sta->mac_addr, false);
}

void
phl_rx_handle_sta_process(struct phl_info_t *phl_info,
                          struct rtw_phl_rx_pkt *rx)
{
	struct rtw_r_meta_data *m = &rx->r.mdata;
	struct rtw_wifi_role_t *role = NULL;
	struct rtw_phl_stainfo_t *sta = NULL;
	void *d = phl_to_drvpriv(phl_info);
	struct rtw_wifi_role_link_t *rlink = NULL;

	if (!phl_info->phl_com->dev_sw_cap.ap_ps)
		return;

	if (m->addr_cam_vld) {
		sta = rtw_phl_get_stainfo_by_macid(phl_info, m->macid);
		if (sta && sta->wrole) {
			role = sta->wrole;
			rlink = sta->rlink;
		}
	}

	if (!sta) { /* can be removed? */
		role = phl_get_wrole_by_addr(phl_info, m->mac_addr);
		if (role) {
			rlink = phl_get_rlink_by_hw_band(role, m->bb_sel);

			sta = rtw_phl_get_stainfo_by_addr(phl_info,
			                                  role, rlink, m->ta);
		}
	}

	if (!role || !sta)
		return;

	if (!rlink)
		return;

	rx->r.tx_sta = sta;
	rx->r.rx_role = role;
	rx->r.rx_rlink = rlink;

	PHL_TRACE(COMP_PHL_PS, _PHL_DEBUG_,
	          "ap-ps: more_frag=%u, frame_type=%u, role_type=%d, pwr_bit=%u, seq=%u\n",
	          m->more_frag, m->frame_type, role->type, m->pwr_bit, m->seq);

	/*
	 * Change STA PS state based on the PM bit in frame control
	 */
	if (!m->more_frag &&
	    (m->frame_type == RTW_FRAME_TYPE_DATA ||
	     m->frame_type == RTW_FRAME_TYPE_MGNT) &&
	    !m->rx_deferred_release &&
	     rtw_phl_role_is_ap_category(role)) {
		/* May get a @rx with macid set to our self macid, check if that
		 * happens here to avoid pausing self macid. This is put here so
		 * we wouldn't do it on our normal rx path, which degrades rx
		 * throughput significantly. */
		if (phl_self_stainfo_chk(phl_info, role, rlink, sta))
			return;

		if (_os_atomic_read(d, &sta->ps_sta)) {
			if (!m->pwr_bit)
				phl_sta_ps_exit(phl_info, sta, role);
		} else {
			if (m->pwr_bit)
				phl_sta_ps_enter(phl_info, sta, role);
		}
	}
}

void
phl_handle_rx_frame_list(struct phl_info_t *phl_info,
                         _os_list *frames)
{
	struct rtw_phl_rx_pkt *pos, *n;
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;
	struct phl_hci_trx_ops *hci_trx_ops = phl_info->hci_trx_ops;

	phl_list_for_loop_safe(pos, n, struct rtw_phl_rx_pkt, frames, list) {
		list_del(&pos->list);
		phl_rx_handle_sta_process(phl_info, pos);
		status = _phl_add_rx_pkt(phl_info, pos);
		if (RTW_PHL_STATUS_RESOURCE == status) {
			hci_trx_ops->recycle_rx_pkt(phl_info, pos);
		}
	}
#ifndef PHL_RX_BATCH_IND
	_phl_indic_new_rxpkt(phl_info);
#endif

}


#define SEQ_MODULO 0x1000
#define SEQ_MASK	0xfff

static inline int seq_less(u16 sq1, u16 sq2)
{
	return ((sq1 - sq2) & SEQ_MASK) > (SEQ_MODULO >> 1);
}

static inline u16 seq_inc(u16 sq)
{
	return (sq + 1) & SEQ_MASK;
}

static inline u16 seq_sub(u16 sq1, u16 sq2)
{
	return (sq1 - sq2) & SEQ_MASK;
}

static inline u16 reorder_index(struct phl_tid_ampdu_rx *r, u16 seq)
{
	return seq % r->buf_size;
}

static void phl_release_reorder_frame(struct phl_info_t *phl_info,
                                      struct phl_tid_ampdu_rx *r,
                                      int index, _os_list *frames)
{
	struct rtw_phl_rx_pkt *pkt = r->reorder_buf[index];
	struct rtw_r_meta_data *meta;

	if (!pkt)
		goto out;

	meta = &pkt->r.mdata;

	/* release the frame from the reorder ring buffer */
	r->stored_mpdu_num--;
	r->reorder_buf[index] = NULL;
	meta->rx_deferred_release = 1;
	list_add_tail(&pkt->list, frames);

out:
	r->head_seq_num = seq_inc(r->head_seq_num);
}

#define HT_RX_REORDER_BUF_TIMEOUT_MS 500

/*
 * If the MPDU at head_seq_num is ready,
 *     1. release all subsequent MPDUs with consecutive SN and
 *     2. if there's MPDU that is ready but left in the reordering
 *        buffer, find it and set reorder timer according to its reorder
 *        time
 *
 * If the MPDU at head_seq_num is not ready and there is no MPDU ready
 * in the buffer at all, return.
 *
 * If the MPDU at head_seq_num is not ready but there is some MPDU in
 * the buffer that is ready, check whether any frames in the reorder
 * buffer have timed out in the following way.
 *
 * Basically, MPDUs that are not ready are purged and MPDUs that are
 * ready are released.
 *
 * The process goes through all the buffer but the one at head_seq_num
 * unless
 *     - there's a MPDU that is ready AND
 *     - there are one or more buffers that are not ready.
 * In this case, the process is stopped, the head_seq_num becomes the
 * first buffer that is not ready and the reorder_timer is reset based
 * on the reorder_time of that ready MPDU.
 */
static void phl_reorder_release(struct phl_info_t *phl_info,
								struct phl_tid_ampdu_rx *r, _os_list *frames)
{
	/* ref ieee80211_sta_reorder_release() and wil_reorder_release() */

	int index, j;

	/* release the buffer until next missing frame */
	index = reorder_index(r, r->head_seq_num);
	if (!r->reorder_buf[index] && r->stored_mpdu_num) {
		/*
		 * No buffers ready to be released, but check whether any
		 * frames in the reorder buffer have timed out.
		 */
		u32 cur_time = _os_get_cur_time_ms();
		int skipped = 1;
		for (j = (index + 1) % r->buf_size; j != index;
			j = (j + 1) % r->buf_size) {
			if (!r->reorder_buf[j]) {
				skipped++;
				continue;
			}
			if (skipped && (s32)(r->reorder_time[j] +
				HT_RX_REORDER_BUF_TIMEOUT_MS - cur_time) > 0)
				goto set_release_timer;

			PHL_TRACE(COMP_PHL_RECV, _PHL_INFO_, "release an RX reorder frame due to timeout on earlier frames\n");

			phl_release_reorder_frame(phl_info, r, j, frames);

			/*
			 * Increment the head seq# also for the skipped slots.
			 */
			r->head_seq_num =
				(r->head_seq_num + skipped) & SEQ_MASK;
			skipped = 0;
		}
	} else while (r->reorder_buf[index]) {
		phl_release_reorder_frame(phl_info, r, index, frames);
		index = reorder_index(r, r->head_seq_num);
	}

	if (r->stored_mpdu_num) {

set_release_timer:

		if (!r->removed)
			_os_set_timer(r->drv_priv, &r->sta->reorder_timer,
			              HT_RX_REORDER_BUF_TIMEOUT_MS);
	}
}

void phl_sta_rx_reorder_timer_expired(void *t)
{
	/* ref sta_rx_agg_reorder_timer_expired() */

	struct rtw_phl_stainfo_t *sta = (struct rtw_phl_stainfo_t *)t;
	struct rtw_phl_com_t *phl_com = sta->wrole->phl_com;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl_com->phl_priv;
	void *drv_priv = phl_to_drvpriv(phl_info);
	_os_list frames;
	u8 i = 0;

	PHL_INFO("Rx reorder timer expired, sta=0x%p\n", sta);

	INIT_LIST_HEAD(&frames);

	_os_spinlock(drv_priv, &sta->tid_rx_lock, _bh, NULL);
	for (i = 0; i < ARRAY_SIZE(sta->tid_rx); i++) {
		if (sta->tid_rx[i])
			phl_reorder_release(phl_info, sta->tid_rx[i], &frames);
	}
	_os_spinunlock(drv_priv, &sta->tid_rx_lock, _bh, NULL);

	phl_handle_rx_frame_list(phl_info, &frames);
#ifdef PHL_RX_BATCH_IND
	_phl_indic_new_rxpkt(phl_info);
#endif

	_os_event_set(drv_priv, &sta->comp_sync);
}

static void phl_release_reorder_frames(struct phl_info_t *phl_info,
										struct phl_tid_ampdu_rx *r,
										u16 head_seq_num, _os_list *frames)
{
	/* ref ieee80211_release_reorder_frames() and
		wil_release_reorder_frames() */

	int index;

	/* note: this function is never called with
	 * hseq preceding r->head_seq_num, i.e it is always true
	 * !seq_less(hseq, r->head_seq_num)
	 * and thus on loop exit it should be
	 * r->head_seq_num == hseq
	 */
	while (seq_less(r->head_seq_num, head_seq_num) &&
		r->stored_mpdu_num) { /* Note: do we need to check this? */
		index = reorder_index(r, r->head_seq_num);
		phl_release_reorder_frame(phl_info, r, index, frames);
	}
	r->head_seq_num = head_seq_num;
}

void rtw_phl_flush_reorder_buf(void *phl, struct rtw_phl_stainfo_t *sta)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	void *drv_priv = phl_to_drvpriv(phl_info);
	_os_list frames;
	u8 i = 0;

	PHL_INFO("%s: sta=0x%p\n", __FUNCTION__, sta);

	INIT_LIST_HEAD(&frames);

	_os_spinlock(drv_priv, &sta->tid_rx_lock, _bh, NULL);
	for (i = 0; i < ARRAY_SIZE(sta->tid_rx); i++) {
		if (sta->tid_rx[i])
			phl_reorder_release(phl_info, sta->tid_rx[i], &frames);
	}
	_os_spinunlock(drv_priv, &sta->tid_rx_lock, _bh, NULL);

	phl_handle_rx_frame_list(phl_info, &frames);
#ifdef PHL_RX_BATCH_IND
	_phl_indic_new_rxpkt(phl_info);
#endif

}

#ifdef PHL_RXSC_AMPDU
static void _phl_rxsc_cache_entry(struct phl_info_t *phl_info,
								struct phl_tid_ampdu_rx *r,
								struct rtw_r_meta_data *meta)
{
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct rtw_rxsc_cache_entry *rxsc_entry = &phl_com->rxsc_entry;

	_os_spinlock(phl_com->drv_priv, &rxsc_entry->rxsc_lock, _bh, NULL);
	rxsc_entry->cached_rx_macid = meta->macid;
	rxsc_entry->cached_rx_tid = meta->tid;
	rxsc_entry->cached_rx_ppdu_cnt = meta->ppdu_cnt;
	rxsc_entry->cached_rx_seq = meta->seq;
	rxsc_entry->cached_r = r;
	_os_spinunlock(phl_com->drv_priv, &rxsc_entry->rxsc_lock, _bh, NULL);

#ifdef DEBUG_PHL_RX
	phl_info->rx_stats.rxsc_ampdu[2]++;
#endif
}

static bool _phl_rxsc_cache_check(struct phl_info_t *phl_info,
									struct rtw_phl_rx_pkt *phl_rx,
									struct rtw_r_meta_data *meta)
{
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct rtw_rxsc_cache_entry *rxsc_entry = &phl_com->rxsc_entry;
	struct phl_tid_ampdu_rx *r;
	u8 res = false;

	if (PHL_MACID_MAX_NUM != rxsc_entry->cached_rx_macid) {
		_os_spinlock(phl_com->drv_priv,
				&rxsc_entry->rxsc_lock, _bh, NULL);
		r = rxsc_entry->cached_r;
		if (r) {
			r->head_seq_num = seq_inc(r->head_seq_num);
			phl_rx->r.rx_role = r->sta->wrole;
			res = true;
		} else {
			PHL_ERR("[%s]RXSC: cached_r is NULL!\n", __func__);
			/* reset cached macid & sta due to illegal cached_r */
			rxsc_entry->cached_rx_macid = PHL_MACID_MAX_NUM;
			res = false;
		}
		_os_spinunlock(phl_com->drv_priv,
				&rxsc_entry->rxsc_lock, _bh, NULL);
#ifdef DEBUG_PHL_RX
		phl_info->rx_stats.rxsc_ampdu[1]++;
#endif
	} else {
#ifdef DEBUG_PHL_RX
		if (meta->ampdu)
			phl_info->rx_stats.rxsc_ampdu[0]++;
#endif
	}

	return res;
}
#endif

static bool phl_manage_sta_reorder_buf(struct phl_info_t *phl_info,
                                       struct rtw_phl_rx_pkt *pkt,
                                       struct phl_tid_ampdu_rx *r,
                                       _os_list *frames)
{
	/* ref ieee80211_sta_manage_reorder_buf() and wil_rx_reorder() */

	struct rtw_r_meta_data *meta = &pkt->r.mdata;
	u16 mpdu_seq_num = meta->seq;
	u16 head_seq_num, buf_size;
	int index;
	struct phl_hci_trx_ops *hci_trx_ops = phl_info->hci_trx_ops;

	buf_size = r->buf_size;
	head_seq_num = r->head_seq_num;

	/*
	 * If the current MPDU's SN is smaller than the SSN, it shouldn't
	 * be reordered.
	 */
	if (!r->started) {
		if (seq_less(mpdu_seq_num, head_seq_num))
			return false;
		r->started = true;
	}

	if (r->sleep) {
		PHL_INFO("tid = %d reorder buffer handling after wake up\n",
		         r->tid);
		PHL_INFO("Update head seq(0x%03x) to the first rx seq(0x%03x) after wake up\n",
		         r->head_seq_num, mpdu_seq_num);
		r->head_seq_num = mpdu_seq_num;
		head_seq_num = r->head_seq_num;
		r->sleep = false;
	}

	/* frame with out of date sequence number */
	if (seq_less(mpdu_seq_num, head_seq_num)) {
		PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "Rx drop: old seq 0x%03x head 0x%03x\n",
				meta->seq, r->head_seq_num);
		hci_trx_ops->recycle_rx_pkt(phl_info, pkt);
#ifdef DEBUG_PHL_RX
		phl_info->rx_stats.rx_drop_reorder++;
		phl_info->rx_stats.reorder_seq_less++;
#endif
		return true;
	}

	/*
	 * If frame the sequence number exceeds our buffering window
	 * size release some previous frames to make room for this one.
	 */
	if (!seq_less(mpdu_seq_num, head_seq_num + buf_size)) {
		head_seq_num = seq_inc(seq_sub(mpdu_seq_num, buf_size));
		/* release stored frames up to new head to stack */
		phl_release_reorder_frames(phl_info, r, head_seq_num, frames);
	}

	/* Now the new frame is always in the range of the reordering buffer */

	index = reorder_index(r, mpdu_seq_num);

	/* check if we already stored this frame */
	if (r->reorder_buf[index]) {
		PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "Rx drop: old seq 0x%03x head 0x%03x\n",
				meta->seq, r->head_seq_num);
		hci_trx_ops->recycle_rx_pkt(phl_info, pkt);
#ifdef DEBUG_PHL_RX
		phl_info->rx_stats.rx_drop_reorder++;
		phl_info->rx_stats.reorder_dup++;
#endif
		return true;
	}

	/*
	 * If the current MPDU is in the right order and nothing else
	 * is stored we can process it directly, no need to buffer it.
	 * If it is first but there's something stored, we may be able
	 * to release frames after this one.
	 */
	if (mpdu_seq_num == r->head_seq_num &&
		r->stored_mpdu_num == 0) {
		r->head_seq_num = seq_inc(r->head_seq_num);
		#ifdef PHL_RXSC_AMPDU
		_phl_rxsc_cache_entry(phl_info, r, meta);
		#endif
		return false;
	}

	/* put the frame in the reordering buffer */
	r->reorder_buf[index] = pkt;
	r->reorder_time[index] = _os_get_cur_time_ms();
	r->stored_mpdu_num++;
	phl_reorder_release(phl_info, r, frames);
#ifdef DEBUG_PHL_RX
	phl_info->rx_stats.rx_put_reorder++;
#endif

	return true;

}

enum rtw_phl_status phl_rx_reorder(struct phl_info_t *phl_info,
                                   struct rtw_phl_rx_pkt *phl_rx,
                                   _os_list *frames)
{
	/* ref wil_rx_reorder() and ieee80211_rx_reorder_ampdu() */

	void *drv_priv = phl_to_drvpriv(phl_info);
	struct rtw_r_meta_data *meta = &phl_rx->r.mdata;
	u16 tid = meta->tid;
	struct rtw_phl_stainfo_t *sta = NULL;
	struct phl_tid_ampdu_rx *r;
	struct phl_hci_trx_ops *hci_trx_ops = phl_info->hci_trx_ops;

	/*
	 * Remove FCS if is is appended
	 * TODO: handle more than one in pkt_list
	 */
	if (phl_info->phl_com->append_fcs) {
		/*
		 * Only last MSDU of A-MSDU includes FCS.
		 * TODO: If A-MSDU cut processing is in HAL, should only deduct
		 * FCS from length of last one of pkt_list. For such case,
		 * phl_rx->r should have pkt_list length.
		 */
		  if (!(meta->amsdu_cut && !meta->last_msdu)) {
			  if (phl_rx->r.pkt_list[0].length <= 4) {
				  PHL_ERR("%s, pkt_list[0].length(%d) too short\n",
				          __func__, phl_rx->r.pkt_list[0].length);
				  goto drop_frame;
			  }
			  phl_rx->r.pkt_list[0].length -= 4;
		  }
	}

	#ifdef PHL_RXSC_AMPDU
	if (_phl_rxsc_cache_check(phl_info, phl_rx, meta))
		goto dont_reorder;
	#endif

	if (phl_is_mp_mode(phl_info->phl_com))
		goto dont_reorder;

	if (meta->bc || meta->mc)
		goto dont_reorder;

	if (!meta->qos)
		goto dont_reorder;

	if (meta->q_null)
		goto dont_reorder;

	/* TODO: check ba policy is either ba or normal */

	/* if the mpdu is fragmented, don't reorder */
	if (meta->more_frag || meta->frag_num) {
		PHL_TRACE(COMP_PHL_RECV, _PHL_ERR_,
		          "Receive QoS Data with more_frag=%u, frag_num=%u\n",
		          meta->more_frag, meta->frag_num);
		goto dont_reorder;
	}

	/* Use MAC ID from address CAM if this packet is address CAM matched */
	if (meta->addr_cam_vld)
		sta = rtw_phl_get_stainfo_by_macid(phl_info, meta->macid);

	if (!sta) {
		PHL_TRACE(COMP_PHL_RECV, _PHL_WARNING_,
		          "%s(): stainfo not found, cam=%u, macid=%u\n",
		          __FUNCTION__, meta->addr_cam, meta->macid);
		goto dont_reorder;
	}

	phl_rx->r.tx_sta = sta;
	phl_rx->r.rx_role = sta->wrole;

	rtw_hal_set_sta_rx_sts(sta, false, meta);

	if (tid >= ARRAY_SIZE(sta->tid_rx)) {
		PHL_TRACE(COMP_PHL_RECV, _PHL_ERR_, "Fail: tid (%u) index out of range (%u)\n",
			tid, (u32)ARRAY_SIZE(sta->tid_rx));
		goto dont_reorder;
	}

	_os_spinlock(drv_priv, &sta->tid_rx_lock, _bh, NULL);

	r = sta->tid_rx[tid];
	if (!r) {
		_os_spinunlock(drv_priv, &sta->tid_rx_lock, _bh, NULL);
		goto dont_reorder;
	}

	if (!phl_manage_sta_reorder_buf(phl_info, phl_rx, r, frames)) {
		_os_spinunlock(drv_priv, &sta->tid_rx_lock, _bh, NULL);
		goto dont_reorder;
	}

	_os_spinunlock(drv_priv, &sta->tid_rx_lock, _bh, NULL);

	return RTW_PHL_STATUS_SUCCESS;

drop_frame:
#ifdef DEBUG_PHL_RX
	phl_info->rx_stats.rx_drop_reorder++;
#endif
	hci_trx_ops->recycle_rx_pkt(phl_info, phl_rx);
	return RTW_PHL_STATUS_FAILURE;

dont_reorder:
#ifdef DEBUG_PHL_RX
	phl_info->rx_stats.rx_dont_reorder++;
#endif
	list_add_tail(&phl_rx->list, frames);
	return RTW_PHL_STATUS_SUCCESS;
}


u8 phl_check_recv_ring_resource(struct phl_info_t *phl_info)
{
	struct rtw_phl_rx_ring *ring = &phl_info->phl_rx_ring;
	u16 avail = 0, wptr = 0, rptr = 0;
	void *drv_priv = phl_to_drvpriv(phl_info);

	wptr = (u16)_os_atomic_read(drv_priv, &ring->phl_idx);
	rptr = (u16)_os_atomic_read(drv_priv, &ring->core_idx);
	avail = phl_calc_avail_wptr(rptr, wptr, MAX_PHL_RX_RING_ENTRY_NUM);

	if (0 == avail)
		return false;
	else
		return true;
}

void dump_phl_rx_ring(void *phl)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	void *drv_priv = phl_to_drvpriv(phl_info);
	s16	diff = 0;
	u16 idx = 0, endidx = 0;
	u16 phl_idx = 0, core_idx = 0;

	PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "===Dump PHL RX Ring===\n");
	phl_idx = (u16)_os_atomic_read(drv_priv, &phl_info->phl_rx_ring.phl_idx);
	core_idx = (u16)_os_atomic_read(drv_priv, &phl_info->phl_rx_ring.core_idx);
	PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_,
			"core_idx = %d\n"
			"phl_idx = %d\n",
			core_idx,
			phl_idx);

	diff= phl_idx-core_idx;
	if(diff < 0)
		diff= 4096+diff;

	endidx = diff > 5 ? (core_idx+6): phl_idx;
	for (idx = core_idx+1; idx < endidx; idx++) {
		PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "entry[%d] = %p\n", idx,
				phl_info->phl_rx_ring.entry[idx%4096]);
	}
}


void phl_event_indicator(void *context)
{
	enum rtw_phl_status sts = RTW_PHL_STATUS_FAILURE;
	struct rtw_phl_handler *phl_handler
		= (struct rtw_phl_handler *)phl_container_of(context,
							struct rtw_phl_handler,
							os_handler);
	struct phl_info_t *phl_info = (struct phl_info_t *)phl_handler->context;
	struct rtw_phl_evt_ops *ops = NULL;
	struct rtw_evt_info_t *evt_info = NULL;
	void *drv_priv = NULL;
	enum rtw_phl_evt evt_bitmap = 0;
	FUNCIN_WSTS(sts);

	if (NULL != phl_info) {
		ops = &phl_info->phl_com->evt_ops;
		evt_info = &phl_info->phl_com->evt_info;
		drv_priv = phl_to_drvpriv(phl_info);

		_os_spinlock(drv_priv, &evt_info->evt_lock, _bh, NULL);
		evt_bitmap = evt_info->evt_bitmap;
		evt_info->evt_bitmap = 0;
		_os_spinunlock(drv_priv, &evt_info->evt_lock, _bh, NULL);

		if (RTW_PHL_EVT_RX & evt_bitmap) {
			if (NULL != ops->rx_process) {
				sts = ops->rx_process(drv_priv);
			}
			dump_phl_rx_ring(phl_info);
		}
	}
	FUNCOUT_WSTS(sts);

}

void _phl_rx_statistics_reset(struct phl_info_t *phl_info)
{
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct rtw_phl_stainfo_t *sta = NULL;
	struct rtw_wifi_role_t *role = NULL;
	void *drv = phl_to_drvpriv(phl_info);
	struct phl_queue *sta_queue;
	u8 i;
	u8 idx = 0;
	struct rtw_wifi_role_link_t *rlink = NULL;

	for (i = 0; i< MAX_WIFI_ROLE_NUMBER; i++) {
		role = &phl_com->wifi_roles[i];
		if (role->active && (role->mstate == MLME_LINKED)) {
			for (idx = 0; idx < role->rlink_num; idx++) {
				rlink = get_rlink(role,idx);
				sta_queue = &rlink->assoc_sta_queue;

				_os_spinlock(drv, &sta_queue->lock, _bh, NULL);
				phl_list_for_loop(sta, struct rtw_phl_stainfo_t,
							&sta_queue->queue, list) {
					if (sta)
						rtw_hal_set_sta_rx_sts(sta, true, NULL);
				}
				_os_spinunlock(drv, &sta_queue->lock, _bh, NULL);
			}
		}
	}
}

void
phl_rx_watchdog(struct phl_info_t *phl_info)
{
	struct rtw_stats *phl_stats = &phl_info->phl_com->phl_stats;

	phl_rx_traffic_upd(phl_stats);
	phl_dump_rx_stats(phl_stats);
	_phl_rx_statistics_reset(phl_info);
}

u16 rtw_phl_query_new_rx_num(void *phl)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct rtw_phl_rx_ring *ring = NULL;
	u16 new_rx = 0, wptr = 0, rptr = 0;

	if (NULL != phl_info) {
		ring = &phl_info->phl_rx_ring;
		wptr = (u16)_os_atomic_read(phl_to_drvpriv(phl_info),
						&ring->phl_idx);
		rptr = (u16)_os_atomic_read(phl_to_drvpriv(phl_info),
						&ring->core_idx);
		new_rx = phl_calc_avail_rptr(rptr, wptr,
						MAX_PHL_RX_RING_ENTRY_NUM);
	}

	return new_rx;
}

struct rtw_recv_pkt *rtw_phl_query_rx_pkt(void *phl)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct rtw_phl_rx_ring *ring = NULL;
	struct rtw_recv_pkt *recvpkt = NULL;
	void *drv_priv = NULL;
	u16 ring_res = 0, wptr = 0, rptr = 0;

	if (NULL != phl_info) {
		ring = &phl_info->phl_rx_ring;
		drv_priv = phl_to_drvpriv(phl_info);

		wptr = (u16)_os_atomic_read(drv_priv, &ring->phl_idx);
		rptr = (u16)_os_atomic_read(drv_priv, &ring->core_idx);

		ring_res = phl_calc_avail_rptr(rptr, wptr,
							MAX_PHL_RX_RING_ENTRY_NUM);

		PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_,
			"[4] %s::[Query] phl_idx =%d , core_idx =%d , ring_res =%d\n",
			__FUNCTION__,
			_os_atomic_read(drv_priv, &ring->phl_idx),
			_os_atomic_read(drv_priv, &ring->core_idx),
			ring_res);

		if (ring_res > 0) {
			rptr = rptr + 1;

			if (rptr >= MAX_PHL_RX_RING_ENTRY_NUM) {
				rptr=0;
				recvpkt = (struct rtw_recv_pkt *)ring->entry[rptr];
				ring->entry[rptr]=NULL;
				_os_atomic_set(drv_priv, &ring->core_idx, 0);
			} else {
				recvpkt = (struct rtw_recv_pkt *)ring->entry[rptr];
				ring->entry[rptr]=NULL;
				_os_atomic_inc(drv_priv, &ring->core_idx);
			}
			if (NULL == recvpkt)
				PHL_TRACE(COMP_PHL_RECV, _PHL_WARNING_, "recvpkt is NULL!\n");
			else
				phl_rx_statistics(phl_info, recvpkt);
		} else {
			PHL_TRACE(COMP_PHL_RECV, _PHL_INFO_, "no available rx packet to query!\n");
		}
	}

	return recvpkt;
}

enum rtw_phl_status rtw_phl_return_rxbuf(void *phl, u8* recvpkt)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct rtw_phl_rx_pkt *phl_rx = NULL;
	struct rtw_recv_pkt *r = (struct rtw_recv_pkt *)recvpkt;

	do {
		if (NULL == recvpkt)
			break;

		phl_rx = phl_container_of(r, struct rtw_phl_rx_pkt, r);
		phl_recycle_rx_buf(phl_info, phl_rx);
		pstatus = RTW_PHL_STATUS_SUCCESS;
	} while (false);

	return pstatus;
}


enum rtw_phl_status rtw_phl_start_rx_process(void *phl)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;

	FUNCIN_WSTS(pstatus);

	pstatus = phl_schedule_handler(phl_info->phl_com,
	                               &phl_info->phl_rx_handler);

	FUNCOUT_WSTS(pstatus);

	return pstatus;
}

void rtw_phl_rx_bar(void *phl, struct rtw_phl_stainfo_t *sta, u8 tid, u16 seq)
{
	/* ref ieee80211_rx_h_ctrl() and wil_rx_bar() */

	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	void *drv_priv = phl_to_drvpriv(phl_info);
	struct phl_tid_ampdu_rx *r;
	_os_list frames;

	if (tid >= RTW_MAX_TID_NUM)
		return;

	INIT_LIST_HEAD(&frames);

	_os_spinlock(drv_priv, &sta->tid_rx_lock, _bh, NULL);

	r = sta->tid_rx[tid];
	if (!r) {
		_os_spinunlock(drv_priv, &sta->tid_rx_lock, _bh, NULL);
		PHL_TRACE(COMP_PHL_RECV, _PHL_ERR_, "BAR for non-existing TID %d\n", tid);
		return;
	}

	if (seq_less(seq, r->head_seq_num)) {
		_os_spinunlock(drv_priv, &sta->tid_rx_lock, _bh, NULL);
		PHL_TRACE(COMP_PHL_RECV, _PHL_ERR_, "BAR Seq 0x%03x preceding head 0x%03x\n",
					seq, r->head_seq_num);
		return;
	}

	PHL_TRACE(COMP_PHL_RECV, _PHL_INFO_, "BAR: TID %d Seq 0x%03x head 0x%03x\n",
				tid, seq, r->head_seq_num);

	phl_release_reorder_frames(phl_info, r, seq, &frames);

	_os_spinunlock(drv_priv, &sta->tid_rx_lock, _bh, NULL);

	phl_handle_rx_frame_list(phl_info, &frames);
}

enum rtw_phl_status
rtw_phl_enter_mon_mode(void *phl, struct rtw_wifi_role_t *wrole)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	enum rtw_hal_status status;
	struct rtw_wifi_role_link_t *rlink = NULL;
	u8 idx = 0, rollback = 0;

	for (idx = 0; idx < wrole->rlink_num; idx++) {
		rlink = get_rlink(wrole, idx);

		status = rtw_hal_enter_mon_mode(phl_info->hal, rlink->hw_band);
		if (status != RTW_HAL_STATUS_SUCCESS) {
			PHL_TRACE(COMP_PHL_RECV, _PHL_ERR_,
			          "%s(): rtw_hal_enter_mon_mode() failed, status=%d",
			          __FUNCTION__, status);

			for (rollback = idx; rollback > 0; rollback--) {
				rlink = &wrole->rlink[rollback-1];
				rtw_hal_leave_mon_mode(phl_info->hal, rlink->hw_band);
			}
			return RTW_PHL_STATUS_FAILURE;
		}
	}

	return RTW_PHL_STATUS_SUCCESS;
}

enum rtw_phl_status
rtw_phl_leave_mon_mode(void *phl, struct rtw_wifi_role_t *wrole)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	enum rtw_hal_status status;
	struct rtw_wifi_role_link_t *rlink = NULL;
	u8 idx = 0, rollback = 0;

	for (idx = 0; idx < wrole->rlink_num; idx++) {
		rlink = get_rlink(wrole, idx);

		status = rtw_hal_leave_mon_mode(phl_info->hal, rlink->hw_band);
		if (status != RTW_HAL_STATUS_SUCCESS) {
			PHL_TRACE(COMP_PHL_RECV, _PHL_ERR_,
			          "%s(): rtw_hal_leave_mon_mode() failed, status=%d",
			          __FUNCTION__, status);

			for (rollback = idx; rollback > 0; rollback--) {
				rlink = &wrole->rlink[rollback-1];
				rtw_hal_enter_mon_mode(phl_info->hal, rlink->hw_band);
			}
			return RTW_PHL_STATUS_FAILURE;
		}
	}

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status
_rtw_phl_get_rx_cnt_by_idx(struct phl_info_t *phl_info,
			   enum phl_band_idx hw_band,
			   enum phl_rxcnt_idx idx,
			   u16 *rx_cnt)
{
	enum rtw_phl_status psts = RTW_PHL_STATUS_SUCCESS;
	enum rtw_hal_status hsts;

	hsts = rtw_hal_get_rx_cnt_by_idx(phl_info->hal, hw_band, idx, rx_cnt);

	if (hsts != RTW_HAL_STATUS_SUCCESS)
		psts = RTW_PHL_STATUS_FAILURE;

	return psts;
}

static enum rtw_phl_status
_rtw_phl_set_reset_rx_cnt(struct phl_info_t *phl_info,
			  enum phl_band_idx hw_band)
{
	enum rtw_phl_status psts = RTW_PHL_STATUS_SUCCESS;
	enum rtw_hal_status hsts;

	hsts = rtw_hal_set_reset_rx_cnt(phl_info->hal, hw_band);

	if (hsts != RTW_HAL_STATUS_SUCCESS)
		psts = RTW_PHL_STATUS_FAILURE;

	return psts;
}

struct cmd_rx_cnt_param {
	enum phl_band_idx hw_band;
	enum phl_rxcnt_idx idx;
	u16 *rx_cnt;
};

static void _phl_cmd_rx_cnt_done(void *drv_priv,
				 u8 *cmd,
				 u32 cmd_len,
				 enum rtw_phl_status status)
{
	struct cmd_rx_cnt_param *param = (struct cmd_rx_cnt_param *)cmd;

	if (param)
		_os_kmem_free(drv_priv, param, cmd_len);
}

enum rtw_phl_status
phl_cmd_get_rx_cnt_by_idx_hdl(struct phl_info_t *phl_info, u8 *cmd)
{
	struct cmd_rx_cnt_param *param = (struct cmd_rx_cnt_param *)cmd;

	return _rtw_phl_get_rx_cnt_by_idx(phl_info, param->hw_band, param->idx,
					  param->rx_cnt);
}

enum rtw_phl_status
phl_cmd_set_reset_rx_cnt_hdl(struct phl_info_t *phl_info, u8 *cmd)
{
	struct cmd_rx_cnt_param *param = (struct cmd_rx_cnt_param *)cmd;

	return _rtw_phl_set_reset_rx_cnt(phl_info, param->hw_band);
}

enum rtw_phl_status
rtw_phl_cmd_get_rx_cnt_by_idx(void *phl,
			      enum phl_band_idx hw_band,
			      enum phl_rxcnt_idx idx,
			      u16 *rx_cnt,
			      enum phl_cmd_type cmd_type,
			      u32 cmd_timeout)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	void *drv = phl_to_drvpriv(phl_info);
	enum rtw_phl_status psts = RTW_PHL_STATUS_FAILURE;
	struct cmd_rx_cnt_param *param = NULL;
	u32 param_len = 0;

	if (cmd_type == PHL_CMD_DIRECTLY) {
		psts = _rtw_phl_get_rx_cnt_by_idx(phl_info, hw_band, idx, rx_cnt);
		goto _exit;
	}

	param_len = sizeof(struct cmd_rx_cnt_param);
	param = _os_kmem_alloc(drv, param_len);
	if (param == NULL) {
		PHL_ERR("%s: alloc param failed!\n", __func__);
		psts = RTW_PHL_STATUS_RESOURCE;
		goto _exit;
	}

	_os_mem_set(drv, param, 0, param_len);
	param->hw_band = hw_band;
	param->idx = idx;
	param->rx_cnt = rx_cnt;

	psts = phl_cmd_enqueue(phl_info,
			       hw_band,
			       MSG_EVT_RX_DBG_CNT_GET_BY_IDX,
			       (u8 *)param,
			       param_len,
			       _phl_cmd_rx_cnt_done,
			       cmd_type,
			       cmd_timeout);
	if (is_cmd_failure(psts)) {
		/* Send cmd success, but wait cmd fail*/
		psts = RTW_PHL_STATUS_FAILURE;
	} else if (psts != RTW_PHL_STATUS_SUCCESS) {
		/* Send cmd fail */
		psts = RTW_PHL_STATUS_FAILURE;
		_os_kmem_free(drv, param, param_len);
	}

_exit:
	return psts;
}

enum rtw_phl_status
rtw_phl_cmd_set_reset_rx_cnt(void *phl,
			     enum phl_band_idx hw_band,
			     enum phl_cmd_type cmd_type,
			     u32 cmd_timeout)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	void *drv = phl_to_drvpriv(phl_info);
	enum rtw_phl_status psts = RTW_PHL_STATUS_FAILURE;
	struct cmd_rx_cnt_param *param = NULL;
	u32 param_len = 0;

	if (cmd_type == PHL_CMD_DIRECTLY) {
		psts = _rtw_phl_set_reset_rx_cnt(phl_info, hw_band);
		goto _exit;
	}

	param_len = sizeof(struct cmd_rx_cnt_param);
	param = _os_kmem_alloc(drv, param_len);
	if (param == NULL) {
		PHL_ERR("%s: alloc param failed!\n", __func__);
		psts = RTW_PHL_STATUS_RESOURCE;
		goto _exit;
	}

	_os_mem_set(drv, param, 0, param_len);
	param->hw_band = hw_band;

	psts = phl_cmd_enqueue(phl_info,
			       hw_band,
			       MSG_EVT_RX_DBG_CNT_RESET,
			       (u8 *)param,
			       param_len,
			       _phl_cmd_rx_cnt_done,
			       cmd_type,
			       cmd_timeout);
	if (is_cmd_failure(psts)) {
		/* Send cmd success, but wait cmd fail*/
		psts = RTW_PHL_STATUS_FAILURE;
	} else if (psts != RTW_PHL_STATUS_SUCCESS) {
		/* Send cmd fail */
		psts = RTW_PHL_STATUS_FAILURE;
		_os_kmem_free(drv, param, param_len);
	}

_exit:
	return psts;
}

#ifdef CONFIG_PHL_RX_PSTS_PER_PKT
void
_phl_rx_proc_frame_list(struct phl_info_t *phl_info, struct phl_queue *pq)
{
	void *d = phl_to_drvpriv(phl_info);
	_os_list *pkt_list = NULL;
	struct rtw_phl_rx_pkt *phl_rx = NULL;

	if (NULL == pq)
		return;
	if (0 == pq->cnt)
		return;

	PHL_TRACE(COMP_PHL_PSTS, _PHL_INFO_,
		  "_phl_rx_proc_frame_list : queue ele cnt = %d\n",
		   pq->cnt);

	while (true == pq_pop(d, pq, &pkt_list, _first, _bh)) {
		phl_rx = (struct rtw_phl_rx_pkt *)pkt_list;
		phl_info->hci_trx_ops->rx_handle_normal(phl_info, phl_rx);
	}
}

void
_phl_rx_copy_phy_sts(struct rtw_phl_ppdu_phy_info *src, struct rtw_phl_ppdu_phy_info *dest)
{
	u8 i = 0;

	dest->is_valid = src->is_valid;
	dest->is_pkt_with_data = src->is_pkt_with_data;
	dest->rssi = src->rssi;
	for (i = 0; i < RTW_PHL_MAX_RF_PATH; i++) {
		dest->rssi_path[i] = src->rssi_path[i];
		dest->snr_fd[i] = src->snr_fd[i];
		dest->snr_td[i] = src->snr_td[i];
	}
	/* dest->ch_idx = src->ch_idx; */
	dest->tx_bf = src->tx_bf;
	dest->frame_type = src->frame_type;
	dest->snr_fd_avg = src->snr_fd_avg;
	dest->snr_td_avg = src->snr_td_avg;
}

enum rtw_phl_status
phl_rx_proc_phy_sts(struct phl_info_t *phl_info, struct rtw_phl_rx_pkt *ppdu_sts)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	struct rtw_phl_ppdu_sts_info *psts_info = &(phl_info->phl_com->ppdu_sts_info);
	struct rtw_phl_ppdu_sts_ent *sts_entry = NULL;
	struct rtw_phl_rx_pkt *phl_rx = NULL;
	void *d = phl_to_drvpriv(phl_info);
	struct rtw_phl_rssi_stat *rssi_stat = &phl_info->phl_com->rssi_stat;
	_os_list *frame = NULL;
	bool upt_psts = true;
	u8 i = 0;
	enum phl_band_idx band = HW_BAND_0;

	if (NULL == ppdu_sts)
		return pstatus;

	if (false == psts_info->en_psts_per_pkt) {
		return pstatus;
	}

	if (ppdu_sts->r.mdata.ppdu_cnt >= PHL_MAX_PPDU_CNT) {
		PHL_TRACE(COMP_PHL_PSTS, _PHL_INFO_,
			  "ppdu_sts->r.mdata.ppdu_cnt >= PHL_MAX_PPDU_CNT!\n");
		return pstatus;
	}

	band = (ppdu_sts->r.mdata.bb_sel > 0) ? HW_BAND_1 : HW_BAND_0;

	if (false == psts_info->en_ppdu_sts[band])
		return pstatus;

	if (ppdu_sts->r.mdata.ppdu_cnt != psts_info->cur_ppdu_cnt[band]) {
		PHL_TRACE(COMP_PHL_PSTS, _PHL_INFO_,
			  "ppdu_sts->r.mdata.ppdu_cnt != psts_info->cur_ppdu_cnt!\n");
		upt_psts = false;
	}

	sts_entry = &psts_info->sts_ent[band][psts_info->cur_ppdu_cnt[band]];
	/* check list empty */
	if (0 == sts_entry->frames.cnt) {
		PHL_TRACE(COMP_PHL_PSTS, _PHL_INFO_,
			  "cur_ppdu_cnt %d --> sts_entry->frames.cnt = 0\n",
			  psts_info->cur_ppdu_cnt[band]);
		pstatus = RTW_PHL_STATUS_SUCCESS;
		return pstatus;
	}

	/* start update phy info to per pkt*/
	if (false == pq_get_front(d, &sts_entry->frames, &frame, _bh)) {
		PHL_ERR(" %s list empty\n", __FUNCTION__);
		return pstatus;
	}
	/**
	 * TODO : How to filter the case :
	 *	pkt(ppdu_cnt = 0) --> missing :psts(ppdu_cnt = 0) --> (all of the pkt, psts dropped/missing)
	 *	--> ppdu_sts(ppdu_cnt = 0)(not for the current buffered pkt.)
	 * workaround : check rate/bw/ppdu_type/... etc
	 **/
	phl_rx = (struct rtw_phl_rx_pkt *)frame;
	if (upt_psts &&
	   ((false == ppdu_sts->r.phy_info.is_pkt_with_data) ||
	    (phl_rx->r.mdata.rx_rate != ppdu_sts->r.mdata.rx_rate) ||
	    (phl_rx->r.mdata.bw != ppdu_sts->r.mdata.bw) ||
	    (phl_rx->r.mdata.rx_gi_ltf != ppdu_sts->r.mdata.rx_gi_ltf) ||
	    (phl_rx->r.mdata.ppdu_type != ppdu_sts->r.mdata.ppdu_type))) {
		    /**
		     * ppdu status is not for the buffered pkt,
		     * skip update phy status to phl_rx
		     **/
		    upt_psts = false;
	}
	/* Get Frame Type */
	ppdu_sts->r.phy_info.frame_type =
		PHL_GET_80211_HDR_TYPE(phl_rx->r.pkt_list[0].vir_addr);

	if ((false == ppdu_sts->r.phy_info.is_valid) &&
	    (true == psts_info->en_fake_psts)) {
		if (phl_rx->r.phy_info.is_drvinfo_vld) {
			ppdu_sts->r.phy_info.rssi = phl_rx->r.phy_info.signal_strength;
			for(i = 0; i< RTW_PHL_MAX_RF_PATH ; i++) {
				ppdu_sts->r.phy_info.rssi_path[i] =
						phl_rx->r.phy_info.signal_strength;
			}
		} else {
			if (RTW_FRAME_TYPE_MGNT == phl_rx->r.mdata.frame_type) {
				ppdu_sts->r.phy_info.rssi =
					rssi_stat->ma_rssi[RTW_RSSI_MGNT_ACAM_A1M];
			} else if (RTW_FRAME_TYPE_DATA == phl_rx->r.mdata.frame_type) {
				ppdu_sts->r.phy_info.rssi =
					rssi_stat->ma_rssi[RTW_RSSI_DATA_ACAM_A1M];
			} else if (RTW_FRAME_TYPE_CTRL == phl_rx->r.mdata.frame_type) {
				ppdu_sts->r.phy_info.rssi =
					rssi_stat->ma_rssi[RTW_RSSI_CTRL_ACAM_A1M];
			} else {
				ppdu_sts->r.phy_info.rssi =
					rssi_stat->ma_rssi[RTW_RSSI_UNKNOWN];
			}
			for(i = 0; i< RTW_PHL_MAX_RF_PATH ; i++) {
				ppdu_sts->r.phy_info.rssi_path[i] =
						ppdu_sts->r.phy_info.rssi;
			}
			ppdu_sts->r.phy_info.ch_idx = rtw_hal_get_cur_ch(phl_info->hal,
							phl_rx->r.mdata.bb_sel);
		}
		ppdu_sts->r.phy_info.is_valid = true;
	}

	do {
		if (false == upt_psts)
			break;
		phl_rx = (struct rtw_phl_rx_pkt *)frame;
		if (phl_rx->r.phy_info.is_drvinfo_vld) {
			/* only copy the ppdu sts, avoid to override drvinfo */
			_phl_rx_copy_phy_sts(&(ppdu_sts->r.phy_info) ,&(phl_rx->r.phy_info));
		} else {
			_os_mem_cpy(d, &(phl_rx->r.phy_info), &(ppdu_sts->r.phy_info),
				    sizeof(struct rtw_phl_ppdu_phy_info));
		}
	} while ((true == psts_info->psts_ampdu) &&
		 (pq_get_next(d, &sts_entry->frames, frame, &frame, _bh)));

	/*2. indicate the frame list*/
	_phl_rx_proc_frame_list(phl_info, &sts_entry->frames);
	/*3. reset the queue */
	pq_reset(d, &(sts_entry->frames), _bh);

	return pstatus;
}

bool
phl_rx_proc_wait_phy_sts(struct phl_info_t *phl_info,
			 struct rtw_phl_rx_pkt *phl_rx)
{
	struct rtw_phl_ppdu_sts_info *psts_info = &(phl_info->phl_com->ppdu_sts_info);
	struct rtw_phl_ppdu_sts_ent *sts_entry = NULL;
	void *d = phl_to_drvpriv(phl_info);
	u8 i = 0;
	bool ret = false;
	enum phl_band_idx band = HW_BAND_0;

	if (false == psts_info->en_psts_per_pkt) {
		return ret;
	}

	if (phl_rx->r.mdata.ppdu_cnt >= PHL_MAX_PPDU_CNT) {
		PHL_ASSERT("phl_rx->r.mdata.ppdu_cnt >= PHL_MAX_PPDU_CNT!");
		return ret;
	}

	band = (phl_rx->r.mdata.bb_sel > 0) ? HW_BAND_1 : HW_BAND_0;

	if (false == psts_info->en_ppdu_sts[band])
		return ret;

	if (psts_info->cur_ppdu_cnt[band] != phl_rx->r.mdata.ppdu_cnt) {
		/* start of PPDU */
		/* 1. Check all of the buffer list is empty */
		/* only check the target rx pkt band */
		for (i = 0; i < PHL_MAX_PPDU_CNT; i++) {
			sts_entry = &psts_info->sts_ent[band][i];
			if (0 != sts_entry->frames.cnt) {
				/* need indicate first */
				PHL_TRACE(COMP_PHL_PSTS, _PHL_INFO_,
					  "band %d ; ppdu_cnt %d queue is not empty \n",
					  band, i);
				_phl_rx_proc_frame_list(phl_info,
						&sts_entry->frames);
				pq_reset(d, &(sts_entry->frames), _bh);
			}
		}

		/* 2. check ppdu status filter condition */
		/* Filter function is supportted only if rxd = long_rxd */
		if ((1 == phl_rx->r.mdata.long_rxd) &&
		    (0 != (psts_info->ppdu_sts_filter &
		           BIT(phl_rx->r.mdata.frame_type)))) {
			/* 3. add new rx pkt to the tail of the queue */
			sts_entry = &psts_info->sts_ent[band][phl_rx->r.mdata.ppdu_cnt];
			pq_reset(d, &(sts_entry->frames), _bh);
			pq_push(d, &(sts_entry->frames), &phl_rx->list,
				_tail, _bh);
			ret = true;
		}
		psts_info->cur_ppdu_cnt[band] = phl_rx->r.mdata.ppdu_cnt;
	} else {
		/* 1. check ppdu status filter condition */
		/* Filter function is supportted only if rxd = long_rxd */
		if ((1 == phl_rx->r.mdata.long_rxd) &&
		    (0 != (psts_info->ppdu_sts_filter &
		           BIT(phl_rx->r.mdata.frame_type)))) {
			/* 2. add to frame list */
			sts_entry = &psts_info->sts_ent[band][phl_rx->r.mdata.ppdu_cnt];
			if (0 == sts_entry->frames.cnt) {
				PHL_TRACE(COMP_PHL_PSTS, _PHL_INFO_,
					  "MPDU is not the start of PPDU, but the queue is empty!!!\n");
			}
			pq_push(d, &(sts_entry->frames), &phl_rx->list,
				_tail, _bh);
			ret = true;
		}
	}

	return ret;
}
#endif

void
phl_rx_proc_ppdu_sts(struct phl_info_t *phl_info, struct rtw_phl_rx_pkt *phl_rx)
{
	u8 i = 0;
	struct rtw_phl_ppdu_sts_info *ppdu_info = NULL;
	struct rtw_phl_ppdu_sts_ent *ppdu_sts_ent = NULL;
	struct rtw_phl_stainfo_t *psta = NULL;

	enum phl_band_idx band = HW_BAND_0;
	struct rtw_rssi_info *rssi_sts;

	if ((NULL == phl_info) || (NULL == phl_rx))
		return;

	band = (phl_rx->r.mdata.bb_sel > 0) ? HW_BAND_1 : HW_BAND_0;
	ppdu_info = &phl_info->phl_com->ppdu_sts_info;
	ppdu_sts_ent = &ppdu_info->sts_ent[band][phl_rx->r.mdata.ppdu_cnt];

	if (false == ppdu_sts_ent->valid)
		return;

	if (true == ppdu_sts_ent->phl_done)
		return;

	ppdu_sts_ent->phl_done = true;

	/* update phl self varibles */
	for(i = 0 ; i < ppdu_sts_ent->usr_num; i++) {
		if (ppdu_sts_ent->sta[i].vld) {
			psta = rtw_phl_get_stainfo_by_macid(phl_info,
				 ppdu_sts_ent->sta[i].macid);
			if (psta == NULL)
				continue;
			rssi_sts = &psta->hal_sta->rssi_stat;
			STA_UPDATE_MA_RSSI_FAST(rssi_sts->ma_rssi, ppdu_sts_ent->phy_info.rssi);
			/* update (re)associate req/resp pkt rssi */
			if (RTW_IS_ASOC_PKT(ppdu_sts_ent->frame_type)) {
				rssi_sts->assoc_rssi =
						ppdu_sts_ent->phy_info.rssi;
			}

			if (RTW_IS_BEACON_OR_PROBE_RESP_PKT(
						ppdu_sts_ent->frame_type)) {
				if (0 == rssi_sts->ma_rssi_mgnt) {
					rssi_sts->ma_rssi_mgnt =
						ppdu_sts_ent->phy_info.rssi;
				} else {
					STA_UPDATE_MA_RSSI_FAST(
						rssi_sts->ma_rssi_mgnt,
						ppdu_sts_ent->phy_info.rssi);
				}
			}
		}
		else {
			if (RTW_IS_ASOC_REQ_PKT(ppdu_sts_ent->frame_type) &&
				(ppdu_sts_ent->usr_num == 1)) {
				psta = rtw_phl_get_stainfo_by_addr_ex(phl_info,
						ppdu_sts_ent->src_mac_addr);
				if (psta) {
					psta->hal_sta->rssi_stat.assoc_rssi =
						ppdu_sts_ent->phy_info.rssi;

					#ifdef DBG_AP_CLIENT_ASSOC_RSSI
					PHL_INFO("%s [Rx-ASOC_REQ] - macid:%d, MAC-Addr:%02x-%02x-%02x-%02x-%02x-%02x, assoc_rssi:%d\n",
						__func__,
						psta->macid,
						ppdu_sts_ent->src_mac_addr[0],
						ppdu_sts_ent->src_mac_addr[1],
						ppdu_sts_ent->src_mac_addr[2],
						ppdu_sts_ent->src_mac_addr[3],
						ppdu_sts_ent->src_mac_addr[4],
						ppdu_sts_ent->src_mac_addr[5],
						psta->hal_sta->rssi_stat.assoc_rssi);
					#endif
				}
			}
		}
	}
}

void phl_rx_wp_report_record_sts(struct phl_info_t *phl_info,
				 u8 macid, u16 ac_queue, u8 txsts)
{
#if defined(CONFIG_PHL_RELEASE_RPT_ENABLE) || defined(CONFIG_PCI_HCI)
	struct rtw_phl_stainfo_t *phl_sta = NULL;
	struct rtw_hal_stainfo_t *hal_sta = NULL;
	struct rtw_wp_rpt_stats *wp_rpt_stats= NULL;
	struct hal_info_t *hal_info = (struct hal_info_t *)(phl_info->hal);

	if (ac_queue >= RTW_MAX_WP_RPT_AC_NUM(hal_info))
		return;

	phl_sta = rtw_phl_get_stainfo_by_macid(phl_info, macid);

	if (phl_sta) {
		hal_sta = phl_sta->hal_sta;

		if (hal_sta->trx_stat.wp_rpt_stats == NULL) {
			PHL_ERR("rtp_stats NULL\n");
			return;
		}
		/* Record Per ac queue statistics */
		wp_rpt_stats = &hal_sta->trx_stat.wp_rpt_stats[ac_queue];

		_os_spinlock(phl_to_drvpriv(phl_info), &hal_sta->trx_stat.tx_sts_lock, _bh, NULL);
		if (TX_STATUS_TX_DONE == txsts) {
			/* record total tx ok*/
			hal_sta->trx_stat.tx_ok_cnt++;
			/* record per ac queue tx ok*/
			wp_rpt_stats->tx_ok_cnt++;
		} else {
			/* record total tx fail*/
			hal_sta->trx_stat.tx_fail_cnt++;
			/* record per ac queue tx fail*/
			if (TX_STATUS_TX_FAIL_REACH_RTY_LMT == txsts)
				wp_rpt_stats->rty_fail_cnt++;
			else if (TX_STATUS_TX_FAIL_LIFETIME_DROP == txsts)
				wp_rpt_stats->lifetime_drop_cnt++;
			else if (TX_STATUS_TX_FAIL_MACID_DROP == txsts)
				wp_rpt_stats->macid_drop_cnt++;
		}
		_os_spinunlock(phl_to_drvpriv(phl_info), &hal_sta->trx_stat.tx_sts_lock, _bh, NULL);

		PHL_TRACE(COMP_PHL_DBG, _PHL_DEBUG_,"macid: %u, ac_queue: %u, tx_ok_cnt: %u, rty_fail_cnt: %u, "
			"lifetime_drop_cnt: %u, macid_drop_cnt: %u\n"
			, macid, ac_queue, wp_rpt_stats->tx_ok_cnt, wp_rpt_stats->rty_fail_cnt
			, wp_rpt_stats->lifetime_drop_cnt, wp_rpt_stats->macid_drop_cnt);
		PHL_TRACE(COMP_PHL_DBG, _PHL_DEBUG_,"totoal tx ok: %u \n totoal tx fail: %u\n"
			, hal_sta->trx_stat.tx_ok_cnt, hal_sta->trx_stat.tx_fail_cnt);
	} else {
		PHL_TRACE(COMP_PHL_DBG, _PHL_DEBUG_, "%s: PHL_STA not found\n",
				__FUNCTION__);
	}
#endif
	return;
}


void _phl_handle_ppdu_sts_q(struct phl_info_t *phl_info)
{
#ifdef CONFIG_PHL_RX_PSTS_PER_PKT
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct rtw_phl_ppdu_sts_ent *sts_entry = NULL;
	void *d = phl_to_drvpriv(phl_info);
	u8 i = 0;
	u8 j = 0;

	for (j = 0; j < HW_BAND_MAX; j++) {
		for (i = 0; i < PHL_MAX_PPDU_CNT; i++) {
			sts_entry = &phl_com->ppdu_sts_info.sts_ent[j][i];

			if (0 != sts_entry->frames.cnt) {
				PHL_TRACE(COMP_PHL_PSTS, _PHL_INFO_, "band %d ; ppdu_cnt %d queue is not empty \n",
				          j, i);
				_phl_rx_proc_frame_list(phl_info,
				                        &sts_entry->frames);
				pq_reset(d, &(sts_entry->frames), _bh);
			}
		}
	}
#else
	return;
#endif
}

void phl_handle_queued_rx(struct phl_info_t *phl_info)
{
	_phl_handle_ppdu_sts_q(phl_info);
}


static void _dump_rx_reorder_info(struct phl_info_t *phl_info,
				  struct rtw_phl_stainfo_t *sta)
{
	void *drv_priv = phl_to_drvpriv(phl_info);
	u8 i;

	PHL_INFO("[PHL_RX] dump rx reorder buffer info:\n");
	for (i = 0; i < ARRAY_SIZE(sta->tid_rx); i++) {

		_os_spinlock(drv_priv, &sta->tid_rx_lock, _bh, NULL);
		if (sta->tid_rx[i]) {
			PHL_INFO("== tid = %d ==\n", sta->tid_rx[i]->tid);
			PHL_INFO("head_seq_num = %d\n",
				 sta->tid_rx[i]->head_seq_num);
			PHL_INFO("[PHL_RX] stored_mpdu_num = %d\n",
				 sta->tid_rx[i]->stored_mpdu_num);
			PHL_INFO("ssn = %d\n", sta->tid_rx[i]->ssn);
			PHL_INFO("buf_size = %d\n", sta->tid_rx[i]->buf_size);
			PHL_INFO("started = %d\n", sta->tid_rx[i]->started);
			PHL_INFO("removed = %d\n", sta->tid_rx[i]->removed);
		}
		_os_spinunlock(drv_priv, &sta->tid_rx_lock, _bh, NULL);
	}
}

void phl_dump_all_sta_rx_info(struct phl_info_t *phl_info)
{
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct rtw_phl_stainfo_t *sta = NULL;
	struct rtw_wifi_role_t *role = NULL;
	void *drv = phl_to_drvpriv(phl_info);
	struct phl_queue *sta_queue;
	u8 i;
	u8 idx = 0;
	struct rtw_wifi_role_link_t *rlink = NULL;

	PHL_INFO("[PHL_RX] dump all sta rx info:\n");
	for (i = 0; i < MAX_WIFI_ROLE_NUMBER; i++) {
		role = &phl_com->wifi_roles[i];
		if (role->active) {
			PHL_INFO("[PHL_RX] wrole idx = %d\n", i);
			PHL_INFO("[PHL_RX] wrole type = %d\n", role->type);
			PHL_INFO("[PHL_RX] wrole mstate = %d\n", role->mstate);

			for (idx = 0; idx < role->rlink_num; idx++) {
				rlink = get_rlink(role,idx);
				PHL_INFO("rlink idx = %u\n", idx);
				PHL_INFO("rlink mstate = %d\n", rlink->mstate);
				sta_queue = &rlink->assoc_sta_queue;

				_os_spinlock(drv, &sta_queue->lock, _bh, NULL);
				phl_list_for_loop(sta, struct rtw_phl_stainfo_t,
							&sta_queue->queue, list) {
					PHL_INFO("%s MACID:%d %02x:%02x:%02x:%02x:%02x:%02x \n",
					 __func__, sta->macid,
					 sta->mac_addr[0],
					 sta->mac_addr[1],
					 sta->mac_addr[2],
					 sta->mac_addr[3],
					 sta->mac_addr[4],
					 sta->mac_addr[5]);
					_dump_rx_reorder_info(phl_info, sta);
				}
				_os_spinunlock(drv, &sta_queue->lock, _bh, NULL);
			}
		}
	}
}

void phl_rx_dbg_dump(struct phl_info_t *phl_info, u8 band_idx)
{
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_FAILURE;

	phl_status = phl_cmd_enqueue(phl_info,
	                   band_idx,
	                   MSG_EVT_DBG_RX_DUMP,
	                   NULL,
	                   0,
	                   NULL,
	                   PHL_CMD_NO_WAIT,
	                   0);
	if (phl_status != RTW_PHL_STATUS_SUCCESS) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_ERR_, "%s: cmd enqueue fail!\n",
			  __func__);
	}

}

#ifdef CONFIG_PCI_HCI
u32 rtw_phl_get_hw_cnt_rdu(void *phl)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct rtw_hal_com_t *hal_com = rtw_hal_get_halcom(phl_info->hal);

	return hal_com->trx_stat.rx_rdu_cnt;
}
#endif


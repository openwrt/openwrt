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
#define _PHL_RX_AGG_C_
#include "phl_headers.h"

/**
 * phl_tid_ampdu_rx_free() - Free the tid ampdu rx entry specified by @r.
 *
 * The caller has to take care of potential race condition:
 *  - if it is in @rtw_phl_stainfo_t.tid_rx, should be called with
 *    @rtw_phl_stainfo_t.tid_rx_lock held
 *  - if it is not in @rtw_phl_stainfo_t.tid_rx, it can be called freely since
 *    other part of the code can't see it
 *
 * @phl_tid_ampdu_rx.removed can be set to make sure that the reorder_time will
 * not be set again.  This is usful when canceling the timer synchronously
 * before releasing it.
 *
 * TODO: On macos, _os_kmem_free() should not be called with lock held since it
 * may block.
 */
void phl_tid_ampdu_rx_free(struct phl_tid_ampdu_rx *r)
{
	/* ref wil_tid_ampdu_rx_free() and ieee80211_free_tid_rx() */

	u16 buf_size;
	void *drv_priv;
	int i;
	struct rtw_phl_rx_pkt *pkt = NULL;
	struct phl_hci_trx_ops *hci_trx_ops = NULL;

	if (!r)
		return;

	buf_size = r->buf_size;
	drv_priv = r->drv_priv;
	hci_trx_ops = r->phl_info->hci_trx_ops;

	for (i = 0; i < r->buf_size; i++) {
		pkt = r->reorder_buf[i];
		if (NULL != pkt)
			hci_trx_ops->recycle_rx_pkt(r->phl_info, pkt);
	}
	_os_kmem_free(drv_priv, r->reorder_time,
	              buf_size * sizeof(u32));
	_os_kmem_free(drv_priv, r->reorder_buf,
	              buf_size * sizeof(struct rtw_phl_rx_pkt *));
	_os_kmem_free(drv_priv, r, sizeof(*r));
}


static inline void
_phl_cancel_rx_reorder_timer(struct phl_info_t *phl_info,
                         struct rtw_phl_stainfo_t *sta)
{
	void *drv = phl_to_drvpriv(phl_info);

#ifdef PHL_PLATFORM_WINDOWS
	/* Cancel the reorder_timer of the stainfo synchronously.
	 * Note that on Windows, _os_cancel_timer() does not guarantee that
	 * after the cancel, no timer callback will ever be called.  Therefore
	 * @comp_sync is used for waiting this residual timer callback on
	 * Windows.*/
	_os_event_reset(drv, &sta->comp_sync);
	_os_cancel_timer(drv, &sta->reorder_timer);
	_os_event_wait(drv, &sta->comp_sync, PHL_REORDER_TIMER_SYNC_TO_MS);
#else /*PHL_PLATFORM_LINUX && PHL_PLATFORM_AP*/
	_os_cancel_timer(drv, &sta->reorder_timer);/*or _os_cancel_timer_async*/
#endif
}

void phl_free_rx_reorder(struct phl_info_t *phl_info,
                         struct rtw_phl_stainfo_t *sta)
{
	void *drv = phl_to_drvpriv(phl_info);
	u8 i = 0;

	/* Mark the enrties as removed to make sure that reorder_timer of the
	 * stainfo will not be set again after the timer is canceled. */
	_os_spinlock(drv, &sta->tid_rx_lock, _bh, NULL);
	for (i = 0; i < ARRAY_SIZE(sta->tid_rx); i++)
		if (sta->tid_rx[i])
			sta->tid_rx[i]->removed = true;
	_os_spinunlock(drv, &sta->tid_rx_lock, _bh, NULL);

	_phl_cancel_rx_reorder_timer(phl_info, sta);

	/* Free the tid ampdu rx entry */
	_os_spinlock(drv, &sta->tid_rx_lock, _bh, NULL);
	for (i = 0; i < ARRAY_SIZE(sta->tid_rx); i++) {
		/* ref wil_disconnect_cid() */
		if (!sta->tid_rx[i])
			continue;
		phl_tid_ampdu_rx_free(sta->tid_rx[i]);
		sta->tid_rx[i] = NULL;
	}
	_os_spinunlock(drv, &sta->tid_rx_lock, _bh, NULL);
}


void rtw_phl_stop_rx_ba_session(void *phl, struct rtw_phl_stainfo_t *sta,
                                u16 tid)
{
	/* ref wmi_evt_delba() and ___ieee80211_stop_rx_ba_session() */

	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	void *drv_priv = phl_to_drvpriv(phl_info);
	struct phl_tid_ampdu_rx *r;

	if (NULL == sta) {
		PHL_TRACE(COMP_PHL_RECV, _PHL_WARNING_,
		          "rtw_phl_stop_rx_ba_session: station info is NULL!\n");
		return;
	}

	PHL_INFO("Stop rx BA session for sta=0x%p, tid=%u\n", sta, tid);

	if (tid >= ARRAY_SIZE(sta->tid_rx))
		return;

	rtw_hal_stop_ba_session(phl_info->hal, sta, tid);

	_os_spinlock(drv_priv, &sta->tid_rx_lock, _bh, NULL);
	if (!sta->tid_rx[tid]) {
		PHL_INFO("No active session found for the specified sta tid pair\n");
		_os_spinunlock(drv_priv, &sta->tid_rx_lock, _bh, NULL);
		return;
	}
	r = sta->tid_rx[tid];
	sta->tid_rx[tid] = NULL;
	r->removed = true;

	/* Note that it is safe to free the tid ampdu rx here.  If reorder_timer
	 * callback is invoked, the tid_rx_lock is held and it does not do
	 * anything to the entry if it is NULL. */
#ifdef PHL_RXSC_AMPDU
	_os_spinlock(drv_priv, &phl_info->phl_com->rxsc_entry.rxsc_lock,
			_bh, NULL);
	if (phl_info->phl_com->rxsc_entry.cached_r == r)
		phl_info->phl_com->rxsc_entry.cached_r = NULL;
	_os_spinunlock(drv_priv, &phl_info->phl_com->rxsc_entry.rxsc_lock,
			_bh, NULL);
#endif
	phl_tid_ampdu_rx_free(r);
	_os_spinunlock(drv_priv, &sta->tid_rx_lock, _bh, NULL);

	PHL_INFO("Rx BA session for sta=0x%p, tid=%u freed\n", sta, tid);
}

struct phl_tid_ampdu_rx *phl_tid_ampdu_rx_alloc(struct phl_info_t *phl_info,
                                                struct rtw_phl_stainfo_t *sta,
                                                u16 timeout, u16 ssn, u16 tid,
                                                u16 buf_size)
{
	/* ref wil_tid_ampdu_rx_alloc() */

	void *drv_priv = phl_to_drvpriv(phl_info);
	struct phl_tid_ampdu_rx *r;

	/* allocate r */
	r = _os_kmem_alloc(drv_priv, sizeof(*r));
	if (!r)
		return NULL;
	_os_mem_set(drv_priv, r, 0, sizeof(*r));

	/* allocate reorder_buf */
	r->reorder_buf =
		_os_kmem_alloc(drv_priv,
		               buf_size * sizeof(struct rtw_phl_rx_pkt *));
	if (!r->reorder_buf) {
		_os_kmem_free(drv_priv, r, sizeof(*r));
		return NULL;
	}
	_os_mem_set(drv_priv, r->reorder_buf, 0,
	            buf_size * sizeof(struct rtw_phl_rx_pkt *));

	/* allocate reorder_time */
	r->reorder_time =
		_os_kmem_alloc(drv_priv, buf_size * sizeof(u32));
	if (!r->reorder_time) {
		_os_kmem_free(drv_priv, r->reorder_buf,
		              buf_size * sizeof(struct rtw_phl_rx_pkt *));
		_os_kmem_free(drv_priv, r, sizeof(*r));
		return NULL;
	}
	_os_mem_set(drv_priv, r->reorder_time, 0,
	            buf_size * sizeof(u32));

	/* init other fields */
	r->sta = sta;
	r->ssn = ssn;
	r->head_seq_num = ssn;
	r->buf_size = buf_size;
	r->stored_mpdu_num = 0;
	r->tid = tid;
	r->started = false;
	r->drv_priv = drv_priv;
	r->phl_info = phl_info;

	return r;
}

enum rtw_phl_status
rtw_phl_start_rx_ba_session(void *phl, struct rtw_phl_stainfo_t *sta,
                            u8 dialog_token, u16 timeout, u16 start_seq_num,
                            u16 ba_policy, u16 tid, u16 buf_size)
{
	/* ref wil_addba_rx_request() and ___ieee80211_start_rx_ba_session() */

	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	enum rtw_hal_status hal_sts = RTW_HAL_STATUS_FAILURE;
	void *drv_priv = phl_to_drvpriv(phl_info);
	struct phl_tid_ampdu_rx *r;

	PHL_INFO("Start rx BA session for sta=0x%p, tid=%u, buf_size=%u, timeout=%u\n",
	         sta, tid, buf_size, timeout);

	if (tid >= ARRAY_SIZE(sta->tid_rx)) {
		PHL_WARN("tid(%u) index out of range (%u)\n", tid, (u32)ARRAY_SIZE(sta->tid_rx));
		return RTW_PHL_STATUS_RESOURCE;
	}

	if (sta->tid_rx[tid])
		rtw_hal_stop_ba_session(phl_info->hal, sta, tid);

	hal_sts = rtw_hal_start_ba_session(phl_info->hal, sta,
	                                   dialog_token, timeout,
	                                   start_seq_num, ba_policy,
	                                   tid, buf_size);

	/* TODO: sta status */

	/* TODO: check sta capability */

	/* apply policies */
	if (ba_policy) {
		PHL_ERR("BACK requested unsupported ba_policy == 1\n");
		return RTW_PHL_STATUS_FAILURE;
	}

	/* apply buf_size */
	if (buf_size == 0) {
		PHL_INFO("Suggest BACK wsize %d\n", PHL_MAX_AGG_WSIZE);
		buf_size = PHL_MAX_AGG_WSIZE;
	}

	/* allocate tid ampdu rx */
	r = phl_tid_ampdu_rx_alloc(phl_info, sta, timeout, start_seq_num, tid,
	                           buf_size);
	if (!r) {
		PHL_ERR("Failed to alloc tid ampdu rx\n");
		return RTW_PHL_STATUS_RESOURCE;
	}

	/* apply */
	_os_spinlock(drv_priv, &sta->tid_rx_lock, _bh, NULL);
	if (sta->tid_rx[tid])
		phl_tid_ampdu_rx_free(sta->tid_rx[tid]);
	sta->tid_rx[tid] = r;
	_os_spinunlock(drv_priv, &sta->tid_rx_lock, _bh, NULL);

	return RTW_PHL_STATUS_SUCCESS;
}

void phl_notify_reorder_sleep(void *phl, struct rtw_phl_stainfo_t *sta)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	void *drv = phl_to_drvpriv(phl_info);
	struct phl_tid_ampdu_rx *r;
	u16 tid = 0;

	if (NULL == sta) {
		PHL_TRACE(COMP_PHL_RECV, _PHL_WARNING_,
		          "rtw_phl_stop_rx_ba_session: station info is NULL!\n");
		return;
	}

	_os_spinlock(drv, &sta->tid_rx_lock, _bh, NULL);
	for (tid = 0; tid < ARRAY_SIZE(sta->tid_rx); tid++) {
		if (!sta->tid_rx[tid])
			continue;

		PHL_INFO("Notify rx BA session sleep for sta=0x%p, tid=%u\n",
		         sta, tid);
		r = sta->tid_rx[tid];
		r->sleep = true;
		PHL_INFO("Rx BA session for sta=0x%p, tid=%u sleep\n", sta, tid);
	}
	_os_spinunlock(drv, &sta->tid_rx_lock, _bh, NULL);
}

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
#define _TRX_TEST_C_
#include "../phl_headers.h"
#include "../phl_api.h"
/*#include "../hal_g6/hal_api_mac.h"*/

#ifdef CONFIG_PHL_TEST_SUITE

void rtw_phl_test_rx_callback(void *context);
enum rtw_phl_status phl_recycle_test_tx(void *phl, struct rtw_xmit_req *treq);



void _phl_free_rx_req_pool(void *phl)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_trx_test *trx_test = (struct phl_trx_test *)phl_info->trx_test;
	struct rtw_pool *rx_req_pool = &trx_test->rx_req_pool;
	void *drv_priv = phl_to_drvpriv(phl_info);

	PHL_INFO("_phl_free_rx_req_pool : idle counter (%d), busy counter (%d), total counter (%d)\n",
			 rx_req_pool->idle_cnt, rx_req_pool->busy_cnt, rx_req_pool->total_cnt);
	_os_spinlock_free(drv_priv, &rx_req_pool->idle_lock);
	_os_spinlock_free(drv_priv, &rx_req_pool->busy_lock);
	INIT_LIST_HEAD(&rx_req_pool->idle_list);
	INIT_LIST_HEAD(&rx_req_pool->busy_list);

	if (NULL != rx_req_pool->buf) {
		_os_mem_free(drv_priv, rx_req_pool->buf, rx_req_pool->buf_len);
	}

	rx_req_pool->buf_len = 0;
	rx_req_pool->idle_cnt = 0;
	rx_req_pool->busy_cnt = 0;
	rx_req_pool->total_cnt = 0;

}

enum rtw_phl_status _phl_alloc_rx_req_pool(void *phl, u32 rx_req_num)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_trx_test *trx_test = (struct phl_trx_test *)phl_info->trx_test;
	struct rtw_pool *rx_req_pool = &trx_test->rx_req_pool;
	void *drv_priv = phl_to_drvpriv(phl_info);
	struct rtw_test_rx *rreq = NULL;
	u32 buf_len = 0, i = 0;
	FUNCIN_WSTS(status);
	do {
		if (0 == rx_req_num)
			break;

		rx_req_pool->total_cnt = rx_req_num;
		buf_len = sizeof(struct rtw_test_rx) * rx_req_num;
		rx_req_pool->buf = _os_mem_alloc(drv_priv, buf_len);

		if (NULL == rx_req_pool->buf)
			break;
		rx_req_pool->buf_len = buf_len;
		INIT_LIST_HEAD(&rx_req_pool->idle_list);
		INIT_LIST_HEAD(&rx_req_pool->busy_list);
		_os_spinlock_init(drv_priv, &rx_req_pool->idle_lock);
		_os_spinlock_init(drv_priv, &rx_req_pool->busy_lock);
		rreq = (struct rtw_test_rx *)rx_req_pool->buf;
		for (i = 0; i < rx_req_num; i++) {
			INIT_LIST_HEAD(&rreq[i].list);
			list_add_tail(&rreq[i].list, &rx_req_pool->idle_list);
			rx_req_pool->idle_cnt++;
		}

		status = RTW_PHL_STATUS_SUCCESS;

	} while (false);
	PHL_INFO("_phl_alloc_rx_req_pool : idle counter (%d), busy counter (%d), total counter (%d)\n",
			 rx_req_pool->idle_cnt, rx_req_pool->busy_cnt, rx_req_pool->total_cnt);
	FUNCOUT_WSTS(status);

	return status;
}

struct rtw_test_rx *_phl_query_idle_rx_req(struct phl_info_t *phl_info)
{
	struct rtw_test_rx *rreq = NULL;
	struct phl_trx_test *trx_test = (struct phl_trx_test *)phl_info->trx_test;
	struct rtw_pool *rx_req_pool = &trx_test->rx_req_pool;
	void *drv_priv = phl_to_drvpriv(phl_info);

	_os_spinlock(drv_priv, &rx_req_pool->idle_lock, _bh, NULL);

	if (false == list_empty(&rx_req_pool->idle_list)) {
		rreq = list_first_entry(&rx_req_pool->idle_list,
				  struct rtw_test_rx, list);
		list_del(&rreq->list);
		rx_req_pool->idle_cnt--;
	}

	_os_spinunlock(drv_priv, &rx_req_pool->idle_lock, _bh, NULL);

	return rreq;
}

struct rtw_test_rx *_phl_query_busy_rx_req(struct phl_info_t *phl_info)
{
	struct rtw_test_rx *rreq = NULL;
	struct phl_trx_test *trx_test = (struct phl_trx_test *)phl_info->trx_test;
	struct rtw_pool *rx_req_pool = &trx_test->rx_req_pool;
	void *drv_priv = phl_to_drvpriv(phl_info);

	_os_spinlock(drv_priv, &rx_req_pool->busy_lock, _bh, NULL);

	if (false == list_empty(&rx_req_pool->busy_list)) {
		rreq = list_first_entry(&rx_req_pool->busy_list,
				  struct rtw_test_rx, list);
		list_del(&rreq->list);
		rx_req_pool->busy_cnt--;
	}

	_os_spinunlock(drv_priv, &rx_req_pool->busy_lock, _bh, NULL);

	return rreq;
}

void _phl_release_rx_req(struct phl_info_t *phl_info, struct rtw_test_rx *rreq)
{
	struct phl_trx_test *trx_test = (struct phl_trx_test *)phl_info->trx_test;
	struct rtw_pool *rx_req_pool = &trx_test->rx_req_pool;
	void *drv_priv = phl_to_drvpriv(phl_info);

	_os_spinlock(drv_priv, &rx_req_pool->idle_lock, _bh, NULL);

	_os_mem_set(drv_priv, &rreq->rx.mdata, 0,
		    sizeof(rreq->rx.mdata));
	_os_mem_set(drv_priv, &rreq->rx.pkt_list, 0,
		    sizeof(rreq->rx.pkt_list) * rreq->rx.pkt_cnt);

	rreq->rx.shortcut_id = 0;
	rreq->rx.pkt_cnt = 0;

	INIT_LIST_HEAD(&rreq->list);
	list_add_tail(&rreq->list, &rx_req_pool->idle_list);
	rx_req_pool->idle_cnt++;

	_os_spinunlock(drv_priv, &rx_req_pool->idle_lock, _bh, NULL);
}

void _phl_insert_busy_rx_req(struct phl_info_t *phl_info, struct rtw_test_rx *rreq)
{
	struct phl_trx_test *trx_test = (struct phl_trx_test *)phl_info->trx_test;
	struct rtw_pool *rx_req_pool = &trx_test->rx_req_pool;
	void *drv_priv = phl_to_drvpriv(phl_info);

	_os_spinlock(drv_priv, &rx_req_pool->busy_lock, _bh, NULL);

	list_add_tail(&rreq->list, &rx_req_pool->busy_list);
	rx_req_pool->busy_cnt++;

	_os_spinunlock(drv_priv, &rx_req_pool->busy_lock, _bh, NULL);
}


void _phl_free_tx_req_pool(void *phl)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_trx_test *trx_test = (struct phl_trx_test *)phl_info->trx_test;
	struct rtw_pool *tx_req_pool = &trx_test->tx_req_pool;
	void *drv_priv = phl_to_drvpriv(phl_info);

	PHL_INFO("_phl_free_tx_req_pool : idle counter (%d), busy counter (%d), total counter (%d)\n",
			 tx_req_pool->idle_cnt, tx_req_pool->busy_cnt, tx_req_pool->total_cnt);
	_os_spinlock_free(drv_priv, &tx_req_pool->idle_lock);
	_os_spinlock_free(drv_priv, &tx_req_pool->busy_lock);
	INIT_LIST_HEAD(&tx_req_pool->idle_list);
	INIT_LIST_HEAD(&tx_req_pool->busy_list);

	if (NULL != tx_req_pool->buf) {
		_os_mem_free(drv_priv, tx_req_pool->buf, tx_req_pool->buf_len);
	}

	tx_req_pool->buf_len = 0;
	tx_req_pool->idle_cnt = 0;
	tx_req_pool->busy_cnt = 0;
	tx_req_pool->total_cnt = 0;

}

enum rtw_phl_status _phl_alloc_tx_req_pool(void *phl, u32 tx_req_num)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_trx_test *trx_test = (struct phl_trx_test *)phl_info->trx_test;
	struct rtw_pool *tx_req_pool = &trx_test->tx_req_pool;
	void *drv_priv = phl_to_drvpriv(phl_info);
	struct rtw_xmit_req *treq = NULL;
	u32 buf_len = 0, i = 0;
	FUNCIN_WSTS(status);

	do {
		if (0 == tx_req_num)
			break;

		tx_req_pool->total_cnt = tx_req_num;
		buf_len = sizeof(struct rtw_xmit_req) * tx_req_num;
		tx_req_pool->buf = _os_mem_alloc(drv_priv, buf_len);

		if (NULL == tx_req_pool->buf)
			break;
		tx_req_pool->buf_len = buf_len;
		INIT_LIST_HEAD(&tx_req_pool->idle_list);
		INIT_LIST_HEAD(&tx_req_pool->busy_list);
		_os_spinlock_init(drv_priv, &tx_req_pool->idle_lock);
		_os_spinlock_init(drv_priv, &tx_req_pool->busy_lock);
		treq = (struct rtw_xmit_req *)tx_req_pool->buf;
		for (i = 0; i < tx_req_num; i++) {
			INIT_LIST_HEAD(&treq[i].list);
			treq[i].treq_type = RTW_PHL_TREQ_TYPE_TEST_PATTERN;
			list_add_tail(&treq[i].list, &tx_req_pool->idle_list);
			tx_req_pool->idle_cnt++;
		}

		status = RTW_PHL_STATUS_SUCCESS;

	} while (false);
	PHL_INFO("_phl_alloc_tx_req_pool : idle counter (%d), busy counter (%d), total counter (%d)\n",
			 tx_req_pool->idle_cnt, tx_req_pool->busy_cnt, tx_req_pool->total_cnt);
	FUNCOUT_WSTS(status);

	return status;
}

struct rtw_xmit_req *_phl_query_idle_tx_req(struct phl_info_t *phl_info)
{
	struct rtw_xmit_req *treq = NULL;
	struct phl_trx_test *trx_test = (struct phl_trx_test *)phl_info->trx_test;
	struct rtw_pool *tx_req_pool = &trx_test->tx_req_pool;
	void *drv_priv = phl_to_drvpriv(phl_info);

	_os_spinlock(drv_priv, &tx_req_pool->idle_lock, _bh, NULL);

	if (false == list_empty(&tx_req_pool->idle_list)) {
		treq = list_first_entry(&tx_req_pool->idle_list,
				  struct rtw_xmit_req, list);
		list_del(&treq->list);
		tx_req_pool->idle_cnt--;
	}

	_os_spinunlock(drv_priv, &tx_req_pool->idle_lock, _bh, NULL);

	return treq;
}

struct rtw_xmit_req *_phl_query_busy_tx_req(struct phl_info_t *phl_info)
{
	struct phl_trx_test *trx_test = (struct phl_trx_test *)phl_info->trx_test;
	struct rtw_pool *tx_req_pool = &trx_test->tx_req_pool;
	struct rtw_xmit_req *treq = NULL;
	void *drv_priv = phl_to_drvpriv(phl_info);

	_os_spinlock(drv_priv, &tx_req_pool->busy_lock, _bh, NULL);

	if (false == list_empty(&tx_req_pool->busy_list)) {
		treq = list_first_entry(&tx_req_pool->busy_list,
				  struct rtw_xmit_req, list);
		list_del(&treq->list);
		tx_req_pool->busy_cnt--;
	}

	_os_spinunlock(drv_priv, &tx_req_pool->busy_lock, _bh, NULL);

	return treq;
}

void _phl_remove_busy_tx_req(struct phl_info_t *phl_info, struct rtw_xmit_req *treq)
{
	struct phl_trx_test *trx_test = (struct phl_trx_test *)phl_info->trx_test;
	struct rtw_pool *tx_req_pool = &trx_test->tx_req_pool;
	void *drv_priv = phl_to_drvpriv(phl_info);

	_os_spinlock(drv_priv, &tx_req_pool->busy_lock, _bh, NULL);

	if (false == list_empty(&tx_req_pool->busy_list)) {
		list_del(&treq->list);
		tx_req_pool->busy_cnt--;
	}

	_os_spinunlock(drv_priv, &tx_req_pool->busy_lock, _bh, NULL);
}


void _phl_release_tx_req(struct phl_info_t *phl_info, struct rtw_xmit_req *treq)
{
	struct phl_trx_test *trx_test = (struct phl_trx_test *)phl_info->trx_test;
	struct rtw_pool *tx_req_pool = &trx_test->tx_req_pool;
	void *drv_priv = phl_to_drvpriv(phl_info);

	_os_spinlock(drv_priv, &tx_req_pool->idle_lock, _bh, NULL);

	_os_mem_set(drv_priv, &treq->mdata, 0, sizeof(treq->mdata));

	treq->shortcut_id = 0;
	treq->total_len = 0;
	treq->pkt_cnt = 0;
	treq->pkt_list = NULL;
	treq->os_priv = NULL;

	INIT_LIST_HEAD(&treq->list);
	list_add_tail(&treq->list, &tx_req_pool->idle_list);
	tx_req_pool->idle_cnt++;

	_os_spinunlock(drv_priv, &tx_req_pool->idle_lock, _bh, NULL);
}

void _phl_insert_busy_tx_req(struct phl_info_t *phl_info, struct rtw_xmit_req *treq)
{
	struct phl_trx_test *trx_test = (struct phl_trx_test *)phl_info->trx_test;
	struct rtw_pool *tx_req_pool = &trx_test->tx_req_pool;
	void *drv_priv = phl_to_drvpriv(phl_info);

	_os_spinlock(drv_priv, &tx_req_pool->busy_lock, _bh, NULL);
	list_add_tail(&treq->list, &tx_req_pool->busy_list);
	tx_req_pool->busy_cnt++;

	_os_spinunlock(drv_priv, &tx_req_pool->busy_lock, _bh, NULL);
}


void _phl_free_tx_pkt_pool(void *phl)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_trx_test *trx_test = (struct phl_trx_test *)phl_info->trx_test;
	struct rtw_pool *tx_pkt_pool = &trx_test->tx_pkt_pool;
	void *drv_priv = phl_to_drvpriv(phl_info);
	struct rtw_payload *tpkt = NULL;
	u32 i = 0;

	PHL_INFO("_phl_free_tx_pkt_pool : idle counter (%d), busy counter (%d), total counter (%d)\n",
			 tx_pkt_pool->idle_cnt, tx_pkt_pool->busy_cnt, tx_pkt_pool->total_cnt);
	_os_spinlock_free(drv_priv, &tx_pkt_pool->idle_lock);
	_os_spinlock_free(drv_priv, &tx_pkt_pool->busy_lock);
	INIT_LIST_HEAD(&tx_pkt_pool->idle_list);
	INIT_LIST_HEAD(&tx_pkt_pool->busy_list);

	tpkt = (struct rtw_payload *)tx_pkt_pool->buf;
	for (i = 0; i <	tx_pkt_pool->total_cnt; i++) {
		INIT_LIST_HEAD(&tpkt->list);
		if (NULL != tpkt->pkt.vir_addr) {
			#ifdef CONFIG_PCI_HCI
			_os_shmem_free(drv_priv,
						tpkt->pkt.vir_addr,
						&tpkt->pkt.phy_addr_l,
						&tpkt->pkt.phy_addr_h,
						MAX_TEST_PAYLOAD_SIZE,
						false,
						DMA_TO_DEVICE,
						tpkt->os_rsvd[0]);
			#else
			_os_mem_free(drv_priv, tpkt->pkt.vir_addr,
							tpkt->pkt.length);
			#endif
		}
		tpkt->pkt.length = 0;
		tpkt++;
	}

	if (NULL != tx_pkt_pool->buf) {
		_os_mem_free(drv_priv, tx_pkt_pool->buf, tx_pkt_pool->buf_len);
	}

	tx_pkt_pool->buf_len = 0;
	tx_pkt_pool->idle_cnt = 0;
	tx_pkt_pool->busy_cnt = 0;
	tx_pkt_pool->total_cnt = 0;

}


enum rtw_phl_status _phl_alloc_tx_pkt_pool(void *phl, u32 tx_pkt_num,
					   u32 tx_pkt_size)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_trx_test *trx_test = (struct phl_trx_test *)phl_info->trx_test;
	struct rtw_pool *tx_pkt_pool = &trx_test->tx_pkt_pool;
	void *drv_priv = phl_to_drvpriv(phl_info);
	struct rtw_payload *tpkt = NULL;
	u32 buf_len = 0, i = 0;
	FUNCIN_WSTS(status);

	do {
		if (0 == tx_pkt_num)
			break;

		tx_pkt_pool->total_cnt = tx_pkt_num;
		buf_len = sizeof(struct rtw_payload) * tx_pkt_num;
		tx_pkt_pool->buf = _os_mem_alloc(drv_priv, buf_len);

		if (NULL == tx_pkt_pool->buf)
			break;
		tx_pkt_pool->buf_len = buf_len;
		INIT_LIST_HEAD(&tx_pkt_pool->idle_list);
		INIT_LIST_HEAD(&tx_pkt_pool->busy_list);
		_os_spinlock_init(drv_priv, &tx_pkt_pool->idle_lock);
		_os_spinlock_init(drv_priv, &tx_pkt_pool->busy_lock);
		tpkt = (struct rtw_payload *)tx_pkt_pool->buf;
		for (i = 0; i < tx_pkt_num; i++) {
			INIT_LIST_HEAD(&tpkt[i].list);
			#ifdef CONFIG_PCI_HCI
			tpkt[i].pkt.vir_addr = _os_shmem_alloc(drv_priv,
						  &tpkt[i].pkt.phy_addr_l,
						  &tpkt[i].pkt.phy_addr_h,
						  tx_pkt_size,
						  false,
						  DMA_TO_DEVICE,
						  &tpkt[i].os_rsvd[0]);
			#else /*USB/SDIO*/
			tpkt[i].pkt.vir_addr = _os_mem_alloc(drv_priv, tx_pkt_size);
			#endif
			if (NULL == tpkt[i].pkt.vir_addr) {
				PHL_WARN("allocate tx pkt buf fail\n");
				break;
			}

			/* hana_todo */
			/* tpkt[i].pkt.phy_addr_l = 0; */
			/* tpkt[i].pkt.phy_addr_h = 0; */
			tpkt[i].pkt.length = (u16)tx_pkt_size;
			tpkt[i].test_id = i;

			list_add_tail(&tpkt[i].list, &tx_pkt_pool->idle_list);
			tx_pkt_pool->idle_cnt++;
		}

		status = RTW_PHL_STATUS_SUCCESS;

	} while (false);
	PHL_INFO("_phl_alloc_tx_pkt_pool : idle counter (%d), busy counter (%d), total counter (%d)\n",
			 tx_pkt_pool->idle_cnt, tx_pkt_pool->busy_cnt, tx_pkt_pool->total_cnt);
	FUNCOUT_WSTS(status);

	return status;
}

struct rtw_payload *_phl_query_idle_tx_pkt(struct phl_info_t *phl_info)
{
	struct phl_trx_test *trx_test = (struct phl_trx_test *)phl_info->trx_test;
	struct rtw_pool *tx_pkt_pool = &trx_test->tx_pkt_pool;
	struct rtw_payload *tpkt = NULL;
	void *drv_priv = phl_to_drvpriv(phl_info);

	_os_spinlock(drv_priv, &tx_pkt_pool->idle_lock, _bh, NULL);

	if (false == list_empty(&tx_pkt_pool->idle_list)) {
		tpkt = list_first_entry(&tx_pkt_pool->idle_list,
				  struct rtw_payload, list);
		list_del(&tpkt->list);
		tx_pkt_pool->idle_cnt--;
	}

	_os_spinunlock(drv_priv, &tx_pkt_pool->idle_lock, _bh, NULL);

	return tpkt;
}

struct rtw_payload *_phl_query_busy_tx_pkt(struct phl_info_t *phl_info)
{
	struct phl_trx_test *trx_test = (struct phl_trx_test *)phl_info->trx_test;
	struct rtw_pool *tx_pkt_pool = &trx_test->tx_pkt_pool;
	struct rtw_payload *tpkt = NULL;
	void *drv_priv = phl_to_drvpriv(phl_info);

	_os_spinlock(drv_priv, &tx_pkt_pool->busy_lock, _bh, NULL);

	if (false == list_empty(&tx_pkt_pool->busy_list)) {
		tpkt = list_first_entry(&tx_pkt_pool->busy_list,
				  struct rtw_payload, list);
		list_del(&tpkt->list);
		tx_pkt_pool->busy_cnt--;
	}

	_os_spinunlock(drv_priv, &tx_pkt_pool->busy_lock, _bh, NULL);

	return tpkt;
}

void _phl_remove_busy_tx_pkt(struct phl_info_t *phl_info, struct rtw_payload *tpkt)
{
	struct phl_trx_test *trx_test = (struct phl_trx_test *)phl_info->trx_test;
	struct rtw_pool *tx_pkt_pool = &trx_test->tx_pkt_pool;
	void *drv_priv = phl_to_drvpriv(phl_info);

	_os_spinlock(drv_priv, &tx_pkt_pool->busy_lock, _bh, NULL);

	if (false == list_empty(&tx_pkt_pool->busy_list)) {
		list_del(&tpkt->list);
		tx_pkt_pool->busy_cnt--;
	}

	_os_spinunlock(drv_priv, &tx_pkt_pool->busy_lock, _bh, NULL);
}


void _phl_release_tx_pkt(struct phl_info_t *phl_info, struct rtw_payload *tpkt)
{
	struct phl_trx_test *trx_test = (struct phl_trx_test *)phl_info->trx_test;
	struct rtw_pool *tx_pkt_pool = &trx_test->tx_pkt_pool;
	void *drv_priv = phl_to_drvpriv(phl_info);

	_os_spinlock(drv_priv, &tx_pkt_pool->idle_lock, _bh, NULL);

	_os_mem_set(drv_priv, tpkt->pkt.vir_addr, 0, tpkt->pkt.length);
	tpkt->pkt.length = 0;

	INIT_LIST_HEAD(&tpkt->list);
	list_add_tail(&tpkt->list, &tx_pkt_pool->idle_list);
	tx_pkt_pool->idle_cnt++;

	_os_spinunlock(drv_priv, &tx_pkt_pool->idle_lock, _bh, NULL);
}

void _phl_insert_busy_tx_pkt(struct phl_info_t *phl_info, struct rtw_payload *tpkt)
{
	struct phl_trx_test *trx_test = (struct phl_trx_test *)phl_info->trx_test;
	struct rtw_pool *tx_pkt_pool = &trx_test->tx_pkt_pool;
	void *drv_priv = phl_to_drvpriv(phl_info);

	_os_spinlock(drv_priv, &tx_pkt_pool->busy_lock, _bh, NULL);

	list_add_tail(&tpkt->list, &tx_pkt_pool->busy_list);
	tx_pkt_pool->busy_cnt++;

	_os_spinunlock(drv_priv, &tx_pkt_pool->busy_lock, _bh, NULL);
}

u8 _phl_is_tx_test_done(void *phl)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_trx_test *trx_test = (struct phl_trx_test *)phl_info->trx_test;
	struct rtw_pool *tx_req_pool = &trx_test->tx_req_pool;
	struct rtw_pool *tx_pkt_pool = &trx_test->tx_pkt_pool;

	if (list_empty(&tx_req_pool->busy_list) && list_empty(&tx_pkt_pool->busy_list))
		return true;
	else
		return false;
}



void phl_update_test_param(void *phl, struct rtw_trx_test_param *test_param)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_trx_test *trx_test = (struct phl_trx_test *)phl_info->trx_test;
	void *drv_priv = phl_to_drvpriv(phl_info);

	_os_mem_cpy(drv_priv, &trx_test->test_param, test_param, sizeof(*test_param));
}

extern enum rtw_phl_status
phl_wifi_role_start(struct phl_info_t *phl_info,
				struct rtw_wifi_role_t *wrole,
				struct rtw_phl_stainfo_t *sta);
enum rtw_phl_status
_phl_test_add_role(void *phl, struct rtw_trx_test_param *test_param)
{
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_FAILURE;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct rtw_t_meta_data *txcap = NULL;
	struct rtw_wifi_role_t *test_wrole = &phl_com->wifi_roles[0];
	struct rtw_wifi_role_link_t *rlink = get_rlink(test_wrole,
	                                               RTW_RLINK_PRIMARY);
	struct rtw_phl_stainfo_t *sta_info = NULL;

	if (NULL != test_param) {
		txcap = &test_param->tx_cap;

		rlink->hw_port = (u8)txcap->macid;

		sta_info = rtw_phl_get_stainfo_by_addr(phl_info,
		                                       test_wrole,
		                                       rlink,
		                                       test_wrole->mac_addr);
		if (NULL != sta_info) {
			test_param->tx_cap.macid = sta_info->macid;
			phl_status = phl_wifi_role_start(phl_info, test_wrole, sta_info);
			PHL_INFO("update test param macid to %d\n", test_param->tx_cap.macid);
		} else {
			PHL_ERR("fail to get stainfo from test wrole!\n");
			phl_status = RTW_PHL_STATUS_FAILURE;
		}
	}

	return phl_status;
}

extern enum rtw_phl_status
phl_wifi_role_stop(struct phl_info_t *phl_info, struct rtw_wifi_role_t *wrole);
enum rtw_phl_status _phl_test_remove_role(
					   void *phl,
				       struct rtw_trx_test_param *test_param)
{
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_FAILURE;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct rtw_wifi_role_t *test_wrole = &phl_com->wifi_roles[0];

	if (NULL != test_param)
		phl_status = phl_wifi_role_stop(phl_info, test_wrole);

	return phl_status;
}

void phl_test_sw_free(void *phl)
{
	FUNCIN();

	_phl_free_tx_pkt_pool(phl);
	_phl_free_rx_req_pool(phl);
	_phl_free_tx_req_pool(phl);

	FUNCOUT();
}


enum rtw_phl_status phl_test_sw_alloc(void *phl)
{
	enum rtw_phl_status sts = RTW_PHL_STATUS_FAILURE;
	FUNCIN_WSTS(sts);
	do {
		sts = _phl_alloc_tx_req_pool(phl, MAX_TEST_TXREQ_NUM);
		if (RTW_PHL_STATUS_SUCCESS != sts)
			break;
		sts = _phl_alloc_rx_req_pool(phl, MAX_TEST_RXREQ_NUM);
		if (RTW_PHL_STATUS_SUCCESS != sts)
			break;
		sts = _phl_alloc_tx_pkt_pool(phl, MAX_TEST_PAYLOAD_NUM,
					     			 MAX_TEST_PAYLOAD_SIZE);

	} while (false);
	FUNCOUT_WSTS(sts);

	return sts;
}


enum rtw_phl_status phl_trx_test_init(void *phl)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_trx_test *trx_test = NULL;
	struct rtw_phl_evt_ops *ops = &phl_info->phl_com->evt_ops;
	void *drv_priv = phl_to_drvpriv(phl_info);
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_FAILURE;

	if (NULL == (trx_test = _os_mem_alloc(drv_priv, sizeof(struct phl_trx_test)))) {
		phl_info->trx_test = NULL;
		PHL_ERR("%s: alloc buffer failed\n", __func__);
		return RTW_PHL_STATUS_FAILURE;
	}

	phl_info->trx_test = trx_test;

	INIT_LIST_HEAD(&trx_test->rx_q);
	_os_mem_set(drv_priv, &trx_test->tx_req_pool, 0, sizeof(trx_test->tx_req_pool));
	_os_mem_set(drv_priv, &trx_test->rx_req_pool, 0, sizeof(trx_test->rx_req_pool));
	_os_mem_set(drv_priv, &trx_test->tx_pkt_pool, 0, sizeof(trx_test->tx_pkt_pool));
	_os_mem_set(drv_priv, &trx_test->test_param, 0, sizeof(trx_test->test_param));
	_os_mem_set(drv_priv, &trx_test->trx_test_obj, 0, sizeof(trx_test->trx_test_obj));
	ops->tx_test_recycle = phl_recycle_test_tx;
	phl_status = phl_test_sw_alloc(phl);
#if 0
	gtest_rxq_handler.type = RTW_PHL_HANDLER_PRIO_HIGH; /* tasklet */
	gtest_rxq_handler.callback = rtw_phl_test_rx_callback;
	gtest_rxq_handler.context = phl_info;
	gtest_rxq_handler.drv_priv = phl_to_drvpriv(phl_info);
	sts = phl_register_handler(phl_info->phl_com, &gtest_rxq_handler);
	if (RTW_PHL_STATUS_SUCCESS != sts) {
		PHL_ERR("register test rx queue handler fail\n");
		phl_deregister_handler(phl_info->phl_com, &gtest_rxq_handler);
	}
#endif
	return phl_status;
}

void phl_trx_test_deinit(void *phl)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	void *drv = phl_to_drvpriv(phl_info);

	phl_test_sw_free(phl);

	_os_mem_free(drv, phl_info->trx_test, sizeof(struct phl_trx_test));
	phl_info->trx_test = NULL;
}


void phl_test_hw_config_init(void *phl, u8 mode)
{
	switch (mode) {
	case TEST_MODE_PHL_TX_RING_TEST:
		/* address cam receive all */
		/* rtl8852a_hal_init_misc: */
		/* rtl8852ae_test_loopback: */
		break;
	default:
		break;

	}


}


void phl_test_hw_config_runtime(void *phl, u8 mode)
{
	switch (mode) {
	case TEST_MODE_PHL_TX_RING_TEST:
		/* debug register :*/
		/* edca config */
		/* zero delimiter counter flush */
		break;
	default:
		break;

	}


}


void phl_test_fill_packet_content(struct phl_info_t *phl_info, u8 *pkt,
				  u16 size,
				  struct rtw_trx_test_param *test_param)
{
	void *drv_priv = phl_to_drvpriv(phl_info);
	u8 test_pattern[7] = {0x00, 0x01, 0x02, 0x55, 0x66, 0x94, 0x87};
	u16 qos_ofst = 0, payload_ofst = 0;

	/* wlan header */
	SET_WHDR_PROTOCOL_VERSION(pkt, 0);
	SET_WHDR_TYPE(pkt, 2); //data
	SET_WHDR_SUBTYPE(pkt, 0);
	if (true == test_param->ap_mode) {
		SET_WHDR_TO_DS(pkt, 0);
		SET_WHDR_FROM_DS(pkt, 1);
		SET_WHDR_ADDRESS1(drv_priv, pkt, test_param->sta_addr);
		SET_WHDR_ADDRESS2(drv_priv, pkt, test_param->bssid);
		SET_WHDR_ADDRESS3(drv_priv, pkt, test_param->cur_addr);
	} else {
		SET_WHDR_TO_DS(pkt, 1);
		SET_WHDR_FROM_DS(pkt, 0);
		SET_WHDR_ADDRESS1(drv_priv, pkt, test_param->bssid);
		SET_WHDR_ADDRESS2(drv_priv, pkt, test_param->cur_addr);
		SET_WHDR_ADDRESS3(drv_priv, pkt, test_param->sta_addr);
	}

	SET_WHDR_MORE_FRAG(pkt, 0);
	SET_WHDR_RETRY(pkt, 0);
	SET_WHDR_PWR_MGNT(pkt, 0);
	SET_WHDR_MORE_DATA(pkt, 0);
	SET_WHDR_WEP(pkt, 0);
	SET_WHDR_ORDER(pkt, 0);

	SET_WHDR_DURATION(pkt, 0);
	SET_WHDR_FRAGMENT_SEQUENCE(pkt, 0);

	qos_ofst = 24;

	if (true == test_param->qos) {
		SET_WHDR_QOS_EN(pkt, 1);
		SET_WHDR_QOS_CTRL_STA_DATA_TID(pkt + qos_ofst,
					       test_param->tx_cap.tid);
		SET_WHDR_QOS_CTRL_STA_DATA_EOSP(pkt + qos_ofst, 0);
		SET_WHDR_QOS_CTRL_STA_DATA_ACK_POLICY(pkt + qos_ofst, 0);
		SET_WHDR_QOS_CTRL_STA_DATA_AMSDU(pkt + qos_ofst, 0);
		SET_WHDR_QOS_CTRL_STA_DATA_TXOP(pkt + qos_ofst, 0);
	}
	/* wlan payload */
	payload_ofst = qos_ofst + WHDR_QOS_LENGTH;
	_os_mem_cpy(drv_priv, pkt + payload_ofst, test_pattern,
		    sizeof(test_pattern));

	debug_dump_data(pkt, size, "phl trx test pattern");
}

void rtw_phl_test_rx_callback(void *context)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)context;
	struct phl_trx_test *trx_test = (struct phl_trx_test *)phl_info->trx_test;
	struct rtw_pool *rx_req_pool = &trx_test->rx_req_pool;
	struct rtw_test_rx *rreq = NULL;
	struct rtw_recv_pkt *recvpkt = NULL;
	struct rtw_payload *tpkt = NULL;

	while (!list_empty(&rx_req_pool->busy_list)) {
		rreq = _phl_query_busy_rx_req(phl_info);
		if (NULL == rreq) {
			PHL_WARN("Get NULL rx request from busy queue\n");
			break;
		}
		recvpkt = &rreq->rx;
		PHL_INFO("Rx test_id = %d\n", rreq->test_id);
		PHL_INFO("Rx mac id = %d\n", recvpkt->mdata.macid);
		PHL_INFO("Rx tid = %d\n", recvpkt->mdata.tid);
		debug_dump_data(recvpkt->mdata.mac_addr, 6, "Rx mac address: ");
		debug_dump_data(recvpkt->pkt_list[0].vir_addr,
				recvpkt->pkt_list[0].length, "Rx Pkt: ");

		_phl_release_rx_req(phl_info, rreq);
		tpkt = (struct rtw_payload *)rreq->tpkt;
	        _phl_release_tx_pkt(phl_info, tpkt);

	}

}


enum rtw_phl_status rtw_phl_rx_reap(void *phl, u8 *xmit_req,
				    struct rtw_trx_test_param *param)
{
	enum rtw_phl_status sts = RTW_PHL_STATUS_FAILURE;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct rtw_xmit_req *treq = (struct rtw_xmit_req *)xmit_req;
	struct rtw_payload *tpkt = NULL;
	struct rtw_test_rx *rreq = NULL;
	struct rtw_recv_pkt *recvpkt = NULL;
	struct rtw_pkt_buf_list *pkt = NULL;
	void *drv_priv = phl_to_drvpriv(phl_info);
	u8 i = 0;

	rreq = _phl_query_idle_rx_req(phl_info);

	do {
		if (NULL == rreq) {
			PHL_ERR("query idle rx request fail!\n");
			break;
		}

		recvpkt = &rreq->rx;
		recvpkt->mdata.macid = treq->mdata.macid;
		recvpkt->mdata.tid = treq->mdata.tid;
		recvpkt->mdata.hal_port = treq->mdata.hal_port;
		recvpkt->mdata.dma_ch = 0; /* normal rx ch */
		_os_mem_cpy(drv_priv, recvpkt->mdata.mac_addr, param->sta_addr,
			    6);
		recvpkt->mdata.seq = treq->mdata.sw_seq;
		recvpkt->mdata.mc = treq->mdata.mc;
		recvpkt->mdata.bc = treq->mdata.bc;

		pkt = (struct rtw_pkt_buf_list *)treq->pkt_list;

		tpkt = phl_container_of(pkt, struct rtw_payload, pkt);
		for (i = 0; i < treq->pkt_cnt; i++) {
			if (i >= MAX_RX_BUF_SEG_NUM) {
				PHL_ERR("tx packet has too many segments\n");
				break;
			}

			recvpkt->pkt_list[i].vir_addr = pkt->vir_addr;
			recvpkt->pkt_list[i].phy_addr_l = pkt->phy_addr_l;
			recvpkt->pkt_list[i].phy_addr_h = pkt->phy_addr_h;
			recvpkt->pkt_list[i].length = pkt->length;

			recvpkt->pkt_cnt++;

			pkt++;
		}

		rreq->tpkt = (u8 *)tpkt;
		rreq->test_id = tpkt->test_id;

		_phl_insert_busy_rx_req(phl_info, rreq);
		sts = RTW_PHL_STATUS_SUCCESS;


	} while (false);

	if (RTW_PHL_STATUS_SUCCESS == sts) {
		_phl_release_tx_req(phl_info, treq);
	}

	return sts;
}

enum rtw_phl_status rtw_phl_test_rxq_notify(void *phl)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;

	pstatus = phl_schedule_handler(phl_info->phl_com,
				       &phl_info->phl_tx_handler);

	return pstatus;
}


enum rtw_phl_status phl_tx_ring_test(void *phl,
				     struct rtw_trx_test_param *test_param)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	void *drv_priv = phl_to_drvpriv(phl_info);
	enum rtw_phl_status sts = RTW_PHL_STATUS_FAILURE;
	struct rtw_xmit_req *treq = NULL;
	struct rtw_payload *tpkt = NULL;
	u32 tx_cnt = 0;

	FUNCIN_WSTS(sts);
	do {
		for (tx_cnt = 0; tx_cnt < test_param->tx_req_num; tx_cnt++) {
			/* query tx request pool */
			treq = _phl_query_idle_tx_req(phl_info);
			if (NULL == treq) {
				PHL_WARN("query idle tx request from pool fail\n");
				break;
			}

			tpkt = _phl_query_idle_tx_pkt(phl_info);
			if (NULL == tpkt) {
				PHL_WARN("query idle tx packet from pool fail\n");
				break;
			}

			/* fill meta_data*/
			_os_mem_cpy(drv_priv, &treq->mdata, &test_param->tx_cap,
						sizeof(struct rtw_t_meta_data));
			/* fill tx request content */
			if (test_param->tx_payload_size > MAX_TEST_PAYLOAD_SIZE)
				tpkt->pkt.length = MAX_TEST_PAYLOAD_SIZE;
			else
				tpkt->pkt.length = (u16)test_param->tx_payload_size;

			phl_test_fill_packet_content(phl_info, tpkt->pkt.vir_addr,
							 tpkt->pkt.length, test_param);
			/* assign this tx pkt to tx request */
			treq->os_priv = tpkt;
			treq->pkt_cnt = 1;
			treq->mdata.wdinfo_en = 1;
			treq->total_len = (u16)tpkt->pkt.length;
			treq->pkt_list = (u8 *)&tpkt->pkt;

			/* add to phl_tx_ring */
			sts = rtw_phl_add_tx_req(phl, treq);
			if (RTW_PHL_STATUS_SUCCESS != sts) {
				PHL_INFO("add new tx request (%d) to phl ring fail\n", tx_cnt);
				_phl_release_tx_req(phl_info, treq);
				_phl_release_tx_pkt(phl_info, tpkt);
				break;
			} else {
				_phl_insert_busy_tx_req(phl_info, treq);
				_phl_insert_busy_tx_pkt(phl_info, tpkt);
			}
		}
		/* schedule tx process */
		sts = rtw_phl_tx_req_notify(phl);
		if (RTW_PHL_STATUS_SUCCESS != sts) {
			PHL_INFO("add notify phl start tx process fail\n");
			break;
		}
	} while (false);
/*
	while (false == _phl_is_tx_test_done(phl) && i < 100) {
		_os_delay_ms(drv_priv, 1);
		i++;
	}
*/
	FUNCOUT_WSTS(sts);
	return sts;
}

enum rtw_phl_status phl_rx_ring_test(void *phl,
				     struct rtw_trx_test_param *test_param)
{
	enum rtw_phl_status sts = RTW_PHL_STATUS_FAILURE;
	FUNCIN_WSTS(sts);
	do {
	  	sts = rtw_phl_start_rx_process(phl);

		if (RTW_PHL_STATUS_FAILURE == sts) {
			PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_, "[WARNING] phl_rx ring fail!\n");
			break;
		}
	} while (false);
	FUNCOUT_WSTS(sts);
	return sts;
}


enum rtw_phl_status phl_hal_tx_test(void *phl,
				     struct rtw_trx_test_param *test_param)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	void *drv_priv = phl_to_drvpriv(phl_info);
	struct phl_hci_trx_ops *hci_trx_ops = phl_info->hci_trx_ops;
	enum rtw_phl_status sts = RTW_PHL_STATUS_FAILURE;
	struct rtw_xmit_req *treq = NULL;
	struct rtw_payload *tpkt = NULL;
	FUNCIN_WSTS(sts);
	do {
		/* query tx request pool */
		treq = _phl_query_idle_tx_req(phl_info);
		if (NULL == treq) {
			PHL_WARN("query idle tx request from pool fail\n");
			break;
		}

		tpkt = _phl_query_idle_tx_pkt(phl_info);
		if (NULL == tpkt) {
			PHL_WARN("query idle tx packet from pool fail\n");
			break;
		}

		/* fill meta_data */
		_os_mem_cpy(drv_priv, &treq->mdata, &test_param->tx_cap,
					sizeof(struct rtw_t_meta_data));
		/* fill tx request content */
		if (test_param->tx_payload_size > MAX_TEST_PAYLOAD_SIZE)
			tpkt->pkt.length = MAX_TEST_PAYLOAD_SIZE;
		else
			tpkt->pkt.length = (u16)test_param->tx_payload_size;

		phl_test_fill_packet_content(phl_info, tpkt->pkt.vir_addr,
					     tpkt->pkt.length, test_param);
		/* assign this tx pkt to tx request */
		treq->os_priv = tpkt;
		treq->pkt_cnt = 1;
		treq->mdata.wdinfo_en = 1;
		treq->total_len = (u16)tpkt->pkt.length;
		treq->pkt_list = (u8 *)&tpkt->pkt;
		/* add to phl_tx_ring */
		sts = hci_trx_ops->prepare_tx(phl_info, treq);
		if (RTW_PHL_STATUS_SUCCESS != sts) {
			PHL_INFO("add new tx request to phl_prepare_tx_pcie fail\n");
			break;
		}
		/* schedule tx process */
		sts = hci_trx_ops->tx(phl_info);
		if (RTW_PHL_STATUS_SUCCESS != sts) {
			PHL_INFO("phl_tx_pcie fail\n");
			break;
		}
	} while (false);

		_os_delay_ms(drv_priv, 100);

	if (RTW_PHL_STATUS_SUCCESS != sts) {
		if (NULL != treq)
			_phl_release_tx_req(phl, treq);
		if (NULL != tpkt)
			_phl_release_tx_pkt(phl, tpkt);
	}
	FUNCOUT_WSTS(sts);
	return sts;
}

enum rtw_phl_status phl_hal_rx_test(void *phl,
				     struct rtw_trx_test_param *test_param)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_hci_trx_ops *hci_trx_ops = phl_info->hci_trx_ops;
	enum rtw_phl_status sts = RTW_PHL_STATUS_FAILURE;
	FUNCIN_WSTS(sts);
	do {

		/* add to phl_tx_ring */
		sts = hci_trx_ops->rx(phl_info);

		if (RTW_PHL_STATUS_FAILURE == sts) {
			PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_, "[WARNING] phl_rx fail!\n");
			break;
		}
	} while (false);
	FUNCOUT_WSTS(sts);
	return sts;
}


enum rtw_phl_status phl_trx_test_start(void *phl,
				       struct rtw_trx_test_param *test_param)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;
	FUNCIN_WSTS(status);
	switch (test_param->mode) {
	case TEST_MODE_PHL_TX_RING_TEST:
		status = phl_tx_ring_test(phl, test_param);
		break;
	case TEST_MODE_PHL_RX_RING_TEST:
		status = phl_rx_ring_test(phl, test_param);
		break;
	case TEST_MODE_HAL_TX_TEST:
		status = phl_hal_tx_test(phl, test_param);
		break;
	case TEST_MODE_HAL_RX_TEST:
		status = phl_hal_rx_test(phl, test_param);
		break;
	default:
		break;

	}
	FUNCOUT_WSTS(status);
	return status;
}

void phl_trx_test_dump_result(void *phl, struct rtw_trx_test_param *test_param)
{
	PHL_INFO("Test Done");
}

enum rtw_phl_status phl_recycle_test_tx(void *phl, struct rtw_xmit_req *treq)
{
	struct phl_info_t *phl_info = NULL;
	struct rtw_payload *tpkt = NULL;
	enum rtw_phl_status sts = RTW_PHL_STATUS_FAILURE;

	FUNCIN_WSTS(sts);
	if (NULL == phl) {
		PHL_ERR("treq is NULL!\n");
		goto end;
	}
	phl_info = (struct phl_info_t *)phl;

	if (NULL == treq) {
		PHL_ERR("treq is NULL!\n");
		goto end;
	}
	tpkt = (struct rtw_payload *)treq->os_priv;

	if (NULL == tpkt) {
		PHL_ERR("tpkt is NULL!\n");
		goto end;
	}
	_phl_remove_busy_tx_req(phl_info, treq);
	_phl_release_tx_req(phl_info, treq);

	_phl_remove_busy_tx_pkt(phl_info, tpkt);
	_phl_release_tx_pkt(phl_info, tpkt);

	sts = RTW_PHL_STATUS_SUCCESS;

end:
	FUNCOUT_WSTS(sts);
	return sts;
}

void _phl_rx_test_pattern(struct phl_info_t *phl_info, void *ptr)
{
	struct rtw_recv_pkt *rpkt = NULL;

	FUNCIN();

	if (NULL == ptr) {
		PHL_ERR("bp_info->ptr is NULL!\n");
		goto end;
	}

	rpkt = (struct rtw_recv_pkt *)ptr;
	if (NULL == rpkt) {
		PHL_ERR("rpkt is NULL!\n");
		goto end;
	}

	PHL_INFO("rpkt->buf_len = %d\n", rpkt->pkt_list[0].length);
	debug_dump_data(rpkt->pkt_list[0].vir_addr, (u16)rpkt->pkt_list[0].length, "dump_rx");

end:
	FUNCOUT();
}

void rtw_phl_trx_default_param(void *phl, struct rtw_trx_test_param *test_param)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	_os_mem_set(phl_to_drvpriv(phl_info), test_param, 0, sizeof(test_param));
	test_param->mode = TEST_MODE_PHL_TX_RING_TEST;
	test_param->ap_mode = 0;
	test_param->pkt_type = TEST_PKT_TYPE_BC;
	test_param->tx_req_num = 1;
	test_param->rx_req_num = 1;
	test_param->tx_payload_num = 1;
	test_param->tx_payload_size = 100;
	test_param->trx_mode = 0;
	test_param->qta_mode = 0;
	test_param->cur_addr[0] = 0x00;
	test_param->cur_addr[1] = 0xE0;
	test_param->cur_addr[2] = 0x4c;
	test_param->cur_addr[3] = 0x88;
	test_param->cur_addr[4] = 0x52;
	test_param->cur_addr[5] = 0xaa;
	if (TEST_PKT_TYPE_BC == test_param->pkt_type || TEST_PKT_TYPE_MC == test_param->pkt_type) {
		test_param->sta_addr[0] = 0xFF;
		test_param->sta_addr[1] = 0xFF;
		test_param->sta_addr[2] = 0xFF;
		test_param->sta_addr[3] = 0xFF;
		test_param->sta_addr[4] = 0xFF;
		test_param->sta_addr[5] = 0xFF;
		test_param->bssid[0] = 0xFF;
		test_param->bssid[1] = 0xFF;
		test_param->bssid[2] = 0xFF;
		test_param->bssid[3] = 0xFF;
		test_param->bssid[4] = 0xFF;
		test_param->bssid[5] = 0xFF;
		test_param->tx_cap.bc = 1;
		test_param->qos = 0;
	} else if (TEST_PKT_TYPE_UNI == test_param->pkt_type){
		test_param->sta_addr[0] = 0x00;
		test_param->sta_addr[1] = 0xE0;
		test_param->sta_addr[2] = 0x4C;
		test_param->sta_addr[3] = 0x88;
		test_param->sta_addr[4] = 0x52;
		test_param->sta_addr[5] = 0xbb;
		test_param->bssid[0] = 0x00;
		test_param->bssid[1] = 0xE0;
		test_param->bssid[2] = 0x4C;
		test_param->bssid[3] = 0x88;
		test_param->bssid[4] = 0x52;
		test_param->bssid[5] = 0xbb;
		test_param->tx_cap.bc = 0;
		test_param->qos = 1;
	}
	test_param->tx_cap.macid= 0x00;
	test_param->tx_cap.tid = 0x03;
	test_param->tx_cap.dma_ch= 0x01;
	test_param->tx_cap.band= 0x0;
	test_param->tx_cap.userate_sel = 0x1;
	test_param->tx_cap.f_rate = 0x87;
	test_param->tx_cap.f_bw = 0;
	test_param->tx_cap.f_gi_ltf = 0;
}

enum rtw_phl_status rtw_phl_trx_testsuite(void *phl,
				   struct rtw_trx_test_param *test_param)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_trx_test *trx_test = (struct phl_trx_test *)phl_info->trx_test;
	struct rtw_trx_test_param *param = &trx_test->test_param;
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;
	FUNCIN_WSTS(status);
	do {
		param->is_trx_test_end = false;
		phl_update_test_param(phl, test_param);

		phl_test_hw_config_init(phl, test_param->mode);

		status = phl_trx_test_start(phl, test_param);
		if (RTW_PHL_STATUS_SUCCESS != status) {
			PHL_ERR("The phl trx test failure\n");

			break;
		}

		phl_trx_test_dump_result(phl, test_param);

		param->is_trx_test_end = true;
	} while (false);
	FUNCOUT_WSTS(status);
	return status;
}


u8 trx_test_bp_handler(void *priv, struct test_bp_info* bp_info)
{
	u8 ret = BP_RET_SKIP_SECTION;

	return ret;
}


u8 trx_test_is_test_end(void *priv)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)priv;
	struct phl_trx_test *trx_test = (struct phl_trx_test *)phl_info->trx_test;
	struct rtw_trx_test_param *test_param = &trx_test->test_param;
	FUNCIN();
	FUNCOUT();
	return (test_param->is_trx_test_end == true) ? (true) : (false);
}
u8 trx_test_is_test_pass(void *priv)
{
	FUNCIN();
	FUNCOUT();
	return true;
}

u8 trx_test_get_fail_rsn(void *priv, char* rsn, u32 max_len)
{
	//struct phl_info_t *phl_info = (struct phl_info_t *)priv;
	FUNCIN();
	FUNCOUT();
	return true;

}

u8 trx_test_start_test(void *priv)
{
	//struct phl_info_t *phl_info = (struct phl_info_t *)priv;
	FUNCIN();
	FUNCOUT();
	return true;
}


void
phl_add_trx_test_obj(void *phl)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_trx_test *trx_test = (struct phl_trx_test *)phl_info->trx_test;
	struct test_obj_ctrl_interface *trx_test_obj = &trx_test->trx_test_obj;
	FUNCIN();
	trx_test_obj->bp_handler = trx_test_bp_handler;
	trx_test_obj->get_fail_rsn = trx_test_get_fail_rsn;
	trx_test_obj->is_test_end = trx_test_is_test_end;
	trx_test_obj->is_test_pass = trx_test_is_test_pass;
	trx_test_obj->start_test = trx_test_start_test;
	rtw_phl_test_add_new_test_obj(phl_info->phl_com,
	                              "tx_test",
	                              phl_info,
	                              TEST_LVL_HIGH,
	                              trx_test_obj,
	                              -1,
	                              TEST_SUB_MODULE_TRX,
	                              UNIT_TEST_MODE);
	FUNCOUT();
}

#endif /* #ifdef CONFIG_PHL_TEST_SUITE */

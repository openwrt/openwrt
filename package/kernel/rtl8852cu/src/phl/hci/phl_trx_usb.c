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
#define _PHL_TRX_USB_C_
#include "../phl_headers.h"
#include "phl_trx_usb.h"

#define IDX_NONE (0xffffffff)
#define GET_SHIFT_OFFSET(_bulk_size) \
	((_bulk_size) == USB_SUPER_SPEED_BULK_SIZE) ? (10) : \
	(((_bulk_size) == USB_HIGH_SPEED_BULK_SIZE) ? (9) : (6))

enum rtw_phl_status
phl_release_rxbuf_usb(struct phl_info_t *phl_info, void *r, u8 ch, enum rtw_rx_type type);

static enum rtw_phl_status enqueue_usb_buf(
					struct phl_info_t *phl_info,
					struct phl_queue	*pool_list,
					struct phl_usb_buf *usb_buf, u8 pos)
{
	void *drv = phl_to_drvpriv(phl_info);
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	_os_spinlockfg sp_flags;

	if (usb_buf != NULL) {
		_os_spinlock(drv, &pool_list->lock, _irq, &sp_flags);
		if (_tail == pos)
			list_add_tail(&usb_buf->list, &pool_list->queue);
		else if (_first == pos)
			list_add(&usb_buf->list, &pool_list->queue);
		pool_list->cnt++;
		_os_spinunlock(drv, &pool_list->lock, _irq, &sp_flags);

		pstatus = RTW_PHL_STATUS_SUCCESS;
	}

	return pstatus;

}

static struct phl_usb_buf *dequeue_usb_buf(struct phl_info_t *phl_info,
	struct phl_queue *pool_list)
{
	void *drv = phl_to_drvpriv(phl_info);
	struct phl_usb_buf *usb_buf = NULL;
	_os_spinlockfg sp_flags;

	_os_spinlock(drv, &pool_list->lock, _irq, &sp_flags);
	if (list_empty(&pool_list->queue)) {
		usb_buf = NULL;
	} else {
		usb_buf = list_first_entry(&pool_list->queue, struct phl_usb_buf, list);

		list_del(&usb_buf->list);
		pool_list->cnt--;
	}
	_os_spinunlock(drv, &pool_list->lock, _irq, &sp_flags);
	return usb_buf;
}

static void _phl_free_txbuf_usb(struct phl_info_t *phl_info,
	struct phl_usb_buf *tx_buf)
{
	void *drv = phl_to_drvpriv(phl_info);

	if (NULL != tx_buf) {
		if (tx_buf->buffer) {
			_os_kmem_free(drv, tx_buf->buffer, tx_buf->buf_len);
			tx_buf->buffer = NULL;
		}
		_os_mem_free(drv, tx_buf, sizeof(struct phl_usb_buf));
		tx_buf = NULL;
	}
}


static void _phl_free_txbuf_pool_usb(struct phl_info_t *phl_info, u8 *txbuf_pool)
{
	struct phl_usb_tx_buf_resource *ring = NULL;
	struct phl_usb_buf *tx_buf = NULL;
	FUNCIN();

	ring = (struct phl_usb_tx_buf_resource *)txbuf_pool;
	if (NULL != ring) {
		while (1) {
			tx_buf = dequeue_usb_buf(phl_info, &ring->idle_txbuf_list);
			if (tx_buf == NULL)
				break;

			_phl_free_txbuf_usb(phl_info, tx_buf);
		}
		while (1) {
			tx_buf = dequeue_usb_buf(phl_info, &ring->mgmt_txbuf_list);
			if (tx_buf == NULL)
				break;

			_phl_free_txbuf_usb(phl_info, tx_buf);
		}
		while (1) {
			tx_buf = dequeue_usb_buf(phl_info, &ring->h2c_txbuf_list);
			if (tx_buf == NULL)
				break;

			_phl_free_txbuf_usb(phl_info, tx_buf);
		}
		_os_mem_free(phl_to_drvpriv(phl_info), ring,
				sizeof(struct phl_usb_tx_buf_resource));
		ring = NULL;
	}

	FUNCOUT();
}


static struct phl_usb_buf *
_phl_alloc_txbuf_usb(struct phl_info_t *phl_info, u32 tx_buf_size)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	struct phl_usb_buf *tx_buf = NULL;

	tx_buf = _os_mem_alloc(phl_to_drvpriv(phl_info), sizeof(struct phl_usb_buf));
	if (tx_buf != NULL) {
		tx_buf->buffer= _os_kmem_alloc(phl_to_drvpriv(phl_info),
			tx_buf_size);
		if (NULL == tx_buf->buffer) {
			pstatus = RTW_PHL_STATUS_RESOURCE;
			goto exit;
		}
		tx_buf->buf_len = tx_buf_size;
		INIT_LIST_HEAD(&tx_buf->list);
		pstatus = RTW_PHL_STATUS_SUCCESS;
	}

	return tx_buf;

exit:
	if (RTW_PHL_STATUS_SUCCESS != pstatus)
		_phl_free_txbuf_usb(phl_info, tx_buf);

	return tx_buf;
}

static enum rtw_phl_status
_phl_alloc_txbuf_pool_usb(struct phl_info_t *phl_info)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	struct phl_usb_tx_buf_resource *tx_buf_ring = NULL;
	struct phl_usb_buf *tx_buf = NULL;
	struct rtw_hal_com_t *hal_com = rtw_hal_get_halcom(phl_info->hal);
	struct bus_cap_t *bus_cap = &hal_com->bus_cap;
	u32 i;
	FUNCIN_WSTS(pstatus);

	tx_buf_ring = _os_mem_alloc(phl_to_drvpriv(phl_info), sizeof(struct phl_usb_tx_buf_resource));

	if (NULL != tx_buf_ring) {
		INIT_LIST_HEAD(&tx_buf_ring->mgmt_txbuf_list.queue);
		INIT_LIST_HEAD(&tx_buf_ring->idle_txbuf_list.queue);
		INIT_LIST_HEAD(&tx_buf_ring->h2c_txbuf_list.queue);
		_os_spinlock_init(phl_to_drvpriv(phl_info), &tx_buf_ring->mgmt_txbuf_list.lock);
		_os_spinlock_init(phl_to_drvpriv(phl_info), &tx_buf_ring->idle_txbuf_list.lock);
		_os_spinlock_init(phl_to_drvpriv(phl_info), &tx_buf_ring->h2c_txbuf_list.lock);
		for (i = 0; i < bus_cap->tx_buf_num; i++) {

			/*YiWei_todo  tx_buf_size need do n bytes aligment*/
			tx_buf = _phl_alloc_txbuf_usb(phl_info, bus_cap->tx_buf_size);
			if (NULL == tx_buf) {
				pstatus = RTW_PHL_STATUS_RESOURCE;
				goto exit;
			}
			tx_buf->type = RTW_PHL_PKT_TYPE_DATA;
			enqueue_usb_buf(phl_info, &tx_buf_ring->idle_txbuf_list, tx_buf, _tail);
			pstatus = RTW_PHL_STATUS_SUCCESS;
		}
		for (i = 0; i < bus_cap->tx_mgnt_buf_num; i++) {
			/*YiWei_todo  tx_buf_size need do n bytes aligment*/
			tx_buf = _phl_alloc_txbuf_usb(phl_info, bus_cap->tx_mgnt_buf_size);
			if (NULL == tx_buf) {
				pstatus = RTW_PHL_STATUS_RESOURCE;
				goto exit;
			}
			tx_buf->type = RTW_PHL_PKT_TYPE_MGNT;
			enqueue_usb_buf(phl_info, &tx_buf_ring->mgmt_txbuf_list, tx_buf, _tail);
			pstatus = RTW_PHL_STATUS_SUCCESS;
		}
		for (i = 0; i < bus_cap->tx_h2c_buf_num; i++) {

			tx_buf = _os_mem_alloc(phl_to_drvpriv(phl_info),
				sizeof(struct phl_usb_buf));
			if (NULL == tx_buf) {
				pstatus = RTW_PHL_STATUS_RESOURCE;
				goto exit;
			}
			tx_buf->type = RTW_PHL_PKT_TYPE_H2C;
			enqueue_usb_buf(phl_info, &tx_buf_ring->h2c_txbuf_list, tx_buf, _tail);
			pstatus = RTW_PHL_STATUS_SUCCESS;
		}
		/*PHL_INFO("%s, mgmt_buf_cnt = %d\n", __func__,
			tx_buf_ring->mgmt_txbuf_list.cnt);
		PHL_INFO("%s, idle_buf_cnt = %d\n", __func__,
			tx_buf_ring->idle_txbuf_list.cnt);
		PHL_INFO("%s, h2c_buf_cnt = %d\n", __func__,
			tx_buf_ring->h2c_txbuf_list.cnt);*/
	}

	if (RTW_PHL_STATUS_SUCCESS == pstatus) {
		phl_info->hci->txbuf_pool = (u8 *)tx_buf_ring;
	}
	FUNCOUT_WSTS(pstatus);
exit:
	if (RTW_PHL_STATUS_SUCCESS != pstatus)
		_phl_free_txbuf_pool_usb(phl_info, (u8 *)tx_buf_ring);

	return pstatus;

}


void _phl_fill_tx_meta_data_usb(struct rtw_t_meta_data *mdata,
					u16 packet_len)
{
	mdata->wp_offset = 0;
	mdata->wd_page_size = 1;
	mdata->pktlen = packet_len;
	mdata->hw_amsdu = 0;
	mdata->bk = 1;
	mdata->ampdu_en = 0;
}

/*	[SD7_Ref] HalUsbInMpdu	*/
enum rtw_phl_status _phl_in_token_usb(struct phl_info_t *phl_info, u8 pipe_idx)
{
	void *drv = phl_to_drvpriv(phl_info);
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_FAILURE;
	struct hci_info_t *hci_info = (struct hci_info_t *)phl_info->hci;
	struct rtw_rx_buf_ring *rx_buf_ring = (struct rtw_rx_buf_ring *)hci_info->rxbuf_pool;
	u8	rx_desc_sz=0;
	struct rtw_rx_buf *rx_buf = NULL;
	_os_list* obj = NULL;

	//PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "[1] %s:: 000 idle_rxbuf_list.cnt=%d\n",
	//          __FUNCTION__, rx_buf_ring->idle_rxbuf_list.cnt);

	if(pq_pop(drv, &rx_buf_ring->idle_rxbuf_list, &obj, _first, _irq)) {
		rx_buf = (struct rtw_rx_buf*)obj;
	}
	else
		return RTW_PHL_STATUS_RESOURCE;

	PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "[1] %s:: ==> [%p] idle_rxbuf_list.cnt=%d\n",
	          __func__, rx_buf, rx_buf_ring->idle_rxbuf_list.cnt);

	hstatus = rtw_hal_query_info(phl_info->hal, RTW_HAL_RXDESC_SIZE, &rx_desc_sz);
	if(RTW_HAL_STATUS_FAILURE == hstatus)
		return RTW_PHL_STATUS_FAILURE;

	/* Send the bulk IN request down.	*/
	rx_buf->pipe_idx = pipe_idx;

	pq_push(drv, &rx_buf_ring->busy_rxbuf_list, &rx_buf->list, _tail, _irq);

	if(os_send_usb_in_token(drv, (void*)rx_buf, rx_buf->buffer,
		rx_buf->buf_len, pipe_idx, rx_desc_sz) != RTW_PHL_STATUS_SUCCESS)
	{
		/*	TODO::	temp::error hanlding	*/
		PHL_TRACE(COMP_PHL_RECV, _PHL_WARNING_, "[1] %s:: [Error] os_send_usb_in_token\n",
		          __func__);
		pq_del_node(drv, &rx_buf_ring->busy_rxbuf_list, &rx_buf->list, _irq);
		phl_release_rxbuf_usb(phl_info, rx_buf, 0, RTW_RX_TYPE_MAX); // usb doesn't care rtw_rx_type

		return RTW_PHL_STATUS_FAILURE;
	}

	PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "[1] %s:: <== [%p] busy_rxbuf_list.cnt=%d\n",
	          __func__, rx_buf, rx_buf_ring->busy_rxbuf_list.cnt);

	return RTW_PHL_STATUS_SUCCESS;
}

enum rtw_phl_status _phl_rx_start_usb(struct phl_info_t *phl_info)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_SUCCESS;
	struct hal_spec_t *hal_spec = phl_get_ic_spec(phl_info->phl_com);
	struct hci_info_t *hci_info = (struct hci_info_t *)phl_info->hci;
	struct rtw_rx_buf_ring *rx_buf_ring = (struct rtw_rx_buf_ring *)hci_info->rxbuf_pool;
	struct rtw_hal_com_t *hal_com = rtw_hal_get_halcom(phl_info->hal);
	struct bus_cap_t *bus_cap = &hal_com->bus_cap;
	u8 pipe_idx = 0, fail_cnt = 0;

	for (pipe_idx = 0; pipe_idx < hal_spec->max_bulkin_num ; pipe_idx++) {

		/* Send the bulk IN request down. */
		if (pipe_idx == WLAN_IN_MPDU_PIPE_IDX )
		{
			while (rx_buf_ring->busy_rxbuf_list.cnt < (int)(bus_cap->in_token_num))
			{
				PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "[0] %s:: rx_buf_ring->busy_rxbuf_list.cnt =%d\n",
				          __func__, rx_buf_ring->busy_rxbuf_list.cnt );

				pstatus = _phl_in_token_usb(phl_info, pipe_idx);

				if (pstatus == RTW_PHL_STATUS_RESOURCE) {
					break;
				} else if (pstatus != RTW_PHL_STATUS_SUCCESS) {
					fail_cnt++;
					if (fail_cnt > bus_cap->in_token_num)
						break;
				}
			}
		}
		/*else if ( pipe_idx == WLAN_IN_INTERRUPT_PIPE_IDX )
		{
			if (_phl_in_token_usb(phl_info, pipe_idx) != RTW_PHL_STATUS_SUCCESS)
			{
				pstatus = RTW_PHL_STATUS_FAILURE;
				break;
			}
		}*/
	}

	return pstatus;
}


void _phl_rx_stop_usb(struct phl_info_t *phl_info)
{
	void *drv = phl_to_drvpriv(phl_info);
	struct hci_info_t *hci_info = (struct hci_info_t *)phl_info->hci;
	struct rtw_rx_buf_ring *rx_buf_ring = (struct rtw_rx_buf_ring *)hci_info->rxbuf_pool;

	_os_cancel_timer(drv, &rx_buf_ring->deferred_timer);
}


/* [ERROR Handling]: driver call this function to check pipe status */
static void _phl_rx_deferred_in_token(void *phl)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_SUCCESS;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	void *drv = phl_to_drvpriv(phl_info);
	struct hci_info_t *hci_info = (struct hci_info_t *)phl_info->hci;
	struct rtw_rx_buf_ring *rx_buf_ring = (struct rtw_rx_buf_ring *)hci_info->rxbuf_pool;

	PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_, "[5] %s:: ==>\n", __func__);

	/* [1] Check driver/nic state*/

	/* [2] Check in token num*/
	pstatus = _phl_rx_start_usb(phl_info);

	// set timer again
	if(pstatus == RTW_PHL_STATUS_RESOURCE){
		_phl_indic_new_rxpkt(phl_info);
		_os_set_timer(drv, &rx_buf_ring->deferred_timer, 10);
	}
}

/* [SD7_Ref] : HalUsbAllocInResource */
/* [SD4_Ref] : _rtw_init_recv_priv	*/
/* [phl_ref] :  _phl_alloc_rxbuf_pool_pcie */
static enum rtw_phl_status
_phl_rx_init_usb(struct phl_info_t *phl_info, u8 pipe_cnt, u32 num_rxbuf)
{
	void *drv = phl_to_drvpriv(phl_info);
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	struct rtw_rx_buf_ring *rx_buf_ring = NULL;
	struct rtw_rx_buf *rx_buf = NULL;
	struct rtw_hal_com_t *hal_com = rtw_hal_get_halcom(phl_info->hal);
	struct bus_cap_t *bus_cap = &hal_com->bus_cap;

	u16 i = 0;
	FUNCIN_WSTS(pstatus);

	rx_buf_ring = _os_mem_alloc(drv, sizeof(struct rtw_rx_buf_ring));
	if (NULL != rx_buf_ring) {

		/* Initialize list for available rtw_rx_buf objects.	*/
		_os_spinlock_init(drv, &rx_buf_ring->idle_rxbuf_list.lock);
		_os_spinlock_init(drv, &rx_buf_ring->busy_rxbuf_list.lock);
		_os_spinlock_init(drv, &rx_buf_ring->pend_rxbuf_list.lock);
		INIT_LIST_HEAD(&rx_buf_ring->idle_rxbuf_list.queue);
		INIT_LIST_HEAD(&rx_buf_ring->busy_rxbuf_list.queue);
		INIT_LIST_HEAD(&rx_buf_ring->pend_rxbuf_list.queue);
		rx_buf_ring->idle_rxbuf_list.cnt = 0;
		rx_buf_ring->busy_rxbuf_list.cnt = 0;
		rx_buf_ring->pend_rxbuf_list.cnt = 0;

		rx_buf_ring->block_cnt_alloc = pipe_cnt * num_rxbuf;

		/* Allocate memory for rtw_rx_buf objects.	*/
		rx_buf_ring->total_blocks_size = sizeof(struct rtw_rx_buf) * rx_buf_ring->block_cnt_alloc;

		rx_buf_ring->rxbufblock= _os_mem_alloc(drv, rx_buf_ring->total_blocks_size);
		if (NULL != rx_buf_ring->rxbufblock) {
			/* Initialize all rtw_rx_buf objects allocated and	*/
			/* put them into the list for further use.			*/
			rx_buf = (struct rtw_rx_buf*)rx_buf_ring->rxbufblock;
			for (i = 0; i < rx_buf_ring->block_cnt_alloc; i++) {
				/* MAX_RECEIVE_BUFFER_SIZE = 512(RX_AGG_BLOCK_SIZE) * 60(MAX_RX_AGG_BLKCNT)	*/
				rx_buf->buf_len = bus_cap->rx_buf_size;
				rx_buf->buffer = _os_kmem_alloc(drv, rx_buf->buf_len);
				if (NULL == rx_buf->buffer) {
					pstatus = RTW_PHL_STATUS_FAILURE;
					break;
				}

				INIT_LIST_HEAD(&rx_buf->list);
				_os_spinlock_init(drv, &rx_buf->lock);
				rx_buf->pktcnt = 0;

				pq_push(drv, &rx_buf_ring->idle_rxbuf_list, &rx_buf->list, _tail, _irq);

				rx_buf++;
			}

			_os_init_timer(drv, &rx_buf_ring->deferred_timer,
					_phl_rx_deferred_in_token, phl_info, "phl_rx_deferred_timer");

		pstatus = RTW_PHL_STATUS_SUCCESS;
		}
	}


	if (RTW_PHL_STATUS_SUCCESS == pstatus)
		phl_info->hci->rxbuf_pool = (u8 *)rx_buf_ring;

	FUNCOUT_WSTS(pstatus);

	return pstatus;
}

static void _phl_rx_deinit_usb(struct phl_info_t *phl_info)
{
	void *drv = phl_to_drvpriv(phl_info);
	struct hci_info_t *hci_info = (struct hci_info_t *)phl_info->hci;
	struct rtw_rx_buf_ring *rx_buf_ring = (struct rtw_rx_buf_ring *)hci_info->rxbuf_pool;
	struct rtw_rx_buf *rx_buf = NULL;
	u32 total_cnt = 0;
	//_os_list* obj = NULL;
	u16 i = 0;

	_os_cancel_timer(drv, &rx_buf_ring->deferred_timer);
	_os_release_timer(drv, &rx_buf_ring->deferred_timer);

	total_cnt = rx_buf_ring->idle_rxbuf_list.cnt +
				rx_buf_ring->busy_rxbuf_list.cnt +
				rx_buf_ring->pend_rxbuf_list.cnt;

	if(total_cnt != rx_buf_ring->block_cnt_alloc)
		PHL_ERR("%s:: alloc(%d)!=idle(%d)+ busy(%d)+ pend(%d)\n", __FUNCTION__,
				rx_buf_ring->block_cnt_alloc, rx_buf_ring->idle_rxbuf_list.cnt,
				rx_buf_ring->busy_rxbuf_list.cnt, rx_buf_ring->pend_rxbuf_list.cnt);

#if 0
	// rtw_rx_buf may queue in phl_rx or recv_pkt , check it later.
	while(rx_buf_ring->idle_rxbuf_list.cnt)
	{
		obj = NULL;
		if(pq_pop(drv, &rx_buf_ring->idle_rxbuf_list, &obj, _first, _irq)) {
			rx_buf = (struct rtw_rx_buf*)obj;
			_os_spinlock_free(drv, &rx_buf->lock);
			_os_kmem_free(drv, rx_buf->buffer, rx_buf->buf_len);
		}
	}

	while(rx_buf_ring->pend_rxbuf_list.cnt)
	{
		obj = NULL;
		if(pq_pop(drv, &rx_buf_ring->pend_rxbuf_list, &obj, _first, _irq)) {
			rx_buf = (struct rtw_rx_buf*)obj;
			_os_spinlock_free(drv, &rx_buf->lock);
			_os_kmem_free(drv, rx_buf->buffer, rx_buf->buf_len);
		}
	}

	while(rx_buf_ring->busy_rxbuf_list.cnt)
	{
		obj = NULL;
		if(pq_pop(drv, &rx_buf_ring->busy_rxbuf_list, &obj, _first, _irq)) {
			rx_buf = (struct rtw_rx_buf*)obj;
			_os_spinlock_free(drv, &rx_buf->lock);
			_os_kmem_free(drv, rx_buf->buffer, rx_buf->buf_len);
		}
	}

#else
	// DeInitialize all rtw_rx_buf objects.
	//
	rx_buf = (struct rtw_rx_buf *)rx_buf_ring->rxbufblock;
	for (i = 0; i < rx_buf_ring->block_cnt_alloc; i++)
	{
		//
		// Free of this rtw_usb_rx_buf->buffer object.
		//
		_os_spinlock_free(drv, &rx_buf->lock);
		_os_kmem_free(drv, rx_buf->buffer, rx_buf->buf_len);
		rx_buf++;
	}
#endif

	/* Free memory block allocated for phl_usb_buf objects.	*/
	_os_mem_free(drv, rx_buf_ring->rxbufblock, rx_buf_ring->total_blocks_size);

	_os_mem_free(drv, rx_buf_ring, sizeof(struct rtw_rx_buf_ring));
}

void phl_rx_handle_normal(struct phl_info_t *phl_info,
						 struct rtw_phl_rx_pkt *phl_rx)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	_os_list frames;

	INIT_LIST_HEAD(&frames);

	if (phl_rx->r.mdata.rx_rate <= RTW_DATA_RATE_HE_NSS4_MCS11)
		phl_info->phl_com->phl_stats.rx_rate_nmr[phl_rx->r.mdata.rx_rate]++;

	pstatus = phl_rx_reorder(phl_info, phl_rx, &frames);
	if (pstatus == RTW_PHL_STATUS_SUCCESS)
		phl_handle_rx_frame_list(phl_info, &frames);
	else
		PHL_TRACE(COMP_PHL_RECV, _PHL_WARNING_, "[WARNING]handle normal rx error (0x%08X)!\n", pstatus);
}

enum rtw_phl_status phl_tx_release_buf_usb(void *phl, u8  *tx_buf_ptr)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_usb_tx_buf_resource *tx_buf_res = NULL;
	struct phl_usb_buf *release_tx_buf = (struct phl_usb_buf *)tx_buf_ptr;

	tx_buf_res =
		(struct phl_usb_tx_buf_resource *)phl_info->hci->txbuf_pool;

	if (release_tx_buf->type == RTW_PHL_PKT_TYPE_H2C) {
		phl_enqueue_idle_h2c_pkt(phl_info,
			(struct rtw_h2c_pkt *)release_tx_buf->buffer);
		release_tx_buf->buffer = NULL;
		pstatus = enqueue_usb_buf(phl_info,
			&tx_buf_res->h2c_txbuf_list, release_tx_buf, _tail);
	} else if (release_tx_buf->type == RTW_PHL_PKT_TYPE_MGNT)
		pstatus = enqueue_usb_buf(phl_info,
			&tx_buf_res->mgmt_txbuf_list, release_tx_buf, _tail);
	else
		pstatus = enqueue_usb_buf(phl_info,
			&tx_buf_res->idle_txbuf_list, release_tx_buf, _tail);

	return pstatus;
}

enum rtw_phl_status
phl_prepare_tx_usb(struct phl_info_t *phl_info, struct rtw_xmit_req *tx_req)
{
	return true;
}


enum rtw_phl_status
_phl_prepare_tx_usb(struct phl_info_t *phl_info, struct rtw_xmit_req *tx_req,
	u8 *tx_buf_data, u32 *packet_len)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_FAILURE;
	enum rtw_phl_status precycle = RTW_PHL_STATUS_FAILURE;
/*	struct hci_info_t *hci_info = (struct hci_info_t *)phl_info->hci; */
	struct rtw_pkt_buf_list *pkt_buf = NULL;
/*	struct rtw_phl_pkt_req phl_pkt_req; */
	struct rtw_phl_evt_ops *ops = &phl_info->phl_com->evt_ops;
	u32 wd_len = 0;
	u8 dummy = 0;
	u8 i = 0;

	do {
		if (NULL == tx_req)
			break;

		/*tx_req->mdata.usb_pkt_ofst = 1;*/

		hstatus = rtw_hal_fill_wd(phl_info->hal, tx_req, tx_buf_data, &wd_len);
		if (hstatus != RTW_HAL_STATUS_SUCCESS) {
			PHL_ERR("%s, fill_wd fail hstatus = %d\n", __func__, hstatus);
			break;
		} else {
			/*PHL_INFO("%s, wd_len = %d\n", __func__, wd_len);*/
			if(tx_req->mdata.usb_pkt_ofst == 1)
				dummy = PKT_OFFSET_DUMMY;
			tx_buf_data = tx_buf_data + wd_len + dummy;
			*packet_len = *packet_len + wd_len + dummy;
			/*PHL_INFO("%s, packet_len = %d\n", __func__, *packet_len);*/
		}

		/*tx_buf_data = tx_buf_data + 8;*/
		/**packet_len = *packet_len + 8;*/

		pkt_buf = (struct rtw_pkt_buf_list *)tx_req->pkt_list;
		for (i = 0; i < tx_req->pkt_cnt; i++) {
			/*PHL_INFO("%s, pkt[%d] 0x%p length = %d\n", __func__, i,
				(void *)pkt_buf->vir_addr, pkt_buf->length);*/
			if (pkt_buf->vir_addr) {
				_os_mem_cpy(phl_to_drvpriv(phl_info),
					tx_buf_data, pkt_buf->vir_addr, pkt_buf->length);
				tx_buf_data = tx_buf_data + pkt_buf->length;
				*packet_len = *packet_len + pkt_buf->length;
				/*PHL_INFO("%s, packet_len = %d\n", __func__, *packet_len);*/
			}
			pkt_buf++;
		}
		pstatus = RTW_PHL_STATUS_SUCCESS;

	} while (false);

	if (RTW_PHL_TREQ_TYPE_TEST_PATTERN == tx_req->treq_type) {
		if (NULL != ops->tx_test_recycle) {
			precycle = ops->tx_test_recycle(phl_info, tx_req);
			if (RTW_PHL_STATUS_SUCCESS != precycle)
				PHL_WARN("tx_test_recycle fail\n");
		}
	} else if (RTW_PHL_TREQ_TYPE_NORMAL == tx_req->treq_type) {
		if (NULL != ops->tx_recycle) {
			precycle = ops->tx_recycle(phl_to_drvpriv(phl_info),
									tx_req);
			if (RTW_PHL_STATUS_SUCCESS != precycle)
				PHL_WARN("tx recycle fail\n");
		}
	}

	return pstatus;
}

static enum rtw_phl_status phl_tx_usb(struct phl_info_t *phl_info)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;

	return pstatus;
}


void phl_trx_deinit_usb(struct phl_info_t *phl_info)
{
	FUNCIN();

	os_disable_usb_out_pipes(phl_to_drvpriv(phl_info));
	os_disable_usb_in_pipes(phl_to_drvpriv(phl_info));

	os_out_token_free(phl_to_drvpriv(phl_info));
	_phl_free_txbuf_pool_usb(phl_info, phl_info->hci->txbuf_pool);

	os_in_token_free(phl_to_drvpriv(phl_info));
	_phl_rx_deinit_usb(phl_info);

	FUNCOUT();
}

void phl_req_tx_stop_usb(struct phl_info_t *phl_info)
{
	void *drv = phl_to_drvpriv(phl_info);

	_os_atomic_set(drv, &phl_info->phl_sw_tx_sts,
		PHL_TX_STATUS_STOP_INPROGRESS);
}

void phl_tx_stop_usb(struct phl_info_t *phl_info)
{
	void *drv = phl_to_drvpriv(phl_info);

	_os_atomic_set(drv, &phl_info->phl_sw_tx_sts, PHL_TX_STATUS_SW_PAUSE);
}

void phl_req_rx_stop_usb(struct phl_info_t *phl_info)
{
	void *drv = phl_to_drvpriv(phl_info);

	_os_atomic_set(drv, &phl_info->phl_sw_rx_sts,
		PHL_RX_STATUS_STOP_INPROGRESS);
}

void phl_rx_stop_usb(struct phl_info_t *phl_info)
{
	void *drv = phl_to_drvpriv(phl_info);

	_os_atomic_set(drv, &phl_info->phl_sw_rx_sts, PHL_RX_STATUS_SW_PAUSE);
}

bool phl_is_tx_sw_pause_usb(struct phl_info_t *phl_info)
{
	void *drvpriv = phl_to_drvpriv(phl_info);

	if (PHL_TX_STATUS_SW_PAUSE == _os_atomic_read(drvpriv,
		&phl_info->phl_sw_tx_sts))
		return true;
	else
		return false;
}

bool phl_is_rx_sw_pause_usb(struct phl_info_t *phl_info)
{
	void *drvpriv = phl_to_drvpriv(phl_info);

	if (PHL_RX_STATUS_SW_PAUSE == _os_atomic_read(drvpriv,
		&phl_info->phl_sw_rx_sts))
		return true;
	else
		return false;
}

void phl_tx_resume_usb(struct phl_info_t *phl_info)
{
	void *drv = phl_to_drvpriv(phl_info);
	_os_atomic_set(drv, &phl_info->phl_sw_tx_sts, PHL_TX_STATUS_RUNNING);
}

void phl_rx_resume_usb(struct phl_info_t *phl_info)
{
	void *drv = phl_to_drvpriv(phl_info);
	_os_atomic_set(drv, &phl_info->phl_sw_rx_sts, PHL_RX_STATUS_RUNNING);
}

void phl_trx_resume_usb(struct phl_info_t *phl_info, u8 type)
{
	if (PHL_CTRL_TX & type)
		phl_tx_resume_usb(phl_info);

	if (PHL_CTRL_RX & type)
		phl_rx_resume_usb(phl_info);
}

void phl_reset_tx_usb(struct phl_info_t *phl_info)
{

}

void phl_reset_rx_usb(struct phl_info_t *phl_info)
{

}

void phl_trx_reset_usb(struct phl_info_t *phl_info, u8 type)
{
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct rtw_stats *phl_stats = &phl_com->phl_stats;

	if (PHL_CTRL_TX & type) {
		phl_reset_tx_usb(phl_info);
		phl_reset_tx_stats(phl_stats);
	}

	if (PHL_CTRL_RX & type) {
		phl_reset_rx_usb(phl_info);
		phl_reset_rx_stats(phl_stats);
	}

	if (PHL_CTRL_IN_PIPE & type) {
		os_enable_usb_in_pipes(phl_to_drvpriv(phl_info));
	}

	if (PHL_CTRL_OUT_PIPE & type) {
		os_enable_usb_out_pipes(phl_to_drvpriv(phl_info));
	}
}

static void phl_tx_reset_hwband_usb(struct phl_info_t *phl_info, enum phl_band_idx band_idx)
{
}

static void _phl_tx_flow_ctrl_usb(struct phl_info_t *phl_info,
				  struct phl_ring_status *target_ring,
				  struct phl_usb_buf **tx_buf)
{
	struct phl_usb_tx_buf_resource *reso = NULL;

	reso = (struct phl_usb_tx_buf_resource *)phl_info->hci->txbuf_pool;
	/*PHL_INFO("%s, idle_txbuf_cnt = %d\n", __func__, reso->idle_txbuf_cnt);*/

	if (reso) {
		if (target_ring) {
			if (target_ring->ring_ptr->cat == RTW_PHL_RING_CAT_MGNT)
				*tx_buf = dequeue_usb_buf(phl_info, &reso->mgmt_txbuf_list);
			else
				*tx_buf = dequeue_usb_buf(phl_info, &reso->idle_txbuf_list);
		}
	} else {
		PHL_ERR("%s, phl_usb_tx_buf_resource == NULL\n", __func__);
	}

	if (*tx_buf == NULL && target_ring != NULL) {
		if (target_ring->ring_ptr->cat == RTW_PHL_RING_CAT_MGNT)
			PHL_WARN("%s, mgnt tx_buf NULL free ring sts\n", __func__);
		else
			PHL_DBG("%s, idle tx_buf NULL free ring sts\n", __func__);
		phl_release_ring_sts(phl_info, target_ring);
		target_ring = NULL;
	}
}

static void _usb_tx_agg_preprocess(struct phl_info_t *phl_info,
				   u8 max_bulkout_wd_num,
				   u16 max_dma_txagg_msk,
				   struct phl_ring_status *ring_sts,
				   u32 *last_idx, u8 *add_dummy)
{
	struct rtw_phl_tx_ring *tring = ring_sts->ring_ptr;
	struct rtw_xmit_req *tx_req = NULL;
	struct rtw_hal_com_t *hal_com = rtw_hal_get_halcom(phl_info->hal);
	struct bus_cap_t *bus_cap = &hal_com->bus_cap;
	u32 total = ring_sts->req_busy;
	u16 cnt = 0;
	u16 rptr = 0;
	void *drv_priv = phl_to_drvpriv(phl_info);
	u32 bulk_size = phl_info->hci->usb_bulkout_size;
	u32 tx_len = 0;
	u32 wd_len = MAX_WD_LEN;
	u32 packet_len = 0;
	u32 packet_align_len = 0;
	u8 trasc_agg_cnt = 0;
	u8 trasc_idx = 0;
	u8 shift_offset = GET_SHIFT_OFFSET(bulk_size);
	u16 idx = 0;

	*last_idx = IDX_NONE;
	*add_dummy = false;

	rptr = (u16)_os_atomic_read(drv_priv, &tring->phl_next_idx);
	while(cnt < total) {
		idx = (rptr + cnt) % MAX_PHL_TX_RING_ENTRY_NUM;
		tx_req = (struct rtw_xmit_req *)tring->entry[idx];
		if (NULL == tx_req) {
			*last_idx = (cnt) ? (cnt - 1) : (IDX_NONE);
			break;
		}
		wd_len = rtw_hal_get_wd_len(phl_info->hal, tx_req);
		if (( _ALIGN(wd_len + tx_req->total_len,
		    8) + tx_len) > bus_cap->tx_buf_size - PKT_OFFSET_DUMMY) {
			*last_idx = (cnt) ? (cnt - 1) : (IDX_NONE);
			break;
		}
		/* packet_len represens the length of PREVIOUS copied tx req
		 * not the current one.
		*/
		packet_align_len = _ALIGN(packet_len, 8);
		packet_align_len -= packet_len;
#ifdef RTW_WKARD_USB_TXAGG_BULK_END_WD
		if(! ((tx_len + wd_len + packet_align_len) % bulk_size)) {
			*last_idx = (cnt) ? (cnt - 1) : (IDX_NONE);
			break;
		}
#endif
		/* if current tx req is accepted for aggregation
		 * 8 byte alignment is required before copying current data.
		*/
		tx_len += packet_align_len;

		/* update packet_len for next alignment reference */
		packet_len = wd_len + tx_req->total_len;
		tx_len += packet_len;
		/* chip only allow a limited number of WD
		* placed inside per bulk transaction.
		* */
		trasc_agg_cnt++;
		if(trasc_idx != tx_len >> shift_offset) {
			trasc_idx = (u8)(tx_len >> shift_offset);
			trasc_agg_cnt = 0;
		}
		if (trasc_agg_cnt >= max_bulkout_wd_num ||
		    (cnt + 1) == max_dma_txagg_msk) {
			*last_idx = cnt;
			break;
		}
		cnt++;
		if( tx_req->mdata.type != RTW_PHL_PKT_TYPE_DATA)
			break;
#ifndef CONFIG_PHL_USB_TX_AGGREGATION
		break;
#endif
	}
	if(*last_idx == IDX_NONE && tx_len && cnt <= total)
		*last_idx = cnt - 1;
	if( tx_len && ! (tx_len % bulk_size) )
		*add_dummy = true;
}
static void _phl_tx_callback_usb(void *context);
static enum rtw_phl_status _phl_handle_xmit_ring_usb(struct phl_info_t *phl_info,
					struct phl_ring_status *ring_sts, struct phl_usb_buf *tx_buf)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
/*	struct phl_hci_trx_ops *hci_trx_ops = phl_info->hci_trx_ops; */
	struct rtw_phl_tx_ring *tring = ring_sts->ring_ptr;
	struct rtw_xmit_req *tx_req = NULL;
	struct phl_usb_tx_buf_resource *tx_buf_res = NULL;
	struct rtw_hal_com_t *hal_com = rtw_hal_get_halcom(phl_info->hal);
	struct bus_cap_t *bus_cap = &hal_com->bus_cap;
	u16 rptr = 0, next_idx = 0;
	u32 packet_len = 0;
	u32 packet_align_len = 0;
	u32 tx_len = 0;
	u8 *tx_buf_data = NULL;
	u8 bulk_id = 0;
	u8 agg_cnt = 0;
	u8 dma_ch = tring->dma_ch;
	enum rtw_packet_type type = 0;
	u8 is_dummy = false;
	u8 trasc_agg_cnt = 0;
	u32 wd_len = 0;
	u32 bulk_size = phl_info->hci->usb_bulkout_size;
	u32 last_idx = IDX_NONE;
#ifdef CONFIG_PHL_USB_TX_AGGREGATION
	u8 agg_en = 1;
	u8 trasc_idx = 0;
	u8 shift_offset = GET_SHIFT_OFFSET(bulk_size);
	u8 max_bulkout_wd_num = rtw_hal_get_max_bulkout_wd_num(phl_info->hal);
	u16 max_dma_txagg_msk = rtw_hal_get_max_dma_txagg_msk(phl_info->hal);
#endif
	void *drv_priv = phl_to_drvpriv(phl_info);
#ifdef CONFIG_PHL_USB_TX_PADDING_CHK
	_usb_tx_agg_preprocess(phl_info, max_bulkout_wd_num, max_dma_txagg_msk,
			       ring_sts, &last_idx, &is_dummy);
#endif
	tx_buf_data = tx_buf->buffer;
	while (0 != ring_sts->req_busy) {
		rptr = (u16)_os_atomic_read(drv_priv, &tring->phl_next_idx);

		tx_req = (struct rtw_xmit_req *)tring->entry[rptr];
		if (NULL == tx_req)  {
			PHL_ERR("tx_req is NULL!\n");
			break;
		}

		tx_req->mdata.band = ring_sts->band;
		tx_req->mdata.wmm = ring_sts->wmm;
		tx_req->mdata.hal_port = ring_sts->port;
		/*tx_req->mdata.mbssid = ring_sts->mbssid;*/
		tx_req->mdata.dma_ch = tring->dma_ch;
		tx_req->mdata.pktlen = (u16)tx_req->total_len;
#ifdef RTW_WKARD_WPOFFSET
		tx_req->mdata.wp_offset = 2;
#endif
		type = tx_req->mdata.type;
		wd_len = rtw_hal_get_wd_len(phl_info->hal, tx_req);
		if(last_idx != IDX_NONE && agg_cnt == (u8)last_idx && is_dummy) {
			tx_req->mdata.usb_pkt_ofst = 1;
			wd_len += PKT_OFFSET_DUMMY;
		}
#ifdef CONFIG_PHL_USB_TX_PADDING_CHK
		if (_ALIGN(wd_len + tx_req->total_len,
		    8) + tx_len > (bus_cap->tx_buf_size - PKT_OFFSET_DUMMY))
			break;
#else
		if (_ALIGN(wd_len + tx_req->total_len,
		    8) + tx_len > bus_cap->tx_buf_size)
			break;
#endif

#ifdef CONFIG_PHL_USB_TX_AGGREGATION
		if (agg_en && type == RTW_PHL_PKT_TYPE_DATA) {
			packet_align_len = _ALIGN(packet_len, 8);
			tx_buf_data = tx_buf_data + packet_align_len;

#ifdef RTW_WKARD_USB_TXAGG_BULK_END_WD
			if (tx_len !=0) {
				int check_len = 0;
				check_len = tx_len + wd_len + packet_align_len - packet_len;
				/*PHL_INFO("%s, tx_len(%d),wd_len(%d),packet_align_len(%d),packet_len(%d)\n",
					__func__, tx_len, wd_len ,packet_align_len ,packet_len);*/
				if(! (check_len % bulk_size)) {
					PHL_TRACE(COMP_PHL_XMIT, _PHL_DEBUG_, "%s:AGG break!\n", __func__);
					break;
				}
			}
#endif

			if (packet_align_len > packet_len)
				tx_len = tx_len + (packet_align_len - packet_len);
		}
#endif

		packet_len = 0;
		pstatus = _phl_prepare_tx_usb(phl_info, tx_req,
			tx_buf_data, &packet_len);
		/*PHL_INFO("%s, packet_len = %d\n", __func__, packet_len);*/

		tx_len = tx_len + packet_len;
		/*PHL_INFO("%s, tx_len = %d\n", __func__, tx_len);*/

		if (RTW_PHL_STATUS_SUCCESS == pstatus) {
			ring_sts->req_busy--;

			/* hana_todo, workaround here to update phl_index */
			_os_atomic_set(drv_priv, &tring->phl_idx, rptr);

			agg_cnt++;
			trasc_agg_cnt++;

			if (type != RTW_PHL_PKT_TYPE_DATA)
				break;

#ifdef CONFIG_PHL_USB_TX_AGGREGATION
			/* chip only allow a limited number of WD
			 * placed inside per bulk transaction.
			 * */
			if(trasc_idx != tx_len >> shift_offset) {
				trasc_idx = (u8)(tx_len >> shift_offset);
				trasc_agg_cnt = 0;
			}
			if (trasc_agg_cnt >= max_bulkout_wd_num) {
				/*PHL_PRINT("%s, hit wd limit(%d), break agg\n",
				__func__, max_bulkout_wd_num);*/
				break;
			}
			if (agg_cnt == max_dma_txagg_msk) {
				/* PHL_PRINT("%s, hit dma txagg limit(%d), break agg\n",
					  __func__, max_dma_txagg_msk); */
				break;
			}
#else
			if (agg_cnt == 1)
				break;
#endif

			if (0 != ring_sts->req_busy) {
				next_idx = rptr + 1;

				if (next_idx >= MAX_PHL_TX_RING_ENTRY_NUM) {
					_os_atomic_set(drv_priv,
								&tring->phl_next_idx, 0);

				} else {
					_os_atomic_inc(drv_priv,
								&tring->phl_next_idx);
				}
			}

		} else {
			_os_atomic_set(drv_priv, &tring->phl_idx, rptr);
			PHL_INFO("HCI prepare tx fail\n");
			break;
		}
	}

	if (pstatus == RTW_PHL_STATUS_SUCCESS) {
		tx_buf_data = tx_buf->buffer;
#ifdef CONFIG_PHL_USB_TX_PADDING_CHK
		if( ! (tx_len % bulk_size) )
			PHL_ERR("%s: no padding dummy\n", __func__);
#endif
#ifdef CONFIG_PHL_USB_TX_AGGREGATION
		/*PHL_INFO("%s, agg_cnt = %d\n", __func__, agg_cnt);*/
		if (agg_en && type == RTW_PHL_PKT_TYPE_DATA && agg_cnt > 1)
			rtw_hal_usb_tx_agg_cfg(phl_info->hal, tx_buf_data, agg_cnt);
#endif
		bulk_id = rtw_hal_get_bulkout_id(phl_info->hal,
			dma_ch, 0);
		pstatus = os_usb_tx(phl_to_drvpriv(phl_info),
			(u8 *)tx_buf, bulk_id, tx_len, tx_buf_data);
	}

	if (pstatus != RTW_PHL_STATUS_SUCCESS) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_, "[WARNING] phl_tx fail!\n");
		if (tx_buf) {
			tx_buf_res =
				(struct phl_usb_tx_buf_resource *)phl_info->hci->txbuf_pool;

			if (tring->cat == RTW_PHL_RING_CAT_MGNT)
				pstatus = enqueue_usb_buf(phl_info,
					&tx_buf_res->mgmt_txbuf_list, tx_buf, _tail);
			else
				pstatus = enqueue_usb_buf(phl_info,
					&tx_buf_res->idle_txbuf_list, tx_buf, _tail);
		}
	}

	phl_release_ring_sts(phl_info, ring_sts);

	return pstatus;
}

static void _phl_tx_callback_usb(void *context)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	struct rtw_phl_handler *phl_handler
		= (struct rtw_phl_handler *)phl_container_of(context,
									struct rtw_phl_handler,
									os_handler);
	struct phl_info_t *phl_info = (struct phl_info_t *)phl_handler->context;
/*	struct phl_hci_trx_ops *hci_trx_ops = phl_info->hci_trx_ops; */
	struct phl_ring_status *ring_sts = NULL, *t;
	struct phl_usb_buf *tx_buf = NULL;
	void *drvpriv = phl_to_drvpriv(phl_info);
	_os_list sta_list;
	bool tx_pause = false;
#ifdef CONFIG_POWER_SAVE
	_os_atomic *trx_more = &phl_info->phl_sw_tx_more;
#endif

	INIT_LIST_HEAD(&sta_list);

	/* check datapath sw state */
	tx_pause = phl_datapath_chk_trx_pause(phl_info, PHL_CTRL_TX);
	if (true == tx_pause)
		goto end;

#ifdef CONFIG_POWER_SAVE
	/* check ps state when tx is not paused */
	if ((true == _os_atomic_read(drvpriv, trx_more)) && (false == phl_ps_is_datapath_allowed(phl_info))) {
		PHL_WARN("%s(): datapath is not allowed now... may in low power.\n", __func__);
		goto chk_stop;
	}
#endif

	if (true == phl_check_xmit_ring_resource(phl_info, &sta_list)) {
		phl_tx_flow_ctrl(phl_info, &sta_list);

		phl_list_for_loop_safe(ring_sts, t, struct phl_ring_status,
					&phl_info->t_fctrl_result, list) {

			list_del(&ring_sts->list);
			_phl_tx_flow_ctrl_usb(phl_info, ring_sts, &tx_buf);

			if (ring_sts && tx_buf)
				pstatus =  _phl_handle_xmit_ring_usb(phl_info,
								     ring_sts,
								     tx_buf);
			else if (!ring_sts)
				PHL_DBG("%s, ring_sts NULL\n", __func__);
			else if (!tx_buf)
				PHL_DBG("%s, tx_buf NULL\n", __func__);
		}

		/*pstatus = hci_trx_ops->tx(phl_info);*/
	}
#ifdef CONFIG_POWER_SAVE
chk_stop:
#endif
	/* for the pause PHL Tx handshake with USB SER */
	if (PHL_TX_STATUS_STOP_INPROGRESS ==
	    _os_atomic_read(drvpriv, &phl_info->phl_sw_tx_sts))
		phl_tx_stop_usb(phl_info);

end:
	phl_free_deferred_tx_ring(phl_info);
}

static void _phl_rx_callback_usb(void *context)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	struct rtw_phl_handler *phl_handler
		= (struct rtw_phl_handler *)phl_container_of(context,
									struct rtw_phl_handler,
									os_handler);
	struct phl_info_t *phl_info = (struct phl_info_t *)phl_handler->context;
	struct phl_hci_trx_ops *hci_trx_ops = phl_info->hci_trx_ops;
	bool rx_pause = false;

	/* check datapath sw state */
	rx_pause = phl_datapath_chk_trx_pause(phl_info, PHL_CTRL_RX);
	if (true == rx_pause)
		goto end;

	do {
		if (false == phl_check_recv_ring_resource(phl_info))
			break;

		pstatus = hci_trx_ops->rx(phl_info);

		if (RTW_PHL_STATUS_FAILURE == pstatus) {
			PHL_TRACE(COMP_PHL_RECV, _PHL_WARNING_, "[WARNING] hci_trx_ops->rx fail!\n");
		}
	} while (false);

	if (PHL_RX_STATUS_STOP_INPROGRESS ==
	    _os_atomic_read(phl_to_drvpriv(phl_info), &phl_info->phl_sw_rx_sts))
		phl_rx_stop_usb(phl_info);

end:
	return;
}

enum rtw_phl_status phl_trx_init_usb(struct phl_info_t *phl_info)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	struct hal_spec_t *hal_spec = phl_get_ic_spec(phl_info->phl_com);
	struct rtw_phl_handler *tx_handler = &phl_info->phl_tx_handler;
	struct rtw_phl_handler *rx_handler = &phl_info->phl_rx_handler;
	struct rtw_hal_com_t *hal_com = rtw_hal_get_halcom(phl_info->hal);
	struct bus_cap_t *bus_cap = &hal_com->bus_cap;
	void *drv_priv = phl_to_drvpriv(phl_info);

	FUNCIN_WSTS(pstatus);
	PHL_INFO("%s, bus_cap->tx_buf_num(%d)\n", __func__, bus_cap->tx_buf_num);
	PHL_INFO("%s, bus_cap->tx_buf_size(%d)\n", __func__, bus_cap->tx_buf_size);
	PHL_INFO("%s, bus_cap->tx_mgnt_buf_num(%d)\n", __func__, bus_cap->tx_mgnt_buf_num);
	PHL_INFO("%s, bus_cap->tx_mgnt_buf_size(%d)\n", __func__, bus_cap->tx_mgnt_buf_size);
	PHL_INFO("%s, bus_cap->tx_h2c_buf_num(%d)\n", __func__, bus_cap->tx_h2c_buf_num);
	PHL_INFO("%s, bus_cap->rx_buf_num(%d)\n", __func__, bus_cap->rx_buf_num);
	PHL_INFO("%s, bus_cap->rx_buf_size(%d)\n", __func__, bus_cap->rx_buf_size);
	PHL_INFO("%s, bus_cap->in_token_num(%d)\n", __func__, bus_cap->in_token_num);
	do {
		tx_handler->type = RTW_PHL_HANDLER_PRIO_HIGH; /* tasklet */
		tx_handler->callback = _phl_tx_callback_usb;
		tx_handler->context = phl_info;
		tx_handler->drv_priv = drv_priv;
		pstatus = phl_register_handler(phl_info->phl_com, tx_handler);
		if (RTW_PHL_STATUS_SUCCESS != pstatus)
			break;

		rx_handler->type = RTW_PHL_HANDLER_PRIO_HIGH;
		rx_handler->callback = _phl_rx_callback_usb;
		rx_handler->context = phl_info;
		rx_handler->drv_priv = drv_priv;
		pstatus = phl_register_handler(phl_info->phl_com, rx_handler);
		if (RTW_PHL_STATUS_SUCCESS != pstatus)
			break;

		/* usb tx sw resource */
		/*YiWei_to need use correct txbuf num and txbuf size*/
		pstatus = _phl_alloc_txbuf_pool_usb(phl_info);
		if (RTW_PHL_STATUS_SUCCESS != pstatus)
			break;

		/* allocate rx packet buffer block */
		pstatus = _phl_rx_init_usb(phl_info, hal_spec->max_bulkin_num, bus_cap->rx_buf_num);
		if (RTW_PHL_STATUS_SUCCESS != pstatus)
			break;

		/* allocate platform in token */
		/* OS maintain in token (number/init/free)*/
		pstatus = os_in_token_alloc(phl_to_drvpriv(phl_info));
		if (RTW_PHL_STATUS_SUCCESS != pstatus)
			break;

		pstatus = os_out_token_alloc(phl_to_drvpriv(phl_info));
		if (RTW_PHL_STATUS_SUCCESS != pstatus)
			break;

		os_enable_usb_out_pipes(phl_to_drvpriv(phl_info));
		os_enable_usb_in_pipes(phl_to_drvpriv(phl_info));

	} while (false);

	if (RTW_PHL_STATUS_SUCCESS != pstatus)
		phl_trx_deinit_usb(phl_info);

	FUNCOUT_WSTS(pstatus);
	return pstatus;
}

#ifdef CONFIG_PHL_USB_RX_AGGREGATION
static enum rtw_phl_status
_rtw_phl_cmd_usb_rx_agg_cfg(struct phl_info_t *phl_info,
			    enum phl_usb_rx_agg_mode mode,
			    u8 agg_mode,
			    u8 drv_define,
			    u8 timeout,
			    u8 size,
			    u8 pkt_num)
{
	enum rtw_phl_status psts = RTW_PHL_STATUS_SUCCESS;
	enum rtw_hal_status hsts;

	hsts = rtw_hal_usb_rx_agg_cfg(phl_info->hal, mode, agg_mode, drv_define,
				      timeout, size, pkt_num);
	if (hsts != RTW_HAL_STATUS_SUCCESS)
		psts = RTW_PHL_STATUS_FAILURE;

	return psts;
}

/* agg_mode refs to enum mac_ax_rx_agg_mode */
struct cmd_usb_rx_agg_param {
	enum phl_usb_rx_agg_mode mode;
	u8 agg_mode;
	u8 drv_define;
	u8 timeout;
	u8 size;
	u8 pkt_num;
};

static void _phl_cmd_usb_rx_agg_cfg_done(void *drv_priv,
					 u8 *cmd,
					 u32 cmd_len,
					 enum rtw_phl_status status)
{
	if (cmd)
		_os_kmem_free(drv_priv, cmd, cmd_len);
}

enum rtw_phl_status
phl_cmd_usb_rx_agg_cfg_hdl(struct phl_info_t *phl_info, u8 *cmd)
{
	struct cmd_usb_rx_agg_param *param = (struct cmd_usb_rx_agg_param *) cmd;

	return _rtw_phl_cmd_usb_rx_agg_cfg(phl_info, param->mode,
					   param->agg_mode, param->drv_define,
					   param->timeout, param->size,
					   param->pkt_num);
}

enum rtw_phl_status
rtw_phl_cmd_usb_rx_agg_cfg(void *phl,
			   enum phl_usb_rx_agg_mode mode,
			   u8 agg_mode,
			   u8 drv_define,
			   u8 timeout,
			   u8 size,
			   u8 pkt_num,
			   enum phl_cmd_type cmd_type,
			   u32 cmd_timeout)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	void *drv = phl_to_drvpriv(phl_info);
	enum rtw_phl_status psts = RTW_PHL_STATUS_FAILURE;
	struct cmd_usb_rx_agg_param *param = NULL;
	u32 param_len = 0;

	if (cmd_type == PHL_CMD_DIRECTLY) {
		psts = _rtw_phl_cmd_usb_rx_agg_cfg(phl_info,
						   mode,
						   agg_mode,
						   drv_define,
						   timeout,
						   size,
						   pkt_num);
		goto _exit;
	}

	param_len =sizeof(struct cmd_usb_rx_agg_param);
	param = _os_kmem_alloc(drv, param_len);
	if (param == NULL) {
		PHL_ERR("%s: alloc param failed!\n", __func__);
		psts = RTW_PHL_STATUS_RESOURCE;
		goto _exit;
	}

	_os_mem_set(drv, param, 0, param_len);
	param->mode = mode;
	param->agg_mode = agg_mode;
	param->drv_define = drv_define;
	param->timeout = timeout;
	param->size = size;
	param->pkt_num = pkt_num;

	psts = phl_cmd_enqueue(phl_info,
			       HW_BAND_0,
			       MSG_EVT_USB_RX_AGG_CFG,
			       (u8 *)param,
			       param_len,
			       _phl_cmd_usb_rx_agg_cfg_done,
			       cmd_type,
			       cmd_timeout);
	if (is_cmd_failure(psts)) {
		/* Send cmd success, but wait cmd fail */
		psts = RTW_PHL_STATUS_FAILURE;
	} else if (psts != RTW_PHL_STATUS_SUCCESS) {
		/* Send cmd fail */
		psts = RTW_PHL_STATUS_FAILURE;
		_os_kmem_free(drv, param, param_len);
	}

_exit:
	return psts;
}
#endif

/* phl_trx_start_usb */
enum rtw_phl_status phl_trx_cfg_usb(struct phl_info_t *phl_info)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_SUCCESS;

#ifdef CONFIG_PHL_USB_RX_AGGREGATION
	rtw_hal_usb_rx_agg_init(phl_info->phl_com, phl_info->hal);
#endif
	os_enable_usb_out_pipes(phl_to_drvpriv(phl_info));
	os_enable_usb_in_pipes(phl_to_drvpriv(phl_info));

	pstatus = _phl_rx_start_usb(phl_info);

	return pstatus;
}


void phl_trx_stop_usb(struct phl_info_t *phl_info)
{
	_phl_rx_stop_usb(phl_info);
	os_disable_usb_out_pipes(phl_to_drvpriv(phl_info));
	os_disable_usb_in_pipes(phl_to_drvpriv(phl_info));
}


/*enqueue */
/* usb_read_port_complete */
enum rtw_phl_status
phl_pend_rxbuf_usb(struct phl_info_t *phl_info, void *rxobj, u32 inbuf_len, u8 status_code)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	void *drv = phl_to_drvpriv(phl_info);
	struct hci_info_t *hci_info = (struct hci_info_t *)phl_info->hci;
	struct rtw_rx_buf_ring *rx_buf_ring = (struct rtw_rx_buf_ring *)hci_info->rxbuf_pool;
	struct rtw_rx_buf *rx_buf = (struct rtw_rx_buf *)rxobj;

	if(!list_empty(&rx_buf_ring->busy_rxbuf_list.queue)) {

		pq_del_node(drv, &rx_buf_ring->busy_rxbuf_list, &rx_buf->list, _irq);
	}
	else
		PHL_ASSERT("[2] %s:: [Notice] rxbuf isn't in busy_rxbuf_list\n", __func__);

	if(status_code == RTW_PHL_STATUS_SUCCESS)
	{
		rx_buf->transfer_len = inbuf_len;
		if(pq_push(drv, &rx_buf_ring->pend_rxbuf_list, &rx_buf->list, _tail, _irq))
			pstatus = RTW_PHL_STATUS_SUCCESS;
	}
	else
	{
		PHL_TRACE(COMP_PHL_RECV, _PHL_INFO_, "[2] %s:: [Error] Complete\n", __func__);
		phl_release_rxbuf_usb(phl_info, rx_buf, 0, RTW_RX_TYPE_MAX); // usb doesn't care rtw_rx_type
	}

#if 0
	/* for the pause PHL Rx handshake with USB SER */
	if (PHL_RX_STATUS_SW_PAUSE == _os_atomic_read(drv,
		&phl_info->phl_sw_rx_sts))
		return RTW_PHL_STATUS_SUCCESS;
#endif

	if(status_code == RTW_PHL_STATUS_SUCCESS || status_code == RTW_PHL_STATUS_INVALID_PARAM)
	{
		if(_phl_in_token_usb(phl_info, rx_buf->pipe_idx) != RTW_PHL_STATUS_SUCCESS)
		{
			PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_, "[5] _phl_in_token_usb failed:: idle_rxbuf empty or intoken send failed \n");

			_os_set_timer(drv, &rx_buf_ring->deferred_timer, 10);
		}
	}

	return pstatus;
}

enum rtw_phl_status
phl_release_rxbuf_usb(struct phl_info_t *phl_info, void *r, u8 ch, enum rtw_rx_type type)
{
	void *drv = phl_to_drvpriv(phl_info);
	struct hci_info_t *hci_info = (struct hci_info_t *)phl_info->hci;
	struct rtw_rx_buf_ring *rx_buf_ring = NULL;
	struct rtw_rx_buf *rx_buf = (struct rtw_rx_buf *)r;
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	_os_spinlockfg sp_flags;
	bool	ret = false;

	rx_buf_ring = (struct rtw_rx_buf_ring *)hci_info->rxbuf_pool;
	if (rx_buf_ring != NULL && rx_buf != NULL) {

		_os_spinlock(drv, &rx_buf->lock, _irq, &sp_flags);
		if(rx_buf->pktcnt)
			rx_buf->pktcnt--;

		if(!rx_buf->pktcnt)
		{
			_os_mem_set(drv, rx_buf->buffer, 0, rx_buf->buf_len);
			INIT_LIST_HEAD(&rx_buf->list);
			_os_spinunlock(drv, &rx_buf->lock, _irq, &sp_flags);

			ret = pq_push(drv, &rx_buf_ring->idle_rxbuf_list, &rx_buf->list, _tail, _irq);
			if(ret)
				pstatus = RTW_PHL_STATUS_SUCCESS;

			PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "[4] %s:: [%p] idle_rxbuf_list.cnt=%d\n", __FUNCTION__, rx_buf, rx_buf_ring->idle_rxbuf_list.cnt);
		}
		else
		{
			_os_spinunlock(drv, &rx_buf->lock, _irq, &sp_flags);

			PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "[4] %s:: [%p] pktcnt =%d\n", __FUNCTION__, rx_buf, rx_buf->pktcnt);
			pstatus = RTW_PHL_STATUS_SUCCESS;
		}
	}

	return pstatus;
}

struct rtw_phl_rx_pkt *phl_get_single_rx(struct phl_info_t *phl_info,
					 struct rtw_rx_buf *rx_buf)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_FAILURE;
	void *drv = phl_to_drvpriv(phl_info);
	struct rtw_phl_rx_pkt *phl_rx = NULL, *phl_rxhead = NULL;

	u8 *pkt_buf, *pkt_buf_end, ref_cnt = 0;
	u8 *netbuf = NULL;
	s32 transfer_len = 0;
	s32 pkt_offset = 0, align_offset = 0;
	bool brel = true, balloc = false;
	_os_spinlockfg sp_flags;
	u8 mfrag = 0, frag_num = 0;
	u16 netbuf_len = 0;

	//initialize for compiler
	pkt_buf = rx_buf->buffer;
	pkt_buf_end = rx_buf->buffer + rx_buf->transfer_len;
	transfer_len = (s32)rx_buf->transfer_len;

	do {
		balloc = false;
		phl_rx = rtw_phl_query_phl_rx(phl_info);
		if (NULL == phl_rx) {
			PHL_TRACE(COMP_PHL_RECV, _PHL_ERR_, "[3] %s(%d):: [Error] phl_rx out of resource\n",
				__func__, __LINE__);

			pstatus = RTW_PHL_STATUS_RESOURCE;
			break;

		}

		// TODO::  Hal handle rx packet (rx desc/physts/hal rx data statistic)
		hstatus = rtw_hal_handle_rx_buffer(phl_info->phl_com,
							phl_info->hal,
							pkt_buf, transfer_len,
							phl_rx);

		if (RTW_HAL_STATUS_SUCCESS != hstatus) {
			phl_release_phl_rx(phl_info, phl_rx);
			phl_rx = NULL;
			pstatus = RTW_PHL_STATUS_FAILURE;

			/*ToDo:: Should we take care that one of ampdu damaged?*/
			if(ref_cnt)
				PHL_ASSERT("[3] %s:: [Notice] one of ampdu damaged\n", __FUNCTION__);
			break;
		}

		pkt_offset = (s32)(phl_rx->r.pkt_list[0].vir_addr - pkt_buf) + phl_rx->r.pkt_list[0].length;

#ifdef CONFIG_PHL_USB_RX_AGGREGATION
		align_offset = _ALIGN(pkt_offset, 8); /*8 bytes aligned*/
#else
		align_offset = pkt_offset;
#endif

		if(phl_rxhead == NULL)
		{
			phl_rxhead = phl_rx;
			INIT_LIST_HEAD(&phl_rxhead->list);

			PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "[3] %s:: [head %p]: transfer_len=%d\n",
							__FUNCTION__, phl_rx->r.pkt_list[0].vir_addr, transfer_len);
		}
		else
		{
			list_add_tail(&phl_rx->list, &phl_rxhead->list);

			PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_,"[3] %s:: [child %p]: offset[%d -> %d] transfer_len=%d\n",
						__FUNCTION__, phl_rx->r.pkt_list[0].vir_addr, pkt_offset, align_offset, transfer_len);
		}

		//shift buffer pointer
		pkt_buf = pkt_buf + align_offset;

		transfer_len -= align_offset;

		if (phl_rx->type == RTW_RX_TYPE_WIFI)
		{
			/* Pre-alloc netbuf and replace pkt_list[0].vir_addr */

			/* For first fragment packet, driver need allocate 1536 to defrag packet.*/
			mfrag = PHL_GET_80211_HDR_MORE_FRAG(phl_rx->r.pkt_list[0].vir_addr);
			frag_num = PHL_GET_80211_HDR_FRAG_NUM(phl_rx->r.pkt_list[0].vir_addr);

			if (mfrag == 1 && frag_num == 0) {
				if (phl_rx->r.pkt_list[0].length < RTW_MAX_ETH_PKT_LEN)
					netbuf_len = RTW_MAX_ETH_PKT_LEN;
				else
					netbuf_len = phl_rx->r.pkt_list[0].length;
			} else {
				netbuf_len = phl_rx->r.pkt_list[0].length;
			}

			netbuf = _os_alloc_netbuf(drv,
						netbuf_len,
						&(phl_rx->r.os_priv));

			if (netbuf) {
				_os_mem_cpy(drv, netbuf,
					phl_rx->r.pkt_list[0].vir_addr, phl_rx->r.pkt_list[0].length);
				phl_rx->r.pkt_list[0].vir_addr = netbuf;
				phl_rx->r.os_netbuf_len = netbuf_len;
				balloc = true;
			}
		}

		if(!balloc)
		{
			/* every phl_rx assign the same rx_buf */
			phl_rx->rxbuf_ptr = (u8 *)rx_buf;
			brel = false;
			ref_cnt++;
		}

		pstatus = RTW_PHL_STATUS_SUCCESS;

	} while (transfer_len > 0);


	if(phl_rxhead)
	{
		if(brel)
		{
			PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "[3] %s:: prealloc netbuff for all ampdu\n",
								__FUNCTION__);
			phl_release_rxbuf_usb(phl_info, rx_buf, phl_rxhead->r.mdata.dma_ch, phl_rxhead->type);
		}
		else
		{
			_os_spinlock(drv, &rx_buf->lock, _irq, &sp_flags);
			rx_buf->pktcnt= ref_cnt;
			_os_spinunlock(drv, &rx_buf->lock, _irq, &sp_flags);
			/*PHL_TRACE(COMP_PHL_RECV,_PHL_DEBUG_,"[3] %s:: phl_rx=%p, ref_cnt= %d\n",
												__FUNCTION__, phl_rx, ref_cnt);*/
		}
	}
	return phl_rxhead;
}

void rtw_phl_post_in_complete(void *phl, void *rxobj, u32 inbuf_len, u8 status_code)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_hci_trx_ops *hci_trx_ops = phl_info->hci_trx_ops;

	pstatus = hci_trx_ops->pend_rxbuf(phl_info, rxobj, inbuf_len, status_code);

	if(pstatus==RTW_PHL_STATUS_SUCCESS)
		rtw_phl_start_rx_process(phl);
}


void _phl_rx_handle_wp_report_usb(struct phl_info_t *phl_info,
							struct rtw_phl_rx_pkt *phl_rx)
{
	struct rtw_recv_pkt *r = &phl_rx->r;
	u8 *pkt = NULL;
	u16 pkt_len = 0;
	u16 rsize = 0;
	u8 macid = 0, ac_queue = 0, txsts = 0;

	pkt = r->pkt_list[0].vir_addr;
	pkt_len = r->pkt_list[0].length;

	while (pkt_len > 0) {
		rsize = rtw_hal_handle_wp_rpt_usb(phl_info->hal, pkt, pkt_len,
						&macid, &ac_queue, &txsts);
		if (0 == rsize)
			break;

		phl_rx_wp_report_record_sts(phl_info, macid, ac_queue, txsts);
		pkt += rsize;
		pkt_len -= rsize;
	}
}


static void phl_rx_process_usb(struct phl_info_t *phl_info,
							struct rtw_phl_rx_pkt *phl_rxhead)
{
	struct rtw_phl_rx_pkt *phl_rx = NULL;

	do {
		/* Get head of link list */
		phl_rx = phl_rxhead;

		/* assign next entry to head */
		phl_rxhead = list_entry(phl_rx->list.next, struct rtw_phl_rx_pkt, list);

		if(phl_rx == phl_rxhead)
			phl_rxhead=NULL;

		list_del(&phl_rx->list);

		switch (phl_rx->type) {
		case RTW_RX_TYPE_WIFI:
			// phl_recycle_rx_buf would in phl_rx_handle_normal or core_rx.
#ifdef CONFIG_PHL_RX_PSTS_PER_PKT
			if (false == phl_rx_proc_wait_phy_sts(phl_info, phl_rx)) {
				PHL_TRACE(COMP_PHL_PSTS, _PHL_DEBUG_,
					  "phl_rx_proc_wait_phy_sts() return false \n");
				phl_rx_handle_normal(phl_info, phl_rx);
			}
#else
			phl_rx_handle_normal(phl_info, phl_rx);
#endif
			break;
		case RTW_RX_TYPE_PPDU_STATUS:
			phl_rx_proc_ppdu_sts(phl_info, phl_rx);
#ifdef CONFIG_PHL_RX_PSTS_PER_PKT
			phl_rx_proc_phy_sts(phl_info, phl_rx);
#endif
			phl_recycle_rx_buf(phl_info, phl_rx);
			break;
		case RTW_RX_TYPE_TX_WP_RELEASE_HOST:
			#ifdef CONFIG_PHL_RELEASE_RPT_ENABLE
			_phl_rx_handle_wp_report_usb(phl_info, phl_rx);
			phl_recycle_rx_buf(phl_info, phl_rx);
			phl_rx = NULL;
			break;
			#endif /* CONFIG_PHL_RELEASE_RPT_ENABLE */
		case RTW_RX_TYPE_C2H:
		case RTW_RX_TYPE_CHANNEL_INFO:
		case RTW_RX_TYPE_TX_RPT:
		case RTW_RX_TYPE_DFS_RPT:
		case RTW_RX_TYPE_MAX:
			PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "phl_rx_process_usb(): Unsupported case:%d, please check it\n",
					phl_rx->type);
			phl_recycle_rx_buf(phl_info, phl_rx);
			phl_rx = NULL;
			break;
		default :
			PHL_TRACE(COMP_PHL_RECV, _PHL_INFO_, "[3] %s:: [Warning] rx type(0x%X) recycle \n",
					__FUNCTION__, phl_rx->type);
			phl_recycle_rx_buf(phl_info, phl_rx);
			phl_rx = NULL;
			break;
		}
	} while(phl_rxhead != NULL);
}

static enum rtw_phl_status phl_rx_usb(struct phl_info_t *phl_info)
{
	void *drv = phl_to_drvpriv(phl_info);
	struct hci_info_t *hci_info = (struct hci_info_t *)phl_info->hci;
	struct rtw_phl_rx_pkt *phl_rx = NULL;
	struct rtw_rx_buf_ring *rx_buf_ring = (struct rtw_rx_buf_ring *)hci_info->rxbuf_pool;
	struct rtw_rx_buf *rx_buf = NULL;
	_os_list* obj = NULL;

	while (rx_buf_ring->pend_rxbuf_list.cnt)
	{
		PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "[3] %s:: pend_rxbuf_list.cnt =%d\n",
							__FUNCTION__, rx_buf_ring->pend_rxbuf_list.cnt );

		if(pq_pop(drv, &rx_buf_ring->pend_rxbuf_list, &obj, _first, _irq)) {
			rx_buf = (struct rtw_rx_buf*)obj;
		}
		else
			break;

		switch(rx_buf->pipe_idx){
			case WLAN_IN_MPDU_PIPE_IDX:
			{
				/* phl_rx maybe single or link-list */
				phl_rx = phl_get_single_rx(phl_info, rx_buf);

				if(phl_rx)
				{
					phl_rx_process_usb(phl_info, phl_rx);
				}
			}
			break;

			case WLAN_IN_INTERRUPT_PIPE_IDX:
			break;

			default:
			break;
		}

		if (phl_rx==NULL && NULL != rx_buf) {
			PHL_TRACE(COMP_PHL_RECV, _PHL_INFO_, "[3] %s:: [Error] [%p] pipe_idx = %d \n",
											__FUNCTION__, rx_buf, rx_buf->pipe_idx);
			phl_release_rxbuf_usb(phl_info, rx_buf, 0, RTW_RX_TYPE_MAX); // usb doesn't care rtw_rx_type
			rx_buf=NULL;
		}

	}

	return RTW_PHL_STATUS_SUCCESS;
}

enum rtw_phl_status phl_pltfm_tx_usb(struct phl_info_t *phl_info,
									void *pkt)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	struct rtw_h2c_pkt *h2c_pkt = (struct rtw_h2c_pkt *)pkt;
	struct phl_usb_tx_buf_resource *reso = NULL;
	struct phl_usb_buf *tx_buf = NULL;
	u8 bulk_id = 0;
	u8 dma_ch = rtw_hal_get_fwcmd_queue_idx(phl_info->hal);

	reso = (struct phl_usb_tx_buf_resource *)phl_info->hci->txbuf_pool;

	if (reso)
		tx_buf = dequeue_usb_buf(phl_info, &reso->h2c_txbuf_list);
	else
		PHL_DBG("%s, phl_usb_tx_buf_resource == NULL\n", __func__);

	if (tx_buf) {
		tx_buf->buffer = (u8 *)h2c_pkt;

		bulk_id = rtw_hal_get_bulkout_id(phl_info->hal,
			dma_ch, 0);
		pstatus = os_usb_tx(phl_to_drvpriv(phl_info),
			(u8 *)tx_buf, bulk_id, h2c_pkt->data_len, h2c_pkt->vir_data);
		if (pstatus == RTW_PHL_STATUS_FAILURE) {
			phl_enqueue_idle_h2c_pkt(phl_info,
				(struct rtw_h2c_pkt *)tx_buf->buffer);
			tx_buf->buffer = NULL;
			enqueue_usb_buf(phl_info,
				&reso->h2c_txbuf_list, tx_buf, _tail);
		}
	}
	return pstatus;
}

static void _phl_free_h2c_pkt_buf_usb(struct phl_info_t *phl_info,
					struct rtw_h2c_pkt *_h2c_pkt)
{
	struct rtw_h2c_pkt *h2c_pkt = _h2c_pkt;

	_os_kmem_free(phl_to_drvpriv(phl_info),
		h2c_pkt->vir_head, h2c_pkt->buf_len);
}

enum rtw_phl_status _phl_alloc_h2c_pkt_buf_usb(struct phl_info_t *phl_info,
					struct rtw_h2c_pkt *_h2c_pkt,
					u32 buf_len)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	struct rtw_h2c_pkt *h2c_pkt = _h2c_pkt;

	h2c_pkt->vir_head = _os_kmem_alloc(phl_to_drvpriv(phl_info), buf_len);

	if (h2c_pkt->vir_head)
		pstatus = RTW_PHL_STATUS_SUCCESS;

	return pstatus;
}

void *phl_get_txbd_buf_usb(struct phl_info_t *phl_info)
{
	return NULL;
}

void *phl_get_rxbd_buf_usb(struct phl_info_t *phl_info)
{
	return NULL;
}

void phl_recycle_rx_pkt_usb(struct phl_info_t *phl_info,
				struct rtw_phl_rx_pkt *phl_rx)
{
	/* Using r.os_priv seems imprperly. */
	/* free netbuff for error case*/
	if (phl_rx->r.os_priv)
		_os_free_netbuf(phl_to_drvpriv(phl_info),
			phl_rx->r.pkt_list[0].vir_addr,
			phl_rx->r.os_netbuf_len,
			phl_rx->r.os_priv);

	phl_recycle_rx_buf(phl_info, phl_rx);
}

void phl_tx_watchdog_usb(struct phl_info_t *phl_info)
{

}

static struct phl_hci_trx_ops ops= {
	.hci_trx_init = phl_trx_init_usb,
	.hci_trx_deinit = phl_trx_deinit_usb,
	.prepare_tx = phl_prepare_tx_usb,
	.recycle_tx_buf = phl_tx_release_buf_usb,
	.recycle_rx_buf = phl_release_rxbuf_usb,
	.tx = phl_tx_usb,
	.rx = phl_rx_usb,
	.trx_cfg = phl_trx_cfg_usb,
	.trx_stop = phl_trx_stop_usb,
	.pend_rxbuf = phl_pend_rxbuf_usb,
	.pltfm_tx = phl_pltfm_tx_usb,
	.alloc_h2c_pkt_buf = _phl_alloc_h2c_pkt_buf_usb,
	.free_h2c_pkt_buf = _phl_free_h2c_pkt_buf_usb,
	.trx_reset = phl_trx_reset_usb,
	.trx_resume = phl_trx_resume_usb,
	.tx_reset_hwband = phl_tx_reset_hwband_usb,
	.req_tx_stop = phl_req_tx_stop_usb,
	.req_rx_stop = phl_req_rx_stop_usb,
	.is_tx_pause = phl_is_tx_sw_pause_usb,
	.is_rx_pause = phl_is_rx_sw_pause_usb,
	.get_txbd_buf = phl_get_txbd_buf_usb,
	.get_rxbd_buf = phl_get_rxbd_buf_usb,
	.recycle_rx_pkt = phl_recycle_rx_pkt_usb,
	.rx_handle_normal = phl_rx_handle_normal,
	.tx_watchdog = phl_tx_watchdog_usb
};

enum rtw_phl_status phl_hook_trx_ops_usb(struct phl_info_t *phl_info)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;

	if (NULL != phl_info) {
		phl_info->hci_trx_ops = &ops;
		pstatus = RTW_PHL_STATUS_SUCCESS;
	}

	return pstatus;
}

u8 rtw_phl_usb_tx_ep_id(void *phl, u16 macid, u8 tid, u8 band)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	u8 dma_ch = 0;
	u8 bulk_id = 0;
	dma_ch = rtw_hal_tx_chnl_mapping(phl_info->hal, macid,
							 tid, band);
	bulk_id = rtw_hal_get_bulkout_id(phl_info->hal,
			dma_ch, 0);
	return bulk_id;
}

enum rtw_phl_status
phl_force_usb_switch(struct phl_info_t *phl_info, u32 speed)
{

	if(speed <= RTW_USB_SPEED_HIGH)
		rtw_hal_force_usb_switch(phl_info->hal, USB_2_0);
	else if(speed < RTW_USB_SPEED_MAX)
		rtw_hal_force_usb_switch(phl_info->hal, USB_3_0);
	PHL_INFO("%s (%d) !!\n", __FUNCTION__, speed);

	return RTW_PHL_STATUS_SUCCESS;
}
#ifdef CONFIG_CMD_DISP
enum rtw_phl_status
rtw_phl_cmd_force_usb_switch(void *phl, u32 speed,
				enum phl_band_idx band_idx,
				enum phl_cmd_type cmd_type, u32 cmd_timeout)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	enum rtw_phl_status psts = RTW_PHL_STATUS_FAILURE;
	u32 usb_speed = speed;

	if (cmd_type == PHL_CMD_DIRECTLY)
		psts = phl_force_usb_switch(phl_info, usb_speed);
	else {
		psts = phl_cmd_enqueue(phl_info,
		                       band_idx,
		                       MSG_EVT_FORCE_USB_SW,
		                       (u8*)&usb_speed,
		                       sizeof(u32),
		                       NULL,
		                       PHL_CMD_WAIT,
		                       0);

		if (is_cmd_failure(psts)) {
			/* Send cmd success, but wait cmd fail*/
			psts = RTW_PHL_STATUS_FAILURE;
		} else if (psts != RTW_PHL_STATUS_SUCCESS) {
			/* Send cmd fail */
			psts = RTW_PHL_STATUS_FAILURE;
		}
	}
	return psts;
}
#else /*for FSM*/
enum rtw_phl_status
rtw_phl_cmd_force_usb_switch(void *phl, u32 speed,
				enum phl_band_idx band_idx,
				enum phl_cmd_type cmd_type, u32 cmd_timeout)
{
	return phl_force_usb_switch((struct phl_info_t *)phl, speed);
}
#endif /*CONFIG_CMD_DISP*/

enum rtw_phl_status
rtw_phl_cmd_get_usb_speed(void *phl, u32* speed,
				enum phl_band_idx band_idx,
				enum phl_cmd_type cmd_type, u32 cmd_timeout)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	enum rtw_phl_status psts = RTW_PHL_STATUS_FAILURE;

#ifdef CONFIG_CMD_DISP
	psts = phl_cmd_enqueue(phl_info,
				band_idx,
				MSG_EVT_GET_USB_SPEED,
				(u8*)speed,
				sizeof(u32),
				NULL,
				PHL_CMD_WAIT,
				0);
	if (is_cmd_failure(psts)) {
		/* Send cmd success, but wait cmd fail*/
		psts = RTW_PHL_STATUS_FAILURE;
	} else if (psts != RTW_PHL_STATUS_SUCCESS) {
		/* Send cmd fail */
		psts = RTW_PHL_STATUS_FAILURE;
	}
#else
	psts = phl_get_cur_usb_speed(phl_info, speed);
#endif
	return psts;
}
enum rtw_phl_status
rtw_phl_cmd_get_usb_support_ability(void *phl, u32* ability,
				enum phl_band_idx band_idx,
				enum phl_cmd_type cmd_type, u32 cmd_timeout)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	enum rtw_phl_status psts = RTW_PHL_STATUS_FAILURE;

#ifdef CONFIG_CMD_DISP
	psts = phl_cmd_enqueue(phl_info,
				band_idx,
				MSG_EVT_GET_USB_SW_ABILITY,
				(u8*)ability,
				sizeof(u32),
				NULL,
				PHL_CMD_WAIT,
				0);

	if (is_cmd_failure(psts)) {
		/* Send cmd success, but wait cmd fail*/
		psts = RTW_PHL_STATUS_FAILURE;
	} else if (psts != RTW_PHL_STATUS_SUCCESS) {
		/* Send cmd fail */
		psts = RTW_PHL_STATUS_FAILURE;
	}
#else
	psts = phl_get_usb_support_ability(phl_info, ability);
#endif
	return psts;
}

enum rtw_phl_status phl_get_cur_usb_speed(struct phl_info_t *phl_info, u32 *speed)
{
	u32 mode = rtwl_hal_get_cur_usb_mode(phl_info->hal);
	struct hci_info_t *hci = phl_info->hci;

	switch(mode) {
		case USB_1_1:
			*speed = RTW_USB_SPEED_FULL;
			hci->usb_bulkout_size = USB_FULL_SPEED_BULK_SIZE;
			break;
		case USB_2_0:
			*speed = RTW_USB_SPEED_HIGH;
			hci->usb_bulkout_size = USB_HIGH_SPEED_BULK_SIZE;
			break;
		case USB_3_0:
			*speed = RTW_USB_SPEED_SUPER;
			hci->usb_bulkout_size = USB_SUPER_SPEED_BULK_SIZE;
			break;
		default:
			*speed = RTW_USB_SPEED_UNKNOWN;
			break;
	}
	PHL_PRINT("%s (%d) bulk size(%d) !!\n", __FUNCTION__, *speed,
					phl_info->hci->usb_bulkout_size);

	return RTW_PHL_STATUS_SUCCESS;
}
enum rtw_phl_status
phl_get_usb_support_ability(struct phl_info_t *phl_info, u32 *ability)
{
	*ability = rtwl_hal_get_usb_support_ability(phl_info->hal);
	/* refer enum rtw_usb_sw_ability for definition of ability */
	PHL_INFO("%s (%d) !!\n", __FUNCTION__, *ability);
	return RTW_PHL_STATUS_SUCCESS;
}


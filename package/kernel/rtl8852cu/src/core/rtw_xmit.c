/******************************************************************************
 *
 * Copyright(c) 2007 - 2021 Realtek Corporation.
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
#define _RTW_XMIT_C_

#include <drv_types.h>

static u8 P802_1H_OUI[P80211_OUI_LEN] = { 0x00, 0x00, 0xf8 };
static u8 RFC1042_OUI[P80211_OUI_LEN] = { 0x00, 0x00, 0x00 };

static void _init_txservq(struct tx_servq *ptxservq)
{
	_rtw_init_listhead(&ptxservq->tx_pending);
	_rtw_init_queue(&ptxservq->sta_pending);
	ptxservq->qcnt = 0;
}


void	_rtw_init_sta_xmit_priv(struct sta_xmit_priv *psta_xmitpriv)
{


	_rtw_memset((unsigned char *)psta_xmitpriv, 0, sizeof(struct sta_xmit_priv));

	_rtw_spinlock_init(&psta_xmitpriv->lock);

	/* for(i = 0 ; i < MAX_NUMBLKS; i++) */
	/*	_init_txservq(&(psta_xmitpriv->blk_q[i])); */

	_init_txservq(&psta_xmitpriv->be_q);
	_init_txservq(&psta_xmitpriv->bk_q);
	_init_txservq(&psta_xmitpriv->vi_q);
	_init_txservq(&psta_xmitpriv->vo_q);
	_rtw_init_listhead(&psta_xmitpriv->legacy_dz);
	_rtw_init_listhead(&psta_xmitpriv->apsd);


}

void rtw_init_xmit_block(_adapter *padapter)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);

	_rtw_spinlock_init(&dvobj->xmit_block_lock);
	dvobj->xmit_block = XMIT_BLOCK_NONE;

}
void rtw_free_xmit_block(_adapter *padapter)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);

	_rtw_spinlock_free(&dvobj->xmit_block_lock);
}

#ifdef RTW_PHL_TX
u8 alloc_txring(_adapter *padapter)
{
	struct xmit_txreq_buf *ptxreq_buf = NULL;
	u32 idx, alloc_sz = 0, alloc_sz_txreq = 0;
	u8 res = _SUCCESS;

	u32 offset_head = (sizeof(struct rtw_xmit_req) * RTW_MAX_FRAG_NUM);
	u32 offset_tail = offset_head + (SZ_HEAD_BUF * RTW_MAX_FRAG_NUM);
	u32 offset_list = offset_tail + (SZ_TAIL_BUF * RTW_MAX_FRAG_NUM);

	PHLTX_ENTER;

	alloc_sz = (SZ_TX_RING * RTW_MAX_FRAG_NUM);
	alloc_sz_txreq = MAX_TX_RING_NUM * (sizeof(struct xmit_txreq_buf));

	RTW_INFO("eric-tx [%s] alloc_sz = %d, alloc_sz_txreq = %d\n", __FUNCTION__, alloc_sz, alloc_sz_txreq);

	padapter->pxmit_txreq_buf = rtw_vmalloc(alloc_sz_txreq);
	ptxreq_buf = (struct xmit_txreq_buf *)padapter->pxmit_txreq_buf;

	_rtw_init_queue(&padapter->free_txreq_queue);

	for (idx = 0; idx < MAX_TX_RING_NUM; idx++) {

		padapter->tx_pool_ring[idx] = rtw_zmalloc(alloc_sz);
		if (!padapter->tx_pool_ring[idx]) {
			RTW_ERR("[core] alloc txring fail, plz check.\n");
			res = _FAIL;
			break;
		}
		_rtw_init_listhead(&ptxreq_buf->list);
		ptxreq_buf->txreq = padapter->tx_pool_ring[idx];
		ptxreq_buf->head = padapter->tx_pool_ring[idx] + offset_head;
		ptxreq_buf->tail = padapter->tx_pool_ring[idx] + offset_tail;
		ptxreq_buf->pkt_list = padapter->tx_pool_ring[idx] + offset_list;

		#ifdef USE_PREV_WLHDR_BUF /* CONFIG_CORE_TXSC */
		ptxreq_buf->macid = 0xff;
		ptxreq_buf->txsc_id = 0xff;
		#endif

		rtw_list_insert_tail(&(ptxreq_buf->list), &(padapter->free_txreq_queue.queue));

		ptxreq_buf++;
	}

	padapter->free_txreq_cnt = MAX_TX_RING_NUM;

	return res;
}

void free_txring(_adapter *padapter)
{
	u32 idx, alloc_sz = 0, alloc_sz_txreq = 0;
#ifdef CONFIG_CORE_TXSC
	struct rtw_xmit_req *txreq = NULL;
	struct xmit_txreq_buf *txreq_buf = NULL;
	u8 j;
#endif

	PHLTX_ENTER;

	alloc_sz = (SZ_TX_RING * RTW_MAX_FRAG_NUM);
	alloc_sz_txreq = MAX_TX_RING_NUM * (sizeof(struct xmit_txreq_buf));

	RTW_INFO("eric-tx [%s] alloc_sz = %d,  alloc_sz_txreq = %d\n", __func__, alloc_sz, alloc_sz_txreq);

	for (idx = 0; idx < MAX_TX_RING_NUM; idx++) {
		if (padapter->tx_pool_ring[idx]) {
#ifdef CONFIG_CORE_TXSC
			txreq = (struct rtw_xmit_req *)padapter->tx_pool_ring[idx];
			if (txreq->treq_type == RTW_PHL_TREQ_TYPE_CORE_TXSC) {
				txreq_buf = (struct xmit_txreq_buf *)txreq->os_priv;
				if (txreq_buf) {
					/* CONFGI_TXSC_AMSDU */
					for (j = 0; j < txreq_buf->pkt_cnt; j++) {
						if (txreq_buf->pkt[j])
							rtw_os_pkt_complete(padapter, (void *)txreq_buf->pkt[j]);
					}
				}
			}
#endif
			rtw_mfree(padapter->tx_pool_ring[idx], alloc_sz);
		}
	}

	_rtw_spinlock_free(&padapter->free_txreq_queue.lock);
	rtw_vmfree(padapter->pxmit_txreq_buf, alloc_sz_txreq);
}

#endif


s32 _rtw_init_xmit_priv(struct xmit_priv *pxmitpriv, _adapter *padapter)
{
	int i;
	#if 0 /*CONFIG_CORE_XMITBUF*/
	struct xmit_buf *pxmitbuf;
	#endif
	struct xmit_frame *pxframe;
	sint res = _SUCCESS;
	/* MGT_TXREQ_MGT */
	u8 *txreq = NULL, *pkt_list = NULL;

	#if 0 /*CONFIG_CORE_XMITBUF*/
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);

	u8 xmitbuf_nr = GET_HAL_XMITBUF_NR(dvobj);
	u16 xmitbuf_sz = GET_HAL_XMITBUF_SZ(dvobj);

	u8 xmitbuf_ext_nr = GET_HAL_XMITBUF_EXT_NR(dvobj);
	u16 xmitbuf_ext_sz = GET_HAL_XMITBUF_EXT_SZ(dvobj);
	#endif

	/* We don't need to memset padapter->XXX to zero, because adapter is allocated by rtw_zvmalloc(). */
	/* _rtw_memset((unsigned char *)pxmitpriv, 0, sizeof(struct xmit_priv)); */

	_rtw_spinlock_init(&pxmitpriv->lock);
	_rtw_spinlock_init(&pxmitpriv->lock_sctx);
	#if 0 /*def CONFIG_XMIT_THREAD_MODE*/
	_rtw_init_sema(&pxmitpriv->xmit_sema, 0);
	#endif

	/*
	Please insert all the queue initializaiton using _rtw_init_queue below
	*/

	pxmitpriv->adapter = padapter;

	/* for(i = 0 ; i < MAX_NUMBLKS; i++) */
	/*	_rtw_init_queue(&pxmitpriv->blk_strms[i]); */

	_rtw_init_queue(&pxmitpriv->be_pending);
	_rtw_init_queue(&pxmitpriv->bk_pending);
	_rtw_init_queue(&pxmitpriv->vi_pending);
	_rtw_init_queue(&pxmitpriv->vo_pending);
	_rtw_init_queue(&pxmitpriv->bm_pending);

	/* _rtw_init_queue(&pxmitpriv->legacy_dz_queue); */
	/* _rtw_init_queue(&pxmitpriv->apsd_queue); */

	_rtw_init_queue(&pxmitpriv->free_xmit_queue);

	/*
	Please allocate memory with the sz = (struct xmit_frame) * NR_XMITFRAME,
	and initialize free_xmit_frame below.
	Please also apply  free_txobj to link_up all the xmit_frames...
	*/

	pxmitpriv->pallocated_frame_buf = rtw_zvmalloc(NR_XMITFRAME * sizeof(struct xmit_frame) + 4);

	if (pxmitpriv->pallocated_frame_buf  == NULL) {
		pxmitpriv->pxmit_frame_buf = NULL;
		res = _FAIL;
		goto exit;
	}
	pxmitpriv->pxmit_frame_buf = (u8 *)N_BYTE_ALIGMENT((SIZE_PTR)(pxmitpriv->pallocated_frame_buf), 4);
	/* pxmitpriv->pxmit_frame_buf = pxmitpriv->pallocated_frame_buf + 4 - */
	/*						((SIZE_PTR) (pxmitpriv->pallocated_frame_buf) &3); */

	pxframe = (struct xmit_frame *) pxmitpriv->pxmit_frame_buf;

	for (i = 0; i < NR_XMITFRAME; i++) {
		_rtw_init_listhead(&(pxframe->list));

		pxframe->padapter = padapter;
		pxframe->frame_tag = NULL_FRAMETAG;

		pxframe->pkt = NULL;

		#if 0 /*CONFIG_CORE_XMITBUF*/
		pxframe->buf_addr = NULL;
		pxframe->pxmitbuf = NULL;
		#else
		/*alloc buf_addr*/
		/*rtw_os_xmit_resource_alloc(padapter, pxframe);*/
		#endif

		rtw_list_insert_tail(&(pxframe->list), &(pxmitpriv->free_xmit_queue.queue));

		pxframe++;
	}

	pxmitpriv->free_xmitframe_cnt = NR_XMITFRAME;

	pxmitpriv->frag_len = MAX_FRAG_THRESHOLD;

	#if 0 /*CONFIG_CORE_XMITBUF*/
	/* init xmit_buf */
	_rtw_init_queue(&pxmitpriv->free_xmitbuf_queue);
	_rtw_init_queue(&pxmitpriv->pending_xmitbuf_queue);

	pxmitpriv->pallocated_xmitbuf = rtw_zvmalloc(xmitbuf_nr * sizeof(struct xmit_buf) + 4);

	if (pxmitpriv->pallocated_xmitbuf  == NULL) {
		res = _FAIL;
		goto exit;
	}

	pxmitpriv->pxmitbuf = (u8 *)N_BYTE_ALIGMENT((SIZE_PTR)(pxmitpriv->pallocated_xmitbuf), 4);
	/* pxmitpriv->pxmitbuf = pxmitpriv->pallocated_xmitbuf + 4 - */
	/*						((SIZE_PTR) (pxmitpriv->pallocated_xmitbuf) &3); */

	pxmitbuf = (struct xmit_buf *)pxmitpriv->pxmitbuf;

	for (i = 0; i < xmitbuf_nr; i++) {
		_rtw_init_listhead(&pxmitbuf->list);

		pxmitbuf->priv_data = NULL;
		pxmitbuf->padapter = padapter;
		pxmitbuf->buf_tag = XMITBUF_DATA;

		/* Tx buf allocation may fail sometimes, so sleep and retry. */
		res = rtw_os_xmit_resource_alloc(padapter, pxmitbuf,
					(xmitbuf_sz + SZ_ALIGN_XMITFRAME_EXT), _TRUE);
		if (res == _FAIL) {
			rtw_msleep_os(10);
			res = rtw_os_xmit_resource_alloc(padapter, pxmitbuf,
					(xmitbuf_sz + SZ_ALIGN_XMITFRAME_EXT), _TRUE);
			if (res == _FAIL)
				goto exit;
		}

#if defined(CONFIG_SDIO_HCI) || defined(CONFIG_GSPI_HCI)
		pxmitbuf->phead = pxmitbuf->pbuf;
		pxmitbuf->pend = pxmitbuf->pbuf + xmitbuf_sz;
		pxmitbuf->len = 0;
		pxmitbuf->pdata = pxmitbuf->ptail = pxmitbuf->phead;
#endif

		pxmitbuf->flags = XMIT_VO_QUEUE;

		rtw_list_insert_tail(&pxmitbuf->list, &(pxmitpriv->free_xmitbuf_queue.queue));
#ifdef DBG_XMIT_BUF
		pxmitbuf->no = i;
#endif

		pxmitbuf++;

	}

	pxmitpriv->free_xmitbuf_cnt = xmitbuf_nr;
	#endif
	/* init xframe_ext queue,  the same count as extbuf */
	_rtw_init_queue(&pxmitpriv->free_xframe_ext_queue);
#ifdef CONFIG_LAYER2_ROAMING
	_rtw_init_queue(&pxmitpriv->rpkt_queue);
#endif

	pxmitpriv->xframe_ext_alloc_addr = rtw_zvmalloc(NR_XMITFRAME_EXT * sizeof(struct xmit_frame) + 4);

	if (pxmitpriv->xframe_ext_alloc_addr  == NULL) {
		pxmitpriv->xframe_ext = NULL;
		res = _FAIL;
		goto exit;
	}
	pxmitpriv->xframe_ext = (u8 *)N_BYTE_ALIGMENT((SIZE_PTR)(pxmitpriv->xframe_ext_alloc_addr), 4);
	pxframe = (struct xmit_frame *)pxmitpriv->xframe_ext;

	/* MGT_TXREQ_QMGT */
	pxmitpriv->xframe_ext_txreq_alloc_addr = rtw_zmalloc(NR_XMITFRAME_EXT * SZ_MGT_RING);
	if (pxmitpriv->xframe_ext_txreq_alloc_addr  == NULL) {
		pxmitpriv->xframe_ext_txreq = NULL;
		res = _FAIL;
		goto exit;
	}
	pxmitpriv->xframe_ext_txreq = pxmitpriv->xframe_ext_txreq_alloc_addr;
	txreq = pxmitpriv->xframe_ext_txreq;
	pkt_list = pxmitpriv->xframe_ext_txreq + sizeof(struct rtw_xmit_req);

	for (i = 0; i < NR_XMITFRAME_EXT; i++) {
		_rtw_init_listhead(&(pxframe->list));

		pxframe->padapter = padapter;
		pxframe->frame_tag = NULL_FRAMETAG;

		pxframe->pkt = NULL;

		#if 0 /*CONFIG_CORE_XMITBUF*/
		pxframe->buf_addr = NULL;
		pxframe->pxmitbuf = NULL;
		#else
		/*alloc buf_addr*/
		rtw_os_xmit_resource_alloc(padapter, pxframe);
		#endif

		pxframe->ext_tag = 1;

		/* MGT_TXREQ_QMGT */
		pxframe->phl_txreq = (struct rtw_xmit_req *)txreq;
		pxframe->phl_txreq->pkt_list = pkt_list;

		rtw_list_insert_tail(&(pxframe->list), &(pxmitpriv->free_xframe_ext_queue.queue));

		pxframe++;
		/* MGT_TXREQ_QMGT */
		txreq += SZ_MGT_RING;
		pkt_list += SZ_MGT_RING;
	}
	pxmitpriv->free_xframe_ext_cnt = NR_XMITFRAME_EXT;

#if 0 /*CONFIG_CORE_XMITBUF*/
	/* Init xmit extension buff */
	_rtw_init_queue(&pxmitpriv->free_xmit_extbuf_queue);

	pxmitpriv->pallocated_xmit_extbuf = rtw_zvmalloc(xmitbuf_ext_nr * sizeof(struct xmit_buf) + 4);

	if (pxmitpriv->pallocated_xmit_extbuf  == NULL) {
		res = _FAIL;
		goto exit;
	}

	pxmitpriv->pxmit_extbuf = (u8 *)N_BYTE_ALIGMENT((SIZE_PTR)(pxmitpriv->pallocated_xmit_extbuf), 4);

	pxmitbuf = (struct xmit_buf *)pxmitpriv->pxmit_extbuf;

	for (i = 0; i < xmitbuf_ext_nr; i++) {
		_rtw_init_listhead(&pxmitbuf->list);

		pxmitbuf->priv_data = NULL;
		pxmitbuf->padapter = padapter;
		pxmitbuf->buf_tag = XMITBUF_MGNT;

		res = rtw_os_xmit_resource_alloc(padapter, pxmitbuf,
					xmitbuf_ext_sz + SZ_ALIGN_XMITFRAME_EXT, _TRUE);
		if (res == _FAIL) {
			res = _FAIL;
			goto exit;
		}

#if defined(CONFIG_SDIO_HCI) || defined(CONFIG_GSPI_HCI)
		pxmitbuf->phead = pxmitbuf->pbuf;
		pxmitbuf->pend = pxmitbuf->pbuf + xmitbuf_ext_sz;
		pxmitbuf->len = 0;
		pxmitbuf->pdata = pxmitbuf->ptail = pxmitbuf->phead;
#endif

		rtw_list_insert_tail(&pxmitbuf->list, &(pxmitpriv->free_xmit_extbuf_queue.queue));
#ifdef DBG_XMIT_BUF_EXT
		pxmitbuf->no = i;
#endif
		pxmitbuf++;

	}

	pxmitpriv->free_xmit_extbuf_cnt = xmitbuf_ext_nr;

	/*GEORGIA_TODO_FIXIT_IC_GEN_DEPENDENCE*/
	for (i = 0; i < CMDBUF_MAX; i++) {
		pxmitbuf = &pxmitpriv->pcmd_xmitbuf[i];
		if (pxmitbuf) {
			_rtw_init_listhead(&pxmitbuf->list);

			pxmitbuf->priv_data = NULL;
			pxmitbuf->padapter = padapter;
			pxmitbuf->buf_tag = XMITBUF_CMD;

			res = rtw_os_xmit_resource_alloc(padapter, pxmitbuf,
					MAX_CMDBUF_SZ + SZ_ALIGN_XMITFRAME_EXT, _TRUE);
			if (res == _FAIL) {
				res = _FAIL;
				goto exit;
			}

#if defined(CONFIG_SDIO_HCI) || defined(CONFIG_GSPI_HCI)
			pxmitbuf->phead = pxmitbuf->pbuf;
			pxmitbuf->pend = pxmitbuf->pbuf + MAX_CMDBUF_SZ;
			pxmitbuf->len = 0;
			pxmitbuf->pdata = pxmitbuf->ptail = pxmitbuf->phead;
#endif
			pxmitbuf->alloc_sz = MAX_CMDBUF_SZ + SZ_ALIGN_XMITFRAME_EXT;
		}
	}
#endif
	rtw_alloc_hwxmits(padapter);
	rtw_init_hwxmits(pxmitpriv->hwxmits, pxmitpriv->hwxmit_entry);

	for (i = 0; i < 4; i++)
		pxmitpriv->wmm_para_seq[i] = i;

#ifdef CONFIG_USB_HCI
	pxmitpriv->txirp_cnt = 1;

	_rtw_init_sema(&(pxmitpriv->tx_retevt), 0);

	/* per AC pending irp */
	pxmitpriv->beq_cnt = 0;
	pxmitpriv->bkq_cnt = 0;
	pxmitpriv->viq_cnt = 0;
	pxmitpriv->voq_cnt = 0;
#ifdef PRIVATE_R
	pxmitpriv->tx_bk_drop_cnt = 0;
	pxmitpriv->tx_vi_drop_cnt = 0;
	pxmitpriv->tx_vo_drop_cnt = 0;
	pxmitpriv->tx_be_drop_cnt = 0;
#endif
#endif


#ifdef CONFIG_XMIT_ACK
	pxmitpriv->ack_tx = _FALSE;
	_rtw_mutex_init(&pxmitpriv->ack_tx_mutex);
	rtw_sctx_init(&pxmitpriv->ack_tx_ops, 0);
	#ifdef CONFIG_XMIT_ACK_BY_REL_RPT
	rtw_ack_txfb_init(padapter, &(pxmitpriv->ack_txfb));
	#endif
#endif

#ifdef CONFIG_TX_AMSDU
	rtw_init_timer(&(pxmitpriv->amsdu_vo_timer),
		rtw_amsdu_vo_timeout_handler, padapter);
	pxmitpriv->amsdu_vo_timeout = RTW_AMSDU_TIMER_UNSET;

	rtw_init_timer(&(pxmitpriv->amsdu_vi_timer),
		rtw_amsdu_vi_timeout_handler, padapter);
	pxmitpriv->amsdu_vi_timeout = RTW_AMSDU_TIMER_UNSET;

	rtw_init_timer(&(pxmitpriv->amsdu_be_timer),
		rtw_amsdu_be_timeout_handler, padapter);
	pxmitpriv->amsdu_be_timeout = RTW_AMSDU_TIMER_UNSET;

	rtw_init_timer(&(pxmitpriv->amsdu_bk_timer),
		rtw_amsdu_bk_timeout_handler, padapter);
	pxmitpriv->amsdu_bk_timeout = RTW_AMSDU_TIMER_UNSET;

	pxmitpriv->amsdu_debug_set_timer = 0;
	pxmitpriv->amsdu_debug_timeout = 0;
	pxmitpriv->amsdu_debug_tasklet = 0;
	pxmitpriv->amsdu_debug_enqueue = 0;
	pxmitpriv->amsdu_debug_dequeue = 0;
	for (i = 0; i < AMSDU_DEBUG_MAX_COUNT; i++)
		pxmitpriv->amsdu_debug_coalesce[i] = 0;
#endif
#ifdef DBG_TXBD_DESC_DUMP
	pxmitpriv->dump_txbd_desc = 0;
#endif
	rtw_init_xmit_block(padapter);
	rtw_intf_init_xmit_priv(padapter);

#ifdef RTW_PHL_TX //alloc xmit resource
	printk("eric-tx CALL alloc_txring !!!!\n");
	if (alloc_txring(padapter) == _FAIL) {
		RTW_ERR("[core] alloc_txring fail !!!\n");
		res = _FAIL;
		goto exit;
	}
#endif

#if defined(CONFIG_CORE_TXSC)
	_rtw_spinlock_init(&pxmitpriv->txsc_lock);
#endif

exit:

	return res;
}

void  rtw_mfree_xmit_priv_lock(struct xmit_priv *pxmitpriv)
{
	_rtw_spinlock_free(&pxmitpriv->lock);
	#if 0 /*def CONFIG_XMIT_THREAD_MODE*/
	_rtw_free_sema(&pxmitpriv->xmit_sema);
	#endif

	_rtw_spinlock_free(&pxmitpriv->be_pending.lock);
	_rtw_spinlock_free(&pxmitpriv->bk_pending.lock);
	_rtw_spinlock_free(&pxmitpriv->vi_pending.lock);
	_rtw_spinlock_free(&pxmitpriv->vo_pending.lock);
	_rtw_spinlock_free(&pxmitpriv->bm_pending.lock);

	/* _rtw_spinlock_free(&pxmitpriv->legacy_dz_queue.lock); */
	/* _rtw_spinlock_free(&pxmitpriv->apsd_queue.lock); */

	_rtw_spinlock_free(&pxmitpriv->free_xmit_queue.lock);
	#if 0 /*CONFIG_CORE_XMITBUF*/
	_rtw_spinlock_free(&pxmitpriv->free_xmitbuf_queue.lock);
	_rtw_spinlock_free(&pxmitpriv->pending_xmitbuf_queue.lock);
	#endif
}


void _rtw_free_xmit_priv(struct xmit_priv *pxmitpriv)
{
	int i;
	_adapter *padapter = pxmitpriv->adapter;
	struct xmit_frame *pxmitframe;
	#if 0 /*CONFIG_CORE_XMITBUF*/
	struct xmit_buf *pxmitbuf = (struct xmit_buf *)pxmitpriv->pxmitbuf;
	#endif

	#if 0 /*CONFIG_CORE_XMITBUF*/
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	u8 xmitbuf_nr = GET_HAL_XMITBUF_NR(dvobj);
	u16 xmitbuf_sz = GET_HAL_XMITBUF_SZ(dvobj);

	u8 xmitbuf_ext_nr = GET_HAL_XMITBUF_EXT_NR(dvobj);
	u16 xmitbuf_ext_sz = GET_HAL_XMITBUF_EXT_SZ(dvobj);
	#endif

	rtw_intf_free_xmit_priv(padapter);

	rtw_mfree_xmit_priv_lock(pxmitpriv);

	if (pxmitpriv->pxmit_frame_buf == NULL)
		goto out;

	pxmitframe = (struct xmit_frame *) pxmitpriv->pxmit_frame_buf;

	for (i = 0; i < NR_XMITFRAME; i++) {
		rtw_os_xmit_complete(padapter, pxmitframe);
		/*alloc buf_addr*/
		/*rtw_os_xmit_resource_free(padapter, pxmitframe);*/
		pxmitframe++;
	}

	#if 0 /*CONFIG_CORE_XMITBUF*/
	for (i = 0; i < xmitbuf_nr; i++) {
		rtw_os_xmit_resource_free(padapter, pxmitbuf,
			(xmitbuf_sz + SZ_ALIGN_XMITFRAME_EXT), _TRUE);

		pxmitbuf++;
	}
	#endif
	if (pxmitpriv->pallocated_frame_buf)
		rtw_vmfree(pxmitpriv->pallocated_frame_buf,
			NR_XMITFRAME * sizeof(struct xmit_frame) + 4);

	#if 0 /*CONFIG_CORE_XMITBUF*/
	if (pxmitpriv->pallocated_xmitbuf)
		rtw_vmfree(pxmitpriv->pallocated_xmitbuf,
			xmitbuf_nr * sizeof(struct xmit_buf) + 4);
	#endif

	/* free xframe_ext queue,  the same count as extbuf */
	if (pxmitpriv->xframe_ext == NULL)
		goto out;

	pxmitframe = (struct xmit_frame *)pxmitpriv->xframe_ext;
	for (i = 0; i < NR_XMITFRAME_EXT; i++) {
		rtw_os_xmit_complete(padapter, pxmitframe);
		/*free buf_addr*/
		rtw_os_xmit_resource_free(padapter, pxmitframe);
		pxmitframe++;
	}

	if (pxmitpriv->xframe_ext_alloc_addr)
		rtw_vmfree(pxmitpriv->xframe_ext_alloc_addr,
			NR_XMITFRAME_EXT * sizeof(struct xmit_frame) + 4);
	_rtw_spinlock_free(&pxmitpriv->free_xframe_ext_queue.lock);

	if (pxmitpriv->xframe_ext_txreq_alloc_addr)
		rtw_mfree(pxmitpriv->xframe_ext_txreq_alloc_addr, NR_XMITFRAME_EXT * SZ_MGT_RING);

#if 0 /*CONFIG_CORE_XMITBUF*/

	/* free xmit extension buff */
	_rtw_spinlock_free(&pxmitpriv->free_xmit_extbuf_queue.lock);

	pxmitbuf = (struct xmit_buf *)pxmitpriv->pxmit_extbuf;
	for (i = 0; i < xmitbuf_ext_nr; i++) {
		rtw_os_xmit_resource_free(padapter, pxmitbuf,
			(xmitbuf_ext_sz + SZ_ALIGN_XMITFRAME_EXT), _TRUE);

		pxmitbuf++;
	}

	if (pxmitpriv->pallocated_xmit_extbuf)
		rtw_vmfree(pxmitpriv->pallocated_xmit_extbuf,
			xmitbuf_ext_nr * sizeof(struct xmit_buf) + 4);

	for (i = 0; i < CMDBUF_MAX; i++) {
		pxmitbuf = &pxmitpriv->pcmd_xmitbuf[i];
		if (pxmitbuf != NULL)
			rtw_os_xmit_resource_free(padapter, pxmitbuf, MAX_CMDBUF_SZ + SZ_ALIGN_XMITFRAME_EXT, _TRUE);
	}
	#endif
	rtw_free_hwxmits(padapter);

#ifdef CONFIG_LAYER2_ROAMING
	_rtw_spinlock_free(&pxmitpriv->rpkt_queue.lock);
#endif

#ifdef RTW_PHL_TX
	free_txring(padapter);
#endif

#ifdef CONFIG_CORE_TXSC
	txsc_clear(padapter);
	_rtw_spinlock_free(&pxmitpriv->txsc_lock);
#endif

#ifdef CONFIG_XMIT_ACK
	_rtw_mutex_free(&pxmitpriv->ack_tx_mutex);
#endif
	rtw_free_xmit_block(padapter);
out:
	return;
}

#ifdef CONFIG_USB_HCI
u8 rtw_init_lite_xmit_resource(struct dvobj_priv *dvobj)
{

	u8 ret = _SUCCESS;
/*YiWei_todo need use correct litexmitbuf_nr urb_nr*/
	u32 litexmitbuf_nr = RTW_LITEXMITBUF_NR;
	u32 litexmitbuf_ext_nr = RTW_LITEXMITBUF_NR;
	struct lite_data_buf *litexmitbuf;
	struct trx_data_buf_q  *litexmitbuf_q = &dvobj->litexmitbuf_q;
	struct trx_data_buf_q  *litexmit_extbuf_q = &dvobj->litexmit_extbuf_q;
	int i;
	struct trx_urb_buf_q *xmit_urb_q = &dvobj->xmit_urb_q;
	struct data_urb *xmiturb;
	u32 urb_nr = RTW_XMITURB_NR;

	/* init lite_xmit_buf */
	_rtw_init_queue(&litexmitbuf_q->free_data_buf_queue);

	litexmitbuf_q->alloc_data_buf =
		rtw_zvmalloc(litexmitbuf_nr * sizeof(struct lite_data_buf) + 4);

	if (litexmitbuf_q->alloc_data_buf  == NULL) {
		ret = _FAIL;
		goto exit;
	}

	litexmitbuf_q->data_buf =
		(u8 *)N_BYTE_ALIGNMENT((SIZE_PTR)(litexmitbuf_q->alloc_data_buf), 4);

	litexmitbuf = (struct lite_data_buf *)litexmitbuf_q->data_buf;

	for (i = 0; i < litexmitbuf_nr; i++) {
		_rtw_init_listhead(&litexmitbuf->list);
		rtw_list_insert_tail(&litexmitbuf->list,
			&(litexmitbuf_q->free_data_buf_queue.queue));
		litexmitbuf++;
	}
	litexmitbuf_q->free_data_buf_cnt = litexmitbuf_nr;


	/* Init lite xmit extension buff */
	_rtw_init_queue(&litexmit_extbuf_q->free_data_buf_queue);

	litexmit_extbuf_q->alloc_data_buf =
		rtw_zvmalloc(litexmitbuf_ext_nr * sizeof(struct lite_data_buf) + 4);

	if (litexmit_extbuf_q->alloc_data_buf  == NULL) {
		ret = _FAIL;
		goto exit;
	}

	litexmit_extbuf_q->data_buf =
		(u8 *)N_BYTE_ALIGNMENT((SIZE_PTR)(litexmit_extbuf_q->alloc_data_buf), 4);

	litexmitbuf = (struct lite_data_buf *)litexmit_extbuf_q->data_buf;

	for (i = 0; i < litexmitbuf_ext_nr; i++) {
		_rtw_init_listhead(&litexmitbuf->list);
		rtw_list_insert_tail(&litexmitbuf->list,
			&(litexmit_extbuf_q->free_data_buf_queue.queue));
		litexmitbuf++;
	}
	litexmit_extbuf_q->free_data_buf_cnt = litexmitbuf_ext_nr;

	/* init xmit_urb */
	_rtw_init_queue(&xmit_urb_q->free_urb_buf_queue);
	xmit_urb_q->alloc_urb_buf =
		rtw_zvmalloc(urb_nr * sizeof(struct data_urb) + 4);
	if (xmit_urb_q->alloc_urb_buf == NULL) {
		ret = _FAIL;
		goto exit;
	}

	xmit_urb_q->urb_buf =
		(u8 *)N_BYTE_ALIGNMENT((SIZE_PTR)(xmit_urb_q->alloc_urb_buf), 4);

	xmiturb = (struct data_urb *)xmit_urb_q->urb_buf;
	for (i = 0; i < urb_nr; i++) {
		_rtw_init_listhead(&xmiturb->list);
		ret = rtw_os_urb_resource_alloc(xmiturb);
		rtw_list_insert_tail(&xmiturb->list,
			&(xmit_urb_q->free_urb_buf_queue.queue));
		xmiturb++;
	}
	xmit_urb_q->free_urb_buf_cnt = urb_nr;

exit:
	return ret;
}

void rtw_free_lite_xmit_resource(struct dvobj_priv *dvobj)
{
	u8 ret = _SUCCESS;
/*YiWei_todo need use correct litexmitbuf_nr urb_nr*/
	u32 litexmitbuf_nr = RTW_LITEXMITBUF_NR;
	u32 litexmitbuf_ext_nr = RTW_LITEXMITBUF_NR;
	struct trx_data_buf_q  *litexmitbuf_q = &dvobj->litexmitbuf_q;
	struct trx_data_buf_q  *litexmit_extbuf_q = &dvobj->litexmit_extbuf_q;
	struct data_urb *xmiturb;
	struct trx_urb_buf_q *xmit_urb_q = &dvobj->xmit_urb_q;
	u32 urb_nr = RTW_XMITURB_NR;
	int i;

	if (litexmitbuf_q->alloc_data_buf)
		rtw_vmfree(litexmitbuf_q->alloc_data_buf,
			litexmitbuf_nr * sizeof(struct lite_data_buf) + 4);

	if (litexmit_extbuf_q->alloc_data_buf)
		rtw_vmfree(litexmit_extbuf_q->alloc_data_buf,
			litexmitbuf_ext_nr * sizeof(struct lite_data_buf) + 4);

	xmiturb = (struct data_urb *)xmit_urb_q->urb_buf;
	for (i = 0; i < urb_nr; i++) {
		rtw_os_urb_resource_free(xmiturb);
		xmiturb++;
	}

	if (xmit_urb_q->alloc_urb_buf)
		rtw_vmfree(xmit_urb_q->alloc_urb_buf,
			urb_nr * sizeof(struct data_urb) + 4);
}
#endif

u8 rtw_get_tx_bw_mode(_adapter *adapter, struct sta_info *sta)
{
	u8 bw;
	struct _ADAPTER_LINK *adapter_link = sta->padapter_link;

	bw = sta->phl_sta->chandef.bw;
	if (MLME_STATE(adapter) & WIFI_ASOC_STATE) {
		switch (adapter_link->mlmeextpriv.chandef.band) {
		case BAND_ON_6G:
			bw = rtw_min(bw, ADAPTER_TX_BW_6G(adapter));
			break;
		case BAND_ON_5G:
			bw = rtw_min(bw, ADAPTER_TX_BW_5G(adapter));
			break;
		case BAND_ON_24G:
		default:
			bw = rtw_min(bw, ADAPTER_TX_BW_2G(adapter));
			break;
		}
	}

	return bw;
}

void rtw_get_adapter_tx_rate_bmp_by_bw(_adapter *adapter, u8 bw, u16 *r_bmp_cck_ofdm, u32 *r_bmp_ht, u64 *r_bmp_vht)
{
/* ToDo */
#if 0
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	struct macid_ctl_t *macid_ctl = dvobj_to_macidctl(dvobj);
	u8 fix_bw = 0xFF;
	u16 bmp_cck_ofdm = 0;
	u32 bmp_ht = 0;
	u64 bmp_vht = 0;
	int i;

	if (adapter->fix_rate != NO_FIX_RATE && adapter->fix_bw != NO_FIX_BW)
		fix_bw = adapter->fix_bw;

	/* TODO: adapter->fix_rate */

	for (i = 0; i < macid_ctl->num; i++) {
		if (!rtw_macid_is_used(macid_ctl, i))
			continue;
		if (!rtw_macid_is_iface_specific(macid_ctl, i, adapter))
			continue;

		if (bw == CHANNEL_WIDTH_20) /* CCK, OFDM always 20MHz */
			bmp_cck_ofdm |= macid_ctl->rate_bmp0[i] & 0x00000FFF;

		/* bypass mismatch bandwidth for HT, VHT */
		if ((fix_bw != 0xFF && fix_bw != bw) || (fix_bw == 0xFF && macid_ctl->bw[i] != bw))
			continue;

		if (macid_ctl->vht_en[i])
			bmp_vht |= (macid_ctl->rate_bmp0[i] >> 12) | (macid_ctl->rate_bmp1[i] << 20);
		else
			bmp_ht |= (macid_ctl->rate_bmp0[i] >> 12) | (macid_ctl->rate_bmp1[i] << 20);
	}

	/* TODO: mlmeext->tx_rate*/

	if (r_bmp_cck_ofdm)
		*r_bmp_cck_ofdm = bmp_cck_ofdm;
	if (r_bmp_ht)
		*r_bmp_ht = bmp_ht;
	if (r_bmp_vht)
		*r_bmp_vht = bmp_vht;
#endif
}

void rtw_get_shared_macid_tx_rate_bmp_by_bw(struct dvobj_priv *dvobj, u8 bw, u16 *r_bmp_cck_ofdm, u32 *r_bmp_ht, u64 *r_bmp_vht)
{
/* ToDo */
#if 0
	struct macid_ctl_t *macid_ctl = dvobj_to_macidctl(dvobj);
	u16 bmp_cck_ofdm = 0;
	u32 bmp_ht = 0;
	u64 bmp_vht = 0;
	int i;

	for (i = 0; i < macid_ctl->num; i++) {
		if (!rtw_macid_is_used(macid_ctl, i))
			continue;
		if (!rtw_macid_is_iface_shared(macid_ctl, i))
			continue;

		if (bw == CHANNEL_WIDTH_20) /* CCK, OFDM always 20MHz */
			bmp_cck_ofdm |= macid_ctl->rate_bmp0[i] & 0x00000FFF;

		/* bypass mismatch bandwidth for HT, VHT */
		if (macid_ctl->bw[i] != bw)
			continue;

		if (macid_ctl->vht_en[i])
			bmp_vht |= (macid_ctl->rate_bmp0[i] >> 12) | (macid_ctl->rate_bmp1[i] << 20);
		else
			bmp_ht |= (macid_ctl->rate_bmp0[i] >> 12) | (macid_ctl->rate_bmp1[i] << 20);
	}

	if (r_bmp_cck_ofdm)
		*r_bmp_cck_ofdm = bmp_cck_ofdm;
	if (r_bmp_ht)
		*r_bmp_ht = bmp_ht;
	if (r_bmp_vht)
		*r_bmp_vht = bmp_vht;
#endif
}

void rtw_update_tx_rate_bmp(struct dvobj_priv *dvobj)
{
#if 0 /*GEORGIA_TODO_FIXIT*/

	struct rf_ctl_t *rf_ctl = dvobj_to_rfctl(dvobj);
	_adapter *adapter = dvobj_get_primary_adapter(dvobj);
	HAL_DATA_TYPE *hal_data = GET_PHL_COM(dvobj);
	u8 bw;
	u16 bmp_cck_ofdm, tmp_cck_ofdm;
	u32 bmp_ht, tmp_ht, ori_bmp_ht[2];
	u64 bmp_vht, tmp_vht, ori_bmp_vht[4];
	int i;

	for (bw = CHANNEL_WIDTH_20; bw <= CHANNEL_WIDTH_160; bw++) {
		/* backup the original ht & vht bmp */
		if (bw <= CHANNEL_WIDTH_40)
			ori_bmp_ht[bw] = rf_ctl->rate_bmp_ht_by_bw[bw];
		if (bw <= CHANNEL_WIDTH_160)
			ori_bmp_vht[bw] = rf_ctl->rate_bmp_vht_by_bw[bw];

		bmp_cck_ofdm = bmp_ht = bmp_vht = 0;
		if (rtw_hw_is_bw_support(dvobj, bw)) {
			for (i = 0; i < dvobj->iface_nums; i++) {
				if (!dvobj->padapters[i])
					continue;
				rtw_get_adapter_tx_rate_bmp_by_bw(dvobj->padapters[i], bw, &tmp_cck_ofdm, &tmp_ht, &tmp_vht);
				bmp_cck_ofdm |= tmp_cck_ofdm;
				bmp_ht |= tmp_ht;
				bmp_vht |= tmp_vht;
			}
			rtw_get_shared_macid_tx_rate_bmp_by_bw(dvobj, bw, &tmp_cck_ofdm, &tmp_ht, &tmp_vht);
			bmp_cck_ofdm |= tmp_cck_ofdm;
			bmp_ht |= tmp_ht;
			bmp_vht |= tmp_vht;
		}
		if (bw == CHANNEL_WIDTH_20)
			rf_ctl->rate_bmp_cck_ofdm = bmp_cck_ofdm;
		if (bw <= CHANNEL_WIDTH_40)
			rf_ctl->rate_bmp_ht_by_bw[bw] = bmp_ht;
		if (bw <= CHANNEL_WIDTH_160)
			rf_ctl->rate_bmp_vht_by_bw[bw] = bmp_vht;
	}

#if CONFIG_TXPWR_LIMIT
#ifndef DBG_HIGHEST_RATE_BMP_BW_CHANGE
#define DBG_HIGHEST_RATE_BMP_BW_CHANGE 0
#endif

	if (hal_data->txpwr_limit_loaded) {
		u8 ori_highest_ht_rate_bw_bmp;
		u8 ori_highest_vht_rate_bw_bmp;
		u8 highest_rate_bw;
		u8 highest_rate_bw_bmp;
		u8 update_ht_rs = _FALSE;
		u8 update_vht_rs = _FALSE;

		/* backup the original ht & vht highest bw bmp */
		ori_highest_ht_rate_bw_bmp = rf_ctl->highest_ht_rate_bw_bmp;
		ori_highest_vht_rate_bw_bmp = rf_ctl->highest_vht_rate_bw_bmp;

		highest_rate_bw_bmp = BW_CAP_20M;
		highest_rate_bw = CHANNEL_WIDTH_20;
		for (bw = CHANNEL_WIDTH_20; bw <= CHANNEL_WIDTH_40; bw++) {
			if (rf_ctl->rate_bmp_ht_by_bw[highest_rate_bw] < rf_ctl->rate_bmp_ht_by_bw[bw]) {
				highest_rate_bw_bmp = ch_width_to_bw_cap(bw);
				highest_rate_bw = bw;
			} else if (rf_ctl->rate_bmp_ht_by_bw[highest_rate_bw] == rf_ctl->rate_bmp_ht_by_bw[bw])
				highest_rate_bw_bmp |= ch_width_to_bw_cap(bw);
		}
		rf_ctl->highest_ht_rate_bw_bmp = highest_rate_bw_bmp;

		if (ori_highest_ht_rate_bw_bmp != rf_ctl->highest_ht_rate_bw_bmp
			|| largest_bit(ori_bmp_ht[highest_rate_bw]) != largest_bit(rf_ctl->rate_bmp_ht_by_bw[highest_rate_bw])
		) {
			if (DBG_HIGHEST_RATE_BMP_BW_CHANGE) {
				RTW_INFO("highest_ht_rate_bw_bmp:0x%02x=>0x%02x\n", ori_highest_ht_rate_bw_bmp, rf_ctl->highest_ht_rate_bw_bmp);
				RTW_INFO("rate_bmp_ht_by_bw[%u]:0x%08x=>0x%08x\n", highest_rate_bw, ori_bmp_ht[highest_rate_bw], rf_ctl->rate_bmp_ht_by_bw[highest_rate_bw]);
			}
			if (rf_ctl->rate_bmp_ht_by_bw[highest_rate_bw])
				update_ht_rs = _TRUE;
		}

		highest_rate_bw_bmp = BW_CAP_20M;
		highest_rate_bw = CHANNEL_WIDTH_20;
		for (bw = CHANNEL_WIDTH_20; bw <= CHANNEL_WIDTH_160; bw++) {
			if (rf_ctl->rate_bmp_vht_by_bw[highest_rate_bw] < rf_ctl->rate_bmp_vht_by_bw[bw]) {
				highest_rate_bw_bmp = ch_width_to_bw_cap(bw);
				highest_rate_bw = bw;
			} else if (rf_ctl->rate_bmp_vht_by_bw[highest_rate_bw] == rf_ctl->rate_bmp_vht_by_bw[bw])
				highest_rate_bw_bmp |= ch_width_to_bw_cap(bw);
		}
		rf_ctl->highest_vht_rate_bw_bmp = highest_rate_bw_bmp;

		if (ori_highest_vht_rate_bw_bmp != rf_ctl->highest_vht_rate_bw_bmp
			|| largest_bit_64(ori_bmp_vht[highest_rate_bw]) != largest_bit_64(rf_ctl->rate_bmp_vht_by_bw[highest_rate_bw])
		) {
			if (DBG_HIGHEST_RATE_BMP_BW_CHANGE) {
				RTW_INFO("highest_vht_rate_bw_bmp:0x%02x=>0x%02x\n", ori_highest_vht_rate_bw_bmp, rf_ctl->highest_vht_rate_bw_bmp);
				RTW_INFO("rate_bmp_vht_by_bw[%u]:0x%016llx=>0x%016llx\n", highest_rate_bw, ori_bmp_vht[highest_rate_bw], rf_ctl->rate_bmp_vht_by_bw[highest_rate_bw]);
			}
			if (rf_ctl->rate_bmp_vht_by_bw[highest_rate_bw])
				update_vht_rs = _TRUE;
		}

		/* TODO: per rfpath and rate section handling? */
		if (update_ht_rs == _TRUE || update_vht_rs == _TRUE)
			rtw_hal_set_tx_power_level(dvobj_get_primary_adapter(dvobj), hal_data->current_channel);
	}
#endif /* CONFIG_TXPWR_LIMIT */
#endif
}

u8 rtw_get_tx_bw_bmp_of_ht_rate(struct dvobj_priv *dvobj, u8 rate, u8 max_bw)
{
	struct rf_ctl_t *rf_ctl = dvobj_to_rfctl(dvobj);
	u8 bw;
	u8 bw_bmp = 0;
	u32 rate_bmp;

	if (!IS_HT_RATE(rate)) {
		rtw_warn_on(1);
		goto exit;
	}

	rate_bmp = 1 << (rate - MGN_MCS0);

	if (max_bw > CHANNEL_WIDTH_40)
		max_bw = CHANNEL_WIDTH_40;

	for (bw = CHANNEL_WIDTH_20; bw <= max_bw; bw++) {
		/* RA may use lower rate for retry */
		if (rf_ctl->rate_bmp_ht_by_bw[bw] >= rate_bmp)
			bw_bmp |= ch_width_to_bw_cap(bw);
	}

exit:
	return bw_bmp;
}

u8 rtw_get_tx_bw_bmp_of_vht_rate(struct dvobj_priv *dvobj, u8 rate, u8 max_bw)
{
	struct rf_ctl_t *rf_ctl = dvobj_to_rfctl(dvobj);
	u8 bw;
	u8 bw_bmp = 0;
	u64 rate_bmp;

	if (!IS_VHT_RATE(rate)) {
		rtw_warn_on(1);
		goto exit;
	}

	rate_bmp = BIT_ULL(rate - MGN_VHT1SS_MCS0);

	if (max_bw > CHANNEL_WIDTH_160)
		max_bw = CHANNEL_WIDTH_160;

	for (bw = CHANNEL_WIDTH_20; bw <= max_bw; bw++) {
		/* RA may use lower rate for retry */
		if (rf_ctl->rate_bmp_vht_by_bw[bw] >= rate_bmp)
			bw_bmp |= ch_width_to_bw_cap(bw);
	}

exit:
	return bw_bmp;
}

s16 rtw_rfctl_get_oper_txpwr_max_mbm(struct rf_ctl_t *rfctl, u8 ch, u8 bw, u8 offset, u8 ifbmp_mod, u8 if_op, bool eirp)
{
	/* TODO: get maximum txpower of current operating class & channel belongs to this radio */
	s16 mbm = 2000;
	return mbm;
}

s16 rtw_rfctl_get_reg_max_txpwr_mbm(struct rf_ctl_t *rfctl, enum band_type band, u8 ch, u8 bw, u8 offset, bool eirp)
{
	/* TODO: get maximum txpower of current operating class & channel belongs to this radio allowed by regulatory */
	s16 mbm = 1300;
	return mbm;
}

u8 query_ra_short_GI(struct sta_info *psta, u8 bw)
{
	u8	sgi = _FALSE, sgi_20m = _FALSE, sgi_40m = _FALSE;
	u8	sgi_80m = _FALSE, sgi_160m = _FALSE;

#ifdef CONFIG_80211N_HT
#ifdef CONFIG_80211AC_VHT
#ifdef CONFIG_80211AX_HE
	/* CONFIG_80211AX_HE_TODO */
#endif /* CONFIG_80211AX_HE */
	if (psta->vhtpriv.vht_option) {
		sgi_80m = psta->vhtpriv.sgi_80m;
		sgi_160m = psta->vhtpriv.sgi_160m;
	}
#endif
	sgi_20m = psta->htpriv.sgi_20m;
	sgi_40m = psta->htpriv.sgi_40m;
#endif

	switch (bw) {
	case CHANNEL_WIDTH_160:
		sgi = sgi_160m;
		break;
	case CHANNEL_WIDTH_80:
		sgi = sgi_80m;
		break;
	case CHANNEL_WIDTH_40:
		sgi = sgi_40m;
		break;
	case CHANNEL_WIDTH_20:
	default:
		sgi = sgi_20m;
		break;
	}

	return sgi;
}

static void update_attrib_vcs_info(_adapter *padapter, struct xmit_frame *pxmitframe)
{
	u32	sz;
	struct pkt_attrib	*pattrib = &pxmitframe->attrib;
	/* struct sta_info	*psta = pattrib->psta; */
	struct _ADAPTER_LINK		*padapter_link = pattrib->adapter_link;
	struct link_mlme_ext_priv	*pmlmeext = &(padapter_link->mlmeextpriv);
	struct link_mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);

	/*
		if(pattrib->psta)
		{
			psta = pattrib->psta;
		}
		else
		{
			RTW_INFO("%s, call rtw_get_stainfo()\n", __func__);
			psta=rtw_get_stainfo(&padapter->stapriv ,&pattrib->ra[0] );
		}

		if(psta==NULL)
		{
			RTW_INFO("%s, psta==NUL\n", __func__);
			return;
		}

		if(!(psta->state &WIFI_ASOC_STATE))
		{
			RTW_INFO("%s, psta->state(0x%x) != WIFI_ASOC_STATE\n", __func__, psta->state);
			return;
		}
	*/

	if (pattrib->nr_frags != 1)
		sz = padapter->xmitpriv.frag_len;
	else /* no frag */
		sz = pattrib->last_txcmdsz;

	/* (1) RTS_Threshold is compared to the MPDU, not MSDU. */
	/* (2) If there are more than one frag in  this MSDU, only the first frag uses protection frame. */
	/*		Other fragments are protected by previous fragment. */
	/*		So we only need to check the length of first fragment. */
	if (pmlmeext->cur_wireless_mode < WLAN_MD_11N  || padapter->registrypriv.wifi_spec) {
		if (sz > padapter->registrypriv.rts_thresh)
			pattrib->vcs_mode = RTS_CTS;
		else {
			if (pattrib->rtsen)
				pattrib->vcs_mode = RTS_CTS;
			else if (pattrib->cts2self)
				pattrib->vcs_mode = CTS_TO_SELF;
			else
				pattrib->vcs_mode = NONE_VCS;
		}
	} else {
		while (_TRUE) {
#if 0 /* Todo */
			/* check IOT action */
			if (pHTInfo->IOTAction & HT_IOT_ACT_FORCED_CTS2SELF) {
				pattrib->vcs_mode = CTS_TO_SELF;
				pattrib->rts_rate = MGN_24M;
				break;
			} else if (pHTInfo->IOTAction & (HT_IOT_ACT_FORCED_RTS | HT_IOT_ACT_PURE_N_MODE)) {
				pattrib->vcs_mode = RTS_CTS;
				pattrib->rts_rate = MGN_24M;
				break;
			}
#endif

			/* IOT action */
			if ((padapter->mlmeextpriv.mlmext_info.assoc_AP_vendor == HT_IOT_PEER_ATHEROS) && (pattrib->ampdu_en == _TRUE) &&
				(padapter->securitypriv.dot11PrivacyAlgrthm == _AES_)) {
				pattrib->vcs_mode = CTS_TO_SELF;
				break;
			}


			/* check ERP protection */
			if (pattrib->rtsen || pattrib->cts2self) {
				if (pattrib->rtsen)
					pattrib->vcs_mode = RTS_CTS;
				else if (pattrib->cts2self)
					pattrib->vcs_mode = CTS_TO_SELF;

				break;
			}

			/* check HT op mode */
			if (pattrib->ht_en) {
				u8 HTOpMode = pmlmeinfo->HT_protection;
				if ((pmlmeext->chandef.bw && (HTOpMode == 2 || HTOpMode == 3)) ||
				    (!pmlmeext->chandef.bw && HTOpMode == 3)) {
					pattrib->vcs_mode = RTS_CTS;
					break;
				}
			}

			/* check rts */
			if (sz > padapter->registrypriv.rts_thresh) {
				pattrib->vcs_mode = RTS_CTS;
				break;
			}

			/* to do list: check MIMO power save condition. */

			/* check AMPDU aggregation for TXOP */
			if (pattrib->ampdu_en == _TRUE) {
				pattrib->vcs_mode = RTS_CTS;
				break;
			}

			pattrib->vcs_mode = NONE_VCS;
			break;
		}
	}

	/* for debug : force driver control vrtl_carrier_sense. */
	if (padapter->driver_vcs_en == 1) {
		/* u8 driver_vcs_en; */ /* Enable=1, Disable=0 driver control vrtl_carrier_sense. */
		/* u8 driver_vcs_type; */ /* force 0:disable VCS, 1:RTS-CTS, 2:CTS-to-self when vcs_en=1. */
		pattrib->vcs_mode = padapter->driver_vcs_type;
	}

}

static void update_attrib_phy_info(_adapter *padapter, struct pkt_attrib *pattrib, struct sta_info *psta)
{
	struct registry_priv *pregistrypriv = &padapter->registrypriv;
	struct link_mlme_ext_priv *mlmeext = &pattrib->adapter_link->mlmeextpriv;
	u8 bw;

	pattrib->rtsen = psta->rtsen;
	pattrib->cts2self = psta->cts2self;
	pattrib->hw_rts_en = psta->hw_rts_en;

	pattrib->mdata = 0;
	pattrib->eosp = 0;
#ifdef CONFIG_80211AX_HE
	if (psta->hepriv.he_option == _TRUE)
		pattrib->eosp = 1;
#endif
	pattrib->triggered = 0;
	pattrib->ampdu_spacing = 0;

	/* ht_en, init rate, ,bw, ch_offset, sgi */

	/* ToDo: Need API to inform hal_sta->ra_info.rate_id */
	/* pattrib->raid = psta->phl_sta->ra_info.rate_id; */

	bw = rtw_get_tx_bw_mode(padapter, psta);
	pattrib->bwmode = rtw_min(bw, mlmeext->chandef.bw);
	pattrib->sgi = query_ra_short_GI(psta, pattrib->bwmode);

	if (psta->phl_sta->wmode & WLAN_MD_11AX) {
		pattrib->ldpc = psta->phl_sta->asoc_cap.he_ldpc;
		pattrib->stbc = (psta->phl_sta->asoc_cap.stbc_he_rx > 0) ? 1:0;
	} else if (psta->phl_sta->wmode & WLAN_MD_11AC) {
		pattrib->ldpc = psta->phl_sta->asoc_cap.vht_ldpc;
		pattrib->stbc = (psta->phl_sta->asoc_cap.stbc_vht_rx > 0) ? 1:0;
	} else if (psta->phl_sta->wmode & WLAN_MD_11N) {
		pattrib->ldpc = psta->phl_sta->asoc_cap.ht_ldpc;
		pattrib->stbc = (psta->phl_sta->asoc_cap.stbc_ht_rx > 0) ? 1:0;
	} else {
		pattrib->ldpc = 0;
		pattrib->stbc = 0;
	}

#ifdef CONFIG_80211N_HT
	if (pregistrypriv->ht_enable &&
		is_supported_ht(pregistrypriv->wireless_mode)) {
		pattrib->ht_en = psta->htpriv.ht_option;
		pattrib->ch_offset = psta->htpriv.ch_offset;
		pattrib->ampdu_en = _FALSE;
		if (padapter->driver_ampdu_spacing != 0xFF) /* driver control AMPDU Density for peer sta's rx */
			pattrib->ampdu_spacing = padapter->driver_ampdu_spacing;
		else
			pattrib->ampdu_spacing = psta->ampdu_priv.rx_ampdu_min_spacing;

		/* check if enable ampdu */
		if ((pattrib->ht_en
#if CONFIG_IEEE80211_BAND_6GHZ
		    || psta->phl_sta->chandef.band == BAND_ON_6G
#endif
		    ) && psta->ampdu_priv.ampdu_enable) {
			if (psta->ampdu_priv.agg_enable_bitmap & BIT(pattrib->priority)) {
				pattrib->ampdu_en = _TRUE;
				if (psta->ampdu_priv.tx_amsdu_enable == _TRUE)
					pattrib->amsdu_ampdu_en = _TRUE;
				else
					pattrib->amsdu_ampdu_en = _FALSE;
			}
		}
	}
#endif /* CONFIG_80211N_HT */
	/* if(pattrib->ht_en && psta->ampdu_priv.ampdu_enable) */
	/* { */
	/*	if(psta->ampdu_priv.agg_enable_bitmap & BIT(pattrib->priority)) */
	/*		pattrib->ampdu_en = _TRUE; */
	/* }	 */

#ifdef CONFIG_TDLS
	if (pattrib->direct_link == _TRUE) {
		psta = pattrib->ptdls_sta;

		/* pattrib->raid = psta->phl_sta->ra_info.rate_id; */
#ifdef CONFIG_80211N_HT
		if (pregistrypriv->ht_enable &&
			is_supported_ht(pregistrypriv->wireless_mode)) {
				pattrib->bwmode = rtw_get_tx_bw_mode(padapter, psta);
				pattrib->ht_en = psta->htpriv.ht_option;
				pattrib->ch_offset = psta->htpriv.ch_offset;
				pattrib->sgi = query_ra_short_GI(psta, pattrib->bwmode);
		}
#endif /* CONFIG_80211N_HT */
	}
#endif /* CONFIG_TDLS */

	pattrib->retry_ctrl = _FALSE;
}

static s32 update_attrib_sec_iv_info(_adapter *padapter, struct pkt_attrib *pattrib)
{
	struct sta_info *psta = pattrib->psta;
	sint bmcast = IS_MCAST(pattrib->ra);

	if (!psta)
		return _FAIL;

	switch (pattrib->encrypt) {
	case _WEP40_:
	case _WEP104_:
		WEP_IV(pattrib->iv, psta->dot11txpn, pattrib->key_idx);
		break;

	case _TKIP_:
		if (bmcast)
			TKIP_IV(pattrib->iv, psta->dot11txpn, pattrib->key_idx);
		else
			TKIP_IV(pattrib->iv, psta->dot11txpn, 0);
		break;

	case _AES_:
		if (bmcast)
			AES_IV(pattrib->iv, psta->dot11txpn, pattrib->key_idx);
		else
			AES_IV(pattrib->iv, psta->dot11txpn, 0);

		break;

	case _GCMP_:
	case _GCMP_256_:
		if (bmcast)
			GCMP_IV(pattrib->iv, psta->dot11txpn, pattrib->key_idx);
		else
			GCMP_IV(pattrib->iv, psta->dot11txpn, 0);

		break;

	case _CCMP_256_:
		if (bmcast)
			GCMP_IV(pattrib->iv, psta->dot11txpn, pattrib->key_idx);
		else
			GCMP_IV(pattrib->iv, psta->dot11txpn, 0);

		break;

#ifdef CONFIG_WAPI_SUPPORT
	case _SMS4_:
		rtw_wapi_get_iv(padapter, pattrib->ra, pattrib->iv);
		break;
#endif
	default:
		break;
	}

	return _SUCCESS;
}

static s32 update_attrib_sec_info(_adapter *padapter, struct pkt_attrib *pattrib, struct sta_info *psta, enum eap_type eapol_type)
{
	sint res = _SUCCESS;
	struct mlme_priv	*pmlmepriv = &padapter->mlmepriv;
	struct security_priv *psecuritypriv = &padapter->securitypriv;
	sint bmcast = IS_MCAST(pattrib->ra);
	s8 hw_decrypted = _FALSE;
	struct _ADAPTER_LINK *padapter_link = psta->padapter_link;
	struct link_security_priv *lsecuritypriv = &padapter_link->securitypriv;
	u8 hw_security_zero_hdrlen = _FALSE;

	_rtw_memset(pattrib->dot118021x_UncstKey.skey, 0, 16);
	_rtw_memset(pattrib->dot11tkiptxmickey.skey, 0, 16);
	pattrib->mac_id = psta->phl_sta->macid;

	/* Comment by Owen at 2020/05/19
	 * Issue: RTK STA sends encrypted 4-way 4/4 when AP thinks the 4-way incomplete
	 * In TCL pressure test, AP may resend 4-way 3/4 with new replay counter in 2 ms.
	 * In this situation, STA sends unencrypted 4-way 4/4 with old replay counter after more
	 * than 2 ms, followed by the encrypted 4-way 4/4 with new replay counter. Because the
	 * AP only accepts unencrypted 4-way 4/4 with a new play counter, and the STA encrypts
	 * each 4-way 4/4 at this time, the 4-way handshake cannot be completed.
	 * So we modified that after STA receives unencrypted 4-way 1/4 and 4-way 3/4,
	 * 4-way 2/4 and 4-way 4/4 sent by STA in the next 100 ms are not encrypted.
	 */
	if (psta->ieee8021x_blocked == _TRUE ||
		((eapol_type == EAPOL_2_4 || eapol_type == EAPOL_4_4) &&
		rtw_get_passing_time_ms(psta->resp_nonenc_eapol_key_starttime) <= 100)) {

		if (eapol_type == EAPOL_2_4 || eapol_type == EAPOL_4_4)
			RTW_INFO("Respond unencrypted eapol key\n");

		pattrib->encrypt = 0;

		if ((pattrib->ether_type != 0x888e) && (check_fwstate(pmlmepriv, WIFI_MP_STATE) == _FALSE)) {
#ifdef DBG_TX_DROP_FRAME
			RTW_INFO("DBG_TX_DROP_FRAME %s psta->ieee8021x_blocked == _TRUE,  pattrib->ether_type(%04x) != 0x888e\n", __FUNCTION__, pattrib->ether_type);
#endif
			res = _FAIL;
			goto exit;
		}
	} else {
		GET_ENCRY_ALGO(psecuritypriv, psta, pattrib->encrypt, bmcast);

#ifdef CONFIG_WAPI_SUPPORT
		if (pattrib->ether_type == 0x88B4)
			pattrib->encrypt = _NO_PRIVACY_;
#endif

		switch (psecuritypriv->dot11AuthAlgrthm) {
		case dot11AuthAlgrthm_Open:
		case dot11AuthAlgrthm_Shared:
		case dot11AuthAlgrthm_Auto:
			pattrib->key_idx = (u8)psecuritypriv->dot11PrivacyKeyIndex;
			break;
		case dot11AuthAlgrthm_8021X:
			if (bmcast)
				pattrib->key_idx = (u8)lsecuritypriv->dot118021XGrpKeyid;
			else
				pattrib->key_idx = 0;
			break;
		default:
			pattrib->key_idx = 0;
			break;
		}

		/* For WPS 1.0 WEP, driver should not encrypt EAPOL Packet for WPS handshake. */
		if (((pattrib->encrypt == _WEP40_) || (pattrib->encrypt == _WEP104_)) && (pattrib->ether_type == 0x888e))
			pattrib->encrypt = _NO_PRIVACY_;

	}

#ifdef CONFIG_TDLS
	if (pattrib->direct_link == _TRUE) {
		if (pattrib->encrypt > 0)
			pattrib->encrypt = _AES_;
	}
#endif

	switch (pattrib->encrypt) {
	case _WEP40_:
	case _WEP104_:
		pattrib->iv_len = 4;
		pattrib->icv_len = 4;
		hw_security_zero_hdrlen = _TRUE;
		break;

	case _TKIP_:
		pattrib->iv_len = 8;
		pattrib->icv_len = 4;
		hw_security_zero_hdrlen = _TRUE;

		if (psecuritypriv->busetkipkey == _FAIL) {
#ifdef DBG_TX_DROP_FRAME
			RTW_INFO("DBG_TX_DROP_FRAME %s psecuritypriv->busetkipkey(%d)==_FAIL drop packet\n", __FUNCTION__, psecuritypriv->busetkipkey);
#endif
			res = _FAIL;
			goto exit;
		}

		_rtw_memcpy(pattrib->dot11tkiptxmickey.skey, psta->dot11tkiptxmickey.skey, 16);

		break;

	case _AES_:

		pattrib->iv_len = 8;
		pattrib->icv_len = 8;
		hw_security_zero_hdrlen = _TRUE;

		break;

	case _GCMP_:
	case _GCMP_256_:

		pattrib->iv_len = 8;
		pattrib->icv_len = 16;
		hw_security_zero_hdrlen = _TRUE;

		break;

	case _CCMP_256_:

		pattrib->iv_len = 8;
		pattrib->icv_len = 16;
		hw_security_zero_hdrlen = _TRUE;

		break;

#ifdef CONFIG_WAPI_SUPPORT
	case _SMS4_:
		pattrib->iv_len = 18;
		pattrib->icv_len = 16;
		break;
#endif
	default:
		pattrib->iv_len = 0;
		pattrib->icv_len = 0;
		break;
	}

	if (pattrib->encrypt > 0) {
		_rtw_memcpy(pattrib->dot118021x_UncstKey.skey
			, psta->dot118021x_UncstKey.skey
			, (pattrib->encrypt & _SEC_TYPE_256_) ? 32 : 16);
	}

	if (!bmcast)
		hw_decrypted = psta->hw_decrypted;
	else
		hw_decrypted = psecuritypriv->hw_decrypted;

	if (pattrib->encrypt &&
		(padapter->securitypriv.sw_encrypt == _TRUE || hw_decrypted == _FALSE || pattrib->encrypt == _TKIP_)) {
		pattrib->bswenc = _TRUE;
	} else {
		pattrib->bswenc = _FALSE;
	}

#if defined(CONFIG_CONCURRENT_MODE)
	pattrib->bmc_camid = padapter->securitypriv.dot118021x_bmc_cam_id;
#endif

#ifdef CONFIG_WAPI_SUPPORT
	if (pattrib->encrypt == _SMS4_)
		pattrib->bswenc = _FALSE;
#endif

	if ((pattrib->encrypt) && (eapol_type == EAPOL_4_4))
		pattrib->bswenc = _TRUE;

	if ((pattrib->bswenc == _FALSE) &&
	    (hw_security_zero_hdrlen = _TRUE) &&
	    (padapter->dvobj->phl_com->dev_cap.sec_cap.hw_form_hdr)) {
		pattrib->iv_len = 0;
	}
exit:

	return res;

}

u8	qos_acm(u8 acm_mask, u8 priority)
{
	u8	change_priority = priority;

	switch (priority) {
	case 0:
	case 3:
		if (acm_mask & BIT(1))
			change_priority = 1;
		break;
	case 1:
	case 2:
		break;
	case 4:
	case 5:
		if (acm_mask & BIT(2))
			change_priority = 0;
		break;
	case 6:
	case 7:
		if (acm_mask & BIT(3))
			change_priority = 5;
		break;
	default:
		RTW_INFO("qos_acm(): invalid pattrib->priority: %d!!!\n", priority);
		break;
	}

	return change_priority;
}

/* refer to IEEE802.11-2016 Table R-3; Comply with IETF RFC4594 */
u8 tos_to_up(u8 tos)
{
	u8 up = 0;
	u8 dscp;
	u8 mode = CONFIG_RTW_UP_MAPPING_RULE;


	/* tos precedence mapping */
	if (mode == 0) {
		up = tos >> 5;
		return up;
	}

	/* refer to IEEE802.11-2016 Table R-3;
	 * DCSP 32(CS4) comply with IETF RFC4594
	 */
	dscp = (tos >> 2);

	if (dscp == 0)
		up = 0;
	else if (dscp >= 1 && dscp <= 9)
		up = 1;
	else if (dscp >= 10 && dscp <= 16)
		up = 2;
	else if (dscp >= 17 && dscp <= 23)
		up = 3;
	else if (dscp >= 24 && dscp <= 31)
		up = 4;
	else if (dscp >= 33 && dscp <= 40)
		up = 5;
	else if ((dscp >= 41 && dscp <= 47) || (dscp == 32))
		up = 6;
	else if (dscp >= 48 && dscp <= 63)
		up = 7;

	return up;
}

#if 0 //RTW_PHL_TX: mark un-finished codes for reading
static void set_qos_core(struct xmit_frame *pxframe)
{
	s32 UserPriority = 0;

	if (!pxframe->pkt)
		goto null_pkt;

	/* get UserPriority from IP hdr */
	if (pxframe->attrib.ether_type == 0x0800) {
		struct pkt_file ppktfile;
		struct ethhdr etherhdr;
		struct iphdr ip_hdr;

		_rtw_open_pktfile(pxframe->pkt, &ppktfile);
		_rtw_pktfile_read(&ppktfile, (unsigned char *)&etherhdr, ETH_HLEN);
		_rtw_pktfile_read(&ppktfile, (u8 *)&ip_hdr, sizeof(ip_hdr));
		UserPriority = tos_to_up(ip_hdr.tos);
	}


	#ifdef CONFIG_ICMP_VOQ
	if (pxframe->attrib.icmp_pkt == 1)/*use VO queue to send icmp packet*/
		UserPriority = 7;
	#endif
	#ifdef CONFIG_IP_R_MONITOR
	if (pxframe->attrib.ether_type == ETH_P_ARP)
		UserPriority = 7;
	#endif/*CONFIG_IP_R_MONITOR*/

null_pkt:
	pxframe->attrib.priority = UserPriority;
	pxframe->attrib.hdrlen = WLAN_HDR_A3_QOS_LEN;
	pxframe->attrib.subtype = WIFI_QOS_DATA_TYPE;
}
#endif

static void set_qos(struct sk_buff *pkt, struct pkt_attrib *pattrib)
{
	s32 UserPriority = 0;

	if (!pkt)
		goto null_pkt;

	/* get UserPriority from IP hdr */
	if (pattrib->ether_type == 0x0800) {
		struct pkt_file ppktfile;
		struct ethhdr etherhdr;
		struct iphdr ip_hdr;

		_rtw_open_pktfile(pkt, &ppktfile);
		_rtw_pktfile_read(&ppktfile, (unsigned char *)&etherhdr, ETH_HLEN);
		_rtw_pktfile_read(&ppktfile, (u8 *)&ip_hdr, sizeof(ip_hdr));
		/*		UserPriority = (ntohs(ip_hdr.tos) >> 5) & 0x3; */
		UserPriority = tos_to_up(ip_hdr.tos);
	}
	/*
		else if (pattrib->ether_type == 0x888e) {


			UserPriority = 7;
		}
	*/

	#ifdef CONFIG_ICMP_VOQ
	if (pattrib->icmp_pkt == 1)/*use VO queue to send icmp packet*/
		UserPriority = 7;
	#endif
	#ifdef CONFIG_IP_R_MONITOR
	if (pattrib->ether_type == ETH_P_ARP)
		UserPriority = 7;
	#endif/*CONFIG_IP_R_MONITOR*/

null_pkt:
	pattrib->priority = UserPriority;
	pattrib->hdrlen = XATTRIB_GET_WDS(pattrib) ? WLAN_HDR_A4_QOS_LEN : WLAN_HDR_A3_QOS_LEN;
	pattrib->subtype = WIFI_QOS_DATA_TYPE;
}

#ifdef CONFIG_TDLS
u8 rtw_check_tdls_established(_adapter *padapter, struct pkt_attrib *pattrib)
{
	pattrib->ptdls_sta = NULL;

	pattrib->direct_link = _FALSE;
	if (padapter->tdlsinfo.link_established == _TRUE) {
		pattrib->ptdls_sta = rtw_get_stainfo(&padapter->stapriv, pattrib->dst);
#if 1
		if ((pattrib->ptdls_sta != NULL) &&
		    (pattrib->ptdls_sta->tdls_sta_state & TDLS_LINKED_STATE) &&
		    (pattrib->ether_type != 0x0806)) {
			pattrib->direct_link = _TRUE;
			/* RTW_INFO("send ptk to "MAC_FMT" using direct link\n", MAC_ARG(pattrib->dst)); */
		}
#else
		if (pattrib->ptdls_sta != NULL &&
		    pattrib->ptdls_sta->tdls_sta_state & TDLS_LINKED_STATE) {
			pattrib->direct_link = _TRUE;
#if 0
			RTW_INFO("send ptk to "MAC_FMT" using direct link\n", MAC_ARG(pattrib->dst));
#endif
		}

		/* ARP frame may be helped by AP*/
		if (pattrib->ether_type != 0x0806)
			pattrib->direct_link = _FALSE;
#endif
	}

	return pattrib->direct_link;
}

s32 update_tdls_attrib(_adapter *padapter, struct _ADAPTER_LINK *padapter_link, struct pkt_attrib *pattrib)
{

	struct sta_info *psta = NULL;
	struct sta_priv		*pstapriv = &padapter->stapriv;
	struct link_mlme_priv	*pmlmepriv = &padapter_link->mlmepriv;
	struct qos_priv		*pqospriv = &pmlmepriv->qospriv;
	struct link_mlme_ext_priv	*pmlmeext = &padapter_link->mlmeextpriv;

	s32 res = _SUCCESS;

	psta = rtw_get_stainfo(pstapriv, pattrib->ra);
	if (psta == NULL)	{
		res = _FAIL;
		goto exit;
	}

	pattrib->mac_id = psta->phl_sta->macid;
	pattrib->psta = psta;
	pattrib->adapter_link = padapter_link;
	pattrib->ack_policy = 0;
	/* get ether_hdr_len */
	pattrib->pkt_hdrlen = ETH_HLEN;

	pattrib->qos_en = psta->qos_option;

	/* [TDLS] TODO: setup req/rsp should be AC_BK */
	if (pqospriv->qos_option &&  psta->qos_option) {
		pattrib->priority = 4;	/* tdls management frame should be AC_VI */
		pattrib->hdrlen = WLAN_HDR_A3_QOS_LEN;
		pattrib->subtype = WIFI_QOS_DATA_TYPE;
	} else {
		pattrib->priority = 0;
		pattrib->hdrlen = WLAN_HDR_A3_LEN;
		pattrib->subtype = WIFI_DATA_TYPE;
	}

	pattrib->rate = pmlmeext->tx_rate;

	/* TODO:_lock */
	if (update_attrib_sec_info(padapter, pattrib, psta, NON_EAPOL) == _FAIL) {
		res = _FAIL;
		goto exit;
	}

	update_attrib_phy_info(padapter, pattrib, psta);


exit:

	return res;
}

#endif /* CONFIG_TDLS */

#if 0 //RTW_PHL_TX: mark un-finished codes for reading
static s32 update_xmitframe_from_hdr(_adapter *padapter, struct xmit_frame *pxframe)
{
	uint i;
	struct pkt_file pktfile;
	struct sta_info *psta = NULL;
	struct ethhdr etherhdr;
	struct sk_buff *pkt = NULL;
	sint bmcast;

	struct sta_priv		*pstapriv = &padapter->stapriv;
	struct mlme_priv	*pmlmepriv = &padapter->mlmepriv;
	struct qos_priv		*pqospriv = &pmlmepriv->qospriv;
	struct xmit_priv	*pxmitpriv = &padapter->xmitpriv;

	PHLTX_LOG;

	if (pxframe->pkt)
		pkt = pxframe->pkt;
	else
		return FAIL;

	PHLTX_LOG;

	_rtw_open_pktfile(pkt, &pktfile);
	i = _rtw_pktfile_read(&pktfile, (u8 *)&etherhdr, ETH_HLEN);

	pxframe->attrib.ether_type = ntohs(etherhdr.h_proto);

	if (MLME_IS_MESH(padapter)) /* address resolve is done for mesh */
		goto get_sta_info;

	_rtw_memcpy(pxframe->attrib.dst, &etherhdr.h_dest, ETH_ALEN);
	_rtw_memcpy(pxframe->attrib.src, &etherhdr.h_source, ETH_ALEN);

	if ((check_fwstate(pmlmepriv, WIFI_ADHOC_STATE) == _TRUE) ||
	    (check_fwstate(pmlmepriv, WIFI_ADHOC_MASTER_STATE) == _TRUE)) {
		_rtw_memcpy(pxframe->attrib.ra, pxframe->attrib.dst, ETH_ALEN);
		_rtw_memcpy(pxframe->attrib.ta, adapter_mac_addr(padapter), ETH_ALEN);
	} else if (MLME_IS_STA(padapter)) {

#if 0//def CONFIG_TDLS  //rtw_phl_tx
		if (rtw_check_tdls_established(padapter, pattrib) == _TRUE)
			_rtw_memcpy(pattrib->ra, pattrib->dst, ETH_ALEN);	/* For TDLS direct link Tx, set ra to be same to dst */
		else
#endif
		_rtw_memcpy(pxframe->attrib.ra, get_bssid(pmlmepriv), ETH_ALEN);
		_rtw_memcpy(pxframe->attrib.ta, adapter_mac_addr(padapter), ETH_ALEN);
		DBG_COUNTER(padapter->tx_logs.core_tx_upd_attrib_sta);
	} else if (check_fwstate(pmlmepriv, WIFI_AP_STATE)) {
		_rtw_memcpy(pxframe->attrib.ra, pxframe->attrib.dst, ETH_ALEN);
		_rtw_memcpy(pxframe->attrib.ta, get_bssid(pmlmepriv), ETH_ALEN);
		DBG_COUNTER(padapter->tx_logs.core_tx_upd_attrib_ap);
	} else
		DBG_COUNTER(padapter->tx_logs.core_tx_upd_attrib_unknown);

	PHLTX_LOG;

get_sta_info:
	bmcast = IS_MCAST(pxframe->attrib.ra);
	if (bmcast) {
		PHLTX_LOG;
		psta = rtw_get_bcmc_stainfo(padapter);
		if (psta == NULL) { /* if we cannot get psta => drop the pkt */
			DBG_COUNTER(padapter->tx_logs.core_tx_upd_attrib_err_sta);
			#ifdef DBG_TX_DROP_FRAME
			RTW_INFO("DBG_TX_DROP_FRAME %s get sta_info fail, ra:" MAC_FMT"\n", __func__, MAC_ARG(pxframe->attrib.ra));
			#endif
			return FAIL;
		}
	} else {
		PHLTX_LOG;
		psta = rtw_get_stainfo(pstapriv, pxframe->attrib.ra);
		if (psta == NULL) { /* if we cannot get psta => drop the pkt */
			DBG_COUNTER(padapter->tx_logs.core_tx_upd_attrib_err_ucast_sta);
			#ifdef DBG_TX_DROP_FRAME
			RTW_INFO("DBG_TX_DROP_FRAME %s get sta_info fail, ra:" MAC_FMT"\n", __func__, MAC_ARG(pxframe->attrib.ra));
			#endif
			return FAIL;
		} else if (check_fwstate(pmlmepriv, WIFI_AP_STATE) == _TRUE && !(psta->state & WIFI_ASOC_STATE)) {
			DBG_COUNTER(padapter->tx_logs.core_tx_upd_attrib_err_ucast_ap_link);
			return FAIL;
		}
	}

	PHLTX_LOG;

	if (!(psta->state & WIFI_ASOC_STATE)) {
		DBG_COUNTER(padapter->tx_logs.core_tx_upd_attrib_err_link);
		RTW_INFO("%s-"ADPT_FMT" psta("MAC_FMT")->state(0x%x) != WIFI_ASOC_STATE\n",
			__func__, ADPT_ARG(padapter), MAC_ARG(psta->phl_sta->mac_addr), psta->state);
		return FAIL;
	}

	pxframe->attrib.psta = psta;

	PHLTX_LOG;

	pxframe->attrib.pktlen = pktfile.pkt_len;

	/* TODO: 802.1Q VLAN header */
	/* TODO: IPV6 */

	if (ETH_P_IP == pxframe->attrib.ether_type) {
		u8 ip[20];

		_rtw_pktfile_read(&pktfile, ip, 20);

		if (GET_IPV4_IHL(ip) * 4 > 20)
			_rtw_pktfile_read(&pktfile, NULL, GET_IPV4_IHL(ip) - 20);

		pxframe->attrib.icmp_pkt = 0;
		pxframe->attrib.dhcp_pkt = 0;
		pxframe->attrib.hipriority_pkt = 0;

		if (GET_IPV4_PROTOCOL(ip) == 0x01) { /* ICMP */
			pxframe->attrib.icmp_pkt = 1;
			DBG_COUNTER(padapter->tx_logs.core_tx_upd_attrib_icmp);

		} else if (GET_IPV4_PROTOCOL(ip) == 0x11) { /* UDP */
			u8 udp[24];

			_rtw_pktfile_read(&pktfile, udp, 24);

			if ((GET_UDP_SRC(udp) == 68 && GET_UDP_DST(udp) == 67)
				|| (GET_UDP_SRC(udp) == 67 && GET_UDP_DST(udp) == 68)
			) {
				/* 67 : UDP BOOTP server, 68 : UDP BOOTP client */
				if (pxframe->attrib.pktlen > 282) { /* MINIMUM_DHCP_PACKET_SIZE */
					pxframe->attrib.dhcp_pkt = 1;
					DBG_COUNTER(padapter->tx_logs.core_tx_upd_attrib_dhcp);
					if (0)
						RTW_INFO("send DHCP packet\n");
				}
			}

			/* WaveAgent packet, increase priority so that the system can read data in time */
			if (((GET_UDP_SIG1(udp) == 0xcc) || (GET_UDP_SIG1(udp) == 0xdd)) &&
				(GET_UDP_SIG2(udp) == 0xe2)) {
				pxframe->attrib.hipriority_pkt = 1;
			}

		} else if (GET_IPV4_PROTOCOL(ip) == 0x06 /* TCP */
			&& rtw_st_ctl_chk_reg_s_proto(&psta->st_ctl, 0x06) == _TRUE
		) {
			u8 tcp[20];

			_rtw_pktfile_read(&pktfile, tcp, 20);

			if (rtw_st_ctl_chk_reg_rule(&psta->st_ctl, padapter, IPV4_SRC(ip), TCP_SRC(tcp), IPV4_DST(ip), TCP_DST(tcp)) == _TRUE) {
				if (GET_TCP_SYN(tcp) && GET_TCP_ACK(tcp)) {
					session_tracker_add_cmd(padapter, psta
						, IPV4_SRC(ip), TCP_SRC(tcp)
						, IPV4_SRC(ip), TCP_DST(tcp));
					if (DBG_SESSION_TRACKER)
						RTW_INFO(FUNC_ADPT_FMT" local:"IP_FMT":"PORT_FMT", remote:"IP_FMT":"PORT_FMT" SYN-ACK\n"
							, FUNC_ADPT_ARG(padapter)
							, IP_ARG(IPV4_SRC(ip)), PORT_ARG(TCP_SRC(tcp))
							, IP_ARG(IPV4_DST(ip)), PORT_ARG(TCP_DST(tcp)));
				}
				if (GET_TCP_FIN(tcp)) {
					session_tracker_del_cmd(padapter, psta
						, IPV4_SRC(ip), TCP_SRC(tcp)
						, IPV4_SRC(ip), TCP_DST(tcp));
					if (DBG_SESSION_TRACKER)
						RTW_INFO(FUNC_ADPT_FMT" local:"IP_FMT":"PORT_FMT", remote:"IP_FMT":"PORT_FMT" FIN\n"
							, FUNC_ADPT_ARG(padapter)
							, IP_ARG(IPV4_SRC(ip)), PORT_ARG(TCP_SRC(tcp))
							, IP_ARG(IPV4_DST(ip)), PORT_ARG(TCP_DST(tcp)));
				}
			}
		}

	} else if (0x888e == pxframe->attrib.ether_type)
		parsing_eapol_packet(padapter, pktfile.cur_addr, psta, 1);
#if defined (DBG_ARP_DUMP) || defined (DBG_IP_R_MONITOR)
	else if (pxframe->attrib.ether_type == ETH_P_ARP) {
		u8 arp[28] = {0};

		_rtw_pktfile_read(&pktfile, arp, 28);
		dump_arp_pkt(RTW_DBGDUMP, etherhdr.h_dest, etherhdr.h_source, arp, 1);
	}
#endif

	PHLTX_LOG;

	if ((pxframe->attrib.ether_type == 0x888e) || (pxframe->attrib.dhcp_pkt == 1))
		rtw_mi_set_scan_deny(padapter, 3000);

	if (MLME_IS_STA(padapter) &&
		pxframe->attrib.ether_type == ETH_P_ARP &&
		!IS_MCAST(pxframe->attrib.dst)) {
		rtw_mi_set_scan_deny(padapter, 1000);
		rtw_mi_scan_abort(padapter, _FALSE); /*rtw_scan_abort_no_wait*/
	}


	PHLTX_LOG;

	/* get ether_hdr_len */
	pxframe->attrib.pkt_hdrlen = ETH_HLEN;/* (pattrib->ether_type == 0x8100) ? (14 + 4 ): 14; */ /* vlan tag */

	pxframe->attrib.hdrlen = WLAN_HDR_A3_LEN;
	pxframe->attrib.type = WIFI_DATA_TYPE;
	pxframe->attrib.subtype = WIFI_DATA_TYPE;
	pxframe->attrib.qos_en = pxframe->attrib.psta->qos_option;
	pxframe->attrib.priority = 0;

	pxframe->attrib.frag_len = pxmitpriv->frag_len;



	PHLTX_LOG;

	return SUCCESS;
}


static s32 update_xmitframe_qos(_adapter *padapter, struct xmit_frame *pxframe)
{

	struct sta_priv		*pstapriv = &padapter->stapriv;
	struct mlme_priv	*pmlmepriv = &padapter->mlmepriv;
	struct qos_priv		*pqospriv = &pmlmepriv->qospriv;
	struct xmit_priv	*pxmitpriv = &padapter->xmitpriv;

	if (!pxframe->attrib.qos_en)
		return SUCCESS;

	if (check_fwstate(pmlmepriv, WIFI_AP_STATE | WIFI_MESH_STATE
			| WIFI_ADHOC_STATE | WIFI_ADHOC_MASTER_STATE)
		) {
				set_qos_core(pxframe);
			#if 0//rtw_phl_tx def CONFIG_RTW_MESH
				if (MLME_IS_MESH(padapter))
					rtw_mesh_tx_set_whdr_mctrl_len(pattrib->mesh_frame_mode, pattrib);
			#endif
		} else {
#if 0// def CONFIG_TDLS
			if (pattrib->direct_link == _TRUE) {
				if (pattrib->qos_en)
					set_qos(pkt, pattrib);
			} else
#endif
			{
				if (pqospriv->qos_option) {
					set_qos_core(pxframe);

					if (pmlmepriv->acm_mask != 0)
						pxframe->attrib.priority = qos_acm(pmlmepriv->acm_mask, pxframe->attrib.priority);
				}
			}
		}

	return SUCCESS;
}

static s32 update_xmitframe_security(_adapter *padapter, struct xmit_frame *pxframe) //rtw_phl_tx todo
{
	sint res = _SUCCESS;
	struct mlme_priv	*pmlmepriv = &padapter->mlmepriv;
	struct security_priv *psecuritypriv = &padapter->securitypriv;
	sint bmcast = IS_MCAST(pxframe->attrib.ra);

#if 0
	_rtw_memset(pattrib->dot118021x_UncstKey.skey,  0, 16);
	_rtw_memset(pattrib->dot11tkiptxmickey.skey,  0, 16);
	pattrib->mac_id = psta->phl_sta->macid;
#endif

	if (pxframe->attrib.psta->ieee8021x_blocked == _TRUE) {

		pxframe->attrib.encrypt = 0;

		if ((pxframe->attrib.ether_type != 0x888e) && (check_fwstate(pmlmepriv, WIFI_MP_STATE) == _FALSE)) {
#ifdef DBG_TX_DROP_FRAME
			RTW_INFO("DBG_TX_DROP_FRAME %s psta->ieee8021x_blocked == _TRUE,  pattrib->ether_type(%04x) != 0x888e\n", __FUNCTION__, pxframe->attrib.ether_type);
#endif
			res = _FAIL;
			goto exit;
		}
	} else {
		GET_ENCRY_ALGO(psecuritypriv, pxframe->attrib.psta, pxframe->attrib.encrypt, bmcast);

#ifdef CONFIG_WAPI_SUPPORT
		if (pxframe->attrib.ether_type == 0x88B4)
			pxframe->attrib.encrypt = _NO_PRIVACY_;
#endif

		switch (psecuritypriv->dot11AuthAlgrthm) {
		case dot11AuthAlgrthm_Open:
		case dot11AuthAlgrthm_Shared:
		case dot11AuthAlgrthm_Auto:
			pxframe->attrib.key_idx = (u8)psecuritypriv->dot11PrivacyKeyIndex;
			break;
		case dot11AuthAlgrthm_8021X:
			if (bmcast)
				pxframe->attrib.key_idx = (u8)psecuritypriv->dot118021XGrpKeyid;
			else
				pxframe->attrib.key_idx = 0;
			break;
		default:
			pxframe->attrib.key_idx = 0;
			break;
		}

		/* For WPS 1.0 WEP, driver should not encrypt EAPOL Packet for WPS handshake. */
		if (((pxframe->attrib.encrypt == _WEP40_) || (pxframe->attrib.encrypt == _WEP104_)) && (pxframe->attrib.ether_type == 0x888e))
			pxframe->attrib.encrypt = _NO_PRIVACY_;

	}

#if 0 //def CONFIG_TDLS
	if (pattrib->direct_link == _TRUE) {
		if (pxframe->attrib.encrypt > 0)
			pxframe->attrib.encrypt = _AES_;
	}
#endif

	switch (pxframe->attrib.encrypt) {
	case _WEP40_:
	case _WEP104_:
		pxframe->attrib.iv_len = 4;
		pxframe->attrib.icv_len = 4;
		WEP_IV(pxframe->attrib.iv, pxframe->attrib.psta->dot11txpn, pxframe->attrib.key_idx);
		break;

	case _TKIP_:
		pxframe->attrib.iv_len = 8;
		pxframe->attrib.icv_len = 4;

		if (psecuritypriv->busetkipkey == _FAIL) {
#ifdef DBG_TX_DROP_FRAME
			RTW_INFO("DBG_TX_DROP_FRAME %s psecuritypriv->busetkipkey(%d)==_FAIL drop packet\n", __FUNCTION__, psecuritypriv->busetkipkey);
#endif
			res = _FAIL;
			goto exit;
		}

		if (bmcast)
			TKIP_IV(pxframe->attrib.iv, pxframe->attrib.psta->dot11txpn, pxframe->attrib.key_idx);
		else
			TKIP_IV(pxframe->attrib.iv, pxframe->attrib.psta->dot11txpn, 0);


		//_rtw_memcpy(pattrib->dot11tkiptxmickey.skey, pxframe->attrib.psta->dot11tkiptxmickey.skey, 16);

		break;

	case _AES_:

		pxframe->attrib.iv_len = 8;
		pxframe->attrib.icv_len = 8;

		if (bmcast)
			AES_IV(pxframe->attrib.iv, pxframe->attrib.psta->dot11txpn, pxframe->attrib.key_idx);
		else
			AES_IV(pxframe->attrib.iv, pxframe->attrib.psta->dot11txpn, 0);

		break;

	case _GCMP_:
	case _GCMP_256_:

		pxframe->attrib.iv_len = 8;
		pxframe->attrib.icv_len = 16;

		if (bmcast)
			GCMP_IV(pxframe->attrib.iv, pxframe->attrib.psta->dot11txpn, pxframe->attrib.key_idx);
		else
			GCMP_IV(pxframe->attrib.iv, pxframe->attrib.psta->dot11txpn, 0);

		break;

	case _CCMP_256_:

		pxframe->attrib.iv_len = 8;
		pxframe->attrib.icv_len = 16;

		if (bmcast)
			GCMP_IV(pxframe->attrib.iv, pxframe->attrib.psta->dot11txpn, pxframe->attrib.key_idx);
		else
			GCMP_IV(pxframe->attrib.iv, pxframe->attrib.psta->dot11txpn, 0);

		break;

#ifdef CONFIG_WAPI_SUPPORT
	case _SMS4_:
		pxframe->attrib.iv_len = 18;
		pxframe->attrib.icv_len = 16;
		rtw_wapi_get_iv(padapter, pxframe->attrib.ra, pxframe->attrib.iv);
		break;
#endif
	default:
		pxframe->attrib.iv_len = 0;
		pxframe->attrib.icv_len = 0;
		break;
	}

#if 0
	if (pxframe->attrib.encrypt > 0) {
		_rtw_memcpy(pattrib->dot118021x_UncstKey.skey
			, pxframe->attrib.psta->dot118021x_UncstKey.skey
			, (pxframe->attrib.encrypt & _SEC_TYPE_256_) ? 32 : 16);
	}
#endif

	if (pxframe->attrib.encrypt &&
	    ((padapter->securitypriv.sw_encrypt == _TRUE) || (psecuritypriv->hw_decrypted == _FALSE))) {
		pxframe->attrib.bswenc = _TRUE;
	} else {
		pxframe->attrib.bswenc = _FALSE;
	}

#if defined(CONFIG_CONCURRENT_MODE)
	//pattrib->bmc_camid = padapter->securitypriv.dot118021x_bmc_cam_id;
#endif

#ifdef CONFIG_WAPI_SUPPORT
	if (pxframe->attrib.encrypt == _SMS4_)
		pxframe->attrib.bswenc = _FALSE;
#endif

exit:
	return res;

}

static s32 update_xmitframe_hw(_adapter *padapter, struct xmit_frame *pxframe)
{
	pxframe->phl_txreq->mdata.rts_en = pxframe->attrib.psta->rtsen;
	pxframe->phl_txreq->mdata.cts2self = pxframe->attrib.psta->cts2self;
	pxframe->phl_txreq->mdata.ampdu_density = 0;
	return 0;
}

#if 0
static s32 rtw_core_update_txattrib(_adapter *padapter, struct xmit_frame *pxframe)
{
	uint i;
	struct pkt_file pktfile;
	struct sta_info *psta = NULL;
	struct ethhdr etherhdr;
	struct sk_buff *pkt = NULL;

	struct sta_priv		*pstapriv = &padapter->stapriv;
	struct mlme_priv	*pmlmepriv = &padapter->mlmepriv;
	struct qos_priv		*pqospriv = &pmlmepriv->qospriv;
	struct xmit_priv	*pxmitpriv = &padapter->xmitpriv;
	sint res = _SUCCESS;

#if 0//rtw_phl_tx todo def CONFIG_BEAMFORMING
	update_attrib_txbf_info(padapter, pattrib, psta);
#endif

#if 0
	/* TODO:_lock */
	if (update_attrib_sec_info(padapter, pattrib, psta, NON_EAPOL) == _FAIL) {
		DBG_COUNTER(padapter->tx_logs.core_tx_upd_attrib_err_sec);
		res = _FAIL;
		goto exit;
	}
#endif

	update_attrib_phy_info(padapter, pattrib, psta);

	/* RTW_INFO("%s ==> mac_id(%d)\n",__FUNCTION__,pattrib->mac_id ); */
	/* TODO:_unlock */

#ifdef CONFIG_AUTO_AP_MODE
	if (psta->isrc && psta->pid > 0)
		pattrib->pctrl = _TRUE;
	else
#endif
		pattrib->pctrl = 0;

	pattrib->ack_policy = 0;

	if (bmcast)
		pattrib->rate = psta->init_rate;

	/* pattrib->priority = 5; */ /* force to used VI queue, for testing */
	pattrib->hw_ssn_sel = pxmitpriv->hw_ssn_seq_no;

	pattrib->wdinfo_en = 1;/*FPGA_test YiWei need modify*/

	rtw_set_tx_chksum_offload(pkt, pattrib);

exit:
	return res;
}
#endif
#endif

static u8 rtw_chk_htc_en(_adapter *padapter, struct sta_info *psta, struct pkt_attrib *pattrib)
{

#ifdef CONFIG_80211AX_HE
	if (psta->hepriv.he_option == _TRUE) {
		/*By test, some HE AP eapol & arp & dhcp pkt can not append ht control*/
		if ((0x888e == pattrib->ether_type) || (0x0806 == pattrib->ether_type) || (pattrib->dhcp_pkt == 1))
			return 0;
		else if (!rtw_chk_phy_can_append_actrl(padapter, psta))
			return 0;
		else
			return rtw_he_htc_en(padapter, psta);
	}
#endif

	return 0;
}

static s32 update_attrib(_adapter *padapter, struct sk_buff *pkt, struct pkt_attrib *pattrib)
{
	uint i;
	struct pkt_file pktfile;
	struct sta_info *psta = NULL;
	struct ethhdr etherhdr;

	sint bmcast;
	struct sta_priv		*pstapriv = &padapter->stapriv;
	struct mlme_priv		*pmlmepriv = &padapter->mlmepriv;
	struct xmit_priv		*pxmitpriv = &padapter->xmitpriv;
	sint res = _SUCCESS;
	enum eap_type eapol_type = NON_EAPOL;
	struct _ADAPTER_LINK *padapter_link = NULL;
	struct qos_priv		*pqospriv = NULL;
	struct rtw_phl_mld_t *pmld = NULL;
	struct sta_info *lsta = NULL;
	u8 lidx;
	u16 macid = 0xffff;

	DBG_COUNTER(padapter->tx_logs.core_tx_upd_attrib);

	_rtw_open_pktfile(pkt, &pktfile);
	i = _rtw_pktfile_read(&pktfile, (u8 *)&etherhdr, ETH_HLEN);

	pattrib->ether_type = ntohs(etherhdr.h_proto);

	if (MLME_STATE(padapter) & (WIFI_AP_STATE | WIFI_MESH_STATE)) /* address resolve is done for ap/mesh */
		goto get_sta_info;

	_rtw_memcpy(pattrib->dst, &etherhdr.h_dest, ETH_ALEN);
	_rtw_memcpy(pattrib->src, &etherhdr.h_source, ETH_ALEN);
	_rtw_memcpy(pattrib->ta, adapter_mac_addr(padapter), ETH_ALEN);

	if ((check_fwstate(pmlmepriv, WIFI_ADHOC_STATE) == _TRUE) ||
	    (check_fwstate(pmlmepriv, WIFI_ADHOC_MASTER_STATE) == _TRUE)) {
		_rtw_memcpy(pattrib->ra, pattrib->dst, ETH_ALEN);
		DBG_COUNTER(padapter->tx_logs.core_tx_upd_attrib_adhoc);
	} else if (MLME_IS_STA(padapter)) {
		#ifdef CONFIG_TDLS
		if (rtw_check_tdls_established(padapter, pattrib) == _TRUE)
			_rtw_memcpy(pattrib->ra, pattrib->dst, ETH_ALEN);	/* For TDLS direct link Tx, set ra to be same to dst */
		else
		#endif
		{
			_rtw_memcpy(pattrib->ra, get_bssid(pmlmepriv), ETH_ALEN);
			#ifdef CONFIG_RTW_WDS
			if (adapter_use_wds(padapter)
				&& _rtw_memcmp(pattrib->src, pattrib->ta, ETH_ALEN) == _FALSE
			) {
				pattrib->wds = 1;
				if (IS_MCAST(pattrib->dst))
					rtw_tx_wds_gptr_update(padapter, pattrib->src);
			}
			#endif
		}
		DBG_COUNTER(padapter->tx_logs.core_tx_upd_attrib_sta);
	} else
		DBG_COUNTER(padapter->tx_logs.core_tx_upd_attrib_unknown);

get_sta_info:
	bmcast = IS_MCAST(pattrib->ra);
	if (bmcast) {
		/* ToDo CONFIG_RTW_MLD: [current primary link only]*/
		padapter_link = GET_PRIMARY_LINK(padapter);
		/* Update ta as link address for bcmc sta */
		_rtw_memcpy(pattrib->ta, padapter_link->mac_addr, ETH_ALEN);
		psta = rtw_get_bcmc_stainfo(padapter, padapter_link);
		if (psta == NULL) { /* if we cannot get psta => drop the pkt */
			DBG_COUNTER(padapter->tx_logs.core_tx_upd_attrib_err_sta);
			#ifdef DBG_TX_DROP_FRAME
			RTW_INFO("DBG_TX_DROP_FRAME %s get sta_info fail, ra:" MAC_FMT"\n", __func__, MAC_ARG(pattrib->ra));
			#endif
			res = _FAIL;
			goto exit;
		}
	} else {
		psta = rtw_get_stainfo(pstapriv, pattrib->ra);
		if (psta == NULL) { /* if we cannot get psta => drop the pkt */
			DBG_COUNTER(padapter->tx_logs.core_tx_upd_attrib_err_ucast_sta);
			#ifdef DBG_TX_DROP_FRAME
			RTW_INFO("DBG_TX_DROP_FRAME %s get sta_info fail, ra:" MAC_FMT"\n", __func__, MAC_ARG(pattrib->ra));
			#endif
			res = _FAIL;
			goto exit;
		} else if (check_fwstate(pmlmepriv, WIFI_AP_STATE) == _TRUE && !(psta->state & WIFI_ASOC_STATE)) {
			DBG_COUNTER(padapter->tx_logs.core_tx_upd_attrib_err_ucast_ap_link);
			res = _FAIL;
			goto exit;
		}

		#ifdef CONFIG_RTW_WDS
		if (XATTRIB_GET_WDS(pattrib) && !(psta->flags & WLAN_STA_WDS))
			pattrib->wds = 0;
		#endif
	}

	/* Find the primary sta with the minimum macid */
	pmld = psta->phl_sta->mld;
	psta = rtw_get_stainfo_by_macid(pstapriv, pmld->phl_sta[RTW_RLINK_PRIMARY]->macid);
	padapter_link = psta->padapter_link;
	pattrib->adapter_link = padapter_link;
	pqospriv = &padapter_link->mlmepriv.qospriv;

	if (!(psta->state & WIFI_ASOC_STATE)) {
		DBG_COUNTER(padapter->tx_logs.core_tx_upd_attrib_err_link);
		RTW_INFO("%s-"ADPT_FMT" psta("MAC_FMT")->state(0x%x) != WIFI_ASOC_STATE\n",
			__func__, ADPT_ARG(padapter), MAC_ARG(psta->phl_sta->mac_addr), psta->state);
		res = _FAIL;
		goto exit;
	}

	pattrib->pktlen = pktfile.pkt_len;
#ifdef CONFIG_CORE_TXSC
	pattrib->frag_len = pxmitpriv->frag_len;
#endif

	/* TODO: 802.1Q VLAN header */
	/* TODO: IPV6 */

	if (ETH_P_IP == pattrib->ether_type) {
		u8 ip[20];

		_rtw_pktfile_read(&pktfile, ip, 20);

		if (GET_IPV4_IHL(ip) * 4 > 20)
			_rtw_pktfile_read(&pktfile, NULL, GET_IPV4_IHL(ip) - 20);

		pattrib->icmp_pkt = 0;
		pattrib->dhcp_pkt = 0;
		pattrib->hipriority_pkt = 0;

		if (GET_IPV4_PROTOCOL(ip) == 0x01) { /* ICMP */
			pattrib->icmp_pkt = 1;
			DBG_COUNTER(padapter->tx_logs.core_tx_upd_attrib_icmp);

		} else if (GET_IPV4_PROTOCOL(ip) == 0x11) { /* UDP */
			u8 udp[24];

			_rtw_pktfile_read(&pktfile, udp, 24);

			if ((GET_UDP_SRC(udp) == 68 && GET_UDP_DST(udp) == 67)
				|| (GET_UDP_SRC(udp) == 67 && GET_UDP_DST(udp) == 68)
			) {
				/* 67 : UDP BOOTP server, 68 : UDP BOOTP client */
				if (pattrib->pktlen > 282) { /* MINIMUM_DHCP_PACKET_SIZE */
					pattrib->dhcp_pkt = 1;
					DBG_COUNTER(padapter->tx_logs.core_tx_upd_attrib_dhcp);
					if (0)
						RTW_INFO("send DHCP packet\n");
				}
			}

			/* WaveAgent packet, increase priority so that the system can read data in time */
			if (((GET_UDP_SIG1(udp) == 0xcc) || (GET_UDP_SIG1(udp) == 0xdd)) &&
				(GET_UDP_SIG2(udp) == 0xe2)) {
				pattrib->hipriority_pkt = 1;
			}

		} else if (GET_IPV4_PROTOCOL(ip) == 0x06 /* TCP */
			&& rtw_st_ctl_chk_reg_s_proto(&psta->st_ctl, 0x06) == _TRUE
		) {
			u8 tcp[20];

			_rtw_pktfile_read(&pktfile, tcp, 20);

			if (rtw_st_ctl_chk_reg_rule(&psta->st_ctl, padapter, IPV4_SRC(ip), TCP_SRC(tcp), IPV4_DST(ip), TCP_DST(tcp)) == _TRUE) {
				if (GET_TCP_SYN(tcp) && GET_TCP_ACK(tcp)) {
					session_tracker_add_cmd(padapter, psta
						, IPV4_SRC(ip), TCP_SRC(tcp)
						, IPV4_SRC(ip), TCP_DST(tcp));
					if (DBG_SESSION_TRACKER)
						RTW_INFO(FUNC_ADPT_FMT" local:"IP_FMT":"PORT_FMT", remote:"IP_FMT":"PORT_FMT" SYN-ACK\n"
							, FUNC_ADPT_ARG(padapter)
							, IP_ARG(IPV4_SRC(ip)), PORT_ARG(TCP_SRC(tcp))
							, IP_ARG(IPV4_DST(ip)), PORT_ARG(TCP_DST(tcp)));
				}
				if (GET_TCP_FIN(tcp)) {
					session_tracker_del_cmd(padapter, psta
						, IPV4_SRC(ip), TCP_SRC(tcp)
						, IPV4_SRC(ip), TCP_DST(tcp));
					if (DBG_SESSION_TRACKER)
						RTW_INFO(FUNC_ADPT_FMT" local:"IP_FMT":"PORT_FMT", remote:"IP_FMT":"PORT_FMT" FIN\n"
							, FUNC_ADPT_ARG(padapter)
							, IP_ARG(IPV4_SRC(ip)), PORT_ARG(TCP_SRC(tcp))
							, IP_ARG(IPV4_DST(ip)), PORT_ARG(TCP_DST(tcp)));
				}
			}
		}

	} else if (0x888e == pattrib->ether_type)
		eapol_type = parsing_eapol_packet(padapter, pktfile.cur_addr, psta, 1);
#if defined (DBG_ARP_DUMP) || defined (DBG_IP_R_MONITOR)
	else if (pattrib->ether_type == ETH_P_ARP) {
		u8 arp[28] = {0};

		_rtw_pktfile_read(&pktfile, arp, 28);
		dump_arp_pkt(RTW_DBGDUMP, etherhdr.h_dest, etherhdr.h_source, arp, 1);
	}
#endif

	if ((pattrib->ether_type == 0x888e) || (pattrib->dhcp_pkt == 1))
		rtw_mi_set_scan_deny(padapter, 3000);

	if (MLME_IS_STA(padapter) &&
		pattrib->ether_type == ETH_P_ARP &&
		!IS_MCAST(pattrib->dst)) {
		rtw_mi_set_scan_deny(padapter, 1000);
		rtw_mi_scan_abort(padapter, _FALSE); /*rtw_scan_abort_no_wait*/
	}

	/* TODO:_lock */
	if (update_attrib_sec_info(padapter, pattrib, psta, eapol_type) == _FAIL) {
		DBG_COUNTER(padapter->tx_logs.core_tx_upd_attrib_err_sec);
		res = _FAIL;
		goto exit;
	}

	/* get ether_hdr_len */
	pattrib->pkt_hdrlen = ETH_HLEN;/* (pattrib->ether_type == 0x8100) ? (14 + 4 ): 14; */ /* vlan tag */

	pattrib->hdrlen = XATTRIB_GET_WDS(pattrib) ? WLAN_HDR_A4_LEN : WLAN_HDR_A3_LEN;
	pattrib->type = WIFI_DATA_TYPE;
	pattrib->subtype = WIFI_DATA_TYPE;
	pattrib->qos_en = psta->qos_option;
	pattrib->priority = 0;

	if (check_fwstate(pmlmepriv, WIFI_AP_STATE | WIFI_MESH_STATE
		| WIFI_ADHOC_STATE | WIFI_ADHOC_MASTER_STATE)
	) {
		if (pattrib->qos_en) {
			set_qos(pkt, pattrib);
			#ifdef CONFIG_RTW_MESH
			if (MLME_IS_MESH(padapter))
				rtw_mesh_tx_set_whdr_mctrl_len(pattrib->mesh_frame_mode, pattrib);
			#endif
		}
	} else {
#ifdef CONFIG_TDLS
		if (pattrib->direct_link == _TRUE) {
			if (pattrib->qos_en)
				set_qos(pkt, pattrib);
		} else
#endif
		{
			if (pqospriv->qos_option) {
				set_qos(pkt, pattrib);

				if (pmlmepriv->acm_mask != 0)
					pattrib->priority = qos_acm(pmlmepriv->acm_mask, pattrib->priority);
			}
		}
	}

	pattrib->order = rtw_chk_htc_en(padapter, psta, pattrib);
	if (pattrib->order) {
		if (pattrib->qos_en)
			pattrib->hdrlen = XATTRIB_GET_WDS(pattrib) ? WLAN_HDR_A4_QOS_HTC_LEN : WLAN_HDR_A3_QOS_HTC_LEN;
		else
			pattrib->hdrlen = XATTRIB_GET_WDS(pattrib) ? WLAN_HDR_A4_HTC_LEN : WLAN_HDR_A3_HTC_LEN;
	}

	update_attrib_phy_info(padapter, pattrib, psta);

	/* RTW_INFO("%s ==> mac_id(%d)\n",__FUNCTION__,pattrib->mac_id ); */

	pattrib->psta = psta;
	/* TODO:_unlock */

#ifdef CONFIG_AUTO_AP_MODE
	if (psta->isrc && psta->pid > 0)
		pattrib->pctrl = _TRUE;
	else
#endif
		pattrib->pctrl = 0;

	pattrib->ack_policy = 0;

	if (bmcast)
		pattrib->rate = psta->init_rate;

	/* pattrib->priority = 5; */ /* force to used VI queue, for testing */
	pattrib->hw_ssn_sel = pxmitpriv->hw_ssn_seq_no;

	pattrib->wdinfo_en = 1;/*FPGA_test YiWei need modify*/

	rtw_set_tx_chksum_offload(pkt, pattrib);

exit:


	return res;
}

static s32 xmitframe_addmic(_adapter *padapter, struct xmit_frame *pxmitframe)
{
	sint			curfragnum, length;
	u8	*pframe, *payload, mic[8];
	struct	mic_data		micdata;
	/* struct	sta_info		*stainfo; */
	struct	pkt_attrib	*pattrib = &pxmitframe->attrib;
	struct	security_priv	*psecuritypriv = &padapter->securitypriv;
	struct	xmit_priv		*pxmitpriv = &padapter->xmitpriv;
	u8 priority[4] = {0x0, 0x0, 0x0, 0x0};
	u8 hw_hdr_offset = 0;
	sint bmcst = IS_MCAST(pattrib->ra);
	struct _ADAPTER_LINK *padapter_link = pattrib->adapter_link;
	struct	link_security_priv	*lsecuritypriv = &padapter_link->securitypriv;

	/*
		if(pattrib->psta)
		{
			stainfo = pattrib->psta;
		}
		else
		{
			RTW_INFO("%s, call rtw_get_stainfo()\n", __func__);
			stainfo=rtw_get_stainfo(&padapter->stapriv ,&pattrib->ra[0]);
		}

		if(stainfo==NULL)
		{
			RTW_INFO("%s, psta==NUL\n", __func__);
			return _FAIL;
		}

		if(!(stainfo->state &WIFI_ASOC_STATE))
		{
			RTW_INFO("%s, psta->state(0x%x) != WIFI_ASOC_STATE\n", __func__, stainfo->state);
			return _FAIL;
		}
	*/


#ifdef CONFIG_USB_TX_AGGREGATION
	hw_hdr_offset = TXDESC_SIZE + (pxmitframe->pkt_offset * PACKET_OFFSET_SZ);;
#else
#ifdef CONFIG_TX_EARLY_MODE
	hw_hdr_offset = TXDESC_OFFSET + EARLY_MODE_INFO_SIZE;
#else
	hw_hdr_offset = TXDESC_OFFSET;
#endif
#endif

	if (pattrib->encrypt == _TKIP_) { /* if(psecuritypriv->dot11PrivacyAlgrthm==_TKIP_PRIVACY_) */
		/* encode mic code */
		/* if(stainfo!= NULL) */
		{
			u8 null_key[16] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};

			pframe = pxmitframe->buf_addr + hw_hdr_offset;

			if (bmcst) {
				if (_rtw_memcmp(lsecuritypriv->dot118021XGrptxmickey[lsecuritypriv->dot118021XGrpKeyid].skey, null_key, 16) == _TRUE) {
					/* DbgPrint("\nxmitframe_addmic:stainfo->dot11tkiptxmickey==0\n"); */
					/* rtw_msleep_os(10); */
					return _FAIL;
				}
				/* start to calculate the mic code */
				rtw_secmicsetkey(&micdata, lsecuritypriv->dot118021XGrptxmickey[lsecuritypriv->dot118021XGrpKeyid].skey);
			} else {
				if (_rtw_memcmp(&pattrib->dot11tkiptxmickey.skey[0], null_key, 16) == _TRUE) {
					/* DbgPrint("\nxmitframe_addmic:stainfo->dot11tkiptxmickey==0\n"); */
					/* rtw_msleep_os(10); */
					return _FAIL;
				}
				/* start to calculate the mic code */
				rtw_secmicsetkey(&micdata, &pattrib->dot11tkiptxmickey.skey[0]);
			}

			if (pframe[1] & 1) { /* ToDS==1 */
				rtw_secmicappend(&micdata, &pframe[16], 6);  /* DA */
				if (pframe[1] & 2) /* From Ds==1 */
					rtw_secmicappend(&micdata, &pframe[24], 6);
				else
					rtw_secmicappend(&micdata, &pframe[10], 6);
			} else {	/* ToDS==0 */
				rtw_secmicappend(&micdata, &pframe[4], 6);   /* DA */
				if (pframe[1] & 2) /* From Ds==1 */
					rtw_secmicappend(&micdata, &pframe[16], 6);
				else
					rtw_secmicappend(&micdata, &pframe[10], 6);

			}

			if (pattrib->qos_en)
				priority[0] = (u8)pxmitframe->attrib.priority;


			rtw_secmicappend(&micdata, &priority[0], 4);

			payload = pframe;

			for (curfragnum = 0; curfragnum < pattrib->nr_frags; curfragnum++) {
				payload = (u8 *)RND4((SIZE_PTR)(payload));

				payload = payload + pattrib->hdrlen + pattrib->iv_len;
				if ((curfragnum + 1) == pattrib->nr_frags) {
					length = pattrib->last_txcmdsz - pattrib->hdrlen - pattrib->iv_len - ((pattrib->bswenc) ? pattrib->icv_len : 0);
					rtw_secmicappend(&micdata, payload, length);
					payload = payload + length;
				} else {
					length = pxmitpriv->frag_len - pattrib->hdrlen - pattrib->iv_len - ((pattrib->bswenc) ? pattrib->icv_len : 0);
					rtw_secmicappend(&micdata, payload, length);
					payload = payload + length + pattrib->icv_len;
				}
			}
			rtw_secgetmic(&micdata, &(mic[0]));
			/* add mic code  and add the mic code length in last_txcmdsz */

			_rtw_memcpy(payload, &(mic[0]), 8);
			pattrib->last_txcmdsz += 8;

			payload = payload - pattrib->last_txcmdsz + 8;
		}
	}


	return _SUCCESS;
}

/*#define DBG_TX_SW_ENCRYPTOR*/

static s32 xmitframe_swencrypt(_adapter *padapter, struct xmit_frame *pxmitframe)
{
	struct pkt_attrib *pattrib = &pxmitframe->attrib;


	if (!pattrib->bswenc)
		return _SUCCESS;

#ifdef DBG_TX_SW_ENCRYPTOR
	RTW_INFO(ADPT_FMT" - sec_type:%s DO SW encryption\n",
		ADPT_ARG(padapter), security_type_str(pattrib->encrypt));
#endif

	switch (pattrib->encrypt) {
	case _WEP40_:
	case _WEP104_:
		rtw_wep_encrypt(padapter, (u8 *)pxmitframe);
		break;
	case _TKIP_:
		rtw_tkip_encrypt(padapter, (u8 *)pxmitframe);
		break;
	case _AES_:
	case _CCMP_256_:
		rtw_aes_encrypt(padapter, (u8 *)pxmitframe);
		break;
	case _GCMP_:
	case _GCMP_256_:
		rtw_gcmp_encrypt(padapter, (u8 *)pxmitframe);
		break;
#ifdef CONFIG_WAPI_SUPPORT
	case _SMS4_:
		rtw_sms4_encrypt(padapter, (u8 *)pxmitframe);
#endif
	default:
		break;
	}

	return _SUCCESS;
}

#if 0 //RTW_PHL_TX: mark un-finished codes for reading
static s32 rtw_core_xmitframe_addmic(_adapter *padapter, struct xmit_frame *pxframe)
{
	sint curfragnum, payload_length;
	u8	*pwlhdr, *payload, mic[8];
	struct	mic_data		micdata;
	/* struct	sta_info		*stainfo; */
	struct	security_priv	*psecuritypriv = &padapter->securitypriv;
	struct	xmit_priv		*pxmitpriv = &padapter->xmitpriv;
	u8 priority[4] = {0x0, 0x0, 0x0, 0x0};
	u8 hw_hdr_offset = 0;
	sint bmcst = IS_MCAST(pxframe->attrib.ra);

	if (pxframe->attrib.encrypt == _TKIP_) { /* if(psecuritypriv->dot11PrivacyAlgrthm==_TKIP_PRIVACY_) */
		/* encode mic code */
		/* if(stainfo!= NULL) */
		{
			u8 null_key[16] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};

			pwlhdr = pxframe->wlhdr[0];
			payload = pxframe->pkt->data + pxframe->attrib.pkt_hdrlen;
			payload_length = pxframe->pkt->len - pxframe->attrib.pkt_hdrlen;

			if (bmcst) {
				if (_rtw_memcmp(psecuritypriv->dot118021XGrptxmickey[psecuritypriv->dot118021XGrpKeyid].skey, null_key, 16) == _TRUE) {
					/* DbgPrint("\nxmitframe_addmic:stainfo->dot11tkiptxmickey==0\n"); */
					/* rtw_msleep_os(10); */
					return _FAIL;
				}
				/* start to calculate the mic code */
				rtw_secmicsetkey(&micdata, psecuritypriv->dot118021XGrptxmickey[psecuritypriv->dot118021XGrpKeyid].skey);
			} else {
				if (_rtw_memcmp(&pxframe->attrib.psta->dot11tkiptxmickey.skey[0], null_key, 16) == _TRUE) {
					/* DbgPrint("\nxmitframe_addmic:stainfo->dot11tkiptxmickey==0\n"); */
					/* rtw_msleep_os(10); */
					return _FAIL;
				}
				/* start to calculate the mic code */
				rtw_secmicsetkey(&micdata, &pxframe->attrib.psta->dot11tkiptxmickey.skey[0]);
			}

			if (pwlhdr[1] & 1) { /* ToDS==1 */
				rtw_secmicappend(&micdata, &pwlhdr[16], 6);  /* DA */
				if (pwlhdr[1] & 2) /* From Ds==1 */
					rtw_secmicappend(&micdata, &pwlhdr[24], 6);
				else
					rtw_secmicappend(&micdata, &pwlhdr[10], 6);
			} else {	/* ToDS==0 */
				rtw_secmicappend(&micdata, &pwlhdr[4], 6);   /* DA */
				if (pwlhdr[1] & 2) /* From Ds==1 */
					rtw_secmicappend(&micdata, &pwlhdr[16], 6);
				else
					rtw_secmicappend(&micdata, &pwlhdr[10], 6);

			}

			if (pxframe->attrib.qos_en)
				priority[0] = (u8)pxframe->attrib.qos_en;

			rtw_secmicappend(&micdata, &priority[0], 4);

			payload = (u8 *)RND4((SIZE_PTR)(payload));
			rtw_secmicappend(&micdata, payload, payload_length);

			rtw_secgetmic(&micdata, &(mic[0]));
			/* add mic code  and add the mic code length in last_txcmdsz */

			_rtw_memcpy(pxframe->wltail[0]+pxframe->attrib.icv_len, &(mic[0]), 8);
		}
	}

	return _SUCCESS;
}

/*#define DBG_TX_SW_ENCRYPTOR*/

static s32 rtw_core_xmitframe_swencrypt(_adapter *padapter, struct xmit_frame *pxframe)
{
	if (pxframe->attrib.bswenc) {
#ifdef DBG_TX_SW_ENCRYPTOR
		RTW_INFO(ADPT_FMT" - sec_type:%s DO SW encryption\n",
			ADPT_ARG(padapter), security_type_str(pxframe->attrib.encrypt));
#endif

		switch (pxframe->attrib.encrypt) {
		case _WEP40_:
		case _WEP104_:
			//rtw_wep_encrypt(padapter, (u8 *)pxmitframe);
			break;
		case _TKIP_:
			//rtw_tkip_encrypt(padapter, (u8 *)pxmitframe);
			break;
		case _AES_:
		case _CCMP_256_:
			rtw_core_aes_encrypt(padapter, (u8 *)pxframe);
			break;
		case _GCMP_:
		case _GCMP_256_:
			//rtw_gcmp_encrypt(padapter, (u8 *)pxmitframe);
			break;
#ifdef CONFIG_WAPI_SUPPORT
		case _SMS4_:
			//rtw_sms4_encrypt(padapter, (u8 *)pxmitframe);
#endif
		default:
			break;
		}

	}
	return _SUCCESS;
}


s32 rtw_core_make_wlanhdr(_adapter *padapter, u8 *hdr, struct xmit_frame *pxframe)
{
	u16 *qc;

	struct rtw_ieee80211_hdr *pwlanhdr = (struct rtw_ieee80211_hdr *)hdr;
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	struct qos_priv *pqospriv = &pmlmepriv->qospriv;
	u8 qos_option = _FALSE;
	sint res = _SUCCESS;
	u16 *fctrl = &pwlanhdr->frame_ctl;

	_rtw_memset(hdr, 0, WLANHDR_OFFSET);

	set_frame_sub_type(fctrl, pxframe->attrib.subtype);

	if (pxframe->attrib.subtype & WIFI_DATA_TYPE) {
		if (MLME_IS_STA(padapter)) {
#ifdef CONFIG_TDLS
			if (pattrib->direct_link == _TRUE) {
				/* TDLS data transfer, ToDS=0, FrDs=0 */
				_rtw_memcpy(pwlanhdr->addr1, pxframe->attrib.dst, ETH_ALEN);
				_rtw_memcpy(pwlanhdr->addr2, pxframe->attrib.src, ETH_ALEN);
				_rtw_memcpy(pwlanhdr->addr3, get_bssid(pmlmepriv), ETH_ALEN);

				if (pxframe->attrib.qos_en)
					qos_option = _TRUE;
			} else
#endif /* CONFIG_TDLS */
			{
				/* to_ds = 1, fr_ds = 0; */
				/* 1.Data transfer to AP */
				/* 2.Arp pkt will relayed by AP */
				SetToDs(fctrl);
				_rtw_memcpy(pwlanhdr->addr1, get_bssid(pmlmepriv), ETH_ALEN);
				_rtw_memcpy(pwlanhdr->addr2, pxframe->attrib.ta, ETH_ALEN);
				_rtw_memcpy(pwlanhdr->addr3, pxframe->attrib.dst, ETH_ALEN);

				if (pqospriv->qos_option)
					qos_option = _TRUE;
			}
		} else if ((check_fwstate(pmlmepriv,  WIFI_AP_STATE) == _TRUE)) {
			/* to_ds = 0, fr_ds = 1; */
			SetFrDs(fctrl);
			_rtw_memcpy(pwlanhdr->addr1, pxframe->attrib.dst, ETH_ALEN);
			_rtw_memcpy(pwlanhdr->addr2, get_bssid(pmlmepriv), ETH_ALEN);
			_rtw_memcpy(pwlanhdr->addr3, pxframe->attrib.src, ETH_ALEN);

			if (pxframe->attrib.qos_en)
				qos_option = _TRUE;
		} else if ((check_fwstate(pmlmepriv, WIFI_ADHOC_STATE) == _TRUE) ||
			(check_fwstate(pmlmepriv, WIFI_ADHOC_MASTER_STATE) == _TRUE)) {
			_rtw_memcpy(pwlanhdr->addr1, pxframe->attrib.dst, ETH_ALEN);
			_rtw_memcpy(pwlanhdr->addr2, pxframe->attrib.ta, ETH_ALEN);
			_rtw_memcpy(pwlanhdr->addr3, get_bssid(pmlmepriv), ETH_ALEN);

			if (pxframe->attrib.qos_en)
				qos_option = _TRUE;
#ifdef CONFIG_RTW_MESH
		} else if (MLME_IS_STA(padapter)) {
			rtw_mesh_tx_build_whdr(padapter, pattrib, fctrl, pwlanhdr);
			if (pxframe->attrib.qos_en)
				qos_option = _TRUE;
			else {
				RTW_WARN("[%s] !qos_en in Mesh\n", __FUNCTION__);
				res = _FAIL;
				goto exit;
			}
#endif
		} else {
			res = _FAIL;
			goto exit;
		}

		if (pxframe->attrib.mdata)
			SetMData(fctrl);

		if (pxframe->attrib.encrypt)
			SetPrivacy(fctrl);

		if (qos_option) {
			qc = (unsigned short *)(hdr + pxframe->attrib.hdrlen - 2);

			if (pxframe->attrib.priority)
				SetPriority(qc, pxframe->attrib.priority);

			SetEOSP(qc, pxframe->attrib.eosp);

			SetAckpolicy(qc, pxframe->attrib.ack_policy);

			if (pxframe->attrib.amsdu)
				SetAMsdu(qc, pxframe->attrib.amsdu);
#ifdef CONFIG_RTW_MESH
			if (MLME_IS_MESH(padapter)) {
				/* active: don't care, light sleep: 0, deep sleep: 1*/
				set_mps_lv(qc, 0); //TBD

				/* TBD: temporary set (rspi, eosp) = (0, 1) which means End MPSP */
				set_rspi(qc, 0);
				SetEOSP(qc, 1);

				set_mctrl_present(qc, 1);
			}
#endif
		}

		/* TODO: fill HT Control Field */

		/* Update Seq Num will be handled by f/w */
		{
			struct sta_info *psta;
			psta = pxframe->attrib.psta;

			if (psta == NULL) {
				RTW_INFO("%s, psta==NUL\n", __func__);
				return _FAIL;
			}

			if (!(psta->state & WIFI_ASOC_STATE)) {
				RTW_INFO("%s, psta->state(0x%x) != WIFI_ASOC_STATE\n", __func__, psta->state);
				return _FAIL;
			}

			if (psta) {
				psta->sta_xmitpriv.txseq_tid[pxframe->attrib.priority]++;
				psta->sta_xmitpriv.txseq_tid[pxframe->attrib.priority] &= 0xFFF;
				pxframe->attrib.seqnum = psta->sta_xmitpriv.txseq_tid[pxframe->attrib.priority];

				SetSeqNum(hdr, pxframe->attrib.seqnum);

#ifdef CONFIG_80211N_HT
#if 0 /* move into update_attrib_phy_info(). */
				/* check if enable ampdu */
				if (pattrib->ht_en && psta->ampdu_priv.ampdu_enable) {
					if (psta->ampdu_priv.agg_enable_bitmap & BIT(pattrib->priority))
						pattrib->ampdu_en = _TRUE;
				}
#endif
				/* re-check if enable ampdu by BA_starting_seqctrl */
				if (pxframe->attrib.ampdu_en == _TRUE) {
					u16 tx_seq;

					tx_seq = psta->BA_starting_seqctrl[pxframe->attrib.priority & 0x0f];

					/* check BA_starting_seqctrl */
					if (SN_LESS(pxframe->attrib.seqnum, tx_seq)) {
						/* RTW_INFO("tx ampdu seqnum(%d) < tx_seq(%d)\n", pattrib->seqnum, tx_seq); */
						pxframe->attrib.ampdu_en = _FALSE;/* AGG BK */
					} else if (SN_EQUAL(pxframe->attrib.seqnum, tx_seq)) {
						psta->BA_starting_seqctrl[pxframe->attrib.priority & 0x0f] = (tx_seq + 1) & 0xfff;

						pxframe->attrib.ampdu_en = _TRUE;/* AGG EN */
					} else {
						/* RTW_INFO("tx ampdu over run\n"); */
						psta->BA_starting_seqctrl[pxframe->attrib.priority & 0x0f] = (pxframe->attrib.seqnum + 1) & 0xfff;
						pxframe->attrib.ampdu_en = _TRUE;/* AGG EN */
					}

				}
#endif /* CONFIG_80211N_HT */
			}
		}

	} else {

	}

exit:


	return res;
}





#endif

static void rtw_fill_htc_in_wlanhdr(_adapter *padapter, struct pkt_attrib *pattrib, u32 *phtc_buf)
{
#ifdef CONFIG_80211AX_HE
	rtw_he_fill_htc(padapter, pattrib, phtc_buf);
#endif
}

s32 rtw_make_wlanhdr(_adapter *padapter, u8 *hdr, struct pkt_attrib *pattrib)
{
	u16 *qc;
	u32 *htc = NULL;

	struct rtw_ieee80211_hdr *pwlanhdr = (struct rtw_ieee80211_hdr *)hdr;
	struct _ADAPTER_LINK *padapter_link = pattrib->adapter_link;
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	struct qos_priv *pqospriv = &padapter_link->mlmepriv.qospriv;
	u8 qos_option = _FALSE, htc_option = _FALSE;
	sint res = _SUCCESS;
	u16 *fctrl = &pwlanhdr->frame_ctl;

	/* struct sta_info *psta; */

	/* sint bmcst = IS_MCAST(pattrib->ra); */


	/*
		psta = rtw_get_stainfo(&padapter->stapriv, pattrib->ra);
		if(pattrib->psta != psta)
		{
			RTW_INFO("%s, pattrib->psta(%p) != psta(%p)\n", __func__, pattrib->psta, psta);
			return;
		}

		if(psta==NULL)
		{
			RTW_INFO("%s, psta==NUL\n", __func__);
			return _FAIL;
		}

		if(!(psta->state &WIFI_ASOC_STATE))
		{
			RTW_INFO("%s, psta->state(0x%x) != WIFI_ASOC_STATE\n", __func__, psta->state);
			return _FAIL;
		}
	*/

#ifdef RTW_PHL_TX
	_rtw_memset(hdr, 0, pattrib->hdrlen);
#else
	_rtw_memset(hdr, 0, WLANHDR_OFFSET);
#endif

	set_frame_sub_type(fctrl, pattrib->subtype);

	if (pattrib->subtype & WIFI_DATA_TYPE) {
		if (MLME_IS_STA(padapter)) {
#ifdef CONFIG_TDLS
			if (pattrib->direct_link == _TRUE) {
				/* TDLS data transfer, ToDS=0, FrDs=0 */
				_rtw_memcpy(pwlanhdr->addr1, pattrib->dst, ETH_ALEN);
				_rtw_memcpy(pwlanhdr->addr2, pattrib->src, ETH_ALEN);
				_rtw_memcpy(pwlanhdr->addr3, get_bssid(pmlmepriv), ETH_ALEN);

				if (pattrib->qos_en)
					qos_option = _TRUE;
			} else
#endif /* CONFIG_TDLS */
			{
				#ifdef CONFIG_RTW_WDS
				if (pattrib->wds) {
					SetToDs(fctrl);
					SetFrDs(fctrl);
					_rtw_memcpy(pwlanhdr->addr1, pattrib->ra, ETH_ALEN);
					_rtw_memcpy(pwlanhdr->addr2, pattrib->ta, ETH_ALEN);
					_rtw_memcpy(pwlanhdr->addr3, pattrib->dst, ETH_ALEN);
					_rtw_memcpy(pwlanhdr->addr4, pattrib->src, ETH_ALEN);
				} else
				#endif
				{
					/* to_ds = 1, fr_ds = 0; */
					/* 1.Data transfer to AP */
					/* 2.Arp pkt will relayed by AP */
					SetToDs(fctrl);
					_rtw_memcpy(pwlanhdr->addr1, get_bssid(pmlmepriv), ETH_ALEN);
					_rtw_memcpy(pwlanhdr->addr2, pattrib->ta, ETH_ALEN);
					_rtw_memcpy(pwlanhdr->addr3, pattrib->dst, ETH_ALEN);
				}

				if (pqospriv->qos_option)
					qos_option = _TRUE;
			}
		} else if ((check_fwstate(pmlmepriv,  WIFI_AP_STATE) == _TRUE)) {
			#ifdef CONFIG_RTW_WDS
			if (pattrib->wds) {
				SetToDs(fctrl);
				SetFrDs(fctrl);
				_rtw_memcpy(pwlanhdr->addr1, pattrib->ra, ETH_ALEN);
				_rtw_memcpy(pwlanhdr->addr2, pattrib->ta, ETH_ALEN);
				_rtw_memcpy(pwlanhdr->addr3, pattrib->dst, ETH_ALEN);
				_rtw_memcpy(pwlanhdr->addr4, pattrib->src, ETH_ALEN);
			} else
			#endif
			{
				/* to_ds = 0, fr_ds = 1; */
				SetFrDs(fctrl);
				_rtw_memcpy(pwlanhdr->addr1, pattrib->dst, ETH_ALEN);
				_rtw_memcpy(pwlanhdr->addr2, get_bssid(pmlmepriv), ETH_ALEN);
				_rtw_memcpy(pwlanhdr->addr3, pattrib->src, ETH_ALEN);
			}

			if (pattrib->qos_en)
				qos_option = _TRUE;
		} else if ((check_fwstate(pmlmepriv, WIFI_ADHOC_STATE) == _TRUE) ||
			(check_fwstate(pmlmepriv, WIFI_ADHOC_MASTER_STATE) == _TRUE)) {
			_rtw_memcpy(pwlanhdr->addr1, pattrib->dst, ETH_ALEN);
			_rtw_memcpy(pwlanhdr->addr2, pattrib->ta, ETH_ALEN);
			_rtw_memcpy(pwlanhdr->addr3, get_bssid(pmlmepriv), ETH_ALEN);

			if (pattrib->qos_en)
				qos_option = _TRUE;
#ifdef CONFIG_RTW_MESH
		} else if (check_fwstate(pmlmepriv, WIFI_MESH_STATE) == _TRUE) {
			rtw_mesh_tx_build_whdr(padapter, pattrib, fctrl, pwlanhdr);
			if (pattrib->qos_en)
				qos_option = _TRUE;
			else {
				RTW_WARN("[%s] !qos_en in Mesh\n", __FUNCTION__);
				res = _FAIL;
				goto exit;
			}
#endif
		} else {
			res = _FAIL;
			goto exit;
		}

		if (pattrib->mdata)
			SetMData(fctrl);

		if (pattrib->encrypt)
			SetPrivacy(fctrl);

		if (pattrib->order)
			htc_option = _TRUE;

		if (qos_option) {
			qc = (unsigned short *)(hdr + (XATTRIB_GET_WDS(pattrib) ? WLAN_HDR_A4_LEN : WLAN_HDR_A3_LEN));

			if (pattrib->priority)
				SetPriority(qc, pattrib->priority);

			SetEOSP(qc, pattrib->eosp);

			SetAckpolicy(qc, pattrib->ack_policy);

			if (pattrib->amsdu)
				SetAMsdu(qc, pattrib->amsdu);
#ifdef CONFIG_RTW_MESH
			if (MLME_IS_MESH(padapter)) {
				/* active: don't care, light sleep: 0, deep sleep: 1*/
				set_mps_lv(qc, 0); //TBD

				/* TBD: temporary set (rspi, eosp) = (0, 1) which means End MPSP */
				set_rspi(qc, 0);
				SetEOSP(qc, 1);

				set_mctrl_present(qc, 1);
			}
#endif
		}

		/* TODO: fill HT Control Field */
		if (htc_option == _TRUE) {
			set_htc_order_bit(fctrl);

			htc = (u32 *)(hdr + pattrib->hdrlen - 4);
			rtw_fill_htc_in_wlanhdr(padapter, pattrib, htc);
		}

		/* Update Seq Num will be handled by f/w */
		{
			struct sta_info *psta;
			psta = rtw_get_stainfo(&padapter->stapriv, pattrib->ra);
			if (pattrib->psta != psta) {
				RTW_INFO("%s, pattrib->psta(%p) != psta(%p)\n", __func__, pattrib->psta, psta);
				return _FAIL;
			}

			if (psta == NULL) {
				RTW_INFO("%s, psta==NUL\n", __func__);
				return _FAIL;
			}

			if (!(psta->state & WIFI_ASOC_STATE)) {
				RTW_INFO("%s, psta->state(0x%x) != WIFI_ASOC_STATE\n", __func__, psta->state);
				return _FAIL;
			}


			if (psta) {
				psta->sta_xmitpriv.txseq_tid[pattrib->priority]++;
				psta->sta_xmitpriv.txseq_tid[pattrib->priority] &= 0xFFF;
				pattrib->seqnum = psta->sta_xmitpriv.txseq_tid[pattrib->priority];

				SetSeqNum(hdr, pattrib->seqnum);

#ifdef CONFIG_80211N_HT
#if 0 /* move into update_attrib_phy_info(). */
				/* check if enable ampdu */
				if (pattrib->ht_en && psta->ampdu_priv.ampdu_enable) {
					if (psta->ampdu_priv.agg_enable_bitmap & BIT(pattrib->priority))
						pattrib->ampdu_en = _TRUE;
				}
#endif
				/* re-check if enable ampdu by BA_starting_seqctrl */
				if (pattrib->ampdu_en == _TRUE) {
					u16 tx_seq;

					tx_seq = psta->BA_starting_seqctrl[pattrib->priority & 0x0f];

					/* check BA_starting_seqctrl */
					if (SN_LESS(pattrib->seqnum, tx_seq)) {
						/* RTW_INFO("tx ampdu seqnum(%d) < tx_seq(%d)\n", pattrib->seqnum, tx_seq); */
						pattrib->ampdu_en = _FALSE;/* AGG BK */
					} else if (SN_EQUAL(pattrib->seqnum, tx_seq)) {
						psta->BA_starting_seqctrl[pattrib->priority & 0x0f] = (tx_seq + 1) & 0xfff;

						pattrib->ampdu_en = _TRUE;/* AGG EN */
					} else {
						/* RTW_INFO("tx ampdu over run\n"); */
						psta->BA_starting_seqctrl[pattrib->priority & 0x0f] = (pattrib->seqnum + 1) & 0xfff;
						pattrib->ampdu_en = _TRUE;/* AGG EN */
					}

				}
#endif /* CONFIG_80211N_HT */
			}
		}

	} else {

	}

exit:


	return res;
}

s32 rtw_txframes_pending(_adapter *padapter)
{
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;

	return ((_rtw_queue_empty(&pxmitpriv->be_pending) == _FALSE) ||
		(_rtw_queue_empty(&pxmitpriv->bk_pending) == _FALSE) ||
		(_rtw_queue_empty(&pxmitpriv->vi_pending) == _FALSE) ||
		(_rtw_queue_empty(&pxmitpriv->vo_pending) == _FALSE));
}

s32 rtw_txframes_sta_ac_pending(_adapter *padapter, struct pkt_attrib *pattrib)
{
	struct sta_info *psta;
	struct tx_servq *ptxservq;
	int priority = pattrib->priority;
	/*
		if(pattrib->psta)
		{
			psta = pattrib->psta;
		}
		else
		{
			RTW_INFO("%s, call rtw_get_stainfo()\n", __func__);
			psta=rtw_get_stainfo(&padapter->stapriv ,&pattrib->ra[0]);
		}
	*/
	psta = rtw_get_stainfo(&padapter->stapriv, pattrib->ra);
	if (pattrib->psta != psta) {
		RTW_INFO("%s, pattrib->psta(%p) != psta(%p)\n", __func__, pattrib->psta, psta);
		return 0;
	}

	if (psta == NULL) {
		RTW_INFO("%s, psta==NUL\n", __func__);
		return 0;
	}

	if (!(psta->state & WIFI_ASOC_STATE)) {
		RTW_INFO("%s, psta->state(0x%x) != WIFI_ASOC_STATE\n", __func__, psta->state);
		return 0;
	}

	switch (priority) {
	case 1:
	case 2:
		ptxservq = &(psta->sta_xmitpriv.bk_q);
		break;
	case 4:
	case 5:
		ptxservq = &(psta->sta_xmitpriv.vi_q);
		break;
	case 6:
	case 7:
		ptxservq = &(psta->sta_xmitpriv.vo_q);
		break;
	case 0:
	case 3:
	default:
		ptxservq = &(psta->sta_xmitpriv.be_q);
		break;

	}

	return ptxservq->qcnt;
}

#ifdef CONFIG_TDLS

int rtw_build_tdls_ies(_adapter *padapter, struct xmit_frame *pxmitframe, u8 *pframe, struct tdls_txmgmt *ptxmgmt)
{
	struct pkt_attrib *pattrib = &pxmitframe->attrib;
	struct sta_info *ptdls_sta = NULL;
	int res = _SUCCESS;

	ptdls_sta = rtw_get_stainfo((&padapter->stapriv), pattrib->dst);
	if (ptdls_sta == NULL) {
		switch (ptxmgmt->action_code) {
		case TDLS_DISCOVERY_REQUEST:
		case TUNNELED_PROBE_REQ:
		case TUNNELED_PROBE_RSP:
			break;
		default:
			RTW_INFO("[TDLS] %s - Direct Link Peer = "MAC_FMT" not found for action = %d\n", __func__, MAC_ARG(pattrib->dst), ptxmgmt->action_code);
			res = _FAIL;
			goto exit;
		}
	}

	switch (ptxmgmt->action_code) {
	case TDLS_SETUP_REQUEST:
		rtw_build_tdls_setup_req_ies(padapter, pxmitframe, pframe, ptxmgmt, ptdls_sta);
		break;
	case TDLS_SETUP_RESPONSE:
		rtw_build_tdls_setup_rsp_ies(padapter, pxmitframe, pframe, ptxmgmt, ptdls_sta);
		break;
	case TDLS_SETUP_CONFIRM:
		rtw_build_tdls_setup_cfm_ies(padapter, pxmitframe, pframe, ptxmgmt, ptdls_sta);
		break;
	case TDLS_TEARDOWN:
		rtw_build_tdls_teardown_ies(padapter, pxmitframe, pframe, ptxmgmt, ptdls_sta);
		break;
	case TDLS_DISCOVERY_REQUEST:
		rtw_build_tdls_dis_req_ies(padapter, pxmitframe, pframe, ptxmgmt);
		break;
	case TDLS_PEER_TRAFFIC_INDICATION:
		rtw_build_tdls_peer_traffic_indication_ies(padapter, pxmitframe, pframe, ptxmgmt, ptdls_sta);
		break;
#ifdef CONFIG_TDLS_CH_SW
	case TDLS_CHANNEL_SWITCH_REQUEST:
		rtw_build_tdls_ch_switch_req_ies(padapter, pxmitframe, pframe, ptxmgmt, ptdls_sta);
		break;
	case TDLS_CHANNEL_SWITCH_RESPONSE:
		rtw_build_tdls_ch_switch_rsp_ies(padapter, pxmitframe, pframe, ptxmgmt, ptdls_sta);
		break;
#endif
	case TDLS_PEER_TRAFFIC_RESPONSE:
		rtw_build_tdls_peer_traffic_rsp_ies(padapter, pxmitframe, pframe, ptxmgmt, ptdls_sta);
		break;
#ifdef CONFIG_WFD
	case TUNNELED_PROBE_REQ:
		rtw_build_tunneled_probe_req_ies(padapter, pxmitframe, pframe);
		break;
	case TUNNELED_PROBE_RSP:
		rtw_build_tunneled_probe_rsp_ies(padapter, pxmitframe, pframe);
		break;
#endif /* CONFIG_WFD */
	default:
		res = _FAIL;
		break;
	}

exit:
	return res;
}

s32 rtw_make_tdls_wlanhdr(_adapter *padapter, u8 *hdr, struct pkt_attrib *pattrib, struct tdls_txmgmt *ptxmgmt)
{
	u16 *qc;
	struct rtw_ieee80211_hdr *pwlanhdr = (struct rtw_ieee80211_hdr *)hdr;
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	struct _ADAPTER_LINK *padapter_link = pattrib->adapter_link;
	struct qos_priv *pqospriv = &padapter_link->mlmepriv.qospriv;
	struct sta_priv	*pstapriv = &padapter->stapriv;
	struct sta_info *psta = NULL, *ptdls_sta = NULL;
	u8 tdls_seq = 0, baddr[ETH_ALEN] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

	sint res = _SUCCESS;
	u16 *fctrl = &pwlanhdr->frame_ctl;


	_rtw_memset(hdr, 0, WLANHDR_OFFSET);

	set_frame_sub_type(fctrl, pattrib->subtype);

	switch (ptxmgmt->action_code) {
	case TDLS_SETUP_REQUEST:
	case TDLS_SETUP_RESPONSE:
	case TDLS_SETUP_CONFIRM:
	case TDLS_PEER_TRAFFIC_INDICATION:
	case TDLS_PEER_PSM_REQUEST:
	case TUNNELED_PROBE_REQ:
	case TUNNELED_PROBE_RSP:
	case TDLS_DISCOVERY_REQUEST:
		SetToDs(fctrl);
		_rtw_memcpy(pwlanhdr->addr1, get_bssid(pmlmepriv), ETH_ALEN);
		_rtw_memcpy(pwlanhdr->addr2, pattrib->src, ETH_ALEN);
		_rtw_memcpy(pwlanhdr->addr3, pattrib->dst, ETH_ALEN);
		break;
	case TDLS_CHANNEL_SWITCH_REQUEST:
	case TDLS_CHANNEL_SWITCH_RESPONSE:
	case TDLS_PEER_PSM_RESPONSE:
	case TDLS_PEER_TRAFFIC_RESPONSE:
		_rtw_memcpy(pwlanhdr->addr1, pattrib->dst, ETH_ALEN);
		_rtw_memcpy(pwlanhdr->addr2, pattrib->src, ETH_ALEN);
		_rtw_memcpy(pwlanhdr->addr3, get_bssid(pmlmepriv), ETH_ALEN);
		tdls_seq = 1;
		break;
	case TDLS_TEARDOWN:
		if (ptxmgmt->status_code == _RSON_TDLS_TEAR_UN_RSN_) {
			_rtw_memcpy(pwlanhdr->addr1, pattrib->dst, ETH_ALEN);
			_rtw_memcpy(pwlanhdr->addr2, pattrib->src, ETH_ALEN);
			_rtw_memcpy(pwlanhdr->addr3, get_bssid(pmlmepriv), ETH_ALEN);
			tdls_seq = 1;
		} else {
			SetToDs(fctrl);
			_rtw_memcpy(pwlanhdr->addr1, get_bssid(pmlmepriv), ETH_ALEN);
			_rtw_memcpy(pwlanhdr->addr2, pattrib->src, ETH_ALEN);
			_rtw_memcpy(pwlanhdr->addr3, pattrib->dst, ETH_ALEN);
		}
		break;
	}

	if (pattrib->encrypt)
		SetPrivacy(fctrl);

	if (ptxmgmt->action_code == TDLS_PEER_TRAFFIC_RESPONSE)
		SetPwrMgt(fctrl);

	if (pqospriv->qos_option) {
		qc = (unsigned short *)(hdr + pattrib->hdrlen - 2);
		if (pattrib->priority)
			SetPriority(qc, pattrib->priority);
		SetAckpolicy(qc, pattrib->ack_policy);
	}

	psta = pattrib->psta;

	/* 1. update seq_num per link by sta_info */
	/* 2. rewrite encrypt to _AES_, also rewrite iv_len, icv_len */
	if (tdls_seq == 1) {
		ptdls_sta = rtw_get_stainfo(pstapriv, pattrib->dst);
		if (ptdls_sta) {
			ptdls_sta->sta_xmitpriv.txseq_tid[pattrib->priority]++;
			ptdls_sta->sta_xmitpriv.txseq_tid[pattrib->priority] &= 0xFFF;
			pattrib->seqnum = ptdls_sta->sta_xmitpriv.txseq_tid[pattrib->priority];
			SetSeqNum(hdr, pattrib->seqnum);

			if (pattrib->encrypt) {
				pattrib->encrypt = _AES_;
				pattrib->iv_len = 8;
				pattrib->icv_len = 8;
				pattrib->bswenc = _FALSE;
			}
			pattrib->mac_id = ptdls_sta->phl_sta->macid;
		} else {
			res = _FAIL;
			goto exit;
		}
	} else if (psta) {
		psta->sta_xmitpriv.txseq_tid[pattrib->priority]++;
		psta->sta_xmitpriv.txseq_tid[pattrib->priority] &= 0xFFF;
		pattrib->seqnum = psta->sta_xmitpriv.txseq_tid[pattrib->priority];
		SetSeqNum(hdr, pattrib->seqnum);
	}


exit:


	return res;
}

s32 rtw_xmit_tdls_coalesce(_adapter *padapter, struct xmit_frame *pxmitframe, struct tdls_txmgmt *ptxmgmt)
{
	s32 llc_sz;

	u8 *pframe, *mem_start;

	struct sta_info		*psta;
	struct sta_priv		*pstapriv = &padapter->stapriv;
	struct pkt_attrib	*pattrib = &pxmitframe->attrib;
	struct _ADAPTER_LINK *padapter_link = pattrib->adapter_link;
	struct link_mlme_priv	*pmlmepriv = &padapter_link->mlmepriv;
	u8 *pbuf_start;
	s32 bmcst = IS_MCAST(pattrib->ra);
	s32 res = _SUCCESS;


	if (pattrib->psta)
		psta = pattrib->psta;
	else {
		if (bmcst)
			psta = rtw_get_bcmc_stainfo(padapter, padapter_link);
		else
			psta = rtw_get_stainfo(&padapter->stapriv, pattrib->ra);
	}

	if (psta == NULL) {
		res = _FAIL;
		goto exit;
	}

	if (pxmitframe->buf_addr == NULL) {
		res = _FAIL;
		goto exit;
	}

	pbuf_start = pxmitframe->buf_addr;
	mem_start = pbuf_start + TXDESC_OFFSET;

	if (rtw_make_tdls_wlanhdr(padapter, mem_start, pattrib, ptxmgmt) == _FAIL) {
		res = _FAIL;
		goto exit;
	}

	pframe = mem_start;
	pframe += pattrib->hdrlen;

	/* adding icv, if necessary... */
	if (pattrib->iv_len) {
		if (psta != NULL) {
			switch (pattrib->encrypt) {
			case _WEP40_:
			case _WEP104_:
				WEP_IV(pattrib->iv, psta->dot11txpn, pattrib->key_idx);
				break;
			case _TKIP_:
				if (bmcst)
					TKIP_IV(pattrib->iv, psta->dot11txpn, pattrib->key_idx);
				else
					TKIP_IV(pattrib->iv, psta->dot11txpn, 0);
				break;
			case _AES_:
				if (bmcst)
					AES_IV(pattrib->iv, psta->dot11txpn, pattrib->key_idx);
				else
					AES_IV(pattrib->iv, psta->dot11txpn, 0);
				break;
			}
		}

		_rtw_memcpy(pframe, pattrib->iv, pattrib->iv_len);
		pframe += pattrib->iv_len;

	}

	llc_sz = rtw_put_snap(pframe, pattrib->ether_type);
	pframe += llc_sz;

	/* pattrib->pktlen will be counted in rtw_build_tdls_ies */
	pattrib->pktlen = 0;

	rtw_build_tdls_ies(padapter, pxmitframe, pframe, ptxmgmt);

	if ((pattrib->icv_len > 0) && (pattrib->bswenc)) {
		pframe += pattrib->pktlen;
		_rtw_memcpy(pframe, pattrib->icv, pattrib->icv_len);
		pframe += pattrib->icv_len;
	}

	pattrib->nr_frags = 1;
	pattrib->last_txcmdsz = pattrib->hdrlen + pattrib->iv_len + llc_sz +
		((pattrib->bswenc) ? pattrib->icv_len : 0) + pattrib->pktlen;

	if (xmitframe_addmic(padapter, pxmitframe) == _FAIL) {
		res = _FAIL;
		goto exit;
	}

	update_attrib_vcs_info(padapter, pxmitframe);

	pattrib->pktlen = pattrib->last_txcmdsz;

exit:


	return res;
}
#endif /* CONFIG_TDLS */

/*
 * Calculate wlan 802.11 packet MAX size from pkt_attrib
 * This function doesn't consider fragment case
 */
u32 rtw_calculate_wlan_pkt_size_by_attribue(struct pkt_attrib *pattrib)
{
	u32	len = 0;

	len = pattrib->hdrlen /* WLAN Header */
		+ pattrib->iv_len /* IV */
		+ XATTRIB_GET_MCTRL_LEN(pattrib)
		+ SNAP_SIZE + sizeof(u16) /* LLC */
		+ pattrib->pktlen
		+ (pattrib->encrypt == _TKIP_ ? 8 : 0) /* MIC */
		+ (pattrib->bswenc ? pattrib->icv_len : 0) /* ICV */
		;

	return len;
}

#ifdef CONFIG_TX_AMSDU
s32 check_amsdu(struct xmit_frame *pxmitframe)
{
	struct pkt_attrib *pattrib;
	struct sta_info *psta = NULL;
	s32 ret = _TRUE;

	if (!pxmitframe)
		ret = _FALSE;

	pattrib = &pxmitframe->attrib;

	psta = rtw_get_stainfo(&pxmitframe->padapter->stapriv, &pattrib->ra[0]);
	if (psta) {
		if (psta->flags & WLAN_STA_AMSDU_DISABLE)
			ret =_FALSE;
	}

	if (IS_MCAST(pattrib->ra))
		ret = _FALSE;

	if ((pattrib->ether_type == 0x888e) ||
		(pattrib->ether_type == 0x0806) ||
		(pattrib->ether_type == 0x88b4) ||
		(pattrib->dhcp_pkt == 1))
		ret = _FALSE;

	if ((pattrib->encrypt == _WEP40_) ||
	    (pattrib->encrypt == _WEP104_) ||
	    (pattrib->encrypt == _TKIP_))
		ret = _FALSE;

	if (!pattrib->qos_en)
		ret = _FALSE;

	if (IS_AMSDU_AMPDU_NOT_VALID(pattrib))
		ret = _FALSE;

	return ret;
}

s32 check_amsdu_tx_support(_adapter *padapter)
{
	struct dvobj_priv *pdvobjpriv;
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);
	struct link_mlme_ext_priv *pmlmeext = &padapter_link->mlmeextpriv;
	int tx_amsdu;
	int tx_amsdu_rate;
	int current_tx_rate;
	s32 ret = _FALSE;

	if(pmlmeext->cur_wireless_mode < WLAN_MD_11N)
		return _FALSE;

	pdvobjpriv = adapter_to_dvobj(padapter);
	tx_amsdu = padapter->tx_amsdu;
	tx_amsdu_rate = padapter->tx_amsdu_rate;
	current_tx_rate = pdvobjpriv->traffic_stat.cur_tx_tp;

	if (tx_amsdu == 1)
		ret = _TRUE;
	else if (tx_amsdu >= 2 && (tx_amsdu_rate == 0 || current_tx_rate > tx_amsdu_rate))
		ret = _TRUE;
	else
		ret = _FALSE;

	return ret;
}

s32 rtw_xmitframe_coalesce_amsdu(_adapter *padapter, struct xmit_frame *pxmitframe, struct xmit_frame *pxmitframe_queue)
{

	struct pkt_file pktfile;
	struct pkt_attrib *pattrib;
	struct sk_buff *pkt;

	struct pkt_file pktfile_queue;
	struct pkt_attrib *pattrib_queue;
	struct sk_buff *pkt_queue;

	s32 llc_sz, mem_sz;

	s32 padding = 0;

	u8 *pframe, *mem_start;
	u8 hw_hdr_offset;

	u16 *len;
	u8 *pbuf_start;
	s32 res = _SUCCESS;

	if (pxmitframe->buf_addr == NULL) {
		RTW_INFO("==> %s buf_addr==NULL\n", __FUNCTION__);
		return _FAIL;
	}


	pbuf_start = pxmitframe->buf_addr;

#ifdef CONFIG_USB_TX_AGGREGATION
	hw_hdr_offset =  TXDESC_SIZE + (pxmitframe->pkt_offset * PACKET_OFFSET_SZ);
#else
#ifdef CONFIG_TX_EARLY_MODE /* for SDIO && Tx Agg */
	hw_hdr_offset = TXDESC_OFFSET + EARLY_MODE_INFO_SIZE;
#else
	hw_hdr_offset = TXDESC_OFFSET;
#endif
#endif

	mem_start = pbuf_start + hw_hdr_offset; //for DMA

	pattrib = &pxmitframe->attrib;

	pattrib->amsdu = 1;

	if (rtw_make_wlanhdr(padapter, mem_start, pattrib) == _FAIL) {
		RTW_INFO("%s: rtw_make_wlanhdr fail; drop pkt\n", __func__);
		res = _FAIL;
		goto exit;
	}

	llc_sz = 0;

	pframe = mem_start;

	//SetMFrag(mem_start);
	ClearMFrag(mem_start);

	pframe += pattrib->hdrlen;

	/* adding icv, if necessary... */
	if (pattrib->iv_len) {
		update_attrib_sec_iv_info(padapter, pattrib);
		_rtw_memcpy(pframe, pattrib->iv, pattrib->iv_len); // queue or new?

		RTW_DBG("%s: keyid=%d pattrib->iv[3]=%.2x pframe=%.2x %.2x %.2x %.2x\n",
			__func__, padapter->securitypriv.dot11PrivacyKeyIndex,
			pattrib->iv[3], *pframe, *(pframe + 1), *(pframe + 2), *(pframe + 3));

		pframe += pattrib->iv_len;
	}

	pattrib->last_txcmdsz = pattrib->hdrlen + pattrib->iv_len;

	if (pxmitframe_queue) {
		pattrib_queue = &pxmitframe_queue->attrib;
		pkt_queue = pxmitframe_queue->pkt;

		_rtw_open_pktfile(pkt_queue, &pktfile_queue);
		_rtw_pktfile_read(&pktfile_queue, NULL, pattrib_queue->pkt_hdrlen);

		#ifdef CONFIG_RTW_MESH
		if (MLME_IS_MESH(padapter)) {
			/* mDA(6), mSA(6), len(2), mctrl */
			_rtw_memcpy(pframe, pattrib_queue->mda, ETH_ALEN);
			pframe += ETH_ALEN;
			_rtw_memcpy(pframe, pattrib_queue->msa, ETH_ALEN);
			pframe += ETH_ALEN;
			len = (u16 *)pframe;
			pframe += 2;
			rtw_mesh_tx_build_mctrl(padapter, pattrib_queue, pframe);
			pframe += XATTRIB_GET_MCTRL_LEN(pattrib_queue);
		} else
		#endif
		{
			/* 802.3 MAC Header DA(6)  SA(6)  Len(2)*/
			_rtw_memcpy(pframe, pattrib_queue->dst, ETH_ALEN);
			pframe += ETH_ALEN;
			_rtw_memcpy(pframe, pattrib_queue->src, ETH_ALEN);
			pframe += ETH_ALEN;
			len = (u16 *)pframe;
			pframe += 2;
		}

		llc_sz = rtw_put_snap(pframe, pattrib_queue->ether_type);
		pframe += llc_sz;

		mem_sz = _rtw_pktfile_read(&pktfile_queue, pframe, pattrib_queue->pktlen);
		pframe += mem_sz;

		*len = htons(XATTRIB_GET_MCTRL_LEN(pattrib_queue) + llc_sz + mem_sz);

		//calc padding
		padding = 4 - ((ETH_HLEN + XATTRIB_GET_MCTRL_LEN(pattrib_queue) + llc_sz + mem_sz) & (4-1));
		if (padding == 4)
			padding = 0;

		//_rtw_memset(pframe,0xaa, padding);
		pframe += padding;

		pattrib->last_txcmdsz += ETH_HLEN + XATTRIB_GET_MCTRL_LEN(pattrib_queue) + llc_sz + mem_sz + padding ;
	}

	//2nd mpdu

	pkt = pxmitframe->pkt;
	_rtw_open_pktfile(pkt, &pktfile);
	_rtw_pktfile_read(&pktfile, NULL, pattrib->pkt_hdrlen);

#ifdef CONFIG_RTW_MESH
	if (MLME_IS_MESH(padapter)) {
		/* mDA(6), mSA(6), len(2), mctrl */
		_rtw_memcpy(pframe, pattrib->mda, ETH_ALEN);
		pframe += ETH_ALEN;
		_rtw_memcpy(pframe, pattrib->msa, ETH_ALEN);
		pframe += ETH_ALEN;
		len = (u16 *)pframe;
		pframe += 2;
		rtw_mesh_tx_build_mctrl(padapter, pattrib, pframe);
		pframe += XATTRIB_GET_MCTRL_LEN(pattrib);
	} else
#endif
	{
		/* 802.3 MAC Header  DA(6)  SA(6)  Len(2) */
		_rtw_memcpy(pframe, pattrib->dst, ETH_ALEN);
		pframe += ETH_ALEN;
		_rtw_memcpy(pframe, pattrib->src, ETH_ALEN);
		pframe += ETH_ALEN;
		len = (u16 *)pframe;
		pframe += 2;
	}

	llc_sz = rtw_put_snap(pframe, pattrib->ether_type);
	pframe += llc_sz;

	mem_sz = _rtw_pktfile_read(&pktfile, pframe, pattrib->pktlen);

	pframe += mem_sz;

	*len = htons(XATTRIB_GET_MCTRL_LEN(pattrib) + llc_sz + mem_sz);

	//the last ampdu has no padding
	padding = 0;

	pattrib->nr_frags = 1;

	pattrib->last_txcmdsz += ETH_HLEN + XATTRIB_GET_MCTRL_LEN(pattrib) + llc_sz + mem_sz + padding +
		((pattrib->bswenc) ? pattrib->icv_len : 0) ;

	if ((pattrib->icv_len > 0) && (pattrib->bswenc)) {
		_rtw_memcpy(pframe, pattrib->icv, pattrib->icv_len);
		pframe += pattrib->icv_len;
	}

	if (xmitframe_addmic(padapter, pxmitframe) == _FAIL) {
		RTW_INFO("xmitframe_addmic(padapter, pxmitframe)==_FAIL\n");
		res = _FAIL;
		goto exit;
	}

	xmitframe_swencrypt(padapter, pxmitframe);

	update_attrib_vcs_info(padapter, pxmitframe);

exit:
	return res;
}
#endif /* CONFIG_TX_AMSDU */

/*

This sub-routine will perform all the following:

1. remove 802.3 header.
2. create wlan_header, based on the info in pxmitframe
3. append sta's iv/ext-iv
4. append LLC
5. move frag chunk from pframe to pxmitframe->mem
6. apply sw-encrypt, if necessary.

*/
s32 rtw_xmitframe_coalesce(_adapter *padapter, struct sk_buff *pkt, struct xmit_frame *pxmitframe)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	struct pkt_file pktfile;

	s32 frg_inx, frg_len, mpdu_len, llc_sz, mem_sz;

	SIZE_PTR addr;

	u8 *pframe, *mem_start;
	u8 hw_hdr_offset;

	/* struct sta_info		*psta; */
	/* struct sta_priv		*pstapriv = &padapter->stapriv; */
	/* struct mlme_priv	*pmlmepriv = &padapter->mlmepriv; */
	struct xmit_priv	*pxmitpriv = &padapter->xmitpriv;

	struct pkt_attrib	*pattrib = &pxmitframe->attrib;

	u8 *pbuf_start;

	s32 bmcst = IS_MCAST(pattrib->ra);
	s32 res = _SUCCESS;


	/*
		if (pattrib->psta)
		{
			psta = pattrib->psta;
		} else
		{
			RTW_INFO("%s, call rtw_get_stainfo()\n", __func__);
			psta = rtw_get_stainfo(&padapter->stapriv, pattrib->ra);
		}

		if(psta==NULL)
		{

			RTW_INFO("%s, psta==NUL\n", __func__);
			return _FAIL;
		}


		if(!(psta->state &WIFI_ASOC_STATE))
		{
			RTW_INFO("%s, psta->state(0x%x) != WIFI_ASOC_STATE\n", __func__, psta->state);
			return _FAIL;
		}
	*/
	if (pxmitframe->buf_addr == NULL) {
		RTW_INFO("==> %s buf_addr==NULL\n", __FUNCTION__);
		return _FAIL;
	}

	pbuf_start = pxmitframe->buf_addr;

#if 0
#ifdef CONFIG_USB_TX_AGGREGATION
	hw_hdr_offset =  TXDESC_SIZE + (pxmitframe->pkt_offset * PACKET_OFFSET_SZ);
#else
#ifdef CONFIG_TX_EARLY_MODE /* for SDIO && Tx Agg */
	hw_hdr_offset = TXDESC_OFFSET + EARLY_MODE_INFO_SIZE;
#else
	hw_hdr_offset = TXDESC_OFFSET;
#endif
#endif
#endif
	hw_hdr_offset = rtw_hal_get_txdesc_len(GET_PHL_COM(dvobj), pattrib); /*FPGA_test*/

	mem_start = pbuf_start +	hw_hdr_offset;

	if (rtw_make_wlanhdr(padapter, mem_start, pattrib) == _FAIL) {
		RTW_INFO("%s: rtw_make_wlanhdr fail; drop pkt\n", __func__);
		res = _FAIL;
		goto exit;
	}

	_rtw_open_pktfile(pkt, &pktfile);
	_rtw_pktfile_read(&pktfile, NULL, pattrib->pkt_hdrlen);

	frg_inx = 0;
	frg_len = pxmitpriv->frag_len - 4;/* 2346-4 = 2342 */

	while (1) {
		llc_sz = 0;

		mpdu_len = frg_len;

		pframe = mem_start;

		SetMFrag(mem_start);

		pframe += pattrib->hdrlen;
		mpdu_len -= pattrib->hdrlen;

		/* adding icv, if necessary... */
		if (pattrib->iv_len) {
			update_attrib_sec_iv_info(padapter, pattrib);
			_rtw_memcpy(pframe, pattrib->iv, pattrib->iv_len);


			pframe += pattrib->iv_len;

			mpdu_len -= pattrib->iv_len;
		}

		if (frg_inx == 0) {
			#ifdef CONFIG_RTW_MESH
			if (MLME_IS_MESH(padapter)) {
				rtw_mesh_tx_build_mctrl(padapter, pattrib, pframe);
				pframe += XATTRIB_GET_MCTRL_LEN(pattrib);
				mpdu_len -= XATTRIB_GET_MCTRL_LEN(pattrib);
			}
			#endif

			llc_sz = rtw_put_snap(pframe, pattrib->ether_type);
			pframe += llc_sz;
			mpdu_len -= llc_sz;
		}

		if ((pattrib->icv_len > 0) && (pattrib->bswenc))
			mpdu_len -= pattrib->icv_len;


		if (bmcst) {
			/* don't do fragment to broadcat/multicast packets */
			mem_sz = _rtw_pktfile_read(&pktfile, pframe, pattrib->pktlen);
		} else
			mem_sz = _rtw_pktfile_read(&pktfile, pframe, mpdu_len);

		pframe += mem_sz;

		if ((pattrib->icv_len > 0) && (pattrib->bswenc)) {
			_rtw_memcpy(pframe, pattrib->icv, pattrib->icv_len);
			pframe += pattrib->icv_len;
		}

		frg_inx++;

		if (bmcst || (rtw_endofpktfile(&pktfile) == _TRUE)) {
			pattrib->nr_frags = frg_inx;

			pattrib->last_txcmdsz = pattrib->hdrlen + pattrib->iv_len +
				((pattrib->nr_frags == 1) ? (XATTRIB_GET_MCTRL_LEN(pattrib) + llc_sz) : 0) +
				((pattrib->bswenc) ? pattrib->icv_len : 0) + mem_sz;

			ClearMFrag(mem_start);

			break;
		}

		addr = (SIZE_PTR)(pframe);

		mem_start = (unsigned char *)RND4(addr) + hw_hdr_offset;
		_rtw_memcpy(mem_start, pbuf_start + hw_hdr_offset, pattrib->hdrlen);

	}

	if (xmitframe_addmic(padapter, pxmitframe) == _FAIL) {
		RTW_INFO("xmitframe_addmic(padapter, pxmitframe)==_FAIL\n");
		res = _FAIL;
		goto exit;
	}

	xmitframe_swencrypt(padapter, pxmitframe);

	if (bmcst == _FALSE)
		update_attrib_vcs_info(padapter, pxmitframe);
	else
		pattrib->vcs_mode = NONE_VCS;

exit:


	return res;
}

#if defined(CONFIG_IEEE80211W) || defined(CONFIG_RTW_MESH)
/*
 * CCMP encryption for unicast robust mgmt frame and broadcast group privicy action
 * BIP for broadcast robust mgmt frame
 */
s32 rtw_mgmt_xmitframe_coalesce(_adapter *padapter, struct sk_buff *pkt, struct xmit_frame *pxmitframe)
{
#define DBG_MGMT_XMIT_COALESEC_DUMP 0
#define DBG_MGMT_XMIT_BIP_DUMP 0
#define DBG_MGMT_XMIT_ENC_DUMP 0

	struct pkt_file pktfile;
	s32 frg_inx, frg_len, mpdu_len, llc_sz, mem_sz;
	SIZE_PTR addr;
	u8 *pframe, *mem_start = NULL, *tmp_buf = NULL;
	u8 hw_hdr_offset, subtype ;
	u8 category = 0xFF;
	struct sta_info		*psta = NULL;
	struct xmit_priv	*pxmitpriv = &padapter->xmitpriv;
	struct pkt_attrib	*pattrib = &pxmitframe->attrib;
	u8 *pbuf_start;
	s32 bmcst = IS_MCAST(pattrib->ra);
	s32 res = _FAIL;
	u8 *BIP_AAD = NULL;
	u8 *MGMT_body = NULL;

	struct mlme_ext_priv	*pmlmeext = &padapter->mlmeextpriv;
	struct mlme_priv	*pmlmepriv = &padapter->mlmepriv;
	struct rtw_ieee80211_hdr	*pwlanhdr;
	u8 mme_cont[_MME_IE_LENGTH_ - 2];
	u8 mme_clen;

	u32	ori_len;
	union pn48 *pn = NULL;
	enum security_type cipher = _NO_PRIVACY_;
	u8 kid;
	struct _ADAPTER_LINK *padapter_link = pattrib->adapter_link;
	u8 hw_security_zero_hdrlen = _FALSE;

	if (pxmitframe->buf_addr == NULL) {
		RTW_WARN(FUNC_ADPT_FMT" pxmitframe->buf_addr\n"
			, FUNC_ADPT_ARG(padapter));
		return _FAIL;
	}

	mem_start = pframe = (u8 *)(pxmitframe->buf_addr) + TXDESC_OFFSET;
	pwlanhdr = (struct rtw_ieee80211_hdr *)pframe;
	subtype = get_frame_sub_type(pframe); /* bit(7)~bit(2) */

	/* check if robust mgmt frame */
	if (subtype != WIFI_DEAUTH && subtype != WIFI_DISASSOC && subtype != WIFI_ACTION)
		return _SUCCESS;
	if (subtype == WIFI_ACTION) {
		category = *(pframe + sizeof(struct rtw_ieee80211_hdr_3addr));
		if (CATEGORY_IS_NON_ROBUST(category))
			return _SUCCESS;
	}
	if (!bmcst) {
		if (pattrib->psta)
			psta = pattrib->psta;
		else
			pattrib->psta = psta = rtw_get_stainfo(&padapter->stapriv, pattrib->ra);
		if (psta == NULL) {
			RTW_INFO(FUNC_ADPT_FMT" unicast sta == NULL\n", FUNC_ADPT_ARG(padapter));
			return _FAIL;
		}
		if (!(psta->flags & WLAN_STA_MFP)) {
			/* peer is not MFP capable, no need to encrypt */
			return _SUCCESS;
		}
		if (psta->bpairwise_key_installed != _TRUE) {
			RTW_INFO(FUNC_ADPT_FMT" PTK is not installed\n"
				, FUNC_ADPT_ARG(padapter));
			return _FAIL;
		}
	}

	ori_len = BIP_AAD_SIZE + pattrib->pktlen + _MME_IE_LENGTH_;
	tmp_buf = BIP_AAD = rtw_zmalloc(ori_len);
	if (BIP_AAD == NULL)
		return _FAIL;

	_rtw_spinlock_bh(&padapter->security_key_mutex);

	if (bmcst) {
		if (subtype == WIFI_ACTION && CATEGORY_IS_GROUP_PRIVACY(category)) {
			/* broadcast group privacy action frame */
			#if DBG_MGMT_XMIT_COALESEC_DUMP
			RTW_INFO(FUNC_ADPT_FMT" broadcast gp action(%u)\n"
				, FUNC_ADPT_ARG(padapter), category);
			#endif

			if (pattrib->psta)
				psta = pattrib->psta;
			else
				pattrib->psta = psta = rtw_get_bcmc_stainfo(padapter, padapter_link);
			if (psta == NULL) {
				RTW_INFO(FUNC_ADPT_FMT" broadcast sta == NULL\n"
					, FUNC_ADPT_ARG(padapter));
				goto xmitframe_coalesce_fail;
			}
			if (padapter_link->securitypriv.binstallGrpkey != _TRUE) {
				RTW_INFO(FUNC_ADPT_FMT" GTK is not installed\n"
					, FUNC_ADPT_ARG(padapter));
				goto xmitframe_coalesce_fail;
			}

			pn = &psta->dot11txpn;
			cipher = padapter->securitypriv.dot118021XGrpPrivacy;
			kid = padapter_link->securitypriv.dot118021XGrpKeyid;
		} else {
			#ifdef CONFIG_IEEE80211W
			/* broadcast robust mgmt frame, using BIP */
			int frame_body_len;
			u8 mic[16];

			/* IGTK key is not install ex: mesh MFP without IGTK */
			if (SEC_IS_BIP_KEY_INSTALLED(&padapter_link->securitypriv) != _TRUE)
				goto xmitframe_coalesce_success;

			#if DBG_MGMT_XMIT_COALESEC_DUMP
			if (subtype == WIFI_DEAUTH)
				RTW_INFO(FUNC_ADPT_FMT" braodcast deauth\n", FUNC_ADPT_ARG(padapter));
			else if (subtype == WIFI_DISASSOC)
				RTW_INFO(FUNC_ADPT_FMT" braodcast disassoc\n", FUNC_ADPT_ARG(padapter));
			else if (subtype == WIFI_ACTION) {
				RTW_INFO(FUNC_ADPT_FMT" braodcast action(%u)\n"
					, FUNC_ADPT_ARG(padapter), category);
			}
			#endif

			/*HW encrypt need to record encrypt type*/
			pattrib->encrypt = padapter->securitypriv.dot11wCipher;

			_rtw_memset(mme_cont, 0, _MME_IE_LENGTH_ - 2);
			mme_clen = padapter->securitypriv.dot11wCipher == _BIP_CMAC_128_ ? 16 : 24;

			MGMT_body = pframe + sizeof(struct rtw_ieee80211_hdr_3addr);
			pframe += pattrib->pktlen;

			/* octent 0 and 1 is key index ,BIP keyid is 4 or 5, LSB only need octent 0 */
			mme_cont[0] = padapter_link->securitypriv.dot11wBIPKeyid;
			/* increase PN and apply to packet */
			padapter_link->securitypriv.dot11wBIPtxpn.val++;
			RTW_PUT_LE64(&mme_cont[2], padapter_link->securitypriv.dot11wBIPtxpn.val);

			/* add MME IE with MIC all zero, MME string doesn't include element id and length */
			pframe = rtw_set_ie(pframe, _MME_IE_, mme_clen, mme_cont, &(pattrib->pktlen));
			pattrib->last_txcmdsz = pattrib->pktlen;

			if (pattrib->encrypt &&
				(padapter->securitypriv.sw_encrypt == _TRUE || padapter->securitypriv.hw_decrypted == _FALSE)) {
				pattrib->bswenc = _TRUE;
			} else {
				/* currently HW only support _BIP_CMAC_128_ */
				if (pattrib->encrypt == _BIP_CMAC_128_)
					pattrib->bswenc = _FALSE;
				else
					pattrib->bswenc = _TRUE;
			}

			if (!pattrib->bswenc) {
				pattrib->key_idx = padapter_link->securitypriv.dot11wBIPKeyid;
				/*Don't need to append MIC part of MME*/
				pattrib->pktlen -= (mme_clen == 16 ? 8 : 16);
				pattrib->last_txcmdsz = pattrib->pktlen;
				goto xmitframe_coalesce_success;
			}

			/* total frame length - header length */
			frame_body_len = pattrib->pktlen - sizeof(struct rtw_ieee80211_hdr_3addr);

			/* conscruct AAD, copy frame control field */
			_rtw_memcpy(BIP_AAD, &pwlanhdr->frame_ctl, 2);
			ClearRetry(BIP_AAD);
			ClearPwrMgt(BIP_AAD);
			ClearMData(BIP_AAD);
			/* conscruct AAD, copy address 1 to address 3 */
			_rtw_memcpy(BIP_AAD + 2, pwlanhdr->addr1, 18);
			/* copy management fram body */
			_rtw_memcpy(BIP_AAD + BIP_AAD_SIZE, MGMT_body, frame_body_len);

			#if DBG_MGMT_XMIT_BIP_DUMP
			/* dump total packet include MME with zero MIC */
			{
				int i;
				printk("Total packet: ");
				for (i = 0; i < BIP_AAD_SIZE + frame_body_len; i++)
					printk(" %02x ", BIP_AAD[i]);
				printk("\n");
			}
			#endif

			/* calculate mic */
			if (rtw_calculate_bip_mic(padapter->securitypriv.dot11wCipher,
				(u8 *)pwlanhdr, pattrib->pktlen,
				padapter_link->securitypriv.dot11wBIPKey[padapter_link->securitypriv.dot11wBIPKeyid].skey,
				BIP_AAD, (BIP_AAD_SIZE + frame_body_len), mic) == _FAIL)
				goto xmitframe_coalesce_fail;

			#if DBG_MGMT_XMIT_BIP_DUMP
			/* dump calculated mic result */
			{
				int i;
				printk("Calculated mic result: ");
				for (i = 0; i < 16; i++)
					printk(" %02x ", mic[i]);
				printk("\n");
			}
			#endif

			/* copy right BIP mic value, total is 128bits, we use the 0~63 bits */
			if (padapter->securitypriv.dot11wCipher == _BIP_CMAC_128_)
				_rtw_memcpy(pframe - 8, mic, 8);
			else
				_rtw_memcpy(pframe - 16, mic, 16);

			#if DBG_MGMT_XMIT_BIP_DUMP
			/*dump all packet after mic ok */
			{
				int pp;
				printk("pattrib->pktlen = %d\n", pattrib->pktlen);
				for (pp = 0; pp < pattrib->pktlen; pp++)
					printk(" %02x ", mem_start[pp]);
				printk("\n");
			}
			#endif

			#endif /* CONFIG_IEEE80211W */

			goto xmitframe_coalesce_success;
		}
	} else {
		/* unicast robust mgmt frame */
		#if DBG_MGMT_XMIT_COALESEC_DUMP
		if (subtype == WIFI_DEAUTH) {
			RTW_INFO(FUNC_ADPT_FMT" unicast deauth to "MAC_FMT"\n"
				, FUNC_ADPT_ARG(padapter), MAC_ARG(pattrib->ra));
		} else if (subtype == WIFI_DISASSOC) {
			RTW_INFO(FUNC_ADPT_FMT" unicast disassoc to "MAC_FMT"\n"
				, FUNC_ADPT_ARG(padapter), MAC_ARG(pattrib->ra));
		} else if (subtype == WIFI_ACTION) {
			RTW_INFO(FUNC_ADPT_FMT" unicast action(%u) to "MAC_FMT"\n"
				, FUNC_ADPT_ARG(padapter), category, MAC_ARG(pattrib->ra));
		}
		#endif

		pn = &psta->dot11txpn;
		cipher = psta->dot118021XPrivacy;
		kid = 0;

		_rtw_memcpy(pattrib->dot118021x_UncstKey.skey
			, psta->dot118021x_UncstKey.skey
			, (cipher & _SEC_TYPE_256_) ? 32 : 16);

		/* To use wrong key */
		if (pattrib->key_type == IEEE80211W_WRONG_KEY) {
			RTW_INFO("use wrong key\n");
			pattrib->dot118021x_UncstKey.skey[0] = 0xff;
		}
	}

	#if DBG_MGMT_XMIT_ENC_DUMP
	/* before encrypt dump the management packet content */
	{
		int i;
		printk("Management pkt: ");
		for (i = 0; i < pattrib->pktlen; i++)
			printk(" %02x ", pframe[i]);
		printk("=======\n");
	}
	#endif

	/* bakeup original management packet */
	_rtw_memcpy(tmp_buf, pframe, pattrib->pktlen);
	/* move to data portion */
	pframe += pattrib->hdrlen;

	if (pattrib->key_type != IEEE80211W_NO_KEY) {
		pattrib->encrypt = cipher;
		pattrib->bswenc = _TRUE;
	}

	/*
	* 802.11w encrypted management packet must be:
	* _AES_, _CCMP_256_, _GCMP_, _GCMP_256_
	*/
	switch (pattrib->encrypt) {
	case _AES_:
		pattrib->iv_len = 8;
		pattrib->icv_len = 8;
		AES_IV(pattrib->iv, (*pn), kid);
		hw_security_zero_hdrlen = _TRUE;
		break;
	case _CCMP_256_:
		pattrib->iv_len = 8;
		pattrib->icv_len = 16;
		AES_IV(pattrib->iv, (*pn), kid);
		hw_security_zero_hdrlen = _TRUE;
		break;
	case _GCMP_:
	case _GCMP_256_:
		pattrib->iv_len = 8;
		pattrib->icv_len = 16;
		GCMP_IV(pattrib->iv, (*pn), kid);
		hw_security_zero_hdrlen = _TRUE;
		break;
	default:
		goto xmitframe_coalesce_fail;
	}

	if (pattrib->encrypt &&
	(padapter->securitypriv.sw_encrypt == _TRUE || psta->hw_decrypted == _FALSE)) {
		pattrib->bswenc = _TRUE;
	} else {
		/* only right key can use HW encrypt */
		if (pattrib->key_type == IEEE80211W_RIGHT_KEY)
			pattrib->bswenc = _FALSE;
		else
			pattrib->bswenc = _TRUE;
	}

	/* at the moment the security CAM may be cleaned already --> use SW encryption */
	if (subtype == WIFI_DEAUTH || subtype == WIFI_DISASSOC)
		pattrib->bswenc = _TRUE;

	if ((pattrib->bswenc == _FALSE) &&
	    (hw_security_zero_hdrlen = _TRUE) &&
	    (padapter->dvobj->phl_com->dev_cap.sec_cap.hw_form_hdr)) {
		pattrib->iv_len = 0;
	}

	/* insert iv header into management frame */
	_rtw_memcpy(pframe, pattrib->iv, pattrib->iv_len);
	pframe += pattrib->iv_len;
	/* copy mgmt data portion after CCMP header */
	_rtw_memcpy(pframe, tmp_buf + pattrib->hdrlen, pattrib->pktlen - pattrib->hdrlen);
	/* move pframe to end of mgmt pkt */
	pframe += pattrib->pktlen - pattrib->hdrlen;
	/* add 8 bytes CCMP IV header to length */
	pattrib->pktlen += pattrib->iv_len;

	#if DBG_MGMT_XMIT_ENC_DUMP
	/* dump management packet include AES IV header */
	{
		int i;
		printk("Management pkt + IV: ");
		/* for(i=0; i<pattrib->pktlen; i++) */

		printk("@@@@@@@@@@@@@\n");
	}
	#endif

	if (!pattrib->bswenc) {
		pattrib->key_idx = kid;
		pattrib->last_txcmdsz = pattrib->pktlen;
		SetPrivacy(mem_start);
		goto xmitframe_coalesce_success;
	}

	if ((pattrib->icv_len > 0) && (pattrib->bswenc)) {
		_rtw_memcpy(pframe, pattrib->icv, pattrib->icv_len);
		pframe += pattrib->icv_len;
	}
	/* add 8 bytes MIC */
	pattrib->pktlen += pattrib->icv_len;
	/* set final tx command size */
	pattrib->last_txcmdsz = pattrib->pktlen;

	/* set protected bit must be beofre SW encrypt */
	SetPrivacy(mem_start);

	#if DBG_MGMT_XMIT_ENC_DUMP
	/* dump management packet include AES header */
	{
		int i;
		printk("prepare to enc Management pkt + IV: ");
		for (i = 0; i < pattrib->pktlen; i++)
			printk(" %02x ", mem_start[i]);
		printk("@@@@@@@@@@@@@\n");
	}
	#endif

	/* software encrypt */
	/* move to core_wlan_sw_encrypt() because of new txreq architecture */

xmitframe_coalesce_success:
	_rtw_spinunlock_bh(&padapter->security_key_mutex);
	rtw_mfree(BIP_AAD, ori_len);
	return _SUCCESS;

xmitframe_coalesce_fail:
	_rtw_spinunlock_bh(&padapter->security_key_mutex);
	rtw_mfree(BIP_AAD, ori_len);

	return _FAIL;
}
#endif /* defined(CONFIG_IEEE80211W) || defined(CONFIG_RTW_MESH) */

/* Logical Link Control(LLC) SubNetwork Attachment Point(SNAP) header
 * IEEE LLC/SNAP header contains 8 octets
 * First 3 octets comprise the LLC portion
 * SNAP portion, 5 octets, is divided into two fields:
 *	Organizationally Unique Identifier(OUI), 3 octets,
 *	type, defined by that organization, 2 octets.
 */
s32 rtw_put_snap(u8 *data, u16 h_proto)
{
	struct ieee80211_snap_hdr *snap;
	u8 *oui;


	snap = (struct ieee80211_snap_hdr *)data;
	snap->dsap = 0xaa;
	snap->ssap = 0xaa;
	snap->ctrl = 0x03;

	if (h_proto == 0x8137 || h_proto == 0x80f3)
		oui = P802_1H_OUI;
	else
		oui = RFC1042_OUI;

	snap->oui[0] = oui[0];
	snap->oui[1] = oui[1];
	snap->oui[2] = oui[2];

	*(u16 *)(data + SNAP_SIZE) = htons(h_proto);


	return SNAP_SIZE + sizeof(u16);
}

void rtw_update_protection(_adapter *padapter, u8 *ie, uint ie_len)
{

	uint	protection;
	u8	*perp;
	sint	 erp_len;
	struct	xmit_priv *pxmitpriv = &padapter->xmitpriv;
	struct	registry_priv *pregistrypriv = &padapter->registrypriv;


	switch (pxmitpriv->vcs_setting) {
	case DISABLE_VCS:
		pxmitpriv->vcs = NONE_VCS;
		break;

	case ENABLE_VCS:
		break;

	case AUTO_VCS:
	default:
		perp = rtw_get_ie(ie, _ERPINFO_IE_, &erp_len, ie_len);
		if (perp == NULL)
			pxmitpriv->vcs = NONE_VCS;
		else {
			protection = (*(perp + 2)) & BIT(1);
			if (protection) {
				if (pregistrypriv->vcs_type == RTS_CTS)
					pxmitpriv->vcs = RTS_CTS;
				else
					pxmitpriv->vcs = CTS_TO_SELF;
			} else
				pxmitpriv->vcs = NONE_VCS;
		}

		break;

	}


}

#ifdef CONFIG_CORE_TXSC
void rtw_count_tx_stats_tx_req(_adapter *padapter, struct rtw_xmit_req *txreq, struct sta_info *psta)
{
	struct stainfo_stats *pstats = NULL;
	struct xmit_priv	*pxmitpriv = &padapter->xmitpriv;
	struct mlme_priv	*pmlmepriv = &padapter->mlmepriv;
	u32 sz = 0;

	if (txreq->mdata.type == RTW_PHL_PKT_TYPE_DATA) {
		pmlmepriv->LinkDetectInfo.NumTxOkInPeriod++;
		pxmitpriv->tx_pkts++;
		sz = txreq->mdata.pktlen - RTW_SZ_LLC - txreq->mdata.hdr_len;
		switch (txreq->mdata.sec_type) {
		case RTW_ENC_WEP104:
		case RTW_ENC_WEP40:
			sz -= 4;
			break;
		case RTW_ENC_TKIP:
			sz -= 8;
			break;
		case RTW_ENC_CCMP:
			sz -= 8;
			break;
		case RTW_ENC_WAPI:
			sz -= 18;
			break;
		case RTW_ENC_GCMP256:
		case RTW_ENC_GCMP:
		case RTW_ENC_CCMP256:
			sz -= 8;
			break;
		default:
			break;
		}
		pxmitpriv->tx_bytes += sz;
		if (psta) {
			pstats = &psta->sta_stats;
			pstats->tx_pkts++;
			pstats->tx_bytes += sz;
			#if 0
			if (is_multicast_mac_addr(psta->phl_sta->mac_addr))
				pxmitpriv->tx_mc_pkts++;
			else if (is_broadcast_mac_addr(psta->phl_sta->mac_addr))
				pxmitpriv->tx_bc_pkts++;
			else
				pxmitpriv->tx_uc_pkts++;
			#endif
		}
	}
}
#endif

void rtw_count_tx_stats(_adapter *padapter, struct xmit_frame *pxmitframe, int sz)
{
	struct sta_info *psta = NULL;
	struct stainfo_stats *pstats = NULL;
	struct xmit_priv	*pxmitpriv = &padapter->xmitpriv;
	struct mlme_priv	*pmlmepriv = &padapter->mlmepriv;


	if (pxmitframe->xftype == RTW_TX_OS) {
		pmlmepriv->LinkDetectInfo.NumTxOkInPeriod++;
		pxmitpriv->tx_pkts++;
		pxmitpriv->tx_bytes += sz;

		psta = pxmitframe->attrib.psta;
		if (psta) {
			pstats = &psta->sta_stats;

			pstats->tx_pkts++;
			pstats->tx_bytes += sz;
		}
	}
}

#if 0 /*CONFIG_CORE_XMITBUF*/
static struct xmit_buf *__rtw_alloc_cmd_xmitbuf(struct xmit_priv *pxmitpriv,
		enum cmdbuf_type buf_type)
{
	struct xmit_buf *pxmitbuf =  NULL;


	pxmitbuf = &pxmitpriv->pcmd_xmitbuf[buf_type];
	if (pxmitbuf !=  NULL) {
		pxmitbuf->priv_data = NULL;

#if defined(CONFIG_SDIO_HCI) || defined(CONFIG_GSPI_HCI)
		pxmitbuf->len = 0;
		pxmitbuf->pdata = pxmitbuf->ptail = pxmitbuf->phead;
		pxmitbuf->agg_num = 0;
		pxmitbuf->pg_num = 0;
#endif
#ifdef CONFIG_PCI_HCI
		pxmitbuf->len = 0;
#ifdef CONFIG_TRX_BD_ARCH
		/*pxmitbuf->buf_desc = NULL;*/
#else
		pxmitbuf->desc = NULL;
#endif
#endif

		if (pxmitbuf->sctx) {
			RTW_INFO("%s pxmitbuf->sctx is not NULL\n", __func__);
			rtw_sctx_done_err(&pxmitbuf->sctx, RTW_SCTX_DONE_BUF_ALLOC);
		}
	} else
		RTW_INFO("%s fail, no xmitbuf available !!!\n", __func__);

	return pxmitbuf;
}

struct xmit_frame *__rtw_alloc_cmdxmitframe(struct xmit_priv *pxmitpriv,
		enum cmdbuf_type buf_type)
{
	struct xmit_frame		*pcmdframe;
	struct xmit_buf		*pxmitbuf;

	pcmdframe = rtw_alloc_xmitframe(pxmitpriv);
	if (pcmdframe == NULL) {
		RTW_INFO("%s, alloc xmitframe fail\n", __FUNCTION__);
		return NULL;
	}

	pxmitbuf = __rtw_alloc_cmd_xmitbuf(pxmitpriv, buf_type);
	if (pxmitbuf == NULL) {
		RTW_INFO("%s, alloc xmitbuf fail\n", __FUNCTION__);
		rtw_free_xmitframe(pxmitpriv, pcmdframe);
		return NULL;
	}

	pcmdframe->frame_tag = MGNT_FRAMETAG;

	pcmdframe->pxmitbuf = pxmitbuf;

	pcmdframe->buf_addr = pxmitbuf->pbuf;

	/* initial memory to zero */
	_rtw_memset(pcmdframe->buf_addr, 0, MAX_CMDBUF_SZ);

	pxmitbuf->priv_data = pcmdframe;

	return pcmdframe;

}

struct xmit_buf *rtw_alloc_xmitbuf_ext(struct xmit_priv *pxmitpriv)
{
	struct xmit_buf *pxmitbuf =  NULL;
	_list *plist, *phead;
	_queue *pfree_queue = &pxmitpriv->free_xmit_extbuf_queue;
	unsigned long sp_flags;

	_rtw_spinlock_irq(&pfree_queue->lock, &sp_flags);

	if (_rtw_queue_empty(pfree_queue) == _TRUE)
		pxmitbuf = NULL;
	else {

		phead = get_list_head(pfree_queue);

		plist = get_next(phead);

		pxmitbuf = LIST_CONTAINOR(plist, struct xmit_buf, list);

		rtw_list_delete(&(pxmitbuf->list));
	}

	if (pxmitbuf !=  NULL) {
		pxmitpriv->free_xmit_extbuf_cnt--;
#ifdef DBG_XMIT_BUF_EXT
		RTW_INFO("DBG_XMIT_BUF_EXT ALLOC no=%d,  free_xmit_extbuf_cnt=%d\n", pxmitbuf->no, pxmitpriv->free_xmit_extbuf_cnt);
#endif


		pxmitbuf->priv_data = NULL;

#if defined(CONFIG_SDIO_HCI) || defined(CONFIG_GSPI_HCI)
		pxmitbuf->len = 0;
		pxmitbuf->pdata = pxmitbuf->ptail = pxmitbuf->phead;
		pxmitbuf->agg_num = 1;
#endif
#ifdef CONFIG_PCI_HCI
		pxmitbuf->len = 0;
#ifdef CONFIG_TRX_BD_ARCH
		/*pxmitbuf->buf_desc = NULL;*/
#else
		pxmitbuf->desc = NULL;
#endif
#endif

		if (pxmitbuf->sctx) {
			RTW_INFO("%s pxmitbuf->sctx is not NULL\n", __func__);
			rtw_sctx_done_err(&pxmitbuf->sctx, RTW_SCTX_DONE_BUF_ALLOC);
		}

	}

	_rtw_spinunlock_irq(&pfree_queue->lock, &sp_flags);


	return pxmitbuf;
}

s32 rtw_free_xmitbuf_ext(struct xmit_priv *pxmitpriv, struct xmit_buf *pxmitbuf)
{
	_queue *pfree_queue = &pxmitpriv->free_xmit_extbuf_queue;
	unsigned long sp_flags;

	if (pxmitbuf == NULL)
		return _FAIL;

	_rtw_spinlock_irq(&pfree_queue->lock, &sp_flags);

	rtw_list_delete(&pxmitbuf->list);

	rtw_list_insert_tail(&(pxmitbuf->list), get_list_head(pfree_queue));
	pxmitpriv->free_xmit_extbuf_cnt++;
#ifdef DBG_XMIT_BUF_EXT
	RTW_INFO("DBG_XMIT_BUF_EXT FREE no=%d, free_xmit_extbuf_cnt=%d\n", pxmitbuf->no, pxmitpriv->free_xmit_extbuf_cnt);
#endif

	_rtw_spinunlock_irq(&pfree_queue->lock, &sp_flags);


	return _SUCCESS;
}

struct xmit_buf *rtw_alloc_xmitbuf(struct xmit_priv *pxmitpriv)
{
	struct xmit_buf *pxmitbuf =  NULL;
	_list *plist, *phead;
	_queue *pfree_xmitbuf_queue = &pxmitpriv->free_xmitbuf_queue;
	unsigned long sp_flags;

	/* RTW_INFO("+rtw_alloc_xmitbuf\n"); */

	_rtw_spinlock_irq(&pfree_xmitbuf_queue->lock, &sp_flags);

	if (_rtw_queue_empty(pfree_xmitbuf_queue) == _TRUE)
		pxmitbuf = NULL;
	else {

		phead = get_list_head(pfree_xmitbuf_queue);

		plist = get_next(phead);

		pxmitbuf = LIST_CONTAINOR(plist, struct xmit_buf, list);

		rtw_list_delete(&(pxmitbuf->list));
	}

	if (pxmitbuf !=  NULL) {
		pxmitpriv->free_xmitbuf_cnt--;
#ifdef DBG_XMIT_BUF
		RTW_INFO("DBG_XMIT_BUF ALLOC no=%d,  free_xmitbuf_cnt=%d\n", pxmitbuf->no, pxmitpriv->free_xmitbuf_cnt);
#endif
		/* RTW_INFO("alloc, free_xmitbuf_cnt=%d\n", pxmitpriv->free_xmitbuf_cnt); */

		pxmitbuf->priv_data = NULL;

#if defined(CONFIG_SDIO_HCI) || defined(CONFIG_GSPI_HCI)
		pxmitbuf->len = 0;
		pxmitbuf->pdata = pxmitbuf->ptail = pxmitbuf->phead;
		pxmitbuf->agg_num = 0;
		pxmitbuf->pg_num = 0;
#endif
#ifdef CONFIG_PCI_HCI
		pxmitbuf->len = 0;
#ifdef CONFIG_TRX_BD_ARCH
		/*pxmitbuf->buf_desc = NULL;*/
#else
		pxmitbuf->desc = NULL;
#endif
#endif

		if (pxmitbuf->sctx) {
			RTW_INFO("%s pxmitbuf->sctx is not NULL\n", __func__);
			rtw_sctx_done_err(&pxmitbuf->sctx, RTW_SCTX_DONE_BUF_ALLOC);
		}
	}
#ifdef DBG_XMIT_BUF
	else
		RTW_INFO("DBG_XMIT_BUF rtw_alloc_xmitbuf return NULL\n");
#endif

	_rtw_spinunlock_irq(&pfree_xmitbuf_queue->lock, &sp_flags);


	return pxmitbuf;
}

s32 rtw_free_xmitbuf(struct xmit_priv *pxmitpriv, struct xmit_buf *pxmitbuf)
{
	_queue *pfree_xmitbuf_queue = &pxmitpriv->free_xmitbuf_queue;
	unsigned long sp_flags;

	/* RTW_INFO("+rtw_free_xmitbuf\n"); */

	if (pxmitbuf == NULL)
		return _FAIL;

	if (pxmitbuf->sctx) {
		RTW_INFO("%s pxmitbuf->sctx is not NULL\n", __func__);
		rtw_sctx_done_err(&pxmitbuf->sctx, RTW_SCTX_DONE_BUF_FREE);
	}

	if (pxmitbuf->buf_tag == XMITBUF_CMD) {
	} else if (pxmitbuf->buf_tag == XMITBUF_MGNT)
		rtw_free_xmitbuf_ext(pxmitpriv, pxmitbuf);
	else {
		_rtw_spinlock_irq(&pfree_xmitbuf_queue->lock, &sp_flags);

		rtw_list_delete(&pxmitbuf->list);

		rtw_list_insert_tail(&(pxmitbuf->list), get_list_head(pfree_xmitbuf_queue));

		pxmitpriv->free_xmitbuf_cnt++;
		/* RTW_INFO("FREE, free_xmitbuf_cnt=%d\n", pxmitpriv->free_xmitbuf_cnt); */
#ifdef DBG_XMIT_BUF
		RTW_INFO("DBG_XMIT_BUF FREE no=%d, free_xmitbuf_cnt=%d\n", pxmitbuf->no, pxmitpriv->free_xmitbuf_cnt);
#endif
		_rtw_spinunlock_irq(&pfree_xmitbuf_queue->lock, &sp_flags);
	}


	return _SUCCESS;
}
#endif

void rtw_init_xmitframe(struct xmit_frame *pxframe)
{
	if (pxframe !=  NULL) { /* default value setting */
		#if 0 /*CONFIG_CORE_XMITBUF*/
		pxframe->buf_addr = NULL;
		pxframe->pxmitbuf = NULL;
		#endif

		_rtw_memset(&pxframe->attrib, 0, sizeof(struct pkt_attrib));
		/* pxframe->attrib.psta = NULL; */

		pxframe->frame_tag = DATA_FRAMETAG;

#ifdef CONFIG_USB_HCI
		pxframe->pkt = NULL;
#ifdef USB_PACKET_OFFSET_SZ
		pxframe->pkt_offset = (PACKET_OFFSET_SZ / 8);
#else
		pxframe->pkt_offset = 1;/* default use pkt_offset to fill tx desc */
#endif

#ifdef CONFIG_USB_TX_AGGREGATION
		pxframe->agg_num = 1;
#endif

#endif /* #ifdef CONFIG_USB_HCI */

#ifdef CONFIG_XMIT_ACK
		pxframe->ack_report = 0;
#endif
		pxframe->txfree_cnt = 0;
	}
}

/*
Calling context:
1. OS_TXENTRY
2. RXENTRY (rx_thread or RX_ISR/RX_CallBack)

If we turn on USE_RXTHREAD, then, no need for critical section.
Otherwise, we must use _enter/_exit critical to protect free_xmit_queue...

Must be very very cautious...

*/

#ifdef RTW_PHL_TX
void core_tx_init_xmitframe(struct xmit_frame *pxframe)
{
	if (!pxframe)
		return;
	#if 0 /*CONFIG_CORE_XMITBUF*/
	pxframe->pxmitbuf = NULL;
	#endif
	_rtw_memset(&pxframe->attrib, 0, sizeof(struct pkt_attrib));
	/* TXREQ_QMGT */
	pxframe->ptxreq_buf = NULL;
	pxframe->phl_txreq = NULL;

	pxframe->txreq_cnt = 0;
	pxframe->txfree_cnt = 0;
}

s32 core_tx_alloc_xmitframe(_adapter *padapter, struct xmit_frame **pxmitframe, u16 os_qid)
{
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;
	struct xmit_frame *pxframe = NULL;
	_queue *pfree_xmit_queue = &pxmitpriv->free_xmit_queue;
	_list *plist, *phead;

	PHLTX_LOG;

	_rtw_spinlock_bh(&pfree_xmit_queue->lock);

	if (_rtw_queue_empty(pfree_xmit_queue) == _TRUE) {
		_rtw_spinunlock_bh(&pfree_xmit_queue->lock);
		return FAIL;
	} else {
		phead = get_list_head(pfree_xmit_queue);

		plist = get_next(phead);

		pxframe = LIST_CONTAINOR(plist, struct xmit_frame, list);

		rtw_list_delete(&pxframe->list);
		pxmitpriv->free_xmitframe_cnt--;
		pxframe->os_qid = os_qid;
	}

	_rtw_spinunlock_bh(&pfree_xmit_queue->lock);
	rtw_os_check_stop_queue(pxmitpriv->adapter, os_qid);
	core_tx_init_xmitframe(pxframe);

	*pxmitframe = pxframe;
	return SUCCESS;
}

s32 core_tx_free_xmitframe(_adapter *padapter, struct xmit_frame *pxframe)
{
	struct xmit_priv 	*pxmitpriv = &padapter->xmitpriv;
	_queue *queue = NULL;
	/* TXREQ_QMGT */
	struct xmit_txreq_buf *ptxreq_buf = NULL;
	int i;
	struct rtw_xmit_req *txreq = NULL;
	struct rtw_pkt_buf_list *pkt_list = NULL;

	PHLTX_LOG;

	if (pxframe == NULL)
		goto exit;

	/* TXREQ_QMGT */
	ptxreq_buf = pxframe->ptxreq_buf;

	pxframe->txfree_cnt++;

	/* ?? shall detail check, like free 1 2 3, not free 2 2 3 */
	/* ?? rtw_alloc_xmitframe_once case, seems no one use*/

	if (pxframe->txfree_cnt < pxframe->txreq_cnt)
		goto exit;

	#if 0 /*CONFIG_CORE_XMITBUF*/
	if (pxframe->pxmitbuf)
		rtw_free_xmitbuf(pxmitpriv, pxframe->pxmitbuf);
	#endif

	for (i = 0; i < pxframe->txreq_cnt; i++) {
		if (!pxframe->buf_need_free)
			break;
		if (!(pxframe->buf_need_free & BIT(i)))
			continue;
		pxframe->buf_need_free &= ~BIT(i);

		txreq = &pxframe->phl_txreq[i];
		rtw_warn_on(txreq->pkt_cnt != 1);
		pkt_list = (struct rtw_pkt_buf_list *)txreq->pkt_list;
		if (pkt_list->vir_addr && pkt_list->length)
			rtw_mfree(pkt_list->vir_addr, pkt_list->length);
	}

	if (ptxreq_buf) {
		queue = &padapter->free_txreq_queue;
		_rtw_spinlock_bh(&queue->lock);

		rtw_list_delete(&ptxreq_buf->list);
		rtw_list_insert_tail(&ptxreq_buf->list, get_list_head(queue));

		padapter->free_txreq_cnt++;
		_rtw_spinunlock_bh(&queue->lock);
	} else {
		if (pxframe->ext_tag == 0)
			;//printk("%s:tx recyele: ptxreq_buf=NULL\n", __FUNCTION__);
	}

	rtw_os_xmit_complete(padapter, pxframe);

	if (pxframe->ext_tag == 0)
		queue = &pxmitpriv->free_xmit_queue;
	else if (pxframe->ext_tag == 1)
		queue = &pxmitpriv->free_xframe_ext_queue;
	else
		rtw_warn_on(1);

	_rtw_spinlock_bh(&queue->lock);

	rtw_list_delete(&pxframe->list);
	rtw_list_insert_tail(&pxframe->list, get_list_head(queue));

	if (pxframe->ext_tag == 0)
		pxmitpriv->free_xmitframe_cnt++;
	else if (pxframe->ext_tag == 1)
		pxmitpriv->free_xframe_ext_cnt++;

	_rtw_spinunlock_bh(&queue->lock);

	if (queue == &pxmitpriv->free_xmit_queue)
		rtw_os_check_wakup_queue(padapter, pxframe->os_qid);

exit:
	return _SUCCESS;
}

#endif

struct xmit_frame *rtw_alloc_xmitframe(struct xmit_priv *pxmitpriv, u16 os_qid)/* (_queue *pfree_xmit_queue) */
{
	/*
		Please remember to use all the osdep_service api,
		and lock/unlock or _enter/_exit critical to protect
		pfree_xmit_queue
	*/

	struct xmit_frame *pxframe = NULL;
	_list *plist, *phead;
	_queue *pfree_xmit_queue = &pxmitpriv->free_xmit_queue;


	_rtw_spinlock_bh(&pfree_xmit_queue->lock);

	if (_rtw_queue_empty(pfree_xmit_queue) == _TRUE) {
		pxframe =  NULL;
	} else {
		phead = get_list_head(pfree_xmit_queue);

		plist = get_next(phead);

		pxframe = LIST_CONTAINOR(plist, struct xmit_frame, list);

		rtw_list_delete(&(pxframe->list));
		pxmitpriv->free_xmitframe_cnt--;
		pxframe->os_qid = os_qid;
	}

	_rtw_spinunlock_bh(&pfree_xmit_queue->lock);

	if (pxframe)
		rtw_os_check_stop_queue(pxmitpriv->adapter, os_qid);

	rtw_init_xmitframe(pxframe);


	return pxframe;
}

struct xmit_frame *rtw_alloc_xmitframe_ext(struct xmit_priv *pxmitpriv)
{
	struct xmit_frame *pxframe = NULL;
	_list *plist, *phead;
	_queue *queue = &pxmitpriv->free_xframe_ext_queue;


	_rtw_spinlock_bh(&queue->lock);

	if (_rtw_queue_empty(queue) == _TRUE) {
		pxframe =  NULL;
	} else {
		phead = get_list_head(queue);
		plist = get_next(phead);
		pxframe = LIST_CONTAINOR(plist, struct xmit_frame, list);

		rtw_list_delete(&(pxframe->list));
		pxmitpriv->free_xframe_ext_cnt--;
	}

	_rtw_spinunlock_bh(&queue->lock);

	rtw_init_xmitframe(pxframe);


	return pxframe;
}

struct xmit_frame *rtw_alloc_xmitframe_once(struct xmit_priv *pxmitpriv)
{
	struct xmit_frame *pxframe = NULL;
	u8 *alloc_addr;

	alloc_addr = rtw_zmalloc(sizeof(struct xmit_frame) + 4);

	if (alloc_addr == NULL)
		goto exit;

	pxframe = (struct xmit_frame *)N_BYTE_ALIGMENT((SIZE_PTR)(alloc_addr), 4);
	pxframe->alloc_addr = alloc_addr;

	pxframe->padapter = pxmitpriv->adapter;
	pxframe->frame_tag = NULL_FRAMETAG;

	pxframe->pkt = NULL;
	#if 0 /*CONFIG_CORE_XMITBUF*/
	pxframe->buf_addr = NULL;
	pxframe->pxmitbuf = NULL;
	#endif

	rtw_init_xmitframe(pxframe);

	RTW_INFO("################## %s ##################\n", __func__);

exit:
	return pxframe;
}

s32 rtw_free_xmitframe(struct xmit_priv *pxmitpriv, struct xmit_frame *pxmitframe)
{
	_queue *queue = NULL;
	_adapter *padapter = pxmitpriv->adapter;
	struct sk_buff *pndis_pkt = NULL;


	if (pxmitframe == NULL) {
		goto exit;
	}

	if (pxmitframe->pkt) {
		pndis_pkt = pxmitframe->pkt;
		pxmitframe->pkt = NULL;
	}

	if (pxmitframe->alloc_addr) {
		RTW_INFO("################## %s with alloc_addr ##################\n", __func__);
		rtw_mfree(pxmitframe->alloc_addr, sizeof(struct xmit_frame) + 4);
		goto check_pkt_complete;
	}

	if (pxmitframe->ext_tag == 0)
		queue = &pxmitpriv->free_xmit_queue;
	else if (pxmitframe->ext_tag == 1)
		queue = &pxmitpriv->free_xframe_ext_queue;
	else
		rtw_warn_on(1);

	_rtw_spinlock_bh(&queue->lock);

	rtw_list_delete(&pxmitframe->list);
	rtw_list_insert_tail(&pxmitframe->list, get_list_head(queue));
	if (pxmitframe->ext_tag == 0) {
		pxmitpriv->free_xmitframe_cnt++;
	} else if (pxmitframe->ext_tag == 1) {
		pxmitpriv->free_xframe_ext_cnt++;
	} else {
	}

	_rtw_spinunlock_bh(&queue->lock);
	if (queue == &pxmitpriv->free_xmit_queue)
		rtw_os_check_wakup_queue(padapter, pxmitframe->os_qid);

check_pkt_complete:

	if (pndis_pkt)
		rtw_os_pkt_complete(padapter, pndis_pkt);

exit:


	return _SUCCESS;
}

void rtw_free_xmitframe_queue(struct xmit_priv *pxmitpriv, _queue *pframequeue)
{
	_list	*plist, *phead;
	struct	xmit_frame	*pxmitframe;


	_rtw_spinlock_bh(&(pframequeue->lock));

	phead = get_list_head(pframequeue);
	plist = get_next(phead);

	while (rtw_end_of_queue_search(phead, plist) == _FALSE) {

		pxmitframe = LIST_CONTAINOR(plist, struct xmit_frame, list);

		plist = get_next(plist);

		rtw_free_xmitframe(pxmitpriv, pxmitframe);

	}
	_rtw_spinunlock_bh(&(pframequeue->lock));

}

s32 rtw_xmitframe_enqueue(_adapter *padapter, struct xmit_frame *pxmitframe)
{
	DBG_COUNTER(padapter->tx_logs.core_tx_enqueue);
	if (rtw_xmit_classifier(padapter, pxmitframe) == _FAIL) {
		/*		pxmitframe->pkt = NULL; */
		return _FAIL;
	}

	return _SUCCESS;
}

static struct xmit_frame *dequeue_one_xmitframe(struct xmit_priv *pxmitpriv, struct hw_xmit *phwxmit, struct tx_servq *ptxservq, _queue *pframe_queue)
{
	_list	*xmitframe_plist, *xmitframe_phead;
	struct	xmit_frame	*pxmitframe = NULL;

	xmitframe_phead = get_list_head(pframe_queue);
	xmitframe_plist = get_next(xmitframe_phead);

	while ((rtw_end_of_queue_search(xmitframe_phead, xmitframe_plist)) == _FALSE) {
		pxmitframe = LIST_CONTAINOR(xmitframe_plist, struct xmit_frame, list);

		/* xmitframe_plist = get_next(xmitframe_plist); */

		/*#ifdef RTK_DMP_PLATFORM
		#ifdef CONFIG_USB_TX_AGGREGATION
				if((ptxservq->qcnt>0) && (ptxservq->qcnt<=2))
				{
					pxmitframe = NULL;

					rtw_tasklet_schedule(&pxmitpriv->xmit_tasklet);

					break;
				}
		#endif
		#endif*/
		rtw_list_delete(&pxmitframe->list);

		ptxservq->qcnt--;

		/* rtw_list_insert_tail(&pxmitframe->list, &phwxmit->pending); */

		/* ptxservq->qcnt--; */

		break;

		/* pxmitframe = NULL; */

	}

	return pxmitframe;
}

static struct xmit_frame *get_one_xmitframe(struct xmit_priv *pxmitpriv, struct hw_xmit *phwxmit, struct tx_servq *ptxservq, _queue *pframe_queue)
{
	_list	*xmitframe_plist, *xmitframe_phead;
	struct	xmit_frame	*pxmitframe = NULL;

	xmitframe_phead = get_list_head(pframe_queue);
	xmitframe_plist = get_next(xmitframe_phead);

	while ((rtw_end_of_queue_search(xmitframe_phead, xmitframe_plist)) == _FALSE) {
		pxmitframe = LIST_CONTAINOR(xmitframe_plist, struct xmit_frame, list);
		break;
	}

	return pxmitframe;
}

struct xmit_frame *rtw_get_xframe(struct xmit_priv *pxmitpriv, int *num_frame)
{
	_list *sta_plist, *sta_phead;
	struct hw_xmit *phwxmit_i = pxmitpriv->hwxmits;
	sint entry =  pxmitpriv->hwxmit_entry;

	struct hw_xmit *phwxmit;
	struct tx_servq *ptxservq = NULL;
	_queue *pframe_queue = NULL;
	struct xmit_frame *pxmitframe = NULL;
	_adapter *padapter = pxmitpriv->adapter;
	int i, inx[4];

	inx[0] = 0;
	inx[1] = 1;
	inx[2] = 2;
	inx[3] = 3;

	*num_frame = 0;

	_rtw_spinlock_bh(&pxmitpriv->lock);

	for (i = 0; i < entry; i++) {
		phwxmit = phwxmit_i + inx[i];

		sta_phead = get_list_head(phwxmit->sta_queue);
		sta_plist = get_next(sta_phead);

		while ((rtw_end_of_queue_search(sta_phead, sta_plist)) == _FALSE) {

			ptxservq = LIST_CONTAINOR(sta_plist, struct tx_servq, tx_pending);
			pframe_queue = &ptxservq->sta_pending;

			if (ptxservq->qcnt) {
				*num_frame = ptxservq->qcnt;
				pxmitframe = get_one_xmitframe(pxmitpriv, phwxmit, ptxservq, pframe_queue);
				goto exit;
			}
			sta_plist = get_next(sta_plist);
		}
	}

exit:

	_rtw_spinunlock_bh(&pxmitpriv->lock);

	return pxmitframe;
}


struct xmit_frame *rtw_dequeue_xframe(struct xmit_priv *pxmitpriv, struct hw_xmit *phwxmit_i, sint entry)
{
	_list *sta_plist, *sta_phead;
	struct hw_xmit *phwxmit;
	struct tx_servq *ptxservq = NULL;
	_queue *pframe_queue = NULL;
	struct xmit_frame *pxmitframe = NULL;
	_adapter *padapter = pxmitpriv->adapter;
	struct registry_priv	*pregpriv = &padapter->registrypriv;
	int i, inx[4];

	inx[0] = 0;
	inx[1] = 1;
	inx[2] = 2;
	inx[3] = 3;

	if (pregpriv->wifi_spec == 1) {
		int j;
#if 0
		if (flags < XMIT_QUEUE_ENTRY) {
			/* priority exchange according to the completed xmitbuf flags. */
			inx[flags] = 0;
			inx[0] = flags;
		}
#endif

#if defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI) || defined(CONFIG_PCI_HCI)
		for (j = 0; j < 4; j++)
			inx[j] = pxmitpriv->wmm_para_seq[j];
#endif
	}

	_rtw_spinlock_bh(&pxmitpriv->lock);

	for (i = 0; i < entry; i++) {
		phwxmit = phwxmit_i + inx[i];

		/* _rtw_spinlock_irq(&phwxmit->sta_queue->lock, &sp_flags); */

		sta_phead = get_list_head(phwxmit->sta_queue);
		sta_plist = get_next(sta_phead);

		while ((rtw_end_of_queue_search(sta_phead, sta_plist)) == _FALSE) {

			ptxservq = LIST_CONTAINOR(sta_plist, struct tx_servq, tx_pending);

			pframe_queue = &ptxservq->sta_pending;

			pxmitframe = dequeue_one_xmitframe(pxmitpriv, phwxmit, ptxservq, pframe_queue);

			if (pxmitframe) {
				phwxmit->accnt--;

				/* Remove sta node when there is no pending packets. */
				if (_rtw_queue_empty(pframe_queue)) /* must be done after get_next and before break */
					rtw_list_delete(&ptxservq->tx_pending);

				/* _rtw_spinunlock_irq(&phwxmit->sta_queue->lock, sp_flags); */

				goto exit;
			}

			sta_plist = get_next(sta_plist);

		}

		/* _rtw_spinunlock_irq(&phwxmit->sta_queue->lock, sp_flags); */

	}

exit:

	_rtw_spinunlock_bh(&pxmitpriv->lock);

	return pxmitframe;
}

#if 1
struct tx_servq *rtw_get_sta_pending(_adapter *padapter, struct sta_info *psta, sint up, u8 *ac)
{
	struct tx_servq *ptxservq = NULL;


	switch (up) {
	case 1:
	case 2:
		ptxservq = &(psta->sta_xmitpriv.bk_q);
		*(ac) = 3;
		break;

	case 4:
	case 5:
		ptxservq = &(psta->sta_xmitpriv.vi_q);
		*(ac) = 1;
		break;

	case 6:
	case 7:
		ptxservq = &(psta->sta_xmitpriv.vo_q);
		*(ac) = 0;
		break;

	case 0:
	case 3:
	default:
		ptxservq = &(psta->sta_xmitpriv.be_q);
		*(ac) = 2;
		break;

	}


	return ptxservq;
}
#else
__inline static struct tx_servq *rtw_get_sta_pending
(_adapter *padapter, _queue **ppstapending, struct sta_info *psta, sint up) {
	struct tx_servq *ptxservq;
	struct hw_xmit *phwxmits =  padapter->xmitpriv.hwxmits;


#ifdef CONFIG_RTL8711

	if (IS_MCAST(psta->phl_sta->mac_addr)) {
		ptxservq = &(psta->sta_xmitpriv.be_q); /* we will use be_q to queue bc/mc frames in BCMC_stainfo */
		*ppstapending = &padapter->xmitpriv.bm_pending;
	} else
#endif
	{
		switch (up) {
		case 1:
		case 2:
			ptxservq = &(psta->sta_xmitpriv.bk_q);
			*ppstapending = &padapter->xmitpriv.bk_pending;
			(phwxmits + 3)->accnt++;
			break;

		case 4:
		case 5:
			ptxservq = &(psta->sta_xmitpriv.vi_q);
			*ppstapending = &padapter->xmitpriv.vi_pending;
			(phwxmits + 1)->accnt++;
			break;

		case 6:
		case 7:
			ptxservq = &(psta->sta_xmitpriv.vo_q);
			*ppstapending = &padapter->xmitpriv.vo_pending;
			(phwxmits + 0)->accnt++;
			break;

		case 0:
		case 3:
		default:
			ptxservq = &(psta->sta_xmitpriv.be_q);
			*ppstapending = &padapter->xmitpriv.be_pending;
			(phwxmits + 2)->accnt++;
			break;

		}

	}


	return ptxservq;
}
#endif

/*
 * Will enqueue pxmitframe to the proper queue,
 * and indicate it to xx_pending list.....
 */
s32 rtw_xmit_classifier(_adapter *padapter, struct xmit_frame *pxmitframe)
{
	u8	ac_index;
	struct sta_info	*psta;
	struct tx_servq	*ptxservq;
	struct pkt_attrib	*pattrib = &pxmitframe->attrib;
	struct hw_xmit	*phwxmits =  padapter->xmitpriv.hwxmits;
	sint res = _SUCCESS;


	DBG_COUNTER(padapter->tx_logs.core_tx_enqueue_class);

	/*
		if (pattrib->psta) {
			psta = pattrib->psta;
		} else {
			RTW_INFO("%s, call rtw_get_stainfo()\n", __func__);
			psta = rtw_get_stainfo(pstapriv, pattrib->ra);
		}
	*/

	psta = rtw_get_stainfo(&padapter->stapriv, pattrib->ra);
	if (pattrib->psta != psta) {
		DBG_COUNTER(padapter->tx_logs.core_tx_enqueue_class_err_sta);
		RTW_INFO("%s, pattrib->psta(%p) != psta(%p)\n", __func__, pattrib->psta, psta);
		return _FAIL;
	}

	if (psta == NULL) {
		DBG_COUNTER(padapter->tx_logs.core_tx_enqueue_class_err_nosta);
		res = _FAIL;
		RTW_INFO("rtw_xmit_classifier: psta == NULL\n");
		goto exit;
	}

	if (!(psta->state & WIFI_ASOC_STATE)) {
		DBG_COUNTER(padapter->tx_logs.core_tx_enqueue_class_err_fwlink);
		RTW_INFO("%s, psta->state(0x%x) != WIFI_ASOC_STATE\n", __func__, psta->state);
		return _FAIL;
	}

	ptxservq = rtw_get_sta_pending(padapter, psta, pattrib->priority, (u8 *)(&ac_index));

	/* _rtw_spinlock_irq(&pstapending->lock, &flags); */

	if (rtw_is_list_empty(&ptxservq->tx_pending))
		rtw_list_insert_tail(&ptxservq->tx_pending, get_list_head(phwxmits[ac_index].sta_queue));

	/* _rtw_spinlock_irq(&ptxservq->sta_pending.lock, &sp_flags); */

	rtw_list_insert_tail(&pxmitframe->list, get_list_head(&ptxservq->sta_pending));
	ptxservq->qcnt++;
	phwxmits[ac_index].accnt++;

	/* _rtw_spinunlock_irq(&ptxservq->sta_pending.lock, &sp_flags); */

	/* _rtw_spinunlock_irq(&pstapending->lock, &flags); */

exit:


	return res;
}

void rtw_alloc_hwxmits(_adapter *padapter)
{
	struct hw_xmit *hwxmits;
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;

	pxmitpriv->hwxmit_entry = HWXMIT_ENTRY;

	pxmitpriv->hwxmits = NULL;

	pxmitpriv->hwxmits = (struct hw_xmit *)rtw_zmalloc(sizeof(struct hw_xmit) * pxmitpriv->hwxmit_entry);

	if (pxmitpriv->hwxmits == NULL) {
		RTW_INFO("alloc hwxmits fail!...\n");
		return;
	}

	hwxmits = pxmitpriv->hwxmits;

	if (pxmitpriv->hwxmit_entry == 5) {
		/* pxmitpriv->bmc_txqueue.head = 0; */
		/* hwxmits[0] .phwtxqueue = &pxmitpriv->bmc_txqueue; */
		hwxmits[0] .sta_queue = &pxmitpriv->bm_pending;

		/* pxmitpriv->vo_txqueue.head = 0; */
		/* hwxmits[1] .phwtxqueue = &pxmitpriv->vo_txqueue; */
		hwxmits[1] .sta_queue = &pxmitpriv->vo_pending;

		/* pxmitpriv->vi_txqueue.head = 0; */
		/* hwxmits[2] .phwtxqueue = &pxmitpriv->vi_txqueue; */
		hwxmits[2] .sta_queue = &pxmitpriv->vi_pending;

		/* pxmitpriv->bk_txqueue.head = 0; */
		/* hwxmits[3] .phwtxqueue = &pxmitpriv->bk_txqueue; */
		hwxmits[3] .sta_queue = &pxmitpriv->bk_pending;

		/* pxmitpriv->be_txqueue.head = 0; */
		/* hwxmits[4] .phwtxqueue = &pxmitpriv->be_txqueue; */
		hwxmits[4] .sta_queue = &pxmitpriv->be_pending;

	} else if (pxmitpriv->hwxmit_entry == 4) {

		/* pxmitpriv->vo_txqueue.head = 0; */
		/* hwxmits[0] .phwtxqueue = &pxmitpriv->vo_txqueue; */
		hwxmits[0] .sta_queue = &pxmitpriv->vo_pending;

		/* pxmitpriv->vi_txqueue.head = 0; */
		/* hwxmits[1] .phwtxqueue = &pxmitpriv->vi_txqueue; */
		hwxmits[1] .sta_queue = &pxmitpriv->vi_pending;

		/* pxmitpriv->be_txqueue.head = 0; */
		/* hwxmits[2] .phwtxqueue = &pxmitpriv->be_txqueue; */
		hwxmits[2] .sta_queue = &pxmitpriv->be_pending;

		/* pxmitpriv->bk_txqueue.head = 0; */
		/* hwxmits[3] .phwtxqueue = &pxmitpriv->bk_txqueue; */
		hwxmits[3] .sta_queue = &pxmitpriv->bk_pending;
	} else {


	}


}

void rtw_free_hwxmits(_adapter *padapter)
{
	struct hw_xmit *hwxmits;
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;

	hwxmits = pxmitpriv->hwxmits;
	if (hwxmits)
		rtw_mfree((u8 *)hwxmits, (sizeof(struct hw_xmit) * pxmitpriv->hwxmit_entry));
}

void rtw_init_hwxmits(struct hw_xmit *phwxmit, sint entry)
{
	sint i;
	for (i = 0; i < entry; i++, phwxmit++) {
		/* _rtw_spinlock_init(&phwxmit->xmit_lock); */
		/* _rtw_init_listhead(&phwxmit->pending);		 */
		/* phwxmit->txcmdcnt = 0; */
		phwxmit->accnt = 0;
	}
}

#ifdef CONFIG_BR_EXT
int rtw_br_client_tx(_adapter *padapter, struct sk_buff **pskb)
{
	struct sk_buff *skb = *pskb;
	/* if(MLME_IS_STA(adapter) */
	{
		void dhcp_flag_bcast(_adapter *priv, struct sk_buff *skb);
		int res, is_vlan_tag = 0, i, do_nat25 = 1;
		unsigned short vlan_hdr = 0;
		void *br_port = NULL;

		/* mac_clone_handle_frame(priv, skb); */

#if (LINUX_VERSION_CODE <= KERNEL_VERSION(2, 6, 35))
		br_port = padapter->pnetdev->br_port;
#else   /* (LINUX_VERSION_CODE <= KERNEL_VERSION(2, 6, 35)) */
		rcu_read_lock();
		br_port = rcu_dereference(padapter->pnetdev->rx_handler_data);
		rcu_read_unlock();
#endif /* (LINUX_VERSION_CODE <= KERNEL_VERSION(2, 6, 35)) */
		_rtw_spinlock_bh(&padapter->br_ext_lock);
		if (!(skb->data[0] & 1) &&
		    br_port &&
		    _rtw_memcmp(skb->data + MACADDRLEN, padapter->br_mac, MACADDRLEN) &&
		    *((unsigned short *)(skb->data + MACADDRLEN * 2)) != __constant_htons(ETH_P_8021Q) &&
		    *((unsigned short *)(skb->data + MACADDRLEN * 2)) == __constant_htons(ETH_P_IP) &&
		    !_rtw_memcmp(padapter->scdb_mac, skb->data + MACADDRLEN, MACADDRLEN) && padapter->scdb_entry) {
			_rtw_memcpy(skb->data + MACADDRLEN, GET_MY_HWADDR(padapter), MACADDRLEN);
			padapter->scdb_entry->ageing_timer = jiffies;
			_rtw_spinunlock_bh(&padapter->br_ext_lock);
		} else
			/* if (!priv->pmib->ethBrExtInfo.nat25_disable)		 */
		{
			/*			if (priv->dev->br_port &&
			 *				 !_rtw_memcmp(skb->data+MACADDRLEN, priv->br_mac, MACADDRLEN)) { */
#if 1
			if (*((unsigned short *)(skb->data + MACADDRLEN * 2)) == __constant_htons(ETH_P_8021Q)) {
				is_vlan_tag = 1;
				vlan_hdr = *((unsigned short *)(skb->data + MACADDRLEN * 2 + 2));
				for (i = 0; i < 6; i++)
					*((unsigned short *)(skb->data + MACADDRLEN * 2 + 2 - i * 2)) = *((unsigned short *)(skb->data + MACADDRLEN * 2 - 2 - i * 2));
				skb_pull(skb, 4);
			}
			/* if SA == br_mac && skb== IP  => copy SIP to br_ip ?? why */
			if (!_rtw_memcmp(skb->data + MACADDRLEN, padapter->br_mac, MACADDRLEN) &&
			    (*((unsigned short *)(skb->data + MACADDRLEN * 2)) == __constant_htons(ETH_P_IP)))
				_rtw_memcpy(padapter->br_ip, skb->data + WLAN_ETHHDR_LEN + 12, 4);

			if (*((unsigned short *)(skb->data + MACADDRLEN * 2)) == __constant_htons(ETH_P_IP)) {
				if (_rtw_memcmp(padapter->scdb_mac, skb->data + MACADDRLEN, MACADDRLEN)) {
					void *scdb_findEntry(_adapter *priv, unsigned char *macAddr, unsigned char *ipAddr);

					padapter->scdb_entry = (struct nat25_network_db_entry *)scdb_findEntry(padapter,
						skb->data + MACADDRLEN, skb->data + WLAN_ETHHDR_LEN + 12);
					if (padapter->scdb_entry != NULL) {
						_rtw_memcpy(padapter->scdb_mac, skb->data + MACADDRLEN, MACADDRLEN);
						_rtw_memcpy(padapter->scdb_ip, skb->data + WLAN_ETHHDR_LEN + 12, 4);
						padapter->scdb_entry->ageing_timer = jiffies;
						do_nat25 = 0;
					}
				} else {
					if (padapter->scdb_entry) {
						padapter->scdb_entry->ageing_timer = jiffies;
						do_nat25 = 0;
					} else {
						_rtw_memset(padapter->scdb_mac, 0, MACADDRLEN);
						_rtw_memset(padapter->scdb_ip, 0, 4);
					}
				}
			}
			_rtw_spinunlock_bh(&padapter->br_ext_lock);
#endif /* 1 */
			if (do_nat25) {
				int nat25_db_handle(_adapter *priv, struct sk_buff *skb, int method);
				if (nat25_db_handle(padapter, skb, NAT25_CHECK) == 0) {
					struct sk_buff *newskb;

					if (is_vlan_tag) {
						skb_push(skb, 4);
						for (i = 0; i < 6; i++)
							*((unsigned short *)(skb->data + i * 2)) = *((unsigned short *)(skb->data + 4 + i * 2));
						*((unsigned short *)(skb->data + MACADDRLEN * 2)) = __constant_htons(ETH_P_8021Q);
						*((unsigned short *)(skb->data + MACADDRLEN * 2 + 2)) = vlan_hdr;
					}

					newskb = rtw_skb_copy(skb);
					if (newskb == NULL) {
						/* priv->ext_stats.tx_drops++; */
						DEBUG_ERR("TX DROP: rtw_skb_copy fail!\n");
						/* goto stop_proc; */
						return -1;
					}
					rtw_skb_free(skb);

					*pskb = skb = newskb;
					if (is_vlan_tag) {
						vlan_hdr = *((unsigned short *)(skb->data + MACADDRLEN * 2 + 2));
						for (i = 0; i < 6; i++)
							*((unsigned short *)(skb->data + MACADDRLEN * 2 + 2 - i * 2)) = *((unsigned short *)(skb->data + MACADDRLEN * 2 - 2 - i * 2));
						skb_pull(skb, 4);
					}
				}

				if (skb_is_nonlinear(skb))
					DEBUG_ERR("%s(): skb_is_nonlinear!!\n", __FUNCTION__);


#if (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 18))
				res = skb_linearize(skb, GFP_ATOMIC);
#else	/* (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 18)) */
				res = skb_linearize(skb);
#endif /* (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 18)) */
				if (res < 0) {
					DEBUG_ERR("TX DROP: skb_linearize fail!\n");
					/* goto free_and_stop; */
					return -1;
				}

				res = nat25_db_handle(padapter, skb, NAT25_INSERT);
				if (res < 0) {
					if (res == -2) {
						/* priv->ext_stats.tx_drops++; */
						DEBUG_ERR("TX DROP: nat25_db_handle fail!\n");
						/* goto free_and_stop; */
						return -1;

					}
					/* we just print warning message and let it go */
					/* DEBUG_WARN("%s()-%d: nat25_db_handle INSERT Warning!\n", __FUNCTION__, __LINE__); */
					/* return -1; */ /* return -1 will cause system crash on 2011/08/30! */
					return 0;
				}
			}

			_rtw_memcpy(skb->data + MACADDRLEN, GET_MY_HWADDR(padapter), MACADDRLEN);

			dhcp_flag_bcast(padapter, skb);

			if (is_vlan_tag) {
				skb_push(skb, 4);
				for (i = 0; i < 6; i++)
					*((unsigned short *)(skb->data + i * 2)) = *((unsigned short *)(skb->data + 4 + i * 2));
				*((unsigned short *)(skb->data + MACADDRLEN * 2)) = __constant_htons(ETH_P_8021Q);
				*((unsigned short *)(skb->data + MACADDRLEN * 2 + 2)) = vlan_hdr;
			}
		}
#if 0
		else {
			if (*((unsigned short *)(skb->data + MACADDRLEN * 2)) == __constant_htons(ETH_P_8021Q))
				is_vlan_tag = 1;

			if (is_vlan_tag) {
				if (ICMPV6_MCAST_MAC(skb->data) && ICMPV6_PROTO1A_VALN(skb->data))
					_rtw_memcpy(skb->data + MACADDRLEN, GET_MY_HWADDR(padapter), MACADDRLEN);
			} else {
				if (ICMPV6_MCAST_MAC(skb->data) && ICMPV6_PROTO1A(skb->data))
					_rtw_memcpy(skb->data + MACADDRLEN, GET_MY_HWADDR(padapter), MACADDRLEN);
			}
		}
#endif /* 0 */

		/* check if SA is equal to our MAC */
		if (_rtw_memcmp(skb->data + MACADDRLEN, GET_MY_HWADDR(padapter), MACADDRLEN)) {
			/* priv->ext_stats.tx_drops++; */
			DEBUG_ERR("TX DROP: untransformed frame SA:%02X%02X%02X%02X%02X%02X!\n",
				skb->data[6], skb->data[7], skb->data[8], skb->data[9], skb->data[10], skb->data[11]);
			/* goto free_and_stop; */
			return -1;
		}
	}
	return 0;
}
#endif /* CONFIG_BR_EXT */

#if 0
static void do_queue_select(_adapter	*padapter, struct pkt_attrib *pattrib)
{
	u8 qsel;

	qsel = pattrib->priority;

	/* high priority packet */
	if (pattrib->hipriority_pkt) {
		pattrib->qsel = rtw_hal_get_qsel(padapter, QSLT_VO_ID);
		pattrib->priority  = rtw_hal_get_qsel(padapter, QSLT_VO_ID);
	}
}
#endif
/*
 * The main transmit(tx) entry
 *
 * Return
 *	1	enqueue
 *	0	success, hardware will handle this xmit frame(packet)
 *	<0	fail
 */
 #if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 24))
s32 rtw_monitor_xmit_entry(struct sk_buff *skb, struct net_device *ndev)
{
	u16 frame_ctl;
	struct ieee80211_radiotap_header rtap_hdr;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(ndev);
	struct pkt_file pktfile;
	struct rtw_ieee80211_hdr *pwlanhdr;
	struct pkt_attrib	*pattrib;
	struct xmit_frame		*pmgntframe;
	struct mlme_ext_priv	*pmlmeext = &(padapter->mlmeextpriv);
	struct xmit_priv	*pxmitpriv = &(padapter->xmitpriv);
	unsigned char	*pframe;
	u8 dummybuf[32];
	int len = skb->len, rtap_len;
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);


	rtw_mstat_update(MSTAT_TYPE_SKB, MSTAT_ALLOC_SUCCESS, skb->truesize);

#ifndef CONFIG_CUSTOMER_ALIBABA_GENERAL
	if (unlikely(skb->len < sizeof(struct ieee80211_radiotap_header)))
		goto fail;

	_rtw_open_pktfile((struct sk_buff *)skb, &pktfile);
	_rtw_pktfile_read(&pktfile, (u8 *)(&rtap_hdr), sizeof(struct ieee80211_radiotap_header));
	rtap_len = ieee80211_get_radiotap_len((u8 *)(&rtap_hdr));
	if (unlikely(rtap_hdr.it_version))
		goto fail;

	if (unlikely(skb->len < rtap_len))
		goto fail;

	if (rtap_len != 12) {
		RTW_INFO("radiotap len (should be 14): %d\n", rtap_len);
		goto fail;
	}
	_rtw_pktfile_read(&pktfile, dummybuf, rtap_len-sizeof(struct ieee80211_radiotap_header));
	len = len - rtap_len;
#endif
	pmgntframe = alloc_mgtxmitframe(pxmitpriv);
	if (pmgntframe == NULL) {
		rtw_udelay_os(500);
		goto fail;
	}

	_rtw_memset(pmgntframe->buf_addr, 0, WLANHDR_OFFSET + TXDESC_OFFSET);
	pframe = (u8 *)(pmgntframe->buf_addr) + TXDESC_OFFSET;
//	_rtw_memcpy(pframe, (void *)checking, len);
	_rtw_pktfile_read(&pktfile, pframe, len);


	/* Check DATA/MGNT frames */
	pwlanhdr = (struct rtw_ieee80211_hdr *)pframe;
	frame_ctl = le16_to_cpu(pwlanhdr->frame_ctl);
	if ((frame_ctl & RTW_IEEE80211_FCTL_FTYPE) == RTW_IEEE80211_FTYPE_DATA) {

		pattrib = &pmgntframe->attrib;
		update_monitor_frame_attrib(padapter, padapter_link, pattrib);

		if (is_broadcast_mac_addr(pwlanhdr->addr3) || is_broadcast_mac_addr(pwlanhdr->addr1))
			pattrib->rate = MGN_24M;

	} else {

		pattrib = &pmgntframe->attrib;
		update_mgntframe_attrib(padapter, padapter_link, pattrib);

	}
	pattrib->retry_ctrl = _FALSE;
	pattrib->pktlen = len;
	pmlmeext->mgnt_seq = GetSequence(pwlanhdr);
	pattrib->seqnum = pmlmeext->mgnt_seq;
	pmlmeext->mgnt_seq++;
	pattrib->last_txcmdsz = pattrib->pktlen;

	dump_mgntframe(padapter, pmgntframe);

fail:
	rtw_skb_free(skb);
	return 0;
}
#endif

/*
 *
 * Return _TRUE when frame has been put to queue, otherwise return _FALSE.
 */
static u8 xmit_enqueue(_adapter *a, struct xmit_frame *frame)
{
	struct sta_info *sta = NULL;
	struct pkt_attrib *attrib = NULL;
	_list *head;
	u8 ret = _TRUE;


	attrib = &frame->attrib;
	sta = attrib->psta;
	if (!sta)
		return _FALSE;

	_rtw_spinlock_bh(&sta->tx_queue.lock);

	head = get_list_head(&sta->tx_queue);

	if ((rtw_is_list_empty(head) == _TRUE) && (!sta->tx_q_enable)) {
		ret = _FALSE;
		goto exit;
	}

	rtw_list_insert_tail(&frame->list, head);
	RTW_INFO(FUNC_ADPT_FMT ": en-queue tx pkt for macid=%d\n",
		 FUNC_ADPT_ARG(a), sta->phl_sta->macid);

exit:
	_rtw_spinunlock_bh(&sta->tx_queue.lock);

	return ret;
}

static void xmit_dequeue(struct sta_info *sta)
{
	_adapter *a;
	_list *head, *list;
	struct xmit_frame *frame;


	a = sta->padapter;

	_rtw_spinlock_bh(&sta->tx_queue.lock);

	head = get_list_head(&sta->tx_queue);

	do {
		if (rtw_is_list_empty(head) == _TRUE)
			break;

		list = get_next(head);
		rtw_list_delete(list);
		frame = LIST_CONTAINOR(list, struct xmit_frame, list);
		RTW_INFO(FUNC_ADPT_FMT ": de-queue tx frame of macid=%d\n",
			 FUNC_ADPT_ARG(a), sta->phl_sta->macid);

		/*rtw_hal_xmit(a, frame);*/
		rtw_intf_data_xmit(a, frame);
	} while (1);

	_rtw_spinunlock_bh(&sta->tx_queue.lock);
}

void rtw_xmit_dequeue_callback(_workitem *work)
{
	struct sta_info *sta;


	sta = container_of(work, struct sta_info, tx_q_work);
	xmit_dequeue(sta);
}

void rtw_xmit_queue_set(struct sta_info *sta)
{
	_rtw_spinlock_bh(&sta->tx_queue.lock);

	if (sta->tx_q_enable) {
		RTW_WARN(FUNC_ADPT_FMT ": duplicated set!\n",
			 FUNC_ADPT_ARG(sta->padapter));
		goto exit;
	}
	sta->tx_q_enable = 1;
	RTW_INFO(FUNC_ADPT_FMT ": enable queue TX for macid=%d\n",
		 FUNC_ADPT_ARG(sta->padapter), sta->phl_sta->macid);

exit:
	_rtw_spinunlock_bh(&sta->tx_queue.lock);
}

void rtw_xmit_queue_clear(struct sta_info *sta)
{
	_rtw_spinlock_bh(&sta->tx_queue.lock);

	if (!sta->tx_q_enable) {
		RTW_WARN(FUNC_ADPT_FMT ": tx queue for macid=%d "
			 "not be enabled!\n",
			 FUNC_ADPT_ARG(sta->padapter), sta->phl_sta->macid);
		goto exit;
	}

	sta->tx_q_enable = 0;
	RTW_INFO(FUNC_ADPT_FMT ": disable queue TX for macid=%d\n",
		 FUNC_ADPT_ARG(sta->padapter), sta->phl_sta->macid);

	_set_workitem(&sta->tx_q_work);

exit:
	_rtw_spinunlock_bh(&sta->tx_queue.lock);
}

/*
 * The main transmit(tx) entry post handle
 *
 * Return
 *	1	enqueue
 *	0	success, hardware will handle this xmit frame(packet)
 *	<0	fail
 */
s32 rtw_xmit_posthandle(_adapter *padapter, struct xmit_frame *pxmitframe,
							struct sk_buff *pkt)
{
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;
	s32 res;

	res = update_attrib(padapter, pkt, &pxmitframe->attrib);

#ifdef CONFIG_WAPI_SUPPORT
	if (pxmitframe->attrib.ether_type != 0x88B4) {
		if (rtw_wapi_drop_for_key_absent(padapter, pxmitframe->attrib.ra)) {
			WAPI_TRACE(WAPI_RX, "drop for key absend when tx\n");
			res = _FAIL;
		}
	}
#endif
	if (res == _FAIL) {
		/*RTW_INFO("%s-"ADPT_FMT" update attrib fail\n", __func__, ADPT_ARG(padapter));*/
#ifdef DBG_TX_DROP_FRAME
		RTW_INFO("DBG_TX_DROP_FRAME %s update attrib fail\n", __FUNCTION__);
#endif
		rtw_free_xmitframe(pxmitpriv, pxmitframe);
		return -1;
	}
	pxmitframe->pkt = pkt;

	rtw_led_tx_control(padapter, pxmitframe->attrib.dst);

	/*do_queue_select(padapter, &pxmitframe->attrib);*/

#ifdef CONFIG_AP_MODE
	_rtw_spinlock_bh(&pxmitpriv->lock);
	if (xmitframe_enqueue_for_sleeping_sta(padapter, pxmitframe) == _TRUE) {
		_rtw_spinunlock_bh(&pxmitpriv->lock);
		DBG_COUNTER(padapter->tx_logs.core_tx_ap_enqueue);
		return 1;
	}
	_rtw_spinunlock_bh(&pxmitpriv->lock);
#endif

	/*if (xmit_enqueue(padapter, pxmitframe) == _TRUE)*/
	/*	return 1;*/

	/* pre_xmitframe */
	/*if (rtw_hal_xmit(padapter, pxmitframe) == _FALSE)*/
	if (rtw_intf_data_xmit(padapter, pxmitframe) == _FALSE)
		return 1;

	return 0;
}

/*
 * The main transmit(tx) entry
 *
 * Return
 *	1	enqueue
 *	0	success, hardware will handle this xmit frame(packet)
 *	<0	fail
 */
s32 rtw_xmit(_adapter *padapter, struct sk_buff **ppkt, u16 os_qid)
{
	static systime start = 0;
	static u32 drop_cnt = 0;
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;
	struct xmit_frame *pxmitframe = NULL;
	s32 res;

	DBG_COUNTER(padapter->tx_logs.core_tx);

	if (IS_CH_WAITING(adapter_to_rfctl(padapter)))
		return -1;

	if (rtw_linked_check(padapter) == _FALSE)
		return -1;

	if (start == 0)
		start = rtw_get_current_time();

	pxmitframe = rtw_alloc_xmitframe(pxmitpriv, os_qid);

	if (rtw_get_passing_time_ms(start) > 2000) {
		if (drop_cnt)
			RTW_INFO("DBG_TX_DROP_FRAME %s no more pxmitframe, drop_cnt:%u\n", __FUNCTION__, drop_cnt);
		start = rtw_get_current_time();
		drop_cnt = 0;
	}

	if (pxmitframe == NULL) {
		drop_cnt++;
		/*RTW_INFO("%s-"ADPT_FMT" no more xmitframe\n", __func__, ADPT_ARG(padapter));*/
		DBG_COUNTER(padapter->tx_logs.core_tx_err_pxmitframe);
		return -1;
	}

#ifdef CONFIG_BR_EXT
	if (MLME_IS_STA(padapter) || MLME_IS_ADHOC(padapter)) {
		void *br_port = NULL;

		#if (LINUX_VERSION_CODE <= KERNEL_VERSION(2, 6, 35))
		br_port = padapter->pnetdev->br_port;
		#else
		rcu_read_lock();
		br_port = rcu_dereference(padapter->pnetdev->rx_handler_data);
		rcu_read_unlock();
		#endif

		if (br_port) {
			res = rtw_br_client_tx(padapter, ppkt);
			if (res == -1) {
				rtw_free_xmitframe(pxmitpriv, pxmitframe);
				DBG_COUNTER(padapter->tx_logs.core_tx_err_brtx);
				return -1;
			}
		}
	}
#endif /* CONFIG_BR_EXT */

#ifdef CONFIG_RTW_MESH
	if (MLME_IS_MESH(padapter)) {
		_list f_list;

		res = rtw_mesh_addr_resolve(padapter, pxmitframe, *ppkt, &f_list);
		if (res == RTW_RA_RESOLVING)
			return 1;
		if (res == _FAIL)
			return -1;

		#if CONFIG_RTW_MESH_DATA_BMC_TO_UC
		if (!rtw_is_list_empty(&f_list)) {
			_list *list = get_next(&f_list);
			struct xmit_frame *fframe;

			while ((rtw_end_of_queue_search(&f_list, list)) == _FALSE) {
				fframe = LIST_CONTAINOR(list, struct xmit_frame, list);
				list = get_next(list);
				rtw_list_delete(&fframe->list);

				fframe->pkt = rtw_skb_copy(*ppkt);
				if (!fframe->pkt) {
					if (res == RTW_ORI_NO_NEED)
						res = _SUCCESS;
					rtw_free_xmitframe(pxmitpriv, fframe);
					continue;
				}

				rtw_xmit_posthandle(padapter, fframe, fframe->pkt);
			}
		}
		#endif /* CONFIG_RTW_MESH_DATA_BMC_TO_UC */

		if (res == RTW_ORI_NO_NEED) {
			rtw_free_xmitframe(&padapter->xmitpriv, pxmitframe);
			return 0;
		}
	}
#endif /* CONFIG_RTW_MESH */

	pxmitframe->pkt = NULL; /* let rtw_xmit_posthandle not to free pkt inside */
	res = rtw_xmit_posthandle(padapter, pxmitframe, *ppkt);

	return res;
}

#ifdef RTW_PHL_TX

#ifdef RTW_PHL_TEST_FPGA
u32 test_seq;
#endif

u8 *get_head_from_txreq(_adapter *padapter, struct xmit_frame *pxframe, u8 frag_idx)
{
	return 0;
}

u8 *get_tail_from_txreq(_adapter *padapter, struct xmit_frame *pxframe, u8 frag_idx)
{
	return 0;
}

void dump_pkt(u8 *start, u32 len)
{
	u32 idx = 0;
	for (idx = 0; idx < len; idx++) {
		printk("%02x ", start[idx]);
		if ((idx % 20) == 19)
			printk("\n");
	}
	printk("\n");
}

/* TXREQ_QMGT */
u8 *get_txreq_buffer(_adapter *padapter, u8 **txreq, u8 **pkt_list, u8 **head, u8 **tail)
{
	struct xmit_txreq_buf *ptxreq_buf = NULL;
	_list *plist, *phead;
	_queue *pfree_txreq_queue = &padapter->free_txreq_queue;
#ifdef CONFIG_CORE_TXSC
	u8 i = 0;
#endif

	_rtw_spinlock_bh(&pfree_txreq_queue->lock);
	if (_rtw_queue_empty(pfree_txreq_queue) == _TRUE) {
		padapter->txreq_full_cnt++;
	} else {
		phead = get_list_head(pfree_txreq_queue);
		plist = get_next(phead);
		ptxreq_buf = LIST_CONTAINOR(plist, struct xmit_txreq_buf, list);
		rtw_list_delete(&ptxreq_buf->list);

		padapter->free_txreq_cnt--;
	}
	_rtw_spinunlock_bh(&pfree_txreq_queue->lock);

	if (ptxreq_buf) {

		if (txreq)
			*txreq = ptxreq_buf->txreq;

		if (head)
			*head = ptxreq_buf->head;

		if (tail)
			*tail = ptxreq_buf->tail;

		if (pkt_list)
			*pkt_list = ptxreq_buf->pkt_list;

#ifdef CONFIG_CORE_TXSC
		for (i = 0; i < MAX_TXSC_SKB_NUM; i++)
			ptxreq_buf->pkt[i] = NULL;
		ptxreq_buf->pkt_cnt = 0;
#endif
	}

	return (u8 *)ptxreq_buf;
}

void get_txreq_resources(_adapter *padapter, struct xmit_frame *pxframe,
	u8 **txreq, u8 **pkt_list, u8 **head, u8 **tail)
{
	u32 offset_head = (sizeof(struct rtw_xmit_req) * RTW_MAX_FRAG_NUM);
	u32 offset_tail = offset_head + (SZ_HEAD_BUF * RTW_MAX_FRAG_NUM);
	u32 offset_list = offset_tail + (SZ_TAIL_BUF * RTW_MAX_FRAG_NUM);
	u8 *pbuf = NULL;

	PHLTX_ENTER;

	//rtw_phl_tx todo: error handle, max tx req limit
	padapter->tx_ring_idx++;
	padapter->tx_ring_idx = (padapter->tx_ring_idx % MAX_TX_RING_NUM);

	pbuf = padapter->tx_pool_ring[padapter->tx_ring_idx];
	//memset(pbuf, 0, (SZ_TX_RING*RTW_MAX_FRAG_NUM));

	if (txreq)
		*txreq = pbuf;

	if (head)
		*head = pbuf + offset_head;

	if (tail)
		*tail = pbuf + offset_tail;

	if (pkt_list)
		*pkt_list = pbuf + offset_list;
}

void dump_xmitframe_txreq(_adapter *padapter, struct xmit_frame *pxframe)
{
	struct rtw_xmit_req *txreq = pxframe->phl_txreq;
	u32 idx, idx1 = 0;

	PHLTX_ENTER;
	printk("total txreq=%d \n", pxframe->txreq_cnt);

	for (idx = 0; idx < pxframe->txreq_cnt; idx++) {
		struct rtw_pkt_buf_list *pkt_list = (struct rtw_pkt_buf_list *)txreq->pkt_list;
		printk("txreq[%d] with %d pkts =====\n", idx, txreq->pkt_cnt);
		for (idx1 = 0; idx1 < txreq->pkt_cnt; idx1++) {
			printk("pkt[%d] 0x%p len=%d\n", idx1, (void *)pkt_list->vir_addr, pkt_list->length);
			dump_pkt(pkt_list->vir_addr, pkt_list->length);
			pkt_list++;
		}
		txreq++;
	}
	printk("\n");
}

#ifdef CONFIG_PCI_HCI
void core_recycle_txreq_phyaddr(_adapter *padapter, struct rtw_xmit_req *txreq)
{
	PPCI_DATA pci_data = dvobj_to_pci(padapter->dvobj);
	struct pci_dev *pdev = pci_data->ppcidev;
	struct rtw_pkt_buf_list *pkt_list = (struct rtw_pkt_buf_list *)txreq->pkt_list;
	u32 idx = 0;

	for (idx = 0; idx < txreq->pkt_cnt; idx++) {
		dma_addr_t phy_addr = (pkt_list->phy_addr_l);

#ifdef CONFIG_ARCH_DMA_ADDR_T_64BIT
		{
			u64 phy_addr_h = pkt_list->phy_addr_h;
			phy_addr |= (phy_addr_h << 32);
		}
#endif
		pci_unmap_bus_addr(pdev, &phy_addr, pkt_list->length, DMA_TO_DEVICE);
		pkt_list++;
	}
}

void fill_txreq_phyaddr(_adapter *padapter, struct xmit_frame *pxframe)
{
	PPCI_DATA pci_data = dvobj_to_pci(padapter->dvobj);
	struct pci_dev *pdev = pci_data->ppcidev;
	struct rtw_xmit_req *txreq = pxframe->phl_txreq;
	u32 idx, idx1 = 0;

	PHLTX_ENTER;

	for (idx = 0; idx < pxframe->txreq_cnt; idx++) {
		struct rtw_pkt_buf_list *pkt_list = (struct rtw_pkt_buf_list *)txreq->pkt_list;

		for (idx1 = 0; idx1 < txreq->pkt_cnt; idx1++) {
			dma_addr_t phy_addr = 0;
			pci_get_bus_addr(pdev, pkt_list->vir_addr, &phy_addr, pkt_list->length, DMA_TO_DEVICE);
#ifdef CONFIG_ARCH_DMA_ADDR_T_64BIT
			pkt_list->phy_addr_h =  phy_addr >> 32;
#else
			pkt_list->phy_addr_h = 0x0;
#endif
			pkt_list->phy_addr_l = phy_addr & 0xFFFFFFFF;
			pkt_list++;
		}
		txreq++;
	}
}
#endif

#ifdef CONFIG_XMIT_ACK_BY_REL_RPT
static void fill_txreq_ack_txfb(_adapter *padapter, struct xmit_frame *pxframe)
{
	struct rtw_xmit_req *txreq = pxframe->phl_txreq;
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;

	if (pxframe->ack_report) {
		#ifdef CONFIG_XMIT_ACK_BY_REL_RPT_DBG
		pxmitpriv->ack_txfb.ctx = (void *)pxframe;
		#endif
		txreq->txfb = &pxmitpriv->ack_txfb;
	}
	else {
		txreq->txfb = NULL;
	}
}
#ifdef CONFIG_XMIT_ACK_BY_REL_RPT_DBG
static void core_ack_txfb_hdl_dbg(_adapter *adapter, struct rtw_txfb_t *txfb)
{
	struct xmit_frame *xframe = (struct xmit_frame *)txfb->ctx;
	struct pkt_attrib *pattrib;
	unsigned char *pframe;

	if (txfb->txsts == TX_STATUS_TX_DONE)
		RTW_INFO("%s TX_STATUS_TX_DONE ("ADPT_FMT")\n", __func__, ADPT_ARG(adapter));
	else if (txfb->txsts == TX_STATUS_TX_FAIL_REACH_RTY_LMT)
		RTW_ERR("%s TX_STATUS_TX_FAIL_REACH_RTY_LMT ("ADPT_FMT")\n", __func__, ADPT_ARG(adapter));
	else if (txfb->txsts == TX_STATUS_TX_FAIL_LIFETIME_DROP)
		RTW_ERR("%s TX_STATUS_TX_FAIL_LIFETIME_DROP ("ADPT_FMT")\n", __func__, ADPT_ARG(adapter));
	else if (txfb->txsts == TX_STATUS_TX_FAIL_MACID_DROP)
		RTW_ERR("%s TX_STATUS_TX_FAIL_MACID_DROP ("ADPT_FMT")\n", __func__, ADPT_ARG(adapter));
	else if (txfb->txsts == TX_STATUS_TX_FAIL_SW_DROP)
		RTW_ERR("%s TX_STATUS_TX_FAIL_SW_DROP ("ADPT_FMT")\n", __func__, ADPT_ARG(adapter));
	else if (txfb->txsts == TX_STATUS_TX_FAIL_FORCE_DROP_BY_STUCK)
		RTW_ERR("%s TX_STATUS_TX_FAIL_FORCE_DROP_BY_STUCK ("ADPT_FMT")\n", __func__, ADPT_ARG(adapter));
	else
		RTW_ERR("%s txfb->txsts(%d) unknow ("ADPT_FMT")\n", __func__, txfb->txsts, ADPT_ARG(adapter));

	if (xframe) {
		pattrib = &xframe->attrib;
		RTW_INFO("[TX-STS] macid: %d\n", pattrib->mac_id);
		RTW_INFO("[TX-STS] seqnum: %d\n", pattrib->seqnum);

		pframe = (u8 *)(xframe->buf_addr) + TXDESC_OFFSET;

		if ((get_frame_sub_type(pframe) & WIFI_DATA_NULL) == WIFI_DATA_NULL)
			RTW_INFO("[TX-STS] WIFI_DATA_NULL\n");
		if ((get_frame_sub_type(pframe) & WIFI_QOS_DATA_TYPE) == WIFI_QOS_DATA_TYPE)
			RTW_INFO("[TX-STS] WIFI_QOS_DATA_TYPE\n");
		else if (get_frame_sub_type(pframe) == WIFI_PROBEREQ)
			RTW_INFO("[TX-STS] WIFI_PROBEREQ\n");
		else if (get_frame_sub_type(pframe) == WIFI_PROBERSP)
			RTW_INFO("[TX-STS] WIFI_PROBERSP\n");
	}
}
#endif

static void core_ack_txfb_hdl(struct rtw_txfb_t *txfb)
{
	_adapter *adapter = NULL;

	if (!txfb) {
		RTW_ERR("%s: txfb is NULL!\n", __func__);
		return;
	}

	adapter = (struct _ADAPTER *)txfb->drvpriv;
#ifdef CONFIG_XMIT_ACK_BY_REL_RPT_DBG
	core_ack_txfb_hdl_dbg(adapter, txfb);
#endif

	if (txfb->txsts == TX_STATUS_TX_DONE)
		rtw_ack_tx_done(&adapter->xmitpriv, RTW_SCTX_DONE_SUCCESS);
	else
		rtw_ack_tx_done(&adapter->xmitpriv, RTW_SCTX_DONE_CCX_PKT_FAIL);
}

void rtw_ack_txfb_init(_adapter *padapter, struct rtw_txfb_t *txfb)
{
	txfb->drvpriv = padapter;
	txfb->txfb_cb = core_ack_txfb_hdl;
}
#endif

static void _fill_txreq_list_skb(_adapter *padapter,
	struct rtw_xmit_req *txreq, struct rtw_pkt_buf_list **pkt_list,
	struct sk_buff *skb, u32 *req_sz, s32 *req_offset)
{
#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 4, 0))
	#define skb_frag_off(f)	((f)->page_offset)
#endif
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 2, 0))
	#define skb_frag_page(f) ((f)->page)
	#define skb_frag_size(f) ((f)->size)
#endif
#define PKT_LIST_APPEND(_addr, _len)	do {				\
		u32 __len = _len;					\
		if (__len == 0)						\
			break;						\
		list->vir_addr = _addr;					\
		list->length = __len;					\
		txreq->pkt_cnt++;					\
		txreq->total_len += __len;				\
		list++;							\
		*pkt_list = list;					\
	} while (0)

	struct rtw_pkt_buf_list *list = *pkt_list;
	u8 nr_frags = skb_shinfo(skb)->nr_frags;
	s32 offset = *req_offset;
	u32 rem_sz = *req_sz;
	u32 cur_frag_total, cur_frag_rem;
	int i;

	/* skb head frag */
	cur_frag_total = skb_headlen(skb);

	if (cur_frag_total > offset) {
		cur_frag_rem = rtw_min(cur_frag_total - offset, rem_sz);
		PKT_LIST_APPEND(skb->data + offset, cur_frag_rem);
		rem_sz -= cur_frag_rem;
		offset = 0;
	} else {
		offset -= cur_frag_total;
	}

	for (i = 0; i < skb_shinfo(skb)->nr_frags; i++) {
		skb_frag_t *frag = &skb_shinfo(skb)->frags[i];
		u8 *addr;

		addr = ((void *)page_address(skb_frag_page(frag))) + skb_frag_off(frag);
		cur_frag_total = skb_frag_size(frag);

		if (offset < cur_frag_total) {
			cur_frag_rem = cur_frag_total - offset;

			if (rem_sz < cur_frag_rem) {
				PKT_LIST_APPEND(addr + offset, rem_sz);
				RTW_WARN("%s:%d, size(rem_sz)=%d cur_frag_rem=%d txreq->total_length = %d\n",
					 __func__, __LINE__, rem_sz, cur_frag_rem, txreq->total_len);
				rem_sz = 0;
				break;
			} else {
				PKT_LIST_APPEND(addr + offset, cur_frag_rem);
				RTW_DBG("%s:%d, size=%d txreq->total_length = %d\n",
					__func__, __LINE__, cur_frag_rem, txreq->total_len);
				rem_sz -= cur_frag_rem;
			}

			offset = 0;
		} else {
			offset -= cur_frag_total;
		}
	}

	*req_sz = rem_sz;
	*req_offset = offset;

#undef PKT_LIST_APPEND
#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 4, 0))
	#undef skb_frag_off
#endif
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 2, 0))
	#undef skb_frag_page
	#undef skb_frag_size
#endif
}

static int skb_total_frag_nr(struct sk_buff *head_skb)
{
	struct sk_buff *skb;
	int nr;

	nr = 1 + skb_shinfo(head_skb)->nr_frags;

	skb_walk_frags(head_skb, skb)
		nr += 1 + skb_shinfo(skb)->nr_frags;

	return nr;
}

static void fill_txreq_list_skb(_adapter *padapter,
	struct rtw_xmit_req *txreq, struct rtw_pkt_buf_list **pkt_list,
	struct sk_buff *head_skb, u32 req_sz, s32 offset)
{
	struct sk_buff *skb;

	if (skb_total_frag_nr(head_skb) > NUM_PKT_LIST_PER_TXREQ - 2) {
		rtw_skb_linearize(head_skb);
		RTW_WARN_LMT("skb total frag nr over %d\n", NUM_PKT_LIST_PER_TXREQ - 2);
	}

	_fill_txreq_list_skb(padapter, txreq, pkt_list, head_skb, &req_sz, &offset);

	skb_walk_frags(head_skb, skb)
		_fill_txreq_list_skb(padapter, txreq, pkt_list, skb, &req_sz, &offset);

	if (req_sz != 0)
		RTW_WARN("remain req_sz=%d should be zero\n", req_sz);
}

s32 rtw_core_replace_skb(struct sk_buff **pskb, u32 need_head, u32 need_tail)
{
	struct sk_buff *newskb;
	struct sk_buff *skb = *pskb;

	newskb = rtw_skb_copy(skb);

	if (newskb == NULL)
		return FAIL;

	rtw_skb_free(skb);
	*pskb = newskb;

	return SUCCESS;
}

#ifdef CONFIG_BR_EXT
s32 core_br_client_tx(_adapter *padapter, struct xmit_frame *pxframe, struct sk_buff **pskb)
{
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;

	if (!adapter_use_wds(padapter) && check_fwstate(&padapter->mlmepriv, WIFI_STATION_STATE | WIFI_ADHOC_STATE) == _TRUE) {
		void *br_port = NULL;

		#if (LINUX_VERSION_CODE <= KERNEL_VERSION(2, 6, 35))
		br_port = padapter->pnetdev->br_port;
		#else
		rcu_read_lock();
		br_port = rcu_dereference(padapter->pnetdev->rx_handler_data);
		rcu_read_unlock();
		#endif

		if (br_port) {
			if (rtw_br_client_tx(padapter, pskb) == FAIL) {
				core_tx_free_xmitframe(padapter, pxframe);
				DBG_COUNTER(padapter->tx_logs.core_tx_err_brtx);
				return FAIL;
			}
		}
	}
	return SUCCESS;
}
#endif

s32 core_tx_update_pkt(_adapter *padapter, struct xmit_frame *pxframe, struct sk_buff **pskb)
{
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;
	struct sk_buff *skb_orig = *pskb;

	PHLTX_LOG;

//rtw_phl_tx todo, BR EXT
	if (core_br_client_tx(padapter, pxframe, pskb) == FAIL)
		return FAIL;

	return SUCCESS;
}

s32 core_tx_update_xmitframe(_adapter *padapter,
	struct xmit_frame *pxframe, struct sk_buff **pskb, struct sta_info *psta, u8 type)
{
	pxframe->xftype = type;
	pxframe->pkt = *pskb;

	PHLTX_LOG;

#if 1
	if (pxframe->xftype == RTW_TX_OS) {
		if (update_attrib(padapter, *pskb, &pxframe->attrib) != _SUCCESS)
			return FAIL;
	}
#else
	pxframe->pkt = *pskb;

	if (update_xmitframe_from_hdr(padapter, pxframe) == FAIL)
		return FAIL;

	PHLTX_LOG;

	if (update_xmitframe_qos(padapter, pxframe) == FAIL)
		return FAIL;

	PHLTX_LOG;

	if (update_xmitframe_security(padapter, pxframe) == FAIL)
		return FAIL;

	PHLTX_LOG;

	//if (update_xmitframe_hw(padapter, pxframe) == FAIL)
		//return FAIL;

	PHLTX_LOG;

	if (pxframe->xftype == RTW_TX_OS) {
		if (pxframe->attrib.bswenc
			&& (skb_shared(*pskb) || skb_cloned(*pskb))
			&& (rtw_core_replace_skb(pskb, RTW_MAX_WL_HEAD, RTW_MAX_WL_TAIL) == FAIL))
		return FAIL;
	}
#endif

	PHLTX_LOG;

	return SUCCESS;
}



void get_wl_frag_paras(_adapter *padapter, struct xmit_frame *pxframe,
	u32 *frag_perfr, u32 *wl_frags)
{
	u32 wl_head, wl_tail, payload_totalsz, payload_fragsz, wl_frag_num;
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;

	wl_head = wl_tail = payload_totalsz = 0;

	wl_head += pxframe->attrib.hdrlen;
	wl_tail += RTW_SZ_FCS;
	if (pxframe->attrib.encrypt) {
		wl_head += pxframe->attrib.iv_len;
		wl_tail += pxframe->attrib.icv_len;
	}

	payload_fragsz = pxmitpriv->frag_len - wl_head - wl_tail;

	payload_totalsz = pxframe->attrib.pktlen;
	if (pxframe->xftype == RTW_TX_OS)
		payload_totalsz += RTW_SZ_LLC;
	if (pxframe->attrib.encrypt == _TKIP_)
		payload_totalsz += RTW_TKIP_MIC_LEN;

	if (pxframe->attrib.amsdu)
		wl_frag_num = 1;
	else if (payload_fragsz < payload_totalsz)
		wl_frag_num = RTW_DIV_ROUND_UP(payload_totalsz, payload_fragsz);
	else
		wl_frag_num = 1;

	pxframe->attrib.frag_datalen = *frag_perfr = payload_fragsz;
	pxframe->attrib.nr_frags = *wl_frags = wl_frag_num;
#ifdef CONFIG_CORE_TXSC
	pxframe->attrib.frag_len_txsc = payload_fragsz - (payload_totalsz - pxframe->attrib.pktlen);
#endif
}

u8 fill_txreq_pkt_perfrag_txos(struct _ADAPTER *padapter,
			       struct xmit_frame *pxframe,
			       u32 frag_perfr, u32 wl_frags)
{
	struct rtw_xmit_req *xf_txreq = NULL;
	struct rtw_pkt_buf_list *pkt_list = NULL;
	struct sk_buff *skb = pxframe->pkt;
	u8 *txreq, *head, *tail, *list;
	u32 head_sz, tail_sz, wlan_tail;
	u32 payload_sz, payload_offset;
	u8 idx;
	u8 *wlhdr[RTW_MAX_FRAG_NUM] = {NULL};
	u8 *wltail[RTW_MAX_FRAG_NUM] = {NULL};
	/* TXREQ_QMGT */
	struct xmit_txreq_buf *txreq_buf = NULL;

	PHLTX_ENTER;

	//printk("pxframe->attrib.pkt_hdrlen=%d pxframe->attrib.hdrlen=%d pxframe->attrib.iv_len=%d \n", pxframe->attrib.pkt_hdrlen, pxframe->attrib.hdrlen, pxframe->attrib.iv_len);

	pxframe->txreq_cnt = wl_frags;

	head_sz = pxframe->attrib.hdrlen + (pxframe->attrib.amsdu ? 0 : RTW_SZ_LLC);
	tail_sz = 0;
	if (pxframe->attrib.encrypt) {
		head_sz += pxframe->attrib.iv_len;
		if (pxframe->attrib.encrypt == _TKIP_)
			tail_sz += RTW_TKIP_MIC_LEN;
		if (pxframe->attrib.bswenc)
			tail_sz += pxframe->attrib.icv_len;
	}

	PHLTX_LOG;

	//get_txreq_resources(padapter, pxframe, &txreq, &list, &head, &tail);
	/* TXREQ_QMGT */
	txreq_buf = (struct xmit_txreq_buf *)get_txreq_buffer(padapter, &txreq, &list, &head, &tail);
	if (txreq_buf == NULL) {
		//do this in core_tx_init_xmitframe
		//pxframe->phl_txreq = NULL;
		//pxframe->ptxreq_buf = NULL;

		//free in rtw_core_tx
		//pxframe->pkt = NULL;//for not recycle in abort_core_tx
		goto fail;
	}
#ifdef USE_PREV_WLHDR_BUF /* CONFIG_CORE_TXSC */
	txreq_buf->macid = 0xff;
	txreq_buf->txsc_id = 0xff;
#endif
	pxframe->ptxreq_buf = txreq_buf;

	PHLTX_LOG;

#if 0
	payload = skb->data+pxframe->attrib.pkt_hdrlen;
	printk("num_txreq=%d, hw_head=%d, hw_tail=%d, list=0x%p\n",
		num_txreq, hw_head, hw_tail, (void *)list);

	printk("p:txreq=0x%p, head=0x%p, tail=0x%p, payload=0x%p\n",
		(void *)txreq, (void *)head, (void *)tail, (void *)payload);
#endif

	pxframe->phl_txreq = xf_txreq = (struct rtw_xmit_req *)txreq;
	pkt_list = (struct rtw_pkt_buf_list *)list;
#ifdef CONFIG_CORE_TXSC
	xf_txreq->shortcut_id = 0;
	xf_txreq->treq_type = RTW_PHL_TREQ_TYPE_NORMAL;
#endif

	PHLTX_LOG;

	/* move to first payload position */
	payload_offset = pxframe->attrib.pkt_hdrlen;

	for (idx = 0; idx < wl_frags; idx++) {
		/* for no memset */
		xf_txreq->pkt_cnt = 0;
		xf_txreq->total_len = 0;
		xf_txreq->pkt_list = (u8 *)pkt_list;

		/* fill head into txreq */
		wlhdr[idx] = head;
		pkt_list->vir_addr = head;
		pkt_list->length = head_sz;
		if (idx) {
			/* deduct LLC size if not first fragment */
			pkt_list->length -= RTW_SZ_LLC;
		}
		head += pkt_list->length;
		xf_txreq->pkt_cnt++;
		xf_txreq->total_len += pkt_list->length;
		pkt_list++;

		/* fill payload into txreq */
		if (idx == (wl_frags - 1)) {
			/* last payload size */
			payload_sz = skb->len - payload_offset;
		} else if (idx == 0) {
			/* first payload size should deduct LLC size */
			payload_sz = frag_perfr - RTW_SZ_LLC;
		} else {
			payload_sz = frag_perfr;
		}
		/* xf_txreq would be update and pkt_list++ inside */
		fill_txreq_list_skb(padapter, xf_txreq, &pkt_list, skb,
				    payload_sz, payload_offset);
		payload_offset += payload_sz;

		/* fill tail(if alloc) into txreq */
		if (tail_sz) {
			wlan_tail = tail_sz;
			if ((pxframe->attrib.encrypt == _TKIP_) && (idx != (wl_frags - 1))) {
				/* deduct MIC size if not last fragment with TKIP */
				wlan_tail -= RTW_TKIP_MIC_LEN;
			}
			if (wlan_tail) {
				wltail[idx] = tail;
				pkt_list->vir_addr = tail;
				pkt_list->length = wlan_tail;
				tail += pkt_list->length;
				xf_txreq->pkt_cnt++;
				xf_txreq->total_len += pkt_list->length;
				pkt_list++;
			}
		}

		if (xf_txreq->pkt_cnt > NUM_PKT_LIST_PER_TXREQ)
			RTW_WARN("xf_txreq->pkt_cnt=%d > NUM_PKT_LIST_PER_TXREQ\n",
				 xf_txreq->pkt_cnt);

		xf_txreq++;
	}

	_rtw_memcpy(pxframe->wlhdr, wlhdr, sizeof(wlhdr));
	_rtw_memcpy(pxframe->wltail, wltail, sizeof(wltail));
	PHLTX_EXIT;
	return _SUCCESS;

fail:
	return _FAIL;
}

/* TXREQ_QMGT, MGT_TXREQ_QMGT */
u8 fill_txreq_pkt_mgmt(_adapter *padapter, struct xmit_frame *pxframe)
{
	struct rtw_xmit_req *xf_txreq = NULL;
	struct rtw_pkt_buf_list *pkt_list = NULL;
	//u8 *txreq, *head, *tail, *list, *mgmt = NULL;

	PHLTX_ENTER;

	if (!pxframe->phl_txreq)
		goto fail;

	xf_txreq = pxframe->phl_txreq;
	pkt_list = (struct rtw_pkt_buf_list *)xf_txreq->pkt_list;

	//get_txreq_resources(padapter, pxframe,
	//	(u8 **)&xf_txreq, (u8 **)&pkt_list, NULL, NULL);
	//printk("p:txreq=0x%p, pkt_list=0x%p \n", (void *)xf_txreq, (void *)pkt_list);

	//for no memset
	xf_txreq->pkt_cnt = 0;
	xf_txreq->total_len = 0;
#ifdef CONFIG_CORE_TXSC
	xf_txreq->shortcut_id = 0;
#endif

	pkt_list->vir_addr = pxframe->buf_addr;
	pkt_list->length = pxframe->attrib.pktlen;

	xf_txreq->pkt_cnt = 1;
	//xf_txreq->pkt_list = (u8 *)pkt_list;
	xf_txreq->treq_type = RTW_PHL_TREQ_TYPE_NORMAL;

	pxframe->txreq_cnt = 1;
	//pxframe->phl_txreq = xf_txreq;

	xf_txreq->total_len = xf_txreq->total_len + pxframe->attrib.pktlen;
	//RTW_INFO("%s,%d, xf_txreq->total_length = %d\n", __func__, __LINE__, xf_txreq->total_len);

#ifdef RTW_PHL_TEST_FPGA
{
	struct rtw_ieee80211_hdr *p = (struct rtw_ieee80211_hdr *)pxframe->buf_addr;

	test_seq++;
	test_seq = test_seq%0xFFF;
	SetSeqNum(p, test_seq);
}
#endif

exit:
	return _SUCCESS;

fail:
	return _FAIL;
}

static u8 merge_txreq_to_one_piece(struct _ADAPTER *a,
				   struct xmit_frame *xf)
{
	struct rtw_xmit_req *txreq = NULL;
	struct rtw_pkt_buf_list *pkt_list = NULL;
	int i, j;
	u32 total_sz;
	u8 *buf, *ptr;


	for (i = 0; i < xf->txreq_cnt; i++) {
		txreq = &xf->phl_txreq[i];
		total_sz = txreq->total_len;
		buf = rtw_zmalloc(total_sz);
		if (!buf)
			return _FAIL;
		xf->buf_need_free |= BIT(i);

		ptr = buf;
		for (j = 0; j < txreq->pkt_cnt; j++) {
			pkt_list = &((struct rtw_pkt_buf_list *)txreq->pkt_list)[j];
			_rtw_memcpy(ptr, pkt_list->vir_addr, pkt_list->length);
			ptr += pkt_list->length;
		}
		txreq->pkt_cnt = 1;
		pkt_list = (struct rtw_pkt_buf_list *)txreq->pkt_list;
		pkt_list->vir_addr = buf;
		pkt_list->length = total_sz;
	}

	return _SUCCESS;
}

#ifdef RTW_PHL_TEST_FPGA
#define F_TX_MACID	(0)
#define F_TX_TID		(1)
#define F_TX_TYPE	RTW_PHL_PKT_TYPE_DATA
#define F_TX_RATE	(0x8F) //HRATE_MCS15
#define F_TX_BW		(1)
#define F_TX_DMACH	(0)
#endif

static u8 get_security_cam_id(struct _ADAPTER *padapter, struct xmit_frame *pxframe, u8 keyid)
{
	struct dvobj_priv *d;
	void *phl;
	u8 sec_cam_id = 0;
	struct sta_priv *pstapriv = &padapter->stapriv;
	struct sta_info *sta;
	sint bmcast = IS_MCAST(pxframe->attrib.ra);
	struct link_mlme_priv *pmlmepriv = &(pxframe->attrib.adapter_link->mlmepriv);
	WLAN_BSSID_EX *pbss_network = (WLAN_BSSID_EX *)&pmlmepriv->cur_network.network;

	if (bmcast == _TRUE) {
		/* WEP: use unicast key type to match halmac rule (see: setkey_hdl) */
		if (pxframe->attrib.encrypt == _WEP40_ || pxframe->attrib.encrypt == _WEP104_)
			bmcast = _FALSE;

		sta = rtw_get_stainfo(pstapriv, pbss_network->MacAddress);
	} else {
		sta = rtw_get_stainfo(pstapriv, pxframe->attrib.ra);
	}

	if (!sta) {
		RTW_ERR("%s sta not found\n", __func__);
		rtw_warn_on(1);
		return sec_cam_id;
	}

	d = adapter_to_dvobj(padapter);
	phl = GET_PHL_INFO(d);

	if (keyid >= 4)
		sec_cam_id = rtw_phl_get_sec_cam_idx(phl, sta->phl_sta, keyid,
			RTW_SEC_KEY_BIP);
	else
		sec_cam_id = rtw_phl_get_sec_cam_idx(phl, sta->phl_sta, keyid,
			bmcast ? RTW_SEC_KEY_MULTICAST : RTW_SEC_KEY_UNICAST);

	return sec_cam_id;
}

/* Todo: HE rate mapping not ready */
static const enum rtw_data_rate mrate2phlrate_tbl[] = {
	[MGN_1M] = RTW_DATA_RATE_CCK1,
	[MGN_2M] = RTW_DATA_RATE_CCK2,
	[MGN_5_5M] = RTW_DATA_RATE_CCK5_5,
	[MGN_11M] = RTW_DATA_RATE_CCK11,
	[MGN_6M] = RTW_DATA_RATE_OFDM6,
	[MGN_9M] = RTW_DATA_RATE_OFDM9,
	[MGN_12M] = RTW_DATA_RATE_OFDM12,
	[MGN_18M] = RTW_DATA_RATE_OFDM18,
	[MGN_24M] = RTW_DATA_RATE_OFDM24,
	[MGN_36M] = RTW_DATA_RATE_OFDM36,
	[MGN_48M] = RTW_DATA_RATE_OFDM48,
	[MGN_54M] = RTW_DATA_RATE_OFDM54,
	[MGN_MCS0] = RTW_DATA_RATE_MCS0,
	[MGN_MCS1] = RTW_DATA_RATE_MCS1,
	[MGN_MCS2] = RTW_DATA_RATE_MCS2,
	[MGN_MCS3] = RTW_DATA_RATE_MCS3,
	[MGN_MCS4] = RTW_DATA_RATE_MCS4,
	[MGN_MCS5] = RTW_DATA_RATE_MCS5,
	[MGN_MCS6] = RTW_DATA_RATE_MCS6,
	[MGN_MCS7] = RTW_DATA_RATE_MCS7,
	[MGN_MCS8] = RTW_DATA_RATE_MCS8,
	[MGN_MCS9] = RTW_DATA_RATE_MCS9,
	[MGN_MCS10] = RTW_DATA_RATE_MCS10,
	[MGN_MCS11] = RTW_DATA_RATE_MCS11,
	[MGN_MCS12] = RTW_DATA_RATE_MCS12,
	[MGN_MCS13] = RTW_DATA_RATE_MCS13,
	[MGN_MCS14] = RTW_DATA_RATE_MCS14,
	[MGN_MCS15] = RTW_DATA_RATE_MCS15,
	[MGN_MCS16] = RTW_DATA_RATE_MCS16,
	[MGN_MCS17] = RTW_DATA_RATE_MCS17,
	[MGN_MCS18] = RTW_DATA_RATE_MCS18,
	[MGN_MCS19] = RTW_DATA_RATE_MCS19,
	[MGN_MCS20] = RTW_DATA_RATE_MCS20,
	[MGN_MCS21] = RTW_DATA_RATE_MCS21,
	[MGN_MCS22] = RTW_DATA_RATE_MCS22,
	[MGN_MCS23] = RTW_DATA_RATE_MCS23,
	[MGN_MCS24] = RTW_DATA_RATE_MCS24,
	[MGN_MCS25] = RTW_DATA_RATE_MCS25,
	[MGN_MCS26] = RTW_DATA_RATE_MCS26,
	[MGN_MCS27] = RTW_DATA_RATE_MCS27,
	[MGN_MCS28] = RTW_DATA_RATE_MCS28,
	[MGN_MCS29] = RTW_DATA_RATE_MCS29,
	[MGN_MCS30] = RTW_DATA_RATE_MCS30,
	[MGN_MCS31] = RTW_DATA_RATE_MCS31,
	[MGN_VHT1SS_MCS0] = RTW_DATA_RATE_VHT_NSS1_MCS0,
	[MGN_VHT1SS_MCS1] = RTW_DATA_RATE_VHT_NSS1_MCS1,
	[MGN_VHT1SS_MCS2] = RTW_DATA_RATE_VHT_NSS1_MCS2,
	[MGN_VHT1SS_MCS3] = RTW_DATA_RATE_VHT_NSS1_MCS3,
	[MGN_VHT1SS_MCS4] = RTW_DATA_RATE_VHT_NSS1_MCS4,
	[MGN_VHT1SS_MCS5] = RTW_DATA_RATE_VHT_NSS1_MCS5,
	[MGN_VHT1SS_MCS6] = RTW_DATA_RATE_VHT_NSS1_MCS6,
	[MGN_VHT1SS_MCS7] = RTW_DATA_RATE_VHT_NSS1_MCS7,
	[MGN_VHT1SS_MCS8] = RTW_DATA_RATE_VHT_NSS1_MCS8,
	[MGN_VHT1SS_MCS9] = RTW_DATA_RATE_VHT_NSS1_MCS9,
	[MGN_VHT2SS_MCS0] = RTW_DATA_RATE_VHT_NSS2_MCS0,
	[MGN_VHT2SS_MCS1] = RTW_DATA_RATE_VHT_NSS2_MCS1,
	[MGN_VHT2SS_MCS2] = RTW_DATA_RATE_VHT_NSS2_MCS2,
	[MGN_VHT2SS_MCS3] = RTW_DATA_RATE_VHT_NSS2_MCS3,
	[MGN_VHT2SS_MCS4] = RTW_DATA_RATE_VHT_NSS2_MCS4,
	[MGN_VHT2SS_MCS5] = RTW_DATA_RATE_VHT_NSS2_MCS5,
	[MGN_VHT2SS_MCS6] = RTW_DATA_RATE_VHT_NSS2_MCS6,
	[MGN_VHT2SS_MCS7] = RTW_DATA_RATE_VHT_NSS2_MCS7,
	[MGN_VHT2SS_MCS8] = RTW_DATA_RATE_VHT_NSS2_MCS8,
	[MGN_VHT2SS_MCS9] = RTW_DATA_RATE_VHT_NSS2_MCS9,
	[MGN_VHT3SS_MCS0] = RTW_DATA_RATE_VHT_NSS3_MCS0,
	[MGN_VHT3SS_MCS1] = RTW_DATA_RATE_VHT_NSS3_MCS1,
	[MGN_VHT3SS_MCS2] = RTW_DATA_RATE_VHT_NSS3_MCS2,
	[MGN_VHT3SS_MCS3] = RTW_DATA_RATE_VHT_NSS3_MCS3,
	[MGN_VHT3SS_MCS4] = RTW_DATA_RATE_VHT_NSS3_MCS4,
	[MGN_VHT3SS_MCS5] = RTW_DATA_RATE_VHT_NSS3_MCS5,
	[MGN_VHT3SS_MCS6] = RTW_DATA_RATE_VHT_NSS3_MCS6,
	[MGN_VHT3SS_MCS7] = RTW_DATA_RATE_VHT_NSS3_MCS7,
	[MGN_VHT3SS_MCS8] = RTW_DATA_RATE_VHT_NSS3_MCS8,
	[MGN_VHT3SS_MCS9] = RTW_DATA_RATE_VHT_NSS3_MCS9,
	[MGN_VHT4SS_MCS0] = RTW_DATA_RATE_VHT_NSS4_MCS0,
	[MGN_VHT4SS_MCS1] = RTW_DATA_RATE_VHT_NSS4_MCS1,
	[MGN_VHT4SS_MCS2] = RTW_DATA_RATE_VHT_NSS4_MCS2,
	[MGN_VHT4SS_MCS3] = RTW_DATA_RATE_VHT_NSS4_MCS3,
	[MGN_VHT4SS_MCS4] = RTW_DATA_RATE_VHT_NSS4_MCS4,
	[MGN_VHT4SS_MCS5] = RTW_DATA_RATE_VHT_NSS4_MCS5,
	[MGN_VHT4SS_MCS6] = RTW_DATA_RATE_VHT_NSS4_MCS6,
	[MGN_VHT4SS_MCS7] = RTW_DATA_RATE_VHT_NSS4_MCS7,
	[MGN_VHT4SS_MCS8] = RTW_DATA_RATE_VHT_NSS4_MCS8,
	[MGN_VHT4SS_MCS9] = RTW_DATA_RATE_VHT_NSS4_MCS9,
	[MGN_HE1SS_MCS0] = RTW_DATA_RATE_HE_NSS1_MCS0,
	[MGN_HE1SS_MCS1] = RTW_DATA_RATE_HE_NSS1_MCS1,
	[MGN_HE1SS_MCS2] = RTW_DATA_RATE_HE_NSS1_MCS2,
	[MGN_HE1SS_MCS3] = RTW_DATA_RATE_HE_NSS1_MCS3,
	[MGN_HE1SS_MCS4] = RTW_DATA_RATE_HE_NSS1_MCS4,
	[MGN_HE1SS_MCS5] = RTW_DATA_RATE_HE_NSS1_MCS5,
	[MGN_HE1SS_MCS6] = RTW_DATA_RATE_HE_NSS1_MCS6,
	[MGN_HE1SS_MCS7] = RTW_DATA_RATE_HE_NSS1_MCS7,
	[MGN_HE1SS_MCS8] = RTW_DATA_RATE_HE_NSS1_MCS8,
	[MGN_HE1SS_MCS9] = RTW_DATA_RATE_HE_NSS1_MCS9,
	[MGN_HE1SS_MCS10] = RTW_DATA_RATE_HE_NSS1_MCS10,
	[MGN_HE1SS_MCS11] = RTW_DATA_RATE_HE_NSS1_MCS11,
	[MGN_HE2SS_MCS0] = RTW_DATA_RATE_HE_NSS2_MCS0,
	[MGN_HE2SS_MCS1] = RTW_DATA_RATE_HE_NSS2_MCS1,
	[MGN_HE2SS_MCS2] = RTW_DATA_RATE_HE_NSS2_MCS2,
	[MGN_HE2SS_MCS3] = RTW_DATA_RATE_HE_NSS2_MCS3,
	[MGN_HE2SS_MCS4] = RTW_DATA_RATE_HE_NSS2_MCS4,
	[MGN_HE2SS_MCS5] = RTW_DATA_RATE_HE_NSS2_MCS5,
	[MGN_HE2SS_MCS6] = RTW_DATA_RATE_HE_NSS2_MCS6,
	[MGN_HE2SS_MCS7] = RTW_DATA_RATE_HE_NSS2_MCS7,
	[MGN_HE2SS_MCS8] = RTW_DATA_RATE_HE_NSS2_MCS8,
	[MGN_HE2SS_MCS9] = RTW_DATA_RATE_HE_NSS2_MCS9,
	[MGN_HE2SS_MCS10] = RTW_DATA_RATE_HE_NSS2_MCS10,
	[MGN_HE2SS_MCS11] = RTW_DATA_RATE_HE_NSS2_MCS11,
	[MGN_HE3SS_MCS0] = RTW_DATA_RATE_HE_NSS3_MCS0,
	[MGN_HE3SS_MCS1] = RTW_DATA_RATE_HE_NSS3_MCS1,
	[MGN_HE3SS_MCS2] = RTW_DATA_RATE_HE_NSS3_MCS2,
	[MGN_HE3SS_MCS3] = RTW_DATA_RATE_HE_NSS3_MCS3,
	[MGN_HE3SS_MCS4] = RTW_DATA_RATE_HE_NSS3_MCS4,
	[MGN_HE3SS_MCS5] = RTW_DATA_RATE_HE_NSS3_MCS5,
	[MGN_HE3SS_MCS6] = RTW_DATA_RATE_HE_NSS3_MCS6,
	[MGN_HE3SS_MCS7] = RTW_DATA_RATE_HE_NSS3_MCS7,
	[MGN_HE3SS_MCS8] = RTW_DATA_RATE_HE_NSS3_MCS8,
	[MGN_HE3SS_MCS9] = RTW_DATA_RATE_HE_NSS3_MCS9,
	[MGN_HE3SS_MCS10] = RTW_DATA_RATE_HE_NSS3_MCS10,
	[MGN_HE3SS_MCS11] = RTW_DATA_RATE_HE_NSS3_MCS11,
	[MGN_HE4SS_MCS0] = RTW_DATA_RATE_HE_NSS4_MCS0,
	[MGN_HE4SS_MCS1] = RTW_DATA_RATE_HE_NSS4_MCS1,
	[MGN_HE4SS_MCS2] = RTW_DATA_RATE_HE_NSS4_MCS2,
	[MGN_HE4SS_MCS3] = RTW_DATA_RATE_HE_NSS4_MCS3,
	[MGN_HE4SS_MCS4] = RTW_DATA_RATE_HE_NSS4_MCS4,
	[MGN_HE4SS_MCS5] = RTW_DATA_RATE_HE_NSS4_MCS5,
	[MGN_HE4SS_MCS6] = RTW_DATA_RATE_HE_NSS4_MCS6,
	[MGN_HE4SS_MCS7] = RTW_DATA_RATE_HE_NSS4_MCS7,
	[MGN_HE4SS_MCS8] = RTW_DATA_RATE_HE_NSS4_MCS8,
	[MGN_HE4SS_MCS9] = RTW_DATA_RATE_HE_NSS4_MCS9,
	[MGN_HE4SS_MCS10] = RTW_DATA_RATE_HE_NSS4_MCS10,
	[MGN_HE4SS_MCS11] = RTW_DATA_RATE_HE_NSS4_MCS11,
};

/*
 * _rate_mrate2phl() - convert data rate from mrate to PHL(MAC)
 * @sta:	struct sta_info *
 * @mrate:	date rate of mrate type, enum MGN_RATE
 *
 * Convert data rate from MGN_RATE definition to PHL's definition.
 *
 * Return PHL's data rate definition "enum rtw_data_rate".
 * 0x0~0xB: CCK 1M ~ OFDM 54M
 * 0x80~0x9F: HT MCS0~MCS31
 * 0x100~0x109: VHT 1SS MCS0~MCS9
 * 0x110~0x119: VHT 2SS MCS0~MCS9
 * 0x120~0x129: VHT 3SS MCS0~MCS9
 * 0x130~0x139: VHT 4SS MCS0~MCS9
 * 0x180~0x18B: HE 1SS MCS0~MCS11
 * 0x190~0x19B: HE 2SS MCS0~MCS11
 * 0x1A0~0x1AB: HE 3SS MCS0~MCS11
 * 0x1B0~0x1BB: HE 4SS MCS0~MCS11
 */
static enum rtw_data_rate _rate_mrate2phl(enum MGN_RATE mrate)
{
	enum rtw_data_rate phl = RTW_DATA_RATE_CCK1;


	if (mrate < ARRAY_SIZE(mrate2phlrate_tbl))
		phl = mrate2phlrate_tbl[mrate];

	if ((mrate != MGN_1M) && (phl == RTW_DATA_RATE_CCK1))
		RTW_WARN("%s: Invalid rate 0x%x\n", __func__, mrate);

	return phl;
}

/*
 * _rate_drv2phl() - convert data rate from drive to PHL(MAC)
 * @sta:	struct sta_info *
 * @rate:	date rate of driver
 *		0x0~0xB: CCK 1M ~ OFDM 54M
 *		>0xB: HT/VHT/HE use the same bits field to represent each
 *		      data rate, so these bits's real definition depended on
 *		      sta's wireless mode.
 *
 * Convert driver's data rate definition to PHL's definition.
 *
 * Return PHL's data rate definition "enum rtw_data_rate".
 */
static enum rtw_data_rate _rate_drv2phl(struct sta_info *sta, u8 rate)
{
	enum rtw_data_rate phl = RTW_DATA_RATE_CCK1;
	u8 ht_support = 0, vht_support = 0, he_support = 0;


	if (rate < 12) {
		/* B/G mode, CCK/OFDM rate */
		return (enum rtw_data_rate)rate;
	}

#ifdef CONFIG_80211N_HT
	if (sta->htpriv.ht_option == _TRUE)
		ht_support = 1;
#ifdef CONFIG_80211AC_VHT
	if (sta->vhtpriv.vht_option == _TRUE)
		vht_support = 1;
#ifdef CONFIG_80211AX_HE
	if (sta->hepriv.he_option == _TRUE)
		he_support = 1;
#endif /* CONFIG_80211AX_HE */
#endif /* CONFIG_80211AC_VHT */
#endif /* CONFIG_80211N_HT */

	rate -= 12;
	if (he_support) {
		if (rate < 12)
			phl = RTW_DATA_RATE_HE_NSS1_MCS0 + rate;
		else if (rate < 24)
			phl = RTW_DATA_RATE_HE_NSS2_MCS0 + (rate - 12);
		else if (rate < 36)
			phl = RTW_DATA_RATE_HE_NSS3_MCS0 + (rate - 24);
		else
			phl = RTW_DATA_RATE_HE_NSS4_MCS0 + (rate - 36);
	} else if (vht_support) {
		if (rate < 10)
			phl = RTW_DATA_RATE_VHT_NSS1_MCS0 + rate;
		else if (rate < 20)
			phl = RTW_DATA_RATE_VHT_NSS2_MCS0 + (rate - 10);
		else if (rate < 30)
			phl = RTW_DATA_RATE_VHT_NSS3_MCS0 + (rate - 20);
		else
			phl = RTW_DATA_RATE_VHT_NSS4_MCS0 + (rate - 30);
	} else if (ht_support) {
		phl = RTW_DATA_RATE_MCS0 + rate;
	}

	return phl;
}

void fill_txreq_mdata(_adapter *padapter, struct xmit_frame *pxframe)
{
	struct rtw_xmit_req *txreq = pxframe->phl_txreq;
	struct sta_info *psta = pxframe->attrib.psta;
	struct rtw_phl_stainfo_t *phl_sta = NULL;
	struct rtw_t_meta_data *mdata = &(txreq->mdata);
#ifdef BMC_ON_HIQ
       struct sta_priv *pstapriv = &padapter->stapriv;
#endif
	u32 idx = 0;
	u8 htc_option = _FALSE;
#ifdef CONFIG_XMIT_ACK_BY_CCX_RPT
	struct xmit_priv *pxmitpriv = &(GET_PRIMARY_ADAPTER(padapter))->xmitpriv;
#endif
	struct _ADAPTER_LINK *padapter_link = pxframe->attrib.adapter_link;
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	struct rtw_phl_com_t *phl_com = GET_PHL_COM(dvobj);

	PHLTX_LOG;

	if (pxframe->attrib.order)
		htc_option = _TRUE;

	/* packet identify */
	if (pxframe->attrib.type == WIFI_MGT_TYPE)
		mdata->type = RTW_PHL_PKT_TYPE_MGNT;
	else
		mdata->type = RTW_PHL_PKT_TYPE_DATA;

	mdata->macid = pxframe->attrib.mac_id;

	/* enable wd info by default */
	mdata->wdinfo_en = 1;

	/* packet content */
	mdata->hdr_len = pxframe->attrib.hdrlen >> 1; /* should be half of the real length */
	mdata->hw_seq_mode = 0;
	mdata->sw_seq = pxframe->attrib.seqnum;
	mdata->nav_use_hdr = 0;
	mdata->hw_sec_iv = padapter->dvobj->phl_com->dev_cap.sec_cap.hw_sec_iv;
	mdata->sw_sec_iv = 0;

	/* packet security */
	if (pxframe->attrib.encrypt == _NO_PRIVACY_ || pxframe->attrib.bswenc == _TRUE) {
		mdata->sec_hw_enc = _FALSE;
		mdata->sec_type = RTW_ENC_NONE;
	} else {
		mdata->sec_hw_enc = _TRUE;
		mdata->sec_type = rtw_sec_algo_drv2phl(pxframe->attrib.encrypt);
		if (padapter->dvobj->phl_com->dev_cap.sec_cap.hw_form_hdr &&
		    mdata->sec_type != RTW_ENC_WAPI && mdata->hw_sec_iv == 0) {
			mdata->sec_keyid = pxframe->attrib.key_idx;
			/* Fill PN of IV */
			for (idx = 0; idx < 6; idx++)
				mdata->iv[idx] = (psta->dot11txpn.val >> 8 * idx) & 0xff;
			RTW_DBG("%s: keyid=%d, iv=" PN_FMT "\n", __func__, mdata->sec_keyid, PN_ARG(mdata->iv));
			update_attrib_sec_iv_info(padapter, &pxframe->attrib);
		}

		if (padapter->dvobj->phl_com->dev_cap.sec_cap.hw_tx_search_key) {
			mdata->force_key_en = false;
		} else {
			mdata->force_key_en = true;
			mdata->sec_cam_idx = get_security_cam_id(padapter, pxframe, pxframe->attrib.key_idx);
		}
	}
	/* Currently dump secrity settings for dbg */
	RTW_DBG("sec_type= %d sec_hw_enc= %d sec_cam_idx= %d \n",
		mdata->sec_type, mdata->sec_hw_enc, mdata->sec_cam_idx);

	/* packet capability */
	if (pxframe->attrib.ampdu_en == _TRUE) {
		mdata->ampdu_en = 1;
		mdata->bk = 0;
		mdata->ampdu_density = pxframe->attrib.ampdu_spacing;

		/* set tx ampdu number */
		if (ALINK_GET_HWBAND(padapter_link) < HW_BAND_MAX)
			mdata->max_agg_num = phl_com->phy_cap[ALINK_GET_HWBAND(padapter_link)].txagg_num;
		else
			mdata->max_agg_num = 0x40; /* temporally fix to 64 */
#ifdef RTW_WKARD_LIMIT_MAX_TXAGG
		/* The maximum value is temporarily limited to 64 */
		if (mdata->max_agg_num > 0x40)
			mdata->max_agg_num = 0x40;
#endif
	} else {
		mdata->ampdu_en = 0;
		mdata->bk = 1;
	}
	mdata->dis_data_rate_fb = 0;
	mdata->dis_rts_rate_fb = 0;
	mdata->data_tx_cnt_lmt_en = 0;
	mdata->data_tx_cnt_lmt = 0;

	/* Set DATA_RTY_LOWEST_RATE: 2.4G to CCK1M & 5G to OFDM6M */
	if (rtw_get_oper_band(padapter, padapter_link) == BAND_ON_24G)
		mdata->data_rty_lowest_rate = RTW_DATA_RATE_CCK1;
	else if ((rtw_get_oper_band(padapter, padapter_link) == BAND_ON_5G) ||
		 (rtw_get_oper_band(padapter, padapter_link) == BAND_ON_6G))
		mdata->data_rty_lowest_rate = RTW_DATA_RATE_OFDM6;
	else
		RTW_WARN("%s: mdata->data_rty_lowest_rate is not set.\n", __func__);

	mdata->life_time_sel = 0;
	mdata->rts_en = pxframe->attrib.rtsen;
	mdata->cts2self = pxframe->attrib.cts2self;
	mdata->hw_rts_en = pxframe->attrib.hw_rts_en;
	mdata->rts_cca_mode = 0;

	mdata->f_bw = pxframe->attrib.bwmode;
	/* Todo: GI and LTF not ready for HE */
	mdata->f_gi_ltf = pxframe->attrib.sgi;

	mdata->mc = IS_MCAST(pxframe->attrib.ra) ? 1 : 0;
	mdata->bc = MacAddr_isBcst(pxframe->attrib.ra) ? 1 : 0;

#ifdef CONFIG_80211AX_HE
	if (psta && htc_option)
		mdata->a_ctrl_bsr = 1;
#endif
	mdata->raw = 0;

	if (pxframe->attrib.type == WIFI_MGT_TYPE) {
		mdata->cat = RTW_PHL_RING_CAT_MGNT;
		mdata->tid = rtw_phl_cvt_cat_to_tid(mdata->cat);

	} else if (pxframe->attrib.type == WIFI_DATA_TYPE) {
		#ifdef BMC_ON_HIQ
		if ((mdata->mc || mdata->bc) &&
		    (rtw_tim_map_anyone_be_set(padapter, pstapriv->sta_dz_bitmap))) {
			mdata->cat = RTW_PHL_RING_CAT_HIQ; /* HIQ */
			mdata->tid = rtw_phl_cvt_cat_to_tid(mdata->cat);
			mdata->mbssid = 0; /* ToDo: Consider MBSSID */
		} else
		#endif
		{
			/*pxframe->attrib.subtype = WIFI_QOS_DATA_TYPE*/
			mdata->tid = pxframe->attrib.priority;
			mdata->cat = rtw_phl_cvt_tid_to_cat(pxframe->attrib.priority);
		}
	} else { /*WIFI_CTRL_TYPE*/
		RTW_ERR("%s invalid attrib.type :%d", __func__, pxframe->attrib.type);
		rtw_warn_on(1);
	}


#ifdef CONFIG_CORE_TXSC
	mdata->ampdu_density = 0;
	mdata->userate_sel = 0;
#endif

	if (pxframe->xftype == RTW_TX_DRV_MGMT) {
		mdata->userate_sel = 1;
		mdata->f_rate = _rate_mrate2phl(pxframe->attrib.rate);
	} else {
		/* low rate for EAPOL/ARP/DHCP */
		if ((pxframe->attrib.ether_type == 0x888e) ||
			(pxframe->attrib.ether_type == 0x0806) ||
			(pxframe->attrib.dhcp_pkt == 1)) {

			mdata->userate_sel = 1;
			if (IS_CCK_RATE(padapter_link->mlmeextpriv.tx_rate))
				mdata->f_rate = RTW_DATA_RATE_CCK1;
			else
				mdata->f_rate = RTW_DATA_RATE_OFDM6;
			mdata->f_bw = CHANNEL_WIDTH_20;
		} else {
			/* fix rate for non specail packet */
			if (padapter->fix_rate != NO_FIX_RATE) {
				mdata->userate_sel = 1;
				mdata->f_rate = GET_FIX_RATE(padapter->fix_rate);
				mdata->f_gi_ltf = GET_FIX_RATE_SGI(padapter->fix_rate);
				if (!padapter->data_fb)
					mdata->dis_data_rate_fb = 1;
			} else {
				mdata->userate_sel = 0;
			}

			if (padapter->fix_bw != NO_FIX_BW)
				mdata->f_bw = padapter->fix_bw;
		}
	}
	mdata->f_er = 0;
	mdata->f_dcm = 0;
	mdata->f_stbc = pxframe->attrib.stbc;
	mdata->f_ldpc = pxframe->attrib.ldpc;

	mdata->band = 0;
	mdata->dma_ch = 0;
	mdata->spe_rpt = 0;
	mdata->sw_define = 0;

#ifdef CONFIG_XMIT_ACK_BY_CCX_RPT
	if (pxframe->ack_report) {
		mdata->spe_rpt = 1;
		mdata->sw_define = pxmitpriv->ack_tx_seq_no;
#ifdef RTW_WKARD_CCX_RPT_LIMIT_CTRL
		mdata->data_tx_cnt_lmt_en = 1;
		mdata->data_tx_cnt_lmt = 8;
#endif
	}
#endif

#ifdef CONFIG_CORE_TXSC
	mdata->pktlen = txreq->total_len;
#endif

#ifdef RTW_PHL_TEST_FPGA
	mdata->type = F_TX_TYPE;
	mdata->macid = F_TX_MACID;
	mdata->tid = F_TX_TID;
	mdata->dma_ch = F_TX_DMACH;
	//mdata->band = cap->band;
	mdata->f_rate = F_TX_RATE;
	mdata->f_bw = F_TX_BW;
	mdata->f_gi_ltf = 0;
	mdata->f_stbc = 0;
	mdata->f_ldpc = 0;

	mdata->userate_sel = 1;
	mdata->dis_data_rate_fb = 1;
	mdata->dis_rts_rate_fb = 1;
#endif

#ifdef RTW_PHL_DBG_CMD
	if (pxframe->xftype != RTW_TX_DRV_MGMT) {
		if (padapter->txForce_enable) {
			if (padapter->txForce_rate != INV_TXFORCE_VAL)
				mdata->f_rate = padapter->txForce_rate;
			if (padapter->txForce_agg != INV_TXFORCE_VAL)
				mdata->ampdu_en = padapter->txForce_agg;
			if (padapter->txForce_aggnum != INV_TXFORCE_VAL)
				mdata->max_agg_num = padapter->txForce_aggnum;
			if (padapter->txForce_gi != INV_TXFORCE_VAL)
				mdata->f_gi_ltf = padapter->txForce_gi;
		}
	}
#endif

#ifdef CONFIG_CORE_TXSC
	_print_txreq_mdata(mdata, __func__);
#endif

	if (pxframe->txreq_cnt > 1) {
		struct rtw_t_meta_data *mdata_tmp;
		txreq++;
		for (idx = 1; idx < pxframe->txreq_cnt; idx++) {
#ifdef CONFIG_CORE_TXSC
			mdata->pktlen = txreq->total_len;
#endif
			mdata_tmp = &(txreq->mdata);
			memcpy(mdata_tmp, mdata, sizeof(struct rtw_t_meta_data));
			txreq++;
		}
	}

}


void fill_txreq_others(_adapter *padapter, struct xmit_frame *pxframe)
{
	struct rtw_xmit_req *txreq = pxframe->phl_txreq;
	u32 idx = 0;

	PHLTX_ENTER;

	for (idx = 0; idx < pxframe->txreq_cnt; idx++) {
		txreq->os_priv = pxframe;
		txreq++;
	}
}

u8 core_wlan_fill_txreq_pre(_adapter *padapter, struct xmit_frame *pxframe)
{
	u32 frag_perfr, wl_frags = 0;

	if (pxframe->xftype == RTW_TX_OS) {
		get_wl_frag_paras(padapter, pxframe, &frag_perfr, &wl_frags);
		if (fill_txreq_pkt_perfrag_txos(padapter, pxframe, frag_perfr, wl_frags) == _FAIL)
			return _FAIL;
	} else if (pxframe->xftype == RTW_TX_DRV_MGMT) {
		if (fill_txreq_pkt_mgmt(padapter, pxframe) == _FAIL)
			return _FAIL;
	}

	return _SUCCESS;
}

void core_wlan_fill_txreq_post(_adapter *padapter, struct xmit_frame *pxframe)
{
	fill_txreq_mdata(padapter, pxframe);
	fill_txreq_others(padapter, pxframe);

#ifdef CONFIG_PCI_HCI
	/*must be called after all pkt contents modified (cache sync)*/
	fill_txreq_phyaddr(padapter, pxframe);
#ifdef CONFIG_XMIT_ACK_BY_REL_RPT
	fill_txreq_ack_txfb(padapter, pxframe);
#endif
#endif

}

void core_wlan_fill_head(_adapter *padapter, struct xmit_frame *pxframe)
{
	u32 idx = 0;
	if (pxframe->xftype == RTW_TX_OS) {
		for (idx = 0; idx < pxframe->attrib.nr_frags; idx++) {
			u8 *pwlanhdr = pxframe->wlhdr[idx];

			if (!pwlanhdr) {
				PHLTX_ERR;
				continue;
			}

			rtw_make_wlanhdr(padapter, pwlanhdr, &pxframe->attrib); //rtw_core_make_wlanhdr(padapter, pwlanhdr, pxframe);

			if (idx == (pxframe->attrib.nr_frags - 1))
				ClearMFrag(pwlanhdr);
			else
				SetMFrag(pwlanhdr);

			if (pxframe->attrib.iv_len) {
				update_attrib_sec_iv_info(padapter, &pxframe->attrib);
				_rtw_memcpy((pwlanhdr+pxframe->attrib.hdrlen), pxframe->attrib.iv, pxframe->attrib.iv_len);
			}

			if (idx == 0 && !pxframe->attrib.amsdu) {
				/* Add LLC/SNAP to first fragment */
				rtw_put_snap(pwlanhdr+pxframe->attrib.hdrlen+pxframe->attrib.iv_len,
					     pxframe->attrib.ether_type);
			}

#ifdef RTW_PHL_TEST_FPGA
{
	struct rtw_ieee80211_hdr *p = (struct rtw_ieee80211_hdr *)pwlanhdr;
	unsigned short *fctrl;
	unsigned int pktlen = 0;
	u16 *qc;

	test_seq++;
	test_seq = test_seq%0xFFF;
	SetSeqNum(p, test_seq);
}
#endif

		}
	}
}


void core_wlan_fill_tail(_adapter *padapter, struct xmit_frame *pxframe)
{
	;

}


u8 core_wlan_fill_tkip_mic(_adapter *padapter, struct xmit_frame *pxframe)
{
	u8 *llc = NULL;
	u8 *payload = NULL;
	u8 mic[8] = {0x0};
	struct mic_data micdata;
	struct pkt_attrib *pattrib = &pxframe->attrib;
	struct security_priv *psecuritypriv = &padapter->securitypriv;
	s8 bmcst = IS_MCAST(pattrib->ra);
	u8 priority[4] = {0x0};
	int i = 0;
	struct rtw_xmit_req *xf_txreq = pxframe->phl_txreq;
	struct rtw_pkt_buf_list *pkt_list = NULL;
	struct _ADAPTER_LINK *padapter_link = pattrib->adapter_link;
	struct link_security_priv *lsecuritypriv = &padapter_link->securitypriv;

	if (pattrib->encrypt == _TKIP_) {
		u8 null_key[16] = {0x0};

		/* set TKIP MIC key */
		if (bmcst) {
			if (_rtw_memcmp(lsecuritypriv->dot118021XGrptxmickey[lsecuritypriv->dot118021XGrpKeyid].skey,
					null_key, 16) == _TRUE)
				return _FAIL;

			rtw_secmicsetkey(&micdata, lsecuritypriv->dot118021XGrptxmickey[lsecuritypriv->dot118021XGrpKeyid].skey);
		} else {
			if (_rtw_memcmp(
					&pattrib->dot11tkiptxmickey.skey[0],
					null_key, 16) == _TRUE)
				return _FAIL;

			rtw_secmicsetkey(&micdata, &pattrib->dot11tkiptxmickey.skey[0]);
		}

		/* set DA, SA */
		rtw_secmicappend(&micdata, &pattrib->dst[0], 6);
		rtw_secmicappend(&micdata, &pattrib->src[0], 6);

		if (pattrib->qos_en)
			priority[0] = pattrib->priority;

		/* set priority */
		rtw_secmicappend(&micdata, &priority[0], 4);

		/* set LLC; TBD: should check if LLC is existed or not */
		llc = pxframe->wlhdr[0] + pxframe->attrib.hdrlen + pxframe->attrib.iv_len;
		rtw_secmicappend(&micdata, llc, SNAP_SIZE + sizeof(u16));

		/* set MSDU payload */
		pkt_list = (struct rtw_pkt_buf_list *) xf_txreq->pkt_list;

		/*ignore hdr move to payload*/
		pkt_list ++;
		/*for loop ignore tail*/
		for (i = 1; i < xf_txreq->pkt_cnt - 1; i++) {
			rtw_secmicappend(&micdata, pkt_list->vir_addr, pkt_list->length);
			pkt_list ++;
		}

		/* calculate MIC */
		rtw_secgetmic(&micdata, &mic[0]);

		/* append MIC to the last tail */
		_rtw_memcpy(pxframe->wltail[pxframe->attrib.nr_frags-1], &(mic[0]), 8);
	}

	return _SUCCESS;
}


static void core_wlan_sw_encrypt(_adapter *padapter, struct xmit_frame *pxframe)
{
	struct pkt_attrib *attrib;
	u8 res;


	attrib = &pxframe->attrib;
	if (!attrib->encrypt)
		return;
	if (!attrib->bswenc)
		return;

	/* convert txreq to one piece */
	res = merge_txreq_to_one_piece(padapter, pxframe);
	if (res != _SUCCESS) {
		RTW_ERR("%s: fail alloc buffer for sw enc!\n", __func__);
		return;
	}
	xmitframe_swencrypt(padapter, pxframe);
}

#ifdef CONFIG_TX_AMSDU_SW_MODE
static bool core_tx_amsdu_timeout(_adapter *padapter, struct pkt_attrib *pattrib)
{
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;
	u8 amsdu_timeout;

	amsdu_timeout = rtw_amsdu_get_timer_status(padapter, pattrib->priority);

	if (amsdu_timeout == RTW_AMSDU_TIMER_UNSET) {
		rtw_amsdu_set_timer_status(padapter,
			pattrib->priority, RTW_AMSDU_TIMER_SETTING);
		rtw_amsdu_set_timer(padapter, pattrib->priority);
		pxmitpriv->amsdu_debug_set_timer++;
		return false;
	} else if (amsdu_timeout == RTW_AMSDU_TIMER_SETTING) {
		return false;
	} else if (amsdu_timeout == RTW_AMSDU_TIMER_TIMEOUT) {
		rtw_amsdu_set_timer_status(padapter,
			pattrib->priority, RTW_AMSDU_TIMER_UNSET);
		pxmitpriv->amsdu_debug_timeout++;
		return true;
	}

	return false;
}

/* 'pxframes[]' is array to store xframe to do AMSDU whose size is 'max_xf_nr',
 * and return value is real used size. If return size is 1, then set 'amsdu' to
 * decide normal frame or AMSDU one.
 */
static int core_tx_amsdu_dequeue(_adapter *padapter, struct xmit_frame *pxframes[],
				 int max_xf_nr, bool *amsdu)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;
	struct registry_priv *pregpriv = &padapter->registrypriv;
	struct xmit_frame *pxframe;
	int tx_amsdu = rtw_min(padapter->tx_amsdu, max_xf_nr);
	int tx_amsdu_rate = padapter->tx_amsdu_rate;
	int current_tx_rate = dvobj->traffic_stat.cur_tx_tp;
	int num_frame;
	int nr_xf;

	if (tx_amsdu == 0)
		goto dequeue_normal_pkt;

	if (!MLME_IS_STA(padapter))
		goto dequeue_normal_pkt;

	if (tx_amsdu >= 2 && tx_amsdu_rate && current_tx_rate < tx_amsdu_rate)
		goto dequeue_normal_pkt;

	/*No amsdu when wifi_spec on*/
	if (pregpriv->wifi_spec == 1)
		goto dequeue_normal_pkt;

	pxframe = rtw_get_xframe(pxmitpriv, &num_frame);
	if (num_frame == 0 || !pxframe)
		return 0;

	if (num_frame < tx_amsdu) {	/* Not enough MSDU for specific A-MSDU */
		if (!core_tx_amsdu_timeout(padapter, &pxframe->attrib))
			return 0;	/* Not timeout yet */
	}

	for (nr_xf = 0; nr_xf < tx_amsdu; nr_xf++) {
		pxframe = rtw_get_xframe(pxmitpriv, &num_frame);

		if (num_frame == 0 || !pxframe)
			break;

		if (!check_amsdu(pxframe))
			break;

		/* TODO: check if size is over peer's capability */

		pxframe = rtw_dequeue_xframe(pxmitpriv, pxmitpriv->hwxmits,
					     pxmitpriv->hwxmit_entry);

		pxframes[nr_xf] = pxframe;
	}

	if (nr_xf == 0) {
		if (num_frame > 0)
			goto dequeue_normal_pkt;
		RTW_WARN("%s: nr_xf=0, num_frame=%d\n", __func__, num_frame);
		return 0;
	}

	if (nr_xf < AMSDU_DEBUG_MAX_COUNT)
		pxmitpriv->amsdu_debug_coalesce[nr_xf-1]++;
	else
		pxmitpriv->amsdu_debug_coalesce[AMSDU_DEBUG_MAX_COUNT-1]++;

	*amsdu = (nr_xf == 1 && tx_amsdu >= 2) ? false : true;

	return nr_xf;

dequeue_normal_pkt:
	pxframe = rtw_dequeue_xframe(pxmitpriv, pxmitpriv->hwxmits,
				     pxmitpriv->hwxmit_entry);
	if (!pxframe)
		return 0;

	pxframes[0] = pxframe;
	*amsdu = false;

	return 1;
}

static bool core_tx_amsdu_dump(_adapter *padapter, struct xmit_frame *pxframes[],
			       int xf_nr, bool amsdu)
{
	struct xmit_frame *head_xframe;
	struct pkt_attrib *head_attrib;
	u32 pktlen;

	/* prepare head xmitframe */
	head_xframe = pxframes[0];
	head_attrib = &head_xframe->attrib;

	if (xf_nr == 1 && !amsdu)
		goto dump_pkt;

	rtw_coalesce_tx_amsdu(padapter, pxframes, xf_nr, amsdu, &pktlen);

	/* update proper attribute */
	head_attrib->amsdu = 1;
	head_attrib->pkt_hdrlen = 0;
	head_attrib->pktlen = pktlen;

dump_pkt:
	if (core_tx_prepare_phl(padapter, head_xframe) == FAIL)
		goto abort_core_tx;

	if (core_tx_call_phl(padapter, head_xframe, NULL) == FAIL)
		goto abort_core_tx;

	return true;

abort_core_tx:
	core_tx_free_xmitframe(padapter, head_xframe);

	return true;
}

void core_tx_amsdu_tasklet(_adapter *padapter)
{
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;
	struct xmit_frame *pxframes[5];
	int xf_nr;
	bool amsdu;

	pxmitpriv->amsdu_debug_tasklet++;

	while (1) {
		xf_nr = core_tx_amsdu_dequeue(padapter, pxframes, ARRAY_SIZE(pxframes),
					      &amsdu);
		if (xf_nr == 0)
			break;

		pxmitpriv->amsdu_debug_dequeue++;

		core_tx_amsdu_dump(padapter, pxframes, xf_nr, amsdu);
	}
}

static s32 core_tx_amsdu_enqueue(_adapter *padapter, struct xmit_frame *pxframe)
{
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;
	struct pkt_attrib *pattrib = &pxframe->attrib;
	int tx_amsdu = padapter->tx_amsdu;
	u8 amsdu_timeout;
	s32 res;

	if (MLME_IS_STA(padapter) && check_amsdu_tx_support(padapter)) {
		if (IS_AMSDU_AMPDU_VALID(pattrib))
			goto enqueue;
	}

	return FAIL;

enqueue:
	_rtw_spinlock_bh(&pxmitpriv->lock);

	res = rtw_xmitframe_enqueue(padapter, pxframe);
	if (res == _FAIL) {
		_rtw_spinunlock_bh(&pxmitpriv->lock);
		return FAIL;
	}

	pxmitpriv->amsdu_debug_enqueue++;

	if (tx_amsdu >= 2) {
		amsdu_timeout = rtw_amsdu_get_timer_status(padapter, pattrib->priority);
		if (amsdu_timeout == RTW_AMSDU_TIMER_SETTING) {
			rtw_amsdu_cancel_timer(padapter, pattrib->priority);
			rtw_amsdu_set_timer_status(padapter, pattrib->priority,
				RTW_AMSDU_TIMER_UNSET);
		}
	}

	_rtw_spinunlock_bh(&pxmitpriv->lock);

	rtw_tasklet_hi_schedule(&pxmitpriv->xmit_tasklet);

	return _TRUE;
}
#endif /* CONFIG_TX_AMSDU_SW_MODE */

s32 core_tx_prepare_phl(_adapter *padapter, struct xmit_frame *pxframe)
{
	if (core_wlan_fill_txreq_pre(padapter, pxframe) == _FAIL)
		return FAIL;

	if (pxframe->xftype == RTW_TX_OS) {
		core_wlan_fill_head(padapter, pxframe);
		if (core_wlan_fill_tkip_mic(padapter, pxframe) == _FAIL) {
			RTW_ERR("core_wlan_fill_tkip_mic FAIL\n");
			return FAIL;
		}
	}
	core_wlan_fill_tail(padapter, pxframe);
	core_wlan_sw_encrypt(padapter, pxframe);

	core_wlan_fill_txreq_post(padapter, pxframe);

	return SUCCESS;
}


s32 core_tx_call_phl(_adapter *padapter, struct xmit_frame *pxframe, void *txsc_pkt)
{
	struct rtw_xmit_req *txreq = NULL;
	void *phl = padapter->dvobj->phl;
	u32 idx = 0;
	u8 txreq_cnt = 0;
#ifdef CONFIG_CORE_TXSC
	struct rtw_xmit_req *ptxsc_txreq = NULL;
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;
#endif

#ifdef CONFIG_CORE_TXSC
	struct txsc_pkt_entry *ptxsc_pkt = (struct txsc_pkt_entry *)txsc_pkt;
	if (ptxsc_pkt)
		ptxsc_txreq = ptxsc_pkt->ptxreq;

	txreq = pxframe ? pxframe->phl_txreq : ptxsc_txreq;
	txreq_cnt = pxframe ? pxframe->txreq_cnt : 1;
#else
	txreq = pxframe->phl_txreq;
	txreq_cnt = pxframe->txreq_cnt;
#endif

	for (idx = 0; idx < txreq_cnt; idx++) {

#ifdef RTW_PHL_TEST_FPGA
		core_add_record(padapter, REC_TX_PHL, txreq);
#endif

		if (rtw_phl_add_tx_req(phl, txreq) != RTW_PHL_STATUS_SUCCESS)
			return FAIL;

		rtw_phl_tx_req_notify(phl);


		txreq++;
	}

	/* should count tx status after add tx req is success */
#ifdef CONFIG_CORE_TXSC
	if (ptxsc_txreq != NULL)
		rtw_count_tx_stats_tx_req(padapter, ptxsc_txreq, ptxsc_pkt->psta);
	else
#endif
		rtw_count_tx_stats(padapter, pxframe, pxframe->attrib.pktlen);

	return SUCCESS;
}

s32 core_tx_per_packet(_adapter *padapter, struct xmit_frame *pxframe,
			   struct sk_buff **pskb, struct sta_info *psta)
{
#if defined(CONFIG_AP_MODE) || defined(CONFIG_CORE_TXSC)
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;
#endif

	if (core_tx_update_xmitframe(padapter, pxframe, pskb, psta, RTW_TX_OS) == FAIL)
		goto abort_tx_per_packet;

#ifdef CONFIG_80211N_HT
	if ((pxframe->attrib.ether_type != 0x0806)
	    && (pxframe->attrib.ether_type != 0x888e)
	    && (pxframe->attrib.dhcp_pkt != 1))
		rtw_issue_addbareq_cmd(padapter, pxframe, _TRUE);
#endif /* CONFIG_80211N_HT */

#ifdef CONFIG_TX_AMSDU_SW_MODE
	if (core_tx_amsdu_enqueue(padapter, pxframe) == _TRUE)
		return SUCCESS;	/* queued to do AMSDU */
#endif

	if (core_tx_prepare_phl(padapter, pxframe) == FAIL)
		goto abort_tx_per_packet;

#ifdef CONFIG_AP_MODE
	_rtw_spinlock_bh(&pxmitpriv->lock);
	if (xmitframe_enqueue_for_sleeping_sta(padapter, pxframe) == _TRUE) {
		_rtw_spinunlock_bh(&pxmitpriv->lock);
		DBG_COUNTER(padapter->tx_logs.core_tx_ap_enqueue);
		return SUCCESS;
	}
	_rtw_spinunlock_bh(&pxmitpriv->lock);
#endif

#if !defined(CONFIG_CORE_TXSC) || defined(CONFIG_RTW_DATA_BMC_TO_UC)
	if (core_tx_call_phl(padapter, pxframe, NULL) == SUCCESS)
#endif
		return SUCCESS;


abort_tx_per_packet:
	if (pxframe == NULL) {
		rtw_os_pkt_complete(padapter, *pskb);
	} else {
		if (pxframe->pkt == NULL)
			rtw_os_pkt_complete(padapter, *pskb);
		core_tx_free_xmitframe(padapter, pxframe);
	}

	return FAIL;
}

s32 rtw_core_tx(_adapter *padapter, struct sk_buff **pskb, struct sta_info *psta, u16 os_qid)
{
	struct xmit_frame *pxframe = NULL;
#if defined(CONFIG_AP_MODE) || defined(CONFIG_CORE_TXSC)
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;
#endif
	s32 res = 0;
#ifdef CONFIG_CORE_TXSC
	struct txsc_pkt_entry txsc_pkt;
#endif

#ifdef CONFIG_CORE_TXSC
	if (txsc_get_sc_cached_entry(padapter, *pskb, &txsc_pkt) == _SUCCESS)
		goto core_txsc;
#endif

	if (core_tx_alloc_xmitframe(padapter, &pxframe, os_qid) == FAIL)
		goto abort_core_tx;

	if (core_tx_update_pkt(padapter, pxframe, pskb) == FAIL)
		goto abort_core_tx;

#if defined(CONFIG_AP_MODE)
	if (MLME_STATE(padapter) & WIFI_AP_STATE) {
		_list f_list;

		res = rtw_ap_addr_resolve(padapter, os_qid, pxframe, *pskb, &f_list);
		if (res == _FAIL)
			goto abort_core_tx;

		#if defined(CONFIG_RTW_WDS) || CONFIG_RTW_DATA_BMC_TO_UC
		if (!rtw_is_list_empty(&f_list)) {
			_list *list = get_next(&f_list);
			struct xmit_frame *fframe;

			while ((rtw_end_of_queue_search(&f_list, list)) == _FALSE) {
				fframe = LIST_CONTAINOR(list, struct xmit_frame, list);
				list = get_next(list);
				rtw_list_delete(&fframe->list);

				if (res == RTW_ORI_NO_NEED && rtw_is_list_empty(&f_list)) {
					fframe->pkt = pxframe->pkt; /*last frame */
					pxframe->pkt = NULL;
				} else {
					fframe->pkt = rtw_skb_copy(*pskb);
				}

				if (!fframe->pkt) {
					if (res == RTW_ORI_NO_NEED && IS_MCAST(pxframe->attrib.dst))
						res = _SUCCESS;

					core_tx_free_xmitframe(padapter, fframe);
					continue;
				}

				core_tx_per_packet(padapter, fframe, &fframe->pkt, NULL);
			}
		}
		#endif

		if (res == RTW_ORI_NO_NEED) {
			core_tx_free_xmitframe(padapter, pxframe);
			return SUCCESS;
		}
	}
#endif /* defined(CONFIG_AP_MODE) */
#ifdef CONFIG_LAYER2_ROAMING
        if ((padapter->mlmepriv.roam_network) && ((*pskb)->protocol != htons(0x888e))) {        /* eapol never enqueue.*/
		pxframe->pkt = *pskb;
		rtw_list_delete(&pxframe->list);
		_rtw_spinlock_bh(&pxmitpriv->rpkt_queue.lock);
		rtw_list_insert_tail(&(pxframe->list), get_list_head(&(pxmitpriv->rpkt_queue)));
		_rtw_spinunlock_bh(&pxmitpriv->rpkt_queue.lock);
		return SUCCESS;
	}
#endif

	res = core_tx_per_packet(padapter, pxframe, pskb, psta);
	if (res == FAIL)
		return FAIL;

#ifdef CONFIG_CORE_TXSC
	txsc_add_sc_cache_entry(padapter, pxframe, &txsc_pkt);

core_txsc:

	if (txsc_apply_sc_cached_entry(padapter, &txsc_pkt) == _FAIL)
		goto abort_core_tx;

	if (core_tx_call_phl(padapter, pxframe, &txsc_pkt) == FAIL)
		goto abort_core_tx;
#endif

	return SUCCESS;

abort_core_tx:
	if (pxframe == NULL) {
#ifdef CONFIG_CORE_TXSC
		if (txsc_pkt.ptxreq)
			txsc_free_txreq(padapter, txsc_pkt.ptxreq);
		else
#endif
			rtw_os_pkt_complete(padapter, *pskb);
	} else {
		if (pxframe->pkt == NULL)
			rtw_os_pkt_complete(padapter, *pskb);

		core_tx_free_xmitframe(padapter, pxframe);
	}

	return FAIL;
}

enum rtw_phl_status
rtw_core_tx_recycle(void *drv_priv, struct rtw_xmit_req *txreq)
{
	_adapter *padapter = NULL;
	struct xmit_frame *pxframe = NULL;
#ifdef CONFIG_CORE_TXSC
	struct xmit_txreq_buf *ptxreq_buf = NULL;
#endif

	if (txreq->os_priv == NULL) {
		RTW_ERR("NULL txreq!\n");
		return RTW_PHL_STATUS_FAILURE;
	}

#ifdef CONFIG_CORE_TXSC
	if (txreq->treq_type == RTW_PHL_TREQ_TYPE_CORE_TXSC) {
		ptxreq_buf = (struct xmit_txreq_buf *)txreq->os_priv;
		padapter = ptxreq_buf->adapter;
		#ifdef RTW_PHL_DBG_CMD
		core_add_record(padapter, REC_TX_PHL_RCC, txreq);
		#endif
		txsc_free_txreq(padapter, txreq);
		return RTW_PHL_STATUS_SUCCESS;
	}
#endif /* CONFIG_CORE_TXSC */

	pxframe = (struct xmit_frame *)txreq->os_priv;
	if (pxframe == NULL) {
		RTW_ERR("%s: NULL xmitframe !!\n", __func__);
		rtw_warn_on(1);
		return RTW_PHL_STATUS_FAILURE;
	}

	padapter = pxframe->padapter;

	#ifdef RTW_PHL_DBG_CMD
	core_add_record(padapter, REC_TX_PHL_RCC, txreq);
	#endif

	#ifdef CONFIG_PCI_HCI
	core_recycle_txreq_phyaddr(padapter, txreq);
	#endif
	core_tx_free_xmitframe(padapter, pxframe);

	return RTW_PHL_STATUS_SUCCESS;
}
#endif


#ifdef CONFIG_TDLS
sint xmitframe_enqueue_for_tdls_sleeping_sta(_adapter *padapter, struct xmit_frame *pxmitframe)
{
	sint ret = _FALSE;

	struct sta_info *ptdls_sta = NULL;
	struct sta_priv *pstapriv = &padapter->stapriv;
	struct pkt_attrib *pattrib = &pxmitframe->attrib;
	struct mlme_ext_priv	*pmlmeext = &(padapter->mlmeextpriv);
	int i;

	ptdls_sta = rtw_get_stainfo(pstapriv, pattrib->dst);
	if (ptdls_sta == NULL)
		return ret;
	else if (ptdls_sta->tdls_sta_state & TDLS_LINKED_STATE) {

		if (pattrib->triggered == 1) {
			ret = _TRUE;
			return ret;
		}

		_rtw_spinlock_bh(&ptdls_sta->sleep_q.lock);

		if (ptdls_sta->state & WIFI_SLEEP_STATE) {
			rtw_list_delete(&pxmitframe->list);

			/* _rtw_spinlock_bh(&psta->sleep_q.lock);	 */

			rtw_list_insert_tail(&pxmitframe->list, get_list_head(&ptdls_sta->sleep_q));

			ptdls_sta->sleepq_len++;
			ptdls_sta->sleepq_ac_len++;

			/* indicate 4-AC queue bit in TDLS peer traffic indication */
			switch (pattrib->priority) {
			case 1:
			case 2:
				ptdls_sta->uapsd_bk |= BIT(1);
				break;
			case 4:
			case 5:
				ptdls_sta->uapsd_vi |= BIT(1);
				break;
			case 6:
			case 7:
				ptdls_sta->uapsd_vo |= BIT(1);
				break;
			case 0:
			case 3:
			default:
				ptdls_sta->uapsd_be |= BIT(1);
				break;
			}

			/* Transmit TDLS PTI via AP */
			if (ptdls_sta->sleepq_len == 1)
				rtw_tdls_cmd(padapter, ptdls_sta->phl_sta->mac_addr, TDLS_ISSUE_PTI);

			ret = _TRUE;
		}

		_rtw_spinunlock_bh(&ptdls_sta->sleep_q.lock);
	}

	return ret;

}
#endif /* CONFIG_TDLS */

#define RTW_HIQ_FILTER_ALLOW_ALL 0
#define RTW_HIQ_FILTER_ALLOW_SPECIAL 1
#define RTW_HIQ_FILTER_DENY_ALL 2

inline bool xmitframe_hiq_filter(struct xmit_frame *xmitframe)
{
	bool allow = _FALSE;
	_adapter *adapter = xmitframe->padapter;
	struct registry_priv *registry = &adapter->registrypriv;

	if (adapter->registrypriv.wifi_spec == 1)
		allow = _TRUE;
	else if (registry->hiq_filter == RTW_HIQ_FILTER_ALLOW_SPECIAL) {

		struct pkt_attrib *attrib = &xmitframe->attrib;

		if (attrib->ether_type == 0x0806
		    || attrib->ether_type == 0x888e
#ifdef CONFIG_WAPI_SUPPORT
		    || attrib->ether_type == 0x88B4
#endif
		    || attrib->dhcp_pkt
		   ) {
			if (0)
				RTW_INFO(FUNC_ADPT_FMT" ether_type:0x%04x%s\n", FUNC_ADPT_ARG(xmitframe->padapter)
					, attrib->ether_type, attrib->dhcp_pkt ? " DHCP" : "");
			allow = _TRUE;
		}
	} else if (registry->hiq_filter == RTW_HIQ_FILTER_ALLOW_ALL)
		allow = _TRUE;
	else if (registry->hiq_filter == RTW_HIQ_FILTER_DENY_ALL)
		allow = _FALSE;
	else
		rtw_warn_on(1);

	return allow;
}

#if defined(CONFIG_AP_MODE) || defined(CONFIG_TDLS)

sint xmitframe_enqueue_for_sleeping_sta(_adapter *padapter, struct xmit_frame *pxmitframe)
{
	sint ret = _FALSE;
	struct sta_info *psta = NULL;
	struct sta_priv *pstapriv = &padapter->stapriv;
	struct pkt_attrib *pattrib = &pxmitframe->attrib;
	sint bmcst = IS_MCAST(pattrib->ra);
	bool update_tim = _FALSE;
	struct _ADAPTER_LINK *padapter_link = pattrib->adapter_link;
#ifdef CONFIG_TDLS

	if (padapter->tdlsinfo.link_established == _TRUE)
		ret = xmitframe_enqueue_for_tdls_sleeping_sta(padapter, pxmitframe);
#endif /* CONFIG_TDLS */

	if (!MLME_IS_AP(padapter) && !MLME_IS_MESH(padapter)) {
		DBG_COUNTER(padapter->tx_logs.core_tx_ap_enqueue_warn_fwstate);
		return ret;
	}
	/*
		if(pattrib->psta)
		{
			psta = pattrib->psta;
		}
		else
		{
			RTW_INFO("%s, call rtw_get_stainfo()\n", __func__);
			psta=rtw_get_stainfo(pstapriv, pattrib->ra);
		}
	*/
	psta = rtw_get_stainfo(&padapter->stapriv, pattrib->ra);
	if (pattrib->psta != psta) {
		DBG_COUNTER(padapter->tx_logs.core_tx_ap_enqueue_warn_sta);
		RTW_INFO("%s, pattrib->psta(%p) != psta(%p)\n", __func__, pattrib->psta, psta);
		return _FALSE;
	}

	if (psta == NULL) {
		DBG_COUNTER(padapter->tx_logs.core_tx_ap_enqueue_warn_nosta);
		RTW_INFO("%s, psta==NUL\n", __func__);
		return _FALSE;
	}

	if (!(psta->state & WIFI_ASOC_STATE)) {
		DBG_COUNTER(padapter->tx_logs.core_tx_ap_enqueue_warn_link);
		RTW_INFO("%s, psta->state(0x%x) != WIFI_ASOC_STATE\n", __func__, psta->state);
		return _FALSE;
	}

	if (pattrib->triggered == 1) {
		DBG_COUNTER(padapter->tx_logs.core_tx_ap_enqueue_warn_trigger);
		/* RTW_INFO("directly xmit pspoll_triggered packet\n"); */

		/* pattrib->triggered=0; */
		#if 0
		if (bmcst && xmitframe_hiq_filter(pxmitframe) == _TRUE)
			pattrib->qsel = rtw_hal_get_qsel(padapter, QSLT_HIGH_ID);/* HIQ */
		#endif
		return ret;
	}


	if (bmcst) {
#ifndef BMC_ON_HIQ
		_rtw_spinlock_bh(&psta->sleep_q.lock);

		if (rtw_tim_map_anyone_be_set(padapter, pstapriv->sta_dz_bitmap)) { /* if anyone sta is in ps mode */
			/* pattrib->qsel = rtw_hal_get_qsel(padapter,QSLT_HIGH_ID);*/ /* HIQ */

			rtw_list_delete(&pxmitframe->list);

			/*_rtw_spinlock_bh(&psta->sleep_q.lock);*/

			rtw_list_insert_tail(&pxmitframe->list, get_list_head(&psta->sleep_q));

			psta->sleepq_len++;

			if (!(rtw_tim_map_is_set(padapter, pstapriv->tim_bitmap, 0)))
				update_tim = _TRUE;

			rtw_tim_map_set(padapter, pstapriv->tim_bitmap, 0);
			rtw_tim_map_set(padapter, pstapriv->sta_dz_bitmap, 0);

			/* RTW_INFO("enqueue, sq_len=%d\n", psta->sleepq_len); */
			/* RTW_INFO_DUMP("enqueue, tim=", pstapriv->tim_bitmap, pstapriv->aid_bmp_len); */
			if (update_tim == _TRUE) {
				if (is_broadcast_mac_addr(pattrib->ra))
					_update_beacon(padapter, padapter_link, _TIM_IE_, NULL, _TRUE, 0, "buffer BC");
				else
					_update_beacon(padapter, padapter_link, _TIM_IE_, NULL, _TRUE, 0, "buffer MC");
			} else
				chk_bmc_sleepq_cmd(padapter);

			/*_rtw_spinunlock_bh(&psta->sleep_q.lock);*/

			ret = _TRUE;

			DBG_COUNTER(padapter->tx_logs.core_tx_ap_enqueue_mcast);
		}

		_rtw_spinunlock_bh(&psta->sleep_q.lock);
#endif
		return ret;

	}


	_rtw_spinlock_bh(&psta->sleep_q.lock);

	if (psta->state & WIFI_SLEEP_STATE) {
		u8 wmmps_ac = 0;

		if (rtw_tim_map_is_set(padapter, pstapriv->sta_dz_bitmap, psta->phl_sta->aid)) {
			rtw_list_delete(&pxmitframe->list);

			/* _rtw_spinlock_bh(&psta->sleep_q.lock);	 */

			rtw_list_insert_tail(&pxmitframe->list, get_list_head(&psta->sleep_q));

			psta->sleepq_len++;

			switch (pattrib->priority) {
			case 1:
			case 2:
				wmmps_ac = psta->uapsd_bk & BIT(0);
				break;
			case 4:
			case 5:
				wmmps_ac = psta->uapsd_vi & BIT(0);
				break;
			case 6:
			case 7:
				wmmps_ac = psta->uapsd_vo & BIT(0);
				break;
			case 0:
			case 3:
			default:
				wmmps_ac = psta->uapsd_be & BIT(0);
				break;
			}

			if (wmmps_ac)
				psta->sleepq_ac_len++;

			if (((psta->has_legacy_ac) && (!wmmps_ac)) || ((!psta->has_legacy_ac) && (wmmps_ac))) {
				if (!(rtw_tim_map_is_set(padapter, pstapriv->tim_bitmap, psta->phl_sta->aid)))
					update_tim = _TRUE;

				rtw_tim_map_set(padapter, pstapriv->tim_bitmap, psta->phl_sta->aid);

				/* RTW_INFO("enqueue, sq_len=%d\n", psta->sleepq_len); */
				/* RTW_INFO_DUMP("enqueue, tim=", pstapriv->tim_bitmap, pstapriv->aid_bmp_len); */

				if (update_tim == _TRUE) {
					/* RTW_INFO("sleepq_len==1, update BCNTIM\n"); */
					/* upate BCN for TIM IE */
					_update_beacon(padapter, padapter_link, _TIM_IE_, NULL, _TRUE, 0, "buffer UC");
				}
			}

			/* _rtw_spinunlock_bh(&psta->sleep_q.lock);			 */

			/* if(psta->sleepq_len > (NR_XMITFRAME>>3)) */
			/* { */
			/*	wakeup_sta_to_xmit(padapter, psta); */
			/* }	 */

			ret = _TRUE;

			DBG_COUNTER(padapter->tx_logs.core_tx_ap_enqueue_ucast);
		}

	}

	_rtw_spinunlock_bh(&psta->sleep_q.lock);

	return ret;

}

static void dequeue_xmitframes_to_sleeping_queue(_adapter *padapter, struct sta_info *psta, _queue *pframequeue)
{
	sint ret;
	_list	*plist, *phead;
	u8	ac_index;
	struct tx_servq	*ptxservq;
	struct pkt_attrib	*pattrib;
	struct xmit_frame	*pxmitframe;
	struct hw_xmit *phwxmits =  padapter->xmitpriv.hwxmits;

	phead = get_list_head(pframequeue);
	plist = get_next(phead);

	while (rtw_end_of_queue_search(phead, plist) == _FALSE) {
		pxmitframe = LIST_CONTAINOR(plist, struct xmit_frame, list);

		plist = get_next(plist);

		pattrib = &pxmitframe->attrib;

		pattrib->triggered = 0;

		ret = xmitframe_enqueue_for_sleeping_sta(padapter, pxmitframe);

		if (_TRUE == ret) {
			ptxservq = rtw_get_sta_pending(padapter, psta, pattrib->priority, (u8 *)(&ac_index));

			ptxservq->qcnt--;
			phwxmits[ac_index].accnt--;
		} else {
			/* RTW_INFO("xmitframe_enqueue_for_sleeping_sta return _FALSE\n"); */
		}

	}

}

void stop_sta_xmit(_adapter *padapter, struct sta_info *psta)
{
	struct sta_info *psta_bmc;
	struct sta_xmit_priv *pstaxmitpriv;
	struct sta_priv *pstapriv = &padapter->stapriv;
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;

	pstaxmitpriv = &psta->sta_xmitpriv;

	/* for BC/MC Frames */
	psta_bmc = rtw_get_bcmc_stainfo(padapter, psta->padapter_link);
	if (!psta_bmc)
		rtw_warn_on(1);

	_rtw_spinlock_bh(&pxmitpriv->lock);

	psta->state |= WIFI_SLEEP_STATE;

#ifdef CONFIG_TDLS
	if (!(psta->tdls_sta_state & TDLS_LINKED_STATE))
#endif /* CONFIG_TDLS */
		rtw_tim_map_set(padapter, pstapriv->sta_dz_bitmap, psta->phl_sta->aid);

	dequeue_xmitframes_to_sleeping_queue(padapter, psta, &pstaxmitpriv->vo_q.sta_pending);
	rtw_list_delete(&(pstaxmitpriv->vo_q.tx_pending));
	dequeue_xmitframes_to_sleeping_queue(padapter, psta, &pstaxmitpriv->vi_q.sta_pending);
	rtw_list_delete(&(pstaxmitpriv->vi_q.tx_pending));
	dequeue_xmitframes_to_sleeping_queue(padapter, psta, &pstaxmitpriv->be_q.sta_pending);
	rtw_list_delete(&(pstaxmitpriv->be_q.tx_pending));
	dequeue_xmitframes_to_sleeping_queue(padapter, psta, &pstaxmitpriv->bk_q.sta_pending);
	rtw_list_delete(&(pstaxmitpriv->bk_q.tx_pending));

	if (psta_bmc != NULL
		#ifdef CONFIG_TDLS
		&& !(psta->tdls_sta_state & TDLS_LINKED_STATE)
		#endif
	) {
		/* for BC/MC Frames */
#ifndef BMC_ON_HIQ
		pstaxmitpriv = &psta_bmc->sta_xmitpriv;
		dequeue_xmitframes_to_sleeping_queue(padapter, psta_bmc, &pstaxmitpriv->vo_q.sta_pending);
		rtw_list_delete(&(pstaxmitpriv->vo_q.tx_pending));
		dequeue_xmitframes_to_sleeping_queue(padapter, psta_bmc, &pstaxmitpriv->vi_q.sta_pending);
		rtw_list_delete(&(pstaxmitpriv->vi_q.tx_pending));
		dequeue_xmitframes_to_sleeping_queue(padapter, psta_bmc, &pstaxmitpriv->be_q.sta_pending);
		rtw_list_delete(&(pstaxmitpriv->be_q.tx_pending));
		dequeue_xmitframes_to_sleeping_queue(padapter, psta_bmc, &pstaxmitpriv->bk_q.sta_pending);
		rtw_list_delete(&(pstaxmitpriv->bk_q.tx_pending));
#endif
	}
	_rtw_spinunlock_bh(&pxmitpriv->lock);


}

void wakeup_sta_to_xmit(_adapter *padapter, struct sta_info *psta)
{
	u8 update_mask = 0, wmmps_ac = 0;
	struct sta_info *psta_bmc;
	_list	*xmitframe_plist, *xmitframe_phead;
	struct xmit_frame *pxmitframe = NULL;
	struct sta_priv *pstapriv = &padapter->stapriv;
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;

	psta_bmc = rtw_get_bcmc_stainfo(padapter, psta->padapter_link);


	/* _rtw_spinlock_bh(&psta->sleep_q.lock); */
	_rtw_spinlock_bh(&pxmitpriv->lock);

	xmitframe_phead = get_list_head(&psta->sleep_q);
	xmitframe_plist = get_next(xmitframe_phead);

	while ((rtw_end_of_queue_search(xmitframe_phead, xmitframe_plist)) == _FALSE) {
		pxmitframe = LIST_CONTAINOR(xmitframe_plist, struct xmit_frame, list);

		xmitframe_plist = get_next(xmitframe_plist);

		rtw_list_delete(&pxmitframe->list);

		switch (pxmitframe->attrib.priority) {
		case 1:
		case 2:
			wmmps_ac = psta->uapsd_bk & BIT(1);
			break;
		case 4:
		case 5:
			wmmps_ac = psta->uapsd_vi & BIT(1);
			break;
		case 6:
		case 7:
			wmmps_ac = psta->uapsd_vo & BIT(1);
			break;
		case 0:
		case 3:
		default:
			wmmps_ac = psta->uapsd_be & BIT(1);
			break;
		}

		psta->sleepq_len--;
		if (psta->sleepq_len > 0)
			pxmitframe->attrib.mdata = 1;
		else
			pxmitframe->attrib.mdata = 0;

		if (wmmps_ac) {
			psta->sleepq_ac_len--;
			if (psta->sleepq_ac_len > 0) {
				pxmitframe->attrib.mdata = 1;
				pxmitframe->attrib.eosp = 0;
			} else {
				pxmitframe->attrib.mdata = 0;
				pxmitframe->attrib.eosp = 1;
			}
		}

		pxmitframe->attrib.triggered = 1;

		/*
				_rtw_spinunlock_bh(&psta->sleep_q.lock);
				//rtw_intf_data_xmit
				if(rtw_hal_xmit(padapter, pxmitframe) == _TRUE)
				{
					rtw_os_xmit_complete(padapter, pxmitframe);
				}
				_rtw_spinlock_bh(&psta->sleep_q.lock);
		*/
		rtw_intf_xmitframe_enqueue(padapter, pxmitframe);


	}

	if (psta->sleepq_len == 0) {
#ifdef CONFIG_TDLS
		if (psta->tdls_sta_state & TDLS_LINKED_STATE) {
			if (psta->state & WIFI_SLEEP_STATE)
				psta->state ^= WIFI_SLEEP_STATE;

			_rtw_spinunlock_bh(&pxmitpriv->lock);
			return;
		}
#endif /* CONFIG_TDLS */

		if (rtw_tim_map_is_set(padapter, pstapriv->tim_bitmap, psta->phl_sta->aid)) {
			/* RTW_INFO("wakeup to xmit, qlen==0\n"); */
			/* RTW_INFO_DUMP("update_BCNTIM, tim=", pstapriv->tim_bitmap, pstapriv->aid_bmp_len); */
			/* upate BCN for TIM IE */
			/* update_BCNTIM(padapter); */
			update_mask = BIT(0);
		}

		rtw_tim_map_clear(padapter, pstapriv->tim_bitmap, psta->phl_sta->aid);

		if (psta->state & WIFI_SLEEP_STATE)
			psta->state ^= WIFI_SLEEP_STATE;

		if (psta->state & WIFI_STA_ALIVE_CHK_STATE) {
			RTW_INFO("%s alive check\n", __func__);
			psta->expire_to = pstapriv->expire_to;
			psta->state ^= WIFI_STA_ALIVE_CHK_STATE;
		}

		rtw_tim_map_clear(padapter, pstapriv->sta_dz_bitmap, psta->phl_sta->aid);
	}

	/* for BC/MC Frames */
	if (!psta_bmc)
		goto _exit;

	if (!(rtw_tim_map_anyone_be_set_exclude_aid0(padapter, pstapriv->sta_dz_bitmap))) { /* no any sta in ps mode */
		xmitframe_phead = get_list_head(&psta_bmc->sleep_q);
		xmitframe_plist = get_next(xmitframe_phead);

		while ((rtw_end_of_queue_search(xmitframe_phead, xmitframe_plist)) == _FALSE) {
			pxmitframe = LIST_CONTAINOR(xmitframe_plist, struct xmit_frame, list);

			xmitframe_plist = get_next(xmitframe_plist);

			rtw_list_delete(&pxmitframe->list);

			psta_bmc->sleepq_len--;
			if (psta_bmc->sleepq_len > 0)
				pxmitframe->attrib.mdata = 1;
			else
				pxmitframe->attrib.mdata = 0;


			pxmitframe->attrib.triggered = 1;
			/*
						_rtw_spinunlock_bh(&psta_bmc->sleep_q.lock);
						//rtw_intf_data_xmit
						if(rtw_hal_xmit(padapter, pxmitframe) == _TRUE)
						{
							rtw_os_xmit_complete(padapter, pxmitframe);
						}
						_rtw_spinlock_bh(&psta_bmc->sleep_q.lock);

			*/
			rtw_intf_xmitframe_enqueue(padapter, pxmitframe);

		}

		if (psta_bmc->sleepq_len == 0) {
			if (rtw_tim_map_is_set(padapter, pstapriv->tim_bitmap, 0)) {
				/* RTW_INFO("wakeup to xmit, qlen==0\n"); */
				/* RTW_INFO_DUMP("update_BCNTIM, tim=", pstapriv->tim_bitmap, pstapriv->aid_bmp_len); */
				/* upate BCN for TIM IE */
				/* update_BCNTIM(padapter); */
				update_mask |= BIT(1);
			}
			rtw_tim_map_clear(padapter, pstapriv->tim_bitmap, 0);
			rtw_tim_map_clear(padapter, pstapriv->sta_dz_bitmap, 0);
		}

	}

_exit:

	/* _rtw_spinunlock_bh(&psta_bmc->sleep_q.lock);	 */
	_rtw_spinunlock_bh(&pxmitpriv->lock);

	if (update_mask) {
		/* update_BCNTIM(padapter); */
		if ((update_mask & (BIT(0) | BIT(1))) == (BIT(0) | BIT(1)))
			_update_beacon(padapter, psta->padapter_link, _TIM_IE_, NULL, _TRUE, 0, "clear UC&BMC");
		else if ((update_mask & BIT(1)) == BIT(1))
			_update_beacon(padapter, psta->padapter_link, _TIM_IE_, NULL, _TRUE, 0, "clear BMC");
		else
			_update_beacon(padapter, psta->padapter_link, _TIM_IE_, NULL, _TRUE, 0, "clear UC");
	}

}

void xmit_delivery_enabled_frames(_adapter *padapter, struct sta_info *psta)
{
	u8 wmmps_ac = 0;
	_list	*xmitframe_plist, *xmitframe_phead;
	struct xmit_frame *pxmitframe = NULL;
	struct sta_priv *pstapriv = &padapter->stapriv;
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;


	/* _rtw_spinlock_bh(&psta->sleep_q.lock); */
	_rtw_spinlock_bh(&pxmitpriv->lock);

	xmitframe_phead = get_list_head(&psta->sleep_q);
	xmitframe_plist = get_next(xmitframe_phead);

	while ((rtw_end_of_queue_search(xmitframe_phead, xmitframe_plist)) == _FALSE) {
		pxmitframe = LIST_CONTAINOR(xmitframe_plist, struct xmit_frame, list);

		xmitframe_plist = get_next(xmitframe_plist);

		switch (pxmitframe->attrib.priority) {
		case 1:
		case 2:
			wmmps_ac = psta->uapsd_bk & BIT(1);
			break;
		case 4:
		case 5:
			wmmps_ac = psta->uapsd_vi & BIT(1);
			break;
		case 6:
		case 7:
			wmmps_ac = psta->uapsd_vo & BIT(1);
			break;
		case 0:
		case 3:
		default:
			wmmps_ac = psta->uapsd_be & BIT(1);
			break;
		}

		if (!wmmps_ac)
			continue;

		rtw_list_delete(&pxmitframe->list);

		psta->sleepq_len--;
		psta->sleepq_ac_len--;

		if (psta->sleepq_ac_len > 0) {
			pxmitframe->attrib.mdata = 1;
			pxmitframe->attrib.eosp = 0;
		} else {
			pxmitframe->attrib.mdata = 0;
			pxmitframe->attrib.eosp = 1;
		}

		pxmitframe->attrib.triggered = 1;
		rtw_intf_xmitframe_enqueue(padapter, pxmitframe);

		if ((psta->sleepq_ac_len == 0) && (!psta->has_legacy_ac) && (wmmps_ac)) {
#ifdef CONFIG_TDLS
			if (psta->tdls_sta_state & TDLS_LINKED_STATE) {
				/* _rtw_spinunlock_bh(&psta->sleep_q.lock); */
				goto exit;
			}
#endif /* CONFIG_TDLS */
			rtw_tim_map_clear(padapter, pstapriv->tim_bitmap, psta->phl_sta->aid);

			/* RTW_INFO("wakeup to xmit, qlen==0\n"); */
			/* RTW_INFO_DUMP("update_BCNTIM, tim=", pstapriv->tim_bitmap, pstapriv->aid_bmp_len); */
			/* upate BCN for TIM IE */
			/* update_BCNTIM(padapter); */
			rtw_update_beacon(padapter, psta->padapter_link, _TIM_IE_, NULL, _TRUE, 0);
			/* update_mask = BIT(0); */
		}

	}

#ifdef CONFIG_TDLS
exit:
#endif
	/* _rtw_spinunlock_bh(&psta->sleep_q.lock);	 */
	_rtw_spinunlock_bh(&pxmitpriv->lock);

	return;
}

#endif /* defined(CONFIG_AP_MODE) || defined(CONFIG_TDLS) */

#if 0 /*#ifdef CONFIG_XMIT_THREAD_MODE*/
void enqueue_pending_xmitbuf(
	struct xmit_priv *pxmitpriv,
	struct xmit_buf *pxmitbuf)
{
	_queue *pqueue;
	_adapter *pri_adapter = pxmitpriv->adapter;

	pqueue = &pxmitpriv->pending_xmitbuf_queue;

	_rtw_spinlock_bh(&pqueue->lock);
	rtw_list_delete(&pxmitbuf->list);
	rtw_list_insert_tail(&pxmitbuf->list, get_list_head(pqueue));
	_rtw_spinunlock_bh(&pqueue->lock);

#if defined(CONFIG_SDIO_HCI) && defined(CONFIG_CONCURRENT_MODE)
	pri_adapter = GET_PRIMARY_ADAPTER(pri_adapter);
#endif /*SDIO_HCI + CONCURRENT*/
	_rtw_up_sema(&(pri_adapter->xmitpriv.xmit_sema));
}

void enqueue_pending_xmitbuf_to_head(
	struct xmit_priv *pxmitpriv,
	struct xmit_buf *pxmitbuf)
{
	_queue *pqueue = &pxmitpriv->pending_xmitbuf_queue;

	_rtw_spinlock_bh(&pqueue->lock);
	rtw_list_delete(&pxmitbuf->list);
	rtw_list_insert_head(&pxmitbuf->list, get_list_head(pqueue));
	_rtw_spinunlock_bh(&pqueue->lock);
}

struct xmit_buf *dequeue_pending_xmitbuf(
	struct xmit_priv *pxmitpriv)
{
	struct xmit_buf *pxmitbuf;
	_queue *pqueue;


	pxmitbuf = NULL;
	pqueue = &pxmitpriv->pending_xmitbuf_queue;

	_rtw_spinlock_bh(&pqueue->lock);

	if (_rtw_queue_empty(pqueue) == _FALSE) {
		_list *plist, *phead;

		phead = get_list_head(pqueue);
		plist = get_next(phead);
		pxmitbuf = LIST_CONTAINOR(plist, struct xmit_buf, list);
		rtw_list_delete(&pxmitbuf->list);
	}

	_rtw_spinunlock_bh(&pqueue->lock);

	return pxmitbuf;
}

static struct xmit_buf *dequeue_pending_xmitbuf_ext(
	struct xmit_priv *pxmitpriv)
{
	struct xmit_buf *pxmitbuf;
	_queue *pqueue;

	pxmitbuf = NULL;
	pqueue = &pxmitpriv->pending_xmitbuf_queue;

	_rtw_spinlock_bh(&pqueue->lock);

	if (_rtw_queue_empty(pqueue) == _FALSE) {
		_list *plist, *phead;

		phead = get_list_head(pqueue);
		plist = phead;
		do {
			plist = get_next(plist);
			if (plist == phead)
				break;

			pxmitbuf = LIST_CONTAINOR(plist, struct xmit_buf, list);

			if (pxmitbuf->buf_tag == XMITBUF_MGNT) {
				rtw_list_delete(&pxmitbuf->list);
				break;
			}
			pxmitbuf = NULL;
		} while (1);
	}

	_rtw_spinunlock_bh(&pqueue->lock);

	return pxmitbuf;
}

struct xmit_buf *select_and_dequeue_pending_xmitbuf(_adapter *padapter)
{
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;
	struct xmit_buf *pxmitbuf = NULL;

	if (_TRUE == rtw_is_xmit_blocked(padapter))
		return pxmitbuf;

	pxmitbuf = dequeue_pending_xmitbuf_ext(pxmitpriv);
	if (pxmitbuf == NULL && rtw_xmit_ac_blocked(padapter) != _TRUE)
		pxmitbuf = dequeue_pending_xmitbuf(pxmitpriv);

	return pxmitbuf;
}

sint check_pending_xmitbuf(
	struct xmit_priv *pxmitpriv)
{
	_queue *pqueue;
	sint	ret = _FALSE;

	pqueue = &pxmitpriv->pending_xmitbuf_queue;

	_rtw_spinlock_bh(&pqueue->lock);

	if (_rtw_queue_empty(pqueue) == _FALSE)
		ret = _TRUE;

	_rtw_spinunlock_bh(&pqueue->lock);

	return ret;
}

thread_return rtw_xmit_thread(thread_context context)
{
	s32 err;
	_adapter *adapter;
#ifdef RTW_XMIT_THREAD_HIGH_PRIORITY
#ifdef PLATFORM_LINUX
	struct sched_param param = { .sched_priority = 1 };

	sched_setscheduler(current, SCHED_FIFO, &param);
#endif /* PLATFORM_LINUX */
#endif /* RTW_XMIT_THREAD_HIGH_PRIORITY */

	err = _SUCCESS;
	adapter = (_adapter *)context;

	rtw_thread_enter("RTW_XMIT_THREAD");

	do {
		err = rtw_intf_xmit_buf_handler(adapter);
		flush_signals_thread();
	} while (_SUCCESS == err);

	RTW_INFO(FUNC_ADPT_FMT " Exit\n", FUNC_ADPT_ARG(adapter));

	rtw_thread_wait_stop();

	return 0;
}
#endif

#ifdef DBG_XMIT_BLOCK
void dump_xmit_block(void *sel, _adapter *padapter)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);

	RTW_PRINT_SEL(sel, "[XMIT-BLOCK] xmit_block :0x%02x\n", dvobj->xmit_block);
	if (dvobj->xmit_block & XMIT_BLOCK_REDLMEM)
		RTW_PRINT_SEL(sel, "Reason:%s\n", "XMIT_BLOCK_REDLMEM");
	if (dvobj->xmit_block & XMIT_BLOCK_SUSPEND)
		RTW_PRINT_SEL(sel, "Reason:%s\n", "XMIT_BLOCK_SUSPEND");
	if (dvobj->xmit_block == XMIT_BLOCK_NONE)
		RTW_PRINT_SEL(sel, "Reason:%s\n", "XMIT_BLOCK_NONE");
}
void dump_xmit_block_info(void *sel, const char *fun_name, _adapter *padapter)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);

	RTW_INFO("\n"ADPT_FMT" call %s\n", ADPT_ARG(padapter), fun_name);
	dump_xmit_block(sel, padapter);
}
#define DBG_XMIT_BLOCK_DUMP(adapter)	dump_xmit_block_info(RTW_DBGDUMP, __func__, adapter)
#endif

void rtw_set_xmit_block(_adapter *padapter, enum XMIT_BLOCK_REASON reason)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);

	_rtw_spinlock_bh(&dvobj->xmit_block_lock);
	dvobj->xmit_block |= reason;
	_rtw_spinunlock_bh(&dvobj->xmit_block_lock);

	#ifdef DBG_XMIT_BLOCK
	DBG_XMIT_BLOCK_DUMP(padapter);
	#endif
}

void rtw_clr_xmit_block(_adapter *padapter, enum XMIT_BLOCK_REASON reason)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);

	_rtw_spinlock_bh(&dvobj->xmit_block_lock);
	dvobj->xmit_block &= ~reason;
	_rtw_spinunlock_bh(&dvobj->xmit_block_lock);

	#ifdef DBG_XMIT_BLOCK
	DBG_XMIT_BLOCK_DUMP(padapter);
	#endif
}
bool rtw_is_xmit_blocked(_adapter *padapter)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);

	#ifdef DBG_XMIT_BLOCK
	DBG_XMIT_BLOCK_DUMP(padapter);
	#endif
	return ((dvobj->xmit_block) ? _TRUE : _FALSE);
}

bool rtw_xmit_ac_blocked(_adapter *adapter)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	struct rf_ctl_t *rfctl = adapter_to_rfctl(adapter);
	_adapter *iface;
	struct mlme_ext_priv *mlmeext;
	bool blocked = _FALSE;
	int i;
#ifdef DBG_CONFIG_ERROR_DETECT
#ifdef DBG_CONFIG_ERROR_RESET
#ifdef CONFIG_USB_HCI
	if (rtw_hal_sreset_inprogress(adapter) == _TRUE) {
		blocked = _TRUE;
		goto exit;
	}
#endif/* #ifdef CONFIG_USB_HCI */
#endif/* #ifdef DBG_CONFIG_ERROR_RESET */
#endif/* #ifdef DBG_CONFIG_ERROR_DETECT */

	if (rfctl->offch_state != OFFCHS_NONE
		#if CONFIG_DFS
		|| IS_RADAR_DETECTED(rfctl)
		#ifdef CONFIG_ECSA_PHL
		|| rtw_mr_is_ecsa_running(adapter)
		#endif
		#endif
	) {
		blocked = _TRUE;
		goto exit;
	}

	for (i = 0; i < dvobj->iface_nums; i++) {
		iface = dvobj->padapters[i];

		if (!rtw_iface_at_same_hwband(adapter, iface))
			continue;

		mlmeext = &iface->mlmeextpriv;
		/* check scan state */
		if (mlmeext_scan_state(mlmeext) != SCAN_DISABLE
			&& mlmeext_scan_state(mlmeext) != SCAN_BACK_OP
		) {
			blocked = _TRUE;
			goto exit;
		}

		if (mlmeext_scan_state(mlmeext) == SCAN_BACK_OP
			&& !mlmeext_chk_scan_backop_flags(mlmeext, SS_BACKOP_TX_RESUME)
		) {
			blocked = _TRUE;
			goto exit;
		}
	}

exit:
	return blocked;
}

#ifdef CONFIG_LAYER2_ROAMING
/*	dequeuq + xmit the cache skb during the roam procedure	*/
void dequeuq_roam_pkt(_adapter *padapter)
{
	struct xmit_frame *rframe;
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;
	_list *plist = NULL, *phead = NULL;

	if (padapter->mlmepriv.roam_network) {
		padapter->mlmepriv.roam_network = NULL;
		_rtw_spinlock_bh(&pxmitpriv->rpkt_queue.lock);
		phead = get_list_head(&pxmitpriv->rpkt_queue);
		plist = get_next(phead);
		while ((rtw_end_of_queue_search(phead, plist)) == _FALSE) {
			rframe = LIST_CONTAINOR(plist, struct xmit_frame, list);
			plist = get_next(plist);
			rtw_list_delete(&rframe->list);
			core_tx_per_packet(padapter, rframe, &rframe->pkt, NULL);
		}
		_rtw_spinunlock_bh(&pxmitpriv->rpkt_queue.lock);
	}
}
#endif

#ifdef CONFIG_TX_AMSDU
void rtw_amsdu_vo_timeout_handler(void *FunctionContext)
{
	_adapter *adapter = (_adapter *)FunctionContext;

	adapter->xmitpriv.amsdu_vo_timeout = RTW_AMSDU_TIMER_TIMEOUT;

	rtw_tasklet_hi_schedule(&adapter->xmitpriv.xmit_tasklet);
}

void rtw_amsdu_vi_timeout_handler(void *FunctionContext)
{
	_adapter *adapter = (_adapter *)FunctionContext;

	adapter->xmitpriv.amsdu_vi_timeout = RTW_AMSDU_TIMER_TIMEOUT;

	rtw_tasklet_hi_schedule(&adapter->xmitpriv.xmit_tasklet);
}

void rtw_amsdu_be_timeout_handler(void *FunctionContext)
{
	_adapter *adapter = (_adapter *)FunctionContext;

	adapter->xmitpriv.amsdu_be_timeout = RTW_AMSDU_TIMER_TIMEOUT;

	rtw_tasklet_hi_schedule(&adapter->xmitpriv.xmit_tasklet);
}

void rtw_amsdu_bk_timeout_handler(void *FunctionContext)
{
	_adapter *adapter = (_adapter *)FunctionContext;

	adapter->xmitpriv.amsdu_bk_timeout = RTW_AMSDU_TIMER_TIMEOUT;

	rtw_tasklet_hi_schedule(&adapter->xmitpriv.xmit_tasklet);
}

u8 rtw_amsdu_get_timer_status(_adapter *padapter, u8 priority)
{
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;
	u8 status =  RTW_AMSDU_TIMER_UNSET;

	switch (priority) {
	case 1:
	case 2:
		status = pxmitpriv->amsdu_bk_timeout;
		break;
	case 4:
	case 5:
		status = pxmitpriv->amsdu_vi_timeout;
		break;
	case 6:
	case 7:
		status = pxmitpriv->amsdu_vo_timeout;
		break;
	case 0:
	case 3:
	default:
		status = pxmitpriv->amsdu_be_timeout;
		break;
	}
	return status;
}

void rtw_amsdu_set_timer_status(_adapter *padapter, u8 priority, u8 status)
{
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;

	switch (priority) {
	case 1:
	case 2:
		pxmitpriv->amsdu_bk_timeout = status;
		break;
	case 4:
	case 5:
		pxmitpriv->amsdu_vi_timeout = status;
		break;
	case 6:
	case 7:
		pxmitpriv->amsdu_vo_timeout = status;
		break;
	case 0:
	case 3:
	default:
		pxmitpriv->amsdu_be_timeout = status;
		break;
	}
}

void rtw_amsdu_set_timer(_adapter *padapter, u8 priority)
{
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;
	_timer *amsdu_timer = NULL;

	switch (priority) {
	case 1:
	case 2:
		amsdu_timer = &pxmitpriv->amsdu_bk_timer;
		break;
	case 4:
	case 5:
		amsdu_timer = &pxmitpriv->amsdu_vi_timer;
		break;
	case 6:
	case 7:
		amsdu_timer = &pxmitpriv->amsdu_vo_timer;
		break;
	case 0:
	case 3:
	default:
		amsdu_timer = &pxmitpriv->amsdu_be_timer;
		break;
	}
	_set_timer(amsdu_timer, 1);
}

void rtw_amsdu_cancel_timer(_adapter *padapter, u8 priority)
{
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;
	_timer *amsdu_timer = NULL;

	switch (priority) {
	case 1:
	case 2:
		amsdu_timer = &pxmitpriv->amsdu_bk_timer;
		break;
	case 4:
	case 5:
		amsdu_timer = &pxmitpriv->amsdu_vi_timer;
		break;
	case 6:
	case 7:
		amsdu_timer = &pxmitpriv->amsdu_vo_timer;
		break;
	case 0:
	case 3:
	default:
		amsdu_timer = &pxmitpriv->amsdu_be_timer;
		break;
	}
	_cancel_timer_ex(amsdu_timer);
}
#endif /* CONFIG_TX_AMSDU */


#ifdef CONFIG_PCI_TX_POLLING
void rtw_tx_poll_init(_adapter *padapter)
{
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;
	_timer *timer = &pxmitpriv->tx_poll_timer;

	if (!is_primary_adapter(padapter))
		return;

	if (timer->function != NULL) {
		RTW_INFO("tx polling timer has been init.\n");
		return;
	}

	rtw_init_timer(timer, rtw_tx_poll_timeout_handler, padapter);
	rtw_tx_poll_timer_set(padapter, 1);
	RTW_INFO("Tx poll timer init!\n");
}

void rtw_tx_poll_timeout_handler(void *FunctionContext)
{
	_adapter *adapter = (_adapter *)FunctionContext;

	rtw_tx_poll_timer_set(adapter, 1);

	if (adapter->dvobj->hal_func.tx_poll_handler)
		adapter->dvobj->hal_func.tx_poll_handler(adapter);
	else
		RTW_WARN("hal ops: tx_poll_handler is NULL\n");
}

void rtw_tx_poll_timer_set(_adapter *padapter, u32 delay)
{
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;
	_timer *timer = NULL;

	timer = &pxmitpriv->tx_poll_timer;
	_set_timer(timer, delay);
}

void rtw_tx_poll_timer_cancel(_adapter *padapter)
{
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;
	_timer *timer = NULL;

	if (!is_primary_adapter(padapter))
		return;

	timer = &pxmitpriv->tx_poll_timer;
	_cancel_timer_ex(timer);
	timer->function = NULL;
	RTW_INFO("Tx poll timer cancel !\n");
}
#endif /* CONFIG_PCI_TX_POLLING */

void rtw_sctx_init(struct submit_ctx *sctx, int timeout_ms)
{
	sctx->timeout_ms = timeout_ms;
	sctx->submit_time = rtw_get_current_time();
	_rtw_init_completion(&sctx->done);
	sctx->status = RTW_SCTX_SUBMITTED;
}

int rtw_sctx_wait(struct submit_ctx *sctx, const char *msg)
{
	int ret = _FAIL;
	unsigned long expire;
	int status = 0;

#ifdef PLATFORM_LINUX
	expire = sctx->timeout_ms ? msecs_to_jiffies(sctx->timeout_ms) : MAX_SCHEDULE_TIMEOUT;
	if (!_rtw_wait_for_comp_timeout(&sctx->done, expire)) {
		/* timeout, do something?? */
		status = RTW_SCTX_DONE_TIMEOUT;
		RTW_INFO("%s timeout: %s\n", __func__, msg);
	} else
		status = sctx->status;
#endif

	if (status == RTW_SCTX_DONE_SUCCESS)
		ret = _SUCCESS;

	return ret;
}

bool rtw_sctx_chk_waring_status(int status)
{
	switch (status) {
	case RTW_SCTX_DONE_UNKNOWN:
	case RTW_SCTX_DONE_BUF_ALLOC:
	case RTW_SCTX_DONE_BUF_FREE:

	case RTW_SCTX_DONE_DRV_STOP:
	case RTW_SCTX_DONE_DEV_REMOVE:
		return _TRUE;
	default:
		return _FALSE;
	}
}

void rtw_sctx_done_err(struct submit_ctx **sctx, int status)
{
	if (*sctx) {
		if (rtw_sctx_chk_waring_status(status))
			RTW_INFO("%s status:%d\n", __func__, status);
		(*sctx)->status = status;
#ifdef PLATFORM_LINUX
		complete(&((*sctx)->done));
#endif
		*sctx = NULL;
	}
}

void rtw_sctx_done(struct submit_ctx **sctx)
{
	rtw_sctx_done_err(sctx, RTW_SCTX_DONE_SUCCESS);
}

#ifdef CONFIG_XMIT_ACK
int rtw_ack_tx_wait(struct xmit_priv *pxmitpriv, u32 timeout_ms)
{
	struct submit_ctx *pack_tx_ops = &pxmitpriv->ack_tx_ops;

	pack_tx_ops->submit_time = rtw_get_current_time();
	pack_tx_ops->timeout_ms = timeout_ms;
	pack_tx_ops->status = RTW_SCTX_SUBMITTED;

	return rtw_sctx_wait(pack_tx_ops, __func__);
}

void rtw_ack_tx_done(struct xmit_priv *pxmitpriv, int status)
{
	struct submit_ctx *pack_tx_ops = &pxmitpriv->ack_tx_ops;

	if (pxmitpriv->ack_tx)
		rtw_sctx_done_err(&pack_tx_ops, status);
	else
		RTW_INFO("%s ack_tx not set\n", __func__);
}
#endif /* CONFIG_XMIT_ACK */

#ifdef CONFIG_PCI_HCI
void rtw_os_query_local_buf(void *priv, struct tx_local_buf *buf)
{
	struct dvobj_priv *obj = (struct dvobj_priv *)priv;
	PPCI_DATA pci_data = dvobj_to_pci(obj);
	struct pci_dev *pdev = pci_data->ppcidev;
	dma_addr_t phy_addr;
	size_t alloc_size = 12 * 1024; /* MPDU maximum size in spec: 12k */

	buf->vir_addr = pci_alloc_cache_mem(pdev, &phy_addr, alloc_size,
					    DMA_TO_DEVICE);
	if (!buf->vir_addr)
		return;

#ifdef CONFIG_ARCH_DMA_ADDR_T_64BIT
	buf->phy_addr_h =  phy_addr >> 32;
#else
	buf->phy_addr_h = 0x0;
#endif
	buf->phy_addr_l = phy_addr & 0xFFFFFFFF;
	buf->buf_max_size = alloc_size;
	buf->cache = true;
}

void rtw_os_return_local_buf(void *priv, struct tx_local_buf *buf)
{
	struct dvobj_priv *obj = (struct dvobj_priv *)priv;
	PPCI_DATA pci_data = dvobj_to_pci(obj);
	struct pci_dev *pdev = pci_data->ppcidev;
	dma_addr_t phy_addr = buf->phy_addr_l;

#ifdef CONFIG_ARCH_DMA_ADDR_T_64BIT
	{
		u64 phy_addr_h = buf->phy_addr_h;

		phy_addr |= (phy_addr_h << 32);
	}
#endif

	pci_free_cache_mem(pdev, buf->vir_addr, &phy_addr, buf->buf_max_size,
			   DMA_TO_DEVICE);
}
#endif

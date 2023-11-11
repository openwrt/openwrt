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
#define _RTW_TRX_PCI_C_
#include <drv_types.h>		/* struct dvobj_priv and etc. */

static void rtw_mi_pci_tasklets_kill(_adapter *padapter)
{
	int i;
	_adapter *iface;
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);

	for (i = 0; i < dvobj->iface_nums; i++) {
		iface = dvobj->padapters[i];
		if ((iface) && rtw_is_adapter_up(iface)) {
			rtw_tasklet_kill(&(padapter->xmitpriv.xmit_tasklet));
		}
	}
}

/********************************xmit section*****************************/
static void pci_xmit_tasklet(unsigned long data)
{
#ifdef CONFIG_TX_AMSDU_SW_MODE
	_adapter *padapter = (_adapter *) data;
	core_tx_amsdu_tasklet(padapter);
#endif
}

s32 pci_init_xmit_priv(_adapter *adapter)
{
	s32 ret = _SUCCESS;
	struct xmit_priv *pxmitpriv = &adapter->xmitpriv;
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);

	_rtw_spinlock_init(&dvobj_to_pci(dvobj)->irq_th_lock);

#ifdef CONFIG_TX_AMSDU_SW_MODE
	rtw_tasklet_init(&pxmitpriv->xmit_tasklet,
		     pci_xmit_tasklet,
		     (unsigned long) adapter);
#endif
	return ret;
}

void pci_free_xmit_priv(_adapter *adapter)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);

	_rtw_spinlock_free(&dvobj_to_pci(dvobj)->irq_th_lock);
}

static s32 pci_xmit_direct(_adapter *adapter, struct xmit_frame *pxmitframe)
{
	s32 res = _SUCCESS;
#if 0
	#ifdef CONFIG_XMIT_THREAD_MODE
	struct xmit_priv *pxmitpriv = &adapter->xmitpriv;
	#endif


	res = rtw_xmitframe_coalesce(adapter, pxmitframe->pkt, pxmitframe);
	if (res == _SUCCESS) {
	#if 0 /*def CONFIG_XMIT_THREAD_MODE*/
		enqueue_pending_xmitbuf(pxmitpriv, pxmitframe->pxmitbuf);
	#else
		pci_dump_xframe(adapter, pxmitframe);
	#endif
	}
#endif
	return res;
}

static s32 pci_data_xmit(_adapter *adapter, struct xmit_frame *pxmitframe)
{
	s32 res;
	#if 0 /*CONFIG_CORE_XMITBUF*/
	struct xmit_buf *pxmitbuf = NULL;
	#endif
	struct xmit_priv *pxmitpriv = &adapter->xmitpriv;
	struct pkt_attrib *pattrib = &pxmitframe->attrib;
	struct mlme_priv *pmlmepriv = &adapter->mlmepriv;
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
#ifdef CONFIG_TX_AMSDU
	int tx_amsdu = adapter->tx_amsdu;
	u8 amsdu_timeout = 0;
#endif

	_rtw_spinlock_bh(&pxmitpriv->lock);

	if (rtw_txframes_sta_ac_pending(adapter, pattrib) > 0)
		goto enqueue;

#ifndef CONFIG_XMIT_THREAD_MODE
	/*
	if (rtw_hal_pci_check_enough_txdesc(GET_PHL_COM(dvobj), pattrib->qsel) == _FALSE)
		goto enqueue;
	*/
	if (rtw_xmit_ac_blocked(adapter) == _TRUE)
		goto enqueue;
#endif


	if (rtw_mi_check_status(adapter, MI_STA_LINKING))
		goto enqueue;

#ifdef CONFIG_TX_AMSDU
	if (MLME_IS_STA(adapter) &&
		check_amsdu_tx_support(adapter)) {

		if (IS_AMSDU_AMPDU_VALID(pattrib))
			goto enqueue;
	}
#endif
#if 0 /*CONFIG_CORE_XMITBUF*/
	pxmitbuf = rtw_alloc_xmitbuf(pxmitpriv);
	if (pxmitbuf == NULL)
		goto enqueue;

	_rtw_spinunlock_bh(&pxmitpriv->lock);

	pxmitframe->pxmitbuf = pxmitbuf;
	pxmitframe->buf_addr = pxmitbuf->pbuf;
	pxmitbuf->priv_data = pxmitframe;
#else
	_rtw_spinunlock_bh(&pxmitpriv->lock);
#endif
	if (pci_xmit_direct(adapter, pxmitframe) != _SUCCESS) {
		#if 0 /*CONFIG_CORE_XMITBUF*/
		rtw_free_xmitbuf(pxmitpriv, pxmitbuf);
		#endif
		rtw_free_xmitframe(pxmitpriv, pxmitframe);
	}

	return _TRUE;

enqueue:
	res = rtw_xmitframe_enqueue(adapter, pxmitframe);

#ifdef CONFIG_TX_AMSDU
	if(res == _SUCCESS && tx_amsdu == 2)
	{
		amsdu_timeout = rtw_amsdu_get_timer_status(adapter, pattrib->priority);
		if(amsdu_timeout == RTW_AMSDU_TIMER_SETTING)
		{
			rtw_amsdu_cancel_timer(adapter, pattrib->priority);
			rtw_amsdu_set_timer_status(adapter, pattrib->priority,
				RTW_AMSDU_TIMER_UNSET);
		}
	}
#endif

	_rtw_spinunlock_bh(&pxmitpriv->lock);

	if (res != _SUCCESS) {
		rtw_free_xmitframe(pxmitpriv, pxmitframe);

		pxmitpriv->tx_drop++;
		return _TRUE;
	}

#ifdef CONFIG_TX_AMSDU
	rtw_tasklet_hi_schedule(&pxmitpriv->xmit_tasklet);
#endif
	return _FALSE;
}

/******************************** recv section*******************************/
s32 pci_init_recv_priv(struct dvobj_priv *dvobj)
{
	s32 ret = _SUCCESS;

	return ret;
}

void pci_free_recv_priv(struct dvobj_priv *dvobj)
{

}


struct rtw_intf_ops pci_ops = {
	.init_xmit_priv = pci_init_xmit_priv,
	.free_xmit_priv = pci_free_xmit_priv,
	.data_xmit	= pci_data_xmit,

	.init_recv_priv = pci_init_recv_priv,
	.free_recv_priv = pci_free_recv_priv,
};


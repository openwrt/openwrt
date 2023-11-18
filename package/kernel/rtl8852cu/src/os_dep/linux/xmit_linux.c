/******************************************************************************
 *
 * Copyright(c) 2007 - 2019 Realtek Corporation.
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
#define _XMIT_OSDEP_C_

#include <drv_types.h>

#define DBG_DUMP_OS_QUEUE_CTL 0

uint rtw_remainder_len(struct pkt_file *pfile)
{
	return pfile->buf_len - ((SIZE_PTR)(pfile->cur_addr) - (SIZE_PTR)(pfile->buf_start));
}

void _rtw_open_pktfile(struct sk_buff *pktptr, struct pkt_file *pfile)
{

	pfile->pkt = pktptr;
	pfile->cur_addr = pfile->buf_start = pktptr->data;
	pfile->pkt_len = pfile->buf_len = pktptr->len;

	pfile->cur_buffer = pfile->buf_start ;

}

uint _rtw_pktfile_read(struct pkt_file *pfile, u8 *rmem, uint rlen)
{
	uint	len = 0;


	len =  rtw_remainder_len(pfile);
	len = (rlen > len) ? len : rlen;

	if (rmem)
		skb_copy_bits(pfile->pkt, pfile->buf_len - pfile->pkt_len, rmem, len);

	pfile->cur_addr += len;
	pfile->pkt_len -= len;


	return len;
}

sint rtw_endofpktfile(struct pkt_file *pfile)
{

	if (pfile->pkt_len == 0) {
		return _TRUE;
	}


	return _FALSE;
}

void rtw_set_tx_chksum_offload(struct sk_buff *pkt, struct pkt_attrib *pattrib)
{
#ifdef CONFIG_TCP_CSUM_OFFLOAD_TX	
	struct sk_buff *skb = (struct sk_buff *)pkt;
	struct iphdr *iph = NULL;
	struct ipv6hdr *i6ph = NULL;
	struct udphdr *uh = NULL;
	struct tcphdr *th = NULL;
	u8 	protocol = 0xFF;

	if (skb->protocol == htons(ETH_P_IP)) {
		iph = (struct iphdr *)skb_network_header(skb);
		protocol = iph->protocol;
	} else if (skb->protocol == htons(ETH_P_IPV6)) {
		i6ph = (struct ipv6hdr *)skb_network_header(skb);
		protocol = i6ph->nexthdr;
	} else
		{}

	/*	HW unable to compute CSUM if header & payload was be encrypted by SW(cause TXDMA error) */
	if (pattrib->bswenc == _TRUE) {
		if (skb->ip_summed == CHECKSUM_PARTIAL)
			skb_checksum_help(skb);
		return;
	}

	/*	For HW rule, clear ipv4_csum & UDP/TCP_csum if it is UDP/TCP packet	*/
	switch (protocol) {
	case IPPROTO_UDP:
		uh = (struct udphdr *)skb_transport_header(skb);
		uh->check = 0;
		if (iph)
			iph->check = 0;
		pattrib->hw_csum = _TRUE;
		break;
	case IPPROTO_TCP:
		th = (struct tcphdr *)skb_transport_header(skb);
		th->check = 0;
		if (iph)
			iph->check = 0;
		pattrib->hw_csum = _TRUE;
		break;
	default:
		break;
	}
#endif

}
#if 0 /*CONFIG_CORE_XMITBUF*/
int rtw_os_xmit_resource_alloc(_adapter *padapter, struct xmit_buf *pxmitbuf, u32 alloc_sz, u8 flag)
{
	if (alloc_sz > 0) {
#ifdef CONFIG_USE_USB_BUFFER_ALLOC_TX
		struct dvobj_priv	*pdvobjpriv = adapter_to_dvobj(padapter);
		struct usb_device	*pusbd = dvobj_to_usb(pdvobjpriv)->pusbdev;

		pxmitbuf->pallocated_buf = rtw_usb_buffer_alloc(pusbd, (size_t)alloc_sz, &pxmitbuf->dma_transfer_addr);
		pxmitbuf->pbuf = pxmitbuf->pallocated_buf;
		if (pxmitbuf->pallocated_buf == NULL)
			return _FAIL;
#else /* CONFIG_USE_USB_BUFFER_ALLOC_TX */

		pxmitbuf->pallocated_buf = rtw_zmalloc(alloc_sz);
		if (pxmitbuf->pallocated_buf == NULL)
			return _FAIL;

		pxmitbuf->pbuf = (u8 *)N_BYTE_ALIGMENT((SIZE_PTR)(pxmitbuf->pallocated_buf), SZ_ALIGN_XMITFRAME_EXT);

#endif /* CONFIG_USE_USB_BUFFER_ALLOC_TX */
	}

	if (flag) {
#ifdef CONFIG_USB_HCI
		int i;
		for (i = 0; i < 8; i++) {
			pxmitbuf->pxmit_urb[i] = usb_alloc_urb(0, GFP_KERNEL);
			if (pxmitbuf->pxmit_urb[i] == NULL) {
				RTW_INFO("pxmitbuf->pxmit_urb[i]==NULL");
				return _FAIL;
			}
		}
#endif
	}

	return _SUCCESS;
}

void rtw_os_xmit_resource_free(_adapter *padapter, struct xmit_buf *pxmitbuf, u32 free_sz, u8 flag)
{
	if (flag) {
#ifdef CONFIG_USB_HCI
		int i;

		for (i = 0; i < 8; i++) {
			if (pxmitbuf->pxmit_urb[i]) {
				/* usb_kill_urb(pxmitbuf->pxmit_urb[i]); */
				usb_free_urb(pxmitbuf->pxmit_urb[i]);
			}
		}
#endif
	}

	if (free_sz > 0) {
#ifdef CONFIG_USE_USB_BUFFER_ALLOC_TX
		struct dvobj_priv	*pdvobjpriv = adapter_to_dvobj(padapter);
		struct usb_device	*pusbd = dvobj_to_usb(pdvobjpriv)->pusbdev;

		rtw_usb_buffer_free(pusbd, (size_t)free_sz, pxmitbuf->pallocated_buf, pxmitbuf->dma_transfer_addr);
		pxmitbuf->pallocated_buf =  NULL;
		pxmitbuf->dma_transfer_addr = 0;
#else	/* CONFIG_USE_USB_BUFFER_ALLOC_TX */
		if (pxmitbuf->pallocated_buf)
			rtw_mfree(pxmitbuf->pallocated_buf, free_sz);
#endif /* CONFIG_USE_USB_BUFFER_ALLOC_TX */
	}
}
#else
u8 rtw_os_xmit_resource_alloc(_adapter *padapter, struct xmit_frame *pxframe)
{
	u32 alloc_sz = SZ_XMITFRAME_EXT + SZ_ALIGN_XMITFRAME_EXT;

#if 0 /*def CONFIG_USE_USB_BUFFER_ALLOC_TX*/
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	struct usb_device *pusbd = dvobj_to_usb(dvobj)->pusbdev;

	pxframe->prealloc_buf_addr = rtw_usb_buffer_alloc(pusbd, (size_t)alloc_sz, &pxframe->dma_transfer_addr);
	if (pxframe->prealloc_buf_addr == NULL) {
		RTW_ERR("%s prealloc_buf_addr failed\n", __func__);
		rtw_warn_on(1);
		return _FAIL;
	}
	pxframe->buf_addr = pxframe->prealloc_buf_addr;
#else
	pxframe->prealloc_buf_addr = rtw_zmalloc(alloc_sz);
	if (pxframe->prealloc_buf_addr == NULL) {
		RTW_ERR("%s prealloc_buf_addr failed\n", __func__);
		rtw_warn_on(1);
		return _FAIL;
	}
	pxframe->buf_addr = (u8 *)N_BYTE_ALIGMENT((SIZE_PTR)(pxframe->prealloc_buf_addr), SZ_ALIGN_XMITFRAME_EXT);
#endif
	return _SUCCESS;
}

void rtw_os_xmit_resource_free(_adapter *padapter, struct xmit_frame *pxframe)
{
	u32 free_sz = SZ_XMITFRAME_EXT + SZ_ALIGN_XMITFRAME_EXT;

#if 0 /*def CONFIG_USE_USB_BUFFER_ALLOC_TX*/
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	struct usb_device *pusbd = dvobj_to_usb(dvobj)->pusbdev;

	if (pxframe->prealloc_buf_addr) {
		rtw_usb_buffer_free(pusbd, (size_t)free_sz, pxframe->prealloc_buf_addr, pxframe->dma_transfer_addr);
		pxframe->prealloc_buf_addr = NULL;
		pxframe->buf_addr = NULL;
		pxframe->dma_transfer_addr = 0;
	}
#else
	if (pxframe->prealloc_buf_addr) {
		rtw_mfree(pxframe->prealloc_buf_addr, free_sz);
		pxframe->prealloc_buf_addr = NULL;
		pxframe->buf_addr = NULL;
	}
#endif
}
#endif

void dump_os_queue(void *sel, _adapter *padapter)
{
	struct net_device *ndev = padapter->pnetdev;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 35))
	int i;

	for (i = 0; i < 4; i++) {
		RTW_PRINT_SEL(sel, "os_queue[%d]:%s\n"
			, i, __netif_subqueue_stopped(ndev, i) ? "stopped" : "waked");
	}
#else
	RTW_PRINT_SEL(sel, "os_queue:%s\n"
		      , netif_queue_stopped(ndev) ? "stopped" : "waked");
#endif
}

#define WMM_XMIT_THRESHOLD	(NR_XMITFRAME*2/5)

static inline bool rtw_os_need_wake_queue(_adapter *padapter, u16 os_qid)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 35))
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;

	if (padapter->registrypriv.wifi_spec) {
		if (pxmitpriv->hwxmits[os_qid].accnt < WMM_XMIT_THRESHOLD)
			return _TRUE;
#ifdef DBG_CONFIG_ERROR_DETECT
#ifdef DBG_CONFIG_ERROR_RESET
	} else if (rtw_hal_sreset_inprogress(padapter) == _TRUE) {
		return _FALSE;
#endif/* #ifdef DBG_CONFIG_ERROR_RESET */
#endif/* #ifdef DBG_CONFIG_ERROR_DETECT */
	} else {
		return _TRUE;
	}
	return _FALSE;
#else
	return _TRUE;
#endif
}

static inline bool rtw_os_need_stop_queue(_adapter *padapter, u16 os_qid)
{
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 35))
	if (padapter->registrypriv.wifi_spec) {
		/* No free space for Tx, tx_worker is too slow */
		if (pxmitpriv->hwxmits[os_qid].accnt > WMM_XMIT_THRESHOLD)
			return _TRUE;
	} else {
		if (pxmitpriv->free_xmitframe_cnt <= 4)
			return _TRUE;
	}
#else
	if (pxmitpriv->free_xmitframe_cnt <= 4)
		return _TRUE;
#endif
	return _FALSE;
}

void rtw_os_pkt_complete(_adapter *padapter, struct sk_buff *pkt)
{
	rtw_skb_free(pkt);
}

void rtw_os_xmit_complete(_adapter *padapter, struct xmit_frame *pxframe)
{
	if (pxframe->pkt)
		rtw_os_pkt_complete(padapter, pxframe->pkt);

	pxframe->pkt = NULL;
}

void rtw_os_xmit_schedule(_adapter *padapter)
{
#if 0 /*defined(CONFIG_SDIO_HCI) || defined(CONFIG_GSPI_HCI)*/
	_adapter *pri_adapter;

	if (!padapter)
		return;
	pri_adapter = GET_PRIMARY_ADAPTER(padapter);

	if (_rtw_queue_empty(&padapter->xmitpriv.pending_xmitbuf_queue) == _FALSE)
		_rtw_up_sema(&pri_adapter->xmitpriv.xmit_sema);


#elif defined(CONFIG_PCI_HCI) || defined(CONFIG_USB_HCI)
	struct xmit_priv *pxmitpriv;

	if (!padapter)
		return;

	pxmitpriv = &padapter->xmitpriv;

	_rtw_spinlock_bh(&pxmitpriv->lock);

	if (rtw_txframes_pending(padapter))
		rtw_tasklet_hi_schedule(&pxmitpriv->xmit_tasklet);

	_rtw_spinunlock_bh(&pxmitpriv->lock);
	
	#if 0 /*defined(CONFIG_PCI_HCI) && defined(CONFIG_XMIT_THREAD_MODE)*/
	if (_rtw_queue_empty(&padapter->xmitpriv.pending_xmitbuf_queue) == _FALSE)
		_rtw_up_sema(&padapter->xmitpriv.xmit_sema);
	#endif
	

#endif
}

void rtw_os_check_wakup_queue(_adapter *padapter, u16 os_qid)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 35))
	if (rtw_os_need_wake_queue(padapter, os_qid)) {
		if (DBG_DUMP_OS_QUEUE_CTL)
			RTW_INFO(FUNC_ADPT_FMT": netif_wake_subqueue[%d]\n", FUNC_ADPT_ARG(padapter), os_qid);
		netif_wake_subqueue(padapter->pnetdev, os_qid);
	}
#else
	if (rtw_os_need_wake_queue(padapter, 0)) {
		if (DBG_DUMP_OS_QUEUE_CTL)
			RTW_INFO(FUNC_ADPT_FMT": netif_wake_queue\n", FUNC_ADPT_ARG(padapter));
		netif_wake_queue(padapter->pnetdev);
	}
#endif
}

bool rtw_os_check_stop_queue(_adapter *padapter, u16 os_qid)
{
	bool busy = _FALSE;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 35))
	if (rtw_os_need_stop_queue(padapter, os_qid)) {
		if (DBG_DUMP_OS_QUEUE_CTL)
			RTW_INFO(FUNC_ADPT_FMT": netif_stop_subqueue[%d]\n", FUNC_ADPT_ARG(padapter), os_qid);
		netif_stop_subqueue(padapter->pnetdev, os_qid);
		busy = _TRUE;
	}
#else
	if (rtw_os_need_stop_queue(padapter, 0)) {
		if (DBG_DUMP_OS_QUEUE_CTL)
			RTW_INFO(FUNC_ADPT_FMT": netif_stop_queue\n", FUNC_ADPT_ARG(padapter));
		rtw_netif_stop_queue(padapter->pnetdev);
		busy = _TRUE;
	}
#endif
	return busy;
}

void rtw_os_wake_queue_at_free_stainfo(_adapter *padapter, int *qcnt_freed)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 35))
	int i;

	for (i = 0; i < 4; i++) {
		if (qcnt_freed[i] == 0)
			continue;

		if (rtw_os_need_wake_queue(padapter, i)) {
			if (DBG_DUMP_OS_QUEUE_CTL)
				RTW_INFO(FUNC_ADPT_FMT": netif_wake_subqueue[%d]\n", FUNC_ADPT_ARG(padapter), i);
			netif_wake_subqueue(padapter->pnetdev, i);
		}
	}
#else
	if (qcnt_freed[0] || qcnt_freed[1] || qcnt_freed[2] || qcnt_freed[3]) {
		if (rtw_os_need_wake_queue(padapter, 0)) {
			if (DBG_DUMP_OS_QUEUE_CTL)
				RTW_INFO(FUNC_ADPT_FMT": netif_wake_queue\n", FUNC_ADPT_ARG(padapter));
			netif_wake_queue(padapter->pnetdev);
		}
	}
#endif
}

int _rtw_xmit_entry(struct sk_buff *pkt, _nic_hdl pnetdev)
{
	_adapter *padapter = (_adapter *)rtw_netdev_priv(pnetdev);
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;
#ifdef CONFIG_TCP_CSUM_OFFLOAD_TX	
	struct sk_buff *skb = pkt;
	struct sk_buff *segs, *nskb;
	netdev_features_t features = padapter->pnetdev->features;
#endif
	u16 os_qid = 0;
	s32 res = 0;

	if (padapter->registrypriv.mp_mode) {
		RTW_INFO("MP_TX_DROP_OS_FRAME\n");
		goto drop_packet;
	}
	DBG_COUNTER(padapter->tx_logs.os_tx);

	if (rtw_if_up(padapter) == _FALSE) {
		DBG_COUNTER(padapter->tx_logs.os_tx_err_up);
		#ifdef DBG_TX_DROP_FRAME
		RTW_INFO("DBG_TX_DROP_FRAME %s if_up fail\n", __FUNCTION__);
		#endif
		goto drop_packet;
	}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 35))
	os_qid = skb_get_queue_mapping(pkt);
#endif

#ifdef CONFIG_TCP_CSUM_OFFLOAD_TX
	if (skb_shinfo(skb)->gso_size) {
	/*	split a big(65k) skb into several small(1.5k) skbs */
		features &= ~(NETIF_F_TSO | NETIF_F_TSO6);
		segs = skb_gso_segment(skb, features);
		if (IS_ERR(segs) || !segs)
			goto drop_packet;

		do {
			nskb = segs;
			segs = segs->next;
			nskb->next = NULL;
			rtw_mstat_update( MSTAT_TYPE_SKB, MSTAT_ALLOC_SUCCESS, nskb->truesize);
			res = rtw_xmit(padapter, &nskb, os_qid);
			if (res < 0) {
				#ifdef DBG_TX_DROP_FRAME
				RTW_INFO("DBG_TX_DROP_FRAME %s rtw_xmit fail\n", __FUNCTION__);
				#endif
				pxmitpriv->tx_drop++;
				rtw_os_pkt_complete(padapter, nskb);
			}
		} while (segs);
		rtw_os_pkt_complete(padapter, skb);
		goto exit;
	}
#endif

	res = rtw_xmit(padapter, &pkt, os_qid);
	if (res < 0) {
		#ifdef DBG_TX_DROP_FRAME
		RTW_INFO("DBG_TX_DROP_FRAME %s rtw_xmit fail\n", __FUNCTION__);
		#endif
		goto drop_packet;
	}

	goto exit;

drop_packet:
	pxmitpriv->tx_drop++;
	rtw_os_pkt_complete(padapter, pkt);

exit:


	return 0;
}

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 10, 0))
/* copy from skbuff.c to be compatible with old kernel */
static void kfree_skb_list(struct sk_buff *segs)
{
	while (segs) {
		struct sk_buff *next = segs->next;

		kfree_skb(segs);
		segs = next;
	}
}
#endif


#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 32))
netdev_tx_t rtw_xmit_entry(struct sk_buff *pkt, _nic_hdl pnetdev)
#else
int rtw_xmit_entry(struct sk_buff *pkt, _nic_hdl pnetdev)
#endif
{
	_adapter *padapter = (_adapter *)rtw_netdev_priv(pnetdev);
	struct	mlme_priv	*pmlmepriv = &(padapter->mlmepriv);
	int ret = 0;

	if (pkt) {
		if (check_fwstate(pmlmepriv, WIFI_MONITOR_STATE) == _TRUE) {
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 24))
			rtw_monitor_xmit_entry((struct sk_buff *)pkt, pnetdev);
#endif
		}
		else {
#ifdef CONFIG_RTW_NETIF_SG
			/* After turning on SG, net stack may (0.0025%) TX
			 * strange skb that is skb_has_frag_list() but linear
			 * (i.e. skb_is_nonlinear() is false). This is out of
			 * our expectation, so I free fragment list to be
			 * compatible with our design.
			 */
			if (skb_has_frag_list(pkt)) {
				if (!skb_is_nonlinear(pkt)) {
					kfree_skb_list(skb_shinfo(pkt)->frag_list);
					skb_shinfo(pkt)->frag_list = NULL;
					RTW_DBG("%s:%d free frag list\n", __func__, __LINE__);
				} else {
					RTW_DBG("%s:%d nonlinear frag list\n", __func__, __LINE__);
				}
			}
#endif
			rtw_mstat_update(MSTAT_TYPE_SKB, MSTAT_ALLOC_SUCCESS, pkt->truesize);
			ret = rtw_os_tx(pkt, pnetdev);
		}

	}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 32))
	return (ret == 0) ? NETDEV_TX_OK : NETDEV_TX_BUSY;
#else
	return ret;
#endif
}



#ifdef RTW_PHL_TX
int rtw_os_is_adapter_ready(_adapter *padapter, struct sk_buff *pkt)
{

	if (padapter->registrypriv.mp_mode) {
		RTW_INFO("MP_TX_DROP_OS_FRAME\n");
		return _FALSE;
	}

	DBG_COUNTER(padapter->tx_logs.os_tx);

	if (rtw_if_up(padapter) == _FALSE) {
		PHLTX_LOG;
		DBG_COUNTER(padapter->tx_logs.os_tx_err_up);
	#ifdef DBG_TX_DROP_FRAME
		RTW_INFO("DBG_TX_DROP_FRAME %s if_up fail\n", __FUNCTION__);
	#endif
		return _FALSE;
	}

	if (IS_CH_WAITING(adapter_to_rfctl(padapter))){
		PHLTX_LOG;
		return _FALSE;
	}

	if (rtw_linked_check(padapter) == _FALSE){
		PHLTX_LOG;
		return _FALSE;
	}

	return _TRUE;
}

int rtw_os_tx(struct sk_buff *pkt, _nic_hdl pnetdev)
{
	_adapter *padapter = (_adapter *)rtw_netdev_priv(pnetdev);
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;
	u16 os_qid = 0;
	s32 res = 0;

#ifdef RTW_PHL_DBG_CMD
	core_add_record(padapter, REC_TX_DATA, pkt);
#endif

	PHLTX_LOG;

	if (pkt->len == 0)
		return 0;

	if ((rtw_os_is_adapter_ready(padapter, pkt) == _FALSE)
#ifdef CONFIG_LAYER2_ROAMING
		&& (!padapter->mlmepriv.roam_network)
#endif
	)
		goto drop_packet;

	if (adapter_to_pwrctl(padapter)->bInSuspend == _TRUE) {
		RTW_INFO("[%s] Stop TX because bInSuspend has been carried! \n", __func__);
		goto drop_packet;
	}

	PHLTX_LOG;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 35))
	os_qid = skb_get_queue_mapping(pkt);
#endif

	PHLTX_LOG;
	if (rtw_core_tx(padapter, &pkt, NULL, os_qid) == FAIL)
		goto inc_drop_cnt;

	PHLTX_LOG;

	goto exit;

drop_packet:
	rtw_os_pkt_complete(padapter, pkt);

inc_drop_cnt:
	pxmitpriv->tx_drop++;

exit:
	return 0;
}
#endif

#ifdef CONFIG_TX_AMSDU_SW_MODE
static void ieee8023_header_to_rfc1042(struct sk_buff *skb, int pads)
{
	void *data;
	int pad;
	__be16 len;
	const int headroom = SNAP_SIZE + 2 + pads;

	if (!skb)
		return;

	if (skb_headroom(skb) < headroom) {
		RTW_WARN("%s: headroom=%d isn't enough\n", __func__, skb_headroom(skb));
		if (pskb_expand_head(skb, headroom, 0, GFP_ATOMIC)) {
			RTW_ERR("%s: no headroom=%d for skb\n",
				__func__, headroom);
			return;
		}
	}

	data = skb_push(skb, headroom);
	memset(data, 0, pads);
	data += pads;
	memmove(data, data + SNAP_SIZE + 2, 2 * ETH_ALEN);
	data += 2 * ETH_ALEN;
	len = cpu_to_be16(skb->len - pads - 2 * ETH_ALEN - 2);
	memcpy(data, &len, 2);
	memcpy(data + 2, rtw_rfc1042_header, SNAP_SIZE);
}

void rtw_coalesce_tx_amsdu(_adapter *padapter, struct xmit_frame *pxframes[],
			   int xf_nr, bool amsdu, u32 *pktlen)
{
	struct xmit_frame *head_xframe;
	struct xmit_frame *pxframe;
	struct sk_buff *skb;
	struct sk_buff *head_skb;
	struct sk_buff **frag_tail;
	int pads;
	int i;

	/* prepare head xmitframe */
	head_xframe = pxframes[0];
	head_skb = head_xframe->pkt;

	ieee8023_header_to_rfc1042(head_skb, 0);

	frag_tail = &skb_shinfo(head_skb)->frag_list;
	while (*frag_tail)
		frag_tail = &(*frag_tail)->next;

	for (i = 1; i < xf_nr; i++) {
		pxframe = pxframes[i];
		skb = pxframe->pkt;

		if (head_skb->len & 0x03)
			pads = 4 - (head_skb->len & 0x03);
		else
			pads = 0;

		ieee8023_header_to_rfc1042(skb, pads);

		/* free sk accounting to have TP like doing skb_linearize() */
		if (skb->destructor)
			skb_orphan(skb);

		/* add this skb to head_skb */
		head_skb->len += skb->len;
		head_skb->data_len += skb->len;
		*frag_tail = skb;
		while (*frag_tail)
			frag_tail = &(*frag_tail)->next;

		/* free this xframe */
		pxframe->pkt = NULL; /* head xframe own */
		core_tx_free_xmitframe(padapter, pxframe);
	}

	/* total skb length (includes all fragments) */
	*pktlen = head_skb->len;
}
#endif /* CONFIG_TX_AMSDU_SW_MODE */

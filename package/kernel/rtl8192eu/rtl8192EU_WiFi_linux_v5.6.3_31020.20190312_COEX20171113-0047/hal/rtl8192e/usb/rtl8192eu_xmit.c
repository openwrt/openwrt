/******************************************************************************
 *
 * Copyright(c) 2012 - 2017 Realtek Corporation.
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
#define _RTL8812AU_XMIT_C_

/* #include <drv_types.h> */
#include <rtl8192e_hal.h>


s32	rtl8192eu_init_xmit_priv(_adapter *padapter)
{
	struct xmit_priv	*pxmitpriv = &padapter->xmitpriv;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(padapter);

#ifdef PLATFORM_LINUX
	tasklet_init(&pxmitpriv->xmit_tasklet,
		     (void(*)(unsigned long))rtl8192eu_xmit_tasklet,
		     (unsigned long)padapter);
#endif
	rtl8192e_init_xmit_priv(padapter);

	return _SUCCESS;
}

void	rtl8192eu_free_xmit_priv(_adapter *padapter)
{
}

u8 urb_zero_packet_chk(_adapter *padapter, int sz)
{
	u8 blnSetTxDescOffset;
	HAL_DATA_TYPE	*pHalData	= GET_HAL_DATA(padapter);
	blnSetTxDescOffset = (((sz + TXDESC_SIZE) %  pHalData->UsbBulkOutSize) == 0) ? 1 : 0;

	return blnSetTxDescOffset;
}

static s32 update_txdesc(struct xmit_frame *pxmitframe, u8 *pmem, s32 sz , u8 bagg_pkt)
{
	int	pull = 0;
	uint	qsel;
	u8 data_rate, pwr_status, offset;
	_adapter			*padapter = pxmitframe->padapter;
	struct mlme_priv	*pmlmepriv = &padapter->mlmepriv;
	struct pkt_attrib	*pattrib = &pxmitframe->attrib;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(padapter);
	u8	*ptxdesc =  pmem;
	struct mlme_ext_priv	*pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);
	sint	bmcst = IS_MCAST(pattrib->ra);

#ifndef CONFIG_USE_USB_BUFFER_ALLOC_TX
	if (padapter->registrypriv.mp_mode == 0) {
		/* if((!bagg_pkt) &&(urb_zero_packet_chk(padapter, sz)==0)) */ /* (sz %512) != 0 */
		if ((PACKET_OFFSET_SZ != 0) && (!bagg_pkt) && (rtw_usb_bulk_size_boundary(padapter, TXDESC_SIZE + sz) == _FALSE)) {
			ptxdesc = (pmem + PACKET_OFFSET_SZ);
			/* RTW_INFO("==> non-agg-pkt,shift pointer...\n"); */
			pull = 1;
		}
	}
#endif /* CONFIG_USE_USB_BUFFER_ALLOC_TX */

	_rtw_memset(ptxdesc, 0, TXDESC_SIZE);

	/* 4 offset 0 */

	/* RTW_INFO("%s==> pkt_len=%d,bagg_pkt=%02x\n",__FUNCTION__,sz,bagg_pkt); */
	SET_TX_DESC_PKT_SIZE_92E(ptxdesc, sz);

	offset = TXDESC_SIZE + OFFSET_SZ;

#ifdef CONFIG_TX_EARLY_MODE
	if (bagg_pkt) {
		offset += EARLY_MODE_INFO_SIZE ;/* 0x28			 */
	}
#endif
	/* RTW_INFO("%s==>offset(0x%02x)\n",__FUNCTION__,offset); */
	SET_TX_DESC_OFFSET_92E(ptxdesc, offset);

	if (bmcst)
		SET_TX_DESC_BMC_92E(ptxdesc, 1);

#ifndef CONFIG_USE_USB_BUFFER_ALLOC_TX
	if (padapter->registrypriv.mp_mode == 0) {
		if ((PACKET_OFFSET_SZ != 0) && (!bagg_pkt)) {
			if ((pull) && (pxmitframe->pkt_offset > 0))
				pxmitframe->pkt_offset = pxmitframe->pkt_offset - 1;
		}
	}
#endif

	/* RTW_INFO("%s, pkt_offset=0x%02x\n",__FUNCTION__,pxmitframe->pkt_offset); */
	/* pkt_offset, unit:8 bytes padding */
	if (pxmitframe->pkt_offset > 0)
		SET_TX_DESC_PKT_OFFSET_92E(ptxdesc, pxmitframe->pkt_offset);

	SET_TX_DESC_MACID_92E(ptxdesc, pattrib->mac_id);
	SET_TX_DESC_RATE_ID_92E(ptxdesc, pattrib->raid);

	SET_TX_DESC_QUEUE_SEL_92E(ptxdesc,  pattrib->qsel);

	/* offset 12 */

	if (!pattrib->qos_en) {
		SET_TX_DESC_EN_HWSEQ_92E(ptxdesc, 1); /* Hw set sequence number */
		SET_TX_DESC_HWSEQ_SEL_92E(ptxdesc, pattrib->hw_ssn_sel);
	} else
		SET_TX_DESC_SEQ_92E(ptxdesc, pattrib->seqnum);

	if ((pxmitframe->frame_tag & 0x0f) == DATA_FRAMETAG) {
		/* RTW_INFO("pxmitframe->frame_tag == DATA_FRAMETAG\n");		 */

		fill_txdesc_sectype(pattrib, ptxdesc);
#if defined(CONFIG_CONCURRENT_MODE)
		if (bmcst)
			fill_txdesc_force_bmc_camid(pattrib, ptxdesc);
#endif
/*#ifdef CONFIG_SUPPORT_DYNAMIC_TXPWR
		rtw_phydm_set_dyntxpwr(padapter, ptxdesc, pattrib->mac_id);
#endi*/

		if (pattrib->ampdu_en == _TRUE) {
			SET_TX_DESC_AGG_ENABLE_92E(ptxdesc, 1);
			SET_TX_DESC_MAX_AGG_NUM_92E(ptxdesc, 0x1f);
			/* Set A-MPDU aggregation. */
			SET_TX_DESC_AMPDU_DENSITY_92E(ptxdesc, pattrib->ampdu_spacing);
		} else
			SET_TX_DESC_BK_92E(ptxdesc, 1);

		/* offset 20 */
#ifdef CONFIG_USB_TX_AGGREGATION
		if (pxmitframe->agg_num > 1) {
			/* RTW_INFO("%s agg_num:%d\n",__FUNCTION__,pxmitframe->agg_num ); */
			SET_TX_DESC_USB_TXAGG_NUM_92E(ptxdesc, pxmitframe->agg_num);
		}
#endif

		if ((pattrib->ether_type != 0x888e) &&
		    (pattrib->ether_type != 0x0806) &&
		    (pattrib->ether_type != 0x88b4) &&
		    (pattrib->dhcp_pkt != 1)
#ifdef CONFIG_AUTO_AP_MODE
		    && (pattrib->pctrl != _TRUE)
#endif
		   ) {
			/* Non EAP & ARP & DHCP type data packet */

			fill_txdesc_vcs(pattrib, ptxdesc);
			fill_txdesc_phy(padapter, pattrib, ptxdesc);

			SET_TX_DESC_RTS_RATE_92E(ptxdesc, 0x8);/* RTS Rate=24M */
			/* DATA/RTS  Rate FB LMT */
			SET_TX_DESC_DATA_RATE_FB_LIMIT_92E(ptxdesc, 0x1f);
			SET_TX_DESC_RTS_RATE_FB_LIMIT_92E(ptxdesc, 0xf);

			if (pHalData->fw_ractrl == _FALSE)  {
				SET_TX_DESC_USE_RATE_92E(ptxdesc, 1);

				if (pHalData->INIDATA_RATE[pattrib->mac_id] & BIT(7))
					SET_TX_DESC_DATA_SHORT_92E(ptxdesc, 1);

				SET_TX_DESC_TX_RATE_92E(ptxdesc, (pHalData->INIDATA_RATE[pattrib->mac_id] & 0x7F));
			}
			if (bmcst)
				fill_txdesc_bmc_tx_rate(pattrib, ptxdesc);

			/* for debug */
			rtl8192e_fixed_rate(padapter, ptxdesc);

			if (pattrib->ldpc)
				SET_TX_DESC_DATA_LDPC_92E(ptxdesc, 1);
			if (pattrib->stbc)
				SET_TX_DESC_DATA_STBC_92E(ptxdesc, 1);

		} else {
			/* EAP data packet and ARP packet and DHCP. */
			/* Use the 1M data rate to send the EAP/ARP packet. */
			/* This will maybe make the handshake smooth. */

			SET_TX_DESC_USE_RATE_92E(ptxdesc, 1);

			SET_TX_DESC_BK_92E(ptxdesc, 1);

			/* HW will ignore this setting if the transmission rate is legacy OFDM. */
			if (pmlmeinfo->preamble_mode == PREAMBLE_SHORT)
				SET_TX_DESC_DATA_SHORT_92E(ptxdesc, 1);
#ifdef CONFIG_IP_R_MONITOR
			if((pattrib->ether_type == ETH_P_ARP) &&
				(IsSupportedTxOFDM(padapter->registrypriv.wireless_mode))) {

				SET_TX_DESC_TX_RATE_92E(ptxdesc, MRateToHwRate(IEEE80211_OFDM_RATE_6MB));
				#ifdef DBG_IP_R_MONITOR
				RTW_INFO(FUNC_ADPT_FMT ": SP Packet(0x%04X) rate=0x%x SeqNum = %d\n",
					FUNC_ADPT_ARG(padapter), pattrib->ether_type, MRateToHwRate(pmlmeext->tx_rate), pattrib->seqnum);
				#endif/*DBG_IP_R_MONITOR*/
			 } else
#endif/*CONFIG_IP_R_MONITOR*/
				SET_TX_DESC_TX_RATE_92E(ptxdesc, MRateToHwRate(pmlmeext->tx_rate));
		}

#ifdef CONFIG_TDLS
#ifdef CONFIG_XMIT_ACK
		/* CCX-TXRPT ack for xmit mgmt frames. */
		if (pxmitframe->ack_report) {
			SET_TX_DESC_SPE_RPT_92E(ptxdesc, 1);
#ifdef DBG_CCX
			RTW_INFO("%s set tx report\n", __func__);
#endif
		}
#endif /* CONFIG_XMIT_ACK */
#endif
	} else if ((pxmitframe->frame_tag & 0x0f) == MGNT_FRAMETAG) {
		/* RTW_INFO("pxmitframe->frame_tag == MGNT_FRAMETAG\n");	 */

		SET_TX_DESC_MBSSID_92E(ptxdesc, pattrib->mbssid);/* for issue port1/mbssid beacon */

		/* offset 20 */
		SET_TX_DESC_RETRY_LIMIT_ENABLE_92E(ptxdesc, 1);
		if (pattrib->retry_ctrl == _TRUE)
			SET_TX_DESC_DATA_RETRY_LIMIT_92E(ptxdesc, 6);
		else
			SET_TX_DESC_DATA_RETRY_LIMIT_92E(ptxdesc, 12);

		SET_TX_DESC_USE_RATE_92E(ptxdesc, 1);

		SET_TX_DESC_TX_RATE_92E(ptxdesc, MRateToHwRate(pattrib->rate));

#ifdef CONFIG_XMIT_ACK
		/* CCX-TXRPT ack for xmit mgmt frames. */
		if (pxmitframe->ack_report) {
			SET_TX_DESC_SPE_RPT_92E(ptxdesc, 1);
#ifdef DBG_CCX
			RTW_INFO("%s set tx report\n", __func__);
#endif
		}
#endif /* CONFIG_XMIT_ACK */
	}
	/*
		else if((pxmitframe->frame_tag&0x0f) == TXAGG_FRAMETAG)
		{
			RTW_INFO("pxmitframe->frame_tag == TXAGG_FRAMETAG\n");
		}
	*/
#ifdef CONFIG_MP_INCLUDED
	else if (((pxmitframe->frame_tag & 0x0f) == MP_FRAMETAG) &&
		 (padapter->registrypriv.mp_mode == 1))
		fill_txdesc_for_mp(padapter, ptxdesc);
#endif
	else {
		RTW_INFO("pxmitframe->frame_tag = %d\n", pxmitframe->frame_tag);

		SET_TX_DESC_USE_RATE_92E(ptxdesc, 1);
		SET_TX_DESC_TX_RATE_92E(ptxdesc, MRateToHwRate(pmlmeext->tx_rate));
	}

#ifdef CONFIG_ANTENNA_DIVERSITY
	if (!bmcst && pattrib->psta)
		odm_set_tx_ant_by_tx_info(adapter_to_phydm(padapter), ptxdesc, pattrib->psta->cmn.mac_id);
#endif

	rtl8192e_cal_txdesc_chksum(ptxdesc);
	_dbg_dump_tx_info(padapter, pxmitframe->frame_tag, ptxdesc);
	return pull;
}


#ifdef CONFIG_XMIT_THREAD_MODE
/*
 * Description
 *	Transmit xmitbuf to hardware tx fifo
 *
 * Return
 *	_SUCCESS	ok
 *	_FAIL		something error
 */
s32 rtl8192eu_xmit_buf_handler(PADAPTER padapter)
{
	PHAL_DATA_TYPE phal;
	struct xmit_priv *pxmitpriv;
	struct xmit_buf *pxmitbuf;
	struct xmit_frame *pxmitframe;
	s32 ret;


	phal = GET_HAL_DATA(padapter);
	pxmitpriv = &padapter->xmitpriv;

	ret = _rtw_down_sema(&pxmitpriv->xmit_sema);
	if (_FAIL == ret) {
		return _FAIL;
	}

	if (RTW_CANNOT_RUN(padapter)) {
		RTW_DBG(FUNC_ADPT_FMT "- bDriverStopped(%s) bSurpriseRemoved(%s)\n",
			FUNC_ADPT_ARG(padapter),
			rtw_is_drv_stopped(padapter) ? "True" : "False",
			rtw_is_surprise_removed(padapter) ? "True" : "False");
		return _FAIL;
	}

	if (rtw_mi_check_pending_xmitbuf(padapter) == 0)
		return _SUCCESS;

#ifdef CONFIG_LPS_LCLK
	ret = rtw_register_tx_alive(padapter);
	if (ret != _SUCCESS) {
		return _SUCCESS;
	}
#endif

	do {
		pxmitbuf = dequeue_pending_xmitbuf(pxmitpriv);
		if (pxmitbuf == NULL)
			break;
		pxmitframe = (struct xmit_frame *) pxmitbuf->priv_data;
		rtw_write_port(padapter, pxmitbuf->ff_hwaddr, pxmitbuf->len, (unsigned char *)pxmitbuf);
		rtw_free_xmitframe(pxmitpriv, pxmitframe);

	} while (1);

#ifdef CONFIG_LPS_LCLK
	rtw_unregister_tx_alive(padapter);
#endif

	return _SUCCESS;
}
#endif


/* for non-agg data frame or  management frame */
static s32 rtw_dump_xframe(_adapter *padapter, struct xmit_frame *pxmitframe)
{
	s32 ret = _SUCCESS;
	s32 inner_ret = _SUCCESS;
	int t, sz, w_sz, pull = 0;
	u8 *mem_addr;
	u32 ff_hwaddr;
	struct xmit_buf *pxmitbuf = pxmitframe->pxmitbuf;
	struct pkt_attrib *pattrib = &pxmitframe->attrib;
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;
	struct security_priv *psecuritypriv = &padapter->securitypriv;
#ifdef CONFIG_80211N_HT
	if ((pxmitframe->frame_tag == DATA_FRAMETAG) &&
	    (pxmitframe->attrib.ether_type != 0x0806) &&
	    (pxmitframe->attrib.ether_type != 0x888e) &&
	    (pxmitframe->attrib.ether_type != 0x88b4) &&
	    (pxmitframe->attrib.dhcp_pkt != 1))
		rtw_issue_addbareq_cmd(padapter, pxmitframe);
#endif /* CONFIG_80211N_HT */
	mem_addr = pxmitframe->buf_addr;


	for (t = 0; t < pattrib->nr_frags; t++) {
		if (inner_ret != _SUCCESS && ret == _SUCCESS)
			ret = _FAIL;

		if (t != (pattrib->nr_frags - 1)) {

			sz = pxmitpriv->frag_len;
			sz = sz - 4 - (psecuritypriv->sw_encrypt ? 0 : pattrib->icv_len);
		} else /* no frag */
			sz = pattrib->last_txcmdsz;

		pull = update_txdesc(pxmitframe, mem_addr, sz, _FALSE);

		if (pull) {
			mem_addr += PACKET_OFFSET_SZ; /* pull txdesc head */

			/* pxmitbuf->pbuf = mem_addr;			 */
			pxmitframe->buf_addr = mem_addr;

			w_sz = sz + TXDESC_SIZE;
		} else
			w_sz = sz + TXDESC_SIZE + PACKET_OFFSET_SZ;

		ff_hwaddr = rtw_get_ff_hwaddr(pxmitframe);

#ifdef CONFIG_XMIT_THREAD_MODE
		pxmitbuf->len = w_sz;
		pxmitbuf->ff_hwaddr = ff_hwaddr;

		if (pxmitframe->attrib.qsel == QSLT_BEACON)
			/* download rsvd page*/
			rtw_write_port(padapter, ff_hwaddr, w_sz, (u8 *)pxmitbuf);
		else
			enqueue_pending_xmitbuf(pxmitpriv, pxmitbuf);

#else
		inner_ret = rtw_write_port(padapter, ff_hwaddr, w_sz, (unsigned char *)pxmitbuf);
#endif

		rtw_count_tx_stats(padapter, pxmitframe, sz);

		/* RTW_INFO("rtw_write_port, w_sz=%d, sz=%d, txdesc_sz=%d, tid=%d\n", w_sz, sz, w_sz-sz, pattrib->priority);       */

		mem_addr += w_sz;

		mem_addr = (u8 *)RND4(((SIZE_PTR)(mem_addr)));

	}

#ifdef CONFIG_XMIT_THREAD_MODE
	if (pxmitframe->attrib.qsel == QSLT_BEACON)
#endif
	rtw_free_xmitframe(pxmitpriv, pxmitframe);

	if (ret != _SUCCESS)
		rtw_sctx_done_err(&pxmitbuf->sctx, RTW_SCTX_DONE_UNKNOWN);

	return ret;
}

#ifdef CONFIG_USB_TX_AGGREGATION
#define IDEA_CONDITION 1	/* check all packets before enqueue */
s32 rtl8192eu_xmitframe_complete(_adapter *padapter, struct xmit_priv *pxmitpriv, struct xmit_buf *pxmitbuf)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(padapter);
	struct xmit_frame *pxmitframe = NULL;
	struct xmit_frame *pfirstframe = NULL;

	/* aggregate variable */
	struct hw_xmit *phwxmit;
	struct sta_info *psta = NULL;
	struct tx_servq *ptxservq = NULL;

	_irqL irqL;
	_list *xmitframe_plist = NULL, *xmitframe_phead = NULL;

	u32	pbuf;	/* next pkt address */
	u32	pbuf_tail;	/* last pkt tail */
	u32	len;	/* packet length, except TXDESC_SIZE and PKT_OFFSET */

	u32	bulkSize = pHalData->UsbBulkOutSize;
	u8	descCount;
	u32	bulkPtr;

	/* dump frame variable */
	u32 ff_hwaddr;

	_list *sta_plist, *sta_phead;
	u8 single_sta_in_queue = _FALSE;

#ifndef IDEA_CONDITION
	int res = _SUCCESS;
#endif



	/* check xmitbuffer is ok */
	if (pxmitbuf == NULL) {
		pxmitbuf = rtw_alloc_xmitbuf(pxmitpriv);
		if (pxmitbuf == NULL) {
			/* RTW_INFO("%s #1, connot alloc xmitbuf!!!!\n",__FUNCTION__); */
			return _FALSE;
		}
	}

	/* RTW_INFO("%s =====================================\n",__FUNCTION__); */
	/* 3 1. pick up first frame */
	do {
		rtw_free_xmitframe(pxmitpriv, pxmitframe);

		pxmitframe = rtw_dequeue_xframe(pxmitpriv, pxmitpriv->hwxmits, pxmitpriv->hwxmit_entry);
		if (pxmitframe == NULL) {
			/* no more xmit frame, release xmit buffer */
			/* RTW_INFO("no more xmit frame ,return\n"); */
			rtw_free_xmitbuf(pxmitpriv, pxmitbuf);
			return _FALSE;
		}

#ifndef IDEA_CONDITION
		if (pxmitframe->frame_tag != DATA_FRAMETAG) {
			/*			rtw_free_xmitframe(pxmitpriv, pxmitframe); */
			continue;
		}

		/* TID 0~15 */
		if ((pxmitframe->attrib.priority < 0) ||
		    (pxmitframe->attrib.priority > 15)) {
			/*			rtw_free_xmitframe(pxmitpriv, pxmitframe); */
			continue;
		}
#endif
		/* RTW_INFO("==> pxmitframe->attrib.priority:%d\n",pxmitframe->attrib.priority); */
		pxmitframe->pxmitbuf = pxmitbuf;
		pxmitframe->buf_addr = pxmitbuf->pbuf;
		pxmitbuf->priv_data = pxmitframe;

		pxmitframe->agg_num = 1; /* alloc xmitframe should assign to 1. */
#ifdef CONFIG_TX_EARLY_MODE
		pxmitframe->pkt_offset = (PACKET_OFFSET_SZ / 8) + 1; /* 2; */ /* first frame of aggregation, reserve one offset for EM info ,another for usb bulk-out block check */
#else
		pxmitframe->pkt_offset = (PACKET_OFFSET_SZ / 8); /* 1; */ /* first frame of aggregation, reserve offset */
#endif

		if (rtw_xmitframe_coalesce(padapter, pxmitframe->pkt, pxmitframe) == _FALSE) {
			RTW_INFO("%s coalesce 1st xmitframe failed\n", __FUNCTION__);
			continue;
		}


		/* always return ndis_packet after rtw_xmitframe_coalesce */
		rtw_os_xmit_complete(padapter, pxmitframe);

		break;
	} while (1);

	/* 3 2. aggregate same priority and same DA(AP or STA) frames */
	pfirstframe = pxmitframe;
	len = rtw_wlan_pkt_size(pfirstframe) + TXDESC_SIZE + (pfirstframe->pkt_offset * PACKET_OFFSET_SZ);
	pbuf_tail = len;
	pbuf = _RND8(pbuf_tail);

	/* check pkt amount in one bulk */
	descCount = 0;
	bulkPtr = bulkSize;
	if (pbuf < bulkPtr)
		descCount++;
	else {
		descCount = 0;
		bulkPtr = ((pbuf / bulkSize) + 1) * bulkSize; /* round to next bulkSize */
	}

	/* dequeue same priority packet from station tx queue */
	psta = pfirstframe->attrib.psta;
	switch (pfirstframe->attrib.priority) {
	case 1:
	case 2:
		ptxservq = &(psta->sta_xmitpriv.bk_q);
		phwxmit = pxmitpriv->hwxmits + 3;
		break;

	case 4:
	case 5:
		ptxservq = &(psta->sta_xmitpriv.vi_q);
		phwxmit = pxmitpriv->hwxmits + 1;
		break;

	case 6:
	case 7:
		ptxservq = &(psta->sta_xmitpriv.vo_q);
		phwxmit = pxmitpriv->hwxmits;
		break;

	case 0:
	case 3:
	default:
		ptxservq = &(psta->sta_xmitpriv.be_q);
		phwxmit = pxmitpriv->hwxmits + 2;
		break;
	}
	/* RTW_INFO("==> pkt_no=%d,pkt_len=%d,len=%d,RND8_LEN=%d,pkt_offset=0x%02x\n", */
	/* pxmitframe->agg_num,pxmitframe->attrib.last_txcmdsz,len,pbuf,pxmitframe->pkt_offset ); */


	_enter_critical_bh(&pxmitpriv->lock, &irqL);

	sta_phead = get_list_head(phwxmit->sta_queue);
	sta_plist = get_next(sta_phead);
	single_sta_in_queue = rtw_end_of_queue_search(sta_phead, get_next(sta_plist));

	xmitframe_phead = get_list_head(&ptxservq->sta_pending);
	xmitframe_plist = get_next(xmitframe_phead);

	while (rtw_end_of_queue_search(xmitframe_phead, xmitframe_plist) == _FALSE) {
		pxmitframe = LIST_CONTAINOR(xmitframe_plist, struct xmit_frame, list);
		xmitframe_plist = get_next(xmitframe_plist);

		if (_FAIL == rtw_hal_busagg_qsel_check(padapter, pfirstframe->attrib.qsel, pxmitframe->attrib.qsel))
			break;

		pxmitframe->agg_num = 0; /* not first frame of aggregation */
#ifdef CONFIG_TX_EARLY_MODE
		pxmitframe->pkt_offset = 1;/* not first frame of aggregation,reserve offset for EM Info */
#else
		pxmitframe->pkt_offset = 0; /* not first frame of aggregation, no need to reserve offset */
#endif

		len = rtw_wlan_pkt_size(pxmitframe) + TXDESC_SIZE + (pxmitframe->pkt_offset * PACKET_OFFSET_SZ);

		if (_RND8(pbuf + len) > MAX_XMITBUF_SZ)
			/* if (_RND8(pbuf + len) > (MAX_XMITBUF_SZ/2))//to do : for TX TP finial tune , Georgia 2012-0323 */
		{
			/* RTW_INFO("%s....len> MAX_XMITBUF_SZ\n",__FUNCTION__); */
			pxmitframe->agg_num = 1;
			pxmitframe->pkt_offset = 1;
			break;
		}
		rtw_list_delete(&pxmitframe->list);
		ptxservq->qcnt--;
		phwxmit->accnt--;

#ifndef IDEA_CONDITION
		/* suppose only data frames would be in queue */
		if (pxmitframe->frame_tag != DATA_FRAMETAG) {
			rtw_free_xmitframe(pxmitpriv, pxmitframe);
			continue;
		}

		/* TID 0~15 */
		if ((pxmitframe->attrib.priority < 0) ||
		    (pxmitframe->attrib.priority > 15)) {
			rtw_free_xmitframe(pxmitpriv, pxmitframe);
			continue;
		}
#endif

		/*		pxmitframe->pxmitbuf = pxmitbuf; */
		pxmitframe->buf_addr = pxmitbuf->pbuf + pbuf;

		if (rtw_xmitframe_coalesce(padapter, pxmitframe->pkt, pxmitframe) == _FALSE) {
			RTW_INFO("%s coalesce failed\n", __FUNCTION__);
			rtw_free_xmitframe(pxmitpriv, pxmitframe);
			continue;
		}

		/* RTW_INFO("==> pxmitframe->attrib.priority:%d\n",pxmitframe->attrib.priority); */
		/* always return ndis_packet after rtw_xmitframe_coalesce */
		rtw_os_xmit_complete(padapter, pxmitframe);

		/* (len - TXDESC_SIZE) == pxmitframe->attrib.last_txcmdsz */
		update_txdesc(pxmitframe, pxmitframe->buf_addr, pxmitframe->attrib.last_txcmdsz, _TRUE);

		/* don't need xmitframe any more */
		rtw_free_xmitframe(pxmitpriv, pxmitframe);

		/* handle pointer and stop condition */
		pbuf_tail = pbuf + len;
		pbuf = _RND8(pbuf_tail);


		pfirstframe->agg_num++;
#ifdef CONFIG_TX_EARLY_MODE
		pxmitpriv->agg_pkt[pfirstframe->agg_num - 1].offset = _RND8(len);
		pxmitpriv->agg_pkt[pfirstframe->agg_num - 1].pkt_len = pxmitframe->attrib.last_txcmdsz;
#endif
		if (MAX_TX_AGG_PACKET_NUMBER == pfirstframe->agg_num)
			break;

		if (pbuf < bulkPtr) {
			descCount++;
			if (descCount == pHalData->UsbTxAggDescNum)
				break;
		} else {
			descCount = 0;
			bulkPtr = ((pbuf / bulkSize) + 1) * bulkSize;
		}
	} /* end while( aggregate same priority and same DA(AP or STA) frames) */
	if (_rtw_queue_empty(&ptxservq->sta_pending) == _TRUE)
		rtw_list_delete(&ptxservq->tx_pending);
	else if (single_sta_in_queue == _FALSE) {
		/* Re-arrange the order of stations in this ac queue to balance the service for these stations */
		rtw_list_delete(&ptxservq->tx_pending);
		rtw_list_insert_tail(&ptxservq->tx_pending, get_list_head(phwxmit->sta_queue));
	}

	_exit_critical_bh(&pxmitpriv->lock, &irqL);
#ifdef CONFIG_80211N_HT
	if ((pfirstframe->attrib.ether_type != 0x0806) &&
	    (pfirstframe->attrib.ether_type != 0x888e) &&
	    (pfirstframe->attrib.ether_type != 0x88b4) &&
	    (pfirstframe->attrib.dhcp_pkt != 1))
		rtw_issue_addbareq_cmd(padapter, pfirstframe);
#endif /* CONFIG_80211N_HT */
#ifndef CONFIG_USE_USB_BUFFER_ALLOC_TX
	/* 3 3. update first frame txdesc */
	if ((PACKET_OFFSET_SZ != 0) && ((pbuf_tail % bulkSize) == 0)) {
		/* remove pkt_offset */
		pbuf_tail -= PACKET_OFFSET_SZ;
		pfirstframe->buf_addr += PACKET_OFFSET_SZ;
		pfirstframe->pkt_offset--;
		/* RTW_INFO("$$$$$ buf size equal to USB block size $$$$$$\n"); */
	}
#endif /* CONFIG_USE_USB_BUFFER_ALLOC_TX */

	update_txdesc(pfirstframe, pfirstframe->buf_addr, pfirstframe->attrib.last_txcmdsz, _TRUE);

#ifdef CONFIG_TX_EARLY_MODE
	/* prepare EM info for first frame, agg_num value start from 1 */
	pxmitpriv->agg_pkt[0].offset = _RND8(pfirstframe->attrib.last_txcmdsz + TXDESC_SIZE + (pfirstframe->pkt_offset * PACKET_OFFSET_SZ));
	pxmitpriv->agg_pkt[0].pkt_len = pfirstframe->attrib.last_txcmdsz;/* get from rtw_xmitframe_coalesce			 */

	UpdateEarlyModeInfo8192E(pxmitpriv, pxmitbuf);
#endif

	/* 3 4. write xmit buffer to USB FIFO */
	ff_hwaddr = rtw_get_ff_hwaddr(pfirstframe);
	/* RTW_INFO("%s ===================================== write port,buf_size(%d)\n",__FUNCTION__,pbuf_tail); */
	/* xmit address == ((xmit_frame*)pxmitbuf->priv_data)->buf_addr */
#ifdef CONFIG_XMIT_THREAD_MODE
	pxmitbuf->len = pbuf_tail;
	pxmitbuf->ff_hwaddr = ff_hwaddr;

	if (pfirstframe->attrib.qsel == QSLT_BEACON)
		/* download rsvd page or fw */
		rtw_write_port(padapter, ff_hwaddr, pbuf_tail, (u8 *)pxmitbuf);
	else
		enqueue_pending_xmitbuf(pxmitpriv, pxmitbuf);
#else
	rtw_write_port(padapter, ff_hwaddr, pbuf_tail, (u8 *)pxmitbuf);
#endif

	/* 3 5. update statisitc */
	pbuf_tail -= (pfirstframe->agg_num * TXDESC_SIZE);
	pbuf_tail -= (pfirstframe->pkt_offset * PACKET_OFFSET_SZ);


	rtw_count_tx_stats(padapter, pfirstframe, pbuf_tail);

#ifdef CONFIG_XMIT_THREAD_MODE
	if (pfirstframe->attrib.qsel == QSLT_BEACON)
#endif
	rtw_free_xmitframe(pxmitpriv, pfirstframe);

	return _TRUE;
}

#else

s32 rtl8192eu_xmitframe_complete(_adapter *padapter, struct xmit_priv *pxmitpriv, struct xmit_buf *pxmitbuf)
{

	struct hw_xmit *phwxmits;
	sint hwentry;
	struct xmit_frame *pxmitframe = NULL;
	int res = _SUCCESS, xcnt = 0;

	phwxmits = pxmitpriv->hwxmits;
	hwentry = pxmitpriv->hwxmit_entry;


	if (pxmitbuf == NULL) {
		pxmitbuf = rtw_alloc_xmitbuf(pxmitpriv);
		if (!pxmitbuf)
			return _FALSE;
	}


	do {
		pxmitframe =  rtw_dequeue_xframe(pxmitpriv, phwxmits, hwentry);

		if (pxmitframe) {
			pxmitframe->pxmitbuf = pxmitbuf;

			pxmitframe->buf_addr = pxmitbuf->pbuf;

			pxmitbuf->priv_data = pxmitframe;

			if ((pxmitframe->frame_tag & 0x0f) == DATA_FRAMETAG) {
				if (pxmitframe->attrib.priority <= 15) /* TID0~15 */
					res = rtw_xmitframe_coalesce(padapter, pxmitframe->pkt, pxmitframe);
				/* RTW_INFO("==> pxmitframe->attrib.priority:%d\n",pxmitframe->attrib.priority); */
				rtw_os_xmit_complete(padapter, pxmitframe);/* always return ndis_packet after rtw_xmitframe_coalesce			 */
			}




			if (res == _SUCCESS)
				rtw_dump_xframe(padapter, pxmitframe);
			else {
				rtw_free_xmitbuf(pxmitpriv, pxmitbuf);
				rtw_free_xmitframe(pxmitpriv, pxmitframe);
			}

			xcnt++;

		} else {
			rtw_free_xmitbuf(pxmitpriv, pxmitbuf);
			return _FALSE;
		}

		break;

	} while (0/*xcnt < (NR_XMITFRAME >> 3)*/);

	return _TRUE;

}
#endif



static s32 xmitframe_direct(_adapter *padapter, struct xmit_frame *pxmitframe)
{
	s32 res = _SUCCESS;
	/* RTW_INFO("==> %s\n",__FUNCTION__); */

	res = rtw_xmitframe_coalesce(padapter, pxmitframe->pkt, pxmitframe);
	if (res == _SUCCESS)
		rtw_dump_xframe(padapter, pxmitframe);
	else
		RTW_INFO("==> %s xmitframe_coalsece failed\n", __FUNCTION__);

	return res;
}

/*
 * Return
 *	_TRUE	dump packet directly
 *	_FALSE	enqueue packet
 */
static s32 pre_xmitframe(_adapter *padapter, struct xmit_frame *pxmitframe)
{
	_irqL irqL;
	s32 res;
	struct xmit_buf *pxmitbuf = NULL;
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;
	struct pkt_attrib *pattrib = &pxmitframe->attrib;
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;

	_enter_critical_bh(&pxmitpriv->lock, &irqL);

	if (rtw_txframes_sta_ac_pending(padapter, pattrib) > 0) {
		/* RTW_INFO("enqueue AC(%d)\n",pattrib->priority); */
		goto enqueue;
	}

	if (rtw_xmit_ac_blocked(padapter) == _TRUE)
		goto enqueue;

	if (DEV_STA_LG_NUM(padapter->dvobj))
		goto enqueue;

	pxmitbuf = rtw_alloc_xmitbuf(pxmitpriv);
	if (pxmitbuf == NULL)
		goto enqueue;

	_exit_critical_bh(&pxmitpriv->lock, &irqL);

	pxmitframe->pxmitbuf = pxmitbuf;
	pxmitframe->buf_addr = pxmitbuf->pbuf;
	pxmitbuf->priv_data = pxmitframe;

	if (xmitframe_direct(padapter, pxmitframe) != _SUCCESS) {
		rtw_free_xmitbuf(pxmitpriv, pxmitbuf);
		rtw_free_xmitframe(pxmitpriv, pxmitframe);
	}

	return _TRUE;

enqueue:
	res = rtw_xmitframe_enqueue(padapter, pxmitframe);
	_exit_critical_bh(&pxmitpriv->lock, &irqL);

	if (res != _SUCCESS) {
		rtw_free_xmitframe(pxmitpriv, pxmitframe);

		pxmitpriv->tx_drop++;
		return _TRUE;
	}

	return _FALSE;
}

s32 rtl8192eu_mgnt_xmit(_adapter *padapter, struct xmit_frame *pmgntframe)
{
	return rtw_dump_xframe(padapter, pmgntframe);
}

/*
 * Return
 *	_TRUE	dump packet directly ok
 *	_FALSE	temporary can't transmit packets to hardware
 */
s32 rtl8192eu_hal_xmit(_adapter *padapter, struct xmit_frame *pxmitframe)
{
	return pre_xmitframe(padapter, pxmitframe);
}

s32	rtl8192eu_hal_xmitframe_enqueue(_adapter *padapter, struct xmit_frame *pxmitframe)
{
	struct xmit_priv	*pxmitpriv = &padapter->xmitpriv;
	s32 err;

	err = rtw_xmitframe_enqueue(padapter, pxmitframe);
	if (err != _SUCCESS) {
		rtw_free_xmitframe(pxmitpriv, pxmitframe);

		pxmitpriv->tx_drop++;
	} else {
#ifdef PLATFORM_LINUX
		tasklet_hi_schedule(&pxmitpriv->xmit_tasklet);
#endif
	}

	return err;

}

#ifdef CONFIG_HOSTAPD_MLME

static void rtl8192eu_hostap_mgnt_xmit_cb(struct urb *urb)
{
#ifdef PLATFORM_LINUX
	struct sk_buff *skb = (struct sk_buff *)urb->context;

	/* RTW_INFO("%s\n", __FUNCTION__); */

	rtw_skb_free(skb);
#endif
}

s32 rtl8192eu_hostap_mgnt_xmit_entry(_adapter *padapter, _pkt *pkt)
{
#if 0
	/* #ifdef PLATFORM_LINUX */
	u16 fc;
	int rc, len, pipe;
	unsigned int bmcst, tid, qsel;
	struct sk_buff *skb, *pxmit_skb;
	struct urb *urb;
	unsigned char *pxmitbuf;
	struct tx_desc *ptxdesc;
	struct rtw_ieee80211_hdr *tx_hdr;
	struct hostapd_priv *phostapdpriv = padapter->phostapdpriv;
	struct net_device *pnetdev = padapter->pnetdev;
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(padapter);
	struct dvobj_priv *pdvobj = adapter_to_dvobj(padapter);


	/* RTW_INFO("%s\n", __FUNCTION__); */

	skb = pkt;

	len = skb->len;
	tx_hdr = (struct rtw_ieee80211_hdr *)(skb->data);
	fc = le16_to_cpu(tx_hdr->frame_ctl);
	bmcst = IS_MCAST(tx_hdr->addr1);

	if ((fc & RTW_IEEE80211_FCTL_FTYPE) != RTW_IEEE80211_FTYPE_MGMT)
		goto _exit;

	pxmit_skb = rtw_skb_alloc(len + TXDESC_SIZE);

	if (!pxmit_skb)
		goto _exit;

	pxmitbuf = pxmit_skb->data;

	urb = usb_alloc_urb(0, GFP_ATOMIC);
	if (!urb)
		goto _exit;

	/* ----- fill tx desc -----	 */
	ptxdesc = (struct tx_desc *)pxmitbuf;
	_rtw_memset(ptxdesc, 0, sizeof(*ptxdesc));

	/* offset 0	 */
	ptxdesc->txdw0 |= cpu_to_le32(len & 0x0000ffff);
	ptxdesc->txdw0 |= cpu_to_le32(((TXDESC_SIZE + OFFSET_SZ) << OFFSET_SHT) & 0x00ff0000); /* default = 32 bytes for TX Desc */
	ptxdesc->txdw0 |= cpu_to_le32(OWN | FSG | LSG);

	if (bmcst)
		ptxdesc->txdw0 |= cpu_to_le32(BIT(24));

	/* offset 4	 */
	ptxdesc->txdw1 |= cpu_to_le32(0x00);/* MAC_ID */

	ptxdesc->txdw1 |= cpu_to_le32((0x12 << QSEL_SHT) & 0x00001f00);

	ptxdesc->txdw1 |= cpu_to_le32((0x06 << 16) & 0x000f0000); /* b mode */

	/* offset 8			 */

	/* offset 12		 */
	ptxdesc->txdw3 |= cpu_to_le32((le16_to_cpu(tx_hdr->seq_ctl) << 16) & 0xffff0000);

	/* offset 16		 */
	ptxdesc->txdw4 |= cpu_to_le32(BIT(8));/* driver uses rate */

	/* offset 20 */


	/* HW append seq */
	ptxdesc->txdw4 |= cpu_to_le32(BIT(7)); /* Hw set sequence number */
	ptxdesc->txdw3 |= cpu_to_le32((8 << 28)); /* set bit3 to 1. Suugested by TimChen. 2009.12.29. */


	rtl8192e_cal_txdesc_chksum(ptxdesc);
	/* ----- end of fill tx desc ----- */

	/*  */
	skb_put(pxmit_skb, len + TXDESC_SIZE);
	pxmitbuf = pxmitbuf + TXDESC_SIZE;
	_rtw_memcpy(pxmitbuf, skb->data, len);

	/* RTW_INFO("mgnt_xmit, len=%x\n", pxmit_skb->len); */


	/* ----- prepare urb for submit ----- */

	/* translate DMA FIFO addr to pipehandle */
	/* pipe = ffaddr2pipehdl(pdvobj, MGT_QUEUE_INX); */
	pipe = usb_sndbulkpipe(pdvobj->pusbdev, pHalData->Queue2EPNum[(u8)MGT_QUEUE_INX] & 0x0f);

	usb_fill_bulk_urb(urb, pdvobj->pusbdev, pipe,
		pxmit_skb->data, pxmit_skb->len, rtl8192eu_hostap_mgnt_xmit_cb, pxmit_skb);

	urb->transfer_flags |= URB_ZERO_PACKET;
	usb_anchor_urb(urb, &phostapdpriv->anchored);
	rc = usb_submit_urb(urb, GFP_ATOMIC);
	if (rc < 0) {
		usb_unanchor_urb(urb);
		kfree_skb(skb);
	}
	usb_free_urb(urb);


_exit:

	rtw_skb_free(skb);

#endif

	return 0;

}
#endif

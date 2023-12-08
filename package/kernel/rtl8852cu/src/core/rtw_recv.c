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
#define _RTW_RECV_C_

#include <drv_types.h>

static void rtw_signal_stat_timer_hdl(void *ctx);

enum {
	SIGNAL_STAT_CALC_PROFILE_0 = 0,
	SIGNAL_STAT_CALC_PROFILE_1,
	SIGNAL_STAT_CALC_PROFILE_MAX
};

u8 signal_stat_calc_profile[SIGNAL_STAT_CALC_PROFILE_MAX][2] = {
	{4, 1},	/* Profile 0 => pre_stat : curr_stat = 4 : 1 */
	{3, 7}	/* Profile 1 => pre_stat : curr_stat = 3 : 7 */
};

#ifndef RTW_SIGNAL_STATE_CALC_PROFILE
	#define RTW_SIGNAL_STATE_CALC_PROFILE SIGNAL_STAT_CALC_PROFILE_1
#endif

u8 rtw_bridge_tunnel_header[] = { 0xaa, 0xaa, 0x03, 0x00, 0x00, 0xf8 };
u8 rtw_rfc1042_header[] = { 0xaa, 0xaa, 0x03, 0x00, 0x00, 0x00 };
static u8 SNAP_ETH_TYPE_IPX[2] = {0x81, 0x37};
static u8 SNAP_ETH_TYPE_APPLETALK_AARP[2] = {0x80, 0xf3};
#ifdef CONFIG_TDLS
static u8 SNAP_ETH_TYPE_TDLS[2] = {0x89, 0x0d};
#endif

void _rtw_init_sta_recv_priv(struct sta_recv_priv *psta_recvpriv)
{

	_rtw_memset((u8 *)psta_recvpriv, 0, sizeof(struct sta_recv_priv));

	_rtw_spinlock_init(&psta_recvpriv->lock);

	/* for(i=0; i<MAX_RX_NUMBLKS; i++) */
	/*	_rtw_init_queue(&psta_recvpriv->blk_strms[i]); */

	_rtw_init_queue(&psta_recvpriv->defrag_q);


}

u8 rtw_init_recv_info(_adapter *adapter)
{
	u8 ret = _SUCCESS;
	struct recv_info *recvinfo = &adapter->recvinfo;

	recvinfo->sink_udpport = 0;
	recvinfo->pre_rtp_rxseq = 0;
	recvinfo->cur_rtp_rxseq = 0;
#ifdef DBG_RX_SIGNAL_DISPLAY_RAW_DATA
	recvinfo->store_law_data_flag = _TRUE;
#else
	recvinfo->store_law_data_flag = _FALSE;
#endif
#ifdef PRIVATE_R
	_rtw_memset(&(recvinfo->rx_vo_pkt_count_per_data_rate), 0, sizeof(recvinfo->rx_vo_pkt_count_per_data_rate));
	recvinfo->rx_vo_pkt_retry_count = 0;
#endif
	#ifdef CONFIG_SIGNAL_STAT_PROCESS
	rtw_init_timer(&recvinfo->signal_stat_timer, rtw_signal_stat_timer_hdl, adapter);

	recvinfo->signal_stat_sampling_interval = 2000; /* ms */
	/* recvinfo->signal_stat_converging_constant = 5000; */ /* ms */
	#endif
	return ret;
}

/*#define DBG_RECV_FRAME*/
#ifdef DBG_RECV_FRAME
void _dump_recv_priv(struct dvobj_priv *dvobj, _queue *pfree_recv_queue)
{
	struct recv_priv *precvpriv = &dvobj->recvpriv;

	RTW_INFO("%s free_recvframe_cnt:%d\n", __func__, precvpriv->free_recvframe_cnt);
	RTW_INFO("%s dvobj:%p pfree_recv_queue:%p : %p\n",
		__func__, dvobj, &(precvpriv->free_recv_queue), pfree_recv_queue);
}

#endif

sint rtw_init_recv_priv(struct dvobj_priv *dvobj)
{
	sint i;
	union recv_frame *precvframe;
	sint	res = _SUCCESS;
	struct recv_priv *precvpriv = &dvobj->recvpriv;

	#ifdef CONFIG_RECV_THREAD_MODE
	_rtw_init_sema(&precvpriv->recv_sema, 0);
	#endif

	_rtw_init_queue(&precvpriv->free_recv_queue);
	#if 0
	_rtw_init_queue(&precvpriv->uc_swdec_pending_queue);
	#endif

	precvpriv->dvobj = dvobj;

	precvpriv->free_recvframe_cnt = NR_RECVFRAME;

	rtw_os_recv_resource_init(precvpriv);

	precvpriv->pallocated_frame_buf = rtw_zvmalloc(NR_RECVFRAME * sizeof(union recv_frame) + RXFRAME_ALIGN_SZ);

	if (precvpriv->pallocated_frame_buf == NULL) {
		res = _FAIL;
		goto exit;
	}
	/* _rtw_memset(precvpriv->pallocated_frame_buf, 0, NR_RECVFRAME * sizeof(union recv_frame) + RXFRAME_ALIGN_SZ); */

	precvpriv->precv_frame_buf = (u8 *)N_BYTE_ALIGMENT((SIZE_PTR)(precvpriv->pallocated_frame_buf), RXFRAME_ALIGN_SZ);
	/* precvpriv->precv_frame_buf = precvpriv->pallocated_frame_buf + RXFRAME_ALIGN_SZ - */
	/*						((SIZE_PTR) (precvpriv->pallocated_frame_buf) &(RXFRAME_ALIGN_SZ-1)); */

	precvframe = (union recv_frame *) precvpriv->precv_frame_buf;


	for (i = 0; i < NR_RECVFRAME ; i++) {
		_rtw_init_listhead(&(precvframe->u.list));

		rtw_list_insert_tail(&(precvframe->u.list), &(precvpriv->free_recv_queue.queue));

		rtw_os_recv_resource_alloc(precvframe);

		precvframe->u.hdr.len = 0;

		precvframe->u.hdr.dvobj = dvobj;
		precvframe->u.hdr.adapter = NULL;
		precvframe->u.hdr.rx_req = NULL;

		precvframe++;
	}
	#ifdef DBG_RECV_FRAME
	RTW_INFO("%s =>precvpriv->free_recvframe_cnt:%d\n", __func__, precvpriv->free_recvframe_cnt);
	#endif

	res = rtw_intf_init_recv_priv(dvobj);
	#ifdef DBG_RECV_FRAME
	_dump_recv_priv(dvobj, &dvobj->recvpriv.free_recv_queue);
	#endif
exit:
	return res;

}

void rtw_free_recv_priv(struct dvobj_priv *dvobj)
{
	struct recv_priv *precvpriv = &dvobj->recvpriv;

	#if 0
	rtw_free_uc_swdec_pending_queue(dvobj);
	#endif

#ifdef CONFIG_RECV_THREAD_MODE
	_rtw_free_sema(&precvpriv->recv_sema);
#endif

	rtw_os_recv_resource_free(precvpriv);

	if (precvpriv->pallocated_frame_buf)
		rtw_vmfree(precvpriv->pallocated_frame_buf, NR_RECVFRAME * sizeof(union recv_frame) + RXFRAME_ALIGN_SZ);

	_rtw_deinit_queue(&precvpriv->free_recv_queue);

	rtw_intf_free_recv_priv(dvobj);
}

union recv_frame *_rtw_alloc_recvframe(_queue *pfree_recv_queue)
{
	union recv_frame  *precvframe;
	_list	*plist, *phead;
	struct recv_priv *precvpriv;
	struct dvobj_priv *dvobj;

#ifdef DBG_RECV_FRAME
	RTW_INFO("%s =>pfree_recv_queue:%p\n", __func__, pfree_recv_queue);
#endif

	if (_rtw_queue_empty(pfree_recv_queue) == _TRUE) {
		precvframe = NULL;
	}
	else {
		phead = get_list_head(pfree_recv_queue);

		plist = get_next(phead);

		precvframe = LIST_CONTAINOR(plist, union recv_frame, u);
		rtw_list_delete(&precvframe->u.hdr.list);
		dvobj = precvframe->u.hdr.dvobj;
		precvpriv = &dvobj->recvpriv;
		precvpriv->free_recvframe_cnt--;
	}

	return precvframe;

}
union recv_frame *rtw_alloc_recvframe(_queue *pfree_recv_queue)
{
	union recv_frame *precvframe = NULL;
	#ifdef DBG_RECV_FRAME
	struct recv_priv *precvpriv;
	struct dvobj_priv *dvobj;

	RTW_INFO("%s =>pfree_recv_queue:%p\n", __func__, pfree_recv_queue);
	#endif
	_rtw_spinlock_bh(&pfree_recv_queue->lock);

	precvframe = _rtw_alloc_recvframe(pfree_recv_queue);

	_rtw_spinunlock_bh(&pfree_recv_queue->lock);

	if(precvframe) {
		precvframe->u.hdr.rx_req = NULL;
		#ifdef DBG_RECV_FRAME
		dvobj = precvframe->u.hdr.dvobj;
		precvpriv = &dvobj->recvpriv;
		RTW_INFO("%s =>dvobj:%p precvpriv->free_recvframe_cnt:%d\n",
			__func__,
			dvobj,
			precvpriv->free_recvframe_cnt);
		#endif
	}
	return precvframe;
}

void rtw_init_recvframe(union recv_frame *precvframe)
{
	/* Perry: This can be removed */
	_rtw_init_listhead(&precvframe->u.hdr.list);

	precvframe->u.hdr.len = 0;
}

int rtw_free_recvframe(union recv_frame *precvframe)
{
	struct dvobj_priv *dvobj;
	struct recv_priv *precvpriv;
	_queue	*pfree_recv_queue;

	if(!precvframe) {
		RTW_ERR("%s precvframe is NULL\n", __func__);
		rtw_warn_on(1);
		return _FAIL;
	}

	dvobj = precvframe->u.hdr.dvobj;
	precvpriv = &dvobj->recvpriv;
	pfree_recv_queue = &(precvpriv->free_recv_queue);
#ifdef DBG_RECV_FRAME
	RTW_INFO("%s dvobj:%p, phl:%p\n", __func__,dvobj, dvobj->phl);
#endif

#ifdef RTW_PHL_RX
	if(precvframe->u.hdr.rx_req)
		rtw_phl_return_rxbuf(GET_PHL_INFO(dvobj), (u8*)precvframe->u.hdr.rx_req);
#endif

	rtw_os_free_recvframe(precvframe);

	_rtw_spinlock_bh(&pfree_recv_queue->lock);

	rtw_list_delete(&(precvframe->u.hdr.list));

	precvframe->u.hdr.len = 0;

	rtw_list_insert_tail(&(precvframe->u.hdr.list), get_list_head(pfree_recv_queue));
	precvpriv->free_recvframe_cnt++;

	_rtw_spinunlock_bh(&pfree_recv_queue->lock);
#ifdef DBG_RECV_FRAME
	RTW_INFO("%s =>precvpriv->free_recvframe_cnt:%d\n", __func__, precvpriv->free_recvframe_cnt);
#endif

	return _SUCCESS;

}


bool rtw_rframe_del_wfd_ie(union recv_frame *rframe, u8 ies_offset)
{
#define DBG_RFRAME_DEL_WFD_IE 0
	u8 *ies = rframe->u.hdr.rx_data + sizeof(struct rtw_ieee80211_hdr_3addr) + ies_offset;
	uint ies_len_ori = rframe->u.hdr.len - (ies - rframe->u.hdr.rx_data);
	uint ies_len;

	ies_len = rtw_del_wfd_ie(ies, ies_len_ori, DBG_RFRAME_DEL_WFD_IE ? __func__ : NULL);
	rframe->u.hdr.len -= ies_len_ori - ies_len;

	return ies_len_ori != ies_len;
}

#if 0
sint _rtw_enqueue_recvframe(union recv_frame *precvframe, _queue *queue)
{

	_adapter *padapter = precvframe->u.hdr.adapter;
	struct recv_priv *precvpriv = &padapter->recvpriv;


	/* _rtw_init_listhead(&(precvframe->u.hdr.list)); */
	rtw_list_delete(&(precvframe->u.hdr.list));


	rtw_list_insert_tail(&(precvframe->u.hdr.list), get_list_head(queue));

	if (padapter != NULL) {
		if (queue == &precvpriv->free_recv_queue)
			precvpriv->free_recvframe_cnt++;
	}


	return _SUCCESS;
}

sint rtw_enqueue_recvframe(union recv_frame *precvframe, _queue *queue)
{
	sint ret;

	/* _spinlock(&pfree_recv_queue->lock); */
	_rtw_spinlock_bh(&queue->lock);
	ret = _rtw_enqueue_recvframe(precvframe, queue);
	/* _rtw_spinunlock(&pfree_recv_queue->lock); */
	_rtw_spinunlock_bh(&queue->lock);

	return ret;
}
#endif

/*
caller : defrag ; recvframe_chk_defrag in recv_thread  (passive)
pframequeue: defrag_queue : will be accessed in recv_thread  (passive)

using spinlock to protect

*/

void rtw_free_recvframe_queue(_queue *pframequeue)
{
	union	recv_frame	*precvframe;
	_list	*plist, *phead;

	_rtw_spinlock_bh(&pframequeue->lock);

	phead = get_list_head(pframequeue);
	plist = get_next(phead);

	while (rtw_end_of_queue_search(phead, plist) == _FALSE) {
		precvframe = LIST_CONTAINOR(plist, union recv_frame, u);

		plist = get_next(plist);

		/* rtw_list_delete(&precvframe->u.hdr.list); */ /* will do this in rtw_free_recvframe() */

		rtw_free_recvframe(precvframe);
	}

	_rtw_spinunlock_bh(&pframequeue->lock);


}

#if 0
u32 rtw_free_uc_swdec_pending_queue(struct dvobj_priv *dvobj)
{
	u32 cnt = 0;
	union recv_frame *pending_frame;
	while ((pending_frame = rtw_alloc_recvframe(&dvobj->recvpriv.uc_swdec_pending_queue))) {
		rtw_free_recvframe(pending_frame);
		cnt++;
	}

	if (cnt)
		RTW_INFO("dequeue %d\n", cnt);

	return cnt;
}
#endif

sint recvframe_chkmic(_adapter *adapter,  union recv_frame *precvframe)
{

	sint	i, res = _SUCCESS;
	u32	datalen;
	u8	miccode[8];
	u8	bmic_err = _FALSE, brpt_micerror = _TRUE;
	u8	*pframe, *payload, *pframemic;
	u8	*mickey;
	/* u8	*iv,rxdata_key_idx=0; */
	struct _ADAPTER_LINK *adapter_link;
	struct	link_security_priv	*lsecuritypriv;
	struct	sta_info		*stainfo;
	struct	rx_pkt_attrib	*prxattrib = &precvframe->u.hdr.attrib;
	struct	security_priv	*psecuritypriv = &adapter->securitypriv;
	struct	mlme_ext_priv	*pmlmeext = &(adapter->mlmeextpriv);
	struct	mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);

	stainfo = precvframe->u.hdr.psta;

	if (prxattrib->encrypt == _TKIP_) {

		/* calculate mic code */
		if (stainfo != NULL) {
			adapter_link = stainfo->padapter_link;
			lsecuritypriv = &adapter_link->securitypriv;
			if (IS_MCAST(prxattrib->ra)) {
				/* mickey=&psecuritypriv->dot118021XGrprxmickey.skey[0]; */
				/* iv = precvframe->u.hdr.rx_data+prxattrib->hdrlen; */
				/* rxdata_key_idx =( ((iv[3])>>6)&0x3) ; */
				mickey = &lsecuritypriv->dot118021XGrprxmickey[prxattrib->key_index].skey[0];

				/* RTW_INFO("\n recvframe_chkmic: bcmc key psecuritypriv->dot118021XGrpKeyid(%d),pmlmeinfo->key_index(%d) ,recv key_id(%d)\n", */
				/*								psecuritypriv->dot118021XGrpKeyid,pmlmeinfo->key_index,rxdata_key_idx); */

				if (lsecuritypriv->binstallGrpkey == _FALSE)
				{
					res = _FAIL;
					RTW_INFO("\n recvframe_chkmic:didn't install group key!!!!!!!!!!\n");
					goto exit;
				}
			} else {
				mickey = &stainfo->dot11tkiprxmickey.skey[0];
			}

			datalen = precvframe->u.hdr.len - prxattrib->hdrlen - prxattrib->iv_len - prxattrib->icv_len - 8; /* icv_len included the mic code */
			pframe = precvframe->u.hdr.rx_data;
			payload = pframe + prxattrib->hdrlen + prxattrib->iv_len;


			/* rtw_seccalctkipmic(&stainfo->dot11tkiprxmickey.skey[0],pframe,payload, datalen ,&miccode[0],(unsigned char)prxattrib->priority); */ /* care the length of the data */

			rtw_seccalctkipmic(mickey, pframe, payload, datalen , &miccode[0], (unsigned char)prxattrib->priority); /* care the length of the data */

			pframemic = payload + datalen;

			bmic_err = _FALSE;

			for (i = 0; i < 8; i++) {
				if (miccode[i] != *(pframemic + i)) {
					bmic_err = _TRUE;
				}
			}


			if (bmic_err == _TRUE) {



				/* double check key_index for some timing issue , */
				/* cannot compare with psecuritypriv->dot118021XGrpKeyid also cause timing issue */
				if ((IS_MCAST(prxattrib->ra) == _TRUE)  && (prxattrib->key_index != pmlmeinfo->key_index))
					brpt_micerror = _FALSE;

				if ((prxattrib->bdecrypted == _TRUE) && (brpt_micerror == _TRUE)) {
					rtw_handle_tkip_mic_err(adapter, stainfo, (u8)IS_MCAST(prxattrib->ra));
					RTW_INFO(" mic error :prxattrib->bdecrypted=%d\n", prxattrib->bdecrypted);
				} else {
					RTW_INFO(" mic error :prxattrib->bdecrypted=%d\n", prxattrib->bdecrypted);
				}

				res = _FAIL;

			} else {
				/* mic checked ok */
				if ((lsecuritypriv->bcheck_grpkey == _FALSE) && (IS_MCAST(prxattrib->ra) == _TRUE))
				{
					lsecuritypriv->bcheck_grpkey = _TRUE;
				}
			}

		}

		recvframe_pull_tail(precvframe, 8);

	}

exit:


	return res;

}

/*#define DBG_RX_SW_DECRYPTOR*/

/* decrypt and set the ivlen,icvlen of the recv_frame */
union recv_frame *decryptor(_adapter *padapter, union recv_frame *precv_frame)
{

	struct rx_pkt_attrib *prxattrib = &precv_frame->u.hdr.attrib;
	struct security_priv *psecuritypriv = &padapter->securitypriv;
	union recv_frame *return_packet = precv_frame;
	struct sta_info *psta = precv_frame->u.hdr.psta;
	u32  res = _SUCCESS;
	struct _ADAPTER_LINK *padapter_link = precv_frame->u.hdr.adapter_link;
	struct link_security_priv *lsecuritypriv = &padapter_link->securitypriv;


	DBG_COUNTER(padapter->rx_logs.core_rx_post_decrypt);


	if (prxattrib->encrypt > 0) {
		u8 *iv = precv_frame->u.hdr.rx_data + prxattrib->hdrlen;
		prxattrib->key_index = (((iv[3]) >> 6) & 0x3) ;

		if (prxattrib->key_index > WEP_KEYS) {
			RTW_INFO("prxattrib->key_index(%d) > WEP_KEYS\n", prxattrib->key_index);

			switch (prxattrib->encrypt) {
			case _WEP40_:
			case _WEP104_:
				prxattrib->key_index = psecuritypriv->dot11PrivacyKeyIndex;
				break;
			case _TKIP_:
			case _AES_:
			case _GCMP_:
			case _GCMP_256_:
			case _CCMP_256_:
			default:
				prxattrib->key_index = lsecuritypriv->dot118021XGrpKeyid;
				break;
			}
		}
	}

	if (prxattrib->encrypt && !prxattrib->bdecrypted) {
		if (GetFrameType(get_recvframe_data(precv_frame)) == WIFI_DATA
			#ifdef CONFIG_CONCURRENT_MODE
			&& !IS_MCAST(prxattrib->ra) /* bc/mc packets may use sw decryption for concurrent mode */
			#endif
		) {
			if (IS_MCAST(prxattrib->ra))
				psecuritypriv->hw_decrypted = _FALSE;
			else
				psta->hw_decrypted = _FALSE;
		}

#ifdef DBG_RX_SW_DECRYPTOR
		RTW_INFO(ADPT_FMT" - sec_type:%s DO SW decryption\n",
			ADPT_ARG(padapter), security_type_str(prxattrib->encrypt));
#endif

#ifdef DBG_RX_DECRYPTOR
		RTW_INFO("[%s] %d: PKT decrypted(%d), PKT encrypt(%d), Set %pM hw_decrypted(%d)\n",
			 __FUNCTION__,
			 __LINE__,
			 prxattrib->bdecrypted,
			 prxattrib->encrypt,
			 psta->phl_sta->mac_addr,
			 psta->hw_decrypted);
#endif

		switch (prxattrib->encrypt) {
		case _WEP40_:
		case _WEP104_:
			DBG_COUNTER(padapter->rx_logs.core_rx_post_decrypt_wep);
			rtw_wep_decrypt(padapter, (u8 *)precv_frame);
			break;
		case _TKIP_:
			DBG_COUNTER(padapter->rx_logs.core_rx_post_decrypt_tkip);
			res = rtw_tkip_decrypt(padapter, (u8 *)precv_frame);
			break;
		case _AES_:
		case _CCMP_256_:
			DBG_COUNTER(padapter->rx_logs.core_rx_post_decrypt_aes);
			res = rtw_aes_decrypt(padapter, (u8 *)precv_frame);
			break;
		case _GCMP_:
		case _GCMP_256_:
			DBG_COUNTER(padapter->rx_logs.core_rx_post_decrypt_gcmp);
			res = rtw_gcmp_decrypt(padapter, (u8 *)precv_frame);
			break;
#ifdef CONFIG_WAPI_SUPPORT
		case _SMS4_:
			DBG_COUNTER(padapter->rx_logs.core_rx_post_decrypt_wapi);
			rtw_sms4_decrypt(padapter, (u8 *)precv_frame);
			break;
#endif
		default:
			break;
		}
	} else if (prxattrib->bdecrypted == 1
		   && prxattrib->encrypt > 0
		&& (psecuritypriv->busetkipkey == 1 || prxattrib->encrypt != _TKIP_)
		  ) {
#if 0
		if ((prxstat->icv == 1) && (prxattrib->encrypt != _AES_)) {
			psecuritypriv->hw_decrypted = _FALSE;


			rtw_free_recvframe(precv_frame);

			return_packet = NULL;

		} else
#endif
		{
			DBG_COUNTER(padapter->rx_logs.core_rx_post_decrypt_hw);

			psecuritypriv->hw_decrypted = _TRUE;
			psta->hw_decrypted = _TRUE;
#ifdef DBG_RX_DECRYPTOR
			RTW_INFO("[%s] %d: PKT decrypted(%d), PKT encrypt(%d), Set %pM hw_decrypted(%d)\n",
			 __FUNCTION__,
			 __LINE__,
			 prxattrib->bdecrypted,
			 prxattrib->encrypt,
			 psta->phl_sta->mac_addr,
			 psta->hw_decrypted);
#endif
		}
	} else {
		DBG_COUNTER(padapter->rx_logs.core_rx_post_decrypt_unknown);
#ifdef DBG_RX_DECRYPTOR
		RTW_INFO("[%s] %d: PKT decrypted(%d), PKT encrypt(%d), Set %pM hw_decrypted(%d)\n",
			 __FUNCTION__,
			 __LINE__,
			 prxattrib->bdecrypted,
			 prxattrib->encrypt,
			 psta->phl_sta->mac_addr,
			 psta->hw_decrypted);
#endif
	}

	#ifdef CONFIG_RTW_MESH
	if (res != _FAIL
		&& !prxattrib->amsdu
		&& prxattrib->mesh_ctrl_present)
		res = rtw_mesh_rx_validate_mctrl_non_amsdu(padapter, precv_frame);
	#endif

	if (res == _FAIL) {
		/* Let rtw_core_rx_process or rtw_mi_buddy_clone_bcmc_packet */
		/* to handle it.*/
		/* rtw_free_recvframe(return_packet); */
		return_packet = NULL;
	} else
		prxattrib->bdecrypted = _TRUE;
	/* recvframe_chkmic(adapter, precv_frame);   */ /* move to recvframme_defrag function */


	return return_packet;

}

/* ###set the security information in the recv_frame */
union recv_frame *portctrl(_adapter *adapter, union recv_frame *precv_frame)
{
	u8 *psta_addr = NULL;
	u8 *ptr;
	uint  auth_alg;
	struct recv_frame_hdr *pfhdr;
	struct sta_info *psta;
	struct sta_priv *pstapriv ;
	union recv_frame *prtnframe;
	u16	ether_type = 0;
	u16  eapol_type = 0x888e;/* for Funia BD's WPA issue  */
	struct rx_pkt_attrib *pattrib;


	pstapriv = &adapter->stapriv;

	auth_alg = adapter->securitypriv.dot11AuthAlgrthm;

	ptr = get_recvframe_data(precv_frame);
	pfhdr = &precv_frame->u.hdr;
	pattrib = &pfhdr->attrib;
	psta_addr = pattrib->ta;

	prtnframe = NULL;

	psta = precv_frame->u.hdr.psta;


	if (auth_alg == dot11AuthAlgrthm_8021X) {
		if ((psta != NULL) && (psta->ieee8021x_blocked)) {
			/* blocked */
			/* only accept EAPOL frame */

			prtnframe = precv_frame;

			/* get ether_type */
			ptr = ptr + pfhdr->attrib.hdrlen + pfhdr->attrib.iv_len + LLC_HEADER_SIZE;
			_rtw_memcpy(&ether_type, ptr, 2);
			ether_type = ntohs((unsigned short)ether_type);

			if (ether_type == eapol_type)
				prtnframe = precv_frame;
			else {
				/* Let rtw_core_rx_process to handle it */
				/* rtw_free_recvframe(precv_frame); */
				prtnframe = NULL;
			}
		} else {
			/* allowed */
			/* check decryption status, and decrypt the frame if needed */


			prtnframe = precv_frame;
			/* check is the EAPOL frame or not (Rekey) */
			/* if(ether_type == eapol_type){ */
			/* check Rekey */

			/*	prtnframe=precv_frame; */
			/* } */
		}
	} else
		prtnframe = precv_frame;


	return prtnframe;

}

/* VALID_PN_CHK
 * Return true when PN is legal, otherwise false.
 * Legal PN:
 *	1. If old PN is 0, any PN is legal
 *	2. PN > old PN
 */
#define PN_LESS_CHK(a, b)	(((a-b) & 0x800000000000) != 0)
#define VALID_PN_CHK(new, old)	(((old) == 0) || PN_LESS_CHK(old, new))

sint recv_ucast_pn_decache(union recv_frame *precv_frame)
{
	struct rx_pkt_attrib *pattrib = &precv_frame->u.hdr.attrib;
	struct sta_info *sta = precv_frame->u.hdr.psta;
	struct stainfo_rxcache *prxcache = &sta->sta_recvpriv.rxcache;
	u8 *pdata = precv_frame->u.hdr.rx_data;
	sint tid = precv_frame->u.hdr.attrib.priority;
	u64 curr_pn = 0, pkt_pn = 0;
	u8 pn[8] = {0};

	if (tid > 15)
		return _FAIL;

	if (pattrib->encrypt == _TKIP_ || pattrib->encrypt == _AES_ ||
	    pattrib->encrypt == _GCMP_ || pattrib->encrypt == _CCMP_256_ ||
	    pattrib->encrypt == _GCMP_256_) {
		rtw_iv_to_pn((pdata + pattrib->hdrlen), pn, NULL, pattrib->encrypt);
		pkt_pn = RTW_GET_LE64(pn);

		rtw_iv_to_pn(prxcache->iv[tid], pn, NULL, pattrib->encrypt);
		curr_pn = RTW_GET_LE64(pn);

		if (!VALID_PN_CHK(pkt_pn, curr_pn)) {
			/* return _FAIL; */
		} else {
			prxcache->last_tid = tid;
			_rtw_memcpy(prxcache->iv[tid],
				    (pdata + pattrib->hdrlen),
				    sizeof(prxcache->iv[tid]));
		}
	}

	return _SUCCESS;
}

sint recv_bcast_pn_decache(union recv_frame *precv_frame)
{
	_adapter *padapter = precv_frame->u.hdr.adapter;
	struct _ADAPTER_LINK *adapter_link = precv_frame->u.hdr.adapter_link;
	struct link_security_priv *lsecuritypriv = &adapter_link->securitypriv;
	struct rx_pkt_attrib *pattrib = &precv_frame->u.hdr.attrib;
	u8 *pdata = precv_frame->u.hdr.rx_data;
	u64 curr_pn = 0, pkt_pn = 0;
	u8 pn[8] = {0};
	u8 key_id;

	if ((pattrib->encrypt == _TKIP_ || pattrib->encrypt == _AES_ ||
	     pattrib->encrypt == _GCMP_ || pattrib->encrypt == _CCMP_256_ ||
	     pattrib->encrypt == _GCMP_256_) &&
	    (MLME_IS_STA(padapter))) {
		rtw_iv_to_pn((pdata + pattrib->hdrlen), pn, &key_id,
			     pattrib->encrypt);
		pkt_pn = RTW_GET_LE64(pn);
		curr_pn = le64_to_cpu(*(u64*)lsecuritypriv->iv_seq[key_id]);

		if (!VALID_PN_CHK(pkt_pn, curr_pn))
			return _FAIL;

		*(u64*)lsecuritypriv->iv_seq[key_id] = cpu_to_le64(pkt_pn);
	}

	return _SUCCESS;
}

sint recv_decache(union recv_frame *precv_frame)
{
	struct sta_info *psta = precv_frame->u.hdr.psta;
	struct rx_pkt_attrib *pattrib = &precv_frame->u.hdr.attrib;
	_adapter *adapter = psta->padapter;
	sint tid = pattrib->priority;
	u16 seq_ctrl = ((precv_frame->u.hdr.attrib.seq_num & 0xffff) << 4) |
		       (precv_frame->u.hdr.attrib.frag_num & 0xf);
	u16 *prxseq;

	if (tid > 15)
		return _FAIL;

	if (pattrib->qos) {
		if (IS_MCAST(pattrib->ra))
			prxseq = &psta->sta_recvpriv.bmc_tid_rxseq[tid];
		else
			prxseq = &psta->sta_recvpriv.rxcache.tid_rxseq[tid];
	} else {
		if (IS_MCAST(pattrib->ra)) {
			prxseq = &psta->sta_recvpriv.nonqos_bmc_rxseq;
			#ifdef DBG_RX_SEQ
			RTW_INFO("DBG_RX_SEQ "FUNC_ADPT_FMT" nonqos bmc seq_num:%d\n"
				, FUNC_ADPT_ARG(adapter), pattrib->seq_num);
			#endif

		} else {
			prxseq = &psta->sta_recvpriv.nonqos_rxseq;
			#ifdef DBG_RX_SEQ
			RTW_INFO("DBG_RX_SEQ "FUNC_ADPT_FMT" nonqos seq_num:%d\n"
				, FUNC_ADPT_ARG(adapter), pattrib->seq_num);
			#endif
		}
	}

	if (seq_ctrl == *prxseq) {
		/* for non-AMPDU case	*/
		psta->sta_stats.duplicate_cnt++;

		if (psta->sta_stats.duplicate_cnt % 100 == 0)
			RTW_INFO("%s: tid=%u seq=%d frag=%d\n", __func__
				, tid, precv_frame->u.hdr.attrib.seq_num
				, precv_frame->u.hdr.attrib.frag_num);

		#ifdef DBG_RX_DROP_FRAME
		RTW_INFO("DBG_RX_DROP_FRAME "FUNC_ADPT_FMT" recv_decache _FAIL for sta="MAC_FMT"\n"
			, FUNC_ADPT_ARG(adapter), MAC_ARG(psta->phl_sta->mac_addr));
		#endif
		return _FAIL;
	}
	*prxseq = seq_ctrl;

	return _SUCCESS;
}

void process_pwrbit_data(_adapter *padapter, union recv_frame *precv_frame, struct sta_info *psta)
{
#ifdef CONFIG_AP_MODE
	unsigned char pwrbit;
	u8 *ptr = precv_frame->u.hdr.rx_data;

	pwrbit = GetPwrMgt(ptr);

	if (pwrbit) {
		if (!(psta->state & WIFI_SLEEP_STATE)) {
			/* psta->state |= WIFI_SLEEP_STATE; */
			/* rtw_tim_map_set(padapter, pstapriv->sta_dz_bitmap, BIT(psta->phl_sta->aid)); */

			stop_sta_xmit(padapter, psta);
			/* RTW_INFO_DUMP("to sleep, sta_dz_bitmap=", pstapriv->sta_dz_bitmap, pstapriv->aid_bmp_len); */
		}
	} else {
		if (psta->state & WIFI_SLEEP_STATE) {
			/* psta->state ^= WIFI_SLEEP_STATE; */
			/* rtw_tim_map_clear(padapter, pstapriv->sta_dz_bitmap, BIT(psta->phl_sta->aid)); */

			wakeup_sta_to_xmit(padapter, psta);
			/* RTW_INFO_DUMP("to wakeup, sta_dz_bitmap=", pstapriv->sta_dz_bitmap, pstapriv->aid_bmp_len); */
		}
	}
#endif
}

void process_wmmps_data(_adapter *padapter, union recv_frame *precv_frame, struct sta_info *psta)
{
#ifdef CONFIG_AP_MODE
	struct rx_pkt_attrib *pattrib = &precv_frame->u.hdr.attrib;

#ifdef CONFIG_TDLS
	if (!(psta->tdls_sta_state & TDLS_LINKED_STATE)) {
#endif /* CONFIG_TDLS */

		if (!psta->qos_option)
			return;

		if (!(psta->qos_info & 0xf))
			return;

#ifdef CONFIG_TDLS
	}
#endif /* CONFIG_TDLS		 */

	if (psta->state & WIFI_SLEEP_STATE) {
		u8 wmmps_ac = 0;

		switch (pattrib->priority) {
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

		if (wmmps_ac) {
			if (psta->sleepq_ac_len > 0) {
				/* process received triggered frame */
				xmit_delivery_enabled_frames(padapter, psta);
			} else {
				/* issue one qos null frame with More data bit = 0 and the EOSP bit set (=1) */
				issue_qos_nulldata(padapter, precv_frame->u.hdr.adapter_link,
						psta->phl_sta->mac_addr, (u16)pattrib->priority, 0, 0, 0, _FALSE);
			}
		}

	}


#endif

}

#ifdef CONFIG_TDLS
sint OnTDLS(_adapter *adapter, union recv_frame *precv_frame)
{
	struct rx_pkt_attrib	*pattrib = &precv_frame->u.hdr.attrib;
	sint ret = _SUCCESS;
	u8 *paction = get_recvframe_data(precv_frame);
	u8 category_field = 1;
#ifdef CONFIG_WFD
	u8 WFA_OUI[3] = { 0x50, 0x6f, 0x9a };
#endif /* CONFIG_WFD */
	struct tdls_info *ptdlsinfo = &(adapter->tdlsinfo);
	u8 *ptr = precv_frame->u.hdr.rx_data;
	struct sta_priv *pstapriv = &(adapter->stapriv);
	struct sta_info *ptdls_sta = NULL;

	/* point to action field */
	paction += pattrib->hdrlen
		   + pattrib->iv_len
		   + SNAP_SIZE
		   + ETH_TYPE_LEN
		   + PAYLOAD_TYPE_LEN
		   + category_field;

	RTW_INFO("[TDLS] Recv %s from "MAC_FMT" with SeqNum = %d\n", rtw_tdls_action_txt(*paction), MAC_ARG(pattrib->src), GetSequence(get_recvframe_data(precv_frame)));

	if (rtw_hw_chk_wl_func(adapter_to_dvobj(adapter), WL_FUNC_TDLS) == _FALSE) {
		RTW_INFO("Ignore tdls frame since hal doesn't support tdls\n");
		ret = _FAIL;
		return ret;
	}

	if (rtw_is_tdls_enabled(adapter) == _FALSE) {
		RTW_INFO("recv tdls frame, "
			 "but tdls haven't enabled\n");
		ret = _FAIL;
		return ret;
	}

	ptdls_sta = rtw_get_stainfo(pstapriv, get_sa(ptr));
	if (ptdls_sta == NULL) {
		switch (*paction) {
		case TDLS_SETUP_REQUEST:
		case TDLS_DISCOVERY_REQUEST:
			break;
		default:
			RTW_INFO("[TDLS] %s - Direct Link Peer = "MAC_FMT" not found for action = %d\n", __func__, MAC_ARG(get_sa(ptr)), *paction);
			ret = _FAIL;
			goto exit;
		}
	}

	switch (*paction) {
	case TDLS_SETUP_REQUEST:
		ret = On_TDLS_Setup_Req(adapter, precv_frame, ptdls_sta);
		break;
	case TDLS_SETUP_RESPONSE:
		ret = On_TDLS_Setup_Rsp(adapter, precv_frame, ptdls_sta);
		break;
	case TDLS_SETUP_CONFIRM:
		ret = On_TDLS_Setup_Cfm(adapter, precv_frame, ptdls_sta);
		break;
	case TDLS_TEARDOWN:
		ret = On_TDLS_Teardown(adapter, precv_frame, ptdls_sta);
		break;
	case TDLS_DISCOVERY_REQUEST:
		ret = On_TDLS_Dis_Req(adapter, precv_frame);
		break;
	case TDLS_PEER_TRAFFIC_INDICATION:
		ret = On_TDLS_Peer_Traffic_Indication(adapter, precv_frame, ptdls_sta);
		break;
	case TDLS_PEER_TRAFFIC_RESPONSE:
		ret = On_TDLS_Peer_Traffic_Rsp(adapter, precv_frame, ptdls_sta);
		break;
#ifdef CONFIG_TDLS_CH_SW
	case TDLS_CHANNEL_SWITCH_REQUEST:
		ret = On_TDLS_Ch_Switch_Req(adapter, precv_frame, ptdls_sta);
		break;
	case TDLS_CHANNEL_SWITCH_RESPONSE:
		ret = On_TDLS_Ch_Switch_Rsp(adapter, precv_frame, ptdls_sta);
		break;
#endif
#ifdef CONFIG_WFD
	/* First byte of WFA OUI */
	case 0x50:
		if (_rtw_memcmp(WFA_OUI, paction, 3)) {
			/* Probe request frame */
			if (*(paction + 3) == 0x04) {
				/* WFDTDLS: for sigma test, do not setup direct link automatically */
				ptdlsinfo->dev_discovered = _TRUE;
				RTW_INFO("recv tunneled probe request frame\n");
				issue_tunneled_probe_rsp(adapter, precv_frame);
			}
			/* Probe response frame */
			if (*(paction + 3) == 0x05) {
				/* WFDTDLS: for sigma test, do not setup direct link automatically */
				ptdlsinfo->dev_discovered = _TRUE;
				RTW_INFO("recv tunneled probe response frame\n");
			}
		}
		break;
#endif /* CONFIG_WFD */
	default:
		RTW_INFO("receive TDLS frame %d but not support\n", *paction);
		ret = _FAIL;
		break;
	}

exit:
	return ret;

}

sint rtw_tdls_rx_data_validate_hdr(
	_adapter *adapter,
	union recv_frame *precv_frame,
	struct sta_info **psta
)
{
	u8 *ptr = precv_frame->u.hdr.rx_data;
	sint ret = _SUCCESS;
	struct rx_pkt_attrib *pattrib = &precv_frame->u.hdr.attrib;
	struct sta_priv *pstapriv = &adapter->stapriv;
	struct mlme_priv *pmlmepriv = &adapter->mlmepriv;
	u8 *mybssid  = get_bssid(pmlmepriv);
	u8 *myhwaddr = adapter_mac_addr(adapter);
	u8 *sta_addr = pattrib->ta;
	sint bmcast = IS_MCAST(pattrib->dst);
	struct tdls_info *ptdlsinfo = &adapter->tdlsinfo;
#ifdef CONFIG_TDLS_CH_SW
	struct tdls_ch_switch *pchsw_info = &ptdlsinfo->chsw_info;
#endif
	struct sta_info *ptdls_sta = NULL;
	u8 *psnap_type = ptr + pattrib->hdrlen + pattrib->iv_len + SNAP_SIZE;
	/* frame body located after [+2]: ether-type, [+1]: payload type */
	u8 *pframe_body = psnap_type + 2 + 1;
	*psta = ptdls_sta = rtw_get_stainfo(pstapriv, pattrib->ta);
	if (ptdls_sta == NULL) {
		ret = _FAIL;
		goto exit;
	} else if (ptdls_sta->tdls_sta_state & TDLS_LINKED_STATE) {
		/* filter packets that SA is myself or multicast or broadcast */
		if (_rtw_memcmp(myhwaddr, pattrib->src, ETH_ALEN)) {
			ret = _FAIL;
			goto exit;
		}
		/* da should be for me */
		if ((!_rtw_memcmp(myhwaddr, pattrib->dst, ETH_ALEN)) && (!bmcast)) {
			ret = _FAIL;
			goto exit;
		}
		/* check BSSID */
		if (_rtw_memcmp(pattrib->bssid, "\x0\x0\x0\x0\x0\x0", ETH_ALEN) ||
		    _rtw_memcmp(mybssid, "\x0\x0\x0\x0\x0\x0", ETH_ALEN) ||
		    (!_rtw_memcmp(pattrib->bssid, mybssid, ETH_ALEN))) {
			ret = _FAIL;
			goto exit;
		}

#ifdef CONFIG_TDLS_CH_SW
		if (ATOMIC_READ(&pchsw_info->chsw_on) == _TRUE) {
			if (adapter->mlmeextpriv.chandef.chan != rtw_get_oper_ch(adapter)) {
				pchsw_info->ch_sw_state |= TDLS_PEER_AT_OFF_STATE;
				if (!(pchsw_info->ch_sw_state & TDLS_CH_SW_INITIATOR_STATE))
					_cancel_timer_ex(&ptdls_sta->ch_sw_timer);
				/* On_TDLS_Peer_Traffic_Rsp(adapter, precv_frame); */
			}
		}
#endif

		/* process UAPSD tdls sta */
		process_pwrbit_data(adapter, precv_frame, ptdls_sta);

		/* if NULL-frame, check pwrbit */
		if ((get_frame_sub_type(ptr) & WIFI_DATA_NULL) == WIFI_DATA_NULL) {
			/* NULL-frame with pwrbit=1, buffer_STA should buffer frames for sleep_STA */
			if (GetPwrMgt(ptr)) {
				/* it would be triggered when we are off channel and receiving NULL DATA */
				/* we can confirm that peer STA is at off channel */
				RTW_INFO("TDLS: recv peer null frame with pwr bit 1\n");
				/* ptdls_sta->tdls_sta_state|=TDLS_PEER_SLEEP_STATE; */
			}
			/* TODO: Updated BSSID's seq. */
			/* RTW_INFO("drop Null Data\n"); */
			ptdls_sta->tdls_sta_state &= ~(TDLS_WAIT_PTR_STATE);
			ret = _FAIL;
			goto exit;
		}

		#if 0
		/* receive some of all TDLS management frames, process it at ON_TDLS */
		if (_rtw_memcmp(psnap_type, SNAP_ETH_TYPE_TDLS, 2)) {
			ret = OnTDLS(adapter, precv_frame);
			goto exit;
		}
		#endif

		if ((get_frame_sub_type(ptr) & WIFI_QOS_DATA_TYPE) == WIFI_QOS_DATA_TYPE)
			process_wmmps_data(adapter, precv_frame, ptdls_sta);

		ptdls_sta->tdls_sta_state &= ~(TDLS_WAIT_PTR_STATE);

	}

exit:
	return ret;
}
#endif /* CONFIG_TDLS */

void count_rx_stats(_adapter *padapter, union recv_frame *prframe, struct sta_info *sta)
{
	int	sz;
	struct sta_info		*psta = NULL;
	struct stainfo_stats	*pstats = NULL;
	struct rx_pkt_attrib	*pattrib = &prframe->u.hdr.attrib;
	struct recv_info *precvinfo = &padapter->recvinfo;

	sz = get_recvframe_len(prframe);
	precvinfo->rx_bytes += sz;

	padapter->mlmepriv.LinkDetectInfo.NumRxOkInPeriod++;

#ifdef PRIVATE_R
	if(pattrib->priority == 6 || pattrib->priority == 7)
		precvinfo->rx_vo_pkt_count_per_data_rate[pattrib->data_rate]++;
#endif
	if ((!MacAddr_isBcst(pattrib->dst)) && (!IS_MCAST(pattrib->dst)))
		padapter->mlmepriv.LinkDetectInfo.NumRxUnicastOkInPeriod++;

	if (sta)
		psta = sta;
	else
		psta = prframe->u.hdr.psta;

	if (psta) {
		u8 is_ra_bmc = IS_MCAST(pattrib->ra);

		pstats = &psta->sta_stats;

		pstats->last_rx_time = rtw_get_current_time();
		pstats->rx_data_pkts++;
		pstats->rx_bytes += sz;
		if (is_broadcast_mac_addr(pattrib->ra)) {
			pstats->rx_data_bc_pkts++;
			pstats->rx_bc_bytes += sz;
		} else if (is_ra_bmc) {
			pstats->rx_data_mc_pkts++;
			pstats->rx_mc_bytes += sz;
		}

		if (!is_ra_bmc) {
			/*pstats->rxratecnt[pattrib->data_rate]++;*/ /* FIXME overflow */
			/*record rx packets for every tid*/
			pstats->rx_data_qos_pkts[pattrib->priority]++;
		}
	}
}

sint sta2sta_data_frame(
	_adapter *adapter,
	union recv_frame *precv_frame,
	struct sta_info **psta
)
{
	u8 *ptr = precv_frame->u.hdr.rx_data;
	sint ret = _SUCCESS;
	struct rx_pkt_attrib *pattrib = &precv_frame->u.hdr.attrib;
	struct	sta_priv		*pstapriv = &adapter->stapriv;
	struct	mlme_priv	*pmlmepriv = &adapter->mlmepriv;
	u8 *mybssid  = get_bssid(pmlmepriv);
	u8 *myhwaddr = adapter_mac_addr(adapter);
	u8 *sta_addr = pattrib->ta;
	sint bmcast = IS_MCAST(pattrib->dst);

	/* RTW_INFO("[%s] %d, seqnum:%d\n", __FUNCTION__, __LINE__, pattrib->seq_num); */

	if ((check_fwstate(pmlmepriv, WIFI_ADHOC_STATE) == _TRUE) ||
	    (check_fwstate(pmlmepriv, WIFI_ADHOC_MASTER_STATE) == _TRUE)) {

		/* filter packets that SA is myself or multicast or broadcast */
		if (rtw_match_self_addr(adapter, pattrib->src))
		{
			ret = _FAIL;
			goto exit;
		}

		if ((!rtw_match_self_addr(adapter, pattrib->dst)) && (!bmcast))
		{
			ret = _FAIL;
			goto exit;
		}

		if (_rtw_memcmp(pattrib->bssid, "\x0\x0\x0\x0\x0\x0", ETH_ALEN) ||
			rtw_match_bssid(adapter, "\x0\x0\x0\x0\x0\x0") ||
			(!rtw_match_bssid(adapter, pattrib->bssid)))
		{
			ret = _FAIL;
			goto exit;
		}

	} else if (check_fwstate(pmlmepriv, WIFI_MP_STATE) == _TRUE) {
		_rtw_memcpy(pattrib->dst, GetAddr1Ptr(ptr), ETH_ALEN);
		_rtw_memcpy(pattrib->src, get_addr2_ptr(ptr), ETH_ALEN);
		_rtw_memcpy(pattrib->bssid, GetAddr3Ptr(ptr), ETH_ALEN);
		_rtw_memcpy(pattrib->ra, pattrib->dst, ETH_ALEN);
		_rtw_memcpy(pattrib->ta, pattrib->src, ETH_ALEN);

		sta_addr = mybssid;
	} else
		ret  = _FAIL;

	*psta = precv_frame->u.hdr.psta;
	if (*psta == NULL) {
#ifdef CONFIG_MP_INCLUDED
		if (adapter->registrypriv.mp_mode == 1) {
			if (check_fwstate(pmlmepriv, WIFI_MP_STATE) == _TRUE)
				adapter->mppriv.rx_pktloss++;
		}
#endif
		ret = _FAIL;
		goto exit;
	}

exit:
	return ret;

}

sint ap2sta_data_frame(
	_adapter *adapter,
	union recv_frame *precv_frame,
	struct sta_info **psta)
{
	u8 *ptr = precv_frame->u.hdr.rx_data;
	struct rx_pkt_attrib *pattrib = &precv_frame->u.hdr.attrib;
	sint ret = _SUCCESS;
	struct	sta_priv		*pstapriv = &adapter->stapriv;
	struct	mlme_priv	*pmlmepriv = &adapter->mlmepriv;
	u8 *myhwaddr = adapter_mac_addr(adapter);
	sint bmcast = IS_MCAST(pattrib->dst);

	if ((check_fwstate(pmlmepriv, WIFI_MP_STATE) == _TRUE) &&
		   (check_fwstate(pmlmepriv, WIFI_ASOC_STATE) == _TRUE)) {
		_rtw_memcpy(pattrib->dst, GetAddr1Ptr(ptr), ETH_ALEN);
		_rtw_memcpy(pattrib->src, get_addr2_ptr(ptr), ETH_ALEN);
		_rtw_memcpy(pattrib->bssid, GetAddr3Ptr(ptr), ETH_ALEN);
		_rtw_memcpy(pattrib->ra, pattrib->dst, ETH_ALEN);
		_rtw_memcpy(pattrib->ta, pattrib->src, ETH_ALEN);


		*psta = precv_frame->u.hdr.psta; /* get sta_info */
		if (*psta == NULL) {
			#ifdef DBG_RX_DROP_FRAME
			RTW_INFO("DBG_RX_DROP_FRAME "FUNC_ADPT_FMT" can't get psta under WIFI_MP_STATE ; drop pkt\n"
				, FUNC_ADPT_ARG(adapter));
			#endif
			ret = _FAIL;
			goto exit;
		}

	} else {
		if (rtw_match_self_addr(adapter, pattrib->dst) && (!bmcast))
		{
			*psta = precv_frame->u.hdr.psta;
			if (*psta == NULL) {

				/* for AP multicast issue , modify by yiwei */
				static systime send_issue_deauth_time = 0;

				/* RTW_INFO("After send deauth , %u ms has elapsed.\n", rtw_get_passing_time_ms(send_issue_deauth_time)); */

				if (rtw_get_passing_time_ms(send_issue_deauth_time) > 10000 || send_issue_deauth_time == 0) {
					send_issue_deauth_time = rtw_get_current_time();

					RTW_INFO("issue_deauth to the ap=" MAC_FMT " for the reason(7)\n", MAC_ARG(pattrib->bssid));

					issue_deauth(adapter, pattrib->bssid, WLAN_REASON_CLASS3_FRAME_FROM_NONASSOC_STA);
				}
			}
		}

		ret = _FAIL;
		#ifdef DBG_RX_DROP_FRAME
		RTW_INFO("DBG_RX_DROP_FRAME "FUNC_ADPT_FMT" fw_state:0x%x\n"
			, FUNC_ADPT_ARG(adapter), get_fwstate(pmlmepriv));
		#endif
	}

exit:
	return ret;

}

sint sta2ap_data_frame(
	_adapter *adapter,
	union recv_frame *precv_frame,
	struct sta_info **psta)
{
	u8 *ptr = precv_frame->u.hdr.rx_data;
	struct rx_pkt_attrib *pattrib = &precv_frame->u.hdr.attrib;
	struct	sta_priv		*pstapriv = &adapter->stapriv;
	struct	mlme_priv	*pmlmepriv = &adapter->mlmepriv;
	sint ret = _SUCCESS;

	if ((check_fwstate(pmlmepriv, WIFI_MP_STATE) == _TRUE) &&
		   (check_fwstate(pmlmepriv, WIFI_ASOC_STATE) == _TRUE)) {
		/* RTW_INFO("%s ,in WIFI_MP_STATE\n",__func__); */
		_rtw_memcpy(pattrib->dst, GetAddr1Ptr(ptr), ETH_ALEN);
		_rtw_memcpy(pattrib->src, get_addr2_ptr(ptr), ETH_ALEN);
		_rtw_memcpy(pattrib->bssid, GetAddr3Ptr(ptr), ETH_ALEN);
		_rtw_memcpy(pattrib->ra, pattrib->dst, ETH_ALEN);
		_rtw_memcpy(pattrib->ta, pattrib->src, ETH_ALEN);


		*psta = precv_frame->u.hdr.psta; /* get sta_info */
		if (*psta == NULL) {
			#ifdef DBG_RX_DROP_FRAME
			RTW_INFO("DBG_RX_DROP_FRAME "FUNC_ADPT_FMT" can't get psta under WIFI_MP_STATE ; drop pkt\n"
				, FUNC_ADPT_ARG(adapter));
			#endif
			ret = _FAIL;
			goto exit;
		}

	} else {
		u8 *myhwaddr = adapter_mac_addr(adapter);
		if (!rtw_match_self_addr(adapter, pattrib->ra))
		{
			ret = RTW_RX_HANDLED;
			goto exit;
		}
#ifndef CONFIG_CUSTOMER_ALIBABA_GENERAL
		RTW_INFO("issue_deauth to sta=" MAC_FMT " for the reason(7)\n", MAC_ARG(pattrib->src));
		issue_deauth(adapter, pattrib->src, WLAN_REASON_CLASS3_FRAME_FROM_NONASSOC_STA);
#endif
		ret = RTW_RX_HANDLED;
		goto exit;
	}

exit:


	return ret;

}

int rtw_sta_rx_data_validate_hdr(_adapter *adapter, union recv_frame *rframe, struct sta_info **sta)
{
	struct sta_priv *stapriv = &adapter->stapriv;
	u8 *myhwaddr = adapter_mac_addr(adapter);
	struct rx_pkt_attrib *rattrib = &rframe->u.hdr.attrib;
	u8 *whdr = get_recvframe_data(rframe);
	u8 is_ra_bmc = IS_MCAST(GetAddr1Ptr(whdr)) ? 1 : 0;
	sint ret = _FAIL;

	if (rattrib->to_fr_ds == 0) {
		_rtw_memcpy(rattrib->ra, GetAddr1Ptr(whdr), ETH_ALEN);
		_rtw_memcpy(rattrib->ta, get_addr2_ptr(whdr), ETH_ALEN);
		_rtw_memcpy(rattrib->dst, GetAddr1Ptr(whdr), ETH_ALEN);
		_rtw_memcpy(rattrib->src, get_addr2_ptr(whdr), ETH_ALEN);
		_rtw_memcpy(rattrib->bssid, GetAddr3Ptr(whdr), ETH_ALEN);

		#ifdef CONFIG_TDLS
		if (adapter->tdlsinfo.link_established == _TRUE)
			ret = rtw_tdls_rx_data_validate_hdr(adapter, rframe, sta);
		else
		#endif
		{
			/* For Station mode, sa and bssid should always be BSSID, and DA is my mac-address */
			if (!_rtw_memcmp(rattrib->bssid, rattrib->src, ETH_ALEN))
				goto exit;

			if (rattrib->addr_cam_vld)
				*sta = rtw_get_stainfo_by_macid(stapriv, rattrib->macid);
			else
				*sta = rtw_get_stainfo(stapriv, get_addr2_ptr(whdr));
			if (*sta)
				ret = _SUCCESS;
		}
		goto exit;
	}

	if (!(MLME_STATE(adapter) & (WIFI_ASOC_STATE | WIFI_UNDER_LINKING))) {
		if (!is_ra_bmc) {
			/* for AP multicast issue , modify by yiwei */
			static systime send_issue_deauth_time = 0;

			/* RTW_INFO("After send deauth , %u ms has elapsed.\n", rtw_get_passing_time_ms(send_issue_deauth_time)); */
			if (rtw_get_passing_time_ms(send_issue_deauth_time) > 10000 || send_issue_deauth_time == 0) {
				send_issue_deauth_time = rtw_get_current_time();
				RTW_INFO(FUNC_ADPT_FMT" issue_deauth to "MAC_FMT" with reason(7), mlme_state:0x%x\n"
					, FUNC_ADPT_ARG(adapter), MAC_ARG(get_addr2_ptr(whdr)), MLME_STATE(adapter));
				issue_deauth(adapter, get_addr2_ptr(whdr), WLAN_REASON_CLASS3_FRAME_FROM_NONASSOC_STA);
			}
		}
		#ifdef DBG_RX_DROP_FRAME
		RTW_INFO("DBG_RX_DROP_FRAME "FUNC_ADPT_FMT" fw_state:0x%x\n"
			, FUNC_ADPT_ARG(adapter), MLME_STATE(adapter));
		#endif
		goto exit;
	}

	_rtw_memcpy(rattrib->ra, GetAddr1Ptr(whdr), ETH_ALEN);
	_rtw_memcpy(rattrib->ta, get_addr2_ptr(whdr), ETH_ALEN);

	switch (rattrib->to_fr_ds) {
	case 1:
		_rtw_memcpy(rattrib->dst, GetAddr1Ptr(whdr), ETH_ALEN);
		_rtw_memcpy(rattrib->src, GetAddr3Ptr(whdr), ETH_ALEN); /* may change after checking AMSDU subframe header */
		_rtw_memcpy(rattrib->bssid, get_addr2_ptr(whdr), ETH_ALEN);
		break;
	case 3:
		_rtw_memcpy(rattrib->dst, GetAddr3Ptr(whdr), ETH_ALEN); /* may change after checking AMSDU subframe header */
		_rtw_memcpy(rattrib->src, GetAddr4Ptr(whdr), ETH_ALEN); /* may change after checking AMSDU subframe header */
		_rtw_memcpy(rattrib->bssid, get_addr2_ptr(whdr), ETH_ALEN);
		break;
	default:
		ret = RTW_RX_HANDLED; /* don't count for drop */
		goto exit;
	}

	/* filter packets that SA is myself */
	if (!rattrib->amsdu && rtw_match_self_addr(adapter, rattrib->src))
	{
		#ifdef DBG_RX_DROP_FRAME
		RTW_INFO("DBG_RX_DROP_FRAME "FUNC_ADPT_FMT" SA="MAC_FMT", myhwaddr="MAC_FMT"\n"
			, FUNC_ADPT_ARG(adapter), MAC_ARG(rattrib->src), MAC_ARG(myhwaddr));
		#endif
		goto exit;
	}

	if (rattrib->addr_cam_vld)
		*sta = rtw_get_stainfo_by_macid(stapriv, rattrib->macid);
	else
		*sta = rtw_get_stainfo(stapriv, rattrib->ta);
	if (*sta == NULL) {
		#ifndef CONFIG_CUSTOMER_ALIBABA_GENERAL
		if (!is_ra_bmc && !IS_RADAR_DETECTED(adapter_to_rfctl(adapter))) {
			RTW_INFO(FUNC_ADPT_FMT" issue_deauth to "MAC_FMT" with reason(7), unknown TA\n"
				, FUNC_ADPT_ARG(adapter), MAC_ARG(rattrib->ta));
			issue_deauth(adapter, rattrib->ta, WLAN_REASON_CLASS3_FRAME_FROM_NONASSOC_STA);
		}
		#endif
		#ifdef DBG_RX_DROP_FRAME
		RTW_INFO("DBG_RX_DROP_FRAME "FUNC_ADPT_FMT" can't get psta under STATION_MODE ; drop pkt\n"
			, FUNC_ADPT_ARG(adapter));
		#endif
		goto exit;
	}

#ifdef CONFIG_RTW_WDS_AUTO_EN
	if (rattrib->to_fr_ds == 3 && !(sta->flags & WLAN_STA_WDS))
		sta->flags |= WLAN_STA_WDS;
#endif

	/*if ((get_frame_sub_type(whdr) & WIFI_QOS_DATA_TYPE) == WIFI_QOS_DATA_TYPE) {
	}
	*/

	if (get_frame_sub_type(whdr) & BIT(6)) {
		/* No data, will not indicate to upper layer, temporily count it here */
		count_rx_stats(adapter, rframe, *sta);
		ret = RTW_RX_HANDLED;
		goto exit;
	}

#ifdef CONFIG_RTW_WDS
	if (adapter_use_wds(adapter)
		&& !rattrib->amsdu && IS_MCAST(rattrib->dst)
		&& rtw_rx_wds_gptr_check(adapter, rattrib->src)
	) {
		/* will not indicate to upper layer, temporily count it here */
		count_rx_stats(adapter, rframe, *sta);
		ret = RTW_RX_HANDLED;
		goto exit;
	}
#endif

	ret = _SUCCESS;

exit:
	return ret;
}

int rtw_sta_rx_amsdu_act_check(union recv_frame *rframe
	, const u8 *da, const u8 *sa)
{
	int act = RTW_RX_MSDU_ACT_INDICATE;

#ifdef CONFIG_RTW_WDS
	_adapter *adapter = rframe->u.hdr.adapter;

	if (adapter_use_wds(adapter)
		&& IS_MCAST(da)
		&& rtw_rx_wds_gptr_check(adapter, sa)
	) {
		act = 0;
	}
#endif

	return act;
}

#ifdef CONFIG_AP_MODE
sint rtw_proccess_pspoll(_adapter *adapter, union recv_frame *precv_frame, struct sta_info *psta)
{
	u8 *pframe = precv_frame->u.hdr.rx_data;
	struct rx_pkt_attrib *pattrib = &precv_frame->u.hdr.attrib;
	struct sta_priv *pstapriv = &adapter->stapriv;
	u16 aid;
	u8 wmmps_ac = 0;

	aid = GetAid(pframe);
	if (psta->phl_sta->aid != aid)
		return _FAIL;

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
		return _FAIL;

	if (psta->state & WIFI_STA_ALIVE_CHK_STATE) {
		RTW_INFO("%s alive check-rx ps-poll\n", __func__);
		psta->expire_to = pstapriv->expire_to;
		psta->state ^= WIFI_STA_ALIVE_CHK_STATE;
	}

	if ((psta->state & WIFI_SLEEP_STATE) && (rtw_tim_map_is_set(adapter, pstapriv->sta_dz_bitmap, psta->phl_sta->aid))) {
		_list	*xmitframe_plist, *xmitframe_phead;
		struct xmit_frame *pxmitframe = NULL;
		struct xmit_priv *pxmitpriv = &adapter->xmitpriv;
		/* _rtw_spinlock_bh(&psta->sleep_q.lock); */
		_rtw_spinlock_bh(&pxmitpriv->lock);

		xmitframe_phead = get_list_head(&psta->sleep_q);
		xmitframe_plist = get_next(xmitframe_phead);

		if ((rtw_end_of_queue_search(xmitframe_phead, xmitframe_plist)) == _FALSE) {
			pxmitframe = LIST_CONTAINOR(xmitframe_plist, struct xmit_frame, list);

			xmitframe_plist = get_next(xmitframe_plist);
			rtw_list_delete(&pxmitframe->list);

			psta->sleepq_len--;

			if (psta->sleepq_len > 0)
				pxmitframe->attrib.mdata = 1;
			else
				pxmitframe->attrib.mdata = 0;

			pxmitframe->attrib.triggered = 1;

			/* RTW_INFO("handling ps-poll, q_len=%d\n", psta->sleepq_len); */
			/* RTW_INFO_DUMP("handling, tim=", pstapriv->tim_bitmap, pstapriv->aid_bmp_len); */
#if 0
			_rtw_spinunlock_bh(&psta->sleep_q.lock);
			if (rtw_hal_xmit(padapter, pxmitframe) == _TRUE)
				rtw_os_xmit_complete(padapter, pxmitframe);
			_rtw_spinlock_bh(&psta->sleep_q.lock);
#endif
			rtw_intf_xmitframe_enqueue(adapter, pxmitframe);

			if (psta->sleepq_len == 0) {
				rtw_tim_map_clear(adapter, pstapriv->tim_bitmap, psta->phl_sta->aid);
				/* RTW_INFO("after handling ps-poll\n"); */
				/* RTW_INFO_DUMP("after handling, tim=", pstapriv->tim_bitmap, pstapriv->aid_bmp_len); */

				/* upate BCN for TIM IE */
				/* update_BCNTIM(padapter);		 */
				rtw_update_beacon(adapter, psta->padapter_link,
						_TIM_IE_, NULL, _TRUE, 0);
			}
			/* _rtw_spinunlock_bh(&psta->sleep_q.lock); */
			_rtw_spinunlock_bh(&pxmitpriv->lock);
		} else {
			/* _rtw_spinunlock_bh(&psta->sleep_q.lock); */
			_rtw_spinunlock_bh(&pxmitpriv->lock);
			/* RTW_INFO("no buffered packets to xmit\n"); */
			if (rtw_tim_map_is_set(adapter, pstapriv->tim_bitmap, psta->phl_sta->aid)) {
				if (psta->sleepq_len == 0) {
					RTW_INFO("no buffered packets to xmit\n");
					/* issue nulldata with More data bit = 0 to indicate we have no buffered packets */
					issue_nulldata(adapter, psta->padapter_link,
							psta->phl_sta->mac_addr, 0, 0, 0);
				} else {
					RTW_INFO("error!psta->sleepq_len=%d\n", psta->sleepq_len);
					psta->sleepq_len = 0;
				}
				rtw_tim_map_clear(adapter, pstapriv->tim_bitmap, psta->phl_sta->aid);

				/* upate BCN for TIM IE */
				/* update_BCNTIM(padapter); */
				rtw_update_beacon(adapter, psta->padapter_link,
						_TIM_IE_, NULL, _TRUE, 0);
			}
		}
	}
	return _SUCCESS;
}
#endif /*CONFIG_AP_MODE*/
sint validate_recv_ctrl_frame(_adapter *padapter, union recv_frame *precv_frame)
{
	struct rx_pkt_attrib *pattrib = &precv_frame->u.hdr.attrib;
	struct sta_priv *pstapriv = &padapter->stapriv;
	u8 *pframe = precv_frame->u.hdr.rx_data;
	struct sta_info *psta = NULL;
	u8 subtype = 0;
	bool ra_is_self = _FALSE;
	bool ra_is_bc = _FALSE;

	/* uint len = precv_frame->u.hdr.len; */

	/* RTW_INFO("+validate_recv_ctrl_frame\n"); */

	if (GetFrameType(pframe) != WIFI_CTRL_TYPE)
		return _FAIL;

	subtype = get_frame_sub_type(pframe);
	/* receive the frames that ra(a1) is my address */
	ra_is_self = rtw_match_self_addr(padapter, GetAddr1Ptr(pframe));
	ra_is_bc = is_broadcast_mac_addr(GetAddr1Ptr(pframe));

	/*only keep to self or BC(trigger frame)*/
	if(ra_is_self == _FALSE) {
		if(ra_is_bc == _TRUE) {
			if(subtype != WIFI_TRIGGER)
				return _FAIL;
		} else {
			return _FAIL;
		}
	}

	if (pattrib->addr_cam_vld)
		psta = rtw_get_stainfo_by_macid(pstapriv, pattrib->macid);
	else
		psta = rtw_get_stainfo(pstapriv, get_addr2_ptr(pframe));
	if (psta == NULL)
		return _FAIL;

	precv_frame->u.hdr.psta = psta;

	/* for rx pkt statistics */
	psta->sta_stats.last_rx_time = rtw_get_current_time();
	psta->sta_stats.rx_ctrl_pkts++;

	switch (subtype) {
	#ifdef CONFIG_AP_MODE
	case WIFI_PSPOLL :
		{
			sint rst;
			/*only ra(a1) is my address*/
			rst = rtw_proccess_pspoll(padapter, precv_frame, psta);
			/*RTW_INFO(FUNC_ADPT_FMT" pspoll handle %d\n", FUNC_ADPT_ARG(padapter), rst);*/
		}
		break;
	#endif
	case WIFI_BAR :
		/*only ra(a1) is my address*/
		rtw_process_bar_frame(padapter, precv_frame);
		break;
	case WIFI_TRIGGER :
		#ifdef CONFIG_80211AX_HE
		rtw_process_he_triggerframe(padapter, precv_frame);
		#endif
		break;
	default :
		break;
	}
	return _FAIL;

}

#if defined(CONFIG_IEEE80211W) || defined(CONFIG_RTW_MESH)
static sint validate_mgmt_protect(_adapter *adapter, union recv_frame *precv_frame)
{
#define DBG_VALIDATE_MGMT_PROTECT 0
#define DBG_VALIDATE_MGMT_DEC 0

	struct security_priv *sec = &adapter->securitypriv;
	struct rx_pkt_attrib *pattrib = &precv_frame->u.hdr.attrib;
	struct sta_info	*psta = precv_frame->u.hdr.psta;
	u8 *ptr;
	u8 type;
	u8 subtype;
	u8 is_bmc;
	u8 category = 0xFF;
	struct _ADAPTER_LINK *adapter_link = precv_frame->u.hdr.adapter_link;
	struct link_security_priv *lsec = &adapter_link->securitypriv;

#ifdef CONFIG_IEEE80211W
	const u8 *igtk;
	u16 igtk_id;
	u64* ipn;
	enum security_type bip_cipher;
#endif

	u8 *mgmt_DATA;
	u32 data_len = 0;

	sint ret;

#ifdef CONFIG_RTW_MESH
	if (MLME_IS_MESH(adapter)) {
		if (!adapter->mesh_info.mesh_auth_id)
			return pattrib->privacy ? _FAIL : _SUCCESS;
	} else
#endif
	if (SEC_IS_BIP_KEY_INSTALLED(lsec) == _FALSE)
		return _SUCCESS;

	ptr = precv_frame->u.hdr.rx_data;
	type = GetFrameType(ptr);
	subtype = get_frame_sub_type(ptr); /* bit(7)~bit(2) */
	is_bmc = IS_MCAST(GetAddr1Ptr(ptr));

#if DBG_VALIDATE_MGMT_PROTECT
	if (subtype == WIFI_DEAUTH) {
		RTW_INFO(FUNC_ADPT_FMT" bmc:%u, deauth, privacy:%u, encrypt:%u, bdecrypted:%u\n"
			, FUNC_ADPT_ARG(adapter)
			, is_bmc, pattrib->privacy, pattrib->encrypt, pattrib->bdecrypted);
	} else if (subtype == WIFI_DISASSOC) {
		RTW_INFO(FUNC_ADPT_FMT" bmc:%u, disassoc, privacy:%u, encrypt:%u, bdecrypted:%u\n"
			, FUNC_ADPT_ARG(adapter)
			, is_bmc, pattrib->privacy, pattrib->encrypt, pattrib->bdecrypted);
	} if (subtype == WIFI_ACTION) {
		if (pattrib->privacy) {
			RTW_INFO(FUNC_ADPT_FMT" bmc:%u, action(?), privacy:%u, encrypt:%u, bdecrypted:%u\n"
				, FUNC_ADPT_ARG(adapter)
				, is_bmc, pattrib->privacy, pattrib->encrypt, pattrib->bdecrypted);
		} else {
			RTW_INFO(FUNC_ADPT_FMT" bmc:%u, action(%u), privacy:%u, encrypt:%u, bdecrypted:%u\n"
				, FUNC_ADPT_ARG(adapter), is_bmc
				, *(ptr + sizeof(struct rtw_ieee80211_hdr_3addr))
				, pattrib->privacy, pattrib->encrypt, pattrib->bdecrypted);
		}
	}
#endif

	if (!pattrib->privacy) {
		if (!psta || !(psta->flags & WLAN_STA_MFP)) {
			/* peer is not MFP capable, no need to check */
			goto exit;
		}

		if (subtype == WIFI_ACTION)
			category = *(ptr + sizeof(struct rtw_ieee80211_hdr_3addr));

		if (is_bmc) {
			/* broadcast cases */
			if (subtype == WIFI_ACTION) {
				if (CATEGORY_IS_GROUP_PRIVACY(category)) {
					/* drop broadcast group privacy action frame without encryption */
					#if DBG_VALIDATE_MGMT_PROTECT
					RTW_INFO(FUNC_ADPT_FMT" broadcast gp action(%u) w/o encrypt\n"
						, FUNC_ADPT_ARG(adapter), category);
					#endif
					goto fail;
				}
				if (CATEGORY_IS_ROBUST(category)) {
					/* broadcast robust action frame need BIP check */
					goto bip_verify;
				}
			}
			if (subtype == WIFI_DEAUTH || subtype == WIFI_DISASSOC) {
				/* broadcast deauth or disassoc frame need BIP check */
				goto bip_verify;
			}
			goto exit;

		} else {
			/* unicast cases */
			#ifdef CONFIG_IEEE80211W
			if (subtype == WIFI_DEAUTH || subtype == WIFI_DISASSOC) {
				if (!MLME_IS_MESH(adapter)
				#ifdef CONFIG_RTW_WNM
				&& (rtw_wnm_try_btm_roam_imnt(adapter) > 0)
				#endif
				) {
					unsigned short reason = le16_to_cpu(*(unsigned short *)(ptr + WLAN_HDR_A3_LEN));

					#if DBG_VALIDATE_MGMT_PROTECT
					RTW_INFO(FUNC_ADPT_FMT" unicast %s, reason=%d w/o encrypt\n"
						, FUNC_ADPT_ARG(adapter), subtype == WIFI_DEAUTH ? "deauth" : "disassoc", reason);
					#endif
					if (reason == 6 || reason == 7) {
						/* issue sa query request */
						issue_action_SA_Query(adapter, psta->padapter_link,
								psta->phl_sta->mac_addr, 0, 0, IEEE80211W_RIGHT_KEY);
					}
				}
				goto fail;
			}
			#endif

			if (subtype == WIFI_ACTION && CATEGORY_IS_ROBUST(category)) {
				if (psta->bpairwise_key_installed == _TRUE) {
					#if DBG_VALIDATE_MGMT_PROTECT
					RTW_INFO(FUNC_ADPT_FMT" unicast robust action(%d) w/o encrypt\n"
						, FUNC_ADPT_ARG(adapter), category);
					#endif
					goto fail;
				}
			}
			goto exit;
		}

bip_verify:
#ifdef CONFIG_IEEE80211W
		#ifdef CONFIG_RTW_MESH
		if (MLME_IS_MESH(adapter)) {
			if (psta->igtk_bmp) {
				bip_cipher = psta->dot11wCipher;
				igtk = psta->igtk.skey;
				igtk_id = psta->igtk_id;
				ipn = &psta->igtk_pn.val;
			} else {
				/* mesh MFP without IGTK */
				goto exit;
			}
		} else
		#endif
		{
			bip_cipher = sec->dot11wCipher;
			igtk = lsec->dot11wBIPKey[lsec->dot11wBIPKeyid].skey;
			igtk_id = lsec->dot11wBIPKeyid;
			ipn = &lsec->dot11wBIPrxpn.val;
		}

		/* verify BIP MME IE */
		ret = rtw_bip_verify(bip_cipher, pattrib->pkt_len,
			get_recvframe_data(precv_frame),
			get_recvframe_len(precv_frame),
			igtk, igtk_id, ipn, (u8 *)precv_frame);

		if (ret == _FAIL) {
			/* RTW_INFO("802.11w BIP verify fail\n"); */
			goto fail;

		} else if (ret == RTW_RX_HANDLED) {
			#if DBG_VALIDATE_MGMT_PROTECT
			RTW_INFO(FUNC_ADPT_FMT" none protected packet\n", FUNC_ADPT_ARG(adapter));
			#endif
			goto fail;
		}
#endif /* CONFIG_IEEE80211W */
		goto exit;
	}

	if (!psta || !(psta->flags & WLAN_STA_MFP)) {
		/* not peer or peer is not MFP capable, drop it */
		goto fail;
	}

	/* cases to decrypt mgmt frame */
#if 0
	pattrib->bdecrypted = 0;
#endif
#ifdef CONFIG_RTW_MESH
	if (is_bmc)
		pattrib->encrypt = psta->group_privacy;
	else
#endif
	pattrib->encrypt = psta->dot118021XPrivacy;
	pattrib->hdrlen = sizeof(struct rtw_ieee80211_hdr_3addr);

	/* set iv and icv length */
	SET_ICE_IV_LEN(pattrib->iv_len, pattrib->icv_len, pattrib->encrypt);
	_rtw_memcpy(pattrib->ra, GetAddr1Ptr(ptr), ETH_ALEN);
	_rtw_memcpy(pattrib->ta, get_addr2_ptr(ptr), ETH_ALEN);

	/* actual management data frame body */
	data_len = pattrib->pkt_len - pattrib->hdrlen - pattrib->iv_len - pattrib->icv_len;
	mgmt_DATA = rtw_zmalloc(data_len);
	if (mgmt_DATA == NULL) {
		RTW_INFO(FUNC_ADPT_FMT" mgmt allocate fail  !!!!!!!!!\n", FUNC_ADPT_ARG(adapter));
		goto fail;
	}

#if DBG_VALIDATE_MGMT_DEC
	/* dump the packet content before decrypt */
	{
		int pp;

		printk("pattrib->pktlen = %d =>", pattrib->pkt_len);
		for (pp = 0; pp < pattrib->pkt_len; pp++)
		printk(" %02x ", ptr[pp]);
		printk("\n");
	}
#endif

	precv_frame = decryptor(adapter, precv_frame);
	/* save actual management data frame body */
	_rtw_memcpy(mgmt_DATA, ptr + pattrib->hdrlen + pattrib->iv_len, data_len);
	/* overwrite the iv field */
	_rtw_memcpy(ptr + pattrib->hdrlen, mgmt_DATA, data_len);
	/* remove the iv and icv length */
	pattrib->pkt_len = pattrib->pkt_len - pattrib->iv_len - pattrib->icv_len;
	rtw_mfree(mgmt_DATA, data_len);

#if DBG_VALIDATE_MGMT_DEC
	/* print packet content after decryption */
	{
		int pp;

		printk("after decryption pattrib->pktlen = %d @@=>", pattrib->pkt_len);
		for (pp = 0; pp < pattrib->pkt_len; pp++)
		printk(" %02x ", ptr[pp]);
		printk("\n");
	}
#endif

	if (!precv_frame) {
		#if DBG_VALIDATE_MGMT_PROTECT
		RTW_INFO(FUNC_ADPT_FMT" mgmt descrypt fail  !!!!!!!!!\n", FUNC_ADPT_ARG(adapter));
		#endif
		goto fail;
	}

exit:
	return _SUCCESS;

fail:
	return _FAIL;

}
#endif /* defined(CONFIG_IEEE80211W) || defined(CONFIG_RTW_MESH) */

s32 recvframe_chk_defrag(_adapter *padapter, union recv_frame **pprecv_frame);

sint validate_recv_mgnt_frame(_adapter *padapter, union recv_frame *precv_frame)
{
	struct rx_pkt_attrib *pattrib = &precv_frame->u.hdr.attrib;
	struct sta_info *psta;
	if (pattrib->addr_cam_vld)
		psta = rtw_get_stainfo_by_macid(&padapter->stapriv, pattrib->macid);
	else
		psta = rtw_get_stainfo(&padapter->stapriv, get_addr2_ptr(precv_frame->u.hdr.rx_data));

	precv_frame->u.hdr.psta = psta;

#if defined(CONFIG_IEEE80211W) || defined(CONFIG_RTW_MESH)
	if (validate_mgmt_protect(padapter, precv_frame) == _FAIL) {
		DBG_COUNTER(padapter->rx_logs.core_rx_pre_mgmt_err_80211w);
		goto exit;
	}
#endif

	if (recvframe_chk_defrag(padapter, &precv_frame) != CORE_RX_CONTINUE)
		return _SUCCESS;

	/* for rx pkt statistics */
	if (psta) {
		psta->sta_stats.last_rx_time = rtw_get_current_time();
		psta->sta_stats.rx_mgnt_pkts++;
		if (get_frame_sub_type(precv_frame->u.hdr.rx_data) == WIFI_BEACON)
			psta->sta_stats.rx_beacon_pkts++;
		else if (get_frame_sub_type(precv_frame->u.hdr.rx_data) == WIFI_PROBEREQ)
			psta->sta_stats.rx_probereq_pkts++;
		else if (get_frame_sub_type(precv_frame->u.hdr.rx_data) == WIFI_PROBERSP) {
			if (rtw_match_self_addr(padapter, GetAddr1Ptr(precv_frame->u.hdr.rx_data)) == _TRUE)
				psta->sta_stats.rx_probersp_pkts++;
			else if (is_broadcast_mac_addr(GetAddr1Ptr(precv_frame->u.hdr.rx_data))
				|| is_multicast_mac_addr(GetAddr1Ptr(precv_frame->u.hdr.rx_data)))
				psta->sta_stats.rx_probersp_bm_pkts++;
			else
				psta->sta_stats.rx_probersp_uo_pkts++;
		}
	}

	mgt_dispatcher(padapter, precv_frame);

#if defined(CONFIG_IEEE80211W) || defined(CONFIG_RTW_MESH)
exit:
#endif
	return _SUCCESS;

}

sint validate_recv_data_frame(_adapter *adapter, union recv_frame *precv_frame)
{
	u8 bretry, a4_shift;
	struct sta_info *psta = NULL;
	u8 *ptr = precv_frame->u.hdr.rx_data;
	struct rx_pkt_attrib *pattrib = &precv_frame->u.hdr.attrib;
	struct security_priv *psecuritypriv = &adapter->securitypriv;
	struct recv_info *precvinfo = &adapter->recvinfo;
	sint ret = _SUCCESS;
#ifdef PRIVATE_R
	struct stainfo_stats *pstats = NULL;
#endif
#ifdef CONFIG_TDLS
	u8 *psnap_type, *pcategory;
#endif
	bretry = GetRetry(ptr);
	a4_shift = (pattrib->to_fr_ds == 3) ? ETH_ALEN : 0;

	/* some address fields are different when using AMSDU */
	if (pattrib->qos)
		pattrib->amsdu = GetAMsdu(ptr + WLAN_HDR_A3_LEN + a4_shift);
	else
		pattrib->amsdu = 0;

#ifdef CONFIG_RTW_MESH
	if (MLME_IS_MESH(adapter)) {
		ret = rtw_mesh_rx_data_validate_hdr(adapter, precv_frame, &psta);
		goto pre_validate_status_chk;
	} else
#endif
#ifdef CONFIG_AP_MODE
	if (MLME_IS_AP(adapter)) {
		ret = rtw_ap_rx_data_validate_hdr(adapter, precv_frame, &psta);
		goto pre_validate_status_chk;
	} else
#endif
	if (MLME_IS_STA(adapter)) {
		ret = rtw_sta_rx_data_validate_hdr(adapter, precv_frame, &psta);
		goto pre_validate_status_chk;
	}

	switch (pattrib->to_fr_ds) {
	case 0:
		_rtw_memcpy(pattrib->ra, GetAddr1Ptr(ptr), ETH_ALEN);
		_rtw_memcpy(pattrib->ta, get_addr2_ptr(ptr), ETH_ALEN);
		_rtw_memcpy(pattrib->dst, GetAddr1Ptr(ptr), ETH_ALEN);
		_rtw_memcpy(pattrib->src, get_addr2_ptr(ptr), ETH_ALEN);
		_rtw_memcpy(pattrib->bssid, GetAddr3Ptr(ptr), ETH_ALEN);
		ret = sta2sta_data_frame(adapter, precv_frame, &psta);
		break;

	case 1:
		_rtw_memcpy(pattrib->ra, GetAddr1Ptr(ptr), ETH_ALEN);
		_rtw_memcpy(pattrib->ta, get_addr2_ptr(ptr), ETH_ALEN);
		_rtw_memcpy(pattrib->dst, GetAddr1Ptr(ptr), ETH_ALEN);
		_rtw_memcpy(pattrib->src, GetAddr3Ptr(ptr), ETH_ALEN);
		_rtw_memcpy(pattrib->bssid, get_addr2_ptr(ptr), ETH_ALEN);
		ret = ap2sta_data_frame(adapter, precv_frame, &psta);
		break;

	case 2:
		_rtw_memcpy(pattrib->ra, GetAddr1Ptr(ptr), ETH_ALEN);
		_rtw_memcpy(pattrib->ta, get_addr2_ptr(ptr), ETH_ALEN);
		_rtw_memcpy(pattrib->dst, GetAddr3Ptr(ptr), ETH_ALEN);
		_rtw_memcpy(pattrib->src, get_addr2_ptr(ptr), ETH_ALEN);
		_rtw_memcpy(pattrib->bssid, GetAddr1Ptr(ptr), ETH_ALEN);
		ret = sta2ap_data_frame(adapter, precv_frame, &psta);
		break;

	case 3:
	default:
		/* WDS is not supported */
		ret = _FAIL;
		break;
	}

pre_validate_status_chk:

#ifdef PRIVATE_R
	if(psta!=NULL && bretry){
		pstats = &psta->sta_stats;
		ATOMIC_ADD((ATOMIC_T *)&pstats->rx_retry_cnt, 1);
	}
#endif

	if (ret == _FAIL) {
		#ifdef DBG_RX_DROP_FRAME
		RTW_INFO("DBG_RX_DROP_FRAME "FUNC_ADPT_FMT" case:%d, res:%d, ra="MAC_FMT", ta="MAC_FMT"\n"
			, FUNC_ADPT_ARG(adapter), pattrib->to_fr_ds, ret, MAC_ARG(GetAddr1Ptr(ptr)), MAC_ARG(get_addr2_ptr(ptr)));
		#endif
		goto exit;
	} else if (ret == RTW_RX_HANDLED)
		goto exit;


	if (psta == NULL) {
		#ifdef DBG_RX_DROP_FRAME
		RTW_INFO("DBG_RX_DROP_FRAME "FUNC_ADPT_FMT" psta == NULL, ra="MAC_FMT", ta="MAC_FMT"\n"
			, FUNC_ADPT_ARG(adapter), MAC_ARG(GetAddr1Ptr(ptr)), MAC_ARG(get_addr2_ptr(ptr)));
		#endif
		ret = _FAIL;
		goto exit;
	}

	if ((psta->flags & WLAN_STA_AMSDU_DISABLE) && pattrib->amsdu) {
	#ifdef DBG_RX_DROP_FRAME
		RTW_INFO("DBG_RX_DROP_FRAME "FUNC_ADPT_FMT" amsdu not allowed"MAC_FMT"\n"
			, FUNC_ADPT_ARG(adapter), MAC_ARG(psta->cmn.mac_addr));
	#endif
		ret = _FAIL;
		goto exit;

	}

	precv_frame->u.hdr.psta = psta;
#ifdef CONFIG_RECV_REORDERING_CTRL
	precv_frame->u.hdr.preorder_ctrl = NULL;
#endif
	pattrib->ack_policy = 0;

	/* parsing QC field */
	if (pattrib->qos == 1) {
		pattrib->priority = GetPriority((ptr + WLAN_HDR_A3_LEN + a4_shift)); /* point to Qos field*/
		pattrib->ack_policy = GetAckpolicy((ptr + WLAN_HDR_A3_LEN + a4_shift));
		pattrib->hdrlen = WLAN_HDR_A3_QOS_LEN + a4_shift;
	} else {
		pattrib->priority = 0;
		pattrib->hdrlen = WLAN_HDR_A3_LEN + a4_shift;
	}

	if (pattrib->order) /* HT-CTRL 11n */
		pattrib->hdrlen += 4;

	/* decache, drop duplicate recv packets */
	ret = recv_decache(precv_frame);
	if (ret  == _FAIL)
		goto exit;

#ifdef PRIVATE_R
	if (bretry && (pattrib->priority == 6 || pattrib->priority == 7))
		precvinfo->rx_vo_pkt_retry_count++;
#endif
	if (!IS_MCAST(pattrib->ra)) {
#ifdef CONFIG_RECV_REORDERING_CTRL
		if (pattrib->qos)
			precv_frame->u.hdr.preorder_ctrl = &psta->recvreorder_ctrl[pattrib->priority];
#endif
		if (recv_ucast_pn_decache(precv_frame) == _FAIL) {
			#ifdef DBG_RX_DROP_FRAME
			RTW_INFO("DBG_RX_DROP_FRAME "FUNC_ADPT_FMT" recv_ucast_pn_decache return _FAIL for sta="MAC_FMT"\n"
				, FUNC_ADPT_ARG(adapter), MAC_ARG(psta->phl_sta->mac_addr));
			#endif
			ret = _FAIL;
			goto exit;
		}
	} else {
		if (recv_bcast_pn_decache(precv_frame) == _FAIL) {
			#ifdef DBG_RX_DROP_FRAME
			RTW_INFO("DBG_RX_DROP_FRAME "FUNC_ADPT_FMT" recv_bcast_pn_decache return _FAIL for sta="MAC_FMT"\n"
				, FUNC_ADPT_ARG(adapter), MAC_ARG(psta->phl_sta->mac_addr));
			#endif
			ret = _FAIL;
			goto exit;
		}
	}

	if (pattrib->privacy) {
#ifdef CONFIG_TDLS
		if ((psta->tdls_sta_state & TDLS_LINKED_STATE) && (psta->dot118021XPrivacy == _AES_))
			pattrib->encrypt = psta->dot118021XPrivacy;
		else
#endif /* CONFIG_TDLS */
			GET_ENCRY_ALGO(psecuritypriv, psta, pattrib->encrypt, IS_MCAST(pattrib->ra));


		SET_ICE_IV_LEN(pattrib->iv_len, pattrib->icv_len, pattrib->encrypt);
	} else {
		pattrib->encrypt = 0;
		pattrib->iv_len = pattrib->icv_len = 0;
	}

	/* drop unprotected frame in protected network. */
	if (psecuritypriv->dot11PrivacyAlgrthm != _NO_PRIVACY_ ) {
		if (IS_MCAST(pattrib->ra)) {
			if (!pattrib->privacy) {
				#ifdef DBG_RX_DROP_FRAME
				RTW_INFO("DBG_RX_DROP_FRAME "FUNC_ADPT_FMT"recv plaintext bmc packet for sta="MAC_FMT"\n"
					, FUNC_ADPT_ARG(adapter), MAC_ARG(psta->cmn.mac_addr));
				#endif
				ret = _FAIL;
				goto exit;
			}
		} else {
			/* unicast */
			u16 ether_type;
			u8* ether_ptr = NULL;
			u16  eapol_type = 0x888e;
			ether_ptr = ptr + pattrib->hdrlen + pattrib->iv_len + RATTRIB_GET_MCTRL_LEN(pattrib) + LLC_HEADER_SIZE;
			_rtw_memcpy(&ether_type, ether_ptr, 2);
			ether_type = ntohs((unsigned short)ether_type);

			if (psecuritypriv->dot11AuthAlgrthm == dot11AuthAlgrthm_8021X) {
			/* CVE-2020-26140, CVE-2020-26143, CVE-2020-26147,  let eapol packet go through*/
				if (!pattrib->privacy && ether_type != eapol_type ) {
					#ifdef DBG_RX_DROP_FRAME
					RTW_INFO("DBG_RX_DROP_FRAME "FUNC_ADPT_FMT"recv plaintext unicast packet for sta="MAC_FMT"\n"
						, FUNC_ADPT_ARG(adapter), MAC_ARG(psta->cmn.mac_addr));
					#endif
					ret = _FAIL;
					goto exit;
				}
				/* CVE-2020-26144, pevernt plaintext A-MSDU */
				/* This can prevent plantext A-MSDU cloacked as an EAPOL frame */
				if (!pattrib->privacy && pattrib->amsdu) {
					#ifdef DBG_RX_DROP_FRAME
					RTW_INFO("DBG_RX_DROP_FRAME "FUNC_ADPT_FMT"recv plaintext A-MSDU for sta="MAC_FMT"\n"
						, FUNC_ADPT_ARG(adapter), MAC_ARG(psta->cmn.mac_addr));
					#endif
					ret = _FAIL;
					goto exit;
				}
				/* CVE-2020-26139,  Drop any forwarding eapol packet until 4-way has done.	*/
				if ((ether_type == eapol_type)
					&& (MLME_IS_AP(adapter) || MLME_IS_MESH(adapter))
					&& (psta->dot118021XPrivacy == _NO_PRIVACY_)
					&& (!rtw_match_self_addr(adapter, pattrib->dst))
				) {
					#ifdef DBG_RX_DROP_FRAME
					RTW_INFO("DBG_RX_DROP_FRAME "FUNC_ADPT_FMT" recv eapol packet forwarding(dst:"MAC_FMT") before 4-way finish.\n"
						, FUNC_ADPT_ARG(adapter), MAC_ARG(pattrib->dst));
					#endif
					ret = _FAIL;
					goto exit;
				}
			} else {
				/* CVE-2020-26140, CVE-2020-26143, CVE-2020-26147 */
				if (!pattrib->privacy) {
					#ifdef DBG_RX_DROP_FRAME
					RTW_INFO("DBG_RX_DROP_FRAME "FUNC_ADPT_FMT"recv plaintext packet for sta="MAC_FMT"\n"
						, FUNC_ADPT_ARG(adapter), MAC_ARG(psta->cmn.mac_addr));
					#endif
					ret = _FAIL;
					goto exit;
				}
		 	}
		}
	}

#ifdef CONFIG_RTW_MESH
	if (!pattrib->amsdu
		&& pattrib->mesh_ctrl_present
		&& (!pattrib->encrypt || pattrib->bdecrypted))
		ret = rtw_mesh_rx_validate_mctrl_non_amsdu(adapter, precv_frame);
#endif

#ifdef CONFIG_TDLS
	/* check TDLS frame */
	psnap_type = get_recvframe_data(precv_frame) + pattrib->hdrlen + pattrib->iv_len + SNAP_SIZE ;
	pcategory = psnap_type + ETH_TYPE_LEN + PAYLOAD_TYPE_LEN;
	if ((_rtw_memcmp(psnap_type, SNAP_ETH_TYPE_TDLS, ETH_TYPE_LEN)) &&
		((*pcategory == RTW_WLAN_CATEGORY_TDLS) || (*pcategory == RTW_WLAN_CATEGORY_P2P))) {
		ret = OnTDLS(adapter, precv_frame);
		if (ret == _FAIL)
			goto exit;
	}
#endif /* CONFIG_TDLS */

exit:

#ifdef RTW_PHL_TEST_FPGA
	return _SUCCESS;
#endif
	return ret;
}

static inline void dump_rx_packet(u8 *ptr)
{
	int i;

	RTW_INFO("#############################\n");
	for (i = 0; i < 64; i = i + 8)
		RTW_INFO("%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:\n", *(ptr + i),
			*(ptr + i + 1), *(ptr + i + 2) , *(ptr + i + 3) , *(ptr + i + 4), *(ptr + i + 5), *(ptr + i + 6), *(ptr + i + 7));
	RTW_INFO("#############################\n");
}

#ifdef DBG_RX_DFRAME_RAW_DATA
static void rx_process_dframe_raw_data(union recv_frame *precvframe)
{
	_adapter *padapter = precvframe->u.hdr.adapter;
	struct dvobj_priv *devob = adapter_to_dvobj(padapter);
	struct sta_recv_dframe_info *psta_dframe_info;
	struct sta_info *psta = precvframe->u.hdr.psta;
	struct rx_pkt_attrib *pattrib = &precvframe->u.hdr.attrib;
	struct mlme_ext_priv *pmlmeext = &(padapter->mlmeextpriv);
	struct mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);
	struct recv_info *recvinfo = &padapter->recvinfo;
	u8 isCCKrate, rf_path, dframe_type;
	u8 *ptr;
	u8 bc_addr[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	ptr = precvframe->u.hdr.rx_data;
	dframe_type = GetFrameType(ptr);

	if (recvinfo->store_law_data_flag) {
		if ((dframe_type == WIFI_DATA_TYPE) || (dframe_type == WIFI_QOS_DATA_TYPE) || (padapter->registrypriv.mp_mode == 1)) {
			if (psta) {
				if (IS_MCAST(get_ra(get_recvframe_data(precvframe)))) {
					psta_dframe_info = &psta->sta_dframe_info_bmc;
				} else {
					psta_dframe_info = &psta->sta_dframe_info;
				}

				if ((_rtw_memcmp(get_my_bssid(&pmlmeinfo->dev_network), bc_addr, ETH_ALEN) != _TRUE) || (padapter->registrypriv.mp_mode == 1)) {
					psta_dframe_info->sta_data_rate = pattrib->data_rate;
					psta_dframe_info->sta_sgi = pattrib->sgi;
					psta_dframe_info->sta_bw_mode = pattrib->bw;
					isCCKrate = (pattrib->data_rate <= DESC_RATE11M) ? _TRUE : _FALSE;
					for (rf_path = 0; rf_path < GET_HAL_RFPATH_NUM(devob); rf_path++) {
						psta_dframe_info->sta_mimo_signal_strength[rf_path] = pattrib->phy_info.signal_strength;
						if (!isCCKrate) {
							psta_dframe_info->sta_ofdm_snr[rf_path] = pattrib->phy_info.snr_fd[rf_path];
							psta_dframe_info->sta_RxPwr[rf_path] = rtw_phl_rssi_to_dbm(pattrib->phy_info.rx_pwr[rf_path]);;
						}
					}
				}
			}
		}
	}
}
#endif /* DBG_RX_DFRAME_RAW_DATA */

#ifdef CONFIG_SNR_RPT
static void rx_process_snr_info(union recv_frame *precvframe)
{
	_adapter *padapter = precvframe->u.hdr.adapter;
	struct rx_pkt_attrib *pattrib = &precvframe->u.hdr.attrib;
	struct phydm_phyinfo_struct *phy_info = &pattrib->phy_info;
	u8 *wlanhdr = NULL;
	u8 *ta, *ra;
	u8 is_ra_bmc;
	struct sta_priv *pstapriv;
	struct sta_info *psta = NULL;
	int i;

	wlanhdr = precvframe->u.hdr.rx_data;
	ta = get_ta(wlanhdr);
	pstapriv = &padapter->stapriv;
	psta = precvframe->u.hdr.psta;

	if (psta) {
		_rtw_spinlock_bh(&psta->lock);
		if (phy_info->is_valid) {
			psta->snr_num++;
			for ( i = 0; i < RTW_PHL_MAX_RF_PATH; i++) {
				psta->snr_fd_total[i] += pattrib->phy_info.snr_fd[i];
				psta->snr_td_total[i] += pattrib->phy_info.snr_td[i];
				psta->snr_fd_avg[i] = psta->snr_fd_total[i]/psta->snr_num;
				psta->snr_td_avg[i] = psta->snr_td_total[i]/psta->snr_num;
				#if 0
				RTW_INFO("path = %d, AVG_SNR_FD = %d, AVG_SNR_TD = %d\n",
				i, psta->snr_fd_avg[i], psta->snr_td_avg[i]);
				#endif
			}

		}
		_rtw_spinunlock_bh(&psta->lock);
	}

}
#endif /* CONFIG_SNR_RPT */
sint validate_recv_frame(_adapter *adapter, union recv_frame *precv_frame)
{
	/* shall check frame subtype, to / from ds, da, bssid */

	/* then call check if rx seq/frag. duplicated. */

	u8 type;
	u8 subtype;
	sint retval = _SUCCESS;

	struct rx_pkt_attrib *pattrib = &precv_frame->u.hdr.attrib;
	struct recv_info *precvinfo = &adapter->recvinfo;

	u8 *ptr = precv_frame->u.hdr.rx_data;
	u8  ver = (unsigned char)(*ptr) & 0x3 ;
#ifdef CONFIG_FIND_BEST_CHANNEL
	struct rtw_chset *chset = adapter_to_chset(adapter);
	struct mlme_ext_priv *pmlmeext = &adapter->mlmeextpriv;
#endif

#ifdef CONFIG_TDLS
	struct tdls_info *ptdlsinfo = &adapter->tdlsinfo;
#endif /* CONFIG_TDLS */
#ifdef CONFIG_WAPI_SUPPORT
	PRT_WAPI_T	pWapiInfo = &adapter->wapiInfo;
	struct recv_frame_hdr *phdr = &precv_frame->u.hdr;
	u8 wai_pkt = 0;
	u16 sc;
	u8 external_len = 0;
#endif
	struct _ADAPTER_LINK *adapter_link = precv_frame->u.hdr.adapter_link;


#ifdef CONFIG_FIND_BEST_CHANNEL
	if (pmlmeext->sitesurvey_res.state == SCAN_PROCESS) {
		int ch_set_idx = rtw_chset_search_ch(chset, rtw_get_oper_ch(adapter, adapter_link));
		if (ch_set_idx >= 0)
			chset->chs[ch_set_idx].rx_count++;
	}
#endif

#ifdef CONFIG_TDLS
	if (ptdlsinfo->ch_sensing == 1 && ptdlsinfo->cur_channel != 0)
		ptdlsinfo->collect_pkt_num[ptdlsinfo->cur_channel - 1]++;
#endif /* CONFIG_TDLS */

#ifdef RTK_DMP_PLATFORM
	if (0) {
		RTW_INFO("++\n");
		{
			int i;
			for (i = 0; i < 64; i = i + 8)
				RTW_INFO("%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:", *(ptr + i),
					*(ptr + i + 1), *(ptr + i + 2) , *(ptr + i + 3) , *(ptr + i + 4), *(ptr + i + 5), *(ptr + i + 6), *(ptr + i + 7));

		}
		RTW_INFO("--\n");
	}
#endif /* RTK_DMP_PLATFORM */

	/* add version chk */
	if (ver != 0) {
		retval = _FAIL;
		DBG_COUNTER(adapter->rx_logs.core_rx_pre_ver_err);
		goto exit;
	}

	type =  GetFrameType(ptr);
	subtype = get_frame_sub_type(ptr); /* bit(7)~bit(2) */

	pattrib->to_fr_ds = get_tofr_ds(ptr);

	pattrib->frag_num = GetFragNum(ptr);
	pattrib->seq_num = GetSequence(ptr);

	pattrib->pw_save = GetPwrMgt(ptr);
	pattrib->mfrag = GetMFrag(ptr);
	pattrib->mdata = GetMData(ptr);
	pattrib->privacy = GetPrivacy(ptr);
	pattrib->order = GetOrder(ptr);
#ifdef CONFIG_WAPI_SUPPORT
	sc = (pattrib->seq_num << 4) | pattrib->frag_num;
#endif

#ifdef RTW_PHL_DBG_CMD
	pattrib->wl_type = type;
	pattrib->wl_subtype = subtype;

	if(type == WIFI_DATA_TYPE)
		core_add_record(adapter, REC_RX_DATA, precv_frame);
	else
		core_add_record(adapter, REC_RX_MGMT, precv_frame);
#endif


#if 1 /* Dump rx packets */
	{
		u8 bDumpRxPkt = 0;

		rtw_hal_get_def_var(adapter, adapter_link, HAL_DEF_DBG_DUMP_RXPKT, &(bDumpRxPkt));
		if (bDumpRxPkt == 1) /* dump all rx packets */
			dump_rx_packet(ptr);
		else if ((bDumpRxPkt == 2) && (type == WIFI_MGT_TYPE))
			dump_rx_packet(ptr);
		else if ((bDumpRxPkt == 3) && (type == WIFI_DATA_TYPE))
			dump_rx_packet(ptr);
	}
#endif
#ifdef RTW_WKARD_CORE_RSSI_V1
	rx_process_phy_info(precv_frame);
#endif

#ifdef CONFIG_SNR_RPT
	rx_process_snr_info(precv_frame);
#endif /* CONFIG_SNR_RPT */
	switch (type) {
	case WIFI_MGT_TYPE: /* mgnt */
		DBG_COUNTER(adapter->rx_logs.core_rx_pre_mgmt);
		retval = validate_recv_mgnt_frame(adapter, precv_frame);
		if (retval == _FAIL) {
			DBG_COUNTER(adapter->rx_logs.core_rx_pre_mgmt_err);
		}
		retval = _FAIL; /* only data frame return _SUCCESS */
		break;
	case WIFI_CTRL_TYPE: /* ctrl */
		DBG_COUNTER(adapter->rx_logs.core_rx_pre_ctrl);
		retval = validate_recv_ctrl_frame(adapter, precv_frame);
		if (retval == _FAIL) {
			DBG_COUNTER(adapter->rx_logs.core_rx_pre_ctrl_err);
		}
		retval = _FAIL; /* only data frame return _SUCCESS */
		break;
	case WIFI_DATA_TYPE: /* data */
		DBG_COUNTER(adapter->rx_logs.core_rx_pre_data);
#ifdef CONFIG_WAPI_SUPPORT
		if (pattrib->qos)
			external_len = 2;
		else
			external_len = 0;

		wai_pkt = rtw_wapi_is_wai_packet(adapter, ptr);

		phdr->bIsWaiPacket = wai_pkt;

		if (wai_pkt != 0) {
			if (sc != adapter->wapiInfo.wapiSeqnumAndFragNum)
				adapter->wapiInfo.wapiSeqnumAndFragNum = sc;
			else {
				retval = _FAIL;
				DBG_COUNTER(adapter->rx_logs.core_rx_pre_data_wapi_seq_err);
				break;
			}
		} else {

			if (rtw_wapi_drop_for_key_absent(adapter, get_addr2_ptr(ptr))) {
				retval = _FAIL;
				WAPI_TRACE(WAPI_RX, "drop for key absent for rx\n");
				DBG_COUNTER(adapter->rx_logs.core_rx_pre_data_wapi_key_err);
				break;
			}
		}

#endif

		pattrib->qos = (subtype & BIT(7)) ? 1 : 0;
		retval = validate_recv_data_frame(adapter, precv_frame);
		if (retval == _FAIL) {
			precvinfo->dbg_rx_drop_count++;
			DBG_COUNTER(adapter->rx_logs.core_rx_pre_data_err);
		} else if (retval == _SUCCESS) {
			#ifdef DBG_RX_DUMP_EAP
			if (!pattrib->encrypt || pattrib->bdecrypted) {
				u8 bDumpRxPkt;
				u16 eth_type;

				/* dump eapol */
				rtw_hal_get_def_var(adapter, adapter_link, HAL_DEF_DBG_DUMP_RXPKT, &(bDumpRxPkt));
				/* get ether_type */
				_rtw_memcpy(&eth_type, ptr + pattrib->hdrlen + pattrib->iv_len + RATTRIB_GET_MCTRL_LEN(pattrib) + LLC_HEADER_SIZE, 2);
				eth_type = ntohs((unsigned short) eth_type);
				if ((bDumpRxPkt == 4) && (eth_type == 0x888e))
					dump_rx_packet(ptr);
			}
			#endif
		} else
			DBG_COUNTER(adapter->rx_logs.core_rx_pre_data_handled);
		break;
	default:
		DBG_COUNTER(adapter->rx_logs.core_rx_pre_unknown);
		#ifdef DBG_RX_DROP_FRAME
		RTW_INFO("DBG_RX_DROP_FRAME "FUNC_ADPT_FMT" fail! type=0x%x\n"
			, FUNC_ADPT_ARG(adapter), type);
		#endif
		retval = _FAIL;
		break;
	}

exit:


	return retval;
}

/* Reture expected handling for LLC */
enum rtw_rx_llc_hdl rtw_recv_llc_parse(u8 *msdu, u16 msdu_len)
{
	u16	eth_type;

	if (msdu_len < 8)
		return RTW_RX_LLC_KEEP;

	eth_type = RTW_GET_BE16(msdu + SNAP_SIZE);

	if ((_rtw_memcmp(msdu, rtw_rfc1042_header, SNAP_SIZE)
			&& eth_type != ETH_P_AARP && eth_type != ETH_P_IPX)
		|| _rtw_memcmp(msdu, rtw_bridge_tunnel_header, SNAP_SIZE)) {
		/* remove RFC1042 or Bridge-Tunnel encapsulation and replace EtherType */
		return RTW_RX_LLC_REMOVE;
	} else {
		/* Leave Ethernet header part of hdr and full payload */
		return RTW_RX_LLC_KEEP;
	}

	/* TODO: VLAN tagged */
}


/* remove the wlanhdr and add the eth_hdr */
sint wlanhdr_to_ethhdr(union recv_frame *precvframe, enum rtw_rx_llc_hdl llc_hdl)
{
	u8 *ptr = get_recvframe_data(precvframe) ; /* point to frame_ctrl field */
	struct rx_pkt_attrib *pattrib = &precvframe->u.hdr.attrib;
	sint rmv_len;
	u16 len;
	sint ret = _SUCCESS;
#ifdef CONFIG_RTW_CORE_RXSC
	_adapter *adapter = precvframe->u.hdr.adapter;
	struct core_rxsc_entry *rxsc_entry = NULL;
	u16 eth_type;
#endif

	if (pattrib->encrypt)
		recvframe_pull_tail(precvframe, pattrib->icv_len);

	rmv_len = pattrib->hdrlen + pattrib->iv_len + RATTRIB_GET_MCTRL_LEN(pattrib) + (llc_hdl ? SNAP_SIZE : 0);
	len = precvframe->u.hdr.len - rmv_len;

#ifdef CONFIG_RTW_CORE_RXSC
	_rtw_memcpy(&eth_type, ptr + rmv_len, 2);
	eth_type = ntohs((unsigned short)eth_type); /* pattrib->ether_type */
	pattrib->eth_type = eth_type;
	if (llc_hdl == RTW_RX_LLC_REMOVE)
		pattrib->bsnaphdr = _TRUE;
	else if (llc_hdl == RTW_RX_LLC_KEEP)
		pattrib->bsnaphdr = _FALSE;

	rxsc_entry = core_rxsc_alloc_entry(adapter, precvframe);

	if (rxsc_entry) {
		/* cache offset of payload */
		rxsc_entry->rxsc_payload_offset = (rmv_len - sizeof(struct ethhdr) + (pattrib->bsnaphdr ? 2 : 0));

		/* cache padding size of tail */
		if (pattrib->encrypt)
			rxsc_entry->rxsc_trim_pad = pattrib->icv_len;
		else
			rxsc_entry->rxsc_trim_pad = 0;

		/* cache WLAN header */
		_rtw_memcpy((void *)&rxsc_entry->rxsc_wlanhdr, ptr, pattrib->hdrlen);
	}
#endif
	ptr = recvframe_pull(precvframe, (rmv_len - sizeof(struct ethhdr) + (llc_hdl ? 2 : 0)));
	if (!ptr) {
		ret = _FAIL;
		goto exiting;
	}


	_rtw_memcpy(ptr, pattrib->dst, ETH_ALEN);
	_rtw_memcpy(ptr + ETH_ALEN, pattrib->src, ETH_ALEN);

	if (!llc_hdl) {
		len = htons(len);
		_rtw_memcpy(ptr + 12, &len, 2);
	}

	rtw_rframe_set_os_pkt(precvframe);

#ifdef CONFIG_RTW_CORE_RXSC
	if (rxsc_entry) {
		/* Cache ETH header */
		memcpy((void *)&rxsc_entry->rxsc_ethhdr, ptr, sizeof(rxsc_entry->rxsc_ethhdr));

		/* Cache Rx Attribute */
		memcpy((void *)&rxsc_entry->rxsc_attrib, pattrib, sizeof(rxsc_entry->rxsc_attrib));
	}
#ifdef RTW_PHL_DBG_CMD
	adapter->core_logs.rxCnt_data_orig++;
#endif
#endif

exiting:
	return ret;

}

#if defined(CONFIG_SDIO_HCI) || defined(CONFIG_GSPI_HCI)
#ifndef CONFIG_SDIO_RX_COPY
#ifdef PLATFORM_LINUX
static void recvframe_expand_pkt(
	_adapter *padapter,
	union recv_frame *prframe)
{
	struct recv_frame_hdr *pfhdr;
	struct sk_buff *ppkt;
	u8 shift_sz;
	u32 alloc_sz;
	u8 *ptr;


	pfhdr = &prframe->u.hdr;

	/*	6 is for IP header 8 bytes alignment in QoS packet case. */
	if (pfhdr->attrib.qos)
		shift_sz = 6;
	else
		shift_sz = 0;

	/* for first fragment packet, need to allocate */
	/* (1536 + RXDESC_SIZE + drvinfo_sz) to reassemble packet */
	/*	8 is for skb->data 8 bytes alignment.
	*	alloc_sz = _RND(1536 + RXDESC_SIZE + pfhdr->attrib.drvinfosize + shift_sz + 8, 128); */
	alloc_sz = 1664; /* round (1536 + 24 + 32 + shift_sz + 8) to 128 bytes alignment */

	/* 3 1. alloc new skb */
	/* prepare extra space for 4 bytes alignment */
	ppkt = rtw_skb_alloc(alloc_sz);

	if (!ppkt)
		return; /* no way to expand */

	/* 3 2. Prepare new skb to replace & release old skb */
	/* force ppkt->data at 8-byte alignment address */
	skb_reserve(ppkt, 8 - ((SIZE_PTR)ppkt->data & 7));
	/* force ip_hdr at 8-byte alignment address according to shift_sz */
	skb_reserve(ppkt, shift_sz);

	/* copy data to new pkt */
	ptr = skb_put(ppkt, pfhdr->len);
	if (ptr)
		_rtw_memcpy(ptr, pfhdr->rx_data, pfhdr->len);

	rtw_skb_free(pfhdr->pkt);

	/* attach new pkt to recvframe */
	pfhdr->pkt = ppkt;
	pfhdr->rx_head = ppkt->head;
	pfhdr->rx_data = ppkt->data;
	pfhdr->rx_tail = skb_tail_pointer(ppkt);
	pfhdr->rx_end = skb_end_pointer(ppkt);
}
#else /*!= PLATFORM_LINUX*/
#warning "recvframe_expand_pkt not implement, defrag may crash system"
#endif
#endif /*#ifndef CONFIG_SDIO_RX_COPY*/
#endif

/* perform defrag */
union recv_frame *recvframe_defrag(_adapter *adapter, _queue *defrag_q)
{
	_list	*plist, *phead;
	u8	*data, wlanhdr_offset;
	u8	curfragnum;
	struct recv_frame_hdr *pfhdr, *pnfhdr;
	union recv_frame *prframe, *pnextrframe;
	u8 *pdata = NULL;
	u64 tmp_iv_hdr = 0;
	u64 pkt_pn = 0, cur_pn = 0;
	struct rx_pkt_attrib *pattrib = NULL;

	curfragnum = 0;
	phead = get_list_head(defrag_q);
	plist = get_next(phead);
	prframe = LIST_CONTAINOR(plist, union recv_frame, u);
	/* CVE-2020-26146 */
	pattrib = &prframe->u.hdr.attrib;
	if (pattrib->encrypt == _AES_  || pattrib->encrypt == _CCMP_256_
		|| pattrib->encrypt == _GCMP_ || pattrib->encrypt == _GCMP_256_ ) {
		pdata = prframe->u.hdr.rx_data;
		tmp_iv_hdr = le64_to_cpu(*(u64*)(pdata + pattrib->hdrlen));
		/* get the first frame's PN. */
		cur_pn = CCMPH_2_PN(tmp_iv_hdr);
	}

	pfhdr = &prframe->u.hdr;
	rtw_list_delete(&(prframe->u.list));

	if (curfragnum != pfhdr->attrib.frag_num) {
		/* the first fragment number must be 0 */
		/* free the whole queue */
		rtw_free_recvframe(prframe);
		rtw_free_recvframe_queue(defrag_q);

		return NULL;
	}

#if defined(CONFIG_SDIO_HCI) || defined(CONFIG_GSPI_HCI)
#ifndef CONFIG_SDIO_RX_COPY
	recvframe_expand_pkt(adapter, prframe);
#endif
#endif

	curfragnum++;

	plist = get_list_head(defrag_q);

	plist = get_next(plist);

	data = get_recvframe_data(prframe);

	while (rtw_end_of_queue_search(phead, plist) == _FALSE) {
		pnextrframe = LIST_CONTAINOR(plist, union recv_frame , u);
		pnfhdr = &pnextrframe->u.hdr;
		/* CVE-2020-26146, check whether the PN is consecutive. */
		pattrib = &pnextrframe->u.hdr.attrib;
		if (pattrib->encrypt == _AES_  || pattrib->encrypt == _CCMP_256_
			|| pattrib->encrypt == _GCMP_ || pattrib->encrypt == _GCMP_256_ ) {
			pdata = pnextrframe->u.hdr.rx_data;
			tmp_iv_hdr = le64_to_cpu(*(u64*)(pdata + pattrib->hdrlen));
			pkt_pn = CCMPH_2_PN(tmp_iv_hdr);
			if (pkt_pn != cur_pn + 1) {
				RTW_INFO("%s non-consective PN! old:%llu, new:%llu\n",
					__func__, cur_pn, pkt_pn);
				/* PN must be consecutive */
				/* release the defrag_q & prframe */
				rtw_free_recvframe(prframe);
				rtw_free_recvframe_queue(defrag_q);
				return NULL;
			} else {
				cur_pn = pkt_pn;
			}
		}

		/* CVE-2020-24587, The keytrack of the fragment is supposed to be the same with other's	*/
		if (pfhdr->keytrack != pnfhdr->keytrack) {
			RTW_INFO("Inconsistent key track, drop fragmented frame!\n");
			rtw_free_recvframe(prframe);
			rtw_free_recvframe_queue(defrag_q);
			return NULL;
		}

		/* check the fragment sequence  (2nd ~n fragment frame) */

		if (curfragnum != pnfhdr->attrib.frag_num) {
			/* the fragment number must be increasing  (after decache) */
			/* release the defrag_q & prframe */
			rtw_free_recvframe(prframe);
			rtw_free_recvframe_queue(defrag_q);
			return NULL;
		}

		/* copy the 2nd~n fragment frame's payload to the first fragment */
		/* get the 2nd~last fragment frame's payload */

		wlanhdr_offset = pnfhdr->attrib.hdrlen + pnfhdr->attrib.iv_len;

		recvframe_pull(pnextrframe, wlanhdr_offset);

		if ((pfhdr->rx_end - pfhdr->rx_tail) < pnfhdr->len) {
			RTW_INFO("Not enough buffer space, drop fragmented frame!\n");
			rtw_free_recvframe(prframe);
			rtw_free_recvframe_queue(defrag_q);
			return NULL;
		}

		curfragnum++;

		/* append  to first fragment frame's tail (if privacy frame, pull the ICV) */
		recvframe_pull_tail(prframe, pfhdr->attrib.icv_len);

		/* _rtw_memcpy */
		_rtw_memcpy(pfhdr->rx_tail, pnfhdr->rx_data, pnfhdr->len);

		recvframe_put(prframe, pnfhdr->len);

		pfhdr->attrib.icv_len = pnfhdr->attrib.icv_len;
		plist = get_next(plist);

	};

	/* free the defrag_q queue and return the prframe */
	rtw_free_recvframe_queue(defrag_q);



	return prframe;
}

/* check if need to defrag, if needed queue the frame to defrag_q */
s32 recvframe_chk_defrag(_adapter *padapter, union recv_frame **pprecv_frame)
{
	u8	ismfrag;
	u8	fragnum;
	u8	*psta_addr;
	struct recv_frame_hdr *pfhdr;
	struct sta_info *psta;
	struct sta_priv *pstapriv;
	_list *phead;
	union recv_frame *prtnframe = NULL;
	_queue *pdefrag_q = NULL;
	union recv_frame *precv_frame = *pprecv_frame;
	s32 ret = CORE_RX_CONTINUE;


	pstapriv = &padapter->stapriv;

	pfhdr = &precv_frame->u.hdr;

	/* need to define struct of wlan header frame ctrl */
	ismfrag = pfhdr->attrib.mfrag;
	fragnum = pfhdr->attrib.frag_num;

	psta_addr = pfhdr->attrib.ta;
	psta = precv_frame->u.hdr.psta;
	if (psta == NULL) {
		u8 type = GetFrameType(pfhdr->rx_data);
		if (type != WIFI_DATA_TYPE) {
			psta = rtw_get_bcmc_stainfo(padapter, precv_frame->u.hdr.adapter_link);
			if (psta)
				pdefrag_q = &psta->sta_recvpriv.defrag_q;
		} else
			pdefrag_q = NULL;
	} else
		pdefrag_q = &psta->sta_recvpriv.defrag_q;

	if ((ismfrag == 0) && (fragnum == 0)) {
		ret = CORE_RX_CONTINUE;
	} else {
		/* CVE-2020-26145, group addressed frame cannot use fragmentation!! */
		if (IS_MCAST(pfhdr->attrib.ra)) {
			RTW_INFO("DROP group addressed fragment!\n");
			ret = CORE_RX_DROP;
		}
		/* CVE-2020-24587 */
		if ((psta) && (pdefrag_q))
			precv_frame->u.hdr.keytrack = ATOMIC_READ(&psta->keytrack);
	}

	if (ismfrag == 1) {
		/* 0~(n-1) fragment frame */
		/* enqueue to defraf_g */
		if (pdefrag_q != NULL) {
			if (fragnum == 0) {
				/* the first fragment */
				if (_rtw_queue_empty(pdefrag_q) == _FALSE) {
					/* free current defrag_q */
					rtw_free_recvframe_queue(pdefrag_q);
				}
			}


			/* Then enqueue the 0~(n-1) fragment into the defrag_q */

			/* _rtw_spinlock(&pdefrag_q->lock); */
			phead = get_list_head(pdefrag_q);
			rtw_list_insert_tail(&pfhdr->list, phead);
			/* _rtw_spinunlock(&pdefrag_q->lock); */


			ret = CORE_RX_DEFRAG;
		} else {
			/* can't find this ta's defrag_queue, so free this recv_frame */
			ret = CORE_RX_DROP;
		}

	}

	if ((ismfrag == 0) && (fragnum != 0)) {
		/* the last fragment frame */
		/* enqueue the last fragment */
		if (pdefrag_q != NULL) {
			/* _rtw_spinlock(&pdefrag_q->lock); */
			phead = get_list_head(pdefrag_q);
			rtw_list_insert_tail(&pfhdr->list, phead);
			/* _rtw_spinunlock(&pdefrag_q->lock); */

			/* call recvframe_defrag to defrag */
			 *pprecv_frame = recvframe_defrag(padapter, pdefrag_q);
			if (*pprecv_frame == NULL)
				ret = CORE_RX_DROP;
			else {
				ret = CORE_RX_CONTINUE;
			}
		} else {
			/* can't find this ta's defrag_queue, so free this recv_frame */
			ret = CORE_RX_DROP;
		}

	}

	return ret;

}

static int rtw_recv_indicatepkt_check(union recv_frame *rframe, u8 *ehdr_pos, u32 pkt_len)
{
	_adapter *adapter = rframe->u.hdr.adapter;
	struct recv_info *recvinfo = &adapter->recvinfo;
	struct ethhdr *ehdr = (struct ethhdr *)ehdr_pos;
	struct rx_pkt_attrib *pattrib = &rframe->u.hdr.attrib;
#ifdef DBG_IP_R_MONITOR
	int i;
	struct mlme_ext_priv *pmlmeext = &adapter->mlmeextpriv;
	struct mlme_priv	*pmlmepriv = &adapter->mlmepriv;
	struct wlan_network *cur_network = &(pmlmepriv->dev_cur_network);
#endif/*DBG_IP_R_MONITOR*/
	enum eap_type eapol_type;
	int ret = _FAIL;

#ifdef CONFIG_WAPI_SUPPORT
	if (rtw_wapi_check_for_drop(adapter, rframe, ehdr_pos)) {
		#ifdef DBG_RX_DROP_FRAME
		RTW_INFO("DBG_RX_DROP_FRAME "FUNC_ADPT_FMT" rtw_wapi_check_for_drop\n"
			, FUNC_ADPT_ARG(adapter));
		#endif
		goto exit;
	}
#endif

	if (rframe->u.hdr.psta)
		rtw_st_ctl_rx(rframe->u.hdr.psta, ehdr_pos);

	if (ntohs(ehdr->h_proto) == 0x888e) {
		eapol_type = parsing_eapol_packet(adapter, ehdr_pos + ETH_HLEN, rframe->u.hdr.psta, 0);
		if ((eapol_type == EAPOL_1_4 || eapol_type == EAPOL_3_4) && pattrib->encrypt == 0) {
			rframe->u.hdr.psta->resp_nonenc_eapol_key_starttime = rtw_get_current_time();
			RTW_INFO("Receive unencrypted eapol key\n");
		}
	}
#ifdef DBG_ARP_DUMP
	else if (ntohs(ehdr->h_proto) == ETH_P_ARP)
		dump_arp_pkt(RTW_DBGDUMP, ehdr->h_dest, ehdr->h_source, ehdr_pos + ETH_HLEN, 0);
#endif

	if (recvinfo->sink_udpport > 0)
		rtw_sink_rtp_seq_dbg(adapter, ehdr_pos);

#ifdef DBG_UDP_PKT_LOSE_11AC
	#define PAYLOAD_LEN_LOC_OF_IP_HDR 0x10 /*ethernet payload length location of ip header (DA + SA+eth_type+(version&hdr_len)) */

	if (ntohs(ehdr->h_proto) == ETH_P_ARP) {
		/* ARP Payload length will be 42bytes or 42+18(tailer)=60bytes*/
		if (pkt_len != 42 && pkt_len != 60)
			RTW_INFO("Error !!%s,ARP Payload length %u not correct\n" , __func__ , pkt_len);
	} else if (ntohs(ehdr->h_proto) == ETH_P_IP) {
		if (be16_to_cpu(*((u16 *)(ehdr_pos + PAYLOAD_LEN_LOC_OF_IP_HDR))) != (pkt_len) - ETH_HLEN) {
			RTW_INFO("Error !!%s,Payload length not correct\n" , __func__);
			RTW_INFO("%s, IP header describe Total length=%u\n" , __func__ , be16_to_cpu(*((u16 *)(ehdr_pos + PAYLOAD_LEN_LOC_OF_IP_HDR))));
			RTW_INFO("%s, Pkt real length=%u\n" , __func__ , (pkt_len) - ETH_HLEN);
		}
	}
#endif

#ifdef DBG_IP_R_MONITOR
	#define LEN_ARP_OP_HDR 7 /*ARP OERATION */
	if (ntohs(ehdr->h_proto) == ETH_P_ARP) {

		if(check_fwstate(pmlmepriv, WIFI_ASOC_STATE) == _TRUE){
			if(ehdr_pos[ETHERNET_HEADER_SIZE+LEN_ARP_OP_HDR] == 2) {

				RTW_INFO("%s,[DBG_ARP] Rx ARP RSP Packet;SeqNum = %d !\n",
					__FUNCTION__, pattrib->seq_num);

				dump_arp_pkt(RTW_DBGDUMP, ehdr->h_dest, ehdr->h_source, ehdr_pos + ETH_HLEN, 0);

			}
		}
	}
#endif/*DBG_IP_R_MONITOR*/

#ifdef CONFIG_AUTO_AP_MODE
	if (ntohs(ehdr->h_proto) == 0x8899)
		rtw_auto_ap_rx_msg_dump(adapter, rframe, ehdr_pos);
#endif

	ret = _SUCCESS;

#ifdef CONFIG_WAPI_SUPPORT
exit:
#endif
	return ret;
}

#if defined(CONFIG_AP_MODE) || defined(CONFIG_RTW_MESH)
static void recv_free_fwd_resource(_adapter *adapter, struct xmit_frame *fwd_frame, _list *f_list)
{
	struct xmit_priv *xmitpriv = &adapter->xmitpriv;

	if (fwd_frame)
		core_tx_free_xmitframe(adapter, fwd_frame);

#if defined(CONFIG_RTW_WDS) || CONFIG_RTW_DATA_BMC_TO_UC
	if (!rtw_is_list_empty(f_list)) {
		struct xmit_frame *fframe;
		_list *list;

		list = get_next(f_list);
		while (rtw_end_of_queue_search(f_list, list) == _FALSE) {
			fframe = LIST_CONTAINOR(list, struct xmit_frame, list);
			list = get_next(list);
			rtw_list_delete(&fframe->list);
			core_tx_free_xmitframe(adapter, fframe);
		}
	}
#endif
}

static void recv_fwd_pkt_hdl(_adapter *adapter, struct sk_buff *pkt
	, u8 act, struct xmit_frame *fwd_frame, _list *f_list)
{
	struct xmit_priv *xmitpriv = &adapter->xmitpriv;
	struct sk_buff *fwd_pkt = pkt;

	if (act & RTW_RX_MSDU_ACT_INDICATE) {
		fwd_pkt = rtw_skb_copy(pkt);
		if (!fwd_pkt) {
			#ifdef DBG_TX_DROP_FRAME
			RTW_INFO("DBG_TX_DROP_FRAME %s rtw_skb_copy fail\n", __func__);
			#endif
			recv_free_fwd_resource(adapter, fwd_frame, f_list);
			goto exit;
		}
	}

#if defined(CONFIG_RTW_WDS) || CONFIG_RTW_DATA_BMC_TO_UC
	if (!rtw_is_list_empty(f_list)) {
		_list *list = get_next(f_list);
		struct xmit_frame *fframe;

		while (rtw_end_of_queue_search(f_list, list) == _FALSE) {
			fframe = LIST_CONTAINOR(list, struct xmit_frame, list);
			list = get_next(list);
			rtw_list_delete(&fframe->list);

			if (!fwd_frame && rtw_is_list_empty(f_list)) /* the last fwd_pkt */
				fframe->pkt = fwd_pkt;
			else
				fframe->pkt = rtw_skb_copy(fwd_pkt);
			if (!fframe->pkt) {
				core_tx_free_xmitframe(adapter, fframe);
				continue;
			}

			core_tx_per_packet(adapter, fframe, &fframe->pkt, NULL);
		}
	}
#endif

	if (fwd_frame) {
		fwd_frame->pkt = fwd_pkt;
		if (core_tx_per_packet(adapter, fwd_frame, &fwd_pkt, NULL) < 0) {
			#ifdef DBG_TX_DROP_FRAME
			RTW_INFO("DBG_TX_DROP_FRAME %s rtw_xmit_posthandle fail\n", __func__);
			#endif
			xmitpriv->tx_drop++;
		}
	}

exit:
	return;
}
#endif /* defined(CONFIG_AP_MODE) || defined(CONFIG_RTW_MESH) */

/*
 * From WFA suggestion:						*
 * If first subframe meets one of the following condition,	*
 * the whole received AMSDU should drop.			*
 * 1. subframe's DA is not the same as RA in From DS case.	*
 * 2. subframe's SA is not the same as TA in To DS case.	*
 * 3. subframe's DA is AA:AA:03:00:00:00			*
								*/
static u8 validate_amsdu_content(_adapter *padapter, union recv_frame *prframe,
	const u8 *da, const u8 *sa)
{
	struct rx_pkt_attrib	*pattrib = &prframe->u.hdr.attrib;
	u8 ret = _SUCCESS;

	/* Use the recommendation method form Wi-Fi alliance to check subframe */
	/* in protected network */
	if (padapter->registrypriv.amsdu_mode == RTW_AMSDU_MODE_NON_SPP &&
		padapter->securitypriv.dot11PrivacyAlgrthm != _NO_PRIVACY_) {

		/* 1.check From DS */
		if (pattrib->to_fr_ds == 1) {
			if (_rtw_memcmp(da, pattrib->ra, ETH_ALEN) == _FALSE)
			ret = _FAIL;
		}

		/* 2.check To DS */
		if (pattrib->to_fr_ds == 2) {
			if (_rtw_memcmp(sa, pattrib->ta, ETH_ALEN) == _FALSE)
			ret = _FAIL;
		}

		/* 3.Check whether DA is AA:AA:03:00:00:00 */
		if (_rtw_memcmp(da, rtw_rfc1042_header, ETH_ALEN) == _TRUE)
			ret = _FAIL;

	}

	return ret;

}

int amsdu_to_msdu(_adapter *padapter, union recv_frame *prframe)
{
	struct rx_pkt_attrib *rattrib = &prframe->u.hdr.attrib;
	int	a_len, padding_len;
	u16	nSubframe_Length;
	u8	nr_subframes, i;
	u8	*pdata;
	struct sk_buff *sub_pkt, *subframes[MAX_SUBFRAME_COUNT];
	const u8 *da, *sa;
	int act;
#if defined(CONFIG_AP_MODE) || defined(CONFIG_RTW_MESH)
	struct xmit_frame *fwd_frame;
	_list f_list;
#endif
	enum rtw_rx_llc_hdl llc_hdl;
	u8 mctrl_len = 0;
	int	ret = _SUCCESS;

	nr_subframes = 0;

	recvframe_pull(prframe, rattrib->hdrlen);

	if (rattrib->iv_len > 0)
		recvframe_pull(prframe, rattrib->iv_len);

	if (rattrib->encrypt)
		recvframe_pull_tail(prframe, rattrib->icv_len);

	a_len = prframe->u.hdr.len;
	pdata = prframe->u.hdr.rx_data;

	while (a_len > ETH_HLEN) {
		/* Offset 12 denote 2 mac address */
		nSubframe_Length = RTW_GET_BE16(pdata + 12);
		if (a_len < (ETHERNET_HEADER_SIZE + nSubframe_Length)) {
			RTW_INFO("nRemain_Length is %d and nSubframe_Length is : %d\n", a_len, nSubframe_Length);
			break;
		}

		act = RTW_RX_MSDU_ACT_INDICATE;

		#if defined(CONFIG_AP_MODE) || defined(CONFIG_RTW_MESH)
		fwd_frame = NULL;
		#endif

		#ifdef CONFIG_RTW_MESH
		if (MLME_IS_MESH(padapter)) {
			u8 *mda = pdata, *msa = pdata + ETH_ALEN;
			struct rtw_ieee80211s_hdr *mctrl = (struct rtw_ieee80211s_hdr *)(pdata + ETH_HLEN);
			int v_ret;

			v_ret = rtw_mesh_rx_data_validate_mctrl(padapter, prframe
				, mctrl, mda, msa, &mctrl_len, &da, &sa);
			if (v_ret != _SUCCESS)
				goto move_to_next;

			if (validate_amsdu_content(padapter, prframe, da, sa) == _FAIL) {
				RTW_INFO("%s check subframe content fail!\n", __func__);
				break;
			}

			llc_hdl = rtw_recv_llc_parse(pdata + ETH_HLEN + mctrl_len, nSubframe_Length - mctrl_len);
			act = rtw_mesh_rx_msdu_act_check(prframe
				, mda, msa, da, sa, mctrl
				, pdata + ETH_HLEN + mctrl_len, llc_hdl
				, &fwd_frame, &f_list);
		} else
		#endif
		{
			da = pdata;
			sa = pdata + ETH_ALEN;

			if (validate_amsdu_content(padapter, prframe, da, sa) == _FAIL) {
				RTW_INFO("%s check subframe content fail!\n", __func__);
				break;
			}

			llc_hdl = rtw_recv_llc_parse(pdata + ETH_HLEN, nSubframe_Length);
			#ifdef CONFIG_AP_MODE
			if (MLME_IS_AP(padapter)) {
				act = rtw_ap_rx_msdu_act_check(prframe, da, sa
					, pdata + ETH_HLEN, llc_hdl, &fwd_frame, &f_list);
			} else
			#endif
			if (MLME_IS_STA(padapter))
				act = rtw_sta_rx_amsdu_act_check(prframe, da, sa);
		}

		if (!act)
			goto move_to_next;

		rtw_led_rx_control(padapter, da);

		sub_pkt = rtw_os_alloc_msdu_pkt(prframe, da, sa
			, pdata + ETH_HLEN + mctrl_len, nSubframe_Length - mctrl_len, llc_hdl);
		if (sub_pkt == NULL) {
			if (act & RTW_RX_MSDU_ACT_INDICATE) {
				#ifdef DBG_RX_DROP_FRAME
				RTW_INFO("DBG_RX_DROP_FRAME %s rtw_os_alloc_msdu_pkt fail\n", __func__);
				#endif
			}
			#if defined(CONFIG_AP_MODE) || defined(CONFIG_RTW_MESH)
			if (act & RTW_RX_MSDU_ACT_FORWARD) {
				#ifdef DBG_TX_DROP_FRAME
				RTW_INFO("DBG_TX_DROP_FRAME %s rtw_os_alloc_msdu_pkt fail\n", __func__);
				#endif
				recv_free_fwd_resource(padapter, fwd_frame, &f_list);
			}
			#endif
			break;
		}

		#if defined(CONFIG_AP_MODE) || defined(CONFIG_RTW_MESH)
		if (act & RTW_RX_MSDU_ACT_FORWARD) {
			recv_fwd_pkt_hdl(padapter, sub_pkt, act, fwd_frame, &f_list);
			if (!(act & RTW_RX_MSDU_ACT_INDICATE))
				goto move_to_next;
		}
		#endif

		if (rtw_recv_indicatepkt_check(prframe, rtw_skb_data(sub_pkt), rtw_skb_len(sub_pkt)) == _SUCCESS)
			subframes[nr_subframes++] = sub_pkt;
		else
			rtw_skb_free(sub_pkt);

#if defined(CONFIG_AP_MODE) || defined(CONFIG_RTW_MESH)
move_to_next:
#endif
		/* move the data point to data content */
		pdata += ETH_HLEN;
		a_len -= ETH_HLEN;

		if (nr_subframes >= MAX_SUBFRAME_COUNT) {
			RTW_WARN("ParseSubframe(): Too many Subframes! Packets dropped!\n");
			break;
		}

		pdata += nSubframe_Length;
		a_len -= nSubframe_Length;
		if (a_len != 0) {
			padding_len = 4 - ((nSubframe_Length + ETH_HLEN) & (4 - 1));
			if (padding_len == 4)
				padding_len = 0;

			if (a_len < padding_len) {
				RTW_INFO("ParseSubframe(): a_len < padding_len !\n");
				break;
			}
			pdata += padding_len;
			a_len -= padding_len;
		}
	}

	for (i = 0; i < nr_subframes; i++) {
		sub_pkt = subframes[i];

		/* Indicat the packets to upper layer */
		if (sub_pkt)
			rtw_os_recv_indicate_pkt(padapter, sub_pkt, prframe);
	}

	prframe->u.hdr.len = 0;
	rtw_free_recvframe(prframe);/* free this recv_frame */

	return ret;
}

static int recv_process_mpdu(_adapter *padapter, union recv_frame *prframe)
{
	struct rx_pkt_attrib *pattrib = &prframe->u.hdr.attrib;
	int ret;

	if (pattrib->amsdu) {
		ret = amsdu_to_msdu(padapter, prframe);
		if (ret != _SUCCESS) {
			#ifdef DBG_RX_DROP_FRAME
			RTW_INFO("DBG_RX_DROP_FRAME "FUNC_ADPT_FMT" amsdu_to_msdu fail\n"
				, FUNC_ADPT_ARG(padapter));
			#endif
			rtw_free_recvframe(prframe);
			goto exit;
		}
	} else {
		int act = RTW_RX_MSDU_ACT_INDICATE;

		#ifdef CONFIG_RTW_MESH /* TODO: move AP mode forward & b2u logic here */
		struct xmit_frame *fwd_frame = NULL;
		_list f_list;

		if (MLME_IS_MESH(padapter) && pattrib->mesh_ctrl_present) {
			act = rtw_mesh_rx_msdu_act_check(prframe
				, pattrib->mda, pattrib->msa
				, pattrib->dst, pattrib->src
				, (struct rtw_ieee80211s_hdr *)(get_recvframe_data(prframe) + pattrib->hdrlen + pattrib->iv_len)
				, &fwd_frame, &f_list);
		}
		#endif

		#ifdef CONFIG_RTW_MESH
		if (!act) {
			rtw_free_recvframe(prframe);
			ret = _FAIL;
			goto exit;
		}
		#endif

		rtw_led_rx_control(padapter, pattrib->dst);

		ret = wlanhdr_to_ethhdr(prframe, 0);
		if (ret != _SUCCESS) {
			if (act & RTW_RX_MSDU_ACT_INDICATE) {
				#ifdef DBG_RX_DROP_FRAME
				RTW_INFO("DBG_RX_DROP_FRAME "FUNC_ADPT_FMT" wlanhdr_to_ethhdr: drop pkt\n"
					, FUNC_ADPT_ARG(padapter));
				#endif
			}
			#ifdef CONFIG_RTW_MESH
			if (act & RTW_RX_MSDU_ACT_FORWARD) {
				#ifdef DBG_TX_DROP_FRAME
				RTW_INFO("DBG_TX_DROP_FRAME %s wlanhdr_to_ethhdr fail\n", __func__);
				#endif
				recv_free_fwd_resource(padapter, fwd_frame, &f_list);
			}
			#endif
			rtw_free_recvframe(prframe);
			goto exit;
		}

		#ifdef CONFIG_RTW_MESH
		if (act & RTW_RX_MSDU_ACT_FORWARD) {
			recv_fwd_pkt_hdl(padapter, prframe->u.hdr.pkt, act, fwd_frame, &f_list);
			if (!(act & RTW_RX_MSDU_ACT_INDICATE)) {
				prframe->u.hdr.pkt = NULL;
				rtw_free_recvframe(prframe);
				goto exit;
			}
		}
		#endif

		if (!RTW_CANNOT_RUN(adapter_to_dvobj(padapter))) {
			ret = rtw_recv_indicatepkt_check(prframe
				, get_recvframe_data(prframe), get_recvframe_len(prframe));
			if (ret != _SUCCESS) {
				rtw_free_recvframe(prframe);
				goto exit;
			}

			/* indicate this recv_frame */
			ret = rtw_recv_indicatepkt(padapter, prframe);
			if (ret != _SUCCESS) {
				#ifdef DBG_RX_DROP_FRAME
				RTW_INFO("DBG_RX_DROP_FRAME "FUNC_ADPT_FMT" rtw_recv_indicatepkt fail!\n"
					, FUNC_ADPT_ARG(padapter));
				#endif
				goto exit;
			}
		} else {
			#ifdef DBG_RX_DROP_FRAME
			RTW_INFO("DBG_RX_DROP_FRAME "FUNC_ADPT_FMT" DS:%u SR:%u\n"
				, FUNC_ADPT_ARG(padapter)
				, dev_is_drv_stopped(adapter_to_dvobj(padapter))
				, dev_is_surprise_removed(adapter_to_dvobj(padapter)));
			#endif
			ret = _SUCCESS; /* don't count as packet drop */
			rtw_free_recvframe(prframe);
		}
	}

exit:
	return ret;
}

#if defined(CONFIG_80211N_HT) && defined(CONFIG_RECV_REORDERING_CTRL)
static int check_indicate_seq(struct recv_reorder_ctrl *preorder_ctrl, u16 seq_num)
{
	_adapter *padapter = preorder_ctrl->padapter;
	struct recv_info *precvinfo = &padapter->recvinfo;
	u8	wsize = preorder_ctrl->wsize_b;
	u16	wend;

	/* Rx Reorder initialize condition. */
	if (preorder_ctrl->indicate_seq == 0xFFFF) {
		preorder_ctrl->indicate_seq = seq_num;
		#ifdef DBG_RX_SEQ
		RTW_INFO("DBG_RX_SEQ "FUNC_ADPT_FMT" tid:%u SN_INIT indicate_seq:%d, seq_num:%d\n"
			, FUNC_ADPT_ARG(padapter), preorder_ctrl->tid, preorder_ctrl->indicate_seq, seq_num);
		#endif
	}
	wend = (preorder_ctrl->indicate_seq + wsize - 1) & 0xFFF; /* % 4096; */

	/* Drop out the packet which SeqNum is smaller than WinStart */
	if (SN_LESS(seq_num, preorder_ctrl->indicate_seq)) {
		#ifdef DBG_RX_DROP_FRAME
		RTW_INFO(FUNC_ADPT_FMT" tid:%u indicate_seq:%d > seq_num:%d\n"
			, FUNC_ADPT_ARG(padapter), preorder_ctrl->tid, preorder_ctrl->indicate_seq, seq_num);
		#endif
		return _FALSE;
	}

	/*
	* Sliding window manipulation. Conditions includes:
	* 1. Incoming SeqNum is equal to WinStart =>Window shift 1
	* 2. Incoming SeqNum is larger than the WinEnd => Window shift N
	*/
	if (SN_EQUAL(seq_num, preorder_ctrl->indicate_seq)) {
		preorder_ctrl->indicate_seq = (preorder_ctrl->indicate_seq + 1) & 0xFFF;
		#ifdef DBG_RX_SEQ
		RTW_INFO("DBG_RX_SEQ "FUNC_ADPT_FMT" tid:%u SN_EQUAL indicate_seq:%d, seq_num:%d\n"
			, FUNC_ADPT_ARG(padapter), preorder_ctrl->tid, preorder_ctrl->indicate_seq, seq_num);
		#endif

	} else if (SN_LESS(wend, seq_num)) {
		/* boundary situation, when seq_num cross 0xFFF */
		if (seq_num >= (wsize - 1))
			preorder_ctrl->indicate_seq = seq_num + 1 - wsize;
		else
			preorder_ctrl->indicate_seq = 0xFFF - (wsize - (seq_num + 1)) + 1;

		precvinfo->dbg_rx_ampdu_window_shift_cnt++;
		#ifdef DBG_RX_SEQ
		RTW_INFO("DBG_RX_SEQ "FUNC_ADPT_FMT" tid:%u SN_LESS(wend, seq_num) indicate_seq:%d, seq_num:%d\n"
			, FUNC_ADPT_ARG(padapter), preorder_ctrl->tid, preorder_ctrl->indicate_seq, seq_num);
		#endif
	}

	return _TRUE;
}

static int enqueue_reorder_recvframe(struct recv_reorder_ctrl *preorder_ctrl, union recv_frame *prframe)
{
	struct rx_pkt_attrib *pattrib = &prframe->u.hdr.attrib;
	_queue *ppending_recvframe_queue = &preorder_ctrl->pending_recvframe_queue;
	_list	*phead, *plist;
	union recv_frame *pnextrframe;
	struct rx_pkt_attrib *pnextattrib;

	/* DbgPrint("+enqueue_reorder_recvframe()\n"); */

	/* _rtw_spinlock_irq(&ppending_recvframe_queue->lock, &sp_flags); */

	phead = get_list_head(ppending_recvframe_queue);
	plist = get_next(phead);

	while (rtw_end_of_queue_search(phead, plist) == _FALSE) {
		pnextrframe = LIST_CONTAINOR(plist, union recv_frame, u);
		pnextattrib = &pnextrframe->u.hdr.attrib;

		if (SN_LESS(pnextattrib->seq_num, pattrib->seq_num))
			plist = get_next(plist);
		else if (SN_EQUAL(pnextattrib->seq_num, pattrib->seq_num)) {
			/* Duplicate entry is found!! Do not insert current entry. */

			/* _rtw_spinunlock_irq(&ppending_recvframe_queue->lock, &sp_flags); */

			return _FALSE;
		} else
			break;

		/* DbgPrint("enqueue_reorder_recvframe():while\n"); */

	}


	/* _rtw_spinlock_irq(&ppending_recvframe_queue->lock, &sp_flags); */

	rtw_list_delete(&(prframe->u.hdr.list));

	rtw_list_insert_tail(&(prframe->u.hdr.list), plist);

	/* _rtw_spinunlock_irq(&ppending_recvframe_queue->lock, &sp_flags); */


	return _TRUE;

}

static void recv_indicatepkts_pkt_loss_cnt(_adapter *padapter, u64 prev_seq, u64 current_seq)
{
	struct recv_info *precvinfo = &padapter->recvinfo;

	if (current_seq < prev_seq) {
		precvinfo->dbg_rx_ampdu_loss_count += (4096 + current_seq - prev_seq);
		precvinfo->rx_drop += (4096 + current_seq - prev_seq);
	} else {
		precvinfo->dbg_rx_ampdu_loss_count += (current_seq - prev_seq);
		precvinfo->rx_drop += (current_seq - prev_seq);
	}
}

static int recv_indicatepkts_in_order(_adapter *padapter, struct recv_reorder_ctrl *preorder_ctrl, int bforced)
{
	_list	*phead, *plist;
	union recv_frame *prframe;
	struct rx_pkt_attrib *pattrib;
	/* u8 index = 0; */
	int bPktInBuf = _FALSE;
	struct recv_info *precvinfo = &padapter->recvinfo;
	_queue *ppending_recvframe_queue = &preorder_ctrl->pending_recvframe_queue;

	DBG_COUNTER(padapter->rx_logs.core_rx_post_indicate_in_oder);

	/* DbgPrint("+recv_indicatepkts_in_order\n"); */

	/* _rtw_spinlock_irq(&ppending_recvframe_queue->lock, &sp_flags); */

	phead =	get_list_head(ppending_recvframe_queue);
	plist = get_next(phead);

#if 0
	/* Check if there is any other indication thread running. */
	if (pTS->RxIndicateState == RXTS_INDICATE_PROCESSING)
		return;
#endif

	/* Handling some condition for forced indicate case. */
	if (bforced == _TRUE) {
		precvinfo->dbg_rx_ampdu_forced_indicate_count++;
		if (rtw_is_list_empty(phead)) {
			/* _rtw_spinunlock_irq(&ppending_recvframe_queue->lock, &sp_flags); */
			return _TRUE;
		}

		prframe = LIST_CONTAINOR(plist, union recv_frame, u);
		pattrib = &prframe->u.hdr.attrib;

		#ifdef DBG_RX_SEQ
		RTW_INFO("DBG_RX_SEQ "FUNC_ADPT_FMT" tid:%u FORCE indicate_seq:%d, seq_num:%d\n"
			, FUNC_ADPT_ARG(padapter), preorder_ctrl->tid, preorder_ctrl->indicate_seq, pattrib->seq_num);
		#endif
		recv_indicatepkts_pkt_loss_cnt(padapter, preorder_ctrl->indicate_seq, pattrib->seq_num);
		preorder_ctrl->indicate_seq = pattrib->seq_num;
	}

	/* Prepare indication list and indication. */
	/* Check if there is any packet need indicate. */
	while (!rtw_is_list_empty(phead)) {

		prframe = LIST_CONTAINOR(plist, union recv_frame, u);
		pattrib = &prframe->u.hdr.attrib;

		if (!SN_LESS(preorder_ctrl->indicate_seq, pattrib->seq_num)) {

#if 0
			/* This protect buffer from overflow. */
			if (index >= REORDER_WIN_SIZE) {
				RT_ASSERT(FALSE, ("IndicateRxReorderList(): Buffer overflow!!\n"));
				bPktInBuf = TRUE;
				break;
			}
#endif

			plist = get_next(plist);
			rtw_list_delete(&(prframe->u.hdr.list));

			if (SN_EQUAL(preorder_ctrl->indicate_seq, pattrib->seq_num)) {
				preorder_ctrl->indicate_seq = (preorder_ctrl->indicate_seq + 1) & 0xFFF;
				#ifdef DBG_RX_SEQ
				RTW_INFO("DBG_RX_SEQ "FUNC_ADPT_FMT" tid:%u SN_EQUAL indicate_seq:%d, seq_num:%d\n"
					, FUNC_ADPT_ARG(padapter), preorder_ctrl->tid, preorder_ctrl->indicate_seq, pattrib->seq_num);
				#endif
			}

#if 0
			index++;
			if (index == 1) {
				/* Cancel previous pending timer. */
				/* PlatformCancelTimer(adapter, &pTS->RxPktPendingTimer); */
				if (bforced != _TRUE) {
					/* RTW_INFO("_cancel_timer_ex(&preorder_ctrl->reordering_ctrl_timer);\n"); */
					_cancel_timer_ex(&preorder_ctrl->reordering_ctrl_timer);
				}
			}
#endif

			/* Set this as a lock to make sure that only one thread is indicating packet. */
			/* pTS->RxIndicateState = RXTS_INDICATE_PROCESSING; */

			/* Indicate packets */
			/* RT_ASSERT((index<=REORDER_WIN_SIZE), ("RxReorderIndicatePacket(): Rx Reorder buffer full!!\n")); */


			/* indicate this recv_frame */
			/* DbgPrint("recv_indicatepkts_in_order, indicate_seq=%d, seq_num=%d\n", precvpriv->indicate_seq, pattrib->seq_num); */
			if (recv_process_mpdu(padapter, prframe) != _SUCCESS)
				precvinfo->dbg_rx_drop_count++;

			/* Update local variables. */
			bPktInBuf = _FALSE;

		} else {
			bPktInBuf = _TRUE;
			break;
		}

		/* DbgPrint("recv_indicatepkts_in_order():while\n"); */

	}

	/* _rtw_spinunlock_irq(&ppending_recvframe_queue->lock, sp_flags); */

#if 0
	/* Release the indication lock and set to new indication step. */
	if (bPktInBuf) {
		/*  Set new pending timer. */
		/* pTS->RxIndicateState = RXTS_INDICATE_REORDER; */
		/* PlatformSetTimer(adapter, &pTS->RxPktPendingTimer, pHTInfo->RxReorderPendingTime); */

		_set_timer(&preorder_ctrl->reordering_ctrl_timer, REORDER_WAIT_TIME);
	} else {
		/* pTS->RxIndicateState = RXTS_INDICATE_IDLE; */
	}
#endif
	/* _rtw_spinunlock_irq(&ppending_recvframe_queue->lock, sp_flags); */

	/* return _TRUE; */
	return bPktInBuf;

}

static int recv_indicatepkt_reorder(_adapter *padapter, union recv_frame *prframe)
{
	struct rx_pkt_attrib *pattrib = &prframe->u.hdr.attrib;
	struct recv_reorder_ctrl *preorder_ctrl = prframe->u.hdr.preorder_ctrl;
	_queue *ppending_recvframe_queue = preorder_ctrl ? &preorder_ctrl->pending_recvframe_queue : NULL;
	struct recv_info  *precvinfo = &padapter->recvinfo;

	if (!pattrib->qos || !preorder_ctrl || preorder_ctrl->enable == _FALSE)
		goto _success_exit;

	DBG_COUNTER(padapter->rx_logs.core_rx_post_indicate_reoder);

	_rtw_spinlock_bh(&ppending_recvframe_queue->lock);

	if(rtw_test_and_clear_bit(RTW_RECV_ACK_OR_TIMEOUT, &preorder_ctrl->rec_abba_rsp_ack))
		preorder_ctrl->indicate_seq = 0xFFFF;
	#ifdef DBG_RX_SEQ
	RTW_INFO("DBG_RX_SEQ %s:preorder_ctrl->rec_abba_rsp_ack = %u,indicate_seq = %d\n"
		, __func__
		, preorder_ctrl->rec_abba_rsp_ack
		, preorder_ctrl->indicate_seq);
	#endif

	/* s2. check if winstart_b(indicate_seq) needs to been updated */
	if (!check_indicate_seq(preorder_ctrl, pattrib->seq_num)) {
		precvinfo->dbg_rx_ampdu_drop_count++;
		/* pHTInfo->RxReorderDropCounter++; */
		/* ReturnRFDList(adapter, pRfd); */
		/* _rtw_spinunlock_irq(&ppending_recvframe_queue->lock, sp_flags); */
		/* return _FAIL; */

		#ifdef DBG_RX_DROP_FRAME
		RTW_INFO("DBG_RX_DROP_FRAME "FUNC_ADPT_FMT" check_indicate_seq fail\n"
			, FUNC_ADPT_ARG(padapter));
		#endif
#if 0
		rtw_recv_indicatepkt(padapter, prframe);

		_rtw_spinunlock_bh(&ppending_recvframe_queue->lock);

		goto _success_exit;
#else
		goto _err_exit;
#endif
	}


	/* s3. Insert all packet into Reorder Queue to maintain its ordering. */
	if (!enqueue_reorder_recvframe(preorder_ctrl, prframe)) {
		/* DbgPrint("recv_indicatepkt_reorder, enqueue_reorder_recvframe fail!\n"); */
		/* _rtw_spinunlock_irq(&ppending_recvframe_queue->lock, sp_flags); */
		/* return _FAIL; */
		#ifdef DBG_RX_DROP_FRAME
		RTW_INFO("DBG_RX_DROP_FRAME "FUNC_ADPT_FMT" enqueue_reorder_recvframe fail\n"
			, FUNC_ADPT_ARG(padapter));
		#endif
		goto _err_exit;
	}


	/* s4. */
	/* Indication process. */
	/* After Packet dropping and Sliding Window shifting as above, we can now just indicate the packets */
	/* with the SeqNum smaller than latest WinStart and buffer other packets. */
	/*  */
	/* For Rx Reorder condition: */
	/* 1. All packets with SeqNum smaller than WinStart => Indicate */
	/* 2. All packets with SeqNum larger than or equal to WinStart => Buffer it. */
	/*  */

	/* recv_indicatepkts_in_order(padapter, preorder_ctrl, _TRUE); */
	if (recv_indicatepkts_in_order(padapter, preorder_ctrl, _FALSE) == _TRUE) {
		if (!preorder_ctrl->bReorderWaiting) {
			preorder_ctrl->bReorderWaiting = _TRUE;
			_set_timer(&preorder_ctrl->reordering_ctrl_timer, REORDER_WAIT_TIME);
		}
		_rtw_spinunlock_bh(&ppending_recvframe_queue->lock);
	} else {
		preorder_ctrl->bReorderWaiting = _FALSE;
		_rtw_spinunlock_bh(&ppending_recvframe_queue->lock);
		_cancel_timer_ex(&preorder_ctrl->reordering_ctrl_timer);
	}

	return RTW_RX_HANDLED;

_success_exit:

	return _SUCCESS;

_err_exit:

	_rtw_spinunlock_bh(&ppending_recvframe_queue->lock);

	return _FAIL;
}


void rtw_reordering_ctrl_timeout_handler(void *pcontext)
{
	struct recv_reorder_ctrl *preorder_ctrl = (struct recv_reorder_ctrl *)pcontext;
	_adapter *padapter = NULL;
	_queue *ppending_recvframe_queue = NULL;


	if (preorder_ctrl == NULL)
		return;

	padapter = preorder_ctrl->padapter;
	if (RTW_CANNOT_RUN(adapter_to_dvobj(padapter)))
		return;

	ppending_recvframe_queue = &preorder_ctrl->pending_recvframe_queue;

	/* RTW_INFO("+rtw_reordering_ctrl_timeout_handler()=>\n"); */

	_rtw_spinlock_bh(&ppending_recvframe_queue->lock);

	preorder_ctrl->bReorderWaiting = _FALSE;

	if (recv_indicatepkts_in_order(padapter, preorder_ctrl, _TRUE) == _TRUE)
		_set_timer(&preorder_ctrl->reordering_ctrl_timer, REORDER_WAIT_TIME);

	_rtw_spinunlock_bh(&ppending_recvframe_queue->lock);

}
#endif /* defined(CONFIG_80211N_HT) && defined(CONFIG_RECV_REORDERING_CTRL) */

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 24))
int recv_frame_monitor(_adapter *padapter, union recv_frame *rframe, struct rtw_recv_pkt *rx_req)
{
	int ret = _SUCCESS;

#ifdef CONFIG_WIFI_MONITOR
	struct net_device *ndev = padapter->pnetdev;
	struct sk_buff *pskb = NULL;

	if (rframe == NULL)
		goto exit;

	/* read skb information from recv frame */
	pskb = rframe->u.hdr.pkt;
	pskb->len = rframe->u.hdr.len;
	pskb->data = rframe->u.hdr.rx_data;
	skb_set_tail_pointer(pskb, rframe->u.hdr.len);

	if (ndev->type == ARPHRD_IEEE80211_RADIOTAP) {
		/* fill radiotap header */
		if (rtw_fill_radiotap_hdr(padapter, rframe->u.hdr.adapter_link,
					&rframe->u.hdr.attrib, rx_req, (u8 *)pskb) == _FAIL) {
			ret = _FAIL;
			goto exit;
		}
	}

	/* write skb information to recv frame */
	skb_reset_mac_header(pskb);
	rframe->u.hdr.len = pskb->len;
	rframe->u.hdr.rx_data = pskb->data;
	rframe->u.hdr.rx_head = pskb->head;
	rframe->u.hdr.rx_tail = skb_tail_pointer(pskb);
	rframe->u.hdr.rx_end = skb_end_pointer(pskb);

	if (!RTW_CANNOT_RUN(adapter_to_dvobj(padapter))) {
		/* indicate this recv_frame */
		ret = rtw_recv_monitor(padapter, rframe);
	} else
		ret = _FAIL;

exit:
#endif /* CONFIG_WIFI_MONITOR */

	return ret;
}
#endif

#if 0
static void recv_set_iseq_before_mpdu_process(union recv_frame *rframe, u16 seq_num, const char *caller)
{
#if defined(CONFIG_80211N_HT) && defined(CONFIG_RECV_REORDERING_CTRL)
	struct recv_reorder_ctrl *reorder_ctrl = rframe->u.hdr.preorder_ctrl;

	if (reorder_ctrl) {
		reorder_ctrl->indicate_seq = seq_num;
		#ifdef DBG_RX_SEQ
		RTW_INFO("DBG_RX_SEQ %s("ADPT_FMT")-B tid:%u indicate_seq:%d, seq_num:%d\n"
			, caller, ADPT_ARG(reorder_ctrl->padapter)
			, reorder_ctrl->tid, reorder_ctrl->indicate_seq, seq_num);
		#endif
	}
#endif
}

static void recv_set_iseq_after_mpdu_process(union recv_frame *rframe, u16 seq_num, const char *caller)
{
#if defined(CONFIG_80211N_HT) && defined(CONFIG_RECV_REORDERING_CTRL)
	struct recv_reorder_ctrl *reorder_ctrl = rframe->u.hdr.preorder_ctrl;

	if (reorder_ctrl) {
		reorder_ctrl->indicate_seq = (reorder_ctrl->indicate_seq + 1) % 4096;
		#ifdef DBG_RX_SEQ
		RTW_INFO("DBG_RX_SEQ %s("ADPT_FMT")-A tid:%u indicate_seq:%d, seq_num:%d\n"
			, caller, ADPT_ARG(reorder_ctrl->padapter)
			, reorder_ctrl->tid, reorder_ctrl->indicate_seq, seq_num);
		#endif
	}
#endif
}


#ifdef CONFIG_MP_INCLUDED
int validate_mp_recv_frame(_adapter *adapter, union recv_frame *precv_frame)
{
	int ret = _SUCCESS;
	u8 *ptr = precv_frame->u.hdr.rx_data;
	u8 type, subtype;
	struct mp_priv *pmppriv = &adapter->mppriv;
	struct mp_tx		*pmptx;
	unsigned char	*sa , *da, *bs;
	struct rx_pkt_attrib *pattrib = &precv_frame->u.hdr.attrib;
	u32 i = 0;
	u8 rtk_prefix[]={0x52, 0x65, 0x61, 0x6C, 0x4C, 0x6F, 0x76, 0x65, 0x54, 0x65, 0x6B};
	u8 *prx_data;
	pmptx = &pmppriv->tx;


	if (pmppriv->mplink_brx == _FALSE) {

		u8 bDumpRxPkt = 0;
		type =  GetFrameType(ptr);
		subtype = get_frame_sub_type(ptr); /* bit(7)~bit(2)	 */

		RTW_INFO("hdr len = %d iv_len=%d \n", pattrib->hdrlen , pattrib->iv_len);
		prx_data = ptr + pattrib->hdrlen + pattrib->iv_len;

		for (i = 0; i < precv_frame->u.hdr.len; i++) {
			if (precv_frame->u.hdr.len < (11 + i))
				break;

			if (_rtw_memcmp(prx_data + i, (void *)&rtk_prefix, 11) == _FALSE) {
				bDumpRxPkt = 0;
				RTW_DBG("prx_data = %02X != rtk_prefix[%d] = %02X \n", *(prx_data + i), i , rtk_prefix[i]);
				} else {
				bDumpRxPkt = 1;
				RTW_DBG("prx_data = %02X = rtk_prefix[%d] = %02X \n", *(prx_data + i), i , rtk_prefix[i]);
				break;
				}
		}

		if (bDumpRxPkt == 1) { /* dump all rx packets */
			int i;
			RTW_INFO("############ type:0x%02x subtype:0x%02x #################\n", type, subtype);

			for (i = 0; i < precv_frame->u.hdr.len; i = i + 8)
				RTW_INFO("%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:\n", *(ptr + i),
					*(ptr + i + 1), *(ptr + i + 2) , *(ptr + i + 3) , *(ptr + i + 4), *(ptr + i + 5), *(ptr + i + 6), *(ptr + i + 7));
				RTW_INFO("#############################\n");
				_rtw_memset(pmppriv->mplink_buf, '\0' , sizeof(pmppriv->mplink_buf));
				_rtw_memcpy(pmppriv->mplink_buf, ptr, precv_frame->u.hdr.len);
				pmppriv->mplink_rx_len = precv_frame->u.hdr.len;
				pmppriv->mplink_brx =_TRUE;
		}
	}
	if (pmppriv->bloopback) {
		if (_rtw_memcmp(ptr + 24, pmptx->buf + 24, precv_frame->u.hdr.len - 24) == _FALSE) {
			RTW_INFO("Compare payload content Fail !!!\n");
			ret = _FAIL;
		}
	}
 	if (pmppriv->bSetRxBssid == _TRUE) {

		sa = get_addr2_ptr(ptr);
		da = GetAddr1Ptr(ptr);
		bs = GetAddr3Ptr(ptr);
		type =	GetFrameType(ptr);
		subtype = get_frame_sub_type(ptr); /* bit(7)~bit(2)  */

		if (_rtw_memcmp(bs, adapter->mppriv.network_macaddr, ETH_ALEN) == _FALSE)
			ret = _FAIL;

		RTW_DBG("############ type:0x%02x subtype:0x%02x #################\n", type, subtype);
		RTW_DBG("A2 sa %02X:%02X:%02X:%02X:%02X:%02X \n", *(sa) , *(sa + 1), *(sa+ 2), *(sa + 3), *(sa + 4), *(sa + 5));
		RTW_DBG("A1 da %02X:%02X:%02X:%02X:%02X:%02X \n", *(da) , *(da + 1), *(da+ 2), *(da + 3), *(da + 4), *(da + 5));
		RTW_DBG("A3 bs %02X:%02X:%02X:%02X:%02X:%02X \n --------------------------\n", *(bs) , *(bs + 1), *(bs+ 2), *(bs + 3), *(bs + 4), *(bs + 5));
	}

	if (!adapter->mppriv.bmac_filter)
		return ret;

	if (_rtw_memcmp(get_addr2_ptr(ptr), adapter->mppriv.mac_filter, ETH_ALEN) == _FALSE)
		ret = _FAIL;

	return ret;
}

static sint MPwlanhdr_to_ethhdr(union recv_frame *precvframe)
{
	sint	rmv_len;
	u16 eth_type, len;
	u8	bsnaphdr;
	u8	*psnap_type;
	u8 mcastheadermac[] = {0x01, 0x00, 0x5e};

	struct ieee80211_snap_hdr	*psnap;

	sint ret = _SUCCESS;
	_adapter	*adapter = precvframe->u.hdr.adapter;

	u8	*ptr = get_recvframe_data(precvframe) ; /* point to frame_ctrl field */
	struct rx_pkt_attrib *pattrib = &precvframe->u.hdr.attrib;


	if (pattrib->encrypt)
		recvframe_pull_tail(precvframe, pattrib->icv_len);

	psnap = (struct ieee80211_snap_hdr *)(ptr + pattrib->hdrlen + pattrib->iv_len);
	psnap_type = ptr + pattrib->hdrlen + pattrib->iv_len + SNAP_SIZE;
	/* convert hdr + possible LLC headers into Ethernet header */
	/* eth_type = (psnap_type[0] << 8) | psnap_type[1]; */
	if ((_rtw_memcmp(psnap, rtw_rfc1042_header, SNAP_SIZE) &&
	     (_rtw_memcmp(psnap_type, SNAP_ETH_TYPE_IPX, 2) == _FALSE) &&
	     (_rtw_memcmp(psnap_type, SNAP_ETH_TYPE_APPLETALK_AARP, 2) == _FALSE)) ||
	    /* eth_type != ETH_P_AARP && eth_type != ETH_P_IPX) || */
	    _rtw_memcmp(psnap, rtw_bridge_tunnel_header, SNAP_SIZE)) {
		/* remove RFC1042 or Bridge-Tunnel encapsulation and replace EtherType */
		bsnaphdr = _TRUE;
	} else {
		/* Leave Ethernet header part of hdr and full payload */
		bsnaphdr = _FALSE;
	}

	rmv_len = pattrib->hdrlen + pattrib->iv_len + (bsnaphdr ? SNAP_SIZE : 0);
	len = precvframe->u.hdr.len - rmv_len;


	_rtw_memcpy(&eth_type, ptr + rmv_len, 2);
	eth_type = ntohs((unsigned short)eth_type); /* pattrib->ether_type */
	pattrib->eth_type = eth_type;

	{
		ptr = recvframe_pull(precvframe, (rmv_len - sizeof(struct ethhdr) + (bsnaphdr ? 2 : 0)));
	}

	_rtw_memcpy(ptr, pattrib->dst, ETH_ALEN);
	_rtw_memcpy(ptr + ETH_ALEN, pattrib->src, ETH_ALEN);

	if (!bsnaphdr) {
		len = htons(len);
		_rtw_memcpy(ptr + 12, &len, 2);
	}


	len = htons(pattrib->seq_num);
	/* RTW_INFO("wlan seq = %d ,seq_num =%x\n",len,pattrib->seq_num); */
	_rtw_memcpy(ptr + 12, &len, 2);
	if (adapter->mppriv.bRTWSmbCfg == _TRUE) {
		/* if(_rtw_memcmp(mcastheadermac, pattrib->dst, 3) == _TRUE) */ /* SimpleConfig Dest. */
		/*			_rtw_memcpy(ptr+ETH_ALEN, pattrib->bssid, ETH_ALEN); */

		if (_rtw_memcmp(mcastheadermac, pattrib->bssid, 3) == _TRUE) /* SimpleConfig Dest. */
			_rtw_memcpy(ptr, pattrib->bssid, ETH_ALEN);

	}


	return ret;

}

int mp_recv_frame(_adapter *padapter, union recv_frame *rframe)
{
	int ret = _SUCCESS;
	struct rx_pkt_attrib *pattrib = &rframe->u.hdr.attrib;
#ifdef CONFIG_MP_INCLUDED
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	struct mp_priv *pmppriv = &padapter->mppriv;
#endif /* CONFIG_MP_INCLUDED */
	u8 type;
	u8 *ptr = rframe->u.hdr.rx_data;
	u8 *psa, *pda, *pbssid;
	struct sta_info *psta = NULL;
	DBG_COUNTER(padapter->rx_logs.core_rx_pre);

	if ((check_fwstate(pmlmepriv, WIFI_ASOC_STATE) == _TRUE)) { /* &&(padapter->mppriv.check_mp_pkt == 0)) */
		if (pattrib->crc_err == 1)
			padapter->mppriv.rx_crcerrpktcount++;
		else {
			if (_SUCCESS == validate_mp_recv_frame(padapter, rframe))
				padapter->mppriv.rx_pktcount++;
			else
				padapter->mppriv.rx_pktcount_filter_out++;
		}

		if (pmppriv->rx_bindicatePkt == _FALSE) {
			ret = _FAIL;
			rtw_free_recvframe(rframe);/* free this recv_frame */
			goto exit;
		} else {
			type =	GetFrameType(ptr);
			pattrib->to_fr_ds = get_tofr_ds(ptr);
			pattrib->frag_num = GetFragNum(ptr);
			pattrib->seq_num = GetSequence(ptr);
			pattrib->pw_save = GetPwrMgt(ptr);
			pattrib->mfrag = GetMFrag(ptr);
			pattrib->mdata = GetMData(ptr);
			pattrib->privacy = GetPrivacy(ptr);
			pattrib->order = GetOrder(ptr);

			if (type == WIFI_DATA_TYPE) {
				pda = get_da(ptr);
				psa = get_sa(ptr);
				pbssid = get_hdr_bssid(ptr);

				_rtw_memcpy(pattrib->dst, pda, ETH_ALEN);
				_rtw_memcpy(pattrib->src, psa, ETH_ALEN);
				_rtw_memcpy(pattrib->bssid, pbssid, ETH_ALEN);

				switch (pattrib->to_fr_ds) {
				case 0:
					_rtw_memcpy(pattrib->ra, pda, ETH_ALEN);
					_rtw_memcpy(pattrib->ta, psa, ETH_ALEN);
					ret = sta2sta_data_frame(padapter, rframe, &psta);
					break;

				case 1:
					_rtw_memcpy(pattrib->ra, pda, ETH_ALEN);
					_rtw_memcpy(pattrib->ta, pbssid, ETH_ALEN);
					ret = ap2sta_data_frame(padapter, rframe, &psta);
					break;

				case 2:
					_rtw_memcpy(pattrib->ra, pbssid, ETH_ALEN);
					_rtw_memcpy(pattrib->ta, psa, ETH_ALEN);
					ret = sta2ap_data_frame(padapter, rframe, &psta);
					break;
				case 3:
					_rtw_memcpy(pattrib->ra, GetAddr1Ptr(ptr), ETH_ALEN);
					_rtw_memcpy(pattrib->ta, get_addr2_ptr(ptr), ETH_ALEN);
					ret = _FAIL;
					break;
				default:
					ret = _FAIL;
					break;
				}

				if (ret != _SUCCESS) {
#ifdef DBG_RX_DROP_FRAME
					RTW_INFO("DBG_RX_DROP_FRAME "FUNC_ADPT_FMT" 2_data_frame fail: drop pkt\n"
															, FUNC_ADPT_ARG(padapter));
#endif
					ret = _FAIL;
					goto exit;
				}

				ret = MPwlanhdr_to_ethhdr(rframe);

				if (ret != _SUCCESS) {
					#ifdef DBG_RX_DROP_FRAME
					RTW_INFO("DBG_RX_DROP_FRAME "FUNC_ADPT_FMT" wlanhdr_to_ethhdr: drop pkt\n"
						, FUNC_ADPT_ARG(padapter));
					#endif
					ret = _FAIL;
					goto exit;
				}
				if (!RTW_CANNOT_RUN(adapter_to_dvobj(padapter))) {
					/* indicate this recv_frame */
					ret = rtw_recv_indicatepkt(padapter, rframe);
					if (ret != _SUCCESS) {
						#ifdef DBG_RX_DROP_FRAME
						RTW_INFO("DBG_RX_DROP_FRAME "FUNC_ADPT_FMT" rtw_recv_indicatepkt fail!\n"
							, FUNC_ADPT_ARG(padapter));
						#endif
						ret = _FAIL;
						goto exit;
					}
				} else {
					#ifdef DBG_RX_DROP_FRAME
					RTW_INFO("DBG_RX_DROP_FRAME "FUNC_ADPT_FMT" bDriverStopped(%s) OR bSurpriseRemoved(%s)\n"
						, FUNC_ADPT_ARG(padapter)
						, dev_is_drv_stopped(adapter_to_dvobj(padapter)) ? "True" : "False"
						, dev_is_surprise_removed(adapter_to_dvobj(padapter)) ? "True" : "False");
					#endif
					ret = _FAIL;
					goto exit;
				}

			}
		}
	}
exit:
	rtw_free_recvframe(rframee);/* free this recv_frame */
	ret = _FAIL;
	return ret;

}
#endif

int recv_func_prehandle(_adapter *padapter, union recv_frame *rframe)
{
	int ret = _SUCCESS;
#ifdef DBG_RX_COUNTER_DUMP
	struct rx_pkt_attrib *pattrib = &rframe->u.hdr.attrib;
#endif
	_queue *pfree_recv_queue = &padapter->recvpriv.free_recv_queue;

#ifdef DBG_RX_COUNTER_DUMP
	if (padapter->dump_rx_cnt_mode & DUMP_DRV_RX_COUNTER) {
		if (pattrib->crc_err == 1)
			padapter->drv_rx_cnt_crcerror++;
		else
			padapter->drv_rx_cnt_ok++;
	}
#endif

#ifdef CONFIG_MP_INCLUDED
	if (padapter->registrypriv.mp_mode == 1 || padapter->mppriv.bRTWSmbCfg == _TRUE) {
		mp_recv_frame(padapter, rframe);
		ret = _FAIL;
		goto exit;
	} else
#endif
	{
		/* check the frame crtl field and decache */
		ret = validate_recv_frame(padapter, rframe);
		if (ret != _SUCCESS) {
			rtw_free_recvframe(rframee);/* free this recv_frame */
			goto exit;
		}
	}
exit:
	return ret;
}

/*#define DBG_RX_BMC_FRAME*/
int recv_func_posthandle(_adapter *padapter, union recv_frame *prframe)
{
	int ret = _SUCCESS;
	union recv_frame *orig_prframe = prframe;
	struct rx_pkt_attrib *pattrib = &prframe->u.hdr.attrib;
	struct recv_info *precvinfo = &padapter->recvinfo;
	_queue *pfree_recv_queue = &padapter->recvpriv.free_recv_queue;
#ifdef CONFIG_TDLS
	u8 *psnap_type, *pcategory;
#endif /* CONFIG_TDLS */

	DBG_COUNTER(padapter->rx_logs.core_rx_post);

	prframe = decryptor(padapter, prframe);
	if (prframe == NULL) {
		#ifdef DBG_RX_DROP_FRAME
		RTW_INFO("DBG_RX_DROP_FRAME "FUNC_ADPT_FMT" decryptor: drop pkt\n"
			, FUNC_ADPT_ARG(padapter));
		#endif
		ret = _FAIL;
		DBG_COUNTER(padapter->rx_logs.core_rx_post_decrypt_err);
		goto _recv_data_drop;
	}

#ifdef DBG_RX_BMC_FRAME
	if (IS_MCAST(pattrib->ra))
		RTW_INFO("%s =>"ADPT_FMT" Rx BC/MC from "MAC_FMT"\n", __func__, ADPT_ARG(padapter), MAC_ARG(pattrib->ta));
#endif

#if 0
	if (is_primary_adapter(padapter)) {
		RTW_INFO("+++\n");
		{
			int i;
			u8	*ptr = get_recvframe_data(prframe);
			for (i = 0; i < 140; i = i + 8)
				RTW_INFO("%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:", *(ptr + i),
					*(ptr + i + 1), *(ptr + i + 2) , *(ptr + i + 3) , *(ptr + i + 4), *(ptr + i + 5), *(ptr + i + 6), *(ptr + i + 7));

		}
		RTW_INFO("---\n");
	}
#endif

#ifdef CONFIG_TDLS
	/* check TDLS frame */
	psnap_type = get_recvframe_data(orig_prframe) + pattrib->hdrlen + pattrib->iv_len + SNAP_SIZE;
	pcategory = psnap_type + ETH_TYPE_LEN + PAYLOAD_TYPE_LEN;

	if ((_rtw_memcmp(psnap_type, SNAP_ETH_TYPE_TDLS, ETH_TYPE_LEN)) &&
	    ((*pcategory == RTW_WLAN_CATEGORY_TDLS) || (*pcategory == RTW_WLAN_CATEGORY_P2P))) {
		ret = OnTDLS(padapter, prframe);
		if (ret == _FAIL)
			goto _exit_recv_func;
	}
#endif /* CONFIG_TDLS */

	prframe = recvframe_chk_defrag(padapter, prframe);
	if (prframe == NULL)	{
		#ifdef DBG_RX_DROP_FRAME
		RTW_INFO("DBG_RX_DROP_FRAME "FUNC_ADPT_FMT" recvframe_chk_defrag: drop pkt\n"
			, FUNC_ADPT_ARG(padapter));
		#endif
		DBG_COUNTER(padapter->rx_logs.core_rx_post_defrag_err);
		goto _recv_data_drop;
	}

	prframe = portctrl(padapter, prframe);
	if (prframe == NULL) {
		#ifdef DBG_RX_DROP_FRAME
		RTW_INFO("DBG_RX_DROP_FRAME "FUNC_ADPT_FMT" portctrl: drop pkt\n"
			, FUNC_ADPT_ARG(padapter));
		#endif
		ret = _FAIL;
		DBG_COUNTER(padapter->rx_logs.core_rx_post_portctrl_err);
		goto _recv_data_drop;
	}

	count_rx_stats(padapter, prframe, NULL);

#ifdef CONFIG_WAPI_SUPPORT
	rtw_wapi_update_info(padapter, prframe);
#endif

#if defined(CONFIG_80211N_HT) && defined(CONFIG_RECV_REORDERING_CTRL)
	/* including perform A-MPDU Rx Ordering Buffer Control */
	ret = recv_indicatepkt_reorder(padapter, prframe);
	if (ret == _FAIL) {
		rtw_free_recvframe(orig_prframe);
		goto _recv_data_drop;
	} else if (ret == RTW_RX_HANDLED) /* queued OR indicated in order */
		goto _exit_recv_func;
#endif

	recv_set_iseq_before_mpdu_process(prframe, pattrib->seq_num, __func__);
	ret = recv_process_mpdu(padapter, prframe);
	recv_set_iseq_after_mpdu_process(prframe, pattrib->seq_num, __func__);
	if (ret == _FAIL)
		goto _recv_data_drop;

_exit_recv_func:
	return ret;

_recv_data_drop:
	precvinfo->dbg_rx_drop_count++;
	return ret;
}

int recv_func(_adapter *padapter, union recv_frame *rframe)
{
	int ret;
	struct rx_pkt_attrib *prxattrib = &rframe->u.hdr.attrib;
	struct recv_priv *recvpriv = &padapter->recvpriv;
	struct security_priv *psecuritypriv = &padapter->securitypriv;
	struct mlme_priv *mlmepriv = &padapter->mlmepriv;
#ifdef CONFIG_CUSTOMER_ALIBABA_GENERAL
	u8 type;
	u8 *ptr = rframe->u.hdr.rx_data;
#endif

	if (check_fwstate(mlmepriv, WIFI_MONITOR_STATE)) {
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 24))
		recv_frame_monitor(padapter, rframe);
#endif
		ret = _SUCCESS;
		goto exit;
	}

#ifdef CONFIG_CUSTOMER_ALIBABA_GENERAL
	type = GetFrameType(ptr);
	if ((type == WIFI_DATA_TYPE)&& MLME_IS_STA(padapter)) {
		struct wlan_network *cur_network = &(mlmepriv->cur_network);
		if ( _rtw_memcmp(get_addr2_ptr(ptr), cur_network->network.MacAddress, ETH_ALEN)==0) {
			recv_frame_monitor(padapter, rframe);
			ret = _SUCCESS;
			goto exit;
		}
	}
#endif
	/* check if need to handle uc_swdec_pending_queue*/
	if (MLME_IS_STA(padapter) && psecuritypriv->busetkipkey) {
		union recv_frame *pending_frame;
		int cnt = 0;

		while ((pending_frame = rtw_alloc_recvframe(&recvpriv->uc_swdec_pending_queue))) {
			cnt++;
			DBG_COUNTER(padapter->rx_logs.core_rx_dequeue);
			recv_func_posthandle(padapter, pending_frame);
		}

		if (cnt)
			RTW_INFO(FUNC_ADPT_FMT" dequeue %d from uc_swdec_pending_queue\n",
				 FUNC_ADPT_ARG(padapter), cnt);
	}

	DBG_COUNTER(padapter->rx_logs.core_rx);
	ret = recv_func_prehandle(padapter, rframe);

	if (ret == _SUCCESS) {

		/* check if need to enqueue into uc_swdec_pending_queue*/
		if (MLME_IS_STA(padapter) &&
		    !IS_MCAST(prxattrib->ra) && prxattrib->encrypt > 0 &&
		    (prxattrib->bdecrypted == 0 || psecuritypriv->sw_decrypt == _TRUE) &&
		    psecuritypriv->ndisauthtype == Ndis802_11AuthModeWPAPSK &&
		    !psecuritypriv->busetkipkey) {
			DBG_COUNTER(padapter->rx_logs.core_rx_enqueue);
			rtw_enqueue_recvframe(rframe, &recvpriv->uc_swdec_pending_queue);
			/* RTW_INFO("%s: no key, enqueue uc_swdec_pending_queue\n", __func__); */

			if (recvpriv->free_recvframe_cnt < NR_RECVFRAME / 4) {
				/* to prevent from recvframe starvation, get recvframe from uc_swdec_pending_queue to free_recvframe_cnt */
				rframe = rtw_alloc_recvframe(&padapter->recvpriv.uc_swdec_pending_queue);
				if (rframe)
					goto do_posthandle;
			}
			goto exit;
		}

do_posthandle:
		ret = recv_func_posthandle(padapter, rframe);
	}

exit:
	return ret;
}

s32 rtw_recv_entry(union recv_frame *precvframe)
{
	_adapter *padapter;
	struct recv_info *precvinfo;
	s32 ret = _SUCCESS;



	padapter = precvframe->u.hdr.adapter;

	precvinfo = &padapter->recvinfo;


	ret = recv_func(padapter, precvframe);
	if (ret == _FAIL) {
		goto _recv_entry_drop;
	}


	precvinfo->rx_pkts++;


	return ret;

_recv_entry_drop:

#ifdef CONFIG_MP_INCLUDED
	if (padapter->registrypriv.mp_mode == 1)
		padapter->mppriv.rx_pktloss = precvinfo->rx_drop;
#endif



	return ret;
}
#endif

#ifdef CONFIG_SIGNAL_STAT_PROCESS
static void rtw_signal_stat_timer_hdl(void *ctx)
{
	_adapter *adapter = (_adapter *)ctx;
	struct recv_info *recvinfo = &adapter->recvinfo;
	/* ToDo CONFIG_RTW_MLD: currently primary link only */
	struct _ADAPTER_LINK *adapter_link = GET_PRIMARY_LINK(adapter);

	u32 tmp_s, tmp_q;
	u8 avg_signal_strength = 0;
	u8 avg_signal_qual = 0;
	u32 num_signal_strength = 0;
	u32 num_signal_qual = 0;
	u8 ratio_pre_stat = 0, ratio_curr_stat = 0, ratio_total = 0, ratio_profile = SIGNAL_STAT_CALC_PROFILE_0;

	if (recvinfo->is_signal_dbg) {
		/* update the user specific value, signal_strength_dbg, to signal_strength, rssi */
		recvinfo->signal_strength = recvinfo->signal_strength_dbg;
		recvinfo->rssi = (s8)rtw_phl_rssi_to_dbm((u8)recvinfo->signal_strength_dbg);
	} else {

		if (recvinfo->signal_strength_data.update_req == 0) { /* update_req is clear, means we got rx */
			avg_signal_strength = recvinfo->signal_strength_data.avg_val;
			num_signal_strength = recvinfo->signal_strength_data.total_num;
			/* after avg_vals are accquired, we can re-stat the signal values */
			recvinfo->signal_strength_data.update_req = 1;
		}

		if (recvinfo->signal_qual_data.update_req == 0) { /* update_req is clear, means we got rx */
			avg_signal_qual = recvinfo->signal_qual_data.avg_val;
			num_signal_qual = recvinfo->signal_qual_data.total_num;
			/* after avg_vals are accquired, we can re-stat the signal values */
			recvinfo->signal_qual_data.update_req = 1;
		}

		if (num_signal_strength == 0) {
			if (rtw_get_on_cur_ch_time(adapter) == 0
			    || rtw_get_passing_time_ms(rtw_get_on_cur_ch_time(adapter)) < 2 * adapter_link->mlmeextpriv.mlmext_info.bcn_interval
			   )
				goto set_timer;
		}

		if (check_fwstate(&adapter->mlmepriv, WIFI_UNDER_SURVEY) == _TRUE
		    || check_fwstate(&adapter->mlmepriv, WIFI_ASOC_STATE) == _FALSE
		   )
			goto set_timer;

#ifdef CONFIG_CONCURRENT_MODE
		if (rtw_mi_buddy_check_fwstate(adapter, WIFI_UNDER_SURVEY) == _TRUE)
			goto set_timer;
#endif

		if (RTW_SIGNAL_STATE_CALC_PROFILE < SIGNAL_STAT_CALC_PROFILE_MAX)
			ratio_profile = RTW_SIGNAL_STATE_CALC_PROFILE;

		ratio_pre_stat = signal_stat_calc_profile[ratio_profile][0];
		ratio_curr_stat = signal_stat_calc_profile[ratio_profile][1];
		ratio_total = ratio_pre_stat + ratio_curr_stat;

		/* update value of signal_strength, rssi, signal_qual */
		tmp_s = (ratio_curr_stat * avg_signal_strength + ratio_pre_stat * recvinfo->signal_strength);
		if (tmp_s % ratio_total)
			tmp_s = tmp_s / ratio_total + 1;
		else
			tmp_s = tmp_s / ratio_total;
		if (tmp_s > PHL_MAX_RSSI)
			tmp_s = PHL_MAX_RSSI;

		tmp_q = (ratio_curr_stat * avg_signal_qual + ratio_pre_stat * recvinfo->signal_qual);
		if (tmp_q % ratio_total)
			tmp_q = tmp_q / ratio_total + 1;
		else
			tmp_q = tmp_q / ratio_total;
		if (tmp_q > PHL_MAX_RSSI)
			tmp_q = PHL_MAX_RSSI;

		recvinfo->signal_strength = tmp_s;
		recvinfo->rssi = (s8)rtw_phl_rssi_to_dbm(tmp_s);
		recvinfo->signal_qual = tmp_q;

#if defined(DBG_RX_SIGNAL_DISPLAY_PROCESSING) && 1
		RTW_INFO(FUNC_ADPT_FMT" signal_strength:%3u, rssi:%3d, signal_qual:%3u"
			 ", num_signal_strength:%u, num_signal_qual:%u"
			 ", on_cur_ch_ms:%d"
			 "\n"
			 , FUNC_ADPT_ARG(adapter)
			 , recvinfo->signal_strength
			 , recvinfo->rssi
			 , recvinfo->signal_qual
			 , num_signal_strength, num_signal_qual
			, rtw_get_on_cur_ch_time(adapter) ? rtw_get_passing_time_ms(rtw_get_on_cur_ch_time(adapter)) : 0
			);
#endif
	}

set_timer:
	rtw_set_signal_stat_timer(recvinfo);

}
#endif/*CONFIG_SIGNAL_STAT_PROCESS*/

/*
* Increase and check if the continual_no_rx_packet of this @param pmlmepriv is larger than MAX_CONTINUAL_NORXPACKET_COUNT
* @return _TRUE:
* @return _FALSE:
*/
int rtw_inc_and_chk_continual_no_rx_packet(struct sta_info *sta, int tid_index)
{

	int ret = _FALSE;
	int value = ATOMIC_INC_RETURN(&sta->continual_no_rx_packet[tid_index]);

	if (value >= MAX_CONTINUAL_NORXPACKET_COUNT)
		ret = _TRUE;

	return ret;
}

/*
* Set the continual_no_rx_packet of this @param pmlmepriv to 0
*/
void rtw_reset_continual_no_rx_packet(struct sta_info *sta, int tid_index)
{
	ATOMIC_SET(&sta->continual_no_rx_packet[tid_index], 0);
}

u8 adapter_allow_bmc_data_rx(_adapter *adapter)
{
	if (check_fwstate(&adapter->mlmepriv, WIFI_MONITOR_STATE | WIFI_MP_STATE) == _TRUE)
		return 1;

	if (MLME_IS_AP(adapter))
		return 0;

	if (rtw_linked_check(adapter) == _FALSE)
		return 0;

	return 1;
}
#if 0
s32 pre_recv_entry(union recv_frame *precvframe, u8 *pphy_status)
{
	s32 ret = _SUCCESS;
	u8 *pbuf = precvframe->u.hdr.rx_data;
	u8 *pda = get_ra(pbuf);
	u8 ra_is_bmc = IS_MCAST(pda);
	_adapter *primary_padapter = precvframe->u.hdr.adapter;
#ifdef CONFIG_CONCURRENT_MODE
	_adapter *iface = NULL;

	#ifdef CONFIG_MP_INCLUDED
	if (rtw_mp_mode_check(primary_padapter))
		goto bypass_concurrent_hdl;
	#endif

	if (ra_is_bmc == _FALSE) { /*unicast packets*/
		iface = rtw_get_iface_by_macddr(primary_padapter , pda);
		if (NULL == iface) {
			#ifdef CONFIG_RTW_CFGVENDOR_RANDOM_MAC_OUI
			if (_rtw_memcmp(pda, adapter_pno_mac_addr(primary_padapter),
					ETH_ALEN) != _TRUE)
			#endif
			RTW_INFO("%s [WARN] Cannot find appropriate adapter - mac_addr : "MAC_FMT"\n", __func__, MAC_ARG(pda));
			/*rtw_warn_on(1);*/
		} else {
			precvframe->u.hdr.adapter = iface;
		}
	} else { /* Handle BC/MC Packets*/
		rtw_mi_buddy_clone_bcmc_packet(primary_padapter, precvframe, pphy_status);
	}
#ifdef CONFIG_MP_INCLUDED
bypass_concurrent_hdl:
#endif
#endif /* CONFIG_CONCURRENT_MODE */
	if (primary_padapter->registrypriv.mp_mode != 1) {
		/* skip unnecessary bmc data frame for primary adapter */
		if (ra_is_bmc == _TRUE && GetFrameType(pbuf) == WIFI_DATA_TYPE
			&& !adapter_allow_bmc_data_rx(precvframe->u.hdr.adapter)
		) {
			rtw_free_recvframe(precvframe);
			goto exit;
		}
	}

	if (pphy_status) {
		rx_query_phy_status(precvframe, pphy_status);

#ifdef CONFIG_WIFI_MONITOR
		rx_query_moinfo(&precvframe->u.hdr.attrib, pphy_status);
#endif
	}
	ret = rtw_recv_entry(precvframe);

exit:
	return ret;
}
#endif


#ifdef CONFIG_RECV_THREAD_MODE
thread_return rtw_recv_thread(thread_context context)
{
	_adapter *adapter = (_adapter *)context;
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	struct recv_priv *recvpriv = &dvobj->recvpriv;
	s32 err = _SUCCESS;
#ifdef RTW_RECV_THREAD_HIGH_PRIORITY
#ifdef PLATFORM_LINUX
	struct sched_param param = { .sched_priority = 1 };

	sched_setscheduler(current, SCHED_FIFO, &param);
#endif /* PLATFORM_LINUX */
#endif /*RTW_RECV_THREAD_HIGH_PRIORITY*/
	rtw_thread_enter("RTW_RECV_THREAD");

	RTW_INFO(FUNC_ADPT_FMT" enter\n", FUNC_ADPT_ARG(adapter));

	do {
		err = _rtw_down_sema(&recvpriv->recv_sema);
		if (_FAIL == err) {
			RTW_ERR(FUNC_ADPT_FMT" down recv_sema fail!\n", FUNC_ADPT_ARG(adapter));
			goto exit;
		}

		if (RTW_CANNOT_RUN(adapter_to_dvobj(adapter))) {
			RTW_DBG(FUNC_ADPT_FMT "- bDriverStopped(%s) bSurpriseRemoved(%s)\n",
				FUNC_ADPT_ARG(adapter),
				dev_is_drv_stopped(adapter_to_dvobj(adapter)) ? "True" : "False",
				dev_is_surprise_removed(adapter_to_dvobj(adapter)) ? "True" : "False");
			goto exit;
		}

		err = rtw_intf_recv_hdl(adapter);

		if (err == RTW_RFRAME_UNAVAIL
			|| err == RTW_RFRAME_PKT_UNAVAIL
		) {
			rtw_msleep_os(1);
			_rtw_up_sema(&recvpriv->recv_sema);
		}

		flush_signals_thread();

	} while (err != _FAIL);

exit:

	RTW_INFO(FUNC_ADPT_FMT " Exit\n", FUNC_ADPT_ARG(adapter));

	rtw_thread_wait_stop();

	return 0;
}
#endif /* CONFIG_RECV_THREAD_MODE */

#ifdef CONFIG_USB_HCI
u8 rtw_init_lite_recv_resource(struct dvobj_priv *dvobj)
{
	u8 ret = _SUCCESS;
	u32 literecvbuf_nr = RTW_LITERECVBUF_NR;
	struct lite_data_buf *literecvbuf;
	struct trx_data_buf_q  *literecvbuf_q = &dvobj->literecvbuf_q;
	int i;
	struct data_urb *recvurb;
	struct trx_urb_buf_q *recv_urb_q = &dvobj->recv_urb_q;
	u32 recvurb_nr = RTW_RECVURB_NR;
#ifdef CONFIG_USB_INTERRUPT_IN_PIPE
	struct lite_data_buf *intinbuf;
	struct trx_data_buf_q  *intin_buf_q = &dvobj->intin_buf_q;
	u32 intin_buf_nr = RTW_INTINBUF_NR;
	struct data_urb *intin_urb;
	struct trx_urb_buf_q *intin_urb_q = &dvobj->intin_urb_q;
	u32 intin_urb_nr = RTW_INTINURB_NR;
#endif 

	/* init lite_recv_buf */
	_rtw_init_queue(&literecvbuf_q->free_data_buf_queue);

	literecvbuf_q->alloc_data_buf =
		rtw_zvmalloc(literecvbuf_nr * sizeof(struct lite_data_buf) + 4);

	if (literecvbuf_q->alloc_data_buf  == NULL) {
		ret = _FAIL;
		goto exit;
	}

	literecvbuf_q->data_buf=
	(u8 *)N_BYTE_ALIGNMENT((SIZE_PTR)(literecvbuf_q->alloc_data_buf), 4);

	literecvbuf = (struct lite_data_buf *)literecvbuf_q->data_buf;

	for (i = 0; i < literecvbuf_nr; i++) {
		_rtw_init_listhead(&literecvbuf->list);
		rtw_list_insert_tail(&literecvbuf->list,
			&(literecvbuf_q->free_data_buf_queue.queue));
		literecvbuf++;
	}
	literecvbuf_q->free_data_buf_cnt = literecvbuf_nr;

	/* init recv_urb */
	_rtw_init_queue(&recv_urb_q->free_urb_buf_queue);
	recv_urb_q->alloc_urb_buf=
		rtw_zvmalloc(recvurb_nr * sizeof(struct data_urb) + 4);
	if (recv_urb_q->alloc_urb_buf== NULL) {
		ret = _FAIL;
		goto exit;
	}

	recv_urb_q->urb_buf =
		(u8 *)N_BYTE_ALIGNMENT((SIZE_PTR)(recv_urb_q->alloc_urb_buf), 4);

	recvurb = (struct data_urb *)recv_urb_q->urb_buf;
	for (i = 0; i < recvurb_nr; i++) {
		_rtw_init_listhead(&recvurb->list);
		ret = rtw_os_urb_resource_alloc(recvurb);
		rtw_list_insert_tail(&recvurb->list,
			&(recv_urb_q->free_urb_buf_queue.queue));
		recvurb++;
	}
	recv_urb_q->free_urb_buf_cnt = recvurb_nr;
	ATOMIC_SET(&(dvobj->rx_pending_cnt), 0);

#ifdef CONFIG_USB_INTERRUPT_IN_PIPE

	/* init int_in_buf */
	_rtw_init_queue(&intin_buf_q->free_data_buf_queue);

	intin_buf_q->alloc_data_buf =
		rtw_zvmalloc(intin_buf_nr * sizeof(struct lite_data_buf) + 4);

	if (intin_buf_q->alloc_data_buf  == NULL) {
		ret = _FAIL;
		goto exit;
	}

	intin_buf_q->data_buf=
	(u8 *)N_BYTE_ALIGNMENT((SIZE_PTR)(intin_buf_q->alloc_data_buf), 4);

	intinbuf = (struct lite_data_buf *)intin_buf_q->data_buf;

	for (i = 0; i < intin_buf_nr; i++) {
		_rtw_init_listhead(&intinbuf->list);
		rtw_list_insert_tail(&intinbuf->list,
			&(intin_buf_q->free_data_buf_queue.queue));
		intinbuf++;
	}
	intin_buf_q->free_data_buf_cnt = intin_buf_nr;

	/* init int_in_urb */
	_rtw_init_queue(&intin_urb_q->free_urb_buf_queue);
	intin_urb_q->alloc_urb_buf=
		rtw_zvmalloc(intin_urb_nr * sizeof(struct data_urb) + 4);
	if (intin_urb_q->alloc_urb_buf== NULL) {
		ret = _FAIL;
		goto exit;
	}

	intin_urb_q->urb_buf =
		(u8 *)N_BYTE_ALIGNMENT((SIZE_PTR)(intin_urb_q->alloc_urb_buf), 4);

	intin_urb = (struct data_urb *)intin_urb_q->urb_buf;
	for (i = 0; i < intin_urb_nr; i++) {
		_rtw_init_listhead(&intin_urb->list);
		ret = rtw_os_urb_resource_alloc(intin_urb);
		rtw_list_insert_tail(&intin_urb->list,
			&(intin_urb_q->free_urb_buf_queue.queue));
		intin_urb++;
	}
	intin_urb_q->free_urb_buf_cnt = intin_urb_nr;
#endif

exit:
	return ret;
}

void rtw_free_lite_recv_resource(struct dvobj_priv *dvobj)
{
	u8 ret = _SUCCESS;
	u32 literecvbuf_nr = RTW_LITERECVBUF_NR;
	struct lite_data_buf *literecvbuf;
	struct trx_data_buf_q  *literecvbuf_q = &dvobj->literecvbuf_q;
	int i;
	struct data_urb *recvurb;
	struct trx_urb_buf_q *recv_urb_q = &dvobj->recv_urb_q;
	u32 recvurb_nr = RTW_RECVURB_NR;
#ifdef CONFIG_USB_INTERRUPT_IN_PIPE
	struct lite_data_buf *intinbuf;
	struct trx_data_buf_q *intin_buf_q = &dvobj->intin_buf_q;
	u32 intin_buf_nr = RTW_INTINBUF_NR;
	struct data_urb *intin_urb;
	struct trx_urb_buf_q *intin_urb_q = &dvobj->intin_urb_q;
	u32 intin_urb_nr = RTW_INTINURB_NR;
#endif 

	if (literecvbuf_q->alloc_data_buf)
		rtw_vmfree(literecvbuf_q->alloc_data_buf,
			literecvbuf_nr * sizeof(struct lite_data_buf) + 4);

	recvurb = (struct data_urb *)recv_urb_q->urb_buf;
	for (i = 0; i < recvurb_nr; i++) {
		rtw_os_urb_resource_free(recvurb);
		recvurb++;
	}

	if (recv_urb_q->alloc_urb_buf)
		rtw_vmfree(recv_urb_q->alloc_urb_buf,
			recvurb_nr * sizeof(struct data_urb) + 4);

#ifdef CONFIG_USB_INTERRUPT_IN_PIPE
	if (intin_buf_q->alloc_data_buf)
		rtw_vmfree(intin_buf_q->alloc_data_buf,
			intin_buf_nr * sizeof(struct lite_data_buf) + 4);

	intin_urb = (struct data_urb *)intin_urb_q->urb_buf;
	for (i = 0; i < intin_urb_nr; i++) {
		rtw_os_urb_resource_free(intin_urb);
		intin_urb++;
	}

	if (intin_urb_q->alloc_urb_buf)
		rtw_vmfree(intin_urb_q->alloc_urb_buf,
			intin_urb_nr * sizeof(struct data_urb) + 4);
#endif
}
#endif

#ifdef RTW_PHL_RX
void rx_dump_skb(struct sk_buff *skb)
{
	int idx=0;
	u8 *tmp=skb->data;
	printk("===");
	printk("[%s]skb=%p len=%d\n", __FUNCTION__, skb, skb->len);
	
#if 0	
	printk("data-tail=0x%x-0x%x(%d)\n", 
		skb->data, skb->tail, (skb->tail - skb->data));
	printk("head-end=0x%x-0x%x(%d)\n", 
		skb->head, skb->end, (skb->end - skb->head));
#endif
	
	for(idx=0; idx<skb->len; idx++){
		printk("%02x ", tmp[idx]);
		if(idx%20==19)
			printk("\n");
	}
	printk("\n===\n");
}

void dump_rxreq(_adapter *adapter, union recv_frame *prframe)
{


}

void dump_recv_frame(_adapter *adapter, union recv_frame *prframe)
{
	struct recv_frame_hdr *hdr = &(prframe->u.hdr);
	struct rx_pkt_attrib *rxattr = &(prframe->u.hdr.attrib);
	
	printk("[%s]prframe=0x%p len=%d\n", __FUNCTION__, prframe, hdr->len);

	printk("head-tail=0x%p-0x%p\n", hdr->rx_head, hdr->rx_tail);
	printk("data-end=0x%p-0x%p\n", hdr->rx_data, hdr->rx_end);

	printk("dst=%pM\n", rxattr->dst);
	printk("src=%pM\n", rxattr->src);
	printk("ra=%pM\n", rxattr->ra);
	printk("ta=%pM\n", rxattr->ta);
	printk("bssid=%pM\n", rxattr->bssid);
}

void core_update_recvframe_pkt( union recv_frame *prframe, struct rtw_recv_pkt *rx_req)
{
	struct rtw_pkt_buf_list *pkt = rx_req->pkt_list;
	struct sk_buff *skb = prframe->u.hdr.pkt;

	skb_reserve(skb, pkt->vir_addr - skb->data);
	skb_put(skb, pkt->length);

	prframe->u.hdr.rx_data = skb->data;
	prframe->u.hdr.rx_tail = skb_tail_pointer(skb);
	prframe->u.hdr.len = skb->len;
	prframe->u.hdr.rx_head = skb->head;
	prframe->u.hdr.rx_end = skb_end_pointer(skb);

	return;
}

static int core_alloc_recvframe_pkt(union recv_frame *prframe,
				    struct rtw_recv_pkt *phlrx)
{
	struct rtw_pkt_buf_list *pktbuf;
	u8 shift_sz;
	u32 alloc_sz;
	struct sk_buff *pkt = NULL;
	u8 *pbuf = NULL;


	rtw_warn_on(phlrx->pkt_cnt > 1);
	pktbuf = phlrx->pkt_list; /* &phlrx->pkt_list[0] */

	/* For 8 bytes IP header alignment. */
	if (phlrx->mdata.qos)
		/* Qos data, wireless lan header length is 26 */
		shift_sz = 6;
	else
		shift_sz = 0;

	/*
	 * For first fragment packet, driver need allocate 1536 to
	 * defrag packet.
	 * And need 8 is for skb->data 8 bytes alignment.
	 * Round (1536 + shift_sz + 8) to 128 bytes alignment,
	 * and finally get 1664.
	 */
	alloc_sz = pktbuf->length;
	if ((phlrx->mdata.more_frag == 1) && (phlrx->mdata.frag_num == 0)) {
		if (alloc_sz <= 1650)
			alloc_sz = 1664;
		else
			alloc_sz += 14;
	} else {
		/*
		 * 6 is for IP header 8 bytes alignment in QoS packet case.
		 * 8 is for skb->data 4 bytes alignment.
		 */
		alloc_sz += 14;
	}

	pkt = rtw_skb_alloc(alloc_sz);
	if (!pkt) {
		RTW_ERR("%s: alloc skb fail! sz=%u (mfrag=%u, frag_num=%u)\n",
			__FUNCTION__, alloc_sz, phlrx->mdata.more_frag,
			phlrx->mdata.frag_num);
		return -1;
	}

	/* force pkt->data at 8-byte alignment address */
	skb_reserve(pkt, 8 - ((SIZE_PTR)pkt->data & 7));
	/* force ip_hdr at 8-byte alignment address according to shift_sz. */
	skb_reserve(pkt, shift_sz);
	pbuf = skb_put(pkt, pktbuf->length);
	_rtw_memcpy(pbuf, pktbuf->vir_addr, pktbuf->length);

	prframe->u.hdr.pkt = pkt;
	prframe->u.hdr.rx_data = pkt->data;
	prframe->u.hdr.rx_tail = skb_tail_pointer(pkt);
	prframe->u.hdr.len = pkt->len;
	prframe->u.hdr.rx_head = pkt->head;
	prframe->u.hdr.rx_end = skb_end_pointer(pkt);

	return 0;
}

void core_update_recvframe_mdata(union recv_frame *prframe, struct rtw_recv_pkt *rx_req)
{
	struct rx_pkt_attrib *prxattrib = &prframe->u.hdr.attrib;
	struct rtw_r_meta_data *mdata = &rx_req->mdata;

	prxattrib->bdecrypted = !(mdata->sw_dec);
	prxattrib->pkt_len = mdata->pktlen;
	prxattrib->icv_err = mdata->icverr;
	prxattrib->crc_err = mdata->crc32;
	prxattrib->data_rate = mdata->rx_rate; /* enum rtw_data_rate */
	prxattrib->gi_ltf = mdata->rx_gi_ltf;
#ifdef CONFIG_TCP_CSUM_OFFLOAD_RX
	prxattrib->csum_valid = mdata->chksum_ofld_en;
	prxattrib->csum_err = mdata->chksum_status;
#endif
	/* when 0 indicate no encrypt. when non-zero, indicate the encrypt algorith */
	prxattrib->encrypt = rtw_sec_algo_phl2drv((enum rtw_enc_algo)mdata->sec_type);

#if 0 //todo
//Security (sw-decrypt & calculate payload offset)
	u8	bdecrypted;
	u8	encrypt; /* when 0 indicate no encrypt. when non-zero, indicate the encrypt algorith */
	u8	iv_len;
	u8	icv_len;
	u8	crc_err;
	u8	icv_err;
#endif
	prxattrib->addr_cam_vld = mdata->addr_cam_vld;
	prxattrib->macid = mdata->macid;


	return;
}

#ifdef RTW_WKARD_CORE_RSSI_V1
static inline void _rx_process_ss_sq(_adapter *padapter, union recv_frame *prframe)
{
	struct rx_pkt_attrib *pattrib = &prframe->u.hdr.attrib;
	struct signal_stat *ss = &padapter->recvinfo.signal_strength_data;
	struct signal_stat *sq = &padapter->recvinfo.signal_qual_data;

	if (ss->update_req) {
		ss->total_num = 0;
		ss->total_val = 0;
		ss->update_req = 0;
	}

	ss->total_num++;
	ss->total_val += pattrib->phy_info.signal_strength;
	ss->avg_val = ss->total_val / ss->total_num;

	if (sq->update_req) {
		sq->total_num = 0;
		sq->total_val = 0;
		sq->update_req = 0;
	}

	sq->total_num++;
	sq->total_val += pattrib->phy_info.signal_quality;
	sq->avg_val = sq->total_val / sq->total_num;
}

/*#define DBG_RECV_INFO*/
void rx_process_phy_info(union recv_frame *precvframe)
{
	_adapter *padapter = precvframe->u.hdr.adapter;
	struct rx_pkt_attrib *pattrib = &precvframe->u.hdr.attrib;
	struct phydm_phyinfo_struct *phy_info = &pattrib->phy_info;
	u8 *wlanhdr = NULL;
	u8 *ta, *ra;
	u8 is_ra_bmc;
	struct sta_priv *pstapriv;
	struct sta_info *psta = NULL;
	struct recv_info  *precvinfo = &padapter->recvinfo;

	bool is_packet_match_bssid = _FALSE;
	bool is_packet_to_self = _FALSE;
	bool is_packet_beacon = _FALSE;

	wlanhdr = precvframe->u.hdr.rx_data;
	ta = get_ta(wlanhdr);
	ra = get_ra(wlanhdr);
	is_ra_bmc = IS_MCAST(ra);

	if (_rtw_memcmp(adapter_mac_addr(padapter), ta, ETH_ALEN) == _TRUE) {
		static systime start_time = 0;

		#if 0 /*For debug */
		if (IsFrameTypeCtrl(wlanhdr)) {
			RTW_INFO("-->Control frame: Y\n");
			RTW_INFO("-->pkt_len: %d\n", pattrib->pkt_len);
			RTW_INFO("-->Sub Type = 0x%X\n", get_frame_sub_type(wlanhdr));
		}

		/* Dump first 40 bytes of header */
		int i = 0;

		for (i = 0; i < 40; i++)
			RTW_INFO("%d: %X\n", i, *((u8 *)wlanhdr + i));

		RTW_INFO("\n");
		#endif

		if ((start_time == 0) || (rtw_get_passing_time_ms(start_time) > 5000)) {
			RTW_ERR("Warning!!! %s: Confilc mac addr!!\n", __func__);
			start_time = rtw_get_current_time();
		}
		precvinfo->dbg_rx_conflic_mac_addr_cnt++;
		return;
	}

	pstapriv = &padapter->stapriv;
	psta = precvframe->u.hdr.psta;

	is_packet_match_bssid = (!IsFrameTypeCtrl(wlanhdr))
			&& (!pattrib->icv_err) && (!pattrib->crc_err)
			&& ((!MLME_IS_MESH(padapter) && _rtw_memcmp(get_hdr_bssid(wlanhdr), get_bssid(&padapter->mlmepriv), ETH_ALEN))
				|| (MLME_IS_MESH(padapter) && psta));

	/*is_to_self = (!pattrib->icv_err) && (!pattrib->crc_err)
			&& _rtw_memcmp(ra, adapter_mac_addr(padapter), ETH_ALEN);*/

	is_packet_to_self = is_packet_match_bssid
			&& _rtw_memcmp(ra, adapter_mac_addr(padapter), ETH_ALEN);

	is_packet_beacon = is_packet_match_bssid
			 && (get_frame_sub_type(wlanhdr) == WIFI_BEACON);


	if (psta && IsFrameTypeData(wlanhdr)) {


		if (is_ra_bmc)
			psta->curr_rx_rate_bmc = pattrib->data_rate;
		else {
			psta->curr_rx_rate = pattrib->data_rate;
			psta->curr_rx_gi_ltf = pattrib->gi_ltf;
		}
	}

	#if 0
	/* If bw is initial value, get from phy status */
	if (pattrib->bw == CHANNEL_WIDTH_MAX)
		pattrib->bw = p_phy_info->band_width;
	#endif
#ifdef DBG_RECV_INFO
	RTW_INFO("%s match_bssid:%d, to_self:%d, is_beacon:%d",
		__func__, is_packet_match_bssid, is_packet_to_self, is_packet_beacon);

	RTW_INFO("hdr_bssid:"MAC_FMT" my_bssid:"MAC_FMT"\n", 
				MAC_ARG(get_hdr_bssid(wlanhdr)),
				MAC_ARG(get_bssid(&padapter->mlmepriv)));

	RTW_INFO("ra:"MAC_FMT" my_addr:"MAC_FMT"\n", 
				MAC_ARG(ra),
				MAC_ARG(adapter_mac_addr(padapter)));

#endif
	precvframe->u.hdr.psta = NULL;
	if (padapter->registrypriv.mp_mode != 1) {
		if ((!MLME_IS_MESH(padapter) && is_packet_match_bssid)
			|| (MLME_IS_MESH(padapter) && psta)) {
			if (psta) {
				precvframe->u.hdr.psta = psta;
#ifdef DBG_RX_DFRAME_RAW_DATA
			rx_process_dframe_raw_data(precvframe);
#endif
			}
			if (phy_info->is_valid)
				_rx_process_ss_sq(padapter, precvframe);/*signal_strength & signal_quality*/
		} else if (is_packet_to_self || is_packet_beacon) {
			if (psta) {
				precvframe->u.hdr.psta = psta;
#ifdef DBG_RX_DFRAME_RAW_DATA
			rx_process_dframe_raw_data(precvframe);
#endif
			}
			if (phy_info->is_valid)
				_rx_process_ss_sq(padapter, precvframe);/*signal_strength & signal_quality*/
		}
	}
	#if 0
	#ifdef CONFIG_MP_INCLUDED
	else {
		#ifdef CONFIG_MP_INCLUDED
		if (padapter->mppriv.brx_filter_beacon == _TRUE) {
			if (is_packet_beacon) {
				RTW_INFO("in MP Rx is_packet_beacon\n");
				if (psta)
					precvframe->u.hdr.psta = psta;
				_rx_process_ss_sq(padapter, precvframe);
			}
		} else
		#endif
		{
			if (psta)
				precvframe->u.hdr.psta = psta;
			_rx_process_ss_sq(padapter, precvframe);
		}
	}
	#endif
	#endif
}


/*#define DBG_PHY_INFO*/
void core_update_recvframe_phyinfo(union recv_frame *prframe, struct rtw_recv_pkt *rx_req)
{
	struct rx_pkt_attrib *attrib = &prframe->u.hdr.attrib;
	struct rtw_phl_ppdu_phy_info *phy_info = &rx_req->phy_info;
	u8 ptype, pstype;
	int i;

	_rtw_memset(&attrib->phy_info, 0, sizeof(struct phydm_phyinfo_struct));

	ptype = phy_info->frame_type & 0x03;
	pstype = phy_info->frame_type >> 2;

	#ifdef DBG_PHY_INFO
	{
		u8 type, subtype;
		u8 *ptr = prframe->u.hdr.rx_data;

		type =	GetFrameType(ptr) >> 2;
		subtype = get_frame_sub_type(ptr) >> 4; /* bit(7)~bit(2) */
		if ((type != WIFI_CTRL_TYPE) && (ptype != type || pstype != subtype)) {
			RTW_INFO("[DBG-PHY-INFO]- FT:0x%02x, FST:0x%02x\t", type, subtype);
			RTW_INFO("A1:"MAC_FMT" A2:"MAC_FMT" A3:"MAC_FMT"\n", 
					MAC_ARG(GetAddr1Ptr(ptr)),
					MAC_ARG(get_addr2_ptr(ptr)),
					MAC_ARG(GetAddr3Ptr(ptr)));
		}
	}
	#endif

	if (phy_info->is_valid) {
		attrib->phy_info.is_valid = true;
		attrib->phy_info.signal_strength = phy_info->rssi;
		attrib->phy_info.signal_quality = phy_info->rssi;
		attrib->phy_info.recv_signal_power = rtw_phl_rssi_to_dbm(phy_info->rssi);
		attrib->ch = phy_info->ch_idx;
		/* snr info */
		attrib->phy_info.snr_fd_avg = phy_info->snr_fd_avg;
		attrib->phy_info.snr_td_avg = phy_info->snr_td_avg;
		for ( i = 0; i < RTW_PHL_MAX_RF_PATH; i++) {
			attrib->phy_info.rx_pwr[i] = phy_info->rssi_path[i];
			attrib->phy_info.snr_fd[i] = phy_info->snr_fd[i];
			attrib->phy_info.snr_td[i] = phy_info->snr_td[i];
		}

		#ifdef DBG_PHY_INFO
		RTW_INFO("[PHY-INFO] ft:0x%02x-0x%02x rssi:%d, ch_idx:%d, tx_bf:%d\n",
			ptype, pstype, phy_info->rssi, phy_info->ch_idx, phy_info->tx_bf);

		RTW_INFO("[PHY-INFO] ss:%d sq:%d rssi:%d\n",
				attrib->phy_info.signal_strength,
				attrib->phy_info.signal_quality,
				attrib->phy_info.recv_signal_power);
		#endif

	} else {
		#ifdef DBG_PHY_INFO
		RTW_INFO("[PHY-INFO-INVALID] ftype:0x%02x-0x%02x rssi:%d, ch_idx:%d, tx_bf:%d\n",
			ptype, pstype, phy_info->rssi, phy_info->ch_idx, phy_info->tx_bf);
		#endif
	}
}
#endif /*RTW_WKARD_CORE_RSSI_V1*/

s32 core_rx_process_amsdu(_adapter *adapter, union recv_frame *prframe)
{
	if(amsdu_to_msdu(adapter, prframe) != _SUCCESS)
		return CORE_RX_DROP;

	return CORE_RX_DONE;
}

s32 core_rx_process_msdu(_adapter *adapter, union recv_frame *prframe)
{
	struct rx_pkt_attrib *pattrib = &prframe->u.hdr.attrib;
	u8 *msdu = get_recvframe_data(prframe)
		+ pattrib->hdrlen + pattrib->iv_len + RATTRIB_GET_MCTRL_LEN(pattrib);
	u16 msdu_len = prframe->u.hdr.len
		- pattrib->hdrlen - pattrib->iv_len - RATTRIB_GET_MCTRL_LEN(pattrib)
		- (pattrib->encrypt ? pattrib->icv_len : 0);
	enum rtw_rx_llc_hdl llc_hdl = rtw_recv_llc_parse(msdu, msdu_len);
	int act = RTW_RX_MSDU_ACT_INDICATE;

#if defined(CONFIG_AP_MODE)
	struct xmit_frame *fwd_frame = NULL;
	_list f_list;

	if (MLME_IS_AP(adapter))
		act = rtw_ap_rx_msdu_act_check(prframe, pattrib->dst, pattrib->src
				, msdu, llc_hdl, &fwd_frame, &f_list);
	if (!act)
		return CORE_RX_DROP;
#endif

	if(wlanhdr_to_ethhdr(prframe, llc_hdl) != _SUCCESS) {
		if (act & RTW_RX_MSDU_ACT_INDICATE) {
			#ifdef DBG_RX_DROP_FRAME
			RTW_INFO("DBG_RX_DROP_FRAME "FUNC_ADPT_FMT" wlanhdr_to_ethhdr: drop pkt\n"
				, FUNC_ADPT_ARG(adapter));
			#endif
		}

		#if defined(CONFIG_AP_MODE)
		if (act & RTW_RX_MSDU_ACT_FORWARD) {
			#ifdef DBG_TX_DROP_FRAME
			RTW_INFO("DBG_TX_DROP_FRAME %s wlanhdr_to_ethhdr fail\n", __func__);
			#endif
			recv_free_fwd_resource(adapter, fwd_frame, &f_list);
		}
		#endif
		return CORE_RX_DROP;
	}

	#if defined(CONFIG_AP_MODE)
	if (act & RTW_RX_MSDU_ACT_FORWARD) {
		recv_fwd_pkt_hdl(adapter, prframe->u.hdr.pkt, act, fwd_frame, &f_list);
		if (!(act & RTW_RX_MSDU_ACT_INDICATE)) {
			prframe->u.hdr.pkt = NULL;
			rtw_free_recvframe(prframe);
			return CORE_RX_DONE;
		}
	}
	#endif

	if(rtw_recv_indicatepkt_check(prframe, 
		get_recvframe_data(prframe), get_recvframe_len(prframe)) != _SUCCESS)
		return CORE_RX_DROP;

	if(rtw_recv_indicatepkt(adapter, prframe) != _SUCCESS)
		return CORE_RX_DROP;

	return CORE_RX_DONE;
}


s32 rtw_core_rx_data_post_process(_adapter *adapter, union recv_frame *prframe)
{
	//amsdu
	//make eth hdr
	//forward

	//recv_process_mpdu
	  //amsdu_to_msdu
	  //wlanhdr_to_ethhdr
	  //rtw_recv_indicatepkt_check
	  //rtw_recv_indicatepkt


//todo hw amsdu
	if (prframe->u.hdr.attrib.amsdu) 
		return core_rx_process_amsdu(adapter, prframe);
	else
		return core_rx_process_msdu(adapter, prframe);
	
}


s32 rtw_core_rx_data_pre_process(_adapter *adapter, union recv_frame **prframe)
{
	//recv_func_posthandle
	  //decryptor
	  //portctrl
	  //count_rx_stats
#ifdef CONFIG_TDLS
#endif
#ifdef DBG_RX_BMC_FRAME
#endif
#ifdef CONFIG_WAPI_SUPPORT
#endif

	union recv_frame * ret_frame = NULL;

	s32 ret = CORE_RX_CONTINUE;

	ret_frame = decryptor(adapter, *prframe);
	if (ret_frame == NULL)
		return CORE_RX_DROP;
	else
		*prframe = ret_frame;

	ret = recvframe_chk_defrag(adapter, prframe);
	if (ret != CORE_RX_CONTINUE) {
		if (ret == CORE_RX_DROP) {
			#ifdef DBG_RX_DROP_FRAME
			RTW_INFO("DBG_RX_DROP_FRAME "FUNC_ADPT_FMT" recvframe_chk_defrag: drop pkt\n"
				, FUNC_ADPT_ARG(adapter));
			#endif
			DBG_COUNTER(adapter->rx_logs.core_rx_post_defrag_err);
		}
		return ret;
	}

	/* Rx TKIP MIC */
	if ((*prframe)->u.hdr.attrib.privacy) {
		if (recvframe_chkmic(adapter,  *prframe) == _FAIL) {
			return CORE_RX_DROP;
		}
	}

	ret_frame = portctrl(adapter, *prframe);
	if (ret_frame == NULL)
		return CORE_RX_DROP;
	else
		*prframe = ret_frame;

	count_rx_stats(adapter, *prframe, NULL);

#ifdef CONFIG_WAPI_SUPPORT
	rtw_wapi_update_info(adapter, *prframe);
#endif

	return CORE_RX_CONTINUE;
}

s32 rtw_core_update_recvframe(struct dvobj_priv *dvobj,
	union recv_frame *prframe, struct rtw_recv_pkt *rx_req)
{
	u8 *pbuf = NULL;
	u8 *pda = NULL;
	_adapter *iface = NULL;
	u8 is_bmc = _FALSE;
	enum rtw_core_rx_state rx_state = CORE_RX_CONTINUE;
	_adapter *primary_padapter = dvobj_get_primary_adapter(dvobj);
	int err;
	struct mlme_priv	*pmlmepriv = NULL;

	if (rx_req->mdata.bc || rx_req->mdata.mc)
		is_bmc = _TRUE;

	//pre_recv_entry
	//rtw_get_iface_by_macddr
	if (rx_req->os_priv) {
		prframe->u.hdr.pkt = rx_req->os_priv; /*skb*/
		core_update_recvframe_pkt(prframe, rx_req);
	} else {
		err = core_alloc_recvframe_pkt(prframe, rx_req);
		if (err) {
			rx_state = CORE_RX_FAIL;
			goto exit;
		}
	}

	core_update_recvframe_mdata(prframe, rx_req);
	#ifdef RTW_WKARD_CORE_RSSI_V1
	core_update_recvframe_phyinfo(prframe, rx_req);
	#endif

	prframe->u.hdr.adapter = primary_padapter;
	prframe->u.hdr.pkt->dev = primary_padapter->pnetdev;

	if (!is_bmc) {
		pbuf = prframe->u.hdr.rx_data;
		pda = get_ra(pbuf);
		iface = rtw_get_iface_by_macddr(primary_padapter, pda);
		if(iface) {
			prframe->u.hdr.adapter = iface;
			prframe->u.hdr.pkt->dev = iface->pnetdev;
			/* ToDo CONFIG_RTW_MLD: TBD to see if there is direct info from mdata */
			if (rx_req->rx_rlink)
				prframe->u.hdr.adapter_link = GET_LINK(iface, rx_req->rx_rlink->id);
			else
				prframe->u.hdr.adapter_link = GET_PRIMARY_LINK(iface);
		}
		else {
			#if 0 /*#ifdef CONFIG_RTW_CFGVENDOR_RANDOM_MAC_OUI - TODO*/
			if (_rtw_memcmp(ra, adapter_pno_mac_addr(primary_padapter), ETH_ALEN))
					goto query_phy_status;
			#endif

			#ifdef CONFIG_RTW_MULTI_AP
			/* unasoc STA RCPI */
			if (rtw_unassoc_sta_src_chk(primary_padapter, UNASOC_STA_SRC_RX_NMY_UC) &&
				prframe->u.hdr.attrib.phy_info.recv_signal_power != 0) {
				rtw_rx_add_unassoc_sta(primary_padapter, UNASOC_STA_SRC_RX_NMY_UC,
					get_ta(pbuf), prframe->u.hdr.attrib.phy_info.recv_signal_power);
				/* ToDo CONFIG_RTW_MLD: TBD to see if there is direct info from mdata */
				if (rx_req->rx_rlink)
					prframe->u.hdr.adapter_link = GET_LINK(primary_padapter, rx_req->rx_rlink->id);
				else
					prframe->u.hdr.adapter_link = GET_PRIMARY_LINK(primary_padapter);
			} else
			#endif
				RTW_DBG("%s [WARN] Cannot find appropriate adapter - mac_addr : "MAC_FMT"\n"
						, __func__, MAC_ARG(pda));
			rx_state = CORE_RX_FAIL;
		}
	}
	else {
		/* ToDo CONFIG_RTW_MLD: TBD to see if there is direct info from mdata */
		if (rx_req->rx_rlink)
			prframe->u.hdr.adapter_link = GET_LINK(primary_padapter, rx_req->rx_rlink->id);
		else
			prframe->u.hdr.adapter_link = GET_PRIMARY_LINK(primary_padapter);
		/*clone bcmc frame for all active adapter*/
		rtw_mi_buddy_clone_bcmc_packet(primary_padapter, prframe);

		#ifdef CONFIG_RTW_MULTI_AP
		pbuf = prframe->u.hdr.rx_data;
		/* unasoc STA RCPI */
		if (rtw_unassoc_sta_src_chk(primary_padapter, UNASOC_STA_SRC_RX_BMC) &&
			prframe->u.hdr.attrib.phy_info.recv_signal_power != 0) {
			rtw_rx_add_unassoc_sta(primary_padapter, UNASOC_STA_SRC_RX_BMC,
				get_ta(pbuf), prframe->u.hdr.attrib.phy_info.recv_signal_power);
		}
		#endif
	}

exit:
	prframe->u.hdr.rx_req = rx_req;

	pmlmepriv = &(prframe->u.hdr.adapter)->mlmepriv;
	if (check_fwstate(pmlmepriv, WIFI_MONITOR_STATE)) {
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 24))
			recv_frame_monitor(prframe->u.hdr.adapter, prframe, rx_req);
#endif
			rx_state = CORE_RX_FAIL;
	}

	return rx_state;
}

#ifdef CONFIG_RTW_NAPI
#ifdef CONFIG_RTW_NAPI_V2
static void rtw_core_rx_napi_v2(struct dvobj_priv *dvobj)
{
	_adapter *adapter = dvobj_get_primary_adapter(dvobj);
	struct recv_priv *recvpriv = &dvobj->recvpriv;

	if (adapter->registrypriv.en_napi) {
		_adapter *a;
		u8 i;

		for (i = 0; i < dvobj->iface_nums; i++) {
			a = dvobj->padapters[i];
			if ((rtw_if_up(a) == _TRUE)
				&& skb_queue_len(&recvpriv->rx_napi_skb_queue))
				napi_schedule(&a->napi);
		}
	}
}
#endif
#endif

enum rtw_phl_status rtw_core_rx_process(void *drv_priv)
{
	struct dvobj_priv *dvobj = (struct dvobj_priv *)drv_priv;
	_adapter *adapter = NULL;
	struct rtw_recv_pkt *rx_req = NULL;
	struct rtw_pkt_buf_list *pkt = NULL;
	union recv_frame *prframe = NULL;
	struct rx_pkt_attrib *prxattrib = NULL;
	u16 rx_pkt_num = 0;
	struct recv_priv *precvpriv = &dvobj->recvpriv;
	s32 pre_process_ret = CORE_RX_CONTINUE;

	rx_pkt_num = rtw_phl_query_new_rx_num(GET_PHL_INFO(dvobj));

#ifdef DBG_RECV_FRAME
	RTW_INFO("%s dvobj:%p, phl:%p rx_pkt_num:%d, free_recv_queue:%p\n",
	__func__, dvobj, dvobj->phl, rx_pkt_num, &precvpriv->free_recv_queue);
#endif

	while (rx_pkt_num--) {
		prframe = rtw_alloc_recvframe(&precvpriv->free_recv_queue);
		if (prframe == NULL) {
			RTW_ERR("F-%s L-%d rtw_alloc_recvframe failed\n", __FUNCTION__, __LINE__);
			goto rx_error;
		}

		//_rtw_init_listhead

		rx_req = rtw_phl_query_rx_pkt(GET_PHL_INFO(dvobj));
		if(rx_req == NULL)
			goto rx_stop;

		if(rtw_core_update_recvframe(dvobj, prframe, rx_req) != CORE_RX_CONTINUE)
			goto rx_next;

		prxattrib = &prframe->u.hdr.attrib;
		if (prxattrib->icv_err || prxattrib->crc_err)
			goto rx_next;

		adapter = prframe->u.hdr.adapter;

		if (!rtw_is_adapter_up(adapter))
			goto rx_next;

#ifdef CONFIG_RTW_CORE_RXSC
		if (core_rxsc_apply_check(adapter, prframe) == CORE_RX_GO_SHORTCUT &&
			core_rxsc_apply_shortcut(adapter, prframe) == CORE_RX_DONE)
			continue;
#endif

		//recv_func_prehandle
		//mgt_dispatcher exist here && sw decrypt mgmt 
		//?? todo power save
		if(validate_recv_frame(adapter, prframe) != CORE_RX_CONTINUE)
			goto rx_next;

		pre_process_ret = rtw_core_rx_data_pre_process(adapter, &prframe);
		if (pre_process_ret == CORE_RX_DEFRAG)
			continue;
		if (pre_process_ret != CORE_RX_CONTINUE)
			goto rx_next;

		if(rtw_core_rx_data_post_process(adapter, prframe) == CORE_RX_DONE) {
			adapter->recvinfo.rx_pkts++;
			continue;
		}

rx_next:
		rtw_free_recvframe(prframe);
		continue;
rx_stop:
		rtw_free_recvframe(prframe);
		break;
rx_error:
		break;
	}

#ifdef CONFIG_RTW_NAPI
#ifdef CONFIG_RTW_NAPI_V2
	rtw_core_rx_napi_v2(dvobj);
#endif
#endif

	return RTW_PHL_STATUS_SUCCESS;
}
#endif /*RTW_PHL_RX*/

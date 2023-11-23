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
#define _RTW_STA_MGT_C_

#include <drv_types.h>

bool test_st_match_rule(_adapter *adapter, u8 *local_naddr, u8 *local_port, u8 *remote_naddr, u8 *remote_port)
{
	if (ntohs(*((u16 *)local_port)) == 5001 || ntohs(*((u16 *)remote_port)) == 5001)
		return _TRUE;
	return _FALSE;
}

struct st_register test_st_reg = {
	.s_proto = 0x06,
	.rule = test_st_match_rule,
};

inline void rtw_st_ctl_init(struct st_ctl_t *st_ctl)
{
	_rtw_memset(st_ctl->reg, 0 , sizeof(struct st_register) * SESSION_TRACKER_REG_ID_NUM);
	_rtw_init_queue(&st_ctl->tracker_q);
}

inline void rtw_st_ctl_clear_tracker_q(struct st_ctl_t *st_ctl)
{
	_list *plist, *phead;
	struct session_tracker *st;

	_rtw_spinlock_bh(&st_ctl->tracker_q.lock);
	phead = &st_ctl->tracker_q.queue;
	plist = get_next(phead);
	while (rtw_end_of_queue_search(phead, plist) == _FALSE) {
		st = LIST_CONTAINOR(plist, struct session_tracker, list);
		plist = get_next(plist);
		rtw_list_delete(&st->list);
		rtw_mfree((u8 *)st, sizeof(struct session_tracker));
	}
	_rtw_spinunlock_bh(&st_ctl->tracker_q.lock);
}

inline void rtw_st_ctl_deinit(struct st_ctl_t *st_ctl)
{
	rtw_st_ctl_clear_tracker_q(st_ctl);
	_rtw_deinit_queue(&st_ctl->tracker_q);
}

inline void rtw_st_ctl_register(struct st_ctl_t *st_ctl, u8 st_reg_id, struct st_register *reg)
{
	if (st_reg_id >= SESSION_TRACKER_REG_ID_NUM) {
		rtw_warn_on(1);
		return;
	}

	st_ctl->reg[st_reg_id].s_proto = reg->s_proto;
	st_ctl->reg[st_reg_id].rule = reg->rule;
}

inline void rtw_st_ctl_unregister(struct st_ctl_t *st_ctl, u8 st_reg_id)
{
	int i;

	if (st_reg_id >= SESSION_TRACKER_REG_ID_NUM) {
		rtw_warn_on(1);
		return;
	}

	st_ctl->reg[st_reg_id].s_proto = 0;
	st_ctl->reg[st_reg_id].rule = NULL;

	/* clear tracker queue if no session trecker registered */
	for (i = 0; i < SESSION_TRACKER_REG_ID_NUM; i++)
		if (st_ctl->reg[i].s_proto != 0)
			break;
	if (i >= SESSION_TRACKER_REG_ID_NUM)
		rtw_st_ctl_clear_tracker_q(st_ctl);
}

inline bool rtw_st_ctl_chk_reg_s_proto(struct st_ctl_t *st_ctl, u8 s_proto)
{
	bool ret = _FALSE;
	int i;

	for (i = 0; i < SESSION_TRACKER_REG_ID_NUM; i++) {
		if (st_ctl->reg[i].s_proto == s_proto) {
			ret = _TRUE;
			break;
		}
	}

	return ret;
}

inline bool rtw_st_ctl_chk_reg_rule(struct st_ctl_t *st_ctl, _adapter *adapter, u8 *local_naddr, u8 *local_port, u8 *remote_naddr, u8 *remote_port)
{
	bool ret = _FALSE;
	int i;
	st_match_rule rule;

	for (i = 0; i < SESSION_TRACKER_REG_ID_NUM; i++) {
		rule = st_ctl->reg[i].rule;
		if (rule && rule(adapter, local_naddr, local_port, remote_naddr, remote_port) == _TRUE) {
			ret = _TRUE;
			break;
		}
	}

	return ret;
}

void rtw_st_ctl_rx(struct sta_info *sta, u8 *ehdr_pos)
{
	_adapter *adapter = sta->padapter;
	struct ethhdr *etherhdr = (struct ethhdr *)ehdr_pos;

	if (ntohs(etherhdr->h_proto) == ETH_P_IP) {
		u8 *ip = ehdr_pos + ETH_HLEN;

		if (GET_IPV4_PROTOCOL(ip) == 0x06  /* TCP */
			&& rtw_st_ctl_chk_reg_s_proto(&sta->st_ctl, 0x06) == _TRUE
		) {
			u8 *tcp = ip + GET_IPV4_IHL(ip) * 4;

			if (rtw_st_ctl_chk_reg_rule(&sta->st_ctl, adapter, IPV4_DST(ip), TCP_DST(tcp), IPV4_SRC(ip), TCP_SRC(tcp)) == _TRUE) {
				if (GET_TCP_SYN(tcp) && GET_TCP_ACK(tcp)) {
					session_tracker_add_cmd(adapter, sta
						, IPV4_DST(ip), TCP_DST(tcp)
						, IPV4_SRC(ip), TCP_SRC(tcp));
					if (DBG_SESSION_TRACKER)
						RTW_INFO(FUNC_ADPT_FMT" local:"IP_FMT":"PORT_FMT", remote:"IP_FMT":"PORT_FMT" SYN-ACK\n"
							, FUNC_ADPT_ARG(adapter)
							, IP_ARG(IPV4_DST(ip)), PORT_ARG(TCP_DST(tcp))
							, IP_ARG(IPV4_SRC(ip)), PORT_ARG(TCP_SRC(tcp)));
				}
				if (GET_TCP_FIN(tcp)) {
					session_tracker_del_cmd(adapter, sta
						, IPV4_DST(ip), TCP_DST(tcp)
						, IPV4_SRC(ip), TCP_SRC(tcp));
					if (DBG_SESSION_TRACKER)
						RTW_INFO(FUNC_ADPT_FMT" local:"IP_FMT":"PORT_FMT", remote:"IP_FMT":"PORT_FMT" FIN\n"
							, FUNC_ADPT_ARG(adapter)
							, IP_ARG(IPV4_DST(ip)), PORT_ARG(TCP_DST(tcp))
							, IP_ARG(IPV4_SRC(ip)), PORT_ARG(TCP_SRC(tcp)));
				}
			}

		}
	}
}

#define SESSION_TRACKER_FMT IP_FMT":"PORT_FMT" "IP_FMT":"PORT_FMT" %u %d"
#define SESSION_TRACKER_ARG(st) IP_ARG(&(st)->local_naddr), PORT_ARG(&(st)->local_port), IP_ARG(&(st)->remote_naddr), PORT_ARG(&(st)->remote_port), (st)->status, rtw_get_passing_time_ms((st)->set_time)

void dump_st_ctl(void *sel, struct st_ctl_t *st_ctl)
{
	int i;
	_list *plist, *phead;
	struct session_tracker *st;

	if (!DBG_SESSION_TRACKER)
		return;

	for (i = 0; i < SESSION_TRACKER_REG_ID_NUM; i++)
		RTW_PRINT_SEL(sel, "reg%d: %u %p\n", i, st_ctl->reg[i].s_proto, st_ctl->reg[i].rule);

	_rtw_spinlock_bh(&st_ctl->tracker_q.lock);
	phead = &st_ctl->tracker_q.queue;
	plist = get_next(phead);
	while (rtw_end_of_queue_search(phead, plist) == _FALSE) {
		st = LIST_CONTAINOR(plist, struct session_tracker, list);
		plist = get_next(plist);

		RTW_PRINT_SEL(sel, SESSION_TRACKER_FMT"\n", SESSION_TRACKER_ARG(st));
	}
	_rtw_spinunlock_bh(&st_ctl->tracker_q.lock);

}

void _rtw_init_stainfo(struct sta_info *psta);
void _rtw_init_stainfo(struct sta_info *psta)
{
	_rtw_memset((u8 *)psta, 0, sizeof(struct sta_info));

	_rtw_spinlock_init(&psta->lock);
	_rtw_init_listhead(&psta->list);
	_rtw_init_listhead(&psta->hash_list);
	/* _rtw_init_listhead(&psta->asoc_list); */
	/* _rtw_init_listhead(&psta->sleep_list); */
	/* _rtw_init_listhead(&psta->wakeup_list);	 */

	_rtw_init_queue(&psta->sleep_q);

	_rtw_init_sta_xmit_priv(&psta->sta_xmitpriv);
	_rtw_init_sta_recv_priv(&psta->sta_recvpriv);

#ifdef CONFIG_AP_MODE
	_rtw_init_listhead(&psta->asoc_list);
	_rtw_init_listhead(&psta->auth_list);
	psta->bpairwise_key_installed = _FALSE;
	psta->start_active = _FALSE;

#ifdef CONFIG_RTW_80211R_AP
	rtw_ft_peer_info_init(psta);
#endif
#ifdef CONFIG_RTW_80211R
	psta->ft_pairwise_key_installed = _FALSE;
#endif
#endif /* CONFIG_AP_MODE	 */
	rtw_st_ctl_init(&psta->st_ctl);
	psta->smps_mode = SM_PS_DISABLE;

	ATOMIC_SET(&psta->deleting, 0);
	psta->first_auth_time = 0;
}

u32	_rtw_init_sta_priv(struct	sta_priv *pstapriv)
{
	_adapter *adapter = container_of(pstapriv, _adapter, stapriv);
	/* struct macid_ctl_t *macid_ctl = adapter_to_macidctl(adapter); */
	struct sta_info *psta;
	s32 i;
	u32 ret = _FAIL;

	pstapriv->padapter = adapter;

	pstapriv->pallocated_stainfo_buf = rtw_zvmalloc(
		sizeof(struct sta_info) * NUM_STA + MEM_ALIGNMENT_OFFSET);
	if (!pstapriv->pallocated_stainfo_buf)
		goto exit;

	pstapriv->pstainfo_buf = pstapriv->pallocated_stainfo_buf;
	if ((SIZE_PTR)pstapriv->pstainfo_buf & MEM_ALIGNMENT_PADDING)
		pstapriv->pstainfo_buf += MEM_ALIGNMENT_OFFSET -
			((SIZE_PTR)pstapriv->pstainfo_buf & MEM_ALIGNMENT_PADDING);

	_rtw_init_queue(&pstapriv->free_sta_queue);

	_rtw_spinlock_init(&pstapriv->sta_hash_lock);
	_rtw_spinlock_init(&pstapriv->active_time_lock);

	/* _rtw_init_queue(&pstapriv->asoc_q); */
	pstapriv->asoc_sta_count = 0;
	_rtw_init_queue(&pstapriv->sleep_q);
	_rtw_init_queue(&pstapriv->wakeup_q);

	psta = (struct sta_info *)(pstapriv->pstainfo_buf);


	for (i = 0; i < NUM_STA; i++) {
		_rtw_init_stainfo(psta);

		_rtw_init_listhead(&(pstapriv->sta_hash[i]));

		rtw_list_insert_tail(&psta->list, get_list_head(&pstapriv->free_sta_queue));

		psta++;
	}

	pstapriv->adhoc_expire_to = 4; /* 4 * 2 = 8 sec */

#ifdef CONFIG_AP_MODE
	pstapriv->max_aid = rtw_phl_get_macid_max_num(
				GET_PHL_INFO(adapter_to_dvobj(pstapriv->padapter)));
	pstapriv->rr_aid = 0;
	pstapriv->started_aid = 1;
	pstapriv->sta_aid = rtw_zmalloc(pstapriv->max_aid * sizeof(struct sta_info *));
	if (!pstapriv->sta_aid)
		goto exit;
	pstapriv->aid_bmp_len = AID_BMP_LEN(pstapriv->max_aid);
	pstapriv->sta_dz_bitmap = rtw_zmalloc(pstapriv->aid_bmp_len);
	if (!pstapriv->sta_dz_bitmap)
		goto exit;
	pstapriv->tim_bitmap = rtw_zmalloc(pstapriv->aid_bmp_len);
	if (!pstapriv->tim_bitmap)
		goto exit;

	_rtw_init_listhead(&pstapriv->asoc_list);
	_rtw_init_listhead(&pstapriv->auth_list);
	_rtw_spinlock_init(&pstapriv->asoc_list_lock);
	_rtw_spinlock_init(&pstapriv->auth_list_lock);
	pstapriv->asoc_list_cnt = 0;
	pstapriv->auth_list_cnt = 0;
#ifdef CONFIG_AP_CMD_DISPR
	_rtw_init_listhead(&pstapriv->add_sta_list);
	pstapriv->add_sta_list_cnt = 0;
#endif
#ifdef CONFIG_RTW_TOKEN_BASED_XMIT
	pstapriv->tbtx_asoc_list_cnt = 0;
#endif

	pstapriv->auth_to = 3; /* 3*2 = 6 sec */
	pstapriv->assoc_to = 3;
	/* pstapriv->expire_to = 900; */ /* 900*2 = 1800 sec = 30 min, expire after no any traffic. */
	/* pstapriv->expire_to = 30; */ /* 30*2 = 60 sec = 1 min, expire after no any traffic. */
#ifdef CONFIG_ACTIVE_KEEP_ALIVE_CHECK
	pstapriv->expire_to = 3; /* 3*2 = 6 sec */
#else
	pstapriv->expire_to = 60;/* 60*2 = 120 sec = 2 min, expire after no any traffic. */
#endif
#ifdef CONFIG_ATMEL_RC_PATCH
	_rtw_memset(pstapriv->atmel_rc_pattern, 0, ETH_ALEN);
#endif
	pstapriv->max_num_sta = NUM_STA;

#endif

#if CONFIG_RTW_MACADDR_ACL
	for (i = 0; i < RTW_ACL_PERIOD_NUM; i++)
		rtw_macaddr_acl_init(adapter, i);
#endif

#if CONFIG_RTW_PRE_LINK_STA
	rtw_pre_link_sta_ctl_init(pstapriv);
#endif

#if defined(DBG_ROAMING_TEST)
	rtw_set_rx_chk_limit(adapter,1);
#elif defined(CONFIG_ACTIVE_KEEP_ALIVE_CHECK)
	rtw_set_rx_chk_limit(adapter,4);
#else
	rtw_set_rx_chk_limit(adapter,8);
#endif

	ret = _SUCCESS;

exit:
	if (ret != _SUCCESS) {
		if (pstapriv->pallocated_stainfo_buf)
			rtw_vmfree(pstapriv->pallocated_stainfo_buf,
				sizeof(struct sta_info) * NUM_STA + MEM_ALIGNMENT_OFFSET);
		#ifdef CONFIG_AP_MODE
		if (pstapriv->sta_aid)
			rtw_mfree(pstapriv->sta_aid, pstapriv->max_aid * sizeof(struct sta_info *));
		if (pstapriv->sta_dz_bitmap)
			rtw_mfree(pstapriv->sta_dz_bitmap, pstapriv->aid_bmp_len);
		#endif
	}

	return ret;
}

inline int rtw_stainfo_offset(struct sta_priv *stapriv, struct sta_info *sta)
{
	int offset = (((u8 *)sta) - stapriv->pstainfo_buf) / sizeof(struct sta_info);

	if (!stainfo_offset_valid(offset))
		RTW_INFO("%s invalid offset(%d), out of range!!!", __func__, offset);

	return offset;
}

inline struct sta_info *rtw_get_stainfo_by_offset(struct sta_priv *stapriv, int offset)
{
	if (!stainfo_offset_valid(offset))
		RTW_INFO("%s invalid offset(%d), out of range!!!", __func__, offset);

	return (struct sta_info *)(stapriv->pstainfo_buf + offset * sizeof(struct sta_info));
}

void	_rtw_free_sta_xmit_priv_lock(struct sta_xmit_priv *psta_xmitpriv);
void	_rtw_free_sta_xmit_priv_lock(struct sta_xmit_priv *psta_xmitpriv)
{

	_rtw_spinlock_free(&psta_xmitpriv->lock);

	_rtw_spinlock_free(&(psta_xmitpriv->be_q.sta_pending.lock));
	_rtw_spinlock_free(&(psta_xmitpriv->bk_q.sta_pending.lock));
	_rtw_spinlock_free(&(psta_xmitpriv->vi_q.sta_pending.lock));
	_rtw_spinlock_free(&(psta_xmitpriv->vo_q.sta_pending.lock));
}

static void	_rtw_free_sta_recv_priv_lock(struct sta_recv_priv *psta_recvpriv)
{

	_rtw_spinlock_free(&psta_recvpriv->lock);

	_rtw_spinlock_free(&(psta_recvpriv->defrag_q.lock));


}

void rtw_mfree_stainfo(struct sta_info *psta);
void rtw_mfree_stainfo(struct sta_info *psta)
{

	if (&psta->lock != NULL)
		_rtw_spinlock_free(&psta->lock);

	_rtw_free_sta_xmit_priv_lock(&psta->sta_xmitpriv);
	_rtw_free_sta_recv_priv_lock(&psta->sta_recvpriv);

}


/* this function is used to free the memory of lock || sema for all stainfos */
void rtw_mfree_all_stainfo(struct sta_priv *pstapriv);
void rtw_mfree_all_stainfo(struct sta_priv *pstapriv)
{
	_list	*plist, *phead;
	struct sta_info *psta = NULL;


	_rtw_spinlock_bh(&pstapriv->sta_hash_lock);

	phead = get_list_head(&pstapriv->free_sta_queue);
	plist = get_next(phead);

	while ((rtw_end_of_queue_search(phead, plist)) == _FALSE) {
		psta = LIST_CONTAINOR(plist, struct sta_info , list);
		plist = get_next(plist);

		rtw_mfree_stainfo(psta);
	}

	_rtw_spinunlock_bh(&pstapriv->sta_hash_lock);


}

void rtw_mfree_sta_priv_lock(struct	sta_priv *pstapriv);
void rtw_mfree_sta_priv_lock(struct	sta_priv *pstapriv)
{
	rtw_mfree_all_stainfo(pstapriv); /* be done before free sta_hash_lock */

	_rtw_spinlock_free(&pstapriv->free_sta_queue.lock);

	_rtw_spinlock_free(&pstapriv->sta_hash_lock);
	_rtw_spinlock_free(&pstapriv->active_time_lock);
	_rtw_spinlock_free(&pstapriv->wakeup_q.lock);
	_rtw_spinlock_free(&pstapriv->sleep_q.lock);

#ifdef CONFIG_AP_MODE
	_rtw_spinlock_free(&pstapriv->asoc_list_lock);
	_rtw_spinlock_free(&pstapriv->auth_list_lock);
#endif

}

u32	_rtw_free_sta_priv(struct	sta_priv *pstapriv)
{
	int	index;

	if (pstapriv) {
		#ifdef CONFIG_RECV_REORDERING_CTRL
		_list	*phead, *plist;
		struct sta_info *psta = NULL;
		struct recv_reorder_ctrl *preorder_ctrl;

		/*	delete all reordering_ctrl_timer		*/
		_rtw_spinlock_bh(&pstapriv->sta_hash_lock);
		for (index = 0; index < NUM_STA; index++) {
			phead = &(pstapriv->sta_hash[index]);
			plist = get_next(phead);

			while ((rtw_end_of_queue_search(phead, plist)) == _FALSE) {
				int i;
				psta = LIST_CONTAINOR(plist, struct sta_info , hash_list);
				plist = get_next(plist);

				for (i = 0; i < 16 ; i++) {
					preorder_ctrl = &psta->recvreorder_ctrl[i];
					_cancel_timer_ex(&preorder_ctrl->reordering_ctrl_timer);
				}
			}
		}
		_rtw_spinunlock_bh(&pstapriv->sta_hash_lock);
		/*===============================*/
		#endif

		rtw_mfree_sta_priv_lock(pstapriv);

#if CONFIG_RTW_MACADDR_ACL
		for (index = 0; index < RTW_ACL_PERIOD_NUM; index++)
			rtw_macaddr_acl_deinit(pstapriv->padapter, index);
#endif

#if CONFIG_RTW_PRE_LINK_STA
		rtw_pre_link_sta_ctl_deinit(pstapriv);
#endif

		if (pstapriv->pallocated_stainfo_buf)
			rtw_vmfree(pstapriv->pallocated_stainfo_buf,
				sizeof(struct sta_info) * NUM_STA + MEM_ALIGNMENT_OFFSET);
		#ifdef CONFIG_AP_MODE
		if (pstapriv->sta_aid)
			rtw_mfree(pstapriv->sta_aid, pstapriv->max_aid * sizeof(struct sta_info *));
		if (pstapriv->sta_dz_bitmap)
			rtw_mfree(pstapriv->sta_dz_bitmap, pstapriv->aid_bmp_len);
		if (pstapriv->tim_bitmap)
			rtw_mfree(pstapriv->tim_bitmap, pstapriv->aid_bmp_len);
		#endif
	}

	return _SUCCESS;
}

#if defined(CONFIG_80211N_HT) && defined(CONFIG_RECV_REORDERING_CTRL)
static void rtw_init_recv_timer(struct recv_reorder_ctrl *preorder_ctrl)
{
	_adapter *padapter = preorder_ctrl->padapter;

	rtw_init_timer(&(preorder_ctrl->reordering_ctrl_timer), rtw_reordering_ctrl_timeout_handler, preorder_ctrl);
}
#endif

/* struct	sta_info *rtw_alloc_stainfo(_queue *pfree_sta_queue, unsigned char *hwaddr) */
static struct sta_info *_rtw_alloc_core_stainfo(struct sta_priv *pstapriv,
													const u8 *hwaddr)
{
	s32	index;
	_list	*phash_list;
	struct sta_info *psta;
	_queue *pfree_sta_queue;
	struct recv_reorder_ctrl *preorder_ctrl;
	struct rtw_wifi_role_t *phl_role = pstapriv->padapter->phl_role;
	int i = 0;
	u16  wRxSeqInitialValue = 0xffff;


	pfree_sta_queue = &pstapriv->free_sta_queue;

	/* _rtw_spinlock_bh(&(pfree_sta_queue->lock)); */
	_rtw_spinlock_bh(&(pstapriv->sta_hash_lock));
	if (_rtw_queue_empty(pfree_sta_queue) == _TRUE) {
		/* _rtw_spinunlock_bh(&(pstapriv->sta_hash_lock));*/

		psta = NULL;
	} else {
		psta = LIST_CONTAINOR(get_next(&pfree_sta_queue->queue), struct sta_info, list);

		rtw_list_delete(&(psta->list));

		/* _rtw_spinunlock_bh(&(pfree_sta_queue->lock)); */
		_rtw_init_stainfo(psta);

		psta->padapter = pstapriv->padapter;

		/*
		for sta mode, due to self sta info & AP sta info are the same sta info
		using self hash index for sta mode, or bmc sta will not found self sta
		*/
		switch (phl_role->type) {
		case PHL_RTYPE_NONE:
		case PHL_RTYPE_STATION:
		case PHL_RTYPE_ADHOC:
		case PHL_RTYPE_P2P_DEVICE:
		case PHL_RTYPE_P2P_GC:
			index = wifi_mac_hash(phl_role->mac_addr);
			break;
		default:
			index = wifi_mac_hash(hwaddr);
			break;
		}

		if (index >= NUM_STA) {
			psta = NULL;
			goto exit;
		}
		phash_list = &(pstapriv->sta_hash[index]);

		/* _rtw_spinlock_bh(&(pstapriv->sta_hash_lock)); */

		rtw_list_insert_tail(&psta->hash_list, phash_list);

		pstapriv->asoc_sta_count++;

		rtw_mi_update_iface_status(&(pstapriv->padapter->mlmepriv), 0);

		/* _rtw_spinunlock_bh(&(pstapriv->sta_hash_lock)); */

		/* Commented by Albert 2009/08/13
		 * For the SMC router, the sequence number of first packet of WPS handshake will be 0.
		 * In this case, this packet will be dropped by recv_decache function if we use the 0x00 as the default value for tid_rxseq variable.
		 * So, we initialize the tid_rxseq variable as the 0xffff. */

		for (i = 0; i < 16; i++) {
			_rtw_memcpy(&psta->sta_recvpriv.rxcache.tid_rxseq[i], &wRxSeqInitialValue, 2);
			_rtw_memcpy(&psta->sta_recvpriv.bmc_tid_rxseq[i], &wRxSeqInitialValue, 2);
			_rtw_memset(&psta->sta_recvpriv.rxcache.iv[i], 0, sizeof(psta->sta_recvpriv.rxcache.iv[i]));
		}

		rtw_init_timer(&psta->addba_retry_timer, addba_timer_hdl, psta);
#ifdef CONFIG_IEEE80211W
		rtw_init_timer(&psta->dot11w_expire_timer, sa_query_timer_hdl, psta);
#endif /* CONFIG_IEEE80211W */
#ifdef CONFIG_TDLS
		rtw_init_tdls_timer(pstapriv->padapter, psta);
#endif /* CONFIG_TDLS */

		/* for A-MPDU Rx reordering buffer control */
		for (i = 0; i < 16 ; i++) {
			preorder_ctrl = &psta->recvreorder_ctrl[i];
			preorder_ctrl->padapter = pstapriv->padapter;
			preorder_ctrl->tid = i;
			preorder_ctrl->enable = _FALSE;
			preorder_ctrl->indicate_seq = 0xffff;
			#ifdef DBG_RX_SEQ
			RTW_INFO("DBG_RX_SEQ "FUNC_ADPT_FMT" tid:%u SN_CLEAR indicate_seq:%d\n"
				, FUNC_ADPT_ARG(pstapriv->padapter), i, preorder_ctrl->indicate_seq);
			#endif

			preorder_ctrl->ampdu_size = RX_AMPDU_SIZE_INVALID;

			#ifdef CONFIG_RECV_REORDERING_CTRL
			/* preorder_ctrl->wsize_b = (NR_RECVBUFF-2); */
			preorder_ctrl->wsize_b = 64;/* 64; */
			_rtw_init_queue(&preorder_ctrl->pending_recvframe_queue);

			rtw_init_recv_timer(preorder_ctrl);
			#endif
			rtw_clear_bit(RTW_RECV_ACK_OR_TIMEOUT, &preorder_ctrl->rec_abba_rsp_ack);

		}
		ATOMIC_SET(&psta->keytrack, 0);
		/* ToDo: need to API to init hal_sta->ra_info->rssi_stat.rssi  */
		#if 0
		psta->phl_sta->rssi_stat.rssi = (-1);
		psta->phl_sta->rssi_stat.rssi_cck = (-1);
		psta->phl_sta->rssi_stat.rssi_ofdm = (-1);
		#endif
#ifdef CONFIG_ATMEL_RC_PATCH
		psta->flag_atmel_rc = 0;
#endif

#ifdef CONFIG_RTW_TOKEN_BASED_XMIT
		psta->tbtx_enable = _FALSE;
#endif
		/* init for the sequence number of received management frame */
		psta->RxMgmtFrameSeqNum = 0xffff;
		_rtw_memset(&psta->sta_stats, 0, sizeof(struct stainfo_stats));

		psta->tx_q_enable = 0;
		_rtw_init_queue(&psta->tx_queue);
		_init_workitem(&psta->tx_q_work, rtw_xmit_dequeue_callback, NULL);
	}

exit:

	_rtw_spinunlock_bh(&(pstapriv->sta_hash_lock));


	if (psta)
		rtw_mi_update_iface_status(&(pstapriv->padapter->mlmepriv), 0);

	return psta;
}

static void _rtw_alloc_phl_stainfo(struct sta_info *sta, struct	sta_priv *stapriv,
				const u8 *hwaddr, enum rtw_device_type dtype, u16 main_id,
				u8 link_idx, enum phl_cmd_type cmd_type)
{
		enum rtw_phl_status pstatus = RTW_PHL_STATUS_SUCCESS;
		void *phl = GET_PHL_INFO(adapter_to_dvobj(stapriv->padapter));
		struct rtw_wifi_role_t *wrole = stapriv->padapter->phl_role;
		struct rtw_wifi_role_link_t *rlink = sta->padapter_link->wrlink;
		bool alloc = _TRUE, only_hw = _FALSE;

	if (sta != NULL) {
		/* Do not use this function in interrupt context  */
		pstatus = rtw_phl_cmd_alloc_stainfo(phl, &sta->phl_sta,
						(u8 *)hwaddr, wrole,
						dtype,
						main_id,
						rlink,
						alloc, only_hw,
						cmd_type, 0);

		if (sta->phl_sta) {
			rtw_dump_phl_sta_info(RTW_DBGDUMP, sta);
		} else {
			RTW_ERR(FUNC_ADPT_FMT ": fail to alloc PHL sta "
				"for " MAC_FMT " (status=%d)!\n",
				FUNC_ADPT_ARG(stapriv->padapter),
				MAC_ARG(hwaddr),
				pstatus);
		}
	}
}

struct sta_info *rtw_alloc_stainfo(struct sta_priv *stapriv, const u8 *hwaddr,
				enum rtw_device_type dtype, u16 main_id, u8 link_idx, enum phl_cmd_type cmd_type)
{
	struct sta_info *sta;
	/* can use in interrupt context */
	sta = _rtw_alloc_core_stainfo(stapriv, hwaddr);
	sta->padapter_link = GET_LINK(sta->padapter, link_idx);

	/* can not use in interrupt context */
	_rtw_alloc_phl_stainfo(sta, stapriv, hwaddr, dtype,
			main_id, link_idx, cmd_type);

	return sta;
}

u32 rtw_alloc_stainfo_hw(struct	sta_priv *stapriv, struct sta_info *psta)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;
	void *phl = GET_PHL_INFO(adapter_to_dvobj(stapriv->padapter));
	struct rtw_wifi_role_t *wrole = stapriv->padapter->phl_role;
	bool alloc = _TRUE, only_hw = _TRUE;

	if ((psta != NULL) && (psta->phl_sta != NULL) && (psta->phl_sta->active == _FALSE))
		status = rtw_phl_cmd_alloc_stainfo(phl, &psta->phl_sta,
						(u8 *)psta->phl_sta->mac_addr,
						wrole,
						psta->phl_sta->mld->type,
						psta->phl_sta->macid,
						psta->phl_sta->rlink,
						alloc, only_hw,
						PHL_CMD_DIRECTLY, 0);

	return (status == RTW_PHL_STATUS_SUCCESS) ? _SUCCESS : _FAIL;
}

/* using pstapriv->sta_hash_lock to protect */
u32 static _rtw_free_core_stainfo(_adapter *padapter, struct sta_info *psta)
{
	int i;
	_queue *pfree_sta_queue;
	struct recv_reorder_ctrl *preorder_ctrl;
	struct	sta_xmit_priv	*pstaxmitpriv;
	struct	xmit_priv	*pxmitpriv = &padapter->xmitpriv;
	struct	sta_priv *pstapriv = &padapter->stapriv;
	struct hw_xmit *phwxmit;
	struct mlme_ext_priv *pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);

	int pending_qcnt[4];
	u8 is_pre_link_sta = _FALSE;

	if (psta == NULL)
		goto exit;

#if defined(CONFIG_CORE_TXSC) && defined(USE_ONE_WLHDR)
	/* free shortcut entry wlhdr buffer */
	for (i = 0; i < CORE_TXSC_ENTRY_NUM; i++) {
		if (psta->txsc_entry_cache[i].txsc_wlhdr) {
			rtw_mfree(psta->txsc_entry_cache[i].txsc_wlhdr, CORE_TXSC_WLHDR_SIZE);
			psta->txsc_entry_cache[i].txsc_wlhdr = NULL;
		}
	}
#endif

#ifdef CONFIG_RTW_80211K
	rm_post_event(padapter, RM_ID_FOR_ALL(psta->phl_sta->aid), RM_EV_cancel);
#endif
	#if CONFIG_RTW_PRE_LINK_STA
	is_pre_link_sta = rtw_is_pre_link_sta(pstapriv, psta->phl_sta->mac_addr);

	if (is_pre_link_sta == _FALSE) {
		_rtw_spinlock_bh(&(pstapriv->sta_hash_lock));
		rtw_list_delete(&psta->hash_list);
		pstapriv->asoc_sta_count--;
		_rtw_spinunlock_bh(&(pstapriv->sta_hash_lock));
		rtw_mi_update_iface_status(&(padapter->mlmepriv), 0);
	} else {
		_rtw_spinlock_bh(&psta->lock);
		psta->state = WIFI_FW_PRE_LINK;
		_rtw_spinunlock_bh(&psta->lock);
	}
	#else
	_rtw_spinlock_bh(&(pstapriv->sta_hash_lock));
	rtw_list_delete(&psta->hash_list);
	pstapriv->asoc_sta_count--;
	_rtw_spinunlock_bh(&(pstapriv->sta_hash_lock));
	rtw_mi_update_iface_status(&(padapter->mlmepriv), 0);

	#endif

	_rtw_spinlock_bh(&psta->lock);
	psta->state &= ~WIFI_ASOC_STATE;
	_rtw_spinunlock_bh(&psta->lock);

	pfree_sta_queue = &pstapriv->free_sta_queue;


	pstaxmitpriv = &psta->sta_xmitpriv;

	/* rtw_list_delete(&psta->sleep_list); */

	/* rtw_list_delete(&psta->wakeup_list); */

	rtw_free_xmitframe_queue(pxmitpriv, &psta->tx_queue);
	_rtw_deinit_queue(&psta->tx_queue);

	_rtw_spinlock_bh(&pxmitpriv->lock);

	rtw_free_xmitframe_queue(pxmitpriv, &psta->sleep_q);
	psta->sleepq_len = 0;

	/* vo */
	/* _rtw_spinlock_bh(&(pxmitpriv->vo_pending.lock)); */
	rtw_free_xmitframe_queue(pxmitpriv, &pstaxmitpriv->vo_q.sta_pending);
	rtw_list_delete(&(pstaxmitpriv->vo_q.tx_pending));
	phwxmit = pxmitpriv->hwxmits;
	phwxmit->accnt -= pstaxmitpriv->vo_q.qcnt;
	pending_qcnt[0] = pstaxmitpriv->vo_q.qcnt;
	pstaxmitpriv->vo_q.qcnt = 0;
	/* _rtw_spinunlock_bh(&(pxmitpriv->vo_pending.lock)); */

	/* vi */
	/* _rtw_spinlock_bh(&(pxmitpriv->vi_pending.lock)); */
	rtw_free_xmitframe_queue(pxmitpriv, &pstaxmitpriv->vi_q.sta_pending);
	rtw_list_delete(&(pstaxmitpriv->vi_q.tx_pending));
	phwxmit = pxmitpriv->hwxmits + 1;
	phwxmit->accnt -= pstaxmitpriv->vi_q.qcnt;
	pending_qcnt[1] = pstaxmitpriv->vi_q.qcnt;
	pstaxmitpriv->vi_q.qcnt = 0;
	/* _rtw_spinunlock_bh(&(pxmitpriv->vi_pending.lock)); */

	/* be */
	/* _rtw_spinlock_bh(&(pxmitpriv->be_pending.lock)); */
	rtw_free_xmitframe_queue(pxmitpriv, &pstaxmitpriv->be_q.sta_pending);
	rtw_list_delete(&(pstaxmitpriv->be_q.tx_pending));
	phwxmit = pxmitpriv->hwxmits + 2;
	phwxmit->accnt -= pstaxmitpriv->be_q.qcnt;
	pending_qcnt[2] = pstaxmitpriv->be_q.qcnt;
	pstaxmitpriv->be_q.qcnt = 0;
	/* _rtw_spinunlock_bh(&(pxmitpriv->be_pending.lock)); */

	/* bk */
	/* _rtw_spinlock_bh(&(pxmitpriv->bk_pending.lock)); */
	rtw_free_xmitframe_queue(pxmitpriv, &pstaxmitpriv->bk_q.sta_pending);
	rtw_list_delete(&(pstaxmitpriv->bk_q.tx_pending));
	phwxmit = pxmitpriv->hwxmits + 3;
	phwxmit->accnt -= pstaxmitpriv->bk_q.qcnt;
	pending_qcnt[3] = pstaxmitpriv->bk_q.qcnt;
	pstaxmitpriv->bk_q.qcnt = 0;
	/* _rtw_spinunlock_bh(&(pxmitpriv->bk_pending.lock)); */

	rtw_os_wake_queue_at_free_stainfo(padapter, pending_qcnt);

	_rtw_spinunlock_bh(&pxmitpriv->lock);


	/* re-init sta_info; 20061114 */ /* will be init in alloc_stainfo */
	/* _rtw_init_sta_xmit_priv(&psta->sta_xmitpriv); */
	/* _rtw_init_sta_recv_priv(&psta->sta_recvpriv); */
#ifdef CONFIG_IEEE80211W
	_cancel_timer_ex(&psta->dot11w_expire_timer);
#endif /* CONFIG_IEEE80211W */
	_cancel_timer_ex(&psta->addba_retry_timer);

#ifdef CONFIG_TDLS
	psta->tdls_sta_state = TDLS_STATE_NONE;
#endif /* CONFIG_TDLS */

	/* for A-MPDU Rx reordering buffer control, cancel reordering_ctrl_timer */
	for (i = 0; i < 16 ; i++) {
		preorder_ctrl = &psta->recvreorder_ctrl[i];
		rtw_clear_bit(RTW_RECV_ACK_OR_TIMEOUT, &preorder_ctrl->rec_abba_rsp_ack);

#ifdef CONFIG_RECV_REORDERING_CTRL
		_list	*phead, *plist;
		union recv_frame *prframe;
		_queue *ppending_recvframe_queue;

		_cancel_timer_ex(&preorder_ctrl->reordering_ctrl_timer);

		ppending_recvframe_queue = &preorder_ctrl->pending_recvframe_queue;

		_rtw_spinlock_bh(&ppending_recvframe_queue->lock);

		phead =	get_list_head(ppending_recvframe_queue);
		plist = get_next(phead);

		while (!rtw_is_list_empty(phead)) {
			prframe = LIST_CONTAINOR(plist, union recv_frame, u);

			plist = get_next(plist);

			rtw_list_delete(&(prframe->u.hdr.list));

			rtw_free_recvframe(prframe);
		}

		_rtw_spinunlock_bh(&ppending_recvframe_queue->lock);
#endif /*CONFIG_RECV_REORDERING_CTRL*/
	}

	/* CVE-2020-24586, clear defrag queue */
	{
		_list	*phead, *plist;
		_queue *pfree_sta_queue, *pdefrag_q = &psta->sta_recvpriv.defrag_q;
		union recv_frame *prframe;

		_rtw_spinlock_bh(&pdefrag_q->lock);
		phead = get_list_head(pdefrag_q);
		plist = get_next(phead);
		while (!rtw_is_list_empty(phead)) {
			prframe = LIST_CONTAINOR(plist, union recv_frame, u);
			plist = get_next(plist);
			rtw_list_delete(&(prframe->u.hdr.list));
			rtw_free_recvframe(prframe);
		}
		_rtw_spinunlock_bh(&pdefrag_q->lock);
	}

#ifdef CONFIG_AP_MODE

	/*
		_rtw_spinlock_bh(&pstapriv->asoc_list_lock);
		rtw_list_delete(&psta->asoc_list);
		_rtw_spinunlock_bh(&pstapriv->asoc_list_lock);
	*/
	_rtw_spinlock_bh(&pstapriv->auth_list_lock);
	if (!rtw_is_list_empty(&psta->auth_list)) {
		rtw_list_delete(&psta->auth_list);
		pstapriv->auth_list_cnt--;
	}
	_rtw_spinunlock_bh(&pstapriv->auth_list_lock);

	psta->expire_to = 0;
#ifdef CONFIG_ATMEL_RC_PATCH
	psta->flag_atmel_rc = 0;
#endif
	psta->sleepq_ac_len = 0;
	psta->qos_info = 0;

	psta->max_sp_len = 0;
	psta->uapsd_bk = 0;
	psta->uapsd_be = 0;
	psta->uapsd_vi = 0;
	psta->uapsd_vo = 0;

	psta->has_legacy_ac = 0;
	psta->start_active = _FALSE;
	psta->smps_mode = SM_PS_DISABLE;

#ifdef CONFIG_NATIVEAP_MLME

	if (pmlmeinfo->state == _HW_STATE_AP_) {
		rtw_tim_map_clear(padapter, pstapriv->sta_dz_bitmap, psta->phl_sta->aid);
		rtw_tim_map_clear(padapter, pstapriv->tim_bitmap, psta->phl_sta->aid);

		/* rtw_indicate_sta_disassoc_event(padapter, psta); */

		if ((psta->phl_sta->aid > 0) && (pstapriv->sta_aid[psta->phl_sta->aid - 1] == psta)) {
			pstapriv->sta_aid[psta->phl_sta->aid - 1] = NULL;
			psta->phl_sta->aid = 0;
		}
	}

#endif /* CONFIG_NATIVEAP_MLME	 */

#if !defined(CONFIG_ACTIVE_KEEP_ALIVE_CHECK) && defined(CONFIG_80211N_HT)
	psta->under_exist_checking = 0;
#endif

#ifdef CONFIG_RTW_80211R_AP
	rtw_ft_peer_info_free(psta);
#endif
#endif /* CONFIG_AP_MODE	 */

	rtw_st_ctl_deinit(&psta->st_ctl);

	if (is_pre_link_sta == _FALSE) {
		_rtw_spinlock_free(&psta->lock);

		/* _rtw_spinlock_bh(&(pfree_sta_queue->lock)); */
		_rtw_spinlock_bh(&(pstapriv->sta_hash_lock));
		rtw_list_insert_tail(&psta->list, get_list_head(pfree_sta_queue));
		_rtw_spinunlock_bh(&(pstapriv->sta_hash_lock));
		/* _rtw_spinunlock_bh(&(pfree_sta_queue->lock)); */
	}

exit:
	return _SUCCESS;
}

static void _rtw_free_phl_stainfo(_adapter *adapter, struct sta_info *sta, u8 only_free_sw)
{
	void *phl = GET_PHL_INFO(adapter_to_dvobj(adapter));
	enum rtw_phl_status pstaus = RTW_PHL_STATUS_SUCCESS;
	u8 hwaddr[ETH_ALEN] = {0};
	bool alloc = _FALSE, only_hw = _FALSE;

	if (sta != NULL) {
		_rtw_memcpy(hwaddr, sta->phl_sta->mac_addr, ETH_ALEN);
		if (only_free_sw)
			pstaus = rtw_phl_free_stainfo_sw(phl, sta->phl_sta);
		else
			pstaus = rtw_phl_cmd_alloc_stainfo(phl, &sta->phl_sta,
							sta->phl_sta->mac_addr,
							adapter->phl_role,
							sta->phl_sta->mld->type,
							sta->phl_sta->macid,
							sta->phl_sta->rlink,
							alloc, only_hw,
							PHL_CMD_DIRECTLY, 0);

		if (pstaus != RTW_PHL_STATUS_SUCCESS)
			RTW_ERR(FUNC_ADPT_FMT ": fail to free PHL sta "
				"for " MAC_FMT " (status=%d)!\n",
				FUNC_ADPT_ARG(adapter),
				MAC_ARG(hwaddr),
				pstaus);
		else
			/* free stainfo success, set pointer to NULL */
			sta->phl_sta = NULL;
	}
}

u32	rtw_free_stainfo(_adapter *padapter, struct sta_info *psta)
{
	_rtw_free_core_stainfo(padapter, psta);
	_rtw_free_phl_stainfo(padapter, psta, _FALSE);
	return _SUCCESS;
}

u32	rtw_free_stainfo_sw(_adapter *padapter, struct sta_info *psta)
{
	_rtw_free_core_stainfo(padapter, psta);
	_rtw_free_phl_stainfo(padapter, psta, _TRUE);
	return _SUCCESS;
}

/* free all stainfo which in sta_hash[all] */
void rtw_free_all_stainfo(_adapter *padapter)
{
	_list	*plist, *phead;
	s32	index;
	struct sta_info *psta = NULL;
	struct	sta_priv *pstapriv = &padapter->stapriv;
	u8 free_sta_num = 0;
	char free_sta_list[NUM_STA];
	int stainfo_offset;
	struct rtw_phl_mld_t *pmld = NULL;

	if (pstapriv->asoc_sta_count == 1)
		goto exit;

	_rtw_spinlock_bh(&pstapriv->sta_hash_lock);

	for (index = 0; index < NUM_STA; index++) {
		phead = &(pstapriv->sta_hash[index]);
		plist = get_next(phead);

		while ((rtw_end_of_queue_search(phead, plist)) == _FALSE) {
			psta = LIST_CONTAINOR(plist, struct sta_info , hash_list);

			plist = get_next(plist);

			/* pbcmc_stainfo is self stainfo */
			if (!rtw_is_self_stainfo(padapter, psta)) {
				#if CONFIG_RTW_PRE_LINK_STA
				if (rtw_is_pre_link_sta(pstapriv, psta->phl_sta->mac_addr) == _FALSE)
				#endif
					rtw_list_delete(&psta->hash_list);

				stainfo_offset = rtw_stainfo_offset(pstapriv, psta);
				if (stainfo_offset_valid(stainfo_offset))
					free_sta_list[free_sta_num++] = stainfo_offset;
			}

		}
	}

	_rtw_spinunlock_bh(&pstapriv->sta_hash_lock);


	for (index = 0; index < free_sta_num; index++) {
		psta = rtw_get_stainfo_by_offset(pstapriv, free_sta_list[index]);
		pmld = psta->phl_sta->mld;
		if (pmld)
			rtw_phl_free_mld(GET_PHL_INFO(adapter_to_dvobj(padapter)), pmld);
		rtw_free_stainfo(padapter , psta);
	}

exit:
	return;
}

bool rtw_is_self_stainfo(_adapter *padapter, struct sta_info *sta)
{
	struct _ADAPTER_LINK *padapter_link;
	u8 lidx;
	struct rtw_phl_stainfo_t *self_sta = NULL;

	for (lidx = 0; lidx < padapter->adapter_link_num; lidx++) {
		padapter_link = GET_LINK(padapter, lidx);
		self_sta = rtw_phl_get_stainfo_self(adapter_to_dvobj(padapter)->phl, padapter_link->wrlink);
		if (sta->phl_sta == self_sta)
			return _TRUE;
	}
	return _FALSE;
}

u32 rtw_free_mld_stainfo(_adapter *padapter, struct rtw_phl_mld_t *mld)
{
	u8 lidx;
	struct sta_info *psta = NULL;
	struct rtw_phl_stainfo_t *phl_sta = NULL;
	if (!mld)
		return _SUCCESS;
	for (lidx = 0; lidx < mld->sta_num; lidx++) {
		phl_sta = mld->phl_sta[lidx];
		if (phl_sta == NULL)
			continue;
		psta = rtw_get_stainfo(&(padapter->stapriv), phl_sta->mac_addr);
		if (psta == NULL)
			continue;
		_rtw_free_core_stainfo(padapter, psta);
		_rtw_free_phl_stainfo(padapter, psta, _FALSE);
	}
	rtw_phl_free_mld(GET_PHL_INFO(adapter_to_dvobj(padapter)), mld);
	return _SUCCESS;
}

struct sta_info *rtw_get_stainfo_by_macid(struct sta_priv *pstapriv, u16 macid)
{
	void *phl = GET_PHL_INFO(adapter_to_dvobj(pstapriv->padapter));
	struct sta_info *sta = NULL;
	struct rtw_phl_stainfo_t *phl_sta = NULL;

	phl_sta = rtw_phl_get_stainfo_by_macid(phl, macid);
	if (phl_sta)
		sta = rtw_get_stainfo(pstapriv, phl_sta->mac_addr);
	return sta;
}

/* any station allocated can be searched by hash list */
struct sta_info *rtw_get_stainfo(struct sta_priv *pstapriv, const u8 *hwaddr)
{
	_list	*plist, *phead;
	struct sta_info *psta = NULL;
	struct rtw_phl_stainfo_t *phl_sta_self = NULL;
	void *phl = GET_PHL_INFO(adapter_to_dvobj(pstapriv->padapter));
	struct rtw_wifi_role_t *phl_role;
	u8 link_addr[ETH_ALEN] = {0};
	struct rtw_phl_mld_t *mld = NULL;
	u8 is_link_addr = _FALSE;
	struct rtw_phl_stainfo_t *phl_sta = NULL;
	u8 lidx;
	u32 index;

	if (hwaddr == NULL)
		return NULL;

	if (pstapriv->padapter->phl_role == NULL) {
		RTW_ERR(FUNC_ADPT_FMT" phl_role == NULL\n", FUNC_ADPT_ARG(pstapriv->padapter));
		rtw_warn_on(1);
		return NULL;
	}
	phl_role = pstapriv->padapter->phl_role;

	_rtw_memcpy(link_addr, hwaddr, ETH_ALEN);

	/* core layer only maintain stainfo with link address
	** so first check if hwaddr is mld address or link address
	** if hwaddr is mld address, replace it with primary link address */
	mld = rtw_phl_get_mld_by_addr(phl, phl_role, link_addr);
	if (mld) {
		for (lidx = 0; lidx < mld->sta_num; lidx++) {
			phl_sta = rtw_phl_get_stainfo_by_mld(mld, lidx);
			if (phl_sta == NULL)
				continue;
			if (_rtw_memcmp(link_addr, phl_sta->mac_addr, ETH_ALEN) == _TRUE) {
				is_link_addr = _TRUE;
				break;
			}
		}
		if (!is_link_addr && mld->phl_sta[RTW_RLINK_PRIMARY] != NULL)
			_rtw_memcpy(link_addr, mld->phl_sta[RTW_RLINK_PRIMARY]->mac_addr, ETH_ALEN);
	}

	/* if addr is bcmc addr, return self phl_sta */
	if (IS_MCAST(link_addr)) {
		/* ToDo CONFIG_RTW_MLD: bcmc self stainfo? */
		phl_sta_self = rtw_phl_get_stainfo_self(phl, &(phl_role->rlink[RTW_RLINK_PRIMARY]));

		if (phl_sta_self == NULL) {
			RTW_INFO("%s: get phl sta self fail", __func__);
			return NULL;
		}

		/*
		due to using self sta to replace bcmc sta
		change index to self hash index
		*/

		index = wifi_mac_hash(phl_role->mac_addr);
	} else {

		/*
		for sta mode due to self sta info & AP sta info are the same sta info
		using self hash index for sta mode, or bmc sta will not found self sta
		*/
		switch (phl_role->type) {
		case PHL_RTYPE_NONE:
		case PHL_RTYPE_STATION:
		case PHL_RTYPE_ADHOC:
		case PHL_RTYPE_P2P_DEVICE:
		case PHL_RTYPE_P2P_GC:
			index = wifi_mac_hash(phl_role->mac_addr);
		break;
		case PHL_RTYPE_TDLS:
			if (_rtw_memcmp(link_addr, pstapriv->padapter->mlmepriv.dev_cur_network.network.MacAddress, ETH_ALEN) == _TRUE)
				index = wifi_mac_hash(phl_role->mac_addr);
			else
				index = wifi_mac_hash(link_addr);
		break;
		default:
			index = wifi_mac_hash(link_addr);
		break;
		}
	}

	_rtw_spinlock_bh(&pstapriv->sta_hash_lock);

	phead = &(pstapriv->sta_hash[index]);
	plist = get_next(phead);


	while ((rtw_end_of_queue_search(phead, plist)) == _FALSE) {

		psta = LIST_CONTAINOR(plist, struct sta_info, hash_list);

		if (psta->phl_sta == NULL) {
			psta = NULL;
			RTW_DBG("phl_sta of sta is NULL\n");
			plist = get_next(plist);
			continue;
		}

		if (IS_MCAST(link_addr) && psta->phl_sta == phl_sta_self)
			break;

		/* if add is not bcmc addr, compare mac_addr  */
		if ((_rtw_memcmp(psta->phl_sta->mac_addr, link_addr, ETH_ALEN)) == _TRUE) {
			/* if found the matched address */
			break;
		}
		psta = NULL;
		plist = get_next(plist);
	}

	_rtw_spinunlock_bh(&pstapriv->sta_hash_lock);
	return psta;

}

u32	rtw_free_self_stainfo(_adapter *adapter)
{
	struct sta_info *sta = NULL;
	struct sta_priv *stapriv = &adapter->stapriv;
	struct rtw_phl_mld_t *mld = NULL;
	u8 lidx;

	mld = rtw_phl_get_mld_self(GET_PHL_INFO(adapter_to_dvobj(adapter)), adapter->phl_role);
	for (lidx = 0; lidx < mld->sta_num; lidx++) {
		sta = rtw_get_stainfo(stapriv, mld->phl_sta[lidx]->mac_addr);
		if (sta != NULL) {
			_rtw_free_core_stainfo(adapter, sta);
			_rtw_free_phl_stainfo(adapter, sta, _FALSE);
		}
	}
	return _SUCCESS;
}

u32 rtw_init_self_stainfo(_adapter *padapter, enum phl_cmd_type cmd_type)
{

	struct sta_info *psta;
	struct tx_servq *ptxservq;
	u32 res = _SUCCESS;
	struct sta_priv *pstapriv = &padapter->stapriv;
	void *phl = GET_PHL_INFO(adapter_to_dvobj(padapter));
	u8 lidx;
	struct rtw_phl_mld_t *mld = NULL;
	struct rtw_wifi_role_link_t *rlink = NULL;
	u16 main_id = rtw_phl_get_macid_max_num(phl);

	mld = rtw_phl_get_mld_by_addr(GET_PHL_INFO(adapter_to_dvobj(padapter)),
				padapter->phl_role,
				padapter->phl_role->mac_addr);
	if (mld == NULL)
		mld = rtw_phl_alloc_mld(GET_PHL_INFO(adapter_to_dvobj(padapter)),
						padapter->phl_role,
						padapter->phl_role->mac_addr,
						DTYPE);
	for (lidx = 0; lidx < mld->sta_num; lidx++) {
		rlink = &(padapter->phl_role->rlink[lidx]);
		psta = rtw_get_stainfo(pstapriv, rlink->mac_addr);
		if (psta == NULL) {
			psta = rtw_alloc_stainfo(pstapriv, rlink->mac_addr, DTYPE, main_id, lidx, cmd_type);
			if (psta == NULL) {
				RTW_ERR("%s alloc self sta fail\n", __func__);
				res = _FAIL;
				goto exit;
			}
		}
		main_id = psta->phl_sta->macid;
	}
	/* self mld and self stainfo, no need to link again */
exit:
	return res;

}


struct sta_info *rtw_get_bcmc_stainfo(_adapter *padapter, struct _ADAPTER_LINK *padapter_link)
{
	struct sta_info	*psta;
	struct sta_priv	*pstapriv = &padapter->stapriv;
	u8 bc_addr[ETH_ALEN] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	/* return self sta */
	_list	*plist, *phead;
	struct rtw_phl_stainfo_t *phl_sta_self = NULL;
	u32 index;

	phl_sta_self = rtw_phl_get_stainfo_self(GET_PHL_INFO(adapter_to_dvobj(padapter)), padapter_link->wrlink);

	if (phl_sta_self == NULL) {
		RTW_INFO("%s: get phl sta self fail", __func__);
		return NULL;
	}
	index = wifi_mac_hash(padapter->phl_role->mac_addr);

	_rtw_spinlock_bh(&pstapriv->sta_hash_lock);

	phead = &(pstapriv->sta_hash[index]);
	plist = get_next(phead);

	while ((rtw_end_of_queue_search(phead, plist)) == _FALSE) {

		psta = LIST_CONTAINOR(plist, struct sta_info, hash_list);

		if (psta->phl_sta == NULL) {
			psta = NULL;
			RTW_ERR("phl_sta of sta is NULL\n");
			plist = get_next(plist);
			continue;
		}

		if (psta->phl_sta == phl_sta_self)
			break;

		psta = NULL;
		plist = get_next(plist);
	}

	_rtw_spinunlock_bh(&pstapriv->sta_hash_lock);
	return psta;

}

#ifdef CONFIG_AP_MODE
u16 rtw_aid_alloc(_adapter *adapter, struct sta_info *sta)
{
	struct sta_priv *stapriv = &adapter->stapriv;
	u16 aid, i, used_cnt = 0;

	for (i = 0; i < stapriv->max_aid; i++) {
		aid = ((i + stapriv->started_aid - 1) % stapriv->max_aid) + 1;
		if (stapriv->sta_aid[aid - 1] == NULL)
			break;
		if (++used_cnt >= stapriv->max_num_sta)
			break;
	}

	/* check for aid limit and assoc limit  */
	if (i >= stapriv->max_aid || used_cnt >= stapriv->max_num_sta)
		aid = 0;

	sta->phl_sta->aid = aid;
	if (aid) {
		stapriv->sta_aid[aid - 1] = sta;
		if (stapriv->rr_aid)
			stapriv->started_aid = (aid % stapriv->max_aid) + 1;
	}

	return aid;
}

void dump_aid_status(void *sel, _adapter *adapter)
{
	struct sta_priv *stapriv = &adapter->stapriv;
	u8 *aid_bmp;
	u16 i, used_cnt = 0;

	aid_bmp = rtw_zmalloc(stapriv->aid_bmp_len);
	if (!aid_bmp)
		return;

	for (i = 1; i <= stapriv->max_aid; i++) {
		if (stapriv->sta_aid[i - 1]) {
			aid_bmp[i / 8] |= BIT(i % 8);
			++used_cnt;
		}
	}

	RTW_PRINT_SEL(sel, "used_cnt:%u/%u\n", used_cnt, stapriv->max_aid);
	RTW_MAP_DUMP_SEL(sel, "aid_map:", aid_bmp, stapriv->aid_bmp_len);
	RTW_PRINT_SEL(sel, "\n");

	RTW_PRINT_SEL(sel, "%-2s %-11s\n", "rr", "started_aid");
	RTW_PRINT_SEL(sel, "%2d %11d\n", stapriv->rr_aid, stapriv->started_aid);

	rtw_mfree(aid_bmp, stapriv->aid_bmp_len);
}
#endif /* CONFIG_AP_MODE */

#if CONFIG_RTW_MACADDR_ACL
const char *const _acl_period_str[RTW_ACL_PERIOD_NUM] = {
	"DEV",
	"BSS",
};

const char *const _acl_mode_str[RTW_ACL_MODE_MAX] = {
	"DISABLED",
	"ACCEPT_UNLESS_LISTED",
	"DENY_UNLESS_LISTED",
};

u8 _rtw_access_ctrl(_adapter *adapter, u8 period, const u8 *mac_addr)
{
	u8 res = _TRUE;
	_list *list, *head;
	struct rtw_wlan_acl_node *acl_node;
	u8 match = _FALSE;
	struct sta_priv *stapriv = &adapter->stapriv;
	struct wlan_acl_pool *acl;
	_queue	*acl_node_q;

	if (period >= RTW_ACL_PERIOD_NUM) {
		rtw_warn_on(1);
		goto exit;
	}

	acl = &stapriv->acl_list[period];
	acl_node_q = &acl->acl_node_q;

	if (acl->mode != RTW_ACL_MODE_ACCEPT_UNLESS_LISTED
		&& acl->mode != RTW_ACL_MODE_DENY_UNLESS_LISTED)
		goto exit;

	_rtw_spinlock_bh(&(acl_node_q->lock));
	head = get_list_head(acl_node_q);
	list = get_next(head);
	while (rtw_end_of_queue_search(head, list) == _FALSE) {
		acl_node = LIST_CONTAINOR(list, struct rtw_wlan_acl_node, list);
		list = get_next(list);

		if (_rtw_memcmp(acl_node->addr, mac_addr, ETH_ALEN)) {
			if (acl_node->valid == _TRUE) {
				match = _TRUE;
				break;
			}
		}
	}
	_rtw_spinunlock_bh(&(acl_node_q->lock));

	if (acl->mode == RTW_ACL_MODE_ACCEPT_UNLESS_LISTED)
		res = (match == _TRUE) ?  _FALSE : _TRUE;
	else /* RTW_ACL_MODE_DENY_UNLESS_LISTED */
		res = (match == _TRUE) ?  _TRUE : _FALSE;

exit:
	return res;
}

struct sta_info *rtw_alloc_stainfo_sw(struct sta_priv *stapriv, enum rtw_device_type dtype,
				u16 main_id, u8 link_idx, const u8 *hwaddr)
{
	struct sta_info *sta;
	struct rtw_phl_com_t *phl_com = GET_PHL_COM(adapter_to_dvobj(stapriv->padapter));
	struct rtw_wifi_role_link_t *rlink = NULL;

	/* can use in interrupt context */
	sta = _rtw_alloc_core_stainfo(stapriv, hwaddr);
	sta->padapter_link = GET_LINK(sta->padapter, link_idx);
	rlink = sta->padapter_link->wrlink;

	if (sta != NULL) {
		sta->phl_sta = rtw_phl_alloc_stainfo_sw(
			GET_PHL_INFO(adapter_to_dvobj(stapriv->padapter)),
			(u8 *)hwaddr, stapriv->padapter->phl_role
			, dtype
			, main_id
			, rlink
			);

		if (sta->phl_sta) {
			rtw_dump_phl_sta_info(RTW_DBGDUMP, sta);
		} else {
			RTW_ERR(FUNC_ADPT_FMT ": fail to alloc PHL sta "
				"for " MAC_FMT " !\n",
				FUNC_ADPT_ARG(stapriv->padapter),
				MAC_ARG(hwaddr));
			_rtw_free_core_stainfo(stapriv->padapter, sta);
			sta = NULL;
		}
	}

	return sta;
}

u8 rtw_access_ctrl(_adapter *adapter, const u8 *mac_addr)
{
	int i;

	for (i = 0; i < RTW_ACL_PERIOD_NUM; i++)
		if (_rtw_access_ctrl(adapter, i, mac_addr) == _FALSE)
			return _FALSE;

	return _TRUE;
}

void dump_macaddr_acl(void *sel, _adapter *adapter)
{
	struct sta_priv *stapriv = &adapter->stapriv;
	struct wlan_acl_pool *acl;
	int i, j;

	for (j = 0; j < RTW_ACL_PERIOD_NUM; j++) {
		RTW_PRINT_SEL(sel, "period:%s(%d)\n", acl_period_str(j), j);

		acl = &stapriv->acl_list[j];
		RTW_PRINT_SEL(sel, "mode:%s(%d)\n", acl_mode_str(acl->mode), acl->mode);
		RTW_PRINT_SEL(sel, "num:%d/%d\n", acl->num, NUM_ACL);
		for (i = 0; i < NUM_ACL; i++) {
			if (acl->aclnode[i].valid == _FALSE)
				continue;
			RTW_PRINT_SEL(sel, MAC_FMT"\n", MAC_ARG(acl->aclnode[i].addr));
		}
		RTW_PRINT_SEL(sel, "\n");
	}
}
#endif /* CONFIG_RTW_MACADDR_ACL */

#if CONFIG_RTW_PRE_LINK_STA
bool rtw_is_pre_link_sta(struct sta_priv *stapriv, u8 *addr)
{
	struct pre_link_sta_ctl_t *pre_link_sta_ctl = &stapriv->pre_link_sta_ctl;
	struct sta_info *sta = NULL;
	u8 exist = _FALSE;
	int i;

	_rtw_spinlock_bh(&(pre_link_sta_ctl->lock));
	for (i = 0; i < RTW_PRE_LINK_STA_NUM; i++) {
		if (pre_link_sta_ctl->node[i].valid == _TRUE
			&& _rtw_memcmp(pre_link_sta_ctl->node[i].addr, addr, ETH_ALEN) == _TRUE
		) {
			exist = _TRUE;
			break;
		}
	}
	_rtw_spinunlock_bh(&(pre_link_sta_ctl->lock));

	return exist;
}
#endif

#if CONFIG_RTW_PRE_LINK_STA
struct sta_info *rtw_pre_link_sta_add(struct sta_priv *stapriv, u8 *hwaddr)
{
	struct pre_link_sta_ctl_t *pre_link_sta_ctl = &stapriv->pre_link_sta_ctl;
	struct pre_link_sta_node_t *node = NULL;
	struct sta_info *sta = NULL;
	u8 exist = _FALSE;
	int i;

	if (rtw_check_invalid_mac_address(hwaddr, _FALSE) == _TRUE)
		goto exit;

	_rtw_spinlock_bh(&(pre_link_sta_ctl->lock));
	for (i = 0; i < RTW_PRE_LINK_STA_NUM; i++) {
		if (pre_link_sta_ctl->node[i].valid == _TRUE
			&& _rtw_memcmp(pre_link_sta_ctl->node[i].addr, hwaddr, ETH_ALEN) == _TRUE
		) {
			node = &pre_link_sta_ctl->node[i];
			exist = _TRUE;
			break;
		}

		if (node == NULL && pre_link_sta_ctl->node[i].valid == _FALSE)
			node = &pre_link_sta_ctl->node[i];
	}

	if (exist == _FALSE && node) {
		_rtw_memcpy(node->addr, hwaddr, ETH_ALEN);
		node->valid = _TRUE;
		pre_link_sta_ctl->num++;
	}
	_rtw_spinunlock_bh(&(pre_link_sta_ctl->lock));

	if (node == NULL)
		goto exit;

	sta = rtw_alloc_stainfo(stapriv, hwaddr, PHL_CMD_WAIT);
	if (!sta)
		goto exit;

	sta->state = WIFI_FW_PRE_LINK;

exit:
	return sta;
}

void rtw_pre_link_sta_del(struct sta_priv *stapriv, u8 *hwaddr)
{
	struct pre_link_sta_ctl_t *pre_link_sta_ctl = &stapriv->pre_link_sta_ctl;
	struct pre_link_sta_node_t *node = NULL;
	struct sta_info *sta = NULL;
	u8 exist = _FALSE;
	int i;
	/* ToDo CONFIG_RTW_MLD: free per MLD or per sta? */

	if (rtw_check_invalid_mac_address(hwaddr, _FALSE) == _TRUE)
		goto exit;

	_rtw_spinlock_bh(&(pre_link_sta_ctl->lock));
	for (i = 0; i < RTW_PRE_LINK_STA_NUM; i++) {
		if (pre_link_sta_ctl->node[i].valid == _TRUE
			&& _rtw_memcmp(pre_link_sta_ctl->node[i].addr, hwaddr, ETH_ALEN) == _TRUE
		) {
			node = &pre_link_sta_ctl->node[i];
			exist = _TRUE;
			break;
		}
	}

	if (exist == _TRUE && node) {
		node->valid = _FALSE;
		pre_link_sta_ctl->num--;
	}
	_rtw_spinunlock_bh(&(pre_link_sta_ctl->lock));

	if (exist == _FALSE)
		goto exit;

	sta = rtw_get_stainfo(stapriv, hwaddr);
	if (!sta)
		goto exit;

	if (sta->state == WIFI_FW_PRE_LINK)
		rtw_free_stainfo(stapriv->padapter, sta);

exit:
	return;
}

void rtw_pre_link_sta_ctl_reset(struct sta_priv *stapriv)
{
	struct pre_link_sta_ctl_t *pre_link_sta_ctl = &stapriv->pre_link_sta_ctl;
	struct pre_link_sta_node_t *node = NULL;
	struct sta_info *sta = NULL;
	int i, j = 0;
	/* ToDo CONFIG_RTW_MLD: reset per MLD or per sta? */

	u8 addrs[RTW_PRE_LINK_STA_NUM][ETH_ALEN];

	_rtw_memset(addrs, 0, RTW_PRE_LINK_STA_NUM * ETH_ALEN);

	_rtw_spinlock_bh(&(pre_link_sta_ctl->lock));
	for (i = 0; i < RTW_PRE_LINK_STA_NUM; i++) {
		if (pre_link_sta_ctl->node[i].valid == _FALSE)
			continue;
		_rtw_memcpy(&(addrs[j][0]), pre_link_sta_ctl->node[i].addr, ETH_ALEN);
		pre_link_sta_ctl->node[i].valid = _FALSE;
		pre_link_sta_ctl->num--;
		j++;
	}
	_rtw_spinunlock_bh(&(pre_link_sta_ctl->lock));

	for (i = 0; i < j; i++) {
		sta = rtw_get_stainfo(stapriv, &(addrs[i][0]));
		if (!sta)
			continue;

		if (sta->state == WIFI_FW_PRE_LINK)
			rtw_free_stainfo(stapriv->padapter, sta);
	}
}

void rtw_pre_link_sta_ctl_init(struct sta_priv *stapriv)
{
	struct pre_link_sta_ctl_t *pre_link_sta_ctl = &stapriv->pre_link_sta_ctl;
	int i;

	_rtw_spinlock_init(&pre_link_sta_ctl->lock);
	pre_link_sta_ctl->num = 0;
	for (i = 0; i < RTW_PRE_LINK_STA_NUM; i++)
		pre_link_sta_ctl->node[i].valid = _FALSE;
}

void rtw_pre_link_sta_ctl_deinit(struct sta_priv *stapriv)
{
	struct pre_link_sta_ctl_t *pre_link_sta_ctl = &stapriv->pre_link_sta_ctl;
	int i;

	rtw_pre_link_sta_ctl_reset(stapriv);

	_rtw_spinlock_free(&pre_link_sta_ctl->lock);
}

void dump_pre_link_sta_ctl(void *sel, struct sta_priv *stapriv)
{
	struct pre_link_sta_ctl_t *pre_link_sta_ctl = &stapriv->pre_link_sta_ctl;
	int i;

	RTW_PRINT_SEL(sel, "num:%d/%d\n", pre_link_sta_ctl->num, RTW_PRE_LINK_STA_NUM);

	for (i = 0; i < RTW_PRE_LINK_STA_NUM; i++) {
		if (pre_link_sta_ctl->node[i].valid == _FALSE)
			continue;
		RTW_PRINT_SEL(sel, MAC_FMT"\n", MAC_ARG(pre_link_sta_ctl->node[i].addr));
	}
}
#endif /* CONFIG_RTW_PRE_LINK_STA */


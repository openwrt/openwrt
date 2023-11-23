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

#include <drv_types.h>

#ifdef CONFIG_CORE_TXSC
u8 DBG_PRINT_MDATA_ONCE;
void _print_txreq_mdata(struct rtw_t_meta_data *mdata, const char *func)
{
	if (DBG_PRINT_MDATA_ONCE == 1) {
		RTW_PRINT("[%s]\n", func);

		RTW_PRINT("da: %02x%02x%02x%02x%02x%02x\n",
			mdata->da[0], mdata->da[1], mdata->da[2], mdata->da[3], mdata->da[4], mdata->da[5]);
		RTW_PRINT("sa: %02x%02x%02x%02x%02x%02x\n",
			mdata->sa[0], mdata->sa[1], mdata->sa[2], mdata->sa[3], mdata->sa[4], mdata->sa[5]);
		RTW_PRINT("to_ds: %d\n", mdata->to_ds);
		RTW_PRINT("from_ds: %d\n", mdata->from_ds);
		RTW_PRINT("band: %d\n", mdata->band);
		RTW_PRINT("type: %d\n", mdata->type);

		RTW_PRINT("hw_seq_mode: %d\n", mdata->hw_seq_mode);
		RTW_PRINT("hw_ssn_sel: %d\n", mdata->hw_ssn_sel);
		RTW_PRINT("smh_en: %d\n", mdata->smh_en);
		RTW_PRINT("hw_amsdu: %d\n", mdata->hw_amsdu);
		RTW_PRINT("hw_sec_iv: %d\n", mdata->hw_sec_iv);
		RTW_PRINT("wd_page_size: %d\n", mdata->wd_page_size);
		RTW_PRINT("hdr_len: %d\n", mdata->hdr_len);
		RTW_PRINT("dma_ch: %d\n", mdata->dma_ch);
		RTW_PRINT("usb_pkt_ofst: %d\n", mdata->usb_pkt_ofst);
		RTW_PRINT("wdinfo_en: %d\n", mdata->wdinfo_en);
		RTW_PRINT("wp_offset: %d\n", mdata->wp_offset);
		RTW_PRINT("shcut_camid: %d\n", mdata->shcut_camid);
		RTW_PRINT("usb_txagg_num: %d\n", mdata->usb_txagg_num);
		RTW_PRINT("pktlen: %d\n", mdata->pktlen);
		RTW_PRINT("tid: %d\n", mdata->tid);
		RTW_PRINT("cat: %d\n", mdata->cat);
		RTW_PRINT("macid: %d\n", mdata->macid);
		RTW_PRINT("sw_seq: %d\n", mdata->sw_seq);
		RTW_PRINT("ampdu_en: %d\n", mdata->ampdu_en);
		RTW_PRINT("bk: %d\n", mdata->bk);
		RTW_PRINT("mbssid: %d\n", mdata->mbssid);
		RTW_PRINT("hal_port: %d\n", mdata->hal_port);
		RTW_PRINT("data_bw_er: %d\n", mdata->data_bw_er);
		RTW_PRINT("dis_rts_rate_fb: %d\n", mdata->dis_rts_rate_fb);
		RTW_PRINT("dis_data_rate_fb: %d\n", mdata->dis_data_rate_fb);
		RTW_PRINT("f_ldpc: %d\n", mdata->f_ldpc);
		RTW_PRINT("f_stbc: %d\n", mdata->f_stbc);
		RTW_PRINT("f_dcm: %d\n", mdata->f_dcm);
		RTW_PRINT("f_er: %d\n", mdata->f_er);
		RTW_PRINT("f_rate: %d\n", mdata->f_rate);
		RTW_PRINT("f_gi_ltf: %d\n", mdata->f_gi_ltf);
		RTW_PRINT("f_bw: %d\n", mdata->f_bw);
		RTW_PRINT("userate_sel: %d\n", mdata->userate_sel);
		RTW_PRINT("ack_ch_info: %d\n", mdata->ack_ch_info);
		RTW_PRINT("max_agg_num: %d\n", mdata->max_agg_num);
		RTW_PRINT("nav_use_hdr: %d\n", mdata->nav_use_hdr);
		RTW_PRINT("bc: %d\n", mdata->bc);
		RTW_PRINT("mc: %d\n", mdata->mc);
		RTW_PRINT("a_ctrl_bqr: %d\n", mdata->a_ctrl_bqr);
		RTW_PRINT("a_ctrl_uph: %d\n", mdata->a_ctrl_uph);
		RTW_PRINT("a_ctrl_bsr: %d\n", mdata->a_ctrl_bsr);
		RTW_PRINT("a_ctrl_cas: %d\n", mdata->a_ctrl_cas);
		RTW_PRINT("data_rty_lowest_rate: %d\n", mdata->data_rty_lowest_rate);
		RTW_PRINT("data_tx_cnt_lmt: %d\n", mdata->data_tx_cnt_lmt);
		RTW_PRINT("data_tx_cnt_lmt_en: %d\n", mdata->data_tx_cnt_lmt_en);
		RTW_PRINT("sec_cam_idx: %d\n", mdata->sec_cam_idx);
		RTW_PRINT("sec_hw_enc: %d\n", mdata->sec_hw_enc);
		RTW_PRINT("sec_type: %d\n", mdata->sec_type);
		RTW_PRINT("life_time_sel: %d\n", mdata->life_time_sel);
		RTW_PRINT("ampdu_density: %d\n", mdata->ampdu_density);
		RTW_PRINT("no_ack: %d\n", mdata->no_ack);
		RTW_PRINT("ndpa: %d\n", mdata->ndpa);
		RTW_PRINT("snd_pkt_sel: %d\n", mdata->snd_pkt_sel);
		RTW_PRINT("sifs_tx: %d\n", mdata->sifs_tx);
		RTW_PRINT("rtt_en: %d\n", mdata->rtt_en);
		RTW_PRINT("spe_rpt: %d\n", mdata->spe_rpt);
		RTW_PRINT("raw: %d\n", mdata->raw);
		RTW_PRINT("sw_define: %d\n", mdata->sw_define);
		RTW_PRINT("rts_en: %d\n", mdata->rts_en);
		RTW_PRINT("cts2self: %d\n", mdata->cts2self);
		RTW_PRINT("rts_cca_mode: %d\n", mdata->rts_cca_mode);
		RTW_PRINT("hw_rts_en: %d\n", mdata->hw_rts_en);

		DBG_PRINT_MDATA_ONCE = 0;
	}
}

u8 DBG_PRINT_TXREQ_ONCE;
void _print_txreq_pklist(struct xmit_frame *pxframe, struct rtw_xmit_req *ptxreq, struct sk_buff *pskb, const char *func)
{
	struct rtw_pkt_buf_list *pkt_list = NULL;
	struct rtw_xmit_req *txreq = NULL;
	u8 pkt_cnt = 0, i;

	if (DBG_PRINT_TXREQ_ONCE == 1) {
		RTW_PRINT("%s\n", func);
		RTW_PRINT("[%s] pxframe=%p txreq=%p\n", func, pxframe, ptxreq);

		if (pskb)
			txsc_dump_data(pskb->data, ETH_HLEN, "ETHHDR");

		if (ptxreq != NULL)
			txreq = ptxreq;
		else
			txreq = pxframe->phl_txreq;

		pkt_list = (struct rtw_pkt_buf_list *)txreq->pkt_list;
		pkt_cnt = txreq->pkt_cnt;

		RTW_PRINT("os_priv:%p, treq_type:%d, pkt_cnt:%d, total_len:%d, shortcut_id:%d\n\n",
				txreq->os_priv, txreq->treq_type, txreq->pkt_cnt, txreq->total_len, txreq->shortcut_id);

		for (i = 0; i < pkt_cnt; i++) {
			RTW_PRINT("pkt_list[%d]\n", i);
			txsc_dump_data(pkt_list->vir_addr, pkt_list->length, "pkt_list");
			pkt_list++;
		}
		DBG_PRINT_TXREQ_ONCE = 0;
	}
}

void txsc_init(_adapter *padapter)
{
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;

	pxmitpriv->txsc_enable = 1; /* default TXSC on */
	pxmitpriv->txsc_debug_mode = 0;
	pxmitpriv->txsc_debug_mask = 0x3;
}

void txsc_clear(_adapter *padapter)
{
	struct sta_priv	*pstapriv = &padapter->stapriv;
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;
	struct sta_info *psta = NULL;
	int i;
	u8 tmp = 0;

	RTW_PRINT("[TXSC] clear txsc entry\n");

	_rtw_spinlock_bh(&pxmitpriv->txsc_lock);

	if (pxmitpriv->txsc_enable) {
		tmp = pxmitpriv->txsc_enable;
		pxmitpriv->txsc_enable = 0;
	}

	pxmitpriv->ptxsc_sta_cached = NULL;

	for (i = 0; i < pstapriv->max_num_sta; i++) {
		psta = pstapriv->sta_aid[i];

		if (!psta)
			continue;

		psta->txsc_cache_num = 0;
		psta->txsc_cur_idx = 0;
		psta->txsc_cache_idx = 0;
		psta->txsc_cache_hit = 0;
		psta->txsc_cache_miss = 0;
		psta->txsc_path_slow = 0;
		_rtw_memset(psta->txsc_entry_cache, 0x0, sizeof(struct txsc_entry) * CORE_TXSC_ENTRY_NUM);

	}

	pxmitpriv->txsc_enable = tmp;

	_rtw_spinunlock_bh(&pxmitpriv->txsc_lock);
}

void txsc_dump(_adapter *padapter)
{
	struct sta_priv	*pstapriv = &padapter->stapriv;
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;
	struct sta_info *psta = NULL;
	int i, j;

	RTW_PRINT("[txsc][core] (txsc,enable) txsc_enable:%x\n", pxmitpriv->txsc_enable);
	RTW_PRINT("[txsc][core] (txsc,debug) txsc_debug_mode:%x\n", pxmitpriv->txsc_debug_mode);

	RTW_PRINT("[txsc][core] txsc_phl_err_cnt1:%d\n", pxmitpriv->txsc_phl_err_cnt1);
	RTW_PRINT("[txsc][core] txsc_phl_err_cnt2:%d\n", pxmitpriv->txsc_phl_err_cnt2);
	RTW_PRINT("\n");
	for (i = 0; i < pstapriv->max_num_sta; i++) {
		psta = pstapriv->sta_aid[i];
		if (!psta)
			continue;

		RTW_PRINT("[%d] STA[%02x:%02x:%02x:%02x:%02x:%02x]\n", i,
			psta->phl_sta->mac_addr[0], psta->phl_sta->mac_addr[1], psta->phl_sta->mac_addr[2],
			psta->phl_sta->mac_addr[3], psta->phl_sta->mac_addr[4], psta->phl_sta->mac_addr[5]);
		RTW_PRINT("[txsc] cur_idx:%d\n", psta->txsc_cur_idx);
		RTW_PRINT("[txsc][core] txsc_path_slow:%d\n", psta->txsc_path_slow);
		RTW_PRINT("[txsc][core] txsc_path_ps:%d\n", psta->txsc_path_ps);
		RTW_PRINT("[txsc][core] txsc_cache_hit:%d\n", psta->txsc_cache_hit);
		RTW_PRINT("[txsc][core] txsc_cache_miss:%d\n", psta->txsc_cache_miss);
		RTW_PRINT("\n");
		for (j = 0 ; j < CORE_TXSC_ENTRY_NUM; j++) {
			if (!psta->txsc_entry_cache[j].txsc_is_used)
				continue;

			RTW_PRINT(" [%d][txsc][core] txsc_core_hit:%d\n", j, psta->txsc_entry_cache[j].txsc_cache_hit);
			#ifdef CONFIG_PHL_TXSC
			RTW_PRINT(" [%d][txsc][phl]  txsc_phl_hit:%d\n", j, psta->phl_sta->phl_txsc[j].txsc_cache_hit);
			#endif
			RTW_PRINT("\n");
		}
	}
}

void txsc_dump_data(u8 *buf, u16 buf_len, const char *prefix)
{
	int i = 0, j;

	RTW_PRINT("[txsc_dump] [%s (%uB)]@%p:\n", prefix, buf_len, buf);

	if (buf == NULL) {
		RTW_PRINT("[txsc_dump] NULL!\n");
		return;
	}

	while (i < buf_len) {
		RTW_PRINT("[txsc_dump] %04X -", i);
		for (j = 0; (j < 4) && (i < buf_len); j++, i += 4)
			RTW_PRINT("  %02X %02X %02X %02X", buf[i], buf[i+1], buf[i+2], buf[i+3]);
		RTW_PRINT("\n");
	}
}

#ifdef CONFIG_PCI_HCI
void txsc_recycle_txreq_phyaddr(_adapter *padapter, struct rtw_xmit_req *txreq)
{
	PPCI_DATA pci_data = dvobj_to_pci(padapter->dvobj);
	struct pci_dev *pdev = pci_data->ppcidev;
	struct rtw_pkt_buf_list *pkt_list = (struct rtw_pkt_buf_list *)txreq->pkt_list;
	dma_addr_t phy_addr = 0;

	/* only recycle pkt_list[1] = skb->data for SW TXSC */
	pkt_list++;

	phy_addr = (pkt_list->phy_addr_l);
#ifdef CONFIG_ARCH_DMA_ADDR_T_64BIT
	phy_addr |= ((u64)pkt_list->phy_addr_h << 32);
#endif
	pci_unmap_bus_addr(pdev, &phy_addr, pkt_list->length, DMA_TO_DEVICE);
}

void txsc_fill_txreq_phyaddr(_adapter *padapter, struct rtw_pkt_buf_list *pkt_list)
{
	PPCI_DATA pci_data = dvobj_to_pci(padapter->dvobj);
	struct pci_dev *pdev = pci_data->ppcidev;
	dma_addr_t phy_addr = 0;

	pci_get_bus_addr(pdev, pkt_list->vir_addr, &phy_addr, pkt_list->length, DMA_TO_DEVICE);
#ifdef CONFIG_ARCH_DMA_ADDR_T_64BIT
	pkt_list->phy_addr_h =  phy_addr >> 32;
#else
	pkt_list->phy_addr_h = 0x0;
#endif
	pkt_list->phy_addr_l = phy_addr & 0xFFFFFFFF;
}
#endif

static void txsc_init_pkt_entry(_adapter *padapter, struct sk_buff *pskb, struct txsc_pkt_entry *txsc_pkt)
{
	u8 priority = 0, i;

	txsc_pkt->step = TXSC_NONE;
	txsc_pkt->txsc_id = 0xff;
	txsc_pkt->ptxreq = NULL;

	for (i = 0; i < MAX_TXSC_SKB_NUM; i++)
		txsc_pkt->xmit_skb[i] = NULL;

	txsc_pkt->psta = NULL;
	txsc_pkt->xmit_skb[0] = pskb;
	txsc_pkt->skb_cnt = 1;

	priority = *(pskb->data + ETH_HLEN + 1);
	txsc_pkt->priority = tos_to_up(priority);
}

static void txsc_add_sc_check(_adapter *padapter, struct xmit_frame *pxframe, struct txsc_pkt_entry *txsc_pkt)
{
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;
	struct pkt_attrib *pattrib = &pxframe->attrib;
	struct sta_info *psta = pxframe->attrib.psta;

	if (!pxmitpriv->txsc_enable) {
		if (psta->txsc_cache_num > 0)
			txsc_pkt->step = TXSC_SKIP;
		else
			txsc_pkt->step = TXSC_NONE;
		goto exit;
	}

	if (pxframe->attrib.nr_frags > 1 || pxframe->attrib.bswenc == 1)
		goto exit;

	if (txsc_pkt->step != TXSC_NONE)
		goto exit;

	if (pattrib->qos_en &&
		pattrib->ampdu_en == 1 &&
		pattrib->ether_type == ETH_P_IP &&
		!IS_MCAST(pattrib->ra) &&
		!pattrib->icmp_pkt &&
		!pattrib->dhcp_pkt) {

		RTW_PRINT("[%s] sta[%02x] add eth_type=0x%x pkt to txsc\n",
			__func__, pattrib->psta->phl_sta->mac_addr[5], pattrib->ether_type);

		txsc_pkt->step = TXSC_ADD;
	}

exit:
	return;
}

static u8 txsc_get_sc_entry(_adapter *padapter, struct sk_buff *pskb, struct txsc_pkt_entry *txsc_pkt)
{
	struct sta_priv	*pstapriv = &padapter->stapriv;
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;
	struct sta_info *psta = NULL;
	u8 *ptxsc_ethdr = NULL;
	u8 i, res, da[6], offset, res2 = 0, sta_hit = 0;

	res = _FAIL;
	offset = 6;

	if (!pxmitpriv->txsc_enable)
		return res;

	if (pxmitpriv->ptxsc_sta_cached) {
		if (pxmitpriv->ptxsc_sta_cached->phl_sta)
			sta_hit = _rtw_memcmp(pxmitpriv->ptxsc_sta_cached->phl_sta->mac_addr, pskb->data, 6);

		if (sta_hit)
			psta = pxmitpriv->ptxsc_sta_cached;
	}

	if (!sta_hit) {
		_rtw_memcpy(da, pskb->data, 6);
		if (IS_MCAST(da)) {
			res = _FAIL;
		} else {
			if (check_fwstate(pmlmepriv, WIFI_AP_STATE))
				psta = rtw_get_stainfo(pstapriv, da);
			else if (check_fwstate(pmlmepriv, WIFI_STATION_STATE))
				psta = rtw_get_stainfo(pstapriv, get_bssid(pmlmepriv));

			if (!psta)
				res = _FAIL;
		}
	}

	if (!psta) {
		res = _FAIL;
		goto exit;
	}

	/* skip power saving mode */
	if (psta->state & WIFI_SLEEP_STATE) {
		res = _FAIL;
		txsc_pkt->step = TXSC_SKIP;
		psta->txsc_path_ps++;
		goto exit;
	}


	if (psta->txsc_cache_num == 0) {
		res = _FAIL;
		goto exit;
	}

	pxmitpriv->ptxsc_sta_cached = psta;
	txsc_pkt->step = TXSC_NONE;
	ptxsc_ethdr = (u8 *)&psta->txsc_entry_cache[psta->txsc_cache_idx].txsc_ethdr;
	res2 = _rtw_memcmp((pskb->data + offset), (ptxsc_ethdr + offset), (ETH_HLEN - offset));

	if (res2 &&
		(pskb->len <= psta->txsc_entry_cache[psta->txsc_cache_idx].txsc_frag_len)) {

		txsc_pkt->step = TXSC_APPLY;
		txsc_pkt->psta = psta;
		txsc_pkt->txsc_id = psta->txsc_cache_idx;

		res = _SUCCESS;
	} else {

		for (i = 0; i < CORE_TXSC_ENTRY_NUM; i++) {
			if (i != psta->txsc_cache_idx && psta->txsc_entry_cache[i].txsc_is_used) {

				ptxsc_ethdr = (u8 *)&(psta->txsc_entry_cache[i].txsc_ethdr);
				if (_rtw_memcmp((pskb->data + offset), (ptxsc_ethdr + offset), (ETH_HLEN - offset)) &&
					(pskb->len <= psta->txsc_entry_cache[i].txsc_frag_len)) {

					txsc_pkt->step = TXSC_APPLY;
					txsc_pkt->txsc_id = i;
					txsc_pkt->psta = psta;
					psta->txsc_cache_idx = i;
					res = _SUCCESS;

					break;
				}
			}
		}
	}

	if ((res == _SUCCESS) && (pxmitpriv->txsc_debug_mode == 1)) {
		txsc_pkt->step = TXSC_DEBUG;
		res = _FAIL;
	}

exit:
	return res;
}

static void txsc_prepare_sc_entry(_adapter *padapter, struct xmit_frame *pxframe, struct txsc_pkt_entry *txsc_pkt)
{
	struct sta_priv	*pstapriv = &padapter->stapriv;
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;
	struct sta_info *psta = pxframe->attrib.psta;
	struct pkt_attrib *pattrib = &pxframe->attrib;
	struct rtw_xmit_req *txreq = pxframe->phl_txreq;
	struct rtw_pkt_buf_list *pkt_list = (struct rtw_pkt_buf_list *)txreq->pkt_list;
	struct rtw_pkt_buf_list *ptxsc_pkt_list0 = NULL;
	struct rtw_t_meta_data *ptxsc_mdata = NULL;
	struct sk_buff *pskb = txsc_pkt->xmit_skb[0];
	u8 i, idx;
	u8 *ptxsc_ethdr = NULL;
	u8 *ptxsc_wlhdr = NULL;
	u8 *ptxsc_wlhdr_len = NULL;

	if (!psta) {
		RTW_ERR("%s: fetal err, XF_STA = NULL, please check.\n", __func__);
		return;
	}

	if (txsc_pkt->step == TXSC_SKIP) {
		if (pxmitpriv->txsc_enable && psta->txsc_cache_num > 0)
			psta->txsc_cache_miss++;
		else
			psta->txsc_path_slow++;
	} else if (txsc_pkt->step == TXSC_NONE) {
		psta->txsc_path_slow++;
	}

	if (txsc_pkt->step != TXSC_ADD)
		return;

	idx = psta->txsc_cur_idx;

	ptxsc_ethdr = (u8 *)&psta->txsc_entry_cache[idx].txsc_ethdr;
	#ifdef USE_ONE_WLHDR
	ptxsc_wlhdr = psta->txsc_entry_cache[idx].txsc_wlhdr;
	#else
	ptxsc_wlhdr = (u8 *)&psta->txsc_entry_cache[idx].txsc_wlhdr;
	#endif
	ptxsc_pkt_list0 = &psta->txsc_entry_cache[idx].txsc_pkt_list0;
	ptxsc_mdata = &psta->txsc_entry_cache[idx].txsc_mdata;
	ptxsc_wlhdr_len = &psta->txsc_entry_cache[idx].txsc_wlhdr_len;

	_rtw_spinlock_bh(&pxmitpriv->txsc_lock);

	if (psta->txsc_entry_cache[idx].txsc_is_used == 1)
		RTW_PRINT("[CORE_TXSC] txsc entry is full, replace rentry[%d]\n", idx);

	/* ALLOC WLHDR in DMA addr */
	#ifdef USE_ONE_WLHDR
	if (!ptxsc_wlhdr) {
		ptxsc_wlhdr = rtw_zmalloc(CORE_TXSC_WLHDR_SIZE);
		psta->txsc_entry_cache[idx].txsc_wlhdr = ptxsc_wlhdr;
	}
	#endif

	/* ETH HDR */
	_rtw_memcpy(ptxsc_ethdr, pskb->data, ETH_HLEN);

	/* WLAN HDR + LLC */
	_rtw_memcpy(ptxsc_wlhdr, pkt_list->vir_addr, pkt_list->length);
	*ptxsc_wlhdr_len = pkt_list->length;

	/* pkt_list[0] */
	ptxsc_pkt_list0->vir_addr = ptxsc_wlhdr;
	ptxsc_pkt_list0->length = pkt_list->length;
	#ifdef CONFIG_PCI_HCI
	txsc_fill_txreq_phyaddr(padapter, ptxsc_pkt_list0);
	#endif

	/* META DATA */
	_rtw_memcpy(ptxsc_mdata, &txreq->mdata, sizeof(*ptxsc_mdata));

	/* FRAGE_LEN */
	psta->txsc_entry_cache[idx].txsc_frag_len = pxframe->attrib.frag_len_txsc;

	psta->txsc_entry_cache[idx].txsc_is_used = 1;
	psta->txsc_cache_idx = idx;
	psta->txsc_cur_idx = (psta->txsc_cur_idx + 1) % CORE_TXSC_ENTRY_NUM;
	if (psta->txsc_cache_num < CORE_TXSC_ENTRY_NUM)
		psta->txsc_cache_num++;

	psta->txsc_path_slow++;

	pxmitpriv->ptxsc_sta_cached = psta;

	txreq->treq_type = RTW_PHL_TREQ_TYPE_PHL_ADD_TXSC | RTW_PHL_TREQ_TYPE_PHL_UPDATE_TXSC;

	/* set shortcut id  */
	txreq->shortcut_id = idx;
	psta->txsc_entry_cache[idx].txsc_phl_id = idx;

	/* disable phl txsc  */
	if (!(pxmitpriv->txsc_debug_mask&BIT1))
		txreq->shortcut_id = 0;

	RTW_PRINT("[CORE_TXSC][ADD] core_txsc_idx:%d(cur_idx:%d), txreq_sc_id:%d, txsc_frag_len:%d\n",
		idx, psta->txsc_cur_idx, txreq->shortcut_id, psta->txsc_entry_cache[idx].txsc_frag_len);

	/* for debug */
	_print_txreq_mdata(ptxsc_mdata, __func__);

	_rtw_spinunlock_bh(&pxmitpriv->txsc_lock);
}

u8 txsc_get_sc_cached_entry(_adapter *padapter, struct sk_buff *pskb, struct txsc_pkt_entry *txsc_pkt)
{
	txsc_init_pkt_entry(padapter, pskb, txsc_pkt);
	return txsc_get_sc_entry(padapter, pskb, txsc_pkt);
}

void txsc_add_sc_cache_entry(_adapter *padapter, struct xmit_frame *pxframe, struct txsc_pkt_entry *txsc_pkt)
{
	txsc_add_sc_check(padapter, pxframe, txsc_pkt);
	txsc_prepare_sc_entry(padapter, pxframe, txsc_pkt);
}

u8 txsc_apply_sc_cached_entry(_adapter *padapter, struct txsc_pkt_entry *txsc_pkt)
{
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;
	struct rtw_pkt_buf_list	*pkt_list = NULL;
	struct rtw_pkt_buf_list *pkt_list0 = NULL;
	struct rtw_t_meta_data	*mdata = NULL;
	struct rtw_xmit_req	*txreq = NULL;
	struct xmit_txreq_buf *txreq_buf = NULL;
	struct txsc_entry *txsc;
	struct sta_info *psta = txsc_pkt->psta;
	struct sk_buff *xmit_skb[MAX_TXSC_SKB_NUM];
	u8 *hdr = NULL;
	u16 *qc;
	int wlhdr_copy_len, payload_offset = ETH_HLEN;
	u8 idx, priority = 0, res = _SUCCESS;
	u8 i, skb_cnt = 0, is_hdr_need_update = 0, txsc_wlhdr_len;
	#ifndef USE_ONE_WLHDR
	u8 *head, *ptxsc_wlhdr;
	#endif

	if (!pxmitpriv->txsc_enable || !psta)
		return _SUCCESS;

	if (txsc_pkt->step != TXSC_APPLY &&
		txsc_pkt->step != TXSC_AMSDU_APPLY &&
		txsc_pkt->step != TXSC_DEBUG)
		return _SUCCESS;

	_rtw_memset(xmit_skb, 0x0, sizeof(xmit_skb));

	/* get cached entry */
	idx = txsc_pkt->txsc_id;
	txsc = &psta->txsc_entry_cache[idx];

	pkt_list0 = &txsc->txsc_pkt_list0;
	mdata = &txsc->txsc_mdata;
	#ifdef USE_ONE_WLHDR
	txsc_wlhdr_len = pkt_list0->length;
	#else
	ptxsc_wlhdr = (u8 *)txsc->txsc_wlhdr;
	txsc_wlhdr_len = txsc->txsc_wlhdr_len;
	#endif

	if (txsc_pkt->step == TXSC_DEBUG) {
		_rtw_memset(psta->debug_buf, 0x0, (sizeof(struct rtw_xmit_req)+sizeof(struct rtw_pkt_buf_list)*2));
		txreq = (struct rtw_xmit_req *)psta->debug_buf;
		pkt_list = (struct rtw_pkt_buf_list *)psta->debug_buf + sizeof(struct rtw_xmit_req);
	} else {
		#ifdef USE_ONE_WLHDR
		txreq_buf = (struct xmit_txreq_buf *)get_txreq_buffer(padapter, (u8 **)&txreq, (u8 **)&pkt_list, NULL, NULL);
		#else
		txreq_buf = (struct xmit_txreq_buf *)get_txreq_buffer(padapter, (u8 **)&txreq, (u8 **)&pkt_list, (u8 **)&head, NULL);
		#endif

		if (txreq_buf == NULL) {
			res = _FAIL;
			goto exit;
		}
	}

	/* init txreq buf */
	txreq_buf->adapter = padapter;

	/* fill txreq other */
	txreq->os_priv = (void *)txreq_buf;
	txreq->pkt_list = (u8 *)pkt_list;
	txreq->pkt_cnt = 0;
	txreq->total_len = 0;

	/* fill txsc pkt entry */
	txsc_pkt->ptxreq = txreq;

	{
		/* for no tx_amsdu case */
		xmit_skb[0] = txsc_pkt->xmit_skb[0];
		skb_cnt = txsc_pkt->skb_cnt;
	}

	if (skb_cnt == 0)
		RTW_PRINT("[ERR][%s:%d] skb_cnt = 0 is a fatel error, plz check\n", __func__, __LINE__);

	/* fill_txreq_mdata */
	_rtw_memcpy(&txreq->mdata, mdata, sizeof(txreq->mdata));

	/* Update TID from IP header */
	/* priority = *(xmit_skb[0]->data + ETH_HLEN + 1); */
	/*txreq->mdata.tid = tos_to_up(priority); */
	priority = txreq->mdata.tid = txsc_pkt->priority;
	txreq->mdata.cat = rtw_phl_cvt_tid_to_cat(priority);

	/* SW shortcut --- */
	/* rtw_core_wlan_fill_head */
	/* use swseq in amsdu */
	if (txreq->mdata.hw_seq_mode == 0) {
		/* generate sw seq */
		//priority = txreq->mdata.tid;
		psta->sta_xmitpriv.txseq_tid[priority]++;
		psta->sta_xmitpriv.txseq_tid[priority] &= 0xFFF;
		txreq->mdata.sw_seq = psta->sta_xmitpriv.txseq_tid[priority];

		hdr = txsc->txsc_wlhdr;
		SetSeqNum(hdr, txreq->mdata.sw_seq);

		is_hdr_need_update = 1;
	}

	wlhdr_copy_len = txsc_wlhdr_len;

	/* WLAN header from cache */
	#ifdef USE_ONE_WLHDR
	_rtw_memcpy(pkt_list, pkt_list0, sizeof(struct rtw_pkt_buf_list));
	#else
	#ifdef USE_PREV_WLHDR_BUF
	if (txreq_buf->macid == txreq->mdata.macid && txreq_buf->txsc_id == idx) {
		if (is_hdr_need_update)
			SetSeqNum(head, txreq->mdata.sw_seq);/* set sw seq */
	} else
	#endif /* USE_PREV_WLHDR_BUF */
	{
		_rtw_memcpy(head, ptxsc_wlhdr, wlhdr_copy_len);
	}

	/* fill wlhdr in pkt_list[0] */
	pkt_list->vir_addr = head;
	pkt_list->length = wlhdr_copy_len;
	#ifdef CONFIG_PCI_HCI
	txsc_fill_txreq_phyaddr(padapter, pkt_list);
	#endif
	#endif/* USE_ONE_WLHDR */

	txreq->total_len += pkt_list->length;
	txreq->pkt_cnt++;

	#ifdef USE_PREV_WLHDR_BUF
	txreq_buf->macid = txreq->mdata.macid;
	txreq_buf->txsc_id = idx;
	#endif/* USE_PREV_WLHDR_BUF */

	/* Payload w.o. ether header */ /* CONFIG_TXSC_AMSDU for multiple skb */
	for (i = 0; i < skb_cnt; i++) {
		pkt_list++;

		pkt_list->vir_addr = xmit_skb[i]->data + payload_offset;
		pkt_list->length = xmit_skb[i]->len - payload_offset;

		txreq->total_len += pkt_list->length;
		txreq->pkt_cnt++;
		#ifdef CONFIG_PCI_HCI
		txsc_fill_txreq_phyaddr(padapter, pkt_list);
		#endif

		txreq_buf->pkt[i] = (u8 *)xmit_skb[i];
	}

	txreq->treq_type = RTW_PHL_TREQ_TYPE_CORE_TXSC;

	if (txsc_pkt->step == TXSC_APPLY) {
		psta->txsc_cache_hit++;
		txsc->txsc_cache_hit++;
	} else
		psta->txsc_path_slow++;
	/* SW shortcut --- */

	txreq_buf->pkt_cnt = skb_cnt;/* for recycle multiple skb */
	txreq->mdata.pktlen = txreq->total_len;
	txreq->shortcut_id = psta->txsc_entry_cache[idx].txsc_phl_id;

	if (!(pxmitpriv->txsc_debug_mask&BIT1))
		txreq->shortcut_id = 0;

	/* send addbareq */
	txsc_issue_addbareq_cmd(padapter, priority, psta, _TRUE);

#ifdef RTW_PHL_DBG_CMD
	/* Update force rate settings so force rate takes effects
	 * after shortcut cached
	 */
	if (padapter->txForce_enable) {
		if (padapter->txForce_rate != INV_TXFORCE_VAL) {
			txreq->mdata.f_rate = padapter->txForce_rate;
			txreq->mdata.userate_sel = 1;
			txreq->mdata.dis_data_rate_fb = 1;
			txreq->mdata.dis_rts_rate_fb = 1;
		}
		if (padapter->txForce_agg != INV_TXFORCE_VAL)
			txreq->mdata.ampdu_en = padapter->txForce_agg;
		if (padapter->txForce_aggnum != INV_TXFORCE_VAL)
			txreq->mdata.max_agg_num = padapter->txForce_aggnum;
		if (padapter->txForce_gi != INV_TXFORCE_VAL)
			txreq->mdata.f_gi_ltf = padapter->txForce_gi;
		if (padapter->txForce_ampdu_density != INV_TXFORCE_VAL)
			txreq->mdata.ampdu_density = padapter->txForce_ampdu_density;
		if (padapter->txForce_bw != INV_TXFORCE_VAL)
			txreq->mdata.f_bw = padapter->txForce_bw;
	}
#endif /* RTW_PHL_DBG_CMD */

	/* for tx debug */
	_print_txreq_mdata(&txreq->mdata, __func__);
	_print_txreq_pklist(NULL, txsc_pkt->ptxreq, xmit_skb[0], __func__);

exit:
	return res;
}

void txsc_free_txreq(_adapter *padapter, struct rtw_xmit_req *txreq)
{
	struct xmit_txreq_buf *ptxreq_buf = NULL;
	struct sk_buff *tx_skb = NULL;
	_queue *queue = NULL;
	u8 i;

	if (txreq != NULL)
		ptxreq_buf = txreq->os_priv;
	else
		return;

	#ifdef CONFIG_PCI_HCI
	#ifdef USE_ONE_WLHDR
	txsc_recycle_txreq_phyaddr(padapter, txreq);
	#else
	core_recycle_txreq_phyaddr(padapter, txreq);
	#endif
	#endif

	if (!ptxreq_buf) {
		RTW_ERR("%s: NULL ptxreq_buf !!\n", __func__);
		rtw_warn_on(1);
		return;
	}

	queue = &padapter->free_txreq_queue;
	_rtw_spinlock_bh(&queue->lock);

	rtw_list_delete(&ptxreq_buf->list);
	rtw_list_insert_tail(&ptxreq_buf->list, get_list_head(queue));

	txreq->os_priv = NULL;
	txreq->pkt_list = NULL;
	txreq->treq_type = RTW_PHL_TREQ_TYPE_NORMAL;

	/* this must be protected in  spinlock section */
	if (ptxreq_buf->pkt_cnt == 0)
		RTW_PRINT("[ERR][%s:%d] pkt_cnt = 0 is a fatel error, plz check\n", __func__, __LINE__);

	for (i = 0; i < ptxreq_buf->pkt_cnt; i++) {
		tx_skb = (struct sk_buff *)ptxreq_buf->pkt[i];
		if (tx_skb)
			rtw_os_pkt_complete(padapter, tx_skb);
		else
			RTW_DBG("%s:tx recyele: tx_skb=NULL\n", __func__);
		ptxreq_buf->pkt[i] = NULL;
	}
	padapter->free_txreq_cnt++;

	_rtw_spinunlock_bh(&queue->lock);
}

void txsc_debug_sc_entry(_adapter *padapter, struct xmit_frame *pxframe, struct txsc_pkt_entry *txsc_pkt)
{
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;
	struct rtw_pkt_buf_list *pkt_list = NULL;
	struct rtw_pkt_buf_list *txsc_pkt_list = NULL;
	struct rtw_xmit_req *txreq = pxframe->phl_txreq;
	struct rtw_xmit_req *ptxsc_txreq = txsc_pkt->ptxreq;
	u8 i;

	if (txsc_pkt->step != TXSC_DEBUG)
		return;

	if (pxmitpriv->txsc_debug_mask&BIT2) {

		RTW_PRINT("\n\nNormal Path TXREQ: %p\n\n", txreq);
		txsc_dump_data((u8 *)txreq, sizeof(struct rtw_xmit_req), "txreq:");
		RTW_PRINT("os_priv=%p\n", txreq->os_priv);
		RTW_PRINT("treq_type=%d shortcut_id=%d\n", txreq->treq_type, txreq->shortcut_id);
		RTW_PRINT("total_len=%d pkt_cnt=%d\n", txreq->total_len, txreq->pkt_cnt);
		pkt_list = (struct rtw_pkt_buf_list *)txreq->pkt_list;
		for (i = 0; i < txreq->pkt_cnt; i++) {
			RTW_PRINT("[%d]", i);
			txsc_dump_data((u8 *)pkt_list, sizeof(struct rtw_pkt_buf_list), "pklist");
			txsc_dump_data((u8 *)pkt_list->vir_addr, pkt_list->length, "pkt_list->vir_addr");
			pkt_list++;
		}
		RTW_PRINT("mdata: pktlen=%d sw_seq=%d\n", txreq->mdata.pktlen, txreq->mdata.sw_seq);

		RTW_PRINT("\n\nShortcut Path TXREQ: %p\n\n", ptxsc_txreq);
		txsc_dump_data((u8 *)ptxsc_txreq, sizeof(struct rtw_xmit_req), "ptxsc_txreq:");
		RTW_PRINT("os_priv=%p\n", ptxsc_txreq->os_priv);
		RTW_PRINT("treq_type=%d shortcut_id=%d\n", ptxsc_txreq->treq_type, ptxsc_txreq->shortcut_id);
		RTW_PRINT("total_len=%d pkt_cnt=%d\n", ptxsc_txreq->total_len, ptxsc_txreq->pkt_cnt);
		txsc_pkt_list = (struct rtw_pkt_buf_list *)ptxsc_txreq->pkt_list;
		for (i = 0; i < txreq->pkt_cnt; i++) {
			RTW_PRINT("[%d]", i);
			txsc_dump_data((u8 *)txsc_pkt_list, sizeof(struct rtw_pkt_buf_list), "pklist");
			txsc_dump_data((u8 *)txsc_pkt_list->vir_addr, txsc_pkt_list->length, "pkt_list->vir_addr");
			txsc_pkt_list++;
		}
		RTW_PRINT("mdata: pktlen=%d sw_seq=%d\n", ptxsc_txreq->mdata.pktlen, ptxsc_txreq->mdata.sw_seq);

	} else {
		if (!_rtw_memcmp(&txreq->mdata, &ptxsc_txreq->mdata, sizeof(struct rtw_t_meta_data))) {
			txsc_dump_data((u8 *)&txreq->mdata, sizeof(struct rtw_t_meta_data), "txreq->mdata");
			txsc_dump_data((u8 *)&ptxsc_txreq->mdata, sizeof(struct rtw_t_meta_data), "ptxsc_txreq->mdata");
		}

		pkt_list = (struct rtw_pkt_buf_list *)txreq->pkt_list;
		txsc_pkt_list = (struct rtw_pkt_buf_list *)ptxsc_txreq->pkt_list;
		if (pkt_list->length != txsc_pkt_list->length) {
			txsc_dump_data((u8 *)pkt_list, sizeof(struct rtw_pkt_buf_list), "pkt_list[0]");
			txsc_dump_data((u8 *)txsc_pkt_list, sizeof(struct rtw_pkt_buf_list), "txsc_pkt_list[0]");
			txsc_dump_data((u8 *)pkt_list->vir_addr, pkt_list->length, "pkt_list[0]->vir_addr");
			txsc_dump_data(txsc_pkt_list->vir_addr, pkt_list->length, "txsc_pkt_list[0]->vir_addr");
		} else if (pkt_list->length == txsc_pkt_list->length) {
			if (!_rtw_memcmp(pkt_list->vir_addr, txsc_pkt_list->vir_addr, pkt_list->length)) {
				txsc_dump_data((u8 *)pkt_list->vir_addr, pkt_list->length, "pkt_list[0]->vir_addr");
				txsc_dump_data((u8 *)txsc_pkt_list->vir_addr, pkt_list->length, "txsc_pkt_list[0]->vir_addr");
			}
		}
	}

	/* DO NOT WD CACHE */
	txreq->shortcut_id = 0;
	txreq->treq_type = RTW_PHL_TREQ_TYPE_NORMAL;
}

#ifdef CONFIG_80211N_HT
static u8 txsc_issue_addbareq_check(_adapter *padapter, u8 issue_when_busy)
{
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	struct registry_priv *pregistry = &padapter->registrypriv;

	if (pregistry->tx_quick_addba_req == 0) {
		if ((issue_when_busy == _TRUE) && (pmlmepriv->LinkDetectInfo.bBusyTraffic == _FALSE))
			return _FALSE;

		if (pmlmepriv->LinkDetectInfo.NumTxOkInPeriod < 100)
			return _FALSE;
	}

	return _TRUE;
}

void txsc_issue_addbareq_cmd(_adapter *padapter, u8 priority, struct sta_info *psta, u8 issue_when_busy)
{
	u8 issued;
	struct ht_priv	*phtpriv;
	struct ampdu_priv *ampdu_priv;

	if (txsc_issue_addbareq_check(padapter, issue_when_busy) == _FALSE)
		return;

	phtpriv = &psta->htpriv;
	ampdu_priv = &psta->ampdu_priv;

	if ((phtpriv->ht_option == _TRUE) && (ampdu_priv->ampdu_enable == _TRUE)) {
		issued = (ampdu_priv->agg_enable_bitmap >> priority) & 0x1;
		issued |= (ampdu_priv->candidate_tid_bitmap >> priority) & 0x1;

		if (issued == 0) {
			RTW_INFO("rtw_issue_addbareq_cmd, p=%d\n", priority);
			psta->ampdu_priv.candidate_tid_bitmap |= BIT((u8)priority);
			rtw_addbareq_cmd(padapter, (u8) priority, psta->phl_sta->mac_addr);
		}
	}

}
#endif /* CONFIG_80211N_HT */
#endif /* CONFIG_CORE_TXSC */


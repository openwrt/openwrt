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
#include <drv_types.h>

static const u8 bcast_addr[ETH_ALEN] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
#ifdef RTW_PHL_TEST_FPGA
static const u8 ap_addr[ETH_ALEN] = {0x00, 0xe0, 0x4c, 0x88, 0x52, 0xa0};
u8 ap_ssid[] = {'p','h','l','-','5','g'};
#else
static const u8 ap_addr[ETH_ALEN] = {0x00, 0xe0, 0x4c, 0x12, 0x34, 0x56};
u8 ap_ssid[] = {'f','a','k','e','a','p'};
#endif
u8 ap_ch = 1;
u16 ap_beacon_interval = 100; /* unit: ms */
u16 ap_capability_info = 0x0001;
u8 ap_datarate[] = {0x82, 0x84, 0x8b, 0x96, 0xc, 0x12, 0x18, 0x24};

int rtw_fakeap_tx(struct _ADAPTER *a, struct xmit_frame *frame)
{
	u8 *data = frame->buf_addr + TXDESC_OFFSET;
	u32 data_len = frame->attrib.last_txcmdsz;
 	struct sk_buff *skb;
	u8 *buf;


	if (GetFrameType(data) != WIFI_MGT_TYPE)
		return _FAIL;

	switch (get_frame_sub_type(data)) {
	case WIFI_AUTH:
	case WIFI_ASSOCREQ:
	case WIFI_REASSOCREQ:
	case WIFI_PROBEREQ:
		break;

	default:
		return _FAIL;
	}

	skb = rtw_skb_alloc(data_len);
	buf = skb_put(skb, data_len);
	_rtw_memcpy(buf, data, data_len);
	rtw_free_xmitbuf(&a->xmitpriv, frame->pxmitbuf);
	rtw_free_xmitframe(&a->xmitpriv, frame);

	skb_queue_tail(&adapter_to_dvobj(a)->fakeap.rxq, skb);
	_set_workitem(&adapter_to_dvobj(a)->fakeap.work);

	return _SUCCESS;
}

static void add_ie(struct sk_buff *skb, u8 index, u8 len, const u8 *source)
{
	u8 *buf;


	buf = skb_put(skb, 2 + len);
	*buf = index;
	*(buf + 1) = len;
	if (len > 0)
		_rtw_memcpy(buf + 2, source, len);
}

void rtw_fakeap_bcn_timer_hdl(void *p)
{
	struct dvobj_priv *d;
	struct sk_buff *skb;
	struct rtw_ieee80211_hdr *wlanhdr;
	int len;
	u8 *buf;
	u8 hdrlen;


	d = (struct dvobj_priv*)p;
	hdrlen = sizeof(struct rtw_ieee80211_hdr_3addr);
	len = hdrlen + _FIXED_IE_LENGTH_
	      + 2 + sizeof(ap_ssid)
	      + 2 + sizeof(ap_datarate)
	      + 2 + 1;
	skb = rtw_skb_alloc(len);
	_rtw_memset(skb->data, 0, len);

	buf = skb_put(skb, hdrlen);
	wlanhdr = (struct rtw_ieee80211_hdr*)buf;
	set_frame_sub_type(&wlanhdr->frame_ctl, WIFI_BEACON);
	_rtw_memcpy(wlanhdr->addr1, bcast_addr, ETH_ALEN);
	_rtw_memcpy(wlanhdr->addr2, ap_addr, ETH_ALEN);
	_rtw_memcpy(wlanhdr->addr3, ap_addr, ETH_ALEN);

	buf = skb_put(skb, _FIXED_IE_LENGTH_);
	/* timestamp: 8 bytes */
	/* beacon interval: 2 bytes */
	*(u16*)(buf + 8) = ap_beacon_interval;
	/* capability info: 2 bytes */
	*(u16*)(buf + 10) = ap_capability_info;

	/* SSID */
	add_ie(skb, _SSID_IE_, sizeof(ap_ssid), ap_ssid);

	/* supported rates... */
	add_ie(skb, _SUPPORTEDRATES_IE_, sizeof(ap_datarate), ap_datarate);

	/* DS parameter set */
	add_ie(skb, _DSSET_IE_, 1, &ap_ch);

	skb_queue_tail(&d->fakeap.rxq, skb);
	_set_workitem(&d->fakeap.work);
}

static void rx_prepare(struct _ADAPTER *adapter, union recv_frame *r)
{
	struct rx_pkt_attrib *a;


	/* init recv_frame */
	_rtw_init_listhead(&r->u.hdr.list);
	r->u.hdr.len = 0;
	a = &r->u.hdr.attrib;
	_rtw_memset(a, 0, sizeof(*a));

	/* fill sec related attrib, iv_len and icv_len will be filled by
	 * validate_recv_data_frame() */
	a->crc_err = 0;
	a->icv_err = 0;
	a->encrypt = 0;

	/* fill rx pkt attrib */
	a->hdrlen = 0;
	a->bw = CHANNEL_WIDTH_MAX;
	a->pkt_len = 0;
	a->pkt_rpt_type = NORMAL_RX;
	a->drvinfo_sz = 0;
	a->bdecrypted = 0;
	a->qos = 0;
	a->priority = 0;
	a->amsdu = 0;
	a->mdata = 0;
	a->mfrag = 0;
	a->seq_num = 0;
	a->frag_num = 0;
	a->data_rate = DESC_RATE6M;
	a->ppdu_cnt = 1;
	a->free_cnt = 0;
}

static union recv_frame* gen_probersp(struct _ADAPTER *a)
{
	union recv_frame *r;
	u32 len;
	struct sk_buff *skb;
	struct rtw_ieee80211_hdr *wlanhdr;
	u8 *buf;
	u8 hdrlen;
	u8 ielen;


	r = rtw_alloc_recvframe(&a->recvpriv.free_recv_queue);
	if (!r) {
		RTW_ERR("%s: alloc recvframe failed!\n", __func__);
		return NULL;
	}
	rtw_init_recvframe(r, &a->recvpriv);
	rx_prepare(a, r);

	hdrlen = sizeof(struct rtw_ieee80211_hdr_3addr);
	len = RXDESC_OFFSET + hdrlen + _CAPABILITY_ + _STATUS_CODE_ + _ASOC_ID_
	      + 2 + sizeof(ap_ssid)
	      + 2 + sizeof(ap_datarate)
	      + 2 + 1;
	skb = rtw_skb_alloc(len);
	_rtw_memset(skb->data, 0, len);
	skb->dev = a->pnetdev;
	skb_reserve(skb, RXDESC_OFFSET);
	r->u.hdr.pkt = skb;

	buf = skb_put(skb, hdrlen);
	wlanhdr = (struct rtw_ieee80211_hdr*)buf;
	set_frame_sub_type(&wlanhdr->frame_ctl, WIFI_PROBERSP);
	_rtw_memcpy(wlanhdr->addr1, adapter_mac_addr(a), ETH_ALEN);
	_rtw_memcpy(wlanhdr->addr2, ap_addr, ETH_ALEN);
	_rtw_memcpy(wlanhdr->addr3, ap_addr, ETH_ALEN);

	buf = skb_put(skb, _FIXED_IE_LENGTH_);
	/* timestamp: 8 bytes */
	/* beacon interval: 2 bytes */
	*(u16*)(buf + 8) = ap_beacon_interval;
	/* capability info: 2 bytes */
	*(u16*)(buf + 10) = ap_capability_info;

	/* SSID */
	add_ie(skb, _SSID_IE_, sizeof(ap_ssid), ap_ssid);

	/* supported rates... */
	add_ie(skb, _SUPPORTEDRATES_IE_, sizeof(ap_datarate), ap_datarate);

	/* DS parameter set */
	add_ie(skb, _DSSET_IE_, 1, &ap_ch);

	/* handle r->u.hdr.attrib */

	/* handle recv_frame pointer */
	r->u.hdr.len = skb->len;
	r->u.hdr.rx_head = skb->head;
	r->u.hdr.rx_data = skb->data;
	r->u.hdr.rx_tail = skb_tail_pointer(skb);
	r->u.hdr.rx_end = skb_end_pointer(skb);

	return r;
}

static union recv_frame* gen_beacon(struct _ADAPTER *a, struct sk_buff *beacon)
{
	union recv_frame *r;
	u32 len;
	struct sk_buff *skb;
	struct rtw_ieee80211_hdr *wlanhdr;
	u8 *buf;
	u8 hdrlen;


	r = rtw_alloc_recvframe(&a->recvpriv.free_recv_queue);
	if (!r) {
		RTW_ERR("%s: alloc recvframe failed!\n", __func__);
		return NULL;
	}
	rtw_init_recvframe(r, &a->recvpriv);
	rx_prepare(a, r);

	hdrlen = sizeof(struct rtw_ieee80211_hdr_3addr);
	len = RXDESC_OFFSET + beacon->len;
	skb = rtw_skb_alloc(len);
	_rtw_memset(skb->data, 0, len);
	skb->dev = a->pnetdev;
	skb_reserve(skb, RXDESC_OFFSET);
	r->u.hdr.pkt = skb;

	buf = skb_put(skb, beacon->len);
	_rtw_memcpy(buf, beacon->data, beacon->len);

	/* handle r->u.hdr.attrib */

	/* handle recv_frame pointer */
	r->u.hdr.len = skb->len;
	r->u.hdr.rx_head = skb->head;
	r->u.hdr.rx_data = skb->data;
	r->u.hdr.rx_tail = skb_tail_pointer(skb);
	r->u.hdr.rx_end = skb_end_pointer(skb);

	return r;
}

static union recv_frame *gen_auth(struct _ADAPTER *a, u8 *bssid)
{
	union recv_frame *r;
	u32 len;
	struct sk_buff *skb;
	struct rtw_ieee80211_hdr *wlanhdr;
	u8 *buf;
	u8 hdrlen;


	r = rtw_alloc_recvframe(&a->recvpriv.free_recv_queue);
	if (!r) {
		RTW_ERR("%s: alloc recvframe failed!\n", __func__);
		return NULL;
	}
	rtw_init_recvframe(r, &a->recvpriv);
	rx_prepare(a, r);

	hdrlen = sizeof(struct rtw_ieee80211_hdr_3addr);
	len = RXDESC_OFFSET + hdrlen
	      + _AUTH_ALGM_NUM_ + _AUTH_SEQ_NUM_ + _STATUS_CODE_;
	skb = rtw_skb_alloc(len);
	_rtw_memset(skb->data, 0, len);
	skb->dev = a->pnetdev;
	skb_reserve(skb, RXDESC_OFFSET);
	r->u.hdr.pkt = skb;

	buf = skb_put(skb, hdrlen);
	wlanhdr = (struct rtw_ieee80211_hdr*)buf;
	set_frame_sub_type(&wlanhdr->frame_ctl, WIFI_AUTH);
	_rtw_memcpy(wlanhdr->addr1, adapter_mac_addr(a), ETH_ALEN);
	_rtw_memcpy(wlanhdr->addr2, bssid, ETH_ALEN);
	_rtw_memcpy(wlanhdr->addr3, bssid, ETH_ALEN);

	/* setting auth algo number */
	buf = skb_put(skb, _AUTH_ALGM_NUM_);
	*(u16*)buf = cpu_to_le16(0);;

	/* setting auth seq number */
	buf = skb_put(skb, _AUTH_SEQ_NUM_);
	*(u16*)buf = cpu_to_le16(2);

	/* setting status code... */
	buf = skb_put(skb, _STATUS_CODE_);
	*(u16*)buf = cpu_to_le16(0);

	/* handle r->u.hdr.attrib */

	/* handle recv_frame pointer */
	r->u.hdr.len = skb->len;
	r->u.hdr.rx_head = skb->head;
	r->u.hdr.rx_data = skb->data;
	r->u.hdr.rx_tail = skb_tail_pointer(skb);
	r->u.hdr.rx_end = skb_end_pointer(skb);

	return r;
}

static union recv_frame *gen_assocrsp(struct _ADAPTER *a, u8 *bssid)
{
	union recv_frame *r;
	u32 len;
	struct sk_buff *skb;
	struct rtw_ieee80211_hdr *wlanhdr;
	u8 *buf;
	u8 hdrlen;


	r = rtw_alloc_recvframe(&a->recvpriv.free_recv_queue);
	if (!r) {
		RTW_ERR("%s: alloc recvframe failed!\n", __func__);
		return NULL;
	}
	rtw_init_recvframe(r, &a->recvpriv);
	rx_prepare(a, r);

	hdrlen = sizeof(struct rtw_ieee80211_hdr_3addr);
	len = RXDESC_OFFSET + hdrlen
	      + _CAPABILITY_ + _STATUS_CODE_ + _ASOC_ID_
	      + 2 + sizeof(ap_datarate);
	skb = rtw_skb_alloc(len);
	_rtw_memset(skb->data, 0, len);
	skb->dev = a->pnetdev;
	skb_reserve(skb, RXDESC_OFFSET);
	r->u.hdr.pkt = skb;

	buf = skb_put(skb, hdrlen);
	wlanhdr = (struct rtw_ieee80211_hdr*)buf;
	set_frame_sub_type(&wlanhdr->frame_ctl, WIFI_ASSOCRSP);
	_rtw_memcpy(wlanhdr->addr1, adapter_mac_addr(a), ETH_ALEN);
	_rtw_memcpy(wlanhdr->addr2, bssid, ETH_ALEN);
	_rtw_memcpy(wlanhdr->addr3, bssid, ETH_ALEN);


	/* capability info: 2 bytes */
	buf = skb_put(skb, _CAPABILITY_);
	*(u16*)buf = ap_capability_info;

	/* status code: 2 bytes */
	buf = skb_put(skb, _STATUS_CODE_);
	*(u16*)buf = 0;

	/* AID: 2 bytes */
	buf = skb_put(skb, _ASOC_ID_);
	*(u16*)buf = 100;

	/* supported rates... */
	add_ie(skb, _SUPPORTEDRATES_IE_, sizeof(ap_datarate), ap_datarate);

	/* handle r->u.hdr.attrib */

	/* handle recv_frame pointer */
	r->u.hdr.len = skb->len;
	r->u.hdr.rx_head = skb->head;
	r->u.hdr.rx_data = skb->data;
	r->u.hdr.rx_tail = skb_tail_pointer(skb);
	r->u.hdr.rx_end = skb_end_pointer(skb);

//	recvframe_put(r, buflen);
//	pre_recv_entry(r, NULL);

	return r;
}

static union recv_frame* fakeap_dispatcher(struct _ADAPTER *a,
					   struct sk_buff *skb)
{
	u8 *pframe;
	union recv_frame *rframe = NULL;


	pframe = skb->data;
	RTW_INFO("A1-" MAC_FMT "\n", MAC_ARG(GetAddr1Ptr(pframe)));
	RTW_INFO("A2-" MAC_FMT "\n", MAC_ARG(get_addr2_ptr(pframe)));
	RTW_INFO("A3-" MAC_FMT "\n", MAC_ARG(GetAddr3Ptr(pframe)));

	if (GetFrameType(pframe) != WIFI_MGT_TYPE)
		return NULL;

	switch (get_frame_sub_type(pframe)) {
	case WIFI_AUTH:
		{
		u16 algo, seq;

		algo = le16_to_cpu(*(u16 *)(pframe + WLAN_HDR_A3_LEN));
		seq = le16_to_cpu(*(u16 *)(pframe + WLAN_HDR_A3_LEN + 2));

		if ((algo == 0) && (seq == 1))
			rframe = gen_auth(a, GetAddr1Ptr(pframe));
		}
		break;

	case WIFI_ASSOCREQ:
	case WIFI_REASSOCREQ:
		rframe = gen_assocrsp(a, GetAddr1Ptr(pframe));
		break;

	case WIFI_PROBEREQ:
		rframe = gen_probersp(a);
		break;

	case WIFI_BEACON:
		rframe = gen_beacon(a, skb);

	default:
		break;
	}

	return rframe;
}

extern sint validate_recv_frame(struct _ADAPTER*, union recv_frame*);
void rtw_fakeap_work(struct work_struct *work)
{
	struct dvobj_priv *d;
	struct _ADAPTER *a;
	struct sk_buff_head *pktq;
	struct sk_buff *skb, *tmp;
	u8 *ta, *ra;
	union recv_frame *rframe;
	int i;


	d = container_of(work, struct dvobj_priv, fakeap.work);

	pktq = &d->fakeap.rxq;
	skb_queue_walk_safe(pktq, skb, tmp) {
		__skb_unlink(skb, pktq);

		ta = get_addr2_ptr(skb->data);
		ra = GetAddr1Ptr(skb->data);
		if (is_broadcast_mac_addr(ra)
		    && (_rtw_memcmp(ta, ap_addr, ETH_ALEN)==_TRUE)
		    && (get_frame_sub_type(skb->data) == WIFI_BEACON)) {
			for (i = 0; i < d->iface_nums; i++) {
				a = d->padapters[i];
				if (a) {
					rframe = fakeap_dispatcher(a, skb);
					if (rframe) {
						validate_recv_frame(a, rframe);
						rframe = NULL;
					}
				}
			}
		} else {
			a = rtw_get_iface_by_macddr(dvobj_get_primary_adapter(d),
						    ta);
			if (a)
				rframe = fakeap_dispatcher(a, skb);
		}
		rtw_skb_free(skb);
		if (rframe) {
			validate_recv_frame(a, rframe);
			rframe = NULL;
		}
	}
}

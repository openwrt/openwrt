/******************************************************************************
 *
 * Copyright(c) 2019 - 2020 Realtek Corporation.
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
#include "phl_headers.h"

#define HAL_PKT_OFLD_ADD(_pkt, _id, _pkt_buf, _len) \
	rtw_hal_pkt_ofld((_pkt)->phl_info->hal, _id, PKT_OFLD_ADD, _pkt_buf, _len)
#define HAL_PKT_OFLD_READ(_pkt, _id) \
	rtw_hal_pkt_ofld((_pkt)->phl_info->hal, _id, PKT_OFLD_READ, NULL, NULL)
#define HAL_PKT_OFLD_DEL(_pkt, _id) \
	rtw_hal_pkt_ofld((_pkt)->phl_info->hal, _id, PKT_OFLD_DEL, NULL, NULL)

static const char *
_phl_pkt_ofld_get_txt(u8 type)
{
	switch (type) {
		case PKT_TYPE_PROBE_RSP:
			return "PROBE RSP";
		case PKT_TYPE_PS_POLL:
			return "PS POLL";
		case PKT_TYPE_NULL_DATA:
			return "NULL DATA";
		case PKT_TYPE_QOS_NULL:
			return "QOS NULL";
		case PKT_TYPE_CTS2SELF:
			return "CTS2SELF";
		case PKT_TYPE_ARP_RSP:
			return "ARP Response";
		case PKT_TYPE_NDP:
			return "Neighbor Discovery Protocol";
		case PKT_TYPE_EAPOL_KEY:
			return "EAPOL-KEY";
		case PKT_TYPE_SA_QUERY:
			return "SA QUERY";
		case PKT_TYPE_PROBE_REQ:
			return "PROBE REQ";
		default:
			return "Unknown?!";
	}
}

static u8
_phl_pkt_ofld_is_pkt_ofld(struct pkt_ofld_info *pkt_info)
{
	if (pkt_info->id != NOT_USED)
		return true;
	else
		return false;
}

static void
_phl_pkt_ofld_dbg_dump_pkt_info(struct pkt_ofld_obj *ofld_obj,
				struct pkt_ofld_info *pkt_info)
{
	struct pkt_ofld_req *pos = NULL;

	phl_list_for_loop(pos, struct pkt_ofld_req, &pkt_info->req_q, list) {

		PHL_TRACE(COMP_PHL_PKTOFLD, _PHL_INFO_,
			"[PKT] ## token %d, req name unknown\n",
			pos->token);
	}

	if (_phl_pkt_ofld_is_pkt_ofld(pkt_info)) {
		HAL_PKT_OFLD_READ(ofld_obj, &pkt_info->id);
	}

}

static void
_phl_pkt_ofld_dbg_dump(struct pkt_ofld_obj *ofld_obj)
{
	u8 idx;
	struct pkt_ofld_entry *pos = NULL;

	phl_list_for_loop(pos, struct pkt_ofld_entry, &ofld_obj->entry_q, list) {

		PHL_TRACE(COMP_PHL_PKTOFLD, _PHL_INFO_,
			"[PKT] => mac id = %d\n", pos->macid);

		for (idx = 0; idx < PKT_OFLD_TYPE_MAX; idx++) {

			PHL_TRACE(COMP_PHL_PKTOFLD, _PHL_INFO_,
				"[PKT] type %-10s:id = %d, req cnt = %d.\n",
				_phl_pkt_ofld_get_txt(idx),
				pos->pkt_info[idx].id,
				pos->pkt_info[idx].req_cnt);

			_phl_pkt_ofld_dbg_dump_pkt_info(ofld_obj,
				&pos->pkt_info[idx]);

		}
	}
}

static struct pkt_ofld_req *
_phl_pkt_ofld_gen_req(struct pkt_ofld_obj *ofld_obj, const char *req_name)
{
	struct pkt_ofld_req *req;
	void *d = phl_to_drvpriv(ofld_obj->phl_info);

	req = _os_mem_alloc(d, sizeof(struct pkt_ofld_req));
	if (req == NULL) {
		PHL_ERR("[PKT] %s: alloc memory req failed.\n", __func__);
		return NULL;
	}

	INIT_LIST_HEAD(&req->list);
	req->token = ofld_obj->cur_seq++;
	req->req_name_len = _os_strlen((u8 *)req_name) +1;

	req->req_name = _os_mem_alloc(d, sizeof(u8)*req->req_name_len);
	if (req->req_name == NULL) {
		PHL_ERR("[PKT] %s: alloc memory req name failed.\n", __func__);
		_os_mem_free(d, req, sizeof(struct pkt_ofld_req));
		return NULL;
	}
	_os_mem_set(d, req->req_name, 0, req->req_name_len);
	_os_mem_cpy(d, req->req_name, (char *)req_name, req->req_name_len);

	PHL_TRACE(COMP_PHL_PKTOFLD, _PHL_INFO_,
		"[PKT] gen request %p, token %d, req %s\n",
		req, req->token, req_name);

	return req;
}

static void _phl_pkt_ofld_del_req(struct pkt_ofld_obj *ofld_obj, struct pkt_ofld_info *pkt_info,
			struct pkt_ofld_req *req)
{
	void *d = phl_to_drvpriv(ofld_obj->phl_info);

	list_del(&req->list);
	pkt_info->req_cnt--;
	_os_mem_free(d, req->req_name, sizeof(u8)*req->req_name_len);
	_os_mem_free(d, req, sizeof(*req));
}

static struct pkt_ofld_req *
_phl_pkt_ofld_get_req(struct pkt_ofld_obj *ofld_obj, struct pkt_ofld_info *pkt_info,
			u32 token)
{
	struct pkt_ofld_req *pos = NULL;
	u8 find = false;

	phl_list_for_loop(pos, struct pkt_ofld_req,
				&pkt_info->req_q, list) {
		if (pos->token == token) {
			find = true;
			break;
		}
	}

	if (find) {
		return pos;
	} else {
		return NULL;
	}
}

static void
_phl_pkt_ofld_add_req(struct pkt_ofld_obj *ofld_obj, struct pkt_ofld_info *pkt_info,
			struct pkt_ofld_req *req)
{
	list_add(&req->list, &pkt_info->req_q);
	pkt_info->req_cnt++;
}


static enum rtw_phl_status
_phl_pkt_ofld_construct_null_data(struct pkt_ofld_obj *ofld_obj, u8 **pkt_buf,
	u16 *len, struct rtw_phl_stainfo_t *phl_sta,
	struct rtw_pkt_ofld_null_info *null_info)
{
	void *d = phl_to_drvpriv(ofld_obj->phl_info);
	struct rtw_wifi_role_t *wrole = phl_sta->wrole;
	*len = NULL_PACKET_LEN;

	*pkt_buf = _os_mem_alloc(d, NULL_PACKET_LEN);
	if (*pkt_buf == NULL) {
		return RTW_PHL_STATUS_RESOURCE;
	}

	_os_mem_set(d, *pkt_buf, 0, NULL_PACKET_LEN);

	SET_80211_PKT_HDR_FRAME_CONTROL(*pkt_buf, 0);
	SET_80211_PKT_HDR_TYPE_AND_SUBTYPE(*pkt_buf, TYPE_NULL_FRAME);
	SET_80211_PKT_HDR_PWR_MGNT(*pkt_buf, 0);

	switch (wrole->type) {
	case PHL_RTYPE_STATION:
	case PHL_RTYPE_P2P_GC:
		SET_80211_PKT_HDR_TO_DS(*pkt_buf, 1);
		SET_80211_PKT_HDR_ADDRESS1(d, *pkt_buf, null_info->a1);
		SET_80211_PKT_HDR_ADDRESS2(d, *pkt_buf, null_info->a2);
		SET_80211_PKT_HDR_ADDRESS3(d, *pkt_buf, null_info->a3);
		break;
	case PHL_RTYPE_P2P_GO:
	case PHL_RTYPE_AP:
		SET_80211_PKT_HDR_FROM_DS(*pkt_buf, 1);
		SET_80211_PKT_HDR_ADDRESS1(d, *pkt_buf, null_info->a1);
		SET_80211_PKT_HDR_ADDRESS2(d, *pkt_buf, null_info->a2);
		SET_80211_PKT_HDR_ADDRESS3(d, *pkt_buf, null_info->a3);
		break;
	case PHL_RTYPE_ADHOC:
	default:
		break;
	}
	SET_80211_PKT_HDR_DURATION(*pkt_buf, 0);
	SET_80211_PKT_HDR_FRAGMENT_SEQUENCE(*pkt_buf, 0);

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status
_phl_pkt_ofld_construct_na(struct pkt_ofld_obj *pkt, u8 **pkt_buf,
	u16 *len, struct rtw_phl_stainfo_t *phl_sta,
	struct rtw_pkt_ofld_na_info *na_info)
{
	void *d = phl_to_drvpriv(pkt->phl_info);
	u8* p_na_body;
	u8 NSLLCHeader[8] = {0xAA, 0xAA, 0x03, 0x00, 0x00, 0x00, 0x86, 0xDD};
	u8 IPv6HeadInfo[4] = {0x60, 0x00, 0x00, 0x00};
	u8 IPv6HeadContx[4] = {0x00, 0x20, 0x3a, 0xff};
	u8 ICMPv6Head[8] = {0x88, 0x00, 0x00, 0x00 , 0x60 , 0x00 , 0x00 , 0x00};
	u8 is_protected = na_info->protect_bit;
	u8 sec_hdr_len = na_info->sec_hdr_len;

	/* size estimation */
	/* sMacHdrLng + LLC header(8) + na element(28) */
	*len = MAC_HDR_LEN + sec_hdr_len + 8 + 72;

	*pkt_buf = _os_mem_alloc(d, *len);

	if (*pkt_buf == NULL)
		return RTW_PHL_STATUS_RESOURCE;


	_os_mem_set(d, *pkt_buf, 0, *len);

	SET_80211_PKT_HDR_FRAME_CONTROL(*pkt_buf, 0);
	SET_80211_PKT_HDR_TYPE_AND_SUBTYPE(*pkt_buf, TYPE_DATA_FRAME);
	SET_80211_PKT_HDR_TO_DS(*pkt_buf, 1);

	SET_80211_PKT_HDR_PROTECT(*pkt_buf, is_protected);

	SET_80211_PKT_HDR_ADDRESS1(d, *pkt_buf, na_info->a1);
	SET_80211_PKT_HDR_ADDRESS2(d, *pkt_buf, na_info->a2);
	SET_80211_PKT_HDR_ADDRESS3(d, *pkt_buf, na_info->a3);

	SET_80211_PKT_HDR_DURATION(*pkt_buf, 0);
	SET_80211_PKT_HDR_FRAGMENT_SEQUENCE(*pkt_buf, 0);

	/* Frame Body */
	p_na_body =  (u8*)(*pkt_buf + MAC_HDR_LEN);

	/* offset for security iv */
	p_na_body += sec_hdr_len;

	/* LLC heade*/
	_os_mem_cpy(d, p_na_body, NSLLCHeader, 8);


	/* IPv6 Heard */


	/* 1 . Information (4 bytes): 0x60 0x00 0x00 0x00 */
	_os_mem_cpy(d, p_na_body+8, IPv6HeadInfo, 4);

	/* 2 . playload : 0x00 0x20 , NextProt : 0x3a (ICMPv6) HopLim : 0xff */
	_os_mem_cpy(d, p_na_body+12, IPv6HeadContx, 4);

	/* 3 . SA : 16 bytes , DA : 16 bytes ( Fw will fill) */
	_os_mem_set(d, p_na_body+16, 0, 32);


	/* ICMPv6 */

	/* 1. Type : 0x88 (NA) , Code : 0x00 , ChechSum : 0x00 0x00 (RSvd) NAFlag: 0x60 0x00 0x00 0x00 ( Solicited , Override) */
	_os_mem_cpy(d, p_na_body+48, ICMPv6Head, 8);

	/* 2. TA : 16 bytes */
	_os_mem_set(d, p_na_body+56, 0, 16);

	/* ICMPv6  Target Link Layer address */

	WriteLE1Byte(p_na_body+72, 0x02); /* type */
	WriteLE1Byte(p_na_body+73, 0x01); /* Len 1 unit of 8 octes */
	_os_mem_set(d, p_na_body+74, 0, 6); /* Target Link Address */

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status
_phl_pkt_ofld_construct_arp_rsp(struct pkt_ofld_obj *pkt, u8 **pkt_buf,
	u16 *len, struct rtw_phl_stainfo_t *phl_sta,
	struct rtw_pkt_ofld_arp_rsp_info *arp_rsp_info)
{
	void *d = phl_to_drvpriv(pkt->phl_info);
	u8* p_arp_rsp_body;
	u8 ARPLLCHeader[8] = {0xAA, 0xAA, 0x03, 0x00, 0x00, 0x00, 0x08, 0x06};
	u8 is_protected = arp_rsp_info->protect_bit;
	u8 sec_hdr_len = arp_rsp_info->sec_hdr_len;

	/* size estimation */
	/* sMacHdrLng + LLC header(8) + arp element(28) */
	*len = MAC_HDR_LEN + sec_hdr_len + 8 + 28;

	*pkt_buf = _os_mem_alloc(d, *len);

	if (*pkt_buf == NULL)
		return RTW_PHL_STATUS_RESOURCE;

	_os_mem_set(d, *pkt_buf, 0, *len);

	SET_80211_PKT_HDR_FRAME_CONTROL(*pkt_buf, 0);
	SET_80211_PKT_HDR_TYPE_AND_SUBTYPE(*pkt_buf, TYPE_DATA_FRAME);
	SET_80211_PKT_HDR_TO_DS(*pkt_buf, 1);

	SET_80211_PKT_HDR_PROTECT(*pkt_buf, is_protected);

	SET_80211_PKT_HDR_ADDRESS1(d, *pkt_buf, arp_rsp_info->a1);
	SET_80211_PKT_HDR_ADDRESS2(d, *pkt_buf, arp_rsp_info->a2);
	SET_80211_PKT_HDR_ADDRESS3(d, *pkt_buf, arp_rsp_info->a3);

	SET_80211_PKT_HDR_DURATION(*pkt_buf, 0);
	SET_80211_PKT_HDR_FRAGMENT_SEQUENCE(*pkt_buf, 0);

	/* Frame bod*/
	p_arp_rsp_body = (u8*)(*pkt_buf + MAC_HDR_LEN);

	/* offset for security iv */
	p_arp_rsp_body += sec_hdr_len;

	/* LLC header */
	_os_mem_cpy(d, p_arp_rsp_body, ARPLLCHeader, 8);

	/* ARP element */
	p_arp_rsp_body += 8;

	/* hardware type  = 0x10 Ethernet */
	WriteLE2Byte(p_arp_rsp_body, 0x0100);
	/* Protcol type = 0800 IP */
	WriteLE2Byte(p_arp_rsp_body+2, 0x0008);
	/* Hardware address length = 6 */
	WriteLE1Byte(p_arp_rsp_body+4, 6);
	/* Protocol address length = 4 */
	WriteLE1Byte(p_arp_rsp_body+5, 4);
	/* Operation = 0x0002 arp response */
	WriteLE2Byte(p_arp_rsp_body+6, 0x0200);
	/* Sender Mac Address */
	_os_mem_cpy(d, p_arp_rsp_body+8, arp_rsp_info->a2, 6);
	/* Sender IPv4 Address */
	_os_mem_cpy(d, p_arp_rsp_body+14,
		&(arp_rsp_info->host_ipv4_addr[0]), 4);
	/* Target Mac Address */
	_os_mem_cpy(d, p_arp_rsp_body+18, arp_rsp_info->remote_mac_addr, 6);
	/* Target IPv4 Address */
	_os_mem_cpy(d, p_arp_rsp_body+24,
		&(arp_rsp_info->remote_ipv4_addr[0]), 4);

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status
_phl_pkt_ofld_construct_eapol_key_data(struct pkt_ofld_obj *ofld_obj, u8 **pkt_buf,
				u16 *len, struct rtw_phl_stainfo_t *phl_sta,
				struct rtw_pkt_ofld_eapol_key_info *eapol_key_info)
{
	void *d = phl_to_drvpriv(ofld_obj->phl_info);
	u8 *pkt = NULL;
	u8 llc_hdr[] = {0xAA, 0xAA, 0x03};
	u8 llc_oui[] = {0x00, 0x00, 0x00};
	u8 llc_proto_id[] = {0x88, 0x8E};
	u8 is_protected = eapol_key_info->protect_bit;
	u8 sec_hdr_len = eapol_key_info->sec_hdr_len;
	u8 key_desc_ver = eapol_key_info->key_desc_ver;

	*len = MAC_HDR_LEN+sec_hdr_len+LLC_LEN+AUTH_1X_HDR_LEN+EAPOLMSG_HDR_LEN;

	*pkt_buf = _os_mem_alloc(d, *len);

	if (*pkt_buf == NULL)
		return RTW_PHL_STATUS_RESOURCE;

	pkt = *pkt_buf;

	_os_mem_set(d, pkt, 0, *len);

	SET_80211_PKT_HDR_FRAME_CONTROL(pkt, 0);
	SET_80211_PKT_HDR_TYPE_AND_SUBTYPE(pkt, TYPE_DATA_FRAME);
	SET_80211_PKT_HDR_TO_DS(pkt, 1);

	SET_80211_PKT_HDR_PROTECT(*pkt_buf, is_protected);

	SET_80211_PKT_HDR_ADDRESS1(d, pkt, eapol_key_info->a1);	/* BSSID */
	SET_80211_PKT_HDR_ADDRESS2(d, pkt, eapol_key_info->a2);	/* SA */
	SET_80211_PKT_HDR_ADDRESS3(d, pkt, eapol_key_info->a3);	/* DA */

	SET_80211_PKT_HDR_DURATION(pkt, 0);
	SET_80211_PKT_HDR_FRAGMENT_SEQUENCE(pkt, 0);
	pkt += MAC_HDR_LEN;

	/* offset for security iv */
	pkt += sec_hdr_len;

	/* LLC */
	SET_LLC_HDR(d, pkt, llc_hdr);
	SET_LLC_OUI(d, pkt, llc_oui);
	SET_LLC_PROTO_ID(d, pkt, llc_proto_id);
	pkt += LLC_LEN;

	/* 802.1x Auth hdr */
	SET_AUTH_1X_PROTO_VER(pkt, LIB1X_EAPOL_VER);
	SET_AUTH_1X_PKT_TYPE(pkt, LIB1X_TYPE_EAPOL_KEY);
	SET_AUTH_1X_LENGTH(pkt, EAPOLMSG_HDR_LEN);
	pkt += AUTH_1X_HDR_LEN;

	/* EAPOL-KEY */
	SET_EAPOLKEY_KEYDESC_TYPE(pkt, EAPOLKEY_KEYDESC_TYPE_RSN);
	SET_EAPOLKEY_KEY_INFO(pkt, EAPOLKEY_SECURE|EAPOLKEY_KEYMIC|key_desc_ver);

	/* SET_EAPOLKEY_REPLAY_CNT(d, pkt, eapol_key_info->replay_cnt); */

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status
_phl_pkt_ofld_construct_sa_query_pkt(struct pkt_ofld_obj *ofld_obj, u8 **pkt_buf,
				u16 *len, struct rtw_phl_stainfo_t *phl_sta,
				struct rtw_pkt_ofld_sa_query_info *sa_query_info)
{
	void *d = phl_to_drvpriv(ofld_obj->phl_info);
	u8 *pkt = NULL;
	u8 is_protected = sa_query_info->protect_bit;
	u8 sec_hdr_len = sa_query_info->sec_hdr_len;

	*len = MAC_HDR_LEN+sec_hdr_len+SAQ_ACTION_LEN;

	*pkt_buf = _os_mem_alloc(d, *len);

	if (*pkt_buf == NULL)
		return RTW_PHL_STATUS_RESOURCE;

	pkt = *pkt_buf;

	_os_mem_set(d, pkt, 0, *len);

	SET_80211_PKT_HDR_FRAME_CONTROL(pkt, 0);
	SET_80211_PKT_HDR_TYPE_AND_SUBTYPE(pkt, TYPE_ACTION_FRAME);
	SET_80211_PKT_HDR_DURATION(pkt, 0);

	SET_80211_PKT_HDR_PROTECT(pkt, is_protected);

	SET_80211_PKT_HDR_ADDRESS1(d, pkt, sa_query_info->a1); /* bssid */
	SET_80211_PKT_HDR_ADDRESS2(d, pkt, sa_query_info->a2); /* sa */
	SET_80211_PKT_HDR_ADDRESS3(d, pkt, sa_query_info->a3); /* da */

	SET_80211_PKT_HDR_FRAGMENT_SEQUENCE(pkt, 0);
	pkt += MAC_HDR_LEN;

	/* offset for security iv */
	pkt += sec_hdr_len;

	SET_ACTION_FRAME_CATEGORY(pkt, ACT_TYPE_SA_QUERY);
	SET_SAQ_ACTION_FIELD(pkt, 1);

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status
_phl_pkt_ofld_construct_realwow_kapkt(struct pkt_ofld_obj *ofld_obj, u8 **pkt_buf,
	u16 *len, struct rtw_phl_stainfo_t *phl_sta,
	struct rtw_pkt_ofld_realwow_kapkt_info *kapkt_info)
{
	void *d = phl_to_drvpriv(ofld_obj->phl_info);

	*len = kapkt_info->keep_alive_pkt_size;

	*pkt_buf = _os_mem_alloc(d, *len);
	if (*pkt_buf == NULL)
		return RTW_PHL_STATUS_RESOURCE;

	_os_mem_set(d, *pkt_buf, 0, *len);

	_os_mem_cpy(d, *pkt_buf, kapkt_info, *len);

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status
_phl_pkt_ofld_construct_realwow_ack(struct pkt_ofld_obj *ofld_obj, u8 **pkt_buf,
	u16 *len, struct rtw_phl_stainfo_t *phl_sta,
	struct rtw_pkt_ofld_realwow_ack_info *ack_info)
{
	void *d = phl_to_drvpriv(ofld_obj->phl_info);

	*len = ack_info->ack_ptrn_size;

	*pkt_buf = _os_mem_alloc(d, *len);
	if (*pkt_buf == NULL)
		return RTW_PHL_STATUS_RESOURCE;

	_os_mem_set(d, *pkt_buf, 0, *len);

	_os_mem_cpy(d, *pkt_buf, ack_info, *len);

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status
_phl_pkt_ofld_construct_realwow_wp(struct pkt_ofld_obj *ofld_obj, u8 **pkt_buf,
	u16 *len, struct rtw_phl_stainfo_t *phl_sta,
	struct rtw_pkt_ofld_realwow_wp_info *wake_info)
{
	void *d = phl_to_drvpriv(ofld_obj->phl_info);

	*len = wake_info->wakeup_ptrn_size;

	*pkt_buf = _os_mem_alloc(d, *len);
	if (*pkt_buf == NULL)
		return RTW_PHL_STATUS_RESOURCE;

	_os_mem_set(d, *pkt_buf, 0, *len);

	_os_mem_cpy(d, *pkt_buf, wake_info, *len);

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status
_phl_pkt_ofld_construct_probe_req(struct pkt_ofld_obj *ofld_obj, u8 **pkt_buf,
	u16 *len, struct rtw_pkt_ofld_probe_req_info *probe_req_info)
{
	void *d = phl_to_drvpriv(ofld_obj->phl_info);
	u8 wild_card[MAC_ADDRESS_LENGTH] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	u8 *tmp_buf = NULL;

	if (probe_req_info->construct_pbreq == NULL) {

		*len = MAC_HDR_LEN;
		*pkt_buf = _os_mem_alloc(d, MAC_HDR_LEN);
		if (*pkt_buf == NULL) {
			return RTW_PHL_STATUS_RESOURCE;
		}

		_os_mem_set(d, *pkt_buf, 0, MAC_HDR_LEN);

		SET_80211_PKT_HDR_FRAME_CONTROL(*pkt_buf, 0);
		SET_80211_PKT_HDR_TYPE_AND_SUBTYPE(*pkt_buf, TYPE_PROBE_REQ_FRAME);
		SET_80211_PKT_HDR_ADDRESS1(d, *pkt_buf, &wild_card);
		SET_80211_PKT_HDR_ADDRESS2(d, *pkt_buf, probe_req_info->a2);
		SET_80211_PKT_HDR_ADDRESS3(d, *pkt_buf, &wild_card);
		SET_80211_PKT_HDR_DURATION(*pkt_buf, 0);
		SET_80211_PKT_HDR_FRAGMENT_SEQUENCE(*pkt_buf, 0);

	} else {

		tmp_buf = _os_mem_alloc(d, MAX_MSDU_LEN + MAC_HDR_LEN + FCS_LEN);

		if (tmp_buf == NULL)
			return RTW_PHL_STATUS_RESOURCE;

		probe_req_info->construct_pbreq(d, tmp_buf, len);

		*pkt_buf = _os_mem_alloc(d, *len);
		if (*pkt_buf == NULL) {
			_os_mem_free(d, tmp_buf, MAX_MSDU_LEN + MAC_HDR_LEN + FCS_LEN);
			return RTW_PHL_STATUS_RESOURCE;
		}

		_os_mem_set(d, *pkt_buf, 0, *len);
		_os_mem_cpy(d, *pkt_buf, tmp_buf, *len);

		_os_mem_free(d, tmp_buf, MAX_MSDU_LEN + MAC_HDR_LEN + FCS_LEN);
	}

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status
_phl_pkt_ofld_construct_packet(struct pkt_ofld_obj *ofld_obj, u16 macid,
			u8 type, u8 **pkt_buf, u16 *len, void *buf)
{
	struct rtw_phl_stainfo_t *phl_sta = rtw_phl_get_stainfo_by_macid(
						ofld_obj->phl_info, macid);
	enum rtw_phl_status status = RTW_PHL_STATUS_SUCCESS;

	switch(type) {
	case PKT_TYPE_NULL_DATA:
		status = _phl_pkt_ofld_construct_null_data(ofld_obj, pkt_buf,
			len, phl_sta, (struct rtw_pkt_ofld_null_info *) buf);
		break;
	case PKT_TYPE_ARP_RSP:
		status = _phl_pkt_ofld_construct_arp_rsp(ofld_obj, pkt_buf,
			len, phl_sta, (struct rtw_pkt_ofld_arp_rsp_info *) buf);
		break;
	case PKT_TYPE_NDP:
		status = _phl_pkt_ofld_construct_na(ofld_obj, pkt_buf,
			len, phl_sta, (struct rtw_pkt_ofld_na_info *) buf);
		break;
	case PKT_TYPE_EAPOL_KEY:
		status = _phl_pkt_ofld_construct_eapol_key_data(ofld_obj, pkt_buf,
			len, phl_sta, (struct rtw_pkt_ofld_eapol_key_info *) buf);
		break;
	case PKT_TYPE_SA_QUERY:
		status = _phl_pkt_ofld_construct_sa_query_pkt(ofld_obj, pkt_buf,
			len, phl_sta, (struct rtw_pkt_ofld_sa_query_info *) buf);
		break;
	case PKT_TYPE_REALWOW_KAPKT:
		status = _phl_pkt_ofld_construct_realwow_kapkt(ofld_obj, pkt_buf,
			len, phl_sta, (struct rtw_pkt_ofld_realwow_kapkt_info *) buf);
		break;
	case PKT_TYPE_REALWOW_ACK:
		status = _phl_pkt_ofld_construct_realwow_ack(ofld_obj, pkt_buf,
			len, phl_sta, (struct rtw_pkt_ofld_realwow_ack_info *) buf);
		break;
	case PKT_TYPE_REALWOW_WP:
		status = _phl_pkt_ofld_construct_realwow_wp(ofld_obj, pkt_buf,
			len, phl_sta, (struct rtw_pkt_ofld_realwow_wp_info *) buf);
		break;
	case PKT_TYPE_PROBE_REQ:
		status = _phl_pkt_ofld_construct_probe_req(ofld_obj, pkt_buf,
			len, (struct rtw_pkt_ofld_probe_req_info *) buf);
		break;
	case PKT_TYPE_PROBE_RSP:
	case PKT_TYPE_PS_POLL:
	case PKT_TYPE_QOS_NULL:
	case PKT_TYPE_CTS2SELF:
	default:
		PHL_ERR("[PKT] packet type %s is not implemented.\n",
			_phl_pkt_ofld_get_txt(type));
		status = RTW_PHL_STATUS_FAILURE;
		break;
	}

	if (status == RTW_PHL_STATUS_SUCCESS) {
		PHL_TRACE(COMP_PHL_PKTOFLD, _PHL_INFO_,"%s:\n", _phl_pkt_ofld_get_txt(type));
		debug_dump_data((u8 *)*pkt_buf, *len, "construct packet");
	} else if (status == RTW_PHL_STATUS_RESOURCE) {
		PHL_ERR("[PKT] %s: alloc memory failed.\n", __func__);
	} else {
		PHL_ERR("[PKT] %s: failed.\n", __func__);
	}

	return status;
}

static void
_phl_pkt_ofld_init_entry(struct pkt_ofld_entry *entry, u16 macid)
{
	u8 idx;

	INIT_LIST_HEAD(&entry->list);
	entry->macid = macid;

	for (idx = 0; idx < PKT_OFLD_TYPE_MAX; idx++) {
		INIT_LIST_HEAD(&entry->pkt_info[idx].req_q);
		entry->pkt_info[idx].id = NOT_USED;
		entry->pkt_info[idx].req_cnt = 0;
	}
}

static u8
_phl_pkt_ofld_is_entry_exist(struct pkt_ofld_obj *ofld_obj, u16 macid)
{
	struct pkt_ofld_entry *pos = NULL;

	phl_list_for_loop(pos, struct pkt_ofld_entry, &ofld_obj->entry_q, list) {
		if (pos->macid == macid) {

			PHL_ERR("[PKT] %s, mac id(%d) already in entry queue.\n",
				__func__, macid);
			return true;
		}
	}

	return false;
}

static void
_phl_pkt_ofld_del_all_req(struct pkt_ofld_obj *ofld_obj,
				struct pkt_ofld_info *pkt_info)
{
	struct pkt_ofld_req *pos = NULL;
	struct pkt_ofld_req *n = NULL;

	phl_list_for_loop_safe(pos, n, struct pkt_ofld_req,
				&pkt_info->req_q, list) {

		_phl_pkt_ofld_del_req(ofld_obj, pkt_info, pos);
	}
}

static void
_phl_pkt_ofld_cancel_entry_pkt(struct pkt_ofld_obj *ofld_obj,
				struct pkt_ofld_entry *entry)
{
	u8 idx;
	u8 id;

	for (idx = 0; idx < PKT_OFLD_TYPE_MAX; idx++) {
		if (_phl_pkt_ofld_is_pkt_ofld(&entry->pkt_info[idx])) {

			id = entry->pkt_info[idx].id;
			if (HAL_PKT_OFLD_DEL(ofld_obj, &id) !=
					RTW_HAL_STATUS_SUCCESS) {

				PHL_ERR("[PKT] %s: delete pkt(%d) failed, id(%d).\n",
					__func__, idx, id);
			}
		}
	}
}

static void
_phl_pkt_ofld_del_entry_req(struct pkt_ofld_obj *ofld_obj,
				struct pkt_ofld_entry *entry)
{
	u8 idx;

	for (idx = 0; idx < PKT_OFLD_TYPE_MAX; idx++) {
		entry->pkt_info[idx].id = NOT_USED;
		_phl_pkt_ofld_del_all_req(ofld_obj, &entry->pkt_info[idx]);
	}
}

static void
_phl_pkt_ofld_add_entry(struct pkt_ofld_obj *ofld_obj,
				struct pkt_ofld_entry *entry, u16 macid)
{
	PHL_TRACE(COMP_PHL_PKTOFLD, _PHL_INFO_,
		"[PKT] New entry %p, mac id = %d\n", entry, macid);

	_phl_pkt_ofld_init_entry(entry, macid);
	list_add(&entry->list, &ofld_obj->entry_q);
	ofld_obj->entry_cnt++;

	if (rtw_hal_pkt_update_ids(ofld_obj->phl_info->hal, entry)
					!= RTW_HAL_STATUS_SUCCESS) {
		PHL_WARN("%s: init general id failed.\n", __func__);
	}
}

static void
_phl_pkt_ofld_del_entry(struct pkt_ofld_obj *ofld_obj, struct pkt_ofld_entry *entry)
{
	void *d = phl_to_drvpriv(ofld_obj->phl_info);

	list_del(&entry->list);
	ofld_obj->entry_cnt--;
	_os_mem_free(d, entry, sizeof(*entry));
}

static struct pkt_ofld_entry *
_phl_pkt_ofld_get_entry(struct pkt_ofld_obj *ofld_obj, u16 macid)
{
	struct pkt_ofld_entry *pos = NULL;
	u8 find = false;

	phl_list_for_loop(pos, struct pkt_ofld_entry, &ofld_obj->entry_q, list) {
		if (pos->macid == macid) {
			find = true;
			break;
		}
	}

	if (find) {
		return pos;
	} else {
		return NULL;
	}
}

static enum rtw_phl_status
_phl_pkt_ofld_add_pkt(struct pkt_ofld_obj *ofld_obj,
		struct pkt_ofld_entry *entry, u8 type, void *buf)
{
	enum rtw_phl_status phl_status;
	enum rtw_hal_status hal_status;
	struct pkt_ofld_info *pkt_info = &entry->pkt_info[type];
	void *d = phl_to_drvpriv(ofld_obj->phl_info);
	u8 *pkt_buf = NULL;
	u16 len = 0;

	if (_phl_pkt_ofld_is_pkt_ofld(pkt_info)) {
		PHL_TRACE(COMP_PHL_PKTOFLD, _PHL_INFO_,
			"[PKT] %s: %s is already offloaded.\n",
			__func__, _phl_pkt_ofld_get_txt(type));
		return RTW_PHL_STATUS_SUCCESS;
	}

	phl_status = _phl_pkt_ofld_construct_packet(ofld_obj, entry->macid, type,
			&pkt_buf, &len, buf);
	if (phl_status != RTW_PHL_STATUS_SUCCESS) {
		PHL_ERR("[PKT] %s: construct packet type(%d) failed.\n",
			__func__, type);
		return RTW_PHL_STATUS_FAILURE;
	}

	hal_status = HAL_PKT_OFLD_ADD(ofld_obj, &pkt_info->id, pkt_buf, &len);

	_os_mem_free(d, pkt_buf, len);

	if (hal_status != RTW_HAL_STATUS_SUCCESS) {
		pkt_info->id = NOT_USED;
		PHL_ERR("[PKT] %s: add packet offload(%d) failed.\n", __func__,
				pkt_info->id);
		return RTW_PHL_STATUS_FAILURE;
	}

	hal_status = rtw_hal_pkt_update_ids(ofld_obj->phl_info->hal, entry);
	if (hal_status != RTW_HAL_STATUS_SUCCESS) {
		pkt_info->id = NOT_USED;
		PHL_ERR("[PKT] %s: update id failed.\n", __func__);
		return RTW_PHL_STATUS_FAILURE;
	}

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status
_phl_pkt_ofld_del_pkt(struct pkt_ofld_obj *ofld_obj,
				struct pkt_ofld_entry *entry, u8 type)
{
	struct pkt_ofld_info *pkt_info = &entry->pkt_info[type];

	if (_phl_pkt_ofld_is_pkt_ofld(pkt_info) == false) {
		PHL_ERR("[PKT] %s, %s is not offload to FW.\n", __func__, _phl_pkt_ofld_get_txt(type));
		return RTW_PHL_STATUS_FAILURE;
	}

	if ((entry->pkt_info[type].req_cnt-1) != 0) {
		PHL_TRACE(COMP_PHL_PKTOFLD, _PHL_INFO_,
			"[PKT] %s: %s is still requested (cnt %d).\n",
			__func__, _phl_pkt_ofld_get_txt(type), entry->pkt_info[type].req_cnt-1);
		return RTW_PHL_STATUS_SUCCESS;
	}

	if (HAL_PKT_OFLD_DEL(ofld_obj, &entry->pkt_info[type].id)
				!= RTW_HAL_STATUS_SUCCESS) {

		PHL_ERR("[PKT] %s: delete id(%d) failed.\n", __func__,
				entry->pkt_info[type].id);
		return RTW_PHL_STATUS_FAILURE;
	}
	entry->pkt_info[type].id = NOT_USED;

	return RTW_PHL_STATUS_SUCCESS;
}

/* For EXTERNAL application to create packet offload object */
enum rtw_phl_status phl_pkt_ofld_init(struct phl_info_t *phl_info)
{
	void *d = phl_to_drvpriv(phl_info);
	struct pkt_ofld_obj *ofld_obj;

	ofld_obj = (struct pkt_ofld_obj *)_os_mem_alloc(d, sizeof(*ofld_obj));
	if (ofld_obj == NULL)
		return RTW_PHL_STATUS_RESOURCE;

	phl_info->pkt_ofld = ofld_obj;

	ofld_obj->phl_info = phl_info;
	INIT_LIST_HEAD(&ofld_obj->entry_q);
	ofld_obj->entry_cnt = 0;
	ofld_obj->cur_seq = 0;
	_os_mutex_init(d, &ofld_obj->mux);

	return RTW_PHL_STATUS_SUCCESS;
}

/* For EXTERNAL application to free packet offload object */
void phl_pkt_ofld_deinit(struct phl_info_t *phl_info)
{
	struct pkt_ofld_obj *ofld_obj = phl_info->pkt_ofld;
	struct pkt_ofld_entry *pos = NULL;
	struct pkt_ofld_entry *n = NULL;
	void *d = phl_to_drvpriv(phl_info);

	_os_mutex_lock(d, &ofld_obj->mux);

	phl_list_for_loop_safe(pos, n, struct pkt_ofld_entry,
				&ofld_obj->entry_q, list) {

		_phl_pkt_ofld_del_entry_req(ofld_obj, pos);
		_phl_pkt_ofld_del_entry(ofld_obj, pos);
	}

	_os_mutex_unlock(d, &ofld_obj->mux);
	_os_mutex_deinit(d, &ofld_obj->mux);

	_os_mem_free(d, ofld_obj, sizeof(*ofld_obj));
}

/* For EXTERNAL application to reset all entry */
void phl_pkt_ofld_del_all_entry_req(struct phl_info_t *phl_info)
{
	struct pkt_ofld_obj *ofld_obj = phl_info->pkt_ofld;
	struct pkt_ofld_entry *pos = NULL;
	void *d = phl_to_drvpriv(phl_info);

	_os_mutex_lock(d, &ofld_obj->mux);

	phl_list_for_loop(pos, struct pkt_ofld_entry,
				&ofld_obj->entry_q, list) {

		_phl_pkt_ofld_del_entry_req(ofld_obj, pos);
	}

	_os_mutex_unlock(d, &ofld_obj->mux);
}

/* For EXTERNAL application to add entry to do packet offload (expose)
 * @phl: refer to rtw_phl_com_t
 * @macid: the mac id of STA
 */
enum rtw_phl_status phl_pkt_ofld_add_entry(struct phl_info_t *phl_info, u16 macid)
{
	struct pkt_ofld_obj *ofld_obj = phl_info->pkt_ofld;
	void *d = phl_to_drvpriv(ofld_obj->phl_info);
	struct pkt_ofld_entry *entry = NULL;

	if (ofld_obj == NULL) {
		PHL_ERR("[PKT] %s: pkt_ofld_obj is NULL.\n", __func__);
		return RTW_PHL_STATUS_FAILURE;
	}

	_os_mutex_lock(d, &ofld_obj->mux);

	if (_phl_pkt_ofld_is_entry_exist(ofld_obj, macid)) {
		_os_mutex_unlock(d, &ofld_obj->mux);
		return RTW_PHL_STATUS_FAILURE;
	}

	entry = _os_mem_alloc(d, sizeof(*entry));
	if (entry == NULL) {
		PHL_ERR("[PKT] %s: alloc memory failed.\n", __func__);
		_os_mutex_unlock(d, &ofld_obj->mux);
		return RTW_PHL_STATUS_RESOURCE;
	}

	_phl_pkt_ofld_add_entry(ofld_obj, entry, macid);

	_os_mutex_unlock(d, &ofld_obj->mux);

	return RTW_PHL_STATUS_SUCCESS;
}

/* For EXTERNAL application to remove entry (expose)
 * @phl: refer to rtw_phl_com_t
 * @macid: the mac id of STA
*/
enum rtw_phl_status phl_pkt_ofld_del_entry(struct phl_info_t *phl_info, u16 macid)
{
	struct pkt_ofld_obj *ofld_obj = phl_info->pkt_ofld;
	void *d = phl_to_drvpriv(ofld_obj->phl_info);
	struct pkt_ofld_entry *entry = NULL;

	if (ofld_obj == NULL) {
		PHL_ERR("[PKT] %s: pkt_ofld_obj is NULL.\n", __func__);
		return RTW_PHL_STATUS_FAILURE;
	}

	_os_mutex_lock(d, &ofld_obj->mux);

	entry = _phl_pkt_ofld_get_entry(ofld_obj, macid);
	if (entry == NULL) {
		_os_mutex_unlock(d, &ofld_obj->mux);
		PHL_ERR("[PKT] %s, mac id(%d) not found.\n", __func__, macid);
		return RTW_PHL_STATUS_FAILURE;
	}

	PHL_TRACE(COMP_PHL_PKTOFLD, _PHL_INFO_,
		"[PKT] remove entry %p, mac id = %d\n", entry, macid);

	_phl_pkt_ofld_cancel_entry_pkt(ofld_obj, entry);
	_phl_pkt_ofld_del_entry_req(ofld_obj, entry);
	_phl_pkt_ofld_del_entry(ofld_obj, entry);

	_os_mutex_unlock(d, &ofld_obj->mux);

	return RTW_PHL_STATUS_SUCCESS;
}

enum rtw_phl_status rtw_phl_pkt_ofld_reset_entry(struct rtw_phl_com_t* phl_com, u16 macid)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl_com->phl_priv;
	void *d = phl_com->drv_priv;
	struct pkt_ofld_obj *ofld_obj = phl_info->pkt_ofld;
	struct pkt_ofld_entry *entry = NULL;

	if (ofld_obj == NULL) {
		PHL_ERR("[PKT] %s: pkt_ofld_obj is NULL.\n", __func__);
		return RTW_PHL_STATUS_FAILURE;
	}

	_os_mutex_lock(d, &ofld_obj->mux);

	entry = _phl_pkt_ofld_get_entry(ofld_obj, macid);
	if (entry == NULL) {
		_os_mutex_unlock(d, &ofld_obj->mux);
		PHL_ERR("[PKT] %s, mac id(%d) not found.\n", __func__, macid);
		return RTW_PHL_STATUS_FAILURE;
	}

	PHL_TRACE(COMP_PHL_PKTOFLD, _PHL_INFO_,
		"[PKT] remove entry %p, mac id = %d\n", entry, macid);

	_phl_pkt_ofld_cancel_entry_pkt(ofld_obj, entry);
	_phl_pkt_ofld_del_entry_req(ofld_obj, entry);

	_os_mutex_unlock(d, &ofld_obj->mux);

	return RTW_PHL_STATUS_SUCCESS;
}

/* For EXTERNAL application to request packet offload to FW (expose)
 * @phl: refer to rtw_phl_com_t
 * @macid: the mac id of STA
 * @type: The type of packet
 * @token: The identifier (return to caller)
 * @req_name: The function name of caller
*/
enum rtw_phl_status
rtw_phl_pkt_ofld_request(struct phl_info_t *phl_info, u16 macid, u8 type,
		u32 *token, void *buf, const char *req_name)
{
	struct pkt_ofld_obj *ofld_obj = phl_info->pkt_ofld;
	void *d = phl_to_drvpriv(ofld_obj->phl_info);
	struct pkt_ofld_entry *entry = NULL;
	struct pkt_ofld_req *req = NULL;

	if (ofld_obj == NULL) {
		PHL_ERR("[PKT] %s: pkt_ofld_obj is NULL.\n", __func__);
		return RTW_PHL_STATUS_FAILURE;
	}

	_os_mutex_lock(d, &ofld_obj->mux);

	entry = _phl_pkt_ofld_get_entry(ofld_obj, macid);
	if (entry == NULL) {
		_os_mutex_unlock(d, &ofld_obj->mux);
		PHL_ERR("[PKT] %s, mac id(%d) not found.\n", __func__, macid);
		return RTW_PHL_STATUS_FAILURE;
	}

	req = _phl_pkt_ofld_gen_req(ofld_obj, req_name);
	if (req == NULL) {
		_os_mutex_unlock(d, &ofld_obj->mux);
		return RTW_PHL_STATUS_RESOURCE;
	}
	_phl_pkt_ofld_add_req(ofld_obj, &entry->pkt_info[type], req);

	if (_phl_pkt_ofld_add_pkt(ofld_obj, entry, type, buf) !=
			RTW_PHL_STATUS_SUCCESS) {

		_phl_pkt_ofld_del_req(ofld_obj, &entry->pkt_info[type], req);
		_os_mutex_unlock(d, &ofld_obj->mux);
		return RTW_PHL_STATUS_FAILURE;
	}

	if (token)
		*token = req->token;

	_os_mutex_unlock(d, &ofld_obj->mux);

	PHL_TRACE(COMP_PHL_PKTOFLD, _PHL_INFO_,
		"[PKT] request: macid %d, pkt type %s, token %d, req %s.\n",
		entry->macid, _phl_pkt_ofld_get_txt(type),
		(token ? *token : NOT_USED), req_name);

	return RTW_PHL_STATUS_SUCCESS;
}

/* For EXTERNAL application to cancel request (expose)
 * @phl: refer to rtw_phl_com_t
 * @macid: the mac id of STA
 * @type: The type of packet
 * @token: The identifier to get the request to be canceled
*/
enum rtw_phl_status rtw_phl_pkt_ofld_cancel(struct phl_info_t *phl_info,
					u16 macid, u8 type, u32 *token)
{
	struct pkt_ofld_obj *ofld_obj = phl_info->pkt_ofld;
	void *d = phl_to_drvpriv(ofld_obj->phl_info);
	struct pkt_ofld_entry *entry = NULL;
	struct pkt_ofld_req *req = NULL;
	struct pkt_ofld_info *pkt_info = NULL;

	if (ofld_obj == NULL) {
		PHL_ERR("[PKT] %s: pkt_ofld_obj is NULL.\n", __func__);
		return RTW_PHL_STATUS_FAILURE;
	}

	_os_mutex_lock(d, &ofld_obj->mux);

	entry = _phl_pkt_ofld_get_entry(ofld_obj, macid);
	if (entry == NULL) {
		_os_mutex_unlock(d, &ofld_obj->mux);
		PHL_ERR("[PKT] %s, macid(%d) not found.\n", __func__, macid);
		return RTW_PHL_STATUS_FAILURE;
	}
	pkt_info = &entry->pkt_info[type];

	if (pkt_info->id == NOT_USED) {
		_os_mutex_unlock(d, &ofld_obj->mux);
		PHL_WARN("[PKT] %s, macid %d, type %s is not used.\n", __func__,
		         entry->macid, _phl_pkt_ofld_get_txt(type));
		return RTW_PHL_STATUS_SUCCESS;
	}

	req = _phl_pkt_ofld_get_req(ofld_obj, pkt_info, *token);
	if (req == NULL) {
		_os_mutex_unlock(d, &ofld_obj->mux);
		PHL_ERR("[PKT] %s, token(%d) not found.\n", __func__, *token);
		return RTW_PHL_STATUS_FAILURE;
	}

	PHL_TRACE(COMP_PHL_PKTOFLD, _PHL_INFO_,
		"[PKT] cancel: macid %d, type %s, token %d.\n",
		entry->macid, _phl_pkt_ofld_get_txt(type), *token);

	if (_phl_pkt_ofld_del_pkt(ofld_obj, entry, type)
			!= RTW_PHL_STATUS_SUCCESS) {
		_os_mutex_unlock(d, &ofld_obj->mux);
		return RTW_PHL_STATUS_FAILURE;
	}
	_phl_pkt_ofld_del_req(ofld_obj, pkt_info, req);

	_os_mutex_unlock(d, &ofld_obj->mux);

	return RTW_PHL_STATUS_SUCCESS;
}

enum rtw_phl_status
rtw_phl_pkt_ofld_null_request(struct rtw_phl_com_t* phl_com,
                              struct rtw_phl_stainfo_t *sta, u32 *token)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	struct rtw_pkt_ofld_null_info null_info = {0};
	struct phl_info_t *phl_info = (struct phl_info_t *)phl_com->phl_priv;
	void *d = phl_com->drv_priv;

	if (!rtw_phl_role_is_client_category(sta->wrole)) {
		PHL_WARN("%s: skip non client role\n", __func__);
		return RTW_PHL_STATUS_FAILURE;
	}

	_os_mem_cpy(d, &(null_info.a1[0]), &(sta->mac_addr[0]),
		MAC_ADDRESS_LENGTH);

	_os_mem_cpy(d,&(null_info.a2[0]), &(sta->wrole->mac_addr[0]),
			MAC_ADDRESS_LENGTH);

	_os_mem_cpy(d, &(null_info.a3[0]), &(sta->mac_addr[0]),
			MAC_ADDRESS_LENGTH);

	pstatus = rtw_phl_pkt_ofld_request(phl_info, sta->macid,
				PKT_TYPE_NULL_DATA, token, &null_info, __func__);

	if (pstatus != RTW_PHL_STATUS_SUCCESS)
		PHL_WARN("%s(): add null pkt ofld fail!\n", __func__);

	return pstatus;
}

/* For EXTERNAL application to show current info (expose)
 * @phl: refer to rtw_phl_com_t
*/
void phl_pkt_ofld_show_info(struct phl_info_t *phl_info)
{
	struct pkt_ofld_obj *ofld_obj = phl_info->pkt_ofld;
	void *d = phl_to_drvpriv(ofld_obj->phl_info);

	_os_mutex_lock(d, &ofld_obj->mux);

	_phl_pkt_ofld_dbg_dump(ofld_obj);

	_os_mutex_unlock(d, &ofld_obj->mux);
}

/* For EXTERNAL application to get id (expose)
 * @phl: refer to rtw_phl_com_t
 * @macid: the mac id of STA
 * @type: The type of packet
*/
u8 phl_pkt_ofld_get_id(struct phl_info_t *phl_info, u16 macid, u8 type)
{
	struct pkt_ofld_obj *ofld_obj = phl_info->pkt_ofld;
	void *d = phl_to_drvpriv(ofld_obj->phl_info);
	struct pkt_ofld_entry *entry = NULL;
	struct pkt_ofld_info *pkt_info = NULL;

	if (ofld_obj == NULL) {
		PHL_ERR("[PKT] %s: pkt_ofld_obj is NULL.\n", __func__);
		return RTW_PHL_STATUS_FAILURE;
	}

	_os_mutex_lock(d, &ofld_obj->mux);

	entry = _phl_pkt_ofld_get_entry(ofld_obj, macid);
	if (entry == NULL) {
		_os_mutex_unlock(d, &ofld_obj->mux);
		PHL_ERR("[PKT] %s, macid(%d) not found.\n", __func__, macid);
		return RTW_PHL_STATUS_FAILURE;
	}
	pkt_info = &entry->pkt_info[type];

	PHL_TRACE(COMP_PHL_PKTOFLD, _PHL_INFO_,
		"[PKT] get id: macid %d, pkt type %s, id %d.\n",
		entry->macid, _phl_pkt_ofld_get_txt(type),
		pkt_info->id);

	_os_mutex_unlock(d, &ofld_obj->mux);

	return pkt_info->id;
}

const char *phl_get_pkt_ofld_str(enum pkt_ofld_type type)
{
	switch(type) {
	case PKT_TYPE_NULL_DATA:
		return "PKT_TYPE_NULL_DATA";
	case PKT_TYPE_ARP_RSP:
		return "PKT_TYPE_ARP_RSP";
	case PKT_TYPE_NDP:
		return "PKT_TYPE_NDP";
	case PKT_TYPE_EAPOL_KEY:
		return "PKT_TYPE_EAPOL_KEY";
	case PKT_TYPE_SA_QUERY:
		return "PKT_TYPE_SA_QUERY";
	case PKT_TYPE_PROBE_REQ:
		return "PKT_TYPE_PROBE_REQ";
	default:
		return "UNKNOWN_PKT_TYPE";
	}
}

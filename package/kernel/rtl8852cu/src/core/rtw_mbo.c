/******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation.
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
#define _RTW_MBO_C

#include <drv_types.h>

#ifdef CONFIG_RTW_MBO

#ifndef RTW_MBO_DBG
#define RTW_MBO_DBG	0
#endif
#if RTW_MBO_DBG
	#define RTW_MBO_INFO(fmt, arg...)	\
		RTW_INFO(fmt, arg)
	#define RTW_MBO_DUMP(str, data, len)	\
		RTW_INFO_DUMP(str, data, len)
#else
	#define RTW_MBO_INFO(fmt, arg...) do {} while (0)
	#define RTW_MBO_DUMP(str, data, len) do {} while (0)
#endif

/* Cellular Data Connectivity field
 * 1 : Cellular data connection available
 * 2 : Cellular data connection not available
 * 3 : Not Cellular data capable
 * otherwise : Reserved
*/
int rtw_mbo_cell_data_conn = 2;
module_param(rtw_mbo_cell_data_conn, int, 0644);

static u8 wfa_mbo_oui[] = {0x50, 0x6F, 0x9A, 0x16};

#define rtw_mbo_get_oui(p) ((u8 *)(p) + 2)

#define rtw_mbo_get_attr_id(p) ((u8 *)(p))

#define rtw_mbo_get_disallow_res(p) ((u8 *)(p) + 3)

#define rtw_mbo_set_1byte_ie(p, v, l)	\
	rtw_set_fixed_ie((p), 1, (v), (l))

#define rtw_mbo_set_2byte_ie(p, v, l)	\
	rtw_set_fixed_ie((p), 2, (v), (l))

#define rtw_mbo_set_4byte_ie(p, v, l)	\
	rtw_set_fixed_ie((p), 4, (v), (l))

#define rtw_mbo_set_nbyte_ie(p, sz, v, l)	\
	rtw_set_fixed_ie((p), (sz), (v), (l))

#define rtw_mbo_subfield_set(p, offset, val) (*(p + offset) = val)

#define rtw_mbo_subfields_set(p, offset, buf, len)	\
	do {	\
		u32 _offset = 0;	\
		u8 *_p = p + offset;	\
		while(_offset < len) {	\
			*(_p + _offset) = *(buf + _offset);	\
			_offset++;	\
		}	\
	} while(0)


void rtw_mbo_ie_init(_adapter *padapter, struct mbo_priv *mbopriv)
{
	if(!mbopriv)
		return;
	mbopriv->assoc_disallow = 0;
	mbopriv->cellular_aware = 0;
	mbopriv->ch_list_num = 0;
	mbopriv->mbo_oce_element_len = 6;
	mbopriv->mbo_oce_element[0] = 0xdd;
	mbopriv->mbo_oce_element[1] = mbopriv->mbo_oce_element_len;
	mbopriv->mbo_oce_element[2] = 0x50;
	mbopriv->mbo_oce_element[3] = 0x6f;
	mbopriv->mbo_oce_element[4] = 0x9a;
	mbopriv->mbo_oce_element[5] = 0x16;
}

void rtw_mbo_fill_non_prefer_channel_list(_adapter *padapter, struct mbo_priv *mbopriv,
										const u8 *pbuf, u8 len)
{
	u8 op_class = 0;
	u8 preference = 0;
	int i;

	/* invalid length */
	if(len != 0 && len < 3)
		return;

	/* reset non-prefer channel list */
	mbopriv->ch_list_num = 0;
	op_class = *pbuf;
	preference = *(pbuf + len - 2);

	if (len == 3 && mbopriv->ch_list_num < MBO_CH_LIST_MAX_NUM) {
		mbopriv->ch_list[mbopriv->ch_list_num].op_class = op_class;
		mbopriv->ch_list[mbopriv->ch_list_num].preference = preference;
		mbopriv->ch_list[mbopriv->ch_list_num].channel = 0;
		mbopriv->ch_list_num += 1;
		RTW_INFO("[%s:%d]channel = %d, preference = %d\n", __func__, __LINE__, 0, preference);
	} else {
		for (i = 0; i < len - 3; i++) {
			if(mbopriv->ch_list_num >= MBO_CH_LIST_MAX_NUM)
				break;
			mbopriv->ch_list[mbopriv->ch_list_num].op_class = op_class;
			mbopriv->ch_list[mbopriv->ch_list_num].preference = preference;
			mbopriv->ch_list[mbopriv->ch_list_num].channel = *(pbuf + 1 + i);
			mbopriv->ch_list_num += 1;
			RTW_INFO("[%s:%d]channel = %d, preference = %d\n", __func__, __LINE__,
									*(pbuf + 1 + i), preference);
		}
	}
}

void rtw_mbo_ie_handler(_adapter *padapter, struct mbo_priv *mbopriv, const u8 *pbuf, uint limit_len)
{
	struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);
	uint total_len = 0;
	u8 attribute_id = 0;
	u8 attribute_len = 0;
	const u8 *p = pbuf;

	if(!mbopriv)
		return;

	rtw_mbo_ie_init(padapter, mbopriv);
	_rtw_memcpy(mbopriv->mbo_oce_element + 6, pbuf, limit_len);
	mbopriv->mbo_oce_element[1] = limit_len + 4;

	while (total_len <= limit_len) {
		attribute_id = *p;
		attribute_len = *(p + 1);
		total_len += attribute_len;
		if(total_len > limit_len)
			break;

		switch (attribute_id) {
			case MBO_AP_CAPABILITY:
				if(attribute_len == 1){
					RTW_INFO("[%s]Find attribute MBO_AP_CAPABILITY\n", __func__);
					if(*(p+2) & 0x40)
						mbopriv->cellular_aware = 1;
				}
				break;
			case ASSOCIATION_DISALLOW:
				if(attribute_len == 1){
					RTW_INFO("[%s]Find attribute ASSOCIATION_DISALLOW\n", __func__);
					mbopriv->assoc_disallow = *(p+2);
				}
				break;
			case NON_PREFER_CHANNEL_RPT:
				RTW_INFO("[%s]Find attribute NON_PREFER_CHANNEL_RPT\n", __func__);
				rtw_mbo_fill_non_prefer_channel_list(padapter, mbopriv, p + 2, attribute_len);
				break;
			case CELLULAR_DATA_CAPABILITY:
			case CELLULAR_DATA_CONNECT_PREFER:
			case TRANS_REASON_CODE:
			case TRANS_REJECT_REASON_CODE:
			case ASSOCIATION_RETRY_DELAY:
				break;
			default:
				RTW_ERR("[%s]Unknown MBO attribute %d\n", __func__, attribute_id);
		}

		p += (attribute_len + 2);
	}
}

void rtw_ap_parse_sta_mbo_element(_adapter *padapter,
	struct sta_info *psta, u8 *ies_buf, u16 ies_len)
{
	uint ie_len = 0;
	u8 *p;
	u8 WIFI_ALLIANCE_OUI[] = {0x50, 0x6f, 0x9a};

	ie_len = 0;
	for (p = ies_buf; ; p += (ie_len + 2)) {
		p = rtw_get_ie(p, _SSN_IE_1_, &ie_len, (ies_len - (ie_len + 2)));
		if ((p) && (_rtw_memcmp(p + 2, WIFI_ALLIANCE_OUI, 3)) && (*(p+5) == MBO_OUI_TYPE)) {
			/* find MBO-OCE information element */
			psta->mbopriv.enable = _TRUE;
			rtw_mbo_ie_handler(padapter, &psta->mbopriv, p + 6, ie_len - 4);
			break;
		}
		if ((p == NULL) || (ie_len == 0))
			break;
	}
}

static u8 *rtw_mbo_ie_get(u8 *pie, u32 *plen, u32 limit)
{
	const u8 *p = pie;
	u32 tmp, i;

	if (limit <= 1)
		return NULL;

	i = 0;
	*plen = 0;
	while (1) {
		if ((*p == _VENDOR_SPECIFIC_IE_) &&
			(_rtw_memcmp(rtw_mbo_get_oui(p), wfa_mbo_oui, 4))) {
			*plen = *(p + 1);
			/* RTW_MBO_DUMP("VENDOR_SPECIFIC_IE MBO: ", p, *(p + 1)); */
			return (u8 *)p;
		} else {
			tmp = *(p + 1);
			p += (tmp + 2);
			i += (tmp + 2);
		}

		if (i >= limit)
			break;
	}

	return NULL;
}

static u8 *rtw_mbo_attrs_get(u8 *pie,
	u32 limit, u8 attr_id,u32 *attr_len, u8 dbg)
{
	u8 *p = NULL;
	u32 offset, plen = 0;

	if ((pie == NULL) || (limit <= 1))
		goto exit;

	if ((p = rtw_mbo_ie_get(pie, &plen, limit)) == NULL)
		goto exit;

	/* shift 2 + OUI size and move to attributes content */
	p = p + 2 + sizeof(wfa_mbo_oui);
	plen = plen - 4;

	if (dbg)
		RTW_MBO_DUMP("Attributes contents: ", p, plen);

	if ((p = rtw_get_ie(p, attr_id, attr_len, plen)) == NULL)
		goto exit;

	if (dbg) {
		RTW_MBO_INFO("%s : id=%u(len=%u)\n",
			__func__, attr_id, *attr_len);
		RTW_MBO_DUMP("contents : ", (p + 2), *attr_len);
	}

exit:
	return p;

}

static u32 rtw_mbo_attr_sz_get(
	_adapter *padapter, u8 id)
{
	u32 len = 0;

	switch (id) {
		case RTW_MBO_ATTR_NPREF_CH_RPT_ID:
			{
				struct rf_ctl_t *prfctl = \
					adapter_to_rfctl(padapter);
				struct npref_ch_rtp *prpt = \
					&(prfctl->ch_rtp);
				struct npref_ch* pch;
				u32 i, attr_len, offset;

				for (i=0; i < prpt->nm_of_rpt; i++) {
					pch = &prpt->ch_rpt[i];
					/*attr_len = ch list + op class
						 + preference + reason */
					attr_len = pch->nm_of_ch + 3;
					/* offset = id + len field
						 + attr_len */
					offset = attr_len + 2;
					len += offset;
				}
			}
			break;
		case RTW_MBO_ATTR_ASSOC_RETRY_DELAY_ID:
			len = 4;
			break;
		case RTW_MBO_ATTR_AP_CAP_ID:
		case RTW_MBO_ATTR_CELL_DATA_CAP_ID:
		case RTW_MBO_ATTR_ASSOC_DISABLED_ID:
		case RTW_MBO_ATTR_TRANS_RES_ID:
		case RTW_MBO_ATTR_TRANS_REJ_ID:
			len = 3;
			break;
		default:
			break;
	}

	return len;
}

static void rtw_mbo_build_mbo_ie_hdr(
	u8 **pframe, struct pkt_attrib *pattrib, u8 payload_len)
{
	u8 eid = RTW_MBO_EID;
	u8 len = payload_len + 4;

	*pframe = rtw_mbo_set_1byte_ie(*pframe, &eid, &(pattrib->pktlen));
	*pframe = rtw_mbo_set_1byte_ie(*pframe, &len, &(pattrib->pktlen));
	*pframe = rtw_mbo_set_4byte_ie(*pframe,
			wfa_mbo_oui, &(pattrib->pktlen));
}

void rtw_mbo_build_cell_data_cap_attr(
	_adapter *padapter, u8 **pframe, struct pkt_attrib *pattrib)
{
	struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);
	u8 attr_id = RTW_MBO_ATTR_CELL_DATA_CAP_ID;
	u8 attr_len = 1;
	u8 cell_data_con = rtw_mbo_cell_data_conn;

	/* used Cellular Data Capabilities from supplicant */
	if (!rtw_mbo_wifi_logo_test(padapter) &&
		pmlmepriv->pcell_data_cap_ie &&
		pmlmepriv->cell_data_cap_len == 1) {
		cell_data_con = *pmlmepriv->pcell_data_cap_ie;
	}

	*pframe = rtw_mbo_set_1byte_ie(*pframe, &attr_id, &(pattrib->pktlen));
	*pframe = rtw_mbo_set_1byte_ie(*pframe, &attr_len, &(pattrib->pktlen));
	*pframe = rtw_mbo_set_1byte_ie(*pframe,
			&cell_data_con, &(pattrib->pktlen));
}


static void rtw_mbo_build_ap_cap_Indication_attr(
	_adapter *padapter, u8 **pframe,
	struct pkt_attrib *pattrib, u8 cap_ind)
{
	struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);
	u8 attr_id = RTW_MBO_ATTR_AP_CAP_ID;
	u8 attr_len = 1;
	u8 ap_cap_ind = cap_ind;

	*pframe = rtw_mbo_set_1byte_ie(*pframe, &attr_id, &(pattrib->pktlen));
	*pframe = rtw_mbo_set_1byte_ie(*pframe, &attr_len, &(pattrib->pktlen));
	*pframe = rtw_mbo_set_1byte_ie(*pframe,
			&ap_cap_ind, &(pattrib->pktlen));
}

static void rtw_mbo_build_ap_disallowed_attr(
	_adapter *padapter, u8 **pframe,
	struct pkt_attrib *pattrib, u8 code)
{
	struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);
	u8 attr_id = RTW_MBO_ATTR_ASSOC_DISABLED_ID;
	u8 attr_len = 1;
	u8 reason = 0;

	if (code > 0) {
		reason = code;
		*pframe = rtw_mbo_set_1byte_ie(*pframe,
				&attr_id, &(pattrib->pktlen));
		*pframe = rtw_mbo_set_1byte_ie(*pframe,
				&attr_len, &(pattrib->pktlen));
		*pframe = rtw_mbo_set_1byte_ie(*pframe,
				&reason, &(pattrib->pktlen));
	}
}

static void rtw_mbo_build_ap_trans_reason_attr(
	_adapter *padapter, u8 **pframe,
	struct pkt_attrib *pattrib, u8 code)
{
	struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);
	u8 attr_id = RTW_MBO_ATTR_TRANS_RES_ID;
	u8 attr_len = 1;
	u8 reason = 0;

	reason = code;
		*pframe = rtw_mbo_set_1byte_ie(*pframe,
				&attr_id, &(pattrib->pktlen));
		*pframe = rtw_mbo_set_1byte_ie(*pframe,
				&attr_len, &(pattrib->pktlen));
		*pframe = rtw_mbo_set_1byte_ie(*pframe,
				&reason, &(pattrib->pktlen));
	}

static void rtw_mbo_build_ap_assoc_retry_delay_attr(
	_adapter *padapter, u8 **pframe,
	struct pkt_attrib *pattrib, u16 code)
{
	struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);
	u8 attr_id = RTW_MBO_ATTR_ASSOC_RETRY_DELAY_ID;
	u8 attr_len = 2;
	u16 delay = 0;

	delay = code;
	*pframe = rtw_mbo_set_1byte_ie(*pframe, &attr_id, &(pattrib->pktlen));
	*pframe = rtw_mbo_set_1byte_ie(*pframe, &attr_len, &(pattrib->pktlen));
	*pframe = rtw_mbo_set_2byte_ie(*pframe,
			(u8 *)&delay, &(pattrib->pktlen));
}

static void rtw_mbo_update_cell_data_cap(
	_adapter *padapter, u8 *pie, u32 ie_len)
{
	struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);
	u8 *mbo_attr;
	u32	mbo_attrlen;

	if ((pie == NULL) || (ie_len == 0))
		return;

	mbo_attr = rtw_mbo_attrs_get(pie, ie_len,
		RTW_MBO_ATTR_CELL_DATA_CAP_ID, &mbo_attrlen, 0);

	if ((mbo_attr == NULL) || (mbo_attrlen == 0) ) {
		RTW_INFO("MBO : Cellular Data Capabilities not found!\n");
		return;
	}

	rtw_buf_update(&pmlmepriv->pcell_data_cap_ie,
		&pmlmepriv->cell_data_cap_len, (mbo_attr + 2), mbo_attrlen);
	RTW_MBO_DUMP("rtw_mbo_update_cell_data_cap : ",
		pmlmepriv->pcell_data_cap_ie, pmlmepriv->cell_data_cap_len);
}

void rtw_mbo_update_ie_data(
	_adapter *padapter, u8 *pie, u32 ie_len)
{
	rtw_mbo_update_cell_data_cap(padapter, pie, ie_len);
}

static u8 rtw_mbo_current_op_class_get(_adapter *padapter)
{
	struct rf_ctl_t *prfctl = adapter_to_rfctl(padapter);
	struct p2p_channels *pch_list =  &(prfctl->channel_list);
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);
	struct link_mlme_ext_priv *pmlmeext = &(padapter_link->mlmeextpriv);
	struct p2p_reg_class *preg_class;
	int class_idx, ch_idx;
	u8 cur_op_class = 0;

	for(class_idx =0; class_idx < pch_list->reg_classes; class_idx++) {
		preg_class =  &pch_list->reg_class[class_idx];
		for (ch_idx = 0; ch_idx <= preg_class->channels; ch_idx++) {
			if (pmlmeext->chandef.chan ==  \
				preg_class->channel[ch_idx]) {
				cur_op_class = preg_class->reg_class;
				RTW_MBO_INFO("%s : current ch : %d,"
					" op class : %d\n",
					__func__, pmlmeext->chandef.chan,
					cur_op_class);
				break;
			}
		}
	}

	return cur_op_class;
}

static void rtw_mbo_supp_op_classes_get(_adapter *padapter, u8 *pclasses)
{
	struct rf_ctl_t *prfctl = adapter_to_rfctl(padapter);
	struct p2p_channels *pch_list =  &(prfctl->channel_list);
	int class_idx;

	if (pclasses == NULL)
		return;

	RTW_MBO_INFO("%s : support op class \n", __func__);
	for(class_idx = 0; class_idx < pch_list->reg_classes; class_idx++) {
		*(pclasses + class_idx) = \
			pch_list->reg_class[class_idx].reg_class;
		RTW_MBO_INFO("%u ,", *(pclasses + class_idx));
	}

	RTW_MBO_INFO("%s : \n", __func__);
}

void rtw_mbo_build_supp_op_class_elem(
	_adapter *padapter, u8 **pframe, struct pkt_attrib *pattrib)
{
	struct rf_ctl_t *prfctl = adapter_to_rfctl(padapter);
	u8 payload[32] = {0};
	u8 delimiter_130 = 130;	/*0x82*/
	u8 reg_class_nm, len;

	if ((reg_class_nm = prfctl->channel_list.reg_classes) == 0)
		return;

	payload[0] = rtw_mbo_current_op_class_get(padapter);
	rtw_mbo_supp_op_classes_get(padapter, &payload[1]);

	/* IEEE 802.11 Std Current Operating Class Extension Sequence */
	payload[reg_class_nm + 1] = delimiter_130;
	payload[reg_class_nm + 2] = 0x00;

	RTW_MBO_DUMP("op class :", payload, reg_class_nm);

	/* Current Operating Class field + Operating Class field
		+ OneHundredAndThirty Delimiter field */
	len = reg_class_nm + 3;
	*pframe = rtw_set_ie(*pframe, EID_SupRegulatory, len ,
					payload, &(pattrib->pktlen));
}

static u8 rtw_mbo_construct_npref_ch_rpt_attr(
	_adapter *padapter, u8 *pbuf, u32 buf_len, u32 *plen)
{
	struct rf_ctl_t *prfctl = adapter_to_rfctl(padapter);
	struct npref_ch_rtp *prpt = &(prfctl->ch_rtp);
	struct npref_ch* pch;
	u32 attr_len, offset;
	int i;
	u8 *p = pbuf;

	if (prpt->nm_of_rpt == 0) {
		*plen = 0;
		return _FALSE;
	}

	for (i=0; i < prpt->nm_of_rpt; i++) {
		pch = &prpt->ch_rpt[i];
		/* attr_len = ch list + op class + preference + reason */
		attr_len = pch->nm_of_ch + 3;
		/* offset = id + len field + attr_len */
		offset = attr_len + 2;
		rtw_mbo_subfield_set(p, 0, RTW_MBO_ATTR_NPREF_CH_RPT_ID);
		rtw_mbo_subfield_set(p, 1, attr_len);
		rtw_mbo_subfield_set(p, 2, pch->op_class);
		rtw_mbo_subfields_set(p, 3, pch->chs, pch->nm_of_ch);
		rtw_mbo_subfield_set(p, (offset - 2), pch->preference);
		rtw_mbo_subfield_set(p, (offset - 1), pch->reason);
		p +=  offset;
		*plen += offset;

		if (*plen >=  buf_len) {
			RTW_ERR("MBO : construct non-preferred-ch rpt fail!\n");
			return _FALSE;
		}
	}

	return _TRUE;
}

void rtw_mbo_build_npref_ch_rpt_attr(
	_adapter *padapter, u8 **pframe, struct pkt_attrib *pattrib)
{
	struct rf_ctl_t *prfctl = adapter_to_rfctl(padapter);
	struct npref_ch_rtp *prpt = &(prfctl->ch_rtp);
	u32 tmp_sz = 0, body_len = 0;
	u8 *ptmp;

	tmp_sz = prpt->nm_of_rpt * sizeof(struct npref_ch);
	ptmp = rtw_zmalloc(tmp_sz);
	if (ptmp == NULL)
		return;

	if (rtw_mbo_construct_npref_ch_rpt_attr(
		padapter, ptmp, tmp_sz, &body_len) == _FALSE) {
		rtw_mfree(ptmp, tmp_sz);
		return;
	}

	RTW_MBO_DUMP("Non-preferred Channel Report :", ptmp, body_len);
	*pframe = rtw_mbo_set_nbyte_ie(*pframe, body_len,
			ptmp, &(pattrib->pktlen));

	rtw_mfree(ptmp, tmp_sz);
}

void rtw_mbo_build_trans_reject_reason_attr(
	_adapter *padapter, u8 **pframe,
	struct pkt_attrib *pattrib, u8 *pres)
{
	u8 attr_id = RTW_MBO_ATTR_TRANS_REJ_ID;
	u8 attr_len = 1;
	u32 len = 0;

	len = rtw_mbo_attr_sz_get(padapter, RTW_MBO_ATTR_TRANS_REJ_ID);
	if ((len == 0) || (len > 3)) {
		RTW_ERR("MBO : build Transition Rejection Reason"
			" attribute fail(len=%u)\n", len);
		return;
	}

	rtw_mbo_build_mbo_ie_hdr(pframe, pattrib, len);
	*pframe = rtw_mbo_set_1byte_ie(*pframe, &attr_id, &(pattrib->pktlen));
	*pframe = rtw_mbo_set_1byte_ie(*pframe, &attr_len, &(pattrib->pktlen));
	*pframe = rtw_mbo_set_1byte_ie(*pframe, pres, &(pattrib->pktlen));
}

u8 rtw_mbo_disallowed_network(struct wlan_network *pnetwork)
{
	u8 *p, *attr_id, *res;
	u32 attr_len = 0;
	u8 disallow = _FALSE;

	if ((pnetwork == NULL) || \
		((p = rtw_mbo_attrs_get(
			pnetwork->network.IEs,
		pnetwork->network.IELength,
		RTW_MBO_ATTR_ASSOC_DISABLED_ID,
			&attr_len, 0)) == NULL)) {
		goto exit;
	}

	RTW_MBO_DUMP("Association Disallowed attribute :",p , attr_len + 2);
	RTW_INFO("MBO : block "MAC_FMT" assoc disallowed reason %d\n",
		MAC_ARG(pnetwork->network.MacAddress),
		*(rtw_mbo_get_disallow_res(p)));

	disallow = _TRUE;
exit:
	return disallow;
}

void rtw_mbo_build_extended_cap(
	_adapter *padapter, u8 **pframe, struct pkt_attrib *pattrib)
{
	struct _ADAPTER_LINK *padapter_link = pattrib->adapter_link;
	struct link_mlme_priv *pmlmepriv = &(padapter_link->mlmepriv);

	if (!rtw_mbo_wifi_logo_test(padapter))
		return;

	rtw_wnm_add_btm_ext_cap(pmlmepriv->ext_capab_ie_data,
				&(pmlmepriv->ext_capab_ie_len));
	rtw_mbo_add_internw_ext_cap(pmlmepriv->ext_capab_ie_data,
				&(pmlmepriv->ext_capab_ie_len));
	*pframe = rtw_set_ie(*pframe,
				WLAN_EID_EXT_CAP,
				pmlmepriv->ext_capab_ie_len,
				pmlmepriv->ext_capab_ie_data,
				&(pattrib->pktlen));
}

static void rtw_mbo_non_pref_chans_dump(struct npref_ch* pch)
{
	int i;
	u8 buf[128] = {0};

	for (i=0; i < pch->nm_of_ch; i++)
		rtw_sprintf(buf, 128, "%s,%d", buf, pch->chs[i]);

	RTW_MBO_INFO("%s : op_class=%01x, ch=%s, preference=%d, reason=%d\n",
		__func__, pch->op_class, buf, pch->preference, pch->reason);
}

static u8 rtw_mbo_non_pref_chan_exist(struct npref_ch* pch, u8 ch)
{
	u32 i;
	u8 found = _FALSE;

	for (i=0; i < pch->nm_of_ch; i++) {
		if (pch->chs[i] == ch) {
			found = _TRUE;
			break;
		}
	}

	return found;
}

static struct npref_ch* rtw_mbo_non_pref_chan_get(
	_adapter *padapter, u8 op_class, u8  prefe, u8  res)
{
	struct rf_ctl_t *prfctl = adapter_to_rfctl(padapter);
	struct npref_ch_rtp *prpt = &(prfctl->ch_rtp);
	struct npref_ch* pch = NULL;
	int i;

	if (prpt->nm_of_rpt == 0)
		return pch;

	for (i=0; i < prpt->nm_of_rpt; i++) {
		if ((prpt->ch_rpt[i].op_class == op_class) &&
			(prpt->ch_rpt[i].preference == prefe) &&
			(prpt->ch_rpt[i].reason == res)) {
			pch = &prpt->ch_rpt[i];
			break;
		}
	}

	return pch;
}

static void rtw_mbo_non_pref_chan_set(
	struct npref_ch* pch, u8 op_class,
	u8 ch, u8  prefe, u8  res, u8 update)
{
	u32 offset = pch->nm_of_ch;

	if (update) {
		if (rtw_mbo_non_pref_chan_exist(pch, ch) == _FALSE) {
			pch->chs[offset] = ch;
			pch->nm_of_ch++;
		}
	} else {
		pch->op_class = op_class;
		pch->chs[0] = ch;
		pch->preference = prefe;
		pch->reason = res;
		pch->nm_of_ch = 1;
	}
}

static void  rtw_mbo_non_pref_chans_update(
	_adapter *padapter, u8 op_class, u8 ch, u8  prefe, u8  res)
{
	struct rf_ctl_t *prfctl = adapter_to_rfctl(padapter);
	struct npref_ch_rtp *pch_rpt = &(prfctl->ch_rtp);
	struct npref_ch* pch;

	if (pch_rpt->nm_of_rpt >= RTW_MBO_MAX_CH_RPT_NUM) {
		RTW_ERR("MBO : %d non_pref_chan entries supported!",
			RTW_MBO_MAX_CH_RPT_NUM);
		return;
	}

	if (pch_rpt->nm_of_rpt == 0) {
		pch = &pch_rpt->ch_rpt[0];
		rtw_mbo_non_pref_chan_set(pch, op_class,
				ch, prefe, res, _FALSE);
		pch_rpt->nm_of_rpt = 1;
		return;
	}

	pch = rtw_mbo_non_pref_chan_get(padapter, op_class, prefe, res);
	if (pch == NULL) {
		pch = &pch_rpt->ch_rpt[pch_rpt->nm_of_rpt];
		rtw_mbo_non_pref_chan_set(pch, op_class,
				ch, prefe, res, _FALSE);
		pch_rpt->nm_of_rpt++;
	} else {
		rtw_mbo_non_pref_chan_set(pch, op_class,
				ch, prefe, res, _TRUE);
	}

	rtw_mbo_non_pref_chans_dump(pch);
}

static void  rtw_mbo_non_pref_chans_set(
	_adapter *padapter, char *param, ssize_t sz)
{
	char *pnext;
	u32 op_class, ch, prefe, res;
	int i = 0;

	do {
		pnext = strsep(&param, " ");
		if (pnext == NULL)
			break;

		sscanf(pnext, "%d:%d:%d:%d", &op_class, &ch, &prefe, &res);
		rtw_mbo_non_pref_chans_update(padapter, op_class,
					ch, prefe, res);

		if ((i++) > 10) {
			RTW_ERR("MBO : overflow %d \n", i);
			break;
		}

	} while (param != (char*)'\0');

}

static void  rtw_mbo_non_pref_chans_del(
	_adapter *padapter, char *param, ssize_t sz)
{
	struct rf_ctl_t *prfctl = adapter_to_rfctl(padapter);
	struct npref_ch_rtp *prpt = &(prfctl->ch_rtp);

	RTW_INFO("%s : delete non_pref_chan %s\n", __func__, param);
	_rtw_memset(prpt, 0, sizeof(struct npref_ch_rtp));
}

ssize_t rtw_mbo_proc_non_pref_chans_set(
	struct file *pfile, const char __user *buffer,
	size_t count, loff_t *pos, void *pdata)
{
	struct net_device *dev = pdata;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	u8 tmp[128] = {0};

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		if (strncmp(tmp, "add", 3) == 0) {
			rtw_mbo_non_pref_chans_set(padapter,
					&tmp[4], (count - 4));
		} else if (strncmp(tmp, "delete", 6) == 0) {
			rtw_mbo_non_pref_chans_del(padapter,
					&tmp[7], (count - 7));
		} else {
			RTW_ERR("MBO : Invalid format : echo [add|delete]"
			" <oper_class>:<chan>:<preference>:<reason>\n");
			return -EFAULT;
		}
	}

#ifdef CONFIG_RTW_WNM
	if (check_fwstate(pmlmepriv, WIFI_ASOC_STATE) &&
		check_fwstate(pmlmepriv, WIFI_STATION_STATE))
		rtw_wnm_issue_action(padapter,
				RTW_WLAN_ACTION_WNM_NOTIF_REQ, 0, 0);
#endif

	return count;
}

int rtw_mbo_proc_non_pref_chans_get(
	struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct rf_ctl_t *prfctl = adapter_to_rfctl(padapter);
	struct npref_ch_rtp *prpt = &(prfctl->ch_rtp);
	struct npref_ch* pch;
	int i,j;
	u8 buf[32] = {0};

	RTW_PRINT_SEL(m, "op_class                     ch    preference    reason \n");
	RTW_PRINT_SEL(m, "=======================================================\n");

	if (prpt->nm_of_rpt == 0) {
		RTW_PRINT_SEL(m, " empty table \n");
		return 0;
	}

	for (i=0; i < prpt->nm_of_rpt; i++) {
		pch = &prpt->ch_rpt[i];
		buf[0]='\0';
		for (j=0; j < pch->nm_of_ch; j++) {
			if (j == 0)
				rtw_sprintf(buf, 32, "%02u", pch->chs[j]);
			else
				rtw_sprintf(buf, 32, "%s,%02u", buf, pch->chs[j]);
		}

		RTW_PRINT_SEL(m, "    %04u    %20s           %02u        %02u\n",
			pch->op_class, buf, pch->preference, pch->reason);
	}

	return 0;
}

ssize_t rtw_mbo_proc_cell_data_set(
	struct file *pfile, const char __user *buffer,
	size_t count, loff_t *pos, void *pdata)
{
	struct net_device *dev = pdata;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	int mbo_cell_data = 0;
	u8 tmp[8] = {0};

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp))
		return -EFAULT;

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		int num = sscanf(tmp, "%d", &mbo_cell_data);
		if (num == 1) {
			rtw_mbo_cell_data_conn = mbo_cell_data;
		#ifdef CONFIG_RTW_WNM
			if (check_fwstate(pmlmepriv, WIFI_ASOC_STATE) &&
				check_fwstate(pmlmepriv, WIFI_STATION_STATE)) {
				rtw_wnm_issue_action(padapter,
					RTW_WLAN_ACTION_WNM_NOTIF_REQ, 0, 0);
			}
		#endif
		}
	}

	return count;
}

int rtw_mbo_proc_cell_data_get(
	struct seq_file *m, void *v)
{
#if 0
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
#endif

	RTW_PRINT_SEL(m, "Cellular Data Connectivity : %d\n",
			rtw_mbo_cell_data_conn);
	return 0;
}


static void rtw_mbo_disassoc(_adapter *padapter, u8 *da,
			u8 reason, u8 wait_ack)
{
	struct xmit_frame *pmgntframe;
	struct pkt_attrib *pattrib;
	struct rtw_ieee80211_hdr *pwlanhdr;
	struct xmit_priv *pxmitpriv = &(padapter->xmitpriv);
	struct mlme_ext_priv *pmlmeext = &(padapter->mlmeextpriv);
	u8 *pframe;
	u16 *fctrl;
	int ret = _FAIL;
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);
	struct link_mlme_ext_info *pmlmeinfo = &(padapter_link->mlmeextpriv.mlmext_info);

	if (alink_is_tx_blocked_by_ch_waiting(padapter_link))
		return;

	pmgntframe = alloc_mgtxmitframe(pxmitpriv);
	if (pmgntframe == NULL)
		return;

	/* update attribute */
	pattrib = &pmgntframe->attrib;
	update_mgntframe_attrib(padapter, padapter_link, pattrib);
	pattrib->retry_ctrl = _FALSE;
        pattrib->key_type = IEEE80211W_RIGHT_KEY;

	_rtw_memset(pmgntframe->buf_addr, 0, WLANHDR_OFFSET + TXDESC_OFFSET);
	pframe = (u8 *)(pmgntframe->buf_addr) + TXDESC_OFFSET;
	pwlanhdr = (struct rtw_ieee80211_hdr *)pframe;
	fctrl = &(pwlanhdr->frame_ctl);
	*(fctrl) = 0;

	_rtw_memcpy(pwlanhdr->addr1, da, ETH_ALEN);
	_rtw_memcpy(pwlanhdr->addr2, adapter_mac_addr(padapter), ETH_ALEN);
	_rtw_memcpy(pwlanhdr->addr3, get_my_bssid(&(pmlmeinfo->network)), ETH_ALEN);

	SetSeqNum(pwlanhdr, pmlmeext->mgnt_seq);
	pmlmeext->mgnt_seq++;
	set_frame_sub_type(pframe, WIFI_DISASSOC);

	pframe += sizeof(struct rtw_ieee80211_hdr_3addr);
	pattrib->pktlen = sizeof(struct rtw_ieee80211_hdr_3addr);

	reason = cpu_to_le16(reason);
	pframe = rtw_set_fixed_ie(pframe, _RSON_CODE_ ,
			(unsigned char *)&reason, &(pattrib->pktlen));

	pattrib->last_txcmdsz = pattrib->pktlen;
	if (wait_ack)
		dump_mgntframe_and_wait_ack(padapter, pmgntframe);
	else
		dump_mgntframe(padapter, pmgntframe);
	RTW_MBO_INFO("%s : reason %u\n", __func__, reason);
}

static void rtw_mbo_construct_user_btm_req(
	_adapter *padapter, struct btm_req_hdr *phdr,
	u8 *purl, u32 url_len, struct wnm_btm_cant *pbtm_cant)
{
	struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);
	struct mbo_attr_info *pmbo_attr = &(pmlmepriv->mbo_attr);
	struct mbo_user_btm_req_pkt *puser = &(pmbo_attr->user_raw);
	struct wnm_btm_cant *puser_cant = NULL;

	if (phdr) {
		puser->hdr.req_mode = phdr->req_mode;
		puser->hdr.disassoc_timer = phdr->disassoc_timer;
		puser->hdr.validity_interval = phdr->validity_interval;
		puser->hdr.term_duration.tsf = phdr->term_duration.tsf;
		puser->hdr.term_duration.duration = \
			phdr->term_duration.duration;
		puser->hdr.term_duration.id = 0x4;
		puser->hdr.term_duration.len = 0xa;
		RTW_MBO_INFO("%s : req-mode=0x%x, disassoc_timer=%u, "
			"validity_interval=%u, tsf=%llu, "
			"duration=%u\n", __func__,
			puser->hdr.req_mode, puser->hdr.disassoc_timer,
			puser->hdr.validity_interval,
			puser->hdr.term_duration.tsf,
			puser->hdr.term_duration.duration);
	}

	if (purl && url_len) {
		/* TODO */
	}

	if (pbtm_cant) {
		struct wnm_btm_cant *pbtm_tb = NULL;
		u8 i, idx, found = _FALSE;

		for (i = 0; i < RTW_MAX_NB_RPT_NUM; i++) {
			puser_cant = &puser->btm_cants[i];
			if (_rtw_memcmp(pbtm_cant->nb_rpt.bssid,
				puser_cant->nb_rpt.bssid, ETH_ALEN)) {
				puser_cant->nb_rpt.bss_info = \
					pbtm_cant->nb_rpt.bss_info;
				puser_cant->nb_rpt.reg_class = \
					pbtm_cant->nb_rpt.reg_class;
				puser_cant->nb_rpt.ch_num = \
					pbtm_cant->nb_rpt.ch_num;
				puser_cant->nb_rpt.phy_type = \
					pbtm_cant->nb_rpt.phy_type;
				puser_cant->preference = \
					pbtm_cant->preference;
				idx = i;
				found = _TRUE;
				break;
			}
		}

		if (!found) {
			if (puser->candidate_cnt >= RTW_MAX_NB_RPT_NUM)
				puser->candidate_cnt = 0;
			puser_cant = &puser->btm_cants[puser->candidate_cnt];
			puser_cant->nb_rpt.id = \
				RTW_WLAN_ACTION_WNM_NB_RPT_ELEM;
			puser_cant->nb_rpt.len = 0x10;
			_rtw_memcpy(puser_cant->nb_rpt.bssid,
				pbtm_cant->nb_rpt.bssid, ETH_ALEN);
			puser_cant->nb_rpt.bss_info = \
				pbtm_cant->nb_rpt.bss_info;
			puser_cant->nb_rpt.reg_class = \
				pbtm_cant->nb_rpt.reg_class;
			puser_cant->nb_rpt.ch_num = \
				pbtm_cant->nb_rpt.ch_num;
			puser_cant->nb_rpt.phy_type = \
				pbtm_cant->nb_rpt.phy_type;
			puser_cant->preference = \
				pbtm_cant->preference;
			idx = puser->candidate_cnt;
			puser->candidate_cnt++;
		}

		RTW_MBO_INFO("%s:%s idx=%u, bssid("MAC_FMT"),"
			" bss_info(0x%04X), reg_class(0x%02X),"
			" ch(%d), phy_type(0x%02X), preference(0x%02X)\n",
			__func__, (found)?"update":"new", idx,
			MAC_ARG(puser_cant->nb_rpt.bssid),
			puser_cant->nb_rpt.bss_info,
			puser_cant->nb_rpt.reg_class,
			puser_cant->nb_rpt.ch_num,
			puser_cant->nb_rpt.phy_type,
			puser_cant->preference);

	} /* end of if (pbtm_cant) */
}

static void rtw_mbo_reset_user_btm_req_preference(_adapter *padapter)
{
	struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);
	struct mbo_attr_info *pmbo_attr = &(pmlmepriv->mbo_attr);
	struct mbo_user_btm_req_pkt *puser = &(pmbo_attr->user_raw);
	struct wnm_btm_cant *puser_cant = NULL;
	u8 i;

	for (i = 0; i < RTW_MAX_NB_RPT_NUM; i++) {
		puser_cant = &puser->btm_cants[i];
		if (_rtw_memcmp(adapter_mac_addr(padapter),
			puser_cant->nb_rpt.bssid, ETH_ALEN)) {
			puser_cant->preference = 0;
			RTW_MBO_INFO("%s : reset "MAC_FMT" BTM preference\n",
				__func__, MAC_ARG(puser_cant->nb_rpt.bssid));
			break;
		}
	}

}

ssize_t rtw_mbo_proc_attr_set(
	struct file *pfile, const char __user *buffer,
	size_t count, loff_t *pos, void *pdata)
{
	struct net_device *dev = pdata;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);
	struct mbo_attr_info *pmbo_attr = &(pmlmepriv->mbo_attr);
	struct mbo_user_btm_req_pkt *puser = &(pmbo_attr->user_raw);
	u32 id, val;
	u8 tmp[64] = {0};

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp))
		return -EFAULT;

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		int num = sscanf(tmp, "%d %d", &id, &val);
		switch (id) {
			case RTW_MBO_ATTR_AP_CAP_ID:
				pmbo_attr->ap_cap_ind = val;
				break;
			case RTW_MBO_ATTR_CELL_DATA_CAP_ID:
				pmbo_attr->cell_data_cap = val;
				break;
			case RTW_MBO_ATTR_ASSOC_DISABLED_ID:
				pmbo_attr->assoc_disallow = val;
				break;
			case RTW_MBO_ATTR_TRANS_RES_ID:
				pmbo_attr->reason = val;
				break;
			case RTW_MBO_ATTR_ASSOC_RETRY_DELAY_ID:
				pmbo_attr->delay = val;
				break;
			case RTW_MBO_TEST_CMD_REST:
				RTW_INFO("%s : RTW_MBO_TEST_CMD_REST\n",
					__func__);
				_rtw_memset(pmbo_attr, 0,
					sizeof(struct mbo_attr_info));
				pmbo_attr->mbo_spec_test = 1;
				break;
			case RTW_MBO_TEST_CMD_BTM_REQ_SET:
				if (count >= 10) {
					struct btm_req_hdr btm_hdr;
					u32 disassoc_imnt, term_bit;
					u32 term_tsf, term_duration;

					_rtw_memset(&btm_hdr, 0,
							sizeof(btm_hdr));

					num = sscanf(tmp, "%d %u %u %u %u",
						&id, &disassoc_imnt,
						&term_bit, &term_tsf,
						&term_duration);

					if (num < 5)
						break;

					if (disassoc_imnt > 0)
						btm_hdr.req_mode |= \
							DISASSOC_IMMINENT;

					if (term_bit > 0)
						btm_hdr.req_mode |= \
						BSS_TERMINATION_INCLUDED;

					btm_hdr.term_duration.tsf = term_tsf;
					btm_hdr.term_duration.duration = \
							term_duration;

					rtw_mbo_construct_user_btm_req(
						padapter, &btm_hdr, NULL, 0,
						NULL);
				}
				break;
			case RTW_MBO_TEST_CMD_BTM_REQ_SEND:
				if (count >= 12) {
					u8 mac_str[18] = {0};
					u8 dst_mac[ETH_ALEN] = {0};
					u32 cand_list = 0, disassoc_timer = 0;

					num = sscanf(tmp, "%d %s %u %u",
						&id, mac_str, &cand_list,
						&disassoc_timer);

					if (num < 4)
						break;

					if (sscanf(mac_str, MAC_SFMT,
						MAC_SARG(
						dst_mac)) != 6) {
						break;
					}

					puser->append_mbo_ie = _TRUE;
					puser->hdr.dialog_token++;
					puser->hdr.validity_interval = 0xf;
					if (cand_list > 0)
						puser->hdr.req_mode |= \
						PREFERRED_CANDIDATE_LIST_INCLUDED;


					puser->hdr.disassoc_timer = \
						disassoc_timer;

					if ((puser->hdr.req_mode & \
						DISASSOC_IMMINENT) == \
						DISASSOC_IMMINENT) {
						rtw_mbo_reset_user_btm_req_preference(padapter);
						if (pmbo_attr->delay == 0)
							pmbo_attr->delay = 1;
						if (puser->hdr.disassoc_timer == 0)
							puser->hdr.disassoc_timer = 1000;
					}

					if ((puser->hdr.req_mode & \
						BSS_TERMINATION_INCLUDED) == \
						BSS_TERMINATION_INCLUDED) {
						 puser->append_mbo_ie = _FALSE;
					}

					if (!puser->candidate_cnt) {
						struct wnm_btm_cant cant;
						_rtw_memset(&cant, 0,
								sizeof(cant));
						_rtw_memcpy(cant.nb_rpt.bssid,
							adapter_mac_addr(padapter),
							ETH_ALEN);
						cant.nb_rpt.reg_class = 115;
						cant.nb_rpt.ch_num = 36;
						cant.preference = 0;
						rtw_mbo_construct_user_btm_req(
							padapter, NULL,
							NULL, 0, &cant);
					}

					rtw_wnm_issue_btm_req(padapter, dst_mac,
						&puser->hdr, NULL, 0,
						(u8 *)&puser->btm_cants,
						puser->candidate_cnt);

					if ((puser->hdr.term_duration.duration)
						&& (puser->hdr.req_mode & \
						BSS_TERMINATION_INCLUDED) == \
						BSS_TERMINATION_INCLUDED) {
						rtw_mbo_disassoc(padapter,
							dst_mac, 3, _TRUE);
					}

					if ((puser->hdr.req_mode & \
						DISASSOC_IMMINENT) == \
						DISASSOC_IMMINENT) {
						rtw_mbo_disassoc(padapter,
							dst_mac, 3, _TRUE);
					}
				} /* end of if (count >= 12) */

				break;
			case RTW_MBO_TEST_CMD_NB_BSS_ADD:
				if (count >= 12) {
					u8 bss_str[18];
					struct wnm_btm_cant cant;
					u32 op, op_ch, perf;

					num = sscanf(tmp, "%d %s %u %u %u",
						&id, bss_str, &op, &op_ch,
						&perf);
					if (num < 5)
						break;
					_rtw_memset(&cant, 0, sizeof(cant));
					if (sscanf(bss_str, MAC_SFMT,
						MAC_SARG(
						cant.nb_rpt.bssid)) != 6) {
						break;
					}

					cant.nb_rpt.reg_class = op;
					cant.nb_rpt.ch_num = op_ch;
					cant.preference = perf;
					rtw_mbo_construct_user_btm_req(
						padapter, NULL, NULL, 0,
						&cant);
				} /* end of if (count >= 12) */
				break;
			default:
				break;
		}
	}

	return count;
}

int rtw_mbo_proc_attr_get(
	struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);
	struct mbo_attr_info *pmbo_attr = &(pmlmepriv->mbo_attr);

	RTW_PRINT_SEL(m, "AP Capability Indication : %d\n",
				pmbo_attr->ap_cap_ind);
	RTW_PRINT_SEL(m, "Cellular Data Capabilities : %d\n",
				pmbo_attr->cell_data_cap);
	RTW_PRINT_SEL(m, "Association Disallowed : %d\n",
				pmbo_attr->assoc_disallow);
	return 0;
}

static void rtw_mbo_non_pref_chan_subelem_parsing(
	_adapter *padapter, u8 *subelem, size_t subelem_len)
{
	u8 *pnon_pref_chans;
	u32 non_pref_chan_offset, op_subelem_len;
	u32 oui_offset = 3;
	/* wpa_supplicant don't apped OUI Type */
	u32 oui_type_offset = 0;

	RTW_MBO_DUMP("Non-preferred Channel subelem : ",
			subelem , subelem_len);

	/* Subelem :
		Vendor Specific | Length | WFA OUI | OUI Type | MBO Attributes */
	non_pref_chan_offset = 2 + oui_offset + oui_type_offset;
	pnon_pref_chans = subelem + non_pref_chan_offset;
	op_subelem_len = subelem_len - non_pref_chan_offset;

	/* wpa_supplicant don't indicate non_pref_chan length,
		so we cannot get how many non_pref_chan in a wnm notification */
	RTW_MBO_DUMP("Non-preferred Channel : ",
			pnon_pref_chans, op_subelem_len);
}

void rtw_mbo_wnm_notification_parsing(
	_adapter *padapter, const u8 *pdata, size_t data_len)
{
	u8 *paction;
	u8 category, action, dialog, type;
	u32 len;

	if ((pdata == NULL) || (data_len == 0))
		return;

	RTW_MBO_DUMP("WNM notification data : ", pdata, data_len);
	paction = (u8 *)pdata + sizeof(struct rtw_ieee80211_hdr_3addr);
	category = paction[0];
	action = paction[1];
	dialog = paction[2];
	type = paction[3];

	if ((action == RTW_WLAN_ACTION_WNM_NOTIF_REQ) &&
		(type == WLAN_EID_VENDOR_SPECIFIC)) {
		rtw_mbo_non_pref_chan_subelem_parsing(padapter, &paction[4],
			(data_len - sizeof(struct rtw_ieee80211_hdr_3addr)));
	}
}

void rtw_mbo_build_wnm_notification(
	_adapter *padapter, u8 **pframe, struct pkt_attrib *pattrib)
{
	struct rf_ctl_t *prfctl = adapter_to_rfctl(padapter);
	struct npref_ch_rtp *prpt = &(prfctl->ch_rtp);
	struct npref_ch* pch;
	u8 subelem_id = WLAN_EID_VENDOR_SPECIFIC;
	u8 non_pref_ch_oui[] = {0x50, 0x6F, 0x9A, 0x2};
	u8 cell_data_cap_oui[] = {0x50, 0x6F, 0x9A, 0x3};
	u8 cell_data_con = rtw_mbo_cell_data_conn;
	u8 len, cell_data_con_len = 0, *pcont = *pframe;
	int i;

	if (rtw_mbo_cell_data_conn > 0) {
		len = 0x5;
		*pframe = rtw_mbo_set_1byte_ie(*pframe,
				&subelem_id, &(pattrib->pktlen));
		*pframe = rtw_mbo_set_1byte_ie(*pframe,
				&len, &(pattrib->pktlen));
		*pframe = rtw_mbo_set_4byte_ie(*pframe,
				cell_data_cap_oui, &(pattrib->pktlen));
		*pframe = rtw_mbo_set_1byte_ie(*pframe,
				&cell_data_con, &(pattrib->pktlen));
		RTW_MBO_INFO("%s : Cellular Data Capabilities subelemen\n",
				__func__);
		RTW_MBO_DUMP(":", pcont, len + 2);
		pcont += len + 2 ;
	}

	if (prpt->nm_of_rpt == 0) {
		len = 0x4;
		*pframe = rtw_mbo_set_1byte_ie(*pframe,
				&subelem_id, &(pattrib->pktlen));
		*pframe = rtw_mbo_set_1byte_ie(*pframe,
				&len, &(pattrib->pktlen));
		*pframe = rtw_mbo_set_4byte_ie(*pframe,
				non_pref_ch_oui, &(pattrib->pktlen));
		RTW_MBO_INFO("%s :Non-preferred Channel Report subelement"
				" without data\n", __func__);
		return;
	}

	for (i=0; i < prpt->nm_of_rpt; i++) {
		pch = &prpt->ch_rpt[i];
		/* OUI(3B)  + OUT-type(1B) + op-class(1B) + ch list(nB)
			+ Preference(1B) + reason(1B) */
		len = pch->nm_of_ch + 7;
		*pframe = rtw_mbo_set_1byte_ie(*pframe,
				&subelem_id, &(pattrib->pktlen));
		*pframe = rtw_mbo_set_1byte_ie(*pframe,
				&len, &(pattrib->pktlen));
		*pframe = rtw_mbo_set_4byte_ie(*pframe,
				non_pref_ch_oui, &(pattrib->pktlen));
		*pframe = rtw_mbo_set_1byte_ie(*pframe,
				&pch->op_class, &(pattrib->pktlen));
		*pframe = rtw_mbo_set_nbyte_ie(*pframe,
				pch->nm_of_ch, pch->chs, &(pattrib->pktlen));
		*pframe = rtw_mbo_set_1byte_ie(*pframe,
				&pch->preference, &(pattrib->pktlen));
		*pframe = rtw_mbo_set_1byte_ie(*pframe,
				&pch->reason, &(pattrib->pktlen));
		RTW_MBO_INFO("%s :Non-preferred Channel Report"
				" subelement\n", __func__);
		RTW_MBO_DUMP(":", pcont, len);
		pcont = *pframe;
	}
}

void rtw_mbo_build_probe_req_ies(
	_adapter *padapter, u8 **pframe, struct pkt_attrib *pattrib)
{
	u32 len =0;

	rtw_mbo_build_extended_cap(padapter, pframe, pattrib);

	len = rtw_mbo_attr_sz_get(padapter, RTW_MBO_ATTR_CELL_DATA_CAP_ID);
	if ((len == 0) || (len > 3)) {
		RTW_ERR("MBO : build Cellular Data Capabilities"
			" attribute fail(len=%u)\n", len);
		return;
	}

	rtw_mbo_build_mbo_ie_hdr(pframe, pattrib, len);
	rtw_mbo_build_cell_data_cap_attr(padapter, pframe, pattrib);
}

void rtw_mbo_build_assoc_req_ies(
	_adapter *padapter, u8 **pframe, struct pkt_attrib *pattrib)
{
	u32 len = 0;

	rtw_mbo_build_supp_op_class_elem(padapter, pframe, pattrib);

	len += rtw_mbo_attr_sz_get(padapter, RTW_MBO_ATTR_CELL_DATA_CAP_ID);
	len += rtw_mbo_attr_sz_get(padapter, RTW_MBO_ATTR_NPREF_CH_RPT_ID);
	if ((len == 0)|| (len < 3)) {
		RTW_ERR("MBO : build assoc MBO IE fail(len=%u)\n", len);
		return;
	}

	rtw_mbo_build_mbo_ie_hdr(pframe, pattrib, len);
	rtw_mbo_build_cell_data_cap_attr(padapter, pframe, pattrib);
	rtw_mbo_build_npref_ch_rpt_attr(padapter, pframe, pattrib);
}

static void rtw_mbo_build_ap_capability(
	_adapter *padapter, u8 **pframe,
	struct pkt_attrib *pattrib)
{
	struct _ADAPTER_LINK *padapter_link = pattrib->adapter_link;
	struct link_mlme_ext_priv *pmlmeext = &(padapter_link->mlmeextpriv);
	struct link_mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);
	WLAN_BSSID_EX *cur_network = &(pmlmeinfo->network);
	struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);
	struct mbo_attr_info *pmbo_attr = &(pmlmepriv->mbo_attr);
	u8 *pcap = NULL;
	u32 cap_len = 0, flen = 0;

	if ((pcap = rtw_mbo_attrs_get(
		(cur_network->IEs + _FIXED_IE_LENGTH_),
		(cur_network->IELength - _FIXED_IE_LENGTH_),
		RTW_MBO_ATTR_AP_CAP_ID, &cap_len, 0)) != NULL)
		return;

	flen += rtw_mbo_attr_sz_get(padapter, RTW_MBO_ATTR_AP_CAP_ID);
	if (pmbo_attr->assoc_disallow > 0)
		flen += rtw_mbo_attr_sz_get(padapter,
				RTW_MBO_ATTR_ASSOC_DISABLED_ID);
	if (flen > 0) {
		rtw_mbo_build_mbo_ie_hdr(pframe, pattrib, flen);
		rtw_mbo_build_ap_cap_Indication_attr(padapter, pframe,
				pattrib, pmbo_attr->ap_cap_ind);

		if (pmbo_attr->assoc_disallow > 0) {
			rtw_mbo_build_ap_disallowed_attr(padapter, pframe,
				pattrib, pmbo_attr->assoc_disallow);
		}
	} /*  end of if (flen > 0) */

}

void rtw_mbo_attr_info_init(_adapter *padapter)
{
	struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);
	struct mbo_attr_info *pmbo_attr = &(pmlmepriv->mbo_attr);

	_rtw_memset(pmbo_attr, 0, sizeof(struct mbo_attr_info));
}

void rtw_mbo_process_assoc_req(
	_adapter *padapter, u8 *pie, int ie_len)
{
	struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);
	struct mbo_attr_info *pmbo_attr = &(pmlmepriv->mbo_attr);
	u8 non_pref_ch = 0, ch_op_pref = 1;
	u8 *pattr = NULL, *ptr = NULL;
	u32 attr_len = 0, ch_nums = 0;
	int i, j;

	if (!pie || !ie_len)
		return;

	if ((pattr = rtw_mbo_attrs_get(pie, ie_len,
		RTW_MBO_ATTR_NPREF_CH_RPT_ID, &attr_len, 1)) == NULL)
		return;

	if (attr_len < 3)
		return;

	ch_nums = (attr_len - 3);

	/* shfit to non-preferred ch rpt field */
	ptr = pattr + 3;
	RTW_MBO_DUMP("non-preferred ch rpt :", ptr, ch_nums);

	ch_op_pref = *(ptr + ch_nums);
	RTW_MBO_INFO("%s : ch_op_pref=0x%02x\n", __func__, ch_op_pref);

	if (ch_op_pref >= 2) {
		RTW_MBO_INFO("%s : unknow ch operating preference(0x%02x)\n",
			__func__, ch_op_pref);
		return;
	}

	for (i = 0; i < ch_nums; i++) {
		if (i >= RTW_MBO_MAX_CH_LIST_NUM)
			break;
		non_pref_ch = *(ptr + i);
		RTW_MBO_INFO("%s : non-pref ch %u found in assoc-req\n",
			__func__, non_pref_ch);

		if (rtw_mbo_wifi_spec_test(padapter)) {
			struct mbo_user_btm_req_pkt *puser_raw = \
				&(pmbo_attr->user_raw);
			struct wnm_btm_cant *pcant = NULL;

			for (j = 0; j < RTW_MAX_NB_RPT_NUM; j++) {
				pcant = &puser_raw->btm_cants[j];
				if ((pcant->nb_rpt.ch_num == non_pref_ch) && \
					(ch_op_pref == 0)) {
					RTW_MBO_INFO("%s : reset "
					"preference(%u) for non-preference ch\n",
					__func__, pcant->preference);
					pcant->preference = 0;
				}
			}
		}

	} /* end of for-loop ch_nums */

}

void rtw_mbo_build_beacon_ies(
	_adapter *padapter, u8 **pframe, struct pkt_attrib *pattrib)
{
	rtw_mbo_build_ap_capability(padapter, pframe, pattrib);
}

void rtw_mbo_build_probe_rsp_ies(
	_adapter *padapter, u8 **pframe, struct pkt_attrib *pattrib)
{
	rtw_mbo_build_ap_capability(padapter, pframe, pattrib);
}

void rtw_mbo_build_assoc_rsp_ies(
	_adapter *padapter, u8 **pframe, struct pkt_attrib *pattrib)
{
	struct _ADAPTER_LINK *padapter_link = pattrib->adapter_link;
	struct link_mlme_ext_priv *pmlmeext = &(padapter_link->mlmeextpriv);
	struct link_mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);
	WLAN_BSSID_EX *cur_network = &(pmlmeinfo->network);
	struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);
	struct mbo_attr_info *pmbo_attr = &(pmlmepriv->mbo_attr);
	u8 *pcap = NULL;
	u32 len = 0, cap_len = 0 ;

	#if 1
	if ((pcap = rtw_mbo_attrs_get(
		(cur_network->IEs + _FIXED_IE_LENGTH_),
		(cur_network->IELength - _FIXED_IE_LENGTH_),
		RTW_MBO_ATTR_AP_CAP_ID, &cap_len, 0)) != NULL)
		return;
	#endif

	len += rtw_mbo_attr_sz_get(padapter, RTW_MBO_ATTR_AP_CAP_ID);
	if (pmbo_attr->assoc_disallow > 0)
		len += rtw_mbo_attr_sz_get(padapter,
			RTW_MBO_ATTR_ASSOC_DISABLED_ID);

	if (len > 0) {
		rtw_mbo_build_mbo_ie_hdr(pframe, pattrib, len);
		rtw_mbo_build_ap_cap_Indication_attr(
			padapter, pframe, pattrib, pmbo_attr->ap_cap_ind);
		if (pmbo_attr->assoc_disallow > 0) {
			rtw_mbo_build_ap_disallowed_attr(padapter, pframe,
				pattrib, pmbo_attr->assoc_disallow);
		}
	}

}

void rtw_mbo_build_wnm_btmreq_reason_ies(
	_adapter *padapter, u8 **pframe, struct pkt_attrib *pattrib)
{
	struct _ADAPTER_LINK *padapter_link = pattrib->adapter_link;
	struct link_mlme_ext_priv *pmlmeext = &(padapter_link->mlmeextpriv);
	struct link_mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);
	WLAN_BSSID_EX *cur_network = &(pmlmeinfo->network);
	struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);
	struct mbo_attr_info *pmbo_attr = &(pmlmepriv->mbo_attr);
	struct mbo_user_btm_req_pkt *puser = &(pmbo_attr->user_raw);
	u8 *pcap = NULL;
	u32 len = 0, cap_len = 0 ;

	if (!puser->append_mbo_ie)
		return;

	len += rtw_mbo_attr_sz_get(padapter, RTW_MBO_ATTR_AP_CAP_ID);
	len += rtw_mbo_attr_sz_get(padapter, RTW_MBO_ATTR_TRANS_RES_ID);
	len += rtw_mbo_attr_sz_get(padapter, RTW_MBO_ATTR_ASSOC_RETRY_DELAY_ID);

	rtw_mbo_build_mbo_ie_hdr(pframe, pattrib, len);

	rtw_mbo_build_ap_cap_Indication_attr(
			padapter, pframe, pattrib, pmbo_attr->ap_cap_ind);

	rtw_mbo_build_ap_trans_reason_attr(padapter, pframe,
			pattrib, pmbo_attr->reason);

	rtw_mbo_build_ap_assoc_retry_delay_attr(padapter, pframe,
			pattrib, pmbo_attr->delay);
}
#endif /* CONFIG_RTW_MBO */

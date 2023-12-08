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
#define _RTW_EHT_C

#include <drv_types.h>

#ifdef CONFIG_80211BE_EHT

void rtw_set_assoc_status(_adapter *padapter, struct rtw_phl_mld_t *pmld, u8 link_id, u16 status)
{
	u8 i;
	for (i = 0; i < padapter->phl_role->rlink_num; i++) {
		if (pmld->assoc_status[i].used)
			continue;
		pmld->assoc_status[i].link_id = link_id;
		pmld->assoc_status[i].status = status;
		pmld->assoc_status[i].used = 1;
		break;
	}
}

void rtw_update_ies_with_inheritance(u8 *cur_ies, u32 *cur_ies_len, u8 *inh_ies, u8 inh_ies_len)
{
	u8 *non_inh_ele, non_inh_ele_len;
	u8 i, j;
	struct _NDIS_802_11_VARIABLE_IEs *pIE = NULL;

	/* Remove the non-inherited elements*/
	for (i = 0; i < inh_ies_len;) {
		pIE = (struct _NDIS_802_11_VARIABLE_IEs *)(inh_ies + i);

		if (pIE->ElementID == WLAN_EID_EXTENSION &&
				pIE->data[0] == EID_EXT_NON_INHERITANCE) {
			/* Non-inherited elements */
			non_inh_ele = &pIE->data[0] + 1;
			non_inh_ele_len = *non_inh_ele++;
			for (j = 0; j < non_inh_ele_len; j++) {
				rtw_ies_remove_ie(cur_ies, cur_ies_len, 0, *non_inh_ele, NULL, 0);
				non_inh_ele++;
			}
			/* Non-inherited extension elements */
			non_inh_ele_len = *non_inh_ele++;
			for (j = 0; j < non_inh_ele_len; j++) {
				rtw_ies_remove_ie(cur_ies, cur_ies_len, 0, WLAN_EID_EXTENSION, non_inh_ele, 1);
				non_inh_ele++;
			}
		} else if (pIE->ElementID == WLAN_EID_EXTENSION) { /* Update extension IEs */
			rtw_ies_remove_ie(cur_ies, cur_ies_len, 0, WLAN_EID_EXTENSION, &pIE->data[0], 1);
			rtw_set_ie(cur_ies + *cur_ies_len, WLAN_EID_EXTENSION,
				pIE->Length, pIE->data, cur_ies_len);
		} else { /* Update IEs */
			rtw_ies_remove_ie(cur_ies, cur_ies_len, 0, pIE->ElementID, NULL, 0);
			rtw_set_ie(cur_ies + *cur_ies_len, pIE->ElementID,
				pIE->Length, pIE->data, cur_ies_len);
		}
		i += (pIE->Length + 2);
	}
}

void assoc_rsp_per_sta_profile_handler(_adapter *padapter,
		struct rtw_phl_per_sta_profile_element *profile,
		u8 *primary_ies, u32 primary_ies_len)
{
	struct _ADAPTER_LINK *cur_link = NULL;
	u8 cur_ies[MAX_IE_SZ];
	u32 cur_ies_len = 0;
	struct rtw_phl_mld_t *pmld = NULL;
	unsigned short status;
	u8 eid_eht_ml = EID_EXT_MULTI_LINK;

	pmld = rtw_phl_get_mld_self(GET_PHL_INFO(adapter_to_dvobj(padapter)), padapter->phl_role);
	cur_link = rtw_get_adapter_link_by_linkid(padapter, profile->link_id);

	if (!cur_link)
		goto _exit;

	/* status */
	status = le16_to_cpu(*(unsigned short *)(profile->sta_profile + 2));
	rtw_set_assoc_status(padapter, pmld, profile->link_id, status);
	if (status != _STATS_SUCCESSFUL_) {
		RTW_INFO("assoc reject for link (link_id = %d), status code: %d\n",
			profile->link_id, status);
		goto _exit;
	}
	cur_link->mlmepriv.is_accepted = _TRUE;
	/* get capabilities */
	cur_link->mlmeextpriv.mlmext_info.capability =
				le16_to_cpu(*(unsigned short *)(profile->sta_profile));
	/* set slot time */
	cur_link->mlmeextpriv.mlmext_info.slotTime =
		(cur_link->mlmeextpriv.mlmext_info.capability & BIT(10)) ? 9 : 20;

	_rtw_memcpy(cur_ies, primary_ies, primary_ies_len);
	cur_ies_len = primary_ies_len;


	/* Remove ML IEs for other links */
	rtw_ies_remove_ie(cur_ies, &cur_ies_len, 0, WLAN_EID_EXTENSION, &eid_eht_ml, 1);

	/* Update cur_ies */
	rtw_update_ies_with_inheritance(cur_ies, &cur_ies_len,
		profile->sta_profile + 4, profile->sta_profile_len - 4);

	rtw_ie_handler(padapter, cur_link, cur_ies, cur_ies_len);

_exit:
	return;
}

u32 rtw_parse_ml_ie(_adapter *padapter, u8 *ies, u32 ies_len,
		struct rtw_phl_ml_element *ml_ele)
{
	struct rtw_phl_com_t *phl_com = GET_PHL_COM(adapter_to_dvobj(padapter));
	u8 eid_eht_ml = EID_EXT_MULTI_LINK;
	u32 eht_ml_ie_len;
	u8 *p;

	p = rtw_get_ie_ex(ies, ies_len, WLAN_EID_EXTENSION,
			&eid_eht_ml, 1, NULL, &eht_ml_ie_len);

	if (p == NULL || eht_ml_ie_len < EHT_ML_BASIC_MIN_LEN)
		return 0;

	rtw_phl_parse_ml_ie(phl_com, p + 3, eht_ml_ie_len - 3, ml_ele);
	return eht_ml_ie_len;
}

void EHT_ml_ie_handler(_adapter *padapter, struct _ADAPTER_LINK *padapter_link,
		u8 *primary_ies, u32 primary_ies_len)
{
	struct mlme_priv	*pmlmepriv = &padapter->mlmepriv;
	struct mlo_priv		*pmlopriv = &pmlmepriv->mlopriv;
	struct rtw_phl_ml_element ml_ele = {0};
	u8 lidx;

	if (!rtw_parse_ml_ie(padapter, primary_ies, primary_ies_len, &ml_ele))
		return;

	for (lidx = 0; lidx < ml_ele.profile_num; lidx++)
		assoc_rsp_per_sta_profile_handler(padapter, &ml_ele.profile[lidx],
				primary_ies, primary_ies_len);

}

void EHT_tid2link_ie_handler(_adapter *padapter, struct _ADAPTER_LINK *padapter_link,
		u8 *ies, u32 ies_len)
{
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	struct mlo_priv *pmlopriv = &pmlmepriv->mlopriv;
	struct link_mlme_ext_priv *pmlmeext = &padapter_link->mlmeextpriv;
	struct link_mlme_ext_info *pmlmeinfo = &pmlmeext->mlmext_info;
	WLAN_BSSID_EX *cur_network = &pmlmeinfo->network;
	struct sta_info		*psta = NULL;
	u8 eid_eht_tid2link = EID_EXT_TID_TO_LINK_MAPPING;
	u32 eht_tid2link_ie_len;
	u8 *p;

	if (pmlopriv->tid2link_nego == _FALSE)
		return;

	psta = rtw_get_stainfo(&padapter->stapriv, cur_network->MacAddress);

	if (!psta)
		return;

	/* TID-To_Link Mapping IE */
	p = rtw_get_ie_ex(ies, ies_len, WLAN_EID_EXTENSION,
			&eid_eht_tid2link, 1, NULL, &eht_tid2link_ie_len);

	if (p == NULL)
		rtw_phl_tid2link_not_present(psta->phl_sta, pmlopriv->tid2link_nego);
	else
		rtw_phl_parse_tid2link(psta->phl_sta, p + 3, eht_tid2link_ie_len - 3);
}

void rtw_eht_ml_ies_handler(_adapter *padapter, struct _ADAPTER_LINK *padapter_link,
		u8 *ies, u32 ies_len)
{
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	struct mlo_priv *pmlopriv = &pmlmepriv->mlopriv;

	if (pmlopriv->mlo_option == _FALSE)
		return;

	/* ML IE */
	EHT_ml_ie_handler(padapter, padapter_link, ies, ies_len);

	/* TID-to-Link IE */
	EHT_tid2link_ie_handler(padapter, padapter_link, ies, ies_len);
}

u8 build_inherited_ies(u8 *cur_ies, u32 cur_ies_len, u8 *primary_ies, u32 primary_ies_len, u8 *sta_ies)
{
	PNDIS_802_11_VARIABLE_IEs sIE = NULL;
	u8 *p;
	u32 i;
	u32 len;
	u8 nih_eles[256];
	u8 nih_eles_len = 0;
	u8 nih_ext_eles[256];
	u8 nih_ext_eles_len = 0;
	u32 sta_ies_len = 0;

	/* Build per sta IEs */
	for (i = 0; i < cur_ies_len;) {
		sIE = (struct _NDIS_802_11_VARIABLE_IEs *)(cur_ies + i);
		if (sIE->ElementID == WLAN_EID_EXTENSION) {
			p = rtw_get_ie_ex(primary_ies, primary_ies_len,
				WLAN_EID_EXTENSION, &sIE->data[0], 1, NULL, &len);
			len -= 2;
		}
		else
			p = rtw_get_ie(primary_ies, sIE->ElementID, &len, primary_ies_len);

		/* Skip the same IEs */
		if (p == NULL) /* Element only exists in cur_ies */
			rtw_set_ie(sta_ies + sta_ies_len, sIE->ElementID, sIE->Length, sIE->data, &sta_ies_len);
		else if ((sIE->Length != len || !_rtw_memcmp(sIE->data, p + 2, len)) /* Element is different between primary_ies and cur_ies */
				&& (sIE->ElementID != WLAN_EID_VENDOR_SPECIFIC)) /* skip vendor specific */
			rtw_set_ie(sta_ies + sta_ies_len, sIE->ElementID, sIE->Length, sIE->data, &sta_ies_len);

		i += (sIE->Length + 2);
	}

	/* Get non-inherited elements */
	nih_eles[nih_eles_len++] = EID_EXT_NON_INHERITANCE;
	nih_eles_len++; /* Skip length first */
	for (i = 0; i < primary_ies_len;) {
		sIE = (struct _NDIS_802_11_VARIABLE_IEs *)(primary_ies + i);
		if (sIE->ElementID == WLAN_EID_EXTENSION) {
			p = rtw_get_ie_ex(cur_ies, cur_ies_len,
				WLAN_EID_EXTENSION, &sIE->data[0], 1, NULL, &len);
			if (p == NULL)
				nih_ext_eles[nih_ext_eles_len++] = sIE->data[0];
		}
		else {
			p = rtw_get_ie(cur_ies, sIE->ElementID, &len, cur_ies_len);
			if (p == NULL)
				nih_eles[nih_eles_len++] = sIE->ElementID;
		}
		i += (sIE->Length + 2);
	}

	/* Append non-inherited element IE */
	nih_eles[1] = nih_eles_len - 2;
	nih_eles[nih_eles_len++] = nih_ext_eles_len;
	for (i = 0; i < nih_ext_eles_len; i++)
		nih_eles[nih_eles_len++] = nih_ext_eles[i];
	if (nih_eles_len > 3) /* non-inherited elements exist */
		rtw_set_ie(sta_ies + sta_ies_len, WLAN_EID_EXTENSION, nih_eles_len, nih_eles, &sta_ies_len);

	return (u8)sta_ies_len;
}

int build_link_assocreq_ies(_adapter *padapter, struct _ADAPTER_LINK *cur_link, u8 *cur_ies, u32 *cur_ies_len, u8 is_reassoc)
{
	struct pkt_attrib cur_attrib = {0};
	int ret;

	cur_attrib.adapter_link = cur_link;
	ret = rtw_build_assocreq_ies(padapter, cur_link, cur_ies, &cur_attrib, is_reassoc);
	*cur_ies_len = cur_attrib.pktlen;

	return ret;
}

void build_link_probersp_ies(_adapter *padapter, struct _ADAPTER_LINK *cur_link, u8 *cur_ies, u32 *cur_ies_len,
		u8 is_valid_p2p_probereq)
{
	struct pkt_attrib cur_attrib = {0};

	cur_attrib.adapter_link = cur_link;
	rtw_build_probersp_ies(padapter, cur_link, cur_ies, &cur_attrib, is_valid_p2p_probereq);
	*cur_ies_len = cur_attrib.pktlen;

}

u8 _build_ml_sta_profile(_adapter *padapter, struct _ADAPTER_LINK *cur_link, struct sta_info *cur_sta,
			enum phl_packet_type pkt_type, u8 *primary_ies, u32 primary_ies_len, u8 *ies, u8 reserved)
{
	u8 cur_ies[MAX_IE_SZ] = {0};
	u32 cur_ies_len = 0;
	u8 offset;
	u8 ies_len = 0;

	switch(pkt_type) {
		case PACKET_ASSOC_REQUEST:
			if (reserved) /* reassoc_req */
				offset = _REASOCREQ_IE_OFFSET_;
			else
				offset = _ASOCREQ_IE_OFFSET_;
			if (build_link_assocreq_ies(padapter, cur_link, cur_ies, &cur_ies_len, reserved)
					!= _SUCCESS) /* don't request the current link if no joint supported rate */
				goto _exit;
			/* capability */
			_rtw_memcpy(ies, cur_ies, 2);
			ies_len += 2;
			/* skip listen interval and current AP field */
			ies_len += build_inherited_ies(cur_ies + offset, cur_ies_len - offset,
					primary_ies + offset, primary_ies_len - offset,
					ies + ies_len);
			break;
		case PACKET_ASSOC_RESPONSE:
			/* ToDo */
			break;
		case PACKET_PROBE_RESPONSE_ML:
			offset = _PROBERSP_IE_OFFSET_;
			build_link_probersp_ies(padapter, cur_link, cur_ies, &cur_ies_len, reserved);
			/* copy capabilities */
			_rtw_memcpy(ies, (unsigned char *)(rtw_get_capability_from_ie(cur_ies)) , 2);
			ies_len += 2;
			ies_len += build_inherited_ies(cur_ies + offset, cur_ies_len - offset,
				primary_ies + offset, primary_ies_len - offset,
				ies + ies_len);
			break;
		default:
			break;
	}

_exit:
	return ies_len;
}

u8 *rtw_build_ml_ies(_adapter *padapter, struct sta_info *psta, u8 *pframe, struct pkt_attrib *pattrib,
			enum phl_packet_type pkt_type, u8 *primary_ies, u32 primary_ies_len, u8 reserved)
{
	struct dvobj_priv *d = adapter_to_dvobj(padapter);
	struct _ADAPTER_LINK *padapter_link = pattrib->adapter_link;
	u32 len = 0;
	struct rtw_phl_ml_ie_info info = {0};
	u8 lidx;
	u8 profiles[256] = {0};
	struct rtw_phl_mld_t *pmld;
	u8 critical_update = 0;
	u32 total_len = 0;

	if (psta)
		info.mld = psta->phl_sta->mld;
	else
		info.mld = rtw_phl_get_mld_self(GET_PHL_INFO(d), padapter->phl_role);
	info.pkt_type = pkt_type;
	info.rlink = padapter_link->wrlink;

	/* The following types need to append link info */
	switch(pkt_type) {
		case PACKET_BEACON:
			/* ToDo CONFIG_RTW_MLD: Get the critical update value */
			info.critical_update = critical_update;
			break;
		case PACKET_ASSOC_REQUEST:
			/* Multilink Element */
			/* Append the to_join link info */
			for (lidx = 0; lidx < padapter->adapter_link_num; lidx++) {
				struct _ADAPTER_LINK *alink = NULL;
				struct rtw_phl_per_sta_profile_info ml_sta_info = {0};
				u8 sta_profile[256] = {0};

				alink = GET_LINK(padapter, lidx);
				if (alink == padapter_link || !alink->mlmepriv.to_join)
					continue;

				ml_sta_info.pkt_type = pkt_type;
				ml_sta_info.mld = rtw_phl_get_mld_self(GET_PHL_INFO(d), padapter->phl_role);
				ml_sta_info.rlink = alink->wrlink;
				ml_sta_info.link_id = alink->mlmepriv.link_id;
				ml_sta_info.sta_profile_len = _build_ml_sta_profile(padapter, alink, NULL, pkt_type, primary_ies,
						primary_ies_len, sta_profile, reserved);
				ml_sta_info.sta_profile = sta_profile;
				len = rtw_phl_build_per_sta_profile(GET_PHL_COM(d), &ml_sta_info, profiles + info.opt_len);
				info.opt_len += len;
			}
			info.opt = profiles;
			total_len += rtw_phl_build_ml_ie(GET_PHL_COM(d), &info, pframe);

			/* TID-to-Link Mapping Element */
			if (padapter->mlmepriv.mlopriv.tid2link_nego)
				total_len += rtw_phl_build_tid2link(padapter_link->wrlink, pframe + total_len);

			break;
		case PACKET_ASSOC_RESPONSE:
			/* ToDo */
			break;
		case PACKET_PROBE_RESPONSE_ML:
			for (lidx = 0; lidx < padapter->adapter_link_num; lidx++) {
				struct _ADAPTER_LINK *alink = NULL;
				struct rtw_phl_per_sta_profile_info ml_sta_info = {0};
				u8 sta_profile[256] = {0};

				alink = GET_LINK(padapter, lidx);
				/* Skip the primary adapter_link and the link without ready content */
				if (alink == padapter_link || !alink->mlmepriv.cur_network.join_res)
					continue;

				ml_sta_info.pkt_type = pkt_type;
				ml_sta_info.mld = rtw_phl_get_mld_self(GET_PHL_INFO(d), padapter->phl_role);
				ml_sta_info.rlink = alink->wrlink;
				ml_sta_info.link_id = alink->wrlink->id;
				ml_sta_info.complete_profile = _TRUE;
				ml_sta_info.sta_profile_len = _build_ml_sta_profile(padapter, alink, NULL, pkt_type, primary_ies,
						primary_ies_len, sta_profile, reserved);
				ml_sta_info.sta_profile = sta_profile;
				len = rtw_phl_build_per_sta_profile(GET_PHL_COM(d), &ml_sta_info, profiles + info.opt_len);
				info.opt_len += len;
			}
			info.opt = profiles;
			total_len += rtw_phl_build_ml_ie(GET_PHL_COM(d), &info, pframe);
			break;
		default:
			total_len += rtw_phl_build_ml_ie(GET_PHL_COM(d), &info, pframe);
			break;
	}

	pattrib->pktlen += total_len;

	return pframe + total_len;
}

u32 rtw_restructure_ml_ie(_adapter *padapter,
					struct _ADAPTER_LINK *padapter_link,
					u8 *in_ie, u8 *out_ie, uint in_len,
					uint *pout_len)
{
	struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);
	struct mlo_priv	*pmlopriv = &pmlmepriv->mlopriv;
	struct rtw_phl_ml_ie_info info = {0};
	struct rtw_phl_ml_element ml_ele = {0};

	if (!rtw_parse_ml_ie(padapter, in_ie + _FIXED_IE_LENGTH_, in_len - _FIXED_IE_LENGTH_, &ml_ele))
		goto exit;

	pmlopriv->mlo_option = _TRUE;

	if (ml_ele.common_info.basic_ml.mld_cap.tid_to_link_nego_support)
		pmlopriv->tid2link_nego = _TRUE;

	info.mld = rtw_phl_get_mld_self(GET_PHL_INFO(adapter_to_dvobj(padapter)), padapter->phl_role);
	info.pkt_type = PACKET_BEACON;
	info.rlink = padapter_link->wrlink;

	*pout_len += rtw_phl_build_ml_ie(GET_PHL_COM(adapter_to_dvobj(padapter)), &info, out_ie + *pout_len);

exit:
	return pmlopriv->mlo_option;
}

#endif /* CONFIG_80211BE_EHT */

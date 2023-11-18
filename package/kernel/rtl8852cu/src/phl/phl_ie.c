/******************************************************************************
 *
 * Copyright(c) 2019 - 2021 Realtek Corporation.
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
#define _PHL_IE_C_
#include "phl_headers.h"

u8 _phl_build_ml_common_info(struct rtw_phl_com_t *phl_com,
                             struct rtw_phl_ml_ie_info *info,
                             u8 *pbuf,
                             u8 *ml_ctrl)
{
	struct rtw_phl_stainfo_t *sta = NULL;
	void *phl = phl_com->phl_priv;
	void *drv = phl_com->drv_priv;
	u8 len = 0;
	u8 *pstart = pbuf;
	u8 *p = pbuf + 1; /* skip Common Info Length subfield */

	if (info->rlink == NULL) {
		PHL_WARN("%s: wrong rlink assignment!\n", __func__);
		goto _exit;
	}

	sta = rtw_phl_get_stainfo_self(phl, info->rlink);

	/* subfields */
	switch (GET_ML_ELE_ML_CTRL_TYPE(ml_ctrl)) {
	case BASIC_ML:
		/* MLD Mac Address subfield */
		_os_mem_cpy(drv, p, info->rlink->wrole->mac_addr, ETH_ALEN);
		p += ETH_ALEN;

		if (GET_ML_ELE_ML_CTRL_LINK_ID_INFO_PRESENT(ml_ctrl)) {
			SET_ML_ELE_COMMON_INFO_LINK_ID(p, sta->link_id);
			p += 1;
		}
		if (GET_ML_ELE_ML_CTRL_BSS_PARAMS_CHG_CNT_PRESENT(ml_ctrl)) {
			u8 val = info->rlink->bss_params_chg_cnt;

			SET_ML_ELE_COMMON_INFO_BSS_PARAMS_CHG_CNT(p, val);
			p += 1;
		}
		if (GET_ML_ELE_ML_CTRL_MEDIUM_SYNC_DELAY_INFO_PRESENT(ml_ctrl)) {
			p += 2;
		}
		if (GET_ML_ELE_ML_CTRL_EML_CAP_PRESENT(ml_ctrl)) {
			p += 3;
		}
		if (GET_ML_ELE_ML_CTRL_MLD_CAP_PRESENT(ml_ctrl)) {
			p += 2;
		}
		break;
	case PROBE_REQUEST_ML:
		if (GET_ML_ELE_ML_CTRL_MLD_ID_PRESENT(ml_ctrl)) {
			SET_ML_ELE_COMMON_INFO_MLD_ID(p, info->mld_id);
			p += 1;
		}
		break;
	default:
		break;
	}

_exit:
	len = (u8)(p - pstart);

	*pstart = len; /* Common Info Length */

	return len;
}

u8 _phl_build_basic_ml_ie(struct rtw_phl_com_t *phl_com,
                          struct rtw_phl_ml_ie_info *info,
                          u8 *pbuf,
                          u8 *ml_ctrl)
{
	void *drv = phl_com->drv_priv;
	u8 len = 0;
	u8 *pstart = pbuf;
	u8 *p = pbuf;

	/* Element ID Extension field */
	*p++ = EID_EXT_MULTI_LINK;

	/* ML Control field */
	_os_mem_cpy(drv, p, ml_ctrl, 2);
	p += 2;

	/* ML Common Info field */
	p += _phl_build_ml_common_info(phl_com, info, p, ml_ctrl);

	/* optional subelements */
	if (info->opt_len) {
		_os_mem_cpy(drv, p, info->opt, info->opt_len);
		p += info->opt_len;
	}

	len = (u8)(p - pstart);

	return len;
}

u8 _phl_build_probe_request_ml_ie(struct rtw_phl_com_t *phl_com,
                                  struct rtw_phl_ml_ie_info *info,
                                  u8 *pbuf,
                                  u8 *ml_ctrl)
{
	void *drv = phl_com->drv_priv;
	u8 len = 0;
	u8 *pstart = pbuf;
	u8 *p = pbuf;

	/* Element ID Extension field */
	*p++ = EID_EXT_MULTI_LINK;

	/* ML Control field */
	_os_mem_cpy(drv, p, ml_ctrl, 2);
	p += 2;

	/* ML Common Info field */
	p += _phl_build_ml_common_info(phl_com, info, p, ml_ctrl);

	/* optional subelements */
	if (info->opt_len) {
		_os_mem_cpy(drv, p, info->opt, info->opt_len);
		p += info->opt_len;
	}

	len = (u8)(p - pstart);

	return len;
}

u8 rtw_phl_build_ml_ie(struct rtw_phl_com_t *phl_com,
                       struct rtw_phl_ml_ie_info *info,
                       u8 *pbuf)
{
	u8 len = 0;
	u8 *pstart = pbuf;
	u8 *p = pbuf + 2;
	u8 ml_ctrl[2] = {0};

	/* set ML Control field in advance */
	switch (info->pkt_type) {
	case PACKET_BEACON:
		if (info->critical_update)
			info->rlink->bss_params_chg_cnt++;
		fallthrough;
	case PACKET_PROBE_RESPONSE:
	case PACKET_PROBE_RESPONSE_ML:
		SET_ML_ELE_ML_CTRL_TYPE(ml_ctrl, BASIC_ML);
		SET_ML_ELE_ML_CTRL_LINK_ID_INFO_PRESENT(ml_ctrl, true);
		SET_ML_ELE_ML_CTRL_BSS_PARAMS_CHG_CNT_PRESENT(ml_ctrl, true);
		len = _phl_build_basic_ml_ie(phl_com, info, p, ml_ctrl);
		break;
	case PACKET_PROBE_REQUEST_ML:
		SET_ML_ELE_ML_CTRL_TYPE(ml_ctrl, PROBE_REQUEST_ML);
		if (info->mld_id_present)
			SET_ML_ELE_ML_CTRL_MLD_ID_PRESENT(ml_ctrl, true);

		len = _phl_build_probe_request_ml_ie(phl_com,
		                                     info,
		                                     p,
		                                     ml_ctrl);
		break;
	case PACKET_AUTH:
		SET_ML_ELE_ML_CTRL_TYPE(ml_ctrl, BASIC_ML);
		/* LINK_ID_INFO, BSS_PARAMS_CHG_CNT, MEDIUM_SYNC_DELAY_INFO are false */
		len = _phl_build_basic_ml_ie(phl_com, info, p, ml_ctrl);
		break;
	case PACKET_ASSOC_REQUEST:
		SET_ML_ELE_ML_CTRL_TYPE(ml_ctrl, BASIC_ML);
		/* LINK_ID_INFO, BSS_PARAMS_CHG_CNT, MEDIUM_SYNC_DELAY_INFO are false */
		SET_ML_ELE_ML_CTRL_EML_CAP_PRESENT(ml_ctrl, true);
		SET_ML_ELE_ML_CTRL_MLD_CAP_PRESENT(ml_ctrl, true);
		len = _phl_build_basic_ml_ie(phl_com, info, p, ml_ctrl);
		break;
	case PACKET_ASSOC_RESPONSE:
		SET_ML_ELE_ML_CTRL_TYPE(ml_ctrl, BASIC_ML);
		SET_ML_ELE_ML_CTRL_LINK_ID_INFO_PRESENT(ml_ctrl, true);
		SET_ML_ELE_ML_CTRL_BSS_PARAMS_CHG_CNT_PRESENT(ml_ctrl, true);
		SET_ML_ELE_ML_CTRL_EML_CAP_PRESENT(ml_ctrl, true);
		SET_ML_ELE_ML_CTRL_MLD_CAP_PRESENT(ml_ctrl, true);
		len = _phl_build_basic_ml_ie(phl_com, info, p, ml_ctrl);
		break;
	default:
		break;
	}

	*pstart = EID_EXTENSION;
	*(pstart + 1) = len;
	len += 2;

	return len;
}

u8 _phl_build_sta_info(struct rtw_phl_com_t *phl_com,
                       struct rtw_phl_per_sta_profile_info *info,
                       u8 *pbuf,
                       u8 *sta_ctrl)
{
	void *drv = phl_com->drv_priv;
	u8 len = 0;
	u8 *pstart = pbuf;
	u8 *p = pbuf + 1; /* skip STA Info Length subfield */

	/* subfields */
	if (GET_STA_CTRL_MAC_ADDR_PRESENT(sta_ctrl)) {
		_os_mem_cpy(drv, p, info->rlink->mac_addr, ETH_ALEN);
		p += ETH_ALEN;
	}
	if (GET_STA_CTRL_BEACON_INTEREVAL_PRESENT(sta_ctrl)) {
		u16 bcn_interval = (u16)info->rlink->bcn_cmn.bcn_interval;

		_os_mem_cpy(drv, p, &(bcn_interval), 2);
		p += 2;
	}
	if (GET_STA_CTRL_DTIM_INFO_PRESENT(sta_ctrl)) {
		u16 dtim_info = 0;
		u8 dtim_cnt = 0;
		u8 dtim_period = 0;

		/* DTIM cnt should be refine later if DTIM period is not 1 */
		dtim_period = (u8)info->rlink->dtim_period;
		dtim_info = (dtim_period << 8) | dtim_cnt;
		_os_mem_cpy(drv, p, &(dtim_info), 2);
		p += 2;
	}
	if (GET_STA_CTRL_NSTR_LINK_PAIR_PRESENT(sta_ctrl)) {
		if (GET_STA_CTRL_NSTR_BITMAP_SIZE(sta_ctrl) == 0) {
			/* TODO */
			p += 1;
		}
		else if (GET_STA_CTRL_NSTR_BITMAP_SIZE(sta_ctrl) == 1) {
			/* TODO */
			p += 2;
		}
	}

	len = (u8)(p - pstart);

	*pstart = len; /* STA Info Length */

	return len;
}

u8 rtw_phl_build_per_sta_profile(struct rtw_phl_com_t *phl_com,
                                 struct rtw_phl_per_sta_profile_info *info,
                                 u8 *pbuf)
{
	struct rtw_phl_stainfo_t *sta = NULL;
	void *phl = phl_com->phl_priv;
	void *drv = phl_com->drv_priv;
	u8 len = 0, total_len = 0;
	u8 *pstart = pbuf;
	u8 *p = pbuf + 2;
	u8 sta_ctrl[2] = {0};

	if (info->rlink == NULL) {
		PHL_WARN("%s: wrong rlink assignment!\n", __func__);
		return total_len;
	}

	sta = rtw_phl_get_stainfo_self(phl, info->rlink);

	/* STA Control field */
	switch (info->pkt_type) {
	case PACKET_BEACON:
	case PACKET_PROBE_RESPONSE:
		SET_STA_CTRL_LINK_ID(sta_ctrl, sta->link_id);
		break;
	case PACKET_PROBE_RESPONSE_ML:
		SET_STA_CTRL_LINK_ID(sta_ctrl, sta->link_id);
		if (info->complete_profile) {
			SET_STA_CTRL_COMPLETE_PROFILE(sta_ctrl, true);
			SET_STA_CTRL_MAC_ADDR_PRESENT(sta_ctrl, true);
			SET_STA_CTRL_BEACON_INTEREVAL_PRESENT(sta_ctrl, true);
			SET_STA_CTRL_DTIM_INFO_PRESENT(sta_ctrl, true);
		}
		break;
	case PACKET_ASSOC_RESPONSE:
		SET_STA_CTRL_LINK_ID(sta_ctrl, sta->link_id);
		SET_STA_CTRL_COMPLETE_PROFILE(sta_ctrl, true);
		SET_STA_CTRL_MAC_ADDR_PRESENT(sta_ctrl, true);
		SET_STA_CTRL_BEACON_INTEREVAL_PRESENT(sta_ctrl, true);
		SET_STA_CTRL_DTIM_INFO_PRESENT(sta_ctrl, true);
		break;
	case PACKET_PROBE_REQUEST_ML:
		SET_STA_CTRL_LINK_ID(sta_ctrl, info->link_id);
		if (info->complete_profile) {
			SET_STA_CTRL_COMPLETE_PROFILE(sta_ctrl, true);
		}
		break;
	case PACKET_ASSOC_REQUEST:
		SET_STA_CTRL_LINK_ID(sta_ctrl, info->link_id);
		SET_STA_CTRL_COMPLETE_PROFILE(sta_ctrl, true);
		SET_STA_CTRL_MAC_ADDR_PRESENT(sta_ctrl, true);
		break;
	default:
		break;
	}

	/* STA Control field */
	_os_mem_cpy(drv, p, sta_ctrl, 2);
	p += 2;
	total_len += 2;

	if (info->pkt_type != PACKET_PROBE_REQUEST_ML) {
		/* STA Info field */
		len = _phl_build_sta_info(phl_com, info, p, sta_ctrl);
		p += len;
		total_len += len;
	}

	/* STA Profile field */
	if (info->sta_profile_len) {
		_os_mem_cpy(drv, p, info->sta_profile, info->sta_profile_len);
		total_len += info->sta_profile_len;
	}

	*pstart = WLAN_SUBEID_PER_STA_PROFILE;
	*(pstart + 1) = total_len;
	total_len += 2;

	return total_len;
}

void _set_link_mapping(u8 *ele_pos,
                       u16 *tid2link,
                       u8 *indicator,
                       u8 *total_len)
{
	u8 idx = 0;
	u8 *ele_linkmap = ele_pos;

	*indicator = 0;
	for(idx = 0; idx < WMM_AC_TID_NUM; idx++) {
		if(tid2link[idx]) {
			SET_LINK_MAPPING_TID(ele_linkmap, tid2link[idx]);
			ele_linkmap +=2;
			*total_len += 2;
			*indicator |= BIT(idx);
		}
	}
}

/*
 * (Re)Association Request/Response
 * Tid-To-Link Mapping Request/Response action frame
*/
u8 rtw_phl_build_tid2link(struct rtw_wifi_role_link_t *rlink,
                          u8 *ele_start)
{
	struct protocol_cap_t *protocol_cap = &rlink->protocol_cap;
	u8 *ele_pos = NULL, *ele_linkmap = 0;
	u8  def_mapping = true, indicator = 0, idx = 0;
	u8 total_len = 0;

	/* Element ID */
	ele_start[0] = EID_EXTENSION;

	/* Element ID Extension field */
	ele_start[2] = EID_EXT_TID_TO_LINK_MAPPING;

	for(idx = 0; idx < WMM_AC_TID_NUM; idx++) {
		if(protocol_cap->tid2link_ul[idx] != 0x7fff ||
		   protocol_cap->tid2link_dl[idx] != 0x7fff ) {
			def_mapping = false;
			break;
		}
	}

	ele_pos = ele_start+3; /* start of tid2link control */
	if(def_mapping) {
		SET_TID2LINK_CTRL_DIRECT(ele_pos, 2);
		SET_TID2LINK_CTRL_DEFAULT(ele_pos, 1);
		SET_TID2LINK_CTRL_INDIC(ele_pos, indicator); /* indicator is 0 */
		total_len += 2;
	}
	else {
		/* 0: uplink */
		SET_TID2LINK_CTRL_DIRECT(ele_pos, 0);
		SET_TID2LINK_CTRL_DEFAULT(ele_pos, 0);
		total_len += 2;
		ele_linkmap = ele_pos+2;
		_set_link_mapping(ele_linkmap, protocol_cap->tid2link_ul, &indicator, &total_len);
		SET_TID2LINK_CTRL_INDIC(ele_pos, indicator);
		ele_pos = ele_pos + total_len;

		/* 1: downlink */
		SET_TID2LINK_CTRL_DIRECT(ele_pos, 1);
		SET_TID2LINK_CTRL_DEFAULT(ele_pos, 0);
		total_len += 2;
		ele_linkmap = ele_pos+2;
		_set_link_mapping(ele_linkmap, protocol_cap->tid2link_dl, &indicator, &total_len);
		SET_TID2LINK_CTRL_INDIC(ele_pos, indicator);
	}

	ele_start[1]= total_len + 1; /* Ext ID field is included */

	return (total_len +3); /* From Element ID/ Len/ Ext ID */
}

void
_dump_tid2link(struct rtw_phl_stainfo_t *sta)
{
	u8 idx =0;
	PHL_INFO("###### _dump_tid2link #######\n");
	for (idx = 0; idx < WMM_AC_TID_NUM; idx++) {
		PHL_INFO("\t[TID-%d] uplink mapp:0x%02X, downlink map:%02X\n",
		         idx, sta->asoc_cap.tid2link_ul[idx],
		         sta->asoc_cap.tid2link_dl[idx]);

		if(!sta->asoc_cap.tid2link_ul[idx] && !sta->asoc_cap.tid2link_dl[idx])
			PHL_ERR("\t[TID-%d]:: a TID shall be mapped to at least one setup link\n",
			         idx);
	}
}

void rtw_phl_parse_tid2link(struct rtw_phl_stainfo_t *sta,
                            u8 *ele_start,
                            u16 ele_len)
{
	u8 *ele_pos = NULL;
	u8 direction = 0, def_mapping = 0, indicator = 0, idx = 0;
	bool downlink = true, uplink = true;

	ele_pos = ele_start;
	do {
		direction = GET_TID2LINK_CTRL_DIRECT(ele_pos);
		downlink = (direction == 0)? false: true; /* 0: uplink */
		uplink  = (direction == 1)? false: true;  /* 1: downlink */
		def_mapping = GET_TID2LINK_CTRL_DEFAULT(ele_pos);
		if (!def_mapping) {

			indicator = GET_TID2LINK_CTRL_INDIC(ele_pos);
			ele_pos += 2; /* skip mapping control */
			for(idx = 0; idx < WMM_AC_TID_NUM; idx++) {
				if(BIT(idx) & indicator) {
					if(uplink)
						sta->asoc_cap.tid2link_ul[idx] =
							GET_LINK_MAPPING_TID(ele_pos);
					if(downlink)
						sta->asoc_cap.tid2link_dl[idx] =
							GET_LINK_MAPPING_TID(ele_pos);
					ele_pos += 2;
				}
			}
		}
		else {
			ele_pos += 2; /* skip mapping control */

			/* 35.3.6.1.2 Default mapping mode */
			for(idx = 0; idx < WMM_AC_TID_NUM; idx++) {
				/* Link id 15 : if the reported AP is not part of an AP MLD. */
				sta->asoc_cap.tid2link_ul[idx] = 0x7fff;
				sta->asoc_cap.tid2link_dl[idx] = 0x7fff;
			}
		}
	} while(ele_pos < (ele_start + ele_len));

	_dump_tid2link(sta);

	phl_mld_link2tid(sta);
}

void rtw_phl_tid2link_not_present(struct rtw_phl_stainfo_t *sta, u8 nego)
{
	struct protocol_cap_t *protocol_cap = &sta->rlink->protocol_cap;
	u8 idx = 0;
	u16 link_ul = 0, link_dl = 0;

	for(idx = 0; idx < WMM_AC_TID_NUM; idx++) {
		link_ul = 0x7fff;
		link_dl = 0x7fff;

		/* AP accept our tid2link negotiation */
		if(nego) {
			link_ul = protocol_cap->tid2link_ul[idx];
			link_dl = protocol_cap->tid2link_dl[idx];
		}

		sta->asoc_cap.tid2link_ul[idx] = link_ul;
		sta->asoc_cap.tid2link_dl[idx] = link_dl;
	}

	phl_mld_link2tid(sta);
}

void
_dump_per_sta_profile(struct rtw_phl_per_sta_profile_element ele)
{
	PHL_INFO("###### _dump_per_sta_profile #######\n");
	PHL_INFO("%-25s: %d\n", "Link ID", ele.link_id);
	PHL_INFO("%-25s: %s\n", "Complete Profile", (ele.complete_profile == true)?"Yes":"No");
	if (ele.mac_addr_present)
		PHL_INFO("%-25s: %2x:%2x:%2x:%2x:%2x:%2x\n",
		         "MAC address",
		         ele.mac_addr[0],
		         ele.mac_addr[1],
		         ele.mac_addr[2],
		         ele.mac_addr[3],
		         ele.mac_addr[4],
		         ele.mac_addr[5]
		         );
	else
		PHL_INFO("%-25s: Not present\n", "MAC address");

	if (ele.bcn_interval_present)
		PHL_INFO("%-25s: %d\n", "Beacon Interval", ele.bcn_interval);
	else
		PHL_INFO("%-25s: Not present\n", "Beacon Interval");

	if (ele.dtim_info_present) {
		PHL_INFO("%-25s: %d\n", "DTIM Count", ele.dtim_cnt);
		PHL_INFO("%-25s: %d\n", "DTIM Period", ele.dtim_period);
	} else {
		PHL_INFO("%-25s: Not present\n", "DTIM Info");
	}

	if (ele.nstr_link_pair_present) {
		PHL_INFO("%-25s: %d\n", "NSTR Bitmap Size", ele.nstr_bitmap_size);
		PHL_INFO("%-25s: 0x%X\n", "NSTR Indication Bitmap", ele.nstr_indication_bitmap);
	} else {
		PHL_INFO("%-25s: Not present\n", "NSTR Link Pair");
	}

	if (ele.sta_profile_len != 0) {
		PHL_INFO("%-25s: %d\n", "STA Profile Length", ele.sta_profile_len);
		PHL_INFO("%-25s: %s\n", "STA Profile Fragment", (ele.sta_profile_frag_len == 0)?"No":"Yes");
	}
}

/*
 *
 * +--------+--------+----------+----------+-------------+
 * | Subele | Length | STA Ctrl | STA Info | STA Profile |
 * | ID     |        |          |          |             |
 * +--------+--------+----------+----------+-------------+
 *                           |
 *                           |
 *            ele_len        v
 *              ^
 *              |      len_before_frag   1     1      len_before_frag - ele_len
 * +--------+---+----+-----------------+-----+------+-----------------+-------------+
 * | Subele | Length | Data 0          | FID | FLEN | DATA 1                        |
 * | ID     |        |                 |     |      |                               |
 * +--------+--------------------------+-----+------+-------------------------------+
 *                   |
 *                   v
 *                   ele_pos
 */
void
phl_parse_per_sta_profile_ie(struct rtw_phl_com_t *phl_com,
                             u8 *ele_pos,
                             u16 ele_len,
                             u8 *ele_frag,
                             u16 len_before_frag,
                             struct rtw_phl_per_sta_profile_element *ele
)
{
	void *d = phlcom_to_drvpriv(phl_com);
	u8 tmp[STA_CTRL_LEN + MAX_STA_INFO_LEN] = {0};
	u8 *sta_ctrl = tmp;
	u8 *sta_info = tmp+2;
	u8 sta_info_len = 0;
	u8 sta_info_offset = 0;

	/* Copy STA Ctrl and STA Info field to tmp for parsing */
	if (len_before_frag == 0) {
		/* Per-STA profile without fragment */
		sta_info_len = ele_pos[STA_CTRL_LEN];
		_os_mem_cpy(d, tmp, ele_pos, (STA_CTRL_LEN + sta_info_len));
	} else {
		/* Per-STA profile with fragment */
		if (len_before_frag <= STA_CTRL_LEN)
			sta_info_len = ele_frag[STA_CTRL_LEN - len_before_frag];
		else
			sta_info_len = ele_pos[STA_CTRL_LEN];

		if (len_before_frag < (STA_CTRL_LEN + sta_info_len)) {
			_os_mem_cpy(d, tmp, ele_pos, len_before_frag);
			_os_mem_cpy(d,
			            (tmp + len_before_frag),
			            ele_frag,
			            (STA_CTRL_LEN + sta_info_len - len_before_frag));
		} else {
			_os_mem_cpy(d, tmp, ele_pos, (STA_CTRL_LEN + sta_info_len));
		}
	}

	sta_info_offset++;

	ele->link_id = (u8)GET_STA_CTRL_LINK_ID(sta_ctrl);

	if (GET_STA_CTRL_COMPLETE_PROFILE(sta_ctrl)) {
		ele->complete_profile = true;
	}

	if (GET_STA_CTRL_MAC_ADDR_PRESENT(sta_ctrl)) {
		ele->mac_addr_present = true;
		_os_mem_cpy(d, ele->mac_addr, (sta_info+sta_info_offset), MAC_ALEN);
		sta_info_offset += 6;
	}

	if (GET_STA_CTRL_BEACON_INTEREVAL_PRESENT(sta_ctrl)) {
		ele->bcn_interval_present = true;
		ele->bcn_interval = LE_BITS_TO_2BYTE(sta_info+sta_info_offset, 0, 16);
		sta_info_offset += 2;
	}

	if (GET_STA_CTRL_DTIM_INFO_PRESENT(sta_ctrl)) {
		ele->dtim_info_present = true;
		ele->dtim_cnt = LE_BITS_TO_1BYTE(sta_info+sta_info_offset, 0, 8);
		sta_info_offset += 1;
		ele->dtim_period = LE_BITS_TO_1BYTE(sta_info+sta_info_offset, 0, 8);
		sta_info_offset += 1;
	}

	if (GET_STA_CTRL_NSTR_LINK_PAIR_PRESENT(sta_ctrl)) {
		ele->nstr_link_pair_present = true;
		if (GET_STA_CTRL_NSTR_BITMAP_SIZE(sta_ctrl)) {
			ele->nstr_indication_bitmap = LE_BITS_TO_2BYTE(sta_info+sta_info_offset, 0, 16);
			ele->nstr_bitmap_size = 2;
			sta_info_offset += 2;
		} else {
			ele->nstr_indication_bitmap = LE_BITS_TO_1BYTE(sta_info+sta_info_offset, 0, 8);
			ele->nstr_bitmap_size = 1;
			sta_info_offset += 1;
		}
	}

	if (len_before_frag == 0) {
		/* Per-STA profile without fragment */
		ele->sta_profile = ele_pos + STA_CTRL_LEN + sta_info_len;
	} else if (len_before_frag <= (STA_CTRL_LEN + sta_info_len)) {
		/* STA profile located in fragment part */
		ele->sta_profile = ele_frag + (STA_CTRL_LEN + sta_info_len - len_before_frag + 2);
	} else {
		/* STA profile is truncated by fragment */
		ele->sta_profile = ele_pos + STA_CTRL_LEN + sta_info_len;
		ele->sta_profile_frag = ele_frag + 2;
		ele->sta_profile_frag_len = (u8)(len_before_frag - (STA_CTRL_LEN + sta_info_len));
	}
	ele->sta_profile_len = (u8)(ele_len - STA_CTRL_LEN - sta_info_len);

	_dump_per_sta_profile(*ele);
}

void
_parse_ml_link_info(struct rtw_phl_com_t *phl_com,
                    u8 *ie_buf,
                    u16 ie_len,
                    u16 link_info_offset,
                    struct rtw_phl_ml_element *ml_ele
)
{
	u16 offset = link_info_offset;
	u8 sub_eid = 0;
	u8 *sub_ele = NULL, *sub_ele_frag = NULL;
	u16 sub_ele_len = 0, sub_ele_len_before_frag = 0;
	u16 len_before_frag = 0;
	/* Exclude the Element ID extension field */
	u16 next_frag_offset = MAX_ELE_LEN - 1;

	do {
		if (offset >= (ie_len-1))
			break;

		sub_ele_frag = NULL;
		sub_ele_len_before_frag = 0;
		len_before_frag = 0;

		/* Check fragment */
		len_before_frag = next_frag_offset - offset;
		if (len_before_frag == 1) {
			sub_eid = ie_buf[offset];
			/* Skip FID and FLEN */
			sub_ele_len = ie_buf[offset+3];
			sub_ele = ie_buf + offset + 4;
		} else if (len_before_frag == 2){
			sub_eid = ie_buf[offset];
			sub_ele_len = ie_buf[offset+1];
			/* Skip FID and FLEN */
			sub_ele = ie_buf + offset + 4;
		} else {
			sub_eid = ie_buf[offset];
			sub_ele_len = ie_buf[offset+1];
			sub_ele = ie_buf + offset + 2;
			if (len_before_frag < (sub_ele_len + 2)) {
				sub_ele_frag = ie_buf + offset + len_before_frag + 2;
				sub_ele_len_before_frag = len_before_frag - 2;
			}
		}
		/*
		 * Next fragment offset would be
		 * 255: Max element length
		 * 1: Fragment ID field
		 * 1: Fragment Length field
		 */
		if (len_before_frag <= (sub_ele_len + 2))
			next_frag_offset += (MAX_ELE_LEN + 2);

		if (sub_eid == WLAN_SUBEID_PER_STA_PROFILE) {
			phl_parse_per_sta_profile_ie(phl_com,
			                             sub_ele,
			                             sub_ele_len,
			                             sub_ele_frag,
			                             sub_ele_len_before_frag,
			                             &(ml_ele->profile[ml_ele->profile_num]));
			ml_ele->profile_num++;
		}

		/* Move offset to the end of the element */
		if (len_before_frag <= (sub_ele_len + 2)) {
			/*
			 * Subele ID field (1) +
			 * Subele len field (1) +
			 * Fragment ID field (1) +
			 * Fragment len field (1) +
			 * Subele length
			 */
			offset += (sub_ele_len + 2 + 2);
		} else {
			/*
			 * Subele ID field (1) +
			 * Subele len field (1) +
			 * Subele length
			 */
			offset += (sub_ele_len + 2);
		}
	} while(1);
}

void
_dump_ml_basic(struct rtw_phl_ml_element ml_ele)
{
	PHL_INFO("###### _dump_ml_basic #######\n");
	if (ml_ele.common_info.basic_ml.link_id_info_present)
		PHL_INFO("%-25s: %d\n", "Link ID", ml_ele.common_info.basic_ml.link_id);
	else
		PHL_INFO("%-25s: Not present\n", "Link ID");
	if (ml_ele.common_info.basic_ml.bss_param_chg_cnt_present)
		PHL_INFO("%-25s: %d\n", "BSS Param Chg Cnt", ml_ele.common_info.basic_ml.bss_param_chg_cnt);
	else
		PHL_INFO("%-25s: Not present\n", "BSS Param Chg Cnt");
	if (ml_ele.common_info.basic_ml.msd_info_present)
		PHL_INFO("%-25s: Present\n", "MSD Info");
	else
		PHL_INFO("%-25s: Not present\n", "MSD Info");
	if (ml_ele.common_info.basic_ml.eml_cap_present)
		PHL_INFO("%-25s: Present\n", "EML Capability");
	else
		PHL_INFO("%-25s: Not present\n", "EML Capability");
	if (ml_ele.common_info.basic_ml.mld_cap_present) {
		PHL_INFO("%-25s =>\n", "MLD Capability");
		PHL_INFO("%-25s: %d\n", "Max Num of SL",
		         ml_ele.common_info.basic_ml.mld_cap.max_num_sl);
		PHL_INFO("%-25s: %s\n", "SRS Support",
		         (ml_ele.common_info.basic_ml.mld_cap.srs_support == true)?"True":"False");
		if (ml_ele.common_info.basic_ml.mld_cap.tid_to_link_nego_support == 0)
			PHL_INFO("%-25s: %s\n",
			         "TID-To-Link Mapping Nego",
			         "Not support");
		else if (ml_ele.common_info.basic_ml.mld_cap.tid_to_link_nego_support == 1)
			PHL_INFO("%-25s: %s\n",
			         "TID-To-Link Mapping Nego",
			         "Same or different link set");
		else
			PHL_INFO("%-25s: %s\n",
			         "TID-To-Link Mapping Nego",
			         "Same link set only");
		if (ml_ele.common_info.basic_ml.mld_cap.freq_sep_for_str)
			PHL_INFO("%-25s: %dMHz\n", "Freq Separation for STR",
			         ((ml_ele.common_info.basic_ml.mld_cap.freq_sep_for_str - 1) * 80));
		PHL_INFO("%-25s: %s\n", "AAR Support",
		         (ml_ele.common_info.basic_ml.mld_cap.aar_support == true)?"True":"False");
	} else {
		PHL_INFO("%-25s: Not present\n", "MLD Capability");
	}
}

void
_parse_ml_basic(struct rtw_phl_com_t *phl_com,
                u8 *ie_buf,
                u16 ie_len,
                struct rtw_phl_ml_element *ml_ele
)
{
	struct basic_ml *basic_ml = &(ml_ele->common_info.basic_ml);
	u8 *ml_ctrl = ie_buf;
	u8 *common_info = ie_buf+2;
	u8 common_info_len = 0;
	u8 common_info_offset = 0;

	common_info_len = common_info[0];
	common_info_offset++;

	basic_ml->mld_address = common_info+1;
	common_info_offset += 6;

	if (GET_ML_ELE_ML_CTRL_LINK_ID_INFO_PRESENT(ml_ctrl)) {
		basic_ml->link_id_info_present = true;
		basic_ml->link_id = LE_BITS_TO_1BYTE(common_info+common_info_offset, 0, 4);
		common_info_offset += 1;
	}

	if (GET_ML_ELE_ML_CTRL_BSS_PARAMS_CHG_CNT_PRESENT(ml_ctrl)) {
		basic_ml->bss_param_chg_cnt_present = true;
		basic_ml->bss_param_chg_cnt = LE_BITS_TO_1BYTE(common_info+common_info_offset, 0, 8);
		common_info_offset += 1;
	}

	if (GET_ML_ELE_ML_CTRL_MEDIUM_SYNC_DELAY_INFO_PRESENT(ml_ctrl)) {
		basic_ml->msd_info_present = true;
		/* TODO: Parse MSD INFO */
		common_info_offset += 2;
	}

	if (GET_ML_ELE_ML_CTRL_EML_CAP_PRESENT(ml_ctrl)) {
		basic_ml->eml_cap_present = true;
		/* TODO: Parse EML cap */
		common_info_offset += 3;
	}

	if (GET_ML_ELE_ML_CTRL_MLD_CAP_PRESENT(ml_ctrl)) {
		basic_ml->mld_cap_present = true;
		basic_ml->mld_cap.max_num_sl = GET_MLD_CAP_MAX_NUM_OF_SL(common_info+common_info_offset);
		basic_ml->mld_cap.srs_support = GET_MLD_CAP_SRS_SUPPORT(common_info+common_info_offset);
		basic_ml->mld_cap.tid_to_link_nego_support = GET_MLD_CAP_TID_TO_LINK_NEGO_SUPPORT(common_info+common_info_offset);
		basic_ml->mld_cap.freq_sep_for_str = GET_MLD_CAP_FREQ_SEP_FOR_STR(common_info+common_info_offset);
		basic_ml->mld_cap.aar_support = GET_MLD_CAP_AAR_SUPPORT(common_info+common_info_offset);
		common_info_offset += 2;
	}

	_dump_ml_basic(*ml_ele);

	_parse_ml_link_info(phl_com,
	                    ie_buf,
	                    ie_len,
	                    (2 + common_info_len),
	                    ml_ele);
}

void
_dump_ml_probe_req(struct rtw_phl_ml_element ml_ele)
{
	PHL_INFO("###### _dump_ml_probe_req #######\n");
	if (ml_ele.common_info.probe_req_ml.mld_id_present)
		PHL_INFO("%-25s: %d\n", "MLD ID", ml_ele.common_info.probe_req_ml.mld_id);
	else
		PHL_INFO("%-25s: Not present\n", "MLD ID");
}

void
_parse_ml_probe_req(struct rtw_phl_com_t *phl_com,
                    u8 *ie_buf,
                    u16 ie_len,
                    struct rtw_phl_ml_element *ml_ele
)
{
	struct probe_req_ml *probe_req_ml = &(ml_ele->common_info.probe_req_ml);
	u8 *ml_ctrl = ie_buf;
	u8 *common_info = ie_buf+2;
	u8 common_info_len = 0;
	u8 common_info_offset = 0;

	common_info_len = common_info[0];
	common_info_offset++;

	if (GET_ML_ELE_ML_CTRL_MLD_ID_PRESENT(ml_ctrl)) {
		probe_req_ml->mld_id_present = true;
		probe_req_ml->mld_id = LE_BITS_TO_1BYTE(common_info+common_info_offset, 0, 8);
		common_info_offset += 1;
	}

	_dump_ml_probe_req(*ml_ele);

	_parse_ml_link_info(phl_com,
	                    ie_buf,
	                    ie_len,
	                    (2 + common_info_len),
	                    ml_ele);
}

void
rtw_phl_parse_ml_ie(struct rtw_phl_com_t *phl_com,
                    u8 *ele_pos,
                    u16 ele_len,
                    struct rtw_phl_ml_element *ml_ele
)
{
	if ((ele_pos == NULL) || (ele_len == 0))
		return;

	if (ml_ele == NULL)
		return;

	ml_ele->type = GET_ML_ELE_ML_CTRL_TYPE(ele_pos);

	if (ml_ele->type == BASIC_ML) {
		_parse_ml_basic(phl_com, ele_pos, ele_len, ml_ele);
	} else if  (ml_ele->type == PROBE_REQUEST_ML) {
		_parse_ml_probe_req(phl_com, ele_pos, ele_len, ml_ele);
	} else {
		PHL_WARN("Unknown type!\n");
	}
}
/*
 * rtw_phl_get_ie: Return the total length of the element (include the fragment
 * element)
 *
 * Ex. Element fragmentation without Element ID Extension
 *                ele_len = 255 + 1 + 1 + 255 + 1 + 1 +n
 *             |------------------------------------------|
 *             |                                          |
 *             v 255    1     1     255    1     1   n    v
 * +------------------------------------------------------+
 * | EID | 255 | Data | FID | 255 | Data | FID | n | Data |
 * +------------------------------------------------------+
 *             ^
 *             |
 *             out_ele
 * Ex. Element fragmentation with Element ID Extension
 *                       ele_len = 254 + 1 + 1 + 255 + 1 + 1 +n
 *                   +----------------+---+---+-----+---+-------+
 *                   v                                          v
 *               1     254    1     1     255    1     1   n
 * +-----+-----+-----+------+-----+-----+------+-----+---+------+
 * | EID | 255 | EXT | Data | FID | 255 | Data | FID | n | Data |
 * +-----+-----+-----+------+-----+-----+------+-----+---+------+
 *                   ^
 *                   +
 *                   out_ele
 */
u16
rtw_phl_get_ie(u8 *ie_start,
               u16 ies_len,
               u8 target_id,
               u8 target_ext_id,
               u8 **out_ele
)
{
	u16 offset = 0, frag_offset = 0;
	u16 ele_len = 0;
	u8 tmp_id = 0;
	u8 tmp_ext_id = 0;
	u16 tmp_ele_len = 0;
	u8 frag_len = 0;
	bool is_fragmentable = false;

	do {
		if ((offset + 2) >= ies_len)
			break;

		/* Get current element ID */
		tmp_id = ie_start[offset];
		tmp_ele_len = ie_start[offset+1];
		if (tmp_id == EID_EXTENSION)
			tmp_ext_id = ie_start[offset+2];
		is_fragmentable = rtw_phl_is_ie_fragmentable(tmp_id, tmp_ext_id);
		if (is_fragmentable) {
			frag_offset = offset + 2 + tmp_ele_len;
			frag_len = (u8)tmp_ele_len;
			while (frag_len == 255) {
				/*
				 * Check there is more data at the end of the IE
				 * and is followed by fragment element
				 */
				if (((frag_offset + 2) < ies_len) &&
				    (ie_start[frag_offset] == EID_FRAGMENT)){
					frag_len = ie_start[frag_offset+1];
					frag_offset += (2+frag_len);
					tmp_ele_len += (2+frag_len);
				}
			}
		}

		/* Check element length is valid (ele length + N * frag length) */
		if ((offset + 2 + tmp_ele_len) > ies_len) {
			PHL_WARN("%s: Get invalid length!\n", __func__);
			return 0;
		}
		if (target_id == tmp_id) {
			if (target_id == EID_EXTENSION) {
				/* Check Extension ID */
				if (target_ext_id != tmp_ext_id) {
					/* Extension ID is different */
					offset += (tmp_ele_len + 2);
					continue;
				}
				/* Return pointer of the first byte after extension ID */
				*out_ele = ie_start + offset + 3;
				/* Return length - extension id field (1) */
				ele_len = tmp_ele_len - 1;
				break;
			}
			/* Return pointer of the first byte after lenght field */
			*out_ele = ie_start + offset + 2;
			ele_len = tmp_ele_len;
			break;
		} else {
			/* Element ID is different */
			offset += (tmp_ele_len + 2);
		}
	} while(1);

	return ele_len;
}

bool
rtw_phl_is_ie_fragmentable(u32 eid,
                           u32 eid_ext
)
{
	bool ret = false;

	if (eid == EID_FILS_INDICATION)
		ret = true;
	else if (eid == EID_EXTENSION) {
		if ((eid_ext == EID_EXT_FILS_KEY_CONFIRM) ||
		    (eid_ext == EID_EXT_FILS_HLP_CONTAINER) ||
		    (eid_ext == EID_EXT_KEY_DELIVERY) ||
		    (eid_ext == EID_EXT_FILS_WRAPPED_DATA) ||
		    (eid_ext == EID_EXT_FILS_PUBLIC_KEY) ||
		    (eid_ext == EID_EXT_CDMG_EXTEND_SCHEDULE) ||
		    (eid_ext == EID_EXT_SSW_REPORT) ||
		    (eid_ext == EID_EXT_SPSH_REPORT) ||
		    (eid_ext == EID_EXT_GAS_EXTENSION) ||
		    (eid_ext == EID_EXT_MULTI_LINK) ||
		    (eid_ext == EID_EXT_TID_TO_LINK_MAPPING))
			ret = true;
	}

	return ret;
}

/*
 * Currently, the reported APs in Reduced Neighbor Report are
 * affiliated with the same MLD as the reported AP. Therefore,
 * MLD ID would be 0.
 */
u8 _build_mld_parameters(void *phl,
                         struct rtw_wifi_role_link_t *rlink,
                         u8 *pbuf)
{
	struct rtw_phl_stainfo_t *sta = rtw_phl_get_stainfo_self(phl, rlink);
	u8 *p = pbuf;

	SET_MLD_PARAMS_MLD_ID(p, 0);
	SET_MLD_PARAMS_LINK_ID(p, sta->link_id); /* = rlink->id for AP mode */
	SET_MLD_PARAMS_BSS_PARAMS_CHG_CNT(p, rlink->bss_params_chg_cnt);

	return 3;
}

/*
 * Currently, the reported APs in Reduced Neighbor Report are
 * affiliated with the same MLD as the reported AP.
 */
u8 rtw_phl_build_reduced_nb_rpt(struct rtw_wifi_role_t *wrole,
                                struct rtw_wifi_role_link_t *rlink,
                                u8 *pbuf)
{
	void *phl = wrole->phl_com->phl_priv;
	void *drv = wrole->phl_com->drv_priv;
	u8 tbtt_info_cnt;
	u8 tbtt_info_len;
	u8 *pstart = pbuf;
	u8 *p = pbuf + 2;
	struct rtw_phl_mld_t *mld = rtw_phl_get_mld_self(phl, wrole);
	struct rtw_wifi_role_link_t *another_rlink;
	u8 lidx;

	/* only support MLD currently */
	if (mld == NULL || mld->type != DEV_TYPE_MLD || wrole->rlink_num == 1)
		return 0;

	tbtt_info_cnt = 0;
	tbtt_info_len = 16;

	for (lidx = 0; lidx < wrole->rlink_num; lidx++) {
		another_rlink = get_rlink(wrole, lidx);
		if (another_rlink == rlink)
			continue;

		/* TBTT Information Header */
		SET_TBTT_INFO_FIELD_TYPE(p, 0);
		SET_FILTED_NB_AP(p, 0);
		SET_TBTT_INFO_CNT(p, tbtt_info_cnt);
		SET_TBTT_INFO_LEN(p, tbtt_info_len);
		p += 2;

		/* Operating Class */
		*p++ = rtw_phl_get_operating_class(another_rlink->chandef);
		/* Channel Number */
		*p++ = another_rlink->chandef.chan;

		/* TBTT Information Set */
		*p++ = 0;      /* Neighbor AP TBTT Offset */
		_os_mem_cpy(drv, p, another_rlink->mac_addr, ETH_ALEN); /* BSSID */
		p += ETH_ALEN;
		p += 4;  /* TODO: Short SSID */
		p += 1;  /* TODO: BSS Parameters */
		p += 1;  /* TODO: 20 MHz PSD */
		p += _build_mld_parameters(phl, another_rlink, p);
	}

	*pstart = EID_REDUCED_NEIGHBOR_REPORT;
	*(pstart + 1) = (u8)(p - pstart - 2);

	return (u8)(p - pstart);
}

void _dump_reduced_nb_rpt(struct rtw_phl_rnb_rpt_element *reduced_nb_rpt)
{
	struct tbtt_info_header *hdr;
	struct rtw_phl_tbtt_info *tbtt_info;
	u8 i, j;

	PHL_INFO("###### _dump_reduced_nb_rpt #######\n");
	for (i = 0; i < reduced_nb_rpt->nb_ap_num; i++) {
		PHL_INFO("%s - %d\n", "Neighbor AP", i);
		hdr = &reduced_nb_rpt->nb_aps[i].tbtt_info_hdr;

		if (!hdr->is_legal) {
			PHL_INFO("%-25s %d %s\n", "Length", hdr->len, "is unrecognized");
			continue;
		}

		PHL_INFO("%-25s: %d\n", "Info Field Type", hdr->type);
		PHL_INFO("%-25s: %s\n", "Filtered Neighbor AP",
			(hdr->filtered_nb_ap == true ? "True" : "False"));
		PHL_INFO("%-25s: %d\n", "TBTT Info Count", hdr->cnt);
		PHL_INFO("%-25s: %d\n", "TBTT Info Length", hdr->len);
		PHL_INFO("%-25s: %d\n", "Operating Class", reduced_nb_rpt->nb_aps[i].op_class);
		PHL_INFO("%-25s: %d\n", "Channel", reduced_nb_rpt->nb_aps[i].ch);

		for (j = 0; j < hdr->cnt + 1; j++) {
			tbtt_info = &reduced_nb_rpt->nb_aps[i].tbtt_infos[j];
			PHL_INFO("%s - %d\n", "TBTT Info", j);
			PHL_INFO("%-25s: %d\n", "Neighbor AP Offset", tbtt_info->offset);

			if (hdr->bssid_is_present)
				PHL_INFO("%-25s: %02x:%02x:%02x:%02x:%02x:%02x\n", "BSSID",
					tbtt_info->bssid[0], tbtt_info->bssid[1],
					tbtt_info->bssid[2], tbtt_info->bssid[3],
					tbtt_info->bssid[4], tbtt_info->bssid[5]);
			else
				PHL_INFO("%-25s: Not present\n", "BSSID");

			if (hdr->short_ssid_is_present)
				PHL_INFO("%-25s: %s\n", "Short SSID", (char *)tbtt_info->short_ssid);
			else
				PHL_INFO("%-25s: Not present\n", "Short SSID");

			if (hdr->bss_param_is_present) {
				PHL_INFO("%-25s: Present\n", "BSS Parameters");
				PHL_INFO("%-25s: %s\n", "- OCT recommended",
						(tbtt_info->bss_param.oct_recomm == true ? "True" : "False"));
				PHL_INFO("%-25s: %s\n", "- Same SSID",
						(tbtt_info->bss_param.same_ssid == true ? "True" : "False"));
				PHL_INFO("%-25s: %s\n", "- Multiple BSSID",
						(tbtt_info->bss_param.multi_bssid == true ? "True" : "False"));
				PHL_INFO("%-25s: %s\n", "- Transmitted BSSID",
						(tbtt_info->bss_param.transmitted_bssid == true ? "True" : "False"));
				PHL_INFO("%-25s: %s\n", "- 24G/5G colocated AP",
						(tbtt_info->bss_param.mem_24G_5G_colated_ap == true ? "True" : "False"));
				PHL_INFO("%-25s: %s\n", "- Unsolicited pbrsp active",
						(tbtt_info->bss_param.unsolicited_probe_resp_act == true ? "True" : "False"));
				PHL_INFO("%-25s: %s\n", "- Colocated AP",
						(tbtt_info->bss_param.colated_ap == true ? "True" : "False"));
			} else
				PHL_INFO("%-25s: Not present\n", "BSS Parameters");

			if (hdr->max_tx_pwr_is_present)
				PHL_INFO("%-25s: %d\n", "20MHz PSD", tbtt_info->max_tx_pwr);
			else
				PHL_INFO("%-25s: Not present\n", "20MHz PSD");

			if (hdr->mld_param_is_present) {
				PHL_INFO("%-25s: Present\n", "MLD Parameters");
				PHL_INFO("%-25s: %d\n", "- MLD ID", tbtt_info->mld_param.mld_id);
				PHL_INFO("%-25s: %d\n", "- Link ID", tbtt_info->mld_param.link_id);
				PHL_INFO("%-25s: %d\n", "- BSS Param Chg Cnt",
						tbtt_info->mld_param.bss_params_chg_cnt);
			} else
				PHL_INFO("%-25s: Not present\n", "MLD Parameters");
		}
	}
	PHL_INFO("###### _dump_reduced_nb_rpt #######\n");
}

u8 _parse_tbtt_header(struct rtw_phl_com_t *phl_com,
                      u8 *pos_start,
                      struct tbtt_info_header *hdr)
{
	u8 *pos = pos_start;

	hdr->type = GET_TBTT_INFO_FIELD_TYPE(pos);
	hdr->filtered_nb_ap = GET_FILTED_NB_AP(pos);
	hdr->cnt = GET_TBTT_INFO_CNT(pos);
	hdr->len = GET_TBTT_INFO_LEN(pos);

	if (hdr->len < 1 || hdr->len == 3) {
		hdr->is_legal = false;
		pos += hdr->len * hdr->cnt;
		goto exit;
	}

	hdr->is_legal = true;
	pos += 2;

	if (hdr->len >= 7)
		hdr->bssid_is_present = true;
	if (hdr->len == 5 || hdr->len == 6 || hdr->len >= 11)
		hdr->short_ssid_is_present = true;
	if (hdr->len == 2 || hdr->len == 6 || hdr->len == 8 || hdr->len == 9 || hdr->len >= 12)
		hdr->bss_param_is_present = true;
	if (hdr->len == 9 || hdr->len >= 13)
		hdr->max_tx_pwr_is_present = true;
	if (hdr->len == 4 || hdr->len == 10 || hdr->len >= 16)
		hdr->mld_param_is_present = true;

exit:
	return (u8)(pos - pos_start);

}

u8 _parse_nb_info(struct rtw_phl_com_t *phl_com,
                  u8 *pos_start,
                  struct rtw_phl_neighbor_ap *nb_ap)
{
	void *drv = phl_com->drv_priv;
	u8 *pos = pos_start;
	u8 *pos_tbtt_info;
	struct tbtt_info_header *hdr = &nb_ap->tbtt_info_hdr;
	struct rtw_phl_tbtt_info *tbtt_info;
	u8 i = 0;

	/* TBTT Information Header */
	pos += _parse_tbtt_header(phl_com, pos, hdr);

	if (!hdr->is_legal)
		goto exit;

	nb_ap->op_class = *pos++;
	nb_ap->ch = *pos++;

	/* translate to chan_def */
	if (!rtw_phl_get_chandef_from_operating_class(nb_ap->ch,
		nb_ap->op_class, &nb_ap->chan_def)) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_,
			"%s: getting channel definition failed !!! \n", __func__);
	}

	/* TBTT Information Set */
	do {
		pos_tbtt_info = pos;
		tbtt_info = &nb_ap->tbtt_infos[i];

		/* TBTT Offset*/
		tbtt_info->offset = *pos++;

		/* BSSID */
		if (hdr->bssid_is_present) {
			_os_mem_cpy(drv, tbtt_info->bssid, pos, MAC_ALEN);
			pos += MAC_ALEN;
		}

		/* Short SSID */
		if (hdr->short_ssid_is_present) {
			_os_mem_cpy(drv, tbtt_info->short_ssid, pos, 4);
			pos += 4;
		}

		/* BSS Parameters */
		if (hdr->bss_param_is_present) {
			tbtt_info->bss_param.oct_recomm =
				GET_BSS_PARAMS_OCT_RECOMM(pos);
			tbtt_info->bss_param.same_ssid =
				GET_BSS_PARAMS_SAME_SSID(pos);
			tbtt_info->bss_param.multi_bssid =
				GET_BSS_PARAMS_MULTI_BSSID(pos);
			tbtt_info->bss_param.transmitted_bssid =
				GET_BSS_PARAMS_TRANSMITTED_BSSID(pos);
			tbtt_info->bss_param.mem_24G_5G_colated_ap =
				GET_BSS_PARAMS_MEM_24G_5G_COLOCATED_AP(pos);
			tbtt_info->bss_param.unsolicited_probe_resp_act =
				GET_BSS_PARAMS_UNSOLICITED_PROBE_RESP_ACTIVE(pos);
			tbtt_info->bss_param.colated_ap =
				GET_BSS_PARAMS_COLOACTED_AP(pos);
			pos += 1;
		}

		/* 20Mhz psd */
		if (hdr->max_tx_pwr_is_present)
			tbtt_info->max_tx_pwr = *pos++;

		/* MLD parameters */
		if (hdr->mld_param_is_present) {
			tbtt_info->mld_param.mld_id = GET_MLD_PARAMS_MLD_ID(pos);
			tbtt_info->mld_param.link_id = GET_MLD_PARAMS_LINK_ID(pos);
			tbtt_info->mld_param.bss_params_chg_cnt = GET_MLD_PARAMS_BSS_PARAMS_CHG_CNT(pos);
			pos += 3;
		}

		/* ignore the rest reserved fields */
		pos = pos_tbtt_info + hdr->len;
		i++;

	} while (i < hdr->cnt + 1 && i < MAX_TBTT_INFO_NUM);

exit:
	return (u8)(pos - pos_start);
}

void rtw_phl_parse_reduced_nb_rpt(struct rtw_phl_com_t *phl_com,
                                  u8 *ele_start,
                                  u16 ele_len,
                                  struct rtw_phl_rnb_rpt_element *reduced_nb_rpt)
{
	u8 *ele_pos = NULL;

	if ((ele_start == NULL) || (ele_len == 0))
		return;

	if (reduced_nb_rpt == NULL)
		return;

	ele_pos = ele_start;

	do {
		if (reduced_nb_rpt->nb_ap_num >= MAX_NEIGHBOR_AP_NUM)
			break;

		ele_pos += _parse_nb_info(phl_com, ele_pos, &reduced_nb_rpt->nb_aps[reduced_nb_rpt->nb_ap_num]);
		reduced_nb_rpt->nb_ap_num++;
	} while(ele_pos < (ele_start + ele_len));

}


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
#define _PHL_WOW_C_
#include "phl_headers.h"

enum rtw_phl_status phl_wow_mdl_init(struct phl_info_t* phl_info)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_SUCCESS;
#ifdef CONFIG_WOWLAN
	struct phl_wow_info *info = phl_to_wow_info(phl_info);

	info->phl_info = phl_info;
	_os_spinlock_init(phl_to_drvpriv(phl_info), &info->wow_lock);
#endif /* CONFIG_WOWLAN */

	return pstatus;
}

void phl_wow_mdl_deinit(struct phl_info_t* phl_info)
{
#ifdef CONFIG_WOWLAN
	struct phl_wow_info *info = phl_to_wow_info(phl_info);

	_os_spinlock_free(phl_to_drvpriv(phl_info), &info->wow_lock);
#endif /* CONFIG_WOWLAN */
}

#ifdef CONFIG_WOWLAN

u8 _phl_chk_hw_form_sechdr(struct dev_cap_t *dev_cap, u8 pairwise_algo)
{
	u8 ret = false;

	if (dev_cap->sec_cap.hw_form_hdr) {

		switch (pairwise_algo) {
		case RTW_ENC_WEP40:
		case RTW_ENC_WEP104:
		case RTW_ENC_TKIP:
		case RTW_ENC_CCMP:
		case RTW_ENC_CCMP256:
		case RTW_ENC_GCMP:
		case RTW_ENC_GCMP256:
			ret = true;
			break;
		case RTW_ENC_WAPI:
		case RTW_ENC_GCMSMS4:
			ret = false;
			break;
		default:
			break;
		}
	}

	return ret;
}

/* TO-DO: Confirm the enum strcut of the algo */
u8 _phl_query_iv_len(u8 algo)
{
	u8 len = 0;

	switch(algo) {
	case RTW_ENC_WEP40:
		len = 4;
		break;
	case RTW_ENC_TKIP:
	case RTW_ENC_CCMP:
	case RTW_ENC_GCMP256:
		len = 8;
		break;
	default:
		len = 0;
		break;
	}

	return len;
}

u8 _phl_query_key_desc_ver(struct phl_wow_info *wow_info, u8 algo)
{
	u8 akm_type = wow_info->gtk_ofld_info.akmtype_byte3;

	if (algo == RTW_ENC_TKIP)
		return EAPOLKEY_KEYDESC_VER_1;

	if (akm_type == 1 || akm_type == 2) {
		return EAPOLKEY_KEYDESC_VER_2;
	} else if (akm_type > 2 && akm_type < 7) {
		return EAPOLKEY_KEYDESC_VER_3;
	} else {
		return 0;
	}
}

static void _phl_cfg_pkt_ofld_null_info(
	struct phl_wow_info *wow_info,
	struct rtw_phl_stainfo_t *phl_sta,
	struct rtw_pkt_ofld_null_info *null_info)
{
	void *drv_priv = phl_to_drvpriv(wow_info->phl_info);

	_os_mem_cpy(drv_priv, &(null_info->a1[0]), &(phl_sta->mac_addr[0]), MAC_ADDRESS_LENGTH);
	_os_mem_cpy(drv_priv, &(null_info->a2[0]), &(phl_sta->wrole->mac_addr[0]), MAC_ADDRESS_LENGTH);
	_os_mem_cpy(drv_priv, &(null_info->a3[0]), &(phl_sta->mac_addr[0]), MAC_ADDRESS_LENGTH);

}

static void
_phl_cfg_pkt_ofld_probe_req_info(struct phl_wow_info *wow_info,
	struct rtw_phl_stainfo_t *phl_sta,
	struct rtw_pkt_ofld_probe_req_info *probe_req_info)
{
	void *drv_priv = phl_to_drvpriv(wow_info->phl_info);

	if (wow_info->nlo_info.construct_pbreq == NULL) {
		_os_mem_cpy(drv_priv, &(probe_req_info->a2[0]),
			&(phl_sta->wrole->mac_addr[0]), MAC_ADDRESS_LENGTH);
	} else {
		probe_req_info->construct_pbreq = wow_info->nlo_info.construct_pbreq;
	}
}

static void _phl_cfg_pkt_ofld_arp_rsp_info(struct phl_wow_info *wow_info, struct rtw_phl_stainfo_t *phl_sta,
						struct rtw_pkt_ofld_arp_rsp_info *arp_rsp_info)
{
	void *drv_priv = phl_to_drvpriv(wow_info->phl_info);
	u8 pairwise_algo = get_wow_pairwise_algo_type(wow_info);
	struct dev_cap_t *dev_cap = &(wow_info->phl_info->phl_com->dev_cap);

	_os_mem_cpy(drv_priv, &(arp_rsp_info->a1[0]), &(phl_sta->mac_addr[0]), MAC_ADDRESS_LENGTH);
	_os_mem_cpy(drv_priv, &(arp_rsp_info->a2[0]), &(phl_sta->wrole->mac_addr[0]), MAC_ADDRESS_LENGTH);
	_os_mem_cpy(drv_priv, &(arp_rsp_info->a3[0]),
		    &(wow_info->arp_ofld_info.arp_ofld_content.a3[0]),
		    MAC_ADDRESS_LENGTH);
	_os_mem_cpy(drv_priv, &(arp_rsp_info->host_ipv4_addr[0]),
		&(wow_info->arp_ofld_info.arp_ofld_content.host_ipv4_addr[0]),
		IPV4_ADDRESS_LENGTH);
	_os_mem_cpy(drv_priv, &(arp_rsp_info->remote_mac_addr[0]),
		&(wow_info->arp_ofld_info.arp_ofld_content.remote_mac_addr[0]),
		MAC_ADDRESS_LENGTH);
	_os_mem_cpy(drv_priv, &(arp_rsp_info->remote_ipv4_addr[0]),
		&(wow_info->arp_ofld_info.arp_ofld_content.remote_ipv4_addr[0]),
		IPV4_ADDRESS_LENGTH);

	arp_rsp_info->protect_bit = (pairwise_algo == RTW_ENC_NONE) ? false : true;

	if (arp_rsp_info->protect_bit)
		arp_rsp_info->sec_hdr_len = (_phl_chk_hw_form_sechdr(dev_cap, pairwise_algo)) ? 0 : _phl_query_iv_len(pairwise_algo);
}

static void _phl_cfg_pkt_ofld_na_info(struct phl_wow_info *wow_info, struct rtw_phl_stainfo_t *phl_sta,
					struct rtw_pkt_ofld_na_info *na_info)
{
	void *drv_priv = phl_to_drvpriv(wow_info->phl_info);
	u8 pairwise_algo = get_wow_pairwise_algo_type(wow_info);
	struct dev_cap_t *dev_cap = &(wow_info->phl_info->phl_com->dev_cap);

	_os_mem_cpy(drv_priv, &(na_info->a1[0]), &(phl_sta->mac_addr[0]), MAC_ADDRESS_LENGTH);
	_os_mem_cpy(drv_priv, &(na_info->a2[0]), &(phl_sta->wrole->mac_addr[0]), MAC_ADDRESS_LENGTH);
	_os_mem_cpy(drv_priv, &(na_info->a3[0]), &(phl_sta->mac_addr[0]), MAC_ADDRESS_LENGTH);

	na_info->protect_bit = (pairwise_algo == RTW_ENC_NONE) ? false : true;

	if (na_info->protect_bit)
		na_info->sec_hdr_len = (_phl_chk_hw_form_sechdr(dev_cap, pairwise_algo)) ? 0 : _phl_query_iv_len(pairwise_algo);

}

static void _phl_cfg_pkt_ofld_eapol_key_info(
	struct phl_wow_info *wow_info,
	struct rtw_phl_stainfo_t *phl_sta,
	struct rtw_pkt_ofld_eapol_key_info *eapol_key_info)
{
	void *drv_priv = phl_to_drvpriv(wow_info->phl_info);
	struct rtw_gtk_ofld_info *gtk_ofld_info = &wow_info->gtk_ofld_info;
	struct dev_cap_t *dev_cap = &(wow_info->phl_info->phl_com->dev_cap);

	u8 pairwise_algo = get_wow_pairwise_algo_type(wow_info);

	_os_mem_cpy(drv_priv, &(eapol_key_info->a1[0]), &(phl_sta->mac_addr[0]),
		MAC_ADDRESS_LENGTH);

	_os_mem_cpy(drv_priv, &(eapol_key_info->a2[0]), &(phl_sta->wrole->mac_addr[0]),
			MAC_ADDRESS_LENGTH);

	_os_mem_cpy(drv_priv, &(eapol_key_info->a3[0]), &(phl_sta->mac_addr[0]),
			MAC_ADDRESS_LENGTH);

	eapol_key_info->protect_bit = (pairwise_algo == RTW_ENC_NONE) ? false : true;

	if (eapol_key_info->protect_bit)
		eapol_key_info->sec_hdr_len = (_phl_chk_hw_form_sechdr(dev_cap, pairwise_algo)) ? 0 : _phl_query_iv_len(pairwise_algo);

	eapol_key_info->key_desc_ver = _phl_query_key_desc_ver(wow_info, pairwise_algo);
	_os_mem_cpy(drv_priv, eapol_key_info->replay_cnt,
				gtk_ofld_info->gtk_ofld_content.replay_cnt, 8);
}

static void _phl_cfg_pkt_ofld_sa_query_info(
	struct phl_wow_info *wow_info,
	struct rtw_phl_stainfo_t *phl_sta,
	struct rtw_pkt_ofld_sa_query_info *sa_query_info)
{
	void *drv_priv = phl_to_drvpriv(wow_info->phl_info);
	u8 pairwise_algo = get_wow_pairwise_algo_type(wow_info);
	struct dev_cap_t *dev_cap = &(wow_info->phl_info->phl_com->dev_cap);

	_os_mem_cpy(drv_priv, &(sa_query_info->a1[0]), &(phl_sta->mac_addr[0]),
			MAC_ADDRESS_LENGTH);

	_os_mem_cpy(drv_priv, &(sa_query_info->a2[0]), &(phl_sta->wrole->mac_addr[0]),
			MAC_ADDRESS_LENGTH);

	_os_mem_cpy(drv_priv, &(sa_query_info->a3[0]), &(phl_sta->mac_addr[0]),
			MAC_ADDRESS_LENGTH);

	sa_query_info->protect_bit = (pairwise_algo == RTW_ENC_NONE) ? false : true;

	if (sa_query_info->protect_bit)
		sa_query_info->sec_hdr_len = (_phl_chk_hw_form_sechdr(dev_cap, pairwise_algo)) ? 0 : _phl_query_iv_len(pairwise_algo);
}

static void _phl_cfg_pkt_ofld_realwow_kapkt_info(
	struct phl_wow_info *wow_info,
	struct rtw_phl_stainfo_t *phl_sta,
	struct rtw_pkt_ofld_realwow_kapkt_info *kapkt_info)
{
	void *drv_priv = phl_to_drvpriv(wow_info->phl_info);

	_os_mem_cpy(drv_priv, &(kapkt_info->keep_alive_pkt_ptrn[0]),
		&(wow_info->realwow_info.realwow_ofld_content.keep_alive_pkt_ptrn[0]),
		wow_info->realwow_info.realwow_ofld_content.keep_alive_pkt_size);

	kapkt_info->keep_alive_pkt_size =
				wow_info->realwow_info.realwow_ofld_content.keep_alive_pkt_size;
}

static void _phl_cfg_pkt_ofld_realwow_ack_info(
	struct phl_wow_info *wow_info,
	struct rtw_pkt_ofld_realwow_ack_info *ack_info)
{
	void *drv_priv = phl_to_drvpriv(wow_info->phl_info);

	_os_mem_cpy(drv_priv, &(ack_info->ack_ptrn[0]),
		&(wow_info->realwow_info.realwow_ofld_content.ack_ptrn[0]),
		wow_info->realwow_info.realwow_ofld_content.ack_ptrn_size);

	ack_info->ack_ptrn_size = wow_info->realwow_info.realwow_ofld_content.ack_ptrn_size;
}

static void _phl_cfg_pkt_ofld_realwow_wp_info(
	struct phl_wow_info *wow_info,
	struct rtw_pkt_ofld_realwow_wp_info *wp_info)
{
	void *drv_priv = phl_to_drvpriv(wow_info->phl_info);

	_os_mem_cpy(drv_priv, &(wp_info->wakeup_ptrn[0]),
		&(wow_info->realwow_info.realwow_ofld_content.wakeup_ptrn[0]),
		wow_info->realwow_info.realwow_ofld_content.wakeup_ptrn_size);

	wp_info->wakeup_ptrn_size = wow_info->realwow_info.realwow_ofld_content.wakeup_ptrn_size;
}

enum rtw_phl_status rtw_phl_cfg_keep_alive_info(void *phl, struct rtw_keep_alive_info *info)
{
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_SUCCESS;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_wow_info *wow_info = phl_to_wow_info(phl_info);
	struct rtw_keep_alive_info *keep_alive_info = &wow_info->keep_alive_info;

	FUNCIN();

	keep_alive_info->keep_alive_en = info->keep_alive_en;
	keep_alive_info->keep_alive_period = info->keep_alive_period;
	keep_alive_info->keep_alive_pkt_type = info->keep_alive_pkt_type;

	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] keep_alive_en %d\n", keep_alive_info->keep_alive_en);
	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] keep_alive_period %d\n", keep_alive_info->keep_alive_period);
	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] keep_alive_pkt_type %d\n", keep_alive_info->keep_alive_pkt_type);

	return phl_status;
}

enum rtw_phl_status rtw_phl_cfg_disc_det_info(void *phl, struct rtw_disc_det_info *info)
{
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_SUCCESS;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_wow_info *wow_info = phl_to_wow_info(phl_info);
	struct rtw_disc_det_info *disc_det_info = &wow_info->disc_det_info;

	FUNCIN();

	disc_det_info->disc_det_en = info->disc_det_en;
	disc_det_info->disc_wake_en = info->disc_wake_en;
	disc_det_info->try_pkt_count = info->try_pkt_count;
	disc_det_info->check_period = info->check_period;
	disc_det_info->cnt_bcn_lost_en = info->cnt_bcn_lost_en;
	disc_det_info->cnt_bcn_lost_limit = info->cnt_bcn_lost_limit;

	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] disc_det_en %d\n", disc_det_info->disc_det_en);
	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] disc_wake_en %d\n", disc_det_info->disc_wake_en);
	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] try_pkt_count %d\n", disc_det_info->try_pkt_count);
	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] check_period %d\n", disc_det_info->check_period);
	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] cnt_bcn_lost_en %d\n", disc_det_info->cnt_bcn_lost_en);
	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] cnt_bcn_lost_limit %d\n", disc_det_info->cnt_bcn_lost_limit);

	return phl_status;
}

static void
_phl_show_nlo_info(struct rtw_nlo_info *info)
{
	u32 i = 0;

	if (info->num_of_networks == 0)
		return;

	for (i = 0; i < info->num_of_networks; i++) {

		PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_,
			"[wow][nlo] #%u ssid/len/cipher = %s/%u/%#x \n",
			i, (char *)info->ssid[i], info->ssidlen[i], info->chipertype[i]);
	}

	for (i = 0; i < info->channel_num; i++) {
		PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_,
			"[wow][nlo] channel #%u: %u \n", i, info->channel_list[i].chan);
	}

	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_,
		"[wow][nlo] num of hidden ap %u \n", info->num_of_hidden_ap);

	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_,
		"[wow][nlo] delyms/cycle/period/slow_period = %u/%u/%u/%u \n",
		info->delay, info->cycle, info->period, info->slow_period);

}

void rtw_phl_cfg_nlo_info(void *phl, struct rtw_nlo_info *info)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_wow_info *wow_info = phl_to_wow_info(phl_info);
	struct rtw_nlo_info *nlo_info = &wow_info->nlo_info;
	void *drv_priv = phl_to_drvpriv(phl_info);

	FUNCIN();

	nlo_info->nlo_en = info->nlo_en;
	nlo_info->num_of_networks = info->num_of_networks;
	_os_mem_cpy(drv_priv, nlo_info->ssid, info->ssid,
		info->num_of_networks * MAX_SSID_LEN);
	_os_mem_cpy(drv_priv, nlo_info->ssidlen,
		info->ssidlen, info->num_of_networks);
	_os_mem_cpy(drv_priv, nlo_info->chipertype,
		info->chipertype, info->num_of_networks);
	nlo_info->num_of_hidden_ap = info->num_of_hidden_ap;
	nlo_info->channel_num = info->channel_num;
	_os_mem_cpy(drv_priv, nlo_info->channel_list, info->channel_list,
		info->channel_num * sizeof(struct scan_ofld_ch_info));
	nlo_info->period = info->period;
	nlo_info->cycle = info->cycle;
	nlo_info->slow_period = info->slow_period;
	nlo_info->delay = info->delay;
	nlo_info->construct_pbreq = info->construct_pbreq;

	_phl_show_nlo_info(nlo_info);
}

void rtw_phl_cfg_periodic_wake_info(void *phl,
				    struct rtw_periodic_wake_info *info)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_wow_info *wow_info = phl_to_wow_info(phl_info);
	struct rtw_periodic_wake_info *pw_info = &wow_info->periodic_wake_info;
	void *drv_priv = phl_to_drvpriv(phl_info);

	FUNCIN();

	_os_mem_cpy(drv_priv, pw_info, info,
		    sizeof(struct rtw_periodic_wake_info));
}

void rtw_phl_cfg_arp_ofld_info(void *phl, struct rtw_arp_ofld_info *info)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_wow_info *wow_info = phl_to_wow_info(phl_info);
	struct rtw_arp_ofld_info *arp_ofld_info = &wow_info->arp_ofld_info;
	void *drv_priv = phl_to_drvpriv(phl_info);

	FUNCIN();


	arp_ofld_info->arp_en = info->arp_en;

	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] arp_en %u\n",
			arp_ofld_info->arp_en);

	/* If not enabled, the following actions are not necessary */
	if (false == arp_ofld_info->arp_en)
		return;

	arp_ofld_info->arp_action = info->arp_action;

	_os_mem_cpy(drv_priv, &(arp_ofld_info->arp_ofld_content.a3[0]),
		    &(info->arp_ofld_content.a3[0]), MAC_ADDRESS_LENGTH);

	_os_mem_cpy(drv_priv,
		&(arp_ofld_info->arp_ofld_content.remote_ipv4_addr[0]),
		&(info->arp_ofld_content.remote_ipv4_addr[0]),
		IPV4_ADDRESS_LENGTH);

	_os_mem_cpy(drv_priv,
		&(arp_ofld_info->arp_ofld_content.host_ipv4_addr[0]),
		&(info->arp_ofld_content.host_ipv4_addr[0]),
		IPV4_ADDRESS_LENGTH);

	_os_mem_cpy(drv_priv,
		&(arp_ofld_info->arp_ofld_content.remote_mac_addr[0]),
		&(info->arp_ofld_content.remote_mac_addr[0]),
		MAC_ADDRESS_LENGTH);

	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] arp_action %u\n",
			arp_ofld_info->arp_action);

	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] arp_remote_ipv4  %u:%u:%u:%u\n",
			arp_ofld_info->arp_ofld_content.remote_ipv4_addr[0],
			arp_ofld_info->arp_ofld_content.remote_ipv4_addr[1],
			arp_ofld_info->arp_ofld_content.remote_ipv4_addr[2],
			arp_ofld_info->arp_ofld_content.remote_ipv4_addr[3]);

	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] arp_host_ipv4  %u:%u:%u:%u\n",
			arp_ofld_info->arp_ofld_content.host_ipv4_addr[0],
			arp_ofld_info->arp_ofld_content.host_ipv4_addr[1],
			arp_ofld_info->arp_ofld_content.host_ipv4_addr[2],
			arp_ofld_info->arp_ofld_content.host_ipv4_addr[3]);

	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] arp_remote_mac  %02x:%02x:%02x:%02x:%02x:%02x \n",
			arp_ofld_info->arp_ofld_content.remote_mac_addr[0],
			arp_ofld_info->arp_ofld_content.remote_mac_addr[1],
			arp_ofld_info->arp_ofld_content.remote_mac_addr[2],
			arp_ofld_info->arp_ofld_content.remote_mac_addr[3],
			arp_ofld_info->arp_ofld_content.remote_mac_addr[4],
			arp_ofld_info->arp_ofld_content.remote_mac_addr[5]);

}

void rtw_phl_cfg_ndp_ofld_info(void *phl, struct rtw_ndp_ofld_info *info)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_wow_info *wow_info = phl_to_wow_info(phl_info);
	struct rtw_ndp_ofld_info *ndp_ofld_info = &wow_info->ndp_ofld_info;
	struct rtw_ndp_ofld_content *pcontent;
	void *drv_priv = phl_to_drvpriv(phl_info);
	u8 idx = 0;

	FUNCIN();

	ndp_ofld_info->ndp_en = info->ndp_en;

	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] ndp_en %u\n",
			ndp_ofld_info->ndp_en);

	/* If not enabled, the following actions are not necessary */
	if (false == ndp_ofld_info->ndp_en)
		return;

	for (idx = 0; idx < 2; idx++) {

		pcontent = &ndp_ofld_info->ndp_ofld_content[idx];
		pcontent->ndp_en = info->ndp_ofld_content[idx].ndp_en;

		pcontent->chk_remote_ip =
			info->ndp_ofld_content[idx].chk_remote_ip;
		pcontent->num_target_ip =
			info->ndp_ofld_content[idx].num_target_ip;

		_os_mem_cpy(drv_priv, &(pcontent->mac_addr[0]),
			&(info->ndp_ofld_content[idx].mac_addr[0]),
			MAC_ADDRESS_LENGTH);
		_os_mem_cpy(drv_priv, &(pcontent->remote_ipv6_addr[0]),
			&(info->ndp_ofld_content[idx].remote_ipv6_addr[0]),
			IPV6_ADDRESS_LENGTH);
		_os_mem_cpy(drv_priv, &(pcontent->target_ipv6_addr[0][0]),
			&(info->ndp_ofld_content[idx].target_ipv6_addr[0][0]),
			IPV6_ADDRESS_LENGTH*2);

		PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] ndp_chk_remote_ip %u\n",
			pcontent->chk_remote_ip);

		PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] ndp_num_target_ip %u\n",
			pcontent->num_target_ip);

		PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] ndp_mac_addr  %02x:%02x:%02x:%02x:%02x:%02x \n",
			pcontent->mac_addr[0],
			pcontent->mac_addr[1],
			pcontent->mac_addr[2],
			pcontent->mac_addr[3],
			pcontent->mac_addr[4],
			pcontent->mac_addr[5]);

		PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_,
			"[wow] ndp_remote_ipv6  %02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x \n",
			pcontent->remote_ipv6_addr[0],
			pcontent->remote_ipv6_addr[1],
			pcontent->remote_ipv6_addr[2],
			pcontent->remote_ipv6_addr[3],
			pcontent->remote_ipv6_addr[4],
			pcontent->remote_ipv6_addr[5],
			pcontent->remote_ipv6_addr[6],
			pcontent->remote_ipv6_addr[7],
			pcontent->remote_ipv6_addr[8],
			pcontent->remote_ipv6_addr[9],
			pcontent->remote_ipv6_addr[10],
			pcontent->remote_ipv6_addr[11],
			pcontent->remote_ipv6_addr[12],
			pcontent->remote_ipv6_addr[13],
			pcontent->remote_ipv6_addr[14],
			pcontent->remote_ipv6_addr[15]);

		PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_,
			"[wow] ndp_target_ipv6_addr  %02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x \n",
			pcontent->target_ipv6_addr[0][0],
			pcontent->target_ipv6_addr[0][1],
			pcontent->target_ipv6_addr[0][2],
			pcontent->target_ipv6_addr[0][3],
			pcontent->target_ipv6_addr[0][4],
			pcontent->target_ipv6_addr[0][5],
			pcontent->target_ipv6_addr[0][6],
			pcontent->target_ipv6_addr[0][7],
			pcontent->target_ipv6_addr[0][8],
			pcontent->target_ipv6_addr[0][9],
			pcontent->target_ipv6_addr[0][10],
			pcontent->target_ipv6_addr[0][11],
			pcontent->target_ipv6_addr[0][12],
			pcontent->target_ipv6_addr[0][13],
			pcontent->target_ipv6_addr[0][14],
			pcontent->target_ipv6_addr[0][15]);

		PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_,
			"[wow] ndp_target_ipv6_addr  %02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x \n",
			pcontent->target_ipv6_addr[1][0],
			pcontent->target_ipv6_addr[1][1],
			pcontent->target_ipv6_addr[1][2],
			pcontent->target_ipv6_addr[1][3],
			pcontent->target_ipv6_addr[1][4],
			pcontent->target_ipv6_addr[1][5],
			pcontent->target_ipv6_addr[1][6],
			pcontent->target_ipv6_addr[1][7],
			pcontent->target_ipv6_addr[1][8],
			pcontent->target_ipv6_addr[1][9],
			pcontent->target_ipv6_addr[1][10],
			pcontent->target_ipv6_addr[1][11],
			pcontent->target_ipv6_addr[1][12],
			pcontent->target_ipv6_addr[1][13],
			pcontent->target_ipv6_addr[1][14],
			pcontent->target_ipv6_addr[1][15]);

	}

}

u8 _phl_query_free_cam_entry_idx(struct rtw_pattern_match_info *pattern_match_info)
{
	struct rtw_wowcam_upd_info *wowcam_info = pattern_match_info->wowcam_info;
	u8 i = 0;

	for (i = 0; i < MAX_WOW_CAM_NUM; ++i)
		if (wowcam_info[i].valid == 0)
			break;

	return i;
}

u16 _phl_cal_crc16(u8 data, u16 crc)
{
	u8 shift_in, data_bit;
	u8 crc_bit4, crc_bit11, crc_bit15;
	u16 crc_result;
	int index;

	for (index = 0; index < 8; index++) {
		crc_bit15 = ((crc & BIT15) ? 1 : 0);
		data_bit = (data & (BIT0 << index) ? 1 : 0);
		shift_in = crc_bit15 ^ data_bit;
		/*printf("crc_bit15=%d, DataBit=%d, shift_in=%d\n",
		 * crc_bit15, data_bit, shift_in);*/

		crc_result = crc << 1;

		if (shift_in == 0)
			crc_result &= (~BIT0);
		else
			crc_result |= BIT0;
		/*printf("CRC =%x\n",CRC_Result);*/

		crc_bit11 = ((crc & BIT11) ? 1 : 0) ^ shift_in;

		if (crc_bit11 == 0)
			crc_result &= (~BIT12);
		else
			crc_result |= BIT12;

		/*printf("bit12 CRC =%x\n",CRC_Result);*/

		crc_bit4 = ((crc & BIT4) ? 1 : 0) ^ shift_in;

		if (crc_bit4 == 0)
			crc_result &= (~BIT5);
		else
			crc_result |= BIT5;

		/* printf("bit5 CRC =%x\n",CRC_Result); */
		crc = crc_result;
	}
	return crc;
}

u16 _phl_cal_wow_ptrn_crc(u8 *pattern, u32 length)
{
	u16 crc = 0xffff;
	u32 i;

	for (i = 0; i < length; i++)
		crc = _phl_cal_crc16(pattern[i], crc);
	crc = ~crc;

	return crc;
}
/*
 * To get the wake up pattern from the mask.
 * We do not count first 12 bits which means
 * DA[6] and SA[6] in the pattern to match HW design.
 */
u32 _phl_get_ptrn_after_mask(struct rtw_wowcam_upd_info *wowcam_info, u8 *ptrn_after_mask)
{
	u32 ptrn_len_after_mask = 0;
	u32 i;
	u8 da_sa_offset = 12;

	for (i = da_sa_offset; i < wowcam_info->ptrn_len; i++) {
		if (wowcam_info->mask[i / 8] >> (i % 8) & 0x01) {
			ptrn_after_mask[ptrn_len_after_mask] = wowcam_info->ptrn[i];
			ptrn_len_after_mask++;
		}
	}

	return ptrn_len_after_mask;
}

/*
 * translate mask from os to mask for hw
 *
 * pattern from OS uses 'ethenet frame', like this:
 *	|    6   |    6   |   2  |     20    |  Variable  |  4  |
 *	|--------+--------+------+-----------+------------+-----|
 *	|    802.3 Mac Header    | IP Header | TCP Packet | FCS |
 *	|   DA   |   SA   | Type |
 *
 * BUT, packet catched by our HW is in '802.11 frame', begin from LLC,
 *	|     24 or 30      |    6   |   2  |     20    |  Variable  |  4  |
 *	|-------------------+--------+------+-----------+------------+-----|
 *	| 802.11 MAC Header |       LLC     | IP Header | TCP Packet | FCS |
 *			    | Others | Type |
 *
 * Therefore, we need to translate mask_from_OS to mask_to_hw.
 * We should left-shift mask_from_os by 6 bits to omit 'DA',
 * to make it correspond to 'LLC' of mask_to_hw.
 * Our HW packet begins from LLC, mask_to_hw[5:0] is part of LLC,
 * but mask_from_os[5:0] is 'SA' after left-shift.
 * They just don't match, so we need to set first 5 bits to 0.
*/

void _phl_to_hw_wake_mask(struct rtw_wowcam_upd_info *wowcam_info)
{
	u8 mask_hw[MAX_WOW_PATTERN_SIZE_BYTE] = {0};
	u32 mask_len = _os_div_round_up(wowcam_info->ptrn_len, 8);
	u32 i;
	u8 sa_offset = 6;

	for (i = 0; i < mask_len - 1; i++) {
		mask_hw[i] = wowcam_info->mask[i] >> sa_offset;
		mask_hw[i] |= (wowcam_info->mask[i + 1] & 0x3F) << 2;
	}
	mask_hw[i] = (wowcam_info->mask[i] >> sa_offset) & 0x3F;
	mask_hw[0] &= 0xC0;

	for (i = 0; i < MAX_WOW_PATTERN_SIZE_DWORD; i++) {
		wowcam_info->wake_mask[i] = mask_hw[i * 4];
		wowcam_info->wake_mask[i] |= (mask_hw[i * 4 + 1] << 8);
		wowcam_info->wake_mask[i] |= (mask_hw[i * 4 + 2] << 16);
		wowcam_info->wake_mask[i] |= (mask_hw[i * 4 + 3] << 24);
	}
}

enum rtw_phl_status rtw_phl_remove_wow_ptrn_info(void *phl, u8 wowcam_id)
{
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_FAILURE;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_wow_info *wow_info = phl_to_wow_info(phl_info);
	struct rtw_pattern_match_info *pattern_match_info = &wow_info->pattern_match_info;
	struct rtw_wowcam_upd_info *wowcam_info = &(pattern_match_info->wowcam_info[wowcam_id]);

	if (wowcam_id < MAX_WOW_CAM_NUM) {
		wowcam_info->valid = 0;
		phl_status = RTW_PHL_STATUS_SUCCESS;
	} else {
		PHL_TRACE(COMP_PHL_WOW, _PHL_WARNING_, "[wow] %s(): Invalid wowcam id(%u), Fail.\n",
						__func__, wowcam_id);
		phl_status = RTW_PHL_STATUS_FAILURE;
	}

	return phl_status;
}

enum rtw_phl_status rtw_phl_add_wow_ptrn_info(void *phl, struct rtw_wowcam_upd_info *info, u8 *wowcam_id)
{
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_FAILURE;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_wow_info *wow_info = phl_to_wow_info(phl_info);
	struct rtw_pattern_match_info *pattern_match_info = &wow_info->pattern_match_info;
	struct rtw_wowcam_upd_info *wowcam_info = NULL;
	void *d = phl_to_drvpriv(phl_info);
	u8 ptrn_after_mask[MAX_WOW_PATTERN_SIZE_BIT] = {0};
	u32 ptrn_len_after_mask = 0;

	*wowcam_id = _phl_query_free_cam_entry_idx(pattern_match_info);

	if (*wowcam_id < MAX_WOW_CAM_NUM) {
		wowcam_info = &(pattern_match_info->wowcam_info[*wowcam_id]);

		_os_mem_set(d, wowcam_info, 0, sizeof(struct rtw_wowcam_upd_info));
		_os_mem_cpy(d, wowcam_info, info, sizeof(struct rtw_wowcam_upd_info));

		ptrn_len_after_mask = _phl_get_ptrn_after_mask(wowcam_info, ptrn_after_mask);
		wowcam_info->match_crc = _phl_cal_wow_ptrn_crc(ptrn_after_mask, ptrn_len_after_mask);

		_phl_to_hw_wake_mask(wowcam_info);

		/* fill in phl */
		wowcam_info->wow_cam_idx = *wowcam_id;
		wowcam_info->rw = 1;
		wowcam_info->is_negative_pattern_match = 0;
		wowcam_info->skip_mac_hdr = 1;
		wowcam_info->valid = 1;

		phl_status = RTW_PHL_STATUS_SUCCESS;
	} else {
		PHL_TRACE(COMP_PHL_WOW, _PHL_WARNING_, "[wow] no free cam entry can be used.\n");
		phl_status = RTW_PHL_STATUS_RESOURCE;
	}

	return phl_status;
}

enum rtw_phl_status rtw_phl_cfg_gtk_ofld_info(void *phl, struct rtw_gtk_ofld_info *info)
{
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_SUCCESS;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_wow_info *wow_info = phl_to_wow_info(phl_info);
	struct rtw_gtk_ofld_info *gtk_ofld_info = &wow_info->gtk_ofld_info;
	void *d = phl_to_drvpriv(phl_info);

	FUNCIN();

	if (info == NULL || gtk_ofld_info == NULL) {
		PHL_TRACE(COMP_PHL_WOW, _PHL_WARNING_, "[wow] %s(): some ptr is NULL\n", __func__);
		phl_status = RTW_PHL_STATUS_FAILURE;

	} else {
		_os_mem_set(d, gtk_ofld_info, 0, sizeof(struct rtw_gtk_ofld_info));

		PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] gtk_en(%u), continue to gtk_ofld.\n", info->gtk_en);

		if (info->gtk_en) {
			_os_mem_cpy(d, gtk_ofld_info, info, sizeof(struct rtw_gtk_ofld_info));

			PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] gtk_ofld_info:\n");
			PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] - gtk_en          = %u\n", gtk_ofld_info->gtk_en);
			PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] - tkip_en         = %u\n", gtk_ofld_info->tkip_en);
			PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] - ieee80211w_en   = %u\n", gtk_ofld_info->ieee80211w_en);
			PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] - pairwise_wakeup = %u\n", gtk_ofld_info->pairwise_wakeup);
			PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] - bip_sec_algo    = %u\n", gtk_ofld_info->bip_sec_algo);

			PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] gtk_ofld_content:\n");
			PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] - akmtype_byte3   = %u\n", gtk_ofld_info->akmtype_byte3);
			PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] - kck_len         = %u\n", gtk_ofld_info->gtk_ofld_content.kck_len);
			PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] - kek_len         = %u\n", gtk_ofld_info->gtk_ofld_content.kek_len);
			PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] - replay_cnt      = 0x%x%x\n",
						*((u32 *)(gtk_ofld_info->gtk_ofld_content.replay_cnt)+1),
						*((u32 *)(gtk_ofld_info->gtk_ofld_content.replay_cnt)));

			if(info->ieee80211w_en) {
				gtk_ofld_info->hw_11w_en = true;
				PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] - igtk_keyid      = 0x%x\n",
								*((u32 *)(gtk_ofld_info->gtk_ofld_content.igtk_keyid)));
				PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] - ipn             = 0x%x%x\n",
								*((u32 *)(gtk_ofld_info->gtk_ofld_content.ipn)+1),
								*((u32 *)(gtk_ofld_info->gtk_ofld_content.ipn)));
				PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] - igtk_len        = %u\n", gtk_ofld_info->gtk_ofld_content.igtk_len);
				PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] - psk_len         = %u\n", gtk_ofld_info->gtk_ofld_content.psk_len);
			}
		} else {
			PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] gtk_ofld_info:\n");
			PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] - gtk_en          = %u\n", gtk_ofld_info->gtk_en);
		}
	}

	FUNCOUT();

	return phl_status;
}

enum rtw_phl_status rtw_phl_cfg_realwow_info(void *phl, struct rtw_realwow_info *info)
{
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_SUCCESS;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_wow_info *wow_info = phl_to_wow_info(phl_info);
	struct rtw_realwow_info *realwow_info = &wow_info->realwow_info;
	void *d = phl_to_drvpriv(phl_info);

	if (info == NULL || realwow_info == NULL) {
		PHL_TRACE(COMP_PHL_WOW, _PHL_WARNING_, "[wow] %s(): some ptr is NULL\n", __func__);
		phl_status = RTW_PHL_STATUS_FAILURE;

	} else {
		_os_mem_set(d, realwow_info, 0, sizeof(struct rtw_realwow_info));

		PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] realwow_en(%u), continue to realwow_ofld.\n",
					info->realwow_en);

		if (info->realwow_en) {
			_os_mem_cpy(d, realwow_info, info, sizeof(struct rtw_realwow_info));

			PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] realwow_ofld_info:\n");
			PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] - realwow_en 		= %u\n",
						realwow_info->realwow_en);
			PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] - tkip_en			= %u\n",
						realwow_info->auto_wakeup);
			PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] - interval			= %u\n",
						realwow_info->realwow_ofld_content.interval);
			PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] - kapktsize			= %u\n",
						realwow_info->realwow_ofld_content.keep_alive_pkt_size);
			PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] - acklostlimit		= %u\n",
						realwow_info->realwow_ofld_content.ack_lost_limit);
			PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] - ackpatternsize		= %u\n",
						realwow_info->realwow_ofld_content.ack_ptrn_size);
			PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] - wakeuppatternsize	= %u\n",
						realwow_info->realwow_ofld_content.wakeup_ptrn_size);
		} else {
			PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] realwow_ofld_info:\n");
			PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] - realwow_en		= %u\n",
						realwow_info->realwow_en);
		}
	}

	return phl_status;
}

enum rtw_phl_status rtw_phl_cfg_wow_wake(void *phl, struct rtw_wow_wake_info *info)
{
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_SUCCESS;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_wow_info *wow_info = phl_to_wow_info(phl_info);
	void *d = phl_to_drvpriv(phl_info);

	struct rtw_wow_wake_info *wow_wake_info = &wow_info->wow_wake_info;

	FUNCIN();

	wow_wake_info->wow_en = info->wow_en;
	wow_wake_info->drop_all_pkt = info->drop_all_pkt;
	wow_wake_info->rx_parse_after_wake = info->rx_parse_after_wake;
	wow_wake_info->pairwise_sec_algo = info->pairwise_sec_algo;
	wow_wake_info->group_sec_algo = info->group_sec_algo;
	wow_wake_info->pattern_match_en = info->pattern_match_en;
	wow_wake_info->magic_pkt_en = info->magic_pkt_en;
	wow_wake_info->hw_unicast_en = info->hw_unicast_en;
	wow_wake_info->fw_unicast_en = info->fw_unicast_en;
	wow_wake_info->deauth_wakeup = info->deauth_wakeup;
	wow_wake_info->rekey_wakeup = info->rekey_wakeup;
	wow_wake_info->eap_wakeup = info->eap_wakeup;
	wow_wake_info->all_data_wakeup = info->all_data_wakeup;
	_os_mem_cpy(d, &wow_wake_info->remote_wake_ctrl_info,
		&info->remote_wake_ctrl_info, sizeof(struct rtw_remote_wake_ctrl_info));

	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] wow_en %d\n", wow_wake_info->wow_en);
	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] drop_all_pkt %d\n", wow_wake_info->drop_all_pkt);
	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] rx_parse_after_wake %d\n", wow_wake_info->rx_parse_after_wake);
	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] pairwise_sec_algo %d\n", wow_wake_info->pairwise_sec_algo);
	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] group_sec_algo %d\n", wow_wake_info->group_sec_algo);
	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] bip_sec_algo %d\n", wow_wake_info->bip_sec_algo);
	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] pattern_match_en %d\n", wow_wake_info->pattern_match_en);
	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] magic_pkt_en %d\n", wow_wake_info->magic_pkt_en);
	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] hw_unicast_en %d\n", wow_wake_info->hw_unicast_en);
	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] fw_unicast_en %d\n", wow_wake_info->fw_unicast_en);
	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] deauth_wakeup %d\n", wow_wake_info->deauth_wakeup);
	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] rekey_wakeup %d\n", wow_wake_info->rekey_wakeup);
	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] eap_wakeup %d\n", wow_wake_info->eap_wakeup);
	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] all_data_wakeup %d\n", wow_wake_info->all_data_wakeup);

	return phl_status;
}

enum rtw_phl_status rtw_phl_cfg_gpio_wake_pulse(void *phl, struct rtw_wow_gpio_info *info)
{
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_SUCCESS;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_wow_info *wow_info = phl_to_wow_info(phl_info);
	struct rtw_wow_gpio_info *wow_gpio = &wow_info->wow_gpio;
	void *drv_priv = phl_to_drvpriv(phl_info);
	struct rtw_dev2hst_gpio_info *d2h_gpio_info = &wow_gpio->d2h_gpio_info;

	FUNCIN();

	_os_mem_cpy(drv_priv, d2h_gpio_info, &info->d2h_gpio_info,
		    sizeof(struct rtw_dev2hst_gpio_info));
	wow_gpio->dev2hst_gpio_mode = info->dev2hst_gpio_mode;
	wow_gpio->dev2hst_gpio = info->dev2hst_gpio;
	wow_gpio->dev2hst_high = info->dev2hst_high;

	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] dev2hst_gpio_en %d\n", d2h_gpio_info->dev2hst_gpio_en);
	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] disable_inband %d\n", d2h_gpio_info->disable_inband);
	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] gpio_output_input %d\n", d2h_gpio_info->gpio_output_input);
	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] gpio_active %d\n", d2h_gpio_info->gpio_active);
	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] toggle_pulse %d\n", d2h_gpio_info->toggle_pulse);
	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] data_pin_wakeup %d\n", d2h_gpio_info->data_pin_wakeup);
	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] gpio_pulse_nonstop %d\n", d2h_gpio_info->gpio_pulse_nonstop);
	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] gpio_time_unit %d\n", d2h_gpio_info->gpio_time_unit);
	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] gpio_num %d\n", d2h_gpio_info->gpio_num);
	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] gpio_pulse_dura %d\n", d2h_gpio_info->gpio_pulse_dura);
	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] gpio_pulse_period %d\n", d2h_gpio_info->gpio_pulse_period);
	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] gpio_pulse_count %d\n", d2h_gpio_info->gpio_pulse_count);
	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] customer_id %d\n", d2h_gpio_info->customer_id);
	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] rsn_a_en %d\n", d2h_gpio_info->rsn_a_en);
	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] rsn_a_time_unit %d\n", d2h_gpio_info->rsn_a_time_unit);
	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] rsn_a_pulse_nonstop %d\n", d2h_gpio_info->rsn_a_pulse_nonstop);
	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] rsn_a %d\n", d2h_gpio_info->rsn_a);
	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] rsn_a_pulse_duration %d\n", d2h_gpio_info->rsn_a_pulse_duration);
	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] rsn_a_pulse_count %d\n", d2h_gpio_info->rsn_a_pulse_count);

	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] dev2hst_gpio_mode %d\n", wow_gpio->dev2hst_gpio_mode);
	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] dev2hst_gpio %d\n", wow_gpio->dev2hst_gpio);
	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] dev2hst_high %d\n", wow_gpio->dev2hst_high);

	return phl_status;
}

void rtw_phl_wow_set_no_link_mode(void *phl, u8 no_link_mode)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_wow_info *wow_info = phl_to_wow_info(phl_info);

	wow_info->no_link_mode = no_link_mode;
}

void phl_record_wow_stat(struct phl_wow_info *wow_info)
{
	struct phl_wow_stat *wow_stat = &wow_info->wow_stat;

	/* init */
	wow_stat->func_en = wow_info->func_en;
	wow_stat->op_mode = wow_info->op_mode;
	wow_stat->keep_alive_en = wow_info->keep_alive_info.keep_alive_en;
	wow_stat->disc_det_en = wow_info->disc_det_info.disc_det_en;
	wow_stat->arp_en = wow_info->arp_ofld_info.arp_en;
	wow_stat->ndp_en = wow_info->ndp_ofld_info.ndp_en;
	wow_stat->gtk_en = wow_info->gtk_ofld_info.gtk_en;
	wow_stat->dot11w_en = wow_info->gtk_ofld_info.ieee80211w_en;
	wow_stat->err.init = wow_info->err.init;
	/* deinit */
	wow_stat->mac_pwr = wow_info->mac_pwr;
	wow_stat->wake_rsn = wow_info->wake_rsn;
	wow_stat->err.deinit = wow_info->err.deinit;

	if (wow_info->aoac_info.rpt_fail)
		++wow_stat->aoac_rpt_fail_cnt;
}

#ifdef CONFIG_PCI_HCI
enum rtw_phl_status _init_precfg(struct phl_info_t *phl_info, u8 band)
{
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_FAILURE;
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	u8 status = false;

	do {
		/* 1. stop Tx DMA */
		rtw_hal_wow_cfg_txdma(phl_info->hal, false);

		/* 2. stop HW Tx */
		hstatus = rtw_hal_wow_drop_tx(phl_info->hal, band);
		if (RTW_HAL_STATUS_SUCCESS != hstatus) {
			PHL_ERR("[wow] rtw_hal_wow_drop_tx fail!\n");
			break;
		}

		/* 3. poll dma idle */
		status = rtw_hal_poll_txdma_idle(phl_info->hal);
		if (!status) {
			PHL_ERR("[wow] rtw_hal_poll_txdma_idle fail!\n");
			break;
		}

	} while (0);

	if (RTW_HAL_STATUS_SUCCESS != hstatus)
		pstatus = RTW_PHL_STATUS_FAILURE;
	else
		pstatus = RTW_PHL_STATUS_SUCCESS;

	FUNCOUT_WSTS(pstatus);

	return pstatus;
}
enum rtw_phl_status _init_postcfg(struct phl_info_t *phl_info)
{
	/* stop tx/rx hci */
	rtw_hal_cfg_txhci(phl_info->hal, false);
	rtw_hal_cfg_rxhci(phl_info->hal, false);

	rtw_hal_poll_txdma_idle(phl_info->hal);

	return RTW_PHL_STATUS_SUCCESS;
}
#elif defined(CONFIG_USB_HCI)
enum rtw_phl_status _init_precfg(struct phl_info_t *phl_info, u8 band)
{
	return RTW_PHL_STATUS_SUCCESS;
}
enum rtw_phl_status _init_postcfg(struct phl_info_t *phl_info)
{
	struct phl_hci_trx_ops *trx_ops = phl_info->hci_trx_ops;

	trx_ops->trx_stop(phl_info);

	return RTW_PHL_STATUS_SUCCESS;
}
#elif defined(CONFIG_SDIO_HCI)
enum rtw_phl_status _init_precfg(struct phl_info_t *phl_info, u8 band)
{
	return RTW_PHL_STATUS_SUCCESS;
}
enum rtw_phl_status _init_postcfg(struct phl_info_t *phl_info)
{
	return RTW_PHL_STATUS_SUCCESS;
}

#endif

static enum rtw_phl_status _init_precfg_set_rxfltr(struct phl_info_t *phl_info)
{
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_SUCCESS;

	hstatus = rtw_hal_set_rxfltr_type_by_mode(phl_info->hal, 0, RX_FLTR_TYPE_MODE_STA_WOW_INIT_PRE);

	return (hstatus == RTW_HAL_STATUS_SUCCESS) ?
			RTW_PHL_STATUS_SUCCESS : RTW_PHL_STATUS_FAILURE;
}

static enum rtw_phl_status _init_postcfg_set_rxfltr(struct phl_info_t *phl_info)
{
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_SUCCESS;

	hstatus = rtw_hal_set_rxfltr_type_by_mode(phl_info->hal, 0, RX_FLTR_TYPE_MODE_STA_WOW_INIT_POST);

	return (hstatus == RTW_HAL_STATUS_SUCCESS) ?
			RTW_PHL_STATUS_SUCCESS : RTW_PHL_STATUS_FAILURE;
}

static void _wow_start_datapath(struct phl_info_t *phl_info, u8 type)
{
	struct phl_hci_trx_ops *trx_ops = phl_info->hci_trx_ops;

	if (type & PHL_CTRL_TX) {
		trx_ops->trx_resume(phl_info, PHL_CTRL_TX);
	}

	if (type & PHL_CTRL_RX) {
		trx_ops->trx_resume(phl_info, PHL_CTRL_RX);
	}
}

#define MAX_POLLING_TRX_STOP_TIME 50000 /* us */
static void _wow_stop_datapath(struct phl_info_t *phl_info, u8 type)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_SUCCESS;
	struct phl_hci_trx_ops *trx_ops = phl_info->hci_trx_ops;
	u32 start_t = 0;

	if (type & PHL_CTRL_TX) {
		trx_ops->req_tx_stop(phl_info);
		pstatus = rtw_phl_tx_req_notify(phl_info);
		if (RTW_PHL_STATUS_SUCCESS != pstatus)
			PHL_ERR("[wow] rtw_phl_tx_req_notify fail, status(%u)\n", pstatus);

		start_t = _os_get_cur_time_us();

		while (1) {
			if (phl_get_passing_time_us(start_t) >= MAX_POLLING_TRX_STOP_TIME) {
				PHL_TRACE(COMP_PHL_WOW, _PHL_WARNING_, "[wow] sw tx pause fail!\n");
				break;
			}
			if (trx_ops->is_tx_pause(phl_info)) {
				PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] sw tx pause succeed.\n");
				break;
			}
			_os_sleep_us(phl_info->phl_com->drv_priv, 50);
		}
	}

	if (type & PHL_CTRL_RX) {
		trx_ops->req_rx_stop(phl_info);
		pstatus = rtw_phl_start_rx_process(phl_info);
		if (RTW_PHL_STATUS_SUCCESS != pstatus)
			PHL_ERR("[wow] rtw_phl_start_rx_process failed.\n");

		start_t = _os_get_cur_time_us();

		while (1) {
			if (phl_get_passing_time_us(start_t) >= MAX_POLLING_TRX_STOP_TIME) {
				PHL_TRACE(COMP_PHL_WOW, _PHL_WARNING_, "[wow] sw rx pause fail!\n");
				break;
			}
			if (trx_ops->is_rx_pause(phl_info)) {
				PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] sw rx pause succeed.\n");
				break;
			}
			_os_sleep_us(phl_info->phl_com->drv_priv, 50);
		}
	}
}

enum rtw_phl_status phl_wow_init_precfg(struct phl_wow_info *wow_info)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_SUCCESS;
	struct phl_info_t *phl_info = wow_info->phl_info;
	struct rtw_wifi_role_t *wrole = wow_info->sta->wrole;
	struct rtw_wifi_role_link_t *rlink = NULL;
	u8 idx = 0;

	_wow_stop_datapath(phl_info, PHL_CTRL_TX);

	/* init pre-configuration for different interfaces */
	for (idx = 0; idx < wrole->rlink_num; idx++) {
		rlink = get_rlink(wrole, idx);

		pstatus = _init_precfg(phl_info, rlink->hw_band);
		if (RTW_PHL_STATUS_SUCCESS != pstatus)
			return pstatus;
	}

	/* set packet drop by setting rx filter */
	pstatus = _init_precfg_set_rxfltr(phl_info);
	if (RTW_PHL_STATUS_SUCCESS != pstatus)
		return pstatus;

	/* disable ppdu sts */
	for (idx = 0; idx < wrole->rlink_num; idx++) {
		rlink = get_rlink(wrole, idx);

		rtw_hal_ppdu_sts_cfg(phl_info->hal, rlink->hw_band, false);
	}

	pstatus = RTW_PHL_STATUS_SUCCESS;

	return pstatus;
}

enum rtw_phl_status phl_wow_init_postcfg(struct phl_wow_info *wow_info)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_FAILURE;
	struct phl_info_t *phl_info = wow_info->phl_info;
	struct phl_hci_trx_ops *trx_ops = phl_info->hci_trx_ops;
	struct rtw_phl_stainfo_t *sta = wow_info->sta;
#ifdef CONFIG_SYNC_INTERRUPT
	struct rtw_phl_evt_ops *evt_ops = &phl_info->phl_com->evt_ops;
#endif /* CONFIG_SYNC_INTERRUPT */

	/* disable interrupt */
#ifdef CONFIG_SYNC_INTERRUPT
	evt_ops->set_interrupt_caps(phl_to_drvpriv(phl_info), false);
#else
	rtw_hal_disable_interrupt(phl_info->phl_com, phl_info->hal);
#endif /* CONFIG_SYNC_INTERRUPT */

	_wow_stop_datapath(phl_info, PHL_CTRL_RX);

	pstatus = _init_postcfg(phl_info);
	if (RTW_PHL_STATUS_SUCCESS != pstatus)
		PHL_ERR("[wow] _init_postcfg failed.\n");

	/* configure wow sleep */
	hstatus = rtw_hal_cfg_wow_sleep(phl_info->hal, true);
	if (RTW_HAL_STATUS_SUCCESS != hstatus)
		return RTW_PHL_STATUS_FAILURE;

	/* forward rx packet to host by setting rx filter */
	pstatus = _init_postcfg_set_rxfltr(phl_info);

	trx_ops->trx_reset(phl_info, PHL_CTRL_TX | PHL_CTRL_RX);

	/* notify reorder sleep */
	phl_notify_reorder_sleep(phl_info, sta);

	return pstatus;
}

static void _wow_initialize_interrupt(struct phl_info_t *phl_info)
{
#ifdef CONFIG_SYNC_INTERRUPT
	struct rtw_phl_evt_ops *evt_ops = &phl_info->phl_com->evt_ops;
#endif /* CONFIG_SYNC_INTERRUPT */

	rtw_hal_init_int_default_value(phl_info->phl_com, phl_info->hal, INT_SET_OPT_HAL_INIT);

#ifdef CONFIG_SYNC_INTERRUPT
	evt_ops->set_interrupt_caps(phlcom_to_drvpriv(phl_info->phl_com), true);
#else
	rtw_hal_enable_interrupt(phl_info->phl_com, phl_info->hal);
#endif /* CONFIG_SYNC_INTERRUPT */
}

enum rtw_phl_status phl_wow_init(struct phl_wow_info *wow_info)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_SUCCESS;
	struct phl_info_t *phl_info = wow_info->phl_info;

	if (RTW_HAL_STATUS_SUCCESS !=
		rtw_hal_wow_init(phl_info->phl_com, phl_info->hal, wow_info->sta))
		pstatus = RTW_PHL_STATUS_FAILURE;

	return pstatus;
}

enum rtw_phl_status phl_wow_deinit(struct phl_wow_info *wow_info)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_SUCCESS;
	struct phl_info_t *phl_info = wow_info->phl_info;

	if (RTW_HAL_STATUS_SUCCESS !=
		rtw_hal_wow_deinit(phl_info->phl_com, phl_info->hal, wow_info->sta))
		pstatus = RTW_PHL_STATUS_FAILURE;

	return pstatus;
}

static void _phl_indic_wake_sec_upd(struct phl_wow_info *wow_info, u8 aoac_report_get_ok, u8 rx_ready)
{
	struct phl_info_t *phl_info = wow_info->phl_info;
	struct rtw_phl_evt_ops *ops = &phl_info->phl_com->evt_ops;
	void *drv_priv = phl_to_drvpriv(phl_info);

	if (NULL != ops->wow_handle_sec_info_update)
		ops->wow_handle_sec_info_update(drv_priv, &wow_info->aoac_info, aoac_report_get_ok, rx_ready);
	else
		PHL_TRACE(COMP_PHL_WOW, _PHL_ERR_, "[wow] %s : evt_ops->wow_handle_sec_info_update is NULL.\n"
			, __func__);
}

static void _phl_handle_aoac_rpt_action(struct phl_wow_info *wow_info, bool rx_ready)
{
	struct phl_info_t *phl_info = wow_info->phl_info;
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_FAILURE;
	u8 aoac_report_get_ok = false;
	static u8 phase_0_ok = false;

	if (wow_info->wow_wake_info.pairwise_sec_algo) {
		if (rx_ready == false) {
			/* phase 0 */
			hstatus = rtw_hal_get_wow_aoac_rpt(phl_info->hal, &wow_info->aoac_info, rx_ready);
			aoac_report_get_ok = (hstatus == RTW_HAL_STATUS_SUCCESS) ? true : false;
			_phl_indic_wake_sec_upd(wow_info, aoac_report_get_ok, rx_ready);

			phase_0_ok = aoac_report_get_ok;
		}

		if (rx_ready == true) {
			/* phase 1 */
			if (phase_0_ok) {
				hstatus = rtw_hal_get_wow_aoac_rpt(phl_info->hal, &wow_info->aoac_info, rx_ready);
				aoac_report_get_ok = (hstatus == RTW_HAL_STATUS_SUCCESS) ? true : false;
				_phl_indic_wake_sec_upd(wow_info, aoac_report_get_ok, rx_ready);
			}

			phase_0_ok = false;

			wow_info->aoac_info.rpt_fail = (aoac_report_get_ok == false) ? true : false;
		}
	}
}

static enum rtw_phl_status _phl_indic_wake_rsn(struct phl_wow_info *wow_info)
{
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_SUCCESS;
	struct phl_info_t *phl_info = wow_info->phl_info;
	struct rtw_phl_evt_ops *evt_ops = &(phl_info->phl_com->evt_ops);

	FUNCIN_WSTS(phl_status);

	if (NULL != evt_ops->indicate_wake_rsn) {
		evt_ops->indicate_wake_rsn(phl_to_drvpriv(phl_info), wow_info->wake_rsn);
	}

	FUNCOUT_WSTS(phl_status);

	return phl_status;
}

static void _phl_get_nlo_rpt(struct phl_wow_info *wow_info)
{
	struct phl_info_t *phl_info = wow_info->phl_info;
	struct rtw_wifi_role_link_t *rlink = NULL;
	u8 idx = 0;

	if (wow_info->nlo_info.nlo_en) {
		for (idx = 0; idx < wow_info->sta->wrole->rlink_num; idx++) {
			rlink = get_rlink(wow_info->sta->wrole, idx);
			rtw_hal_wow_cfg_nlo(phl_info->hal, SCAN_OFLD_OP_RPT,
			                    wow_info->sta->macid,
			                    rlink->hw_band,
			                    rlink->hw_port, NULL);
		}
	}
}

/*
 * return role map of excluded role from suspension
 */
u8
phl_get_wow_excld_susp_role_map(struct phl_info_t *phl_i)
{
	struct phl_wow_info *wow_info = phl_to_wow_info(phl_i);

	return BIT(wow_info->sta->wrole->id);
}

void phl_wow_handle_wake_rsn(struct phl_wow_info *wow_info, u8 *reset)
{
	struct phl_info_t *phl_info = wow_info->phl_info;

	rtw_hal_get_wake_rsn(phl_info->hal, &wow_info->wake_rsn, reset);
	_phl_indic_wake_rsn(wow_info);
}

#ifdef CONFIG_PCI_HCI
enum rtw_phl_status _deinit_precfg(struct phl_info_t *phl_info)
{
#ifdef DBG_RST_BDRAM_TIME
	u32 rst_bdram_start = _os_get_cur_time_ms();
#endif /* DBG_RST_BDRAM_TIME */

	rtw_hal_clear_bdidx(phl_info->hal);

#ifdef DBG_RST_BDRAM_TIME
	rst_bdram_start = _os_get_cur_time_ms();
#endif
	rtw_hal_rst_bdram(phl_info->hal);

#ifdef DBG_RST_BDRAM_TIME
	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] %s : Reset bdram takes %u (ms).\n"
		, __func__, phl_get_passing_time_ms(rst_bdram_start));
#endif

	rtw_hal_cfg_txhci(phl_info->hal, true);
	rtw_hal_cfg_rxhci(phl_info->hal, true);

	rtw_hal_wow_cfg_txdma(phl_info->hal, true);

	return RTW_PHL_STATUS_SUCCESS;
}
#elif defined(CONFIG_USB_HCI)
enum rtw_phl_status _deinit_precfg(struct phl_info_t *phl_info)
{
	struct phl_hci_trx_ops *trx_ops = phl_info->hci_trx_ops;

	trx_ops->trx_cfg(phl_info);

	return RTW_PHL_STATUS_SUCCESS;
}
#elif defined(CONFIG_SDIO_HCI)
enum rtw_phl_status _deinit_precfg(struct phl_info_t *phl_info)
{
	return RTW_PHL_STATUS_SUCCESS;
}
#endif

enum rtw_phl_status phl_wow_deinit_precfg(struct phl_wow_info *wow_info)
{
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_SUCCESS;
	struct phl_info_t *phl_info = wow_info->phl_info;

	_deinit_precfg(phl_info);

	rtw_hal_cfg_wow_sleep(phl_info->hal, false);

	_phl_get_nlo_rpt(wow_info);

	_phl_handle_aoac_rpt_action(wow_info, false);

	_wow_start_datapath(phl_info, PHL_CTRL_RX);

	_wow_initialize_interrupt(phl_info);

	_phl_handle_aoac_rpt_action(wow_info, true);

	return phl_status;
}

void phl_reset_wow_info(struct phl_wow_info *wow_info)
{
	struct phl_info_t *phl_info = wow_info->phl_info;
	void *d = phl_to_drvpriv(phl_info);

	wow_info->func_en = 0;
	wow_info->no_link_mode = 0;
	wow_info->op_mode = RTW_WOW_OP_NONE;
	wow_info->mac_pwr = RTW_MAC_PWR_NONE;
	wow_info->ps_pwr_lvl = PS_PWR_LVL_PWRON;

	_os_mem_set(d, &wow_info->err, 0, sizeof(struct phl_wow_error));
	_os_mem_set(d, &wow_info->keep_alive_info, 0, sizeof(struct rtw_keep_alive_info));
	_os_mem_set(d, &wow_info->disc_det_info, 0, sizeof(struct rtw_disc_det_info));
	_os_mem_set(d, &wow_info->nlo_info, 0, sizeof(struct rtw_nlo_info));
	_os_mem_set(d, &wow_info->arp_ofld_info, 0, sizeof(struct rtw_arp_ofld_info));
	_os_mem_set(d, &wow_info->ndp_ofld_info, 0, sizeof(struct rtw_ndp_ofld_info));
	_os_mem_set(d, &wow_info->gtk_ofld_info, 0, sizeof(struct rtw_gtk_ofld_info));
	_os_mem_set(d, &wow_info->realwow_info, 0, sizeof(struct rtw_realwow_info));
	_os_mem_set(d, &wow_info->wow_wake_info, 0, sizeof(struct rtw_wow_wake_info));
	/* _os_mem_set(d, &wow_info->pattern_match_info, 0, sizeof(struct rtw_pattern_match_info)); */
	_os_mem_set(d, &wow_info->wow_gpio, 0, sizeof(struct rtw_wow_gpio_info));
	_os_mem_set(d, &wow_info->periodic_wake_info, 0, sizeof(struct rtw_periodic_wake_info));
	_os_mem_set(d, &wow_info->aoac_info, 0, sizeof(struct rtw_aoac_report));

	wow_info->wake_rsn = RTW_MAC_WOW_UNKNOWN;
}

enum rtw_phl_status phl_wow_deinit_postcfg(struct phl_wow_info *wow_info)
{
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_SUCCESS;
	struct phl_info_t *phl_info = wow_info->phl_info;
	struct rtw_wifi_role_t *wrole = wow_info->sta->wrole;
	struct rtw_wifi_role_link_t *rlink = NULL;
	u8 idx = 0;

	_wow_start_datapath(phl_info, PHL_CTRL_TX);

	/* enable ppdu sts */
	for (idx = 0; idx < wrole->rlink_num; idx++) {
		rlink = get_rlink(wrole, idx);
		rtw_hal_ppdu_sts_cfg(phl_info->hal, rlink->hw_band, true);
	}

	return phl_status;
}

enum rtw_phl_status _phl_wow_cfg_pkt_ofld(struct phl_wow_info *wow_info, u8 pkt_type, u8 *pkt_id, void *buf)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	u16 macid = wow_info->sta->macid;
	u32 *token;

	switch(pkt_type) {
	case PKT_TYPE_NULL_DATA:
		token = &wow_info->null_pkt_token;
		break;
	case PKT_TYPE_ARP_RSP:
		token = &wow_info->arp_pkt_token;
		break;
	case PKT_TYPE_NDP:
		token = &wow_info->ndp_pkt_token;
		break;
	case PKT_TYPE_EAPOL_KEY:
		token = &wow_info->eapol_key_pkt_token;
		break;
	case PKT_TYPE_SA_QUERY:
		token = &wow_info->sa_query_pkt_token;
		break;
	case PKT_TYPE_REALWOW_KAPKT:
		token = &wow_info->kapkt_pkt_token;
		break;
	case PKT_TYPE_REALWOW_ACK:
		token = &wow_info->ack_pkt_token;
		break;
	case PKT_TYPE_REALWOW_WP:
		token = &wow_info->wp_token;
		break;
	case PKT_TYPE_PROBE_REQ:
		token = &wow_info->probe_req_pkt_token;
		break;
	default:
		PHL_TRACE(COMP_PHL_WOW, _PHL_ERR_, "[wow] %s : unknown pkt_type %d.\n"
			, __func__, pkt_type);
		return pstatus;
	}

	pstatus = rtw_phl_pkt_ofld_request(wow_info->phl_info, macid, pkt_type, token, buf, __func__);

	if (pstatus == RTW_PHL_STATUS_SUCCESS)
		*pkt_id = phl_pkt_ofld_get_id(wow_info->phl_info, macid, pkt_type);

	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] %s : pkt_type %s, pkt_id %d, token %u, status(%u)\n",
		__func__, phl_get_pkt_ofld_str(pkt_type), *pkt_id, *token, pstatus);

	return pstatus;
}

static enum rtw_phl_status _phl_wow_cfg_nlo(struct phl_wow_info *wow_info)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_SUCCESS;
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_FAILURE;
	struct phl_info_t *phl_info = wow_info->phl_info;
	struct rtw_phl_stainfo_t *sta = wow_info->sta;

	do {

		/* always stop first */
		hstatus = rtw_hal_wow_cfg_nlo(phl_info->hal, SCAN_OFLD_OP_STOP,
		                              sta->macid, sta->rlink->hw_band,
		                              sta->rlink->hw_port,
		                              &wow_info->nlo_info);
		if (RTW_HAL_STATUS_SUCCESS != hstatus) {
			pstatus = RTW_PHL_STATUS_FAILURE;
			break;
		}

		/* construct channel list and offload to fw */
		hstatus = rtw_hal_wow_cfg_nlo_chnl_list(phl_info->hal,
		                                        &wow_info->nlo_info);
		if (RTW_HAL_STATUS_SUCCESS != hstatus) {
			pstatus = RTW_PHL_STATUS_FAILURE;
			break;
		}

		hstatus = rtw_hal_wow_cfg_nlo(phl_info->hal, SCAN_OFLD_OP_START,
		                              sta->macid, sta->rlink->hw_band,
		                              sta->rlink->hw_port, &wow_info->nlo_info);
		if (RTW_HAL_STATUS_SUCCESS != hstatus) {
			pstatus = RTW_PHL_STATUS_FAILURE;
			break;
		}

	} while (0);

	return pstatus;
}

enum rtw_phl_status phl_wow_func_en(struct phl_wow_info *wow_info)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_FAILURE;
	struct phl_info_t *phl_info = wow_info->phl_info;
	struct rtw_phl_stainfo_t *sta = wow_info->sta;
	struct rtw_pkt_ofld_null_info null_info = {0};
	struct rtw_pkt_ofld_arp_rsp_info arp_rsp_info = {0};
	struct rtw_pkt_ofld_na_info na_info = {0};
	struct rtw_pkt_ofld_eapol_key_info eapol_key_info = {0};
	struct rtw_pkt_ofld_sa_query_info sa_query_info = {0};
	struct rtw_pkt_ofld_realwow_kapkt_info kapkt_info = {0};
	struct rtw_pkt_ofld_realwow_ack_info ack_info = {0};
	struct rtw_pkt_ofld_realwow_wp_info wakeup_info = {0};
	struct rtw_pkt_ofld_probe_req_info probe_req_info = {0};
	struct rtw_hal_wow_cfg cfg;

	FUNCIN();

	if (!wow_info->wow_wake_info.wow_en) {
		PHL_WARN("%s : wow func is not enabled!\n", __func__);
		return pstatus;
	}

	do {

		hstatus = rtw_hal_reset_pkt_ofld_state(phl_info->hal);
		if (RTW_HAL_STATUS_SUCCESS != hstatus) {
			pstatus = RTW_PHL_STATUS_FAILURE;
			break;
		}

		if (wow_info->arp_ofld_info.arp_en) {
			_phl_cfg_pkt_ofld_arp_rsp_info(wow_info, sta, &arp_rsp_info);

			pstatus = _phl_wow_cfg_pkt_ofld(wow_info,
					PKT_TYPE_ARP_RSP,
					&wow_info->arp_ofld_info.arp_rsp_id,
					(void *)&arp_rsp_info);

			if (pstatus != RTW_PHL_STATUS_SUCCESS)
				break;
		}

		if (wow_info->keep_alive_info.keep_alive_en) {
			if (wow_info->keep_alive_info.keep_alive_pkt_type == PKT_TYPE_ARP_RSP) {
				wow_info->keep_alive_info.keep_alive_pkt_id = wow_info->arp_ofld_info.arp_rsp_id;
			} else if (wow_info->keep_alive_info.keep_alive_pkt_type == PKT_TYPE_NULL_DATA) {
				_phl_cfg_pkt_ofld_null_info(wow_info, sta, &null_info);

				pstatus = _phl_wow_cfg_pkt_ofld(wow_info,
						PKT_TYPE_NULL_DATA,
						&wow_info->keep_alive_info.keep_alive_pkt_id,
						(void *)&null_info);

				if (pstatus != RTW_PHL_STATUS_SUCCESS)
					break;
			} else {
				PHL_ERR("[wow] %s wrong keep_alive_pkt_type (%d)\n",
					__func__, wow_info->keep_alive_info.keep_alive_pkt_type);
				pstatus = RTW_PHL_STATUS_FAILURE;
				break;
			}
		}

		if (wow_info->ndp_ofld_info.ndp_en) {

			_phl_cfg_pkt_ofld_na_info(wow_info, sta, &na_info);

			pstatus = _phl_wow_cfg_pkt_ofld(wow_info,
					PKT_TYPE_NDP, &wow_info->ndp_ofld_info.ndp_id,
					(void *)&na_info);

			if (pstatus != RTW_PHL_STATUS_SUCCESS)
				break;
		}

		if (wow_info->gtk_ofld_info.gtk_en) {
			_phl_cfg_pkt_ofld_eapol_key_info(wow_info, sta, &eapol_key_info);

			pstatus = _phl_wow_cfg_pkt_ofld(wow_info,
					PKT_TYPE_EAPOL_KEY, &wow_info->gtk_ofld_info.gtk_rsp_id,
					(void *)&eapol_key_info);

			if (pstatus != RTW_PHL_STATUS_SUCCESS)
				break;

			if (wow_info->gtk_ofld_info.ieee80211w_en) {
				_phl_cfg_pkt_ofld_sa_query_info(wow_info, sta, &sa_query_info);

				pstatus = _phl_wow_cfg_pkt_ofld(wow_info,
					PKT_TYPE_SA_QUERY, &wow_info->gtk_ofld_info.sa_query_id,
					(void *)&sa_query_info);

				if (pstatus != RTW_PHL_STATUS_SUCCESS)
					break;
			}
		}

		if (wow_info->realwow_info.realwow_en) {

			/* realwow keep alive */
			_phl_cfg_pkt_ofld_realwow_kapkt_info(wow_info, sta, &kapkt_info);

			pstatus = _phl_wow_cfg_pkt_ofld(wow_info,
					PKT_TYPE_REALWOW_KAPKT,
					&wow_info->realwow_info.keepalive_id,
					(void *)&kapkt_info);

			if (pstatus != RTW_PHL_STATUS_SUCCESS)
				break;

			/* realwow ack */
			_phl_cfg_pkt_ofld_realwow_ack_info(wow_info, &ack_info);

			pstatus = _phl_wow_cfg_pkt_ofld(wow_info,
					PKT_TYPE_REALWOW_ACK,
					&wow_info->realwow_info.ack_pattern_id,
					(void *)&ack_info);

			if (pstatus != RTW_PHL_STATUS_SUCCESS)
				break;

			/* realwow wake up */
			_phl_cfg_pkt_ofld_realwow_wp_info(wow_info, &wakeup_info);

			pstatus = _phl_wow_cfg_pkt_ofld(wow_info,
					PKT_TYPE_REALWOW_WP,
					&wow_info->realwow_info.wakeup_pattern_id,
					(void *)&wakeup_info);

			if (pstatus != RTW_PHL_STATUS_SUCCESS)
				break;
		}

		if (wow_info->nlo_info.nlo_en) {

			_phl_cfg_pkt_ofld_probe_req_info(wow_info, sta, &probe_req_info);

			pstatus = _phl_wow_cfg_pkt_ofld(wow_info,
			                                PKT_TYPE_PROBE_REQ,
			                                &wow_info->nlo_info.probe_req_id,
			                                (void *)&probe_req_info);
			if (pstatus != RTW_PHL_STATUS_SUCCESS)
				break;

			pstatus = _phl_wow_cfg_nlo(wow_info);
			if (pstatus != RTW_PHL_STATUS_SUCCESS)
				break;
		}

		cfg.keep_alive_cfg = &wow_info->keep_alive_info;
		cfg.disc_det_cfg = &wow_info->disc_det_info;
		cfg.nlo_cfg = &wow_info->nlo_info;
		cfg.arp_ofld_cfg = &wow_info->arp_ofld_info;
		cfg.ndp_ofld_cfg = &wow_info->ndp_ofld_info;
		cfg.gtk_ofld_cfg = &wow_info->gtk_ofld_info;
		cfg.realwow_cfg = &wow_info->realwow_info;
		cfg.wow_wake_cfg = &wow_info->wow_wake_info;
		cfg.pattern_match_info = &wow_info->pattern_match_info;
		cfg.wow_gpio = &wow_info->wow_gpio;
		cfg.periodic_wake_cfg = &wow_info->periodic_wake_info;

		hstatus = rtw_hal_wow_func_en(phl_info->phl_com, phl_info->hal, sta->macid, &cfg);
		if (hstatus != RTW_HAL_STATUS_SUCCESS) {
			PHL_ERR("rtw_hal_wow_func_en fail, status (%u)\n", hstatus);
			pstatus = RTW_PHL_STATUS_FAILURE;
			break;
		}

		hstatus = rtw_hal_wow_func_start(phl_info->phl_com, phl_info->hal, sta->macid, &cfg);
		if (hstatus != RTW_HAL_STATUS_SUCCESS) {
			PHL_ERR("rtw_hal_wow_func_start fail, status (%u)\n", hstatus);
			pstatus = RTW_PHL_STATUS_FAILURE;
			break;
		}

		wow_info->func_en = true;
		pstatus = RTW_PHL_STATUS_SUCCESS;

	} while (0);

	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] %s status (%u).\n", __func__, pstatus);

	return pstatus;
}

void phl_wow_func_dis(struct phl_wow_info *wow_info)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_FAILURE;
	struct phl_info_t *phl_info = wow_info->phl_info;
	struct rtw_phl_stainfo_t *sta = wow_info->sta;
	struct rtw_hal_wow_cfg cfg;

	if (!wow_info->wow_wake_info.wow_en) {
		PHL_WARN("%s : wow func is not enabled!\n", __func__);
		return;
	}

	cfg.keep_alive_cfg = &wow_info->keep_alive_info;
	cfg.disc_det_cfg = &wow_info->disc_det_info;
	cfg.nlo_cfg = &wow_info->nlo_info;
	cfg.arp_ofld_cfg = &wow_info->arp_ofld_info;
	cfg.ndp_ofld_cfg = &wow_info->ndp_ofld_info;
	cfg.gtk_ofld_cfg = &wow_info->gtk_ofld_info;
	cfg.realwow_cfg = &wow_info->realwow_info;
	cfg.wow_wake_cfg = &wow_info->wow_wake_info;
	cfg.pattern_match_info = &wow_info->pattern_match_info;
	cfg.wow_gpio = &wow_info->wow_gpio;
	cfg.periodic_wake_cfg = &wow_info->periodic_wake_info;

	hstatus = rtw_hal_wow_func_dis(phl_info->phl_com, phl_info->hal, sta->macid,
	                               &cfg);
	if (hstatus != RTW_HAL_STATUS_SUCCESS)
		PHL_ERR("rtw_hal_wow_func_dis fail, status (%u)\n", hstatus);

	if (wow_info->arp_ofld_info.arp_en) {
		rtw_phl_pkt_ofld_cancel(phl_info, sta->macid,
							PKT_TYPE_ARP_RSP, &wow_info->arp_pkt_token);
	}

	if (wow_info->keep_alive_info.keep_alive_en &&
	    wow_info->keep_alive_info.keep_alive_pkt_type == PKT_TYPE_NULL_DATA) {
		rtw_phl_pkt_ofld_cancel(phl_info, sta->macid,
							PKT_TYPE_NULL_DATA, &wow_info->null_pkt_token);
	}

	if (wow_info->ndp_ofld_info.ndp_en) {
		rtw_phl_pkt_ofld_cancel(phl_info, sta->macid,
							PKT_TYPE_NDP, &wow_info->ndp_pkt_token);
	}

	if (wow_info->gtk_ofld_info.gtk_en) {
		rtw_phl_pkt_ofld_cancel(phl_info, sta->macid,
							PKT_TYPE_EAPOL_KEY, &wow_info->eapol_key_pkt_token);
		if (wow_info->gtk_ofld_info.ieee80211w_en) {
			rtw_phl_pkt_ofld_cancel(phl_info, sta->macid,
								PKT_TYPE_SA_QUERY, &wow_info->sa_query_pkt_token);
		}
	}

	if (wow_info->realwow_info.realwow_en) {
		rtw_phl_pkt_ofld_cancel(phl_info, sta->macid,
					PKT_TYPE_REALWOW_KAPKT, &wow_info->kapkt_pkt_token);
		rtw_phl_pkt_ofld_cancel(phl_info, sta->macid,
					PKT_TYPE_REALWOW_ACK, &wow_info->ack_pkt_token);
		rtw_phl_pkt_ofld_cancel(phl_info, sta->macid,
					PKT_TYPE_REALWOW_WP, &wow_info->wp_token);
	}

	if (wow_info->nlo_info.nlo_en) {

		pstatus = rtw_phl_pkt_ofld_cancel(phl_info, sta->macid,
		                              PKT_TYPE_PROBE_REQ,
		                              &wow_info->probe_req_pkt_token);

		hstatus = rtw_hal_wow_cfg_nlo(phl_info->hal, SCAN_OFLD_OP_STOP,
		                              sta->macid, sta->rlink->hw_band,
		                              sta->rlink->hw_port,
		                              &wow_info->nlo_info);
	}


	hstatus = rtw_hal_wow_func_stop(phl_info->phl_com, phl_info->hal, sta->macid);
	if (hstatus != RTW_HAL_STATUS_SUCCESS)
		PHL_ERR("rtw_hal_wow_func_stop fail, status (%u)\n", hstatus);

	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] %s done.\n", __func__);
}

void phl_wow_decide_op_mode(struct phl_wow_info *wow_info,
                            struct rtw_phl_stainfo_t *sta)
{
	enum mlme_state mstat = sta->wrole->mstate;
	struct rtw_ps_cap_t *ps_cap = _get_ps_cap(wow_info->phl_info);

	wow_info->sta = sta;
	wow_info->ps_pwr_lvl = PS_PWR_LVL_PWRON;

	if (mstat == MLME_NO_LINK && wow_info->no_link_mode) {
		wow_info->op_mode = RTW_WOW_OP_DISCONNECT_STBY;
		#ifdef CONFIG_POWER_SAVE
		if (ps_cap->ips_wow_en)
			wow_info->ps_pwr_lvl = phl_ps_judge_pwr_lvl(ps_cap->ips_wow_cap,
			                                            PS_MODE_IPS, true);
		#endif
	} else if (mstat == MLME_LINKED) {
		wow_info->op_mode = RTW_WOW_OP_CONNECT_STBY;
		#ifdef CONFIG_POWER_SAVE
		if (ps_cap->lps_wow_en)
			wow_info->ps_pwr_lvl = phl_ps_judge_pwr_lvl(ps_cap->lps_wow_cap,
			                                            PS_MODE_LPS, true);
		#endif
	} else {
		wow_info->op_mode = RTW_WOW_OP_PWR_DOWN;
	}

	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] %s op mode set to %d, pwr lvl %s.\n.",
			  __func__, wow_info->op_mode, phl_ps_pwr_lvl_to_str(wow_info->ps_pwr_lvl));
}

#ifdef CONFIG_POWER_SAVE
/**
 * phl_wow_ps_proto_cfg - set the ps protocol under wowlan
 * @wow_info: see struct phl_wow_info
 * @enter_ps: enter lps or not
 *
 * return enum rtw_phl_status
 */
enum rtw_phl_status phl_wow_ps_proto_cfg(struct phl_wow_info *wow_info, bool enter_ps)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_SUCCESS;
	struct phl_info_t *phl_info = wow_info->phl_info;
	struct ps_cfg cfg = {0};
	struct rtw_ps_cap_t *ps_cap = _get_ps_cap(phl_info);

	if (wow_info->op_mode == RTW_WOW_OP_DISCONNECT_STBY) {
		/* IPS */
		if (ps_cap->ips_wow_en) {
			cfg.macid = wow_info->sta->macid;
			pstatus = phl_ps_ips_cfg(phl_info, &cfg, enter_ps);
		}
	} else if (wow_info->op_mode == RTW_WOW_OP_CONNECT_STBY) {
		/* IPS for no wake up after disconnection */
		if (ps_cap->ips_wow_en &&
		    !wow_info->wow_wake_info.deauth_wakeup) {
			cfg.macid = wow_info->sta->macid;
			pstatus = phl_ps_ips_cfg(phl_info, &cfg, enter_ps);
			if (pstatus != RTW_PHL_STATUS_SUCCESS)
				goto exit;
		}

		/* LPS */
		if (ps_cap->lps_wow_en) {
			cfg.macid = wow_info->sta->macid;
			cfg.awake_interval = ps_cap->lps_wow_awake_interval;
			cfg.listen_bcn_mode = ps_cap->lps_wow_listen_bcn_mode;
			cfg.smart_ps_mode = ps_cap->lps_wow_smart_ps_mode;
			cfg.bcnnohit_en = ps_cap->lps_wow_bcnnohit_en;
			pstatus = phl_ps_lps_cfg(phl_info, &cfg, enter_ps);
		}
	} else {
		PHL_ERR("%s : undefined wowlan op mode.\n", __func__);
	}

exit:
	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] %s : op mode %d, enter ps %d, pwr lvl %s.\n.",
			  __func__, wow_info->op_mode, enter_ps, phl_ps_pwr_lvl_to_str(wow_info->ps_pwr_lvl));

	return pstatus;
}

/**
 * phl_wow_ps_pwr_ntfy - notify the power level when enter low power
 * @wow_info: see struct phl_wow_info
 * @enter_ps: enter low power or not
 *
 */
void phl_wow_ps_pwr_ntfy(struct phl_wow_info *wow_info, bool enter_ps)
{
	struct phl_info_t *phl_info = wow_info->phl_info;

	if (wow_info->ps_pwr_lvl == PS_PWR_LVL_PWRON)
		return;

	if (wow_info->op_mode == RTW_WOW_OP_DISCONNECT_STBY) {
		/* IPS */
	} else if (wow_info->op_mode == RTW_WOW_OP_CONNECT_STBY) {
		#ifdef CONFIG_BTCOEX
		rtw_hal_btc_radio_state_ntfy(phl_info->hal, (enter_ps == true ?
							BTC_RFCTRL_FW_CTRL : BTC_RFCTRL_WL_ON));
		#endif
	} else {
		PHL_ERR("%s : undefined wowlan op mode.\n", __func__);
	}

	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] %s : op mode %d, enter ps %d, pwr lvl %s.\n.",
			  __func__, wow_info->op_mode, enter_ps, phl_ps_pwr_lvl_to_str(wow_info->ps_pwr_lvl));
}


/**
 * phl_wow_ps_pwr_cfg - set the low power level under wowlan
 * @wow_info: see struct phl_wow_info
 * @enter_ps: enter low power or not
 *
 * returns enum rtw_phl_status
 */
enum rtw_phl_status phl_wow_ps_pwr_cfg(struct phl_wow_info *wow_info, bool enter_ps)
{
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_SUCCESS;
	struct phl_info_t *phl_info = wow_info->phl_info;

	if (wow_info->ps_pwr_lvl == PS_PWR_LVL_PWRON)
		return hstatus;

	if (wow_info->op_mode == RTW_WOW_OP_DISCONNECT_STBY) {
		hstatus = rtw_hal_ps_pwr_lvl_cfg(phl_info->phl_com, phl_info->hal,
					(enter_ps == true ? wow_info->ps_pwr_lvl : PS_PWR_LVL_PWRON));
	} else if (wow_info->op_mode == RTW_WOW_OP_CONNECT_STBY) {
		hstatus = rtw_hal_ps_pwr_lvl_cfg(phl_info->phl_com, phl_info->hal,
					(enter_ps == true ? wow_info->ps_pwr_lvl : PS_PWR_LVL_PWRON));
	} else {
		PHL_ERR("%s : undefined wowlan op mode.\n", __func__);
	}

	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] %s : op mode %d, enter ps %d, pwr lvl %s.\n.",
			  __func__, wow_info->op_mode, enter_ps, phl_ps_pwr_lvl_to_str(wow_info->ps_pwr_lvl));

	return (hstatus == RTW_HAL_STATUS_SUCCESS ?
			RTW_PHL_STATUS_SUCCESS : RTW_PHL_STATUS_FAILURE);
}
#endif /* CONFIG_POWER_SAVE */

#define case_rsn(rsn) \
	case RTW_MAC_WOW_##rsn: return #rsn

const char *rtw_phl_get_wow_rsn_str(void *phl, u8 wake_rsn)
{
	switch (wake_rsn) {
	case_rsn(UNKNOWN);
	case_rsn(RX_PAIRWISEKEY);
	case_rsn(RX_GTK);
	case_rsn(RX_FOURWAY_HANDSHAKE);
	case_rsn(RX_DISASSOC);
	case_rsn(RX_DEAUTH);
	case_rsn(RX_ARP_REQUEST);
	case_rsn(RX_NS);
	case_rsn(RX_EAPREQ_IDENTIFY);
	case_rsn(FW_DECISION_DISCONNECT);
	case_rsn(RX_MAGIC_PKT);
	case_rsn(RX_UNICAST_PKT);
	case_rsn(RX_PATTERN_PKT);
	case_rsn(RTD3_SSID_MATCH);
	case_rsn(RX_DATA_PKT);
	case_rsn(RX_SSDP_MATCH);
	case_rsn(RX_WSD_MATCH);
	case_rsn(RX_SLP_MATCH);
	case_rsn(RX_LLTD_MATCH);
	case_rsn(RX_MDNS_MATCH);
	case_rsn(RX_REALWOW_V2_WAKEUP_PKT);
	case_rsn(RX_REALWOW_V2_ACK_LOST);
	case_rsn(RX_REALWOW_V2_TX_KAPKT);
	case_rsn(ENABLE_FAIL_DMA_IDLE);
	case_rsn(ENABLE_FAIL_DMA_PAUSE);
	case_rsn(RTIME_FAIL_DMA_IDLE);
	case_rsn(RTIME_FAIL_DMA_PAUSE);
	case_rsn(RX_SNMP_MISMATCHED_PKT);
	case_rsn(RX_DESIGNATED_MAC_PKT);
	case_rsn(NLO_SSID_MACH);
	case_rsn(AP_OFFLOAD_WAKEUP);
	case_rsn(DMAC_ERROR_OCCURRED);
	case_rsn(EXCEPTION_OCCURRED);
	case_rsn(L0_TO_L1_ERROR_OCCURRED);
	case_rsn(ASSERT_OCCURRED);
	case_rsn(L2_ERROR_OCCURRED);
	case_rsn(WDT_TIMEOUT_WAKE);
	case_rsn(NO_WAKE_RX_PAIRWISEKEY);
	case_rsn(NO_WAKE_RX_GTK);
	case_rsn(NO_WAKE_RX_DISASSOC);
	case_rsn(NO_WAKE_RX_DEAUTH);
	case_rsn(NO_WAKE_RX_EAPREQ_IDENTIFY);
	case_rsn(NO_WAKE_FW_DECISION_DISCONNECT);
	case_rsn(RX_ACTION);
	case_rsn(CLK_32K_UNLOCK);
	case_rsn(CLK_32K_LOCK);
	default:
		return "UNDEFINED";
	}
}

enum rtw_phl_status rtw_phl_cfg_wow_set_sw_gpio_mode(void *phl, struct rtw_wow_gpio_info *info)
{
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_SUCCESS;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_wow_info *wow_info = phl_to_wow_info(phl_info);
	struct rtw_wow_gpio_info *wow_gpio = &wow_info->wow_gpio;

	FUNCIN();

	wow_gpio->dev2hst_gpio = info->dev2hst_gpio;
	wow_gpio->dev2hst_gpio_mode = info->dev2hst_gpio_mode;
	phl_status = rtw_hal_set_sw_gpio_mode(phl_info->phl_com, phl_info->hal
		, wow_gpio->dev2hst_gpio_mode, wow_gpio->dev2hst_gpio);

	PHL_INFO("%s, gpio=%d, gpio_mode=%d\n", __FUNCTION__
		, wow_gpio->dev2hst_gpio, wow_gpio->dev2hst_gpio_mode);

	return phl_status;
}

enum rtw_phl_status rtw_phl_cfg_wow_sw_gpio_ctrl(void *phl, struct rtw_wow_gpio_info *info)
{
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_SUCCESS;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_wow_info *wow_info = phl_to_wow_info(phl_info);
	struct rtw_wow_gpio_info *wow_gpio = &wow_info->wow_gpio;

	FUNCIN();

	wow_gpio->dev2hst_high = info->dev2hst_high;
	phl_status = rtw_hal_sw_gpio_ctrl(phl_info->phl_com, phl_info->hal
		, wow_gpio->dev2hst_high, wow_gpio->dev2hst_gpio);

	PHL_INFO("%s, gpio=%d, output=%d\n", __FUNCTION__
		, wow_gpio->dev2hst_gpio, wow_gpio->dev2hst_high);

	return phl_status;
}

#endif /* CONFIG_WOWLAN */

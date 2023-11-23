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
#ifndef __RTW_AP_H_
#define __RTW_AP_H_


#ifdef CONFIG_AP_MODE

/* external function */
extern void rtw_indicate_sta_assoc_event(_adapter *padapter, struct sta_info *psta);
extern void rtw_indicate_sta_disassoc_event(_adapter *padapter, struct sta_info *psta);


void init_mlme_ap_info(_adapter *padapter);
void free_mlme_ap_info(_adapter *padapter);
u8 rtw_set_tim_ie(u8 dtim_cnt, u8 dtim_period
	, const u8 *tim_bmp, u8 tim_bmp_len, u8 *tim_ie);
/* void update_BCNTIM(_adapter *padapter); */
void rtw_add_bcn_ie(_adapter *padapter, WLAN_BSSID_EX *pnetwork, u8 index, u8 *data, u8 len);
void rtw_remove_bcn_ie(_adapter *padapter, WLAN_BSSID_EX *pnetwork, u8 index);
void rtw_add_bcn_ie_ex(_adapter *padapter, WLAN_BSSID_EX *pnetwork, u8 eid_ex, u8 *data, u8 len);
void rtw_remove_bcn_ie_ex(_adapter *padapter, WLAN_BSSID_EX *pnetwork, u8 index, u8* pindex_ex, u8 index_ex_len);
void _update_beacon(_adapter *padapter,struct _ADAPTER_LINK *padapter_link,
		u8 ie_id, u8 *oui, u8 tx, u8 flags, const char *tag);
#define rtw_update_beacon(adapter, adapter_link, ie_id, oui, tx, flags) _update_beacon((adapter), (adapter_link), (ie_id), (oui), (tx), (flags), __func__)
/*rtw_update_beacon - (flags) can set to normal enqueue (0) and RTW_CMDF_WAIT_ACK enqueue.
 (flags) = RTW_CMDF_DIRECTLY  is not currently implemented, it will do normal enqueue.*/

void expire_timeout_chk(_adapter *padapter);
void update_sta_info_apmode(_adapter *padapter, struct sta_info *psta);
void rtw_start_bss_hdl_after_chbw_decided(_adapter *adapter, struct _ADAPTER_LINK *adapter_link);
void rtw_core_ap_prepare(_adapter *padapter, struct createbss_parm *parm);
int rtw_check_beacon_data(_adapter *padapter, u8 *pbuf,  int len);
void rtw_ap_restore_network(_adapter *padapter);

#if CONFIG_RTW_MACADDR_ACL
void rtw_macaddr_acl_init(_adapter *adapter, u8 period);
void rtw_macaddr_acl_deinit(_adapter *adapter, u8 period);
void rtw_macaddr_acl_clear(_adapter *adapter, u8 period);
void rtw_set_macaddr_acl(_adapter *adapter, u8 period, int mode);
int rtw_acl_add_sta(_adapter *adapter, u8 period, const u8 *addr);
int rtw_acl_remove_sta(_adapter *adapter, u8 period, const u8 *addr);
#endif /* CONFIG_RTW_MACADDR_ACL */

u8 rtw_ap_set_sta_key(_adapter *adapter, const u8 *addr, u8 alg, const u8 *key, u8 keyid, u8 gk);
u8 rtw_ap_set_pairwise_key(_adapter *padapter, struct sta_info *psta);
int rtw_ap_set_group_key(_adapter *padapter,struct _ADAPTER_LINK *padapter_link,
			u8 *key, u8 alg, int keyid);
int rtw_ap_set_wep_key(_adapter *padapter, struct _ADAPTER_LINK *padapter_link,
			u8 *key, u8 keylen, int keyid, u8 set_tx);

#ifdef CONFIG_NATIVEAP_MLME
void associated_clients_update(_adapter *padapter, u8 updated, u32 sta_info_type);
void bss_cap_update_on_sta_join(_adapter *padapter, struct sta_info *psta);
u8 bss_cap_update_on_sta_leave(_adapter *padapter, struct sta_info *psta);
void sta_info_update(_adapter *padapter, struct sta_info *psta);
u8 ap_free_sta(_adapter *padapter, struct sta_info *psta, bool active, u16 reason, bool enqueue, u8 disassoc);
int rtw_sta_flush(_adapter *padapter, bool enqueue);
int rtw_ap_inform_ch_switch(_adapter *padapter, struct _ADAPTER_LINK *padapter_link,
			u8 new_ch, u8 ch_offset);
void start_ap_mode(_adapter *padapter);
void stop_ap_mode(_adapter *padapter);
#endif

void rtw_ap_update_clients_rainfo(struct _ADAPTER *a, enum phl_cmd_type flag);
void rtw_ap_update_bss_bchbw(_adapter *adapter, struct _ADAPTER_LINK *adapter_link,
				WLAN_BSSID_EX *bss, struct rtw_chan_def *chandef);
u8 rtw_ap_bchbw_decision(_adapter *adapter, u8 ifbmp, u8 excl_ifbmp, s8 req_band
	, s16 req_ch, s8 req_bw, s8 req_offset, struct rtw_chan_def *chdef);

#ifdef CONFIG_AUTO_AP_MODE
void rtw_auto_ap_rx_msg_dump(_adapter *padapter, union recv_frame *precv_frame, u8 *ehdr_pos);
extern void rtw_start_auto_ap(_adapter *adapter);
#endif /* CONFIG_AUTO_AP_MODE */

void rtw_ap_parse_sta_capability(_adapter *adapter, struct sta_info *sta, u8 *cap);
u16 rtw_ap_parse_sta_supported_rates(_adapter *adapter, struct sta_info *sta, u8 *tlv_ies, u16 tlv_ies_len);
u16 rtw_ap_parse_sta_security_ie(_adapter *adapter, struct sta_info *sta, struct rtw_ieee802_11_elems *elems);
void rtw_ap_parse_sta_wmm_ie(_adapter *adapter, struct sta_info *sta, u8 *tlv_ies, u16 tlv_ies_len);
void rtw_ap_parse_sta_ht_ie(_adapter *adapter, struct sta_info *sta, struct rtw_ieee802_11_elems *elems);
void rtw_ap_parse_sta_vht_ie(_adapter *adapter, struct sta_info *sta, struct rtw_ieee802_11_elems *elems);
void rtw_ap_parse_sta_he_ie(_adapter *adapter, struct sta_info *sta, struct rtw_ieee802_11_elems *elems);
void rtw_ap_parse_sta_multi_ap_ie(_adapter *adapter, struct sta_info *sta, u8 *ies, int ies_len);


void rtw_core_ap_start(_adapter *padapter, struct createbss_parm *parm);
void rtw_core_ap_swch_start(_adapter *padapter, struct createbss_parm *parm);

/* b2u flags */
#define RTW_AP_B2U_ALL		BIT0
#define RTW_AP_B2U_GA_UCAST	BIT1 /* WDS group addressed unicast frame, forward only */
#define RTW_AP_B2U_BCAST	BIT2
#define RTW_AP_B2U_IP_MCAST	BIT3

#define rtw_ap_src_b2u_policy_chk(flags, da) ( \
	(flags & RTW_AP_B2U_ALL) \
	|| ((flags & RTW_AP_B2U_BCAST) && is_broadcast_mac_addr(da)) \
	|| ((flags & RTW_AP_B2U_IP_MCAST) && (IP_MCAST_MAC(da) || ICMPV6_MCAST_MAC(da))) \
	)

#define rtw_ap_fwd_b2u_policy_chk(flags, da, gaucst) ( \
	(flags & RTW_AP_B2U_ALL) \
	|| ((flags & RTW_AP_B2U_GA_UCAST) && gaucst) \
	|| ((flags & RTW_AP_B2U_BCAST) && is_broadcast_mac_addr(da)) \
	|| ((flags & RTW_AP_B2U_IP_MCAST) && (IP_MCAST_MAC(da) || ICMPV6_MCAST_MAC(da))) \
	)

void dump_ap_b2u_flags(void *sel, _adapter *adapter);

int rtw_ap_addr_resolve(_adapter *adapter, u16 os_qid, struct xmit_frame *xframe, struct sk_buff *pkt, _list *f_list);
int rtw_ap_rx_data_validate_hdr(_adapter *adapter, union recv_frame *rframe, struct sta_info **sta);
int rtw_ap_rx_msdu_act_check(union recv_frame *rframe
	, const u8 *da, const u8 *sa
	, u8 *msdu, enum rtw_rx_llc_hdl llc_hdl
	, struct xmit_frame **fwd_frame, _list *f_list);

#ifdef CONFIG_BMC_TX_RATE_SELECT
void rtw_update_bmc_sta_tx_rate(_adapter *adapter);
#endif

void rtw_process_ht_action_smps(_adapter *padapter, u8 *ta, u8 ctrl_field);
void rtw_process_public_act_bsscoex(_adapter *padapter, u8 *pframe, uint frame_len);
#ifdef CONFIG_80211N_HT
int rtw_ht_operation_update(_adapter *padapter, struct _ADAPTER_LINK *padapter_link);
#endif /* CONFIG_80211N_HT */
u8 rtw_ap_sta_states_check(_adapter *adapter);

void rtw_ap_set_sta_wmode(_adapter *padapter, struct sta_info *sta);

#if defined(CONFIG_RTW_ACS) && defined(WKARD_ACS)
void rtw_acs_start(_adapter *padapter);
void rtw_acs_stop(_adapter *padapter);
#endif /* defined(CONFIG_RTW_ACS) && defined(WKARD_ACS) */
void rtw_ap_set_edca(_adapter *padapter, struct _ADAPTER_LINK *padapter_link, enum rtw_ac ac, u32 param);

#ifdef CONFIG_AP_CMD_DISPR

#define CMD_APSTART_ACQUIRE BIT0
#define CMD_APSTOP_ACQUIRE BIT1
#define	CMD_APSTOP_STARTED BIT2

enum rtw_phl_status rtw_ap_start_cmd(struct cmd_obj *p);
enum rtw_phl_status rtw_ap_stop_cmd(struct cmd_obj *p);
enum rtw_phl_status rtw_ap_add_del_sta_cmd(struct _ADAPTER *padapter);
enum rtw_phl_status rtw_free_bcn_entry(struct _ADAPTER *padapter);

bool rtw_add_del_sta_cmd_check(struct _ADAPTER *padapter, unsigned char *MacAddr, bool add_sta);
void rtw_cmd_ap_add_del_sta_req_init(struct _ADAPTER *padapter);
void rtw_cmd_ap_add_del_sta_req_free(struct _ADAPTER *padapter);

#endif
#endif /* end of CONFIG_AP_MODE */
#endif /*__RTW_AP_H_*/

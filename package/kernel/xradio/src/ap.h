/*
 * STA and AP APIs for XRadio drivers
 *
 * Copyright (c) 2013, XRadio
 * Author: XRadio
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef AP_H_INCLUDED
#define AP_H_INCLUDED

#define XRADIO_NOA_NOTIFICATION_DELAY 10

#ifdef AP_HT_CAP_UPDATE
#define HT_INFO_OFFSET 4
#define HT_INFO_MASK 0x0011
#define HT_INFO_IE_LEN 22
#endif

int xradio_set_tim(struct ieee80211_hw *dev, struct ieee80211_sta *sta,
		   bool set);
int xradio_sta_add(struct ieee80211_hw *hw, struct ieee80211_vif *vif,
		   struct ieee80211_sta *sta);
int xradio_sta_remove(struct ieee80211_hw *hw, struct ieee80211_vif *vif,
		      struct ieee80211_sta *sta);
void xradio_sta_notify(struct ieee80211_hw *dev, struct ieee80211_vif *vif,
		       enum sta_notify_cmd notify_cmd,
		       struct ieee80211_sta *sta);
void xradio_bss_info_changed(struct ieee80211_hw *dev,
			     struct ieee80211_vif *vif,
			     struct ieee80211_bss_conf *info,
			     u32 changed);
int xradio_ampdu_action(struct ieee80211_hw *hw,
			struct ieee80211_vif *vif,
			struct ieee80211_ampdu_params *params);
/*			enum ieee80211_ampdu_mlme_action action,
			struct ieee80211_sta *sta, u16 tid, u16 *ssn,
			u8 buf_size);*/

void xradio_suspend_resume(struct xradio_vif *priv,
			  struct wsm_suspend_resume *arg);
void xradio_set_tim_work(struct work_struct *work);
void xradio_set_cts_work(struct work_struct *work);
void xradio_multicast_start_work(struct work_struct *work);
void xradio_multicast_stop_work(struct work_struct *work);
void xradio_mcast_timeout(struct timer_list *t);
int xradio_find_link_id(struct xradio_vif *priv, const u8 *mac);
int xradio_alloc_link_id(struct xradio_vif *priv, const u8 *mac);
void xradio_link_id_work(struct work_struct *work);
void xradio_link_id_gc_work(struct work_struct *work);
#if defined(CONFIG_XRADIO_USE_EXTENSIONS)
/*in linux3.4 mac,it does't have the noa pass*/
//void xradio_notify_noa(struct xradio_vif *priv, int delay);
#endif
int xrwl_unmap_link(struct xradio_vif *priv, int link_id);
#ifdef AP_HT_CAP_UPDATE
void xradio_ht_oper_update_work(struct work_struct *work);
#endif

#endif

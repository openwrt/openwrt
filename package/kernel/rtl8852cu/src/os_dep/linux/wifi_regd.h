/******************************************************************************
 *
 * Copyright(c) 2009-2010 - 2017 Realtek Corporation.
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

#ifndef __WIFI_REGD_H__
#define __WIFI_REGD_H__

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 27)) && (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 31))
#define IEEE80211_CHAN_NO_HT40PLUS IEEE80211_CHAN_NO_FAT_ABOVE
#define IEEE80211_CHAN_NO_HT40MINUS IEEE80211_CHAN_NO_FAT_BELOW
#define IEEE80211_CHAN_NO_HT40 (IEEE80211_CHAN_NO_HT40PLUS | IEEE80211_CHAN_NO_HT40MINUS)
#endif
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 16, 0)) && (LINUX_VERSION_CODE < KERNEL_VERSION(4, 2, 0))
#define IEEE80211_CHAN_IR_CONCURRENT IEEE80211_CHAN_GO_CONCURRENT
#endif

void rtw_chset_hook_os_channels(struct rtw_chset *chset, void *os_ref);
void rtw_regd_change_complete_sync(struct wiphy *wiphy, struct get_chplan_resp *chplan, bool rtnl_lock_needed);
int rtw_regd_change_complete_async(struct wiphy *wiphy, struct get_chplan_resp *chplan);
#ifdef CONFIG_REGD_SRC_FROM_OS
void rtw_chset_apply_from_os(struct rtw_chset *chset, u8 d_flags);
s16 rtw_os_get_total_txpwr_regd_lmt_mbm(_adapter *adapter, enum band_type band, u8 cch, enum channel_width bw);
#endif

#if CONFIG_RTW_CFG80211_CAC_EVENT
void rtw_cfg80211_radar_detected_event(struct rf_ctl_t *rfctl, u8 band_idx
	, u8 cch, enum channel_width bw);
void rtw_cfg80211_cac_started_event(struct rf_ctl_t *rfctl, u8 band_idx
	, u8 ifbmp, u8 cch, enum channel_width bw);
void rtw_cfg80211_cac_finished_event(struct rf_ctl_t *rfctl, u8 band_idx
	, u8 ifbmp, u8 cch, enum channel_width bw);
void rtw_cfg80211_cac_aborted_event(struct rf_ctl_t *rfctl, u8 band_idx
	, u8 ifbmp, u8 cch, enum channel_width bw);
void rtw_cfg80211_cac_force_finished(struct rf_ctl_t *rfctl, u8 band_idx
	, u8 ifbmp, u8 cch, enum channel_width bw);
void rtw_cfg80211_nop_finished_event(struct rf_ctl_t *rfctl, u8 band_idx
	, u8 band, u8 cch, enum channel_width bw);
void rtw_cfg80211_nop_started_event(struct rf_ctl_t *rfctl, u8 band_idx
	, u8 band, u8 cch, enum channel_width bw, bool called_on_cmd_thd);
void rtw_regd_free_du_wdev(struct wiphy *wiphy);
#else
#define rtw_cfg80211_radar_detected_event(rfctl, band_idx, cch, bw) do {} while (0)
#define rtw_cfg80211_cac_started_event(rfctl, band_idx, ifbmp, cch, bw) do {} while (0)
#define rtw_cfg80211_cac_finished_event(rfctl, band_idx, ifbmp, cch, bw) do {} while (0)
#define rtw_cfg80211_cac_aborted_event(rfctl, band_idx, ifbmp, cch, bw) do {} while (0)
#define rtw_cfg80211_cac_force_finished(rfctl, band_idx, ifbmp, cch, bw) do {} while (0)
#define rtw_cfg80211_nop_finished_event(rfctl, band_idx, band, cch, bw) do {} while (0)
#define rtw_cfg80211_nop_started_event(rfctl, band_idx, band, cch, bw, called_on_cmd_thd) do {} while (0)
#define rtw_regd_free_du_wdev(wiphy) do {} while (0)
#endif /* CONFIG_RTW_CFG80211_CAC_EVENT */

int rtw_regd_init(struct wiphy *wiphy);
void rtw_regd_deinit(struct wiphy *wiphy);

#endif /* __WIFI_REGD_H__ */

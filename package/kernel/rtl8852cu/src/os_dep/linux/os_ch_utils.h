/******************************************************************************
 *
 * Copyright(c) 2007 - 2022 Realtek Corporation.
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

#ifndef __OS_CH_UTILS_H__
#define __OS_CH_UTILS_H__

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 26)) && (LINUX_VERSION_CODE < KERNEL_VERSION(4, 7, 0))
#define NL80211_BAND_2GHZ IEEE80211_BAND_2GHZ
#define NL80211_BAND_5GHZ IEEE80211_BAND_5GHZ
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 6, 0))
#define NL80211_BAND_60GHZ IEEE80211_BAND_60GHZ
#endif
#define NUM_NL80211_BANDS IEEE80211_NUM_BANDS
#endif

extern const enum nl80211_band _rtw_band_to_nl80211_band[];
#define rtw_band_to_nl80211_band(band) (((band) < BAND_MAX) ? _rtw_band_to_nl80211_band[(band)] : NUM_NL80211_BANDS)

extern const enum band_type _nl80211_band_to_rtw_band[];
#define nl80211_band_to_rtw_band(band) (((band) < NUM_NL80211_BANDS) ? _nl80211_band_to_rtw_band[(band)] : BAND_MAX)

extern const struct ieee80211_channel rtw_2ghz_channels[MAX_CHANNEL_NUM_2G];
#if CONFIG_IEEE80211_BAND_5GHZ
extern const struct ieee80211_channel rtw_5ghz_a_channels[MAX_CHANNEL_NUM_5G];
#endif
#if CONFIG_IEEE80211_BAND_6GHZ
extern const struct ieee80211_channel rtw_6ghz_channels[MAX_CHANNEL_NUM_6G];
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 8, 0))
const char *nl80211_chan_width_str(enum nl80211_chan_width cwidth);
u8 rtw_bchbw_to_cfg80211_chan_def(struct wiphy *wiphy, struct cfg80211_chan_def *chdef
	, enum band_type band, u8 ch, u8 bw, u8 offset, u8 ht);
void rtw_get_bchbw_from_cfg80211_chan_def(struct cfg80211_chan_def *chdef
	, u8 *ht, enum band_type *band, u8 *ch, u8 *bw, u8 *offset);
RTW_FUNC_2G_5G_ONLY u8 rtw_chbw_to_cfg80211_chan_def(struct wiphy *wiphy, struct cfg80211_chan_def *chdef
	, u8 ch, u8 bw, u8 offset, u8 ht);
RTW_FUNC_2G_5G_ONLY void rtw_get_chbw_from_cfg80211_chan_def(struct cfg80211_chan_def *chdef
	, u8 *ht, u8 *ch, u8 *bw, u8 *offset);
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 29))
const char *nl80211_channel_type_str(enum nl80211_channel_type ctype);
enum nl80211_channel_type rtw_bchbw_to_nl80211_channel_type(enum band_type band, u8 ch, u8 bw, u8 offset, u8 ht);
void rtw_get_bchbw_from_nl80211_channel_type(struct ieee80211_channel *chan, enum nl80211_channel_type ctype
	, enum band_type *band, u8 *ht, u8 *ch, u8 *bw, u8 *offset);
RTW_FUNC_2G_5G_ONLY enum nl80211_channel_type rtw_chbw_to_nl80211_channel_type(u8 ch, u8 bw, u8 offset, u8 ht);
RTW_FUNC_2G_5G_ONLY void rtw_get_chbw_from_nl80211_channel_type(struct ieee80211_channel *chan, enum nl80211_channel_type ctype
	, u8 *ht, u8 *ch, u8 *bw, u8 *offset);
#endif /* (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 29)) */

#endif /* __OS_CH_UTILS_H__ */


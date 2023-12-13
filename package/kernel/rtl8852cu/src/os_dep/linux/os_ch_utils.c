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
#define  _OS_CH_UTILS_C_

#include <drv_types.h>

#ifdef CONFIG_IOCTL_CFG80211

#define CHAN2G(_channel, _freq, _flags) {			\
		.band			= NL80211_BAND_2GHZ,	\
		.center_freq		= (_freq),		\
		.hw_value		= (_channel),		\
		.flags			= (_flags),		\
		.max_antenna_gain	= 0,			\
		.max_power		= 0,			\
	}

#define CHAN5G(_channel, _flags) {					\
		.band			= NL80211_BAND_5GHZ,		\
		.center_freq		= 5000 + (5 * (_channel)),	\
		.hw_value		= (_channel),			\
		.flags			= (_flags),			\
		.max_antenna_gain	= 0,				\
		.max_power		= 0,				\
	}

#define CHAN6G(_channel, _flags) {					\
		.band			= NL80211_BAND_6GHZ,		\
		.center_freq		= 5950 + (5 * (_channel)),	\
		.hw_value		= (_channel),			\
		.flags			= (_flags),			\
		.max_antenna_gain	= 0,				\
		.max_power		= 0,				\
	}

/* from center_ch_2g */
const struct ieee80211_channel rtw_2ghz_channels[MAX_CHANNEL_NUM_2G] = {
	CHAN2G(1, 2412, 0),
	CHAN2G(2, 2417, 0),
	CHAN2G(3, 2422, 0),
	CHAN2G(4, 2427, 0),
	CHAN2G(5, 2432, 0),
	CHAN2G(6, 2437, 0),
	CHAN2G(7, 2442, 0),
	CHAN2G(8, 2447, 0),
	CHAN2G(9, 2452, 0),
	CHAN2G(10, 2457, 0),
	CHAN2G(11, 2462, 0),
	CHAN2G(12, 2467, 0),
	CHAN2G(13, 2472, 0),
	CHAN2G(14, 2484, 0),
};

#if CONFIG_IEEE80211_BAND_5GHZ
/* from center_ch_5g_20m */
const struct ieee80211_channel rtw_5ghz_a_channels[MAX_CHANNEL_NUM_5G] = {
	CHAN5G(36, 0),	CHAN5G(40, 0),	CHAN5G(44, 0),	CHAN5G(48, 0),

	CHAN5G(52, 0),	CHAN5G(56, 0),	CHAN5G(60, 0),	CHAN5G(64, 0),

	CHAN5G(100, 0),	CHAN5G(104, 0),	CHAN5G(108, 0),	CHAN5G(112, 0),
	CHAN5G(116, 0),	CHAN5G(120, 0),	CHAN5G(124, 0),	CHAN5G(128, 0),
	CHAN5G(132, 0),	CHAN5G(136, 0),	CHAN5G(140, 0),	CHAN5G(144, 0),

	CHAN5G(149, 0),	CHAN5G(153, 0),	CHAN5G(157, 0),	CHAN5G(161, 0),
	CHAN5G(165, 0),	CHAN5G(169, 0),	CHAN5G(173, 0),	CHAN5G(177, 0),
};
#endif

#if CONFIG_IEEE80211_BAND_6GHZ
/* from center_ch_6g_20m */
const struct ieee80211_channel rtw_6ghz_channels[MAX_CHANNEL_NUM_6G] = {
	CHAN6G(1, 0),	CHAN6G(5, 0),	CHAN6G(9, 0),	CHAN6G(13, 0),
	CHAN6G(17, 0),	CHAN6G(21, 0),	CHAN6G(25, 0),	CHAN6G(29, 0),
	CHAN6G(33, 0),	CHAN6G(37, 0),	CHAN6G(41, 0),	CHAN6G(45, 0),
	CHAN6G(49, 0),	CHAN6G(53, 0),	CHAN6G(57, 0),	CHAN6G(61, 0),
	CHAN6G(65, 0),	CHAN6G(69, 0),	CHAN6G(73, 0),	CHAN6G(77, 0),
	CHAN6G(81, 0),	CHAN6G(85, 0),	CHAN6G(89, 0),	CHAN6G(93, 0),

	CHAN6G(97, 0),	CHAN6G(101, 0),	CHAN6G(105, 0),	CHAN6G(109, 0),
	CHAN6G(113, 0),	CHAN6G(117, 0),

					CHAN6G(121, 0),	CHAN6G(125, 0),
	CHAN6G(129, 0),	CHAN6G(133, 0),	CHAN6G(137, 0),	CHAN6G(141, 0),
	CHAN6G(145, 0),	CHAN6G(149, 0),	CHAN6G(153, 0),	CHAN6G(157, 0),
	CHAN6G(161, 0),	CHAN6G(165, 0),	CHAN6G(169, 0),	CHAN6G(173, 0),
	CHAN6G(177, 0),	CHAN6G(181, 0),	CHAN6G(185, 0),	CHAN6G(189, 0),

	CHAN6G(193, 0),	CHAN6G(197, 0),	CHAN6G(201, 0),	CHAN6G(205, 0),
	CHAN6G(209, 0),	CHAN6G(213, 0),	CHAN6G(217, 0),	CHAN6G(221, 0),
	CHAN6G(225, 0),	CHAN6G(229, 0),	CHAN6G(233, 0),	CHAN6G(237, 0),

	CHAN6G(241, 0),	CHAN6G(245, 0),	CHAN6G(249, 0),	CHAN6G(253, 0),
};
#endif /* CONFIG_IEEE80211_BAND_6GHZ */

const enum nl80211_band _rtw_band_to_nl80211_band[] = {
	[BAND_ON_24G]	= NL80211_BAND_2GHZ,
	[BAND_ON_5G]	= NL80211_BAND_5GHZ,
#if CONFIG_IEEE80211_BAND_6GHZ
	#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 4, 0))
	[BAND_ON_6G]	= NL80211_BAND_6GHZ,
	#else
	[BAND_ON_6G]	= NUM_NL80211_BANDS,
	#endif
#endif
};

const enum band_type _nl80211_band_to_rtw_band[] = {
	[NL80211_BAND_2GHZ]	= BAND_ON_24G,
	[NL80211_BAND_5GHZ]	= BAND_ON_5G,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 6, 0))
	[NL80211_BAND_60GHZ]	= BAND_MAX,
#endif
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 4, 0))
	#if CONFIG_IEEE80211_BAND_6GHZ
	[NL80211_BAND_6GHZ]	= BAND_ON_6G,
	#else
	[NL80211_BAND_6GHZ]	= BAND_MAX,
	#endif
#endif
};

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 8, 0))
const char *nl80211_chan_width_str(enum nl80211_chan_width cwidth)
{
	switch (cwidth) {
	case NL80211_CHAN_WIDTH_20_NOHT:
		return "20_NOHT";
	case NL80211_CHAN_WIDTH_20:
		return "20";
	case NL80211_CHAN_WIDTH_40:
		return "40";
	case NL80211_CHAN_WIDTH_80:
		return "80";
	case NL80211_CHAN_WIDTH_80P80:
		return "80+80";
	case NL80211_CHAN_WIDTH_160:
		return "160";
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 11, 0))
	case NL80211_CHAN_WIDTH_5:
		return "5";
	case NL80211_CHAN_WIDTH_10:
		return "10";
#endif
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 9, 0))
	case NL80211_CHAN_WIDTH_1:
		return "1";
	case NL80211_CHAN_WIDTH_2:
		return "2";
	case NL80211_CHAN_WIDTH_4:
		return "4";
	case NL80211_CHAN_WIDTH_8:
		return "8";
	case NL80211_CHAN_WIDTH_16:
		return "16";
#endif
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 18, 0))
	case NL80211_CHAN_WIDTH_320:
		return "320";
#endif
	default:
		return "INVALID";
	};
}

u8 rtw_bchbw_to_cfg80211_chan_def(struct wiphy *wiphy, struct cfg80211_chan_def *chdef
	, enum band_type band, u8 ch, u8 bw, u8 offset, u8 ht)
{
	int freq, cfreq;
	struct ieee80211_channel *chan;
	u8 ret = _FAIL;

	_rtw_memset(chdef, 0, sizeof(*chdef));

	freq = rtw_bch2freq(band, ch);
	if (!freq)
		goto exit;

	cfreq = rtw_get_center_ch_by_band(band, ch, bw, offset);
	if (!cfreq)
		goto exit;
	cfreq = rtw_bch2freq(band, cfreq);
	if (!cfreq)
		goto exit;

	chan = ieee80211_get_channel(wiphy, freq);
	if (!chan)
		goto exit;

	if (bw == CHANNEL_WIDTH_20)
		chdef->width = ht ? NL80211_CHAN_WIDTH_20 : NL80211_CHAN_WIDTH_20_NOHT;
	else if (bw == CHANNEL_WIDTH_40)
		chdef->width = NL80211_CHAN_WIDTH_40;
	else if (bw == CHANNEL_WIDTH_80)
		chdef->width = NL80211_CHAN_WIDTH_80;
	else if (bw == CHANNEL_WIDTH_160)
		chdef->width = NL80211_CHAN_WIDTH_160;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 11, 0))
	else if (bw == CHANNEL_WIDTH_5)
		chdef->width = NL80211_CHAN_WIDTH_5;
	else if (bw == CHANNEL_WIDTH_10)
		chdef->width = NL80211_CHAN_WIDTH_10;
#endif /* (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 11, 0)) */
	else {
		rtw_warn_on(1);
		goto exit;
	}

	chdef->chan = chan;
	chdef->center_freq1 = cfreq;

	ret = _SUCCESS;

exit:
	return ret;
}

void rtw_get_bchbw_from_cfg80211_chan_def(struct cfg80211_chan_def *chdef
	, u8 *ht, enum band_type *band, u8 *ch, u8 *bw, u8 *offset)
{
	int pri_freq;
	struct ieee80211_channel *chan = chdef->chan;

	*ch = 0;
	*band = nl80211_band_to_rtw_band(chan->band);
	if (*band == BAND_MAX) {
		RTW_WARN("invalid band:%d\n", chan->band);
		rtw_warn_on(1);
		return;
	}

	pri_freq = rtw_bch2freq(*band, chan->hw_value);
	if (!pri_freq) {
		RTW_WARN("invalid channel:%d on %s\n", chan->hw_value, band_str(*band));
		rtw_warn_on(1);
		return;
	}

	switch (chdef->width) {
	case NL80211_CHAN_WIDTH_20_NOHT:
		*ht = 0;
		*bw = CHANNEL_WIDTH_20;
		*offset = CHAN_OFFSET_NO_EXT;
		*ch = chan->hw_value;
		break;
	case NL80211_CHAN_WIDTH_20:
		*ht = 1;
		*bw = CHANNEL_WIDTH_20;
		*offset = CHAN_OFFSET_NO_EXT;
		*ch = chan->hw_value;
		break;
	case NL80211_CHAN_WIDTH_40:
		*ht = 1;
		*bw = CHANNEL_WIDTH_40;
		*offset = pri_freq > chdef->center_freq1 ? CHAN_OFFSET_LOWER : CHAN_OFFSET_UPPER;
		if (!rtw_get_offset_by_bchbw(*band, chan->hw_value, *bw, offset)) {
			RTW_WARN("invalid channel:%d bw:%s on %s\n", chan->hw_value, ch_width_str(*bw), band_str(*band));
			rtw_warn_on(1);
			return;
		}
		*ch = chan->hw_value;
		break;
	case NL80211_CHAN_WIDTH_80:
		*ht = 1;
		*bw = CHANNEL_WIDTH_80;
		if (!rtw_get_offset_by_bchbw(*band, chan->hw_value, *bw, offset)) {
			RTW_WARN("invalid channel:%d bw:%s on %s\n", chan->hw_value, ch_width_str(*bw), band_str(*band));
			rtw_warn_on(1);
			return;
		}
		*ch = chan->hw_value;
		break;
	case NL80211_CHAN_WIDTH_160:
		*ht = 1;
		*bw = CHANNEL_WIDTH_160;
		if (!rtw_get_offset_by_bchbw(*band, chan->hw_value, *bw, offset)) {
			RTW_WARN("invalid channel:%d bw:%s on %s\n", chan->hw_value, ch_width_str(*bw), band_str(*band));
			rtw_warn_on(1);
			return;
		}
		*ch = chan->hw_value;
		break;
	case NL80211_CHAN_WIDTH_80P80:
	#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 11, 0))
	case NL80211_CHAN_WIDTH_5:
	case NL80211_CHAN_WIDTH_10:
	#endif
	#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 9, 0))
	case NL80211_CHAN_WIDTH_1:
	case NL80211_CHAN_WIDTH_2:
	case NL80211_CHAN_WIDTH_4:
	case NL80211_CHAN_WIDTH_8:
	case NL80211_CHAN_WIDTH_16:
	#endif
	#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 18, 0))
	case NL80211_CHAN_WIDTH_320:
	#endif
	default:
		*ht = 0;
		*bw = CHANNEL_WIDTH_20;
		*offset = CHAN_OFFSET_NO_EXT;
		RTW_INFO("unsupported cwidth:%s\n", nl80211_chan_width_str(chdef->width));
		rtw_warn_on(1);
	};
}

#if CONFIG_ALLOW_FUNC_2G_5G_ONLY
RTW_FUNC_2G_5G_ONLY u8 rtw_chbw_to_cfg80211_chan_def(struct wiphy *wiphy, struct cfg80211_chan_def *chdef
	, u8 ch, u8 bw, u8 offset, u8 ht)
{
	return rtw_bchbw_to_cfg80211_chan_def(wiphy, chdef, rtw_is_2g_ch(ch) ? BAND_ON_24G : BAND_ON_5G, ch, bw, offset, ht);
}

RTW_FUNC_2G_5G_ONLY void rtw_get_chbw_from_cfg80211_chan_def(struct cfg80211_chan_def *chdef
	, u8 *ht, u8 *ch, u8 *bw, u8 *offset)
{
	enum band_type band;

	rtw_get_bchbw_from_cfg80211_chan_def(chdef, ht, &band, ch, bw, offset);
}
#endif

#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 29))
const char *nl80211_channel_type_str(enum nl80211_channel_type ctype)
{
	switch (ctype) {
	case NL80211_CHAN_NO_HT:
		return "NO_HT";
	case NL80211_CHAN_HT20:
		return "HT20";
	case NL80211_CHAN_HT40MINUS:
		return "HT40-";
	case NL80211_CHAN_HT40PLUS:
		return "HT40+";
	default:
		return "INVALID";
	};
}

enum nl80211_channel_type rtw_bchbw_to_nl80211_channel_type(enum band_type band, u8 ch, u8 bw, u8 offset, u8 ht)
{
	rtw_warn_on(!ht && (bw >= CHANNEL_WIDTH_40 || offset != CHAN_OFFSET_NO_EXT));

	if (!ht)
		return NL80211_CHAN_NO_HT;
	if (bw >= CHANNEL_WIDTH_40) {
		if (offset == CHAN_OFFSET_LOWER)
			return NL80211_CHAN_HT40MINUS;
		else if (offset == CHAN_OFFSET_UPPER)
			return NL80211_CHAN_HT40PLUS;
		else
			rtw_warn_on(1);
	}
	return NL80211_CHAN_HT20;
}

void rtw_get_bchbw_from_nl80211_channel_type(struct ieee80211_channel *chan, enum nl80211_channel_type ctype
	, enum band_type *band, u8 *ht, u8 *ch, u8 *bw, u8 *offset)
{
	int pri_freq;

	*ch = 0;
	*band = nl80211_band_to_rtw_band(chan->band);
	if (*band == BAND_MAX) {
		RTW_WARN("invalid band:%d\n", chan->band);
		rtw_warn_on(1);
		return;
	}

	pri_freq = rtw_bch2freq(*band, chan->hw_value);
	if (!pri_freq) {
		RTW_WARN("invalid channel:%d on %s\n", chan->hw_value, band_str(*band));
		rtw_warn_on(1);
		return;
	}
	*ch = chan->hw_value;

	switch (ctype) {
	case NL80211_CHAN_NO_HT:
		*ht = 0;
		*bw = CHANNEL_WIDTH_20;
		*offset = CHAN_OFFSET_NO_EXT;
		break;
	case NL80211_CHAN_HT20:
		*ht = 1;
		*bw = CHANNEL_WIDTH_20;
		*offset = CHAN_OFFSET_NO_EXT;
		break;
	case NL80211_CHAN_HT40MINUS:
		*ht = 1;
		*bw = CHANNEL_WIDTH_40;
		*offset = CHAN_OFFSET_LOWER;
		break;
	case NL80211_CHAN_HT40PLUS:
		*ht = 1;
		*bw = CHANNEL_WIDTH_40;
		*offset = CHAN_OFFSET_UPPER;
		break;
	default:
		*ht = 0;
		*bw = CHANNEL_WIDTH_20;
		*offset = CHAN_OFFSET_NO_EXT;
		RTW_INFO("unsupported ctype:%s\n", nl80211_channel_type_str(ctype));
		rtw_warn_on(1);
	};
}

#if CONFIG_ALLOW_FUNC_2G_5G_ONLY
RTW_FUNC_2G_5G_ONLY enum nl80211_channel_type rtw_chbw_to_nl80211_channel_type(u8 ch, u8 bw, u8 offset, u8 ht)
{
	return rtw_bchbw_to_nl80211_channel_type(rtw_is_2g_ch(ch) ? BAND_ON_24G : BAND_ON_5G, ch, bw, offset, ht);
}

RTW_FUNC_2G_5G_ONLY void rtw_get_chbw_from_nl80211_channel_type(struct ieee80211_channel *chan, enum nl80211_channel_type ctype
	, u8 *ht, u8 *ch, u8 *bw, u8 *offset)
{
	enum band_type band;

	rtw_get_bchbw_from_nl80211_channel_type(chan, ctype, &band, ht, ch, bw, offset);
}
#endif
#endif /* (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 29)) */

#endif /* CONFIG_IOCTL_CFG80211 */


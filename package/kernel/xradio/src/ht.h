/*
 * HT-related code for XRadio drivers
 *
 * Copyright (c) 2013, XRadio
 * Author: XRadio
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef XRADIO_HT_H_INCLUDED
#define XRADIO_HT_H_INCLUDED

#include <net/mac80211.h>

struct xradio_ht_oper {
	struct ieee80211_sta_ht_cap  ht_cap;
	enum nl80211_channel_type    channel_type;
	u16                          operation_mode;
};

static inline int xradio_is_ht(const struct xradio_ht_oper *ht_oper)
{
	return ht_oper->channel_type != NL80211_CHAN_NO_HT;
}

static inline int xradio_ht_greenfield(const struct xradio_ht_oper *ht_oper)
{
	return (xradio_is_ht(ht_oper) &&
	       (ht_oper->ht_cap.cap      & IEEE80211_HT_CAP_GRN_FLD) &&
	       !(ht_oper->operation_mode & IEEE80211_HT_OP_MODE_NON_GF_STA_PRSNT));
}

static inline int xradio_ht_ampdu_density(const struct xradio_ht_oper *ht_oper)
{
	if (!xradio_is_ht(ht_oper))
		return 0;
	return ht_oper->ht_cap.ampdu_density;
}

int xradio_apcompat_detect(struct xradio_vif *priv, u8 rx_rate);
void xradio_remove_ht_ie(struct xradio_vif *priv, struct sk_buff *skb);

#endif /* XRADIO_HT_H_INCLUDED */

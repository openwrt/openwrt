/*
 * Copyright 2002-2004, Instant802 Networks, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef WPA_H
#define WPA_H

ieee80211_txrx_result
ieee80211_tx_h_michael_mic_add(struct ieee80211_txrx_data *tx);
ieee80211_txrx_result
ieee80211_rx_h_michael_mic_verify(struct ieee80211_txrx_data *rx);

ieee80211_txrx_result
ieee80211_tx_h_tkip_encrypt(struct ieee80211_txrx_data *tx);
ieee80211_txrx_result
ieee80211_rx_h_tkip_decrypt(struct ieee80211_txrx_data *rx);

ieee80211_txrx_result
ieee80211_tx_h_ccmp_encrypt(struct ieee80211_txrx_data *tx);
ieee80211_txrx_result
ieee80211_rx_h_ccmp_decrypt(struct ieee80211_txrx_data *rx);

int ieee80211_get_hdr_info(const struct sk_buff *skb, u8 **sa, u8 **da,
			   u8 *qos_tid, u8 **data, size_t *data_len);

#endif /* WPA_H */

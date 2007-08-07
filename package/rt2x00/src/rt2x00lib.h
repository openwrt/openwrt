/*
	Copyright (C) 2004 - 2007 rt2x00 SourceForge Project
	<http://rt2x00.serialmonkey.com>

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the
	Free Software Foundation, Inc.,
	59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/*
	Module: rt2x00lib
	Abstract: Data structures for the rt2x00lib module.
	Supported chipsets: RT2460, RT2560, RT2570,
	rt2561, rt2561s, rt2661, rt2571W & rt2671.
 */

#ifndef RT2X00LIB_H
#define RT2X00LIB_H

/*
 * Driver allocation handlers.
 */
int rt2x00lib_probe_dev(struct rt2x00_dev *rt2x00dev);
void rt2x00lib_remove_dev(struct rt2x00_dev *rt2x00dev);

/*
 * Driver status handlers.
 */
int rt2x00lib_suspend(struct rt2x00_dev *rt2x00dev, pm_message_t state);
int rt2x00lib_resume(struct rt2x00_dev *rt2x00dev);

/*
 * Interrupt context handlers.
 */
void rt2x00lib_txdone(struct data_entry *entry,
	const int status, const int retry);
void rt2x00lib_rxdone(struct data_entry *entry, char *data,
	const int size, const int signal, const int rssi, const int ofdm);

/*
 * TX descriptor initializer
 */
void rt2x00lib_write_tx_desc(struct rt2x00_dev *rt2x00dev,
	struct data_entry *entry, struct data_desc *txd,
	struct ieee80211_hdr *ieee80211hdr, unsigned int length,
	struct ieee80211_tx_control *control);

/*
 * mac80211 handlers.
 */
int rt2x00lib_tx(struct ieee80211_hw *hw, struct sk_buff *skb,
	struct ieee80211_tx_control *control);
int rt2x00lib_reset(struct ieee80211_hw *hw);
int rt2x00lib_add_interface(struct ieee80211_hw *hw,
	struct ieee80211_if_init_conf *conf);
void rt2x00lib_remove_interface(struct ieee80211_hw *hw,
	struct ieee80211_if_init_conf *conf);
int rt2x00lib_config(struct ieee80211_hw *hw, struct ieee80211_conf *conf);
int rt2x00lib_config_interface(struct ieee80211_hw *hw, int if_id,
	struct ieee80211_if_conf *conf);
void rt2x00lib_set_multicast_list(struct ieee80211_hw *hw,
	unsigned short flags, int mc_count);
int rt2x00lib_get_tx_stats(struct ieee80211_hw *hw,
	struct ieee80211_tx_queue_stats *stats);
int rt2x00lib_conf_tx(struct ieee80211_hw *hw, int queue,
	const struct ieee80211_tx_queue_params *params);

#include "rt2x00debug.h"

#endif /* RT2X00LIB_H */

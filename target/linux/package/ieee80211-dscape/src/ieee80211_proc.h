/*
 * Copyright 2003-2004, Instant802 Networks, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef IEEE80211_PROC_H
#define IEEE80211_PROC_H

#ifdef CONFIG_PROC_FS

void ieee80211_proc_init_sta(struct ieee80211_local *local,
			     struct sta_info *sta);
void ieee80211_proc_deinit_sta(struct ieee80211_local *local,
			       struct sta_info *sta);
void ieee80211_proc_init_virtual(struct net_device *dev);
void ieee80211_proc_deinit_virtual(struct net_device *dev);
void ieee80211_proc_init_interface(struct ieee80211_local *local);
void ieee80211_proc_deinit_interface(struct ieee80211_local *local);
void ieee80211_proc_init(void);
void ieee80211_proc_deinit(void);

#else /* CONFIG_PROC_FS */

static inline void ieee80211_proc_init_sta(struct ieee80211_local *local,
					   struct sta_info *sta) {}
static inline void ieee80211_proc_deinit_sta(struct ieee80211_local *local,
					     struct sta_info *sta) {}
static inline void ieee80211_proc_init_virtual(struct net_device *dev) {}
static inline void ieee80211_proc_deinit_virtual(struct net_device *dev) {}
static inline void
ieee80211_proc_init_interface(struct ieee80211_local *local) {}
static inline void
ieee80211_proc_deinit_interface(struct ieee80211_local *local) {}
static inline void ieee80211_proc_init(void) {}
static inline void ieee80211_proc_deinit(void) {}
#endif /* CONFIG_PROC_FS */

#endif /* IEEE80211_PROC_H */

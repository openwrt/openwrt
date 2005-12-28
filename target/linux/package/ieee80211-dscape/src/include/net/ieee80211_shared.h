/*
 * IEEE 802.11 -- shared defines for low-level drivers, 80211.o, and hostapd
 * Copyright 2002-2004, Instant802 Networks, Inc.
 * Copyright 2005, Devicescape Software, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef IEEE80211_SHARED_H
#define IEEE80211_SHARED_H

/* 802.11g is backwards-compatible with 802.11b, so a wlan card can
 * actually be both in 11b and 11g modes at the same time. */
enum {
	MODE_IEEE80211A = 0 /* IEEE 802.11a */,
	MODE_IEEE80211B = 1 /* IEEE 802.11b only */,
	MODE_ATHEROS_TURBO = 2 /* Atheros Turbo mode (2x.11a at 5 GHz) */,
	MODE_IEEE80211G = 3 /* IEEE 802.11g (and 802.11b compatibility) */,
	MODE_ATHEROS_TURBOG = 4 /* Atheros Turbo mode (2x.11g at 2.4 GHz) */,
	MODE_ATHEROS_PRIME = 5 /* Atheros Dynamic Turbo mode */,
	MODE_ATHEROS_PRIMEG = 6 /* Atheros Dynamic Turbo mode G */,
	MODE_ATHEROS_XR = 7 /* Atheros XR mode  */,
	NUM_IEEE80211_MODES = 8
};

#define IEEE80211_CHAN_W_SCAN 0x00000001
#define IEEE80211_CHAN_W_ACTIVE_SCAN 0x00000002
#define IEEE80211_CHAN_W_IBSS 0x00000004

/* Low-level driver should set PREAMBLE2, OFDM, CCK, and TURBO flags.
 * BASIC, SUPPORTED, ERP, and MANDATORY flags are set in 80211.o based on the
 * configuration. */
#define IEEE80211_RATE_ERP 0x00000001
#define IEEE80211_RATE_BASIC 0x00000002
#define IEEE80211_RATE_PREAMBLE2 0x00000004
#define IEEE80211_RATE_SUPPORTED 0x00000010
#define IEEE80211_RATE_OFDM 0x00000020
#define IEEE80211_RATE_CCK 0x00000040
#define IEEE80211_RATE_TURBO 0x00000080
#define IEEE80211_RATE_MANDATORY 0x00000100
#define IEEE80211_RATE_XR 0x00000200

#define IEEE80211_RATE_CCK_2 (IEEE80211_RATE_CCK | IEEE80211_RATE_PREAMBLE2)
#define IEEE80211_RATE_MODULATION(f) \
(f & (IEEE80211_RATE_CCK | IEEE80211_RATE_OFDM))


#endif /* IEEE80211_SHARED_H */

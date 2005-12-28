/*
 * Copyright 2002-2004, Instant802 Networks, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/config.h>
#include <linux/netdevice.h>
#include <linux/types.h>

#ifdef CONFIG_OAP_LEDS_WLAN
extern void leds_wlan_set(int unit, int tx, int state);
#endif

void ieee80211_rx_led(int state, struct net_device *dev) {
#ifdef CONFIG_OAP_LEDS_WLAN
	static unsigned int count = 0;

	if (state == 2) {
		leds_wlan_set(0, 0, (++count) & 1);
	}
#endif
}

void ieee80211_tx_led(int state, struct net_device *dev) {
#ifdef CONFIG_OAP_LEDS_WLAN
        leds_wlan_set(0, 1, state);
#endif
}


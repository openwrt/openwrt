/*
 * power management interfaces for XRadio drivers
 *
 * Copyright (c) 2013, XRadio
 * Author: XRadio
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */


#ifndef PM_H_INCLUDED
#define PM_H_INCLUDED

#ifdef CONFIG_WAKELOCK
#include <linux/wakelock.h>
#endif

/* ******************************************************************** */
/* mac80211 API */

#ifdef CONFIG_PM

#define XRADIO_PM_DEVICE   "xradio_pm"
#define XRADIO_WAKE_LOCK   "xradio_wlan"

/* extern */   struct xradio_common; 
 /* private */ struct xradio_suspend_state;

struct xradio_pm_state {
#ifdef CONFIG_WAKELOCK
	struct wake_lock wakelock;
#else
	struct timer_list stay_awake;
#endif
	struct platform_device *pm_dev;
	spinlock_t lock;
	unsigned long expires_save;
};

struct xradio_pm_state_vif {
	struct xradio_suspend_state *suspend_state;
};

#ifdef CONFIG_XRADIO_SUSPEND_POWER_OFF
enum suspend_state {
	XRADIO_RESUME = 0,
	XRADIO_CONNECT_SUSP,
	XRADIO_DISCONNECT_SUSP,
	XRADIO_POWEROFF_SUSP
};
#endif
int xradio_pm_init(struct xradio_pm_state *pm, struct xradio_common *priv);
void xradio_pm_deinit(struct xradio_pm_state *pm);
void xradio_pm_stay_awake(struct xradio_pm_state *pm, unsigned long tmo);
void xradio_pm_lock_awake(struct xradio_pm_state *pm);
void xradio_pm_unlock_awake(struct xradio_pm_state *pm);
int xradio_wow_suspend(struct ieee80211_hw *hw, struct cfg80211_wowlan *wowlan);
int xradio_wow_resume(struct ieee80211_hw *hw);

#endif /* CONFIG_PM */

#endif

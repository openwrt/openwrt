/*
 * PM implementation for XRadio drivers
 *
 * Copyright (c) 2013, XRadio
 * Author: XRadio
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
 
#include <linux/platform_device.h>
#include <linux/if_ether.h>
#include "xradio.h"
#include "pm.h"
#include "sta.h"
#include "bh.h"
#include "sdio.h"

#define XRADIO_BEACON_SKIPPING_MULTIPLIER 3

struct xradio_udp_port_filter {
	struct wsm_udp_port_filter_hdr hdr;
	struct wsm_udp_port_filter dhcp;
	struct wsm_udp_port_filter upnp;
} __packed;

struct xradio_ether_type_filter {
	struct wsm_ether_type_filter_hdr hdr;
	struct wsm_ether_type_filter ip;
	struct wsm_ether_type_filter pae;
	struct wsm_ether_type_filter wapi;
} __packed;

static struct xradio_udp_port_filter xradio_udp_port_filter_on = {
	.hdr.nrFilters = 2,
	.dhcp = {
		.filterAction = WSM_FILTER_ACTION_FILTER_OUT,
		.portType = WSM_FILTER_PORT_TYPE_DST,
		.udpPort = __cpu_to_le16(67),
	},
	.upnp = {
		.filterAction = WSM_FILTER_ACTION_FILTER_OUT,
		.portType = WSM_FILTER_PORT_TYPE_DST,
		.udpPort = __cpu_to_le16(1900),
	},
	/* Please add other known ports to be filtered out here and
	 * update nrFilters field in the header.
	 * Up to 4 filters are allowed. */
};

static struct wsm_udp_port_filter_hdr xradio_udp_port_filter_off = {
	.nrFilters = 0,
};

#ifndef ETH_P_WAPI
#define ETH_P_WAPI     0x88B4
#endif

#ifdef TES_P2P_000B_DISABLE_EAPOL_FILTER
/* TES_P2P_000B WorkAround:
 * when the link keep 10min more or less(i am not sure),
 * wpa_s session maybe expired, and want to update group key.
 * it will use eapol frame(802.1x,0x888E).
 * if driver suspend, and discard eapol frame, then session end.
 * i don't know why original code discards eapol frame in suspend.
 * but now make this filter disable as WorkAround. wzw */
static struct xradio_ether_type_filter xradio_ether_type_filter_on = {
	.hdr.nrFilters = 1,
/*	.ip = {
		.filterAction = WSM_FILTER_ACTION_FILTER_IN,
		.etherType = __cpu_to_le16(ETH_P_IP),
	},*/
/*	.pae = {
		.filterAction = WSM_FILTER_ACTION_FILTER_IN,
		.etherType = __cpu_to_le16(ETH_P_PAE),
	},*/
	.wapi = {
		.filterAction = WSM_FILTER_ACTION_FILTER_IN,
		.etherType = __cpu_to_le16(ETH_P_WAPI),
	},
	/* Please add other known ether types to be filtered out here and
	 * update nrFilters field in the header.
	 * Up to 4 filters are allowed. */
};
#else
static struct xradio_ether_type_filter xradio_ether_type_filter_on = {
	.hdr.nrFilters = 2,
/*	.ip = {
		.filterAction = WSM_FILTER_ACTION_FILTER_IN,
		.etherType = __cpu_to_le16(ETH_P_IP),
	},*/
	.pae = {
		.filterAction = WSM_FILTER_ACTION_FILTER_IN,
		.etherType = __cpu_to_le16(ETH_P_PAE),
	},
	.wapi = {
		.filterAction = WSM_FILTER_ACTION_FILTER_IN,
		.etherType = __cpu_to_le16(ETH_P_WAPI),
	},
	/* Please add other known ether types to be filtered out here and
	 * update nrFilters field in the header.
	 * Up to 4 filters are allowed. */
};
#endif

static struct wsm_ether_type_filter_hdr xradio_ether_type_filter_off = {
	.nrFilters = 0,
};

static int xradio_suspend_late(struct device *dev);
static void xradio_pm_release(struct device *dev);
static int xradio_pm_probe(struct platform_device *pdev);
static int __xradio_wow_suspend(struct xradio_vif *priv,
                                struct cfg80211_wowlan *wowlan);
static int __xradio_wow_resume(struct xradio_vif *priv);
#ifdef CONFIG_XRADIO_SUSPEND_POWER_OFF
static int xradio_poweroff_suspend(struct xradio_common *hw_priv);
static int xradio_poweroff_resume(struct xradio_common *hw_priv);
#endif


/* private */
struct xradio_suspend_state {
	unsigned long bss_loss_tmo;
	unsigned long connection_loss_tmo;
	unsigned long join_tmo;
	unsigned long direct_probe;
	unsigned long link_id_gc;
	bool beacon_skipping;
};

static const struct dev_pm_ops xradio_pm_ops = {
	.suspend_noirq = xradio_suspend_late,
};

static struct platform_driver xradio_power_driver = {
	.probe = xradio_pm_probe,
	.driver = {
		.name = XRADIO_PM_DEVICE,
		.pm = &xradio_pm_ops,
	},
};

static int xradio_pm_init_common(struct xradio_pm_state *pm,
				  struct xradio_common *hw_priv)
{
	int ret;
	pm_printk(XRADIO_DBG_TRC,"%s\n", __FUNCTION__);

	spin_lock_init(&pm->lock);
	/* Register pm driver. */
	ret = platform_driver_register(&xradio_power_driver);
	if (ret) {
		pm_printk(XRADIO_DBG_ERROR, "%s:platform_driver_register failed(%d)!\n",
		           __FUNCTION__, ret);
		return ret;
	}

	/* Add pm device. */
	pm->pm_dev = platform_device_alloc(XRADIO_PM_DEVICE, 0);
	if (!pm->pm_dev) {
		pm_printk(XRADIO_DBG_ERROR, "%s:platform_device_alloc failed!\n",
		           __FUNCTION__);
		platform_driver_unregister(&xradio_power_driver);
		return -ENOMEM;
	}
	pm->pm_dev->dev.platform_data = hw_priv;
	ret = platform_device_add(pm->pm_dev);
	if (ret) {
		pm_printk(XRADIO_DBG_ERROR, "%s:platform_device_add failed(%d)!\n",
		           __FUNCTION__, ret);
		platform_driver_unregister(&xradio_power_driver);
		kfree(pm->pm_dev);
		pm->pm_dev = NULL;
	}

	return ret;
}

static void xradio_pm_deinit_common(struct xradio_pm_state *pm)
{
	pm_printk(XRADIO_DBG_TRC,"%s\n", __FUNCTION__);
	platform_driver_unregister(&xradio_power_driver);
	if (pm->pm_dev) {
		pm->pm_dev->dev.platform_data = NULL;
		platform_device_unregister(pm->pm_dev); /* kfree is already do */
		pm->pm_dev = NULL;
	}
}

#ifdef CONFIG_WAKELOCK

int xradio_pm_init(struct xradio_pm_state *pm,
		   struct xradio_common *hw_priv)
{
	int ret = 0;
	pm_printk(XRADIO_DBG_TRC,"%s\n", __FUNCTION__);

	ret = xradio_pm_init_common(pm, hw_priv);
	if (!ret)
		wake_lock_init(&pm->wakelock, WAKE_LOCK_SUSPEND, XRADIO_WAKE_LOCK);
	else
		pm_printk(XRADIO_DBG_ERROR,"xradio_pm_init_common failed!\n");
	return ret;
}

void xradio_pm_deinit(struct xradio_pm_state *pm)
{
	pm_printk(XRADIO_DBG_TRC,"%s\n", __FUNCTION__);
	if (wake_lock_active(&pm->wakelock))
		wake_unlock(&pm->wakelock);
	wake_lock_destroy(&pm->wakelock);
	xradio_pm_deinit_common(pm);
}

void xradio_pm_stay_awake(struct xradio_pm_state *pm,
			  unsigned long tmo)
{
	long cur_tmo;
	pm_printk(XRADIO_DBG_MSG,"%s\n", __FUNCTION__);

	spin_lock_bh(&pm->lock);
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 10, 0))
	cur_tmo = pm->wakelock.ws.timer.expires - jiffies;
#else
	cur_tmo = pm->wakelock.expires - jiffies;
#endif
	if (!wake_lock_active(&pm->wakelock) || cur_tmo < (long)tmo)
		wake_lock_timeout(&pm->wakelock, tmo);
	spin_unlock_bh(&pm->lock);
}
void xradio_pm_lock_awake(struct xradio_pm_state *pm)
{

	spin_lock_bh(&pm->lock);
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 10, 0))
	pm->expires_save = pm->wakelock.ws.timer.expires;
#else
	pm->expires_save = pm->wakelock.expires;
#endif
	wake_lock_timeout(&pm->wakelock, LONG_MAX);
	spin_unlock_bh(&pm->lock);
}
void xradio_pm_unlock_awake(struct xradio_pm_state *pm)
{

	spin_lock_bh(&pm->lock);
	pm->expires_save -= jiffies;
	if (pm->expires_save)
		wake_lock_timeout(&pm->wakelock, pm->expires_save);
	else
		wake_lock_timeout(&pm->wakelock, 1);
	spin_unlock_bh(&pm->lock);
}

#else /* CONFIG_WAKELOCK */

static void xradio_pm_stay_awake_tmo(struct timer_list *t)
{
	struct xradio_pm_state *pm = from_timer(pm, t, stay_awake);
	(void)pm;
}

int xradio_pm_init(struct xradio_pm_state *pm,
		   struct xradio_common *hw_priv)
{
	int ret = 0;
	pm_printk(XRADIO_DBG_MSG,"%s\n", __FUNCTION__);

	ret = xradio_pm_init_common(pm, hw_priv);
	if (!ret) {
		timer_setup(&pm->stay_awake, xradio_pm_stay_awake_tmo, 0);
	} else 
		pm_printk(XRADIO_DBG_ERROR,"xradio_pm_init_common failed!\n");
	return ret;
}

void xradio_pm_deinit(struct xradio_pm_state *pm)
{
	del_timer_sync(&pm->stay_awake);
	xradio_pm_deinit_common(pm);
}

void xradio_pm_stay_awake(struct xradio_pm_state *pm,
			  unsigned long tmo)
{
	long cur_tmo;

	spin_lock_bh(&pm->lock);
	cur_tmo = pm->stay_awake.expires - jiffies;
	if (!timer_pending(&pm->stay_awake) || cur_tmo < (long)tmo)
		mod_timer(&pm->stay_awake, jiffies + tmo);
	spin_unlock_bh(&pm->lock);
}
void xradio_pm_lock_awake(struct xradio_pm_state *pm)
{

	spin_lock_bh(&pm->lock);
	pm->expires_save = pm->stay_awake.expires;
	mod_timer(&pm->stay_awake, jiffies + LONG_MAX);
	spin_unlock_bh(&pm->lock);
}
void xradio_pm_unlock_awake(struct xradio_pm_state *pm)
{

	spin_lock_bh(&pm->lock);
	if (time_before(jiffies, pm->expires_save))
		mod_timer(&pm->stay_awake, pm->expires_save);
	else
		mod_timer(&pm->stay_awake, jiffies + 1);
	spin_unlock_bh(&pm->lock);
}
#endif /* CONFIG_WAKELOCK */

static long xradio_suspend_work(struct delayed_work *work)
{
	int ret = cancel_delayed_work(work);
	long tmo;
	pm_printk(XRADIO_DBG_TRC, "%s\n", __func__);

	if (ret > 0) {
		/* Timer is pending */
		tmo = work->timer.expires - jiffies;
		if (tmo < 0)
			tmo = 0;
	} else {
		tmo = -1;
	}
	return tmo;
}

static int xradio_resume_work(struct xradio_common *hw_priv,
			       struct delayed_work *work,
			       unsigned long tmo)
{
	pm_printk(XRADIO_DBG_TRC, "%s\n", __func__);
	if ((long)tmo < 0)
		return 1;

	return queue_delayed_work(hw_priv->workqueue, work, tmo);
}

static int xradio_suspend_late(struct device *dev)
{
#ifdef CONFIG_XRADIO_SUSPEND_POWER_OFF
	struct xradio_common *hw_priv = dev->platform_data;

	if (XRADIO_POWEROFF_SUSP == atomic_read(&hw_priv->suspend_state)) {
		return 0; /* we don't rx data when power down wifi.*/
	}
#endif

	//if (atomic_read(&hw_priv->bh_rx)) {
	//	pm_printk(XRADIO_DBG_WARN, "%s: Suspend interrupted.\n", __func__);
	//	return -EAGAIN;
	//}
	return 0;
}

static void xradio_pm_release(struct device *dev)
{
	pm_printk(XRADIO_DBG_TRC, "%s\n", __func__);
}

static int xradio_pm_probe(struct platform_device *pdev)
{
	pm_printk(XRADIO_DBG_TRC, "%s\n", __func__);
	pdev->dev.release = xradio_pm_release;
	return 0;
}

int xradio_wow_suspend(struct ieee80211_hw *hw, struct cfg80211_wowlan *wowlan)
{
	struct xradio_common *hw_priv = hw->priv;
	struct xradio_vif *priv;
	int i, ret = 0;


	if(hw_priv->bh_error) return -EBUSY;
	WARN_ON(!atomic_read(&hw_priv->num_vifs));

	if (work_pending(&hw_priv->query_work))
		return -EBUSY;

#ifdef ROAM_OFFLOAD
	xradio_for_each_vif(hw_priv, priv, i) {
		if (!priv)
			continue;
		if((priv->vif->type == NL80211_IFTYPE_STATION)
		&& (priv->join_status == XRADIO_JOIN_STATUS_STA)) {
			down(&hw_priv->scan.lock);
			hw_priv->scan.if_id = priv->if_id;
			xradio_sched_scan_work(&hw_priv->scan.swork);
		}
	}
#endif /*ROAM_OFFLOAD*/

	/* Do not suspend when datapath is not idle */
	if (hw_priv->tx_queue_stats.num_queued[0] + 
		  hw_priv->tx_queue_stats.num_queued[1]) {
		pm_printk(XRADIO_DBG_WARN, "Don't suspend "
		           "because of tx_queue is not empty.\n");
		return -EBUSY;
	}

	/* Make sure there is no configuration requests in progress. */
	if (!mutex_trylock(&hw_priv->conf_mutex)) {
		pm_printk(XRADIO_DBG_WARN, "Don't suspend "
		           "because of configuration requests.\n");
		return -EBUSY;
	}

	/* Make sure there is no wsm_oper_lock in progress. */
	if (!mutex_trylock(&hw_priv->wsm_oper_lock)) {
		pm_printk(XRADIO_DBG_WARN, "Don't suspend "
			   "because of wsm_oper_lock.\n");
		mutex_unlock(&hw_priv->conf_mutex);
		return -EBUSY;
	}

	/* Do not suspend when scanning or ROC*/
	if (down_trylock(&hw_priv->scan.lock)) {
		pm_printk(XRADIO_DBG_WARN, "Don't suspend "
		           "because of scan requests.\n");
		goto revert1;
	}

	if (delayed_work_pending(&hw_priv->scan.probe_work)) {
		pm_printk(XRADIO_DBG_WARN, "Don't suspend "
		           "because of probe frames tx in progress.\n");
		goto revert2;
	}

	/* Lock TX. */
	wsm_lock_tx_async(hw_priv);

	/* Wait to avoid possible race with bh code.
	 * But do not wait too long... */
	if (wait_event_timeout(hw_priv->bh_evt_wq, 
	                       !hw_priv->hw_bufs_used, HZ / 10) <= 0) {
		pm_printk(XRADIO_DBG_WARN, "Don't suspend "
		           "because of there are frames not confirm.\n");
		goto revert3;
	}
		
#ifdef CONFIG_XRADIO_SUSPEND_POWER_OFF
//	if (STANDBY_WITH_POWER_OFF == standby_level) {
	if (1) {
		return xradio_poweroff_suspend(hw_priv);
	}
#endif
	
	xradio_for_each_vif(hw_priv, priv, i) {
		if (!priv)
			continue;

		ret = __xradio_wow_suspend(priv, wowlan);
		if (ret) {
			for (; i >= 0; i--) {
				if (!hw_priv->vif_list[i])
					continue;
				priv = (struct xradio_vif *)hw_priv->vif_list[i]->drv_priv;
				__xradio_wow_resume(priv);
			}
			pm_printk(XRADIO_DBG_WARN, "Don't suspend "
			           "because of __xradio_wow_suspend failed!\n");
			goto revert3;
		}
	}

	/* Stop serving thread */
	if (xradio_bh_suspend(hw_priv)) {
		pm_printk(XRADIO_DBG_WARN, "Don't suspend "
		           "because of xradio_bh_suspend failed!\n");
		xradio_wow_resume(hw);
		return -EBUSY;
	}

	/* Enable IRQ wake */
	ret = sdio_pm(hw_priv, true);
	if (ret) {
		pm_printk(XRADIO_DBG_WARN, "Don't suspend sbus pm failed\n");
		xradio_wow_resume(hw);
		return -EBUSY;
	}

	/* Force resume if event is coming from the device. */
	//if (atomic_read(&hw_priv->bh_rx)) {
	//	pm_printk(XRADIO_DBG_WARN, "Don't suspend "
	//	           "because of recieved rx event!\n");
	//	xradio_wow_resume(hw);
	//	return -EAGAIN;
	//}
	return 0;

revert3:
	wsm_unlock_tx(hw_priv);
revert2:
	up(&hw_priv->scan.lock);
revert1:
	mutex_unlock(&hw_priv->conf_mutex);
	mutex_unlock(&hw_priv->wsm_oper_lock);
	return -EBUSY;
}

static int __xradio_wow_suspend(struct xradio_vif *priv,
				struct cfg80211_wowlan *wowlan)
{
	struct xradio_common *hw_priv = xrwl_vifpriv_to_hwpriv(priv);
	struct xradio_pm_state_vif *pm_state_vif = &priv->pm_state_vif;
	struct xradio_suspend_state *state;
	int ret;
#ifdef MCAST_FWDING
	struct wsm_forwarding_offload fwdoffload = {
		.fwenable = 0x1,
		.flags    = 0x1,
	};
#endif


	/* Do not suspend when join work is scheduled */
	if (work_pending(&priv->join_work)) {
		pm_printk(XRADIO_DBG_WARN, "%s:Do not suspend "
		           "when join work is scheduled\n", __func__);
		goto revert1;
	}

	/* Set UDP filter */
	wsm_set_udp_port_filter(hw_priv, &xradio_udp_port_filter_on.hdr,
	                        priv->if_id);

	/* Set ethernet frame type filter */
	wsm_set_ether_type_filter(hw_priv, &xradio_ether_type_filter_on.hdr,
	                          priv->if_id);

	/* Set IP multicast filter */
    wsm_set_host_sleep(hw_priv, 1, priv->if_id);
    
	if (priv->join_status == XRADIO_JOIN_STATUS_AP)
		WARN_ON(wsm_set_keepalive_filter(priv, true));

#ifdef XRADIO_SUSPEND_RESUME_FILTER_ENABLE
	/* Set Multicast Address Filter */
	if (priv->multicast_filter.numOfAddresses) {
		priv->multicast_filter.enable = 1;
		wsm_set_multicast_filter(hw_priv, &priv->multicast_filter, priv->if_id);
	}

	/* Set Enable Broadcast Address Filter */
	priv->broadcast_filter.action_mode = 1;
	if (priv->join_status == XRADIO_JOIN_STATUS_AP)
		priv->broadcast_filter.address_mode = 3;

	xradio_set_macaddrfilter(hw_priv, priv, (u8 *)&priv->broadcast_filter);
#endif

#ifdef MCAST_FWDING
	if (priv->join_status == XRADIO_JOIN_STATUS_AP)
		WARN_ON(wsm_set_forwarding_offlad(hw_priv, &fwdoffload,priv->if_id));
#endif

	/* Allocate state */
	state = kzalloc(sizeof(struct xradio_suspend_state), GFP_KERNEL);
	if (!state) {
		pm_printk(XRADIO_DBG_WARN, "%s:Do not suspend "
		           "alloc xradio_suspend_state failed.\n", __func__);
		goto revert2;
	}
	/* Store delayed work states. */
	state->bss_loss_tmo        = xradio_suspend_work(&priv->bss_loss_work);
	state->connection_loss_tmo = xradio_suspend_work(&priv->connection_loss_work);
	state->join_tmo   = xradio_suspend_work(&priv->join_timeout);
	state->link_id_gc = xradio_suspend_work(&priv->link_id_gc_work);

	/* Enable beacon skipping */
	if (priv->join_status == XRADIO_JOIN_STATUS_STA && 
		  priv->join_dtim_period &&  !priv->has_multicast_subscription) {
		state->beacon_skipping = true;
		wsm_set_beacon_wakeup_period(hw_priv, priv->join_dtim_period,
		                             XRADIO_BEACON_SKIPPING_MULTIPLIER * \
		                             priv->join_dtim_period, priv->if_id);
	}

	ret = timer_pending(&priv->mcast_timeout);
	if (ret) {
		pm_printk(XRADIO_DBG_WARN, "%s:Do not suspend "
		           "mcast timeout timer_pending failed.\n", __func__);
		goto revert3;
	}

	/* Store suspend state */
	pm_state_vif->suspend_state = state;

	return 0;

revert3:
	xradio_resume_work(hw_priv, &priv->bss_loss_work, state->bss_loss_tmo);
	xradio_resume_work(hw_priv, &priv->connection_loss_work, 
	                   state->connection_loss_tmo);
	xradio_resume_work(hw_priv, &priv->join_timeout, state->join_tmo);
	xradio_resume_work(hw_priv, &priv->link_id_gc_work, state->link_id_gc);
	kfree(state);

revert2:
	wsm_set_udp_port_filter(hw_priv, &xradio_udp_port_filter_off, priv->if_id);
	wsm_set_ether_type_filter(hw_priv, &xradio_ether_type_filter_off, priv->if_id);
    wsm_set_host_sleep(hw_priv, 0, priv->if_id);

	if (priv->join_status == XRADIO_JOIN_STATUS_AP)
		WARN_ON(wsm_set_keepalive_filter(priv, false));

#ifdef XRADIO_SUSPEND_RESUME_FILTER_ENABLE
	/* Set Multicast Address Filter */
	if (priv->multicast_filter.numOfAddresses) {
		priv->multicast_filter.enable = 0;
		wsm_set_multicast_filter(hw_priv, &priv->multicast_filter, priv->if_id);
	}

	/* Set Enable Broadcast Address Filter */
	priv->broadcast_filter.action_mode = 0;
	if (priv->join_status == XRADIO_JOIN_STATUS_AP)
		priv->broadcast_filter.address_mode = 0;
	xradio_set_macaddrfilter(hw_priv, priv, (u8 *)&priv->broadcast_filter);
#endif

#ifdef MCAST_FWDING
	fwdoffload.flags = 0x0;	
	if (priv->join_status == XRADIO_JOIN_STATUS_AP)
		WARN_ON(wsm_set_forwarding_offlad(hw_priv, &fwdoffload,priv->if_id));
#endif

revert1:
	/* mutex_unlock(&hw_priv->conf_mutex); */
	return -EBUSY;
}

int xradio_wow_resume(struct ieee80211_hw *hw)
{

	struct xradio_common *hw_priv = hw->priv;
	struct xradio_vif *priv;
	int i, ret = 0;


	WARN_ON(!atomic_read(&hw_priv->num_vifs));
	if(hw_priv->bh_error) return 0;

#ifdef CONFIG_XRADIO_SUSPEND_POWER_OFF
	if (XRADIO_POWEROFF_SUSP == atomic_read(&hw_priv->suspend_state)) {
		return xradio_poweroff_resume(hw_priv);
	}
#endif

	/* Disable IRQ wake */
	sdio_pm(hw_priv, false);

	up(&hw_priv->scan.lock);

	/* Resume BH thread */
	WARN_ON(xradio_bh_resume(hw_priv));

	xradio_for_each_vif(hw_priv, priv, i) {
		if (!priv)
			continue;
		ret = __xradio_wow_resume(priv);
		if (ret) {
			pm_printk(XRADIO_DBG_ERROR, "%s:__xradio_wow_resume failed!\n", __func__);
			break;
		}
	}

	wsm_unlock_tx(hw_priv);

	/* Unlock configuration mutex */
	mutex_unlock(&hw_priv->conf_mutex);
	mutex_unlock(&hw_priv->wsm_oper_lock);

	return ret;
}

static int __xradio_wow_resume(struct xradio_vif *priv)
{
	struct xradio_common *hw_priv = xrwl_vifpriv_to_hwpriv(priv);
	struct xradio_pm_state_vif *pm_state_vif = &priv->pm_state_vif;
	struct xradio_suspend_state *state;
#ifdef MCAST_FWDING
	struct wsm_forwarding_offload fwdoffload = {
		.fwenable = 0x1,
		.flags = 0x0,
	};
#endif


	/* Restore suspend state */
	state = pm_state_vif->suspend_state;
	pm_state_vif->suspend_state = NULL;

#ifdef ROAM_OFFLOAD
	if((priv->vif->type == NL80211_IFTYPE_STATION)
	&& (priv->join_status == XRADIO_JOIN_STATUS_STA))
		xradio_hw_sched_scan_stop(hw_priv);
#endif /*ROAM_OFFLOAD*/

	if (state->beacon_skipping) {
#ifdef XRADIO_USE_LONG_DTIM_PERIOD
		int join_dtim_period_extend;
		if (priv->join_dtim_period <= 3) {
			join_dtim_period_extend = priv->join_dtim_period * 3;
		} else if (priv->join_dtim_period <= 5) {
			join_dtim_period_extend = priv->join_dtim_period * 2;
		} else {
			join_dtim_period_extend = priv->join_dtim_period;
		}
		wsm_set_beacon_wakeup_period(hw_priv,
			((priv->beacon_int * join_dtim_period_extend) > MAX_BEACON_SKIP_TIME_MS ?
			 1 : join_dtim_period_extend) , 0, priv->if_id);
#else
		wsm_set_beacon_wakeup_period(hw_priv, priv->beacon_int *
			(priv->join_dtim_period > MAX_BEACON_SKIP_TIME_MS ? 1 : priv->join_dtim_period), 
			0, priv->if_id);
#endif
		state->beacon_skipping = false;
	}

	if (priv->join_status == XRADIO_JOIN_STATUS_AP)
		WARN_ON(wsm_set_keepalive_filter(priv, false));

#ifdef XRADIO_SUSPEND_RESUME_FILTER_ENABLE
	/* Set Multicast Address Filter */
	if (priv->multicast_filter.numOfAddresses) {
		priv->multicast_filter.enable = 0;
		wsm_set_multicast_filter(hw_priv, &priv->multicast_filter, priv->if_id);
	}
	/* Set Enable Broadcast Address Filter */
	priv->broadcast_filter.action_mode = 0;
	if (priv->join_status == XRADIO_JOIN_STATUS_AP)
		priv->broadcast_filter.address_mode = 0;
	xradio_set_macaddrfilter(hw_priv, priv, (u8 *)&priv->broadcast_filter);
#endif

#ifdef MCAST_FWDING
	if (priv->join_status == XRADIO_JOIN_STATUS_AP)
		WARN_ON(wsm_set_forwarding_offlad(hw_priv, &fwdoffload,priv->if_id));
#endif

	/* Resume delayed work */
	xradio_resume_work(hw_priv, &priv->bss_loss_work, state->bss_loss_tmo);
	xradio_resume_work(hw_priv, &priv->connection_loss_work,
	                   state->connection_loss_tmo);
	xradio_resume_work(hw_priv, &priv->join_timeout, state->join_tmo);
	xradio_resume_work(hw_priv, &priv->link_id_gc_work, state->link_id_gc);

	/* Remove UDP port filter */
	wsm_set_udp_port_filter(hw_priv, &xradio_udp_port_filter_off, priv->if_id);

	/* Remove ethernet frame type filter */
	wsm_set_ether_type_filter(hw_priv, &xradio_ether_type_filter_off, priv->if_id);
	
	/* Remove IP multicast filter */
    wsm_set_host_sleep(hw_priv, 0, priv->if_id);
	/* Free memory */
	kfree(state);

	return 0;
}
#ifdef CONFIG_XRADIO_SUSPEND_POWER_OFF
static int xradio_poweroff_suspend(struct xradio_common *hw_priv)
{

	//flush all work.
	cancel_work_sync(&hw_priv->query_work);
	flush_workqueue(hw_priv->workqueue);
	/* Schedule hardware restart, ensure no cmds in progress.*/
	mutex_lock(&hw_priv->wsm_cmd_mux);
	atomic_set(&hw_priv->suspend_state, XRADIO_POWEROFF_SUSP);
	//hw_priv->hw_restart = true;
	mutex_unlock(&hw_priv->wsm_cmd_mux);
	/* Stop serving thread */
	if (xradio_bh_suspend(hw_priv)) {
		pm_printk(XRADIO_DBG_WARN, "%s, xradio_bh_suspend failed!\n", __func__);
		return -EBUSY;
	}

	return 0;
}

static int xradio_poweroff_resume(struct xradio_common *hw_priv)
{

	/* Revert locks */
	wsm_unlock_tx(hw_priv);
	up(&hw_priv->scan.lock);
	mutex_unlock(&hw_priv->conf_mutex);
	mutex_unlock(&hw_priv->wsm_oper_lock);
	//if (schedule_work(&hw_priv->hw_restart_work) <= 0)
	//	pm_printk(XRADIO_DBG_ERROR, "%s restart_work failed!\n", __func__);
	return 0;
}
#endif

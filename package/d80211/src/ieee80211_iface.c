/*
 * Copyright 2002-2005, Instant802 Networks, Inc.
 * Copyright 2005-2006, Devicescape Software, Inc.
 * Copyright (c) 2006 Jiri Benc <jbenc@suse.cz>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/kernel.h>
#include <linux/if_arp.h>
#include <linux/netdevice.h>
#include <linux/rtnetlink.h>
#include <net/d80211.h>
#include "ieee80211_i.h"
#include "sta_info.h"

void ieee80211_if_sdata_init(struct ieee80211_sub_if_data *sdata)
{
	int i;

	/* Default values for sub-interface parameters */
	sdata->drop_unencrypted = 0;
	sdata->eapol = 1;
	for (i = 0; i < IEEE80211_FRAGMENT_MAX; i++)
		skb_queue_head_init(&sdata->fragments[i].skb_list);
}

static void ieee80211_if_sdata_deinit(struct ieee80211_sub_if_data *sdata)
{
	int i;

	for (i = 0; i < IEEE80211_FRAGMENT_MAX; i++) {
		__skb_queue_purge(&sdata->fragments[i].skb_list);
	}
}

/* Must be called with rtnl lock held. */
int ieee80211_if_add(struct net_device *dev, const char *name,
		     int format, struct net_device **new_dev)
{
	struct net_device *ndev, *tmp_dev;
	struct ieee80211_local *local = dev->ieee80211_ptr;
	struct ieee80211_sub_if_data *sdata = NULL;
	int ret;
	int i;

	ASSERT_RTNL();
	ndev = *new_dev = alloc_netdev(sizeof(struct ieee80211_sub_if_data),
				       "", ieee80211_if_setup);
	if (!ndev)
		return -ENOMEM;

	ndev->ieee80211_ptr = local;
	if (strlen(name) == 0) {
		i = 0;
		do {
			snprintf(ndev->name, sizeof(ndev->name), "%s.%d",
				 dev->name, i++);
			tmp_dev = dev_get_by_name(ndev->name);
			if (!tmp_dev)
				break;
			dev_put(tmp_dev);
		} while (i < 10000);
	} else if (format) {
		ret = dev_alloc_name(ndev, name);
		if (ret < 0)
			goto fail;
	} else {
		snprintf(ndev->name, IFNAMSIZ, "%s", name);
	}

	memcpy(ndev->dev_addr, local->hw.perm_addr, ETH_ALEN);
	ndev->base_addr = dev->base_addr;
	ndev->irq = dev->irq;
	ndev->mem_start = dev->mem_start;
	ndev->mem_end = dev->mem_end;
	ndev->flags = dev->flags & IFF_MULTICAST;
	SET_NETDEV_DEV(ndev, local->hw.dev);

	sdata = IEEE80211_DEV_TO_SUB_IF(ndev);
	sdata->type = IEEE80211_IF_TYPE_AP;
	sdata->dev = ndev;
	sdata->local = local;
	ieee80211_if_sdata_init(sdata);

	ret = register_netdevice(ndev);
	if (ret)
		goto fail;
	ret = ieee80211_sysfs_add_netdevice(ndev);
	if (ret) {
		/* ndev will be freed by ndev->destructor */
		unregister_netdevice(ndev);
		*new_dev = NULL;
		return ret;
	}

	list_add(&sdata->list, &local->sub_if_list);
	ieee80211_update_default_wep_only(local);

	return 0;

fail:
	free_netdev(ndev);
	*new_dev = NULL;
	return ret;
}

int ieee80211_if_add_mgmt(struct ieee80211_local *local)
{
	struct net_device *ndev;
	struct ieee80211_sub_if_data *nsdata;
	int ret;

	ASSERT_RTNL();

	ndev = alloc_netdev(sizeof(struct ieee80211_sub_if_data), "",
			    ieee80211_if_mgmt_setup);
	if (!ndev)
		return -ENOMEM;
	ret = dev_alloc_name(ndev, "wmgmt%d");
	if (ret < 0)
		goto fail;

	ndev->ieee80211_ptr = local;
	memcpy(ndev->dev_addr, local->hw.perm_addr, ETH_ALEN);
	SET_NETDEV_DEV(ndev, local->hw.dev);

	nsdata = IEEE80211_DEV_TO_SUB_IF(ndev);
	nsdata->type = IEEE80211_IF_TYPE_MGMT;
	nsdata->dev = ndev;
	nsdata->local = local;
	ieee80211_if_sdata_init(nsdata);

	ret = register_netdevice(ndev);
	if (ret)
		goto fail;
	ret = ieee80211_sysfs_add_netdevice(ndev);
	if (ret)
		goto fail_sysfs;
	if (local->open_count > 0)
		dev_open(ndev);
	local->apdev = ndev;
	return 0;

fail_sysfs:
	unregister_netdevice(ndev);
fail:
	free_netdev(ndev);
	return ret;
}

void ieee80211_if_del_mgmt(struct ieee80211_local *local)
{
	struct net_device *apdev;

	ASSERT_RTNL();
	apdev = local->apdev;
	ieee80211_sysfs_remove_netdevice(apdev);
	local->apdev = NULL;
	unregister_netdevice(apdev);
}

void ieee80211_if_set_type(struct net_device *dev, int type)
{
	struct ieee80211_sub_if_data *sdata = IEEE80211_DEV_TO_SUB_IF(dev);
	struct ieee80211_local *local = dev->ieee80211_ptr;

	sdata->type = type;
	switch (type) {
	case IEEE80211_IF_TYPE_WDS:
		sdata->bss = NULL;
		break;
	case IEEE80211_IF_TYPE_VLAN:
		break;
	case IEEE80211_IF_TYPE_AP:
		sdata->u.ap.dtim_period = 2;
		sdata->u.ap.force_unicast_rateidx = -1;
		sdata->u.ap.max_ratectrl_rateidx = -1;
		skb_queue_head_init(&sdata->u.ap.ps_bc_buf);
		sdata->bss = &sdata->u.ap;
		break;
	case IEEE80211_IF_TYPE_STA:
	case IEEE80211_IF_TYPE_IBSS: {
		struct ieee80211_sub_if_data *msdata;
		struct ieee80211_if_sta *ifsta;

		ifsta = &sdata->u.sta;
		INIT_DELAYED_WORK(&ifsta->work, ieee80211_sta_work);

		ifsta->capab = WLAN_CAPABILITY_ESS;
		ifsta->auth_algs = IEEE80211_AUTH_ALG_OPEN |
			IEEE80211_AUTH_ALG_SHARED_KEY;
		ifsta->create_ibss = 1;
		ifsta->wmm_enabled = 1;

		msdata = IEEE80211_DEV_TO_SUB_IF(sdata->local->mdev);
		sdata->bss = &msdata->u.ap;
		break;
	}
	case IEEE80211_IF_TYPE_MNTR:
		dev->type = ARPHRD_IEEE80211_PRISM;
		break;
	default:
		printk(KERN_WARNING "%s: %s: Unknown interface type 0x%x",
		       dev->name, __FUNCTION__, type);
	}
	ieee80211_sysfs_change_if_type(dev);
	ieee80211_update_default_wep_only(local);
}

/* Must be called with rtnl lock held. */
void ieee80211_if_reinit(struct net_device *dev)
{
	struct ieee80211_local *local = dev->ieee80211_ptr;
	struct ieee80211_sub_if_data *sdata = IEEE80211_DEV_TO_SUB_IF(dev);
	struct sta_info *sta;
	int i;

	ASSERT_RTNL();
	ieee80211_if_sdata_deinit(sdata);
	for (i = 0; i < NUM_DEFAULT_KEYS; i++) {
		if (!sdata->keys[i])
			continue;
#if 0
		/* The interface is down at the moment, so there is not
		 * really much point in disabling the keys at this point. */
		memset(addr, 0xff, ETH_ALEN);
		if (local->ops->set_key)
			local->ops->set_key(local_to_hw(local), DISABLE_KEY, addr,
					    local->keys[i], 0);
#endif
		ieee80211_key_free(sdata->keys[i]);
		sdata->keys[i] = NULL;
	}

	switch (sdata->type) {
	case IEEE80211_IF_TYPE_AP: {
		/* Remove all virtual interfaces that use this BSS
		 * as their sdata->bss */
		struct ieee80211_sub_if_data *tsdata, *n;

		list_for_each_entry_safe(tsdata, n, &local->sub_if_list, list) {
			if (tsdata != sdata && tsdata->bss == &sdata->u.ap) {
				printk(KERN_DEBUG "%s: removing virtual "
				       "interface %s because its BSS interface"
				       " is being removed\n",
				       sdata->dev->name, tsdata->dev->name);
				__ieee80211_if_del(local, tsdata);
			}
		}

		kfree(sdata->u.ap.beacon_head);
		kfree(sdata->u.ap.beacon_tail);
		kfree(sdata->u.ap.generic_elem);

		if (dev != local->mdev) {
			struct sk_buff *skb;
			while ((skb = skb_dequeue(&sdata->u.ap.ps_bc_buf))) {
				local->total_ps_buffered--;
				dev_kfree_skb(skb);
			}
		}

		break;
	}
	case IEEE80211_IF_TYPE_WDS:
		sta = sta_info_get(local, sdata->u.wds.remote_addr);
		if (sta) {
			sta_info_put(sta);
			sta_info_free(sta, 0);
		} else {
#ifdef CONFIG_D80211_VERBOSE_DEBUG
			printk(KERN_DEBUG "%s: Someone had deleted my STA "
			       "entry for the WDS link\n", dev->name);
#endif /* CONFIG_D80211_VERBOSE_DEBUG */
		}
		break;
	case IEEE80211_IF_TYPE_STA:
	case IEEE80211_IF_TYPE_IBSS:
		kfree(sdata->u.sta.extra_ie);
		sdata->u.sta.extra_ie = NULL;
		kfree(sdata->u.sta.assocreq_ies);
		sdata->u.sta.assocreq_ies = NULL;
		kfree(sdata->u.sta.assocresp_ies);
		sdata->u.sta.assocresp_ies = NULL;
		if (sdata->u.sta.probe_resp) {
			dev_kfree_skb(sdata->u.sta.probe_resp);
			sdata->u.sta.probe_resp = NULL;
		}

		break;
	case IEEE80211_IF_TYPE_MNTR:
		dev->type = ARPHRD_ETHER;
		break;
	}

	/* remove all STAs that are bound to this virtual interface */
	sta_info_flush(local, dev);

	memset(&sdata->u, 0, sizeof(sdata->u));
	ieee80211_if_sdata_init(sdata);
}

/* Must be called with rtnl lock held. */
void __ieee80211_if_del(struct ieee80211_local *local,
			struct ieee80211_sub_if_data *sdata)
{
	struct net_device *dev = sdata->dev;

	list_del(&sdata->list);
	ieee80211_sysfs_remove_netdevice(dev);
	unregister_netdevice(dev);
	/* Except master interface, the net_device will be freed by
	 * net_device->destructor (i. e. ieee80211_if_free). */
}

/* Must be called with rtnl lock held. */
int ieee80211_if_remove(struct net_device *dev, const char *name, int id)
{
	struct ieee80211_local *local = dev->ieee80211_ptr;
	struct ieee80211_sub_if_data *sdata, *n;

	ASSERT_RTNL();

	list_for_each_entry_safe(sdata, n, &local->sub_if_list, list) {
		if ((sdata->type == id || id == -1) &&
		    strcmp(name, sdata->dev->name) == 0 &&
		    sdata->dev != local->mdev) {
			__ieee80211_if_del(local, sdata);
			ieee80211_update_default_wep_only(local);
			return 0;
		}
	}
	return -ENODEV;
}

void ieee80211_if_free(struct net_device *dev)
{
	struct ieee80211_local *local = dev->ieee80211_ptr;
	struct ieee80211_sub_if_data *sdata = IEEE80211_DEV_TO_SUB_IF(dev);

	/* local->apdev must be NULL when freeing management interface */
	BUG_ON(dev == local->apdev);
	ieee80211_if_sdata_deinit(sdata);
	free_netdev(dev);
}

/* Must be called with rtnl lock held. */
void ieee80211_if_flush(struct net_device *dev)
{
	struct ieee80211_local *local = dev->ieee80211_ptr;
	struct ieee80211_sub_if_data *sdata, *n;

	ASSERT_RTNL();
	list_for_each_entry_safe(sdata, n, &local->sub_if_list, list) {
		__ieee80211_if_del(local, sdata);
	}
}

void ieee80211_if_del(struct net_device *dev)
{
	struct ieee80211_local *local = dev->ieee80211_ptr;
	struct ieee80211_sub_if_data *sdata = IEEE80211_DEV_TO_SUB_IF(dev);

	rtnl_lock();
	if (sdata->type == IEEE80211_IF_TYPE_MGMT)
		ieee80211_if_del_mgmt(local);
	else
		__ieee80211_if_del(local, sdata);
	rtnl_unlock();
}

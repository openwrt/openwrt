/*
 * Copyright (c) 2006 Jiri Benc <jbenc@suse.cz>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/if.h>
#include <linux/if_ether.h>
#include <linux/netdevice.h>
#include <net/d80211.h>
#include "ieee80211_i.h"
#include "ieee80211_led.h"

struct ieee80211_dev_list {
	struct list_head list;
	int dev_index;
	struct ieee80211_local *local;
};

static LIST_HEAD(dev_list);
static DEFINE_SPINLOCK(dev_list_lock);


/* Caller must hold dev_list_lock */
static struct ieee80211_dev_list *__ieee80211_dev_find(int index)
{
	struct ieee80211_dev_list *dev_item;

	list_for_each_entry(dev_item, &dev_list, list) {
		if (dev_item->dev_index == index)
			return dev_item;
	}
	return NULL;
}

int ieee80211_dev_alloc_index(struct ieee80211_local *local)
{
	struct ieee80211_dev_list *dev_item, *new;
	int index = 0;

	new = kmalloc(sizeof(struct ieee80211_dev_list), GFP_KERNEL);
	if (!new)
		return -ENOMEM;
	new->local = local;
	spin_lock(&dev_list_lock);
	list_for_each_entry(dev_item, &dev_list, list) {
		if (index < dev_item->dev_index)
			break;
		index++;
	}
	new->dev_index = index;
	list_add_tail(&new->list, &dev_item->list);
	spin_unlock(&dev_list_lock);
	local->hw.index = index;
	return index;
}

void ieee80211_dev_free_index(struct ieee80211_local *local)
{
	struct ieee80211_dev_list *dev_item;

	spin_lock(&dev_list_lock);
	dev_item = __ieee80211_dev_find(local->hw.index);
	if (dev_item)
		list_del(&dev_item->list);
	spin_unlock(&dev_list_lock);
	if (dev_item)
		kfree(dev_item);
	local->hw.index = -1;
}

struct ieee80211_local *ieee80211_dev_find(int index)
{
	struct ieee80211_dev_list *dev_item;

	spin_lock(&dev_list_lock);
	dev_item = __ieee80211_dev_find(index);
	spin_unlock(&dev_list_lock);
	return dev_item ? dev_item->local : NULL;
}

int ieee80211_dev_find_index(struct ieee80211_local *local)
{
	struct ieee80211_dev_list *dev_item;
	int index = -1;

	spin_lock(&dev_list_lock);
	list_for_each_entry(dev_item, &dev_list, list) {
		if (dev_item->local == local) {
			index = dev_item->dev_index;
			break;
		}
	}
	spin_unlock(&dev_list_lock);
	return index;
}

struct ieee80211_local *ieee80211_dev_alloc(gfp_t flags)
{
	struct ieee80211_local *local;

	local = kzalloc(sizeof(struct ieee80211_local), flags);
	if (!local)
		return NULL;
	local->hw.index = -1;
	ieee80211_dev_sysfs_init(local);
	return local;
}

void ieee80211_dev_free(struct ieee80211_local *local)
{
	ieee80211_dev_sysfs_put(local);
}

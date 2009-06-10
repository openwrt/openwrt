/*
 * wprobe-core.c: Wireless probe interface core
 * Copyright (C) 2008-2009 Felix Fietkau <nbd@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/spinlock.h>
#include <linux/rcupdate.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,26)
#include <linux/rculist.h>
#else
#include <linux/list.h>
#endif
#include <linux/skbuff.h>
#include <linux/wprobe.h>
#include <linux/math64.h>

#define static

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,28)
#define list_for_each_rcu __list_for_each_rcu
#endif

#define WPROBE_MIN_INTERVAL		100 /* minimum measurement interval in msecs */

static struct list_head wprobe_if;
static spinlock_t wprobe_lock;

static struct genl_family wprobe_fam = {
	.id = GENL_ID_GENERATE,
	.name = "wprobe",
	.hdrsize = 0,
	.version = 1,
	/* only the first set of attributes is used for queries */
	.maxattr = WPROBE_ATTR_LAST,
};

static void wprobe_update_stats(struct wprobe_iface *dev, struct wprobe_link *l);
static int wprobe_sync_data(struct wprobe_iface *dev, struct wprobe_link *l, bool query);

int
wprobe_add_link(struct wprobe_iface *s, struct wprobe_link *l, const char *addr)
{
	unsigned long flags;

	INIT_LIST_HEAD(&l->list);
	l->val = kzalloc(sizeof(struct wprobe_value) * s->n_link_items, GFP_ATOMIC);
	if (!l->val)
		return -ENOMEM;

	l->iface = s;
	memcpy(&l->addr, addr, ETH_ALEN);
	spin_lock_irqsave(&wprobe_lock, flags);
	list_add_tail_rcu(&l->list, &s->links);
	spin_unlock_irqrestore(&wprobe_lock, flags);

	return 0;
}
EXPORT_SYMBOL(wprobe_add_link);

void
wprobe_remove_link(struct wprobe_iface *s, struct wprobe_link *l)
{
	unsigned long flags;

	spin_lock_irqsave(&wprobe_lock, flags);
	list_del_rcu(&l->list);
	spin_unlock_irqrestore(&wprobe_lock, flags);
	synchronize_rcu();
	kfree(l->val);
}
EXPORT_SYMBOL(wprobe_remove_link);

static void
wprobe_measure_timer(unsigned long data)
{
	struct wprobe_iface *dev = (struct wprobe_iface *) data;

	/* set next measurement interval */
	mod_timer(&dev->measure_timer, jiffies +
		msecs_to_jiffies(dev->measure_interval));

	/* perform measurement */
	wprobe_sync_data(dev, NULL, false);
}

int
wprobe_add_iface(struct wprobe_iface *s)
{
	unsigned long flags;
	int vsize;

	/* reset only wprobe private area */
	memset(&s->list, 0, sizeof(struct wprobe_iface) - offsetof(struct wprobe_iface, list));

	BUG_ON(!s->name);
	INIT_LIST_HEAD(&s->list);
	INIT_LIST_HEAD(&s->links);
	setup_timer(&s->measure_timer, wprobe_measure_timer, (unsigned long) s);

	vsize = max(s->n_link_items, s->n_global_items);
	s->val = kzalloc(sizeof(struct wprobe_value) * vsize, GFP_ATOMIC);
	if (!s->val)
		goto error;

	s->query_val = kzalloc(sizeof(struct wprobe_value) * vsize, GFP_ATOMIC);
	if (!s->query_val)
		goto error;

	/* initialize defaults to be able to handle overflow,
	 * user space will need to handle this if it keeps an
	 * internal histogram */
	s->scale_min = 20;
	s->scale_max = (1 << 31);

	s->scale_m = 1;
	s->scale_d = 10;

	spin_lock_irqsave(&wprobe_lock, flags);
	list_add_rcu(&s->list, &wprobe_if);
	spin_unlock_irqrestore(&wprobe_lock, flags);

	return 0;

error:
	if (s->val)
		kfree(s->val);
	return -ENOMEM;
}
EXPORT_SYMBOL(wprobe_add_iface);

void
wprobe_remove_iface(struct wprobe_iface *s)
{
	unsigned long flags;

	BUG_ON(!list_empty(&s->links));

	del_timer_sync(&s->measure_timer);
	spin_lock_irqsave(&wprobe_lock, flags);
	list_del_rcu(&s->list);
	spin_unlock_irqrestore(&wprobe_lock, flags);

	/* wait for all queries to finish before freeing the
	 * temporary value storage buffer */
	synchronize_rcu();

	kfree(s->val);
	kfree(s->query_val);
}
EXPORT_SYMBOL(wprobe_remove_iface);

static struct wprobe_iface *
wprobe_get_dev(struct nlattr *attr)
{
	struct wprobe_iface *dev = NULL;
	struct wprobe_iface *p;
	const char *name;
	int i = 0;

	if (!attr)
		return NULL;

	name = nla_data(attr);
	list_for_each_entry_rcu(p, &wprobe_if, list) {
		i++;
		if (strcmp(name, p->name) != 0)
			continue;

		dev = p;
		break;
	}

	return dev;
}

static int
wprobe_sync_data(struct wprobe_iface *dev, struct wprobe_link *l, bool query)
{
	struct wprobe_value *val;
	unsigned long flags;
	int n, err;

	if (l) {
		n = dev->n_link_items;
		val = l->val;
	} else {
		n = dev->n_global_items;
		val = dev->val;
	}

	spin_lock_irqsave(&dev->lock, flags);
	err = dev->sync_data(dev, l, val, !query);
	if (err)
		goto done;

	if (query)
		memcpy(dev->query_val, val, sizeof(struct wprobe_value) * n);

	wprobe_update_stats(dev, l);
done:
	spin_unlock_irqrestore(&dev->lock, flags);
	return 0;
}
EXPORT_SYMBOL(wprobe_sync_data);

static void
wprobe_scale_stats(struct wprobe_iface *dev, const struct wprobe_item *item,
                   struct wprobe_value *val, int n)
{
	u64 scale_ts = jiffies_64;
	int i;

	for (i = 0; i < n; i++) {
		if (!(item[i].flags & WPROBE_F_KEEPSTAT))
			continue;

		if (val[i].n <= dev->scale_min)
			continue;

		/* FIXME: div_s64 seems to be very imprecise here, even when
		 * the values are scaled up */
		val[i].s *= dev->scale_m;
		val[i].s = div_s64(val[i].s, dev->scale_d);

		val[i].ss *= dev->scale_m;
		val[i].ss = div_s64(val[i].ss, dev->scale_d);

		val[i].n = (val[i].n * dev->scale_m) / dev->scale_d;
		val[i].scale_timestamp = scale_ts;
	}
}


void
wprobe_update_stats(struct wprobe_iface *dev, struct wprobe_link *l)
{
	const struct wprobe_item *item;
	struct wprobe_value *val;
	bool scale_stats = false;
	int i, n;

	if (l) {
		n = dev->n_link_items;
		item = dev->link_items;
		val = l->val;
	} else {
		n = dev->n_global_items;
		item = dev->global_items;
		val = dev->val;
	}

	/* process statistics */
	for (i = 0; i < n; i++) {
		s64 v;

		if (!val[i].pending)
			continue;

		val[i].n++;
		if ((item[i].flags & WPROBE_F_KEEPSTAT) &&
			(dev->scale_max > 0) && (val[i].n > dev->scale_max)) {
			scale_stats = true;
		}

		switch(item[i].type) {
		case WPROBE_VAL_S8:
			v = val[i].S8;
			break;
		case WPROBE_VAL_S16:
			v = val[i].S16;
			break;
		case WPROBE_VAL_S32:
			v = val[i].S32;
			break;
		case WPROBE_VAL_S64:
			v = val[i].S64;
			break;
		case WPROBE_VAL_U8:
			v = val[i].U8;
			break;
		case WPROBE_VAL_U16:
			v = val[i].U16;
			break;
		case WPROBE_VAL_U32:
			v = val[i].U32;
			break;
		case WPROBE_VAL_U64:
			v = val[i].U64;
			break;
		default:
			continue;
		}

		val[i].s += v;
		val[i].ss += v * v;
		val[i].pending = false;
	}
	if (scale_stats)
		wprobe_scale_stats(dev, item, val, n);
}
EXPORT_SYMBOL(wprobe_update_stats);

static const struct nla_policy wprobe_policy[WPROBE_ATTR_LAST+1] = {
	[WPROBE_ATTR_INTERFACE] = { .type = NLA_NUL_STRING },
	[WPROBE_ATTR_MAC] = { .type = NLA_STRING },
	[WPROBE_ATTR_FLAGS] = { .type = NLA_U32 },

	/* config */
	[WPROBE_ATTR_INTERVAL] = { .type = NLA_MSECS },
	[WPROBE_ATTR_SAMPLES_MIN] = { .type = NLA_U32 },
	[WPROBE_ATTR_SAMPLES_MAX] = { .type = NLA_U32 },
	[WPROBE_ATTR_SAMPLES_SCALE_M] = { .type = NLA_U32 },
	[WPROBE_ATTR_SAMPLES_SCALE_D] = { .type = NLA_U32 },
};

static bool
wprobe_check_ptr(struct list_head *list, struct list_head *ptr)
{
	struct list_head *p;

	list_for_each_rcu(p, list) {
		if (ptr == p)
			return true;
	}
	return false;
}

static bool
wprobe_send_item_value(struct sk_buff *msg, struct netlink_callback *cb,
                       struct wprobe_iface *dev, struct wprobe_link *l,
                       const struct wprobe_item *item,
                       int i, u32 flags)
{
	struct genlmsghdr *hdr;
	struct wprobe_value *val = dev->query_val;
	u64 time = val[i].last - val[i].first;

	hdr = genlmsg_put(msg, NETLINK_CB(cb->skb).pid, cb->nlh->nlmsg_seq,
			&wprobe_fam, NLM_F_MULTI, WPROBE_CMD_GET_INFO);

	NLA_PUT_U32(msg, WPROBE_ATTR_ID, i);
	NLA_PUT_U32(msg, WPROBE_ATTR_FLAGS, flags);
	NLA_PUT_U8(msg, WPROBE_ATTR_TYPE, item[i].type);
	NLA_PUT_U64(msg, WPROBE_ATTR_DURATION, time);

	switch(item[i].type) {
	case WPROBE_VAL_S8:
	case WPROBE_VAL_U8:
		NLA_PUT_U8(msg, item[i].type, val[i].U8);
		break;
	case WPROBE_VAL_S16:
	case WPROBE_VAL_U16:
		NLA_PUT_U16(msg, item[i].type, val[i].U16);
		break;
	case WPROBE_VAL_S32:
	case WPROBE_VAL_U32:
		NLA_PUT_U32(msg, item[i].type, val[i].U32);
		break;
	case WPROBE_VAL_S64:
	case WPROBE_VAL_U64:
		NLA_PUT_U64(msg, item[i].type, val[i].U64);
		break;
	case WPROBE_VAL_STRING:
		if (val[i].STRING)
			NLA_PUT_STRING(msg, item[i].type, val[i].STRING);
		else
			NLA_PUT_STRING(msg, item[i].type, "");
		/* bypass avg/stdev */
		goto done;
	default:
		/* skip unknown values */
		goto done;
	}
	if (item[i].flags & WPROBE_F_KEEPSTAT) {
		NLA_PUT_U64(msg, WPROBE_VAL_SUM, val[i].s);
		NLA_PUT_U64(msg, WPROBE_VAL_SUM_SQ, val[i].ss);
		NLA_PUT_U32(msg, WPROBE_VAL_SAMPLES, (u32) val[i].n);
		NLA_PUT_MSECS(msg, WPROBE_VAL_SCALE_TIME, val[i].scale_timestamp);
	}
done:
	genlmsg_end(msg, hdr);
	return true;

nla_put_failure:
	genlmsg_cancel(msg, hdr);
	return false;
}

static bool
wprobe_send_item_info(struct sk_buff *msg, struct netlink_callback *cb,
                       struct wprobe_iface *dev,
                       const struct wprobe_item *item, int i)
{
	struct genlmsghdr *hdr;

	hdr = genlmsg_put(msg, NETLINK_CB(cb->skb).pid, cb->nlh->nlmsg_seq,
			&wprobe_fam, NLM_F_MULTI, WPROBE_CMD_GET_LIST);

	if ((i == 0) && (dev->addr != NULL))
		NLA_PUT(msg, WPROBE_ATTR_MAC, 6, dev->addr);
	NLA_PUT_U32(msg, WPROBE_ATTR_ID, (u32) i);
	NLA_PUT_STRING(msg, WPROBE_ATTR_NAME, item[i].name);
	NLA_PUT_U8(msg, WPROBE_ATTR_TYPE, item[i].type);
	NLA_PUT_U32(msg, WPROBE_ATTR_FLAGS, item[i].flags);
	genlmsg_end(msg, hdr);
	return true;

nla_put_failure:
	genlmsg_cancel(msg, hdr);
	return false;
}


static struct wprobe_link *
wprobe_find_link(struct wprobe_iface *dev, const char *mac)
{
	struct wprobe_link *l;

	list_for_each_entry_rcu(l, &dev->links, list) {
		if (!memcmp(l->addr, mac, 6))
			return l;
	}
	return NULL;
}

static bool
wprobe_dump_link(struct sk_buff *msg, struct wprobe_link *l, struct netlink_callback *cb)
{
	struct genlmsghdr *hdr;

	hdr = genlmsg_put(msg, NETLINK_CB(cb->skb).pid, cb->nlh->nlmsg_seq,
			&wprobe_fam, NLM_F_MULTI, WPROBE_CMD_GET_LINKS);
	if (!hdr)
		return false;

	NLA_PUT(msg, WPROBE_ATTR_MAC, 6, l->addr);
	genlmsg_end(msg, hdr);
	return true;

nla_put_failure:
	genlmsg_cancel(msg, hdr);
	return false;
}

static int
wprobe_dump_links(struct sk_buff *skb, struct netlink_callback *cb)
{
	struct wprobe_iface *dev = (struct wprobe_iface *)cb->args[0];
	struct wprobe_link *l;
	int err = 0;
	int i = 0;

	if (!dev) {
		err = nlmsg_parse(cb->nlh, GENL_HDRLEN + wprobe_fam.hdrsize,
				wprobe_fam.attrbuf, wprobe_fam.maxattr, wprobe_policy);
		if (err)
			goto done;

		dev = wprobe_get_dev(wprobe_fam.attrbuf[WPROBE_ATTR_INTERFACE]);
		if (!dev) {
			err = -ENODEV;
			goto done;
		}

		cb->args[0] = (long) dev;
	} else {
		if (!wprobe_check_ptr(&wprobe_if, &dev->list)) {
			err = -ENODEV;
			goto done;
		}
	}

	rcu_read_lock();
	list_for_each_entry_rcu(l, &dev->links, list) {
		if (i < cb->args[1])
			continue;

		if (unlikely(!wprobe_dump_link(skb, l, cb)))
			break;

		i++;
	}
	cb->args[1] = i;
	rcu_read_unlock();
	err = skb->len;
done:
	return err;
}

#define WPROBE_F_LINK (1 << 31) /* for internal use */
static int
wprobe_dump_info(struct sk_buff *skb, struct netlink_callback *cb)
{
	struct wprobe_iface *dev = (struct wprobe_iface *)cb->args[0];
	struct wprobe_link *l = (struct wprobe_link *)cb->args[1];
	struct wprobe_value *val;
	const struct wprobe_item *item;
	struct genlmsghdr *hdr;
	unsigned long flags;
	int cmd, n, i = cb->args[3];
	u32	vflags = cb->args[2];
	int err = 0;

	hdr = (struct genlmsghdr *)nlmsg_data(cb->nlh);
	cmd = hdr->cmd;

	/* since the attribute value list might be too big for a single netlink
	 * message, the device, link and offset get stored in the netlink callback.
	 * if this is the first request, we need to do the full lookup for the device.
	 *
	 * access to the device and link structure is synchronized through rcu.
	 */
	rcu_read_lock();
	if (!dev) {
		err = nlmsg_parse(cb->nlh, GENL_HDRLEN + wprobe_fam.hdrsize,
				wprobe_fam.attrbuf, wprobe_fam.maxattr, wprobe_policy);
		if (err)
			goto done;

		err = -ENOENT;
		dev = wprobe_get_dev(wprobe_fam.attrbuf[WPROBE_ATTR_INTERFACE]);
		if (!dev)
			goto done;

		if (cmd == WPROBE_CMD_GET_INFO) {
			if (wprobe_fam.attrbuf[WPROBE_ATTR_MAC]) {
				l = wprobe_find_link(dev, nla_data(wprobe_fam.attrbuf[WPROBE_ATTR_MAC]));
				if (!l)
					goto done;

				vflags = l->flags;
			}

			if (l) {
				item = dev->link_items;
				n = dev->n_link_items;
				val = l->val;
			} else {
				item = dev->global_items;
				n = dev->n_global_items;
				val = dev->val;
			}

			/* sync data and move to temp storage for the query */
			spin_lock_irqsave(&dev->lock, flags);
			err = wprobe_sync_data(dev, l, true);
			if (!err)
				memcpy(dev->query_val, val, n * sizeof(struct wprobe_value));
			spin_unlock_irqrestore(&dev->lock, flags);

			if (err)
				goto done;
		}

		if (wprobe_fam.attrbuf[WPROBE_ATTR_FLAGS])
			vflags |= nla_get_u32(wprobe_fam.attrbuf[WPROBE_ATTR_FLAGS]);

		if (wprobe_fam.attrbuf[WPROBE_ATTR_MAC])
			vflags |= WPROBE_F_LINK;

		cb->args[0] = (long) dev;
		cb->args[1] = (long) l;
		cb->args[2] = vflags;
		cb->args[3] = 0;
	} else {
		/* when pulling pointers from the callback, validate them
		 * against the list using rcu to make sure that we won't
		 * dereference pointers to free'd memory after the last
		 * grace period */
		err = -ENOENT;
		if (!wprobe_check_ptr(&wprobe_if, &dev->list))
			goto done;

		if (l && !wprobe_check_ptr(&dev->links, &l->list))
			goto done;
	}

	if (vflags & WPROBE_F_LINK) {
		item = dev->link_items;
		n = dev->n_link_items;
	} else {
		item = dev->global_items;
		n = dev->n_global_items;
	}

	err = 0;
	switch(cmd) {
	case WPROBE_CMD_GET_INFO:
		while (i < n) {
			if (!wprobe_send_item_value(skb, cb, dev, l, item, i, vflags))
				break;
			i++;
		}
		break;
	case WPROBE_CMD_GET_LIST:
		while (i < n) {
			if (!wprobe_send_item_info(skb, cb, dev, item, i))
				break;
			i++;
		}
		break;
	default:
		err = -EINVAL;
		goto done;
	}
	cb->args[3] = i;
	err = skb->len;

done:
	rcu_read_unlock();
	return err;
}
#undef WPROBE_F_LINK

static int
wprobe_update_auto_measurement(struct wprobe_iface *dev, u32 interval)
{
	if (interval && (interval < WPROBE_MIN_INTERVAL))
		return -EINVAL;

	if (!interval && dev->measure_interval)
		del_timer_sync(&dev->measure_timer);

	dev->measure_interval = interval;
	if (!interval)
		return 0;

	/* kick of a new measurement immediately */
	mod_timer(&dev->measure_timer, jiffies + 1);

	return 0;
}

static int
wprobe_measure(struct sk_buff *skb, struct genl_info *info)
{
	struct wprobe_iface *dev;
	struct wprobe_link *l = NULL;
	int err = -ENOENT;

	rcu_read_lock();
	dev = wprobe_get_dev(info->attrs[WPROBE_ATTR_INTERFACE]);
	if (!dev)
		goto done;

	if (info->attrs[WPROBE_ATTR_MAC]) {
		l = wprobe_find_link(dev, nla_data(wprobe_fam.attrbuf[WPROBE_ATTR_MAC]));
		if (!l)
			goto done;
	}

	err = wprobe_sync_data(dev, l, false);

done:
	rcu_read_unlock();
	return err;
}

static int
wprobe_set_config(struct sk_buff *skb, struct genl_info *info)
{
	struct wprobe_iface *dev;
	unsigned long flags;
	int err = -ENOENT;
	u32 scale_min, scale_max;
	u32 scale_m, scale_d;

	rcu_read_lock();
	dev = wprobe_get_dev(info->attrs[WPROBE_ATTR_INTERFACE]);
	if (!dev)
		goto done_unlocked;

	err = -EINVAL;
	spin_lock_irqsave(&dev->lock, flags);
	if (info->attrs[WPROBE_ATTR_MAC]) {
		/* not supported yet */
		goto done;
	}

	if (info->attrs[WPROBE_ATTR_SAMPLES_MIN] ||
		info->attrs[WPROBE_ATTR_SAMPLES_MAX]) {
		if (info->attrs[WPROBE_ATTR_SAMPLES_MIN])
			scale_min = nla_get_u32(info->attrs[WPROBE_ATTR_SAMPLES_MIN]);
		else
			scale_min = dev->scale_min;

		if (info->attrs[WPROBE_ATTR_SAMPLES_MAX])
			scale_max = nla_get_u32(info->attrs[WPROBE_ATTR_SAMPLES_MAX]);
		else
			scale_max = dev->scale_max;

		if ((!scale_min && !scale_max) ||
		    (scale_min && scale_max && (scale_min < scale_max))) {
			dev->scale_min = scale_min;
			dev->scale_max = scale_max;
		} else {
			goto done;
		}
	}

	if (info->attrs[WPROBE_ATTR_SAMPLES_SCALE_M] &&
		info->attrs[WPROBE_ATTR_SAMPLES_SCALE_D]) {

		scale_m = nla_get_u32(info->attrs[WPROBE_ATTR_SAMPLES_SCALE_M]);
		scale_d = nla_get_u32(info->attrs[WPROBE_ATTR_SAMPLES_SCALE_D]);

		if (!scale_d || (scale_m > scale_d))
			goto done;

		dev->scale_m = scale_m;
		dev->scale_d = scale_d;
	}

	err = 0;
	if (info->attrs[WPROBE_ATTR_INTERVAL]) {
		/* change of measurement interval requested */
		err = wprobe_update_auto_measurement(dev,
			(u32) nla_get_u64(info->attrs[WPROBE_ATTR_INTERVAL]));
	}

done:
	spin_unlock_irqrestore(&dev->lock, flags);
done_unlocked:
	rcu_read_unlock();
	return err;
}

static struct genl_ops wprobe_ops[] = {
	{
		.cmd = WPROBE_CMD_GET_INFO,
		.dumpit = wprobe_dump_info,
		.policy = wprobe_policy,
	},
	{
		.cmd = WPROBE_CMD_GET_LIST,
		.dumpit = wprobe_dump_info,
		.policy = wprobe_policy,
	},
	{
		.cmd = WPROBE_CMD_MEASURE,
		.doit = wprobe_measure,
		.policy = wprobe_policy,
	},
	{
		.cmd = WPROBE_CMD_GET_LINKS,
		.dumpit = wprobe_dump_links,
		.policy = wprobe_policy,
	},
	{
		.cmd = WPROBE_CMD_CONFIG,
		.doit = wprobe_set_config,
	},
};

static void __exit
wprobe_exit(void)
{
	BUG_ON(!list_empty(&wprobe_if));
	genl_unregister_family(&wprobe_fam);
}


static int __init
wprobe_init(void)
{
	int i, err;

	spin_lock_init(&wprobe_lock);
	INIT_LIST_HEAD(&wprobe_if);

	err = genl_register_family(&wprobe_fam);
	if (err)
		return err;

	for (i = 0; i < ARRAY_SIZE(wprobe_ops); i++) {
		err = genl_register_ops(&wprobe_fam, &wprobe_ops[i]);
		if (err)
			goto error;
	}

	return 0;

error:
	genl_unregister_family(&wprobe_fam);
	return err;
}

module_init(wprobe_init);
module_exit(wprobe_exit);
MODULE_LICENSE("GPL");


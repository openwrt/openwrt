/*
 *  Button Hotplug driver
 *
 *  Copyright (C) 2008 Gabor Juhos <juhosg@openwrt.org>
 *
 *  Based on the diag.c - GPIO interface driver for Broadcom boards
 *    Copyright (C) 2006 Mike Baker <mbm@openwrt.org>,
 *    Copyright (C) 2006-2007 Felix Fietkau <nbd@openwrt.org>
 *    Copyright (C) 2008 Andy Boyett <agb@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/version.h>
#include <linux/kmod.h>
#include <linux/input.h>

#include <linux/workqueue.h>
#include <linux/skbuff.h>
#include <linux/netlink.h>
#include <net/sock.h>

#define DRV_NAME	"button-hotplug"
#define DRV_VERSION	"0.3.1"
#define DRV_DESC	"Button Hotplug driver"

#define BH_SKB_SIZE	2048

#define BH_BTN_MIN	BTN_0
#define BH_BTN_MAX	BTN_9

#define BH_BTN_COUNT	(BH_BTN_MAX - BH_BTN_MIN + 1)

#define PFX	DRV_NAME ": "

#undef BH_DEBUG

#ifdef BH_DEBUG
#define BH_DBG(fmt, args...) printk(KERN_DEBUG "%s: " fmt, DRV_NAME, ##args )
#else
#define BH_DBG(fmt, args...) do {} while (0)
#endif

#define BH_ERR(fmt, args...) printk(KERN_ERR "%s: " fmt, DRV_NAME, ##args )

#ifndef BIT_MASK
#define BIT_MASK(nr)            (1UL << ((nr) % BITS_PER_LONG))
#endif

struct bh_priv {
	unsigned long		seen[BH_BTN_COUNT];
	struct input_handle	handle;
};

struct bh_event {
	char			*name;
	char			*action;
	unsigned long		seen;

	struct sk_buff		*skb;
	struct work_struct	work;
};

extern struct sock *uevent_sock;
extern u64 uevent_next_seqnum(void);

static char *button_names[BH_BTN_COUNT] = {
	"BTN_0", "BTN_1", "BTN_2", "BTN_3", "BTN_4",
	"BTN_5", "BTN_6", "BTN_7", "BTN_8", "BTN_9"
};

/* -------------------------------------------------------------------------*/

static int bh_event_add_var(struct bh_event *event, int argv,
		const char *format, ...)
{
	static char buf[128];
	char *s;
	va_list args;
	int len;

	if (argv)
		return 0;

	va_start(args, format);
	len = vsnprintf(buf, sizeof(buf), format, args);
	va_end(args);

	if (len >= sizeof(buf)) {
		BH_ERR("buffer size too small\n");
		WARN_ON(1);
		return -ENOMEM;
	}

	s = skb_put(event->skb, len + 1);
	strcpy(s, buf);

	BH_DBG("added variable '%s'\n", s);

	return 0;
}

static int button_hotplug_fill_event(struct bh_event *event)
{
	int ret;

	ret = bh_event_add_var(event, 0, "HOME=%s", "/");
	if (ret)
		return ret;

	ret = bh_event_add_var(event, 0, "PATH=%s",
					"/sbin:/bin:/usr/sbin:/usr/bin");
	if (ret)
		return ret;

	ret = bh_event_add_var(event, 0, "SUBSYSTEM=%s", "button");
	if (ret)
		return ret;

	ret = bh_event_add_var(event, 0, "ACTION=%s", event->action);
	if (ret)
		return ret;

	ret = bh_event_add_var(event, 0, "BUTTON=%s", event->name);
	if (ret)
		return ret;

	ret = bh_event_add_var(event, 0, "SEEN=%ld", event->seen);
	if (ret)
		return ret;

	ret = bh_event_add_var(event, 0, "SEQNUM=%llu", uevent_next_seqnum());

	return ret;
}

static void button_hotplug_work(struct work_struct *work)
{
	struct bh_event *event = container_of(work, struct bh_event, work);
	int ret = 0;

	if (!uevent_sock)
		goto out_free_event;

	event->skb = alloc_skb(BH_SKB_SIZE, GFP_KERNEL);
	if (!event->skb)
		goto out_free_event;

	ret = bh_event_add_var(event, 0, "%s@", event->action);
	if (ret)
		goto out_free_skb;

	ret = button_hotplug_fill_event(event);
	if (ret)
		goto out_free_skb;

	NETLINK_CB(event->skb).dst_group = 1;
	netlink_broadcast(uevent_sock, event->skb, 0, 1, GFP_KERNEL);

 out_free_skb:
	if (ret) {
		BH_ERR("work error %d\n", ret);
		kfree_skb(event->skb);
	}
 out_free_event:
	kfree(event);
}

static int button_hotplug_create_event(char *name, unsigned long seen,
		int pressed)
{
	struct bh_event *event;

	BH_DBG("create event, name=%s, seen=%lu, pressed=%d\n",
		name, seen, pressed);

	event = kzalloc(sizeof(*event), GFP_KERNEL);
	if (!event)
		return -ENOMEM;

	event->name = name;
	event->seen = seen;
	event->action = pressed ? "pressed" : "released";

	INIT_WORK(&event->work, (void *)(void *)button_hotplug_work);
	schedule_work(&event->work);

	return 0;
}

/* -------------------------------------------------------------------------*/

#ifdef	CONFIG_HOTPLUG
static void button_hotplug_event(struct input_handle *handle,
			   unsigned int type, unsigned int code, int value)
{
	struct bh_priv *priv = handle->private;
	unsigned long seen = jiffies;
	unsigned int btn;

	BH_DBG("event type=%u, code=%u, value=%d\n", type, code, value);

	if (type != EV_KEY)
		return;

	if (code < BH_BTN_MIN || code > BH_BTN_MAX)
		return;

	btn = code - BH_BTN_MIN;
	button_hotplug_create_event(button_names[btn],
			(seen - priv->seen[btn]) / HZ, value);
	priv->seen[btn] = seen;
}
#else
static void button_hotplug_event(struct input_handle *handle,
			   unsigned int type, unsigned int code, int value)
{
}
#endif	/* CONFIG_HOTPLUG */

static int button_hotplug_connect(struct input_handler *handler,
		struct input_dev *dev, const struct input_device_id *id)
{
	struct bh_priv *priv;
	int ret;
	int i;

	for (i = BH_BTN_MIN; i <= BH_BTN_MAX; i++)
		if (test_bit(i, dev->keybit))
			break;

	if (i > BH_BTN_MAX)
		return -ENODEV;

	priv = kzalloc(sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	priv->handle.private = priv;
	priv->handle.dev = dev;
	priv->handle.handler = handler;
	priv->handle.name = DRV_NAME;

	ret = input_register_handle(&priv->handle);
	if (ret)
		goto err_free_priv;

	ret = input_open_device(&priv->handle);
	if (ret)
		goto err_unregister_handle;

	BH_DBG("connected to %s\n", dev->name);

	return 0;

 err_unregister_handle:
	input_unregister_handle(&priv->handle);

 err_free_priv:
	kfree(priv);
	return ret;
}

static void button_hotplug_disconnect(struct input_handle *handle)
{
	struct bh_priv *priv = handle->private;

	input_close_device(handle);
	input_unregister_handle(handle);

	kfree(priv);
}

static const struct input_device_id button_hotplug_ids[] = {
	{
                .flags = INPUT_DEVICE_ID_MATCH_EVBIT,
                .evbit = { BIT_MASK(EV_KEY) },
        },
	{
		/* Terminating entry */
	},
};

MODULE_DEVICE_TABLE(input, button_hotplug_ids);

static struct input_handler button_hotplug_handler = {
	.event =	button_hotplug_event,
	.connect =	button_hotplug_connect,
	.disconnect =	button_hotplug_disconnect,
	.name =		DRV_NAME,
	.id_table =	button_hotplug_ids,
};

/* -------------------------------------------------------------------------*/

static int __init button_hotplug_init(void)
{
	int ret;

	printk(KERN_INFO DRV_DESC " version " DRV_VERSION "\n");
	ret = input_register_handler(&button_hotplug_handler);
	if (ret)
		BH_ERR("unable to register input handler\n");

	return ret;
}
module_init(button_hotplug_init);

static void __exit button_hotplug_exit(void)
{
	input_unregister_handler(&button_hotplug_handler);
}
module_exit(button_hotplug_exit);

MODULE_DESCRIPTION(DRV_DESC);
MODULE_VERSION(DRV_VERSION);
MODULE_AUTHOR("Gabor Juhos <juhosg@openwrt.org>");
MODULE_LICENSE("GPL v2");


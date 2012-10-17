/*
 *  GPIO Button Hotplug driver
 *
 *  Copyright (C) 2012 Felix Fietkau <nbd@openwrt.org>
 *  Copyright (C) 2008-2010 Gabor Juhos <juhosg@openwrt.org>
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

#include <linux/workqueue.h>
#include <linux/skbuff.h>
#include <linux/netlink.h>
#include <linux/kobject.h>
#include <linux/input.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>
#include <linux/gpio_keys.h>

#define DRV_NAME	"gpio-keys-polled"

#define BH_SKB_SIZE	2048

#define PFX	DRV_NAME ": "

#undef BH_DEBUG

#ifdef BH_DEBUG
#define BH_DBG(fmt, args...) printk(KERN_DEBUG "%s: " fmt, DRV_NAME, ##args )
#else
#define BH_DBG(fmt, args...) do {} while (0)
#endif

#define BH_ERR(fmt, args...) printk(KERN_ERR "%s: " fmt, DRV_NAME, ##args )

struct bh_priv {
	unsigned long		seen;
};

struct bh_event {
	const char		*name;
	char			*action;
	unsigned long		seen;

	struct sk_buff		*skb;
	struct work_struct	work;
};

struct bh_map {
	unsigned int	code;
	const char	*name;
};

struct gpio_keys_button_data {
	struct delayed_work work;
	struct bh_priv bh;
	int last_state;
	int count;
	int threshold;
	int can_sleep;
};

extern u64 uevent_next_seqnum(void);

#define BH_MAP(_code, _name)		\
	{				\
		.code = (_code),	\
		.name = (_name),	\
	}

static struct bh_map button_map[] = {
	BH_MAP(BTN_0,		"BTN_0"),
	BH_MAP(BTN_1,		"BTN_1"),
	BH_MAP(BTN_2,		"BTN_2"),
	BH_MAP(BTN_3,		"BTN_3"),
	BH_MAP(BTN_4,		"BTN_4"),
	BH_MAP(BTN_5,		"BTN_5"),
	BH_MAP(BTN_6,		"BTN_6"),
	BH_MAP(BTN_7,		"BTN_7"),
	BH_MAP(BTN_8,		"BTN_8"),
	BH_MAP(BTN_9,		"BTN_9"),
	BH_MAP(KEY_RESTART,	"reset"),
#ifdef KEY_WPS_BUTTON
	BH_MAP(KEY_WPS_BUTTON,	"wps"),
#endif /* KEY_WPS_BUTTON */
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
	broadcast_uevent(event->skb, 0, 1, GFP_KERNEL);

 out_free_skb:
	if (ret) {
		BH_ERR("work error %d\n", ret);
		kfree_skb(event->skb);
	}
 out_free_event:
	kfree(event);
}

static int button_hotplug_create_event(const char *name, unsigned long seen,
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
static int button_get_index(unsigned int code)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(button_map); i++)
		if (button_map[i].code == code)
			return i;

	return -1;
}
static void button_hotplug_event(struct gpio_keys_button_data *data,
			   unsigned int type, unsigned int code, int value)
{
	struct bh_priv *priv = &data->bh;
	unsigned long seen = jiffies;
	int btn;

	BH_DBG("event type=%u, code=%u, value=%d\n", type, code, value);

	if (type != EV_KEY)
		return;

	btn = button_get_index(code);
	if (btn < 0)
		return;

	button_hotplug_create_event(button_map[btn].name,
			(seen - priv->seen) / HZ, value);
	priv->seen = seen;
}
#else
static void button_hotplug_event(struct gpio_keys_button_data *data,
			   unsigned int type, unsigned int code, int value)
{
}
#endif	/* CONFIG_HOTPLUG */

struct gpio_keys_polled_dev {
	struct delayed_work work;

	struct device *dev;
	struct gpio_keys_platform_data *pdata;
	struct gpio_keys_button_data data[0];
};

static void gpio_keys_polled_check_state(struct gpio_keys_button *button,
					 struct gpio_keys_button_data *bdata)
{
	int state;

	if (bdata->can_sleep)
		state = !!gpio_get_value_cansleep(button->gpio);
	else
		state = !!gpio_get_value(button->gpio);

	state = !!(state ^ button->active_low);
	if (state != bdata->last_state) {
		unsigned int type = button->type ?: EV_KEY;

		button_hotplug_event(bdata, type, button->code, state);
		bdata->count = 0;
		bdata->last_state = state;
	}
}

static void gpio_keys_polled_queue_work(struct gpio_keys_polled_dev *bdev)
{
	struct gpio_keys_platform_data *pdata = bdev->pdata;
	unsigned long delay = msecs_to_jiffies(pdata->poll_interval);

	if (delay >= HZ)
	    delay = round_jiffies_relative(delay);
	schedule_delayed_work(&bdev->work, delay);
}

static void gpio_keys_polled_poll(struct work_struct *work)
{
	struct gpio_keys_polled_dev *bdev =
		container_of(work, struct gpio_keys_polled_dev, work.work);
	struct gpio_keys_platform_data *pdata = bdev->pdata;
	int i;

	for (i = 0; i < bdev->pdata->nbuttons; i++) {
		struct gpio_keys_button_data *bdata = &bdev->data[i];

		if (bdata->count < bdata->threshold)
			bdata->count++;
		else
			gpio_keys_polled_check_state(&pdata->buttons[i], bdata);
	}
	gpio_keys_polled_queue_work(bdev);
}

static void __devinit gpio_keys_polled_open(struct gpio_keys_polled_dev *bdev)
{
	struct gpio_keys_platform_data *pdata = bdev->pdata;
	int i;

	if (pdata->enable)
		pdata->enable(bdev->dev);

	/* report initial state of the buttons */
	for (i = 0; i < pdata->nbuttons; i++)
		gpio_keys_polled_check_state(&pdata->buttons[i], &bdev->data[i]);

	gpio_keys_polled_queue_work(bdev);
}

static void __devexit gpio_keys_polled_close(struct gpio_keys_polled_dev *bdev)
{
	struct gpio_keys_platform_data *pdata = bdev->pdata;

	cancel_delayed_work_sync(&bdev->work);

	if (pdata->disable)
		pdata->disable(bdev->dev);
}

static int __devinit gpio_keys_polled_probe(struct platform_device *pdev)
{
	struct gpio_keys_platform_data *pdata = pdev->dev.platform_data;
	struct device *dev = &pdev->dev;
	struct gpio_keys_polled_dev *bdev;
	int error;
	int i;

	if (!pdata || !pdata->poll_interval)
		return -EINVAL;

	bdev = kzalloc(sizeof(struct gpio_keys_polled_dev) +
		       pdata->nbuttons * sizeof(struct gpio_keys_button_data),
		       GFP_KERNEL);
	if (!bdev) {
		dev_err(dev, "no memory for private data\n");
		return -ENOMEM;
	}

	for (i = 0; i < pdata->nbuttons; i++) {
		struct gpio_keys_button *button = &pdata->buttons[i];
		struct gpio_keys_button_data *bdata = &bdev->data[i];
		unsigned int gpio = button->gpio;

		if (button->wakeup) {
			dev_err(dev, DRV_NAME " does not support wakeup\n");
			error = -EINVAL;
			goto err_free_gpio;
		}

		error = gpio_request(gpio,
				     button->desc ? button->desc : DRV_NAME);
		if (error) {
			dev_err(dev, "unable to claim gpio %u, err=%d\n",
				gpio, error);
			goto err_free_gpio;
		}

		error = gpio_direction_input(gpio);
		if (error) {
			dev_err(dev,
				"unable to set direction on gpio %u, err=%d\n",
				gpio, error);
			goto err_free_gpio;
		}

		bdata->can_sleep = gpio_cansleep(gpio);
		bdata->last_state = 0;
		bdata->threshold = DIV_ROUND_UP(button->debounce_interval,
						pdata->poll_interval);
	}

	bdev->dev = &pdev->dev;
	bdev->pdata = pdata;
	platform_set_drvdata(pdev, bdev);

	INIT_DELAYED_WORK(&bdev->work, gpio_keys_polled_poll);

	gpio_keys_polled_open(bdev);

	return 0;

err_free_gpio:
	while (--i >= 0)
		gpio_free(pdata->buttons[i].gpio);

	kfree(bdev);
	platform_set_drvdata(pdev, NULL);

	return error;
}

static int __devexit gpio_keys_polled_remove(struct platform_device *pdev)
{
	struct gpio_keys_polled_dev *bdev = platform_get_drvdata(pdev);
	struct gpio_keys_platform_data *pdata = bdev->pdata;
	int i = pdata->nbuttons;

	gpio_keys_polled_close(bdev);

	while (--i >= 0)
		gpio_free(pdata->buttons[i].gpio);

	kfree(bdev);
	platform_set_drvdata(pdev, NULL);

	return 0;
}

static struct platform_driver gpio_keys_polled_driver = {
	.probe	= gpio_keys_polled_probe,
	.remove	= __devexit_p(gpio_keys_polled_remove),
	.driver	= {
		.name	= DRV_NAME,
		.owner	= THIS_MODULE,
	},
};

static int __init gpio_keys_polled_init(void)
{
	return platform_driver_register(&gpio_keys_polled_driver);
}

static void __exit gpio_keys_polled_exit(void)
{
	platform_driver_unregister(&gpio_keys_polled_driver);
}

module_init(gpio_keys_polled_init);
module_exit(gpio_keys_polled_exit);

MODULE_AUTHOR("Gabor Juhos <juhosg@openwrt.org>");
MODULE_AUTHOR("Felix Fietkau <nbd@openwrt.org>");
MODULE_DESCRIPTION("Polled GPIO Buttons hotplug driver");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:" DRV_NAME);

/*
 *  LED Morse Trigger
 *
 *  Copyright (C) 2007 Gabor Juhos <juhosg at openwrt.org>
 *
 *  This file was based on: drivers/led/ledtrig-timer.c
 *	Copyright 2005-2006 Openedhand Ltd.
 *	Author: Richard Purdie <rpurdie@openedhand.com>
 *
 *  also based on the patch '[PATCH] 2.5.59 morse code panics' posted
 *  in the LKML by Tomas Szepe at Thu, 30 Jan 2003
 *	Copyright (C) 2002 Andrew Rodland <arodland@noln.com>
 *	Copyright (C) 2003 Tomas Szepe <szepe@pinerecords.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 */

#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/jiffies.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/spinlock.h>
#include <linux/device.h>
#include <linux/sysdev.h>
#include <linux/timer.h>
#include <linux/ctype.h>
#include <linux/leds.h>
#include <linux/slab.h>

#include "leds.h"

#define MORSE_DELAY_BASE	(HZ/2)

#define MORSE_STATE_BLINK_START	0
#define MORSE_STATE_BLINK_STOP	1

#define MORSE_DIT_LEN	1
#define MORSE_DAH_LEN	3
#define MORSE_SPACE_LEN	7

struct morse_trig_data {
	unsigned long delay;
	char *msg;

	unsigned char morse;
	unsigned char state;
	char *msgpos;
	struct timer_list timer;
};

const unsigned char morsetable[] = {
	0122, 0, 0310, 0, 0, 0163,				/* "#$%&' */
	055, 0155, 0, 0, 0163, 0141, 0152, 0051, 		/* ()*+,-./ */
	077, 076, 074, 070, 060, 040, 041, 043, 047, 057,	/* 0-9 */
	0107, 0125, 0, 0061, 0, 0114, 0, 			/* :;<=>?@ */
	006, 021, 025, 011, 002, 024, 013, 020, 004,		/* A-I */
	036, 015, 022, 007, 005, 017, 026, 033, 012,		/* J-R */
	010, 003, 014, 030, 016, 031, 035, 023,			/* S-Z */
	0, 0, 0, 0, 0154					/* [\]^_ */
};

static inline unsigned char tomorse(char c) {
	if (c >= 'a' && c <= 'z')
		c = c - 'a' + 'A';
	if (c >= '"' && c <= '_') {
		return morsetable[c - '"'];
	} else
		return 0;
}

static inline unsigned long dit_len(struct morse_trig_data *morse_data)
{
	return MORSE_DIT_LEN*morse_data->delay;
}

static inline unsigned long dah_len(struct morse_trig_data *morse_data)
{
	return MORSE_DAH_LEN*morse_data->delay;
}

static inline unsigned long space_len(struct morse_trig_data *morse_data)
{
	return MORSE_SPACE_LEN*morse_data->delay;
}

static void morse_timer_function(unsigned long data)
{
	struct led_classdev *led_cdev = (struct led_classdev *)data;
	struct morse_trig_data *morse_data = led_cdev->trigger_data;
	unsigned long brightness = LED_OFF;
	unsigned long delay = 0;

	if (!morse_data->msg)
		goto set_led;

	switch (morse_data->state) {
	case MORSE_STATE_BLINK_START:
		/* Starting a new blink.  We have a valid code in morse. */
		delay = (morse_data->morse & 001) ? dah_len(morse_data):
			dit_len(morse_data);
		brightness = LED_FULL;
		morse_data->state = MORSE_STATE_BLINK_STOP;
		morse_data->morse >>= 1;
		break;
	case MORSE_STATE_BLINK_STOP:
		/* Coming off of a blink. */
		morse_data->state = MORSE_STATE_BLINK_START;

		if (morse_data->morse > 1) {
			/* Not done yet, just a one-dit pause. */
			delay = dit_len(morse_data);
			break;
		}

		/* Get a new char, figure out how much space. */
		/* First time through */
		if (!morse_data->msgpos)
			morse_data->msgpos = (char *)morse_data->msg;

		if (!*morse_data->msgpos) {
			/* Repeating */
			morse_data->msgpos = (char *)morse_data->msg;
			delay = space_len(morse_data);
		} else {
			/* Inter-letter space */
			delay = dah_len(morse_data);
		}

		if (!(morse_data->morse = tomorse(*morse_data->msgpos))) {
			delay = space_len(morse_data);
			/* And get us back here */
			morse_data->state = MORSE_STATE_BLINK_STOP;
		}
		morse_data->msgpos++;
		break;
	}

	mod_timer(&morse_data->timer, jiffies + msecs_to_jiffies(delay));

set_led:
	led_set_brightness(led_cdev, brightness);
}

static ssize_t _morse_delay_show(struct led_classdev *led_cdev, char *buf)
{
	struct morse_trig_data *morse_data = led_cdev->trigger_data;

	sprintf(buf, "%lu\n", morse_data->delay);

	return strlen(buf) + 1;
}

static ssize_t _morse_delay_store(struct led_classdev *led_cdev,
		const char *buf, size_t size)
{
	struct morse_trig_data *morse_data = led_cdev->trigger_data;
	char *after;
	unsigned long state = simple_strtoul(buf, &after, 10);
	size_t count = after - buf;
	int ret = -EINVAL;

	if (*after && isspace(*after))
		count++;

	if (count == size) {
		morse_data->delay = state;
		mod_timer(&morse_data->timer, jiffies + 1);
		ret = count;
	}

	return ret;
}

static ssize_t _morse_msg_show(struct led_classdev *led_cdev, char *buf)
{
	struct morse_trig_data *morse_data = led_cdev->trigger_data;

	if (!morse_data->msg)
		sprintf(buf, "<none>\n");
	else
		sprintf(buf, "%s\n", morse_data->msg);

	return strlen(buf) + 1;
}

static ssize_t _morse_msg_store(struct led_classdev *led_cdev,
		const char *buf, size_t size)
{
	struct morse_trig_data *morse_data = led_cdev->trigger_data;
	char *m;

	m = kmalloc(size, GFP_KERNEL);
	if (!m)
		return -ENOMEM;

	memcpy(m,buf,size);
	m[size]='\0';

	if (morse_data->msg)
		kfree(morse_data->msg);

	morse_data->msg = m;
	morse_data->msgpos = NULL;
	morse_data->state = MORSE_STATE_BLINK_STOP;

	mod_timer(&morse_data->timer, jiffies + 1);

	return size;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,23)
static ssize_t morse_delay_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct led_classdev *led_cdev = dev_get_drvdata(dev);

	return _morse_delay_show(led_cdev, buf);
}

static ssize_t morse_delay_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	struct led_classdev *led_cdev = dev_get_drvdata(dev);

	return _morse_delay_store(led_cdev, buf, size);
}

static ssize_t morse_msg_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct led_classdev *led_cdev = dev_get_drvdata(dev);

	return _morse_msg_show(led_cdev, buf);
}

static ssize_t morse_msg_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	struct led_classdev *led_cdev = dev_get_drvdata(dev);

	return _morse_msg_store(led_cdev, buf, size);
}

static DEVICE_ATTR(delay, 0644, morse_delay_show, morse_delay_store);
static DEVICE_ATTR(message, 0644, morse_msg_show, morse_msg_store);

#define led_device_create_file(leddev, attr) \
	device_create_file(leddev->dev, &dev_attr_ ## attr)
#define led_device_remove_file(leddev, attr) \
	device_remove_file(leddev->dev, &dev_attr_ ## attr)

#else
static ssize_t morse_delay_show(struct class_device *dev, char *buf)
{
	struct led_classdev *led_cdev = class_get_devdata(dev);

	return _morse_delay_show(led_cdev, buf);
}

static ssize_t morse_delay_store(struct class_device *dev, const char *buf,
		size_t size)
{
	struct led_classdev *led_cdev = class_get_devdata(dev);

	return _morse_delay_store(led_cdev, buf, size);
}

static ssize_t morse_msg_show(struct class_device *dev, char *buf)
{
	struct led_classdev *led_cdev = class_get_devdata(dev);

	return _morse_msg_show(led_cdev, buf);
}

static ssize_t morse_msg_store(struct class_device *dev, const char *buf,
				size_t size)
{
	struct led_classdev *led_cdev = class_get_devdata(dev);

	return _morse_msg_store(led_cdev, buf, size);
}

static CLASS_DEVICE_ATTR(delay, 0644, morse_delay_show, morse_delay_store);
static CLASS_DEVICE_ATTR(message, 0644, morse_msg_show, morse_msg_store);

#define led_device_create_file(leddev, attr) \
	class_device_create_file(leddev->class_dev, &class_device_attr_ ## attr)
#define led_device_remove_file(leddev, attr) \
	class_device_remove_file(leddev->class_dev, &class_device_attr_ ## attr)

#endif

static void morse_trig_activate(struct led_classdev *led_cdev)
{
	struct morse_trig_data *morse_data;
	int rc;

	morse_data = kzalloc(sizeof(*morse_data), GFP_KERNEL);
	if (!morse_data)
		return;

	morse_data->delay = MORSE_DELAY_BASE;
	init_timer(&morse_data->timer);
	morse_data->timer.function = morse_timer_function;
	morse_data->timer.data = (unsigned long)led_cdev;

	rc = led_device_create_file(led_cdev, delay);
	if (rc) goto err;

	rc = led_device_create_file(led_cdev, message);
	if (rc) goto err_delay;

	led_cdev->trigger_data = morse_data;

	return;

err_delay:
	led_device_remove_file(led_cdev, delay);
err:
	kfree(morse_data);
}

static void morse_trig_deactivate(struct led_classdev *led_cdev)
{
	struct morse_trig_data *morse_data = led_cdev->trigger_data;

	if (!morse_data)
		return;

	led_device_remove_file(led_cdev, message);
	led_device_remove_file(led_cdev, delay);

	del_timer_sync(&morse_data->timer);
	if (morse_data->msg)
		kfree(morse_data->msg);

	kfree(morse_data);
}

static struct led_trigger morse_led_trigger = {
	.name		= "morse",
	.activate	= morse_trig_activate,
	.deactivate	= morse_trig_deactivate,
};

static int __init morse_trig_init(void)
{
	return led_trigger_register(&morse_led_trigger);
}

static void __exit morse_trig_exit(void)
{
	led_trigger_unregister(&morse_led_trigger);
}

module_init(morse_trig_init);
module_exit(morse_trig_exit);

MODULE_AUTHOR("Gabor Juhos <juhosg at openwrt.org>");
MODULE_DESCRIPTION("Morse LED trigger");
MODULE_LICENSE("GPL");

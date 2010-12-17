/*
 * LED USB device Trigger
 *
 * Toggles the LED to reflect the presence and activity of an USB device
 * Copyright (C) Gabor Juhos <juhosg@openwrt.org>
 *
 * derived from ledtrig-netdev.c:
 *	Copyright 2007 Oliver Jowett <oliver@opencloud.com>
 *
 * ledtrig-netdev.c derived from ledtrig-timer.c:
 *	Copyright 2005-2006 Openedhand Ltd.
 *	Author: Richard Purdie <rpurdie@openedhand.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#include <linux/module.h>
#include <linux/jiffies.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/spinlock.h>
#include <linux/device.h>
#include <linux/sysdev.h>
#include <linux/timer.h>
#include <linux/ctype.h>
#include <linux/slab.h>
#include <linux/leds.h>
#include <linux/usb.h>

#include "leds.h"

#define DEV_BUS_ID_SIZE		32

/*
 * Configurable sysfs attributes:
 *
 * device_name - name of the USB device to monitor
 * activity_interval - duration of LED blink, in milliseconds
 */

struct usbdev_trig_data {
	rwlock_t lock;

	struct timer_list timer;
	struct notifier_block notifier;

	struct led_classdev *led_cdev;
	struct usb_device *usb_dev;

	char device_name[DEV_BUS_ID_SIZE];
	unsigned interval;
	int last_urbnum;
};

static void usbdev_trig_update_state(struct usbdev_trig_data *td)
{
	if (td->usb_dev)
		led_set_brightness(td->led_cdev, LED_FULL);
	else
		led_set_brightness(td->led_cdev, LED_OFF);

	if (td->interval && td->usb_dev)
		mod_timer(&td->timer, jiffies + td->interval);
	else
		del_timer(&td->timer);
}

static ssize_t usbdev_trig_name_show(struct device *dev,
				     struct device_attribute *attr,
				     char *buf)
{
	struct led_classdev *led_cdev = dev_get_drvdata(dev);
	struct usbdev_trig_data *td = led_cdev->trigger_data;

	read_lock(&td->lock);
	sprintf(buf, "%s\n", td->device_name);
	read_unlock(&td->lock);

	return strlen(buf) + 1;
}

static ssize_t usbdev_trig_name_store(struct device *dev,
				      struct device_attribute *attr,
				      const char *buf,
				      size_t size)
{
	struct led_classdev *led_cdev = dev_get_drvdata(dev);
	struct usbdev_trig_data *td = led_cdev->trigger_data;

	if (size < 0 || size >= DEV_BUS_ID_SIZE)
		return -EINVAL;

	write_lock(&td->lock);

	strcpy(td->device_name, buf);
	if (size > 0 && td->device_name[size - 1] == '\n')
		td->device_name[size - 1] = 0;

	if (td->device_name[0] != 0) {
		struct usb_device *usb_dev;

		/* check for existing device to update from */
		usb_dev = usb_find_device_by_name(td->device_name);
		if (usb_dev) {
			if (td->usb_dev)
				usb_put_dev(td->usb_dev);

			td->usb_dev = usb_dev;
			td->last_urbnum = atomic_read(&usb_dev->urbnum);
		}

		/* updates LEDs, may start timers */
		usbdev_trig_update_state(td);
	}

	write_unlock(&td->lock);
	return size;
}

static DEVICE_ATTR(device_name, 0644, usbdev_trig_name_show,
		   usbdev_trig_name_store);

static ssize_t usbdev_trig_interval_show(struct device *dev,
				 	 struct device_attribute *attr,
					 char *buf)
{
	struct led_classdev *led_cdev = dev_get_drvdata(dev);
	struct usbdev_trig_data *td = led_cdev->trigger_data;

	read_lock(&td->lock);
	sprintf(buf, "%u\n", jiffies_to_msecs(td->interval));
	read_unlock(&td->lock);

	return strlen(buf) + 1;
}

static ssize_t usbdev_trig_interval_store(struct device *dev,
					  struct device_attribute *attr,
					  const char *buf,
					  size_t size)
{
	struct led_classdev *led_cdev = dev_get_drvdata(dev);
	struct usbdev_trig_data *td = led_cdev->trigger_data;
	int ret = -EINVAL;
	char *after;
	unsigned long value = simple_strtoul(buf, &after, 10);
	size_t count = after - buf;

	if (*after && isspace(*after))
		count++;

	if (count == size && value <= 10000) {
		write_lock(&td->lock);
		td->interval = msecs_to_jiffies(value);
		usbdev_trig_update_state(td); /* resets timer */
		write_unlock(&td->lock);
		ret = count;
	}

	return ret;
}

static DEVICE_ATTR(activity_interval, 0644, usbdev_trig_interval_show,
		   usbdev_trig_interval_store);

static int usbdev_trig_notify(struct notifier_block *nb,
			      unsigned long evt,
			      void *data)
{
	struct usb_device *usb_dev;
	struct usbdev_trig_data *td;

	if (evt != USB_DEVICE_ADD && evt != USB_DEVICE_REMOVE)
		return NOTIFY_DONE;

	usb_dev = data;
	td = container_of(nb, struct usbdev_trig_data, notifier);

	write_lock(&td->lock);

	if (strcmp(dev_name(&usb_dev->dev), td->device_name))
		goto done;

	if (evt == USB_DEVICE_ADD) {
		usb_get_dev(usb_dev);
		if (td->usb_dev != NULL)
			usb_put_dev(td->usb_dev);
		td->usb_dev = usb_dev;
		td->last_urbnum = atomic_read(&usb_dev->urbnum);
	} else if (evt == USB_DEVICE_REMOVE) {
		if (td->usb_dev != NULL) {
			usb_put_dev(td->usb_dev);
			td->usb_dev = NULL;
		}
	}

	usbdev_trig_update_state(td);

done:
	write_unlock(&td->lock);
	return NOTIFY_DONE;
}

/* here's the real work! */
static void usbdev_trig_timer(unsigned long arg)
{
	struct usbdev_trig_data *td = (struct usbdev_trig_data *)arg;
	int new_urbnum;

	write_lock(&td->lock);

	if (!td->usb_dev || td->interval == 0) {
		/*
		 * we don't need to do timer work, just reflect device presence
		 */
		if (td->usb_dev)
			led_set_brightness(td->led_cdev, LED_FULL);
		else
			led_set_brightness(td->led_cdev, LED_OFF);

		goto no_restart;
	}

	if (td->interval)
		new_urbnum = atomic_read(&td->usb_dev->urbnum);
	else
		new_urbnum = 0;

	if (td->usb_dev) {
		/*
		 * Base state is ON (device is present). If there's no device,
		 * we don't get this far and the LED is off.
		 * OFF -> ON always
		 * ON -> OFF on activity
		 */
		if (td->led_cdev->brightness == LED_OFF)
			led_set_brightness(td->led_cdev, LED_FULL);
		else if (td->last_urbnum != new_urbnum)
			led_set_brightness(td->led_cdev, LED_OFF);
	} else {
		/*
		 * base state is OFF
		 * ON -> OFF always
		 * OFF -> ON on activity
		 */
		if (td->led_cdev->brightness == LED_FULL)
			led_set_brightness(td->led_cdev, LED_OFF);
		else if (td->last_urbnum != new_urbnum)
			led_set_brightness(td->led_cdev, LED_FULL);
	}

	td->last_urbnum = new_urbnum;
	mod_timer(&td->timer, jiffies + td->interval);

no_restart:
	write_unlock(&td->lock);
}

static void usbdev_trig_activate(struct led_classdev *led_cdev)
{
	struct usbdev_trig_data *td;
	int rc;

	td = kzalloc(sizeof(struct usbdev_trig_data), GFP_KERNEL);
	if (!td)
		return;

	rwlock_init(&td->lock);

	td->notifier.notifier_call = usbdev_trig_notify;
	td->notifier.priority = 10;

	setup_timer(&td->timer, usbdev_trig_timer, (unsigned long) td);

	td->led_cdev = led_cdev;
	td->interval = msecs_to_jiffies(50);

	led_cdev->trigger_data = td;

	rc = device_create_file(led_cdev->dev, &dev_attr_device_name);
	if (rc)
		goto err_out;

	rc = device_create_file(led_cdev->dev, &dev_attr_activity_interval);
	if (rc)
		goto err_out_device_name;

	usb_register_notify(&td->notifier);
	return;

err_out_device_name:
	device_remove_file(led_cdev->dev, &dev_attr_device_name);
err_out:
	led_cdev->trigger_data = NULL;
	kfree(td);
}

static void usbdev_trig_deactivate(struct led_classdev *led_cdev)
{
	struct usbdev_trig_data *td = led_cdev->trigger_data;

	if (td) {
		usb_unregister_notify(&td->notifier);

		device_remove_file(led_cdev->dev, &dev_attr_device_name);
		device_remove_file(led_cdev->dev, &dev_attr_activity_interval);

		write_lock(&td->lock);

		if (td->usb_dev) {
			usb_put_dev(td->usb_dev);
			td->usb_dev = NULL;
		}

		write_unlock(&td->lock);

		del_timer_sync(&td->timer);

		kfree(td);
	}
}

static struct led_trigger usbdev_led_trigger = {
	.name		= "usbdev",
	.activate	= usbdev_trig_activate,
	.deactivate	= usbdev_trig_deactivate,
};

static int __init usbdev_trig_init(void)
{
	return led_trigger_register(&usbdev_led_trigger);
}

static void __exit usbdev_trig_exit(void)
{
	led_trigger_unregister(&usbdev_led_trigger);
}

module_init(usbdev_trig_init);
module_exit(usbdev_trig_exit);

MODULE_AUTHOR("Gabor Juhos <juhosg@openwrt.org>");
MODULE_DESCRIPTION("USB device LED trigger");
MODULE_LICENSE("GPL v2");

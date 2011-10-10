/*
 *  EASY98000 CPLD LED driver
 *
 *  Copyright (C) 2010 Ralph Hempel <ralph.hempel@lantiq.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2  as published
 *  by the Free Software Foundation.
 *
 */

#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/errno.h>
#include <linux/leds.h>
#include <linux/slab.h>

#include "dev-leds-easy98000-cpld.h"

const char *led_name[8] = {
	"ge0_act",
	"ge0_link",
	"ge1_act",
	"ge1_link",
	"fe2_act",
	"fe2_link",
	"fe3_act",
	"fe3_link"
};

#define cpld_base7			((u16 *)(KSEG1 | 0x17c0000c))
#define cpld_base8			((u16 *)(KSEG1 | 0x17c00012))

#define ltq_r16(reg)			__raw_readw(reg)
#define ltq_w16(val, reg)		__raw_writew(val, reg)

struct cpld_led_dev {
	struct led_classdev	cdev;
	u8			mask;
	u16			*base;
};

struct cpld_led_drvdata {
	struct cpld_led_dev	*led_devs;
	int			num_leds;
};

void led_set(u8 mask, u16 *base)
{
	ltq_w16(ltq_r16(base) | mask, base);
}

void led_clear(u8 mask, u16 *base)
{
	ltq_w16(ltq_r16(base) & (~mask), base);
}

void led_blink_clear(u8 mask, u16 *base)
{
	led_clear(mask, base);
}

static void led_brightness(struct led_classdev *led_cdev,
			       enum led_brightness value)
{
	struct cpld_led_dev *led_dev =
	    container_of(led_cdev, struct cpld_led_dev, cdev);

	if (value)
		led_set(led_dev->mask, led_dev->base);
	else
		led_clear(led_dev->mask, led_dev->base);
}

static int led_probe(struct platform_device *pdev)
{
	int i;
	char name[32];
	struct cpld_led_drvdata *drvdata;
	int ret = 0;

	drvdata = kzalloc(sizeof(struct cpld_led_drvdata) +
			  sizeof(struct cpld_led_dev) * MAX_LED,
			  GFP_KERNEL);
	if (!drvdata)
		return -ENOMEM;

	drvdata->led_devs = (struct cpld_led_dev *) &drvdata[1];

	for (i = 0; i < MAX_LED; i++) {
		struct cpld_led_dev *led_dev = &drvdata->led_devs[i];
		led_dev->cdev.brightness_set = led_brightness;
		led_dev->cdev.default_trigger = NULL;
		led_dev->mask = 1 << (i % 8);
		if(i < 8) {
			sprintf(name, "easy98000-cpld:%s", led_name[i]);
			led_dev->base = cpld_base8;
		} else {
			sprintf(name, "easy98000-cpld:red:%d", i-8);
			led_dev->base = cpld_base7;
		}
		led_dev->cdev.name = name;
		ret = led_classdev_register(&pdev->dev, &led_dev->cdev);
		if (ret)
			goto err;
	}
	platform_set_drvdata(pdev, drvdata);
	return 0;

err:
	printk("led_probe: 3\n");
	for (i = i - 1; i >= 0; i--)
		led_classdev_unregister(&drvdata->led_devs[i].cdev);

	kfree(drvdata);
	return ret;
}

static int led_remove(struct platform_device *pdev)
{
	int i;
	struct cpld_led_drvdata *drvdata = platform_get_drvdata(pdev);
	for (i = 0; i < MAX_LED; i++)
		led_classdev_unregister(&drvdata->led_devs[i].cdev);
	kfree(drvdata);
	return 0;
}

static struct platform_driver led_driver = {
	.probe = led_probe,
	.remove = __devexit_p(led_remove),
	.driver = {
		   .name = LED_NAME,
		   .owner = THIS_MODULE,
		   },
};

int __init easy98000_cpld_led_init(void)
{
	pr_info(LED_DESC ", Version " LED_VERSION
		" (c) Copyright 2011, Lantiq Deutschland GmbH\n");
	return platform_driver_register(&led_driver);
}

void __exit easy98000_cpld_led_exit(void)
{
	platform_driver_unregister(&led_driver);
}

module_init(easy98000_cpld_led_init);
module_exit(easy98000_cpld_led_exit);

MODULE_DESCRIPTION(LED_NAME);
MODULE_DESCRIPTION(LED_DESC);
MODULE_AUTHOR("Ralph Hempel <ralph.hempel@lantiq.com>");
MODULE_LICENSE("GPL v2");


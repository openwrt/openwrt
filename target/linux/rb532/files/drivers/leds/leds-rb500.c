/*
 * linux/drivers/leds/leds-rb500.c
 *
 * Copyright (C) 2006
 *     Twente Institute for Wireless and Mobile Communications BV
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details (see file GPLv2).
 *
 * Author: Tjalling Hattink <tjalling.hattink@ti-wmc.nl>
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/leds.h>
#include <asm/rc32434/rb.h>

static void rb500led_amber_set(struct led_classdev *led_cdev, enum led_brightness value)
{
	if (value)
		changeLatchU5(LO_ULED, 0);
	else
		changeLatchU5(0, LO_ULED);
}

static struct led_classdev rb500_amber_led = {
	.name			= "rb500led:amber",
	.default_trigger	= "ide-disk",
	.brightness_set		= rb500led_amber_set,
};

static int rb500led_probe(struct platform_device *pdev)
{
	int ret;

	changeLatchU5(0, LO_ULED);

	ret = led_classdev_register(&pdev->dev, &rb500_amber_led);

	return ret;
}

static int rb500led_remove(struct platform_device *pdev)
{
	led_classdev_unregister(&rb500_amber_led);

	return 0;
}

static struct platform_driver rb500led_driver = {
	.probe		= rb500led_probe,
	.remove		= rb500led_remove,
	.driver		= {
		.name		= "rb500-led",
	},
};

static int __init rb500led_init(void)
{
	return platform_driver_register(&rb500led_driver);
}

static void __exit rb500led_exit(void)
{
 	platform_driver_unregister(&rb500led_driver);
}

module_init(rb500led_init);
module_exit(rb500led_exit);

MODULE_AUTHOR("tjalling.hattink@ti-wmc.nl");
MODULE_DESCRIPTION("Mikrotik RB500 LED driver");
MODULE_LICENSE("GPL");

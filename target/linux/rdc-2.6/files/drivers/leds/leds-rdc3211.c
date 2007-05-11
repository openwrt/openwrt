/*
 * LED driver for RDC3211 boards
 *
 * Copyright 2007 Florian Fainelli <florian@openwrt.org>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/leds.h>
#include <linux/err.h>

#include <asm/io.h>

#define LED_VAL	0x8000384C    // the data ofset of gpio 0~30

static struct platform_device *pdev;

static void rdc3211_led_set(struct led_classdev *led_cdev, enum led_brightness brightness)
{
	unsigned long ul_ledstat = 0xffffffff;
	unsigned long led_bit = 1 << (led_cdev->flags);

	if (brightness)
		ul_ledstat &= ~led_bit;
	else
		ul_ledstat|=  led_bit;

	outl(LED_VAL, 0xcf8);
	outl(ul_ledstat, 0xcfc);
}

static struct led_classdev rdc3211_power_led = {
	.name = "rdc3211:power",
	.flags = 15,
	.brightness_set = rdc3211_led_set,
};

static struct led_classdev rdc3211_dmz_led = {
	.name = "rdc3211:dmz",
	.flags = 16,
	.brightness_set = rdc3211_led_set,
};

static int rdc3211_leds_probe(struct platform_device *pdev)
{
	int ret;

	ret = led_classdev_register(&pdev->dev, &rdc3211_power_led);
	if (ret < 0)
		return ret;

	ret = led_classdev_register(&pdev->dev, &rdc3211_dmz_led);
	if (ret < 0)
		led_classdev_unregister(&rdc3211_power_led);

	return ret;
}

static int rdc3211_leds_remove(struct platform_device *pdev)
{
	led_classdev_unregister(&rdc3211_power_led);
	led_classdev_unregister(&rdc3211_dmz_led);
	return 0;
}

static struct platform_driver rdc3211_leds_driver = {
	.probe = rdc3211_leds_probe,
	.remove = rdc3211_leds_remove,
	.driver = {
		.name = "rdc3211-leds",
	}
};

static int __init rdc3211_leds_init(void)
{
	int ret;

	ret = platform_driver_register(&rdc3211_leds_driver);
	if (ret < 0)
		goto out;

	pdev = platform_device_register_simple("rdc3211-leds", -1, NULL, 0);
	if (IS_ERR(pdev)) {
		ret = PTR_ERR(pdev);
		platform_driver_unregister(&rdc3211_leds_driver);
		goto out;
	}

out:
	return ret;
}
		
static void __exit rdc3211_leds_exit(void)
{
	platform_driver_unregister(&rdc3211_leds_driver);
}

module_init(rdc3211_leds_init);
module_exit(rdc3211_leds_exit);
		
MODULE_AUTHOR("Florian Fainelli <florian@openwrt.org>");
MODULE_DESCRIPTION("RDC3211 LED driver");
MODULE_LICENSE("GPL");

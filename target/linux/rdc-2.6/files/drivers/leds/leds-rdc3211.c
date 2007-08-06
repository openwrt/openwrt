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
#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/leds.h>
#include <linux/err.h>

#include <asm/gpio.h>

/* This is just for testing purpose */
int gpio;
module_param(gpio, int, 0444);
MODULE_PARM_DESC(gpio, " GPIO line");

static void rdc321x_led_set(struct led_classdev *led_cdev, enum led_brightness brightness)
{
	gpio_set_value(gpio, brightness ? 1 : 0);
}

/* The DMZ led is at GPIO line 1 */
static struct led_classdev rdc321x_dmz_led = {
	.name = "rdc321x:dmz",
	.brightness_set = rdc321x_led_set,
};

static int rdc321x_leds_probe(struct platform_device *pdev)
{
	return led_classdev_register(&pdev->dev, &rdc321x_dmz_led);
}

static int rdc321x_leds_remove(struct platform_device *pdev)
{
	led_classdev_unregister(&rdc321x_dmz_led);
	return 0;
}

static struct platform_driver rdc321x_leds_driver = {
	.probe = rdc321x_leds_probe,
	.remove = rdc321x_leds_remove,
	.driver = {
		.name = "rdc321x-leds",
		.owner = THIS_MODULE,
	}
};

static int __init rdc321x_leds_init(void)
{
	int ret;

	ret = platform_driver_register(&rdc321x_leds_driver);

	return ret;
}
		
static void __exit rdc321x_leds_exit(void)
{
	platform_driver_unregister(&rdc321x_leds_driver);
}

module_init(rdc321x_leds_init);
module_exit(rdc321x_leds_exit);
		
MODULE_AUTHOR("Florian Fainelli <florian@openwrt.org>");
MODULE_DESCRIPTION("RDC321x LED driver");
MODULE_LICENSE("GPL");

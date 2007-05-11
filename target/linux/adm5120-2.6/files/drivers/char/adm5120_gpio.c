/*
 *	ADM5120 LED (GPIO) driver
 *
 *	Copyright (C) Jeroen Vreeken (pe1rxq@amsat.org), 2005
 *	Copyright (C) OpenWrt.org, Florian Fainelli <florian@openwrt.org>, 2007
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License version 2 as
 *	published by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/leds.h>

#define GPIO_IO        ((unsigned long *)0xb20000b8)

static void adm5120_led_set(struct led_classdev *led_cdev, enum led_brightness brightness)
{
	if (brightness)
		*GPIO_IO=(*GPIO_IO & 0x00ffffff) | (brightness << 24);
	else
		*GPIO_IO=(*GPIO_IO & 0x00ffffff) | (0 << 24);
}

static struct led_classdev adm5120_gpio_led = {
	.name 		= "adm5120:led",
	.brightness_set = adm5120_led_set,
};

static int __init adm5120_led_init(void)
{
	int ret = led_classdev_register(NULL, &adm5120_gpio_led);

	if (ret < 0)
		printk(KERN_WARNING "adm5120: unable to register LED device\n");

	return ret;
}

static void __exit adm5120_led_exit(void)
{
	led_classdev_unregister(&adm5120_gpio_led);
}

module_init(adm5120_led_init);
module_exit(adm5120_led_exit);

MODULE_DESCRIPTION("ADM5120 LED driver");
MODULE_AUTHOR("Jeroen Vreeken, OpenWrt.org");
MODULE_LICENSE("GPL");

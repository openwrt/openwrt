/*
 * LED driver for MTX-1 boards
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
#include <asm/mach-au1x00/au1000.h>

static struct platform_device *pdev;

static void mtx1_green_led_set(struct led_classdev *led_cdev, enum led_brightness brightness)
{
	/* The power LED cannot be controlled the same way as for the Status LED */
	if (brightness) {
        	au_writel( 0x18000800, GPIO2_OUTPUT );
	} else {
		au_writel( 0x18000000, GPIO2_OUTPUT);
	}
}

static void mtx1_red_led_set(struct led_classdev *led_cdev, enum led_brightness brightness)
{
	/* We store GPIO address (originally address - 200) in the "flags" field*/
	unsigned long pinmask = 1 << led_cdev->flags; 
	if (brightness) {
		au_writel((pinmask << 16) | pinmask, GPIO2_OUTPUT); 
	} else { 
		au_writel((pinmask << 16) | 0, GPIO2_OUTPUT);
	}
}

static struct led_classdev mtx1_green_led = {
	.name = "mtx1:green",
	.brightness_set = mtx1_green_led_set,
};

static struct led_classdev mtx1_red_led = {
	.name = "mtx1:red",
	.flags = 12,
	.brightness_set = mtx1_red_led_set,
	.default_trigger = "ide-disk",
};

static int mtx1_leds_probe(struct platform_device *pdev)
{
	int ret;

	ret = led_classdev_register(&pdev->dev, &mtx1_green_led);
	if (ret < 0)
		goto out;

	ret = led_classdev_register(&pdev->dev, &mtx1_red_led);
	if (ret < 0)
		led_classdev_unregister(&mtx1_green_led);

out:
	return ret;
}

static int mtx1_leds_remove(struct platform_device *pdev)
{
	led_classdev_unregister(&mtx1_green_led);
	led_classdev_unregister(&mtx1_red_led);
	return 0;
}

static struct platform_driver mtx1_leds_driver = {
	.probe = mtx1_leds_probe,
	.remove = mtx1_leds_remove,
	.driver = {
		.name = "mtx1-leds",
	}
};

static int __init mtx1_leds_init(void)
{
	int ret;

        ret = platform_driver_register(&mtx1_leds_driver);
        if (ret < 0)
                goto out;

        pdev = platform_device_register_simple("mtx1-leds", -1, NULL, 0);
        if (IS_ERR(pdev)) {
                ret = PTR_ERR(pdev);
                platform_driver_unregister(&mtx1_leds_driver);
                goto out;
        }

out:
        return ret;

}
		
static void __exit mtx1_leds_exit(void)
{
	platform_device_unregister(pdev);
	platform_driver_unregister(&mtx1_leds_driver);
}

module_init(mtx1_leds_init);
module_exit(mtx1_leds_exit);
		
MODULE_AUTHOR("Florian Fainelli <florian@openwrt.org>");
MODULE_DESCRIPTION("MTX-1 LED driver");
MODULE_LICENSE("GPL");

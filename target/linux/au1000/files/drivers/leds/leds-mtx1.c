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
#include <asm/gpio.h>

static struct platform_device *pdev;

static void mtx1_led_set(struct led_classdev *led_cdev, enum led_brightness brightness)
{
	if (!strcmp("mtx1:green", led_cdev->name))
		gpio_set_value(211, brightness ? 1 : 0);
	else
		gpio_set_value(212, brightness ? 1 : 0);
}

static struct led_classdev mtx1_green_led = {
	.name = "mtx1:green",
	.brightness_set = mtx1_led_set,
};

static struct led_classdev mtx1_red_led = {
	.name = "mtx1:red",
	.brightness_set = mtx1_led_set,
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

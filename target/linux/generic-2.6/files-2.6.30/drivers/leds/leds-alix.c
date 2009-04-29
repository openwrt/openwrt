/*
 * LEDs driver for PCEngines ALIX 2/3 series
 *
 * Copyright (C) 2007 Petr Liebman
 *
 * Based on leds-wrap.c
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/leds.h>
#include <linux/err.h>
#include <asm/io.h>

#define DRVNAME "alix-led"

#define ALIX_LED1_PORT		(0x6100)
#define ALIX_LED1_ON		(1<<22)
#define ALIX_LED1_OFF		(1<<6)

#define ALIX_LED2_PORT		(0x6180)
#define ALIX_LED2_ON		(1<<25)
#define ALIX_LED2_OFF		(1<<9)

#define ALIX_LED3_PORT		(0x6180)
#define ALIX_LED3_ON		(1<<27)
#define ALIX_LED3_OFF		(1<<11)


static struct platform_device *pdev;

static void alix_led_set_1(struct led_classdev *led_cdev,
		enum led_brightness value)
{
	if (value)
		outl(ALIX_LED1_ON, ALIX_LED1_PORT);
	else
		outl(ALIX_LED1_OFF, ALIX_LED1_PORT);
}

static void alix_led_set_2(struct led_classdev *led_cdev,
		enum led_brightness value)
{
	if (value)
		outl(ALIX_LED2_ON, ALIX_LED2_PORT);
	else
		outl(ALIX_LED2_OFF, ALIX_LED2_PORT);
}

static void alix_led_set_3(struct led_classdev *led_cdev,
		enum led_brightness value)
{
	if (value)
		outl(ALIX_LED3_ON, ALIX_LED3_PORT);
	else
		outl(ALIX_LED3_OFF, ALIX_LED3_PORT);
}

static struct led_classdev alix_led_1 = {
	.name		= "alix:1",
	.brightness_set	= alix_led_set_1,
};

static struct led_classdev alix_led_2 = {
	.name		= "alix:2",
	.brightness_set	= alix_led_set_2,
};

static struct led_classdev alix_led_3 = {
	.name		= "alix:3",
	.brightness_set	= alix_led_set_3,
};


#ifdef CONFIG_PM
static int alix_led_suspend(struct platform_device *dev,
		pm_message_t state)
{
	led_classdev_suspend(&alix_led_1);
	led_classdev_suspend(&alix_led_2);
	led_classdev_suspend(&alix_led_3);
	return 0;
}

static int alix_led_resume(struct platform_device *dev)
{
	led_classdev_resume(&alix_led_1);
	led_classdev_resume(&alix_led_2);
	led_classdev_resume(&alix_led_3);
	return 0;
}
#else
#define alix_led_suspend NULL
#define alix_led_resume NULL
#endif

static int alix_led_probe(struct platform_device *pdev)
{
	int ret;

	ret = led_classdev_register(&pdev->dev, &alix_led_1);
	if (ret >= 0)
	{
		ret = led_classdev_register(&pdev->dev, &alix_led_2);
		if (ret >= 0)
		{
			ret = led_classdev_register(&pdev->dev, &alix_led_3);
			if (ret < 0)
				led_classdev_unregister(&alix_led_2);
		}
		if (ret < 0)
			led_classdev_unregister(&alix_led_1);
	}
	return ret;
}

static int alix_led_remove(struct platform_device *pdev)
{
	led_classdev_unregister(&alix_led_1);
	led_classdev_unregister(&alix_led_2);
	led_classdev_unregister(&alix_led_3);
	return 0;
}

static struct platform_driver alix_led_driver = {
	.probe		= alix_led_probe,
	.remove		= alix_led_remove,
	.suspend	= alix_led_suspend,
	.resume		= alix_led_resume,
	.driver		= {
		.name		= DRVNAME,
		.owner		= THIS_MODULE,
	},
};

static int __init alix_led_init(void)
{
	int ret;

	ret = platform_driver_register(&alix_led_driver);
	if (ret < 0)
		goto out;

	pdev = platform_device_register_simple(DRVNAME, -1, NULL, 0);
	if (IS_ERR(pdev)) {
		ret = PTR_ERR(pdev);
		platform_driver_unregister(&alix_led_driver);
		goto out;
	}

out:
	return ret;
}

static void __exit alix_led_exit(void)
{
	platform_device_unregister(pdev);
	platform_driver_unregister(&alix_led_driver);
}

module_init(alix_led_init);
module_exit(alix_led_exit);

MODULE_AUTHOR("Petr Liebman");
MODULE_DESCRIPTION("PCEngines ALIX LED driver");
MODULE_LICENSE("GPL");


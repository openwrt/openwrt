/*
 * LED driver for the Openmoko GTA02 GSM phone
 *
 * (C) 2006-2008 by Openmoko, Inc.
 * Author: Harald Welte <laforge@openmoko.org>
 * All rights reserved.
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
#include <mach/hardware.h>
#include <asm/mach-types.h>
#include <mach/gta02.h>
#include <plat/regs-timer.h>

#define MAX_LEDS 3
#define COUNTER 256

struct gta02_led_priv
{
	spinlock_t lock;
	struct led_classdev cdev;
	unsigned int gpio;
};

struct gta02_led_bundle
{
	int num_leds;
	struct gta02_led_priv led[MAX_LEDS];
};

static inline struct gta02_led_priv *to_priv(struct led_classdev *led_cdev)
{
	return container_of(led_cdev, struct gta02_led_priv, cdev);
}

static inline struct gta02_led_bundle *to_bundle(struct led_classdev *led_cdev)
{
	return dev_get_drvdata(led_cdev->dev->parent);
}

static void gta02led_set(struct led_classdev *led_cdev,
			 enum led_brightness value)
{
	unsigned long flags;
	struct gta02_led_priv *lp = to_priv(led_cdev);

	spin_lock_irqsave(&lp->lock, flags);
	s3c2410_gpio_setpin(lp->gpio, value ? 1 : 0);
	spin_unlock_irqrestore(&lp->lock, flags);
}

#ifdef CONFIG_PM
static int gta02led_suspend(struct platform_device *pdev, pm_message_t state)
{
	struct gta02_led_bundle *bundle = platform_get_drvdata(pdev);
	int i;

	for (i = 0; i < bundle->num_leds; i++)
		led_classdev_suspend(&bundle->led[i].cdev);

	return 0;
}

static int gta02led_resume(struct platform_device *pdev)
{
	struct gta02_led_bundle *bundle = platform_get_drvdata(pdev);
	int i;

	for (i = 0; i < bundle->num_leds; i++)
		led_classdev_resume(&bundle->led[i].cdev);

	return 0;
}
#endif

static int __init gta02led_probe(struct platform_device *pdev)
{
	int i, rc;
	struct gta02_led_bundle *bundle;

	if (!machine_is_neo1973_gta02())
		return -EIO;

	bundle = kzalloc(sizeof(struct gta02_led_bundle), GFP_KERNEL);
	if (!bundle)
		return -ENOMEM;
	platform_set_drvdata(pdev, bundle);

	for (i = 0; i < pdev->num_resources; i++) {
		struct gta02_led_priv *lp;
		struct resource *r;

		if (i >= MAX_LEDS)
			break;

		r = platform_get_resource(pdev, 0, i);
		if (!r || !r->start || !r->name)
			continue;

		lp = &bundle->led[i];

		lp->gpio = r->start;
		lp->cdev.name = r->name;
		lp->cdev.brightness_set = gta02led_set;

		switch (lp->gpio) {
		case S3C2410_GPB0:
		case S3C2410_GPB1:
		case S3C2410_GPB2:
			s3c2410_gpio_cfgpin(lp->gpio, S3C2410_GPIO_OUTPUT);
			break;
		default:
			break;
		}

		spin_lock_init(&lp->lock);
		rc = led_classdev_register(&pdev->dev, &lp->cdev);
	}

	bundle->num_leds = i;

	return 0;
}

static int gta02led_remove(struct platform_device *pdev)
{
	struct gta02_led_bundle *bundle = platform_get_drvdata(pdev);
	int i;

	for (i = 0; i < bundle->num_leds; i++) {
		struct gta02_led_priv *lp = &bundle->led[i];
		gta02led_set(&lp->cdev, 0);
		led_classdev_unregister(&lp->cdev);
	}

	platform_set_drvdata(pdev, NULL);
	kfree(bundle);

	return 0;
}

static struct platform_driver gta02led_driver = {
	.probe		= gta02led_probe,
	.remove		= gta02led_remove,
#ifdef CONFIG_PM
	.suspend	= gta02led_suspend,
	.resume		= gta02led_resume,
#endif
	.driver		= {
		.name		= "gta02-led",
	},
};

static int __init gta02led_init(void)
{
	return platform_driver_register(&gta02led_driver);
}

static void __exit gta02led_exit(void)
{
	platform_driver_unregister(&gta02led_driver);
}

module_init(gta02led_init);
module_exit(gta02led_exit);

MODULE_AUTHOR("Harald Welte <laforge@openmoko.org>");
MODULE_DESCRIPTION("Openmoko GTA02 LED driver");
MODULE_LICENSE("GPL");

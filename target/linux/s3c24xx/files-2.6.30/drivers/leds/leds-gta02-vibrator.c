/*
 * LED driver for the vibrator of the Openmoko GTA01/GTA02 GSM Phones
 *
 * (C) 2006-2008 by Openmoko, Inc.
 * Author: Harald Welte <laforge@openmoko.org>
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Javi Roman <javiroman@kernel-labs.org>:
 *	Implement PWM support for GTA01Bv4 and later
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/leds.h>
#include <mach/hardware.h>
#include <asm/mach-types.h>
#include <plat/pwm.h>
#include <mach/regs-gpio.h>
#include <plat/regs-timer.h>
#include <linux/gta02-vibrator.h>

#define COUNTER 64

static struct gta02_vib_priv {
	struct led_classdev cdev;
	unsigned int gpio;
	spinlock_t lock;
	unsigned int has_pwm;
	struct s3c2410_pwm pwm;

	unsigned long vib_gpio_pin; /* which pin to meddle with */
	u8 vib_pwm; /* 0 = OFF -- will ensure GPIO deasserted and stop FIQ */
	u8 vib_pwm_latched;
	u32 fiq_count;

	struct gta02_vib_platform_data *pdata;
} gta02_vib_priv;

int gta02_vibrator_fiq_handler(void)
{
	gta02_vib_priv.fiq_count++;

	if (!gta02_vib_priv.vib_pwm_latched && !gta02_vib_priv.vib_pwm)
		/* idle */
		return 0;

	if ((u8)gta02_vib_priv.fiq_count == gta02_vib_priv.vib_pwm_latched)
		s3c2410_gpio_setpin(gta02_vib_priv.vib_gpio_pin, 0);

	if ((u8)gta02_vib_priv.fiq_count)
		return 1;

	gta02_vib_priv.vib_pwm_latched = gta02_vib_priv.vib_pwm;
	if (gta02_vib_priv.vib_pwm_latched)
		s3c2410_gpio_setpin(gta02_vib_priv.vib_gpio_pin, 1);

	return 1;
}

static void gta02_vib_vib_set(struct led_classdev *led_cdev,
				enum led_brightness value)
{
    gta02_vib_priv.vib_pwm = value; /* set it for FIQ */
    gta02_vib_priv.pdata->kick_fiq(); /* start up FIQs if not already going */
}

static struct gta02_vib_priv gta02_vib_led = {
	.cdev = {
		.name = "gta02:vibrator",
		.brightness_set = gta02_vib_vib_set,
	},
};

static int gta02_vib_init_hw(struct gta02_vib_priv *vp)
{
	int rc;

	rc = s3c2410_pwm_init(&vp->pwm);
	if (rc)
		return rc;

	vp->pwm.timerid = PWM3;
	/* use same prescaler as arch/arm/plat-s3c24xx/time.c */
	vp->pwm.prescaler = (6 - 1) / 2;
	vp->pwm.divider = S3C2410_TCFG1_MUX3_DIV2;
	vp->pwm.counter = COUNTER;
	vp->pwm.comparer = COUNTER;

	rc = s3c2410_pwm_enable(&vp->pwm);
	if (rc)
		return rc;

	s3c2410_pwm_start(&vp->pwm);

	return 0;
}

#ifdef CONFIG_PM
static int gta02_vib_suspend(struct platform_device *dev, pm_message_t state)
{
	led_classdev_suspend(&gta02_vib_led.cdev);
	if (gta02_vib_priv.pdata)
		gta02_vib_priv.pdata->disable_fiq();
	return 0;
}

static int gta02_vib_resume(struct platform_device *dev)
{
	struct gta02_vib_priv *vp = platform_get_drvdata(dev);

	if (vp->has_pwm)
		gta02_vib_init_hw(vp);

	led_classdev_resume(&gta02_vib_led.cdev);
	if (gta02_vib_priv.pdata)
		gta02_vib_priv.pdata->enable_fiq();

	return 0;
}
#endif /* CONFIG_PM */

static int __init gta02_vib_probe(struct platform_device *pdev)
{
	struct resource *r;

	r = platform_get_resource(pdev, 0, 0);
	if (!r || !r->start)
		return -EIO;

	gta02_vib_led.gpio = r->start;

	gta02_vib_priv.pdata = pdev->dev.platform_data;
	platform_set_drvdata(pdev, &gta02_vib_led);

    s3c2410_gpio_setpin(gta02_vib_led.gpio, 0); /* off */
    s3c2410_gpio_cfgpin(gta02_vib_led.gpio, S3C2410_GPIO_OUTPUT);
    /* safe, kmalloc'd copy needed for FIQ ISR */
    gta02_vib_priv.vib_gpio_pin = gta02_vib_led.gpio;
    gta02_vib_priv.vib_pwm = 0; /* off */
	spin_lock_init(&gta02_vib_led.lock);

	return led_classdev_register(&pdev->dev, &gta02_vib_led.cdev);
}

static int gta02_vib_remove(struct platform_device *pdev)
{
    gta02_vib_priv.vib_pwm = 0; /* off */
	/* would only need kick if already off so no kick needed */

	if (gta02_vib_led.has_pwm)
		s3c2410_pwm_disable(&gta02_vib_led.pwm);

	led_classdev_unregister(&gta02_vib_led.cdev);

	return 0;
}

static struct platform_driver gta02_vib_driver = {
	.probe		= gta02_vib_probe,
	.remove		= gta02_vib_remove,
#ifdef CONFIG_PM
	.suspend	= gta02_vib_suspend,
	.resume		= gta02_vib_resume,
#endif
	.driver		= {
		.name		= "gta02-vibrator",
	},
};

static int __init gta02_vib_init(void)
{
	return platform_driver_register(&gta02_vib_driver);
}

static void __exit gta02_vib_exit(void)
{
	platform_driver_unregister(&gta02_vib_driver);
}

module_init(gta02_vib_init);
module_exit(gta02_vib_exit);

MODULE_AUTHOR("Harald Welte <laforge@openmoko.org>");
MODULE_DESCRIPTION("Openmoko Freerunner vibrator driver");
MODULE_LICENSE("GPL");

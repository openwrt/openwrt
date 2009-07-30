/*
 * GPS Power Management code for the Openmoko Freerunner GSM Phone
 *
 * (C) 2007-2009 by Openmoko Inc.
 * Author: Harald Welte <laforge@openmoko.org>
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation
 *
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/platform_device.h>

#include <mach/hardware.h>
#include <mach/cpu.h>

#include <asm/mach-types.h>

#include <mach/gta02.h>
#include <linux/mfd/pcf50633/core.h>
#include <linux/mfd/pcf50633/pmic.h>

#include <linux/regulator/consumer.h>
#include <linux/err.h>

struct gta02_pm_gps_data {
#ifdef CONFIG_PM
	int keep_on_in_suspend;
#endif
	int power_was_on;
	struct regulator *regulator;
};

static struct gta02_pm_gps_data gta02_gps;

int gta02_pm_gps_is_on(void)
{
	return gta02_gps.power_was_on;
}
EXPORT_SYMBOL_GPL(gta02_pm_gps_is_on);

/* This is the POWERON pin */
static void gps_pwron_set(int on)
{
	if (on) {
		/* return UART pins to being UART pins */
		s3c2410_gpio_cfgpin(S3C2410_GPH4, S3C2410_GPH4_TXD1);
		/* remove pulldown now it won't be floating any more */
		s3c2410_gpio_pullup(S3C2410_GPH5, 0);

		if (!gta02_gps.power_was_on)
			regulator_enable(gta02_gps.regulator);
	} else {
		/*
		 * take care not to power unpowered GPS from UART TX
		 * return them to GPIO and force low
		 */
		s3c2410_gpio_cfgpin(S3C2410_GPH4, S3C2410_GPH4_OUTP);
		s3c2410_gpio_setpin(S3C2410_GPH4, 0);
		/* don't let RX from unpowered GPS float */
		s3c2410_gpio_pullup(S3C2410_GPH5, 1);
		if (gta02_gps.power_was_on)
			regulator_disable(gta02_gps.regulator);
	}
}

static int gps_pwron_get(void)
{
	return regulator_is_enabled(gta02_gps.regulator);
}

#ifdef CONFIG_PM
/* This is the flag for keeping gps ON during suspend */
static void gps_keep_on_in_suspend_set(int on)
{
	gta02_gps.keep_on_in_suspend = on;
}

static int gps_keep_on_in_suspend_get(void)
{
	return gta02_gps.keep_on_in_suspend;
}
#endif

static ssize_t power_gps_read(struct device *dev,
			      struct device_attribute *attr, char *buf)
{
	int ret = 0;

	if (!strcmp(attr->attr.name, "power_on") ||
	    !strcmp(attr->attr.name, "pwron")) {
		ret = gps_pwron_get();
#ifdef CONFIG_PM
	} else if (!strcmp(attr->attr.name, "keep_on_in_suspend")) {
		ret = gps_keep_on_in_suspend_get();
#endif
	}
	if (ret)
		return strlcpy(buf, "1\n", 3);
	else
		return strlcpy(buf, "0\n", 3);
}

static ssize_t power_gps_write(struct device *dev,
			       struct device_attribute *attr, const char *buf,
			       size_t count)
{
	unsigned long on = simple_strtoul(buf, NULL, 10);

	if (!strcmp(attr->attr.name, "power_on") ||
	    !strcmp(attr->attr.name, "pwron")) {
		gps_pwron_set(on);
		gta02_gps.power_was_on = !!on;
#ifdef CONFIG_PM
	} else if (!strcmp(attr->attr.name, "keep_on_in_suspend")) {
		gps_keep_on_in_suspend_set(on);
#endif
	}
	return count;
}

#ifdef CONFIG_PM
static int gta02_pm_gps_suspend(struct platform_device *pdev,
				pm_message_t state)
{
	if (!gta02_gps.keep_on_in_suspend ||
		!gta02_gps.power_was_on)
		gps_pwron_set(0);
	else
		dev_warn(&pdev->dev, "GTA02: keeping gps ON "
			 "during suspend\n");
	return 0;
}

static int gta02_pm_gps_resume(struct platform_device *pdev)
{
	if (!gta02_gps.keep_on_in_suspend && gta02_gps.power_was_on)
		gps_pwron_set(1);

	return 0;
}

static DEVICE_ATTR(keep_on_in_suspend, 0644, power_gps_read, power_gps_write);
#else
#define gta02_pm_gps_suspend	NULL
#define gta02_pm_gps_resume	NULL
#endif

static DEVICE_ATTR(power_on, 0644, power_gps_read, power_gps_write);

static struct attribute *gta02_gps_sysfs_entries[] = {
	&dev_attr_power_on.attr,
#ifdef CONFIG_PM
	&dev_attr_keep_on_in_suspend.attr,
#endif
	NULL
};

static struct attribute_group gta02_gps_attr_group = {
	.name	= NULL,
	.attrs	= gta02_gps_sysfs_entries,
};

static int __init gta02_pm_gps_probe(struct platform_device *pdev)
{
	gta02_gps.regulator = regulator_get(&pdev->dev, "RF_3V");
	if (IS_ERR(gta02_gps.regulator)) {
		dev_err(&pdev->dev, "probe failed %ld\n",
			PTR_ERR(gta02_gps.regulator));

		return PTR_ERR(gta02_gps.regulator);
	}

	dev_info(&pdev->dev, "starting\n");

	/*
	 * Here we should call the code that handles the set GPS power
	 * off action.  But, the regulator API does not allow us to
	 * reassert regulator state, and when we read the regulator API
	 * logical state, it can differ from the actual state,  So
	 * a workaround for this is to just set the regulator off in the
	 * PMU directly.  Because that's different from normal flow, we
	 * have to reproduce other things from the OFF action here too.
	 */

	/*
	 * u-boot enables LDO5 (GPS), which doesn't make sense and
	 * causes confusion. We therefore disable the regulator here.
	 */
	pcf50633_reg_write(gta02_pcf, PCF50633_REG_LDO5ENA, 0);

	/*
	 * take care not to power unpowered GPS from UART TX
	 * return them to GPIO and force low
	 */
	s3c2410_gpio_cfgpin(S3C2410_GPH4, S3C2410_GPH4_OUTP);
	s3c2410_gpio_setpin(S3C2410_GPH4, 0);
	/* don't let RX from unpowered GPS float */
	s3c2410_gpio_pullup(S3C2410_GPH5, 1);

	return sysfs_create_group(&pdev->dev.kobj,
				  &gta02_gps_attr_group);
}

static int gta02_pm_gps_remove(struct platform_device *pdev)
{
	regulator_put(gta02_gps.regulator);
	sysfs_remove_group(&pdev->dev.kobj, &gta02_gps_attr_group);
	return 0;
}

static struct platform_driver gta02_pm_gps_driver = {
	.probe		= gta02_pm_gps_probe,
	.remove		= gta02_pm_gps_remove,
	.suspend	= gta02_pm_gps_suspend,
	.resume		= gta02_pm_gps_resume,
	.driver		= {
		.name		= "gta02-pm-gps",
	},
};

static int __devinit gta02_pm_gps_init(void)
{
	return platform_driver_register(&gta02_pm_gps_driver);
}

static void gta02_pm_gps_exit(void)
{
	platform_driver_unregister(&gta02_pm_gps_driver);
}

module_init(gta02_pm_gps_init);
module_exit(gta02_pm_gps_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Harald Welte <laforge@openmoko.org>");

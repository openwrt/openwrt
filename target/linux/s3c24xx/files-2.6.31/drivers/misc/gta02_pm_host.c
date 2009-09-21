/*
 * Bluetooth PM code for the FIC gta02 GSM Phone
 *
 * (C) 2007 by Openmoko Inc.
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
#include <linux/platform_device.h>

#include <mach/hardware.h>
#include <asm/mach-types.h>

#include <mach/gta02.h>
#include <linux/mfd/pcf50633/gpio.h>

static ssize_t pm_host_read(struct device *dev, struct device_attribute *attr,
			    char *buf)
{
	return sprintf(buf, "%d\n",
		       pcf50633_gpio_get(gta02_pcf, PCF50633_GPO)
		       				== PCF50633_GPOCFG_GPOSEL_1);
}

static ssize_t pm_host_write(struct device *dev, struct device_attribute *attr,
			     const char *buf, size_t count)
{
	unsigned long on = simple_strtoul(buf, NULL, 10);
	u8 val;

	if (on)
		val = PCF50633_GPOCFG_GPOSEL_1;
	else
		val = PCF50633_GPOCFG_GPOSEL_0;


	pcf50633_gpio_set(gta02_pcf, PCF50633_GPO, val);

	return count;
}

static DEVICE_ATTR(hostmode, 0644, pm_host_read, pm_host_write);

static struct attribute *gta02_pm_host_sysfs_entries[] = {
	&dev_attr_hostmode.attr,
	NULL
};

static struct attribute_group gta02_pm_host_attr_group = {
	.name	= NULL,
	.attrs	= gta02_pm_host_sysfs_entries,
};

static int __init gta02_pm_host_probe(struct platform_device *pdev)
{
	dev_info(&pdev->dev, "starting\n");
	return sysfs_create_group(&pdev->dev.kobj, &gta02_pm_host_attr_group);
}

static int gta02_pm_host_remove(struct platform_device *pdev)
{
	sysfs_remove_group(&pdev->dev.kobj, &gta02_pm_host_attr_group);
	return 0;
}

static struct platform_driver gta02_pm_host_driver = {
	.probe		= gta02_pm_host_probe,
	.remove		= gta02_pm_host_remove,
	.driver		= {
		.name		= "gta02-pm-host",
	},
};

static int __devinit gta02_pm_host_init(void)
{
	return platform_driver_register(&gta02_pm_host_driver);
}

static void gta02_pm_host_exit(void)
{
	platform_driver_unregister(&gta02_pm_host_driver);
}

module_init(gta02_pm_host_init);
module_exit(gta02_pm_host_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Andy Green <andy@openmoko.com>");
MODULE_DESCRIPTION("Openmoko Freerunner USB Host Power Management");

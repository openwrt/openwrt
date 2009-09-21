/*
 * Resume reason sysfs for the Openmoko Freerunner GSM Phone
 *
 * (C) 2008 by Openmoko Inc.
 * Author: Andy Green <andy@openmoko.com>
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License resume_reason 2 as
 * published by the Free Software Foundation
 *
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/io.h>

#include <mach/regs-gpio.h>

static unsigned int *gstatus4_mapped;
static char *resume_reasons[17] = {
	"EINT00_ACCEL1",
	"EINT01_GSM",
	"EINT02_BLUETOOTH",
	"EINT03_DEBUGBRD",
	"EINT04_JACK",
	"EINT05_WLAN",
	"EINT06_AUXKEY",
	"EINT07_HOLDKEY",
	"EINT08_ACCEL2",
	"EINT09_PMU",
	"EINT10_NULL",
	"EINT11_NULL",
	"EINT12_GLAMO",
	"EINT13_NULL",
	"EINT14_NULL",
	"EINT15_NULL",
	NULL
};

static ssize_t resume_reason_read(struct device *dev,
				  struct device_attribute *attr,
				  char *buf)
{
	int bit = 0;
	char *end = buf;

	for (bit = 0; resume_reasons[bit]; bit++) {
		if ((*gstatus4_mapped) & (1 << bit))
			end += sprintf(end, "* %s\n", resume_reasons[bit]);
		else
			end += sprintf(end, "  %s\n", resume_reasons[bit]);

	}

	return end - buf;
}


static DEVICE_ATTR(resume_reason, 0644, resume_reason_read, NULL);

static struct attribute *gta02_resume_reason_sysfs_entries[] = {
	&dev_attr_resume_reason.attr,
	NULL
};

static struct attribute_group gta02_resume_reason_attr_group = {
	.name	= NULL,
	.attrs	= gta02_resume_reason_sysfs_entries,
};

static int __init gta02_resume_reason_probe(struct platform_device *pdev)
{
	dev_info(&pdev->dev, "starting\n");

	gstatus4_mapped = ioremap(S3C24XX_GSTATUS4, 0x4);
	if (!gstatus4_mapped) {
		dev_err(&pdev->dev, "failed to ioremap() memory region\n");
		return -EINVAL;
	}

	return sysfs_create_group(&pdev->dev.kobj,
					    &gta02_resume_reason_attr_group);
}

static int gta02_resume_reason_remove(struct platform_device *pdev)
{
	sysfs_remove_group(&pdev->dev.kobj, &gta02_resume_reason_attr_group);
	iounmap(gstatus4_mapped);
	return 0;
}

static struct platform_driver gta02_resume_reason_driver = {
	.probe		= gta02_resume_reason_probe,
	.remove		= gta02_resume_reason_remove,
	.driver		= {
		.name		= "gta02-resume-reason",
	},
};

static int __devinit gta02_resume_reason_init(void)
{
	return platform_driver_register(&gta02_resume_reason_driver);
}

static void gta02_resume_reason_exit(void)
{
	platform_driver_unregister(&gta02_resume_reason_driver);
}

module_init(gta02_resume_reason_init);
module_exit(gta02_resume_reason_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Andy Green <andy@openmoko.com>");
MODULE_DESCRIPTION("GTA02 resume_reason");

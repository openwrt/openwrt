/*
 * Bluetooth PM code for the Openmoko Freerunner GSM Phone
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
#include <linux/rfkill.h>
#include <linux/err.h>

#include <mach/hardware.h>
#include <asm/mach-types.h>

#include <mach/gta02.h>
#include <linux/mfd/pcf50633/gpio.h>

#include <linux/regulator/consumer.h>

#define DRVMSG "Openmoko Freerunner Bluetooth Power Management"

struct gta02_pm_bt_data {
	struct regulator *regulator;
	struct rfkill *rfkill;
	int pre_resume_state;
};

static ssize_t bt_read(struct device *dev, struct device_attribute *attr,
		       char *buf)
{
	int ret = 0;
	if (!strcmp(attr->attr.name, "power_on")) {
		if (s3c2410_gpio_getpin(GTA02_GPIO_BT_EN))
			ret = 1;
	} else if (!strcmp(attr->attr.name, "reset")) {
		if (s3c2410_gpio_getpin(GTA02_GPIO_BT_EN) == 0)
			ret = 1;
	}

	if (!ret) {
		return strlcpy(buf, "0\n", 3);
	} else {
		return strlcpy(buf, "1\n", 3);
	}
}

static void __gta02_pm_bt_toggle_radio(struct device *dev, unsigned int on)
{
	struct gta02_pm_bt_data *bt_data = dev_get_drvdata(dev);

	dev_info(dev, "__gta02_pm_bt_toggle_radio %d\n", on);

	bt_data = dev_get_drvdata(dev);

    s3c2410_gpio_setpin(GTA02_GPIO_BT_EN, !on);

	if (on) {
		if (!regulator_is_enabled(bt_data->regulator))
			regulator_enable(bt_data->regulator);
	} else {
		if (regulator_is_enabled(bt_data->regulator))
			regulator_disable(bt_data->regulator);
	}

	s3c2410_gpio_setpin(GTA02_GPIO_BT_EN, on);
}


static int bt_rfkill_toggle_radio(void *data, enum rfkill_state state)
{
	struct device *dev = data;
	unsigned long on = (state == RFKILL_STATE_ON);

	__gta02_pm_bt_toggle_radio(dev, on);

	return 0;
}

static ssize_t bt_write(struct device *dev, struct device_attribute *attr,
			const char *buf, size_t count)
{
	unsigned long on = simple_strtoul(buf, NULL, 10);
	struct gta02_pm_bt_data *bt_data = dev_get_drvdata(dev);

	if (!strcmp(attr->attr.name, "power_on")) {
		enum rfkill_state state = on ? RFKILL_STATE_ON : RFKILL_STATE_OFF;
		bt_rfkill_toggle_radio(dev, state);
		bt_data->rfkill->state = state;

		__gta02_pm_bt_toggle_radio(dev, on);
	} else if (!strcmp(attr->attr.name, "reset")) {
		/* reset is low-active, so we need to invert */
		s3c2410_gpio_setpin(GTA02_GPIO_BT_EN, on ? 0 : 1);
	}

	return count;
}

static DEVICE_ATTR(power_on, 0644, bt_read, bt_write);
static DEVICE_ATTR(reset, 0644, bt_read, bt_write);

#ifdef CONFIG_PM
static int gta02_bt_suspend(struct platform_device *pdev, pm_message_t state)
{
	struct gta02_pm_bt_data *bt_data = dev_get_drvdata(&pdev->dev);

	dev_dbg(&pdev->dev, DRVMSG ": suspending\n");

	bt_data->pre_resume_state = s3c2410_gpio_getpin(GTA02_GPIO_BT_EN);
	__gta02_pm_bt_toggle_radio(&pdev->dev, 0);

	return 0;
}

static int gta02_bt_resume(struct platform_device *pdev)
{
	struct gta02_pm_bt_data *bt_data = dev_get_drvdata(&pdev->dev);
	dev_dbg(&pdev->dev, DRVMSG ": resuming\n");

	__gta02_pm_bt_toggle_radio(&pdev->dev, bt_data->pre_resume_state);
	return 0;
}
#else
#define gta02_bt_suspend	NULL
#define gta02_bt_resume		NULL
#endif

static struct attribute *gta02_bt_sysfs_entries[] = {
	&dev_attr_power_on.attr,
	&dev_attr_reset.attr,
	NULL
};

static struct attribute_group gta02_bt_attr_group = {
	.name	= NULL,
	.attrs	= gta02_bt_sysfs_entries,
};

static int __init gta02_bt_probe(struct platform_device *pdev)
{
	struct rfkill *rfkill;
	struct regulator *regulator;
	struct gta02_pm_bt_data *bt_data;
	int ret;

	dev_info(&pdev->dev, DRVMSG ": starting\n");

	bt_data = kzalloc(sizeof(*bt_data), GFP_KERNEL);
	dev_set_drvdata(&pdev->dev, bt_data);

	regulator = regulator_get(&pdev->dev, "BT_3V2");
	if (IS_ERR(regulator))
		return -ENODEV;

	bt_data->regulator = regulator;

	/* this tests the true physical state of the regulator... */
	if (regulator_is_enabled(regulator)) {
		/*
		 * but these only operate on the logical state of the
		 * regulator... so we need to logicaly "adopt" it on
		 * to turn it off
		 */
		regulator_enable(regulator);
		regulator_disable(regulator);
	}

	/* we pull reset to low to make sure that the chip doesn't
	 * drain power through the reset line */
	s3c2410_gpio_setpin(GTA02_GPIO_BT_EN, 0);

	rfkill = rfkill_allocate(&pdev->dev, RFKILL_TYPE_BLUETOOTH);

	rfkill->name = pdev->name;
	rfkill->data = &pdev->dev;
	rfkill->state = RFKILL_STATE_OFF;
	rfkill->toggle_radio = bt_rfkill_toggle_radio;

	ret = rfkill_register(rfkill);
	if (ret) {
		dev_err(&pdev->dev, "Failed to register rfkill\n");
		return ret;
	}

	bt_data->rfkill = rfkill;

	return sysfs_create_group(&pdev->dev.kobj, &gta02_bt_attr_group);
}

static int gta02_bt_remove(struct platform_device *pdev)
{
	struct gta02_pm_bt_data *bt_data = dev_get_drvdata(&pdev->dev);
	struct regulator *regulator;

	sysfs_remove_group(&pdev->dev.kobj, &gta02_bt_attr_group);

	if (bt_data->rfkill) {
		rfkill_unregister(bt_data->rfkill);
		rfkill_free(bt_data->rfkill);
	}

	if (!bt_data || !bt_data->regulator)
		return 0;

	regulator = bt_data->regulator;

	/* Make sure regulator is disabled before calling regulator_put */
	if (regulator_is_enabled(regulator))
		regulator_disable(regulator);

	regulator_put(regulator);

	kfree(bt_data);

	return 0;
}

static struct platform_driver gta02_bt_driver = {
	.probe		= gta02_bt_probe,
	.remove		= gta02_bt_remove,
	.suspend	= gta02_bt_suspend,
	.resume		= gta02_bt_resume,
	.driver		= {
		.name		= "gta02-pm-bt",
	},
};

static int __devinit gta02_bt_init(void)
{
	return platform_driver_register(&gta02_bt_driver);
}

static void gta02_bt_exit(void)
{
	platform_driver_unregister(&gta02_bt_driver);
}

module_init(gta02_bt_init);
module_exit(gta02_bt_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Harald Welte <laforge@openmoko.org>");
MODULE_DESCRIPTION(DRVMSG);

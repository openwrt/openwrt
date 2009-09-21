/*
 * GSM Management code for the Openmoko Freerunner GSM Phone
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
#include <linux/console.h>
#include <linux/errno.h>
#include <linux/interrupt.h>
#include <linux/delay.h>

#include <mach/gpio.h>
#include <asm/mach-types.h>

#include <mach/hardware.h>
#include <mach/cpu.h>

#include <mach/gta02.h>
#include <linux/mfd/pcf50633/gpio.h>
#include <mach/regs-gpio.h>
#include <mach/regs-gpioj.h>

int gta_gsm_interrupts;
EXPORT_SYMBOL(gta_gsm_interrupts);

extern void s3c24xx_serial_console_set_silence(int);

struct gta02pm_priv {
	int gpio_ndl_gsm;
	struct console *con;
};

static struct gta02pm_priv gta02_gsm;

static struct console *find_s3c24xx_console(void)
{
	struct console *con;

	acquire_console_sem();

	for (con = console_drivers; con; con = con->next) {
		if (!strcmp(con->name, "ttySAC"))
			break;
	}

	release_console_sem();

	return con;
}

static ssize_t gsm_read(struct device *dev, struct device_attribute *attr,
			char *buf)
{
	if (!strcmp(attr->attr.name, "power_on")) {
		if (pcf50633_gpio_get(gta02_pcf, PCF50633_GPIO2))
			goto out_1;
	} else if (!strcmp(attr->attr.name, "download")) {
		if (!s3c2410_gpio_getpin(GTA02_GPIO_nDL_GSM))
			goto out_1;
	} else if (!strcmp(attr->attr.name, "flowcontrolled")) {
		if (s3c2410_gpio_getcfg(S3C2410_GPH(1)) == S3C2410_GPIO_OUTPUT)
			goto out_1;
	}

	return strlcpy(buf, "0\n", 3);
out_1:
	return strlcpy(buf, "1\n", 3);
}

static void gsm_on_off(struct device *dev, int on)
{
	if (!on) {
		s3c2410_gpio_cfgpin(S3C2410_GPH(1), S3C2410_GPIO_INPUT);
		s3c2410_gpio_cfgpin(S3C2410_GPH(2), S3C2410_GPIO_INPUT);

		pcf50633_gpio_set(gta02_pcf, PCF50633_GPIO2, 0);

		if (gta02_gsm.con) {
			s3c24xx_serial_console_set_silence(0);
			console_start(gta02_gsm.con);

			dev_dbg(dev, "powered down gta02 GSM, enabling "
					"serial console\n");
		}

		return;
	}

	if (gta02_gsm.con) {
		dev_dbg(dev, "powering up GSM, thus "
				"disconnecting serial console\n");

		console_stop(gta02_gsm.con);
		s3c24xx_serial_console_set_silence(1);
	}

	/* allow UART to talk to GSM side now we will power it */
	s3c2410_gpio_cfgpin(S3C2410_GPH(1), S3C2410_GPH1_nRTS0);
	s3c2410_gpio_cfgpin(S3C2410_GPH(2), S3C2410_GPH2_TXD0);

	pcf50633_gpio_set(gta02_pcf, PCF50633_GPIO2, 7);

	msleep(100);

	s3c2410_gpio_setpin(GTA02_GPIO_MODEM_ON, 1);
	msleep(500);
	s3c2410_gpio_setpin(GTA02_GPIO_MODEM_ON, 0);

	/*
	 * workaround for calypso firmware moko10 and earlier,
	 * without this it will leave IRQ line high after
	 * booting
	 */
	s3c2410_gpio_setpin(S3C2410_GPH(1), 1);
	s3c2410_gpio_cfgpin(S3C2410_GPH(1), S3C2410_GPIO_OUTPUT);
	msleep(1000);
	s3c2410_gpio_cfgpin(S3C2410_GPH(1), S3C2410_GPH1_nRTS0);

}

static ssize_t gsm_write(struct device *dev, struct device_attribute *attr,
			 const char *buf, size_t count)
{
	unsigned long on = simple_strtoul(buf, NULL, 10);

	if (!strcmp(attr->attr.name, "power_on")) {
		gsm_on_off(dev, on);

		return count;
	}

	if (!strcmp(attr->attr.name, "download")) {
		/*
		 * the keyboard / buttons driver requests and enables
		 * the JACK_INSERT IRQ.  We have to take care about
		 * not enabling and disabling the IRQ when it was
		 * already in that state or we get "unblanaced IRQ"
		 * kernel warnings and stack dumps.  So we use the
		 * copy of the ndl_gsm state to figure out if we should
		 * enable or disable the jack interrupt
		 */
		if (on) {
			if (gta02_gsm.gpio_ndl_gsm)
				disable_irq(gpio_to_irq(
						   GTA02_GPIO_JACK_INSERT));
		} else {
			if (!gta02_gsm.gpio_ndl_gsm)
				enable_irq(gpio_to_irq(
						   GTA02_GPIO_JACK_INSERT));
		}

		gta02_gsm.gpio_ndl_gsm = !on;
		s3c2410_gpio_setpin(GTA02_GPIO_nDL_GSM, !on);

		return count;
	}

	if (!strcmp(attr->attr.name, "flowcontrolled")) {
		if (on) {
			gta_gsm_interrupts = 0;
			s3c2410_gpio_setpin(S3C2410_GPH(1), 1);
			s3c2410_gpio_cfgpin(S3C2410_GPH(1), S3C2410_GPIO_OUTPUT);
		} else
			s3c2410_gpio_cfgpin(S3C2410_GPH(1), S3C2410_GPH1_nRTS0);
	}

	return count;
}

static DEVICE_ATTR(power_on, 0644, gsm_read, gsm_write);
static DEVICE_ATTR(reset, 0644, gsm_read, gsm_write);
static DEVICE_ATTR(download, 0644, gsm_read, gsm_write);
static DEVICE_ATTR(flowcontrolled, 0644, gsm_read, gsm_write);

#ifdef CONFIG_PM

static int gta02_gsm_resume(struct platform_device *pdev);
static int gta02_gsm_suspend(struct platform_device *pdev, pm_message_t state)
{
	/* GPIO state is saved/restored by S3C2410 core GPIO driver, so we
	 * don't need to do much here. */

	/* If flowcontrol asserted, abort if GSM already interrupted */
	if (s3c2410_gpio_getcfg(S3C2410_GPH(1)) == S3C2410_GPIO_OUTPUT) {
		if (gta_gsm_interrupts)
			goto busy;
	}

	/* disable DL GSM to prevent jack_insert becoming 'floating' */
	s3c2410_gpio_setpin(GTA02_GPIO_nDL_GSM, 1);
	return 0;

busy:
	return -EBUSY;
}

static int
gta02_gsm_suspend_late(struct platform_device *pdev, pm_message_t state)
{
	/* Last chance: abort if GSM already interrupted */
	if (s3c2410_gpio_getcfg(S3C2410_GPH(1)) == S3C2410_GPIO_OUTPUT) {
		if (gta_gsm_interrupts)
			return -EBUSY;
	}
	return 0;
}

static int gta02_gsm_resume(struct platform_device *pdev)
{
	/* GPIO state is saved/restored by S3C2410 core GPIO driver, so we
	 * don't need to do much here. */

	/* Make sure that the kernel console on the serial port is still
	 * disabled. FIXME: resume ordering race with serial driver! */
	if (gta02_gsm.con && s3c2410_gpio_getpin(GTA02_GPIO_MODEM_ON))
		console_stop(gta02_gsm.con);

	s3c2410_gpio_setpin(GTA02_GPIO_nDL_GSM, gta02_gsm.gpio_ndl_gsm);

	return 0;
}
#else
#define gta02_gsm_suspend	NULL
#define gta02_gsm_suspend_late	NULL
#define gta02_gsm_resume	NULL
#endif /* CONFIG_PM */

static struct attribute *gta02_gsm_sysfs_entries[] = {
	&dev_attr_power_on.attr,
	&dev_attr_reset.attr,
	&dev_attr_download.attr,
	&dev_attr_flowcontrolled.attr,
	NULL
};

static struct attribute_group gta02_gsm_attr_group = {
	.name	= NULL,
	.attrs	= gta02_gsm_sysfs_entries,
};

static int __init gta02_gsm_probe(struct platform_device *pdev)
{
	switch (S3C_SYSTEM_REV_ATAG) {
	case GTA02v1_SYSTEM_REV:
	case GTA02v2_SYSTEM_REV:
	case GTA02v3_SYSTEM_REV:
	case GTA02v4_SYSTEM_REV:
	case GTA02v5_SYSTEM_REV:
	case GTA02v6_SYSTEM_REV:
		break;
	default:
		dev_warn(&pdev->dev, "Unknown Freerunner Revision 0x%x, "
			 "some PM features not available!!!\n",
			 system_rev);
		break;
	}

	gta02_gsm.con = find_s3c24xx_console();
	if (!gta02_gsm.con)
		dev_warn(&pdev->dev,
			 "cannot find S3C24xx console driver\n");

	/* note that download initially disabled, and enforce that */
	gta02_gsm.gpio_ndl_gsm = 1;
	s3c2410_gpio_setpin(GTA02_GPIO_nDL_GSM, 1);

	/* GSM is to be initially off (at boot, or if this module inserted) */
	gsm_on_off(&pdev->dev, 0);

	return sysfs_create_group(&pdev->dev.kobj, &gta02_gsm_attr_group);
}

static int gta02_gsm_remove(struct platform_device *pdev)
{
	sysfs_remove_group(&pdev->dev.kobj, &gta02_gsm_attr_group);

	return 0;
}

static struct platform_driver gta02_gsm_driver = {
	.probe		= gta02_gsm_probe,
	.remove		= gta02_gsm_remove,
	.suspend	= gta02_gsm_suspend,
	.suspend_late	= gta02_gsm_suspend_late,
	.resume		= gta02_gsm_resume,
	.driver		= {
		.name		= "gta02-pm-gsm",
	},
};

static int __devinit gta02_gsm_init(void)
{
	return platform_driver_register(&gta02_gsm_driver);
}

static void gta02_gsm_exit(void)
{
	platform_driver_unregister(&gta02_gsm_driver);
}

module_init(gta02_gsm_init);
module_exit(gta02_gsm_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Harald Welte <laforge@openmoko.org>");
MODULE_DESCRIPTION("Openmoko Freerunner GSM Power Management");

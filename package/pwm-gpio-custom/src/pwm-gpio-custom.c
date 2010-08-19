/*
 *  Custom GPIO-based PWM driver
 *
 *  Based on w1-gpio-custom by Copyright (C) 2007 Gabor Juhos <juhosg at openwrt.org>
 *  Copyright (C) 2010 Claudio Mignanti <c.mignanti@gmail.com >
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 *
 * ---------------------------------------------------------------------------
 *
 *  The behaviour of this driver can be altered by setting some parameters
 *  from the insmod command line.
 *
 *  The following parameters are adjustable:
 *
 *	bus0	These four arguments must be arrays
 *	bus1
 *	.....
 *	bus10	<id>,<pin>,<od>
 *
 *  where:
 *
 *  <id>	ID to used as device_id for the corresponding bus (required)
 *  <pin>	GPIO pin ID for PWM channel (required)
 * *
 *  If this driver is built into the kernel, you can use the following kernel
 *  command line parameters, with the same values as the corresponding module
 *  parameters listed above:
 *
 *	pwm-gpio-custom.bus0
 *	pwm-gpio-custom.bus1
 *	pwm-gpio-custom.bunN
 *	pwm-gpio-custom.bus10
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/workqueue.h>

#include <linux/pwm/pwm.h>

#define DRV_NAME	"pwm-gpio-custom"
#define DRV_DESC	"Custom GPIO-based pwm driver"
#define DRV_VERSION	"0.1.0"

#define PFX		DRV_NAME ": "

#define BUS_PARAM_ID		0
#define BUS_PARAM_PIN		1

#define BUS_PARAM_REQUIRED	2
#define BUS_PARAM_COUNT		2
#define BUS_COUNT_MAX		10

static unsigned int bus0[BUS_PARAM_COUNT] __initdata;
static unsigned int bus1[BUS_PARAM_COUNT] __initdata;
static unsigned int bus2[BUS_PARAM_COUNT] __initdata;
static unsigned int bus3[BUS_PARAM_COUNT] __initdata;
static unsigned int bus4[BUS_PARAM_COUNT] __initdata;
static unsigned int bus5[BUS_PARAM_COUNT] __initdata;
static unsigned int bus6[BUS_PARAM_COUNT] __initdata;
static unsigned int bus7[BUS_PARAM_COUNT] __initdata;
static unsigned int bus8[BUS_PARAM_COUNT] __initdata;
static unsigned int bus9[BUS_PARAM_COUNT] __initdata;

static unsigned int bus_nump[BUS_COUNT_MAX] __initdata;

#define BUS_PARM_DESC " config -> id,pin"

module_param_array(bus0, uint, &bus_nump[0], 0);
MODULE_PARM_DESC(bus0, "bus0" BUS_PARM_DESC);
module_param_array(bus1, uint, &bus_nump[1], 0);
MODULE_PARM_DESC(bus1, "bus1" BUS_PARM_DESC);
module_param_array(bus2, uint, &bus_nump[2], 0);
MODULE_PARM_DESC(bus2, "bus2" BUS_PARM_DESC);
module_param_array(bus3, uint, &bus_nump[3], 0);
MODULE_PARM_DESC(bus3, "bus3" BUS_PARM_DESC);
module_param_array(bus4, uint, &bus_nump[4], 0);
MODULE_PARM_DESC(bus4, "bus4" BUS_PARM_DESC);
module_param_array(bus5, uint, &bus_nump[5], 0);
MODULE_PARM_DESC(bus5, "bus5" BUS_PARM_DESC);
module_param_array(bus6, uint, &bus_nump[6], 0);
MODULE_PARM_DESC(bus6, "bus6" BUS_PARM_DESC);
module_param_array(bus7, uint, &bus_nump[7], 0);
MODULE_PARM_DESC(bus7, "bus7" BUS_PARM_DESC);
module_param_array(bus8, uint, &bus_nump[8], 0);
MODULE_PARM_DESC(bus8, "bus8" BUS_PARM_DESC);
module_param_array(bus9, uint, &bus_nump[9], 0);
MODULE_PARM_DESC(bus9, "bus9" BUS_PARM_DESC);

static struct platform_device *devices[BUS_COUNT_MAX];
static unsigned int nr_devices;

static void pwm_gpio_custom_cleanup(void)
{
	int i;

	for (i = 0; i < nr_devices; i++)
		if (devices[i])
			platform_device_put(devices[i]);
}

static int __init pwm_gpio_custom_add_one(unsigned int id, unsigned int *params)
{
	struct platform_device *pdev;
	struct gpio_pwm_platform_data pdata;
	int err;

	if (!bus_nump[id])
		return 0;

	if (bus_nump[id] < BUS_PARAM_REQUIRED) {
		printk(KERN_ERR PFX "not enough parameters for bus%d\n", id);
		err = -EINVAL;
		goto err;
	}

	pdev = platform_device_alloc("gpio_pwm", params[BUS_PARAM_ID]);
	if (!pdev) {
		err = -ENOMEM;
		goto err;
	}

	pdata.gpio = params[BUS_PARAM_PIN];

	err = platform_device_add_data(pdev, &pdata, sizeof(pdata));
	if (err)
		goto err_put;

	err = platform_device_add(pdev);
	if (err)
		goto err_put;

	devices[nr_devices++] = pdev;
	return 0;

 err_put:
	platform_device_put(pdev);
 err:
	return err;
}

static int __init pwm_gpio_custom_probe(void)
{
	int err;

	nr_devices = 0;
	printk(KERN_INFO DRV_DESC " version " DRV_VERSION "\n");

	err = pwm_gpio_custom_add_one(0, bus0);
	if (err) goto err;

	err = pwm_gpio_custom_add_one(1, bus1);
	if (err) goto err;

	err = pwm_gpio_custom_add_one(2, bus2);
	if (err) goto err;

	err = pwm_gpio_custom_add_one(3, bus3);
	if (err) goto err;

	err = pwm_gpio_custom_add_one(4, bus4);
	if (err) goto err;

	err = pwm_gpio_custom_add_one(5, bus5);
	if (err) goto err;

	err = pwm_gpio_custom_add_one(6, bus6);
	if (err) goto err;

	err = pwm_gpio_custom_add_one(7, bus7);
	if (err) goto err;

	err = pwm_gpio_custom_add_one(8, bus8);
	if (err) goto err;

	err = pwm_gpio_custom_add_one(9, bus9);
	if (err) goto err;

	if (!nr_devices) {
		printk(KERN_ERR PFX "no bus parameter(s) specified\n");
		err = -ENODEV;
		goto err;
	}

	return 0;

err:
	pwm_gpio_custom_cleanup();
	return err;
}

#ifdef MODULE
static int __init pwm_gpio_custom_init(void)
{
	return pwm_gpio_custom_probe();
}
module_init(pwm_gpio_custom_init);

static void __exit pwm_gpio_custom_exit(void)
{
	pwm_gpio_custom_cleanup();
}
module_exit(pwm_gpio_custom_exit);
#else
subsys_initcall(pwm_gpio_custom_probe);
#endif /* MODULE*/

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Bifferos <bifferos at yahoo.co.uk >");
MODULE_AUTHOR("Claudio Mignanti <c.mignanti@gmail.com >");
MODULE_DESCRIPTION(DRV_DESC);
MODULE_VERSION(DRV_VERSION);


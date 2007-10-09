/*
 *  Custom GPIO-based I2C driver
 *
 *  Copyright (C) 2007 Gabor Juhos <juhosg at openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>

#include <linux/i2c-gpio.h>

#define DRV_NAME	"i2c-gpio-custom"
#define DRV_DESC	"Custom GPIO I2C device driver"

static unsigned int sda = CONFIG_I2C_GPIO_CUSTOM_SDA;
static unsigned int scl = CONFIG_I2C_GPIO_CUSTOM_SCL;
static int id = CONFIG_I2C_GPIO_CUSTOM_DEVICE_ID;

module_param(sda, uint, S_IRUGO);
MODULE_PARM_DESC(sda, "GPIO pin for SDA");

module_param(scl, uint, S_IRUGO);
MODULE_PARM_DESC(scl, "GPIO pin for SCL");

module_param(id, int, S_IRUGO);
MODULE_PARM_DESC(id, "device id of the i2c-gpio device");

static struct i2c_gpio_platform_data i2c_data;
static struct platform_device i2c_device;

static void i2c_gpio_custom_release(struct platform_device *pdev)
{
	/* nothing to do */
}

static int __init i2c_gpio_custom_init(void)
{
	int err;

	i2c_data.sda_pin = sda;
	i2c_data.scl_pin = scl;

	i2c_device.name	= "i2c-gpio";
	i2c_device.id	= id;

	i2c_device.dev.platform_data	= &i2c_data,
	i2c_device.dev.release		= i2c_gpio_custom_release,

	err = platform_device_register(&i2c_device);

	return err;
}

static void __exit i2c_gpio_custom_exit(void)
{
	platform_device_unregister(&i2c_device);
}

module_init(i2c_gpio_custom_init);
module_exit(i2c_gpio_custom_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Gabor Juhos <juhosg at openwrt.org >");
MODULE_DESCRIPTION(DRV_DESC);


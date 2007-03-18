/*
 * linux/drivers/leds/leds-ar7.c
 * 
 * Copyright (C) 2007 OpenWrt.org
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */


#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/leds.h>
#include <linux/err.h>
#include <asm/io.h>

#include <asm/gpio.h>

#define DRVNAME "ar7-leds"
#define LONGNAME "TI AR7 LEDs driver"

MODULE_AUTHOR("Nicolas Thill <nico@openwrt.org>");
MODULE_DESCRIPTION(LONGNAME);
MODULE_LICENSE("GPL");

static void ar7_status_led_set(struct led_classdev *pled, 
		enum led_brightness value)
{
	gpio_set_value(AR7_GPIO_BIT_STATUS_LED, value ? 0 : 1);
}

static struct led_classdev ar7_status_led = {
	.name		= "ar7:status",
	.brightness_set	= ar7_status_led_set,
};

#ifdef CONFIG_PM
static int ar7_leds_suspend(struct platform_device *dev,
		pm_message_t state)
{
	led_classdev_suspend(&ar7_status_led);
	return 0;
}

static int ar7_leds_resume(struct platform_device *dev)
{
	led_classdev_resume(&ar7_status_led);
	return 0;
}
#else /* CONFIG_PM */
#define ar7_leds_suspend NULL
#define ar7_leds_resume NULL
#endif /* CONFIG_PM */

static int ar7_leds_probe(struct platform_device *pdev)
{
	int rc;

	rc = led_classdev_register(&pdev->dev, &ar7_status_led);
	if (rc < 0 )
		goto out;

	ar7_gpio_enable(AR7_GPIO_BIT_STATUS_LED);
	gpio_direction_output(AR7_GPIO_BIT_STATUS_LED);

out:
	return rc;
}

static int ar7_leds_remove(struct platform_device *pdev)
{
	led_classdev_unregister(&ar7_status_led);

	return 0;
}

static struct platform_device *ar7_leds_device;

static struct platform_driver ar7_leds_driver = {
	.probe		= ar7_leds_probe,
	.remove		= ar7_leds_remove,
	.suspend	= ar7_leds_suspend,
	.resume		= ar7_leds_resume,
	.driver		= {
		.name		= DRVNAME,
	},
};

static int __init ar7_leds_init(void)
{
	int rc;

	ar7_leds_device = platform_device_alloc(DRVNAME, -1);
	if (!ar7_leds_device)
		return -ENOMEM;

	rc = platform_device_add(ar7_leds_device);
	if (rc < 0)
		goto out_put;

	rc = platform_driver_register(&ar7_leds_driver);
	if (rc < 0)
		goto out_put;

	goto out;

out_put:
	platform_device_put(ar7_leds_device);
out:
	return rc;
}

static void __exit ar7_leds_exit(void)
{
	platform_driver_unregister(&ar7_leds_driver);
	platform_device_unregister(ar7_leds_device);
}

module_init(ar7_leds_init);
module_exit(ar7_leds_exit);

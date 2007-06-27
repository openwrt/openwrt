/*
 *  $Id$
 *
 *  Driver for LEDs connected to GPIO lines
 *
 *  Copyright (C) 2007 OpenWrt.org
 *  Copyright (C) 2007 Gabor Juhos <juhosg@freemail.hu>
 *
 *  This file was derived from:
 *    /drivers/led/leds-s3c24xx.c
 *    (c) 2006 Simtec Electronics, Ben Dooks <ben@simtec.co.uk>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the
 *  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA  02110-1301, USA.
 *
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/leds.h>
#include <linux/err.h>

#include <linux/gpio_leds.h>

#include <asm/io.h>
#include <asm/gpio.h>

#define DRV_NAME "gpio-led"
#define DRV_DESC "GPIO LEDs driver"

struct gpio_led_device {
	struct led_classdev cdev;
	struct gpio_led_platform_data *pdata;
};

static inline struct gpio_led_device *pdev_to_led(struct platform_device *dev)
{
	return platform_get_drvdata(dev);
}

static inline struct gpio_led_device *class_to_led(struct led_classdev *led_cdev)
{
	return container_of(led_cdev, struct gpio_led_device, cdev);
}

static void gpio_led_set(struct led_classdev *led_cdev,
			    enum led_brightness brightness)
{
	struct gpio_led_device *led;
	struct gpio_led_platform_data *pdata;

	led = class_to_led(led_cdev);
	pdata = led->pdata;

	switch (brightness) {
	case LED_OFF:
		gpio_direction_output(pdata->gpio, pdata->value_off);
		break;
	default:
		gpio_direction_output(pdata->gpio, pdata->value_on);
		break;
	}
}

static int __devinit gpio_led_probe(struct platform_device *dev)
{
	struct gpio_led_platform_data *pdata;
	struct gpio_led_device *led;
	int ret;

	pdata = dev->dev.platform_data;
	if (pdata == NULL) {
		dev_err(&dev->dev, "no platform data, id=%d\n", dev->id);
		ret = -EINVAL;
		goto err;
	}

	if (pdata->name == NULL) {
		dev_err(&dev->dev, "no led name specified\n");
		ret = -EINVAL;
		goto err;
	}

	ret = gpio_request(pdata->gpio, pdata->name);
	if (ret) {
		dev_err(&dev->dev, "gpio_request failed\n");
		goto err;
	}

	led = kzalloc(sizeof(*led), GFP_KERNEL);
	if (led == NULL) {
		dev_err(&dev->dev, "no memory for device");
		ret = -ENOMEM;
		goto err_free_gpio;
	}

	platform_set_drvdata(dev, led);
	led->pdata = pdata;
	led->cdev.name = pdata->name;
	led->cdev.brightness_set = gpio_led_set;
#ifdef CONFIG_LEDS_TRIGGERS
	led->cdev.default_trigger = pdata->trigger;
#endif

	ret = led_classdev_register(&dev->dev, &led->cdev);
	if (ret < 0) {
		dev_err(&dev->dev, "led_classdev_register failed");
		goto err_free_led;
	}

	return 0;

err_free_led:
	kfree(led);
err_free_gpio:
	gpio_free(pdata->gpio);
err:
	return ret;
}

static int __devexit gpio_led_remove(struct platform_device *dev)
{
	struct gpio_led_device *led;
	struct gpio_led_platform_data *pdata;

	pdata = dev->dev.platform_data;

	led = pdev_to_led(dev);
	led_classdev_unregister(&led->cdev);
	kfree(led);

	gpio_free(pdata->gpio);

	return 0;
}

#ifdef CONFIG_PM
static int gpio_led_suspend(struct platform_device *dev,
		pm_message_t state)
{
	struct gpio_led_device *led;

	led = pdev_to_led(dev);
	led_classdev_suspend(&led->cdev);

	return 0;
}

static int gpio_led_resume(struct platform_device *dev)
{
	struct gpio_led_device *led;

	led = pdev_to_led(dev);
	led_classdev_resume(&led->cdev);

	return 0;
}
#endif /* CONFIG_PM */

static struct platform_driver gpio_led_driver = {
	.probe		= gpio_led_probe,
	.remove		= __devexit_p(gpio_led_remove),
#ifdef CONFIG_PM
	.suspend	= gpio_led_suspend,
	.resume		= gpio_led_resume,
#endif
	.driver		= {
		.name		= DRV_NAME,
		.owner		= THIS_MODULE,
	},
};

static int __init gpio_led_init(void)
{
	int ret;

	ret = platform_driver_register(&gpio_led_driver);
	if (ret)
		printk(KERN_ALERT DRV_DESC " register failed\n");
	else
		printk(KERN_INFO DRV_DESC " registered\n");

	return ret;
}

static void __exit gpio_led_exit(void)
{
	platform_driver_unregister(&gpio_led_driver);
}

module_init(gpio_led_init);
module_exit(gpio_led_exit);

MODULE_AUTHOR("Gabor Juhos <juhosg@freemail.hu>");
MODULE_DESCRIPTION(DRV_DESC);
MODULE_LICENSE("GPL");

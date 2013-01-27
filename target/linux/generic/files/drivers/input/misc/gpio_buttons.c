/*
 *  Driver for buttons on GPIO lines not capable of generating interrupts
 *
 *  Copyright (C) 2007-2010 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2010 Nuno Goncalves <nunojpg@gmail.com>
 *
 *  This file was based on: /drivers/input/misc/cobalt_btns.c
 *	Copyright (C) 2007 Yoichi Yuasa <yoichi_yuasa@tripeaks.co.jp>
 *
 *  also was based on: /drivers/input/keyboard/gpio_keys.c
 *	Copyright 2005 Phil Blundell
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 *
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/input.h>
#include <linux/input-polldev.h>
#include <linux/ioport.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>
#include <linux/gpio_buttons.h>

#define DRV_NAME	"gpio-buttons"

struct gpio_button_data {
	int last_state;
	int count;
	int can_sleep;
};

struct gpio_buttons_dev {
	struct input_polled_dev *poll_dev;
	struct gpio_buttons_platform_data *pdata;
	struct gpio_button_data *data;
};

static void gpio_buttons_check_state(struct input_dev *input,
				      struct gpio_button *button,
				      struct gpio_button_data *bdata)
{
	int state;

	if (bdata->can_sleep)
		state = !!gpio_get_value_cansleep(button->gpio);
	else
		state = !!gpio_get_value(button->gpio);

	if (state != bdata->last_state) {
		unsigned int type = button->type ?: EV_KEY;

		input_event(input, type, button->code,
			    !!(state ^ button->active_low));
		input_sync(input);
		bdata->count = 0;
		bdata->last_state = state;
	}
}

static void gpio_buttons_poll(struct input_polled_dev *dev)
{
	struct gpio_buttons_dev *bdev = dev->private;
	struct gpio_buttons_platform_data *pdata = bdev->pdata;
	struct input_dev *input = dev->input;
	int i;

	for (i = 0; i < bdev->pdata->nbuttons; i++) {
		struct gpio_button *button = &pdata->buttons[i];
		struct gpio_button_data *bdata = &bdev->data[i];

		if (bdata->count < button->threshold)
			bdata->count++;
		else
			gpio_buttons_check_state(input, button, bdata);

	}
}

static int gpio_buttons_probe(struct platform_device *pdev)
{
	struct gpio_buttons_platform_data *pdata = pdev->dev.platform_data;
	struct device *dev = &pdev->dev;
	struct gpio_buttons_dev *bdev;
	struct input_polled_dev *poll_dev;
	struct input_dev *input;
	int error;
	int i;

	if (!pdata)
		return -ENXIO;

	bdev = kzalloc(sizeof(struct gpio_buttons_dev) +
		       pdata->nbuttons * sizeof(struct gpio_button_data),
		       GFP_KERNEL);
	if (!bdev) {
		dev_err(dev, "no memory for private data\n");
		return -ENOMEM;
	}

	bdev->data = (struct gpio_button_data *) &bdev[1];

	poll_dev = input_allocate_polled_device();
	if (!poll_dev) {
		dev_err(dev, "no memory for polled device\n");
		error = -ENOMEM;
		goto err_free_bdev;
	}

	poll_dev->private = bdev;
	poll_dev->poll = gpio_buttons_poll;
	poll_dev->poll_interval = pdata->poll_interval;

	input = poll_dev->input;

	input->evbit[0] = BIT(EV_KEY);
	input->name = pdev->name;
	input->phys = "gpio-buttons/input0";
	input->dev.parent = &pdev->dev;

	input->id.bustype = BUS_HOST;
	input->id.vendor = 0x0001;
	input->id.product = 0x0001;
	input->id.version = 0x0100;

	for (i = 0; i < pdata->nbuttons; i++) {
		struct gpio_button *button = &pdata->buttons[i];
		unsigned int gpio = button->gpio;
		unsigned int type = button->type ?: EV_KEY;

		error = gpio_request(gpio,
				     button->desc ? button->desc : DRV_NAME);
		if (error) {
			dev_err(dev, "unable to claim gpio %u, err=%d\n",
				gpio, error);
			goto err_free_gpio;
		}

		error = gpio_direction_input(gpio);
		if (error) {
			dev_err(dev,
				"unable to set direction on gpio %u, err=%d\n",
				gpio, error);
			goto err_free_gpio;
		}

		bdev->data[i].can_sleep = gpio_cansleep(gpio);
		bdev->data[i].last_state = -1;

		input_set_capability(input, type, button->code);
	}

	bdev->poll_dev = poll_dev;
	bdev->pdata = pdata;
	platform_set_drvdata(pdev, bdev);

	error = input_register_polled_device(poll_dev);
	if (error) {
		dev_err(dev, "unable to register polled device, err=%d\n",
			error);
		goto err_free_gpio;
	}

	/* report initial state of the buttons */
	for (i = 0; i < pdata->nbuttons; i++)
		gpio_buttons_check_state(input, &pdata->buttons[i],
					 &bdev->data[i]);

	return 0;

err_free_gpio:
	for (i = i - 1; i >= 0; i--)
		gpio_free(pdata->buttons[i].gpio);

	input_free_polled_device(poll_dev);

err_free_bdev:
	kfree(bdev);

	platform_set_drvdata(pdev, NULL);
	return error;
}

static int gpio_buttons_remove(struct platform_device *pdev)
{
	struct gpio_buttons_dev *bdev = platform_get_drvdata(pdev);
	struct gpio_buttons_platform_data *pdata = bdev->pdata;
	int i;

	input_unregister_polled_device(bdev->poll_dev);

	for (i = 0; i < pdata->nbuttons; i++)
		gpio_free(pdata->buttons[i].gpio);

	input_free_polled_device(bdev->poll_dev);

	kfree(bdev);
	platform_set_drvdata(pdev, NULL);

	return 0;
}

static struct platform_driver gpio_buttons_driver = {
	.probe	= gpio_buttons_probe,
	.remove	= gpio_buttons_remove,
	.driver	= {
		.name	= DRV_NAME,
		.owner	= THIS_MODULE,
	},
};

static int __init gpio_buttons_init(void)
{
	return platform_driver_register(&gpio_buttons_driver);
}

static void __exit gpio_buttons_exit(void)
{
	platform_driver_unregister(&gpio_buttons_driver);
}

module_init(gpio_buttons_init);
module_exit(gpio_buttons_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Gabor Juhos <juhosg@openwrt.org>");
MODULE_DESCRIPTION("Polled GPIO Buttons driver");

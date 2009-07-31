/*
 * Keyboard driver for Openmoko Freerunner GSM phone
 *
 * (C) 2006-2007 by Openmoko, Inc.
 * Author: Harald Welte <laforge@openmoko.org>
 * All rights reserved.
 *
 * inspired by corkgbd.c by Richard Purdie
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/init.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/jiffies.h>
#include <linux/module.h>
#include <linux/slab.h>

#include <mach/gpio.h>
#include <asm/mach-types.h>

#ifdef CONFIG_PM
extern int global_inside_suspend;
#else
#define global_inside_suspend 0
#endif

struct gta02kbd {
	struct platform_device *pdev;
	struct input_dev *input;
	int aux_state;
};

enum keys {
	GTA02_KEY_AUX,
	GTA02_KEY_HOLD,
};

struct gta02kbd_key {
	const char * name;
	irqreturn_t (*isr)(int irq, void *dev_id);
	int irq;
	int input_key;
};

static irqreturn_t gta02kbd_aux_irq(int irq, void *dev_id);
static irqreturn_t gta02kbd_default_key_irq(int irq, void *dev_id);


static struct gta02kbd_key keys[] = {
	[GTA02_KEY_AUX] = {
		.name = "GTA02 AUX button",
		.isr = gta02kbd_aux_irq,
		.input_key = KEY_PHONE,
	},
	[GTA02_KEY_HOLD] = {
		.name = "GTA02 HOLD button",
		.isr = gta02kbd_default_key_irq,
		.input_key = KEY_PAUSE,
	},
};

/* This timer section filters AUX button IRQ bouncing */

static void aux_key_timer_f(unsigned long data);

static struct timer_list aux_key_timer =
		TIMER_INITIALIZER(aux_key_timer_f, 0, 0);

#define AUX_TIMER_TIMEOUT (HZ >> 7)
#define AUX_TIMER_ALLOWED_NOOP 2
#define AUX_TIMER_CONSECUTIVE_EVENTS 5

struct gta02kbd *timer_kbd;

static void aux_key_timer_f(unsigned long data)
{
	static int noop_counter;
	static int last_key = -1;
	static int last_count;
	int key_pressed;

	key_pressed =
		gpio_get_value(timer_kbd->pdev->resource[GTA02_KEY_AUX].start);

	if (likely(key_pressed == last_key))
		last_count++;
	else {
		last_count = 1;
		last_key = key_pressed;
	}

	if (unlikely(last_count >= AUX_TIMER_CONSECUTIVE_EVENTS)) {
		if (timer_kbd->aux_state != last_key) {
			input_report_key(timer_kbd->input, KEY_PHONE, last_key);
			input_sync(timer_kbd->input);

			timer_kbd->aux_state = last_key;
			noop_counter = 0;
		}
		last_count = 0;
		if (unlikely(++noop_counter > AUX_TIMER_ALLOWED_NOOP)) {
			noop_counter = 0;
			return;
		}
	}

	mod_timer(&aux_key_timer, jiffies + AUX_TIMER_TIMEOUT);
}

static irqreturn_t gta02kbd_aux_irq(int irq, void *dev)
{
	mod_timer(&aux_key_timer, jiffies + AUX_TIMER_TIMEOUT);

	return IRQ_HANDLED;
}

static irqreturn_t gta02kbd_default_key_irq(int irq, void *dev_id)
{
	struct gta02kbd *kbd = dev_id;
	int n;

	for (n = 0; n < ARRAY_SIZE(keys); n++) {

		if (irq != keys[n].irq)
			continue;

		input_report_key(kbd->input, keys[n].input_key,
				  gpio_get_value(kbd->pdev->resource[n].start));
		input_sync(kbd->input);
	}

	return IRQ_HANDLED;
}

#ifdef CONFIG_PM
static int gta02kbd_suspend(struct platform_device *dev, pm_message_t state)
{
	disable_irq(keys[GTA02_KEY_AUX].irq);
	del_timer_sync(&aux_key_timer);
	return 0;
}

static int gta02kbd_resume(struct platform_device *dev)
{
	enable_irq(keys[GTA02_KEY_AUX].irq);
	return 0;
}
#else
#define gta02kbd_suspend	NULL
#define gta02kbd_resume	NULL
#endif

static int gta02kbd_probe(struct platform_device *pdev)
{
	struct gta02kbd *gta02kbd;
	struct input_dev *input_dev;
	int rc;
	int irq;
	int n;

	if (pdev->resource[0].flags != 0)
		return -EINVAL;

	gta02kbd = kzalloc(sizeof(struct gta02kbd), GFP_KERNEL);
	input_dev = input_allocate_device();
	if (!gta02kbd || !input_dev) {
		kfree(gta02kbd);
		input_free_device(input_dev);
		return -ENOMEM;
	}

	gta02kbd->pdev = pdev;
	timer_kbd = gta02kbd;

	platform_set_drvdata(pdev, gta02kbd);

	gta02kbd->input = input_dev;

	input_dev->name = "GTA02 Buttons";
	input_dev->phys = "gta02kbd/input0";
	input_dev->id.bustype = BUS_HOST;
	input_dev->id.vendor = 0x0001;
	input_dev->id.product = 0x0001;
	input_dev->id.version = 0x0100;
	input_dev->dev.parent = &pdev->dev;

	input_dev->evbit[0] = BIT(EV_KEY) | BIT(EV_SW);
	set_bit(KEY_PHONE, input_dev->keybit);
	set_bit(KEY_PAUSE, input_dev->keybit);

	rc = input_register_device(gta02kbd->input);
	if (rc)
		goto out_register;

	/* register GPIO IRQs */
	for(n = 0; n < min(pdev->num_resources, ARRAY_SIZE(keys)); n++) {

		if (!pdev->resource[0].start)
			continue;

		irq = gpio_to_irq(pdev->resource[n].start);
		if (irq < 0)
			continue;

		if (request_irq(irq, keys[n].isr, IRQF_DISABLED |
				IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
				keys[n].name, gta02kbd)) {
			dev_err(&pdev->dev, "Can't get IRQ %u\n", irq);

			/* unwind any irq registrations and fail */

			while (n > 0) {
				n--;
				free_irq(gpio_to_irq(pdev->resource[n].start),
								    gta02kbd);
			}
			goto out_device_create;
		}

		keys[n].irq = irq;
	}


	return 0;

out_device_create:
	input_unregister_device(gta02kbd->input);
out_register:
	input_free_device(gta02kbd->input);
	platform_set_drvdata(pdev, NULL);
	kfree(gta02kbd);

	return -ENODEV;
}

static int gta02kbd_remove(struct platform_device *pdev)
{
	struct gta02kbd *gta02kbd = platform_get_drvdata(pdev);

	free_irq(gpio_to_irq(pdev->resource[1].start), gta02kbd);
	free_irq(gpio_to_irq(pdev->resource[0].start), gta02kbd);

	input_unregister_device(gta02kbd->input);
	input_free_device(gta02kbd->input);
	platform_set_drvdata(pdev, NULL);
	kfree(gta02kbd);

	return 0;
}

static struct platform_driver gta02kbd_driver = {
	.probe		= gta02kbd_probe,
	.remove		= gta02kbd_remove,
	.suspend	= gta02kbd_suspend,
	.resume		= gta02kbd_resume,
	.driver		= {
		.name	= "gta02-button",
	},
};

static int __devinit gta02kbd_init(void)
{
	return platform_driver_register(&gta02kbd_driver);
}

static void __exit gta02kbd_exit(void)
{
	platform_driver_unregister(&gta02kbd_driver);
}

module_init(gta02kbd_init);
module_exit(gta02kbd_exit);

MODULE_AUTHOR("Harald Welte <laforge@openmoko.org>");
MODULE_DESCRIPTION("Openmoko Freerunner buttons input driver");
MODULE_LICENSE("GPL");

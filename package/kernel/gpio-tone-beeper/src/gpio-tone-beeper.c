// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  GPIO tone beeper driver
 *
 *  Plays tones on a passive piezo buzzer by toggling a GPIO from a
 *  high-resolution timer, for kernels built without PWM support.
 *
 *  Copyright (C) 2026 Santiago Hormazabal <santiagohssl@gmail.com>
 *
 *  The input device handling is based on:
 *    pwm-beeper.c, Copyright (C) 2010, Lars-Peter Clausen <lars@metafoo.de>
 *    gpio-beeper.c, Copyright (C) 2013-2014 Alexander Shiyan <shc_work@mail.ru>
 */

#include <linux/gpio/consumer.h>
#include <linux/hrtimer.h>
#include <linux/input.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/spinlock.h>

#define GPIO_TONE_BEEPER_BELL_HZ	1000

struct gpio_tone_beeper {
	struct input_dev *input;
	struct gpio_desc *gpio;
	struct hrtimer timer;
	spinlock_t lock;
	ktime_t half_period;
	bool active;
	bool level;
};

static enum hrtimer_restart gpio_tone_beeper_toggle(struct hrtimer *timer)
{
	struct gpio_tone_beeper *beeper =
		container_of(timer, struct gpio_tone_beeper, timer);
	unsigned long flags;
	enum hrtimer_restart ret = HRTIMER_NORESTART;

	spin_lock_irqsave(&beeper->lock, flags);
	if (beeper->active) {
		beeper->level = !beeper->level;
		gpiod_set_value(beeper->gpio, beeper->level);
		hrtimer_forward_now(timer, beeper->half_period);
		ret = HRTIMER_RESTART;
	}
	spin_unlock_irqrestore(&beeper->lock, flags);

	return ret;
}

static void gpio_tone_beeper_set(struct gpio_tone_beeper *beeper,
				 unsigned int hz)
{
	unsigned long flags;

	hrtimer_cancel(&beeper->timer);

	spin_lock_irqsave(&beeper->lock, flags);
	beeper->active = hz > 0;
	beeper->level = false;
	gpiod_set_value(beeper->gpio, 0);
	if (hz)
		beeper->half_period = ns_to_ktime(NSEC_PER_SEC / (2 * hz));
	spin_unlock_irqrestore(&beeper->lock, flags);

	if (hz)
		hrtimer_start(&beeper->timer, beeper->half_period,
			      HRTIMER_MODE_REL);
}

static int gpio_tone_beeper_event(struct input_dev *input, unsigned int type,
				  unsigned int code, int value)
{
	struct gpio_tone_beeper *beeper = input_get_drvdata(input);

	if (type != EV_SND || value < 0)
		return -EINVAL;

	switch (code) {
	case SND_BELL:
		value = value ? GPIO_TONE_BEEPER_BELL_HZ : 0;
		break;
	case SND_TONE:
		break;
	default:
		return -EINVAL;
	}

	gpio_tone_beeper_set(beeper, value);

	return 0;
}

static void gpio_tone_beeper_close(struct input_dev *input)
{
	gpio_tone_beeper_set(input_get_drvdata(input), 0);
}

static int gpio_tone_beeper_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct gpio_tone_beeper *beeper;
	int ret;

	beeper = devm_kzalloc(dev, sizeof(*beeper), GFP_KERNEL);
	if (!beeper)
		return -ENOMEM;

	beeper->gpio = devm_gpiod_get(dev, NULL, GPIOD_OUT_LOW);
	if (IS_ERR(beeper->gpio))
		return dev_err_probe(dev, PTR_ERR(beeper->gpio),
				     "failed to get GPIO\n");

	/* The GPIO is toggled from hard interrupt context */
	if (gpiod_cansleep(beeper->gpio))
		return dev_err_probe(dev, -EINVAL,
				     "GPIOs that can sleep are not supported\n");

	spin_lock_init(&beeper->lock);
	hrtimer_setup(&beeper->timer, gpio_tone_beeper_toggle, CLOCK_MONOTONIC,
		      HRTIMER_MODE_REL);

	beeper->input = devm_input_allocate_device(dev);
	if (!beeper->input)
		return -ENOMEM;

	beeper->input->name = pdev->name;
	beeper->input->phys = "gpio-tone-beeper/input0";
	beeper->input->id.bustype = BUS_HOST;
	beeper->input->event = gpio_tone_beeper_event;
	beeper->input->close = gpio_tone_beeper_close;
	input_set_capability(beeper->input, EV_SND, SND_TONE);
	input_set_capability(beeper->input, EV_SND, SND_BELL);
	input_set_drvdata(beeper->input, beeper);

	ret = input_register_device(beeper->input);
	if (ret)
		return ret;

	platform_set_drvdata(pdev, beeper);

	return 0;
}

static void gpio_tone_beeper_remove(struct platform_device *pdev)
{
	struct gpio_tone_beeper *beeper = platform_get_drvdata(pdev);

	gpio_tone_beeper_set(beeper, 0);
}

static const struct of_device_id gpio_tone_beeper_match[] = {
	{ .compatible = "openwrt,gpio-tone-beeper" },
	{}
};
MODULE_DEVICE_TABLE(of, gpio_tone_beeper_match);

static struct platform_driver gpio_tone_beeper_driver = {
	.probe = gpio_tone_beeper_probe,
	.remove = gpio_tone_beeper_remove,
	.driver = {
		.name = "gpio-tone-beeper",
		.of_match_table = gpio_tone_beeper_match,
	},
};

module_platform_driver(gpio_tone_beeper_driver);

MODULE_DESCRIPTION("GPIO tone beeper driver");
MODULE_LICENSE("GPL");

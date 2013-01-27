/*
 * drivers/pwm/gpio.c
 *
 * Models a single-channel PWM device using a timer and a GPIO pin.
 *
 * Copyright (C) 2010 Bill Gatliff <bgat@billgatliff.com>
 *
 * This program is free software; you may redistribute and/or modify
 * it under the terms of the GNU General Public License Version 2, as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/hrtimer.h>
#include <linux/err.h>
#include <linux/platform_device.h>
#include <linux/workqueue.h>
#include <linux/gpio.h>
#include <linux/slab.h>
#include <linux/pwm/pwm.h>

struct gpio_pwm {
	struct pwm_device pwm;
	struct hrtimer timer;
	struct work_struct work;
	pwm_callback_t callback;
	int gpio;
	unsigned long polarity : 1;
	unsigned long active : 1;
};

static inline struct gpio_pwm *to_gpio_pwm(const struct pwm_channel *p)
{
	return container_of(p->pwm, struct gpio_pwm, pwm);
}

static void
gpio_pwm_work (struct work_struct *work)
{
	struct gpio_pwm *gp = container_of(work, struct gpio_pwm, work);

	if (gp->active)
		gpio_direction_output(gp->gpio, gp->polarity ? 1 : 0);
	else
		gpio_direction_output(gp->gpio, gp->polarity ? 0 : 1);
}

static enum hrtimer_restart
gpio_pwm_timeout(struct hrtimer *t)
{
	struct gpio_pwm *gp = container_of(t, struct gpio_pwm, timer);
	ktime_t tnew;

	if (unlikely(gp->pwm.channels[0].duty_ticks == 0))
		gp->active = 0;
	else if (unlikely(gp->pwm.channels[0].duty_ticks
			  == gp->pwm.channels[0].period_ticks))
		gp->active = 1;
	else
		gp->active ^= 1;

	if (gpio_cansleep(gp->gpio))
		schedule_work(&gp->work);
	else
		gpio_pwm_work(&gp->work);

	if (!gp->active && gp->pwm.channels[0].callback)
		gp->pwm.channels[0].callback(&gp->pwm.channels[0]);

	if (unlikely(!gp->active &&
		     (gp->pwm.channels[0].flags & BIT(FLAG_STOP)))) {
		clear_bit(FLAG_STOP, &gp->pwm.channels[0].flags);
		complete_all(&gp->pwm.channels[0].complete);
		return HRTIMER_NORESTART;
	}

	if (gp->active)
		tnew = ktime_set(0, gp->pwm.channels[0].duty_ticks);
	else
		tnew = ktime_set(0, gp->pwm.channels[0].period_ticks
				 - gp->pwm.channels[0].duty_ticks);
	hrtimer_start(&gp->timer, tnew, HRTIMER_MODE_REL);

	return HRTIMER_NORESTART;
}

static void gpio_pwm_start(struct pwm_channel *p)
{
	struct gpio_pwm *gp = to_gpio_pwm(p);

	gp->active = 0;
	gpio_pwm_timeout(&gp->timer);
}

static int
gpio_pwm_config_nosleep(struct pwm_channel *p,
			struct pwm_channel_config *c)
{
	struct gpio_pwm *gp = to_gpio_pwm(p);
	int ret = 0;
	unsigned long flags;

	spin_lock_irqsave(&p->lock, flags);

	switch (c->config_mask) {

	case PWM_CONFIG_DUTY_TICKS:
		p->duty_ticks = c->duty_ticks;
		break;

	case PWM_CONFIG_START:
		if (!hrtimer_active(&gp->timer)) {
			gpio_pwm_start(p);
		}
		break;
	default:
		ret = -EINVAL;
		break;
	}

	spin_unlock_irqrestore(&p->lock, flags);
	return ret;
}

static int
gpio_pwm_stop_sync(struct pwm_channel *p)
{
	struct gpio_pwm *gp = to_gpio_pwm(p);
	int ret;
	int was_on = hrtimer_active(&gp->timer);

	if (was_on) {
		do {
			init_completion(&p->complete);
			set_bit(FLAG_STOP, &p->flags);
			ret = wait_for_completion_interruptible(&p->complete);
			if (ret)
				return ret;
		} while (p->flags & BIT(FLAG_STOP));
	}

	return was_on;
}

static int
gpio_pwm_config(struct pwm_channel *p,
		struct pwm_channel_config *c)
{
	struct gpio_pwm *gp = to_gpio_pwm(p);
	int was_on = 0;

	if (p->pwm->config_nosleep) {
		if (!p->pwm->config_nosleep(p, c))
			return 0;
	}

	might_sleep();

	was_on = gpio_pwm_stop_sync(p);
	if (was_on < 0)
		return was_on;

	if (c->config_mask & PWM_CONFIG_PERIOD_TICKS)
		p->period_ticks = c->period_ticks;

	if (c->config_mask & PWM_CONFIG_DUTY_TICKS)
		p->duty_ticks = c->duty_ticks;

	if (c->config_mask & PWM_CONFIG_POLARITY) {
		gp->polarity = c->polarity ? 1 : 0;
		p->active_high = gp->polarity;
	}

	if ((c->config_mask & PWM_CONFIG_START)
	    || (was_on && !(c->config_mask & PWM_CONFIG_STOP)))
		gpio_pwm_start(p);

	return 0;
}

static int
gpio_pwm_set_callback(struct pwm_channel *p,
		      pwm_callback_t callback)
{
	struct gpio_pwm *gp = to_gpio_pwm(p);
	gp->callback = callback;
	return 0;
}

static int
gpio_pwm_request(struct pwm_channel *p)
{
	p->tick_hz = 1000000000UL;
	return 0;
}

static int
gpio_pwm_probe(struct platform_device *pdev)
{
	struct gpio_pwm *gp;
	struct gpio_pwm_platform_data *gpd = pdev->dev.platform_data;
	int ret = 0;

	/* TODO: create configfs entries, so users can assign GPIOs to
	 * PWMs at runtime instead of creating a platform_device
	 * specification and rebuilding their kernel */

	if (!gpd || gpio_request(gpd->gpio, dev_name(&pdev->dev)))
		return -EINVAL;

	gp = kzalloc(sizeof(*gp), GFP_KERNEL);
	if (!gp) {
		ret = -ENOMEM;
		goto err_alloc;
	}

	platform_set_drvdata(pdev, gp);

	gp->pwm.dev = &pdev->dev;
	gp->pwm.bus_id = dev_name(&pdev->dev);
	gp->pwm.nchan = 1;
	gp->gpio = gpd->gpio;

	INIT_WORK(&gp->work, gpio_pwm_work);

	hrtimer_init(&gp->timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	gp->timer.function = gpio_pwm_timeout;

	gp->pwm.owner = THIS_MODULE;
	gp->pwm.config_nosleep = gpio_pwm_config_nosleep;
	gp->pwm.config = gpio_pwm_config;
	gp->pwm.request = gpio_pwm_request;
	gp->pwm.set_callback = gpio_pwm_set_callback;

	ret = pwm_register(&gp->pwm);
	if (ret)
		goto err_pwm_register;

	return 0;

err_pwm_register:
	platform_set_drvdata(pdev, 0);
	kfree(gp);
err_alloc:
	return ret;
}

static int
gpio_pwm_remove(struct platform_device *pdev)
{
	struct gpio_pwm *gp = platform_get_drvdata(pdev);
	int ret;

	ret = pwm_unregister(&gp->pwm);
	hrtimer_cancel(&gp->timer);
	cancel_work_sync(&gp->work);
	platform_set_drvdata(pdev, 0);
	kfree(gp);

	return 0;
}

static struct platform_driver gpio_pwm_driver = {
	.driver = {
		.name = "gpio_pwm",
		.owner = THIS_MODULE,
	},
	.probe = gpio_pwm_probe,
	.remove = gpio_pwm_remove,
};

static int __init gpio_pwm_init(void)
{
	return platform_driver_register(&gpio_pwm_driver);
}
module_init(gpio_pwm_init);

static void __exit gpio_pwm_exit(void)
{
	platform_driver_unregister(&gpio_pwm_driver);
}
module_exit(gpio_pwm_exit);

MODULE_AUTHOR("Bill Gatliff <bgat@billgatliff.com>");
MODULE_DESCRIPTION("PWM output using GPIO and a high-resolution timer");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:gpio_pwm");

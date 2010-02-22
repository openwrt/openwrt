/*
 * arch/ubicom32/mach-common/ubicom32input.c
 *   Ubicom32 Input driver
 *
 *   based on gpio-keys
 *
 * (C) Copyright 2009, Ubicom, Inc.
 *
 * This file is part of the Ubicom32 Linux Kernel Port.
 *
 * The Ubicom32 Linux Kernel Port is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 2 of the
 * License, or (at your option) any later version.
 *
 * The Ubicom32 Linux Kernel Port is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 * the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Ubicom32 Linux Kernel Port.  If not,
 * see <http://www.gnu.org/licenses/>.
 *
 * Ubicom32 implementation derived from (with many thanks):
 *   arch/m68knommu
 *   arch/blackfin
 *   arch/parisc
 *
 *
 * TODO: add groups for inputs which can be sampled together (i.e. I2C)
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/input.h>
#include <linux/input-polldev.h>
#include <linux/delay.h>
#include <linux/gpio.h>

#include <asm/ubicom32input.h>

struct ubicom32input_data {
	struct ubicom32input_platform_data	*pdata;

	struct input_polled_dev			*poll_dev;

	/*
	 * collection of previous states for buttons
	 */
	u8					prev_state[0];
};

/*
 * ubicom32input_poll
 */
static void ubicom32input_poll(struct input_polled_dev *dev)
{
	struct ubicom32input_data *ud =
		(struct ubicom32input_data *)dev->private;
	struct ubicom32input_platform_data *pdata = ud->pdata;
	struct input_dev *id = dev->input;
	int i;
	int sync_needed = 0;

	for (i = 0; i < pdata->nbuttons; i++) {
		const struct ubicom32input_button *ub = &pdata->buttons[i];
		int state = 0;

		int val = gpio_get_value(ub->gpio);

		/*
		 * Check to see if the state changed from the last time we
		 * looked
		 */
		if (val == ud->prev_state[i]) {
			continue;
		}

		/*
		 * The state has changed, determine if we are "up" or "down"
		 */
		ud->prev_state[i] = val;

		if ((!val && ub->active_low) || (val && !ub->active_low)) {
			state = 1;
		}

		input_event(id, ub->type, ub->code, state);
		sync_needed = 1;
	}

	if (sync_needed) {
		input_sync(id);
	}
}

/*
 * ubicom32input_probe
 */
static int __devinit ubicom32input_probe(struct platform_device *pdev)
{
	int i;
	struct ubicom32input_data *ud;
	struct input_polled_dev *poll_dev;
	struct input_dev *input_dev;
	struct ubicom32input_platform_data *pdata;
	int ret;

	pdata = pdev->dev.platform_data;
	if (!pdata) {
		return -EINVAL;
	}

	ud = kzalloc(sizeof(struct ubicom32input_data) +
		     pdata->nbuttons, GFP_KERNEL);
	if (!ud) {
		return -ENOMEM;
	}
	ud->pdata = pdata;

	poll_dev = input_allocate_polled_device();
	if (!poll_dev) {
		ret = -ENOMEM;
		goto fail;
	}

	platform_set_drvdata(pdev, ud);

	ud->poll_dev = poll_dev;
	poll_dev->private = ud;
	poll_dev->poll = ubicom32input_poll;

	/*
	 * Set the poll interval requested, default to 50 msec
	 */
	if (pdata->poll_interval) {
		poll_dev->poll_interval = pdata->poll_interval;
	} else {
		poll_dev->poll_interval = 50;
	}

	/*
	 * Setup the input device
	 */
	input_dev = poll_dev->input;
	input_dev->name = pdata->name ? pdata->name : "Ubicom32 Input";
	input_dev->phys = "ubicom32input/input0";
	input_dev->dev.parent = &pdev->dev;
	input_dev->id.bustype = BUS_HOST;

	/*
	 * Reserve the GPIOs
	 */
	for (i = 0; i < pdata->nbuttons; i++) {
		const struct ubicom32input_button *ub = &pdata->buttons[i];

		ret = gpio_request(ub->gpio,
				   ub->desc ? ub->desc : "ubicom32input");
		if (ret < 0) {
			pr_err("ubicom32input: failed to request "
			       "GPIO %d ret=%d\n", ub->gpio, ret);
			goto fail2;
		}

		ret = gpio_direction_input(ub->gpio);
		if (ret < 0) {
			pr_err("ubicom32input: failed to set "
			       "GPIO %d to input ret=%d\n", ub->gpio, ret);
			goto fail2;
		}

		/*
		 * Set the previous state to the non-active stae
		 */
		ud->prev_state[i] = ub->active_low;

		input_set_capability(input_dev,
				     ub->type ? ub->type : EV_KEY, ub->code);
	}

	/*
	 * Register
	 */
	ret = input_register_polled_device(ud->poll_dev);
	if (ret) {
		goto fail2;
	}

	return 0;

fail2:
	/*
	 * release the GPIOs we have already requested.
	 */
	while (--i >= 0) {
		gpio_free(pdata->buttons[i].gpio);
	}

fail:
	printk(KERN_ERR "Ubicom32Input: Failed to register driver %d", ret);
	platform_set_drvdata(pdev, NULL);
	input_free_polled_device(poll_dev);
	kfree(ud);
	return ret;
}

/*
 * ubicom32input_remove
 */
static int __devexit ubicom32input_remove(struct platform_device *dev)
{
	struct ubicom32input_data *ud =
		(struct ubicom32input_data *)platform_get_drvdata(dev);
	int i;

	/*
	 * Free the GPIOs
	 */
	for (i = 0; i < ud->pdata->nbuttons; i++) {
		gpio_free(ud->pdata->buttons[i].gpio);
	}

	platform_set_drvdata(dev, NULL);
	input_unregister_polled_device(ud->poll_dev);
	input_free_polled_device(ud->poll_dev);

	kfree(ud);

	return 0;
}

static struct platform_driver ubicom32input_driver = {
	.driver         = {
		.name   = "ubicom32input",
		.owner  = THIS_MODULE,
	},
	.probe          = ubicom32input_probe,
	.remove         = __devexit_p(ubicom32input_remove),
};

/*
 * ubicom32input_init
 */
static int __devinit ubicom32input_init(void)
{
	return platform_driver_register(&ubicom32input_driver);
}

/*
 * ubicom32input_exit
 */
static void __exit ubicom32input_exit(void)
{
	platform_driver_unregister(&ubicom32input_driver);
}

module_init(ubicom32input_init);
module_exit(ubicom32input_exit);

MODULE_AUTHOR("Pat Tjin <pattjin@ubicom.com>");
MODULE_DESCRIPTION("Ubicom32 Input Driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:ubicom32-input");

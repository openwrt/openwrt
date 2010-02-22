/*
 * drivers/video/backlight/ubicom32lcdpowerpower.c
 *	LCD power driver for the Ubicom32 platform
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
 */
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/lcd.h>
#include <linux/fb.h>
#include <linux/gpio.h>

#include <asm/ubicom32lcdpower.h>
#include <asm/ip5000.h>

#define DRIVER_NAME			"ubicom32lcdpower"

struct ubicom32lcdpower_data {
	/*
	 * Pointer to the platform data structure.  Keep this around since we need values
	 * from it to set the backlight intensity.
	 */
	const struct ubicom32lcdpower_platform_data	*pdata;

	/*
	 * LCD device, we have to save this for use when we remove ourselves.
	 */
	struct lcd_device			*lcddev;
};

/*
 * ubicom32lcdpower_set_power
 */
static int ubicom32lcdpower_set_power(struct lcd_device *ld, int power)
{
	struct ubicom32lcdpower_data *ud = (struct ubicom32lcdpower_data *)lcd_get_data(ld);
	if (power == FB_BLANK_UNBLANK) {
		gpio_direction_output(ud->pdata->vgh_gpio, ud->pdata->vgh_polarity);
		return 0;
	}

	gpio_direction_output(ud->pdata->vgh_gpio, !ud->pdata->vgh_polarity);
	return 0;
}

/*
 * ubicom32lcdpower_get_power
 */
static int ubicom32lcdpower_get_power(struct lcd_device *ld)
{
	struct ubicom32lcdpower_data *ud = (struct ubicom32lcdpower_data *)lcd_get_data(ld);
	int vgh = gpio_get_value(ud->pdata->vgh_gpio);
	if ((vgh && ud->pdata->vgh_polarity) || (!vgh && !ud->pdata->vgh_polarity)) {
		return 1;
	}

	return 0;
}

static struct lcd_ops ubicom32lcdpower_ops = {
	.get_power = ubicom32lcdpower_get_power,
	.set_power = ubicom32lcdpower_set_power,
};

/*
 * ubicom32lcdpower_probe
 */
static int ubicom32lcdpower_probe(struct platform_device *pdev)
{
	const struct ubicom32lcdpower_platform_data *pdata = pdev->dev.platform_data;
	struct ubicom32lcdpower_data *ud;
	struct lcd_device *lcddev;
	int retval;

	/*
	 * Check to see if we have any platform data, if we don't have a LCD to control
	 */
	if (!pdata) {
		return -ENODEV;
	}

	/*
	 * Allocate our private data
	 */
	ud = kzalloc(sizeof(struct ubicom32lcdpower_data), GFP_KERNEL);
	if (!ud) {
		return -ENOMEM;
	}

	ud->pdata = pdata;

	/*
	 * Request our GPIOs
	 */
	retval = gpio_request(pdata->vgh_gpio, "vgh");
	if (retval) {
		dev_err(&pdev->dev, "Failed to allocate vgh GPIO\n");
		goto fail_gpio;
	}

	/*
	 * Register our lcd device
	 */
	lcddev = lcd_device_register(DRIVER_NAME, &pdev->dev, ud, &ubicom32lcdpower_ops);
	if (IS_ERR(lcddev)) {
		retval = PTR_ERR(lcddev);
		goto fail;
	}

	ud->lcddev = lcddev;
	platform_set_drvdata(pdev, ud);

	ubicom32lcdpower_set_power(lcddev, FB_BLANK_UNBLANK);

	printk(KERN_INFO DRIVER_NAME ": LCD driver started\n");

	return 0;

fail:
	gpio_free(pdata->vgh_gpio);

fail_gpio:
	platform_set_drvdata(pdev, NULL);
	kfree(ud);
	return retval;
}

/*
 * ubicom32lcdpower_remove
 */
static int __exit ubicom32lcdpower_remove(struct platform_device *pdev)
{
	struct ubicom32lcdpower_data *ud = platform_get_drvdata(pdev);

	lcd_device_unregister(ud->lcddev);
	platform_set_drvdata(pdev, NULL);
	kfree(ud);

	return 0;
}

static struct platform_driver ubicom32lcdpower_driver = {
	.driver = {
		.name = DRIVER_NAME,
		.owner = THIS_MODULE,
	},

	.remove = __exit_p(ubicom32lcdpower_remove),
};

/*
 * ubicom32lcdpower_init
 */
static int __init ubicom32lcdpower_init(void)
{
	return platform_driver_probe(&ubicom32lcdpower_driver, ubicom32lcdpower_probe);
}
module_init(ubicom32lcdpower_init);

/*
 * ubicom32lcdpower_exit
 */
static void __exit ubicom32lcdpower_exit(void)
{
	platform_driver_unregister(&ubicom32lcdpower_driver);
}
module_exit(ubicom32lcdpower_exit);

MODULE_AUTHOR("Patrick Tjin <@ubicom.com>");
MODULE_DESCRIPTION("Ubicom32 lcd power driver");
MODULE_LICENSE("GPL");

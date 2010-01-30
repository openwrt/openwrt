/*
 *  Ralink SoC GPIO LED device support
 *
 *  Copyright (C) 2009 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/init.h>
#include <linux/platform_device.h>

#include <asm/mach-ralink/dev-gpio-leds.h>

void __init ramips_register_gpio_leds(int id, unsigned num_leds,
				      struct gpio_led *leds)
{
	struct platform_device *pdev;
	struct gpio_led_platform_data pdata;
	struct gpio_led *p;
	int err;

	p = kmalloc(num_leds * sizeof(*p), GFP_KERNEL);
	if (!p)
		return;

	memcpy(p, leds, num_leds * sizeof(*p));

	pdev = platform_device_alloc("leds-gpio", id);
	if (!pdev)
		goto err_free_leds;

	memset(&pdata, 0, sizeof(pdata));
	pdata.num_leds = num_leds;
	pdata.leds = p;

	err = platform_device_add_data(pdev, &pdata, sizeof(pdata));
	if (err)
		goto err_put_pdev;

	err = platform_device_add(pdev);
	if (err)
		goto err_put_pdev;

	return;

err_put_pdev:
	platform_device_put(pdev);

err_free_leds:
	kfree(p);
}

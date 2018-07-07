/*
 *  Realtek boards' GPIO button support
 *
 *  Copyright (C) 2008-2010 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2017 Weijie Gao <hackpascal@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include "linux/init.h"
#include "linux/slab.h"
#include <linux/platform_device.h>

#include "dev-gpio-buttons.h"

void __init realtek_register_gpio_keys_polled(int id,
					      unsigned poll_interval,
					      unsigned nbuttons,
					      struct gpio_keys_button *buttons)
{
	struct platform_device *pdev;
	struct gpio_keys_platform_data pdata;
	struct gpio_keys_button *p;
	int err;

	p = kmemdup(buttons, nbuttons * sizeof(*p), GFP_KERNEL);
	if (!p)
		return;

	pdev = platform_device_alloc("gpio-keys-polled", id);
	if (!pdev)
		goto err_free_buttons;

	memset(&pdata, 0, sizeof(pdata));
	pdata.poll_interval = poll_interval;
	pdata.nbuttons = nbuttons;
	pdata.buttons = p;

	err = platform_device_add_data(pdev, &pdata, sizeof(pdata));
	if (err)
		goto err_put_pdev;

	err = platform_device_add(pdev);
	if (err)
		goto err_put_pdev;

	return;

err_put_pdev:
	platform_device_put(pdev);

err_free_buttons:
	kfree(p);
}

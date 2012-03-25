/*
 *  Lantiq GPIO button support
 *
 *  Copyright (C) 2008-2009 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#ifndef _LANTIQ_DEV_GPIO_BUTTONS_H
#define _LANTIQ_DEV_GPIO_BUTTONS_H

#include <linux/input.h>
#include <linux/gpio_keys.h>

#define LTQ_KEYS_POLL_INTERVAL		20 /* msecs */
#define LTQ_KEYS_DEBOUNCE_INTERVAL	(3 * LTQ_KEYS_POLL_INTERVAL)

void ltq_register_gpio_keys_polled(int id,
				      unsigned poll_interval,
				      unsigned nbuttons,
				      struct gpio_keys_button *buttons);

#endif /* _LANTIQ_DEV_GPIO_BUTTONS_H */

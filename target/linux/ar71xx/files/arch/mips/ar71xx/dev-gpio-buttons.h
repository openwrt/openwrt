/*
 *  Atheros AR71xx GPIO button support
 *
 *  Copyright (C) 2008-2009 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#ifndef _AR71XX_DEV_GPIO_BUTTONS_H
#define _AR71XX_DEV_GPIO_BUTTONS_H

#include <linux/input.h>
#include <linux/gpio_keys.h>

void ar71xx_register_gpio_keys_polled(int id,
				      unsigned poll_interval,
				      unsigned nbuttons,
				      struct gpio_keys_button *buttons);

#endif /* _AR71XX_DEV_GPIO_BUTTONS_H */

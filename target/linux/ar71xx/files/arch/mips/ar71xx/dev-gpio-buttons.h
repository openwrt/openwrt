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
#include <linux/gpio_buttons.h>

#include <asm/mach-ar71xx/platform.h>

void ar71xx_add_device_gpio_buttons(int id,
				    unsigned poll_interval,
				    unsigned nbuttons,
				    struct gpio_button *buttons) __init;

#endif /* _AR71XX_DEV_GPIO_BUTTONS_H */

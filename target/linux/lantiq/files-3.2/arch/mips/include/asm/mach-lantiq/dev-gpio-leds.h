/*
 *  Lantiq GPIO LED device support
 *
 *  Copyright (C) 2008-2009 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#ifndef _LANTIQ_DEV_LEDS_GPIO_H
#define _LANTIQ_DEV_LEDS_GPIO_H

#include <linux/leds.h>

void ltq_add_device_gpio_leds(int id,
	unsigned num_leds,
	struct gpio_led *leds) __init;

#endif /* _LANTIQ_DEV_LEDS_GPIO_H */

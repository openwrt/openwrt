/*
 *  Ralink SoC GPIO button support
 *
 *  Copyright (C) 2010 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#ifndef __ASM_MACH_RALINK_DEV_GPIO_BUTTONS_H
#define __ASM_MACH_RALINK_DEV_GPIO_BUTTONS_H

#include <linux/input.h>
#include <linux/gpio_buttons.h>

#ifdef CONFIG_RALINK_DEV_GPIO_BUTTONS
void __init ramips_register_gpio_buttons(int id,
					 unsigned poll_interval,
					 unsigned nbuttons,
					 struct gpio_button *buttons);
#else
static inline void ramips_register_gpio_buttons(int id,
						unsigned poll_interval,
						unsigned nbuttons,
						struct gpio_button *buttons);
{
}
#endif

#endif /* __ASM_MACH_RALINK_DEV_GPIO_BUTTONS_H */

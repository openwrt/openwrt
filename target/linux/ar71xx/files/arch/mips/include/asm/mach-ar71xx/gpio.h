/*
 *  Atheros AR71xx GPIO API definitions
 *
 *  Copyright (C) 2008 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 */

#ifndef __ASM_MACH_AR71XX_GPIO_H
#define __ASM_MACH_AR71XX_GPIO_H

#define ARCH_NR_GPIOS	64
#include <asm-generic/gpio.h>

extern unsigned long ar71xx_gpio_count;
extern void __ar71xx_gpio_set_value(unsigned gpio, int value);
extern int __ar71xx_gpio_get_value(unsigned gpio);
int gpio_to_irq(unsigned gpio);
int irq_to_gpio(unsigned gpio);

static inline int gpio_get_value(unsigned gpio)
{
	if (gpio < ar71xx_gpio_count)
		return __ar71xx_gpio_get_value(gpio);

	return __gpio_get_value(gpio);
}

static inline void gpio_set_value(unsigned gpio, int value)
{
	if (gpio < ar71xx_gpio_count)
		__ar71xx_gpio_set_value(gpio, value);
	else
		__gpio_set_value(gpio, value);
}

#define gpio_cansleep	__gpio_cansleep

#endif /* __ASM_MACH_AR71XX_GPIO_H */

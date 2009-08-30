/*
 *  Ralink RT288x GPIO API definitions
 *
 *  Copyright (C) 2008-2009 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 */

#ifndef __ASM_MACH_RT288X_GPIO_H
#define __ASM_MACH_RT288X_GPIO_H

#define ARCH_NR_GPIOS	64
#include <asm-generic/gpio.h>

#include <asm/mach-ralink/rt288x.h>

extern void __rt288x_gpio_set_value(unsigned gpio, int value);
extern int __rt288x_gpio_get_value(unsigned gpio);

static inline int gpio_to_irq(unsigned gpio)
{
	return RT288X_GPIO_IRQ(gpio);
}

static inline int irq_to_gpio(unsigned irq)
{
	return irq - RT288X_GPIO_IRQ_BASE;
}

static inline int gpio_get_value(unsigned gpio)
{
	if (gpio < RT288X_GPIO_COUNT)
		return __rt288x_gpio_get_value(gpio);

	return __gpio_get_value(gpio);
}

static inline void gpio_set_value(unsigned gpio, int value)
{
	if (gpio < RT288X_GPIO_COUNT)
		__rt288x_gpio_set_value(gpio, value);
	else
		__gpio_set_value(gpio, value);
}

#define gpio_cansleep	__gpio_cansleep

#endif /* __ASM_MACH_RT288X_GPIO_H */

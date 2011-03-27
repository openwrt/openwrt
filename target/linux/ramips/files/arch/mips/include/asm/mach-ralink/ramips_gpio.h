/*
 * Ralink SoC specific GPIO support
 *
 * Copyright (C) 2011 Gabor Juhos <juhosg@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#ifndef _RAMIPS_GPIO_H
#define _RAMIPS_GPIO_H

#include <linux/gpio.h>
#include <linux/spinlock.h>

enum ramips_gpio_reg {
	RAMIPS_GPIO_REG_INT = 0,	/* Interrupt status */
	RAMIPS_GPIO_REG_EDGE,
	RAMIPS_GPIO_REG_RENA,
	RAMIPS_GPIO_REG_FENA,
	RAMIPS_GPIO_REG_DATA,
	RAMIPS_GPIO_REG_DIR,		/* Direction, 0:in, 1: out */
	RAMIPS_GPIO_REG_POL,		/* Polarity, 0: normal, 1: invert */
	RAMIPS_GPIO_REG_SET,
	RAMIPS_GPIO_REG_RESET,
	RAMIPS_GPIO_REG_TOGGLE,
	RAMIPS_GPIO_REG_MAX
};

struct ramips_gpio_chip {
	struct gpio_chip chip;
	unsigned long map_base;
	unsigned long map_size;
	u8 regs[RAMIPS_GPIO_REG_MAX];

	spinlock_t lock;
	void __iomem *regs_base;
};

struct ramips_gpio_data {
	unsigned int num_chips;
	struct ramips_gpio_chip *chips;
};

int ramips_gpio_init(struct ramips_gpio_data *data);

#endif /* _RAMIPS_GPIO_H */

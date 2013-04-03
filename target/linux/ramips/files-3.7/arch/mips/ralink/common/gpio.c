/*
 * Ralink SoC specific GPIO support
 *
 * Copyright (C) 2009-2011 Gabor Juhos <juhosg@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#include <linux/init.h>
#include <linux/io.h>

#include <asm/mach-ralink/ramips_gpio.h>

static inline struct ramips_gpio_chip *to_ramips_gpio(struct gpio_chip *chip)
{
	struct ramips_gpio_chip *rg;

	rg = container_of(chip, struct ramips_gpio_chip, chip);
	return rg;
}

static inline void ramips_gpio_wr(struct ramips_gpio_chip *rg, u8 reg, u32 val)
{
	__raw_writel(val, rg->regs_base + rg->regs[reg]);
}

static inline u32 ramips_gpio_rr(struct ramips_gpio_chip *rg, u8 reg)
{
	return __raw_readl(rg->regs_base + rg->regs[reg]);
}

static int ramips_gpio_direction_input(struct gpio_chip *chip, unsigned offset)
{
	struct ramips_gpio_chip *rg = to_ramips_gpio(chip);
	unsigned long flags;
	u32 t;

	spin_lock_irqsave(&rg->lock, flags);
	t = ramips_gpio_rr(rg, RAMIPS_GPIO_REG_DIR);
	t &= ~(1 << offset);
	ramips_gpio_wr(rg, RAMIPS_GPIO_REG_DIR, t);
	spin_unlock_irqrestore(&rg->lock, flags);

	return 0;
}

static int ramips_gpio_direction_output(struct gpio_chip *chip,
					unsigned offset, int value)
{
	struct ramips_gpio_chip *rg = to_ramips_gpio(chip);
	unsigned long flags;
	u32 reg;
	u32 t;

	reg = (value) ? RAMIPS_GPIO_REG_SET : RAMIPS_GPIO_REG_RESET;

	spin_lock_irqsave(&rg->lock, flags);
	ramips_gpio_wr(rg, reg, 1 << offset);

	t = ramips_gpio_rr(rg, RAMIPS_GPIO_REG_DIR);
	t |= 1 << offset;
	ramips_gpio_wr(rg, RAMIPS_GPIO_REG_DIR, t);
	spin_unlock_irqrestore(&rg->lock, flags);

	return 0;
}

static void ramips_gpio_set(struct gpio_chip *chip, unsigned offset, int value)
{
	struct ramips_gpio_chip *rg = to_ramips_gpio(chip);
	u32 reg;

	reg = (value) ? RAMIPS_GPIO_REG_SET : RAMIPS_GPIO_REG_RESET;
	ramips_gpio_wr(rg, reg, 1 << offset);
}

static int ramips_gpio_get(struct gpio_chip *chip, unsigned offset)
{
	struct ramips_gpio_chip *rg = to_ramips_gpio(chip);
	u32 t;

	t = ramips_gpio_rr(rg, RAMIPS_GPIO_REG_DATA);
	return !!(t & (1 << offset));
}

static __init void ramips_gpio_chip_add(struct ramips_gpio_chip *rg)
{
	spin_lock_init(&rg->lock);

	rg->regs_base = ioremap(rg->map_base, rg->map_size);

	rg->chip.direction_input = ramips_gpio_direction_input;
	rg->chip.direction_output = ramips_gpio_direction_output;
	rg->chip.get = ramips_gpio_get;
	rg->chip.set = ramips_gpio_set;

	/* set polarity to low for all lines */
	ramips_gpio_wr(rg, RAMIPS_GPIO_REG_POL, 0);

	gpiochip_add(&rg->chip);
}

__init int ramips_gpio_init(struct ramips_gpio_data *data)
{
	int i;

	for (i = 0; i < data->num_chips; i++)
		ramips_gpio_chip_add(&data->chips[i]);

	return 0;
}

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
#include <linux/spinlock.h>
#include <linux/gpio.h>

#include <ralink_soc.h>

#define GPIO0_REG_INT		0x00
#define GPIO0_REG_EDGE		0x04
#define GPIO0_REG_RENA		0x08
#define GPIO0_REG_FENA		0x0c
#define GPIO0_REG_DATA		0x20
#define GPIO0_REG_DIR		0x24
#define GPIO0_REG_POL		0x28
#define GPIO0_REG_SET		0x2c
#define GPIO0_REG_RESET		0x30
#define GPIO0_REG_TOGGLE	0x34

#define GPIO1_REG_INT		0x38
#define GPIO1_REG_EDGE		0x3c
#define GPIO1_REG_RENA		0x40
#define GPIO1_REG_FENA		0x44
#define GPIO1_REG_DATA		0x48
#define GPIO1_REG_DIR		0x4c
#define GPIO1_REG_POL		0x50
#define GPIO1_REG_SET		0x54
#define GPIO1_REG_RESET		0x58
#define GPIO1_REG_TOGGLE	0x5c

#define GPIO2_REG_INT		0x60
#define GPIO2_REG_EDGE		0x64
#define GPIO2_REG_RENA		0x68
#define GPIO2_REG_FENA		0x6c
#define GPIO2_REG_DATA		0x70
#define GPIO2_REG_DIR		0x74
#define GPIO2_REG_POL		0x78
#define GPIO2_REG_SET		0x7c
#define GPIO2_REG_RESET		0x80
#define GPIO2_REG_TOGGLE	0x84

enum ramips_pio_reg {
	RAMIPS_GPIO_REG_INT,		/* Interrupt status */
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
	struct gpio_chip	chip;
	u8			regs[RAMIPS_GPIO_REG_MAX];
	unsigned long		map_base;
	unsigned long		map_size;

	spinlock_t		lock;
	void __iomem		*regs_base;
};

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

static struct ramips_gpio_chip ramips_gpio_chip0 = {
	.chip = {
		.label			= "ramips-gpio0",
		.base			= 0,
		.ngpio			= RALINK_SOC_GPIO0_COUNT,
	},
	.regs = {
		[RAMIPS_GPIO_REG_INT]	= GPIO0_REG_INT,
		[RAMIPS_GPIO_REG_EDGE]	= GPIO0_REG_EDGE,
		[RAMIPS_GPIO_REG_RENA]	= GPIO0_REG_RENA,
		[RAMIPS_GPIO_REG_FENA]	= GPIO0_REG_FENA,
		[RAMIPS_GPIO_REG_DATA]	= GPIO0_REG_DATA,
		[RAMIPS_GPIO_REG_DIR]	= GPIO0_REG_DIR,
		[RAMIPS_GPIO_REG_POL]	= GPIO0_REG_POL,
		[RAMIPS_GPIO_REG_SET]	= GPIO0_REG_SET,
		[RAMIPS_GPIO_REG_RESET]	= GPIO0_REG_RESET,
		[RAMIPS_GPIO_REG_TOGGLE] = GPIO0_REG_TOGGLE,
	},
	.map_base	= RALINK_SOC_GPIO_BASE,
	.map_size	= PAGE_SIZE,
};

static struct ramips_gpio_chip ramips_gpio_chip1 = {
	.chip = {
		.label			= "ramips-gpio1",
		.base			= 32,
		.ngpio			= RALINK_SOC_GPIO1_COUNT,
	},
	.regs = {
		[RAMIPS_GPIO_REG_INT]	= GPIO1_REG_INT,
		[RAMIPS_GPIO_REG_EDGE]	= GPIO1_REG_EDGE,
		[RAMIPS_GPIO_REG_RENA]	= GPIO1_REG_RENA,
		[RAMIPS_GPIO_REG_FENA]	= GPIO1_REG_FENA,
		[RAMIPS_GPIO_REG_DATA]	= GPIO1_REG_DATA,
		[RAMIPS_GPIO_REG_DIR]	= GPIO1_REG_DIR,
		[RAMIPS_GPIO_REG_POL]	= GPIO1_REG_POL,
		[RAMIPS_GPIO_REG_SET]	= GPIO1_REG_SET,
		[RAMIPS_GPIO_REG_RESET]	= GPIO1_REG_RESET,
		[RAMIPS_GPIO_REG_TOGGLE] = GPIO1_REG_TOGGLE,
	},
	.map_base	= RALINK_SOC_GPIO_BASE,
	.map_size	= PAGE_SIZE,
};

static struct ramips_gpio_chip ramips_gpio_chip2 = {
	.chip = {
		.label			= "ramips-gpio2",
		.base			= 64,
		.ngpio			= RALINK_SOC_GPIO2_COUNT,
	},
	.regs = {
		[RAMIPS_GPIO_REG_INT]	= GPIO2_REG_INT,
		[RAMIPS_GPIO_REG_EDGE]	= GPIO2_REG_EDGE,
		[RAMIPS_GPIO_REG_RENA]	= GPIO2_REG_RENA,
		[RAMIPS_GPIO_REG_FENA]	= GPIO2_REG_FENA,
		[RAMIPS_GPIO_REG_DATA]	= GPIO2_REG_DATA,
		[RAMIPS_GPIO_REG_DIR]	= GPIO2_REG_DIR,
		[RAMIPS_GPIO_REG_POL]	= GPIO2_REG_POL,
		[RAMIPS_GPIO_REG_SET]	= GPIO2_REG_SET,
		[RAMIPS_GPIO_REG_RESET]	= GPIO2_REG_RESET,
		[RAMIPS_GPIO_REG_TOGGLE] = GPIO2_REG_TOGGLE,
	},
	.map_base	= RALINK_SOC_GPIO_BASE,
	.map_size	= PAGE_SIZE,
};

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

__init int ramips_gpio_init(void)
{
	ramips_gpio_chip_add(&ramips_gpio_chip0);
	ramips_gpio_chip_add(&ramips_gpio_chip1);
	ramips_gpio_chip_add(&ramips_gpio_chip2);

	return 0;
}

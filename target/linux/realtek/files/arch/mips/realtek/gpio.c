/*
 *  Realtek RLX based SoC GPIO device
 *
 *  Copyright (C) 2017 Weijie Gao <hackpascal@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/err.h>
#include <linux/gpio/driver.h>

#include <asm/mach-realtek/realtek.h>
#include <asm/mach-realtek/platform.h>
#include "common.h"

struct realtek_gpio_chip {
	struct gpio_chip chip;
	void __iomem *base;
	spinlock_t lock;
};

#define get_realtek_gpio_chip(c) container_of(c, struct realtek_gpio_chip, chip)

static void realtek_gpio_set_value(struct gpio_chip *chip,
				   unsigned gpio, int value)
{
	struct realtek_gpio_chip *ctrl = get_realtek_gpio_chip(chip);
	unsigned long flags;
	unsigned long gpio_data;

	spin_lock_irqsave(&ctrl->lock, flags);

	gpio_data = __raw_readl(ctrl->base + REALTEK_GPIO_REG_DATA);

	if (value)
		gpio_data |= BIT(gpio);
	else
		gpio_data &= ~BIT(gpio);

	__raw_writel(gpio_data, ctrl->base + REALTEK_GPIO_REG_DATA);

	spin_unlock_irqrestore(&ctrl->lock, flags);
}

static int realtek_gpio_get_value(struct gpio_chip *chip, unsigned gpio)
{
	struct realtek_gpio_chip *ctrl = get_realtek_gpio_chip(chip);

	return (__raw_readl(ctrl->base + REALTEK_GPIO_REG_DATA) >> gpio) & 0x1;
}

static int realtek_gpio_direction_input(struct gpio_chip *chip,
				        unsigned offset)
{
	struct realtek_gpio_chip *ctrl = get_realtek_gpio_chip(chip);
	unsigned long flags;
	unsigned long gpio_dir;

	spin_lock_irqsave(&ctrl->lock, flags);

	gpio_dir = __raw_readl(ctrl->base + REALTEK_GPIO_REG_DIR);
	gpio_dir &= ~BIT(offset);
	__raw_writel(gpio_dir, ctrl->base + REALTEK_GPIO_REG_DIR);

	spin_unlock_irqrestore(&ctrl->lock, flags);

	return 0;
}

static int realtek_gpio_direction_output(struct gpio_chip *chip,
					 unsigned offset, int value)
{
	struct realtek_gpio_chip *ctrl = get_realtek_gpio_chip(chip);
	unsigned long flags;
	unsigned long gpio_dir, gpio_data;

	spin_lock_irqsave(&ctrl->lock, flags);

	gpio_dir = __raw_readl(ctrl->base + REALTEK_GPIO_REG_DIR);
	gpio_dir |= BIT(offset);
	__raw_writel(gpio_dir, ctrl->base + REALTEK_GPIO_REG_DIR);

	gpio_data = __raw_readl(ctrl->base + REALTEK_GPIO_REG_DATA);

	if (value)
		gpio_data |= BIT(offset);
	else
		gpio_data &= ~BIT(offset);

	__raw_writel(gpio_data, ctrl->base + REALTEK_GPIO_REG_DATA);

	spin_unlock_irqrestore(&ctrl->lock, flags);

	return 0;
}

static struct realtek_gpio_chip realtek_gpio_abcd = {
	.chip = {
		.label			= "realtek-abcd",
		.get			= realtek_gpio_get_value,
		.set			= realtek_gpio_set_value,
		.direction_input	= realtek_gpio_direction_input,
		.direction_output	= realtek_gpio_direction_output,
		.base			= 0,
	},
};

static struct realtek_gpio_chip realtek_gpio_efgh = {
	.chip = {
		.label			= "realtek-efgh",
		.get			= realtek_gpio_get_value,
		.set			= realtek_gpio_set_value,
		.direction_input	= realtek_gpio_direction_input,
		.direction_output	= realtek_gpio_direction_output,
		.base			= 32,
	},
};

void __init realtek_set_gpio_control(u32 gpio, bool soft_ctrl)
{
	unsigned long val;
	void __iomem *reg;

	if (gpio < 32) {
		reg = realtek_gpio_abcd.base;
	} else if (gpio < 64) {
		reg = realtek_gpio_efgh.base;
		gpio -= 32;
	} else
		return;

	val = __raw_readl(reg + REALTEK_GPIO_REG_CTRL);

	if (soft_ctrl)
		val &= ~BIT(gpio);
	else
		val |= BIT(gpio);

	__raw_writel(val, reg + REALTEK_GPIO_REG_CTRL);
}

void __init realtek_set_gpio_mux(u32 clear, u32 set)
{
	unsigned long val;

	val = realtek_sys_read(REALTEK_SYS_REG_GPIO_MUX);
	val &= ~clear;
	val |= set;
	realtek_sys_write(REALTEK_SYS_REG_GPIO_MUX, val);
}

void __init realtek_gpio_init(void)
{
	realtek_gpio_abcd.base = ioremap_nocache(REALTEK_GPIO_ABCD_BASE, REALTEK_GPIO_ABCD_SIZE);
	realtek_gpio_efgh.base = ioremap_nocache(REALTEK_GPIO_EFGH_BASE, REALTEK_GPIO_EFGH_SIZE);

	spin_lock_init(&realtek_gpio_abcd.lock);

	if (soc_is_rtl8196c())
		realtek_gpio_abcd.chip.ngpio = 17;
	else
		BUG();

	if (realtek_gpio_abcd.chip.ngpio)
		gpiochip_add(&realtek_gpio_abcd.chip);

	if (realtek_gpio_efgh.chip.ngpio)
		gpiochip_add(&realtek_gpio_efgh.chip);
}

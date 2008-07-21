/*
 *  Atheros AR71xx SoC GPIO API support
 *
 *  Copyright (C) 2008 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/spinlock.h>
#include <linux/io.h>
#include <linux/ioport.h>
#include <linux/gpio.h>

#include <asm/mach-ar71xx/ar71xx.h>

static DEFINE_SPINLOCK(ar71xx_gpio_lock);

void __ar71xx_gpio_set_value(unsigned gpio, int value)
{
	unsigned long flags;

	spin_lock_irqsave(&ar71xx_gpio_lock, flags);

	if (value)
		ar71xx_gpio_wr(GPIO_REG_SET, (1 << gpio));
	else
		ar71xx_gpio_wr(GPIO_REG_CLEAR, (1 << gpio));

	spin_unlock_irqrestore(&ar71xx_gpio_lock, flags);
}
EXPORT_SYMBOL(__ar71xx_gpio_set_value);

int __ar71xx_gpio_get_value(unsigned gpio)
{
	return (ar71xx_gpio_rr(GPIO_REG_IN) & (1 << gpio)) ? 1 : 0;
}
EXPORT_SYMBOL(__ar71xx_gpio_get_value);

static int ar71xx_gpio_get_value(struct gpio_chip *chip, unsigned offset)
{
	return __ar71xx_gpio_get_value(offset);
}

static void ar71xx_gpio_set_value(struct gpio_chip *chip,
				  unsigned offset, int value)
{
	__ar71xx_gpio_set_value(offset, value);
}

static int ar71xx_gpio_direction_input(struct gpio_chip *chip,
				       unsigned offset)
{
	unsigned long flags;

	spin_lock_irqsave(&ar71xx_gpio_lock, flags);

	ar71xx_gpio_wr(GPIO_REG_OE,
			ar71xx_gpio_rr(GPIO_REG_OE) & ~(1 << offset));

	spin_unlock_irqrestore(&ar71xx_gpio_lock, flags);

	return 0;
}

static int ar71xx_gpio_direction_output(struct gpio_chip *chip,
					unsigned offset, int value)
{
	unsigned long flags;

	spin_lock_irqsave(&ar71xx_gpio_lock, flags);

	if (value)
		ar71xx_gpio_wr(GPIO_REG_SET, (1 << offset));
	else
		ar71xx_gpio_wr(GPIO_REG_CLEAR, (1 << offset));

	ar71xx_gpio_wr(GPIO_REG_OE,
			ar71xx_gpio_rr(GPIO_REG_OE) | (1 << offset));

	spin_unlock_irqrestore(&ar71xx_gpio_lock, flags);

	return 0;
}

static struct gpio_chip ar71xx_gpio_chip = {
	.label			= "ar71xx",
	.get			= ar71xx_gpio_get_value,
	.set			= ar71xx_gpio_set_value,
	.direction_input	= ar71xx_gpio_direction_input,
	.direction_output	= ar71xx_gpio_direction_output,
	.base			= 0,
	.ngpio			= AR71XX_GPIO_COUNT,
};

void ar71xx_gpio_function_enable(u32 mask)
{
	unsigned long flags;

	spin_lock_irqsave(&ar71xx_gpio_lock, flags);

	ar71xx_gpio_wr(GPIO_REG_FUNC, ar71xx_gpio_rr(GPIO_REG_FUNC) | mask);

	spin_unlock_irqrestore(&ar71xx_gpio_lock, flags);
}

void ar71xx_gpio_function_disable(u32 mask)
{
	unsigned long flags;

	spin_lock_irqsave(&ar71xx_gpio_lock, flags);

	ar71xx_gpio_wr(GPIO_REG_FUNC, ar71xx_gpio_rr(GPIO_REG_FUNC) & ~mask);

	spin_unlock_irqrestore(&ar71xx_gpio_lock, flags);
}

void __init ar71xx_gpio_init(void)
{
	int err;

	if (!request_mem_region(AR71XX_GPIO_BASE, AR71XX_GPIO_SIZE,
				"AR71xx GPIO controller"))
		panic("cannot allocate AR71xx GPIO registers page");

	err = gpiochip_add(&ar71xx_gpio_chip);
	if (err)
		panic("cannot add AR71xx GPIO chip, error=%d", err);
}

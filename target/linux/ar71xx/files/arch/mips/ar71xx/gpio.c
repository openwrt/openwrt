/*
 *  Atheros AR7XXX/AR9XXX SoC GPIO API support
 *
 *  Copyright (C) 2008-2011 Gabor Juhos <juhosg@openwrt.org>
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

unsigned long ar71xx_gpio_count;
EXPORT_SYMBOL(ar71xx_gpio_count);

void __ar71xx_gpio_set_value(unsigned gpio, int value)
{
	void __iomem *base = ar71xx_gpio_base;

	if (value)
		__raw_writel(1 << gpio, base + AR71XX_GPIO_REG_SET);
	else
		__raw_writel(1 << gpio, base + AR71XX_GPIO_REG_CLEAR);
}
EXPORT_SYMBOL(__ar71xx_gpio_set_value);

int __ar71xx_gpio_get_value(unsigned gpio)
{
	return (__raw_readl(ar71xx_gpio_base + AR71XX_GPIO_REG_IN) >> gpio) & 1;
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
	void __iomem *base = ar71xx_gpio_base;
	unsigned long flags;

	spin_lock_irqsave(&ar71xx_gpio_lock, flags);

	__raw_writel(__raw_readl(base + AR71XX_GPIO_REG_OE) & ~(1 << offset),
		     base + AR71XX_GPIO_REG_OE);

	spin_unlock_irqrestore(&ar71xx_gpio_lock, flags);

	return 0;
}

static int ar71xx_gpio_direction_output(struct gpio_chip *chip,
					unsigned offset, int value)
{
	void __iomem *base = ar71xx_gpio_base;
	unsigned long flags;

	spin_lock_irqsave(&ar71xx_gpio_lock, flags);

	if (value)
		__raw_writel(1 << offset, base + AR71XX_GPIO_REG_SET);
	else
		__raw_writel(1 << offset, base + AR71XX_GPIO_REG_CLEAR);

	__raw_writel(__raw_readl(base + AR71XX_GPIO_REG_OE) | (1 << offset),
		     base + AR71XX_GPIO_REG_OE);

	spin_unlock_irqrestore(&ar71xx_gpio_lock, flags);

	return 0;
}

static int ar934x_gpio_direction_input(struct gpio_chip *chip,
				       unsigned offset)
{
	void __iomem *base = ar71xx_gpio_base;
	unsigned long flags;

	spin_lock_irqsave(&ar71xx_gpio_lock, flags);

	__raw_writel(__raw_readl(base + AR71XX_GPIO_REG_OE) | (1 << offset),
		     base + AR71XX_GPIO_REG_OE);

	spin_unlock_irqrestore(&ar71xx_gpio_lock, flags);

	return 0;
}

static int ar934x_gpio_direction_output(struct gpio_chip *chip,
					unsigned offset, int value)
{
	void __iomem *base = ar71xx_gpio_base;
	unsigned long flags;

	spin_lock_irqsave(&ar71xx_gpio_lock, flags);

	if (value)
		__raw_writel(1 << offset, base + AR71XX_GPIO_REG_SET);
	else
		__raw_writel(1 << offset, base + AR71XX_GPIO_REG_CLEAR);

	__raw_writel(__raw_readl(base + AR71XX_GPIO_REG_OE) & ~(1 << offset),
		     base + AR71XX_GPIO_REG_OE);

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
	void __iomem *base = ar71xx_gpio_base;
	unsigned long flags;
	unsigned int reg;

	if (ar71xx_soc == AR71XX_SOC_AR9341 ||
	    ar71xx_soc == AR71XX_SOC_AR9342 ||
	    ar71xx_soc == AR71XX_SOC_AR9344) {
		reg = AR934X_GPIO_REG_FUNC;
	} else {
		reg = AR71XX_GPIO_REG_FUNC;
	}

	spin_lock_irqsave(&ar71xx_gpio_lock, flags);

	__raw_writel(__raw_readl(base + reg) | mask, base + reg);
	/* flush write */
	(void) __raw_readl(base + reg);

	spin_unlock_irqrestore(&ar71xx_gpio_lock, flags);
}

void ar71xx_gpio_function_disable(u32 mask)
{
	void __iomem *base = ar71xx_gpio_base;
	unsigned long flags;
	unsigned int reg;

	if (ar71xx_soc == AR71XX_SOC_AR9341 ||
	    ar71xx_soc == AR71XX_SOC_AR9342 ||
	    ar71xx_soc == AR71XX_SOC_AR9344) {
		reg = AR934X_GPIO_REG_FUNC;
	} else {
		reg = AR71XX_GPIO_REG_FUNC;
	}

	spin_lock_irqsave(&ar71xx_gpio_lock, flags);

	__raw_writel(__raw_readl(base + reg) & ~mask, base + reg);
	/* flush write */
	(void) __raw_readl(base + reg);

	spin_unlock_irqrestore(&ar71xx_gpio_lock, flags);
}

void ar71xx_gpio_function_setup(u32 set, u32 clear)
{
	void __iomem *base = ar71xx_gpio_base;
	unsigned long flags;
	unsigned int reg;

	if (ar71xx_soc == AR71XX_SOC_AR9341 ||
	    ar71xx_soc == AR71XX_SOC_AR9342 ||
	    ar71xx_soc == AR71XX_SOC_AR9344) {
		reg = AR934X_GPIO_REG_FUNC;
	} else {
		reg = AR71XX_GPIO_REG_FUNC;
	}

	spin_lock_irqsave(&ar71xx_gpio_lock, flags);

	__raw_writel((__raw_readl(base + reg) & ~clear) | set, base + reg);
	/* flush write */
	(void) __raw_readl(base + reg);

	spin_unlock_irqrestore(&ar71xx_gpio_lock, flags);
}
EXPORT_SYMBOL(ar71xx_gpio_function_setup);

void __init ar71xx_gpio_output_select(unsigned gpio, u8 val)
{
	void __iomem *base = ar71xx_gpio_base;
	unsigned long flags;
	unsigned int reg;
	u32 t, s;

	if (ar71xx_soc != AR71XX_SOC_AR9341 &&
	    ar71xx_soc != AR71XX_SOC_AR9342 &&
	    ar71xx_soc != AR71XX_SOC_AR9344)
		return;

	if (gpio >= AR934X_GPIO_COUNT)
		return;

	reg = AR934X_GPIO_REG_OUT_FUNC0 + 4 * (gpio / 4);
	s = 8 * (gpio % 4);

	spin_lock_irqsave(&ar71xx_gpio_lock, flags);

	t = __raw_readl(base + reg);
	t &= ~(0xff << s);
	t |= val << s;
	__raw_writel(t, base + reg);

	/* flush write */
	(void) __raw_readl(base + reg);

	spin_unlock_irqrestore(&ar71xx_gpio_lock, flags);
}

void __init ar71xx_gpio_init(void)
{
	int err;

	if (!request_mem_region(AR71XX_GPIO_BASE, AR71XX_GPIO_SIZE,
				"AR71xx GPIO controller"))
		panic("cannot allocate AR71xx GPIO registers page");

	switch (ar71xx_soc) {
	case AR71XX_SOC_AR7130:
	case AR71XX_SOC_AR7141:
	case AR71XX_SOC_AR7161:
		ar71xx_gpio_chip.ngpio = AR71XX_GPIO_COUNT;
		break;

	case AR71XX_SOC_AR7240:
		ar71xx_gpio_chip.ngpio = AR7240_GPIO_COUNT;
		break;

	case AR71XX_SOC_AR7241:
	case AR71XX_SOC_AR7242:
		ar71xx_gpio_chip.ngpio = AR7241_GPIO_COUNT;
		break;

	case AR71XX_SOC_AR9130:
	case AR71XX_SOC_AR9132:
		ar71xx_gpio_chip.ngpio = AR91XX_GPIO_COUNT;
		break;

	case AR71XX_SOC_AR9330:
	case AR71XX_SOC_AR9331:
		ar71xx_gpio_chip.ngpio = AR933X_GPIO_COUNT;
		break;

	case AR71XX_SOC_AR9341:
	case AR71XX_SOC_AR9342:
	case AR71XX_SOC_AR9344:
		ar71xx_gpio_chip.ngpio = AR934X_GPIO_COUNT;
		ar71xx_gpio_chip.direction_input = ar934x_gpio_direction_input;
		ar71xx_gpio_chip.direction_output = ar934x_gpio_direction_output;
		break;

	default:
		BUG();
	}

	err = gpiochip_add(&ar71xx_gpio_chip);
	if (err)
		panic("cannot add AR71xx GPIO chip, error=%d", err);
}

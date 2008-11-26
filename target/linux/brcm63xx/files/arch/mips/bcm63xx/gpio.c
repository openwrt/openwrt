/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2008 Maxime Bizon <mbizon@freebox.fr>
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/spinlock.h>
#include <bcm63xx_cpu.h>
#include <bcm63xx_gpio.h>
#include <bcm63xx_io.h>
#include <bcm63xx_regs.h>

static DEFINE_SPINLOCK(bcm63xx_gpio_lock);

void bcm63xx_gpio_set_dataout(int gpio, int val)
{
	u32 reg;
	u32 mask;
	u32 tmp;
	unsigned long flags;

	if (gpio >= BCM63XX_GPIO_COUNT)
		BUG();

	if (gpio < 32) {
		reg = GPIO_DATA_LO_REG;
		mask = 1 << gpio;
	} else {
		reg = GPIO_DATA_HI_REG;
		mask = 1 << (gpio - 32);
	}

	spin_lock_irqsave(&bcm63xx_gpio_lock, flags);
	tmp = bcm_gpio_readl(reg);
	if (val)
		tmp |= mask;
	else
		tmp &= ~mask;
	bcm_gpio_writel(tmp, reg);
	spin_unlock_irqrestore(&bcm63xx_gpio_lock, flags);
}

EXPORT_SYMBOL(bcm63xx_gpio_set_dataout);

int bcm63xx_gpio_get_datain(int gpio)
{
	u32 reg;
	u32 mask;

	if (gpio >= BCM63XX_GPIO_COUNT)
		BUG();

	if (gpio < 32) {
		reg = GPIO_DATA_LO_REG;
		mask = 1 << gpio;
	} else {
		reg = GPIO_DATA_HI_REG;
		mask = 1 << (gpio - 32);
	}

	return !!(bcm_gpio_readl(reg) & mask);
}

EXPORT_SYMBOL(bcm63xx_gpio_get_datain);

void bcm63xx_gpio_set_direction(int gpio, int dir)
{
	u32 reg;
	u32 mask;
	u32 tmp;
	unsigned long flags;

	if (gpio >= BCM63XX_GPIO_COUNT)
		BUG();

	if (gpio < 32) {
		reg = GPIO_CTL_LO_REG;
		mask = 1 << gpio;
	} else {
		reg = GPIO_CTL_HI_REG;
		mask = 1 << (gpio - 32);
	}

	spin_lock_irqsave(&bcm63xx_gpio_lock, flags);
	tmp = bcm_gpio_readl(reg);
	if (dir == GPIO_DIR_IN)
		tmp &= ~mask;
	else
		tmp |= mask;
	bcm_gpio_writel(tmp, reg);
	spin_unlock_irqrestore(&bcm63xx_gpio_lock, flags);
}

EXPORT_SYMBOL(bcm63xx_gpio_set_direction);

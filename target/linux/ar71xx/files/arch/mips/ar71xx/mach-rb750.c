/*
 *  MikroTik RouterBOARD 750 support
 *
 *  Copyright (C) 2010 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <asm/mach-ar71xx/ar71xx.h>
#include <asm/mach-ar71xx/mach-rb750.h>

#include "machtype.h"
#include "dev-ap91-eth.h"

int rb750_latch_change(u32 mask_clr, u32 mask_set)
{
	static DEFINE_SPINLOCK(lock);
	static u32 latch_set = RB750_LED_BITS | RB750_LVC573_LE;
	static u32 latch_oe;
	static u32 latch_clr;
	unsigned long flags;
	u32 t;
	int ret = 0;

	spin_lock_irqsave(&lock, flags);

	if ((mask_clr & BIT(31)) != 0 &&
	    (latch_set & RB750_LVC573_LE) == 0) {
		goto unlock;
	}

	latch_set = (latch_set | mask_set) & ~mask_clr;
	latch_clr = (latch_clr | mask_clr) & ~mask_set;

	if (latch_oe == 0)
		latch_oe = __raw_readl(ar71xx_gpio_base + GPIO_REG_OE);

	if (likely(latch_set & RB750_LVC573_LE)) {
		void __iomem *base = ar71xx_gpio_base;

		t = __raw_readl(base + GPIO_REG_OE);
		t |= mask_clr | latch_oe | mask_set;

		__raw_writel(t, base + GPIO_REG_OE);
		__raw_writel(latch_clr, base + GPIO_REG_CLEAR);
		__raw_writel(latch_set, base + GPIO_REG_SET);
	} else if (mask_clr & RB750_LVC573_LE) {
		void __iomem *base = ar71xx_gpio_base;

		latch_oe = __raw_readl(base + GPIO_REG_OE);
		__raw_writel(RB750_LVC573_LE, base + GPIO_REG_CLEAR);
		/* flush write */
		__raw_readl(base + GPIO_REG_CLEAR);
	}

	ret = 1;

 unlock:
	spin_unlock_irqrestore(&lock, flags);
	return ret;
}
EXPORT_SYMBOL_GPL(rb750_latch_change);

static void __init rb750_setup(void)
{
	ap91_eth_init(NULL);
}

MIPS_MACHINE(AR71XX_MACH_RB_750, "750i", "MikroTik RouterBOARD 750",
	     rb750_setup);

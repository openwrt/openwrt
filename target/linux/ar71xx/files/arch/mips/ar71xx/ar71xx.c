/*
 *  AR71xx SoC routines
 *
 *  Copyright (C) 2008 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>

#include <asm/mach-ar71xx/ar71xx.h>

void __iomem *ar71xx_ddr_base;
EXPORT_SYMBOL_GPL(ar71xx_ddr_base);

void __iomem *ar71xx_pll_base;
EXPORT_SYMBOL_GPL(ar71xx_pll_base);

void __iomem *ar71xx_reset_base;
EXPORT_SYMBOL_GPL(ar71xx_reset_base);

void __iomem *ar71xx_gpio_base;
EXPORT_SYMBOL_GPL(ar71xx_gpio_base);

void __iomem *ar71xx_usb_ctrl_base;
EXPORT_SYMBOL_GPL(ar71xx_usb_ctrl_base);

void ar71xx_device_stop(u32 mask)
{
	unsigned long flags;

	local_irq_save(flags);
	ar71xx_reset_wr(RESET_REG_RESET_MODULE,
			ar71xx_reset_rr(RESET_REG_RESET_MODULE) | mask);
	local_irq_restore(flags);
}
EXPORT_SYMBOL_GPL(ar71xx_device_stop);

void ar71xx_device_start(u32 mask)
{
	unsigned long flags;

	local_irq_save(flags);
	ar71xx_reset_wr(RESET_REG_RESET_MODULE,
			ar71xx_reset_rr(RESET_REG_RESET_MODULE) & ~mask);
	local_irq_restore(flags);
}
EXPORT_SYMBOL_GPL(ar71xx_device_start);

void ar71xx_ddr_flush(u32 reg)
{
	ar71xx_ddr_wr(reg, 1);
	while ((ar71xx_ddr_rr(reg) & 0x1));

	ar71xx_ddr_wr(reg, 1);
	while ((ar71xx_ddr_rr(reg) & 0x1));
}
EXPORT_SYMBOL_GPL(ar71xx_ddr_flush);


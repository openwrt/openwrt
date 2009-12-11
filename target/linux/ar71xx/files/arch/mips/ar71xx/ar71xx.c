/*
 *  AR71xx SoC routines
 *
 *  Copyright (C) 2008-2009 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/mutex.h>

#include <asm/mach-ar71xx/ar71xx.h>

static DEFINE_MUTEX(ar71xx_flash_mutex);

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
	u32 mask_inv;
	u32 t;

	switch (ar71xx_soc) {
	case AR71XX_SOC_AR7130:
	case AR71XX_SOC_AR7141:
	case AR71XX_SOC_AR7161:
		local_irq_save(flags);
		t = ar71xx_reset_rr(AR71XX_RESET_REG_RESET_MODULE);
		ar71xx_reset_wr(AR71XX_RESET_REG_RESET_MODULE, t | mask);
		local_irq_restore(flags);
		break;

	case AR71XX_SOC_AR7240:
		mask_inv = mask & RESET_MODULE_USB_OHCI_DLL_7240;
		local_irq_save(flags);
		t = ar71xx_reset_rr(AR724X_RESET_REG_RESET_MODULE);
		t |= mask;
		t &= ~mask_inv;
		ar71xx_reset_wr(AR724X_RESET_REG_RESET_MODULE, t);
		local_irq_restore(flags);
		break;

	case AR71XX_SOC_AR9130:
	case AR71XX_SOC_AR9132:
		local_irq_save(flags);
		t = ar71xx_reset_rr(AR91XX_RESET_REG_RESET_MODULE);
		ar71xx_reset_wr(AR91XX_RESET_REG_RESET_MODULE, t | mask);
		local_irq_restore(flags);
		break;

	default:
		BUG();
	}
}
EXPORT_SYMBOL_GPL(ar71xx_device_stop);

void ar71xx_device_start(u32 mask)
{
	unsigned long flags;
	u32 mask_inv;
	u32 t;

	switch (ar71xx_soc) {
	case AR71XX_SOC_AR7130:
	case AR71XX_SOC_AR7141:
	case AR71XX_SOC_AR7161:
		local_irq_save(flags);
		t = ar71xx_reset_rr(AR71XX_RESET_REG_RESET_MODULE);
		ar71xx_reset_wr(AR71XX_RESET_REG_RESET_MODULE, t & ~mask);
		local_irq_restore(flags);
		break;

	case AR71XX_SOC_AR7240:
		mask_inv = mask & RESET_MODULE_USB_OHCI_DLL_7240;
		local_irq_save(flags);
		t = ar71xx_reset_rr(AR724X_RESET_REG_RESET_MODULE);
		t &= ~mask;
		t |= mask_inv;
		ar71xx_reset_wr(AR724X_RESET_REG_RESET_MODULE, t);
		local_irq_restore(flags);
		break;

	case AR71XX_SOC_AR9130:
	case AR71XX_SOC_AR9132:
		local_irq_save(flags);
		t = ar71xx_reset_rr(AR91XX_RESET_REG_RESET_MODULE);
		ar71xx_reset_wr(AR91XX_RESET_REG_RESET_MODULE, t & ~mask);
		local_irq_restore(flags);
		break;

	default:
		BUG();
	}
}
EXPORT_SYMBOL_GPL(ar71xx_device_start);

int ar71xx_device_stopped(u32 mask)
{
	unsigned long flags;
	u32 t;

	switch (ar71xx_soc) {
	case AR71XX_SOC_AR7130:
	case AR71XX_SOC_AR7141:
	case AR71XX_SOC_AR7161:
		local_irq_save(flags);
		t = ar71xx_reset_rr(AR71XX_RESET_REG_RESET_MODULE);
		local_irq_restore(flags);
		break;

	case AR71XX_SOC_AR7240:
		local_irq_save(flags);
		t = ar71xx_reset_rr(AR724X_RESET_REG_RESET_MODULE);
		local_irq_restore(flags);
		break;

	case AR71XX_SOC_AR9130:
	case AR71XX_SOC_AR9132:
		local_irq_save(flags);
		t = ar71xx_reset_rr(AR91XX_RESET_REG_RESET_MODULE);
		local_irq_restore(flags);
		break;

	default:
		BUG();
	}

	return ((t & mask) == mask);
}
EXPORT_SYMBOL_GPL(ar71xx_device_stopped);

void ar71xx_ddr_flush(u32 reg)
{
	ar71xx_ddr_wr(reg, 1);
	while ((ar71xx_ddr_rr(reg) & 0x1));

	ar71xx_ddr_wr(reg, 1);
	while ((ar71xx_ddr_rr(reg) & 0x1));
}
EXPORT_SYMBOL_GPL(ar71xx_ddr_flush);

void ar71xx_flash_acquire(void)
{
	mutex_lock(&ar71xx_flash_mutex);
}
EXPORT_SYMBOL_GPL(ar71xx_flash_acquire);

void ar71xx_flash_release(void)
{
	mutex_unlock(&ar71xx_flash_mutex);
}
EXPORT_SYMBOL_GPL(ar71xx_flash_release);

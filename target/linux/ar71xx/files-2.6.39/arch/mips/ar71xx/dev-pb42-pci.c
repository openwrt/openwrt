/*
 *  Atheros PB42 reference board PCI initialization
 *
 *  Copyright (C) 2008-2009 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 *  Parts of this file are based on Atheros' 2.6.15 BSP
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/pci.h>

#include <asm/mach-ar71xx/ar71xx.h>
#include <asm/mach-ar71xx/pci.h>

#include "dev-pb42-pci.h"

static struct ar71xx_pci_irq pb42_pci_irqs[] __initdata = {
	{
		.slot	= 0,
		.pin	= 1,
		.irq	= AR71XX_PCI_IRQ_DEV0,
	}, {
		.slot	= 1,
		.pin	= 1,
		.irq	= AR71XX_PCI_IRQ_DEV1,
	}, {
		.slot	= 2,
		.pin	= 1,
		.irq	= AR71XX_PCI_IRQ_DEV2,
	}
};

void __init pb42_pci_init(void)
{
	ar71xx_pci_init(ARRAY_SIZE(pb42_pci_irqs), pb42_pci_irqs);
}

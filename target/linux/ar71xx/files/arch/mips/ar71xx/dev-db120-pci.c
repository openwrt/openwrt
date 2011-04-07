/*
 *  Atheros db120 reference board PCI initialization
 *
 *  Copyright (C) 2010-2011 Jaiganesh Narayanan <jnarayanan@atheros.com>
 *
 *  Parts of this file are based on Atheros linux 2.6.31 BSP
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/pci.h>

#include <asm/mach-ar71xx/ar71xx.h>
#include <asm/mach-ar71xx/pci.h>

#include "dev-db120-pci.h"

static struct ar71xx_pci_irq db120_pci_irqs[] __initdata = {
	{
		.slot	= 0,
		.pin	= 1,
		.irq	= AR71XX_PCI_IRQ_DEV0,
	}
};

void __init db120_pci_init(void)
{
	ar71xx_pci_init(ARRAY_SIZE(db120_pci_irqs), db120_pci_irqs);
}

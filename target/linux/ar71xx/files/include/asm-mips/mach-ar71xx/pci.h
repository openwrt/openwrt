/*
 *  Atheros AR71xx SoC specific PCI definitions
 *
 *  Copyright (C) 2008 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#ifndef __ASM_MACH_AR71XX_PCI_H
#define __ASM_MACH_AR71XX_PCI_H

struct ar71xx_pci_irq {
	int	irq;
	u8	slot;
	u8	pin;
};

extern int (*ar71xx_pci_be_handler)(int is_fixup);
extern int (*ar71xx_pci_bios_init)(unsigned nr_irqs,
				    struct ar71xx_pci_irq *map) __initdata;

extern int ar71xx_pci_init(unsigned nr_irqs,
			   struct ar71xx_pci_irq *map) __init;

#endif /* __ASM_MACH_AR71XX_PCI_H */

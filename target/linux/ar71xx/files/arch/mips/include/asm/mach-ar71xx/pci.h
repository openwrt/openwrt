/*
 *  Atheros AR71xx SoC specific PCI definitions
 *
 *  Copyright (C) 2008-2009 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#ifndef __ASM_MACH_AR71XX_PCI_H
#define __ASM_MACH_AR71XX_PCI_H

struct pci_dev;

struct ar71xx_pci_irq {
	int	irq;
	u8	slot;
	u8	pin;
};

#ifdef CONFIG_PCI
extern int (*ar71xx_pci_plat_dev_init)(struct pci_dev *dev);
extern unsigned ar71xx_pci_nr_irqs __initdata;
extern struct ar71xx_pci_irq *ar71xx_pci_irq_map __initdata;

int ar71xx_pcibios_map_irq(const struct pci_dev *dev,
			   uint8_t slot, uint8_t pin) __init;
int ar71xx_pcibios_init(void) __init;

int ar71xx_pci_be_handler(int is_fixup);

int ar724x_pcibios_map_irq(const struct pci_dev *dev,
			   uint8_t slot, uint8_t pin) __init;
int ar724x_pcibios_init(void) __init;

int ar71xx_pci_init(unsigned nr_irqs, struct ar71xx_pci_irq *map) __init;
#else
static inline int ar71xx_pci_init(unsigned nr_irqs, struct ar71xx_pci_irq *map)
{
	return 0;
}
#endif

#endif /* __ASM_MACH_AR71XX_PCI_H */

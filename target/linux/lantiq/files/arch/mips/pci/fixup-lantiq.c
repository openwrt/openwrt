/*
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 *  Copyright (C) 2012 John Crispin <blogic@openwrt.org>
 */

#include <linux/of_irq.h>
#include <linux/of_pci.h>

int (*ltqpci_map_irq)(const struct pci_dev *dev, u8 slot, u8 pin) = NULL;
int (*ltqpci_plat_arch_init)(struct pci_dev *dev) = NULL;
int (*ltqpci_plat_dev_init)(struct pci_dev *dev) = NULL;
int *ltq_pci_irq_map;

int pcibios_plat_dev_init(struct pci_dev *dev)
{
	if (ltqpci_plat_arch_init)
		return ltqpci_plat_arch_init(dev);

	if (ltqpci_plat_dev_init)
		return ltqpci_plat_dev_init(dev);

	return 0;
}

int __init pcibios_map_irq(const struct pci_dev *dev, u8 slot, u8 pin)
{
	if (ltqpci_map_irq)
		return ltqpci_map_irq(dev, slot, pin);
	if (ltq_pci_irq_map[slot]) {
		dev_info(&dev->dev, "SLOT:%d PIN:%d IRQ:%d\n", slot, pin, ltq_pci_irq_map[slot]);
		return ltq_pci_irq_map[slot];
	}
	printk(KERN_ERR "lq_pci: trying to map irq for unknown slot %d\n",
		slot);

	return 0;
}



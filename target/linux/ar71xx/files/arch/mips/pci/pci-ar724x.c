/*
 *  Atheros AR724x PCI host controller driver
 *
 *  Copyright (C) 2009 Gabor Juhos <juhosg@openwrt.org>
 *
 *  Parts of this file are based on Atheros' 2.6.15 BSP
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/resource.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/bitops.h>
#include <linux/pci.h>
#include <linux/pci_regs.h>

#include <asm/mach-ar71xx/ar71xx.h>
#include <asm/mach-ar71xx/pci.h>

#undef DEBUG
#ifdef DEBUG
#define DBG(fmt, args...)	printk(KERN_INFO fmt, ## args)
#else
#define DBG(fmt, args...)
#endif

static void __iomem *ar724x_pci_localcfg_base;
static void __iomem *ar724x_pci_devcfg_base;
static int ar724x_pci_fixup_enable;

static DEFINE_SPINLOCK(ar724x_pci_lock);

static void ar724x_pci_read(void __iomem *base, int where, int size, u32 *value)
{
	unsigned long flags;
	u32 data;

	spin_lock_irqsave(&ar724x_pci_lock, flags);
	data = __raw_readl(base + (where & ~3));

	switch (size) {
	case 1:
		if (where & 1)
			data >>= 8;
		if (where & 2)
			data >>= 16;
		data &= 0xFF;
		break;
	case 2:
		if (where & 2)
			data >>= 16;
		data &= 0xFFFF;
		break;
	}

	*value = data;
	spin_unlock_irqrestore(&ar724x_pci_lock, flags);
}

static void ar724x_pci_write(void __iomem *base, int where, int size, u32 value)
{
	unsigned long flags;
	u32 data;
	int s;

	spin_lock_irqsave(&ar724x_pci_lock, flags);
	data = __raw_readl(base + (where & ~3));

	switch (size) {
	case 1:
		s = ((where & 3) << 3);
		data &= ~(0xFF << s);
		data |= ((value & 0xFF) << s);
		break;
	case 2:
		s = ((where & 2) << 3);
		data &= ~(0xFFFF << s);
		data |= ((value & 0xFFFF) << s);
		break;
	case 4:
		data = value;
		break;
	}

	__raw_writel(data, base + (where & ~3));
	/* flush write */
	(void)__raw_readl(base + (where & ~3));
	spin_unlock_irqrestore(&ar724x_pci_lock, flags);
}

static int ar724x_pci_read_config(struct pci_bus *bus, unsigned int devfn,
				  int where, int size, u32 *value)
{

	if (bus->number != 0 || devfn != 0)
		return PCIBIOS_DEVICE_NOT_FOUND;

	ar724x_pci_read(ar724x_pci_devcfg_base, where, size, value);

	DBG("PCI: read config: %02x:%02x.%01x/%02x:%01d, value=%08x\n",
			bus->number, PCI_SLOT(devfn), PCI_FUNC(devfn),
			where, size, *value);

	/*
	 * WAR for BAR issue - We are unable to access the PCI device space
	 * if we set the BAR with proper base address
	 */
	if ((where == 0x10) && (size == 4))
		ar724x_pci_write(ar724x_pci_devcfg_base, where, size, 0xffff);

	return PCIBIOS_SUCCESSFUL;
}

static int ar724x_pci_write_config(struct pci_bus *bus, unsigned int devfn,
				   int where, int size, u32 value)
{
	if (bus->number != 0 || devfn != 0)
		return PCIBIOS_DEVICE_NOT_FOUND;

	DBG("PCI: write config: %02x:%02x.%01x/%02x:%01d, value=%08x\n",
		bus->number, PCI_SLOT(devfn), PCI_FUNC(devfn),
		where, size, value);

	ar724x_pci_write(ar724x_pci_devcfg_base, where, size, value);

	return PCIBIOS_SUCCESSFUL;
}

static void ar724x_pci_fixup(struct pci_dev *dev)
{
	u32 t;

	if (!ar724x_pci_fixup_enable)
		return;

	if (dev->bus->number != 0 || dev->devfn != 0)
		return;

	DBG("PCI: fixup host controller %s (%04x:%04x)\n", pci_name(dev),
		dev->vendor, dev->device);

	/* setup COMMAND register */
	t = PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER | PCI_COMMAND_INVALIDATE
	  | PCI_COMMAND_PARITY | PCI_COMMAND_SERR | PCI_COMMAND_FAST_BACK;

	pci_write_config_word(dev, PCI_COMMAND, t);
}
DECLARE_PCI_FIXUP_EARLY(PCI_ANY_ID, PCI_ANY_ID, ar724x_pci_fixup);

int __init ar724x_pcibios_map_irq(const struct pci_dev *dev, uint8_t slot,
				  uint8_t pin)
{
	int irq = -1;
	int i;

	for (i = 0; i < ar71xx_pci_nr_irqs; i++) {
		struct ar71xx_pci_irq *entry;
		entry = &ar71xx_pci_irq_map[i];

		if (entry->slot == slot && entry->pin == pin) {
			irq = entry->irq;
			break;
		}
	}

	if (irq < 0)
		printk(KERN_ALERT "PCI: no irq found for pin%u@%s\n",
				pin, pci_name((struct pci_dev *)dev));
	else
		printk(KERN_INFO "PCI: mapping irq %d to pin%u@%s\n",
				irq, pin, pci_name((struct pci_dev *)dev));

	return irq;
}

static struct pci_ops ar724x_pci_ops = {
	.read	= ar724x_pci_read_config,
	.write	= ar724x_pci_write_config,
};

static struct resource ar724x_pci_io_resource = {
	.name		= "PCI IO space",
	.start		= 0,
	.end		= 0,
	.flags		= IORESOURCE_IO,
};

static struct resource ar724x_pci_mem_resource = {
	.name		= "PCI memory space",
	.start		= AR71XX_PCI_MEM_BASE,
	.end		= AR71XX_PCI_MEM_BASE + AR71XX_PCI_MEM_SIZE - 1,
	.flags		= IORESOURCE_MEM
};

static struct pci_controller ar724x_pci_controller = {
	.pci_ops	= &ar724x_pci_ops,
	.mem_resource	= &ar724x_pci_mem_resource,
	.io_resource	= &ar724x_pci_io_resource,
};

int __init ar724x_pcibios_init(void)
{
	u32 t;

	ar724x_pci_localcfg_base = ioremap_nocache(AR724X_PCI_CRP_BASE,
						   AR724X_PCI_CRP_SIZE);

	ar724x_pci_devcfg_base = ioremap_nocache(AR724X_PCI_CFG_BASE,
						 AR724X_PCI_CFG_SIZE);

	/* setup COMMAND register */
	t = PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER | PCI_COMMAND_INVALIDATE |
	    PCI_COMMAND_PARITY | PCI_COMMAND_SERR | PCI_COMMAND_FAST_BACK;

	ar724x_pci_write(ar724x_pci_localcfg_base, PCI_COMMAND, 4, t);

	ar724x_pci_fixup_enable = 1;
	register_pci_controller(&ar724x_pci_controller);

	return 0;
}

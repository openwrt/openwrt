/*
 *  Ralink RT3883 SoC PCI support
 *
 *  Copyright (C) 2011-2012 Gabor Juhos <juhosg@openwrt.org>
 *
 *  Parts of this file are based on Ralink's 2.6.21 BSP
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/types.h>
#include <linux/pci.h>
#include <linux/io.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/interrupt.h>

#include <asm/mach-ralink/rt3883.h>
#include <asm/mach-ralink/rt3883_regs.h>

#define RT3883_MEMORY_BASE		0x00000000
#define RT3883_MEMORY_SIZE		0x02000000

#define RT3883_PCI_MEM_BASE		0x20000000
#define RT3883_PCI_MEM_SIZE		0x10000000
#define RT3883_PCI_IO_BASE		0x10160000
#define RT3883_PCI_IO_SIZE		0x00010000

#define RT3883_PCI_REG_PCICFG_ADDR	0x00
#define RT3883_PCI_REG_PCIRAW_ADDR	0x04
#define RT3883_PCI_REG_PCIINT_ADDR	0x08
#define RT3883_PCI_REG_PCIMSK_ADDR	0x0c
#define   RT3833_PCI_PCIINT_PCIE	BIT(20)
#define   RT3833_PCI_PCIINT_PCI1	BIT(19)
#define   RT3833_PCI_PCIINT_PCI0	BIT(18)

#define RT3883_PCI_REG_CONFIG_ADDR	0x20
#define RT3883_PCI_REG_CONFIG_DATA	0x24
#define RT3883_PCI_REG_MEMBASE		0x28
#define RT3883_PCI_REG_IOBASE		0x2c
#define RT3883_PCI_REG_ARBCTL		0x80

#define RT3883_PCI_REG_BASE(_x)		(0x1000 + (_x) * 0x1000)
#define RT3883_PCI_REG_BAR0SETUP_ADDR(_x) (RT3883_PCI_REG_BASE((_x)) + 0x10)
#define RT3883_PCI_REG_IMBASEBAR0_ADDR(_x) (RT3883_PCI_REG_BASE((_x)) + 0x18)
#define RT3883_PCI_REG_ID(_x)		(RT3883_PCI_REG_BASE((_x)) + 0x30)
#define RT3883_PCI_REG_CLASS(_x)	(RT3883_PCI_REG_BASE((_x)) + 0x34)
#define RT3883_PCI_REG_SUBID(_x)	(RT3883_PCI_REG_BASE((_x)) + 0x38)
#define RT3883_PCI_REG_STATUS(_x)	(RT3883_PCI_REG_BASE((_x)) + 0x50)

static int (*rt3883_pci_plat_dev_init)(struct pci_dev *dev);
static void __iomem *rt3883_pci_base;
static DEFINE_SPINLOCK(rt3883_pci_lock);

static inline u32 rt3883_pci_rr(unsigned reg)
{
	return readl(rt3883_pci_base + reg);
}

static inline void rt3883_pci_wr(u32 val, unsigned reg)
{
	writel(val, rt3883_pci_base + reg);
}

static inline u32 rt3883_pci_get_cfgaddr(unsigned int bus, unsigned int slot,
					 unsigned int func, unsigned int where)
{
	return ((bus << 16) | (slot << 11) | (func << 8) | (where & 0xfc) |
		0x80000000);
}

static u32 rt3883_pci_read_u32(unsigned bus, unsigned slot,
			       unsigned func, unsigned reg)
{
	unsigned long flags;
	u32 address;
	u32 ret;

	address = rt3883_pci_get_cfgaddr(bus, slot, func, reg);

	spin_lock_irqsave(&rt3883_pci_lock, flags);
	rt3883_pci_wr(address, RT3883_PCI_REG_CONFIG_ADDR);
	ret = rt3883_pci_rr(RT3883_PCI_REG_CONFIG_DATA);
	spin_unlock_irqrestore(&rt3883_pci_lock, flags);

	return ret;
}

static void rt3883_pci_write_u32(unsigned bus, unsigned slot,
				 unsigned func, unsigned reg, u32 val)
{
	unsigned long flags;
	u32 address;

	address = rt3883_pci_get_cfgaddr(bus, slot, func, reg);

	spin_lock_irqsave(&rt3883_pci_lock, flags);
	rt3883_pci_wr(address, RT3883_PCI_REG_CONFIG_ADDR);
	rt3883_pci_wr(val, RT3883_PCI_REG_CONFIG_DATA);
	spin_unlock_irqrestore(&rt3883_pci_lock, flags);
}

static void rt3883_pci_irq_handler(unsigned int irq, struct irq_desc *desc)
{
	u32 pending;

	pending = rt3883_pci_rr(RT3883_PCI_REG_PCIINT_ADDR) &
		  rt3883_pci_rr(RT3883_PCI_REG_PCIMSK_ADDR);

	if (!pending) {
		spurious_interrupt();
		return;
	}

	if (pending & RT3833_PCI_PCIINT_PCI0)
		generic_handle_irq(RT3883_PCI_IRQ_PCI0);

	if (pending & RT3833_PCI_PCIINT_PCI1)
		generic_handle_irq(RT3883_PCI_IRQ_PCI1);

	if (pending & RT3833_PCI_PCIINT_PCIE)
		generic_handle_irq(RT3883_PCI_IRQ_PCIE);
}

static void rt3883_pci_irq_unmask(struct irq_data *d)
{
	int irq = d->irq;
	u32 mask;
	u32 t;

	switch (irq) {
	case RT3883_PCI_IRQ_PCI0:
		mask = RT3833_PCI_PCIINT_PCI0;
		break;
	case RT3883_PCI_IRQ_PCI1:
		mask = RT3833_PCI_PCIINT_PCI1;
		break;
	case RT3883_PCI_IRQ_PCIE:
		mask = RT3833_PCI_PCIINT_PCIE;
		break;
	default:
		BUG();
	}

	t = rt3883_pci_rr(RT3883_PCI_REG_PCIMSK_ADDR);
	rt3883_pci_wr(t | mask, RT3883_PCI_REG_PCIMSK_ADDR);
	/* flush write */
	rt3883_pci_rr(RT3883_PCI_REG_PCIMSK_ADDR);
}

static void rt3883_pci_irq_mask(struct irq_data *d)
{
	int irq = d->irq;
	u32 mask;
	u32 t;

	switch (irq) {
	case RT3883_PCI_IRQ_PCI0:
		mask = RT3833_PCI_PCIINT_PCI0;
		break;
	case RT3883_PCI_IRQ_PCI1:
		mask = RT3833_PCI_PCIINT_PCI1;
		break;
	case RT3883_PCI_IRQ_PCIE:
		mask = RT3833_PCI_PCIINT_PCIE;
		break;
	default:
		BUG();
	}

	t = rt3883_pci_rr(RT3883_PCI_REG_PCIMSK_ADDR);
	rt3883_pci_wr(t & ~mask, RT3883_PCI_REG_PCIMSK_ADDR);
	/* flush write */
	rt3883_pci_rr(RT3883_PCI_REG_PCIMSK_ADDR);
}

static struct irq_chip rt3883_pci_irq_chip = {
	.name		= "RT3883 PCI",
	.irq_mask	= rt3883_pci_irq_mask,
	.irq_unmask	= rt3883_pci_irq_unmask,
	.irq_mask_ack	= rt3883_pci_irq_mask,
};

static void __init rt3883_pci_irq_init(void)
{
	int i;

	/* disable all interrupts */
	rt3883_pci_wr(0, RT3883_PCI_REG_PCIMSK_ADDR);

	for (i = RT3883_PCI_IRQ_BASE;
	     i < RT3883_PCI_IRQ_BASE + RT3883_PCI_IRQ_COUNT; i++) {
		irq_set_chip_and_handler(i, &rt3883_pci_irq_chip,
					 handle_level_irq);
	}

	irq_set_chained_handler(RT3883_CPU_IRQ_PCI, rt3883_pci_irq_handler);
}

static int rt3883_pci_config_read(struct pci_bus *bus, unsigned int devfn,
				  int where, int size, u32 *val)
{
	unsigned long flags;
	u32 address;
	u32 data;

	address = rt3883_pci_get_cfgaddr(bus->number, PCI_SLOT(devfn),
					 PCI_FUNC(devfn), where);

	spin_lock_irqsave(&rt3883_pci_lock, flags);
	rt3883_pci_wr(address, RT3883_PCI_REG_CONFIG_ADDR);
	data = rt3883_pci_rr(RT3883_PCI_REG_CONFIG_DATA);
	spin_unlock_irqrestore(&rt3883_pci_lock, flags);

	switch (size) {
	case 1:
		*val = (data >> ((where & 3) << 3)) & 0xff;
		break;
	case 2:
		*val = (data >> ((where & 3) << 3)) & 0xffff;
		break;
	case 4:
		*val = data;
		break;
	}

	return PCIBIOS_SUCCESSFUL;
}

static int rt3883_pci_config_write(struct pci_bus *bus, unsigned int devfn,
				   int where, int size, u32 val)
{
	unsigned long flags;
	u32 address;
	u32 data;

	address = rt3883_pci_get_cfgaddr(bus->number, PCI_SLOT(devfn),
					 PCI_FUNC(devfn), where);

	spin_lock_irqsave(&rt3883_pci_lock, flags);
	rt3883_pci_wr(address, RT3883_PCI_REG_CONFIG_ADDR);
	data = rt3883_pci_rr(RT3883_PCI_REG_CONFIG_DATA);

	switch (size) {
	case 1:
		data = (data & ~(0xff << ((where & 3) << 3))) |
		       (val << ((where & 3) << 3));
		break;
	case 2:
		data = (data & ~(0xffff << ((where & 3) << 3))) |
		       (val << ((where & 3) << 3));
		break;
	case 4:
		data = val;
		break;
	}

	rt3883_pci_wr(data, RT3883_PCI_REG_CONFIG_DATA);
	spin_unlock_irqrestore(&rt3883_pci_lock, flags);

	return PCIBIOS_SUCCESSFUL;
}

static struct pci_ops rt3883_pci_ops = {
	.read	= rt3883_pci_config_read,
	.write	= rt3883_pci_config_write,
};

static struct resource rt3883_pci_mem_resource = {
	.name	= "PCI MEM space",
	.start	= RT3883_PCI_MEM_BASE,
	.end	= RT3883_PCI_MEM_BASE + RT3883_PCI_MEM_SIZE - 1,
	.flags	= IORESOURCE_MEM,
};

static struct resource rt3883_pci_io_resource = {
	.name	= "PCI IO space",
	.start	= RT3883_PCI_IO_BASE,
	.end	= RT3883_PCI_IO_BASE + RT3883_PCI_IO_SIZE - 1,
	.flags	= IORESOURCE_IO,
};

static struct pci_controller rt3883_pci_controller = {
	.pci_ops	= &rt3883_pci_ops,
	.mem_resource	= &rt3883_pci_mem_resource,
	.io_resource	= &rt3883_pci_io_resource,
};

static void rt3883_pci_preinit(unsigned mode)
{
	u32 syscfg1;
	u32 rstctrl;
	u32 clkcfg1;

	if (mode & RT3883_PCI_MODE_PCIE) {
		u32 val;

		val = rt3883_sysc_rr(RT3883_SYSC_REG_SYSCFG1);
		val &= ~(0x30);
		val |= (2 << 4);
		rt3883_sysc_wr(val, RT3883_SYSC_REG_SYSCFG1);

		val = rt3883_sysc_rr(RT3883_SYSC_REG_PCIE_CLK_GEN0);
		val &= ~BIT(31);
		rt3883_sysc_wr(val, RT3883_SYSC_REG_PCIE_CLK_GEN0);

		val = rt3883_sysc_rr(RT3883_SYSC_REG_PCIE_CLK_GEN1);
		val &= 0x80ffffff;
		rt3883_sysc_wr(val, RT3883_SYSC_REG_PCIE_CLK_GEN1);

		val = rt3883_sysc_rr(RT3883_SYSC_REG_PCIE_CLK_GEN1);
		val |= 0xa << 24;
		rt3883_sysc_wr(val, RT3883_SYSC_REG_PCIE_CLK_GEN1);

		val = rt3883_sysc_rr(RT3883_SYSC_REG_PCIE_CLK_GEN0);
		val |= BIT(31);
		rt3883_sysc_wr(val, RT3883_SYSC_REG_PCIE_CLK_GEN0);

		msleep(50);
	}

	syscfg1 = rt3883_sysc_rr(RT3883_SYSC_REG_SYSCFG1);
	syscfg1 &= ~(RT3883_SYSCFG1_PCIE_RC_MODE |
		     RT3883_SYSCFG1_PCI_HOST_MODE);

	rstctrl = rt3883_sysc_rr(RT3883_SYSC_REG_RSTCTRL);
	rstctrl |= (RT3883_RSTCTRL_PCI | RT3883_RSTCTRL_PCIE);

	clkcfg1 = rt3883_sysc_rr(RT3883_SYSC_REG_CLKCFG1);
	clkcfg1 &= ~(RT3883_CLKCFG1_PCI_CLK_EN |
		     RT3883_CLKCFG1_PCIE_CLK_EN);

	if (mode & RT3883_PCI_MODE_PCI) {
		syscfg1 |= RT3883_SYSCFG1_PCI_HOST_MODE;
		clkcfg1 |= RT3883_CLKCFG1_PCI_CLK_EN;
		rstctrl &= ~RT3883_RSTCTRL_PCI;
	}
	if (mode & RT3883_PCI_MODE_PCIE) {
		syscfg1 |= RT3883_SYSCFG1_PCI_HOST_MODE |
			   RT3883_SYSCFG1_PCIE_RC_MODE;
		clkcfg1 |= RT3883_CLKCFG1_PCIE_CLK_EN;
		rstctrl &= ~RT3883_RSTCTRL_PCIE;
	}

	rt3883_sysc_wr(syscfg1, RT3883_SYSC_REG_SYSCFG1);
	rt3883_sysc_wr(rstctrl, RT3883_SYSC_REG_RSTCTRL);
	rt3883_sysc_wr(clkcfg1, RT3883_SYSC_REG_CLKCFG1);

	msleep(500);
}

static int rt3883_pcie_ready(void)
{
	u32 status;

	msleep(500);

	status = rt3883_pci_rr(RT3883_PCI_REG_STATUS(1));
	if (status & BIT(0))
		return 0;

	/* TODO: reset PCIe and turn off PCIe clock */

	return -ENODEV;
}

void __init rt3883_pci_init(unsigned mode)
{
	u32 val;
	int err;

	rt3883_pci_preinit(mode);

	rt3883_pci_base = ioremap(RT3883_PCI_BASE, PAGE_SIZE);
	if (rt3883_pci_base == NULL) {
		pr_err("failed to ioremap PCI registers\n");
		return;
	}

	rt3883_pci_wr(0, RT3883_PCI_REG_PCICFG_ADDR);
	if (mode & RT3883_PCI_MODE_PCI)
		rt3883_pci_wr(BIT(16), RT3883_PCI_REG_PCICFG_ADDR);

	msleep(500);

	if (mode & RT3883_PCI_MODE_PCIE) {
		err = rt3883_pcie_ready();
		if (err)
			return;
	}

	if (mode & RT3883_PCI_MODE_PCI)
		rt3883_pci_wr(0x79, RT3883_PCI_REG_ARBCTL);

	rt3883_pci_wr(RT3883_PCI_MEM_BASE, RT3883_PCI_REG_MEMBASE);
	rt3883_pci_wr(RT3883_PCI_IO_BASE, RT3883_PCI_REG_IOBASE);

	/* PCI */
	rt3883_pci_wr(0x03ff0000, RT3883_PCI_REG_BAR0SETUP_ADDR(0));
	rt3883_pci_wr(RT3883_MEMORY_BASE, RT3883_PCI_REG_IMBASEBAR0_ADDR(0));
	rt3883_pci_wr(0x08021814, RT3883_PCI_REG_ID(0));
	rt3883_pci_wr(0x00800001, RT3883_PCI_REG_CLASS(0));
	rt3883_pci_wr(0x28801814, RT3883_PCI_REG_SUBID(0));

	/* PCIe */
	rt3883_pci_wr(0x01ff0000, RT3883_PCI_REG_BAR0SETUP_ADDR(1));
	rt3883_pci_wr(RT3883_MEMORY_BASE, RT3883_PCI_REG_IMBASEBAR0_ADDR(1));
	rt3883_pci_wr(0x08021814, RT3883_PCI_REG_ID(1));
	rt3883_pci_wr(0x06040001, RT3883_PCI_REG_CLASS(1));
	rt3883_pci_wr(0x28801814, RT3883_PCI_REG_SUBID(1));

	rt3883_pci_irq_init();

	/* PCIe */
	val = rt3883_pci_read_u32(0, 0x01, 0, PCI_COMMAND);
	val |= 0x7;
	rt3883_pci_write_u32(0, 0x01, 0, PCI_COMMAND, val);

	/* PCI */
	val = rt3883_pci_read_u32(0, 0x00, 0, PCI_COMMAND);
	val |= 0x7;
	rt3883_pci_write_u32(0, 0x00, 0, PCI_COMMAND, val);

	ioport_resource.start = rt3883_pci_io_resource.start;
	ioport_resource.end = rt3883_pci_io_resource.end;

	register_pci_controller(&rt3883_pci_controller);
}

int __init pcibios_map_irq(const struct pci_dev *dev, u8 slot, u8 pin)
{
	int irq = -1;

	switch (dev->bus->number) {
	case 0:
		switch (PCI_SLOT(dev->devfn)) {
		case 0x00:
			rt3883_pci_wr(0x03ff0001,
					     RT3883_PCI_REG_BAR0SETUP_ADDR(0));
			rt3883_pci_wr(0x03ff0001,
					     RT3883_PCI_REG_BAR0SETUP_ADDR(1));

			rt3883_pci_write_u32(0, 0x00, 0, PCI_BASE_ADDRESS_0,
					     RT3883_MEMORY_BASE);
			rt3883_pci_read_u32(0, 0x00, 0, PCI_BASE_ADDRESS_0);

			irq = RT3883_CPU_IRQ_PCI;
			break;
		case 0x01:
			rt3883_pci_write_u32(0, 0x01, 0, PCI_IO_BASE,
					     0x00000101);
			break;
		case 0x11:
			irq = RT3883_PCI_IRQ_PCI0;
			break;
		case 0x12:
			irq = RT3883_PCI_IRQ_PCI1;
			break;
		}
		break;

	case 1:
		irq = RT3883_PCI_IRQ_PCIE;
		break;

	default:
		dev_err(&dev->dev, "no IRQ specified\n");
		return irq;
	}

	return irq;
}

void __init rt3883_pci_set_plat_dev_init(int (*f)(struct pci_dev *dev))
{
	rt3883_pci_plat_dev_init = f;
}

int pcibios_plat_dev_init(struct pci_dev *dev)
{
	if (rt3883_pci_plat_dev_init)
		return rt3883_pci_plat_dev_init(dev);

	return 0;
}

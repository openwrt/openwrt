/*
 *  $Id$
 *
 *  ADM5120 PCI Host Controller driver
 *
 *  Copyright (C) ADMtek Incorporated.
 *  Copyright (C) 2005 Jeroen Vreeken (pe1rxq@amsat.org)
 *  Copyright (C) 2007 Gabor Juhos <juhosg at openwrt.org>
 *  Copyright (C) 2007 OpenWrt.org
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the
 *  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA  02110-1301, USA.
 *
 */
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/spinlock.h>

#include <linux/pci.h>
#include <linux/pci_ids.h>
#include <linux/pci_regs.h>

#include <asm/io.h>
#include <asm/delay.h>
#include <asm/bootinfo.h>

#include <adm5120_defs.h>
#include <adm5120_info.h>
#include <adm5120_defs.h>
#include <adm5120_platform.h>

#undef DEBUG

#ifdef DEBUG
#define DBG(f, a...)	printk(KERN_DEBUG f, ## a )
#else
#define DBG(f, a...)	do {} while (0)
#endif

#define PCI_ENABLE 0x80000000

/* -------------------------------------------------------------------------*/

static unsigned int adm5120_pci_nr_irqs __initdata = 0;
static struct adm5120_pci_irq *adm5120_pci_irq_map __initdata = NULL;

static spinlock_t pci_lock = SPIN_LOCK_UNLOCKED;

/* -------------------------------------------------------------------------*/

static inline void write_cfgaddr(u32 addr)
{
	__raw_writel((addr | PCI_ENABLE),
		(void __iomem *)(KSEG1ADDR(ADM5120_PCICFG_ADDR)));
}

static inline void write_cfgdata(u32 data)
{
	__raw_writel(data, (void __iomem *)KSEG1ADDR(ADM5120_PCICFG_DATA));
}

static inline u32 read_cfgdata(void)
{
	return __raw_readl((void __iomem *)KSEG1ADDR(ADM5120_PCICFG_DATA));
}

static inline u32 mkaddr(struct pci_bus *bus, unsigned int devfn, int where)
{
	return (((bus->number & 0xFF) << 16) | ((devfn & 0xFF) << 8) | \
		(where & 0xFC));
}

/* -------------------------------------------------------------------------*/

static int pci_config_read(struct pci_bus *bus, unsigned int devfn, int where,
		int size, u32 *val)
{
	unsigned long flags;
	u32 data;

	spin_lock_irqsave(&pci_lock, flags);

	write_cfgaddr(mkaddr(bus,devfn,where));
	data = read_cfgdata();

	DBG("PCI: cfg_read  %02u.%02u.%01u/%02X:%01d, cfg:0x%08X",
		bus->number, PCI_SLOT(devfn), PCI_FUNC(devfn),
		where, size, data);

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

	*val = data;
	DBG(", 0x%08X returned\n", data);

	spin_unlock_irqrestore(&pci_lock, flags);

	return PCIBIOS_SUCCESSFUL;
}

static int pci_config_write(struct pci_bus *bus, unsigned int devfn, int where,
		int size, u32 val)
{
	unsigned long flags;
	u32 data;
	int s;

	spin_lock_irqsave(&pci_lock, flags);

	write_cfgaddr(mkaddr(bus,devfn,where));
	data = read_cfgdata();

	DBG("PCI: cfg_write %02u.%02u.%01u/%02X:%01d, cfg:0x%08X",
		bus->number, PCI_SLOT(devfn), PCI_FUNC(devfn),
		where, size, data);

	switch (size) {
	case 1:
		s = ((where & 3) << 3);
		data &= ~(0xFF << s);
		data |= ((val & 0xFF) << s);
		break;
	case 2:
		s = ((where & 2) << 4);
		data &= ~(0xFFFF << s);
		data |= ((val & 0xFFFF) << s);
		break;
	case 4:
		data = val;
		break;
	}

	write_cfgdata(data);
	DBG(", 0x%08X written\n", data);

	spin_unlock_irqrestore(&pci_lock, flags);

	return PCIBIOS_SUCCESSFUL;
}

struct pci_ops adm5120_pci_ops = {
	.read	= pci_config_read,
	.write	= pci_config_write,
};

/* -------------------------------------------------------------------------*/

static void adm5120_pci_fixup(struct pci_dev *dev)
{
	if (dev->devfn != 0)
		return;

	/* setup COMMAND register */
	pci_write_config_word(dev, PCI_COMMAND,
		(PCI_COMMAND_IO | PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER));

	/* setup CACHE_LINE_SIZE register */
	pci_write_config_byte(dev, PCI_CACHE_LINE_SIZE, 4);

	/* setup BARS */
	pci_write_config_dword(dev, PCI_BASE_ADDRESS_0, 0);
	pci_write_config_dword(dev, PCI_BASE_ADDRESS_1, 0);
}

DECLARE_PCI_FIXUP_HEADER(PCI_VENDOR_ID_ADMTEK, PCI_DEVICE_ID_ADMTEK_ADM5120,
	adm5120_pci_fixup);

/* -------------------------------------------------------------------------*/

void __init adm5120_pci_set_irq_map(unsigned int nr_irqs,
		struct adm5120_pci_irq *map)
{
	adm5120_pci_nr_irqs = nr_irqs;
	adm5120_pci_irq_map = map;
}

int __init pcibios_map_irq(struct pci_dev *dev, u8 slot, u8 pin)
{
	int irq = -1;
	int i;

	if ((!adm5120_pci_nr_irqs) || (!adm5120_pci_irq_map)) {
		printk(KERN_ALERT "PCI: pci_irq_map is not initialized\n");
		goto out;
	}

	if (slot < 1 || slot > 3) {
		printk(KERN_ALERT "PCI: slot number %u is not supported\n",
			slot);
		goto out;
	}

	for (i = 0; i < adm5120_pci_nr_irqs; i++) {
		if ((adm5120_pci_irq_map[i].slot == slot)
			&& (adm5120_pci_irq_map[i].func == PCI_FUNC(dev->devfn))
			&& (adm5120_pci_irq_map[i].pin == pin)) {
			irq = adm5120_pci_irq_map[i].irq;
			break;
		}
	}

	if (irq < 0) {
		printk(KERN_ALERT "PCI: no irq found for %s pin:%u\n",
			pci_name(dev), pin);
	} else {
		printk(KERN_INFO "PCI: mapping irq for %s pin:%u, irq:%d\n",
			pci_name(dev), pin, irq);
	}

out:
	return irq;
}

int pcibios_plat_dev_init(struct pci_dev *dev)
{
	return 0;
}

/* -------------------------------------------------------------------------*/

static struct resource pci_io_resource = {
	.name	= "ADM5120 PCI I/O",
	.start	= ADM5120_PCIIO_BASE,
	.end	= ADM5120_PCICFG_ADDR-1,
	.flags	= IORESOURCE_IO
};

static struct resource pci_mem_resource = {
	.name	= "ADM5120 PCI MEM",
	.start	= ADM5120_PCIMEM_BASE,
	.end	= ADM5120_PCIIO_BASE-1,
	.flags	= IORESOURCE_MEM
};

static struct pci_controller adm5120_controller = {
	.pci_ops	= &adm5120_pci_ops,
	.io_resource	= &pci_io_resource,
	.mem_resource	= &pci_mem_resource,
};

static int __init adm5120_pci_setup(void)
{
	int pci_bios;

	pci_bios = adm5120_has_pci();

	printk(KERN_INFO "adm5120: system has %sPCI BIOS\n",
		pci_bios ? "" : "no ");
	if (pci_bios == 0)
		return -1;

	/* Avoid ISA compat ranges.  */
	PCIBIOS_MIN_IO = 0x00000000;
	PCIBIOS_MIN_MEM = 0x00000000;

	/* Set I/O resource limits.  */
	ioport_resource.end = 0x1fffffff;
	iomem_resource.end = 0xffffffff;

	register_pci_controller(&adm5120_controller);
	return 0;
}

arch_initcall(adm5120_pci_setup);

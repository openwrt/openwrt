/*
 * Copyright (C) 2010 Scott Nicholas <neutronscott@scottn.us>
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/pci.h>
#include <linux/types.h>
#include <asm/byteorder.h>
#include <asm/pci.h>
#include <adm8668.h>

volatile u32* pci_config_address_reg = (volatile u32*)KSEG1ADDR(PCICFG_BASE);
volatile u32* pci_config_data_reg = (volatile u32*)KSEG1ADDR(PCIDAT_BASE);

#define PCI_ENABLE			0x80000000
#define	ADMPCI_IO_BASE			0x12600000
#define	ADMPCI_IO_SIZE			0x1fffff
#define	ADMPCI_MEM_BASE			0x16000000
#define	ADMPCI_MEM_SIZE			0x7ffffff
#define PCI_CMM_IOACC_EN		0x1
#define PCI_CMM_MEMACC_EN		0x2
#define PCI_CMM_MASTER_EN		0x4
#define PCI_CMM_DEF			(PCI_CMM_IOACC_EN | PCI_CMM_MEMACC_EN | PCI_CMM_MASTER_EN)

#define PCI_DEF_CACHE_LINE_SZ	0
#define PCI_DEF_LATENCY_TIMER	0x20
#define PCI_DEF_CACHE_LATENCY	((PCI_DEF_LATENCY_TIMER << 8) | PCI_DEF_CACHE_LINE_SZ)


#define cfgaddr(bus, devfn, where)	(	\
				(bus ? ((bus->number & 0xff) << 0x10) : 0) | \
					 ((devfn & 0xff) << 0x08) | \
					 (where & 0xfc)) | PCI_ENABLE

/* assumed little endian */
static int adm8668_read_config(struct pci_bus *bus, unsigned int devfn,
				int where, int size, u32 *val)
{
    	switch (size)
	{
	case 1:
		*pci_config_address_reg = cfgaddr(bus, devfn, where);
		*val = (le32_to_cpu(*pci_config_data_reg) >> ((where&3)<<3)) & 0xff;
		break;
	case 2:
		if (where & 1)
			return PCIBIOS_BAD_REGISTER_NUMBER;
		*pci_config_address_reg = cfgaddr(bus, devfn, where);
		*val = (le32_to_cpu(*pci_config_data_reg) >> ((where&3)<<3)) & 0xffff;
		break;
	case 4:
		if (where & 3)
			return PCIBIOS_BAD_REGISTER_NUMBER;
		*pci_config_address_reg = cfgaddr(bus, devfn, where);
		*val = le32_to_cpu(*pci_config_data_reg);
		break;
	}

	return PCIBIOS_SUCCESSFUL;
}

static int adm8668_write_config(struct pci_bus *bus, unsigned int devfn,
				int where, int size, u32 val)
{
	switch (size)
	{
	case 1:
		*pci_config_address_reg = cfgaddr(bus, devfn, where);
		*(volatile u8 *)(((int)pci_config_data_reg) + (where & 3)) = val;
		break;
	case 2:
		if (where & 1)
			return PCIBIOS_BAD_REGISTER_NUMBER;
		*pci_config_address_reg = cfgaddr(bus, devfn, where);
		*(volatile u16 *)(((int)pci_config_data_reg) + (where & 2)) = val;
		break;
	case 4:
		if (where & 3)
			return PCIBIOS_BAD_REGISTER_NUMBER;
		*pci_config_address_reg = cfgaddr(bus, devfn, where);
		*pci_config_data_reg = (val);
	}

	return PCIBIOS_SUCCESSFUL;
}


struct pci_ops adm8668_pci_ops = {
	.read = adm8668_read_config,
	.write = adm8668_write_config
};


struct resource pciioport_resource = {
	.name	= "adm8668_pci",
	.start	= ADMPCI_IO_BASE,
	.end	= ADMPCI_IO_BASE + ADMPCI_IO_SIZE,
	.flags	= IORESOURCE_IO
};


struct resource pciiomem_resource = {
	.name	= "adm8668_pci",
	.start	= ADMPCI_MEM_BASE,
	.end	= ADMPCI_MEM_BASE + ADMPCI_MEM_SIZE,
	.flags	= IORESOURCE_MEM
};

#ifdef CONFIG_ADM8668_DISABLE_PCI
struct pci_controller mips_pci_channels[] = {
	{ NULL, NULL, NULL , NULL , NULL}
};
#else
struct pci_controller mips_pci_channels = {
	.pci_ops = &adm8668_pci_ops,
	.io_resource = &pciioport_resource,
	.mem_resource = &pciiomem_resource,
};
#endif

int pcibios_map_irq(const struct pci_dev *dev, u8 slot, u8 pin)
{
	switch (slot)
	{
		case 1:
			return 14;
		case 2:
			return 13;
		case 3:
			return 12;
		default:
			return dev->irq;
	}
}

int pcibios_plat_dev_init(struct pci_dev *dev)
{
	return 0;
}

static int __init adm8668_pci_init(void)
{
	void __iomem *io_map_base;

	printk("adm8668_pci_init()\n");

	/* what's an io port? this is MIPS... *shrug* */
	ioport_resource.start   = ADMPCI_IO_BASE;
	ioport_resource.end     = ADMPCI_IO_BASE + ADMPCI_IO_SIZE;

	io_map_base = ioremap(ADMPCI_IO_BASE, ADMPCI_IO_SIZE);
	if (!io_map_base)
		printk("io_map_base didn't work.\n");
	mips_pci_channels.io_map_base = (unsigned long)io_map_base;
	register_pci_controller(&mips_pci_channels);

	/* this needed? linksys' gpl 2.4 did it... */
	adm8668_write_config(NULL, 0, PCI_CACHE_LINE_SIZE, 2, 0);
	adm8668_write_config(NULL, 0, PCI_BASE_ADDRESS_0, 4, 0);
	adm8668_write_config(NULL, 0, PCI_BASE_ADDRESS_1, 4, 0);
	adm8668_write_config(NULL, 0, PCI_COMMAND, 4, PCI_CMM_DEF);

	return 0;
}

arch_initcall(adm8668_pci_init);

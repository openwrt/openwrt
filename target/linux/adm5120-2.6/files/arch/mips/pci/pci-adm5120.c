/*
 *	Copyright (C) ADMtek Incorporated.
 *	Copyright (C) 2005 Jeroen Vreeken (pe1rxq@amsat.org)
 *	Copyright (C) 2007 Gabor Juhos <juhosg@freemail.hu>
 *	Copyright (C) 2007 OpenWrt.org
 */

#include <linux/autoconf.h>
#include <linux/types.h>
#include <linux/pci.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <adm5120_info.h>
#include <adm5120_defs.h>

extern struct pci_ops adm5120_pci_ops;

#define PCI_CMM_IOACC_EN	0x1
#define PCI_CMM_MEMACC_EN	0x2
#define PCI_CMM_MASTER_EN	0x4
#define PCI_CMM_DEF \
		(PCI_CMM_IOACC_EN | PCI_CMM_MEMACC_EN | PCI_CMM_MASTER_EN)

#define PCI_DEF_CACHE_LINE_SZ	4


struct resource pci_io_resource = {
	.name = "ADM5120 PCI I/O",
	.start = ADM5120_PCIIO_BASE,
	.end = ADM5120_PCICFG_ADDR-1,
	.flags = IORESOURCE_IO
};

struct resource pci_mem_resource = {
	.name = "ADM5120 PCI MEM",
	.start = ADM5120_PCIMEM_BASE,
	.end = ADM5120_PCIIO_BASE-1,
	.flags = IORESOURCE_MEM
};

static struct pci_controller adm5120_controller = {
	.pci_ops	= &adm5120_pci_ops,
	.io_resource	= &pci_io_resource,
	.mem_resource	= &pci_mem_resource,
};

int __init pcibios_map_irq(struct pci_dev *dev, u8 slot, u8 pin)
{
	if (slot < 2 || slot > 4)
		return -1;
	return slot + 4;
}

static void adm5120_pci_fixup(struct pci_dev *dev)
{
	if (dev->devfn == 0) {
		pci_write_config_word(dev, PCI_COMMAND, PCI_CMM_DEF);
		pci_write_config_byte(dev, PCI_CACHE_LINE_SIZE,
		    PCI_DEF_CACHE_LINE_SZ);
		pci_write_config_dword(dev, PCI_BASE_ADDRESS_0, 0);
		pci_write_config_dword(dev, PCI_BASE_ADDRESS_1, 0);
	}
}

DECLARE_PCI_FIXUP_HEADER(PCI_ANY_ID, PCI_ANY_ID, adm5120_pci_fixup);


int pcibios_plat_dev_init(struct pci_dev *dev)
{
	return 0;
}

static int __init adm5120_pci_setup(void)
{
	int pci_bios;

	pci_bios = adm5120_has_pci();

	printk("adm5120: system has %sPCI BIOS\n", pci_bios ? "" : "no ");
	if (pci_bios == 0)
		return 1;

	/* Avoid ISA compat ranges.  */
	PCIBIOS_MIN_IO = 0x00000000;
	PCIBIOS_MIN_MEM = 0x00000000;

	/* Set I/O resource limits.  */
	ioport_resource.end = 0x1fffffff;
	iomem_resource.end = 0xffffffff;

	register_pci_controller(&adm5120_controller);
	return 0;
}

subsys_initcall(adm5120_pci_setup);

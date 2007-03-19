/*
 *	Copyright (C) ADMtek Incorporated.
 *	Copyright (C) 2005 Jeroen Vreeken (pe1rxq@amsat.org)
 */

#include <linux/autoconf.h>
#include <linux/types.h>
#include <linux/pci.h>
#include <linux/kernel.h>
#include <linux/init.h>

extern struct pci_ops adm5120_pci_ops;

#define ADM5120_CODE		0x12000000
#define ADM5120_CODE_PQFP	0x20000000

#define PCI_CMM_IOACC_EN	0x1
#define PCI_CMM_MEMACC_EN	0x2
#define PCI_CMM_MASTER_EN	0x4
#define PCI_CMM_DEF \
		(PCI_CMM_IOACC_EN | PCI_CMM_MEMACC_EN | PCI_CMM_MASTER_EN)

#define PCI_DEF_CACHE_LINE_SZ	4


struct resource pci_io_resource = {
	"PCI IO space", 
	0x11500000,  
	0x115ffff0-1,
	IORESOURCE_IO
};

struct resource pci_mem_resource = {
	"PCI memory space", 
	0x11400000,
	0x11500000,
	IORESOURCE_MEM
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

	if ((*(volatile u32 *)(KSEG1ADDR(ADM5120_CODE))) & ADM5120_CODE_PQFP) {
		printk("System has no PCI BIOS\n");
		return 1;
	}
	printk("System has PCI BIOS\n");

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

/*
 *	Copyright (C) ADMtek Incorporated.
 *	Copyright (C) 2005 Jeroen Vreeken (pe1rxq@amsat.org)
 */

#include <linux/autoconf.h>
#include <linux/types.h>
#include <linux/pci.h>
#include <linux/kernel.h>
#include <linux/init.h>

volatile u32* pci_config_address_reg = (volatile u32*)KSEG1ADDR(0x115ffff0);
volatile u32* pci_config_data_reg = (volatile u32*)KSEG1ADDR(0x115ffff8);

#define PCI_ENABLE 0x80000000
                          
static int pci_config_read(struct pci_bus *bus, unsigned int devfn, int where,
                           int size, uint32_t *val)
{
	*pci_config_address_reg = ((bus->number & 0xff) << 0x10) |
	    ((devfn & 0xff) << 0x08) | (where & 0xfc) | PCI_ENABLE;
	switch (size) {
		case 1:
			*val = ((*pci_config_data_reg)>>((where&3)<<3))&0xff;
			break;
		case 2:
			*val = ((*pci_config_data_reg)>>((where&3)<<3))&0xffff;
			break;
		default:
			*val = (*pci_config_data_reg);
	}
	return PCIBIOS_SUCCESSFUL;
}

static int pci_config_write(struct pci_bus *bus, unsigned int devfn, int where,
                            int size, uint32_t val)
{
	*pci_config_address_reg = ((bus->number & 0xff) << 0x10) |
	    ((devfn & 0xff) << 0x08) | (where & 0xfc) | PCI_ENABLE;
	switch (size) {
		case 1:
			*(volatile u8 *)(((int)pci_config_data_reg) +
			    (where & 3)) = val;
			break;
		case 2:
			*(volatile u16 *)(((int)pci_config_data_reg) +
			    (where & 2)) = (val);
			break;
		default:
			*pci_config_data_reg = (val);
	}

	return PCIBIOS_SUCCESSFUL;
}

struct pci_ops adm5120_pci_ops = {
	.read	= pci_config_read,
	.write	= pci_config_write,
};

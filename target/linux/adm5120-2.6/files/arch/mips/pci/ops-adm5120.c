/*
 *  $Id$
 *
 *  ADM5120 specific PCI operations
 *
 *  Copyright (C) ADMtek Incorporated.
 *  Copyright (C) 2005 Jeroen Vreeken (pe1rxq@amsat.org)
 *  Copyright (C) 2007 Gabor Juhos <juhosg@freemail.hu>
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
#include <linux/pci.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <asm/mach-adm5120/adm5120_defs.h>

volatile u32* pci_config_address_reg = (volatile u32*)KSEG1ADDR(ADM5120_PCICFG_ADDR);
volatile u32* pci_config_data_reg = (volatile u32*)KSEG1ADDR(ADM5120_PCICFG_DATA);

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
		break;
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

/*
 *  $Id$
 *
 *  ADM5120 specific PCI fixups
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

#include <linux/autoconf.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/pci.h>
#include <linux/pci_ids.h>
#include <linux/pci_regs.h>

#include <asm/delay.h>
#include <asm/bootinfo.h>

#include <asm/mach-adm5120/adm5120_info.h>
#include <asm/mach-adm5120/adm5120_defs.h>
#include <asm/mach-adm5120/adm5120_irq.h>

static void adm5120_pci_fixup(struct pci_dev *dev)
{
	if (dev->devfn !=0)
		return;

	/* setup COMMAND register */
	pci_write_config_word(dev, PCI_COMMAND, 
		(PCI_COMMAND_IO | PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER));

	/* setup CACHE_LINE_SIZE register */
	pci_write_config_byte(dev, PCI_CACHE_LINE_SIZE, 4);

	/* setting up BARS */
	pci_write_config_dword(dev, PCI_BASE_ADDRESS_0, 0);
	pci_write_config_dword(dev, PCI_BASE_ADDRESS_1, 0);
}

DECLARE_PCI_FIXUP_HEADER(PCI_VENDOR_ID_ADMTEK, PCI_DEVICE_ID_ADMTEK_ADM5120, 
	adm5120_pci_fixup);


int __init pcibios_map_irq(struct pci_dev *dev, u8 slot, u8 pin)
{
	int irq;
	
	irq = -1;
#if 1
	/* FIXME: this code should be working on all boards? */
	if (slot > 0 && slot < 4)
		irq = ADM5120_IRQ_PCI0+slot-1;
#else	
	switch (mips_machtype) {
	case MACH_ADM5120_RB_111:
	case MACH_ADM5120_RB_112:
	case MACH_ADM5120_RB_133:
	case MACH_ADM5120_RB_133C:
	case MACH_ADM5120_RB_153:
		if (slot > 0 && slot < 4)
			irq = ADM5120_IRQ_PCI0+slot-1;
		break;
	default:
		if (slot > 1 && slot < 5)
			irq = ADM5120_IRQ_PCI0+slot-2;
		break;
	}
#endif
	printk(KERN_INFO "PCI: mapping irq for device %s, slot:%u, pin:%u, "
		"irq:%d\n", pci_name(dev), slot, pin, irq);
		
	return irq;
}

int pcibios_plat_dev_init(struct pci_dev *dev)
{
	return 0;
}


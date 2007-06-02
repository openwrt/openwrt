/*
 *  $Id$
 *
 *  ADM5120 PCI Host Controller driver
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

#include <asm/mach-adm5120/adm5120_info.h>
#include <asm/mach-adm5120/adm5120_defs.h>
#include <asm/mach-adm5120/adm5120_irq.h>

extern struct pci_ops adm5120_pci_ops;

static struct resource pci_io_resource = {
	.name = "ADM5120 PCI I/O",
	.start = ADM5120_PCIIO_BASE,
	.end = ADM5120_PCICFG_ADDR-1,
	.flags = IORESOURCE_IO
};

static struct resource pci_mem_resource = {
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

static int __init adm5120_pci_setup(void)
{
	int pci_bios;

	pci_bios = adm5120_has_pci();

	printk("adm5120: system has %sPCI BIOS\n", pci_bios ? "" : "no ");
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

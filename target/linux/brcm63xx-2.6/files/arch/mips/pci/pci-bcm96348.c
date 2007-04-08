/*
<:copyright-gpl
 Copyright 2002 Broadcom Corp. All Rights Reserved.

 This program is free software; you can distribute it and/or modify it
 under the terms of the GNU General Public License (Version 2) as
 published by the Free Software Foundation.

 This program is distributed in the hope it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 for more details.

 You should have received a copy of the GNU General Public License along
 with this program; if not, write to the Free Software Foundation, Inc.,
 59 Temple Place - Suite 330, Boston MA 02111-1307, USA.
:>
*/
#include <linux/types.h>
#include <linux/pci.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <bcmpci.h>

static struct resource bcm_pci_io_resource = {
	.name   = "bcm96348 pci IO space",
	.start  = BCM_PCI_IO_BASE,
	.end    = BCM_PCI_IO_BASE + BCM_PCI_IO_SIZE_64KB - 1,
	.flags  = IORESOURCE_IO
};

static struct resource bcm_pci_mem_resource = {
	.name   = "bcm96348 pci memory space",
	.start  = BCM_PCI_MEM_BASE,
	.end    = BCM_PCI_MEM_BASE + BCM_PCI_MEM_SIZE_16MB - 1,
	.flags  = IORESOURCE_MEM
};

extern struct pci_ops bcm96348_pci_ops;

struct pci_controller bcm96348_controller = {
	.pci_ops   	= &bcm96348_pci_ops,
	.io_resource	= &bcm_pci_io_resource,
	.mem_resource	= &bcm_pci_mem_resource,
};

static __init int bcm96348_pci_init(void)
{
	/* Avoid ISA compat ranges.  */
	PCIBIOS_MIN_IO = 0x00000000;
	PCIBIOS_MIN_MEM = 0x00000000;

	/* Set I/O resource limits.  */
	ioport_resource.end = 0x1fffffff;
	iomem_resource.end = 0xffffffff;

	register_pci_controller(&bcm96348_controller);
        return 0;
}

subsys_initcall(bcm96348_pci_init);

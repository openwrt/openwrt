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
#include <linux/init.h>
#include <linux/types.h>
#include <linux/pci.h>

#include <bcmpci.h>
#include <bcm_intr.h>
#include <bcm_map_part.h>

static volatile MpiRegisters * mpi = (MpiRegisters *)(MPI_BASE);

static char irq_tab_bcm96348[] __initdata = {
    [0] = INTERRUPT_ID_MPI,
    [1] = INTERRUPT_ID_MPI,
#if defined(CONFIG_USB)
    [USB_HOST_SLOT] = INTERRUPT_ID_USBH
#endif
};

int __init pcibios_map_irq(struct pci_dev *dev, u8 slot, u8 pin)
{
    return irq_tab_bcm96348[slot];
}

static void bcm96348_fixup(struct pci_dev *dev)
{
    uint32 memaddr;
    uint32 size;

    memaddr = pci_resource_start(dev, 0);
    size = pci_resource_len(dev, 0);

    switch (PCI_SLOT(dev->devfn)) {
        case 0:
            // UBUS to PCI address range
            // Memory Window 1. Mask determines which bits are decoded.
            mpi->l2pmrange1 = ~(size-1);
            // UBUS to PCI Memory base address. This is akin to the ChipSelect base
            // register.
            mpi->l2pmbase1 = memaddr & BCM_PCI_ADDR_MASK;
            // UBUS to PCI Remap Address. Replaces the masked address bits in the
            // range register with this setting.
            // Also, enable direct I/O and direct Memory accesses
            mpi->l2pmremap1 = (memaddr | MEM_WINDOW_EN);
            break;

        case 1:
            // Memory Window 2
            mpi->l2pmrange2 = ~(size-1);
            // UBUS to PCI Memory base address.
            mpi->l2pmbase2 = memaddr & BCM_PCI_ADDR_MASK;
            // UBUS to PCI Remap Address
            mpi->l2pmremap2 = (memaddr | MEM_WINDOW_EN);
            break;

#if defined(CONFIG_USB)
        case USB_HOST_SLOT:
            dev->resource[0].start = USB_HOST_BASE;
            dev->resource[0].end = USB_HOST_BASE+USB_BAR0_MEM_SIZE-1;
            break;
#endif
    }
}

int pcibios_plat_dev_init(struct pci_dev *dev)
{
        return 0;
}

DECLARE_PCI_FIXUP_FINAL(PCI_VENDOR_ID_BROADCOM, PCI_ANY_ID,
          bcm96348_fixup);

/*struct pci_fixup pcibios_fixups[] = {
    { PCI_FIXUP_FINAL, PCI_ANY_ID, PCI_ANY_ID, bcm96348_fixup },
    {0}
};*/

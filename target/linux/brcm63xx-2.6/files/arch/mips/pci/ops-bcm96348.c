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
#include <asm/addrspace.h>

#include <bcm_intr.h>
#include <bcm_map_part.h>
#include <bcmpci.h>

#include <linux/delay.h>

#if defined(CONFIG_USB)
#if 0
#define DPRINT(x...)        printk(x)
#else
#define DPRINT(x...)
#endif

static int 
pci63xx_int_read(unsigned int devfn, int where, u32 * value, int size);
static int 
pci63xx_int_write(unsigned int devfn, int where, u32 * value, int size);

static bool usb_mem_size_rd = FALSE;
static uint32 usb_mem_base = 0;
static uint32 usb_cfg_space_cmd_reg = 0;
#endif
static bool pci_mem_size_rd = FALSE;

static volatile MpiRegisters * mpi = (MpiRegisters *)(MPI_BASE);

static void mpi_SetupPciConfigAccess(uint32 addr)
{
    mpi->l2pcfgctl = (DIR_CFG_SEL | DIR_CFG_USEREG | addr) & ~CONFIG_TYPE;
}

static void mpi_ClearPciConfigAccess(void)
{
    mpi->l2pcfgctl = 0x00000000;
}

#if defined(CONFIG_USB)
/* --------------------------------------------------------------------------
    Name: pci63xx_int_write
Abstract: PCI Config write on internal device(s)
 -------------------------------------------------------------------------- */
static int 
pci63xx_int_write(unsigned int devfn, int where, u32 * value, int size)
{
    if (PCI_SLOT(devfn) != USB_HOST_SLOT) {
        return PCIBIOS_SUCCESSFUL;
    }

    switch (size) {
        case 1:
            DPRINT("W => Slot: %d Where: %2X Len: %d Data: %02X\n", 
                PCI_SLOT(devfn), where, size, *value);
            break;
        case 2:
            DPRINT("W => Slot: %d Where: %2X Len: %d Data: %04X\n", 
                PCI_SLOT(devfn), where, size, *value);
            switch (where) {
                case PCI_COMMAND:
                    usb_cfg_space_cmd_reg = *value;
                    break;
                default:
                    break;
            }
            break;
        case 4:
            DPRINT("W => Slot: %d Where: %2X Len: %d Data: %08lX\n", 
                PCI_SLOT(devfn), where, size, *value);
            switch (where) {
                case PCI_BASE_ADDRESS_0:
                    if (*value == 0xffffffff) {
                        usb_mem_size_rd = TRUE;
                    } else {
                        usb_mem_base = *value;
                    }
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }

    return PCIBIOS_SUCCESSFUL;
}

/* --------------------------------------------------------------------------
    Name: pci63xx_int_read
Abstract: PCI Config read on internal device(s)
 -------------------------------------------------------------------------- */
static int 
pci63xx_int_read(unsigned int devfn, int where, u32 * value, int size)
{
    uint32 retValue = 0xFFFFFFFF;

    if (PCI_SLOT(devfn) != USB_HOST_SLOT) {
        return PCIBIOS_SUCCESSFUL;
    }

    // For now, this is specific to the USB Host controller. We can
    // make it more general if we have to...
    // Emulate PCI Config accesses
    switch (where) {
        case PCI_VENDOR_ID:     
        case PCI_DEVICE_ID:
            retValue = PCI_VENDOR_ID_BROADCOM | 0x63000000;
            break;
        case PCI_COMMAND:
        case PCI_STATUS:
            retValue = (0x0006 << 16) | usb_cfg_space_cmd_reg;
            break;
        case PCI_CLASS_REVISION:
        case PCI_CLASS_DEVICE:
            retValue = (PCI_CLASS_SERIAL_USB << 16) | (0x10 << 8) | 0x01;
            break;
        case PCI_BASE_ADDRESS_0:
            if (usb_mem_size_rd) {
                retValue = USB_BAR0_MEM_SIZE;
            } else {
                if (usb_mem_base != 0)
                    retValue = usb_mem_base;
                else
                    retValue = USB_HOST_BASE;
            }
            usb_mem_size_rd = FALSE;
            break;
        case PCI_CACHE_LINE_SIZE:
        case PCI_LATENCY_TIMER:
            retValue = 0;
            break;
        case PCI_HEADER_TYPE:
            retValue = PCI_HEADER_TYPE_NORMAL;
            break;
        case PCI_SUBSYSTEM_VENDOR_ID:
            retValue = PCI_VENDOR_ID_BROADCOM;
            break;
        case PCI_SUBSYSTEM_ID:
            retValue = 0x6300;
            break;
        case PCI_INTERRUPT_LINE:
            retValue = INTERRUPT_ID_USBH; 
            break;
        default:
            break;
    }

    switch (size) {
        case 1:
            *value = (retValue >> ((where & 3) << 3)) & 0xff;
            DPRINT("R <= Slot: %d Where: %2X Len: %d Data: %02X\n", 
                PCI_SLOT(devfn), where, size, *value);
            break;
        case 2:
            *value = (retValue >> ((where & 3) << 3)) & 0xffff;
            DPRINT("R <= Slot: %d Where: %2X Len: %d Data: %04X\n", 
                PCI_SLOT(devfn), where, size, *value);
            break;
        case 4:
            *value = retValue;
            DPRINT("R <= Slot: %d Where: %2X Len: %d Data: %08lX\n", 
                PCI_SLOT(devfn), where, size, *value);
            break;
        default:
            break;
    }

    return PCIBIOS_SUCCESSFUL;
}
#endif

static int bcm96348_pcibios_read(struct pci_bus *bus, unsigned int devfn,
	int where, int size, u32 * val)
{
    volatile unsigned char *ioBase = (unsigned char *)(mpi->l2piobase | KSEG1);
    uint32 data;

#if defined(CONFIG_USB)
    if (PCI_SLOT(devfn) == USB_HOST_SLOT)
        return pci63xx_int_read(devfn, where, val, size);
#endif

    mpi_SetupPciConfigAccess(BCM_PCI_CFG(PCI_SLOT(devfn), PCI_FUNC(devfn), where));
    data = *(uint32 *)ioBase;
    switch(size) {
        case 1:
            *val = (data >> ((where & 3) << 3)) & 0xff;
            break;
        case 2:
            *val = (data >> ((where & 3) << 3)) & 0xffff;
            break;
        case 4:
            *val = data;
             /* Special case for reading PCI device range */
            if ((where >= PCI_BASE_ADDRESS_0) && (where <= PCI_BASE_ADDRESS_5)) {
                if (pci_mem_size_rd) {
                    /* bcm6348 PCI memory window minimum size is 64K */
                    *val &= PCI_SIZE_64K;
                }
            }
            break;
        default:
            break;
    }
    pci_mem_size_rd = FALSE;
    mpi_ClearPciConfigAccess();

    return PCIBIOS_SUCCESSFUL;
}

static int bcm96348_pcibios_write(struct pci_bus *bus, unsigned int devfn,
	int where, int size, u32 val)
{
    volatile unsigned char *ioBase = (unsigned char *)(mpi->l2piobase | KSEG1);
    uint32 data;

#if defined(CONFIG_USB)
    if (PCI_SLOT(devfn) == USB_HOST_SLOT)
        return pci63xx_int_write(devfn, where, &val, size);
#endif
    mpi_SetupPciConfigAccess(BCM_PCI_CFG(PCI_SLOT(devfn), PCI_FUNC(devfn), where));
    data = *(uint32 *)ioBase;
    switch(size) {
        case 1:
            data = (data & ~(0xff << ((where & 3) << 3))) |
                (val << ((where & 3) << 3));
            break;
        case 2:
            data = (data & ~(0xffff << ((where & 3) << 3))) |
                (val << ((where & 3) << 3));
            break;
        case 4:
            data = val;
            /* Special case for reading PCI device range */
            if ((where >= PCI_BASE_ADDRESS_0) && (where <= PCI_BASE_ADDRESS_5)) {
                if (val == 0xffffffff)
                    pci_mem_size_rd = TRUE;
            }
            break;
        default:
            break;
    }
    *(uint32 *)ioBase = data;
    udelay(500);
    mpi_ClearPciConfigAccess();

    return PCIBIOS_SUCCESSFUL;
}

struct pci_ops bcm96348_pci_ops = {
    .read   = bcm96348_pcibios_read,
    .write  = bcm96348_pcibios_write
};

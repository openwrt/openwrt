/*
<:copyright-gpl 
 Copyright 2004 Broadcom Corp. All Rights Reserved. 
 
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

//
// bcmpci.h - bcm96348 PCI, Cardbus, and PCMCIA definition
//
#ifndef BCMPCI_H
#define BCMPCI_H

/* Memory window in internal system bus address space */ 
#define BCM_PCI_MEM_BASE        0x08000000
/* IO window in internal system bus address space */ 
#define BCM_PCI_IO_BASE         0x0C000000

#define BCM_PCI_ADDR_MASK       0x1fffffff

/* Memory window size (range) */
#define BCM_PCI_MEM_SIZE_16MB   0x01000000
/* IO window size (range) */
#define BCM_PCI_IO_SIZE_64KB    0x00010000

/* PCI Configuration and I/O space acesss */
#define BCM_PCI_CFG(d, f, o)    ( (d << 11) | (f << 8) | (o/4 << 2) )

/* fake USB PCI slot */
#define USB_HOST_SLOT           9
#define USB_BAR0_MEM_SIZE       0x0800

#define BCM_HOST_MEM_SPACE1     0x10000000
#define BCM_HOST_MEM_SPACE2     0x00000000

/* 
 * EBI bus clock is 33MHz and share with PCI bus
 * each clock cycle is 30ns.
 */
/* attribute memory access wait cnt for 4306 */
#define PCMCIA_ATTR_CE_HOLD     3  // data hold time 70ns
#define PCMCIA_ATTR_CE_SETUP    3  // data setup time 50ns
#define PCMCIA_ATTR_INACTIVE    6  // time between read/write cycles 180ns. For the total cycle time 600ns (cnt1+cnt2+cnt3+cnt4)
#define PCMCIA_ATTR_ACTIVE      10 // OE/WE pulse width 300ns

/* common memory access wait cnt for 4306 */
#define PCMCIA_MEM_CE_HOLD      1  // data hold time 30ns
#define PCMCIA_MEM_CE_SETUP     1  // data setup time 30ns
#define PCMCIA_MEM_INACTIVE     2  // time between read/write cycles 40ns. For the total cycle time 250ns (cnt1+cnt2+cnt3+cnt4)
#define PCMCIA_MEM_ACTIVE       5  // OE/WE pulse width 150ns

#define PCCARD_VCC_MASK     0x00070000  // Mask Reset also
#define PCCARD_VCC_33V      0x00010000
#define PCCARD_VCC_50V      0x00020000

typedef enum {
    MPI_CARDTYPE_NONE,      // No Card in slot
    MPI_CARDTYPE_PCMCIA,    // 16-bit PCMCIA card in slot    
    MPI_CARDTYPE_CARDBUS,   // 32-bit CardBus card in slot
}   CardType;

#define CARDBUS_SLOT        0    // Slot 0 is default for CardBus

#define pcmciaAttrOffset    0x00200000
#define pcmciaMemOffset     0x00000000
// Needs to be right above PCI I/O space. Give 0x8000 (32K) to PCMCIA. 
#define pcmciaIoOffset      (BCM_PCI_IO_BASE + 0x80000)
// Base Address is that mapped into the MPI ChipSelect registers. 
// UBUS bridge MemoryWindow 0 outputs a 0x00 for the base.
#define pcmciaBase          0xbf000000
#define pcmciaAttr          (pcmciaAttrOffset | pcmciaBase)
#define pcmciaMem           (pcmciaMemOffset  | pcmciaBase)
#define pcmciaIo            (pcmciaIoOffset   | pcmciaBase)

#endif

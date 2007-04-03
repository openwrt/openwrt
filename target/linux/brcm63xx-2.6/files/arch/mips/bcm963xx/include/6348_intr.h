/*
<:copyright-gpl 
 Copyright 2003 Broadcom Corp. All Rights Reserved. 
 
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

#ifndef __6348_INTR_H
#define __6348_INTR_H


/*=====================================================================*/
/* BCM6348 External Interrupt Level Assignments                       */
/*=====================================================================*/
#define INTERRUPT_ID_EXTERNAL_0         3
#define INTERRUPT_ID_EXTERNAL_1         4
#define INTERRUPT_ID_EXTERNAL_2         5
#define INTERRUPT_ID_EXTERNAL_3         6

/*=====================================================================*/
/* BCM6348 Timer Interrupt Level Assignments                          */
/*=====================================================================*/
#define MIPS_TIMER_INT                  7

/*=====================================================================*/
/* Peripheral ISR Table Offset                                              */
/*=====================================================================*/
#define INTERNAL_ISR_TABLE_OFFSET       8

/*=====================================================================*/
/* Logical Peripheral Interrupt IDs                                    */
/*=====================================================================*/

#define INTERRUPT_ID_TIMER               (INTERNAL_ISR_TABLE_OFFSET + 0)
#define INTERRUPT_ID_SPI                 (INTERNAL_ISR_TABLE_OFFSET + 1)
#define INTERRUPT_ID_UART                (INTERNAL_ISR_TABLE_OFFSET + 2)
#define INTERRUPT_ID_ADSL                (INTERNAL_ISR_TABLE_OFFSET + 4)
#define INTERRUPT_ID_ATM                 (INTERNAL_ISR_TABLE_OFFSET + 5)
#define INTERRUPT_ID_USBS                (INTERNAL_ISR_TABLE_OFFSET + 6)
#define INTERRUPT_ID_EMAC2               (INTERNAL_ISR_TABLE_OFFSET + 7)
#define INTERRUPT_ID_EMAC1               (INTERNAL_ISR_TABLE_OFFSET + 8)
#define INTERRUPT_ID_EPHY                (INTERNAL_ISR_TABLE_OFFSET + 9)
#define INTERRUPT_ID_M2M                 (INTERNAL_ISR_TABLE_OFFSET + 10)
#define INTERRUPT_ID_ACLC                (INTERNAL_ISR_TABLE_OFFSET + 11)
#define INTERRUPT_ID_USBH                (INTERNAL_ISR_TABLE_OFFSET + 12)
#define INTERRUPT_ID_SDRAM               (INTERNAL_ISR_TABLE_OFFSET + 13)
#define INTERRUPT_ID_USB_CNTL_RX_DMA     (INTERNAL_ISR_TABLE_OFFSET + 14)
#define INTERRUPT_ID_USB_CNTL_TX_DMA     (INTERNAL_ISR_TABLE_OFFSET + 15)
#define INTERRUPT_ID_USB_BULK_RX_DMA     (INTERNAL_ISR_TABLE_OFFSET + 16)
#define INTERRUPT_ID_USB_BULK_TX_DMA     (INTERNAL_ISR_TABLE_OFFSET + 17)
#define INTERRUPT_ID_USB_ISO_RX_DMA      (INTERNAL_ISR_TABLE_OFFSET + 18)
#define INTERRUPT_ID_USB_ISO_TX_DMA      (INTERNAL_ISR_TABLE_OFFSET + 19)
#define INTERRUPT_ID_EMAC1_RX_DMA        (INTERNAL_ISR_TABLE_OFFSET + 20)
#define INTERRUPT_ID_EMAC1_TX_DMA        (INTERNAL_ISR_TABLE_OFFSET + 21)
#define INTERRUPT_ID_EMAC2_RX_DMA        (INTERNAL_ISR_TABLE_OFFSET + 22)
#define INTERRUPT_ID_EMAC2_TX_DMA        (INTERNAL_ISR_TABLE_OFFSET + 23)
#define INTERRUPT_ID_MPI                 (INTERNAL_ISR_TABLE_OFFSET + 24)
#define INTERRUPT_ID_DG                  (INTERNAL_ISR_TABLE_OFFSET + 25)


#endif  /* __BCM6348_H */


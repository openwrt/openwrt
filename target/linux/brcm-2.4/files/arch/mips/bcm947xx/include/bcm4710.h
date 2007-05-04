/*
 * BCM4710 address space map and definitions
 * Think twice before adding to this file, this is not the kitchen sink
 * These definitions are not guaranteed for all 47xx chips, only the 4710
 *
 * Copyright 2004, Broadcom Corporation   
 * All Rights Reserved.   
 *    
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY   
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM   
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS   
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.   
 *
 * $Id: bcm4710.h,v 1.3 2004/09/27 07:23:30 tallest Exp $
 */

#ifndef _bcm4710_h_
#define _bcm4710_h_

/* Address map */
#define BCM4710_SDRAM		0x00000000	/* Physical SDRAM */
#define BCM4710_PCI_MEM		0x08000000	/* Host Mode PCI memory access space (64 MB) */
#define BCM4710_PCI_CFG		0x0c000000	/* Host Mode PCI configuration space (64 MB) */
#define BCM4710_PCI_DMA		0x40000000	/* Client Mode PCI memory access space (1 GB) */
#define	BCM4710_SDRAM_SWAPPED	0x10000000	/* Byteswapped Physical SDRAM */
#define BCM4710_ENUM		0x18000000	/* Beginning of core enumeration space */

/* Core register space */
#define BCM4710_REG_SDRAM	0x18000000	/* SDRAM core registers */
#define BCM4710_REG_ILINE20	0x18001000	/* InsideLine20 core registers */
#define BCM4710_REG_EMAC0	0x18002000	/* Ethernet MAC 0 core registers */
#define BCM4710_REG_CODEC	0x18003000	/* Codec core registers */
#define BCM4710_REG_USB		0x18004000	/* USB core registers */
#define BCM4710_REG_PCI		0x18005000	/* PCI core registers */
#define BCM4710_REG_MIPS	0x18006000	/* MIPS core registers */
#define BCM4710_REG_EXTIF	0x18007000	/* External Interface core registers */
#define BCM4710_REG_EMAC1	0x18008000	/* Ethernet MAC 1 core registers */

#define	BCM4710_EXTIF		0x1f000000	/* External Interface base address */
#define BCM4710_PCMCIA_MEM	0x1f000000	/* External Interface PCMCIA memory access */
#define BCM4710_PCMCIA_IO	0x1f100000	/* PCMCIA I/O access */
#define BCM4710_PCMCIA_CONF	0x1f200000	/* PCMCIA configuration */
#define BCM4710_PROG		0x1f800000	/* Programable interface */
#define BCM4710_FLASH		0x1fc00000	/* Flash */

#define	BCM4710_EJTAG		0xff200000	/* MIPS EJTAG space (2M) */

#define	BCM4710_UART		(BCM4710_REG_EXTIF + 0x00000300)

#define	BCM4710_EUART		(BCM4710_EXTIF + 0x00800000)
#define	BCM4710_LED		(BCM4710_EXTIF + 0x00900000)

#define	SBFLAG_PCI	0
#define	SBFLAG_ENET0	1
#define	SBFLAG_ILINE20	2
#define	SBFLAG_CODEC	3
#define	SBFLAG_USB	4
#define	SBFLAG_EXTIF	5
#define	SBFLAG_ENET1	6

#ifdef	CONFIG_HWSIM
#define	BCM4710_TRACE(trval)        do { *((int *)0xa0000f18) = (trval); } while (0)
#else
#define	BCM4710_TRACE(trval)
#endif


/* BCM94702 CPCI -ExtIF used for LocalBus devs */

#define BCM94702_CPCI_RESET_ADDR    	 BCM4710_EXTIF
#define BCM94702_CPCI_BOARDID_ADDR  	(BCM4710_EXTIF | 0x4000)
#define BCM94702_CPCI_DOC_ADDR      	(BCM4710_EXTIF | 0x6000)
#define BCM94702_DOC_ADDR                BCM94702_CPCI_DOC_ADDR
#define BCM94702_CPCI_LED_ADDR      	(BCM4710_EXTIF | 0xc000)
#define BCM94702_CPCI_NVRAM_ADDR    	(BCM4710_EXTIF | 0xe000)
#define BCM94702_CPCI_NVRAM_SIZE         0x1ff0 /* 8K NVRAM : DS1743/STM48txx*/
#define BCM94702_CPCI_TOD_REG_BASE       (BCM94702_CPCI_NVRAM_ADDR | 0x1ff0)

#define LED_REG(x)      \
 (*(volatile unsigned char *) (KSEG1ADDR(BCM94702_CPCI_LED_ADDR) + (x)))

/* 
 * Reset function implemented in PLD.  Read or write should trigger hard reset 
 */
#define SYS_HARD_RESET()   \
    { for (;;) \
     *( (volatile unsigned char *)\
      KSEG1ADDR(BCM94702_CPCI_RESET_ADDR) ) = 0x80; \
    }

#endif /* _bcm4710_h_ */

/*
 * BCM47XX Sonics SiliconBackplane PCI core hardware definitions.
 *
 * $Id$
 * Copyright 2004, Broadcom Corporation      
 * All Rights Reserved.      
 *       
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY      
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM      
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS      
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.      
 */

#ifndef	_SBPCI_H
#define	_SBPCI_H

/* cpp contortions to concatenate w/arg prescan */
#ifndef PAD
#define	_PADLINE(line)	pad ## line
#define	_XSTR(line)	_PADLINE(line)
#define	PAD		_XSTR(__LINE__)
#endif

/* Sonics side: PCI core and host control registers */
typedef struct sbpciregs {
	uint32 control;		/* PCI control */
	uint32 PAD[3];
	uint32 arbcontrol;	/* PCI arbiter control */
	uint32 PAD[3];
	uint32 intstatus;	/* Interrupt status */
	uint32 intmask;		/* Interrupt mask */
	uint32 sbtopcimailbox;	/* Sonics to PCI mailbox */
	uint32 PAD[9];
	uint32 bcastaddr;	/* Sonics broadcast address */
	uint32 bcastdata;	/* Sonics broadcast data */
	uint32 PAD[2];
	uint32 gpioin;		/* ro: gpio input (>=rev2) */
	uint32 gpioout;		/* rw: gpio output (>=rev2) */
	uint32 gpioouten;	/* rw: gpio output enable (>= rev2) */
	uint32 gpiocontrol;	/* rw: gpio control (>= rev2) */
	uint32 PAD[36];
	uint32 sbtopci0;	/* Sonics to PCI translation 0 */
	uint32 sbtopci1;	/* Sonics to PCI translation 1 */
	uint32 sbtopci2;	/* Sonics to PCI translation 2 */
	uint32 PAD[445];
	uint16 sprom[36];	/* SPROM shadow Area */
	uint32 PAD[46];
} sbpciregs_t;

/* PCI control */
#define PCI_RST_OE	0x01	/* When set, drives PCI_RESET out to pin */
#define PCI_RST		0x02	/* Value driven out to pin */
#define PCI_CLK_OE	0x04	/* When set, drives clock as gated by PCI_CLK out to pin */
#define PCI_CLK		0x08	/* Gate for clock driven out to pin */	

/* PCI arbiter control */
#define PCI_INT_ARB	0x01	/* When set, use an internal arbiter */
#define PCI_EXT_ARB	0x02	/* When set, use an external arbiter */
#define PCI_PARKID_MASK	0x06	/* Selects which agent is parked on an idle bus */
#define PCI_PARKID_SHIFT   1
#define PCI_PARKID_LAST	   0	/* Last requestor */
#define PCI_PARKID_4710	   1	/* 4710 */
#define PCI_PARKID_EXTREQ0 2	/* External requestor 0 */
#define PCI_PARKID_EXTREQ1 3	/* External requestor 1 */

/* Interrupt status/mask */
#define PCI_INTA	0x01	/* PCI INTA# is asserted */
#define PCI_INTB	0x02	/* PCI INTB# is asserted */
#define PCI_SERR	0x04	/* PCI SERR# has been asserted (write one to clear) */
#define PCI_PERR	0x08	/* PCI PERR# has been asserted (write one to clear) */
#define PCI_PME		0x10	/* PCI PME# is asserted */

/* (General) PCI/SB mailbox interrupts, two bits per pci function */
#define	MAILBOX_F0_0	0x100	/* function 0, int 0 */
#define	MAILBOX_F0_1	0x200	/* function 0, int 1 */
#define	MAILBOX_F1_0	0x400	/* function 1, int 0 */
#define	MAILBOX_F1_1	0x800	/* function 1, int 1 */
#define	MAILBOX_F2_0	0x1000	/* function 2, int 0 */
#define	MAILBOX_F2_1	0x2000	/* function 2, int 1 */
#define	MAILBOX_F3_0	0x4000	/* function 3, int 0 */
#define	MAILBOX_F3_1	0x8000	/* function 3, int 1 */

/* Sonics broadcast address */
#define BCAST_ADDR_MASK	0xff	/* Broadcast register address */

/* Sonics to PCI translation types */
#define SBTOPCI0_MASK	0xfc000000
#define SBTOPCI1_MASK	0xfc000000
#define SBTOPCI2_MASK	0xc0000000
#define SBTOPCI_MEM	0
#define SBTOPCI_IO	1
#define SBTOPCI_CFG0	2
#define SBTOPCI_CFG1	3
#define	SBTOPCI_PREF	0x4	/* prefetch enable */
#define	SBTOPCI_BURST	0x8	/* burst enable */

/* PCI side: Reserved PCI configuration registers (see pcicfg.h) */
#define cap_list	rsvd_a[0]
#define bar0_window	dev_dep[0x80 - 0x40]
#define bar1_window	dev_dep[0x84 - 0x40]
#define sprom_control	dev_dep[0x88 - 0x40]

#ifndef _LANGUAGE_ASSEMBLY

extern int sbpci_read_config(void *sbh, uint bus, uint dev, uint func, uint off, void *buf, int len);
extern int sbpci_write_config(void *sbh, uint bus, uint dev, uint func, uint off, void *buf, int len);
extern void sbpci_ban(uint16 core);
extern int sbpci_init(void *sbh);
extern void sbpci_check(void *sbh);

#endif /* !_LANGUAGE_ASSEMBLY */

#endif	/* _SBPCI_H */

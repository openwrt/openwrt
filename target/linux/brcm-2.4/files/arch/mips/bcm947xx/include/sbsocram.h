/*
 * BCM47XX Sonics SiliconBackplane embedded ram core
 *
 * Copyright 2007, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id$
 */

#ifndef	_SBSOCRAM_H
#define	_SBSOCRAM_H

#ifndef _LANGUAGE_ASSEMBLY

/* cpp contortions to concatenate w/arg prescan */
#ifndef PAD
#define	_PADLINE(line)	pad ## line
#define	_XSTR(line)	_PADLINE(line)
#define	PAD		_XSTR(__LINE__)
#endif	/* PAD */

/* Memcsocram core registers */
typedef volatile struct sbsocramregs {
	uint32	coreinfo;
	uint32	bwalloc;
	uint32	PAD;
	uint32	biststat;
	uint32	bankidx;
	uint32	standbyctrl;
	uint32	PAD[116];
	uint32	pwrctl;		/* corerev >= 2 */
} sbsocramregs_t;

#endif	/* _LANGUAGE_ASSEMBLY */

/* Register offsets */
#define	SR_COREINFO		0x00
#define	SR_BWALLOC		0x04
#define	SR_BISTSTAT		0x0c
#define	SR_BANKINDEX		0x10
#define	SR_BANKSTBYCTL		0x14
#define SR_PWRCTL		0x1e8

/* Coreinfo register */
#define	SRCI_PT_MASK		0x00030000
#define	SRCI_PT_SHIFT		16
/* corerev >= 3 */
#define SRCI_LSS_MASK		0x00f00000
#define SRCI_LSS_SHIFT		20
#define SRCI_LRS_MASK		0x0f000000
#define SRCI_LRS_SHIFT		24

/* In corerev 0, the memory size is 2 to the power of the
 * base plus 16 plus to the contents of the memsize field plus 1.
 */
#define	SRCI_MS0_MASK		0xf
#define SR_MS0_BASE		16

/*
 * In corerev 1 the bank size is 2 ^ the bank size field plus 14,
 * the memory size is number of banks times bank size.
 * The same applies to rom size.
 */
#define	SRCI_ROMNB_MASK		0xf000
#define	SRCI_ROMNB_SHIFT	12
#define	SRCI_ROMBSZ_MASK	0xf00
#define	SRCI_ROMBSZ_SHIFT	8
#define	SRCI_SRNB_MASK		0xf0
#define	SRCI_SRNB_SHIFT		4
#define	SRCI_SRBSZ_MASK		0xf
#define	SRCI_SRBSZ_SHIFT	0

#define SR_BSZ_BASE		14

/* Standby control register */
#define	SRSC_SBYOVR_MASK		0x80000000
#define	SRSC_SBYOVR_SHIFT		31
#define	SRSC_SBYOVRVAL_MASK		0x60000000
#define	SRSC_SBYOVRVAL_SHIFT		29

#endif	/* _SBSOCRAM_H */

/*
 * BCM47XX Sonics SiliconBackplane embedded ram core
 *
 * Copyright 2006, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id: sbsocram.h,v 1.1.1.3 2006/02/27 03:43:16 honor Exp $
 */

#ifndef	_SBSOCRAM_H
#define	_SBSOCRAM_H

#define	SR_COREINFO		0x00
#define	SR_BWALLOC		0x04
#define	SR_BISTSTAT		0x0c
#define	SR_BANKINDEX		0x10
#define	SR_BANKSTBYCTL		0x14


#ifndef _LANGUAGE_ASSEMBLY

/* Memcsocram core registers */
typedef volatile struct sbsocramregs {
	uint32	coreinfo;
	uint32	bwalloc;
	uint32	PAD;
	uint32	biststat;
	uint32	bankidx;
	uint32	standbyctrl;
} sbsocramregs_t;

#endif

/* Coreinfo register */
#define	SRCI_PT_MASK		0x30000
#define	SRCI_PT_SHIFT		16

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
#endif	/* _SBSOCRAM_H */

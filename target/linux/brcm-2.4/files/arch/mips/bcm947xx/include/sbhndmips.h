/*
 * Broadcom SiliconBackplane MIPS definitions
 *
 * SB MIPS cores are custom MIPS32 processors with SiliconBackplane
 * OCP interfaces. The CP0 processor ID is 0x00024000, where bits
 * 23:16 mean Broadcom and bits 15:8 mean a MIPS core with an OCP
 * interface. The core revision is stored in the SB ID register in SB
 * configuration space.
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

#ifndef	_sbhndmips_h_
#define	_sbhndmips_h_

#include <mipsinc.h>

#ifndef _LANGUAGE_ASSEMBLY

/* cpp contortions to concatenate w/arg prescan */
#ifndef PAD
#define	_PADLINE(line)	pad ## line
#define	_XSTR(line)	_PADLINE(line)
#define	PAD		_XSTR(__LINE__)
#endif	/* PAD */

typedef volatile struct {
	uint32	corecontrol;
	uint32	PAD[2];
	uint32	biststatus;
	uint32	PAD[4];
	uint32	intstatus;
	uint32	intmask;
	uint32	timer;
} mipsregs_t;

#endif	/* _LANGUAGE_ASSEMBLY */

#endif	/* _sbhndmips_h_ */

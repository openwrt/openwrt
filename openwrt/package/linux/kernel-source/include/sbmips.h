/*
 * Broadcom SiliconBackplane MIPS definitions
 *
 * SB MIPS cores are custom MIPS32 processors with SiliconBackplane
 * OCP interfaces. The CP0 processor ID is 0x00024000, where bits
 * 23:16 mean Broadcom and bits 15:8 mean a MIPS core with an OCP
 * interface. The core revision is stored in the SB ID register in SB
 * configuration space.
 *
 * Copyright 2004, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id$
 */

#ifndef	_SBMIPS_H
#define	_SBMIPS_H

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

extern uint32 sb_flag(void *sbh);
extern uint sb_irq(void *sbh);

extern void sb_serial_init(void *sbh, void (*add)(void *regs, uint irq, uint baud_base, uint reg_shift));

extern void sb_mips_init(void *sbh);
extern uint32 sb_mips_clock(void *sbh);
extern bool sb_mips_setclock(void *sbh, uint32 mipsclock, uint32 sbclock, uint32 pciclock);

extern uint32 sb_memc_get_ncdl(void *sbh);

#endif /* _LANGUAGE_ASSEMBLY */

#endif	/* _SBMIPS_H */

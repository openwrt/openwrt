/*
 * HND SiliconBackplane MIPS/ARM cores software interface.
 *
 * Copyright 2006, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id: hndcpu.h,v 1.1.1.1 2006/02/27 03:43:16 honor Exp $
 */

#ifndef _hndcpu_h_
#define _hndcpu_h_

#if defined(mips)
#include <hndmips.h>
#elif defined(__ARM_ARCH_4T__)
#include <hndarm.h>
#endif

extern uint sb_irq(sb_t *sbh);
extern uint32 sb_cpu_clock(sb_t *sbh);
extern void sb_cpu_wait(void);

#endif /* _hndcpu_h_ */

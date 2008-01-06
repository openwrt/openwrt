/*
 * HND SiliconBackplane MIPS/ARM cores software interface.
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

#ifndef _hndcpu_h_
#define _hndcpu_h_

#if defined(mips)
#include <hndmips.h>
#elif defined(__arm__) || defined(__thumb__) || defined(__thumb2__)
#include <hndarm.h>
#endif

extern uint sb_irq(sb_t *sbh);
extern uint32 sb_cpu_clock(sb_t *sbh);
extern void hnd_cpu_wait(sb_t *sbh);
extern void hnd_cpu_jumpto(void *addr);
extern void hnd_cpu_reset(sb_t *sbh);

#endif /* _hndcpu_h_ */

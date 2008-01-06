/*
 * HND SiliconBackplane chipcommon support.
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

#ifndef _hndchipc_h_
#define _hndchipc_h_

typedef void (*sb_serial_init_fn)(void *regs, uint irq, uint baud_base, uint reg_shift);

extern void sb_serial_init(sb_t *sbh, sb_serial_init_fn add);

extern void *sb_jtagm_init(sb_t *sbh, uint clkd, bool exttap);
extern void sb_jtagm_disable(osl_t *osh, void *h);
extern uint32 jtag_rwreg(osl_t *osh, void *h, uint32 ir, uint32 dr);

typedef	void (*cc_isr_fn)(void* cbdata, uint32 ccintst);

extern bool sb_cc_register_isr(sb_t *sbh, cc_isr_fn isr, uint32 ccintmask, void *cbdata);
extern void sb_cc_isr(sb_t *sbh, chipcregs_t *regs);

#endif /* _hndchipc_h_ */

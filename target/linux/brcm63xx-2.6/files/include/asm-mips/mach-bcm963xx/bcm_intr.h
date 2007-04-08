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

#ifndef __BCM_INTR_H
#define __BCM_INTR_H

#ifdef __cplusplus
    extern "C" {
#endif

#if defined(CONFIG_BCM96338)
#include <6338_intr.h>
#endif
#if defined(CONFIG_BCM96345)
#include <6345_intr.h>
#endif
#if defined(CONFIG_BCM96348)
#include <6348_intr.h>
#endif

/* defines */
struct pt_regs;
typedef int (*FN_HANDLER) (int, void *);

/* prototypes */
extern void enable_brcm_irq(unsigned int irq);
extern void disable_brcm_irq(unsigned int irq);
extern int request_external_irq(unsigned int irq,
    FN_HANDLER handler, unsigned long irqflags, 
    const char * devname, void *dev_id);
extern unsigned int BcmHalMapInterrupt(FN_HANDLER isr, unsigned int param,
    unsigned int interruptId);
extern void dump_intr_regs(void);

/* compatibility definitions */
#define BcmHalInterruptEnable(irq)      enable_brcm_irq( irq )
#define BcmHalInterruptDisable(irq)     disable_brcm_irq( irq )

#ifdef __cplusplus
    }
#endif

#endif

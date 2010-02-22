/*
 * arch/ubicom32/include/asm/irq.h
 *   IRQ definitions for Ubicom32 architecture.
 *
 * (C) Copyright 2009, Ubicom, Inc.
 *
 * This file is part of the Ubicom32 Linux Kernel Port.
 *
 * The Ubicom32 Linux Kernel Port is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 2 of the
 * License, or (at your option) any later version.
 *
 * The Ubicom32 Linux Kernel Port is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 * the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Ubicom32 Linux Kernel Port.  If not,
 * see <http://www.gnu.org/licenses/>.
 *
 * Ubicom32 implementation derived from (with many thanks):
 *   arch/m68knommu
 *   arch/blackfin
 *   arch/parisc
 */
#ifndef _ASM_UBICOM32_IRQ_H
#define _ASM_UBICOM32_IRQ_H

#include <asm/irqflags.h>

/*
 * We setup the IRQS to cover the full range of interrupt registers in
 * processor.
 */
#define NR_IRQS		64

#define irq_canonicalize(irq) (irq)

extern int irq_soft_alloc(unsigned int *soft);
extern void ack_bad_irq(unsigned int irq);
extern void do_IRQ(int irq, struct pt_regs *fp);

#endif /* _ASM_UBICOM32_IRQ_H */

/*
 * arch/ubicom32/include/asm/hardirq.h
 *   Definition of ack_bad_irq() for Ubicom32 architecture.
 *
 * (C) Copyright 2009, Ubicom, Inc.
 * Copyright (C) 1997, 98, 99, 2000, 01, 05 Ralf Baechle (ralf@linux-mips.org)
 * Copyright (C) 1999, 2000 Silicon Graphics, Inc.
 * Copyright (C) 2001 MIPS Technologies, Inc.
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
#ifndef _ASM_UBICOM32_HARDIRQ_H
#define _ASM_UBICOM32_HARDIRQ_H

#include <linux/threads.h>
#include <linux/irq.h>

/*
 * The hardirq mask has to be large enough to have space
 * for potentially all IRQ sources in the system nesting
 * on a single CPU.  For Ubicom32, we have 64 IRQ sources.
 */
#define HARDIRQ_BITS	6
#if (1 << HARDIRQ_BITS) < NR_IRQS
# error HARDIRQ_BITS is too low!
#endif

typedef struct {
	unsigned int __softirq_pending;
} ____cacheline_aligned irq_cpustat_t;

#include <linux/irq_cpustat.h>	/* Standard mappings for irq_cpustat_t above */

extern void ack_bad_irq(unsigned int irq);

#endif /* _ASM_UBICOM32_HARDIRQ_H */

/*
 * arch/ubicom32/include/asm/irqflags.h
 *   Raw implementation of local IRQ functions.
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
#ifndef _ASM_UBICOM32_IRQFLAGS_H
#define _ASM_UBICOM32_IRQFLAGS_H

#include <linux/thread_info.h>
#include <asm/ubicom32-common.h>
#if defined(CONFIG_SMP)
#include <asm/smp.h>
#endif
#include <asm/ldsr.h>

#if defined(CONFIG_PREEMPT)
#error Not supported by Ubicom32 irq handling, yet!
#endif

/*
 * raw_local_irq_enable()
 *	Enable interrupts for this thread.
 */
static inline void raw_local_irq_enable(void)
{
	ldsr_local_irq_enable();
}

/*
 * raw_local_irq_disable()
 *	Disable interrupts for this thread.
 */
static inline void raw_local_irq_disable(void)
{
	ldsr_local_irq_disable();
}

/*
 * raw_local_save_flags()
 *	Get the current IRQ state.
 */
#define raw_local_save_flags(flags) 		\
do {						\
	(flags) = ldsr_local_irq_is_disabled();	\
} while (0)

/*
 * raw_local_irq_save()
 *	Save the current interrupt state and disable interrupts.
 */
#define raw_local_irq_save(flags)		\
do {						\
	(flags) = ldsr_local_irq_save();	\
} while (0)

/*
 * raw_local_irq_restore()
 *	Restore the IRQ state back to flags.
 */
static inline void raw_local_irq_restore(unsigned long flags)
{
	ldsr_local_irq_restore(flags);
}

/*
 * raw_irqs_disabled_flags()
 *	Return true if the flags indicate that IRQ(s) are disabled.
 */
static inline int raw_irqs_disabled_flags(unsigned long flags)
{
	return (flags);
}

#endif /* _ASM_UBICOM32_IRQFLAGS_H */

/*
 * arch/ubicom32/include/asm/ldsr.h
 *   Ubicom32 LDSR interface definitions.
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
#ifndef _ASM_UBICOM32_LDSR_H
#define _ASM_UBICOM32_LDSR_H

#include <asm/ubicom32-common.h>
#include <asm/types.h>
#include <asm/thread.h>

extern unsigned int ldsr_soft_irq_mask;

/*
 * ldsr_local_irq_is_disabled()
 *	Test if interrupts are disabled for this thread?
 */
static inline int ldsr_local_irq_is_disabled(void)
{
	int ret;
	thread_t self = thread_get_self();
	unsigned int mask = (1 << self);

	asm volatile (
	"	and.4	%0, scratchpad1, %1	\n\t"
		: "=r" (ret)
		: "d" (mask)
		: "cc"
	);

	/*
	 *  We return a simple 1 == disabled, 0 == enabled
	 *  losing which tid this is for, because Linux
	 *  can restore interrupts on a different thread.
	 */
	return ret >> self;
}

/*
 * ldsr_local_irq_save()
 *	Get the current interrupt state and disable interrupts.
 */
static inline unsigned int ldsr_local_irq_save(void)
{
	int ret;
	thread_t self = thread_get_self();
	unsigned int mask = (1 << self);

	/*
	 * Ensure the compiler can not optimize out the code
	 * (volatile) and that it does not "cache" values around
	 * the interrupt state change (memory).  This ensures
	 * that interrupt changes are treated as a critical
	 * section.
	 */
	asm volatile (
	"	and.4	%0, scratchpad1, %1		\n\t"
	"	or.4	scratchpad1, scratchpad1, %1	\n\t"
		: "=&r" (ret)
		: "d" (mask)
		: "cc", "memory"
	);

	/*
	 *  We return a simple 1 == disabled, 0 == enabled
	 *  losing which tid this is for, because Linux
	 *  can restore interrupts on a different thread.
	 */
	return ret >> self;
}

/*
 * ldsr_local_irq_restore()
 * 	Restore this cpu's interrupt enable/disable state.
 *
 * Note: flags is either 0 or 1.
 */
static inline void ldsr_local_irq_restore(unsigned int flags)
{
	unsigned int temp;
	thread_t self = thread_get_self();
	unsigned int mask = (1 << self);
	flags = (flags << self);

	/*
	 * Ensure the compiler can not optimize out the code
	 * (volatile) and that it does not "cache" values around
	 * the interrupt state change (memory).  This ensures
	 * that interrupt changes are treated as a critical
	 * section.
	 *
	 * Atomic change to our bit in scratchpad1 without
	 * causing any temporary glitch in the value and
	 * without effecting other values.  Also this uses
	 * no branches so no penalties.
	 */
	asm volatile (
	"	xor.4	%0, scratchpad1, %1		\n\t"
	"	and.4	%0, %2, %0			\n\t"
	"	xor.4	scratchpad1, scratchpad1, %0	\n\t"
	"	move.4	int_set0, %3			\n\t"
		: "=&d"(temp)
		: "d"(flags), "r"(mask), "r"(ldsr_soft_irq_mask)
		: "cc", "memory"
	);
}

/*
 * ldsr_local_irq_disable_interrupt()
 * 	Disable ints for this thread.
 */
static inline void ldsr_local_irq_disable(void)
{
	unsigned int mask = (1 << thread_get_self());

	/*
	 * Ensure the compiler can not optimize out the code
	 * (volatile) and that it does not "cache" values around
	 * the interrupt state change (memory).  This ensures
	 * that interrupt changes are treated as a critical
	 * section.
	 */
	asm  volatile (
	"	or.4	scratchpad1, scratchpad1, %0	\n\t"
		:
		: "d" (mask)
		: "cc", "memory"
	);
}

/*
 * ldsr_local_irq_enable_interrupt
 * 	Enable ints for this thread.
 */
static inline void ldsr_local_irq_enable(void)
{
	unsigned int mask = (1 << thread_get_self());

	/*
	 * Ensure the compiler can not optimize out the code
	 * (volatile) and that it does not "cache" values around
	 * the interrupt state change (memory).  This ensures
	 * that interrupt changes are treated as a critical
	 * section.
	 */
	asm volatile (
	"	and.4	scratchpad1, scratchpad1, %0	\n\t"
	"	move.4	int_set0, %1			\n\t"
		:
		: "d" (~mask), "r" (ldsr_soft_irq_mask)
		: "cc", "memory"
	);
}

extern void ldsr_init(void);
extern void ldsr_set_trap_irq(unsigned int irq);
extern void ldsr_mask_vector(unsigned int vector);
extern void ldsr_unmask_vector(unsigned int vector);
extern void ldsr_enable_vector(unsigned int vector);
extern void ldsr_disable_vector(unsigned int vector);
extern thread_t ldsr_get_threadid(void);

#endif /* _ASM_UBICOM32_LDSR_H */

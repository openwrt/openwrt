/*
 * arch/ubicom32/include/asm/system.h
 *   Low level switching definitions.
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
#ifndef _ASM_UBICOM32_SYSTEM_H
#define _ASM_UBICOM32_SYSTEM_H

#include <linux/irqflags.h>
#include <linux/linkage.h>
#include <asm/segment.h>
#include <asm/entry.h>
#include <asm/ldsr.h>
#include <asm/irq.h>
#include <asm/percpu.h>
#include <asm/ubicom32-common.h>
#include <asm/processor.h>

/*
 * switch_to(n) should switch tasks to task ptr, first checking that
 * ptr isn't the current task, in which case it does nothing.
 */
asmlinkage void resume(void);
extern void *__switch_to(struct task_struct *prev,
		struct thread_struct *prev_switch,
		struct thread_struct *next_switch);

/*
 * We will need a per linux thread sw_ksp for the switch_to macro to
 * track the kernel stack pointer for the current thread on that linux thread.
 */
#define switch_to(prev,next,last)					\
({									\
	void *_last;							\
	_last = (void *)						\
		__switch_to(prev, &prev->thread, &next->thread);	\
	(last) = _last;							\
})

/*
 * Force strict CPU ordering.
 * Not really required on ubicom32...
 */
#define nop()  asm volatile ("nop"::)
#define mb()   asm volatile (""   : : :"memory")
#define rmb()  asm volatile (""   : : :"memory")
#define wmb()  asm volatile (""   : : :"memory")
#define set_mb(var, value)	({ (var) = (value); wmb(); })

#ifdef CONFIG_SMP
#define smp_mb()	mb()
#define smp_rmb()	rmb()
#define smp_wmb()	wmb()
#define smp_read_barrier_depends()	read_barrier_depends()
#else
#define smp_mb()	mb()
#define smp_rmb()	rmb()
#define smp_wmb()	wmb()
#define smp_read_barrier_depends()	do { } while(0)
#endif

#define read_barrier_depends()  ((void)0)

/*
 * The following defines change how the scheduler calls the switch_to()
 * macro.
 *
 * 1) The first causes the runqueue to be unlocked on entry to
 * switch_to().  Since our ctx code does not play with the runqueue
 * we do not need it unlocked.
 *
 * 2) The later turns interrupts on during a ctxsw to reduce the latency of
 * interrupts during ctx.  At this point in the port, we believe that this
 * latency is not a problem since we have very little code to perform a ctxsw.
 */
// #define __ARCH_WANT_UNLOCKED_CTXSW
// #define __ARCH_WANT_INTERRUPTS_ON_CTXSW

#endif /* _ASM_UBICOM32_SYSTEM_H */

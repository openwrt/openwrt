/*
 * arch/ubicom32/kernel/thread.c
 *   Ubicom32 architecture hardware thread support.
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

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/profile.h>
#include <linux/clocksource.h>
#include <linux/types.h>
#include <asm/ip5000.h>
#include <asm/machdep.h>
#include <asm/asm-offsets.h>
#include <asm/thread.h>

/*
 * TODO: At some point change the name here to be thread_ksp
 */
unsigned int sw_ksp[THREAD_ARCHITECTURAL_MAX];

static unsigned int thread_mask = -1;
static unsigned int thread_mainline_mask;

/*
 * thread_entry()
 *	Returning from the called function will disable the thread.
 *
 * This could be a naked call to allow for hwthreads that do not have stacks.
 * However, with -O0, the code still writes to thex stack, and this was
 * corrupting memory just after the callers stack.
 */
static void thread_entry(void *arg, thread_exec_fn_t exec)
{
	/*
	 * Call thread function
	 */
	exec(arg);

	/*
	 * Complete => Disable self
	 */
	thread_disable(thread_get_self());
}

/*
 * thread_start()
 *	Start the specified function on the specified hardware thread.
 */
thread_t thread_start(thread_t thread,
		      thread_exec_fn_t exec,
		      void *arg,
		      unsigned int *sp_high,
		      thread_type_t type)
{
	/*
	 * Sanity check
	 */
	unsigned int enabled, mask, csr;
	asm volatile (
		"move.4		%0, MT_EN\n\t"
		: "=m" (enabled)
	);

	mask = 1 << thread;
	if (enabled & mask) {
		printk(KERN_WARNING "request to enable a previously enabled thread\n");
		return (thread_t)-1;
	}

	/*
	 * Update thread state
	 */
	csr = (thread << 15) | (1 << 14);
	asm volatile (
		"setcsr		%0		\n\t"
		"setcsr_flush	0		\n\t"

		"move.4		A0, #0		\n\t"
		"move.4		A1, #0		\n\t"
		"move.4		A2, #0		\n\t"
		"move.4		A3, #0		\n\t"
		"move.4		A4, #0		\n\t"
		"move.4		A5, #0		\n\t"
		"move.4		A6, #0		\n\t"
		"move.4		SP, %4		\n\t"	/* A7 is SP */

		"move.4		D0, %3		\n\t"
		"move.4		D1, %2		\n\t"
		"move.4		D2, #0		\n\t"
		"move.4		D3, #0		\n\t"
		"move.4		D4, #0		\n\t"
		"move.4		D5, #0		\n\t"
		"move.4		D6, #0		\n\t"
		"move.4		D7, #0		\n\t"
		"move.4		D8, #0		\n\t"
		"move.4		D9, #0		\n\t"
		"move.4		D10, #0		\n\t"
		"move.4		D11, #0		\n\t"
		"move.4		D12, #0		\n\t"
		"move.4		D13, #0		\n\t"
		"move.4		D14, #0		\n\t"
		"move.4		D15, #0		\n\t"

		"move.4		INT_MASK0, #0	\n\t"
		"move.4		INT_MASK1, #0	\n\t"
		"move.4		PC, %1		\n\t"
		"setcsr		#0		\n\t"
		"setcsr_flush	0		\n\t"
		:
		: "r" (csr), "r" (thread_entry), "r" (exec),
		  "r" (arg), "r" (sp_high)
	);

	/*
	 * Apply HRT state
	 */
	if (type & THREAD_TYPE_HRT) {
		asm volatile (
			"or.4		MT_HRT, MT_HRT, %0\n\t"
			:
			: "d" (mask)
			: "cc"
		);
	} else {
		asm volatile (
			"and.4		MT_HRT, MT_HRT, %0\n\t"
			:
			: "d" (~mask)
			: "cc"
		);
	}

	/*
	 * Set priority
	 */
	asm volatile (
		"or.4		MT_HPRI, MT_HPRI, %0\n\t"
		:
		: "d" (mask)
		: "cc"
	);

	/*
	 * Enable thread
	 */
	asm volatile (
		"move.4		MT_ACTIVE_SET, %0	\n\t"
		:
		: "d" (mask)
	);
	thread_enable_mask(mask);
	return thread;
}

/*
 * thread_get_mainline()
 *	Return a mask of those threads that are Linux mainline threads.
 */
unsigned int thread_get_mainline(void)
{
	return thread_mainline_mask;
}

/*
 * thread_set_mainline()
 *	Indicate that the specified thread is a Linux mainline thread.
 */
void thread_set_mainline(thread_t tid)
{
	thread_mainline_mask |= (1 << tid);
}

/*
 * thread_alloc()
 *	Allocate an unused hardware thread.
 */
thread_t thread_alloc(void)
{
	thread_t tid;

	/*
	 * If this is the first time we are here get the list of unused
	 * threads from the processor device tree node.
	 */
	if (thread_mask == -1) {
		thread_mask = processor_threads();
	}

	if (!thread_mask) {
		return (thread_t)-1;
	}

	tid = ffs(thread_mask);
	if (tid != 0) {
		tid--;
		thread_mask &= ~(1 << tid);
		return tid;
	}

	return (thread_t)-1;
}

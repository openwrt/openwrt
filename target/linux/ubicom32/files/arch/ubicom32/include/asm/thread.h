/*
 * arch/ubicom32/include/asm/thread.h
 *   Ubicom32 architecture specific thread definitions.
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
#ifndef _ASM_UBICOM32_THREAD_H
#define _ASM_UBICOM32_THREAD_H

#if !defined(__ASSEMBLY__)

#include <asm/ptrace.h>
#include <asm/ubicom32-common.h>

typedef int thread_t;
typedef unsigned char thread_type_t;
typedef void (*thread_exec_fn_t)(void *arg);

#define THREAD_NULL 0x40
#define THREAD_TYPE_HRT (1 << 0)
#define THREAD_TYPE_SPECIAL 0
#define THREAD_TYPE_NORMAL 0
#define THREAD_TYPE_BACKGROUND (1 << 1)

/*
 * This is the upper bound on the maximum hardware threads that one will find
 * on a Ubicom processor. It is used to size per hardware thread data structures.
 */
#define THREAD_ARCHITECTURAL_MAX 16

/*
 * TODO: Rename this at some point to be thread_
 */
extern unsigned int sw_ksp[THREAD_ARCHITECTURAL_MAX];


/*
 * thread_get_self()
 */
static inline thread_t thread_get_self(void)
{
	thread_t result;

	/*
	 * Note that ROSR has zeroes in bits 6 through 31 and so we don't need
	 * to do any additional bit masking here.
	 */
	asm (
		"lsr.4	%0, ROSR, #2	\n\t"
		: "=d" (result)
		:
		: "cc"
	);

	return result;
}

/*
 * thread_suspend()
 */
static inline void thread_suspend(void)
{
	asm volatile (
		"suspend\n\t"
		:
		:
	);
}

/*
 * thread_resume()
 */
static inline void thread_resume(thread_t thread)
{
	asm volatile (
		"move.4		MT_ACTIVE_SET, %0	\n\t"
		"pipe_flush	0			\n\t"
		"pipe_flush	0			\n\t"
		:
		: "d" (1 << thread)
	);
}



/*
 * thread_enable_mask()
 *	Enable all threads in the mask.
 *
 * All writes to MT_EN must be protected by the MT_EN_LOCK bit
 */
static inline void thread_enable_mask(unsigned int mask)
{
	/*
	 * must flush the pipeline twice.
	 * first pipe_flush is to ensure write to MT_EN is completed
	 * second one is to ensure any new instructions from
	 * the targeted thread (the one being disabled), that
	 * are issued while the write to MT_EN is being executed,
	 * are completed.
	 */
	UBICOM32_LOCK(MT_EN_LOCK_BIT);
	asm volatile (
		"or.4		MT_EN, MT_EN, %0	\n\t"
		"pipe_flush	0			\n\t"
		"pipe_flush	0			\n\t"
		:
		: "d" (mask)
		: "cc"
	);
	UBICOM32_UNLOCK(MT_EN_LOCK_BIT);
}

/*
 * thread_enable()
 */
static inline void thread_enable(thread_t thread)
{
	thread_enable_mask(1 << thread);
}

/*
 * thread_disable_mask()
 *	Disable all threads in the mask.
 *
 * All writes to MT_EN must be protected by the MT_EN_LOCK bit
 */
static inline void thread_disable_mask(unsigned int mask)
{
	/*
	 * must flush the pipeline twice.
	 * first pipe_flush is to ensure write to MT_EN is completed
	 * second one is to ensure any new instructions from
	 * the targeted thread (the one being disabled), that
	 * are issued while the write to MT_EN is being executed,
	 * are completed.
	 */
	UBICOM32_LOCK(MT_EN_LOCK_BIT);
	asm volatile (
		"and.4		MT_EN, MT_EN, %0	\n\t"
		"pipe_flush	0			\n\t"
		"pipe_flush	0			\n\t"
		:
		: "d" (~mask)
		: "cc"
	);
	UBICOM32_UNLOCK(MT_EN_LOCK_BIT);
}

/*
 * thread_disable()
 */
static inline void thread_disable(thread_t thread)
{
	thread_disable_mask(1 << thread);
}

/*
 * thread_disable_others()
 *	Disable all other threads
 */
static inline void thread_disable_others(void)
{
	thread_t self = thread_get_self();
	thread_disable_mask(~(1 << self));
}

/*
 * thread_is_trapped()
 *	Is the specified tid trapped?
 */
static inline int thread_is_trapped(thread_t tid)
{
	int thread_mask = (1 << tid);
	int trap_thread;

	asm (
		"move.4		%0, MT_TRAP		\n\t"
		: "=d" (trap_thread)
		:
	);
	return (trap_thread & thread_mask);
}

/*
 * thread_is_enabled()
 *	Is the specified tid enabled?
 */
static inline int thread_is_enabled(thread_t tid)
{
	int thread_mask = (1 << tid);
	int enabled_threads;

	asm (
		"move.4		%0, MT_EN		\n\t"
		: "=d" (enabled_threads)
		:
	);
	return (enabled_threads & thread_mask);
}

/*
 * thread_get_instruction_count()
 */
static inline unsigned int thread_get_instruction_count(void)
{
	unsigned int result;
	asm (
		"move.4		%0, INST_CNT		\n\t"
		: "=r" (result)
	);
	return result;
}

/*
 * thread_get_pc()
 *	pc could point to a speculative and cancelled instruction unless thread is disabled
 */
static inline void *thread_get_pc(thread_t thread)
{
	void *result;
	asm (
		"move.4		csr, %1		\n\t"
		"setcsr_flush	0		\n\t"
		"move.4		%0, pc		\n\t"
		"move.4		csr, #0		\n\t"
		"setcsr_flush	0		\n\t"
		: "=r" (result)
		: "r" ((thread << 9) | (1 << 8))
	);
	return result;
}

/*
 * thread_get_trap_cause()
 *	This should be called only when the thread is not running
 */
static inline unsigned int thread_get_trap_cause(thread_t thread)
{
	unsigned int result;
	asm (
		"move.4		csr, %1		\n\t"
		"setcsr_flush	0		\n\t"
		"move.4		%0, trap_cause	\n\t"
		"move.4		csr, #0		\n\t"
		"setcsr_flush	0		\n\t"
		: "=r" (result)
		: "r" ((thread << 9) | (1 << 8))
	);
	return result;
}

/*
 * THREAD_STALL macro.
 */
#define THREAD_STALL \
		asm volatile ( \
			"move.4	mt_dbg_active_clr, #-1	\n\t" \
			"pipe_flush 0			\n\t" \
			: \
			: \
		)

extern unsigned int thread_get_mainline(void);
extern void thread_set_mainline(thread_t tid);
extern thread_t thread_alloc(void);
extern thread_t thread_start(thread_t thread, thread_exec_fn_t exec, void *arg, unsigned int *sp_high, thread_type_t type);

/*
 * asm macros
 */
asm (
/*
 * thread_get_self
 *	Read and shift the current thread into reg
 *
 * Note that we don't need to mask the result as bits 6 through 31 of the
 * ROSR are zeroes.
 */
".macro	thread_get_self reg		\n\t"
"	lsr.4	\\reg, ROSR, #2		\n\t"
".endm					\n\t"

/*
 * thread_get_self_mask
 *	Read and shift the current thread mask into reg
 */
".macro	thread_get_self_mask reg	\n\t"
"	lsr.4	\\reg, ROSR, #2		\n\t"
"	lsl.4	\\reg, #1, \\reg	\n\t"	 /* Thread bit */
".endm					\n\t"
);

#else /* __ASSEMBLY__ */

#include <asm/thread-asm.h>

#endif /* __ASSEMBLY__ */
#endif /* _ASM_UBICOM32_THREAD_H */

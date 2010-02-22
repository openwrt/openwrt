/*
 * arch/ubicom32/include/asm/processor.h
 *   Thread related definitions for Ubicom32 architecture.
 *
 * (C) Copyright 2009, Ubicom, Inc.
 * Copyright (C) 1995 Hamish Macdonald
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

#ifndef _ASM_UBICOM32_PROCESSOR_H
#define _ASM_UBICOM32_PROCESSOR_H

/*
 * Default implementation of macro that returns current
 * instruction pointer ("program counter").
 */
#define current_text_addr() ({ __label__ _l; _l: &&_l;})

#include <linux/compiler.h>
#include <linux/threads.h>
#include <asm/types.h>
#include <asm/segment.h>
#include <asm/fpu.h>
#include <asm/ptrace.h>
#include <asm/current.h>
#include <asm/thread_info.h>

#if defined(CONFIG_UBICOM32_V3)
	#define	CPU "IP5K"
#endif
#if defined(CONFIG_UBICOM32_V4)
	#define	CPU "IP7K"
#endif
#ifndef CPU
	#define	CPU "UNKNOWN"
#endif

/*
 * User space process size: 1st byte beyond user address space.
 */
extern unsigned long memory_end;
#define TASK_SIZE	(memory_end)

/*
 * This decides where the kernel will search for a free chunk of vm
 * space during mmap's. We won't be using it
 */
#define TASK_UNMAPPED_BASE	0

/*
 * This is the structure where we are going to save callee-saved registers.
 * A5 is the return address, A7 is the stack pointer, A6 is the frame
 * pointer.  This is the frame that is created because of switch_to. This
 * is not the frame due to interrupt preemption or because of syscall entry.
 */

struct thread_struct {
	unsigned long  d10;		/* D10  */
	unsigned long  d11;		/* D11  */
	unsigned long  d12;		/* D12  */
	unsigned long  d13;		/* D13  */
	unsigned long  a1;		/* A1  */
	unsigned long  a2;		/* A2  */
	unsigned long  a5;		/* A5 return address. */
	unsigned long  a6;		/* A6 */
	unsigned long  sp;		/* A7 kernel stack pointer. */
};

#define INIT_THREAD  { \
	0, 0, 0, 0, 0, 0, 0, 0, \
	sizeof(init_stack) + (unsigned long) init_stack - 8, \
}

/*
 * Do necessary setup to start up a newly executed thread.
 *
 * pass the data segment into user programs if it exists,
 * it can't hurt anything as far as I can tell
 */
/*
 * Do necessary setup to start up a newly executed thread.
 */
#define start_thread(regs, new_pc, new_sp)	 \
	do {					 \
		regs->pc = new_pc & ~3;		 \
		regs->an[5] = new_pc & ~3;	 \
		regs->an[7] = new_sp;		 \
		regs->nesting_level = -1;	 \
		regs->frame_type = UBICOM32_FRAME_TYPE_NEW_THREAD; \
		regs->thread_type = NORMAL_THREAD; \
	} while(0)

/* Forward declaration, a strange C thing */
struct task_struct;

/* Free all resources held by a thread. */
static inline void release_thread(struct task_struct *dead_task)
{
}

/* Prepare to copy thread state - unlazy all lazy status */
#define prepare_to_copy(tsk)	do { } while (0)

extern int kernel_thread(int (*fn)(void *), void * arg, unsigned long flags);

/*
 * Free current thread data structures etc..
 */
static inline void exit_thread(void)
{
}

unsigned long thread_saved_pc(struct task_struct *tsk);
unsigned long get_wchan(struct task_struct *p);

#define	KSTK_EIP(tsk)	(tsk->thread.a5)
#define	KSTK_ESP(tsk)	(tsk->thread.sp)

#define cpu_relax()    barrier()

extern void processor_init(void);
extern unsigned int processor_timers(void);
extern unsigned int processor_threads(void);
extern unsigned int processor_frequency(void);
extern int processor_interrupts(unsigned int *int0, unsigned int *int1);
extern void processor_ocm(unsigned long *socm, unsigned long *eocm);
extern void processor_dram(unsigned long *sdram, unsigned long *edram);

#define THREAD_SIZE_LONGS      (THREAD_SIZE/sizeof(unsigned long))
#define KSTK_TOP(info)                                                 \
({                                                                     \
       unsigned long *__ptr = (unsigned long *)(info);                 \
       (unsigned long)(&__ptr[THREAD_SIZE_LONGS]);                     \
})

#define task_pt_regs(task)                                             \
({                                                                     \
       struct pt_regs *__regs__;                                       \
       __regs__ = (struct pt_regs *)(KSTK_TOP(task_stack_page(task))-8); \
       __regs__ - 1;                                                   \
})

#endif	/* _ASM_UBICOM32_PROCESSOR_H */

/*
 * arch/ubicom32/kernel/stacktrace.c
 *   Ubicom32 architecture stack back trace implementation.
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
#include <linux/sched.h>
#include <linux/stacktrace.h>
#include <linux/module.h>
#include <asm/stacktrace.h>
#include <asm/thread.h>
#include <asm/ip5000.h>

/*
 * These symbols are filled in by the linker.
 */
extern unsigned long _stext;
extern unsigned long _etext;

extern unsigned long __ocm_text_run_begin;
extern unsigned long __data_begin;

/*
 * stacktrace_iterate()
 *	Walk the stack looking for call and calli instructions on an aligned
 *	boundary.
 *
 * Trace must point to the top of the current stack frame.
 */
unsigned long stacktrace_iterate(unsigned long **trace,
				 unsigned long stext,
				 unsigned long etext,
				 unsigned long ocm_stext,
				 unsigned long ocm_etext,
				 unsigned long sstack,
				 unsigned long estack)
{
	unsigned int thread_trap_en, instruction;
	unsigned long address;
	unsigned int limit = 0;
	unsigned long result = 0;
	unsigned long *sp = *trace;

	/*
	 * Exclude the current thread from being monitored for traps.
	 */
	asm volatile(
		"	thread_get_self_mask d15		\n\t"
			/* save current trap status */
		"	and.4	%0, MT_TRAP_EN, d15		\n\t"
		"	not.4	d15, d15			\n\t"
			/* disable trap */
		"	and.4	MT_TRAP_EN, MT_TRAP_EN, d15	\n\t"
		"	pipe_flush 0				\n\t"
		: "=r" (thread_trap_en)
		:
		: "d15", "cc"
	);

	while (limit++ < 256) {
		/*
		 * See if we have a valid stack.
		 */
		if (!between((unsigned long)sp, sstack, estack)) {
#ifdef TRAP_DEBUG_STACK_TRACE
			printk(KERN_EMERG "stack address is out of range - "
			       "sp: %x, sstack: %x, estack: %x\n",
			       (unsigned int)sp, (unsigned int)sstack,
			       (unsigned int)estack);
#endif
			result = 0;
			*trace = 0;
			break;
		}

		/*
		 * Get the value off the stack and back up 4 bytes to what
		 * should be the address of a call or calli.
		 */
		address = (*sp++) - 4;

		/*
		 * If the address is not within the text segment, skip this
		 * value.
		 */
		if (!between(address, stext, etext) &&
		    !between(address, ocm_stext, ocm_etext)) {
#ifdef TRAP_DEBUG_STACK_TRACE
			printk(KERN_EMERG "not a text address - "
			       "address: %08x, stext: %08x, etext: %08x\n"
			       "ocm_stext: %08x, ocm_etext: %08x\n",
			       (unsigned int)address,
			       (unsigned int)stext,
			       (unsigned int)etext,
			       (unsigned int)ocm_stext,
			       (unsigned int)ocm_etext);
#endif
			continue;

		}

		/*
		 * If the address is not on an aligned boundary it can not be a
		 * return address.
		 */
		if (address & 0x3) {
			continue;
		}

		/*
		 * Read the probable instruction.
		 */
		instruction = *(unsigned int *)address;

		/*
		 * Is this a call instruction?
		 */
		if ((instruction & 0xF8000000) == (u32_t)(0x1B << 27)) {
#ifdef TRAP_DEBUG_STACK_TRACE
			printk(KERN_EMERG "call inst. result: %x, "
			       "test: %x\n", (unsigned int)address,
			       (unsigned int)instruction);
#endif
			*trace = sp;
			result = address;
			break;
		}

		/*
		 * Is this a calli instruction?
		 */
		if ((instruction & 0xF8000000) == (u32_t)(0x1E << 27)) {
#ifdef TRAP_DEBUG_STACK_TRACE
			printk(KERN_EMERG "calli inst. result: %x, "
			       "test: %x\n", (unsigned int)address,
			       (unsigned int)instruction);
#endif
			*trace = sp;
			result = address;
			break;
		}
	}

	/*
	 * Restore the current thread to be monitored for traps.
	 */
	if (thread_trap_en) {
		asm volatile(
		"	thread_get_self_mask d15		\n\t"
		"	or.4	MT_TRAP_EN, MT_TRAP_EN, d15	\n\t"
			:
			:
			: "d15", "cc"
		);
	}
	return result;
}

#ifdef CONFIG_STACKTRACE
/*
 * stacktrace_save_entries()
 *	Save stack back trace information into the provided trace structure.
 */
void stacktrace_save_entries(struct task_struct *tsk,
			     struct stack_trace *trace,
			     unsigned long sp)
{
	unsigned long code_start = (unsigned long)&_stext;
	unsigned long code_end = (unsigned long)&_etext;
	unsigned long ocm_code_start = (unsigned long)&__ocm_text_run_begin;
	unsigned long ocm_code_end = (unsigned long)&__data_begin;
	unsigned long stack_end = (unsigned long)(tsk->stack + THREAD_SIZE - 8);
	unsigned long stack = (unsigned long)sp;
	unsigned int idx = 0;
	unsigned long *handle;
	int skip = trace->skip;

	handle = (unsigned long *)stack;
	while (idx < trace->max_entries) {
		if (skip) {
			skip--;
			continue;
		}
		trace->entries[idx] = stacktrace_iterate(&handle,
					code_start, code_end,
					ocm_code_start, ocm_code_end,
					(unsigned long)stack, stack_end);
		if (trace->entries[idx] == 0) {
			break;
		}
		idx++;
	}
}

/*
 * save_stack_trace()
 *	Save the specified amount of the kernel stack trace information
 *	for the current task.
 */
void save_stack_trace(struct stack_trace *trace)
{
	unsigned long sp = 0;
	asm volatile (
	"	move.4	%0, SP		\n\t"
		: "=r" (sp)
	);
	stacktrace_save_entries(current, trace, sp);
}
EXPORT_SYMBOL_GPL(save_stack_trace);

/*
 * save_stack_trace_tsk()
 *	Save the specified amount of the kernel stack trace information
 *	for the specified task.
 *
 * Note: We assume the specified task is not currently running.
 */
void save_stack_trace_tsk(struct task_struct *tsk, struct stack_trace *trace)
{
	stacktrace_save_entries(tsk, trace, tsk->thread.sp);
}
EXPORT_SYMBOL_GPL(save_stack_trace_tsk);
#endif /* CONFIG_STACKTRACE */

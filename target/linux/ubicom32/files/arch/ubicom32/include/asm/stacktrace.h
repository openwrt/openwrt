/*
 * arch/ubicom32/include/asm/stacktrace.h
 *   Stacktrace functions for the Ubicom32 architecture.
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
#ifndef _ASM_UBICOM32_STACKTRACE_H
#define _ASM_UBICOM32_STACKTRACE_H

#define between(a, b, c)	(( \
			((unsigned long) a) >= ((unsigned long) b)) && \
			(((unsigned long)a) <= ((unsigned long)c)))

/*
 * These symbols are filled in by the linker.
 */
extern unsigned long _stext;
extern unsigned long _etext;

/* OCM text goes from __ocm_text_run_begin to __data_begin */
extern unsigned long __ocm_text_run_begin;
extern unsigned long __data_begin;

/* Account for OCM case - see stacktrace.c maybe combine(also trap.c) */
/*
 * ubicom32_is_kernel()
 *
 * 	Check to see if the given address belongs to the kernel.
 * NOMMU does not permit any other means.
 */
static inline int ubicom32_is_kernel(unsigned long addr)
{
	int is_kernel = between(addr, &_stext, &_etext) || \
			between(addr, &__ocm_text_run_begin, &__data_begin);

#ifdef CONFIG_MODULES
	if (!is_kernel)
		is_kernel = is_module_address(addr);
#endif
	return is_kernel;
}

extern unsigned long stacktrace_iterate(
				unsigned long **trace,
				unsigned long stext, unsigned long etext,
				unsigned long ocm_stext, unsigned long ocm_etext,
				unsigned long sstack, unsigned long estack);
#ifdef CONFIG_STACKTRACE
void stacktrace_save_entries(struct task_struct *tsk, struct stack_trace *trace, unsigned long sp);
#endif
#endif /* _ASM_UBICOM32_STACKTRACE_H */

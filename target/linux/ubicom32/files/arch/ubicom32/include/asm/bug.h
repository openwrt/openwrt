/*
 * arch/ubicom32/include/asm/bug.h
 *   Generic bug.h for Ubicom32 architecture.
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
#ifndef _ASM_UBICOM32_BUG_H
#define _ASM_UBICOM32_BUG_H

#include <linux/kernel.h>
#include <asm/thread.h>

#if defined(CONFIG_BUG) && defined(CONFIG_STOP_ON_BUG)

/*
 * BUG()
 *	Ubicom specific version of the BUG() macro.
 *
 * This implementation performs a THREAD_STALL stopping all threads before
 * calling panic.  This enables a developer to see the "real" state of the
 * machine (since panic alters the system state).  We do the printf first
 * because while it is slow, it does not alter system state (like
 * interrupts).
 *
 * TODO: Implement the trap sequence used by other architectures.
 */
#define BUG() do { \
	printk("BUG: failure at %s:%d/%s()!\n", __FILE__, __LINE__, __func__); \
	THREAD_STALL; \
	panic("BUG!"); \
} while (0)


/*
 * __WARN()
 *	WARN() using printk() for now.
 *
 * TODO: Implement the trap sequence used by other architectures.
 */
#define __WARN()							\
	do {								\
		printk("WARN: failure at %s:%d/%s()!\n", __FILE__, __LINE__, __func__); \
	} while(0)

/*
 * WARN_ON()
 *	Ubicom specific version of the WARN_ON macro.
 *
 * This implementation performs a printk for the WARN_ON() instead
 * of faulting into the kernel and using report_bug().
 *
 * TODO: Implement the trap sequence used by other architectures.
 */
#define WARN_ON(x) ({						\
	int __ret_warn_on = !!(x);				\
	if (__builtin_constant_p(__ret_warn_on)) {		\
		if (__ret_warn_on)				\
			__WARN();				\
	} else {						\
		if (unlikely(__ret_warn_on))			\
			__WARN();				\
	}							\
	unlikely(__ret_warn_on);				\
})


#define HAVE_ARCH_BUG
#define HAVE_ARCH_WARN_ON

#endif

#include <asm-generic/bug.h>

#endif /* _ASM_UBICOM32_BUG_H */

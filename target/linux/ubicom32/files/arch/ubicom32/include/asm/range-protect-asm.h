/*
 * arch/ubicom32/include/asm/range-protect-asm.h
 *   Assembly macros for enabling memory protection.
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

#ifndef _ASM_UBICOM32_RANGE_PROTECT_ASM_H
#define _ASM_UBICOM32_RANGE_PROTECT_ASM_H

#if defined(__ASSEMBLY__)

#include <asm/thread-asm.h>

/*
 * You should only use the enable/disable ranges when you have the atomic lock,
 * if you do not there will be problems.
 */

/*
 * enable_kernel_ranges
 *	Enable the kernel ranges (disabling protection) for thread,
 *	where thread == (1 << thread number)
 */
.macro	enable_kernel_ranges thread
#ifdef CONFIG_PROTECT_KERNEL
	or.4	I_RANGE0_EN, I_RANGE0_EN, \thread	 /* Enable Range Register */
	or.4	D_RANGE0_EN, D_RANGE0_EN, \thread
	or.4	D_RANGE1_EN, D_RANGE1_EN, \thread
#endif
.endm

/*
 * enable_kernel_ranges_for_current
 *	Enable the kernel ranges (disabling protection) for this thread
 */
.macro	enable_kernel_ranges_for_current scratch_reg
#ifdef CONFIG_PROTECT_KERNEL
	thread_get_self_mask \scratch_reg
	enable_kernel_ranges \scratch_reg
#endif
.endm

/*
 * disable_kernel_ranges
 *	Disables the kernel ranges (enabling protection) for thread
 *	where thread == (1 << thread number)
 */
.macro	disable_kernel_ranges thread
#ifdef CONFIG_PROTECT_KERNEL
	not.4	\thread, \thread
	and.4	I_RANGE0_EN, I_RANGE0_EN, \thread	 /* Disable Range Register */
	and.4	D_RANGE0_EN, D_RANGE0_EN, \thread
	and.4	D_RANGE1_EN, D_RANGE1_EN, \thread
#endif
.endm

/*
 * disable_kernel_ranges_for_current
 *	Disable kernel ranges (enabling protection) for this thread
 */
.macro	disable_kernel_ranges_for_current scratch_reg
#ifdef CONFIG_PROTECT_KERNEL
	thread_get_self_mask \scratch_reg
	disable_kernel_ranges \scratch_reg
#endif
.endm
#endif

#endif  /* _ASM_UBICOM32_RANGE_PROTECT_ASM_H */

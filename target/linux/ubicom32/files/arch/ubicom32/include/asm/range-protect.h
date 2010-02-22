/*
 * arch/ubicom32/include/asm/range-protect.h
 *   Assembly macros declared in C for enabling memory protection.
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

#ifndef _ASM_UBICOM32_RANGE_PROTECT_H
#define _ASM_UBICOM32_RANGE_PROTECT_H

#if !defined(__ASSEMBLY__)
#include <asm/thread.h>
/*
 * The following macros should be the identical to the ones in
 * range-protect-asm.h
 *
 * You should only use the enable/disable ranges when you have the atomic lock,
 * if you do not there will be problems.
 */

/*
 * enable_kernel_ranges
 *	Enable the kernel ranges (disabling protection) for thread,
 *	where thread == (1 << thread number)
 */
asm (
	".macro	enable_kernel_ranges thread			\n\t"
#ifdef CONFIG_PROTECT_KERNEL
	"	or.4	I_RANGE0_EN, I_RANGE0_EN, \\thread	\n\t" /* Enable Range Register */
	"	or.4	D_RANGE0_EN, D_RANGE0_EN, \\thread	\n\t"
	"	or.4	D_RANGE1_EN, D_RANGE1_EN, \\thread	\n\t"
#endif
	".endm							\n\t"
);

#else /* __ASSEMBLY__ */

#include <asm/range-protect-asm.h>

#endif
#endif  /* _ASM_UBICOM32_RANGE_PROTECT_H */

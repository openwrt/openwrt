/*
 * arch/ubicom32/include/asm/thread-asm.h
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
#ifndef _ASM_UBICOM32_THREAD_ASM_H
#define _ASM_UBICOM32_THREAD_ASM_H

/*
 * thread_get_self
 *	Read and shift the current thread into reg
 *
 * Note that we don't need to mask the result as bits 6 through 31 of the
 * ROSR are zeroes.
 */
.macro	thread_get_self reg
	lsr.4	\reg, ROSR, #2
.endm

/*
 * thread_get_self_mask
 *	Read and shift the current thread mask into reg
 */
.macro	thread_get_self_mask reg
	lsr.4	\reg, ROSR, #2
	lsl.4	\reg, #1, \reg	 /* Thread bit */
.endm

#endif /* _ASM_UBICOM32_THREAD_ASM_H */

/*
 * arch/ubicom32/include/asm/ubicom32-common-asm.h
 *   Ubicom32 atomic lock operations.
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

#ifndef _ASM_UBICOM32_UBICOM32_COMMON_ASM_H
#define _ASM_UBICOM32_UBICOM32_COMMON_ASM_H

/*
 * atomic_lock_acquire macro
 *	Equivalent to __atomic_lock_acquire()
 */
.macro atomic_lock_acquire
	bset scratchpad1, scratchpad1, #ATOMIC_LOCK_BIT
	jmpne.f	.-4
.endm

/*
 * atomic_lock_release macro
 *	Equivalent to __atomic_lock_release()
 */
.macro atomic_lock_release
	bclr scratchpad1, scratchpad1, #ATOMIC_LOCK_BIT
.endm

#endif /* _ASM_UBICOM32_UBICOM32_COMMON_ASM_H */

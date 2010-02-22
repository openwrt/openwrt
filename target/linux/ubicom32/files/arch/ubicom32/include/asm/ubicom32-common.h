/*
 * arch/ubicom32/include/asm/ubicom32-common.h
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

#ifndef _ASM_UBICOM32_UBICOM32_COMMON_H
#define _ASM_UBICOM32_UBICOM32_COMMON_H

#define S(arg) #arg
#define D(arg) S(arg)
/*
 * scratchpad1 is owned by the LDSR.
 *
 * The upper bits provide 16 global spinlocks.  Acquiring one of these
 * global spinlocks synchornizes across multiple threads and prevents
 * the LDSR from delivering any interrupts while the lock is held.
 * Use these locks only when absolutely required.
 *
 * The lower 16 bits of scratchpad1 are used as per thread interrupt
 * enable/disable bits.  These bits will prevent a thread from receiving
 * any interrupts.
 *
 * Bit Usage:
 * - MT_EN_LOCK_BIT   - Protects writes to MT_EN, so code can read current value
 *			then write a new value atomically (profiler for example)
 * - ATOMIC_LOCK_BIT - Used to provide general purpose atomic handling.
 * - LDSR_LOCK_BIT   - Used by the LDSR exclusively to provide protection.
 * - DCCR_LOCK_BIT   - Used to limit access to the DCCR cache control peripheral
 * - ICCR_LOCK_BIT   - Used to limit access to the ICCR cache control peripheral
 * - LSB 16 bits     - Used by the LDSR to represent thread enable/disable bits.
 */
#define MT_EN_LOCK_BIT	31
#define ATOMIC_LOCK_BIT 30
#define LDSR_LOCK_BIT   29
#define PCI_LOCK_BIT	28
#define ICCR_LOCK_BIT	27
#define DCCR_LOCK_BIT	26

#if !defined(__ASSEMBLY__)

#define UBICOM32_TRYLOCK(bit) \
	asm volatile (						      \
	"	move.4 %0, #0					\n\t" \
	"	bset scratchpad1, scratchpad1, #"D(bit)"	\n\t" \
	"	jmpne.f	1f					\n\t" \
	"	move.4 %0, #1					\n\t" \
	"1:							\n\t" \
		: "=r" (ret)					      \
		:						      \
		: "cc", "memory"				      \
	)							      \

#define UBICOM32_UNLOCK(bit) \
	asm volatile (						      \
	"	bclr scratchpad1, scratchpad1, #"D(bit)"	\n\t" \
		:						      \
		:						      \
		: "cc", "memory"				      \
	)							      \

#define UBICOM32_LOCK(bit) \
	asm volatile (						      \
	"1:	bset scratchpad1, scratchpad1, #"D(bit)"	\n\t" \
	"	jmpne.f	1b					\n\t" \
		:						      \
		:						      \
		: "cc", "memory"				      \
	)							      \

/*
 * __atomic_lock_trylock()
 *	Attempt to acquire the lock, return TRUE if acquired.
 */
static inline int __atomic_lock_trylock(void)
{
	int ret;
	UBICOM32_TRYLOCK(ATOMIC_LOCK_BIT);
	return ret;
}

/*
 * __atomic_lock_release()
 *	Release the global atomic lock.
 *
 * Note: no one is suspended waiting since this lock is a spinning lock.
 */
static inline void __atomic_lock_release(void)
{
	UBICOM32_UNLOCK(ATOMIC_LOCK_BIT);
}

/*
 * __atomic_lock_acquire()
 *	Acquire the global atomic lock, spin if not available.
 */
static inline void __atomic_lock_acquire(void)
{
	UBICOM32_LOCK(ATOMIC_LOCK_BIT);
}
#else /* __ASSEMBLY__ */

#include <asm/ubicom32-common-asm.h>

#endif /* __ASSEMBLY__ */
#endif /* _ASM_UBICOM32_UBICOM32_COMMON_H */

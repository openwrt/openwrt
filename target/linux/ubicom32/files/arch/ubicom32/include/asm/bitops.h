/*
 * arch/ubicom32/include/asm/bitops.h
 *   Bit manipulation definitions for Ubicom32 architecture.
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
#ifndef _ASM_UBICOM32_BITOPS_H
#define _ASM_UBICOM32_BITOPS_H

/*
 * Copyright 1992, Linus Torvalds.
 */

#include <linux/compiler.h>
#include <asm/byteorder.h>	/* swab32 */

#ifdef __KERNEL__

#ifndef _LINUX_BITOPS_H
#error only <linux/bitops.h> can be included directly
#endif

#include <asm-generic/bitops/ffs.h>
#include <asm-generic/bitops/__ffs.h>

#include <asm-generic/bitops/sched.h>
#include <asm-generic/bitops/ffz.h>

#include <asm/ubicom32-common.h>

static inline void set_bit(int bit, volatile unsigned long *p)
{
	unsigned long mask = 1UL << (bit & 31);

	p += bit >> 5;

	__atomic_lock_acquire();
	*p |= mask;
	__atomic_lock_release();
}

static inline void clear_bit(int bit, volatile unsigned long *p)
{
	unsigned long mask = 1UL << (bit & 31);

	p += bit >> 5;

	__atomic_lock_acquire();
	*p &= ~mask;
	__atomic_lock_release();
}

/*
 * clear_bit() doesn't provide any barrier for the compiler.
 */
#define smp_mb__before_clear_bit()	barrier()
#define smp_mb__after_clear_bit()	barrier()

static inline void change_bit(int bit, volatile unsigned long *p)
{
	unsigned long mask = 1UL << (bit & 31);

	p += bit >> 5;

	__atomic_lock_acquire();
	*p ^= mask;
	__atomic_lock_release();
}

static inline int test_and_set_bit(int bit, volatile unsigned long *p)
{
	unsigned int res;
	unsigned long mask = 1UL << (bit & 31);

	p += bit >> 5;

	__atomic_lock_acquire();
	res = *p;
	*p = res | mask;
	__atomic_lock_release();

	return res & mask;
}

static inline int test_and_clear_bit(int bit, volatile unsigned long *p)
{
	unsigned int res;
	unsigned long mask = 1UL << (bit & 31);

	p += bit >> 5;

	__atomic_lock_acquire();
	res = *p;
	*p = res & ~mask;
	__atomic_lock_release();

	return res & mask;
}

static inline int test_and_change_bit(int bit, volatile unsigned long *p)
{
	unsigned int res;
	unsigned long mask = 1UL << (bit & 31);

	p += bit >> 5;

	__atomic_lock_acquire();
	res = *p;
	*p = res ^ mask;
	__atomic_lock_release();

	return res & mask;
}

#include <asm-generic/bitops/non-atomic.h>

/*
 * This routine doesn't need to be atomic.
 */
static inline int __constant_test_bit(int nr, const volatile unsigned long *addr)
{
	return ((1UL << (nr & 31)) & (((const volatile unsigned int *) addr)[nr >> 5])) != 0;
}

static inline int __test_bit(int nr, const volatile unsigned long *addr)
{
	int 	* a = (int *) addr;
	int	mask;

	a += nr >> 5;
	mask = 1 << (nr & 0x1f);
	return ((mask & *a) != 0);
}

#define test_bit(nr,addr) (__builtin_constant_p(nr) ?  __constant_test_bit((nr),(addr)) :  __test_bit((nr),(addr)))

#include <asm-generic/bitops/find.h>
#include <asm-generic/bitops/hweight.h>
#include <asm-generic/bitops/lock.h>

#include <asm-generic/bitops/ext2-non-atomic.h>
#include <asm-generic/bitops/ext2-atomic.h>
#include <asm-generic/bitops/minix.h>

#endif /* __KERNEL__ */

#include <asm-generic/bitops/fls.h>
#include <asm-generic/bitops/__fls.h>
#include <asm-generic/bitops/fls64.h>

#endif /* _ASM_UBICOM32_BITOPS_H */

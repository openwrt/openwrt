/*
 * arch/ubicom32/include/asm/atomic.h
 *   Atomic operations definitions for Ubicom32 architecture.
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
#ifndef _ASM_UBICOM32_ATOMIC_H
#define _ASM_UBICOM32_ATOMIC_H

#include <asm/system.h>
#include <asm/ubicom32-common.h>
#include <asm/types.h>

/*
 * Most instructions on the Ubicom32 processor are atomic in that they
 * execute in one clock cycle.  However, Linux has several operations
 * (e.g. compare and swap) which will require more than a single instruction
 * to perform.   To achieve this, the Ubicom32 processor uses a single
 * global bit in a scratchpad register as a critical section lock. All
 * atomic operations acquire this lock.
 *
 * NOTE: To AVOID DEADLOCK(s), the atomic lock must only be used for atomic
 * operations or by the ldsr to avoid disabling a thread performing an atomic
 * operation.
 *
 * Do not attempt to disable interrupts while holding the atomic operations
 * lock or you will DEADLOCK the system.
 */

#define ATOMIC_INIT(i)	{ (i) }

/*
 * __atomic_add()
 * 	Add i to v and return the result.
 */
static inline void __atomic_add(int i, atomic_t *v)
{
	atomic_t *vt = v;

	__atomic_lock_acquire();
	vt->counter += i;
	__atomic_lock_release();
}

/*
 * __atomic_sub()
 * 	Subtract i from v and return the result.
 */
static inline void __atomic_sub(int i, atomic_t *v)
{
	atomic_t *vt = v;

	__atomic_lock_acquire();
	vt->counter -= i;
	__atomic_lock_release();
}

/*
 * __atomic_add_return()
 * 	Add i to v and return the result.
 *
 * The implementation here looks rather odd because we appear to be doing
 * the addition twice.  In fact that's exactly what we're doing but with
 * the ubicom32 instruction set we can do the inner load and add with two
 * instructions whereas generating both the atomic result and the "ret"
 * result requires three instructions.  The second add is generally only as
 * costly as a move instruction and in cases where we compare the result
 * with a constant the compiler can fold two constant values and do a
 * single instruction, thus saving an instruction overall!
 *
 * At the worst we save one instruction inside the atomic lock.
 */
static inline int __atomic_add_return(int i, atomic_t *v)
{
	int ret;
	atomic_t *vt = v;

	__atomic_lock_acquire();
	ret = vt->counter;
	vt->counter = ret + i;
	__atomic_lock_release();

	return ret + i;
}

/*
 * __atomic_sub_return()
 * 	Subtract i from v and return the result.
 *
 * The implementation here looks rather odd because we appear to be doing
 * the subtraction twice.  In fact that's exactly what we're doing but with
 * the ubicom32 instruction set we can do the inner load and sub with two
 * instructions whereas generating both the atomic result and the "ret"
 * result requires three instructions.  The second sub is generally only as
 * costly as a move instruction and in cases where we compare the result
 * with a constant the compiler can fold two constant values and do a
 * single instruction, thus saving an instruction overall!
 *
 * At the worst we save one instruction inside the atomic lock.
 */
static inline int __atomic_sub_return(int i, atomic_t *v)
{
	int ret;
	atomic_t *vt = v;

	__atomic_lock_acquire();
	ret = vt->counter;
	vt->counter = ret - i;
	__atomic_lock_release();

	return ret - i;
}

/*
 * PUBLIC API FOR ATOMIC!
 */
#define atomic_add(i,v)	(__atomic_add( ((int)i),(v)))
#define atomic_sub(i,v)	(__atomic_sub( ((int)i),(v)))
#define atomic_inc(v)	(__atomic_add(   1,(v)))
#define atomic_dec(v)	(__atomic_sub(   1,(v)))
#define atomic_add_return(i,v)	(__atomic_add_return( ((int)i),(v)))
#define atomic_sub_return(i,v)	(__atomic_sub_return( ((int)i),(v)))
#define atomic_inc_return(v)	(__atomic_add_return(   1,(v)))
#define atomic_dec_return(v)	(__atomic_sub_return(   1,(v)))
#define atomic_inc_and_test(v)	(atomic_inc_return(v) == 0)
#define atomic_dec_and_test(v)	(atomic_dec_return(v) == 0)
#define atomic_add_negative(a, v)	(atomic_add_return((a), (v)) < 0)
#define atomic_sub_and_test(i,v)	(atomic_sub_return((i),(v)) == 0)

/*
 * atomic_read()
 * 	Acquire the atomic lock and read the variable.
 */
static inline int atomic_read(const atomic_t *v)
{
	int ret;
	const atomic_t *vt = v;

	__atomic_lock_acquire();
	ret = vt->counter;
	__atomic_lock_release();

	return ret;
}

/*
 * atomic_set()
 * 	Acquire the atomic lock and set the variable.
 */
static inline void atomic_set(atomic_t *v, int i)
{
	atomic_t *vt = v;

	__atomic_lock_acquire();
	vt->counter = i;
	__atomic_lock_release();
}

/*
 * atomic_cmpxchg
 * 	Acquire the atomic lock and exchange if current == old.
 */
static inline int atomic_cmpxchg(atomic_t *v, int old, int new)
{
	int prev;
	atomic_t *vt = v;

	__atomic_lock_acquire();
	prev = vt->counter;
	if (prev == old) {
		vt->counter = new;
	}
	__atomic_lock_release();

	return prev;
}

/*
 * atomic_xchg()
 * 	Acquire the atomic lock and exchange values.
 */
static inline int atomic_xchg(atomic_t *v, int new)
{
	int prev;
	atomic_t *vt = v;

	__atomic_lock_acquire();
	prev = vt->counter;
	vt->counter = new;
	__atomic_lock_release();

	return prev;
}

/*
 * atomic_add_unless()
 * 	Acquire the atomic lock and add a unless the value is u.
 */
static inline int atomic_add_unless(atomic_t *v, int a, int u)
{
	int prev;
	atomic_t *vt = v;

	__atomic_lock_acquire();
	prev = vt->counter;
	if (prev != u) {
		vt->counter += a;
		__atomic_lock_release();
		return 1;
	}

	__atomic_lock_release();
	return 0;
}

#define atomic_inc_not_zero(v) atomic_add_unless((v), 1, 0)

#include <asm-generic/atomic.h>

/*
 * The following is not a real function.  The compiler should remove the function
 * call as long as the user does not pass in a size that __xchg and __cmpxchg
 * are not prepared for.  If the user does pass in an unknown size, the user
 * will get a link time error.
 *
 * The no return is to prevent a compiler error that can occur when dealing with
 * uninitialized variables. Given that the function doesn't exist there is no
 * net effect (and if it did it would not return).
 */
extern void __xchg_called_with_bad_pointer(void) __attribute__((noreturn));

/*
 * __xchg()
 *	Xchange *ptr for x atomically.
 *
 * Must be both locally atomic and atomic on SMP. Ubicom32 does not have an
 * atomic exchange instruction so we use the global atomic_lock.
 */
static inline unsigned long __xchg(unsigned long x, volatile void *ptr, int size)
{
	unsigned long ret;

	__atomic_lock_acquire();

	switch (size) {
	case 1:
		ret = *(volatile unsigned char *)ptr;
		*(volatile unsigned char *)ptr = x;
		break;

	case 2:
		ret = *(volatile unsigned short *)ptr;
		*(volatile unsigned short *)ptr = x;
		break;

	case 4:
		ret = *(volatile unsigned int *)ptr;
		*(volatile unsigned int *)ptr = x;
		break;

	default:
		__xchg_called_with_bad_pointer();
		break;
	}
	__atomic_lock_release();
	return ret;
}

#define xchg(ptr,x) ((__typeof__(*(ptr)))__xchg((unsigned long)(x),(ptr),sizeof(*(ptr))))

/*
 * __cmpxchg()
 *	Compare and Xchange *ptr for x atomically.
 *
 * Must be both locally atomic and atomic on SMP. Ubicom32 does not have an
 * atomic exchange instruction so we use the global atomic_lock.
 */
static inline unsigned long __cmpxchg(volatile void *ptr, unsigned long old, unsigned long next, int size)
{
	unsigned long prev;

	__atomic_lock_acquire();
	switch (size) {
	case 1:
		prev = *(u8 *)ptr;
		if (prev == old) {
			*(u8 *)ptr = (u8)next;
		}
		break;

	case 2:
		prev = *(u16 *)ptr;
		if (prev == old) {
			*(u16 *)ptr = (u16)next;
		}
		break;

	case 4:
		prev = *(u32 *)ptr;
		if (prev == old) {
			*(u32 *)ptr = (u32)next;
		}
		break;

	default:
		__xchg_called_with_bad_pointer();
		break;
	}
	__atomic_lock_release();
	return prev;
}

/*
 * cmpxchg_local and cmpxchg64_local are atomic wrt current CPU. Always make
 * them available.
 */
#define cmpxchg_local(ptr, o, n) \
	((__typeof__(*(ptr)))__cmpxchg((ptr), (unsigned long)(o), (unsigned long)(n), sizeof(*(ptr))))

#define cmpxchg(ptr, o, n) __cmpxchg((ptr), (o), (n), sizeof(*(ptr)))

#define smp_mb__before_atomic_inc() asm volatile ("" : : : "memory")
#define smp_mb__after_atomic_inc() asm volatile ("" : : : "memory")
#define smp_mb__before_atomic_dec() asm volatile ("" : : : "memory")
#define smp_mb__after_atomic_dec() asm volatile ("" : : : "memory")

#endif /* _ASM_UBICOM32_ATOMIC_H */

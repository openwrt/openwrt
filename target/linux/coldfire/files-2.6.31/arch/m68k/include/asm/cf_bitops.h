/* 
 * Copyright 2007-2009 Freescale Semiconductor, Inc. All Rights Reserved.
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file COPYING in the main directory of this archive
 * for more details.
 */
#ifndef __CF_BITOPS__
#define __CF_BITOPS__

#ifndef _LINUX_BITOPS_H
#error only <linux/bitops.h> can be included directly
#endif

#include <linux/compiler.h>

#define test_and_set_bit(nr,vaddr)			\
  (__builtin_constant_p(nr) ?				\
   __constant_coldfire_test_and_set_bit(nr, vaddr) :	\
   __generic_coldfire_test_and_set_bit(nr, vaddr))

static __inline__ int __constant_coldfire_test_and_set_bit(int nr,
	volatile void *vaddr)
{
	char retval;
	volatile char *p = &((volatile char *)vaddr)[(nr^31) >> 3];
	__asm__ __volatile__ ("bset %2,(%4); sne %0"
	     : "=d" (retval), "=m" (*p)
	     : "di" (nr & 7), "m" (*p), "a" (p));
	return retval;
}

static __inline__ int __generic_coldfire_test_and_set_bit(int nr,
	volatile void *vaddr)
{
	char retval;

	__asm__ __volatile__ ("bset %2,%1; sne %0"
	     : "=d" (retval), "=m" (((volatile char *)vaddr)[(nr^31) >> 3])
	     : "d" (nr)
	     : "memory");
	return retval;
}
#define __test_and_set_bit(nr, vaddr) test_and_set_bit(nr, vaddr)

#define set_bit(nr,vaddr)			\
  (__builtin_constant_p(nr) ?			\
   __constant_coldfire_set_bit(nr, vaddr) :	\
   __generic_coldfire_set_bit(nr, vaddr))

static __inline__ void __constant_coldfire_set_bit(int nr,
	volatile void *vaddr)
{
	volatile char *p = &((volatile char *)vaddr)[(nr^31) >> 3];
	__asm__ __volatile__ ("bset %1,(%3)"
	     : "=m" (*p) : "di" (nr & 7), "m" (*p), "a" (p));
}

static __inline__ void __generic_coldfire_set_bit(int nr,
	volatile void *vaddr)
{
	__asm__ __volatile__ ("bset %1,%0"
	     : "=m" (((volatile char *)vaddr)[(nr^31) >> 3])
	     : "d" (nr)
	     : "memory");
}
#define __set_bit(nr, vaddr) set_bit(nr, vaddr)

#define test_and_clear_bit(nr, vaddr)			\
  (__builtin_constant_p(nr) ?				\
   __constant_coldfire_test_and_clear_bit(nr, vaddr) :	\
   __generic_coldfire_test_and_clear_bit(nr, vaddr))

static __inline__ int __constant_coldfire_test_and_clear_bit(int nr,
	volatile void *vaddr)
{
	char retval;
	volatile char *p = &((volatile char *)vaddr)[(nr^31) >> 3];

	__asm__ __volatile__ ("bclr %2,(%4); sne %0"
	     : "=d" (retval), "=m" (*p)
	     : "id" (nr & 7), "m" (*p), "a" (p));

	return retval;
}

static __inline__ int __generic_coldfire_test_and_clear_bit(int nr,
	volatile void *vaddr)
{
	char retval;

	__asm__ __volatile__ ("bclr %2,%1; sne %0"
	     : "=d" (retval), "=m" (((volatile char *)vaddr)[(nr^31) >> 3])
	     : "d" (nr & 7)
	     : "memory");

	return retval;
}
#define __test_and_clear_bit(nr, vaddr) test_and_clear_bit(nr, vaddr)

/*
 * clear_bit() doesn't provide any barrier for the compiler.
 */
#define smp_mb__before_clear_bit()	barrier()
#define smp_mb__after_clear_bit()	barrier()

#define clear_bit(nr,vaddr)			\
  (__builtin_constant_p(nr) ?			\
   __constant_coldfire_clear_bit(nr, vaddr) :	\
   __generic_coldfire_clear_bit(nr, vaddr))

static __inline__ void __constant_coldfire_clear_bit(int nr,
	volatile void *vaddr)
{
	volatile char *p = &((volatile char *)vaddr)[(nr^31) >> 3];
	__asm__ __volatile__ ("bclr %1,(%3)"
	     : "=m" (*p) : "id" (nr & 7), "m" (*p), "a" (p));
}

static __inline__ void __generic_coldfire_clear_bit(int nr,
	volatile void *vaddr)
{
	__asm__ __volatile__ ("bclr %1,%0"
	     : "=m" (((volatile char *)vaddr)[(nr^31) >> 3])
	     : "d" (nr)
	     : "memory");
}
#define __clear_bit(nr, vaddr) clear_bit(nr, vaddr)

#define test_and_change_bit(nr, vaddr)			\
  (__builtin_constant_p(nr) ?				\
   __constant_coldfire_test_and_change_bit(nr, vaddr) :	\
   __generic_coldfire_test_and_change_bit(nr, vaddr))

static __inline__ int __constant_coldfire_test_and_change_bit(int nr,
	volatile void *vaddr)
{
	char retval;
	volatile char *p = &((volatile char *)vaddr)[(nr^31) >> 3];

	__asm__ __volatile__ ("bchg %2,(%4); sne %0"
	     : "=d" (retval), "=m" (*p)
	     : "id" (nr & 7), "m" (*p), "a" (p));

	return retval;
}

static __inline__ int __generic_coldfire_test_and_change_bit(int nr,
	volatile void *vaddr)
{
	char retval;

	__asm__ __volatile__ ("bchg %2,%1; sne %0"
	     : "=d" (retval), "=m" (((volatile char *)vaddr)[(nr^31) >> 3])
	     : "id" (nr)
	     : "memory");

	return retval;
}
#define __test_and_change_bit(nr, vaddr) test_and_change_bit(nr, vaddr)
#define __change_bit(nr, vaddr) change_bit(nr, vaddr)

#define change_bit(nr,vaddr)			\
  (__builtin_constant_p(nr) ?			\
   __constant_coldfire_change_bit(nr, vaddr) :	\
   __generic_coldfire_change_bit(nr, vaddr))

static __inline__ void __constant_coldfire_change_bit(int nr,
	volatile void *vaddr)
{
	volatile char *p = &((volatile char *)vaddr)[(nr^31) >> 3];
	__asm__ __volatile__ ("bchg %1,(%3)"
	     : "=m" (*p) : "id" (nr & 7), "m" (*p), "a" (p));
}

static __inline__ void __generic_coldfire_change_bit(int nr,
	volatile void *vaddr)
{
	__asm__ __volatile__ ("bchg %1,%0"
	     : "=m" (((volatile char *)vaddr)[(nr^31) >> 3])
	     : "d" (nr)
	     : "memory");
}

static inline int test_bit(int nr, const unsigned long *vaddr)
{
	return (vaddr[nr >> 5] & (1UL << (nr & 31))) != 0;
}

static __inline__ unsigned long ffz(unsigned long word)
{
	unsigned long result = 0;

	while (word & 1) {
		result++;
		word >>= 1;
	}
	return result;
}

/* find_next_zero_bit() finds the first zero bit in a bit string of length
 * 'size' bits, starting the search at bit 'offset'.  This is largely based
 * on Linus's ALPHA routines.
 */
static __inline__ unsigned long find_next_zero_bit(void *addr,
	unsigned long size, unsigned long offset)
{
	unsigned long *p = ((unsigned long *) addr) + (offset >> 5);
	unsigned long result = offset & ~31UL;
	unsigned long tmp;

	if (offset >= size)
		return size;
	size -= result;
	offset &= 31UL;
	if (offset) {
		tmp = *(p++);
		tmp |= ~0UL >> (32-offset);
		if (size < 32)
			goto found_first;
		if (~tmp)
			goto found_middle;
		size -= 32;
		result += 32;
	}
	while (size & ~31UL) {
		tmp = *(p++);
		if (~tmp)
			goto found_middle;
		result += 32;
		size -= 32;
	}
	if (!size)
		return result;
	tmp = *p;

found_first:
	tmp |= ~0UL >> size;
found_middle:
	return result + ffz(tmp);
}

#define find_first_zero_bit(addr, size) find_next_zero_bit(((void *)addr), \
		(size), 0)

/* Ported from included/linux/bitops.h  */
static __inline__ int ffs(int x)
{
	int r = 1;

	if (!x)
		return 0;
	if (!(x & 0xffff)) {
		x >>= 16;
		r += 16;
	}
	if (!(x & 0xff)) {
		x >>= 8;
		r += 8;
	}
	if (!(x & 0xf)) {
		x >>= 4;
		r += 4;
	}
	if (!(x & 3)) {
		x >>= 2;
		r += 2;
	}
	if (!(x & 1)) {
		x >>= 1;
		r += 1;
	}
	return r;
}
#define __ffs(x) (ffs(x) - 1)

/* find_next_bit - find the next set bit in a memory region
 * (from asm-ppc/bitops.h)
 */
static __inline__ unsigned long find_next_bit(const unsigned long *addr,
	unsigned long size, unsigned long offset)
{
	unsigned int *p = ((unsigned int *) addr) + (offset >> 5);
	unsigned int result = offset & ~31UL;
	unsigned int tmp;

	if (offset >= size)
		return size;
	size -= result;
	offset &= 31UL;
	if (offset) {
		tmp = *p++;
		tmp &= ~0UL << offset;
		if (size < 32)
			goto found_first;
		if (tmp)
			goto found_middle;
		size -= 32;
		result += 32;
	}
	while (size >= 32) {
		tmp = *p++;
		if (tmp != 0)
			goto found_middle;
		result += 32;
		size -= 32;
	}
	if (!size)
		return result;
	tmp = *p;

found_first:
	tmp &= ~0UL >> (32 - size);
	if (tmp == 0UL)        /* Are any bits set? */
		return result + size; /* Nope. */
found_middle:
	return result + __ffs(tmp);
}

#define find_first_bit(addr, size) find_next_bit((addr), (size), 0)

#ifdef __KERNEL__

/* Ported from include/linux/bitops.h */
static  __inline__ int fls(int x)
{
	int r = 32;

	if (!x)
		return 0;
	if (!(x & 0xffff0000u)) {
		x <<= 16;
		r -= 16;
	}
	if (!(x & 0xff000000u)) {
		x <<= 8;
		r -= 8;
	}
	if (!(x & 0xf0000000u)) {
		x <<= 4;
		r -= 4;
	}
	if (!(x & 0xc0000000u)) {
		x <<= 2;
		r -= 2;
	}
	if (!(x & 0x80000000u)) {
		x <<= 1;
		r -= 1;
	}
	return r;
}

static inline int __fls(int x)
{
	return fls(x) - 1;
}

#include <asm-generic/bitops/fls64.h>
#include <asm-generic/bitops/sched.h>
#include <asm-generic/bitops/hweight.h>
#include <asm-generic/bitops/lock.h>

#define minix_find_first_zero_bit(addr, size)	find_next_zero_bit((addr), \
							(size), 0)
#define minix_test_and_set_bit(nr, addr)	test_and_set_bit((nr), \
							(unsigned long *)(addr))
#define minix_set_bit(nr, addr)			set_bit((nr), \
							(unsigned long *)(addr))
#define minix_test_and_clear_bit(nr, addr)	test_and_clear_bit((nr), \
							(unsigned long *)(addr))

static inline int minix_test_bit(int nr, const volatile unsigned long *vaddr)
{
	int 	*a = (int *)vaddr;
	int	mask;

	a += nr >> 5;
	mask = 1 << (nr & 0x1f);
	return ((mask & *a) != 0);
}

#define ext2_set_bit(nr, addr)			test_and_set_bit((nr) ^ 24, \
							(unsigned long *)(addr))
#define ext2_set_bit_atomic(lock, nr, addr)	test_and_set_bit((nr) ^ 24, \
							(unsigned long *)(addr))
#define ext2_clear_bit(nr, addr)		test_and_clear_bit((nr) ^ 24, \
							(unsigned long *)(addr))
#define ext2_clear_bit_atomic(lock, nr, addr)	test_and_clear_bit((nr) ^ 24, \
							(unsigned long *)(addr))

static inline int ext2_test_bit(int nr, const void *vaddr)
{
	const unsigned char *p = vaddr;
	return (p[nr >> 3] & (1U << (nr & 7))) != 0;
}

static inline int ext2_find_first_zero_bit(const void *vaddr, unsigned size)
{
	const unsigned long *p = vaddr, *addr = vaddr;
	int res;

	if (!size)
		return 0;

	size = (size >> 5) + ((size & 31) > 0);
	while (*p++ == ~0UL) {
		if (--size == 0)
			return (p - addr) << 5;
	}

	--p;
	for (res = 0; res < 32; res++)
		if (!ext2_test_bit (res, p))
			break;
	return (p - addr) * 32 + res;
}

static inline int ext2_find_next_zero_bit(const void *vaddr, unsigned size,
					  unsigned offset)
{
	const unsigned long *addr = vaddr;
	const unsigned long *p = addr + (offset >> 5);
	int bit = offset & 31UL, res;

	if (offset >= size)
		return size;

	if (bit) {
		/* Look for zero in first longword */
		for (res = bit; res < 32; res++)
			if (!ext2_test_bit (res, p))
				return (p - addr) * 32 + res;
		p++;
	}
	/* No zero yet, search remaining full bytes for a zero */
	res = ext2_find_first_zero_bit(p, size - 32 * (p - addr));
	return (p - addr) * 32 + res;
}

#endif /* KERNEL */

#endif /* __CF_BITOPS__ */

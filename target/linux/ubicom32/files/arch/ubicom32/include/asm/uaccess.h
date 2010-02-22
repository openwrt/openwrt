/*
 * arch/ubicom32/include/asm/uaccess.h
 *   User space memory access functions for Ubicom32 architecture.
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
 *   arch/alpha
 */
#ifndef _ASM_UBICOM32_UACCESS_H
#define _ASM_UBICOM32_UACCESS_H

/*
 * User space memory access functions
 */
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/string.h>

#include <asm/segment.h>

#define VERIFY_READ	0
#define VERIFY_WRITE	1

/*
 * The exception table consists of pairs of addresses: the first is the
 * address of an instruction that is allowed to fault, and the second is
 * the address at which the program should continue.  No registers are
 * modified, so it is entirely up to the continuation code to figure out
 * what to do.
 *
 * All the routines below use bits of fixup code that are out of line
 * with the main instruction path.  This means when everything is well,
 * we don't even have to jump over them.  Further, they do not intrude
 * on our cache or tlb entries.
 */
struct exception_table_entry
{
	unsigned long insn, fixup;
};

/*
 * Ubicom32 does not currently support the exception table handling.
 */
extern unsigned long search_exception_table(unsigned long);


#if defined(CONFIG_ACCESS_OK_CHECKS_ENABLED)
extern int __access_ok(unsigned long addr, unsigned long size);
#else
static inline int __access_ok(unsigned long addr, unsigned long size)
{
	return 1;
}
#endif
#define access_ok(type, addr, size) \
	likely(__access_ok((unsigned long)(addr), (size)))

/*
 * The following functions do not exist.  They keep callers
 * of put_user and get_user from passing unsupported argument
 * types.  They result in a link time error.
 */
extern int __put_user_bad(void);
extern int __get_user_bad(void);

/*
 * __put_user_no_check()
 *	Put the requested data into the user space verifying the address
 *
 * Careful to not
 * (a) re-use the arguments for side effects (sizeof/typeof is ok)
 * (b) require any knowledge of processes at this stage
 */
#define __put_user_no_check(x, ptr, size)		\
({							\
	int __pu_err = 0;				\
	__typeof__(*(ptr)) __user *__pu_addr = (ptr);	\
	switch (size) {					\
	case 1:						\
	case 2:						\
	case 4:						\
	case 8:						\
		*__pu_addr = (__typeof__(*(ptr)))x;	\
		break;					\
	default:					\
		__pu_err = __put_user_bad();		\
		break;					\
	}						\
	__pu_err;					\
})

/*
 * __put_user_check()
 *	Put the requested data into the user space verifying the address
 *
 * Careful to not
 * (a) re-use the arguments for side effects (sizeof/typeof is ok)
 * (b) require any knowledge of processes at this stage
 *
 * If requested, access_ok() will verify that ptr is a valid user
 * pointer.
 */
#define __put_user_check(x, ptr, size)				\
({								\
	int __pu_err = -EFAULT;					\
	__typeof__(*(ptr)) __user *__pu_addr = (ptr);		\
	if (access_ok(VERIFY_WRITE, __pu_addr, size)) {		\
		__pu_err = 0;					\
		switch (size) {					\
		case 1:						\
		case 2:						\
		case 4:						\
		case 8:						\
			*__pu_addr = (__typeof__(*(ptr)))x;	\
			break;					\
		default:					\
			__pu_err = __put_user_bad();		\
			break;					\
		}						\
	}							\
	__pu_err;						\
})

/*
 * __get_user_no_check()
 *	Read the value at ptr into x.
 *
 * If requested, access_ok() will verify that ptr is a valid user
 * pointer.  If the caller passes a modifying argument for ptr (e.g. x++)
 * this macro will not work.
 */
#define __get_user_no_check(x, ptr, size)			\
({								\
	int __gu_err = 0;					\
	__typeof__((x)) __gu_val = 0;				\
	const __typeof__(*(ptr)) __user *__gu_addr = (ptr);	\
	switch (size) {						\
	case 1:							\
	case 2:							\
	case 4:							\
	case 8:							\
		__gu_val = (__typeof__((x)))*(__gu_addr);	\
		break;						\
	default:						\
		__gu_err = __get_user_bad();			\
		(x) = 0;					\
		break;						\
	}							\
	(x) = __gu_val;						\
	__gu_err;						\
})

/*
 * __get_user_check()
 *	Read the value at ptr into x.
 *
 * If requested, access_ok() will verify that ptr is a valid user
 * pointer.
 */
#define __get_user_check(x, ptr, size)					\
({									\
	int __gu_err = -EFAULT;						\
	__typeof__(x) __gu_val = 0;					\
	const __typeof__(*(ptr)) __user *__gu_addr = (ptr);		\
	if (access_ok(VERIFY_READ, __gu_addr, size)) {			\
		__gu_err = 0;						\
		switch (size) {						\
		case 1:							\
		case 2:							\
		case 4:							\
		case 8:							\
			__gu_val = (__typeof__((x)))*(__gu_addr);	\
			break;						\
		default:						\
			__gu_err = __get_user_bad();			\
			(x) = 0;					\
			break;						\
		}							\
	}								\
	(x) = __gu_val;							\
	__gu_err;							\
})

/*
 * The "xxx" versions are allowed to perform some amount of address
 * space checking.  See access_ok().
 */
#define put_user(x,ptr) \
	__put_user_check((__typeof__(*(ptr)))(x),(ptr), sizeof(*(ptr)))
#define get_user(x,ptr) \
	__get_user_check((x), (ptr), sizeof(*(ptr)))

/*
 * The "__xxx" versions do not do address space checking, useful when
 * doing multiple accesses to the same area (the programmer has to do the
 * checks by hand with "access_ok()")
 */
#define __put_user(x,ptr) \
	__put_user_no_check((__typeof__(*(ptr)))(x),(ptr), sizeof(*(ptr)))
#define __get_user(x,ptr) \
	__get_user_no_check((x), (ptr), sizeof(*(ptr)))

/*
 * __copy_tofrom_user_no_check()
 *	Copy the data either to or from user space.
 *
 * Return the number of bytes NOT copied.
 */
static inline unsigned long
__copy_tofrom_user_no_check(void *to, const void *from, unsigned long n)
{
	memcpy(to, from, n);
	return 0;
}

/*
 * copy_to_user()
 * 	Copy the kernel data to user space.
 *
 * Return the number of bytes that were copied.
 */
static inline unsigned long
copy_to_user(void __user *to, const void *from, unsigned long n)
{
	if (!access_ok(VERIFY_WRITE, to, n)) {
		return n;
	}
	return __copy_tofrom_user_no_check((__force void *)to, from, n);
}

/*
 * copy_from_user()
 * 	Copy the user data to kernel space.
 *
 * Return the number of bytes that were copied.  On error, we zero
 * out the destination.
 */
static inline unsigned long
copy_from_user(void *to, const void __user *from, unsigned long n)
{
	if (!access_ok(VERIFY_READ, from, n)) {
		return n;
	}
	return __copy_tofrom_user_no_check(to, (__force void *)from, n);
}

#define __copy_to_user(to, from, n) \
	__copy_tofrom_user_no_check((__force void *)to, from, n)
#define __copy_from_user(to, from, n) \
	__copy_tofrom_user_no_check(to, (__force void *)from, n)
#define __copy_to_user_inatomic(to, from, n) \
	__copy_tofrom_user_no_check((__force void *)to, from, n)
#define __copy_from_user_inatomic(to, from, n) \
	__copy_tofrom_user_no_check(to, (__force void *)from, n)

#define copy_to_user_ret(to, from, n, retval) \
	({ if (copy_to_user(to, from, n)) return retval; })

#define copy_from_user_ret(to, from, n, retval) \
	({ if (copy_from_user(to, from, n)) return retval; })

/*
 * strncpy_from_user()
 *	Copy a null terminated string from userspace.
 *
 * dst - Destination in kernel space.  The buffer must be at least count.
 * src - Address of string in user space.
 * count - Maximum number of bytes to copy (including the trailing NULL).
 *
 * Returns the length of the string (not including the trailing NULL.  If
 * count is smaller than the length of the string, we copy count bytes
 * and return count.
 *
 */
static inline long strncpy_from_user(char *dst, const __user char *src, long count)
{
	char *tmp;
	if (!access_ok(VERIFY_READ, src, 1)) {
		return -EFAULT;
	}

	strncpy(dst, src, count);
	for (tmp = dst; *tmp && count > 0; tmp++, count--) {
		;
	}
	return(tmp - dst);
}

/*
 * strnlen_user()
 *	Return the size of a string (including the ending 0)
 *
 * Return -EFAULT on exception, a value greater than <n> if too long
 */
static inline long strnlen_user(const __user char *src, long n)
{
	if (!access_ok(VERIFY_READ, src, 1)) {
		return -EFAULT;
	}
	return(strlen(src) + 1);
}

#define strlen_user(str) strnlen_user(str, 32767)

/*
 * __clear_user()
 *	Zero Userspace
 */
static inline unsigned long __clear_user(__user void *to, unsigned long n)
{
	memset(to, 0, n);
	return 0;
}

/*
 * clear_user()
 *	Zero user space (check for valid addresses)
 */
static inline unsigned long clear_user(__user void *to, unsigned long n)
{
	if (!access_ok(VERIFY_WRITE, to, n)) {
		return -EFAULT;
	}
	return __clear_user(to, n);
}

#endif /* _ASM_UBICOM32_UACCESS_H */

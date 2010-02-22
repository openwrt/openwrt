/*
 * arch/ubicom32/include/asm/checksum.h
 *   Checksum utilities for Ubicom32 architecture.
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
#ifndef _ASM_UBICOM32_CHECKSUM_H
#define _ASM_UBICOM32_CHECKSUM_H

#include <linux/in6.h>

/*
 * computes the checksum of a memory block at buff, length len,
 * and adds in "sum" (32-bit)
 *
 * returns a 32-bit number suitable for feeding into itself
 * or csum_tcpudp_magic
 *
 * this function must be called with even lengths, except
 * for the last fragment, which may be odd
 *
 * it's best to have buff aligned on a 32-bit boundary
 */
__wsum csum_partial(const void *buff, int len, __wsum sum);

/*
 * the same as csum_partial, but copies from src while it
 * checksums
 *
 * here even more important to align src and dst on a 32-bit (or even
 * better 64-bit) boundary
 */

__wsum csum_partial_copy_nocheck(const void *src, void *dst,
	int len, __wsum sum);


/*
 * the same as csum_partial_copy, but copies from user space.
 *
 * here even more important to align src and dst on a 32-bit (or even
 * better 64-bit) boundary
 */

extern __wsum csum_partial_copy_from_user(const void __user *src,
	void *dst, int len, __wsum sum, int *csum_err);

__sum16 ip_fast_csum(const void *iph, unsigned int ihl);

/*
 *	Fold a partial checksum
 */

static inline __sum16 csum_fold(__wsum sum)
{
	asm volatile (
	"	lsr.4	d15, %0, #16	\n\t"
	"	bfextu	%0, %0, #16	\n\t"
	"	add.4	%0, d15, %0	\n\t"
	"	lsr.4	d15, %0, #16	\n\t"
	"	bfextu	%0, %0, #16	\n\t"
	"	add.4	%0, d15, %0	\n\t"
		: "=&d" (sum)
		: "0"(sum)
		: "d15"
	);
	return (__force __sum16)~sum;
}


/*
 * computes the checksum of the TCP/UDP pseudo-header
 * returns a 16-bit checksum, already complemented
 */

static inline __wsum
csum_tcpudp_nofold(__be32 saddr, __be32 daddr, unsigned short len,
		  unsigned short proto, __wsum sum)
{
	asm volatile (
	"	add.4	%0, %2, %0	\n\t"
	"	addc	%0, %3, %0	\n\t"
	"	addc	%0, %4, %0	\n\t"
	"	addc	%0, %5, %0	\n\t"
	"	addc	%0, #0, %0	\n\t"
		: "=&d" (sum)
		: "0"(sum), "r" (saddr), "r" (daddr), "r" (len), "r"(proto)
	);
	return sum;
}

static inline __sum16
csum_tcpudp_magic(__be32 saddr, __be32 daddr, unsigned short len,
		  unsigned short proto, __wsum sum)
{
	return csum_fold(csum_tcpudp_nofold(saddr,daddr,len,proto,sum));
}

/*
 * this routine is used for miscellaneous IP-like checksums, mainly
 * in icmp.c
 */
extern __sum16 ip_compute_csum(const void *buff, int len);

#define _HAVE_ARCH_IPV6_CSUM

static __inline__ __sum16
csum_ipv6_magic(const struct in6_addr *saddr, const struct in6_addr *daddr,
		__u32 len, unsigned short proto, __wsum sum)
{
	asm volatile (
	"	add.4	%0, 0(%2), %0	\n\t"
	"	addc	%0, 4(%2), %0	\n\t"
	"	addc	%0, 8(%2), %0	\n\t"
	"	addc	%0, 12(%2), %0	\n\t"
	"	addc	%0, 0(%3), %0	\n\t"
	"	addc	%0, 4(%3), %0	\n\t"
	"	addc	%0, 8(%3), %0	\n\t"
	"	addc	%0, 12(%3), %0	\n\t"
	"	addc	%0, %4, %0	\n\t"
	"	addc	%0, #0, %0	\n\t"
		: "=&d" (sum)
		: "0" (sum), "a" (saddr), "a" (daddr), "d" (len + proto)
	);
	return csum_fold(sum);
}

#endif /* _ASM_UBICOM32_CHECKSUM_H */

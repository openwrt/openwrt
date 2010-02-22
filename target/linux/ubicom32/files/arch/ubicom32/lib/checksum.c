/*
 * arch/ubicom32/lib/checksum.c
 *   Optimized checksum utilities for IP.
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
/*
 * INET		An implementation of the TCP/IP protocol suite for the LINUX
 *		operating system.  INET is implemented using the  BSD Socket
 *		interface as the means of communication with the user level.
 *
 *		IP/TCP/UDP checksumming routines
 *
 * Authors:	Jorge Cwik, <jorge@laser.satlink.net>
 *		Arnt Gulbrandsen, <agulbra@nvg.unit.no>
 *		Tom May, <ftom@netcom.com>
 *		Andreas Schwab, <schwab@issan.informatik.uni-dortmund.de>
 *		Lots of code moved from tcp.c and ip.c; see those files
 *		for more names.
 *
 * 03/02/96	Jes Sorensen, Andreas Schwab, Roman Hodek:
 *		Fixed some nasty bugs, causing some horrible crashes.
 *		A: At some points, the sum (%0) was used as
 *		length-counter instead of the length counter
 *		(%1). Thanks to Roman Hodek for pointing this out.
 *		B: GCC seems to mess up if one uses too many
 *		data-registers to hold input values and one tries to
 *		specify d0 and d1 as scratch registers. Letting gcc choose these
 *		registers itself solves the problem.
 *
 *		This program is free software; you can redistribute it and/or
 *		modify it under the terms of the GNU General Public License
 *		as published by the Free Software Foundation; either version
 *		2 of the License, or (at your option) any later version.
 */

/* Revised by Kenneth Albanowski for m68knommu. Basic problem: unaligned access kills, so most
   of the assembly has to go. */

#include <linux/module.h>
#include <net/checksum.h>

static unsigned long do_csum(const unsigned char * buff, int len)
{
	int count;
	unsigned long result = 0;

	/*
	 * The following optimized assembly code cannot handle data length less than 7 bytes!
	 */
	if (likely(len >= 7)) {
		len -= (4 - (int)buff) & 3;
		count = len >> 2;
		asm (
		"	sub.4		d15, #0, %2		\n\t"	// set up for jump table
		"	and.4		d15, #(32-1), d15	\n\t"	// d15 = (-m) & (32 - 1)

		"	bfextu		d14, %0, #2		\n\t"	// test 2 LSB of buff
		"	jmpne.w.f	100f			\n\t"
		"	add.4		%1, #0, %1		\n\t"	// clear C
		"	moveai		a3, #%%hi(1f)		\n\t"	// table jump
		"	lea.1		a3, %%lo(1f)(a3)	\n\t"
		"	lea.4		a3, (a3,d15)		\n\t"
		"	calli		a3, 0(a3)		\n\t"

		"100:	sub.4		%0, %0, d14		\n\t"
		"	sub.4		d14, #4, d14		\n\t"
		"	lsl.4		d14, d14, #3		\n\t"
		"	add.4		%1, #0, %1		\n\t"	// clear C
		"	moveai		a3, #%%hi(1f)		\n\t"	// table jump
		"	lea.1		a3, %%lo(1f)(a3)	\n\t"
		"	lea.4		a3, (a3,d15)		\n\t"
		"	bfextu		%1, (%0)4++, d14	\n\t"	// read first partial word
		"	calli		a3, 0(a3)		\n\t"
#if 1
		"200:	lsl.4		%3, %3, #3		\n\t"
		"	bfrvrs		d15, (%0), #0		\n\t"	// read last word (partial)
		"	bfextu		d15, d15, %3		\n\t"
		"	bfrvrs		d15, d15, #0		\n\t"
		"	add.4		%1, d15, %1		\n\t"
		"	addc		%1, #0, %1		\n\t"	// sample C again
		"	jmpt.w.t	2f			\n\t"
#else
		"200:	move.1		d15, 0(%0)		\n\t"
		"	lsl.4		d15, d15, #8		\n\t"
		"	add.4		%1, d15, %1		\n\t"
		"	addc		%1, #0, %1		\n\t"	// sample C again
		"	add.4		%3, #-1, %3		\n\t"
		"	jmpeq.w.t	2f			\n\t"

		"	move.1		d15, 1(%0)		\n\t"
		"	add.4		%1, d15, %1		\n\t"
		"	addc		%1, #0, %1		\n\t"	// sample C again
		"	add.4		%3, #-1, %3		\n\t"
		"	jmpeq.w.t	2f			\n\t"

		"	move.1		d15, 2(%0)		\n\t"
		"	lsl.4		d15, d15, #8		\n\t"
		"	add.4		%1, d15, %1		\n\t"
		"	addc		%1, #0, %1		\n\t"	// sample C again
		"	jmpt.w.t	2f			\n\t"
#endif
#if defined(IP7000) || defined(IP7000_REV2)
		"300:	swapb.2		%1, %1			\n\t"
#else
		"300:	shmrg.2		%1, %1, %1		\n\t"
		"	lsr.4		%1, %1, #8		\n\t"
		"	bfextu		%1, %1, #16		\n\t"
#endif
		"	jmpt.w.t	3f			\n\t"

		"1:	add.4		%1, (%0)4++, %1		\n\t"	// first add without C
		"	.rept		31			\n\t"
		"	addc		%1, (%0)4++, %1		\n\t"
		"	.endr					\n\t"
		"	addc		%1, #0, %1		\n\t"	// sample C again
		"	add.4		%2, #-32, %2		\n\t"
		"	jmpgt.w.t	1b			\n\t"

		"	and.4		%3, #3, %3		\n\t"	// check n
		"	jmpne.w.f	200b			\n\t"

		"2:	.rept		2			\n\t"
		"	lsr.4		d15, %1, #16		\n\t"
		"	bfextu		%1, %1, #16		\n\t"
		"	add.4		%1, d15, %1		\n\t"
		"	.endr					\n\t"
		"	btst		d14, #3			\n\t"	// start from odd address (<< 3)?
		"	jmpne.w.f	300b			\n\t"
		"3:						\n\t"

			: "+a"(buff), "+d"(result), "+d"(count), "+d"(len)
			:
			: "d15", "d14", "a3", "cc"
		);

		return result;
	}

	/*
	 * handle a few bytes and fold result into 16-bit
	 */
	while (len-- > 0) {
		result += (*buff++ << 8);
		if (len) {
			result += *buff++;
			len--;
		}
	}
	asm (
	"	.rept		2			\n\t"
	"	lsr.4		d15, %0, #16		\n\t"
	"	bfextu		%0, %0, #16		\n\t"
	"	add.4		%0, d15, %0		\n\t"
	"	.endr					\n\t"
		: "+d" (result)
		:
		: "d15", "cc"
	);

	return result;
}

/*
 *	This is a version of ip_compute_csum() optimized for IP headers,
 *	which always checksum on 4 octet boundaries.
 */
__sum16 ip_fast_csum(const void *iph, unsigned int ihl)
{
	return (__force __sum16)~do_csum(iph,ihl*4);
}

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
__wsum csum_partial(const void *buff, int len, __wsum sum)
{
	unsigned int result = do_csum(buff, len);

	/* add in old sum, and carry.. */
	result += (__force u32)sum;
	if ((__force u32)sum > result)
		result += 1;
	return (__force __wsum)result;
}

EXPORT_SYMBOL(csum_partial);

/*
 * this routine is used for miscellaneous IP-like checksums, mainly
 * in icmp.c
 */
__sum16 ip_compute_csum(const void *buff, int len)
{
	return (__force __sum16)~do_csum(buff,len);
}

/*
 * copy from fs while checksumming, otherwise like csum_partial
 */

__wsum
csum_partial_copy_from_user(const void __user *src, void *dst,
			    int len, __wsum sum, int *csum_err)
{
	if (csum_err) *csum_err = 0;
	memcpy(dst, (__force const void *)src, len);
	return csum_partial(dst, len, sum);
}

/*
 * copy from ds while checksumming, otherwise like csum_partial
 */

__wsum
csum_partial_copy_nocheck(const void *src, void *dst, int len, __wsum sum)
{
	memcpy(dst, src, len);
	return csum_partial(dst, len, sum);
}

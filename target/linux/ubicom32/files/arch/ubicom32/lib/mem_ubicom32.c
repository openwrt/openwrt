/*
 * arch/ubicom32/lib/mem_ubicom32.c
 *   String functions.
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

#include <linux/module.h>
#include <linux/types.h>
#include <linux/compiler.h>

#define LIKELY likely
#define UNLIKELY unlikely

typedef u32_t addr_t;

/*
 * memcpy()
 */
void *memcpy(void *dest, const void *src, size_t n)
{
	void *dest_ret = dest;

	if (LIKELY((((addr_t)dest ^ (addr_t)src) & 3) == 0) && LIKELY(n > 6)) {
		size_t m;
		n -= (4 - (addr_t)dest) & 0x03;
		m = n >> 2;
		asm volatile (
		"	sub.4		d15, #0, %2		\n\t"	// set up for jump table
		"	and.4		d15, #(32-1), d15	\n\t"	// d15 = (-m) & (32 - 1)
		"	moveai		a3, #%%hi(1f)		\n\t"
		"	lea.1		a3, %%lo(1f)(a3)	\n\t"
		"	lea.4		a3, (a3,d15)		\n\t"

		"	bfextu		d15, %0, #2		\n\t"	// d15 = (dest & 3)
		"	jmpne.w.f	100f			\n\t"
		"	calli		a3, 0(a3)		\n\t"	// 4-byte alignment

		"100:	cmpi		d15, #2			\n\t"
		"	jmpne.s.f	101f			\n\t"
		"	move.2		(%0)2++, (%1)2++	\n\t"
		"	calli		a3, 0(a3)		\n\t"	// 2-byte alignment

		"101:	move.1		(%0)1++, (%1)1++	\n\t"
		"	jmpgt.s.f	102f			\n\t"	// 3-byte alignment
		"	move.2		(%0)2++, (%1)2++	\n\t"	// 1-byte alignment
		"102:	calli		a3, 0(a3)		\n\t"

		"200:	cmpi		%3, #2			\n\t"
		"	jmplt.s.f	201f			\n\t"
		"	move.2		(%0)2++, (%1)2++	\n\t"
		"	jmpeq.s.t	2f			\n\t"
		"201:	move.1		(%0)1++, (%1)1++	\n\t"
		"	jmpt.w.t	2f			\n\t"

		"1:	.rept		25			\n\t"
		"	movea		(%0)4++, (%1)4++	\n\t"
		"	.endr					\n\t"
		"	.rept		7			\n\t"
		"	move.4		(%0)4++, (%1)4++	\n\t"
		"	.endr					\n\t"
		"	add.4		%2, #-32, %2		\n\t"
		"	jmpgt.w.f	1b			\n\t"

		"	and.4		%3, #3, %3		\n\t"	// check n
		"	jmpne.w.f	200b			\n\t"
		"2:						\n\t"
			: "+a" (dest), "+a" (src), "+d" (m), "+d" (n)
			:
			: "d15", "a3", "memory", "cc"
		);

		return dest_ret;
	}

	if (LIKELY((((addr_t)dest ^ (addr_t)src) & 1) == 0) && LIKELY(n > 2)) {
		size_t m;
		n -= (addr_t)dest & 0x01;
		m = n >> 1;
		asm volatile (
		"	sub.4		d15, #0, %2		\n\t"	// set up for jump table
		"	and.4		d15, #(32-1), d15	\n\t"	// d15 = (-m) & (32 - 1)
		"	moveai		a3, #%%hi(1f)		\n\t"
		"	lea.1		a3, %%lo(1f)(a3)	\n\t"
		"	lea.4		a3, (a3,d15)		\n\t"

		"	btst		%0, #0			\n\t"	// check bit 0
		"	jmpne.w.f	100f			\n\t"
		"	calli		a3, 0(a3)		\n\t"	// 4-byte alignment

		"100:	move.1		(%0)1++, (%1)1++	\n\t"
		"	calli		a3, 0(a3)		\n\t"

		"200:	move.1		(%0)1++, (%1)1++	\n\t"
		"	jmpt.w.t	2f			\n\t"

		"1:	.rept		32			\n\t"
		"	move.2		(%0)2++, (%1)2++	\n\t"
		"	.endr					\n\t"
		"	add.4		%2, #-32, %2		\n\t"
		"	jmpgt.w.f	1b			\n\t"

		"	and.4		%3, #1, %3		\n\t"	// check n
		"	jmpne.w.f	200b			\n\t"
		"2:						\n\t"

			: "+a" (dest), "+a" (src), "+d" (m), "+d" (n)
			:
			: "d15", "a3", "memory", "cc"
		);

		return dest_ret;
	}

	asm volatile (
	"	sub.4		d15, #0, %2		\n\t"
	"	jmpeq.w.f	2f			\n\t"
	"	and.4		d15, #(16-1), d15	\n\t"	// d15 = (-n) & (16 - 1)
	"	moveai		a3, #%%hi(1f)		\n\t"
	"	lea.1		a3, %%lo(1f)(a3)	\n\t"
	"	lea.4		a3, (a3,d15)		\n\t"
	"	calli		a3, 0(a3)		\n\t"

	"1:	.rept		16			\n\t"
	"	move.1		(%0)1++, (%1)1++	\n\t"
	"	.endr					\n\t"
	"	add.4		%2, #-16, %2		\n\t"
	"	jmpgt.w.f	1b			\n\t"
	"2:						\n\t"

		: "+a" (dest), "+a" (src), "+d" (n)
		:
		: "d15", "a3", "memory", "cc"
	);

	return dest_ret;
}

/*
 * memset()
 */
void *memset(void *s, int c, size_t n)
{
	void *s_ret = s;

	if (LIKELY(n > 6)) {
		size_t m;
		n -= (4 - (addr_t)s) & 0x03;
		m = n >> 2;
		asm volatile (
		"	sub.4		d15, #0, %2		\n\t"	// set up for jump table
		"	and.4		d15, #(32-1), d15	\n\t"	// d15 = (-m) & (32 - 1)
		"	shmrg.1		%1, %1, %1		\n\t"
		"	shmrg.2		%1, %1, %1		\n\t"	// %1 = (c<<24)|(c<<16)|(c<<8)|c
		"	moveai		a3, #%%hi(1f)		\n\t"
		"	lea.1		a3, %%lo(1f)(a3)	\n\t"
		"	lea.4		a3, (a3,d15)		\n\t"

		"	bfextu		d15, %0, #2		\n\t"	// d15 = (s & 3)
		"	jmpne.w.f	100f			\n\t"
		"	calli		a3, 0(a3)		\n\t"	// 4-byte alignment

		"100:	cmpi		d15, #2			\n\t"
		"	jmpne.s.f	101f			\n\t"
		"	move.2		(%0)2++, %1		\n\t"
		"	calli		a3, 0(a3)		\n\t"	// 2-byte alignment

		"101:	move.1		(%0)1++, %1		\n\t"
		"	jmpgt.s.f	102f			\n\t"	// 3-byte alignment
		"	move.2		(%0)2++, %1		\n\t"	// 1-byte alignment
		"102:	calli		a3, 0(a3)		\n\t"

		"200:	cmpi		%3, #2			\n\t"
		"	jmplt.s.f	201f			\n\t"
		"	move.2		(%0)2++, %1		\n\t"
		"	jmpeq.s.t	2f			\n\t"
		"201:	move.1		(%0)1++, %1		\n\t"
		"	jmpt.w.t	2f			\n\t"

		"1:	.rept		25			\n\t"
		"	movea		(%0)4++, %1		\n\t"
		"	.endr					\n\t"
		"	.rept		7			\n\t"
		"	move.4		(%0)4++, %1		\n\t"
		"	.endr					\n\t"
		"	add.4		%2, #-32, %2		\n\t"
		"	jmpgt.w.f	1b			\n\t"

		"	and.4		%3, #3, %3		\n\t"	// test bit 1 of n
		"	jmpne.w.f	200b			\n\t"
		"2:						\n\t"

			: "+a" (s), "+d" (c), "+d" (m), "+d" (n)
			:
			: "d15", "a3", "memory", "cc"
		);

		return s_ret;
	}

	asm volatile (
	"	sub.4		d15, #0, %2		\n\t"
	"	jmpeq.w.f	2f			\n\t"
	"	and.4		d15, #(8-1), d15	\n\t"	// d15 = (-%2) & (16 - 1)
	"	moveai		a3, #%%hi(1f)		\n\t"
	"	lea.1		a3, %%lo(1f)(a3)	\n\t"
	"	lea.4		a3, (a3,d15)		\n\t"
	"	calli		a3, 0(a3)		\n\t"

	"1:	.rept		8			\n\t"
	"	move.1		(%0)1++, %1		\n\t"
	"	.endr					\n\t"
	"2:						\n\t"

		: "+a" (s), "+d" (c), "+d" (n)
		:
		: "d15", "a3", "memory", "cc"
	);

	return s_ret;
}

void *memmove(void *dest, const void *src, size_t n)
{
	char *tmp;
	const char *s;

	if (n == 0)
		return dest;

	tmp = dest;
	s = src;

	/*
	 * Will perform 16-bit move if possible
	 */
	if (likely((((u32)dest | (u32)src | n) & 1) == 0)) {
		if (dest <= src) {
			asm volatile (
			"	sub.4		d15, #0, %2		\n\t"	// set up for jump table
			"	and.4		d15, #(32-2), d15	\n\t"	// d15 = (- count) & (32 - 2)
			"	moveai		a3, #%%hi(1f)		\n\t"
			"	lea.1		a3, %%lo(1f)(a3)	\n\t"
			"	lea.2		a3, (a3,d15)		\n\t"
			"	calli		a3, 0(a3)		\n\t"

			"1:	.rept		16			\n\t"
			"	move.2		(%0)2++, (%1)2++	\n\t"
			"	.endr					\n\t"
			"	add.4		%2, #-32, %2		\n\t"
			"	jmpgt.w.f	1b			\n\t"

			: "+a" (tmp), "+a" (s), "+d" (n)
			:
			: "d15", "a3", "memory", "cc"
			);
		} else {
			tmp += n;
			s += n;
			asm volatile (
			"	sub.4		d15, #0, %2		\n\t"	// set up for jump table
			"	and.4		d15, #(32-2), d15	\n\t"	// d15 = (- count) & (32 - 2)
			"	moveai		a3, #%%hi(1f)		\n\t"
			"	lea.1		a3, %%lo(1f)(a3)	\n\t"
			"	lea.2		a3, (a3,d15)		\n\t"
			"	calli		a3, 0(a3)		\n\t"

			"1:	.rept		16			\n\t"
			"	move.2		-2(%0)++, -2(%1)++	\n\t"
			"	.endr					\n\t"
			"	add.4		%2, #-32, %2		\n\t"
			"	jmpgt.w.f	1b			\n\t"

			: "+a" (tmp), "+a" (s), "+d" (n)
			:
			: "d15", "a3", "memory", "cc"
			);
		}
		return dest;
	}

	if (dest <= src) {
		asm volatile (
		"	sub.4		d15, #0, %2		\n\t"	// set up for jump table
		"	and.4		d15, #(16-1), d15	\n\t"	// d15 = (- count) & (16 - 1)
		"	moveai		a3, #%%hi(1f)		\n\t"
		"	lea.1		a3, %%lo(1f)(a3)	\n\t"
		"	lea.4		a3, (a3,d15)		\n\t"
		"	calli		a3, 0(a3)		\n\t"

		"1:	.rept		16			\n\t"
		"	move.1		(%0)1++, (%1)1++	\n\t"
		"	.endr					\n\t"
		"	add.4		%2, #-16, %2		\n\t"
		"	jmpgt.w.f	1b			\n\t"
		: "+a" (tmp), "+a" (s), "+d" (n)
		:
		: "d15", "a3", "memory", "cc"
		);
	} else {
		tmp += n;
		s += n;
		asm volatile (
		"	sub.4		d15, #0, %2		\n\t"	// set up for jump table
		"	and.4		d15, #(16-1), d15	\n\t"	// d15 = (- count) & (16 - 1)
		"	moveai		a3, #%%hi(1f)		\n\t"
		"	lea.1		a3, %%lo(1f)(a3)	\n\t"
		"	lea.4		a3, (a3,d15)		\n\t"
		"	calli		a3, 0(a3)		\n\t"

		"1:	.rept		16			\n\t"
		"	move.1		-1(%0)++, -1(%1)++	\n\t"
		"	.endr					\n\t"
		"	add.4		%2, #-16, %2		\n\t"
		"	jmpgt.w.f	1b			\n\t"
		: "+a" (tmp), "+a" (s), "+d" (n)
		:
		: "d15", "a3", "memory", "cc"
		);
	}
	return dest;
}

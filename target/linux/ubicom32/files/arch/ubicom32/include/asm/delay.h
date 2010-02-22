/*
 * arch/ubicom32/include/asm/delay.h
 *   Definition of delay routines for Ubicom32 architecture.
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
#ifndef _ASM_UBICOM32_DELAY_H
#define _ASM_UBICOM32_DELAY_H

#include <asm/param.h>
#include <asm/ip5000.h>

static inline void __delay(unsigned long loops)
{
	if (loops == 0) {
		return;
	}

	asm volatile (
	"1:	add.4	%0, #-1, %0		\n\t"
	"	jmpne.t	1b			\n\t"
	: "+d" (loops)
	);
}

/*
 *	Ubicom32 processor uses fixed 12MHz external OSC.
 *	So we use that as reference to count 12 cycles/us
 */

extern unsigned long loops_per_jiffy;

static inline void _udelay(unsigned long usecs)
{
#if defined(CONFIG_UBICOM32_V4) || defined(CONFIG_UBICOM32_V3)
	asm volatile (
		"	add.4		d15, 0(%0), %1			\n\t"
		"	sub.4		#0, 0(%0), d15			\n\t"
		"	jmpmi.w.f	.-4				\n\t"
		:
		: "a"(TIMER_BASE + TIMER_MPTVAL), "d"(usecs * (12000000/1000000))
		: "d15"
	);
#else
	BUG();
#endif
}

/*
 *	Moved the udelay() function into library code, no longer inlined.
 */
extern void udelay(unsigned long usecs);

#endif /* _ASM_UBICOM32_DELAY_H */

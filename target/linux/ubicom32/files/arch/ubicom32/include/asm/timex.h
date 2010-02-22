/*
 * arch/ubicom32/include/asm/timex.h
 *   Ubicom32 architecture timex specifications.
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
#ifndef _ASM_UBICOM32_TIMEX_H
#define _ASM_UBICOM32_TIMEX_H

#define CLOCK_TICK_RATE	266000000

// #define ARCH_HAS_READ_CURRENT_TIMER

typedef unsigned long cycles_t;

static inline cycles_t get_cycles(void)
{
	return 0;
}

extern int timer_alloc(void);
extern void timer_set(int timervector, unsigned int cycles);
extern int timer_reset(int timervector, unsigned int cycles);
extern void timer_tick_init(void);
extern void timer_device_init(void);

#if defined(CONFIG_GENERIC_CLOCKEVENTS_BROADCAST)
extern void local_timer_interrupt(void);
#endif

#if defined(CONFIG_LOCAL_TIMERS) || defined(CONFIG_GENERIC_CLOCKEVENTS_BROADCAST)
extern int local_timer_setup(unsigned int cpu);
#endif

#endif /* _ASM_UBICOM32_TIMEX_H */

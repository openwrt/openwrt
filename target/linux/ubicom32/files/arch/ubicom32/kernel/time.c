/*
 * arch/ubicom32/kernel/time.c
 *	Initialize the timer list and start the appropriate timers.
 *
 * (C) Copyright 2009, Ubicom, Inc.
 * Copyright (C) 1991, 1992, 1995  Linus Torvalds
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

#include <linux/profile.h>
#include <linux/smp.h>
#include <asm/ip5000.h>
#include <asm/machdep.h>

/*
 * A bitmap of the timers on the processor indicates
 * that the timer is free or in-use.
 */
static unsigned int timers;

/*
 * timer_set()
 *	Init the specified compare register to go off <n> cycles from now.
 */
void timer_set(int timervector, unsigned int cycles)
{
	int idx = UBICOM32_VECTOR_TO_TIMER_INDEX(timervector);
	UBICOM32_IO_TIMER->syscom[idx] =
			UBICOM32_IO_TIMER->sysval + cycles;
	ldsr_enable_vector(timervector);
}

/*
 * timer_reset()
 *	Set/reset the timer to go off again.
 *
 * Because sysval is a continuous timer, this function is able
 * to ensure that we do not have clock sku by using the previous
 * value in syscom to set the next value for syscom.
 *
 * Returns the number of ticks that transpired since the last event.
 */
int timer_reset(int timervector, unsigned int cycles)
{
	/*
	 * Reset the timer in the LDSR thread to go off appropriately.
	 *
	 * Use the previous value of the timer to calculate the new stop
	 * time.  This allows us to account for it taking an
	 * indeterminate amount of time to get here.
	 */
	const int timer_index = UBICOM32_VECTOR_TO_TIMER_INDEX(timervector);
	unsigned int prev = UBICOM32_IO_TIMER->syscom[timer_index];
	unsigned int next = prev + cycles;
	int scratchpad3;
	int diff;
	int ticks = 1;

	/*
	 * If the difference is negative, we have missed at least one
	 * timer tick.
	 *
	 * TODO: Decide if we want to "ignore" time (as done below) or
	 * if we want to process time (unevenly) by calling timer_tick()
	 * lost_ticks times.
	 */
	while (1) {
		/*
		 * Set our future time first.
		 */
		UBICOM32_IO_TIMER->syscom[timer_index] = next;

		/*
		 * Then check if we are really set time in the futrue.
		 */
		diff = (int)next - (int)UBICOM32_IO_TIMER->sysval;
		if (diff >= 0) {
			break;
		}

		/*
		 * Oops, we are too slow. Playing catch up.
		 *
		 * If the debugger is connected the there is a good
		 * chance that we lost time because we were in a
		 * break-point, so in this case we do not print out
		 * diagnostics.
		 */
		asm volatile ("move.4 %0, scratchpad3"
			      : "=r" (scratchpad3));
		if ((scratchpad3 & 0x1) == 0) {
			/*
			 * No debugger attached, print to the console
			 */
			printk(KERN_EMERG "diff: %d, timer has lost %u "
			       "ticks [rounded up]\n",
			       -diff,
			       (unsigned int)((-diff + cycles - 1) / cycles));
		}

		do {
			next += cycles;
			diff = (int)next - (int)UBICOM32_IO_TIMER->sysval;
			ticks++;
		} while (diff < 0);
	}
	return ticks;
}

/*
 * sched_clock()
 *	Returns current time in nano-second units.
 *
 * Notes:
 * 1) This is an override for the weak alias in
 * kernel/sched_clock.c.
 * 2) Do not use xtime_lock as this function is
 * sometimes called with xtime_lock held.
 * 3) We use a retry algorithm to ensure that
 * we get a consistent value.
 * 4) sched_clock must be overwritten if IRQ tracing
 * is enabled because the default implementation uses
 * the xtime_lock sequence while holding xtime_lock.
 */
unsigned long long sched_clock(void)
{
	unsigned long long my_jiffies;
	unsigned long jiffies_top;
	unsigned long jiffies_bottom;

	do {
		jiffies_top = jiffies_64 >> 32;
		jiffies_bottom = jiffies_64 & 0xffffffff;
	} while (unlikely(jiffies_top != (unsigned long)(jiffies_64 >> 32)));

	my_jiffies = ((unsigned long long)jiffies_top << 32) | (jiffies_bottom);
	return (my_jiffies - INITIAL_JIFFIES) * (NSEC_PER_SEC / HZ);
}

/*
 * timer_free()
 *	Free a hardware timer.
 */
void timer_free(int interrupt)
{
	unsigned int bit = interrupt - TIMER_INT(0);

	/*
	 * The timer had not been allocated.
	 */
	BUG_ON(timers & (1 << bit));
	timers |= (1 << bit);
}

/*
 * timer_alloc()
 *	Allocate a hardware timer.
 */
int timer_alloc(void)
{
	unsigned int bit = find_first_bit((unsigned long *)&timers, 32);
	if (!bit) {
		printk(KERN_WARNING "no more free timers\n");
		return -1;
	}

	timers &= ~(1 << bit);
	return bit + TIMER_INT(0);
}

/*
 * time_init()
 *	Time init function.
 */
void time_init(void)
{
	/*
	 * Find the processor node and determine what timers are
	 * available for us.
	 */
	timers = processor_timers();
	if (timers == 0) {
		printk(KERN_WARNING "no timers are available for Linux\n");
		return;
	}

#ifdef CONFIG_GENERIC_CLOCKEVENTS
	timer_device_init();
#else
	timer_tick_init();
#endif
}

/*
 * arch/ubicom32/kernel/timer_tick.c
 *	Impelemets a perodic timer.
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

#include <asm/ip5000.h>
#include <asm/machdep.h>
#if defined(CONFIG_SMP)
#include <asm/smp.h>
#endif

static unsigned int timervector;
static unsigned int frequency;

/*
 * timer_tick()
 *	Kernel system timer support. Needs to keep up the real-time clock,
 * 	as well as call the "do_timer()" routine every clocktick.
 */
static irqreturn_t timer_tick(int irq, void *dummy)
{
	int ticks;

	BUG_ON(!irqs_disabled());
	ticks = timer_reset(timervector, frequency);

	write_seqlock(&xtime_lock);
	do_timer(ticks);
	write_sequnlock(&xtime_lock);

	update_process_times(user_mode(get_irq_regs()));
	profile_tick(CPU_PROFILING);

#if defined(CONFIG_SMP)
	smp_send_timer_all();
#endif
	return(IRQ_HANDLED);
}

/*
 * Data used by setup_irq for the timer.
 */
static struct irqaction timer_irq = {
	.name	 = "timer",
	.flags	 = IRQF_DISABLED | IRQF_TIMER,
	.handler = timer_tick,
};

/*
 * timer_tick_init()
 * 	Implements a periodic timer
 *
 * This implementation directly calls the timer_tick() and move
 * the Linux kernel forward.  This is used when the user has not
 * selected GENERIC_CLOCKEVENTS.
 */
void timer_tick_init(void)
{
	/*
	 * Now allocate a timer to ourselves.
	 */
	timervector = timer_alloc();
	if (timervector == -1) {
		printk(KERN_WARNING "where did the timer go?\n");
		return;
	}

	setup_irq(timervector, &timer_irq);

	/*
	 * Get the frequency from the processor device tree node or use
	 * the default if not available. We will store this as the frequency
	 * of the timer to avoid future calculations.
	 */
	frequency = processor_frequency();
	if (frequency == 0) {
		frequency = CLOCK_TICK_RATE;
	}
	frequency /= CONFIG_HZ;

	printk(KERN_NOTICE "timer will interrupt every: %d cycles\n", frequency);
	timer_set(timervector, frequency);
}

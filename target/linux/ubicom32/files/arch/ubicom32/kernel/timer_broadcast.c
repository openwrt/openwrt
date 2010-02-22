/*
 * arch/ubicom32/kernel/timer_broadcast.c
 *   Implements a dummy clock event for each cpu.
 *
 * Copyright (C) 2008  Paul Mundt
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
 *   arch/arm
 *   arch/sh
 */
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/smp.h>
#include <linux/jiffies.h>
#include <linux/percpu.h>
#include <linux/clockchips.h>
#include <linux/irq.h>

static DEFINE_PER_CPU(struct clock_event_device, local_clockevent);

/*
 *  The broadcast trick only works when the timer will be used in a periodic mode.
 *  If the user has configured either NO_HZ or HIGH_RES_TIMERS they must have
 *  a per cpu timer.
 */
#if defined(CONFIG_NO_HZ) || defined(CONFIG_HIGH_RES_TIMERS)
#error "Tickless and High Resolution Timers require per-CPU local timers: CONFIG_LOCAL_TIMERS"
#endif

/*
 * local_timer_interrupt()
 *	Used on SMP for local timer interrupt sent via an IPI.
 */
void local_timer_interrupt(void)
{
	struct clock_event_device *dev = &__get_cpu_var(local_clockevent);

	dev->event_handler(dev);
}

/*
 * dummy_timer_set_next_event()
 *	Cause the timer to go off "cycles" from now.
 */
static int dummy_timer_set_next_event(unsigned long cycles, struct clock_event_device *dev)
{
	return 0;
}

/*
 * dummy_timer_set_mode()
 *	Do Nothing.
 */
static void dummy_timer_set_mode(enum clock_event_mode mode,
				 struct clock_event_device *clk)
{
}

/*
 * local_timer_setup()
 *	Adds a clock event for the specified cpu.
 */
int __cpuinit local_timer_setup(unsigned int cpu)
{
	struct clock_event_device *dev = &per_cpu(local_clockevent, cpu);

	dev->name		= "timer-dummy";
	dev->features		= CLOCK_EVT_FEAT_DUMMY;
	dev->rating		= 200;
	dev->mult		= 1;
	dev->set_mode		= dummy_timer_set_mode;
	dev->set_next_event	= dummy_timer_set_next_event;
	dev->broadcast		= smp_timer_broadcast;
	dev->cpumask		= cpumask_of_cpu(cpu);
	dev->irq		= -1;
	printk(KERN_NOTICE "timer[%d]: %s - created\n", dev->irq, dev->name);

	clockevents_register_device(dev);
	return 0;
}

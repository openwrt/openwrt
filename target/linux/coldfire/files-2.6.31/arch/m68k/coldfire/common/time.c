/*
 *  linux/arch/m68k/coldfire/time.c
 *
 *  This file contains the coldfire specific time handling pieces.
 *
 *  Copyright 2008-2009 Freescale Semiconductor, Inc. All Rights Reserved.
 *  Kurt Mahan <kmahan@freescale.com>
 *  Jason Jin Jason.Jin@freescale.com
 *  Shrek Wu B16972@freescale.com
 *
 *  based on linux/arch/m68k/kernel/time.c
 */
#include <linux/clk.h>
#include <linux/clk.h>
#include <linux/clocksource.h>
#include <linux/clockchips.h>
#include <linux/time.h>
#include <linux/timex.h>
#include <linux/errno.h>
#include <linux/module.h>
#include <linux/sysdev.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/param.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/rtc.h>

#include <asm/machdep.h>
#include <linux/io.h>
#include <asm/irq_regs.h>

#include <linux/profile.h>
#include <asm/mcfsim.h>
#ifdef CONFIG_GENERIC_CLOCKEVENTS
/*extern unsigned long long sys_dtim0_read(void);
extern void sys_dtim_init(void);*/
extern unsigned long long sys_dtim2_read(void);
extern void sys_dtim2_init(void);
static int cfv4_set_next_event(unsigned long evt,
	struct clock_event_device *dev);
static void cfv4_set_mode(enum clock_event_mode mode,
	struct clock_event_device *dev);
#if defined(CONFIG_M5445X)
#define FREQ    (MCF_BUSCLK / 16)
#else
#define FREQ    (MCF_BUSCLK)
#endif
/*
 * realtime clock dummy code
 */

static unsigned long null_rtc_get_time(void)
{
	return mktime(2008, 1, 1, 0, 0, 0);
}

static int null_rtc_set_time(unsigned long sec)
{
	return 0;
}

static unsigned long (*cf_rtc_get_time)(void) = null_rtc_get_time;
static int (*cf_rtc_set_time)(unsigned long) = null_rtc_set_time;
#endif /* CONFIG_GENERIC_CLOCKEVENTS */

/*
 * old pre-GENERIC clock code
 */

#ifndef CONFIG_GENERIC_CLOCKEVENTS
/*
 * timer_interrupt() needs to keep up the real-time clock,
 * as well as call the "do_timer()" routine every clocktick
 */
static irqreturn_t timer_interrupt(int irq, void *dummy)
{
#ifdef CONFIG_COLDFIRE
	/* kick hardware timer if necessary */
	if (mach_tick)
		mach_tick();
#endif
	do_timer(1);
#ifndef CONFIG_SMP
	update_process_times(user_mode(get_irq_regs()));
#endif
	profile_tick(CPU_PROFILING);

#ifdef CONFIG_HEARTBEAT
	/* use power LED as a heartbeat instead -- much more useful
	   for debugging -- based on the version for PReP by Cort */
	/* acts like an actual heart beat -- ie thump-thump-pause... */
	if (mach_heartbeat) {
		unsigned cnt = 0, period = 0, dist = 0;

		if (cnt == 0 || cnt == dist)
			mach_heartbeat(1);
		else if (cnt == 7 || cnt == dist+7)
			mach_heartbeat(0);

		if (++cnt > period) {
			cnt = 0;
			/* The hyperbolic function below modifies
			 * the heartbeat period length in dependency
			 * of the current (5min) load. It goes through
			 * the points f(0)=126, f(1)=86, f(5)=51,
			 * f(inf)->30. */
			period = ((672<<FSHIFT)/(5*avenrun[0]+(7<<FSHIFT)))
					+ 30;
			dist = period / 4;
		}
	}
#endif /* CONFIG_HEARTBEAT */
	return IRQ_HANDLED;
}

void __init time_init(void)
{
	struct rtc_time time;

	if (mach_hwclk) {
		mach_hwclk(0, &time);
		time.tm_year += 1900;
		if (time.tm_year < 1970)
			time.tm_year += 100;
		xtime.tv_sec = mktime(time.tm_year, time.tm_mon, time.tm_mday,
				      time.tm_hour, time.tm_min, time.tm_sec);
		xtime.tv_nsec = 0;
	}
	wall_to_monotonic.tv_sec = -xtime.tv_sec;

	mach_sched_init(timer_interrupt);
}
#endif /* !CONFIG_GENERIC_CLOCKEVENTS */

#ifndef CONFIG_GENERIC_TIME
/*
 * This version of gettimeofday has near microsecond resolution.
 */
void do_gettimeofday(struct timeval *tv)
{
	unsigned long flags;
	unsigned long seq;
	unsigned long usec, sec;
	unsigned long max_ntp_tick = tick_usec - tickadj;

	do {
		seq = read_seqbegin_irqsave(&xtime_lock, flags);

		usec = mach_gettimeoffset();

		/*
		 * If time_adjust is negative then NTP is slowing the clock
		 * so make sure not to go into next possible interval.
		 * Better to lose some accuracy than have time go backwards..
		 */
		if (unlikely(time_adjust < 0))
			usec = min(usec, max_ntp_tick);

		sec = xtime.tv_sec;
		usec += xtime.tv_nsec/1000;
	} while (read_seqretry_irqrestore(&xtime_lock, seq, flags));


	while (usec >= 1000000) {
		usec -= 1000000;
		sec++;
	}

	tv->tv_sec = sec;
	tv->tv_usec = usec;
}
EXPORT_SYMBOL(do_gettimeofday);

int do_settimeofday(struct timespec *tv)
{
	time_t wtm_sec, sec = tv->tv_sec;
	long wtm_nsec, nsec = tv->tv_nsec;

	if ((unsigned long)tv->tv_nsec >= NSEC_PER_SEC)
		return -EINVAL;

	write_seqlock_irq(&xtime_lock);
	/* This is revolting. We need to set the xtime.tv_nsec
	 * correctly. However, the value in this location is
	 * is value at the last tick.
	 * Discover what correction gettimeofday
	 * would have done, and then undo it!
	 */
	nsec -= 1000 * mach_gettimeoffset();

	wtm_sec  = wall_to_monotonic.tv_sec + (xtime.tv_sec - sec);
	wtm_nsec = wall_to_monotonic.tv_nsec + (xtime.tv_nsec - nsec);

	set_normalized_timespec(&xtime, sec, nsec);
	set_normalized_timespec(&wall_to_monotonic, wtm_sec, wtm_nsec);

	ntp_clear();
	write_sequnlock_irq(&xtime_lock);
	clock_was_set();
	return 0;
}
EXPORT_SYMBOL(do_settimeofday);
#endif /* !CONFIG_GENERIC_TIME */

#ifdef CONFIG_GENERIC_CLOCKEVENTS
/*
 * Clock Evnt setup
 */
static struct clock_event_device clockevent_cfv4 = {
	.name           = "CFV4 timer2even",
	.features       = CLOCK_EVT_FEAT_PERIODIC | CLOCK_EVT_FEAT_ONESHOT,
	.rating         = 200,
	.shift          = 20,
	.set_mode       = cfv4_set_mode,
	.set_next_event = cfv4_set_next_event,
};

static int cfv4_set_next_event(unsigned long evt,
	struct clock_event_device *dev)
{
	return 0;
}

static void cfv4_set_mode(enum clock_event_mode mode,
	struct clock_event_device *dev)
{
	if (mode != CLOCK_EVT_MODE_ONESHOT)
		cfv4_set_next_event((FREQ / HZ), dev);
}

static int __init cfv4_clockevent_init(void)
{
	clockevent_cfv4.mult =
			div_sc(FREQ, NSEC_PER_SEC,
			clockevent_cfv4.shift);
	clockevent_cfv4.max_delta_ns =
		clockevent_delta2ns((FREQ / HZ),
			&clockevent_cfv4);
	clockevent_cfv4.min_delta_ns =
		clockevent_delta2ns(1, &clockevent_cfv4);

	clockevent_cfv4.cpumask = &cpumask_of_cpu(0);

	printk(KERN_INFO "timer: register clockevent\n");
		clockevents_register_device(&clockevent_cfv4);

	return 0;
}

/*
 * clocksource setup
 */

struct clocksource clocksource_cfv4 = {
	.name	= "ColdfireV4",
	.rating	= 250,
	.mask	= CLOCKSOURCE_MASK(32),
	.read	= sys_dtim2_read,
	.shift	= 20,
	.flags	= CLOCK_SOURCE_IS_CONTINUOUS,
};

/*
 * Initialize time subsystem.  Called from linux/init/main.c
 */
void __init time_init(void)
{
	int ret;

	printk(KERN_INFO "Initializing time\n");
#if 0
	/* initialize system clocks */
	clk_init();
#endif
	cfv4_clockevent_init();
	/* initialize the system timer */
	/*sys_dtim_init();*/
	sys_dtim2_init();
	/* setup initial system time */
	xtime.tv_sec = cf_rtc_get_time();
	xtime.tv_nsec = 0;
	set_normalized_timespec(&wall_to_monotonic, -xtime.tv_sec,
				-xtime.tv_nsec);

	/* JKM */
	clocksource_cfv4.mult = clocksource_hz2mult(FREQ,
			clocksource_cfv4.shift);

	/* register our clocksource */
	ret = clocksource_register(&clocksource_cfv4);
	if (ret)
		printk(KERN_ERR "timer: unable to "
			"register clocksource - %d\n", ret);
}

/*
 * sysfs pieces
 */

static struct sysdev_class timer_class = {
	.name	= "timer",
};

static struct sys_device timer_device = {
	.id	= 0,
	.cls	= &timer_class,
};

static int __init timer_init_sysfs(void)
{
	int err = sysdev_class_register(&timer_class);
	if (!err)
		err = sysdev_register(&timer_device);
	return err;
}
device_initcall(timer_init_sysfs);
#endif /* CONFIG_GENERIC_CLOCKEVENTS */

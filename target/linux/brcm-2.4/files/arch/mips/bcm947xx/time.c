/*
 * Copyright 2006, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 */
#include <linux/config.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/serial_reg.h>
#include <linux/interrupt.h>
#include <asm/addrspace.h>
#include <asm/io.h>
#include <asm/time.h>

#include <typedefs.h>
#include <osl.h>
#include <bcmnvram.h>
#include <sbconfig.h>
#include <sbutils.h>
#include <sbchipc.h>
#include <hndmips.h>
#include <mipsinc.h>
#include <hndcpu.h>
#include <bcmdevs.h>

/* Global SB handle */
extern void *bcm947xx_sbh;
extern spinlock_t bcm947xx_sbh_lock;

/* Convenience */
#define sbh bcm947xx_sbh
#define sbh_lock bcm947xx_sbh_lock

extern int panic_timeout;
static int watchdog = 0;

void __init
bcm947xx_time_init(void)
{
	unsigned int hz;

	/*
	 * Use deterministic values for initial counter interrupt
	 * so that calibrate delay avoids encountering a counter wrap.
	 */
	write_c0_count(0);
	write_c0_compare(0xffff);

	if (!(hz = sb_cpu_clock(sbh)))
		hz = 100000000;

	printk("CPU: BCM%04x rev %d at %d MHz\n", sb_chip(sbh), sb_chiprev(sbh),
	       (hz + 500000) / 1000000);

	/* Set MIPS counter frequency for fixed_rate_gettimeoffset() */
	mips_hpt_frequency = hz / 2;

	/* Set watchdog interval in ms */
	watchdog = simple_strtoul(nvram_safe_get("watchdog"), NULL, 0);

	/* Please set the watchdog to 3 sec if it is less than 3 but not equal to 0 */
	if (watchdog > 0) {
		if (watchdog < 3000)
			watchdog = 3000;
	}

	/* Set panic timeout in seconds */
	panic_timeout = watchdog / 1000;
}

static void
bcm947xx_timer_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
	/* Generic MIPS timer code */
	timer_interrupt(irq, dev_id, regs);

	/* Set the watchdog timer to reset after the specified number of ms */
	if (watchdog > 0) {
		if (sb_chip(sbh) == BCM5354_CHIP_ID)
			sb_watchdog(sbh, WATCHDOG_CLOCK_5354 / 1000 * watchdog);
		else
			sb_watchdog(sbh, WATCHDOG_CLOCK / 1000 * watchdog);
	}
	
}

static struct irqaction bcm947xx_timer_irqaction = {
	bcm947xx_timer_interrupt,
	SA_INTERRUPT,
	0,
	"timer",
	NULL,
	NULL
};

void __init
bcm947xx_timer_setup(struct irqaction *irq)
{
	int x;

	/* Enable the timer interrupt */
	setup_irq(7, &bcm947xx_timer_irqaction);

	sti();

	for (x=0; x<5; x++) {
		unsigned long ticks; 
		ticks = jiffies;
		while (ticks == jiffies) 
			/* do nothing */;
	}
}

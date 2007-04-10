/*
<:copyright-gpl
 Copyright 2004 Broadcom Corp. All Rights Reserved.

 This program is free software; you can distribute it and/or modify it
 under the terms of the GNU General Public License (Version 2) as
 published by the Free Software Foundation.

 This program is distributed in the hope it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 for more details.

 You should have received a copy of the GNU General Public License along
 with this program; if not, write to the Free Software Foundation, Inc.,
 59 Temple Place - Suite 330, Boston MA 02111-1307, USA.
:>
*/
/*
 * Setup time for Broadcom 963xx MIPS boards
 */

#include <linux/autoconf.h>
#include <linux/init.h>
#include <linux/kernel_stat.h>
#include <linux/sched.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/time.h>
#include <linux/timex.h>

#include <asm/mipsregs.h>
#include <asm/ptrace.h>
#include <asm/div64.h>
#include <asm/time.h>

#include <6348_map_part.h>
#include <6348_intr.h>
#include <bcm_map_part.h>
#include <bcm_intr.h>

static unsigned long r4k_offset;	/* Amount to increment compare reg each time */
static unsigned long r4k_cur;		/* What counter should be at next timer irq */

/*  *********************************************************************
    *  calculateCpuSpeed()
    *      Calculate the BCM6348 CPU speed by reading the PLL strap register
    *      and applying the following formula:
    *      cpu_clk = (.25 * 64MHz freq) * (N1 + 1) * (N2 + 2) / (M1_CPU + 1)
    *  Input parameters:
    *      none
    *  Return value:
    *      none
    ********************************************************************* */

static inline unsigned long __init calculateCpuSpeed(void)
{
    u32 pllStrap = PERF->PllStrap;
    int n1 = (pllStrap & PLL_N1_MASK) >> PLL_N1_SHFT;
    int n2 = (pllStrap & PLL_N2_MASK) >> PLL_N2_SHFT;
    int m1cpu = (pllStrap & PLL_M1_CPU_MASK) >> PLL_M1_CPU_SHFT;

	return (16 * (n1 + 1) * (n2 + 2) / (m1cpu + 1)) * 1000000;
}


static inline unsigned long __init cal_r4koff(void)
{   
	mips_hpt_frequency = calculateCpuSpeed() / 2;
	return (mips_hpt_frequency / HZ);
}


/*
 * There are a lot of conceptually broken versions of the MIPS timer interrupt
 * handler floating around.  This one is rather different, but the algorithm
 * is provably more robust.
 */
irqreturn_t brcm_timer_interrupt(struct pt_regs *regs)
{
	int irq = MIPS_TIMER_INT;

	irq_enter();
	kstat_this_cpu.irqs[irq]++;

	timer_interrupt(irq, regs);
	irq_exit();
	return IRQ_HANDLED;
}


void __init brcm_time_init(void)
{
	unsigned int est_freq, flags;
	local_irq_save(flags);

	printk("calculating r4koff... ");
	r4k_offset = cal_r4koff();
	printk("%08lx(%d)\n", r4k_offset, (int)r4k_offset);

	est_freq = 2 * r4k_offset * HZ;
	est_freq += 5000;   /* round */
	est_freq -= est_freq % 10000;
	printk("CPU frequency %d.%02d MHz\n", est_freq / 1000000,
		   (est_freq % 1000000) * 100 / 1000000);
	local_irq_restore(flags);
}


void __init plat_timer_setup(struct irqaction *irq)
{
	r4k_cur = (read_c0_count() + r4k_offset);
	write_c0_compare(r4k_cur);
	set_c0_status(IE_IRQ5);
}

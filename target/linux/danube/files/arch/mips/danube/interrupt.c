/*
 *   arch/mips/danube/interrupt.c
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 *
 *   Copyright (C) 2005 Wu Qi Ming infineon
 *
 *   Rewrite of Infineon Danube code, thanks to infineon for the support,
 *   software and hardware
 *
 *   Copyright (C) 2007 John Crispin <blogic@openwrt.org> 
 *
 */

#include <linux/init.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/kernel_stat.h>
#include <linux/module.h>

#include <asm/bootinfo.h>
#include <asm/irq.h>
#include <asm/danube/danube.h>
#include <asm/danube/danube_irq.h>
#include <asm/irq_cpu.h>


void
disable_danube_irq (unsigned int irq_nr)
{
	int i;
	u32 *danube_ier = DANUBE_ICU_IM0_IER;

	irq_nr -= INT_NUM_IRQ0;
	for (i = 0; i <= 4; i++)
	{
		if (irq_nr < INT_NUM_IM_OFFSET){
			writel(readl(danube_ier) & ~(1 << irq_nr ), danube_ier);
			return;
		}
		danube_ier += DANUBE_ICU_OFFSET;
		irq_nr -= INT_NUM_IM_OFFSET;
	}
}
EXPORT_SYMBOL (disable_danube_irq);

void
mask_and_ack_danube_irq (unsigned int irq_nr)
{
	int i;
	u32 *danube_ier = DANUBE_ICU_IM0_IER;
	u32 *danube_isr = DANUBE_ICU_IM0_ISR;

	irq_nr -= INT_NUM_IRQ0;
	for (i = 0; i <= 4; i++)
	{
		if (irq_nr < INT_NUM_IM_OFFSET)
		{
			writel(readl(danube_ier) & ~(1 << irq_nr ), danube_ier);
			writel((1 << irq_nr ), danube_isr);
			return;
		}
		danube_ier += DANUBE_ICU_OFFSET;
		danube_isr += DANUBE_ICU_OFFSET;
		irq_nr -= INT_NUM_IM_OFFSET;
	}
}
EXPORT_SYMBOL (mask_and_ack_danube_irq);

void
enable_danube_irq (unsigned int irq_nr)
{
	int i;
	u32 *danube_ier = DANUBE_ICU_IM0_IER;

	irq_nr -= INT_NUM_IRQ0;
	for (i = 0; i <= 4; i++)
	{
		if (irq_nr < INT_NUM_IM_OFFSET)
		{
			writel(readl(danube_ier) | (1 << irq_nr ), danube_ier);
			return;
		}
		danube_ier += DANUBE_ICU_OFFSET;
		irq_nr -= INT_NUM_IM_OFFSET;
	}
}
EXPORT_SYMBOL (enable_danube_irq);

static unsigned int
startup_danube_irq (unsigned int irq)
{
	enable_danube_irq (irq);
	return 0;
}

static void
end_danube_irq (unsigned int irq)
{
	if (!(irq_desc[irq].status & (IRQ_DISABLED | IRQ_INPROGRESS)))
		enable_danube_irq (irq);
}

static struct hw_interrupt_type danube_irq_type = {
	"DANUBE",
	.startup = startup_danube_irq,
	.enable = enable_danube_irq,
	.disable = disable_danube_irq,
	.unmask = enable_danube_irq,
	.ack = end_danube_irq,
	.mask = disable_danube_irq,
	.mask_ack = mask_and_ack_danube_irq,
	.end = end_danube_irq,
};

static inline int
ls1bit32(unsigned long x)
{
	__asm__ (
		"       .set    push                                    \n"
		"       .set    mips32                                  \n"
		"       clz     %0, %1                                  \n"
		"       .set    pop                                     \n"
		: "=r" (x)
		: "r" (x));

		return 31 - x;
}

void
danube_hw_irqdispatch (int module)
{
	u32 irq;

	irq = readl(DANUBE_ICU_IM0_IOSR + (module * DANUBE_ICU_OFFSET));
	if (irq == 0)
		return;

	irq = ls1bit32 (irq);
	do_IRQ ((int) irq + INT_NUM_IM0_IRL0 + (INT_NUM_IM_OFFSET * module));

	if ((irq == 22) && (module == 0)){
		writel(readl(DANUBE_EBU_PCC_ISTAT) | 0x10, DANUBE_EBU_PCC_ISTAT);
	}
}

asmlinkage void
plat_irq_dispatch (void)
{
	unsigned int pending = read_c0_status() & read_c0_cause() & ST0_IM;
	unsigned int i;

	if (pending & CAUSEF_IP7){
		do_IRQ(MIPS_CPU_TIMER_IRQ);
		goto out;
	} else {
		for (i = 0; i < 5; i++)
		{
			if (pending & (CAUSEF_IP2 << i))
			{
				danube_hw_irqdispatch(i);
				goto out;
			}
		}
	}
	printk("Spurious IRQ: CAUSE=0x%08x\n", read_c0_status());

out:
	return;
}

static struct irqaction cascade = {
	.handler = no_action,
	.flags = IRQF_DISABLED,
	.name = "cascade",
};

void __init
arch_init_irq(void)
{
	int i;

	for (i = 0; i < 5; i++)
	{
		writel(0, DANUBE_ICU_IM0_IER + (i * DANUBE_ICU_OFFSET));
	}

	mips_cpu_irq_init();

	for (i = 2; i <= 6; i++)
	{
		setup_irq(i, &cascade);
	}

	for (i = INT_NUM_IRQ0; i <= (INT_NUM_IRQ0 + (5 * INT_NUM_IM_OFFSET)); i++)
	{
#if 0
		irq_desc[i].status = IRQ_DISABLED;
		irq_desc[i].action = NULL;
		irq_desc[i].depth = 1;
#endif
		set_irq_chip_and_handler(i, &danube_irq_type, handle_level_irq);
	}

	set_c0_status (IE_IRQ0 | IE_IRQ1 | IE_IRQ2 | IE_IRQ3 | IE_IRQ4 | IE_IRQ5);
}

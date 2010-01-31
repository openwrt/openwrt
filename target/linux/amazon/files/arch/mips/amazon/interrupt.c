/*
 *  Gary Jennejohn (C) 2003 <gj@denx.de>
 *  Copyright (C) 2007 Felix Fietkau <nbd@openwrt.org>
 *  Copyright (C) 2007 John Crispin <blogic@openwrt.org>
 *
 *  This program is free software; you can distribute it and/or modify it
 *  under the terms of the GNU General Public License (Version 2) as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 *  for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place - Suite 330, Boston MA 02111-1307, USA.
 *
 * Routines for generic manipulation of the interrupts found on the 
 * AMAZON boards.
 */

#include <linux/init.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/kernel_stat.h>
#include <linux/module.h>

#include <asm/amazon/amazon.h>
#include <asm/amazon/irq.h>
#include <asm/bootinfo.h>
#include <asm/irq_cpu.h>
#include <asm/irq.h>
#include <asm/time.h>

static void amazon_disable_irq(unsigned int irq_nr)
{
	int i;
	u32 amazon_ier = AMAZON_ICU_IM0_IER;

	if (irq_nr <= INT_NUM_IM0_IRL11 && irq_nr >= INT_NUM_IM0_IRL0)
		amazon_writel(amazon_readl(amazon_ier) & (~(AMAZON_DMA_H_MASK)), amazon_ier);
	else {
		irq_nr -= INT_NUM_IRQ0;
		for (i = 0; i <= 4; i++)
		{
			if (irq_nr <= 31) 
	        	amazon_writel(amazon_readl(amazon_ier) & ~(1 << irq_nr ), amazon_ier);
			amazon_ier += 0x10;
			irq_nr -= 32;
		}
	}	
}

static void amazon_mask_and_ack_irq(unsigned int irq_nr)
{
	int i;
	u32 amazon_ier = AMAZON_ICU_IM0_IER;
	u32 amazon_isr = AMAZON_ICU_IM0_ISR;

	if (irq_nr <= INT_NUM_IM0_IRL11 && irq_nr >= INT_NUM_IM0_IRL0){
		amazon_writel(amazon_readl(amazon_ier) & (~(AMAZON_DMA_H_MASK)), amazon_ier);
		amazon_writel(AMAZON_DMA_H_MASK, amazon_isr); 
	} else {
		irq_nr -= INT_NUM_IRQ0;
		for (i = 0; i <= 4; i++)
		{
			if (irq_nr <= 31){ 
	        	amazon_writel(amazon_readl(amazon_ier) & ~(1 << irq_nr ), amazon_ier);
				amazon_writel((1 << irq_nr ), amazon_isr);
			}
			amazon_ier += 0x10;
			amazon_isr += 0x10;
			irq_nr -= 32;
		}
	}
}

static void amazon_enable_irq(unsigned int irq_nr)
{
	int i;
	u32 amazon_ier = AMAZON_ICU_IM0_IER;

	if (irq_nr <= INT_NUM_IM0_IRL11 && irq_nr >= INT_NUM_IM0_IRL0)
		amazon_writel(amazon_readl(amazon_ier) | AMAZON_DMA_H_MASK, amazon_ier);
	else {
		irq_nr -= INT_NUM_IRQ0;
		for (i = 0; i <= 4; i++)
		{
			if (irq_nr <= 31)
				amazon_writel(amazon_readl(amazon_ier) | (1 << irq_nr ), amazon_ier);
			amazon_ier += 0x10;
			irq_nr -= 32;
		}
	}
}

static unsigned int amazon_startup_irq(unsigned int irq)
{
	amazon_enable_irq(irq);
	return 0;
}

static void amazon_end_irq(unsigned int irq)
{
	if (!(irq_desc[irq].status & (IRQ_DISABLED|IRQ_INPROGRESS))) {
		amazon_enable_irq(irq);
	}
}

static struct irq_chip amazon_irq_type = {
	.name = "AMAZON",
	.startup = amazon_startup_irq,
	.enable = amazon_enable_irq,
	.disable = amazon_disable_irq,
	.unmask = amazon_enable_irq,
	.ack = amazon_mask_and_ack_irq,
	.mask = amazon_disable_irq,
	.mask_ack = amazon_mask_and_ack_irq,
	.end = amazon_end_irq
};

/* Cascaded interrupts from IM0-4 */
static inline void amazon_hw_irqdispatch(u8 line)
{
	u32 irq;

	irq = (amazon_readl(AMAZON_ICU_IM_VEC) >> (line * 5)) & AMAZON_ICU_IM0_VEC_MASK;
	if (line == 0 && irq <= 11 && irq >= 0) {
		//DMA fixed to IM0_IRL0
		irq = 0;
	}
	do_IRQ(irq + INT_NUM_IRQ0 + (line * 32));
}

asmlinkage void plat_irq_dispatch(void)
{
	unsigned int pending = read_c0_status() & read_c0_cause() & ST0_IM;
	if (pending & CAUSEF_IP7){
		do_IRQ(MIPS_CPU_TIMER_IRQ);
		goto out;
	} else {
		unsigned int i;
		for (i = 0; i <= 4; i++)
		{
			if(pending & (CAUSEF_IP2 << i)){
				amazon_hw_irqdispatch(i);
				goto out;
			}
		}
	}
	printk("Spurious IRQ: CAUSE=0x%08x\n", read_c0_status());
out:
	return;
}

static struct irqaction cascade = {
	.handler	= no_action,
	.flags  	= IRQF_DISABLED,
	.name   	= "cascade",
};

void __init arch_init_irq(void)
{
	int i;

	/* mask all interrupt sources */
	for(i = 0; i <= 4; i++){
		amazon_writel(0, AMAZON_ICU_IM0_IER + (i * 0x10));
	}

	mips_cpu_irq_init();

	/* set up irq cascade */
	for (i = 2; i <= 6; i++) {
		setup_irq(i, &cascade);
	}

	for (i = INT_NUM_IRQ0; i <= INT_NUM_IM4_IRL31; i++)
		set_irq_chip_and_handler(i, &amazon_irq_type,
			handle_level_irq);

	set_c0_status(IE_IRQ0 | IE_IRQ1 | IE_IRQ2 | IE_IRQ3 | IE_IRQ4 | IE_IRQ5);
}

void __cpuinit arch_fixup_c0_irqs(void)
{
	/* FIXME: check for CPUID and only do fix for specific chips/versions */
	cp0_compare_irq = CP0_LEGACY_COMPARE_IRQ;
	cp0_perfcount_irq = CP0_LEGACY_PERFCNT_IRQ;
}

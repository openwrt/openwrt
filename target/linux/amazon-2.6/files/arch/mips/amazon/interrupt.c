/*
 *  Gary Jennejohn (C) 2003 <gj@denx.de>
 *  Copyright (C) 2007 Felix Fietkau <nbd@openwrt.org>
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
	/* have to access the correct register here */
	if (irq_nr <= INT_NUM_IM0_IRL11 && irq_nr >= INT_NUM_IM0_IRL0)
		/* access IM0 DMA channels */
		*AMAZON_ICU_IM0_IER &= (~(AMAZON_DMA_H_MASK));
	else if (irq_nr <= INT_NUM_IM0_IRL31 && irq_nr >= INT_NUM_IM0_IRL12)
		/* access IM0 except DMA*/
		*AMAZON_ICU_IM0_IER &= (~AMAZON_ICU_IM0_IER_IR(irq_nr));
	else if (irq_nr <= INT_NUM_IM1_IRL31 && irq_nr >= INT_NUM_IM1_IRL0)
		/* access IM1 */
		*AMAZON_ICU_IM1_IER &= (~AMAZON_ICU_IM1_IER_IR(irq_nr - INT_NUM_IM1_IRL0));
	else if (irq_nr <= INT_NUM_IM2_IRL31 && irq_nr >= INT_NUM_IM2_IRL0)
		/* access IM2 */
		*AMAZON_ICU_IM2_IER &= (~AMAZON_ICU_IM2_IER_IR(irq_nr - INT_NUM_IM2_IRL0));
	else if (irq_nr <= INT_NUM_IM3_IRL31 && irq_nr >= INT_NUM_IM3_IRL0)
		/* access IM3 */
		*AMAZON_ICU_IM3_IER &= (~AMAZON_ICU_IM3_IER_IR((irq_nr - INT_NUM_IM3_IRL0)));
	else if (irq_nr <= INT_NUM_IM4_IRL31 && irq_nr >= INT_NUM_IM4_IRL0)
		/* access IM4 */
		*AMAZON_ICU_IM4_IER &= (~AMAZON_ICU_IM4_IER_IR((irq_nr - INT_NUM_IM4_IRL0)));
}

static void amazon_mask_and_ack_irq(unsigned int irq_nr)
{
	/* have to access the correct register here */
	if (irq_nr <= INT_NUM_IM0_IRL11 && irq_nr >= INT_NUM_IM0_IRL0) {
		/* access IM0 DMA channels */
		*AMAZON_ICU_IM0_IER &= (~(AMAZON_DMA_H_MASK)); /* mask */
		*AMAZON_ICU_IM0_ISR = AMAZON_DMA_H_MASK; /* ack */
	} else if (irq_nr <= INT_NUM_IM0_IRL31 && irq_nr >= INT_NUM_IM0_IRL12) {
		/* access IM0 except DMA */
		*AMAZON_ICU_IM0_IER &= ~AMAZON_ICU_IM0_IER_IR(irq_nr - INT_NUM_IM0_IRL0); /* mask */
		*AMAZON_ICU_IM0_ISR = AMAZON_ICU_IM0_ISR_IR(irq_nr - INT_NUM_IM0_IRL0); /* ack */
	} else if (irq_nr <= INT_NUM_IM1_IRL31 && irq_nr >= INT_NUM_IM1_IRL0) {
		/* access IM1 */
		*AMAZON_ICU_IM1_IER &= ~AMAZON_ICU_IM1_IER_IR(irq_nr - INT_NUM_IM1_IRL0); /* mask */
		*AMAZON_ICU_IM1_ISR = AMAZON_ICU_IM1_ISR_IR(irq_nr - INT_NUM_IM1_IRL0); /* ack */
	} else if (irq_nr <= INT_NUM_IM2_IRL31 && irq_nr >= INT_NUM_IM2_IRL0) {
		/* access IM2 */
		*AMAZON_ICU_IM2_IER &= ~AMAZON_ICU_IM2_IER_IR(irq_nr - INT_NUM_IM2_IRL0); /* mask */
		*AMAZON_ICU_IM2_ISR = AMAZON_ICU_IM2_ISR_IR(irq_nr - INT_NUM_IM2_IRL0); /* ack */
	} else if (irq_nr <= INT_NUM_IM3_IRL31 && irq_nr >= INT_NUM_IM3_IRL0) {
		/* access IM3 */
		*AMAZON_ICU_IM3_IER &= ~AMAZON_ICU_IM3_IER_IR(irq_nr - INT_NUM_IM3_IRL0); /* mask */
		*AMAZON_ICU_IM3_ISR = AMAZON_ICU_IM3_ISR_IR(irq_nr - INT_NUM_IM3_IRL0); /* ack */
	} else if (irq_nr <= INT_NUM_IM4_IRL31 && irq_nr >= INT_NUM_IM4_IRL0) {
		*AMAZON_ICU_IM4_IER &= ~AMAZON_ICU_IM4_IER_IR(irq_nr - INT_NUM_IM4_IRL0); /* mask */
		*AMAZON_ICU_IM4_ISR = AMAZON_ICU_IM4_ISR_IR(irq_nr - INT_NUM_IM4_IRL0); /* ack */
	}
}

static void amazon_enable_irq(unsigned int irq_nr)
{
	/* have to access the correct register here */
	if (irq_nr <= INT_NUM_IM0_IRL11 && irq_nr >= INT_NUM_IM0_IRL0)
		/* access IM0 DMA*/
		*AMAZON_ICU_IM0_IER |= AMAZON_DMA_H_MASK;
	else if (irq_nr <= INT_NUM_IM0_IRL31 && irq_nr >= INT_NUM_IM0_IRL12)
		/* access IM0 except DMA*/
		*AMAZON_ICU_IM0_IER |= AMAZON_ICU_IM0_IER_IR(irq_nr - INT_NUM_IM0_IRL0);
	else if (irq_nr <= INT_NUM_IM1_IRL31 && irq_nr >= INT_NUM_IM1_IRL0)
		/* access IM1 */
		*AMAZON_ICU_IM1_IER |= AMAZON_ICU_IM1_IER_IR(irq_nr - INT_NUM_IM1_IRL0);
	else if (irq_nr <= INT_NUM_IM2_IRL31 && irq_nr >= INT_NUM_IM2_IRL0)
		/* access IM2 */
		*AMAZON_ICU_IM2_IER |= AMAZON_ICU_IM2_IER_IR(irq_nr - INT_NUM_IM2_IRL0);
	else if (irq_nr <= INT_NUM_IM3_IRL31 && irq_nr >= INT_NUM_IM3_IRL0)
		/* access IM3 */
		*AMAZON_ICU_IM3_IER |= AMAZON_ICU_IM3_IER_IR((irq_nr - INT_NUM_IM3_IRL0));
	else if (irq_nr <= INT_NUM_IM4_IRL31 && irq_nr >= INT_NUM_IM4_IRL0)
		/* access IM4 */
		*AMAZON_ICU_IM4_IER |= AMAZON_ICU_IM4_IER_IR((irq_nr - INT_NUM_IM4_IRL0));
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

static struct hw_interrupt_type amazon_irq_type = {
	"AMAZON",
	.startup = amazon_startup_irq,
	.enable = amazon_enable_irq,
	.disable = amazon_disable_irq,
	.unmask = amazon_enable_irq,
	.ack = amazon_mask_and_ack_irq,
	.mask = amazon_disable_irq,
	.mask_ack = amazon_mask_and_ack_irq,
	.end = amazon_end_irq
};

/* Cascaded interrupts from IM0 */
static inline void amazon_hw0_irqdispatch(void)
{
	u32 irq;
																								 
	irq = (*AMAZON_ICU_IM_VEC) & AMAZON_ICU_IM0_VEC_MASK;
	if (irq <= 11 && irq >= 0) {
		//DMA fixed to IM0_IRL0
		irq = 0;
	}
	do_IRQ(irq + INT_NUM_IM0_IRL0);
}
                                                                                                         
/* Cascaded interrupts from IM1 */
static inline void amazon_hw1_irqdispatch(void)
{
	u32 irq;

	irq = ((*AMAZON_ICU_IM_VEC) & AMAZON_ICU_IM1_VEC_MASK) >> 5;
	do_IRQ(irq + INT_NUM_IM1_IRL0);
}

/* Cascaded interrupts from IM2 */
static inline void amazon_hw2_irqdispatch(void)
{
	u32 irq;
																								 
	irq = ((*AMAZON_ICU_IM_VEC) & AMAZON_ICU_IM2_VEC_MASK) >> 10;
	do_IRQ(irq + INT_NUM_IM2_IRL0);
}

/* Cascaded interrupts from IM3 */
static inline void amazon_hw3_irqdispatch(void)
{
	u32 irq;

	irq = ((*AMAZON_ICU_IM_VEC) & AMAZON_ICU_IM3_VEC_MASK) >> 15;
	do_IRQ(irq + INT_NUM_IM3_IRL0);
}

/* Cascaded interrupts from IM4 */
static inline void amazon_hw4_irqdispatch(void)
{
	u32 irq;

	irq = ((*AMAZON_ICU_IM_VEC) & AMAZON_ICU_IM4_VEC_MASK) >> 20;
	do_IRQ(irq + INT_NUM_IM4_IRL0);
}

asmlinkage void plat_irq_dispatch(void)
{
	unsigned int pending = read_c0_status() & read_c0_cause() & ST0_IM;
	if (pending & CAUSEF_IP7){
		do_IRQ(MIPS_CPU_TIMER_IRQ);
	}
	else if (pending & CAUSEF_IP2)
		amazon_hw0_irqdispatch();
	else if (pending & CAUSEF_IP3)
		amazon_hw1_irqdispatch();
	else if (pending & CAUSEF_IP4)
		amazon_hw2_irqdispatch();
	else if (pending & CAUSEF_IP5)
		amazon_hw3_irqdispatch();
	else if (pending & CAUSEF_IP6)
		amazon_hw4_irqdispatch();
	else
		printk("Spurious IRQ: CAUSE=0x%08x\n", read_c0_status());
}

static struct irqaction cascade = {
	.handler	= no_action,
	.flags  	= SA_INTERRUPT,
	.name   	= "cascade",
};


/* Function for careful CP0 interrupt mask access */

void __init arch_init_irq(void)
{
	int i;

	/* mask all interrupt sources */
	*AMAZON_ICU_IM0_IER = 0;
	*AMAZON_ICU_IM1_IER = 0;
	*AMAZON_ICU_IM2_IER = 0;
	*AMAZON_ICU_IM3_IER = 0;
	*AMAZON_ICU_IM4_IER = 0;

	mips_cpu_irq_init();

	/* set up irq cascade */
	for (i = 2; i <= 6; i++) {
		setup_irq(i, &cascade);
	}

	for (i = INT_NUM_IRQ0; i <= INT_NUM_IM4_IRL31; i++) {
		irq_desc[i].status	= IRQ_DISABLED;
		irq_desc[i].action	= 0;
		irq_desc[i].depth	= 1;
		set_irq_chip(i, &amazon_irq_type);
	}
}

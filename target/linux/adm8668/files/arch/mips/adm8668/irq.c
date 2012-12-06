/*
 * Copyright (C) 2010 Scott Nicholas <neutronscott@scottn.us>
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 */

#include <linux/init.h>
#include <linux/kernel_stat.h>
#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/random.h>
#include <linux/pm.h>
#include <linux/irq.h>
#include <asm/mipsregs.h>
#include <asm/irq_cpu.h>
#include <asm/irq.h>
#include <adm8668.h>

static void adm8668_irq_cascade(void)
{
	int i;
	unsigned long intsrc;

	intsrc = ADM8668_INTC_REG(IRQ_STATUS_REG) & IRQ_MASK;
	for (i = 0; intsrc; intsrc >>= 1, i++)
		if (intsrc & 0x1)
			do_IRQ(i);
}

/*
 * System irq dispatch
 */
void plat_irq_dispatch(void)
{
	unsigned int pending;

	pending = read_c0_cause() & read_c0_status() & ST0_IM;

	/* timer interrupt, that we renumbered */
	if (pending & STATUSF_IP7)
		do_IRQ(MIPS_CPU_IRQ_BASE + 7);
	if (pending & STATUSF_IP2)
		adm8668_irq_cascade();
}

/*
 * enable 8668 irq
 */
static void enable_adm8668_irq(struct irq_data *d)
{
	ADM8668_INTC_REG(IRQ_ENABLE_REG) = (1 << d->irq);
}


static void ack_adm8668_irq(struct irq_data *d)
{
	ADM8668_INTC_REG(IRQ_DISABLE_REG) = (1 << d->irq);
}

/*
 * system irq type
 */

static struct irq_chip adm8668_irq_type = {
	.name = "adm8668",
	.irq_ack = ack_adm8668_irq,
	.irq_mask = ack_adm8668_irq,
	.irq_unmask = enable_adm8668_irq
};

/*
 * irq init
 */
static void __init init_adm8668_irqs(void)
{
	int i;

	for (i = 0; i <= INT_LVL_MAX; i++)
		irq_set_chip_and_handler(i, &adm8668_irq_type,
			handle_level_irq);

	/* hw0 is where our interrupts are uh.. interrupted at. */
	set_c0_status(IE_IRQ0);
}

/*
 * system init
 */
void __init arch_init_irq(void)
{
	mips_cpu_irq_init();
	init_adm8668_irqs();
}

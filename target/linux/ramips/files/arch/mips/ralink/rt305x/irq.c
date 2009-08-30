/*
 *  Ralink RT305x SoC specific interrupt handling
 *
 *  Copyright (C) 2009 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/irq.h>

#include <asm/irq_cpu.h>
#include <asm/mipsregs.h>

#include <asm/mach-ralink/rt305x.h>
#include <asm/mach-ralink/rt305x_regs.h>

static void rt305x_intc_irq_dispatch(void)
{
	u32 pending;

	pending = rt305x_intc_rr(INTC_REG_STATUS0);

	if (pending & RT305X_INTC_INT_TIMER0)
		do_IRQ(RT305X_INTC_IRQ_TIMER0);

	else if (pending & RT305X_INTC_INT_TIMER1)
		do_IRQ(RT305X_INTC_IRQ_TIMER1);

	else if (pending & RT305X_INTC_INT_UART0)
		do_IRQ(RT305X_INTC_IRQ_UART0);

	else if (pending & RT305X_INTC_INT_UART1)
		do_IRQ(RT305X_INTC_IRQ_UART1);

	/* TODO: handle PIO interrupts as well */

	else
		spurious_interrupt();
}

static void rt305x_intc_irq_unmask(unsigned int irq)
{
	irq -= RT305X_INTC_IRQ_BASE;
	rt305x_intc_wr((1 << irq), INTC_REG_ENABLE);
}

static void rt305x_intc_irq_mask(unsigned int irq)
{
	irq -= RT305X_INTC_IRQ_BASE;
	rt305x_intc_wr((1 << irq), INTC_REG_DISABLE);
}

struct irq_chip rt305x_intc_irq_chip = {
	.name		= "RT305X INTC",
	.unmask		= rt305x_intc_irq_unmask,
	.mask		= rt305x_intc_irq_mask,
	.mask_ack	= rt305x_intc_irq_mask,
};

static struct irqaction rt305x_intc_irqaction = {
	.handler	= no_action,
	.name		= "cascade [RT305X INTC]",
};

static void __init rt305x_intc_irq_init(void)
{
	int i;

	/* disable all interrupts */
	rt305x_intc_wr(~0, INTC_REG_DISABLE);

	/* route all INTC interrupts to MIPS HW0 interrupt */
	rt305x_intc_wr(0, INTC_REG_TYPE);

	for (i = RT305X_INTC_IRQ_BASE;
	     i < RT305X_INTC_IRQ_BASE + RT305X_INTC_IRQ_COUNT; i++) {
		set_irq_chip_and_handler(i, &rt305x_intc_irq_chip,
					 handle_level_irq);
	}

	setup_irq(RT305X_CPU_IRQ_INTC, &rt305x_intc_irqaction);

	/* enable interrupt masking */
	rt305x_intc_wr(RT305X_INTC_INT_GLOBAL, INTC_REG_ENABLE);
}

asmlinkage void plat_irq_dispatch(void)
{
	unsigned long pending;

	pending = read_c0_status() & read_c0_cause() & ST0_IM;

	if (pending & STATUSF_IP7)
		do_IRQ(RT305X_CPU_IRQ_COUNTER);

	else if (pending & STATUSF_IP5)
		do_IRQ(RT305X_CPU_IRQ_FE);

	else if (pending & STATUSF_IP6)
		do_IRQ(RT305X_CPU_IRQ_WNIC);

	else if (pending & STATUSF_IP2)
		rt305x_intc_irq_dispatch();

	else
		spurious_interrupt();
}

void __init arch_init_irq(void)
{
	mips_cpu_irq_init();
	rt305x_intc_irq_init();
}

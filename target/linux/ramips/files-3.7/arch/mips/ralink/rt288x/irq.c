/*
 *  Ralink RT288x SoC specific interrupt handling
 *
 *  Copyright (C) 2008-2009 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
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

#include <asm/mach-ralink/common.h>
#include <asm/mach-ralink/rt288x.h>
#include <asm/mach-ralink/rt288x_regs.h>

static void rt288x_intc_irq_dispatch(void)
{
	u32 pending;

	pending = ramips_intc_get_status();

	if (pending & RT2880_INTC_INT_TIMER0)
		do_IRQ(RT2880_INTC_IRQ_TIMER0);

	else if (pending & RT2880_INTC_INT_TIMER1)
		do_IRQ(RT2880_INTC_IRQ_TIMER1);

	else if (pending & RT2880_INTC_INT_UART0)
		do_IRQ(RT2880_INTC_IRQ_UART0);

	else if (pending & RT2880_INTC_INT_PCM)
		do_IRQ(RT2880_INTC_IRQ_PCM);

	else if (pending & RT2880_INTC_INT_UART1)
		do_IRQ(RT2880_INTC_IRQ_UART1);

	/* TODO: handle PIO interrupts as well */

	else
		spurious_interrupt();
}

asmlinkage void plat_irq_dispatch(void)
{
	unsigned long pending;

	pending = read_c0_status() & read_c0_cause() & ST0_IM;

	if (pending & STATUSF_IP7)
		do_IRQ(RT288X_CPU_IRQ_COUNTER);

	else if (pending & STATUSF_IP4)
		do_IRQ(RT288X_CPU_IRQ_PCI);

	else if (pending & STATUSF_IP5)
		do_IRQ(RT288X_CPU_IRQ_FE);

	else if (pending & STATUSF_IP6)
		do_IRQ(RT288X_CPU_IRQ_WNIC);

	else if (pending & STATUSF_IP2)
		rt288x_intc_irq_dispatch();

	else
		spurious_interrupt();
}

void __init arch_init_irq(void)
{
	mips_cpu_irq_init();
	ramips_intc_irq_init(RT2880_INTC_BASE, RT288X_CPU_IRQ_INTC,
			     RT288X_INTC_IRQ_BASE);
}

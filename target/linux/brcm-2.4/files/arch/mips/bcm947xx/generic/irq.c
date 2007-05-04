/*
 * Generic interrupt control functions for Broadcom MIPS boards
 *
 * Copyright 2004, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id: irq.c,v 1.1 2005/03/16 13:50:00 wbx Exp $
 */

#include <linux/config.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/irq.h>

#include <asm/irq.h>
#include <asm/mipsregs.h>
#include <asm/gdb-stub.h>

#define ALLINTS (IE_IRQ0 | IE_IRQ1 | IE_IRQ2 | IE_IRQ3 | IE_IRQ4 | IE_IRQ5)

extern asmlinkage void brcmIRQ(void);
extern asmlinkage unsigned int do_IRQ(int irq, struct pt_regs *regs);

void
brcm_irq_dispatch(struct pt_regs *regs)
{
	u32 cause;

	cause = read_c0_cause() &
		read_c0_status() &
		CAUSEF_IP;

#ifdef CONFIG_KERNPROF
	change_c0_status(cause | 1, 1);
#else
	clear_c0_status(cause);
#endif

	if (cause & CAUSEF_IP7)
		do_IRQ(7, regs);
	if (cause & CAUSEF_IP2)
		do_IRQ(2, regs);
	if (cause & CAUSEF_IP3)
		do_IRQ(3, regs);
	if (cause & CAUSEF_IP4)
		do_IRQ(4, regs);
	if (cause & CAUSEF_IP5)
		do_IRQ(5, regs);
	if (cause & CAUSEF_IP6)
		do_IRQ(6, regs);
}

static void
enable_brcm_irq(unsigned int irq)
{
	if (irq < 8)
		set_c0_status(1 << (irq + 8));
	else
		set_c0_status(IE_IRQ0);
}

static void
disable_brcm_irq(unsigned int irq)
{
	if (irq < 8)
		clear_c0_status(1 << (irq + 8));
	else
		clear_c0_status(IE_IRQ0);
}

static void
ack_brcm_irq(unsigned int irq)
{
	/* Already done in brcm_irq_dispatch */
}

static unsigned int
startup_brcm_irq(unsigned int irq)
{ 
	enable_brcm_irq(irq);

	return 0; /* never anything pending */
}

static void
end_brcm_irq(unsigned int irq)
{
	if (!(irq_desc[irq].status & (IRQ_DISABLED|IRQ_INPROGRESS)))
		enable_brcm_irq(irq);
}

static struct hw_interrupt_type brcm_irq_type = {
	typename: "MIPS",
	startup: startup_brcm_irq,
	shutdown: disable_brcm_irq,
	enable: enable_brcm_irq,
	disable: disable_brcm_irq,
	ack: ack_brcm_irq,
	end: end_brcm_irq,
	NULL
};

void __init
init_IRQ(void)
{
	int i;

	for (i = 0; i < NR_IRQS; i++) {
		irq_desc[i].status = IRQ_DISABLED;
		irq_desc[i].action = 0;
		irq_desc[i].depth = 1;
		irq_desc[i].handler = &brcm_irq_type;
	}

    	set_except_vector(0, brcmIRQ);
	change_c0_status(ST0_IM, ALLINTS);

#ifdef CONFIG_REMOTE_DEBUG
	printk("Breaking into debugger...\n");
	set_debug_traps();
	breakpoint(); 
#endif
}

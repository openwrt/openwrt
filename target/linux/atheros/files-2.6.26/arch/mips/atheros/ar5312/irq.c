/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2003 Atheros Communications, Inc.,  All Rights Reserved.
 * Copyright (C) 2006 FON Technology, SL.
 * Copyright (C) 2006 Imre Kaloz <kaloz@openwrt.org>
 * Copyright (C) 2006 Felix Fietkau <nbd@openwrt.org>
 */

/*
 * Platform devices for Atheros SoCs
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/reboot.h>
#include <asm/bootinfo.h>
#include <asm/time.h>
#include <asm/irq.h>
#include <asm/io.h>

#include <ar531x.h>
#include <gpio.h>

/*
 * Called when an interrupt is received, this function
 * determines exactly which interrupt it was, and it
 * invokes the appropriate handler.
 *
 * Implicitly, we also define interrupt priority by
 * choosing which to dispatch first.
 */
asmlinkage void ar5312_irq_dispatch(void)
{
	int pending = read_c0_status() & read_c0_cause();

	if (pending & CAUSEF_IP2)
		do_IRQ(AR5312_IRQ_WLAN0_INTRS);
	else if (pending & CAUSEF_IP3)
		do_IRQ(AR5312_IRQ_ENET0_INTRS);
	else if (pending & CAUSEF_IP4)
		do_IRQ(AR5312_IRQ_ENET1_INTRS);
	else if (pending & CAUSEF_IP5)
		do_IRQ(AR5312_IRQ_WLAN1_INTRS);
	else if (pending & CAUSEF_IP6) {
		unsigned int ar531x_misc_intrs = sysRegRead(AR531X_ISR) & sysRegRead(AR531X_IMR);

		if (ar531x_misc_intrs & AR531X_ISR_TIMER) {
			do_IRQ(AR531X_MISC_IRQ_TIMER);
			(void)sysRegRead(AR531X_TIMER);
		} else if (ar531x_misc_intrs & AR531X_ISR_AHBPROC)
			do_IRQ(AR531X_MISC_IRQ_AHB_PROC);
		else if ((ar531x_misc_intrs & AR531X_ISR_UART0))
			do_IRQ(AR531X_MISC_IRQ_UART0);
		else if (ar531x_misc_intrs & AR531X_ISR_WD)
			do_IRQ(AR531X_MISC_IRQ_WATCHDOG);
		else
			do_IRQ(AR531X_MISC_IRQ_NONE);
	} else if (pending & CAUSEF_IP7) {
		do_IRQ(AR531X_IRQ_CPU_CLOCK);
	}
}


/* Enable the specified AR531X_MISC_IRQ interrupt */
static void
ar5312_misc_intr_enable(unsigned int irq)
{
	unsigned int imr;

	imr = sysRegRead(AR531X_IMR);
	imr |= (1 << (irq - AR531X_MISC_IRQ_BASE - 1));
	sysRegWrite(AR531X_IMR, imr);
	sysRegRead(AR531X_IMR); /* flush write buffer */
}

/* Disable the specified AR531X_MISC_IRQ interrupt */
static void
ar5312_misc_intr_disable(unsigned int irq)
{
	unsigned int imr;

	imr = sysRegRead(AR531X_IMR);
	imr &= ~(1 << (irq - AR531X_MISC_IRQ_BASE - 1));
	sysRegWrite(AR531X_IMR, imr);
	sysRegRead(AR531X_IMR); /* flush write buffer */
}

/* Turn on the specified AR531X_MISC_IRQ interrupt */
static unsigned int
ar5312_misc_intr_startup(unsigned int irq)
{
	ar5312_misc_intr_enable(irq);
	return 0;
}

/* Turn off the specified AR531X_MISC_IRQ interrupt */
static void
ar5312_misc_intr_shutdown(unsigned int irq)
{
	ar5312_misc_intr_disable(irq);
}

static void
ar5312_misc_intr_ack(unsigned int irq)
{
	ar5312_misc_intr_disable(irq);
}

static void
ar5312_misc_intr_end(unsigned int irq)
{
	if (!(irq_desc[irq].status & (IRQ_DISABLED | IRQ_INPROGRESS)))
		ar5312_misc_intr_enable(irq);
}

static struct irq_chip ar5312_misc_intr_controller = {
	.typename	= "AR5312 misc",
	.startup	= ar5312_misc_intr_startup,
	.shutdown	= ar5312_misc_intr_shutdown,
	.enable		= ar5312_misc_intr_enable,
	.disable	= ar5312_misc_intr_disable,
	.ack		= ar5312_misc_intr_ack,
	.end		= ar5312_misc_intr_end,
};

static irqreturn_t ar5312_ahb_proc_handler(int cpl, void *dev_id)
{
	u32 proc1 = sysRegRead(AR531X_PROC1);
	u32 procAddr = sysRegRead(AR531X_PROCADDR); /* clears error state */
	u32 dma1 = sysRegRead(AR531X_DMA1);
	u32 dmaAddr = sysRegRead(AR531X_DMAADDR);   /* clears error state */

	printk("AHB interrupt: PROCADDR=0x%8.8x  PROC1=0x%8.8x  DMAADDR=0x%8.8x  DMA1=0x%8.8x\n",
			procAddr, proc1, dmaAddr, dma1);

	machine_restart("AHB error"); /* Catastrophic failure */
	return IRQ_HANDLED;
}


static struct irqaction ar5312_ahb_proc_interrupt  = {
	.handler	= ar5312_ahb_proc_handler,
	.flags		= IRQF_DISABLED,
	.name		= "ar5312_ahb_proc_interrupt",
};


static struct irqaction cascade  = {
	.handler	= no_action,
	.flags		= IRQF_DISABLED,
	.name		= "cascade",
};

void __init ar5312_misc_intr_init(int irq_base)
{
	int i;

	for (i = irq_base; i < irq_base + AR531X_MISC_IRQ_COUNT; i++) {
		irq_desc[i].status = IRQ_DISABLED;
		irq_desc[i].action = NULL;
		irq_desc[i].depth = 1;
		irq_desc[i].chip = &ar5312_misc_intr_controller;
	}
	setup_irq(AR531X_MISC_IRQ_AHB_PROC, &ar5312_ahb_proc_interrupt);
	setup_irq(AR5312_IRQ_MISC_INTRS, &cascade);
}



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
#include <linux/interrupt.h>
#include <linux/bitops.h>
#include <asm/bootinfo.h>
#include <asm/irq_cpu.h>
#include <asm/io.h>

#include <ar531x.h>
#include <gpio.h>

static u32 gpiointmask = 0, gpiointval = 0;

static inline void ar5315_gpio_irq(void)
{
	u32 pend;
	sysRegWrite(AR5315_ISR, sysRegRead(AR5315_IMR) | ~AR5315_ISR_GPIO);

	/* only do one gpio interrupt at a time */
	pend = (sysRegRead(AR5315_GPIO_DI) ^ gpiointval) & gpiointmask;
	if (!pend)
		return;

	do_IRQ(AR531X_GPIO_IRQ_BASE + fls(pend) - 1);
}


/*
 * Called when an interrupt is received, this function
 * determines exactly which interrupt it was, and it
 * invokes the appropriate handler.
 *
 * Implicitly, we also define interrupt priority by
 * choosing which to dispatch first.
 */
asmlinkage void ar5315_irq_dispatch(void)
{
	int pending = read_c0_status() & read_c0_cause();

	if (pending & CAUSEF_IP3)
		do_IRQ(AR5315_IRQ_WLAN0_INTRS);
	else if (pending & CAUSEF_IP4)
		do_IRQ(AR5315_IRQ_ENET0_INTRS);
#ifdef CONFIG_PCI
	else if (pending & CAUSEF_IP5)
		ar5315_pci_irq(AR5315_IRQ_LCBUS_PCI);
#endif
	else if (pending & CAUSEF_IP2) {
		unsigned int ar531x_misc_intrs = sysRegRead(AR5315_ISR) & sysRegRead(AR5315_IMR);

		if (ar531x_misc_intrs & AR5315_ISR_SPI)
			do_IRQ(AR531X_MISC_IRQ_SPI);
		else if (ar531x_misc_intrs & AR5315_ISR_TIMER)
			do_IRQ(AR531X_MISC_IRQ_TIMER);
		else if (ar531x_misc_intrs & AR5315_ISR_AHB)
			do_IRQ(AR531X_MISC_IRQ_AHB_PROC);
		else if (ar531x_misc_intrs & AR5315_ISR_GPIO)
			ar5315_gpio_irq();
		else if (ar531x_misc_intrs & AR5315_ISR_UART0)
			do_IRQ(AR531X_MISC_IRQ_UART0);
		else if (ar531x_misc_intrs & AR5315_ISR_WD)
			do_IRQ(AR531X_MISC_IRQ_WATCHDOG);
		else
			do_IRQ(AR531X_MISC_IRQ_NONE);
	} else if (pending & CAUSEF_IP7)
		do_IRQ(AR531X_IRQ_CPU_CLOCK);
}

#ifdef CONFIG_PCI
static inline void pci_abort_irq(void)
{
	sysRegWrite(AR5315_PCI_INT_STATUS, AR5315_PCI_ABORT_INT);
	(void)sysRegRead(AR5315_PCI_INT_STATUS); /* flush write to hardware */
}

static inline void pci_ack_irq(void)
{
	sysRegWrite(AR5315_PCI_INT_STATUS, AR5315_PCI_EXT_INT);
	(void)sysRegRead(AR5315_PCI_INT_STATUS); /* flush write to hardware */
}

void ar5315_pci_irq(int irq)
{
	if (sysRegRead(AR5315_PCI_INT_STATUS) == AR5315_PCI_ABORT_INT)
		pci_abort_irq();
	else {
		do_IRQ(irq);
		pci_ack_irq();
	}
}
#endif

static void ar5315_gpio_intr_enable(unsigned int irq)
{
	u32 gpio, mask;
	gpio = irq - AR531X_GPIO_IRQ_BASE;
	mask = 1 << gpio;
	gpiointmask |= mask;

	/* reconfigure GPIO line as input */
	sysRegMask(AR5315_GPIO_CR, AR5315_GPIO_CR_M(gpio), AR5315_GPIO_CR_I(gpio));

	/* Enable interrupt with edge detection */
	sysRegMask(AR5315_GPIO_INT, AR5315_GPIO_INT_M | AR5315_GPIO_INT_LVL_M, gpio | AR5315_GPIO_INT_LVL(3));
}

static void ar5315_gpio_intr_disable(unsigned int irq)
{
	u32 gpio, mask;
	gpio = irq - AR531X_GPIO_IRQ_BASE;
	mask = 1 << gpio;

	gpiointmask &= ~mask;

	/* Disable interrupt with edge detection */
	sysRegMask(AR5315_GPIO_INT, AR5315_GPIO_INT_M | AR5315_GPIO_INT_LVL_M, gpio | AR5315_GPIO_INT_LVL(0));
}

/* Turn on the specified AR531X_MISC_IRQ interrupt */
static unsigned int ar5315_gpio_intr_startup(unsigned int irq)
{
	ar5315_gpio_intr_enable(irq);
	return 0;
}

/* Turn off the specified AR531X_MISC_IRQ interrupt */
static void
ar5315_gpio_intr_shutdown(unsigned int irq)
{
	ar5315_gpio_intr_disable(irq);
}

static void
ar5315_gpio_intr_ack(unsigned int irq)
{
	ar5315_gpio_intr_disable(irq);
}

static void
ar5315_gpio_intr_end(unsigned int irq)
{
	if (!(irq_desc[irq].status & (IRQ_DISABLED | IRQ_INPROGRESS)))
		ar5315_gpio_intr_enable(irq);
}

static struct irq_chip ar5315_gpio_intr_controller = {
	.typename	= "AR5315 GPIO",
	.startup	= ar5315_gpio_intr_startup,
	.shutdown	= ar5315_gpio_intr_shutdown,
	.enable		= ar5315_gpio_intr_enable,
	.disable	= ar5315_gpio_intr_disable,
	.ack		= ar5315_gpio_intr_ack,
	.end		= ar5315_gpio_intr_end,
};


/* Enable the specified AR531X_MISC_IRQ interrupt */
static void
ar5315_misc_intr_enable(unsigned int irq)
{
	unsigned int imr;

	imr = sysRegRead(AR5315_IMR);
	switch(irq)
	{
	   case AR531X_MISC_IRQ_SPI:
		 imr |= AR5315_ISR_SPI;
		 break;

	   case AR531X_MISC_IRQ_TIMER:
	     imr |= AR5315_ISR_TIMER;
	     break;

	   case AR531X_MISC_IRQ_AHB_PROC:
	     imr |= AR5315_ISR_AHB;
	     break;

	   case AR531X_MISC_IRQ_AHB_DMA:
	     imr |= 0/* ?? */;
	     break;

	   case	AR531X_MISC_IRQ_GPIO:
	     imr |= AR5315_ISR_GPIO;
	     break;

	   case AR531X_MISC_IRQ_UART0:
	     imr |= AR5315_ISR_UART0;
	     break;


	   case	AR531X_MISC_IRQ_WATCHDOG:
	     imr |= AR5315_ISR_WD;
	     break;

	   case AR531X_MISC_IRQ_LOCAL:
	     imr |= 0/* ?? */;
	     break;

	}
	sysRegWrite(AR5315_IMR, imr);
	imr=sysRegRead(AR5315_IMR); /* flush write buffer */
}

/* Disable the specified AR531X_MISC_IRQ interrupt */
static void
ar5315_misc_intr_disable(unsigned int irq)
{
	unsigned int imr;

	imr = sysRegRead(AR5315_IMR);
	switch(irq)
	{
	   case AR531X_MISC_IRQ_SPI:
		 imr &= ~AR5315_ISR_SPI;
		 break;

	   case AR531X_MISC_IRQ_TIMER:
	     imr &= (~AR5315_ISR_TIMER);
	     break;

	   case AR531X_MISC_IRQ_AHB_PROC:
	     imr &= (~AR5315_ISR_AHB);
	     break;

	   case AR531X_MISC_IRQ_AHB_DMA:
	     imr &= 0/* ?? */;
	     break;

	   case	AR531X_MISC_IRQ_GPIO:
	     imr &= ~AR5315_ISR_GPIO;
	     break;

	   case AR531X_MISC_IRQ_UART0:
	     imr &= (~AR5315_ISR_UART0);
	     break;

	   case	AR531X_MISC_IRQ_WATCHDOG:
	     imr &= (~AR5315_ISR_WD);
	     break;

	   case AR531X_MISC_IRQ_LOCAL:
	     imr &= ~0/* ?? */;
	     break;

	}
	sysRegWrite(AR5315_IMR, imr);
	sysRegRead(AR5315_IMR); /* flush write buffer */
}

/* Turn on the specified AR531X_MISC_IRQ interrupt */
static unsigned int
ar5315_misc_intr_startup(unsigned int irq)
{
	ar5315_misc_intr_enable(irq);
	return 0;
}

/* Turn off the specified AR531X_MISC_IRQ interrupt */
static void
ar5315_misc_intr_shutdown(unsigned int irq)
{
	ar5315_misc_intr_disable(irq);
}

static void
ar5315_misc_intr_ack(unsigned int irq)
{
	ar5315_misc_intr_disable(irq);
}

static void
ar5315_misc_intr_end(unsigned int irq)
{
	if (!(irq_desc[irq].status & (IRQ_DISABLED | IRQ_INPROGRESS)))
		ar5315_misc_intr_enable(irq);
}

static struct irq_chip ar5315_misc_intr_controller = {
	.typename	= "AR5315 misc",
	.startup	= ar5315_misc_intr_startup,
	.shutdown	= ar5315_misc_intr_shutdown,
	.enable		= ar5315_misc_intr_enable,
	.disable	= ar5315_misc_intr_disable,
	.ack		= ar5315_misc_intr_ack,
	.end		= ar5315_misc_intr_end,
};

static irqreturn_t ar5315_ahb_proc_handler(int cpl, void *dev_id)
{
    sysRegWrite(AR5315_AHB_ERR0,AHB_ERROR_DET);
    sysRegRead(AR5315_AHB_ERR1);

    printk("AHB fatal error\n");
    machine_restart("AHB error"); /* Catastrophic failure */

    return IRQ_HANDLED;
}

static struct irqaction ar5315_ahb_proc_interrupt  = {
	.handler	= ar5315_ahb_proc_handler,
	.flags		= IRQF_DISABLED,
	.name		= "ar5315_ahb_proc_interrupt",
};


static struct irqaction cascade  = {
	.handler	= no_action,
	.flags		= IRQF_DISABLED,
	.name		= "cascade",
};

static void ar5315_gpio_intr_init(int irq_base)
{
	int i;

	for (i = irq_base; i < irq_base + AR531X_GPIO_IRQ_COUNT; i++) {
		irq_desc[i].status = IRQ_DISABLED;
		irq_desc[i].action = NULL;
		irq_desc[i].depth = 1;
		irq_desc[i].chip = &ar5315_gpio_intr_controller;
	}
	setup_irq(AR531X_MISC_IRQ_GPIO, &cascade);
	gpiointval = sysRegRead(AR5315_GPIO_DI);
}

void ar5315_misc_intr_init(int irq_base)
{
	int i;

	for (i = irq_base; i < irq_base + AR531X_MISC_IRQ_COUNT; i++) {
		irq_desc[i].status = IRQ_DISABLED;
		irq_desc[i].action = NULL;
		irq_desc[i].depth = 1;
		irq_desc[i].chip = &ar5315_misc_intr_controller;
	}
	setup_irq(AR531X_MISC_IRQ_AHB_PROC, &ar5315_ahb_proc_interrupt);
	setup_irq(AR5315_IRQ_MISC_INTRS, &cascade);
	ar5315_gpio_intr_init(AR531X_GPIO_IRQ_BASE);
}


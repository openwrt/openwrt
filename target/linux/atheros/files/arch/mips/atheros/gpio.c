/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2003 Atheros Communications, Inc.,  All Rights Reserved.
 * Copyright (C) 2006 FON Technology, SL.
 * Copyright (C) 2006 Imre Kaloz <kaloz@openwrt.org>
 * Copyright (C) 2007 Othello <bach_ag@hotmail.com>
 */

/*
 * Support for AR531X GPIO -- General Purpose Input/Output Pins
 */

#include <linux/autoconf.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/types.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/irq.h>

#include <asm/addrspace.h>
#include <asm/io.h>
#include <asm/irq_cpu.h>
#include <asm/gpio.h>
#include "ar531x.h"
/*
   GPIO Interrupt Support
      Make use of request_irq() and the function gpio_to_irq() to trap gpio events
 */

/* Global variables */
static u32 ar531x_gpio_intr_Mask = 0;
/*
    AR5312: I don't have any devices with this chip. Assumed to be similar to AR5215
    will someone who has one try the code and remove this message if it works?
 */

#ifdef CONFIG_ATHEROS_AR5315
/*
    AR5315: Up to 2 GPIO pins may be monitored simultaneously
    specifying more pins if you already have 2 will not have any effect
        however, the excess gpio irqs will also be triggered if a valid gpio being monitored triggers
    only high, low or edge triggered interrupt supported
 */
static unsigned int ar5315_gpio_set_type_gpio = 0;
static unsigned int ar5315_gpio_set_type_lvl = IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING;
#endif

#ifdef CONFIG_ATHEROS_AR5312
/* Enable the specified AR5312_GPIO_IRQ interrupt */
static void ar5312_gpio_intr_enable(unsigned int irq) {
	u32 reg;
	unsigned int gpio;
	unsigned int imr;

	gpio = irq - (AR531X_GPIO_IRQ(0));
	if (gpio >= AR531X_NUM_GPIO)
		return;
	ar531x_gpio_intr_Mask |= (1<<gpio);

	reg = sysRegRead(AR531X_GPIO_CR);
	reg &= ~(AR531X_GPIO_CR_M(gpio) | AR531X_GPIO_CR_UART(gpio) | AR531X_GPIO_CR_INT(gpio));
	reg |= AR531X_GPIO_CR_I(gpio);
	reg |= AR531X_GPIO_CR_INT(gpio);

	sysRegWrite(AR531X_GPIO_CR, reg);
	(void)sysRegRead(AR531X_GPIO_CR); /* flush to hardware */

	imr = sysRegRead(AR531X_IMR);
	imr |= AR531X_ISR_GPIO;
	sysRegWrite(AR531X_IMR, imr);
	imr = sysRegRead(AR531X_IMR); /* flush write buffer */
}

/* Disable the specified AR5312_GPIO_IRQ interrupt */
static void ar5312_gpio_intr_disable(unsigned int irq) {
	u32 reg;
	unsigned int gpio;
	gpio = irq - (AR531X_GPIO_IRQ(0));
	if (gpio >= AR531X_NUM_GPIO)
		return;

	reg = sysRegRead(AR531X_GPIO_CR);
	reg &= ~(AR531X_GPIO_CR_M(gpio) | AR531X_GPIO_CR_UART(gpio) | AR531X_GPIO_CR_INT(gpio));
	reg |= AR531X_GPIO_CR_I(gpio);
	/* No GPIO_CR_INT bit */

	sysRegWrite(AR531X_GPIO_CR, reg);
	(void)sysRegRead(AR531X_GPIO_CR); /* flush to hardware */

	/* Disable Interrupt if no gpio needs triggering */
	if (ar531x_gpio_intr_Mask != 0) {
		unsigned int imr;

		imr = sysRegRead(AR531X_IMR);
		imr &= ~AR531X_ISR_GPIO;
		sysRegWrite(AR531X_IMR, imr);
		imr = sysRegRead(AR531X_IMR); /* flush write buffer */
	}

	ar531x_gpio_intr_Mask &= ~(1<<gpio);
}

/* Turn on the specified AR5312_GPIO_IRQ interrupt */
static unsigned int ar5312_gpio_intr_startup(unsigned int irq) {
	ar5312_gpio_intr_enable(irq);
	return 0;
}

static void ar5312_gpio_intr_end(unsigned int irq) {
	if (!(irq_desc[irq].status & (IRQ_DISABLED | IRQ_INPROGRESS)))
		ar5312_gpio_intr_enable(irq);
}

asmlinkage void ar5312_gpio_irq_dispatch(void) {
	int i;
	u32 gpioIntPending;
	gpioIntPending = sysRegRead(AR531X_GPIO_DI) & ar531x_gpio_intr_Mask;
	sysRegWrite(AR531X_ISR, sysRegRead(AR531X_IMR) | ~AR531X_ISR_GPIO);
	for (i=0; i<AR531X_GPIO_IRQ_COUNT; i++) {
	if (gpioIntPending & (1 << i))
		do_IRQ(AR531X_GPIO_IRQ(i));
	}
}
#endif	/* #ifdef CONFIG_ATHEROS_AR5312 */

#ifdef CONFIG_ATHEROS_AR5315
/* Enable the specified AR5315_GPIO_IRQ interrupt */
static void ar5315_gpio_intr_enable(unsigned int irq) {
	u32 reg;
	unsigned int gpio;
	unsigned int imr;
	unsigned int i;

	gpio = irq - (AR531X_GPIO_IRQ(0));
	if (gpio >= AR5315_NUM_GPIO)
		return;
	ar531x_gpio_intr_Mask |= (1<<gpio);

	reg = sysRegRead(AR5315_GPIO_CR);
	reg &= ~(AR5315_GPIO_CR_M(gpio));
	reg |= AR5315_GPIO_CR_I(gpio);
	sysRegWrite(AR5315_GPIO_CR, reg);
	(void)sysRegRead(AR5315_GPIO_CR); /* flush write to hardware */

	/* Locate a free register slot to enable gpio intr
	   will fail silently if no more slots are available
	 */
	reg = sysRegRead(AR5315_GPIO_INT);
	for (i=0 ; i<=AR5315_GPIO_INT_MAX_Y ; i++) {
		/* Free slot means trigger level = 0 */
		if ( AR5315_GPIO_INT_LVL_OFF ==
		    (reg & AR5315_GPIO_INT_LVL_M) ) {

			unsigned int def_lvl = AR5315_GPIO_INT_LVL_EDGE;
			if (ar5315_gpio_set_type_gpio == gpio)
				def_lvl = ar5315_gpio_set_type_lvl;

			/* Set the gpio level trigger mode */
/*			reg &= ~(AR5315_GPIO_INT_LVL_M(i)); */
			reg |= AR5315_GPIO_INT_LVL(def_lvl);

			/* Enable the gpio pin */
			reg &= ~(AR5315_GPIO_INT_M);
			reg |= AR5315_GPIO_INT_S(i);

			sysRegWrite(AR5315_GPIO_INT, reg);
			(void)sysRegRead(AR5315_GPIO_INT); /* flush write to hardware */

			/* break out of for loop */
			break;
		} /* end if trigger level for slot i is 0 */
	} /* end for each slot */

	imr = sysRegRead(AR5315_IMR);
	imr |= AR5315_ISR_GPIO;
	sysRegWrite(AR5315_IMR, imr);
	imr = sysRegRead(AR5315_IMR); /* flush write buffer */
}


/* Disable the specified AR5315_GPIO_IRQ interrupt */
static void ar5315_gpio_intr_disable(unsigned int irq) {
	u32 reg;
	unsigned int gpio;
	unsigned int i;

	gpio = irq - (AR531X_GPIO_IRQ(0));
	if (gpio >= AR5315_NUM_GPIO)
		return;

	reg = sysRegRead(AR5315_GPIO_CR);
	reg &= ~(AR5315_GPIO_CR_M(gpio));
	reg |= AR5315_GPIO_CR_I(gpio);
	sysRegWrite(AR5315_GPIO_CR, reg);
	(void)sysRegRead(AR5315_GPIO_CR); /* flush write to hardware */

	/* Locate a the correct register slot to disable gpio intr */
	reg = sysRegRead(AR5315_GPIO_INT);
	for (i=0 ; i<=AR5315_GPIO_INT_MAX_Y ; i++) {
		/* If this correct  */
		if ( AR5315_GPIO_INT_S(i) ==
		    (reg & AR5315_GPIO_INT_M) ) {
			/* Clear the gpio level trigger mode */
			reg &= ~(AR5315_GPIO_INT_LVL_M);

			sysRegWrite(AR5315_GPIO_INT, reg);
			(void)sysRegRead(AR5315_GPIO_INT); /* flush write to hardware */
			break;
		} /* end if trigger level for slot i is 0 */
	} /* end for each slot */

	/* Disable interrupt only if no gpio needs triggering */
	if (ar531x_gpio_intr_Mask != 0) {
		unsigned int imr;

		imr = sysRegRead(AR5315_IMR);
		imr &= ~AR5315_ISR_GPIO;
		sysRegWrite(AR5315_IMR, imr);
		imr = sysRegRead(AR5315_IMR); /* flush write buffer */
	}

	ar531x_gpio_intr_Mask &= ~(1<<gpio);
}

/* Turn on the specified AR5315_GPIO_IRQ interrupt */
static unsigned int ar5315_gpio_intr_startup(unsigned int irq) {
	ar5315_gpio_intr_enable(irq);
	return 0;
}

static void ar5315_gpio_intr_end(unsigned int irq) {
	if (!(irq_desc[irq].status & (IRQ_DISABLED | IRQ_INPROGRESS)))
		ar5315_gpio_intr_enable(irq);
}

static int ar5315_gpio_intr_set_type(unsigned int irq, unsigned int flow_type) {
	ar5315_gpio_set_type_gpio = irq - (AR531X_GPIO_IRQ(0));
	if (ar5315_gpio_set_type_gpio > AR5315_NUM_GPIO)
		return -EINVAL;
	switch (flow_type & IRQF_TRIGGER_MASK) {
		case IRQF_TRIGGER_RISING:
		case IRQF_TRIGGER_FALLING:
			printk(KERN_WARNING "AR5315 GPIO %u falling back to edge triggered\n", ar5315_gpio_set_type_gpio);
		case IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING:
			ar5315_gpio_set_type_lvl = AR5315_GPIO_INT_LVL_EDGE;
			break;
		case IRQF_TRIGGER_LOW:
			ar5315_gpio_set_type_lvl = AR5315_GPIO_INT_LVL_LOW;
			break;
		case IRQF_TRIGGER_HIGH:
			ar5315_gpio_set_type_lvl = AR5315_GPIO_INT_LVL_HIGH;
			break;
		default:
			return -EINVAL;
	}
	return 0;
}

asmlinkage void ar5315_gpio_irq_dispatch(void){
	int i;
	u32 gpioIntPending;
	gpioIntPending = sysRegRead(AR5315_GPIO_DI) & ar531x_gpio_intr_Mask;
	sysRegWrite(AR5315_ISR, sysRegRead(AR5315_IMR) | ~AR5315_ISR_GPIO);
	for (i=0; i<AR531X_GPIO_IRQ_COUNT; i++) {
	if (gpioIntPending & (1 << i))
		do_IRQ(AR531X_GPIO_IRQ(i));
	}
}
#endif /* #ifdef CONFIG_ATHEROS_AR5315  */

/* Common Code */
static struct irq_chip ar531x_gpio_intr_controller = {
	.typename	= "AR531X GPIO",
};

/* ARGSUSED */
irqreturn_t
spurious_gpio_handler(int cpl, void *dev_id)
{
	u32 gpioDataIn;
	DO_AR5312(gpioDataIn = sysRegRead(AR531X_GPIO_DI);)
	DO_AR5315(gpioDataIn = sysRegRead(AR5315_GPIO_DI);)

	printk("spurious_gpio_handler: 0x%08x dev=%p DI=0x%08x gpioIntMask=0x%08x\n",
		cpl, dev_id, gpioDataIn, ar531x_gpio_intr_Mask);

	return IRQ_NONE;
}

static struct irqaction spurious_gpio  = {
	.handler	= spurious_gpio_handler,
	.name		= "spurious_gpio",
};

/* Initialize AR531X GPIO interrupts */
static int __init ar531x_gpio_init(void)
{
	int i;

	DO_AR5312( \
		ar531x_gpio_intr_controller.startup = ar5312_gpio_intr_startup; \
		ar531x_gpio_intr_controller.shutdown = ar5312_gpio_intr_disable; \
		ar531x_gpio_intr_controller.enable = ar5312_gpio_intr_enable; \
		ar531x_gpio_intr_controller.disable = ar5312_gpio_intr_disable; \
		ar531x_gpio_intr_controller.ack = ar5312_gpio_intr_disable; \
		ar531x_gpio_intr_controller.end = ar5312_gpio_intr_end; \
	)

	DO_AR5315( \
		ar531x_gpio_intr_controller.startup = ar5315_gpio_intr_startup; \
		ar531x_gpio_intr_controller.shutdown = ar5315_gpio_intr_disable; \
		ar531x_gpio_intr_controller.enable = ar5315_gpio_intr_enable; \
		ar531x_gpio_intr_controller.disable = ar5315_gpio_intr_disable; \
		ar531x_gpio_intr_controller.ack = ar5315_gpio_intr_disable; \
		ar531x_gpio_intr_controller.end = ar5315_gpio_intr_end; \
		ar531x_gpio_intr_controller.set_type = ar5315_gpio_intr_set_type; \
	)

	for (i = AR531X_GPIO_IRQ_BASE;
	     i < AR531X_GPIO_IRQ_BASE + AR531X_GPIO_IRQ_COUNT;
	     i++) {
		irq_desc[i].status = IRQ_DISABLED;
		irq_desc[i].action = NULL;
		irq_desc[i].depth = 1;
		irq_desc[i].chip = &ar531x_gpio_intr_controller;
	}

	setup_irq(AR531X_GPIO_IRQ_NONE, &spurious_gpio);

	return 0;
}

subsys_initcall(ar531x_gpio_init);


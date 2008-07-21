/*
 *  Atheros AR71xx SoC specific interrupt handling
 *
 *  Copyright (C) 2008 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 *  Parts of this file are based on Atheros' 2.6.15 BSP
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

#include <asm/mach-ar71xx/ar71xx.h>

#ifdef CONFIG_PCI
static void ar71xx_pci_irq_dispatch(void)
{
	u32 pending;

	pending = ar71xx_reset_rr(RESET_REG_PCI_INT_STATUS) &
	    ar71xx_reset_rr(RESET_REG_PCI_INT_ENABLE);

	if (pending & PCI_INT_DEV0)
		do_IRQ(AR71XX_PCI_IRQ_DEV0);

	else if (pending & PCI_INT_DEV1)
		do_IRQ(AR71XX_PCI_IRQ_DEV1);

	else if (pending & PCI_INT_DEV2)
		do_IRQ(AR71XX_PCI_IRQ_DEV2);

	else
		spurious_interrupt();
}

static void ar71xx_pci_irq_unmask(unsigned int irq)
{
	irq -= AR71XX_PCI_IRQ_BASE;
	ar71xx_reset_wr(RESET_REG_PCI_INT_ENABLE,
		ar71xx_reset_rr(RESET_REG_PCI_INT_ENABLE) | (1 << irq));
}

static void ar71xx_pci_irq_mask(unsigned int irq)
{
	irq -= AR71XX_PCI_IRQ_BASE;
	ar71xx_reset_wr(RESET_REG_PCI_INT_ENABLE,
		ar71xx_reset_rr(RESET_REG_PCI_INT_ENABLE) & ~(1 << irq));
}

static struct irq_chip ar71xx_pci_irq_chip = {
	.name		= "AR71XX PCI ",
	.mask		= ar71xx_pci_irq_mask,
	.unmask		= ar71xx_pci_irq_unmask,
	.mask_ack	= ar71xx_pci_irq_mask,
};

static struct irqaction ar71xx_pci_irqaction = {
	.handler	= no_action,
	.name		= "cascade [AR71XX PCI]",
};

static void __init ar71xx_pci_irq_init(void)
{
	int i;

	ar71xx_reset_wr(RESET_REG_PCI_INT_ENABLE, 0);
	ar71xx_reset_wr(RESET_REG_PCI_INT_STATUS, 0);

	for (i = AR71XX_PCI_IRQ_BASE;
	     i < AR71XX_PCI_IRQ_BASE + AR71XX_PCI_IRQ_COUNT; i++) {
		irq_desc[i].status = IRQ_DISABLED;
		set_irq_chip_and_handler(i, &ar71xx_pci_irq_chip,
					 handle_level_irq);
	}

	setup_irq(AR71XX_CPU_IRQ_PCI, &ar71xx_pci_irqaction);
}

#endif /* CONFIG_PCI */

static void ar71xx_gpio_irq_dispatch(void)
{
	u32 pending;

	pending = ar71xx_gpio_rr(GPIO_REG_INT_PENDING)
		& ar71xx_gpio_rr(GPIO_REG_INT_ENABLE);

	if (pending)
		do_IRQ(AR71XX_GPIO_IRQ_BASE + fls(pending) - 1);
	else
		spurious_interrupt();
}

static void ar71xx_gpio_irq_unmask(unsigned int irq)
{
	irq -= AR71XX_GPIO_IRQ_BASE;
	ar71xx_gpio_wr(GPIO_REG_INT_ENABLE,
			ar71xx_gpio_rr(GPIO_REG_INT_ENABLE) | (1 << irq));
}

static void ar71xx_gpio_irq_mask(unsigned int irq)
{
	irq -= AR71XX_GPIO_IRQ_BASE;
	ar71xx_gpio_wr(GPIO_REG_INT_ENABLE,
			ar71xx_gpio_rr(GPIO_REG_INT_ENABLE) & ~(1 << irq));
}

#if 0
static int ar71xx_gpio_irq_set_type(unsigned int irq, unsigned int flow_type)
{
	/* TODO: implement */
	return 0;
}
#else
#define ar71xx_gpio_irq_set_type	NULL
#endif

struct irq_chip ar71xx_gpio_irq_chip = {
	.name		= "AR71XX GPIO",
	.unmask		= ar71xx_gpio_irq_unmask,
	.mask		= ar71xx_gpio_irq_mask,
	.mask_ack	= ar71xx_gpio_irq_mask,
	.set_type	= ar71xx_gpio_irq_set_type,
};

static struct irqaction ar71xx_gpio_irqaction = {
	.handler	= no_action,
	.name		= "cascade [AR71XX GPIO]",
};

#define GPIO_IRQ_INIT_STATUS (IRQ_LEVEL | IRQ_TYPE_LEVEL_HIGH | IRQ_DISABLED)
#define GPIO_INT_ALL	0xffff

static void __init ar71xx_gpio_irq_init(void)
{
	int i;

	ar71xx_gpio_wr(GPIO_REG_INT_ENABLE, 0);
	ar71xx_gpio_wr(GPIO_REG_INT_PENDING, 0);

	/* setup type of all GPIO interrupts to level sensitive */
	ar71xx_gpio_wr(GPIO_REG_INT_TYPE, GPIO_INT_ALL);

	/* setup polarity of all GPIO interrupts to active high */
	ar71xx_gpio_wr(GPIO_REG_INT_POLARITY, GPIO_INT_ALL);

	for (i = AR71XX_GPIO_IRQ_BASE;
	     i < AR71XX_GPIO_IRQ_BASE + AR71XX_GPIO_IRQ_COUNT; i++) {
		irq_desc[i].status = GPIO_IRQ_INIT_STATUS;
		set_irq_chip_and_handler(i, &ar71xx_gpio_irq_chip,
					 handle_level_irq);
	}

	setup_irq(AR71XX_MISC_IRQ_GPIO, &ar71xx_gpio_irqaction);
}

static void ar71xx_misc_irq_dispatch(void)
{
	u32 pending;

	pending = ar71xx_reset_rr(RESET_REG_MISC_INT_STATUS)
	    & ar71xx_reset_rr(RESET_REG_MISC_INT_ENABLE);

	if (pending & MISC_INT_UART)
		do_IRQ(AR71XX_MISC_IRQ_UART);

	else if (pending & MISC_INT_DMA)
		do_IRQ(AR71XX_MISC_IRQ_DMA);

	else if (pending & MISC_INT_PERFC)
		do_IRQ(AR71XX_MISC_IRQ_PERFC);

	else if (pending & MISC_INT_TIMER)
		do_IRQ(AR71XX_MISC_IRQ_TIMER);

	else if (pending & MISC_INT_OHCI)
		do_IRQ(AR71XX_MISC_IRQ_OHCI);

	else if (pending & MISC_INT_ERROR)
		do_IRQ(AR71XX_MISC_IRQ_ERROR);

	else if (pending & MISC_INT_GPIO)
		ar71xx_gpio_irq_dispatch();

	else if (pending & MISC_INT_WDOG)
		do_IRQ(AR71XX_MISC_IRQ_WDOG);

	else
		spurious_interrupt();
}

static void ar71xx_misc_irq_unmask(unsigned int irq)
{
	irq -= AR71XX_MISC_IRQ_BASE;
	ar71xx_reset_wr(RESET_REG_MISC_INT_ENABLE,
		ar71xx_reset_rr(RESET_REG_MISC_INT_ENABLE) | (1 << irq));
}

static void ar71xx_misc_irq_mask(unsigned int irq)
{
	irq -= AR71XX_MISC_IRQ_BASE;
	ar71xx_reset_wr(RESET_REG_MISC_INT_ENABLE,
		ar71xx_reset_rr(RESET_REG_MISC_INT_ENABLE) & ~(1 << irq));
}

struct irq_chip ar71xx_misc_irq_chip = {
	.name		= "AR71XX MISC",
	.unmask		= ar71xx_misc_irq_unmask,
	.mask		= ar71xx_misc_irq_mask,
	.mask_ack	= ar71xx_misc_irq_mask,
};

static struct irqaction ar71xx_misc_irqaction = {
	.handler	= no_action,
	.name		= "cascade [AR71XX MISC]",
};

static void __init ar71xx_misc_irq_init(void)
{
	int i;

	ar71xx_reset_wr(RESET_REG_MISC_INT_ENABLE, 0);
	ar71xx_reset_wr(RESET_REG_MISC_INT_STATUS, 0);

	for (i = AR71XX_MISC_IRQ_BASE;
	     i < AR71XX_MISC_IRQ_BASE + AR71XX_MISC_IRQ_COUNT; i++) {
		irq_desc[i].status = IRQ_DISABLED;
		set_irq_chip_and_handler(i, &ar71xx_misc_irq_chip,
					 handle_level_irq);
	}

	setup_irq(AR71XX_CPU_IRQ_MISC, &ar71xx_misc_irqaction);
}

asmlinkage void plat_irq_dispatch(void)
{
	unsigned long pending;

	pending = read_c0_status() & read_c0_cause() & ST0_IM;

	if (pending & STATUSF_IP7)
		do_IRQ(AR71XX_CPU_IRQ_TIMER);

#ifdef CONFIG_PCI
	else if (pending & STATUSF_IP2)
		ar71xx_pci_irq_dispatch();
#endif

	else if (pending & STATUSF_IP4)
		do_IRQ(AR71XX_CPU_IRQ_GE0);

	else if (pending & STATUSF_IP5)
		do_IRQ(AR71XX_CPU_IRQ_GE1);

	else if (pending & STATUSF_IP3)
		do_IRQ(AR71XX_CPU_IRQ_USB);

	else if (pending & STATUSF_IP6)
		ar71xx_misc_irq_dispatch();

	else
		spurious_interrupt();
}

void __init arch_init_irq(void)
{
	mips_cpu_irq_init();

	ar71xx_misc_irq_init();

#ifdef CONFIG_PCI
	ar71xx_pci_irq_init();
#endif

	ar71xx_gpio_irq_init();
}

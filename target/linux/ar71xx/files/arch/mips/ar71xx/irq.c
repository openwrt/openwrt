/*
 *  Atheros AR71xx SoC specific interrupt handling
 *
 *  Copyright (C) 2010-2011 Jaiganesh Narayanan <jnarayanan@atheros.com>
 *  Copyright (C) 2008-2010 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 *  Parts of this file are based on Atheros 2.6.15 BSP
 *  Parts of this file are based on Atheros 2.6.31 BSP
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

static int ip2_flush_reg;

static void ar71xx_gpio_irq_dispatch(void)
{
	void __iomem *base = ar71xx_gpio_base;
	u32 pending;

	pending = __raw_readl(base + GPIO_REG_INT_PENDING) &
		  __raw_readl(base + GPIO_REG_INT_ENABLE);

	if (pending)
		do_IRQ(AR71XX_GPIO_IRQ_BASE + fls(pending) - 1);
	else
		spurious_interrupt();
}

static void ar71xx_gpio_irq_unmask(unsigned int irq)
{
	void __iomem *base = ar71xx_gpio_base;
	u32 t;

	irq -= AR71XX_GPIO_IRQ_BASE;

	t = __raw_readl(base + GPIO_REG_INT_ENABLE);
	__raw_writel(t | (1 << irq), base + GPIO_REG_INT_ENABLE);

	/* flush write */
	(void) __raw_readl(base + GPIO_REG_INT_ENABLE);
}

static void ar71xx_gpio_irq_mask(unsigned int irq)
{
	void __iomem *base = ar71xx_gpio_base;
	u32 t;

	irq -= AR71XX_GPIO_IRQ_BASE;

	t = __raw_readl(base + GPIO_REG_INT_ENABLE);
	__raw_writel(t & ~(1 << irq), base + GPIO_REG_INT_ENABLE);

	/* flush write */
	(void) __raw_readl(base + GPIO_REG_INT_ENABLE);
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

static struct irq_chip ar71xx_gpio_irq_chip = {
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
	void __iomem *base = ar71xx_gpio_base;
	int i;

	__raw_writel(0, base + GPIO_REG_INT_ENABLE);
	__raw_writel(0, base + GPIO_REG_INT_PENDING);

	/* setup type of all GPIO interrupts to level sensitive */
	__raw_writel(GPIO_INT_ALL, base + GPIO_REG_INT_TYPE);

	/* setup polarity of all GPIO interrupts to active high */
	__raw_writel(GPIO_INT_ALL, base + GPIO_REG_INT_POLARITY);

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

	pending = ar71xx_reset_rr(AR71XX_RESET_REG_MISC_INT_STATUS)
	    & ar71xx_reset_rr(AR71XX_RESET_REG_MISC_INT_ENABLE);

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

	else if (pending & MISC_INT_TIMER2)
		do_IRQ(AR71XX_MISC_IRQ_TIMER2);

	else if (pending & MISC_INT_TIMER3)
		do_IRQ(AR71XX_MISC_IRQ_TIMER3);

	else if (pending & MISC_INT_TIMER4)
		do_IRQ(AR71XX_MISC_IRQ_TIMER4);

	else if (pending & MISC_INT_DDR_PERF)
		do_IRQ(AR71XX_MISC_IRQ_DDR_PERF);

	else if (pending & MISC_INT_ENET_LINK)
		do_IRQ(AR71XX_MISC_IRQ_ENET_LINK);

	else
		spurious_interrupt();
}

static void ar71xx_misc_irq_unmask(unsigned int irq)
{
	void __iomem *base = ar71xx_reset_base;
	u32 t;

	irq -= AR71XX_MISC_IRQ_BASE;

	t = __raw_readl(base + AR71XX_RESET_REG_MISC_INT_ENABLE);
	__raw_writel(t | (1 << irq), base + AR71XX_RESET_REG_MISC_INT_ENABLE);

	/* flush write */
	(void) __raw_readl(base + AR71XX_RESET_REG_MISC_INT_ENABLE);
}

static void ar71xx_misc_irq_mask(unsigned int irq)
{
	void __iomem *base = ar71xx_reset_base;
	u32 t;

	irq -= AR71XX_MISC_IRQ_BASE;

	t = __raw_readl(base + AR71XX_RESET_REG_MISC_INT_ENABLE);
	__raw_writel(t & ~(1 << irq), base + AR71XX_RESET_REG_MISC_INT_ENABLE);

	/* flush write */
	(void) __raw_readl(base + AR71XX_RESET_REG_MISC_INT_ENABLE);
}

static void ar724x_misc_irq_ack(unsigned int irq)
{
	void __iomem *base = ar71xx_reset_base;
	u32 t;

	irq -= AR71XX_MISC_IRQ_BASE;

	t = __raw_readl(base + AR71XX_RESET_REG_MISC_INT_STATUS);
	__raw_writel(t & ~(1 << irq), base + AR71XX_RESET_REG_MISC_INT_STATUS);

	/* flush write */
	(void) __raw_readl(base + AR71XX_RESET_REG_MISC_INT_STATUS);
}

static struct irq_chip ar71xx_misc_irq_chip = {
	.name		= "AR71XX MISC",
	.unmask		= ar71xx_misc_irq_unmask,
	.mask		= ar71xx_misc_irq_mask,
};

static struct irqaction ar71xx_misc_irqaction = {
	.handler	= no_action,
	.name		= "cascade [AR71XX MISC]",
};

static void __init ar71xx_misc_irq_init(void)
{
	void __iomem *base = ar71xx_reset_base;
	int i;

	__raw_writel(0, base + AR71XX_RESET_REG_MISC_INT_ENABLE);
	__raw_writel(0, base + AR71XX_RESET_REG_MISC_INT_STATUS);

	switch (ar71xx_soc) {
	case AR71XX_SOC_AR7240:
	case AR71XX_SOC_AR7241:
	case AR71XX_SOC_AR7242:
	case AR71XX_SOC_AR9341:
	case AR71XX_SOC_AR9342:
	case AR71XX_SOC_AR9344:
		ar71xx_misc_irq_chip.ack = ar724x_misc_irq_ack;
		break;
	default:
		ar71xx_misc_irq_chip.mask_ack = ar71xx_misc_irq_mask;
		break;
	}

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

	else if (pending & STATUSF_IP2) {
		/*
		 * This IRQ is meant for a PCI device. Drivers for PCI devices
		 * typically allocate coherent DMA memory for the descriptor
		 * ring, however the DMA controller may still have some
		 * unsynchronized data in the FIFO.
		 * Issue a flush here to ensure that the driver sees the update.
		 */
		ar71xx_ddr_flush(ip2_flush_reg);
		do_IRQ(AR71XX_CPU_IRQ_IP2);
	}

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
	switch (ar71xx_soc) {
	case AR71XX_SOC_AR7240:
	case AR71XX_SOC_AR7241:
	case AR71XX_SOC_AR7242:
		ip2_flush_reg = AR724X_DDR_REG_FLUSH_PCIE;
		break;
	case AR71XX_SOC_AR9130:
	case AR71XX_SOC_AR9132:
		ip2_flush_reg = AR91XX_DDR_REG_FLUSH_WMAC;
		break;
	case AR71XX_SOC_AR9341:
	case AR71XX_SOC_AR9342:
	case AR71XX_SOC_AR9344:
		ip2_flush_reg = AR934X_DDR_REG_FLUSH_PCIE;
		break;

	default:
		ip2_flush_reg = AR71XX_DDR_REG_FLUSH_PCI;
		break;
	}

	mips_cpu_irq_init();

	ar71xx_misc_irq_init();

	cp0_perfcount_irq = AR71XX_MISC_IRQ_PERFC;

	ar71xx_gpio_irq_init();
}

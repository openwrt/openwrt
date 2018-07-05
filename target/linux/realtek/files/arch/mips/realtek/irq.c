/*
 *  Realtek RLX based SoC specific interrupt handling
 *
 *  Copyright (C) 2017 Weijie Gao <hackpascal@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/irqchip.h>

#include <asm/irq_cpu.h>
#include <asm/mipsregs.h>
#include <asm/c-lexra.h>

#include <asm/mach-realtek/realtek.h>
#include <asm/mach-realtek/platform.h>
#include "common.h"

static void __iomem *realtek_intctl_base;

static u32 mips_chip_bits;

static void realtek_soc_irq_handler(struct irq_desc *desc)
{
	u32 pending;

	pending = __raw_readl(realtek_intctl_base + REALTEK_INTCTL_REG_MASK) &
		  __raw_readl(realtek_intctl_base + REALTEK_INTCTL_REG_STATUS);

	if (pending & mips_chip_bits) {
		/*
		 * interrupts routed to mips core found here
		 * clear these bits as they can't be handled here
		 */
		__raw_writel(mips_chip_bits, realtek_intctl_base + REALTEK_INTCTL_REG_STATUS);
		pending &= ~mips_chip_bits;

		if (!pending)
			return;
	}

	if (!pending) {
		spurious_interrupt();
		return;
	}

	while (pending) {
		int bit = __ffs(pending);

		generic_handle_irq(REALTEK_SOC_IRQ(bit));
		pending &= ~BIT(bit);
	}
}

static void realtek_soc_irq_unmask(struct irq_data *d)
{
	unsigned int irq = d->irq - REALTEK_SOC_IRQ_BASE;
	u32 t;

	t = __raw_readl(realtek_intctl_base + REALTEK_INTCTL_REG_MASK);
	__raw_writel(t | (1 << irq), realtek_intctl_base + REALTEK_INTCTL_REG_MASK);

	/* flush write */
	__raw_readl(realtek_intctl_base + REALTEK_INTCTL_REG_MASK);
}

static void realtek_soc_irq_mask(struct irq_data *d)
{
	unsigned int irq = d->irq - REALTEK_SOC_IRQ_BASE;
	u32 t;

	t = __raw_readl(realtek_intctl_base + REALTEK_INTCTL_REG_MASK);
	__raw_writel(t & ~(1 << irq), realtek_intctl_base + REALTEK_INTCTL_REG_MASK);

	/* flush write */
	__raw_readl(realtek_intctl_base + REALTEK_INTCTL_REG_MASK);
}

static void realtek_soc_irq_ack(struct irq_data *d)
{
}

static struct irq_chip realtek_misc_irq_chip = {
	.name		= "soc",
	.irq_unmask	= realtek_soc_irq_unmask,
	.irq_mask	= realtek_soc_irq_mask,
	.irq_ack	= realtek_soc_irq_ack,
};

static void __init rtl8196c_irq_init(void)
{
	mips_chip_bits = RTL8196C_INTCTL_IRQ_TC0 | RTL8196C_INTCTL_IRQ_USB |
		RTL8196C_INTCTL_IRQ_PCIE | RTL8196C_INTCTL_IRQ_SWCORE;

	__raw_writel(mips_chip_bits, realtek_intctl_base + REALTEK_INTCTL_REG_MASK);

	__raw_writel(2 << RTL8196C_INTCTL_IRR0_UART_SHIFT,
		realtek_intctl_base + REALTEK_INTCTL_REG_IRR0);
		
	__raw_writel((7 << RTL8196C_INTCTL_IRR1_TC0_SHIFT) |
		(2 << RTL8196C_INTCTL_IRR1_GPIO_SHIFT) |
		(6 << RTL8196C_INTCTL_IRR1_SWCORE_SHIFT) |
		(5 << RTL8196C_INTCTL_IRR1_PCIE_SHIFT),
		realtek_intctl_base + REALTEK_INTCTL_REG_IRR1);

	__raw_writel(4 << RTL8196C_INTCTL_IRR2_USB_HOST_SHIFT,
		realtek_intctl_base + REALTEK_INTCTL_REG_IRR2);
}

static void __init realtek_soc_irq_init(void)
{
	int i;
	
	realtek_intctl_base = ioremap_nocache(REALTEK_INTCTL_BASE, REALTEK_INTCTL_SIZE);

	__raw_writel(0, realtek_intctl_base + REALTEK_INTCTL_REG_MASK);
	
	if (soc_is_rtl8196c())
		rtl8196c_irq_init();

	for (i = REALTEK_SOC_IRQ_BASE;
	     i < REALTEK_SOC_IRQ_BASE + REALTEK_SOC_IRQ_COUNT; i++) {
		irq_set_chip_and_handler(i, &realtek_misc_irq_chip,
					 handle_level_irq);
	}

	irq_set_chained_handler(REALTEK_CPU_IRQ(2), realtek_soc_irq_handler);
}

asmlinkage void plat_irq_dispatch(void)
{
	unsigned long pending;
	int irq;

	pending = read_c0_status() & read_c0_cause() & ST0_IM;

	if (!pending) {
		spurious_interrupt();
		return;
	}

	pending >>= CAUSEB_IP;
	while (pending) {
		irq = fls(pending) - 1;
		do_IRQ(MIPS_CPU_IRQ_BASE + irq);
		pending &= ~BIT(irq);
	}
}

void __init arch_init_irq(void)
{
	mips_cpu_irq_init();
	realtek_soc_irq_init();
}

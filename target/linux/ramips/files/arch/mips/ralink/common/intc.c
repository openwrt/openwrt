/*
 * Ralink SoC Interrupt controller routines
 *
 * Copyright (C) 2009 Gabor Juhos <juhosg@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/bitops.h>

#include <asm/irq_cpu.h>
#include <asm/mipsregs.h>

#include <asm/mach-ralink/common.h>

/* INTC register offsets */
#define INTC_REG_STATUS0	0x00
#define INTC_REG_STATUS1	0x04
#define INTC_REG_TYPE		0x20
#define INTC_REG_RAW_STATUS	0x30
#define INTC_REG_ENABLE		0x34
#define INTC_REG_DISABLE	0x38

#define INTC_INT_GLOBAL		BIT(31)
#define INTC_IRQ_COUNT		32

static unsigned int ramips_intc_irq_base;
static void __iomem *ramips_intc_base;

static inline void ramips_intc_wr(u32 val, unsigned reg)
{
	__raw_writel(val, ramips_intc_base + reg);
}

static inline u32 ramips_intc_rr(unsigned reg)
{
	return __raw_readl(ramips_intc_base + reg);
}

static void ramips_intc_irq_unmask(unsigned int irq)
{
	irq -= ramips_intc_irq_base;
	ramips_intc_wr((1 << irq), INTC_REG_ENABLE);
}

static void ramips_intc_irq_mask(unsigned int irq)
{
	irq -= ramips_intc_irq_base;
	ramips_intc_wr((1 << irq), INTC_REG_DISABLE);
}

static struct irq_chip ramips_intc_irq_chip = {
	.name		= "INTC",
	.unmask		= ramips_intc_irq_unmask,
	.mask		= ramips_intc_irq_mask,
	.mask_ack	= ramips_intc_irq_mask,
};

static struct irqaction ramips_intc_irqaction = {
	.handler	= no_action,
	.name		= "cascade [INTC]",
};

void __init ramips_intc_irq_init(unsigned intc_base, unsigned irq,
				 unsigned irq_base)
{
	int i;

	ramips_intc_base = ioremap_nocache(intc_base, PAGE_SIZE);
	ramips_intc_irq_base = irq_base;

	/* disable all interrupts */
	ramips_intc_wr(~0, INTC_REG_DISABLE);

	/* route all INTC interrupts to MIPS HW0 interrupt */
	ramips_intc_wr(0, INTC_REG_TYPE);

	for (i = ramips_intc_irq_base;
	     i < ramips_intc_irq_base + INTC_IRQ_COUNT; i++) {
		set_irq_chip_and_handler(i, &ramips_intc_irq_chip,
					 handle_level_irq);
	}

	setup_irq(irq, &ramips_intc_irqaction);
	ramips_intc_wr(INTC_INT_GLOBAL, INTC_REG_ENABLE);
}

u32 ramips_intc_get_status(void)
{
	return ramips_intc_rr(INTC_REG_STATUS0);
}

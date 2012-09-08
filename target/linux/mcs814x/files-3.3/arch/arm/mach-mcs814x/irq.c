/*
 * Moschip MCS814x generic interrupt controller routines
 *
 * Copyright (C) 2012, Florian Fainelli <florian@openwrt.org>
 *
 * Licensed under the GPLv2
 */
#include <linux/init.h>
#include <linux/irq.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/irqdomain.h>

#include <asm/exception.h>
#include <asm/mach/irq.h>
#include <mach/mcs814x.h>

void __iomem *mcs814x_intc_base;

static void __init mcs814x_alloc_gc(void __iomem *base, unsigned int irq_start,
					unsigned int num)
{
	struct irq_chip_generic *gc;
	struct irq_chip_type *ct;

	gc = irq_alloc_generic_chip("mcs814x-intc", 1,
			irq_start, base, handle_level_irq);
	if (!gc)
		panic("unable to allocate generic irq chip");

	ct = gc->chip_types;
	ct->chip.irq_ack = irq_gc_unmask_enable_reg;
	ct->chip.irq_mask = irq_gc_mask_clr_bit;
	ct->chip.irq_unmask = irq_gc_mask_set_bit;
	ct->regs.mask = MCS814X_IRQ_MASK;
	ct->regs.enable = MCS814X_IRQ_ICR;

	irq_setup_generic_chip(gc, IRQ_MSK(num), IRQ_GC_INIT_MASK_CACHE,
		IRQ_NOREQUEST, 0);

	/* Clear all interrupts */
	writel_relaxed(0xffffffff, base + MCS814X_IRQ_ICR);
}

static const struct of_device_id mcs814x_intc_ids[] = {
	{ .compatible = "moschip,mcs814x-intc" },
	{ /* sentinel */ },
};

void __init mcs814x_of_irq_init(void)
{
	struct device_node *np;

	np = of_find_matching_node(NULL, mcs814x_intc_ids);
	if (!np)
		panic("unable to find compatible intc node in dtb\n");

	mcs814x_intc_base = of_iomap(np, 0);
	if (!mcs814x_intc_base)
		panic("unable to map intc cpu registers\n");

	irq_domain_add_simple(np, 0);

	of_node_put(np);

	mcs814x_alloc_gc(mcs814x_intc_base, 0, 32);
}


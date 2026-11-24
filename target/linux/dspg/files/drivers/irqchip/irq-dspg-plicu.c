// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2012 DSPG Technologies GmbH
 * Copyright (C) 2026 John Crispin <john@phrozen.org>
 */

#include <linux/bitops.h>
#include <linux/io.h>
#include <linux/irq.h>
#include <linux/irqchip.h>
#include <linux/irqdomain.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/spinlock.h>

#include <asm/exception.h>

#define PLICU_CH_CLR_INT	0x00
#define PLICU_CH_EDGE_LEVEL	0x08
#define PLICU_CAUSE		0x2c
#define PLICU_SET_CH_MASK	0x3c
#define PLICU_CLR_CH_MASK	0x44

#define PLICU_IRQS_PER_BANK	32
#define PLICU_NUM_BANKS		2
#define PLICU_NUM_IRQS		(PLICU_IRQS_PER_BANK * PLICU_NUM_BANKS)

static void __iomem *plicu_base;
static struct irq_domain *plicu_domain;
static DEFINE_RAW_SPINLOCK(plicu_lock);

static void __iomem *plicu_reg(irq_hw_number_t hwirq, unsigned int reg)
{
	return plicu_base + reg + (hwirq / PLICU_IRQS_PER_BANK) * 4;
}

static u32 plicu_bit(irq_hw_number_t hwirq)
{
	return BIT(hwirq % PLICU_IRQS_PER_BANK);
}

static void plicu_ack(struct irq_data *d)
{
	writel_relaxed(plicu_bit(d->hwirq), plicu_reg(d->hwirq, PLICU_CH_CLR_INT));
}

static void plicu_mask(struct irq_data *d)
{
	writel_relaxed(plicu_bit(d->hwirq), plicu_reg(d->hwirq, PLICU_CLR_CH_MASK));
}

static void plicu_unmask(struct irq_data *d)
{
	writel_relaxed(plicu_bit(d->hwirq), plicu_reg(d->hwirq, PLICU_SET_CH_MASK));
}

static void plicu_edge_set(irq_hw_number_t hwirq, bool edge)
{
	void __iomem *reg = plicu_reg(hwirq, PLICU_CH_EDGE_LEVEL);
	unsigned long flags;
	u32 val;

	raw_spin_lock_irqsave(&plicu_lock, flags);
	val = readl_relaxed(reg);
	if (edge)
		val |= plicu_bit(hwirq);
	else
		val &= ~plicu_bit(hwirq);
	writel_relaxed(val, reg);
	raw_spin_unlock_irqrestore(&plicu_lock, flags);
}

static bool plicu_is_top(struct irq_data *d)
{
	return irq_desc_get_irq_data(irq_data_to_desc(d)) == d;
}

static int plicu_set_type(struct irq_data *d, unsigned int type)
{
	irq_flow_handler_t handler;

	switch (type & IRQ_TYPE_SENSE_MASK) {
	case IRQ_TYPE_EDGE_RISING:
		plicu_edge_set(d->hwirq, true);
		handler = handle_edge_irq;
		break;
	case IRQ_TYPE_LEVEL_HIGH:
		plicu_edge_set(d->hwirq, false);
		handler = handle_level_irq;
		break;
	default:
		return -EINVAL;
	}

	if (plicu_is_top(d))
		irq_set_handler_locked(d, handler);

	return 0;
}

static struct irq_chip plicu_chip = {
	.name		= "plicu",
	.irq_ack	= plicu_ack,
	.irq_mask	= plicu_mask,
	.irq_unmask	= plicu_unmask,
	.irq_set_type	= plicu_set_type,
	.flags		= IRQCHIP_SKIP_SET_WAKE,
};

static int plicu_irq_alloc(struct irq_domain *d, unsigned int irq,
			   unsigned int nr_irqs, void *data)
{
	struct irq_fwspec *fwspec = data;
	irq_hw_number_t hwirq;
	unsigned int type, i;
	int ret;

	ret = irq_domain_translate_twocell(d, fwspec, &hwirq, &type);
	if (ret)
		return ret;

	if (hwirq + nr_irqs > PLICU_NUM_IRQS)
		return -EINVAL;

	for (i = 0; i < nr_irqs; i++) {
		plicu_edge_set(hwirq + i, false);
		irq_domain_set_hwirq_and_chip(d, irq + i, hwirq + i,
					      &plicu_chip, NULL);
		if (irq_get_irq_data(irq + i)->domain != d)
			continue;

		irq_set_handler(irq + i, handle_level_irq);
		irq_set_probe(irq + i);
	}

	return 0;
}

static const struct irq_domain_ops plicu_domain_ops = {
	.translate	= irq_domain_translate_twocell,
	.alloc		= plicu_irq_alloc,
	.free		= irq_domain_free_irqs_common,
};

static void __exception_irq_entry plicu_handle_irq(struct pt_regs *regs)
{
	unsigned int bank;
	bool handled;
	u32 stat;

	do {
		handled = false;

		for (bank = 0; bank < PLICU_NUM_BANKS; bank++) {
			stat = readl_relaxed(plicu_base + PLICU_CAUSE + bank * 4);
			handled |= !!stat;

			while (stat) {
				unsigned int bit = __ffs(stat);

				generic_handle_domain_irq(plicu_domain,
						bank * PLICU_IRQS_PER_BANK + bit);
				stat &= ~BIT(bit);
			}
		}
	} while (handled);
}

static int __init plicu_init(struct device_node *np, struct device_node *parent)
{
	unsigned int bank;

	if (parent)
		return -EINVAL;

	plicu_base = of_iomap(np, 0);
	if (!plicu_base)
		return -ENOMEM;

	for (bank = 0; bank < PLICU_NUM_BANKS; bank++) {
		writel_relaxed(~0, plicu_base + PLICU_CLR_CH_MASK + bank * 4);
		writel_relaxed(~0, plicu_base + PLICU_CH_CLR_INT + bank * 4);
	}

	plicu_domain = irq_domain_create_linear(of_fwnode_handle(np),
						PLICU_NUM_IRQS,
						&plicu_domain_ops, NULL);
	if (!plicu_domain) {
		iounmap(plicu_base);
		return -ENOMEM;
	}

	set_handle_irq(plicu_handle_irq);

	return 0;
}
IRQCHIP_DECLARE(dspg_plicu, "dspg,plicu", plicu_init);

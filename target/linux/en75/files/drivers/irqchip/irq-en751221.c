// SPDX-License-Identifier: GPL-2.0-only
/*
 * EN751221 Interrupt Controller Driver.
 *
 * This is a simple interrupt controller with a single bit for each interrupt
 * which must be set to unmask. It has basic support for 34Kc MT SMP with
 * 2 VPEs. Each interrupt can be routed to one VPE or the other but not both.
 * To support per-cpu IRQs, a second IRQ number is allocated for the purpose of
 * masking and unmasking the interrupt on VPE#1. We call this a
 * "shadow interrupt" because it only exists for the purpose  of controlling
 * the second VPE's view of its parent interrupt.
 *
 * Assignment of shadow interrupts is done by the SoC integrater as with all
 * other interrupt numbers. If you have an SoC with a shadow interrupt, for
 * example IRQ 30 shadows IRQ 31, put in your DTS file:
 *
 * econet,shadow-interrupts = <30 31>;
 *
 * Then your driver requiring a per-cpu interrupt should request IRQ 31 as it
 * normally would. You can put as many pairs as you want, e.g. <30 31 3 8>.
 *
 * Copyright (C) 2025 Caleb James DeLisle <cjd@cjdns.fr>
 */

#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/irqdomain.h>
#include <linux/irqchip.h>
#include <linux/irqchip/chained_irq.h>

#define EN75_INTC_IRQ_COUNT	40

#define INTC_NO_SHADOW		0xff
#define INTC_IS_SHADOW		0xfe
static_assert(EN75_INTC_IRQ_COUNT < INTC_IS_SHADOW);

#define REG_MASK0		0x04
#define REG_MASK1		0x50
#define REG_PENDING0		0x08
#define REG_PENDING1		0x54

static const struct en75_intc {
	const struct irq_chip chip;

	const struct irq_domain_ops domain_ops;
} en75_intc;

static struct {
	void __iomem *membase;
	u8 shadow_interrupts[EN75_INTC_IRQ_COUNT];
} en75_intc_rai __ro_after_init;

static DEFINE_RAW_SPINLOCK(irq_lock);

static void en75_wreg(u32 reg, u32 val, u32 mask)
{
	unsigned long flags;
	u32 v;

	raw_spin_lock_irqsave(&irq_lock, flags);

	v = ioread32(en75_intc_rai.membase + reg);
	v &= ~mask;
	v |= val & mask;
	iowrite32(v, en75_intc_rai.membase + reg);

	raw_spin_unlock_irqrestore(&irq_lock, flags);
}

static void en75_chmask(u32 hwirq, bool unmask)
{
	u32 reg;
	u32 mask;
	u32 bit;
	u8 shadow;

	shadow = en75_intc_rai.shadow_interrupts[hwirq];
	if (WARN_ON_ONCE(shadow == INTC_IS_SHADOW))
		return;
	else if (shadow < INTC_NO_SHADOW && smp_processor_id() > 0)
		hwirq = shadow;

	if (hwirq > 32) {
		reg = REG_MASK1;
		mask = BIT(hwirq - 33);
	} else {
		reg = REG_MASK0;
		mask = BIT(hwirq - 1);
	}
	bit = (unmask) ? mask : 0;

	en75_wreg(reg, bit, mask);
}

static void en75_intc_mask(struct irq_data *d)
{
	en75_chmask(d->hwirq, false);
}

static void en75_intc_unmask(struct irq_data *d)
{
	en75_chmask(d->hwirq, true);
}

static void en75_mask_all(void)
{
	en75_wreg(REG_MASK0, 0, ~0);
	en75_wreg(REG_MASK1, 0, ~0);
}

static void en75_intc_handle_pending(struct irq_domain *d, u32 pending, u32 offset)
{
	int hwirq;

	while (pending) {
		hwirq = fls(pending) - 1;
		// pr_info("Handle IRQ %d on CPU %d\n", hwirq + offset, smp_processor_id());
		generic_handle_domain_irq(d, hwirq + offset);
		pending &= ~BIT(hwirq);
	}
}

static void en75_intc_from_parent(struct irq_desc *desc)
{
	struct irq_chip *chip = irq_desc_get_chip(desc);
	struct irq_domain *domain;
	u32 pending0;
	u32 pending1;

	chained_irq_enter(chip, desc);

	pending0 = ioread32(en75_intc_rai.membase + REG_PENDING0);
	pending1 = ioread32(en75_intc_rai.membase + REG_PENDING1);

	if (unlikely(!(pending0 | pending1))) {
		spurious_interrupt();
		goto out;
	}

	domain = irq_desc_get_handler_data(desc);

	en75_intc_handle_pending(domain, pending0, 1);
	en75_intc_handle_pending(domain, pending1, 32);

out:
	chained_irq_exit(chip, desc);
}

static int en75_intc_map(struct irq_domain *d, u32 irq, irq_hw_number_t hwirq)
{
	int ret;

	if (hwirq >= EN75_INTC_IRQ_COUNT) {
		pr_err("%s: hwirq %lu out of range\n", __func__, hwirq);
		return -EINVAL;
	} else if (en75_intc_rai.shadow_interrupts[hwirq] == INTC_IS_SHADOW) {
		pr_err("%s: can't map hwirq %lu, it is a shadow interrupt\n",
		       __func__, hwirq);
		return -EINVAL;
	}
	if (en75_intc_rai.shadow_interrupts[hwirq] != INTC_NO_SHADOW) {
		irq_set_chip_and_handler(
			irq, &en75_intc.chip, handle_percpu_devid_irq);
		ret = irq_set_percpu_devid(irq);
		if (ret) {
			pr_warn("%s: Failed irq_set_percpu_devid for %u: %d\n",
				d->name, irq, ret);
		}
	} else {
		irq_set_chip_and_handler(
			irq, &en75_intc.chip, handle_level_irq);
	}
	irq_set_chip_data(irq, NULL);
	return 0;
}

static const struct en75_intc en75_intc = {
	.chip = {
		.name		= "en75-intc",
		.irq_unmask	= en75_intc_unmask,
		.irq_mask	= en75_intc_mask,
		.irq_mask_ack	= en75_intc_mask,
	},
	.domain_ops = {
		.xlate = irq_domain_xlate_onecell,
		.map = en75_intc_map,
	},
};

static int __init get_shadow_interrupts(struct device_node *node)
{
	const char *field = "econet,shadow-interrupts";
	int n_shadow_interrupts;
	u32 *shadow_interrupts;

	n_shadow_interrupts = of_property_count_u32_elems(node, field);
	memset(en75_intc_rai.shadow_interrupts, INTC_NO_SHADOW,
	       sizeof(en75_intc_rai.shadow_interrupts));
	if (n_shadow_interrupts <= 0) {
		return 0;
	} else if (n_shadow_interrupts % 2) {
		pr_err("%pOF: %s count is odd, ignoring\n", node, field);
		return 0;
	}
	shadow_interrupts = kmalloc_array(n_shadow_interrupts, sizeof(u32),
					  GFP_KERNEL);
	if (!shadow_interrupts)
		return -ENOMEM;
	if (of_property_read_u32_array(node, field,
				       shadow_interrupts, n_shadow_interrupts)
	) {
		pr_err("%pOF: Failed to read %s\n", node, field);
		kfree(shadow_interrupts);
		return -EINVAL;
	}
	for (int i = 0; i < n_shadow_interrupts; i += 2) {
		u32 shadow = shadow_interrupts[i];
		u32 target = shadow_interrupts[i + 1];

		if (shadow > EN75_INTC_IRQ_COUNT) {
			pr_err("%pOF: %s[%d] shadow(%d) out of range\n",
			       node, field, i, shadow);
			continue;
		}
		if (target >= EN75_INTC_IRQ_COUNT) {
			pr_err("%pOF: %s[%d] target(%d) out of range\n",
			       node, field, i + 1, target);
			continue;
		}
		en75_intc_rai.shadow_interrupts[target] = shadow;
		en75_intc_rai.shadow_interrupts[shadow] = INTC_IS_SHADOW;
	}
	kfree(shadow_interrupts);
	return 0;
}

static int __init en75_intc_of_init(struct device_node *node, struct device_node *parent)
{
	int ret;
	int irq;
	struct resource res;
	struct irq_domain *domain;

	pr_info("%pOF: Init\n", node);

	ret = get_shadow_interrupts(node);
	if (ret)
		return ret;

	irq = irq_of_parse_and_map(node, 0);
	if (!irq) {
		pr_err("%pOF: DT: Failed to get IRQ from 'interrupts'\n", node);
		return -EINVAL;
	}

	if (of_address_to_resource(node, 0, &res)) {
		pr_err("%pOF: DT: Failed to get 'reg'\n", node);
		ret = -EINVAL;
		goto err_dispose_mapping;
	}

	if (!request_mem_region(res.start, resource_size(&res), res.name)) {
		pr_err("%pOF: Failed to request memory\n", node);
		ret = -EBUSY;
		goto err_dispose_mapping;
	}

	en75_intc_rai.membase = ioremap(res.start, resource_size(&res));
	if (!en75_intc_rai.membase) {
		pr_err("%pOF: Failed to remap membase\n", node);
		ret = -ENOMEM;
		goto err_release;
	}

	en75_mask_all();

	domain = irq_domain_add_linear(
		node, EN75_INTC_IRQ_COUNT,
		&en75_intc.domain_ops, NULL);
	if (!domain) {
		pr_err("%pOF: Failed to add irqdomain\n", node);
		ret = -ENOMEM;
		goto err_unmap;
	}

	irq_set_chained_handler_and_data(irq, en75_intc_from_parent, domain);

	return 0;

err_unmap:
	iounmap(en75_intc_rai.membase);
err_release:
	release_mem_region(res.start, resource_size(&res));
err_dispose_mapping:
	irq_dispose_mapping(irq);
	return ret;
}

IRQCHIP_DECLARE(en75_en751221_intc, "econet,en751221-intc", en75_intc_of_init);

// SPDX-License-Identifier: GPL-2.0
#include <linux/io.h>
#include <linux/irq.h>
#include <linux/irqchip.h>
#include <linux/irqchip/chained_irq.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>

#define	REGBUS_IRQ_NUM	(15)
#define	REGBUS_IRQ_MASK	((0x01UL << REGBUS_IRQ_NUM) - 1)

#define	OFFS_INT_STAT	(0x00)
#define	OFFS_INT_ENABLE	(0x04)

static void cs75xx_intc_irq_handler(struct irq_desc *desc)
{
	struct irq_domain *domain;
	struct irq_chip_generic *gc;
	struct irq_chip *chip;
	u32 irq_stat;

	chip = irq_desc_get_chip(desc);
	domain = irq_desc_get_handler_data(desc);
	gc = irq_get_domain_generic_chip(domain, 0);

	chained_irq_enter(chip, desc);

	irq_stat = readl(gc->reg_base + OFFS_INT_STAT);
	irq_stat &= readl(gc->reg_base + OFFS_INT_ENABLE) & REGBUS_IRQ_MASK;

	while (irq_stat) {
		u32 int_no = __fls(irq_stat);

		generic_handle_domain_irq(domain, gc->irq_base + int_no);
		irq_stat &= ~(0x01UL << int_no);
	}

	chained_irq_exit(chip, desc);
}

static int __init cs75xx_irq_init(struct device_node *np, struct device_node *np_parent)
{
	struct resource r;
	int irq;
	void __iomem *reg_base;
	int res;
	struct irq_domain *domain;
	struct irq_chip_generic *gc;
	const unsigned int _CLR = IRQ_NOREQUEST | IRQ_NOPROBE | IRQ_NOAUTOEN;
	const unsigned int _SET = IRQ_LEVEL;

	irq = irq_of_parse_and_map(np, 0);
	if (irq <= 0) {
		pr_err("%s: could not parse irq.\n", np->name);
		return -EINVAL;
	}

	res = of_address_to_resource(np, 0, &r);
	if (res) {
		pr_err("%s: could not get resource.\n", np->name);
		return res;
	}
	if (request_mem_region(r.start, resource_size(&r), np->name) == NULL) {
		pr_err("%s: request_mem_region() failed.\n", np->name);
		return -ENOMEM;
	}
	reg_base = ioremap(r.start, resource_size(&r));
	if (reg_base == NULL) {
		pr_err("%s: ioremap() failed.\n", np->name);
		return -ENOMEM;
	}

	domain = irq_domain_add_linear(np, REGBUS_IRQ_NUM, &irq_generic_chip_ops, NULL);
	if (domain == NULL) {
		pr_err("%s: could not add irq_domain.\n", np->name);
		res = -ENOMEM;
		goto l_unmap;
	}

	res = irq_alloc_domain_generic_chips(domain, REGBUS_IRQ_NUM, 1, "REGBUS",
				handle_level_irq, _CLR, _SET, IRQ_GC_INIT_MASK_CACHE);
	if (res) {
		pr_err("%s: could not alloc irq_domain and gc.\n", np->name);
		goto l_unmap;
	}

	gc = irq_get_domain_generic_chip(domain, 0);
	gc->reg_base = reg_base;
	gc->chip_types[0].regs.mask = OFFS_INT_ENABLE;
	gc->chip_types[0].chip.irq_mask = irq_gc_mask_clr_bit;
	gc->chip_types[0].chip.irq_unmask = irq_gc_mask_set_bit;

	writel(0x00000000UL, gc->reg_base + OFFS_INT_ENABLE);

	irq_set_chained_handler_and_data(irq, cs75xx_intc_irq_handler, domain);

	return 0;

l_unmap:
	iounmap(reg_base);
	return res;
}
IRQCHIP_DECLARE(cs75xx_intc, "cortina,cs75xx-intc", cs75xx_irq_init);

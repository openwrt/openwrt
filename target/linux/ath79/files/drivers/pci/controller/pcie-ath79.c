// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Atheros ATH79 PCIe host controller driver
 *
 *  Copyright (C) 2011 René Bolldorf <xsecute@googlemail.com>
 *  Copyright (C) 2009-2011 Gabor Juhos <juhosg@openwrt.org>
 */

#include <linux/init.h>
#include <linux/iopoll.h>
#include <linux/irq.h>
#include <linux/irqchip/chained_irq.h>
#include <linux/irqdomain.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/of_pci.h>
#include <linux/pci.h>
#include <linux/platform_device.h>
#include <linux/reset.h>

#include <asm/mach-ath79/ar71xx_regs.h>
#include <asm/mach-ath79/ath79.h>

#define ATH79_PCIE_REG_APP		0x00
#define ATH79_PCIE_REG_RESET		0x18
#define ATH79_PCIE_REG_INT_STATUS	0x4c
#define ATH79_PCIE_REG_INT_MASK		0x50

#define ATH79_PCIE_APP_LTSSM_ENABLE	BIT(0)

#define ATH79_PCIE_RESET_EP_RESET_L	BIT(2)
#define ATH79_PCIE_RESET_LINK_UP	BIT(0)

#define ATH79_PCIE_INT_DEV0		BIT(14)

#define ATH79_PCIE_IRQ_COUNT		4

#define AR7240_BAR0_WAR_VALUE		0xffff

#define ATH79_PCIE_CMD_INIT	(PCI_COMMAND_MEMORY |		\
				 PCI_COMMAND_MASTER |		\
				 PCI_COMMAND_PARITY |		\
				 PCI_COMMAND_SERR)

#define ATH79_PLL_PCIE_CONFIG_PLL_BYPASS	BIT(16)
#define ATH79_PLL_PCIE_CONFIG_PLL_PWD		BIT(30)

#define QCA955X_PLL_PCIE_CONFIG_REG		0x0c

struct ath79_pcie_controller {
	void __iomem *devcfg_base;
	void __iomem *ctrl_base;
	void __iomem *crp_base;

	struct device *dev;
	int irq;

	bool link_up;
	bool bar0_is_cached;
	u32  bar0_value;

	struct irq_domain *domain;

	struct reset_control *hc_reset;
	struct reset_control *phy_reset;

	raw_spinlock_t lock;
	unsigned int unmask_count;
};

static inline bool ath79_pcie_check_link(struct ath79_pcie_controller *apc)
{
	u32 reset;

	reset = readl_relaxed(apc->ctrl_base + ATH79_PCIE_REG_RESET);
	return reset & ATH79_PCIE_RESET_LINK_UP;
}

static int ath79_pcie_rmw(void __iomem *base, int where, int size, u32 value)
{
	u32 data;
	int shift;

	if (size != 1 && size != 2 && size != 4)
		return PCIBIOS_BAD_REGISTER_NUMBER;

	data = readl_relaxed(base + (where & ~3));

	switch (size) {
	case 1:
		shift = (where & 3) * 8;
		data &= ~GENMASK(shift + 7, shift);
		data |= (value & 0xff) << shift;
		break;
	case 2:
		shift = (where & 2) * 8;
		data &= ~GENMASK(shift + 15, shift);
		data |= (value & 0xffff) << shift;
		break;
	case 4:
		data = value;
		break;
	default:
		return PCIBIOS_BAD_REGISTER_NUMBER;
	}

	writel_relaxed(data, base + (where & ~3));
	wmb();
	/* flush write */
	readl_relaxed(base + (where & ~3));

	return PCIBIOS_SUCCESSFUL;
}

static int ath79_pcie_read(struct pci_bus *bus, unsigned int devfn, int where,
			    int size, uint32_t *value)
{
	struct ath79_pcie_controller *apc = bus->sysdata;
	void __iomem *base;
	u32 data;

	if (!apc->link_up)
		return PCIBIOS_DEVICE_NOT_FOUND;

	if (devfn)
		return PCIBIOS_DEVICE_NOT_FOUND;

	base = apc->devcfg_base;
	data = readl_relaxed(base + (where & ~3));

	switch (size) {
	case 1:
		data >>= (where & 3) * 8;
		data &= 0xff;
		break;
	case 2:
		data >>= (where & 2) * 8;
		data &= 0xffff;
		break;
	case 4:
		break;
	default:
		return PCIBIOS_BAD_REGISTER_NUMBER;
	}

	if (where == PCI_BASE_ADDRESS_0 && size == 4 &&
	    apc->bar0_is_cached) {
		/* use the cached value */
		*value = apc->bar0_value;
	} else {
		*value = data;
	}

	return PCIBIOS_SUCCESSFUL;
}

static int ath79_pcie_write(struct pci_bus *bus, unsigned int devfn, int where,
			     int size, uint32_t value)
{
	struct ath79_pcie_controller *apc = bus->sysdata;

	if (!apc->link_up)
		return PCIBIOS_DEVICE_NOT_FOUND;

	if (devfn)
		return PCIBIOS_DEVICE_NOT_FOUND;

	if (soc_is_ar7240() && where == PCI_BASE_ADDRESS_0 && size == 4) {
		if (value != 0xffffffff) {
			/*
			 * WAR for a hw issue. If the BAR0 register of the
			 * device is set to the proper base address, the
			 * memory space of the device is not accessible.
			 *
			 * Cache the intended value so it can be read back,
			 * and write a SoC specific constant value to the
			 * BAR0 register in order to make the device memory
			 * accessible.
			 */
			apc->bar0_is_cached = true;
			apc->bar0_value = value;

			value = AR7240_BAR0_WAR_VALUE;
		} else {
			apc->bar0_is_cached = false;
		}
	}

	return ath79_pcie_rmw(apc->devcfg_base, where, size, value);
}

static int ath79_pcie_local_write(struct ath79_pcie_controller *apc,
				  int where, int size, u32 value)
{
	if (!apc->link_up)
		return PCIBIOS_DEVICE_NOT_FOUND;

	WARN_ON(where & (size - 1));

	return ath79_pcie_rmw(apc->crp_base, where, size, value);
}

static struct pci_ops ath79_pcie_ops = {
	.read	= ath79_pcie_read,
	.write	= ath79_pcie_write,
};

static void ath79_pcie_irq_handler(struct irq_desc *desc)
{
	struct irq_chip *chip = irq_desc_get_chip(desc);
	struct ath79_pcie_controller *apc = irq_desc_get_handler_data(desc);
	u32 pending;

	chained_irq_enter(chip, desc);
	pending = readl_relaxed(apc->ctrl_base + ATH79_PCIE_REG_INT_STATUS) &
		  readl_relaxed(apc->ctrl_base + ATH79_PCIE_REG_INT_MASK);

	if (pending & ATH79_PCIE_INT_DEV0) {
		int hwirq;

		/*
		 * Ack the summary bit before processing so the controller can
		 * re-latch any new assertion that arrives during the fan-out.
		 */
		writel_relaxed(ATH79_PCIE_INT_DEV0,
			       apc->ctrl_base + ATH79_PCIE_REG_INT_STATUS);
		wmb();
		readl_relaxed(apc->ctrl_base + ATH79_PCIE_REG_INT_STATUS);

		/*
		 * The controller collapses all legacy INTx interrupts into a
		 * single summary bit (ATH79_PCIE_INT_DEV0) with no per-pin
		 * status reporting.  Fan out only to INTx hwirqs that have
		 * a mapping so that mask/unmask on the summary bit stay
		 * balanced: for an unrequested line handle_level_irq() calls
		 * mask_ack_irq() but returns early via irq_can_handle()
		 * without cond_unmask_irq(), which would corrupt the refcount.
		 */
		for (hwirq = 0; hwirq < ATH79_PCIE_IRQ_COUNT; hwirq++) {
			unsigned int virq = irq_find_mapping(apc->domain, hwirq);

			if (virq)
				generic_handle_irq(virq);
		}
	} else {
		dev_warn_ratelimited(apc->dev, "unexpected PCIe interrupt\n");
	}
	chained_irq_exit(chip, desc);
}

static void ath79_pcie_irq_unmask(struct irq_data *d)
{
	struct ath79_pcie_controller *apc = irq_data_get_irq_chip_data(d);
	unsigned long flags;

	raw_spin_lock_irqsave(&apc->lock, flags);
	if (apc->unmask_count++ == 0) {
		void __iomem *base = apc->ctrl_base;
		u32 t;

		t = readl_relaxed(base + ATH79_PCIE_REG_INT_MASK);
		writel_relaxed(t | ATH79_PCIE_INT_DEV0, base + ATH79_PCIE_REG_INT_MASK);
		wmb();
		/* flush write */
		readl_relaxed(base + ATH79_PCIE_REG_INT_MASK);
	}
	raw_spin_unlock_irqrestore(&apc->lock, flags);
}

static void ath79_pcie_irq_mask(struct irq_data *d)
{
	struct ath79_pcie_controller *apc = irq_data_get_irq_chip_data(d);
	unsigned long flags;

	raw_spin_lock_irqsave(&apc->lock, flags);
	if (--apc->unmask_count == 0) {
		void __iomem *base = apc->ctrl_base;
		u32 t;

		t = readl_relaxed(base + ATH79_PCIE_REG_INT_MASK);
		writel_relaxed(t & ~ATH79_PCIE_INT_DEV0, base + ATH79_PCIE_REG_INT_MASK);
		wmb();
		/* flush write */
		readl_relaxed(base + ATH79_PCIE_REG_INT_MASK);
	}
	raw_spin_unlock_irqrestore(&apc->lock, flags);
}

static void ath79_pcie_irq_ack(struct irq_data *d)
{
	struct ath79_pcie_controller *apc = irq_data_get_irq_chip_data(d);
	void __iomem *base = apc->ctrl_base;

	writel_relaxed(ATH79_PCIE_INT_DEV0, base + ATH79_PCIE_REG_INT_STATUS);
	wmb();
	/* flush write */
	readl_relaxed(base + ATH79_PCIE_REG_INT_STATUS);
}

static struct irq_chip ath79_pcie_irq_chip = {
	.name		= "ATH79 PCIE",
	.irq_mask	= ath79_pcie_irq_mask,
	.irq_unmask	= ath79_pcie_irq_unmask,
	.irq_ack	= ath79_pcie_irq_ack,
};

static int ath79_pcie_irq_map(struct irq_domain *d,
			      unsigned int irq, irq_hw_number_t hw)
{
	struct ath79_pcie_controller *apc = d->host_data;

	irq_set_chip_and_handler(irq, &ath79_pcie_irq_chip, handle_level_irq);
	irq_set_chip_data(irq, apc);

	return 0;
}

static const struct irq_domain_ops ath79_pcie_domain_ops = {
	.xlate = irq_domain_xlate_onecell,
	.map = ath79_pcie_irq_map,
};

static int ath79_pcie_irq_init(struct ath79_pcie_controller *apc)
{
	void __iomem *base = apc->ctrl_base;

	writel_relaxed(0, base + ATH79_PCIE_REG_INT_MASK);
	writel_relaxed(0, base + ATH79_PCIE_REG_INT_STATUS);

	apc->domain = irq_domain_create_linear(apc->dev->fwnode,
					       ATH79_PCIE_IRQ_COUNT,
					       &ath79_pcie_domain_ops, apc);
	if (!apc->domain)
		return -ENOMEM;

	irq_set_chained_handler_and_data(apc->irq, ath79_pcie_irq_handler, apc);

	return 0;
}

static int ath79_pcie_power_on(struct ath79_pcie_controller *apc)
{
	int ret;

	ret = reset_control_deassert(apc->hc_reset);
	if (ret) {
		dev_err(apc->dev, "failed to deassert hc reset: %d\n", ret);
		return ret;
	}

	ret = reset_control_deassert(apc->phy_reset);
	if (ret) {
		dev_err(apc->dev, "failed to deassert phy reset: %d\n", ret);
		return ret;
	}

	return 0;
}

static int ath79_pcie_init_phy(struct ath79_pcie_controller *apc, bool qca)
{
	u32 ppl;

	if (qca) {
		ppl = ath79_pll_rr(QCA955X_PLL_PCIE_CONFIG_REG);
		ppl &= ~ATH79_PLL_PCIE_CONFIG_PLL_PWD;
		ath79_pll_wr(QCA955X_PLL_PCIE_CONFIG_REG, ppl);

		ppl = ath79_pll_rr(QCA955X_PLL_PCIE_CONFIG_REG);
		ppl &= ~ATH79_PLL_PCIE_CONFIG_PLL_BYPASS;
		ath79_pll_wr(QCA955X_PLL_PCIE_CONFIG_REG, ppl);
	} else {
		ppl = ath79_pll_rr(AR724X_PLL_REG_PCIE_CONFIG);
		ppl &= ~(ATH79_PLL_PCIE_CONFIG_PLL_PWD |
			 AR724X_PLL_REG_PCIE_CONFIG_PPL_RESET);
		ath79_pll_wr(AR724X_PLL_REG_PCIE_CONFIG, ppl);

		ppl = ath79_pll_rr(AR724X_PLL_REG_PCIE_CONFIG);
		ppl &= ~ATH79_PLL_PCIE_CONFIG_PLL_BYPASS;
		ath79_pll_wr(AR724X_PLL_REG_PCIE_CONFIG, ppl);
	}

	return 0;
}

static int ath79_pcie_start_link(struct ath79_pcie_controller *apc)
{
	u32 rst, app;

	rst = readl_relaxed(apc->ctrl_base + ATH79_PCIE_REG_RESET);
	rst |= ATH79_PCIE_RESET_EP_RESET_L;
	writel_relaxed(rst, apc->ctrl_base + ATH79_PCIE_REG_RESET);

	app = readl_relaxed(apc->ctrl_base + ATH79_PCIE_REG_APP);
	app |= ATH79_PCIE_APP_LTSSM_ENABLE;
	writel_relaxed(app, apc->ctrl_base + ATH79_PCIE_REG_APP);

	return 0;
}

static int ath79_pcie_wait_link(struct ath79_pcie_controller *apc)
{
	u32 rst;

	return readl_poll_timeout(apc->ctrl_base + ATH79_PCIE_REG_RESET, rst,
				 rst & ATH79_PCIE_RESET_LINK_UP,
				 10000, 100000);
}

static int ath79_pcie_probe(struct platform_device *pdev)
{
	struct ath79_pcie_controller *apc;
	struct reset_control *phy_reset;
	struct reset_control *hc_reset;
	struct pci_host_bridge *bridge;
	struct device *dev;
	bool qca;
	int ret;

	dev = &pdev->dev;
	hc_reset = devm_reset_control_get_exclusive(dev, "hc");
	if (IS_ERR(hc_reset))
		return dev_err_probe(dev, PTR_ERR(hc_reset),
				     "failed to get hc reset\n");

	phy_reset = devm_reset_control_get_exclusive(dev, "phy");
	if (IS_ERR(phy_reset))
		return dev_err_probe(dev, PTR_ERR(phy_reset),
				     "failed to get phy reset\n");

	bridge = devm_pci_alloc_host_bridge(dev, sizeof(*apc));
	if (!bridge)
		return -ENOMEM;

	apc = pci_host_bridge_priv(bridge);
	apc->phy_reset = phy_reset;
	apc->hc_reset = hc_reset;
	apc->dev = dev;

	raw_spin_lock_init(&apc->lock);

	apc->ctrl_base = devm_platform_ioremap_resource_byname(pdev, "ctrl_base");
	if (IS_ERR(apc->ctrl_base))
		return PTR_ERR(apc->ctrl_base);

	apc->devcfg_base = devm_platform_ioremap_resource_byname(pdev, "cfg_base");
	if (IS_ERR(apc->devcfg_base))
		return PTR_ERR(apc->devcfg_base);

	apc->crp_base = devm_platform_ioremap_resource_byname(pdev, "crp_base");
	if (IS_ERR(apc->crp_base))
		return PTR_ERR(apc->crp_base);

	apc->irq = platform_get_irq(pdev, 0);
	if (apc->irq < 0)
		return apc->irq;

	/*
	 * Do the full PCIE Root Complex Initialization Sequence if the PCIe
	 * host controller is in reset.
	 */
	if (reset_control_status(apc->hc_reset)) {
		qca = device_is_compatible(dev, "qcom,qca9550-pci");

		ret = ath79_pcie_power_on(apc);
		if (ret)
			return ret;

		ret = ath79_pcie_init_phy(apc, qca);
		if (ret)
			return ret;

		ret = ath79_pcie_start_link(apc);
		if (ret)
			return ret;

		ret = ath79_pcie_wait_link(apc);
		if (ret)
			return ret;
	}

	apc->link_up = ath79_pcie_check_link(apc);
	if (!apc->link_up)
		dev_warn(dev, "PCIe link is down\n");

	ret = ath79_pcie_irq_init(apc);
	if (ret)
		return ret;

	if (apc->link_up)
		ath79_pcie_local_write(apc, PCI_COMMAND, 4, ATH79_PCIE_CMD_INIT);

	bridge->sysdata = apc;
	bridge->ops = &ath79_pcie_ops;
	bridge->map_irq = of_irq_parse_and_map_pci;
	bridge->swizzle_irq = pci_common_swizzle;

	return pci_host_probe(bridge);
}

static const struct of_device_id ath79_pcie_ids[] = {
	{ .compatible = "qcom,ar7240-pci" },
	{ .compatible = "qcom,qca9550-pci" },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, ath79_pcie_ids);

static struct platform_driver ath79_pcie_driver = {
	.probe = ath79_pcie_probe,
	.driver = {
		.name = "ath79-pcie",
		.of_match_table = ath79_pcie_ids,
		.suppress_bind_attrs = true,
	},
};
builtin_platform_driver(ath79_pcie_driver);

MODULE_DESCRIPTION("Atheros ATH79 PCIe host controller driver");
MODULE_LICENSE("GPL");

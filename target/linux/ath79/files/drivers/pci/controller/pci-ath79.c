// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Atheros AR71xx PCI host controller driver
 *
 *  Copyright (C) 2008-2011 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 *  Parts of this file are based on Atheros' 2.6.15 BSP
 */

#include <linux/bitops.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/irqchip/chained_irq.h>
#include <linux/irqdomain.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/of_pci.h>
#include <linux/pci.h>
#include <linux/pci_regs.h>
#include <linux/platform_device.h>
#include <linux/resource.h>
#include <linux/types.h>

#include <asm/mach-ath79/ar71xx_regs.h>
#include <asm/mach-ath79/ath79.h>

#define AR71XX_PCI_REG_CRP_AD_CBE	0x00
#define AR71XX_PCI_REG_CRP_WRDATA	0x04
#define AR71XX_PCI_REG_CRP_RDDATA	0x08
#define AR71XX_PCI_REG_CFG_AD		0x0c
#define AR71XX_PCI_REG_CFG_CBE		0x10
#define AR71XX_PCI_REG_CFG_WRDATA	0x14
#define AR71XX_PCI_REG_CFG_RDDATA	0x18
#define AR71XX_PCI_REG_PCI_ERR		0x1c
#define AR71XX_PCI_REG_PCI_ERR_ADDR	0x20
#define AR71XX_PCI_REG_AHB_ERR		0x24
#define AR71XX_PCI_REG_AHB_ERR_ADDR	0x28

#define AR71XX_PCI_CRP_CMD_WRITE	0x00010000
#define AR71XX_PCI_CRP_CMD_READ		0x00000000
#define AR71XX_PCI_CFG_CMD_READ		0x0000000a
#define AR71XX_PCI_CFG_CMD_WRITE	0x0000000b

#define AR71XX_PCI_INT_CORE		BIT(4)
#define AR71XX_PCI_INT_DEV2		BIT(2)
#define AR71XX_PCI_INT_DEV1		BIT(1)
#define AR71XX_PCI_INT_DEV0		BIT(0)

#define AR71XX_PCI_IRQ_COUNT		5

struct ar71xx_pci_controller {
	struct device *dev;
	struct device_node *np;
	void __iomem *cfg_base;
	int irq;
	struct irq_domain *domain;
};

/* Byte lane enable bits */
static const u8 ar71xx_pci_ble_table[4][4] = {
	{0x0, 0xf, 0xf, 0xf},
	{0xe, 0xd, 0xb, 0x7},
	{0xc, 0xf, 0x3, 0xf},
	{0xf, 0xf, 0xf, 0xf},
};

static const u32 ar71xx_pci_read_mask[8] = {
	0, 0xff, 0xffff, 0, 0xffffffff, 0, 0, 0
};

static inline u32 ar71xx_pci_get_ble(int where, int size, int local)
{
	u32 t;

	t = ar71xx_pci_ble_table[size & 3][where & 3];
	BUG_ON(t == 0xf);
	t <<= (local) ? 20 : 4;

	return t;
}

static inline u32 ar71xx_pci_bus_addr(struct pci_bus *bus, unsigned int devfn,
				      int where)
{
	u32 ret;

	if (!bus->number) {
		/* type 0 */
		ret = (1 << PCI_SLOT(devfn)) | (PCI_FUNC(devfn) << 8) |
		      (where & ~3);
	} else {
		/* type 1 */
		ret = (bus->number << 16) | (PCI_SLOT(devfn) << 11) |
		      (PCI_FUNC(devfn) << 8) | (where & ~3) | 1;
	}

	return ret;
}

static int ar71xx_pci_check_error(struct ar71xx_pci_controller *apc, int quiet)
{
	void __iomem *base = apc->cfg_base;
	u32 pci_err;
	u32 ahb_err;

	pci_err = readl(base + AR71XX_PCI_REG_PCI_ERR) & 3;
	if (pci_err) {
		if (!quiet) {
			u32 addr;

			addr = readl(base + AR71XX_PCI_REG_PCI_ERR_ADDR);
			pr_crit("ar71xx: %s bus error %d at addr 0x%x\n",
				"PCI", pci_err, addr);
		}

		/* clear PCI error status */
		writel(pci_err, base + AR71XX_PCI_REG_PCI_ERR);
	}

	ahb_err = readl(base + AR71XX_PCI_REG_AHB_ERR) & 1;
	if (ahb_err) {
		if (!quiet) {
			u32 addr;

			addr = readl(base + AR71XX_PCI_REG_AHB_ERR_ADDR);
			pr_crit("ar71xx: %s bus error %d at addr 0x%x\n",
				"AHB", ahb_err, addr);
		}

		/* clear AHB error status */
		writel(ahb_err, base + AR71XX_PCI_REG_AHB_ERR);
	}

	return !!(ahb_err | pci_err);
}

static inline void ar71xx_pci_local_write(struct ar71xx_pci_controller *apc,
					  int where, int size, u32 value)
{
	void __iomem *base = apc->cfg_base;
	u32 ad_cbe;

	value = value << (8 * (where & 3));

	ad_cbe = AR71XX_PCI_CRP_CMD_WRITE | (where & ~3);
	ad_cbe |= ar71xx_pci_get_ble(where, size, 1);

	writel(ad_cbe, base + AR71XX_PCI_REG_CRP_AD_CBE);
	writel(value, base + AR71XX_PCI_REG_CRP_WRDATA);
}

static inline int ar71xx_pci_set_cfgaddr(struct pci_bus *bus,
					 unsigned int devfn,
					 int where, int size, u32 cmd)
{
	struct ar71xx_pci_controller *apc = bus->sysdata;
	void __iomem *base = apc->cfg_base;
	u32 addr;

	addr = ar71xx_pci_bus_addr(bus, devfn, where);

	writel(addr, base + AR71XX_PCI_REG_CFG_AD);
	writel(cmd | ar71xx_pci_get_ble(where, size, 0),
		     base + AR71XX_PCI_REG_CFG_CBE);

	return ar71xx_pci_check_error(apc, 1);
}

static int ar71xx_pci_read_config(struct pci_bus *bus, unsigned int devfn,
				  int where, int size, u32 *value)
{
	struct ar71xx_pci_controller *apc = bus->sysdata;
	void __iomem *base = apc->cfg_base;
	u32 data;
	int err;
	int ret;

	ret = PCIBIOS_SUCCESSFUL;
	data = ~0;

	err = ar71xx_pci_set_cfgaddr(bus, devfn, where, size,
				     AR71XX_PCI_CFG_CMD_READ);
	if (err)
		ret = PCIBIOS_DEVICE_NOT_FOUND;
	else
		data = readl(base + AR71XX_PCI_REG_CFG_RDDATA);

	*value = (data >> (8 * (where & 3))) & ar71xx_pci_read_mask[size & 7];

	return ret;
}

static int ar71xx_pci_write_config(struct pci_bus *bus, unsigned int devfn,
				   int where, int size, u32 value)
{
	struct ar71xx_pci_controller *apc = bus->sysdata;
	void __iomem *base = apc->cfg_base;
	int err;
	int ret;

	value = value << (8 * (where & 3));
	ret = PCIBIOS_SUCCESSFUL;

	err = ar71xx_pci_set_cfgaddr(bus, devfn, where, size,
				     AR71XX_PCI_CFG_CMD_WRITE);
	if (err)
		ret = PCIBIOS_DEVICE_NOT_FOUND;
	else
		writel(value, base + AR71XX_PCI_REG_CFG_WRDATA);

	return ret;
}

static struct pci_ops ar71xx_pci_ops = {
	.read	= ar71xx_pci_read_config,
	.write	= ar71xx_pci_write_config,
};

static void ar71xx_pci_irq_handler(struct irq_desc *desc)
{
	struct irq_chip *chip = irq_desc_get_chip(desc);
	struct ar71xx_pci_controller *apc = irq_desc_get_handler_data(desc);
	void __iomem *base = ath79_reset_base;
	u32 pending;

	chained_irq_enter(chip, desc);
	pending = readl(base + AR71XX_RESET_REG_PCI_INT_STATUS) &
		  readl(base + AR71XX_RESET_REG_PCI_INT_ENABLE);

	if (pending & AR71XX_PCI_INT_DEV0)
		generic_handle_domain_irq(apc->domain, 0);
	else if (pending & AR71XX_PCI_INT_DEV1)
		generic_handle_domain_irq(apc->domain, 1);
	else if (pending & AR71XX_PCI_INT_DEV2)
		generic_handle_domain_irq(apc->domain, 2);
	else if (pending & AR71XX_PCI_INT_CORE)
		generic_handle_domain_irq(apc->domain, 4);
	else
		spurious_interrupt();
	chained_irq_exit(chip, desc);
}

static void ar71xx_pci_irq_unmask(struct irq_data *d)
{
	void __iomem *base = ath79_reset_base;
	irq_hw_number_t hwirq = irqd_to_hwirq(d);
	u32 t;

	t = readl(base + AR71XX_RESET_REG_PCI_INT_ENABLE);
	writel(t | BIT(hwirq), base + AR71XX_RESET_REG_PCI_INT_ENABLE);

	/* flush write */
	readl(base + AR71XX_RESET_REG_PCI_INT_ENABLE);
}

static void ar71xx_pci_irq_mask(struct irq_data *d)
{
	void __iomem *base = ath79_reset_base;
	irq_hw_number_t hwirq = irqd_to_hwirq(d);
	u32 t;

	t = readl(base + AR71XX_RESET_REG_PCI_INT_ENABLE);
	writel(t & ~BIT(hwirq), base + AR71XX_RESET_REG_PCI_INT_ENABLE);

	/* flush write */
	readl(base + AR71XX_RESET_REG_PCI_INT_ENABLE);
}

static struct irq_chip ar71xx_pci_irq_chip = {
	.name		= "AR71XX PCI",
	.irq_mask	= ar71xx_pci_irq_mask,
	.irq_unmask	= ar71xx_pci_irq_unmask,
	.irq_mask_ack	= ar71xx_pci_irq_mask,
};

static int ar71xx_pci_irq_map(struct irq_domain *d,
			      unsigned int irq, irq_hw_number_t hw)
{
	struct ar71xx_pci_controller *apc = d->host_data;

	irq_set_chip_and_handler(irq, &ar71xx_pci_irq_chip, handle_level_irq);
	irq_set_chip_data(irq, apc);

	return 0;
}

static const struct irq_domain_ops ar71xx_pci_domain_ops = {
	.xlate = irq_domain_xlate_onecell,
	.map = ar71xx_pci_irq_map,
};

static int ar71xx_pci_irq_init(struct ar71xx_pci_controller *apc)
{
	void __iomem *base = ath79_reset_base;

	writel(0, base + AR71XX_RESET_REG_PCI_INT_ENABLE);
	writel(0, base + AR71XX_RESET_REG_PCI_INT_STATUS);

	apc->domain = irq_domain_create_linear(of_fwnode_handle(apc->np),
					       AR71XX_PCI_IRQ_COUNT,
					       &ar71xx_pci_domain_ops, apc);
	if (!apc->domain)
		return -ENOMEM;

	irq_set_chained_handler_and_data(apc->irq, ar71xx_pci_irq_handler, apc);

	return 0;
}

static void ar71xx_pci_reset(void)
{
	ath79_device_reset_set(AR71XX_RESET_PCI_BUS | AR71XX_RESET_PCI_CORE);
	mdelay(100);

	ath79_device_reset_clear(AR71XX_RESET_PCI_BUS | AR71XX_RESET_PCI_CORE);
	mdelay(100);

	ath79_ddr_set_pci_windows();
	mdelay(100);
}

static int ar71xx_pci_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct ar71xx_pci_controller *apc;
	struct pci_host_bridge *bridge;
	int ret;
	u32 t;

	bridge = devm_pci_alloc_host_bridge(dev, sizeof(*apc));
	if (!bridge)
		return -ENOMEM;

	apc = pci_host_bridge_priv(bridge);
	apc->dev = dev;
	apc->np = dev->of_node;

	apc->cfg_base = devm_platform_ioremap_resource_byname(pdev, "cfg_base");
	if (IS_ERR(apc->cfg_base))
		return PTR_ERR(apc->cfg_base);

	apc->irq = platform_get_irq(pdev, 0);
	if (apc->irq < 0)
		return apc->irq;

	ar71xx_pci_reset();

	/* setup COMMAND register */
	t = PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER | PCI_COMMAND_INVALIDATE
	  | PCI_COMMAND_PARITY | PCI_COMMAND_SERR | PCI_COMMAND_FAST_BACK;
	ar71xx_pci_local_write(apc, PCI_COMMAND, 4, t);

	/* clear bus errors */
	ar71xx_pci_check_error(apc, 1);

	ret = ar71xx_pci_irq_init(apc);
	if (ret)
		return ret;

	bridge->sysdata = apc;
	bridge->ops = &ar71xx_pci_ops;
	bridge->map_irq = of_irq_parse_and_map_pci;
	bridge->swizzle_irq = pci_common_swizzle;

	return pci_host_probe(bridge);
}

static const struct of_device_id ar71xx_pci_ids[] = {
	{ .compatible = "qca,ar7100-pci" },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, ar71xx_pci_ids);

static struct platform_driver ar71xx_pci_driver = {
	.probe = ar71xx_pci_probe,
	.driver = {
		.name = "ar71xx-pci",
		.of_match_table = ar71xx_pci_ids,
		.suppress_bind_attrs = true,
	},
};
builtin_platform_driver(ar71xx_pci_driver);

MODULE_DESCRIPTION("Atheros AR71xx PCI host controller driver");
MODULE_LICENSE("GPL");

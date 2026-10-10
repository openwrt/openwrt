// SPDX-License-Identifier: GPL-2.0-only
/*
 * ZTE zx279128s PCIe host controller
 *
 * The SoC has two DesignWare PCIe controllers with one lane each. Their
 * clocks, resets and PHYs are controlled through the Top CRM (clock and reset
 * module) blocks, and a pin state from the pin mux is applied before probe.
 * The register sequence comes from the vendor firmware, and the meaning of
 * most of its bits is not documented.
 *
 * Copyright (C) 2026 Navid Ghahremani <ghahramani.navid@gmail.com>
 */

#include <linux/bitfield.h>
#include <linux/bits.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/gpio/consumer.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/mfd/syscon.h>
#include <linux/mod_devicetable.h>
#include <linux/mutex.h>
#include <linux/of.h>
#include <linux/pci.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>

#include "pcie-designware.h"

/* Top CRM 1: PHY and PLL controls */
#define CRM1_PHY_CTRL		0x008
#define CRM1_MISC		0x024
#define CRM1_MISC_BIT26		BIT(26)
#define CRM1_PHY1_CAL0		0x1b0
#define CRM1_PHY1_CAL1		0x1b4

/* Top CRM 2: clocks and resets */
#define CRM2_CLK		0x070
#define CRM2_RST		0x074
#define CRM2_RST_ALL		0x3f

/* Controller wrapper ("ctrl" registers) */
#define CTRL_CFG		0x000
#define CTRL_CFG_EN		BIT(5)
#define CTRL_STATUS		0x0dc
#define CTRL_STATUS_LTSSM	GENMASK(22, 17)
#define CTRL_LTSSM_L0		0x11

enum { ZX_CRM1, ZX_CRM2, ZX_NUM_CRM };

/* The CRM bits of one controller, which is identified by its ctrl address */
struct zx279128s_pcie_port {
	resource_size_t ctrl_addr;
	u32 clk_on;		/* CRM2_CLK bits set first */
	u32 clk_off;		/* CRM2_CLK bit cleared at the end */
	u32 phy_on[3];		/* CRM1_PHY_CTRL bits, set in this order */
	bool phy_cal;		/* write the CRM1_PHY1_CAL values */
	u32 rst;		/* CRM2_RST bit */
};

static const struct zx279128s_pcie_port zx279128s_pcie_ports[] = {
	{
		.ctrl_addr = 0x09500000,
		.clk_on = 0xb5,
		.clk_off = BIT(8),
		.phy_on = { BIT(15), BIT(13), BIT(14) },
		.rst = BIT(0),
	}, {
		.ctrl_addr = 0x09600000,
		.clk_on = 0x1aa00,
		.clk_off = BIT(17),
		.phy_on = { BIT(27), BIT(25), BIT(26) },
		.phy_cal = true,
		.rst = BIT(3),
	},
};

struct zx279128s_pcie {
	struct dw_pcie pci;
	void __iomem *ctrl;
	struct regmap *crm[ZX_NUM_CRM];
	const struct zx279128s_pcie_port *port;
	struct gpio_desc *reset;
	struct gpio_desc *enable;
};

#define to_zx279128s_pcie(x)	container_of(x, struct zx279128s_pcie, pci)

/* The board reset lines are shared by both controllers, see host_init */
static DEFINE_MUTEX(zx279128s_board_lock);
static bool zx279128s_board_released;

/* Limit the link to 2.5 GT/s and retrain it, as the vendor firmware does */
static void zx279128s_pcie_force_gen1(struct dw_pcie *pci)
{
	u8 cap = dw_pcie_find_capability(pci, PCI_CAP_ID_EXP);
	u32 val;

	if (WARN_ON(!cap))
		return;

	dw_pcie_dbi_ro_wr_en(pci);

	/*
	 * The CRM setup supports Gen1 in L0. With L0s/L1 enabled the H3600
	 * endpoints stop responding, so do not advertise those states or
	 * clock power management until their wakeup sequence is understood.
	 */
	val = dw_pcie_readl_dbi(pci, cap + PCI_EXP_LNKCAP);
	val &= ~(PCI_EXP_LNKCAP_SLS | PCI_EXP_LNKCAP_ASPMS | PCI_EXP_LNKCAP_CLKPM);
	val |= PCI_EXP_LNKCAP_SLS_2_5GB;
	dw_pcie_writel_dbi(pci, cap + PCI_EXP_LNKCAP, val);

	val = dw_pcie_readl_dbi(pci, cap + PCI_EXP_LNKCTL2);
	val &= ~PCI_EXP_LNKCTL2_TLS;
	val |= PCI_EXP_LNKCTL2_TLS_2_5GT;
	dw_pcie_writel_dbi(pci, cap + PCI_EXP_LNKCTL2, val);

	val = dw_pcie_readl_dbi(pci, cap + PCI_EXP_LNKCTL);
	val &= ~(PCI_EXP_LNKCTL_ASPMC | PCI_EXP_LNKCTL_CLKREQ_EN);
	dw_pcie_writel_dbi(pci, cap + PCI_EXP_LNKCTL, val | PCI_EXP_LNKCTL_RL);

	dw_pcie_dbi_ro_wr_dis(pci);
}

static int zx279128s_pcie_init_crm(struct zx279128s_pcie *pcie)
{
	const struct zx279128s_pcie_port *port = pcie->port;
	struct regmap *crm1 = pcie->crm[ZX_CRM1];
	struct regmap *crm2 = pcie->crm[ZX_CRM2];
	int ret, i;

	/* Ungate the clocks */
	ret = regmap_write_bits(crm2, CRM2_CLK, port->clk_on, port->clk_on);
	if (ret)
		return ret;
	fsleep(50);

	/* Power up the PHY and its PLL */
	for (i = 0; i < ARRAY_SIZE(port->phy_on); i++) {
		ret = regmap_write_bits(crm1, CRM1_PHY_CTRL, port->phy_on[i],
					port->phy_on[i]);
		if (ret)
			return ret;
		fsleep(50);

		if (i == 0 && port->phy_cal) {
			ret = regmap_write(crm1, CRM1_PHY1_CAL1, 0x00202d5a);
			if (ret)
				return ret;
			ret = regmap_write(crm1, CRM1_PHY1_CAL0, 0x0046c24a);
			if (ret)
				return ret;
		}
	}

	ret = regmap_write_bits(crm2, CRM2_RST, port->rst, 0);
	if (ret)
		return ret;
	fsleep(50);

	ret = regmap_write_bits(crm1, CRM1_MISC, CRM1_MISC_BIT26, 0);
	if (ret)
		return ret;
	fsleep(50);

	ret = regmap_write_bits(crm2, CRM2_RST, port->rst, port->rst);
	if (ret)
		return ret;
	ret = regmap_write_bits(crm2, CRM2_CLK, port->clk_off, 0);
	if (ret)
		return ret;

	return regmap_write(crm2, CRM2_RST, CRM2_RST_ALL);
}

static int zx279128s_pcie_host_init(struct dw_pcie_rp *pp)
{
	struct dw_pcie *pci = to_dw_pcie_from_pp(pp);
	struct zx279128s_pcie *pcie = to_zx279128s_pcie(pci);
	int ret;

	/*
	 * The board lines reset a device that can sit behind both controllers
	 * (the MT7915 of the H3600 uses both). The first controller to start
	 * resets and releases it; the second must not reset it again.
	 */
	mutex_lock(&zx279128s_board_lock);
	if (!zx279128s_board_released && (pcie->reset || pcie->enable)) {
		ret = gpiod_direction_output(pcie->reset, 1);
		if (ret)
			goto err_unlock;
		ret = gpiod_direction_output(pcie->enable, 0);
		if (ret)
			goto err_unlock;
		msleep(100);
		ret = gpiod_set_value_cansleep(pcie->reset, 0);
		if (ret)
			goto err_unlock;
		ret = gpiod_set_value_cansleep(pcie->enable, 1);
		if (ret)
			goto err_unlock;
		msleep(200);
		zx279128s_board_released = true;
	}
	mutex_unlock(&zx279128s_board_lock);

	ret = zx279128s_pcie_init_crm(pcie);
	if (ret)
		return dev_err_probe(pci->dev, ret, "failed to initialize CRM\n");

	/* Enable the controller, which starts link training */
	writel(readl(pcie->ctrl + CTRL_CFG) | CTRL_CFG_EN, pcie->ctrl + CTRL_CFG);

	zx279128s_pcie_force_gen1(pci);

	return 0;

err_unlock:
	mutex_unlock(&zx279128s_board_lock);
	return dev_err_probe(pci->dev, ret, "failed to drive the board GPIOs\n");
}

/*
 * The Wi-Fi on the H3600 uses legacy INTx interrupts. Providing msi_init
 * keeps the DWC core from setting up its internal MSI controller, which has
 * not been tested on this SoC.
 */
static int zx279128s_pcie_msi_init(struct dw_pcie_rp *pp)
{
	return 0;
}

static const struct dw_pcie_host_ops zx279128s_pcie_host_ops = {
	.init = zx279128s_pcie_host_init,
	.msi_init = zx279128s_pcie_msi_init,
};

static int zx279128s_pcie_start_link(struct dw_pcie *pci)
{
	zx279128s_pcie_force_gen1(pci);

	return 0;
}

static bool zx279128s_pcie_link_up(struct dw_pcie *pci)
{
	struct zx279128s_pcie *pcie = to_zx279128s_pcie(pci);
	u32 val = readl(pcie->ctrl + CTRL_STATUS);

	return FIELD_GET(CTRL_STATUS_LTSSM, val) == CTRL_LTSSM_L0;
}

static const struct dw_pcie_ops zx279128s_pcie_ops = {
	.link_up = zx279128s_pcie_link_up,
	.start_link = zx279128s_pcie_start_link,
};

static int zx279128s_pcie_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct zx279128s_pcie *pcie;
	struct device_node *np;
	struct resource *res;
	int i;

	pcie = devm_kzalloc(dev, sizeof(*pcie), GFP_KERNEL);
	if (!pcie)
		return -ENOMEM;

	pcie->pci.dev = dev;
	pcie->pci.ops = &zx279128s_pcie_ops;
	pcie->pci.pp.ops = &zx279128s_pcie_host_ops;

	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "ctrl");
	pcie->ctrl = devm_ioremap_resource(dev, res);
	if (IS_ERR(pcie->ctrl))
		return PTR_ERR(pcie->ctrl);

	for (i = 0; i < ARRAY_SIZE(zx279128s_pcie_ports); i++)
		if (zx279128s_pcie_ports[i].ctrl_addr == res->start)
			pcie->port = &zx279128s_pcie_ports[i];
	if (!pcie->port)
		return dev_err_probe(dev, -EINVAL, "unknown controller at %pR\n",
				     res);

	for (i = 0; i < ZX_NUM_CRM; i++) {
		np = of_parse_phandle(dev->of_node, "zte,crm", i);
		if (!np)
			return dev_err_probe(dev, -EINVAL,
					     "missing Top CRM %d\n", i + 1);

		pcie->crm[i] = syscon_node_to_regmap(np);
		of_node_put(np);
		if (IS_ERR(pcie->crm[i]))
			return dev_err_probe(dev, PTR_ERR(pcie->crm[i]),
					     "failed to get Top CRM %d\n", i + 1);
	}

	/*
	 * Shared with the other controller: take the lines as they are, so
	 * that a late probe can't reset a device the other one already uses.
	 */
	pcie->reset = devm_gpiod_get_optional(dev, "reset",
					      GPIOD_ASIS | GPIOD_FLAGS_BIT_NONEXCLUSIVE);
	if (IS_ERR(pcie->reset))
		return dev_err_probe(dev, PTR_ERR(pcie->reset),
				     "failed to get the reset GPIO\n");

	pcie->enable = devm_gpiod_get_optional(dev, "enable",
					       GPIOD_ASIS | GPIOD_FLAGS_BIT_NONEXCLUSIVE);
	if (IS_ERR(pcie->enable))
		return dev_err_probe(dev, PTR_ERR(pcie->enable),
				     "failed to get the enable GPIO\n");

	return dw_pcie_host_init(&pcie->pci.pp);
}

static const struct of_device_id zx279128s_pcie_of_match[] = {
	{ .compatible = "zte,zx279128s-pcie" },
	{ }
};

static struct platform_driver zx279128s_pcie_driver = {
	.probe = zx279128s_pcie_probe,
	.driver = {
		.name = "zx279128s-pcie",
		.of_match_table = zx279128s_pcie_of_match,
		.suppress_bind_attrs = true,
	},
};
builtin_platform_driver(zx279128s_pcie_driver);

// SPDX-License-Identifier: GPL-2.0-only
/*
 * ZTE zx279128s PCIe host controller
 *
 * The SoC has two DesignWare PCIe controllers with one lane each. Their
 * clocks, resets and PHYs are managed through the CCF, Reset and Generic PHY
 * frameworks.
 *
 * Copyright (C) 2026 Navid Ghahremani <ghahramani.navid@gmail.com>
 */

#include <linux/bitfield.h>
#include <linux/bits.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/gpio/consumer.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/mutex.h>
#include <linux/of.h>
#include <linux/pci.h>
#include <linux/phy/phy.h>
#include <linux/platform_device.h>
#include <linux/reset.h>

#include "pcie-designware.h"

/* Controller wrapper ("ctrl" registers) */
#define CTRL_CFG		0x000
#define CTRL_CFG_EN		BIT(5)
#define CTRL_STATUS		0x0dc
#define CTRL_STATUS_LTSSM	GENMASK(22, 17)
#define CTRL_LTSSM_L0		0x11

struct zx279128s_pcie {
	struct dw_pcie pci;
	void __iomem *ctrl;
	struct clk *clk;
	struct reset_control *rst;
	struct phy *phy;
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
	u16 val16;

	if (WARN_ON(!cap))
		return;

	dw_pcie_dbi_ro_wr_en(pci);

	/*
	 * With L0s/L1 enabled the H3600 endpoints stop responding, so do not
	 * advertise those states or clock power management until their wakeup
	 * sequence is understood.
	 */
	val = dw_pcie_readl_dbi(pci, cap + PCI_EXP_LNKCAP);
	val &= ~(PCI_EXP_LNKCAP_SLS | PCI_EXP_LNKCAP_ASPMS |
		 PCI_EXP_LNKCAP_CLKPM);
	val |= PCI_EXP_LNKCAP_SLS_2_5GB;
	dw_pcie_writel_dbi(pci, cap + PCI_EXP_LNKCAP, val);

	val16 = dw_pcie_readw_dbi(pci, cap + PCI_EXP_LNKCTL2);
	val16 &= ~PCI_EXP_LNKCTL2_TLS;
	val16 |= PCI_EXP_LNKCTL2_TLS_2_5GT;
	dw_pcie_writew_dbi(pci, cap + PCI_EXP_LNKCTL2, val16);

	val16 = dw_pcie_readw_dbi(pci, cap + PCI_EXP_LNKCTL);
	val16 &= ~(PCI_EXP_LNKCTL_ASPMC | PCI_EXP_LNKCTL_CLKREQ_EN);
	dw_pcie_writew_dbi(pci, cap + PCI_EXP_LNKCTL, val16 | PCI_EXP_LNKCTL_RL);

	dw_pcie_dbi_ro_wr_dis(pci);
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

	ret = phy_init(pcie->phy);
	if (ret)
		goto err_unlock;

	ret = phy_power_on(pcie->phy);
	if (ret)
		goto err_phy_exit;

	ret = reset_control_assert(pcie->rst);
	if (ret)
		goto err_phy_off;
	fsleep(50);
	ret = reset_control_deassert(pcie->rst);
	if (ret)
		goto err_phy_off;

	mutex_unlock(&zx279128s_board_lock);

	/* Enable the controller, which starts link training */
	writel(readl(pcie->ctrl + CTRL_CFG) | CTRL_CFG_EN, pcie->ctrl + CTRL_CFG);

	zx279128s_pcie_force_gen1(pci);

	return 0;

err_phy_off:
	phy_power_off(pcie->phy);
err_phy_exit:
	phy_exit(pcie->phy);
err_unlock:
	mutex_unlock(&zx279128s_board_lock);
	return dev_err_probe(pci->dev, ret, "failed to initialize PCIe\n");
}

static void zx279128s_pcie_host_deinit(struct dw_pcie_rp *pp)
{
	struct dw_pcie *pci = to_dw_pcie_from_pp(pp);
	struct zx279128s_pcie *pcie = to_zx279128s_pcie(pci);

	mutex_lock(&zx279128s_board_lock);
	reset_control_assert(pcie->rst);
	phy_power_off(pcie->phy);
	phy_exit(pcie->phy);
	mutex_unlock(&zx279128s_board_lock);
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
	.deinit = zx279128s_pcie_host_deinit,
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
	struct resource *res;

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

	pcie->clk = devm_clk_get_enabled(dev, "pcie");
	if (IS_ERR(pcie->clk))
		return dev_err_probe(dev, PTR_ERR(pcie->clk),
				     "failed to get pcie clock\n");

	pcie->rst = devm_reset_control_get_exclusive(dev, "core");
	if (IS_ERR(pcie->rst))
		pcie->rst = devm_reset_control_get_exclusive(dev, "pcie");
	if (IS_ERR(pcie->rst))
		return dev_err_probe(dev, PTR_ERR(pcie->rst),
				     "failed to get pcie reset\n");

	pcie->phy = devm_phy_get(dev, "pcie");
	if (IS_ERR(pcie->phy))
		return dev_err_probe(dev, PTR_ERR(pcie->phy),
				     "failed to get pcie phy\n");

	/*
	 * Optional board lines to reset and enable endpoints. If already
	 * claimed by another controller, treat -EBUSY as already managed.
	 */
	pcie->reset = devm_gpiod_get_optional(dev, "reset", GPIOD_ASIS);
	if (IS_ERR(pcie->reset)) {
		if (PTR_ERR(pcie->reset) == -EBUSY)
			pcie->reset = NULL;
		else
			return dev_err_probe(dev, PTR_ERR(pcie->reset),
					     "failed to get the reset GPIO\n");
	}

	pcie->enable = devm_gpiod_get_optional(dev, "enable", GPIOD_ASIS);
	if (IS_ERR(pcie->enable)) {
		if (PTR_ERR(pcie->enable) == -EBUSY)
			pcie->enable = NULL;
		else
			return dev_err_probe(dev, PTR_ERR(pcie->enable),
					     "failed to get the enable GPIO\n");
	}

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

// SPDX-License-Identifier: GPL-2.0-or-later

/* PCIe host controller driver for Realtek SoCs */

#include <linux/bitfield.h>
#include <linux/bitops.h>
#include <linux/delay.h>
#include <linux/gpio/consumer.h>
#include <linux/io.h>
#include <linux/iopoll.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/pci.h>
#include <linux/phy/phy.h>
#include <linux/platform_device.h>

#include "../pci.h"

/* Host config register offsets */
#define RTPCIE_HOSTCFG_CAP	0x70
#define RTPCIE_HOSTCFG_ENABLE	0x80c
#define RTPCIE_ENABLE_BIT17	BIT(17)
#define RTPCIE_LINK_STATUS	0x728
#define RTPCIE_LINKUP_MASK	GENMASK(4, 0)
#define RTPCIE_IS_LINKUP	(BIT(4) | BIT(0))
#define RTPCIE_PCI_CMD_BIT20	BIT(20)
/**
 * struct rtpcie_ctrl - Realtek PCIe port information
 * @dev: pointer to PCIe device
 * @hostcfg_base: host config register base
 * @hostext_base: host extension register base
 * @devcfg_base: device config register base
 * @reset_gpio: gpio reset
 * @phy: pointer to PHY control block
 * @bus_number: assigned PCI bus number
 */
struct rtpcie_ctrl {
	struct device *dev;
	void __iomem *hostcfg_base;
	void __iomem *hostext_base;
	void __iomem *devcfg_base;
	struct gpio_desc *reset_gpio;
	struct phy *phy;
	u8 bus_number;
};

static void __iomem *rtpcie_map_bus(struct pci_bus *bus, unsigned int devfn, int where)
{
	struct rtpcie_ctrl *pcie = bus->sysdata;

	if (pcie->bus_number == 0xff)
		pcie->bus_number = bus->number;

	if (bus->number != pcie->bus_number)
		return NULL;

	writel(PCI_FUNC(devfn), pcie->hostext_base);

	switch (PCI_SLOT(devfn)) {
	case 0:
		return pcie->hostcfg_base + where;
	case 1:
		return pcie->devcfg_base + where;
	default:
		return NULL;
	}
}

static struct pci_ops rtpcie_ops = {
	.map_bus = rtpcie_map_bus,
	.read = pci_generic_config_read,
	.write = pci_generic_config_write,
};

static int rtpcie_hw_init(struct rtpcie_ctrl *pcie)
{
	u16 devctl, link;
	int err;
	u32 val;

	/* Assert reset pcie */
	gpiod_set_value_cansleep(pcie->reset_gpio, 1);

	err = phy_init(pcie->phy);
	if (err) {
		dev_err(pcie->dev, "failed to initialize pcie-phy\n");
		return err;
	}

	/* Deassert reset pcie */
	gpiod_set_value_cansleep(pcie->reset_gpio, 0);

	/* Wait for Link Up */
	err = readl_poll_timeout(pcie->hostcfg_base + RTPCIE_LINK_STATUS, val,
				 (val & RTPCIE_LINKUP_MASK) == RTPCIE_IS_LINKUP,
				 10000, 100000);

	if (err) {
		dev_err(pcie->dev, "PCIE Link Up Failed!\n");
		phy_exit(pcie->phy);
		return err;
	}

	msleep(100);

	/* Enable PCIE host */
	val = RTPCIE_PCI_CMD_BIT20 | PCI_COMMAND_IO | PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER;
	writel(val, pcie->hostcfg_base + PCI_COMMAND);

	/* Clear max payload size bits in DEVCTL */
	devctl = readw(pcie->hostcfg_base + RTPCIE_HOSTCFG_CAP + PCI_EXP_DEVCTL);
	devctl &= ~PCI_EXP_DEVCTL_PAYLOAD;
	writew(devctl, pcie->hostcfg_base + RTPCIE_HOSTCFG_CAP + PCI_EXP_DEVCTL);

	val = readl(pcie->hostcfg_base + RTPCIE_HOSTCFG_ENABLE);
	writel(val | RTPCIE_ENABLE_BIT17, pcie->hostcfg_base + RTPCIE_HOSTCFG_ENABLE);

	usleep_range(1000, 2000);

	link = readw(pcie->hostcfg_base + RTPCIE_HOSTCFG_CAP + PCI_EXP_LNKSTA);
	dev_info(pcie->dev, "link up, %s\n",
		 pci_speed_string(pcie_link_speed[FIELD_GET(PCI_EXP_LNKSTA_CLS, link)]));

	return err;
}

static int rtpcie_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct rtpcie_ctrl *pcie;
	struct pci_host_bridge *bridge;
	int ret;

	bridge = devm_pci_alloc_host_bridge(dev, sizeof(*pcie));
	if (!bridge)
		return -ENOMEM;

	pcie = pci_host_bridge_priv(bridge);
	pcie->dev = dev;
	pcie->bus_number = 0xff;
	platform_set_drvdata(pdev, pcie);

	pcie->hostcfg_base = devm_platform_ioremap_resource_byname(pdev, "hostcfg");
	if (IS_ERR(pcie->hostcfg_base))
		return dev_err_probe(dev, PTR_ERR(pcie->hostcfg_base),
				     "failed to map hostcfg\n");

	pcie->hostext_base = devm_platform_ioremap_resource_byname(pdev, "hostext");
	if (IS_ERR(pcie->hostext_base))
		return dev_err_probe(dev, PTR_ERR(pcie->hostext_base),
				     "failed to map hostext\n");

	pcie->devcfg_base = devm_platform_ioremap_resource_byname(pdev, "devcfg");
	if (IS_ERR(pcie->devcfg_base))
		return dev_err_probe(dev, PTR_ERR(pcie->devcfg_base),
				     "failed to map devcfg\n");

	pcie->reset_gpio = devm_gpiod_get(dev, "reset", GPIOD_OUT_LOW);
	if (IS_ERR(pcie->reset_gpio))
		return dev_err_probe(dev, PTR_ERR(pcie->reset_gpio),
				     "failed to get reset GPIO\n");

	pcie->phy = devm_of_phy_get(dev, dev->of_node, NULL);
	if (IS_ERR(pcie->phy))
		return dev_err_probe(dev, PTR_ERR(pcie->phy), "failed to get pcie-phy");

	ret = rtpcie_hw_init(pcie);
	if (ret)
		return ret;

	bridge->sysdata = pcie;
	bridge->ops = &rtpcie_ops;

	ret = pci_host_probe(bridge);
	if (ret) {
		phy_exit(pcie->phy);
		return ret;
	}

	return 0;
}

static const struct of_device_id rtpcie_of_match[] = {
	{ .compatible = "realtek,rtl9607c-pcie" },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, rtpcie_of_match);

static struct platform_driver rtpcie_driver = {
	.probe = rtpcie_probe,
	.driver = {
		.name = "realtek-pcie",
		.of_match_table = rtpcie_of_match,
	},
};
builtin_platform_driver(rtpcie_driver);

MODULE_DESCRIPTION("PCIe host controller driver for Realtek SoC");
MODULE_LICENSE("GPL");

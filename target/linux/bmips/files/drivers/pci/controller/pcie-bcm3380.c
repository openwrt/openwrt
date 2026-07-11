// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * BCM3380 PCIe Controller Driver
 *
 * The bring-up sequence is derived from the vendor eCos firmware's
 * MCP_iPciCoreInit() and the BCM3380 GPL pcie_blockdef.h register layout.
 */

#include <linux/bitops.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/gpio/consumer.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/of_pci.h>
#include <linux/pci.h>
#include <linux/platform_device.h>
#include <linux/reset.h>
#include <linux/types.h>
#include <linux/vmalloc.h>
#include <soc/bcm/bcm3380-gphy.h>

#include "../pci.h"

#define PCIE_BUS_BRIDGE			0
#define PCIE_BUS_DEVICE			1

#define PCIE_DEVICE_OFFSET			0x8000

#define PCIE_COMMAND_STATUS_REG			0x0004
#define PCIE_BUS_NUMBER_REG			0x0018
#define PCIE_BUS_NUMBER_INIT			0x00010100
#define PCIE_MEMORY_BASE_LIMIT_REG		0x0020
#define PCIE_PREF_MEMORY_BASE_LIMIT_REG		0x0024
#define PCIE_PREF_MEMORY_DISABLED		0x0000fff0
#define PCIE_PREF_BASE_UPPER32_REG		0x0028
#define PCIE_PREF_LIMIT_UPPER32_REG		0x002c

#define PCIE_DEVICE_CONTROL_REG			0x00b4
#define PCIE_DEVICE_CONTROL_INIT		0x00002c10

#define PCIE_CONFIG2_REG			0x0408
#define PCIE_CONFIG2_BAR1_SIZE_MASK		0x0000000f

#define PCIE_IDVAL3_REG				0x043c
#define PCIE_IDVAL3_CLASS_CODE_MASK		0x00ffffff
#define PCIE_IDVAL3_SUBCLASS_SHIFT		8

#define PCIE_RC_CFG_0800_REG			0x0800
#define PCIE_RC_CFG_0800_INIT_BITS		0x0c000000
#define PCIE_RC_CFG_0804_REG			0x0804
#define PCIE_RC_CFG_0804_INIT_BITS		0x0a000000

#define PCIE_DLSTATUS_REG			0x1048
#define PCIE_DLSTATUS_PHYLINKUP			BIT(13)

/* PcieBar1RemapReg.Reg32 / PcieBar2RemapReg.Reg32 */
#define PCIE_BAR1_REMAP_REG			0x2818
#define PCIE_BAR2_REMAP_REG			0x281c
#define PCIE_BAR_REMAP_ENABLE			BIT(1)

/* PcieUbus2PcieBar0BaseMaskReg.Reg32 */
#define PCIE_UBUS2PCIE_BAR0_BASEMASK_REG	0x2828
#define PCIE_UBUS2PCIE_BAR0_REMAP_ENABLE	BIT(0)
#define PCIE_UBUS2PCIE_BAR0_MASK_SHIFT		4
#define PCIE_UBUS2PCIE_BAR0_BASE_SHIFT		20

/* PcieUbus2PcieBar0RemapAddReg.Reg32 */
#define PCIE_UBUS2PCIE_BAR0_REMAP_REG		0x282c
#define PCIE_UBUS2PCIE_BAR0_REMAP_SHIFT		20

#define PCIE_RC_CFG_0558_REG			0x0558
#define PCIE_RC_CFG_0558_INIT			0x1000008c
#define PCIE_RC_CFG_055c_REG			0x055c
#define PCIE_RC_CFG_0560_REG			0x0560
#define PCIE_RC_CFG_0560_INIT			0x0000008c
#define PCIE_RC_CFG_0564_REG			0x0564

#define SERDES_CNTRL_REG			0x0000
#define SERDES_CNTRL_PCIE_ENABLE		0x00008001
#define SERDES_STATUS_REG			0x0004

struct bcm3380_pcie {
	void __iomem *base;
	void __iomem *serdes;
	int irq;
	struct bcm3380_gphy *gphy;
	struct clk_bulk_data *clks;
	int num_clks;
	struct gpio_desc *enable_gpio;
	struct reset_control *reset;
	struct reset_control *reset_core;
	struct reset_control *reset_perst;
};

static struct bcm3380_pcie bcm3380_pcie;

extern int bmips_pci_irq;

static int postprocess_read(u32 data, int where, unsigned int size)
{
	u32 ret = 0;

	switch (size) {
	case 1:
		ret = (data >> ((where & 3) << 3)) & 0xff;
		break;
	case 2:
		ret = (data >> ((where & 3) << 3)) & 0xffff;
		break;
	case 4:
		ret = data;
		break;
	}

	return ret;
}

static int preprocess_write(u32 orig_data, u32 val, int where,
			    unsigned int size)
{
	u32 ret = 0;

	switch (size) {
	case 1:
		ret = (orig_data & ~(0xff << ((where & 3) << 3))) |
		      (val << ((where & 3) << 3));
		break;
	case 2:
		ret = (orig_data & ~(0xffff << ((where & 3) << 3))) |
		      (val << ((where & 3) << 3));
		break;
	case 4:
		ret = val;
		break;
	}

	return ret;
}

static bool bcm3380_pcie_link_up(struct bcm3380_pcie *priv)
{
	return __raw_readl(priv->base + PCIE_DLSTATUS_REG) &
	       PCIE_DLSTATUS_PHYLINKUP;
}

static bool bcm3380_pcie_can_access(struct pci_bus *bus, int devfn)
{
	struct bcm3380_pcie *priv = &bcm3380_pcie;

	switch (bus->number) {
	case PCIE_BUS_BRIDGE:
		return PCI_SLOT(devfn) == 0;
	case PCIE_BUS_DEVICE:
		if (PCI_SLOT(devfn) == 0)
			return bcm3380_pcie_link_up(priv);
		fallthrough;
	default:
		return false;
	}
}

static int bcm3380_pcie_read(struct pci_bus *bus, unsigned int devfn,
			     int where, int size, u32 *val)
{
	struct bcm3380_pcie *priv = &bcm3380_pcie;
	u32 reg = where & ~3;

	if (!bcm3380_pcie_can_access(bus, devfn))
		return PCIBIOS_DEVICE_NOT_FOUND;

	if (bus->number == PCIE_BUS_DEVICE)
		reg += PCIE_DEVICE_OFFSET;

	*val = postprocess_read(__raw_readl(priv->base + reg), where, size);

	return PCIBIOS_SUCCESSFUL;
}

static int bcm3380_pcie_write(struct pci_bus *bus, unsigned int devfn,
			      int where, int size, u32 val)
{
	struct bcm3380_pcie *priv = &bcm3380_pcie;
	u32 reg = where & ~3;

	if (!bcm3380_pcie_can_access(bus, devfn))
		return PCIBIOS_DEVICE_NOT_FOUND;

	if (bus->number == PCIE_BUS_DEVICE)
		reg += PCIE_DEVICE_OFFSET;

	u32 data = __raw_readl(priv->base + reg);

	__raw_writel(preprocess_write(data, val, where, size),
		     priv->base + reg);

	return PCIBIOS_SUCCESSFUL;
}

static struct pci_ops bcm3380_pcie_ops = {
	.read = bcm3380_pcie_read,
	.write = bcm3380_pcie_write,
};

static struct resource bcm3380_pcie_io_resource;
static struct resource bcm3380_pcie_mem_resource;
static struct resource bcm3380_pcie_busn_resource;

static struct pci_controller bcm3380_pcie_controller = {
	.pci_ops = &bcm3380_pcie_ops,
	.io_resource = &bcm3380_pcie_io_resource,
	.mem_resource = &bcm3380_pcie_mem_resource,
};

static void bcm3380_pcie_disable_clks(void *data)
{
	struct bcm3380_pcie *priv = data;

	clk_bulk_disable_unprepare(priv->num_clks, priv->clks);
}

static void bcm3380_pcie_reset(struct bcm3380_pcie *priv)
{
	reset_control_assert(priv->reset);
	reset_control_assert(priv->reset_core);
	reset_control_assert(priv->reset_perst);
	mdelay(1);

	reset_control_deassert(priv->reset_perst);
	mdelay(1);

	reset_control_deassert(priv->reset);
	reset_control_deassert(priv->reset_core);
	mdelay(2);
}

static void bcm3380_pcie_enable_serdes(struct bcm3380_pcie *priv)
{
	__raw_writel(SERDES_CNTRL_PCIE_ENABLE,
		     priv->serdes + SERDES_CNTRL_REG);
}

static int bcm3380_pcie_wait_link(struct device *dev, struct bcm3380_pcie *priv)
{
	for (int i = 0; i < 10; i++) {
		if (bcm3380_pcie_link_up(priv))
			return 0;

		mdelay(2);
	}

	dev_err(dev, "PCIe link did not come up: DLSTATUS=0x%08x\n",
		__raw_readl(priv->base + PCIE_DLSTATUS_REG));

	return -ENODEV;
}

static u32 bcm3380_pcie_bridge_memory_base_limit(void)
{
	resource_size_t window_end = bcm3380_pcie_mem_resource.start +
				    resource_size(&bcm3380_pcie_mem_resource);

	return ((bcm3380_pcie_mem_resource.start >> 16) & 0xfff0) |
	       (window_end & 0xfff00000);
}

static u32 bcm3380_pcie_ubus_bar0_basemask(void)
{
	resource_size_t window_end = bcm3380_pcie_mem_resource.start +
				    resource_size(&bcm3380_pcie_mem_resource);

	return ((bcm3380_pcie_mem_resource.start >> 20)
		<< PCIE_UBUS2PCIE_BAR0_BASE_SHIFT) |
	       ((window_end >> 20) << PCIE_UBUS2PCIE_BAR0_MASK_SHIFT) |
	       PCIE_UBUS2PCIE_BAR0_REMAP_ENABLE;
}

static void bcm3380_pcie_setup(struct bcm3380_pcie *priv)
{
	__raw_writel(PCIE_BUS_NUMBER_INIT, priv->base + PCIE_BUS_NUMBER_REG);
	__raw_writel(bcm3380_pcie_bridge_memory_base_limit(),
		     priv->base + PCIE_MEMORY_BASE_LIMIT_REG);
	__raw_writel(PCIE_PREF_MEMORY_DISABLED,
		     priv->base + PCIE_PREF_MEMORY_BASE_LIMIT_REG);
	__raw_writel(0, priv->base + PCIE_PREF_BASE_UPPER32_REG);
	__raw_writel(0, priv->base + PCIE_PREF_LIMIT_UPPER32_REG);
	__raw_writel(PCIE_DEVICE_CONTROL_INIT,
		     priv->base + PCIE_DEVICE_CONTROL_REG);

	u32 val = __raw_readl(priv->base + PCIE_IDVAL3_REG);

	val &= ~PCIE_IDVAL3_CLASS_CODE_MASK;
	val |= PCI_CLASS_BRIDGE_PCI << PCIE_IDVAL3_SUBCLASS_SHIFT;
	__raw_writel(val, priv->base + PCIE_IDVAL3_REG);

	val = __raw_readl(priv->base + PCIE_CONFIG2_REG);
	val &= ~PCIE_CONFIG2_BAR1_SIZE_MASK;
	__raw_writel(val, priv->base + PCIE_CONFIG2_REG);

	__raw_writel(bcm3380_pcie_ubus_bar0_basemask(),
		     priv->base + PCIE_UBUS2PCIE_BAR0_BASEMASK_REG);
	__raw_writel((bcm3380_pcie_mem_resource.start >> 20)
		     << PCIE_UBUS2PCIE_BAR0_REMAP_SHIFT,
		     priv->base + PCIE_UBUS2PCIE_BAR0_REMAP_REG);

	val = __raw_readl(priv->base + PCIE_RC_CFG_0800_REG);
	val |= PCIE_RC_CFG_0800_INIT_BITS;
	__raw_writel(val, priv->base + PCIE_RC_CFG_0800_REG);

	val = __raw_readl(priv->base + PCIE_RC_CFG_0804_REG);
	val |= PCIE_RC_CFG_0804_INIT_BITS;
	__raw_writel(val, priv->base + PCIE_RC_CFG_0804_REG);

	__raw_writel(PCIE_BAR_REMAP_ENABLE,
		     priv->base + PCIE_BAR1_REMAP_REG);
	__raw_writel(PCIE_BAR_REMAP_ENABLE,
		     priv->base + PCIE_BAR2_REMAP_REG);

	__raw_writel(PCIE_RC_CFG_0558_INIT,
		     priv->base + PCIE_RC_CFG_0558_REG);
	__raw_writel(0, priv->base + PCIE_RC_CFG_055c_REG);
	__raw_writel(PCIE_RC_CFG_0560_INIT,
		     priv->base + PCIE_RC_CFG_0560_REG);
	__raw_writel(0, priv->base + PCIE_RC_CFG_0564_REG);

	__raw_writel(bcm3380_pcie_mem_resource.start,
		     priv->base + PCIE_DEVICE_OFFSET + PCI_BASE_ADDRESS_0);

	val = __raw_readl(priv->base + PCIE_DEVICE_OFFSET + PCIE_COMMAND_STATUS_REG);
	val |= PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER;
	__raw_writel(val, priv->base + PCIE_DEVICE_OFFSET + PCIE_COMMAND_STATUS_REG);

	val = __raw_readl(priv->base + PCIE_COMMAND_STATUS_REG);
	val |= PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER;
	__raw_writel(val, priv->base + PCIE_COMMAND_STATUS_REG);
}

static int bcm3380_pcie_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct device_node *np = dev->of_node;
	struct bcm3380_pcie *priv = &bcm3380_pcie;
	struct resource *res = platform_get_resource_byname(pdev, IORESOURCE_MEM,
							    "pcie");
	LIST_HEAD(resources);
	int ret;

	dev_info(dev, "starting BCM3380 PCIe bring-up\n");
	priv->clks = NULL;
	priv->num_clks = 0;
	priv->gphy = NULL;

	of_pci_check_probe_only();

	priv->base = devm_ioremap_resource(dev, res);
	if (IS_ERR(priv->base))
		return PTR_ERR(priv->base);

	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "serdes");
	if (!res)
		return -EINVAL;

	/*
	 * TimerRegs is shared with the watchdog.  The DTS resource covers only
	 * TimerRegs.SerdesCntrl and TimerRegs.SerdesStatus.
	 */
	priv->serdes = devm_ioremap_resource(dev, res);
	if (IS_ERR(priv->serdes))
		return PTR_ERR(priv->serdes);

	ret = gphy_get(dev, &priv->gphy);
	if (ret && ret != -ENODEV)
		return dev_err_probe(dev, ret, "failed to get GPHY provider\n");
	if (!ret) {
		ret = devm_add_action_or_reset(dev, gphy_put, priv->gphy);
		if (ret)
			return ret;
		dev_info(dev, "Using shared GPHY provider\n");
	}

	priv->irq = platform_get_irq(pdev, 0);
	if (priv->irq < 0)
		return priv->irq;

	bmips_pci_irq = priv->irq;

	priv->reset = devm_reset_control_get_exclusive(dev, "pcie");
	if (IS_ERR(priv->reset))
		return PTR_ERR(priv->reset);

	priv->reset_core = devm_reset_control_get_exclusive(dev, "pcie-core");
	if (IS_ERR(priv->reset_core))
		return PTR_ERR(priv->reset_core);

	priv->reset_perst = devm_reset_control_get_exclusive(dev, "perst");
	if (IS_ERR(priv->reset_perst))
		return PTR_ERR(priv->reset_perst);

	priv->num_clks = devm_clk_bulk_get_all(dev, &priv->clks);
	if (priv->num_clks < 0)
		return dev_err_probe(dev, priv->num_clks,
				     "could not get clocks\n");

	priv->enable_gpio = devm_gpiod_get_optional(dev, "pcie-enable",
						    GPIOD_OUT_LOW);
	if (IS_ERR(priv->enable_gpio))
		return PTR_ERR(priv->enable_gpio);

	if (priv->enable_gpio)
		gpiod_set_value_cansleep(priv->enable_gpio, 1);

	if (priv->enable_gpio)
		mdelay(10);

	ret = clk_bulk_prepare_enable(priv->num_clks, priv->clks);
	if (ret)
		return dev_err_probe(dev, ret, "could not enable PCIe clocks\n");

	dev_info(dev, "PCIe clocks enabled\n");

	ret = devm_add_action_or_reset(dev, bcm3380_pcie_disable_clks, priv);
	if (ret)
		return ret;

	bcm3380_pcie_enable_serdes(priv);

	pci_load_of_ranges(&bcm3380_pcie_controller, np);
	if (!bcm3380_pcie_mem_resource.start)
		return -EINVAL;

	of_pci_parse_bus_range(np, &bcm3380_pcie_busn_resource);
	pci_add_resource(&resources, &bcm3380_pcie_busn_resource);

	bcm3380_pcie_reset(priv);

	ret = bcm3380_pcie_wait_link(dev, priv);
	if (ret)
		return ret;

	bcm3380_pcie_setup(priv);

	dev_info(dev, "PCIe link up: endpoint id=0x%08x, window=%pR\n",
		 __raw_readl(priv->base + PCIE_DEVICE_OFFSET),
		 &bcm3380_pcie_mem_resource);

	register_pci_controller(&bcm3380_pcie_controller);

	return 0;
}

static const struct of_device_id bcm3380_pcie_of_match[] = {
	{ .compatible = "brcm,bcm3380-pcie", },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, bcm3380_pcie_of_match);

static struct platform_driver bcm3380_pcie_driver = {
	.probe = bcm3380_pcie_probe,
	.driver	= {
		.name = "bcm3380-pcie",
		.of_match_table = bcm3380_pcie_of_match,
	},
};
module_platform_driver(bcm3380_pcie_driver);

MODULE_DESCRIPTION("BCM3380 PCIe Controller Driver");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:bcm3380-pcie");

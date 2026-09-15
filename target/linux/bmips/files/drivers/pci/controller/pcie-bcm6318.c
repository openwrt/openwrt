// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * BCM6318 PCIe Controller Driver
 *
 * Copyright (C) 2026 Hang Zhou <929513338@qq.com>
 * Copyright (C) 2020 Álvaro Fernández Rojas <noltari@gmail.com>
 * Copyright (C) 2015 Jonas Gorski <jonas.gorski@gmail.com>
 * Copyright (C) 2008 Maxime Bizon <mbizon@freebox.fr>
 */

#include <linux/bitops.h>
#include <linux/clk.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/mfd/syscon.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/of_irq.h>
#include <linux/of_device.h>
#include <linux/of_pci.h>
#include <linux/of_platform.h>
#include <linux/pci.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>
#include <linux/reset.h>
#include <linux/types.h>
#include <linux/version.h>
#include <linux/vmalloc.h>

#include "../pci.h"

#define PCIE_BUS_BRIDGE			0
#define PCIE_BUS_DEVICE			1

#define PCIE_SPECIFIC_REG		0x188
#define SPECIFIC_ENDIAN_MODE_BAR1_SHIFT	0
#define SPECIFIC_ENDIAN_MODE_BAR1_MASK	(0x3 << SPECIFIC_ENDIAN_MODE_BAR1_SHIFT)
#define SPECIFIC_ENDIAN_MODE_BAR2_SHIFT	2
#define SPECIFIC_ENDIAN_MODE_BAR2_MASK	(0x3 << SPECIFIC_ENDIAN_MODE_BAR1_SHIFT)
#define SPECIFIC_ENDIAN_MODE_BAR3_SHIFT	4
#define SPECIFIC_ENDIAN_MODE_BAR3_MASK	(0x3 << SPECIFIC_ENDIAN_MODE_BAR1_SHIFT)
#define SPECIFIC_ENDIAN_MODE_WORD_ALIGN	0
#define SPECIFIC_ENDIAN_MODE_HALFWORD_ALIGN 1
#define SPECIFIC_ENDIAN_MODE_BYTE_ALIGN	2

#define PCIE_CONFIG2_REG		0x408
#define CONFIG2_BAR1_SIZE_EN		1
#define CONFIG2_BAR1_SIZE_MASK		0xf

#define PCIE_IDVAL3_REG			0x43c
#define IDVAL3_CLASS_CODE_MASK		0xffffff
#define IDVAL3_SUBCLASS_SHIFT		8
#define IDVAL3_CLASS_SHIFT		16

#define PCIE_DLSTATUS_REG		0x1048
#define DLSTATUS_PHYLINKUP		(1 << 13)

#define PCIE_CPU_2_PCIE_MEM_WIN0_LO_REG	0x400c
#define C2P_MEM_WIN_ENDIAN_MODE_MASK	0x3
#define C2P_MEM_WIN_ENDIAN_NO_SWAP	0
#define C2P_MEM_WIN_ENDIAN_HALF_WORD_SWAP 1
#define C2P_MEM_WIN_ENDIAN_HALF_BYTE_SWAP 2
#define C2P_MEM_WIN_BASE_ADDR_SHIFT	20
#define C2P_MEM_WIN_BASE_ADDR_MASK	(0xfff << C2P_MEM_WIN_BASE_ADDR_SHIFT)

#define PCIE_RC_BAR1_CONFIG_LO_REG	0x402c
#define RC_BAR_CFG_LO_SIZE_256MB	0xd
#define RC_BAR_CFG_LO_MATCH_ADDR_SHIFT	20
#define RC_BAR_CFG_LO_MATCH_ADDR_MASK	(0xfff << RC_BAR_CFG_LO_MATCH_ADDR_SHIFT)

#define PCIE_CPU_2_PCIE_MEM_WIN0_BASELIMIT_REG 0x4070
#define C2P_BASELIMIT_LIMIT_SHIFT	20
#define C2P_BASELIMIT_LIMIT_MASK	(0xfff << C2P_BASELIMIT_LIMIT_SHIFT)
#define C2P_BASELIMIT_BASE_SHIFT	4
#define C2P_BASELIMIT_BASE_MASK		(0xfff << C2P_BASELIMIT_BASE_SHIFT)

#define PCIE_UBUS_BAR1_CFG_REMAP_REG	0x4088
#define BAR1_CFG_REMAP_OFFSET_SHIFT	20
#define BAR1_CFG_REMAP_OFFSET_MASK	(0xfff << BAR1_CFG_REMAP_OFFSET_SHIFT)
#define BAR1_CFG_REMAP_ACCESS_EN	1

#define PCIE_HARD_DEBUG_REG		0x4204
#define HARD_DEBUG_SERDES_IDDQ		(1 << 23)

#define PCIE_CPU_INT1_MASK_CLEAR_REG	0x830c
#define CPU_INT_PCIE_ERR_ATTN_CPU	(1 << 0)
#define CPU_INT_PCIE_INTA		(1 << 1)
#define CPU_INT_PCIE_INTB		(1 << 2)
#define CPU_INT_PCIE_INTC		(1 << 3)
#define CPU_INT_PCIE_INTD		(1 << 4)
#define CPU_INT_PCIE_INTR		(1 << 5)
#define CPU_INT_PCIE_NMI		(1 << 6)
#define CPU_INT_PCIE_UBUS		(1 << 7)
#define CPU_INT_IPI			(1 << 8)

#define PCIE_EXT_CFG_INDEX_REG		0x8400
#define EXT_CFG_FUNC_NUM_SHIFT		12
#define EXT_CFG_FUNC_NUM_MASK		(0x7 << EXT_CFG_FUNC_NUM_SHIFT)
#define EXT_CFG_DEV_NUM_SHIFT		15
#define EXT_CFG_DEV_NUM_MASK		(0xf << EXT_CFG_DEV_NUM_SHIFT)
#define EXT_CFG_BUS_NUM_SHIFT		20
#define EXT_CFG_BUS_NUM_MASK		(0xff << EXT_CFG_BUS_NUM_SHIFT)

#define PCIE_DEVICE_OFFSET		0x9000

/* PcieCfgType1Rc.StatusCommand.{MemSpace,BusMaster} */
#define PCIE_COMMAND_STATUS_REG		0x004

/* IntControlSoftReset.Bits.SoftRstPcie0Core */
#define BCM3383_CORE_RESET_PCIE0_CORE	BIT(18)

struct bcm6318_pcie;

struct pcie_variant_data {
	const char *name;
	const char * const *reset_names;
	unsigned int num_resets;
	bool needs_softreset;
	bool uses_top_reset;
	void (*reset)(struct bcm6318_pcie *priv);
	void (*setup)(struct bcm6318_pcie *priv);
};

struct bcm6318_pcie {
	void __iomem *base;
	int irq;
	const struct pcie_variant_data *variant;
	struct clk_bulk_data *clks;
	int num_clks;
	struct reset_control *reset;
	union {
		/* BCM6318 */
		struct {
			struct reset_control *ext;
			struct reset_control *core;
			struct reset_control *hard;
		};

		/* BCM3383 */
		struct {
			struct reset_control *top;
			struct regmap *soft;
		};
	} resets;
};

static struct bcm6318_pcie bcm6318_pcie;

extern int bmips_pci_irq;

/*
 * swizzle 32bits data to return only the needed part
 */
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

static int bcm6318_pcie_can_access(struct pci_bus *bus, int devfn)
{
	struct bcm6318_pcie *priv = &bcm6318_pcie;

	switch (bus->number) {
	case PCIE_BUS_BRIDGE:
		return PCI_SLOT(devfn) == 0;
	case PCIE_BUS_DEVICE:
		if (PCI_SLOT(devfn) == 0)
			return __raw_readl(priv->base + PCIE_DLSTATUS_REG)
					& DLSTATUS_PHYLINKUP;
		fallthrough;
	default:
		return false;
	}
}

static int bcm6318_pcie_read(struct pci_bus *bus, unsigned int devfn,
			     int where, int size, u32 *val)
{
	struct bcm6318_pcie *priv = &bcm6318_pcie;
	u32 data;
	u32 reg = where & ~3;

	if (!bcm6318_pcie_can_access(bus, devfn))
		return PCIBIOS_DEVICE_NOT_FOUND;

	if (bus->number == PCIE_BUS_DEVICE)
		reg += PCIE_DEVICE_OFFSET;

	data = __raw_readl(priv->base + reg);
	*val = postprocess_read(data, where, size);

	return PCIBIOS_SUCCESSFUL;
}

static int bcm6318_pcie_write(struct pci_bus *bus, unsigned int devfn,
			      int where, int size, u32 val)
{
	struct bcm6318_pcie *priv = &bcm6318_pcie;
	u32 data;
	u32 reg = where & ~3;

	if (!bcm6318_pcie_can_access(bus, devfn))
		return PCIBIOS_DEVICE_NOT_FOUND;

	if (bus->number == PCIE_BUS_DEVICE)
		reg += PCIE_DEVICE_OFFSET;

	data = __raw_readl(priv->base + reg);
	data = preprocess_write(data, val, where, size);
	__raw_writel(data, priv->base + reg);

	return PCIBIOS_SUCCESSFUL;
}

static struct pci_ops bcm6318_pcie_ops = {
	.read = bcm6318_pcie_read,
	.write = bcm6318_pcie_write,
};

static struct resource bcm6318_pcie_io_resource;
static struct resource bcm6318_pcie_mem_resource;
static struct resource bcm6318_pcie_busn_resource;

static struct pci_controller bcm6318_pcie_controller = {
	.pci_ops = &bcm6318_pcie_ops,
	.io_resource = &bcm6318_pcie_io_resource,
	.mem_resource = &bcm6318_pcie_mem_resource,
};

static void pcie_disable_clks(void *data)
{
	struct bcm6318_pcie *priv = data;

	clk_bulk_disable_unprepare(priv->num_clks, priv->clks);
}

static void bcm6318_pcie_reset(struct bcm6318_pcie *priv)
{
	u32 val;

	reset_control_deassert(priv->resets.hard);

	reset_control_assert(priv->reset);
	reset_control_assert(priv->resets.core);
	reset_control_assert(priv->resets.ext);
	mdelay(10);

	reset_control_deassert(priv->resets.ext);
	mdelay(10);

	reset_control_deassert(priv->reset);
	mdelay(10);

	val = __raw_readl(priv->base + PCIE_HARD_DEBUG_REG);
	val &= ~HARD_DEBUG_SERDES_IDDQ;
	__raw_writel(val, priv->base + PCIE_HARD_DEBUG_REG);
	mdelay(10);

	reset_control_deassert(priv->resets.core);
	mdelay(200);
}

static void bcm3383_pcie_reset(struct bcm6318_pcie *priv)
{
	u32 val;

	regmap_update_bits(priv->resets.soft, 0,
			   BCM3383_CORE_RESET_PCIE0_CORE, 0);
	reset_control_assert(priv->reset);
	reset_control_assert(priv->resets.top);
	mdelay(10);

	reset_control_deassert(priv->reset);
	mdelay(10);

	reset_control_deassert(priv->resets.top);
	mdelay(10);

	val = __raw_readl(priv->base + PCIE_HARD_DEBUG_REG);
	val &= ~HARD_DEBUG_SERDES_IDDQ;
	__raw_writel(val, priv->base + PCIE_HARD_DEBUG_REG);

	regmap_update_bits(priv->resets.soft, 0,
			   BCM3383_CORE_RESET_PCIE0_CORE,
			   BCM3383_CORE_RESET_PCIE0_CORE);
	mdelay(200);
}

static void bcm6318_pcie_setup(struct bcm6318_pcie *priv)
{
	u32 val;

	__raw_writel(CPU_INT_PCIE_INTA | CPU_INT_PCIE_INTB |
		     CPU_INT_PCIE_INTC | CPU_INT_PCIE_INTD,
		     priv->base + PCIE_CPU_INT1_MASK_CLEAR_REG);

	val = bcm6318_pcie_mem_resource.end & C2P_BASELIMIT_LIMIT_MASK;
	val |= (bcm6318_pcie_mem_resource.start >> C2P_BASELIMIT_LIMIT_SHIFT)
	       << C2P_BASELIMIT_BASE_SHIFT;
	__raw_writel(val, priv->base + PCIE_CPU_2_PCIE_MEM_WIN0_BASELIMIT_REG);

	/* setup class code as bridge */
	val = __raw_readl(priv->base + PCIE_IDVAL3_REG);
	val &= ~IDVAL3_CLASS_CODE_MASK;
	val |= (PCI_CLASS_BRIDGE_PCI << IDVAL3_SUBCLASS_SHIFT);
	__raw_writel(val, priv->base + PCIE_IDVAL3_REG);

	/* disable bar1 size */
	val = __raw_readl(priv->base + PCIE_CONFIG2_REG);
	val &= ~CONFIG2_BAR1_SIZE_MASK;
	__raw_writel(val, priv->base + PCIE_CONFIG2_REG);

	/* set bar0 to little endian */
	val = __raw_readl(priv->base + PCIE_CPU_2_PCIE_MEM_WIN0_LO_REG);
	val |= bcm6318_pcie_mem_resource.start & C2P_MEM_WIN_BASE_ADDR_MASK;
	val |= C2P_MEM_WIN_ENDIAN_HALF_BYTE_SWAP;
	__raw_writel(val, priv->base + PCIE_CPU_2_PCIE_MEM_WIN0_LO_REG);

	__raw_writel(SPECIFIC_ENDIAN_MODE_BYTE_ALIGN,
		     priv->base + PCIE_SPECIFIC_REG);

	__raw_writel(RC_BAR_CFG_LO_SIZE_256MB,
		     priv->base + PCIE_RC_BAR1_CONFIG_LO_REG);

	__raw_writel(BAR1_CFG_REMAP_ACCESS_EN,
		     priv->base + PCIE_UBUS_BAR1_CFG_REMAP_REG);

	__raw_writel(PCIE_BUS_DEVICE << EXT_CFG_BUS_NUM_SHIFT,
		     priv->base + PCIE_EXT_CFG_INDEX_REG);
}

static void bcm3383_pcie_setup(struct bcm6318_pcie *priv)
{
	u32 val;

	bcm6318_pcie_setup(priv);

	val = __raw_readl(priv->base + PCIE_COMMAND_STATUS_REG);
	val |= PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER;
	__raw_writel(val, priv->base + PCIE_COMMAND_STATUS_REG);
}

static int bcm6318_pcie_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct device_node *np = dev->of_node;
	struct bcm6318_pcie *priv = &bcm6318_pcie;
	struct resource *res;
	struct reset_control_bulk_data resets[5] = {};
	int ret;
	LIST_HEAD(resources);

	priv->variant = of_device_get_match_data(dev);
	if (!priv->variant)
		return dev_err_probe(dev, -EINVAL, "missing PCIe variant data\n");

	dev_info(dev, "starting %s PCIe bring-up\n", priv->variant->name);
	priv->clks = NULL;
	priv->num_clks = 0;
	priv->reset = NULL;
	priv->resets.ext = NULL;
	priv->resets.core = NULL;
	priv->resets.hard = NULL;

	of_pci_check_probe_only();

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	priv->base = devm_ioremap_resource(dev, res);
	if (IS_ERR(priv->base))
		return PTR_ERR(priv->base);

	priv->irq = platform_get_irq(pdev, 0);
	if (!priv->irq)
		return -ENODEV;

	bmips_pci_irq = priv->irq;

	if (priv->variant->needs_softreset) {
		priv->resets.soft =
			syscon_regmap_lookup_by_phandle(np, "brcm,softreset");
		if (IS_ERR(priv->resets.soft))
			return dev_err_probe(dev, PTR_ERR(priv->resets.soft),
					     "could not get SoftReset syscon\n");
	}

	if (priv->variant->num_resets > ARRAY_SIZE(resets))
		return dev_err_probe(dev, -EINVAL, "too many PCIe resets\n");

	for (unsigned int i = 0; i < priv->variant->num_resets; i++)
		resets[i].id = priv->variant->reset_names[i];

	ret = devm_reset_control_bulk_get_exclusive(dev, priv->variant->num_resets, resets);
	if (ret)
		return dev_err_probe(dev, ret, "could not get PCIe resets\n");

	priv->reset = resets[0].rstc;
	if (priv->variant->uses_top_reset) {
		priv->resets.top = resets[1].rstc;
	} else {
		priv->resets.ext = resets[1].rstc;
		priv->resets.core = resets[2].rstc;
		priv->resets.hard = resets[3].rstc;
	}

	priv->num_clks = devm_clk_bulk_get_all(dev, &priv->clks);
	if (priv->num_clks < 0)
		return dev_err_probe(dev, priv->num_clks,
				     "could not get PCIe clocks\n");

	ret = clk_bulk_prepare_enable(priv->num_clks, priv->clks);
	if (ret) {
		return dev_err_probe(dev, ret, "could not enable PCIe clocks\n");
	}

	ret = devm_add_action_or_reset(dev, pcie_disable_clks, priv);
	if (ret) {
		return ret;
	}

	pci_load_of_ranges(&bcm6318_pcie_controller, np);
	if (!bcm6318_pcie_mem_resource.start)
		return -EINVAL;

	of_pci_parse_bus_range(np, &bcm6318_pcie_busn_resource);
	pci_add_resource(&resources, &bcm6318_pcie_busn_resource);

	priv->variant->reset(priv);
	priv->variant->setup(priv);

	register_pci_controller(&bcm6318_pcie_controller);

	return 0;
}

static const char * const bcm6318_pcie_reset_names[] = {
	"pcie",
	"pcie-ext",
	"pcie-core",
	"pcie-hard",
};

static const char * const bcm3383_pcie_reset_names[] = {
	"pcie",
	"pcie-top",
};

static const struct pcie_variant_data bcm6318_pcie_variant_data = {
	.name = "BCM6318",
	.reset_names = bcm6318_pcie_reset_names,
	.num_resets = ARRAY_SIZE(bcm6318_pcie_reset_names),
	.needs_softreset = false,
	.uses_top_reset = false,
	.reset = bcm6318_pcie_reset,
	.setup = bcm6318_pcie_setup,
};

static const struct pcie_variant_data bcm3383_pcie_variant_data = {
	.name = "BCM3383",
	.reset_names = bcm3383_pcie_reset_names,
	.num_resets = ARRAY_SIZE(bcm3383_pcie_reset_names),
	.needs_softreset = true,
	.uses_top_reset = true,
	.reset = bcm3383_pcie_reset,
	.setup = bcm3383_pcie_setup,
};

static const struct of_device_id bcm6318_pcie_of_match[] = {
	{ .compatible = "brcm,bcm6318-pcie", .data = &bcm6318_pcie_variant_data },
	{ .compatible = "brcm,bcm3383-pcie", .data = &bcm3383_pcie_variant_data },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, bcm6318_pcie_of_match);

static struct platform_driver bcm6318_pcie_driver = {
	.probe = bcm6318_pcie_probe,
	.driver	= {
		.name = "bcm6318-pcie",
		.of_match_table = bcm6318_pcie_of_match,
	},
};
module_platform_driver(bcm6318_pcie_driver);

MODULE_AUTHOR("Álvaro Fernández Rojas <noltari@gmail.com>");
MODULE_DESCRIPTION("BCM6318 PCIe Controller Driver");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:bcm6318-pcie");

// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * BCM3384 PCIe Controller Driver
 *
 * Copyright (C) 2026 Hang Zhou <929513338@qq.com>
 */

#include <linux/bitops.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/iopoll.h>
#include <linux/kernel.h>
#include <linux/mfd/syscon.h>
#include <linux/module.h>
#include <linux/of_irq.h>
#include <linux/of_pci.h>
#include <linux/pci.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>
#include <linux/regulator/consumer.h>
#include <linux/reset.h>
#include <linux/types.h>
#include <linux/vmalloc.h>

#include "../pci.h"

#define PCIE_BUS_BRIDGE			0
#define PCIE_BUS_DEVICE			1
#define PCIE_NUM_OUTBOUND_WINS		4
#define PCIE_NUM_PORTS			2

#define PCIE_VENDOR_SPECIFIC_REG1	0x0188
#define  PCIE_ENDIAN_MODE_BAR2_SHIFT	2
#define  PCIE_ENDIAN_MODE_BAR2_MASK	GENMASK(3, 2)
#define  PCIE_ENDIAN_MODE_BYTE_ALIGN	2

#define PCIE_PRIV1_ID_VAL3		0x043c
#define  PCIE_ID_VAL3_CLASS_CODE_MASK	0x00ffffff
#define  PCIE_ID_VAL3_SUBCLASS_SHIFT	8

/* BCHP_PCIE_MISC_{0,1}_MISC_CTRL */
#define PCIE_MISC_CTRL			0x4008
#define  PCIE_MISC_CTRL_SCB_ACCESS_EN	BIT(12)
#define  PCIE_MISC_CTRL_CFG_READ_UR_MODE	BIT(13)
#define  PCIE_MISC_CTRL_MAX_BURST_256B	BIT(20)

/* BCHP_PCIE_MISC_{0,1}_CPU_2_PCIE_MEM_WIN0_LO */
#define PCIE_MEM_WIN0_LO		0x400c
#define PCIE_MEM_WIN_LO(win)		(PCIE_MEM_WIN0_LO + (win) * 0x8)
#define  PCIE_MEM_WIN_ENDIAN_HALF_BYTE_SWAP	2

/* BCHP_PCIE_MISC_{0,1}_CPU_2_PCIE_MEM_WIN0_HI */
#define PCIE_MEM_WIN0_HI		0x4010
#define PCIE_MEM_WIN_HI(win)		(PCIE_MEM_WIN0_HI + (win) * 0x8)

/* BCHP_PCIE_MISC_{0,1}_RC_BAR1_CONFIG_LO */
#define PCIE_RC_BAR1_CONFIG_LO		0x402c

/* BCHP_PCIE_MISC_{0,1}_RC_BAR2_CONFIG_{LO,HI} */
#define PCIE_RC_BAR2_CONFIG_LO		0x4034
#define PCIE_RC_BAR2_CONFIG_HI		0x4038

/* BCHP_PCIE_MISC_{0,1}_RC_BAR3_CONFIG_LO */
#define PCIE_RC_BAR3_CONFIG_LO		0x403c

/* BCHP_PCIE_MISC_{0,1}_MSI_BAR_CONFIG_LO */
#define PCIE_MSI_BAR_CONFIG_LO		0x4044

/* BCHP_PCIE_MISC_{0,1}_PCIE_STATUS */
#define PCIE_MISC_PCIE_STATUS		0x4068
#define  PCIE_STATUS_PHYLINKUP		BIT(4)
#define  PCIE_STATUS_DL_ACTIVE		BIT(5)
#define  PCIE_STATUS_LINK_UP		(PCIE_STATUS_PHYLINKUP | \
					 PCIE_STATUS_DL_ACTIVE)

/* BCHP_PCIE_MISC_{0,1}_CPU_2_PCIE_MEM_WIN0_BASE_LIMIT */
#define PCIE_MEM_WIN0_BASE_LIMIT	0x4070
#define PCIE_MEM_WIN_BASE_LIMIT(win)	(PCIE_MEM_WIN0_BASE_LIMIT + \
					 (win) * 0x4)
#define  PCIE_MEM_WIN_BASE_SHIFT	4
#define  PCIE_MEM_WIN_LIMIT_SHIFT	20

/* BCHP_PCIE_MISC_{0,1}_HARD_PCIE_HARD_DEBUG */
#define PCIE_HARD_DEBUG		0x4204
#define  PCIE_HARD_DEBUG_CLKREQ_DEBUG_ENABLE	BIT(1)
#define  PCIE_HARD_DEBUG_SERDES_IDDQ		BIT(23)

/* BCHP_PCIE_INTR2_CPU_{0,1}_{CLEAR,MASK_CLEAR,MASK_SET} */
#define PCIE_INTR2_CPU_CLEAR		0x4300
#define PCIE_INTR2_CPU_MASK_CLEAR	0x4308
#define PCIE_INTR2_CPU_MASK_SET		0x430c

/* BCHP_PCIE_CPU_INTR1_{0,1}_MASK_SET / MASK_CLEAR */
#define PCIE_CPU_INTR1_MASK_SET		0x8308
#define PCIE_CPU_INTR1_MASK_CLEAR	0x830c
#define  PCIE_CPU_INTR_INTA		BIT(1)
#define  PCIE_CPU_INTR_INTB		BIT(2)
#define  PCIE_CPU_INTR_INTC		BIT(3)
#define  PCIE_CPU_INTR_INTD		BIT(4)

/*
 * BCM3384 GPL:
 *   BCHP_PCIE_EXT_CFG_{0,1}_REG_START      = base + 0x8400
 *   BCHP_PCIE_EXT_CFG_DATA_{0,1}_REG_START = base + 0x9000
 */
#define PCIE_EXT_CFG_INDEX		0x8400
#define PCIE_EXT_CFG_DATA		0x9000
#define  PCIE_EXT_CFG_FUNC_NUM_SHIFT	12
#define  PCIE_EXT_CFG_DEV_NUM_SHIFT	15
#define  PCIE_EXT_CFG_BUS_NUM_SHIFT	20

/* PcieCfgType1Rc.StatusCommand */
#define PCIE_COMMAND_STATUS		0x0004
#define PCIE_PRI_SEC_BUS_NO		0x0018
#define PCIE_RC_MEM_BASE_LIMIT		0x0020
#define PCIE_RC_PREF_BASE_LIMIT		0x0024

/* GPIO_PER.STRAP_* offsets relative to the BCM3384 gpio_cntl syscon. */
#define GPIO_PER_STRAP_BUS		0x94
#define GPIO_PER_STRAP_OVERRIDE		0x98
#define GPIO_PER_STRAP_OUT_BUS		0xa0
/* GPL bchp_gpio_per.h: GPIO_PER.STRAP_BUS/STRAP_OUT_BUS.pcie_rc_ep_port{0,1}. */
#define GPIO_PER_STRAP_PCIE_ROOT_COMPLEX(id)	BIT(21 + (id))
/* GPL bchp_gpio_per.h: GPIO_PER.STRAP_OVERRIDE.STRAP_OVERRIDE. */
#define GPIO_PER_STRAP_OVERRIDE_ENABLE		BIT(0)
struct bcm3384_pcie {
	struct device *dev;
	void __iomem *base;
	struct pci_controller controller;
	struct resource io_resource;
	struct resource mem_resource;
	struct resource busn_resource;
	struct clk_bulk_data *clks;
	int num_clks;
	int irq;
	u32 id;
	struct reset_control *softreset;
	struct regmap *gpio_per;
};

static int bcm3384_pcie_busno[PCIE_NUM_PORTS];

static int bcm3384_pcie0_get_busno(void)
{
	return bcm3384_pcie_busno[0];
}

static int bcm3384_pcie1_get_busno(void)
{
	return bcm3384_pcie_busno[1];
}

static int (* const bcm3384_pcie_get_busno[PCIE_NUM_PORTS])(void) = {
	bcm3384_pcie0_get_busno,
	bcm3384_pcie1_get_busno,
};

static int postprocess_read(u32 data, int where, unsigned int size)
{
	switch (size) {
	case 1:
		return (data >> ((where & 3) << 3)) & 0xff;
	case 2:
		return (data >> ((where & 3) << 3)) & 0xffff;
	case 4:
		return data;
	default:
		return 0;
	}
}

static u32 preprocess_write(u32 orig_data, u32 val, int where,
			    unsigned int size)
{
	switch (size) {
	case 1:
		return (orig_data & ~(0xff << ((where & 3) << 3))) |
		       (val << ((where & 3) << 3));
	case 2:
		return (orig_data & ~(0xffff << ((where & 3) << 3))) |
		       (val << ((where & 3) << 3));
	case 4:
		return val;
	default:
		return orig_data;
	}
}

static struct bcm3384_pcie *bcm3384_pcie_from_bus(struct pci_bus *bus)
{
	struct pci_controller *controller = bus->sysdata;

	return container_of(controller, struct bcm3384_pcie, controller);
}

static bool bcm3384_pcie_link_up(struct bcm3384_pcie *pcie)
{
	u32 val = __raw_readl(pcie->base + PCIE_MISC_PCIE_STATUS);

	return (val & PCIE_STATUS_LINK_UP) == PCIE_STATUS_LINK_UP;
}

static bool bcm3384_pcie_can_access(struct bcm3384_pcie *pcie,
				    struct pci_bus *bus, int devfn)
{
	unsigned int root_bus = pcie->busn_resource.start;
	unsigned int device_bus = root_bus + 1;

	if (bus->number == root_bus)
		return PCI_SLOT(devfn) == 0;

	if (bus->number == device_bus && PCI_SLOT(devfn) == 0)
		return bcm3384_pcie_link_up(pcie);

	return false;
}

static u32 bcm3384_pcie_cfg_index(struct pci_bus *bus, unsigned int devfn)
{
	return (bus->number << PCIE_EXT_CFG_BUS_NUM_SHIFT) |
	       (PCI_SLOT(devfn) << PCIE_EXT_CFG_DEV_NUM_SHIFT) |
	       (PCI_FUNC(devfn) << PCIE_EXT_CFG_FUNC_NUM_SHIFT);
}

static u32 bcm3384_pcie_read_device_config(struct bcm3384_pcie *pcie,
					   struct pci_bus *bus,
					   unsigned int devfn, u32 reg)
{
	__raw_writel(bcm3384_pcie_cfg_index(bus, devfn),
		     pcie->base + PCIE_EXT_CFG_INDEX);

	return __raw_readl(pcie->base + PCIE_EXT_CFG_DATA + reg);
}

static void bcm3384_pcie_write_device_config(struct bcm3384_pcie *pcie,
					     struct pci_bus *bus,
					     unsigned int devfn, u32 reg,
					     u32 data)
{
	__raw_writel(bcm3384_pcie_cfg_index(bus, devfn),
		     pcie->base + PCIE_EXT_CFG_INDEX);
	__raw_writel(data, pcie->base + PCIE_EXT_CFG_DATA + reg);
}

static int bcm3384_pcie_read(struct pci_bus *bus, unsigned int devfn,
			     int where, int size, u32 *val)
{
	struct bcm3384_pcie *pcie = bcm3384_pcie_from_bus(bus);
	u32 reg = where & ~3;
	u32 data;

	if (!bcm3384_pcie_can_access(pcie, bus, devfn))
		return PCIBIOS_DEVICE_NOT_FOUND;

	if (bus->number != pcie->busn_resource.start)
		data = bcm3384_pcie_read_device_config(pcie, bus, devfn, reg);
	else
		data = __raw_readl(pcie->base + reg);

	*val = postprocess_read(data, where, size);

	return PCIBIOS_SUCCESSFUL;
}

static int bcm3384_pcie_write(struct pci_bus *bus, unsigned int devfn,
			      int where, int size, u32 val)
{
	struct bcm3384_pcie *pcie = bcm3384_pcie_from_bus(bus);
	u32 reg = where & ~3;
	u32 data;

	if (!bcm3384_pcie_can_access(pcie, bus, devfn))
		return PCIBIOS_DEVICE_NOT_FOUND;

	if (bus->number != pcie->busn_resource.start)
		data = bcm3384_pcie_read_device_config(pcie, bus, devfn, reg);
	else
		data = __raw_readl(pcie->base + reg);

	data = preprocess_write(data, val, where, size);

	if (bus->number != pcie->busn_resource.start)
		bcm3384_pcie_write_device_config(pcie, bus, devfn, reg, data);
	else
		__raw_writel(data, pcie->base + reg);

	return PCIBIOS_SUCCESSFUL;
}

static struct pci_ops bcm3384_pcie_ops = {
	.read = bcm3384_pcie_read,
	.write = bcm3384_pcie_write,
};

static void bcm3384_pcie_disable_clks(void *data)
{
	struct bcm3384_pcie *pcie = data;

	clk_bulk_disable_unprepare(pcie->num_clks, pcie->clks);
}

static void bcm3384_pcie_disable_regulator(void *data)
{
	regulator_disable(data);
}

static void bcm3384_pcie_log_straps(struct bcm3384_pcie *pcie,
				    const char *stage)
{
	u32 strap_bus;
	u32 strap_override;
	u32 strap_out_bus;

	if (regmap_read(pcie->gpio_per, GPIO_PER_STRAP_BUS, &strap_bus) ||
	    regmap_read(pcie->gpio_per, GPIO_PER_STRAP_OVERRIDE,
			&strap_override) ||
	    regmap_read(pcie->gpio_per, GPIO_PER_STRAP_OUT_BUS,
			&strap_out_bus))
		return;

	dev_info(pcie->dev,
		 "straps %s: StrapBus=0x%08x StrapOverride=0x%08x StrapOut=0x%08x\n",
		 stage, strap_bus, strap_override, strap_out_bus);
}

static void bcm3384_pcie_force_root_complex_straps(struct bcm3384_pcie *pcie)
{
	u32 mask = GPIO_PER_STRAP_PCIE_ROOT_COMPLEX(pcie->id);

	bcm3384_pcie_log_straps(pcie, "before root-complex override");

	regmap_update_bits(pcie->gpio_per, GPIO_PER_STRAP_BUS, mask, mask);
	regmap_update_bits(pcie->gpio_per, GPIO_PER_STRAP_OUT_BUS, mask, mask);
	regmap_update_bits(pcie->gpio_per, GPIO_PER_STRAP_OVERRIDE,
			   GPIO_PER_STRAP_OVERRIDE_ENABLE,
			   GPIO_PER_STRAP_OVERRIDE_ENABLE);

	bcm3384_pcie_log_straps(pcie, "after root-complex override");
}

static void bcm3384_pcie_set_hard_debug(void __iomem *base)
{
	u32 val = __raw_readl(base + PCIE_HARD_DEBUG);

	__raw_writel((val & ~PCIE_HARD_DEBUG_SERDES_IDDQ) |
		     PCIE_HARD_DEBUG_CLKREQ_DEBUG_ENABLE,
		     base + PCIE_HARD_DEBUG);
	__raw_readl(base + PCIE_HARD_DEBUG);
}

static void bcm3384_pcie_set_outbound_win_base(void __iomem *base,
					       unsigned int win, u32 start,
					       u32 size)
{
	u32 limit = start + size - 1;

	__raw_writel(start + PCIE_MEM_WIN_ENDIAN_HALF_BYTE_SWAP,
		     base + PCIE_MEM_WIN_LO(win));
	__raw_writel(0, base + PCIE_MEM_WIN_HI(win));
	__raw_writel(((start >> 20) << PCIE_MEM_WIN_BASE_SHIFT) |
		     ((limit >> 20) << PCIE_MEM_WIN_LIMIT_SHIFT),
		     base + PCIE_MEM_WIN_BASE_LIMIT(win));
}

static void bcm3384_pcie_early_setup_base(void __iomem *base, u32 mem_start)
{
	u32 win_size = 0x08000000;

	__raw_writel(PCIE_MISC_CTRL_SCB_ACCESS_EN |
		     PCIE_MISC_CTRL_CFG_READ_UR_MODE |
		     PCIE_MISC_CTRL_MAX_BURST_256B,
		     base + PCIE_MISC_CTRL);

	for (unsigned int win = 0; win < PCIE_NUM_OUTBOUND_WINS; win++) {
		bcm3384_pcie_set_outbound_win_base(base, win, mem_start,
						   win_size);
		mem_start += win_size;
	}

	__raw_writel(0x0000000f, base + PCIE_RC_BAR2_CONFIG_LO);
	__raw_writel(0x00000000, base + PCIE_RC_BAR2_CONFIG_HI);
	__raw_writel(0x00000000, base + PCIE_RC_BAR1_CONFIG_LO);
	__raw_writel(0x00000000, base + PCIE_RC_BAR3_CONFIG_LO);
	__raw_writel(0x00000000, base + PCIE_MSI_BAR_CONFIG_LO);
	__raw_writel(0x00000000, base + PCIE_INTR2_CPU_CLEAR);
	__raw_writel(0x00000000, base + PCIE_INTR2_CPU_MASK_CLEAR);
	__raw_writel(0xffffffff, base + PCIE_INTR2_CPU_MASK_SET);
}

static int bcm3384_pcie_reset(struct bcm3384_pcie *pcie)
{
	u32 val;
	int ret;

	bcm3384_pcie_force_root_complex_straps(pcie);

	struct regulator *vpcie =
		devm_regulator_get_optional(pcie->dev, "vpcie");
	if (IS_ERR(vpcie)) {
		if (PTR_ERR(vpcie) != -ENODEV)
			return dev_err_probe(pcie->dev, PTR_ERR(vpcie),
					     "failed to get PCIe supply\n");
	} else {
		ret = regulator_enable(vpcie);
		if (ret)
			return dev_err_probe(pcie->dev, ret,
					     "failed to enable PCIe supply\n");

		ret = devm_add_action_or_reset(pcie->dev,
					       bcm3384_pcie_disable_regulator,
					       vpcie);
		if (ret)
			return ret;

		msleep(100);
	}

	ret = reset_control_assert(pcie->softreset);
	if (ret)
		return ret;

	udelay(100);

	ret = reset_control_deassert(pcie->softreset);
	if (ret)
		return ret;

	__raw_writel(PCIE_CPU_INTR_INTA,
		     pcie->base + PCIE_CPU_INTR1_MASK_SET);
	bcm3384_pcie_set_hard_debug(pcie->base);

	bcm3384_pcie_early_setup_base(pcie->base, pcie->mem_resource.start);

	ret = read_poll_timeout(__raw_readl, val,
				(val & PCIE_STATUS_LINK_UP) == PCIE_STATUS_LINK_UP,
				2000, 1000000, false,
				pcie->base + PCIE_MISC_PCIE_STATUS);
	if (ret)
		dev_warn(pcie->dev,
			 "link did not come up: status=0x%08x hard_debug=0x%08x reset=%d\n",
			 val, __raw_readl(pcie->base + PCIE_HARD_DEBUG),
			 reset_control_status(pcie->softreset));

	bcm3384_pcie_log_straps(pcie, "after link poll");

	return 0;
}

static void bcm3384_pcie_setup(struct bcm3384_pcie *pcie)
{
	unsigned int root_bus = pcie->busn_resource.start;
	unsigned int device_bus = root_bus + 1;
	u32 val;

	val = __raw_readl(pcie->base + PCIE_PRIV1_ID_VAL3);
	val &= ~PCIE_ID_VAL3_CLASS_CODE_MASK;
	val |= PCI_CLASS_BRIDGE_PCI << PCIE_ID_VAL3_SUBCLASS_SHIFT;
	__raw_writel(val, pcie->base + PCIE_PRIV1_ID_VAL3);

	__raw_writel(PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER,
		     pcie->base + PCIE_COMMAND_STATUS);

	__raw_writel((((u32)pcie->mem_resource.end >> 20) << PCIE_MEM_WIN_LIMIT_SHIFT) |
		     ((pcie->mem_resource.start >> 20) << PCIE_MEM_WIN_BASE_SHIFT),
		     pcie->base + PCIE_RC_MEM_BASE_LIMIT);
	__raw_writel(0x0000fff0, pcie->base + PCIE_RC_PREF_BASE_LIMIT);
	__raw_writel(root_bus | device_bus << 8 | device_bus << 16,
		     pcie->base + PCIE_PRI_SEC_BUS_NO);

	val = __raw_readl(pcie->base + PCIE_VENDOR_SPECIFIC_REG1);
	val &= ~PCIE_ENDIAN_MODE_BAR2_MASK;
	val |= PCIE_ENDIAN_MODE_BYTE_ALIGN << PCIE_ENDIAN_MODE_BAR2_SHIFT;
	__raw_writel(val, pcie->base + PCIE_VENDOR_SPECIFIC_REG1);

	__raw_writel(device_bus << PCIE_EXT_CFG_BUS_NUM_SHIFT,
		     pcie->base + PCIE_EXT_CFG_INDEX);
	__raw_writel(PCIE_CPU_INTR_INTA,
		     pcie->base + PCIE_CPU_INTR1_MASK_CLEAR);
}

static int bcm3384_pcie_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct device_node *np = dev_of_node(dev);
	struct bcm3384_pcie *pcie;
	int ret;

	dev_info(dev, "starting BCM3384 PCIe bring-up\n");

	pcie = devm_kzalloc(dev, sizeof(*pcie), GFP_KERNEL);
	if (!pcie)
		return -ENOMEM;

	pcie->dev = dev;

	of_pci_check_probe_only();
	pci_add_flags(PCI_PROBE_ONLY);

	pcie->base = devm_platform_ioremap_resource_byname(pdev, "pcie");
	if (IS_ERR(pcie->base))
		return PTR_ERR(pcie->base);

	ret = of_property_read_u32(np, "brcm,pcie-id", &pcie->id);
	if (ret)
		return dev_err_probe(dev, ret, "missing PCIe ID\n");
	if (pcie->id >= PCIE_NUM_PORTS)
		return dev_err_probe(dev, -EINVAL, "invalid PCIe ID %u\n",
				     pcie->id);

	pcie->softreset = devm_reset_control_get_exclusive(dev, NULL);
	if (IS_ERR(pcie->softreset))
		return dev_err_probe(dev, PTR_ERR(pcie->softreset),
				     "could not get PCIe core reset\n");

	pcie->gpio_per = syscon_regmap_lookup_by_phandle(np, "brcm,gpio-per");
	if (IS_ERR(pcie->gpio_per))
		return dev_err_probe(dev, PTR_ERR(pcie->gpio_per),
				     "could not get GPIO_PER syscon\n");

	pcie->irq = platform_get_irq(pdev, 0);
	if (pcie->irq <= 0)
		return -ENODEV;

	pcie->num_clks = devm_clk_bulk_get_all(dev, &pcie->clks);
	if (pcie->num_clks < 0)
		return dev_err_probe(dev, pcie->num_clks,
				     "could not get PCIe clocks\n");

	ret = clk_bulk_prepare_enable(pcie->num_clks, pcie->clks);
	if (ret)
		return dev_err_probe(dev, ret, "could not enable PCIe clocks\n");

	ret = devm_add_action_or_reset(dev, bcm3384_pcie_disable_clks, pcie);
	if (ret)
		return ret;

	ret = of_pci_parse_bus_range(np, &pcie->busn_resource);
	if (ret)
		return dev_err_probe(dev, ret, "missing PCI bus range\n");

	pcie->controller.pci_ops = &bcm3384_pcie_ops;
	pcie->controller.io_resource = &pcie->io_resource;
	pcie->controller.mem_resource = &pcie->mem_resource;
	pcie->controller.get_busno = bcm3384_pcie_get_busno[pcie->id];
	pci_load_of_ranges(&pcie->controller, np);
	if (!pcie->mem_resource.start)
		return -EINVAL;

	if (!pcie->io_resource.flags) {
		pcie->io_resource.name = dev_name(dev);
		pcie->io_resource.flags = IORESOURCE_IO;
		pcie->io_resource.start = pcie->busn_resource.start;
		pcie->io_resource.end = pcie->busn_resource.start;
	}

	bcm3384_pcie_busno[pcie->id] = pcie->busn_resource.start;

	ret = bcm3384_pcie_reset(pcie);
	if (ret)
		return dev_err_probe(dev, ret, "failed to reset PCIe\n");

	bcm3384_pcie_setup(pcie);

	dev_info(dev,
		 "BCM3384 PCIe status=0x%08x hard_debug=0x%08x reset=%d\n",
		 __raw_readl(pcie->base + PCIE_MISC_PCIE_STATUS),
		 __raw_readl(pcie->base + PCIE_HARD_DEBUG),
		 reset_control_status(pcie->softreset));

	register_pci_controller(&pcie->controller);

	return 0;
}

static const struct of_device_id bcm3384_pcie_of_match[] = {
	{ .compatible = "brcm,bcm3384-pcie" },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, bcm3384_pcie_of_match);

static struct platform_driver bcm3384_pcie_driver = {
	.probe = bcm3384_pcie_probe,
	.driver	= {
		.name = "bcm3384-pcie",
		.of_match_table = bcm3384_pcie_of_match,
	},
};
module_platform_driver(bcm3384_pcie_driver);

MODULE_AUTHOR("Hang Zhou <929513338@qq.com>");
MODULE_DESCRIPTION("BCM3384 PCIe Controller Driver");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:bcm3384-pcie");

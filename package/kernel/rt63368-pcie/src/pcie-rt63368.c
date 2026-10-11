// SPDX-License-Identifier: GPL-2.0
/*
 * RT63365/RT63368 PCIe host bridge.
 *
 * Written for the ZyXEL Keenetic LTE after the vendor RT63365 PCI code.
 * This is an older Ralink-style controller with an indirect
 * CFG_ADDR/CFG_DATA configuration window; it is not the MediaTek/EN7528
 * controller used by EN751221.
 *
 * Only port 0 is used; the second port is powered down. Registers are
 * accessed natively (__raw_*).
 */

#include <linux/bitops.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/of_pci.h>
#include <linux/pci.h>
#include <linux/platform_device.h>
#include <linux/spinlock.h>

/* Offsets in the controller window (0x1fb80000). */
#define PCIE_CTRL		0x0000
#define PCIE_INT_ENABLE		0x000c
#define PCIE_CFG_ADDR		0x0020
#define PCIE_CFG_DATA		0x0024
#define PCIE_RC_BAR0		0x2010
#define PCIE_RC_CLASS		0x2034
#define PCIE_RC_LINK		0x2050

#define PCIE_CTRL_PORT_RESET	BIT(1)
#define PCIE_RC_LINK_UP		BIT(0)

/* SCU registers (inside the clock driver's region, mapped separately). */
#define SCU_PCIEC		0x1fb00088
#define SCU_PCIEC_PORT1_EN	BIT(22)
#define SCU_RSTCTRL		0x1fb00834
#define SCU_RST_PCIE0		BIT(26)
#define SCU_RST_PCIE_HOST	BIT(29)

struct rt63368_pcie {
	struct device *dev;
	void __iomem *base;
	spinlock_t cfg_lock;
};

static inline u32 pcie_read(struct rt63368_pcie *pcie, u32 reg)
{
	return __raw_readl(pcie->base + reg);
}

static inline void pcie_write(struct rt63368_pcie *pcie, u32 reg, u32 val)
{
	__raw_writel(val, pcie->base + reg);
}

static u32 rt63368_cfg_addr(struct pci_bus *bus, unsigned int devfn, int where)
{
	return (bus->number << 24) | (devfn << 16) | (where & 0xffc);
}

static u32 rt63368_cfg_read_word(struct rt63368_pcie *pcie,
				 struct pci_bus *bus, unsigned int devfn,
				 int where)
{
	u32 val;

	pcie_write(pcie, PCIE_CFG_ADDR, rt63368_cfg_addr(bus, devfn, where));
	/* The vendor RT63365 code reads the data register twice. */
	val = pcie_read(pcie, PCIE_CFG_DATA);
	val = pcie_read(pcie, PCIE_CFG_DATA);
	return val;
}

static int rt63368_pci_read(struct pci_bus *bus, unsigned int devfn,
			    int where, int size, u32 *value)
{
	struct rt63368_pcie *pcie = bus->sysdata;
	unsigned long flags;
	u32 data;

	/* Only the root port (00:00.0) and one device behind it exist. */
	if ((bus->number == 0 && devfn != 0) ||
	    (bus->number == 1 && PCI_SLOT(devfn) != 0)) {
		PCI_SET_ERROR_RESPONSE(value);
		return PCIBIOS_DEVICE_NOT_FOUND;
	}

	spin_lock_irqsave(&pcie->cfg_lock, flags);
	data = rt63368_cfg_read_word(pcie, bus, devfn, where);
	spin_unlock_irqrestore(&pcie->cfg_lock, flags);

	if (size == 1)
		data = (data >> ((where & 3) * 8)) & 0xff;
	else if (size == 2)
		data = (data >> ((where & 2) * 8)) & 0xffff;
	*value = data;
	return PCIBIOS_SUCCESSFUL;
}

static int rt63368_pci_write(struct pci_bus *bus, unsigned int devfn,
			     int where, int size, u32 value)
{
	struct rt63368_pcie *pcie = bus->sysdata;
	unsigned long flags;
	u32 data, shift, mask;

	if ((bus->number == 0 && devfn != 0) ||
	    (bus->number == 1 && PCI_SLOT(devfn) != 0))
		return PCIBIOS_DEVICE_NOT_FOUND;

	spin_lock_irqsave(&pcie->cfg_lock, flags);
	if (size == 4) {
		data = value;
	} else {
		data = rt63368_cfg_read_word(pcie, bus, devfn, where);
		shift = (where & (size == 1 ? 3 : 2)) * 8;
		mask = size == 1 ? 0xff : 0xffff;
		data = (data & ~(mask << shift)) | ((value & mask) << shift);
	}
	pcie_write(pcie, PCIE_CFG_ADDR, rt63368_cfg_addr(bus, devfn, where));
	pcie_write(pcie, PCIE_CFG_DATA, data);
	spin_unlock_irqrestore(&pcie->cfg_lock, flags);
	return PCIBIOS_SUCCESSFUL;
}

static struct pci_ops rt63368_pci_ops = {
	.read = rt63368_pci_read,
	.write = rt63368_pci_write,
};

static void rt63368_scu_rmw(phys_addr_t reg, u32 clear, u32 set)
{
	void __iomem *p = ioremap(reg, 4);

	if (!p)
		return;
	__raw_writel((__raw_readl(p) & ~clear) | set, p);
	iounmap(p);
}

static int rt63368_pcie_link_up(struct rt63368_pcie *pcie)
{
	int i;

	if (!(pcie_read(pcie, PCIE_RC_LINK) & PCIE_RC_LINK_UP)) {
		u32 ctrl;

		/* Single radio board: keep the unpowered second port off. */
		rt63368_scu_rmw(SCU_PCIEC, SCU_PCIEC_PORT1_EN, 0);
		mdelay(1);

		rt63368_scu_rmw(SCU_RSTCTRL, 0,
				SCU_RST_PCIE0 | SCU_RST_PCIE_HOST);
		mdelay(1);
		rt63368_scu_rmw(SCU_RSTCTRL,
				SCU_RST_PCIE0 | SCU_RST_PCIE_HOST, 0);
		mdelay(1);

		ctrl = pcie_read(pcie, PCIE_CTRL);
		pcie_write(pcie, PCIE_CTRL, ctrl | PCIE_CTRL_PORT_RESET);
		mdelay(1);
		pcie_write(pcie, PCIE_CTRL, ctrl & ~PCIE_CTRL_PORT_RESET);
		mdelay(1);
	}

	for (i = 0; i < 1000; i++) {
		if (pcie_read(pcie, PCIE_RC_LINK) & PCIE_RC_LINK_UP)
			break;
		mdelay(1);
	}

	if (i == 1000) {
		dev_err(pcie->dev, "link down (RC_LINK=%08x)\n",
			pcie_read(pcie, PCIE_RC_LINK));
		return -ENODEV;
	}
	return 0;
}

static int rt63368_pcie_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct pci_host_bridge *bridge;
	struct rt63368_pcie *pcie;
	int err;

	bridge = devm_pci_alloc_host_bridge(dev, sizeof(*pcie));
	if (!bridge)
		return -ENOMEM;

	pcie = pci_host_bridge_priv(bridge);
	pcie->dev = dev;
	spin_lock_init(&pcie->cfg_lock);

	pcie->base = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(pcie->base))
		return PTR_ERR(pcie->base);

	err = rt63368_pcie_link_up(pcie);
	if (err)
		return err;

	/* Root complex fix-ups used by the vendor driver. */
	pcie_write(pcie, PCIE_RC_BAR0, 0xffff0001);
	pcie_write(pcie, PCIE_RC_CLASS, 0x06040001);

	bridge->sysdata = pcie;
	bridge->ops = &rt63368_pci_ops;
	bridge->map_irq = of_irq_parse_and_map_pci;
	bridge->swizzle_irq = pci_common_swizzle;

	return pci_host_probe(bridge);
}

static const struct of_device_id rt63368_pcie_of_match[] = {
	{ .compatible = "econet,rt63368-pcie" },
	{ }
};
MODULE_DEVICE_TABLE(of, rt63368_pcie_of_match);

static struct platform_driver rt63368_pcie_driver = {
	.probe = rt63368_pcie_probe,
	.driver = {
		.name = "rt63368-pcie",
		.of_match_table = rt63368_pcie_of_match,
		.suppress_bind_attrs = true,
	},
};
module_platform_driver(rt63368_pcie_driver);

MODULE_DESCRIPTION("RT63368 PCIe host bridge");
MODULE_LICENSE("GPL");

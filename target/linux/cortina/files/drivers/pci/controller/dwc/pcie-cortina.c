// SPDX-License-Identifier: GPL-2.0
#include <linux/bitfield.h>
#include <linux/delay.h>
#include <linux/gpio/consumer.h>
#include <linux/interrupt.h>
#include <linux/irqchip/chained_irq.h>
#include <linux/irqdomain.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/pci.h>
#include <linux/platform_device.h>
#include <linux/resource.h>

#include "pcie-designware.h"

#define GLOBAL_JTAG_ID		0x00
#define   JTAG_ID_REV		GENMASK(15, 12)
#define   JTAG_REV_CS7542_A0	0x0
#define   JTAG_REV_CS7522_A0	0x1
#define   JTAG_REV_CS7542_A1	0x8
#define   JTAG_REV_CS7522_A1	0x9
#define GLOBAL_BLOCK_RESET	0x04
#define GLOBAL_CONFIG		0x10
#define GLOBAL_PHY_CONTROL	0x14

#define BLOCK_RESET_PCIE(p)	BIT(6 + (p))

#define PHY_CTL_REFCLKSEL(p, v)	(((v) & 3) << ((p) * 2))
#define PHY_CTL_REFCLKSEL_MASK(p) (3 << ((p) * 2))
#define PHY_CTL_REFCLK_24MHZ	2
#define PHY_CTL_PD(p)		BIT(8 + (p))
#define PHY_CTL_CMU_RESETN(p)	BIT(16 + (p) * 4)
#define PHY_CTL_LN0_RESETN(p)	BIT(17 + (p) * 4)
#define PHY_CTL_POR_N(p)	BIT(19 + (p) * 4)

#define APP_INT_STATUS		0x00
#define APP_INT_ENABLE		0x04
#define APP_CORE_CONFIG		0x1c
#define APP_CMU_OK_DEBUG13	0x6c
#define APP_CFG_START		0x80
#define APP_CFG_END		0x84

#define INT_INTX_ASSERT(x)	BIT((x) * 2)
#define INT_INTX_ASSERT_MASK	(BIT(0) | BIT(2) | BIT(4) | BIT(6))
#define INT_MSI			BIT(8)
#define INT_LINK_UP		BIT(9)
#define INT_RADM_NONFATAL_ERR	BIT(12)
#define INT_RADM_FATAL_ERR	BIT(13)
#define INT_GM_COMPOSER_ERR	BIT(16)
#define INT_RADMX_COMPOSER_ERR	BIT(17)
#define INT_ELASTIC_OVF		BIT(18)
#define INT_ELASTIC_UNF		BIT(19)
#define INT_LINK_DOWN		BIT(22)
#define INT_ERR_MASK		(INT_RADM_NONFATAL_ERR | INT_RADM_FATAL_ERR | \
				 INT_GM_COMPOSER_ERR | INT_RADMX_COMPOSER_ERR | \
				 INT_ELASTIC_OVF | INT_ELASTIC_UNF | INT_LINK_DOWN)

#define CORE_CFG_LTSSM_EN	BIT(0)
#define CORE_CFG_AXI_UR_MASK	BIT(5)

#define CMU_OK			BIT(29)
#define CMU_OK_POLLS		1000
#define CMU_OK_POLL_US		100

#define APP_PORT_STRIDE		0x400
#define APP_PHYS_BASE		0xF00A0000
#define APP_CFG_WINDOW_LEN	0x1000
#define APP_CFG_POLLS		256
#define APP_MSI_TARGET		0xC0
#define RC_CFG_LIMIT		0x200

#define SNOW_COM_REG0		0x00
#define   SNOW_REG0_CFG		0x00090006
#define   SNOW_REG0_AHB_RESETN	BIT(0)
#define SNOW_REV_REG		96
#define   SNOW_REV_REG_A0	0x5e082e00
#define   SNOW_REV_REG_A1	0x5e080600
#define SNOW_COM_LANE_REG0	0xc8
#define   SNOW_LANE_REG0_CFG	0x400290c0
#define   SNOW_LANE_REG0_MASTER_RESETN	BIT(1)

#define PERST_ASSERT_US		100
#define PERST_SETTLE_MS		10

static const u32 cortina_pcie_clken[] = { BIT(9), BIT(11), BIT(12) };

struct cortina_pcie {
	struct dw_pcie		pci;
	void __iomem		*app;
	void __iomem		*global;
	void __iomem		*phy;
	struct irq_domain	*intx_domain;
	raw_spinlock_t		lock;
	resource_size_t		dbi_phys;
	resource_size_t		app_phys;
	int			port;
};

#define to_cortina_pcie(x)	container_of((x), struct cortina_pcie, pci)

struct snow_reg {
	u16 off;
	u32 val;
};

static const struct snow_reg snow_phy_24mhz[] = {
	{ 0,   SNOW_REG0_CFG }, { 4,   0x000e0960 }, { 32,  0x68a00000 },
	{ 44,  0x50040000 }, { 48,  0x40250270 }, { 52,  0x00004001 },
	{ 100, 0xF0914200 }, { 104, 0x4c0c9048 }, { 108, 0x00000373 },
	{ 124, 0x04841000 }, { 128, 0x000000e0 }, { 132, 0x04000023 },
	{ 136, 0x68001038 }, { 140, 0x0d181ea2 }, { 144, 0x0000000c },
	{ 196, 0x0f600000 }, { 200, SNOW_LANE_REG0_CFG }, { 204, 0x0000003c },
	{ 208, 0xc68b8300 }, { 212, 0x98280301 }, { 216, 0xe1782819 },
	{ 220, 0x00f410f0 }, { 232, 0xa0a0a000 }, { 236, 0xa0a0a0a0 },
	{ 240, 0x9fc00068 }, { 244, 0x00000001 }, { 248, 0x00000000 },
	{ 252, 0xd07e4130 }, { 256, 0x935285cc }, { 260, 0xb0dd49e0 },
	{ 264, 0x0000020b }, { 300, 0xd8000000 }, { 304, 0x0001ff1a },
	{ 308, 0xf0000000 }, { 312, 0xffffffff }, { 316, 0x3fc3c21c },
	{ 320, 0x0000000a }, { 324, 0x00f80000 },
};

static void cortina_global_rmw(struct cortina_pcie *cp, u32 reg, u32 clr, u32 set)
{
	u32 v = readl(cp->global + reg);

	v &= ~clr;
	v |= set;
	writel(v, cp->global + reg);
}

static int cortina_pcie_phy_init(struct cortina_pcie *cp)
{
	struct device *dev = cp->pci.dev;
	int p = cp->port;
	u32 jtag, rev, rev_val;
	int i;

	jtag = readl(cp->global + GLOBAL_JTAG_ID);
	rev = FIELD_GET(JTAG_ID_REV, jtag);
	switch (rev) {
	case JTAG_REV_CS7542_A0:
	case JTAG_REV_CS7522_A0:
		rev_val = SNOW_REV_REG_A0;
		break;
	case JTAG_REV_CS7542_A1:
	case JTAG_REV_CS7522_A1:
		rev_val = SNOW_REV_REG_A1;
		break;
	default:
		dev_warn(dev, "unknown SoC revision (JTAG ID 0x%08x), assuming A1\n", jtag);
		rev_val = SNOW_REV_REG_A1;
	}

	cortina_global_rmw(cp, GLOBAL_PHY_CONTROL,
			   PHY_CTL_POR_N(p) | PHY_CTL_LN0_RESETN(p) | PHY_CTL_CMU_RESETN(p) |
			   PHY_CTL_REFCLKSEL_MASK(p),
			   PHY_CTL_PD(p) | PHY_CTL_REFCLKSEL(p, PHY_CTL_REFCLK_24MHZ));
	msleep(100);

	cortina_global_rmw(cp, GLOBAL_PHY_CONTROL,
			   PHY_CTL_PD(p) | PHY_CTL_REFCLKSEL_MASK(p),
			   PHY_CTL_POR_N(p) | PHY_CTL_REFCLKSEL(p, PHY_CTL_REFCLK_24MHZ));
	udelay(10);

	for (i = 0; i < ARRAY_SIZE(snow_phy_24mhz); i++) {
		if (snow_phy_24mhz[i].off == SNOW_REV_REG + 4)
			writel(rev_val, cp->phy + SNOW_REV_REG);
		writel(snow_phy_24mhz[i].val, cp->phy + snow_phy_24mhz[i].off);
	}
	writel(SNOW_REG0_CFG | SNOW_REG0_AHB_RESETN, cp->phy + SNOW_COM_REG0);

	cortina_global_rmw(cp, GLOBAL_PHY_CONTROL, 0, PHY_CTL_CMU_RESETN(p));
	for (i = 0; i < CMU_OK_POLLS; i++) {
		if (readl(cp->app + APP_CMU_OK_DEBUG13) & CMU_OK)
			break;
		udelay(CMU_OK_POLL_US);
	}
	if (i == CMU_OK_POLLS)
		dev_err(dev, "port %d: CMU not ready\n", p);

	writel(SNOW_LANE_REG0_CFG | SNOW_LANE_REG0_MASTER_RESETN,
	       cp->phy + SNOW_COM_LANE_REG0);

	cortina_global_rmw(cp, GLOBAL_PHY_CONTROL, 0, PHY_CTL_LN0_RESETN(p));
	msleep(100);

	cortina_global_rmw(cp, GLOBAL_BLOCK_RESET, 0, BLOCK_RESET_PCIE(p));
	msleep(10);
	cortina_global_rmw(cp, GLOBAL_BLOCK_RESET, BLOCK_RESET_PCIE(p), 0);
	msleep(10);

	return 0;
}

static void cortina_pcie_port_init(struct cortina_pcie *cp)
{
	int p = cp->port;
	int i;

	cortina_global_rmw(cp, GLOBAL_CONFIG, 0, cortina_pcie_clken[p]);

	writel(cp->dbi_phys, cp->app + APP_CFG_START);
	writel(cp->dbi_phys + APP_CFG_WINDOW_LEN - 1, cp->app + APP_CFG_END);
	for (i = 0; i < APP_CFG_POLLS; i++)
		if (readl(cp->app + APP_CFG_END) == cp->dbi_phys + APP_CFG_WINDOW_LEN - 1)
			break;
	if (i == APP_CFG_POLLS)
		dev_warn(cp->pci.dev, "port %d: cfg window readback failed\n", p);

	writel(0, cp->app + APP_INT_ENABLE);
	writel(readl(cp->app + APP_INT_STATUS) & ~INT_LINK_UP, cp->app + APP_INT_STATUS);
	writel(INT_MSI | INT_LINK_DOWN, cp->app + APP_INT_ENABLE);
	readl(cp->app + APP_INT_ENABLE);
}

static bool cortina_pcie_link_up(struct dw_pcie *pci)
{
	struct cortina_pcie *cp = to_cortina_pcie(pci);
	u32 s = readl(cp->app + APP_INT_STATUS);

	return (s & INT_LINK_UP) && !(s & INT_LINK_DOWN);
}

static int cortina_pcie_start_link(struct dw_pcie *pci)
{
	struct cortina_pcie *cp = to_cortina_pcie(pci);
	u32 v;

	v = readl(cp->app + APP_CORE_CONFIG);
	v |= CORE_CFG_LTSSM_EN | CORE_CFG_AXI_UR_MASK;
	writel(v, cp->app + APP_CORE_CONFIG);
	return 0;
}

static void cortina_pcie_irq_handler(struct irq_desc *desc)
{
	struct cortina_pcie *cp = irq_desc_get_handler_data(desc);
	struct irq_chip *chip = irq_desc_get_chip(desc);
	u32 status, clear;
	int i;

	chained_irq_enter(chip, desc);

	status = readl(cp->app + APP_INT_STATUS);
	clear = status & ~INT_LINK_UP;
	if (status & INT_LINK_DOWN)
		clear |= INT_LINK_UP;
	writel(clear, cp->app + APP_INT_STATUS);

	if (status & INT_ERR_MASK)
		dev_err_ratelimited(cp->pci.dev, "port %d: PCIe error, status %#x\n",
				    cp->port, status);

	for (i = 0; i < PCI_NUM_INTX; i++)
		if (status & INT_INTX_ASSERT(i))
			generic_handle_domain_irq(cp->intx_domain, i);
	if ((status & INT_MSI) && cp->pci.pp.irq_domain)
		dw_handle_msi_irq(&cp->pci.pp);

	chained_irq_exit(chip, desc);
}

static void cortina_intx_mask(struct irq_data *d)
{
	struct cortina_pcie *cp = irq_data_get_irq_chip_data(d);
	u32 bit = INT_INTX_ASSERT(d->hwirq);
	unsigned long flags;

	raw_spin_lock_irqsave(&cp->lock, flags);
	writel(readl(cp->app + APP_INT_ENABLE) & ~bit, cp->app + APP_INT_ENABLE);
	raw_spin_unlock_irqrestore(&cp->lock, flags);
}

static void cortina_intx_unmask(struct irq_data *d)
{
	struct cortina_pcie *cp = irq_data_get_irq_chip_data(d);
	u32 bit = INT_INTX_ASSERT(d->hwirq);
	unsigned long flags;

	raw_spin_lock_irqsave(&cp->lock, flags);
	writel(readl(cp->app + APP_INT_ENABLE) | bit, cp->app + APP_INT_ENABLE);
	raw_spin_unlock_irqrestore(&cp->lock, flags);
}

static struct irq_chip cortina_intx_chip = {
	.name		= "cortina-INTx",
	.irq_mask	= cortina_intx_mask,
	.irq_unmask	= cortina_intx_unmask,
};

static int cortina_pcie_intx_map(struct irq_domain *domain, unsigned int irq,
				 irq_hw_number_t hwirq)
{
	irq_set_chip_and_handler(irq, &cortina_intx_chip, handle_simple_irq);
	irq_set_chip_data(irq, domain->host_data);
	return 0;
}

static const struct irq_domain_ops cortina_intx_domain_ops = {
	.map = cortina_pcie_intx_map,
	.xlate = pci_irqd_intx_xlate,
};

static int cortina_pcie_init_irq_domain(struct cortina_pcie *cp)
{
	struct device *dev = cp->pci.dev;
	struct device_node *intc;

	intc = of_get_child_by_name(dev->of_node, "interrupt-controller");
	if (!intc)
		return dev_err_probe(dev, -ENODEV, "no interrupt-controller child\n");

	cp->intx_domain = irq_domain_create_linear(of_fwnode_handle(intc), PCI_NUM_INTX,
						   &cortina_intx_domain_ops, cp);
	of_node_put(intc);
	if (!cp->intx_domain)
		return dev_err_probe(dev, -ENOMEM, "cannot create INTx domain\n");
	return 0;
}

static int cortina_pcie_rd_own_conf(struct pci_bus *bus, unsigned int devfn,
				    int where, int size, u32 *val)
{
	if (where >= RC_CFG_LIMIT) {
		*val = 0;
		return PCIBIOS_SUCCESSFUL;
	}
	return pci_generic_config_read(bus, devfn, where, size, val);
}

static int cortina_pcie_wr_own_conf(struct pci_bus *bus, unsigned int devfn,
				    int where, int size, u32 val)
{
	if (where >= RC_CFG_LIMIT) {
		dev_dbg(bus->dev.parent, "ignoring RC config write at %#x\n", where);
		return PCIBIOS_SUCCESSFUL;
	}
	return pci_generic_config_write(bus, devfn, where, size, val);
}

static struct pci_ops cortina_own_pci_ops = {
	.map_bus	= dw_pcie_own_conf_map_bus,
	.read		= cortina_pcie_rd_own_conf,
	.write		= cortina_pcie_wr_own_conf,
};

static int cortina_pcie_host_init(struct dw_pcie_rp *pp)
{
	struct dw_pcie *pci = to_dw_pcie_from_pp(pp);
	u8 cap = dw_pcie_find_capability(pci, PCI_CAP_ID_MSI);

	pp->bridge->ops = &cortina_own_pci_ops;
	if (cap)
		dw_pcie_writew_dbi(pci, cap + PCI_MSI_FLAGS,
				   dw_pcie_readw_dbi(pci, cap + PCI_MSI_FLAGS) | PCI_MSI_FLAGS_ENABLE);
	return 0;
}

static int cortina_pcie_msi_init(struct dw_pcie_rp *pp)
{
	struct dw_pcie *pci = to_dw_pcie_from_pp(pp);
	struct cortina_pcie *cp = to_cortina_pcie(pci);
	u32 ctrl, num_ctrls;
	int ret;

	ret = dw_pcie_msi_host_init(pp);
	if (ret)
		return ret;
	pp->msi_data = cp->app_phys + APP_MSI_TARGET;
	num_ctrls = pp->num_vectors / MAX_MSI_IRQS_PER_CTRL;
	for (ctrl = 0; ctrl < num_ctrls; ctrl++) {
		dw_pcie_writel_dbi(pci, PCIE_MSI_INTR0_MASK + ctrl * MSI_REG_CTRL_BLOCK_SIZE,
				   pp->irq_mask[ctrl]);
		dw_pcie_writel_dbi(pci, PCIE_MSI_INTR0_ENABLE + ctrl * MSI_REG_CTRL_BLOCK_SIZE,
				   ~0);
	}
	dw_pcie_writel_dbi(pci, PCIE_MSI_ADDR_LO, lower_32_bits(pp->msi_data));
	dw_pcie_writel_dbi(pci, PCIE_MSI_ADDR_HI, upper_32_bits(pp->msi_data));
	return 0;
}

static const struct dw_pcie_host_ops cortina_pcie_host_ops = {
	.init		= cortina_pcie_host_init,
	.msi_init	= cortina_pcie_msi_init,
};


static const struct dw_pcie_ops cortina_dw_pcie_ops = {
	.link_up	= cortina_pcie_link_up,
	.start_link	= cortina_pcie_start_link,
};

static void __iomem *cortina_map_byname(struct platform_device *pdev, const char *name,
					struct resource **out)
{
	struct resource *res = platform_get_resource_byname(pdev, IORESOURCE_MEM, name);

	if (!res)
		return ERR_PTR(-ENODEV);
	if (out)
		*out = res;
	return devm_ioremap(&pdev->dev, res->start, resource_size(res)) ?: ERR_PTR(-ENOMEM);
}

static int cortina_pcie_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct cortina_pcie *cp;
	struct gpio_desc *rst;
	struct dw_pcie *pci;
	struct dw_pcie_rp *pp;
	struct resource *res;
	int irq, ret;

	cp = devm_kzalloc(dev, sizeof(*cp), GFP_KERNEL);
	if (!cp)
		return -ENOMEM;

	pci = &cp->pci;
	pci->dev = dev;
	pci->ops = &cortina_dw_pcie_ops;
	pp = &pci->pp;

	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "dbi");
	if (!res)
		return dev_err_probe(dev, -ENODEV, "no dbi resource\n");
	cp->dbi_phys = res->start;

	cp->app = cortina_map_byname(pdev, "app", &res);
	if (IS_ERR(cp->app))
		return dev_err_probe(dev, PTR_ERR(cp->app), "no app resource\n");
	cp->app_phys = res->start;
	if (res->start < APP_PHYS_BASE ||
	    (res->start - APP_PHYS_BASE) / APP_PORT_STRIDE >= ARRAY_SIZE(cortina_pcie_clken))
		return dev_err_probe(dev, -EINVAL, "bad app address\n");
	cp->port = (res->start - APP_PHYS_BASE) / APP_PORT_STRIDE;
	raw_spin_lock_init(&cp->lock);

	cp->global = cortina_map_byname(pdev, "global", NULL);
	if (IS_ERR(cp->global))
		return dev_err_probe(dev, PTR_ERR(cp->global), "no global resource\n");

	cp->phy = cortina_map_byname(pdev, "phy", NULL);
	if (IS_ERR(cp->phy))
		return dev_err_probe(dev, PTR_ERR(cp->phy), "no phy resource\n");

	irq = platform_get_irq(pdev, 0);
	if (irq < 0)
		return irq;

	ret = cortina_pcie_init_irq_domain(cp);
	if (ret)
		return ret;

	rst = devm_gpiod_get_optional(dev, "reset", GPIOD_OUT_HIGH);
	if (IS_ERR(rst))
		return dev_err_probe(dev, PTR_ERR(rst), "reset gpio\n");
	if (rst)
		udelay(PERST_ASSERT_US);

	ret = cortina_pcie_phy_init(cp);
	if (ret)
		return ret;

	if (rst) {
		gpiod_set_value_cansleep(rst, 0);
		msleep(PERST_SETTLE_MS);
	}
	cortina_pcie_port_init(cp);

	irq_set_chained_handler_and_data(irq, cortina_pcie_irq_handler, cp);

	platform_set_drvdata(pdev, cp);

	pp->ops = &cortina_pcie_host_ops;
	pp->msi_irq[0] = -ENODEV;

	ret = dw_pcie_host_init(pp);
	if (ret)
		return dev_err_probe(dev, ret, "failed to initialize host\n");

	return 0;
}

static const struct of_device_id cortina_pcie_of_match[] = {
	{ .compatible = "cortina,cs75xx-pcie" },
	{},
};

static struct platform_driver cortina_pcie_driver = {
	.driver = {
		.name = "cortina-pcie",
		.of_match_table = cortina_pcie_of_match,
		.suppress_bind_attrs = true,
	},
	.probe = cortina_pcie_probe,
};
builtin_platform_driver(cortina_pcie_driver);

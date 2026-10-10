// SPDX-License-Identifier: GPL-2.0-only
/*
 * ZTE zx279128s MDIO controller
 *
 * Copyright (C) 2026 Navid Ghahremani <ghahramani.navid@gmail.com>
 */

#include <linux/bitfield.h>
#include <linux/bits.h>
#include <linux/clk.h>
#include <linux/err.h>
#include <linux/io.h>
#include <linux/iopoll.h>
#include <linux/mod_devicetable.h>
#include <linux/module.h>
#include <linux/of_mdio.h>
#include <linux/phy.h>
#include <linux/platform_device.h>

#define ZX_MDIO_DATA_WR		0x04
#define ZX_MDIO_DATA_RD		0x08
#define ZX_MDIO_CFG		0x0c
#define ZX_MDIO_STATUS		0x10
#define  ZX_MDIO_STATUS_DONE	BIT(0)
#define ZX_MDIO_CMD		0x14
#define  ZX_MDIO_CMD_REG	GENMASK(4, 0)
#define  ZX_MDIO_CMD_ADDR	GENMASK(9, 5)
#define  ZX_MDIO_CMD_WR		BIT(10)
#define  ZX_MDIO_CMD_RD		BIT(11)
#define  ZX_MDIO_CMD_CLK_DIV	BIT(12)
#define  ZX_MDIO_CMD_BIT13	BIT(13)
#define  ZX_MDIO_CMD_START	BIT(14)
#define  ZX_MDIO_CMD_BIT15	BIT(15)

#define ZX_MDIO_POLL_US		10
#define ZX_MDIO_TIMEOUT_US	2000

struct zx279128s_mdio {
	void __iomem *base;
};

static int zx279128s_mdio_xfer(struct mii_bus *bus, u32 op, int addr,
			       int regnum, u16 data)
{
	struct zx279128s_mdio *priv = bus->priv;
	void __iomem *base = priv->base;
	u32 cmd, val;
	int ret;

	/* Clear the previous command and its status */
	writel(readl(base + ZX_MDIO_CMD) & ~ZX_MDIO_CMD_START,
	       base + ZX_MDIO_CMD);
	writel(0, base + ZX_MDIO_STATUS);

	cmd = readl(base + ZX_MDIO_CMD);
	cmd &= ~(ZX_MDIO_CMD_WR | ZX_MDIO_CMD_RD | ZX_MDIO_CMD_ADDR |
		 ZX_MDIO_CMD_REG);
	cmd |= op | FIELD_PREP(ZX_MDIO_CMD_ADDR, addr) |
	       FIELD_PREP(ZX_MDIO_CMD_REG, regnum);

	if (op == ZX_MDIO_CMD_WR)
		writel(data, base + ZX_MDIO_DATA_WR);
	writel(cmd | ZX_MDIO_CMD_START, base + ZX_MDIO_CMD);

	ret = readl_poll_timeout(base + ZX_MDIO_STATUS, val,
				 val & ZX_MDIO_STATUS_DONE, ZX_MDIO_POLL_US,
				 ZX_MDIO_TIMEOUT_US);
	if (ret) {
		dev_err_ratelimited(&bus->dev, "timeout (addr %d, reg %d)\n",
				    addr, regnum);
		return ret;
	}

	writel(0, base + ZX_MDIO_STATUS);
	writel(readl(base + ZX_MDIO_CMD) & ~ZX_MDIO_CMD_START,
	       base + ZX_MDIO_CMD);

	return 0;
}

static int zx279128s_mdio_read(struct mii_bus *bus, int addr, int regnum)
{
	struct zx279128s_mdio *priv = bus->priv;
	int ret;

	ret = zx279128s_mdio_xfer(bus, ZX_MDIO_CMD_RD, addr, regnum, 0);
	if (ret)
		return ret;

	return readl(priv->base + ZX_MDIO_DATA_RD) & 0xffff;
}

static int zx279128s_mdio_write(struct mii_bus *bus, int addr, int regnum,
				u16 val)
{
	return zx279128s_mdio_xfer(bus, ZX_MDIO_CMD_WR, addr, regnum, val);
}

static int zx279128s_mdio_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct zx279128s_mdio *priv;
	struct mii_bus *bus;
	struct clk *clk;
	u32 val;

	bus = devm_mdiobus_alloc_size(dev, sizeof(*priv));
	if (!bus)
		return -ENOMEM;

	priv = bus->priv;
	priv->base = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(priv->base))
		return PTR_ERR(priv->base);

	clk = devm_clk_get_enabled(dev, "pclk");
	if (IS_ERR(clk))
		return dev_err_probe(dev, PTR_ERR(clk),
				     "failed to get the register clock\n");
	clk = devm_clk_get_enabled(dev, "wclk");
	if (IS_ERR(clk))
		return dev_err_probe(dev, PTR_ERR(clk),
				     "failed to get the work clock\n");

	val = readl(priv->base + ZX_MDIO_CMD);
	val &= ~(ZX_MDIO_CMD_START | ZX_MDIO_CMD_BIT15 | ZX_MDIO_CMD_BIT13);
	val |= ZX_MDIO_CMD_CLK_DIV;
	writel(val, priv->base + ZX_MDIO_CMD);
	writel(0, priv->base + ZX_MDIO_STATUS);
	writel(0, priv->base + ZX_MDIO_CFG);

	bus->name = "zx279128s-mdio";
	bus->read = zx279128s_mdio_read;
	bus->write = zx279128s_mdio_write;
	snprintf(bus->id, MII_BUS_ID_SIZE, "%s", dev_name(dev));
	bus->parent = dev;

	return devm_of_mdiobus_register(dev, bus, dev->of_node);
}

static const struct of_device_id zx279128s_mdio_of_match[] = {
	{ .compatible = "zte,zx279128s-mdio" },
	{ }
};
MODULE_DEVICE_TABLE(of, zx279128s_mdio_of_match);

static struct platform_driver zx279128s_mdio_driver = {
	.probe = zx279128s_mdio_probe,
	.driver = {
		.name = "zx279128s-mdio",
		.of_match_table = zx279128s_mdio_of_match,
	},
};
module_platform_driver(zx279128s_mdio_driver);

MODULE_AUTHOR("Navid Ghahremani <ghahramani.navid@gmail.com>");
MODULE_DESCRIPTION("ZTE zx279128s MDIO controller driver");
MODULE_LICENSE("GPL");

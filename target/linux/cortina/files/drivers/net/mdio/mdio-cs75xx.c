// SPDX-License-Identifier: GPL-2.0
#include <linux/bitfield.h>
#include <linux/bits.h>
#include <linux/io.h>
#include <linux/iopoll.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_mdio.h>
#include <linux/phy.h>
#include <linux/platform_device.h>

#define PER_MDIO_CFG			0x00
#define   MDIO_CFG_MANUAL		BIT(0)
#define   MDIO_CFG_PRE_SCALE		GENMASK(31, 16)
#define PER_MDIO_ADDR			0x04
#define   MDIO_ADDR_PHY			GENMASK(4, 0)
#define   MDIO_ADDR_REG			GENMASK(12, 8)
#define   MDIO_ADDR_READ		BIT(15)
#define PER_MDIO_WRDATA			0x08
#define PER_MDIO_RDDATA			0x0c
#define PER_MDIO_CTRL			0x10
#define   MDIO_CTRL_DONE		BIT(0)
#define   MDIO_CTRL_START		BIT(7)

#define CS75XX_MDIO_PRESCALE		0x20
#define CS75XX_MDIO_TIMEOUT_US		10000

struct cs75xx_mdio {
	void __iomem *base;
	struct mii_bus *bus;
};

static int cs75xx_mdio_wait(struct cs75xx_mdio *priv)
{
	u32 val;
	int ret;

	ret = readl_poll_timeout(priv->base + PER_MDIO_CTRL, val,
				 val & MDIO_CTRL_DONE, 1,
				 CS75XX_MDIO_TIMEOUT_US);
	writel(MDIO_CTRL_DONE, priv->base + PER_MDIO_CTRL);
	return ret;
}

static int cs75xx_mdio_read(struct mii_bus *bus, int phy, int reg)
{
	struct cs75xx_mdio *priv = bus->priv;
	int ret;

	writel(MDIO_CTRL_DONE, priv->base + PER_MDIO_CTRL);
	writel(MDIO_ADDR_READ | FIELD_PREP(MDIO_ADDR_REG, reg) |
	       FIELD_PREP(MDIO_ADDR_PHY, phy), priv->base + PER_MDIO_ADDR);
	writel(MDIO_CTRL_START, priv->base + PER_MDIO_CTRL);
	ret = cs75xx_mdio_wait(priv);
	if (ret)
		return ret;
	return readl(priv->base + PER_MDIO_RDDATA) & 0xffff;
}

static int cs75xx_mdio_write(struct mii_bus *bus, int phy, int reg, u16 val)
{
	struct cs75xx_mdio *priv = bus->priv;

	writel(MDIO_CTRL_DONE, priv->base + PER_MDIO_CTRL);
	writel(FIELD_PREP(MDIO_ADDR_REG, reg) | FIELD_PREP(MDIO_ADDR_PHY, phy),
	       priv->base + PER_MDIO_ADDR);
	writel(val, priv->base + PER_MDIO_WRDATA);
	writel(MDIO_CTRL_START, priv->base + PER_MDIO_CTRL);
	return cs75xx_mdio_wait(priv);
}

static int cs75xx_mdio_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct cs75xx_mdio *priv;
	struct mii_bus *bus;
	u32 cfg;

	bus = devm_mdiobus_alloc_size(dev, sizeof(*priv));
	if (!bus)
		return -ENOMEM;

	priv = bus->priv;
	priv->bus = bus;
	priv->base = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(priv->base))
		return PTR_ERR(priv->base);

	cfg = readl(priv->base + PER_MDIO_CFG);
	cfg &= ~MDIO_CFG_PRE_SCALE;
	cfg |= MDIO_CFG_MANUAL | FIELD_PREP(MDIO_CFG_PRE_SCALE, CS75XX_MDIO_PRESCALE);
	writel(cfg, priv->base + PER_MDIO_CFG);
	writel(MDIO_CTRL_DONE, priv->base + PER_MDIO_CTRL);

	bus->name = "cs75xx-mdio";
	bus->read = cs75xx_mdio_read;
	bus->write = cs75xx_mdio_write;
	bus->parent = dev;
	snprintf(bus->id, MII_BUS_ID_SIZE, "%s", dev_name(dev));

	return devm_of_mdiobus_register(dev, bus, dev->of_node);
}

static const struct of_device_id cs75xx_mdio_of_match[] = {
	{ .compatible = "cortina,cs75xx-mdio" },
	{ }
};
MODULE_DEVICE_TABLE(of, cs75xx_mdio_of_match);

static struct platform_driver cs75xx_mdio_driver = {
	.probe = cs75xx_mdio_probe,
	.driver = {
		.name = "cs75xx-mdio",
		.of_match_table = cs75xx_mdio_of_match,
	},
};
module_platform_driver(cs75xx_mdio_driver);

MODULE_DESCRIPTION("Cortina CS75xx MDIO bus driver");
MODULE_LICENSE("GPL");

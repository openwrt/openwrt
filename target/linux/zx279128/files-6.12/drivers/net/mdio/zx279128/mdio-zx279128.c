// SPDX-License-Identifier: GPL-2.0-only
/*
 * MDIO bus controller for ZTE ZX279128S (ZXHN E1600).
 *
 * Register layout reverse-engineered from the stock 4.1.25 kernel
 * (artifacts/stock-vmlinux.elf: zx_mdio_read @ 0xc00192a0,
 * zx_mdio_write @ 0xc00193f4, controller base 0xf0801000, window 0x18)
 * and cross-verified against the mainline-style ZX279133 driver in
 * Zlion-Y/e2631-mainline-zx279128s (drivers/net/mdio/mdio-zx279133.c,
 * commit f3a814f, GPL-2.0) — the two agree bit-for-bit on the C22 path.
 *
 * Differences from the ZX279133 reference on this SoC:
 *  - No clock framework: the MDIO block clock (stock DT
 *    clocks = <&lsp1crpm 1>) is left in the state the bootloader
 *    configured it, matching the vendor driver, which never touches
 *    clocks, and our SPIFC/UART bring-up convention of relying on
 *    bootloader-initialized fixed clocks.  Verified on hardware:
 *    MDIO works with whatever LSP1+0x14 the bootloader left.
 *  - A reset pulse on the PON CRM SW/GEPHY reset register
 *    (0x92000008) is issued at probe, mirroring U-Boot's
 *    gephy_global_init().  The warm reset (topcrm+0x44) does not
 *    reset the GEPHY block, so without this pulse a PHY MDIO state
 *    machine wedged by a previous kernel stays wedged across reboots.
 *  - The controller window contains no MDC divider register; MDC
 *    timing comes from the (bootloader-enabled) functional clock.
 *  - The bus is registered with plain devm_mdiobus_register() (full
 *    32-address C22 scan, phy_mask = 0) instead of of_mdiobus, which
 *    would mask all addresses and only instantiate DT child nodes.
 *    Verified on hardware 2026-10-01: integrated GEPHYs appear at
 *    addrs 10-13 with PHY ID2 = 0x84b9.
 */

#include <linux/bitfield.h>
#include <linux/delay.h>
#include <linux/iopoll.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/phy.h>
#include <linux/platform_device.h>
#include <linux/spinlock.h>

#define ZX279128_MDIO_WRITE_DATA	0x04
#define ZX279128_MDIO_READ_DATA		0x08
#define ZX279128_MDIO_C45_REG		0x0c
#define ZX279128_MDIO_STATUS		0x10
#define ZX279128_MDIO_CONTROL		0x14

#define ZX279128_MDIO_STATUS_DONE	BIT(0)

#define ZX279128_MDIO_CONTROL_REG	GENMASK(4, 0)
#define ZX279128_MDIO_CONTROL_PHY	GENMASK(9, 5)
#define ZX279128_MDIO_CONTROL_OP	GENMASK(11, 10)
#define ZX279128_MDIO_CONTROL_C22	BIT(12)
#define ZX279128_MDIO_CONTROL_C45	BIT(13)
#define ZX279128_MDIO_CONTROL_START	BIT(14)

#define ZX279128_MDIO_OP_C45_ADDR	0
#define ZX279128_MDIO_OP_WRITE		1
#define ZX279128_MDIO_OP_C22_READ	2
#define ZX279128_MDIO_OP_C45_READ	3

#define ZX279128_MDIO_POLL_US		1
#define ZX279128_MDIO_TIMEOUT_US	10000

/* PON CRM SW/GEPHY reset control register.  U-Boot's gephy_global_init()
 * and the stock 4.1 plat driver both pulse this at boot: clear the port
 * reset bits, delay, then write all-ones to release.  The warm reset
 * (topcrm+0x44) does not reset the GEPHY block, so a PHY MDIO state
 * machine wedged by a previous kernel stays wedged across reboots until
 * this pulse runs.  Verified on hardware 2026-10-01.
 */
#define ZX279128_PON_GEPHY_RST		0x92000008

struct zx279128_mdio {
	void __iomem *base;
	struct device *dev;
	/* MDIO transactions are not reentrant on this hardware. */
	raw_spinlock_t lock;
};

static int zx279128_mdio_transfer(struct zx279128_mdio *priv, u32 control)
{
	unsigned long flags;
	u32 status;
	int ret;

	raw_spin_lock_irqsave(&priv->lock, flags);

	writel(0, priv->base + ZX279128_MDIO_STATUS);
	writel(control | ZX279128_MDIO_CONTROL_START,
	       priv->base + ZX279128_MDIO_CONTROL);

	ret = readl_poll_timeout_atomic(priv->base + ZX279128_MDIO_STATUS,
					status,
					status & ZX279128_MDIO_STATUS_DONE,
					ZX279128_MDIO_POLL_US,
					ZX279128_MDIO_TIMEOUT_US);

	writel(0, priv->base + ZX279128_MDIO_STATUS);
	writel(control, priv->base + ZX279128_MDIO_CONTROL);

	if (ret)
		dev_err(priv->dev,
			"MDIO transfer timeout: control=%#x status=%#x\n",
			control, status);

	raw_spin_unlock_irqrestore(&priv->lock, flags);

	return ret;
}

static int zx279128_mdio_c22_control(int addr, int regnum, u32 op,
				     u32 *control)
{
	if (addr < 0 || addr > 0x1f || regnum < 0 || regnum > 0x1f)
		return -EINVAL;

	*control = ZX279128_MDIO_CONTROL_C22 |
		   FIELD_PREP(ZX279128_MDIO_CONTROL_OP, op) |
		   FIELD_PREP(ZX279128_MDIO_CONTROL_PHY, addr) |
		   FIELD_PREP(ZX279128_MDIO_CONTROL_REG, regnum);

	return 0;
}

static int zx279128_mdio_read(struct mii_bus *bus, int addr, int regnum)
{
	struct zx279128_mdio *priv = bus->priv;
	u32 control;
	int ret;

	ret = zx279128_mdio_c22_control(addr, regnum,
					ZX279128_MDIO_OP_C22_READ, &control);
	if (ret)
		return ret;

	ret = zx279128_mdio_transfer(priv, control);
	if (ret)
		return ret;

	return readl(priv->base + ZX279128_MDIO_READ_DATA) & 0xffff;
}

static int zx279128_mdio_write(struct mii_bus *bus, int addr, int regnum,
			       u16 value)
{
	struct zx279128_mdio *priv = bus->priv;
	u32 control;
	int ret;

	ret = zx279128_mdio_c22_control(addr, regnum, ZX279128_MDIO_OP_WRITE,
					&control);
	if (ret)
		return ret;

	writel(value, priv->base + ZX279128_MDIO_WRITE_DATA);

	return zx279128_mdio_transfer(priv, control);
}

static void zx279128_gephy_reset_pulse(struct device *dev)
{
	void __iomem *pon;

	pon = ioremap(ZX279128_PON_GEPHY_RST, 4);
	if (!pon) {
		dev_warn(dev, "can't ioremap PON GEPHY reset register\n");
		return;
	}

	/* Match U-Boot gephy_global_init(): assert all port resets, hold,
	 * then release all. Delay scaled up from U-Boot's short delay.
	 */
	writel(0, pon);
	mdelay(1);
	writel(0xffffffff, pon);
	mdelay(10);

	iounmap(pon);
}

static int zx279128_mdio_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct zx279128_mdio *priv;
	struct mii_bus *bus;

	bus = devm_mdiobus_alloc_size(dev, sizeof(*priv));
	if (!bus)
		return -ENOMEM;

	priv = bus->priv;
	priv->dev = dev;
	priv->base = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(priv->base))
		return PTR_ERR(priv->base);

	raw_spin_lock_init(&priv->lock);

	/* Revive the integrated GEPHYs the way the stock boot path does:
	 * a warm reset does not reset the GEPHY block, and a PHY MDIO state
	 * machine wedged by a previous kernel only clears on this pulse.
	 */
	zx279128_gephy_reset_pulse(dev);

	bus->name = "ZX279128 MDIO";
	bus->parent = dev;
	bus->read = zx279128_mdio_read;
	bus->write = zx279128_mdio_write;
	snprintf(bus->id, MII_BUS_ID_SIZE, "%s", dev_name(dev));

	/* Publish our DT node on the bus device: of_mdio_find_bus()
	 * matches on bus->dev.of_node, and plain mdiobus_register()
	 * does not set it (only __of_mdiobus_register does). Without
	 * this the Ethernet driver's of_mdio_find_bus(zte,mdio) never
	 * resolves and its probe defers forever (Gate G4 attempt 2,
	 * 2026-10-02: devices_deferred = 92350000.ethernet). */
	bus->dev.of_node = dev->of_node;

	/* Plain mdiobus_register, NOT of_mdiobus_register:
	 * __of_mdiobus_register() sets phy_mask = ~0 and only instantiates
	 * PHYs from DT child nodes; our mdio0 node has no children, so the
	 * C22 scan would never run a single read().  A plain register scans
	 * all 32 addresses and finds the four integrated GEPHYs at 10-13.
	 */
	return devm_mdiobus_register(dev, bus);
}

static const struct of_device_id zx279128_mdio_of_match[] = {
	{ .compatible = "zte,zx279128s-mdio" },
	{ }
};
MODULE_DEVICE_TABLE(of, zx279128_mdio_of_match);

static struct platform_driver zx279128_mdio_driver = {
	.probe = zx279128_mdio_probe,
	.driver = {
		.name = "zx279128-mdio",
		.of_match_table = zx279128_mdio_of_match,
		.suppress_bind_attrs = true,
	},
};
module_platform_driver(zx279128_mdio_driver);

MODULE_DESCRIPTION("ZTE ZX279128S MDIO bus controller");
MODULE_LICENSE("GPL");

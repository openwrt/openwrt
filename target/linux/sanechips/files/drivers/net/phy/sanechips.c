// SPDX-License-Identifier: GPL-2.0-only
/*
 * Sanechips ZX5201 Ethernet PHY
 *
 * The ZX5201 answers on two MDIO addresses: the copper PHY on its own
 * address and its MAC-side SerDes interface on the next one. The copper PHY
 * uses the standard registers and the generic PHY code; this driver only adds
 * the setup that the PHY needs before it passes traffic. The values come from
 * the vendor firmware of the ZTE ZXHN H3600, and most of them are not
 * documented.
 *
 * Copyright (C) 2026 Navid Ghahremani <ghahramani.navid@gmail.com>
 */

#include <linux/mod_devicetable.h>
#include <linux/module.h>
#include <linux/phy.h>

#define PHY_ID_ZX5201		0x84b95011

/* Copper PHY vendor registers */
#define ZX5201_REG18		18
#define ZX5201_EXT_ADDR		16	/* extended register access: address */
#define ZX5201_EXT_DATA		17	/* extended register access: data */
#define ZX5201_REG29		29

/*
 * SerDes interface registers, at the PHY's MDIO address + 1. The names are
 * a best guess from the way the vendor firmware uses them.
 */
#define ZX5201_SDS_WDATA_LO	16
#define ZX5201_SDS_WDATA_HI	17
#define ZX5201_SDS_CMD		18
#define ZX5201_SDS_RDATA_LO	20
#define ZX5201_SDS_RDATA_HI	21
#define ZX5201_SDS_REG22	22
#define ZX5201_SDS_REG27	27

static int zx5201_sds_read(struct phy_device *phydev, u32 regnum)
{
	return mdiobus_read(phydev->mdio.bus, phydev->mdio.addr + 1, regnum);
}

static int zx5201_sds_write(struct phy_device *phydev, u32 regnum, u16 val)
{
	return mdiobus_write(phydev->mdio.bus, phydev->mdio.addr + 1, regnum,
			     val);
}

static int zx5201_ext_write(struct phy_device *phydev, u16 reg, u16 val)
{
	int ret;

	ret = phy_write(phydev, ZX5201_EXT_ADDR, reg);
	if (ret)
		return ret;

	return phy_write(phydev, ZX5201_EXT_DATA, val);
}

static int zx5201_config_init(struct phy_device *phydev)
{
	static const u16 led_regs[] = { 0xb409, 0xb407, 0xb406, 0xb408 };
	int ret, lo, hi, i;

	ret = phy_write(phydev, ZX5201_REG18, 0x8402);
	if (ret)
		return ret;

	ret = zx5201_sds_write(phydev, ZX5201_SDS_REG22, 0x0a0f);
	if (ret)
		return ret;

	ret = zx5201_sds_write(phydev, ZX5201_SDS_REG27, 0x0800);
	if (ret)
		return ret;

	ret = phy_write(phydev, ZX5201_REG29, 0x0355);
	if (ret)
		return ret;

	ret = zx5201_ext_write(phydev, 0xb62d, 0x0006);
	if (ret)
		return ret;

	/*
	 * This looks like an indirect read-modify-write of SerDes register 4:
	 * read it, set bits 9-13 of its high half (bits 25-29 of the register)
	 * to 0b10100, and write it back.
	 */
	ret = zx5201_sds_write(phydev, ZX5201_SDS_CMD, 0x0004);
	if (ret)
		return ret;

	hi = zx5201_sds_read(phydev, ZX5201_SDS_RDATA_HI);
	if (hi < 0)
		return hi;

	lo = zx5201_sds_read(phydev, ZX5201_SDS_RDATA_LO);
	if (lo < 0)
		return lo;

	ret = zx5201_sds_write(phydev, ZX5201_SDS_WDATA_HI,
			       (hi & 0xc1ff) | 0x2800);
	if (ret)
		return ret;

	ret = zx5201_sds_write(phydev, ZX5201_SDS_WDATA_LO, lo);
	if (ret)
		return ret;

	ret = zx5201_sds_write(phydev, ZX5201_SDS_CMD, 0x0204);
	if (ret)
		return ret;

	ret = zx5201_sds_read(phydev, ZX5201_SDS_REG22);
	if (ret < 0)
		return ret;

	ret = zx5201_sds_write(phydev, ZX5201_SDS_REG22, (ret & 0xfff3) | 0x0004);
	if (ret)
		return ret;

	/* LED control registers, all cleared as the vendor firmware does */
	for (i = 0; i < ARRAY_SIZE(led_regs); i++) {
		ret = zx5201_ext_write(phydev, led_regs[i], 0);
		if (ret)
			return ret;
	}

	return 0;
}

static struct phy_driver sanechips_phy_driver[] = {
	{
		PHY_ID_MATCH_EXACT(PHY_ID_ZX5201),
		.name		= "Sanechips ZX5201",
		.config_init	= zx5201_config_init,
		.suspend	= genphy_suspend,
		.resume		= genphy_resume,
	},
};
module_phy_driver(sanechips_phy_driver);

static const struct mdio_device_id __maybe_unused sanechips_phy_tbl[] = {
	{ PHY_ID_MATCH_EXACT(PHY_ID_ZX5201) },
	{ }
};
MODULE_DEVICE_TABLE(mdio, sanechips_phy_tbl);

MODULE_AUTHOR("Navid Ghahremani <ghahramani.navid@gmail.com>");
MODULE_DESCRIPTION("Sanechips Ethernet PHY driver");
MODULE_LICENSE("GPL");

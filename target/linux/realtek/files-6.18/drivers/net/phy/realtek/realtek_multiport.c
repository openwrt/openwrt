// SPDX-License-Identifier: GPL-2.0-only
/* Realtek RTL838X Ethernet MDIO interface driver
 *
 * Copyright (C) 2020 B. Koblitz
 */

#include <linux/module.h>
#include <linux/bitfield.h>
#include <linux/delay.h>
#include <linux/of.h>
#include <linux/phy.h>
#include <linux/netdevice.h>
#include <linux/firmware.h>
#include <linux/crc32.h>
#include <linux/sfp.h>
#include <linux/mii.h>
#include <linux/mdio.h>
#include <linux/slab.h>
#include <linux/workqueue.h>
#include "../phylib.h"

/*
 * Realtek PHYs have three special page registers. Register 31 (page select) switches the
 * register pages and gives access to special registers that are mapped into register
 * range 16-24. Register 30 (extended page select) does basically the same. It especially
 * grants access to special internal data like fibre, copper or serdes setup. Register
 * 29 is a write only companion of register 30. As it flips back to 0 and register 30
 * shows the original write content it should be avoided at all cost.
 */
#define RTL821x_PAGE_SELECT		0x1f
#define RTL821x_EXT_PAGE_SELECT		0x1e

#define RTL8XXX_PAGE_MAIN		0x0000
#define RTL821X_PAGE_PORT		0x0266
#define RTL821X_PAGE_POWER		0x0a40
#define RTL821X_PAGE_GPHY		0x0a42
#define RTL821X_PAGE_MAC		0x0a43
#define RTL821X_PAGE_STATE		0x0b80
#define RTL821X_PAGE_PATCH		0x0b82
#define RTL821X_MAC_SDS_PAGE(sds, page)	(0x404 + (sds) * 0x20 + (page))

#define RTL821X_PHYCR2			0x19
#define RTL821X_PHYCR2_PHY_EEE_ENABLE	BIT(5)

#define RTL821X_MEDIA_PAGE_AUTO		0
#define RTL821X_MEDIA_PAGE_COPPER	1
#define RTL821X_MEDIA_PAGE_FIBRE	3
#define RTL821X_MEDIA_PAGE_INTERNAL	8

#define RTL8214FC_MEDIA_COPPER		BIT(11)

#define RTL8214FC_FIBRE_RX_PAGE		0x0c
#define RTL8214FC_FIBRE_RX_CTRL		0x16
#define RTL8214FC_FIBRE_RX_RESET	BIT(15)

#define PHY_ID_RTL8214C			0x001cc942
#define PHY_ID_RTL8218B_E		0x001cc980
#define PHY_ID_RTL8214_OR_8218		0x001cc981
#define PHY_ID_RTL8218D			0x001cc983
#define PHY_ID_RTL8218E			0x001cc984
#define PHY_ID_RTL8218B_I		0x001cca40
#define PHY_ID_RTL8214QF		0x001ccab0

/* These PHYs share the same id (0x001cc981) */
#define PHY_IS_NOT_RTL821X		0
#define PHY_IS_RTL8214FC		1
#define PHY_IS_RTL8214FB		2
#define PHY_IS_RTL8218B_E		3

struct rtl821x_shared_priv {
	int base_addr;
	bool setup_done;	/* the chip has been set up (RTL8214QF) */
};

struct rtl8214fc_priv {
	struct phy_device *phydev;
	struct delayed_work fibre_rx_work;
	atomic_t link_down_latched;
};

static int rtl821x_package_join(struct phy_device *phydev, int ports)
{
	struct rtl821x_shared_priv *shared_priv;
	int base_addr = phydev->mdio.addr & ~(ports - 1);
	int ret;

	ret = devm_phy_package_join(&phydev->mdio.dev, phydev, base_addr,
				    sizeof(struct rtl821x_shared_priv));
	if (ret)
		return ret;

	shared_priv = phy_package_get_priv(phydev);
	shared_priv->base_addr = base_addr;

	return 0;
}

static inline struct phy_device *get_package_phy(struct phy_device *phydev, int port)
{
	struct rtl821x_shared_priv *shared_priv = phy_package_get_priv(phydev);

	return mdiobus_get_phy(phydev->mdio.bus, shared_priv->base_addr + port);
}

static inline struct phy_device *get_base_phy(struct phy_device *phydev)
{
	return get_package_phy(phydev, 0);
}

/* The PHY at the base address of its package, as joined, sets up the whole package */
static inline bool rtl821x_is_package_base(struct phy_device *phydev)
{
	struct rtl821x_shared_priv *shared_priv = phy_package_get_priv(phydev);

	return phydev->mdio.addr == shared_priv->base_addr;
}

static int rtl821x_match_phy_device(struct phy_device *phydev)
{
	int oldpage, oldxpage, chip_mode, chip_cfg_mode;
	struct mii_bus *bus = phydev->mdio.bus;
	int addr = phydev->mdio.addr & ~3;

	if (phydev->phy_id == PHY_ID_RTL8218B_E)
		return PHY_IS_RTL8218B_E;

	if (phydev->phy_id != PHY_ID_RTL8214_OR_8218)
		return PHY_IS_NOT_RTL821X;

	/*
	 * RTL8214FC and RTL8218B are the same PHYs with different configurations. That info is
	 * stored in the first PHY of the package. In all known configurations packages start at
	 * bus addresses that are multiples of four. Avoid paged access as this is not available
	 * during detection.
	 */

	oldpage = mdiobus_read(bus, addr, RTL821x_PAGE_SELECT);
	oldxpage = mdiobus_read(bus, addr, RTL821x_EXT_PAGE_SELECT);

	mdiobus_write(bus, addr, RTL821x_EXT_PAGE_SELECT, 0x8);
	mdiobus_write(bus, addr, RTL821x_PAGE_SELECT, 0x278);
	mdiobus_write(bus, addr, 0x12, 0x455);
	mdiobus_write(bus, addr, RTL821x_PAGE_SELECT, 0x260);
	chip_mode = mdiobus_read(bus, addr, 0x12);
	dev_dbg(&phydev->mdio.dev, "got RTL8218B/RTL8214Fx chip mode %04x\n", chip_mode);

	mdiobus_write(bus, addr, RTL821x_EXT_PAGE_SELECT, oldxpage);
	mdiobus_write(bus, addr, RTL821x_PAGE_SELECT, oldpage);

	/* no values while reading the 5th port during 5-8th port detection of RTL8218B */
	if (!chip_mode)
		return PHY_IS_RTL8218B_E;

	chip_cfg_mode = (chip_mode >> 4) & 0xf;
	chip_mode &= 0xf;

	if (chip_mode == 0xd || chip_mode == 0xf)
		return PHY_IS_RTL8218B_E;

	if (chip_mode == 0x4 || chip_mode == 0x6)
		return PHY_IS_RTL8214FC;

	if (chip_mode != 0xc && chip_mode != 0xe)
		return PHY_IS_NOT_RTL821X;

	if (chip_cfg_mode == 0x4 || chip_cfg_mode == 0x6)
		return PHY_IS_RTL8214FC;

	return PHY_IS_RTL8214FB;
}

static int rtl8218b_ext_match_phy_device(struct phy_device *phydev,
					 const struct phy_driver *phydrv)
{
	return rtl821x_match_phy_device(phydev) == PHY_IS_RTL8218B_E;
}

static int rtl8214fc_match_phy_device(struct phy_device *phydev,
				      const struct phy_driver *phydrv)
{
	return rtl821x_match_phy_device(phydev) == PHY_IS_RTL8214FC;
}

static int rtl821x_read_page(struct phy_device *phydev)
{
	return __phy_read(phydev, RTL821x_PAGE_SELECT);
}

static int rtl821x_write_page(struct phy_device *phydev, int page)
{
	return __phy_write(phydev, RTL821x_PAGE_SELECT, page);
}

/* Paged access with the MDIO bus already locked, e.g. by phy_save_page() */
static int __rtl821x_read_paged(struct phy_device *phydev, int page, u32 regnum)
{
	int ret;

	ret = rtl821x_write_page(phydev, page);
	if (ret < 0)
		return ret;

	return __phy_read(phydev, regnum);
}

static int __rtl821x_write_paged(struct phy_device *phydev, int page, u32 regnum, u16 val)
{
	int ret;

	ret = rtl821x_write_page(phydev, page);
	if (ret < 0)
		return ret;

	return __phy_write(phydev, regnum, val);
}

static int __rtl821x_modify_paged(struct phy_device *phydev, int page, u32 regnum,
				  u16 mask, u16 set)
{
	int ret;

	ret = rtl821x_write_page(phydev, page);
	if (ret < 0)
		return ret;

	return __phy_modify(phydev, regnum, mask, set);
}

/* Like struct reg_sequence of regmap */
struct rtl821x_reg_sequence {
	u16 page;
	u16 reg;
	u16 val;
};

/* Like regmap_multi_reg_write(), with the MDIO bus already locked */
static int __rtl821x_multi_reg_write(struct phy_device *phydev,
				     const struct rtl821x_reg_sequence *regs, int num_regs)
{
	int ret;

	for (int i = 0; i < num_regs; i++) {
		ret = __rtl821x_write_paged(phydev, regs[i].page, regs[i].reg, regs[i].val);
		if (ret < 0)
			return ret;
	}

	return 0;
}

static bool __rtl8214fc_media_is_fibre(struct phy_device *phydev)
{
	struct phy_device *basephy = get_base_phy(phydev);
	static int regs[] = {16, 19, 20, 21};
	int reg = regs[phydev->mdio.addr & 3];
	int oldpage, oldxpage, val;

	oldpage = __phy_read(basephy, RTL821x_PAGE_SELECT);
	oldxpage = __phy_read(basephy, RTL821x_EXT_PAGE_SELECT);

	__phy_write(basephy, RTL821x_EXT_PAGE_SELECT, RTL821X_MEDIA_PAGE_INTERNAL);
	__phy_write(basephy, RTL821x_PAGE_SELECT, RTL821X_PAGE_PORT);
	val = __phy_read(basephy, reg);

	__phy_write(basephy, RTL821x_EXT_PAGE_SELECT, oldxpage);
	__phy_write(basephy, RTL821x_PAGE_SELECT, oldpage);

	return !(val & RTL8214FC_MEDIA_COPPER);
}

static bool rtl8214fc_media_is_fibre(struct phy_device *phydev)
{
	int ret;

	phy_lock_mdio_bus(phydev);
	ret = __rtl8214fc_media_is_fibre(phydev);
	phy_unlock_mdio_bus(phydev);

	return ret;
}

static void rtl8214fc_power_set(struct phy_device *phydev, int port, bool on)
{
	int page = port == PORT_FIBRE ? RTL821X_MEDIA_PAGE_FIBRE : RTL821X_MEDIA_PAGE_COPPER;
	int pdown = on ? 0 : BMCR_PDOWN;
	int oldpage, oldxpage;

	phydev_info(phydev, "power %s %s\n", on ? "on" : "off",
		    port == PORT_FIBRE ? "fibre" : "copper");

	/* Hold the bus lock for the whole sequence. On the base port, other ports
	 * change the page registers in between (__rtl8214fc_media_is_fibre()), and
	 * a stale oldxpage would leave the port stuck on the internal page.
	 */
	phy_lock_mdio_bus(phydev);
	oldxpage = __phy_read(phydev, RTL821x_EXT_PAGE_SELECT);
	oldpage = __phy_read(phydev, RTL821x_PAGE_SELECT);
	__phy_write(phydev, RTL821x_EXT_PAGE_SELECT, page);
	__phy_write(phydev, RTL821x_PAGE_SELECT, RTL821X_PAGE_POWER);
	__phy_modify(phydev, 0x10, BMCR_PDOWN, pdown);
	__phy_write(phydev, RTL821x_PAGE_SELECT, oldpage);
	__phy_write(phydev, RTL821x_EXT_PAGE_SELECT, oldxpage);
	phy_unlock_mdio_bus(phydev);
}

static int rtl8214fc_suspend(struct phy_device *phydev)
{
	rtl8214fc_power_set(phydev, PORT_MII, false);
	rtl8214fc_power_set(phydev, PORT_FIBRE, false);

	return 0;
}

static int rtl8214fc_resume(struct phy_device *phydev)
{
	bool set_fibre = rtl8214fc_media_is_fibre(phydev);

	rtl8214fc_power_set(phydev, PORT_MII, !set_fibre);
	rtl8214fc_power_set(phydev, PORT_FIBRE, set_fibre);

	return 0;
}

static void rtl8214fc_media_set(struct phy_device *phydev, bool set_fibre)
{
	struct phy_device *basephy = get_base_phy(phydev);
	int copper = set_fibre ? 0 : RTL8214FC_MEDIA_COPPER;
	static int regs[] = {16, 19, 20, 21};
	int reg = regs[phydev->mdio.addr & 3];
	int oldpage, oldxpage;

	phydev_info(phydev, "switch to %s\n", set_fibre ? "fibre" : "copper");

	/* Hold the bus lock for the same reason as in rtl8214fc_power_set() */
	phy_lock_mdio_bus(phydev);
	oldxpage = __phy_read(basephy, RTL821x_EXT_PAGE_SELECT);
	oldpage = __phy_read(basephy, RTL821x_PAGE_SELECT);
	__phy_write(basephy, RTL821x_EXT_PAGE_SELECT, RTL821X_MEDIA_PAGE_INTERNAL);
	__phy_write(basephy, RTL821x_PAGE_SELECT, RTL821X_PAGE_PORT);
	__phy_modify(basephy, reg, RTL8214FC_MEDIA_COPPER, copper);
	__phy_write(basephy, RTL821x_PAGE_SELECT, oldpage);
	__phy_write(basephy, RTL821x_EXT_PAGE_SELECT, oldxpage);
	phy_unlock_mdio_bus(phydev);

	if (!phydev->suspended) {
		rtl8214fc_power_set(phydev, PORT_MII, !set_fibre);
		rtl8214fc_power_set(phydev, PORT_FIBRE, set_fibre);
	}
}

static int rtl8214fc_set_tunable(struct phy_device *phydev,
				 struct ethtool_tunable *tuna, const void *data)
{
	/*
	 * The RTL8214FC driver usually detects insertion of SFP modules and automatically toggles
	 * between copper and fiber. There may be cases where the user wants to switch the port on
	 * demand. Usually ethtool offers to change the port of a multiport network card with
	 * "ethtool -s lan25 port fibre/tp" if the driver supports it. This does not work for
	 * attached phys. For more details see phy_ethtool_ksettings_set(). To avoid patching the
	 * kernel misuse the phy downshift tunable to offer that feature. For this use
	 * "ethtool --set-phy-tunable lan25 downshift on/off".
	 */
	switch (tuna->id) {
	case ETHTOOL_PHY_DOWNSHIFT:
		rtl8214fc_media_set(phydev, !rtl8214fc_media_is_fibre(phydev));
		return 0;
	default:
		return -EOPNOTSUPP;
	}
}

static int rtl8214fc_get_tunable(struct phy_device *phydev,
				 struct ethtool_tunable *tuna, void *data)
{
	/* Needed to make rtl8214fc_set_tunable() work */
	return 0;
}

static int rtl8214fc_get_features(struct phy_device *phydev)
{
	int ret = 0;

	ret = genphy_read_abilities(phydev);
	if (ret)
		return ret;
	/*
	 * The RTL8214FC only advertises TP capabilities in the standard registers. This is
	 * independent from what fibre/copper combination is currently activated. For now just
	 * announce the superset of all possible features.
	 */
	linkmode_set_bit(ETHTOOL_LINK_MODE_1000baseX_Full_BIT, phydev->supported);
	linkmode_set_bit(ETHTOOL_LINK_MODE_100baseFX_Full_BIT, phydev->supported);
	linkmode_set_bit(ETHTOOL_LINK_MODE_FIBRE_BIT, phydev->supported);

	return 0;
}

static int rtl8214fc_read_status(struct phy_device *phydev)
{
	struct rtl8214fc_priv *priv = phydev->priv;
	bool changed;
	int ret;

	if (rtl8214fc_media_is_fibre(phydev)) {
		phydev->port = PORT_FIBRE;
		ret = genphy_c37_read_status(phydev, &changed);
	} else {
		phydev->port = PORT_MII; /* for now aligend with rest of code */
		ret = genphy_read_status(phydev);
	}

	/* Report a latched drop consumed by the recovery worker. */
	if (!ret && atomic_xchg(&priv->link_down_latched, 0))
		phydev->link = 0;

	return ret;
}

/*
 * RTL8214QF: quad 1000BASE-X fibre PHY with clause 37 registers, documented with a QSGMII
 * interface. Two of them can also share one XSGMII SerDes, as on the Acorid GLS7700-24F8G-4X: the
 * master carries all eight ports, the slave reaches it through the master (slave SerDes S0 to
 * master S1). Only that setup is tested, and the link between the two chips only trains when
 * they leave reset. Pages: 0x1xx SerDes S0, 0x3xx S1, 0x400-0x7ff the SerDes of ports 0-3.
 * Undocumented registers follow the vendor SDK.
 */
#define RTL8214QF_CHIP_MODE		0x11	/* page 0 */
#define   RTL8214QF_CHIP_MODE_MASK	GENMASK(3, 0)
#define   RTL8214QF_CHIP_MODE_SLAVE	0xb

#define RTL8214QF_PAGE_FIBRE_CTRL	0x5
#define RTL8214QF_FIBRE_CTRL(port)	(0x14 + (port))
#define   RTL8214QF_FIBRE_CTRL_HOLD	BIT(14)

#define RTL8214QF_PAGE_S0_CTRL		0x180
#define RTL8214QF_S0_RESET		0x10
#define   RTL8214QF_S0_RESET_STEP1	0x0400
#define   RTL8214QF_S0_RESET_STEP2	0x0c00

#define RTL8214QF_PORT_PAGE(port, page)	(0x400 + (port) * 0x100 + (page))
#define RTL8214QF_PORT_RX_ERR_PAGE	0x04
#define RTL8214QF_PORT_RX_ERR		0x10
#define RTL8214QF_PORT_SDS_PAGE		0x80
#define RTL8214QF_PORT_SDS_CTRL		0x12
#define   RTL8214QF_PORT_SDS_RESET	BIT(10)
#define RTL8214QF_PORT_RX_PAGE		0x91
#define RTL8214QF_PORT_RX_CTRL		0x10
#define   RTL8214QF_PORT_RX_RESET	BIT(9)

/*
 * Net effect of the vendor SDK patch sequence, measured by register dumps after a cold power-on
 * and after the vendor firmware setup: the changes common to all chips, and those of the master
 * only. Registers that ignore writes are left out. Without it autonegotiation never completes.
 */
static const struct rtl821x_reg_sequence rtl8214qf_init_common[] = {
	{ 0x000, 23, 0x2000 }, { 0x001, 17, 0x0001 }, { 0x001, 22, 0x5800 },
	{ 0x103, 20, 0xc000 }, { 0x17c, 18, 0x0016 }, { 0x184, 16, 0xe150 },
	{ 0x184, 18, 0x1ca4 }, { 0x184, 19, 0x4612 }, { 0x184, 21, 0x420d },
	{ 0x189, 20, 0x2527 }, { 0x18b, 16, 0x3e48 }, { 0x18b, 22, 0x7ffa },
	{ 0x18d, 17, 0xffd0 }, { 0x18d, 19, 0x7c17 }, { 0x18d, 20, 0x002d },
	{ 0x18e, 20, 0x8389 }, { 0x18e, 21, 0x3f89 }, { 0x196, 20, 0x8294 },
	{ 0x196, 21, 0x3e94 }, { 0x1a8, 19, 0xf000 }, { 0x1a9, 23, 0xbfdf },
	{ 0x1aa, 18, 0x2100 }, { 0x1aa, 20, 0x1378 }, { 0x1ab, 22, 0x07fa },
	{ 0x1ac, 18, 0x114f }, { 0x1ac, 21, 0x787c }, { 0x1ae, 20, 0x8294 },
	{ 0x1ae, 21, 0x3e94 }, { 0x1af, 16, 0x8107 }, { 0x1af, 17, 0x8107 },
	{ 0x1af, 21, 0x0019 }, { 0x1af, 22, 0xf0e0 }, { 0x1b9, 20, 0x2527 },
	{ 0x1bb, 16, 0x3e48 }, { 0x1bb, 22, 0x7ffa }, { 0x1bd, 17, 0xffd0 },
	{ 0x1bd, 19, 0x7c17 }, { 0x1bd, 20, 0x002d }, { 0x1be, 20, 0x8389 },
	{ 0x1be, 21, 0x3f89 }, { 0x384, 21, 0x420d }, { 0x38e, 20, 0x8389 },
	{ 0x38e, 21, 0x3f89 }, { 0x396, 20, 0x8294 }, { 0x396, 21, 0x3e94 },
	{ 0x3a8, 19, 0xf000 }, { 0x3a9, 23, 0xbfdf }, { 0x3aa, 18, 0x2100 },
	{ 0x3aa, 20, 0x1378 }, { 0x3ab, 22, 0x07fa }, { 0x3ac, 18, 0x114f },
	{ 0x3ac, 21, 0x787c }, { 0x3ae, 20, 0x8294 }, { 0x3ae, 21, 0x3e94 },
	{ 0x3af, 16, 0x8107 }, { 0x3af, 17, 0x8107 }, { 0x3af, 21, 0x0019 },
	{ 0x3af, 22, 0xf0e0 }, { 0x3be, 20, 0x8389 }, { 0x3be, 21, 0x3f89 },
	{ 0x400, 16, 0x1503 }, { 0x400, 20, 0x074d }, { 0x488, 19, 0x5cd8 },
	{ 0x489, 20, 0xe002 }, { 0x490, 19, 0x5cd8 }, { 0x491, 20, 0xe002 },
	{ 0x500, 16, 0x1503 }, { 0x500, 20, 0x074d }, { 0x588, 19, 0x5cd8 },
	{ 0x589, 20, 0xe002 }, { 0x590, 19, 0x5cd8 }, { 0x591, 20, 0xe002 },
	{ 0x600, 16, 0x1503 }, { 0x600, 20, 0x074d }, { 0x688, 19, 0x5cd8 },
	{ 0x689, 20, 0xe002 }, { 0x690, 19, 0x5cd8 }, { 0x691, 20, 0xe002 },
	{ 0x700, 16, 0x1503 }, { 0x700, 20, 0x074d }, { 0x788, 19, 0x5cd8 },
	{ 0x789, 20, 0xe002 }, { 0x790, 19, 0x5cd8 }, { 0x791, 20, 0xe002 },
};

static const struct rtl821x_reg_sequence rtl8214qf_init_master[] = {
	{ 0x389, 20, 0x2527 }, { 0x38b, 16, 0x3e48 }, { 0x38b, 22, 0x7ffa },
	{ 0x38d, 17, 0xffd0 }, { 0x38d, 19, 0x7c17 }, { 0x38d, 20, 0x002d },
	{ 0x3b9, 20, 0x2527 }, { 0x3bb, 16, 0x3e48 }, { 0x3bb, 22, 0x7ffa },
	{ 0x3bd, 17, 0xffd0 }, { 0x3bd, 19, 0x7c17 }, { 0x3bd, 20, 0x002d },
};

/* Vendor SDK phy_8214qf_fiberRx_reset(); releases the reset and hold also on errors */
static int __rtl8214qf_fibre_rx_reset(struct phy_device *base, int port)
{
	int rx_page = RTL8214QF_PORT_PAGE(port, RTL8214QF_PORT_RX_PAGE);
	int ctrl, ret, err;

	ctrl = __rtl821x_read_paged(base, RTL8214QF_PAGE_FIBRE_CTRL, RTL8214QF_FIBRE_CTRL(port));
	if (ctrl < 0)
		return ctrl;

	/* The hold is only ever set here */
	ctrl &= ~RTL8214QF_FIBRE_CTRL_HOLD;

	ret = __rtl821x_write_paged(base, RTL8214QF_PAGE_FIBRE_CTRL, RTL8214QF_FIBRE_CTRL(port),
				    ctrl | RTL8214QF_FIBRE_CTRL_HOLD);
	if (!ret)
		ret = __rtl821x_modify_paged(base, rx_page, RTL8214QF_PORT_RX_CTRL, 0,
					     RTL8214QF_PORT_RX_RESET);

	/* A failed write may still have taken effect */
	err = __rtl821x_modify_paged(base, rx_page, RTL8214QF_PORT_RX_CTRL,
				     RTL8214QF_PORT_RX_RESET, 0);
	if (!ret)
		ret = err;

	err = __rtl821x_write_paged(base, RTL8214QF_PAGE_FIBRE_CTRL, RTL8214QF_FIBRE_CTRL(port),
				    ctrl);
	if (!ret)
		ret = err;
	if (ret < 0)
		return ret;

	return __rtl821x_write_paged(base, RTL8214QF_PORT_PAGE(port, RTL8214QF_PORT_RX_ERR_PAGE),
				     RTL8214QF_PORT_RX_ERR, 0);
}

/* Vendor SDK reset of SerDes S0; released also on errors */
static int __rtl8214qf_s0_reset(struct phy_device *base)
{
	int ret, err;

	ret = __rtl821x_write_paged(base, RTL8214QF_PAGE_S0_CTRL, RTL8214QF_S0_RESET,
				    RTL8214QF_S0_RESET_STEP1);
	if (!ret)
		ret = __rtl821x_write_paged(base, RTL8214QF_PAGE_S0_CTRL, RTL8214QF_S0_RESET,
					    RTL8214QF_S0_RESET_STEP2);

	err = __rtl821x_write_paged(base, RTL8214QF_PAGE_S0_CTRL, RTL8214QF_S0_RESET, 0);
	if (!ret)
		ret = err;

	return ret;
}

/* Vendor SDK _phy_8214qf_serdes_reset(); released also on errors */
static int __rtl8214qf_port_sds_reset(struct phy_device *base, int port)
{
	int page = RTL8214QF_PORT_PAGE(port, RTL8214QF_PORT_SDS_PAGE);
	int ret, err;

	ret = __rtl821x_modify_paged(base, page, RTL8214QF_PORT_SDS_CTRL, 0,
				     RTL8214QF_PORT_SDS_RESET);
	if (!ret)
		usleep_range(1000, 2000);

	err = __rtl821x_modify_paged(base, page, RTL8214QF_PORT_SDS_CTRL,
				     RTL8214QF_PORT_SDS_RESET, 0);
	if (!ret)
		ret = err;
	if (ret < 0)
		return ret;

	return __rtl8214qf_fibre_rx_reset(base, port);
}

static int __rtl8214qf_chip_init(struct phy_device *base)
{
	int mode, ret;

	mode = __rtl821x_read_paged(base, 0, RTL8214QF_CHIP_MODE);
	if (mode < 0)
		return mode;

	ret = __rtl821x_multi_reg_write(base, rtl8214qf_init_common,
					ARRAY_SIZE(rtl8214qf_init_common));
	if (ret < 0)
		return ret;

	if (FIELD_GET(RTL8214QF_CHIP_MODE_MASK, mode) != RTL8214QF_CHIP_MODE_SLAVE) {
		ret = __rtl821x_multi_reg_write(base, rtl8214qf_init_master,
						ARRAY_SIZE(rtl8214qf_init_master));
		if (ret < 0)
			return ret;
	}

	ret = __rtl8214qf_s0_reset(base);
	if (ret < 0)
		return ret;

	for (int port = 0; port < 4; port++) {
		ret = __rtl8214qf_port_sds_reset(base, port);
		if (ret < 0)
			return ret;
	}

	return 0;
}

static int rtl8214qf_probe(struct phy_device *phydev)
{
	int ret;

	ret = rtl821x_package_join(phydev, 4);
	if (ret)
		return ret;

	phydev->port = PORT_FIBRE;

	return 0;
}

/*
 * The chip is set up once, through its first port, which must be described in DT. The bus stays
 * locked for the whole setup and only success is recorded, so the next port retries after an
 * error. The chip must not be reset later on; the MDIO controller only resets it before probing.
 */
static int rtl8214qf_config_init(struct phy_device *phydev)
{
	struct rtl821x_shared_priv *shared_priv = phy_package_get_priv(phydev);
	struct phy_device *base = get_base_phy(phydev);
	int oldpage, ret = 0;

	if (!base) {
		phydev_err(phydev, "first port of the RTL8214QF is not described\n");
		return -ENODEV;
	}

	oldpage = phy_save_page(base);
	if (oldpage >= 0 && !shared_priv->setup_done) {
		ret = __rtl8214qf_chip_init(base);
		if (ret)
			phydev_err(phydev, "RTL8214QF setup failed: %d\n", ret);
		shared_priv->setup_done = !ret;
	}

	return phy_restore_page(base, oldpage, ret);
}

static int rtl8214qf_get_features(struct phy_device *phydev)
{
	linkmode_set_bit(ETHTOOL_LINK_MODE_1000baseX_Full_BIT, phydev->supported);
	linkmode_set_bit(ETHTOOL_LINK_MODE_Autoneg_BIT, phydev->supported);
	linkmode_set_bit(ETHTOOL_LINK_MODE_FIBRE_BIT, phydev->supported);
	linkmode_set_bit(ETHTOOL_LINK_MODE_Pause_BIT, phydev->supported);
	linkmode_set_bit(ETHTOOL_LINK_MODE_Asym_Pause_BIT, phydev->supported);

	return 0;
}

static int rtl8214qf_read_status(struct phy_device *phydev)
{
	bool changed;

	return genphy_c37_read_status(phydev, &changed);
}

static int rtl8214fc_config_aneg(struct phy_device *phydev)
{
	int ret;

	if (rtl8214fc_media_is_fibre(phydev)) {
		if (linkmode_test_bit(ETHTOOL_LINK_MODE_100baseT_Full_BIT, phydev->advertising)) {
			linkmode_clear_bit(ETHTOOL_LINK_MODE_100baseT_Full_BIT, phydev->advertising);
			linkmode_set_bit(ETHTOOL_LINK_MODE_100baseFX_Full_BIT, phydev->advertising);
		}
		ret = genphy_c37_config_aneg(phydev);
	} else {
		ret = genphy_config_aneg(phydev);
	}

	return ret;
}

static int rtl821x_read_mmd(struct phy_device *phydev, int devnum, u16 regnum)
{
	struct mii_bus *bus = phydev->mdio.bus;
	int addr = phydev->mdio.addr;
	int ret;

	/*
	 * The RTL821x PHYs are usually only C22 capable and are defined accordingly in DTS.
	 * Nevertheless GPL source drops clearly indicate that EEE features can be accessed
	 * directly via C45. Testing shows that C45 over C22 (as used in kernel EEE framework)
	 * works as well but only as long as PHY polling is disabled in the SOC. To avoid ugly
	 * hacks pass through C45 accesses for important EEE registers. Maybe some day the mdio
	 * bus can intercept these patterns and switch off/on polling on demand. That way this
	 * phy device driver can avoid handling special cases on its own.
	 */

	if ((devnum == MDIO_MMD_PCS && regnum == MDIO_PCS_EEE_ABLE) ||
	    (devnum == MDIO_MMD_AN && regnum == MDIO_AN_EEE_ADV) ||
	    (devnum == MDIO_MMD_AN && regnum == MDIO_AN_EEE_LPABLE))
		ret = __mdiobus_c45_read(bus, addr, devnum, regnum);
	else
		ret = -EOPNOTSUPP;

	return ret;
}

static int rtl821x_write_mmd(struct phy_device *phydev, int devnum, u16 regnum, u16 val)
{
	struct mii_bus *bus = phydev->mdio.bus;
	int addr = phydev->mdio.addr;
	int ret;

	/* see rtl821x_read_mmd() */
	if (devnum == MDIO_MMD_AN && regnum == MDIO_AN_EEE_ADV)
		ret = __mdiobus_c45_write(bus, addr, devnum, regnum, val);
	else
		ret = -EOPNOTSUPP;

	return ret;
}

static int rtl8214fc_read_mmd(struct phy_device *phydev, int devnum, u16 regnum)
{
	if (__rtl8214fc_media_is_fibre(phydev))
		return -EOPNOTSUPP;

	return rtl821x_read_mmd(phydev, devnum, regnum);
}

static int rtl8214fc_write_mmd(struct phy_device *phydev, int devnum, u16 regnum, u16 val)
{
	if (__rtl8214fc_media_is_fibre(phydev))
		return -EOPNOTSUPP;

	return rtl821x_write_mmd(phydev, devnum, regnum, val);
}

static int rtl8214fc_sfp_insert(void *upstream, const struct sfp_eeprom_id *id)
{
	struct phy_device *phydev = upstream;
	const struct sfp_module_caps *caps;
	phy_interface_t iface;

	caps = sfp_get_module_caps(phydev->sfp_bus);
	iface = sfp_select_interface(phydev->sfp_bus, caps->link_modes);

	phydev_info(phydev, "%s SFP module inserted\n", phy_modes(iface));

	rtl8214fc_media_set(phydev, true);

	return 0;
}

static void rtl8214fc_fibre_rx_work(struct work_struct *work)
{
	struct rtl8214fc_priv *priv = container_of(work, struct rtl8214fc_priv,
						   fibre_rx_work.work);
	int oldpage, oldxpage = -1, err, ret = 0;
	struct phy_device *phydev = priv->phydev;

	oldpage = phy_save_page(phydev);
	if (oldpage < 0 || !__rtl8214fc_media_is_fibre(phydev))
		goto restore;

	oldxpage = __phy_read(phydev, RTL821x_EXT_PAGE_SELECT);
	if (oldxpage < 0) {
		ret = oldxpage;
		goto restore;
	}

	ret = __phy_read(phydev, MII_BMSR);
	if (ret < 0 || (ret & BMSR_LSTATUS))
		goto restore;

	/* Preserve the latched low for phylib */
	atomic_set(&priv->link_down_latched, 1);

	ret = __phy_read(phydev, MII_BMSR);
	if (ret < 0 || (ret & BMSR_LSTATUS))
		goto restore;

	ret = __phy_write(phydev, RTL821x_EXT_PAGE_SELECT, RTL821X_MEDIA_PAGE_FIBRE);
	if (ret < 0)
		goto restore;

	ret = rtl821x_write_page(phydev, RTL8214FC_FIBRE_RX_PAGE);
	if (ret < 0)
		goto restore;

	ret = __phy_set_bits(phydev, RTL8214FC_FIBRE_RX_CTRL, RTL8214FC_FIBRE_RX_RESET);
	msleep(100);
	err = __phy_clear_bits(phydev, RTL8214FC_FIBRE_RX_CTRL, RTL8214FC_FIBRE_RX_RESET);
	if (!ret)
		ret = err;
restore:
	if (oldxpage >= 0) {
		err = __phy_write(phydev, RTL821x_EXT_PAGE_SELECT, oldxpage);
		if (ret >= 0)
			ret = err;
	}
	ret = phy_restore_page(phydev, oldpage, ret);
	if (ret < 0)
		phydev_warn(phydev, "fiber RX recovery failed: %d\n", ret);
}

static void rtl8214fc_cancel_fibre_rx_work(void *data)
{
	struct rtl8214fc_priv *priv = data;

	cancel_delayed_work_sync(&priv->fibre_rx_work);
}

static void rtl8214fc_sfp_link_up(void *upstream)
{
	struct phy_device *phydev = upstream;
	struct rtl8214fc_priv *priv = phydev->priv;

	mod_delayed_work(system_power_efficient_wq, &priv->fibre_rx_work,
			 msecs_to_jiffies(500));
}

static void rtl8214fc_sfp_link_down(void *upstream)
{
	struct phy_device *phydev = upstream;

	rtl8214fc_cancel_fibre_rx_work(phydev->priv);
}

static void rtl8214fc_sfp_detach(void *upstream, struct sfp_bus *bus)
{
	rtl8214fc_sfp_link_down(upstream);
	phy_sfp_detach(upstream, bus);
}

static void rtl8214fc_sfp_remove(void *upstream)
{
	struct phy_device *phydev = upstream;

	rtl8214fc_sfp_link_down(upstream);
	rtl8214fc_media_set(phydev, false);
}

static const struct sfp_upstream_ops rtl8214fc_sfp_ops = {
	.attach = phy_sfp_attach,
	.detach = rtl8214fc_sfp_detach,
	.link_up = rtl8214fc_sfp_link_up,
	.link_down = rtl8214fc_sfp_link_down,
	.module_insert = rtl8214fc_sfp_insert,
	.module_remove = rtl8214fc_sfp_remove,
};

static int rtl8214c_phy_probe(struct phy_device *phydev)
{
	return rtl821x_package_join(phydev, 4);
}

static int rtl8218x_phy_probe(struct phy_device *phydev)
{
	return rtl821x_package_join(phydev, 8);
}

static int rtl821x_config_init(struct phy_device *phydev)
{
	/* Disable PHY-mode EEE so LPI is passed to the MAC */
	phy_modify_paged(phydev, RTL821X_PAGE_MAC, RTL821X_PHYCR2,
			 RTL821X_PHYCR2_PHY_EEE_ENABLE, 0);

	return 0;
}

static int rtl8218e_config_init(struct phy_device *phydev)
{
	int oldpage, oldxpage;
	bool is_xsgmii;

	rtl821x_config_init(phydev);

	if (phydev->mdio.addr % 8)
		return 0;

	oldpage = phy_read(phydev, RTL821x_PAGE_SELECT);
	oldxpage = phy_read(phydev, RTL821x_EXT_PAGE_SELECT);

	phy_write(phydev, RTL821x_EXT_PAGE_SELECT, 0x8);
	is_xsgmii = (phy_read_paged(phydev, 0x260, 0x12) & 0xf0) == 0xb0;

	/* QSGMII configuration is not currently supported. */
	if (!is_xsgmii)
		goto restore;

	/* Apply the minimal vendor XSGMII configuration, undocumented magic. */
	phy_write_paged(phydev, 0x4b8, 0x10, 0x8714);
	phy_write_paged(phydev, 0x4b8, 0x12, 0x7230);
	phy_write_paged(phydev, 0x4be, 0x16, 0x1000);
	phy_write_paged(phydev, 0x4bf, 0x11, 0x7000);
	phy_write_paged(phydev, 0x4bf, 0x16, 0x0042);
	phy_write_paged(phydev, 0x4ba, 0x14, 0x9d04);

	/* Reset SerDes 0. */
	phy_write_paged(phydev, 0x400, 0x10, 0x1700);
	phy_write_paged(phydev, 0x400, 0x10, 0x1703);

	/* Disable XSGMII autonegotiation on both MAC-side channel groups. */
	phy_modify_paged(phydev, 0x400, 0x12, GENMASK(9, 8), BIT(8));
	phy_modify_paged(phydev, 0x500, 0x12, GENMASK(9, 8), BIT(8));

restore:
	phy_write(phydev, RTL821x_EXT_PAGE_SELECT, oldxpage);
	phy_write(phydev, RTL821x_PAGE_SELECT, oldpage);

	return 0;
}

static int rtl8218d_config_init(struct phy_device *phydev)
{
	int oldpage, oldxpage;
	bool is_qsgmii;
	int chip_info;

	rtl821x_config_init(phydev);

	if (!rtl821x_is_package_base(phydev))
		return 0;
	/*
	 * The RTl8218D has two MAC (aka SoC side) operation modes. Either dual QSGMII
	 * or single XSGMII (Realtek proprietary) link. The mode is usually configured via
	 * strapping pins CHIP_MODE1/2. For the moment offer a configuration that at least
	 * works for RTL93xx devices. This sequence even "revives" a PHY that has been hard
	 * reset with
	 *
	 *	phy_write(phydev, 0x1e, 0x8);
	 *	phy_write_paged(phydev, 0x262, 0x10, 0x1);
	 */
	oldpage = phy_read(phydev, RTL821x_PAGE_SELECT);
	oldxpage = phy_read(phydev, RTL821x_EXT_PAGE_SELECT);

	phy_write(phydev, RTL821x_EXT_PAGE_SELECT, 0x8);
	chip_info = phy_read_paged(phydev, 0x327, 0x15);
	is_qsgmii = (phy_read_paged(phydev, 0x260, 0x12) & 0xf0) == 0xd0;

	pr_info("RTL8218D (chip_id=%d, rev_id=%d) on port %d running in %s mode\n",
		(chip_info >> 7) & 0x7, chip_info & 0x7f, phydev->mdio.addr,
		is_qsgmii ? "QSGMII" : "XSGMII");

	if (is_qsgmii) {
		for (int sds = 0; sds < 2; sds++) {
			/* unknown amplification value */
			phy_modify_paged(phydev, 0x4a8 + sds * 0x100, 0x12, BIT(3), 0);
			/* main aplification */
			phy_modify_paged(phydev, 0x4ab + sds * 0x100, 0x16, 0x3e0, 0x1e0);
			/* unknown LCCMU value */
			phy_write_paged(phydev, 0x4ac + sds * 0x100, 0x15, 0x4380);
		}
	} else {
		/* serdes 0/1 disable auto negotiation */
		phy_modify_paged(phydev, 0x400, 0x12, 0, BIT(8));
		phy_modify_paged(phydev, 0x500, 0x12, 0, BIT(8));
		/* unknown eye configuration */
		phy_modify_paged(phydev, 0x4b8, 0x12, BIT(3), 0);
	}

	/* reset serdes 0 */
	phy_write_paged(phydev, 0x400, 0x10, 0x1700);
	phy_write_paged(phydev, 0x400, 0x10, 0x1703);

	/* reset serdes 1 */
	phy_write_paged(phydev, 0x500, 0x10, 0x1400);
	phy_write_paged(phydev, 0x500, 0x10, 0x1403);

	phy_write(phydev, RTL821x_EXT_PAGE_SELECT, oldxpage);
	phy_write(phydev, RTL821x_PAGE_SELECT, oldpage);

	return 0;
}

static void rtl8218b_cmu_reset(struct phy_device *phydev, int reset_id)
{
	int bitpos = reset_id * 2;

	/* CMU seems to have 8 pairs of reset bits that always work the same way */
	phy_modify_paged(phydev, 0x467, 0x14, 0, BIT(bitpos));
	phy_modify_paged(phydev, 0x467, 0x14, 0, BIT(bitpos + 1));
	phy_write_paged(phydev, 0x467, 0x14, 0x0);
}

static int rtl8218b_config_init(struct phy_device *phydev)
{
	int oldpage, oldxpage;

	rtl821x_config_init(phydev);

	if (!rtl821x_is_package_base(phydev))
		return 0;
	/*
	 * Realtek provides two ways of initializing the PHY package. Either by U-Boot or via
	 * vendor software and SDK. In case U-Boot setup is missing, run basic configuration
	 * so that ports at least get link up and pass traffic.
	 */

	oldpage = phy_read(phydev, RTL821x_PAGE_SELECT);
	oldxpage = phy_read(phydev, RTL821x_EXT_PAGE_SELECT);
	phy_write(phydev, RTL821x_EXT_PAGE_SELECT, 0x8);

	/* activate 32/40 bit redundancy algorithm for first MAC serdes */
	phy_modify_paged(phydev, RTL821X_MAC_SDS_PAGE(0, 1), 0x14, 0, BIT(3));
	/* magic CMU setting for stable connectivity of first MAC serdes */
	phy_write_paged(phydev, 0x462, 0x15, 0x6e58);
	/* magic setting for rate select 10G full */
	phy_write_paged(phydev, 0x464, 0x15, 0x202a);
	/* magic setting for variable gain amplifier */
	phy_modify_paged(phydev, 0x464, 0x12, 0, 0x1f80);
	/* magic setting for equalizer of second MAC serdes */
	phy_write_paged(phydev, RTL821X_MAC_SDS_PAGE(1, 8), 0x12, 0x2020);
	/* unknown magic for second MAC serdes */
	phy_write_paged(phydev, RTL821X_MAC_SDS_PAGE(1, 9), 0x11, 0xc014);
	rtl8218b_cmu_reset(phydev, 0);

	for (int sds = 0; sds < 2; sds++) {
		/* disable ring PLL for serdes 2+3 */
		phy_modify_paged(phydev, RTL821X_MAC_SDS_PAGE(sds + 1, 8), 0x11, BIT(15), 0);
		/* force negative clock edge */
		phy_modify_paged(phydev, RTL821X_MAC_SDS_PAGE(sds, 0), 0x17, 0, BIT(14));
		rtl8218b_cmu_reset(phydev, 5 + sds);
		/* soft reset */
		phy_modify_paged(phydev, RTL821X_MAC_SDS_PAGE(sds, 0), 0x13, 0, BIT(6));
		phy_modify_paged(phydev, RTL821X_MAC_SDS_PAGE(sds, 0), 0x13, BIT(6), 0);
	}

	phy_write(phydev, RTL821x_EXT_PAGE_SELECT, oldxpage);
	phy_write(phydev, RTL821x_PAGE_SELECT, oldpage);

	return 0;
}

static int rtl8214fc_phy_probe(struct phy_device *phydev)
{
	struct device *dev = &phydev->mdio.dev;
	static int regs[] = {16, 19, 20, 21};
	struct rtl8214fc_priv *priv;
	int ret;

	ret = rtl821x_package_join(phydev, 4);
	if (ret)
		return ret;

	priv = devm_kzalloc(dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	priv->phydev = phydev;
	phydev->priv = priv;
	INIT_DELAYED_WORK(&priv->fibre_rx_work, rtl8214fc_fibre_rx_work);
	ret = devm_add_action_or_reset(dev, rtl8214fc_cancel_fibre_rx_work, priv);
	if (ret)
		return ret;

	/*
	 * Normally phy_probe() only initializes PHY structures and setup is run in
	 * config_init(). The RTL8214FC needs configuration before SFP probing while
	 * the preferred media is still copper. This way all SFP events (even before
	 * the first config_init()) will find a consistent port state.
	 */

	/* Step 1 - package setup: Due to similar design reuse RTL8218B coding */
	ret = rtl8218b_config_init(phydev);
	if (ret)
		return ret;

	if (rtl821x_is_package_base(phydev)) {
		/* Force all ports to copper */
		phy_write(phydev, RTL821x_EXT_PAGE_SELECT, RTL821X_MEDIA_PAGE_INTERNAL);
		for (int port = 0; port < 4; port++)
			phy_modify_paged(phydev, 0x266, regs[port], 0, GENMASK(11, 10));
	}

	/* Step 2 - port setup */
	phy_write(phydev, RTL821x_EXT_PAGE_SELECT, RTL821X_MEDIA_PAGE_FIBRE);
	/* set fiber SerDes RX to negative edge */
	phy_modify_paged(phydev, 0x8, 0x17, 0, BIT(14));
	/* auto negotiation disable link on */
	phy_modify_paged(phydev, 0x8, 0x14, 0, BIT(2));
	/* disable fiber 100MBit */
	phy_modify_paged(phydev, 0x8, 0x11, BIT(5), 0);
	phy_write(phydev, RTL821x_EXT_PAGE_SELECT, RTL821X_MEDIA_PAGE_AUTO);

	/* Disable EEE. 0xa5d/0x10 is the same as MDIO_MMD_AN / MDIO_AN_EEE_ADV */
	phy_write_paged(phydev, 0xa5d, 0x10, 0x0000);

	return phy_sfp_probe(phydev, &rtl8214fc_sfp_ops);
}

static struct phy_driver rtl83xx_phy_driver[] = {
	{
		PHY_ID_MATCH_EXACT(PHY_ID_RTL8214C),
		.name		= "Realtek RTL8214C",
		.features	= PHY_GBIT_FEATURES,
		.probe		= rtl8214c_phy_probe,
		.read_page	= rtl821x_read_page,
		.write_page	= rtl821x_write_page,
		.suspend	= genphy_suspend,
		.resume		= genphy_resume,
	},
	{
		PHY_ID_MATCH_EXACT(PHY_ID_RTL8214QF),
		.name		= "Realtek RTL8214QF",
		.config_aneg	= genphy_c37_config_aneg,
		.config_init	= rtl8214qf_config_init,
		.get_features	= rtl8214qf_get_features,
		.probe		= rtl8214qf_probe,
		.read_page	= rtl821x_read_page,
		.read_status	= rtl8214qf_read_status,
		.resume		= genphy_resume,
		.suspend	= genphy_suspend,
		.write_page	= rtl821x_write_page,
	},
	{
		.match_phy_device = rtl8214fc_match_phy_device,
		.name		= "Realtek RTL8214FC",
		.config_aneg	= rtl8214fc_config_aneg,
		.get_features	= rtl8214fc_get_features,
		.get_tunable    = rtl8214fc_get_tunable,
		.probe		= rtl8214fc_phy_probe,
		.read_mmd	= rtl8214fc_read_mmd,
		.read_page	= rtl821x_read_page,
		.read_status    = rtl8214fc_read_status,
		.resume		= rtl8214fc_resume,
		.set_tunable	= rtl8214fc_set_tunable,
		.suspend	= rtl8214fc_suspend,
		.write_mmd	= rtl8214fc_write_mmd,
		.write_page	= rtl821x_write_page,
	},
	{
		.match_phy_device = rtl8218b_ext_match_phy_device,
		.name		= "Realtek RTL8218B (external)",
		.config_init	= rtl8218b_config_init,
		.features	= PHY_GBIT_FEATURES,
		.probe		= rtl8218x_phy_probe,
		.read_mmd	= rtl821x_read_mmd,
		.read_page	= rtl821x_read_page,
		.resume		= genphy_resume,
		.suspend	= genphy_suspend,
		.write_mmd	= rtl821x_write_mmd,
		.write_page	= rtl821x_write_page,
	},
	{
		PHY_ID_MATCH_MODEL(PHY_ID_RTL8218B_I),
		.name		= "Realtek RTL8218B (internal)",
		.config_init	= rtl821x_config_init,
		.features	= PHY_GBIT_FEATURES,
		.probe		= rtl8218x_phy_probe,
		.read_mmd	= rtl821x_read_mmd,
		.read_page	= rtl821x_read_page,
		.resume		= genphy_resume,
		.suspend	= genphy_suspend,
		.write_mmd	= rtl821x_write_mmd,
		.write_page	= rtl821x_write_page,
	},
	{
		PHY_ID_MATCH_EXACT(PHY_ID_RTL8218D),
		.name		= "REALTEK RTL8218D",
		.config_init	= rtl8218d_config_init,
		.features	= PHY_GBIT_FEATURES,
		.probe		= rtl8218x_phy_probe,
		.read_mmd	= rtl821x_read_mmd,
		.read_page	= rtl821x_read_page,
		.resume		= genphy_resume,
		.suspend	= genphy_suspend,
		.write_mmd	= rtl821x_write_mmd,
		.write_page	= rtl821x_write_page,
	},
	{
		PHY_ID_MATCH_EXACT(PHY_ID_RTL8218E),
		.name		= "REALTEK RTL8218E",
		.config_init	= rtl8218e_config_init,
		.features	= PHY_GBIT_FEATURES,
		.probe		= rtl8218x_phy_probe,
		.read_mmd	= rtl821x_read_mmd,
		.read_page	= rtl821x_read_page,
		.resume		= genphy_resume,
		.suspend	= genphy_suspend,
		.write_mmd	= rtl821x_write_mmd,
		.write_page	= rtl821x_write_page,
	},
};

module_phy_driver(rtl83xx_phy_driver);

static const struct mdio_device_id __maybe_unused rtl83xx_tbl[] = {
	{ PHY_ID_MATCH_MODEL(PHY_ID_RTL8214_OR_8218) },
	{ }
};

MODULE_DEVICE_TABLE(mdio, rtl83xx_tbl);

MODULE_AUTHOR("B. Koblitz");
MODULE_DESCRIPTION("RTL83xx PHY driver");
MODULE_LICENSE("GPL");

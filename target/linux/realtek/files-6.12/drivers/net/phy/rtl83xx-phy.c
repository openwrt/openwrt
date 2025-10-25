// SPDX-License-Identifier: GPL-2.0-only
/* Realtek RTL838X Ethernet MDIO interface driver
 *
 * Copyright (C) 2020 B. Koblitz
 */

#include <linux/module.h>
#include <linux/delay.h>
#include <linux/of.h>
#include <linux/phy.h>
#include <linux/netdevice.h>
#include <linux/firmware.h>
#include <linux/crc32.h>
#include <linux/sfp.h>
#include <linux/mii.h>
#include <linux/mdio.h>

#include <asm/mach-rtl838x/mach-rtl83xx.h>
#include "rtl83xx-phy.h"

extern struct rtl83xx_soc_info soc_info;

/* all Clause-22 RealTek MDIO PHYs use register 0x1f for page select */
#define RTL8XXX_PAGE_SELECT		0x1f

#define RTL8XXX_PAGE_MAIN		0x0000
#define RTL821X_PAGE_PORT		0x0266
#define RTL821X_PAGE_POWER		0x0a40
#define RTL821X_PAGE_GPHY		0x0a42
#define RTL821X_PAGE_MAC		0x0a43
#define RTL821X_PAGE_STATE		0x0b80
#define RTL821X_PAGE_PATCH		0x0b82
#define RTL821X_MAC_SDS_PAGE(sds, page)	(0x404 + (sds) * 0x20 + (page))

/* Using the special page 0xfff with the MDIO controller found in
 * RealTek SoCs allows to access the PHY in RAW mode, ie. bypassing
 * the cache and paging engine of the MDIO controller.
 */
#define RTL838X_PAGE_RAW		0x0fff
#define RTL839X_PAGE_RAW		0x1fff

/* internal RTL821X PHY uses register 0x1d to select media page */
#define RTL821XINT_MEDIA_PAGE_SELECT	0x1d
/* external RTL821X PHY uses register 0x1e to select media page */
#define RTL821XEXT_MEDIA_PAGE_SELECT	0x1e
#define RTL821X_PHYCR2			0x19
#define RTL821X_PHYCR2_PHY_EEE_ENABLE	BIT(5)

#define RTL821X_CHIP_ID			0x6276

#define RTL821X_MEDIA_PAGE_AUTO		0
#define RTL821X_MEDIA_PAGE_COPPER	1
#define RTL821X_MEDIA_PAGE_FIBRE	3
#define RTL821X_MEDIA_PAGE_INTERNAL	8

static const struct firmware rtl838x_8380_fw;
static const struct firmware rtl838x_8214fc_fw;
static const struct firmware rtl838x_8218b_fw;

static inline struct phy_device *get_package_phy(struct phy_device *phydev, int port)
{
	return mdiobus_get_phy(phydev->mdio.bus, phydev->shared->base_addr + port);
}

static inline struct phy_device *get_base_phy(struct phy_device *phydev)
{
	return get_package_phy(phydev, 0);
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

	oldpage = mdiobus_read(bus, addr, 0x1f);
	oldxpage = mdiobus_read(bus, addr, 0x1e);

	mdiobus_write(bus, addr, 0x1e, 0x8);
	mdiobus_write(bus, addr, 0x1f, 0x278);
	mdiobus_write(bus, addr, 0x12, 0x455);
	mdiobus_write(bus, addr, 0x1f, 0x260);
	chip_mode = mdiobus_read(bus, addr, 0x12);
	dev_dbg(&phydev->mdio.dev, "got RTL8218B/RTL8214Fx chip mode %04x\n", chip_mode);

	mdiobus_write(bus, addr, 0x1e, oldxpage);
	mdiobus_write(bus, addr, 0x1f, oldpage);

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

static void rtl8380_int_phy_on_off(struct phy_device *phydev, bool on)
{
	phy_modify(phydev, 0, BMCR_PDOWN, on ? 0 : BMCR_PDOWN);
}

static void rtl8380_phy_reset(struct phy_device *phydev)
{
	phy_modify(phydev, 0, BMCR_RESET, BMCR_RESET);
}

/* Read the link and speed status of the 2 internal SGMII/1000Base-X
 * ports of the RTL838x SoCs
 */
static int rtl8380_read_status(struct phy_device *phydev)
{
	int err;

	err = genphy_read_status(phydev);

	if (phydev->link) {
		phydev->speed = SPEED_1000;
		phydev->duplex = DUPLEX_FULL;
	}

	return err;
}

/* Read the link and speed status of the 2 internal SGMII/1000Base-X
 * ports of the RTL8393 SoC
 */
static int rtl8393_read_status(struct phy_device *phydev)
{
	int offset = 0;
	int err;
	int phy_addr = phydev->mdio.addr;
	u32 v;

	err = genphy_read_status(phydev);
	if (phy_addr == 49)
		offset = 0x100;

	if (phydev->link) {
		phydev->speed = SPEED_100;
		/* Read SPD_RD_00 (bit 13) and SPD_RD_01 (bit 6) out of the internal
		 * PHY registers
		 */
		v = sw_r32(RTL839X_SDS12_13_XSG0 + offset + 0x80);
		if (!(v & (1 << 13)) && (v & (1 << 6)))
			phydev->speed = SPEED_1000;
		phydev->duplex = DUPLEX_FULL;
	}

	return err;
}

static int rtl821x_read_page(struct phy_device *phydev)
{
	return __phy_read(phydev, RTL8XXX_PAGE_SELECT);
}

static int rtl821x_write_page(struct phy_device *phydev, int page)
{
	return __phy_write(phydev, RTL8XXX_PAGE_SELECT, page);
}

static struct fw_header *rtl838x_request_fw(struct phy_device *phydev,
					    const struct firmware *fw,
					    const char *name)
{
	struct device *dev = &phydev->mdio.dev;
	int err;
	struct fw_header *h;
	uint32_t checksum, my_checksum;

	err = request_firmware(&fw, name, dev);
	if (err < 0)
		goto out;

	if (fw->size < sizeof(struct fw_header)) {
		pr_err("Firmware size too small.\n");
		err = -EINVAL;
		goto out;
	}

	h = (struct fw_header *) fw->data;
	pr_info("Firmware loaded. Size %d, magic: %08x\n", fw->size, h->magic);

	if (h->magic != 0x83808380) {
		pr_err("Wrong firmware file: MAGIC mismatch.\n");
		goto out;
	}

	checksum = h->checksum;
	h->checksum = 0;
	my_checksum = ~crc32(0xFFFFFFFFU, fw->data, fw->size);
	if (checksum != my_checksum) {
		pr_err("Firmware checksum mismatch.\n");
		err = -EINVAL;
		goto out;
	}
	h->checksum = checksum;

	return h;
out:
	dev_err(dev, "Unable to load firmware %s (%d)\n", name, err);
	return NULL;
}

static void rtl821x_phy_setup_package_broadcast(struct phy_device *phydev, bool enable)
{
	int mac = phydev->mdio.addr;

	/* select main page 0 */
	phy_write_paged(phydev, RTL838X_PAGE_RAW, RTL8XXX_PAGE_SELECT, RTL8XXX_PAGE_MAIN);
	/* write to 0x8 to register 0x1d on main page 0 */
	phy_write_paged(phydev, RTL838X_PAGE_RAW, RTL821XINT_MEDIA_PAGE_SELECT, RTL821X_MEDIA_PAGE_INTERNAL);
	/* select page 0x266 */
	phy_write_paged(phydev, RTL838X_PAGE_RAW, RTL8XXX_PAGE_SELECT, RTL821X_PAGE_PORT);
	/* set phy id and target broadcast bitmap in register 0x16 on page 0x266 */
	phy_write_paged(phydev, RTL838X_PAGE_RAW, 0x16, (enable?0xff00:0x00) | mac);
	/* return to main page 0 */
	phy_write_paged(phydev, RTL838X_PAGE_RAW, RTL8XXX_PAGE_SELECT, RTL8XXX_PAGE_MAIN);
	/* write to 0x0 to register 0x1d on main page 0 */
	phy_write_paged(phydev, RTL838X_PAGE_RAW, RTL821XINT_MEDIA_PAGE_SELECT, RTL821X_MEDIA_PAGE_AUTO);
	mdelay(1);
}

static int rtl8390_configure_generic(struct phy_device *phydev)
{
	int mac = phydev->mdio.addr;
	u32 val, phy_id;

	val = phy_read(phydev, 2);
	phy_id = val << 16;
	val = phy_read(phydev, 3);
	phy_id |= val;
	pr_debug("Phy on MAC %d: %x\n", mac, phy_id);

	/* Read internal PHY ID */
	phy_write_paged(phydev, 31, 27, 0x0002);
	val = phy_read_paged(phydev, 31, 28);

	/* Internal RTL8218B, version 2 */
	phydev_info(phydev, "Detected unknown %x\n", val);

	return 0;
}

static int rtl821x_prepare_patch(struct phy_device *phydev, int ports)
{
	struct phy_device *patchphy;
	int tries = 50;

	for (int port = 0; port < ports; port++) {
		patchphy = get_package_phy(phydev, port);
		phy_write_paged(patchphy, RTL821X_PAGE_PATCH, 0x10, 0x10);
	}

	for (int port = 0; port < ports; port++) {
		patchphy = get_package_phy(phydev, port);
		while (tries && !(phy_read_paged(patchphy, RTL821X_PAGE_STATE, 0x10) & 0x40)) {
			tries--;
			usleep_range(10000, 25000);
		};
	}

	if (!tries)
		phydev_err(get_package_phy(phydev, 0), "package not ready for patch.\n");

	return tries ? 0 : -EIO;
}

static int rtl8380_configure_int_rtl8218b(struct phy_device *phydev)
{
	u32 *rtl838x_6275B_intPhy_perport;
	u32 *rtl8218b_6276B_hwEsd_perport;
	struct phy_device *patchphy;
	struct fw_header *h;
	int ret;
	u32 val;

	/* Read internal PHY ID */
	phy_write_paged(phydev, 31, 27, 0x0002);
	val = phy_read_paged(phydev, 31, 28);
	if (val != 0x6275) {
		phydev_err(phydev, "Expected internal RTL8218B, found PHY-ID %x\n", val);
		return -1;
	}

	h = rtl838x_request_fw(phydev, &rtl838x_8380_fw, FIRMWARE_838X_8380_1);
	if (!h)
		return -1;

	if (h->phy != 0x83800000) {
		phydev_err(phydev, "Wrong firmware file: PHY mismatch.\n");
		return -1;
	}

	rtl838x_6275B_intPhy_perport = (void *)h + sizeof(struct fw_header) + h->parts[8].start;
	rtl8218b_6276B_hwEsd_perport = (void *)h + sizeof(struct fw_header) + h->parts[9].start;

	phydev_info(phydev, "patch\n");

	val = phy_read(phydev, MII_BMCR);
	if (val & BMCR_PDOWN)
		rtl8380_int_phy_on_off(phydev, true);
	else
		rtl8380_phy_reset(phydev);
	msleep(100);

	ret = rtl821x_prepare_patch(phydev, 8);
	if (ret)
		return ret;

	for (int port = 0; port < 8; port++) {
		int i;

		patchphy = get_package_phy(phydev, port);

		i = 0;
		while (rtl838x_6275B_intPhy_perport[i * 2]) {
			phy_write_paged(patchphy, RTL838X_PAGE_RAW,
					rtl838x_6275B_intPhy_perport[i * 2],
					rtl838x_6275B_intPhy_perport[i * 2 + 1]);
			i++;
		}
		i = 0;
		while (rtl8218b_6276B_hwEsd_perport[i * 2]) {
			phy_write_paged(patchphy, RTL838X_PAGE_RAW,
					rtl8218b_6276B_hwEsd_perport[i * 2],
					rtl8218b_6276B_hwEsd_perport[i * 2 + 1]);
			i++;
		}
	}

	return 0;
}

static int rtl8380_configure_ext_rtl8218b(struct phy_device *phydev)
{
	u32 *rtl8218B_6276B_rtl8380_perport;
	u32 *rtl8380_rtl8218b_perchip;
	u32 *rtl8380_rtl8218b_perport;
	struct phy_device *patchphy;
	struct fw_header *h;
	u32 val, ipd;
	int ret;

	/* Read internal PHY ID */
	phy_write_paged(phydev, 31, 27, 0x0002);
	val = phy_read_paged(phydev, 31, 28);
	if (val != RTL821X_CHIP_ID) {
		phydev_err(phydev, "Expected external RTL8218B, found PHY-ID %x\n", val);
		return -1;
	}

	h = rtl838x_request_fw(phydev, &rtl838x_8218b_fw, FIRMWARE_838X_8218b_1);
	if (!h)
		return -1;

	if (h->phy != 0x8218b000) {
		phydev_err(phydev, "Wrong firmware file: PHY mismatch.\n");
		return -1;
	}

	rtl8380_rtl8218b_perchip = (void *)h + sizeof(struct fw_header) + h->parts[0].start;
	rtl8218B_6276B_rtl8380_perport = (void *)h + sizeof(struct fw_header) + h->parts[1].start;
	rtl8380_rtl8218b_perport = (void *)h + sizeof(struct fw_header) + h->parts[2].start;

	val = phy_read(phydev, MII_BMCR);
	if (val & BMCR_PDOWN)
		rtl8380_int_phy_on_off(phydev, true);
	else
		rtl8380_phy_reset(phydev);

	msleep(100);

	/* Get Chip revision */
	phy_write_paged(phydev, RTL838X_PAGE_RAW, RTL8XXX_PAGE_SELECT, RTL8XXX_PAGE_MAIN);
	phy_write_paged(phydev, RTL838X_PAGE_RAW, 0x1b, 0x4);
	val = phy_read_paged(phydev, RTL838X_PAGE_RAW, 0x1c);

	phydev_info(phydev, "patch chip revision %d\n", val);

	for (int i = 0; rtl8380_rtl8218b_perchip[i * 3] &&
	                rtl8380_rtl8218b_perchip[i * 3 + 1]; i++) {
		patchphy = get_package_phy(phydev, rtl8380_rtl8218b_perchip[i * 3]);
		phy_write_paged(patchphy, RTL838X_PAGE_RAW,
				rtl8380_rtl8218b_perchip[i * 3 + 1],
				rtl8380_rtl8218b_perchip[i * 3 + 2]);
	}

	/* Enable PHY */
	for (int port = 0; port < 8; port++) {
		patchphy = get_package_phy(phydev, port);
		phy_write_paged(patchphy, RTL838X_PAGE_RAW, RTL8XXX_PAGE_SELECT, RTL8XXX_PAGE_MAIN);
		phy_write_paged(patchphy, RTL838X_PAGE_RAW, 0x00, 0x1140);
	}
	mdelay(100);

	ret = rtl821x_prepare_patch(phydev, 8);
	if (ret)
		return ret;

	/* Use Broadcast ID method for patching */
	rtl821x_phy_setup_package_broadcast(phydev, true);

	phy_write_paged(phydev, RTL838X_PAGE_RAW, 30, 8);
	phy_write_paged(phydev, 0x26e, 17, 0xb);
	phy_write_paged(phydev, 0x26e, 16, 0x2);
	mdelay(1);
	ipd = phy_read_paged(phydev, 0x26e, 19);
	phy_write_paged(phydev, 0, 30, 0);
	ipd = (ipd >> 4) & 0xf; /* unused ? */

	for (int i = 0; rtl8218B_6276B_rtl8380_perport[i * 2]; i++) {
		phy_write_paged(phydev, RTL838X_PAGE_RAW, rtl8218B_6276B_rtl8380_perport[i * 2],
		                rtl8218B_6276B_rtl8380_perport[i * 2 + 1]);
	}

	/* Disable broadcast ID */
	rtl821x_phy_setup_package_broadcast(phydev, false);

	return 0;
}

static bool __rtl8214fc_media_is_fibre(struct phy_device *phydev)
{
	struct phy_device *basephy = get_base_phy(phydev);
	static int regs[] = {16, 19, 20, 21};
	int reg = regs[phydev->mdio.addr & 3];
	int oldpage, val;

	/* The fiber status is a package "global" in the first phy. */
	oldpage = __phy_read(basephy, RTL8XXX_PAGE_SELECT);
	__phy_write(basephy, RTL821XINT_MEDIA_PAGE_SELECT, RTL821X_MEDIA_PAGE_INTERNAL);
	__phy_write(basephy, RTL8XXX_PAGE_SELECT, RTL821X_PAGE_PORT);
	val = __phy_read(basephy, reg);
	__phy_write(basephy, RTL821XINT_MEDIA_PAGE_SELECT, RTL821X_MEDIA_PAGE_AUTO);
	__phy_write(basephy, RTL8XXX_PAGE_SELECT, oldpage);

	return !(val & BMCR_PDOWN);
}

static bool rtl8214fc_media_is_fibre(struct phy_device *phydev)
{
	struct mii_bus *bus = phydev->mdio.bus;
	int ret;

	mutex_lock(&bus->mdio_lock);
	ret = __rtl8214fc_media_is_fibre(phydev);
	mutex_unlock(&bus->mdio_lock);

	return ret;
}

static void rtl8214fc_power_set(struct phy_device *phydev, int port, bool on)
{
	int page = port == PORT_FIBRE ? RTL821X_MEDIA_PAGE_FIBRE : RTL821X_MEDIA_PAGE_COPPER;
	int pdown = on ? 0 : BMCR_PDOWN;

	phydev_info(phydev, "power %s %s\n", on ? "on" : "off",
		    port == PORT_FIBRE ? "fibre" : "copper");

	phy_write(phydev, RTL821XINT_MEDIA_PAGE_SELECT, page);
	phy_modify_paged(phydev, RTL821X_PAGE_POWER, 0x10, BMCR_PDOWN, pdown);
	phy_write(phydev, RTL821XINT_MEDIA_PAGE_SELECT, RTL821X_MEDIA_PAGE_AUTO);
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
	int pdown = set_fibre ? 0 : BMCR_PDOWN;
	static int regs[] = {16, 19, 20, 21};
	int reg = regs[phydev->mdio.addr & 3];

	phydev_info(phydev, "switch to %s\n", set_fibre ? "fibre" : "copper");
	phy_write_paged(basephy, RTL838X_PAGE_RAW, RTL821XINT_MEDIA_PAGE_SELECT, RTL821X_MEDIA_PAGE_INTERNAL);
	phy_modify_paged(basephy, RTL821X_PAGE_PORT, reg, BMCR_PDOWN, pdown);
	phy_write_paged(basephy, RTL838X_PAGE_RAW, RTL821XINT_MEDIA_PAGE_SELECT, RTL821X_MEDIA_PAGE_AUTO);

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
	linkmode_set_bit(ETHTOOL_LINK_MODE_FIBRE_BIT, phydev->supported);

	return 0;
}

static int rtl8214fc_read_status(struct phy_device *phydev)
{
	bool changed;
	int ret;

	if (rtl8214fc_media_is_fibre(phydev)) {
		phydev->port = PORT_FIBRE;
		ret = genphy_c37_read_status(phydev, &changed);
	} else {
		phydev->port = PORT_MII; /* for now aligend with rest of code */
		ret = genphy_read_status(phydev);
	}

	return ret;
}

static int rtl8214fc_config_aneg(struct phy_device *phydev)
{
	int ret;

	if (rtl8214fc_media_is_fibre(phydev))
		ret = genphy_c37_config_aneg(phydev);
	else
		ret = genphy_config_aneg(phydev);

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

static int rtl8380_configure_rtl8214c(struct phy_device *phydev)
{
	u32 phy_id, val;
	int mac = phydev->mdio.addr;

	val = phy_read(phydev, 2);
	phy_id = val << 16;
	val = phy_read(phydev, 3);
	phy_id |= val;
	pr_debug("Phy on MAC %d: %x\n", mac, phy_id);

	phydev_info(phydev, "Detected external RTL8214C\n");

	/* GPHY auto conf */
	phy_write_paged(phydev, RTL821X_PAGE_GPHY, RTL821XINT_MEDIA_PAGE_SELECT, RTL821X_MEDIA_PAGE_AUTO);

	return 0;
}

static int rtl8380_configure_rtl8214fc(struct phy_device *phydev)
{
	u32 *rtl8380_rtl8214fc_perchip;
	u32 *rtl8380_rtl8214fc_perport;
	struct phy_device *patchphy;
	struct fw_header *h;
	u32 val;
	int ret;

	/* Read internal PHY id */
	phy_write_paged(phydev, 0, RTL821XEXT_MEDIA_PAGE_SELECT, RTL821X_MEDIA_PAGE_COPPER);
	phy_write_paged(phydev, 0x1f, 0x1b, 0x0002);
	val = phy_read_paged(phydev, 0x1f, 0x1c);
	if (val != RTL821X_CHIP_ID) {
		phydev_err(phydev, "Expected external RTL8214FC, found PHY-ID %x\n", val);
		return -1;
	}

	h = rtl838x_request_fw(phydev, &rtl838x_8214fc_fw, FIRMWARE_838X_8214FC_1);
	if (!h)
		return -1;

	if (h->phy != 0x8214fc00) {
		phydev_err(phydev, "Wrong firmware file: PHY mismatch.\n");
		return -1;
	}

	phydev_info(phydev, "patch\n");

	rtl8380_rtl8214fc_perchip = (void *)h + sizeof(struct fw_header) + h->parts[0].start;
	rtl8380_rtl8214fc_perport = (void *)h + sizeof(struct fw_header) + h->parts[1].start;

	/* detect phy version */
	phy_write_paged(phydev, RTL838X_PAGE_RAW, 27, 0x0004);
	val = phy_read_paged(phydev, RTL838X_PAGE_RAW, 28);

	val = phy_read(phydev, 16);
	if (val & BMCR_PDOWN) {
		rtl8214fc_power_set(phydev, PORT_MII, true);
		rtl8214fc_power_set(phydev, PORT_FIBRE, true);
	} else
		rtl8380_phy_reset(phydev);

	msleep(100);
	phy_write_paged(phydev, 0, RTL821XEXT_MEDIA_PAGE_SELECT, RTL821X_MEDIA_PAGE_COPPER);

	for (int i = 0; rtl8380_rtl8214fc_perchip[i * 3] &&
	                rtl8380_rtl8214fc_perchip[i * 3 + 1]; i++) {
		u32 page = 0;

		if (rtl8380_rtl8214fc_perchip[i * 3 + 1] == 0x1f)
			page = rtl8380_rtl8214fc_perchip[i * 3 + 2];
		if (rtl8380_rtl8214fc_perchip[i * 3 + 1] == 0x13 && page == 0x260) {
			val = phy_read_paged(phydev, 0x260, 13);
			val = (val & 0x1f00) | (rtl8380_rtl8214fc_perchip[i * 3 + 2] & 0xe0ff);
			phy_write_paged(phydev, RTL838X_PAGE_RAW,
					rtl8380_rtl8214fc_perchip[i * 3 + 1], val);
		} else {
			phy_write_paged(phydev, RTL838X_PAGE_RAW,
					rtl8380_rtl8214fc_perchip[i * 3 + 1],
					rtl8380_rtl8214fc_perchip[i * 3 + 2]);
		}
	}

	/* Force copper medium */
	for (int port = 0; port < 4; port++) {
		patchphy = get_package_phy(phydev, port);
		phy_write_paged(patchphy, RTL838X_PAGE_RAW, RTL8XXX_PAGE_SELECT, RTL8XXX_PAGE_MAIN);
		phy_write_paged(patchphy, RTL838X_PAGE_RAW, RTL821XEXT_MEDIA_PAGE_SELECT, RTL821X_MEDIA_PAGE_COPPER);
	}

	/* Enable PHY */
	for (int port = 0; port < 4; port++) {
		patchphy = get_package_phy(phydev, port);
		phy_write_paged(patchphy, RTL838X_PAGE_RAW, RTL8XXX_PAGE_SELECT, RTL8XXX_PAGE_MAIN);
		phy_write_paged(patchphy, RTL838X_PAGE_RAW, 0x00, 0x1140);
	}
	mdelay(100);

	/* Disable Autosensing */
	for (int port = 0; port < 4; port++) {
		int i;

		patchphy = get_package_phy(phydev, port);
		for (i = 0; i < 100; i++) {
			val = phy_read_paged(patchphy, RTL821X_PAGE_GPHY, 0x10);
			if ((val & 0x7) >= 3)
				break;
		}
		if (i >= 100) {
			phydev_err(phydev, "Could not disable autosensing\n");
			return -1;
		}
	}

	ret = rtl821x_prepare_patch(phydev, 4);
	if (ret)
		return ret;

	/* Use Broadcast ID method for patching */
	rtl821x_phy_setup_package_broadcast(phydev, true);

	for (int i = 0; rtl8380_rtl8214fc_perport[i * 2]; i++) {
		phy_write_paged(phydev, RTL838X_PAGE_RAW, rtl8380_rtl8214fc_perport[i * 2],
		                rtl8380_rtl8214fc_perport[i * 2 + 1]);
	}

	/* Disable broadcast ID */
	rtl821x_phy_setup_package_broadcast(phydev, false);

	/* Auto medium selection */
	for (int i = 0; i < 4; i++) {
		phy_write_paged(phydev, RTL838X_PAGE_RAW, RTL8XXX_PAGE_SELECT, RTL8XXX_PAGE_MAIN);
		phy_write_paged(phydev, RTL838X_PAGE_RAW, RTL821XEXT_MEDIA_PAGE_SELECT, RTL821X_MEDIA_PAGE_AUTO);
	}

	return 0;
}

static int rtl8380_configure_serdes(struct phy_device *phydev)
{
	u32 v;
	u32 sds_conf_value;
	int i;
	struct fw_header *h;
	u32 *rtl8380_sds_take_reset;
	u32 *rtl8380_sds_common;
	u32 *rtl8380_sds01_qsgmii_6275b;
	u32 *rtl8380_sds23_qsgmii_6275b;
	u32 *rtl8380_sds4_fiber_6275b;
	u32 *rtl8380_sds5_fiber_6275b;
	u32 *rtl8380_sds_reset;
	u32 *rtl8380_sds_release_reset;

	phydev_info(phydev, "Detected internal RTL8380 SERDES\n");

	h = rtl838x_request_fw(phydev, &rtl838x_8218b_fw, FIRMWARE_838X_8380_1);
	if (!h)
		return -1;

	if (h->magic != 0x83808380) {
		phydev_err(phydev, "Wrong firmware file: magic number mismatch.\n");
		return -1;
	}

	rtl8380_sds_take_reset = (void *)h + sizeof(struct fw_header) + h->parts[0].start;

	rtl8380_sds_common = (void *)h + sizeof(struct fw_header) + h->parts[1].start;

	rtl8380_sds01_qsgmii_6275b = (void *)h + sizeof(struct fw_header) + h->parts[2].start;

	rtl8380_sds23_qsgmii_6275b = (void *)h + sizeof(struct fw_header) + h->parts[3].start;

	rtl8380_sds4_fiber_6275b = (void *)h + sizeof(struct fw_header) + h->parts[4].start;

	rtl8380_sds5_fiber_6275b = (void *)h + sizeof(struct fw_header) + h->parts[5].start;

	rtl8380_sds_reset = (void *)h + sizeof(struct fw_header) + h->parts[6].start;

	rtl8380_sds_release_reset = (void *)h + sizeof(struct fw_header) + h->parts[7].start;

	/* Back up serdes power off value */
	sds_conf_value = sw_r32(RTL838X_SDS_CFG_REG);
	pr_info("SDS power down value: %x\n", sds_conf_value);

	/* take serdes into reset */
	i = 0;
	while (rtl8380_sds_take_reset[2 * i]) {
		sw_w32(rtl8380_sds_take_reset[2 * i + 1], rtl8380_sds_take_reset[2 * i]);
		i++;
		udelay(1000);
	}

	/* apply common serdes patch */
	i = 0;
	while (rtl8380_sds_common[2 * i]) {
		sw_w32(rtl8380_sds_common[2 * i + 1], rtl8380_sds_common[2 * i]);
		i++;
		udelay(1000);
	}

	/* internal R/W enable */
	sw_w32(3, RTL838X_INT_RW_CTRL);

	/* SerDes ports 4 and 5 are FIBRE ports */
	sw_w32_mask(0x7 | 0x38, 1 | (1 << 3), RTL838X_INT_MODE_CTRL);

	/* SerDes module settings, SerDes 0-3 are QSGMII */
	v = 0x6 << 25 | 0x6 << 20 | 0x6 << 15 | 0x6 << 10;
	/* SerDes 4 and 5 are 1000BX FIBRE */
	v |= 0x4 << 5 | 0x4;
	sw_w32(v, RTL838X_SDS_MODE_SEL);

	pr_info("PLL control register: %x\n", sw_r32(RTL838X_PLL_CML_CTRL));
	sw_w32_mask(0xfffffff0, 0xaaaaaaaf & 0xf, RTL838X_PLL_CML_CTRL);
	i = 0;
	while (rtl8380_sds01_qsgmii_6275b[2 * i]) {
		sw_w32(rtl8380_sds01_qsgmii_6275b[2 * i + 1],
		       rtl8380_sds01_qsgmii_6275b[2 * i]);
		i++;
	}

	i = 0;
	while (rtl8380_sds23_qsgmii_6275b[2 * i]) {
		sw_w32(rtl8380_sds23_qsgmii_6275b[2 * i + 1], rtl8380_sds23_qsgmii_6275b[2 * i]);
		i++;
	}

	i = 0;
	while (rtl8380_sds4_fiber_6275b[2 * i]) {
		sw_w32(rtl8380_sds4_fiber_6275b[2 * i + 1], rtl8380_sds4_fiber_6275b[2 * i]);
		i++;
	}

	i = 0;
	while (rtl8380_sds5_fiber_6275b[2 * i]) {
		sw_w32(rtl8380_sds5_fiber_6275b[2 * i + 1], rtl8380_sds5_fiber_6275b[2 * i]);
		i++;
	}

	i = 0;
	while (rtl8380_sds_reset[2 * i]) {
		sw_w32(rtl8380_sds_reset[2 * i + 1], rtl8380_sds_reset[2 * i]);
		i++;
	}

	i = 0;
	while (rtl8380_sds_release_reset[2 * i]) {
		sw_w32(rtl8380_sds_release_reset[2 * i + 1], rtl8380_sds_release_reset[2 * i]);
		i++;
	}

	pr_info("SDS power down value now: %x\n", sw_r32(RTL838X_SDS_CFG_REG));
	sw_w32(sds_conf_value, RTL838X_SDS_CFG_REG);

	pr_info("Configuration of SERDES done\n");

	return 0;
}

static int rtl8390_configure_serdes(struct phy_device *phydev)
{
	phydev_info(phydev, "Detected internal RTL8390 SERDES\n");

	/* In autoneg state, force link, set SR4_CFG_EN_LINK_FIB1G */
	sw_w32_mask(0, 1 << 18, RTL839X_SDS12_13_XSG0 + 0x0a);

	/* Disable EEE: Clear FRE16_EEE_RSG_FIB1G, FRE16_EEE_STD_FIB1G,
	 * FRE16_C1_PWRSAV_EN_FIB1G, FRE16_C2_PWRSAV_EN_FIB1G
	 * and FRE16_EEE_QUIET_FIB1G
	 */
	sw_w32_mask(0x1f << 10, 0, RTL839X_SDS12_13_XSG0 + 0xe0);

	return 0;
}

static int rtl8214fc_sfp_insert(void *upstream, const struct sfp_eeprom_id *id)
{
	__ETHTOOL_DECLARE_LINK_MODE_MASK(support) = { 0, };
	DECLARE_PHY_INTERFACE_MASK(interfaces);
	struct phy_device *phydev = upstream;
	phy_interface_t iface;

	sfp_parse_support(phydev->sfp_bus, id, support, interfaces);
	iface = sfp_select_interface(phydev->sfp_bus, support);

	phydev_info(phydev, "%s SFP module inserted\n", phy_modes(iface));

	rtl8214fc_media_set(phydev, true);

	return 0;
}

static void rtl8214fc_sfp_remove(void *upstream)
{
	struct phy_device *phydev = upstream;

	rtl8214fc_media_set(phydev, false);
}

static const struct sfp_upstream_ops rtl8214fc_sfp_ops = {
	.attach = phy_sfp_attach,
	.detach = phy_sfp_detach,
	.module_insert = rtl8214fc_sfp_insert,
	.module_remove = rtl8214fc_sfp_remove,
};

static int rtl8214fc_phy_probe(struct phy_device *phydev)
{
	int base_addr = phydev->mdio.addr & ~3;
	int ret = 0;

	devm_phy_package_join(&phydev->mdio.dev, phydev, base_addr, 0);
	if (phydev->mdio.addr == base_addr + 3) {
		if (soc_info.family == RTL8380_FAMILY_ID)
			ret = rtl8380_configure_rtl8214fc(get_base_phy(phydev));
		if (ret)
			return ret;
	}

	return phy_sfp_probe(phydev, &rtl8214fc_sfp_ops);
}

static int rtl8214c_phy_probe(struct phy_device *phydev)
{
	int base_addr = phydev->mdio.addr & ~3;

	devm_phy_package_join(&phydev->mdio.dev, phydev, base_addr, 0);
	if (phydev->mdio.addr == base_addr + 3)
		return rtl8380_configure_rtl8214c(get_base_phy(phydev));

	return 0;
}

static int rtl8218b_ext_phy_probe(struct phy_device *phydev)
{
	int base_addr = phydev->mdio.addr & ~7;

	devm_phy_package_join(&phydev->mdio.dev, phydev, base_addr, 0);
	if (phydev->mdio.addr == base_addr + 7) {
		if (soc_info.family == RTL8380_FAMILY_ID)
			return rtl8380_configure_ext_rtl8218b(get_base_phy(phydev));
	}

	return 0;
}

static int rtl8218b_int_phy_probe(struct phy_device *phydev)
{
	int base_addr = phydev->mdio.addr & ~7;

	if (soc_info.family != RTL8380_FAMILY_ID)
		return -ENODEV;
	if (base_addr >= 24)
		return -ENODEV;

	devm_phy_package_join(&phydev->mdio.dev, phydev, base_addr, 0);
	if (phydev->mdio.addr == base_addr + 7)
		return rtl8380_configure_int_rtl8218b(get_base_phy(phydev));

	return 0;
}

static int rtl8218x_phy_probe(struct phy_device *phydev)
{
	int base_addr = phydev->mdio.addr & ~7;

	devm_phy_package_join(&phydev->mdio.dev, phydev, base_addr, 0);

	return 0;
}

static int rtl821x_config_init(struct phy_device *phydev)
{
	/* Disable PHY-mode EEE so LPI is passed to the MAC */
	phy_modify_paged(phydev, RTL821X_PAGE_MAC, RTL821X_PHYCR2,
			 RTL821X_PHYCR2_PHY_EEE_ENABLE, 0);

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

	if (phydev->mdio.addr % 8)
		return 0;
	/*
	 * Realtek provides two ways of initializing the PHY package. Either by U-Boot or via
	 * vendor software and SDK. In case U-Boot setup is missing, run basic configuration
	 * so that ports at least get link up and pass traffic.
	 */

	oldpage = phy_read(phydev, RTL8XXX_PAGE_SELECT);
	oldxpage = phy_read(phydev, RTL821XEXT_MEDIA_PAGE_SELECT);
	phy_write(phydev, RTL821XEXT_MEDIA_PAGE_SELECT, 0x8);

	/* activate 32/40 bit redundancy algorithm for first MAC serdes */
	phy_modify_paged(phydev, RTL821X_MAC_SDS_PAGE(0, 1), 0x14, 0, BIT(3));
	/* magic CMU setting for stable connectivity of first MAC serdes */
	phy_write_paged(phydev, 0x462, 0x15, 0x6e58);
	rtl8218b_cmu_reset(phydev, 0);

	for (int sds = 0; sds < 2; sds++) {
		/* force negative clock edge */
		phy_modify_paged(phydev, RTL821X_MAC_SDS_PAGE(sds, 0), 0x17, 0, BIT(14));
		rtl8218b_cmu_reset(phydev, 5 + sds);
		/* soft reset */
		phy_modify_paged(phydev, RTL821X_MAC_SDS_PAGE(sds, 0), 0x13, 0, BIT(6));
		phy_modify_paged(phydev, RTL821X_MAC_SDS_PAGE(sds, 0), 0x13, BIT(6), 0);
	}

	phy_write(phydev, RTL821XEXT_MEDIA_PAGE_SELECT, oldxpage);
	phy_write(phydev, RTL8XXX_PAGE_SELECT, oldpage);

	return 0;
}

static int rtl838x_serdes_probe(struct phy_device *phydev)
{
	int addr = phydev->mdio.addr;

	if (soc_info.family != RTL8380_FAMILY_ID)
		return -ENODEV;
	if (addr < 24)
		return -ENODEV;

	/* On the RTL8380M, PHYs 24-27 connect to the internal SerDes */
	if (soc_info.id == 0x8380) {
		if (addr == 24)
			return rtl8380_configure_serdes(phydev);
		return 0;
	}

	return -ENODEV;
}

static int rtl8393_serdes_probe(struct phy_device *phydev)
{
	int addr = phydev->mdio.addr;

	pr_info("%s: id: %d\n", __func__, addr);
	if (soc_info.family != RTL8390_FAMILY_ID)
		return -ENODEV;

	if (addr < 24)
		return -ENODEV;

	return rtl8390_configure_serdes(phydev);
}

static int rtl8390_serdes_probe(struct phy_device *phydev)
{
	int addr = phydev->mdio.addr;

	if (soc_info.family != RTL8390_FAMILY_ID)
		return -ENODEV;

	if (addr < 24)
		return -ENODEV;

	return rtl8390_configure_generic(phydev);
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
		.match_phy_device = rtl8214fc_match_phy_device,
		.name		= "Realtek RTL8214FC",
		.config_aneg	= rtl8214fc_config_aneg,
		.config_init	= rtl821x_config_init,
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
		.probe		= rtl8218b_ext_phy_probe,
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
		.probe		= rtl8218b_int_phy_probe,
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
		PHY_ID_MATCH_EXACT(PHY_ID_RTL8218E),
		.name		= "REALTEK RTL8218E",
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
		PHY_ID_MATCH_MODEL(PHY_ID_RTL8218B_I),
		.name		= "Realtek RTL8380 SERDES",
		.features	= PHY_GBIT_FIBRE_FEATURES,
		.probe		= rtl838x_serdes_probe,
		.read_page	= rtl821x_read_page,
		.write_page	= rtl821x_write_page,
		.suspend	= genphy_suspend,
		.resume		= genphy_resume,
		.read_status	= rtl8380_read_status,
	},
	{
		PHY_ID_MATCH_MODEL(PHY_ID_RTL8393_I),
		.name		= "Realtek RTL8393 SERDES",
		.features	= PHY_GBIT_FIBRE_FEATURES,
		.probe		= rtl8393_serdes_probe,
		.read_page	= rtl821x_read_page,
		.write_page	= rtl821x_write_page,
		.suspend	= genphy_suspend,
		.resume		= genphy_resume,
		.read_status	= rtl8393_read_status,
	},
	{
		PHY_ID_MATCH_MODEL(PHY_ID_RTL8390_GENERIC),
		.name		= "Realtek RTL8390 Generic",
		.features	= PHY_GBIT_FIBRE_FEATURES,
		.read_page	= rtl821x_read_page,
		.write_page	= rtl821x_write_page,
		.probe		= rtl8390_serdes_probe,
		.suspend	= genphy_suspend,
		.resume		= genphy_resume,
	},
};

module_phy_driver(rtl83xx_phy_driver);

static struct mdio_device_id __maybe_unused rtl83xx_tbl[] = {
	{ PHY_ID_MATCH_MODEL(PHY_ID_RTL8214_OR_8218) },
	{ }
};

MODULE_DEVICE_TABLE(mdio, rtl83xx_tbl);

MODULE_AUTHOR("B. Koblitz");
MODULE_DESCRIPTION("RTL83xx PHY driver");
MODULE_LICENSE("GPL");

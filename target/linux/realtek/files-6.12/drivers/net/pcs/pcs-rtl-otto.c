// SPDX-License-Identifier: GPL-2.0-or-later

#include <linux/mdio.h>
#include <linux/mfd/syscon.h>
#include <linux/of.h>
#include <linux/of_mdio.h>
#include <linux/of_platform.h>
#include <linux/phy.h>
#include <linux/platform_device.h>
#include <linux/phylink.h>
#include <linux/regmap.h>

#define RTPCS_PORT_CNT				57

#define RTPCS_SPEED_10				0
#define RTPCS_SPEED_100				1
#define RTPCS_SPEED_1000			2
#define RTPCS_SPEED_10000_LEGACY		3
#define RTPCS_SPEED_10000			4
#define RTPCS_SPEED_2500			5
#define RTPCS_SPEED_5000			6

#define RTPCS_838X_CPU_PORT			28
#define RTPCS_838X_MAC_LINK_DUP_STS		0xa19c
#define RTPCS_838X_MAC_LINK_SPD_STS		0xa190
#define RTPCS_838X_MAC_LINK_STS			0xa188
#define RTPCS_838X_MAC_RX_PAUSE_STS		0xa1a4
#define RTPCS_838X_MAC_TX_PAUSE_STS		0xa1a0

#define RTPCS_839X_CPU_PORT			52
#define RTPCS_839X_MAC_LINK_DUP_STS		0x03b0
#define RTPCS_839X_MAC_LINK_SPD_STS		0x03a0
#define RTPCS_839X_MAC_LINK_STS			0x0390
#define RTPCS_839X_MAC_RX_PAUSE_STS		0x03c0
#define RTPCS_839X_MAC_TX_PAUSE_STS		0x03b8

#define RTPCS_83XX_MAC_LINK_SPD_BITS		2

#define RTPCS_930X_CPU_PORT			28
#define RTPCS_930X_MAC_LINK_DUP_STS		0xcb28
#define RTPCS_930X_MAC_LINK_SPD_STS		0xcb18
#define RTPCS_930X_MAC_LINK_STS			0xcb10
#define RTPCS_930X_MAC_RX_PAUSE_STS		0xcb30
#define RTPCS_930X_MAC_TX_PAUSE_STS		0xcb2c

#define RTPCS_931X_CPU_PORT			56
#define RTPCS_931X_MAC_LINK_DUP_STS		0x0ef0
#define RTPCS_931X_MAC_LINK_SPD_STS		0x0ed0
#define RTPCS_931X_MAC_LINK_STS			0x0ec0
#define RTPCS_931X_MAC_RX_PAUSE_STS		0x0f00
#define RTPCS_931X_MAC_TX_PAUSE_STS		0x0ef8

#define RTPCS_93XX_MAC_LINK_SPD_BITS		4

#define RTL93XX_MODEL_NAME_INFO			(0x0004)
#define RTL93XX_CHIP_INFO			(0x0008)

/* Registers of the internal SerDes of the 9310 */
#define RTL931X_SERDES_MODE_CTRL		(0x13cc)
#define RTL931X_PS_SERDES_OFF_MODE_CTRL_ADDR	(0x13F4)
#define RTL931X_MAC_SERDES_MODE_CTRL(sds)	(0x136C + (((sds) << 2)))

struct rtpcs_ctrl {
	struct device *dev;
	struct regmap *map;
	struct mii_bus *bus;
	const struct rtpcs_config *cfg;
	struct rtpcs_link *link[RTPCS_PORT_CNT];
	struct mutex lock;
};

struct rtpcs_link {
	struct rtpcs_ctrl *ctrl;
	struct phylink_pcs pcs;
	int sds;
	int port;
};

struct rtpcs_config {
	int cpu_port;
	int mac_link_dup_sts;
	int mac_link_spd_bits;
	int mac_link_spd_sts;
	int mac_link_sts;
	int mac_rx_pause_sts;
	int mac_tx_pause_sts;
	const struct phylink_pcs_ops *pcs_ops;
	int (*set_autoneg)(struct rtpcs_ctrl *ctrl, int sds, unsigned int neg_mode);
	int (*setup_serdes)(struct rtpcs_ctrl *ctrl, int sds, phy_interface_t mode);
};

typedef struct {
	u8 page;
	u8 reg;
	u16 data;
} sds_config;

static int rtpcs_sds_to_mmd(int sds_page, int sds_regnum)
{
	return (sds_page << 8) + sds_regnum;
}

static int rtpcs_sds_read(struct rtpcs_ctrl *ctrl, int sds, int page, int regnum)
{
	int mmd_regnum = rtpcs_sds_to_mmd(page, regnum);

	return mdiobus_c45_read(ctrl->bus, sds, MDIO_MMD_VEND1, mmd_regnum);
}

static int rtpcs_sds_read_bits(struct rtpcs_ctrl *ctrl, int sds, int page,
			       int regnum, int bithigh, int bitlow)
{
	int mask, val;

	WARN_ON(bithigh < bitlow);

	mask = GENMASK(bithigh, bitlow);
	val = rtpcs_sds_read(ctrl, sds, page, regnum);
	if (val < 0)
		return val;

	return (val & mask) >> bitlow;
}

static int rtpcs_sds_write(struct rtpcs_ctrl *ctrl, int sds, int page, int regnum, u16 value)
{
	int mmd_regnum = rtpcs_sds_to_mmd(page, regnum);

	return mdiobus_c45_write(ctrl->bus, sds, MDIO_MMD_VEND1, mmd_regnum, value);
}

static int rtpcs_sds_write_bits(struct rtpcs_ctrl *ctrl, int sds, int page,
				int regnum, int bithigh, int bitlow, u16 value)
{
	int mask, reg;

	WARN_ON(bithigh < bitlow);

	mask = GENMASK(bithigh, bitlow);
	reg = rtpcs_sds_read(ctrl, sds, page, regnum);
	if (reg < 0)
		return reg;

	reg = (reg & ~mask);
	reg |= (value << bitlow) & mask;

	return rtpcs_sds_write(ctrl, sds, page, regnum, reg);
}

static int rtpcs_sds_modify(struct rtpcs_ctrl *ctrl, int sds, int page, int regnum,
			    u16 mask, u16 set)
{
	int mmd_regnum = rtpcs_sds_to_mmd(page, regnum);

	return mdiobus_c45_modify(ctrl->bus, sds, MDIO_MMD_VEND1, mmd_regnum,
				  mask, set);
}

static int rtpcs_regmap_read_bits(struct rtpcs_ctrl *ctrl, int base, int bithigh, int bitlow)
{
	int offset = base + (bitlow / 32) * 4;
	int bits = bithigh + 1 - bitlow;
	int shift = bitlow % 32;
	int value;

	regmap_read(ctrl->map, offset, &value);
	value = (value >> shift) & (BIT(bits) - 1);

	return value;
}

static struct rtpcs_link *rtpcs_phylink_pcs_to_link(struct phylink_pcs *pcs)
{
	return container_of(pcs, struct rtpcs_link, pcs);
}

/* Variant-specific functions */

/* RTL931X */

static void rtpcs_931x_sds_reset(struct rtpcs_ctrl *ctrl, u32 sds)
{
	u32 o, v, o_mode;
	int shift = ((sds & 0x3) << 3);

	/* TODO: We need to lock this! */

	regmap_read(ctrl->map, RTL931X_PS_SERDES_OFF_MODE_CTRL_ADDR, &o);
	v = o | BIT(sds);
	regmap_write(ctrl->map, RTL931X_PS_SERDES_OFF_MODE_CTRL_ADDR, v);

	regmap_read(ctrl->map, RTL931X_SERDES_MODE_CTRL + 4 * (sds >> 2), &o_mode);
	v = BIT(7) | 0x1F;
	regmap_write_bits(ctrl->map, RTL931X_SERDES_MODE_CTRL + 4 * (sds >> 2),
			  0xff << shift, v << shift);
	regmap_write(ctrl->map, RTL931X_SERDES_MODE_CTRL + 4 * (sds >> 2), o_mode);

	regmap_write(ctrl->map, RTL931X_PS_SERDES_OFF_MODE_CTRL_ADDR, o);
}

static void rtpcs_931x_sds_disable(struct rtpcs_ctrl *ctrl, u32 sds)
{
	regmap_write(ctrl->map, RTL931X_SERDES_MODE_CTRL + (sds >> 2) * 4, 0x9f);
}

static void rtpcs_931x_sds_symerr_clear(struct rtpcs_ctrl *ctrl, u32 sds,
					phy_interface_t mode)
{

	switch (mode) {
	case PHY_INTERFACE_MODE_NA:
		break;
	case PHY_INTERFACE_MODE_XGMII:
		for (int i = 0; i < 4; ++i) {
			rtpcs_sds_write_bits(ctrl, sds, 0x41, 24,  2, 0, i);
			rtpcs_sds_write_bits(ctrl, sds, 0x41,  3, 15, 8, 0x0);
			rtpcs_sds_write_bits(ctrl, sds, 0x41,  2, 15, 0, 0x0);
		}

		for (int i = 0; i < 4; ++i) {
			rtpcs_sds_write_bits(ctrl, sds, 0x81, 24,  2, 0, i);
			rtpcs_sds_write_bits(ctrl, sds, 0x81,  3, 15, 8, 0x0);
			rtpcs_sds_write_bits(ctrl, sds, 0x81,  2, 15, 0, 0x0);
		}

		rtpcs_sds_write_bits(ctrl, sds, 0x41, 0, 15, 0, 0x0);
		rtpcs_sds_write_bits(ctrl, sds, 0x41, 1, 15, 8, 0x0);
		rtpcs_sds_write_bits(ctrl, sds, 0x81, 0, 15, 0, 0x0);
		rtpcs_sds_write_bits(ctrl, sds, 0x81, 1, 15, 8, 0x0);
		break;
	default:
		break;
	}

	return;
}

__attribute__((unused))
static void rtpcs_931x_sds_fiber_disable(struct rtpcs_ctrl *ctrl, u32 sds)
{
	u32 v = 0x3F;

	rtpcs_sds_write_bits(ctrl, sds, 0x1F, 0x9, 11, 6, v);
}

static void rtpcs_931x_sds_fiber_mode_set(struct rtpcs_ctrl *ctrl, u32 sds,
					  phy_interface_t mode)
{
	u32 val;

	/* clear symbol error count before changing mode */
	rtpcs_931x_sds_symerr_clear(ctrl, sds, mode);

	rtpcs_931x_sds_disable(ctrl, sds);

	switch (mode) {
	case PHY_INTERFACE_MODE_SGMII:
		val = 0x5;
		break;

	case PHY_INTERFACE_MODE_1000BASEX:
		/* serdes mode FIBER1G */
		val = 0x9;
		break;

	case PHY_INTERFACE_MODE_10GBASER:
	case PHY_INTERFACE_MODE_10GKR:
		val = 0x35;
		break;
/*	case MII_10GR1000BX_AUTO:
		val = 0x39;
		break; */


	case PHY_INTERFACE_MODE_USXGMII:
		val = 0x1B;
		break;
	default:
		val = 0x25;
	}

	pr_info("%s writing analog SerDes Mode value %02x\n", __func__, val);
	rtpcs_sds_write_bits(ctrl, sds, 0x1F, 0x9, 11, 6, val);

	return;
}

static int rtpcs_931x_sds_cmu_page_get(phy_interface_t mode)
{
	switch (mode) {
	case PHY_INTERFACE_MODE_SGMII:
	case PHY_INTERFACE_MODE_1000BASEX:	/* MII_1000BX_FIBER / 100BX_FIBER / 1000BX100BX_AUTO */
		return 0x24;
	case PHY_INTERFACE_MODE_2500BASEX:	/* MII_2500Base_X: */
		return 0x28;
/*	case MII_HISGMII_5G: */
/*		return 0x2a; */
	case PHY_INTERFACE_MODE_QSGMII:
		return 0x2a;			/* Code also has 0x34 */
	case PHY_INTERFACE_MODE_XAUI:		/* MII_RXAUI_LITE: */
		return 0x2c;
	case PHY_INTERFACE_MODE_XGMII:		/* MII_XSGMII */
	case PHY_INTERFACE_MODE_10GKR:
	case PHY_INTERFACE_MODE_10GBASER:	/* MII_10GR */
		return 0x2e;
	default:
		return -1;
	}

	return -1;
}

static void rtpcs_931x_sds_cmu_type_set(struct rtpcs_ctrl *ctrl, u32 sds,
					phy_interface_t mode, int chiptype)
{
	int cmu_type = 0; /* Clock Management Unit */
	u32 cmu_page = 0;
	u32 frc_cmu_spd;
	u32 evenSds;
	u32 lane, frc_lc_mode_bitnum, frc_lc_mode_val_bitnum;

	switch (mode) {
	case PHY_INTERFACE_MODE_NA:
	case PHY_INTERFACE_MODE_10GKR:
	case PHY_INTERFACE_MODE_XGMII:
	case PHY_INTERFACE_MODE_10GBASER:
	case PHY_INTERFACE_MODE_USXGMII:
		return;

/*	case MII_10GR1000BX_AUTO:
		if (chiptype)
			rtpcs_sds_write_bits(ctrl, sds, 0x24, 0xd, 14, 14, 0);
		return; */

	case PHY_INTERFACE_MODE_QSGMII:
		cmu_type = 1;
		frc_cmu_spd = 0;
		break;

	case PHY_INTERFACE_MODE_1000BASEX:
		cmu_type = 1;
		frc_cmu_spd = 0;
		break;

/*	case MII_1000BX100BX_AUTO:
		cmu_type = 1;
		frc_cmu_spd = 0;
		break; */

	case PHY_INTERFACE_MODE_SGMII:
		cmu_type = 1;
		frc_cmu_spd = 0;
		break;

	case PHY_INTERFACE_MODE_2500BASEX:
		cmu_type = 1;
		frc_cmu_spd = 1;
		break;

	default:
		pr_info("SerDes %d mode is invalid\n", sds);
		return;
	}

	if (cmu_type == 1)
		cmu_page = rtpcs_931x_sds_cmu_page_get(mode);

	lane = sds % 2;

	if (!lane) {
		frc_lc_mode_bitnum = 4;
		frc_lc_mode_val_bitnum = 5;
	} else {
		frc_lc_mode_bitnum = 6;
		frc_lc_mode_val_bitnum = 7;
	}

	evenSds = sds - lane;

	pr_info("%s: cmu_type %0d cmu_page %x frc_cmu_spd %d lane %d sds %d\n",
	        __func__, cmu_type, cmu_page, frc_cmu_spd, lane, sds);

	if (cmu_type == 1) {
		pr_info("%s A CMU page 0x28 0x7 %08x\n", __func__, rtpcs_sds_read(ctrl, sds, 0x28, 0x7));
		rtpcs_sds_write_bits(ctrl, sds, cmu_page, 0x7, 15, 15, 0);
		pr_info("%s B CMU page 0x28 0x7 %08x\n", __func__, rtpcs_sds_read(ctrl, sds, 0x28, 0x7));
		if (chiptype) {
			rtpcs_sds_write_bits(ctrl, sds, cmu_page, 0xd, 14, 14, 0);
		}

		rtpcs_sds_write_bits(ctrl, evenSds, 0x20, 0x12, 3, 2, 0x3);
		rtpcs_sds_write_bits(ctrl, evenSds, 0x20, 0x12, frc_lc_mode_bitnum, frc_lc_mode_bitnum, 1);
		rtpcs_sds_write_bits(ctrl, evenSds, 0x20, 0x12, frc_lc_mode_val_bitnum, frc_lc_mode_val_bitnum, 0);
		rtpcs_sds_write_bits(ctrl, evenSds, 0x20, 0x12, 12, 12, 1);
		rtpcs_sds_write_bits(ctrl, evenSds, 0x20, 0x12, 15, 13, frc_cmu_spd);
	}

	pr_info("%s CMU page 0x28 0x7 %08x\n", __func__, rtpcs_sds_read(ctrl, sds, 0x28, 0x7));
	return;
}

static void rtpcs_931x_sds_rx_reset(struct rtpcs_ctrl *ctrl, u32 sds)
{
	if (sds < 2)
		return;

	rtpcs_sds_write(ctrl, sds, 0x2e, 0x12, 0x2740);
	rtpcs_sds_write(ctrl, sds, 0x2f, 0x0, 0x0);
	rtpcs_sds_write(ctrl, sds, 0x2f, 0x2, 0x2010);
	rtpcs_sds_write(ctrl, sds, 0x20, 0x0, 0xc10);

	rtpcs_sds_write(ctrl, sds, 0x2e, 0x12, 0x27c0);
	rtpcs_sds_write(ctrl, sds, 0x2f, 0x0, 0xc000);
	rtpcs_sds_write(ctrl, sds, 0x2f, 0x2, 0x6010);
	rtpcs_sds_write(ctrl, sds, 0x20, 0x0, 0xc30);

	mdelay(50);
}

static void rtpcs_931x_sds_mii_mode_set(struct rtpcs_ctrl *ctrl, u32 sds,
					phy_interface_t mode)
{
	u32 val;

	switch (mode) {
	case PHY_INTERFACE_MODE_QSGMII:
		val = 0x6;
		break;
	case PHY_INTERFACE_MODE_XGMII:
		val = 0x10; /* serdes mode XSGMII */
		break;
	case PHY_INTERFACE_MODE_USXGMII:
	case PHY_INTERFACE_MODE_2500BASEX:
		val = 0xD;
		break;
	case PHY_INTERFACE_MODE_SGMII:
		val = 0x2;
		break;
	default:
		return;
	}

	val |= (1 << 7);

	regmap_write(ctrl->map, RTL931X_SERDES_MODE_CTRL + 4 * (sds >> 2), val);
}

static int rtpcs_931x_sds_cmu_band_set(struct rtpcs_ctrl *ctrl, int sds,
				       bool enable, u32 band,
				       phy_interface_t mode)
{
	int page = rtpcs_931x_sds_cmu_page_get(mode);

	sds -= (sds % 2);
	sds = sds & ~1;
	page += 1;

	if (enable) {
		rtpcs_sds_write_bits(ctrl, sds, page, 0x7, 13, 13, 0);
		rtpcs_sds_write_bits(ctrl, sds, page, 0x7, 11, 11, 0);
	} else {
		rtpcs_sds_write_bits(ctrl, sds, page, 0x7, 13, 13, 0);
		rtpcs_sds_write_bits(ctrl, sds, page, 0x7, 11, 11, 0);
	}

	rtpcs_sds_write_bits(ctrl, sds, page, 0x7, 4, 0, band);

	rtpcs_931x_sds_reset(ctrl, sds);

	return 0;
}

static int rtpcs_931x_sds_cmu_band_get(struct rtpcs_ctrl *ctrl, int sds,
				       phy_interface_t mode)
{
	int page = rtpcs_931x_sds_cmu_page_get(mode);
	u32 band;

	sds -= (sds % 2);
	page += 1;
	rtpcs_sds_write(ctrl, sds, 0x1f, 0x02, 73);

	rtpcs_sds_write_bits(ctrl, sds, page, 0x5, 15, 15, 1);
	band = rtpcs_sds_read_bits(ctrl, sds, 0x1f, 0x15, 8, 3);
	pr_info("%s band is: %d\n", __func__, band);

	return band;
}

__attribute__((unused))
static int rtpcs_931x_sds_link_sts_get(struct rtpcs_ctrl *ctrl, u32 sds)
{
	u32 sts, sts1, latch_sts, latch_sts1;
	if (0){
		sts = rtpcs_sds_read_bits(ctrl, sds, 0x41, 29, 8, 0);
		sts1 = rtpcs_sds_read_bits(ctrl, sds, 0x81, 29, 8, 0);
		latch_sts = rtpcs_sds_read_bits(ctrl, sds, 0x41, 30, 8, 0);
		latch_sts1 = rtpcs_sds_read_bits(ctrl, sds, 0x81, 30, 8, 0);
	} else {
		sts = rtpcs_sds_read_bits(ctrl, sds, 0x5, 0, 12, 12);
		latch_sts = rtpcs_sds_read_bits(ctrl, sds, 0x4, 1, 2, 2);
		latch_sts1 = rtpcs_sds_read_bits(ctrl, sds, 0x42, 1, 2, 2);
		sts1 = rtpcs_sds_read_bits(ctrl, sds, 0x42, 1, 2, 2);
	}

	pr_info("%s: serdes %d sts %d, sts1 %d, latch_sts %d, latch_sts1 %d\n", __func__,
		sds, sts, sts1, latch_sts, latch_sts1);

	return sts1;
}

static sds_config sds_config_10p3125g_type1[] = {
	{ 0x2E, 0x00, 0x0107 }, { 0x2E, 0x01, 0x01A3 }, { 0x2E, 0x02, 0x6A24 },
	{ 0x2E, 0x03, 0xD10D }, { 0x2E, 0x04, 0x8000 }, { 0x2E, 0x05, 0xA17E },
	{ 0x2E, 0x06, 0xE31D }, { 0x2E, 0x07, 0x800E }, { 0x2E, 0x08, 0x0294 },
	{ 0x2E, 0x09, 0x0CE4 }, { 0x2E, 0x0A, 0x7FC8 }, { 0x2E, 0x0B, 0xE0E7 },
	{ 0x2E, 0x0C, 0x0200 }, { 0x2E, 0x0D, 0xDF80 }, { 0x2E, 0x0E, 0x0000 },
	{ 0x2E, 0x0F, 0x1FC2 }, { 0x2E, 0x10, 0x0C3F }, { 0x2E, 0x11, 0x0000 },
	{ 0x2E, 0x12, 0x27C0 }, { 0x2E, 0x13, 0x7E1D }, { 0x2E, 0x14, 0x1300 },
	{ 0x2E, 0x15, 0x003F }, { 0x2E, 0x16, 0xBE7F }, { 0x2E, 0x17, 0x0090 },
	{ 0x2E, 0x18, 0x0000 }, { 0x2E, 0x19, 0x4000 }, { 0x2E, 0x1A, 0x0000 },
	{ 0x2E, 0x1B, 0x8000 }, { 0x2E, 0x1C, 0x011F }, { 0x2E, 0x1D, 0x0000 },
	{ 0x2E, 0x1E, 0xC8FF }, { 0x2E, 0x1F, 0x0000 }, { 0x2F, 0x00, 0xC000 },
	{ 0x2F, 0x01, 0xF000 }, { 0x2F, 0x02, 0x6010 }, { 0x2F, 0x12, 0x0EE7 },
	{ 0x2F, 0x13, 0x0000 }
};

static sds_config sds_config_10p3125g_cmu_type1[] = {
	{ 0x2F, 0x03, 0x4210 }, { 0x2F, 0x04, 0x0000 }, { 0x2F, 0x05, 0x0019 },
	{ 0x2F, 0x06, 0x18A6 }, { 0x2F, 0x07, 0x2990 }, { 0x2F, 0x08, 0xFFF4 },
	{ 0x2F, 0x09, 0x1F08 }, { 0x2F, 0x0A, 0x0000 }, { 0x2F, 0x0B, 0x8000 },
	{ 0x2F, 0x0C, 0x4224 }, { 0x2F, 0x0D, 0x0000 }, { 0x2F, 0x0E, 0x0000 },
	{ 0x2F, 0x0F, 0xA470 }, { 0x2F, 0x10, 0x8000 }, { 0x2F, 0x11, 0x037B }
};

__attribute__((unused))
static int rtpcs_931x_setup_serdes(struct rtpcs_ctrl *ctrl, int sds,
				   phy_interface_t mode)
{
	u32 board_sds_tx_type1[] = {
		0x01c3, 0x01c3, 0x01c3, 0x01a3, 0x01a3, 0x01a3,
		0x0143, 0x0143, 0x0143, 0x0143, 0x0163, 0x0163,
	};
	u32 board_sds_tx[] = {
		0x1a00, 0x1a00, 0x0200, 0x0200, 0x0200, 0x0200,
		0x01a3, 0x01a3, 0x01a3, 0x01a3, 0x01e3, 0x01e3
	};
	u32 board_sds_tx2[] = {
		0x0dc0, 0x01c0, 0x0200, 0x0180, 0x0160, 0x0123,
		0x0123, 0x0163, 0x01a3, 0x01a0, 0x01c3, 0x09c3,
	};
	u32 band, ori, model_info, val;
	int chiptype = 0;

	if (sds < 0 || sds > 13)
		return -EINVAL;

	/*
	 * TODO: USXGMII is currently the swiss army knife to declare 10G
	 * multi port PHYs. Real devices use other modes instead. Especially
	 *
	 * - RTL8224 is driven in 10G_QXGMII
	 * - RTL8218D/E are driven in (Realtek proprietary) XSGMII (10G SGMII)
	 *
	 * For now disable all USXGMII SerDes handling and rely on U-Boot setup.
	 */
	if (mode == PHY_INTERFACE_MODE_USXGMII)
		return -ENOTSUPP;

	pr_info("%s: set sds %d to mode %d\n", __func__, sds, mode);
	val = rtpcs_sds_read_bits(ctrl, sds, 0x1F, 0x9, 11, 6);

	pr_info("%s: fibermode %08X stored mode 0x%x", __func__,
			rtpcs_sds_read(ctrl, sds, 0x1f, 0x9), val);
	pr_info("%s: SGMII mode %08X in 0x24 0x9", __func__,
			rtpcs_sds_read(ctrl, sds, 0x24, 0x9));
	pr_info("%s: CMU mode %08X stored even SDS %d", __func__,
			rtpcs_sds_read(ctrl, sds & ~1, 0x20, 0x12), sds & ~1);
	pr_info("%s: serdes_mode_ctrl %08X", __func__,  RTL931X_SERDES_MODE_CTRL + 4 * (sds >> 2));
	pr_info("%s CMU page 0x24 0x7 %08x\n", __func__, rtpcs_sds_read(ctrl, sds, 0x24, 0x7));
	pr_info("%s CMU page 0x26 0x7 %08x\n", __func__, rtpcs_sds_read(ctrl, sds, 0x26, 0x7));
	pr_info("%s CMU page 0x28 0x7 %08x\n", __func__, rtpcs_sds_read(ctrl, sds, 0x28, 0x7));
	pr_info("%s XSG page 0x0 0xe %08x\n", __func__, rtpcs_sds_read(ctrl, sds, 0x40, 0xe));
	pr_info("%s XSG2 page 0x0 0xe %08x\n", __func__, rtpcs_sds_read(ctrl, sds, 0x80, 0xe));

	regmap_read(ctrl->map, RTL93XX_MODEL_NAME_INFO, &model_info);
	if ((model_info >> 4) & 0x1) {
		pr_info("detected chiptype 1\n");
		chiptype = 1;
	} else {
		pr_info("detected chiptype 0\n");
	}

	pr_info("%s: 2.5gbit %08X", __func__,
	        rtpcs_sds_read(ctrl, sds, 0x41, 0x14));

	regmap_read(ctrl->map, RTL931X_PS_SERDES_OFF_MODE_CTRL_ADDR, &ori);
	pr_info("%s: RTL931X_PS_SERDES_OFF_MODE_CTRL_ADDR 0x%08X\n", __func__, ori);
	val = ori | (1 << sds);
	regmap_write(ctrl->map, RTL931X_PS_SERDES_OFF_MODE_CTRL_ADDR, val);

	/* this was in rtl931x_phylink_mac_config in dsa/rtl83xx/dsa.c before */
	band = rtpcs_931x_sds_cmu_band_get(ctrl, sds, mode);

	switch (mode) {
	case PHY_INTERFACE_MODE_NA:
		break;

	case PHY_INTERFACE_MODE_XGMII: /* MII_XSGMII */

		if (chiptype) {
			/* fifo inv clk */
			rtpcs_sds_write_bits(ctrl, sds, 0x41, 0x1, 7, 4, 0xf);
			rtpcs_sds_write_bits(ctrl, sds, 0x41, 0x1, 3, 0, 0xf);

			rtpcs_sds_write_bits(ctrl, sds, 0x81, 0x1, 7, 4, 0xf);
			rtpcs_sds_write_bits(ctrl, sds, 0x81, 0x1, 3, 0, 0xf);

		}

		rtpcs_sds_write_bits(ctrl, sds, 0x40, 0xE, 12, 12, 1);
		rtpcs_sds_write_bits(ctrl, sds, 0x80, 0xE, 12, 12, 1);
		break;

	case PHY_INTERFACE_MODE_USXGMII: /* MII_USXGMII_10GSXGMII/10GDXGMII/10GQXGMII: */
		u32 op_code = 0x6003;
		u32 evenSds;

		if (chiptype) {
			rtpcs_sds_write_bits(ctrl, sds, 0x6, 0x2, 12, 12, 1);

			for (int i = 0; i < sizeof(sds_config_10p3125g_type1) / sizeof(sds_config); ++i) {
				rtpcs_sds_write(ctrl, sds,
						sds_config_10p3125g_type1[i].page - 0x4,
						sds_config_10p3125g_type1[i].reg,
						sds_config_10p3125g_type1[i].data);
			}

			evenSds = sds & ~1;

			for (int i = 0; i < sizeof(sds_config_10p3125g_cmu_type1) / sizeof(sds_config); ++i) {
				rtpcs_sds_write(ctrl, evenSds,
						sds_config_10p3125g_cmu_type1[i].page - 0x4,
						sds_config_10p3125g_cmu_type1[i].reg,
						sds_config_10p3125g_cmu_type1[i].data);
			}

			rtpcs_sds_write_bits(ctrl, sds, 0x6, 0x2, 12, 12, 0);
		} else {

			rtpcs_sds_write_bits(ctrl, sds, 0x2e, 0xd, 6, 0, 0x0);
			rtpcs_sds_write_bits(ctrl, sds, 0x2e, 0xd, 7, 7, 0x1);

			rtpcs_sds_write_bits(ctrl, sds, 0x2e, 0x1c, 5, 0, 0x1E);
			rtpcs_sds_write_bits(ctrl, sds, 0x2e, 0x1d, 11, 0, 0x00);
			rtpcs_sds_write_bits(ctrl, sds, 0x2e, 0x1f, 11, 0, 0x00);
			rtpcs_sds_write_bits(ctrl, sds, 0x2f, 0x0, 11, 0, 0x00);
			rtpcs_sds_write_bits(ctrl, sds, 0x2f, 0x1, 11, 0, 0x00);

			rtpcs_sds_write_bits(ctrl, sds, 0x2e, 0xf, 12, 6, 0x7F);
			rtpcs_sds_write(ctrl, sds, 0x2f, 0x12, 0xaaa);

			rtpcs_931x_sds_rx_reset(ctrl, sds);

			rtpcs_sds_write(ctrl, sds, 0x7, 0x10, op_code);
			rtpcs_sds_write(ctrl, sds, 0x6, 0x1d, 0x0480);
			rtpcs_sds_write(ctrl, sds, 0x6, 0xe, 0x0400);
		}
		break;

	case PHY_INTERFACE_MODE_10GBASER: /* MII_10GR / MII_10GR1000BX_AUTO: */
	                                  /* configure 10GR fiber mode=1 */
		rtpcs_sds_write_bits(ctrl, sds, 0x1f, 0xb, 1, 1, 1);

		/* init fiber_1g */
		rtpcs_sds_write_bits(ctrl, sds, 0x43, 0x13, 15, 14, 0);

		rtpcs_sds_write_bits(ctrl, sds, 0x42, 0x0, 12, 12, 1);
		rtpcs_sds_write_bits(ctrl, sds, 0x42, 0x0, 6, 6, 1);
		rtpcs_sds_write_bits(ctrl, sds, 0x42, 0x0, 13, 13, 0);

		/* init auto */
		rtpcs_sds_write_bits(ctrl, sds, 0x1f, 13, 15, 0, 0x109e);
		rtpcs_sds_write_bits(ctrl, sds, 0x1f, 0x6, 14, 10, 0x8);
		rtpcs_sds_write_bits(ctrl, sds, 0x1f, 0x7, 10, 4, 0x7f);
		break;

	case PHY_INTERFACE_MODE_1000BASEX: /* MII_1000BX_FIBER */
		rtpcs_sds_write_bits(ctrl, sds, 0x43, 0x13, 15, 14, 0);

		rtpcs_sds_write_bits(ctrl, sds, 0x42, 0x0, 12, 12, 1);
		rtpcs_sds_write_bits(ctrl, sds, 0x42, 0x0, 6, 6, 1);
		rtpcs_sds_write_bits(ctrl, sds, 0x42, 0x0, 13, 13, 0);
		break;

	case PHY_INTERFACE_MODE_SGMII:
		rtpcs_sds_write_bits(ctrl, sds, 0x24, 0x9, 15, 15, 0);

		/* this was in rtl931x_phylink_mac_config in dsa/rtl83xx/dsa.c before */
		band = rtpcs_931x_sds_cmu_band_set(ctrl, sds, true, 62, PHY_INTERFACE_MODE_SGMII);
		break;

	case PHY_INTERFACE_MODE_2500BASEX:
		rtpcs_sds_write_bits(ctrl, sds, 0x41, 0x14, 8, 8, 1);
		break;

	case PHY_INTERFACE_MODE_QSGMII:
	default:
		pr_info("%s: PHY mode %s not supported by SerDes %d\n",
		        __func__, phy_modes(mode), sds);
		return -ENOTSUPP;
	}

	rtpcs_931x_sds_cmu_type_set(ctrl, sds, mode, chiptype);

	if (sds >= 2 && sds <= 13) {
		if (chiptype)
			rtpcs_sds_write(ctrl, sds, 0x2E, 0x1, board_sds_tx_type1[sds - 2]);
		else {
			val = 0xa0000;
			regmap_write(ctrl->map, RTL93XX_CHIP_INFO, val);
			regmap_read(ctrl->map, RTL93XX_CHIP_INFO, &val);
			if (val & BIT(28)) /* consider 9311 etc. RTL9313_CHIP_ID == HWP_CHIP_ID(unit)) */
			{
				rtpcs_sds_write(ctrl, sds, 0x2E, 0x1, board_sds_tx2[sds - 2]);
			} else {
				rtpcs_sds_write(ctrl, sds, 0x2E, 0x1, board_sds_tx[sds - 2]);
			}
			val = 0;
			regmap_write(ctrl->map, RTL93XX_CHIP_INFO, val);
		}
	}

	val = ori & ~BIT(sds);
	regmap_write(ctrl->map, RTL931X_PS_SERDES_OFF_MODE_CTRL_ADDR, val);
	regmap_read(ctrl->map, RTL931X_PS_SERDES_OFF_MODE_CTRL_ADDR, &val);
	pr_debug("%s: RTL931X_PS_SERDES_OFF_MODE_CTRL_ADDR 0x%08X\n", __func__, val);

	if (mode == PHY_INTERFACE_MODE_XGMII ||
	    mode == PHY_INTERFACE_MODE_QSGMII ||
	    mode == PHY_INTERFACE_MODE_SGMII ||
	    mode == PHY_INTERFACE_MODE_USXGMII) {
		if (mode == PHY_INTERFACE_MODE_XGMII)
			rtpcs_931x_sds_mii_mode_set(ctrl, sds, mode);
		else
			rtpcs_931x_sds_fiber_mode_set(ctrl, sds, mode);
	}

	return 0;
}

/* Common functions */

static void rtpcs_pcs_get_state(struct phylink_pcs *pcs, struct phylink_link_state *state)
{
	struct rtpcs_link *link = rtpcs_phylink_pcs_to_link(pcs);
	struct rtpcs_ctrl *ctrl = link->ctrl;
	int port = link->port;
	int linkup, speed;

	state->link = 0;
	state->speed = SPEED_UNKNOWN;
	state->duplex = DUPLEX_UNKNOWN;
	state->pause &= ~(MLO_PAUSE_RX | MLO_PAUSE_TX);

	/* Read MAC side link twice */
	for (int i = 0; i < 2; i++)
		linkup = rtpcs_regmap_read_bits(ctrl, ctrl->cfg->mac_link_sts, port, port);

	if (!linkup)
		return;

	state->link = 1;
	state->duplex = rtpcs_regmap_read_bits(ctrl, ctrl->cfg->mac_link_dup_sts, port, port);

	speed = rtpcs_regmap_read_bits(ctrl, ctrl->cfg->mac_link_spd_sts,
				       ctrl->cfg->mac_link_spd_bits * (port + 1) - 1,
				       ctrl->cfg->mac_link_spd_bits * port);
	switch (speed) {
	case RTPCS_SPEED_10:
		state->speed = SPEED_10;
		break;
	case RTPCS_SPEED_100:
		state->speed = SPEED_100;
		break;
	case RTPCS_SPEED_1000:
		state->speed = SPEED_1000;
		break;
	case RTPCS_SPEED_10000:
	case RTPCS_SPEED_10000_LEGACY:
		/*
		 * The legacy mode is ok so far with minor inconsistencies. On RTL838x this flag
		 * is either 500M or 2G. It might be that MAC_GLITE_STS register tells more. On
		 * RTL839x this is either 500M or 10G. More info might be in MAC_LINK_500M_STS.
		 * Without support for the 500M modes simply resolve to 10G.
		 */
		state->speed = SPEED_10000;
		break;
	case RTPCS_SPEED_2500:
		state->speed = SPEED_2500;
		break;
	case RTPCS_SPEED_5000:
		state->speed = SPEED_5000;
		break;
	default:
		dev_err(ctrl->dev, "unknown speed %d\n", speed);
	}

	if (rtpcs_regmap_read_bits(ctrl, ctrl->cfg->mac_rx_pause_sts, port, port))
		state->pause |= MLO_PAUSE_RX;
	if (rtpcs_regmap_read_bits(ctrl, ctrl->cfg->mac_tx_pause_sts, port, port))
		state->pause |= MLO_PAUSE_TX;
}

static void rtpcs_pcs_an_restart(struct phylink_pcs *pcs)
{
	struct rtpcs_link *link = rtpcs_phylink_pcs_to_link(pcs);
	struct rtpcs_ctrl *ctrl = link->ctrl;

	dev_warn(ctrl->dev, "an_restart() for port %d and sds %d not yet implemented\n",
		 link->port, link->sds);
}

static int rtpcs_pcs_config(struct phylink_pcs *pcs, unsigned int neg_mode,
			    phy_interface_t interface, const unsigned long *advertising,
			    bool permit_pause_to_mac)
{
	struct rtpcs_link *link = rtpcs_phylink_pcs_to_link(pcs);
	struct rtpcs_ctrl *ctrl = link->ctrl;
	int ret = 0;

	if (link->sds < 0)
		return 0;

	/*
	 * TODO: This (or copies of this) will be the central function for configuring the
	 * link between PHY and SerDes. As of now a lot of the code is scattered throughout
	 * all the other Realtek drivers. Maybe some day this will live up to the expectations.
	 */

	dev_warn(ctrl->dev, "pcs_config(%s) for port %d and sds %d not yet fully implemented\n",
		 phy_modes(interface), link->port, link->sds);

	mutex_lock(&ctrl->lock);

	if (ctrl->cfg->setup_serdes) {
		ret = ctrl->cfg->setup_serdes(ctrl, link->sds, interface);
		if (ret < 0)
			goto out;
	}

	if (ctrl->cfg->set_autoneg) {
		ret = ctrl->cfg->set_autoneg(ctrl, link->sds, neg_mode);
		if (ret < 0)
			goto out;
	}

out:
	mutex_unlock(&ctrl->lock);

	return ret;
}

struct phylink_pcs *rtpcs_create(struct device *dev, struct device_node *np, int port);
struct phylink_pcs *rtpcs_create(struct device *dev, struct device_node *np, int port)
{
	struct platform_device *pdev;
	struct device_node *pcs_np;
	struct rtpcs_ctrl *ctrl;
	struct rtpcs_link *link;
	int sds;

	/*
	 * RTL838x devices have a built-in octa port RTL8218B PHY that is not attached via
	 * a SerDes. Allow to be called with an empty SerDes device node. In this case lookup
	 * the parent/driver node directly.
	 */
	if (np) {
		if (!of_device_is_available(np))
			return ERR_PTR(-ENODEV);

		if (of_property_read_u32(np, "reg", &sds))
			return ERR_PTR(-EINVAL);

		pcs_np = of_get_parent(np);
	} else {
		pcs_np = of_find_compatible_node(NULL, NULL, "realtek,otto-pcs");
		sds = -1;
	}

	if (!pcs_np)
		return ERR_PTR(-ENODEV);

	if (!of_device_is_available(pcs_np)) {
		of_node_put(pcs_np);
		return ERR_PTR(-ENODEV);
	}

	pdev = of_find_device_by_node(pcs_np);
	of_node_put(pcs_np);
	if (!pdev)
		return ERR_PTR(-EPROBE_DEFER);

	ctrl = platform_get_drvdata(pdev);
	if (!ctrl) {
		put_device(&pdev->dev);
		return ERR_PTR(-EPROBE_DEFER);
	}

	if (port < 0 || port > ctrl->cfg->cpu_port)
		return ERR_PTR(-EINVAL);

	if (sds !=-1 && rtpcs_sds_read(ctrl, sds, 0 , 0) < 0)
		return ERR_PTR(-EINVAL);

	link = kzalloc(sizeof(*link), GFP_KERNEL);
	if (!link) {
		put_device(&pdev->dev);
		return ERR_PTR(-ENOMEM);
	}

	device_link_add(dev, ctrl->dev, DL_FLAG_AUTOREMOVE_CONSUMER);

	link->ctrl = ctrl;
	link->port = port;
	link->sds = sds;
	link->pcs.ops = ctrl->cfg->pcs_ops;
	link->pcs.neg_mode = true;

	ctrl->link[port] = link;

	dev_dbg(ctrl->dev, "phylink_pcs created, port %d, sds %d\n", port, sds);

	return &link->pcs;
}
EXPORT_SYMBOL(rtpcs_create);

static struct mii_bus *rtpcs_probe_serdes_bus(struct rtpcs_ctrl *ctrl)
{
	struct device_node *np;
	struct mii_bus *bus;

	np = of_find_compatible_node(NULL, NULL, "realtek,otto-serdes-mdio");
	if (!np) {
		dev_err(ctrl->dev, "SerDes mdio bus not found in DT");
		return ERR_PTR(-ENODEV);
	}

	bus = of_mdio_find_bus(np);
	of_node_put(np);
	if (!bus) {
		dev_warn(ctrl->dev, "SerDes mdio bus not (yet) active");
		return ERR_PTR(-EPROBE_DEFER);
	}

	if (!of_device_is_available(np)) {
		dev_err(ctrl->dev, "SerDes mdio bus not usable");
		return ERR_PTR(-ENODEV);
	}

	return bus;
}

static int rtpcs_probe(struct platform_device *pdev)
{
	struct device_node *np = pdev->dev.of_node;
	struct device *dev = &pdev->dev;
	struct rtpcs_ctrl *ctrl;

	ctrl = devm_kzalloc(dev, sizeof(*ctrl), GFP_KERNEL);
	if (!ctrl)
		return -ENOMEM;

	mutex_init(&ctrl->lock);

	ctrl->dev = dev;
	ctrl->cfg = (const struct rtpcs_config *)device_get_match_data(ctrl->dev);
	ctrl->map = syscon_node_to_regmap(np->parent);
	if (IS_ERR(ctrl->map))
		return PTR_ERR(ctrl->map);

	ctrl->bus = rtpcs_probe_serdes_bus(ctrl);
	if (IS_ERR(ctrl->bus))
		return PTR_ERR(ctrl->bus);
	/*
	 * rtpcs_create() relies on that fact that data is attached to the platform device to
	 * determine if the driver is ready. Do this after everything is initialized properly.
	 */
	platform_set_drvdata(pdev, ctrl);

	dev_info(dev, "Realtek PCS driver initialized\n");

	return 0;
}

static int rtpcs_93xx_set_autoneg(struct rtpcs_ctrl *ctrl, int sds,
				  unsigned int neg_mode)
{
	u16 bmcr = neg_mode == PHYLINK_PCS_NEG_INBAND_ENABLED ? BMCR_ANENABLE : 0;

	return rtpcs_sds_modify(ctrl, sds, 2, MII_BMCR, BMCR_ANENABLE, bmcr);
}

static const struct phylink_pcs_ops rtpcs_838x_pcs_ops = {
	.pcs_an_restart		= rtpcs_pcs_an_restart,
	.pcs_config		= rtpcs_pcs_config,
	.pcs_get_state		= rtpcs_pcs_get_state,
};

static const struct rtpcs_config rtpcs_838x_cfg = {
	.cpu_port		= RTPCS_838X_CPU_PORT,
	.mac_link_dup_sts	= RTPCS_838X_MAC_LINK_DUP_STS,
	.mac_link_spd_sts	= RTPCS_838X_MAC_LINK_SPD_STS,
	.mac_link_spd_bits	= RTPCS_83XX_MAC_LINK_SPD_BITS,
	.mac_link_sts		= RTPCS_838X_MAC_LINK_STS,
	.mac_rx_pause_sts	= RTPCS_838X_MAC_RX_PAUSE_STS,
	.mac_tx_pause_sts	= RTPCS_838X_MAC_TX_PAUSE_STS,
	.pcs_ops		= &rtpcs_838x_pcs_ops,
};

static const struct phylink_pcs_ops rtpcs_839x_pcs_ops = {
	.pcs_an_restart		= rtpcs_pcs_an_restart,
	.pcs_config		= rtpcs_pcs_config,
	.pcs_get_state		= rtpcs_pcs_get_state,
};

static const struct rtpcs_config rtpcs_839x_cfg = {
	.cpu_port		= RTPCS_839X_CPU_PORT,
	.mac_link_dup_sts	= RTPCS_839X_MAC_LINK_DUP_STS,
	.mac_link_spd_sts	= RTPCS_839X_MAC_LINK_SPD_STS,
	.mac_link_spd_bits	= RTPCS_83XX_MAC_LINK_SPD_BITS,
	.mac_link_sts		= RTPCS_839X_MAC_LINK_STS,
	.mac_rx_pause_sts	= RTPCS_839X_MAC_RX_PAUSE_STS,
	.mac_tx_pause_sts	= RTPCS_839X_MAC_TX_PAUSE_STS,
	.pcs_ops		= &rtpcs_839x_pcs_ops,
};

static const struct phylink_pcs_ops rtpcs_930x_pcs_ops = {
	.pcs_an_restart		= rtpcs_pcs_an_restart,
	.pcs_config		= rtpcs_pcs_config,
	.pcs_get_state		= rtpcs_pcs_get_state,
};

static const struct rtpcs_config rtpcs_930x_cfg = {
	.cpu_port		= RTPCS_930X_CPU_PORT,
	.mac_link_dup_sts	= RTPCS_930X_MAC_LINK_DUP_STS,
	.mac_link_spd_sts	= RTPCS_930X_MAC_LINK_SPD_STS,
	.mac_link_spd_bits	= RTPCS_93XX_MAC_LINK_SPD_BITS,
	.mac_link_sts		= RTPCS_930X_MAC_LINK_STS,
	.mac_rx_pause_sts	= RTPCS_930X_MAC_RX_PAUSE_STS,
	.mac_tx_pause_sts	= RTPCS_930X_MAC_TX_PAUSE_STS,
	.pcs_ops		= &rtpcs_930x_pcs_ops,
	.set_autoneg		= rtpcs_93xx_set_autoneg,
};

static const struct phylink_pcs_ops rtpcs_931x_pcs_ops = {
	.pcs_an_restart		= rtpcs_pcs_an_restart,
	.pcs_config		= rtpcs_pcs_config,
	.pcs_get_state		= rtpcs_pcs_get_state,
};

static const struct rtpcs_config rtpcs_931x_cfg = {
	.cpu_port		= RTPCS_931X_CPU_PORT,
	.mac_link_dup_sts	= RTPCS_931X_MAC_LINK_DUP_STS,
	.mac_link_spd_sts	= RTPCS_931X_MAC_LINK_SPD_STS,
	.mac_link_spd_bits	= RTPCS_93XX_MAC_LINK_SPD_BITS,
	.mac_link_sts		= RTPCS_931X_MAC_LINK_STS,
	.mac_rx_pause_sts	= RTPCS_931X_MAC_RX_PAUSE_STS,
	.mac_tx_pause_sts	= RTPCS_931X_MAC_TX_PAUSE_STS,
	.pcs_ops		= &rtpcs_931x_pcs_ops,
	.set_autoneg		= rtpcs_93xx_set_autoneg,
	.setup_serdes		= rtpcs_931x_setup_serdes,
};

static const struct of_device_id rtpcs_of_match[] = {
	{
		.compatible = "realtek,rtl8380-pcs",
		.data = &rtpcs_838x_cfg,
	},
	{
		.compatible = "realtek,rtl8392-pcs",
		.data = &rtpcs_839x_cfg,
	},
	{
		.compatible = "realtek,rtl9301-pcs",
		.data = &rtpcs_930x_cfg,
	},
	{
		.compatible = "realtek,rtl9311-pcs",
		.data = &rtpcs_931x_cfg,
	},
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, rtpcs_of_match);

static struct platform_driver rtpcs_driver = {
	.driver = {
		.name = "realtek-otto-pcs",
		.of_match_table = rtpcs_of_match
	},
	.probe = rtpcs_probe,
};
module_platform_driver(rtpcs_driver);

MODULE_AUTHOR("Markus Stockhausen <markus.stockhausen@gmx.de>");
MODULE_DESCRIPTION("Realtek Otto SerDes PCS driver");
MODULE_LICENSE("GPL v2");

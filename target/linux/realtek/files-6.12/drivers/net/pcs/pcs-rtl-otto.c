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

#define RTPCS_SDS_CNT				14
#define RTPCS_PORT_CNT				57

#define RTPCS_SPEED_10				0
#define RTPCS_SPEED_100				1
#define RTPCS_SPEED_1000			2
#define RTPCS_SPEED_10000_LEGACY		3
#define RTPCS_SPEED_10000			4
#define RTPCS_SPEED_2500			5
#define RTPCS_SPEED_5000			6

#define RTPCS_838X_CPU_PORT			28
#define RTPCS_838X_SERDES_CNT			6
#define RTPCS_838X_MAC_LINK_DUP_STS		0xa19c
#define RTPCS_838X_MAC_LINK_SPD_STS		0xa190
#define RTPCS_838X_MAC_LINK_STS			0xa188
#define RTPCS_838X_MAC_RX_PAUSE_STS		0xa1a4
#define RTPCS_838X_MAC_TX_PAUSE_STS		0xa1a0

#define RTPCS_839X_CPU_PORT			52
#define RTPCS_839X_SERDES_CNT			14
#define RTPCS_839X_MAC_LINK_DUP_STS		0x03b0
#define RTPCS_839X_MAC_LINK_SPD_STS		0x03a0
#define RTPCS_839X_MAC_LINK_STS			0x0390
#define RTPCS_839X_MAC_RX_PAUSE_STS		0x03c0
#define RTPCS_839X_MAC_TX_PAUSE_STS		0x03b8

#define RTPCS_83XX_MAC_LINK_SPD_BITS		2

#define RTPCS_930X_CPU_PORT			28
#define RTPCS_930X_SERDES_CNT			12
#define RTPCS_930X_MAC_LINK_DUP_STS		0xcb28
#define RTPCS_930X_MAC_LINK_SPD_STS		0xcb18
#define RTPCS_930X_MAC_LINK_STS			0xcb10
#define RTPCS_930X_MAC_RX_PAUSE_STS		0xcb30
#define RTPCS_930X_MAC_TX_PAUSE_STS		0xcb2c

#define RTPCS_931X_CPU_PORT			56
#define RTPCS_931X_SERDES_CNT			14
#define RTPCS_931X_MAC_LINK_DUP_STS		0x0ef0
#define RTPCS_931X_MAC_LINK_SPD_STS		0x0ed0
#define RTPCS_931X_MAC_LINK_STS			0x0ec0
#define RTPCS_931X_MAC_RX_PAUSE_STS		0x0f00
#define RTPCS_931X_MAC_TX_PAUSE_STS		0x0ef8

#define RTPCS_838X_SDS_CFG_REG			0x34
#define RTPCS_838X_RST_GLB_CTRL_0		0x3c
#define RTPCS_838X_SDS_MODE_SEL			0x0028
#define RTPCS_838X_INT_RW_CTRL			0x0058
#define RTPCS_838X_INT_MODE_CTRL		0x005c
#define RTPCS_838X_PLL_CML_CTRL			0x0ff8

#define RTPCS_93XX_MAC_LINK_SPD_BITS		4

#define RTL93XX_MODEL_NAME_INFO			(0x0004)
#define RTL93XX_CHIP_INFO			(0x0008)

#define PHY_PAGE_2	2
#define PHY_PAGE_4	4

#define RTL9300_PHY_ID_MASK 0xf0ffffff

/* RTL930X SerDes supports the following modes:
 * 0x02: SGMII		0x04: 1000BX_FIBER	0x05: FIBER100
 * 0x06: QSGMII		0x09: RSGMII		0x0d: USXGMII
 * 0x10: XSGMII		0x12: HISGMII		0x16: 2500Base_X
 * 0x17: RXAUI_LITE	0x19: RXAUI_PLUS	0x1a: 10G Base-R
 * 0x1b: 10GR1000BX_AUTO			0x1f: OFF
 */
#define RTL930X_SDS_MODE_SGMII		0x02
#define RTL930X_SDS_MODE_1000BASEX	0x04
#define RTL930X_SDS_MODE_USXGMII	0x0d
#define RTL930X_SDS_MODE_XGMII		0x10
#define RTL930X_SDS_MODE_2500BASEX	0x16
#define RTL930X_SDS_MODE_10GBASER	0x1a
#define RTL930X_SDS_OFF			0x1f
#define RTL930X_SDS_MASK		0x1f

/* RTL930X SerDes supports two submodes when mode is USXGMII:
 * 0x00: USXGMII (aka USXGMII_SX)
 * 0x02: 10G_QXGMII (aka USXGMII_QX)
 */
#define RTL930X_SDS_SUBMODE_USXGMII_SX	0x0
#define RTL930X_SDS_SUBMODE_USXGMII_QX	0x2

#define RTSDS_930X_PLL_1000		0x1
#define RTSDS_930X_PLL_10000		0x5
#define RTSDS_930X_PLL_2500		0x3
#define RTSDS_930X_PLL_LC		0x3
#define RTSDS_930X_PLL_RING		0x1

/* Registers of the internal SerDes of the 9310 */
#define RTL931X_SERDES_MODE_CTRL		(0x13cc)
#define RTL931X_PS_SERDES_OFF_MODE_CTRL_ADDR	(0x13F4)
#define RTL931X_MAC_SERDES_MODE_CTRL(sds)	(0x136C + (((sds) << 2)))

enum rtpcs_sds_mode {
	RTPCS_SDS_MODE_OFF = 0,

	/* fiber modes */
	RTPCS_SDS_MODE_1000BASEX,
	RTPCS_SDS_MODE_2500BASEX,
	RTPCS_SDS_MODE_10GBASER,

	/* mii modes */
	RTPCS_SDS_MODE_SGMII,
	RTPCS_SDS_MODE_HISGMII,
	RTPCS_SDS_MODE_QSGMII,
	RTPCS_SDS_MODE_QHSGMII,
	RTPCS_SDS_MODE_XSGMII,

	RTPCS_SDS_MODE_USXGMII_10GSXGMII,
	RTPCS_SDS_MODE_USXGMII_10GDXGMII,
	RTPCS_SDS_MODE_USXGMII_10GQXGMII,
	RTPCS_SDS_MODE_USXGMII_5GSXGMII,
	RTPCS_SDS_MODE_USXGMII_5GDXGMII,
	RTPCS_SDS_MODE_USXGMII_2_5GSXGMII,
};

struct rtpcs_ctrl;

struct rtpcs_serdes {
	struct rtpcs_ctrl *ctrl;
	u8 id;
	enum rtpcs_sds_mode mode;

	bool rx_pol_inv;
	bool tx_pol_inv;
};

struct rtpcs_ctrl {
	struct device *dev;
	struct regmap *map;
	struct mii_bus *bus;
	const struct rtpcs_config *cfg;
	struct rtpcs_serdes serdes[RTPCS_SDS_CNT];
	struct rtpcs_link *link[RTPCS_PORT_CNT];
	struct mutex lock;
};

struct rtpcs_link {
	struct rtpcs_ctrl *ctrl;
	struct phylink_pcs pcs;
	struct rtpcs_serdes *sds;
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
	u8 serdes_count;

	const struct phylink_pcs_ops *pcs_ops;
	int (*init_serdes_common)(struct rtpcs_ctrl *ctrl);
	int (*set_autoneg)(struct rtpcs_serdes *sds, unsigned int neg_mode);
	int (*setup_serdes)(struct rtpcs_serdes *sds, phy_interface_t mode);
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

static int rtpcs_sds_read(struct rtpcs_serdes *sds, int page, int regnum)
{
	int mmd_regnum = rtpcs_sds_to_mmd(page, regnum);

	return mdiobus_c45_read(sds->ctrl->bus, sds->id, MDIO_MMD_VEND1,
				mmd_regnum);
}

static int rtpcs_sds_read_bits(struct rtpcs_serdes *sds, int page,
			       int regnum, int bithigh, int bitlow)
{
	int mask, val;

	WARN_ON(bithigh < bitlow);

	mask = GENMASK(bithigh, bitlow);
	val = rtpcs_sds_read(sds, page, regnum);
	if (val < 0)
		return val;

	return (val & mask) >> bitlow;
}

static int rtpcs_sds_write(struct rtpcs_serdes *sds, int page, int regnum, u16 value)
{
	int mmd_regnum = rtpcs_sds_to_mmd(page, regnum);

	return mdiobus_c45_write(sds->ctrl->bus, sds->id, MDIO_MMD_VEND1,
				 mmd_regnum, value);
}

static int rtpcs_sds_write_bits(struct rtpcs_serdes *sds, int page,
				int regnum, int bithigh, int bitlow, u16 value)
{
	int mask, reg;

	WARN_ON(bithigh < bitlow);

	mask = GENMASK(bithigh, bitlow);
	reg = rtpcs_sds_read(sds, page, regnum);
	if (reg < 0)
		return reg;

	reg = (reg & ~mask);
	reg |= (value << bitlow) & mask;

	return rtpcs_sds_write(sds, page, regnum, reg);
}

static int rtpcs_sds_modify(struct rtpcs_serdes *sds, int page, int regnum,
			    u16 mask, u16 set)
{
	int mmd_regnum = rtpcs_sds_to_mmd(page, regnum);

	return mdiobus_c45_modify(sds->ctrl->bus, sds->id, MDIO_MMD_VEND1,
				  mmd_regnum, mask, set);
}

static struct rtpcs_serdes *rtpcs_sds_get_even(struct rtpcs_serdes *sds)
{
	u32 even_sds = sds->id & ~1;
	return &sds->ctrl->serdes[even_sds];
}

static struct rtpcs_serdes *rtpcs_sds_get_odd(struct rtpcs_serdes *sds)
{
	u32 odd_sds = sds->id | 1;
	return &sds->ctrl->serdes[odd_sds];
}

static struct rtpcs_serdes *rtpcs_sds_get_neighbor(struct rtpcs_serdes *sds)
{
	u32 nb_sds = sds->id ^ 1;
	return &sds->ctrl->serdes[nb_sds];
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

/* RTL838X */

#define SDS(ctrl,n)	(&(ctrl)->serdes[n])

static void rtpcs_838x_sds_patch_01_qsgmii_6275b(struct rtpcs_ctrl *ctrl)
{
	rtpcs_sds_write(SDS(ctrl, 0), 1, 3, 0xf46f);
	rtpcs_sds_write(SDS(ctrl, 0), 1, 2, 0x85fa);
	rtpcs_sds_write(SDS(ctrl, 1), 1, 2, 0x85fa);
	rtpcs_sds_write(SDS(ctrl, 0), 1, 6, 0x20d8);
	rtpcs_sds_write(SDS(ctrl, 1), 1, 6, 0x20d8);
	rtpcs_sds_write(SDS(ctrl, 0), 1, 17, 0xb7c9);
	rtpcs_sds_write(SDS(ctrl, 1), 1, 11, 0x482);
	rtpcs_sds_write(SDS(ctrl, 1), 1, 10, 0x80c7);
	rtpcs_sds_write(SDS(ctrl, 0), 1, 18, 0xab8e);
	rtpcs_sds_write(SDS(ctrl, 0), 1, 11, 0x482);
	rtpcs_sds_write(SDS(ctrl, 0), 1, 19, 0x24ab);
	rtpcs_sds_write(SDS(ctrl, 1), 1, 17, 0x4208);
	rtpcs_sds_write(SDS(ctrl, 1), 1, 18, 0xc208);
	rtpcs_sds_write(SDS(ctrl, 0), 2, 25, 0x303);
	rtpcs_sds_write(SDS(ctrl, 1), 2, 25, 0x303);
	rtpcs_sds_write(SDS(ctrl, 0), 1, 14, 0xfcc2);
	rtpcs_sds_write(SDS(ctrl, 1), 1, 14, 0xfcc2);

	rtpcs_sds_write(SDS(ctrl, 0), 1, 9, 0x8e64);
	rtpcs_sds_write(SDS(ctrl, 0), 1, 9, 0x8c64);

	rtpcs_sds_write(SDS(ctrl, 1), 1, 9, 0x8e64);
	rtpcs_sds_write(SDS(ctrl, 1), 1, 9, 0x8c64);
}

static void rtpcs_838x_sds_patch_23_qsgmii_6275b(struct rtpcs_ctrl *ctrl)
{
	rtpcs_sds_write(SDS(ctrl, 2), 1, 3, 0xf46d);
	rtpcs_sds_write(SDS(ctrl, 2), 1, 2, 0x85fa);
	rtpcs_sds_write(SDS(ctrl, 3), 1, 2, 0x85fa);
	rtpcs_sds_write(SDS(ctrl, 2), 1, 6, 0x20d8);
	rtpcs_sds_write(SDS(ctrl, 3), 1, 6, 0x20d8);
	rtpcs_sds_write(SDS(ctrl, 2), 1, 17, 0xb7c9);
	rtpcs_sds_write(SDS(ctrl, 2), 1, 18, 0xab8e);
	rtpcs_sds_write(SDS(ctrl, 2), 1, 11, 0x482);
	rtpcs_sds_write(SDS(ctrl, 3), 1, 11, 0x482);
	rtpcs_sds_write(SDS(ctrl, 2), 1, 19, 0x24ab);
	rtpcs_sds_write(SDS(ctrl, 3), 1, 17, 0x4208);
	rtpcs_sds_write(SDS(ctrl, 3), 1, 18, 0xc208);
	rtpcs_sds_write(SDS(ctrl, 2), 2, 25, 0x303);
	rtpcs_sds_write(SDS(ctrl, 3), 2, 25, 0x303);
	rtpcs_sds_write(SDS(ctrl, 2), 1, 14, 0xfcc2);
	rtpcs_sds_write(SDS(ctrl, 3), 1, 14, 0xfcc2);

	rtpcs_sds_write(SDS(ctrl, 2), 1, 9, 0x8e64);
	rtpcs_sds_write(SDS(ctrl, 2), 1, 9, 0x8c64);

	rtpcs_sds_write(SDS(ctrl, 3), 1, 9, 0x8e64);
	rtpcs_sds_write(SDS(ctrl, 3), 1, 9, 0x8c64);
}

static void rtpcs_838x_sds_patch_4_fiber_6275b(struct rtpcs_ctrl *ctrl)
{
	rtpcs_sds_write(SDS(ctrl, 4), 1, 2, 0x85fa);
	rtpcs_sds_write(SDS(ctrl, 4), 1, 11, 0x1482);
	rtpcs_sds_write(SDS(ctrl, 4), 1, 6, 0x20d8);
	rtpcs_sds_write(SDS(ctrl, 4), 1, 10, 0xc3);
	rtpcs_sds_write(SDS(ctrl, 4), 1, 17, 0xb7c9);
	rtpcs_sds_write(SDS(ctrl, 4), 1, 18, 0xab8e);
	rtpcs_sds_write(SDS(ctrl, 4), 2, 25, 0x303);
	rtpcs_sds_write(SDS(ctrl, 4), 1, 14, 0xfcc2);

	rtpcs_sds_write(SDS(ctrl, 4), 1, 9, 0x8e64);
	rtpcs_sds_write(SDS(ctrl, 4), 1, 9, 0x8c64);
}

static void rtpcs_838x_sds_patch_4_qsgmii_6275b(struct rtpcs_ctrl *ctrl)
{
	rtpcs_sds_write(SDS(ctrl, 4), 1, 3, 0xf46d);
	rtpcs_sds_write(SDS(ctrl, 4), 1, 2, 0x85fa);
	rtpcs_sds_write(SDS(ctrl, 4), 1, 11, 0x0482);
	rtpcs_sds_write(SDS(ctrl, 4), 1, 6, 0x20d8);
	rtpcs_sds_write(SDS(ctrl, 4), 1, 10, 0x58c7);
	rtpcs_sds_write(SDS(ctrl, 4), 1, 17, 0xb7c9);
	rtpcs_sds_write(SDS(ctrl, 4), 1, 18, 0xab8e);
	rtpcs_sds_write(SDS(ctrl, 4), 2, 25, 0x303);
	rtpcs_sds_write(SDS(ctrl, 4), 1, 14, 0xfcc2);

	rtpcs_sds_write(SDS(ctrl, 4), 1, 9, 0x8e64);
	rtpcs_sds_write(SDS(ctrl, 4), 1, 9, 0x8c64);
}

static void rtpcs_838x_sds_patch_5_fiber_6275b(struct rtpcs_ctrl *ctrl)
{
	rtpcs_sds_write(SDS(ctrl, 5), 1, 2, 0x85fa);
	rtpcs_sds_write(SDS(ctrl, 5), 1, 3, 0x00);
	rtpcs_sds_write(SDS(ctrl, 5), 1, 4, 0xdccc);
	rtpcs_sds_write(SDS(ctrl, 5), 1, 5, 0x00);
	rtpcs_sds_write(SDS(ctrl, 5), 1, 6, 0x3600);
	rtpcs_sds_write(SDS(ctrl, 5), 1, 7, 0x03);
	rtpcs_sds_write(SDS(ctrl, 5), 1, 8, 0x79aa);
	rtpcs_sds_write(SDS(ctrl, 5), 1, 9, 0x8c64);
	rtpcs_sds_write(SDS(ctrl, 5), 1, 10, 0xc3);
	rtpcs_sds_write(SDS(ctrl, 5), 1, 11, 0x1482);
	rtpcs_sds_write(SDS(ctrl, 5), 2, 24, 0x14aa);
	rtpcs_sds_write(SDS(ctrl, 5), 2, 25, 0x303);
	rtpcs_sds_write(SDS(ctrl, 5), 1, 14, 0xf002);
	rtpcs_sds_write(SDS(ctrl, 5), 2, 27, 0x4bf);

	rtpcs_sds_write(SDS(ctrl, 5), 1, 9, 0x8e64);
	rtpcs_sds_write(SDS(ctrl, 5), 1, 9, 0x8c64);
}

static void rtpcs_838x_sds_reset(struct rtpcs_serdes *sds)
{
	rtpcs_sds_write_bits(sds, 2, 0, 11, 11, 0x0);	/* FIB_REG0 CFG_FIB_PDOWN */

	/* analog reset */
	rtpcs_sds_write_bits(sds, 0, 0, 1, 0, 0x0);	/* REG0 EN_RX/EN_TX */
	rtpcs_sds_write_bits(sds, 0, 0, 1, 0, 0x3);	/* REG0 EN_RX/EN_TX */

	/* digital reset */
	rtpcs_sds_write_bits(sds, 0, 3, 6, 6, 0x1);	/* REG3 SOFT_RST */
	rtpcs_sds_write_bits(sds, 0, 3, 6, 6, 0x0);	/* REG3 SOFT_RST */

	dev_info(sds->ctrl->dev, "SerDes %d reset\n", sds->id);
}

static bool rtpcs_838x_sds_is_mode_supported(struct rtpcs_serdes *sds, phy_interface_t mode)
{
	switch (sds->id) {
	case 0 ... 3:
		return mode == PHY_INTERFACE_MODE_QSGMII;
	case 4:
		return mode == PHY_INTERFACE_MODE_QSGMII ||
		       mode == PHY_INTERFACE_MODE_SGMII ||
		       mode == PHY_INTERFACE_MODE_1000BASEX;
	case 5:
		return mode == PHY_INTERFACE_MODE_SGMII ||
		       mode == PHY_INTERFACE_MODE_1000BASEX;
	default:
		return false;
	}
}

static int rtpcs_838x_sds_power(struct rtpcs_serdes *sds, bool power_on)
{
	u8 sds_id = sds->id;
	int ret;
	u8 val;

	val = power_on ? 0 : BIT(sds_id);

	ret = regmap_write_bits(sds->ctrl->map, RTPCS_838X_SDS_CFG_REG, BIT(sds_id), val);
	if (ret)
		return ret;

	if (sds_id >= 4)
		ret = regmap_write_bits(sds->ctrl->map, RTPCS_838X_SDS_CFG_REG,
					BIT(sds_id) << 2, val << 2); /* SDS*_PHY_MODE */

	return ret;
}

static int rtpcs_838x_sds_set_mode(struct rtpcs_serdes *sds,
				   phy_interface_t mode)
{
	u8 sds_mode_shift, int_mode_shift;
	u32 sds_mode_val, int_mode_val;

	switch (mode) {
	case PHY_INTERFACE_MODE_1000BASEX:
		sds_mode_val = 0x4;
		int_mode_val = 0x1;
		break;
	case PHY_INTERFACE_MODE_SGMII:
		sds_mode_val = 0x2;
		int_mode_val = 0x2;
		break;
	case PHY_INTERFACE_MODE_QSGMII:
		sds_mode_val = 0x6;
		int_mode_val = 0x5;
		break;
	default:
		return -EINVAL;
	}

	/* Configure SerDes module mode (all SDS 0-5) */
	sds_mode_shift = (5 - sds->id) * 5;
	regmap_write_bits(sds->ctrl->map, RTPCS_838X_SDS_MODE_SEL,
			  0x1f << sds_mode_shift, sds_mode_val << sds_mode_shift);

	/* Configure MAC interface mode (only SDS 4-5) */
	if (sds->id >= 4) {
		int_mode_shift = (sds->id == 5) ? 3 : 0;
		regmap_write_bits(sds->ctrl->map, RTPCS_838X_INT_MODE_CTRL,
				  0x7 << int_mode_shift, int_mode_val << int_mode_shift);
	}

	return 0;
}

static int rtpcs_838x_sds_patch(struct rtpcs_serdes *sds,
				phy_interface_t mode)
{
	struct rtpcs_ctrl *ctrl = sds->ctrl;
	u8 sds_id = sds->id;

	rtpcs_sds_write(sds, 0, 1, 0xf00);
	mdelay(1);
	rtpcs_sds_write(sds, 0, 2, 0x7060);
	mdelay(1);

	if (sds_id >= 4) {
		rtpcs_sds_write(sds, 2, 30, 0x71e);
		mdelay(1);
		rtpcs_sds_write(sds, 0, 4, 0x74d);
		mdelay(1);
	}

	switch (mode) {
	case PHY_INTERFACE_MODE_1000BASEX:
		if (sds_id == 4)
			rtpcs_838x_sds_patch_4_fiber_6275b(ctrl);
		else if (sds_id == 5)
			rtpcs_838x_sds_patch_5_fiber_6275b(ctrl);

		break;
	case PHY_INTERFACE_MODE_QSGMII:
		if (sds_id == 0 || sds_id == 1)
			rtpcs_838x_sds_patch_01_qsgmii_6275b(ctrl);
		else if (sds_id == 2 || sds_id == 3)
			rtpcs_838x_sds_patch_23_qsgmii_6275b(ctrl);
		else if (sds_id == 4)
			rtpcs_838x_sds_patch_4_qsgmii_6275b(ctrl);

		break;
	default:
		break;
	}
	
	return 0;
}

static int rtpcs_838x_init_serdes_common(struct rtpcs_ctrl *ctrl)
{
	u32 val;

	dev_dbg(ctrl->dev, "Init RTL838X SerDes common\n");

	/* enable R/W of some protected registers */
	regmap_write(ctrl->map, RTPCS_838X_INT_RW_CTRL, 0x3);

	regmap_read(ctrl->map, RTPCS_838X_PLL_CML_CTRL, &val);
	dev_dbg(ctrl->dev, "PLL control register: %x\n", val);
	regmap_write_bits(ctrl->map, RTPCS_838X_PLL_CML_CTRL, 0xfffffff0,
			  0xaaaaaaaf & 0xf);

	/* power off and reset all SerDes */
	regmap_write(ctrl->map, RTPCS_838X_SDS_CFG_REG, 0x3f);
	regmap_write(ctrl->map, RTPCS_838X_RST_GLB_CTRL_0, 0x10); /* SW_SERDES_RST */
	return 0;
}

static int rtpcs_838x_setup_serdes(struct rtpcs_serdes *sds,
				   phy_interface_t mode)
{
	int ret;

	if (!rtpcs_838x_sds_is_mode_supported(sds, mode))
		return -EINVAL;

	rtpcs_838x_sds_power(sds, false);

	/* take reset */
	rtpcs_sds_write(sds, 0x0, 0x0, 0xc00);
	rtpcs_sds_write(sds, 0x0, 0x3, 0x7146);

	ret = rtpcs_838x_sds_set_mode(sds, mode);
	if (ret)
		return ret;

	rtpcs_838x_sds_patch(sds, mode);
	rtpcs_838x_sds_reset(sds);
	
	/* release reset */
	rtpcs_sds_write(sds, 0, 3, 0x7106);

	rtpcs_838x_sds_power(sds, true);
	return 0;
}

/* RTL930X */

/* The access registers for SDS_MODE_SEL and the LSB for each SDS within */
u16 rtpcs_930x_sds_regs[] = { 0x0194, 0x0194, 0x0194, 0x0194, 0x02a0, 0x02a0, 0x02a0, 0x02a0,
			      0x02A4, 0x02A4, 0x0198, 0x0198 };
u8  rtpcs_930x_sds_lsb[]  = { 0, 6, 12, 18, 0, 6, 12, 18, 0, 6, 0, 6};

u16 rtpcs_930x_sds_submode_regs[] = { 0x1cc, 0x1cc, 0x2d8, 0x2d8, 0x2d8, 0x2d8,
				      0x2d8, 0x2d8};
u8  rtpcs_930x_sds_submode_lsb[]  = { 0, 5, 0, 5, 10, 15, 20, 25 };

static void rtpcs_930x_sds_set(struct rtpcs_serdes *sds, u32 mode)
{
	u8 sds_id = sds->id;

	pr_info("%s %d\n", __func__, mode);

	regmap_write_bits(sds->ctrl->map, rtpcs_930x_sds_regs[sds_id],
			  RTL930X_SDS_MASK << rtpcs_930x_sds_lsb[sds_id],
			  mode << rtpcs_930x_sds_lsb[sds_id]);
	mdelay(10);
}

__always_unused
static u32 rtpcs_930x_sds_mode_get(struct rtpcs_serdes *sds)
{
	u8 sds_id = sds->id;
	u32 v;

	regmap_read(sds->ctrl->map, rtpcs_930x_sds_regs[sds_id], &v);
	v >>= rtpcs_930x_sds_lsb[sds_id];

	return v & RTL930X_SDS_MASK;
}

__always_unused
static u32 rtpcs_930x_sds_submode_get(struct rtpcs_serdes *sds)
{
	u8 sds_id = sds->id;
	u32 v;

	if (sds_id < 2 || sds_id > 9) {
		pr_err("%s: unsupported SerDes %d\n", __func__, sds_id);
		return 0;
	}

	regmap_read(sds->ctrl->map, rtpcs_930x_sds_submode_regs[sds_id], &v);
	v >>= rtpcs_930x_sds_submode_lsb[sds_id];

	return v & RTL930X_SDS_MASK;
}

static void rtpcs_930x_sds_submode_set(struct rtpcs_serdes *sds,
				       u32 submode)
{
	u8 sds_id = sds->id;

	if (sds_id < 2 || sds_id > 9) {
		pr_err("%s: submode unsupported on serdes %d\n", __func__, sds_id);
		return;
	}

	if (submode != RTL930X_SDS_SUBMODE_USXGMII_SX &&
	    submode != RTL930X_SDS_SUBMODE_USXGMII_QX) {
		pr_err("%s: unsupported submode 0x%x\n", __func__, submode);
	}

	regmap_write_bits(sds->ctrl->map, rtpcs_930x_sds_submode_regs[sds_id - 2],
			  RTL930X_SDS_MASK << rtpcs_930x_sds_submode_lsb[sds_id - 2],
			  submode << rtpcs_930x_sds_submode_lsb[sds_id - 2]);
}

static void rtpcs_930x_sds_rx_reset(struct rtpcs_serdes *sds,
				    phy_interface_t phy_if)
{
	int page = 0x2e; /* 10GR and USXGMII */

	if (phy_if == PHY_INTERFACE_MODE_1000BASEX)
		page = 0x24;

	rtpcs_sds_write_bits(sds, page, 0x15, 4, 4, 0x1);
	mdelay(5);
	rtpcs_sds_write_bits(sds, page, 0x15, 4, 4, 0x0);
}

static void rtpcs_930x_sds_get_pll_data(struct rtpcs_serdes *sds,
					int *pll, int *speed)
{
	struct rtpcs_serdes *even_sds = rtpcs_sds_get_even(sds);
	int sbit, pbit = (sds == even_sds) ? 4 : 6;

	/*
	 * PLL data is shared between adjacent SerDes in the even lane. Each SerDes defines
	 * what PLL it needs (ring or LC) while the PLL itself stores the current speed.
	 */

	*pll = rtpcs_sds_read_bits(even_sds, 0x20, 0x12, pbit + 1, pbit);
	sbit = *pll == RTSDS_930X_PLL_LC ? 8 : 12;
	*speed = rtpcs_sds_read_bits(even_sds, 0x20, 0x12, sbit + 3, sbit);
}

static int rtpcs_930x_sds_set_pll_data(struct rtpcs_serdes *sds,
				       int pll, int speed)
{
	struct rtpcs_serdes *even_sds = rtpcs_sds_get_even(sds);
	int sbit = pll == RTSDS_930X_PLL_LC ? 8 : 12;
	int pbit = (sds == even_sds) ? 4 : 6;

	if ((speed != RTSDS_930X_PLL_1000) &&
	    (speed != RTSDS_930X_PLL_2500) &&
	    (speed != RTSDS_930X_PLL_10000))
		return -EINVAL;

	if ((pll != RTSDS_930X_PLL_RING) && (pll != RTSDS_930X_PLL_LC))
		return -EINVAL;

	if ((pll == RTSDS_930X_PLL_RING) && (speed == RTSDS_930X_PLL_10000))
		return -EINVAL;

	/*
	 * A SerDes clock can either be taken from the low speed ring PLL or the high speed
	 * LC PLL. As it is unclear if disabling PLLs has any positive or negative effect,
	 * always activate both.
	 */

	rtpcs_sds_write_bits(even_sds, 0x20, 0x12, 3, 0, 0xf);
	rtpcs_sds_write_bits(even_sds, 0x20, 0x12, pbit + 1, pbit, pll);
	rtpcs_sds_write_bits(even_sds, 0x20, 0x12, sbit + 3, sbit, speed);

	return 0;
}

static void rtpcs_930x_sds_reset_cmu(struct rtpcs_serdes *sds)
{
	struct rtpcs_serdes *even_sds = rtpcs_sds_get_even(sds);
	int reset_sequence[4] = { 3, 2, 3, 1 };
	int pll, speed, i, bit;

	/*
	 * After the PLL speed has changed, the CMU must take over the new values. The models
	 * of the Otto platform have different reset sequences. Luckily it always boils down
	 * to flipping two bits in a special sequence.
	 */

	rtpcs_930x_sds_get_pll_data(sds, &pll, &speed);
	bit = pll == RTSDS_930X_PLL_LC ? 2 : 0;

	for (i = 0; i < ARRAY_SIZE(reset_sequence); i++)
		rtpcs_sds_write_bits(even_sds, 0x21, 0x0b, bit + 1, bit,
				     reset_sequence[i]);
}

static int rtpcs_930x_sds_wait_clock_ready(struct rtpcs_serdes *sds)
{
	struct rtpcs_serdes *even_sds = rtpcs_sds_get_even(sds);
	int i, ready, ready_cnt = 0, bit = (sds == even_sds) ? 4 : 5;

	/*
	 * While reconfiguring a SerDes it might take some time until its clock is in sync with
	 * the PLL. During that timespan the ready signal might toggle randomly. According to
	 * GPL sources it is enough to verify that 3 consecutive clock ready checks say "ok".
	 */

	for (i = 0; i < 20; i++) {
		usleep_range(10000, 15000);

		rtpcs_sds_write(even_sds, 0x1f, 0x02, 53);
		ready = rtpcs_sds_read_bits(even_sds, 0x1f, 0x14, bit, bit);

		ready_cnt = ready ? ready_cnt + 1 : 0;
		if (ready_cnt >= 3)
			return 0;
	}

	return -EBUSY;
}

static int rtpcs_930x_sds_get_internal_mode(struct rtpcs_serdes *sds)
{
	return rtpcs_sds_read_bits(sds, 0x1f, 0x09, 11, 7);
}

static void rtpcs_930x_sds_set_internal_mode(struct rtpcs_serdes *sds, int mode)
{
	rtpcs_sds_write_bits(sds, 0x1f, 0x09, 6, 6, 0x1); /* Force mode enable */
	rtpcs_sds_write_bits(sds, 0x1f, 0x09, 11, 7, mode);
}

static void rtpcs_930x_sds_set_power(struct rtpcs_serdes *sds, bool on)
{
	int power_down = on ? 0x0 : 0x3;
	int rx_enable = on ? 0x3 : 0x1;

	rtpcs_sds_write_bits(sds, 0x20, 0x00, 7, 6, power_down);
	rtpcs_sds_write_bits(sds, 0x20, 0x00, 5, 4, rx_enable);
}

static void rtpcs_930x_sds_reconfigure_pll(struct rtpcs_serdes *sds, int pll)
{
	int mode, tmp, speed;

	mode = rtpcs_930x_sds_get_internal_mode(sds);
	rtpcs_930x_sds_get_pll_data(sds, &tmp, &speed);

	rtpcs_930x_sds_set_power(sds, false);
	rtpcs_930x_sds_set_internal_mode(sds, RTL930X_SDS_OFF);

	rtpcs_930x_sds_set_pll_data(sds, pll, speed);
	rtpcs_930x_sds_reset_cmu(sds);

	rtpcs_930x_sds_set_internal_mode(sds, mode);
	if (rtpcs_930x_sds_wait_clock_ready(sds))
		pr_err("%s: SDS %d could not sync clock\n", __func__, sds->id);

	rtpcs_930x_sds_set_power(sds, true);
}

static int rtpcs_930x_sds_config_pll(struct rtpcs_serdes *sds,
				     phy_interface_t interface)
{
	struct rtpcs_serdes *nb_sds = rtpcs_sds_get_neighbor(sds);
	int neighbor_speed, neighbor_mode, neighbor_pll;
	bool speed_changed = true;
	int pll, speed;

	/*
	 * A SerDes pair on the RTL930x is driven by two PLLs. A low speed ring PLL can generate
	 * signals of 1.25G and 3.125G for link speeds of 1G/2.5G. A high speed LC PLL can
	 * additionally generate a 10.3125G signal for 10G speeds. To drive the pair at different
	 * speeds each SerDes must use its own PLL. But what if the SerDess attached to the ring
	 * PLL suddenly needs 10G but the LC PLL is running at 1G? To avoid reconfiguring the
	 * "partner" SerDes we must choose wisely what assignment serves the current needs. The
	 * logic boils down to the following rules:
	 *
	 * - Use ring PLL for slow 1G speeds
	 * - Use LC PLL for fast 10G speeds
	 * - For 2.5G prefer ring over LC PLL
	 *
	 * However, when we want to configure 10G speed while the other SerDes is already using
	 * the LC PLL for a slower speed, there is no way to avoid reconfiguration. Note that
	 * this can even happen when the other SerDes is not actually in use, because changing
	 * the state of a SerDes back to RTL930X_SDS_OFF is not (yet) implemented.
	 */

	neighbor_mode = rtpcs_930x_sds_get_internal_mode(nb_sds);
	rtpcs_930x_sds_get_pll_data(nb_sds, &neighbor_pll, &neighbor_speed);

	if ((interface == PHY_INTERFACE_MODE_1000BASEX) ||
	    (interface == PHY_INTERFACE_MODE_SGMII))
		speed = RTSDS_930X_PLL_1000;
	else if (interface == PHY_INTERFACE_MODE_2500BASEX)
		speed = RTSDS_930X_PLL_2500;
	else if (interface == PHY_INTERFACE_MODE_10GBASER)
		speed = RTSDS_930X_PLL_10000;
	else
		return -ENOTSUPP;

	if (!neighbor_mode)
		pll = speed == RTSDS_930X_PLL_10000 ? RTSDS_930X_PLL_LC : RTSDS_930X_PLL_RING;
	else if (speed == neighbor_speed) {
		speed_changed = false;
		pll = neighbor_pll;
	} else if (neighbor_pll == RTSDS_930X_PLL_RING)
		pll = RTSDS_930X_PLL_LC;
	else if (speed == RTSDS_930X_PLL_10000) {
		pr_info("%s: SDS %d needs LC PLL, reconfigure SDS %d to use ring PLL\n",
			__func__, sds->id, nb_sds->id);
		rtpcs_930x_sds_reconfigure_pll(nb_sds, RTSDS_930X_PLL_RING);
		pll = RTSDS_930X_PLL_LC;
	} else
		pll = RTSDS_930X_PLL_RING;

	rtpcs_930x_sds_set_pll_data(sds, pll, speed);

	if (speed_changed)
		rtpcs_930x_sds_reset_cmu(sds);

	pr_info("%s: SDS %d using %s PLL for %s\n", __func__, sds->id,
		pll == RTSDS_930X_PLL_LC ? "LC" : "ring", phy_modes(interface));

	return 0;
}

static void rtpcs_930x_sds_reset_state_machine(struct rtpcs_serdes *sds)
{
	rtpcs_sds_write_bits(sds, 0x06, 0x02, 12, 12, 0x01); /* SM_RESET bit */
	usleep_range(10000, 20000);
	rtpcs_sds_write_bits(sds, 0x06, 0x02, 12, 12, 0x00);
	usleep_range(10000, 20000);
}

static int rtpcs_930x_sds_init_state_machine(struct rtpcs_serdes *sds,
					     phy_interface_t interface)
{
	int loopback, link, cnt = 20, ret = -EBUSY;

	if (interface != PHY_INTERFACE_MODE_10GBASER)
		return 0;
	/*
	 * After a SerDes mode change it takes some time until the frontend state machine
	 * works properly for 10G. To verify operation readyness run a connection check via
	 * loopback.
	 */
	loopback = rtpcs_sds_read_bits(sds, 0x06, 0x01, 2, 2); /* CFG_AFE_LPK bit */
	rtpcs_sds_write_bits(sds, 0x06, 0x01, 2, 2, 0x01);

	while (cnt-- && ret) {
		rtpcs_930x_sds_reset_state_machine(sds);
		link = rtpcs_sds_read_bits(sds, 0x05, 0x00, 12, 12); /* 10G link state (latched) */
		link = rtpcs_sds_read_bits(sds, 0x05, 0x00, 12, 12);
		if (link)
			ret = 0;
	}

	rtpcs_sds_write_bits(sds, 0x06, 0x01, 2, 2, loopback);
	rtpcs_930x_sds_reset_state_machine(sds);

	return ret;
}

static void rtpcs_930x_sds_force_mode(struct rtpcs_serdes *sds,
				      phy_interface_t interface)
{
	int mode;

	/*
	 * TODO: Usually one would expect that it is enough to modify the SDS_MODE_SEL_*
	 * registers (lets call it MAC setup). It seems as if this complex sequence is only
	 * needed for modes that cannot be set by the SoC itself. Additionally it is unclear
	 * if this sequence should quit early in case of errors.
	 */

	switch (interface) {
	case PHY_INTERFACE_MODE_SGMII:
		mode = RTL930X_SDS_MODE_SGMII;
		break;
	case PHY_INTERFACE_MODE_1000BASEX:
		mode = RTL930X_SDS_MODE_1000BASEX;
		break;
	case PHY_INTERFACE_MODE_2500BASEX:
		mode = RTL930X_SDS_MODE_2500BASEX;
		break;
	case PHY_INTERFACE_MODE_10GBASER:
		mode = RTL930X_SDS_MODE_10GBASER;
		break;
	case PHY_INTERFACE_MODE_NA:
		mode = RTL930X_SDS_OFF;
		break;
	default:
		pr_err("%s: SDS %d does not support %s\n", __func__,
		       sds->id, phy_modes(interface));
		return;
	}

	rtpcs_930x_sds_set_power(sds, false);
	rtpcs_930x_sds_set_internal_mode(sds, RTL930X_SDS_OFF);
	if (interface == PHY_INTERFACE_MODE_NA)
		return;

	if (rtpcs_930x_sds_config_pll(sds, interface))
		pr_err("%s: SDS %d could not configure PLL for %s\n", __func__,
		       sds->id, phy_modes(interface));

	rtpcs_930x_sds_set_internal_mode(sds, mode);
	if (rtpcs_930x_sds_wait_clock_ready(sds))
		pr_err("%s: SDS %d could not sync clock\n", __func__, sds->id);

	if (rtpcs_930x_sds_init_state_machine(sds, interface))
		pr_err("%s: SDS %d could not reset state machine\n", __func__,
		       sds->id);

	rtpcs_930x_sds_set_power(sds, true);
	rtpcs_930x_sds_rx_reset(sds, interface);
}

static void rtpcs_930x_sds_mode_set(struct rtpcs_serdes *sds,
				    phy_interface_t phy_mode)
{
	u32 mode;
	u32 submode;

	switch (phy_mode) {
	case PHY_INTERFACE_MODE_SGMII:
	case PHY_INTERFACE_MODE_1000BASEX:
	case PHY_INTERFACE_MODE_2500BASEX:
	case PHY_INTERFACE_MODE_10GBASER:
		rtpcs_930x_sds_force_mode(sds, phy_mode);
		return;
	case PHY_INTERFACE_MODE_10G_QXGMII:
		mode = RTL930X_SDS_MODE_USXGMII;
		submode = RTL930X_SDS_SUBMODE_USXGMII_QX;
		break;
	default:
		pr_warn("%s: unsupported mode %s\n", __func__, phy_modes(phy_mode));
		return;
	}

	/* SerDes off first. */
	rtpcs_930x_sds_set(sds, RTL930X_SDS_OFF);

	/* Set the mode. */
	rtpcs_930x_sds_set(sds, mode);

	/* Set the submode if needed. */
	if (phy_mode == PHY_INTERFACE_MODE_10G_QXGMII)
		rtpcs_930x_sds_submode_set(sds, submode);
}

static void rtpcs_930x_sds_tx_config(struct rtpcs_serdes *sds,
				     phy_interface_t phy_if)
{
	/* parameters: rtl9303_80G_txParam_s2 */
	int impedance = 0x8;
	int pre_amp = 0x2;
	int main_amp = 0x9;
	int post_amp = 0x2;
	int pre_en = 0x1;
	int post_en = 0x1;
	int page;

	switch (phy_if) {
	case PHY_INTERFACE_MODE_1000BASEX:
	case PHY_INTERFACE_MODE_SGMII:
		pre_amp = 0x1;
		main_amp = 0x9;
		post_amp = 0x1;
		page = 0x25;
		break;
	case PHY_INTERFACE_MODE_2500BASEX:
		pre_amp = 0;
		post_amp = 0x8;
		pre_en = 0;
		page = 0x29;
		break;
	case PHY_INTERFACE_MODE_10GBASER:
	case PHY_INTERFACE_MODE_USXGMII:
	case PHY_INTERFACE_MODE_10G_QXGMII:
	case PHY_INTERFACE_MODE_XGMII:
		pre_en = 0;
		pre_amp = 0;
		main_amp = 0x10;
		post_amp = 0;
		post_en	= 0;
		page = 0x2f;
		break;
	default:
		pr_err("%s: unsupported PHY mode\n", __func__);
		return;
	}

	rtpcs_sds_write_bits(sds, page, 0x01, 15, 11, pre_amp);
	rtpcs_sds_write_bits(sds, page, 0x06,  4,  0, post_amp);
	rtpcs_sds_write_bits(sds, page, 0x07,  0,  0, pre_en);
	rtpcs_sds_write_bits(sds, page, 0x07,  3,  3, post_en);
	rtpcs_sds_write_bits(sds, page, 0x07,  8,  4, main_amp);
	rtpcs_sds_write_bits(sds, page, 0x18, 15, 12, impedance);
}

__always_unused
static void rtpcs_930x_sds_rxcal_dcvs_manual(struct rtpcs_serdes *sds,
					     u32 dcvs_id, bool manual, u32 dvcs_list[])
{
	if (manual) {
		switch (dcvs_id) {
		case 0:
			rtpcs_sds_write_bits(sds, 0x2e, 0x1e, 14, 14, 0x1);
			rtpcs_sds_write_bits(sds, 0x2f, 0x03,  5,  5, dvcs_list[0]);
			rtpcs_sds_write_bits(sds, 0x2f, 0x03,  4,  0, dvcs_list[1]);
			break;
		case 1:
			rtpcs_sds_write_bits(sds, 0x2e, 0x1e, 13, 13, 0x1);
			rtpcs_sds_write_bits(sds, 0x2e, 0x1d, 15, 15, dvcs_list[0]);
			rtpcs_sds_write_bits(sds, 0x2e, 0x1d, 14, 11, dvcs_list[1]);
			break;
		case 2:
			rtpcs_sds_write_bits(sds, 0x2e, 0x1e, 12, 12, 0x1);
			rtpcs_sds_write_bits(sds, 0x2e, 0x1d, 10, 10, dvcs_list[0]);
			rtpcs_sds_write_bits(sds, 0x2e, 0x1d,  9,  6, dvcs_list[1]);
			break;
		case 3:
			rtpcs_sds_write_bits(sds, 0x2e, 0x1e, 11, 11, 0x1);
			rtpcs_sds_write_bits(sds, 0x2e, 0x1d,  5,  5, dvcs_list[0]);
			rtpcs_sds_write_bits(sds, 0x2e, 0x1d,  4,  1, dvcs_list[1]);
			break;
		case 4:
			rtpcs_sds_write_bits(sds, 0x2e, 0x01, 15, 15, 0x1);
			rtpcs_sds_write_bits(sds, 0x2e, 0x11, 10, 10, dvcs_list[0]);
			rtpcs_sds_write_bits(sds, 0x2e, 0x11,  9,  6, dvcs_list[1]);
			break;
		case 5:
			rtpcs_sds_write_bits(sds, 0x2e, 0x02, 11, 11, 0x1);
			rtpcs_sds_write_bits(sds, 0x2e, 0x11,  4,  4, dvcs_list[0]);
			rtpcs_sds_write_bits(sds, 0x2e, 0x11,  3,  0, dvcs_list[1]);
			break;
		default:
			break;
		}
	} else {
		switch (dcvs_id) {
		case 0:
			rtpcs_sds_write_bits(sds, 0x2e, 0x1e, 14, 14, 0x0);
			break;
		case 1:
			rtpcs_sds_write_bits(sds, 0x2e, 0x1e, 13, 13, 0x0);
			break;
		case 2:
			rtpcs_sds_write_bits(sds, 0x2e, 0x1e, 12, 12, 0x0);
			break;
		case 3:
			rtpcs_sds_write_bits(sds, 0x2e, 0x1e, 11, 11, 0x0);
			break;
		case 4:
			rtpcs_sds_write_bits(sds, 0x2e, 0x01, 15, 15, 0x0);
			break;
		case 5:
			rtpcs_sds_write_bits(sds, 0x2e, 0x02, 11, 11, 0x0);
			break;
		default:
			break;
		}
		mdelay(1);
	}
}

__always_unused
static void rtpcs_930x_sds_rxcal_dcvs_get(struct rtpcs_serdes *sds,
					  u32 dcvs_id, u32 dcvs_list[])
{
	struct rtpcs_serdes *even_sds = rtpcs_sds_get_even(sds);
	u32 dcvs_sign_out = 0, dcvs_coef_bin = 0;
	bool dcvs_manual;

	if (sds == even_sds)
		rtpcs_sds_write(sds, 0x1f, 0x2, 0x2f);
	else
		rtpcs_sds_write(even_sds, 0x1f, 0x2, 0x31);

	/* ##Page0x2E, Reg0x15[9], REG0_RX_EN_TEST=[1] */
	rtpcs_sds_write_bits(sds, 0x2e, 0x15, 9, 9, 0x1);

	/* ##Page0x21, Reg0x06[11 6], REG0_RX_DEBUG_SEL=[1 0 x x x x] */
	rtpcs_sds_write_bits(sds, 0x21, 0x06, 11, 6, 0x20);

	switch (dcvs_id) {
	case 0:
		rtpcs_sds_write_bits(sds, 0x2f, 0x0c, 5, 0, 0x22);
		mdelay(1);

		/* ##DCVS0 Read Out */
		dcvs_sign_out = rtpcs_sds_read_bits(sds, 0x1f, 0x14,  4,  4);
		dcvs_coef_bin = rtpcs_sds_read_bits(sds, 0x1f, 0x14,  3,  0);
		dcvs_manual = !!rtpcs_sds_read_bits(sds, 0x2e, 0x1e, 14, 14);
		break;

	case 1:
		rtpcs_sds_write_bits(sds, 0x2f, 0x0c, 5, 0, 0x23);
		mdelay(1);

		/* ##DCVS0 Read Out */
		dcvs_coef_bin = rtpcs_sds_read_bits(sds, 0x1f, 0x14,  4,  4);
		dcvs_coef_bin = rtpcs_sds_read_bits(sds, 0x1f, 0x14,  3,  0);
		dcvs_manual = !!rtpcs_sds_read_bits(sds, 0x2e, 0x1e, 13, 13);
		break;

	case 2:
		rtpcs_sds_write_bits(sds, 0x2f, 0x0c, 5, 0, 0x24);
		mdelay(1);

		/* ##DCVS0 Read Out */
		dcvs_sign_out = rtpcs_sds_read_bits(sds, 0x1f, 0x14,  4,  4);
		dcvs_coef_bin = rtpcs_sds_read_bits(sds, 0x1f, 0x14,  3,  0);
		dcvs_manual = !!rtpcs_sds_read_bits(sds, 0x2e, 0x1e, 12, 12);
		break;
	case 3:
		rtpcs_sds_write_bits(sds, 0x2f, 0x0c, 5, 0, 0x25);
		mdelay(1);

		/* ##DCVS0 Read Out */
		dcvs_sign_out = rtpcs_sds_read_bits(sds, 0x1f, 0x14,  4,  4);
		dcvs_coef_bin = rtpcs_sds_read_bits(sds, 0x1f, 0x14,  3,  0);
		dcvs_manual   = rtpcs_sds_read_bits(sds, 0x2e, 0x1e, 11, 11);
		break;

	case 4:
		rtpcs_sds_write_bits(sds, 0x2f, 0x0c, 5, 0, 0x2c);
		mdelay(1);

		/* ##DCVS0 Read Out */
		dcvs_sign_out = rtpcs_sds_read_bits(sds, 0x1f, 0x14,  4,  4);
		dcvs_coef_bin = rtpcs_sds_read_bits(sds, 0x1f, 0x14,  3,  0);
		dcvs_manual = !!rtpcs_sds_read_bits(sds, 0x2e, 0x01, 15, 15);
		break;

	case 5:
		rtpcs_sds_write_bits(sds, 0x2f, 0x0c, 5, 0, 0x2d);
		mdelay(1);

		/* ##DCVS0 Read Out */
		dcvs_sign_out = rtpcs_sds_read_bits(sds, 0x1f, 0x14,  4,  4);
		dcvs_coef_bin = rtpcs_sds_read_bits(sds, 0x1f, 0x14,  3,  0);
		dcvs_manual   = rtpcs_sds_read_bits(sds, 0x2e, 0x02, 11, 11);
		break;

	default:
		break;
	}

	if (dcvs_sign_out)
		pr_info("%s DCVS %u Sign: -", __func__, dcvs_id);
	else
		pr_info("%s DCVS %u Sign: +", __func__, dcvs_id);

	pr_info("DCVS %u even coefficient = %u", dcvs_id, dcvs_coef_bin);
	pr_info("DCVS %u manual = %u", dcvs_id, dcvs_manual);

	dcvs_list[0] = dcvs_sign_out;
	dcvs_list[1] = dcvs_coef_bin;
}

static void rtpcs_930x_sds_rxcal_leq_manual(struct rtpcs_serdes *sds,
					    bool manual, u32 leq_gray)
{
	if (manual) {
		rtpcs_sds_write_bits(sds, 0x2e, 0x18, 15, 15, 0x1);
		rtpcs_sds_write_bits(sds, 0x2e, 0x16, 14, 10, leq_gray);
	} else {
		rtpcs_sds_write_bits(sds, 0x2e, 0x18, 15, 15, 0x0);
		mdelay(100);
	}
}

static void rtpcs_930x_sds_rxcal_leq_offset_manual(struct rtpcs_serdes *sds,
						   bool manual, u32 offset)
{
	if (manual) {
		rtpcs_sds_write_bits(sds, 0x2e, 0x17, 6, 2, offset);
	} else {
		rtpcs_sds_write_bits(sds, 0x2e, 0x17, 6, 2, offset);
		mdelay(1);
	}
}

#define GRAY_BITS 5
static u32 rtpcs_930x_sds_rxcal_gray_to_binary(u32 gray_code)
{
	int i, j, m;
	u32 g[GRAY_BITS];
	u32 c[GRAY_BITS];
	u32 leq_binary = 0;

	for (i = 0; i < GRAY_BITS; i++)
		g[i] = (gray_code & BIT(i)) >> i;

	m = GRAY_BITS - 1;

	c[m] = g[m];

	for (i = 0; i < m; i++) {
		c[i] = g[i];
		for (j  = i + 1; j < GRAY_BITS; j++)
			c[i] = c[i] ^ g[j];
	}

	for (i = 0; i < GRAY_BITS; i++)
		leq_binary += c[i] << i;

	return leq_binary;
}

static u32 rtpcs_930x_sds_rxcal_leq_read(struct rtpcs_serdes *sds)
{
	struct rtpcs_serdes *even_sds = rtpcs_sds_get_even(sds);
	u32 leq_gray, leq_bin;
	bool leq_manual;

	if (sds == even_sds)
		rtpcs_sds_write(sds, 0x1f, 0x2, 0x2f);
	else
		rtpcs_sds_write(even_sds, 0x1f, 0x2, 0x31);

	/* ##Page0x2E, Reg0x15[9], REG0_RX_EN_TEST=[1] */
	rtpcs_sds_write_bits(sds, 0x2e, 0x15, 9, 9, 0x1);

	/* ##Page0x21, Reg0x06[11 6], REG0_RX_DEBUG_SEL=[0 1 x x x x] */
	rtpcs_sds_write_bits(sds, 0x21, 0x06, 11, 6, 0x10);
	mdelay(1);

	/* ##LEQ Read Out */
	leq_gray = rtpcs_sds_read_bits(sds, 0x1f, 0x14, 7, 3);
	leq_manual = !!rtpcs_sds_read_bits(sds, 0x2e, 0x18, 15, 15);
	leq_bin = rtpcs_930x_sds_rxcal_gray_to_binary(leq_gray);

	pr_info("LEQ_gray: %u, LEQ_bin: %u", leq_gray, leq_bin);
	pr_info("LEQ manual: %u", leq_manual);

	return leq_bin;
}

static void rtpcs_930x_sds_rxcal_vth_manual(struct rtpcs_serdes *sds,
					    bool manual, u32 vth_list[])
{
	if (manual) {
		rtpcs_sds_write_bits(sds, 0x2e, 0x0f, 13, 13, 0x1);
		rtpcs_sds_write_bits(sds, 0x2e, 0x13,  5,  3, vth_list[0]);
		rtpcs_sds_write_bits(sds, 0x2e, 0x13,  2,  0, vth_list[1]);
	} else {
		rtpcs_sds_write_bits(sds, 0x2e, 0x0f, 13, 13, 0x0);
		mdelay(10);
	}
}

static void rtpcs_930x_sds_rxcal_vth_get(struct rtpcs_serdes *sds,
					 u32 vth_list[])
{
	struct rtpcs_serdes *even_sds = rtpcs_sds_get_even(sds);
	u32 vth_manual;

	/* ##Page0x1F, Reg0x02[15 0], REG_DBGO_SEL=[0x002F]; */ /* Lane0 */
	/* ##Page0x1F, Reg0x02[15 0], REG_DBGO_SEL=[0x0031]; */ /* Lane1 */
	if (sds == even_sds)
		rtpcs_sds_write(sds, 0x1f, 0x2, 0x2f);
	else
		rtpcs_sds_write(even_sds, 0x1f, 0x2, 0x31);

	/* ##Page0x2E, Reg0x15[9], REG0_RX_EN_TEST=[1] */
	rtpcs_sds_write_bits(sds, 0x2e, 0x15, 9, 9, 0x1);
	/* ##Page0x21, Reg0x06[11 6], REG0_RX_DEBUG_SEL=[1 0 x x x x] */
	rtpcs_sds_write_bits(sds, 0x21, 0x06, 11, 6, 0x20);
	/* ##Page0x2F, Reg0x0C[5 0], REG0_COEF_SEL=[0 0 1 1 0 0] */
	rtpcs_sds_write_bits(sds, 0x2f, 0x0c, 5, 0, 0xc);

	mdelay(1);

	/* ##VthP & VthN Read Out */
	vth_list[0] = rtpcs_sds_read_bits(sds, 0x1f, 0x14, 2, 0); /* v_thp set bin */
	vth_list[1] = rtpcs_sds_read_bits(sds, 0x1f, 0x14, 5, 3); /* v_thn set bin */

	pr_info("vth_set_bin = %d", vth_list[0]);
	pr_info("vth_set_bin = %d", vth_list[1]);

	vth_manual = !!rtpcs_sds_read_bits(sds, 0x2e, 0x0f, 13, 13);
	pr_info("Vth Maunal = %d", vth_manual);
}

static void rtpcs_930x_sds_rxcal_tap_manual(struct rtpcs_serdes *sds,
					    int tap_id, bool manual, u32 tap_list[])
{
	if (manual) {
		switch (tap_id) {
		case 0:
			/* ##REG0_LOAD_IN_INIT[0]=1; REG0_TAP0_INIT[5:0]=Tap0_Value */
			rtpcs_sds_write_bits(sds, 0x2e, 0x0f, tap_id + 7, tap_id + 7, 0x1);
			rtpcs_sds_write_bits(sds, 0x2f, 0x03, 5, 5, tap_list[0]);
			rtpcs_sds_write_bits(sds, 0x2f, 0x03, 4, 0, tap_list[1]);
			break;
		case 1:
			rtpcs_sds_write_bits(sds, 0x2e, 0x0f, tap_id + 7, tap_id + 7, 0x1);
			rtpcs_sds_write_bits(sds, 0x21, 0x07, 6, 6, tap_list[0]);
			rtpcs_sds_write_bits(sds, 0x2e, 0x09, 11, 6, tap_list[1]);
			rtpcs_sds_write_bits(sds, 0x21, 0x07, 5, 5, tap_list[2]);
			rtpcs_sds_write_bits(sds, 0x2f, 0x12, 5, 0, tap_list[3]);
			break;
		case 2:
			rtpcs_sds_write_bits(sds, 0x2e, 0x0f, tap_id + 7, tap_id + 7, 0x1);
			rtpcs_sds_write_bits(sds, 0x2e, 0x09, 5, 5, tap_list[0]);
			rtpcs_sds_write_bits(sds, 0x2e, 0x09, 4, 0, tap_list[1]);
			rtpcs_sds_write_bits(sds, 0x2e, 0x0a, 11, 11, tap_list[2]);
			rtpcs_sds_write_bits(sds, 0x2e, 0x0a, 10, 6, tap_list[3]);
			break;
		case 3:
			rtpcs_sds_write_bits(sds, 0x2e, 0x0f, tap_id + 7, tap_id + 7, 0x1);
			rtpcs_sds_write_bits(sds, 0x2e, 0x0a, 5, 5, tap_list[0]);
			rtpcs_sds_write_bits(sds, 0x2e, 0x0a, 4, 0, tap_list[1]);
			rtpcs_sds_write_bits(sds, 0x2e, 0x06, 5, 5, tap_list[2]);
			rtpcs_sds_write_bits(sds, 0x2e, 0x06, 4, 0, tap_list[3]);
			break;
		case 4:
			rtpcs_sds_write_bits(sds, 0x2e, 0x0f, tap_id + 7, tap_id + 7, 0x1);
			rtpcs_sds_write_bits(sds, 0x2f, 0x01, 5, 5, tap_list[0]);
			rtpcs_sds_write_bits(sds, 0x2f, 0x01, 4, 0, tap_list[1]);
			rtpcs_sds_write_bits(sds, 0x2e, 0x06, 11, 11, tap_list[2]);
			rtpcs_sds_write_bits(sds, 0x2e, 0x06, 10, 6, tap_list[3]);
			break;
		default:
			break;
		}
	} else {
		rtpcs_sds_write_bits(sds, 0x2e, 0x0f, tap_id + 7, tap_id + 7, 0x0);
		mdelay(10);
	}
}

static void rtpcs_930x_sds_rxcal_tap_get(struct rtpcs_serdes *sds,
					 u32 tap_id, u32 tap_list[])
{
	struct rtpcs_serdes *even_sds = rtpcs_sds_get_even(sds);
	u32 tap0_sign_out;
	u32 tap0_coef_bin;
	u32 tap_sign_out_even;
	u32 tap_coef_bin_even;
	u32 tap_sign_out_odd;
	u32 tap_coef_bin_odd;
	bool tap_manual;

	if (sds == even_sds)
		rtpcs_sds_write(sds, 0x1f, 0x2, 0x2f);
	else
		rtpcs_sds_write(even_sds, 0x1f, 0x2, 0x31);

	/* ##Page0x2E, Reg0x15[9], REG0_RX_EN_TEST=[1] */
	rtpcs_sds_write_bits(sds, 0x2e, 0x15, 9, 9, 0x1);
	/* ##Page0x21, Reg0x06[11 6], REG0_RX_DEBUG_SEL=[1 0 x x x x] */
	rtpcs_sds_write_bits(sds, 0x21, 0x06, 11, 6, 0x20);

	if (!tap_id) {
		/* ##Page0x2F, Reg0x0C[5 0], REG0_COEF_SEL=[0 0 0 0 0 1] */
		rtpcs_sds_write_bits(sds, 0x2f, 0x0c, 5, 0, 0);
		/* ##Tap1 Even Read Out */
		mdelay(1);
		tap0_sign_out = rtpcs_sds_read_bits(sds, 0x1f, 0x14, 5, 5);
		tap0_coef_bin = rtpcs_sds_read_bits(sds, 0x1f, 0x14, 4, 0);

		if (tap0_sign_out == 1)
			pr_info("Tap0 Sign : -");
		else
			pr_info("Tap0 Sign : +");

		pr_info("tap0_coef_bin = %d", tap0_coef_bin);

		tap_list[0] = tap0_sign_out;
		tap_list[1] = tap0_coef_bin;

		tap_manual = !!rtpcs_sds_read_bits(sds, 0x2e, 0x0f, 7, 7);
		pr_info("tap0 manual = %u", tap_manual);
	} else {
		/* ##Page0x2F, Reg0x0C[5 0], REG0_COEF_SEL=[0 0 0 0 0 1] */
		rtpcs_sds_write_bits(sds, 0x2f, 0x0c, 5, 0, tap_id);
		mdelay(1);
		/* ##Tap1 Even Read Out */
		tap_sign_out_even = rtpcs_sds_read_bits(sds, 0x1f, 0x14, 5, 5);
		tap_coef_bin_even = rtpcs_sds_read_bits(sds, 0x1f, 0x14, 4, 0);

		/* ##Page0x2F, Reg0x0C[5 0], REG0_COEF_SEL=[0 0 0 1 1 0] */
		rtpcs_sds_write_bits(sds, 0x2f, 0x0c, 5, 0, (tap_id + 5));
		/* ##Tap1 Odd Read Out */
		tap_sign_out_odd = rtpcs_sds_read_bits(sds, 0x1f, 0x14, 5, 5);
		tap_coef_bin_odd = rtpcs_sds_read_bits(sds, 0x1f, 0x14, 4, 0);

		if (tap_sign_out_even == 1)
			pr_info("Tap %u even sign: -", tap_id);
		else
			pr_info("Tap %u even sign: +", tap_id);

		pr_info("Tap %u even coefficient = %u", tap_id, tap_coef_bin_even);

		if (tap_sign_out_odd == 1)
			pr_info("Tap %u odd sign: -", tap_id);
		else
			pr_info("Tap %u odd sign: +", tap_id);

		pr_info("Tap %u odd coefficient = %u", tap_id, tap_coef_bin_odd);

		tap_list[0] = tap_sign_out_even;
		tap_list[1] = tap_coef_bin_even;
		tap_list[2] = tap_sign_out_odd;
		tap_list[3] = tap_coef_bin_odd;

		tap_manual = rtpcs_sds_read_bits(sds, 0x2e, 0x0f, tap_id + 7, tap_id + 7);
		pr_info("tap %u manual = %d", tap_id, tap_manual);
	}
}

__always_unused
static void rtpcs_930x_sds_do_rx_calibration_1(struct rtpcs_serdes *sds,
					       phy_interface_t phy_mode)
{
	/* From both rtl9300_rxCaliConf_serdes_myParam and rtl9300_rxCaliConf_phy_myParam */
	int tap0_init_val = 0x1f; /* Initial Decision Fed Equalizer 0 tap */
	int vth_min       = 0x0;

	pr_info("start_1.1.1 initial value for sds %d\n", sds->id);
	rtpcs_sds_write(sds, 6,  0, 0);

	/* FGCAL */
	rtpcs_sds_write_bits(sds, 0x2e, 0x01, 14, 14, 0x00);
	rtpcs_sds_write_bits(sds, 0x2e, 0x1c, 10,  5, 0x20);
	rtpcs_sds_write_bits(sds, 0x2f, 0x02,  0,  0, 0x01);

	/* DCVS */
	rtpcs_sds_write_bits(sds, 0x2e, 0x1e, 14, 11, 0x00);
	rtpcs_sds_write_bits(sds, 0x2e, 0x01, 15, 15, 0x00);
	rtpcs_sds_write_bits(sds, 0x2e, 0x02, 11, 11, 0x00);
	rtpcs_sds_write_bits(sds, 0x2e, 0x1c,  4,  0, 0x00);
	rtpcs_sds_write_bits(sds, 0x2e, 0x1d, 15, 11, 0x00);
	rtpcs_sds_write_bits(sds, 0x2e, 0x1d, 10,  6, 0x00);
	rtpcs_sds_write_bits(sds, 0x2e, 0x1d,  5,  1, 0x00);
	rtpcs_sds_write_bits(sds, 0x2e, 0x02, 10,  6, 0x00);
	rtpcs_sds_write_bits(sds, 0x2e, 0x11,  4,  0, 0x00);
	rtpcs_sds_write_bits(sds, 0x2f, 0x00,  3,  0, 0x0f);
	rtpcs_sds_write_bits(sds, 0x2e, 0x04,  6,  6, 0x01);
	rtpcs_sds_write_bits(sds, 0x2e, 0x04,  7,  7, 0x01);

	/* LEQ (Long Term Equivalent signal level) */
	rtpcs_sds_write_bits(sds, 0x2e, 0x16, 14,  8, 0x00);

	/* DFE (Decision Fed Equalizer) */
	rtpcs_sds_write_bits(sds, 0x2f, 0x03,  5,  0, tap0_init_val);
	rtpcs_sds_write_bits(sds, 0x2e, 0x09, 11,  6, 0x00);
	rtpcs_sds_write_bits(sds, 0x2e, 0x09,  5,  0, 0x00);
	rtpcs_sds_write_bits(sds, 0x2e, 0x0a,  5,  0, 0x00);
	rtpcs_sds_write_bits(sds, 0x2f, 0x01,  5,  0, 0x00);
	rtpcs_sds_write_bits(sds, 0x2f, 0x12,  5,  0, 0x00);
	rtpcs_sds_write_bits(sds, 0x2e, 0x0a, 11,  6, 0x00);
	rtpcs_sds_write_bits(sds, 0x2e, 0x06,  5,  0, 0x00);
	rtpcs_sds_write_bits(sds, 0x2f, 0x01,  5,  0, 0x00);

	/* Vth */
	rtpcs_sds_write_bits(sds, 0x2e, 0x13,  5,  3, 0x07);
	rtpcs_sds_write_bits(sds, 0x2e, 0x13,  2,  0, 0x07);
	rtpcs_sds_write_bits(sds, 0x2f, 0x0b,  5,  3, vth_min);

	pr_info("end_1.1.1 --\n");

	pr_info("start_1.1.2 Load DFE init. value\n");

	rtpcs_sds_write_bits(sds, 0x2e, 0x0f, 13,  7, 0x7f);

	pr_info("end_1.1.2\n");

	pr_info("start_1.1.3 disable LEQ training,enable DFE clock\n");

	rtpcs_sds_write_bits(sds, 0x2e, 0x17,  7,  7, 0x00);
	rtpcs_sds_write_bits(sds, 0x2e, 0x17,  6,  2, 0x00);
	rtpcs_sds_write_bits(sds, 0x2e, 0x0c,  8,  8, 0x00);
	rtpcs_sds_write_bits(sds, 0x2e, 0x0b,  4,  4, 0x01);
	rtpcs_sds_write_bits(sds, 0x2e, 0x12, 14, 14, 0x00);
	rtpcs_sds_write_bits(sds, 0x2f, 0x02, 15, 15, 0x00);

	pr_info("end_1.1.3 --\n");

	pr_info("start_1.1.4 offset cali setting\n");

	rtpcs_sds_write_bits(sds, 0x2e, 0x0f, 15, 14, 0x03);

	pr_info("end_1.1.4\n");

	pr_info("start_1.1.5 LEQ and DFE setting\n");

	/* TODO: make this work for DAC cables of different lengths */
	/* For a 10GBit serdes wit Fibre, SDS 8 or 9 */
	if (phy_mode == PHY_INTERFACE_MODE_10GBASER ||
	    phy_mode == PHY_INTERFACE_MODE_1000BASEX ||
	    phy_mode == PHY_INTERFACE_MODE_SGMII)
		rtpcs_sds_write_bits(sds, 0x2e, 0x16,  3,  2, 0x02);
	else
		pr_err("%s not PHY-based or SerDes, implement DAC!\n", __func__);

	/* No serdes, check for Aquantia PHYs */
	rtpcs_sds_write_bits(sds, 0x2e, 0x16,  3,  2, 0x02);

	rtpcs_sds_write_bits(sds, 0x2e, 0x0f,  6,  0, 0x5f);
	rtpcs_sds_write_bits(sds, 0x2f, 0x05,  7,  2, 0x1f);
	rtpcs_sds_write_bits(sds, 0x2e, 0x19,  9,  5, 0x1f);
	rtpcs_sds_write_bits(sds, 0x2f, 0x0b, 15,  9, 0x3c);
	rtpcs_sds_write_bits(sds, 0x2e, 0x0b,  1,  0, 0x03);

	pr_info("end_1.1.5\n");
}

static void rtpcs_930x_sds_do_rx_calibration_2_1(struct rtpcs_serdes *sds)
{
	pr_info("start_1.2.1 ForegroundOffsetCal_Manual\n");

	/* Gray config endis to 1 */
	rtpcs_sds_write_bits(sds, 0x2f, 0x02,  2,  2, 0x01);

	/* ForegroundOffsetCal_Manual(auto mode) */
	rtpcs_sds_write_bits(sds, 0x2e, 0x01, 14, 14, 0x00);

	pr_info("end_1.2.1");
}

static void rtpcs_930x_sds_do_rx_calibration_2_2(struct rtpcs_serdes *sds)
{
	/* Force Rx-Run = 0 */
	rtpcs_sds_write_bits(sds, 0x2e, 0x15, 8, 8, 0x0);

	rtpcs_930x_sds_rx_reset(sds, PHY_INTERFACE_MODE_10GBASER);
}

static void rtpcs_930x_sds_do_rx_calibration_2_3(struct rtpcs_serdes *sds)
{
	struct rtpcs_serdes *even_sds = rtpcs_sds_get_even(sds);
	u32 fgcal_binary, fgcal_gray;
	u32 offset_range;

	pr_info("start_1.2.3 Foreground Calibration\n");

	while (1) {
		if (sds == even_sds)
			rtpcs_sds_write(sds, 0x1f, 0x2, 0x2f);
		else
			rtpcs_sds_write(even_sds, 0x1f, 0x2, 0x31);

		/* ##Page0x2E, Reg0x15[9], REG0_RX_EN_TEST=[1] */
		rtpcs_sds_write_bits(sds, 0x2e, 0x15, 9, 9, 0x1);
		/* ##Page0x21, Reg0x06[11 6], REG0_RX_DEBUG_SEL=[1 0 x x x x] */
		rtpcs_sds_write_bits(sds, 0x21, 0x06, 11, 6, 0x20);
		/* ##Page0x2F, Reg0x0C[5 0], REG0_COEF_SEL=[0 0 1 1 1 1] */
		rtpcs_sds_write_bits(sds, 0x2f, 0x0c, 5, 0, 0xf);
		/* ##FGCAL read gray */
		fgcal_gray = rtpcs_sds_read_bits(sds, 0x1f, 0x14, 5, 0);
		/* ##Page0x2F, Reg0x0C[5 0], REG0_COEF_SEL=[0 0 1 1 1 0] */
		rtpcs_sds_write_bits(sds, 0x2f, 0x0c, 5, 0, 0xe);
		/* ##FGCAL read binary */
		fgcal_binary = rtpcs_sds_read_bits(sds, 0x1f, 0x14, 5, 0);

		pr_info("%s: fgcal_gray: %d, fgcal_binary %d\n",
			__func__, fgcal_gray, fgcal_binary);

		offset_range = rtpcs_sds_read_bits(sds, 0x2e, 0x15, 15, 14);

		if (fgcal_binary <= 60 && fgcal_binary >= 3)
			break;

		if (offset_range == 3) {
			pr_info("%s: Foreground Calibration result marginal!", __func__);
			break;
		}

		offset_range++;
		rtpcs_sds_write_bits(sds, 0x2e, 0x15, 15, 14, offset_range);
		rtpcs_930x_sds_do_rx_calibration_2_2(sds);
	}
	pr_info("%s: end_1.2.3\n", __func__);
}

__always_unused
static void rtpcs_930x_sds_do_rx_calibration_2(struct rtpcs_serdes *sds)
{
	rtpcs_930x_sds_rx_reset(sds, PHY_INTERFACE_MODE_10GBASER);
	rtpcs_930x_sds_do_rx_calibration_2_1(sds);
	rtpcs_930x_sds_do_rx_calibration_2_2(sds);
	rtpcs_930x_sds_do_rx_calibration_2_3(sds);
}

static void rtpcs_930x_sds_rxcal_3_1(struct rtpcs_serdes *sds,
				     phy_interface_t phy_mode)
{
	pr_info("start_1.3.1");

	/* ##1.3.1 */
	if (phy_mode != PHY_INTERFACE_MODE_10GBASER &&
	    phy_mode != PHY_INTERFACE_MODE_1000BASEX &&
	    phy_mode != PHY_INTERFACE_MODE_SGMII)
		rtpcs_sds_write_bits(sds, 0x2e, 0xc, 8, 8, 0);

	rtpcs_sds_write_bits(sds, 0x2e, 0x17, 7, 7, 0x0);
	rtpcs_930x_sds_rxcal_leq_manual(sds, false, 0);

	pr_info("end_1.3.1");
}

static void rtpcs_930x_sds_rxcal_3_2(struct rtpcs_serdes *sds,
				     phy_interface_t phy_mode)
{
	u32 sum10 = 0, avg10, int10;
	int dac_long_cable_offset;
	bool eq_hold_enabled;
	int i;

	if (phy_mode == PHY_INTERFACE_MODE_10GBASER ||
	    phy_mode == PHY_INTERFACE_MODE_1000BASEX ||
	    phy_mode == PHY_INTERFACE_MODE_SGMII) {
		/* rtl9300_rxCaliConf_serdes_myParam */
		dac_long_cable_offset = 3;
		eq_hold_enabled = true;
	} else {
		/* rtl9300_rxCaliConf_phy_myParam */
		dac_long_cable_offset = 0;
		eq_hold_enabled = false;
	}

	if (phy_mode != PHY_INTERFACE_MODE_10GBASER)
		pr_warn("%s: LEQ only valid for 10GR!\n", __func__);

	pr_info("start_1.3.2");

	for (i = 0; i < 10; i++) {
		sum10 += rtpcs_930x_sds_rxcal_leq_read(sds);
		mdelay(10);
	}

	avg10 = (sum10 / 10) + (((sum10 % 10) >= 5) ? 1 : 0);
	int10 = sum10 / 10;

	pr_info("sum10:%u, avg10:%u, int10:%u", sum10, avg10, int10);

	if (phy_mode == PHY_INTERFACE_MODE_10GBASER ||
	    phy_mode == PHY_INTERFACE_MODE_1000BASEX ||
	    phy_mode == PHY_INTERFACE_MODE_SGMII) {
		if (dac_long_cable_offset) {
			rtpcs_930x_sds_rxcal_leq_offset_manual(sds, 1,
							       dac_long_cable_offset);
			rtpcs_sds_write_bits(sds, 0x2e, 0x17, 7, 7,
					     eq_hold_enabled);
			if (phy_mode == PHY_INTERFACE_MODE_10GBASER)
				rtpcs_930x_sds_rxcal_leq_manual(sds,
								true, avg10);
		} else {
			if (sum10 >= 5) {
				rtpcs_930x_sds_rxcal_leq_offset_manual(sds, 1, 3);
				rtpcs_sds_write_bits(sds, 0x2e, 0x17, 7, 7, 0x1);
				if (phy_mode == PHY_INTERFACE_MODE_10GBASER)
					rtpcs_930x_sds_rxcal_leq_manual(sds, true, avg10);
			} else {
				rtpcs_930x_sds_rxcal_leq_offset_manual(sds, 1, 0);
				rtpcs_sds_write_bits(sds, 0x2e, 0x17, 7, 7, 0x1);
				if (phy_mode == PHY_INTERFACE_MODE_10GBASER)
					rtpcs_930x_sds_rxcal_leq_manual(sds, true, avg10);
			}
		}
	}

	pr_info("Sds:%u LEQ = %u", sds->id, rtpcs_930x_sds_rxcal_leq_read(sds));

	pr_info("end_1.3.2");
}

__always_unused
static void rtpcs_930x_sds_do_rx_calibration_3(struct rtpcs_serdes *sds,
					       phy_interface_t phy_mode)
{
	rtpcs_930x_sds_rxcal_3_1(sds, phy_mode);

	if (phy_mode == PHY_INTERFACE_MODE_10GBASER ||
	    phy_mode == PHY_INTERFACE_MODE_1000BASEX ||
	    phy_mode == PHY_INTERFACE_MODE_SGMII)
		rtpcs_930x_sds_rxcal_3_2(sds, phy_mode);
}

static void rtpcs_930x_sds_do_rx_calibration_4_1(struct rtpcs_serdes *sds)
{
	u32 vth_list[2] = {0, 0};
	u32 tap0_list[4] = {0, 0, 0, 0};

	pr_info("start_1.4.1");

	/* ##1.4.1 */
	rtpcs_930x_sds_rxcal_vth_manual(sds, false, vth_list);
	rtpcs_930x_sds_rxcal_tap_manual(sds, 0, false, tap0_list);
	mdelay(200);

	pr_info("end_1.4.1");
}

static void rtpcs_930x_sds_do_rx_calibration_4_2(struct rtpcs_serdes *sds)
{
	u32 vth_list[2];
	u32 tap_list[4];

	pr_info("start_1.4.2");

	rtpcs_930x_sds_rxcal_vth_get(sds, vth_list);
	rtpcs_930x_sds_rxcal_vth_manual(sds, true, vth_list);

	mdelay(100);

	rtpcs_930x_sds_rxcal_tap_get(sds, 0, tap_list);
	rtpcs_930x_sds_rxcal_tap_manual(sds, 0, true, tap_list);

	pr_info("end_1.4.2");
}

static void rtpcs_930x_sds_do_rx_calibration_4(struct rtpcs_serdes *sds)
{
	rtpcs_930x_sds_do_rx_calibration_4_1(sds);
	rtpcs_930x_sds_do_rx_calibration_4_2(sds);
}

static void rtpcs_930x_sds_do_rx_calibration_5_2(struct rtpcs_serdes *sds)
{
	u32 tap1_list[4] = {0};
	u32 tap2_list[4] = {0};
	u32 tap3_list[4] = {0};
	u32 tap4_list[4] = {0};

	pr_info("start_1.5.2");

	rtpcs_930x_sds_rxcal_tap_manual(sds, 1, false, tap1_list);
	rtpcs_930x_sds_rxcal_tap_manual(sds, 2, false, tap2_list);
	rtpcs_930x_sds_rxcal_tap_manual(sds, 3, false, tap3_list);
	rtpcs_930x_sds_rxcal_tap_manual(sds, 4, false, tap4_list);

	mdelay(30);

	pr_info("end_1.5.2");
}

static void rtpcs_930x_sds_do_rx_calibration_5(struct rtpcs_serdes *sds,
					       phy_interface_t phy_mode)
{
	if (phy_mode == PHY_INTERFACE_MODE_10GBASER) /* dfeTap1_4Enable true */
		rtpcs_930x_sds_do_rx_calibration_5_2(sds);
}

static void rtpcs_930x_sds_do_rx_calibration_dfe_disable(struct rtpcs_serdes *sds)
{
	u32 tap1_list[4] = {0};
	u32 tap2_list[4] = {0};
	u32 tap3_list[4] = {0};
	u32 tap4_list[4] = {0};

	rtpcs_930x_sds_rxcal_tap_manual(sds, 1, true, tap1_list);
	rtpcs_930x_sds_rxcal_tap_manual(sds, 2, true, tap2_list);
	rtpcs_930x_sds_rxcal_tap_manual(sds, 3, true, tap3_list);
	rtpcs_930x_sds_rxcal_tap_manual(sds, 4, true, tap4_list);

	mdelay(10);
}

static void rtpcs_930x_sds_do_rx_calibration(struct rtpcs_serdes *sds,
					     phy_interface_t phy_mode)
{
	u32 latch_sts;

	rtpcs_930x_sds_do_rx_calibration_1(sds, phy_mode);
	rtpcs_930x_sds_do_rx_calibration_2(sds);
	rtpcs_930x_sds_do_rx_calibration_4(sds);
	rtpcs_930x_sds_do_rx_calibration_5(sds, phy_mode);
	mdelay(20);

	/* Do this only for 10GR mode, SDS active in mode 0x1a */
	if (rtpcs_sds_read_bits(sds, 0x1f, 9, 11, 7) == RTL930X_SDS_MODE_10GBASER) {
		pr_info("%s: SDS enabled\n", __func__);
		latch_sts = rtpcs_sds_read_bits(sds, 0x4, 1, 2, 2);
		mdelay(1);
		latch_sts = rtpcs_sds_read_bits(sds, 0x4, 1, 2, 2);
		if (latch_sts) {
			rtpcs_930x_sds_do_rx_calibration_dfe_disable(sds);
			rtpcs_930x_sds_do_rx_calibration_4(sds);
			rtpcs_930x_sds_do_rx_calibration_5(sds, phy_mode);
		}
	}
}

static int rtpcs_930x_sds_sym_err_reset(struct rtpcs_serdes *sds,
					phy_interface_t phy_mode)
{
	switch (phy_mode) {
	case PHY_INTERFACE_MODE_XGMII:
		break;

	case PHY_INTERFACE_MODE_10GBASER:
		/* Read twice to clear */
		rtpcs_sds_read(sds, 5, 1);
		rtpcs_sds_read(sds, 5, 1);
		break;

	case PHY_INTERFACE_MODE_1000BASEX:
	case PHY_INTERFACE_MODE_SGMII:
	case PHY_INTERFACE_MODE_10G_QXGMII:
		rtpcs_sds_write_bits(sds, 0x1, 24, 2, 0, 0);
		rtpcs_sds_write_bits(sds, 0x1, 3, 15, 8, 0);
		rtpcs_sds_write_bits(sds, 0x1, 2, 15, 0, 0);
		break;

	default:
		pr_info("%s unsupported phy mode\n", __func__);
		return -1;
	}

	return 0;
}

static u32 rtpcs_930x_sds_sym_err_get(struct rtpcs_serdes *sds,
				      phy_interface_t phy_mode)
{
	u32 v = 0;

	switch (phy_mode) {
	case PHY_INTERFACE_MODE_XGMII:
	case PHY_INTERFACE_MODE_10G_QXGMII:
		break;

	case PHY_INTERFACE_MODE_1000BASEX:
	case PHY_INTERFACE_MODE_SGMII:
	case PHY_INTERFACE_MODE_10GBASER:
		v = rtpcs_sds_read(sds, 5, 1);
		return v & 0xff;

	default:
		pr_info("%s unsupported PHY-mode\n", __func__);
	}

	return v;
}

static int rtpcs_930x_sds_check_calibration(struct rtpcs_serdes *sds,
					    phy_interface_t phy_mode)
{
	u32 errors1, errors2;

	rtpcs_930x_sds_sym_err_reset(sds, phy_mode);
	rtpcs_930x_sds_sym_err_reset(sds, phy_mode);

	/* Count errors during 1ms */
	errors1 = rtpcs_930x_sds_sym_err_get(sds, phy_mode);
	mdelay(1);
	errors2 = rtpcs_930x_sds_sym_err_get(sds, phy_mode);

	switch (phy_mode) {
	case PHY_INTERFACE_MODE_1000BASEX:
	case PHY_INTERFACE_MODE_SGMII:
	case PHY_INTERFACE_MODE_XGMII:
		if ((errors2 - errors1 > 100) ||
		    (errors1 >= 0xffff00) || (errors2 >= 0xffff00)) {
			pr_info("%s XSGMII error rate too high\n", __func__);
			return 1;
		}
		break;
	case PHY_INTERFACE_MODE_10GBASER:
	case PHY_INTERFACE_MODE_10G_QXGMII:
		if (errors2 > 0) {
			pr_info("%s: %s error rate too high\n", __func__, phy_modes(phy_mode));
			return 1;
		}
		break;
	default:
		return 1;
	}

	return 0;
}

static void rtpcs_930x_phy_enable_10g_1g(struct rtpcs_serdes *sds)
{

	u32 v;

	/* Enable 1GBit PHY */
	v = rtpcs_sds_read(sds, PHY_PAGE_2, MII_BMCR);
	pr_info("%s 1gbit phy: %08x\n", __func__, v);
	v &= ~BMCR_PDOWN;
	rtpcs_sds_write(sds, PHY_PAGE_2, MII_BMCR, v);
	pr_info("%s 1gbit phy enabled: %08x\n", __func__, v);

	/* Enable 10GBit PHY */
	v = rtpcs_sds_read(sds, PHY_PAGE_4, MII_BMCR);
	pr_info("%s 10gbit phy: %08x\n", __func__, v);
	v &= ~BMCR_PDOWN;
	rtpcs_sds_write(sds, PHY_PAGE_4, MII_BMCR, v);
	pr_info("%s 10gbit phy after: %08x\n", __func__, v);

	/* dal_longan_construct_mac_default_10gmedia_fiber */
	v = rtpcs_sds_read(sds, 0x1f, 11);
	pr_info("%s set medium: %08x\n", __func__, v);
	v |= BIT(1);
	rtpcs_sds_write(sds, 0x1f, 11, v);
	pr_info("%s set medium after: %08x\n", __func__, v);
}

static int rtpcs_930x_sds_10g_idle(struct rtpcs_serdes *sds)
{
	struct rtpcs_serdes *even_sds = rtpcs_sds_get_even(sds);
	bool busy;
	int i = 0;

	do {
		if (sds == even_sds) {
			rtpcs_sds_write_bits(sds, 0x1f, 0x2, 15, 0, 53);
			busy = !!rtpcs_sds_read_bits(sds, 0x1f, 0x14, 0, 0);
		} else {
			rtpcs_sds_write_bits(even_sds, 0x1f, 0x2, 15, 0, 53);
			busy = !!rtpcs_sds_read_bits(even_sds, 0x1f, 0x14, 1, 1);
		}
		i++;
	} while (busy && i < 100);

	if (i < 100)
		return 0;

	pr_warn("%s WARNING: Waiting for RX idle timed out, SDS %d\n",
		__func__, sds->id);
	return -EIO;
}

static int rtpcs_930x_sds_set_polarity(struct rtpcs_serdes *sds,
				       bool tx_inv, bool rx_inv)
{
	u8 rx_val = rx_inv ? 1 : 0;
	u8 tx_val = tx_inv ? 1 : 0;
	u32 val;
	int ret;

	/* 10GR */
	val = (tx_val << 1) | rx_val;
	ret = rtpcs_sds_write_bits(sds, 0x6, 0x2, 14, 13, val);
	if (ret)
		return ret;

	/* 1G */
	val = (rx_val << 1) | tx_val;
	return rtpcs_sds_write_bits(sds, 0x0, 0x0, 9, 8, val);
}

static const sds_config rtpcs_930x_sds_cfg_10gr_even[] = {
	/* 1G */
	{0x00, 0x0E, 0x3053}, {0x01, 0x14, 0x0100}, {0x21, 0x03, 0x8206},
	{0x21, 0x05, 0x40B0}, {0x21, 0x06, 0x0010}, {0x21, 0x07, 0xF09F},
	{0x21, 0x0C, 0x0007}, {0x21, 0x0D, 0x6009}, {0x21, 0x0E, 0x0000},
	{0x21, 0x0F, 0x0008}, {0x24, 0x00, 0x0668}, {0x24, 0x02, 0xD020},
	{0x24, 0x06, 0xC000}, {0x24, 0x0B, 0x1892}, {0x24, 0x0F, 0xFFDF},
	{0x24, 0x12, 0x03C4}, {0x24, 0x13, 0x027F}, {0x24, 0x14, 0x1311},
	{0x24, 0x16, 0x00C9}, {0x24, 0x17, 0xA100}, {0x24, 0x1A, 0x0001},
	{0x24, 0x1C, 0x0400}, {0x25, 0x01, 0x0300}, {0x25, 0x02, 0x1017},
	{0x25, 0x03, 0xFFDF}, {0x25, 0x05, 0x7F7C}, {0x25, 0x07, 0x8100},
	{0x25, 0x08, 0x0001}, {0x25, 0x09, 0xFFD4}, {0x25, 0x0A, 0x7C2F},
	{0x25, 0x0E, 0x003F}, {0x25, 0x0F, 0x0121}, {0x25, 0x10, 0x0020},
	{0x25, 0x11, 0x8840}, {0x2B, 0x13, 0x0050}, {0x2B, 0x18, 0x8E88},
	{0x2B, 0x19, 0x4902}, {0x2B, 0x1D, 0x2501}, {0x2D, 0x13, 0x0050},
	{0x2D, 0x18, 0x8E88}, {0x2D, 0x19, 0x4902}, {0x2D, 0x1D, 0x2641},
	{0x2F, 0x13, 0x0050}, {0x2F, 0x18, 0x8E88}, {0x2F, 0x19, 0x4902},
	{0x2F, 0x1D, 0x66E1},
	/* 3.125G */
	{0x28, 0x00, 0x0668}, {0x28, 0x02, 0xD020}, {0x28, 0x06, 0xC000},
	{0x28, 0x0B, 0x1892}, {0x28, 0x0F, 0xFFDF}, {0x28, 0x12, 0x01C4},
	{0x28, 0x13, 0x027F}, {0x28, 0x14, 0x1311}, {0x28, 0x16, 0x00C9},
	{0x28, 0x17, 0xA100}, {0x28, 0x1A, 0x0001}, {0x28, 0x1C, 0x0400},
	{0x29, 0x01, 0x0300}, {0x29, 0x02, 0x1017}, {0x29, 0x03, 0xFFDF},
	{0x29, 0x05, 0x7F7C}, {0x29, 0x07, 0x8100}, {0x29, 0x08, 0x0001},
	{0x29, 0x09, 0xFFD4}, {0x29, 0x0A, 0x7C2F}, {0x29, 0x0E, 0x003F},
	{0x29, 0x0F, 0x0121}, {0x29, 0x10, 0x0020}, {0x29, 0x11, 0x8840},
	/* 10G */
	{0x06, 0x0D, 0x0F00}, {0x06, 0x00, 0x0000}, {0x06, 0x01, 0xC800},
	{0x21, 0x03, 0x8206}, {0x21, 0x05, 0x40B0}, {0x21, 0x06, 0x0010},
	{0x21, 0x07, 0xF09F}, {0x21, 0x0C, 0x0007}, {0x21, 0x0D, 0x6009},
	{0x21, 0x0E, 0x0000}, {0x21, 0x0F, 0x0008}, {0x2E, 0x00, 0xA668},
	{0x2E, 0x02, 0xD020}, {0x2E, 0x06, 0xC000}, {0x2E, 0x0B, 0x1892},
	{0x2E, 0x0F, 0xFFDF}, {0x2E, 0x11, 0x8280}, {0x2E, 0x12, 0x0044},
	{0x2E, 0x13, 0x027F}, {0x2E, 0x14, 0x1311}, {0x2E, 0x17, 0xA100},
	{0x2E, 0x1A, 0x0001}, {0x2E, 0x1C, 0x0400}, {0x2F, 0x01, 0x0300},
	{0x2F, 0x02, 0x1217}, {0x2F, 0x03, 0xFFDF}, {0x2F, 0x05, 0x7F7C},
	{0x2F, 0x07, 0x80C4}, {0x2F, 0x08, 0x0001}, {0x2F, 0x09, 0xFFD4},
	{0x2F, 0x0A, 0x7C2F}, {0x2F, 0x0E, 0x003F}, {0x2F, 0x0F, 0x0121},
	{0x2F, 0x10, 0x0020}, {0x2F, 0x11, 0x8840}, {0x2F, 0x14, 0xE008},
	{0x2B, 0x13, 0x0050}, {0x2B, 0x18, 0x8E88}, {0x2B, 0x19, 0x4902},
	{0x2B, 0x1D, 0x2501}, {0x2D, 0x13, 0x0050}, {0x2D, 0x17, 0x4109},
	{0x2D, 0x18, 0x8E88}, {0x2D, 0x19, 0x4902}, {0x2D, 0x1C, 0x1109},
	{0x2D, 0x1D, 0x2641}, {0x2F, 0x13, 0x0050}, {0x2F, 0x18, 0x8E88},
	{0x2F, 0x19, 0x4902}, {0x2F, 0x1D, 0x76E1},
};

static const sds_config rtpcs_930x_sds_cfg_10gr_odd[] = {
	/* 1G */
	{0x00, 0x0E, 0x3053}, {0x01, 0x14, 0x0100}, {0x21, 0x03, 0x8206},
	{0x21, 0x06, 0x0010}, {0x21, 0x07, 0xF09F}, {0x21, 0x0A, 0x0003},
	{0x21, 0x0B, 0x0005}, {0x21, 0x0C, 0x0007}, {0x21, 0x0D, 0x6009},
	{0x21, 0x0E, 0x0000}, {0x21, 0x0F, 0x0008}, {0x24, 0x00, 0x0668},
	{0x24, 0x02, 0xD020}, {0x24, 0x06, 0xC000}, {0x24, 0x0B, 0x1892},
	{0x24, 0x0F, 0xFFDF}, {0x24, 0x12, 0x03C4}, {0x24, 0x13, 0x027F},
	{0x24, 0x14, 0x1311}, {0x24, 0x16, 0x00C9}, {0x24, 0x17, 0xA100},
	{0x24, 0x1A, 0x0001}, {0x24, 0x1C, 0x0400}, {0x25, 0x00, 0x820F},
	{0x25, 0x01, 0x0300}, {0x25, 0x02, 0x1017}, {0x25, 0x03, 0xFFDF},
	{0x25, 0x05, 0x7F7C}, {0x25, 0x07, 0x8100}, {0x25, 0x08, 0x0001},
	{0x25, 0x09, 0xFFD4}, {0x25, 0x0A, 0x7C2F}, {0x25, 0x0E, 0x003F},
	{0x25, 0x0F, 0x0121}, {0x25, 0x10, 0x0020}, {0x25, 0x11, 0x8840},
	{0x2B, 0x13, 0x3D87}, {0x2B, 0x14, 0x3108}, {0x2D, 0x13, 0x3C87},
	{0x2D, 0x14, 0x1808},
	/* 3.125G */
	{0x28, 0x00, 0x0668}, {0x28, 0x02, 0xD020}, {0x28, 0x06, 0xC000},
	{0x28, 0x0B, 0x1892}, {0x28, 0x0F, 0xFFDF}, {0x28, 0x12, 0x01C4},
	{0x28, 0x13, 0x027F}, {0x28, 0x14, 0x1311}, {0x28, 0x16, 0x00C9},
	{0x28, 0x17, 0xA100}, {0x28, 0x1A, 0x0001}, {0x28, 0x1C, 0x0400},
	{0x29, 0x00, 0x820F}, {0x29, 0x01, 0x0300}, {0x29, 0x02, 0x1017},
	{0x29, 0x03, 0xFFDF}, {0x29, 0x05, 0x7F7C}, {0x29, 0x07, 0x8100},
	{0x29, 0x08, 0x0001}, {0x29, 0x0A, 0x7C2F}, {0x29, 0x0E, 0x003F},
	{0x29, 0x0F, 0x0121}, {0x29, 0x10, 0x0020}, {0x29, 0x11, 0x8840},
	/* 10G */
	{0x06, 0x0D, 0x0F00}, {0x06, 0x00, 0x0000}, {0x06, 0x01, 0xC800},
	{0x21, 0x03, 0x8206}, {0x21, 0x05, 0x40B0}, {0x21, 0x06, 0x0010},
	{0x21, 0x07, 0xF09F}, {0x21, 0x0A, 0x0003}, {0x21, 0x0B, 0x0005},
	{0x21, 0x0C, 0x0007}, {0x21, 0x0D, 0x6009}, {0x21, 0x0E, 0x0000},
	{0x21, 0x0F, 0x0008}, {0x2E, 0x00, 0xA668}, {0x2E, 0x02, 0xD020},
	{0x2E, 0x06, 0xC000}, {0x2E, 0x0B, 0x1892}, {0x2E, 0x0F, 0xFFDF},
	{0x2E, 0x11, 0x8280}, {0x2E, 0x12, 0x0044}, {0x2E, 0x13, 0x027F},
	{0x2E, 0x14, 0x1311}, {0x2E, 0x17, 0xA100}, {0x2E, 0x1A, 0x0001},
	{0x2E, 0x1C, 0x0400}, {0x2F, 0x00, 0x820F}, {0x2F, 0x01, 0x0300},
	{0x2F, 0x02, 0x1217}, {0x2F, 0x03, 0xFFDF}, {0x2F, 0x05, 0x7F7C},
	{0x2F, 0x07, 0x80C4}, {0x2F, 0x08, 0x0001}, {0x2F, 0x09, 0xFFD4},
	{0x2F, 0x0A, 0x7C2F}, {0x2F, 0x0E, 0x003F}, {0x2F, 0x0F, 0x0121},
	{0x2F, 0x10, 0x0020}, {0x2F, 0x11, 0x8840}, {0x2B, 0x13, 0x3D87},
	{0x2B, 0x14, 0x3108}, {0x2D, 0x13, 0x3C87}, {0x2D, 0x14, 0x1808},
};

static const sds_config rtpcs_930x_sds_cfg_10g_2500bx_even[] = {
	{0x00, 0x0E, 0x3053}, {0x01, 0x14, 0x0100},
	{0x21, 0x03, 0x8206}, {0x21, 0x05, 0x40B0}, {0x21, 0x06, 0x0010}, {0x21, 0x07, 0xF09F},
	{0x21, 0x0C, 0x0007}, {0x21, 0x0D, 0x6009}, {0x21, 0x0E, 0x0000}, {0x21, 0x0F, 0x0008},
	{0x24, 0x00, 0x0668}, {0x24, 0x02, 0xD020}, {0x24, 0x06, 0xC000}, {0x24, 0x0B, 0x1892},
	{0x24, 0x0F, 0xFFDF}, {0x24, 0x12, 0x03C4}, {0x24, 0x13, 0x027F}, {0x24, 0x14, 0x1311},
	{0x24, 0x16, 0x00C9}, {0x24, 0x17, 0xA100}, {0x24, 0x1A, 0x0001}, {0x24, 0x1C, 0x0400},
	{0x25, 0x01, 0x0300}, {0x25, 0x02, 0x1017}, {0x25, 0x03, 0xFFDF}, {0x25, 0x05, 0x7F7C},
	{0x25, 0x07, 0x8100}, {0x25, 0x08, 0x0001}, {0x25, 0x09, 0xFFD4}, {0x25, 0x0A, 0x7C2F},
	{0x25, 0x0E, 0x003F}, {0x25, 0x0F, 0x0121}, {0x25, 0x10, 0x0020}, {0x25, 0x11, 0x8840},
	{0x28, 0x00, 0x0668}, {0x28, 0x02, 0xD020}, {0x28, 0x06, 0xC000}, {0x28, 0x0B, 0x1892},
	{0x28, 0x0F, 0xFFDF}, {0x28, 0x12, 0x01C4}, {0x28, 0x13, 0x027F}, {0x28, 0x14, 0x1311},
	{0x28, 0x16, 0x00C9}, {0x28, 0x17, 0xA100}, {0x28, 0x1A, 0x0001}, {0x28, 0x1C, 0x0400},
	{0x29, 0x01, 0x0300}, {0x29, 0x02, 0x1017}, {0x29, 0x03, 0xFFDF}, {0x29, 0x05, 0x7F7C},
	{0x29, 0x07, 0x8100}, {0x29, 0x08, 0x0001}, {0x29, 0x09, 0xFFD4}, {0x29, 0x0A, 0x7C2F},
	{0x29, 0x0E, 0x003F}, {0x29, 0x0F, 0x0121}, {0x29, 0x10, 0x0020}, {0x29, 0x11, 0x8840},
	{0x2B, 0x13, 0x0050}, {0x2B, 0x18, 0x8E88}, {0x2B, 0x19, 0x4902}, {0x2B, 0x1D, 0x2501},
	{0x2D, 0x13, 0x0050}, {0x2D, 0x18, 0x8E88}, {0x2D, 0x17, 0x4109}, {0x2D, 0x19, 0x4902},
	{0x2D, 0x1C, 0x1109}, {0x2D, 0x1D, 0x2641},
	{0x2F, 0x13, 0x0050}, {0x2F, 0x18, 0x8E88}, {0x2F, 0x19, 0x4902}, {0x2F, 0x1D, 0x66E1},
};

static const sds_config rtpcs_930x_sds_cfg_10g_2500bx_odd[] = {
	{0x00, 0x0E, 0x3053}, {0x01, 0x14, 0x0100},
	{0x21, 0x03, 0x8206}, {0x21, 0x06, 0x0010}, {0x21, 0x07, 0xF09F}, {0x21, 0x0A, 0x0003},
	{0x21, 0x0B, 0x0005}, {0x21, 0x0C, 0x0007}, {0x21, 0x0D, 0x6009}, {0x21, 0x0E, 0x0000},
	{0x21, 0x0F, 0x0008},
	{0x24, 0x00, 0x0668}, {0x24, 0x02, 0xD020}, {0x24, 0x06, 0xC000}, {0x24, 0x0B, 0x1892},
	{0x24, 0x0F, 0xFFDF}, {0x24, 0x12, 0x03C4}, {0x24, 0x13, 0x027F}, {0x24, 0x14, 0x1311},
	{0x24, 0x16, 0x00C9}, {0x24, 0x17, 0xA100}, {0x24, 0x1A, 0x0001}, {0x24, 0x1C, 0x0400},
	{0x25, 0x00, 0x820F}, {0x25, 0x01, 0x0300}, {0x25, 0x02, 0x1017}, {0x25, 0x03, 0xFFDF},
	{0x25, 0x05, 0x7F7C}, {0x25, 0x07, 0x8100}, {0x25, 0x08, 0x0001}, {0x25, 0x09, 0xFFD4},
	{0x25, 0x0A, 0x7C2F}, {0x25, 0x0E, 0x003F}, {0x25, 0x0F, 0x0121}, {0x25, 0x10, 0x0020},
	{0x25, 0x11, 0x8840},
	{0x28, 0x00, 0x0668}, {0x28, 0x02, 0xD020}, {0x28, 0x06, 0xC000}, {0x28, 0x0B, 0x1892},
	{0x28, 0x0F, 0xFFDF}, {0x28, 0x12, 0x01C4}, {0x28, 0x13, 0x027F}, {0x28, 0x14, 0x1311},
	{0x28, 0x16, 0x00C9}, {0x28, 0x17, 0xA100}, {0x28, 0x1A, 0x0001}, {0x28, 0x1C, 0x0400},
	{0x29, 0x00, 0x820F}, {0x29, 0x01, 0x0300}, {0x29, 0x02, 0x1017}, {0x29, 0x03, 0xFFDF},
	{0x29, 0x05, 0x7F7C}, {0x29, 0x07, 0x8100}, {0x29, 0x08, 0x0001}, {0x29, 0x0A, 0x7C2F},
	{0x29, 0x0E, 0x003F}, {0x29, 0x0F, 0x0121}, {0x29, 0x10, 0x0020}, {0x29, 0x11, 0x8840},
	{0x2B, 0x13, 0x3D87}, {0x2B, 0x14, 0x3108},
	{0x2D, 0x13, 0x3C87}, {0x2D, 0x14, 0x1808},
};

static void rtpcs_930x_sds_usxgmii_config(struct rtpcs_serdes *sds, int nway_en,
					  u32 opcode, u32 am_period,
					  u32 all_am_markers, u32 an_table,
					  u32 sync_bit)
{
	rtpcs_sds_write_bits(sds, 0x7, 0x11, 0, 0, nway_en);
	rtpcs_sds_write_bits(sds, 0x7, 0x11, 1, 1, nway_en);
	rtpcs_sds_write_bits(sds, 0x7, 0x11, 2, 2, nway_en);
	rtpcs_sds_write_bits(sds, 0x7, 0x11, 3, 3, nway_en);
	rtpcs_sds_write_bits(sds, 0x6, 0x12, 15, 0, am_period);
	rtpcs_sds_write_bits(sds, 0x6, 0x13, 7,  0, all_am_markers);
	rtpcs_sds_write_bits(sds, 0x6, 0x13, 15, 8, all_am_markers);
	rtpcs_sds_write_bits(sds, 0x6, 0x14, 7,  0, all_am_markers);
	rtpcs_sds_write_bits(sds, 0x6, 0x14, 15, 8, all_am_markers);
	rtpcs_sds_write_bits(sds, 0x6, 0x15, 7,  0, all_am_markers);
	rtpcs_sds_write_bits(sds, 0x6, 0x15, 15, 8, all_am_markers);
	rtpcs_sds_write_bits(sds, 0x6, 0x16, 7,  0, all_am_markers);
	rtpcs_sds_write_bits(sds, 0x6, 0x16, 15, 8, all_am_markers);
	rtpcs_sds_write_bits(sds, 0x6, 0x17, 7,  0, all_am_markers);
	rtpcs_sds_write_bits(sds, 0x6, 0x17, 15, 8, all_am_markers);
	rtpcs_sds_write_bits(sds, 0x6, 0x18, 7,  0, all_am_markers);
	rtpcs_sds_write_bits(sds, 0x6, 0x18, 15, 8, all_am_markers);
	rtpcs_sds_write_bits(sds, 0x7, 0x10, 7, 0, opcode);
	rtpcs_sds_write_bits(sds, 0x6, 0xe, 10, 10, an_table);
	rtpcs_sds_write_bits(sds, 0x6, 0x1d, 11, 10, sync_bit);
}

static void rtpcs_930x_sds_patch(struct rtpcs_serdes *sds, phy_interface_t mode)
{
	struct rtpcs_serdes *even_sds = rtpcs_sds_get_even(sds);
	const sds_config *config;
	bool is_even_sds;
	size_t count;

	is_even_sds = (sds == even_sds);

	switch (mode) {
	case PHY_INTERFACE_MODE_1000BASEX:
	case PHY_INTERFACE_MODE_SGMII:
	case PHY_INTERFACE_MODE_10GBASER:
		if (is_even_sds) {
			config = rtpcs_930x_sds_cfg_10gr_even;
			count = ARRAY_SIZE(rtpcs_930x_sds_cfg_10gr_even);
		} else {
			config = rtpcs_930x_sds_cfg_10gr_odd;
			count = ARRAY_SIZE(rtpcs_930x_sds_cfg_10gr_odd);
		}
		break;

	case PHY_INTERFACE_MODE_2500BASEX:
		if (is_even_sds) {
			config = rtpcs_930x_sds_cfg_10g_2500bx_even;
			count = ARRAY_SIZE(rtpcs_930x_sds_cfg_10g_2500bx_even);
		} else {
			config = rtpcs_930x_sds_cfg_10g_2500bx_odd;
			count = ARRAY_SIZE(rtpcs_930x_sds_cfg_10g_2500bx_odd);
		}
		break;

	case PHY_INTERFACE_MODE_10G_QXGMII:
		return;

	default:
		pr_warn("%s: unsupported mode %s on serdes %d\n", __func__, phy_modes(mode),
			sds->id);
		return;
	}

	for (size_t i = 0; i < count; ++i)
		rtpcs_sds_write(sds, config[i].page, config[i].reg, config[i].data);

	if (mode == PHY_INTERFACE_MODE_10G_QXGMII) {
		/* Default configuration */
		rtpcs_930x_sds_usxgmii_config(sds, 1, 0xaa, 0x5078, 0, 1, 0x1);
	}
}

__always_unused
static int rtpcs_930x_sds_cmu_band_get(struct rtpcs_serdes *sds)
{
	struct rtpcs_serdes *even_sds = rtpcs_sds_get_even(sds);
	struct rtpcs_serdes *odd_sds = rtpcs_sds_get_odd(sds);
	u32 page;
	u32 en;
	u32 cmu_band;

/*	page = rtl9300_sds_cmu_page_get(sds); */
	page = 0x25; /* 10GR and 1000BX */

	rtpcs_sds_write_bits(even_sds, page, 0x1c, 15, 15, 1);
	rtpcs_sds_write_bits(odd_sds, page, 0x1c, 15, 15, 1);

	en = rtpcs_sds_read_bits(even_sds, page, 27, 1, 1);
	if (!en) { /* Auto mode */
		rtpcs_sds_write(even_sds, 0x1f, 0x02, 31);

		cmu_band = rtpcs_sds_read_bits(even_sds, 0x1f, 0x15, 5, 1);
	} else {
		cmu_band = rtpcs_sds_read_bits(even_sds, page, 30, 4, 0);
	}

	return cmu_band;
}

static int rtpcs_930x_setup_serdes(struct rtpcs_serdes *sds,
				   phy_interface_t phy_mode)
{
	int calib_tries = 0;

	/* Rely on setup from U-boot for some modes, e.g. USXGMII */
	switch (phy_mode) {
	case PHY_INTERFACE_MODE_1000BASEX:
	case PHY_INTERFACE_MODE_SGMII:
	case PHY_INTERFACE_MODE_2500BASEX:
	case PHY_INTERFACE_MODE_10GBASER:
	case PHY_INTERFACE_MODE_10G_QXGMII:
		break;
	default:
		return 0;
	}

	/* Turn Off Serdes */
	rtpcs_930x_sds_set(sds, RTL930X_SDS_OFF);

	/* Apply serdes patches */
	rtpcs_930x_sds_patch(sds, phy_mode);

	/* Maybe use dal_longan_sds_init */

	/* dal_longan_construct_serdesConfig_init */ /* Serdes Construct */
	rtpcs_930x_phy_enable_10g_1g(sds);

	/* ----> dal_longan_sds_mode_set */
	pr_info("%s: Configuring RTL9300 SERDES %d\n", __func__, sds->id);

	/* Set SDS polarity */
	rtpcs_930x_sds_set_polarity(sds, sds->tx_pol_inv, sds->rx_pol_inv);

	/* Enable SDS in desired mode */
	rtpcs_930x_sds_mode_set(sds, phy_mode);

	/* Enable Fiber RX */
	rtpcs_sds_write_bits(sds, 0x20, 2, 12, 12, 0);

	/* Calibrate SerDes receiver in loopback mode */
	rtpcs_930x_sds_10g_idle(sds);
	do {
		rtpcs_930x_sds_do_rx_calibration(sds, phy_mode);
		calib_tries++;
		mdelay(50);
	} while (rtpcs_930x_sds_check_calibration(sds, phy_mode) && calib_tries < 3);
	if (calib_tries >= 3)
		pr_warn("%s: SerDes RX calibration failed\n", __func__);

	/* Leave loopback mode */
	rtpcs_930x_sds_tx_config(sds, phy_mode);

	return 0;
}

/* RTL931X */

static void rtpcs_931x_sds_reset(struct rtpcs_serdes *sds)
{
	struct rtpcs_ctrl *ctrl = sds->ctrl;
	u32 sds_id = sds->id;
	u32 o, v, o_mode;
	int shift = ((sds_id & 0x3) << 3);

	/* TODO: We need to lock this! */

	regmap_read(ctrl->map, RTL931X_PS_SERDES_OFF_MODE_CTRL_ADDR, &o);
	v = o | BIT(sds_id);
	regmap_write(ctrl->map, RTL931X_PS_SERDES_OFF_MODE_CTRL_ADDR, v);

	regmap_read(ctrl->map, RTL931X_SERDES_MODE_CTRL + 4 * (sds_id >> 2), &o_mode);
	v = BIT(7) | 0x1F;
	regmap_write_bits(ctrl->map, RTL931X_SERDES_MODE_CTRL + 4 * (sds_id >> 2),
			  0xff << shift, v << shift);
	regmap_write(ctrl->map, RTL931X_SERDES_MODE_CTRL + 4 * (sds_id >> 2), o_mode);

	regmap_write(ctrl->map, RTL931X_PS_SERDES_OFF_MODE_CTRL_ADDR, o);
}

static void rtpcs_931x_sds_disable(struct rtpcs_serdes *sds)
{
	regmap_write(sds->ctrl->map,
		     RTL931X_SERDES_MODE_CTRL + (sds->id >> 2) * 4, 0x9f);
}

static void rtpcs_931x_sds_symerr_clear(struct rtpcs_serdes *sds,
					phy_interface_t mode)
{
	switch (mode) {
	case PHY_INTERFACE_MODE_NA:
		break;
	case PHY_INTERFACE_MODE_XGMII:
		for (int i = 0; i < 4; ++i) {
			rtpcs_sds_write_bits(sds, 0x41, 24,  2, 0, i);
			rtpcs_sds_write_bits(sds, 0x41,  3, 15, 8, 0x0);
			rtpcs_sds_write_bits(sds, 0x41,  2, 15, 0, 0x0);
		}

		for (int i = 0; i < 4; ++i) {
			rtpcs_sds_write_bits(sds, 0x81, 24,  2, 0, i);
			rtpcs_sds_write_bits(sds, 0x81,  3, 15, 8, 0x0);
			rtpcs_sds_write_bits(sds, 0x81,  2, 15, 0, 0x0);
		}

		rtpcs_sds_write_bits(sds, 0x41, 0, 15, 0, 0x0);
		rtpcs_sds_write_bits(sds, 0x41, 1, 15, 8, 0x0);
		rtpcs_sds_write_bits(sds, 0x81, 0, 15, 0, 0x0);
		rtpcs_sds_write_bits(sds, 0x81, 1, 15, 8, 0x0);
		break;
	default:
		break;
	}
}

__always_unused
static void rtpcs_931x_sds_fiber_disable(struct rtpcs_serdes *sds)
{
	u32 v = 0x3F;

	rtpcs_sds_write_bits(sds, 0x1F, 0x9, 11, 6, v);
}

static void rtpcs_931x_sds_fiber_mode_set(struct rtpcs_serdes *sds,
					  phy_interface_t mode)
{
	u32 val;

	/* clear symbol error count before changing mode */
	rtpcs_931x_sds_symerr_clear(sds, mode);

	rtpcs_931x_sds_disable(sds);

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
	rtpcs_sds_write_bits(sds, 0x1F, 0x9, 11, 6, val);
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

static void rtpcs_931x_sds_cmu_type_set(struct rtpcs_serdes *sds,
					phy_interface_t mode, int chiptype)
{
	struct rtpcs_serdes *even_sds = rtpcs_sds_get_even(sds);
	int cmu_type = 0; /* Clock Management Unit */
	u32 cmu_page = 0;
	u32 frc_cmu_spd;
	u32 frc_lc_mode_bitnum, frc_lc_mode_val_bitnum;

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
		pr_info("SerDes %d mode is invalid\n", sds->id);
		return;
	}

	if (cmu_type == 1)
		cmu_page = rtpcs_931x_sds_cmu_page_get(mode);

	if (sds == even_sds) { 
		frc_lc_mode_bitnum = 4;
		frc_lc_mode_val_bitnum = 5;
	} else {
		frc_lc_mode_bitnum = 6;
		frc_lc_mode_val_bitnum = 7;
	}

	pr_info("%s: cmu_type %0d cmu_page %x frc_cmu_spd %d even_sds %d sds %d\n",
		__func__, cmu_type, cmu_page, frc_cmu_spd, even_sds->id,
		sds->id);

	if (cmu_type == 1) {
		pr_info("%s A CMU page 0x28 0x7 %08x\n", __func__, rtpcs_sds_read(sds, 0x28, 0x7));
		rtpcs_sds_write_bits(sds, cmu_page, 0x7, 15, 15, 0);
		pr_info("%s B CMU page 0x28 0x7 %08x\n", __func__, rtpcs_sds_read(sds, 0x28, 0x7));
		if (chiptype)
			rtpcs_sds_write_bits(sds, cmu_page, 0xd, 14, 14, 0);

		rtpcs_sds_write_bits(even_sds, 0x20, 0x12, 3, 2, 0x3);
		rtpcs_sds_write_bits(even_sds, 0x20, 0x12, frc_lc_mode_bitnum, frc_lc_mode_bitnum, 1);
		rtpcs_sds_write_bits(even_sds, 0x20, 0x12, frc_lc_mode_val_bitnum, frc_lc_mode_val_bitnum, 0);
		rtpcs_sds_write_bits(even_sds, 0x20, 0x12, 12, 12, 1);
		rtpcs_sds_write_bits(even_sds, 0x20, 0x12, 15, 13, frc_cmu_spd);
	}

	pr_info("%s CMU page 0x28 0x7 %08x\n", __func__, rtpcs_sds_read(sds, 0x28, 0x7));
}

static void rtpcs_931x_sds_rx_reset(struct rtpcs_serdes *sds)
{
	if (sds->id < 2)
		return;

	rtpcs_sds_write(sds, 0x2e, 0x12, 0x2740);
	rtpcs_sds_write(sds, 0x2f, 0x0, 0x0);
	rtpcs_sds_write(sds, 0x2f, 0x2, 0x2010);
	rtpcs_sds_write(sds, 0x20, 0x0, 0xc10);

	rtpcs_sds_write(sds, 0x2e, 0x12, 0x27c0);
	rtpcs_sds_write(sds, 0x2f, 0x0, 0xc000);
	rtpcs_sds_write(sds, 0x2f, 0x2, 0x6010);
	rtpcs_sds_write(sds, 0x20, 0x0, 0xc30);

	mdelay(50);
}

static void rtpcs_931x_sds_mii_mode_set(struct rtpcs_serdes *sds,
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

	regmap_write(sds->ctrl->map,
		     RTL931X_SERDES_MODE_CTRL + 4 * (sds->id >> 2), val);
}

static int rtpcs_931x_sds_cmu_band_set(struct rtpcs_serdes *sds,
				       bool enable, u32 band,
				       phy_interface_t mode)
{
	struct rtpcs_serdes *even_sds = rtpcs_sds_get_even(sds);
	int page = rtpcs_931x_sds_cmu_page_get(mode);

	page += 1;

	if (enable) {
		rtpcs_sds_write_bits(even_sds, page, 0x7, 13, 13, 0);
		rtpcs_sds_write_bits(even_sds, page, 0x7, 11, 11, 0);
	} else {
		rtpcs_sds_write_bits(even_sds, page, 0x7, 13, 13, 0);
		rtpcs_sds_write_bits(even_sds, page, 0x7, 11, 11, 0);
	}

	rtpcs_sds_write_bits(even_sds, page, 0x7, 4, 0, band);

	rtpcs_931x_sds_reset(even_sds);

	return 0;
}

static int rtpcs_931x_sds_cmu_band_get(struct rtpcs_serdes *sds,
				       phy_interface_t mode)
{
	struct rtpcs_serdes *even_sds = rtpcs_sds_get_even(sds);
	int page = rtpcs_931x_sds_cmu_page_get(mode);
	u32 band;

	page += 1;
	rtpcs_sds_write(even_sds, 0x1f, 0x02, 73);

	rtpcs_sds_write_bits(even_sds, page, 0x5, 15, 15, 1);
	band = rtpcs_sds_read_bits(even_sds, 0x1f, 0x15, 8, 3);
	pr_info("%s band is: %d\n", __func__, band);

	return band;
}

__always_unused
static int rtpcs_931x_sds_link_sts_get(struct rtpcs_serdes *sds)
{
	u32 sts, sts1, latch_sts, latch_sts1;

	if (0) {
		sts = rtpcs_sds_read_bits(sds, 0x41, 29, 8, 0);
		sts1 = rtpcs_sds_read_bits(sds, 0x81, 29, 8, 0);
		latch_sts = rtpcs_sds_read_bits(sds, 0x41, 30, 8, 0);
		latch_sts1 = rtpcs_sds_read_bits(sds, 0x81, 30, 8, 0);
	} else {
		sts = rtpcs_sds_read_bits(sds, 0x5, 0, 12, 12);
		latch_sts = rtpcs_sds_read_bits(sds, 0x4, 1, 2, 2);
		latch_sts1 = rtpcs_sds_read_bits(sds, 0x42, 1, 2, 2);
		sts1 = rtpcs_sds_read_bits(sds, 0x42, 1, 2, 2);
	}

	pr_info("%s: serdes %d sts %d, sts1 %d, latch_sts %d, latch_sts1 %d\n", __func__,
		sds->id, sts, sts1, latch_sts, latch_sts1);

	return sts1;
}

static int rtpcs_931x_sds_set_polarity(struct rtpcs_serdes *sds,
				       bool tx_inv, bool rx_inv)
{
	u8 rx_val = rx_inv ? 1 : 0;
	u8 tx_val = tx_inv ? 1 : 0;
	u32 val;
	int ret;

	/* 10gr_*_inv */
	val = (tx_val << 1) | rx_val;
	ret = rtpcs_sds_write_bits(sds, 0x6, 0x2, 14, 13, val);
	if (ret)
		return ret;

	/* xsg_*_inv */
	val = (rx_val << 1) | tx_val;
	ret = rtpcs_sds_write_bits(sds, 0x40, 0x0, 9, 8, val);
	if (ret)
		return ret;

	return rtpcs_sds_write_bits(sds, 0x80, 0x0, 9, 8, val);
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

static int rtpcs_931x_setup_serdes(struct rtpcs_serdes *sds,
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
	struct rtpcs_serdes *even_sds = rtpcs_sds_get_even(sds);
	struct rtpcs_ctrl *ctrl = sds->ctrl;
	u32 band, ori, model_info, val;
	u32 sds_id = sds->id;
	int chiptype = 0;

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

	pr_info("%s: set sds %d to mode %d\n", __func__, sds_id, mode);
	val = rtpcs_sds_read_bits(sds, 0x1F, 0x9, 11, 6);

	pr_info("%s: fibermode %08X stored mode 0x%x", __func__,
		rtpcs_sds_read(sds, 0x1f, 0x9), val);
	pr_info("%s: SGMII mode %08X in 0x24 0x9", __func__,
		rtpcs_sds_read(sds, 0x24, 0x9));
	pr_info("%s: CMU mode %08X stored even SDS %d", __func__,
		rtpcs_sds_read(even_sds, 0x20, 0x12), even_sds->id);
	pr_info("%s: serdes_mode_ctrl %08X", __func__,  RTL931X_SERDES_MODE_CTRL + 4 * (sds_id >> 2));
	pr_info("%s CMU page 0x24 0x7 %08x\n", __func__, rtpcs_sds_read(sds, 0x24, 0x7));
	pr_info("%s CMU page 0x26 0x7 %08x\n", __func__, rtpcs_sds_read(sds, 0x26, 0x7));
	pr_info("%s CMU page 0x28 0x7 %08x\n", __func__, rtpcs_sds_read(sds, 0x28, 0x7));
	pr_info("%s XSG page 0x0 0xe %08x\n", __func__, rtpcs_sds_read(sds, 0x40, 0xe));
	pr_info("%s XSG2 page 0x0 0xe %08x\n", __func__, rtpcs_sds_read(sds, 0x80, 0xe));

	regmap_read(ctrl->map, RTL93XX_MODEL_NAME_INFO, &model_info);
	if ((model_info >> 4) & 0x1) {
		pr_info("detected chiptype 1\n");
		chiptype = 1;
	} else {
		pr_info("detected chiptype 0\n");
	}

	pr_info("%s: 2.5gbit %08X", __func__, rtpcs_sds_read(sds, 0x41, 0x14));

	regmap_read(ctrl->map, RTL931X_PS_SERDES_OFF_MODE_CTRL_ADDR, &ori);
	pr_info("%s: RTL931X_PS_SERDES_OFF_MODE_CTRL_ADDR 0x%08X\n", __func__, ori);
	val = ori | (1 << sds_id);
	regmap_write(ctrl->map, RTL931X_PS_SERDES_OFF_MODE_CTRL_ADDR, val);

	/* this was in rtl931x_phylink_mac_config in dsa/rtl83xx/dsa.c before */
	band = rtpcs_931x_sds_cmu_band_get(sds, mode);

	switch (mode) {
	case PHY_INTERFACE_MODE_NA:
		break;

	case PHY_INTERFACE_MODE_XGMII: /* MII_XSGMII */

		if (chiptype) {
			/* fifo inv clk */
			rtpcs_sds_write_bits(sds, 0x41, 0x1, 7, 4, 0xf);
			rtpcs_sds_write_bits(sds, 0x41, 0x1, 3, 0, 0xf);

			rtpcs_sds_write_bits(sds, 0x81, 0x1, 7, 4, 0xf);
			rtpcs_sds_write_bits(sds, 0x81, 0x1, 3, 0, 0xf);
		}

		rtpcs_sds_write_bits(sds, 0x40, 0xE, 12, 12, 1);
		rtpcs_sds_write_bits(sds, 0x80, 0xE, 12, 12, 1);
		break;

	case PHY_INTERFACE_MODE_USXGMII: /* MII_USXGMII_10GSXGMII/10GDXGMII/10GQXGMII: */
		u32 op_code = 0x6003;

		if (chiptype) {
			rtpcs_sds_write_bits(sds, 0x6, 0x2, 12, 12, 1);

			for (int i = 0; i < sizeof(sds_config_10p3125g_type1) / sizeof(sds_config); ++i) {
				rtpcs_sds_write(sds,
						sds_config_10p3125g_type1[i].page - 0x4,
						sds_config_10p3125g_type1[i].reg,
						sds_config_10p3125g_type1[i].data);
			}

			for (int i = 0; i < sizeof(sds_config_10p3125g_cmu_type1) / sizeof(sds_config); ++i) {
				rtpcs_sds_write(even_sds,
						sds_config_10p3125g_cmu_type1[i].page - 0x4,
						sds_config_10p3125g_cmu_type1[i].reg,
						sds_config_10p3125g_cmu_type1[i].data);
			}

			rtpcs_sds_write_bits(sds, 0x6, 0x2, 12, 12, 0);
		} else {
			rtpcs_sds_write_bits(sds, 0x2e, 0xd, 6, 0, 0x0);
			rtpcs_sds_write_bits(sds, 0x2e, 0xd, 7, 7, 0x1);

			rtpcs_sds_write_bits(sds, 0x2e, 0x1c, 5, 0, 0x1E);
			rtpcs_sds_write_bits(sds, 0x2e, 0x1d, 11, 0, 0x00);
			rtpcs_sds_write_bits(sds, 0x2e, 0x1f, 11, 0, 0x00);
			rtpcs_sds_write_bits(sds, 0x2f, 0x0, 11, 0, 0x00);
			rtpcs_sds_write_bits(sds, 0x2f, 0x1, 11, 0, 0x00);

			rtpcs_sds_write_bits(sds, 0x2e, 0xf, 12, 6, 0x7F);
			rtpcs_sds_write(sds, 0x2f, 0x12, 0xaaa);

			rtpcs_931x_sds_rx_reset(sds);

			rtpcs_sds_write(sds, 0x7, 0x10, op_code);
			rtpcs_sds_write(sds, 0x6, 0x1d, 0x0480);
			rtpcs_sds_write(sds, 0x6, 0xe, 0x0400);
		}
		break;

	case PHY_INTERFACE_MODE_10GBASER: /* MII_10GR / MII_10GR1000BX_AUTO: */
					  /* configure 10GR fiber mode=1 */
		rtpcs_sds_write_bits(sds, 0x1f, 0xb, 1, 1, 1);

		/* init fiber_1g */
		rtpcs_sds_write_bits(sds, 0x43, 0x13, 15, 14, 0);

		rtpcs_sds_write_bits(sds, 0x42, 0x0, 12, 12, 1);
		rtpcs_sds_write_bits(sds, 0x42, 0x0, 6, 6, 1);
		rtpcs_sds_write_bits(sds, 0x42, 0x0, 13, 13, 0);

		/* init auto */
		rtpcs_sds_write_bits(sds, 0x1f, 13, 15, 0, 0x109e);
		rtpcs_sds_write_bits(sds, 0x1f, 0x6, 14, 10, 0x8);
		rtpcs_sds_write_bits(sds, 0x1f, 0x7, 10, 4, 0x7f);
		break;

	case PHY_INTERFACE_MODE_1000BASEX: /* MII_1000BX_FIBER */
		rtpcs_sds_write_bits(sds, 0x43, 0x13, 15, 14, 0);

		rtpcs_sds_write_bits(sds, 0x42, 0x0, 12, 12, 1);
		rtpcs_sds_write_bits(sds, 0x42, 0x0, 6, 6, 1);
		rtpcs_sds_write_bits(sds, 0x42, 0x0, 13, 13, 0);
		break;

	case PHY_INTERFACE_MODE_SGMII:
		rtpcs_sds_write_bits(sds, 0x24, 0x9, 15, 15, 0);

		/* this was in rtl931x_phylink_mac_config in dsa/rtl83xx/dsa.c before */
		band = rtpcs_931x_sds_cmu_band_set(sds, true, 62, PHY_INTERFACE_MODE_SGMII);
		break;

	case PHY_INTERFACE_MODE_2500BASEX:
		rtpcs_sds_write_bits(sds, 0x41, 0x14, 8, 8, 1);
		break;

	case PHY_INTERFACE_MODE_QSGMII:
	default:
		pr_info("%s: PHY mode %s not supported by SerDes %d\n",
			__func__, phy_modes(mode), sds_id);
		return -ENOTSUPP;
	}

	rtpcs_931x_sds_cmu_type_set(sds, mode, chiptype);

	if (sds_id >= 2) {
		if (chiptype)
			rtpcs_sds_write(sds, 0x2E, 0x1, board_sds_tx_type1[sds_id - 2]);
		else {
			val = 0xa0000;
			regmap_write(ctrl->map, RTL93XX_CHIP_INFO, val);
			regmap_read(ctrl->map, RTL93XX_CHIP_INFO, &val);

			if (val & BIT(28)) /* consider 9311 etc. RTL9313_CHIP_ID == HWP_CHIP_ID(unit)) */
				rtpcs_sds_write(sds, 0x2E, 0x1, board_sds_tx2[sds_id - 2]);
			else
				rtpcs_sds_write(sds, 0x2E, 0x1, board_sds_tx[sds_id - 2]);

			val = 0;
			regmap_write(ctrl->map, RTL93XX_CHIP_INFO, val);
		}
	}

	rtpcs_931x_sds_set_polarity(sds, sds->tx_pol_inv, sds->rx_pol_inv);

	val = ori & ~BIT(sds_id);
	regmap_write(ctrl->map, RTL931X_PS_SERDES_OFF_MODE_CTRL_ADDR, val);
	regmap_read(ctrl->map, RTL931X_PS_SERDES_OFF_MODE_CTRL_ADDR, &val);
	pr_debug("%s: RTL931X_PS_SERDES_OFF_MODE_CTRL_ADDR 0x%08X\n", __func__, val);

	if (mode == PHY_INTERFACE_MODE_XGMII ||
	    mode == PHY_INTERFACE_MODE_QSGMII ||
	    mode == PHY_INTERFACE_MODE_SGMII ||
	    mode == PHY_INTERFACE_MODE_USXGMII) {
		if (mode == PHY_INTERFACE_MODE_XGMII)
			rtpcs_931x_sds_mii_mode_set(sds, mode);
		else
			rtpcs_931x_sds_fiber_mode_set(sds, mode);
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
		 link->port, link->sds->id);
}

static int rtpcs_pcs_config(struct phylink_pcs *pcs, unsigned int neg_mode,
			    phy_interface_t interface, const unsigned long *advertising,
			    bool permit_pause_to_mac)
{
	struct rtpcs_link *link = rtpcs_phylink_pcs_to_link(pcs);
	struct rtpcs_ctrl *ctrl = link->ctrl;
	int ret = 0;

	/*
	 * TODO: This (or copies of this) will be the central function for configuring the
	 * link between PHY and SerDes. As of now a lot of the code is scattered throughout
	 * all the other Realtek drivers. Maybe some day this will live up to the expectations.
	 */

	dev_warn(ctrl->dev, "pcs_config(%s) for port %d and sds %d not yet fully implemented\n",
		 phy_modes(interface), link->port, link->sds->id);

	mutex_lock(&ctrl->lock);

	if (ctrl->cfg->setup_serdes) {
		ret = ctrl->cfg->setup_serdes(link->sds, interface);
		if (ret < 0)
			goto out;
	}

	if (ctrl->cfg->set_autoneg) {
		ret = ctrl->cfg->set_autoneg(link->sds, neg_mode);
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
	u32 sds_id;

	if (!np || !of_device_is_available(np))
		return ERR_PTR(-ENODEV);

	pcs_np = of_get_parent(np);
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

	if (of_property_read_u32(np, "reg", &sds_id))
		return ERR_PTR(-EINVAL);
	if (sds_id >= ctrl->cfg->serdes_count)
		return ERR_PTR(-EINVAL);
	if (rtpcs_sds_read(&ctrl->serdes[sds_id], 0, 0) < 0)
		return ERR_PTR(-EINVAL);

	link = kzalloc(sizeof(*link), GFP_KERNEL);
	if (!link) {
		put_device(&pdev->dev);
		return ERR_PTR(-ENOMEM);
	}

	device_link_add(dev, ctrl->dev, DL_FLAG_AUTOREMOVE_CONSUMER);

	link->ctrl = ctrl;
	link->port = port;
	link->sds = &ctrl->serdes[sds_id];
	link->pcs.ops = ctrl->cfg->pcs_ops;
	link->pcs.neg_mode = true;

	ctrl->link[port] = link;

	dev_dbg(ctrl->dev, "phylink_pcs created, port %d, sds %d\n", port, sds_id);

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
	struct device_node *child;
	struct rtpcs_serdes *sds;
	struct rtpcs_ctrl *ctrl;
	u32 sds_id;
	int i, ret;

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

	for (i = 0; i < ctrl->cfg->serdes_count; i++) {
		sds = &ctrl->serdes[i];
		sds->ctrl = ctrl;
		sds->id = i;
	}

	for_each_child_of_node(dev->of_node, child) {
		ret = of_property_read_u32(child, "reg", &sds_id);
		if (ret)
			return ret;
		if (sds_id >= ctrl->cfg->serdes_count)
			return -EINVAL;

		sds = &ctrl->serdes[sds_id];
		sds->rx_pol_inv = of_property_read_bool(child, "realtek,pnswap-rx");
		sds->tx_pol_inv = of_property_read_bool(child, "realtek,pnswap-tx");
	}

	if (ctrl->cfg->init_serdes_common) {
		ret = ctrl->cfg->init_serdes_common(ctrl);
		if (ret)
			return ret;
	}

	/*
	 * rtpcs_create() relies on that fact that data is attached to the platform device to
	 * determine if the driver is ready. Do this after everything is initialized properly.
	 */
	platform_set_drvdata(pdev, ctrl);

	dev_info(dev, "Realtek PCS driver initialized\n");

	return 0;
}

static int rtpcs_93xx_set_autoneg(struct rtpcs_serdes *sds, unsigned int neg_mode)
{
	u16 bmcr = neg_mode == PHYLINK_PCS_NEG_INBAND_ENABLED ? BMCR_ANENABLE : 0;

	return rtpcs_sds_modify(sds, 2, MII_BMCR, BMCR_ANENABLE, bmcr);
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
	.serdes_count		= RTPCS_838X_SERDES_CNT,
	.pcs_ops		= &rtpcs_838x_pcs_ops,
	.init_serdes_common	= rtpcs_838x_init_serdes_common,
	.setup_serdes		= rtpcs_838x_setup_serdes,
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
	.serdes_count		= RTPCS_839X_SERDES_CNT,
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
	.serdes_count		= RTPCS_930X_SERDES_CNT,
	.pcs_ops		= &rtpcs_930x_pcs_ops,
	.set_autoneg		= rtpcs_93xx_set_autoneg,
	.setup_serdes		= rtpcs_930x_setup_serdes,
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
	.serdes_count		= RTPCS_931X_SERDES_CNT,
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

// SPDX-License-Identifier: GPL-2.0-or-later

#include <linux/mdio.h>
#include <linux/mfd/syscon.h>
#include <linux/of.h>
#include <linux/of_mdio.h>
#include <linux/of_platform.h>
#include <linux/pcs/pcs-provider.h>
#include <linux/phy.h>
#include <linux/phy/phy-common-props.h>
#include <linux/phylink.h>
#include <linux/platform_device.h>
#include <linux/property.h>
#include <linux/regmap.h>
#include <linux/rtnetlink.h>

#define RTPCS_SDS_CNT				14
#define RTPCS_MAX_LINKS_PER_SDS			8

#define RTPCS_SPEED_10				0
#define RTPCS_SPEED_100				1
#define RTPCS_SPEED_1000			2
#define RTPCS_SPEED_10000_LEGACY		3
#define RTPCS_SPEED_10000			4
#define RTPCS_SPEED_2500			5
#define RTPCS_SPEED_5000			6

/* USXGMII-AN opcodes. RTK variant unused but kept for documentation */
#define RTPCS_USXGMII_AN_OPC_STD		0x03
#define RTPCS_USXGMII_AN_OPC_RTK		0xaa

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
#define RTPCS_838X_INT_MODE_CTRL		0x005c
#define RTPCS_838X_PLL_CML_CTRL			0x0ff8

#define RTPCS_839X_MAC_SERDES_IF_CTRL		0x0008

#define RTPCS_93XX_MAC_LINK_SPD_BITS		4

#define RTPCS_93XX_MODEL_NAME_INFO		(0x0004)
#define RTPCS_93XX_CHIP_INFO			(0x0008)

#define RTPCS_930X_SDS_MODE_SEL_0		0x0194
#define RTPCS_930X_SDS_MODE_SEL_1		0x02a0
#define RTPCS_930X_SDS_MODE_SEL_2		0x02a4
#define RTPCS_930X_SDS_MODE_SEL_3		0x0198
#define RTPCS_930X_SDS_SUBMODE_CTRL_0		0x01cc
#define RTPCS_930X_SDS_SUBMODE_CTRL_1		0x02d8

#define RTPCS_93XX_SDS_MODE_SGMII		0x02
#define RTPCS_93XX_SDS_MODE_1000BASEX		0x04
#define RTPCS_93XX_SDS_MODE_QSGMII		0x06
#define RTPCS_93XX_SDS_MODE_USXGMII		0x0d
#define RTPCS_93XX_SDS_MODE_XSGMII		0x10
#define RTPCS_93XX_SDS_MODE_2500BASEX		0x16
#define RTPCS_93XX_SDS_MODE_10GBASER		0x1a
#define RTPCS_93XX_SDS_MODE_OFF			0x1f

#define RTPCS_93XX_SDS_USXGMII_SUBMODE_10GSX	0x00
#define RTPCS_93XX_SDS_USXGMII_SUBMODE_10GDX	0x01
#define RTPCS_93XX_SDS_USXGMII_SUBMODE_10GQX	0x02
#define RTPCS_93XX_SDS_USXGMII_SUBMODE_5GSX	0x03
#define RTPCS_93XX_SDS_USXGMII_SUBMODE_5GDX	0x04
#define RTPCS_93XX_SDS_USXGMII_SUBMODE_2_5GSX	0x05

/* Registers of the internal SerDes of the 9310 */
#define RTPCS_931X_MAC_GROUP0_1_CTRL		(0x13a4)
#define RTPCS_931X_MAC_GROUP2_3_CTRL		(0x13a8)
#define RTPCS_931X_MAC_GROUP4_CTRL		(0x13ac)
#define RTPCS_931X_MAC_GROUP5_CTRL		(0x13b0)
#define RTPCS_931X_MAC_GROUP6_7_CTRL		(0x13b4)
#define RTPCS_931X_MAC_GROUP8_11_CTRL		(0x13b8)
#define RTPCS_931X_SERDES_MODE_CTRL		(0x13cc)
#define RTPCS_931X_SDS_USXGMII_SUBMODE		(0x13e8)
#define RTPCS_931X_PS_SERDES_OFF_MODE_CTRL_ADDR	(0x13F4)
#define RTPCS_931X_MAC_SERDES_MODE_CTRL(sds)	(0x136C + (((sds) << 2)))
#define RTPCS_931X_ISR_SERDES_RXIDLE		(0x12f8)

#define RTPCS_931X_SDS_PRE_AMP_MASK		GENMASK(4, 0)
#define RTPCS_931X_SDS_MAIN_AMP_MASK		GENMASK(9, 5)
#define RTPCS_931X_SDS_POST_AMP_MASK		GENMASK(14, 10)

/*
 * A SerDes has a register space separated into several pages. Each page
 * serves a different purpose and is the home of common settings. E.g.,
 * there are dedicated pages for each operating speed of a SerDes.
 */
enum rtpcs_page {
	PAGE_SDS		= 0x00,
	PAGE_SDS_EXT		= 0x01,
	PAGE_FIB		= 0x02,
	PAGE_FIB_EXT		= 0x03,
	PAGE_TGR_STD_0		= 0x04,
	PAGE_TGR_STD_1		= 0x05,
	PAGE_TGR_PRO_0		= 0x06,
	PAGE_TGR_PRO_1		= 0x07,
	PAGE_TGX_STD_0		= 0x08,
	PAGE_TGX_STD_1		= 0x09,
	PAGE_TGX_PRO_0		= 0x0a,
	PAGE_TGX_PRO_1		= 0x0b,
	PAGE_WDIG		= 0x1f,
	PAGE_ANA_MISC		= 0x20,
	PAGE_ANA_COM		= 0x21,
	PAGE_ANA_SPD		= 0x22,
	PAGE_ANA_SPD_EXT	= 0x23,
	PAGE_ANA_1G2		= 0x24,
	PAGE_ANA_1G2_EXT	= 0x25,
	PAGE_ANA_2G5		= 0x26,
	PAGE_ANA_2G5_EXT	= 0x27,
	PAGE_ANA_3G1		= 0x28,
	PAGE_ANA_3G1_EXT	= 0x29,
	PAGE_ANA_5G0		= 0x2a,
	PAGE_ANA_5G0_EXT	= 0x2b,
	PAGE_ANA_6G2		= 0x2c,
	PAGE_ANA_6G2_EXT	= 0x2d,
	PAGE_ANA_10G		= 0x2e,
	PAGE_ANA_10G_EXT	= 0x2f,
	PAGE_GPON_SP		= 0x30,
	PAGE_GPON_SP_EXT	= 0x31,
	PAGE_EPON_SP		= 0x32,
	PAGE_EPON_SP_EXT	= 0x33,
	PAGE_ANA_6G0		= 0x34,
	PAGE_ANA_6G0_EXT	= 0x35,
};

/*
 * RTL931X only: the digital SDS 1/2 register pages mirror an analog page at a
 * fixed +0x40 / +0x80 region offset. See rtpcs_931x_sds_op_xsg_write().
 */
#define DIGI_1(page)	((page) + 0x40)
#define DIGI_2(page)	((page) + 0x80)

enum rtpcs_sds_type {
	RTPCS_SDS_TYPE_UNKNOWN,
	RTPCS_SDS_TYPE_5G,
	RTPCS_SDS_TYPE_10G,
};

enum rtpcs_sds_mode {
	RTPCS_SDS_MODE_OFF = 0,

	/* fiber modes */
	RTPCS_SDS_MODE_100BASEX,
	RTPCS_SDS_MODE_1000BASEX,
	RTPCS_SDS_MODE_2500BASEX,
	RTPCS_SDS_MODE_10GBASER,

	/* mii modes */
	RTPCS_SDS_MODE_SGMII,
	RTPCS_SDS_MODE_QSGMII,
	RTPCS_SDS_MODE_XSGMII,

	RTPCS_SDS_MODE_USXGMII_10GSXGMII,
	RTPCS_SDS_MODE_USXGMII_10GDXGMII,
	RTPCS_SDS_MODE_USXGMII_10GQXGMII,
	RTPCS_SDS_MODE_USXGMII_5GSXGMII,
	RTPCS_SDS_MODE_USXGMII_5GDXGMII,
	RTPCS_SDS_MODE_USXGMII_2_5GSXGMII,

	RTPCS_SDS_MODE_MAX,
};

enum rtpcs_sds_media {
	RTPCS_SDS_MEDIA_NONE,
	RTPCS_SDS_MEDIA_FIBER,
	RTPCS_SDS_MEDIA_DAC_SHORT,	/*  < 3m */
	RTPCS_SDS_MEDIA_DAC_LONG,	/* >= 3m */
	RTPCS_SDS_MEDIA_PCB,
};

enum rtpcs_sds_pll_type {
	RTPCS_SDS_PLL_TYPE_RING = 0,
	RTPCS_SDS_PLL_TYPE_LC = 1,
	RTPCS_SDS_PLL_TYPE_END,
};

enum rtpcs_sds_pll_speed {
	RTPCS_SDS_PLL_SPD_1000 = 0,
	RTPCS_SDS_PLL_SPD_2500 = 1,
	RTPCS_SDS_PLL_SPD_10000 = 2,
	RTPCS_SDS_PLL_SPD_END,
};

enum rtpcs_chip_version {
	RTPCS_CHIP_V1 = 0,
	RTPCS_CHIP_V2,
};

struct rtpcs_ctrl;
struct rtpcs_serdes;

struct rtpcs_sds_ops {
	int (*read)(struct rtpcs_serdes *sds, enum rtpcs_page page, int regnum,
		    int bithigh, int bitlow);
	int (*write)(struct rtpcs_serdes *sds, enum rtpcs_page page, int regnum,
		     int bithigh, int bitlow, u16 value);

	/* optional */
	int (*xsg_write)(struct rtpcs_serdes *sds, enum rtpcs_page page, int regnum,
			 int bithigh, int bitlow, u16 value);

	int (*set_autoneg)(struct rtpcs_serdes *sds, unsigned int neg_mode,
			   const unsigned long *advertising);
	void (*restart_autoneg)(struct rtpcs_serdes *sds);

	/* CMU management */
	int (*get_pll_select)(struct rtpcs_serdes *sds, enum rtpcs_sds_pll_type *pll);
	int (*set_pll_select)(struct rtpcs_serdes *sds, enum rtpcs_sds_mode hw_mode,
			      enum rtpcs_sds_pll_type pll);
	int (*reset_cmu)(struct rtpcs_serdes *sds, enum rtpcs_sds_pll_type pll);
	/* online reconfiguration of a running SerDes to another PLL */
	int (*reconfigure_to_pll)(struct rtpcs_serdes *sds, enum rtpcs_sds_pll_type pll);

	int (*config_polarity)(struct rtpcs_serdes *sds, unsigned int tx_pol,
			       unsigned int rx_pol);

	/* required: power down before reconfiguration */
	int (*deactivate)(struct rtpcs_serdes *sds);
	/* required: power back up */
	int (*activate)(struct rtpcs_serdes *sds);
	/* required: configure SerDes for hardware mode */
	int (*config_hw_mode)(struct rtpcs_serdes *sds, enum rtpcs_sds_mode hw_mode);
	/* required: set hardware mode */
	int (*set_hw_mode)(struct rtpcs_serdes *sds, enum rtpcs_sds_mode hw_mode);
	/* optional: configure media-specific parameters */
	int (*config_media)(struct rtpcs_serdes *sds, enum rtpcs_sds_media media,
			    enum rtpcs_sds_mode hw_mode);
	/* optional: finalization that must follow power-up, e.g. RX calibration */
	int (*post_config)(struct rtpcs_serdes *sds, enum rtpcs_sds_mode hw_mode);
};

struct rtpcs_sds_reg_field {
	enum rtpcs_page page;
	u8 reg;
	u8 msb;
	u8 lsb;
};

struct rtpcs_sds_regs {
	struct rtpcs_sds_reg_field an_enable;
	struct rtpcs_sds_reg_field an_restart;
	struct rtpcs_sds_reg_field an_advertise;
};

struct rtpcs_serdes {
	struct rtpcs_ctrl *ctrl;
	struct fwnode_handle *fwnode;
	const struct rtpcs_sds_ops *ops;
	const struct rtpcs_sds_regs *regs;
	enum rtpcs_sds_type type;
	DECLARE_BITMAP(supported_modes, RTPCS_SDS_MODE_MAX);
	struct {
		struct regmap_field *mac_mode;
		struct regmap_field *mac_mode_force;	/* nullable, 931x only */
		struct regmap_field *usxgmii_submode;	/* nullable, 93xx only */
	} swcore_regs;
	struct rtpcs_link *link[RTPCS_MAX_LINKS_PER_SDS];
	s16 link_port[RTPCS_MAX_LINKS_PER_SDS];

	enum rtpcs_sds_mode hw_mode;
	enum rtpcs_sds_media media;
	u8 id;
	u8 num_of_links;
	bool first_start;
};

struct rtpcs_ctrl {
	struct device *dev;
	struct regmap *map;
	struct mii_bus *bus;
	const struct rtpcs_config *cfg;
	struct rtpcs_serdes serdes[RTPCS_SDS_CNT];
	struct mutex lock;

	/* meaning and source may be family-specific */
	enum rtpcs_chip_version chip_version;
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
	const struct rtpcs_sds_ops *sds_ops;
	const struct rtpcs_sds_regs *sds_regs;
	const s16 *sds_hw_mode_vals;   /* enum rtpcs_sds_mode, -1 = unsupported */

	int (*init)(struct rtpcs_ctrl *ctrl);
	int (*sds_probe)(struct rtpcs_serdes *sds);
};

struct rtpcs_sds_config {
	enum rtpcs_page page;
	u8 reg;
	u16 data;
};

struct rtpcs_sds_tx_config {
	u8 pre_amp;
	u8 main_amp;
	u8 post_amp;
};

static int rtpcs_sds_to_mmd(enum rtpcs_page sds_page, int sds_regnum)
{
	return (sds_page << 8) + sds_regnum;
}

/*
 * Basic helpers
 *
 * These work on the plain SerDes ID. They shouldn't be used except for
 * implementing the SerDes read/write ops.
 */

static int __rtpcs_sds_read_raw(struct rtpcs_ctrl *ctrl, int sds_id, enum rtpcs_page page,
				int regnum, int bithigh, int bitlow)
{
	int mmd_regnum = rtpcs_sds_to_mmd(page, regnum);
	u16 mask;
	int val;

	if (WARN_ON(bithigh < bitlow))
		return -EINVAL;

	mask = GENMASK(bithigh, bitlow);
	val = mdiobus_c45_read(ctrl->bus, sds_id, MDIO_MMD_VEND1, mmd_regnum);
	if (val < 0)
		return val;

	return (val & mask) >> bitlow;
}

static int __rtpcs_sds_write_raw(struct rtpcs_ctrl *ctrl, int sds_id, enum rtpcs_page page,
				 int regnum, int bithigh, int bitlow, u16 value)
{
	int mmd_regnum = rtpcs_sds_to_mmd(page, regnum);
	u16 mask, set;

	if (WARN_ON(bithigh < bitlow))
		return -EINVAL;

	if (bithigh == 15 && bitlow == 0)
		return mdiobus_c45_write(ctrl->bus, sds_id, MDIO_MMD_VEND1, mmd_regnum, value);

	mask = GENMASK(bithigh, bitlow);
	set = (value << bitlow) & mask;
	return mdiobus_c45_modify(ctrl->bus, sds_id, MDIO_MMD_VEND1, mmd_regnum, mask, set);
}

/* Generic implementations, if no special behavior is needed */

static int rtpcs_generic_sds_op_read(struct rtpcs_serdes *sds, enum rtpcs_page page, int regnum,
				     int bithigh, int bitlow)
{
	return __rtpcs_sds_read_raw(sds->ctrl, sds->id, page, regnum, bithigh, bitlow);
}

static int rtpcs_generic_sds_op_write(struct rtpcs_serdes *sds, enum rtpcs_page page, int regnum,
				      int bithigh, int bitlow, u16 value)
{
	return __rtpcs_sds_write_raw(sds->ctrl, sds->id, page, regnum, bithigh, bitlow, value);
}

/* Convenience helpers */

static int rtpcs_sds_read_bits(struct rtpcs_serdes *sds, enum rtpcs_page page, int regnum,
			       int bithigh, int bitlow)
{
	return sds->ops->read(sds, page, regnum, bithigh, bitlow);
}

static int rtpcs_sds_write_bits(struct rtpcs_serdes *sds, enum rtpcs_page page, int regnum,
				int bithigh, int bitlow, u16 value)
{
	return sds->ops->write(sds, page, regnum, bithigh, bitlow, value);
}

static int rtpcs_sds_read(struct rtpcs_serdes *sds, enum rtpcs_page page, int regnum)
{
	return sds->ops->read(sds, page, regnum, 15, 0);
}

static int rtpcs_sds_write(struct rtpcs_serdes *sds, enum rtpcs_page page, int regnum, u16 value)
{
	return sds->ops->write(sds, page, regnum, 15, 0, value);
}

__maybe_unused
static int rtpcs_sds_read_field(struct rtpcs_serdes *sds, const struct rtpcs_sds_reg_field *field)
{
	return sds->ops->read(sds, field->page, field->reg, field->msb, field->lsb);
}

static int rtpcs_sds_write_field(struct rtpcs_serdes *sds, const struct rtpcs_sds_reg_field *field,
				 u16 value)
{
	return sds->ops->write(sds, field->page, field->reg, field->msb, field->lsb, value);
}

static int rtpcs_sds_xsg_write_bits(struct rtpcs_serdes *sds, enum rtpcs_page page, int regnum,
				    int bithigh, int bitlow, u16 value)
{
	if (!sds->ops->xsg_write)
		return -ENOTSUPP;

	return sds->ops->xsg_write(sds, page, regnum, bithigh, bitlow, value);
}

static int rtpcs_sds_xsg_write(struct rtpcs_serdes *sds, enum rtpcs_page page, int regnum,
			       u16 value)
{
	if (!sds->ops->xsg_write)
		return -ENOTSUPP;

	return sds->ops->xsg_write(sds, page, regnum, 15, 0, value);
}

/* Other helpers */

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

static int rtpcs_sds_select_hw_mode(struct rtpcs_serdes *sds, phy_interface_t if_mode,
				    enum rtpcs_sds_mode *hw_mode)
{
	u8 n_links = sds->num_of_links;

	/* turn off SerDes when there are no links */
	if (!n_links) {
		*hw_mode = RTPCS_SDS_MODE_OFF;
		return 0;
	}

	switch (if_mode) {
	case PHY_INTERFACE_MODE_NA:
		*hw_mode = RTPCS_SDS_MODE_OFF;
		break;
	case PHY_INTERFACE_MODE_100BASEX:
		*hw_mode = RTPCS_SDS_MODE_100BASEX;
		break;
	case PHY_INTERFACE_MODE_1000BASEX:
		*hw_mode = RTPCS_SDS_MODE_1000BASEX;
		break;
	case PHY_INTERFACE_MODE_2500BASEX:
		*hw_mode = RTPCS_SDS_MODE_2500BASEX;
		break;
	case PHY_INTERFACE_MODE_10GBASER:
		*hw_mode = RTPCS_SDS_MODE_10GBASER;
		break;
	case PHY_INTERFACE_MODE_SGMII:
		*hw_mode = RTPCS_SDS_MODE_SGMII;
		break;
	case PHY_INTERFACE_MODE_QSGMII:
		*hw_mode = RTPCS_SDS_MODE_QSGMII;
		break;
	case PHY_INTERFACE_MODE_USXGMII:
		if (n_links == 1)
			*hw_mode = RTPCS_SDS_MODE_USXGMII_10GSXGMII;
		else if (n_links == 2)
			*hw_mode = RTPCS_SDS_MODE_USXGMII_10GDXGMII;
		else if (n_links <= 4)
			*hw_mode = RTPCS_SDS_MODE_USXGMII_10GQXGMII;
		else if (n_links <= 8)
			*hw_mode = RTPCS_SDS_MODE_XSGMII;

		break;
	case PHY_INTERFACE_MODE_10G_QXGMII:
		*hw_mode = RTPCS_SDS_MODE_USXGMII_10GQXGMII;
		break;
	default:
		return -EOPNOTSUPP;
	}

	if (!test_bit(*hw_mode, sds->supported_modes))
		return -EOPNOTSUPP;

	return 0;
}

static int rtpcs_sds_select_media(enum rtpcs_sds_mode hw_mode, enum rtpcs_sds_media *media)
{
	switch (hw_mode) {
	case RTPCS_SDS_MODE_OFF:
		*media = RTPCS_SDS_MEDIA_NONE;
		break;
	case RTPCS_SDS_MODE_SGMII:
	case RTPCS_SDS_MODE_1000BASEX:
	case RTPCS_SDS_MODE_2500BASEX:
	case RTPCS_SDS_MODE_10GBASER:
		*media = RTPCS_SDS_MEDIA_FIBER;
		break;
	default:
		*media = RTPCS_SDS_MEDIA_PCB;
		break;
	}

	return 0;
}

static bool rtpcs_sds_mode_is_usxgmii(enum rtpcs_sds_mode hw_mode)
{
	return (hw_mode == RTPCS_SDS_MODE_USXGMII_10GSXGMII ||
		hw_mode == RTPCS_SDS_MODE_USXGMII_10GDXGMII ||
		hw_mode == RTPCS_SDS_MODE_USXGMII_10GQXGMII ||
		hw_mode == RTPCS_SDS_MODE_USXGMII_5GSXGMII ||
		hw_mode == RTPCS_SDS_MODE_USXGMII_5GDXGMII ||
		hw_mode == RTPCS_SDS_MODE_USXGMII_2_5GSXGMII);
}

/* Generic auto-negotiation config */

static int rtpcs_generic_sds_set_autoneg(struct rtpcs_serdes *sds, unsigned int neg_mode,
					 const unsigned long *advertising)
{
	u16 bmcr, adv, adv_old;
	bool changed = false;
	int ret;

	if ((sds->hw_mode == RTPCS_SDS_MODE_1000BASEX) ||
	    (sds->hw_mode == RTPCS_SDS_MODE_2500BASEX)) {
		adv = ADVERTISE_1000XFULL;
		if (linkmode_test_bit(ETHTOOL_LINK_MODE_Pause_BIT,
				      advertising))
			adv |= ADVERTISE_1000XPAUSE;
		if (linkmode_test_bit(ETHTOOL_LINK_MODE_Asym_Pause_BIT,
				      advertising))
			adv |= ADVERTISE_1000XPSE_ASYM;

		adv_old = rtpcs_sds_read_field(sds, &sds->regs->an_advertise);
		if (adv_old < 0)
			return adv_old;

		if (adv != adv_old) {
			changed = true;
			ret = rtpcs_sds_write_field(sds, &sds->regs->an_advertise, adv);
			if (ret < 0)
				return ret;
		}
	}

	bmcr = neg_mode == PHYLINK_PCS_NEG_INBAND_ENABLED ? 1 : 0;

	ret = rtpcs_sds_write_field(sds, &sds->regs->an_enable, bmcr);
	if (ret < 0)
		return ret;

	return changed;
}

static void rtpcs_generic_sds_restart_autoneg(struct rtpcs_serdes *sds)
{
	rtpcs_sds_write_field(sds, &sds->regs->an_restart, 0x1);
}

static int rtpcs_sds_select_pll_speed(enum rtpcs_sds_mode hw_mode, enum rtpcs_sds_pll_speed *speed)
{
	switch (hw_mode) {
	case RTPCS_SDS_MODE_1000BASEX:
	case RTPCS_SDS_MODE_SGMII:
	case RTPCS_SDS_MODE_QSGMII:
		*speed = RTPCS_SDS_PLL_SPD_1000;
		break;
	case RTPCS_SDS_MODE_2500BASEX:
		*speed = RTPCS_SDS_PLL_SPD_2500;
		break;
	case RTPCS_SDS_MODE_10GBASER:
	case RTPCS_SDS_MODE_XSGMII:
	case RTPCS_SDS_MODE_USXGMII_10GSXGMII:
	case RTPCS_SDS_MODE_USXGMII_10GDXGMII:
	case RTPCS_SDS_MODE_USXGMII_10GQXGMII:
	case RTPCS_SDS_MODE_USXGMII_5GSXGMII:
	case RTPCS_SDS_MODE_USXGMII_5GDXGMII:
	case RTPCS_SDS_MODE_USXGMII_2_5GSXGMII:
		*speed = RTPCS_SDS_PLL_SPD_10000;
		break;
	default:
		return -ENOTSUPP;
	}

	return 0;
}

static int rtpcs_sds_apply_config(struct rtpcs_serdes *sds,
				  const struct rtpcs_sds_config *config, size_t count)
{
	int ret;

	for (size_t i = 0; i < count; i++) {
		ret = rtpcs_sds_write(sds, config[i].page, config[i].reg, config[i].data);
		if (ret)
			return ret;
	}
	return 0;
}

static int rtpcs_sds_apply_config_xsg(struct rtpcs_serdes *sds,
				      const struct rtpcs_sds_config *config, size_t count)
{
	int ret;

	for (size_t i = 0; i < count; i++) {
		ret = rtpcs_sds_xsg_write(sds, config[i].page, config[i].reg, config[i].data);
		if (ret)
			return ret;
	}
	return 0;
}

/*
 * Allocate a regmap_field on the SoC-side register map for this SerDes and
 * store the resulting pointer in *dst. Convenience helper for per-SerDes
 * register fields computed from the SerDes ID. Taking reg/lsb/msb as
 * integer arguments (rather than a struct reg_field) keeps callers free of
 * either local reg_field declarations or compound-literal casts, since
 * REG_FIELD() is a brace-initializer and not a usable expression.
 */
static int rtpcs_sds_alloc_field(struct rtpcs_serdes *sds, struct regmap_field **dst,
				 u32 reg, u8 lsb, u8 msb)
{
	struct reg_field rf = REG_FIELD(reg, lsb, msb);

	*dst = devm_regmap_field_alloc(sds->ctrl->dev, sds->ctrl->map, rf);
	return PTR_ERR_OR_ZERO(*dst);
}

/*
 * Write the SerDes MAC mode register. This is the common minimum shared by
 * all variants. Variant-specific extras (force bit, companion registers,
 * USXGMII submode, post-write delay) live in per-variant wrappers.
 */
static int rtpcs_sds_set_mac_mode(struct rtpcs_serdes *sds, enum rtpcs_sds_mode hw_mode)
{
	const struct rtpcs_config *cfg = sds->ctrl->cfg;
	int val;

	if (hw_mode >= RTPCS_SDS_MODE_MAX)
		return -EINVAL;

	val = cfg->sds_hw_mode_vals[hw_mode];
	if (val < 0)
		return -EOPNOTSUPP;

	return regmap_field_write(sds->swcore_regs.mac_mode, val);
}

/* Variant-specific functions */

/* RTL838X */

/* RTL838X SDS_MODE_SEL field values */
static const s16 rtpcs_838x_sds_hw_mode_vals[RTPCS_SDS_MODE_MAX] = {
	[0 ... RTPCS_SDS_MODE_MAX - 1]	= -1,
	[RTPCS_SDS_MODE_1000BASEX]	= 0x4,
	[RTPCS_SDS_MODE_SGMII]		= 0x2,
	[RTPCS_SDS_MODE_QSGMII]		= 0x6,
};

static void rtpcs_838x_sds_patch_qsgmii(struct rtpcs_serdes *sds)
{
	bool is_even = (rtpcs_sds_get_even(sds) == sds);

	rtpcs_sds_write(sds, PAGE_SDS_EXT, 0x3, 0xf46d);
	rtpcs_sds_write(sds, PAGE_SDS_EXT, 0x2, 0x85fa);
	rtpcs_sds_write(sds, PAGE_SDS_EXT, 0x6, 0x20d8);

	rtpcs_sds_write(sds, PAGE_SDS_EXT, 10, is_even ? 0x58c7 : 0x80c7);
	rtpcs_sds_write(sds, PAGE_SDS_EXT, 17, is_even ? 0xb7c9 : 0x4208);
	rtpcs_sds_write(sds, PAGE_SDS_EXT, 18, is_even ? 0xab8e : 0xc208);
	rtpcs_sds_write(sds, PAGE_SDS_EXT, 11, 0x482);
	if (is_even)
		rtpcs_sds_write(sds, PAGE_SDS_EXT, 19, 0x24ab);

	rtpcs_sds_write(sds, PAGE_FIB, 25, 0x303);
	rtpcs_sds_write(sds, PAGE_SDS_EXT, 14, 0xfcc2);
}

static void rtpcs_838x_sds_patch_fiber(struct rtpcs_serdes *sds)
{
	bool is_even = rtpcs_sds_get_even(sds) == sds;

	rtpcs_sds_write(sds, PAGE_SDS_EXT, 2, 0x85fa);
	rtpcs_sds_write(sds, PAGE_SDS_EXT, 3, 0x00);
	rtpcs_sds_write(sds, PAGE_SDS_EXT, 4, 0xdccc);
	rtpcs_sds_write(sds, PAGE_SDS_EXT, 5, 0x00);
	rtpcs_sds_write(sds, PAGE_SDS_EXT, 6, is_even ? 0x20d8 : 0x3600);
	rtpcs_sds_write(sds, PAGE_SDS_EXT, 7, 0x03);
	rtpcs_sds_write(sds, PAGE_SDS_EXT, 8, 0x79aa);
	rtpcs_sds_write(sds, PAGE_SDS_EXT, 9, 0x8c64);
	rtpcs_sds_write(sds, PAGE_SDS_EXT, 10, 0xc3);
	rtpcs_sds_write(sds, PAGE_SDS_EXT, 11, 0x1482);
	rtpcs_sds_write(sds, PAGE_SDS_EXT, 17, 0xb7c9);
	rtpcs_sds_write(sds, PAGE_SDS_EXT, 18, 0xab8e);
	rtpcs_sds_write(sds, PAGE_FIB, 24, 0x14aa);
	rtpcs_sds_write(sds, PAGE_FIB, 25, 0x303);
	rtpcs_sds_write(sds, PAGE_SDS_EXT, 14, 0xf002);
	rtpcs_sds_write(sds, PAGE_FIB, 27, 0x4bf);
}

static void rtpcs_838x_sds_reset(struct rtpcs_serdes *sds)
{
	rtpcs_sds_write_bits(sds, PAGE_SDS, 3, 6, 6, 0x1); /* REG3 SOFT_RST */
	rtpcs_sds_write_bits(sds, PAGE_SDS, 3, 6, 6, 0x0); /* REG3 SOFT_RST */
}

static void rtpcs_838x_sds_fill_caps(struct rtpcs_serdes *sds)
{
	__set_bit(RTPCS_SDS_MODE_OFF, sds->supported_modes);

	if (sds->id <= 4)
		__set_bit(RTPCS_SDS_MODE_QSGMII, sds->supported_modes);

	if (sds->id >= 4) {
		__set_bit(RTPCS_SDS_MODE_SGMII, sds->supported_modes);
		__set_bit(RTPCS_SDS_MODE_1000BASEX, sds->supported_modes);
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

static int rtpcs_838x_sds_deactivate(struct rtpcs_serdes *sds)
{
	int ret;

	ret = rtpcs_838x_sds_power(sds, false);
	if (ret)
		return ret;

	/* EN_RX | EN_TX */
	ret = rtpcs_sds_write_bits(sds, PAGE_SDS, 0, 1, 0, 0x0);
	if (ret)
		return ret;

	/* CFG_FIB_PDOWN / BMCR_PDOWN */
	return rtpcs_sds_write_bits(sds, PAGE_FIB, MII_BMCR, 11, 11, 0x1);
}

static int rtpcs_838x_sds_activate(struct rtpcs_serdes *sds)
{
	int ret;

	rtpcs_838x_sds_reset(sds);

	/* CFG_FIB_PDOWN / BMCR_PDOWN */
	ret = rtpcs_sds_write_bits(sds, PAGE_FIB, MII_BMCR, 11, 11, 0x0);
	if (ret)
		return ret;

	/* EN_RX | EN_TX */
	ret = rtpcs_sds_write_bits(sds, PAGE_SDS, 0, 1, 0, 0x3);
	if (ret)
		return ret;

	return rtpcs_838x_sds_power(sds, true);
}

/*
 * RTL838X wrapper: after setting the MAC mode, SerDes 4-5 also need the
 * companion INT_MODE_CTRL field written.
 */
static int rtpcs_838x_sds_set_mode(struct rtpcs_serdes *sds, enum rtpcs_sds_mode hw_mode)
{
	u8 int_mode_shift, int_mode_val;
	int ret;

	ret = rtpcs_sds_set_mac_mode(sds, hw_mode);
	if (ret)
		return ret;

	if (sds->id < 4)
		return 0;

	int_mode_shift = (sds->id == 5) ? 3 : 0;
	switch (hw_mode) {
	case RTPCS_SDS_MODE_1000BASEX:
		int_mode_val = 0x1;
		break;
	case RTPCS_SDS_MODE_SGMII:
		int_mode_val = 0x2;
		break;
	case RTPCS_SDS_MODE_QSGMII:
		int_mode_val = 0x5;
		break;
	default:
		return -EINVAL;
	}

	return regmap_write_bits(sds->ctrl->map, RTPCS_838X_INT_MODE_CTRL,
				 0x7 << int_mode_shift, int_mode_val << int_mode_shift);
}

static int rtpcs_838x_sds_config_hw_mode(struct rtpcs_serdes *sds, enum rtpcs_sds_mode hw_mode)
{
	struct rtpcs_ctrl *ctrl = sds->ctrl;
	u8 sds_id = sds->id;

	rtpcs_sds_write(sds, PAGE_SDS, 1, 0xf00);
	usleep_range(1000, 2000);
	rtpcs_sds_write(sds, PAGE_SDS, 2, 0x7060);
	usleep_range(1000, 2000);

	if (sds_id >= 4) {
		rtpcs_sds_write(sds, PAGE_FIB, 30, 0x71e);
		usleep_range(1000, 2000);
		rtpcs_sds_write(sds, PAGE_SDS, 4, 0x74d);
		usleep_range(1000, 2000);
	}

	switch (hw_mode) {
	case RTPCS_SDS_MODE_1000BASEX:
		rtpcs_838x_sds_patch_fiber(sds);
		break;
	case RTPCS_SDS_MODE_QSGMII:
		rtpcs_838x_sds_patch_qsgmii(sds);
		break;
	default:
		break;
	}

	if (sds_id < 2) {
		/*
		 * These settings have to match to make QSGMII working.
		 * Testing showed that both variants work:
		 *   - CKREFBUF_S0S1 = 0xf + REG_CML_SEL = 0x1
		 *   - CKREFBUF_S0S1 = 0x0 + REG_CML_SEL = 0x0
		 */

		/* CKREFBUF_S0S1 */
		regmap_write_bits(ctrl->map, RTPCS_838X_PLL_CML_CTRL, 0xf, 0xf);
		rtpcs_sds_write_bits(sds, PAGE_SDS_EXT, 0x3, 1, 1, 0x1); /* REG_CML_SEL */
	}

	rtpcs_sds_write(sds, PAGE_SDS_EXT, 0x9, 0x8e64);
	rtpcs_sds_write(sds, PAGE_SDS_EXT, 0x9, 0x8c64);

	return 0;
}

static int rtpcs_838x_sds_probe(struct rtpcs_serdes *sds)
{
	u8 lsb = (5 - sds->id) * 5;

	sds->type = RTPCS_SDS_TYPE_5G;

	rtpcs_838x_sds_fill_caps(sds);

	/*
	 * SDS_MODE_SEL packs 5-bit fields in reverse order: SDS 0 at [25:29],
	 * SDS 5 at [0:4].
	 */
	return rtpcs_sds_alloc_field(sds, &sds->swcore_regs.mac_mode,
				     RTPCS_838X_SDS_MODE_SEL, lsb, lsb + 4);
}

static int rtpcs_838x_init(struct rtpcs_ctrl *ctrl)
{
	dev_dbg(ctrl->dev, "Init RTL838X PCS\n");

	/* power off and reset all SerDes */
	regmap_write(ctrl->map, RTPCS_838X_SDS_CFG_REG, 0x3f);
	regmap_write(ctrl->map, RTPCS_838X_RST_GLB_CTRL_0, 0x10); /* SW_SERDES_RST */
	return 0;
}

static int rtpcs_838x_sds_post_config(struct rtpcs_serdes *sds, enum rtpcs_sds_mode hw_mode)
{
	/*
	 * Run a switch queue reset after the first start of a SerDes. This recovers ports that
	 * were already connected during boot and will not pass traffic. Sometimes the bug can
	 * be seen in registers INGR_DBG_REG0-INGR_DBG_REG2 but this is quite erratic. The SDK
	 * seems to have no issues because it starts all SerDes then PHYs and runs a queue reset
	 * finally during NIC start.
	 *
	 * Of course this is totally wrong here and should be part of the DSA driver. But
	 * implementing it over there requires more tricks than this (e.g. delayed work).
	 */
	if (sds->first_start)
		regmap_write(sds->ctrl->map, RTPCS_838X_RST_GLB_CTRL_0, 0x4);

	return 0;
}

/* RTL839X */

/*
 * RTL839X MAC_SERDES_IF_CTRL mode values.
 * From the vendor SDK; 100BASEX (0x8), 1000BASEX/SGMII (0x7) are documented
 * but not yet exercised here.
 */
static const s16 rtpcs_839x_sds_hw_mode_vals[RTPCS_SDS_MODE_MAX] = {
	[0 ... RTPCS_SDS_MODE_MAX - 1]  = -1,
	[RTPCS_SDS_MODE_OFF]            = 0x0,
	/* [RTPCS_SDS_MODE_100BASEX]    = 0x8, */
	/* [RTPCS_SDS_MODE_1000BASEX]   = 0x7, */
	/* [RTPCS_SDS_MODE_SGMII]       = 0x7, */
	[RTPCS_SDS_MODE_QSGMII]         = 0x6,
};

static void rtpcs_839x_sds_reset(struct rtpcs_serdes *sds)
{
	struct rtpcs_serdes *even_sds = rtpcs_sds_get_even(sds);
	struct rtpcs_serdes *odd_sds = rtpcs_sds_get_odd(sds);

	/* FIXME: The reset sequence seems to break some of the 5G SerDes
	 * though the SDK is calling it for all SerDes during init. Until
	 * this is solved, skip reset.
	 */
	if (sds->type == RTPCS_SDS_TYPE_5G)
		return;

	if (sds->type == RTPCS_SDS_TYPE_10G) {
		rtpcs_sds_write_bits(odd_sds, PAGE_ANA_10G_EXT, 0x1d, 3, 0, 0x5);
		msleep(500);
		rtpcs_sds_write_bits(odd_sds, PAGE_ANA_10G_EXT, 0x1d, 3, 0, 0xf);
		rtpcs_sds_write_bits(odd_sds, PAGE_ANA_10G_EXT, 0x1d, 3, 0, 0x0);

		rtpcs_sds_write_bits(even_sds, PAGE_ANA_10G, 0x10, 3, 3, 0x0);
		rtpcs_sds_write_bits(even_sds, PAGE_ANA_10G_EXT, 0x0, 15, 15, 0x1);
		msleep(100);
		rtpcs_sds_write_bits(even_sds, PAGE_ANA_10G_EXT, 0x0, 15, 15, 0x0);
	} else {
		rtpcs_sds_write(odd_sds, PAGE_ANA_1G2_EXT, 0x1, 0x0050);
		rtpcs_sds_write(odd_sds, PAGE_ANA_1G2_EXT, 0x1, 0x00f0);
		rtpcs_sds_write(odd_sds, PAGE_ANA_1G2_EXT, 0x1, 0x0000);

		rtpcs_sds_write_bits(sds, PAGE_ANA_1G2, 0x14, 0, 0, 0x0);
		rtpcs_sds_write_bits(sds, PAGE_ANA_1G2, 0x14, 9, 9, 0x1);
		msleep(100);
		rtpcs_sds_write_bits(sds, PAGE_ANA_1G2, 0x14, 9, 9, 0x0);
	}

	rtpcs_sds_write(even_sds, PAGE_SDS, 0x3, 0x7146);
	msleep(100);
	rtpcs_sds_write(even_sds, PAGE_SDS, 0x3, 0x7106);

	rtpcs_sds_write(odd_sds, PAGE_SDS, 0x3, 0x7146);
	msleep(100);
	rtpcs_sds_write(odd_sds, PAGE_SDS, 0x3, 0x7106);
}

static void rtpcs_839x_sds_fill_caps(struct rtpcs_serdes *sds)
{
	__set_bit(RTPCS_SDS_MODE_OFF, sds->supported_modes);

	if (sds->id <= 12)
		__set_bit(RTPCS_SDS_MODE_QSGMII, sds->supported_modes);

	/* Uncomment this when modes are supported
	if (sds->id >= 12) {
		__set_bit(RTPCS_SDS_MODE_SGMII, sds->supported_modes);
		__set_bit(RTPCS_SDS_MODE_100BASEX, sds->supported_modes);
		__set_bit(RTPCS_SDS_MODE_1000BASEX, sds->supported_modes);
	}
	*/
}

static int rtpcs_839x_sds_probe(struct rtpcs_serdes *sds)
{
	u8 id = sds->id;
	bool is_even = id % 2 == 0;
	u8 lsb = (id % 8) * 4;
	int ret;

	ret = rtpcs_sds_alloc_field(sds, &sds->swcore_regs.mac_mode,
				    RTPCS_839X_MAC_SERDES_IF_CTRL + (id / 8) * 4,
				    lsb, lsb + 3);
	if (ret)
		return ret;

	if (id == 8 || id == 9 || id == 12 || id == 13)
		sds->type = RTPCS_SDS_TYPE_10G;
	else
		sds->type = RTPCS_SDS_TYPE_5G;

	rtpcs_839x_sds_fill_caps(sds);

	/*
	 * This function is quite "mystic". It has been taken over from the vendor SDK function
	 * rtl839x_serdes_patch_init(). There is not much documentation about it but one could
	 * lookup the fields from the field headers. The 5G SerDes seem to work out of the box
	 * so only setup the 10G SerDes for now.
	 */
	if (sds->type == RTPCS_SDS_TYPE_5G)
		return 0;

	/* Part 1: register setup */
	rtpcs_sds_write(sds, PAGE_ANA_10G, 0x0, 0x5800);
	rtpcs_sds_write(sds, PAGE_ANA_10G, 0x1, 0x4000);
	rtpcs_sds_write(sds, PAGE_ANA_10G, 0x2, is_even ? 0x5400 : 0x5000);
	rtpcs_sds_write(sds, PAGE_ANA_10G, 0x3, 0x0000);
	rtpcs_sds_write(sds, PAGE_ANA_10G, 0x4, 0x0000);
	rtpcs_sds_write(sds, PAGE_ANA_10G, 0x5, 0x4000);
	rtpcs_sds_write(sds, PAGE_ANA_10G, 0x6, 0x4000);
	rtpcs_sds_write(sds, PAGE_ANA_10G, 0x7, 0xffff);
	rtpcs_sds_write(sds, PAGE_ANA_10G, 0x8, 0xffff);
	rtpcs_sds_write(sds, PAGE_ANA_10G, 0x9, 0x806f);
	rtpcs_sds_write(sds, PAGE_ANA_10G, 0xa, 0x0004);
	rtpcs_sds_write(sds, PAGE_ANA_10G, 0xb, 0x0000);
	rtpcs_sds_write(sds, PAGE_ANA_10G, 0xc, 0x0000);
	rtpcs_sds_write(sds, PAGE_ANA_10G, 0xd, 0x0000);
	rtpcs_sds_write(sds, PAGE_ANA_10G, 0xe, 0x0a00);
	rtpcs_sds_write(sds, PAGE_ANA_10G, 0xf, 0x2000);
	rtpcs_sds_write(sds, PAGE_ANA_10G, 0x10, 0xf00e);
	rtpcs_sds_write(sds, PAGE_ANA_10G, 0x11, is_even ? 0xf04a : 0xfdab);
	rtpcs_sds_write(sds, PAGE_ANA_10G, 0x12, is_even ? 0x97b3 : 0x96ea);
	rtpcs_sds_write(sds, PAGE_ANA_10G, 0x13, 0x5318);
	rtpcs_sds_write(sds, PAGE_ANA_10G, 0x14, 0x0f03);
	rtpcs_sds_write(sds, PAGE_ANA_10G, 0x15, 0x0000);
	rtpcs_sds_write(sds, PAGE_ANA_10G, 0x16, 0x0000);
	rtpcs_sds_write(sds, PAGE_ANA_10G, 0x17, 0x0000);
	rtpcs_sds_write(sds, PAGE_ANA_10G, 0x18, 0x0000);
	rtpcs_sds_write(sds, PAGE_ANA_10G, 0x19, 0x0000);
	rtpcs_sds_write(sds, PAGE_ANA_10G, 0x1a, 0xffff);
	rtpcs_sds_write(sds, PAGE_ANA_10G, 0x1b, 0x0000);
	rtpcs_sds_write(sds, PAGE_ANA_10G, 0x1c, 0x1203);
	rtpcs_sds_write(sds, PAGE_ANA_10G, 0x1d, 0x0000);
	rtpcs_sds_write(sds, PAGE_ANA_10G, 0x1e, 0xa052);
	rtpcs_sds_write(sds, PAGE_ANA_10G, 0x1f, 0x9a00);
	rtpcs_sds_write(sds, PAGE_ANA_10G_EXT, 0x0, 0x00f5);
	rtpcs_sds_write(sds, PAGE_ANA_10G_EXT, 0x1, 0xf000);
	rtpcs_sds_write(sds, PAGE_ANA_10G_EXT, 0x2, is_even ? 0x41ff : 0x4079);
	rtpcs_sds_write(sds, PAGE_ANA_10G_EXT, 0x3, 0x0000);
	rtpcs_sds_write(sds, PAGE_ANA_10G_EXT, 0x4, is_even ? 0x39ff : 0x93fa);
	rtpcs_sds_write(sds, PAGE_ANA_10G_EXT, 0x5, 0x3340);
	rtpcs_sds_write(sds, PAGE_ANA_10G_EXT, 0x6, is_even ? 0x40aa : 0x4280);
	rtpcs_sds_write(sds, PAGE_ANA_10G_EXT, 0x7, 0x0000);
	rtpcs_sds_write(sds, PAGE_ANA_10G_EXT, 0x8, 0x801f);
	rtpcs_sds_write(sds, PAGE_ANA_10G_EXT, 0x9, 0x0000);
	rtpcs_sds_write(sds, PAGE_ANA_10G_EXT, 0xa, 0x619c);
	rtpcs_sds_write(sds, PAGE_ANA_10G_EXT, 0xb, 0xffed);
	rtpcs_sds_write(sds, PAGE_ANA_10G_EXT, 0xc, 0x29ff);
	rtpcs_sds_write(sds, PAGE_ANA_10G_EXT, 0xd, 0x29ff);
	rtpcs_sds_write(sds, PAGE_ANA_10G_EXT, 0xe, is_even ? 0x4e10 : 0x4c50);
	rtpcs_sds_write(sds, PAGE_ANA_10G_EXT, 0xf, is_even ? 0x4e10 : 0x4c50);
	rtpcs_sds_write(sds, PAGE_ANA_10G_EXT, 0x10, 0x0000);
	rtpcs_sds_write(sds, PAGE_ANA_10G_EXT, 0x11, 0x0000);
	rtpcs_sds_write(sds, PAGE_SDS, 0xc, 0x08ec);
	if (!is_even)
		rtpcs_sds_write(sds, PAGE_ANA_10G_EXT, 0x1f, 0x003f);

	/* Part 2: register bit patching (contains some "reset flips") */
	rtpcs_sds_write_bits(sds, PAGE_SDS, 0x7, 14, 14, 0x0001);
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G_EXT, 0x9, 15, 0, 0x417f);
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x1c, 9, 9, 0x0000);
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x1c, 12, 10, 0x0000);
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x1c, 5, 3, 0x0005);
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x1c, 8, 6, 0x0000);
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x1c, 2, 0, 0x0002);
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G_EXT, 0x1, 15, 0, 0xc440);
	if (is_even)
		rtpcs_sds_write_bits(sds, PAGE_ANA_10G_EXT, 0x6, 3, 3, 0x0000);
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x5, 15, 0, 0x8000);
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x6, 15, 0, 0x8000);
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0xa, 15, 0, 0x0000);
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x1e, 15, 0, 0x0002);
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x1f, 15, 0, 0xbe00);
	if (is_even) {
		rtpcs_sds_write_bits(sds, PAGE_ANA_10G_EXT, 0xe, 10, 10, 0x0000);
		rtpcs_sds_write_bits(sds, PAGE_ANA_10G_EXT, 0xf, 10, 10, 0x0000);
		rtpcs_sds_write_bits(sds, PAGE_ANA_10G_EXT, 0xe, 14, 14, 0x0000);
		rtpcs_sds_write_bits(sds, PAGE_ANA_10G_EXT, 0xf, 14, 14, 0x0000);
	}
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x10, 5, 5, 0x0000);
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G_EXT, 0x9, 8, 8, 0x0000);
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x3, 15, 12, 0x000f);
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x1f, 13, 12, 0x0003);
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x1f, 11, 9, 0x0007);
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G_EXT, 0x1, 15, 15, 0x0001);
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G_EXT, 0x1, 14, 14, 0x0001);
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G_EXT, 0x1, 13, 13, 0x0000);
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G_EXT, 0x1, 12, 12, 0x0000);
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G_EXT, 0x1, 11, 9, 0x0002);
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G_EXT, 0x1, 8, 6, 0x0002);
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G_EXT, 0x1, 5, 3, 0x0000);
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G_EXT, 0x1, 2, 0, 0x0000);
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G_EXT, 0xc, 9, 9, 0x0001);
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G_EXT, 0xd, 9, 9, 0x0001);
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G_EXT, 0x8, 5, 5, 0x0001);
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G_EXT, 0x8, 6, 6, 0x0000);
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x1c, 15, 15, 0x0000);
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x10, 15, 12, 0x0000);
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x13, 4, 4, 0x0000);
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x13, 9, 9, 0x0000);
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x13, 3, 0, 0x0008);
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x13, 8, 5, 0x0008);

	return 0;
}

static int rtpcs_839x_init(struct rtpcs_ctrl *ctrl)
{
	/* reset all SerDes once after patching has been applied before */
	for (int sds_id = 0; sds_id < ctrl->cfg->serdes_count; sds_id++)
		rtpcs_839x_sds_reset(&ctrl->serdes[sds_id]);

	return 0;
}

/*
 * These no-op stubs satisfy the mandatory activate/deactivate contract until
 * real power sequencing is implemented.
 */
static int rtpcs_839x_sds_deactivate(struct rtpcs_serdes *sds)
{
	return 0;
}

static int rtpcs_839x_sds_activate(struct rtpcs_serdes *sds)
{
	rtpcs_839x_sds_reset(sds);
	return 0;
}

/*
 * Keep this as a no-op stub until RTL839x is extended to do proper configuration
 * here. E.g., the still missing SGMII, 100BASEX and 1000BASEX setup should go here.
 */
static int rtpcs_839x_sds_config_hw_mode(struct rtpcs_serdes *sds, enum rtpcs_sds_mode hw_mode)
{
	return 0;
}

/* RTL93XX */

/* forward mapping: enum rtpcs_sds_mode, -1 = unsupported */
static const s16 rtpcs_93xx_sds_hw_mode_vals[RTPCS_SDS_MODE_MAX] = {
	[0 ... RTPCS_SDS_MODE_MAX - 1]		= -1,
	[RTPCS_SDS_MODE_OFF]			= RTPCS_93XX_SDS_MODE_OFF,
	[RTPCS_SDS_MODE_SGMII]			= RTPCS_93XX_SDS_MODE_SGMII,
	[RTPCS_SDS_MODE_1000BASEX]		= RTPCS_93XX_SDS_MODE_1000BASEX,
	[RTPCS_SDS_MODE_2500BASEX]		= RTPCS_93XX_SDS_MODE_2500BASEX,
	[RTPCS_SDS_MODE_10GBASER]		= RTPCS_93XX_SDS_MODE_10GBASER,
	[RTPCS_SDS_MODE_QSGMII]			= RTPCS_93XX_SDS_MODE_QSGMII,
	[RTPCS_SDS_MODE_XSGMII]			= RTPCS_93XX_SDS_MODE_XSGMII,
	[RTPCS_SDS_MODE_USXGMII_10GSXGMII]	= RTPCS_93XX_SDS_MODE_USXGMII,
	[RTPCS_SDS_MODE_USXGMII_10GDXGMII]	= RTPCS_93XX_SDS_MODE_USXGMII,
	[RTPCS_SDS_MODE_USXGMII_10GQXGMII]	= RTPCS_93XX_SDS_MODE_USXGMII,
	[RTPCS_SDS_MODE_USXGMII_5GSXGMII]	= RTPCS_93XX_SDS_MODE_USXGMII,
	[RTPCS_SDS_MODE_USXGMII_5GDXGMII]	= RTPCS_93XX_SDS_MODE_USXGMII,
	[RTPCS_SDS_MODE_USXGMII_2_5GSXGMII]	= RTPCS_93XX_SDS_MODE_USXGMII,
};

static int rtpcs_93xx_sds_set_autoneg(struct rtpcs_serdes *sds, unsigned int neg_mode,
				      const unsigned long *advertising)
{
	u16 en_val;

	switch (sds->hw_mode) {
	case RTPCS_SDS_MODE_XSGMII: /* XSG N-way state */
		en_val = neg_mode == PHYLINK_PCS_NEG_INBAND_ENABLED ? 0x0 : 0x1;

		return rtpcs_sds_xsg_write_bits(sds, PAGE_SDS, 0x2, 9, 8, en_val);

	case RTPCS_SDS_MODE_USXGMII_10GSXGMII ... RTPCS_SDS_MODE_USXGMII_2_5GSXGMII:
		/*
		 * CFG_QHSG_AN_EN_CHX: bits [3:0] enable AN on channels 3..0
		 *
		 * We do not support forced USXGMII link yet, always activate USXGMII-AN
		 * for now.
		 */
		return rtpcs_sds_write_bits(sds, PAGE_TGR_PRO_1, 0x11, 3, 0, 0xf);

	default:
		return rtpcs_generic_sds_set_autoneg(sds, neg_mode, advertising);
	}
}

static void rtpcs_93xx_sds_usxgmii_config(struct rtpcs_serdes *sds, u32 opcode, u32 am_period,
					  u32 all_am_markers, u32 an_table, u32 sync_bit)
{
	/* this comes from USXGMII patch sequences of the SDK */
	rtpcs_sds_write(sds, PAGE_TGR_PRO_0, 0x00, 0x0000);
	rtpcs_sds_write(sds, PAGE_TGR_PRO_0, 0x0D, 0x0F00);
	rtpcs_sds_write(sds, PAGE_TGR_PRO_0, 0x1D, 0x0600);

	rtpcs_sds_write(sds, PAGE_TGR_PRO_1, 0x06, 0x1401); /* CFG_QHSG_TXCFG_MAC_CH0 */
	rtpcs_sds_write(sds, PAGE_TGR_PRO_1, 0x08, 0x1401); /* CFG_QHSG_TXCFG_MAC_CH1 */
	rtpcs_sds_write(sds, PAGE_TGR_PRO_1, 0x0a, 0x1401); /* CFG_QHSG_TXCFG_MAC_CH2 */
	rtpcs_sds_write(sds, PAGE_TGR_PRO_1, 0x0c, 0x1401); /* CFG_QHSG_TXCFG_MAC_CH3 */

	/*
	 * Controls the USXGMII AN mode. Two states are currently known:
	 * - 0x03: generic/standard-compliant mode
	 * - 0xaa: Realtek-proprietary mode (e.g. RTL8224)
	 */
	rtpcs_sds_write_bits(sds, PAGE_TGR_PRO_1, 0x10, 7, 0, opcode); /* CFG_QHSG_AN_OPC */

	rtpcs_sds_write_bits(sds, PAGE_TGR_PRO_0, 0x12, 15, 0, am_period);
	rtpcs_sds_write_bits(sds, PAGE_TGR_PRO_0, 0x13, 7,  0, all_am_markers); /* CFG_AM0_M0 */
	rtpcs_sds_write_bits(sds, PAGE_TGR_PRO_0, 0x13, 15, 8, all_am_markers); /* CFG_AM0_M1 */
	rtpcs_sds_write_bits(sds, PAGE_TGR_PRO_0, 0x14, 7,  0, all_am_markers); /* CFG_AM0_M2 */
	rtpcs_sds_write_bits(sds, PAGE_TGR_PRO_0, 0x14, 15, 8, all_am_markers); /* CFG_AM1_M0 */
	rtpcs_sds_write_bits(sds, PAGE_TGR_PRO_0, 0x15, 7,  0, all_am_markers); /* CFG_AM1_M1 */
	rtpcs_sds_write_bits(sds, PAGE_TGR_PRO_0, 0x15, 15, 8, all_am_markers); /* CFG_AM1_M2 */
	rtpcs_sds_write_bits(sds, PAGE_TGR_PRO_0, 0x16, 7,  0, all_am_markers); /* CFG_AM2_M0 */
	rtpcs_sds_write_bits(sds, PAGE_TGR_PRO_0, 0x16, 15, 8, all_am_markers); /* CFG_AM2_M1 */
	rtpcs_sds_write_bits(sds, PAGE_TGR_PRO_0, 0x17, 7,  0, all_am_markers); /* CFG_AM2_M2 */
	rtpcs_sds_write_bits(sds, PAGE_TGR_PRO_0, 0x17, 15, 8, all_am_markers); /* CFG_AM3_M0 */
	rtpcs_sds_write_bits(sds, PAGE_TGR_PRO_0, 0x18, 7,  0, all_am_markers); /* CFG_AM3_M1 */
	rtpcs_sds_write_bits(sds, PAGE_TGR_PRO_0, 0x18, 15, 8, all_am_markers); /* CFG_AM3_M2 */
	rtpcs_sds_write_bits(sds, PAGE_TGR_PRO_0, 0xe, 10, 10, an_table);
	rtpcs_sds_write_bits(sds, PAGE_TGR_PRO_0, 0x1d, 11, 10, sync_bit);

	rtpcs_sds_write_bits(sds, PAGE_TGR_PRO_0, 0x03, 15, 15, 0x1); /* FP_TGR3_CFG_EEE_EN */
}

static int rtpcs_93xx_init(struct rtpcs_ctrl *ctrl)
{
	u32 model_info = 0;
	int rl_vid, val;

	regmap_read(ctrl->map, RTPCS_93XX_MODEL_NAME_INFO, &model_info);
	if (model_info & BIT(4))
		dev_warn(ctrl->dev, "ES chip variants may not work properly!\n");

	val = 0xa0000; /* CHIP_INFO_EN */
	regmap_write(ctrl->map, RTPCS_93XX_CHIP_INFO, val);
	regmap_read(ctrl->map, RTPCS_93XX_CHIP_INFO, &val);
	rl_vid = FIELD_GET(GENMASK(31, 28), val);

	if (rl_vid & BIT(0))
		ctrl->chip_version = RTPCS_CHIP_V2;

	val = 0;
	regmap_write(ctrl->map, RTPCS_93XX_CHIP_INFO, val);

	dev_dbg(ctrl->dev, "chip_version %u\n", ctrl->chip_version + 1);
	return 0;
}

static int rtpcs_93xx_sds_get_pll_config(struct rtpcs_serdes *sds, enum rtpcs_sds_pll_type pll,
					 enum rtpcs_sds_pll_speed *speed)
{
	struct rtpcs_serdes *even_sds = rtpcs_sds_get_even(sds);
	int sbit, speed_val;

	/*
	 * PLL config is shared between adjacent SerDes in the even lane. Each SerDes defines
	 * what PLL it needs (ring or LC) while the PLL itself stores the current speed.
	 */

	sbit = pll == RTPCS_SDS_PLL_TYPE_LC ? 8 : 12;
	speed_val = rtpcs_sds_read_bits(even_sds, PAGE_ANA_MISC, 0x12, sbit + 3, sbit);
	if (speed_val < 0)
		return speed_val;

	/* bit 0 is force-bit, bits [3:1] are speed selector */
	*speed = (enum rtpcs_sds_pll_speed)(speed_val >> 1);
	return 0;
}

static int rtpcs_93xx_sds_set_pll_config(struct rtpcs_serdes *sds, enum rtpcs_sds_pll_type pll,
					 enum rtpcs_sds_pll_speed speed)
{
	struct rtpcs_serdes *even_sds = rtpcs_sds_get_even(sds);
	int sbit = pll == RTPCS_SDS_PLL_TYPE_LC ? 8 : 12;
	int ret;

	if (speed >= RTPCS_SDS_PLL_SPD_END)
		return -EINVAL;

	if (pll >= RTPCS_SDS_PLL_TYPE_END)
		return -EINVAL;

	if ((pll == RTPCS_SDS_PLL_TYPE_RING) && (speed == RTPCS_SDS_PLL_SPD_10000))
		return -EINVAL;

	/*
	 * A SerDes clock can either be taken from the low speed ring PLL or the high speed
	 * LC PLL. As it is unclear if disabling PLLs has any positive or negative effect,
	 * always activate both.
	 */
	ret = rtpcs_sds_write_bits(even_sds, PAGE_ANA_MISC, 0x12, 3, 0, 0xf);
	if (ret < 0)
		return ret;

	/* bit 0 is force-bit, bits [3:1] are speed selector */
	ret = rtpcs_sds_write_bits(even_sds, PAGE_ANA_MISC, 0x12, sbit + 3, sbit,
				   (speed << 1) | BIT(0));
	if (ret < 0)
		return ret;

	if (sds->ops->reset_cmu)
		ret = sds->ops->reset_cmu(sds, pll);

	return ret;
}

static int rtpcs_93xx_sds_config_cmu(struct rtpcs_serdes *sds, enum rtpcs_sds_mode hw_mode)
{
	struct rtpcs_serdes *nb_sds = rtpcs_sds_get_neighbor(sds);
	enum rtpcs_sds_pll_speed speed, neighbor_speed;
	enum rtpcs_sds_pll_type pll, neighbor_pll;
	bool speed_changed = true;
	int ret;

	/*
	 * A SerDes pair on RTL93xx is driven by a shared CMU with two PLLs:
	 *
	 * - a low speed ring PLL which can generate signals of 1.25G and 3.125G for link
	 *   speeds of 1G/2.5G
	 * - a high speed LC PLL which can additionally generate a 10.3125G signal for
	 *   10G link speeds
	 *
	 * To drive the pair at different speeds, each SerDes must use its own PLL and we
	 * must wisely assign the PLLs to the SerDes based on their needs. The logic boils
	 * down to the following rules:
	 *
	 * - use ring PLL for slow 1G speeds
	 * - use LC PLL for fast 10G speeds
	 * - for 2.5G prefer ring over LC PLL
	 *
	 * For the case that we want to configure 10G speed but the LC PLL is already used
	 * by the neighbor SerDes and running with a slower speed, there's no way to avoid
	 * reconfiguration. The neighbor SerDes is reconfigured online to the ring PLL.
	 */

	if (hw_mode == RTPCS_SDS_MODE_OFF)
		return 0;

	ret = rtpcs_sds_select_pll_speed(hw_mode, &speed);
	if (ret < 0)
		return ret;

	if (nb_sds->hw_mode == RTPCS_SDS_MODE_OFF) {
		pll = (speed == RTPCS_SDS_PLL_SPD_10000) ? RTPCS_SDS_PLL_TYPE_LC
							 : RTPCS_SDS_PLL_TYPE_RING;
		goto pll_setup;
	}

	ret = nb_sds->ops->get_pll_select(nb_sds, &neighbor_pll);
	if (ret < 0)
		return ret;

	ret = rtpcs_93xx_sds_get_pll_config(nb_sds, neighbor_pll, &neighbor_speed);
	if (ret < 0)
		return ret;

	if (speed == neighbor_speed) {
		speed_changed = false;
		pll = neighbor_pll;
	} else if (neighbor_pll == RTPCS_SDS_PLL_TYPE_RING)
		pll = RTPCS_SDS_PLL_TYPE_LC;
	else if (speed == RTPCS_SDS_PLL_SPD_10000) {
		pr_info("%s: SDS %d needs LC PLL, reconfigure SDS %d to use ring PLL\n",
			__func__, sds->id, nb_sds->id);

		ret = nb_sds->ops->reconfigure_to_pll(nb_sds, RTPCS_SDS_PLL_TYPE_RING);
		if (ret < 0)
			return ret;

		pll = RTPCS_SDS_PLL_TYPE_LC;
	} else
		pll = RTPCS_SDS_PLL_TYPE_RING;

pll_setup:
	if (speed_changed) {
		ret = rtpcs_93xx_sds_set_pll_config(sds, pll, speed);
		if (ret < 0)
			return ret;
	}

	ret = sds->ops->set_pll_select(sds, hw_mode, pll);
	if (ret < 0)
		return ret;

	pr_info("%s: SDS %d using %s PLL for mode %d\n", __func__, sds->id,
		pll == RTPCS_SDS_PLL_TYPE_LC ? "LC" : "ring", hw_mode);
	return ret;
}

static const s16 rtpcs_93xx_sds_usxgmii_submodes[RTPCS_SDS_MODE_MAX] = {
	[0 ... RTPCS_SDS_MODE_MAX - 1]      = -1,
	[RTPCS_SDS_MODE_USXGMII_10GSXGMII]  = RTPCS_93XX_SDS_USXGMII_SUBMODE_10GSX,
	[RTPCS_SDS_MODE_USXGMII_10GDXGMII]  = RTPCS_93XX_SDS_USXGMII_SUBMODE_10GDX,
	[RTPCS_SDS_MODE_USXGMII_10GQXGMII]  = RTPCS_93XX_SDS_USXGMII_SUBMODE_10GQX,
	[RTPCS_SDS_MODE_USXGMII_5GSXGMII]   = RTPCS_93XX_SDS_USXGMII_SUBMODE_5GSX,
	[RTPCS_SDS_MODE_USXGMII_5GDXGMII]   = RTPCS_93XX_SDS_USXGMII_SUBMODE_5GDX,
	[RTPCS_SDS_MODE_USXGMII_2_5GSXGMII] = RTPCS_93XX_SDS_USXGMII_SUBMODE_2_5GSX,
};

static int rtpcs_93xx_sds_apply_usxgmii_submode(struct rtpcs_serdes *sds,
						enum rtpcs_sds_mode hw_mode)
{
	s16 val = rtpcs_93xx_sds_usxgmii_submodes[hw_mode];

	if (val < 0)
		return 0;

	if (!sds->swcore_regs.usxgmii_submode)
		return -EOPNOTSUPP;

	return regmap_field_write(sds->swcore_regs.usxgmii_submode, val);
}

/*
 * RTL93XX wrapper: set MAC mode, then handle variant-specific extras:
 * - post-write delay (required on 930x)
 * - force-mode bit (931x only; nullable field)
 *
 * Each extra no-ops on the variant that doesn't need it — either because
 * the corresponding regmap_field is NULL, or because the mode doesn't match.
 */
static int rtpcs_93xx_sds_set_mac_mode(struct rtpcs_serdes *sds, enum rtpcs_sds_mode hw_mode)
{
	int ret;

	ret = rtpcs_sds_set_mac_mode(sds, hw_mode);
	if (ret)
		return ret;
	msleep(10);

	if (sds->swcore_regs.mac_mode_force) {
		ret = regmap_field_write(sds->swcore_regs.mac_mode_force, 1);
		if (ret)
			return ret;
	}

	return 0;
}

/*
 * Bring up a MAC-driven mode: release the IP mode force-lock so the MAC
 * side takes over (deactivate forces IP=OFF; this undoes that), set the
 * MAC mode, then apply the USXGMII submode if the mode needs one.
 */
static int rtpcs_93xx_sds_set_mac_driven_mode(struct rtpcs_serdes *sds,
					      enum rtpcs_sds_mode hw_mode)
{
	int ret;

	ret = rtpcs_sds_write_bits(sds, PAGE_WDIG, 0x09, 6, 6, 0);
	if (ret)
		return ret;

	ret = rtpcs_93xx_sds_set_mac_mode(sds, hw_mode);
	if (ret)
		return ret;

	return rtpcs_93xx_sds_apply_usxgmii_submode(sds, hw_mode);
}

/*
 * Read/write the SerDes IP mode register: page 0x1f reg 0x09, bits 11:7
 * hold the 5-bit mode value, bit 6 is the "force mode" enable. The same
 * physical field is used on RTL930x and RTL931x.
 */
static int rtpcs_93xx_sds_get_ip_mode(struct rtpcs_serdes *sds)
{
	const s16 *vals = sds->ctrl->cfg->sds_hw_mode_vals;
	int raw;

	raw = rtpcs_sds_read_bits(sds, PAGE_WDIG, 0x09, 11, 7);
	if (raw < 0)
		return raw;

	for (int i = 0; i < RTPCS_SDS_MODE_MAX; i++)
		if (vals[i] == raw)
			return i;

	return -ENOENT;
}

static int rtpcs_93xx_sds_set_ip_mode(struct rtpcs_serdes *sds, enum rtpcs_sds_mode hw_mode)
{
	int raw;

	if (hw_mode >= RTPCS_SDS_MODE_MAX)
		return -EINVAL;

	raw = sds->ctrl->cfg->sds_hw_mode_vals[hw_mode];
	if (raw < 0)
		return -EOPNOTSUPP;

	/* BIT(0) is force mode enable bit */
	return rtpcs_sds_write_bits(sds, PAGE_WDIG, 0x09, 11, 6, raw << 1 | BIT(0));
}

static void rtpcs_93xx_sds_fill_caps(struct rtpcs_serdes *sds)
{
	__set_bit(RTPCS_SDS_MODE_OFF, sds->supported_modes);

	switch (sds->type) {
	case RTPCS_SDS_TYPE_5G:
		__set_bit(RTPCS_SDS_MODE_QSGMII, sds->supported_modes);
		break;
	case RTPCS_SDS_TYPE_10G:
		__set_bit(RTPCS_SDS_MODE_SGMII, sds->supported_modes);
		__set_bit(RTPCS_SDS_MODE_XSGMII, sds->supported_modes);
		__set_bit(RTPCS_SDS_MODE_USXGMII_10GSXGMII, sds->supported_modes);
		__set_bit(RTPCS_SDS_MODE_USXGMII_10GDXGMII, sds->supported_modes);
		__set_bit(RTPCS_SDS_MODE_USXGMII_10GQXGMII, sds->supported_modes);
		__set_bit(RTPCS_SDS_MODE_USXGMII_5GSXGMII, sds->supported_modes);
		__set_bit(RTPCS_SDS_MODE_USXGMII_5GDXGMII, sds->supported_modes);
		__set_bit(RTPCS_SDS_MODE_USXGMII_2_5GSXGMII, sds->supported_modes);

		__set_bit(RTPCS_SDS_MODE_1000BASEX, sds->supported_modes);
		__set_bit(RTPCS_SDS_MODE_2500BASEX, sds->supported_modes);
		__set_bit(RTPCS_SDS_MODE_10GBASER, sds->supported_modes);
		break;
	case RTPCS_SDS_TYPE_UNKNOWN:
	default:
		break;
	}
}

/* RTL930X */

/* This mapping is not coherent so it cannot be expressed arithmetically */
static const struct reg_field rtpcs_930x_mac_mode_fields[RTPCS_930X_SERDES_CNT] = {
	[0]  = REG_FIELD(RTPCS_930X_SDS_MODE_SEL_0,  0,  4),
	[1]  = REG_FIELD(RTPCS_930X_SDS_MODE_SEL_0,  6, 10),
	[2]  = REG_FIELD(RTPCS_930X_SDS_MODE_SEL_0, 12, 16),
	[3]  = REG_FIELD(RTPCS_930X_SDS_MODE_SEL_0, 18, 22),
	[4]  = REG_FIELD(RTPCS_930X_SDS_MODE_SEL_1,  0,  4),
	[5]  = REG_FIELD(RTPCS_930X_SDS_MODE_SEL_1,  6, 10),
	[6]  = REG_FIELD(RTPCS_930X_SDS_MODE_SEL_1, 12, 16),
	[7]  = REG_FIELD(RTPCS_930X_SDS_MODE_SEL_1, 18, 22),
	[8]  = REG_FIELD(RTPCS_930X_SDS_MODE_SEL_2,  0,  4),
	[9]  = REG_FIELD(RTPCS_930X_SDS_MODE_SEL_2,  6, 10),
	[10] = REG_FIELD(RTPCS_930X_SDS_MODE_SEL_3,  0,  4),
	[11] = REG_FIELD(RTPCS_930X_SDS_MODE_SEL_3,  6, 10),
};

static const struct reg_field rtpcs_930x_usxgmii_submode_fields[] = {
	[0] = REG_FIELD(RTPCS_930X_SDS_SUBMODE_CTRL_0,  0,  4),
	[1] = REG_FIELD(RTPCS_930X_SDS_SUBMODE_CTRL_0,  5,  9),
	[2] = REG_FIELD(RTPCS_930X_SDS_SUBMODE_CTRL_1,  0,  4),
	[3] = REG_FIELD(RTPCS_930X_SDS_SUBMODE_CTRL_1,  5,  9),
	[4] = REG_FIELD(RTPCS_930X_SDS_SUBMODE_CTRL_1, 10, 14),
	[5] = REG_FIELD(RTPCS_930X_SDS_SUBMODE_CTRL_1, 15, 19),
	[6] = REG_FIELD(RTPCS_930X_SDS_SUBMODE_CTRL_1, 20, 24),
	[7] = REG_FIELD(RTPCS_930X_SDS_SUBMODE_CTRL_1, 25, 29),
};

/*
 * RTL930X needs a special mapping from logic SerDes ID to physical SerDes ID,
 * which takes the page into account. This applies to most of read/write calls.
 */
static int rtpcs_930x_sds_get_phys_sds_id(int sds_id, int page)
{
        if (sds_id == 3 && page < 4)
                return 10;

        return sds_id;
}

static int rtpcs_930x_sds_op_read(struct rtpcs_serdes *sds, enum rtpcs_page page, int regnum,
				  int bithigh, int bitlow)
{
	int sds_id = rtpcs_930x_sds_get_phys_sds_id(sds->id, page);

	return __rtpcs_sds_read_raw(sds->ctrl, sds_id, page, regnum, bithigh, bitlow);
}

static int rtpcs_930x_sds_op_write(struct rtpcs_serdes *sds, enum rtpcs_page page, int regnum,
				   int bithigh, int bitlow, u16 value)
{
	int sds_id = rtpcs_930x_sds_get_phys_sds_id(sds->id, page);

	return __rtpcs_sds_write_raw(sds->ctrl, sds_id, page, regnum, bithigh, bitlow, value);
}

/*
 * Realtek uses some nasty logic for digital parts of SerDes 2 and 3.
 *
 * This implements 'dal_longan_sds_xsg_field_write' and a combination of
 * '_rtl9300_serdes_index_to_physical' and '_rtl9300_serdes_reg_write' from the SDK.
 */
static int rtpcs_930x_sds_op_xsg_write(struct rtpcs_serdes *sds, enum rtpcs_page page, int regnum,
				       int bithigh, int bitlow, u16 value)
{
	int phys_sds_id, ret;

	switch (sds->id) {
	case 2:
		phys_sds_id = 2;
		break;
	case 3:
		phys_sds_id = 10;
		break;
	default:
		return -ENOTSUPP;
	}

	if (page >= 4)
		return sds->ops->write(sds, page, regnum, bithigh, bitlow, value);

	ret = __rtpcs_sds_write_raw(sds->ctrl, phys_sds_id, page, regnum, bithigh, bitlow, value);
	if (ret)
		return ret;

	return __rtpcs_sds_write_raw(sds->ctrl, phys_sds_id + 1, page, regnum, bithigh, bitlow,
				     value);
}

static void rtpcs_930x_sds_rx_reset(struct rtpcs_serdes *sds,
				    enum rtpcs_sds_mode hw_mode)
{
	enum rtpcs_page page = PAGE_ANA_10G; /* 10GR and USXGMII */

	if (hw_mode == RTPCS_SDS_MODE_1000BASEX)
		page = PAGE_ANA_1G2;

	rtpcs_sds_write_bits(sds, page, 0x15, 4, 4, 0x1);
	mdelay(5);
	rtpcs_sds_write_bits(sds, page, 0x15, 4, 4, 0x0);
}

static int rtpcs_930x_sds_get_pll_select(struct rtpcs_serdes *sds, enum rtpcs_sds_pll_type *pll)
{
	struct rtpcs_serdes *even_sds = rtpcs_sds_get_even(sds);
	int pbit = (sds == even_sds) ? 4 : 6;
	int pll_sel;

	pll_sel = rtpcs_sds_read_bits(even_sds, PAGE_ANA_MISC, 0x12, pbit + 1, pbit);
	if (pll_sel < 0)
		return pll_sel;

	/* bit 0 is force-bit, bit 1 is PLL selector */
	*pll = (enum rtpcs_sds_pll_type)(pll_sel >> 1);
	return 0;
}

static int rtpcs_930x_sds_set_pll_select(struct rtpcs_serdes *sds, enum rtpcs_sds_mode hw_mode,
					 enum rtpcs_sds_pll_type pll)
{
	struct rtpcs_serdes *even_sds = rtpcs_sds_get_even(sds);
	int pbit = (sds == even_sds) ? 4 : 6;

	/* Selecting the PLL a SerDes uses is done in the even lane register */

	/* bit 0 is force-bit, bit 1 is PLL selector */
	return rtpcs_sds_write_bits(even_sds, PAGE_ANA_MISC, 0x12, pbit + 1, pbit,
				    (pll << 1) | BIT(0));
}

static int rtpcs_930x_sds_reset_cmu(struct rtpcs_serdes *sds, enum rtpcs_sds_pll_type pll)
{
	struct rtpcs_serdes *even_sds = rtpcs_sds_get_even(sds);
	int reset_sequence[4] = { 3, 2, 3, 1 };
	int bit, i, ret;

	/*
	 * After the PLL speed has changed, the CMU must take over the new values. The models
	 * of the Otto platform have different reset sequences. Luckily it always boils down
	 * to flipping two bits in a special sequence.
	 */

	bit = pll == RTPCS_SDS_PLL_TYPE_LC ? 2 : 0;

	for (i = 0; i < ARRAY_SIZE(reset_sequence); i++) {
		ret = rtpcs_sds_write_bits(even_sds, PAGE_ANA_COM, 0x0b, bit + 1, bit,
					   reset_sequence[i]);
		if (ret < 0)
			return ret;
	}
	return 0;
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

		rtpcs_sds_write(even_sds, PAGE_WDIG, 0x02, 53);
		ready = rtpcs_sds_read_bits(even_sds, PAGE_WDIG, 0x14, bit, bit);

		ready_cnt = ready ? ready_cnt + 1 : 0;
		if (ready_cnt >= 3)
			return 0;
	}

	return -EBUSY;
}

static void rtpcs_930x_sds_set_power(struct rtpcs_serdes *sds, bool on)
{
	int power_down = on ? 0x0 : 0x3;
	int rx_enable = on ? 0x3 : 0x1;

	rtpcs_sds_write_bits(sds, PAGE_ANA_MISC, 0x00, 7, 6, power_down);
	rtpcs_sds_write_bits(sds, PAGE_ANA_MISC, 0x00, 5, 4, rx_enable);
}

static int rtpcs_930x_sds_reconfigure_to_pll(struct rtpcs_serdes *sds, enum rtpcs_sds_pll_type pll)
{
	enum rtpcs_sds_pll_speed speed;
	enum rtpcs_sds_pll_type old_pll;
	int hw_mode, ret;

	hw_mode = rtpcs_93xx_sds_get_ip_mode(sds);
	if (hw_mode < 0)
		return hw_mode;

	ret = rtpcs_930x_sds_get_pll_select(sds, &old_pll);
	if (ret < 0)
		return ret;

	ret = rtpcs_93xx_sds_get_pll_config(sds, old_pll, &speed);
	if (ret < 0)
		return ret;

	rtpcs_930x_sds_set_power(sds, false);
	rtpcs_93xx_sds_set_ip_mode(sds, RTPCS_SDS_MODE_OFF);

	ret = rtpcs_93xx_sds_set_pll_config(sds, pll, speed);
	if (ret < 0)
		return ret;

	ret = rtpcs_930x_sds_set_pll_select(sds, sds->hw_mode, pll);
	if (ret < 0)
		return ret;

	rtpcs_93xx_sds_set_ip_mode(sds, hw_mode);
	if (rtpcs_930x_sds_wait_clock_ready(sds))
		pr_err("%s: SDS %d could not sync clock\n", __func__, sds->id);

	rtpcs_930x_sds_set_power(sds, true);
	return 0;
}

static void rtpcs_930x_sds_reset_state_machine(struct rtpcs_serdes *sds)
{
	rtpcs_sds_write_bits(sds, PAGE_TGR_PRO_0, 0x02, 12, 12, 0x01); /* SM_RESET bit */
	usleep_range(10000, 20000);
	rtpcs_sds_write_bits(sds, PAGE_TGR_PRO_0, 0x02, 12, 12, 0x00);
	usleep_range(10000, 20000);
}

static int rtpcs_930x_sds_init_state_machine(struct rtpcs_serdes *sds,
					     enum rtpcs_sds_mode hw_mode)
{
	int loopback, link, cnt = 20, ret = -EBUSY;

	if (hw_mode != RTPCS_SDS_MODE_10GBASER)
		return 0;
	/*
	 * After a SerDes mode change it takes some time until the frontend state machine
	 * works properly for 10G. To verify operation readyness run a connection check via
	 * loopback.
	 */
	loopback = rtpcs_sds_read_bits(sds, PAGE_TGR_PRO_0, 0x01, 2, 2); /* CFG_AFE_LPK bit */
	rtpcs_sds_write_bits(sds, PAGE_TGR_PRO_0, 0x01, 2, 2, 0x01);

	while (cnt-- && ret) {
		rtpcs_930x_sds_reset_state_machine(sds);

		/* 10G link state (latched) */
		link = rtpcs_sds_read_bits(sds, PAGE_TGR_STD_1, 0x00, 12, 12);
		link = rtpcs_sds_read_bits(sds, PAGE_TGR_STD_1, 0x00, 12, 12);
		if (link)
			ret = 0;
	}

	rtpcs_sds_write_bits(sds, PAGE_TGR_PRO_0, 0x01, 2, 2, loopback);
	rtpcs_930x_sds_reset_state_machine(sds);

	return ret;
}

static int rtpcs_930x_sds_apply_ip_mode(struct rtpcs_serdes *sds,
					enum rtpcs_sds_mode hw_mode)
{
	int ret;

	/*
	 * TODO: Usually one would expect that it is enough to modify the SDS_MODE_SEL_*
	 * registers (lets call it MAC setup). It seems as if this complex sequence is only
	 * needed for modes that cannot be set by the SoC itself. Additionally it is unclear
	 * if this sequence should quit early in case of errors.
	 */

	ret = rtpcs_93xx_sds_set_ip_mode(sds, RTPCS_SDS_MODE_OFF);
	if (ret < 0)
		return ret;

	if (hw_mode == RTPCS_SDS_MODE_OFF)
		return 0;

	ret = rtpcs_93xx_sds_config_cmu(sds, hw_mode);
	if (ret < 0)
		pr_err("%s: SDS %d could not configure PLL for mode %d: %d\n", __func__,
		       sds->id, hw_mode, ret);

	ret = rtpcs_93xx_sds_set_ip_mode(sds, hw_mode);
	if (ret < 0)
		return ret;

	if (rtpcs_930x_sds_wait_clock_ready(sds))
		pr_err("%s: SDS %d could not sync clock\n", __func__, sds->id);

	if (rtpcs_930x_sds_init_state_machine(sds, hw_mode))
		pr_err("%s: SDS %d could not reset state machine\n", __func__,
		       sds->id);

	return 0;
}

static int rtpcs_930x_sds_set_mode(struct rtpcs_serdes *sds, enum rtpcs_sds_mode hw_mode)
{
	/*
	 * Several modes can be configured via MAC setup, just by setting
	 * a register to a specific value and the MAC will configure
	 * "everything" as needed. For some modes, this seems incomplete and
	 * we need to do manual configuration in the SerDes IP core itself.
	 */

	switch (hw_mode) {
	case RTPCS_SDS_MODE_SGMII:
	case RTPCS_SDS_MODE_1000BASEX:
	case RTPCS_SDS_MODE_2500BASEX:
	case RTPCS_SDS_MODE_10GBASER:
		return rtpcs_930x_sds_apply_ip_mode(sds, hw_mode);

	default:
		break;
	}

	return rtpcs_93xx_sds_set_mac_driven_mode(sds, hw_mode);
}

static int rtpcs_930x_sds_deactivate(struct rtpcs_serdes *sds)
{
	int ret;

	/* Power down the SerDes core analog block. */
	rtpcs_930x_sds_set_power(sds, false);

	/* Force MAC and IP mode registers to OFF, leaving the SerDes inert. */
	ret = rtpcs_93xx_sds_set_mac_mode(sds, RTPCS_SDS_MODE_OFF);
	if (ret)
		return ret;

	ret = rtpcs_93xx_sds_set_ip_mode(sds, RTPCS_SDS_MODE_OFF);
	if (ret)
		return ret;

	/* Disable fiber RX. */
	ret = rtpcs_sds_write_bits(sds, PAGE_ANA_MISC, 2, 12, 12, 1);
	if (ret)
		return ret;

	/* Power down the 1G PHY block. */
	ret = rtpcs_sds_write_bits(sds, PAGE_FIB, MII_BMCR, 11, 11, 1); /* BMCR_PDOWN */
	if (ret)
		return ret;

	/* Power down the 10G PHY block. */
	return rtpcs_sds_write_bits(sds, PAGE_TGR_STD_0, MII_BMCR, 11, 11, 1); /* BMCR_PDOWN */
}

static int rtpcs_930x_sds_activate(struct rtpcs_serdes *sds)
{
	int ret;

	/* Power up the SerDes core analog block and reset its RX path. */
	rtpcs_930x_sds_set_power(sds, true);
	rtpcs_930x_sds_rx_reset(sds, sds->hw_mode);

	/* Enable fiber RX. */
	ret = rtpcs_sds_write_bits(sds, PAGE_ANA_MISC, 2, 12, 12, 0);
	if (ret)
		return ret;

	/* Power up the 1G PHY block. */
	ret = rtpcs_sds_write_bits(sds, PAGE_FIB, MII_BMCR, 11, 11, 0); /* BMCR_PDOWN */
	if (ret)
		return ret;

	/* Power up the 10G PHY block. */
	return rtpcs_sds_write_bits(sds, PAGE_TGR_STD_0, MII_BMCR, 11, 11, 0); /* BMCR_PDOWN */
}

static void rtpcs_930x_sds_tx_config(struct rtpcs_serdes *sds,
				     enum rtpcs_sds_mode hw_mode)
{
	/* parameters: rtl9303_80G_txParam_s2 */
	enum rtpcs_page page;
	int impedance = 0x8;
	int pre_amp = 0x2;
	int main_amp = 0x9;
	int post_amp = 0x2;
	int pre_en = 0x1;
	int post_en = 0x1;

	switch (hw_mode) {
	case RTPCS_SDS_MODE_1000BASEX:
	case RTPCS_SDS_MODE_SGMII:
		pre_amp = 0x1;
		main_amp = 0x9;
		post_amp = 0x1;
		page = PAGE_ANA_1G2_EXT;
		break;
	case RTPCS_SDS_MODE_2500BASEX:
		pre_amp = 0;
		post_amp = 0x8;
		pre_en = 0;
		page = PAGE_ANA_3G1_EXT;
		break;
	case RTPCS_SDS_MODE_10GBASER:
	case RTPCS_SDS_MODE_USXGMII_10GSXGMII:
	case RTPCS_SDS_MODE_USXGMII_10GQXGMII:
	case RTPCS_SDS_MODE_XSGMII:
		pre_en = 0;
		pre_amp = 0;
		main_amp = 0x10;
		post_amp = 0;
		post_en	= 0;
		page = PAGE_ANA_10G_EXT;
		break;
	case RTPCS_SDS_MODE_QSGMII:
		return;
	default:
		pr_err("%s: unsupported SerDes hw mode\n", __func__);
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
					     u32 dcvs_id, bool manual, u32 dcvs_list[])
{
	u8 reg[6] = { 0x1e, 0x1e, 0x1e, 0x1e, 0x01, 0x02 };
	u8 bit[6] = { 14, 13, 12, 11, 15, 11 };

	if (dcvs_id > 5)
		return;

	/* set DCVS auto/manual */
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G, reg[dcvs_id], bit[dcvs_id], bit[dcvs_id],
			     manual ? 0x1 : 0x0);

	if (!manual) {
		/* give auto mode some time */
		mdelay(1);
		return;
	}

	switch (dcvs_id) {
	case 0:
		rtpcs_sds_write_bits(sds, PAGE_ANA_10G_EXT, 0x03,  5,  5, dcvs_list[0]);
		rtpcs_sds_write_bits(sds, PAGE_ANA_10G_EXT, 0x03,  4,  0, dcvs_list[1]);
		break;
	case 1:
		rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x1d, 15, 15, dcvs_list[0]);
		rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x1d, 14, 11, dcvs_list[1]);
		break;
	case 2:
		rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x1d, 10, 10, dcvs_list[0]);
		rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x1d,  9,  6, dcvs_list[1]);
		break;
	case 3:
		rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x1d,  5,  5, dcvs_list[0]);
		rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x1d,  4,  1, dcvs_list[1]);
		break;
	case 4:
		rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x11, 10, 10, dcvs_list[0]);
		rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x11,  9,  6, dcvs_list[1]);
		break;
	case 5:
		rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x11,  4,  4, dcvs_list[0]);
		rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x11,  3,  0, dcvs_list[1]);
		break;
	default:
		break;
	}
}

__always_unused
static void rtpcs_930x_sds_rxcal_dcvs_get(struct rtpcs_serdes *sds,
					  u32 dcvs_id, u32 dcvs_list[])
{
	u8 manual_reg[6] = { 0x1e, 0x1e, 0x1e, 0x1e, 0x01, 0x02 };
	u8 coeff_sel[6] = { 0x22, 0x23, 0x24, 0x25, 0x2c, 0x2d };
	u8 manual_bit[6] = { 14, 13, 12, 11, 15, 11 };
	u32 dcvs_sign_out = 0, dcvs_coef_bin = 0;
	struct rtpcs_serdes *even_sds;
	bool dcvs_manual;

	if (dcvs_id > 5)
		return;

	even_sds = rtpcs_sds_get_even(sds);
	if (sds == even_sds)
		rtpcs_sds_write(sds, PAGE_WDIG, 0x2, 0x2f);
	else
		rtpcs_sds_write(even_sds, PAGE_WDIG, 0x2, 0x31);

	rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x15, 9, 9, 0x1);	/* REG0_RX_EN_TEST */
	rtpcs_sds_write_bits(sds, PAGE_ANA_COM, 0x06, 11, 6, 0x20); /* REG0_RX_DEBUG_SEL */

	rtpcs_sds_write_bits(sds, PAGE_ANA_10G_EXT, 0x0c, 5, 0, coeff_sel[dcvs_id]);
	mdelay(1);

	/* ## DCVSX Read Out */
	dcvs_sign_out = rtpcs_sds_read_bits(sds, PAGE_WDIG, 0x14,  4,  4);
	dcvs_coef_bin = rtpcs_sds_read_bits(sds, PAGE_WDIG, 0x14,  3,  0);
	dcvs_manual = !!rtpcs_sds_read_bits(sds, PAGE_ANA_10G, manual_reg[dcvs_id],
					    manual_bit[dcvs_id], manual_bit[dcvs_id]);

	pr_info("%s: DCVS %u sign = %s, manual = %u, even coefficient = %u\n", __func__,
		dcvs_id, dcvs_sign_out ? "-" : "+", dcvs_manual, dcvs_coef_bin);

	dcvs_list[0] = dcvs_sign_out;
	dcvs_list[1] = dcvs_coef_bin;
}

static void rtpcs_930x_sds_rxcal_leq_manual(struct rtpcs_serdes *sds,
					    bool manual, u32 leq_gray)
{
	if (manual) {
		rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x18, 15, 15, 0x1);
		rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x16, 14, 10, leq_gray);
	} else {
		rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x18, 15, 15, 0x0);
		mdelay(100);
	}
}

static void rtpcs_930x_sds_rxcal_leq_offset_manual(struct rtpcs_serdes *sds,
						   bool manual, u32 offset)
{
	if (manual) {
		rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x17, 6, 2, offset);
	} else {
		rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x17, 6, 2, offset);
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

	rtpcs_sds_write(even_sds, PAGE_WDIG, 0x2, (sds == even_sds) ? 0x2f : 0x31); /* REG_DBGO_SEL */
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x15, 9, 9, 0x1);	/* REG0_RX_EN_TEST */
	rtpcs_sds_write_bits(sds, PAGE_ANA_COM, 0x06, 11, 6, 0x10);	/* REG0_RX_DEBUG_SEL */
	mdelay(1);

	/* ##LEQ Read Out */
	leq_gray = rtpcs_sds_read_bits(sds, PAGE_WDIG, 0x14, 7, 3);
	leq_manual = !!rtpcs_sds_read_bits(sds, PAGE_ANA_10G, 0x18, 15, 15);
	leq_bin = rtpcs_930x_sds_rxcal_gray_to_binary(leq_gray);

	pr_info("LEQ gray: %u, LEQ bin: %u, LEQ manual: %u\n", leq_gray, leq_bin, leq_manual);
	return leq_bin;
}

static void rtpcs_930x_sds_rxcal_vth_manual(struct rtpcs_serdes *sds,
					    bool manual, u32 vth_list[])
{
	/* REG0_LOAD_IN_INIT, [13:13] = VTH */
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x0f, 13, 13, manual ? 0x1 : 0x0);

	if (manual) {
		rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x13,  5,  3, vth_list[0]);
		rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x13,  2,  0, vth_list[1]);
	} else
		mdelay(10);
}

static void rtpcs_930x_sds_rxcal_vth_get(struct rtpcs_serdes *sds,
					 u32 vth_list[])
{
	struct rtpcs_serdes *even_sds = rtpcs_sds_get_even(sds);
	int vth_manual;

	rtpcs_sds_write(even_sds, PAGE_WDIG, 0x2, (sds == even_sds) ? 0x2f : 0x31); /* REG_DBGO_SEL */
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x15, 9, 9, 0x1);		/* REG0_RX_EN_TEST */
	rtpcs_sds_write_bits(sds, PAGE_ANA_COM, 0x06, 11, 6, 0x20);	/* REG0_RX_DEBUG_SEL */
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G_EXT, 0x0c, 5, 0, 0xc);	/* REG0_COEF_SEL */

	mdelay(1);

	/* ##VthP & VthN Read Out */
	vth_list[0] = rtpcs_sds_read_bits(sds, PAGE_WDIG, 0x14, 2, 0); /* v_thp set bin */
	vth_list[1] = rtpcs_sds_read_bits(sds, PAGE_WDIG, 0x14, 5, 3); /* v_thn set bin */
	vth_manual = rtpcs_sds_read_bits(sds, PAGE_ANA_10G, 0x0f, 13, 13);

	pr_info("vthp_set_bin = %d, vthn_set_bin = %d, manual = %d\n", vth_list[0], vth_list[1],
		vth_manual);
}

static void rtpcs_930x_sds_rxcal_tap_manual(struct rtpcs_serdes *sds,
					    int tap_id, bool manual, u32 tap_list[])
{
	if (tap_id > 4)
		return;

	/* ##REG0_LOAD_IN_INIT[0], [11:7] = TAP0-TAP4 */
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0xf, tap_id + 7, tap_id + 7,
			     manual ? 0x1 : 0x0);

	if (!manual) {
		mdelay(10);
		return;
	}

	switch (tap_id) {
	case 0:
		/* ##REG0_TAP0_INIT[5:0]=Tap0_Value */
		rtpcs_sds_write_bits(sds, PAGE_ANA_10G_EXT, 0x03, 5, 5, tap_list[0]);
		rtpcs_sds_write_bits(sds, PAGE_ANA_10G_EXT, 0x03, 4, 0, tap_list[1]);
		break;
	case 1:
		rtpcs_sds_write_bits(sds, PAGE_ANA_COM, 0x07, 6, 6, tap_list[0]);
		rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x09, 11, 6, tap_list[1]);
		rtpcs_sds_write_bits(sds, PAGE_ANA_COM, 0x07, 5, 5, tap_list[2]);
		rtpcs_sds_write_bits(sds, PAGE_ANA_10G_EXT, 0x12, 5, 0, tap_list[3]);
		break;
	case 2:
		rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x09, 5, 5, tap_list[0]);
		rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x09, 4, 0, tap_list[1]);
		rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x0a, 11, 11, tap_list[2]);
		rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x0a, 10, 6, tap_list[3]);
		break;
	case 3:
		rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x0a, 5, 5, tap_list[0]);
		rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x0a, 4, 0, tap_list[1]);
		rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x06, 5, 5, tap_list[2]);
		rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x06, 4, 0, tap_list[3]);
		break;
	case 4:
		rtpcs_sds_write_bits(sds, PAGE_ANA_10G_EXT, 0x01, 5, 5, tap_list[0]);
		rtpcs_sds_write_bits(sds, PAGE_ANA_10G_EXT, 0x01, 4, 0, tap_list[1]);
		rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x06, 11, 11, tap_list[2]);
		rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x06, 10, 6, tap_list[3]);
		break;
	default:
		break;
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

	rtpcs_sds_write(even_sds, PAGE_WDIG, 0x2, (sds == even_sds) ? 0x2f : 0x31); /* REG_DBGO_SEL */
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x15, 9, 9, 0x1);	/* REG0_RX_EN_TEST */
	rtpcs_sds_write_bits(sds, PAGE_ANA_COM, 0x06, 11, 6, 0x20);	/* REG0_RX_DEBUG_SEL */

	if (!tap_id) {
		rtpcs_sds_write_bits(sds, PAGE_ANA_10G_EXT, 0x0c, 5, 0, 0);	/* REG0_COEF_SEL */
		/* ##Tap1 Even Read Out */
		mdelay(1);
		tap0_sign_out = rtpcs_sds_read_bits(sds, PAGE_WDIG, 0x14, 5, 5);
		tap0_coef_bin = rtpcs_sds_read_bits(sds, PAGE_WDIG, 0x14, 4, 0);

		pr_info("tap0: coef_bin = %d, sign = %s\n", tap0_coef_bin,
			tap0_sign_out ? "-" : "+");

		tap_list[0] = tap0_sign_out;
		tap_list[1] = tap0_coef_bin;

		tap_manual = !!rtpcs_sds_read_bits(sds, PAGE_ANA_10G, 0x0f, 7, 7);
		pr_info("tap0: manual = %u\n", tap_manual);
	} else {
		rtpcs_sds_write_bits(sds, PAGE_ANA_10G_EXT, 0x0c, 5, 0, tap_id); /* REG0_COEF_SEL */
		mdelay(1);
		/* ##Tap1 Even Read Out */
		tap_sign_out_even = rtpcs_sds_read_bits(sds, PAGE_WDIG, 0x14, 5, 5);
		tap_coef_bin_even = rtpcs_sds_read_bits(sds, PAGE_WDIG, 0x14, 4, 0);

		rtpcs_sds_write_bits(sds, PAGE_ANA_10G_EXT, 0x0c, 5, 0, (tap_id + 5)); /* REG0_COEF_SEL */
		/* ##Tap1 Odd Read Out */
		tap_sign_out_odd = rtpcs_sds_read_bits(sds, PAGE_WDIG, 0x14, 5, 5);
		tap_coef_bin_odd = rtpcs_sds_read_bits(sds, PAGE_WDIG, 0x14, 4, 0);

		pr_info("tap%u: even coefficient = %u, sign = %s\n", tap_id, tap_coef_bin_even,
			tap_sign_out_even ? "-" : "+");

		pr_info("tap%u: odd coefficient = %u, sign = %s\n", tap_id, tap_coef_bin_odd,
			tap_sign_out_odd ? "-" : "+");

		tap_list[0] = tap_sign_out_even;
		tap_list[1] = tap_coef_bin_even;
		tap_list[2] = tap_sign_out_odd;
		tap_list[3] = tap_coef_bin_odd;

		tap_manual = rtpcs_sds_read_bits(sds, PAGE_ANA_10G, 0x0f, tap_id + 7,
						 tap_id + 7);
		pr_info("tap%u: manual = %d\n", tap_id, tap_manual);
	}
}

static void rtpcs_930x_sds_do_rx_calibration_1(struct rtpcs_serdes *sds,
					       enum rtpcs_sds_mode hw_mode)
{
	/* From both rtl9300_rxCaliConf_serdes_myParam and rtl9300_rxCaliConf_phy_myParam */
	int tap0_init_val = 0x1f; /* Initial Decision Fed Equalizer 0 tap */
	int vth_min = 0x1;

	/* 1.1.1 --- */
	rtpcs_sds_write(sds, PAGE_TGR_PRO_0, 0, 0); /* initial value */

	/* FGCAL */
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x01, 14, 14, 0x00);
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x1c, 10,  5, 0x20);
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G_EXT, 0x02,  0,  0, 0x01);

	/* DCVS */
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x1e, 14, 11, 0x00);
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x01, 15, 15, 0x00);
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x02, 11, 11, 0x00);
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x1c,  4,  0, 0x00);
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x1d, 15, 11, 0x00);
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x1d, 10,  6, 0x00);
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x1d,  5,  1, 0x00);
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x02, 10,  6, 0x00);
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x11,  4,  0, 0x00);
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G_EXT, 0x00,  3,  0, 0x0f);
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x04,  6,  6, 0x01);
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x04,  7,  7, 0x01);

	/* LEQ (Long Term Equivalent signal level) */
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x16, 14,  8, 0x00);

	/* DFE (Decision Fed Equalizer) */
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G_EXT, 0x03,  5,  0, tap0_init_val);
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x09, 11,  6, 0x00);
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x09,  5,  0, 0x00);
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x0a,  5,  0, 0x00);
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G_EXT, 0x01,  5,  0, 0x00);
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G_EXT, 0x12,  5,  0, 0x00);
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x0a, 11,  6, 0x00);
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x06,  5,  0, 0x00);
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G_EXT, 0x01,  5,  0, 0x00);

	/* Vth */
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x13,  5,  3, 0x07);
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x13,  2,  0, 0x07);
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G_EXT, 0x0b,  5,  3, vth_min);

	/* --- 1.1.1 */

	/* 1.1.2 Load DFE initial value --- */
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x0f, 13,  7, 0x7f);

	/* --- 1.1.2 */

	/* 1.1.3 disable LEQ training, enable DFE clock --- */
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x17,  7,  7, 0x00);
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x17,  6,  2, 0x00);
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x0c,  8,  8, 0x00);
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x0b,  4,  4, 0x01);
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x12, 14, 14, 0x00);
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G_EXT, 0x02, 15, 15, 0x00);

	/* --- 1.1.3 */

	/* 1.1.4 offset cali setting --- */
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x0f, 15, 14, 0x03);
	/* --- 1.1.4 */

	/* 1.1.5 LEQ and DFE setting --- */

	/* assume this is equivalent with (PHY_TYPE == SERDES && MEDIA == FIBER_10G) for now */
	if (hw_mode == RTPCS_SDS_MODE_10GBASER) {
		rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x03, 13, 8, 0x1f);
		rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x00, 13, 13, 0x01);
		rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x16, 14, 8, 0x00); /* REG0_FILTER_OUT */
	}

	/* REG0_LEQ_DC_GAIN, 0x01 for short DACs */
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x16,  3,  2, 0x02);
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x0f,  6,  0, 0x5f);
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G_EXT, 0x05,  7,  2, 0x1f);
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x19,  9,  5, 0x1f);
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G_EXT, 0x0b, 15,  9, 0x3c);
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x0b,  1,  0, 0x03);

	/* --- 1.1.5 */
}

static void rtpcs_930x_sds_rxcal_fgcal(struct rtpcs_serdes *sds)
{
	struct rtpcs_serdes *even_sds = rtpcs_sds_get_even(sds);
	u32 fgcal_binary, fgcal_gray;
	u32 offset_range;

	rtpcs_930x_sds_rx_reset(sds, RTPCS_SDS_MODE_10GBASER);

	/* ForegroundOffsetCal_Manual */

	/* Gray config endis to 1 */
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G_EXT, 0x02, 2, 2, 0x01);

	/* ForegroundOffsetCal_Manual(auto mode) */
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x01, 14, 14, 0x00);

	/* Force Rx-Run = 0 */
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x15, 8, 8, 0x0);
	rtpcs_930x_sds_rx_reset(sds, RTPCS_SDS_MODE_10GBASER);

	/* Foreground Calibration --- */

	for (int run = 0; run < 10; run++) {
		/* REG_DBGO_SEL */
		rtpcs_sds_write(even_sds, PAGE_WDIG, 0x2, (sds == even_sds) ? 0x2f : 0x31);
		rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x15, 9, 9, 0x1);	/* REG0_RX_EN_TEST */
		rtpcs_sds_write_bits(sds, PAGE_ANA_COM, 0x06, 11, 6, 0x20);	/* REG0_RX_DEBUG_SEL */

		rtpcs_sds_write_bits(sds, PAGE_ANA_10G_EXT, 0x0c, 5, 0, 0xf); /* REG0_COEF_SEL */
		/* ##FGCAL read gray */
		fgcal_gray = rtpcs_sds_read_bits(sds, PAGE_WDIG, 0x14, 5, 0);
		rtpcs_sds_write_bits(sds, PAGE_ANA_10G_EXT, 0x0c, 5, 0, 0xe); /* REG0_COEF_SEL */
		/* ##FGCAL read binary */
		fgcal_binary = rtpcs_sds_read_bits(sds, PAGE_WDIG, 0x14, 5, 0);

		if (fgcal_binary <= 60 && fgcal_binary >= 3)
			break;

		pr_info("%s: fgcal_gray = %d, fgcal_binary = %d\n", __func__, fgcal_gray,
			fgcal_binary);

		offset_range = rtpcs_sds_read_bits(sds, PAGE_ANA_10G, 0x15, 15, 14);
		if (offset_range == 3) {
			pr_info("%s: Foreground Calibration result marginal!", __func__);
			break;
		}

		offset_range++;
		rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x15, 15, 14, offset_range);

		/* Force Rx-Run = 0 */
		rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x15, 8, 8, 0x0);
		rtpcs_930x_sds_rx_reset(sds, RTPCS_SDS_MODE_10GBASER);
	}
}

__always_unused
static void rtpcs_930x_sds_rxcal_leq_adapt_lock(struct rtpcs_serdes *sds)
{
	/*
	 * SDK dacLongCableOffset / eqHoldEnable from rtl9300_rxCaliConf_serdes/phy_myParam.
	 * These distinguish direct SerDes connections (DAC, fiber SFP — no external PHY in
	 * the signal path) from PHY-attached ports (PCB traces to an external PHY). On
	 * PHY-attached ports the PHY handles its own equalization, so the SerDes LEQ is left
	 * in auto-adapt and no correction offset is needed.
	 */
	bool direct_serdes = sds->media == RTPCS_SDS_MEDIA_FIBER ||
			     sds->media == RTPCS_SDS_MEDIA_DAC_SHORT ||
			     sds->media == RTPCS_SDS_MEDIA_DAC_LONG;
	u32 sum10 = 0, avg10;
	int i;

	/* 1.3.1: release LEQ auto-adapt, let it settle from zero */
	if (!direct_serdes)
		rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0xc, 8, 8, 0x0);
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x17, 7, 7, 0x0);
	rtpcs_930x_sds_rxcal_leq_manual(sds, false, 0);

	/* 1.3.2: sample the auto-adapted LEQ value 10 times over ~100ms */
	for (i = 0; i < 10; i++) {
		sum10 += rtpcs_930x_sds_rxcal_leq_read(sds);
		mdelay(10);
	}

	/* rounded average of where auto-adapt settled */
	avg10 = (sum10 / 10) + (((sum10 % 10) >= 5) ? 1 : 0);

	/*
	 * Empirical correction based on media type.
	 * Direct SerDes connections get a base offset of +3; DAC cables add further
	 * correction for their attenuation. PHY-attached (PCB) needs none.
	 */
	switch (sds->media) {
	case RTPCS_SDS_MEDIA_FIBER:
		avg10 += 3;
		break;
	case RTPCS_SDS_MEDIA_DAC_SHORT:
		avg10 += 4;	/* base 3 + 1 for short DAC */
		break;
	case RTPCS_SDS_MEDIA_DAC_LONG:
		avg10 += 6;	/* base 3 + 3 for long DAC */
		break;
	default:
		break;
	}

	pr_info("sum10:%u, avg10:%u", sum10, avg10);

	/* lock LEQ at corrected value for direct SerDes; PHY-attached stays in auto-adapt */
	if (direct_serdes) {
		rtpcs_930x_sds_rxcal_leq_offset_manual(sds, 1, 0);
		rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x17, 7, 7, 0x1);
		rtpcs_930x_sds_rxcal_leq_manual(sds, true, avg10);
	}

	pr_info("SDS %u LEQ = %u", sds->id, rtpcs_930x_sds_rxcal_leq_read(sds));
}

static void rtpcs_930x_sds_rxcal_vth_tap0_adapt_lock(struct rtpcs_serdes *sds)
{
	u32 tap0_list[4] = {0};
	u32 vth_list[2] = {0};

	/* run VTH/TAP auto-adapt */
	rtpcs_930x_sds_rxcal_vth_manual(sds, false, vth_list);
	rtpcs_930x_sds_rxcal_tap_manual(sds, 0, false, tap0_list);
	mdelay(200);

	/* manually set learned VTH */
	rtpcs_930x_sds_rxcal_vth_get(sds, vth_list);
	rtpcs_930x_sds_rxcal_vth_manual(sds, true, vth_list);

	mdelay(100);

	/* manually set learned TAP0 */
	rtpcs_930x_sds_rxcal_tap_get(sds, 0, tap0_list);
	rtpcs_930x_sds_rxcal_tap_manual(sds, 0, true, tap0_list);
}

static void rtpcs_930x_sds_rxcal_dfe_taps_adapt(struct rtpcs_serdes *sds)
{
	u32 tap1_list[4] = {0};
	u32 tap2_list[4] = {0};
	u32 tap3_list[4] = {0};
	u32 tap4_list[4] = {0};

	/* dfeTap1_4Enable true */
	rtpcs_930x_sds_rxcal_tap_manual(sds, 1, false, tap1_list);
	rtpcs_930x_sds_rxcal_tap_manual(sds, 2, false, tap2_list);
	rtpcs_930x_sds_rxcal_tap_manual(sds, 3, false, tap3_list);
	rtpcs_930x_sds_rxcal_tap_manual(sds, 4, false, tap4_list);

	mdelay(30);
}

static void rtpcs_930x_sds_rxcal_dfe_disable(struct rtpcs_serdes *sds)
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
					     enum rtpcs_sds_mode hw_mode)
{
	u32 latch_sts;

	rtpcs_930x_sds_do_rx_calibration_1(sds, hw_mode);
	rtpcs_930x_sds_rxcal_fgcal(sds);
	rtpcs_930x_sds_rxcal_vth_tap0_adapt_lock(sds);

	/* Do this only for 10GR mode */
	if (hw_mode == RTPCS_SDS_MODE_10GBASER) {
		rtpcs_930x_sds_rxcal_dfe_taps_adapt(sds);
		mdelay(20);

		latch_sts = rtpcs_sds_read_bits(sds, PAGE_TGR_STD_0, 1, 2, 2);
		mdelay(1);
		latch_sts = rtpcs_sds_read_bits(sds, PAGE_TGR_STD_0, 1, 2, 2);
		if (latch_sts) {
			rtpcs_930x_sds_rxcal_dfe_disable(sds);
			rtpcs_930x_sds_rxcal_vth_tap0_adapt_lock(sds);
			rtpcs_930x_sds_rxcal_dfe_taps_adapt(sds);
		}
	}
}

static int rtpcs_930x_sds_sym_err_reset(struct rtpcs_serdes *sds,
					enum rtpcs_sds_mode hw_mode)
{
	int channel, channels;

	switch (hw_mode) {
	case RTPCS_SDS_MODE_10GBASER:
	case RTPCS_SDS_MODE_USXGMII_10GSXGMII:
		/* Read twice to clear */
		rtpcs_sds_read(sds, PAGE_TGR_STD_1, 0x1);
		rtpcs_sds_read(sds, PAGE_TGR_STD_1, 0x1);
		return 0;

	case RTPCS_SDS_MODE_XSGMII:
	case RTPCS_SDS_MODE_QSGMII:
		channels = 4;
		break;

	default:
		channels = 1;
	}

	for (channel = 0; channel < channels; channel++) {
		if (hw_mode == RTPCS_SDS_MODE_XSGMII) {
			rtpcs_sds_xsg_write_bits(sds, PAGE_SDS_EXT, 0x18, 2, 0, channel);
			rtpcs_sds_xsg_write_bits(sds, PAGE_SDS_EXT, 0x3, 15, 8, 0x0);
			rtpcs_sds_xsg_write_bits(sds, PAGE_SDS_EXT, 0x2, 15, 0, 0x0);
		} else {
			rtpcs_sds_write_bits(sds, PAGE_SDS_EXT, 0x18, 2, 0, channel);
			rtpcs_sds_write_bits(sds, PAGE_SDS_EXT, 0x3, 15, 8, 0x0);
			rtpcs_sds_write_bits(sds, PAGE_SDS_EXT, 0x2, 15, 0, 0x0);
		}
	}

	if (channels > 1) {
		if (hw_mode == RTPCS_SDS_MODE_XSGMII) {
			rtpcs_sds_xsg_write_bits(sds, PAGE_SDS_EXT, 0x0, 15, 0, 0x0);
			rtpcs_sds_xsg_write_bits(sds, PAGE_SDS_EXT, 0x1, 15, 8, 0x0);
		} else {
			rtpcs_sds_write_bits(sds, PAGE_SDS_EXT, 0x0, 15, 0, 0x0);
			rtpcs_sds_write_bits(sds, PAGE_SDS_EXT, 0x1, 15, 8, 0x0);
		}
	}

	return 0;
}

static u32 rtpcs_930x_sds_sym_err_get(struct rtpcs_serdes *sds,
				      enum rtpcs_sds_mode hw_mode)
{
	u32 v = 0;

	switch (hw_mode) {
	case RTPCS_SDS_MODE_QSGMII:
	case RTPCS_SDS_MODE_XSGMII:
		v = rtpcs_sds_read_bits(sds, PAGE_SDS_EXT, 0x1, 15, 8) << 16; /* ALL_SYMBOLERR_CNT_NEW_23_16 */
		v |= rtpcs_sds_read_bits(sds, PAGE_SDS_EXT, 0x0, 15, 0); /* ALL_SYMBOLERR_CNT_NEW_15_0 */
		break;

	case RTPCS_SDS_MODE_USXGMII_10GQXGMII:
		break;

	case RTPCS_SDS_MODE_1000BASEX:
	case RTPCS_SDS_MODE_SGMII:
	case RTPCS_SDS_MODE_10GBASER:
	case RTPCS_SDS_MODE_USXGMII_10GSXGMII:
		v = rtpcs_sds_read(sds, PAGE_TGR_STD_1, 0x1);
		v &= 0xff;
		break;

	default:
		rtpcs_sds_write_bits(sds, PAGE_SDS_EXT, 24, 2, 0, 0);

		v = rtpcs_sds_read_bits(sds, PAGE_SDS_EXT, 0x3, 15, 8) << 16; /* MUX_SYMBOLERR_CNT_NEW_23_16 */
		v |= rtpcs_sds_read_bits(sds, PAGE_SDS_EXT, 0x2, 15, 0); /* MUX_SYMBOLERR_CNT_NEW_15_0 */
	}

	return v;
}

static int rtpcs_930x_sds_check_calibration(struct rtpcs_serdes *sds,
					    enum rtpcs_sds_mode hw_mode)
{
	u32 errors1, errors2;

	rtpcs_930x_sds_sym_err_reset(sds, hw_mode);
	rtpcs_930x_sds_sym_err_reset(sds, hw_mode);

	/* Count errors during 1ms */
	errors1 = rtpcs_930x_sds_sym_err_get(sds, hw_mode);
	mdelay(1);
	errors2 = rtpcs_930x_sds_sym_err_get(sds, hw_mode);

	switch (hw_mode) {
	case RTPCS_SDS_MODE_XSGMII:
		if ((errors2 - errors1 > 100) ||
		    (errors1 >= 0xffff00) || (errors2 >= 0xffff00)) {
			pr_info("%s XSGMII error rate too high\n", __func__);
			return 1;
		}
		break;
	default:
		if (errors2 > 0) {
			pr_info("%s: symbol error rate too high\n", __func__);
			return 1;
		}
		break;
	}

	return 0;
}

static int rtpcs_930x_sds_10g_idle(struct rtpcs_serdes *sds)
{
	struct rtpcs_serdes *even_sds = rtpcs_sds_get_even(sds);
	ktime_t timeout;
	int bit, busy;

	bit = (sds == even_sds) ? 0 : 1;
	timeout = ktime_add_us(ktime_get(), 10000); /* timeout after 10 msecs */

	do {
		rtpcs_sds_write(even_sds, PAGE_WDIG, 0x2, 53);
		busy = rtpcs_sds_read_bits(even_sds, PAGE_WDIG, 0x14, bit, bit);
		if (busy < 0)
			return busy;

		if (!busy)
			return 0;

		usleep_range(100, 200); /* wait ~100 usecs before retry */
	} while (ktime_before(ktime_get(), timeout));

	pr_warn("%s: WARNING Waiting for RX idle timed out, SDS %d\n",
		__func__, sds->id);
	return -ETIMEDOUT;
}

static int rtpcs_930x_sds_config_polarity(struct rtpcs_serdes *sds, unsigned int tx_pol,
					  unsigned int rx_pol)
{
	u8 rx_val = (rx_pol == PHY_POL_INVERT) ? 1 : 0;
	u8 tx_val = (tx_pol == PHY_POL_INVERT) ? 1 : 0;
	u32 val;
	int ret;

	/* 10GR */
	val = (tx_val << 1) | rx_val;
	ret = rtpcs_sds_write_bits(sds, PAGE_TGR_PRO_0, 0x2, 14, 13, val);
	if (ret)
		return ret;

	/* 1G */
	val = (rx_val << 1) | tx_val;
	return rtpcs_sds_write_bits(sds, PAGE_SDS, 0x0, 9, 8, val);
}

static const struct rtpcs_sds_config rtpcs_930x_sds_cfg_ana_com[] = {
	{PAGE_ANA_COM, 0x03, 0x8206}, {PAGE_ANA_COM, 0x05, 0x40B0},
	{PAGE_ANA_COM, 0x06, 0x0010}, {PAGE_ANA_COM, 0x07, 0xF09F},
	{PAGE_ANA_COM, 0x0A, 0x0003}, {PAGE_ANA_COM, 0x0B, 0x0005},
	{PAGE_ANA_COM, 0x0C, 0x0007}, {PAGE_ANA_COM, 0x0D, 0x6009},
	{PAGE_ANA_COM, 0x0E, 0x0000}, {PAGE_ANA_COM, 0x0F, 0x0008}
};

static const struct rtpcs_sds_config rtpcs_930x_sds_cfg_ana_1g[] = {
	{PAGE_ANA_1G2, 0x00, 0x0668}, {PAGE_ANA_1G2, 0x02, 0xD020},
	{PAGE_ANA_1G2, 0x06, 0xC000}, {PAGE_ANA_1G2, 0x0B, 0x1892},
	{PAGE_ANA_1G2, 0x0F, 0xFFDF}, {PAGE_ANA_1G2, 0x12, 0x03C4},
	{PAGE_ANA_1G2, 0x13, 0x027F}, {PAGE_ANA_1G2, 0x14, 0x1311},
	{PAGE_ANA_1G2, 0x16, 0x00C9}, {PAGE_ANA_1G2, 0x17, 0xA100},
	{PAGE_ANA_1G2, 0x1A, 0x0001}, {PAGE_ANA_1G2, 0x1C, 0x0400},
	{PAGE_ANA_1G2_EXT, 0x00, 0x820F}, {PAGE_ANA_1G2_EXT, 0x01, 0x0300},
	{PAGE_ANA_1G2_EXT, 0x02, 0x1017}, {PAGE_ANA_1G2_EXT, 0x03, 0xFFDF},
	{PAGE_ANA_1G2_EXT, 0x05, 0x7F7C}, {PAGE_ANA_1G2_EXT, 0x07, 0x8100},
	{PAGE_ANA_1G2_EXT, 0x08, 0x0001}, {PAGE_ANA_1G2_EXT, 0x09, 0xFFD4},
	{PAGE_ANA_1G2_EXT, 0x0A, 0x7C2F}, {PAGE_ANA_1G2_EXT, 0x0E, 0x003F},
	{PAGE_ANA_1G2_EXT, 0x0F, 0x0121}, {PAGE_ANA_1G2_EXT, 0x10, 0x0020},
	{PAGE_ANA_1G2_EXT, 0x11, 0x8840}
};

static const struct rtpcs_sds_config rtpcs_930x_sds_cfg_ana_3g[] = {
	{PAGE_ANA_3G1, 0x00, 0x0668}, {PAGE_ANA_3G1, 0x02, 0xD020},
	{PAGE_ANA_3G1, 0x06, 0xC000}, {PAGE_ANA_3G1, 0x0B, 0x1892},
	{PAGE_ANA_3G1, 0x0F, 0xFFDF}, {PAGE_ANA_3G1, 0x12, 0x01C4},
	{PAGE_ANA_3G1, 0x13, 0x027F}, {PAGE_ANA_3G1, 0x14, 0x1311},
	{PAGE_ANA_3G1, 0x16, 0x00C9}, {PAGE_ANA_3G1, 0x17, 0xA100},
	{PAGE_ANA_3G1, 0x1A, 0x0001}, {PAGE_ANA_3G1, 0x1C, 0x0400},
	{PAGE_ANA_3G1_EXT, 0x00, 0x820F}, {PAGE_ANA_3G1_EXT, 0x01, 0x0300},
	{PAGE_ANA_3G1_EXT, 0x02, 0x1017}, {PAGE_ANA_3G1_EXT, 0x03, 0xFFDF},
	{PAGE_ANA_3G1_EXT, 0x05, 0x7F7C}, {PAGE_ANA_3G1_EXT, 0x07, 0x8100},
	{PAGE_ANA_3G1_EXT, 0x08, 0x0001}, {PAGE_ANA_3G1_EXT, 0x09, 0xFFD4},
	{PAGE_ANA_3G1_EXT, 0x0A, 0x7C2F}, {PAGE_ANA_3G1_EXT, 0x0E, 0x003F},
	{PAGE_ANA_3G1_EXT, 0x0F, 0x0121}, {PAGE_ANA_3G1_EXT, 0x10, 0x0020},
	{PAGE_ANA_3G1_EXT, 0x11, 0x8840},
};

static const struct rtpcs_sds_config rtpcs_930x_sds_cfg_ana_10g[] = {
	{PAGE_ANA_10G, 0x00, 0xA668}, {PAGE_ANA_10G, 0x01, 0x2088},
	{PAGE_ANA_10G, 0x02, 0xD020}, {PAGE_ANA_10G, 0x06, 0xC000},
	{PAGE_ANA_10G, 0x0B, 0x1892}, {PAGE_ANA_10G, 0x0F, 0xFFDF},
	{PAGE_ANA_10G, 0x11, 0x8280}, {PAGE_ANA_10G, 0x12, 0x0044},
	{PAGE_ANA_10G, 0x13, 0x027F}, {PAGE_ANA_10G, 0x14, 0x1311},
	{PAGE_ANA_10G, 0x17, 0xA100}, {PAGE_ANA_10G, 0x1A, 0x0001},
	{PAGE_ANA_10G, 0x1C, 0x0400},
	{PAGE_ANA_10G_EXT, 0x00, 0x820F}, {PAGE_ANA_10G_EXT, 0x01, 0x0300},
	{PAGE_ANA_10G_EXT, 0x02, 0x1217}, {PAGE_ANA_10G_EXT, 0x03, 0xFFDF},
	{PAGE_ANA_10G_EXT, 0x05, 0x7F7C}, {PAGE_ANA_10G_EXT, 0x07, 0x80C4},
	{PAGE_ANA_10G_EXT, 0x08, 0x0001}, {PAGE_ANA_10G_EXT, 0x09, 0xFFD4},
	{PAGE_ANA_10G_EXT, 0x0A, 0x7C2F}, {PAGE_ANA_10G_EXT, 0x0E, 0x003F},
	{PAGE_ANA_10G_EXT, 0x0F, 0x0121}, {PAGE_ANA_10G_EXT, 0x10, 0x0020},
	{PAGE_ANA_10G_EXT, 0x11, 0x8840},
};

static const struct rtpcs_sds_config rtpcs_930x_sds_cfg_usxgmii_xsgmii[] = {
	{PAGE_ANA_10G, 0x12, 0x0484},
	{PAGE_ANA_10G_EXT, 0x02, 0x1017}, {PAGE_ANA_10G_EXT, 0x07, 0x8104}
};

static const struct rtpcs_sds_config rtpcs_930x_sds_cfg_5g_qsgmii[] =
{
	{PAGE_ANA_COM, 0x00, 0x3C91}, {PAGE_ANA_COM, 0x02, 0xB602},
	{PAGE_ANA_COM, 0x07, 0xFA66}, {PAGE_ANA_COM, 0x0A, 0xDF40},
	{PAGE_ANA_5G0, 0x02, 0x35A1}, {PAGE_ANA_5G0, 0x03, 0x6960},
};

static const struct rtpcs_sds_config rtpcs_930x_sds_cfg_final_even[] =
{
	{PAGE_ANA_5G0_EXT, 0x13, 0x0050}, {PAGE_ANA_5G0_EXT, 0x18, 0x8E88},
	{PAGE_ANA_5G0_EXT, 0x19, 0x4902}, {PAGE_ANA_5G0_EXT, 0x1D, 0x2501},

	{PAGE_ANA_6G2_EXT, 0x13, 0x0050}, {PAGE_ANA_6G2_EXT, 0x17, 0x4109},
	{PAGE_ANA_6G2_EXT, 0x18, 0x8E88}, {PAGE_ANA_6G2_EXT, 0x19, 0x4902},
	{PAGE_ANA_6G2_EXT, 0x1C, 0x1109}, {PAGE_ANA_6G2_EXT, 0x1D, 0x2641},

	{PAGE_ANA_10G_EXT, 0x13, 0x0050}, {PAGE_ANA_10G_EXT, 0x18, 0x8E88},
	{PAGE_ANA_10G_EXT, 0x19, 0x4902}, {PAGE_ANA_10G_EXT, 0x1D, 0x66E1},
};

static const struct rtpcs_sds_config rtpcs_930x_sds_cfg_final_odd[] =
{
	{PAGE_ANA_5G0_EXT, 0x13, 0x3D87}, {PAGE_ANA_5G0_EXT, 0x14, 0x3108},
	{PAGE_ANA_6G2_EXT, 0x13, 0x3C87}, {PAGE_ANA_6G2_EXT, 0x14, 0x1808}
};

static int rtpcs_930x_sds_config_hw_mode(struct rtpcs_serdes *sds, enum rtpcs_sds_mode hw_mode)
{
	int (*apply_fn)(struct rtpcs_serdes *, const struct rtpcs_sds_config *, size_t);
	bool is_xsgmii = (hw_mode == RTPCS_SDS_MODE_XSGMII);
	bool is_even_sds = (sds == rtpcs_sds_get_even(sds));
	int ret;

	apply_fn = is_xsgmii ? rtpcs_sds_apply_config_xsg : rtpcs_sds_apply_config;

	if (hw_mode == RTPCS_SDS_MODE_QSGMII) {
		if (sds->type != RTPCS_SDS_TYPE_5G)
			return -ENOTSUPP;

		return rtpcs_sds_apply_config(sds, rtpcs_930x_sds_cfg_5g_qsgmii,
					      ARRAY_SIZE(rtpcs_930x_sds_cfg_5g_qsgmii));
	}

	if (hw_mode != RTPCS_SDS_MODE_USXGMII_10GSXGMII &&
	    hw_mode != RTPCS_SDS_MODE_USXGMII_10GQXGMII) {
		if (is_xsgmii)
			rtpcs_sds_xsg_write(sds, PAGE_SDS, 0x0E, 0x3053);
		else {
			rtpcs_sds_write(sds, PAGE_SDS, 0x0E, 0x3053);
			rtpcs_sds_write(sds, PAGE_SDS_EXT, 0x14, 0x0100);
		}
	}

	ret = apply_fn(sds, rtpcs_930x_sds_cfg_ana_com, ARRAY_SIZE(rtpcs_930x_sds_cfg_ana_com));
	if (ret < 0)
		return ret;

	switch (hw_mode) {
	case RTPCS_SDS_MODE_1000BASEX:
	case RTPCS_SDS_MODE_SGMII:
		ret = rtpcs_sds_apply_config(sds, rtpcs_930x_sds_cfg_ana_1g,
					     ARRAY_SIZE(rtpcs_930x_sds_cfg_ana_1g));
		if (ret < 0)
			return ret;

		break;

	case RTPCS_SDS_MODE_10GBASER:
		rtpcs_sds_write(sds, PAGE_TGR_PRO_0, 0x0D, 0x0F00);
		rtpcs_sds_write(sds, PAGE_TGR_PRO_0, 0x00, 0x0000);
		rtpcs_sds_write(sds, PAGE_TGR_PRO_0, 0x01, 0xC800);
		/*
		 * TODO: Do the 1G and 3G sequences need to be applied? The SDK usually
		 * uses a 10GR-1000BX automatic mode covering all speeds. But in Linux,
		 * we switch the mode on demand so might only need to apply one sequence
		 * at a time.
		 */
		ret = rtpcs_sds_apply_config(sds, rtpcs_930x_sds_cfg_ana_1g,
					     ARRAY_SIZE(rtpcs_930x_sds_cfg_ana_1g));
		if (ret < 0)
			return ret;

		ret = rtpcs_sds_apply_config(sds, rtpcs_930x_sds_cfg_ana_3g,
					     ARRAY_SIZE(rtpcs_930x_sds_cfg_ana_3g));
		if (ret < 0)
			return ret;

		ret = rtpcs_sds_apply_config(sds, rtpcs_930x_sds_cfg_ana_10g,
					     ARRAY_SIZE(rtpcs_930x_sds_cfg_ana_10g));
		if (ret < 0)
			return ret;

		rtpcs_sds_write(sds, PAGE_ANA_10G_EXT, 0x14, 0xE008);
		break;

	case RTPCS_SDS_MODE_2500BASEX:
		ret = rtpcs_sds_apply_config(sds, rtpcs_930x_sds_cfg_ana_1g,
					     ARRAY_SIZE(rtpcs_930x_sds_cfg_ana_1g));
		if (ret < 0)
			return ret;

		ret = rtpcs_sds_apply_config(sds, rtpcs_930x_sds_cfg_ana_3g,
					     ARRAY_SIZE(rtpcs_930x_sds_cfg_ana_3g));
		if (ret < 0)
			return ret;

		break;

	case RTPCS_SDS_MODE_XSGMII:
	case RTPCS_SDS_MODE_USXGMII_10GSXGMII:
	case RTPCS_SDS_MODE_USXGMII_10GQXGMII:
		ret = apply_fn(sds, rtpcs_930x_sds_cfg_ana_10g,
			       ARRAY_SIZE(rtpcs_930x_sds_cfg_ana_10g));
		if (ret < 0)
			return ret;

		ret = apply_fn(sds, rtpcs_930x_sds_cfg_usxgmii_xsgmii,
			       ARRAY_SIZE(rtpcs_930x_sds_cfg_usxgmii_xsgmii));
		if (ret < 0)
			return ret;

		if (!is_xsgmii)
			rtpcs_93xx_sds_usxgmii_config(sds, RTPCS_USXGMII_AN_OPC_STD,
						      0xa4, 0, 1, 0x1);
		break;

	default:
		return 0;
	}

	if (is_even_sds)
		ret = apply_fn(sds, rtpcs_930x_sds_cfg_final_even,
			       ARRAY_SIZE(rtpcs_930x_sds_cfg_final_even));
	else
		ret = apply_fn(sds, rtpcs_930x_sds_cfg_final_odd,
			       ARRAY_SIZE(rtpcs_930x_sds_cfg_final_odd));

	if (ret < 0)
		return ret;

	if (hw_mode == RTPCS_SDS_MODE_10GBASER && is_even_sds)
		rtpcs_sds_write(sds, PAGE_ANA_10G_EXT, 0x1D, 0x76E1);

	return 0;
}

static int rtpcs_930x_sds_config_media(struct rtpcs_serdes *sds, enum rtpcs_sds_media media,
				       enum rtpcs_sds_mode hw_mode)
{
	/*
	 * dal_longan_construct_mac_default_10gmedia_fiber: set medium to fiber.
	 * TODO: this is unconditional regardless of hw_mode; needs mode-aware
	 * handling.
	 */
	rtpcs_sds_write_bits(sds, PAGE_WDIG, 11, 1, 1, 1);

	rtpcs_930x_sds_tx_config(sds, hw_mode);
	return 0;
}

static int rtpcs_930x_sds_post_config(struct rtpcs_serdes *sds, enum rtpcs_sds_mode hw_mode)
{
	int calib_tries = 0;

	if (hw_mode == RTPCS_SDS_MODE_QSGMII)
		return 0;

	/* Calibrate SerDes receiver in loopback mode */
	rtpcs_930x_sds_10g_idle(sds);
	do {
		rtpcs_930x_sds_do_rx_calibration(sds, hw_mode);
		calib_tries++;
		mdelay(50);
	} while (rtpcs_930x_sds_check_calibration(sds, hw_mode) && calib_tries < 3);
	if (calib_tries >= 3)
		pr_warn("%s: SerDes RX calibration failed\n", __func__);

	return 0;
}

static int rtpcs_930x_sds_probe(struct rtpcs_serdes *sds)
{
	struct device *dev = sds->ctrl->dev;
	struct regmap *map = sds->ctrl->map;
	u8 id = sds->id;

	if (id < 2)
		sds->type = RTPCS_SDS_TYPE_5G;
	else if (id <= 9)
		sds->type = RTPCS_SDS_TYPE_10G;
	else
		sds->type = RTPCS_SDS_TYPE_UNKNOWN;

	rtpcs_93xx_sds_fill_caps(sds);

	sds->swcore_regs.mac_mode = devm_regmap_field_alloc(dev, map,
							    rtpcs_930x_mac_mode_fields[id]);
	if (IS_ERR(sds->swcore_regs.mac_mode))
		return PTR_ERR(sds->swcore_regs.mac_mode);

	/* submode only for 10G SerDes (id 2-9) */
	if (sds->type == RTPCS_SDS_TYPE_10G) {
		sds->swcore_regs.usxgmii_submode = devm_regmap_field_alloc(dev, map,
							rtpcs_930x_usxgmii_submode_fields[id - 2]);
		if (IS_ERR(sds->swcore_regs.usxgmii_submode))
			return PTR_ERR(sds->swcore_regs.usxgmii_submode);
	}

	return 0;
}

/* RTL931X */

/*
 * The SerDes MDIO driver maps page regions to different background SerDes.
 * 0x00 - 0x3f	analog SDS
 * 0x40 - 0x7f	digital SDS 1
 * 0x80 - 0xbf	digital SDS 2
 *
 * An XSG write operates on digital SDS 1 and digital SDS 2. Map that to the
 * page ranges accordingly.
 */
static int rtpcs_931x_sds_op_xsg_write(struct rtpcs_serdes *sds, enum rtpcs_page page, int regnum,
                                       int bithigh, int bitlow, u16 value)
{
        int ret;

        ret = __rtpcs_sds_write_raw(sds->ctrl, sds->id, DIGI_1(page), regnum, bithigh, bitlow,
				    value);
        if (ret)
                return ret;

        return __rtpcs_sds_write_raw(sds->ctrl, sds->id, DIGI_2(page), regnum, bithigh, bitlow,
				     value);
}

__maybe_unused
static int rtpcs_931x_sds_fiber_get_symerr(struct rtpcs_serdes *sds,
					   enum rtpcs_sds_mode hw_mode)
{
	int symerr, val, val2;

	switch (hw_mode) {
	case RTPCS_SDS_MODE_10GBASER:
		symerr = rtpcs_sds_read_bits(sds, PAGE_TGR_STD_1, 0x1, 7, 0);
		break;
	case RTPCS_SDS_MODE_1000BASEX:
		rtpcs_sds_write_bits(sds, DIGI_1(PAGE_SDS_EXT), 0x18, 2, 0, 0x0);

		val = rtpcs_sds_read_bits(sds, DIGI_1(PAGE_SDS_EXT), 0x3, 15, 8);
		val2 = rtpcs_sds_read_bits(sds, DIGI_1(PAGE_SDS_EXT), 0x2, 15, 0);
		symerr = (val << 16) | val2;
		break;
	default:
		symerr = -EINVAL;
	}

	return symerr;
}

static void rtpcs_931x_sds_clear_symerr(struct rtpcs_serdes *sds,
					enum rtpcs_sds_mode hw_mode)
{
	switch (hw_mode) {
	case RTPCS_SDS_MODE_SGMII:
	case RTPCS_SDS_MODE_XSGMII:
		for (int i = 0; i < 4; ++i) {
			rtpcs_sds_xsg_write_bits(sds, PAGE_SDS_EXT, 0x18, 2, 0, i);
			rtpcs_sds_xsg_write_bits(sds, PAGE_SDS_EXT, 0x3, 15, 8, 0x0);
			rtpcs_sds_xsg_write(sds, PAGE_SDS_EXT, 0x2, 0x0);
		}

		rtpcs_sds_xsg_write(sds, PAGE_SDS_EXT, 0x0, 0x0);
		rtpcs_sds_xsg_write_bits(sds, PAGE_SDS_EXT, 0x1, 15, 8, 0x0);
		break;
	case RTPCS_SDS_MODE_1000BASEX:
		rtpcs_sds_write_bits(sds, DIGI_1(PAGE_SDS_EXT), 0x18, 2, 0, 0x0);
		rtpcs_sds_write_bits(sds, DIGI_1(PAGE_SDS_EXT), 0x3, 15, 8, 0x0);
		rtpcs_sds_write_bits(sds, DIGI_1(PAGE_SDS_EXT), 0x2, 15, 0, 0x0);
		break;
	case RTPCS_SDS_MODE_10GBASER:
		/* to be verified: clear on read? */
		rtpcs_sds_read_bits(sds, PAGE_TGR_STD_1, 0x1, 7, 0);
		break;
	case RTPCS_SDS_MODE_OFF:
	default:
		break;
	}
}

/**
 * rtpcs_931x_sds_reset_leq_dfe() - Reset LEQ + DFE to a baseline.
 *
 * @sds: Reference to SerDes instance
 *
 * Reset both LEQ and DFE in the RX path to baseline configuration. I.e.
 * sets LEQ and DFE to manual mode and sets certain values (mostly 0) for
 * LEQ and DFE coefficients/parameters.
 *
 * LEQ and DFE can run in two modes:
 * * manual: specific values are set and used
 * * auto: both adapt their parameters automatically
 *
 */
static int rtpcs_931x_sds_reset_leq_dfe(struct rtpcs_serdes *sds)
{
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0xd, 6, 0, 0x0);	/* [6:2] LEQ gain */
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0xd, 7, 7, 0x1);	/* LEQ manual 1=true,0=false */

	rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x1c, 5, 0, 0x1e); /* TAP0 */
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x1d, 11, 0, 0x0); /* TAP1 [11:6] ODD | [5:0] EVEN */
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x1f, 11, 0, 0x0); /* TAP2 [11:6] ODD | [5:0] EVEN */
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G_EXT, 0x0, 11, 0, 0x0); /* TAP3 [11:6] ODD | [5:0] EVEN */
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G_EXT, 0x1, 11, 0, 0x0); /* TAP4 [11:6] ODD | [5:0] EVEN */

	rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0xf, 12, 6, 0x7f);	/* set manual mode */
	rtpcs_sds_write(sds, PAGE_ANA_10G_EXT, 0x12, 0xaaa); /* [11:8] VTHN | [7:4] VTHP */

	return 0;
}

static int rtpcs_931x_sds_power(struct rtpcs_serdes *sds, bool power_on)
{
	u32 en_val = power_on ? 0 : BIT(sds->id);

	return regmap_write_bits(sds->ctrl->map,
				 RTPCS_931X_PS_SERDES_OFF_MODE_CTRL_ADDR,
				 BIT(sds->id), en_val);
}

/*
 * Set the SerDes mode in the SerDes IP block's registers, after clearing
 * the symbol error counter and forcing the MAC mode off.
 */
static int rtpcs_931x_sds_apply_ip_mode(struct rtpcs_serdes *sds,
					enum rtpcs_sds_mode hw_mode)
{
	int ret;

	/* clear symbol error count before changing mode */
	rtpcs_931x_sds_clear_symerr(sds, hw_mode);
	ret = rtpcs_93xx_sds_set_mac_mode(sds, RTPCS_SDS_MODE_OFF);
	if (ret)
		return ret;

	return rtpcs_93xx_sds_set_ip_mode(sds, hw_mode);
}

static int rtpcs_931x_sds_set_mode(struct rtpcs_serdes *sds,
				   enum rtpcs_sds_mode hw_mode)
{
	int ret;

	if (hw_mode == RTPCS_SDS_MODE_XSGMII)
		return rtpcs_93xx_sds_set_mac_driven_mode(sds, hw_mode);

	ret = rtpcs_931x_sds_apply_ip_mode(sds, hw_mode);
	if (ret)
		return ret;

	return rtpcs_93xx_sds_apply_usxgmii_submode(sds, hw_mode);
}

static int rtpcs_931x_sds_deactivate(struct rtpcs_serdes *sds)
{
	int ret;

	ret = rtpcs_931x_sds_power(sds, false);
	if (ret)
		return ret;

	return rtpcs_931x_sds_set_mode(sds, RTPCS_SDS_MODE_OFF);
}

static int rtpcs_931x_sds_activate(struct rtpcs_serdes *sds)
{
	return rtpcs_931x_sds_power(sds, true);
}

__maybe_unused
static void rtpcs_931x_sds_reset(struct rtpcs_serdes *sds)
{
	u32 o_mode, f_bit;

	/* TODO: We need to lock this! */

	rtpcs_931x_sds_power(sds, false);

	/* save current */
	regmap_field_read(sds->swcore_regs.mac_mode, &o_mode);
	regmap_field_read(sds->swcore_regs.mac_mode_force, &f_bit);

	/* force off */
	regmap_field_write(sds->swcore_regs.mac_mode, 0x1f);
	regmap_field_write(sds->swcore_regs.mac_mode_force, 1);

	/* restore previous */
	regmap_field_write(sds->swcore_regs.mac_mode, o_mode);
	regmap_field_write(sds->swcore_regs.mac_mode_force, f_bit);

	rtpcs_931x_sds_power(sds, true);
}

static void rtpcs_931x_sds_rx_reset(struct rtpcs_serdes *sds)
{
	if (sds->type != RTPCS_SDS_TYPE_10G)
		return;

	rtpcs_sds_write(sds, PAGE_ANA_10G, 0x12, 0x2740);
	rtpcs_sds_write(sds, PAGE_ANA_10G_EXT, 0x0, 0x0);	/* [11:6] DFE_TAP3_ODD | [5:0] DFE_TAP3_EVEN */
	rtpcs_sds_write(sds, PAGE_ANA_10G_EXT, 0x2, 0x2010);
	rtpcs_sds_write(sds, PAGE_ANA_MISC, 0x0, 0xc10);

	rtpcs_sds_write(sds, PAGE_ANA_10G, 0x12, 0x27c0);
	rtpcs_sds_write(sds, PAGE_ANA_10G_EXT, 0x0, 0xc000); /* [11:6] DFE_TAP3_ODD | [5:0] DFE_TAP3_EVEN */
	rtpcs_sds_write(sds, PAGE_ANA_10G_EXT, 0x2, 0x6010);
	rtpcs_sds_write(sds, PAGE_ANA_MISC, 0x0, 0xc30);

	mdelay(50);
}

static int rtpcs_931x_sds_cmu_page_get(enum rtpcs_sds_mode hw_mode)
{
	switch (hw_mode) {
	case RTPCS_SDS_MODE_SGMII:
	case RTPCS_SDS_MODE_1000BASEX:
		return PAGE_ANA_1G2;
	case RTPCS_SDS_MODE_2500BASEX:
		return PAGE_ANA_3G1;
	case RTPCS_SDS_MODE_QSGMII:
		return PAGE_ANA_5G0;
	//	return PAGE_ANA_6G0;
	case RTPCS_SDS_MODE_XSGMII:
	case RTPCS_SDS_MODE_USXGMII_10GSXGMII:
	case RTPCS_SDS_MODE_USXGMII_10GDXGMII:
	case RTPCS_SDS_MODE_USXGMII_10GQXGMII:
	case RTPCS_SDS_MODE_USXGMII_5GSXGMII:
	case RTPCS_SDS_MODE_USXGMII_5GDXGMII:
	case RTPCS_SDS_MODE_USXGMII_2_5GSXGMII:
	case RTPCS_SDS_MODE_10GBASER:
		return PAGE_ANA_10G;
	default:
		return -ENOTSUPP;
	}
}

static int rtpcs_931x_sds_get_pll_select(struct rtpcs_serdes *sds, enum rtpcs_sds_pll_type *pll)
{
	int cmu_page, pll_sel;

	cmu_page = rtpcs_931x_sds_cmu_page_get(sds->hw_mode);
	if (cmu_page < 0)
		return cmu_page;

	pll_sel = rtpcs_sds_read_bits(sds, cmu_page, 0x7, 15, 15);
	if (pll_sel < 0)
		return pll_sel;

	*pll = (enum rtpcs_sds_pll_type)pll_sel;
	return 0;
}

static int rtpcs_931x_sds_set_pll_select(struct rtpcs_serdes *sds, enum rtpcs_sds_mode hw_mode,
					 enum rtpcs_sds_pll_type pll)
{
	struct rtpcs_serdes *even_sds = rtpcs_sds_get_even(sds);
	int cmu_page, ret, val;
	int frc_lc_mode_bit;

	cmu_page = rtpcs_931x_sds_cmu_page_get(hw_mode);
	if (cmu_page < 0)
		return cmu_page;

	/*
	 * bits [5:4] (even) / [7:6] (odd) are used by RTL930x as selector. The selector
	 * for RTL931x SerDes is in the CMU page of each SerDes, depending on the hardware
	 * mode.
	 *
	 * Here, the SDK calls them 'frc_lc_mode' and 'frc_lc_mode_val'. However, they don't
	 * seem to have any effect and thus their purpose is unknown. So just set them as
	 * the SDK does.
	 */
	val = (pll == RTPCS_SDS_PLL_TYPE_LC) ? 0x3 : 0x1;
	frc_lc_mode_bit = (sds == even_sds) ? 4 : 6;
	ret = rtpcs_sds_write_bits(even_sds, PAGE_ANA_MISC, 0x12, frc_lc_mode_bit + 1,
				   frc_lc_mode_bit, val);
	if (ret < 0)
		return ret;

	return rtpcs_sds_write_bits(sds, cmu_page, 0x7, 15, 15, pll);
}

static int rtpcs_931x_sds_reconfigure_to_pll(struct rtpcs_serdes *sds, enum rtpcs_sds_pll_type pll)
{
	enum rtpcs_sds_pll_type tmp_pll;
	enum rtpcs_sds_pll_speed speed;
	enum rtpcs_sds_mode tmp_mode;
	int ret;

	/* assume we always reconfigure to the other PLL */
	tmp_pll = (pll == RTPCS_SDS_PLL_TYPE_LC) ? RTPCS_SDS_PLL_TYPE_RING : RTPCS_SDS_PLL_TYPE_LC;

	ret = rtpcs_93xx_sds_get_pll_config(sds, tmp_pll, &speed);
	if (ret < 0)
		return ret;

	tmp_mode = sds->hw_mode;

	/* turn off SerDes for reconfiguration */
	ret = rtpcs_931x_sds_power(sds, false);
	if (ret < 0)
		return ret;

	ret = rtpcs_931x_sds_set_mode(sds, RTPCS_SDS_MODE_OFF);
	if (ret < 0)
		return ret;

	/* reconfigure to other PLL */
	ret = rtpcs_93xx_sds_set_pll_config(sds, pll, speed);
	if (ret < 0)
		return ret;

	ret = rtpcs_931x_sds_set_pll_select(sds, sds->hw_mode, pll);
	if (ret < 0)
		return ret;

	/* turn on SerDes again */
	ret = rtpcs_931x_sds_set_mode(sds, tmp_mode);
	if (ret < 0)
		return ret;

	return rtpcs_931x_sds_power(sds, true);
}

__always_unused
static int rtpcs_931x_sds_link_sts_get(struct rtpcs_serdes *sds)
{
	u32 sts, sts1, latch_sts, latch_sts1;

	switch (sds->hw_mode) {
	case RTPCS_SDS_MODE_XSGMII:
		sts = rtpcs_sds_read_bits(sds, DIGI_1(PAGE_SDS_EXT), 29, 8, 0);
		sts1 = rtpcs_sds_read_bits(sds, DIGI_2(PAGE_SDS_EXT), 29, 8, 0);
		latch_sts = rtpcs_sds_read_bits(sds, DIGI_1(PAGE_SDS_EXT), 30, 8, 0);
		latch_sts1 = rtpcs_sds_read_bits(sds, DIGI_2(PAGE_SDS_EXT), 30, 8, 0);
		break;

	case RTPCS_SDS_MODE_SGMII:
	case RTPCS_SDS_MODE_2500BASEX:
		sts = rtpcs_sds_read_bits(sds, DIGI_1(PAGE_SDS_EXT), 29, 8, 0);
		latch_sts = rtpcs_sds_read_bits(sds, DIGI_1(PAGE_SDS_EXT), 30, 8, 0);
		break;

	default:
		sts = rtpcs_sds_read_bits(sds, PAGE_TGR_STD_1, 0, 12, 12);
		latch_sts = rtpcs_sds_read_bits(sds, PAGE_TGR_STD_0, 1, 2, 2);
		latch_sts1 = rtpcs_sds_read_bits(sds, DIGI_1(PAGE_FIB), 1, 2, 2);
		sts1 = rtpcs_sds_read_bits(sds, DIGI_1(PAGE_FIB), 1, 2, 2);
	}

	pr_info("%s: serdes %d sts %d, sts1 %d, latch_sts %d, latch_sts1 %d\n", __func__,
		sds->id, sts, sts1, latch_sts, latch_sts1);

	return sts1;
}

static int rtpcs_931x_sds_config_polarity(struct rtpcs_serdes *sds, unsigned int tx_pol,
					  unsigned int rx_pol)
{
	u8 rx_val = (rx_pol == PHY_POL_INVERT) ? 1 : 0;
	u8 tx_val = (tx_pol == PHY_POL_INVERT) ? 1 : 0;
	u32 val;
	int ret;

	/* 10gr_*_inv */
	val = (tx_val << 1) | rx_val;
	ret = rtpcs_sds_write_bits(sds, PAGE_TGR_PRO_0, 0x2, 14, 13, val);
	if (ret)
		return ret;

	/* xsg_*_inv */
	val = (rx_val << 1) | tx_val;
	return rtpcs_sds_xsg_write_bits(sds, PAGE_SDS, 0x0, 9, 8, val);
}

static const struct rtpcs_sds_tx_config rtpcs_931x_sds_tx_cfg_v1[] = {
	{ .pre_amp = 0x00, .main_amp = 0x10, .post_amp = 0x06 },
	{ .pre_amp = 0x00, .main_amp = 0x10, .post_amp = 0x06 },
	{ .pre_amp = 0x00, .main_amp = 0x10 },
	{ .pre_amp = 0x00, .main_amp = 0x10 },
	{ .pre_amp = 0x00, .main_amp = 0x10 },
	{ .pre_amp = 0x00, .main_amp = 0x10 },
	{ .pre_amp = 0x03, .main_amp = 0x0d },
	{ .pre_amp = 0x03, .main_amp = 0x0d },
	{ .pre_amp = 0x03, .main_amp = 0x0d },
	{ .pre_amp = 0x03, .main_amp = 0x0d },
	{ .pre_amp = 0x03, .main_amp = 0x0f },
	{ .pre_amp = 0x03, .main_amp = 0x0f },
};
static const struct rtpcs_sds_tx_config rtpcs_931x_sds_tx_cfg_v2[] = {
	{ .pre_amp = 0x00, .main_amp = 0x0e, .post_amp = 0x03 },
	{ .pre_amp = 0x00, .main_amp = 0x0e },
	{ .pre_amp = 0x00, .main_amp = 0x10 },
	{ .pre_amp = 0x00, .main_amp = 0x0c },
	{ .pre_amp = 0x00, .main_amp = 0x0b },
	{ .pre_amp = 0x03, .main_amp = 0x09 },
	{ .pre_amp = 0x03, .main_amp = 0x09 },
	{ .pre_amp = 0x03, .main_amp = 0x0b },
	{ .pre_amp = 0x03, .main_amp = 0x0d },
	{ .pre_amp = 0x00, .main_amp = 0x0d },
	{ .pre_amp = 0x03, .main_amp = 0x0e },
	{ .pre_amp = 0x03, .main_amp = 0x0e, .post_amp = 0x02 },
};
static const struct rtpcs_sds_tx_config rtpcs_931x_sds_tx_cfg_sdac = { /* short DACs */
	.pre_amp = 0x00, .main_amp = 0x1a, .post_amp = 0x04
};
static const struct rtpcs_sds_tx_config rtpcs_931x_sds_tx_cfg_ldac = { /* long DACs */
	.pre_amp = 0x00, .main_amp = 0x10, .post_amp = 0x14
};

/**
 * rtpcs_931x_sds_config_tx_amps - Configure SerDes TX amplifiers
 *
 * A SerDes has three amplifiers (pre, main, post) in the TX path that allow to tune the signal,
 * usually based on eye diagrams. This is needed to account for different tx media, i.e. PCB
 * trace, fiber, DAC. Using the amplifier coefficients, one can precondition the signal in such
 * a way so that it arrives "clean" at the partner.
 */
static int rtpcs_931x_sds_config_tx_amps(struct rtpcs_serdes *sds, u8 pre_amp, u8 main_amp,
					 u8 post_amp)
{
	u16 cfg_val, en_val = 0;
	int ret;

	cfg_val = FIELD_PREP(RTPCS_931X_SDS_PRE_AMP_MASK, pre_amp) |
		  FIELD_PREP(RTPCS_931X_SDS_MAIN_AMP_MASK, main_amp) |
		  FIELD_PREP(RTPCS_931X_SDS_POST_AMP_MASK, post_amp);
	ret = rtpcs_sds_write(sds, PAGE_ANA_10G, 0x1, cfg_val);
	if (ret < 0)
		return ret;

	/* enable/disable pre + post amp, main amp has no enable bit so seems always active */
	if (post_amp)
		en_val |= BIT(0);
	if (pre_amp)
		en_val |= BIT(1);

	return rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0x0, 1, 0, en_val);
}

/**
 * rtpcs_931x_sds_config_tx - Configure static TX path parameters
 */
static int rtpcs_931x_sds_config_tx(struct rtpcs_serdes *sds,
				    enum rtpcs_sds_media sds_media)
{
	const struct rtpcs_sds_tx_config *tx_cfg;

	if (sds->type != RTPCS_SDS_TYPE_10G)
		return 0;

	switch (sds_media) {
	case RTPCS_SDS_MEDIA_DAC_SHORT:
		tx_cfg = &rtpcs_931x_sds_tx_cfg_sdac;
		break;

	case RTPCS_SDS_MEDIA_DAC_LONG:
		tx_cfg = &rtpcs_931x_sds_tx_cfg_ldac;
		break;

	default:
		if (sds->ctrl->chip_version == RTPCS_CHIP_V2)
			/* consider 9311 vs. 9313 here too, see SDK */
			tx_cfg = &rtpcs_931x_sds_tx_cfg_v2[sds->id - 2];
		else
			tx_cfg = &rtpcs_931x_sds_tx_cfg_v1[sds->id - 2];

		break;
	}

	return rtpcs_931x_sds_config_tx_amps(sds, tx_cfg->pre_amp, tx_cfg->main_amp,
					     tx_cfg->post_amp);
}

/**
 * rtpcs_931x_sds_config_rx - Configure static RX path parameters
 */
static int rtpcs_931x_sds_config_rx(struct rtpcs_serdes *sds,
				    enum rtpcs_sds_media sds_media)
{
	/* TODO: Put all static RX configuration here */
	return 0;
}

static int rtpcs_931x_sds_config_media(struct rtpcs_serdes *sds, enum rtpcs_sds_media sds_media,
				       enum rtpcs_sds_mode hw_mode)
{
	struct rtpcs_serdes *even_sds = rtpcs_sds_get_even(sds);
	bool is_dac, is_10g;
	int ret;

	/*
	 * SDK identifies this as some kind of gating. It's enabled
	 * here and later deactivated for non-10G and XSGMII.
	 * (from DMS1250 SDK)
	 */
	rtpcs_sds_write_bits(sds, DIGI_1(PAGE_WDIG), 0x1, 0, 0, 0x1);

	/* from _phy_rtl9310_sds_init */
	rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0xe, 13, 11, 0x0);
	if (hw_mode != RTPCS_SDS_MODE_XSGMII)
		rtpcs_931x_sds_reset_leq_dfe(sds);

	/*
	 * SDK says: media none behavior
	 *
	 * - the first three calls are the same as in rx_reset
	 * - the last one slightly differs in the value. Something is taken into power down
	 *   while rx_reset doesn't do this.
	 */
	rtpcs_sds_write(sds, PAGE_ANA_10G, 0x12, 0x2740);
	rtpcs_sds_write(sds, PAGE_ANA_10G_EXT, 0x0, 0x0);	/* [11:6] DFE_TAP3_ODD | [5:0] DFE_TAP3_EVEN */
	rtpcs_sds_write(sds, PAGE_ANA_10G_EXT, 0x2, 0x2010);
	rtpcs_sds_write(sds, PAGE_ANA_MISC, 0x0, 0xcd1); /* from 930x: [7:6] POWER_DOWN OF ?? */

	rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0xf, 5, 0, 0x4);
	rtpcs_sds_write_bits(sds, PAGE_ANA_5G0, 0x12, 7, 6, 0x1);

	if (sds_media == RTPCS_SDS_MEDIA_NONE)
		return 0;

	/* config SerDes TX path (amps, impedance, etc.) */
	ret = rtpcs_931x_sds_config_tx(sds, sds_media);
	if (ret < 0)
		return ret;

	/* config SerDes RX path (LEQ, DFE, etc.) */
	ret = rtpcs_931x_sds_config_rx(sds, sds_media);
	if (ret < 0)
		return ret;

	is_dac = (sds_media == RTPCS_SDS_MEDIA_DAC_SHORT ||
		  sds_media == RTPCS_SDS_MEDIA_DAC_LONG);
	is_10g = (hw_mode == RTPCS_SDS_MODE_10GBASER ||
		  hw_mode == RTPCS_SDS_MODE_XSGMII ||
		  rtpcs_sds_mode_is_usxgmii(hw_mode));

	rtpcs_sds_write_bits(sds, PAGE_ANA_MISC, 0x0, 11, 10, 0x0);
	rtpcs_sds_write_bits(sds, PAGE_ANA_5G0, 0x7, 15, 15, is_dac ? 0x1 : 0x0);
	rtpcs_sds_write_bits(sds, PAGE_ANA_MISC, 0x0, 11, 10, 0x3);

	switch (sds_media) {
	case RTPCS_SDS_MEDIA_DAC_SHORT:
	case RTPCS_SDS_MEDIA_DAC_LONG:
		rtpcs_sds_write(sds, PAGE_ANA_COM, 0x19, 0xf0a5);	/* from XS1930-10 SDK */
		rtpcs_sds_write(even_sds, PAGE_ANA_10G, 0x8, 0x02a0); /* [10:7] impedance */
		break;

	case RTPCS_SDS_MEDIA_FIBER:
		if (is_10g)
			rtpcs_sds_write_bits(sds, PAGE_ANA_10G, 0xf, 5, 0, 0x2); /* from DMS1250 SDK */

		fallthrough;
	default:
		rtpcs_sds_write(sds, PAGE_ANA_COM, 0x19, 0xf0f0); /* from XS1930 SDK */
		rtpcs_sds_write(even_sds, PAGE_ANA_10G, 0x8, 0x0294); /* [10:7] TX impedance */
		break;
	}

	/* CFG_LINKDW_SEL? (same semantics as 930x) */
	rtpcs_sds_write_bits(sds, PAGE_TGR_PRO_0, 0xd, 6, 6, is_dac ? 0x0 : 0x1);

	if (is_10g) {
		rtpcs_sds_write(sds, PAGE_ANA_10G, 0x12, 0x27c0);
		rtpcs_sds_write(sds, PAGE_ANA_10G_EXT, 0x0, 0xc000); /* [11:6] DFE_TAP3_ODD | [5:0] DFE_TAP3_EVEN */
		rtpcs_sds_write(sds, PAGE_ANA_10G_EXT, 0x2, 0x6010);
	}

	/* FIXME: is this redundant with the writes below? */
	rtpcs_sds_write(sds, PAGE_ANA_MISC, 0x0, 0xc30);			/* from 930x: [7:6] POWER_DOWN OF ?? */
	rtpcs_sds_write_bits(sds, PAGE_ANA_MISC, 0x0, 9, 0, 0x30);
	rtpcs_sds_write_bits(sds, PAGE_ANA_5G0, 0x12, 7, 6, 0x3);

	rtpcs_sds_write_bits(sds, PAGE_ANA_MISC, 0x0, 11, 10, 0x1);
	rtpcs_sds_write_bits(sds, PAGE_ANA_MISC, 0x0, 11, 10, 0x3);

	/* clear pending SerDes RX idle interrupt flag */
	regmap_write_bits(sds->ctrl->map, RTPCS_931X_ISR_SERDES_RXIDLE,
			  BIT(sds->id - 2), BIT(sds->id - 2));

	/* Gating as mentioned above, deactivated here for non-10G and XSGMII */
	if (!is_10g || hw_mode == RTPCS_SDS_MODE_XSGMII)
		rtpcs_sds_write_bits(sds, DIGI_1(PAGE_WDIG), 0x1, 0, 0, 0x0);

	return 0;
}

static int rtpcs_931x_sds_config_fiber_1g(struct rtpcs_serdes *sds)
{
	rtpcs_sds_write_bits(sds, DIGI_1(PAGE_FIB_EXT), 0x12, 15, 14, 0x0);

	rtpcs_sds_write_bits(sds, DIGI_1(PAGE_FIB), 0x0, 6, 6, 0x1);
	rtpcs_sds_write_bits(sds, DIGI_1(PAGE_FIB), 0x0, 13, 13, 0x0);

	return 0;
}

static int rtpcs_931x_sds_config_hw_mode(struct rtpcs_serdes *sds,
					 enum rtpcs_sds_mode hw_mode)
{
	switch (hw_mode) {
	case RTPCS_SDS_MODE_OFF:
		break;

	case RTPCS_SDS_MODE_1000BASEX:
		rtpcs_931x_sds_config_fiber_1g(sds);
		break;

	case RTPCS_SDS_MODE_2500BASEX:
		rtpcs_sds_write_bits(sds, DIGI_1(PAGE_SDS_EXT), 0x14, 8, 8, 1);
		break;

	case RTPCS_SDS_MODE_10GBASER: /* 10GR1000BX_AUTO */
		/* configure 10GR fiber mode=1 */
		rtpcs_sds_write_bits(sds, PAGE_WDIG, 0xb, 1, 1, 1);

		rtpcs_931x_sds_config_fiber_1g(sds);

		/* init auto */
		rtpcs_sds_write_bits(sds, PAGE_WDIG, 13, 15, 0, 0x109e);
		rtpcs_sds_write_bits(sds, PAGE_WDIG, 0x6, 14, 10, 0x8);
		rtpcs_sds_write_bits(sds, PAGE_WDIG, 0x7, 10, 4, 0x7f);
		break;

	case RTPCS_SDS_MODE_SGMII:
		rtpcs_sds_write_bits(sds, PAGE_ANA_1G2, 0x9, 15, 15, 0);
		break;

	case RTPCS_SDS_MODE_XSGMII:
		rtpcs_sds_xsg_write_bits(sds, PAGE_SDS, 0xe, 12, 12, 0x1);
		break;

	case RTPCS_SDS_MODE_USXGMII_10GSXGMII:
	case RTPCS_SDS_MODE_USXGMII_10GDXGMII:
	case RTPCS_SDS_MODE_USXGMII_10GQXGMII:
	case RTPCS_SDS_MODE_USXGMII_5GSXGMII:
	case RTPCS_SDS_MODE_USXGMII_5GDXGMII:
	case RTPCS_SDS_MODE_USXGMII_2_5GSXGMII:
		rtpcs_931x_sds_reset_leq_dfe(sds);
		rtpcs_931x_sds_rx_reset(sds);

		rtpcs_93xx_sds_usxgmii_config(sds, RTPCS_USXGMII_AN_OPC_STD, 0xa4, 0, 1, 0x1);
		break;

	case RTPCS_SDS_MODE_QSGMII:
	default:
		return -ENOTSUPP;
	}

	return rtpcs_93xx_sds_config_cmu(sds, hw_mode);
}

/**
 * rtpcs_931x_init_mac_groups - Initialize MAC groups
 *
 * RTL931x organizes MACs into 12 groups (one per SerDes) that must be explicitly
 * enabled before link establishment. Without initialization, link may fail or
 * packets may be corrupted, especially in USXGMII/XSGMII modes.
 *
 * Simply enable all MACs by writing 0xffffffff to all group registers. Unused
 * MACs and reserved bits are harmless, avoiding complex per-SerDes logic.
 *
 * This lives in the PCS driver since groups are tied to SerDes, and the DSA
 * driver has no SerDes awareness.
 */
static int rtpcs_931x_init_mac_groups(struct rtpcs_ctrl *ctrl)
{
	static const u32 mac_group_regs[] = {
		RTPCS_931X_MAC_GROUP0_1_CTRL,
		RTPCS_931X_MAC_GROUP2_3_CTRL,
		RTPCS_931X_MAC_GROUP4_CTRL,
		RTPCS_931X_MAC_GROUP5_CTRL,
		RTPCS_931X_MAC_GROUP6_7_CTRL,
		RTPCS_931X_MAC_GROUP8_11_CTRL,
	};
	int ret;

	for (int i = 0; i < ARRAY_SIZE(mac_group_regs); i++) {
		ret = regmap_write(ctrl->map, mac_group_regs[i], 0xffffffff);
		if (ret)
			return ret;
	}

	return 0;
}

static int rtpcs_931x_sds_probe(struct rtpcs_serdes *sds)
{
	u32 base = RTPCS_931X_SERDES_MODE_CTRL + 4 * (sds->id >> 2);
	u8 lsb = (sds->id & 3) * 8;
	int ret;

	if (sds->id >= 2)
		sds->type = RTPCS_SDS_TYPE_10G;
	else
		sds->type = RTPCS_SDS_TYPE_UNKNOWN;

	rtpcs_93xx_sds_fill_caps(sds);

	/*
	 * Width is 7 bits (lsb..lsb+6) so every MAC mode write also clears
	 * bit 5 (FEC enable) and bit 6 (10G speedup). These are mode-dependent
	 * and not yet programmed here; keeping them cleared matches the
	 * original 8-bit-wide write behaviour.
	 */
	ret = rtpcs_sds_alloc_field(sds, &sds->swcore_regs.mac_mode,
				    base, lsb, lsb + 6);
	if (ret)
		return ret;

	ret = rtpcs_sds_alloc_field(sds, &sds->swcore_regs.mac_mode_force,
				    base, lsb + 7, lsb + 7);
	if (ret)
		return ret;

	/*
	 * USXGMII submode is packed at 5 bits per SerDes for IDs 2..13,
	 * six entries per 32-bit word, non-straddling.
	 */
	if (sds->type == RTPCS_SDS_TYPE_10G) {
		u8 submode_lsb = ((sds->id - 2) % 6) * 5;

		ret = rtpcs_sds_alloc_field(sds, &sds->swcore_regs.usxgmii_submode,
					    RTPCS_931X_SDS_USXGMII_SUBMODE + ((sds->id - 2) / 6) * 4,
					    submode_lsb, submode_lsb + 4);
		if (ret)
			return ret;
	}

	return 0;
}

static int rtpcs_931x_init(struct rtpcs_ctrl *ctrl)
{
	int ret;

	ret = rtpcs_931x_init_mac_groups(ctrl);
	if (ret < 0)
		return ret;

	return rtpcs_93xx_init(ctrl);
}

/* Common functions */

static int rtpcs_sds_config_polarity(struct rtpcs_serdes *sds, phy_interface_t if_mode)
{
	unsigned int rx_pol, tx_pol;
	int ret;

	if (!sds->fwnode)
		return 0;

	ret = phy_get_manual_rx_polarity(sds->fwnode, phy_modes(if_mode), &rx_pol);
	if (ret < 0)
		return ret;

	ret = phy_get_manual_tx_polarity(sds->fwnode, phy_modes(if_mode), &tx_pol);
	if (ret < 0)
		return ret;

	if (!sds->ops->config_polarity) {
		if (tx_pol != PHY_POL_NORMAL || rx_pol != PHY_POL_NORMAL)
			dev_warn(sds->ctrl->dev,
				 "Polarity change requested but not supported\n");
		return 0;
	}

	return sds->ops->config_polarity(sds, tx_pol, rx_pol);
}

static void rtpcs_pcs_get_state(struct phylink_pcs *pcs, unsigned int neg_mode,
				struct phylink_link_state *state)
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
	struct rtpcs_serdes *sds = link->sds;

	mutex_lock(&ctrl->lock);
	sds->ops->restart_autoneg(sds);
	mutex_unlock(&ctrl->lock);
}

static int rtpcs_pcs_config(struct phylink_pcs *pcs, unsigned int neg_mode,
			    phy_interface_t interface, const unsigned long *advertising,
			    bool permit_pause_to_mac)
{
	struct rtpcs_link *link = rtpcs_phylink_pcs_to_link(pcs);
	struct rtpcs_ctrl *ctrl = link->ctrl;
	struct rtpcs_serdes *sds = link->sds;
	enum rtpcs_sds_media sds_media;
	enum rtpcs_sds_mode hw_mode;
	int ret;

	ret = rtpcs_sds_select_hw_mode(sds, interface, &hw_mode);
	if (ret < 0) {
		dev_err(ctrl->dev, "SerDes %u doesn't support %s mode\n", sds->id,
			phy_modes(interface));
		return -EOPNOTSUPP;
	}

	scoped_guard(mutex, &ctrl->lock) {
		if (sds->hw_mode != hw_mode) {
			dev_info(ctrl->dev, "configure SerDes %u for mode %s\n", sds->id,
				 phy_modes(interface));

			ret = rtpcs_sds_config_polarity(sds, interface);
			if (ret < 0) {
				dev_err(ctrl->dev, "failed to configure polarity of SerDes %u\n",
					sds->id);
				return ret;
			}

			ret = sds->ops->deactivate(sds);
			if (ret < 0)
				return ret;

			ret = sds->ops->config_hw_mode(sds, hw_mode);
			if (ret < 0)
				return ret;

			if (sds->ops->config_media) {
				ret = rtpcs_sds_select_media(hw_mode, &sds_media);
				if (ret < 0)
					return ret;

				sds->media = sds_media;

				ret = sds->ops->config_media(sds, sds_media, hw_mode);
				if (ret < 0)
					return ret;
			}

			ret = sds->ops->set_hw_mode(sds, hw_mode);
			if (ret < 0)
				return ret;

			sds->hw_mode = hw_mode;

			ret = sds->ops->activate(sds);
			if (ret < 0)
				return ret;

			if (sds->ops->post_config) {
				ret = sds->ops->post_config(sds, hw_mode);
				if (ret < 0)
					return ret;
			}

			sds->first_start = false;
		} else
			dev_dbg(ctrl->dev, "SerDes %u already in mode %s, no change\n",
				 sds->id, phy_modes(interface));

		ret = sds->ops->set_autoneg(sds, neg_mode, advertising);
	}

	return ret;
}

static struct mii_bus *rtpcs_probe_serdes_bus(struct rtpcs_ctrl *ctrl)
{
	struct device_node *np;
	struct mii_bus *bus;

	np = of_find_compatible_node(NULL, NULL, "realtek,otto-serdes-mdio");
	if (!np) {
		dev_err(ctrl->dev, "SerDes mdio bus not found in DT");
		return ERR_PTR(-ENODEV);
	}

	if (!of_device_is_available(np)) {
		dev_err(ctrl->dev, "SerDes mdio bus not usable");
		of_node_put(np);
		return ERR_PTR(-ENODEV);
	}

	bus = of_mdio_find_bus(np);
	of_node_put(np);
	if (!bus) {
		dev_warn(ctrl->dev, "SerDes mdio bus not (yet) active");
		return ERR_PTR(-EPROBE_DEFER);
	}

	return bus;
}

static void rtpcs_sds_put_fwnode(void *data)
{
	struct rtpcs_serdes *sds = data;

	fwnode_handle_put(sds->fwnode);
}

static void rtpcs_del_provider_action(void *data)
{
	struct rtpcs_serdes *sds = data;

	fwnode_pcs_del_provider(sds->fwnode);

	rtnl_lock();
	for (int i = 0; i < RTPCS_MAX_LINKS_PER_SDS; i++) {
		if (!sds->link[i])
			continue;

		phylink_release_pcs(&sds->link[i]->pcs);
	}
	rtnl_unlock();
}

static struct rtpcs_serdes *rtpcs_find_serdes(struct rtpcs_ctrl *ctrl,
					      struct fwnode_handle *fwnode)
{
	for (int i = 0; i < ctrl->cfg->serdes_count; i++) {
		if (ctrl->serdes[i].fwnode == fwnode)
			return &ctrl->serdes[i];
	}
	return NULL;
}

/*
 * Walk the sibling switch's ethernet-ports subtree to learn which MAC port
 * each (SerDes, link_idx) pair serves. Same "backwards" topology lookup the
 * sibling MDIO driver does for phy-handle: the DT already encodes the
 * mapping via per-port pcs-handle properties, so the driver doesn't need a
 * parallel per-SoC table. pcs_get_state still needs the port number to
 * index MAC-side link status registers; it reads link_port[] populated
 * here.
 */
static int rtpcs_map_links(struct device *dev, struct rtpcs_ctrl *ctrl)
{
	struct fwnode_handle *fw_dev = dev_fwnode(dev);
	struct fwnode_handle *fw_switch, *fw_ports;
	int ret = 0;

	fw_switch = fwnode_get_parent(fw_dev);
	if (!fw_switch)
		return -ENODEV;

	fw_ports = fwnode_get_named_child_node(fw_switch, "ethernet-ports");
	if (!fw_ports) {
		ret = dev_err_probe(dev, -ENODEV, "%pfwP missing ethernet-ports\n",
				    fw_switch);
		goto put_switch;
	}

	fwnode_for_each_child_node_scoped(fw_ports, fw_port) {
		struct fwnode_reference_args args;
		struct rtpcs_serdes *sds;
		int link_idx;
		u32 pn;

		if (fwnode_property_read_u32(fw_port, "reg", &pn))
			continue;

		if (fwnode_property_get_reference_args(fw_port, "pcs-handle", "#pcs-cells",
						       -1, 0, &args))
			continue;

		link_idx = args.args[0];
		sds = rtpcs_find_serdes(ctrl, args.fwnode);
		fwnode_handle_put(args.fwnode);
		if (!sds)
			continue;

		if (link_idx >= RTPCS_MAX_LINKS_PER_SDS) {
			ret = dev_err_probe(dev, -ERANGE,
					    "%pfwP: pcs-handle link %d exceeds max %u\n",
					    fw_port, link_idx, RTPCS_MAX_LINKS_PER_SDS);
			break;
		}

		if (sds->link_port[link_idx] >= 0) {
			ret = dev_err_probe(dev, -EEXIST,
					    "%pfwP: sds%u link %d already assigned to port %d\n",
					    fw_port, sds->id, link_idx,
					    sds->link_port[link_idx]);
			break;
		}

		sds->link_port[link_idx] = pn;
		sds->num_of_links++;
	}

	fwnode_handle_put(fw_ports);
put_switch:
	fwnode_handle_put(fw_switch);
	return ret;
}

static struct phylink_pcs *rtpcs_pcs_get(struct fwnode_reference_args *pcsspec, void *data)
{
	struct rtpcs_serdes *sds = data;
	struct rtpcs_link *link;
	unsigned int link_idx;
	struct device *dev;

	dev = sds->ctrl->dev;
	if (!pcsspec->nargs) {
		dev_err(dev, "invalid number of cells in 'pcs' property\n");
		return ERR_PTR(-EINVAL);
	}

	link_idx = pcsspec->args[0];
	if (link_idx >= RTPCS_MAX_LINKS_PER_SDS)
		return ERR_PTR(-EINVAL);

	if (sds->link_port[link_idx] < 0) {
		dev_err(dev, "sds %u link %d not associated with any port\n",
			sds->id, link_idx);
		return ERR_PTR(-ENODEV);
	}

	if (!sds->link[link_idx]) {
		link = devm_kzalloc(dev, sizeof(*link), GFP_KERNEL);
		if (!link)
			return ERR_PTR(-ENOMEM);

		link->ctrl = sds->ctrl;
		link->port = sds->link_port[link_idx];
		link->sds = sds;
		link->pcs.ops = sds->ctrl->cfg->pcs_ops;

		sds->link[link_idx] = link;
	}

	return &sds->link[link_idx]->pcs;
}

static int rtpcs_probe(struct platform_device *pdev)
{
	struct device_node *np = pdev->dev.of_node;
	struct device *dev = &pdev->dev;
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
		sds->first_start = true;
		sds->id = i;
		sds->ops = ctrl->cfg->sds_ops;
		sds->regs = ctrl->cfg->sds_regs;
		for (int j = 0; j < RTPCS_MAX_LINKS_PER_SDS; j++)
			sds->link_port[j] = -1;

		ret = ctrl->cfg->sds_probe(sds);
		if (ret)
			return ret;
	}

	for_each_child_of_node_scoped(dev->of_node, child) {
		ret = of_property_read_u32(child, "reg", &sds_id);
		if (ret)
			return ret;

		if (sds_id >= ctrl->cfg->serdes_count)
			return -EINVAL;

		sds = &ctrl->serdes[sds_id];
		sds->fwnode = fwnode_handle_get(of_fwnode_handle(child));
		ret = devm_add_action_or_reset(dev, rtpcs_sds_put_fwnode, sds);
		if (ret)
			return ret;
	}

	ret = rtpcs_map_links(dev, ctrl);
	if (ret)
		return ret;

	if (ctrl->cfg->init) {
		ret = ctrl->cfg->init(ctrl);
		if (ret)
			return ret;
	}

	platform_set_drvdata(pdev, ctrl);

	for (i = 0; i < ctrl->cfg->serdes_count; i++) {
		sds = &ctrl->serdes[i];
		if (!sds->fwnode)
			continue;

		ret = fwnode_pcs_add_provider(sds->fwnode, rtpcs_pcs_get, sds);
		if (ret)
			return ret;
		ret = devm_add_action_or_reset(dev, rtpcs_del_provider_action,
					       sds);
		if (ret)
			return ret;
	}

	dev_info(dev, "Realtek PCS driver initialized\n");
	return 0;
}

static const struct phylink_pcs_ops rtpcs_838x_pcs_ops = {
	.pcs_an_restart		= rtpcs_pcs_an_restart,
	.pcs_config		= rtpcs_pcs_config,
	.pcs_get_state		= rtpcs_pcs_get_state,
};

static const struct rtpcs_sds_ops rtpcs_838x_sds_ops = {
	.read			= rtpcs_generic_sds_op_read,
	.write			= rtpcs_generic_sds_op_write,
	.set_autoneg		= rtpcs_generic_sds_set_autoneg,
	.restart_autoneg	= rtpcs_generic_sds_restart_autoneg,
	.deactivate		= rtpcs_838x_sds_deactivate,
	.activate		= rtpcs_838x_sds_activate,
	.config_hw_mode		= rtpcs_838x_sds_config_hw_mode,
	.set_hw_mode		= rtpcs_838x_sds_set_mode,
	.post_config		= rtpcs_838x_sds_post_config,
};

static const struct rtpcs_sds_regs rtpcs_838x_sds_regs = {
	.an_enable	= { .page = PAGE_FIB, .reg = MII_BMCR, .msb = 12, .lsb = 12 },
	.an_restart	= { .page = PAGE_FIB, .reg = MII_BMCR, .msb = 9, .lsb = 9 },
	.an_advertise	= { .page = PAGE_FIB, .reg = MII_ADVERTISE, .msb = 15, .lsb = 0 },
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
	.sds_ops		= &rtpcs_838x_sds_ops,
	.sds_regs		= &rtpcs_838x_sds_regs,
	.sds_hw_mode_vals	= rtpcs_838x_sds_hw_mode_vals,
	.init			= rtpcs_838x_init,
	.sds_probe		= rtpcs_838x_sds_probe,
};

static const struct phylink_pcs_ops rtpcs_839x_pcs_ops = {
	.pcs_an_restart		= rtpcs_pcs_an_restart,
	.pcs_config		= rtpcs_pcs_config,
	.pcs_get_state		= rtpcs_pcs_get_state,
};

static const struct rtpcs_sds_ops rtpcs_839x_sds_ops = {
	.read			= rtpcs_generic_sds_op_read,
	.write			= rtpcs_generic_sds_op_write,
	.set_autoneg		= rtpcs_generic_sds_set_autoneg,
	.restart_autoneg	= rtpcs_generic_sds_restart_autoneg,
	.deactivate		= rtpcs_839x_sds_deactivate,
	.activate		= rtpcs_839x_sds_activate,
	.config_hw_mode		= rtpcs_839x_sds_config_hw_mode,
	.set_hw_mode		= rtpcs_sds_set_mac_mode,
};

static const struct rtpcs_sds_regs rtpcs_839x_sds_regs = {
	.an_enable	= { .page = PAGE_FIB, .reg = MII_BMCR, .msb = 12, .lsb = 12 },
	.an_restart	= { .page = PAGE_FIB, .reg = MII_BMCR, .msb = 9, .lsb = 9 },
	.an_advertise	= { .page = PAGE_FIB, .reg = MII_ADVERTISE, .msb = 15, .lsb = 0 },
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
	.sds_ops		= &rtpcs_839x_sds_ops,
	.sds_regs		= &rtpcs_839x_sds_regs,
	.sds_hw_mode_vals	= rtpcs_839x_sds_hw_mode_vals,
	.init			= rtpcs_839x_init,
	.sds_probe		= rtpcs_839x_sds_probe,
};

static const struct phylink_pcs_ops rtpcs_930x_pcs_ops = {
	.pcs_an_restart		= rtpcs_pcs_an_restart,
	.pcs_config		= rtpcs_pcs_config,
	.pcs_get_state		= rtpcs_pcs_get_state,
};

static const struct rtpcs_sds_ops rtpcs_930x_sds_ops = {
	.read			= rtpcs_930x_sds_op_read,
	.write			= rtpcs_930x_sds_op_write,
	.xsg_write		= rtpcs_930x_sds_op_xsg_write,
	.set_autoneg		= rtpcs_93xx_sds_set_autoneg,
	.restart_autoneg	= rtpcs_generic_sds_restart_autoneg,
	.get_pll_select		= rtpcs_930x_sds_get_pll_select,
	.set_pll_select		= rtpcs_930x_sds_set_pll_select,
	.reset_cmu		= rtpcs_930x_sds_reset_cmu,
	.reconfigure_to_pll	= rtpcs_930x_sds_reconfigure_to_pll,
	.config_polarity	= rtpcs_930x_sds_config_polarity,
	.deactivate		= rtpcs_930x_sds_deactivate,
	.activate		= rtpcs_930x_sds_activate,
	.config_hw_mode		= rtpcs_930x_sds_config_hw_mode,
	.set_hw_mode		= rtpcs_930x_sds_set_mode,
	.config_media		= rtpcs_930x_sds_config_media,
	.post_config		= rtpcs_930x_sds_post_config,
};

static const struct rtpcs_sds_regs rtpcs_930x_sds_regs = {
	.an_enable	= { .page = PAGE_FIB, .reg = MII_BMCR, .msb = 12, .lsb = 12 },
	.an_restart	= { .page = PAGE_FIB, .reg = MII_BMCR, .msb = 9, .lsb = 9 },
	.an_advertise	= { .page = PAGE_FIB, .reg = MII_ADVERTISE, .msb = 15, .lsb = 0 },
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
	.sds_ops		= &rtpcs_930x_sds_ops,
	.sds_regs		= &rtpcs_930x_sds_regs,
	.sds_hw_mode_vals	= rtpcs_93xx_sds_hw_mode_vals,
	.init			= rtpcs_93xx_init,
	.sds_probe		= rtpcs_930x_sds_probe,
};

static const struct phylink_pcs_ops rtpcs_931x_pcs_ops = {
	.pcs_an_restart		= rtpcs_pcs_an_restart,
	.pcs_config		= rtpcs_pcs_config,
	.pcs_get_state		= rtpcs_pcs_get_state,
};

static const struct rtpcs_sds_ops rtpcs_931x_sds_ops = {
	.read			= rtpcs_generic_sds_op_read,
	.write			= rtpcs_generic_sds_op_write,
	.xsg_write		= rtpcs_931x_sds_op_xsg_write,
	.set_autoneg		= rtpcs_93xx_sds_set_autoneg,
	.restart_autoneg	= rtpcs_generic_sds_restart_autoneg,
	.get_pll_select		= rtpcs_931x_sds_get_pll_select,
	.set_pll_select		= rtpcs_931x_sds_set_pll_select,
	.reconfigure_to_pll	= rtpcs_931x_sds_reconfigure_to_pll,
	.config_polarity	= rtpcs_931x_sds_config_polarity,
	.deactivate		= rtpcs_931x_sds_deactivate,
	.activate		= rtpcs_931x_sds_activate,
	.config_hw_mode		= rtpcs_931x_sds_config_hw_mode,
	.set_hw_mode		= rtpcs_931x_sds_set_mode,
	.config_media		= rtpcs_931x_sds_config_media,
};

static const struct rtpcs_sds_regs rtpcs_931x_sds_regs = {
	.an_enable	= { .page = DIGI_1(PAGE_FIB), .reg = MII_BMCR, .msb = 12, .lsb = 12 },
	.an_restart	= { .page = DIGI_1(PAGE_FIB), .reg = MII_BMCR, .msb = 9, .lsb = 9 },
	.an_advertise	= { .page = DIGI_1(PAGE_FIB), .reg = MII_ADVERTISE, .msb = 15, .lsb = 0 },
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
	.sds_ops		= &rtpcs_931x_sds_ops,
	.sds_regs		= &rtpcs_931x_sds_regs,
	.sds_hw_mode_vals	= rtpcs_93xx_sds_hw_mode_vals,
	.init			= rtpcs_931x_init,
	.sds_probe		= rtpcs_931x_sds_probe,
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
MODULE_AUTHOR("Jonas Jelonek <jelonek.jonas@gmail.com>");
MODULE_DESCRIPTION("Realtek Otto SerDes PCS driver");
MODULE_LICENSE("GPL v2");

// SPDX-License-Identifier: GPL-2.0-only
/* Realtek RTL8224 Ethernet MDIO interface driver
 *
 * Copyright (C) 2025 D. Dejean
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
#include "rtl8224-phy.h"

extern struct rtl83xx_soc_info soc_info;

static inline u32 rtl8224_mask(u8 msb, u8 lsb)
{
	int l = msb - lsb + 1;
	return (BIT(l) - 1) << lsb;
}

static int rtl8224_reg_r32(struct phy_device *phydev, u32 reg, u8 msb, u8 lsb)
{
	u32 l_data = 0;
	u32 h_data = 0;
	u32 mask = 0;

	/* Reg [15:0] */
	l_data = phy_package_read_mmd(phydev, 0, MDIO_MMD_VEND1, reg);
	if (l_data < 0)
		return l_data;

	/* Reg [31:16] */
	h_data = phy_package_read_mmd(phydev, 0, MDIO_MMD_VEND1, reg + 1);
	if (h_data < 0)
		return h_data;

	mask = rtl8224_mask(msb, lsb);
	return (((h_data << 16) | l_data) & mask) >> lsb;
}

static int rtl8224_reg_w32(struct phy_device *phydev, u32 reg, u8 msb, u8 lsb,
			   u32 data)
{
	int ret;
	u32 l_data = 0;
	u32 h_data = 0;
	u32 reg_data = 0;
	u32 mask = 0;

	/* Reg [15:0]*/
	l_data = phy_package_read_mmd(phydev, 0, MDIO_MMD_VEND1, reg);
	if (l_data < 0)
		return l_data;

	/* Reg [31:16]*/
	h_data = phy_package_read_mmd(phydev, 0, MDIO_MMD_VEND1, reg + 1);
	if (h_data < 0)
		return ret;

	mask = rtl8224_mask(msb, lsb);
	reg_data = ((h_data << 16) | (l_data));
	reg_data &= ~(mask);
	reg_data |= (((data) << lsb) & mask);

	l_data = (reg_data & 0xffff);
	h_data = ((reg_data >> 16) & 0xffff);

	ret = phy_package_write_mmd(phydev, 0, MDIO_MMD_VEND1, reg, l_data);
	if (ret < 0)
		return ret;

	ret = phy_package_write_mmd(phydev, 0, MDIO_MMD_VEND1, reg + 1, h_data);
	if (ret < 0)
		return ret;

	return 0;
}

static int rtl8224_sds_write_field(struct phy_device *phydev, int page, u32 reg,
				   u8 msb, u8 lsb, u32 data)
{
	u32 val;
	u32 mask;

	mask = rtl8224_mask(msb, lsb);
	val = rtl8224_sds_read(phydev, page, reg);
	if (val < 0) {
		return val;
	}

	val &= ~mask;
	val |= (data << lsb) & mask;

	return rtl8224_sds_write(phydev, page, reg, val);
}

static void rtl8224_sds_set_op_code(struct phy_device *phydev, u32 op_code)
{
	rtl8224_sds_write_field(phydev, PHY_8224_NWAY_OPCODE_PAGE,
				PHY_8224_NWAY_OPCODE_REG,
				PHY_8224_NWAY_OPCODE_HIGH_BIT,
				PHY_8224_NWAY_OPCODE_LOW_BIT, op_code);
}

static void rtl8224_sds_set_am_period(struct phy_device *phydev, u32 period)
{
	rtl8224_sds_write_field(phydev, PHY_8224_AM_PERIOD_PAGE,
				PHY_8224_AM_PERIOD_REG,
				PHY_8224_AM_PERIOD_HIGH_BIT,
				PHY_8224_AM_PERIOD_LOW_BIT, period);
}

static void rtl8224_sds_set_all_am(struct phy_device *phydev, u32 value)
{
	rtl8224_sds_write_field(phydev, PHY_8224_AM_PAGE, PHY_8224_AM_CFG0_REG,
				PHY_8224_AM0_M0_HIGH_BIT,
				PHY_8224_AM0_M0_LOW_BIT, value);
	rtl8224_sds_write_field(phydev, PHY_8224_AM_PAGE, PHY_8224_AM_CFG0_REG,
				PHY_8224_AM0_M1_HIGH_BIT,
				PHY_8224_AM0_M1_LOW_BIT, value);
	rtl8224_sds_write_field(phydev, PHY_8224_AM_PAGE, PHY_8224_AM_CFG1_REG,
				PHY_8224_AM0_M2_HIGH_BIT,
				PHY_8224_AM0_M2_LOW_BIT, value);
	rtl8224_sds_write_field(phydev, PHY_8224_AM_PAGE, PHY_8224_AM_CFG1_REG,
				PHY_8224_AM1_M0_HIGH_BIT,
				PHY_8224_AM1_M0_LOW_BIT, value);
	rtl8224_sds_write_field(phydev, PHY_8224_AM_PAGE, PHY_8224_AM_CFG2_REG,
				PHY_8224_AM1_M1_HIGH_BIT,
				PHY_8224_AM1_M1_LOW_BIT, value);
	rtl8224_sds_write_field(phydev, PHY_8224_AM_PAGE, PHY_8224_AM_CFG2_REG,
				PHY_8224_AM1_M2_HIGH_BIT,
				PHY_8224_AM1_M2_LOW_BIT, value);
	rtl8224_sds_write_field(phydev, PHY_8224_AM_PAGE, PHY_8224_AM_CFG3_REG,
				PHY_8224_AM2_M0_HIGH_BIT,
				PHY_8224_AM2_M0_LOW_BIT, value);
	rtl8224_sds_write_field(phydev, PHY_8224_AM_PAGE, PHY_8224_AM_CFG3_REG,
				PHY_8224_AM2_M1_HIGH_BIT,
				PHY_8224_AM2_M1_LOW_BIT, value);
	rtl8224_sds_write_field(phydev, PHY_8224_AM_PAGE, PHY_8224_AM_CFG4_REG,
				PHY_8224_AM2_M2_HIGH_BIT,
				PHY_8224_AM2_M2_LOW_BIT, value);
	rtl8224_sds_write_field(phydev, PHY_8224_AM_PAGE, PHY_8224_AM_CFG4_REG,
				PHY_8224_AM3_M0_HIGH_BIT,
				PHY_8224_AM3_M0_LOW_BIT, value);
	rtl8224_sds_write_field(phydev, PHY_8224_AM_PAGE, PHY_8224_AM_CFG5_REG,
				PHY_8224_AM3_M1_HIGH_BIT,
				PHY_8224_AM3_M1_LOW_BIT, value);
	rtl8224_sds_write_field(phydev, PHY_8224_AM_PAGE, PHY_8224_AM_CFG5_REG,
				PHY_8224_AM3_M2_HIGH_BIT,
				PHY_8224_AM3_M2_LOW_BIT, value);
}

static void rtl8224_sds_set_extra(struct phy_device *phydev)
{
	rtl8224_sds_write_field(phydev, PHY_8224_AM_PERIOD_PAGE,
				PHY_8224_TGR_PRO_0_REG17,
				PHY_8224_USXG_V2P1P2_RXOFF_HIGH_BIT,
				PHY_8224_USXG_V2P1P2_RXOFF_LOW_BIT, 0x1);
	rtl8224_sds_write_field(phydev, PHY_8224_NWAY_OPCODE_PAGE,
				PHY_8224_TGR_PRO_1_REG19,
				PHY_8224_USXG_RXDV_OFF_HIGH_BIT,
				PHY_8224_USXG_RXDV_OFF_LOW_BIT, 0x1);
}

/**
 * rtl8224_sram_read is a helper to read a value from the chip SRAM at the
 * specified address.
 */
static u32 rtl8224_sram_read(struct phy_device *phydev, int offset, u32 addr)
{
	phy_package_write_mmd(phydev, offset, MDIO_MMD_VEND1, RTL8224_SRAM_ADDR,
			      addr);
	return phy_package_read_mmd(phydev, offset, MDIO_MMD_VEND1,
				    RTL8224_SRAM_DATA);
}

/**
 * rtl8224_sram_write is a helper to write a value at a specific address in the
 * chip SRAM.
 */
void rtl8224_sram_write(struct phy_device *phydev, int offset, u32 addr,
			u32 val)
{
	phy_package_write_mmd(phydev, offset, MDIO_MMD_VEND1, RTL8224_SRAM_ADDR,
			      addr);
	phy_package_write_mmd(phydev, offset, MDIO_MMD_VEND1, RTL8224_SRAM_DATA,
			      val);
}

/**
 * rtl8224_set_mdi_reverse allows to reverse the PHY MDI channel.
 *
 * @phydev: phy_device struct that represents the device.
 * @swap_pin: true if the MDI should be reversed.
 */
static void rtl8224_set_mdi_reverse(struct phy_device *phydev, bool swap_pin)
{
	u8 port_offset = PORT_OFFSET(phydev->mdio.addr);
	u32 value;
	if (swap_pin) {
		value = 1;
	} else {
		value = 0;
	}
	rtl8224_reg_w32(phydev, RTL8224_PHY_MDI_REVERSE, port_offset,
			port_offset, value);
}

/**
 * rtl8224_set_polarity_swap controls the polarity of the MDI pairs allowing
 * to swap one or more pairs to match the wiring of the chip.
 *
 * @phydev: phy_device struct representing the device.
 * @swap: pairs to swap, bits 0 to 4 controls channel A to D.
 */
static void rtl8224_set_polarity_swap(struct phy_device *phydev, u32 swap)
{
	u32 msb, lsb;
	u8 port_offset = PORT_OFFSET(phydev->mdio.addr);

	lsb = port_offset * RTL8224_PHY_TX_POLARITY_SWAP_OFFSET;
	msb = lsb + RTL8224_PHY_TX_POLARITY_SWAP_OFFSET - 1;
	rtl8224_reg_w32(phydev, RTL8224_PHY_TX_POLARITY_SWAP, msb, lsb, swap);
}

/**
 * rtl8224_get_version reads the version of the RTL8224 chip behind phydev.
 *
 * @phydev: The phy_device struct that represents the device from which the
 * version is read.
 */
static int rtl8224_get_version(struct phy_device *phydev)
{
	int vers;
	rtl8224_reg_w32(phydev, 0xc, 19, 16, 0xa);
	vers = rtl8224_reg_r32(phydev, 0xc, 31, 28);
	rtl8224_reg_w32(phydev, 0xc, 19, 16, 0x0);
	return vers;
}

/**
 * rtl8224_interrupt_init initializes the interruption subsytem of the chip to
 * be ready to use.
 */
static void rtl8224_interrupt_init(struct phy_device *phydev)
{
	int base_port;
	int port_offset;

	base_port = PORT_BASE(phydev->mdio.addr);
	port_offset = PORT_OFFSET(phydev->mdio.addr);

	/* Read the GPHY interrupt pending bits to clear it */
	phy_read_mmd(phydev, MDIO_MMD_VEND2, RTL8224_GPHY_INTR_STATUS_REG);

	/* Clear ISR_EXT_GPHY */
	rtl8224_reg_w32(phydev, RTL8224_ISR_EXT_GPHY, port_offset, port_offset,
			1);

	/* The next configuration only applies on base package port. */
	if (phydev->mdio.addr != base_port) {
		return;
	}

	/* Disable IMR_EXT_GPHY */
	rtl8224_reg_w32(phydev, RTL8224_IMR_EXT_GPHY,
			RTL8224_IMR_EXT_GPHY_3_0_MSB,
			RTL8224_IMR_EXT_GPHY_3_0_OFFSET, 0);

	/* Disable Thermal/PTP 1588/MACsec IMR */
	rtl8224_reg_w32(phydev, RTL8224_IMR_EXT_MISC, RTL8224_INTER_REG_MSB,
			RTL8224_INTER_REG_LSB, 0);

	/* Disable WOL IMR */
	rtl8224_reg_w32(phydev, RTL8224_IMR_EXT_WOL, RTL8224_INTER_REG_MSB,
			RTL8224_INTER_REG_LSB, 0);

	/* Disable Rapid Link Fault Detection (RLFD) interrupt pin output */
	rtl8224_reg_w32(phydev, RTL8224_IMR_EXT_RLFD, RTL8224_INTER_REG_MSB,
			RTL8224_INTER_REG_LSB, 0);
	rtl8224_reg_w32(phydev, RTL8224_IMR_INT_RLFD, RTL8224_INTER_REG_MSB,
			RTL8224_INTER_REG_LSB, 0);
	/* Enable RLFD IMR */
	rtl8224_reg_w32(phydev, RTL8224_RLFD_CTRL_REG, RTL8224_RLFD_SEL_OFFSET,
			RTL8224_RLFD_EN_OFFSET, 1);
	rtl8224_reg_w32(phydev, RTL8224_CFG_PHY_POLL_CMD1_REG,
			RTL8224_CMD_RD_EN_3_0_MSB, RTL8224_CMD_RD_EN_3_0_LSB,
			7);
	phy_set_bits_mmd(phydev, MDIO_MMD_VEND2, 0xa442,
			 1 << PHY_8224_RLFD_ENABLE_OFFSET);

	/* Enable Global IMR for interrupt GPIO output, high level trigger */
	rtl8224_reg_w32(phydev, RTL8224_ISR_SW_INT_MODE,
			RTL8224_SWITCH_IE_OFFSET, RTL8224_SWITCH_IE_OFFSET, 1);

	/* Interrupt mode to low level trigger */
	rtl8224_reg_w32(phydev, RTL8224_ISR_SW_INT_MODE,
			RTL8224_SWITCH_INT_MODE_MSB,
			RTL8224_SWITCH_INT_MODE_OFFSET,
			PHY_8224_INT_MODE_LOW_LEVEL);
}

/**
 * rtl8224_reset_chip resets the PHY chip while saving the port patches.
 */
static void rtl8224_reset_chip(struct phy_device *phydev)
{
	u32 regs[RTL8224_PORT_NUM];
	u32 fields[RTL8224_PORT_NUM];

	/* Backup */
	for (int i = 0; i < RTL8224_PORT_NUM; i++) {
		regs[i] = rtl8224_sram_read(phydev, i, RTL8224_TAPBIN_DEF_ADDR);
		fields[i] = (regs[i] & RTL8224_TAPBIN_DEF_BITMASK) >>
			    RTL8224_TAPBIN_DEF_OFFSET;
	}

	/* Chip reset */
	phy_write_mmd(phydev, MDIO_MMD_VEND1, 0x24, 0x0001);
	mdelay(50);

	/* Restore */
	for (int i = 0; i < RTL8224_PORT_NUM; i++) {
		u32 field, val;
		rtl8224_sram_write(phydev, i, RTL8224_TAPBIN_DEF_ADDR, regs[i]);
		field = fields[i];
		val = (field << 12) | (field << 8) | (field << 4) | (field);
		phy_package_write_mmd(phydev, i, MDIO_MMD_VEND1, 0xbce0, val);
	}
}

static int rtl8224_init(struct phy_device *phydev, bool mdi_pin_swap,
			u32 mdi_pair_swap)
{
	int ret;

	/* Reset the port */
	phy_write_mmd(phydev, MDIO_MMD_VEND1, 0x145, 0x0001);
	phy_write_mmd(phydev, MDIO_MMD_VEND1, 0x145, 0x0000);
	mdelay(100);

	/* Apply the patch sequences */
	ret = rtl8224_patch(phydev);
	if (ret < 0)
		return ret;

	/* Disable 100M/1G EEE advertise ability */
	phy_write_mmd(phydev, MDIO_MMD_VEND2, 0xa5d0, 0x0);
	/* Disable 1G Lite ability */
	phy_clear_bits_mmd(phydev, MDIO_MMD_VEND2, RTL8224_PHY_CFG_1G_LITE,
			   RTL8224_PHY_CFG_1G_LITE_BIT);

	switch (soc_info.family) {
	case RTL8380_FAMILY_ID:
	case RTL8390_FAMILY_ID:
		rtl8224_sds_set_op_code(phydev, 0x3);
		rtl8224_sds_set_am_period(phydev, 0xa4);
		break;
	case RTL9300_FAMILY_ID:
	case RTL9310_FAMILY_ID:
		rtl8224_sds_set_op_code(phydev, 0xaa);
		rtl8224_sds_set_am_period(phydev, 0x5078);
		rtl8224_sds_set_all_am(phydev, 0x0);
		rtl8224_sds_set_extra(phydev);
		break;
	}

	/* Swap MDI pin if required on this port */
	rtl8224_set_mdi_reverse(phydev, mdi_pin_swap);

	/* Apply pair swap configuration if required */
	rtl8224_set_polarity_swap(phydev, mdi_pair_swap);

	/* Initialize the interrupts correctly to avoid spurious behaviors */
	rtl8224_interrupt_init(phydev);

	/* Disable thermal sensor output latch */
	rtl8224_reg_w32(phydev, RTL8224_TM0_CTRL2, RTL8224_REG_EN_LATCH_OFFSET,
			RTL8224_REG_EN_LATCH_OFFSET, 0);
	/* Disable MIB counters */
	rtl8224_reg_w32(phydev, RTL8224_PHY_MIB_GLOBAL_CONFIG, 11, 11, 0);
	/* Clear phy counters (0 to 3) */
	rtl8224_reg_w32(phydev, RTL8224_PHY_MIB_GLOBAL_CONFIG, 3, 0, 0xf);
	/* Enable MIB counters */
	rtl8224_reg_w32(phydev, RTL8224_PHY_MIB_GLOBAL_CONFIG, 11, 11, 1);

	return 0;
}

static int rtl8224_phy_probe(struct phy_device *phydev)
{
	struct device *dev = &phydev->mdio.dev;
	int addr = phydev->mdio.addr;
	bool mdi_pin_swap = false;
	u32 mdi_pair_swap = 0;

	/* Obtain PHY parameters from DT */
	if (of_property_read_bool(dev->of_node, "mdi-pin-swap")) {
		mdi_pin_swap = true;
	}
	if (of_property_read_u32(dev->of_node, "mdi-pair-swap",
				 &mdi_pair_swap)) {
		mdi_pair_swap = 0;
	}

	/* All base addresses of the PHYs start at multiples of 4 */
	devm_phy_package_join(dev, phydev, PORT_BASE(addr),
			      sizeof(struct rtl8224_shared_private));

	if (PORT_OFFSET(addr) == 0) {
		struct rtl8224_shared_private *shared = phydev->shared->priv;
		shared->version = rtl8224_get_version(phydev);

		phydev_info(phydev, "Detected RTL8224 revision %d\n",
			    shared->version);

		/* Reset the chip */
		rtl8224_reset_chip(phydev);
	}

	return rtl8224_init(phydev, mdi_pin_swap, mdi_pair_swap);
}

static int rtl8224_config_init(struct phy_device *phydev)
{
	/* Power on the device. */
	return phy_clear_bits_mmd(phydev, MDIO_MMD_PMAPMD, MDIO_CTRL1,
				  BMCR_PDOWN);
}

static int rtl8224_read_status(struct phy_device *phydev)
{
	int ret;
	u32 val, speed;

	val = genphy_c45_read_status(phydev);
	if (val < 0)
		return val;

	/* Read speed */
	val = phy_read_mmd(phydev, MDIO_MMD_VEND2, RTL8224_PHY_SR);
	if (val < 0)
		return val;

	/* Cross over status */
	if (val & RTL8224_PHY_SR_MDIX_BIT)
		phydev->mdix = ETH_TP_MDI;
	else
		phydev->mdix = ETH_TP_MDI_X;

	/* Compute the speed. */
	speed = RTL8224_PHY_SR_SPEED(val);
	switch (speed) {
	case 0:
		phydev->speed = SPEED_10;
		break;
	case 1: /* 100 Mbps */
	case 3: /* 500 Mbps */
		phydev->speed = SPEED_100;
		break;
	case 2: /* 1 Gbps */
	case 7: /* 2.5 Gbps Lite */
		phydev->speed = SPEED_1000;
		break;
	case 5:
		phydev->speed = SPEED_2500;
		break;
	case 4: /* 10G */
	case 6: /* 5G */
	case 8: /* 5G Lite */
	case 9: /* 10G Lite */
	default:
		pr_debug("%s: unsupported speed %d\n", __func__, speed);
		phydev->speed = SPEED_UNKNOWN;
		break;
	}

	/* Link partner advertisement. */
	if (phydev->autoneg == AUTONEG_ENABLE) {
		linkmode_clear_bit(ETHTOOL_LINK_MODE_1000baseT_Half_BIT,
				   phydev->lp_advertising);
		linkmode_clear_bit(ETHTOOL_LINK_MODE_1000baseT_Full_BIT,
				   phydev->lp_advertising);

		ret = genphy_c45_read_lpa(phydev);
		if (ret)
			return ret;

		val = phy_read_mmd(phydev, MDIO_MMD_VEND2,
				   RTL8224_PHY_LP_SPEED_ABILITY);
		if (val < 0)
			return val;

		linkmode_mod_bit(ETHTOOL_LINK_MODE_1000baseT_Half_BIT,
				 phydev->lp_advertising,
				 val & RTL8224_PHY_LP_1000T_HALF_BIT);
		linkmode_mod_bit(ETHTOOL_LINK_MODE_1000baseT_Full_BIT,
				 phydev->lp_advertising,
				 val & RTL8224_PHY_LP_1000T_FULL_BIT);
		phy_resolve_aneg_linkmode(phydev);
	}

	return 0;
}

static int rtl8224_config_aneg(struct phy_device *phydev)
{
	bool changed = false;
	int ret, val;

	if (phydev->autoneg == AUTONEG_DISABLE) {
		/* Disable autonegotiation */
		phy_clear_bits_mmd(phydev, MDIO_MMD_AN, MDIO_CTRL1,
				   MDIO_AN_CTRL1_ENABLE |
					   MDIO_AN_CTRL1_RESTART);
		/* Force device speed */
		return genphy_c45_pma_setup_forced(phydev);
	}

	/* Use C45 autoneg configuration */
	ret = genphy_c45_an_config_aneg(phydev);
	if (ret < 0)
		return ret;
	if (ret > 0)
		changed = true;

	/* 1000baseT has to be handled separately since there's no C45 support. */
	val = linkmode_adv_to_mii_ctrl1000_t(phydev->advertising);

	ret = phy_modify_mmd_changed(phydev, MDIO_MMD_VEND2,
				     RTL8224_PHY_LL_SPEED_ABILITY,
				     RTL8224_PHY_LL_1000T_FULL_BIT, val);
	if (ret < 0)
		return ret;
	if (ret > 0)
		changed = true;

	return genphy_c45_check_and_restart_aneg(phydev, changed);
}

static int rtl8224_get_features(struct phy_device *phydev)
{
	linkmode_set_bit(ETHTOOL_LINK_MODE_TP_BIT, phydev->supported);

	return genphy_c45_pma_read_abilities(phydev);
}

static struct phy_driver rtl8224_phy_driver[] = {
	{
		PHY_ID_MATCH_MODEL(PHY_ID_RTL8224),
		.name = "Realtek RTL8224",
		.suspend = genphy_suspend,
		.resume = genphy_resume,
		.set_loopback = genphy_loopback,
		.probe = rtl8224_phy_probe,
		.config_init = rtl8224_config_init,
		.get_features = rtl8224_get_features,
		.read_status = rtl8224_read_status,
		.config_aneg = rtl8224_config_aneg,
	},
};

module_phy_driver(rtl8224_phy_driver);

MODULE_AUTHOR("D. Dejean");
MODULE_DESCRIPTION("Realtek RTL8224 PHY driver");
MODULE_LICENSE("GPL");

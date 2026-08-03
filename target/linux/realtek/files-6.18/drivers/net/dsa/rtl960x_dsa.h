/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef __RTL960X_DSA_H
#define __RTL960X_DSA_H

#include <linux/bitfield.h>

/*
 * RTL9607C family switch-core port map (11 ports, 0..10):
 *   0..4  internal GPHY ports
 *   5     SGMII or PON                          (WAN-capable)
 *   6     HSGMII, muxed with USB 3.0            (WAN-capable)
 *   7     2nd HSGMII, muxed with PCIE1 or GMAC2
 *   8     reserved / unused
 *   9,10  CPU ports (GMAC0 / GMAC1)
 *
 * The indirect MDIO bus exposes all five GPHY addresses. At bring-up, the
 * disabled-port mask read from the efuse determines which are usable.
 */

#define RTL960X_NUM_PORTS	11
#define RTL960X_CPU_PORT	9
#define RTL960X_NUM_VLANS	4096
#define SWCORE_MAX_PHY_PORT	5	/* internal GPHY ports 0..4 */

/* MDIO access registers */
#define PHY_OCP_ADDR_PHYREG_BASE	0xa400
#define SWCORE_GPHY_IND_WD		0x000
#define SWCORE_GPHY_IND_CMD		0x004
#define MDIO_CMD_WRITE			BIT(22)
#define MDIO_CMD_READ			0
#define MDIO_CMD_EN			BIT(21)
#define MDIO_CMD_PHYID			GENMASK(20, 16)
#define SWCORE_GPHY_IND_RD		0x008
#define MDIO_STS_BUSY			BIT(16)
#define MDIO_STS_DATA(v)		((v) & 0xffff)

#define SWCORE_CFG_PHY_INI		0x04c
#define SWCORE_CFG_PHY_INI_PWRUP	GENMASK(14, 10)
#define SWCORE_WRAP_GPHY_MISC		0x114		/* PHY analog patch trigger */
#define SWCORE_FORCE_P_ABLTY(port)	(0x1cc + (port) * 4)
#define FORCE_P_ABLTY_NWAY		BIT(7)
#define FORCE_P_ABLTY_LINK		BIT(4)
#define FORCE_P_ABLTY_DUPLEX		BIT(2)
#define FORCE_P_ABLTY_SPEED_MASK	GENMASK(1, 0)
#define FORCE_P_ABLTY_SPEED_1000	FIELD_PREP(FORCE_P_ABLTY_SPEED_MASK, 2)
#define SWCORE_ABLTY_FORCE_MODE(port)	(0x238 + (port) * 4)
#define SWCORE_EFUSE_BOND_RSLT		0x23024
#define SWCORE_EFUSE_PORT_CAPABILITY	GENMASK(5, 1)

/* Port isolation register */
#define SWCORE_PORT_ISO_CTRL(p)		(0x27000 + (p) * 4)
#define SWCORE_PORT_ISO_MASK		0x7ff

/* MSTP port state registers - one per port, a 2-bit state per tree instance */
#define SWCORE_MSTI_CTRL(p)		(0x1704c + (p) * 4)
#define SWCORE_MSTI_STATE_MASK(msti)	GENMASK(2 * (msti) + 1, 2 * (msti))
#define SWCORE_STP_DISABLED		0
#define SWCORE_STP_BLOCKING		1
#define SWCORE_STP_LEARNING		2
#define SWCORE_STP_FORWARDING		3

/* Port flush command registers */
#define SWCORE_L2_FLUSH_CTRL		0x17044
#define SWCORE_L2_FLUSH_BUSY		BIT(0)
#define SWCORE_L2_FLUSH_MODE_MASK	GENMASK(2, 1)
#define SWCORE_L2_FLUSH_MODE_PORT	FIELD_PREP(SWCORE_L2_FLUSH_MODE_MASK, 0)
#define SWCORE_L2_FLUSH_STATIC		BIT(3)
#define SWCORE_L2_FLUSH_DYNAMIC		BIT(4)
#define SWCORE_L2_FLUSH_EN		0x17048

/* Table Access Registers */
#define SWCORE_TBL_ACCESS_CTRL		0x12000
#define SWCORE_TBL_ACCESS_TYPE_L2	0
#define SWCORE_TBL_ACCESS_TYPE_VLAN	1
#define SWCORE_TBL_ACCESS_CMD_MASK	GENMASK(4, 3)
#define SWCORE_TBL_ACCESS_CMD(cmd)	FIELD_PREP(SWCORE_TBL_ACCESS_CMD_MASK, (cmd))
#define SWCORE_TBL_ACCESS_CMD_READ	0
#define SWCORE_TBL_ACCESS_CMD_WRITE	1
#define SWCORE_TBL_ACCESS_METHOD_MASK	GENMASK(7, 5)
#define SWCORE_TBL_ACCESS_METHOD(v)	FIELD_PREP(SWCORE_TBL_ACCESS_METHOD_MASK, (v))
#define SWCORE_TBL_ACCESS_METHOD_MAC	0
#define SWCORE_TBL_ACCESS_METHOD_NEXT_L2UC	3
#define SWCORE_TBL_ACCESS_ADDR_MASK	GENMASK(23, 12)
#define SWCORE_TBL_ACCESS_ADDR(v)	FIELD_PREP(SWCORE_TBL_ACCESS_ADDR_MASK, (v))
#define SWCORE_TBL_ACCESS_STS		0x12004
#define SWCORE_TBL_ACCESS_ADDR_RET	GENMASK(11, 0)
#define SWCORE_TBL_ACCESS_HIT		BIT(12)
#define SWCORE_TBL_ACCESS_BUSY		BIT(13)
#define SWCORE_TBL_ACCESS_WR_DATA(n)	(0x12008 + (n) * 4)
#define SWCORE_TBL_ACCESS_RD_DATA(n)	(0x1201c + (n) * 4)

#define SWCORE_L2_NUM_ENTRIES		2112	/* 2048 hash + 64 CAM */
#define SWCORE_L2_AGE_MAX		7

/*
 * L2 unicast table fields (bit offset over the 80-bit entry). NOT_SALEARN
 * marks static entries; AGE is an independent timer kept at its maximum.
 */
#define L2UC_MAC_LSP		0
#define L2UC_MAC_LEN		48
#define L2UC_CVID_LSP		48
#define L2UC_CVID_LEN		12
#define L2UC_NOSALEARN_LSP	61
#define L2UC_IVL_SVL_LSP	62
#define L2UC_FID_LSP		63
#define L2UC_FID_LEN		2
#define L2UC_SPA_LSP		66
#define L2UC_SPA_LEN		4
#define L2UC_AGE_LSP		70
#define L2UC_AGE_LEN		3
#define L2UC_VALID_LSP		79

/* VLAN table entry (one 32-bit word): member mask [10:0], untag mask [21:11] */
#define VLAN_MBR_MASK		0x7ff
#define VLAN_UNTAG_SHIFT	11

/* VLAN registers */
#define SWCORE_VLAN_ACCEPT		0x13000
#define SWCORE_VLAN_INGRESS		0x13004
#define SWCORE_VLAN_CTRL		0x13008
#define SWCORE_VLAN_CTRL_FILTER_EN	BIT(0)
#define SWCORE_VLAN_PB_VID		0x1300c
#define SWCORE_VLAN_PVID_BITS		12
#define SWCORE_VLAN_PVID_MASK		0xfff
#define SWCORE_VLAN_EGR_TAG(p)		(0x2a000 + (p) * 4)
#define SWCORE_VLAN_EGR_MODE_ORI	0

#endif /* __RTL960X_DSA_H */

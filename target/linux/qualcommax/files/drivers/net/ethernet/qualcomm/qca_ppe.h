/* SPDX-License-Identifier: GPL-2.0-or-later OR MIT */

#ifndef __QCA_PPE_H
#define __QCA_PPE_H

#include <linux/bitfield.h>
#include <linux/bitmap.h>
#include <linux/cleanup.h>
#include <linux/mutex.h>
#include <linux/regmap.h>
#include <linux/spinlock.h>
#include <linux/workqueue.h>
#include <linux/types.h>
#include <linux/io.h>
#include <net/dsa.h>

#define QCA_PPE_MAX_PORTS		8
#define QCA_PPE_CPU_PORT		0
#define QCA_PPE_MAX_BRIDGES		8


/* --- Global --- */
#define PPE_PORT_MUX_CTRL		0x10

/* CPPE (IPQ60xx) PORT_MUX_CTRL bit layout */
#define   CPPE_PORT3_PCS_SEL		GENMASK(1, 0)
#define   CPPE_PORT5_PCS_SEL		GENMASK(5, 4)
#define   CPPE_PORT5_GMAC_SEL		BIT(6)
#define   CPPE_PCS0_CH4_SEL		BIT(7)
#define   CPPE_PORT3_PCS0_CH4		1
#define   CPPE_PORT5_PCS1_CH0		1

/* HPPE (IPQ807x) PORT_MUX_CTRL bit layout */
#define   HPPE_PORT4_PCS_SEL		BIT(0)
#define   HPPE_PORT5_PCS_SEL		GENMASK(2, 1)
#define   HPPE_PORT5_GMAC_SEL		BIT(3)
#define   HPPE_PORT6_PCS_SEL		BIT(4)
#define   HPPE_PORT6_GMAC_SEL		BIT(5)
#define   HPPE_PORT4_PCS0		1
#define   HPPE_PORT5_PCS0		1
#define   HPPE_PORT5_PCS1		2
#define   HPPE_PORT6_PCS2		1
#define   HPPE_PORT5_GMAC_SEL_GMAC	1
#define   HPPE_PORT6_GMAC_SEL_GMAC	1

/* --- MAC CSR (base 0x001000) --- */
#define PPE_MAC_CSR_BASE		0x001000

#define PPE_GMAC_ENABLE(gmac)		(PPE_MAC_CSR_BASE + (gmac) * 0x200)
#define   PPE_MAC_ENABLE_RXMAC_EN	BIT(0)
#define   PPE_MAC_ENABLE_TXMAC_EN	BIT(1)
#define   PPE_MAC_ENABLE_DUPLEX		BIT(4)
#define   PPE_MAC_ENABLE_RX_FLOW_EN	BIT(5)
#define   PPE_MAC_ENABLE_TX_FLOW_EN	BIT(6)

#define PPE_LPBK_ENABLE(gmac)		(PPE_MAC_CSR_BASE + (gmac) * 0x200)
#define   PPE_LPBK_EN			BIT(0)
#define   PPE_LPBK_CRC_STRIP_EN		BIT(3)

#define PPE_GMAC_SPEED(gmac)		(PPE_MAC_CSR_BASE + (gmac) * 0x200 + 0x4)
#define   PPE_GMAC_SPEED_MASK		GENMASK(1, 0)

#define PPE_LPBK_PPS_CTRL(gmac)		(PPE_MAC_CSR_BASE + (gmac) * 0x200 + 0x0c)
#define   PPE_LPBK_PPS_THRESHOLD	GENMASK(8, 0)

#define PPE_GMAC_CTRL2(gmac)		(PPE_MAC_CSR_BASE + (gmac) * 0x200 + 0x18)
#define   PPE_GMAC_CTRL2_MAXFR		GENMASK(21, 8)
#define   PPE_GMAC_CTRL2_CRS_SEL	BIT(6)
#define   PPE_GMAC_CTRL2_TX_THD		GENMASK(27, 24)

#define PPE_GMAC_DBG_CTRL(gmac)		(PPE_MAC_CSR_BASE + (gmac) * 0x200 + 0x1c)
#define   PPE_GMAC_DBG_CTRL_HIHG_IPG	GENMASK(15, 8)

#define PPE_GMAC_JUMBO_SIZE(gmac)	(PPE_MAC_CSR_BASE + (gmac) * 0x200 + 0x30)

#define PPE_GMAC_MIB_CTRL(gmac)		(PPE_MAC_CSR_BASE + (gmac) * 0x200 + 0x34)
#define   PPE_MIB_EN			BIT(0)
#define   PPE_MIB_RD_CLR		BIT(2)

#define PPE_GMAC_MIB(gmac, off)		(PPE_MAC_CSR_BASE + (gmac) * 0x200 + (off))
#define   PPE_MIB_RXBROAD		0x40
#define   PPE_MIB_RXPAUSE		0x44
#define   PPE_MIB_RXMULTI		0x48
#define   PPE_MIB_RXFCSERR		0x4c
#define   PPE_MIB_RXALIGNERR		0x50
#define   PPE_MIB_RXRUNT		0x54
#define   PPE_MIB_RXFRAG		0x58
#define   PPE_MIB_RXJUMBOFCSERR		0x5c
#define   PPE_MIB_RXJUMBOALIGNERR	0x60
#define   PPE_MIB_RXPKT64		0x64
#define   PPE_MIB_RXPKT65TO127		0x68
#define   PPE_MIB_RXPKT128TO255		0x6c
#define   PPE_MIB_RXPKT256TO511		0x70
#define   PPE_MIB_RXPKT512TO1023	0x74
#define   PPE_MIB_RXPKT1024TO1518	0x78
#define   PPE_MIB_RXPKT1519TOX		0x7c
#define   PPE_MIB_RXTOOLONG		0x80
#define   PPE_MIB_RXGOODBYTE_L		0x84
#define   PPE_MIB_RXGOODBYTE_H		0x88
#define   PPE_MIB_RXBADBYTE_L		0x8c
#define   PPE_MIB_RXBADBYTE_H		0x90
#define   PPE_MIB_RXUNI			0x94
#define   PPE_MIB_TXBROAD		0xa0
#define   PPE_MIB_TXPAUSE		0xa4
#define   PPE_MIB_TXMULTI		0xa8
#define   PPE_MIB_TXUNDERRUN		0xac
#define   PPE_MIB_TXPKT64		0xb0
#define   PPE_MIB_TXPKT65TO127		0xb4
#define   PPE_MIB_TXPKT128TO255		0xb8
#define   PPE_MIB_TXPKT256TO511		0xbc
#define   PPE_MIB_TXPKT512TO1023	0xc0
#define   PPE_MIB_TXPKT1024TO1518	0xc4
#define   PPE_MIB_TXPKT1519TOX		0xc8
#define   PPE_MIB_TXBYTE_L		0xcc
#define   PPE_MIB_TXBYTE_H		0xd0
#define   PPE_MIB_TXCOLLISIONS		0xd4
#define   PPE_MIB_TXABORTCOL		0xd8
#define   PPE_MIB_TXMULTICOL		0xdc
#define   PPE_MIB_TXSINGLECOL		0xe0
#define   PPE_MIB_TXEXCESSIVEDEFER	0xe4
#define   PPE_MIB_TXDEFER		0xe8
#define   PPE_MIB_TXLATECOL		0xec
#define   PPE_MIB_TXUNI			0xf0

/* --- XGMAC (base 0x003000) --- */
#define PPE_MAC_XGMAC_CSR_BASE		0x003000

/* MMC block: transmit counters from 0x800, receive from 0x900. Which offset
 * answers for which GMAC counter is in the MIB table in qca_ppe_main.c.
 */
#define PPE_XGMAC_MIB(xgmac, off)	(PPE_MAC_XGMAC_CSR_BASE + \
					 (xgmac) * 0x4000 + (off))

#define PPE_XGMAC_TX_CONF(xgmac)	(PPE_MAC_XGMAC_CSR_BASE + (xgmac) * 0x4000)
#define   PPE_XGMAC_TX_ENABLE		BIT(0)
#define   PPE_XGMAC_JABBER_DISABLE	BIT(16) /* Called JD */
#define   PPE_XGMAC_SPEED_SELECT	GENMASK(30, 29)
#define     PPE_XGMAC_SPEED_SELECT_10000 FIELD_PREP_CONST(PPE_XGMAC_SPEED_SELECT, 0x0)
#define     PPE_XGMAC_SPEED_SELECT_5000	FIELD_PREP_CONST(PPE_XGMAC_SPEED_SELECT, 0x1)
#define     PPE_XGMAC_SPEED_SELECT_2500	FIELD_PREP_CONST(PPE_XGMAC_SPEED_SELECT, 0x2)
#define     PPE_XGMAC_SPEED_SELECT_1000	FIELD_PREP_CONST(PPE_XGMAC_SPEED_SELECT, 0x3)
#define   PPE_XGMAC_USXGMII_SELECT	BIT(31)

#define PPE_XGMAC_RX_CONF(xgmac)	(PPE_MAC_XGMAC_CSR_BASE + (xgmac) * 0x4000 + 0x4)
#define   PPE_XGMAC_RX_ENABLE		BIT(0)
#define   PPE_XGMAC_AUTO_CRC_STRIP	BIT(1) /* Called ACS */
#define   PPE_XGMAC_CRC_STRIP_TYPE	BIT(2) /* Called CST */
#define   PPE_XGMAC_GMII_MPLS_LAYER_CK	BIT(6) /* Called GMPSLCE */
#define   PPE_XGMAC_WATCHDOG_DISABLE	BIT(7) /* Called WD */
#define   PPE_XGMAC_LOOPBACK		BIT(10) /* Called LM */

#define PPE_XGMAC_PACKET_FILTER(xgmac)	(PPE_MAC_XGMAC_CSR_BASE + (xgmac) * 0x4000 + 0x8)
#define   PPE_XGMAC_PROMISCUOUS		BIT(0) /* Called PR */
#define   PPE_XGMAC_PASS_CONTROL_FRAME	GENMASK(7, 6) /* Called PCF */
#define   PPE_XGMAC_RATE_ADAPTATION	BIT(31) /* Called RA */

#define PPE_XGMAC_WATCHDOG_TIMEOUT(xgmac) (PPE_MAC_XGMAC_CSR_BASE + (xgmac) * 0x4000 + 0xc)
#define   PPE_XGMAC_WATCHDOG_THRESHOLD	GENMASK(3, 0)
#define   PPE_XGMAC_WATCHDOG_ENABLE	BIT(8)

#define PPE_XGMAC_TX_FLOW_CTRL(xgmac)	(PPE_MAC_XGMAC_CSR_BASE + (xgmac) * 0x4000 + 0x70)
#define   PPE_XGMAC_TX_FLOW_ENABLE	BIT(1)
#define   PPE_XGMAC_PAUSE_TIME		GENMASK(31, 16)

#define PPE_XGMAC_RX_FLOW_CTRL(xgmac)	(PPE_MAC_XGMAC_CSR_BASE + (xgmac) * 0x4000 + 0x90)
#define   PPE_XGMAC_RX_FLOW_ENABLE	BIT(0)

/* --- PRX (base 0x00b000) --- */
#define PPE_PRX_BASE			0x00b000

#define PPE_PRX_TDM_CTRL		(PPE_PRX_BASE + 0x0)
#define   PPE_TDM_DEPTH			GENMASK(7, 0)
#define   PPE_TDM_EN			BIT(31)

#define PPE_PRX_TDM_CFG(i)		(PPE_PRX_BASE + 0x1000 + (i) * 0x10)
#define   PPE_TDM_PORT_NUM		GENMASK(3, 0)
#define   PPE_TDM_DIR			BIT(4)
#define   PPE_TDM_VALID			BIT(5)

/* --- Ingress VLAN (base 0x00f000) --- */
#define PPE_IVLAN_BASE			0x00f000

#define PPE_PORT_DEF_VID(port)		(PPE_IVLAN_BASE + 0x10 + (port) * 0x4)
#define   PPE_PORT_DEF_CVID		GENMASK(27, 16)
#define   PPE_PORT_DEF_CVID_EN		BIT(28)

#define PPE_PORT_VLAN_CFG(port)		(PPE_IVLAN_BASE + 0x50 + (port) * 0x4)
#define   PPE_VLAN_XLT_MISS_FWD		GENMASK(6, 5)

#define PPE_XLT_RULE_TBL(idx)		(PPE_IVLAN_BASE + 0x2000 + (idx) * 0x10)
#define   PPE_XLT_VALID			BIT(0)
#define   PPE_XLT_PORT_BMP		GENMASK(8, 1)
#define   PPE_XLT_SKEY_FMT		GENMASK(11, 9)
#define   PPE_XLT_SKEY_UNTAGGED		1
#define   PPE_XLT_CKEY_FMT_0		BIT(31)

#define PPE_XLT_RULE_W1(idx)		(PPE_IVLAN_BASE + 0x2000 + (idx) * 0x10 + 0x4)
#define   PPE_XLT_CKEY_FMT_1		GENMASK(1, 0)
#define   PPE_XLT_CKEY_VID_INCL		BIT(2)
#define   PPE_XLT_CKEY_VID		GENMASK(14, 3)

#define PPE_XLT_ACTION_TBL(idx)		(PPE_IVLAN_BASE + 0x4000 + (idx) * 0x10)

#define PPE_XLT_ACTION_W1(idx)		(PPE_IVLAN_BASE + 0x4000 + (idx) * 0x10 + 0x4)
#define   PPE_XLT_VSI_CMD		BIT(11)
#define   PPE_XLT_VSI			GENMASK(16, 12)

/* --- PTX (base 0x020000) --- */
#define PPE_PTX_BASE			0x020000

#define PPE_EG_VSI_TAG(vsi)		(PPE_PTX_BASE + (vsi) * 0x4)
#define   PPE_EG_VSI_TAG_UNMODIFIED	0xaaaa

#define PPE_EG_BRIDGE_CONFIG		(PPE_PTX_BASE + 0x6000)
#define   PPE_EG_L2_EDIT_EN		BIT(1)
#define   PPE_EG_QUEUE_CNT_EN		BIT(2)

#define PPE_PORT_EG_VLAN(port)		(PPE_PTX_BASE + 0x420 + (port) * 0x4)
#define   PPE_PORT_EG_VLAN_CTAG_MODE	GENMASK(2, 1)
#define   PPE_PORT_EG_VLAN_STAG_MODE	GENMASK(4, 3)
#define   PPE_PORT_EG_VSI_TAG_EN	BIT(5)
#define   PPE_PORT_EG_VLAN_TX_CNT_EN	BIT(8)

#define PPE_EG_UNTOUCHED		3

/* --- L2 (base 0x060000) --- */
#define PPE_L2_BASE			0x060000

#define PPE_FDB_OP			(PPE_L2_BASE + 0x8)
#define PPE_FDB_RD_OP			(PPE_L2_BASE + 0x10)
#define PPE_FDB_OP_RSLT			(PPE_L2_BASE + 0x20)
#define PPE_FDB_RD_OP_RSLT		(PPE_L2_BASE + 0x30)

#define PPE_AGE_TIMER			(PPE_L2_BASE + 0x34)
#define   PPE_AGE_TIMER_MASK		GENMASK(19, 0)

#define PPE_L2_GLOBAL_CONF		(PPE_L2_BASE + 0x38)
#define   PPE_L2_LRN_EN			BIT(6)
#define   PPE_L2_AGE_EN			BIT(7)

#define PPE_CST_STATE(port)		(PPE_L2_BASE + 0x100 + (port) * 0x4)
#define   PPE_STP_DISABLED		0
#define   PPE_STP_BLOCKING		1
#define   PPE_STP_LEARNING		2
#define   PPE_STP_FORWARDING		3
#define   PPE_STP_STATE_MASK		GENMASK(1, 0)

#define PPE_FDB_RD_RSLT_DATA0		(PPE_L2_BASE + 0x200)
#define PPE_FDB_RD_RSLT_DATA1		(PPE_L2_BASE + 0x204)
#define PPE_FDB_RD_RSLT_DATA2		(PPE_L2_BASE + 0x208)

#define PPE_FDB_OP_DATA0		(PPE_L2_BASE + 0x230)
#define PPE_FDB_OP_DATA1		(PPE_L2_BASE + 0x234)
#define PPE_FDB_OP_DATA2		(PPE_L2_BASE + 0x238)

#define PPE_FDB_RD_OP_DATA0		(PPE_L2_BASE + 0x260)
#define PPE_FDB_RD_OP_DATA1		(PPE_L2_BASE + 0x264)
#define PPE_FDB_RD_OP_DATA2		(PPE_L2_BASE + 0x268)

#define PPE_MIRROR_ANALYZER		(PPE_L2_BASE + 0x40)
#define   PPE_MIRROR_IN_ANALYZER	GENMASK(5, 0)
#define   PPE_MIRROR_EG_ANALYZER	GENMASK(13, 8)

#define PPE_PORT_MIRROR(port)		(PPE_L2_BASE + 0x800 + (port) * 0x4)
#define   PPE_PORT_MIRROR_IN_EN		BIT(0)
#define   PPE_PORT_MIRROR_EG_EN		BIT(1)

#define PPE_PORT_BRIDGE_CTRL(port)	(PPE_L2_BASE + 0x300 + (port) * 0x4)
#define   PPE_BRIDGE_NEW_LRN_EN		BIT(0)
#define   PPE_BRIDGE_STA_MOVE_EN	BIT(3)
#define   PPE_BRIDGE_PORT_ISOL		GENMASK(15, 8)
#define   PPE_PORT_BRIDGE_CTRL_TXMAC_EN	BIT(16)

#define PPE_MC_MTU_CTRL(port)		(PPE_L2_BASE + 0xa00 + (port) * 0x4)
#define   PPE_MC_MTU_CTRL_MTU		GENMASK(13, 0)
#define   PPE_MC_MTU_CTRL_MTU_CMD	GENMASK(15, 14)
#define   PPE_MC_MTU_CTRL_TX_CNT_EN	BIT(16)

#define PPE_RFDB_TBL(idx)		(PPE_L2_BASE + 0x1000 + (idx) * 0x8)

#define PPE_APP_CTRL(idx)		(PPE_L2_BASE + 0x1400 + (idx) * 0x10)
/* Fields in the third 32-bit word of APP_CTRL. */
#define   PPE_APP_CTRL_PORT_BITMAP_EN	BIT(2)
#define   PPE_APP_CTRL_PORT_BITMAP	GENMASK(10, 3)
#define   PPE_APP_CTRL_STP_BYPASS	BIT(12)
#define   PPE_APP_CTRL_CMD		GENMASK(16, 15)
#define   PPE_APP_CTRL_REDIRECT_CPU	3

#define PPE_PORT_QOS_CTRL(p)		(PPE_L2_BASE + 0x900 + (p) * 0x10)
#define   PPE_QOS_DSCP_PREC		GENMASK(5, 3)
#define   PPE_QOS_PCP_PREC		GENMASK(8, 6)
#define   PPE_QOS_PREHEADER_PREC	GENMASK(11, 9)
#define   PPE_QOS_FLOW_PREC		GENMASK(14, 12)
#define   PPE_QOS_ACL_PREC		GENMASK(17, 15)

#define PPE_PCP_QOS_GROUP(g, pcp)	(PPE_L2_BASE + 0xb00 + (g) * 0x100 + \
					 (pcp) * 0x4)
#define PPE_PCP_QOS_ENTRIES		16
#define PPE_FLOW_QOS_GROUP(g, prof)	(PPE_L2_BASE + 0xd00 + (g) * 0x100 + \
					 (prof) * 0x4)
#define PPE_DSCP_QOS_GROUP(g, dscp)	(PPE_L2_BASE + 0x2000 + (g) * 0x800 + \
					 (dscp) * 0x10)
#define   PPE_QOS_INFO_PCP		GENMASK(2, 0)
#define   PPE_QOS_INFO_DEI		BIT(3)
#define   PPE_QOS_INFO_PRI		GENMASK(7, 4)
#define   PPE_QOS_INFO_DSCP		GENMASK(13, 8)
#define   PPE_QOS_INFO_DP		GENMASK(15, 14)

/* The highest priority a packet may be given. The port's top four unicast
 * queues share an L0 node and a DRR node with its multicast queues, and the
 * scheduler cannot be told to rotate credit for two queues on one node.
 */
#define PPE_QOS_MAX_PRI			11

#define PPE_VSI_TBL(vsi)		(PPE_L2_BASE + 0x1800 + (vsi) * 0x10)
#define   PPE_VSI_TBL_MEMBER		GENMASK(7, 0)
#define   PPE_VSI_TBL_UUC		GENMASK(15, 8)
#define   PPE_VSI_TBL_UMC		GENMASK(23, 16)
#define   PPE_VSI_TBL_BC		GENMASK(31, 24)
#define   PPE_VSI_TBL_NEW_ADDR_LRN_EN	BIT(0)
#define   PPE_VSI_TBL_STA_MOVE_LRN_EN	BIT(3)

#define PPE_MRU_MTU_CTRL(port, stride)	(PPE_L2_BASE + 0x3000 + (port) * (stride))
#define   PPE_MRU_MTU_CTRL_MRU		GENMASK(13, 0)
#define   PPE_MRU_MTU_CTRL_MRU_CMD	GENMASK(15, 14)
#define   PPE_MRU_MTU_CTRL_MTU		GENMASK(29, 16)
#define   PPE_MRU_MTU_CTRL_MTU_CMD	GENMASK(31, 30)
#define     PPE_SIZE_CMD_DROP		1
#define     PPE_SIZE_CMD_RDT_TO_CPU	3
#define   PPE_MRU_MTU_CTRL_RX_CNT_EN	BIT(0)
#define   PPE_MRU_MTU_CTRL_TX_CNT_EN	BIT(1)
/* IPQ6018 field positions, in the second word of the port's MRU/MTU entry. */
#define   PPE_MRU_QOS_PREHEADER_PREC	GENMASK(10, 8)
#define   PPE_MRU_QOS_PCP_PREC		GENMASK(13, 11)
#define   PPE_MRU_QOS_DSCP_PREC		GENMASK(16, 14)
#define   PPE_MRU_QOS_FLOW_PREC		GENMASK(19, 17)
#define   PPE_MRU_QOS_ACL_PREC		GENMASK(22, 20)

/* --- L3 (base 0x200000) --- */
#define PPE_L3_BASE			0x200000

#define PPE_L3_VP_PORT_TBL(port)	(PPE_L3_BASE + 0x1000 + (port) * 0x10)
#define   PPE_L3_VP_VSI_VALID		BIT(9)
#define   PPE_L3_VP_VSI			GENMASK(14, 10)

/* --- Ingress policer (base 0x100000) --- */
#define PPE_POLICER_BASE		0x100000

#define PPE_POLICER_TIME_SLOT		(PPE_POLICER_BASE + 0x40)
#define   PPE_POLICER_SLOT_TIME		GENMASK(9, 0)

#define PPE_PORT_METER_W0(p)		(PPE_POLICER_BASE + 0xc000 + (p) * 0x10)
#define   PPE_METER_EN			BIT(0)
#define   PPE_METER_FRAME_TYPE		GENMASK(6, 2)
#define   PPE_METER_MODE		BIT(8)
#define   PPE_METER_TOKEN_UNIT		GENMASK(11, 9)
#define   PPE_METER_UNIT		BIT(12)
#define   PPE_METER_CBS			GENMASK(28, 13)
#define   PPE_METER_CIR_LO		GENMASK(31, 29)
#define PPE_PORT_METER_W1(p)		(PPE_POLICER_BASE + 0xc000 + (p) * 0x10 + 0x4)
#define   PPE_METER_CIR_HI		GENMASK(14, 0)
#define PPE_PORT_METER_W2(p)		(PPE_POLICER_BASE + 0xc000 + (p) * 0x10 + 0x8)
#define PPE_PORT_METER_W3(p)		(PPE_POLICER_BASE + 0xc000 + (p) * 0x10 + 0xc)

#define PPE_PORT_TX_DROP_CNT(p)	(PPE_POLICER_BASE + 0x7d000 + (p) * 0x10)

#define PPE_PORT_METER_CNT(p, c)	(PPE_POLICER_BASE + 0xe000 + \
					 ((p) * 3 + (c)) * 0x10)

/* --- Traffic Manager (base 0x400000) --- */
#define PPE_TM_BASE			0x400000

#define PPE_TM_TDM_DEPTH		(PPE_TM_BASE + 0x0)
#define   PPE_TM_TDM_DEPTH_MASK		GENMASK(7, 0)

#define PPE_TM_L0_FLOW_MAP(i)		(PPE_TM_BASE + 0x2000 + (i) * 0x10)
#define   PPE_L0_SP_ID			GENMASK(5, 0)
#define   PPE_L0_C_PRI			GENMASK(8, 6)
#define   PPE_L0_E_PRI			GENMASK(11, 9)
#define   PPE_L0_C_DRR_WT		GENMASK(21, 12)
#define   PPE_L0_E_DRR_WT		GENMASK(31, 22)

#define PPE_TM_L0_C_SP(i)		(PPE_TM_BASE + 0x4000 + (i) * 0x10)
#define PPE_TM_L0_E_SP(i)		(PPE_TM_BASE + 0x6000 + (i) * 0x10)
#define   PPE_L0_SP_DRR_ID		GENMASK(7, 0)

#define PPE_TM_L0_PORT_MAP(i)		(PPE_TM_BASE + 0x8000 + (i) * 0x10)
#define   PPE_L0_PORT_NUM		GENMASK(3, 0)

#define PPE_TM_RING_Q_MAP(r)		(PPE_TM_BASE + 0x2a000 + (r) * 0x40)

#define PPE_TM_L1_FLOW_MAP(i)		(PPE_TM_BASE + 0x40000 + (i) * 0x10)
#define   PPE_L1_SP_ID			GENMASK(3, 0)
#define   PPE_L1_C_PRI			GENMASK(6, 4)
#define   PPE_L1_E_PRI			GENMASK(9, 7)
#define   PPE_L1_C_DRR_WT		GENMASK(19, 10)
#define   PPE_L1_E_DRR_WT		GENMASK(29, 20)

#define PPE_TM_L1_C_SP(i)		(PPE_TM_BASE + 0x42000 + (i) * 0x10)
#define PPE_TM_L1_E_SP(i)		(PPE_TM_BASE + 0x44000 + (i) * 0x10)
#define   PPE_L1_SP_DRR_ID		GENMASK(5, 0)

#define PPE_TM_L1_PORT_MAP(i)		(PPE_TM_BASE + 0x46000 + (i) * 0x10)
#define   PPE_L1_PORT_NUM		GENMASK(3, 0)

#define PPE_TM_PSCH_TDM(i)		(PPE_TM_BASE + 0x7a000 + (i) * 0x10)
#define   PPE_PSCH_DES_PORT		GENMASK(3, 0)
#define   PPE_PSCH_ENS_PORT		GENMASK(7, 4)
#define   PPE_PSCH_ENS_PORT_BMP		GENMASK(15, 8)

#define PPE_TM_IPG_PRE_LEN		(PPE_TM_BASE + 0x8)
#define   PPE_IPG_PRE_LEN		GENMASK(4, 0)

#define PPE_TM_SHP_SLOT_PORT		(PPE_TM_BASE + 0x18)
#define   PPE_PORT_SHP_SLOT_TIME	GENMASK(11, 0)

#define PPE_TM_SHP_SLOT_L0		(PPE_TM_BASE + 0x10)
#define PPE_TM_SHP_SLOT_L1		(PPE_TM_BASE + 0x14)
#define   PPE_SHP_SLOT_TIME		GENMASK(11, 0)

/* A scheduler node's shaper is two token buckets in one entry, committed and
 * excess, one per input the node offers its parent, and each has its own
 * enable. W0 is the committed rate and depth, W1 the excess pair in the same
 * two fields, W2 the units and the enables. The credit entry is one bucket per
 * input as well, which is why clearing it takes both of its words.
 */
#define PPE_TM_L0_SHP_CREDIT(q)		(PPE_TM_BASE + 0x1a000 + (q) * 0x10)
#define PPE_TM_L0_SHP_CFG(q)		(PPE_TM_BASE + 0x1c000 + (q) * 0x10)
#define PPE_TM_L1_SHP_CREDIT(n)		(PPE_TM_BASE + 0x5c000 + (n) * 0x10)
#define PPE_TM_L1_SHP_CFG(n)		(PPE_TM_BASE + 0x5e000 + (n) * 0x10)
#define   PPE_SHP_CIR			GENMASK(17, 0)
#define   PPE_SHP_CBS			GENMASK(31, 18)
#define   PPE_SHP_TOKEN_UNIT		GENMASK(2, 0)
#define   PPE_SHP_METER_UNIT		BIT(3)
#define   PPE_SHP_C_EN			BIT(4)
#define   PPE_SHP_E_EN			BIT(5)

#define PPE_TM_PSCH_SHP_SIGN(p)	(PPE_TM_BASE + 0x70000 + (p) * 0x10)
#define PPE_TM_PSCH_SHP_CREDIT(p)	(PPE_TM_BASE + 0x72000 + (p) * 0x10)

#define PPE_TM_PSCH_SHP_CFG_W0(p)	(PPE_TM_BASE + 0x74000 + (p) * 0x10)
#define   PPE_PSCH_SHP_CIR		GENMASK(17, 0)
#define   PPE_PSCH_SHP_CBS		GENMASK(31, 18)
#define PPE_TM_PSCH_SHP_CFG_W1(p)	(PPE_TM_BASE + 0x74000 + (p) * 0x10 + 0x4)
#define   PPE_PSCH_SHP_TOKEN_UNIT	GENMASK(2, 0)
#define   PPE_PSCH_SHP_METER_UNIT	BIT(3)
#define   PPE_PSCH_SHP_EN		BIT(4)

/* --- Buffer Manager (base 0x600000) --- */
#define PPE_BM_BASE			0x600000

#define PPE_BM_FC_MODE(i)		(PPE_BM_BASE + 0x100 + (i) * 0x4)
#define   PPE_BM_FC_EN			BIT(0)

#define PPE_BM_GROUP_ID(i)		(PPE_BM_BASE + 0x180 + (i) * 0x4)

#define PPE_BM_SHARED_GRP(g)		(PPE_BM_BASE + 0x290 + (g) * 0x4)
#define   PPE_BM_SHARED_LIMIT		GENMASK(10, 0)

#define PPE_BM_PORT_FC_W0(i)		(PPE_BM_BASE + 0x1000 + (i) * 0x10)
#define PPE_BM_PORT_FC_W1(i)		(PPE_BM_BASE + 0x1000 + (i) * 0x10 + 0x4)
#define   PPE_BM_REACT_LIMIT		GENMASK(8, 0)
#define   PPE_BM_RESUME_FLOOR		GENMASK(17, 9)
#define   PPE_BM_RESUME_OFF		GENMASK(28, 18)
#define   PPE_BM_CEILING_LO		GENMASK(31, 29)
#define   PPE_BM_CEILING_HI		GENMASK(7, 0)
#define   PPE_BM_WEIGHT			GENMASK(10, 8)
#define   PPE_BM_DYNAMIC		BIT(11)
#define   PPE_BM_PREALLOC		GENMASK(22, 12)

/* --- Queue Manager (base 0x800000) --- */
#define PPE_QM_BASE			0x800000

#define PPE_QM_UCAST_MAP(i)		(PPE_QM_BASE + 0x10000 + (i) * 0x10)
#define   PPE_QM_PROFILE_ID		GENMASK(3, 0)
#define   PPE_QM_QUEUE_ID		GENMASK(11, 4)

#define PPE_QM_UCAST_HASH_MAP(i)	(PPE_QM_BASE + 0x30000 + (i) * 0x10)
#define   PPE_QM_HASH_CLASS		GENMASK(3, 0)

#define PPE_QM_UCAST_PRI_MAP(i)		(PPE_QM_BASE + 0x42000 + (i) * 0x10)
#define   PPE_QM_PRI_CLASS		GENMASK(3, 0)

#define PPE_QM_AC_UNI_W0(i)		(PPE_QM_BASE + 0x48000 + (i) * 0x10)
#define PPE_QM_AC_UNI_W1(i)		(PPE_QM_BASE + 0x48000 + (i) * 0x10 + 0x4)
#define PPE_QM_AC_UNI_W2(i)		(PPE_QM_BASE + 0x48000 + (i) * 0x10 + 0x8)
#define PPE_QM_AC_UNI_W3(i)		(PPE_QM_BASE + 0x48000 + (i) * 0x10 + 0xc)
#define   PPE_AC_EN			BIT(0)
/* The vendor calls this FORCE_AC_EN and the in-kernel ppe driver calls it
 * FC_EN; measured, it is what makes the limit bind. A queue held to twelve
 * buffers under a 20 Mbit/s ceiling passes 5 Mbit/s with this set and the
 * ceiling's full rate with it clear, so clear means the limit is advisory.
 * The vendor leaves it clear, and so does this driver until a port is shaped.
 */
#define   PPE_AC_FORCE_AC_EN		BIT(2)
#define   PPE_AC_GRP_ID			GENMASK(5, 4)
#define   PPE_AC_SHARED_DYNAMIC		BIT(17)
#define   PPE_AC_SHARED_WEIGHT		GENMASK(20, 18)
#define   PPE_AC_SHARED_CEILING		GENMASK(31, 21)
#define   PPE_AC_GRN_RESUME_OFF		GENMASK(23, 13)

#define PPE_QM_AC_MUL_W0(i)		(PPE_QM_BASE + 0x4a000 + (i) * 0x10)
#define PPE_QM_AC_MUL_W1(i)		(PPE_QM_BASE + 0x4a000 + (i) * 0x10 + 0x4)
#define PPE_QM_AC_MUL_W2(i)		(PPE_QM_BASE + 0x4a000 + (i) * 0x10 + 0x8)
#define   PPE_AC_MUL_EN			BIT(0)
#define   PPE_AC_MUL_CEILING		GENMASK(26, 16)
#define   PPE_AC_MUL_GRN_MAX_LO		GENMASK(31, 27)
#define   PPE_AC_MUL_GRN_MAX_HI		GENMASK(5, 0)
#define   PPE_AC_MUL_GRN_RESUME_HI	GENMASK(17, 7)

#define PPE_QM_AC_GRP_W0(g)		(PPE_QM_BASE + 0x4c000 + (g) * 0x10)
#define PPE_QM_AC_GRP_W1(g)		(PPE_QM_BASE + 0x4c000 + (g) * 0x10 + 0x4)
#define PPE_QM_AC_GRP_W2(g)		(PPE_QM_BASE + 0x4c000 + (g) * 0x10 + 0x8)
#define   PPE_AC_GRP_LIMIT		GENMASK(14, 4)
#define   PPE_AC_GRP_PALLOC		GENMASK(26, 16)

/* --- FDB fields --- */
#define PPE_FDB_OP_CMD_ID		GENMASK(3, 0)
#define PPE_FDB_OP_TYPE			GENMASK(7, 5)
#define PPE_FDB_OP_HASH_BLOCK		GENMASK(9, 8)
#define PPE_FDB_OP_MODE			BIT(10)
#define PPE_FDB_OP_ENTRY_IDX		GENMASK(21, 11)

#define PPE_FDB_RSLT_CMD_ID		GENMASK(3, 0)

#define PPE_FDB_DATA1_VALID		BIT(16)
#define PPE_FDB_DATA1_LKP_VALID		BIT(17)
#define PPE_FDB_DATA1_VSI		GENMASK(22, 18)
#define PPE_FDB_DATA1_DST_LO		GENMASK(31, 23)

#define PPE_FDB_DATA2_DST_HI		GENMASK(2, 0)
#define PPE_FDB_DATA2_DST_TYPE		GENMASK(4, 3)
#define PPE_FDB_DATA2_HIT_AGE		GENMASK(10, 9)

/* --- Constants --- */
#define PPE_NUM_PORTS			8
#define PPE_MAX_SP_PRI			8
#define PPE_L0_QUEUES			300
#define PPE_L0_UCAST_QUEUES		256

#define PPE_BM_BUF_SIZE			256
#define PPE_BM_PORTS			15
#define PPE_BM_PHY_START		8

#define PPE_VSI_MAX			32
#define PPE_VSI_INVALID			U32_MAX
#define PPE_DEFAULT_MTU			1514
#define PPE_MAX_FRAME_SIZE		12288
#define PPE_AGE_UNIT_MS			8000

#define PPE_FDB_TBL_NUM			2048
#define PPE_FDB_OP_ADD			0
#define PPE_FDB_OP_DEL			1
#define PPE_FDB_OP_GET			2
#define PPE_FDB_DST_PORT		2
#define PPE_FDB_DST_PORTMAP		3
#define PPE_FDB_AGE_STATIC		3
#define PPE_FDB_OP_FLUSH		4

#define PPE_XLT_TBL_NUM			64
#define PPE_XLT_MISS_RDT_TO_CPU		3
#define PPE_XLT_CKEY_TAGGED		4

#define PPE_EG_UNTAGGED			0
#define PPE_EG_TAGGED			1
#define PPE_EG_UNMODIFIED		2

#define PPE_MAX_SERVICE_CODES		256
#define PPE_MAX_CPU_CODES		256
#define PPE_MAX_VPORT			256

#define QM_VP_PORT_OFFSET		0
#define QM_CPU_CODE_OFFSET		1024
#define QM_SERVICE_CODE_OFFSET		2048

struct psch_tdm_data {
	const struct psch_tdm_entry *entries;
	int num;
};

struct bm_tdm_data {
	const struct bm_tdm_entry *entries;
	int num;
};

enum ppe_type {
	PPE_TYPE_IPQ6018,
	PPE_TYPE_IPQ8074,
};

struct ppe_data {
	enum ppe_type type;
	u8 num_ports;
	u8 num_gmacs;
	u8 mru_mtu_ctrl_stride;
	u8 loopback_port;
	u8 bm_phy_end;
	u8 bm_internal_start;
	u16 bm_group_buf;
	u16 bm_ceiling;
	u16 qm_total_buf;
	u16 qm_ceiling;
	u16 qm_green_max;
	const struct psch_tdm_data *psch_tdm;
	const struct bm_tdm_data *bm_tdm;
};

struct qca_ppe_bridge_vsi {
	struct net_device *br_dev;
	u32 vsi;
	int refcount;
};

struct qca_ppe_vlan_entry {
	struct net_device *br_dev;
	u16 vid;
	u32 vsi;
	u8 ports;
	u8 pvid_ports;
	int xlt_idx;
	int xlt_pvid_idx;
};

/* Defined beside the MIB table that dimensions it. */
struct qca_ppe_mib_stats;

/* A traffic class's rate and the scheduler node that will carry it, worked out
 * before any of it is programmed.
 */
struct ppe_class_shaper {
	u32 cfg;
	u32 credit;
	u32 slot;
	u64 rate_bps;
};

/* What a port's offloaded tbf was given, so that a stats read can be a delta and
 * its queues can be resized without the rate being asked for again.
 */
struct ppe_port_shaper {
	u32 tbf_handle;
	u32 bands_handle;
	u32 limit;
	u64 rate_bps;
	u64 queue_rate[PPE_QOS_MAX_PRI + 1];
	u64 base_bytes;
	u32 base_pkts;
	u32 base_drops;
};

struct qca_ppe_priv {
	struct dsa_switch ds;
	struct regmap *regmap;
	const struct ppe_data *data;
	spinlock_t fdb_lock;
	u32 fdb_cmd_id;
	u32 fdb_rd_cmd_id;
	/* Guards the VSI, translation-index and bridge-VLAN state, and the
	 * read-modify-write an MDB update makes of an FDB entry. The switchdev
	 * ops reach it under rtnl, the FDB and MDB work from a workqueue that
	 * holds none.
	 */
	struct mutex vlan_lock;
	s8 mirror_port;
	u16 mirror_ref;
	u8 mirror_dir_ref[QCA_PPE_MAX_PORTS][2];
	struct ppe_port_shaper shaper[QCA_PPE_MAX_PORTS];
	struct dentry *debugfs;
	DECLARE_BITMAP(vsi_bitmap, PPE_VSI_MAX);
	DECLARE_BITMAP(xlt_bitmap, PPE_XLT_TBL_NUM);
	u32 port_vsi[QCA_PPE_MAX_PORTS];
	struct qca_ppe_bridge_vsi bridges[QCA_PPE_MAX_BRIDGES];
	struct qca_ppe_vlan_entry vlans[PPE_VSI_MAX];
	struct net_device *port_br_dev[QCA_PPE_MAX_PORTS];
	u32 vlan_filtering;
	u16 port_pvid[QCA_PPE_MAX_PORTS];
	struct clk *ppe_clk;
	struct clk *port_rx_clk[QCA_PPE_MAX_PORTS];
	struct clk *port_tx_clk[QCA_PPE_MAX_PORTS];
	struct reset_control *port_rst[QCA_PPE_MAX_PORTS];
	bool port_xgmac[QCA_PPE_MAX_PORTS];
	bool mib_xgmac[QCA_PPE_MAX_PORTS];
	bool mib_rebase[QCA_PPE_MAX_PORTS];
	struct qca_ppe_mib_stats *port_mib;
	/* Guards port_mib, port_xgmac, mib_xgmac and mib_rebase; get_stats64
	 * takes it in atomic context, so it is never a mutex.
	 */
	spinlock_t mib_lock;
	struct delayed_work mib_work;
};

extern const struct psch_tdm_data cppe_psch_tdm_data;
extern const struct psch_tdm_data hppe_psch_tdm_data;

extern const struct bm_tdm_data cppe_bm_tdm_data;
extern const struct bm_tdm_data hppe_bm_tdm_data;

static inline struct qca_ppe_priv *ds_to_priv(struct dsa_switch *ds)
{
	return container_of(ds, struct qca_ppe_priv, ds);
}

u64 ppe_mib_read(struct qca_ppe_priv *priv, int port, unsigned int off);

struct tc_tbf_qopt_offload;
struct tc_ets_qopt_offload;
struct tc_query_caps_base;
struct tc_mqprio_qopt_offload;

void ppe_scheduler_init(struct qca_ppe_priv *priv);
int qca_ppe_setup_tc(struct dsa_switch *ds, int port, enum tc_setup_type type,
		     void *type_data);
int qca_ppe_port_get_dscp_prio(struct dsa_switch *ds, int port, u8 dscp);
int qca_ppe_port_add_dscp_prio(struct dsa_switch *ds, int port, u8 dscp,
			       u8 prio);
int qca_ppe_port_del_dscp_prio(struct dsa_switch *ds, int port, u8 dscp,
			       u8 prio);
int qca_ppe_port_get_apptrust(struct dsa_switch *ds, int port, u8 *sel,
			      int *nsel);
int qca_ppe_port_set_apptrust(struct dsa_switch *ds, int port, const u8 *sel,
			      int nsel);
int qca_ppe_setup_tc_tbf(struct qca_ppe_priv *priv, int port,
			 struct tc_tbf_qopt_offload *qopt);
int qca_ppe_setup_tc_ets(struct qca_ppe_priv *priv, int port,
			 struct tc_ets_qopt_offload *qopt);
int qca_ppe_tc_query_caps(struct tc_query_caps_base *base);
int qca_ppe_setup_tc_mqprio(struct qca_ppe_priv *priv, int port,
			    struct tc_mqprio_qopt_offload *qopt);
int qca_ppe_port_mirror_add(struct dsa_switch *ds, int port,
			    struct dsa_mall_mirror_tc_entry *mirror,
			    bool ingress, struct netlink_ext_ack *extack);
void qca_ppe_port_mirror_del(struct dsa_switch *ds, int port,
			     struct dsa_mall_mirror_tc_entry *mirror);
int qca_ppe_port_policer_add(struct dsa_switch *ds, int port,
			     const struct flow_action_police *policer,
			     struct netlink_ext_ack *extack);
void qca_ppe_port_policer_del(struct dsa_switch *ds, int port);

int ppe_vsi_alloc(struct qca_ppe_priv *priv);
void ppe_vsi_free(struct qca_ppe_priv *priv, u32 vsi);
void ppe_vsi_member_set(struct qca_ppe_priv *priv, u32 vsi, u32 portmask);

int qca_ppe_vlan_setup(struct dsa_switch *ds);
int qca_ppe_port_vlan_filtering(struct dsa_switch *ds, int port,
				bool vlan_filtering,
				struct netlink_ext_ack *extack);
struct qca_ppe_vlan_entry *
ppe_vlan_find(struct qca_ppe_priv *priv, struct net_device *br_dev, u16 vid);
int qca_ppe_port_vlan_add(struct dsa_switch *ds, int port,
			  const struct switchdev_obj_port_vlan *vlan,
			  struct netlink_ext_ack *extack);
int qca_ppe_port_vlan_del(struct dsa_switch *ds, int port,
			  const struct switchdev_obj_port_vlan *vlan);

unsigned long ppe_clk_rate(struct qca_ppe_priv *priv);

#endif

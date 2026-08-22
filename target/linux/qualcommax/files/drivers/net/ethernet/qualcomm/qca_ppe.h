/* SPDX-License-Identifier: GPL-2.0-or-later OR MIT */

#ifndef __QCA_PPE_H
#define __QCA_PPE_H

#include <linux/bitfield.h>
#include <linux/bitmap.h>
#include <linux/cleanup.h>
#include <linux/mutex.h>
#include <linux/units.h>
#include <linux/regmap.h>
#include <linux/rhashtable.h>
#include <linux/spinlock.h>
#include <linux/workqueue.h>
#include <linux/types.h>
#include <linux/io.h>
#include <net/dsa.h>

#define QCA_PPE_MAX_PORTS	8
#define QCA_PPE_CPU_PORT	0
#define QCA_PPE_MAX_BRIDGES	8

/* Flow table op engine */
#define PPE_TBL_OP_ADD		0
#define PPE_TBL_OP_DEL		1
#define PPE_TBL_OP_GET		2
#define PPE_TBL_OP_FLUSH	3
#define PPE_FLOW_HASH_BLOCKS	3

#define PPE_FLOW_ENTRY_WORDS_V4	5
#define PPE_FLOW_ENTRY_WORDS_V6	9
#define PPE_HOST_ENTRY_WORDS_V4	3
#define PPE_HOST_ENTRY_WORDS_V6	5

/* The two-bit age field is loaded at its maximum and counts down. */
#define PPE_FLOW_AGE_MAX	3

/* Flow control is configured per packet type and per direction. */
#define PPE_FLOW_PKT_TYPES	3
#define PPE_FLOW_MISS_FORWARD	0

/* Age unit 0 counts seconds, which the hardware derives from the clock rate
 * programmed into PPE_FLOW_CLK_FREQ_MHZ.
 */
#define PPE_FLOW_AGE_UNIT_SEC	0
#define PPE_FLOW_AGE_SECS	60

/* Forward types of a flow entry, as the hardware numbers them. */
#define PPE_FLOW_FWD_FORWARD	0
#define PPE_FLOW_FWD_SNAT	1
#define PPE_FLOW_FWD_DNAT	2
#define PPE_FLOW_FWD_ROUTE	3
#define PPE_FLOW_FWD_BRIDGE	4
#define PPE_FLOW_FWD_DROP	5
#define PPE_FLOW_FWD_TO_CPU	6

/* Protocol encoding of a flow entry's two-bit protocol field. */
#define PPE_FLOW_PROTO_OTHER	0
#define PPE_FLOW_PROTO_TCP	1
#define PPE_FLOW_PROTO_UDP	2


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
#define   PPE_LPBK_CRC_STRIP_EN	BIT(3)

#define PPE_GMAC_SPEED(gmac)		(PPE_MAC_CSR_BASE + (gmac) * 0x200 + 0x4)
#define   PPE_GMAC_SPEED_MASK		GENMASK(1, 0)

#define PPE_LPBK_PPS_CTRL(gmac)	(PPE_MAC_CSR_BASE + (gmac) * 0x200 + 0x0c)
#define   PPE_LPBK_PPS_THRESHOLD	GENMASK(8, 0)

#define PPE_GMAC_CTRL2(gmac)		(PPE_MAC_CSR_BASE + (gmac) * 0x200 + 0x18)
#define   PPE_GMAC_CTRL2_MAXFR		GENMASK(21, 8)
#define   PPE_GMAC_CTRL2_CRS_SEL	BIT(6)
#define   PPE_GMAC_CTRL2_TX_THD	GENMASK(27, 24)

#define PPE_GMAC_DBG_CTRL(gmac)	(PPE_MAC_CSR_BASE + (gmac) * 0x200 + 0x1c)
#define   PPE_GMAC_DBG_CTRL_HIHG_IPG	GENMASK(15, 8)

#define PPE_GMAC_JUMBO_SIZE(gmac)	(PPE_MAC_CSR_BASE + (gmac) * 0x200 + 0x30)

#define PPE_GMAC_MIB_CTRL(gmac)	(PPE_MAC_CSR_BASE + (gmac) * 0x200 + 0x34)
#define   PPE_MIB_EN			BIT(0)
#define   PPE_MIB_RD_CLR		BIT(2)

#define PPE_GMAC_MIB(gmac, off)	(PPE_MAC_CSR_BASE + (gmac) * 0x200 + (off))
#define   PPE_MIB_RXBROAD		0x40
#define   PPE_MIB_RXPAUSE		0x44
#define   PPE_MIB_RXMULTI		0x48
#define   PPE_MIB_RXFCSERR		0x4c
#define   PPE_MIB_RXALIGNERR		0x50
#define   PPE_MIB_RXRUNT		0x54
#define   PPE_MIB_RXFRAG		0x58
#define   PPE_MIB_RXJUMBOFCSERR	0x5c
#define   PPE_MIB_RXJUMBOALIGNERR	0x60
#define   PPE_MIB_RXPKT64		0x64
#define   PPE_MIB_RXPKT65TO127		0x68
#define   PPE_MIB_RXPKT128TO255	0x6c
#define   PPE_MIB_RXPKT256TO511	0x70
#define   PPE_MIB_RXPKT512TO1023	0x74
#define   PPE_MIB_RXPKT1024TO1518	0x78
#define   PPE_MIB_RXPKT1519TOX		0x7c
#define   PPE_MIB_RXTOOLONG		0x80
#define   PPE_MIB_RXGOODBYTE_L		0x84
#define   PPE_MIB_RXGOODBYTE_H		0x88
#define   PPE_MIB_RXBADBYTE_L		0x8c
#define   PPE_MIB_RXBADBYTE_H		0x90
#define   PPE_MIB_RXUNI		0x94
#define   PPE_MIB_TXBROAD		0xa0
#define   PPE_MIB_TXPAUSE		0xa4
#define   PPE_MIB_TXMULTI		0xa8
#define   PPE_MIB_TXUNDERRUN		0xac
#define   PPE_MIB_TXPKT64		0xb0
#define   PPE_MIB_TXPKT65TO127		0xb4
#define   PPE_MIB_TXPKT128TO255	0xb8
#define   PPE_MIB_TXPKT256TO511	0xbc
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
#define   PPE_MIB_TXUNI		0xf0

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
#define   PPE_TDM_DEPTH		GENMASK(7, 0)
#define   PPE_TDM_EN			BIT(31)

#define PPE_PRX_TDM_CFG(i)		(PPE_PRX_BASE + 0x1000 + (i) * 0x10)
#define   PPE_TDM_PORT_NUM		GENMASK(3, 0)
#define   PPE_TDM_DIR			BIT(4)
#define   PPE_TDM_VALID		BIT(5)


/* --- Ingress VLAN (base 0x00f000) --- */
#define PPE_IVLAN_BASE			0x00f000

#define PPE_PORT_DEF_VID(port)		(PPE_IVLAN_BASE + 0x10 + (port) * 0x4)
#define   PPE_PORT_DEF_CVID		GENMASK(27, 16)
#define   PPE_PORT_DEF_CVID_EN		BIT(28)

#define PPE_PORT_VLAN_CFG(port)		(PPE_IVLAN_BASE + 0x50 + (port) * 0x4)
#define   PPE_VLAN_XLT_MISS_FWD	GENMASK(6, 5)

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
#define   PPE_XLT_CVID_CMD		GENMASK(16, 15)

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
#define   PPE_L2_LRN_EN		BIT(6)
#define   PPE_L2_AGE_EN		BIT(7)

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

#define PPE_PORT_BRIDGE_CTRL(port)	(PPE_L2_BASE + 0x300 + (port) * 0x4)
#define   PPE_BRIDGE_NEW_LRN_EN	BIT(0)
#define   PPE_BRIDGE_STA_MOVE_EN	BIT(3)
#define   PPE_BRIDGE_PORT_ISOL		GENMASK(15, 8)
#define   PPE_PORT_BRIDGE_CTRL_TXMAC_EN	BIT(16)

#define PPE_MC_MTU_CTRL(port)		(PPE_L2_BASE + 0xa00 + (port) * 0x4)
#define   PPE_MC_MTU_CTRL_MTU		GENMASK(13, 0)
#define   PPE_MC_MTU_CTRL_TX_CNT_EN	BIT(16)

#define PPE_RFDB_TBL(idx)		(PPE_L2_BASE + 0x1000 + (idx) * 0x8)

#define PPE_APP_CTRL(idx)		(PPE_L2_BASE + 0x1400 + (idx) * 0x10)

#define PPE_PORT_QOS_CTRL(p)		(PPE_L2_BASE + 0x900 + (p) * 0x10)
#define   PPE_QOS_DSCP_PREC		GENMASK(5, 3)
#define   PPE_QOS_PCP_PREC		GENMASK(8, 6)
#define   PPE_QOS_PREHEADER_PREC	GENMASK(11, 9)
#define   PPE_QOS_FLOW_PREC		GENMASK(14, 12)
#define   PPE_QOS_ACL_PREC		GENMASK(17, 15)

#define PPE_VSI_TBL(vsi)		(PPE_L2_BASE + 0x1800 + (vsi) * 0x10)
#define   PPE_VSI_TBL_MEMBER		GENMASK(7, 0)
#define   PPE_VSI_TBL_UUC		GENMASK(15, 8)
#define   PPE_VSI_TBL_UMC		GENMASK(23, 16)
#define   PPE_VSI_TBL_BC		GENMASK(31, 24)
#define   PPE_VSI_TBL_NEW_ADDR_LRN_EN	BIT(0)
#define   PPE_VSI_TBL_STA_MOVE_LRN_EN	BIT(3)

#define PPE_MRU_MTU_CTRL(port, stride)	(PPE_L2_BASE + 0x3000 + (port) * (stride))
#define   PPE_MRU_MTU_CTRL_MRU		GENMASK(13, 0)
#define   PPE_MRU_MTU_CTRL_MTU		GENMASK(29, 16)
#define   PPE_MRU_MTU_CTRL_RX_CNT_EN	BIT(0)
#define   PPE_MRU_MTU_CTRL_TX_CNT_EN	BIT(1)

/* --- L3 (base 0x200000) --- */
#define PPE_L3_BASE			0x200000

#define PPE_L3_VP_PORT_TBL(port)	(PPE_L3_BASE + 0x1000 + (port) * 0x10)
#define   PPE_L3_VP_VSI_VALID		BIT(9)
#define   PPE_L3_VP_VSI		GENMASK(14, 10)

#define PPE_MY_MAC_TBL(i)		(PPE_L3_BASE + (i) * 0x8)
#define   PPE_MY_MAC_ENTRIES		8
#define   PPE_MY_MAC_WORDS		2
#define   PPE_MY_MAC_ADDR_OFF		0
#define   PPE_MY_MAC_ADDR_LEN		48
#define   PPE_MY_MAC_VALID_OFF		48
#define   PPE_MY_MAC_VALID_LEN		1

#define PPE_L3_VSI_TBL(vsi)		(PPE_L3_BASE + 0x40 + (vsi) * 0x4)
#define   PPE_L3_VSI_IF_VALID		BIT(0)
#define   PPE_L3_VSI_IF_INDEX		GENMASK(8, 1)

#define PPE_FLOW_CTRL0			(PPE_L3_BASE + 0x368)
#define   PPE_FLOW_EN			BIT(0)
#define   PPE_FLOW_HASH_MODE0		GENMASK(2, 1)
#define   PPE_FLOW_HASH_MODE1		GENMASK(4, 3)
#define   PPE_FLOW_AGE_TIMER		GENMASK(20, 5)
#define   PPE_FLOW_AGE_TIMER_UNIT	GENMASK(22, 21)
#define   PPE_FLOW_CLK_FREQ_MHZ		GENMASK(31, 23)

/* One register per packet type, five six-bit direction groups per register. */
#define PPE_FLOW_CTRL1(type)		(PPE_L3_BASE + 0x36c + (type) * 0x4)
#define   PPE_FLOW_CTRL1_DIRS		5
#define   PPE_FLOW_CTRL1_DIR_BITS	6
#define   PPE_FLOW_MISS_ACTION		GENMASK(1, 0)
#define   PPE_FLOW_FRAG_BYPASS		BIT(2)
#define   PPE_FLOW_TCP_SPECIAL		BIT(3)
#define   PPE_FLOW_ALL_BYPASS		BIT(4)
#define   PPE_FLOW_KEY_SEL		BIT(5)

#define PPE_PUB_IP_TBL(i)		(PPE_L3_BASE + 0x378 + (i) * 0x4)
#define   PPE_PUB_IP_ENTRIES		16

#define PPE_FLOW_TBL_OP			(PPE_L3_BASE + 0x3b8)
#define   PPE_FLOW_OP_CMD_ID		GENMASK(3, 0)
#define   PPE_FLOW_OP_TYPE		GENMASK(7, 5)
#define   PPE_FLOW_OP_HASH_BLOCK	GENMASK(9, 8)
#define   PPE_FLOW_OP_INDEX_MODE	BIT(10)
#define   PPE_FLOW_OP_HOST_EN		BIT(11)
#define   PPE_FLOW_OP_ENTRY_IDX		GENMASK(23, 12)

#define PPE_FLOW_HOST_TBL_OP		(PPE_L3_BASE + 0x3bc)
#define   PPE_FLOW_HOST_OP_HASH_BLOCK	GENMASK(1, 0)
#define   PPE_FLOW_HOST_OP_ENTRY_IDX	GENMASK(14, 2)

#define PPE_FLOW_TBL_OP_DATA(i)		(PPE_L3_BASE + 0x3c0 + (i) * 0x4)
#define PPE_FLOW_HOST_TBL_OP_DATA(i)	(PPE_L3_BASE + 0x3e4 + (i) * 0x4)

/* Reading the result register consumes it: read once, keep the value. */
#define PPE_FLOW_TBL_OP_RSLT		(PPE_L3_BASE + 0x40c)
#define   PPE_FLOW_RSLT_CMD_ID		GENMASK(3, 0)
#define   PPE_FLOW_RSLT_FAIL		BIT(4)
#define   PPE_FLOW_RSLT_ENTRY_IDX	GENMASK(16, 5)
#define   PPE_FLOW_RSLT_VALID_CNT	GENMASK(20, 17)

#define PPE_FLOW_HOST_TBL_OP_RSLT	(PPE_L3_BASE + 0x410)
#define   PPE_FLOW_HOST_RSLT_ENTRY_IDX	GENMASK(12, 0)

#define PPE_FLOW_TBL_RD_OP		(PPE_L3_BASE + 0x414)
#define PPE_FLOW_HOST_TBL_RD_OP		(PPE_L3_BASE + 0x418)
#define PPE_FLOW_TBL_RD_OP_DATA(i)	(PPE_L3_BASE + 0x41c + (i) * 0x4)
#define PPE_FLOW_HOST_TBL_RD_OP_DATA(i)	(PPE_L3_BASE + 0x440 + (i) * 0x4)
#define PPE_FLOW_TBL_RD_OP_RSLT		(PPE_L3_BASE + 0x468)
#define PPE_FLOW_HOST_TBL_RD_OP_RSLT	(PPE_L3_BASE + 0x46c)
#define PPE_FLOW_TBL_RD_RSLT_DATA(i)	(PPE_L3_BASE + 0x470 + (i) * 0x4)

#define PPE_HOST_TBL_OP			(PPE_L3_BASE + 0x4bc)
#define   PPE_HOST_OP_CMD_ID		GENMASK(3, 0)
#define   PPE_HOST_OP_TYPE		GENMASK(7, 5)
#define   PPE_HOST_OP_HASH_BLOCK	GENMASK(9, 8)
#define   PPE_HOST_OP_INDEX_MODE	BIT(10)
#define   PPE_HOST_OP_ENTRY_IDX		GENMASK(23, 11)
#define PPE_HOST_TBL_OP_DATA(i)		(PPE_L3_BASE + 0x4c0 + (i) * 0x4)
#define PPE_HOST_TBL_OP_RSLT		(PPE_L3_BASE + 0x4e8)
#define   PPE_HOST_RSLT_CMD_ID		GENMASK(3, 0)
#define   PPE_HOST_RSLT_FAIL		BIT(4)
#define   PPE_HOST_RSLT_ENTRY_IDX	GENMASK(17, 5)
#define   PPE_HOST_RSLT_VALID_CNT	GENMASK(21, 18)

#define PPE_IN_L3_IF_TBL(i)		(PPE_L3_BASE + 0x2000 + (i) * 0x8)
#define   PPE_L3_IF_MRU			GENMASK(13, 0)
#define   PPE_L3_IF_MTU			GENMASK(27, 14)
#define   PPE_L3_IF_TTL_DEC_BYPASS	BIT(28)
#define   PPE_L3_IF_IPV4_ROUTE_EN	BIT(29)
#define   PPE_L3_IF_IPV6_ROUTE_EN	BIT(30)
#define   PPE_L3_IF_ICMP_TRIGGER_EN	BIT(31)
#define   PPE_L3_IF_TTL_EXCEED_CMD	GENMASK(1, 0)
#define   PPE_L3_IF_TTL_EXCEED_DEACCEL	BIT(2)
#define   PPE_L3_IF_MAC_BITMAP		GENMASK(10, 3)
#define   PPE_L3_IF_PPPOE_EN		BIT(11)

#define PPE_HOST_TBL(i)			(PPE_L3_BASE + 0x20000 + (i) * 0x10)
#define   PPE_HOST_E_VALID_OFF		0
#define   PPE_HOST_E_VALID_LEN		1
#define   PPE_HOST_E_KEY_TYPE_OFF	1
#define   PPE_HOST_E_KEY_TYPE_LEN	2
#define   PPE_HOST_E_IPV6_OFF		22
#define   PPE_HOST_E_IPV4_OFF		32
#define   PPE_HOST_E_IPV4_LEN		32
#define   PPE_HOST_KEY_IPV4		0
#define   PPE_HOST_KEY_IPV6		2
#define PPE_IN_FLOW_TBL(i)		(PPE_L3_BASE + 0x40000 + (i) * 0x20)
/* Flow entry fields, as bit offset and width within the entry's words. The
 * IPv4 and IPv6 forms agree up to the address: the ports sit at the same place
 * and only the address offset differs.
 */
#define   PPE_FLOW_E_VALID_OFF		0
#define   PPE_FLOW_E_VALID_LEN		1
#define   PPE_FLOW_E_TYPE_OFF		1
#define   PPE_FLOW_E_TYPE_LEN		1
#define   PPE_FLOW_E_HOST_IDX_OFF	3
#define   PPE_FLOW_E_HOST_IDX_LEN	13
#define   PPE_FLOW_E_PROTO_OFF		16
#define   PPE_FLOW_E_PROTO_LEN		2
#define   PPE_FLOW_E_AGE_OFF		18
#define   PPE_FLOW_E_AGE_LEN		2
#define   PPE_FLOW_E_AGE_MASK		GENMASK(19, 18)
#define   PPE_FLOW_E_FWD_TYPE_OFF	29
#define   PPE_FLOW_E_FWD_TYPE_LEN	3
#define   PPE_FLOW_E_NEXTHOP_OFF	32
#define   PPE_FLOW_E_NEXTHOP_LEN	12
#define   PPE_FLOW_E_NEW_PORT_OFF	44
#define   PPE_FLOW_E_NEW_PORT_LEN	16
#define   PPE_FLOW_E_IPV4_OFF		76
#define   PPE_FLOW_E_IPV4_LEN		32
#define   PPE_FLOW_E_SPORT_OFF		108
#define   PPE_FLOW_E_SPORT_LEN		16
#define   PPE_FLOW_E_DPORT_OFF		124
#define   PPE_FLOW_E_DPORT_LEN		16
#define   PPE_FLOW_E_IPV6_OFF		140

#define   PPE_FLOW_E_TYPE_IPV6		BIT(1)
#define PPE_IN_NEXTHOP_TBL(i)		(PPE_L3_BASE + 0x60000 + (i) * 0x10)
#define   PPE_NEXTHOP_WORDS		4
#define   PPE_NEXTHOP_PORT_OFF		1
#define   PPE_NEXTHOP_PORT_LEN		8
#define   PPE_NEXTHOP_POST_L3_IF_OFF	9
#define   PPE_NEXTHOP_POST_L3_IF_LEN	8
#define   PPE_NEXTHOP_CTAG_FMT_OFF	31
#define   PPE_NEXTHOP_CTAG_FMT_LEN	1
#define   PPE_NEXTHOP_CVID_OFF		32
#define   PPE_NEXTHOP_CVID_LEN		12
#define   PPE_NEXTHOP_PUB_IP_IDX_OFF	44
#define   PPE_NEXTHOP_PUB_IP_IDX_LEN	4
#define   PPE_NEXTHOP_MAC_OFF		48
#define   PPE_NEXTHOP_MAC_LEN		48
#define   PPE_NEXTHOP_DNAT_IP_OFF	96
#define   PPE_NEXTHOP_DNAT_IP_LEN	32

/* Egress L3 interface: source MAC and PPPoE session, in the PTX block. */
#define PPE_EG_L3_IF_TBL(i)		(PPE_PTX_BASE + 0xe000 + (i) * 0x10)
#define   PPE_EG_L3_IF_ENTRIES		256
#define   PPE_EG_L3_IF_WORDS		3
#define   PPE_EG_L3_IF_MAC_OFF		0
#define   PPE_EG_L3_IF_MAC_LEN		48
#define   PPE_EG_L3_IF_SESSION_OFF	48
#define   PPE_EG_L3_IF_SESSION_LEN	16
#define   PPE_EG_L3_IF_PPPOE_EN_OFF	64
#define   PPE_EG_L3_IF_PPPOE_EN_LEN	1

/* --- Ingress policer (base 0x100000) --- */
#define PPE_POLICER_BASE		0x100000

#define PPE_IN_FLOW_CNT_TBL(i)		(PPE_POLICER_BASE + 0x20000 + (i) * 0x10)
#define   PPE_FLOW_CNT_WORDS		3
#define   PPE_FLOW_CNT_BYTES_HI		GENMASK(7, 0)
#define PPE_RT_IF_CNT_TBL(i)		(PPE_POLICER_BASE + 0x40000 + (i) * 0x20)

/* --- Traffic Manager (base 0x400000) --- */
#define PPE_TM_BASE			0x400000

#define PPE_TM_TDM_DEPTH		(PPE_TM_BASE + 0x0)
#define   PPE_TM_TDM_DEPTH_MASK	GENMASK(7, 0)

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
#define   PPE_PSCH_ENS_PORT_BMP	GENMASK(15, 8)

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
#define   PPE_BM_WEIGHT		GENMASK(10, 8)
#define   PPE_BM_DYNAMIC		BIT(11)
#define   PPE_BM_PREALLOC		GENMASK(22, 12)

/* --- Queue Manager (base 0x800000) --- */
#define PPE_QM_BASE			0x800000

#define PPE_QM_UCAST_MAP(i)		(PPE_QM_BASE + 0x10000 + (i) * 0x10)
#define   PPE_QM_PROFILE_ID		GENMASK(3, 0)
#define   PPE_QM_QUEUE_ID		GENMASK(11, 4)

#define PPE_QM_UCAST_HASH_MAP(i)	(PPE_QM_BASE + 0x30000 + (i) * 0x10)
#define   PPE_QM_HASH_CLASS		GENMASK(3, 0)

#define PPE_QM_UCAST_PRI_MAP(i)	(PPE_QM_BASE + 0x42000 + (i) * 0x10)
#define   PPE_QM_PRI_CLASS		GENMASK(3, 0)

#define PPE_QM_AC_UNI_W0(i)		(PPE_QM_BASE + 0x48000 + (i) * 0x10)
#define PPE_QM_AC_UNI_W1(i)		(PPE_QM_BASE + 0x48000 + (i) * 0x10 + 0x4)
#define PPE_QM_AC_UNI_W2(i)		(PPE_QM_BASE + 0x48000 + (i) * 0x10 + 0x8)
#define PPE_QM_AC_UNI_W3(i)		(PPE_QM_BASE + 0x48000 + (i) * 0x10 + 0xc)
#define   PPE_AC_EN			BIT(0)
#define   PPE_AC_GRP_ID		GENMASK(5, 4)
#define   PPE_AC_SHARED_DYNAMIC	BIT(17)
#define   PPE_AC_SHARED_WEIGHT		GENMASK(20, 18)
#define   PPE_AC_SHARED_CEILING	GENMASK(31, 21)
#define   PPE_AC_GRN_RESUME_OFF	GENMASK(23, 13)

#define PPE_QM_AC_MUL_W0(i)		(PPE_QM_BASE + 0x4a000 + (i) * 0x10)
#define PPE_QM_AC_MUL_W1(i)		(PPE_QM_BASE + 0x4a000 + (i) * 0x10 + 0x4)
#define PPE_QM_AC_MUL_W2(i)		(PPE_QM_BASE + 0x4a000 + (i) * 0x10 + 0x8)
#define   PPE_AC_MUL_EN		BIT(0)
#define   PPE_AC_MUL_CEILING		GENMASK(26, 16)
#define   PPE_AC_MUL_GRN_MAX_LO	GENMASK(31, 27)
#define   PPE_AC_MUL_GRN_MAX_HI	GENMASK(5, 0)
#define   PPE_AC_MUL_GRN_RESUME_HI	GENMASK(17, 11)

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
#define PPE_FDB_DATA1_LKP_VALID	BIT(17)
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
#define PPE_XLT_MISS_FWD_DROP		3
#define PPE_XLT_CVID_DEL		2
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
	u16 num_flow_entries;
	u16 num_host_entries;
	u16 num_nexthop_entries;
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

struct qca_ppe_priv {
	struct dsa_switch ds;
	struct regmap *regmap;
	const struct ppe_data *data;
	struct clk_bulk_data *clks;
	int num_clks;
	spinlock_t fdb_lock;
	struct mutex flow_lock;
	u32 flow_cmd_id;
	struct dentry *debugfs;
	DECLARE_BITMAP(vsi_bitmap, PPE_VSI_MAX);
	DECLARE_BITMAP(xlt_bitmap, PPE_XLT_TBL_NUM);
	u32 port_vsi[QCA_PPE_MAX_PORTS];
	struct qca_ppe_bridge_vsi bridges[QCA_PPE_MAX_BRIDGES];
	struct qca_ppe_vlan_entry vlans[PPE_VSI_MAX];
	struct net_device *port_br_dev[QCA_PPE_MAX_PORTS];
	u16 port_pvid[QCA_PPE_MAX_PORTS];
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

/* Set or read a field of a multi-word hardware table entry. Fields of the flow
 * entry straddle 32-bit boundaries, so neither can be expressed with FIELD_PREP.
 */
static inline void ppe_entry_set(u32 *words, u32 offset, u32 len, u64 val)
{
	u32 word = offset / 32;
	u32 bit = offset % 32;

	val &= (len >= 64) ? U64_MAX : (1ULL << len) - 1;

	words[word] |= (u32)(val << bit);
	if (bit + len > 32)
		words[word + 1] |= (u32)(val >> (32 - bit));
	if (bit + len > 64)
		words[word + 2] |= (u32)(val >> (64 - bit));
}

static inline u64 ppe_entry_get(const u32 *words, u32 offset, u32 len)
{
	u32 word = offset / 32;
	u32 bit = offset % 32;
	u64 val = words[word] >> bit;

	if (bit + len > 32)
		val |= (u64)words[word + 1] << (32 - bit);

	return len >= 64 ? val : val & ((1ULL << len) - 1);
}

static inline struct qca_ppe_priv *ds_to_priv(struct dsa_switch *ds)
{
	return container_of(ds, struct qca_ppe_priv, ds);
}

void ppe_scheduler_init(struct qca_ppe_priv *priv);

int ppe_vsi_alloc(struct qca_ppe_priv *priv);
void ppe_vsi_free(struct qca_ppe_priv *priv, u32 vsi);
void ppe_vsi_member_set(struct qca_ppe_priv *priv, u32 vsi, u32 portmask);

int qca_ppe_vlan_setup(struct dsa_switch *ds);
int qca_ppe_port_vlan_filtering(struct dsa_switch *ds, int port,
				bool vlan_filtering,
				struct netlink_ext_ack *extack);
int qca_ppe_port_vlan_add(struct dsa_switch *ds, int port,
			  const struct switchdev_obj_port_vlan *vlan,
			  struct netlink_ext_ack *extack);
int qca_ppe_port_vlan_del(struct dsa_switch *ds, int port,
			  const struct switchdev_obj_port_vlan *vlan);

void ppe_flow_init(struct qca_ppe_priv *priv);
int ppe_flow_op(struct qca_ppe_priv *priv, u32 op_type,
		const u32 *entry, int nentry, const u32 *host, int nhost,
		u32 *index, u32 *host_index);
int ppe_host_del(struct qca_ppe_priv *priv, u32 index);
int ppe_flow_entry_read(struct qca_ppe_priv *priv, u32 index, u32 *words,
			int nwords);
void ppe_flow_counter_read(struct qca_ppe_priv *priv, u32 index, u64 *packets,
			   u64 *bytes);
void ppe_flow_counter_clear(struct qca_ppe_priv *priv, u32 index);
int ppe_flow_entry_delete(struct qca_ppe_priv *priv, u32 index);
void ppe_flow_debugfs_init(struct qca_ppe_priv *priv);
void ppe_flow_debugfs_exit(struct qca_ppe_priv *priv);


#endif

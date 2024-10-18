/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (C) 2023 Min Yao <min.yao@airoha.com>
 */

#ifndef __AN8855_H
#define __AN8855_H

#define BITS(m, n)	 (~(BIT(m) - 1) & ((BIT(n) - 1) | BIT(n)))

#define AN8855_NUM_PORTS				6
#define AN8855_CPU_PORT					5
#define AN8855_WORD_SIZE				4
#define AN8855_NUM_FDB_RECORDS			2048
#define AN8855_ALL_MEMBERS				0x3f
#define AN8855_RESERVED_VLAN			2
#define AN8855_GPHY_SMI_ADDR_DEFAULT	1
#define AN8855_DFL_INTR_ID				0xd
#define AN8855_DFL_EXT_SURGE			0x0

enum an8855_id {
	ID_AN8855 = 0,
};

enum sgmii_mode {
	SGMII_MODE_AN,
	SGMII_MODE_FORCE,
};

/* Registers to mac forward control for unknown frames */
#define AN8855_MFC			0x10200010
#define	 CPU_EN				BIT(15)
#define	 CPU_PORT(x)		((x) << 8)
#define	 CPU_MASK			(0x9f << 8)

#define AN8855_UNUF			0x102000b4
#define AN8855_UNMF			0x102000b8
#define AN8855_BCF			0x102000bc

/* Registers for mirror port control */
#define	AN8855_MIR						  0x102000cc
#define	 AN8855_MIRROR_EN				  BIT(7)
#define	 AN8855_MIRROR_MASK				  (0x1f)
#define	 AN8855_MIRROR_PORT_GET(x)		  ((x) & AN8855_MIRROR_MASK)
#define	 AN8855_MIRROR_PORT_SET(x)		  ((x) & AN8855_MIRROR_MASK)

/* Registers for BPDU and PAE frame control*/
#define AN8855_BPC					0x102000D0
#define	AN8855_BPDU_PORT_FW_MASK	GENMASK(2, 0)

enum an8855_bpdu_port_fw {
	AN8855_BPDU_FOLLOW_MFC,
	AN8855_BPDU_CPU_EXCLUDE = 4,
	AN8855_BPDU_CPU_INCLUDE = 5,
	AN8855_BPDU_CPU_ONLY = 6,
	AN8855_BPDU_DROP = 7,
};

/* Registers for address table access */
#define AN8855_ATA1			0x10200304
#define AN8855_ATA2			0x10200308

/* Register for address table write data */
#define AN8855_ATWD			0x10200324
#define AN8855_ATWD2		0x10200328

/* Register for address table control */
#define AN8855_ATC			0x10200300
#define	 ATC_BUSY			BIT(31)
#define	 ATC_INVALID		~BIT(30)
#define	 ATC_HASH			16
#define	 ATC_HASH_MASK		0x1ff
#define	 ATC_HIT			12
#define	 ATC_HIT_MASK		0xf
#define	 ATC_MAT(x)			(((x) & 0x1f) << 7)
#define	 ATC_MAT_MACTAB		ATC_MAT(1)

enum an8855_fdb_cmds {
	AN8855_FDB_READ = 0,
	AN8855_FDB_WRITE = 1,
	AN8855_FDB_FLUSH = 2,
	AN8855_FDB_START = 4,
	AN8855_FDB_NEXT = 5,
};

/* Registers for table search read address */
#define AN8855_ATRDS		0x10200330
#define AN8855_ATRD0		0x10200334
#define	 CVID				10
#define	 CVID_MASK			0xfff

enum an8855_fdb_type {
	AN8855_MAC_TB_TY_MAC = 0,
	AN8855_MAC_TB_TY_DIP = 1,
	AN8855_MAC_TB_TY_DIP_SIP = 2,
};

#define AN8855_ATRD1		0x10200338
#define	 MAC_BYTE_4			24
#define	 MAC_BYTE_5			16
#define	 AGE_TIMER			3
#define	 AGE_TIMER_MASK		0x1ff

#define AN8855_ATRD2		0x1020033c
#define	 MAC_BYTE_0			24
#define	 MAC_BYTE_1			16
#define	 MAC_BYTE_2			8
#define	 MAC_BYTE_3			0
#define	 MAC_BYTE_MASK		0xff

#define AN8855_ATRD3		0x10200340
#define	 PORT_MAP			4
#define	 PORT_MAP_MASK		0xff

/* Register for vlan table control */
#define AN8855_VTCR			0x10200600
#define	 VTCR_BUSY			BIT(31)
#define	 VTCR_FUNC(x)		(((x) & 0xf) << 12)
#define	 VTCR_VID			((x) & 0xfff)

enum an8855_vlan_cmd {
	/* Read/Write the specified VID entry from VAWD register based
	 * on VID.
	 */
	AN8855_VTCR_RD_VID = 0,
	AN8855_VTCR_WR_VID = 1,
};

/* Register for setup vlan write data */
#define AN8855_VAWD0			0x10200604

/* Independent VLAN Learning */
#define	 IVL_MAC			BIT(5)
/* Per VLAN Egress Tag Control */
#define	 VTAG_EN			BIT(10)
/* Egress Tag Control */
#define PORT_EG_CTRL_SHIFT	12
/* VLAN Member Control */
#define	 PORT_MEM_SHFT		26
#define	 PORT_MEM_MASK		0x7f
#define	 PORT_MEM(x)		(((x) & PORT_MEM_MASK) << PORT_MEM_SHFT)
/* VLAN Entry Valid */
#define	 VLAN_VALID			BIT(0)

#define AN8855_VAWD1			0x10200608
#define	 PORT_STAG			BIT(1)
/* Egress Tag Control */
#define	 ETAG_CTRL_P(p, x)		(((x) & 0x3) << ((p) << 1))
#define	 ETAG_CTRL_P_MASK(p)	ETAG_CTRL_P(p, 3)
#define	 ETAG_CTRL_MASK			(0x3FFF)

#define AN8855_VARD0			0x10200618

enum an8855_vlan_egress_attr {
	AN8855_VLAN_EGRESS_UNTAG = 0,
	AN8855_VLAN_EGRESS_TAG = 2,
	AN8855_VLAN_EGRESS_STACK = 3,
};

/* Register for port STP state control */
#define AN8855_SSP_P(x)		(0x10208000 + ((x) * 0x200))
#define	 FID_PST(x)			((x) & 0x3)
#define	 FID_PST_MASK		FID_PST(0x3)

enum an8855_stp_state {
	AN8855_STP_DISABLED = 0,
	AN8855_STP_BLOCKING = 1,
	AN8855_STP_LISTENING = 1,
	AN8855_STP_LEARNING = 2,
	AN8855_STP_FORWARDING = 3
};

/* Register for port control */
#define AN8855_PCR_P(x)		(0x10208004 + ((x) * 0x200))
#define	 PORT_TX_MIR		BIT(20)
#define	 PORT_RX_MIR		BIT(16)
#define	 PORT_VLAN(x)		((x) & 0x3)

enum an8855_port_mode {
	/* Port Matrix Mode: Frames are forwarded by the PCR_MATRIX members. */
	AN8855_PORT_MATRIX_MODE = PORT_VLAN(0),

	/* Fallback Mode: Forward received frames with ingress ports that do
	 * not belong to the VLAN member. Frames whose VID is not listed on
	 * the VLAN table are forwarded by the PCR_MATRIX members.
	 */
	AN8855_PORT_FALLBACK_MODE = PORT_VLAN(1),

	/* Security Mode: Discard any frame due to ingress membership
	 * violation or VID missed on the VLAN table.
	 */
	AN8855_PORT_SECURITY_MODE = PORT_VLAN(3),
};

#define	 PORT_PRI(x)		(((x) & 0x7) << 24)
#define	 EG_TAG(x)			(((x) & 0x3) << 28)
#define	 PCR_PORT_VLAN_MASK	PORT_VLAN(3)

/* Register for port security control */
#define AN8855_PSC_P(x)		(0x1020800c + ((x) * 0x200))
#define	 SA_DIS				BIT(4)

/* Register for port vlan control */
#define AN8855_PVC_P(x)			(0x10208010 + ((x) * 0x200))
#define	 PORT_SPEC_REPLACE_MODE	BIT(11)
#define	 PORT_SPEC_TAG			BIT(5)
#define	 PVC_EG_TAG(x)			(((x) & 0x7) << 8)
#define	 PVC_EG_TAG_MASK		PVC_EG_TAG(7)
#define	 VLAN_ATTR(x)			(((x) & 0x3) << 6)
#define	 VLAN_ATTR_MASK			VLAN_ATTR(3)

#define AN8855_PORTMATRIX_P(x)	(0x10208044 + ((x) * 0x200))
#define PORTMATRIX_MATRIX(x)	((x) & 0x3f)
#define PORTMATRIX_MASK			PORTMATRIX_MATRIX(0x3f)
#define PORTMATRIX_CLR			PORTMATRIX_MATRIX(0)

enum an8855_vlan_port_eg_tag {
	AN8855_VLAN_EG_DISABLED = 0,
	AN8855_VLAN_EG_CONSISTENT = 1,
};

enum an8855_vlan_port_attr {
	AN8855_VLAN_USER = 0,
	AN8855_VLAN_TRANSPARENT = 3,
};

/* Register for port PVID */
#define AN8855_PVID_P(x)		(0x10208048 + ((x) * 0x200))
#define	 G0_PORT_VID(x)			(((x) & 0xfff) << 0)
#define	 G0_PORT_VID_MASK		G0_PORT_VID(0xfff)
#define	 G0_PORT_VID_DEF		G0_PORT_VID(1)

/* Register for port MAC control register */
#define AN8855_PMCR_P(x)		(0x10210000 + ((x) * 0x200))
#define	 PMCR_IFG_XMIT(x)		(((x) & 0x3) << 20)
#define	 PMCR_EXT_PHY			BIT(19)
#define	 PMCR_MAC_MODE			BIT(18)
#define	 PMCR_FORCE_MODE		BIT(31)
#define	 PMCR_TX_EN				BIT(16)
#define	 PMCR_RX_EN				BIT(15)
#define	 PMCR_BACKOFF_EN		BIT(12)
#define	 PMCR_BACKPR_EN			BIT(11)
#define	 PMCR_FORCE_EEE2P5G		BIT(8)
#define	 PMCR_FORCE_EEE1G		BIT(7)
#define	 PMCR_FORCE_EEE100		BIT(6)
#define	 PMCR_TX_FC_EN			BIT(5)
#define	 PMCR_RX_FC_EN			BIT(4)
#define	 PMCR_FORCE_SPEED_2500	(0x3 << 28)
#define	 PMCR_FORCE_SPEED_1000	(0x2 << 28)
#define	 PMCR_FORCE_SPEED_100	(0x1 << 28)
#define	 PMCR_FORCE_FDX			BIT(25)
#define	 PMCR_FORCE_LNK			BIT(24)
#define	 PMCR_SPEED_MASK		BITS(28, 30)
#define	 AN8855_FORCE_LNK		BIT(31)
#define	 AN8855_FORCE_MODE		(AN8855_FORCE_LNK)
#define	 PMCR_LINK_SETTINGS_MASK	(PMCR_TX_EN | \
					 PMCR_RX_EN | PMCR_FORCE_SPEED_2500 | \
					 PMCR_TX_FC_EN | PMCR_RX_FC_EN | \
					 PMCR_FORCE_FDX | PMCR_FORCE_LNK)
#define	 PMCR_CPU_PORT_SETTING(id)	(AN8855_FORCE_MODE | \
					 PMCR_IFG_XMIT(1) | PMCR_MAC_MODE | \
					 PMCR_BACKOFF_EN | PMCR_BACKPR_EN | \
					 PMCR_TX_EN | PMCR_RX_EN | \
					 PMCR_TX_FC_EN | PMCR_RX_FC_EN | \
					 PMCR_FORCE_SPEED_2500 | \
					 PMCR_FORCE_FDX | PMCR_FORCE_LNK)

#define AN8855_PMSR_P(x)		(0x10210010 + (x) * 0x200)
#define	 PMSR_EEE1G				BIT(7)
#define	 PMSR_EEE100M			BIT(6)
#define	 PMSR_RX_FC				BIT(5)
#define	 PMSR_TX_FC				BIT(4)
#define	 PMSR_SPEED_2500		(0x3 << 28)
#define	 PMSR_SPEED_1000		(0x2 << 28)
#define	 PMSR_SPEED_100			(0x1 << 28)
#define	 PMSR_SPEED_10			(0x0 << 28)
#define	 PMSR_SPEED_MASK		BITS(28, 30)
#define	 PMSR_DPX				BIT(25)
#define	 PMSR_LINK				BIT(24)

#define AN8855_PMEEECR_P(x)		(0x10210004 + (x) * 0x200)
#define	 WAKEUP_TIME_2500(x)	((x & 0xFF) << 16)
#define	 WAKEUP_TIME_1000(x)	((x & 0xFF) << 8)
#define	 WAKEUP_TIME_100(x)		((x & 0xFF) << 0)
#define	 LPI_MODE_EN			BIT(31)
#define AN8855_PMEEECR2_P(x)	(0x10210008 + (x) * 0x200)
#define	 WAKEUP_TIME_5000(x)	((x & 0xFF) << 0)

#define AN8855_CKGCR			(0x10213e1c)
#define LPI_TXIDLE_THD			14
#define LPI_TXIDLE_THD_MASK		BITS(14, 31)
#define CKG_LNKDN_GLB_STOP	0x01
#define CKG_LNKDN_PORT_STOP	0x02

/* Register for MIB */
#define AN8855_PORT_MIB_COUNTER(x)	(0x10214000 + (x) * 0x200)
#define AN8855_MIB_CCR			0x10213e30
#define	 CCR_MIB_ENABLE			BIT(31)
#define	 CCR_RX_OCT_CNT_GOOD	BIT(7)
#define	 CCR_RX_OCT_CNT_BAD		BIT(6)
#define	 CCR_TX_OCT_CNT_GOOD	BIT(5)
#define	 CCR_TX_OCT_CNT_BAD		BIT(4)
#define	 CCR_RX_OCT_CNT_GOOD_2	BIT(3)
#define	 CCR_RX_OCT_CNT_BAD_2	BIT(2)
#define	 CCR_TX_OCT_CNT_GOOD_2	BIT(1)
#define	 CCR_TX_OCT_CNT_BAD_2	BIT(0)
#define	 CCR_MIB_FLUSH			(CCR_RX_OCT_CNT_GOOD | \
					 CCR_RX_OCT_CNT_BAD | \
					 CCR_TX_OCT_CNT_GOOD | \
					 CCR_TX_OCT_CNT_BAD | \
					 CCR_RX_OCT_CNT_BAD_2 | \
					 CCR_TX_OCT_CNT_BAD_2)
#define	 CCR_MIB_ACTIVATE		(CCR_MIB_ENABLE | \
					 CCR_RX_OCT_CNT_GOOD | \
					 CCR_RX_OCT_CNT_BAD | \
					 CCR_TX_OCT_CNT_GOOD | \
					 CCR_TX_OCT_CNT_BAD | \
					 CCR_RX_OCT_CNT_BAD_2 | \
					 CCR_TX_OCT_CNT_BAD_2)

/* AN8855 SGMII register group */
#define AN8855_SGMII_REG_BASE		0x10220000
#define AN8855_SGMII_REG(p, r)		(AN8855_SGMII_REG_BASE + \
					((p) - 5) * 0x1000 + (r))

/* Register forSGMII PCS_CONTROL_1 */
#define AN8855_PCS_CONTROL_1(p)		AN8855_SGMII_REG(p, 0x00)
#define	 AN8855_SGMII_AN_ENABLE		BIT(12)
#define	 AN8855_SGMII_AN_RESTART	BIT(9)

/* Register for system reset */
#define AN8855_RST_CTRL			0x100050c0
#define	 SYS_CTRL_SYS_RST		BIT(31)

#define INT_MASK			0x100050F0
#define INT_SYS_BIT			BIT(15)

#define RG_CLK_CPU_ICG		0x10005034
#define MCU_ENABLE			BIT(3)

#define RG_TIMER_CTL		0x1000a100
#define WDOG_ENABLE			BIT(25)

#define CKGCR				0x10213E1C
#define CKG_LNKDN_GLB_STOP	0x01
#define CKG_LNKDN_PORT_STOP	0x02

#define PKG_SEL				0x10000094
#define PAG_SEL_AN8855H		0x2

/* Register for hw trap status */
#define AN8855_HWTRAP			0x1000009c

#define AN8855_CREV				0x10005000
#define	 AN8855_ID				0x8855

#define SCU_BASE				0x10000000
#define RG_RGMII_TXCK_C			(SCU_BASE + 0x1d0)
#define RG_GPIO_LED_MODE		(SCU_BASE + 0x0054)
#define RG_GPIO_LED_SEL(i)	(SCU_BASE + (0x0058 + ((i) * 4)))
#define RG_INTB_MODE			(SCU_BASE + 0x0080)
#define RG_GDMP_RAM				(SCU_BASE + 0x10000)

#define RG_GPIO_L_INV			(SCU_BASE + 0x0010)
#define RG_GPIO_CTRL			(SCU_BASE + 0xa300)
#define RG_GPIO_DATA			(SCU_BASE + 0xa304)
#define RG_GPIO_OE			(SCU_BASE + 0xa314)

#define HSGMII_AN_CSR_BASE		0x10220000
#define SGMII_REG_AN0			(HSGMII_AN_CSR_BASE + 0x000)
#define SGMII_REG_AN_13			(HSGMII_AN_CSR_BASE + 0x034)
#define SGMII_REG_AN_FORCE_CL37	(HSGMII_AN_CSR_BASE + 0x060)

#define HSGMII_CSR_PCS_BASE		0x10220000
#define RG_HSGMII_PCS_CTROL_1	(HSGMII_CSR_PCS_BASE + 0xa00)
#define RG_AN_SGMII_MODE_FORCE	(HSGMII_CSR_PCS_BASE + 0xa24)

#define MULTI_SGMII_CSR_BASE	0x10224000
#define SGMII_STS_CTRL_0		(MULTI_SGMII_CSR_BASE + 0x018)
#define MSG_RX_CTRL_0			(MULTI_SGMII_CSR_BASE + 0x100)
#define MSG_RX_LIK_STS_0		(MULTI_SGMII_CSR_BASE + 0x514)
#define MSG_RX_LIK_STS_2		(MULTI_SGMII_CSR_BASE + 0x51c)
#define PHY_RX_FORCE_CTRL_0		(MULTI_SGMII_CSR_BASE + 0x520)

#define XFI_CSR_PCS_BASE		0x10225000
#define RG_USXGMII_AN_CONTROL_0	(XFI_CSR_PCS_BASE + 0xbf8)

#define MULTI_PHY_RA_CSR_BASE	0x10226000
#define RG_RATE_ADAPT_CTRL_0	(MULTI_PHY_RA_CSR_BASE + 0x000)
#define RATE_ADP_P0_CTRL_0		(MULTI_PHY_RA_CSR_BASE + 0x100)
#define MII_RA_AN_ENABLE		(MULTI_PHY_RA_CSR_BASE + 0x300)

#define QP_DIG_CSR_BASE			0x1022a000
#define QP_CK_RST_CTRL_4		(QP_DIG_CSR_BASE + 0x310)
#define QP_DIG_MODE_CTRL_0		(QP_DIG_CSR_BASE + 0x324)
#define QP_DIG_MODE_CTRL_1		(QP_DIG_CSR_BASE + 0x330)

#define SERDES_WRAPPER_BASE		0x1022c000
#define USGMII_CTRL_0			(SERDES_WRAPPER_BASE + 0x000)

#define QP_PMA_TOP_BASE			0x1022e000
#define PON_RXFEDIG_CTRL_0		(QP_PMA_TOP_BASE + 0x100)
#define PON_RXFEDIG_CTRL_9		(QP_PMA_TOP_BASE + 0x124)

#define SS_LCPLL_PWCTL_SETTING_2	(QP_PMA_TOP_BASE + 0x208)
#define SS_LCPLL_TDC_FLT_2			(QP_PMA_TOP_BASE + 0x230)
#define SS_LCPLL_TDC_FLT_5			(QP_PMA_TOP_BASE + 0x23c)
#define SS_LCPLL_TDC_PCW_1			(QP_PMA_TOP_BASE + 0x248)
#define INTF_CTRL_8			(QP_PMA_TOP_BASE + 0x320)
#define INTF_CTRL_9			(QP_PMA_TOP_BASE + 0x324)
#define INTF_CTRL_10		(QP_PMA_TOP_BASE + 0x328)
#define INTF_CTRL_11		(QP_PMA_TOP_BASE + 0x32c)
#define PLL_CTRL_0			(QP_PMA_TOP_BASE + 0x400)
#define PLL_CTRL_2			(QP_PMA_TOP_BASE + 0x408)
#define PLL_CTRL_3			(QP_PMA_TOP_BASE + 0x40c)
#define PLL_CTRL_4			(QP_PMA_TOP_BASE + 0x410)
#define PLL_CK_CTRL_0		(QP_PMA_TOP_BASE + 0x414)
#define RX_DLY_0			(QP_PMA_TOP_BASE + 0x614)
#define RX_CTRL_2			(QP_PMA_TOP_BASE + 0x630)
#define RX_CTRL_5			(QP_PMA_TOP_BASE + 0x63c)
#define RX_CTRL_6			(QP_PMA_TOP_BASE + 0x640)
#define RX_CTRL_7			(QP_PMA_TOP_BASE + 0x644)
#define RX_CTRL_8			(QP_PMA_TOP_BASE + 0x648)
#define RX_CTRL_26			(QP_PMA_TOP_BASE + 0x690)
#define RX_CTRL_42			(QP_PMA_TOP_BASE + 0x6d0)

#define QP_ANA_CSR_BASE				0x1022f000
#define RG_QP_RX_DAC_EN				(QP_ANA_CSR_BASE + 0x00)
#define RG_QP_RXAFE_RESERVE			(QP_ANA_CSR_BASE + 0x04)
#define RG_QP_CDR_LPF_BOT_LIM		(QP_ANA_CSR_BASE + 0x08)
#define RG_QP_CDR_LPF_MJV_LIM		(QP_ANA_CSR_BASE + 0x0c)
#define RG_QP_CDR_LPF_SETVALUE		(QP_ANA_CSR_BASE + 0x14)
#define RG_QP_CDR_PR_CKREF_DIV1		(QP_ANA_CSR_BASE + 0x18)
#define RG_QP_CDR_PR_KBAND_DIV_PCIE	(QP_ANA_CSR_BASE + 0x1c)
#define RG_QP_CDR_FORCE_IBANDLPF_R_OFF	(QP_ANA_CSR_BASE + 0x20)
#define RG_QP_TX_MODE_16B_EN		(QP_ANA_CSR_BASE + 0x28)
#define RG_QP_PLL_IPLL_DIG_PWR_SEL	(QP_ANA_CSR_BASE + 0x3c)
#define RG_QP_PLL_SDM_ORD			(QP_ANA_CSR_BASE + 0x40)

#define ETHER_SYS_BASE				0x1028c800
#define RG_GPHY_AFE_PWD				(ETHER_SYS_BASE + 0x40)
#define RG_GPHY_SMI_ADDR			(ETHER_SYS_BASE + 0x48)

#define MIB_DESC(_s, _o, _n)	\
	{			\
		.size = (_s),	\
		.offset = (_o),	\
		.name = (_n),	\
	}

struct an8855_mib_desc {
	unsigned int size;
	unsigned int offset;
	const char *name;
};

struct an8855_fdb {
	u16 vid;
	u8 port_mask;
	u8 aging;
	u8 mac[6];
	bool noarp;
	u8 live;
	u8 type;
	u8 fid;
	u8 ivl;
};

/* Definition of LED */
#define LED_ON_EVENT	(LED_ON_EVT_LINK_1000M | \
			LED_ON_EVT_LINK_100M | LED_ON_EVT_LINK_10M |\
			LED_ON_EVT_LINK_HD | LED_ON_EVT_LINK_FD)

#define LED_BLK_EVENT	(LED_BLK_EVT_1000M_TX_ACT | \
			LED_BLK_EVT_1000M_RX_ACT | \
			LED_BLK_EVT_100M_TX_ACT | \
			LED_BLK_EVT_100M_RX_ACT | \
			LED_BLK_EVT_10M_TX_ACT | \
			LED_BLK_EVT_10M_RX_ACT)

#define LED_FREQ	AIR_LED_BLK_DUR_64M

enum phy_led_idx {
	P0_LED0,
	P0_LED1,
	P0_LED2,
	P0_LED3,
	P1_LED0,
	P1_LED1,
	P1_LED2,
	P1_LED3,
	P2_LED0,
	P2_LED1,
	P2_LED2,
	P2_LED3,
	P3_LED0,
	P3_LED1,
	P3_LED2,
	P3_LED3,
	P4_LED0,
	P4_LED1,
	P4_LED2,
	P4_LED3,
	PHY_LED_MAX
};

/* TBD */
enum an8855_led_blk_dur {
	AIR_LED_BLK_DUR_32M,
	AIR_LED_BLK_DUR_64M,
	AIR_LED_BLK_DUR_128M,
	AIR_LED_BLK_DUR_256M,
	AIR_LED_BLK_DUR_512M,
	AIR_LED_BLK_DUR_1024M,
	AIR_LED_BLK_DUR_LAST
};

enum an8855_led_polarity {
	LED_LOW,
	LED_HIGH,
};
enum an8855_led_mode {
	AN8855_LED_MODE_DISABLE,
	AN8855_LED_MODE_USER_DEFINE,
	AN8855_LED_MODE_LAST
};

struct an8855_led_cfg {
	u16 en;
	u8  phy_led_idx;
	u16 pol;
	u16 on_cfg;
	u16 blk_cfg;
	u8 led_freq;
};

/* struct an8855_port -	This is the main data structure for holding the state
 *			of the port.
 * @enable:	The status used for show port is enabled or not.
 * @pm:		The matrix used to show all connections with the port.
 * @pvid:	The VLAN specified is to be considered a PVID at ingress.  Any
 *		untagged frames will be assigned to the related VLAN.
 * @vlan_filtering: The flags indicating whether the port that can recognize
 *			VLAN-tagged frames.
 */
struct an8855_port {
	bool enable;
	u32 pm;
	u16 pvid;
};

/* struct an8855_info -	This is the main data structure for holding the specific
 *			part for each supported device
 * @sw_setup:		Holding the handler to a device initialization
 * @phy_read:		Holding the way reading PHY port
 * @phy_write:		Holding the way writing PHY port
 * @pad_setup:		Holding the way setting up the bus pad for a certain
 *			MAC port
 * @phy_mode_supported:	Check if the PHY type is being supported on a certain
 *			port
 * @mac_port_validate:	Holding the way to set addition validate type for a
 *			certan MAC port
 * @mac_port_get_state: Holding the way getting the MAC/PCS state for a certain
 *			MAC port
 * @mac_port_config:	Holding the way setting up the PHY attribute to a
 *			certain MAC port
 * @mac_pcs_an_restart	Holding the way restarting PCS autonegotiation for a
 *			certain MAC port
 * @mac_pcs_link_up:	Holding the way setting up the PHY attribute to the pcs
 *			of the certain MAC port
 */
struct an8855_dev_info {
	enum an8855_id id;

	const struct phylink_pcs_ops *pcs_ops;

	int (*sw_setup)(struct dsa_switch *ds);
	int (*phy_read)(struct dsa_switch *ds, int port, int regnum);
	int (*phy_write)(struct dsa_switch *ds, int port, int regnum,
			  u16 val);
	int (*pad_setup)(struct dsa_switch *ds, phy_interface_t interface);
	int (*cpu_port_config)(struct dsa_switch *ds, int port);
	bool (*phy_mode_supported)(struct dsa_switch *ds, int port,
					const struct phylink_link_state *state);
	void (*mac_port_validate)(struct dsa_switch *ds, int port,
				   unsigned long *supported);
	int (*mac_port_config)(struct dsa_switch *ds, int port,
				unsigned int mode, phy_interface_t interface);
};

struct an8855_priv;

struct an8855_pcs {
	struct phylink_pcs pcs;
	struct an8855_priv *priv;
	int port;
};

/* struct an8855_priv -	This is the main data structure for holding the state
 *			of the driver
 * @dev:		The device pointer
 * @ds:			The pointer to the dsa core structure
 * @bus:		The bus used for the device and built-in PHY
 * @rstc:		The pointer to reset control used by MCM
 * @core_pwr:		The power supplied into the core
 * @io_pwr:		The power supplied into the I/O
 * @reset:		The descriptor for GPIO line tied to its reset pin
 * @mcm:		Flag for distinguishing if standalone IC or module
 *			coupling
 * @ports:		Holding the state among ports
 * @reg_mutex:		The lock for protecting among process accessing
 *			registers
 * @p6_interface	Holding the current port 6 interface
 * @p5_intf_sel:	Holding the current port 5 interface select
 */
struct an8855_priv {
	struct device *dev;
	struct dsa_switch *ds;
	struct mii_bus *bus;
	struct reset_control *rstc;
	struct regulator *core_pwr;
	struct regulator *io_pwr;
	struct gpio_desc *reset;
	void __iomem *base;
	const struct an8855_dev_info *info;
	unsigned int phy_base;
	int phy_base_new;
	unsigned int id;
	u32 intr_pin;
	phy_interface_t p5_interface;
	unsigned int p5_intf_sel;
	u8 mirror_rx;
	u8 mirror_tx;
	u8 eee_enable;
	u32 extSurge;

	struct an8855_port ports[AN8855_NUM_PORTS];
	struct an8855_pcs pcs[AN8855_NUM_PORTS];
	/* protect among processes for registers access */
	struct mutex reg_mutex;
};

struct an8855_hw_vlan_entry {
	int port;
	u8 old_members;
	bool untagged;
};

static inline void an8855_hw_vlan_entry_init(struct an8855_hw_vlan_entry *e,
						 int port, bool untagged)
{
	e->port = port;
	e->untagged = untagged;
}

typedef void (*an8855_vlan_op) (struct an8855_priv *,
				struct an8855_hw_vlan_entry *);

struct an8855_hw_stats {
	const char *string;
	u16 reg;
	u8 sizeof_stat;
};

struct an8855_dummy_poll {
	struct an8855_priv *priv;
	u32 reg;
};

static inline void INIT_AN8855_DUMMY_POLL(struct an8855_dummy_poll *p,
					  struct an8855_priv *priv, u32 reg)
{
	p->priv = priv;
	p->reg = reg;
}

int an8855_phy_setup(struct dsa_switch *ds);
u32 an8855_read(struct an8855_priv *priv, u32 reg);
void an8855_write(struct an8855_priv *priv, u32 reg, u32 val);
int an8855_phy_cl22_read(struct an8855_priv *priv, int port, int regnum);
int an8855_phy_cl22_write(struct an8855_priv *priv, int port,
				int regnum, u16 val);
int an8855_phy_cl45_read(struct an8855_priv *priv, int port, int devad,
				int regnum);
int an8855_phy_cl45_write(struct an8855_priv *priv, int port, int devad,
				int regnum,	u16 val);
#endif /* __AN8855_H */

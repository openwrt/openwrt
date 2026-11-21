/* SPDX-License-Identifier: GPL-2.0-only */
/* Registers and FoE table of the first-generation Ralink PPE. Register
 * offsets are relative to the frame engine base. Field layouts are taken
 * from the Ralink SDK hw_nat driver.
 */

#ifndef __RALINK_PPE_V1_H
#define __RALINK_PPE_V1_H

#include <linux/bitfield.h>
#include <linux/bits.h>
#include <linux/types.h>

/* GDMA forward configuration: one 3-bit destination per frame class */
#define PPE_GDMA_FWD_UC_MYMAC		GENMASK(14, 12)
#define PPE_GDMA_FWD_BC			GENMASK(10, 8)
#define PPE_GDMA_FWD_MC			GENMASK(6, 4)
#define PPE_GDMA_FWD_UC_OTHER		GENMASK(2, 0)
#define PPE_GDMA_FWD_MASK		(PPE_GDMA_FWD_UC_MYMAC | \
					 PPE_GDMA_FWD_BC | PPE_GDMA_FWD_MC | \
					 PPE_GDMA_FWD_UC_OTHER)
#define PPE_PSE_PORT_CPU		0
#define PPE_PSE_PORT_GDMA1		1
#define PPE_PSE_PORT_GDMA2		2
#define PPE_PSE_PORT_PPE		6

/* FoE timestamp counter (1 s ticks, 16 bit) */
#define PPE_REG_FOE_TS			0x01c

#define PPE_REG_BASE			0x200
#define PPE_REG_GLO_CFG			(PPE_REG_BASE + 0x00)
#define PPE_REG_STD_GA_H		(PPE_REG_BASE + 0x04)
#define PPE_REG_STD_GA_L		(PPE_REG_BASE + 0x08)
#define PPE_REG_EXT_GA_H		(PPE_REG_BASE + 0x0c)
#define PPE_REG_EXT_GA_L		(PPE_REG_BASE + 0x10)
#define PPE_REG_FLOW_SET		(PPE_REG_BASE + 0x14)
#define PPE_REG_PRE_ACL			(PPE_REG_BASE + 0x18)
#define PPE_REG_PRE_MTR			(PPE_REG_BASE + 0x1c)
#define PPE_REG_PRE_AC			(PPE_REG_BASE + 0x20)
#define PPE_REG_POST_MTR		(PPE_REG_BASE + 0x24)
#define PPE_REG_POST_AC			(PPE_REG_BASE + 0x28)
#define PPE_REG_POL_CFG			(PPE_REG_BASE + 0x2c)
#define PPE_REG_FOE_CFG			(PPE_REG_BASE + 0x30)
#define PPE_REG_FOE_BASE		(PPE_REG_BASE + 0x34)
#define PPE_REG_FOE_USE			(PPE_REG_BASE + 0x38)
#define PPE_REG_FOE_BNDR		(PPE_REG_BASE + 0x3c)
#define PPE_REG_FOE_LMT1		(PPE_REG_BASE + 0x40)
#define PPE_REG_FOE_LMT2		(PPE_REG_BASE + 0x44)
#define PPE_REG_FOE_KA			(PPE_REG_BASE + 0x48)
#define PPE_REG_FOE_UNB_AGE		(PPE_REG_BASE + 0x4c)
#define PPE_REG_FOE_BND_AGE0		(PPE_REG_BASE + 0x50)
#define PPE_REG_FOE_BND_AGE1		(PPE_REG_BASE + 0x54)
#define PPE_REG_CPU_PORT_CFG		(PPE_REG_BASE + 0x58)
#define PPE_REG_GE1_PORT_CFG		(PPE_REG_BASE + 0x5c)
/* QoS: user priority (UP) from DSCP, VLAN priority, frame length or ACL */
#define PPE_REG_DSCP_MAP_UP(n)		(PPE_REG_BASE + 0x60 + (n) * 4)
#define PPE_REG_AUTO_UP_CFG1		(PPE_REG_BASE + 0x80)
#define PPE_REG_AUTO_UP_CFG2		(PPE_REG_BASE + 0x84)
#define PPE_REG_UP_RES			(PPE_REG_BASE + 0x88)
#define PPE_REG_UP_MAP_VPRI		(PPE_REG_BASE + 0x8c)
#define PPE_REG_UP_MAP_IDSCP(n)		(PPE_REG_BASE + 0x90 + (n) * 4)
#define PPE_REG_UP_MAP_ODSCP(n)		(PPE_REG_BASE + 0x98 + (n) * 4)
#define PPE_REG_UP_MAP_AC		(PPE_REG_BASE + 0xa0)

/* Accounting counters, meters, and the policy (ACL) table of 8-byte rules */
#define PPE_REG_AC_BASE			0x400
#define PPE_REG_MTR_BASE		0x600
#define PPE_REG_POLICY_BASE		0x1000

#define PPE_GLO_CFG_EN			BIT(0)
#define PPE_GLO_CFG_TTL0_DRP		BIT(4)
#define PPE_GLO_CFG_VPRI_EN		BIT(8)
#define PPE_GLO_CFG_DPRI_EN		BIT(9)
#define PPE_GLO_CFG_REG_VPRI		BIT(10)
#define PPE_GLO_CFG_REG_DSCP		BIT(11)
#define PPE_GLO_CFG_RED_MODE		GENMASK(13, 12)
#define PPE_GLO_CFG_ACL_PRI_EN		BIT(14)

/* Per frame class: post-account, post-meter, FoE, pre-account, pre-meter
 * and ACL engine
 */
#define PPE_FLOW_SET_BC_POSA		BIT(0)
#define PPE_FLOW_SET_BC_POSM		BIT(1)
#define PPE_FLOW_SET_BC_FOE		BIT(2)
#define PPE_FLOW_SET_BC_PREA		BIT(3)
#define PPE_FLOW_SET_BC_PREM		BIT(4)
#define PPE_FLOW_SET_BC_ACL		BIT(5)
#define PPE_FLOW_SET_BC_ENGINES		GENMASK(5, 0)
#define PPE_FLOW_SET_MC_POSA		BIT(8)
#define PPE_FLOW_SET_MC_POSM		BIT(9)
#define PPE_FLOW_SET_MC_FOE		BIT(10)
#define PPE_FLOW_SET_MC_PREA		BIT(11)
#define PPE_FLOW_SET_MC_PREM		BIT(12)
#define PPE_FLOW_SET_MC_ACL		BIT(13)
#define PPE_FLOW_SET_MC_ENGINES		GENMASK(13, 8)
#define PPE_FLOW_SET_UC_POSA		BIT(16)
#define PPE_FLOW_SET_UC_POSM		BIT(17)
#define PPE_FLOW_SET_UC_FOE		BIT(18)
#define PPE_FLOW_SET_UC_PREA		BIT(19)
#define PPE_FLOW_SET_UC_PREM		BIT(20)
#define PPE_FLOW_SET_UC_ACL		BIT(21)
#define PPE_FLOW_SET_IPV6_FOE		BIT(24)
#define PPE_FLOW_SET_IPV6_PE		BIT(25)
#define PPE_FLOW_SET_IPV4_NAT		BIT(26)
#define PPE_FLOW_SET_IPV4_NAPT		BIT(27)

/* PRE_ACL, PRE_MTR, PRE_AC, POST_MTR, POST_AC: rule range of the engine */
#define PPE_RULE_START			GENMASK(8, 0)
#define PPE_RULE_END			GENMASK(24, 16)

#define PPE_POL_CFG_POST_AC_EN		BIT(12)
#define PPE_POL_CFG_PRE_AC_EN		BIT(13)
#define PPE_POL_CFG_AC_PRD		GENMASK(31, 16)	/* ms */

#define PPE_FOE_CFG_TBL_SIZE		GENMASK(2, 0)	/* 0..4 = 1K..16K */
#define PPE_FOE_CFG_HASH_MODE		BIT(3)
#define PPE_FOE_CFG_SMA			GENMASK(5, 4)	/* search miss action */
#define PPE_FOE_SMA_DROP		0
#define PPE_FOE_SMA_ONLY_FWD_CPU	2
#define PPE_FOE_SMA_FWD_CPU_BUILD	3
#define PPE_FOE_CFG_UNB_AGE		BIT(8)
#define PPE_FOE_CFG_TCP_AGE		BIT(9)
#define PPE_FOE_CFG_UDP_AGE		BIT(10)
#define PPE_FOE_CFG_FIN_AGE		BIT(11)
#define PPE_FOE_CFG_KA_ORG_HDR		BIT(12)
#define PPE_FOE_CFG_KA_EN		BIT(13)

#define PPE_FOE_BNDR_RATE		GENMASK(15, 0)
#define PPE_FOE_LMT1_QURT		GENMASK(13, 0)
#define PPE_FOE_LMT1_HALF		GENMASK(29, 16)
#define PPE_FOE_LMT2_FULL		GENMASK(13, 0)
#define PPE_FOE_KA_T			GENMASK(15, 0)
#define PPE_FOE_KA_TCP			GENMASK(23, 16)
#define PPE_FOE_KA_UDP			GENMASK(31, 24)
#define PPE_FOE_UNB_AGE_DLTA		GENMASK(7, 0)
#define PPE_FOE_UNB_AGE_MNP		GENMASK(31, 16)
#define PPE_FOE_BND_AGE0_UDP		GENMASK(15, 0)
#define PPE_FOE_BND_AGE1_TCP		GENMASK(15, 0)
#define PPE_FOE_BND_AGE1_FIN		GENMASK(31, 16)

/* Weight of each UP source when resolving the user priority */
#define PPE_UP_RES_PORT			GENMASK(2, 0)
#define PPE_UP_RES_VPRI			GENMASK(6, 4)
#define PPE_UP_RES_DSCP			GENMASK(10, 8)
#define PPE_UP_RES_ACL			GENMASK(14, 12)
#define PPE_UP_RES_FOE			GENMASK(18, 16)
#define PPE_UP_RES_AUTO			GENMASK(22, 20)

/* Fields of the map registers, by DSCP 8n + i or by UP */
#define PPE_DSCP_MAP_UP(i)		(GENMASK(2, 0) << ((i) * 4))
#define PPE_UP_MAP_VPRI(up)		(GENMASK(2, 0) << ((up) * 4))
#define PPE_UP_MAP_DSCP(up)		(GENMASK(5, 0) << ((up) % 4 * 8))
#define PPE_UP_MAP_AC(up)		(GENMASK(1, 0) << ((up) * 2))

/* CPU reason codes reported for frames the PPE hands to the CPU */
enum ppe_v1_cpu_reason {
	PPE_CPU_REASON_TTL_0			= 0x80,
	PPE_CPU_REASON_NOT_IPV4_HLEN5		= 0x90,
	PPE_CPU_REASON_NOT_TCP_UDP		= 0x91,
	PPE_CPU_REASON_TCP_SYN_FIN_RST		= 0x92,
	PPE_CPU_REASON_UN_HIT			= 0x93,
	PPE_CPU_REASON_HIT_UNBIND		= 0x94,
	PPE_CPU_REASON_HIT_UNBIND_RATE_REACH	= 0x95,
	PPE_CPU_REASON_HIT_FIN			= 0x96,
	PPE_CPU_REASON_HIT_BIND_TTL_1		= 0x97,
	PPE_CPU_REASON_HIT_BIND_KEEPALIVE	= 0x98,
	PPE_CPU_REASON_HIT_BIND_FORCE_TO_CPU	= 0x99,
	PPE_CPU_REASON_ACL_FOE_TBL_ERR		= 0x9a,
	PPE_CPU_REASON_ACL_TBL_TTL_1		= 0x9b,
	PPE_CPU_REASON_ACL_ALERT_CPU		= 0x9c,
	PPE_CPU_REASON_NO_FORCE_DEST_PORT	= 0xa0,
	PPE_CPU_REASON_EXCEED_MTU		= 0xa1,
	/* ACL forced user priority 0 to 7 */
	PPE_CPU_REASON_ACL_FORCE_UP0		= 0xa8,
	PPE_CPU_REASON_ACL_FORCE_UP7		= 0xaf,
};

/* FoE table */
#define PPE_FOE_ENTRY_SIZE		64
#define PPE_FOE_ENTRIES_MIN		1024
#define PPE_FOE_ENTRIES_MAX		16384

/* Information block 1, common bits */
#define PPE_IB1_PKT_TYPE		GENMASK(27, 26)
#define PPE_PKT_TYPE_IPV4_HNAPT		0
#define PPE_PKT_TYPE_IPV4_HNAT		1
#define PPE_PKT_TYPE_IPV6_1T_ROUTE	2
#define PPE_IB1_STATE			GENMASK(29, 28)
#define PPE_STATE_INVALID		0
#define PPE_STATE_UNBIND		1
#define PPE_STATE_BIND			2
#define PPE_STATE_FIN			3
#define PPE_IB1_UDP			BIT(30)
#define PPE_IB1_STATIC			BIT(31)

/* Information block 1, unbound view */
#define PPE_IB1_UNB_TIMESTAMP		GENMASK(7, 0)
#define PPE_IB1_UNB_PCNT		GENMASK(23, 8)

/* Information block 1, bound view */
#define PPE_IB1_BIND_TIMESTAMP		GENMASK(15, 0)
#define PPE_IB1_BIND_VLAN1		GENMASK(17, 16)
#define PPE_IB1_BIND_VLAN2		GENMASK(19, 18)
#define PPE_IB1_BIND_SNAP		GENMASK(21, 20)
#define PPE_IB1_BIND_PPPOE		GENMASK(23, 22)
#define PPE_IB1_BIND_TTL		BIT(24)
#define PPE_IB1_BIND_KEEPALIVE		BIT(25)

/* L2 header actions for the VLAN1/VLAN2/SNAP/PPPoE fields above */
#define PPE_L2_ACT_NONE			0
#define PPE_L2_ACT_MODIFY		1
#define PPE_L2_ACT_INSERT		2
#define PPE_L2_ACT_DELETE		3

/* Information block 2 */
#define PPE_IB2_FORCE_DP		BIT(0)
#define PPE_IB2_DP			GENMASK(3, 1)
#define PPE_IB2_FORCE_UP		BIT(4)
#define PPE_IB2_UP			GENMASK(7, 5)
#define PPE_IB2_PORT_MG			GENMASK(13, 8)
#define PPE_IB2_METER_EN		BIT(15)
#define PPE_IB2_PORT_AG			GENMASK(21, 16)
#define PPE_IB2_DSCP_REMARK		BIT(22)
#define PPE_IB2_ACCT_EN			BIT(23)
#define PPE_IB2_DSCP			GENMASK(31, 24)

/* L2 action dword; act_dp is left to software, the PPE ignores it */
#define PPE_L2_ACT_SNAP_CTRL		GENMASK(23, 0)
#define PPE_L2_ACT_DP			GENMASK(29, 24)

/* IPv4 HNAPT/HNAT entry, 16 little-endian dwords. Addresses and ports are
 * stored as host-order numbers. MAC addresses are split as a 48-bit number:
 * the *_hi field holds bytes 0-1, the *_lo dword bytes 2-5.
 */
struct ppe_v1_foe_ipv4 {
	u32 ib1;
	u32 sip;
	u32 dip;
	u16 dport;
	u16 sport;
	u32 ib2;
	u32 new_sip;
	u32 new_dip;
	u16 new_dport;
	u16 new_sport;
	u16 dmac_hi;
	u16 vlan1;
	u32 dmac_lo;
	u16 smac_hi;
	u16 pppoe_id;
	u32 smac_lo;
	u32 act;
	u16 vlan2;
	u16 resv2;
	u32 resv3;
	u32 resv4;
} __packed;

struct ppe_v1_foe_entry {
	union {
		struct ppe_v1_foe_ipv4 ipv4;
		u32 data[PPE_FOE_ENTRY_SIZE / 4];
	};
};

static_assert(sizeof(struct ppe_v1_foe_entry) == PPE_FOE_ENTRY_SIZE);

/* RX descriptor word 4: where the PPE reports what it did with a frame */
#define PPE_RXD4_FOE_ENTRY		GENMASK(13, 0)
#define PPE_RXD4_FVLD			BIT(14)
#define PPE_RXD4_ALG			BIT(15)
#define PPE_RXD4_AI			GENMASK(23, 16)
#define PPE_RXD4_SP			GENMASK(26, 24)
#define PPE_RXD4_AIS			BIT(27)

#endif /* __RALINK_PPE_V1_H */

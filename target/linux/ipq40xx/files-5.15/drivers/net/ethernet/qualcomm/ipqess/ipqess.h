// SPDX-License-Identifier: (GPL-2.0 OR ISC)
/* Copyright (c) 2014 - 2016, The Linux Foundation. All rights reserved.
 * Copyright (c) 2017 - 2018, John Crispin <john@phrozen.org>
 * Copyright (c) 2018 - 2019, Christian Lamparter <chunkeey@gmail.com>
 * Copyright (c) 2020 - 2021, Gabor Juhos <j4g8y7@gmail.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all copies.
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef _IPQESS_H_
#define _IPQESS_H_

#define IPQESS_NETDEV_QUEUES	4

#define IPQESS_TPD_EOP_SHIFT 31

#define IPQESS_PORT_ID_SHIFT 12
#define IPQESS_PORT_ID_MASK 0x7

/* tpd word 3 bit 18-28 */
#define IPQESS_TPD_PORT_BITMAP_SHIFT 18

#define IPQESS_TPD_FROM_CPU_SHIFT 25

#define IPQESS_RX_RING_SIZE 128
#define IPQESS_RX_HEAD_BUFF_SIZE 1540
#define IPQESS_TX_RING_SIZE 128
#define IPQESS_MAX_RX_QUEUE 8
#define IPQESS_MAX_TX_QUEUE 16


/* Configurations */
#define IPQESS_INTR_CLEAR_TYPE 0
#define IPQESS_INTR_SW_IDX_W_TYPE 0
#define IPQESS_FIFO_THRESH_TYPE 0
#define IPQESS_RSS_TYPE 0
#define IPQESS_RX_IMT 0x0020
#define IPQESS_TX_IMT 0x0050
#define IPQESS_TPD_BURST 5
#define IPQESS_TXF_BURST 0x100
#define IPQESS_RFD_BURST 8
#define IPQESS_RFD_THR 16
#define IPQESS_RFD_LTHR 0

/* Flags used in transmit direction */
#define IPQESS_DESC_LAST 0x1
#define IPQESS_DESC_SINGLE 0x2
#define IPQESS_DESC_PAGE 0x4

struct ipqesstool_statistics {
	u32 tx_q0_pkt;
	u32 tx_q1_pkt;
	u32 tx_q2_pkt;
	u32 tx_q3_pkt;
	u32 tx_q4_pkt;
	u32 tx_q5_pkt;
	u32 tx_q6_pkt;
	u32 tx_q7_pkt;
	u32 tx_q8_pkt;
	u32 tx_q9_pkt;
	u32 tx_q10_pkt;
	u32 tx_q11_pkt;
	u32 tx_q12_pkt;
	u32 tx_q13_pkt;
	u32 tx_q14_pkt;
	u32 tx_q15_pkt;
	u32 tx_q0_byte;
	u32 tx_q1_byte;
	u32 tx_q2_byte;
	u32 tx_q3_byte;
	u32 tx_q4_byte;
	u32 tx_q5_byte;
	u32 tx_q6_byte;
	u32 tx_q7_byte;
	u32 tx_q8_byte;
	u32 tx_q9_byte;
	u32 tx_q10_byte;
	u32 tx_q11_byte;
	u32 tx_q12_byte;
	u32 tx_q13_byte;
	u32 tx_q14_byte;
	u32 tx_q15_byte;
	u32 rx_q0_pkt;
	u32 rx_q1_pkt;
	u32 rx_q2_pkt;
	u32 rx_q3_pkt;
	u32 rx_q4_pkt;
	u32 rx_q5_pkt;
	u32 rx_q6_pkt;
	u32 rx_q7_pkt;
	u32 rx_q0_byte;
	u32 rx_q1_byte;
	u32 rx_q2_byte;
	u32 rx_q3_byte;
	u32 rx_q4_byte;
	u32 rx_q5_byte;
	u32 rx_q6_byte;
	u32 rx_q7_byte;
	u32 tx_desc_error;
};

struct ipqess_tx_desc {
	__le16  len;
	__le16  svlan_tag;
	__le32  word1;
	__le32  addr;
	__le32  word3;
} __aligned(16) __packed;

struct ipqess_rx_desc {
	u16 rrd0;
	u16 rrd1;
	u16 rrd2;
	u16 rrd3;
	u16 rrd4;
	u16 rrd5;
	u16 rrd6;
	u16 rrd7;
} __aligned(16) __packed;

struct ipqess_buf {
	struct sk_buff *skb;
	dma_addr_t dma;
	u32 flags;
	u16 length;
};

struct ipqess_tx_ring {
	struct napi_struct napi_tx;
	u32 idx;
	int ring_id;
	struct ipqess *ess;
	struct netdev_queue *nq;
	struct ipqess_tx_desc *hw_desc;
	struct ipqess_buf *buf;
	dma_addr_t dma;
	u16 count;
	u16 head;
	u16 tail;
};

struct ipqess_rx_ring {
	struct napi_struct napi_rx;
	u32 idx;
	int ring_id;
	struct ipqess *ess;
	struct device *ppdev;
	struct ipqess_rx_desc **hw_desc;
	struct ipqess_buf *buf;
	dma_addr_t dma;
	u16 head;
	u16 tail;
	atomic_t refill_count;
};

struct ipqess_rx_ring_refill {
	struct ipqess_rx_ring *rx_ring;
	struct work_struct refill_work;
};

#define IPQESS_IRQ_NAME_LEN	32

struct ipqess {
	struct net_device *netdev;
	void __iomem *hw_addr;
	struct clk *ess_clk;
	struct reset_control *ess_rst;

	struct ipqess_rx_ring rx_ring[IPQESS_NETDEV_QUEUES];

	struct platform_device *pdev;
	struct phylink *phylink;
	struct phylink_config phylink_config;
	struct ipqess_tx_ring tx_ring[IPQESS_NETDEV_QUEUES];

	struct ipqesstool_statistics ipqessstats;
	spinlock_t stats_lock;
	struct net_device_stats stats;

	struct ipqess_rx_ring_refill rx_refill[IPQESS_NETDEV_QUEUES];
	u32 tx_irq[IPQESS_MAX_TX_QUEUE];
	char tx_irq_names[IPQESS_MAX_TX_QUEUE][IPQESS_IRQ_NAME_LEN];
	u32 rx_irq[IPQESS_MAX_RX_QUEUE];
	char rx_irq_names[IPQESS_MAX_TX_QUEUE][IPQESS_IRQ_NAME_LEN];
};

static inline void build_test(void)
{
	struct ipqess *ess;
	BUILD_BUG_ON(ARRAY_SIZE(ess->rx_ring) != ARRAY_SIZE(ess->rx_refill));
}

void ipqess_set_ethtool_ops(struct net_device *netdev);
void ipqess_update_hw_stats(struct ipqess *ess);

/* register definition */
#define IPQESS_REG_MAS_CTRL 0x0
#define IPQESS_REG_TIMEOUT_CTRL 0x004
#define IPQESS_REG_DBG0 0x008
#define IPQESS_REG_DBG1 0x00C
#define IPQESS_REG_SW_CTRL0 0x100
#define IPQESS_REG_SW_CTRL1 0x104

/* Interrupt Status Register */
#define IPQESS_REG_RX_ISR 0x200
#define IPQESS_REG_TX_ISR 0x208
#define IPQESS_REG_MISC_ISR 0x210
#define IPQESS_REG_WOL_ISR 0x218

#define IPQESS_MISC_ISR_RX_URG_Q(x) (1 << x)

#define IPQESS_MISC_ISR_AXIR_TIMEOUT 0x00000100
#define IPQESS_MISC_ISR_AXIR_ERR 0x00000200
#define IPQESS_MISC_ISR_TXF_DEAD 0x00000400
#define IPQESS_MISC_ISR_AXIW_ERR 0x00000800
#define IPQESS_MISC_ISR_AXIW_TIMEOUT 0x00001000

#define IPQESS_WOL_ISR 0x00000001

/* Interrupt Mask Register */
#define IPQESS_REG_MISC_IMR 0x214
#define IPQESS_REG_WOL_IMR 0x218

#define IPQESS_RX_IMR_NORMAL_MASK 0x1
#define IPQESS_TX_IMR_NORMAL_MASK 0x1
#define IPQESS_MISC_IMR_NORMAL_MASK 0x80001FFF
#define IPQESS_WOL_IMR_NORMAL_MASK 0x1

/* Edma receive consumer index */
#define IPQESS_REG_RX_SW_CONS_IDX_Q(x) (0x220 + ((x) << 2)) /* x is the queue id */

/* Edma transmit consumer index */
#define IPQESS_REG_TX_SW_CONS_IDX_Q(x) (0x240 + ((x) << 2)) /* x is the queue id */

/* IRQ Moderator Initial Timer Register */
#define IPQESS_REG_IRQ_MODRT_TIMER_INIT 0x280
#define IPQESS_IRQ_MODRT_TIMER_MASK 0xFFFF
#define IPQESS_IRQ_MODRT_RX_TIMER_SHIFT 0
#define IPQESS_IRQ_MODRT_TX_TIMER_SHIFT 16

/* Interrupt Control Register */
#define IPQESS_REG_INTR_CTRL 0x284
#define IPQESS_INTR_CLR_TYP_SHIFT 0
#define IPQESS_INTR_SW_IDX_W_TYP_SHIFT 1
#define IPQESS_INTR_CLEAR_TYPE_W1 0
#define IPQESS_INTR_CLEAR_TYPE_R 1

/* RX Interrupt Mask Register */
#define IPQESS_REG_RX_INT_MASK_Q(x) (0x300 + ((x) << 2)) /* x = queue id */

/* TX Interrupt mask register */
#define IPQESS_REG_TX_INT_MASK_Q(x) (0x340 + ((x) << 2)) /* x = queue id */

/* Load Ptr Register
 * Software sets this bit after the initialization of the head and tail
 */
#define IPQESS_REG_TX_SRAM_PART 0x400
#define IPQESS_LOAD_PTR_SHIFT 16

/* TXQ Control Register */
#define IPQESS_REG_TXQ_CTRL 0x404
#define IPQESS_TXQ_CTRL_IP_OPTION_EN 0x10
#define IPQESS_TXQ_CTRL_TXQ_EN 0x20
#define IPQESS_TXQ_CTRL_ENH_MODE 0x40
#define IPQESS_TXQ_CTRL_LS_8023_EN 0x80
#define IPQESS_TXQ_CTRL_TPD_BURST_EN 0x100
#define IPQESS_TXQ_CTRL_LSO_BREAK_EN 0x200
#define IPQESS_TXQ_NUM_TPD_BURST_MASK 0xF
#define IPQESS_TXQ_TXF_BURST_NUM_MASK 0xFFFF
#define IPQESS_TXQ_NUM_TPD_BURST_SHIFT 0
#define IPQESS_TXQ_TXF_BURST_NUM_SHIFT 16

#define	IPQESS_REG_TXF_WATER_MARK 0x408 /* In 8-bytes */
#define IPQESS_TXF_WATER_MARK_MASK 0x0FFF
#define IPQESS_TXF_LOW_WATER_MARK_SHIFT 0
#define IPQESS_TXF_HIGH_WATER_MARK_SHIFT 16
#define IPQESS_TXQ_CTRL_BURST_MODE_EN 0x80000000

/* WRR Control Register */
#define IPQESS_REG_WRR_CTRL_Q0_Q3 0x40c
#define IPQESS_REG_WRR_CTRL_Q4_Q7 0x410
#define IPQESS_REG_WRR_CTRL_Q8_Q11 0x414
#define IPQESS_REG_WRR_CTRL_Q12_Q15 0x418

/* Weight round robin(WRR), it takes queue as input, and computes
 * starting bits where we need to write the weight for a particular
 * queue
 */
#define IPQESS_WRR_SHIFT(x) (((x) * 5) % 20)

/* Tx Descriptor Control Register */
#define IPQESS_REG_TPD_RING_SIZE 0x41C
#define IPQESS_TPD_RING_SIZE_SHIFT 0
#define IPQESS_TPD_RING_SIZE_MASK 0xFFFF

/* Transmit descriptor base address */
#define IPQESS_REG_TPD_BASE_ADDR_Q(x) (0x420 + ((x) << 2)) /* x = queue id */

/* TPD Index Register */
#define IPQESS_REG_TPD_IDX_Q(x) (0x460 + ((x) << 2)) /* x = queue id */

#define IPQESS_TPD_PROD_IDX_BITS 0x0000FFFF
#define IPQESS_TPD_CONS_IDX_BITS 0xFFFF0000
#define IPQESS_TPD_PROD_IDX_MASK 0xFFFF
#define IPQESS_TPD_CONS_IDX_MASK 0xFFFF
#define IPQESS_TPD_PROD_IDX_SHIFT 0
#define IPQESS_TPD_CONS_IDX_SHIFT 16

/* TX Virtual Queue Mapping Control Register */
#define IPQESS_REG_VQ_CTRL0 0x4A0
#define IPQESS_REG_VQ_CTRL1 0x4A4

/* Virtual QID shift, it takes queue as input, and computes
 * Virtual QID position in virtual qid control register
 */
#define IPQESS_VQ_ID_SHIFT(i) (((i) * 3) % 24)

/* Virtual Queue Default Value */
#define IPQESS_VQ_REG_VALUE 0x240240

/* Tx side Port Interface Control Register */
#define IPQESS_REG_PORT_CTRL 0x4A8
#define IPQESS_PAD_EN_SHIFT 15

/* Tx side VLAN Configuration Register */
#define IPQESS_REG_VLAN_CFG 0x4AC

#define IPQESS_VLAN_CFG_SVLAN_TPID_SHIFT 0
#define IPQESS_VLAN_CFG_SVLAN_TPID_MASK 0xffff
#define IPQESS_VLAN_CFG_CVLAN_TPID_SHIFT 16
#define IPQESS_VLAN_CFG_CVLAN_TPID_MASK 0xffff

#define IPQESS_TX_CVLAN 16
#define IPQESS_TX_INS_CVLAN 17
#define IPQESS_TX_CVLAN_TAG_SHIFT 0

#define IPQESS_TX_SVLAN 14
#define IPQESS_TX_INS_SVLAN 15
#define IPQESS_TX_SVLAN_TAG_SHIFT 16

/* Tx Queue Packet Statistic Register */
#define IPQESS_REG_TX_STAT_PKT_Q(x) (0x700 + ((x) << 3)) /* x = queue id */

#define IPQESS_TX_STAT_PKT_MASK 0xFFFFFF

/* Tx Queue Byte Statistic Register */
#define IPQESS_REG_TX_STAT_BYTE_Q(x) (0x704 + ((x) << 3)) /* x = queue id */

/* Load Balance Based Ring Offset Register */
#define IPQESS_REG_LB_RING 0x800
#define IPQESS_LB_RING_ENTRY_MASK 0xff
#define IPQESS_LB_RING_ID_MASK 0x7
#define IPQESS_LB_RING_PROFILE_ID_MASK 0x3
#define IPQESS_LB_RING_ENTRY_BIT_OFFSET 8
#define IPQESS_LB_RING_ID_OFFSET 0
#define IPQESS_LB_RING_PROFILE_ID_OFFSET 3
#define IPQESS_LB_REG_VALUE 0x6040200

/* Load Balance Priority Mapping Register */
#define IPQESS_REG_LB_PRI_START 0x804
#define IPQESS_REG_LB_PRI_END 0x810
#define IPQESS_LB_PRI_REG_INC 4
#define IPQESS_LB_PRI_ENTRY_BIT_OFFSET 4
#define IPQESS_LB_PRI_ENTRY_MASK 0xf

/* RSS Priority Mapping Register */
#define IPQESS_REG_RSS_PRI 0x820
#define IPQESS_RSS_PRI_ENTRY_MASK 0xf
#define IPQESS_RSS_RING_ID_MASK 0x7
#define IPQESS_RSS_PRI_ENTRY_BIT_OFFSET 4

/* RSS Indirection Register */
#define IPQESS_REG_RSS_IDT(x) (0x840 + ((x) << 2)) /* x = No. of indirection table */
#define IPQESS_NUM_IDT 16
#define IPQESS_RSS_IDT_VALUE 0x64206420

/* Default RSS Ring Register */
#define IPQESS_REG_DEF_RSS 0x890
#define IPQESS_DEF_RSS_MASK 0x7

/* RSS Hash Function Type Register */
#define IPQESS_REG_RSS_TYPE 0x894
#define IPQESS_RSS_TYPE_NONE 0x01
#define IPQESS_RSS_TYPE_IPV4TCP 0x02
#define IPQESS_RSS_TYPE_IPV6_TCP 0x04
#define IPQESS_RSS_TYPE_IPV4_UDP 0x08
#define IPQESS_RSS_TYPE_IPV6UDP 0x10
#define IPQESS_RSS_TYPE_IPV4 0x20
#define IPQESS_RSS_TYPE_IPV6 0x40
#define IPQESS_RSS_HASH_MODE_MASK 0x7f

#define IPQESS_REG_RSS_HASH_VALUE 0x8C0

#define IPQESS_REG_RSS_TYPE_RESULT 0x8C4

#define IPQESS_HASH_TYPE_START 0
#define IPQESS_HASH_TYPE_END 5
#define IPQESS_HASH_TYPE_SHIFT 12

#define IPQESS_RFS_FLOW_ENTRIES 1024
#define IPQESS_RFS_FLOW_ENTRIES_MASK (IPQESS_RFS_FLOW_ENTRIES - 1)
#define IPQESS_RFS_EXPIRE_COUNT_PER_CALL 128

/* RFD Base Address Register */
#define IPQESS_REG_RFD_BASE_ADDR_Q(x) (0x950 + ((x) << 2)) /* x = queue id */

/* RFD Index Register */
#define IPQESS_REG_RFD_IDX_Q(x) (0x9B0 + ((x) << 2)) /* x = queue id */

#define IPQESS_RFD_PROD_IDX_BITS 0x00000FFF
#define IPQESS_RFD_CONS_IDX_BITS 0x0FFF0000
#define IPQESS_RFD_PROD_IDX_MASK 0xFFF
#define IPQESS_RFD_CONS_IDX_MASK 0xFFF
#define IPQESS_RFD_PROD_IDX_SHIFT 0
#define IPQESS_RFD_CONS_IDX_SHIFT 16

/* Rx Descriptor Control Register */
#define IPQESS_REG_RX_DESC0 0xA10
#define IPQESS_RFD_RING_SIZE_MASK 0xFFF
#define IPQESS_RX_BUF_SIZE_MASK 0xFFFF
#define IPQESS_RFD_RING_SIZE_SHIFT 0
#define IPQESS_RX_BUF_SIZE_SHIFT 16

#define IPQESS_REG_RX_DESC1 0xA14
#define IPQESS_RXQ_RFD_BURST_NUM_MASK 0x3F
#define IPQESS_RXQ_RFD_PF_THRESH_MASK 0x1F
#define IPQESS_RXQ_RFD_LOW_THRESH_MASK 0xFFF
#define IPQESS_RXQ_RFD_BURST_NUM_SHIFT 0
#define IPQESS_RXQ_RFD_PF_THRESH_SHIFT 8
#define IPQESS_RXQ_RFD_LOW_THRESH_SHIFT 16

/* RXQ Control Register */
#define IPQESS_REG_RXQ_CTRL 0xA18
#define IPQESS_FIFO_THRESH_TYPE_SHIF 0
#define IPQESS_FIFO_THRESH_128_BYTE 0x0
#define IPQESS_FIFO_THRESH_64_BYTE 0x1
#define IPQESS_RXQ_CTRL_RMV_VLAN 0x00000002
#define IPQESS_RXQ_CTRL_EN_MASK			GENMASK(15, 8)
#define IPQESS_RXQ_CTRL_EN(__qid)		BIT(8 + (__qid))

/* AXI Burst Size Config */
#define IPQESS_REG_AXIW_CTRL_MAXWRSIZE 0xA1C
#define IPQESS_AXIW_MAXWRSIZE_VALUE 0x0

/* Rx Statistics Register */
#define IPQESS_REG_RX_STAT_BYTE_Q(x) (0xA30 + ((x) << 2)) /* x = queue id */
#define IPQESS_REG_RX_STAT_PKT_Q(x) (0xA50 + ((x) << 2)) /* x = queue id */

/* WoL Pattern Length Register */
#define IPQESS_REG_WOL_PATTERN_LEN0 0xC00
#define IPQESS_WOL_PT_LEN_MASK 0xFF
#define IPQESS_WOL_PT0_LEN_SHIFT 0
#define IPQESS_WOL_PT1_LEN_SHIFT 8
#define IPQESS_WOL_PT2_LEN_SHIFT 16
#define IPQESS_WOL_PT3_LEN_SHIFT 24

#define IPQESS_REG_WOL_PATTERN_LEN1 0xC04
#define IPQESS_WOL_PT4_LEN_SHIFT 0
#define IPQESS_WOL_PT5_LEN_SHIFT 8
#define IPQESS_WOL_PT6_LEN_SHIFT 16

/* WoL Control Register */
#define IPQESS_REG_WOL_CTRL 0xC08
#define IPQESS_WOL_WK_EN 0x00000001
#define IPQESS_WOL_MG_EN 0x00000002
#define IPQESS_WOL_PT0_EN 0x00000004
#define IPQESS_WOL_PT1_EN 0x00000008
#define IPQESS_WOL_PT2_EN 0x00000010
#define IPQESS_WOL_PT3_EN 0x00000020
#define IPQESS_WOL_PT4_EN 0x00000040
#define IPQESS_WOL_PT5_EN 0x00000080
#define IPQESS_WOL_PT6_EN 0x00000100

/* MAC Control Register */
#define IPQESS_REG_MAC_CTRL0 0xC20
#define IPQESS_REG_MAC_CTRL1 0xC24

/* WoL Pattern Register */
#define IPQESS_REG_WOL_PATTERN_START 0x5000
#define IPQESS_PATTERN_PART_REG_OFFSET 0x40


/* TX descriptor fields */
#define IPQESS_TPD_HDR_SHIFT 0
#define IPQESS_TPD_PPPOE_EN 0x00000100
#define IPQESS_TPD_IP_CSUM_EN 0x00000200
#define IPQESS_TPD_TCP_CSUM_EN 0x0000400
#define IPQESS_TPD_UDP_CSUM_EN 0x00000800
#define IPQESS_TPD_CUSTOM_CSUM_EN 0x00000C00
#define IPQESS_TPD_LSO_EN 0x00001000
#define IPQESS_TPD_LSO_V2_EN 0x00002000
/* The VLAN_TAGGED bit is not used in the publicly available
 * drivers. The definition has been stolen from the Atheros
 * 'alx' driver (drivers/net/ethernet/atheros/alx/hw.h). It
 * seems that it has the same meaning in regard to the EDMA
 * hardware.
 */
#define IPQESS_TPD_VLAN_TAGGED 0x00004000
#define IPQESS_TPD_IPV4_EN 0x00010000
#define IPQESS_TPD_MSS_MASK 0x1FFF
#define IPQESS_TPD_MSS_SHIFT 18
#define IPQESS_TPD_CUSTOM_CSUM_SHIFT 18

/* RRD descriptor fields */
#define IPQESS_RRD_NUM_RFD_MASK 0x000F
#define IPQESS_RRD_PKT_SIZE_MASK 0x3FFF
#define IPQESS_RRD_SRC_PORT_NUM_MASK 0x4000
#define IPQESS_RRD_SVLAN 0x8000
#define IPQESS_RRD_FLOW_COOKIE_MASK 0x07FF;

#define IPQESS_RRD_PKT_SIZE_MASK 0x3FFF
#define IPQESS_RRD_CSUM_FAIL_MASK 0xC000
#define IPQESS_RRD_CVLAN 0x0001
#define IPQESS_RRD_DESC_VALID 0x8000

#define IPQESS_RRD_PRIORITY_SHIFT 4
#define IPQESS_RRD_PRIORITY_MASK 0x7
#define IPQESS_RRD_PORT_TYPE_SHIFT 7
#define IPQESS_RRD_PORT_TYPE_MASK 0x1F

#endif

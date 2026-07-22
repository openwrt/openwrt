/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (c) 2016, 2018-2021, The Linux Foundation. All rights reserved.
 */

#ifndef __QCA_EDMA_H__
#define __QCA_EDMA_H__

#include <linux/dsa/oob.h>
#include <linux/etherdevice.h>
#include <linux/netdevice.h>
#include <linux/platform_device.h>
#include <net/page_pool/helpers.h>

#define EDMA_HW_RESET_ID "edma_rst"

/* Global / control registers */
#define EDMA_REG_MAS_CTRL 0x0
#define EDMA_REG_PORT_CTRL 0x4
#define EDMA_REG_RXDESC2FILL_MAP_0 0x18
#define EDMA_REG_RXDESC2FILL_MAP_1 0x1c
#define EDMA_REG_DMAR_CTRL 0x48
#define EDMA_REG_AXIW_CTRL 0x50
#define EDMA_REG_MISC_INT_STAT 0x5c
#define EDMA_REG_MISC_INT_MASK 0x60

#define EDMA_PORT_PAD_EN 0x1
#define EDMA_PORT_EDMA_EN 0x2

#define EDMA_DMAR_REQ_PRI_MASK 0x7
#define EDMA_DMAR_REQ_PRI_SHIFT 0
#define EDMA_DMAR_BURST_LEN_MASK 0x1
#define EDMA_DMAR_BURST_LEN_SHIFT 3
#define EDMA_DMAR_TXDATA_NUM_MASK 0x1f
#define EDMA_DMAR_TXDATA_NUM_SHIFT 4
#define EDMA_DMAR_TXDESC_NUM_MASK 0x7
#define EDMA_DMAR_TXDESC_NUM_SHIFT 9
#define EDMA_DMAR_RXFILL_NUM_MASK 0x7
#define EDMA_DMAR_RXFILL_NUM_SHIFT 12

#define EDMA_DMAR_REQ_PRI_SET(x) \
	(((x)&EDMA_DMAR_REQ_PRI_MASK) << EDMA_DMAR_REQ_PRI_SHIFT)
#define EDMA_DMAR_TXDATA_NUM_SET(x) \
	(((x)&EDMA_DMAR_TXDATA_NUM_MASK) << EDMA_DMAR_TXDATA_NUM_SHIFT)
#define EDMA_DMAR_TXDESC_NUM_SET(x) \
	(((x)&EDMA_DMAR_TXDESC_NUM_MASK) << EDMA_DMAR_TXDESC_NUM_SHIFT)
#define EDMA_DMAR_RXFILL_NUM_SET(x) \
	(((x)&EDMA_DMAR_RXFILL_NUM_MASK) << EDMA_DMAR_RXFILL_NUM_SHIFT)
#define EDMA_DMAR_BURST_LEN_SET(x) \
	(((x)&EDMA_DMAR_BURST_LEN_MASK) << EDMA_DMAR_BURST_LEN_SHIFT)

#define EDMA_AXIW_MAX_WR_SIZE_EN 0x400

/* TX descriptor ring registers */
#define EDMA_REG_TXDESC_BA(n) (0x1000 + (0x1000 * (n)))
#define EDMA_REG_TXDESC_PROD_IDX(n) (0x1004 + (0x1000 * (n)))
#define EDMA_REG_TXDESC_CONS_IDX(n) (0x1008 + (0x1000 * (n)))
#define EDMA_REG_TXDESC_RING_SIZE(n) (0x100c + (0x1000 * (n)))
#define EDMA_REG_TXDESC_CTRL(n) (0x1010 + (0x1000 * (n)))

#define EDMA_TXDESC_PROD_IDX_MASK 0xffff
#define EDMA_TXDESC_CONS_IDX_MASK 0xffff
#define EDMA_TXDESC_RING_SIZE_MASK 0xffff
#define EDMA_TXDESC_TX_EN 0x1

/* TX completion ring registers */
#define EDMA_REG_TXCMPL_BA(b, n)        ((b) + (0x1000 * (n)))
#define EDMA_REG_TXCMPL_PROD_IDX(b, n)  ((b) + 0x004 + (0x1000 * (n)))
#define EDMA_REG_TXCMPL_CONS_IDX(b, n)  ((b) + 0x008 + (0x1000 * (n)))
#define EDMA_REG_TXCMPL_RING_SIZE(b, n) ((b) + 0x00c + (0x1000 * (n)))
#define EDMA_REG_TXCMPL_CTRL(b, n)      ((b) + 0x014 + (0x1000 * (n)))

#define EDMA_TXCMPL_PROD_IDX_MASK 0xffff
#define EDMA_TXCMPL_CONS_IDX_MASK 0xffff
#define EDMA_TXCMPL_RETMODE_OPAQUE 0x0

/* TX interrupt registers */
#define EDMA_REG_TX_INT_STAT(b, n)  ((b) + (0x1000 * (n)))
#define EDMA_REG_TX_INT_MASK(b, n)  ((b) + 0x004 + (0x1000 * (n)))
#define EDMA_REG_TX_MOD_TIMER(b, n) ((b) + 0x008 + (0x1000 * (n)))
#define EDMA_REG_TX_INT_CTRL(b, n)  ((b) + 0x00c + (0x1000 * (n)))

#define EDMA_TX_INT_MASK 0x3
#define EDMA_TX_MOD_TIMER 150

/* RX fill ring registers */
#define EDMA_REG_RXFILL_BA(n) (0x29000 + (0x1000 * (n)))
#define EDMA_REG_RXFILL_PROD_IDX(n) (0x29004 + (0x1000 * (n)))
#define EDMA_REG_RXFILL_CONS_IDX(n) (0x29008 + (0x1000 * (n)))
#define EDMA_REG_RXFILL_RING_SIZE(n) (0x2900c + (0x1000 * (n)))
#define EDMA_REG_RXFILL_RING_EN(n) (0x2901c + (0x1000 * (n)))

#define EDMA_RXFILL_PROD_IDX_MASK 0xffff
#define EDMA_RXFILL_CONS_IDX_MASK 0xffff
#define EDMA_RXFILL_RING_SIZE_MASK 0xffff
#define EDMA_RXFILL_BUF_SIZE_MASK 0x3fff
#define EDMA_RXFILL_RING_EN 0x1
#define EDMA_RXFILL_INT_MASK 0x1

/* RX fill ring interrupt registers */
#define EDMA_REG_RXFILL_INT_STAT(n) (0x31000 + (0x1000 * (n)))
#define EDMA_REG_RXFILL_INT_MASK(n) (0x31004 + (0x1000 * (n)))

/* RX descriptor ring registers */
#define EDMA_REG_RXDESC_BA(n) (0x39000 + (0x1000 * (n)))
#define EDMA_REG_RXDESC_PROD_IDX(n) (0x39004 + (0x1000 * (n)))
#define EDMA_REG_RXDESC_CONS_IDX(n) (0x39008 + (0x1000 * (n)))
#define EDMA_REG_RXDESC_RING_SIZE(n) (0x3900c + (0x1000 * (n)))
#define EDMA_REG_RXDESC_CTRL(n) (0x39018 + (0x1000 * (n)))

#define EDMA_RXDESC_PROD_IDX_MASK 0xffff
#define EDMA_RXDESC_CONS_IDX_MASK 0xffff
#define EDMA_RXDESC_RING_SIZE_MASK 0xffff
#define EDMA_RXDESC_PL_OFFSET_MASK 0x1ff
#define EDMA_RXDESC_PL_OFFSET_SHIFT 16
#define EDMA_RXDESC_RX_EN 0x1
#define EDMA_RXDESC_PACKET_LEN_MASK 0x3fff

/* RX descriptor ring interrupt registers */
#define EDMA_REG_RXDESC_INT_STAT(n) (0x49000 + (0x1000 * (n)))
#define EDMA_REG_RXDESC_INT_MASK(n) (0x49004 + (0x1000 * (n)))
#define EDMA_REG_RX_MOD_TIMER(n) (0x49008 + (0x1000 * (n)))
#define EDMA_REG_RX_INT_CTRL(n) (0x4900c + (0x1000 * (n)))

#define EDMA_RXDESC_INT_MASK_PKT_INT 0x1
#define EDMA_RX_MOD_TIMER_INIT 1000

/* QID to ring mapping */
#define EDMA_QID2RID_TABLE_MEM(q) (0x5a000 + (0x4 * (q)))

/* TXDESC to TXCMPL ring mapping */
#define EDMA_REG_TXDESC2CMPL_MAP(n) (0x0c + 0x4 * (n))

/* Misc error interrupt masks */
#define EDMA_MISC_AXI_RD_ERR_MASK_EN 0x1
#define EDMA_MISC_AXI_WR_ERR_MASK_EN 0x2
#define EDMA_MISC_RX_DESC_FIFO_FULL_MASK_EN 0x4
#define EDMA_MISC_RX_ERR_BUF_SIZE_MASK_EN 0x8
#define EDMA_MISC_TX_SRAM_FULL_MASK_EN 0x10
#define EDMA_MISC_TX_CMPL_BUF_FULL_MASK_EN 0x20
#define EDMA_MISC_DATA_LEN_ERR_MASK_EN 0x40
#define EDMA_MISC_TX_TIMEOUT_MASK_EN 0x80

/* Sizes and ring configuration */
#define EDMA_RX_BUFFER_SIZE 1984
#define EDMA_RX_PREHDR_SIZE (sizeof(struct edma_rx_preheader))
#define EDMA_TX_PREHDR_SIZE (sizeof(struct edma_tx_preheader))
#define EDMA_TX_RING_SIZE 128
#define EDMA_RX_RING_SIZE 2048
#define EDMA_TX_RING_THRESH 16

/* Descriptor accessors */
#define EDMA_GET_DESC(R, i, type) (&(((type *)((R)->desc))[i]))
#define EDMA_RXFILL_DESC(R, i) EDMA_GET_DESC(R, i, struct edma_rxfill_desc)
#define EDMA_RXDESC_DESC(R, i) EDMA_GET_DESC(R, i, struct edma_rxdesc)
#define EDMA_TXDESC_DESC(R, i) EDMA_GET_DESC(R, i, struct edma_txdesc)
#define EDMA_TXCMPL_DESC(R, i) EDMA_GET_DESC(R, i, struct edma_txcmpl)

/* TX descriptor fields */
#define EDMA_TXDESC_PREHEADER_SHIFT 29
#define EDMA_TXDESC_DATA_OFFSET_SHIFT 16
#define EDMA_TXDESC_DATA_OFFSET_MASK 0xff
#define EDMA_TXDESC_DATA_LENGTH_MASK 0xffff

/* Preheader fields */
#define EDMA_DST_PORT_TYPE 0x20
#define EDMA_DST_PORT_ID_MASK 0x1f
#define EDMA_SRC_PORT_MASK 0x0fff
#define EDMA_PREHDR_DSTINFO_PORTID_IND 0x20
#define EDMA_RXPH_SRC_INFO_TYPE_GET(rxph) (((rxph)->src_info >> 8) & 0xf0)

struct edma_txdesc {
	u32 buffer_addr;
	u32 word1;
};

struct edma_txcmpl {
	u32 buffer_addr;
	u32 status;
};

struct edma_rxdesc {
	u32 buffer_addr;
	u32 status;
};

struct edma_rxfill_desc {
	u32 buffer_addr;
	u32 word1;
};

struct edma_tx_preheader {
	u32 opaque;
	u16 src_info;
	u16 dst_info;
	u32 tx_pre2;
	u32 tx_pre3;
	u32 tx_pre4;
	u32 tx_pre5;
	u32 tx_pre6;
	u32 tx_pre7;
};

struct edma_rx_preheader {
	u32 opaque;
	u16 src_info;
	u16 dst_info;
	u32 rx_pre2;
	u32 rx_pre3;
	u32 rx_pre4;
	u32 rx_pre5;
	u32 rx_pre6;
	u32 rx_pre7;
};

struct edma_soc_data {
	u32 txcmpl_base;
	u32 tx_int_base;
	u32 misc_int_mask;
	u8 txdesc_ring;
	u8 txcmpl_ring;
	u8 rxfill_ring;
	u8 rxdesc_ring;
	u8 tx_min_size;
	bool burst_enable;
	bool axiw_enable;
};

struct edma_ring {
	void *desc;
	dma_addr_t dma;
	u16 count;
	struct sk_buff **skb_store;
};

struct edma_priv {
	const struct edma_soc_data *soc;
	struct napi_struct tx_napi;
	struct napi_struct rx_napi;
	struct net_device *netdev;
	struct platform_device *pdev;
	struct regmap *regmap;
	struct reset_control *rst;
	struct page_pool *page_pool;

	struct edma_ring txdesc_ring;
	struct edma_ring txcmpl_ring;
	struct edma_ring rxfill_ring;
	struct edma_ring rxdesc_ring;

	spinlock_t tx_lock;

	int txcmpl_irq;
	int rxfill_irq;
	int rxdesc_irq;
	int misc_irq;
};

#endif /* __QCA_EDMA_H__ */

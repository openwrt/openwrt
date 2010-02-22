/*
 * drivers/net/ubi32-eth.h
 *   Ubicom32 ethernet TIO interface driver definitions.
 *
 * (C) Copyright 2009, Ubicom, Inc.
 *
 * This file is part of the Ubicom32 Linux Kernel Port.
 *
 * The Ubicom32 Linux Kernel Port is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 2 of the
 * License, or (at your option) any later version.
 *
 * The Ubicom32 Linux Kernel Port is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 * the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Ubicom32 Linux Kernel Port.  If not,
 * see <http://www.gnu.org/licenses/>.
 *
 * Ubicom32 implementation derived from (with many thanks):
 *   arch/m68knommu
 *   arch/blackfin
 *   arch/parisc
 */
#ifndef _UBI32_ETH_H
#define _UBI32_ETH_H

#include <asm/devtree.h>

#define UBI32_ETH_NUM_OF_DEVICES 2

/*
 * Number of bytes trashed beyond the packet data.
 */
#define UBI32_ETH_TRASHED_MEMORY	(CACHE_LINE_SIZE + ETH_HLEN - 1)

/*
 * Linux already reserves NET_SKB_PAD bytes of headroom in each sk_buff.
 * We want to be able to reserve at least one cache line to align Ethernet
 * and IP header to cache line.
 * Note that the TIO expects a CACHE_LINE_SIZE - ETH_HLEN aligned Ethernet
 * header, while satisfies NET_IP_ALIGN (= 2) automatically.
 * (NET_SKB_PAD is 16, NET_IP_ALIGN is 2, CACHE_LINE_SIZE is 32).
 * You can add more space by making UBI32_ETH_RESERVE_EXTRA != 0.
 */
#define UBI32_ETH_RESERVE_EXTRA (1 * CACHE_LINE_SIZE)
#define UBI32_ETH_RESERVE_SPACE	(UBI32_ETH_RESERVE_EXTRA + CACHE_LINE_SIZE)

struct ubi32_eth_dma_desc {
	volatile void 	*data_pointer;	/* pointer to the buffer */
	volatile u16 	buffer_len;	/* the buffer size */
	volatile u16	data_len;	/* actual frame length */
	volatile u32	status;		/* bit0: status to be update by VP; bit[31:1] time stamp */
};

#define TX_DMA_RING_SIZE (1<<8)
#define TX_DMA_RING_MASK (TX_DMA_RING_SIZE - 1)
#define RX_DMA_RING_SIZE (1<<8)
#define RX_DMA_RING_MASK (RX_DMA_RING_SIZE - 1)

#define RX_DMA_MAX_QUEUE_SIZE (RX_DMA_RING_SIZE - 1)	/* no more than (RX_DMA_RING_SIZE - 1) */
#define RX_MAX_PKT_SIZE (ETH_DATA_LEN + ETH_HLEN + VLAN_HLEN)
#define RX_MIN_PKT_SIZE	ETH_ZLEN
#define RX_BUF_SIZE (RX_MAX_PKT_SIZE + VLAN_HLEN)	/* allow double VLAN tag */

#define UBI32_ETH_VP_TX_TIMEOUT (10*HZ)

struct ubi32_eth_vp_stats {
	u32	rx_alloc_err;
	u32	tx_q_full_cnt;
	u32	rx_q_full_cnt;
	u32	rx_throttle;
};

struct ubi32_eth_private {
	struct net_device *dev;
	struct ubi32_eth_vp_stats vp_stats;
	spinlock_t lock;
#ifdef UBICOM32_USE_NAPI
	struct napi_struct napi;
#else
	struct tasklet_struct tsk;
#endif
	struct ethtionode *regs;
	u16	rx_tail;
	u16	tx_tail;
	u32	vp_int_bit;
};

struct ethtionode {
	struct devtree_node dn;
	volatile u16	command;
	volatile u16	status;
	volatile u16	int_mask;	/* interrupt mask */
	volatile u16	int_status;	/* interrupt mask */
	volatile u16	tx_in;		/* owned by driver */
	volatile u16	tx_out;		/* owned by vp */
	volatile u16	rx_in;		/* owned by driver */
	volatile u16	rx_out;		/* owned by vp */
	u16		tx_sz;		/* owned by driver */
	u16		rx_sz;		/* owned by driver */
	struct ubi32_eth_dma_desc **tx_dma_ring;
	struct ubi32_eth_dma_desc **rx_dma_ring;
};

#define UBI32_ETH_VP_STATUS_LINK	(1<<0)
#define UBI32_ETH_VP_STATUS_SPEED100	(0x1<<1)
#define UBI32_ETH_VP_STATUS_SPEED1000	(0x1<<2)
#define UBI32_ETH_VP_STATUS_DUPLEX	(0x1<<3)
#define UBI32_ETH_VP_STATUS_FLOW_CTRL	(0x1<<4)

#define UBI32_ETH_VP_STATUS_RX_STATE	(0x1<<5)
#define UBI32_ETH_VP_STATUS_TX_STATE	(0x1<<6)

#define UBI32_ETH_VP_STATUS_TX_Q_FULL	(1<<8)

#define UBI32_ETH_VP_INT_RX	(1<<0)
#define UBI32_ETH_VP_INT_TX	(1<<1)

#define UBI32_ETH_VP_CMD_RX_ENABLE	(1<<0)
#define UBI32_ETH_VP_CMD_TX_ENABLE	(1<<1)

#define UBI32_ETH_VP_RX_OK		(1<<0)
#define UBI32_ETH_VP_TX_OK		(1<<1)

#define UBI32_TX_BOUND		TX_DMA_RING_SIZE
#define UBI32_RX_BOUND		64
#define UBI32_ETH_NAPI_WEIGHT	64		/* for GigE */
#endif

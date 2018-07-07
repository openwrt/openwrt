/*
 *  Realtek RE865X-style built-in ethernet mac and switch driver
 *
 *  Copyright (C) 2017 Weijie Gao <hackpascal@gmail.com>
 *
 *  Based on Realtek RE865X asic driver
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#ifndef __RE865X_H
#define __RE865X_H

#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/random.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/ethtool.h>
#include <linux/etherdevice.h>
#include <linux/if_vlan.h>
#include <linux/phy.h>
#include <linux/skbuff.h>
#include <linux/dma-mapping.h>
#include <linux/workqueue.h>
#include <linux/bitops.h>
#include <linux/ip.h>
#include <linux/platform_data/re865x.h>

#include <asm/div64.h>

#include <asm/mach-realtek/realtek.h>

#include "rtl865x.h"


#define DRV_NAME				"re865x"
#define DRV_VERSION				"1.0.0"

#define RE865X_RX_RINGS_SIZE			128
#define RE865X_TX_RINGS_SIZE			128

#define RE865X_MBUF_CLUSTER_SIZE		2048
#define RE865X_BUS_BURST_SIZE			32

#define RE865X_NAPI_WEIGHT			32
#define RE865X_OOM_REFILL			(1 + HZ/10)

#define RE865X_DMA_MIN_SIZE			64

struct pktHdr;

struct mBuf {
	u32				m_next;
	u32				m_pkthdr;	/* Points to the pkthdr structure */
	u16				m_len;		/* data bytes used in this cluster */
	u16				m_flags;	/* mbuf flags; see below */
	u32				m_data;		/* location of data in the cluster */
	u32				m_extbuf;	/* start of buffer */
	u16				m_extsize;	/* sizeof the cluster */
	u8				m_reserved[2];	/* padding */

	union {
		struct {
			struct sk_buff	*skb;
			u32		unused;
		} tx;
		struct {
			void		*buf;
			dma_addr_t	dma_addr;
		} rx;
	};
};

#define MBUF_FLAG_FREE			0x00	/* Free. Not occupied. should be on free list */
#define MBUF_FLAG_USED			0x80	/* Buffer is occupied */
#define MBUF_FLAG_EXT			0x10	/* has associated with an external cluster, this is always set. */
#define MBUF_FLAG_PKTHDR		0x08	/* is the 1st mbuf of this packet */
#define MBUF_FLAG_EOR			0x04	/* is the last mbuf of this packet. Set only by ASIC */

#define MBUF_DESC_SIZE	roundup(sizeof(struct mBuf), L1_CACHE_BYTES)

struct pktHdr {
	u32		ph_mbuf;			/* 1st mbuf of this pkt */

	u16		ph_len;				/* total packet length */

	u16		ph_reserved1:		1;	/* reserved */
	u16		ph_queueId:		3;	/* bit 2~0: Queue ID */
	u16		ph_extPortList:		4;	/* dest extension port list. must be 0 for TX */
	u16		ph_reserved2:		3;	/* reserved */
	u16		ph_hwFwd:		1;	/* hwFwd - copy from HSA bit 200 */
	u16		ph_isOriginal:		1;	/* isOriginal - DP included cpu port or more than one ext port */
	u16		ph_l2Trans:		1;	/* l2Trans - copy from HSA bit 129 */
	u16		ph_srcExtPortNum:	2;	/* Both in RX & TX. Source extension port number. */

	u16		ph_type:		3;
	u16		ph_vlanTagged:		1;	/* the tag status after ALE */
	u16		ph_LLCTagged:		1;	/* the tag status after ALE */
	u16		ph_pppeTagged:		1;	/* the tag status after ALE */
	u16		ph_pppoeIdx:		3;
	u16		ph_linkID:		7;	/* for WLAN WDS multiple tunnel */

	u16		ph_reason;			/* indicates wht the packet is received by CPU */

	u16		ph_flags;			/* NEW: Packet header status bits */

	u8		ph_orgtos;			/* RX: original TOS of IP header's value before remarking, TX: undefined */
	u8		ph_portlist;			/* RX: source port number, TX: destination portmask */

	u16		ph_vlanId_resv:		1;
	u16		ph_txPriority:		3;
	u16		ph_vlanId:		12;

	union {
		u16	value;				/* RX: bit 15: Reserved, bit14~12: Original Priority, bit 11~0: Original VLAN ID */
							/* TX: bit 15~6: Reserved, bit 5~0: Per Port Tag mask setting for TX(bit 5:MII, bit 4~0: Physical Port) */
		struct {
			/* RX: bit 15: Reserved, bit14~12: Original Priority, bit 11~0: Original VLAN ID */
			u16	reserved:1;
			u16	rxPktPriority:3;	/* Rx packet's original priority */
			u16	sVlanId:12;		/* Source (Original) VLAN ID */
		} rx;

		struct {
			/* TX: bit 15~6: Reserved, bit 5~0: Per Port Tag mask setting for TX(bit 5:MII, bit 4~0: Physical Port) */
			u16	reserved:10;
			u16	txCVlanTagAutoAdd:6;	/* BitMask to indicate the port which would need to add VLAN tag */
		} tx;
	} ph_flags2;
};

#define PKTHDR_TYPE_ETHERNET		0
#define PKTHDR_TYPE_PPTP		1
#define PKTHDR_TYPE_IP			2
#define PKTHDR_TYPE_ICMP		3
#define PKTHDR_TYPE_IGMP		4
#define PKTHDR_TYPE_TCP			5
#define PKTHDR_TYPE_UDP			6
#define PKTHDR_TYPE_IPV6		7

#define PKTHDR_FLAG_FREE		0x0000		/* Free. Not occupied. should be on free list */
#define PKTHDR_FLAG_USED		0x8000
#define PKTHDR_FLAG_CPU_OWNED		0x4000
#define PKTHDR_FLAG_PKT_INCOMING	0x1000		/* Incoming: packet is incoming */
#define PKTHDR_FLAG_PKT_OUTGOING	0x0800		/* Outgoing: packet is outgoing */
#define PKTHDR_FLAG_PKT_BCAST		0x0100		/* send/received as link-level broadcast */
#define PKTHDR_FLAG_PKT_MCAST		0x0080		/* send/received as link-level multicast */
#define PKTHDR_FLAG_BRIDGING		0x0040		/* when PKTHDR_HWLOOKUP is on. 1: Hardware assist to do L2 bridging only, 0:hardware assist to do NAPT*/
#define PKTHDR_FLAG_HWLOOKUP		0x0020		/* valid when ph_extPortList!=0. 1:Hardware table lookup assistance*/
#define PKTHDR_FLAG_PPPOE_AUTOADD	0x0004		/* PPPoE header auto-add */
#define PKTHDR_FLAG_CSUM_IP		0x0002		/* Outgoing: IP header cksum offload to ASIC */
#define PKTHDR_FLAG_CSUM_TCPUDP		0x0001		/* Outgoing:TCP or UDP cksum offload to ASIC */

#define PKDHDR_DESC_SIZE	roundup(sizeof(struct pktHdr), L1_CACHE_BYTES)


#define ALL_PORT_MASK			0x3F
#define AP_MODE_PORT_MASK		0x30

struct re865x_ring {
	unsigned int		num_descs;

	u32			*pkthdr_descs_cpu;
	dma_addr_t		pkthdr_descs_dma;

	u32			*mbuf_descs_cpu;
	dma_addr_t		mbuf_descs_dma;

	u8			*pkdhdrs_cpu;
	dma_addr_t		pkdhdrs_dma;
	struct pktHdr		**pkthdrs;

	u8			*mbufs_cpu;
	dma_addr_t		mbufs_dma;
	struct mBuf		**mbufs;

	u64	top;
	u64	bottom;
};

struct re865x {
	void __iomem		*nic_base;
	void __iomem		*sw_base;
	void __iomem		*swtbl_base;

	spinlock_t		lock;
	struct platform_device	*pdev;
	struct net_device	*dev;
	struct napi_struct	napi;
	u32			msg_enable;

	struct re865x_ring	rx_ring;
	struct re865x_ring	tx_ring;
	struct re865x_ring	tx_ring_dummy;

	unsigned int		rx_buf_size;

	struct timer_list	oom_timer;

	struct rtl865x		sw;
};

#define NIC_CONTROL_REG						0x00

#define NIC_CONTROL_TX_CMD					BIT(31)
#define NIC_CONTROL_RX_CMD					BIT(30)

#define NIC_CONTROL_BURST_SIZE_SHIFT				28
#define NIC_CONTROL_BURST_SIZE_MASK				0x3
#define NIC_CONTROL_BURST_SIZE_32B				0
#define NIC_CONTROL_BURST_SIZE_64B				1
#define NIC_CONTROL_BURST_SIZE_128B				2

#define NIC_CONTROL_EXTMBUF_CLUSTER_SIZE_SHIFT			24
#define NIC_CONTROL_EXTMBUF_CLUSTER_SIZE_MASK			0x7
#define NIC_CONTROL_EXTMBUF_CLUSTER_SIZE_128B			0
#define NIC_CONTROL_EXTMBUF_CLUSTER_SIZE_256B			1
#define NIC_CONTROL_EXTMBUF_CLUSTER_SIZE_512B			2
#define NIC_CONTROL_EXTMBUF_CLUSTER_SIZE_1KB			3
#define NIC_CONTROL_EXTMBUF_CLUSTER_SIZE_2KB			4

#define NIC_CONTROL_TX_EN					BIT(23)
#define NIC_CONTROL_SOFT_RESET					BIT(22)
#define NIC_CONTROL_TX_STOP					BIT(21)
#define NIC_CONTROL_SOFT_INT_SET				BIT(20)
#define NIC_CONTROL_LOOPBACK					BIT(19)

#define NIC_CONTROL_LOOPBACK_CLOCK_SHIFT			18
#define NIC_CONTROL_LOOPBACK_CLOCK_MASK				0x1
#define NIC_CONTROL_LOOPBACK_CLOCK_100MHZ			0
#define NIC_CONTROL_LOOPBACK_CLOCK_10MHZ			1

#define NIC_CONTROL_MITIGATION_TIMER1				BIT(17)
#define NIC_CONTROL_EXCLUDE_CRC_LENGTH				BIT(16)

#define NIC_CONTROL_RX_SHIFT_SHIFT				0
#define NIC_CONTROL_RX_SHIFT_MASK				0xff


#define NIC_RX_PKTHDR_DESC_0_REG				0x04
#define NIC_RX_PKTHDR_DESC_1_REG				0x08
#define NIC_RX_PKTHDR_DESC_2_REG				0x0c
#define NIC_RX_PKTHDR_DESC_3_REG				0x10
#define NIC_RX_PKTHDR_DESC_4_REG				0x14
#define NIC_RX_PKTHDR_DESC_5_REG				0x18
#define NIC_RX_PKTHDR_DESC_REG(x)				(NIC_RX_PKTHDR_DESC_0_REG + ((x) << 2))

#define NIC_RX_MBUF_DESC_REG					0x1c

#define NIC_TX_PKTHDR_DESC_0_REG				0x20
#define NIC_TX_PKTHDR_DESC_1_REG				0x24
#define NIC_TX_PKTHDR_DESC_REG(x)				(NIC_TX_PKTHDR_DESC_0_REG + ((x) << 2))

#define NIC_DESC_OWN						BIT(0)
#define NIC_DESC_WRAP						BIT(1)


#define NIC_INTERRUPT_MASK_REG					0x28
#define NIC_INTERRUPT_STATUS_REG				0x2c

#define NIC_INTERRUPT_LINK_CHANGE				BIT(31)
#define NIC_INTERRUPT_RX_DESC_5_ERROR				BIT(30)
#define NIC_INTERRUPT_RX_DESC_4_ERROR				BIT(29)
#define NIC_INTERRUPT_RX_DESC_3_ERROR				BIT(28)
#define NIC_INTERRUPT_RX_DESC_2_ERROR				BIT(27)
#define NIC_INTERRUPT_RX_DESC_1_ERROR				BIT(26)
#define NIC_INTERRUPT_RX_DESC_0_ERROR				BIT(25)
#define NIC_INTERRUPT_TX_DESC_1_ERROR				BIT(24)
#define NIC_INTERRUPT_TX_DESC_0_ERROR				BIT(23)
#define NIC_INTERRUPT_RX_DESC_5_EMPTY				BIT(22)
#define NIC_INTERRUPT_RX_DESC_4_EMPTY				BIT(21)
#define NIC_INTERRUPT_RX_DESC_3_EMPTY				BIT(20)
#define NIC_INTERRUPT_RX_DESC_2_EMPTY				BIT(19)
#define NIC_INTERRUPT_RX_DESC_1_EMPTY				BIT(18)
#define NIC_INTERRUPT_RX_DESC_0_EMPTY				BIT(17)
#define NIC_INTERRUPT_RX_MBUF_DESC_EMPTY			BIT(16)
#define NIC_INTERRUPT_TX_DESC_1_DONE				BIT(10)
#define NIC_INTERRUPT_TX_DESC_0_DONE				BIT(9)
#define NIC_INTERRUPT_RX_DESC_5_DONE				BIT(8)
#define NIC_INTERRUPT_RX_DESC_4_DONE				BIT(7)
#define NIC_INTERRUPT_RX_DESC_3_DONE				BIT(6)
#define NIC_INTERRUPT_RX_DESC_2_DONE				BIT(5)
#define NIC_INTERRUPT_RX_DESC_1_DONE				BIT(4)
#define NIC_INTERRUPT_RX_DESC_0_DONE				BIT(3)
#define NIC_INTERRUPT_TX_ALL_DESC_1_DONE			BIT(2)
#define NIC_INTERRUPT_TX_ALL_DESC_0_DONE			BIT(1)

#define INTERRUPT_TX		(NIC_INTERRUPT_TX_ALL_DESC_0_DONE | \
				 NIC_INTERRUPT_TX_DESC_0_DONE)

#define INTERRUPT_RX		(NIC_INTERRUPT_RX_DESC_0_DONE)

#define INTERRUPT_RX_EMPTY	(NIC_INTERRUPT_RX_MBUF_DESC_EMPTY | \
				 NIC_INTERRUPT_RX_DESC_0_EMPTY)

#define INTERRUPT_ERR		(NIC_INTERRUPT_TX_DESC_0_ERROR | \
				 NIC_INTERRUPT_RX_DESC_0_ERROR)

#define INTERRUPT_POLL		(INTERRUPT_TX | INTERRUPT_RX | \
				 INTERRUPT_RX_EMPTY)

#define INTERRUPT_ALL		(INTERRUPT_POLL | INTERRUPT_ERR)
			 

/* helper functions for descriptors */
static inline bool re865x_desc_is_owned(u32 desc)
{
	return desc & NIC_DESC_OWN;
}

static inline bool re865x_desc_set_own(u32 *desc)
{
	return *desc |= NIC_DESC_OWN;
}

static inline bool re865x_desc_clear_own(u32 *desc)
{
	return *desc &= ~NIC_DESC_OWN;
}


/* helper functions for registers */
static inline u32 re865x_reg_read(struct re865x *re, unsigned reg)
{
	return __raw_readl(re->nic_base + reg);
}

static inline void re865x_reg_write(struct re865x *re, unsigned reg, u32 value)
{
	__raw_writel(value, re->nic_base + reg);
}

static inline void re865x_reg_rmw(struct re865x *re, unsigned reg, u32 clear, u32 set)
{
	u32 value;

	value = __raw_readl(re->nic_base + reg);
	value &= ~clear;
	value |= set;
	__raw_writel(value, re->nic_base + reg);
}

/* helper functions for interrupts */
static inline void re865x_interrupt_enable(struct re865x *re)
{
	re865x_reg_write(re, NIC_INTERRUPT_MASK_REG, INTERRUPT_ALL);
}

static inline void re865x_interrupt_disable(struct re865x *re)
{
	re865x_reg_write(re, NIC_INTERRUPT_MASK_REG, 0);
}

static inline u32 re865x_interrupt_status(struct re865x *re)
{
	return re865x_reg_read(re, NIC_INTERRUPT_STATUS_REG);
}

static inline void re865x_interrupt_acknowledge(struct re865x *re, u32 bits)
{
	re865x_reg_write(re, NIC_INTERRUPT_STATUS_REG, bits);
}


/* helper functions for mbuf */
static inline unsigned int re865x_max_cluster_len(unsigned int mtu)
{
	return ETH_HLEN + VLAN_HLEN + mtu + ETH_FCS_LEN;
}

static inline int re865x_buffer_size(struct re865x *re)
{
	return re->rx_buf_size +
	       SKB_DATA_ALIGN(sizeof(struct skb_shared_info));
}


/* helper functions for descriptor rings */
static inline unsigned int re865x_ring_top_to_index(struct re865x_ring *ring)
{
	u32 rem;

	div_u64_rem(ring->top, ring->num_descs, &rem);

	return rem;
}

static inline unsigned int re865x_ring_bottom_to_index(struct re865x_ring *ring)
{
	u32 rem;

	div_u64_rem(ring->bottom, ring->num_descs, &rem);

	return rem;
}

static inline unsigned int re865x_ring_mbuf_to_index(struct re865x_ring *ring, u32 index)
{
	return (ring->pkthdrs[index]->ph_mbuf - ring->mbufs_dma) / MBUF_DESC_SIZE;
}


extern struct ethtool_ops re865x_ethtool_ops;

#endif /* __RE865X_H */

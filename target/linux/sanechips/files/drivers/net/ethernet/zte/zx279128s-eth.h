/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * ZTE zx279128s Ethernet switch and DMA driver: registers and shared state
 *
 * Copyright (C) 2026 Navid Ghahremani <ghahramani.navid@gmail.com>
 */

#ifndef ZX279128S_ETH_H
#define ZX279128S_ETH_H

#include <linux/bitmap.h>
#include <linux/hrtimer.h>
#include <linux/if_ether.h>
#include <linux/mutex.h>
#include <linux/netdevice.h>
#include <linux/notifier.h>
#include <linux/phylink.h>
#include <linux/rhashtable.h>
#include <linux/spinlock.h>
#include <linux/types.h>
#include <linux/workqueue.h>

#define DRV_NAME		"zx279128s-eth"

/*
 * Register offsets from the start of the block (0x92000000). The names follow
 * the vendor modules where they are known. Registers that only appear in the
 * vendor's init sequences, with no known meaning, keep their offsets.
 */

/* Global control */
#define ZX_GLB_RESET		0x000008	/* write 0, then all ones */
#define ZX_GLB_CLK_CTRL		0x040018
#define  ZX_GLB_CLK_CTRL_BITS	GENMASK(1, 0)	/* cleared by the vendor code */
#define ZX_GLB_PORT_CLK_EN	0x04001c	/* bit n: clock of MAC port n */

/* Switch core */
#define ZX_SW_PORT_FILTER(p)	(0x1c0060 + 4 * (p))
#define  ZX_SW_PORT_FILTER_BITS	GENMASK(25, 23)	/* cleared for every port */
#define ZX_SW_FWD_CTRL		0x1cc000
#define  ZX_SW_FWD_CTRL_ON	17

/* Switch port attributes (stock tm.ko "spa") */
#define ZX_SPA_UP_REG_PKT_EN	0x1d4000
#define ZX_SPA_DN_REG_PKT_EN	0x1d4040
#define ZX_SPA_TRAP_DMAC	0x1d41a0	/* 8 bytes per entry */
#define  ZX_SPA_TRAP_DMAC_NUM	2		/* entries the driver uses */
#define ZX_SPA_TRAP_PROTO	0x1d41c0
#define ZX_SPA_TRAP_ETH_TYPE	0x1d41c4	/* 4 x 16 bits, big end first */
#define ZX_SPA_PORT_EN(i)	(0x1d428c + 4 * (i))
#define ZX_SPA_ONU_MAC		0x1d4120	/* 16 x 8 bytes: our MACs, routed */
#define  ZX_SPA_ONU_MAC_NUM	16
#define ZX_SPA_REG_PKT_TCP_ACK	BIT(4)		/* word 2: TCP without payload */

/* MAC ports 0-4 */
#define ZX_MAC_BASE		0x200000
#define ZX_PORT_STRIDE		0x040000
#define ZX_MAC_PORT(p)		(ZX_MAC_BASE + (p) * ZX_PORT_STRIDE)
#define ZX_MAC_CTRL		0x000
#define  ZX_MAC_CTRL_EN		GENMASK(1, 0)	/* receive and transmit */
#define  ZX_MAC_CTRL_FD		BIT(13)		/* full duplex */
#define  ZX_MAC_CTRL_100	BIT(14)		/* 100 Mbit/s, with _MII */
#define  ZX_MAC_CTRL_MII	BIT(15)		/* 10 or 100 Mbit/s */

/* MAC port counters: 32 bits, not cleared by reading */
#define ZX_MAC_TX_HIST(i)	(0x724 + 4 * (i))	/* frame size bins */
#define ZX_MAC_TX_MCAST		0x740
#define ZX_MAC_TX_BCAST		0x744
#define ZX_MAC_TX_OCTETS	0x764
#define ZX_MAC_TX_FRAMES	0x768
#define ZX_MAC_RX_OCTETS	0x784
#define ZX_MAC_RX_BCAST		0x78c
#define ZX_MAC_RX_MCAST		0x790
#define ZX_MAC_RX_HIST(i)	(0x7ac + 4 * (i))
#define ZX_MAC_RX_UCAST		0x7c4
#define ZX_MAC_HIST_BINS	6	/* 64, 65-127, ... 1024 bytes and up */

/* Traffic manager */
#define ZX_TM_BASE		0x340000
#define ZX_TM_CTRL		(ZX_TM_BASE + 0x00)
#define  ZX_TM_CTRL_BIT6	BIT(6)		/* set by the vendor code */
#define ZX_TM_CONFIG_START	(ZX_TM_BASE + 0x04)
#define ZX_TM_TAB0_ADDR		(ZX_TM_BASE + 0xe8)
#define ZX_TM_TAB1_ADDR		(ZX_TM_BASE + 0xec)
#define ZX_TM_BUF_F0		(ZX_TM_BASE + 0xf0)
#define ZX_TM_BUF_START		(ZX_TM_BASE + 0xf4)
#define ZX_TM_BUF_END		(ZX_TM_BASE + 0xf8)
#define ZX_TM_TAB_LIMIT		(ZX_TM_BASE + 0xfc)
#define ZX_TM_INT_STATUS	(ZX_TM_BASE + 0x100)
#define ZX_TM_INT_MASK		(ZX_TM_BASE + 0x104)	/* 1: masked */
#define  ZX_TM_INT_RX		GENMASK_U32(1, 0)		/* frames for the CPU */

/* Traffic manager tables, reached through an indirect access window */
#define ZX_TM_IND_CMD		(ZX_TM_BASE + 0x4014)
#define  ZX_TM_IND_INDEX	GENMASK(21, 0)
#define  ZX_TM_IND_TABLE	GENMASK(26, 22)
#define  ZX_TM_IND_READ		BIT(27)
#define ZX_TM_IND_STATUS	(ZX_TM_BASE + 0x4018)
#define  ZX_TM_IND_READY	BIT(0)
#define ZX_TM_IND_DATA(i)	(ZX_TM_BASE + 0x401c + 4 * (i))
#define ZX_TM_TABLE_QUEUE	0
#define ZX_TM_TABLE_QUEUE_USE	1
#define ZX_TM_TABLE_RED		2
#define ZX_TM_TABLE_SCHED	4

/* Release of consumed RX slots */
#define ZX_RX_REL_CTRL		(ZX_TM_BASE + 0x4064)
#define  ZX_RX_REL_BUSY		BIT(0)
#define ZX_RX_REL_DATA		(ZX_TM_BASE + 0x4068)
#define  ZX_RX_REL_QUEUE	GENMASK(2, 0)
#define  ZX_RX_REL_CLASS	BIT(3)
#define  ZX_RX_REL_COUNT	GENMASK(13, 4)

/* Buffer management unit */
#define ZX_BMU_BASE		(ZX_TM_BASE + 0x8000)
#define ZX_BMU_CTRL		(ZX_BMU_BASE + 0x00)
#define  ZX_BMU_CTRL_EN		BIT(0)
#define ZX_BMU_CFG1		(ZX_BMU_BASE + 0x04)
#define ZX_BMU_CFG2		(ZX_BMU_BASE + 0x08)
#define ZX_BMU_BUF_ID		(ZX_BMU_BASE + 0x0c)
#define  ZX_BMU_BUF_ID_VALID	BIT(31)
#define  ZX_BMU_BUF_ID_BP	GENMASK(15, 0)
#define ZX_BMU_BUF_FREE		(ZX_BMU_BASE + 0x10)
#define ZX_BMU_BUF_ALLOC	(ZX_BMU_BASE + 0x14)
#define  ZX_BMU_BUF_ALLOC_REQ	BIT(0)
#define  ZX_BMU_BUF_ALLOC_BUSY	GENMASK(1, 0)
#define ZX_BMU_FREE_STATUS	(ZX_BMU_BASE + 0xdc)
#define  ZX_BMU_FREE_SLOTS	GENMASK(8, 3)
#define ZX_BMU_THRES1		(ZX_BMU_BASE + 0x48)
#define ZX_BMU_THRES2		(ZX_BMU_BASE + 0x4c)
#define ZX_BMU_LIMIT1		(ZX_BMU_BASE + 0x58)
#define ZX_BMU_LIMIT2		(ZX_BMU_BASE + 0x5c)

/* DMA engine between the switch and the CPU */
#define ZX_DMA_BASE		(ZX_TM_BASE + 0x10000)
#define ZX_DMA_CTRL		(ZX_DMA_BASE + 0x00)
#define  ZX_DMA_CTRL_EN		GENMASK(19, 16)
#define  ZX_DMA_CTRL_BIT21	BIT(21)		/* always set, as in the vendor code */
#define ZX_DMA_RX_IRQ_TIMER	(ZX_DMA_BASE + 0x30)	/* in 20 ns ticks */
#define ZX_DMA_RX_IRQ_FRAMES	(ZX_DMA_BASE + 0x34)
#define ZX_DMA_RING_CFG		(ZX_DMA_BASE + 0x3c)
#define ZX_DMA_AREA50		(ZX_DMA_BASE + 0x50)	/* 1 MiB area, use unknown */
#define ZX_DMA_TX_RING		(ZX_DMA_BASE + 0x60)	/* ring of frames to send */
#define ZX_DMA_TX_START		(ZX_DMA_BASE + 0x64)
#define ZX_DMA_RX_QUEUE_CNT(q)	(ZX_DMA_BASE + 0x100 + ((q) * 4))

/* Packet processor */
#define ZX_PP_RESET		0x380000

/* Bridge block (sbrg), ports numbered with ZX_BRPORT() */
#define ZX_SBRG_PORT_CTRL	0x388004
#define ZX_SBRG_FLUSH_PORTS	GENMASK(15, 8)	/* drop the entries of these */
#define ZX_SBRG_FLUSH		BIT(16)
#define ZX_SBRG_IND_CMD		0x388014	/* 27: read, 26-22: memory, 11-0: entry */
#define ZX_SBRG_IND_READ	BIT(27)
#define ZX_SBRG_IND_DONE	0x388018
#define ZX_SBRG_IND_DATA(i)	(0x38801c + 4 * (i))
#define ZX_SBRG_TABLE_SEL	0x388184	/* source table: 4 RAMs of 1024/256/512 */
#define ZX_SBRG_LEARN		0x3881c0	/* ports that learn source MACs */
#define ZX_SBRG_EGRESS(bp)	(0x3883c0 + 4 * (bp))	/* ports bp may send to */
#define ZX_SBRG_MEM_VLAN	4		/* 1 word per VID */
#define ZX_SBRG_VLAN_VALID	BIT(0)
#define ZX_SBRG_VLAN_UNTAG(bp)	(1 << (2 * (bp) + 1))

/* Packet processor tables, reached through an indirect access window:
 * command (index | table << 22 | read << 27), status bit 0 = idle, data.
 * The classifier (CLA) has 17 data words, the packet modifier (PM) 8.
 */
#define ZX_CLA_BASE		0x38c000
#define ZX_PM_BASE		0x39c000
#define ZX_PP_IND_CMD		0x14
#define ZX_PP_IND_STATUS	0x18
#define ZX_PP_IND_DATA		0x1c
#define ZX_PM_IND_DATA_HI	0x100	/* PM data words 4-7 */
#define ZX_PP_IND_READ		BIT(27)

#define ZX_CLA_EXTRA_INDEX	0	/* header fields each packet type extracts */
#define ZX_CLA_EXTRA_RULE	1
#define ZX_CLA_HASH0		2	/* 256 flow entries, CRC-32 slot */
#define ZX_CLA_HASH1		3	/* 128 flow entries, CRC-32C slot */
#define ZX_CLA_AGING		8	/* hit flags, bank 0 then bank 1 */
#define ZX_PM_FLOW		0	/* rewrite of a flow */
#define ZX_PM_NEXT_HOP		1	/* new destination IP and MAC */
#define ZX_PM_CMD		3
#define ZX_PM_SUB		6
#define ZX_PM_SRC_MAC		12	/* source MAC of each subnet */
#define ZX_PP_SNAT_IP(s)	(0x3a0400 + (s) * 4)	/* source IP of each subnet */

#define ZX_PPE_HASH0_SIZE	256
#define ZX_PPE_HASH_SIZE	(256 + 128)
/*
 * The rewrite index in a classifier entry has 7 bits (ZX_CLA_W0_FLOW), and
 * the stock firmware never used more than 125, so only 127 flows (1-127) can
 * be offloaded at a time. More would reuse the rewrite of another flow.
 */
#define ZX_PPE_FLOWS		128
#define ZX_PPE_NEXT_HOPS	512
#define ZX_PPE_SUBNETS		16
/* IPv4 TCP/UDP: extract index 9 and its rule; hashed with this rule id */
#define ZX_PPE_V4_INDEX		9
#define ZX_PPE_V4_RULE		0x98

/* Word 0 of a classifier hash entry */
#define ZX_CLA_W0_FLOW		GENMASK(31, 25)	/* packet modifier rewrite index */
#define ZX_CLA_W0_FWD		BIT(24)		/* forward to the egress port */
#define ZX_CLA_W0_EGRESS	GENMASK(15, 12)	/* bridge port, ZX_BRPORT() */
#define ZX_CLA_W0_BITS		0x44		/* as in every stock entry */

/* Bridge ports: 0 is the PON side, 1-5 are the switch ports 0-4 */
#define ZX_BRPORT(p)		((p) + 1)

/* DMA buffer pool: 4096 buffers x 2048 bytes = 8 MiB */
#define ZX_NUM_BUFFERS		4096
#define ZX_BUF_SIZE		2048
#define ZX_BUF_POOL_SIZE	(ZX_NUM_BUFFERS * ZX_BUF_SIZE)

/* Free-index tables (written big-endian u16 per U-Boot convention) */
#define ZX_TAB0_ENTRIES		ZX_NUM_BUFFERS
#define ZX_TAB1_ENTRIES		512

/* DMA descriptor rings */
#define ZX_NUM_TX_DESC		1024
#define ZX_TX_QUEUE_LIMIT	256
#define ZX_TX_POLL_US		250	/* see zx_tx_kick() */

/*
 * The RX interrupt comes after this many frames, or this long after the
 * first one.  Larger batches cost less per frame: 500 us receive faster
 * than polling every 1 ms did and still answer sooner.
 */
#define ZX_RX_IRQ_FRAMES	64
#define ZX_RX_IRQ_USECS		500
#define ZX_NUM_RX_DESC		1024
#define ZX_DESC_SIZE		16	/* bytes per descriptor */

/* Layout of the 16 MiB DMA arena */
#define ZX_DMA_ARENA_SIZE	0x1000000
#define ZX_TAB0_OFFSET		0x800000
#define ZX_TAB1_OFFSET		0x808000
#define ZX_RX_OFFSET		0xa00000
#define ZX_AREA50_OFFSET	0xb00000
#define ZX_TX_OFFSET		0xc00000
/* RX ring of queue q at ZX_RX_OFFSET + q * 16 KiB (1024 descriptors) */
#define ZX_RX_QUEUE_STRIDE	0x4000
#define ZX_RX_QUEUES		8
#define ZX_DMA_TX_DONE		(ZX_DMA_BASE + 0x68)

struct zx_eth_adapter;

/* One net_device per switch port: ports 0-3 LAN, port 4 WAN */
#define ZX_NUM_PORTS		5
/* TM egress queue of switch port p (as in the vendor driver) */
#define ZX_PORT_TX_QUEUE(p)	(40 + (p))
/* MACs the Linux bridge moved away from a switch port, queued for removal */
#define ZX_SW_FDB_QUEUE		16

/* The MAC counters the driver keeps, see zx_mib_regs[] */
enum zx_mib {
	ZX_MIB_TX_OCTETS,
	ZX_MIB_TX_FRAMES,
	ZX_MIB_TX_MCAST,
	ZX_MIB_TX_BCAST,
	ZX_MIB_TX_HIST,
	ZX_MIB_RX_OCTETS = ZX_MIB_TX_HIST + ZX_MAC_HIST_BINS,
	ZX_MIB_RX_UCAST,
	ZX_MIB_RX_MCAST,
	ZX_MIB_RX_BCAST,
	ZX_MIB_RX_HIST,
	ZX_MIB_NUM = ZX_MIB_RX_HIST + ZX_MAC_HIST_BINS,
};

struct zx_eth_priv {
	struct net_device	*netdev;
	struct zx_eth_adapter	*adapter;
	int			port;
	struct phylink		*phylink;
	struct phylink_config	phylink_config;

	/* MAC counters, summed up into 64 bits (under adapter->mib_lock) */
	u64			mib[ZX_MIB_NUM];
	u32			mib_last[ZX_MIB_NUM];	/* last register values */
};

struct zx_eth_adapter {
	struct device		*dev;
	void __iomem		*base;
	struct napi_struct	napi;
	struct hrtimer		tx_timer;

	struct net_device	*ports[ZX_NUM_PORTS];
	int			open_count;
	int			irq;

	/* DMA arena remains allocated across down/up */
	void			*buffer_pool;
	dma_addr_t		buf_pool_dma;
	void			*tx_ring;
	dma_addr_t		tx_ring_dma;
	void			*rx_ring;
	dma_addr_t		rx_ring_dma;
	void			*tab0;
	dma_addr_t		tab0_dma;
	void			*tab1;
	dma_addr_t		tab1_dma;

	u32			tx_cur_idx;
	u32			tx_pending;
	u32			tx_complete_cnt;
	u32			tx_done_excess;
	u32			bmu_free_slots;
	u32			bmu_alloc_fail;
	u32			bmu_alloc_late;		/* buffers of timed-out requests */
	bool			bmu_alloc_pending;
	u32			bmu_free_fail;
	u32			rx_cur_idx[ZX_RX_QUEUES];
	bool			rx_synced[ZX_RX_QUEUES];
	u32			rx_resync_cnt;
	u32			rx_alias_cnt;
	u32			rx_nobuf_cnt;
	u32			rx_rel_fail;
	u32			rx_bad_cnt;

	/* TX ring, BMU buffer allocation and the TX counters */
	spinlock_t		tx_lock;
	/* BMU free register and bmu_free_slots */
	spinlock_t		bmu_free_lock;

	/* MAC counters of all ports, read before their 32 bits wrap */
	struct mutex		mib_lock;
	struct delayed_work	mib_work;

	/* Hardware flow offload (packet processor NAT) */
	struct mutex		ppe_lock;
	bool			ppe_active;
	int			ppe_users;	/* bound flowtables */
	struct rhashtable	ppe_flows;
	DECLARE_BITMAP(ppe_hash_used, ZX_PPE_HASH_SIZE);
	DECLARE_BITMAP(ppe_flow_used, ZX_PPE_FLOWS);
	DECLARE_BITMAP(ppe_nh_used, ZX_PPE_NEXT_HOPS);
	struct {
		u8	mac[ETH_ALEN];
		__be32	ip;
		int	refs;
	} ppe_subnet[ZX_PPE_SUBNETS];
	u32			ppe_saved[4];
	u32			ppe_count;
	u32			ppe_add_skip;	/* flows the hardware cannot take */
	u32			ppe_add_fail;	/* no free entry, access timeout */

	/* Hardware LAN switching between the ports of one Linux bridge */
	spinlock_t		trap_lock;	/* zx_update_traps() */
	struct mutex		sw_lock;
	bool			sw_ready;	/* bridge notifications registered */
	u8			sw_members;	/* bridge ports that switch */
	struct work_struct	sw_work;
	spinlock_t		sw_fdb_lock;	/* the fields below */
	struct net_device	*sw_bridge[ZX_NUM_PORTS];
	bool			sw_recompute;
	u8			sw_flush;	/* bridge ports to flush */
	bool			sw_fdb_overflow;
	unsigned int		sw_fdb_count;
	u8			sw_fdb_mac[ZX_SW_FDB_QUEUE][ETH_ALEN];
	u32			sw_fdb_deleted;
	struct notifier_block	sw_netdev_nb;
	struct notifier_block	sw_switchdev_nb;
	struct notifier_block	sw_switchdev_blocking_nb;
};

/* zx279128s-main.c */
bool zx_eth_is_port(const struct net_device *dev);
void zx_set_trap_dmac(struct zx_eth_adapter *adapter, int idx, const u8 *mac);

/* zx279128s-ppe.c */
extern const struct rhashtable_params zx_ppe_ht_params;
void zx_update_traps(struct zx_eth_adapter *adapter);
void zx_update_mac_tables(struct zx_eth_adapter *adapter);
int zx_eth_setup_tc(struct net_device *dev, enum tc_setup_type type,
		    void *type_data);

/* zx279128s-switch.c */
void zx_sw_init(struct zx_eth_adapter *adapter);
void zx_sw_work(struct work_struct *work);
void zx_sw_port_down(struct zx_eth_adapter *adapter, int port);

#endif

// SPDX-License-Identifier: GPL-2.0-only
/*
 * ZTE zx279128s Ethernet switch and DMA driver
 *
 * Copyright (C) 2026 Navid Ghahremani <ghahramani.navid@gmail.com>
 *
 * The zx279128s has a five-port gigabit switch with a traffic manager (TM),
 * a buffer management unit (BMU) and a DMA engine that moves frames between
 * the switch and the CPU. Every port is its own net_device (lan1-lan4 and wan
 * on the ZTE ZXHN H3600). Ports in the same Linux bridge switch known unicast
 * in hardware (zx279128s-switch.c), and established flows can be offloaded to
 * the packet processor (zx279128s-ppe.c).
 *
 * All frames pass through buffers of the BMU pool, which lives in a 16 MiB
 * DMA arena that is allocated once at probe and kept. Received frames are
 * copied out of their buffer into an skb, frames to send are copied into a
 * buffer. The ingress port of a received frame is in its descriptor, and a
 * frame to send goes to the TM queue of its port. The hardware tables are
 * programmed from scratch, with the values of the vendor firmware.
 */

#include <linux/bitfield.h>
#include <linux/debugfs.h>
#include <linux/delay.h>
#include <linux/dma-mapping.h>
#include <linux/etherdevice.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/iopoll.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_net.h>
#include <linux/phy.h>
#include <linux/platform_device.h>
#include <linux/rtnetlink.h>
#include <linux/seq_file.h>

#include "zx279128s-eth.h"

/* ---- BMU helpers --------------------------------------------------- */

/*
 * Allocate a buffer from the BMU (caller holds tx_lock).  A request that
 * does not complete in time stays with the BMU, which hands out its
 * buffer later: keep it pending and collect that buffer on the next call,
 * as the vendor driver does.  Starting a new request instead lost one
 * buffer for good with every timeout.
 */
static int zx_bmu_alloc_bp(struct zx_eth_adapter *adapter)
{
	void __iomem *base = adapter->base;
	u32 val;
	int ret;

	if (adapter->bmu_alloc_pending) {
		if (readl(base + ZX_BMU_BUF_ALLOC) & ZX_BMU_BUF_ALLOC_REQ) {
			adapter->bmu_alloc_fail++;
			return -EBUSY;
		}
		adapter->bmu_alloc_pending = false;
		val = readl(base + ZX_BMU_BUF_ID);
		if (val & ZX_BMU_BUF_ID_VALID) {
			adapter->bmu_alloc_late++;
			return FIELD_GET(ZX_BMU_BUF_ID_BP, val);
		}
	}

	val = readl(base + ZX_BMU_BUF_ALLOC);
	writel(val | ZX_BMU_BUF_ALLOC_REQ, base + ZX_BMU_BUF_ALLOC);
	ret = readl_poll_timeout_atomic(base + ZX_BMU_BUF_ALLOC, val,
					!(val & ZX_BMU_BUF_ALLOC_BUSY), 1, 1000);
	if (!ret) {
		val = readl(base + ZX_BMU_BUF_ID);
		if (val & ZX_BMU_BUF_ID_VALID)
			return FIELD_GET(ZX_BMU_BUF_ID_BP, val);
		ret = -ENOSPC;
	}

	adapter->bmu_alloc_pending = true;
	adapter->bmu_alloc_fail++;
	return ret;
}

static int zx_bmu_free_bp(struct zx_eth_adapter *adapter, u16 bp)
{
	u32 val;
	int ret = 0;

	spin_lock_bh(&adapter->bmu_free_lock);
	if (!adapter->bmu_free_slots) {
		ret = readl_poll_timeout_atomic(adapter->base + ZX_BMU_FREE_STATUS,
						val, val & ZX_BMU_FREE_SLOTS, 1, 5000);
		if (ret) {
			adapter->bmu_free_fail++;
			goto out;
		}
		adapter->bmu_free_slots = FIELD_GET(ZX_BMU_FREE_SLOTS, val);
	}

	adapter->bmu_free_slots--;
	writel(bp, adapter->base + ZX_BMU_BUF_FREE);
out:
	spin_unlock_bh(&adapter->bmu_free_lock);
	return ret;
}

/* ---- RX descriptor release ----------------------------------------- */

/*
 * Hand @n consumed slots of RX queue @q back to the hardware with one
 * command, as the vendor driver does (count in bits 13:4, ring B), instead
 * of two register polls and two writes per frame.
 *
 * The traffic manager counts each frame it hands to the CPU against the
 * class in descriptor byte 6 bit 0, and a release only returns slots of the
 * class it names.  Releasing a class-1 slot as class 0 leaves the frame
 * counted in the CPU queue for good: after 1024 frames the queue's reserved
 * space is used up.  So count the slots per class and release each class
 * separately, as the vendor driver does.
 */
static int zx_rx_release(struct zx_eth_adapter *adapter, int q, int class,
			 u32 n)
{
	u32 val;
	int ret;

	ret = readl_poll_timeout_atomic(adapter->base + ZX_RX_REL_CTRL,
					val, !(val & ZX_RX_REL_BUSY), 1, 5000);
	if (ret)
		return ret;
	writel(FIELD_PREP(ZX_RX_REL_COUNT, n) | FIELD_PREP(ZX_RX_REL_CLASS, class) |
	       FIELD_PREP(ZX_RX_REL_QUEUE, q), adapter->base + ZX_RX_REL_DATA);
	writel(ZX_RX_REL_BUSY, adapter->base + ZX_RX_REL_CTRL);
	return 0;
}

/*
 * Clear a consumed descriptor.  All cleared descriptors must be visible
 * before their slots are released: with a full ring the hardware refills a
 * slot as soon as it is released, and clearing it afterwards would wipe
 * the new descriptor.
 */
static void zx_rx_clear_desc(__le32 *desc)
{
	desc[0] = 0; desc[1] = 0; desc[2] = 0; desc[3] = 0;
}

/* Count a consumed slot for its release class, then clear it. */
static void zx_rx_consume(__le32 *desc, u32 rel[2])
{
	rel[((u8 *)desc)[6] & 1]++;
	zx_rx_clear_desc(desc);
}

/* ---- MAC port helpers ---------------------------------------------- */

static void zx_port_set_speed(struct zx_eth_adapter *adapter, int port,
			      int speed, int duplex)
{
	void __iomem *reg = adapter->base + ZX_MAC_PORT(port) + ZX_MAC_CTRL;
	u32 val = readl(reg);

	if (speed == SPEED_1000) {
		val &= ~(ZX_MAC_CTRL_MII | ZX_MAC_CTRL_100);
		val |= ZX_MAC_CTRL_FD;
	} else {
		if (duplex == DUPLEX_FULL) {
			val |= ZX_MAC_CTRL_MII | ZX_MAC_CTRL_FD;
		} else {
			val &= ~ZX_MAC_CTRL_FD;
			val |= ZX_MAC_CTRL_MII;
		}
		if (speed == SPEED_100)
			val |= ZX_MAC_CTRL_100;
		else
			val &= ~ZX_MAC_CTRL_100;
	}
	writel(val, reg);
}

static void zx_port_enable(struct zx_eth_adapter *adapter, int port, bool enable)
{
	void __iomem *reg = adapter->base + ZX_MAC_PORT(port) + ZX_MAC_CTRL;
	u32 val = readl(reg);

	if (enable)
		val |= ZX_MAC_CTRL_EN;
	else
		val &= ~ZX_MAC_CTRL_EN;
	writel(val, reg);
}

/* ---- PHY link callbacks -------------------------------------------- */

static struct zx_eth_priv *zx_config_to_priv(struct phylink_config *config)
{
	return netdev_priv(to_net_dev(config->dev));
}

static void zx_mac_config(struct phylink_config *config, unsigned int mode,
			  const struct phylink_link_state *state)
{
	/* the MAC follows the link in zx_mac_link_up() only */
}

static void zx_mac_link_down(struct phylink_config *config, unsigned int mode,
			     phy_interface_t interface)
{
	struct zx_eth_priv *priv = zx_config_to_priv(config);

	zx_port_enable(priv->adapter, priv->port, false);
	zx_sw_port_down(priv->adapter, priv->port);
}

static void zx_mac_link_up(struct phylink_config *config,
			   struct phy_device *phy, unsigned int mode,
			   phy_interface_t interface, int speed, int duplex,
			   bool tx_pause, bool rx_pause)
{
	struct zx_eth_priv *priv = zx_config_to_priv(config);

	zx_port_set_speed(priv->adapter, priv->port, speed, duplex);
	zx_port_enable(priv->adapter, priv->port, true);
}

static const struct phylink_mac_ops zx_phylink_mac_ops = {
	.mac_config	= zx_mac_config,
	.mac_link_down	= zx_mac_link_down,
	.mac_link_up	= zx_mac_link_up,
};

/*
 * Frames addressed to one of our MAC addresses go to the CPU, whatever
 * their type (the ethertype traps below cover the common ones).
 */
void zx_set_trap_dmac(struct zx_eth_adapter *adapter, int idx, const u8 *mac)
{
	writel(mac[2] << 24 | mac[3] << 16 | mac[4] << 8 | mac[5],
	       adapter->base + ZX_SPA_TRAP_DMAC + idx * 8);
	writel(mac[0] << 8 | mac[1], adapter->base + ZX_SPA_TRAP_DMAC + idx * 8 + 4);
}

static void zx_dma_enable(struct zx_eth_adapter *adapter, bool enable)
{
	u32 val = readl(adapter->base + ZX_DMA_CTRL);

	val &= ~ZX_DMA_CTRL_EN;
	val |= ZX_DMA_CTRL_BIT21 | (enable ? ZX_DMA_CTRL_EN : 0);
	writel(val, adapter->base + ZX_DMA_CTRL);
	readl(adapter->base + ZX_DMA_CTRL);
}

static int zx_tm_write(struct zx_eth_adapter *adapter, u32 table, u32 index,
		       const u32 data[4])
{
	void __iomem *base = adapter->base;
	u32 val;
	int i, ret;

	ret = readl_poll_timeout(base + ZX_TM_IND_STATUS, val,
				 val & ZX_TM_IND_READY, 1, 1000);
	if (ret)
		return ret;
	writel(FIELD_PREP(ZX_TM_IND_INDEX, index) |
	       FIELD_PREP(ZX_TM_IND_TABLE, table), base + ZX_TM_IND_CMD);
	for (i = 3; i >= 0; i--)
		writel(data[i], base + ZX_TM_IND_DATA(i));
	return 0;
}

static int zx_tm_read0(struct zx_eth_adapter *adapter, u32 table, u32 index,
		       u32 *data)
{
	void __iomem *base = adapter->base;
	u32 val;
	int ret;

	ret = readl_poll_timeout(base + ZX_TM_IND_STATUS, val,
				 val & ZX_TM_IND_READY, 1, 1000);
	if (ret)
		return ret;
	writel(FIELD_PREP(ZX_TM_IND_INDEX, index) |
	       FIELD_PREP(ZX_TM_IND_TABLE, table) | ZX_TM_IND_READ,
	       base + ZX_TM_IND_CMD);
	ret = readl_poll_timeout(base + ZX_TM_IND_STATUS, val,
				 val & ZX_TM_IND_READY, 1, 1000);
	if (ret)
		return ret;
	*data = readl(base + ZX_TM_IND_DATA(0));
	return 0;
}

/* Queue, RED and scheduler tables.  The RED profiles use the vendor
 * pon_tm_red_init() value.
 *
 * Queue word 0 is the reserved space (bits 10:0) plus the shared space
 * (bits 25:11), in buffers.  Queues 0-15 feed the CPU.  They get 1023
 * reserved and no shared buffers, as in the stock firmware: a frame stays
 * counted in its queue until the driver releases its RX slot, so the
 * traffic manager never has more than 1023 frames in the 1024-slot RX
 * ring and drops the excess itself.  With more, a burst faster than the
 * driver drains the ring makes the DMA overwrite slots that were not read
 * yet, and those frames and their buffers are lost.
 *
 * The queue usage in table 1 survives the reset.  U-Boot receives a TFTP
 * download on CPU queue 0 and never returns the slots, so after a TFTP
 * boot the queue starts with up to 1024 frames counted that will never be
 * released, and a 1023 limit would drop every frame.  Raise the limit of
 * each CPU queue by what is left counted in it.
 */
static int zx_tm_init(struct zx_eth_adapter *adapter)
{
	static const u32 queue[4] = { 0x00800400, 0, 0, 0 };
	static const u32 red[4] = { 0x00200020, 0, 0, 0 };
	static const u32 scheduler[4] = {
		0xff803fff, 0x0100ff80, 0x00100200, 32
	};
	u32 cpu_queue[4] = { 0, 0, 0, 0 };
	u32 used, limit;
	int i, ret;

	for (i = 0; i < 16; i++) {
		ret = zx_tm_read0(adapter, ZX_TM_TABLE_QUEUE_USE, i, &used);
		if (ret)
			return ret;
		used = (used & 0x7ff) + ((used >> 11) & 0x7fff);
		if (used)
			dev_dbg(adapter->dev,
				"CPU queue %d: %u frames left counted by the boot loader\n",
				i, used);
		limit = 1023 + used;
		cpu_queue[0] = min(limit, 0x7ffU) | (limit - min(limit, 0x7ffU)) << 11;
		ret = zx_tm_write(adapter, ZX_TM_TABLE_QUEUE, i, cpu_queue);
		if (ret)
			return ret;
	}
	for (i = 16; i < 400; i++) {
		ret = zx_tm_write(adapter, ZX_TM_TABLE_QUEUE, i, queue);
		if (ret)
			return ret;
	}
	for (i = 0; i < 384; i++) {
		ret = zx_tm_write(adapter, ZX_TM_TABLE_RED, i, red);
		if (ret)
			return ret;
		ret = zx_tm_write(adapter, ZX_TM_TABLE_SCHED, i, scheduler);
		if (ret)
			return ret;
	}
	return 0;
}

/* ---- Hardware initialisation --------------------------------------- */

static void zx_pp_init(struct zx_eth_adapter *adapter)
{
	void __iomem *base = adapter->base;

	/* Mode-2 TX goes through the packet processor bridge.  The bootloader
	 * initializes it for TFTP, but not when booting directly from NAND.
	 * Initialize it ourselves, while DMA and the MACs are quiesced.  These
	 * are the H3600 bootloader's PP reset, bridge and classifier settings.
	 */
	writel(2, base + ZX_PP_RESET);
	usleep_range(1000, 2000);

	writel(0x000200ff, base + ZX_SBRG_PORT_CTRL);
	/* Stock firmware value.  With the reset value 0x00010000 the bridge
	 * steered IPv6 unicast arriving on WAN to a queue the driver never
	 * services, and each frame kept a BMU buffer: about one leaked
	 * buffer per background WAN frame on an IPv6 network.
	 */
	writel(0x0000dfdf, base + 0x388008);
	/* The bridge would forward a frame between ports only through VLAN
	 * table entries, which the driver does not program, so it drops what
	 * the traps leave.  Frames between two LAN ports must reach the Linux
	 * bridge instead.  Turn off source address learning, so that every
	 * unicast destination is unknown, and send unknown unicast arriving on
	 * a switch port (bridge ports 1-5) to the CPU.  Without this, lifting
	 * the IPv4 trap for flow offload loses IPv4 between two LAN hosts.
	 */
	writel(0x00000000, base + ZX_SBRG_LEARN);
	writel(0xff5ffdff, base + 0x388340);
	writel(0x0000003e, base + 0x388344);
	writel(0x0000003f, base + 0x388380);
	writel(0xaaaaaaaa, base + 0x38863c);
	writel(0x00005555, base + 0x3881c4);
	writel(0x000bf874, base + 0x388188);
	writel(0x000000ff, base + 0x3882c0);
	writel(0x0000ffff, base + 0x388300);
	writel(0x0000003e, base + 0x388304);

	writel(0x0000309a, base + 0x38c080);
	writel(0, base + 0x38c088);
	writel(1, base + 0x38c0cc);
	writel(0, base + 0x3a0010);
	writel(0, base + 0x3a0014);
}

/* Reset the switch and leave its ports and DMA stopped */
static void zx_hw_reset(struct zx_eth_adapter *adapter)
{
	void __iomem *base = adapter->base;
	int i;

	/* Master reset pulse */
	writel(0, base + ZX_GLB_RESET);
	fsleep(10);
	writel(0xffffffff, base + ZX_GLB_RESET);

	/* Quiesce all 5 ports */
	for (i = 0; i < ZX_NUM_PORTS; i++)
		zx_port_enable(adapter, i, false);
	zx_dma_enable(adapter, false);
	writel(0, base + ZX_BMU_CTRL);
}

static int zx_hw_init(struct zx_eth_adapter *adapter)
{
	void __iomem *base = adapter->base;
	static const int port_to_sw_idx[8] = { 0, 1, 2, 3, 4, -1, 5, 6 };
	__be16 *tab;
	u32 val;
	int i, ret;

	/* 0. Reset, interrupts off until a port is opened */
	zx_hw_reset(adapter);
	writel(~0, base + ZX_TM_INT_MASK);
	msleep(20);

	/* 1. Clock gating: enable the clocks of MAC ports 0-4 */
	writel(readl(base + ZX_GLB_CLK_CTRL) & ~ZX_GLB_CLK_CTRL_BITS,
	       base + ZX_GLB_CLK_CTRL);
	writel(GENMASK(ZX_NUM_PORTS - 1, 0), base + ZX_GLB_PORT_CLK_EN);
	zx_pp_init(adapter);

	/* 2. Free-buffer index tables */
	tab = adapter->tab0;
	for (i = 0; i < ZX_TAB0_ENTRIES; i++)
		tab[i] = cpu_to_be16(i);

	tab = adapter->tab1;
	for (i = 0; i < ZX_TAB1_ENTRIES; i++)
		tab[i] = cpu_to_be16(i);

	dma_wmb();
	ret = zx_tm_init(adapter);
	if (ret)
		return dev_err_probe(adapter->dev, ret, "TM table setup timed out\n");

	/* 3. Traffic Manager buffer pool */
	writel((u32)adapter->tab0_dma, base + ZX_TM_TAB0_ADDR);
	writel((u32)adapter->tab1_dma, base + ZX_TM_TAB1_ADDR);
	writel(0x08000800, base + ZX_TM_TAB_LIMIT);
	writel((u32)adapter->buf_pool_dma, base + ZX_TM_BUF_START);
	writel((u32)adapter->buf_pool_dma + ZX_BUF_POOL_SIZE, base + ZX_TM_BUF_END);
	writel((u32)adapter->rx_ring_dma, base + ZX_TM_BUF_F0);
	writel(16, base + ZX_TM_CONFIG_START);

	/* DMA and TM control values from the vendor pon_tm_dma_init() and
	 * the stock firmware, programmed before the BMU is configured.
	 */
	writel(1,      base + ZX_DMA_BASE + 0x28);
	writel(1,      base + ZX_DMA_BASE + 0x2c);
	writel(ZX_RX_IRQ_USECS * 50, base + ZX_DMA_RX_IRQ_TIMER);
	writel(ZX_RX_IRQ_FRAMES, base + ZX_DMA_RX_IRQ_FRAMES);
	writel(0x7f,   base + ZX_DMA_BASE + 0x04);
	writel(32,     base + ZX_DMA_BASE + 0x20);
	writel(32,     base + ZX_DMA_BASE + 0x24);
	/* The bootloader leaves 0x00131213 here, the stock firmware runs with
	 * 0x00131217.
	 */
	writel(0x00131217, base + ZX_DMA_BASE + 0x388);
	writel(readl(base + ZX_TM_CTRL) | ZX_TM_CTRL_BIT6, base + ZX_TM_CTRL);

	/* 4. Buffer Management Unit */
	writel(0,          base + ZX_BMU_CTRL);
	writel(0x0104c040, base + ZX_BMU_CFG1);
	writel(0x0104c040, base + ZX_BMU_CFG2);
	/* Pool size in units of 32 buffers.  With a 2048-buffer pool the BMU
	 * never fills its external free list (BPPE) after a cold NAND boot and
	 * runs on its ~64-entry internal cache only.  4096 works.
	 */
	writel(ZX_NUM_BUFFERS / 32 - 1, base + ZX_BMU_LIMIT1);
	writel(15,         base + ZX_BMU_LIMIT2);
	writel(ZX_NUM_BUFFERS << 16, base + ZX_BMU_THRES1);
	writel(0x2000000,  base + ZX_BMU_THRES2);
	writel(0,          base + ZX_BMU_BUF_ALLOC);
	writel(ZX_BMU_CTRL_EN, base + ZX_BMU_CTRL);

	writel(17, base + 0x3a00e0);

	/* 5. DMA descriptor rings */
	memset(adapter->tx_ring, 0, ZX_NUM_TX_DESC * ZX_DESC_SIZE);
	memset(adapter->rx_ring, 0, ZX_RX_QUEUES * ZX_RX_QUEUE_STRIDE);
	adapter->tx_cur_idx = 0;
	for (i = 0; i < ZX_RX_QUEUES; i++) {
		adapter->rx_cur_idx[i] = 0;
		adapter->rx_synced[i] = false;
	}
	adapter->tx_pending = 0;
	adapter->tx_complete_cnt = 0;
	adapter->tx_done_excess = 0;
	adapter->bmu_free_slots = 0;

	writel((u32)adapter->buf_pool_dma + ZX_AREA50_OFFSET, base + ZX_DMA_AREA50);
	writel((u32)adapter->tx_ring_dma, base + ZX_DMA_TX_RING);
	writel(0x00400040,             base + ZX_DMA_RING_CFG);
	readl(base + ZX_DMA_TX_DONE);

	dma_wmb();

	/* 6. Switch: enable forwarding */
	writel(ZX_SW_FWD_CTRL_ON, base + ZX_SW_FWD_CTRL);

	for (i = 0; i < 8; i++) {
		int idx = port_to_sw_idx[i];

		if (idx >= 0)
			writel(1, base + ZX_SPA_PORT_EN(idx));
	}

	/*
	 * Enable every class of packets the switch can hand to the CPU, up
	 * and down direction, as the stock firmware does.  With only a few
	 * classes enabled, IPv6 neighbour discovery never reached the CPU.
	 */
	writel(0xffffffff, base + ZX_SPA_UP_REG_PKT_EN);
	writel(0xffffffff, base + ZX_SPA_UP_REG_PKT_EN + 4);
	writel(0xffffffff, base + ZX_SPA_DN_REG_PKT_EN);
	writel(0xffffffff, base + ZX_SPA_DN_REG_PKT_EN + 4);

	/*
	 * The switch never forwards from port to port and hands the CPU only
	 * the frames a trap matches; the Linux bridge forwards between the
	 * port netdevs.  So trap the common ethertypes whatever their
	 * destination (IPv4, IPv6, ARP, VLAN tagged).  This also covers what
	 * the IPv4 protocol trap (TCP, UDP, ICMP) did, so clear that one.
	 */
	writel(ETH_P_IP << 16 | ETH_P_IPV6, base + ZX_SPA_TRAP_ETH_TYPE);
	writel(ETH_P_ARP << 16 | ETH_P_8021Q, base + ZX_SPA_TRAP_ETH_TYPE + 4);
	writel(0, base + ZX_SPA_TRAP_PROTO);

	/* Clear port filter bits for ports 0-4 */
	for (i = 0; i < ZX_NUM_PORTS; i++) {
		val = readl(base + ZX_SW_PORT_FILTER(i));
		val &= ~ZX_SW_PORT_FILTER_BITS;
		writel(val, base + ZX_SW_PORT_FILTER(i));
	}

	/* 7. MAC ports 0-4 */
	for (i = 0; i < ZX_NUM_PORTS; i++) {
		void __iomem *pb = base + ZX_MAC_PORT(i);

		writel(0x00bbe000, pb + ZX_MAC_CTRL);
		writel(0x80000001, pb + 0x08);
		writel(0x0000fffe, pb + 0x04);
		writel(0x00011200, pb + 0xe0);
		writel(50,         pb + 0xd00);
		writel(168,        pb + 0xd30);
		writel(0x00300002, pb + 0x70);
		writel(0x4000,     pb + 0xb4);
		writel(0x0010ff11, pb + 0xb00);

		zx_port_set_speed(adapter, i, SPEED_1000, DUPLEX_FULL);
		zx_port_enable(adapter, i, false);
	}

	return 0;
}

/*
 * No interrupt reports sent frames.  They are reclaimed on every transmit
 * and in the NAPI poll; while a full ring keeps the queues stopped, a timer
 * runs the poll until they can be woken again.  Called with tx_lock held.
 */
static void zx_tx_kick(struct zx_eth_adapter *adapter)
{
	if (!hrtimer_active(&adapter->tx_timer))
		hrtimer_start(&adapter->tx_timer, us_to_ktime(ZX_TX_POLL_US),
			      HRTIMER_MODE_REL);
}

static void zx_tx_reclaim(struct zx_eth_adapter *adapter)
{
	u32 done = readl(adapter->base + ZX_DMA_TX_DONE) & 0xffff;
	u32 n = min(done, adapter->tx_pending);

	if (unlikely(done > adapter->tx_pending))
		adapter->tx_done_excess += done - adapter->tx_pending;
	adapter->tx_complete_cnt += n;
	adapter->tx_pending -= n;
	if (adapter->tx_pending < ZX_TX_QUEUE_LIMIT) {
		int p;

		for (p = 0; p < ZX_NUM_PORTS; p++)
			if (adapter->ports[p] && netif_queue_stopped(adapter->ports[p]))
				netif_wake_queue(adapter->ports[p]);
	}
}

/* ---- TX path ------------------------------------------------------- */

static netdev_tx_t zx_eth_xmit(struct sk_buff *skb, struct net_device *netdev)
{
	struct zx_eth_priv *priv = netdev_priv(netdev);
	struct zx_eth_adapter *adapter = priv->adapter;
	void __iomem *base = adapter->base;
	unsigned int len = skb->len;
	__le32 *desc;
	u32 tx_idx;
	u8 *buf_ptr;
	int bp;

	if (len > 1536) {
		dev_kfree_skb_any(skb);
		dev_dstats_tx_dropped(netdev);
		return NETDEV_TX_OK;
	}

	spin_lock_bh(&adapter->tx_lock);
	zx_tx_reclaim(adapter);
	/*
	 * The ports share the ring.  The queues are stopped as soon as it is
	 * full, but another port's transmit may already be waiting for the
	 * lock: give its frame back to the stack.
	 */
	if (adapter->tx_pending >= ZX_TX_QUEUE_LIMIT) {
		netif_stop_queue(netdev);
		zx_tx_kick(adapter);
		spin_unlock_bh(&adapter->tx_lock);
		return NETDEV_TX_BUSY;
	}
	if (skb_put_padto(skb, ETH_ZLEN)) {
		spin_unlock_bh(&adapter->tx_lock);
		dev_dstats_tx_dropped(netdev);
		return NETDEV_TX_OK;
	}
	len = skb->len;
	bp = zx_bmu_alloc_bp(adapter);
	if (bp < 0 || bp >= ZX_NUM_BUFFERS) {
		spin_unlock_bh(&adapter->tx_lock);
		dev_dstats_tx_dropped(netdev);
		dev_kfree_skb_any(skb);
		return NETDEV_TX_OK;
	}

	buf_ptr = (u8 *)adapter->buffer_pool + ((u32)bp * ZX_BUF_SIZE) + 16;
	if (skb_copy_bits(skb, 0, buf_ptr, len)) {
		zx_bmu_free_bp(adapter, bp);
		spin_unlock_bh(&adapter->tx_lock);
		dev_dstats_tx_dropped(netdev);
		dev_kfree_skb_any(skb);
		return NETDEV_TX_OK;
	}

	tx_idx = adapter->tx_cur_idx % ZX_NUM_TX_DESC;
	desc = (__le32 *)adapter->tx_ring + tx_idx * (ZX_DESC_SIZE / sizeof(*desc));

	/*
	 * Directed egress to the port's TM queue.  The switch does not
	 * forward between ports: the Linux bridge does, one netdev per port.
	 */
	desc[0] = cpu_to_le32(0x80 | (ZX_PORT_TX_QUEUE(priv->port) << 20));
	desc[3] = cpu_to_le32(0x3 | ((len & 0x3fff) << 2));
	desc[1] = cpu_to_le32(0x10000);
	desc[2] = cpu_to_le32(0x21000000 | ((len & 0x3fff) << 9));

	((u8 *)desc)[7] = (u8)((bp & 0x7f) << 1);
	((u8 *)desc)[8] = (u8)(bp >> 7);

	dma_wmb();
	writel(1, base + ZX_DMA_TX_START);

	adapter->tx_cur_idx++;
	adapter->tx_pending++;
	if (adapter->tx_pending >= ZX_TX_QUEUE_LIMIT) {
		int p;

		for (p = 0; p < ZX_NUM_PORTS; p++)
			if (adapter->ports[p])
				netif_stop_queue(adapter->ports[p]);
		zx_tx_kick(adapter);
	}
	spin_unlock_bh(&adapter->tx_lock);
	dev_dstats_tx_add(netdev, len);
	dev_kfree_skb_any(skb);
	return NETDEV_TX_OK;
}

/* ---- RX path ------------------------------------------------------- */

static bool zx_is_own_addr(struct zx_eth_adapter *adapter, const u8 *addr)
{
	int i;

	for (i = 0; i < ZX_NUM_PORTS; i++)
		if (adapter->ports[i] &&
		    ether_addr_equal(adapter->ports[i]->dev_addr, addr))
			return true;
	return false;
}

static int zx_eth_rx(struct zx_eth_adapter *adapter, int budget)
{
	int work_done = 0, q;

	for (q = ZX_RX_QUEUES - 1; q >= 0 && work_done < budget; q--) {
		u32 avail = readl(adapter->base + ZX_DMA_RX_QUEUE_CNT(q)) & 0xffff;
		u32 rel[2] = { 0, 0 };

		while (avail && work_done < budget) {
			u32 idx = adapter->rx_cur_idx[q] % ZX_NUM_RX_DESC;
			__le32 *desc = adapter->rx_ring + q * ZX_RX_QUEUE_STRIDE +
				       idx * ZX_DESC_SIZE;
			u16 bp, len;
			int port;
			struct sk_buff *skb;
			struct net_device *dest_dev;

			dma_rmb();
			bp = (((u8 *)desc)[7] >> 1) | ((u16)((u8 *)desc)[8] << 7);
			len = (le32_to_cpu(desc[3]) >> 2) & 0x3fff;
			port = (((u8 *)desc)[6] >> 3) ? ((((u8 *)desc)[6] >> 3) - 1) : 0;

			if (bp >= ZX_NUM_BUFFERS || len < ETH_HLEN || len > 1536) {
				u32 scan, idx2, found = 0;

				if (adapter->rx_synced[q]) {
					/*
					 * The hardware counted this slot but it
					 * holds no frame: it can leave a slot
					 * empty (e.g. when it had no buffer).
					 * Consume it as the vendor driver does.
					 * Waiting for it would leave our index
					 * behind the hardware's for good.
					 */
					adapter->rx_bad_cnt++;
					if (net_ratelimit())
						dev_warn(adapter->dev,
							 "rxq%d: skipping slot %u (%08x %08x %08x %08x), %u pending\n",
							 q, idx, le32_to_cpu(desc[0]),
							 le32_to_cpu(desc[1]),
							 le32_to_cpu(desc[2]),
							 le32_to_cpu(desc[3]), avail);
					zx_rx_consume(desc, rel);
					adapter->rx_cur_idx[q]++;
					work_done++;
					avail--;
					if (bp && bp < ZX_NUM_BUFFERS &&
					    zx_bmu_free_bp(adapter, bp))
						adapter->bmu_free_fail++;
					continue;
				}

				for (scan = 1; scan < ZX_NUM_RX_DESC; scan++) {
					__le32 *d2;
					u16 bp2, len2;

					idx2 = (idx + scan) % ZX_NUM_RX_DESC;
					d2 = adapter->rx_ring + q * ZX_RX_QUEUE_STRIDE +
					     idx2 * ZX_DESC_SIZE;
					dma_rmb();
					bp2 = (((u8 *)d2)[7] >> 1) |
					      ((u16)((u8 *)d2)[8] << 7);
					len2 = (le32_to_cpu(d2[3]) >> 2) & 0x3fff;
					if (bp2 < ZX_NUM_BUFFERS &&
					    len2 >= ETH_HLEN && len2 <= 1536) {
						found = 1;
						break;
					}
				}
				if (!found)
					break;
				adapter->rx_cur_idx[q] = idx2;
				adapter->rx_resync_cnt++;
				continue;
			}
			adapter->rx_synced[q] = true;

			if (bp == 0) {
				adapter->rx_nobuf_cnt++;
				zx_rx_consume(desc, rel);
				adapter->rx_cur_idx[q]++;
				work_done++;
				avail--;
				continue;
			}

			/* Aliasing detector: frames from one of our own addresses */
			if (len >= ETH_HLEN) {
				u8 *d = adapter->buffer_pool + bp * ZX_BUF_SIZE + 16;

				if (zx_is_own_addr(adapter, d + ETH_ALEN)) {
					adapter->rx_alias_cnt++;
					zx_rx_consume(desc, rel);
					adapter->rx_cur_idx[q]++;
					work_done++;
					avail--;
					/*
					 * A frame of our own that the switch
					 * sent back still has an RX buffer of
					 * its own: return it, or every such
					 * frame leaks one BMU buffer.
					 */
					if (zx_bmu_free_bp(adapter, bp))
						adapter->bmu_free_fail++;
					continue;
				}
			}

			/* Deliver to the netdev of the ingress port */
			dest_dev = port < ZX_NUM_PORTS ? adapter->ports[port] : NULL;

			skb = napi_alloc_skb(&adapter->napi, len);
			if (skb && dest_dev && (dest_dev->flags & IFF_UP)) {
				dma_rmb();
				memcpy(skb_put(skb, len), adapter->buffer_pool +
				       bp * ZX_BUF_SIZE + 16, len);
				skb->dev = dest_dev;
				skb->protocol = eth_type_trans(skb, dest_dev);
				napi_gro_receive(&adapter->napi, skb);
				dev_dstats_rx_add(dest_dev, len);
			} else {
				if (skb)
					dev_kfree_skb_any(skb);
				if (dest_dev)
					dev_dstats_rx_dropped(dest_dev);
			}

			zx_rx_consume(desc, rel);
			adapter->rx_cur_idx[q]++;
			work_done++;
			avail--;
			if (zx_bmu_free_bp(adapter, bp))
				adapter->bmu_free_fail++;
		}
		if (rel[0] || rel[1])
			dma_wmb();
		if (rel[0] && zx_rx_release(adapter, q, 0, rel[0]))
			adapter->rx_rel_fail++;
		if (rel[1] && zx_rx_release(adapter, q, 1, rel[1]))
			adapter->rx_rel_fail++;
	}
	return work_done;
}

/* ---- interrupt and NAPI poll -------------------------------------- */

static irqreturn_t zx_eth_isr(int irq, void *dev_id)
{
	struct zx_eth_adapter *adapter = dev_id;

	if (!(readl(adapter->base + ZX_TM_INT_STATUS) & ZX_TM_INT_RX))
		return IRQ_NONE;

	writel(~0, adapter->base + ZX_TM_INT_MASK);
	napi_schedule(&adapter->napi);
	return IRQ_HANDLED;
}

static int zx_eth_poll(struct napi_struct *napi, int budget)
{
	struct zx_eth_adapter *adapter =
		container_of(napi, struct zx_eth_adapter, napi);
	int work_done;

	spin_lock_bh(&adapter->tx_lock);
	zx_tx_reclaim(adapter);
	if (adapter->tx_pending >= ZX_TX_QUEUE_LIMIT)
		zx_tx_kick(adapter);
	spin_unlock_bh(&adapter->tx_lock);
	work_done = budget ? zx_eth_rx(adapter, budget) : 0;

	if (work_done < budget && napi_complete_done(napi, work_done))
		writel(~ZX_TM_INT_RX, adapter->base + ZX_TM_INT_MASK);

	return work_done;
}

static enum hrtimer_restart zx_eth_tx_timer(struct hrtimer *timer)
{
	struct zx_eth_adapter *adapter =
		container_of(timer, struct zx_eth_adapter, tx_timer);

	napi_schedule(&adapter->napi);
	return HRTIMER_NORESTART;
}

/* ---- ndo_open / ndo_stop ------------------------------------------- */

static int zx_eth_open(struct net_device *netdev)
{
	struct zx_eth_priv *priv = netdev_priv(netdev);
	struct zx_eth_adapter *adapter = priv->adapter;

	if (adapter->open_count == 0) {
		napi_enable(&adapter->napi);
		writel(~ZX_TM_INT_RX, adapter->base + ZX_TM_INT_MASK);
	}
	adapter->open_count++;

	phylink_start(priv->phylink);
	netif_start_queue(netdev);
	return 0;
}

static int zx_eth_stop(struct net_device *netdev)
{
	struct zx_eth_priv *priv = netdev_priv(netdev);
	struct zx_eth_adapter *adapter = priv->adapter;

	netif_tx_disable(netdev);
	phylink_stop(priv->phylink);
	adapter->open_count--;

	if (adapter->open_count == 0) {
		writel(~0, adapter->base + ZX_TM_INT_MASK);
		hrtimer_cancel(&adapter->tx_timer);
		napi_disable(&adapter->napi);
		/* a last poll may have unmasked it again */
		writel(~0, adapter->base + ZX_TM_INT_MASK);
		synchronize_irq(adapter->irq);
	}

	return 0;
}

/* ---- net_device_ops ------------------------------------------------ */

static int zx_eth_set_mac_address(struct net_device *netdev, void *p)
{
	struct zx_eth_priv *priv = netdev_priv(netdev);
	struct zx_eth_adapter *adapter = priv->adapter;
	int ret;

	ret = eth_mac_addr(netdev, p);
	if (ret)
		return ret;

	mutex_lock(&adapter->ppe_lock);
	zx_update_mac_tables(adapter);
	mutex_unlock(&adapter->ppe_lock);
	return 0;
}

static int zx_eth_ioctl(struct net_device *netdev, struct ifreq *ifr, int cmd)
{
	struct zx_eth_priv *priv = netdev_priv(netdev);

	return phylink_mii_ioctl(priv->phylink, ifr, cmd);
}

static const struct net_device_ops zx_netdev_ops = {
	.ndo_open		= zx_eth_open,
	.ndo_stop		= zx_eth_stop,
	.ndo_start_xmit		= zx_eth_xmit,
	.ndo_set_mac_address	= zx_eth_set_mac_address,
	.ndo_validate_addr	= eth_validate_addr,
	.ndo_eth_ioctl		= zx_eth_ioctl,
	.ndo_setup_tc		= zx_eth_setup_tc,
};

bool zx_eth_is_port(const struct net_device *dev)
{
	return dev->netdev_ops == &zx_netdev_ops;
}

/* ---- MAC counters -------------------------------------------------- */

/*
 * The offsets are from the vendor's statistics table, and the vendor driver
 * picks the same counters.  The MACs have more (errors, collisions, pause
 * frames), but these are the ones checked against known traffic.
 */
static const u16 zx_mib_regs[ZX_MIB_NUM] = {
	[ZX_MIB_TX_OCTETS]	= ZX_MAC_TX_OCTETS,
	[ZX_MIB_TX_FRAMES]	= ZX_MAC_TX_FRAMES,
	[ZX_MIB_TX_MCAST]	= ZX_MAC_TX_MCAST,
	[ZX_MIB_TX_BCAST]	= ZX_MAC_TX_BCAST,
	[ZX_MIB_TX_HIST + 0]	= ZX_MAC_TX_HIST(0),
	[ZX_MIB_TX_HIST + 1]	= ZX_MAC_TX_HIST(1),
	[ZX_MIB_TX_HIST + 2]	= ZX_MAC_TX_HIST(2),
	[ZX_MIB_TX_HIST + 3]	= ZX_MAC_TX_HIST(3),
	[ZX_MIB_TX_HIST + 4]	= ZX_MAC_TX_HIST(4),
	[ZX_MIB_TX_HIST + 5]	= ZX_MAC_TX_HIST(5),
	[ZX_MIB_RX_OCTETS]	= ZX_MAC_RX_OCTETS,
	[ZX_MIB_RX_UCAST]	= ZX_MAC_RX_UCAST,
	[ZX_MIB_RX_MCAST]	= ZX_MAC_RX_MCAST,
	[ZX_MIB_RX_BCAST]	= ZX_MAC_RX_BCAST,
	[ZX_MIB_RX_HIST + 0]	= ZX_MAC_RX_HIST(0),
	[ZX_MIB_RX_HIST + 1]	= ZX_MAC_RX_HIST(1),
	[ZX_MIB_RX_HIST + 2]	= ZX_MAC_RX_HIST(2),
	[ZX_MIB_RX_HIST + 3]	= ZX_MAC_RX_HIST(3),
	[ZX_MIB_RX_HIST + 4]	= ZX_MAC_RX_HIST(4),
	[ZX_MIB_RX_HIST + 5]	= ZX_MAC_RX_HIST(5),
};

/*
 * The counters are 32 bits wide and keep running.  At 1 Gbit/s the byte
 * counters wrap after 34 s, so they are read every few seconds, and what
 * they grew by since the last read is added to 64-bit sums.
 */
#define ZX_MIB_INTERVAL		(5 * HZ)

static void zx_mib_update(struct zx_eth_priv *priv)
{
	void __iomem *base = priv->adapter->base + ZX_MAC_PORT(priv->port);
	int i;

	lockdep_assert_held(&priv->adapter->mib_lock);

	for (i = 0; i < ZX_MIB_NUM; i++) {
		u32 val = readl(base + zx_mib_regs[i]);

		priv->mib[i] += val - priv->mib_last[i];	/* modulo 2^32 */
		priv->mib_last[i] = val;
	}
}

static void zx_mib_work(struct work_struct *work)
{
	struct zx_eth_adapter *adapter =
		container_of(to_delayed_work(work), struct zx_eth_adapter,
			     mib_work);
	int i;

	mutex_lock(&adapter->mib_lock);
	for (i = 0; i < ZX_NUM_PORTS; i++)
		if (adapter->ports[i])
			zx_mib_update(netdev_priv(adapter->ports[i]));
	mutex_unlock(&adapter->mib_lock);

	schedule_delayed_work(&adapter->mib_work, ZX_MIB_INTERVAL);
}

/* ---- ethtool_ops --------------------------------------------------- */

static int zx_eth_get_link_ksettings(struct net_device *netdev,
				     struct ethtool_link_ksettings *cmd)
{
	struct zx_eth_priv *priv = netdev_priv(netdev);

	return phylink_ethtool_ksettings_get(priv->phylink, cmd);
}

static int zx_eth_set_link_ksettings(struct net_device *netdev,
				     const struct ethtool_link_ksettings *cmd)
{
	struct zx_eth_priv *priv = netdev_priv(netdev);

	return phylink_ethtool_ksettings_set(priv->phylink, cmd);
}

static int zx_eth_nway_reset(struct net_device *netdev)
{
	struct zx_eth_priv *priv = netdev_priv(netdev);

	return phylink_ethtool_nway_reset(priv->phylink);
}

static void zx_eth_get_eth_mac_stats(struct net_device *netdev,
				     struct ethtool_eth_mac_stats *mac_stats)
{
	struct zx_eth_priv *priv = netdev_priv(netdev);
	const u64 *mib = priv->mib;

	mutex_lock(&priv->adapter->mib_lock);
	zx_mib_update(priv);

	mac_stats->FramesTransmittedOK = mib[ZX_MIB_TX_FRAMES];
	mac_stats->OctetsTransmittedOK = mib[ZX_MIB_TX_OCTETS];
	mac_stats->MulticastFramesXmittedOK = mib[ZX_MIB_TX_MCAST];
	mac_stats->BroadcastFramesXmittedOK = mib[ZX_MIB_TX_BCAST];
	/* there is a total, but the vendor driver adds up these three */
	mac_stats->FramesReceivedOK = mib[ZX_MIB_RX_UCAST] +
				      mib[ZX_MIB_RX_MCAST] +
				      mib[ZX_MIB_RX_BCAST];
	mac_stats->OctetsReceivedOK = mib[ZX_MIB_RX_OCTETS];
	mac_stats->MulticastFramesReceivedOK = mib[ZX_MIB_RX_MCAST];
	mac_stats->BroadcastFramesReceivedOK = mib[ZX_MIB_RX_BCAST];

	mutex_unlock(&priv->adapter->mib_lock);
}

/* The last bin also counts 1522-byte (VLAN-tagged) frames */
static const struct ethtool_rmon_hist_range zx_rmon_ranges[] = {
	{   64,   64 },
	{   65,  127 },
	{  128,  255 },
	{  256,  511 },
	{  512, 1023 },
	{ 1024, 1522 },
	{}
};

static void zx_eth_get_rmon_stats(struct net_device *netdev,
				  struct ethtool_rmon_stats *rmon_stats,
				  const struct ethtool_rmon_hist_range **ranges)
{
	struct zx_eth_priv *priv = netdev_priv(netdev);
	int i;

	BUILD_BUG_ON(ARRAY_SIZE(zx_rmon_ranges) != ZX_MAC_HIST_BINS + 1);

	mutex_lock(&priv->adapter->mib_lock);
	zx_mib_update(priv);
	for (i = 0; i < ZX_MAC_HIST_BINS; i++) {
		rmon_stats->hist[i] = priv->mib[ZX_MIB_RX_HIST + i];
		rmon_stats->hist_tx[i] = priv->mib[ZX_MIB_TX_HIST + i];
	}
	mutex_unlock(&priv->adapter->mib_lock);

	*ranges = zx_rmon_ranges;
}

static const struct ethtool_ops zx_ethtool_ops = {
	.get_link		= ethtool_op_get_link,
	.get_link_ksettings	= zx_eth_get_link_ksettings,
	.set_link_ksettings	= zx_eth_set_link_ksettings,
	.nway_reset		= zx_eth_nway_reset,
	.get_eth_mac_stats	= zx_eth_get_eth_mac_stats,
	.get_rmon_stats		= zx_eth_get_rmon_stats,
};

/* ---- debugfs ------------------------------------------------------- */

/* Driver counters and the main DMA, BMU and switch registers */
static int zx_eth_dma_state_show(struct seq_file *s, void *data)
{
	struct zx_eth_adapter *adapter = dev_get_drvdata(s->private);
	static const u32 regs[] = {
		ZX_TM_TAB0_ADDR, ZX_TM_TAB1_ADDR, ZX_TM_BUF_F0,
		ZX_TM_BUF_START, ZX_TM_BUF_END, ZX_BMU_CTRL,
		ZX_BMU_FREE_STATUS,
		ZX_DMA_CTRL, ZX_DMA_RING_CFG,
		ZX_DMA_AREA50, ZX_DMA_TX_RING,
		ZX_DMA_TX_START,
		ZX_RX_REL_CTRL, ZX_RX_REL_DATA,
		ZX_MAC_PORT(0), ZX_MAC_PORT(1), ZX_MAC_PORT(2),
		ZX_MAC_PORT(3), ZX_MAC_PORT(4),
		ZX_SW_FWD_CTRL,
		ZX_SPA_UP_REG_PKT_EN, ZX_SPA_UP_REG_PKT_EN + 4,
		ZX_SPA_DN_REG_PKT_EN, ZX_SPA_DN_REG_PKT_EN + 4,
		0x1d4080, 0x1d4084,
		0x1d40c0, 0x1d40c4,
		0x1d4100, 0x1d4104,
		ZX_SPA_TRAP_PROTO,
		ZX_SPA_PORT_EN(0), ZX_SPA_PORT_EN(1), ZX_SPA_PORT_EN(2),
		ZX_SPA_PORT_EN(3), ZX_SPA_PORT_EN(4), ZX_SPA_PORT_EN(6),
	};
	int i;

	rtnl_lock();
	seq_printf(s, "arena=%pad open_count=%d\n",
		   &adapter->buf_pool_dma, adapter->open_count);
	seq_printf(s, "tx_submitted=%u tx_completed=%u tx_pending=%u tx_done_excess=%u rx_resync=%u\n",
		   adapter->tx_cur_idx, adapter->tx_complete_cnt,
		   adapter->tx_pending, adapter->tx_done_excess,
		   adapter->rx_resync_cnt);
	seq_printf(s, "rx_alias=%u rx_nobuf=%u rx_rel_fail=%u rx_bad=%u\n",
		   adapter->rx_alias_cnt, adapter->rx_nobuf_cnt,
		   adapter->rx_rel_fail, adapter->rx_bad_cnt);
	seq_printf(s, "ppe_active=%u ppe_flows=%u ppe_add_skip=%u ppe_add_fail=%u\n",
		   adapter->ppe_active, adapter->ppe_count,
		   adapter->ppe_add_skip, adapter->ppe_add_fail);
	seq_printf(s, "sw_members=%#x sw_fdb_deleted=%u\n",
		   adapter->sw_members,
		   adapter->sw_fdb_deleted);
	seq_printf(s, "bmu_alloc_fail=%u bmu_alloc_late=%u bmu_free_fail=%u bmu_free_slots=%u\n",
		   adapter->bmu_alloc_fail, adapter->bmu_alloc_late,
		   adapter->bmu_free_fail, adapter->bmu_free_slots);

	for (i = 0; i < 0x100; i += 4)
		seq_printf(s, "bmu+%03x=%08x\n", i,
			   readl(adapter->base + ZX_BMU_BASE + i));
	/* Reading 0x68 (TX done) would clear the count of sent frames */
	for (i = 0; i < 0x120; i += 4) {
		if (i == 0x68)
			continue;
		seq_printf(s, "dma+%03x=%08x\n", i,
			   readl(adapter->base + ZX_DMA_BASE + i));
	}

	for (i = 0; i < ARRAY_SIZE(regs); i++)
		seq_printf(s, "%06x=%08x\n", regs[i],
			   readl(adapter->base + regs[i]));

	for (i = 0; i < ZX_RX_QUEUES; i++)
		seq_printf(s, "rxq%d count=%u consumed=%u\n", i,
			   readl(adapter->base + ZX_DMA_RX_QUEUE_CNT(i)) & 0xffff,
			   READ_ONCE(adapter->rx_cur_idx[i]));
	rtnl_unlock();

	return 0;
}

/*
 * The driver cannot be unbound (suppress_bind_attrs) and has no remove
 * function, so the directory stays until reboot.
 */
static void zx_eth_debugfs_init(struct zx_eth_adapter *adapter)
{
	struct dentry *dir = debugfs_create_dir(dev_name(adapter->dev), NULL);

	debugfs_create_devm_seqfile(adapter->dev, "dma_state", dir,
				    zx_eth_dma_state_show);
}

static int zx_eth_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct zx_eth_adapter *adapter;
	struct device_node *np;
	u8 addrs[ZX_NUM_PORTS][ETH_ALEN];
	unsigned long have_addr = 0;
	int i, ret;

	struct device_node *ports_np __free(device_node) =
		of_get_child_by_name(dev->of_node, "ethernet-ports");
	if (!ports_np)
		return dev_err_probe(dev, -ENODEV, "no ethernet-ports node\n");

	/*
	 * The MAC addresses can come from nvmem cells on a flash partition
	 * that is not registered yet.  Get them before the hardware is set
	 * up, so that deferring the probe leaves no DMA running.
	 */
	for_each_available_child_of_node_scoped(ports_np, port_np) {
		u32 port;

		if (of_property_read_u32(port_np, "reg", &port) ||
		    port >= ZX_NUM_PORTS)
			continue;
		ret = of_get_mac_address(port_np, addrs[port]);
		if (ret == -EPROBE_DEFER)
			return ret;
		if (!ret)
			__set_bit(port, &have_addr);
	}

	ret = dma_set_mask_and_coherent(dev, DMA_BIT_MASK(32));
	if (ret)
		return dev_err_probe(dev, ret, "failed to set the DMA mask\n");

	adapter = devm_kzalloc(dev, sizeof(*adapter), GFP_KERNEL);
	if (!adapter)
		return -ENOMEM;

	adapter->irq = platform_get_irq(pdev, 0);
	if (adapter->irq < 0)
		return adapter->irq;

	adapter->dev = dev;
	spin_lock_init(&adapter->tx_lock);
	spin_lock_init(&adapter->bmu_free_lock);
	mutex_init(&adapter->ppe_lock);
	spin_lock_init(&adapter->trap_lock);
	mutex_init(&adapter->sw_lock);
	spin_lock_init(&adapter->sw_fdb_lock);
	INIT_WORK(&adapter->sw_work, zx_sw_work);
	mutex_init(&adapter->mib_lock);
	INIT_DELAYED_WORK(&adapter->mib_work, zx_mib_work);
	adapter->base = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(adapter->base))
		return PTR_ERR(adapter->base);

	ret = rhashtable_init(&adapter->ppe_flows, &zx_ppe_ht_params);
	if (ret)
		return ret;

	adapter->buffer_pool = dma_alloc_coherent(dev, ZX_DMA_ARENA_SIZE,
						  &adapter->buf_pool_dma, GFP_KERNEL);
	if (!adapter->buffer_pool) {
		ret = -ENOMEM;
		goto err_hashtable;
	}
	adapter->tab0 = adapter->buffer_pool + ZX_TAB0_OFFSET;
	adapter->tab0_dma = adapter->buf_pool_dma + ZX_TAB0_OFFSET;
	adapter->tab1 = adapter->buffer_pool + ZX_TAB1_OFFSET;
	adapter->tab1_dma = adapter->buf_pool_dma + ZX_TAB1_OFFSET;
	adapter->rx_ring = adapter->buffer_pool + ZX_RX_OFFSET;
	adapter->rx_ring_dma = adapter->buf_pool_dma + ZX_RX_OFFSET;
	adapter->tx_ring = adapter->buffer_pool + ZX_TX_OFFSET;
	adapter->tx_ring_dma = adapter->buf_pool_dma + ZX_TX_OFFSET;

	ret = zx_hw_init(adapter);
	if (ret)
		goto err_dma;

	/* One netdev per port, named by the port's label */
	for_each_available_child_of_node(ports_np, np) {
		struct net_device *netdev;
		struct zx_eth_priv *priv;
		phy_interface_t interface;
		const char *label;
		u32 port;

		if (of_property_read_u32(np, "reg", &port) || port >= ZX_NUM_PORTS ||
		    adapter->ports[port]) {
			dev_warn(dev, "%pOF: bad or duplicate reg\n", np);
			continue;
		}

		netdev = devm_alloc_etherdev(dev, sizeof(struct zx_eth_priv));
		if (!netdev) {
			ret = -ENOMEM;
			of_node_put(np);
			goto err_disconnect;
		}
		SET_NETDEV_DEV(netdev, dev);
		if (!of_property_read_string(np, "label", &label)) {
			strscpy(netdev->name, label, IFNAMSIZ);
			netdev->name_assign_type = NET_NAME_PREDICTABLE;
		}
		if (test_bit(port, &have_addr))
			eth_hw_addr_set(netdev, addrs[port]);
		else
			eth_hw_addr_random(netdev);
		netdev->priv_flags |= IFF_LIVE_ADDR_CHANGE;
		netdev->netdev_ops = &zx_netdev_ops;
		netdev->ethtool_ops = &zx_ethtool_ops;
		netdev->pcpu_stat_type = NETDEV_PCPU_STAT_DSTATS;
		netdev->hw_features |= NETIF_F_HW_TC;
		netdev->features |= NETIF_F_HW_TC;
		netdev->min_mtu    = 68;
		netdev->max_mtu    = 1500;
		priv = netdev_priv(netdev);
		priv->netdev  = netdev;
		priv->adapter = adapter;
		priv->port    = port;
		adapter->ports[port] = netdev;

		ret = of_get_phy_mode(np, &interface);
		if (ret) {
			dev_err_probe(dev, ret, "%pOF: no valid phy-mode\n", np);
			of_node_put(np);
			goto err_disconnect;
		}
		priv->phylink_config.dev = &netdev->dev;
		priv->phylink_config.type = PHYLINK_NETDEV;
		priv->phylink_config.mac_capabilities = MAC_10 | MAC_100 |
							MAC_1000FD;
		__set_bit(PHY_INTERFACE_MODE_INTERNAL,
			  priv->phylink_config.supported_interfaces);
		__set_bit(PHY_INTERFACE_MODE_GMII,
			  priv->phylink_config.supported_interfaces);
		priv->phylink = phylink_create(&priv->phylink_config,
					       of_fwnode_handle(np), interface,
					       &zx_phylink_mac_ops);
		if (IS_ERR(priv->phylink)) {
			ret = dev_err_probe(dev, PTR_ERR(priv->phylink),
					    "port %u: phylink\n", port);
			priv->phylink = NULL;
			of_node_put(np);
			goto err_disconnect;
		}
		ret = phylink_of_phy_connect(priv->phylink, np, 0);
		if (ret) {
			dev_err_probe(dev, ret, "port %u: PHY connect failed\n", port);
			of_node_put(np);
			goto err_disconnect;
		}
	}

	/* The MAC counters start from zero, whatever the registers hold */
	mutex_lock(&adapter->mib_lock);
	for (i = 0; i < ZX_NUM_PORTS; i++) {
		struct zx_eth_priv *priv;

		if (!adapter->ports[i])
			continue;
		priv = netdev_priv(adapter->ports[i]);
		zx_mib_update(priv);
		memset(priv->mib, 0, sizeof(priv->mib));
	}
	mutex_unlock(&adapter->mib_lock);

	for (i = 0; i < ZX_NUM_PORTS; i++)
		if (adapter->ports[i])
			break;
	if (i == ZX_NUM_PORTS) {
		ret = dev_err_probe(dev, -ENODEV, "no usable port\n");
		goto err_disconnect;
	}

	mutex_lock(&adapter->ppe_lock);
	zx_update_mac_tables(adapter);
	mutex_unlock(&adapter->ppe_lock);

	/* One NAPI context serves all ports */
	netif_napi_add(adapter->ports[i], &adapter->napi, zx_eth_poll);
	hrtimer_setup(&adapter->tx_timer, zx_eth_tx_timer,
		      CLOCK_MONOTONIC, HRTIMER_MODE_REL);

	/* zx_hw_init() masked the interrupt, a port's open enables it */
	ret = devm_request_irq(dev, adapter->irq, zx_eth_isr, 0, dev_name(dev),
			       adapter);
	if (ret)
		goto err_napi;

	/* Start DMA only after every port, NAPI and the interrupt are ready. */
	zx_dma_enable(adapter, true);

	for (i = 0; i < ZX_NUM_PORTS; i++) {
		if (!adapter->ports[i])
			continue;
		ret = register_netdev(adapter->ports[i]);
		if (ret) {
			dev_err(dev, "port %d: register_netdev failed: %d\n", i, ret);
			while (--i >= 0)
				if (adapter->ports[i])
					unregister_netdev(adapter->ports[i]);
			goto err_irq;
		}
	}

	platform_set_drvdata(pdev, adapter);

	zx_sw_init(adapter);
	schedule_delayed_work(&adapter->mib_work, ZX_MIB_INTERVAL);

	zx_eth_debugfs_init(adapter);
	return 0;

err_irq:
	writel(~0, adapter->base + ZX_TM_INT_MASK);
	devm_free_irq(dev, adapter->irq, adapter);
err_napi:
	netif_napi_del(&adapter->napi);
err_disconnect:
	for (i = 0; i < ZX_NUM_PORTS; i++) {
		struct zx_eth_priv *priv;

		if (!adapter->ports[i])
			continue;
		priv = netdev_priv(adapter->ports[i]);
		if (priv->phylink) {
			phylink_disconnect_phy(priv->phylink);
			phylink_destroy(priv->phylink);
		}
	}
err_dma:
	/* A failed or deferred probe must leave no DMA targeting this arena. */
	zx_hw_reset(adapter);
	dma_free_coherent(dev, ZX_DMA_ARENA_SIZE, adapter->buffer_pool,
			  adapter->buf_pool_dma);
err_hashtable:
	rhashtable_destroy(&adapter->ppe_flows);
	return ret;
}

/* ---- module glue --------------------------------------------------- */

/*
 * The switch writes received frames, and frames of offloaded flows, into
 * the buffer pool without the CPU.  Stop it before a reboot or kexec: after
 * kexec, that memory belongs to the next kernel.
 */
static void zx_eth_shutdown(struct platform_device *pdev)
{
	struct zx_eth_adapter *adapter = platform_get_drvdata(pdev);
	int i;

	if (!adapter)
		return;

	rtnl_lock();
	for (i = 0; i < ZX_NUM_PORTS; i++) {
		if (!adapter->ports[i])
			continue;
		netif_device_detach(adapter->ports[i]);
		dev_close(adapter->ports[i]);
	}
	rtnl_unlock();
	cancel_work_sync(&adapter->sw_work);
	cancel_delayed_work_sync(&adapter->mib_work);

	zx_hw_reset(adapter);
}

static const struct of_device_id zx_eth_dt_ids[] = {
	{ .compatible = "zte,zx279128s-gmac" },
	{ }
};
MODULE_DEVICE_TABLE(of, zx_eth_dt_ids);

static struct platform_driver zx_eth_driver = {
	.probe  = zx_eth_probe,
	.shutdown = zx_eth_shutdown,
	.driver = {
		.name           = DRV_NAME,
		.of_match_table = zx_eth_dt_ids,
		.suppress_bind_attrs = true,
	},
};
module_platform_driver(zx_eth_driver);

MODULE_AUTHOR("Navid Ghahremani <ghahramani.navid@gmail.com>");
MODULE_DESCRIPTION("ZTE zx279128s Ethernet switch and DMA driver");
MODULE_LICENSE("GPL");

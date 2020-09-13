// SPDX-License-Identifier: GPL-2.0-only
/*
 * linux/drivers/net/ethernet/rtl838x_eth.c
 * Copyright (C) 2020 B. Koblitz
 */

#include <linux/dma-mapping.h>
#include <linux/etherdevice.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/platform_device.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/of.h>
#include <linux/of_net.h>
#include <linux/of_mdio.h>
#include <linux/module.h>
#include <linux/phylink.h>
#include <net/dsa.h>

#include <asm/mach-rtl838x/mach-rtl838x.h>
#include "rtl838x_eth.h"

extern struct rtl838x_soc_info soc_info;

/*
 * Maximum number of RX rings is 8, assigned by switch based on
 * packet/port priortity (not implemented)
 * Maximum number of TX rings is 2 (only ring 0 used)
 * RX ringlength needs to be at least 200, otherwise CPU and Switch
 * may gridlock.
 */
#define RXRINGS		2
#define RXRINGLEN	300
#define TXRINGS		2
#define TXRINGLEN	160
#define TX_EN		0x8
#define RX_EN		0x4
#define TX_DO		0x2
#define WRAP		0x2

#define RING_BUFFER	1600

struct p_hdr {
	uint8_t		*buf;
	uint16_t	reserved;
	uint16_t	size;   /* buffer size */
	uint16_t	offset;
	uint16_t	len;    /* pkt len */
	uint16_t	reserved2;
	uint16_t	cpu_tag[5];
} __packed __aligned(1);

struct ring_b {
	uint32_t	rx_r[RXRINGS][RXRINGLEN];
	uint32_t	tx_r[TXRINGS][TXRINGLEN];
	struct	p_hdr	rx_header[RXRINGS][RXRINGLEN];
	struct	p_hdr	tx_header[TXRINGS][TXRINGLEN];
	uint32_t	c_rx[RXRINGS];
	uint32_t	c_tx[TXRINGS];
	uint8_t		rx_space[RXRINGS*RXRINGLEN*RING_BUFFER];
	uint8_t		tx_space[TXRINGLEN*RING_BUFFER];
};

struct rtl838x_eth_priv {
	struct net_device *netdev;
	struct platform_device *pdev;
	void		*membase;
	spinlock_t	lock;
	struct mii_bus	*mii_bus;
	struct napi_struct napi;
	struct phylink *phylink;
	struct phylink_config phylink_config;
	u16 id;
	u16 family_id;
	const struct rtl838x_reg *r;
	u8 cpu_port;
};

static const struct rtl838x_reg rtl838x_reg = {
	.mac_port_ctrl = rtl838x_mac_port_ctrl,
	.dma_if_intr_sts = RTL838X_DMA_IF_INTR_STS,
	.dma_if_intr_msk = RTL838X_DMA_IF_INTR_MSK,
	.dma_if_ctrl = RTL838X_DMA_IF_CTRL,
	.mac_force_mode_ctrl = rtl838x_mac_force_mode_ctrl,
	.dma_rx_base = rtl838x_dma_rx_base,
	.dma_tx_base = rtl838x_dma_tx_base,
	.dma_if_rx_ring_size = rtl838x_dma_if_rx_ring_size,
	.dma_if_rx_ring_cntr = rtl838x_dma_if_rx_ring_cntr,
	.dma_if_rx_cur = rtl838x_dma_if_rx_cur,
	.rst_glb_ctrl = RTL838X_RST_GLB_CTRL_0,
	.get_mac_link_sts = rtl838x_get_mac_link_sts,
	.get_mac_link_dup_sts = rtl838x_get_mac_link_dup_sts,
	.get_mac_link_spd_sts = rtl838x_get_mac_link_spd_sts,
	.get_mac_rx_pause_sts = rtl838x_get_mac_rx_pause_sts,
	.get_mac_tx_pause_sts = rtl838x_get_mac_tx_pause_sts,
	.mac = RTL838X_MAC,
	.l2_tbl_flush_ctrl = RTL838X_L2_TBL_FLUSH_CTRL,
};

static const struct rtl838x_reg rtl839x_reg = {
	.mac_port_ctrl = rtl839x_mac_port_ctrl,
	.dma_if_intr_sts = RTL839X_DMA_IF_INTR_STS,
	.dma_if_intr_msk = RTL839X_DMA_IF_INTR_MSK,
	.dma_if_ctrl = RTL839X_DMA_IF_CTRL,
	.mac_force_mode_ctrl = rtl839x_mac_force_mode_ctrl,
	.dma_rx_base = rtl839x_dma_rx_base,
	.dma_tx_base = rtl839x_dma_tx_base,
	.dma_if_rx_ring_size = rtl839x_dma_if_rx_ring_size,
	.dma_if_rx_ring_cntr = rtl839x_dma_if_rx_ring_cntr,
	.dma_if_rx_cur = rtl839x_dma_if_rx_cur,
	.rst_glb_ctrl = RTL839X_RST_GLB_CTRL,
	.get_mac_link_sts = rtl839x_get_mac_link_sts,
	.get_mac_link_dup_sts = rtl839x_get_mac_link_dup_sts,
	.get_mac_link_spd_sts = rtl839x_get_mac_link_spd_sts,
	.get_mac_rx_pause_sts = rtl839x_get_mac_rx_pause_sts,
	.get_mac_tx_pause_sts = rtl839x_get_mac_tx_pause_sts,
	.mac = RTL839X_MAC,
	.l2_tbl_flush_ctrl = RTL839X_L2_TBL_FLUSH_CTRL,
};

/*
 * Discard the RX ring-buffers, called as part of the net-ISR
 * when the buffer runs over
 * Caller needs to hold priv->lock
 */
static void rtl838x_rb_cleanup(struct rtl838x_eth_priv *priv)
{
	int r;
	u32	*last;
	struct p_hdr *h;
	struct ring_b *ring = priv->membase;

	for (r = 0; r < RXRINGS; r++) {
		last = (u32 *)KSEG1ADDR(sw_r32(priv->r->dma_if_rx_cur(r)));
		do {
			if ((ring->rx_r[r][ring->c_rx[r]] & 0x1))
				break;
			h = &ring->rx_header[r][ring->c_rx[r]];
			h->buf = (u8 *)CPHYSADDR(ring->rx_space
					+ r * ring->c_rx[r] * RING_BUFFER);
			h->size = RING_BUFFER;
			h->len = 0;
			/* make sure the header is visible to the ASIC */
			mb();

			ring->rx_r[r][ring->c_rx[r]] = CPHYSADDR(h) | 0x1
				| (ring->c_rx[r] == (RXRINGLEN-1) ? WRAP : 0x1);
			ring->c_rx[r] = (ring->c_rx[r] + 1) % RXRINGLEN;
		} while (&ring->rx_r[r][ring->c_rx[r]] != last);
	}
}

static irqreturn_t rtl838x_net_irq(int irq, void *dev_id)
{
	struct net_device *dev = dev_id;
	struct rtl838x_eth_priv *priv = netdev_priv(dev);
	u32 status = sw_r32(priv->r->dma_if_intr_sts);

	spin_lock(&priv->lock);
	/*  Ignore TX interrupt */
	if ((status & 0xf0000)) {
		/* Clear ISR */
		sw_w32(0x000f0000, priv->r->dma_if_intr_sts);
	}

	/* RX interrupt */
	if (status & 0x0ff00) {
		/* Disable RX interrupt */
		sw_w32_mask(0xff00, 0, priv->r->dma_if_intr_msk);
		sw_w32(0x0000ff00, priv->r->dma_if_intr_sts);
		napi_schedule(&priv->napi);
	}

	/* RX buffer overrun */
	if (status & 0x000ff) {
		sw_w32(0x000000ff, priv->r->dma_if_intr_sts);
		rtl838x_rb_cleanup(priv);
	}

	spin_unlock(&priv->lock);
	return IRQ_HANDLED;
}

static void rtl838x_hw_reset(struct rtl838x_eth_priv *priv)
{
	u32 int_saved;

	pr_info("RESETTING %x, CPU_PORT %d\n", priv->family_id, priv->cpu_port);
	/* Stop TX/RX */
	sw_w32(0x0, priv->r->mac_port_ctrl(priv->cpu_port));
	mdelay(500);

	int_saved = sw_r32(priv->r->dma_if_intr_msk);
	/* Reset NIC */
	sw_w32(0x08, priv->r->rst_glb_ctrl);
	do {
		udelay(20);
	} while (sw_r32(priv->r->rst_glb_ctrl) & 0x08);
	mdelay(100);

	/* Restore notification settings: on RTL838x these bits are null */
	sw_w32_mask(7 << 20, int_saved & (7 << 20), priv->r->dma_if_intr_msk);

	/* Restart TX/RX to CPU port */
	sw_w32(0x03, priv->r->mac_port_ctrl(priv->cpu_port));

	if (priv->family_id == RTL8380_FAMILY_ID) {
		/* Set Speed, duplex, flow control
		 * FORCE_EN | LINK_EN | NWAY_EN | DUP_SEL
		 * | SPD_SEL = 0b10 | FORCE_FC_EN | PHY_MASTER_SLV_MANUAL_EN
		 * | MEDIA_SEL
		 */
		sw_w32(0x6192F, priv->r->mac_force_mode_ctrl(priv->cpu_port));
		/* allow CRC errors on CPU-port */
		sw_w32_mask(0, 0x8, priv->r->mac_port_ctrl(priv->cpu_port));
	} else {
		/* Force CPU port link up */
		sw_w32_mask(0, 3, priv->r->mac_force_mode_ctrl(priv->cpu_port));
	}

	/* Disable and clear interrupts */
	sw_w32(0x00000000, priv->r->dma_if_intr_msk);
	sw_w32(0xffffffff, priv->r->dma_if_intr_sts);
}

static void rtl838x_hw_ring_setup(struct rtl838x_eth_priv *priv)
{
	int i;
	struct ring_b *ring = priv->membase;

	for (i = 0; i < RXRINGS; i++)
		sw_w32(CPHYSADDR(&ring->rx_r[i]), priv->r->dma_rx_base(i));

	for (i = 0; i < TXRINGS; i++)
		sw_w32(CPHYSADDR(&ring->tx_r[i]), priv->r->dma_tx_base(i));
}

static void rtl838x_hw_en_rxtx(struct rtl838x_eth_priv *priv)
{
	u32 v;

	pr_info("%s\n", __func__);
	/* Disable Head of Line features for all RX rings */
	sw_w32(0xffffffff, priv->r->dma_if_rx_ring_size(0));

	/* Truncate RX buffer to 0x640 (1600) bytes, pad TX */
	sw_w32(0x06400020, priv->r->dma_if_ctrl);

	/* Enable RX done, RX overflow and TX done interrupts */
	sw_w32(0xfffff, priv->r->dma_if_intr_msk);

	/* Enable traffic, engine expects empty FCS field */
	sw_w32_mask(0, RX_EN | TX_EN, priv->r->dma_if_ctrl);

	/* Make sure to flood all traffic to CPU_PORT */
	if (priv->family_id == RTL8390_FAMILY_ID) {
		/* CPU port joins Lookup Miss Flooding Portmask */
		/* Table access: CMD: read, table = 2 */
		/* Sets MC_PMSK table port bit for port 52 to 1 */
		sw_w32(0x28000, RTL839X_TBL_ACCESS_L2_CTRL);
		do { } while (sw_r32(RTL839X_TBL_ACCESS_L2_CTRL) & (1 << 17));
		v = sw_r32(RTL839X_TBL_ACCESS_L2_DATA(0));
		sw_w32(v | 0x80000000, RTL839X_TBL_ACCESS_L2_DATA(0));
		sw_w32(0x38000, RTL839X_TBL_ACCESS_L2_CTRL);
		do { } while (sw_r32(RTL839X_TBL_ACCESS_L2_CTRL) & (1 << 17));
	}
}

static void rtl838x_setup_ring_buffer(struct ring_b *ring)
{
	int i, j;

	struct p_hdr *h;

	for (i = 0; i < RXRINGS; i++) {
		for (j = 0; j < RXRINGLEN; j++) {
			h = &ring->rx_header[i][j];
			h->buf = (u8 *)CPHYSADDR(ring->rx_space + i * j * RING_BUFFER);
			h->reserved = 0;
			h->size = RING_BUFFER;
			h->offset = 0;
			h->len = 0;
			/* All rings owned by switch, last one wraps */
			ring->rx_r[i][j] = CPHYSADDR(h) | 1 | (j == (RXRINGLEN - 1) ? WRAP : 0);
		}
		ring->c_rx[i] = 0;
	}

	for (i = 0; i < TXRINGS; i++) {
		for (j = 0; j < TXRINGLEN; j++) {
			h = &ring->tx_header[i][j];
			h->buf = (u8 *)CPHYSADDR(ring->tx_space + i * j * RING_BUFFER);
			h->reserved = 0;
			h->size = RING_BUFFER;
			h->offset = 0;
			h->len = 0;
			ring->tx_r[i][j] = CPHYSADDR(&ring->tx_header[i][j]);
		}
		/* Last header is wrapping around */
		ring->tx_r[i][j-1] |= 2;
		ring->c_tx[i] = 0;
	}
}

static int rtl838x_eth_open(struct net_device *ndev)
{
	unsigned long flags;
	struct rtl838x_eth_priv *priv = netdev_priv(ndev);
	struct ring_b *ring = priv->membase;
	int err;

	pr_info("%s called %x, ring %x\n", __func__, (uint32_t)priv, (uint32_t)ring);
	spin_lock_irqsave(&priv->lock, flags);
	rtl838x_hw_reset(priv);
	rtl838x_setup_ring_buffer(ring);
	rtl838x_hw_ring_setup(priv);
	err = request_irq(ndev->irq, rtl838x_net_irq, IRQF_SHARED,
			ndev->name, ndev);
	if (err) {
		netdev_err(ndev, "%s: could not acquire interrupt: %d\n",
			   __func__, err);
		return err;
	}
	phylink_start(priv->phylink);

	napi_enable(&priv->napi);
	netif_start_queue(ndev);

	rtl838x_hw_en_rxtx(priv);

	spin_unlock_irqrestore(&priv->lock, flags);

	return 0;
}

static void rtl838x_hw_stop(struct rtl838x_eth_priv *priv)
{
	int i;

	/* Block all ports */
	if (priv->family_id == RTL8380_FAMILY_ID) {
		sw_w32(0x03000000, RTL838X_TBL_ACCESS_DATA_0(0));
		sw_w32(0x00000000, RTL838X_TBL_ACCESS_DATA_0(1));
		sw_w32(1 << 15 | 2 << 12, RTL838X_TBL_ACCESS_CTRL_0);
	}

	/* Flush L2 address cache */
	if (priv->family_id == RTL8380_FAMILY_ID) {
		for (i = 0; i <= priv->cpu_port; i++) {
			sw_w32(1 << 26 | 1 << 23 | i << 5, priv->r->l2_tbl_flush_ctrl);
			do { } while (sw_r32(priv->r->l2_tbl_flush_ctrl) & (1 << 26));
		}
	} else {
		for (i = 0; i <= priv->cpu_port; i++) {
			sw_w32(1 << 28 | 1 << 25 | i << 5, priv->r->l2_tbl_flush_ctrl);
			do { } while (sw_r32(priv->r->l2_tbl_flush_ctrl) & (1 << 28));
		}
	}

	/* CPU-Port: Link down BUG: Works only for RTL838x */
	sw_w32(0x6192D, priv->r->mac_force_mode_ctrl(priv->cpu_port));
	mdelay(100);

	/* Disable traffic */
	sw_w32_mask(RX_EN | TX_EN, 0, priv->r->dma_if_ctrl);
	mdelay(200);

	/* Disable all TX/RX interrupts */
	sw_w32(0x00000000, priv->r->dma_if_intr_msk);
	sw_w32(0x000fffff, priv->r->dma_if_intr_sts);
	sw_w32(0x00000000, priv->r->dma_if_ctrl);
	mdelay(200);
}

static int rtl838x_eth_stop(struct net_device *ndev)
{
	unsigned long flags;
	struct rtl838x_eth_priv *priv = netdev_priv(ndev);

	pr_info("in %s\n", __func__);

	spin_lock_irqsave(&priv->lock, flags);
	phylink_stop(priv->phylink);
	rtl838x_hw_stop(priv);
	free_irq(ndev->irq, ndev);
	napi_disable(&priv->napi);
	netif_stop_queue(ndev);
	spin_unlock_irqrestore(&priv->lock, flags);
	return 0;
}

static void rtl838x_eth_set_multicast_list(struct net_device *dev)
{

}

static void rtl838x_eth_tx_timeout(struct net_device *ndev)
{
	unsigned long flags;
	struct rtl838x_eth_priv *priv = netdev_priv(ndev);

	pr_info("in %s\n", __func__);
	spin_lock_irqsave(&priv->lock, flags);
	rtl838x_hw_stop(priv);
	rtl838x_hw_ring_setup(priv);
	rtl838x_hw_en_rxtx(priv);
	netif_trans_update(ndev);
	netif_start_queue(ndev);
	spin_unlock_irqrestore(&priv->lock, flags);
}

static int rtl838x_eth_tx(struct sk_buff *skb, struct net_device *dev)
{
	int len, i;
	struct rtl838x_eth_priv *priv = netdev_priv(dev);
	struct ring_b *ring = priv->membase;
	uint32_t val;
	int ret;
	unsigned long flags;
	struct p_hdr *h;
	int dest_port = -1;

	spin_lock_irqsave(&priv->lock, flags);
	len = skb->len;

	/* Check for DSA tagging at the end of the buffer */
	if (netdev_uses_dsa(dev) && skb->data[len-4] == 0x80 && skb->data[len-3] > 0
			&& skb->data[len-3] < 28 &&  skb->data[len-2] == 0x10
			&&  skb->data[len-1] == 0x00) {
		/* Reuse tag space for CRC */
		dest_port = skb->data[len-3];
		len -= 4;
	}
	if (len < ETH_ZLEN)
		len = ETH_ZLEN;

	/* ASIC expects that packet includes CRC, so we extend by 4 bytes */
	len += 4;

	if (skb_padto(skb, len)) {
		ret = NETDEV_TX_OK;
		goto txdone;
	}

	/* We can send this packet if CPU owns the descriptor */
	if (!(ring->tx_r[0][ring->c_tx[0]] & 0x1)) {
		/* Set descriptor for tx */
		h = &ring->tx_header[0][ring->c_tx[0]];

		h->buf = (u8 *)CPHYSADDR(ring->tx_space);
		h->size = len;
		h->len = len;

		/* Create cpu_tag */
		if (dest_port > 0) {
			h->cpu_tag[0] = 0x0400;
			h->cpu_tag[1] = 0x0200;
			h->cpu_tag[2] = 0x0000;
			h->cpu_tag[3] = (1 << dest_port) >> 16;
			h->cpu_tag[4] = (1 << dest_port) & 0xffff;
		} else {
			h->cpu_tag[0] = 0;
			h->cpu_tag[1] = 0;
			h->cpu_tag[2] = 0;
			h->cpu_tag[3] = 0;
			h->cpu_tag[4] = 0;
		}

		/* Copy packet data to tx buffer */
		memcpy((void *)KSEG1ADDR(h->buf), skb->data, len);
		/* Make sure packet data is visible to ASIC */
		mb();

		/* Hand over to switch */
		ring->tx_r[0][ring->c_tx[0]] = ring->tx_r[0][ring->c_tx[0]] | 0x1;

		/* BUG: before tx fetch, need to make sure right data is accessed */
		for (i = 0; i < 10; i++) {
			val = sw_r32(priv->r->dma_if_ctrl);
			if ((val & 0xc) == 0xc)
				break;
		}

		/* Tell switch to send data */
		sw_w32_mask(0, TX_DO, priv->r->dma_if_ctrl);

		dev->stats.tx_packets++;
		dev->stats.tx_bytes += len;
		dev_kfree_skb(skb);
		ring->c_tx[0] = (ring->c_tx[0] + 1) % TXRINGLEN;
		ret = NETDEV_TX_OK;
	} else {
		dev_warn(&priv->pdev->dev, "Data is owned by switch\n");
		ret = NETDEV_TX_BUSY;
	}
txdone:
	spin_unlock_irqrestore(&priv->lock, flags);
	return ret;
}

static int rtl838x_hw_receive(struct net_device *dev, int r, int budget)
{
	struct rtl838x_eth_priv *priv = netdev_priv(dev);
	struct ring_b *ring = priv->membase;
	struct sk_buff *skb;
	unsigned long flags;
	int i, len, work_done = 0;
	u8 *data, *skb_data;
	unsigned int val;
	u32	*last;
	struct p_hdr *h;
	bool dsa = netdev_uses_dsa(dev);

	spin_lock_irqsave(&priv->lock, flags);
	last = (u32 *)KSEG1ADDR(sw_r32(priv->r->dma_if_rx_cur(r)));

	if (&ring->rx_r[r][ring->c_rx[r]] == last) {
		spin_unlock_irqrestore(&priv->lock, flags);
		return 0;
	}
	do {
		if ((ring->rx_r[r][ring->c_rx[r]] & 0x1)) {
			netdev_warn(dev, "WARNING Ring contention: ring %x, last %x, current %x, cPTR %x, ISR %x\n", r, (uint32_t)last,
				    (u32) &ring->rx_r[r][ring->c_rx[r]],
				    ring->rx_r[r][ring->c_rx[r]],
				sw_r32(priv->r->dma_if_intr_sts));
			break;
		}

		h = &ring->rx_header[r][ring->c_rx[r]];
		data = (u8 *)KSEG1ADDR(h->buf);
		len = h->len;

		if (!len)
			break;
		h->buf = (u8 *)CPHYSADDR(ring->rx_space
				+ r * ring->c_rx[r] * RING_BUFFER);
		h->size = RING_BUFFER;
		h->len = 0;
		work_done++;

		len -= 4; /* strip the CRC */
		/* Add 4 bytes for cpu_tag */
		if (dsa)
			len += 4;

		skb = alloc_skb(len + 4, GFP_KERNEL);
		skb_reserve(skb, NET_IP_ALIGN);

		if (likely(skb)) {
			/* BUG: Prevent ASIC bug */
			sw_w32(0xffffffff, priv->r->dma_if_rx_ring_size(0));
			for (i = 0; i < RXRINGS; i++) {
				/* Update each ring cnt */
				val = sw_r32(priv->r->dma_if_rx_ring_cntr(i));
				sw_w32(val, priv->r->dma_if_rx_ring_cntr(i));
			}

			skb_data = skb_put(skb, len);
			/* Make sure data is visible */
			mb();
			memcpy(skb->data, (u8 *)KSEG1ADDR(data), len);
			/* Overwrite CRC with cpu_tag */
			if (dsa) {
				skb->data[len-4] = 0x80;
				skb->data[len-3] = h->cpu_tag[0] & 0x1f;
				skb->data[len-2] = 0x10;
				skb->data[len-1] = 0x00;
			}

			skb->protocol = eth_type_trans(skb, dev);
			dev->stats.rx_packets++;
			dev->stats.rx_bytes += len;

			netif_receive_skb(skb);
		} else {
			if (net_ratelimit())
				dev_warn(&dev->dev, "low on memory - packet dropped\n");
			dev->stats.rx_dropped++;
		}
		ring->rx_r[r][ring->c_rx[r]]
			= CPHYSADDR(h) | 0x1 | (ring->c_rx[r] == (RXRINGLEN-1) ? WRAP : 0x1);
		ring->c_rx[r] = (ring->c_rx[r] + 1) % RXRINGLEN;
	} while (&ring->rx_r[r][ring->c_rx[r]] != last && work_done < budget);

	spin_unlock_irqrestore(&priv->lock, flags);
	return work_done;
}

static int rtl838x_poll_rx(struct napi_struct *napi, int budget)
{
	struct rtl838x_eth_priv *priv = container_of(napi, struct rtl838x_eth_priv, napi);
	int work_done = 0, r = 0;

	while (work_done < budget && r < RXRINGS) {
		work_done += rtl838x_hw_receive(priv->netdev, r, budget - work_done);
		r++;
	}

	if (work_done < budget) {
		napi_complete_done(napi, work_done);
		/* Enable RX interrupt */
		sw_w32(0xfffff, priv->r->dma_if_intr_msk);
	}
	return work_done;
}


static void rtl838x_validate(struct phylink_config *config,
			 unsigned long *supported,
			 struct phylink_link_state *state)
{
	__ETHTOOL_DECLARE_LINK_MODE_MASK(mask) = { 0, };

	pr_info("In %s\n", __func__);

	if (!phy_interface_mode_is_rgmii(state->interface) &&
	    state->interface != PHY_INTERFACE_MODE_1000BASEX &&
	    state->interface != PHY_INTERFACE_MODE_MII &&
	    state->interface != PHY_INTERFACE_MODE_REVMII &&
	    state->interface != PHY_INTERFACE_MODE_GMII &&
	    state->interface != PHY_INTERFACE_MODE_QSGMII &&
	    state->interface != PHY_INTERFACE_MODE_INTERNAL &&
	    state->interface != PHY_INTERFACE_MODE_SGMII) {
		bitmap_zero(supported, __ETHTOOL_LINK_MODE_MASK_NBITS);
		pr_err("Unsupported interface: %d\n", state->interface);
		return;
	}

	/* Allow all the expected bits */
	phylink_set(mask, Autoneg);
	phylink_set_port_modes(mask);
	phylink_set(mask, Pause);
	phylink_set(mask, Asym_Pause);

	/* With the exclusion of MII and Reverse MII, we support Gigabit,
	 * including Half duplex
	 */
	if (state->interface != PHY_INTERFACE_MODE_MII &&
	    state->interface != PHY_INTERFACE_MODE_REVMII) {
		phylink_set(mask, 1000baseT_Full);
		phylink_set(mask, 1000baseT_Half);
	}

	phylink_set(mask, 10baseT_Half);
	phylink_set(mask, 10baseT_Full);
	phylink_set(mask, 100baseT_Half);
	phylink_set(mask, 100baseT_Full);

	bitmap_and(supported, supported, mask,
		   __ETHTOOL_LINK_MODE_MASK_NBITS);
	bitmap_and(state->advertising, state->advertising, mask,
		   __ETHTOOL_LINK_MODE_MASK_NBITS);
}


static void rtl838x_mac_config(struct phylink_config *config,
			       unsigned int mode,
			       const struct phylink_link_state *state)
{
	/* This is only being called for the master device,
	 * i.e. the CPU-Port
	 */

	pr_info("In %s, mode %x\n", __func__, mode);
}

static void rtl838x_mac_an_restart(struct phylink_config *config)
{
	struct net_device *dev = container_of(config->dev, struct net_device, dev);
	struct rtl838x_eth_priv *priv = netdev_priv(dev);

	pr_info("In %s\n", __func__);
	/* Restart by disabling and re-enabling link */
	sw_w32(0x6192D, priv->r->mac_force_mode_ctrl(priv->cpu_port));
	mdelay(20);
	sw_w32(0x6192F, priv->r->mac_force_mode_ctrl(priv->cpu_port));
}

static int rtl838x_mac_pcs_get_state(struct phylink_config *config,
				  struct phylink_link_state *state)
{
	u32 speed;
	struct net_device *dev = container_of(config->dev, struct net_device, dev);
	struct rtl838x_eth_priv *priv = netdev_priv(dev);
	int port = priv->cpu_port;

	pr_info("In %s\n", __func__);

	state->link = priv->r->get_mac_link_sts(port) ? 1 : 0;
	state->duplex = priv->r->get_mac_link_dup_sts(port) ? 1 : 0;

	speed = priv->r->get_mac_link_spd_sts(port);
	switch (speed) {
	case 0:
		state->speed = SPEED_10;
		break;
	case 1:
		state->speed = SPEED_100;
		break;
		state->speed = SPEED_1000;
		break;
	default:
		state->speed = SPEED_UNKNOWN;
		break;
	}

	state->pause &= (MLO_PAUSE_RX | MLO_PAUSE_TX);
	if (priv->r->get_mac_rx_pause_sts(port))
		state->pause |= MLO_PAUSE_RX;
	if (priv->r->get_mac_tx_pause_sts(port))
		state->pause |= MLO_PAUSE_TX;

	return 1;
}

static void dump_mac_conf(struct rtl838x_eth_priv *priv)
{
	int p;

	for (p = 8; p < 16; p++) {
		pr_debug("%d: %x, force %x\n", p, sw_r32(priv->r->mac_port_ctrl(p)),
					     sw_r32(priv->r->mac_force_mode_ctrl(p))
		);
	}
	pr_debug("CPU: %x, force %x\n", sw_r32(priv->r->mac_port_ctrl(priv->cpu_port)),
					sw_r32(priv->r->mac_force_mode_ctrl(priv->cpu_port))
		);
}

static void rtl838x_mac_link_down(struct phylink_config *config,
				  unsigned int mode,
				  phy_interface_t interface)
{
	struct net_device *dev = container_of(config->dev, struct net_device, dev);
	struct rtl838x_eth_priv *priv = netdev_priv(dev);

	pr_info("In %s\n", __func__);
	/* Stop TX/RX to port */
	sw_w32_mask(0x03, 0, priv->r->mac_port_ctrl(priv->cpu_port));
}

static void rtl838x_mac_link_up(struct phylink_config *config, unsigned int mode,
			    phy_interface_t interface,
			    struct phy_device *phy)
{
	struct net_device *dev = container_of(config->dev, struct net_device, dev);
	struct rtl838x_eth_priv *priv = netdev_priv(dev);

	pr_info("In %s\n", __func__);
	/* Restart TX/RX to port */
	sw_w32_mask(0, 0x03, priv->r->mac_port_ctrl(priv->cpu_port));
}

static void rtl838x_set_mac_hw(struct net_device *dev, u8 *mac)
{
	struct rtl838x_eth_priv *priv = netdev_priv(dev);
	unsigned long flags;

	spin_lock_irqsave(&priv->lock, flags);
	pr_info("In %s\n", __func__);
	sw_w32((mac[0] << 8) | mac[1], priv->r->mac);
	sw_w32((mac[2] << 24) | (mac[3] << 16) | (mac[4] << 8) | mac[5], priv->r->mac + 4);

	if (priv->family_id == RTL8380_FAMILY_ID) {
		/* 2 more registers, ALE/MAC block */
		sw_w32((mac[0] << 8) | mac[1], RTL838X_MAC_ALE);
		sw_w32((mac[2] << 24) | (mac[3] << 16) | (mac[4] << 8) | mac[5],
		       (RTL838X_MAC_ALE + 4));

		sw_w32((mac[0] << 8) | mac[1], RTL838X_MAC2);
		sw_w32((mac[2] << 24) | (mac[3] << 16) | (mac[4] << 8) | mac[5],
		       RTL838X_MAC2 + 4);
	}
	spin_unlock_irqrestore(&priv->lock, flags);
}

static int rtl838x_set_mac_address(struct net_device *dev, void *p)
{
	struct rtl838x_eth_priv *priv = netdev_priv(dev);
	const struct sockaddr *addr = p;
	u8 *mac = (u8 *) (addr->sa_data);

	if (!is_valid_ether_addr(addr->sa_data))
		return -EADDRNOTAVAIL;

	memcpy(dev->dev_addr, addr->sa_data, ETH_ALEN);
	rtl838x_set_mac_hw(dev, mac);

	pr_info("Using MAC %08x%08x\n", sw_r32(priv->r->mac), sw_r32(priv->r->mac + 4));
	return 0;
}

static int rtl8390_init_mac(struct rtl838x_eth_priv *priv)
{
	pr_info("Configuring RTL8390 MAC\n");
	// from mac_config_init
	sw_w32(0x80,  RTL839X_MAC_EFUSE_CTRL);
	sw_w32(0x4, RTL839X_RST_GLB_CTRL);
	sw_w32(0x3c324f40, RTL839X_MAC_GLB_CTRL);
	/* Unlimited egress rate */
	sw_w32(0x1297b961, RTL839X_SCHED_LB_TICK_TKN_CTRL);

	return 0;
}

static int rtl8380_init_mac(struct rtl838x_eth_priv *priv)
{
	int i;

	if (priv->family_id == 0x8390)
		return rtl8390_init_mac(priv);

	if (priv->family_id != 0x8380)
		return 0;

	pr_info("%s\n", __func__);
	/* fix timer for EEE */
	sw_w32(0x5001411, RTL838X_EEE_TX_TIMER_GIGA_CTRL);
	sw_w32(0x5001417, RTL838X_EEE_TX_TIMER_GELITE_CTRL);

	/* Init VLAN */
	if (priv->id == 0x8382) {
		for (i = 0; i <= 28; i++)
			sw_w32(0, 0xd57c + i * 0x80);
	}
	if (priv->id == 0x8380) {
		for (i = 8; i <= 28; i++)
			sw_w32(0, 0xd57c + i * 0x80);
	}
	return 0;
}

static int rtl838x_get_link_ksettings(struct net_device *ndev,
				      struct ethtool_link_ksettings *cmd)
{
	struct rtl838x_eth_priv *priv = netdev_priv(ndev);

	pr_info("%s called\n", __func__);
	return phylink_ethtool_ksettings_get(priv->phylink, cmd);
}

static int rtl838x_set_link_ksettings(struct net_device *ndev,
				      const struct ethtool_link_ksettings *cmd)
{
	struct rtl838x_eth_priv *priv = netdev_priv(ndev);

	pr_info("%s called\n", __func__);
	return phylink_ethtool_ksettings_set(priv->phylink, cmd);
}


static int rtl838x_mdio_read(struct mii_bus *bus, int mii_id, int regnum)
{
	u32 val;
	u32 offset = 0;
	int err;
	struct rtl838x_eth_priv *priv = bus->priv;

	if (mii_id >= 24 && mii_id <= 27 && priv->id == 0x8380) {
		if (mii_id == 26)
			offset = 0x100;
		val = sw_r32(MAPLE_SDS4_FIB_REG0r + offset + (regnum << 2)) & 0xffff;
		return val;
	}
	err = rtl838x_read_phy(mii_id, 0, regnum, &val);
	if (err)
		return err;
	return val;
}

static int rtl839x_mdio_read(struct mii_bus *bus, int mii_id, int regnum)
{
	u32 val;
	int err;

	err = rtl839x_read_phy(mii_id, 0, regnum, &val);
	if (err)
		return err;
	return val;
}

static int rtl838x_mdio_write(struct mii_bus *bus, int mii_id,
			      int regnum, u16 value)
{
	u32 offset = 0;
	struct rtl838x_eth_priv *priv = bus->priv;

	if (mii_id >= 24 && mii_id <= 27 && priv->id == 0x8380) {
		if (mii_id == 26)
			offset = 0x100;
		sw_w32(value, MAPLE_SDS4_FIB_REG0r + offset + (regnum << 2));
		return 0;
	}
	return rtl838x_write_phy(mii_id, 0, regnum, value);
}

static int rtl839x_mdio_write(struct mii_bus *bus, int mii_id,
			      int regnum, u16 value)
{
	return rtl839x_write_phy(mii_id, 0, regnum, value);
}

static int rtl838x_mdio_reset(struct mii_bus *bus)
{
	pr_info("%s called\n", __func__);
	/* Disable MAC polling the PHY so that we can start configuration */
	sw_w32(0x00000000, RTL838X_SMI_POLL_CTRL);

	/* Enable PHY control via SoC */
	sw_w32_mask(0, 1 << 15, RTL838X_SMI_GLB_CTRL);

	return 0;
}

static int rtl839x_mdio_reset(struct mii_bus *bus)
{
	pr_info("%s called\n", __func__);
	/* Disable MAC polling the PHY so that we can start configuration */
	sw_w32(0x00000000, RTL839X_SMI_PORT_POLLING_CTRL);
	sw_w32(0x00000000, RTL839X_SMI_PORT_POLLING_CTRL + 4);
	/* Disable PHY polling via SoC */
	sw_w32_mask(1 << 7, 0, RTL839X_SMI_GLB_CTRL);

	return 0;
}


static int rtl838x_mdio_init(struct rtl838x_eth_priv *priv)
{
	struct device_node *mii_np;
	int ret;

	pr_info("%s called\n", __func__);
	mii_np = of_get_child_by_name(priv->pdev->dev.of_node, "mdio-bus");

	if (!mii_np) {
		dev_err(&priv->pdev->dev, "no %s child node found", "mdio-bus");
		return -ENODEV;
	}

	if (!of_device_is_available(mii_np)) {
		ret = -ENODEV;
		goto err_put_node;
	}

	priv->mii_bus = devm_mdiobus_alloc(&priv->pdev->dev);
	if (!priv->mii_bus) {
		ret = -ENOMEM;
		goto err_put_node;
	}

	if (priv->family_id == RTL8380_FAMILY_ID) {
		priv->mii_bus->name = "rtl838x-eth-mdio";
		priv->mii_bus->read = rtl838x_mdio_read;
		priv->mii_bus->write = rtl838x_mdio_write;
		priv->mii_bus->reset = rtl838x_mdio_reset;
	} else {
		priv->mii_bus->name = "rtl839x-eth-mdio";
		priv->mii_bus->read = rtl839x_mdio_read;
		priv->mii_bus->write = rtl839x_mdio_write;
		priv->mii_bus->reset = rtl839x_mdio_reset;
	}
	priv->mii_bus->priv = priv;
	priv->mii_bus->parent = &priv->pdev->dev;

	snprintf(priv->mii_bus->id, MII_BUS_ID_SIZE, "%pOFn", mii_np);
	ret = of_mdiobus_register(priv->mii_bus, mii_np);

err_put_node:
	of_node_put(mii_np);
	return ret;
}

static int rtl838x_mdio_remove(struct rtl838x_eth_priv *priv)
{
	pr_info("%s called\n", __func__);
	if (!priv->mii_bus)
		return 0;

	mdiobus_unregister(priv->mii_bus);
	mdiobus_free(priv->mii_bus);

	return 0;
}

static const struct net_device_ops rtl838x_eth_netdev_ops = {
	.ndo_open = rtl838x_eth_open,
	.ndo_stop = rtl838x_eth_stop,
	.ndo_start_xmit = rtl838x_eth_tx,
	.ndo_set_mac_address = rtl838x_set_mac_address,
	.ndo_validate_addr = eth_validate_addr,
	.ndo_set_rx_mode = rtl838x_eth_set_multicast_list,
	.ndo_tx_timeout = rtl838x_eth_tx_timeout,
};

static const struct phylink_mac_ops rtl838x_phylink_ops = {
	.validate = rtl838x_validate,
	.mac_link_state = rtl838x_mac_pcs_get_state,
	.mac_an_restart = rtl838x_mac_an_restart,
	.mac_config = rtl838x_mac_config,
	.mac_link_down = rtl838x_mac_link_down,
	.mac_link_up = rtl838x_mac_link_up,
};

static const struct ethtool_ops rtl838x_ethtool_ops = {
	.get_link_ksettings     = rtl838x_get_link_ksettings,
	.set_link_ksettings     = rtl838x_set_link_ksettings,
};

static int __init rtl838x_eth_probe(struct platform_device *pdev)
{
	struct net_device *dev;
	struct device_node *dn = pdev->dev.of_node;
	struct rtl838x_eth_priv *priv;
	struct resource *res, *mem;
	const void *mac;
	phy_interface_t phy_mode;
	struct phylink *phylink;
	int err = 0;

	pr_info("Probing RTL838X eth device pdev: %x, dev: %x\n",
		(u32)pdev, (u32)(&(pdev->dev)));

	if (!dn) {
		dev_err(&pdev->dev, "No DT found\n");
		return -EINVAL;
	}

	dev = alloc_etherdev(sizeof(struct rtl838x_eth_priv));
	if (!dev) {
		err = -ENOMEM;
		goto err_free;
	}
	SET_NETDEV_DEV(dev, &pdev->dev);
	priv = netdev_priv(dev);

	/* obtain buffer memory space */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (res) {
		mem = devm_request_mem_region(&pdev->dev, res->start,
			resource_size(res), res->name);
		if (!mem) {
			dev_err(&pdev->dev, "cannot request memory space\n");
			err = -ENXIO;
			goto err_free;
		}

		dev->mem_start = mem->start;
		dev->mem_end   = mem->end;
	} else {
		dev_err(&pdev->dev, "cannot request IO resource\n");
		err = -ENXIO;
		goto err_free;
	}

	/* Allocate buffer memory */
	priv->membase = dmam_alloc_coherent(&pdev->dev,
				sizeof(struct ring_b), (void *)&dev->mem_start,
				GFP_KERNEL);
	if (!priv->membase) {
		dev_err(&pdev->dev, "cannot allocate DMA buffer\n");
		err = -ENOMEM;
		goto err_free;
	}

	spin_lock_init(&priv->lock);

	/* obtain device IRQ number */
	res = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	if (!res) {
		dev_err(&pdev->dev, "cannot obtain IRQ, using default 24\n");
		dev->irq = 24;
	} else {
		dev->irq = res->start;
	}
	dev->ethtool_ops = &rtl838x_ethtool_ops;

	priv->id = soc_info.id;
	priv->family_id = soc_info.family;
	if (priv->id) {
		pr_info("Found SoC ID: %4x: %s, family %x\n",
			priv->id, soc_info.name, priv->family_id);
	} else {
		pr_err("Unknown chip id (%04x)\n", priv->id);
		return -ENODEV;
	}

	if (priv->family_id == 0x8390) {
		priv->cpu_port = RTL839X_CPU_PORT;
		priv->r = &rtl839x_reg;
	} else {
		priv->cpu_port = RTL838X_CPU_PORT;
		priv->r = &rtl838x_reg;
	}

	rtl8380_init_mac(priv);

	/* try to get mac address in the following order:
	 * 1) from device tree data
	 * 2) from internal registers set by bootloader
	 */
	mac = of_get_mac_address(pdev->dev.of_node);
	if (!IS_ERR(mac)) {
		memcpy(dev->dev_addr, mac, ETH_ALEN);
		rtl838x_set_mac_hw(dev, (u8 *)mac);
	} else {
		dev->dev_addr[0] = (sw_r32(priv->r->mac) >> 8) & 0xff;
		dev->dev_addr[1] = sw_r32(priv->r->mac) & 0xff;
		dev->dev_addr[2] = (sw_r32(priv->r->mac + 4) >> 24) & 0xff;
		dev->dev_addr[3] = (sw_r32(priv->r->mac + 4) >> 16) & 0xff;
		dev->dev_addr[4] = (sw_r32(priv->r->mac + 4) >> 8) & 0xff;
		dev->dev_addr[5] = sw_r32(priv->r->mac + 4) & 0xff;
	}
	/* if the address is invalid, use a random value */
	if (!is_valid_ether_addr(dev->dev_addr)) {
		struct sockaddr sa = { AF_UNSPEC };

		netdev_warn(dev, "Invalid MAC address, using random\n");
		eth_hw_addr_random(dev);
		memcpy(sa.sa_data, dev->dev_addr, ETH_ALEN);
		if (rtl838x_set_mac_address(dev, &sa))
			netdev_warn(dev, "Failed to set MAC address.\n");
	}
	pr_info("Using MAC %08x%08x\n", sw_r32(priv->r->mac),
					sw_r32(priv->r->mac + 4));
	strcpy(dev->name, "eth%d");
	dev->netdev_ops = &rtl838x_eth_netdev_ops;
	priv->pdev = pdev;
	priv->netdev = dev;

	err = rtl838x_mdio_init(priv);
	if (err)
		goto err_free;

	err = register_netdev(dev);
	if (err)
		goto err_free;

	netif_napi_add(dev, &priv->napi, rtl838x_poll_rx, 64);
	platform_set_drvdata(pdev, dev);

	phy_mode = of_get_phy_mode(dn);
	if (phy_mode < 0) {
		dev_err(&pdev->dev, "incorrect phy-mode\n");
		err = -EINVAL;
		goto err_free;
	}
	priv->phylink_config.dev = &dev->dev;
	priv->phylink_config.type = PHYLINK_NETDEV;

	phylink = phylink_create(&priv->phylink_config, pdev->dev.fwnode,
				 phy_mode, &rtl838x_phylink_ops);
	if (IS_ERR(phylink)) {
		err = PTR_ERR(phylink);
		goto err_free;
	}
	priv->phylink = phylink;
	return 0;

err_free:
	pr_err("Error setting up netdev, freeing it again.\n");
	free_netdev(dev);
	return err;
}

static int rtl838x_eth_remove(struct platform_device *pdev)
{
	struct net_device *dev = platform_get_drvdata(pdev);
	struct rtl838x_eth_priv *priv = netdev_priv(dev);

	if (dev) {
		pr_info("Removing platform driver for rtl838x-eth\n");
		rtl838x_mdio_remove(priv);
		rtl838x_hw_stop(priv);
		netif_stop_queue(dev);
		netif_napi_del(&priv->napi);
		unregister_netdev(dev);
		free_netdev(dev);
	}
	return 0;
}

static const struct of_device_id rtl838x_eth_of_ids[] = {
	{ .compatible = "realtek,rtl838x-eth"},
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, rtl838x_eth_of_ids);

static struct platform_driver rtl838x_eth_driver = {
	.probe = rtl838x_eth_probe,
	.remove = rtl838x_eth_remove,
	.driver = {
		.name = "rtl838x-eth",
		.pm = NULL,
		.of_match_table = rtl838x_eth_of_ids,
	},
};

module_platform_driver(rtl838x_eth_driver);

MODULE_AUTHOR("B. Koblitz");
MODULE_DESCRIPTION("RTL838X SoC Ethernet Driver");
MODULE_LICENSE("GPL");

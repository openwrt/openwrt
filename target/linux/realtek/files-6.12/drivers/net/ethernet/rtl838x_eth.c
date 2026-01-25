// SPDX-License-Identifier: GPL-2.0-only
/* linux/drivers/net/ethernet/rtl838x_eth.c
 * Copyright (C) 2020 B. Koblitz
 */

#include <linux/cacheflush.h>
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
#include <linux/pkt_sched.h>
#include <net/dsa.h>
#include <net/switchdev.h>

#include <asm/mach-rtl838x/mach-rtl83xx.h>
#include "rtl838x_eth.h"

int rtl83xx_setup_tc(struct net_device *dev, enum tc_setup_type type, void *type_data);

/* Maximum number of RX rings is 8 on RTL83XX and 32 on the 93XX
 * The ring is assigned by switch based on packet/port priortity
 * Maximum number of TX rings is 2, Ring 2 being the high priority
 * ring on the RTL93xx SoCs. MAX_RXLEN gives the maximum length
 * for an RX ring, MAX_ENTRIES the maximum number of entries
 * available in total for all queues.
 */
#define MAX_RXRINGS	32
#define MAX_RXLEN	300
#define MAX_ENTRIES	(300 * 8)
#define TXRINGS		2
#define TXRINGLEN	160
#define NOTIFY_EVENTS	10
#define NOTIFY_BLOCKS	10
#define TX_EN		0x8
#define RX_EN		0x4
#define TX_EN_93XX	0x20
#define RX_EN_93XX	0x10
#define RX_TRUNCATE_EN_93XX BIT(6)
#define RX_TRUNCATE_EN_83XX BIT(4)
#define TX_PAD_EN_838X BIT(5)
#define TX_DO		0x2
#define WRAP		0x2
#define RING_BUFFER	1600

struct p_hdr {
	u8	*buf;
	u16	reserved;
	u16	size;		/* buffer size */
	u16	offset;
	u16	len;		/* pkt len */
	/* cpu_tag[0] is a reserved u16 on RTL83xx */
	u16	cpu_tag[10];
} __packed __aligned(1);

struct n_event {
	u32	type:2;
	u32	fidVid:12;
	u64	mac:48;
	u32	slp:6;
	u32	valid:1;
	u32	reserved:27;
} __packed __aligned(1);

struct ring_b {
	u32	rx_r[MAX_RXRINGS][MAX_RXLEN];
	u32	tx_r[TXRINGS][TXRINGLEN];
	struct	p_hdr	rx_header[MAX_RXRINGS][MAX_RXLEN];
	struct	p_hdr	tx_header[TXRINGS][TXRINGLEN];
	u32	c_rx[MAX_RXRINGS];
	u32	c_tx[TXRINGS];
	u8		tx_space[TXRINGS * TXRINGLEN * RING_BUFFER];
	u8		*rx_space;
};

struct notify_block {
	struct n_event	events[NOTIFY_EVENTS];
};

struct notify_b {
	struct notify_block	blocks[NOTIFY_BLOCKS];
	u32			reserved1[8];
	u32			ring[NOTIFY_BLOCKS];
	u32			reserved2[8];
};

static void rteth_838x_create_tx_header(struct p_hdr *h, unsigned int dest_port, int prio)
{
	/* cpu_tag[0] is reserved on the RTL83XX SoCs */
	h->cpu_tag[1] = 0x0400;  /* BIT 10: RTL8380_CPU_TAG */
	h->cpu_tag[2] = 0x0200;  /* Set only AS_DPM, to enable DPM settings below */
	h->cpu_tag[3] = 0x0000;
	h->cpu_tag[4] = BIT(dest_port) >> 16;
	h->cpu_tag[5] = BIT(dest_port) & 0xffff;

	/* Set internal priority (PRI) and enable (AS_PRI) */
	if (prio >= 0)
		h->cpu_tag[2] |= ((prio & 0x7) | BIT(3)) << 12;
}

static void rteth_839x_create_tx_header(struct p_hdr *h, unsigned int dest_port, int prio)
{
	/* cpu_tag[0] is reserved on the RTL83XX SoCs */
	h->cpu_tag[1] = 0x0100; /* RTL8390_CPU_TAG marker */
	h->cpu_tag[2] = BIT(4); /* AS_DPM flag */
	h->cpu_tag[3] = h->cpu_tag[4] = h->cpu_tag[5] = 0;
	/* h->cpu_tag[1] |= BIT(1) | BIT(0); */ /* Bypass filter 1/2 */
	if (dest_port >= 32) {
		dest_port -= 32;
		h->cpu_tag[2] |= (BIT(dest_port) >> 16) & 0xf;
		h->cpu_tag[3] = BIT(dest_port) & 0xffff;
	} else {
		h->cpu_tag[4] = BIT(dest_port) >> 16;
		h->cpu_tag[5] = BIT(dest_port) & 0xffff;
	}

	/* Set internal priority (PRI) and enable (AS_PRI) */
	if (prio >= 0)
		h->cpu_tag[2] |= ((prio & 0x7) | BIT(3)) << 8;
}

static void rteth_930x_create_tx_header(struct p_hdr *h, unsigned int dest_port, int prio)
{
	h->cpu_tag[0] = 0x8000;  /* CPU tag marker */

	h->cpu_tag[1] = FIELD_PREP(RTL93XX_CPU_TAG1_FWD_MASK,
				   RTL93XX_CPU_TAG1_FWD_PHYSICAL);
	h->cpu_tag[1] |= FIELD_PREP(RTL93XX_CPU_TAG1_IGNORE_STP_MASK, 1);
	h->cpu_tag[2] = 0;
	h->cpu_tag[3] = 0;
	h->cpu_tag[4] = 0;
	h->cpu_tag[5] = 0;
	h->cpu_tag[6] = BIT(dest_port) >> 16;
	h->cpu_tag[7] = BIT(dest_port) & 0xffff;

	/* Enable (AS_QID) and set priority queue (QID) */
	if (prio >= 0)
		h->cpu_tag[2] = (BIT(5) | (prio & 0x1f)) << 8;
}

static void rteth_931x_create_tx_header(struct p_hdr *h, unsigned int dest_port, int prio)
{
	h->cpu_tag[0] = 0x8000;  /* CPU tag marker */

	h->cpu_tag[1] = FIELD_PREP(RTL93XX_CPU_TAG1_FWD_MASK,
				   RTL93XX_CPU_TAG1_FWD_PHYSICAL);
	h->cpu_tag[1] |= FIELD_PREP(RTL93XX_CPU_TAG1_IGNORE_STP_MASK, 1);
	h->cpu_tag[2] = 0;
	h->cpu_tag[3] = 0;
	h->cpu_tag[4] = h->cpu_tag[5] = h->cpu_tag[6] = h->cpu_tag[7] = 0;
	if (dest_port >= 32) {
		dest_port -= 32;
		h->cpu_tag[4] = BIT(dest_port) >> 16;
		h->cpu_tag[5] = BIT(dest_port) & 0xffff;
	} else {
		h->cpu_tag[6] = BIT(dest_port) >> 16;
		h->cpu_tag[7] = BIT(dest_port) & 0xffff;
	}

	/* Enable (AS_QID) and set priority queue (QID) */
	if (prio >= 0)
		h->cpu_tag[2] = (BIT(5) | (prio & 0x1f)) << 8;
}

// Currently unused
// static void rtl93xx_header_vlan_set(struct p_hdr *h, int vlan)
// {
// 	h->cpu_tag[2] |= BIT(4); /* Enable VLAN forwarding offload */
// 	h->cpu_tag[2] |= (vlan >> 8) & 0xf;
// 	h->cpu_tag[3] |= (vlan & 0xff) << 8;
// }

struct rtl838x_rx_q {
	int id;
	struct rteth_ctrl *ctrl;
	struct napi_struct napi;
};

struct rteth_ctrl {
	struct net_device *netdev;
	struct platform_device *pdev;
	void *membase;
	spinlock_t lock;
	struct mii_bus *mii_bus;
	struct rtl838x_rx_q rx_qs[MAX_RXRINGS];
	struct phylink *phylink;
	struct phylink_config phylink_config;
	struct phylink_pcs pcs;
	const struct rteth_config *r;
	u32 lastEvent;
	u16 rxrings;
	u16 rxringlen;
};

/* On the RTL93XX, the RTL93XX_DMA_IF_RX_RING_CNTR track the fill level of
 * the rings. Writing x into these registers substracts x from its content.
 * When the content reaches the ring size, the ASIC no longer adds
 * packets to this receive queue.
 */
static void rteth_838x_update_counter(int r, int released)
{
	/* This feature is not available on RTL838x SoCs */
}

static void rteth_839x_update_counter(int r, int released)
{
	/* This feature is not available on RTL839x SoCs */
}

static void rteth_930x_update_counter(int r, int released)
{
	u32 reg = rtl930x_dma_if_rx_ring_cntr(r);
	int pos = (r % 3) * 10;

	sw_w32(released << pos, reg);
}

static void rteth_931x_update_counter(int r, int released)
{
	u32 reg = rtl931x_dma_if_rx_ring_cntr(r);
	int pos = (r % 3) * 10;

	sw_w32(released << pos, reg);
}

struct dsa_tag {
	u8	reason;
	u8	queue;
	u16	port;
	u8	l2_offloaded;
	u8	prio;
	bool	crc_error;
};

static bool rteth_838x_decode_tag(struct p_hdr *h, struct dsa_tag *t)
{
	/* cpu_tag[0] is reserved. Fields are off-by-one */
	t->reason = h->cpu_tag[4] & 0xf;
	t->queue = (h->cpu_tag[1] & 0xe0) >> 5;
	t->port = h->cpu_tag[1] & 0x1f;
	t->crc_error = t->reason == 13;

	pr_debug("Reason: %d\n", t->reason);
	if (t->reason != 6) /* NIC_RX_REASON_SPECIAL_TRAP */
		t->l2_offloaded = 1;
	else
		t->l2_offloaded = 0;

	return t->l2_offloaded;
}

static bool rteth_839x_decode_tag(struct p_hdr *h, struct dsa_tag *t)
{
	/* cpu_tag[0] is reserved. Fields are off-by-one */
	t->reason = h->cpu_tag[5] & 0x1f;
	t->queue = (h->cpu_tag[4] & 0xe000) >> 13;
	t->port = h->cpu_tag[1] & 0x3f;
	t->crc_error = h->cpu_tag[4] & BIT(6);

	pr_debug("Reason: %d\n", t->reason);
	if ((t->reason >= 7 && t->reason <= 13) || /* NIC_RX_REASON_RMA */
	    (t->reason >= 23 && t->reason <= 25))  /* NIC_RX_REASON_SPECIAL_TRAP */
		t->l2_offloaded = 0;
	else
		t->l2_offloaded = 1;

	return t->l2_offloaded;
}

static bool rteth_930x_decode_tag(struct p_hdr *h, struct dsa_tag *t)
{
	t->reason = h->cpu_tag[7] & 0x3f;
	t->queue =  (h->cpu_tag[2] >> 11) & 0x1f;
	t->port = (h->cpu_tag[0] >> 8) & 0x1f;
	t->crc_error = h->cpu_tag[1] & BIT(6);

	pr_debug("Reason %d, port %d, queue %d\n", t->reason, t->port, t->queue);
	if (t->reason >= 19 && t->reason <= 27)
		t->l2_offloaded = 0;
	else
		t->l2_offloaded = 1;

	return t->l2_offloaded;
}

static bool rteth_931x_decode_tag(struct p_hdr *h, struct dsa_tag *t)
{
	t->reason = h->cpu_tag[7] & 0x3f;
	t->queue =  (h->cpu_tag[2] >> 11) & 0x1f;
	t->port = (h->cpu_tag[0] >> 8) & 0x3f;
	t->crc_error = h->cpu_tag[1] & BIT(6);

	if (t->reason != 63)
		pr_info("%s: Reason %d, port %d, queue %d\n", __func__, t->reason, t->port, t->queue);
	if (t->reason >= 19 && t->reason <= 27)	/* NIC_RX_REASON_RMA */
		t->l2_offloaded = 0;
	else
		t->l2_offloaded = 1;

	return t->l2_offloaded;
}

struct fdb_update_work {
	struct work_struct work;
	struct net_device *ndev;
	u64 macs[NOTIFY_EVENTS + 1];
};

static void rtl838x_fdb_sync(struct work_struct *work)
{
	const struct fdb_update_work *uw = container_of(work, struct fdb_update_work, work);

	for (int i = 0; uw->macs[i]; i++) {
		struct switchdev_notifier_fdb_info info;
		u8 addr[ETH_ALEN];
		int action;

		action = (uw->macs[i] & (1ULL << 63)) ?
			 SWITCHDEV_FDB_ADD_TO_BRIDGE :
			 SWITCHDEV_FDB_DEL_TO_BRIDGE;
		u64_to_ether_addr(uw->macs[i] & 0xffffffffffffULL, addr);
		info.addr = &addr[0];
		info.vid = 0;
		info.offloaded = 1;
		pr_debug("FDB entry %d: %llx, action %d\n", i, uw->macs[0], action);
		call_switchdev_notifiers(action, uw->ndev, &info.info, NULL);
	}
	kfree(work);
}

static void rtl839x_l2_notification_handler(struct rteth_ctrl *ctrl)
{
	struct notify_b *nb = ctrl->membase + sizeof(struct ring_b);
	u32 e = ctrl->lastEvent;

	while (!(nb->ring[e] & 1)) {
		struct fdb_update_work *w;
		struct n_event *event;
		u64 mac;
		int i;

		w = kzalloc(sizeof(*w), GFP_ATOMIC);
		if (!w)
			return;

		INIT_WORK(&w->work, rtl838x_fdb_sync);

		for (i = 0; i < NOTIFY_EVENTS; i++) {
			event = &nb->blocks[e].events[i];
			if (!event->valid)
				continue;
			mac = event->mac;
			if (event->type)
				mac |= 1ULL << 63;
			w->ndev = ctrl->netdev;
			w->macs[i] = mac;
		}

		/* Hand the ring entry back to the switch */
		nb->ring[e] = nb->ring[e] | 1;
		e = (e + 1) % NOTIFY_BLOCKS;

		w->macs[i] = 0ULL;
		schedule_work(&w->work);
	}
	ctrl->lastEvent = e;
}

static irqreturn_t rteth_83xx_net_irq(int irq, void *dev_id)
{
	struct net_device *ndev = dev_id;
	struct rteth_ctrl *ctrl = netdev_priv(ndev);
	u32 status = sw_r32(ctrl->r->dma_if_intr_sts);
	unsigned long ring, rings;

	netdev_dbg(ndev, "rx interrupt received, status %08x\n", status);

	if (status & RTL83XX_DMA_IF_INTR_RX_RUN_OUT_MASK)
		if (net_ratelimit())
			netdev_warn(ndev, "rx ring overrun, status 0x%08x, mask 0x%08x\n",
				    status, sw_r32(ctrl->r->dma_if_intr_msk));

	rings = FIELD_GET(RTL83XX_DMA_IF_INTR_RX_DONE_MASK, status);
	for_each_set_bit(ring, &rings, ctrl->rxrings) {
		netdev_dbg(ndev, "schedule rx ring %lu\n", ring);
		sw_w32_mask(RTL83XX_DMA_IF_INTR_RX_MASK(ring), 0, ctrl->r->dma_if_intr_msk);
		napi_schedule(&ctrl->rx_qs[ring].napi);
	}

	if (status & RTL839X_DMA_IF_INTR_NOTIFY_MASK)
		rtl839x_l2_notification_handler(ctrl);

	sw_w32(status, ctrl->r->dma_if_intr_sts);

	return IRQ_HANDLED;
}

static irqreturn_t rteth_93xx_net_irq(int irq, void *dev_id)
{
	struct net_device *dev = dev_id;
	struct rteth_ctrl *ctrl = netdev_priv(dev);
	u32 status_rx_r = sw_r32(ctrl->r->dma_if_intr_rx_runout_sts);
	u32 status_rx = sw_r32(ctrl->r->dma_if_intr_rx_done_sts);
	u32 status_tx = sw_r32(ctrl->r->dma_if_intr_tx_done_sts);

	pr_debug("In %s, status_tx: %08x, status_rx: %08x, status_rx_r: %08x\n",
		 __func__, status_tx, status_rx, status_rx_r);

	/*  Ignore TX interrupt */
	if (status_tx) {
		/* Clear ISR */
		pr_debug("TX done\n");
		sw_w32(status_tx, ctrl->r->dma_if_intr_tx_done_sts);
	}

	/* RX interrupt */
	if (status_rx) {
		pr_debug("RX IRQ\n");
		/* ACK and disable RX interrupt for given rings */
		sw_w32(status_rx, ctrl->r->dma_if_intr_rx_done_sts);
		sw_w32_mask(status_rx, 0, ctrl->r->dma_if_intr_rx_done_msk);
		for (int i = 0; i < ctrl->rxrings; i++) {
			if (status_rx & BIT(i)) {
				pr_debug("Scheduling queue: %d\n", i);
				napi_schedule(&ctrl->rx_qs[i].napi);
			}
		}
	}

	/* RX buffer overrun */
	if (status_rx_r) {
		pr_debug("RX buffer overrun: status %x, mask: %x\n",
			 status_rx_r, sw_r32(ctrl->r->dma_if_intr_rx_runout_msk));
		sw_w32(status_rx_r, ctrl->r->dma_if_intr_rx_runout_sts);
	}

	return IRQ_HANDLED;
}

static void rteth_nic_reset(struct rteth_ctrl *ctrl, int reset_mask)
{
	pr_info("RESETTING CPU_PORT %d\n", ctrl->r->cpu_port);
	sw_w32_mask(0x3, 0, ctrl->r->mac_l2_port_ctrl);
	mdelay(100);

	/* Reset NIC (SW_NIC_RST) and queues (SW_Q_RST) */
	sw_w32_mask(0, reset_mask, ctrl->r->rst_glb_ctrl);
	while (sw_r32(ctrl->r->rst_glb_ctrl) & reset_mask)
		udelay(20);

	mdelay(100);
}

static void rteth_838x_hw_reset(struct rteth_ctrl *ctrl)
{
	/* Disable and clear interrupts */
	sw_w32(0x00000000, ctrl->r->dma_if_intr_msk);
	sw_w32(0xffffffff, ctrl->r->dma_if_intr_sts);

	rteth_nic_reset(ctrl, 0xc);

	/* Setup Head of Line */
	sw_w32(0, RTL838X_DMA_IF_RX_RING_SIZE);  /* Disabled on RTL8380 */
}

static void rteth_839x_hw_reset(struct rteth_ctrl *ctrl)
{
	u32 int_saved, nbuf;

	/* Disable and clear interrupts */
	sw_w32(0x00000000, ctrl->r->dma_if_intr_msk);
	sw_w32(0xffffffff, ctrl->r->dma_if_intr_sts);

	/* Preserve L2 notification and NBUF settings */
	int_saved = sw_r32(ctrl->r->dma_if_intr_msk);
	nbuf = sw_r32(RTL839X_DMA_IF_NBUF_BASE_DESC_ADDR_CTRL);

	/* Disable link change interrupt on RTL839x */
	sw_w32(0, RTL839X_IMR_PORT_LINK_STS_CHG);
	sw_w32(0, RTL839X_IMR_PORT_LINK_STS_CHG + 4);

	rteth_nic_reset(ctrl, 0xc);

	/* Setup Head of Line */
	sw_w32(0xffffffff, RTL839X_DMA_IF_RX_RING_CNTR);

	/* Re-enable link change interrupt */
	sw_w32(0xffffffff, RTL839X_ISR_PORT_LINK_STS_CHG);
	sw_w32(0xffffffff, RTL839X_ISR_PORT_LINK_STS_CHG + 4);
	sw_w32(0xffffffff, RTL839X_IMR_PORT_LINK_STS_CHG);
	sw_w32(0xffffffff, RTL839X_IMR_PORT_LINK_STS_CHG + 4);

	/* Restore notification settings: on RTL838x these bits are null */
	sw_w32_mask(7 << 20, int_saved & (7 << 20), ctrl->r->dma_if_intr_msk);
	sw_w32(nbuf, RTL839X_DMA_IF_NBUF_BASE_DESC_ADDR_CTRL);
}

static void rteth_93xx_hw_reset(struct rteth_ctrl *ctrl)
{
	/* Disable and clear interrupts */
	sw_w32(0x00000000, ctrl->r->dma_if_intr_rx_runout_msk);
	sw_w32(0xffffffff, ctrl->r->dma_if_intr_rx_runout_sts);
	sw_w32(0x00000000, ctrl->r->dma_if_intr_rx_done_msk);
	sw_w32(0xffffffff, ctrl->r->dma_if_intr_rx_done_sts);
	sw_w32(0x00000000, ctrl->r->dma_if_intr_tx_done_msk);
	sw_w32(0x0000000f, ctrl->r->dma_if_intr_tx_done_sts);

	rteth_nic_reset(ctrl, 0x6);

	/* Setup Head of Line */
	for (int i = 0; i < ctrl->rxrings; i++) {
		int pos = (i % 3) * 10;

		sw_w32_mask(0x3ff << pos, 0, ctrl->r->dma_if_rx_ring_size(i));
		sw_w32_mask(0x3ff << pos, ctrl->rxringlen, ctrl->r->dma_if_rx_ring_cntr(i));
	}
}

static void rteth_hw_ring_setup(struct rteth_ctrl *ctrl)
{
	struct ring_b *ring = ctrl->membase;

	for (int i = 0; i < ctrl->rxrings; i++)
		sw_w32(KSEG1ADDR(&ring->rx_r[i]), ctrl->r->dma_rx_base + i * 4);

	for (int i = 0; i < TXRINGS; i++)
		sw_w32(KSEG1ADDR(&ring->tx_r[i]), ctrl->r->dma_tx_base + i * 4);
}

static void rtl838x_hw_en_rxtx(struct rteth_ctrl *ctrl)
{
	/* Disable Head of Line features for all RX rings */
	sw_w32(0xffffffff, ctrl->r->dma_if_rx_ring_size(0));

	/* Truncate RX buffer to DEFAULT_MTU bytes, pad TX */
	sw_w32((DEFAULT_MTU << 16) | RX_TRUNCATE_EN_83XX | TX_PAD_EN_838X, ctrl->r->dma_if_ctrl);

	/* Enable RX done, RX overflow and TX done interrupts */
	sw_w32(0xfffff, ctrl->r->dma_if_intr_msk);

	/* Enable DMA, engine expects empty FCS field */
	sw_w32_mask(0, RX_EN | TX_EN, ctrl->r->dma_if_ctrl);

	/* Restart TX/RX to CPU port */
	sw_w32_mask(0x0, 0x3, ctrl->r->mac_l2_port_ctrl);
	/* Set Speed, duplex, flow control
	 * FORCE_EN | LINK_EN | NWAY_EN | DUP_SEL
	 * | SPD_SEL = 0b10 | FORCE_FC_EN | PHY_MASTER_SLV_MANUAL_EN
	 * | MEDIA_SEL
	 */
	sw_w32(0x6192F, ctrl->r->mac_force_mode_ctrl + ctrl->r->cpu_port * 4);

	/* Enable CRC checks on CPU-port */
	sw_w32_mask(0, BIT(3), ctrl->r->mac_l2_port_ctrl);
}

static void rtl839x_hw_en_rxtx(struct rteth_ctrl *ctrl)
{
	/* Setup CPU-Port: RX Buffer */
	sw_w32((DEFAULT_MTU << 5) | RX_TRUNCATE_EN_83XX, ctrl->r->dma_if_ctrl);

	/* Enable Notify, RX done, RX overflow and TX done interrupts */
	sw_w32(0x007fffff, ctrl->r->dma_if_intr_msk); /* Notify IRQ! */

	/* Enable DMA */
	sw_w32_mask(0, RX_EN | TX_EN, ctrl->r->dma_if_ctrl);

	/* Restart TX/RX to CPU port, enable CRC checking */
	sw_w32_mask(0x0, 0x3 | BIT(3), ctrl->r->mac_l2_port_ctrl);

	/* CPU port joins Lookup Miss Flooding Portmask */
	/* TODO: The code below should also work for the RTL838x */
	sw_w32(0x28000, RTL839X_TBL_ACCESS_L2_CTRL);
	sw_w32_mask(0, 0x80000000, RTL839X_TBL_ACCESS_L2_DATA(0));
	sw_w32(0x38000, RTL839X_TBL_ACCESS_L2_CTRL);

	/* Force CPU port link up */
	sw_w32_mask(0, 3, ctrl->r->mac_force_mode_ctrl + ctrl->r->cpu_port * 4);
}

static void rtl93xx_hw_en_rxtx(struct rteth_ctrl *ctrl)
{
	/* Setup CPU-Port: RX Buffer truncated at DEFAULT_MTU Bytes */
	sw_w32((DEFAULT_MTU << 16) | RX_TRUNCATE_EN_93XX, ctrl->r->dma_if_ctrl);

	for (int i = 0; i < ctrl->rxrings; i++) {
		int cnt = min(ctrl->rxringlen - 2, 0x3ff);
		int pos = (i % 3) * 10;
		u32 v;

		sw_w32_mask(0x3ff << pos, cnt << pos, ctrl->r->dma_if_rx_ring_size(i));

		/* Some SoCs have issues with missing underflow protection */
		v = (sw_r32(ctrl->r->dma_if_rx_ring_cntr(i)) >> pos) & 0x3ff;
		sw_w32_mask(0x3ff << pos, v, ctrl->r->dma_if_rx_ring_cntr(i));
	}

	/* Enable Notify, RX done, RX overflow and TX done interrupts */
	sw_w32(0xffffffff, ctrl->r->dma_if_intr_rx_runout_msk);
	sw_w32(0xffffffff, ctrl->r->dma_if_intr_rx_done_msk);
	sw_w32(0x0000000f, ctrl->r->dma_if_intr_tx_done_msk);

	/* Enable DMA */
	sw_w32_mask(0, RX_EN_93XX | TX_EN_93XX, ctrl->r->dma_if_ctrl);

	/* Restart TX/RX to CPU port, enable CRC checking */
	sw_w32_mask(0x0, 0x3 | BIT(4), ctrl->r->mac_l2_port_ctrl);

	if (ctrl->r->family_id == RTL9300_FAMILY_ID)
		sw_w32_mask(0, BIT(ctrl->r->cpu_port), RTL930X_L2_UNKN_UC_FLD_PMSK);
	else
		sw_w32_mask(0, BIT(ctrl->r->cpu_port), RTL931X_L2_UNKN_UC_FLD_PMSK);

	if (ctrl->r->family_id == RTL9300_FAMILY_ID)
		sw_w32(0x217, ctrl->r->mac_force_mode_ctrl + ctrl->r->cpu_port * 4);
	else
		sw_w32(0x2a1d, ctrl->r->mac_force_mode_ctrl + ctrl->r->cpu_port * 4);
}

static void rteth_setup_ring_buffer(struct rteth_ctrl *ctrl, struct ring_b *ring)
{
	for (int i = 0; i < ctrl->rxrings; i++) {
		struct p_hdr *h;
		int j;

		for (j = 0; j < ctrl->rxringlen; j++) {
			h = &ring->rx_header[i][j];
			memset(h, 0, sizeof(struct p_hdr));
			h->buf = (u8 *)KSEG1ADDR(ring->rx_space +
						 i * ctrl->rxringlen * RING_BUFFER +
						 j * RING_BUFFER);
			h->size = RING_BUFFER;
			/* All rings owned by switch, last one wraps */
			ring->rx_r[i][j] = KSEG1ADDR(h) | 1 | (j == (ctrl->rxringlen - 1) ?
					   WRAP :
					   0);
		}
		ring->c_rx[i] = 0;
	}

	for (int i = 0; i < TXRINGS; i++) {
		struct p_hdr *h;
		int j;

		for (j = 0; j < TXRINGLEN; j++) {
			h = &ring->tx_header[i][j];
			memset(h, 0, sizeof(struct p_hdr));
			h->buf = (u8 *)KSEG1ADDR(ring->tx_space +
						 i * TXRINGLEN * RING_BUFFER +
						 j * RING_BUFFER);
			h->size = RING_BUFFER;
			ring->tx_r[i][j] = KSEG1ADDR(&ring->tx_header[i][j]);
		}
		/* Last header is wrapping around */
		ring->tx_r[i][j - 1] |= WRAP;
		ring->c_tx[i] = 0;
	}
}

static void rtl839x_setup_notify_ring_buffer(struct rteth_ctrl *ctrl)
{
	struct notify_b *b = ctrl->membase + sizeof(struct ring_b);

	for (int i = 0; i < NOTIFY_BLOCKS; i++)
		b->ring[i] = KSEG1ADDR(&b->blocks[i]) | 1 | (i == (NOTIFY_BLOCKS - 1) ? WRAP : 0);

	sw_w32((u32)b->ring, RTL839X_DMA_IF_NBUF_BASE_DESC_ADDR_CTRL);
	sw_w32_mask(0x3ff << 2, 100 << 2, RTL839X_L2_NOTIFICATION_CTRL);

	/* Setup notification events */
	sw_w32_mask(0, 1 << 14, RTL839X_L2_CTRL_0); /* RTL8390_L2_CTRL_0_FLUSH_NOTIFY_EN */
	sw_w32_mask(0, 1 << 12, RTL839X_L2_NOTIFICATION_CTRL); /* SUSPEND_NOTIFICATION_EN */

	/* Enable Notification */
	sw_w32_mask(0, 1 << 0, RTL839X_L2_NOTIFICATION_CTRL);
	ctrl->lastEvent = 0;
}

static int rteth_open(struct net_device *ndev)
{
	unsigned long flags;
	struct rteth_ctrl *ctrl = netdev_priv(ndev);
	struct ring_b *ring = ctrl->membase;

	pr_debug("%s called: RX rings %d(length %d), TX rings %d(length %d)\n",
		 __func__, ctrl->rxrings, ctrl->rxringlen, TXRINGS, TXRINGLEN);

	spin_lock_irqsave(&ctrl->lock, flags);
	ctrl->r->hw_reset(ctrl);
	rteth_setup_ring_buffer(ctrl, ring);
	if (ctrl->r->family_id == RTL8390_FAMILY_ID) {
		rtl839x_setup_notify_ring_buffer(ctrl);
		/* Make sure the ring structure is visible to the ASIC */
		mb();
		flush_cache_all();
	}

	rteth_hw_ring_setup(ctrl);
	phylink_start(ctrl->phylink);

	for (int i = 0; i < ctrl->rxrings; i++)
		napi_enable(&ctrl->rx_qs[i].napi);

	switch (ctrl->r->family_id) {
	case RTL8380_FAMILY_ID:
		rtl838x_hw_en_rxtx(ctrl);
		/* Trap IGMP/MLD traffic to CPU-Port */
		sw_w32(0x3, RTL838X_SPCL_TRAP_IGMP_CTRL);
		/* Flush learned FDB entries on link down of a port */
		sw_w32_mask(0, BIT(7), RTL838X_L2_CTRL_0);
		break;

	case RTL8390_FAMILY_ID:
		rtl839x_hw_en_rxtx(ctrl);
		/* Trap MLD and IGMP messages to CPU_PORT */
		sw_w32(0x3, RTL839X_SPCL_TRAP_IGMP_CTRL);
		/* Flush learned FDB entries on link down of a port */
		sw_w32_mask(0, BIT(7), RTL839X_L2_CTRL_0);
		break;

	case RTL9300_FAMILY_ID:
		rtl93xx_hw_en_rxtx(ctrl);
		/* Flush learned FDB entries on link down of a port */
		sw_w32_mask(0, BIT(7), RTL930X_L2_CTRL);
		/* Trap MLD and IGMP messages to CPU_PORT */
		sw_w32((0x2 << 3) | 0x2,  RTL930X_VLAN_APP_PKT_CTRL);
		break;

	case RTL9310_FAMILY_ID:
		rtl93xx_hw_en_rxtx(ctrl);

		/* Trap MLD and IGMP messages to CPU_PORT */
		sw_w32((0x2 << 3) | 0x2,  RTL931X_VLAN_APP_PKT_CTRL);

		/* Set PCIE_PWR_DOWN */
		sw_w32_mask(0, BIT(1), RTL931X_PS_SOC_CTRL);
		break;
	}

	netif_tx_start_all_queues(ndev);

	spin_unlock_irqrestore(&ctrl->lock, flags);

	return 0;
}

static void rtl838x_hw_stop(struct rteth_ctrl *ctrl)
{
	u32 force_mac = ctrl->r->family_id == RTL8380_FAMILY_ID ? 0x6192C : 0x75;
	u32 clear_irq = ctrl->r->family_id == RTL8380_FAMILY_ID ? 0x000fffff : 0x007fffff;

	/* Disable RX/TX from/to CPU-port */
	sw_w32_mask(0x3, 0, ctrl->r->mac_l2_port_ctrl);

	/* Disable traffic */
	if (ctrl->r->family_id == RTL9300_FAMILY_ID || ctrl->r->family_id == RTL9310_FAMILY_ID)
		sw_w32_mask(RX_EN_93XX | TX_EN_93XX, 0, ctrl->r->dma_if_ctrl);
	else
		sw_w32_mask(RX_EN | TX_EN, 0, ctrl->r->dma_if_ctrl);
	mdelay(200); /* Test, whether this is needed */

	/* Block all ports */
	if (ctrl->r->family_id == RTL8380_FAMILY_ID) {
		sw_w32(0x03000000, RTL838X_TBL_ACCESS_DATA_0(0));
		sw_w32(0x00000000, RTL838X_TBL_ACCESS_DATA_0(1));
		sw_w32(1 << 15 | 2 << 12, RTL838X_TBL_ACCESS_CTRL_0);
	}

	/* Flush L2 address cache */
	if (ctrl->r->family_id == RTL8380_FAMILY_ID) {
		/* Disable FAST_AGE_OUT otherwise flush will hang */
		sw_w32_mask(BIT(23), 0, RTL838X_L2_CTRL_1);
		for (int i = 0; i <= ctrl->r->cpu_port; i++) {
			sw_w32(BIT(26) | BIT(23) | i << 5, ctrl->r->l2_tbl_flush_ctrl);
			do { } while (sw_r32(ctrl->r->l2_tbl_flush_ctrl) & BIT(26));
		}
	} else if (ctrl->r->family_id == RTL8390_FAMILY_ID) {
		for (int i = 0; i <= ctrl->r->cpu_port; i++) {
			sw_w32(BIT(28) | BIT(25) | i << 5, ctrl->r->l2_tbl_flush_ctrl);
			do { } while (sw_r32(ctrl->r->l2_tbl_flush_ctrl) & BIT(28));
		}
	}
	/* TODO: L2 flush register is 64 bit on RTL931X and 930X */

	/* CPU-Port: Link down */
	if (ctrl->r->family_id == RTL8380_FAMILY_ID || ctrl->r->family_id == RTL8390_FAMILY_ID)
		sw_w32(force_mac, ctrl->r->mac_force_mode_ctrl + ctrl->r->cpu_port * 4);
	else if (ctrl->r->family_id == RTL9300_FAMILY_ID)
		sw_w32_mask(0x3, 0, ctrl->r->mac_force_mode_ctrl + ctrl->r->cpu_port * 4);
	else if (ctrl->r->family_id == RTL9310_FAMILY_ID)
		sw_w32_mask(BIT(0) | BIT(9), 0, ctrl->r->mac_force_mode_ctrl + ctrl->r->cpu_port * 4);
	mdelay(100);

	/* Disable all TX/RX interrupts */
	if (ctrl->r->family_id == RTL9300_FAMILY_ID || ctrl->r->family_id == RTL9310_FAMILY_ID) {
		sw_w32(0x00000000, ctrl->r->dma_if_intr_rx_runout_msk);
		sw_w32(0xffffffff, ctrl->r->dma_if_intr_rx_runout_sts);
		sw_w32(0x00000000, ctrl->r->dma_if_intr_rx_done_msk);
		sw_w32(0xffffffff, ctrl->r->dma_if_intr_rx_done_sts);
		sw_w32(0x00000000, ctrl->r->dma_if_intr_tx_done_msk);
		sw_w32(0x0000000f, ctrl->r->dma_if_intr_tx_done_sts);
	} else {
		sw_w32(0x00000000, ctrl->r->dma_if_intr_msk);
		sw_w32(clear_irq, ctrl->r->dma_if_intr_sts);
	}

	/* Disable TX/RX DMA */
	sw_w32(0x00000000, ctrl->r->dma_if_ctrl);
	mdelay(200);
}

static int rteth_stop(struct net_device *ndev)
{
	struct rteth_ctrl *ctrl = netdev_priv(ndev);

	pr_info("in %s\n", __func__);

	phylink_stop(ctrl->phylink);
	rtl838x_hw_stop(ctrl);

	for (int i = 0; i < ctrl->rxrings; i++)
		napi_disable(&ctrl->rx_qs[i].napi);

	netif_tx_stop_all_queues(ndev);

	return 0;
}

static void rteth_838x_set_rx_mode(struct net_device *ndev)
{
	/* Flood all classes of RMA addresses (01-80-C2-00-00-{01..2F})
	 * CTRL_0_FULL = GENMASK(21, 0) = 0x3FFFFF
	 */
	if (!(ndev->flags & (IFF_PROMISC | IFF_ALLMULTI))) {
		sw_w32(0x0, RTL838X_RMA_CTRL_0);
		sw_w32(0x0, RTL838X_RMA_CTRL_1);
	}
	if (ndev->flags & IFF_ALLMULTI)
		sw_w32(GENMASK(21, 0), RTL838X_RMA_CTRL_0);
	if (ndev->flags & IFF_PROMISC) {
		sw_w32(GENMASK(21, 0), RTL838X_RMA_CTRL_0);
		sw_w32(0x7fff, RTL838X_RMA_CTRL_1);
	}
}

static void rteth_839x_set_rx_mode(struct net_device *ndev)
{
	/* Flood all classes of RMA addresses (01-80-C2-00-00-{01..2F})
	 * CTRL_0_FULL = GENMASK(31, 2) = 0xFFFFFFFC
	 * Lower two bits are reserved, corresponding to RMA 01-80-C2-00-00-00
	 * CTRL_1_FULL = CTRL_2_FULL = GENMASK(31, 0)
	 */
	if (!(ndev->flags & (IFF_PROMISC | IFF_ALLMULTI))) {
		sw_w32(0x0, RTL839X_RMA_CTRL_0);
		sw_w32(0x0, RTL839X_RMA_CTRL_1);
		sw_w32(0x0, RTL839X_RMA_CTRL_2);
		sw_w32(0x0, RTL839X_RMA_CTRL_3);
	}
	if (ndev->flags & IFF_ALLMULTI) {
		sw_w32(GENMASK(31, 2), RTL839X_RMA_CTRL_0);
		sw_w32(GENMASK(31, 0), RTL839X_RMA_CTRL_1);
		sw_w32(GENMASK(31, 0), RTL839X_RMA_CTRL_2);
	}
	if (ndev->flags & IFF_PROMISC) {
		sw_w32(GENMASK(31, 2), RTL839X_RMA_CTRL_0);
		sw_w32(GENMASK(31, 0), RTL839X_RMA_CTRL_1);
		sw_w32(GENMASK(31, 0), RTL839X_RMA_CTRL_2);
		sw_w32(0x3ff, RTL839X_RMA_CTRL_3);
	}
}

static void rteth_930x_set_rx_mode(struct net_device *ndev)
{
	/* Flood all classes of RMA addresses (01-80-C2-00-00-{01..2F})
	 * CTRL_0_FULL = GENMASK(31, 2) = 0xFFFFFFFC
	 * Lower two bits are reserved, corresponding to RMA 01-80-C2-00-00-00
	 * CTRL_1_FULL = CTRL_2_FULL = GENMASK(31, 0)
	 */
	if (ndev->flags & (IFF_ALLMULTI | IFF_PROMISC)) {
		sw_w32(GENMASK(31, 2), RTL930X_RMA_CTRL_0);
		sw_w32(GENMASK(31, 0), RTL930X_RMA_CTRL_1);
		sw_w32(GENMASK(31, 0), RTL930X_RMA_CTRL_2);
	} else {
		sw_w32(0x0, RTL930X_RMA_CTRL_0);
		sw_w32(0x0, RTL930X_RMA_CTRL_1);
		sw_w32(0x0, RTL930X_RMA_CTRL_2);
	}
}

static void rtl931x_eth_set_multicast_list(struct net_device *ndev)
{
	/* Flood all classes of RMA addresses (01-80-C2-00-00-{01..2F})
	 * CTRL_0_FULL = GENMASK(31, 2) = 0xFFFFFFFC
	 * Lower two bits are reserved, corresponding to RMA 01-80-C2-00-00-00.
	 * CTRL_1_FULL = CTRL_2_FULL = GENMASK(31, 0)
	 */
	if (ndev->flags & (IFF_ALLMULTI | IFF_PROMISC)) {
		sw_w32(GENMASK(31, 2), RTL931X_RMA_CTRL_0);
		sw_w32(GENMASK(31, 0), RTL931X_RMA_CTRL_1);
		sw_w32(GENMASK(31, 0), RTL931X_RMA_CTRL_2);
	} else {
		sw_w32(0x0, RTL931X_RMA_CTRL_0);
		sw_w32(0x0, RTL931X_RMA_CTRL_1);
		sw_w32(0x0, RTL931X_RMA_CTRL_2);
	}
}

static void rteth_tx_timeout(struct net_device *ndev, unsigned int txqueue)
{
	unsigned long flags;
	struct rteth_ctrl *ctrl = netdev_priv(ndev);

	pr_warn("%s\n", __func__);
	spin_lock_irqsave(&ctrl->lock, flags);
	rtl838x_hw_stop(ctrl);
	rteth_hw_ring_setup(ctrl);
	rtl838x_hw_en_rxtx(ctrl);
	netif_trans_update(ndev);
	netif_start_queue(ndev);
	spin_unlock_irqrestore(&ctrl->lock, flags);
}

static int rteth_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
	int len;
	struct rteth_ctrl *ctrl = netdev_priv(dev);
	struct ring_b *ring = ctrl->membase;
	int ret;
	unsigned long flags;
	struct p_hdr *h;
	int dest_port = -1;
	int q = skb_get_queue_mapping(skb) % TXRINGS;

	if (q) /* Check for high prio queue */
		pr_debug("SKB priority: %d\n", skb->priority);

	spin_lock_irqsave(&ctrl->lock, flags);
	len = skb->len;

	/* Check for DSA tagging at the end of the buffer */
	if (netdev_uses_dsa(dev) &&
	    skb->data[len - 4] == 0x80 &&
	    skb->data[len - 3] < ctrl->r->cpu_port &&
	    skb->data[len - 2] == 0x10 &&
	    skb->data[len - 1] == 0x00) {
		/* Reuse tag space for CRC if possible */
		dest_port = skb->data[len - 3];
		skb->data[len - 4] = skb->data[len - 3] = skb->data[len - 2] = skb->data[len - 1] = 0x00;
		len -= 4;
	}

	len += 4; /* Add space for CRC */

	if (skb_padto(skb, len)) {
		ret = NETDEV_TX_OK;
		goto txdone;
	}

	/* We can send this packet if CPU owns the descriptor */
	if (!(ring->tx_r[q][ring->c_tx[q]] & 0x1)) {
		/* Set descriptor for tx */
		h = &ring->tx_header[q][ring->c_tx[q]];
		h->size = len;
		h->len = len;
		/* On RTL8380 SoCs, small packet lengths being sent need adjustments */
		if (ctrl->r->family_id == RTL8380_FAMILY_ID) {
			if (len < ETH_ZLEN - 4)
				h->len -= 4;
		}

		if (dest_port >= 0)
			ctrl->r->create_tx_header(h, dest_port, skb->priority >> 1);

		/* Copy packet data to tx buffer */
		memcpy((void *)KSEG1ADDR(h->buf), skb->data, len);
		/* Make sure packet data is visible to ASIC */
		wmb();

		/* Hand over to switch */
		ring->tx_r[q][ring->c_tx[q]] |= 1;

		/* Before starting TX, prevent a Lextra bus bug on RTL8380 SoCs */
		if (ctrl->r->family_id == RTL8380_FAMILY_ID) {
			for (int i = 0; i < 10; i++) {
				u32 val = sw_r32(ctrl->r->dma_if_ctrl);

				if ((val & 0xc) == 0xc)
					break;
			}
		}

		/* Tell switch to send data */
		if (ctrl->r->family_id == RTL9310_FAMILY_ID || ctrl->r->family_id == RTL9300_FAMILY_ID) {
			/* Ring ID q == 0: Low priority, Ring ID = 1: High prio queue */
			if (!q)
				sw_w32_mask(0, BIT(2), ctrl->r->dma_if_ctrl);
			else
				sw_w32_mask(0, BIT(3), ctrl->r->dma_if_ctrl);
		} else {
			sw_w32_mask(0, TX_DO, ctrl->r->dma_if_ctrl);
		}

		dev->stats.tx_packets++;
		dev->stats.tx_bytes += len;
		dev_kfree_skb(skb);
		ring->c_tx[q] = (ring->c_tx[q] + 1) % TXRINGLEN;
		ret = NETDEV_TX_OK;
	} else {
		dev_warn(&ctrl->pdev->dev, "Data is owned by switch\n");
		ret = NETDEV_TX_BUSY;
	}

txdone:
	spin_unlock_irqrestore(&ctrl->lock, flags);

	return ret;
}

/* Return queue number for TX. On the RTL83XX, these queues have equal priority
 * so we do round-robin
 */
static u16 rteth_83xx_pick_tx_queue(struct net_device *dev, struct sk_buff *skb,
				 struct net_device *sb_dev)
{
	static u8 last;

	last++;
	return last % TXRINGS;
}

/* Return queue number for TX. On the RTL93XX, queue 1 is the high priority queue
 */
static u16 rteth_93xx_pick_tx_queue(struct net_device *dev, struct sk_buff *skb,
				 struct net_device *sb_dev)
{
	if (skb->priority >= TC_PRIO_CONTROL)
		return 1;

	return 0;
}

static int rtl838x_hw_receive(struct net_device *dev, int r, int budget)
{
	struct rteth_ctrl *ctrl = netdev_priv(dev);
	struct ring_b *ring = ctrl->membase;
	unsigned long flags;
	int work_done = 0;
	u32	*last;
	bool dsa = netdev_uses_dsa(dev);

	pr_debug("---------------------------------------------------------- RX - %d\n", r);
	spin_lock_irqsave(&ctrl->lock, flags);
	last = (u32 *)KSEG1ADDR(sw_r32(ctrl->r->dma_if_rx_cur + r * 4));

	do {
		struct sk_buff *skb;
		struct dsa_tag tag;
		struct p_hdr *h;
		u8 *data;
		int len;

		if ((ring->rx_r[r][ring->c_rx[r]] & 0x1)) {
			if (&ring->rx_r[r][ring->c_rx[r]] != last) {
				netdev_warn(dev, "Ring contention: r: %x, last %x, cur %x\n",
					    r, (u32)last, (u32)&ring->rx_r[r][ring->c_rx[r]]);
			}
			break;
		}

		h = &ring->rx_header[r][ring->c_rx[r]];
		data = (u8 *)KSEG1ADDR(h->buf);
		len = h->len;
		if (!len)
			break;
		work_done++;

		len -= 4; /* strip the CRC */
		/* Add 4 bytes for cpu_tag */
		if (dsa)
			len += 4;

		skb = netdev_alloc_skb_ip_align(dev, len);
		if (likely(skb)) {
			/* BUG: Prevent bug on RTL838x SoCs */
			if (ctrl->r->family_id == RTL8380_FAMILY_ID) {
				sw_w32(0xffffffff, ctrl->r->dma_if_rx_ring_size(0));
				for (int i = 0; i < ctrl->rxrings; i++) {
					unsigned int val;

					/* Update each ring cnt */
					val = sw_r32(ctrl->r->dma_if_rx_ring_cntr(i));
					sw_w32(val, ctrl->r->dma_if_rx_ring_cntr(i));
				}
			}

			/* Make sure data is visible */
			mb();
			skb_put_data(skb, (u8 *)KSEG1ADDR(data), len);
			/* Overwrite CRC with cpu_tag */
			if (dsa) {
				ctrl->r->decode_tag(h, &tag);
				skb->data[len - 4] = 0x80;
				skb->data[len - 3] = tag.port;
				skb->data[len - 2] = 0x10;
				skb->data[len - 1] = 0x00;
				if (tag.l2_offloaded)
					skb->data[len - 3] |= 0x40;
			}

			if (tag.queue >= 0)
				pr_debug("Queue: %d, len: %d, reason %d port %d\n",
					 tag.queue, len, tag.reason, tag.port);

			skb->protocol = eth_type_trans(skb, dev);
			if (dev->features & NETIF_F_RXCSUM) {
				if (tag.crc_error)
					skb_checksum_none_assert(skb);
				else
					skb->ip_summed = CHECKSUM_UNNECESSARY;
			}
			dev->stats.rx_packets++;
			dev->stats.rx_bytes += len;

			napi_gro_receive(&ctrl->rx_qs[r].napi, skb);
		} else {
			if (net_ratelimit())
				dev_warn(&dev->dev, "low on memory - packet dropped\n");
			dev->stats.rx_dropped++;
		}

		/* Reset header structure */
		memset(h, 0, sizeof(struct p_hdr));
		h->buf = data;
		h->size = RING_BUFFER;

		ring->rx_r[r][ring->c_rx[r]] = KSEG1ADDR(h) | 0x1 | (ring->c_rx[r] == (ctrl->rxringlen - 1) ?
					       WRAP :
					       0x1);
		ring->c_rx[r] = (ring->c_rx[r] + 1) % ctrl->rxringlen;
		last = (u32 *)KSEG1ADDR(sw_r32(ctrl->r->dma_if_rx_cur + r * 4));
	} while (&ring->rx_r[r][ring->c_rx[r]] != last && work_done < budget);

	/* Update counters */
	ctrl->r->update_counter(r, work_done);

	spin_unlock_irqrestore(&ctrl->lock, flags);

	return work_done;
}

static int rtl838x_poll_rx(struct napi_struct *napi, int budget)
{
	struct rtl838x_rx_q *rx_q = container_of(napi, struct rtl838x_rx_q, napi);
	struct rteth_ctrl *ctrl = rx_q->ctrl;
	unsigned long flags;
	int ring = rx_q->id;
	int work_done = 0;

	while (work_done < budget) {
		int work = rtl838x_hw_receive(ctrl->netdev, ring, budget - work_done);

		if (!work)
			break;
		work_done += work;
	}

	if (work_done < budget && napi_complete_done(napi, work_done)) {
		/* Re-enable rx interrupts */
		spin_lock_irqsave(&ctrl->lock, flags);
		if (ctrl->r->family_id == RTL9300_FAMILY_ID || ctrl->r->family_id == RTL9310_FAMILY_ID)
			sw_w32_mask(0, RTL93XX_DMA_IF_INTR_RX_MASK(ring), ctrl->r->dma_if_intr_rx_done_msk);
		else
			sw_w32_mask(0, RTL83XX_DMA_IF_INTR_RX_MASK(ring), ctrl->r->dma_if_intr_msk);
		spin_unlock_irqrestore(&ctrl->lock, flags);
	}

	return work_done;
}

static void rteth_mac_config(struct phylink_config *config,
			     unsigned int mode,
			     const struct phylink_link_state *state)
{
	/* This is only being called for the master device,
	 * i.e. the CPU-Port. We don't need to do anything.
	 */

	pr_info("In %s, mode %x\n", __func__, mode);
}

static void rteth_pcs_an_restart(struct phylink_pcs *pcs)
{
	struct rteth_ctrl *ctrl = container_of(pcs, struct rteth_ctrl, pcs);

	/* This works only on RTL838x chips */
	if (ctrl->r->family_id != RTL8380_FAMILY_ID)
		return;

	pr_debug("In %s\n", __func__);
	/* Restart by disabling and re-enabling link */
	sw_w32(0x6192D, ctrl->r->mac_force_mode_ctrl + ctrl->r->cpu_port * 4);
	mdelay(20);
	sw_w32(0x6192F, ctrl->r->mac_force_mode_ctrl + ctrl->r->cpu_port * 4);
}

static void rteth_pcs_get_state(struct phylink_pcs *pcs,
				struct phylink_link_state *state)
{
	u32 speed;
	struct rteth_ctrl *ctrl = container_of(pcs, struct rteth_ctrl, pcs);
	int port = ctrl->r->cpu_port;

	pr_info("In %s\n", __func__);

	state->link = ctrl->r->get_mac_link_sts(port) ? 1 : 0;
	state->duplex = ctrl->r->get_mac_link_dup_sts(port) ? 1 : 0;

	pr_info("%s link status is %d\n", __func__, state->link);
	speed = ctrl->r->get_mac_link_spd_sts(port);
	switch (speed) {
	case 0:
		state->speed = SPEED_10;
		break;
	case 1:
		state->speed = SPEED_100;
		break;
	case 2:
		state->speed = SPEED_1000;
		break;
	case 5:
		state->speed = SPEED_2500;
		break;
	case 6:
		state->speed = SPEED_5000;
		break;
	case 4:
		state->speed = SPEED_10000;
		break;
	default:
		state->speed = SPEED_UNKNOWN;
		break;
	}

	state->pause &= (MLO_PAUSE_RX | MLO_PAUSE_TX);
	if (ctrl->r->get_mac_rx_pause_sts(port))
		state->pause |= MLO_PAUSE_RX;
	if (ctrl->r->get_mac_tx_pause_sts(port))
		state->pause |= MLO_PAUSE_TX;
}

static int rteth_pcs_config(struct phylink_pcs *pcs, unsigned int neg_mode,
			    phy_interface_t interface,
			    const unsigned long *advertising,
			    bool permit_pause_to_mac)
{
	return 0;
}

static void rteth_mac_link_down(struct phylink_config *config,
				unsigned int mode,
				phy_interface_t interface)
{
	struct net_device *dev = container_of(config->dev, struct net_device, dev);
	struct rteth_ctrl *ctrl = netdev_priv(dev);

	pr_debug("In %s\n", __func__);
	/* Stop TX/RX to port */
	sw_w32_mask(0x03, 0, ctrl->r->mac_l2_port_ctrl);
}

static void rteth_mac_link_up(struct phylink_config *config,
			      struct phy_device *phy, unsigned int mode,
			      phy_interface_t interface, int speed, int duplex,
			      bool tx_pause, bool rx_pause)
{
	struct net_device *dev = container_of(config->dev, struct net_device, dev);
	struct rteth_ctrl *ctrl = netdev_priv(dev);

	pr_debug("In %s\n", __func__);
	/* Restart TX/RX to port */
	sw_w32_mask(0, 0x03, ctrl->r->mac_l2_port_ctrl);
}

static void rteth_set_mac_hw(struct net_device *dev, u8 *mac)
{
	struct rteth_ctrl *ctrl = netdev_priv(dev);
	unsigned long flags;

	spin_lock_irqsave(&ctrl->lock, flags);
	pr_debug("In %s\n", __func__);
	sw_w32((mac[0] << 8) | mac[1], ctrl->r->mac);
	sw_w32((mac[2] << 24) | (mac[3] << 16) | (mac[4] << 8) | mac[5], ctrl->r->mac + 4);

	if (ctrl->r->family_id == RTL8380_FAMILY_ID) {
		/* 2 more registers, ALE/MAC block */
		sw_w32((mac[0] << 8) | mac[1], RTL838X_MAC_ALE);
		sw_w32((mac[2] << 24) | (mac[3] << 16) | (mac[4] << 8) | mac[5],
		       (RTL838X_MAC_ALE + 4));

		sw_w32((mac[0] << 8) | mac[1], RTL838X_MAC2);
		sw_w32((mac[2] << 24) | (mac[3] << 16) | (mac[4] << 8) | mac[5],
		       RTL838X_MAC2 + 4);
	}
	spin_unlock_irqrestore(&ctrl->lock, flags);
}

static int rteth_set_mac_address(struct net_device *dev, void *p)
{
	struct rteth_ctrl *ctrl = netdev_priv(dev);
	const struct sockaddr *addr = p;
	u8 *mac = (u8 *)(addr->sa_data);

	if (!is_valid_ether_addr(addr->sa_data))
		return -EADDRNOTAVAIL;

	dev_addr_set(dev, addr->sa_data);
	rteth_set_mac_hw(dev, mac);

	pr_info("Using MAC %08x%08x\n", sw_r32(ctrl->r->mac), sw_r32(ctrl->r->mac + 4));

	return 0;
}

static int rteth_838x_init_mac(struct rteth_ctrl *ctrl)
{
	pr_info("%s\n", __func__);
	/* fix timer for EEE */
	sw_w32(0x5001411, RTL838X_EEE_TX_TIMER_GIGA_CTRL);
	sw_w32(0x5001417, RTL838X_EEE_TX_TIMER_GELITE_CTRL);

	/* Init VLAN. TODO: Understand what is being done, here */
	for (int i = 0; i <= 28; i++)
		sw_w32(0, 0xd57c + i * 0x80);

	return 0;
}

static int rteth_839x_init_mac(struct rteth_ctrl *ctrl)
{
	/* We will need to set-up EEE and the egress-rate limitation */
	return 0;
}

static int rteth_930x_init_mac(struct rteth_ctrl *ctrl)
{
	return 0;
}

static int rteth_931x_init_mac(struct rteth_ctrl *ctrl)
{
	pr_info("In %s\n", __func__);

	/* Initialize Encapsulation memory and wait until finished */
	sw_w32(0x1, RTL931X_MEM_ENCAP_INIT);
	do { } while (sw_r32(RTL931X_MEM_ENCAP_INIT) & 1);
	pr_info("%s: init ENCAP done\n", __func__);

	/* Initialize Managemen Information Base memory and wait until finished */
	sw_w32(0x1, RTL931X_MEM_MIB_INIT);
	do { } while (sw_r32(RTL931X_MEM_MIB_INIT) & 1);
	pr_info("%s: init MIB done\n", __func__);

	/* Initialize ACL (PIE) memory and wait until finished */
	sw_w32(0x1, RTL931X_MEM_ACL_INIT);
	do { } while (sw_r32(RTL931X_MEM_ACL_INIT) & 1);
	pr_info("%s: init ACL done\n", __func__);

	/* Initialize ALE memory and wait until finished */
	sw_w32(0xFFFFFFFF, RTL931X_MEM_ALE_INIT_0);
	do { } while (sw_r32(RTL931X_MEM_ALE_INIT_0));
	sw_w32(0x7F, RTL931X_MEM_ALE_INIT_1);
	sw_w32(0x7ff, RTL931X_MEM_ALE_INIT_2);
	do { } while (sw_r32(RTL931X_MEM_ALE_INIT_2) & 0x7ff);
	pr_info("%s: init ALE done\n", __func__);

	/* Enable ESD auto recovery */
	sw_w32(0x1, RTL931X_MDX_CTRL_RSVD);

	return 0;
}

static int rteth_get_link_ksettings(struct net_device *ndev,
				    struct ethtool_link_ksettings *cmd)
{
	struct rteth_ctrl *ctrl = netdev_priv(ndev);

	pr_debug("%s called\n", __func__);

	return phylink_ethtool_ksettings_get(ctrl->phylink, cmd);
}

static int rteth_set_link_ksettings(struct net_device *ndev,
				    const struct ethtool_link_ksettings *cmd)
{
	struct rteth_ctrl *ctrl = netdev_priv(ndev);

	pr_debug("%s called\n", __func__);

	return phylink_ethtool_ksettings_set(ctrl->phylink, cmd);
}

static netdev_features_t rteth_fix_features(struct net_device *dev,
					      netdev_features_t features)
{
	return features;
}

static int rteth_83xx_set_features(struct net_device *dev, netdev_features_t features)
{
	struct rteth_ctrl *ctrl = netdev_priv(dev);

	if ((features ^ dev->features) & NETIF_F_RXCSUM) {
		if (!(features & NETIF_F_RXCSUM))
			sw_w32_mask(BIT(3), 0, ctrl->r->mac_l2_port_ctrl);
		else
			sw_w32_mask(0, BIT(3), ctrl->r->mac_l2_port_ctrl);
	}

	return 0;
}

static int rteth_93xx_set_features(struct net_device *dev, netdev_features_t features)
{
	struct rteth_ctrl *ctrl = netdev_priv(dev);

	if ((features ^ dev->features) & NETIF_F_RXCSUM) {
		if (!(features & NETIF_F_RXCSUM))
			sw_w32_mask(BIT(4), 0, ctrl->r->mac_l2_port_ctrl);
		else
			sw_w32_mask(0, BIT(4), ctrl->r->mac_l2_port_ctrl);
	}

	return 0;
}

static struct phylink_pcs *rteth_mac_select_pcs(struct phylink_config *config,
						phy_interface_t interface)
{
	struct net_device *dev = to_net_dev(config->dev);
	struct rteth_ctrl *ctrl = netdev_priv(dev);

	return &ctrl->pcs;
}

static const struct net_device_ops rteth_838x_netdev_ops = {
	.ndo_open = rteth_open,
	.ndo_stop = rteth_stop,
	.ndo_start_xmit = rteth_start_xmit,
	.ndo_select_queue = rteth_83xx_pick_tx_queue,
	.ndo_set_mac_address = rteth_set_mac_address,
	.ndo_validate_addr = eth_validate_addr,
	.ndo_set_rx_mode = rteth_838x_set_rx_mode,
	.ndo_tx_timeout = rteth_tx_timeout,
	.ndo_set_features = rteth_83xx_set_features,
	.ndo_fix_features = rteth_fix_features,
	.ndo_setup_tc = rtl83xx_setup_tc,
};

static const struct rteth_config rteth_838x_cfg = {
	.family_id = RTL8380_FAMILY_ID,
	.cpu_port = RTETH_838X_CPU_PORT,
	.net_irq = rteth_83xx_net_irq,
	.mac_l2_port_ctrl = RTETH_838X_MAC_L2_PORT_CTRL,
	.dma_if_intr_sts = RTL838X_DMA_IF_INTR_STS,
	.dma_if_intr_msk = RTL838X_DMA_IF_INTR_MSK,
	.dma_if_ctrl = RTL838X_DMA_IF_CTRL,
	.mac_force_mode_ctrl = RTL838X_MAC_FORCE_MODE_CTRL,
	.dma_rx_base = RTL838X_DMA_RX_BASE,
	.dma_tx_base = RTL838X_DMA_TX_BASE,
	.dma_if_rx_ring_size = rtl838x_dma_if_rx_ring_size,
	.dma_if_rx_ring_cntr = rtl838x_dma_if_rx_ring_cntr,
	.dma_if_rx_cur = RTL838X_DMA_IF_RX_CUR,
	.rst_glb_ctrl = RTL838X_RST_GLB_CTRL_0,
	.get_mac_link_sts = rtl838x_get_mac_link_sts,
	.get_mac_link_dup_sts = rtl838x_get_mac_link_dup_sts,
	.get_mac_link_spd_sts = rtl838x_get_mac_link_spd_sts,
	.get_mac_rx_pause_sts = rtl838x_get_mac_rx_pause_sts,
	.get_mac_tx_pause_sts = rtl838x_get_mac_tx_pause_sts,
	.mac = RTL838X_MAC,
	.l2_tbl_flush_ctrl = RTL838X_L2_TBL_FLUSH_CTRL,
	.update_counter = rteth_838x_update_counter,
	.create_tx_header = rteth_838x_create_tx_header,
	.decode_tag = rteth_838x_decode_tag,
	.hw_reset = &rteth_838x_hw_reset,
	.init_mac = &rteth_838x_init_mac,
	.netdev_ops = &rteth_838x_netdev_ops,
};

static const struct net_device_ops rteth_839x_netdev_ops = {
	.ndo_open = rteth_open,
	.ndo_stop = rteth_stop,
	.ndo_start_xmit = rteth_start_xmit,
	.ndo_select_queue = rteth_83xx_pick_tx_queue,
	.ndo_set_mac_address = rteth_set_mac_address,
	.ndo_validate_addr = eth_validate_addr,
	.ndo_set_rx_mode = rteth_839x_set_rx_mode,
	.ndo_tx_timeout = rteth_tx_timeout,
	.ndo_set_features = rteth_83xx_set_features,
	.ndo_fix_features = rteth_fix_features,
	.ndo_setup_tc = rtl83xx_setup_tc,
};

static const struct rteth_config rteth_839x_cfg = {
	.family_id = RTL8390_FAMILY_ID,
	.cpu_port = RTETH_839X_CPU_PORT,
	.net_irq = rteth_83xx_net_irq,
	.mac_l2_port_ctrl = RTETH_839X_MAC_L2_PORT_CTRL,
	.dma_if_intr_sts = RTL839X_DMA_IF_INTR_STS,
	.dma_if_intr_msk = RTL839X_DMA_IF_INTR_MSK,
	.dma_if_ctrl = RTL839X_DMA_IF_CTRL,
	.mac_force_mode_ctrl = RTL839X_MAC_FORCE_MODE_CTRL,
	.dma_rx_base = RTL839X_DMA_RX_BASE,
	.dma_tx_base = RTL839X_DMA_TX_BASE,
	.dma_if_rx_ring_size = rtl839x_dma_if_rx_ring_size,
	.dma_if_rx_ring_cntr = rtl839x_dma_if_rx_ring_cntr,
	.dma_if_rx_cur = RTL839X_DMA_IF_RX_CUR,
	.rst_glb_ctrl = RTL839X_RST_GLB_CTRL,
	.get_mac_link_sts = rtl839x_get_mac_link_sts,
	.get_mac_link_dup_sts = rtl839x_get_mac_link_dup_sts,
	.get_mac_link_spd_sts = rtl839x_get_mac_link_spd_sts,
	.get_mac_rx_pause_sts = rtl839x_get_mac_rx_pause_sts,
	.get_mac_tx_pause_sts = rtl839x_get_mac_tx_pause_sts,
	.mac = RTL839X_MAC,
	.l2_tbl_flush_ctrl = RTL839X_L2_TBL_FLUSH_CTRL,
	.update_counter = rteth_839x_update_counter,
	.create_tx_header = rteth_839x_create_tx_header,
	.decode_tag = rteth_839x_decode_tag,
	.hw_reset = &rteth_839x_hw_reset,
	.init_mac = &rteth_839x_init_mac,
	.netdev_ops = &rteth_839x_netdev_ops,
};

static const struct net_device_ops rteth_930x_netdev_ops = {
	.ndo_open = rteth_open,
	.ndo_stop = rteth_stop,
	.ndo_start_xmit = rteth_start_xmit,
	.ndo_select_queue = rteth_93xx_pick_tx_queue,
	.ndo_set_mac_address = rteth_set_mac_address,
	.ndo_validate_addr = eth_validate_addr,
	.ndo_set_rx_mode = rteth_930x_set_rx_mode,
	.ndo_tx_timeout = rteth_tx_timeout,
	.ndo_set_features = rteth_93xx_set_features,
	.ndo_fix_features = rteth_fix_features,
	.ndo_setup_tc = rtl83xx_setup_tc,
};

static const struct rteth_config rteth_930x_cfg = {
	.family_id = RTL9300_FAMILY_ID,
	.cpu_port = RTETH_930X_CPU_PORT,
	.net_irq = rteth_93xx_net_irq,
	.mac_l2_port_ctrl = RTETH_930X_MAC_L2_PORT_CTRL,
	.dma_if_intr_rx_runout_sts = RTL930X_DMA_IF_INTR_RX_RUNOUT_STS,
	.dma_if_intr_rx_done_sts = RTL930X_DMA_IF_INTR_RX_DONE_STS,
	.dma_if_intr_tx_done_sts = RTL930X_DMA_IF_INTR_TX_DONE_STS,
	.dma_if_intr_rx_runout_msk = RTL930X_DMA_IF_INTR_RX_RUNOUT_MSK,
	.dma_if_intr_rx_done_msk = RTL930X_DMA_IF_INTR_RX_DONE_MSK,
	.dma_if_intr_tx_done_msk = RTL930X_DMA_IF_INTR_TX_DONE_MSK,
	.l2_ntfy_if_intr_sts = RTL930X_L2_NTFY_IF_INTR_STS,
	.l2_ntfy_if_intr_msk = RTL930X_L2_NTFY_IF_INTR_MSK,
	.dma_if_ctrl = RTL930X_DMA_IF_CTRL,
	.mac_force_mode_ctrl = RTL930X_MAC_FORCE_MODE_CTRL,
	.dma_rx_base = RTL930X_DMA_RX_BASE,
	.dma_tx_base = RTL930X_DMA_TX_BASE,
	.dma_if_rx_ring_size = rtl930x_dma_if_rx_ring_size,
	.dma_if_rx_ring_cntr = rtl930x_dma_if_rx_ring_cntr,
	.dma_if_rx_cur = RTL930X_DMA_IF_RX_CUR,
	.rst_glb_ctrl = RTL930X_RST_GLB_CTRL_0,
	.get_mac_link_sts = rtl930x_get_mac_link_sts,
	.get_mac_link_dup_sts = rtl930x_get_mac_link_dup_sts,
	.get_mac_link_spd_sts = rtl930x_get_mac_link_spd_sts,
	.get_mac_rx_pause_sts = rtl930x_get_mac_rx_pause_sts,
	.get_mac_tx_pause_sts = rtl930x_get_mac_tx_pause_sts,
	.mac = RTL930X_MAC_L2_ADDR_CTRL,
	.l2_tbl_flush_ctrl = RTL930X_L2_TBL_FLUSH_CTRL,
	.update_counter = rteth_930x_update_counter,
	.create_tx_header = rteth_930x_create_tx_header,
	.decode_tag = rteth_930x_decode_tag,
	.hw_reset = &rteth_93xx_hw_reset,
	.init_mac = &rteth_930x_init_mac,
	.netdev_ops = &rteth_930x_netdev_ops,
};

static const struct net_device_ops rteth_931x_netdev_ops = {
	.ndo_open = rteth_open,
	.ndo_stop = rteth_stop,
	.ndo_start_xmit = rteth_start_xmit,
	.ndo_select_queue = rteth_93xx_pick_tx_queue,
	.ndo_set_mac_address = rteth_set_mac_address,
	.ndo_validate_addr = eth_validate_addr,
	.ndo_set_rx_mode = rtl931x_eth_set_multicast_list,
	.ndo_tx_timeout = rteth_tx_timeout,
	.ndo_set_features = rteth_93xx_set_features,
	.ndo_fix_features = rteth_fix_features,
};

static const struct rteth_config rteth_931x_cfg = {
	.family_id = RTL9310_FAMILY_ID,
	.cpu_port = RTETH_931X_CPU_PORT,
	.net_irq = rteth_93xx_net_irq,
	.mac_l2_port_ctrl = RTETH_931X_MAC_L2_PORT_CTRL,
	.dma_if_intr_rx_runout_sts = RTL931X_DMA_IF_INTR_RX_RUNOUT_STS,
	.dma_if_intr_rx_done_sts = RTL931X_DMA_IF_INTR_RX_DONE_STS,
	.dma_if_intr_tx_done_sts = RTL931X_DMA_IF_INTR_TX_DONE_STS,
	.dma_if_intr_rx_runout_msk = RTL931X_DMA_IF_INTR_RX_RUNOUT_MSK,
	.dma_if_intr_rx_done_msk = RTL931X_DMA_IF_INTR_RX_DONE_MSK,
	.dma_if_intr_tx_done_msk = RTL931X_DMA_IF_INTR_TX_DONE_MSK,
	.l2_ntfy_if_intr_sts = RTL931X_L2_NTFY_IF_INTR_STS,
	.l2_ntfy_if_intr_msk = RTL931X_L2_NTFY_IF_INTR_MSK,
	.dma_if_ctrl = RTL931X_DMA_IF_CTRL,
	.mac_force_mode_ctrl = RTL931X_MAC_FORCE_MODE_CTRL,
	.dma_rx_base = RTL931X_DMA_RX_BASE,
	.dma_tx_base = RTL931X_DMA_TX_BASE,
	.dma_if_rx_ring_size = rtl931x_dma_if_rx_ring_size,
	.dma_if_rx_ring_cntr = rtl931x_dma_if_rx_ring_cntr,
	.dma_if_rx_cur = RTL931X_DMA_IF_RX_CUR,
	.rst_glb_ctrl = RTL931X_RST_GLB_CTRL,
	.get_mac_link_sts = rtldsa_931x_get_mac_link_sts,
	.get_mac_link_dup_sts = rtl931x_get_mac_link_dup_sts,
	.get_mac_link_spd_sts = rtl931x_get_mac_link_spd_sts,
	.get_mac_rx_pause_sts = rtl931x_get_mac_rx_pause_sts,
	.get_mac_tx_pause_sts = rtl931x_get_mac_tx_pause_sts,
	.mac = RTL931X_MAC_L2_ADDR_CTRL,
	.l2_tbl_flush_ctrl = RTL931X_L2_TBL_FLUSH_CTRL,
	.update_counter = rteth_931x_update_counter,
	.create_tx_header = rteth_931x_create_tx_header,
	.decode_tag = rteth_931x_decode_tag,
	.hw_reset = &rteth_93xx_hw_reset,
	.init_mac = &rteth_931x_init_mac,
	.netdev_ops = &rteth_931x_netdev_ops,
};

static const struct phylink_pcs_ops rteth_pcs_ops = {
	.pcs_get_state = rteth_pcs_get_state,
	.pcs_an_restart = rteth_pcs_an_restart,
	.pcs_config = rteth_pcs_config,
};

static const struct phylink_mac_ops rteth_mac_ops = {
	.mac_select_pcs = rteth_mac_select_pcs,
	.mac_config = rteth_mac_config,
	.mac_link_down = rteth_mac_link_down,
	.mac_link_up = rteth_mac_link_up,
};

static const struct ethtool_ops rteth_ethtool_ops = {
	.get_link_ksettings = rteth_get_link_ksettings,
	.set_link_ksettings = rteth_set_link_ksettings,
};

static int rtl838x_eth_probe(struct platform_device *pdev)
{
	struct net_device *dev;
	struct device_node *dn = pdev->dev.of_node;
	struct rteth_ctrl *ctrl;
	const struct rteth_config *matchdata;
	phy_interface_t phy_mode;
	struct phylink *phylink;
	u8 mac_addr[ETH_ALEN] = {0};
	int err = 0, rxrings, rxringlen;
	struct ring_b *ring;

	pr_info("Probing RTL838X eth device pdev: %x, dev: %x\n",
		(u32)pdev, (u32)(&pdev->dev));

	if (!dn) {
		dev_err(&pdev->dev, "No DT found\n");
		return -EINVAL;
	}

	matchdata = (const struct rteth_config *)device_get_match_data(&pdev->dev);

	rxrings = (matchdata->family_id == RTL8380_FAMILY_ID ||
		   matchdata->family_id == RTL8390_FAMILY_ID) ? 8 : 32;
	rxrings = rxrings > MAX_RXRINGS ? MAX_RXRINGS : rxrings;
	rxringlen = MAX_ENTRIES / rxrings;
	rxringlen = rxringlen > MAX_RXLEN ? MAX_RXLEN : rxringlen;

	dev = devm_alloc_etherdev_mqs(&pdev->dev, sizeof(struct rteth_ctrl), TXRINGS, rxrings);
	if (!dev)
		return -ENOMEM;
	SET_NETDEV_DEV(dev, &pdev->dev);
	ctrl = netdev_priv(dev);
	ctrl->r = matchdata;

	/* Allocate buffer memory */
	ctrl->membase = dmam_alloc_coherent(&pdev->dev, rxrings * rxringlen * RING_BUFFER +
					    sizeof(struct ring_b) + sizeof(struct notify_b),
					    (void *)&dev->mem_start, GFP_KERNEL);
	if (!ctrl->membase) {
		dev_err(&pdev->dev, "cannot allocate DMA buffer\n");
		return -ENOMEM;
	}

	/* Allocate ring-buffer space at the end of the allocated memory */
	ring = ctrl->membase;
	ring->rx_space = ctrl->membase + sizeof(struct ring_b) + sizeof(struct notify_b);

	spin_lock_init(&ctrl->lock);

	dev->ethtool_ops = &rteth_ethtool_ops;
	dev->min_mtu = ETH_ZLEN;
	dev->max_mtu = DEFAULT_MTU;
	dev->features = NETIF_F_RXCSUM | NETIF_F_HW_CSUM;
	dev->hw_features = NETIF_F_RXCSUM;
	dev->netdev_ops = ctrl->r->netdev_ops;

	ctrl->rxringlen = rxringlen;
	ctrl->rxrings = rxrings;

	/* Obtain device IRQ number */
	dev->irq = platform_get_irq(pdev, 0);
	if (dev->irq < 0)
		return -ENODEV;

	err = devm_request_irq(&pdev->dev, dev->irq, ctrl->r->net_irq,
			       IRQF_SHARED, dev->name, dev);
	if (err) {
		dev_err(&pdev->dev, "%s: could not acquire interrupt: %d\n",
			__func__, err);
		return err;
	}

	ctrl->r->init_mac(ctrl);

	/* Try to get mac address in the following order:
	 * 1) from device tree data
	 * 2) from internal registers set by bootloader
	 */
	err = of_get_mac_address(pdev->dev.of_node, mac_addr);
	if (err == -EPROBE_DEFER)
		return err;

	if (is_valid_ether_addr(mac_addr)) {
		rteth_set_mac_hw(dev, mac_addr);
	} else {
		mac_addr[0] = (sw_r32(ctrl->r->mac) >> 8) & 0xff;
		mac_addr[1] = sw_r32(ctrl->r->mac) & 0xff;
		mac_addr[2] = (sw_r32(ctrl->r->mac + 4) >> 24) & 0xff;
		mac_addr[3] = (sw_r32(ctrl->r->mac + 4) >> 16) & 0xff;
		mac_addr[4] = (sw_r32(ctrl->r->mac + 4) >> 8) & 0xff;
		mac_addr[5] = sw_r32(ctrl->r->mac + 4) & 0xff;
	}
	dev_addr_set(dev, mac_addr);
	/* if the address is invalid, use a random value */
	if (!is_valid_ether_addr(dev->dev_addr)) {
		struct sockaddr sa = { AF_UNSPEC };

		netdev_warn(dev, "Invalid MAC address, using random\n");
		eth_hw_addr_random(dev);
		memcpy(sa.sa_data, dev->dev_addr, ETH_ALEN);
		if (rteth_set_mac_address(dev, &sa))
			netdev_warn(dev, "Failed to set MAC address.\n");
	}
	pr_info("Using MAC %08x%08x\n", sw_r32(ctrl->r->mac),
		sw_r32(ctrl->r->mac + 4));
	strscpy(dev->name, "eth%d", sizeof(dev->name));

	ctrl->pdev = pdev;
	ctrl->netdev = dev;

	for (int i = 0; i < ctrl->rxrings; i++) {
		ctrl->rx_qs[i].id = i;
		ctrl->rx_qs[i].ctrl = ctrl;
		netif_napi_add(dev, &ctrl->rx_qs[i].napi, rtl838x_poll_rx);
	}

	platform_set_drvdata(pdev, dev);

	err = devm_register_netdev(&pdev->dev, dev);
	if (err)
		return err;

	phy_mode = PHY_INTERFACE_MODE_NA;
	err = of_get_phy_mode(dn, &phy_mode);
	if (err < 0) {
		dev_err(&pdev->dev, "incorrect phy-mode\n");
		return -EINVAL;
	}

	ctrl->pcs.ops = &rteth_pcs_ops;
	ctrl->phylink_config.dev = &dev->dev;
	ctrl->phylink_config.type = PHYLINK_NETDEV;
	ctrl->phylink_config.mac_capabilities =
		MAC_10 | MAC_100 | MAC_1000FD |	MAC_SYM_PAUSE | MAC_ASYM_PAUSE;

	__set_bit(PHY_INTERFACE_MODE_INTERNAL, ctrl->phylink_config.supported_interfaces);

	phylink = phylink_create(&ctrl->phylink_config, pdev->dev.fwnode,
				 phy_mode, &rteth_mac_ops);

	if (IS_ERR(phylink))
		return PTR_ERR(phylink);
	ctrl->phylink = phylink;

	return 0;
}

static void rtl838x_eth_remove(struct platform_device *pdev)
{
	struct net_device *dev = platform_get_drvdata(pdev);
	struct rteth_ctrl *ctrl = netdev_priv(dev);

	if (dev) {
		pr_info("Removing platform driver for rtl838x-eth\n");
		rtl838x_hw_stop(ctrl);

		netif_tx_stop_all_queues(dev);

		for (int i = 0; i < ctrl->rxrings; i++)
			netif_napi_del(&ctrl->rx_qs[i].napi);
	}
}

static const struct of_device_id rtl838x_eth_of_ids[] = {
	{
		.compatible = "realtek,rtl8380-eth",
		.data = &rteth_838x_cfg,
	},
	{
		.compatible = "realtek,rtl8392-eth",
		.data = &rteth_839x_cfg,
	},
	{
		.compatible = "realtek,rtl9301-eth",
		.data = &rteth_930x_cfg,
	},
	{
		.compatible = "realtek,rtl9311-eth",
		.data = &rteth_931x_cfg,
	},
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, rtl838x_eth_of_ids);

static struct platform_driver rtl838x_eth_driver = {
	.probe  = rtl838x_eth_probe,
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

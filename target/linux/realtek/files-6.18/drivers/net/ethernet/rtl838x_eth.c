// SPDX-License-Identifier: GPL-2.0-only
/* linux/drivers/net/ethernet/rtl838x_eth.c
 * Copyright (C) 2020 B. Koblitz
 */

#include <linux/cacheflush.h>
#include <linux/dma-mapping.h>
#include <linux/etherdevice.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/mfd/syscon.h>
#include <linux/minmax.h>
#include <linux/platform_device.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/of.h>
#include <linux/of_net.h>
#include <linux/of_mdio.h>
#include <linux/module.h>
#include <linux/phylink.h>
#include <linux/pkt_sched.h>
#include <linux/regmap.h>
#include <net/dsa.h>
#include <net/switchdev.h>

#include "rtl838x_eth.h"

#define RTETH_OWN_CPU			1
#define RTETH_RX_RING_SIZE		128
#define RTETH_RX_RINGS			2
#define RTETH_TX_RING_SIZE		16
#define RTETH_TX_RINGS			2
#define RTETH_TX_TRIGGER(ctrl, ring)	((0x16 >> ring) & ctrl->r->tx_trigger_mask)

#define NOTIFY_EVENTS	10
#define NOTIFY_BLOCKS	10
#define RX_TRUNCATE_EN_93XX BIT(6)
#define RX_TRUNCATE_EN_83XX BIT(4)
#define TX_PAD_EN_838X BIT(5)
#define WRAP		0x2
#define RING_BUFFER	1600

struct rteth_packet {
	/* hardware header part as required by SoC */
	dma_addr_t		dma;
	u16			reserved;
	u16			size;
	u16			offset;
	u16			len;
	u16			cpu_tag[10];
	/* software mangement and data part */
	union {
		struct sk_buff	*skb;
		char		*buf;
	};
} __packed __aligned(1);

struct rteth_rx {
	int			slot;
	dma_addr_t		ring[RTETH_RX_RING_SIZE];
	struct rteth_packet	packet[RTETH_RX_RING_SIZE];
};

struct rteth_tx {
	int			slot;
	dma_addr_t		ring[RTETH_TX_RING_SIZE];
	struct rteth_packet	packet[RTETH_TX_RING_SIZE];
};

struct n_event {
	u32	type:2;
	u32	fidVid:12;
	u64	mac:48;
	u32	slp:6;
	u32	valid:1;
	u32	reserved:27;
} __packed __aligned(1);

struct notify_block {
	struct n_event	events[NOTIFY_EVENTS];
};

struct notify_b {
	struct notify_block	blocks[NOTIFY_BLOCKS];
	u32			reserved1[8];
	u32			ring[NOTIFY_BLOCKS];
	u32			reserved2[8];
};

static void rteth_838x_create_tx_header(struct rteth_packet *h, unsigned int dest_port, int prio)
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

static void rteth_839x_create_tx_header(struct rteth_packet *h, unsigned int dest_port, int prio)
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

static void rteth_93xx_create_tx_header(struct rteth_packet *h, unsigned int dest_port, int prio)
{
	h->cpu_tag[0] = 0x8000;  /* CPU tag marker */
	h->cpu_tag[1] = FIELD_PREP(RTL93XX_CPU_TAG1_FWD_MASK, RTL93XX_CPU_TAG1_FWD_PHYSICAL) |
		        FIELD_PREP(RTL93XX_CPU_TAG1_IGNORE_STP_MASK, 1);

	h->cpu_tag[2] = (prio >= 0) ? (BIT(5) | (prio & 0x1f)) << 8 : 0;
	h->cpu_tag[3] = 0;
	h->cpu_tag[4] = BIT_ULL(dest_port) >> 48;
	h->cpu_tag[5] = BIT_ULL(dest_port) >> 32;
	h->cpu_tag[6] = BIT_ULL(dest_port) >> 16;
	h->cpu_tag[7] = BIT_ULL(dest_port) & 0xffff;
}

struct rtl838x_rx_q {
	int id;
	struct rteth_ctrl *ctrl;
	struct napi_struct napi;
};

struct rteth_ctrl {
	struct regmap *map;
	struct net_device *netdev;
	struct platform_device *pdev;
	void *membase;
	spinlock_t lock;
	struct mii_bus *mii_bus;
	struct rtl838x_rx_q rx_qs[RTETH_RX_RINGS];
	struct phylink *phylink;
	struct phylink_config phylink_config;
	const struct rteth_config *r;
	u32 lastEvent;
	/* receive handling */
	dma_addr_t		rx_buf_dma;
	char			*rx_buf;
	dma_addr_t		rx_data_dma;
	spinlock_t		rx_lock;
	struct rteth_rx		*rx_data;
	/* transmit handling */
	dma_addr_t		tx_dma;
	spinlock_t		tx_lock;
	struct rteth_tx		*tx_data;
};

static inline void rteth_reenable_irq(struct rteth_ctrl *ctrl, int ring)
{
	u32 shift = ctrl->r->rx_rings % 32;
	u32 reg = ctrl->r->rx_rings / 32;
	u32 bit = BIT(ring + shift);
	unsigned long flags;

	/* locking needed for synchronization with rteth_confirm_and_disable_irqs() */
	spin_lock_irqsave(&ctrl->lock, flags);
	regmap_update_bits(ctrl->map, ctrl->r->dma_if_intr_msk + reg * 4, bit, bit);
	spin_unlock_irqrestore(&ctrl->lock, flags);
}

static inline void rteth_confirm_and_disable_irqs(struct rteth_ctrl *ctrl,
						  unsigned long *rings, bool *l2)
{
	u32 mask = GENMASK(ctrl->r->rx_rings - 1, 0);
	u32 shift = ctrl->r->rx_rings % 32;
	u32 reg = ctrl->r->rx_rings / 32;
	unsigned long flags;
	u32 active;

	/* get all irqs, disable only rx (on RTL839x this keeps L2), confirm all */
	spin_lock_irqsave(&ctrl->lock, flags);
	regmap_read(ctrl->map, ctrl->r->dma_if_intr_sts + reg * 4, &active);
	regmap_update_bits(ctrl->map, ctrl->r->dma_if_intr_msk + reg * 4,
			   active & (mask << shift), 0);
	regmap_write(ctrl->map, ctrl->r->dma_if_intr_sts + reg * 4, active);
	spin_unlock_irqrestore(&ctrl->lock, flags);

	/* ~mask filters out RTL93xx devices */
	*l2 = !!(active & ~mask & RTL839X_DMA_IF_INTR_NOTIFY_MASK);
	*rings = (active >> shift) & mask;
}

static void rteth_disable_all_irqs(struct rteth_ctrl *ctrl)
{
	int registers = ((ctrl->r->rx_rings * 2 + 7) / 32) + 1;

	for (int reg = 0; reg < registers; reg++) {
		regmap_write(ctrl->map, ctrl->r->dma_if_intr_msk + reg * 4, 0);
		regmap_write(ctrl->map, ctrl->r->dma_if_intr_sts + reg * 4, GENMASK(31, 0));
	}
}

static void rteth_enable_all_rx_irqs(struct rteth_ctrl *ctrl)
{
	int mask, reg;

	/*
	 * The hardware has several types of interrupts. Basically for rx/tx completion and
	 * if hardware queues run out. For now the driver only needs notification about new
	 * incoming packets. Leave everything else disabled.
	 */
	mask = GENMASK(ctrl->r->rx_rings - 1, 0) << (ctrl->r->rx_rings % 32);
	reg = ctrl->r->rx_rings / 32;
	regmap_update_bits(ctrl->map, ctrl->r->dma_if_intr_msk + reg * 4, mask, mask);

	/*
	 * RTL839x has additional L2 notification interrupts. Simply activate them. All other
	 * devices that do not have the feature have adequate reserved bit space and ignore it.
	 */
	mask = GENMASK(2, 0) << ((ctrl->r->rx_rings * 2 + 4) % 32);
	reg = (ctrl->r->rx_rings * 2 + 4) / 32;
	regmap_update_bits(ctrl->map, ctrl->r->dma_if_intr_msk + reg * 4, mask, mask);
}

static void rteth_83xx_update_counter(struct rteth_ctrl *ctrl, int ring, int released)
{
	/* Free floating rings without space tracking */
}

static void rteth_93xx_update_counter(struct rteth_ctrl *ctrl, int ring, int released)
{
	int shift = (ring % 3) * 10;
	int reg = (ring / 3) * 4;

	/* writing x to the ring counter increases ring free space by x */
	regmap_write(ctrl->map, ctrl->r->dma_if_rx_ring_cntr + reg, released << shift);
}

struct dsa_tag {
	u8	reason;
	u8	queue;
	u16	port;
	u8	l2_offloaded;
	u8	prio;
	bool	crc_error;
};

static bool rteth_838x_decode_tag(struct rteth_packet *h, struct dsa_tag *t)
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

static bool rteth_839x_decode_tag(struct rteth_packet *h, struct dsa_tag *t)
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

static bool rteth_93xx_decode_tag(struct rteth_packet *h, struct dsa_tag *t)
{
	t->port = (h->cpu_tag[0] >> 8) & 0x3f;
	t->queue = (h->cpu_tag[2] >> 11) & 0x1f;
	t->reason = h->cpu_tag[7] & 0x3f;
	t->crc_error = h->cpu_tag[1] & BIT(6);
	t->l2_offloaded = (t->reason >= 19 && t->reason <= 27) ? 0 : 1;

	if (t->reason != 63)
		pr_debug("%s: Reason %d, port %d, queue %d\n", __func__, t->reason, t->port, t->queue);

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
	struct notify_b *nb = ctrl->membase;
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

static irqreturn_t rteth_net_irq(int irq, void *dev_id)
{
	struct net_device *ndev = dev_id;
	struct rteth_ctrl *ctrl = netdev_priv(ndev);
	unsigned long ring, rings;
	bool l2;

	rteth_confirm_and_disable_irqs(ctrl, &rings, &l2);
	for_each_set_bit(ring, &rings, RTETH_RX_RINGS) {
		netdev_dbg(ndev, "schedule rx ring %lu\n", ring);
		napi_schedule(&ctrl->rx_qs[ring].napi);
	}

	if (unlikely(l2))
		rtl839x_l2_notification_handler(ctrl);

	return IRQ_HANDLED;
}

static void rteth_nic_reset(struct rteth_ctrl *ctrl, int reset_mask)
{
	int val;

	pr_info("RESETTING CPU_PORT %d\n", ctrl->r->cpu_port);
	regmap_update_bits(ctrl->map, ctrl->r->mac_l2_port_ctrl, 0x3, 0x0);
	msleep(100);

	/* Reset NIC (SW_NIC_RST) and queues (SW_Q_RST) */
	regmap_update_bits(ctrl->map, ctrl->r->rst_glb_ctrl, reset_mask, reset_mask);
	regmap_read_poll_timeout(ctrl->map, ctrl->r->rst_glb_ctrl, val,
				 !(val & reset_mask), 1000, 1000000);

	msleep(100);
}

static void rteth_838x_hw_reset(struct rteth_ctrl *ctrl)
{
	rteth_nic_reset(ctrl, 0xc);

	/* Free floating rings without space tracking */
	regmap_write(ctrl->map, ctrl->r->dma_if_rx_ring_size, 0);
}

static void rteth_839x_hw_reset(struct rteth_ctrl *ctrl)
{
	u32 int_saved, nbuf;

	/* Preserve L2 notification and NBUF settings */
	regmap_read(ctrl->map, ctrl->r->dma_if_intr_msk, &int_saved);
	regmap_read(ctrl->map, RTL839X_DMA_IF_NBUF_BASE_DESC_ADDR_CTRL, &nbuf);

	/* Disable link change interrupt on RTL839x */
	regmap_write(ctrl->map, RTL839X_IMR_PORT_LINK_STS_CHG, 0);
	regmap_write(ctrl->map, RTL839X_IMR_PORT_LINK_STS_CHG + 4, 0);

	rteth_nic_reset(ctrl, 0xc);

	/* Re-enable link change interrupt */
	regmap_write(ctrl->map, RTL839X_ISR_PORT_LINK_STS_CHG, 0xffffffff);
	regmap_write(ctrl->map, RTL839X_ISR_PORT_LINK_STS_CHG + 4, 0xffffffff);
	regmap_write(ctrl->map, RTL839X_IMR_PORT_LINK_STS_CHG, 0xffffffff);
	regmap_write(ctrl->map, RTL839X_IMR_PORT_LINK_STS_CHG + 4, 0xffffffff);

	/* Restore notification settings: on RTL838x these bits are null */
	regmap_update_bits(ctrl->map, ctrl->r->dma_if_intr_msk, 7 << 20, int_saved & (7 << 20));
	regmap_write(ctrl->map, RTL839X_DMA_IF_NBUF_BASE_DESC_ADDR_CTRL, nbuf);

	/* Free floating rings without space tracking */
	regmap_write(ctrl->map, ctrl->r->dma_if_rx_ring_size, 0);
}

static void rteth_93xx_hw_reset(struct rteth_ctrl *ctrl)
{
	rteth_nic_reset(ctrl, 0x6);

	/* Setup Head of Line */
	for (int ring = 0; ring < RTETH_RX_RINGS; ring++) {
		int cnt = min(RTETH_RX_RING_SIZE, 0x3ff);
		int shift = (ring % 3) * 10;
		int reg = (ring / 3) * 4;
		u32 v;

		/* set ring size */
		regmap_update_bits(ctrl->map, ctrl->r->dma_if_rx_ring_size + reg,
				   0x3ff << shift, cnt << shift);
		/* clear counters by simply writing the current register values back */
		regmap_read(ctrl->map, ctrl->r->dma_if_rx_ring_cntr + reg, &v);
		regmap_write(ctrl->map, ctrl->r->dma_if_rx_ring_cntr + reg, v);
	}
}

static void rteth_setup_cpu_rx_rings(struct rteth_ctrl *ctrl)
{
	/*
	 * Realtek switches either have 8 (RTL83xx) or 32 (RTL93xx) receive queues. Whenever
	 * a packet is trapped/received for the CPU it is put into one of these queues. This
	 * is configured via mapping registers in two ways:
	 *
	 * - Switching queue/priority to CPU queue mapping (RTL83xx)
	 * - Reason (why it is sent to CPU) to CPU queue mapping (all devices)
	 *
	 * With only low performance CPUs there is not much benefit of using all of these
	 * queues in parallel. Especially because each queue needs buffer space. To keep
	 * the queue limit simple, just write the desired CPU queue in a round robin style
	 * to the registers.
	 */

	if (ctrl->r->qm_pkt2cpu_intpri_map) {
		for (int priority = 0; priority < 8; priority++) {
			int reg = ctrl->r->qm_pkt2cpu_intpri_map;
			int ring = priority % RTETH_RX_RINGS;
			int shift = priority * 3;

			regmap_update_bits(ctrl->map, reg, 0x7 << shift, ring << shift);
		}
	}

	if (ctrl->r->qm_rsn2cpuqid_ctrl) {
		int mask, bits_per_field, fields_per_reg, reason_cnt;

		mask = ctrl->r->rx_rings - 1;
		bits_per_field = fls(mask);
		fields_per_reg = 32 / bits_per_field;
		reason_cnt = ctrl->r->qm_rsn2cpuqid_cnt * fields_per_reg;

		/* Reason registers have gaps. Do not care for now. */
		for (int reason = 0; reason < reason_cnt; reason++) {
			int reg = ctrl->r->qm_rsn2cpuqid_ctrl + 4 * (reason / fields_per_reg);
			int shift = (reason % fields_per_reg) * bits_per_field;
			int ring = reason % RTETH_RX_RINGS;

			regmap_update_bits(ctrl->map, reg, mask << shift, ring << shift);
		}
	}
}

static void rteth_hw_ring_setup(struct rteth_ctrl *ctrl)
{
	for (int r = 0; r < RTETH_RX_RINGS; r++)
		regmap_write(ctrl->map, ctrl->r->dma_rx_base + r * 4,
			     ctrl->rx_data_dma +
			     r * sizeof(struct rteth_rx) + offsetof(struct rteth_rx, ring));

	for (int r = 0; r < RTETH_TX_RINGS; r++)
		regmap_write(ctrl->map, ctrl->r->dma_tx_base + r * 4,
			     ctrl->tx_dma +
			     r * sizeof(struct rteth_tx) + offsetof(struct rteth_tx, ring));
}

static void rteth_838x_hw_en_rxtx(struct rteth_ctrl *ctrl)
{
	/* Truncate RX buffer to DEFAULT_MTU bytes, pad TX */
	regmap_write(ctrl->map, ctrl->r->dma_if_ctrl,
		     (DEFAULT_MTU << 16) | RX_TRUNCATE_EN_83XX | TX_PAD_EN_838X);

	rteth_enable_all_rx_irqs(ctrl);

	/* Enable DMA, engine expects empty FCS field */
	regmap_update_bits(ctrl->map, ctrl->r->dma_if_ctrl,
			   ctrl->r->tx_rx_enable, ctrl->r->tx_rx_enable);

	/* Restart TX/RX to CPU port */
	regmap_update_bits(ctrl->map, ctrl->r->dma_if_ctrl, 0x3, 0x3);
	/* Set Speed, duplex, flow control
	 * FORCE_EN | LINK_EN | NWAY_EN | DUP_SEL
	 * | SPD_SEL = 0b10 | FORCE_FC_EN | PHY_MASTER_SLV_MANUAL_EN
	 * | MEDIA_SEL
	 */
	regmap_write(ctrl->map, ctrl->r->mac_force_mode_ctrl, 0x6192F);

	/* Enable CRC checks on CPU-port */
	regmap_update_bits(ctrl->map, ctrl->r->mac_l2_port_ctrl, BIT(3), BIT(3));
}

static void rteth_839x_hw_en_rxtx(struct rteth_ctrl *ctrl)
{
	/* Setup CPU-Port: RX Buffer */
	regmap_write(ctrl->map, ctrl->r->dma_if_ctrl, (DEFAULT_MTU << 5) | RX_TRUNCATE_EN_83XX);

	rteth_enable_all_rx_irqs(ctrl);

	/* Enable DMA */
	regmap_update_bits(ctrl->map, ctrl->r->dma_if_ctrl,
			   ctrl->r->tx_rx_enable, ctrl->r->tx_rx_enable);

	/* Restart TX/RX to CPU port, enable CRC checking */
	regmap_update_bits(ctrl->map, ctrl->r->mac_l2_port_ctrl, 0x3 | BIT(3), 0x3 | BIT(3));

	/* CPU port joins Lookup Miss Flooding Portmask */
	/* TODO: The code below should also work for the RTL838x */
	regmap_write(ctrl->map, RTL839X_TBL_ACCESS_L2_CTRL, 0x28000);
	regmap_update_bits(ctrl->map, RTL839X_TBL_ACCESS_L2_DATA(0), BIT(31), BIT(31));
	regmap_write(ctrl->map, RTL839X_TBL_ACCESS_L2_CTRL, 0x38000);

	/* Force CPU port link up */
	regmap_update_bits(ctrl->map, ctrl->r->mac_force_mode_ctrl, 0x3, 0x3);
}

static void rteth_930x_hw_en_rxtx(struct rteth_ctrl *ctrl)
{
	/* Setup CPU-Port: RX Buffer truncated at DEFAULT_MTU Bytes */
	regmap_write(ctrl->map, ctrl->r->dma_if_ctrl, (DEFAULT_MTU << 16) | RX_TRUNCATE_EN_93XX);

	rteth_enable_all_rx_irqs(ctrl);

	/* Enable DMA */
	regmap_set_bits(ctrl->map, ctrl->r->dma_if_ctrl, ctrl->r->tx_rx_enable);

	/* Restart TX/RX to CPU port, enable CRC checking */
	regmap_set_bits(ctrl->map, ctrl->r->mac_l2_port_ctrl, 0x3 | BIT(4));

	regmap_set_bits(ctrl->map, RTL930X_L2_UNKN_UC_FLD_PMSK, BIT(ctrl->r->cpu_port));
	regmap_write(ctrl->map, ctrl->r->mac_force_mode_ctrl, 0x217);
}

static void rteth_931x_hw_en_rxtx(struct rteth_ctrl *ctrl)
{
	/* Setup CPU-Port: RX Buffer truncated at DEFAULT_MTU Bytes */
	regmap_write(ctrl->map, ctrl->r->dma_if_ctrl, (DEFAULT_MTU << 16) | RX_TRUNCATE_EN_93XX);

	rteth_enable_all_rx_irqs(ctrl);

	/* Enable DMA */
	regmap_set_bits(ctrl->map, ctrl->r->dma_if_ctrl, ctrl->r->tx_rx_enable);

	/* Restart TX/RX to CPU port, enable CRC checking */
	regmap_set_bits(ctrl->map, ctrl->r->mac_l2_port_ctrl, 0x3 | BIT(4));

	regmap_set_bits(ctrl->map, RTL931X_L2_UNKN_UC_FLD_PMSK, BIT(ctrl->r->cpu_port));
	regmap_write(ctrl->map, ctrl->r->mac_force_mode_ctrl, 0x2a1d);
}

static void rteth_setup_ring_buffer(struct rteth_ctrl *ctrl)
{
	dma_addr_t rx_buf_dma = ctrl->rx_buf_dma;
	char *rx_buf = ctrl->rx_buf;

	for (int r = 0; r < RTETH_RX_RINGS; r++) {
		for (int i = 0; i < RTETH_RX_RING_SIZE; i++) {
			ctrl->rx_data[r].packet[i].size = RING_BUFFER;
			ctrl->rx_data[r].packet[i].dma = rx_buf_dma;
			ctrl->rx_data[r].packet[i].buf = rx_buf;
			ctrl->rx_data[r].ring[i] = ctrl->rx_data_dma +
						   sizeof(struct rteth_rx) * r +
						   offsetof(struct rteth_rx, packet) +
						   sizeof(struct rteth_packet) * i +
						   RTETH_OWN_CPU;
			rx_buf += RING_BUFFER;
			rx_buf_dma += RING_BUFFER;
		}

		ctrl->rx_data[r].ring[RTETH_RX_RING_SIZE - 1] |= WRAP;
		ctrl->rx_data[r].slot = 0;
	}

	for (int r = 0; r < RTETH_TX_RINGS; r++) {
		for (int i = 0; i < RTETH_TX_RING_SIZE; i++) {
			ctrl->tx_data[r].packet[i].skb = NULL;
			ctrl->tx_data[r].ring[i] = ctrl->tx_dma +
						   sizeof(struct rteth_tx) * r +
						   offsetof(struct rteth_tx, packet) +
						   sizeof(struct rteth_packet) * i;
		}

		ctrl->tx_data[r].ring[RTETH_TX_RING_SIZE - 1] |= WRAP;
		ctrl->tx_data[r].slot = 0;
	}
}

static void rteth_839x_setup_notify_ring_buffer(struct rteth_ctrl *ctrl)
{
	struct notify_b *b = ctrl->membase;

	for (int i = 0; i < NOTIFY_BLOCKS; i++)
		b->ring[i] = KSEG1ADDR(&b->blocks[i]) | 1 | (i == (NOTIFY_BLOCKS - 1) ? WRAP : 0);

	regmap_write(ctrl->map, RTL839X_DMA_IF_NBUF_BASE_DESC_ADDR_CTRL, (u32)b->ring);
	regmap_update_bits(ctrl->map, RTL839X_L2_NOTIFICATION_CTRL, 0x3ff << 2, 100 << 2);

	/* Setup notification events */

	/* RTL8390_L2_CTRL_0_FLUSH_NOTIFY_EN */
	regmap_set_bits(ctrl->map, RTL839X_L2_CTRL_0, BIT(14));
	/* SUSPEND_NOTIFICATION_EN */
	regmap_set_bits(ctrl->map, RTL839X_L2_NOTIFICATION_CTRL, BIT(12));

	/* Enable Notification */
	regmap_set_bits(ctrl->map, RTL839X_L2_NOTIFICATION_CTRL, BIT(0));
	ctrl->lastEvent = 0;

	/* Make sure the ring structure is visible to the ASIC */
	mb();
	flush_cache_all();
}

static void rteth_838x_hw_init(struct rteth_ctrl *ctrl)
{
	/* Trap IGMP/MLD traffic to CPU-Port */
	regmap_write(ctrl->map, RTL838X_SPCL_TRAP_IGMP_CTRL, 0x3);
	/* Flush learned FDB entries on link down of a port */
	regmap_set_bits(ctrl->map, RTL838X_L2_CTRL_0, BIT(7));
}

static void rteth_839x_hw_init(struct rteth_ctrl *ctrl)
{
	/* Trap MLD and IGMP messages to CPU_PORT */
	regmap_write(ctrl->map, RTL839X_SPCL_TRAP_IGMP_CTRL, 0x3);
	/* Flush learned FDB entries on link down of a port */
	regmap_set_bits(ctrl->map, RTL839X_L2_CTRL_0, BIT(7));
}

static void rteth_930x_hw_init(struct rteth_ctrl *ctrl)
{
	/* Trap MLD and IGMP messages to CPU_PORT */
	regmap_write(ctrl->map, RTL930X_VLAN_APP_PKT_CTRL, 0x12);
	/* Flush learned FDB entries on link down of a port */
	regmap_set_bits(ctrl->map, RTL930X_L2_CTRL, BIT(7));
}

static void rteth_931x_hw_init(struct rteth_ctrl *ctrl)
{
	/* Trap MLD and IGMP messages to CPU_PORT */
	regmap_write(ctrl->map, RTL931X_VLAN_APP_PKT_CTRL, 0x12);
	/* Set PCIE_PWR_DOWN */
	regmap_set_bits(ctrl->map, RTL931X_PS_SOC_CTRL, BIT(1));
}

static int rteth_open(struct net_device *ndev)
{
	unsigned long flags;
	struct rteth_ctrl *ctrl = netdev_priv(ndev);

	pr_debug("%s called: RX rings %d(length %d), TX rings %d(length %d)\n",
		 __func__, RTETH_RX_RINGS, RTETH_RX_RING_SIZE, RTETH_TX_RINGS, RTETH_TX_RING_SIZE);

	spin_lock_irqsave(&ctrl->lock, flags);
	ctrl->r->hw_reset(ctrl);
	rteth_setup_cpu_rx_rings(ctrl);
	rteth_setup_ring_buffer(ctrl);
	if (ctrl->r->setup_notify_ring_buffer)
		ctrl->r->setup_notify_ring_buffer(ctrl);

	rteth_hw_ring_setup(ctrl);
	phylink_start(ctrl->phylink);

	for (int i = 0; i < RTETH_RX_RINGS; i++)
		napi_enable(&ctrl->rx_qs[i].napi);

	ctrl->r->hw_init(ctrl);
	ctrl->r->hw_en_rxtx(ctrl);
	netif_tx_start_all_queues(ndev);
	spin_unlock_irqrestore(&ctrl->lock, flags);

	return 0;
}

static void rteth_838x_hw_stop(struct rteth_ctrl *ctrl)
{
	u32 val;

	/* Block all ports. TODO: this is an unprotected table access */
	regmap_write(ctrl->map, RTL838X_TBL_ACCESS_DATA_0(0), 0x3000000);
	regmap_write(ctrl->map, RTL838X_TBL_ACCESS_DATA_0(1), 0x0);
	regmap_write(ctrl->map, RTL838X_TBL_ACCESS_CTRL_0, 1 << 15 | 2 << 12);

	/* Disable FAST_AGE_OUT otherwise flush will hang */
	regmap_clear_bits(ctrl->map, RTL838X_L2_CTRL_1, BIT(23));

	/* Flush L2 address cache */
	for (int i = 0; i <= ctrl->r->cpu_port; i++) {
		regmap_write(ctrl->map, ctrl->r->l2_tbl_flush_ctrl, BIT(26) | BIT(23) | i << 5);
		regmap_read_poll_timeout(ctrl->map, ctrl->r->l2_tbl_flush_ctrl,
					 val, !(val & BIT(26)), 100, 100000);
	}

	/* CPU-Port: Link down */
	regmap_write(ctrl->map, ctrl->r->mac_force_mode_ctrl, 0x6192C);
}

static void rteth_839x_hw_stop(struct rteth_ctrl *ctrl)
{
	u32 val;

	/* Flush L2 address cache */
	for (int i = 0; i <= ctrl->r->cpu_port; i++) {
		regmap_write(ctrl->map, ctrl->r->l2_tbl_flush_ctrl, BIT(28) | BIT(25) | i << 5);
		regmap_read_poll_timeout(ctrl->map, ctrl->r->l2_tbl_flush_ctrl,
					 val, !(val & BIT(28)), 100, 100000);
	}

	regmap_write(ctrl->map, ctrl->r->mac_force_mode_ctrl, 0x75);
}

static void rteth_930x_hw_stop(struct rteth_ctrl *ctrl)
{
	/* TODO: L2 flush needed */

	/* CPU-Port: Link down */
	regmap_clear_bits(ctrl->map, ctrl->r->mac_force_mode_ctrl, 0x3);
}

static void rteth_931x_hw_stop(struct rteth_ctrl *ctrl)
{
	/* TODO: L2 flush needed */

	/* CPU-Port: Link down */
	regmap_clear_bits(ctrl->map, ctrl->r->mac_force_mode_ctrl, BIT(0) | BIT(9));
}

static void rteth_hw_stop(struct rteth_ctrl *ctrl)
{
	/* Disable RX/TX from/to CPU-port */
	regmap_clear_bits(ctrl->map, ctrl->r->mac_l2_port_ctrl, 0x3);

	/* Disable traffic */
	regmap_clear_bits(ctrl->map, ctrl->r->dma_if_ctrl, ctrl->r->tx_rx_enable);
	mdelay(200); /* Test, whether this is needed */

	/* family specific stop */
	ctrl->r->hw_stop(ctrl);
	mdelay(100);

	rteth_disable_all_irqs(ctrl);

	/* Disable TX/RX DMA */
	regmap_write(ctrl->map, ctrl->r->dma_if_ctrl, 0);
	mdelay(200);
}

static int rteth_stop(struct net_device *ndev)
{
	struct rteth_ctrl *ctrl = netdev_priv(ndev);

	pr_info("in %s\n", __func__);

	phylink_stop(ctrl->phylink);
	rteth_hw_stop(ctrl);

	for (int i = 0; i < RTETH_RX_RINGS; i++)
		napi_disable(&ctrl->rx_qs[i].napi);

	netif_tx_stop_all_queues(ndev);

	return 0;
}

static void rteth_838x_set_rx_mode(struct net_device *ndev)
{
	struct rteth_ctrl *ctrl = netdev_priv(ndev);

	/* Flood all classes of RMA addresses (01-80-C2-00-00-{01..2F})
	 * CTRL_0_FULL = GENMASK(21, 0) = 0x3FFFFF
	 */
	if (!(ndev->flags & (IFF_PROMISC | IFF_ALLMULTI))) {
		regmap_write(ctrl->map, RTETH_838X_RMA_CTRL_0, 0);
		regmap_write(ctrl->map, RTETH_838X_RMA_CTRL_1, 0);
	}
	if (ndev->flags & IFF_ALLMULTI)
		regmap_write(ctrl->map, RTETH_838X_RMA_CTRL_0, GENMASK(21, 0));
	if (ndev->flags & IFF_PROMISC) {
		regmap_write(ctrl->map, RTETH_838X_RMA_CTRL_0, GENMASK(21, 0));
		regmap_write(ctrl->map, RTETH_838X_RMA_CTRL_1, GENMASK(14, 0));
	}
}

static void rteth_839x_set_rx_mode(struct net_device *ndev)
{
	struct rteth_ctrl *ctrl = netdev_priv(ndev);

	/* Flood all classes of RMA addresses (01-80-C2-00-00-{01..2F})
	 * CTRL_0_FULL = GENMASK(31, 2) = 0xFFFFFFFC
	 * Lower two bits are reserved, corresponding to RMA 01-80-C2-00-00-00
	 * CTRL_1_FULL = CTRL_2_FULL = GENMASK(31, 0)
	 */
	if (!(ndev->flags & (IFF_PROMISC | IFF_ALLMULTI))) {
		regmap_write(ctrl->map, RTETH_839X_RMA_CTRL_0, 0);
		regmap_write(ctrl->map, RTETH_839X_RMA_CTRL_1, 0);
		regmap_write(ctrl->map, RTETH_839X_RMA_CTRL_2, 0);
		regmap_write(ctrl->map, RTETH_839X_RMA_CTRL_3, 0);
	}
	if (ndev->flags & IFF_ALLMULTI) {
		regmap_write(ctrl->map, RTETH_839X_RMA_CTRL_0, GENMASK(31, 2));
		regmap_write(ctrl->map, RTETH_839X_RMA_CTRL_1, GENMASK(31, 0));
		regmap_write(ctrl->map, RTETH_839X_RMA_CTRL_2, GENMASK(31, 0));
	}
	if (ndev->flags & IFF_PROMISC) {
		regmap_write(ctrl->map, RTETH_839X_RMA_CTRL_0, GENMASK(31, 2));
		regmap_write(ctrl->map, RTETH_839X_RMA_CTRL_1, GENMASK(31, 0));
		regmap_write(ctrl->map, RTETH_839X_RMA_CTRL_2, GENMASK(31, 0));
		regmap_write(ctrl->map, RTETH_839X_RMA_CTRL_3, GENMASK(9, 0));
	}
}

static void rteth_930x_set_rx_mode(struct net_device *ndev)
{
	struct rteth_ctrl *ctrl = netdev_priv(ndev);

	/* Flood all classes of RMA addresses (01-80-C2-00-00-{01..2F})
	 * CTRL_0_FULL = GENMASK(31, 2) = 0xFFFFFFFC
	 * Lower two bits are reserved, corresponding to RMA 01-80-C2-00-00-00
	 * CTRL_1_FULL = CTRL_2_FULL = GENMASK(31, 0)
	 */
	if (ndev->flags & (IFF_ALLMULTI | IFF_PROMISC)) {
		regmap_write(ctrl->map, RTETH_930X_RMA_CTRL_0, GENMASK(31, 2));
		regmap_write(ctrl->map, RTETH_930X_RMA_CTRL_1, GENMASK(31, 0));
		regmap_write(ctrl->map, RTETH_930X_RMA_CTRL_2, GENMASK(31, 0));
	} else {
		regmap_write(ctrl->map, RTETH_930X_RMA_CTRL_0, 0);
		regmap_write(ctrl->map, RTETH_930X_RMA_CTRL_1, 0);
		regmap_write(ctrl->map, RTETH_930X_RMA_CTRL_2, 0);
	}
}

static void rteth_931x_set_rx_mode(struct net_device *ndev)
{
	struct rteth_ctrl *ctrl = netdev_priv(ndev);

	/* Flood all classes of RMA addresses (01-80-C2-00-00-{01..2F})
	 * CTRL_0_FULL = GENMASK(31, 2) = 0xFFFFFFFC
	 * Lower two bits are reserved, corresponding to RMA 01-80-C2-00-00-00.
	 * CTRL_1_FULL = CTRL_2_FULL = GENMASK(31, 0)
	 */
	if (ndev->flags & (IFF_ALLMULTI | IFF_PROMISC)) {
		regmap_write(ctrl->map, RTETH_931X_RMA_CTRL_0, GENMASK(31, 2));
		regmap_write(ctrl->map, RTETH_931X_RMA_CTRL_1, GENMASK(31, 0));
		regmap_write(ctrl->map, RTETH_931X_RMA_CTRL_2, GENMASK(31, 0));
	} else {
		regmap_write(ctrl->map, RTETH_931X_RMA_CTRL_0, 0);
		regmap_write(ctrl->map, RTETH_931X_RMA_CTRL_1, 0);
		regmap_write(ctrl->map, RTETH_931X_RMA_CTRL_2, 0);
	}
}

static void rteth_tx_timeout(struct net_device *ndev, unsigned int txqueue)
{
	unsigned long flags;
	struct rteth_ctrl *ctrl = netdev_priv(ndev);

	pr_warn("%s\n", __func__);
	spin_lock_irqsave(&ctrl->lock, flags);
	rteth_hw_stop(ctrl);
	rteth_hw_ring_setup(ctrl);
	ctrl->r->hw_en_rxtx(ctrl);
	netif_trans_update(ndev);
	netif_start_queue(ndev);
	spin_unlock_irqrestore(&ctrl->lock, flags);
}

static int rteth_start_xmit(struct sk_buff *skb, struct net_device *netdev)
{
	struct rteth_ctrl *ctrl = netdev_priv(netdev);
	int val, slot, len = skb->len, dest_port = -1;
	int ring = skb_get_queue_mapping(skb);
	struct device *dev = &ctrl->pdev->dev;
	struct rteth_packet *packet;
	dma_addr_t packet_dma;

	if (netdev_uses_dsa(netdev) &&
	    skb->data[len - 4] == 0x80 &&
	    skb->data[len - 3] < ctrl->r->cpu_port &&
	    skb->data[len - 2] == 0x10 &&
	    skb->data[len - 1] == 0x00) {
		dest_port = skb->data[len - 3];
		/* space will be reused for 4 byte layer 2 FCS */
	} else {
		/* No DSA tag, add space for 4 byte layer 2 FCS */
		len += ETH_FCS_LEN;
	}

	len = max(ETH_ZLEN + ETH_FCS_LEN, len);
	if (unlikely(skb_put_padto(skb, len))) {
		netdev->stats.tx_errors++;
		dev_warn(dev, "skb pad failed\n");

		return NETDEV_TX_OK;
	}

	slot = ctrl->tx_data[ring].slot;
	packet = &ctrl->tx_data[ring].packet[slot];
	packet_dma = ctrl->tx_data[ring].ring[slot];

	if (unlikely(packet_dma & RTETH_OWN_CPU)) {
		netif_stop_subqueue(netdev, ring);
		if (net_ratelimit())
			dev_warn(dev, "tx ring %d busy, waiting for slot %d\n", ring, slot);

		return NETDEV_TX_BUSY;
	}

	packet->dma = dma_map_single(dev, skb->data, len, DMA_TO_DEVICE);
	if (unlikely(dma_mapping_error(dev, packet->dma))) {
		dev_kfree_skb_any(skb);
		netdev->stats.tx_errors++;

		return NETDEV_TX_OK;
	}

	if (likely(packet->skb)) {
		/* cleanup old data of this slot */
		dma_unmap_single(dev, packet->dma, packet->skb->len, DMA_TO_DEVICE);
		dev_kfree_skb_any(packet->skb);
	}

	if (dest_port >= 0)
		ctrl->r->create_tx_header(packet, dest_port, 0); // TODO ok to set prio to 0?

	/* Transfer data and hand packet over to switch */
	packet->len = len;
	packet->skb = skb;
	dma_wmb();
	ctrl->tx_data[ring].ring[slot] = packet_dma | RTETH_OWN_CPU;
	ctrl->tx_data[ring].slot = (slot + 1) % RTETH_TX_RING_SIZE;
	wmb();

	spin_lock(&ctrl->tx_lock);

	/*
	 * Issue send for 1 or 2 triggers. On some SoCs (especially RTL838x) there is a known
	 * bug, where the hardware sometimes reads empty values from the register. Work around
	 * that with a poll that checks if TX/RX is enabled in the register.
	 */
	if (regmap_read_poll_timeout(ctrl->map, ctrl->r->dma_if_ctrl,
				     val, val & ctrl->r->tx_rx_enable, 0, 5000))
		dev_warn_once(dev, "DMA interface ctrl register read failed\n");

	regmap_write(ctrl->map, ctrl->r->dma_if_ctrl, val | RTETH_TX_TRIGGER(ctrl, ring));

	netdev->stats.tx_packets++;
	netdev->stats.tx_bytes += len;

	spin_unlock(&ctrl->tx_lock);

	return NETDEV_TX_OK;
}

static int rteth_hw_receive(struct net_device *dev, int ring, int budget)
{
	int slot, len, work_done = 0, rx_packets = 0, rx_bytes = 0;
	struct rteth_ctrl *ctrl = netdev_priv(dev);
	bool dsa = netdev_uses_dsa(dev);
	struct rteth_packet *packet;
	dma_addr_t packet_dma;
	struct sk_buff *skb;
	struct dsa_tag tag;

	while (work_done < budget) {
		slot = ctrl->rx_data[ring].slot;
		packet_dma = ctrl->rx_data[ring].ring[slot];
		rmb();

		if (packet_dma & RTETH_OWN_CPU)
			break;

		packet = &ctrl->rx_data[ring].packet[slot];
		len = packet->len;

		if (len < ETH_FCS_LEN || len > RING_BUFFER) {
			netdev_err(dev, "invalid packet with %d bytes received\n", len);
			break;
		} else if (!dsa) {
			len -= ETH_FCS_LEN;
		}

		skb = netdev_alloc_skb_ip_align(dev, len);
		if (unlikely(!skb)) {
			netdev_warn(dev, "low memory, packet dropped\n");
			dev->stats.rx_dropped++;
		} else {
			dma_sync_single_for_cpu(&ctrl->pdev->dev, packet->dma, len, DMA_FROM_DEVICE);
			dma_rmb();
			skb_put_data(skb, packet->buf, len);

			if (dsa) {
				ctrl->r->decode_tag(packet, &tag);
				skb->data[len - 4] = 0x80;
				skb->data[len - 3] = tag.port;
				skb->data[len - 2] = 0x10;
				skb->data[len - 1] = 0x00;
				if (tag.l2_offloaded)
					skb->data[len - 3] |= 0x40;
			}

			skb->protocol = eth_type_trans(skb, dev);
			if (dev->features & NETIF_F_RXCSUM) {
				if (tag.crc_error)
					skb_checksum_none_assert(skb);
				else
					skb->ip_summed = CHECKSUM_UNNECESSARY;
			}

			rx_packets++;
			rx_bytes += len;
			napi_gro_receive(&ctrl->rx_qs[ring].napi, skb);
		}

		ctrl->rx_data[ring].ring[slot] = packet_dma | RTETH_OWN_CPU;
		ctrl->rx_data[ring].slot = (slot + 1) % RTETH_RX_RING_SIZE;
		work_done++;
	}

	spin_lock(&ctrl->rx_lock);
	ctrl->r->update_counter(ctrl, ring, work_done);
	dev->stats.rx_packets += rx_packets;
	dev->stats.rx_bytes += rx_bytes;
	spin_unlock(&ctrl->rx_lock);

	return work_done;
}

static int rteth_poll_rx(struct napi_struct *napi, int budget)
{
	struct rtl838x_rx_q *rx_q = container_of(napi, struct rtl838x_rx_q, napi);
	struct rteth_ctrl *ctrl = rx_q->ctrl;
	int work_done, ring = rx_q->id;

	work_done = rteth_hw_receive(ctrl->netdev, ring, budget);
	if (work_done < budget && napi_complete_done(napi, work_done))
		rteth_reenable_irq(ctrl, ring);

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

static void rteth_mac_link_down(struct phylink_config *config,
				unsigned int mode,
				phy_interface_t interface)
{
	struct net_device *dev = container_of(config->dev, struct net_device, dev);
	struct rteth_ctrl *ctrl = netdev_priv(dev);

	pr_debug("In %s\n", __func__);
	/* Stop TX/RX to port */
	regmap_clear_bits(ctrl->map, ctrl->r->mac_l2_port_ctrl, 0x3);
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
	regmap_set_bits(ctrl->map, ctrl->r->mac_l2_port_ctrl, 0x3);
}

static void rteth_set_mac_hw(struct net_device *dev, u8 *mac)
{
	u32 mac_lo = (mac[2] << 24) | (mac[3] << 16) | (mac[4] << 8) | mac[5];
	u32 mac_hi = (mac[0] << 8) | mac[1];
	struct rteth_ctrl *ctrl;
	unsigned long flags;

	ctrl = netdev_priv(dev);
	spin_lock_irqsave(&ctrl->lock, flags);

	for (int i = 0; i < RTETH_MAX_MAC_REGS; i++)
		if (ctrl->r->mac_reg[i]) {
			regmap_write(ctrl->map, ctrl->r->mac_reg[i], mac_hi);
			regmap_write(ctrl->map, ctrl->r->mac_reg[i] + 4, mac_lo);
		}

	spin_unlock_irqrestore(&ctrl->lock, flags);
}

static int rteth_set_mac_address(struct net_device *dev, void *p)
{
	const struct sockaddr *addr = p;
	u8 *mac = (u8 *)(addr->sa_data);

	if (!is_valid_ether_addr(addr->sa_data))
		return -EADDRNOTAVAIL;

	dev_addr_set(dev, addr->sa_data);
	rteth_set_mac_hw(dev, mac);

	pr_info("Using MAC %pM\n", dev->dev_addr);

	return 0;
}

static int rteth_838x_init_mac(struct rteth_ctrl *ctrl)
{
	pr_info("%s\n", __func__);
	/* fix timer for EEE */
	regmap_write(ctrl->map, RTL838X_EEE_TX_TIMER_GIGA_CTRL, 0x5001411);
	regmap_write(ctrl->map, RTL838X_EEE_TX_TIMER_GELITE_CTRL, 0x5001417);

	/* Init VLAN. TODO: Understand what is being done, here */
	for (int i = 0; i <= 28; i++)
		regmap_write(ctrl->map, 0xd57c + i * 0x80, 0);

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
	struct device *dev = &ctrl->pdev->dev;
	unsigned int val;
	int ret;

	/* Initialize Encapsulation memory and wait until finished */
	regmap_write(ctrl->map, RTL931X_MEM_ENCAP_INIT, 0x1);
	ret = regmap_read_poll_timeout(ctrl->map, RTL931X_MEM_ENCAP_INIT,
				       val, !(val & 1), 0, 100000);
	if (ret)
		dev_err(dev, "ENCAP init timeout\n");

	/* Initialize Management Information Base memory and wait until finished */
	regmap_write(ctrl->map, RTL931X_MEM_MIB_INIT, 0x1);
	ret = regmap_read_poll_timeout(ctrl->map, RTL931X_MEM_MIB_INIT,
				       val, !(val & 1), 0, 100000);
	if (ret)
		dev_err(dev, "MIB init timeout\n");

	/* Initialize ACL (PIE) memory and wait until finished */
	regmap_write(ctrl->map, RTL931X_MEM_ACL_INIT, 0x1);
	ret = regmap_read_poll_timeout(ctrl->map, RTL931X_MEM_ACL_INIT,
				       val, !(val & 1), 0, 100000);
	if (ret)
		dev_err(dev, "ACL init timeout\n");

	/* Initialize ALE memory and wait until finished */
	regmap_write(ctrl->map, RTL931X_MEM_ALE_INIT_0, 0xffffffff);
	ret = regmap_read_poll_timeout(ctrl->map, RTL931X_MEM_ALE_INIT_0,
				       val, !val, 0, 100000);
	if (ret)
		dev_err(dev, "ALE_0 init timeout\n");

	regmap_write(ctrl->map, RTL931X_MEM_ALE_INIT_1, 0x7f);
	ret = regmap_read_poll_timeout(ctrl->map, RTL931X_MEM_ALE_INIT_1,
				       val, !val, 0, 100000);
	if (ret)
		dev_err(dev, "ALE_1 init timeout\n");

	regmap_write(ctrl->map, RTL931X_MEM_ALE_INIT_2, 0x7ff);
	ret = regmap_read_poll_timeout(ctrl->map, RTL931X_MEM_ALE_INIT_2,
				       val, !val, 0, 100000);
	if (ret)
		dev_err(dev, "ALE_2 init timeout\n");

	/* Enable ESD auto recovery */
	regmap_write(ctrl->map, RTL931X_MDX_CTRL_RSVD, 0x1);

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

	if ((features ^ dev->features) & NETIF_F_RXCSUM)
		regmap_assign_bits(ctrl->map, ctrl->r->mac_l2_port_ctrl, BIT(3), features & NETIF_F_RXCSUM);

	return 0;
}

static int rteth_93xx_set_features(struct net_device *dev, netdev_features_t features)
{
	struct rteth_ctrl *ctrl = netdev_priv(dev);

	if ((features ^ dev->features) & NETIF_F_RXCSUM)
		regmap_assign_bits(ctrl->map, ctrl->r->mac_l2_port_ctrl, BIT(4), features & NETIF_F_RXCSUM);

	return 0;
}

static int rteth_setup_tc(struct net_device *dev, enum tc_setup_type type, void *type_data)
{
    struct dsa_switch *ds;
    struct dsa_port *dp;

    if (!netdev_uses_dsa(dev))
        return -EOPNOTSUPP;

    dp = dev->dsa_ptr;
    ds = dp->ds;

    if (!ds->ops->port_setup_tc)
        return -EOPNOTSUPP;

    return ds->ops->port_setup_tc(ds, dp->index, type, type_data);
}

static const struct net_device_ops rteth_838x_netdev_ops = {
	.ndo_open = rteth_open,
	.ndo_stop = rteth_stop,
	.ndo_start_xmit = rteth_start_xmit,
	.ndo_set_mac_address = rteth_set_mac_address,
	.ndo_validate_addr = eth_validate_addr,
	.ndo_set_rx_mode = rteth_838x_set_rx_mode,
	.ndo_tx_timeout = rteth_tx_timeout,
	.ndo_set_features = rteth_83xx_set_features,
	.ndo_fix_features = rteth_fix_features,
	.ndo_setup_tc = rteth_setup_tc,
};

static const struct rteth_config rteth_838x_cfg = {
	.cpu_port = RTETH_838X_CPU_PORT,
	.rx_rings = 8,
	.tx_rx_enable = 0xc,
	.tx_trigger_mask = BIT(1),
	.mac_l2_port_ctrl = RTETH_838X_MAC_L2_PORT_CTRL,
	.qm_pkt2cpu_intpri_map = RTETH_838X_QM_PKT2CPU_INTPRI_MAP,
	.qm_rsn2cpuqid_ctrl = RTETH_838X_QM_PKT2CPU_INTPRI_0,
	.qm_rsn2cpuqid_cnt = RTETH_838X_QM_PKT2CPU_INTPRI_CNT,
	.dma_if_ctrl = RTETH_838X_DMA_IF_CTRL,
	.dma_if_intr_sts = RTETH_838X_DMA_IF_INTR_STS,
	.dma_if_intr_msk = RTETH_838X_DMA_IF_INTR_MSK,
	.dma_if_rx_ring_cntr = RTETH_838X_DMA_IF_RX_RING_CNTR,
	.dma_if_rx_ring_size = RTETH_838X_DMA_IF_RX_RING_SIZE,
	.dma_rx_base = RTETH_838X_DMA_RX_BASE,
	.dma_tx_base = RTETH_838X_DMA_TX_BASE,
	.mac_force_mode_ctrl = RTETH_838X_MAC_FORCE_MODE_CTRL,
	.rst_glb_ctrl = RTL838X_RST_GLB_CTRL_0,
	.mac_reg = { RTETH_838X_MAC_ADDR_CTRL,
		     RTETH_838X_MAC_ADDR_CTRL_ALE,
		     RTETH_838X_MAC_ADDR_CTRL_MAC },
	.l2_tbl_flush_ctrl = RTL838X_L2_TBL_FLUSH_CTRL,
	.update_counter = rteth_83xx_update_counter,
	.create_tx_header = rteth_838x_create_tx_header,
	.decode_tag = rteth_838x_decode_tag,
	.hw_en_rxtx = rteth_838x_hw_en_rxtx,
	.hw_init = &rteth_838x_hw_init,
	.hw_stop = &rteth_838x_hw_stop,
	.hw_reset = &rteth_838x_hw_reset,
	.init_mac = &rteth_838x_init_mac,
	.netdev_ops = &rteth_838x_netdev_ops,
};

static const struct net_device_ops rteth_839x_netdev_ops = {
	.ndo_open = rteth_open,
	.ndo_stop = rteth_stop,
	.ndo_start_xmit = rteth_start_xmit,
	.ndo_set_mac_address = rteth_set_mac_address,
	.ndo_validate_addr = eth_validate_addr,
	.ndo_set_rx_mode = rteth_839x_set_rx_mode,
	.ndo_tx_timeout = rteth_tx_timeout,
	.ndo_set_features = rteth_83xx_set_features,
	.ndo_fix_features = rteth_fix_features,
	.ndo_setup_tc = rteth_setup_tc,
};

static const struct rteth_config rteth_839x_cfg = {
	.cpu_port = RTETH_839X_CPU_PORT,
	.rx_rings = 8,
	.tx_rx_enable = 0xc,
	.tx_trigger_mask = BIT(1),
	.mac_l2_port_ctrl = RTETH_839X_MAC_L2_PORT_CTRL,
	.qm_pkt2cpu_intpri_map = RTETH_839X_QM_PKT2CPU_INTPRI_MAP,
	.qm_rsn2cpuqid_ctrl = RTETH_839X_QM_PKT2CPU_INTPRI_0,
	.qm_rsn2cpuqid_cnt = RTETH_839X_QM_PKT2CPU_INTPRI_CNT,
	.dma_if_ctrl = RTETH_839X_DMA_IF_CTRL,
	.dma_if_intr_sts = RTETH_839X_DMA_IF_INTR_STS,
	.dma_if_intr_msk = RTETH_839X_DMA_IF_INTR_MSK,
	.dma_if_rx_ring_cntr = RTETH_839X_DMA_IF_RX_RING_CNTR,
	.dma_if_rx_ring_size = RTETH_839X_DMA_IF_RX_RING_SIZE,
	.dma_rx_base = RTETH_839X_DMA_RX_BASE,
	.dma_tx_base = RTETH_839X_DMA_TX_BASE,
	.mac_force_mode_ctrl = RTETH_839X_MAC_FORCE_MODE_CTRL,
	.rst_glb_ctrl = RTL839X_RST_GLB_CTRL,
	.mac_reg = { RTETH_839X_MAC_ADDR_CTRL },
	.l2_tbl_flush_ctrl = RTL839X_L2_TBL_FLUSH_CTRL,
	.update_counter = rteth_83xx_update_counter,
	.create_tx_header = rteth_839x_create_tx_header,
	.decode_tag = rteth_839x_decode_tag,
	.hw_en_rxtx = rteth_839x_hw_en_rxtx,
	.hw_init = &rteth_839x_hw_init,
	.hw_stop = &rteth_839x_hw_stop,
	.hw_reset = &rteth_839x_hw_reset,
	.init_mac = &rteth_839x_init_mac,
	.setup_notify_ring_buffer = &rteth_839x_setup_notify_ring_buffer,
	.netdev_ops = &rteth_839x_netdev_ops,
};

static const struct net_device_ops rteth_930x_netdev_ops = {
	.ndo_open = rteth_open,
	.ndo_stop = rteth_stop,
	.ndo_start_xmit = rteth_start_xmit,
	.ndo_set_mac_address = rteth_set_mac_address,
	.ndo_validate_addr = eth_validate_addr,
	.ndo_set_rx_mode = rteth_930x_set_rx_mode,
	.ndo_tx_timeout = rteth_tx_timeout,
	.ndo_set_features = rteth_93xx_set_features,
	.ndo_fix_features = rteth_fix_features,
	.ndo_setup_tc = rteth_setup_tc,
};

static const struct rteth_config rteth_930x_cfg = {
	.cpu_port = RTETH_930X_CPU_PORT,
	.rx_rings = 32,
	.tx_rx_enable = 0x30,
	.tx_trigger_mask = GENMASK(3, 2),
	.mac_l2_port_ctrl = RTETH_930X_MAC_L2_PORT_CTRL,
	.qm_rsn2cpuqid_ctrl = RTETH_930X_QM_RSN2CPUQID_CTRL_0,
	.qm_rsn2cpuqid_cnt = RTETH_930X_QM_RSN2CPUQID_CTRL_CNT,
	.dma_if_ctrl = RTETH_930X_DMA_IF_CTRL,
	.dma_if_intr_sts = RTETH_930X_DMA_IF_INTR_STS,
	.dma_if_intr_msk = RTETH_930X_DMA_IF_INTR_MSK,
	.dma_if_rx_ring_cntr = RTETH_930X_DMA_IF_RX_RING_CNTR,
	.dma_if_rx_ring_size = RTETH_930X_DMA_IF_RX_RING_SIZE,
	.dma_rx_base = RTETH_930X_DMA_RX_BASE,
	.dma_tx_base = RTETH_930X_DMA_TX_BASE,
	.l2_ntfy_if_intr_sts = RTL930X_L2_NTFY_IF_INTR_STS,
	.l2_ntfy_if_intr_msk = RTL930X_L2_NTFY_IF_INTR_MSK,
	.mac_force_mode_ctrl = RTETH_930X_MAC_FORCE_MODE_CTRL,
	.rst_glb_ctrl = RTL930X_RST_GLB_CTRL_0,
	.mac_reg = { RTETH_930X_MAC_L2_ADDR_CTRL },
	.l2_tbl_flush_ctrl = RTL930X_L2_TBL_FLUSH_CTRL,
	.update_counter = rteth_93xx_update_counter,
	.create_tx_header = rteth_93xx_create_tx_header,
	.decode_tag = rteth_93xx_decode_tag,
	.hw_en_rxtx = rteth_930x_hw_en_rxtx,
	.hw_init = &rteth_930x_hw_init,
	.hw_stop = &rteth_930x_hw_stop,
	.hw_reset = &rteth_93xx_hw_reset,
	.init_mac = &rteth_930x_init_mac,
	.netdev_ops = &rteth_930x_netdev_ops,
};

static const struct net_device_ops rteth_931x_netdev_ops = {
	.ndo_open = rteth_open,
	.ndo_stop = rteth_stop,
	.ndo_start_xmit = rteth_start_xmit,
	.ndo_set_mac_address = rteth_set_mac_address,
	.ndo_validate_addr = eth_validate_addr,
	.ndo_set_rx_mode = rteth_931x_set_rx_mode,
	.ndo_tx_timeout = rteth_tx_timeout,
	.ndo_set_features = rteth_93xx_set_features,
	.ndo_fix_features = rteth_fix_features,
	.ndo_setup_tc = rteth_setup_tc,
};

static const struct rteth_config rteth_931x_cfg = {
	.cpu_port = RTETH_931X_CPU_PORT,
	.rx_rings = 32,
	.tx_rx_enable = 0x30,
	.tx_trigger_mask = GENMASK(3, 2),
	.mac_l2_port_ctrl = RTETH_931X_MAC_L2_PORT_CTRL,
	.qm_rsn2cpuqid_ctrl = RTETH_931X_QM_RSN2CPUQID_CTRL_0,
	.qm_rsn2cpuqid_cnt = RTETH_931X_QM_RSN2CPUQID_CTRL_CNT,
	.dma_if_ctrl = RTETH_931X_DMA_IF_CTRL,
	.dma_if_intr_sts = RTETH_931X_DMA_IF_INTR_STS,
	.dma_if_intr_msk = RTETH_931X_DMA_IF_INTR_MSK,
	.dma_if_rx_ring_cntr = RTETH_931X_DMA_IF_RX_RING_CNTR,
	.dma_if_rx_ring_size = RTETH_931X_DMA_IF_RX_RING_SIZE,
	.dma_rx_base = RTETH_931X_DMA_RX_BASE,
	.dma_tx_base = RTETH_931X_DMA_TX_BASE,
	.l2_ntfy_if_intr_sts = RTL931X_L2_NTFY_IF_INTR_STS,
	.l2_ntfy_if_intr_msk = RTL931X_L2_NTFY_IF_INTR_MSK,
	.mac_force_mode_ctrl = RTETH_931X_MAC_FORCE_MODE_CTRL,
	.rst_glb_ctrl = RTL931X_RST_GLB_CTRL,
	.mac_reg = { RTETH_930X_MAC_L2_ADDR_CTRL },
	.l2_tbl_flush_ctrl = RTL931X_L2_TBL_FLUSH_CTRL,
	.update_counter = rteth_93xx_update_counter,
	.create_tx_header = rteth_93xx_create_tx_header,
	.decode_tag = rteth_93xx_decode_tag,
	.hw_en_rxtx = rteth_931x_hw_en_rxtx,
	.hw_init = &rteth_931x_hw_init,
	.hw_stop = &rteth_931x_hw_stop,
	.hw_reset = &rteth_93xx_hw_reset,
	.init_mac = &rteth_931x_init_mac,
	.netdev_ops = &rteth_931x_netdev_ops,
};

static const struct phylink_mac_ops rteth_mac_ops = {
	.mac_config = rteth_mac_config,
	.mac_link_down = rteth_mac_link_down,
	.mac_link_up = rteth_mac_link_up,
};

static const struct ethtool_ops rteth_ethtool_ops = {
	.get_link_ksettings = rteth_get_link_ksettings,
	.set_link_ksettings = rteth_set_link_ksettings,
};

static int rteth_probe(struct platform_device *pdev)
{
	struct net_device *dev;
	struct device_node *dn = pdev->dev.of_node;
	struct rteth_ctrl *ctrl;
	const struct rteth_config *cfg;
	phy_interface_t phy_mode;
	struct phylink *phylink;
	u8 mac_addr[ETH_ALEN] = {0};
	int err = 0;

	pr_info("Probing RTL838X eth device pdev: %x, dev: %x\n",
		(u32)pdev, (u32)(&pdev->dev));

	cfg = device_get_match_data(&pdev->dev);

	dev = devm_alloc_etherdev_mqs(&pdev->dev, sizeof(struct rteth_ctrl), RTETH_TX_RINGS, RTETH_RX_RINGS);
	if (!dev)
		return -ENOMEM;
	SET_NETDEV_DEV(dev, &pdev->dev);
	ctrl = netdev_priv(dev);
	ctrl->r = cfg;
	ctrl->map = syscon_node_to_regmap(dn->parent);
	if (IS_ERR(ctrl->map))
		return PTR_ERR(ctrl->map);

	/* Allocate buffer memory */
	ctrl->membase = dmam_alloc_coherent(&pdev->dev, sizeof(struct notify_b),
					    (void *)&dev->mem_start, GFP_KERNEL);
	if (!ctrl->membase) {
		dev_err(&pdev->dev, "cannot allocate DMA buffer\n");
		return -ENOMEM;
	}

	ctrl->rx_buf = dma_alloc_noncoherent(&pdev->dev,
					     RTETH_RX_RINGS * RTETH_RX_RING_SIZE * RING_BUFFER,
					     &ctrl->rx_buf_dma, DMA_FROM_DEVICE, GFP_KERNEL);
	ctrl->rx_data = dmam_alloc_coherent(&pdev->dev, sizeof(struct rteth_rx) * RTETH_RX_RINGS,
					    &ctrl->rx_data_dma, GFP_KERNEL);
	ctrl->tx_data = dmam_alloc_coherent(&pdev->dev, sizeof(struct rteth_tx) * RTETH_TX_RINGS,
					    &ctrl->tx_dma, GFP_KERNEL);

	spin_lock_init(&ctrl->lock);
	spin_lock_init(&ctrl->rx_lock);
	spin_lock_init(&ctrl->tx_lock);

	dev->ethtool_ops = &rteth_ethtool_ops;
	dev->min_mtu = ETH_ZLEN;
	dev->max_mtu = DEFAULT_MTU;
	dev->features = NETIF_F_RXCSUM;
	dev->hw_features = NETIF_F_RXCSUM;
	dev->netdev_ops = ctrl->r->netdev_ops;

	/* Obtain device IRQ number */
	dev->irq = platform_get_irq(pdev, 0);
	if (dev->irq < 0)
		return -ENODEV;

	rteth_disable_all_irqs(ctrl);
	err = devm_request_irq(&pdev->dev, dev->irq, rteth_net_irq, IRQF_SHARED, dev->name, dev);
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
		u32 mac_hi, mac_lo;

		regmap_read(ctrl->map, ctrl->r->mac_reg[0], &mac_hi);
		regmap_read(ctrl->map, ctrl->r->mac_reg[0] + 4, &mac_lo);

		mac_addr[0] = (mac_hi >> 8) & 0xff;
		mac_addr[1] = mac_hi & 0xff;
		mac_addr[2] = (mac_lo >> 24) & 0xff;
		mac_addr[3] = (mac_lo >> 16) & 0xff;
		mac_addr[4] = (mac_lo >> 8) & 0xff;
		mac_addr[5] = mac_lo & 0xff;
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
	pr_info("Using MAC %pM\n", dev->dev_addr);
	strscpy(dev->name, "eth%d", sizeof(dev->name));

	ctrl->pdev = pdev;
	ctrl->netdev = dev;

	for (int i = 0; i < RTETH_RX_RINGS; i++) {
		ctrl->rx_qs[i].id = i;
		ctrl->rx_qs[i].ctrl = ctrl;
		netif_napi_add(dev, &ctrl->rx_qs[i].napi, rteth_poll_rx);
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

static void rteth_remove(struct platform_device *pdev)
{
	struct net_device *dev = platform_get_drvdata(pdev);
	struct rteth_ctrl *ctrl = netdev_priv(dev);

	pr_info("Removing platform driver for rtl838x-eth\n");
	rteth_hw_stop(ctrl);

	netif_tx_stop_all_queues(dev);

	for (int i = 0; i < RTETH_RX_RINGS; i++)
		netif_napi_del(&ctrl->rx_qs[i].napi);
}

static const struct of_device_id rteth_of_ids[] = {
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
MODULE_DEVICE_TABLE(of, rteth_of_ids);

static struct platform_driver rtl838x_eth_driver = {
	.probe  = rteth_probe,
	.remove = rteth_remove,
	.driver = {
		.name = KBUILD_MODNAME,
		.pm = NULL,
		.of_match_table = rteth_of_ids,
	},
};

module_platform_driver(rtl838x_eth_driver);

MODULE_AUTHOR("B. Koblitz");
MODULE_DESCRIPTION("RTL838X SoC Ethernet Driver");
MODULE_LICENSE("GPL");

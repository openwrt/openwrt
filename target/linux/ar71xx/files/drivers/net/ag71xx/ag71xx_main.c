/*
 *  Atheros AR71xx built-in ethernet mac driver
 *
 *  Copyright (C) 2008-2010 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 *  Based on Atheros' AG7100 driver
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include "ag71xx.h"

#define AG71XX_DEFAULT_MSG_ENABLE	\
	(NETIF_MSG_DRV			\
	| NETIF_MSG_PROBE		\
	| NETIF_MSG_LINK		\
	| NETIF_MSG_TIMER		\
	| NETIF_MSG_IFDOWN		\
	| NETIF_MSG_IFUP		\
	| NETIF_MSG_RX_ERR		\
	| NETIF_MSG_TX_ERR)

static int ag71xx_msg_level = -1;

module_param_named(msg_level, ag71xx_msg_level, int, 0);
MODULE_PARM_DESC(msg_level, "Message level (-1=defaults,0=none,...,16=all)");

static void ag71xx_dump_dma_regs(struct ag71xx *ag)
{
	DBG("%s: dma_tx_ctrl=%08x, dma_tx_desc=%08x, dma_tx_status=%08x\n",
		ag->dev->name,
		ag71xx_rr(ag, AG71XX_REG_TX_CTRL),
		ag71xx_rr(ag, AG71XX_REG_TX_DESC),
		ag71xx_rr(ag, AG71XX_REG_TX_STATUS));

	DBG("%s: dma_rx_ctrl=%08x, dma_rx_desc=%08x, dma_rx_status=%08x\n",
		ag->dev->name,
		ag71xx_rr(ag, AG71XX_REG_RX_CTRL),
		ag71xx_rr(ag, AG71XX_REG_RX_DESC),
		ag71xx_rr(ag, AG71XX_REG_RX_STATUS));
}

static void ag71xx_dump_regs(struct ag71xx *ag)
{
	DBG("%s: mac_cfg1=%08x, mac_cfg2=%08x, ipg=%08x, hdx=%08x, mfl=%08x\n",
		ag->dev->name,
		ag71xx_rr(ag, AG71XX_REG_MAC_CFG1),
		ag71xx_rr(ag, AG71XX_REG_MAC_CFG2),
		ag71xx_rr(ag, AG71XX_REG_MAC_IPG),
		ag71xx_rr(ag, AG71XX_REG_MAC_HDX),
		ag71xx_rr(ag, AG71XX_REG_MAC_MFL));
	DBG("%s: mac_ifctl=%08x, mac_addr1=%08x, mac_addr2=%08x\n",
		ag->dev->name,
		ag71xx_rr(ag, AG71XX_REG_MAC_IFCTL),
		ag71xx_rr(ag, AG71XX_REG_MAC_ADDR1),
		ag71xx_rr(ag, AG71XX_REG_MAC_ADDR2));
	DBG("%s: fifo_cfg0=%08x, fifo_cfg1=%08x, fifo_cfg2=%08x\n",
		ag->dev->name,
		ag71xx_rr(ag, AG71XX_REG_FIFO_CFG0),
		ag71xx_rr(ag, AG71XX_REG_FIFO_CFG1),
		ag71xx_rr(ag, AG71XX_REG_FIFO_CFG2));
	DBG("%s: fifo_cfg3=%08x, fifo_cfg4=%08x, fifo_cfg5=%08x\n",
		ag->dev->name,
		ag71xx_rr(ag, AG71XX_REG_FIFO_CFG3),
		ag71xx_rr(ag, AG71XX_REG_FIFO_CFG4),
		ag71xx_rr(ag, AG71XX_REG_FIFO_CFG5));
}

static inline void ag71xx_dump_intr(struct ag71xx *ag, char *label, u32 intr)
{
	DBG("%s: %s intr=%08x %s%s%s%s%s%s\n",
		ag->dev->name, label, intr,
		(intr & AG71XX_INT_TX_PS) ? "TXPS " : "",
		(intr & AG71XX_INT_TX_UR) ? "TXUR " : "",
		(intr & AG71XX_INT_TX_BE) ? "TXBE " : "",
		(intr & AG71XX_INT_RX_PR) ? "RXPR " : "",
		(intr & AG71XX_INT_RX_OF) ? "RXOF " : "",
		(intr & AG71XX_INT_RX_BE) ? "RXBE " : "");
}

static void ag71xx_ring_free(struct ag71xx_ring *ring)
{
	kfree(ring->buf);

	if (ring->descs_cpu)
		dma_free_coherent(NULL, ring->size * ring->desc_size,
				  ring->descs_cpu, ring->descs_dma);
}

static int ag71xx_ring_alloc(struct ag71xx_ring *ring, unsigned int size)
{
	int err;
	int i;

	ring->desc_size = sizeof(struct ag71xx_desc);
	if (ring->desc_size % cache_line_size()) {
		DBG("ag71xx: ring %p, desc size %u rounded to %u\n",
			ring, ring->desc_size,
			roundup(ring->desc_size, cache_line_size()));
		ring->desc_size = roundup(ring->desc_size, cache_line_size());
	}

	ring->descs_cpu = dma_alloc_coherent(NULL, size * ring->desc_size,
					     &ring->descs_dma, GFP_ATOMIC);
	if (!ring->descs_cpu) {
		err = -ENOMEM;
		goto err;
	}

	ring->size = size;

	ring->buf = kzalloc(size * sizeof(*ring->buf), GFP_KERNEL);
	if (!ring->buf) {
		err = -ENOMEM;
		goto err;
	}

	for (i = 0; i < size; i++) {
		int idx = i * ring->desc_size;
		ring->buf[i].desc = (struct ag71xx_desc *)&ring->descs_cpu[idx];
		DBG("ag71xx: ring %p, desc %d at %p\n",
			ring, i, ring->buf[i].desc);
	}

	return 0;

err:
	return err;
}

static void ag71xx_ring_tx_clean(struct ag71xx *ag)
{
	struct ag71xx_ring *ring = &ag->tx_ring;
	struct net_device *dev = ag->dev;

	while (ring->curr != ring->dirty) {
		u32 i = ring->dirty % AG71XX_TX_RING_SIZE;

		if (!ag71xx_desc_empty(ring->buf[i].desc)) {
			ring->buf[i].desc->ctrl = 0;
			dev->stats.tx_errors++;
		}

		if (ring->buf[i].skb)
			dev_kfree_skb_any(ring->buf[i].skb);

		ring->buf[i].skb = NULL;

		ring->dirty++;
	}

	/* flush descriptors */
	wmb();

}

static void ag71xx_ring_tx_init(struct ag71xx *ag)
{
	struct ag71xx_ring *ring = &ag->tx_ring;
	int i;

	for (i = 0; i < AG71XX_TX_RING_SIZE; i++) {
		ring->buf[i].desc->next = (u32) (ring->descs_dma +
			ring->desc_size * ((i + 1) % AG71XX_TX_RING_SIZE));

		ring->buf[i].desc->ctrl = DESC_EMPTY;
		ring->buf[i].skb = NULL;
	}

	/* flush descriptors */
	wmb();

	ring->curr = 0;
	ring->dirty = 0;
}

static void ag71xx_ring_rx_clean(struct ag71xx *ag)
{
	struct ag71xx_ring *ring = &ag->rx_ring;
	int i;

	if (!ring->buf)
		return;

	for (i = 0; i < AG71XX_RX_RING_SIZE; i++)
		if (ring->buf[i].skb) {
			dma_unmap_single(&ag->dev->dev, ring->buf[i].dma_addr,
					 AG71XX_RX_PKT_SIZE, DMA_FROM_DEVICE);
			kfree_skb(ring->buf[i].skb);
		}
}

static int ag71xx_rx_reserve(struct ag71xx *ag)
{
	int reserve = 0;

	if (ag71xx_get_pdata(ag)->is_ar724x) {
		if (!ag71xx_has_ar8216(ag))
			reserve = 2;

		if (ag->phy_dev)
			reserve += 4 - (ag->phy_dev->pkt_align % 4);

		reserve %= 4;
	}

	return reserve + AG71XX_RX_PKT_RESERVE;
}


static int ag71xx_ring_rx_init(struct ag71xx *ag)
{
	struct ag71xx_ring *ring = &ag->rx_ring;
	unsigned int reserve = ag71xx_rx_reserve(ag);
	unsigned int i;
	int ret;

	ret = 0;
	for (i = 0; i < AG71XX_RX_RING_SIZE; i++) {
		ring->buf[i].desc->next = (u32) (ring->descs_dma +
			ring->desc_size * ((i + 1) % AG71XX_RX_RING_SIZE));

		DBG("ag71xx: RX desc at %p, next is %08x\n",
			ring->buf[i].desc,
			ring->buf[i].desc->next);
	}

	for (i = 0; i < AG71XX_RX_RING_SIZE; i++) {
		struct sk_buff *skb;
		dma_addr_t dma_addr;

		skb = dev_alloc_skb(AG71XX_RX_PKT_SIZE + reserve);
		if (!skb) {
			ret = -ENOMEM;
			break;
		}

		skb->dev = ag->dev;
		skb_reserve(skb, reserve);

		dma_addr = dma_map_single(&ag->dev->dev, skb->data,
					  AG71XX_RX_PKT_SIZE,
					  DMA_FROM_DEVICE);
		ring->buf[i].skb = skb;
		ring->buf[i].dma_addr = dma_addr;
		ring->buf[i].desc->data = (u32) dma_addr;
		ring->buf[i].desc->ctrl = DESC_EMPTY;
	}

	/* flush descriptors */
	wmb();

	ring->curr = 0;
	ring->dirty = 0;

	return ret;
}

static int ag71xx_ring_rx_refill(struct ag71xx *ag)
{
	struct ag71xx_ring *ring = &ag->rx_ring;
	unsigned int reserve = ag71xx_rx_reserve(ag);
	unsigned int count;

	count = 0;
	for (; ring->curr - ring->dirty > 0; ring->dirty++) {
		unsigned int i;

		i = ring->dirty % AG71XX_RX_RING_SIZE;

		if (ring->buf[i].skb == NULL) {
			dma_addr_t dma_addr;
			struct sk_buff *skb;

			skb = dev_alloc_skb(AG71XX_RX_PKT_SIZE + reserve);
			if (skb == NULL)
				break;

			skb_reserve(skb, reserve);
			skb->dev = ag->dev;

			dma_addr = dma_map_single(&ag->dev->dev, skb->data,
						  AG71XX_RX_PKT_SIZE,
						  DMA_FROM_DEVICE);

			ring->buf[i].skb = skb;
			ring->buf[i].dma_addr = dma_addr;
			ring->buf[i].desc->data = (u32) dma_addr;
		}

		ring->buf[i].desc->ctrl = DESC_EMPTY;
		count++;
	}

	/* flush descriptors */
	wmb();

	DBG("%s: %u rx descriptors refilled\n", ag->dev->name, count);

	return count;
}

static int ag71xx_rings_init(struct ag71xx *ag)
{
	int ret;

	ret = ag71xx_ring_alloc(&ag->tx_ring, AG71XX_TX_RING_SIZE);
	if (ret)
		return ret;

	ag71xx_ring_tx_init(ag);

	ret = ag71xx_ring_alloc(&ag->rx_ring, AG71XX_RX_RING_SIZE);
	if (ret)
		return ret;

	ret = ag71xx_ring_rx_init(ag);
	return ret;
}

static void ag71xx_rings_cleanup(struct ag71xx *ag)
{
	ag71xx_ring_rx_clean(ag);
	ag71xx_ring_free(&ag->rx_ring);

	ag71xx_ring_tx_clean(ag);
	ag71xx_ring_free(&ag->tx_ring);
}

static unsigned char *ag71xx_speed_str(struct ag71xx *ag)
{
	switch (ag->speed) {
	case SPEED_1000:
		return "1000";
	case SPEED_100:
		return "100";
	case SPEED_10:
		return "10";
	}

	return "?";
}

void ag71xx_link_adjust(struct ag71xx *ag)
{
	struct ag71xx_platform_data *pdata = ag71xx_get_pdata(ag);
	u32 cfg2;
	u32 ifctl;
	u32 fifo5;
	u32 mii_speed;

	if (!ag->link) {
		netif_carrier_off(ag->dev);
		if (netif_msg_link(ag))
			printk(KERN_INFO "%s: link down\n", ag->dev->name);
		return;
	}

	cfg2 = ag71xx_rr(ag, AG71XX_REG_MAC_CFG2);
	cfg2 &= ~(MAC_CFG2_IF_1000 | MAC_CFG2_IF_10_100 | MAC_CFG2_FDX);
	cfg2 |= (ag->duplex) ? MAC_CFG2_FDX : 0;

	ifctl = ag71xx_rr(ag, AG71XX_REG_MAC_IFCTL);
	ifctl &= ~(MAC_IFCTL_SPEED);

	fifo5 = ag71xx_rr(ag, AG71XX_REG_FIFO_CFG5);
	fifo5 &= ~FIFO_CFG5_BM;

	switch (ag->speed) {
	case SPEED_1000:
		mii_speed =  MII_CTRL_SPEED_1000;
		cfg2 |= MAC_CFG2_IF_1000;
		fifo5 |= FIFO_CFG5_BM;
		break;
	case SPEED_100:
		mii_speed = MII_CTRL_SPEED_100;
		cfg2 |= MAC_CFG2_IF_10_100;
		ifctl |= MAC_IFCTL_SPEED;
		break;
	case SPEED_10:
		mii_speed = MII_CTRL_SPEED_10;
		cfg2 |= MAC_CFG2_IF_10_100;
		break;
	default:
		BUG();
		return;
	}

	if (pdata->is_ar91xx)
		ag71xx_wr(ag, AG71XX_REG_FIFO_CFG3, 0x00780fff);
	else if (pdata->is_ar724x)
		ag71xx_wr(ag, AG71XX_REG_FIFO_CFG3, pdata->fifo_cfg3);
	else
		ag71xx_wr(ag, AG71XX_REG_FIFO_CFG3, 0x008001ff);

	if (pdata->set_pll)
		pdata->set_pll(ag->speed);

	ag71xx_mii_ctrl_set_speed(ag, mii_speed);

	ag71xx_wr(ag, AG71XX_REG_MAC_CFG2, cfg2);
	ag71xx_wr(ag, AG71XX_REG_FIFO_CFG5, fifo5);
	ag71xx_wr(ag, AG71XX_REG_MAC_IFCTL, ifctl);

	netif_carrier_on(ag->dev);
	if (netif_msg_link(ag))
		printk(KERN_INFO "%s: link up (%sMbps/%s duplex)\n",
			ag->dev->name,
			ag71xx_speed_str(ag),
			(DUPLEX_FULL == ag->duplex) ? "Full" : "Half");

	DBG("%s: fifo_cfg0=%#x, fifo_cfg1=%#x, fifo_cfg2=%#x\n",
		ag->dev->name,
		ag71xx_rr(ag, AG71XX_REG_FIFO_CFG0),
		ag71xx_rr(ag, AG71XX_REG_FIFO_CFG1),
		ag71xx_rr(ag, AG71XX_REG_FIFO_CFG2));

	DBG("%s: fifo_cfg3=%#x, fifo_cfg4=%#x, fifo_cfg5=%#x\n",
		ag->dev->name,
		ag71xx_rr(ag, AG71XX_REG_FIFO_CFG3),
		ag71xx_rr(ag, AG71XX_REG_FIFO_CFG4),
		ag71xx_rr(ag, AG71XX_REG_FIFO_CFG5));

	DBG("%s: mac_cfg2=%#x, mac_ifctl=%#x, mii_ctrl=%#x\n",
		ag->dev->name,
		ag71xx_rr(ag, AG71XX_REG_MAC_CFG2),
		ag71xx_rr(ag, AG71XX_REG_MAC_IFCTL),
		ag71xx_mii_ctrl_rr(ag));
}

static void ag71xx_hw_set_macaddr(struct ag71xx *ag, unsigned char *mac)
{
	u32 t;

	t = (((u32) mac[5]) << 24) | (((u32) mac[4]) << 16)
	  | (((u32) mac[3]) << 8) | ((u32) mac[2]);

	ag71xx_wr(ag, AG71XX_REG_MAC_ADDR1, t);

	t = (((u32) mac[1]) << 24) | (((u32) mac[0]) << 16);
	ag71xx_wr(ag, AG71XX_REG_MAC_ADDR2, t);
}

static void ag71xx_dma_reset(struct ag71xx *ag)
{
	u32 val;
	int i;

	ag71xx_dump_dma_regs(ag);

	/* stop RX and TX */
	ag71xx_wr(ag, AG71XX_REG_RX_CTRL, 0);
	ag71xx_wr(ag, AG71XX_REG_TX_CTRL, 0);

	/*
	 * give the hardware some time to really stop all rx/tx activity
	 * clearing the descriptors too early causes random memory corruption
	 */
	mdelay(1);

	/* clear descriptor addresses */
	ag71xx_wr(ag, AG71XX_REG_TX_DESC, 0);
	ag71xx_wr(ag, AG71XX_REG_RX_DESC, 0);

	/* clear pending RX/TX interrupts */
	for (i = 0; i < 256; i++) {
		ag71xx_wr(ag, AG71XX_REG_RX_STATUS, RX_STATUS_PR);
		ag71xx_wr(ag, AG71XX_REG_TX_STATUS, TX_STATUS_PS);
	}

	/* clear pending errors */
	ag71xx_wr(ag, AG71XX_REG_RX_STATUS, RX_STATUS_BE | RX_STATUS_OF);
	ag71xx_wr(ag, AG71XX_REG_TX_STATUS, TX_STATUS_BE | TX_STATUS_UR);

	val = ag71xx_rr(ag, AG71XX_REG_RX_STATUS);
	if (val)
		printk(KERN_ALERT "%s: unable to clear DMA Rx status: %08x\n",
			ag->dev->name, val);

	val = ag71xx_rr(ag, AG71XX_REG_TX_STATUS);

	/* mask out reserved bits */
	val &= ~0xff000000;

	if (val)
		printk(KERN_ALERT "%s: unable to clear DMA Tx status: %08x\n",
			ag->dev->name, val);

	ag71xx_dump_dma_regs(ag);
}

#define MAC_CFG1_INIT	(MAC_CFG1_RXE | MAC_CFG1_TXE | \
			 MAC_CFG1_SRX | MAC_CFG1_STX)

#define FIFO_CFG0_INIT	(FIFO_CFG0_ALL << FIFO_CFG0_ENABLE_SHIFT)

#define FIFO_CFG4_INIT	(FIFO_CFG4_DE | FIFO_CFG4_DV | FIFO_CFG4_FC | \
			 FIFO_CFG4_CE | FIFO_CFG4_CR | FIFO_CFG4_LM | \
			 FIFO_CFG4_LO | FIFO_CFG4_OK | FIFO_CFG4_MC | \
			 FIFO_CFG4_BC | FIFO_CFG4_DR | FIFO_CFG4_LE | \
			 FIFO_CFG4_CF | FIFO_CFG4_PF | FIFO_CFG4_UO | \
			 FIFO_CFG4_VT)

#define FIFO_CFG5_INIT	(FIFO_CFG5_DE | FIFO_CFG5_DV | FIFO_CFG5_FC | \
			 FIFO_CFG5_CE | FIFO_CFG5_LO | FIFO_CFG5_OK | \
			 FIFO_CFG5_MC | FIFO_CFG5_BC | FIFO_CFG5_DR | \
			 FIFO_CFG5_CF | FIFO_CFG5_PF | FIFO_CFG5_VT | \
			 FIFO_CFG5_LE | FIFO_CFG5_FT | FIFO_CFG5_16 | \
			 FIFO_CFG5_17 | FIFO_CFG5_SF)

static void ag71xx_hw_init(struct ag71xx *ag)
{
	struct ag71xx_platform_data *pdata = ag71xx_get_pdata(ag);

	ag71xx_sb(ag, AG71XX_REG_MAC_CFG1, MAC_CFG1_SR);
	udelay(20);

	ar71xx_device_stop(pdata->reset_bit);
	mdelay(100);
	ar71xx_device_start(pdata->reset_bit);
	mdelay(100);

	/* setup MAC configuration registers */
	if (pdata->is_ar724x)
		ag71xx_wr(ag, AG71XX_REG_MAC_CFG1,
			  MAC_CFG1_INIT | MAC_CFG1_TFC | MAC_CFG1_RFC);
	else
		ag71xx_wr(ag, AG71XX_REG_MAC_CFG1, MAC_CFG1_INIT);

	ag71xx_sb(ag, AG71XX_REG_MAC_CFG2,
		  MAC_CFG2_PAD_CRC_EN | MAC_CFG2_LEN_CHECK);

	/* setup max frame length */
	ag71xx_wr(ag, AG71XX_REG_MAC_MFL, AG71XX_TX_MTU_LEN);

	/* setup MII interface type */
	ag71xx_mii_ctrl_set_if(ag, pdata->mii_if);

	/* setup FIFO configuration registers */
	ag71xx_wr(ag, AG71XX_REG_FIFO_CFG0, FIFO_CFG0_INIT);
	if (pdata->is_ar724x) {
		ag71xx_wr(ag, AG71XX_REG_FIFO_CFG1, pdata->fifo_cfg1);
		ag71xx_wr(ag, AG71XX_REG_FIFO_CFG2, pdata->fifo_cfg2);
	} else {
		ag71xx_wr(ag, AG71XX_REG_FIFO_CFG1, 0x0fff0000);
		ag71xx_wr(ag, AG71XX_REG_FIFO_CFG2, 0x00001fff);
	}
	ag71xx_wr(ag, AG71XX_REG_FIFO_CFG4, FIFO_CFG4_INIT);
	ag71xx_wr(ag, AG71XX_REG_FIFO_CFG5, FIFO_CFG5_INIT);

	ag71xx_dma_reset(ag);
}

static void ag71xx_hw_start(struct ag71xx *ag)
{
	/* start RX engine */
	ag71xx_wr(ag, AG71XX_REG_RX_CTRL, RX_CTRL_RXE);

	/* enable interrupts */
	ag71xx_wr(ag, AG71XX_REG_INT_ENABLE, AG71XX_INT_INIT);
}

static void ag71xx_hw_stop(struct ag71xx *ag)
{
	/* disable all interrupts */
	ag71xx_wr(ag, AG71XX_REG_INT_ENABLE, 0);

	ag71xx_dma_reset(ag);
}

static int ag71xx_open(struct net_device *dev)
{
	struct ag71xx *ag = netdev_priv(dev);
	int ret;

	ret = ag71xx_rings_init(ag);
	if (ret)
		goto err;

	napi_enable(&ag->napi);

	netif_carrier_off(dev);
	ag71xx_phy_start(ag);

	ag71xx_wr(ag, AG71XX_REG_TX_DESC, ag->tx_ring.descs_dma);
	ag71xx_wr(ag, AG71XX_REG_RX_DESC, ag->rx_ring.descs_dma);

	ag71xx_hw_set_macaddr(ag, dev->dev_addr);

	ag71xx_hw_start(ag);

	netif_start_queue(dev);

	return 0;

err:
	ag71xx_rings_cleanup(ag);
	return ret;
}

static int ag71xx_stop(struct net_device *dev)
{
	struct ag71xx *ag = netdev_priv(dev);
	unsigned long flags;

	netif_carrier_off(dev);
	ag71xx_phy_stop(ag);

	spin_lock_irqsave(&ag->lock, flags);

	netif_stop_queue(dev);

	ag71xx_hw_stop(ag);

	napi_disable(&ag->napi);
	del_timer_sync(&ag->oom_timer);

	spin_unlock_irqrestore(&ag->lock, flags);

	ag71xx_rings_cleanup(ag);

	return 0;
}

static netdev_tx_t ag71xx_hard_start_xmit(struct sk_buff *skb,
					  struct net_device *dev)
{
	struct ag71xx *ag = netdev_priv(dev);
	struct ag71xx_ring *ring = &ag->tx_ring;
	struct ag71xx_desc *desc;
	dma_addr_t dma_addr;
	int i;

	i = ring->curr % AG71XX_TX_RING_SIZE;
	desc = ring->buf[i].desc;

	if (!ag71xx_desc_empty(desc))
		goto err_drop;

	if (ag71xx_has_ar8216(ag))
		ag71xx_add_ar8216_header(ag, skb);

	if (skb->len <= 0) {
		DBG("%s: packet len is too small\n", ag->dev->name);
		goto err_drop;
	}

	dma_addr = dma_map_single(&dev->dev, skb->data, skb->len,
				  DMA_TO_DEVICE);

	ring->buf[i].skb = skb;

	/* setup descriptor fields */
	desc->data = (u32) dma_addr;
	desc->ctrl = (skb->len & DESC_PKTLEN_M);

	/* flush descriptor */
	wmb();

	ring->curr++;
	if (ring->curr == (ring->dirty + AG71XX_TX_THRES_STOP)) {
		DBG("%s: tx queue full\n", ag->dev->name);
		netif_stop_queue(dev);
	}

	DBG("%s: packet injected into TX queue\n", ag->dev->name);

	/* enable TX engine */
	ag71xx_wr(ag, AG71XX_REG_TX_CTRL, TX_CTRL_TXE);

	return NETDEV_TX_OK;

err_drop:
	dev->stats.tx_dropped++;

	dev_kfree_skb(skb);
	return NETDEV_TX_OK;
}

static int ag71xx_do_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd)
{
	struct ag71xx *ag = netdev_priv(dev);
	int ret;

	switch (cmd) {
	case SIOCETHTOOL:
		if (ag->phy_dev == NULL)
			break;

		spin_lock_irq(&ag->lock);
		ret = phy_ethtool_ioctl(ag->phy_dev, (void *) ifr->ifr_data);
		spin_unlock_irq(&ag->lock);
		return ret;

	case SIOCSIFHWADDR:
		if (copy_from_user
			(dev->dev_addr, ifr->ifr_data, sizeof(dev->dev_addr)))
			return -EFAULT;
		return 0;

	case SIOCGIFHWADDR:
		if (copy_to_user
			(ifr->ifr_data, dev->dev_addr, sizeof(dev->dev_addr)))
			return -EFAULT;
		return 0;

	case SIOCGMIIPHY:
	case SIOCGMIIREG:
	case SIOCSMIIREG:
		if (ag->phy_dev == NULL)
			break;

		return phy_mii_ioctl(ag->phy_dev, ifr, cmd);

	default:
		break;
	}

	return -EOPNOTSUPP;
}

static void ag71xx_oom_timer_handler(unsigned long data)
{
	struct net_device *dev = (struct net_device *) data;
	struct ag71xx *ag = netdev_priv(dev);

	napi_schedule(&ag->napi);
}

static void ag71xx_tx_timeout(struct net_device *dev)
{
	struct ag71xx *ag = netdev_priv(dev);

	if (netif_msg_tx_err(ag))
		printk(KERN_DEBUG "%s: tx timeout\n", ag->dev->name);

	schedule_work(&ag->restart_work);
}

static void ag71xx_restart_work_func(struct work_struct *work)
{
	struct ag71xx *ag = container_of(work, struct ag71xx, restart_work);
	struct ag71xx_platform_data *pdata = ag71xx_get_pdata(ag);

	ag71xx_stop(ag->dev);

	if (pdata->is_ar724x)
		ag71xx_hw_init(ag);

	ag71xx_open(ag->dev);
}

static int ag71xx_tx_packets(struct ag71xx *ag)
{
	struct ag71xx_ring *ring = &ag->tx_ring;
	int sent;

	DBG("%s: processing TX ring\n", ag->dev->name);

	sent = 0;
	while (ring->dirty != ring->curr) {
		unsigned int i = ring->dirty % AG71XX_TX_RING_SIZE;
		struct ag71xx_desc *desc = ring->buf[i].desc;
		struct sk_buff *skb = ring->buf[i].skb;

		if (!ag71xx_desc_empty(desc))
			break;

		ag71xx_wr(ag, AG71XX_REG_TX_STATUS, TX_STATUS_PS);

		ag->dev->stats.tx_bytes += skb->len;
		ag->dev->stats.tx_packets++;

		dev_kfree_skb_any(skb);
		ring->buf[i].skb = NULL;

		ring->dirty++;
		sent++;
	}

	DBG("%s: %d packets sent out\n", ag->dev->name, sent);

	if ((ring->curr - ring->dirty) < AG71XX_TX_THRES_WAKEUP)
		netif_wake_queue(ag->dev);

	return sent;
}

static int ag71xx_rx_packets(struct ag71xx *ag, int limit)
{
	struct net_device *dev = ag->dev;
	struct ag71xx_ring *ring = &ag->rx_ring;
	int done = 0;

	DBG("%s: rx packets, limit=%d, curr=%u, dirty=%u\n",
			dev->name, limit, ring->curr, ring->dirty);

	while (done < limit) {
		unsigned int i = ring->curr % AG71XX_RX_RING_SIZE;
		struct ag71xx_desc *desc = ring->buf[i].desc;
		struct sk_buff *skb;
		int pktlen;
		int err = 0;

		if (ag71xx_desc_empty(desc))
			break;

		if ((ring->dirty + AG71XX_RX_RING_SIZE) == ring->curr) {
			ag71xx_assert(0);
			break;
		}

		ag71xx_wr(ag, AG71XX_REG_RX_STATUS, RX_STATUS_PR);

		skb = ring->buf[i].skb;
		pktlen = ag71xx_desc_pktlen(desc);
		pktlen -= ETH_FCS_LEN;

		dma_unmap_single(&dev->dev, ring->buf[i].dma_addr,
				 AG71XX_RX_PKT_SIZE, DMA_FROM_DEVICE);

		dev->last_rx = jiffies;
		dev->stats.rx_packets++;
		dev->stats.rx_bytes += pktlen;

		skb_put(skb, pktlen);
		if (ag71xx_has_ar8216(ag))
			err = ag71xx_remove_ar8216_header(ag, skb, pktlen);

		if (err) {
			dev->stats.rx_dropped++;
			kfree_skb(skb);
		} else {
			skb->dev = dev;
			skb->ip_summed = CHECKSUM_NONE;
			if (ag->phy_dev) {
				ag->phy_dev->netif_receive_skb(skb);
			} else {
				skb->protocol = eth_type_trans(skb, dev);
				netif_receive_skb(skb);
			}
		}

		ring->buf[i].skb = NULL;
		done++;

		ring->curr++;
	}

	ag71xx_ring_rx_refill(ag);

	DBG("%s: rx finish, curr=%u, dirty=%u, done=%d\n",
		dev->name, ring->curr, ring->dirty, done);

	return done;
}

static int ag71xx_poll(struct napi_struct *napi, int limit)
{
	struct ag71xx *ag = container_of(napi, struct ag71xx, napi);
	struct ag71xx_platform_data *pdata = ag71xx_get_pdata(ag);
	struct net_device *dev = ag->dev;
	struct ag71xx_ring *rx_ring;
	unsigned long flags;
	u32 status;
	int tx_done;
	int rx_done;

	pdata->ddr_flush();
	tx_done = ag71xx_tx_packets(ag);

	DBG("%s: processing RX ring\n", dev->name);
	rx_done = ag71xx_rx_packets(ag, limit);

	ag71xx_debugfs_update_napi_stats(ag, rx_done, tx_done);

	rx_ring = &ag->rx_ring;
	if (rx_ring->buf[rx_ring->dirty % AG71XX_RX_RING_SIZE].skb == NULL)
		goto oom;

	status = ag71xx_rr(ag, AG71XX_REG_RX_STATUS);
	if (unlikely(status & RX_STATUS_OF)) {
		ag71xx_wr(ag, AG71XX_REG_RX_STATUS, RX_STATUS_OF);
		dev->stats.rx_fifo_errors++;

		/* restart RX */
		ag71xx_wr(ag, AG71XX_REG_RX_CTRL, RX_CTRL_RXE);
	}

	if (rx_done < limit) {
		if (status & RX_STATUS_PR)
			goto more;

		status = ag71xx_rr(ag, AG71XX_REG_TX_STATUS);
		if (status & TX_STATUS_PS)
			goto more;

		DBG("%s: disable polling mode, rx=%d, tx=%d,limit=%d\n",
			dev->name, rx_done, tx_done, limit);

		napi_complete(napi);

		/* enable interrupts */
		spin_lock_irqsave(&ag->lock, flags);
		ag71xx_int_enable(ag, AG71XX_INT_POLL);
		spin_unlock_irqrestore(&ag->lock, flags);
		return rx_done;
	}

more:
	DBG("%s: stay in polling mode, rx=%d, tx=%d, limit=%d\n",
			dev->name, rx_done, tx_done, limit);
	return rx_done;

oom:
	if (netif_msg_rx_err(ag))
		printk(KERN_DEBUG "%s: out of memory\n", dev->name);

	mod_timer(&ag->oom_timer, jiffies + AG71XX_OOM_REFILL);
	napi_complete(napi);
	return 0;
}

static irqreturn_t ag71xx_interrupt(int irq, void *dev_id)
{
	struct net_device *dev = dev_id;
	struct ag71xx *ag = netdev_priv(dev);
	u32 status;

	status = ag71xx_rr(ag, AG71XX_REG_INT_STATUS);
	ag71xx_dump_intr(ag, "raw", status);

	if (unlikely(!status))
		return IRQ_NONE;

	if (unlikely(status & AG71XX_INT_ERR)) {
		if (status & AG71XX_INT_TX_BE) {
			ag71xx_wr(ag, AG71XX_REG_TX_STATUS, TX_STATUS_BE);
			dev_err(&dev->dev, "TX BUS error\n");
		}
		if (status & AG71XX_INT_RX_BE) {
			ag71xx_wr(ag, AG71XX_REG_RX_STATUS, RX_STATUS_BE);
			dev_err(&dev->dev, "RX BUS error\n");
		}
	}

	if (likely(status & AG71XX_INT_POLL)) {
		ag71xx_int_disable(ag, AG71XX_INT_POLL);
		DBG("%s: enable polling mode\n", dev->name);
		napi_schedule(&ag->napi);
	}

	ag71xx_debugfs_update_int_stats(ag, status);

	return IRQ_HANDLED;
}

static void ag71xx_set_multicast_list(struct net_device *dev)
{
	/* TODO */
}

#ifdef CONFIG_NET_POLL_CONTROLLER
/*
 * Polling 'interrupt' - used by things like netconsole to send skbs
 * without having to re-enable interrupts. It's not called while
 * the interrupt routine is executing.
 */
static void ag71xx_netpoll(struct net_device *dev)
{
	disable_irq(dev->irq);
	ag71xx_interrupt(dev->irq, dev);
	enable_irq(dev->irq);
}
#endif

static const struct net_device_ops ag71xx_netdev_ops = {
	.ndo_open		= ag71xx_open,
	.ndo_stop		= ag71xx_stop,
	.ndo_start_xmit		= ag71xx_hard_start_xmit,
	.ndo_set_multicast_list	= ag71xx_set_multicast_list,
	.ndo_do_ioctl		= ag71xx_do_ioctl,
	.ndo_tx_timeout		= ag71xx_tx_timeout,
	.ndo_change_mtu		= eth_change_mtu,
	.ndo_set_mac_address	= eth_mac_addr,
	.ndo_validate_addr	= eth_validate_addr,
#ifdef CONFIG_NET_POLL_CONTROLLER
	.ndo_poll_controller	= ag71xx_netpoll,
#endif
};

static int __devinit ag71xx_probe(struct platform_device *pdev)
{
	struct net_device *dev;
	struct resource *res;
	struct ag71xx *ag;
	struct ag71xx_platform_data *pdata;
	int err;

	pdata = pdev->dev.platform_data;
	if (!pdata) {
		dev_err(&pdev->dev, "no platform data specified\n");
		err = -ENXIO;
		goto err_out;
	}

	if (pdata->mii_bus_dev == NULL) {
		dev_err(&pdev->dev, "no MII bus device specified\n");
		err = -EINVAL;
		goto err_out;
	}

	dev = alloc_etherdev(sizeof(*ag));
	if (!dev) {
		dev_err(&pdev->dev, "alloc_etherdev failed\n");
		err = -ENOMEM;
		goto err_out;
	}

	SET_NETDEV_DEV(dev, &pdev->dev);

	ag = netdev_priv(dev);
	ag->pdev = pdev;
	ag->dev = dev;
	ag->msg_enable = netif_msg_init(ag71xx_msg_level,
					AG71XX_DEFAULT_MSG_ENABLE);
	spin_lock_init(&ag->lock);

	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "mac_base");
	if (!res) {
		dev_err(&pdev->dev, "no mac_base resource found\n");
		err = -ENXIO;
		goto err_out;
	}

	ag->mac_base = ioremap_nocache(res->start, res->end - res->start + 1);
	if (!ag->mac_base) {
		dev_err(&pdev->dev, "unable to ioremap mac_base\n");
		err = -ENOMEM;
		goto err_free_dev;
	}

	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "mii_ctrl");
	if (!res) {
		dev_err(&pdev->dev, "no mii_ctrl resource found\n");
		err = -ENXIO;
		goto err_unmap_base;
	}

	ag->mii_ctrl = ioremap_nocache(res->start, res->end - res->start + 1);
	if (!ag->mii_ctrl) {
		dev_err(&pdev->dev, "unable to ioremap mii_ctrl\n");
		err = -ENOMEM;
		goto err_unmap_base;
	}

	dev->irq = platform_get_irq(pdev, 0);
	err = request_irq(dev->irq, ag71xx_interrupt,
			  IRQF_DISABLED,
			  dev->name, dev);
	if (err) {
		dev_err(&pdev->dev, "unable to request IRQ %d\n", dev->irq);
		goto err_unmap_mii_ctrl;
	}

	dev->base_addr = (unsigned long)ag->mac_base;
	dev->netdev_ops = &ag71xx_netdev_ops;
	dev->ethtool_ops = &ag71xx_ethtool_ops;

	INIT_WORK(&ag->restart_work, ag71xx_restart_work_func);

	init_timer(&ag->oom_timer);
	ag->oom_timer.data = (unsigned long) dev;
	ag->oom_timer.function = ag71xx_oom_timer_handler;

	memcpy(dev->dev_addr, pdata->mac_addr, ETH_ALEN);

	netif_napi_add(dev, &ag->napi, ag71xx_poll, AG71XX_NAPI_WEIGHT);

	err = register_netdev(dev);
	if (err) {
		dev_err(&pdev->dev, "unable to register net device\n");
		goto err_free_irq;
	}

	printk(KERN_INFO "%s: Atheros AG71xx at 0x%08lx, irq %d\n",
	       dev->name, dev->base_addr, dev->irq);

	ag71xx_dump_regs(ag);

	ag71xx_hw_init(ag);

	ag71xx_dump_regs(ag);

	err = ag71xx_phy_connect(ag);
	if (err)
		goto err_unregister_netdev;

	err = ag71xx_debugfs_init(ag);
	if (err)
		goto err_phy_disconnect;

	platform_set_drvdata(pdev, dev);

	return 0;

err_phy_disconnect:
	ag71xx_phy_disconnect(ag);
err_unregister_netdev:
	unregister_netdev(dev);
err_free_irq:
	free_irq(dev->irq, dev);
err_unmap_mii_ctrl:
	iounmap(ag->mii_ctrl);
err_unmap_base:
	iounmap(ag->mac_base);
err_free_dev:
	kfree(dev);
err_out:
	platform_set_drvdata(pdev, NULL);
	return err;
}

static int __devexit ag71xx_remove(struct platform_device *pdev)
{
	struct net_device *dev = platform_get_drvdata(pdev);

	if (dev) {
		struct ag71xx *ag = netdev_priv(dev);

		ag71xx_debugfs_exit(ag);
		ag71xx_phy_disconnect(ag);
		unregister_netdev(dev);
		free_irq(dev->irq, dev);
		iounmap(ag->mii_ctrl);
		iounmap(ag->mac_base);
		kfree(dev);
		platform_set_drvdata(pdev, NULL);
	}

	return 0;
}

static struct platform_driver ag71xx_driver = {
	.probe		= ag71xx_probe,
	.remove		= __exit_p(ag71xx_remove),
	.driver = {
		.name	= AG71XX_DRV_NAME,
	}
};

static int __init ag71xx_module_init(void)
{
	int ret;

	ret = ag71xx_debugfs_root_init();
	if (ret)
		goto err_out;

	ret = ag71xx_mdio_driver_init();
	if (ret)
		goto err_debugfs_exit;

	ret = platform_driver_register(&ag71xx_driver);
	if (ret)
		goto err_mdio_exit;

	return 0;

err_mdio_exit:
	ag71xx_mdio_driver_exit();
err_debugfs_exit:
	ag71xx_debugfs_root_exit();
err_out:
	return ret;
}

static void __exit ag71xx_module_exit(void)
{
	platform_driver_unregister(&ag71xx_driver);
	ag71xx_mdio_driver_exit();
	ag71xx_debugfs_root_exit();
}

module_init(ag71xx_module_init);
module_exit(ag71xx_module_exit);

MODULE_VERSION(AG71XX_DRV_VERSION);
MODULE_AUTHOR("Gabor Juhos <juhosg@openwrt.org>");
MODULE_AUTHOR("Imre Kaloz <kaloz@openwrt.org>");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:" AG71XX_DRV_NAME);

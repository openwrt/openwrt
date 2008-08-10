/*
 *  Atheros AR71xx built-in ethernet mac driver
 *
 *  Copyright (C) 2008 Gabor Juhos <juhosg@openwrt.org>
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
	( NETIF_MSG_DRV 		\
	| NETIF_MSG_PROBE		\
	| NETIF_MSG_LINK		\
	| NETIF_MSG_TIMER		\
	| NETIF_MSG_IFDOWN		\
	| NETIF_MSG_IFUP		\
	| NETIF_MSG_RX_ERR		\
	| NETIF_MSG_TX_ERR )

static int ag71xx_debug = -1;

module_param(ag71xx_debug, int, 0);
MODULE_PARM_DESC(ag71xx_debug, "Debug level (-1=defaults,0=none,...,16=all)");

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
	DBG("%s: fifo_cfg3=%08x, fifo_cfg3=%08x, fifo_cfg5=%08x\n",
		ag->dev->name,
		ag71xx_rr(ag, AG71XX_REG_FIFO_CFG3),
		ag71xx_rr(ag, AG71XX_REG_FIFO_CFG4),
		ag71xx_rr(ag, AG71XX_REG_FIFO_CFG5));
}

static void ag71xx_ring_free(struct ag71xx_ring *ring)
{
	kfree(ring->buf);

	if (ring->descs)
		dma_free_coherent(NULL, ring->size * sizeof(*ring->descs),
				  ring->descs, ring->descs_dma);
}

static int ag71xx_ring_alloc(struct ag71xx_ring *ring, unsigned int size)
{
	int err;

	ring->descs = dma_alloc_coherent(NULL, size * sizeof(*ring->descs),
					 &ring->descs_dma,
					 GFP_ATOMIC);
	if (!ring->descs) {
		err = -ENOMEM;
		goto err;
	}

	ring->size = size;

	ring->buf = kzalloc(size * sizeof(*ring->buf), GFP_KERNEL);
	if (!ring->buf) {
		err = -ENOMEM;
		goto err;
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

		if (!ag71xx_desc_empty(&ring->descs[i])) {
			ring->descs[i].ctrl = 0;
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
		ring->descs[i].next = (u32) (ring->descs_dma +
			sizeof(*ring->descs) * ((i + 1) % AG71XX_TX_RING_SIZE));

		ring->descs[i].ctrl = DESC_EMPTY;
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
		if (ring->buf[i].skb)
			kfree_skb(ring->buf[i].skb);

}

static int ag71xx_ring_rx_init(struct ag71xx *ag)
{
	struct ag71xx_ring *ring = &ag->rx_ring;
	unsigned int i;
	int ret;

	ret = 0;
	for (i = 0; i < AG71XX_RX_RING_SIZE; i++)
		ring->descs[i].next = (u32) (ring->descs_dma +
			sizeof(*ring->descs) * ((i + 1) % AG71XX_RX_RING_SIZE));

	for (i = 0; i < AG71XX_RX_RING_SIZE; i++) {
		struct sk_buff *skb;

		skb = dev_alloc_skb(AG71XX_RX_PKT_SIZE);
		if (!skb) {
			ret = -ENOMEM;
			break;
		}

		skb->dev = ag->dev;
		skb_reserve(skb, AG71XX_RX_PKT_RESERVE);

		ring->buf[i].skb = skb;
		ring->descs[i].data = virt_to_phys(skb->data);
		ring->descs[i].ctrl = DESC_EMPTY;
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
	unsigned int count;

	count = 0;
	for (; ring->curr - ring->dirty > 0; ring->dirty++) {
		unsigned int i;

		i = ring->dirty % AG71XX_RX_RING_SIZE;

		if (ring->buf[i].skb == NULL) {
			struct sk_buff *skb;

			skb = dev_alloc_skb(AG71XX_RX_PKT_SIZE);
			if (skb == NULL) {
				printk(KERN_ERR "%s: no memory for skb\n",
					ag->dev->name);
				break;
			}

			skb_reserve(skb, AG71XX_RX_PKT_RESERVE);
			skb->dev = ag->dev;
			ring->buf[i].skb = skb;
			ring->descs[i].data = virt_to_phys(skb->data);
		}

		ring->descs[i].ctrl = DESC_EMPTY;
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

static void ag71xx_hw_set_macaddr(struct ag71xx *ag, unsigned char *mac)
{
	u32 t;

	t = (((u32) mac[0]) << 24) | (((u32) mac[1]) << 16)
	  | (((u32) mac[2]) << 8) | ((u32) mac[2]);

	ag71xx_wr(ag, AG71XX_REG_MAC_ADDR1, t);

	t = (((u32) mac[4]) << 24) | (((u32) mac[5]) << 16);
	ag71xx_wr(ag, AG71XX_REG_MAC_ADDR2, t);
}

#define MAC_CFG1_INIT	(MAC_CFG1_RXE | MAC_CFG1_TXE | MAC_CFG1_SRX \
			| MAC_CFG1_STX)

static void ag71xx_hw_init(struct ag71xx *ag)
{
	struct ag71xx_platform_data *pdata = ag71xx_get_pdata(ag);

	ag71xx_sb(ag, AG71XX_REG_MAC_CFG1, MAC_CFG1_SR);
	udelay(20);

	ar71xx_device_stop(pdata->reset_bit);
	mdelay(100);
	ar71xx_device_start(pdata->reset_bit);
	mdelay(100);

	ag71xx_wr(ag, AG71XX_REG_MAC_CFG1, MAC_CFG1_INIT);

	/* TODO: set max packet size */

	ag71xx_sb(ag, AG71XX_REG_MAC_CFG2,
		  MAC_CFG2_PAD_CRC_EN | MAC_CFG2_LEN_CHECK);

	ag71xx_wr(ag, AG71XX_REG_FIFO_CFG0, 0x00001f00);

	ag71xx_mii_ctrl_set_if(ag, pdata->mii_if);

	ag71xx_wr(ag, AG71XX_REG_FIFO_CFG1, 0x0fff0000);
	ag71xx_wr(ag, AG71XX_REG_FIFO_CFG2, 0x00001fff);
	ag71xx_wr(ag, AG71XX_REG_FIFO_CFG4, 0x0000ffff);
	ag71xx_wr(ag, AG71XX_REG_FIFO_CFG5, 0x0007ffef);
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
	/* stop RX and TX */
	ag71xx_wr(ag, AG71XX_REG_RX_CTRL, 0);
	ag71xx_wr(ag, AG71XX_REG_TX_CTRL, 0);

	/* disable all interrupts */
	ag71xx_wr(ag, AG71XX_REG_INT_ENABLE, 0);
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

	spin_lock_irqsave(&ag->lock, flags);

	netif_stop_queue(dev);

	ag71xx_hw_stop(ag);

	netif_carrier_off(dev);
	ag71xx_phy_stop(ag);

	napi_disable(&ag->napi);

	spin_unlock_irqrestore(&ag->lock, flags);

	ag71xx_rings_cleanup(ag);

	return 0;
}

static int ag71xx_hard_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
	struct ag71xx *ag = netdev_priv(dev);
	struct ag71xx_platform_data *pdata = ag71xx_get_pdata(ag);
	struct ag71xx_ring *ring = &ag->tx_ring;
	struct ag71xx_desc *desc;
	unsigned long flags;
	int i;

	i = ring->curr % AG71XX_TX_RING_SIZE;
	desc = &ring->descs[i];

	spin_lock_irqsave(&ag->lock, flags);
	ar71xx_ddr_flush(pdata->flush_reg);
	spin_unlock_irqrestore(&ag->lock, flags);

	if (!ag71xx_desc_empty(desc))
		goto err_drop;

	if (skb->len <= 0) {
		DBG("%s: packet len is too small\n", ag->dev->name);
		goto err_drop;
	}

	dma_cache_wback_inv((unsigned long)skb->data, skb->len);

	ring->buf[i].skb = skb;

	/* setup descriptor fields */
	desc->data = virt_to_phys(skb->data);
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

	dev->trans_start = jiffies;

	return 0;

err_drop:
	dev->stats.tx_dropped++;

	dev_kfree_skb(skb);
	return 0;
}

static int ag71xx_do_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd)
{
	struct mii_ioctl_data *data = (struct mii_ioctl_data *) &ifr->ifr_data;
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

		return phy_mii_ioctl(ag->phy_dev, data, cmd);

	default:
		break;
	}

	return -EOPNOTSUPP;
}

static void ag71xx_tx_packets(struct ag71xx *ag)
{
	struct ag71xx_platform_data *pdata = ag71xx_get_pdata(ag);
	struct ag71xx_ring *ring = &ag->tx_ring;
	unsigned int sent;

	DBG("%s: processing TX ring\n", ag->dev->name);

#ifdef AG71XX_NAPI_TX
	ar71xx_ddr_flush(pdata->flush_reg);
#endif

	sent = 0;
	while (ring->dirty != ring->curr) {
		unsigned int i = ring->dirty % AG71XX_TX_RING_SIZE;
		struct ag71xx_desc *desc = &ring->descs[i];
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

}

static int ag71xx_rx_packets(struct ag71xx *ag, int limit)
{
	struct net_device *dev = ag->dev;
	struct ag71xx_ring *ring = &ag->rx_ring;
#ifndef AG71XX_NAPI_TX
	struct ag71xx_platform_data *pdata = ag71xx_get_pdata(ag);
	unsigned long flags;
#endif
	int done = 0;

#ifndef AG71XX_NAPI_TX
	spin_lock_irqsave(&ag->lock, flags);
	ar71xx_ddr_flush(pdata->flush_reg);
	spin_unlock_irqrestore(&ag->lock, flags);
#endif

	DBG("%s: rx packets, limit=%d, curr=%u, dirty=%u\n",
			dev->name, limit, ring->curr, ring->dirty);

	while (done < limit) {
		unsigned int i = ring->curr % AG71XX_RX_RING_SIZE;
		struct ag71xx_desc *desc = &ring->descs[i];
		struct sk_buff *skb;
		int pktlen;

		if (ag71xx_desc_empty(desc))
			break;

		if ((ring->dirty + AG71XX_RX_RING_SIZE) == ring->curr) {
			ag71xx_assert(0);
			break;
		}

		skb = ring->buf[i].skb;
		pktlen = ag71xx_desc_pktlen(desc);
		pktlen -= ETH_FCS_LEN;

		/* TODO: move it into the refill function */
		dma_cache_wback_inv((unsigned long)skb->data, pktlen);
		skb_put(skb, pktlen);

		skb->dev = dev;
		skb->protocol = eth_type_trans(skb, dev);
		skb->ip_summed = CHECKSUM_UNNECESSARY;

		netif_receive_skb(skb);

		dev->last_rx = jiffies;
		dev->stats.rx_packets++;
		dev->stats.rx_bytes += pktlen;

		ring->buf[i].skb = NULL;
		done++;

		ag71xx_wr(ag, AG71XX_REG_RX_STATUS, RX_STATUS_PR);

		ring->curr++;
		if ((ring->curr - ring->dirty) > (AG71XX_RX_RING_SIZE / 4))
			ag71xx_ring_rx_refill(ag);
	}

	ag71xx_ring_rx_refill(ag);

	DBG("%s: rx finish, curr=%u, dirty=%u, done=%d\n",
		dev->name, ring->curr, ring->dirty, done);

	return done;
}

static int ag71xx_poll(struct napi_struct *napi, int limit)
{
	struct ag71xx *ag = container_of(napi, struct ag71xx, napi);
#ifdef AG71XX_NAPI_TX
	struct ag71xx_platform_data *pdata = ag71xx_get_pdata(ag);
#endif
	struct net_device *dev = ag->dev;
	unsigned long flags;
	u32 status;
	int done;

#ifdef AG71XX_NAPI_TX
	ar71xx_ddr_flush(pdata->flush_reg);
	ag71xx_tx_packets(ag);
#endif

	DBG("%s: processing RX ring\n", dev->name);
	done = ag71xx_rx_packets(ag, limit);

	/* TODO: add OOM handler */

	status = ag71xx_rr(ag, AG71XX_REG_INT_STATUS);
	status &= AG71XX_INT_POLL;

	if ((done < limit) && (!status)) {
		DBG("%s: disable polling mode, done=%d, status=%x\n",
			dev->name, done, status);

		netif_rx_complete(dev, napi);

		/* enable interrupts */
		spin_lock_irqsave(&ag->lock, flags);
		ag71xx_int_enable(ag, AG71XX_INT_POLL);
		spin_unlock_irqrestore(&ag->lock, flags);
		return 0;
	}

	if (status & AG71XX_INT_RX_OF) {
		if (netif_msg_rx_err(ag))
			printk(KERN_ALERT "%s: rx owerflow, restarting dma\n",
				dev->name);

		/* ack interrupt */
		ag71xx_wr(ag, AG71XX_REG_RX_STATUS, RX_STATUS_OF);
		/* restart RX */
		ag71xx_wr(ag, AG71XX_REG_RX_CTRL, RX_CTRL_RXE);
	}

	DBG("%s: stay in polling mode, done=%d, status=%x\n",
			dev->name, done, status);
	return 1;
}

static irqreturn_t ag71xx_interrupt(int irq, void *dev_id)
{
	struct net_device *dev = dev_id;
	struct ag71xx *ag = netdev_priv(dev);
	u32 status;

	status = ag71xx_rr(ag, AG71XX_REG_INT_STATUS);
	status &= ag71xx_rr(ag, AG71XX_REG_INT_ENABLE);

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

#if 0
	if (unlikely(status & AG71XX_INT_TX_UR)) {
		ag71xx_wr(ag, AG71XX_REG_TX_STATUS, TX_STATUS_UR);
		DBG("%s: TX underrun\n", dev->name);
	}
#endif

#ifndef AG71XX_NAPI_TX
	if (likely(status & AG71XX_INT_TX_PS))
		ag71xx_tx_packets(ag);
#endif

	if (likely(status & AG71XX_INT_POLL)) {
		ag71xx_int_disable(ag, AG71XX_INT_POLL);
		DBG("%s: enable polling mode\n", dev->name);
		netif_rx_schedule(dev, &ag->napi);
	}

	return IRQ_HANDLED;
}

static void ag71xx_set_multicast_list(struct net_device *dev)
{
	/* TODO */
}

static int __init ag71xx_probe(struct platform_device *pdev)
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
	ag->mii_bus = &ag71xx_mdio_bus->mii_bus;
	ag->msg_enable = netif_msg_init(ag71xx_debug,
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

	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "mac_base2");
	if (!res) {
		dev_err(&pdev->dev, "no mac_base2 resource found\n");
		err = -ENXIO;
		goto err_unmap_base1;
	}

	ag->mac_base2 = ioremap_nocache(res->start, res->end - res->start + 1);
	if (!ag->mac_base) {
		dev_err(&pdev->dev, "unable to ioremap mac_base2\n");
		err = -ENOMEM;
		goto err_unmap_base1;
	}

	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "mii_ctrl");
	if (!res) {
		dev_err(&pdev->dev, "no mii_ctrl resource found\n");
		err = -ENXIO;
		goto err_unmap_base2;
	}

	ag->mii_ctrl = ioremap_nocache(res->start, res->end - res->start + 1);
	if (!ag->mii_ctrl) {
		dev_err(&pdev->dev, "unable to ioremap mii_ctrl\n");
		err = -ENOMEM;
		goto err_unmap_base2;
	}

	dev->irq = platform_get_irq(pdev, 0);
	err = request_irq(dev->irq, ag71xx_interrupt,
			  IRQF_DISABLED | IRQF_SAMPLE_RANDOM,
			  dev->name, dev);
	if (err) {
		dev_err(&pdev->dev, "unable to request IRQ %d\n", dev->irq);
		goto err_unmap_mii_ctrl;
	}

	dev->base_addr = (unsigned long)ag->mac_base;
	dev->open = ag71xx_open;
	dev->stop = ag71xx_stop;
	dev->hard_start_xmit = ag71xx_hard_start_xmit;
	dev->set_multicast_list = ag71xx_set_multicast_list;
	dev->do_ioctl = ag71xx_do_ioctl;
	dev->ethtool_ops = &ag71xx_ethtool_ops;

	netif_napi_add(dev, &ag->napi, ag71xx_poll, AG71XX_NAPI_WEIGHT);

	if (is_valid_ether_addr(pdata->mac_addr))
		memcpy(dev->dev_addr, pdata->mac_addr, ETH_ALEN);
	else {
		dev->dev_addr[0] = 0xde;
		dev->dev_addr[1] = 0xad;
		get_random_bytes(&dev->dev_addr[2], 3);
		dev->dev_addr[5] = pdev->id & 0xff;
	}

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

	/* Reset the mdio bus explicitly */
	if (ag->mii_bus) {
		mutex_lock(&ag->mii_bus->mdio_lock);
		ag->mii_bus->reset(ag->mii_bus);
		mutex_unlock(&ag->mii_bus->mdio_lock);
	}

	err = ag71xx_phy_connect(ag);
	if (err)
		goto err_unregister_netdev;

	platform_set_drvdata(pdev, dev);

	return 0;

err_unregister_netdev:
	unregister_netdev(dev);
err_free_irq:
	free_irq(dev->irq, dev);
err_unmap_mii_ctrl:
	iounmap(ag->mii_ctrl);
err_unmap_base2:
	iounmap(ag->mac_base2);
err_unmap_base1:
	iounmap(ag->mac_base);
err_free_dev:
	kfree(dev);
err_out:
	platform_set_drvdata(pdev, NULL);
	return err;
}

static int __exit ag71xx_remove(struct platform_device *pdev)
{
	struct net_device *dev = platform_get_drvdata(pdev);

	if (dev) {
		struct ag71xx *ag = netdev_priv(dev);

		ag71xx_phy_disconnect(ag);
		unregister_netdev(dev);
		free_irq(dev->irq, dev);
		iounmap(ag->mii_ctrl);
		iounmap(ag->mac_base2);
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

	ret = ag71xx_mdio_driver_init();
	if (ret)
		goto err_out;

	ret = platform_driver_register(&ag71xx_driver);
	if (ret)
		goto err_mdio_exit;

	return 0;

err_mdio_exit:
	ag71xx_mdio_driver_exit();
err_out:
	return ret;
}

static void __exit ag71xx_module_exit(void)
{
	platform_driver_unregister(&ag71xx_driver);
}

module_init(ag71xx_module_init);
module_exit(ag71xx_module_exit);

MODULE_VERSION(AG71XX_DRV_VERSION);
MODULE_AUTHOR("Gabor Juhos <juhosg@openwrt.org>");
MODULE_AUTHOR("Imre Kaloz <kaloz@openwrt.org>");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:" AG71XX_DRV_NAME);

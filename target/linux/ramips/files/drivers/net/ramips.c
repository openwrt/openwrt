/*
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; version 2 of the License
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 *
 *   Copyright (C) 2009 John Crispin <blogic@openwrt.org>
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/dma-mapping.h>
#include <linux/init.h>
#include <linux/skbuff.h>
#include <linux/etherdevice.h>
#include <linux/platform_device.h>

#include <ramips_eth_platform.h>
#include "ramips_eth.h"

#define TX_TIMEOUT (20 * HZ / 100)
#define	MAX_RX_LENGTH	1600

#ifdef CONFIG_RALINK_RT305X
#include "ramips_esw.c"
#endif

#define phys_to_bus(a)  (a & 0x1FFFFFFF)

static struct net_device * ramips_dev;
static void __iomem *ramips_fe_base = 0;

static inline void
ramips_fe_wr(u32 val, unsigned reg)
{
	__raw_writel(val, ramips_fe_base + reg);
}

static inline u32
ramips_fe_rr(unsigned reg)
{
	return __raw_readl(ramips_fe_base + reg);
}

static void
ramips_cleanup_dma(struct net_device *dev)
{
	struct raeth_priv *priv = netdev_priv(dev);
	int i;

	for (i = 0; i < NUM_RX_DESC; i++)
		if (priv->rx_skb[i])
			dev_kfree_skb_any(priv->rx_skb[i]);

	if (priv->rx)
		dma_free_coherent(NULL,
				  NUM_RX_DESC * sizeof(struct ramips_rx_dma),
				  priv->rx, priv->phy_rx);

	if (priv->tx)
		dma_free_coherent(NULL,
				  NUM_TX_DESC * sizeof(struct ramips_tx_dma),
				  priv->tx, priv->phy_tx);
}

static int
ramips_alloc_dma(struct net_device *dev)
{
	struct raeth_priv *priv = netdev_priv(dev);
	int err = -ENOMEM;
	int i;

	priv->skb_free_idx = 0;

	/* setup tx ring */
	priv->tx = dma_alloc_coherent(NULL,
				      NUM_TX_DESC * sizeof(struct ramips_tx_dma),
				      &priv->phy_tx, GFP_ATOMIC);
	if (!priv->tx)
		goto err_cleanup;

	for (i = 0; i < NUM_TX_DESC; i++) {
		memset(&priv->tx[i], 0, sizeof(struct ramips_tx_dma));
		priv->tx[i].txd2 |= TX_DMA_LSO | TX_DMA_DONE;
		priv->tx[i].txd4 &= (TX_DMA_QN_MASK | TX_DMA_PN_MASK);
		priv->tx[i].txd4 |= TX_DMA_QN(3) | TX_DMA_PN(1);
	}

	/* setup rx ring */
	priv->rx = dma_alloc_coherent(NULL,
				      NUM_RX_DESC * sizeof(struct ramips_rx_dma),
				      &priv->phy_rx, GFP_ATOMIC);
	if (!priv->rx)
		goto err_cleanup;

	memset(priv->rx, 0, sizeof(struct ramips_rx_dma) * NUM_RX_DESC);
	for (i = 0; i < NUM_RX_DESC; i++) {
		struct sk_buff *new_skb = dev_alloc_skb(MAX_RX_LENGTH + 2);

		if (!new_skb)
			goto err_cleanup;

		skb_reserve(new_skb, 2);
		priv->rx[i].rxd1 = dma_map_single(NULL,
						  skb_put(new_skb, 2),
						  MAX_RX_LENGTH + 2,
						  DMA_FROM_DEVICE);
		priv->rx[i].rxd2 |= RX_DMA_LSO;
		priv->rx_skb[i] = new_skb;
	}

	return 0;

 err_cleanup:
	ramips_cleanup_dma(dev);
	return err;
}

static void
ramips_setup_dma(struct net_device *dev)
{
	struct raeth_priv *priv = netdev_priv(dev);

	ramips_fe_wr(phys_to_bus(priv->phy_tx), RAMIPS_TX_BASE_PTR0);
	ramips_fe_wr(NUM_TX_DESC, RAMIPS_TX_MAX_CNT0);
	ramips_fe_wr(0, RAMIPS_TX_CTX_IDX0);
	ramips_fe_wr(RAMIPS_PST_DTX_IDX0, RAMIPS_PDMA_RST_CFG);

	ramips_fe_wr(phys_to_bus(priv->phy_rx), RAMIPS_RX_BASE_PTR0);
	ramips_fe_wr(NUM_RX_DESC, RAMIPS_RX_MAX_CNT0);
	ramips_fe_wr((NUM_RX_DESC - 1), RAMIPS_RX_CALC_IDX0);
	ramips_fe_wr(RAMIPS_PST_DRX_IDX0, RAMIPS_PDMA_RST_CFG);
}

static int
ramips_eth_hard_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
	struct raeth_priv *priv = netdev_priv(dev);
	unsigned long tx;
	unsigned int tx_next;
	unsigned int mapped_addr;
	unsigned long flags;

	if (priv->plat->min_pkt_len) {
		if (skb->len < priv->plat->min_pkt_len) {
			if (skb_padto(skb, priv->plat->min_pkt_len)) {
				printk(KERN_ERR
				       "ramips_eth: skb_padto failed\n");
				kfree_skb(skb);
				return 0;
			}
			skb_put(skb, priv->plat->min_pkt_len - skb->len);
		}
	}

	dev->trans_start = jiffies;
	mapped_addr = (unsigned int) dma_map_single(NULL, skb->data, skb->len,
						    DMA_TO_DEVICE);
	dma_sync_single_for_device(NULL, mapped_addr, skb->len, DMA_TO_DEVICE);
	spin_lock_irqsave(&priv->page_lock, flags);
	tx = ramips_fe_rr(RAMIPS_TX_CTX_IDX0);
	if (tx == NUM_TX_DESC - 1)
		tx_next = 0;
	else
		tx_next = tx + 1;

	if ((priv->tx_skb[tx]) || (priv->tx_skb[tx_next]) ||
	    !(priv->tx[tx].txd2 & TX_DMA_DONE) ||
	    !(priv->tx[tx_next].txd2 & TX_DMA_DONE))
		goto out;

	priv->tx[tx].txd1 = mapped_addr;
	priv->tx[tx].txd2 &= ~(TX_DMA_PLEN0_MASK | TX_DMA_DONE);
	priv->tx[tx].txd2 |= TX_DMA_PLEN0(skb->len);
	dev->stats.tx_packets++;
	dev->stats.tx_bytes += skb->len;
	priv->tx_skb[tx] = skb;
	wmb();
	ramips_fe_wr((tx + 1) % NUM_TX_DESC, RAMIPS_TX_CTX_IDX0);
	spin_unlock_irqrestore(&priv->page_lock, flags);
	return NETDEV_TX_OK;

 out:
	spin_unlock_irqrestore(&priv->page_lock, flags);
	dev->stats.tx_dropped++;
	kfree_skb(skb);
	return NETDEV_TX_OK;
}

static void
ramips_eth_rx_hw(unsigned long ptr)
{
	struct net_device *dev = (struct net_device *) ptr;
	struct raeth_priv *priv = netdev_priv(dev);
	int rx;
	int max_rx = 16;

	while (max_rx) {
		struct sk_buff *rx_skb, *new_skb;

		rx = (ramips_fe_rr(RAMIPS_RX_CALC_IDX0) + 1) % NUM_RX_DESC;
		if (!(priv->rx[rx].rxd2 & RX_DMA_DONE))
			break;
		max_rx--;

		rx_skb = priv->rx_skb[rx];
		rx_skb->len = RX_DMA_PLEN0(priv->rx[rx].rxd2);
		rx_skb->dev = dev;
		rx_skb->protocol = eth_type_trans(rx_skb, dev);
		rx_skb->ip_summed = CHECKSUM_NONE;
		dev->stats.rx_packets++;
		dev->stats.rx_bytes += rx_skb->len;
		netif_rx(rx_skb);

		new_skb = netdev_alloc_skb(dev, MAX_RX_LENGTH + 2);
		priv->rx_skb[rx] = new_skb;
		BUG_ON(!new_skb);
		skb_reserve(new_skb, 2);
		priv->rx[rx].rxd1 = dma_map_single(NULL,
						   new_skb->data,
						   MAX_RX_LENGTH + 2,
						   DMA_FROM_DEVICE);
		priv->rx[rx].rxd2 &= ~RX_DMA_DONE;
		wmb();
		ramips_fe_wr(rx, RAMIPS_RX_CALC_IDX0);
	}

	if (max_rx == 0)
		tasklet_schedule(&priv->rx_tasklet);
	else
		ramips_fe_wr(ramips_fe_rr(RAMIPS_FE_INT_ENABLE) | RAMIPS_RX_DLY_INT,
			     RAMIPS_FE_INT_ENABLE);
}

static void
ramips_eth_tx_housekeeping(unsigned long ptr)
{
	struct net_device *dev = (struct net_device*)ptr;
	struct raeth_priv *priv = netdev_priv(dev);

	while ((priv->tx[priv->skb_free_idx].txd2 & TX_DMA_DONE) &&
	       (priv->tx_skb[priv->skb_free_idx])) {
		dev_kfree_skb_irq((struct sk_buff *) priv->tx_skb[priv->skb_free_idx]);
		priv->tx_skb[priv->skb_free_idx] = 0;
		priv->skb_free_idx++;
		if (priv->skb_free_idx >= NUM_TX_DESC)
			priv->skb_free_idx = 0;
	}

	ramips_fe_wr(ramips_fe_rr(RAMIPS_FE_INT_ENABLE) | RAMIPS_TX_DLY_INT,
		     RAMIPS_FE_INT_ENABLE);
}

static int
ramips_eth_set_mac_addr(struct net_device *dev, void *priv)
{
	unsigned char *mac = (unsigned char *) priv;

	if (netif_running(dev))
		return -EBUSY;

	memcpy(dev->dev_addr, ((struct sockaddr*)priv)->sa_data, dev->addr_len);
	ramips_fe_wr((mac[0] << 8) | mac[1], RAMIPS_GDMA1_MAC_ADRH);
	ramips_fe_wr(RAMIPS_GDMA1_MAC_ADRL,
		     (mac[2] << 24) | (mac[3] << 16) | (mac[4] << 8) | mac[5]);
	return 0;
}

static void
ramips_eth_timeout(struct net_device *dev)
{
	struct raeth_priv *priv = netdev_priv(dev);

	tasklet_schedule(&priv->tx_housekeeping_tasklet);
}

static irqreturn_t
ramips_eth_irq(int irq, void *dev)
{
	struct raeth_priv *priv = netdev_priv(dev);
	unsigned long fe_int = ramips_fe_rr(RAMIPS_FE_INT_STATUS);

	ramips_fe_wr(0xFFFFFFFF, RAMIPS_FE_INT_STATUS);

	if (fe_int & RAMIPS_RX_DLY_INT) {
		ramips_fe_wr(ramips_fe_rr(RAMIPS_FE_INT_ENABLE) & ~(RAMIPS_RX_DLY_INT),
			     RAMIPS_FE_INT_ENABLE);
		tasklet_schedule(&priv->rx_tasklet);
	}

	if (fe_int & RAMIPS_TX_DLY_INT)
		ramips_eth_tx_housekeeping((unsigned long)dev);

	return IRQ_HANDLED;
}

static int
ramips_eth_open(struct net_device *dev)
{
	struct raeth_priv *priv = netdev_priv(dev);
	int err;

	err = request_irq(dev->irq, ramips_eth_irq, IRQF_DISABLED,
			  dev->name, dev);
	if (err)
		return err;

	err = ramips_alloc_dma(dev);
	if (err)
		goto err_free_irq;

	ramips_setup_dma(dev);
	ramips_fe_wr((ramips_fe_rr(RAMIPS_PDMA_GLO_CFG) & 0xff) |
		(RAMIPS_TX_WB_DDONE | RAMIPS_RX_DMA_EN |
		RAMIPS_TX_DMA_EN | RAMIPS_PDMA_SIZE_4DWORDS),
		RAMIPS_PDMA_GLO_CFG);
	ramips_fe_wr((ramips_fe_rr(RAMIPS_FE_GLO_CFG) &
		~(RAMIPS_US_CYC_CNT_MASK << RAMIPS_US_CYC_CNT_SHIFT)) |
		((rt305x_sys_freq / RAMIPS_US_CYC_CNT_DIVISOR) << RAMIPS_US_CYC_CNT_SHIFT),
		RAMIPS_FE_GLO_CFG);

	tasklet_init(&priv->tx_housekeeping_tasklet, ramips_eth_tx_housekeeping,
		     (unsigned long)dev);
	tasklet_init(&priv->rx_tasklet, ramips_eth_rx_hw, (unsigned long)dev);

	ramips_fe_wr(RAMIPS_DELAY_INIT, RAMIPS_DLY_INT_CFG);
	ramips_fe_wr(RAMIPS_TX_DLY_INT | RAMIPS_RX_DLY_INT, RAMIPS_FE_INT_ENABLE);
	ramips_fe_wr(ramips_fe_rr(RAMIPS_GDMA1_FWD_CFG) &
		~(RAMIPS_GDM1_ICS_EN | RAMIPS_GDM1_TCS_EN | RAMIPS_GDM1_UCS_EN | 0xffff),
		RAMIPS_GDMA1_FWD_CFG);
	ramips_fe_wr(ramips_fe_rr(RAMIPS_CDMA_CSG_CFG) &
		~(RAMIPS_ICS_GEN_EN | RAMIPS_TCS_GEN_EN | RAMIPS_UCS_GEN_EN),
		RAMIPS_CDMA_CSG_CFG);
	ramips_fe_wr(RAMIPS_PSE_FQFC_CFG_INIT, RAMIPS_PSE_FQ_CFG);
	ramips_fe_wr(1, RAMIPS_FE_RST_GL);
	ramips_fe_wr(0, RAMIPS_FE_RST_GL);

	netif_start_queue(dev);
	return 0;

 err_free_irq:
	free_irq(dev->irq, dev);
	return err;
}

static int
ramips_eth_stop(struct net_device *dev)
{
	struct raeth_priv *priv = netdev_priv(dev);

	ramips_fe_wr(RAMIPS_PDMA_GLO_CFG, ramips_fe_rr(RAMIPS_PDMA_GLO_CFG) &
		~(RAMIPS_TX_WB_DDONE | RAMIPS_RX_DMA_EN | RAMIPS_TX_DMA_EN));
	free_irq(dev->irq, dev);
	netif_stop_queue(dev);
	tasklet_kill(&priv->tx_housekeeping_tasklet);
	tasklet_kill(&priv->rx_tasklet);
	ramips_cleanup_dma(dev);
	printk(KERN_DEBUG "ramips_eth: stopped\n");
	return 0;
}

static int __init
ramips_eth_probe(struct net_device *dev)
{
	struct raeth_priv *priv = netdev_priv(dev);
	struct sockaddr addr;

	BUG_ON(!priv->plat->reset_fe);
	priv->plat->reset_fe();
	net_srandom(jiffies);
	memcpy(addr.sa_data, priv->plat->mac, 6);
	ramips_eth_set_mac_addr(dev, &addr);

	ether_setup(dev);
	dev->open = ramips_eth_open;
	dev->stop = ramips_eth_stop;
	dev->hard_start_xmit = ramips_eth_hard_start_xmit;
	dev->set_mac_address = ramips_eth_set_mac_addr;
	dev->mtu = 1500;
	dev->tx_timeout = ramips_eth_timeout;
	dev->watchdog_timeo = TX_TIMEOUT;
	spin_lock_init(&priv->page_lock);

	return 0;
}

static int
ramips_eth_plat_probe(struct platform_device *plat)
{
	struct raeth_priv *priv;
	struct ramips_eth_platform_data *data = plat->dev.platform_data;
	struct resource *res;
	int err;

	if (!data) {
		dev_err(&plat->dev, "no platform data specified\n");
		return -EINVAL;
	}

	res = platform_get_resource(plat, IORESOURCE_MEM, 0);
	if (!res) {
		dev_err(&plat->dev, "no memory resource found\n");
		return -ENXIO;
	}

	ramips_fe_base = ioremap_nocache(res->start, res->end - res->start + 1);
	if (!ramips_fe_base)
		return -ENOMEM;

	ramips_dev = alloc_etherdev(sizeof(struct raeth_priv));
	if (!ramips_dev) {
		dev_err(&plat->dev, "alloc_etherdev failed\n");
		err = -ENOMEM;
		goto err_unmap;
	}

	strcpy(ramips_dev->name, "eth%d");
	ramips_dev->irq = platform_get_irq(plat, 0);
	if (ramips_dev->irq < 0) {
		dev_err(&plat->dev, "no IRQ resource found\n");
		err = -ENXIO;
		goto err_free_dev;
	}
	ramips_dev->addr_len = ETH_ALEN;
	ramips_dev->base_addr = (unsigned long)ramips_fe_base;
	ramips_dev->init = ramips_eth_probe;

	priv = netdev_priv(ramips_dev);
	priv->plat = data;

	err = register_netdev(ramips_dev);
	if (err) {
		dev_err(&plat->dev, "error bringing up device\n");
		goto err_free_dev;
	}

#ifdef CONFIG_RALINK_RT305X
	rt305x_esw_init();
#endif
	printk(KERN_DEBUG "ramips_eth: loaded\n");
	return 0;

 err_free_dev:
	kfree(ramips_dev);
 err_unmap:
	iounmap(ramips_fe_base);
	return err;
}

static int
ramips_eth_plat_remove(struct platform_device *plat)
{
	unregister_netdev(ramips_dev);
	free_netdev(ramips_dev);
	printk(KERN_DEBUG "ramips_eth: unloaded\n");
	return 0;
}

static struct platform_driver ramips_eth_driver = {
	.probe = ramips_eth_plat_probe,
	.remove = ramips_eth_plat_remove,
	.driver = {
		.name = "ramips_eth",
		.owner = THIS_MODULE,
	},
};

static int __init
ramips_eth_init(void)
{
	int ret = platform_driver_register(&ramips_eth_driver);
	if (ret)
		printk(KERN_ERR
		       "ramips_eth: Error registering platfom driver!\n");
	return ret;
}

static void __exit
ramips_eth_cleanup(void)
{
	platform_driver_unregister(&ramips_eth_driver);
}

module_init(ramips_eth_init);
module_exit(ramips_eth_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("John Crispin <blogic@openwrt.org>");
MODULE_DESCRIPTION("ethernet driver for ramips boards");

/*
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
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
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/pci.h>
#include <linux/init.h>
#include <linux/skbuff.h>
#include <linux/if_vlan.h>
#include <linux/if_ether.h>
#include <linux/platform_device.h>
#include <asm/uaccess.h>
#include <net/sock.h>
#include <asm/uaccess.h>

#include <eth.h>

#define TX_TIMEOUT (20 * HZ / 100)
#define	MAX_RX_LENGTH	1500

#ifdef CONFIG_RALINK_RT305X
#include "ramips_esw.c"
#endif

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

static int
ramips_alloc_dma(struct net_device *dev)
{
#define phys_to_bus(a)  (a & 0x1FFFFFFF)
	struct raeth_priv *priv = netdev_priv(dev);
	int i;

	priv->skb_free_idx = 0;

	/* setup tx ring */
	priv->tx = pci_alloc_consistent(NULL,
		NUM_TX_DESC * sizeof(struct ramips_tx_dma), &priv->phy_tx);
	for(i = 0; i < NUM_TX_DESC; i++)
	{
		memset(&priv->tx[i], 0, sizeof(struct ramips_tx_dma));
		priv->tx[i].txd2 |= TX_DMA_LSO | TX_DMA_DONE;
		priv->tx[i].txd4 &= (TX_DMA_QN_MASK | TX_DMA_PN_MASK);
		priv->tx[i].txd4 |= TX_DMA_QN(3) | TX_DMA_PN(1);
	}
	ramips_fe_wr(phys_to_bus(priv->phy_tx), RAMIPS_TX_BASE_PTR0);
	ramips_fe_wr(NUM_TX_DESC, RAMIPS_TX_MAX_CNT0);
	ramips_fe_wr(0, RAMIPS_TX_CTX_IDX0);
	ramips_fe_wr(RAMIPS_PST_DTX_IDX0, RAMIPS_PDMA_RST_CFG);

	/* setup rx ring */
	priv->rx = pci_alloc_consistent(NULL,
		NUM_RX_DESC * sizeof(struct ramips_rx_dma), &priv->phy_rx);
	memset(priv->rx, 0, sizeof(struct ramips_rx_dma) * NUM_RX_DESC);
	for(i = 0; i < NUM_RX_DESC; i++)
	{
		struct sk_buff *new_skb = dev_alloc_skb(MAX_RX_LENGTH + 2);
		BUG_ON(!new_skb);
		skb_reserve(new_skb, 2);
		priv->rx[i].rxd1 =
			dma_map_single(NULL, skb_put(new_skb, 2), MAX_RX_LENGTH + 2,
				PCI_DMA_FROMDEVICE);
		priv->rx[i].rxd2 |= RX_DMA_LSO;
		priv->rx_skb[i] = new_skb;
	}
	dma_cache_wback_inv((unsigned long)priv->rx,
		NUM_RX_DESC * (sizeof(struct ramips_rx_dma)));

	ramips_fe_wr(phys_to_bus(priv->phy_rx), RAMIPS_RX_BASE_PTR0);
	ramips_fe_wr(NUM_RX_DESC, RAMIPS_RX_MAX_CNT0);
	ramips_fe_wr((NUM_RX_DESC - 1), RAMIPS_RX_CALC_IDX0);
	ramips_fe_wr(RAMIPS_PST_DRX_IDX0, RAMIPS_PDMA_RST_CFG);

	return 0;
}

static int
ramips_eth_hard_start_xmit(struct sk_buff* skb, struct net_device *dev)
{
	struct raeth_priv *priv = netdev_priv(dev);
	unsigned long tx;
	unsigned int tx_next;

	if(priv->plat->min_pkt_len)
	{
		if(skb->len < priv->plat->min_pkt_len)
		 {
		     if(skb_padto(skb, priv->plat->min_pkt_len))
			 {
				 printk(KERN_ERR "ramips_eth: skb_padto failed\n");
				 kfree_skb(skb);
				 return 0;
			 }
		     skb_put(skb, priv->plat->min_pkt_len - skb->len);
		 }
	}
	dev->trans_start = jiffies;
	dma_cache_wback_inv((unsigned long)skb->data, skb->len);
	tx = ramips_fe_rr(RAMIPS_TX_CTX_IDX0);
	if(tx == NUM_TX_DESC - 1)
		tx_next = 0;
	else
		tx_next = tx + 1;
	if((priv->tx_skb[tx]== 0) && (priv->tx_skb[tx_next] == 0))
	{
		if(!(priv->tx[tx].txd2 & TX_DMA_DONE))
		{
			kfree_skb(skb);
			dev->stats.tx_dropped++;
			printk(KERN_ERR "%s: dropping\n", dev->name);
			return 0;
		}
		priv->tx[tx].txd1 = virt_to_phys(skb->data);
		priv->tx[tx].txd2 &= ~(TX_DMA_PLEN0_MASK | TX_DMA_DONE);
		priv->tx[tx].txd2 |= TX_DMA_PLEN0(skb->len);
		ramips_fe_wr((tx + 1) % NUM_TX_DESC, RAMIPS_TX_CTX_IDX0);
		dev->stats.tx_packets++;
		dev->stats.tx_bytes += skb->len;
		priv->tx_skb[tx] = skb;
		ramips_fe_wr((tx + 1) % NUM_TX_DESC, RAMIPS_TX_CTX_IDX0);
	} else {
		dev->stats.tx_dropped++;
		kfree_skb(skb);
	}
	return 0;
}

static void
ramips_eth_rx_hw(unsigned long ptr)
{
	struct net_device *dev = (struct net_device*)ptr;
	struct raeth_priv *priv = netdev_priv(dev);
	int rx;
	int max_rx = 16;

	while(max_rx)
	{
		struct sk_buff *rx_skb, *new_skb;

		rx = (ramips_fe_rr(RAMIPS_RX_CALC_IDX0) + 1) % NUM_RX_DESC;
		if(!(priv->rx[rx].rxd2 & RX_DMA_DONE))
			break;
		max_rx--;

		rx_skb = priv->rx_skb[rx];
		rx_skb->len = RX_DMA_PLEN0(priv->rx[rx].rxd2);
		rx_skb->tail = rx_skb->data + rx_skb->len;
		rx_skb->dev = dev;
		rx_skb->protocol = eth_type_trans(rx_skb, dev);
		rx_skb->ip_summed = CHECKSUM_NONE;
		dev->stats.rx_packets++;
		dev->stats.rx_bytes += rx_skb->len;
		netif_rx(rx_skb);

		new_skb = __dev_alloc_skb(MAX_RX_LENGTH + 2, GFP_DMA | GFP_ATOMIC);
		priv->rx_skb[rx] = new_skb;
		BUG_ON(!new_skb);
		skb_reserve(new_skb, 2);
		priv->rx[rx].rxd1 =
			dma_map_single(NULL, new_skb->data, MAX_RX_LENGTH + 2,
			PCI_DMA_FROMDEVICE);
		priv->rx[rx].rxd2 &= ~RX_DMA_DONE;
		dma_cache_wback_inv((unsigned long)&priv->rx[rx],
			sizeof(struct ramips_rx_dma));
		ramips_fe_wr(rx, RAMIPS_RX_CALC_IDX0);
	}
	if(max_rx == 0)
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

	while((priv->tx[priv->skb_free_idx].txd2 & TX_DMA_DONE) &&
		(priv->tx_skb[priv->skb_free_idx]))
	{
		dev_kfree_skb_irq((struct sk_buff*)priv->tx_skb[priv->skb_free_idx]);
		priv->tx_skb[priv->skb_free_idx] = 0;
		priv->skb_free_idx++;
		if(priv->skb_free_idx >= NUM_TX_DESC)
			priv->skb_free_idx = 0;
	}
	ramips_fe_wr(ramips_fe_rr(RAMIPS_FE_INT_ENABLE) | RAMIPS_TX_DLY_INT,
		RAMIPS_FE_INT_ENABLE);
}

static int
ramips_eth_set_mac_addr(struct net_device *dev, void *priv)
{
	unsigned char *mac = (unsigned char*)priv;

	if(netif_running(dev))
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

	if(fe_int & RAMIPS_RX_DLY_INT)
	{
		ramips_fe_wr(ramips_fe_rr(RAMIPS_FE_INT_ENABLE) & ~(RAMIPS_RX_DLY_INT),
			RAMIPS_FE_INT_ENABLE);
		tasklet_schedule(&priv->rx_tasklet);
	}
	if(fe_int & RAMIPS_TX_DLY_INT)
		tasklet_schedule(&priv->tx_housekeeping_tasklet);
	ramips_fe_wr(0xFFFFFFFF, RAMIPS_FE_INT_STATUS);
	return IRQ_HANDLED;
}

static int
ramips_eth_open(struct net_device *dev)
{
	struct raeth_priv *priv = netdev_priv(dev);

	ramips_alloc_dma(dev);
	ramips_fe_wr((ramips_fe_rr(RAMIPS_PDMA_GLO_CFG) & 0xff) |
		(RAMIPS_TX_WB_DDONE | RAMIPS_RX_DMA_EN |
		RAMIPS_TX_DMA_EN | RAMIPS_PDMA_SIZE_4DWORDS),
		RAMIPS_PDMA_GLO_CFG);
	ramips_fe_wr((ramips_fe_rr(RAMIPS_FE_GLO_CFG) &
		~(RAMIPS_US_CYC_CNT_MASK << RAMIPS_US_CYC_CNT_SHIFT)) |
		((rt305x_sys_freq / RAMIPS_US_CYC_CNT_DIVISOR) << RAMIPS_US_CYC_CNT_SHIFT),
		RAMIPS_FE_GLO_CFG);
	request_irq(dev->irq, ramips_eth_irq, IRQF_DISABLED, dev->name, dev);
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
	pci_free_consistent(NULL, NUM_TX_DESC * sizeof(struct ramips_tx_dma),
		priv->tx, priv->phy_tx);
	pci_free_consistent(NULL, NUM_RX_DESC * sizeof(struct ramips_rx_dma),
		priv->rx, priv->phy_rx);
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
	dev->mtu = MAX_RX_LENGTH;
	dev->tx_timeout = ramips_eth_timeout;
	dev->watchdog_timeo = TX_TIMEOUT;
	return 0;
}

static int
ramips_eth_plat_probe(struct platform_device *plat)
{
	struct raeth_priv *priv;
	struct ramips_eth_platform_data *data = plat->dev.platform_data;
	ramips_fe_base = ioremap_nocache(data->base_addr, PAGE_SIZE);
	if(!ramips_fe_base)
		return -ENOMEM;
	ramips_dev = alloc_etherdev(sizeof(struct raeth_priv));
	if(!ramips_dev)
		return -ENOMEM;
	strcpy(ramips_dev->name, "eth%d");
	ramips_dev->irq = data->irq;
	ramips_dev->addr_len = ETH_ALEN;
	ramips_dev->base_addr = (unsigned long)ramips_fe_base;
	ramips_dev->init = ramips_eth_probe;
	priv = (struct raeth_priv*)netdev_priv(ramips_dev);
	priv->plat = data;
	if(register_netdev(ramips_dev))
	{
		printk(KERN_ERR "ramips_eth: error bringing up device\n");
		return -ENXIO;
	}
#ifdef CONFIG_RALINK_RT305X
	rt305x_esw_init();
#endif
	printk(KERN_DEBUG "ramips_eth: loaded\n");
	return 0;
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

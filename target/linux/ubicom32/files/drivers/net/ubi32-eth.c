/*
 * drivers/net/ubi32-eth.c
 *   Ubicom32 ethernet TIO interface driver.
 *
 * (C) Copyright 2009, Ubicom, Inc.
 *
 * This file is part of the Ubicom32 Linux Kernel Port.
 *
 * The Ubicom32 Linux Kernel Port is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 2 of the
 * License, or (at your option) any later version.
 *
 * The Ubicom32 Linux Kernel Port is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 * the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Ubicom32 Linux Kernel Port.  If not,
 * see <http://www.gnu.org/licenses/>.
 *
 * Ubicom32 implementation derived from (with many thanks):
 *   arch/m68knommu
 *   arch/blackfin
 *   arch/parisc
 */
/*
 * ubi32_eth.c
 * Ethernet driver for Ip5k/Ip7K
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/moduleparam.h>

#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/interrupt.h>

#include <linux/in.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/mii.h>
#include <linux/if_vlan.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/skbuff.h>
#include <asm/checksum.h>
#include <asm/ip5000.h>
#include <asm/devtree.h>
#include <asm/system.h>

#define UBICOM32_USE_NAPI	/* define this to use NAPI instead of tasklet */
//#define UBICOM32_USE_POLLING	/* define this to use polling instead of interrupt */
#include "ubi32-eth.h"

/*
 * TODO:
 * mac address from flash
 * multicast filter
 * ethtool support
 * sysfs support
 * skb->nrfrag support
 * ioctl
 * monitor phy status
 */

extern int ubi32_ocm_skbuf_max, ubi32_ocm_skbuf, ubi32_ddr_skbuf;
static const char *eth_if_name[UBI32_ETH_NUM_OF_DEVICES] =
	{"eth_lan", "eth_wan"};
static struct net_device *ubi32_eth_devices[UBI32_ETH_NUM_OF_DEVICES] =
	{NULL, NULL};
static u8_t mac_addr[UBI32_ETH_NUM_OF_DEVICES][ETH_ALEN] = {
	{0x00, 0x03, 0x64, 'l', 'a', 'n'},
	{0x00, 0x03, 0x64, 'w', 'a', 'n'}};

#if (defined(CONFIG_ZONE_DMA) && defined(CONFIG_UBICOM32_OCM_FOR_SKB))
static inline struct sk_buff *ubi32_alloc_skb_ocm(struct net_device *dev, unsigned int length)
{
	return __dev_alloc_skb(length, GFP_ATOMIC | __GFP_NOWARN | __GFP_NORETRY | GFP_DMA);
}
#endif

static inline struct sk_buff *ubi32_alloc_skb(struct net_device *dev, unsigned int length)
{
	return __dev_alloc_skb(length, GFP_ATOMIC | __GFP_NOWARN);
}

static void ubi32_eth_vp_rxtx_enable(struct net_device *dev)
{
	struct ubi32_eth_private *priv = netdev_priv(dev);
	priv->regs->command = UBI32_ETH_VP_CMD_RX_ENABLE | UBI32_ETH_VP_CMD_TX_ENABLE;
	priv->regs->int_mask = (UBI32_ETH_VP_INT_RX | UBI32_ETH_VP_INT_TX);
	ubicom32_set_interrupt(priv->vp_int_bit);
}

static void ubi32_eth_vp_rxtx_stop(struct net_device *dev)
{
	struct ubi32_eth_private *priv = netdev_priv(dev);
	priv->regs->command = 0;
	priv->regs->int_mask = 0;
	ubicom32_set_interrupt(priv->vp_int_bit);

	/* Wait for graceful shutdown */
	while (priv->regs->status & (UBI32_ETH_VP_STATUS_RX_STATE | UBI32_ETH_VP_STATUS_TX_STATE));
}

/*
 * ubi32_eth_tx_done()
 */
static int ubi32_eth_tx_done(struct net_device *dev)
{
	struct ubi32_eth_private *priv;
	struct sk_buff *skb;
	volatile void *pdata;
	struct ubi32_eth_dma_desc *desc;
	u32_t 	count = 0;

	priv = netdev_priv(dev);

	priv->regs->int_status &= ~UBI32_ETH_VP_INT_TX;
	while (priv->tx_tail != priv->regs->tx_out) {
		pdata = priv->regs->tx_dma_ring[priv->tx_tail];
		BUG_ON(pdata == NULL);

		skb = container_of((void *)pdata, struct sk_buff, cb);
		desc = (struct ubi32_eth_dma_desc *)pdata;
		if (unlikely(!(desc->status & UBI32_ETH_VP_TX_OK))) {
			dev->stats.tx_errors++;
		} else {
			dev->stats.tx_packets++;
			dev->stats.tx_bytes += skb->len;
		}
		dev_kfree_skb_any(skb);
		priv->regs->tx_dma_ring[priv->tx_tail] = NULL;
		priv->tx_tail = (priv->tx_tail + 1) & TX_DMA_RING_MASK;
		count++;
	}

	if (unlikely(priv->regs->status & UBI32_ETH_VP_STATUS_TX_Q_FULL)) {
		spin_lock(&priv->lock);
		if (priv->regs->status & UBI32_ETH_VP_STATUS_TX_Q_FULL) {
			priv->regs->status &= ~UBI32_ETH_VP_STATUS_TX_Q_FULL;
			netif_wake_queue(dev);
		}
		spin_unlock(&priv->lock);
	}
	return count;
}

/*
 * ubi32_eth_receive()
 *	To avoid locking overhead, this is called only
 *	by tasklet when not using NAPI, or
 *	by NAPI poll when using NAPI.
 *	return number of frames processed
 */
static int ubi32_eth_receive(struct net_device *dev, int quota)
{
	struct ubi32_eth_private *priv = netdev_priv(dev);
	unsigned short rx_in = priv->regs->rx_in;
	struct sk_buff *skb;
	struct ubi32_eth_dma_desc *desc = NULL;
	volatile void *pdata;

	int extra_reserve_adj;
	int extra_alloc = UBI32_ETH_RESERVE_SPACE + UBI32_ETH_TRASHED_MEMORY;
	int replenish_cnt, count = 0;
	int replenish_max = RX_DMA_MAX_QUEUE_SIZE;
#if (defined(CONFIG_ZONE_DMA) && defined(CONFIG_UBICOM32_OCM_FOR_SKB))
	if (likely(dev == ubi32_eth_devices[0]))
		replenish_max = min(ubi32_ocm_skbuf_max, RX_DMA_MAX_QUEUE_SIZE);;
#endif

	if (unlikely(rx_in == priv->regs->rx_out))
		priv->vp_stats.rx_q_full_cnt++;

	priv->regs->int_status &= ~UBI32_ETH_VP_INT_RX;
	while (priv->rx_tail != priv->regs->rx_out) {
		if (unlikely(count == quota)) {
			/* There is still frame pending to be processed */
			priv->vp_stats.rx_throttle++;
			break;
		}

		pdata = priv->regs->rx_dma_ring[priv->rx_tail];
		BUG_ON(pdata == NULL);

		desc = (struct ubi32_eth_dma_desc *)pdata;
		skb = container_of((void *)pdata, struct sk_buff, cb);
		count++;
		priv->regs->rx_dma_ring[priv->rx_tail] = NULL;
		priv->rx_tail = ((priv->rx_tail + 1) & RX_DMA_RING_MASK);

		/*
		 * Check only RX_OK bit here.
		 * The rest of status word is used as timestamp
		 */
		if (unlikely(!(desc->status & UBI32_ETH_VP_RX_OK))) {
			dev->stats.rx_errors++;
			dev_kfree_skb_any(skb);
			continue;
		}

		skb_put(skb, desc->data_len);
		skb->dev = dev;
		skb->protocol = eth_type_trans(skb, dev);
		skb->ip_summed = CHECKSUM_NONE;
		dev->stats.rx_bytes += skb->len;
		dev->stats.rx_packets++;
#ifndef UBICOM32_USE_NAPI
		netif_rx(skb);
#else
		netif_receive_skb(skb);
#endif
	}

	/* fill in more descripor for VP*/
	replenish_cnt =  replenish_max -
		((RX_DMA_RING_SIZE + rx_in - priv->rx_tail) & RX_DMA_RING_MASK);
	if (replenish_cnt > 0) {
#if (defined(CONFIG_ZONE_DMA) && defined(CONFIG_UBICOM32_OCM_FOR_SKB))
		/*
		 * black magic for perforamnce:
		 *   Try to allocate skb from OCM only for first Ethernet I/F.
		 *   Also limit number of RX buffers to 21 due to limited OCM.
		 */
		if (likely(dev == ubi32_eth_devices[0])) {
			do {
				skb = ubi32_alloc_skb_ocm(dev, RX_BUF_SIZE + extra_alloc);
				if (!skb) {
					break;
				}
				/* set up dma descriptor */
				ubi32_ocm_skbuf++;
				desc = (struct ubi32_eth_dma_desc *)skb->cb;
				extra_reserve_adj =
					((u32)skb->data + UBI32_ETH_RESERVE_SPACE + ETH_HLEN) &
					(CACHE_LINE_SIZE - 1);
				skb_reserve(skb, UBI32_ETH_RESERVE_SPACE - extra_reserve_adj);
				desc->data_pointer = skb->data;
				desc->buffer_len = RX_BUF_SIZE + UBI32_ETH_TRASHED_MEMORY;
				desc->data_len = 0;
				desc->status = 0;
				priv->regs->rx_dma_ring[rx_in] = desc;
				rx_in = (rx_in + 1) & RX_DMA_RING_MASK;
			} while (--replenish_cnt > 0);
		}
#endif

		while (replenish_cnt-- > 0) {
			skb = ubi32_alloc_skb(dev, RX_BUF_SIZE + extra_alloc);
			if (!skb) {
				priv->vp_stats.rx_alloc_err++;
				break;
			}
			/* set up dma descriptor */
			ubi32_ddr_skbuf++;
			desc = (struct ubi32_eth_dma_desc *)skb->cb;
			extra_reserve_adj =
				((u32)skb->data + UBI32_ETH_RESERVE_SPACE + ETH_HLEN) &
				(CACHE_LINE_SIZE - 1);
			skb_reserve(skb, UBI32_ETH_RESERVE_SPACE - extra_reserve_adj);
			desc->data_pointer = skb->data;
			desc->buffer_len = RX_BUF_SIZE + UBI32_ETH_TRASHED_MEMORY;
			desc->data_len = 0;
			desc->status = 0;
			priv->regs->rx_dma_ring[rx_in] = desc;
			rx_in = (rx_in + 1) & RX_DMA_RING_MASK;
		}

		wmb();
		priv->regs->rx_in = rx_in;
		ubicom32_set_interrupt(priv->vp_int_bit);
	}

	if (likely(count > 0)) {
		dev->last_rx = jiffies;
	}
	return count;
}

#ifdef UBICOM32_USE_NAPI
static int ubi32_eth_napi_poll(struct napi_struct *napi, int budget)
{
	struct ubi32_eth_private *priv = container_of(napi, struct ubi32_eth_private, napi);
	struct net_device *dev = priv->dev;
	u32_t count;

	if (priv->tx_tail != priv->regs->tx_out) {
                ubi32_eth_tx_done(dev);
        }

	count = ubi32_eth_receive(dev, budget);

	if (count < budget) {
		napi_complete(napi);
		priv->regs->int_mask |= (UBI32_ETH_VP_INT_RX | UBI32_ETH_VP_INT_TX);
		if ((priv->rx_tail != priv->regs->rx_out) || (priv->tx_tail != priv->regs->tx_out)) {
			if (napi_reschedule(napi)) {
				priv->regs->int_mask = 0;
			}
		}
	}
	return count;
}

#else
static void ubi32_eth_do_tasklet(unsigned long arg)
{
	struct net_device *dev = (struct net_device *)arg;
	struct ubi32_eth_private *priv = netdev_priv(dev);

	if (priv->tx_tail != priv->regs->tx_out) {
		ubi32_eth_tx_done(dev);
	}

	/* always call receive to process new RX frame as well as replenish RX buffers */
	ubi32_eth_receive(dev, UBI32_RX_BOUND);

	priv->regs->int_mask |= (UBI32_ETH_VP_INT_RX | UBI32_ETH_VP_INT_TX);
	if ((priv->rx_tail != priv->regs->rx_out) || (priv->tx_tail != priv->regs->tx_out)) {
		priv->regs->int_mask = 0;
		tasklet_schedule(&priv->tsk);
	}
}
#endif

#if defined(UBICOM32_USE_POLLING)
static struct timer_list eth_poll_timer;

static void ubi32_eth_poll(unsigned long arg)
{
	struct net_device *dev;
	struct ubi32_eth_private *priv;
	int i;

	for (i = 0; i < UBI32_ETH_NUM_OF_DEVICES; i++) {
		dev = ubi32_eth_devices[i];
		if (dev && (dev->flags & IFF_UP)) {
			priv = netdev_priv(dev);
#ifdef UBICOM32_USE_NAPI
			napi_schedule(&priv->napi);
#else
			tasklet_schedule(&priv->tsk);
#endif
		}
	}

	eth_poll_timer.expires = jiffies + 2;
	add_timer(&eth_poll_timer);
}

#else
static irqreturn_t ubi32_eth_interrupt(int irq, void *dev_id)
{
	struct ubi32_eth_private *priv;

	struct net_device *dev = (struct net_device *)dev_id;
	BUG_ON(irq != dev->irq);

	priv = netdev_priv(dev);
	if (unlikely(!(priv->regs->int_status & priv->regs->int_mask))) {
		return IRQ_NONE;
	}

	/*
	 * Disable port interrupt
	 */
#ifdef UBICOM32_USE_NAPI
	if (napi_schedule_prep(&priv->napi)) {
		priv->regs->int_mask = 0;
		__napi_schedule(&priv->napi);
	}
#else
	priv->regs->int_mask = 0;
	tasklet_schedule(&priv->tsk);
#endif
	return IRQ_HANDLED;
}
#endif

/*
 * ubi32_eth_open
 */
static int ubi32_eth_open(struct net_device *dev)
{
	struct ubi32_eth_private *priv = netdev_priv(dev);
	int err;

	printk(KERN_INFO "eth open %s\n",dev->name);
#ifndef UBICOM32_USE_POLLING
	/* request_region() */
	err = request_irq(dev->irq, ubi32_eth_interrupt, IRQF_DISABLED, dev->name, dev);
	if (err) {
		printk(KERN_WARNING "fail to request_irq %d\n",err);
		 return -ENODEV;
	}
#endif
#ifdef  UBICOM32_USE_NAPI
	napi_enable(&priv->napi);
#else
	tasklet_init(&priv->tsk, ubi32_eth_do_tasklet, (unsigned long)dev);
#endif

	/* call receive to supply RX buffers */
	ubi32_eth_receive(dev, RX_DMA_MAX_QUEUE_SIZE);

	/* check phy status and call netif_carrier_on */
	ubi32_eth_vp_rxtx_enable(dev);
	netif_start_queue(dev);
	return 0;
}

static int ubi32_eth_close(struct net_device *dev)
{
	struct ubi32_eth_private *priv = netdev_priv(dev);
	volatile void *pdata;
	struct sk_buff *skb;

#ifndef UBICOM32_USE_POLLING
	free_irq(dev->irq, dev);
#endif
	netif_stop_queue(dev); /* can't transmit any more */
#ifdef UBICOM32_USE_NAPI
	napi_disable(&priv->napi);
#else
	tasklet_kill(&priv->tsk);
#endif
	ubi32_eth_vp_rxtx_stop(dev);

	/*
	 * RX clean up
	 */
	while (priv->rx_tail != priv->regs->rx_in) {
		pdata = priv->regs->rx_dma_ring[priv->rx_tail];
		skb = container_of((void *)pdata, struct sk_buff, cb);
		priv->regs->rx_dma_ring[priv->rx_tail] = NULL;
		dev_kfree_skb_any(skb);
		priv->rx_tail = ((priv->rx_tail + 1) & RX_DMA_RING_MASK);
	}
	priv->regs->rx_in = 0;
	priv->regs->rx_out = priv->regs->rx_in;
	priv->rx_tail = priv->regs->rx_in;

	/*
	 * TX clean up
	 */
	BUG_ON(priv->regs->tx_out != priv->regs->tx_in);
	ubi32_eth_tx_done(dev);
	BUG_ON(priv->tx_tail != priv->regs->tx_in);
	priv->regs->tx_in = 0;
	priv->regs->tx_out = priv->regs->tx_in;
	priv->tx_tail = priv->regs->tx_in;

	return 0;
}

/*
 * ubi32_eth_set_config
 */
static int ubi32_eth_set_config(struct net_device *dev, struct ifmap *map)
{
	/* if must to down to config it */
	printk(KERN_INFO "set_config %x\n", dev->flags);
	if (dev->flags & IFF_UP)
		return -EBUSY;

	/* I/O and IRQ can not be changed */
	if (map->base_addr != dev->base_addr) {
		printk(KERN_WARNING "%s: Can't change I/O address\n", dev->name);
		return -EOPNOTSUPP;
	}

#ifndef UBICOM32_USE_POLLING
	if (map->irq != dev->irq) {
		printk(KERN_WARNING "%s: Can't change IRQ\n", dev->name);
		return -EOPNOTSUPP;
	}
#endif

	/* ignore other fields */
	return 0;
}

static int ubi32_eth_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
	struct ubi32_eth_private *priv = netdev_priv(dev);
	struct ubi32_eth_dma_desc *desc = NULL;
	unsigned short space, tx_in;

	tx_in = priv->regs->tx_in;

	dev->trans_start = jiffies; /* save the timestamp */
	space = TX_DMA_RING_MASK - ((TX_DMA_RING_SIZE + tx_in - priv->tx_tail) & TX_DMA_RING_MASK);

	if (unlikely(space == 0)) {
		if (!(priv->regs->status & UBI32_ETH_VP_STATUS_TX_Q_FULL)) {
			spin_lock(&priv->lock);
			if (!(priv->regs->status & UBI32_ETH_VP_STATUS_TX_Q_FULL)) {
				priv->regs->status |= UBI32_ETH_VP_STATUS_TX_Q_FULL;
				priv->vp_stats.tx_q_full_cnt++;
				netif_stop_queue(dev);
			}
			spin_unlock(&priv->lock);
		}

		/* give both HW and this driver an extra trigger */
		priv->regs->int_mask |= UBI32_ETH_VP_INT_TX;
#ifndef UBICOM32_USE_POLLING
		ubicom32_set_interrupt(dev->irq);
#endif
		ubicom32_set_interrupt(priv->vp_int_bit);

		return NETDEV_TX_BUSY;
	}

	/*still have room */
	desc = (struct ubi32_eth_dma_desc *)skb->cb;
	desc->data_pointer = skb->data;
	desc->data_len = skb->len;
	priv->regs->tx_dma_ring[tx_in] = desc;
	tx_in = ((tx_in + 1) & TX_DMA_RING_MASK);
	wmb();
	priv->regs->tx_in = tx_in;
	/* kick the HRT */
	ubicom32_set_interrupt(priv->vp_int_bit);

	return NETDEV_TX_OK;
}

/*
 * Deal with a transmit timeout.
 */
static void ubi32_eth_tx_timeout (struct net_device *dev)
{
	struct ubi32_eth_private *priv = netdev_priv(dev);
	dev->stats.tx_errors++;
	priv->regs->int_mask |= UBI32_ETH_VP_INT_TX;
#ifndef UBICOM32_USE_POLLING
	ubicom32_set_interrupt(dev->irq);
#endif
	ubicom32_set_interrupt(priv->vp_int_bit);
}

static int ubi32_eth_ioctl(struct net_device *dev, struct ifreq *rq, int cmd)
{
	struct ubi32_eth_private *priv = netdev_priv(dev);
	struct mii_ioctl_data *data = if_mii(rq);

	printk(KERN_INFO "ioctl %s, %d\n", dev->name, cmd);
	switch (cmd) {
	case SIOCGMIIPHY:
		data->phy_id = 0;
		break;

	case SIOCGMIIREG:
		if ((data->reg_num & 0x1F) == MII_BMCR) {
			/* Make up MII control register value from what we know */
			data->val_out = 0x0000
			| ((priv->regs->status & UBI32_ETH_VP_STATUS_DUPLEX)
					? BMCR_FULLDPLX : 0)
			| ((priv->regs->status & UBI32_ETH_VP_STATUS_SPEED100)
					? BMCR_SPEED100 : 0)
			| ((priv->regs->status & UBI32_ETH_VP_STATUS_SPEED1000)
					? BMCR_SPEED1000 : 0);
		} else if ((data->reg_num & 0x1F) == MII_BMSR) {
			/* Make up MII status register value from what we know */
			data->val_out =
			(BMSR_100FULL|BMSR_100HALF|BMSR_10FULL|BMSR_10HALF)
			| ((priv->regs->status & UBI32_ETH_VP_STATUS_LINK)
					? BMSR_LSTATUS : 0);
		} else {
			return -EIO;
		}
		break;

	case SIOCSMIIREG:
		return -EOPNOTSUPP;
		break;

	default:
		return -EOPNOTSUPP;
	}

	return 0;
}

/*
 * Return statistics to the caller
 */
static struct net_device_stats *ubi32_eth_get_stats(struct net_device *dev)
{
	return &dev->stats;
}


static int ubi32_eth_change_mtu(struct net_device *dev, int new_mtu)
{
	struct ubi32_eth_private *priv = netdev_priv(dev);
	unsigned long flags;

	if ((new_mtu < 68) || (new_mtu > 1500))
		return -EINVAL;

	spin_lock_irqsave(&priv->lock, flags);
	dev->mtu = new_mtu;
	spin_unlock_irqrestore(&priv->lock, flags);
	printk(KERN_INFO "set mtu to %d", new_mtu);
	return 0;
}

/*
 * ubi32_eth_cleanup: unload the module
 */
void ubi32_eth_cleanup(void)
{
	struct ubi32_eth_private *priv;
	struct net_device *dev;
	int i;

	for (i = 0; i < UBI32_ETH_NUM_OF_DEVICES; i++) {
		dev = ubi32_eth_devices[i];
		if (dev) {
			priv = netdev_priv(dev);
			kfree(priv->regs->tx_dma_ring);
			unregister_netdev(dev);
			free_netdev(dev);
			ubi32_eth_devices[i] = NULL;
		}
	}
}

	static const struct net_device_ops ubi32_netdev_ops = {
		.ndo_open		= ubi32_eth_open,
		.ndo_stop		= ubi32_eth_close,
		.ndo_start_xmit		= ubi32_eth_start_xmit,
		.ndo_tx_timeout		= ubi32_eth_tx_timeout,
		.ndo_do_ioctl		= ubi32_eth_ioctl,
		.ndo_change_mtu		= ubi32_eth_change_mtu,
		.ndo_set_config		= ubi32_eth_set_config,
		.ndo_get_stats		= ubi32_eth_get_stats,
		.ndo_validate_addr	= eth_validate_addr,
		.ndo_set_mac_address	= eth_mac_addr,
	};

int ubi32_eth_init_module(void)
{
	struct ethtionode *eth_node;
	struct net_device *dev;
	struct ubi32_eth_private *priv;
	int i, err;

	/*
	 * Device allocation.
	 */
	err = 0;
	for (i = 0; i < UBI32_ETH_NUM_OF_DEVICES; i++) {
		/*
		 * See if the eth_vp is in the device tree.
		 */
		eth_node = (struct ethtionode *)devtree_find_node(eth_if_name[i]);
		if (!eth_node) {
			printk(KERN_INFO "%s does not exist\n", eth_if_name[i]);
			continue;
		}

		eth_node->tx_dma_ring = (struct ubi32_eth_dma_desc **)kmalloc(
				sizeof(struct ubi32_eth_dma_desc *) *
				(TX_DMA_RING_SIZE + RX_DMA_RING_SIZE),
				GFP_ATOMIC | __GFP_NOWARN | __GFP_NORETRY | GFP_DMA);

		if (eth_node->tx_dma_ring == NULL) {
			eth_node->tx_dma_ring = (struct ubi32_eth_dma_desc **)kmalloc(
				sizeof(struct ubi32_eth_dma_desc *) *
				(TX_DMA_RING_SIZE + RX_DMA_RING_SIZE), GFP_KERNEL);
			printk(KERN_INFO "fail to allocate from OCM\n");
		}

		if (!eth_node->tx_dma_ring) {
			err = -ENOMEM;
			break;
		}
		eth_node->rx_dma_ring = eth_node->tx_dma_ring + TX_DMA_RING_SIZE;
		eth_node->tx_sz = TX_DMA_RING_SIZE - 1;
		eth_node->rx_sz = RX_DMA_RING_SIZE - 1;

		dev = alloc_etherdev(sizeof(struct ubi32_eth_private));
		if (!dev) {
			kfree(eth_node->tx_dma_ring);
			err = -ENOMEM;
			break;
		}
		priv = netdev_priv(dev);
		priv->dev = dev;

		/*
		 * This just fill in some default Ubicom MAC address
		 */
		memcpy(dev->dev_addr, mac_addr[i], ETH_ALEN);
		memset(dev->broadcast, 0xff, ETH_ALEN);

		priv->regs = eth_node;
		priv->regs->command = 0;
		priv->regs->int_mask = 0;
		priv->regs->int_status = 0;
		priv->regs->tx_out = 0;
		priv->regs->rx_out = 0;
		priv->regs->tx_in = 0;
		priv->regs->rx_in = 0;
		priv->rx_tail = 0;
		priv->tx_tail = 0;

		priv->vp_int_bit = eth_node->dn.sendirq;
		dev->irq = eth_node->dn.recvirq;

		spin_lock_init(&priv->lock);

		dev->netdev_ops = &ubi32_netdev_ops;

		dev->watchdog_timeo	= UBI32_ETH_VP_TX_TIMEOUT;
#ifdef UBICOM32_USE_NAPI
		netif_napi_add(dev, &priv->napi, ubi32_eth_napi_poll, UBI32_ETH_NAPI_WEIGHT);
#endif
		err = register_netdev(dev);
		if (err) {
			printk(KERN_WARNING "Failed to register netdev %s\n", eth_if_name[i]);
			//release_region();
			free_netdev(dev);
			kfree(eth_node->tx_dma_ring);
			break;
		}

		ubi32_eth_devices[i] = dev;
		printk(KERN_INFO "%s vp_base:0x%p, tio_int:%d irq:%d feature:0x%lx\n",
			dev->name, priv->regs, eth_node->dn.sendirq, dev->irq, dev->features);
	}

	if (err) {
		ubi32_eth_cleanup();
		return err;
	}

	if (!ubi32_eth_devices[0] && !ubi32_eth_devices[1]) {
		return -ENODEV;
	}

#if defined(UBICOM32_USE_POLLING)
	init_timer(&eth_poll_timer);
	eth_poll_timer.function = ubi32_eth_poll;
	eth_poll_timer.data = (unsigned long)0;
	eth_poll_timer.expires = jiffies + 2;
	add_timer(&eth_poll_timer);
#endif

	return 0;
}

module_init(ubi32_eth_init_module);
module_exit(ubi32_eth_cleanup);

MODULE_AUTHOR("Kan Yan, Greg Ren");
MODULE_LICENSE("GPL");

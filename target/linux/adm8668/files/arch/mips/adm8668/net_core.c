/*
 * originally drivers/net/tulip_core.c
 *   Copyright 2000,2001  The Linux Kernel Team
 *   Written/copyright 1994-2001 by Donald Becker.
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 */

#define DRV_NAME	"tulip"
#define DRV_VERSION	"1.1.15-NAPI" /* Keep at least for test */
#define DRV_RELDATE	"Feb 27, 2007"

#include "net.h"

static char version[] __devinitdata =
	"ADM8668net driver version " DRV_VERSION " (" DRV_RELDATE ")\n";

#define	MAX_UNITS	2

/*
  Set the bus performance register.
	Typical: Set 16 longword cache alignment, no burst limit.
	Cache alignment bits 15:14	     Burst length 13:8
		0000	No alignment		0x00000000 unlimited	0800 8 longwords
		4000	8  longwords		0100 1 longword		1000 16 longwords
		8000	16 longwords		0200 2 longwords	2000 32 longwords
		C000	32  longwords		0400 4 longwords
	Warning: many older 486 systems are broken and require setting 0x00A04800
	   8 longword cache alignment, 8 longword burst.
	ToDo: Non-Intel setting could be better.
*/

//static int csr0 = 0x00200000 | 0x4000;
static int csr0 = 0;

/* Operational parameters that usually are not changed. */
/* Time in jiffies before concluding the transmitter is hung. */
#define TX_TIMEOUT  (4*HZ)

MODULE_AUTHOR("Scott Nicholas <neutronscott@scottn.us>");
MODULE_DESCRIPTION("ADM8668 new ethernet driver.");
MODULE_LICENSE("GPL");
MODULE_VERSION(DRV_VERSION);

#ifdef TULIP_DEBUG
int tulip_debug = TULIP_DEBUG;
#else
int tulip_debug = 1;
#endif

static void tulip_tx_timeout(struct net_device *dev);
static void tulip_init_ring(struct net_device *dev);
static void tulip_free_ring(struct net_device *dev);
static netdev_tx_t tulip_start_xmit(struct sk_buff *skb,
					  struct net_device *dev);
static int tulip_open(struct net_device *dev);
static int tulip_close(struct net_device *dev);
static void tulip_up(struct net_device *dev);
static void tulip_down(struct net_device *dev);
static struct net_device_stats *tulip_get_stats(struct net_device *dev);
//static int private_ioctl(struct net_device *dev, struct ifreq *rq, int cmd);
static void set_rx_mode(struct net_device *dev);
#ifdef CONFIG_NET_POLL_CONTROLLER
static void poll_tulip(struct net_device *dev);
#endif

static void tulip_up(struct net_device *dev)
{
	struct tulip_private *tp = netdev_priv(dev);
	void __iomem *ioaddr = tp->base_addr;

	napi_enable(&tp->napi);

	/* Reset the chip, holding bit 0 set at least 50 PCI cycles. */
	iowrite32(0x00000001, ioaddr + CSR0);

	/* Deassert reset.
	   Wait the specified 50 PCI cycles after a reset by initializing
	   Tx and Rx queues and the address filter list. */
	iowrite32(tp->csr0, ioaddr + CSR0);

	if (tulip_debug > 1)
		printk(KERN_DEBUG "%s: tulip_up(), irq==%d\n",
		       dev->name, dev->irq);

	iowrite32(tp->rx_ring_dma, ioaddr + CSR3);
	iowrite32(tp->tx_ring_dma, ioaddr + CSR4);
	tp->cur_rx = tp->cur_tx = 0;
	tp->dirty_rx = tp->dirty_tx = 0;

	/* set mac address */
	iowrite32(get_unaligned_le32(dev->dev_addr), ioaddr + 0xA4);
	iowrite32(get_unaligned_le16(dev->dev_addr + 4), ioaddr + 0xA8);
	iowrite32(0, ioaddr + CSR27);
	iowrite32(0, ioaddr + CSR28);

	tp->csr6 = 0;

	/* Enable automatic Tx underrun recovery. */
	iowrite32(ioread32(ioaddr + CSR18) | 1, ioaddr + CSR18);
	tp->csr6 = 0x00040000;

	/* Start the chip's Tx to process setup frame. */
	tulip_stop_rxtx(tp);
	barrier();
	udelay(5);
	iowrite32(tp->csr6 | TxOn, ioaddr + CSR6);

	/* Enable interrupts by setting the interrupt mask. */
	iowrite32(VALID_INTR, ioaddr + CSR5);
	iowrite32(VALID_INTR, ioaddr + CSR7);
	tulip_start_rxtx(tp);
	iowrite32(0, ioaddr + CSR2);		/* Rx poll demand */

	if (tulip_debug > 2) {
		printk(KERN_DEBUG "%s: Done tulip_up(), CSR0 %08x, CSR5 %08x CSR6 %08x\n",
		       dev->name, ioread32(ioaddr + CSR0),
		       ioread32(ioaddr + CSR5),
		       ioread32(ioaddr + CSR6));
	}

	init_timer(&tp->oom_timer);
        tp->oom_timer.data = (unsigned long)dev;
        tp->oom_timer.function = oom_timer;
}

static int
tulip_open(struct net_device *dev)
{
	int retval;

	tulip_init_ring (dev);

	retval = request_irq(dev->irq, tulip_interrupt, 0, dev->name, dev);
	if (retval)
		goto free_ring;

	tulip_up (dev);

	netif_start_queue (dev);

	return 0;

free_ring:
	tulip_free_ring (dev);
	return retval;
}


static void tulip_tx_timeout(struct net_device *dev)
{
	struct tulip_private *tp = netdev_priv(dev);
	void __iomem *ioaddr = tp->base_addr;
	unsigned long flags;

	spin_lock_irqsave (&tp->lock, flags);

	dev_warn(&dev->dev,
		 "Transmit timed out, status %08x, CSR12 %08x, resetting...\n",
		 ioread32(ioaddr + CSR5), ioread32(ioaddr + CSR12));

	tulip_tx_timeout_complete(tp, ioaddr);

	spin_unlock_irqrestore (&tp->lock, flags);
	dev->trans_start = jiffies; /* prevent tx timeout */
	netif_wake_queue (dev);
}


/* Initialize the Rx and Tx rings, along with various 'dev' bits. */
static void tulip_init_ring(struct net_device *dev)
{
	struct tulip_private *tp = netdev_priv(dev);
	int i;

	tp->nir = 0;

	for (i = 0; i < RX_RING_SIZE; i++) {
		tp->rx_ring[i].status = 0x00000000;
		tp->rx_ring[i].length = cpu_to_le32(PKT_BUF_SZ);
		tp->rx_ring[i].buffer2 = cpu_to_le32(tp->rx_ring_dma + sizeof(struct tulip_rx_desc) * (i + 1));
		tp->rx_buffers[i].skb = NULL;
		tp->rx_buffers[i].mapping = 0;
	}
	/* Mark the last entry as wrapping the ring. */
	tp->rx_ring[i-1].length = cpu_to_le32(PKT_BUF_SZ | DESC_RING_WRAP);
	tp->rx_ring[i-1].buffer2 = cpu_to_le32(tp->rx_ring_dma);

	for (i = 0; i < RX_RING_SIZE; i++) {
		dma_addr_t mapping;
		/* Note the receive buffer must be longword aligned.
		   dev_alloc_skb() provides 16 byte alignment.  But do *not*
		   use skb_reserve() to align the IP header! */
		struct sk_buff *skb = dev_alloc_skb(PKT_BUF_SZ);
		tp->rx_buffers[i].skb = skb;
		if (skb == NULL)
			break;
		mapping = dma_map_single(&dev->dev, skb->data,
				PKT_BUF_SZ, DMA_FROM_DEVICE);
		tp->rx_buffers[i].mapping = mapping;
		skb->dev = dev;			/* Mark as being used by this device. */
		tp->rx_ring[i].status = cpu_to_le32(DescOwned);	/* Owned by Tulip chip */
		tp->rx_ring[i].buffer1 = cpu_to_le32(mapping);
	}
	tp->dirty_rx = (unsigned int)(i - RX_RING_SIZE);

	/* The Tx buffer descriptor is filled in as needed, but we
	   do need to clear the ownership bit. */
	for (i = 0; i < TX_RING_SIZE; i++) {
		tp->tx_buffers[i].skb = NULL;
		tp->tx_buffers[i].mapping = 0;
		tp->tx_ring[i].status = 0x00000000;
		tp->tx_ring[i].buffer2 = cpu_to_le32(tp->tx_ring_dma + sizeof(struct tulip_tx_desc) * (i + 1));
	}
	tp->tx_ring[i-1].buffer2 = cpu_to_le32(tp->tx_ring_dma);
}

static netdev_tx_t
tulip_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
	struct tulip_private *tp = netdev_priv(dev);
	int entry;
	u32 flag;
	dma_addr_t mapping;
	unsigned long flags;

	spin_lock_irqsave(&tp->lock, flags);

	/* Calculate the next Tx descriptor entry. */
	entry = tp->cur_tx % TX_RING_SIZE;

	tp->tx_buffers[entry].skb = skb;
	mapping = dma_map_single(&tp->pdev->dev, skb->data, skb->len,
			DMA_TO_DEVICE);
	tp->tx_buffers[entry].mapping = mapping;
	tp->tx_ring[entry].buffer1 = cpu_to_le32(mapping);

	if (tp->cur_tx - tp->dirty_tx < TX_RING_SIZE/2) {/* Typical path */
		flag = 0x60000000; /* No interrupt */
	} else if (tp->cur_tx - tp->dirty_tx == TX_RING_SIZE/2) {
		flag = 0xe0000000; /* Tx-done intr. */
	} else if (tp->cur_tx - tp->dirty_tx < TX_RING_SIZE - 2) {
		flag = 0x60000000; /* No Tx-done intr. */
	} else {		/* Leave room for set_rx_mode() to fill entries. */
		flag = 0xe0000000; /* Tx-done intr. */
		netif_stop_queue(dev);
	}
	if (entry == TX_RING_SIZE-1)
		flag = 0xe0000000 | DESC_RING_WRAP;

	tp->tx_ring[entry].length = cpu_to_le32(skb->len | flag);
	/* if we were using Transmit Automatic Polling, we would need a
	 * wmb() here. */
	tp->tx_ring[entry].status = cpu_to_le32(DescOwned);
	wmb();

	tp->cur_tx++;

	/* Trigger an immediate transmit demand. */
	iowrite32(0, tp->base_addr + CSR1);

	spin_unlock_irqrestore(&tp->lock, flags);

	return NETDEV_TX_OK;
}

static void tulip_clean_tx_ring(struct tulip_private *tp)
{
	unsigned int dirty_tx;

	for (dirty_tx = tp->dirty_tx ; tp->cur_tx - dirty_tx > 0;
		dirty_tx++) {
		int entry = dirty_tx % TX_RING_SIZE;
		int status = le32_to_cpu(tp->tx_ring[entry].status);

		if (status < 0) {
			tp->stats.tx_errors++;	/* It wasn't Txed */
			tp->tx_ring[entry].status = 0;
		}

		dma_unmap_single(&tp->pdev->dev, tp->tx_buffers[entry].mapping,
			tp->tx_buffers[entry].skb->len,
			DMA_TO_DEVICE);

		/* Free the original skb. */
		dev_kfree_skb_irq(tp->tx_buffers[entry].skb);
		tp->tx_buffers[entry].skb = NULL;
		tp->tx_buffers[entry].mapping = 0;
	}
}

static void tulip_down (struct net_device *dev)
{
	struct tulip_private *tp = netdev_priv(dev);
	void __iomem *ioaddr = tp->base_addr;
	unsigned long flags;

	napi_disable(&tp->napi);
	del_timer_sync (&tp->oom_timer);
	spin_lock_irqsave (&tp->lock, flags);

	/* Disable interrupts by clearing the interrupt mask. */
	iowrite32 (0x00000000, ioaddr + CSR7);

	/* Stop the Tx and Rx processes. */
	tulip_stop_rxtx(tp);

	/* prepare receive buffers */
	tulip_refill_rx(dev);

	/* release any unconsumed transmit buffers */
	tulip_clean_tx_ring(tp);

	if (ioread32 (ioaddr + CSR6) != 0xffffffff)
		tp->stats.rx_missed_errors += ioread32 (ioaddr + CSR8) & 0xffff;

	spin_unlock_irqrestore (&tp->lock, flags);
}

static void tulip_free_ring (struct net_device *dev)
{
	struct tulip_private *tp = netdev_priv(dev);
	int i;

	/* Free all the skbuffs in the Rx queue. */
	for (i = 0; i < RX_RING_SIZE; i++) {
		struct sk_buff *skb = tp->rx_buffers[i].skb;
		dma_addr_t mapping = tp->rx_buffers[i].mapping;

		tp->rx_buffers[i].skb = NULL;
		tp->rx_buffers[i].mapping = 0;

		tp->rx_ring[i].status = 0;	/* Not owned by Tulip chip. */
		tp->rx_ring[i].length = 0;
		/* An invalid address. */
		tp->rx_ring[i].buffer1 = cpu_to_le32(0xBADF00D0);
		if (skb) {
			dma_unmap_single(&tp->pdev->dev, mapping, PKT_BUF_SZ,
				DMA_FROM_DEVICE);
			dev_kfree_skb (skb);
		}
	}

	for (i = 0; i < TX_RING_SIZE; i++) {
		struct sk_buff *skb = tp->tx_buffers[i].skb;

		if (skb != NULL) {
			dma_unmap_single(&tp->pdev->dev,
				tp->tx_buffers[i].mapping, skb->len, DMA_TO_DEVICE);
			dev_kfree_skb (skb);
		}
		tp->tx_buffers[i].skb = NULL;
		tp->tx_buffers[i].mapping = 0;
	}
}

static int tulip_close (struct net_device *dev)
{
	struct tulip_private *tp = netdev_priv(dev);
	void __iomem *ioaddr = tp->base_addr;

	netif_stop_queue (dev);

	tulip_down (dev);

	if (tulip_debug > 1)
		dev_printk(KERN_DEBUG, &dev->dev,
			   "Shutting down ethercard, status was %02x\n",
			   ioread32 (ioaddr + CSR5));

	free_irq (dev->irq, dev);

	tulip_free_ring (dev);

	return 0;
}

static struct net_device_stats *tulip_get_stats(struct net_device *dev)
{
	struct tulip_private *tp = netdev_priv(dev);
	void __iomem *ioaddr = tp->base_addr;

	if (netif_running(dev)) {
		unsigned long flags;

		spin_lock_irqsave (&tp->lock, flags);

		tp->stats.rx_missed_errors += ioread32(ioaddr + CSR8) & 0xffff;

		spin_unlock_irqrestore(&tp->lock, flags);
	}

	return &tp->stats;
}


static void tulip_get_drvinfo(struct net_device *dev, struct ethtool_drvinfo *info)
{
	strcpy(info->driver, DRV_NAME);
	strcpy(info->version, DRV_VERSION);
	strcpy(info->bus_info, "mmio");
}

static const struct ethtool_ops ops = {
	.get_drvinfo = tulip_get_drvinfo
};

static void set_rx_mode(struct net_device *dev)
{
	struct tulip_private *tp = netdev_priv(dev);
	void __iomem *ioaddr = tp->base_addr;
	int csr6;

	csr6 = ioread32(ioaddr + CSR6) & ~0x00D5;

	tp->csr6 &= ~0x00D5;
	if (dev->flags & IFF_PROMISC) {			/* Set promiscuous. */
		tp->csr6 |= AcceptAllMulticast | AcceptAllPhys;
		csr6 |= AcceptAllMulticast | AcceptAllPhys;
	} else if ((netdev_mc_count(dev) > 1000) ||
		   (dev->flags & IFF_ALLMULTI)) {
		/* Too many to filter well -- accept all multicasts. */
		tp->csr6 |= AcceptAllMulticast;
		csr6 |= AcceptAllMulticast;
	} else {
		/* Some work-alikes have only a 64-entry hash filter table. */
		/* Should verify correctness on big-endian/__powerpc__ */
		struct netdev_hw_addr *ha;
		if (netdev_mc_count(dev) > 64) {
			/* Arbitrary non-effective limit. */
			tp->csr6 |= AcceptAllMulticast;
			csr6 |= AcceptAllMulticast;
		} else {
			u32 mc_filter[2] = {0, 0};		 /* Multicast hash filter */
			int filterbit;
			netdev_for_each_mc_addr(ha, dev) {
				filterbit = ether_crc_le(ETH_ALEN, ha->addr);
				filterbit &= 0x3f;
				mc_filter[filterbit >> 5] |= 1 << (filterbit & 31);
				if (tulip_debug > 2)
					dev_info(&dev->dev,
						 "Added filter for %pM  %08x bit %d\n",
						 ha->addr,
						 ether_crc(ETH_ALEN, ha->addr),
						 filterbit);
			}
			if (mc_filter[0] == tp->mc_filter[0]  &&
				mc_filter[1] == tp->mc_filter[1])
				;				/* No change. */
			iowrite32(mc_filter[0], ioaddr + CSR27);
			iowrite32(mc_filter[1], ioaddr + CSR28);
			tp->mc_filter[0] = mc_filter[0];
			tp->mc_filter[1] = mc_filter[1];
		}
	}

	if (dev->irq == ADM8668_LAN_IRQ)
		csr6 |= (1 << 9);	/* force 100Mbps full duplex */
//	csr6 |= 1;	/* pad 2 bytes. vlan? */

	iowrite32(csr6, ioaddr + CSR6);
}

static const struct net_device_ops tulip_netdev_ops = {
	.ndo_open		= tulip_open,
	.ndo_start_xmit		= tulip_start_xmit,
	.ndo_tx_timeout		= tulip_tx_timeout,
	.ndo_stop		= tulip_close,
	.ndo_get_stats		= tulip_get_stats,
	.ndo_set_rx_mode	= set_rx_mode,
	.ndo_change_mtu		= eth_change_mtu,
	.ndo_set_mac_address	= eth_mac_addr,
	.ndo_validate_addr	= eth_validate_addr,
#ifdef CONFIG_NET_POLL_CONTROLLER
	.ndo_poll_controller	 = poll_tulip,
#endif
};

static int __devinit adm8668net_probe(struct platform_device *pdev)
{
	struct tulip_private *tp;
	struct net_device *dev;
	struct resource *res;
	void __iomem *ioaddr;
	int irq;

	if (pdev->id < 0 || pdev->id >= MAX_UNITS)
		return -EINVAL;

	if (!(res = platform_get_resource(pdev, IORESOURCE_IRQ, 0)))
		return -ENODEV;
	irq = res->start;
	if (!(res = platform_get_resource(pdev, IORESOURCE_MEM, 0)))
		return -ENODEV;
	if (!(ioaddr = ioremap(res->start, res->end - res->start)))
		return -ENODEV;
	if (!(dev = alloc_etherdev(sizeof (*tp))))
		return -ENOMEM;

	/* setup net dev */
	dev->base_addr = (unsigned long)res->start;
	dev->irq = irq;
	SET_NETDEV_DEV(dev, &pdev->dev);

	/* tulip private struct */
	tp = netdev_priv(dev);
	tp->dev = dev;
	tp->base_addr = ioaddr;
	tp->csr0 = csr0;
	tp->pdev = pdev;
	tp->rx_ring = dma_alloc_coherent(&pdev->dev,
				sizeof(struct tulip_rx_desc) * RX_RING_SIZE +
				sizeof(struct tulip_tx_desc) * TX_RING_SIZE,
				&tp->rx_ring_dma, GFP_KERNEL);
	if (!tp->rx_ring)
		return -ENODEV;
	tp->tx_ring = (struct tulip_tx_desc *)(tp->rx_ring + RX_RING_SIZE);
	tp->tx_ring_dma = tp->rx_ring_dma + sizeof(struct tulip_rx_desc) * RX_RING_SIZE;

	spin_lock_init(&tp->lock);

	/* Stop the chip's Tx and Rx processes. */
	tulip_stop_rxtx(tp);

	/* Clear the missed-packet counter. */
	ioread32(ioaddr + CSR8);

	/* Addresses are stored in BSP area of NOR flash */
	if (irq == ADM8668_WAN_IRQ)
		memcpy(dev->dev_addr, (char *)ADM8668_WAN_MACADDR, 6);
	else
		memcpy(dev->dev_addr, (char *)ADM8668_LAN_MACADDR, 6);

	/* The Tulip-specific entries in the device structure. */
	dev->netdev_ops = &tulip_netdev_ops;
	dev->watchdog_timeo = TX_TIMEOUT;
	netif_napi_add(dev, &tp->napi, tulip_poll, 16);
	SET_ETHTOOL_OPS(dev, &ops);

	if (register_netdev(dev))
		goto err_out_free_ring;

	dev_info(&dev->dev,
		 "ADM8668net at MMIO %#lx %pM, IRQ %d\n",
		 (unsigned long)dev->base_addr, dev->dev_addr, irq);

	platform_set_drvdata(pdev, dev);
	return 0;

err_out_free_ring:
	dma_free_coherent(&pdev->dev,
		     sizeof (struct tulip_rx_desc) * RX_RING_SIZE +
		     sizeof (struct tulip_tx_desc) * TX_RING_SIZE,
		     tp->rx_ring, tp->rx_ring_dma);
	return -ENODEV;
}

static int __devexit adm8668net_remove(struct platform_device *pdev)
{
	struct net_device *dev = platform_get_drvdata (pdev);
	struct tulip_private *tp;

	if (!dev)
		return -ENODEV;

	tp = netdev_priv(dev);
	unregister_netdev(dev);
	dma_free_coherent(&pdev->dev,
			     sizeof (struct tulip_rx_desc) * RX_RING_SIZE +
			     sizeof (struct tulip_tx_desc) * TX_RING_SIZE,
			     tp->rx_ring, tp->rx_ring_dma);
	iounmap(tp->base_addr);
	free_netdev(dev);
	platform_set_drvdata(pdev, NULL);
	return 0;
}

#ifdef CONFIG_NET_POLL_CONTROLLER
/*
 * Polling 'interrupt' - used by things like netconsole to send skbs
 * without having to re-enable interrupts. It's not called while
 * the interrupt routine is executing.
 */

static void poll_tulip (struct net_device *dev)
{
	/* disable_irq here is not very nice, but with the lockless
	   interrupt handler we have no other choice. */
	disable_irq(dev->irq);
	tulip_interrupt(dev->irq, dev);
	enable_irq(dev->irq);
}
#endif

static struct platform_driver adm8668net_platform_driver = {
	.probe = adm8668net_probe,
	.remove = __devexit_p(adm8668net_remove),
	.driver = {
		.owner = THIS_MODULE,
		.name = "adm8668_eth"
	},
};

static int __init adm8668net_init(void)
{
	pr_info("%s", version);
	return platform_driver_register(&adm8668net_platform_driver);
}

static void __exit adm8668net_exit(void)
{
	platform_driver_unregister(&adm8668net_platform_driver);
}

module_init(adm8668net_init);
module_exit(adm8668net_exit);

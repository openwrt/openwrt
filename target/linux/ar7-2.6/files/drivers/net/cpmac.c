/*
 * $Id$
 * 
 * Copyright (C) 2006, 2007 OpenWrt.org
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/moduleparam.h>

#include <linux/sched.h>
#include <linux/kernel.h> /* printk() */
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/delay.h>

#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/ethtool.h>
#include <linux/skbuff.h>
#include <linux/mii.h>
#include <linux/phy.h>
#include <linux/platform_device.h>
#include <asm/ar7/ar7.h>
#include <asm/gpio.h>

MODULE_AUTHOR("Eugene Konev");
MODULE_DESCRIPTION("TI AR7 ethernet driver (CPMAC)");
MODULE_LICENSE("GPL");

/* Register definitions */
struct cpmac_control_regs {
	volatile u32 revision;
	volatile u32 control;
	volatile u32 teardown;
	volatile u32 unused;
};

struct cpmac_int_regs {
	volatile u32 stat_raw;
	volatile u32 stat_masked;
	volatile u32 enable;
	volatile u32 clear;
};

struct cpmac_stats {
	volatile u32 good;
	volatile u32 bcast;
	volatile u32 mcast;
	volatile u32 pause;
	volatile u32 crc_error;
	volatile u32 align_error;
	volatile u32 oversized;
	volatile u32 jabber;
	volatile u32 undersized;
	volatile u32 fragment;
	volatile u32 filtered;
	volatile u32 qos_filtered;
	volatile u32 octets;
};

struct cpmac_regs {
	struct cpmac_control_regs tx_ctrl;
	struct cpmac_control_regs rx_ctrl;
	volatile u32 unused1[56];
	volatile u32 mbp;
/* MBP bits */
#define MBP_RXPASSCRC         0x40000000
#define MBP_RXQOS             0x20000000
#define MBP_RXNOCHAIN         0x10000000
#define MBP_RXCMF             0x01000000
#define MBP_RXSHORT           0x00800000
#define MBP_RXCEF             0x00400000
#define MBP_RXPROMISC         0x00200000
#define MBP_PROMISCCHAN(chan) (((chan) & 0x7) << 16)
#define MBP_RXBCAST           0x00002000
#define MBP_BCASTCHAN(chan)   (((chan) & 0x7) << 8)
#define MBP_RXMCAST           0x00000020
#define MBP_MCASTCHAN(chan)   ((chan) & 0x7)
	volatile u32 unicast_enable;
	volatile u32 unicast_clear;
	volatile u32 max_len;
	volatile u32 buffer_offset;
	volatile u32 filter_flow_threshold;
	volatile u32 unused2[2];
	volatile u32 flow_thre[8];
	volatile u32 free_buffer[8];
	volatile u32 mac_control;
#define MAC_TXPTYPE  0x00000200
#define MAC_TXPACE   0x00000040
#define MAC_MII      0x00000020
#define MAC_TXFLOW   0x00000010
#define MAC_RXFLOW   0x00000008
#define MAC_MTEST    0x00000004
#define MAC_LOOPBACK 0x00000002
#define MAC_FDX      0x00000001
	volatile u32 mac_status;
#define MACST_QOS    0x4
#define MACST_RXFLOW 0x2
#define MACST_TXFLOW 0x1
	volatile u32 emc_control;
	volatile u32 unused3;
	struct cpmac_int_regs tx_int;
	volatile u32 mac_int_vector;
/* Int Status bits */
#define INTST_STATUS 0x80000
#define INTST_HOST   0x40000
#define INTST_RX     0x20000
#define INTST_TX     0x10000
	volatile u32 mac_eoi_vector;
	volatile u32 unused4[2];
	struct cpmac_int_regs rx_int;
	volatile u32 mac_int_stat_raw;
	volatile u32 mac_int_stat_masked;
	volatile u32 mac_int_enable;
	volatile u32 mac_int_clear;
	volatile u32 mac_addr_low[8];
	volatile u32 mac_addr_mid;
	volatile u32 mac_addr_high;
	volatile u32 mac_hash_low;
	volatile u32 mac_hash_high;
	volatile u32 boff_test;
	volatile u32 pac_test;
	volatile u32 rx_pause;
	volatile u32 tx_pause;
	volatile u32 unused5[2];
	struct cpmac_stats rx_stats;
	struct cpmac_stats tx_stats;
	volatile u32 unused6[232];
	volatile u32 tx_ptr[8];
	volatile u32 rx_ptr[8];
	volatile u32 tx_ack[8];
	volatile u32 rx_ack[8];
	
};

struct cpmac_mdio_regs {
	volatile u32 version;
	volatile u32 control;
#define MDIOC_IDLE        0x80000000
#define MDIOC_ENABLE      0x40000000
#define MDIOC_PREAMBLE    0x00100000
#define MDIOC_FAULT       0x00080000
#define MDIOC_FAULTDETECT 0x00040000
#define MDIOC_INTTEST     0x00020000
#define MDIOC_CLKDIV(div) ((div) & 0xff)
	volatile u32 alive;
	volatile u32 link;
	struct cpmac_int_regs link_int;
	struct cpmac_int_regs user_int;
	u32 unused[20];
	volatile u32 access;
#define MDIO_BUSY       0x80000000
#define MDIO_WRITE      0x40000000
#define MDIO_REG(reg)   (((reg) & 0x1f) << 21)
#define MDIO_PHY(phy)   (((phy) & 0x1f) << 16)
#define MDIO_DATA(data) ((data) & 0xffff)
	volatile u32 physel;
};

/* Descriptor */
struct cpmac_desc {
	u32 hw_next;
	u32 hw_data;
	u16 buflen;
	u16 bufflags;
	u16 datalen;
	u16 dataflags;
/* Flags bits */
#define CPMAC_SOP 0x8000
#define CPMAC_EOP 0x4000
#define CPMAC_OWN 0x2000
#define CPMAC_EOQ 0x1000
	u32 jiffies;
	struct sk_buff *skb;
	struct cpmac_desc *next;
};

struct cpmac_priv {
	struct net_device_stats stats;
	spinlock_t lock;
	int free_tx_channels;
	struct cpmac_desc *tx_pool;
	struct cpmac_desc *rx_channels[8];
	struct cpmac_desc *tx_channels[8];
	struct cpmac_regs *regs;
	struct mii_bus *mii_bus;
	struct phy_device *phy;
	char phy_name[BUS_ID_SIZE];
	unsigned long pages;
	int order;
	struct plat_cpmac_data *config;
	int oldlink, oldspeed, oldduplex;
	u32 msg_enable;
};

static irqreturn_t cpmac_irq(int, void *);
void cpmac_exit(void);

#ifdef CPMAC_DEBUG
static void cpmac_dump_regs(u32 *base, int count)
{
	int i;
	for (i = 0; i < (count + 3) / 4; i++) {
		if (i % 4 == 0) printk("\nCPMAC[0x%04x]:", i * 4);
		printk(" 0x%08x", *(base + i));
	}
	printk("\n");
}
#endif

static int cpmac_mdio_read(struct mii_bus *bus, int phy_id, int regnum)
{
	struct cpmac_mdio_regs *regs = (struct cpmac_mdio_regs *)bus->priv;
	volatile u32 val;

	while ((val = regs->access) & MDIO_BUSY);
	regs->access = MDIO_BUSY | MDIO_REG(regnum & 0x1f) |
		MDIO_PHY(phy_id & 0x1f);
	while ((val = regs->access) & MDIO_BUSY);

	return val & 0xffff;
}

static int cpmac_mdio_write(struct mii_bus *bus, int phy_id, int regnum, u16 val)
{
	struct cpmac_mdio_regs *regs = (struct cpmac_mdio_regs *)bus->priv;
	volatile u32 tmp;

	while ((tmp = regs->access) & MDIO_BUSY);
	regs->access = MDIO_BUSY | MDIO_WRITE | 
		MDIO_REG(regnum & 0x1f) | MDIO_PHY(phy_id & 0x1f) |
		val;

	return 0;
}

static int cpmac_mdio_reset(struct mii_bus *bus)
{
	struct cpmac_mdio_regs *regs = (struct cpmac_mdio_regs *)bus->priv;

	ar7_device_reset(AR7_RESET_BIT_MDIO);
	regs->control = MDIOC_ENABLE |
		MDIOC_CLKDIV(ar7_cpmac_freq() / 2200000 - 1);

	return 0;
}

static int mii_irqs[PHY_MAX_ADDR] = { PHY_POLL, };

struct mii_bus cpmac_mii = {
	.name = "cpmac-mii",
	.read = cpmac_mdio_read,
	.write = cpmac_mdio_write,
	.reset = cpmac_mdio_reset,
	.irq = mii_irqs,
};

int cpmac_config(struct net_device *dev, struct ifmap *map)
{
	if (dev->flags & IFF_UP)
		return -EBUSY;

	/* Don't allow changing the I/O address */
	if (map->base_addr != dev->base_addr)
		return -EOPNOTSUPP;

	/* ignore other fields */
	return 0;
}

int cpmac_set_mac_address(struct net_device *dev, void *addr)
{
	struct sockaddr *sa = addr;

	if (dev->flags & IFF_UP)
		return -EBUSY;

	memcpy(dev->dev_addr, sa->sa_data, dev->addr_len);

	return 0;
}

void cpmac_set_multicast_list(struct net_device *dev)
{
	struct dev_mc_list *iter;
	int i;
	int hash, tmp;
	int hashlo = 0, hashhi = 0;
	struct cpmac_priv *priv = netdev_priv(dev);

	if(dev->flags & IFF_PROMISC) {
		priv->regs->mbp &= ~MBP_PROMISCCHAN(0); /* promisc channel 0 */
		priv->regs->mbp |= MBP_RXPROMISC;
        } else {
		priv->regs->mbp &= ~MBP_RXPROMISC;
		if(dev->flags & IFF_ALLMULTI) {
			/* enable all multicast mode */
			priv->regs->mac_hash_low = 0xffffffff;
			priv->regs->mac_hash_high = 0xffffffff;
		} else {
			for(i = 0, iter = dev->mc_list; i < dev->mc_count;
			    i++, iter = iter->next) {
				hash = 0;
				tmp = iter->dmi_addr[0];
				hash  ^= (tmp >> 2) ^ (tmp << 4);
				tmp = iter->dmi_addr[1];
				hash  ^= (tmp >> 4) ^ (tmp << 2);
				tmp = iter->dmi_addr[2];
				hash  ^= (tmp >> 6) ^ tmp;
				tmp = iter->dmi_addr[4];
				hash  ^= (tmp >> 2) ^ (tmp << 4);
				tmp = iter->dmi_addr[5];
				hash  ^= (tmp >> 4) ^ (tmp << 2);
				tmp = iter->dmi_addr[6];
				hash  ^= (tmp >> 6) ^ tmp;
				hash &= 0x3f;
				if(hash < 32) {
					hashlo |= 1<<hash;
				} else {
					hashhi |= 1<<(hash - 32);
				}
			}

			priv->regs->mac_hash_low = hashlo;
			priv->regs->mac_hash_high = hashhi;
		}
	}
}

static void cpmac_rx(struct net_device *dev, int channel)
{
	struct cpmac_desc *pkt;
	struct sk_buff *skb;
	char *data;
	struct cpmac_priv *priv = netdev_priv(dev);

	spin_lock(&priv->lock);
	pkt = priv->rx_channels[channel];
	if (!pkt) {
		if (printk_ratelimit())
			printk(KERN_NOTICE "%s: rx: spurious interrupt\n",
			       dev->name); 
		priv->stats.rx_errors++;
		return;
	}

	priv->regs->rx_ack[channel] = virt_to_phys(pkt);
	dma_cache_inv((u32)pkt, 16);
	if (!pkt->datalen) {
		if (printk_ratelimit())
			printk(KERN_NOTICE "%s: rx: spurious interrupt\n",
			       dev->name); 
		priv->stats.rx_errors++;
		return;
	}
	skb = dev_alloc_skb(1536);
	if (!skb) {
		if (printk_ratelimit())
			printk(KERN_NOTICE "%s: rx: low on mem - packet dropped\n",
			       dev->name); 
		priv->stats.rx_dropped++;
	} else {
		data = (char *)phys_to_virt(pkt->hw_data);
		dma_cache_inv((u32)data, pkt->datalen);
		skb_put(pkt->skb, pkt->datalen);
		pkt->skb->protocol = eth_type_trans(pkt->skb, dev);
		pkt->skb->ip_summed = CHECKSUM_NONE;
		priv->stats.rx_packets++;
		priv->stats.rx_bytes += pkt->datalen;
		netif_rx(pkt->skb);
		skb_reserve(skb, 2);
		skb->dev = dev;
		pkt->skb = skb;
		pkt->hw_data = virt_to_phys(skb->data);
	}
	spin_unlock(&priv->lock);
	pkt->buflen = 1500 + ETH_HLEN + 4;
	pkt->datalen = 0;
	pkt->dataflags = CPMAC_OWN;
	dma_cache_wback_inv((u32)pkt, 16);
	priv->regs->rx_ptr[channel] = virt_to_phys(pkt);
}

struct cpmac_desc *cpmac_get_desc(struct net_device *dev) 
{
	struct cpmac_desc *pkt;
	struct cpmac_priv *priv = netdev_priv(dev);
	pkt = priv->tx_pool;
	priv->tx_pool = pkt->next;
	pkt->next = NULL;
	if (priv->tx_pool == NULL)
		netif_stop_queue(dev);
	return pkt;
}

void cpmac_release_desc(struct net_device *dev, struct cpmac_desc *pkt)
{
	struct cpmac_priv *priv = netdev_priv(dev);
	struct cpmac_desc *p;
	p = pkt;
	while (p->next) p = p->next;
	p->next = priv->tx_pool;
	priv->tx_pool = pkt;
}

int cpmac_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
	unsigned long flags;
	int i, len, frag;
	skb_frag_t *this_frag;
	void *data;
	struct cpmac_desc *head, *tail, *curr;
	struct cpmac_priv *priv = netdev_priv(dev);

	BUG_ON(priv->free_tx_channels < 1);
	len = skb->len;
	if (len < ETH_ZLEN) {
		if (skb_padto(skb, ETH_ZLEN)) {
			if (printk_ratelimit())
				printk(KERN_NOTICE "%s: padding failed, dropping\n",
				       dev->name); 
			spin_lock_irqsave(&priv->lock, flags);
			priv->stats.tx_dropped++;
			spin_unlock_irqrestore(&priv->lock, flags);
			return -ENOMEM;
		}
		len = ETH_ZLEN;
	}
	spin_lock_irqsave(&priv->lock, flags);
	dev->trans_start = jiffies;
	for (i = 0; i < 8; i++)
		if (!priv->tx_channels[i])
			break;

	BUG_ON(i == 8);

	head = cpmac_get_desc(dev);
	priv->tx_channels[i] = head;
	head->jiffies = dev->trans_start;
	if (!(--priv->free_tx_channels))
		netif_stop_queue(dev);
	spin_unlock_irqrestore(&priv->lock, flags);

	head->dataflags = CPMAC_SOP | CPMAC_OWN;
	head->skb = skb;
	head->hw_data = virt_to_phys(skb->data);
	dma_cache_wback_inv((u32)skb->data, len);
	head->buflen = len;
	head->datalen = len;
	tail = head;
	for (frag = 0; frag < skb_shinfo(skb)->nr_frags; frag++) {
		dma_cache_wback_inv((u32)tail, 16);
		this_frag = &skb_shinfo(skb)->frags[frag];
		curr = cpmac_get_desc(dev);
		data = page_address(this_frag->page) +
			this_frag->page_offset;
		curr->hw_data = virt_to_phys(data);
		curr->buflen = this_frag->size;
		curr->datalen = this_frag->size;
		curr->dataflags = CPMAC_OWN;
		dma_cache_wback_inv((u32)data, len);
		tail->hw_next = virt_to_phys(curr);
		tail->next = curr;
		tail = curr;
	}
	tail->hw_next = 0;
	tail->dataflags |= CPMAC_EOP;
	dma_cache_wback_inv((u32)tail, 16);
	priv->regs->tx_ptr[i] = virt_to_phys(head);
	return 0;
}

void cpmac_end_xmit(struct net_device *dev, int channel)
{
	struct cpmac_desc *pkt;
	struct cpmac_priv *priv = netdev_priv(dev);

	spin_lock(&priv->lock);
	pkt = priv->tx_channels[channel];
	priv->tx_channels[channel] = NULL;
	priv->free_tx_channels++;
	priv->regs->tx_ack[channel] = virt_to_phys(pkt);
	if (pkt) {
		priv->stats.tx_packets++;
		priv->stats.tx_bytes += pkt->skb->len;
		dev_kfree_skb_irq(pkt->skb);
		cpmac_release_desc(dev, pkt);
		if (netif_queue_stopped(dev))
			netif_wake_queue(dev);
	} else {
		if (printk_ratelimit())
			printk(KERN_NOTICE "%s: end_xmit: spurious interrupt\n",
			       dev->name); 
	}
	spin_unlock(&priv->lock);
}

static irqreturn_t cpmac_irq(int irq, void *dev_id)
{
	struct net_device *dev = (struct net_device *)dev_id;
	struct cpmac_priv *priv = netdev_priv(dev);
	u32 status;

	if (!dev)
		return IRQ_NONE;

	status = priv->regs->mac_int_vector;

	if (status & INTST_TX) {
		cpmac_end_xmit(dev, (status & 7));
	}

	if (status & INTST_RX) {
		cpmac_rx(dev, (status >> 8) & 7);
	}

	if (status & INTST_HOST) { /* host interrupt ??? */
		printk("%s: host int, something bad happened...\n", dev->name);
		printk("%s: mac status: 0x%08x\n", dev->name,
		       priv->regs->mac_status);
	}

	if (status & INTST_STATUS) { /* status interrupt ??? */
		printk("%s: status int, what are we gonna do?\n", dev->name);
	}

	priv->regs->mac_eoi_vector = 0;
	return IRQ_HANDLED;
}

void cpmac_tx_timeout(struct net_device *dev)
{
	int i;
	struct cpmac_priv *priv = netdev_priv(dev);
	struct cpmac_desc *pkt = NULL, *tmp;

	priv->stats.tx_errors++;
	for (i = 0; i < 8; i++) {
		tmp = priv->tx_channels[i];
		if (tmp && (!pkt || (pkt->jiffies > tmp->jiffies)))
			pkt = tmp;
	}
	if (pkt) {
		printk("Transmit timeout at %ld, latency %ld\n", jiffies,
		       jiffies - pkt->jiffies);
		for (i = 0; i < 8; i++) 
			if (priv->tx_channels[i] == pkt)
				priv->tx_channels[i] = NULL;
		dev_kfree_skb(pkt->skb);
		cpmac_release_desc(dev, pkt);
		priv->free_tx_channels++;
		netif_wake_queue(dev);
	}
}

int cpmac_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd)
{
	struct cpmac_priv *priv = netdev_priv(dev);
	if (!(netif_running(dev)))
		return -EINVAL;
	if (!priv->phy)
		return -EINVAL;
	return phy_mii_ioctl(priv->phy, if_mii(ifr), cmd);
}

static int cpmac_get_settings(struct net_device *dev, struct ethtool_cmd *cmd)
{
        struct cpmac_priv *priv = netdev_priv(dev);

        if (priv->phy)
                return phy_ethtool_gset(priv->phy, cmd);

        return -EINVAL;
}

static int cpmac_set_settings(struct net_device *dev, struct ethtool_cmd *cmd)
{
        struct cpmac_priv *priv = netdev_priv(dev);

        if (!capable(CAP_NET_ADMIN))
                return -EPERM;

        if (priv->phy)
                return phy_ethtool_sset(priv->phy, cmd);

        return -EINVAL;
}

static void cpmac_get_drvinfo(struct net_device *dev, 
			      struct ethtool_drvinfo *info)
{
        strcpy(info->driver, "cpmac");
        strcpy(info->version, "0.0.3");
        info->fw_version[0] = '\0';
        sprintf(info->bus_info, "%s", "cpmac");
        info->regdump_len = 0;
}

static const struct ethtool_ops cpmac_ethtool_ops = {
        .get_settings = cpmac_get_settings,
        .set_settings = cpmac_set_settings,
        .get_drvinfo = cpmac_get_drvinfo,
        .get_link = ethtool_op_get_link,
};

static struct net_device_stats *cpmac_stats(struct net_device *dev)
{
	struct cpmac_priv *priv = netdev_priv(dev);

	if (netif_device_present(dev))
		return &priv->stats;

	return NULL;
}

static int cpmac_change_mtu(struct net_device *dev, int mtu)
{
	unsigned long flags;
	struct cpmac_priv *priv = netdev_priv(dev);
	spinlock_t *lock = &priv->lock;
    
	if ((mtu < 68) || (mtu > 1500))
		return -EINVAL;

	spin_lock_irqsave(lock, flags);
	dev->mtu = mtu;
	spin_unlock_irqrestore(lock, flags);

	return 0;
}

static void cpmac_reset(struct net_device *dev)
{
	int i;
	struct cpmac_priv *priv = netdev_priv(dev);

	ar7_device_reset(priv->config->reset_bit);
	priv->regs->rx_ctrl.control &= ~1;
	priv->regs->tx_ctrl.control &= ~1;
	for (i = 0; i < 8; i++) {
		priv->regs->tx_ptr[i] = 0;
		priv->regs->rx_ptr[i] = 0;
	}
	priv->regs->mac_control &= ~MAC_MII; /* disable mii */
}

static void cpmac_adjust_link(struct net_device *dev)
{
	struct cpmac_priv *priv = netdev_priv(dev);
	unsigned long flags;
	int new_state = 0;

	spin_lock_irqsave(&priv->lock, flags);
	if (priv->phy->link) {
		if (priv->phy->duplex != priv->oldduplex) {
			new_state = 1;
			priv->oldduplex = priv->phy->duplex;
		}

		if (priv->phy->speed != priv->oldspeed) {
			new_state = 1;
			priv->oldspeed = priv->phy->speed;
		}

		if (!priv->oldlink) {
			new_state = 1;
			priv->oldlink = 1;
			netif_schedule(dev);
		}
	} else if (priv->oldlink) {
		new_state = 1;
		priv->oldlink = 0;
		priv->oldspeed = 0;
		priv->oldduplex = -1;
	}

	if (new_state)
		phy_print_status(priv->phy);

	spin_unlock_irqrestore(&priv->lock, flags);
}

int cpmac_open(struct net_device *dev)
{
	int i, j, res;
	struct cpmac_priv *priv = netdev_priv(dev);
	struct cpmac_desc *pkt;
	struct sk_buff *skb;

/*	priv->phy = phy_connect(dev, priv->phy_name, &cpmac_adjust_link,
	0, PHY_INTERFACE_MODE_MII);*/
	priv->phy = phy_connect(dev, priv->phy_name, &cpmac_adjust_link, 0);
	if (IS_ERR(priv->phy)) {
		printk(KERN_ERR "%s: Could not attach to PHY\n", dev->name);
		return PTR_ERR(priv->phy);
	}

	if (!request_mem_region(dev->mem_start, dev->mem_end -
				dev->mem_start, dev->name)) {
		printk("%s: failed to request registers\n",
		       dev->name); 
		res = -ENXIO;
		goto fail_reserve;
	}

	priv->regs = ioremap_nocache(dev->mem_start, dev->mem_end -
				     dev->mem_start);
	if (!priv->regs) {
		printk("%s: failed to remap registers\n", dev->name);
		res = -ENXIO;
		goto fail_remap;
	}

	priv->order = get_order(4096);
	priv->pages = __get_dma_pages(GFP_KERNEL, priv->order);
	if (!priv->pages) {
		res = -ENOMEM;
		goto fail_alloc;
	}
	memset((char *)priv->pages, 0, 4096);

	priv->tx_pool = NULL;

	for (i = 0; i < 4096 / sizeof(struct cpmac_desc); i++) {
		pkt = (struct cpmac_desc *)
			(priv->pages + i * sizeof(struct cpmac_desc));
		memset(pkt, sizeof(struct cpmac_desc), 0);
		if (i < 8) {
			skb = alloc_skb(1500 + ETH_HLEN + 6, GFP_KERNEL);
			if (!skb) {
				for(j = 0; j < i - 1; j++)
					kfree_skb(priv->rx_channels[j]->skb);
				free_pages(priv->pages, priv->order);
				res = -ENOMEM;
				goto fail_alloc;
			}
			skb_reserve(skb, 2);
			skb->dev = dev;
			pkt->skb = skb;
			pkt->hw_data = virt_to_phys(skb->data);
			pkt->buflen = 1500 + ETH_HLEN + 4;
			pkt->dataflags = CPMAC_OWN;
			dma_cache_wback_inv((u32)pkt, 16);
			priv->rx_channels[i] = pkt;
			priv->tx_channels[i] = NULL;
		} else {
			pkt->next = priv->tx_pool;
			priv->tx_pool = pkt;
		}
	}

	cpmac_reset(dev);
	priv->free_tx_channels = 8;

	for (i = 0; i < 8; i++) {
		priv->regs->tx_ptr[i] = 0;
		priv->regs->rx_ptr[i] = virt_to_phys(priv->rx_channels[i]);
	}

	priv->regs->mbp = MBP_RXNOCHAIN | MBP_RXSHORT | MBP_RXBCAST |
		MBP_RXMCAST;
	priv->regs->unicast_enable = 0xff;
	priv->regs->unicast_clear = 0;
	priv->regs->buffer_offset = 0;
	for (i = 0; i < 8; i++)
		priv->regs->mac_addr_low[i] = dev->dev_addr[5];
	priv->regs->mac_addr_mid = dev->dev_addr[4];
	priv->regs->mac_addr_high = dev->dev_addr[0] | (dev->dev_addr[1] << 8)
		| (dev->dev_addr[2] << 16) | (dev->dev_addr[3] << 24);
	priv->regs->max_len = 1536;
	priv->regs->rx_int.enable = 0xff;
	priv->regs->rx_int.clear = 0;
	priv->regs->tx_int.enable = 0xff;
	priv->regs->tx_int.clear = 0;
	priv->regs->mac_int_enable = 3;
	priv->regs->mac_int_clear = 0xfc;

	if((res = request_irq(dev->irq, cpmac_irq, SA_INTERRUPT,
			      dev->name, dev))) {
		printk("%s: failed to obtain irq\n", dev->name);
		goto fail_irq;
	}

	priv->regs->rx_ctrl.control |= 1;
	priv->regs->tx_ctrl.control |= 1;
	priv->regs->mac_control |= MAC_MII | MAC_FDX;

	priv->phy->state = PHY_CHANGELINK;
	phy_start(priv->phy);

	netif_start_queue(dev);

	return 0;

fail_irq:
	for(i = 0; i < 8; i++)
		if (priv->rx_channels[i]->skb)
			kfree_skb(priv->rx_channels[i]->skb);
	free_pages(priv->pages, priv->order);

fail_alloc:
	iounmap(priv->regs);

fail_remap:
	release_mem_region(dev->mem_start, dev->mem_end -
			   dev->mem_start);

fail_reserve:
	phy_disconnect(priv->phy);

	return res;
}

int cpmac_stop(struct net_device *dev)
{
	int i;
	struct cpmac_priv *priv = netdev_priv(dev);

	netif_stop_queue(dev);

	phy_stop(priv->phy);
	phy_disconnect(priv->phy);
	priv->phy = NULL;

	cpmac_reset(dev);

	for (i = 0; i < 8; i++) {
		priv->regs->rx_ptr[i] = 0;
		priv->regs->tx_ptr[i] = 0;
		priv->regs->mbp = 0;
	}

	free_irq(dev->irq, dev);
	release_mem_region(dev->mem_start, dev->mem_end -
			   dev->mem_start);

	for(i = 0; i < 8; i++)
		if (priv->rx_channels[i]->skb)
			kfree_skb(priv->rx_channels[i]->skb);
	if (priv->pages) 
		free_pages(priv->pages, priv->order);

	return 0;
}

static int external_switch = 0;

int __devinit cpmac_probe(struct platform_device *pdev)
{
	int i, rc, phy_id;
	struct resource *res;
	struct cpmac_priv *priv;
	struct net_device *dev;
	struct plat_cpmac_data *pdata;

	if (strcmp(pdev->name, "cpmac") != 0)
		return -ENODEV;

	pdata = pdev->dev.platform_data;

	for (phy_id = 0; phy_id < PHY_MAX_ADDR; phy_id++) {
		if (!(pdata->phy_mask & (1 << phy_id)))
			continue;
		if (!cpmac_mii.phy_map[phy_id])
			continue;
		break;
	}

	if (phy_id == PHY_MAX_ADDR) {
		if (external_switch) {
			phy_id = 0;
		} else {
			printk("cpmac: no PHY present\n");
			return -ENODEV;
		}
	}

	dev = alloc_etherdev(sizeof(struct cpmac_priv));

	if (!dev) {
		printk(KERN_ERR "cpmac: Unable to allocate net_device structure!\n");
		return -ENOMEM;
	}

	SET_MODULE_OWNER(dev);
	platform_set_drvdata(pdev, dev);
	priv = netdev_priv(dev);

	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "regs");
	if (!res) {
	        rc = -ENODEV;
		goto fail;
	}

	dev->mem_start = res->start;
	dev->mem_end = res->end;
	dev->irq = platform_get_irq_byname(pdev, "irq");

	dev->mtu                = 1500;
	dev->open               = cpmac_open;
	dev->stop               = cpmac_stop;
	dev->set_config         = cpmac_config;
	dev->hard_start_xmit    = cpmac_start_xmit;
	dev->do_ioctl           = cpmac_ioctl;
	dev->get_stats          = cpmac_stats;
	dev->change_mtu         = cpmac_change_mtu;  
	dev->set_mac_address    = cpmac_set_mac_address;  
	dev->set_multicast_list = cpmac_set_multicast_list;
	dev->tx_timeout         = cpmac_tx_timeout;
	dev->ethtool_ops        = &cpmac_ethtool_ops;

	memset(priv, 0, sizeof(struct cpmac_priv));
	spin_lock_init(&priv->lock);
	priv->msg_enable = netif_msg_init(NETIF_MSG_WOL, 0x3fff);
	priv->config = pdata;
	memcpy(dev->dev_addr, priv->config->dev_addr, sizeof(dev->dev_addr));
	if (phy_id == 31) {
		snprintf(priv->phy_name, BUS_ID_SIZE, PHY_ID_FMT,
			 cpmac_mii.id, phy_id);
	} else {
		snprintf(priv->phy_name, BUS_ID_SIZE, "fixed@%d:%d", 100, 1);
	}

	if ((rc = register_netdev(dev))) {
		printk("cpmac: error %i registering device %s\n",
		       rc, dev->name);
		goto fail;
	}

	printk("cpmac: device %s (regs: %p, irq: %d, phy: %s, mac: ",
	       dev->name, (u32 *)dev->mem_start, dev->irq,
	       priv->phy_name);
	for (i = 0; i < 6; i++) {
		printk("%02x", dev->dev_addr[i]);
		if (i < 5) printk(":");
		else printk(")\n");
	}

	return 0;

fail:
	free_netdev(dev);
	return rc;
}

static int __devexit cpmac_remove(struct platform_device *pdev)
{
	struct net_device *dev = platform_get_drvdata(pdev);
	unregister_netdev(dev);
	free_netdev(dev);
	return 0;
}

static struct platform_driver cpmac_driver = {
	.driver.name = "cpmac",
	.probe = cpmac_probe,
	.remove = cpmac_remove,
};

int __devinit cpmac_init(void)
{
	volatile u32 mask;
	int i, res;
	cpmac_mii.priv = (struct cpmac_mdio_regs *)
		ioremap_nocache(AR7_REGS_MDIO, sizeof(struct cpmac_mdio_regs));

	if (!cpmac_mii.priv) {
		printk("Can't ioremap mdio registers\n");
		return -ENXIO;
	}

#warning FIXME: unhardcode gpio&reset bits
	ar7_gpio_disable(26);
	ar7_gpio_disable(27);
/*	ar7_device_reset(17);
	ar7_device_reset(21);
	ar7_device_reset(26);*/

	cpmac_mii.reset(&cpmac_mii);

	for (i = 0; i < 300000; i++) {
		mask = ((struct cpmac_mdio_regs *)cpmac_mii.priv)->alive;
		if (mask)
			break;
	}

	mask &= 0x7fffffff;
	if (mask & (mask - 1)) {
		external_switch = 1;
		mask = 0;
	}

	cpmac_mii.phy_mask = ~(mask | 0x80000000);

	res = mdiobus_register(&cpmac_mii);
	if (res)
		goto fail_mii;

	res = platform_driver_register(&cpmac_driver);
	if (res)
		goto fail_cpmac;

	return 0;

fail_cpmac:
	mdiobus_unregister(&cpmac_mii);

fail_mii:
	iounmap(cpmac_mii.priv);

	return res;
}

void __devexit cpmac_exit(void)
{
	platform_driver_unregister(&cpmac_driver);
	mdiobus_unregister(&cpmac_mii);
}

module_init(cpmac_init);
module_exit(cpmac_exit);

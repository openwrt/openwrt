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
#include <linux/ethtool.h>
#include <linux/platform_device.h>
#include <linux/phy.h>

#include <ramips_eth_platform.h>
#include "ramips_eth.h"

#define TX_TIMEOUT (20 * HZ / 100)
#define	MAX_RX_LENGTH	1600

#ifdef CONFIG_RALINK_RT305X
#include "ramips_esw.c"
#else
static inline int rt305x_esw_init(void) { return 0; }
static inline void rt305x_esw_exit(void) { }
#endif

#define phys_to_bus(a)  (a & 0x1FFFFFFF)

#ifdef CONFIG_RAMIPS_ETH_DEBUG
#define RADEBUG(fmt, args...)	printk(KERN_DEBUG fmt, ## args)
#else
#define RADEBUG(fmt, args...)	do {} while (0)
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

static inline void
ramips_fe_int_disable(u32 mask)
{
	ramips_fe_wr(ramips_fe_rr(RAMIPS_FE_INT_ENABLE) & ~mask,
		     RAMIPS_FE_INT_ENABLE);
	/* flush write */
	ramips_fe_rr(RAMIPS_FE_INT_ENABLE);
}

static inline void
ramips_fe_int_enable(u32 mask)
{
	ramips_fe_wr(ramips_fe_rr(RAMIPS_FE_INT_ENABLE) | mask,
		     RAMIPS_FE_INT_ENABLE);
	/* flush write */
	ramips_fe_rr(RAMIPS_FE_INT_ENABLE);
}

static inline void
ramips_hw_set_macaddr(unsigned char *mac)
{
	ramips_fe_wr((mac[0] << 8) | mac[1], RAMIPS_GDMA1_MAC_ADRH);
	ramips_fe_wr((mac[2] << 24) | (mac[3] << 16) | (mac[4] << 8) | mac[5],
		     RAMIPS_GDMA1_MAC_ADRL);
}

static struct sk_buff *
ramips_alloc_skb(struct raeth_priv *re)
{
	struct sk_buff *skb;

	skb = netdev_alloc_skb(re->netdev, MAX_RX_LENGTH + NET_IP_ALIGN);
	if (!skb)
		return NULL;

	skb_reserve(skb, NET_IP_ALIGN);

	return skb;
}

static void
ramips_ring_setup(struct raeth_priv *re)
{
	int len;
	int i;

	memset(re->tx_info, 0, NUM_TX_DESC * sizeof(struct raeth_tx_info));

	len = NUM_TX_DESC * sizeof(struct ramips_tx_dma);
	memset(re->tx, 0, len);

	for (i = 0; i < NUM_TX_DESC; i++) {
		struct raeth_tx_info *txi;
		struct ramips_tx_dma *txd;

		txd = &re->tx[i];
		txd->txd4 = TX_DMA_QN(3) | TX_DMA_PN(1);
		txd->txd2 = TX_DMA_LSO | TX_DMA_DONE;

		txi = &re->tx_info[i];
		txi->tx_desc = txd;
		if (txi->tx_skb != NULL) {
			netdev_warn(re->netdev,
				    "dirty skb for TX desc %d\n", i);
			txi->tx_skb = NULL;
		}
	}

	len = NUM_RX_DESC * sizeof(struct ramips_rx_dma);
	memset(re->rx, 0, len);

	for (i = 0; i < NUM_RX_DESC; i++) {
		struct raeth_rx_info *rxi;
		struct ramips_rx_dma *rxd;
		dma_addr_t dma_addr;

		rxd = &re->rx[i];
		rxi = &re->rx_info[i];
		BUG_ON(rxi->rx_skb == NULL);
		dma_addr = dma_map_single(&re->netdev->dev, rxi->rx_skb->data,
					  MAX_RX_LENGTH, DMA_FROM_DEVICE);
		rxi->rx_dma = dma_addr;
		rxi->rx_desc = rxd;

		rxd->rxd1 = (unsigned int) dma_addr;
		rxd->rxd2 = RX_DMA_LSO;
	}

	/* flush descriptors */
	wmb();
}

static void
ramips_ring_cleanup(struct raeth_priv *re)
{
	int i;

	for (i = 0; i < NUM_RX_DESC; i++) {
		struct raeth_rx_info *rxi;

		rxi = &re->rx_info[i];
		if (rxi->rx_skb)
			dma_unmap_single(&re->netdev->dev, rxi->rx_dma,
					 MAX_RX_LENGTH, DMA_FROM_DEVICE);
	}

	for (i = 0; i < NUM_TX_DESC; i++) {
		struct raeth_tx_info *txi;

		txi = &re->tx_info[i];
		if (txi->tx_skb) {
			dev_kfree_skb_any(txi->tx_skb);
			txi->tx_skb = NULL;
		}
	}
}

#if defined(CONFIG_RALINK_RT288X) || defined(CONFIG_RALINK_RT3883)

#define RAMIPS_MDIO_RETRY	1000

static unsigned char *ramips_speed_str(struct raeth_priv *re)
{
	switch (re->speed) {
	case SPEED_1000:
		return "1000";
	case SPEED_100:
		return "100";
	case SPEED_10:
		return "10";
	}

	return "?";
}

static void ramips_link_adjust(struct raeth_priv *re)
{
	struct ramips_eth_platform_data *pdata;
	u32 mdio_cfg;

	pdata = re->parent->platform_data;
	if (!re->link) {
		netif_carrier_off(re->netdev);
		netdev_info(re->netdev, "link down\n");
		return;
	}

	mdio_cfg = RAMIPS_MDIO_CFG_TX_CLK_SKEW_200 |
		   RAMIPS_MDIO_CFG_TX_CLK_SKEW_200 |
		   RAMIPS_MDIO_CFG_GP1_FRC_EN;

	if (re->duplex == DUPLEX_FULL)
		mdio_cfg |= RAMIPS_MDIO_CFG_GP1_DUPLEX;

	if (re->tx_fc)
		mdio_cfg |= RAMIPS_MDIO_CFG_GP1_FC_TX;

	if (re->rx_fc)
		mdio_cfg |= RAMIPS_MDIO_CFG_GP1_FC_RX;

	switch (re->speed) {
	case SPEED_10:
		mdio_cfg |= RAMIPS_MDIO_CFG_GP1_SPEED_10;
		break;
	case SPEED_100:
		mdio_cfg |= RAMIPS_MDIO_CFG_GP1_SPEED_100;
		break;
	case SPEED_1000:
		mdio_cfg |= RAMIPS_MDIO_CFG_GP1_SPEED_1000;
		break;
	default:
		BUG();
	}

	ramips_fe_wr(mdio_cfg, RAMIPS_MDIO_CFG);

	netif_carrier_on(re->netdev);
	netdev_info(re->netdev, "link up (%sMbps/%s duplex)\n",
		    ramips_speed_str(re),
		    (DUPLEX_FULL == re->duplex) ? "Full" : "Half");
}

static int
ramips_mdio_wait_ready(struct raeth_priv *re)
{
	int retries;

	retries = RAMIPS_MDIO_RETRY;
	while (1) {
		u32 t;

		t = ramips_fe_rr(RAMIPS_MDIO_ACCESS);
		if ((t & (0x1 << 31)) == 0)
			return 0;

		if (retries-- == 0)
			break;

		udelay(1);
	}

	dev_err(re->parent, "MDIO operation timed out\n");
	return -ETIMEDOUT;
}

static int
ramips_mdio_read(struct mii_bus *bus, int phy_addr, int phy_reg)
{
	struct raeth_priv *re = bus->priv;
	int err;
	u32 t;

	err = ramips_mdio_wait_ready(re);
	if (err)
		return 0xffff;

	t = (phy_addr << 24) | (phy_reg << 16);
	ramips_fe_wr(t, RAMIPS_MDIO_ACCESS);
	t |= (1 << 31);
	ramips_fe_wr(t, RAMIPS_MDIO_ACCESS);

	err = ramips_mdio_wait_ready(re);
	if (err)
		return 0xffff;

	RADEBUG("%s: addr=%04x, reg=%04x, value=%04x\n", __func__,
		phy_addr, phy_reg, ramips_fe_rr(RAMIPS_MDIO_ACCESS) & 0xffff);

	return ramips_fe_rr(RAMIPS_MDIO_ACCESS) & 0xffff;
}

static int
ramips_mdio_write(struct mii_bus *bus, int phy_addr, int phy_reg, u16 val)
{
	struct raeth_priv *re = bus->priv;
	int err;
	u32 t;

	RADEBUG("%s: addr=%04x, reg=%04x, value=%04x\n", __func__,
		phy_addr, phy_reg, ramips_fe_rr(RAMIPS_MDIO_ACCESS) & 0xffff);

	err = ramips_mdio_wait_ready(re);
	if (err)
		return err;

	t = (1 << 30) | (phy_addr << 24) | (phy_reg << 16) | val;
	ramips_fe_wr(t, RAMIPS_MDIO_ACCESS);
	t |= (1 << 31);
	ramips_fe_wr(t, RAMIPS_MDIO_ACCESS);

	return ramips_mdio_wait_ready(re);
}

static int
ramips_mdio_reset(struct mii_bus *bus)
{
	/* TODO */
	return 0;
}

static int
ramips_mdio_init(struct raeth_priv *re)
{
	int err;
	int i;

	re->mii_bus = mdiobus_alloc();
	if (re->mii_bus == NULL)
		return -ENOMEM;

	re->mii_bus->name = "ramips_mdio";
	re->mii_bus->read = ramips_mdio_read;
	re->mii_bus->write = ramips_mdio_write;
	re->mii_bus->reset = ramips_mdio_reset;
	re->mii_bus->irq = re->mii_irq;
	re->mii_bus->priv = re;
	re->mii_bus->parent = re->parent;

	snprintf(re->mii_bus->id, MII_BUS_ID_SIZE, "%s", "ramips_mdio");
	re->mii_bus->phy_mask = 0;

	for (i = 0; i < PHY_MAX_ADDR; i++)
		re->mii_irq[i] = PHY_POLL;

	err = mdiobus_register(re->mii_bus);
	if (err)
		goto err_free_bus;

	return 0;

err_free_bus:
	kfree(re->mii_bus);
	return err;
}

static void
ramips_mdio_cleanup(struct raeth_priv *re)
{
	mdiobus_unregister(re->mii_bus);
	kfree(re->mii_bus);
}

static void
ramips_phy_link_adjust(struct net_device *dev)
{
	struct raeth_priv *re = netdev_priv(dev);
	struct phy_device *phydev = re->phy_dev;
	unsigned long flags;
	int status_change = 0;

	spin_lock_irqsave(&re->phy_lock, flags);

	if (phydev->link)
		if (re->duplex != phydev->duplex ||
		    re->speed != phydev->speed)
			status_change = 1;

	if (phydev->link != re->link)
		status_change = 1;

	re->link = phydev->link;
	re->duplex = phydev->duplex;
	re->speed = phydev->speed;

	if (status_change)
		ramips_link_adjust(re);

	spin_unlock_irqrestore(&re->phy_lock, flags);
}

static int
ramips_phy_connect_multi(struct raeth_priv *re)
{
	struct net_device *netdev = re->netdev;
	struct ramips_eth_platform_data *pdata;
	struct phy_device *phydev = NULL;
	int phy_addr;
	int ret = 0;

	pdata = re->parent->platform_data;
	for (phy_addr = 0; phy_addr < PHY_MAX_ADDR; phy_addr++) {
		if (!(pdata->phy_mask & (1 << phy_addr)))
			continue;

		if (re->mii_bus->phy_map[phy_addr] == NULL)
			continue;

		RADEBUG("%s: PHY found at %s, uid=%08x\n",
			netdev->name,
			dev_name(&re->mii_bus->phy_map[phy_addr]->dev),
			re->mii_bus->phy_map[phy_addr]->phy_id);

		if (phydev == NULL)
			phydev = re->mii_bus->phy_map[phy_addr];
	}

	if (!phydev) {
		netdev_err(netdev, "no PHY found with phy_mask=%08x\n",
			   pdata->phy_mask);
		return -ENODEV;
	}

	re->phy_dev = phy_connect(netdev, dev_name(&phydev->dev),
				  ramips_phy_link_adjust, 0,
				  pdata->phy_if_mode);

	if (IS_ERR(re->phy_dev)) {
		netdev_err(netdev, "could not connect to PHY at %s\n",
			   dev_name(&phydev->dev));
		return PTR_ERR(re->phy_dev);
	}

	phydev->supported &= PHY_GBIT_FEATURES;
	phydev->advertising = phydev->supported;

	RADEBUG("%s: connected to PHY at %s [uid=%08x, driver=%s]\n",
		netdev->name, dev_name(&phydev->dev),
		phydev->phy_id, phydev->drv->name);

	re->link = 0;
	re->speed = 0;
	re->duplex = -1;
	re->rx_fc = 0;
	re->tx_fc = 0;

	return ret;
}

static int
ramips_phy_connect_fixed(struct raeth_priv *re)
{
	struct ramips_eth_platform_data *pdata;

	pdata = re->parent->platform_data;
	switch (pdata->speed) {
	case SPEED_10:
	case SPEED_100:
	case SPEED_1000:
		break;
	default:
		netdev_err(re->netdev, "invalid speed specified\n");
		return -EINVAL;
	}

	RADEBUG("%s: using fixed link parameters\n", re->netdev->name);

	re->speed = pdata->speed;
	re->duplex = pdata->duplex;
	re->tx_fc = pdata->tx_fc;
	re->rx_fc = pdata->tx_fc;

	return 0;
}

static int
ramips_phy_connect(struct raeth_priv *re)
{
	struct ramips_eth_platform_data *pdata;

	pdata = re->parent->platform_data;
	if (pdata->phy_mask)
		return ramips_phy_connect_multi(re);

	return ramips_phy_connect_fixed(re);
}

static void
ramips_phy_disconnect(struct raeth_priv *re)
{
	if (re->phy_dev)
		phy_disconnect(re->phy_dev);
}

static void
ramips_phy_start(struct raeth_priv *re)
{
	unsigned long flags;

	if (re->phy_dev) {
		phy_start(re->phy_dev);
	} else {
		spin_lock_irqsave(&re->phy_lock, flags);
		re->link = 1;
		ramips_link_adjust(re);
		spin_unlock_irqrestore(&re->phy_lock, flags);
	}
}

static void
ramips_phy_stop(struct raeth_priv *re)
{
	unsigned long flags;

	if (re->phy_dev)
		phy_stop(re->phy_dev);

	spin_lock_irqsave(&re->phy_lock, flags);
	re->link = 0;
	ramips_link_adjust(re);
	spin_unlock_irqrestore(&re->phy_lock, flags);
}
#else
static inline int
ramips_mdio_init(struct raeth_priv *re)
{
	return 0;
}

static inline void
ramips_mdio_cleanup(struct raeth_priv *re)
{
}

static inline int
ramips_phy_connect(struct raeth_priv *re)
{
	return 0;
}

static inline void
ramips_phy_disconnect(struct raeth_priv *re)
{
}

static inline void
ramips_phy_start(struct raeth_priv *re)
{
}

static inline void
ramips_phy_stop(struct raeth_priv *re)
{
}
#endif /* CONFIG_RALINK_RT288X || CONFIG_RALINK_RT3883 */

static void
ramips_ring_free(struct raeth_priv *re)
{
	int len;
	int i;

	if (re->rx_info) {
		for (i = 0; i < NUM_RX_DESC; i++) {
			struct raeth_rx_info *rxi;

			rxi = &re->rx_info[i];
			if (rxi->rx_skb)
				dev_kfree_skb_any(rxi->rx_skb);
		}
		kfree(re->rx_info);
	}

	if (re->rx) {
		len = NUM_RX_DESC * sizeof(struct ramips_rx_dma);
		dma_free_coherent(&re->netdev->dev, len, re->rx,
				  re->rx_desc_dma);
	}

	if (re->tx) {
		len = NUM_TX_DESC * sizeof(struct ramips_tx_dma);
		dma_free_coherent(&re->netdev->dev, len, re->tx,
				  re->tx_desc_dma);
	}

	kfree(re->tx_info);
}

static int
ramips_ring_alloc(struct raeth_priv *re)
{
	int len;
	int err = -ENOMEM;
	int i;

	re->tx_info = kzalloc(NUM_TX_DESC * sizeof(struct raeth_tx_info),
			      GFP_ATOMIC);
	if (!re->tx_info)
		goto err_cleanup;

	re->rx_info = kzalloc(NUM_RX_DESC * sizeof(struct raeth_rx_info),
			      GFP_ATOMIC);
	if (!re->rx_info)
		goto err_cleanup;

	/* allocate tx ring */
	len = NUM_TX_DESC * sizeof(struct ramips_tx_dma);
	re->tx = dma_alloc_coherent(&re->netdev->dev, len,
					  &re->tx_desc_dma, GFP_ATOMIC);
	if (!re->tx)
		goto err_cleanup;

	/* allocate rx ring */
	len = NUM_RX_DESC * sizeof(struct ramips_rx_dma);
	re->rx = dma_alloc_coherent(&re->netdev->dev, len,
				    &re->rx_desc_dma, GFP_ATOMIC);
	if (!re->rx)
		goto err_cleanup;

	for (i = 0; i < NUM_RX_DESC; i++) {
		struct sk_buff *skb;

		skb = ramips_alloc_skb(re);
		if (!skb)
			goto err_cleanup;

		re->rx_info[i].rx_skb = skb;
	}

	return 0;

err_cleanup:
	ramips_ring_free(re);
	return err;
}

static void
ramips_setup_dma(struct raeth_priv *re)
{
	ramips_fe_wr(re->tx_desc_dma, RAMIPS_TX_BASE_PTR0);
	ramips_fe_wr(NUM_TX_DESC, RAMIPS_TX_MAX_CNT0);
	ramips_fe_wr(0, RAMIPS_TX_CTX_IDX0);
	ramips_fe_wr(RAMIPS_PST_DTX_IDX0, RAMIPS_PDMA_RST_CFG);

	ramips_fe_wr(re->rx_desc_dma, RAMIPS_RX_BASE_PTR0);
	ramips_fe_wr(NUM_RX_DESC, RAMIPS_RX_MAX_CNT0);
	ramips_fe_wr((NUM_RX_DESC - 1), RAMIPS_RX_CALC_IDX0);
	ramips_fe_wr(RAMIPS_PST_DRX_IDX0, RAMIPS_PDMA_RST_CFG);
}

static int
ramips_eth_hard_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
	struct raeth_priv *re = netdev_priv(dev);
	struct raeth_tx_info *txi, *txi_next;
	struct ramips_tx_dma *txd, *txd_next;
	unsigned long tx;
	unsigned int tx_next;
	dma_addr_t mapped_addr;

	if (re->plat->min_pkt_len) {
		if (skb->len < re->plat->min_pkt_len) {
			if (skb_padto(skb, re->plat->min_pkt_len)) {
				printk(KERN_ERR
				       "ramips_eth: skb_padto failed\n");
				kfree_skb(skb);
				return 0;
			}
			skb_put(skb, re->plat->min_pkt_len - skb->len);
		}
	}

	dev->trans_start = jiffies;
	mapped_addr = dma_map_single(&re->netdev->dev, skb->data, skb->len,
				     DMA_TO_DEVICE);

	spin_lock(&re->page_lock);
	tx = ramips_fe_rr(RAMIPS_TX_CTX_IDX0);
	tx_next = (tx + 1) % NUM_TX_DESC;

	txi = &re->tx_info[tx];
	txd = txi->tx_desc;
	txi_next = &re->tx_info[tx_next];
	txd_next = txi_next->tx_desc;

	if ((txi->tx_skb) || (txi_next->tx_skb) ||
	    !(txd->txd2 & TX_DMA_DONE) ||
	    !(txd_next->txd2 & TX_DMA_DONE))
		goto out;

	txi->tx_skb = skb;

	txd->txd1 = (unsigned int) mapped_addr;
	wmb();
	txd->txd2 = TX_DMA_LSO | TX_DMA_PLEN0(skb->len);
	dev->stats.tx_packets++;
	dev->stats.tx_bytes += skb->len;
	ramips_fe_wr(tx_next, RAMIPS_TX_CTX_IDX0);
	spin_unlock(&re->page_lock);
	return NETDEV_TX_OK;

 out:
	spin_unlock(&re->page_lock);
	dev->stats.tx_dropped++;
	kfree_skb(skb);
	return NETDEV_TX_OK;
}

static void
ramips_eth_rx_hw(unsigned long ptr)
{
	struct net_device *dev = (struct net_device *) ptr;
	struct raeth_priv *re = netdev_priv(dev);
	int rx;
	int max_rx = 16;

	while (max_rx) {
		struct raeth_rx_info *rxi;
		struct ramips_rx_dma *rxd;
		struct sk_buff *rx_skb, *new_skb;
		int pktlen;

		rx = (ramips_fe_rr(RAMIPS_RX_CALC_IDX0) + 1) % NUM_RX_DESC;

		rxi = &re->rx_info[rx];
		rxd = rxi->rx_desc;
		if (!(rxd->rxd2 & RX_DMA_DONE))
			break;

		rx_skb = rxi->rx_skb;
		pktlen = RX_DMA_PLEN0(rxd->rxd2);

		new_skb = ramips_alloc_skb(re);
		/* Reuse the buffer on allocation failures */
		if (new_skb) {
			dma_addr_t dma_addr;

			dma_unmap_single(&re->netdev->dev, rxi->rx_dma,
					 MAX_RX_LENGTH, DMA_FROM_DEVICE);

			skb_put(rx_skb, pktlen);
			rx_skb->dev = dev;
			rx_skb->protocol = eth_type_trans(rx_skb, dev);
			rx_skb->ip_summed = CHECKSUM_NONE;
			dev->stats.rx_packets++;
			dev->stats.rx_bytes += pktlen;
			netif_rx(rx_skb);

			rxi->rx_skb = new_skb;

			dma_addr = dma_map_single(&re->netdev->dev,
						  new_skb->data,
						  MAX_RX_LENGTH,
						  DMA_FROM_DEVICE);
			rxi->rx_dma = dma_addr;
			rxd->rxd1 = (unsigned int) dma_addr;
			wmb();
		} else {
			dev->stats.rx_dropped++;
		}

		rxd->rxd2 = RX_DMA_LSO;
		ramips_fe_wr(rx, RAMIPS_RX_CALC_IDX0);
		max_rx--;
	}

	if (max_rx == 0)
		tasklet_schedule(&re->rx_tasklet);
	else
		ramips_fe_int_enable(RAMIPS_RX_DLY_INT);
}

static void
ramips_eth_tx_housekeeping(unsigned long ptr)
{
	struct net_device *dev = (struct net_device*)ptr;
	struct raeth_priv *re = netdev_priv(dev);

	spin_lock(&re->page_lock);
	while (1) {
		struct raeth_tx_info *txi;
		struct ramips_tx_dma *txd;

		txi = &re->tx_info[re->skb_free_idx];
		txd = txi->tx_desc;

		if (!(txd->txd2 & TX_DMA_DONE) || !(txi->tx_skb))
			break;

		dev_kfree_skb_irq(txi->tx_skb);
		txi->tx_skb = NULL;
		re->skb_free_idx++;
		if (re->skb_free_idx >= NUM_TX_DESC)
			re->skb_free_idx = 0;
	}
	spin_unlock(&re->page_lock);

	ramips_fe_int_enable(RAMIPS_TX_DLY_INT);
}

static void
ramips_eth_timeout(struct net_device *dev)
{
	struct raeth_priv *re = netdev_priv(dev);

	tasklet_schedule(&re->tx_housekeeping_tasklet);
}

static irqreturn_t
ramips_eth_irq(int irq, void *dev)
{
	struct raeth_priv *re = netdev_priv(dev);
	unsigned int status;

	status = ramips_fe_rr(RAMIPS_FE_INT_STATUS);
	status &= ramips_fe_rr(RAMIPS_FE_INT_ENABLE);

	if (!status)
		return IRQ_NONE;

	ramips_fe_wr(status, RAMIPS_FE_INT_STATUS);

	if (status & RAMIPS_RX_DLY_INT) {
		ramips_fe_int_disable(RAMIPS_RX_DLY_INT);
		tasklet_schedule(&re->rx_tasklet);
	}

	if (status & RAMIPS_TX_DLY_INT) {
		ramips_fe_int_disable(RAMIPS_TX_DLY_INT);
		tasklet_schedule(&re->tx_housekeeping_tasklet);
	}

	raeth_debugfs_update_int_stats(re, status);

	return IRQ_HANDLED;
}

static int
ramips_eth_open(struct net_device *dev)
{
	struct raeth_priv *re = netdev_priv(dev);
	int err;

	err = request_irq(dev->irq, ramips_eth_irq, IRQF_DISABLED,
			  dev->name, dev);
	if (err)
		return err;

	err = ramips_ring_alloc(re);
	if (err)
		goto err_free_irq;

	ramips_ring_setup(re);
	ramips_hw_set_macaddr(dev->dev_addr);

	ramips_setup_dma(re);
	ramips_fe_wr((ramips_fe_rr(RAMIPS_PDMA_GLO_CFG) & 0xff) |
		(RAMIPS_TX_WB_DDONE | RAMIPS_RX_DMA_EN |
		RAMIPS_TX_DMA_EN | RAMIPS_PDMA_SIZE_4DWORDS),
		RAMIPS_PDMA_GLO_CFG);
	ramips_fe_wr((ramips_fe_rr(RAMIPS_FE_GLO_CFG) &
		~(RAMIPS_US_CYC_CNT_MASK << RAMIPS_US_CYC_CNT_SHIFT)) |
		((re->plat->sys_freq / RAMIPS_US_CYC_CNT_DIVISOR) << RAMIPS_US_CYC_CNT_SHIFT),
		RAMIPS_FE_GLO_CFG);

	tasklet_init(&re->tx_housekeeping_tasklet, ramips_eth_tx_housekeeping,
		     (unsigned long)dev);
	tasklet_init(&re->rx_tasklet, ramips_eth_rx_hw, (unsigned long)dev);

	ramips_phy_start(re);

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
	struct raeth_priv *re = netdev_priv(dev);

	ramips_fe_wr(ramips_fe_rr(RAMIPS_PDMA_GLO_CFG) &
		     ~(RAMIPS_TX_WB_DDONE | RAMIPS_RX_DMA_EN | RAMIPS_TX_DMA_EN),
		     RAMIPS_PDMA_GLO_CFG);

	/* disable all interrupts in the hw */
	ramips_fe_wr(0, RAMIPS_FE_INT_ENABLE);

	ramips_phy_stop(re);
	free_irq(dev->irq, dev);
	netif_stop_queue(dev);
	tasklet_kill(&re->tx_housekeeping_tasklet);
	tasklet_kill(&re->rx_tasklet);
	ramips_ring_cleanup(re);
	ramips_ring_free(re);
	RADEBUG("ramips_eth: stopped\n");
	return 0;
}

static int __init
ramips_eth_probe(struct net_device *dev)
{
	struct raeth_priv *re = netdev_priv(dev);
	int err;

	BUG_ON(!re->plat->reset_fe);
	re->plat->reset_fe();
	net_srandom(jiffies);
	memcpy(dev->dev_addr, re->plat->mac, ETH_ALEN);

	ether_setup(dev);
	dev->mtu = 1500;
	dev->watchdog_timeo = TX_TIMEOUT;
	spin_lock_init(&re->page_lock);
	spin_lock_init(&re->phy_lock);

	err = ramips_mdio_init(re);
	if (err)
		return err;

	err = ramips_phy_connect(re);
	if (err)
		goto err_mdio_cleanup;

	err = raeth_debugfs_init(re);
	if (err)
		goto err_phy_disconnect;

	return 0;

err_phy_disconnect:
	ramips_phy_disconnect(re);
err_mdio_cleanup:
	ramips_mdio_cleanup(re);
	return err;
}

static void
ramips_eth_uninit(struct net_device *dev)
{
	struct raeth_priv *re = netdev_priv(dev);

	raeth_debugfs_exit(re);
	ramips_phy_disconnect(re);
	ramips_mdio_cleanup(re);
}

static const struct net_device_ops ramips_eth_netdev_ops = {
	.ndo_init		= ramips_eth_probe,
	.ndo_uninit		= ramips_eth_uninit,
	.ndo_open		= ramips_eth_open,
	.ndo_stop		= ramips_eth_stop,
	.ndo_start_xmit		= ramips_eth_hard_start_xmit,
	.ndo_tx_timeout		= ramips_eth_timeout,
	.ndo_change_mtu		= eth_change_mtu,
	.ndo_set_mac_address	= eth_mac_addr,
	.ndo_validate_addr	= eth_validate_addr,
};

static int
ramips_eth_plat_probe(struct platform_device *plat)
{
	struct raeth_priv *re;
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
	ramips_dev->netdev_ops = &ramips_eth_netdev_ops;

	re = netdev_priv(ramips_dev);

	re->netdev = ramips_dev;
	re->parent = &plat->dev;
	re->speed = data->speed;
	re->duplex = data->duplex;
	re->rx_fc = data->rx_fc;
	re->tx_fc = data->tx_fc;
	re->plat = data;

	err = register_netdev(ramips_dev);
	if (err) {
		dev_err(&plat->dev, "error bringing up device\n");
		goto err_free_dev;
	}

	RADEBUG("ramips_eth: loaded\n");
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
	RADEBUG("ramips_eth: unloaded\n");
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
	int ret;

	ret = raeth_debugfs_root_init();
	if (ret)
		goto err_out;

	ret = rt305x_esw_init();
	if (ret)
		goto err_debugfs_exit;

	ret = platform_driver_register(&ramips_eth_driver);
	if (ret) {
		printk(KERN_ERR
		       "ramips_eth: Error registering platfom driver!\n");
		goto esw_cleanup;
	}

	return 0;

esw_cleanup:
	rt305x_esw_exit();
err_debugfs_exit:
	raeth_debugfs_root_exit();
err_out:
	return ret;
}

static void __exit
ramips_eth_cleanup(void)
{
	platform_driver_unregister(&ramips_eth_driver);
	rt305x_esw_exit();
	raeth_debugfs_root_exit();
}

module_init(ramips_eth_init);
module_exit(ramips_eth_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("John Crispin <blogic@openwrt.org>");
MODULE_DESCRIPTION("ethernet driver for ramips boards");

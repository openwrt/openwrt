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
 *   Copyright (C) 2009-2013 John Crispin <blogic@openwrt.org>
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
#include <linux/of_device.h>
#include <linux/clk.h>
#include <linux/of_net.h>
#include <linux/of_mdio.h>
#include <linux/if_vlan.h>
#include <linux/reset.h>
#include <linux/tcp.h>
#include <linux/io.h>

#include <asm/mach-ralink/ralink_regs.h>

#include "ralink_soc_eth.h"
#include "esw_rt3052.h"
#include "mdio.h"
#include "ralink_ethtool.h"

#define TX_TIMEOUT		(2 * HZ)
#define	MAX_RX_LENGTH		1536
#define FE_RX_OFFSET		(NET_SKB_PAD + NET_IP_ALIGN)
#define FE_RX_HLEN		(FE_RX_OFFSET + VLAN_ETH_HLEN + VLAN_HLEN + \
		ETH_FCS_LEN)
#define DMA_DUMMY_DESC		0xffffffff
#define FE_DEFAULT_MSG_ENABLE    \
        (NETIF_MSG_DRV      | \
         NETIF_MSG_PROBE    | \
         NETIF_MSG_LINK     | \
         NETIF_MSG_TIMER    | \
         NETIF_MSG_IFDOWN   | \
         NETIF_MSG_IFUP     | \
         NETIF_MSG_RX_ERR   | \
         NETIF_MSG_TX_ERR)

#define TX_DMA_DESP2_DEF	(TX_DMA_LS0 | TX_DMA_DONE)
#define TX_DMA_DESP4_DEF	(TX_DMA_QN(3) | TX_DMA_PN(1))
#define NEXT_TX_DESP_IDX(X)	(((X) + 1) & (NUM_DMA_DESC - 1))
#define NEXT_RX_DESP_IDX(X)	(((X) + 1) & (NUM_DMA_DESC - 1))

static int fe_msg_level = -1;
module_param_named(msg_level, fe_msg_level, int, 0);
MODULE_PARM_DESC(msg_level, "Message level (-1=defaults,0=none,...,16=all)");

static const u32 fe_reg_table_default[FE_REG_COUNT] = {
	[FE_REG_PDMA_GLO_CFG] = FE_PDMA_GLO_CFG,
	[FE_REG_PDMA_RST_CFG] = FE_PDMA_RST_CFG,
	[FE_REG_DLY_INT_CFG] = FE_DLY_INT_CFG,
	[FE_REG_TX_BASE_PTR0] = FE_TX_BASE_PTR0,
	[FE_REG_TX_MAX_CNT0] = FE_TX_MAX_CNT0,
	[FE_REG_TX_CTX_IDX0] = FE_TX_CTX_IDX0,
	[FE_REG_RX_BASE_PTR0] = FE_RX_BASE_PTR0,
	[FE_REG_RX_MAX_CNT0] = FE_RX_MAX_CNT0,
	[FE_REG_RX_CALC_IDX0] = FE_RX_CALC_IDX0,
	[FE_REG_FE_INT_ENABLE] = FE_FE_INT_ENABLE,
	[FE_REG_FE_INT_STATUS] = FE_FE_INT_STATUS,
	[FE_REG_FE_DMA_VID_BASE] = FE_DMA_VID0,
	[FE_REG_FE_COUNTER_BASE] = FE_GDMA1_TX_GBCNT,
	[FE_REG_FE_RST_GL] = FE_FE_RST_GL,
};

static const u32 *fe_reg_table = fe_reg_table_default;

static void __iomem *fe_base = 0;

void fe_w32(u32 val, unsigned reg)
{
	__raw_writel(val, fe_base + reg);
}

u32 fe_r32(unsigned reg)
{
	return __raw_readl(fe_base + reg);
}

void fe_reg_w32(u32 val, enum fe_reg reg)
{
	fe_w32(val, fe_reg_table[reg]);
}

u32 fe_reg_r32(enum fe_reg reg)
{
	return fe_r32(fe_reg_table[reg]);
}

static inline void fe_int_disable(u32 mask)
{
	fe_reg_w32(fe_reg_r32(FE_REG_FE_INT_ENABLE) & ~mask,
		     FE_REG_FE_INT_ENABLE);
	/* flush write */
	fe_reg_r32(FE_REG_FE_INT_ENABLE);
}

static inline void fe_int_enable(u32 mask)
{
	fe_reg_w32(fe_reg_r32(FE_REG_FE_INT_ENABLE) | mask,
		     FE_REG_FE_INT_ENABLE);
	/* flush write */
	fe_reg_r32(FE_REG_FE_INT_ENABLE);
}

static inline void fe_hw_set_macaddr(struct fe_priv *priv, unsigned char *mac)
{
	unsigned long flags;

	spin_lock_irqsave(&priv->page_lock, flags);
	fe_w32((mac[0] << 8) | mac[1], FE_GDMA1_MAC_ADRH);
	fe_w32((mac[2] << 24) | (mac[3] << 16) | (mac[4] << 8) | mac[5],
		     FE_GDMA1_MAC_ADRL);
	spin_unlock_irqrestore(&priv->page_lock, flags);
}

static int fe_set_mac_address(struct net_device *dev, void *p)
{
	int ret = eth_mac_addr(dev, p);

	if (!ret) {
		struct fe_priv *priv = netdev_priv(dev);

		if (priv->soc->set_mac)
			priv->soc->set_mac(priv, dev->dev_addr);
		else
			fe_hw_set_macaddr(priv, p);
	}

	return ret;
}

static inline int fe_max_frag_size(int mtu)
{
	return SKB_DATA_ALIGN(FE_RX_HLEN + mtu) +
		SKB_DATA_ALIGN(sizeof(struct skb_shared_info));
}

static inline int fe_max_buf_size(int frag_size)
{
	return frag_size - FE_RX_HLEN -
		SKB_DATA_ALIGN(sizeof(struct skb_shared_info));
}

static inline void fe_get_rxd(struct fe_rx_dma *rxd, struct fe_rx_dma *dma_rxd)
{
	rxd->rxd1 = dma_rxd->rxd1;
	rxd->rxd2 = dma_rxd->rxd2;
	rxd->rxd3 = dma_rxd->rxd3;
	rxd->rxd4 = dma_rxd->rxd4;
}

static inline void fe_get_txd(struct fe_tx_dma *txd, struct fe_tx_dma *dma_txd)
{
	txd->txd1 = dma_txd->txd1;
	txd->txd2 = dma_txd->txd2;
	txd->txd3 = dma_txd->txd3;
	txd->txd4 = dma_txd->txd4;
}

static inline void fe_set_txd(struct fe_tx_dma *txd, struct fe_tx_dma *dma_txd)
{
	dma_txd->txd1 = txd->txd1;
	dma_txd->txd3 = txd->txd3;
	dma_txd->txd4 = txd->txd4;
	/* clean dma done flag last */
	dma_txd->txd2 = txd->txd2;
}

static void fe_clean_rx(struct fe_priv *priv)
{
	int i;

	if (priv->rx_data) {
		for (i = 0; i < NUM_DMA_DESC; i++)
			if (priv->rx_data[i]) {
				if (priv->rx_dma && priv->rx_dma[i].rxd1)
					dma_unmap_single(&priv->netdev->dev,
							priv->rx_dma[i].rxd1,
							priv->rx_buf_size,
							DMA_FROM_DEVICE);
				put_page(virt_to_head_page(priv->rx_data[i]));
			}

		kfree(priv->rx_data);
		priv->rx_data = NULL;
	}

	if (priv->rx_dma) {
		dma_free_coherent(&priv->netdev->dev,
				NUM_DMA_DESC * sizeof(*priv->rx_dma),
				priv->rx_dma,
				priv->rx_phys);
		priv->rx_dma = NULL;
	}
}

static int fe_alloc_rx(struct fe_priv *priv)
{
	struct net_device *netdev = priv->netdev;
	int i;

	priv->rx_data = kcalloc(NUM_DMA_DESC, sizeof(*priv->rx_data),
			GFP_KERNEL);
	if (!priv->rx_data)
		goto no_rx_mem;

	for (i = 0; i < NUM_DMA_DESC; i++) {
		priv->rx_data[i] = netdev_alloc_frag(priv->frag_size);
		if (!priv->rx_data[i])
			goto no_rx_mem;
	}

	priv->rx_dma = dma_alloc_coherent(&netdev->dev,
			NUM_DMA_DESC * sizeof(*priv->rx_dma),
			&priv->rx_phys,
			GFP_ATOMIC | __GFP_ZERO);
	if (!priv->rx_dma)
		goto no_rx_mem;

	for (i = 0; i < NUM_DMA_DESC; i++) {
		dma_addr_t dma_addr = dma_map_single(&netdev->dev,
				priv->rx_data[i] + FE_RX_OFFSET,
				priv->rx_buf_size,
				DMA_FROM_DEVICE);
		if (unlikely(dma_mapping_error(&netdev->dev, dma_addr)))
			goto no_rx_mem;
		priv->rx_dma[i].rxd1 = (unsigned int) dma_addr;

		if (priv->soc->rx_dma)
			priv->soc->rx_dma(&priv->rx_dma[i], priv->rx_buf_size);
		else
			priv->rx_dma[i].rxd2 = RX_DMA_LSO;
	}
	wmb();

	fe_reg_w32(priv->rx_phys, FE_REG_RX_BASE_PTR0);
	fe_reg_w32(NUM_DMA_DESC, FE_REG_RX_MAX_CNT0);
	fe_reg_w32((NUM_DMA_DESC - 1), FE_REG_RX_CALC_IDX0);
	fe_reg_w32(FE_PST_DRX_IDX0, FE_REG_PDMA_RST_CFG);

	return 0;

no_rx_mem:
	return -ENOMEM;
}

static void fe_clean_tx(struct fe_priv *priv)
{
	int i;

	if (priv->tx_skb) {
		for (i = 0; i < NUM_DMA_DESC; i++) {
			if (priv->tx_skb[i])
				dev_kfree_skb_any(priv->tx_skb[i]);
		}
		kfree(priv->tx_skb);
		priv->tx_skb = NULL;
	}

	if (priv->tx_dma) {
		dma_free_coherent(&priv->netdev->dev,
				NUM_DMA_DESC * sizeof(*priv->tx_dma),
				priv->tx_dma,
				priv->tx_phys);
		priv->tx_dma = NULL;
	}
}

static int fe_alloc_tx(struct fe_priv *priv)
{
	int i;

	priv->tx_free_idx = 0;

	priv->tx_skb = kcalloc(NUM_DMA_DESC, sizeof(*priv->tx_skb),
			GFP_KERNEL);
	if (!priv->tx_skb)
		goto no_tx_mem;

	priv->tx_dma = dma_alloc_coherent(&priv->netdev->dev,
			NUM_DMA_DESC * sizeof(*priv->tx_dma),
			&priv->tx_phys,
			GFP_ATOMIC | __GFP_ZERO);
	if (!priv->tx_dma)
		goto no_tx_mem;

	for (i = 0; i < NUM_DMA_DESC; i++) {
		if (priv->soc->tx_dma) {
			priv->soc->tx_dma(&priv->tx_dma[i]);
			continue;
		}
		priv->tx_dma[i].txd2 = TX_DMA_DESP2_DEF;
	}
	wmb();

	fe_reg_w32(priv->tx_phys, FE_REG_TX_BASE_PTR0);
	fe_reg_w32(NUM_DMA_DESC, FE_REG_TX_MAX_CNT0);
	fe_reg_w32(0, FE_REG_TX_CTX_IDX0);
	fe_reg_w32(FE_PST_DTX_IDX0, FE_REG_PDMA_RST_CFG);

	return 0;

no_tx_mem:
	return -ENOMEM;
}

static int fe_init_dma(struct fe_priv *priv)
{
	int err;

	err = fe_alloc_tx(priv);
	if (err)
		return err;

	err = fe_alloc_rx(priv);
	if (err)
		return err;

	return 0;
}

static void fe_free_dma(struct fe_priv *priv)
{
	fe_clean_tx(priv);
	fe_clean_rx(priv);

	netdev_reset_queue(priv->netdev);
}

static inline void txd_unmap_single(struct device *dev, struct fe_tx_dma *txd)
{
	if (txd->txd1 && TX_DMA_GET_PLEN0(txd->txd2))
		dma_unmap_single(dev, txd->txd1,
				TX_DMA_GET_PLEN0(txd->txd2),
				DMA_TO_DEVICE);
}

static inline void txd_unmap_page0(struct device *dev, struct fe_tx_dma *txd)
{
	if (txd->txd1 && TX_DMA_GET_PLEN0(txd->txd2))
		dma_unmap_page(dev, txd->txd1,
				TX_DMA_GET_PLEN0(txd->txd2),
				DMA_TO_DEVICE);
}

static inline void txd_unmap_page1(struct device *dev, struct fe_tx_dma *txd)
{
	if (txd->txd3 && TX_DMA_GET_PLEN1(txd->txd2))
		dma_unmap_page(dev, txd->txd3,
				TX_DMA_GET_PLEN1(txd->txd2),
				DMA_TO_DEVICE);
}

void fe_stats_update(struct fe_priv *priv)
{
	struct fe_hw_stats *hwstats = priv->hw_stats;
	unsigned int base = fe_reg_table[FE_REG_FE_COUNTER_BASE];

	u64_stats_update_begin(&hwstats->syncp);

	if (IS_ENABLED(CONFIG_SOC_MT7621)) {
		hwstats->rx_bytes			+= fe_r32(base);
		hwstats->rx_packets			+= fe_r32(base + 0x08);
		hwstats->rx_overflow			+= fe_r32(base + 0x10);
		hwstats->rx_fcs_errors			+= fe_r32(base + 0x14);
		hwstats->rx_short_errors		+= fe_r32(base + 0x18);
		hwstats->rx_long_errors			+= fe_r32(base + 0x1c);
		hwstats->rx_checksum_errors		+= fe_r32(base + 0x20);
		hwstats->rx_flow_control_packets	+= fe_r32(base + 0x24);
		hwstats->tx_skip			+= fe_r32(base + 0x28);
		hwstats->tx_collisions			+= fe_r32(base + 0x2c);
		hwstats->tx_bytes			+= fe_r32(base + 0x30);
		hwstats->tx_packets			+= fe_r32(base + 0x38);
	} else {
		hwstats->tx_bytes			+= fe_r32(base);
		hwstats->tx_packets			+= fe_r32(base + 0x04);
		hwstats->tx_skip			+= fe_r32(base + 0x08);
		hwstats->tx_collisions			+= fe_r32(base + 0x0c);
		hwstats->rx_bytes			+= fe_r32(base + 0x20);
		hwstats->rx_packets			+= fe_r32(base + 0x24);
		hwstats->rx_overflow			+= fe_r32(base + 0x28);
		hwstats->rx_fcs_errors			+= fe_r32(base + 0x2c);
		hwstats->rx_short_errors		+= fe_r32(base + 0x30);
		hwstats->rx_long_errors			+= fe_r32(base + 0x34);
		hwstats->rx_checksum_errors		+= fe_r32(base + 0x38);
		hwstats->rx_flow_control_packets	+= fe_r32(base + 0x3c);
	}

	u64_stats_update_end(&hwstats->syncp);
}

static struct rtnl_link_stats64 *fe_get_stats64(struct net_device *dev,
		struct rtnl_link_stats64 *storage)
{
	struct fe_priv *priv = netdev_priv(dev);
	struct fe_hw_stats *hwstats = priv->hw_stats;
	unsigned int base = fe_reg_table[FE_REG_FE_COUNTER_BASE];
	unsigned int start;

	if (!base) {
		netdev_stats_to_stats64(storage, &dev->stats);
		return storage;
	}

	if (netif_running(dev) && netif_device_present(dev)) {
		if (spin_trylock(&hwstats->stats_lock)) {
			fe_stats_update(priv);
			spin_unlock(&hwstats->stats_lock);
		}
	}

	do {
		start = u64_stats_fetch_begin_bh(&hwstats->syncp);
		storage->rx_packets = hwstats->rx_packets;
		storage->tx_packets = hwstats->tx_packets;
		storage->rx_bytes = hwstats->rx_bytes;
		storage->tx_bytes = hwstats->tx_bytes;
		storage->collisions = hwstats->tx_collisions;
		storage->rx_length_errors = hwstats->rx_short_errors +
			hwstats->rx_long_errors;
		storage->rx_over_errors = hwstats->rx_overflow;
		storage->rx_crc_errors = hwstats->rx_fcs_errors;
		storage->rx_errors = hwstats->rx_checksum_errors;
		storage->tx_aborted_errors = hwstats->tx_skip;
	} while (u64_stats_fetch_retry_bh(&hwstats->syncp, start));

	storage->tx_errors = priv->netdev->stats.tx_errors;
	storage->rx_dropped = priv->netdev->stats.rx_dropped;
	storage->tx_dropped = priv->netdev->stats.tx_dropped;

	return storage;
}

static int fe_vlan_rx_add_vid(struct net_device *dev,
		__be16 proto, u16 vid)
{
	struct fe_priv *priv = netdev_priv(dev);
	u32 idx = (vid & 0xf);
	u32 vlan_cfg;

	if (!((fe_reg_table[FE_REG_FE_DMA_VID_BASE]) &&
			(dev->features | NETIF_F_HW_VLAN_CTAG_TX)))
		return 0;

	if (test_bit(idx, &priv->vlan_map)) {
		netdev_warn(dev, "disable tx vlan offload\n");
		dev->wanted_features &= ~NETIF_F_HW_VLAN_CTAG_TX;
		netdev_update_features(dev);
	} else {
		vlan_cfg = fe_r32(fe_reg_table[FE_REG_FE_DMA_VID_BASE] +
				((idx >> 1) << 2));
		if (idx & 0x1) {
			vlan_cfg &= 0xffff;
			vlan_cfg |= (vid << 16);
		} else {
			vlan_cfg &= 0xffff0000;
			vlan_cfg |= vid;
		}
		fe_w32(vlan_cfg, fe_reg_table[FE_REG_FE_DMA_VID_BASE] +
				((idx >> 1) << 2));
		set_bit(idx, &priv->vlan_map);
	}

	return 0;
}

static int fe_vlan_rx_kill_vid(struct net_device *dev,
		__be16 proto, u16 vid)
{
	struct fe_priv *priv = netdev_priv(dev);
	u32 idx = (vid & 0xf);

	if (!((fe_reg_table[FE_REG_FE_DMA_VID_BASE]) &&
				(dev->features | NETIF_F_HW_VLAN_CTAG_TX)))
		return 0;

	clear_bit(idx, &priv->vlan_map);

	return 0;
}

static int fe_tx_map_dma(struct sk_buff *skb, struct net_device *dev,
		int idx)
{
	struct fe_priv *priv = netdev_priv(dev);
	struct skb_frag_struct *frag;
	struct fe_tx_dma txd, *ptxd;
	dma_addr_t mapped_addr;
	unsigned int nr_frags;
	u32 def_txd4;
	int i, j, unmap_idx, tx_num;

	memset(&txd, 0, sizeof(txd));
	nr_frags = skb_shinfo(skb)->nr_frags;
	tx_num = 1 + (nr_frags >> 1);

	/* init tx descriptor */
	if (priv->soc->tx_dma)
		priv->soc->tx_dma(&txd);
	else
		txd.txd4 = TX_DMA_DESP4_DEF;
	def_txd4 = txd.txd4;

	/* use dma_unmap_single to free it */
	txd.txd4 |= priv->soc->tx_udf_bit;

	/* TX Checksum offload */
	if (skb->ip_summed == CHECKSUM_PARTIAL)
		txd.txd4 |= TX_DMA_CHKSUM;

	/* VLAN header offload */
	if (vlan_tx_tag_present(skb)) {
		if (IS_ENABLED(CONFIG_SOC_MT7621))
			txd.txd4 |= TX_DMA_INS_VLAN_MT7621 | vlan_tx_tag_get(skb);
		else
			txd.txd4 |= TX_DMA_INS_VLAN |
				((vlan_tx_tag_get(skb) >> VLAN_PRIO_SHIFT) << 4) |
				(vlan_tx_tag_get(skb) & 0xF);
	}

	/* TSO: fill MSS info in tcp checksum field */
	if (skb_is_gso(skb)) {
		if (skb_cow_head(skb, 0)) {
			netif_warn(priv, tx_err, dev,
					"GSO expand head fail.\n");
			goto err_out;
		}
		if (skb_shinfo(skb)->gso_type &
				(SKB_GSO_TCPV4 | SKB_GSO_TCPV6)) {
			txd.txd4 |= TX_DMA_TSO;
			tcp_hdr(skb)->check = htons(skb_shinfo(skb)->gso_size);
		}
	}

	mapped_addr = dma_map_single(&dev->dev, skb->data,
			skb_headlen(skb), DMA_TO_DEVICE);
	if (unlikely(dma_mapping_error(&dev->dev, mapped_addr)))
		goto err_out;
	txd.txd1 = mapped_addr;
	txd.txd2 = TX_DMA_PLEN0(skb_headlen(skb));

	/* TX SG offload */
	j = idx;
	for (i = 0; i < nr_frags; i++) {

		frag = &skb_shinfo(skb)->frags[i];
		mapped_addr = skb_frag_dma_map(&dev->dev, frag, 0,
				skb_frag_size(frag), DMA_TO_DEVICE);
		if (unlikely(dma_mapping_error(&dev->dev, mapped_addr)))
			goto err_dma;

		if (i & 0x1) {
			j = NEXT_TX_DESP_IDX(j);
			txd.txd1 = mapped_addr;
			txd.txd2 = TX_DMA_PLEN0(frag->size);
			txd.txd4 = def_txd4;
		} else {
			txd.txd3 = mapped_addr;
			txd.txd2 |= TX_DMA_PLEN1(frag->size);
			if (i != (nr_frags -1)) {
				fe_set_txd(&txd, &priv->tx_dma[j]);
				memset(&txd, 0, sizeof(txd));
			}
			priv->tx_skb[j] = (struct sk_buff *) DMA_DUMMY_DESC;
		}
	}

	/* set last segment */
	if (nr_frags & 0x1)
		txd.txd2 |= TX_DMA_LS1;
	else
		txd.txd2 |= TX_DMA_LS0;
	fe_set_txd(&txd, &priv->tx_dma[j]);

	/* store skb to cleanup */
	priv->tx_skb[j] = skb;

	netdev_sent_queue(dev, skb->len);
	skb_tx_timestamp(skb);

	wmb();
	j = NEXT_TX_DESP_IDX(j);
	fe_reg_w32(j, FE_REG_TX_CTX_IDX0);

	return 0;

err_dma:
	/* unmap dma */
	ptxd = &priv->tx_dma[idx];
	txd_unmap_single(&dev->dev, ptxd);

	j = idx;
	unmap_idx = i;
	for (i = 0; i < unmap_idx; i++) {
		if (i & 0x1) {
			j = NEXT_TX_DESP_IDX(j);
			ptxd = &priv->tx_dma[j];
			txd_unmap_page0(&dev->dev, ptxd);
		} else {
			txd_unmap_page1(&dev->dev, ptxd);
		}
	}

err_out:
	/* reinit descriptors and skb */
	j = idx;
	for (i = 0; i < tx_num; i++) {
		priv->tx_dma[j].txd2 = TX_DMA_DESP2_DEF;
		priv->tx_skb[j] = NULL;
		j = NEXT_TX_DESP_IDX(j);
	}
	wmb();

	return -1;
}

static inline int fe_skb_padto(struct sk_buff *skb, struct fe_priv *priv) {
	unsigned int len;
	int ret;

	ret = 0;
	if (unlikely(skb->len < VLAN_ETH_ZLEN)) {
		if ((priv->flags & FE_FLAG_PADDING_64B) &&
				!(priv->flags & FE_FLAG_PADDING_BUG))
			return ret;

		if (vlan_tx_tag_present(skb))
			len = ETH_ZLEN;
		else if (skb->protocol == cpu_to_be16(ETH_P_8021Q))
			len = VLAN_ETH_ZLEN;
		else if(!(priv->flags & FE_FLAG_PADDING_64B))
			len = ETH_ZLEN;
		else
			return ret;

		if (skb->len < len) {
			if ((ret = skb_pad(skb, len - skb->len)) < 0)
				return ret;
			skb->len = len;
			skb_set_tail_pointer(skb, len);
		}
	}

	return ret;
}

static inline u32 fe_empty_txd(struct fe_priv *priv, u32 tx_fill_idx)
{
	return (u32)(NUM_DMA_DESC - ((tx_fill_idx - priv->tx_free_idx) &
				(NUM_DMA_DESC - 1)));
}

static int fe_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
	struct fe_priv *priv = netdev_priv(dev);
	struct net_device_stats *stats = &dev->stats;
	u32 tx;
	int tx_num;
	int len = skb->len;

	if (fe_skb_padto(skb, priv)) {
		netif_warn(priv, tx_err, dev, "tx padding failed!\n");
		return NETDEV_TX_OK;
	}

	spin_lock(&priv->page_lock);
	tx_num = 1 + (skb_shinfo(skb)->nr_frags >> 1);
	tx = fe_reg_r32(FE_REG_TX_CTX_IDX0);
	if (unlikely(fe_empty_txd(priv, tx) <= tx_num))
	{
		netif_stop_queue(dev);
		spin_unlock(&priv->page_lock);
		netif_err(priv, tx_queued,dev,
				"Tx Ring full when queue awake!\n");
		return NETDEV_TX_BUSY;
	}

	if (fe_tx_map_dma(skb, dev, tx) < 0) {
		kfree_skb(skb);

		stats->tx_dropped++;
	} else {
		stats->tx_packets++;
		stats->tx_bytes += len;
	}

	spin_unlock(&priv->page_lock);

	return NETDEV_TX_OK;
}

static inline void fe_rx_vlan(struct sk_buff *skb)
{
	struct ethhdr *ehdr;
	u16 vlanid;

	if (!__vlan_get_tag(skb, &vlanid)) {
		/* pop the vlan tag */
		ehdr = (struct ethhdr *)skb->data;
		memmove(skb->data + VLAN_HLEN, ehdr, ETH_ALEN * 2);
		skb_pull(skb, VLAN_HLEN);
		__vlan_hwaccel_put_tag(skb, htons(ETH_P_8021Q), vlanid);
	}
}

static int fe_poll_rx(struct napi_struct *napi, int budget,
		struct fe_priv *priv)
{
	struct net_device *netdev = priv->netdev;
	struct net_device_stats *stats = &netdev->stats;
	struct fe_soc_data *soc = priv->soc;
	u32 checksum_bit;
	int idx = fe_reg_r32(FE_REG_RX_CALC_IDX0);
	struct sk_buff *skb;
	u8 *data, *new_data;
	struct fe_rx_dma *rxd, trxd;
	int done = 0;
	bool rx_vlan = netdev->features & NETIF_F_HW_VLAN_CTAG_RX;

	if (netdev->features & NETIF_F_RXCSUM)
		checksum_bit = soc->checksum_bit;
	else
		checksum_bit = 0;

	while (done < budget) {
		unsigned int pktlen;
		dma_addr_t dma_addr;
		idx = NEXT_RX_DESP_IDX(idx);
		rxd = &priv->rx_dma[idx];
		data = priv->rx_data[idx];

		fe_get_rxd(&trxd, rxd);
		if (!(trxd.rxd2 & RX_DMA_DONE))
			break;

		/* alloc new buffer */
		new_data = netdev_alloc_frag(priv->frag_size);
		if (unlikely(!new_data)) {
			stats->rx_dropped++;
			goto release_desc;
		}
		dma_addr = dma_map_single(&netdev->dev,
				new_data + FE_RX_OFFSET,
				priv->rx_buf_size,
				DMA_FROM_DEVICE);
		if (unlikely(dma_mapping_error(&netdev->dev, dma_addr))) {
			put_page(virt_to_head_page(new_data));
			goto release_desc;
		}

		/* receive data */
		skb = build_skb(data, priv->frag_size);
		if (unlikely(!skb)) {
			put_page(virt_to_head_page(new_data));
			goto release_desc;
		}
		skb_reserve(skb, FE_RX_OFFSET);

		dma_unmap_single(&netdev->dev, trxd.rxd1,
				priv->rx_buf_size, DMA_FROM_DEVICE);
		pktlen = RX_DMA_PLEN0(trxd.rxd2);
		skb_put(skb, pktlen);
		skb->dev = netdev;
		if (trxd.rxd4 & checksum_bit) {
			skb->ip_summed = CHECKSUM_UNNECESSARY;
		} else {
			skb_checksum_none_assert(skb);
		}
		if (rx_vlan)
			fe_rx_vlan(skb);
		skb->protocol = eth_type_trans(skb, netdev);

		stats->rx_packets++;
		stats->rx_bytes += pktlen;

		if (skb->ip_summed == CHECKSUM_NONE)
			netif_receive_skb(skb);
		else
			napi_gro_receive(napi, skb);

		priv->rx_data[idx] = new_data;
		rxd->rxd1 = (unsigned int) dma_addr;

release_desc:
		if (soc->rx_dma)
			soc->rx_dma(rxd, priv->rx_buf_size);
		else
			rxd->rxd2 = RX_DMA_LSO;

		wmb();
		fe_reg_w32(idx, FE_REG_RX_CALC_IDX0);
		done++;
	}

	return done;
}

static int fe_poll_tx(struct fe_priv *priv, int budget)
{
	struct net_device *netdev = priv->netdev;
	struct device *dev = &netdev->dev;
	unsigned int bytes_compl = 0;
	struct sk_buff *skb;
	struct fe_tx_dma txd;
	int done = 0, idx;
	u32 udf_bit = priv->soc->tx_udf_bit;

	idx = priv->tx_free_idx;
	while (done < budget) {
		fe_get_txd(&txd, &priv->tx_dma[idx]);
		skb = priv->tx_skb[idx];

		if (!(txd.txd2 & TX_DMA_DONE) || !skb)
			break;

		txd_unmap_page1(dev, &txd);

		if (txd.txd4 & udf_bit)
			txd_unmap_single(dev, &txd);
		else
			txd_unmap_page0(dev, &txd);

		if (skb != (struct sk_buff *) DMA_DUMMY_DESC) {
			bytes_compl += skb->len;
			dev_kfree_skb_any(skb);
			done++;
		}
		priv->tx_skb[idx] = NULL;
		idx = NEXT_TX_DESP_IDX(idx);
	}
	priv->tx_free_idx = idx;

	if (!done)
		return 0;

	netdev_completed_queue(netdev, done, bytes_compl);
	if (unlikely(netif_queue_stopped(netdev) &&
				netif_carrier_ok(netdev))) {
		netif_wake_queue(netdev);
	}

	return done;
}

static int fe_poll(struct napi_struct *napi, int budget)
{
	struct fe_priv *priv = container_of(napi, struct fe_priv, rx_napi);
	struct fe_hw_stats *hwstat = priv->hw_stats;
	int tx_done, rx_done;
	u32 status, mask;
	u32 tx_intr, rx_intr;

	status = fe_reg_r32(FE_REG_FE_INT_STATUS);
	tx_intr = priv->soc->tx_int;
	rx_intr = priv->soc->rx_int;
	tx_done = rx_done = 0;

poll_again:
	if (status & tx_intr) {
		tx_done += fe_poll_tx(priv, budget - tx_done);
		if (tx_done < budget) {
			fe_reg_w32(tx_intr, FE_REG_FE_INT_STATUS);
		}
		status = fe_reg_r32(FE_REG_FE_INT_STATUS);
	}

	if (status & rx_intr) {
		rx_done += fe_poll_rx(napi, budget - rx_done, priv);
		if (rx_done < budget) {
			fe_reg_w32(rx_intr, FE_REG_FE_INT_STATUS);
		}
	}

	if (unlikely(hwstat && (status & FE_CNT_GDM_AF))) {
		if (spin_trylock(&hwstat->stats_lock)) {
			fe_stats_update(priv);
			spin_unlock(&hwstat->stats_lock);
		}
		fe_reg_w32(FE_CNT_GDM_AF, FE_REG_FE_INT_STATUS);
	}

	if (unlikely(netif_msg_intr(priv))) {
		mask = fe_reg_r32(FE_REG_FE_INT_ENABLE);
		netdev_info(priv->netdev,
				"done tx %d, rx %d, intr 0x%x/0x%x\n",
				tx_done, rx_done, status, mask);
	}

	if ((tx_done < budget) && (rx_done < budget)) {
		status = fe_reg_r32(FE_REG_FE_INT_STATUS);
		if (status & (tx_intr | rx_intr )) {
			goto poll_again;
		}
		napi_complete(napi);
		fe_int_enable(tx_intr | rx_intr);
	}

	return rx_done;
}

static void fe_tx_timeout(struct net_device *dev)
{
	struct fe_priv *priv = netdev_priv(dev);

	priv->netdev->stats.tx_errors++;
	netif_err(priv, tx_err, dev,
			"transmit timed out, waking up the queue\n");
	netif_info(priv, drv, dev, ": dma_cfg:%08x, free_idx:%d, " \
			"dma_ctx_idx=%u, dma_crx_idx=%u\n",
			fe_reg_r32(FE_REG_PDMA_GLO_CFG), priv->tx_free_idx,
			fe_reg_r32(FE_REG_TX_CTX_IDX0),
			fe_reg_r32(FE_REG_RX_CALC_IDX0));
	netif_wake_queue(dev);
}

static irqreturn_t fe_handle_irq(int irq, void *dev)
{
	struct fe_priv *priv = netdev_priv(dev);
	u32 status, int_mask;

	status = fe_reg_r32(FE_REG_FE_INT_STATUS);

	if (unlikely(!status))
		return IRQ_NONE;

	int_mask = (priv->soc->rx_int | priv->soc->tx_int);
	if (likely(status & int_mask)) {
		fe_int_disable(int_mask);
		napi_schedule(&priv->rx_napi);
	} else {
		fe_reg_w32(status, FE_REG_FE_INT_STATUS);
	}

	return IRQ_HANDLED;
}

#ifdef CONFIG_NET_POLL_CONTROLLER
static void fe_poll_controller(struct net_device *dev)
{
	struct fe_priv *priv = netdev_priv(dev);
	u32 int_mask = priv->soc->tx_int | priv->soc->rx_int;

	fe_int_disable(int_mask);
	fe_handle_irq(dev->irq, dev);
	fe_int_enable(int_mask);
}
#endif

int fe_set_clock_cycle(struct fe_priv *priv)
{
	unsigned long sysclk = priv->sysclk;

	if (!sysclk) {
		return -EINVAL;
	}

	sysclk /= FE_US_CYC_CNT_DIVISOR;
	sysclk <<= FE_US_CYC_CNT_SHIFT;

	fe_w32((fe_r32(FE_FE_GLO_CFG) &
				~(FE_US_CYC_CNT_MASK << FE_US_CYC_CNT_SHIFT)) |
			sysclk,
			FE_FE_GLO_CFG);
	return 0;
}

void fe_fwd_config(struct fe_priv *priv)
{
	u32 fwd_cfg;

	fwd_cfg = fe_r32(FE_GDMA1_FWD_CFG);

	/* disable jumbo frame */
	if (priv->flags & FE_FLAG_JUMBO_FRAME)
		fwd_cfg &= ~FE_GDM1_JMB_EN;

	/* set unicast/multicast/broadcast frame to cpu */
	fwd_cfg &= ~0xffff;

	fe_w32(fwd_cfg, FE_GDMA1_FWD_CFG);
}

static void fe_rxcsum_config(bool enable)
{
	if (enable)
		fe_w32(fe_r32(FE_GDMA1_FWD_CFG) | (FE_GDM1_ICS_EN |
					FE_GDM1_TCS_EN | FE_GDM1_UCS_EN),
				FE_GDMA1_FWD_CFG);
	else
		fe_w32(fe_r32(FE_GDMA1_FWD_CFG) & ~(FE_GDM1_ICS_EN |
					FE_GDM1_TCS_EN | FE_GDM1_UCS_EN),
				FE_GDMA1_FWD_CFG);
}

static void fe_txcsum_config(bool enable)
{
	if (enable)
		fe_w32(fe_r32(FE_CDMA_CSG_CFG) | (FE_ICS_GEN_EN |
					FE_TCS_GEN_EN | FE_UCS_GEN_EN),
				FE_CDMA_CSG_CFG);
	else
		fe_w32(fe_r32(FE_CDMA_CSG_CFG) & ~(FE_ICS_GEN_EN |
					FE_TCS_GEN_EN | FE_UCS_GEN_EN),
				FE_CDMA_CSG_CFG);
}

void fe_csum_config(struct fe_priv *priv)
{
	struct net_device *dev = priv_netdev(priv);

	fe_txcsum_config((dev->features & NETIF_F_IP_CSUM));
	fe_rxcsum_config((dev->features & NETIF_F_RXCSUM));
}

static int fe_hw_init(struct net_device *dev)
{
	struct fe_priv *priv = netdev_priv(dev);
	int i, err;

	err = devm_request_irq(priv->device, dev->irq, fe_handle_irq, 0,
				dev_name(priv->device), dev);
	if (err)
		return err;

	if (priv->soc->set_mac)
		priv->soc->set_mac(priv, dev->dev_addr);
	else
		fe_hw_set_macaddr(priv, dev->dev_addr);

	fe_int_disable(priv->soc->tx_int | priv->soc->rx_int);

        /* frame engine will push VLAN tag regarding to VIDX feild in Tx desc. */
	if (fe_reg_table[FE_REG_FE_DMA_VID_BASE])
		for (i = 0; i < 16; i += 2)
			fe_w32(((i + 1) << 16) + i,
					fe_reg_table[FE_REG_FE_DMA_VID_BASE] +
					(i * 2));

	BUG_ON(!priv->soc->fwd_config);
	if (priv->soc->fwd_config(priv))
		netdev_err(dev, "unable to get clock\n");

	if (fe_reg_table[FE_REG_FE_RST_GL]) {
		fe_reg_w32(1, FE_REG_FE_RST_GL);
		fe_reg_w32(0, FE_REG_FE_RST_GL);
	}

	return 0;
}

static int fe_open(struct net_device *dev)
{
	struct fe_priv *priv = netdev_priv(dev);
	unsigned long flags;
	u32 val;
	int err;

	err = fe_init_dma(priv);
	if (err)
		goto err_out;

	spin_lock_irqsave(&priv->page_lock, flags);
	napi_enable(&priv->rx_napi);

	val = FE_TX_WB_DDONE | FE_RX_DMA_EN | FE_TX_DMA_EN;
	val |= priv->soc->pdma_glo_cfg;
	fe_reg_w32(val, FE_REG_PDMA_GLO_CFG);

	spin_unlock_irqrestore(&priv->page_lock, flags);

	if (priv->phy)
		priv->phy->start(priv);

	if (priv->soc->has_carrier && priv->soc->has_carrier(priv))
		netif_carrier_on(dev);

	netif_start_queue(dev);
	fe_int_enable(priv->soc->tx_int | priv->soc->rx_int);

	return 0;

err_out:
	fe_free_dma(priv);
	return err;
}

static int fe_stop(struct net_device *dev)
{
	struct fe_priv *priv = netdev_priv(dev);
	unsigned long flags;
	int i;

	fe_int_disable(priv->soc->tx_int | priv->soc->rx_int);

	netif_tx_disable(dev);

	if (priv->phy)
		priv->phy->stop(priv);

	spin_lock_irqsave(&priv->page_lock, flags);
	napi_disable(&priv->rx_napi);

	fe_reg_w32(fe_reg_r32(FE_REG_PDMA_GLO_CFG) &
		     ~(FE_TX_WB_DDONE | FE_RX_DMA_EN | FE_TX_DMA_EN),
		     FE_REG_PDMA_GLO_CFG);
	spin_unlock_irqrestore(&priv->page_lock, flags);

	/* wait dma stop */
	for (i = 0; i < 10; i++) {
		if (fe_reg_r32(FE_REG_PDMA_GLO_CFG) &
				(FE_TX_DMA_BUSY | FE_RX_DMA_BUSY)) {
			msleep(10);
			continue;
		}
		break;
	}

	fe_free_dma(priv);

	return 0;
}

static int __init fe_init(struct net_device *dev)
{
	struct fe_priv *priv = netdev_priv(dev);
	struct device_node *port;
	int err;

	BUG_ON(!priv->soc->reset_fe);
	priv->soc->reset_fe();

	if (priv->soc->switch_init)
		priv->soc->switch_init(priv);

	memcpy(dev->dev_addr, priv->soc->mac, ETH_ALEN);
	of_get_mac_address_mtd(priv->device->of_node, dev->dev_addr);

	err = fe_mdio_init(priv);
	if (err)
		return err;

	if (priv->soc->port_init)
		for_each_child_of_node(priv->device->of_node, port)
			if (of_device_is_compatible(port, "ralink,eth-port") && of_device_is_available(port))
				priv->soc->port_init(priv, port);

	if (priv->phy) {
		err = priv->phy->connect(priv);
		if (err)
			goto err_phy_disconnect;
	}

	err = fe_hw_init(dev);
	if (err)
		goto err_phy_disconnect;

	if (priv->soc->switch_config)
		priv->soc->switch_config(priv);

	return 0;

err_phy_disconnect:
	if (priv->phy)
		priv->phy->disconnect(priv);
	fe_mdio_cleanup(priv);

	return err;
}

static void fe_uninit(struct net_device *dev)
{
	struct fe_priv *priv = netdev_priv(dev);

	if (priv->phy)
		priv->phy->disconnect(priv);
	fe_mdio_cleanup(priv);

	fe_reg_w32(0, FE_REG_FE_INT_ENABLE);
	free_irq(dev->irq, dev);
}

static int fe_do_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd)
{
	struct fe_priv *priv = netdev_priv(dev);

	if (!priv->phy_dev)
		return -ENODEV;

	switch (cmd) {
	case SIOCETHTOOL:
		return phy_ethtool_ioctl(priv->phy_dev,
				(void *) ifr->ifr_data);
	case SIOCGMIIPHY:
	case SIOCGMIIREG:
	case SIOCSMIIREG:
		return phy_mii_ioctl(priv->phy_dev, ifr, cmd);
	default:
		break;
	}

	return -EOPNOTSUPP;
}

static int fe_change_mtu(struct net_device *dev, int new_mtu)
{
	struct fe_priv *priv = netdev_priv(dev);
	int frag_size, old_mtu;
	u32 fwd_cfg;

	if (!(priv->flags & FE_FLAG_JUMBO_FRAME))
		return eth_change_mtu(dev, new_mtu);

	frag_size = fe_max_frag_size(new_mtu);
	if (new_mtu < 68 || frag_size > PAGE_SIZE)
		return -EINVAL;

	old_mtu = dev->mtu;
	dev->mtu = new_mtu;

	/* return early if the buffer sizes will not change */
	if (old_mtu <= ETH_DATA_LEN && new_mtu <= ETH_DATA_LEN)
		return 0;
	if (old_mtu > ETH_DATA_LEN && new_mtu > ETH_DATA_LEN)
		return 0;

	if (new_mtu <= ETH_DATA_LEN) {
		priv->frag_size = fe_max_frag_size(ETH_DATA_LEN);
		priv->rx_buf_size = fe_max_buf_size(ETH_DATA_LEN);
	} else {
		priv->frag_size = PAGE_SIZE;
		priv->rx_buf_size = fe_max_buf_size(PAGE_SIZE);
	}

	if (!netif_running(dev))
		return 0;

	fe_stop(dev);
	fwd_cfg = fe_r32(FE_GDMA1_FWD_CFG);
	if (new_mtu <= ETH_DATA_LEN)
		fwd_cfg &= ~FE_GDM1_JMB_EN;
	else {
		fwd_cfg &= ~(FE_GDM1_JMB_LEN_MASK << FE_GDM1_JMB_LEN_SHIFT);
		fwd_cfg |= (DIV_ROUND_UP(frag_size, 1024) <<
				FE_GDM1_JMB_LEN_SHIFT) | FE_GDM1_JMB_EN;
	}
	fe_w32(fwd_cfg, FE_GDMA1_FWD_CFG);

	return fe_open(dev);
}

static const struct net_device_ops fe_netdev_ops = {
	.ndo_init		= fe_init,
	.ndo_uninit		= fe_uninit,
	.ndo_open		= fe_open,
	.ndo_stop		= fe_stop,
	.ndo_start_xmit		= fe_start_xmit,
	.ndo_set_mac_address	= fe_set_mac_address,
	.ndo_validate_addr	= eth_validate_addr,
	.ndo_do_ioctl		= fe_do_ioctl,
	.ndo_change_mtu		= fe_change_mtu,
	.ndo_tx_timeout		= fe_tx_timeout,
	.ndo_get_stats64        = fe_get_stats64,
	.ndo_vlan_rx_add_vid	= fe_vlan_rx_add_vid,
	.ndo_vlan_rx_kill_vid	= fe_vlan_rx_kill_vid,
#ifdef CONFIG_NET_POLL_CONTROLLER
	.ndo_poll_controller	= fe_poll_controller,
#endif
};

static int fe_probe(struct platform_device *pdev)
{
	struct resource *res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	const struct of_device_id *match;
	struct fe_soc_data *soc;
	struct net_device *netdev;
	struct fe_priv *priv;
	struct clk *sysclk;
	int err;

	device_reset(&pdev->dev);

	match = of_match_device(of_fe_match, &pdev->dev);
	soc = (struct fe_soc_data *) match->data;

	if (soc->reg_table)
		fe_reg_table = soc->reg_table;
	else
		soc->reg_table = fe_reg_table;

	fe_base = devm_request_and_ioremap(&pdev->dev, res);
	if (!fe_base) {
		err = -EADDRNOTAVAIL;
		goto err_out;
	}

	netdev = alloc_etherdev(sizeof(*priv));
	if (!netdev) {
		dev_err(&pdev->dev, "alloc_etherdev failed\n");
		err = -ENOMEM;
		goto err_iounmap;
	}

	SET_NETDEV_DEV(netdev, &pdev->dev);
	netdev->netdev_ops = &fe_netdev_ops;
	netdev->base_addr = (unsigned long) fe_base;
	netdev->watchdog_timeo = TX_TIMEOUT;

	netdev->irq = platform_get_irq(pdev, 0);
	if (netdev->irq < 0) {
		dev_err(&pdev->dev, "no IRQ resource found\n");
		err = -ENXIO;
		goto err_free_dev;
	}

	if (soc->init_data)
		soc->init_data(soc, netdev);
	/* fake NETIF_F_HW_VLAN_CTAG_RX for good GRO performance */
	netdev->hw_features |= NETIF_F_HW_VLAN_CTAG_RX;
	netdev->vlan_features = netdev->hw_features &
		~(NETIF_F_HW_VLAN_CTAG_TX | NETIF_F_HW_VLAN_CTAG_RX);
	netdev->features |= netdev->hw_features;

	/* fake rx vlan filter func. to support tx vlan offload func */
	if (fe_reg_table[FE_REG_FE_DMA_VID_BASE])
		netdev->features |= NETIF_F_HW_VLAN_CTAG_FILTER;

	priv = netdev_priv(netdev);
	spin_lock_init(&priv->page_lock);
	if (fe_reg_table[FE_REG_FE_COUNTER_BASE]) {
		priv->hw_stats = kzalloc(sizeof(*priv->hw_stats), GFP_KERNEL);
		if (!priv->hw_stats) {
			err = -ENOMEM;
			goto err_free_dev;
		}
		spin_lock_init(&priv->hw_stats->stats_lock);
	}

	sysclk = devm_clk_get(&pdev->dev, NULL);
	if (!IS_ERR(sysclk))
		priv->sysclk = clk_get_rate(sysclk);

	priv->netdev = netdev;
	priv->device = &pdev->dev;
	priv->soc = soc;
	priv->msg_enable = netif_msg_init(fe_msg_level, FE_DEFAULT_MSG_ENABLE);
	priv->frag_size = fe_max_frag_size(ETH_DATA_LEN);
	priv->rx_buf_size = fe_max_buf_size(ETH_DATA_LEN);
	if (priv->frag_size > PAGE_SIZE) {
		dev_err(&pdev->dev, "error frag size.\n");
		err = -EINVAL;
		goto err_free_dev;
	}

	netif_napi_add(netdev, &priv->rx_napi, fe_poll, 32);
	fe_set_ethtool_ops(netdev);

	err = register_netdev(netdev);
	if (err) {
		dev_err(&pdev->dev, "error bringing up device\n");
		goto err_free_dev;
	}

	platform_set_drvdata(pdev, netdev);

	netif_info(priv, probe, netdev, "ralink at 0x%08lx, irq %d\n",
			netdev->base_addr, netdev->irq);

	return 0;

err_free_dev:
	free_netdev(netdev);
err_iounmap:
	devm_iounmap(&pdev->dev, fe_base);
err_out:
	return err;
}

static int fe_remove(struct platform_device *pdev)
{
        struct net_device *dev = platform_get_drvdata(pdev);
	struct fe_priv *priv = netdev_priv(dev);

	netif_napi_del(&priv->rx_napi);
	if (priv->hw_stats)
		kfree(priv->hw_stats);

	unregister_netdev(dev);
	free_netdev(dev);
	platform_set_drvdata(pdev, NULL);

	return 0;
}

static struct platform_driver fe_driver = {
	.probe = fe_probe,
	.remove = fe_remove,
	.driver = {
		.name = "ralink_soc_eth",
		.owner = THIS_MODULE,
		.of_match_table = of_fe_match,
	},
};

static int __init init_rtfe(void)
{
	int ret;

	ret = rtesw_init();
	if (ret)
		return ret;

	ret = platform_driver_register(&fe_driver);
	if (ret)
		rtesw_exit();

	return ret;
}

static void __exit exit_rtfe(void)
{
	platform_driver_unregister(&fe_driver);
	rtesw_exit();
}

module_init(init_rtfe);
module_exit(exit_rtfe);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("John Crispin <blogic@openwrt.org>");
MODULE_DESCRIPTION("Ethernet driver for Ralink SoC");
MODULE_VERSION(FE_DRV_VERSION);

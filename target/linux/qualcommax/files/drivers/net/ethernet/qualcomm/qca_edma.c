// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2016-2021, The Linux Foundation. All rights reserved.
 * Copyright (c) 2023-2024, Qualcomm Innovation Center, Inc. All rights reserved.
 */

#include <linux/clk.h>
#include <linux/ethtool.h>
#include <linux/if_vlan.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_platform.h>
#include <linux/property.h>
#include <linux/regmap.h>
#include <linux/reset.h>
#include "qca_edma.h"

static void edma_irq_disable_all(struct edma_priv *priv)
{
	const struct edma_soc_data *soc = priv->soc;
	int i;

	for (i = 0; i <= soc->txdesc_ring; i++)
		regmap_write(priv->regmap,
			     EDMA_REG_TX_INT_MASK(soc->tx_int_base, i),
			     0);

	for (i = 0; i <= soc->rxfill_ring; i++)
		regmap_write(priv->regmap, EDMA_REG_RXFILL_INT_MASK(i), 0);

	for (i = 0; i <= soc->rxdesc_ring; i++) {
		regmap_write(priv->regmap, EDMA_REG_RXDESC_INT_MASK(i), 0);
		regmap_write(priv->regmap, EDMA_REG_RX_INT_CTRL(i), 0);
	}
}

static void edma_tx_irq_mask(struct edma_priv *priv)
{
	const struct edma_soc_data *soc = priv->soc;

	regmap_write(priv->regmap,
		     EDMA_REG_TX_INT_MASK(soc->tx_int_base, soc->txcmpl_ring),
		     0);
}

static void edma_tx_irq_unmask(struct edma_priv *priv)
{
	const struct edma_soc_data *soc = priv->soc;

	regmap_write(priv->regmap,
		     EDMA_REG_TX_INT_MASK(soc->tx_int_base, soc->txcmpl_ring),
		     EDMA_TX_INT_MASK);
}

static void edma_rx_irq_mask(struct edma_priv *priv)
{
	const struct edma_soc_data *soc = priv->soc;

	regmap_write(priv->regmap,
		     EDMA_REG_RXFILL_INT_MASK(soc->rxfill_ring), 0);
	regmap_write(priv->regmap,
		     EDMA_REG_RXDESC_INT_MASK(soc->rxdesc_ring), 0);
}

static void edma_rx_irq_unmask(struct edma_priv *priv)
{
	const struct edma_soc_data *soc = priv->soc;

	regmap_write(priv->regmap,
		     EDMA_REG_RXFILL_INT_MASK(soc->rxfill_ring),
		     EDMA_RXFILL_INT_MASK);
	regmap_write(priv->regmap,
		     EDMA_REG_RXDESC_INT_MASK(soc->rxdesc_ring),
		     EDMA_RXDESC_INT_MASK_PKT_INT);
}

static irqreturn_t edma_tx_irq_handle(int irq, void *ctx)
{
	const struct edma_soc_data *soc;
	struct edma_priv *priv = ctx;
	u32 val;

	soc = priv->soc;

	regmap_read(priv->regmap,
		    EDMA_REG_TX_INT_STAT(soc->tx_int_base,
					 soc->txcmpl_ring), &val);
	if (!val)
		return IRQ_NONE;

	edma_tx_irq_mask(priv);

	if (likely(napi_schedule_prep(&priv->tx_napi)))
		__napi_schedule(&priv->tx_napi);

	return IRQ_HANDLED;
}

static irqreturn_t edma_rx_irq_handle(int irq, void *ctx)
{
	const struct edma_soc_data *soc;
	struct edma_priv *priv = ctx;
	u32 val, status = 0;

	soc = priv->soc;

	regmap_read(priv->regmap,
		    EDMA_REG_RXDESC_INT_STAT(soc->rxdesc_ring),
		    &val);
	status |= val;
	regmap_read(priv->regmap,
		    EDMA_REG_RXFILL_INT_STAT(soc->rxfill_ring),
		    &val);
	status |= val;

	if (!status)
		return IRQ_NONE;

	edma_rx_irq_mask(priv);

	if (likely(napi_schedule_prep(&priv->rx_napi)))
		__napi_schedule(&priv->rx_napi);

	return IRQ_HANDLED;
}

static irqreturn_t edma_misc_irq_handle(int irq, void *ctx)
{
	struct edma_priv *priv = ctx;
	u32 val;

	regmap_read(priv->regmap, EDMA_REG_MISC_INT_STAT, &val);
	if (!val)
		return IRQ_NONE;

	return IRQ_HANDLED;
}

static int edma_ring_alloc(struct edma_priv *priv, struct edma_ring *ring, int count,
			   int desc_size)
{
	struct device *dev = &priv->pdev->dev;

	ring->count = count;
	ring->desc = dma_alloc_coherent(dev, count * desc_size, &ring->dma,
					GFP_KERNEL);
	if (!ring->desc)
		return -ENOMEM;

	return 0;
}

static int edma_tx_ring_alloc(struct edma_priv *priv, struct edma_ring *ring,
			      int count, int desc_size)
{
	int ret;

	ret = edma_ring_alloc(priv, ring, count, desc_size);
	if (ret)
		return ret;

	ring->skb_store = kcalloc(count, sizeof(struct sk_buff *), GFP_KERNEL);
	if (!ring->skb_store) {
		dma_free_coherent(&priv->pdev->dev, count * desc_size,
				  ring->desc, ring->dma);
		ring->desc = NULL;
		return -ENOMEM;
	}

	return 0;
}

static void edma_ring_free(struct edma_priv *priv, struct edma_ring *ring,
			   int desc_size)
{
	if (ring->desc) {
		dma_free_coherent(&priv->pdev->dev, ring->count * desc_size,
				  ring->desc, ring->dma);
		ring->desc = NULL;
	}
}

static void edma_tx_ring_free(struct edma_priv *priv, struct edma_ring *ring,
			      int desc_size)
{
	int i;

	if (ring->skb_store) {
		for (i = 0; i < ring->count; i++)
			dev_kfree_skb_any(ring->skb_store[i]);
		kfree(ring->skb_store);
		ring->skb_store = NULL;
	}

	edma_ring_free(priv, ring, desc_size);
}

static int edma_rx_fill(struct edma_priv *priv, struct edma_ring *rxfill_ring)
{
	const struct edma_soc_data *soc = priv->soc;
	struct edma_rxfill_desc *rxfill_desc;
	u16 prod, cons, next;
	struct page *page;
	u16 filled = 0;
	dma_addr_t dma;
	u32 val;

	regmap_read(priv->regmap, EDMA_REG_RXFILL_PROD_IDX(soc->rxfill_ring),
		    &val);
	prod = val & EDMA_RXFILL_PROD_IDX_MASK & (rxfill_ring->count - 1);

	regmap_read(priv->regmap, EDMA_REG_RXFILL_CONS_IDX(soc->rxfill_ring),
		    &val);
	cons = val & EDMA_RXFILL_CONS_IDX_MASK & (rxfill_ring->count - 1);

	while (1) {
		next = prod + 1;
		if (next == rxfill_ring->count)
			next = 0;

		if (next == cons)
			break;

		page = page_pool_dev_alloc_pages(priv->page_pool);
		if (unlikely(!page))
			break;

		rxfill_desc = EDMA_RXFILL_DESC(rxfill_ring, prod);

		dma = page_pool_get_dma_addr(page) + NET_SKB_PAD;
		rxfill_desc->buffer_addr = cpu_to_le32(dma);
		rxfill_desc->word1 = cpu_to_le32(EDMA_RX_BUFFER_SIZE &
						 EDMA_RXFILL_BUF_SIZE_MASK);

		filled++;
		prod = next;
	}

	if (filled) {
		wmb();
		regmap_write(priv->regmap,
			     EDMA_REG_RXFILL_PROD_IDX(soc->rxfill_ring),
			     prod & EDMA_RXFILL_PROD_IDX_MASK);
	}

	return filled;
}

static u32 edma_clean_tx(struct edma_priv *priv, struct edma_ring *txcmpl_ring,
			 int budget)
{
	const struct edma_soc_data *soc = priv->soc;
	struct platform_device *pdev = priv->pdev;
	struct edma_txcmpl *txcmpl;
	struct edma_txdesc *txdesc;
	u32 cleaned = 0, bytes = 0;
	u16 prod, cons;
	struct sk_buff *skb;
	u32 val, len;
	int idx;

	regmap_read(priv->regmap,
		    EDMA_REG_TXCMPL_PROD_IDX(soc->txcmpl_base,
					     soc->txcmpl_ring),
		    &val);
	prod = val & EDMA_TXCMPL_PROD_IDX_MASK;

	regmap_read(priv->regmap,
		    EDMA_REG_TXCMPL_CONS_IDX(soc->txcmpl_base,
					     soc->txcmpl_ring),
		    &val);
	cons = val & EDMA_TXCMPL_CONS_IDX_MASK;

	while (cons != prod && cleaned < budget) {
		txcmpl = EDMA_TXCMPL_DESC(txcmpl_ring, cons);

		idx = txcmpl->buffer_addr;
		skb = priv->txdesc_ring.skb_store[idx];
		priv->txdesc_ring.skb_store[idx] = NULL;

		if (unlikely(!skb)) {
			dev_warn(&pdev->dev,
				 "invalid skb: cons:%u prod:%u status %x\n",
				 cons, prod, txcmpl->status);
			goto next;
		}

		txdesc = EDMA_TXDESC_DESC(&priv->txdesc_ring, idx);
		len = skb_headlen(skb);

		dma_unmap_single(&pdev->dev,
				 le32_to_cpu(txdesc->buffer_addr),
				 len, DMA_TO_DEVICE);
		bytes += len - EDMA_TX_PREHDR_SIZE;
		napi_consume_skb(skb, budget);

next:
		if (++cons == txcmpl_ring->count)
			cons = 0;

		cleaned++;
	}

	if (cleaned == 0)
		return 0;

	netdev_tx_completed_queue(netdev_get_tx_queue(priv->netdev, 0), cleaned,
				  bytes);

	/* Ensure all TX completions are processed before updating cons idx */
	wmb();
	regmap_write(priv->regmap,
		     EDMA_REG_TXCMPL_CONS_IDX(soc->txcmpl_base,
					      soc->txcmpl_ring),
		     cons);

	return cleaned;
}

static u32 edma_clean_rx(struct edma_priv *priv, int budget,
			 struct edma_ring *rxdesc_ring)
{
	const struct edma_soc_data *soc = priv->soc;
	struct platform_device *pdev = priv->pdev;
	struct dsa_oob_tag_info *tag_info;
	struct edma_rx_preheader *rxph;
	struct edma_rxdesc *rxdesc;
	struct sk_buff *skb;
	u16 prod, cons;
	struct page *page;
	u32 done = 0;
	u32 src_port;
	int pkt_len;
	u32 val;

	regmap_read(priv->regmap, EDMA_REG_RXDESC_PROD_IDX(soc->rxdesc_ring),
		    &val);
	prod = val & EDMA_RXDESC_PROD_IDX_MASK;

	regmap_read(priv->regmap, EDMA_REG_RXDESC_CONS_IDX(soc->rxdesc_ring),
		    &val);
	cons = val & EDMA_RXDESC_CONS_IDX_MASK;

	while (cons != prod && done < budget) {
		rxdesc = EDMA_RXDESC_DESC(rxdesc_ring, cons);

		rxph = phys_to_virt(rxdesc->buffer_addr);
		page = virt_to_head_page(rxph);

		pkt_len = rxdesc->status & EDMA_RXDESC_PACKET_LEN_MASK;

		page_pool_dma_sync_for_cpu(priv->page_pool, page, 0,
					   EDMA_RX_PREHDR_SIZE + pkt_len);

		if (EDMA_RXPH_SRC_INFO_TYPE_GET(rxph) !=
		    EDMA_PREHDR_DSTINFO_PORTID_IND) {
			dev_warn(
				&pdev->dev,
				"rx drop: src_info_type=0x%x src_info=0x%04x dst_info=0x%04x\n",
				EDMA_RXPH_SRC_INFO_TYPE_GET(rxph),
				rxph->src_info, rxph->dst_info);
			page_pool_put_full_page(priv->page_pool, page, true);
			goto next;
		}

		src_port = rxph->src_info & EDMA_SRC_PORT_MASK;

		skb = napi_build_skb(page_address(page), PAGE_SIZE);
		if (unlikely(!skb)) {
			page_pool_put_full_page(priv->page_pool, page, true);
			goto next;
		}

		skb_mark_for_recycle(skb);
		skb_reserve(skb, NET_SKB_PAD + EDMA_RX_PREHDR_SIZE);
		skb_put(skb, pkt_len);

		skb->protocol = eth_type_trans(skb, priv->netdev);

		tag_info = skb_ext_add(skb, SKB_EXT_DSA_OOB);
		if (unlikely(!tag_info)) {
			dev_kfree_skb_any(skb);
			goto next;
		}
		tag_info->port = src_port;

		dev_sw_netstats_rx_add(priv->netdev, pkt_len);
		napi_gro_receive(&priv->rx_napi, skb);

next:
		if (++cons == rxdesc_ring->count)
			cons = 0;

		done++;
	}

	edma_rx_fill(priv, &priv->rxfill_ring);

	wmb();
	regmap_write(priv->regmap,
		     EDMA_REG_RXDESC_CONS_IDX(soc->rxdesc_ring),
		     cons);
	return done;
}

static int edma_tx_napi(struct napi_struct *napi, int budget)
{
	struct edma_priv *priv = container_of(napi, struct edma_priv, tx_napi);
	int work = edma_clean_tx(priv, &priv->txcmpl_ring, budget);
	const struct edma_soc_data *soc = priv->soc;
	u32 val;

	if (priv->netdev && netif_queue_stopped(priv->netdev) &&
	    netif_carrier_ok(priv->netdev)) {
		u16 prod, cons, free;

		regmap_read(priv->regmap,
			    EDMA_REG_TXDESC_PROD_IDX(soc->txdesc_ring), &val);
		prod = val & EDMA_TXDESC_PROD_IDX_MASK;
		regmap_read(priv->regmap,
			    EDMA_REG_TXDESC_CONS_IDX(soc->txdesc_ring), &val);
		cons = val & EDMA_TXDESC_CONS_IDX_MASK;
		free = (cons - prod - 1) & (priv->txdesc_ring.count - 1);

		if (free > EDMA_TX_RING_THRESH)
			netif_wake_queue(priv->netdev);
	}

	if (work < budget) {
		regmap_read(priv->regmap,
			    EDMA_REG_TX_INT_STAT(soc->tx_int_base,
						 soc->txcmpl_ring),
			    &val);
		if (val)
			return budget;

		if (napi_complete_done(napi, work))
			edma_tx_irq_unmask(priv);
	}

	return work;
}

static int edma_rx_napi(struct napi_struct *napi, int budget)
{
	struct edma_priv *priv = container_of(napi, struct edma_priv, rx_napi);
	const struct edma_soc_data *soc = priv->soc;
	int done;
	u16 prod, cons;

	done = edma_clean_rx(priv, budget, &priv->rxdesc_ring);

	if (done < budget) {
		u32 val;

		regmap_read(priv->regmap,
			    EDMA_REG_RXDESC_INT_STAT(soc->rxdesc_ring),
			    &val);
		prod = val;
		regmap_read(priv->regmap,
			    EDMA_REG_RXFILL_INT_STAT(soc->rxfill_ring),
			    &val);
		cons = val;
		if (prod || cons)
			return budget;

		regmap_read(priv->regmap,
			    EDMA_REG_RXFILL_PROD_IDX(soc->rxfill_ring),
			    &val);
		prod = val & (priv->rxfill_ring.count - 1);
		regmap_read(priv->regmap,
			    EDMA_REG_RXFILL_CONS_IDX(soc->rxfill_ring),
			    &val);
		cons = val & (priv->rxfill_ring.count - 1);
		if (prod == cons) {
			dev_warn_ratelimited(&priv->pdev->dev,
					     "RXFILL ring starved\n");
			edma_rx_fill(priv, &priv->rxfill_ring);
			return budget;
		}

		if (napi_complete_done(napi, done))
			edma_rx_irq_unmask(priv);
	}

	return done;
}

static netdev_tx_t edma_ring_xmit(struct edma_priv *priv, struct net_device *netdev,
				  struct sk_buff *skb,
				  struct edma_ring *txdesc_ring)
{
	const struct edma_soc_data *soc = priv->soc;
	struct edma_tx_preheader *txph;
	struct dsa_oob_tag_info *tag_info;
	struct edma_txdesc *txdesc;
	u16 prod, cons, next;
	u16 buf_len, dst_info;
	dma_addr_t dma;
	u32 val, idx;

	spin_lock_bh(&priv->tx_lock);

	regmap_read(priv->regmap,
		    EDMA_REG_TXDESC_PROD_IDX(soc->txdesc_ring),
		    &val);
	prod = val & EDMA_TXDESC_PROD_IDX_MASK;

	regmap_read(priv->regmap,
		    EDMA_REG_TXDESC_CONS_IDX(soc->txdesc_ring),
		    &val);
	cons = val & EDMA_TXDESC_CONS_IDX_MASK;

	next = (prod + 1) & (txdesc_ring->count - 1);

	if (next == cons) {
		spin_unlock_bh(&priv->tx_lock);
		return NETDEV_TX_BUSY;
	}

	buf_len = skb_headlen(skb);

	tag_info = skb_ext_find(skb, SKB_EXT_DSA_OOB);
	if (tag_info)
		dst_info = (EDMA_DST_PORT_TYPE << 8) |
			   (tag_info->port & EDMA_DST_PORT_ID_MASK);
	else
		dst_info = 0;

	txph = (struct edma_tx_preheader *)skb_push(skb, EDMA_TX_PREHDR_SIZE);
	memset((void *)txph, 0, EDMA_TX_PREHDR_SIZE);

	txph->dst_info = dst_info;

	idx = prod & (txdesc_ring->count - 1);
	if (unlikely(txdesc_ring->skb_store[idx] != NULL)) {
		spin_unlock_bh(&priv->tx_lock);
		return NETDEV_TX_BUSY;
	}

	txdesc_ring->skb_store[idx] = skb;
	txph->opaque = idx;

	txdesc = EDMA_TXDESC_DESC(txdesc_ring, prod);

	dma = dma_map_single(&priv->pdev->dev, skb->data,
			     buf_len + EDMA_TX_PREHDR_SIZE, DMA_TO_DEVICE);
	if (dma_mapping_error(&priv->pdev->dev, dma)) {
		dev_kfree_skb_any(skb);
		txdesc_ring->skb_store[idx] = NULL;
		spin_unlock_bh(&priv->tx_lock);
		return NETDEV_TX_OK;
	}
	txdesc->buffer_addr = cpu_to_le32(dma);

	txdesc->word1 = (1 << EDMA_TXDESC_PREHEADER_SHIFT) |
			((EDMA_TX_PREHDR_SIZE & EDMA_TXDESC_DATA_OFFSET_MASK)
			 << EDMA_TXDESC_DATA_OFFSET_SHIFT) |
			(buf_len & EDMA_TXDESC_DATA_LENGTH_MASK);

	prod = (prod + 1) & (txdesc_ring->count - 1);

	dev_sw_netstats_tx_add(netdev, 1, buf_len);
	netdev_tx_sent_queue(netdev_get_tx_queue(netdev, 0), buf_len);

	/* Ensure descriptor writes are visible before updating prod idx */
	wmb();
	regmap_write(priv->regmap,
		     EDMA_REG_TXDESC_PROD_IDX(soc->txdesc_ring),
		     prod & EDMA_TXDESC_PROD_IDX_MASK);

	if (((cons - prod - 1) & (txdesc_ring->count - 1)) <
	    EDMA_TX_RING_THRESH)
		netif_stop_queue(netdev);

	spin_unlock_bh(&priv->tx_lock);
	return NETDEV_TX_OK;
}

static void edma_rxfill_drain(struct edma_priv *priv, struct edma_ring *rxfill_ring)
{
	const struct edma_soc_data *soc = priv->soc;
	struct edma_rxfill_desc *rxfill_desc;
	u16 cons, prod;
	dma_addr_t dma;
	struct page *page;
	u32 val;

	regmap_read(priv->regmap,
		    EDMA_REG_RXFILL_PROD_IDX(soc->rxfill_ring),
		    &val);
	prod = val & EDMA_RXFILL_PROD_IDX_MASK & (rxfill_ring->count - 1);

	regmap_read(priv->regmap,
		    EDMA_REG_RXFILL_CONS_IDX(soc->rxfill_ring),
		    &val);
	cons = val & EDMA_RXFILL_CONS_IDX_MASK & (rxfill_ring->count - 1);

	while (prod != cons) {
		rxfill_desc = EDMA_RXFILL_DESC(rxfill_ring, cons);
		dma = le32_to_cpu(rxfill_desc->buffer_addr);
		page = virt_to_head_page(phys_to_virt(dma));
		page_pool_put_full_page(priv->page_pool, page, false);

		if (++cons == rxfill_ring->count)
			cons = 0;
	}
}

static void edma_rxdesc_drain(struct edma_priv *priv, struct edma_ring *rxdesc_ring)
{
	const struct edma_soc_data *soc = priv->soc;
	struct edma_rxdesc *rxdesc;
	struct page *page;
	u16 prod, cons;
	u32 val;

	regmap_read(priv->regmap,
		    EDMA_REG_RXDESC_CONS_IDX(soc->rxdesc_ring),
		    &val);
	cons = val & EDMA_RXDESC_CONS_IDX_MASK;

	regmap_read(priv->regmap,
		    EDMA_REG_RXDESC_PROD_IDX(soc->rxdesc_ring),
		    &val);
	prod = val & EDMA_RXDESC_PROD_IDX_MASK;

	while (cons != prod) {
		rxdesc = EDMA_RXDESC_DESC(rxdesc_ring, cons);
		page = virt_to_head_page(phys_to_virt(rxdesc->buffer_addr));
		page_pool_put_full_page(priv->page_pool, page, false);

		if (++cons == rxdesc_ring->count)
			cons = 0;
	}

	regmap_write(priv->regmap,
		     EDMA_REG_RXDESC_CONS_IDX(soc->rxdesc_ring),
		     cons);
}

static void edma_txdesc_drain(struct edma_priv *priv, struct edma_ring *txdesc_ring)
{
	const struct edma_soc_data *soc = priv->soc;
	struct platform_device *pdev = priv->pdev;
	struct edma_txdesc *txdesc;
	struct sk_buff *skb;
	u16 prod, cons;
	size_t buf_len;
	u32 val;

	regmap_read(priv->regmap,
		    EDMA_REG_TXDESC_PROD_IDX(soc->txdesc_ring),
		    &val);
	prod = val & EDMA_TXDESC_PROD_IDX_MASK;

	regmap_read(priv->regmap,
		    EDMA_REG_TXDESC_CONS_IDX(soc->txdesc_ring),
		    &val);
	cons = val & EDMA_TXDESC_CONS_IDX_MASK;

	while (cons != prod) {
		txdesc = EDMA_TXDESC_DESC(txdesc_ring, cons);

		skb = txdesc_ring->skb_store[cons];
		txdesc_ring->skb_store[cons] = NULL;

		if (!skb)
			goto next;

		buf_len = txdesc->word1 & EDMA_TXDESC_DATA_LENGTH_MASK;

		dma_unmap_single(&pdev->dev,
				 le32_to_cpu(txdesc->buffer_addr),
				 buf_len + EDMA_TX_PREHDR_SIZE, DMA_TO_DEVICE);

		dev_kfree_skb_any(skb);
next:
		if (++cons == txdesc_ring->count)
			cons = 0;
	}
}

static int edma_rings_alloc(struct edma_priv *priv)
{
	int ret;

	ret = edma_tx_ring_alloc(priv, &priv->txdesc_ring, EDMA_TX_RING_SIZE,
				 sizeof(struct edma_txdesc));
	if (ret)
		return ret;

	ret = edma_ring_alloc(priv, &priv->txcmpl_ring, EDMA_TX_RING_SIZE,
			      sizeof(struct edma_txcmpl));
	if (ret)
		goto err_txcmpl;

	ret = edma_ring_alloc(priv, &priv->rxfill_ring, EDMA_RX_RING_SIZE,
			      sizeof(struct edma_rxfill_desc));
	if (ret)
		goto err_rxfill;

	ret = edma_ring_alloc(priv, &priv->rxdesc_ring, EDMA_RX_RING_SIZE,
			      sizeof(struct edma_rxdesc));
	if (ret)
		goto err_rxdesc;

	return 0;

err_rxdesc:
	edma_ring_free(priv, &priv->rxfill_ring,
		       sizeof(struct edma_rxfill_desc));
err_rxfill:
	edma_ring_free(priv, &priv->txcmpl_ring, sizeof(struct edma_txcmpl));
err_txcmpl:
	edma_tx_ring_free(priv, &priv->txdesc_ring, sizeof(struct edma_txdesc));
	return ret;
}

static void edma_rings_drain(struct edma_priv *priv)
{
	edma_txdesc_drain(priv, &priv->txdesc_ring);
	edma_clean_tx(priv, &priv->txcmpl_ring, INT_MAX);
	edma_rxfill_drain(priv, &priv->rxfill_ring);
	edma_rxdesc_drain(priv, &priv->rxdesc_ring);

	edma_tx_ring_free(priv, &priv->txdesc_ring, sizeof(struct edma_txdesc));
	edma_ring_free(priv, &priv->txcmpl_ring, sizeof(struct edma_txcmpl));
	edma_ring_free(priv, &priv->rxfill_ring,
		       sizeof(struct edma_rxfill_desc));
	edma_ring_free(priv, &priv->rxdesc_ring, sizeof(struct edma_rxdesc));
}

static void edma_configure_txdesc_ring(struct edma_priv *priv,
				       struct edma_ring *txdesc_ring)
{
	const struct edma_soc_data *soc = priv->soc;
	u32 val;

	regmap_write(priv->regmap, EDMA_REG_TXDESC_BA(soc->txdesc_ring),
		    (u32)txdesc_ring->dma);

	regmap_write(priv->regmap,
		     EDMA_REG_TXDESC_RING_SIZE(soc->txdesc_ring),
		     txdesc_ring->count & EDMA_TXDESC_RING_SIZE_MASK);

	regmap_read(priv->regmap, EDMA_REG_TXDESC_CONS_IDX(soc->txdesc_ring),
		    &val);
	val &= ~EDMA_TXDESC_CONS_IDX_MASK;

	regmap_update_bits(priv->regmap,
			   EDMA_REG_TXDESC_PROD_IDX(soc->txdesc_ring),
			   EDMA_TXDESC_PROD_IDX_MASK, val);
}

static void edma_configure_txcmpl_ring(struct edma_priv *priv,
				       struct edma_ring *txcmpl_ring)
{
	const struct edma_soc_data *soc = priv->soc;

	regmap_write(priv->regmap,
		     EDMA_REG_TXCMPL_BA(soc->txcmpl_base, soc->txcmpl_ring),
		     (u32)txcmpl_ring->dma);
	regmap_write(priv->regmap,
		     EDMA_REG_TXCMPL_RING_SIZE(soc->txcmpl_base,
					       soc->txcmpl_ring),
		     txcmpl_ring->count & EDMA_TXDESC_RING_SIZE_MASK);

	regmap_write(priv->regmap,
		     EDMA_REG_TXCMPL_CTRL(soc->txcmpl_base,
					  soc->txcmpl_ring),
		     EDMA_TXCMPL_RETMODE_OPAQUE);

	regmap_write(priv->regmap,
		     EDMA_REG_TX_MOD_TIMER(soc->tx_int_base,
					   soc->txcmpl_ring),
		     EDMA_TX_MOD_TIMER);

	regmap_write(priv->regmap,
		     EDMA_REG_TX_INT_CTRL(soc->tx_int_base,
					  soc->txcmpl_ring),
		     0x2);
}

static void edma_configure_rxdesc_ring(struct edma_priv *priv,
				       struct edma_ring *rxdesc_ring)
{
	const struct edma_soc_data *soc = priv->soc;
	u32 val;

	regmap_write(priv->regmap,
		     EDMA_REG_RXDESC_BA(soc->rxdesc_ring),
		     (u32)rxdesc_ring->dma);

	val = rxdesc_ring->count & EDMA_RXDESC_RING_SIZE_MASK;
	val |= (EDMA_RX_PREHDR_SIZE & EDMA_RXDESC_PL_OFFSET_MASK)
	       << EDMA_RXDESC_PL_OFFSET_SHIFT;
	regmap_write(priv->regmap,
		     EDMA_REG_RXDESC_RING_SIZE(soc->rxdesc_ring),
		     val);

	regmap_write(priv->regmap,
		     EDMA_REG_RX_MOD_TIMER(soc->rxdesc_ring),
		     EDMA_RX_MOD_TIMER_INIT);

	regmap_write(priv->regmap,
		     EDMA_REG_RX_INT_CTRL(soc->rxdesc_ring),
		     0x2);
}

static void edma_configure_rxfill_ring(struct edma_priv *priv,
				       struct edma_ring *rxfill_ring)
{
	const struct edma_soc_data *soc = priv->soc;

	regmap_write(priv->regmap,
		     EDMA_REG_RXFILL_BA(soc->rxfill_ring),
		     (u32)rxfill_ring->dma);

	regmap_write(priv->regmap,
		     EDMA_REG_RXFILL_RING_SIZE(soc->rxfill_ring),
		     rxfill_ring->count & EDMA_RXFILL_RING_SIZE_MASK);

	edma_rx_fill(priv, rxfill_ring);
}

static void edma_configure_rings(struct edma_priv *priv)
{
	edma_configure_txdesc_ring(priv, &priv->txdesc_ring);
	edma_configure_txcmpl_ring(priv, &priv->txcmpl_ring);
	edma_configure_rxfill_ring(priv, &priv->rxfill_ring);
	edma_configure_rxdesc_ring(priv, &priv->rxdesc_ring);
}

static void edma_rings_disable(struct edma_priv *priv)
{
	const struct edma_soc_data *soc = priv->soc;
	int i;

	for (i = 0; i <= soc->rxdesc_ring; i++)
		regmap_clear_bits(priv->regmap, EDMA_REG_RXDESC_CTRL(i),
				  EDMA_RXDESC_RX_EN);

	for (i = 0; i <= soc->rxfill_ring; i++)
		regmap_clear_bits(priv->regmap, EDMA_REG_RXFILL_RING_EN(i),
				  EDMA_RXFILL_RING_EN);

	for (i = 0; i <= soc->txdesc_ring; i++)
		regmap_clear_bits(priv->regmap, EDMA_REG_TXDESC_CTRL(i),
				  EDMA_TXDESC_TX_EN);
}

static void edma_rings_enable(struct edma_priv *priv)
{
	const struct edma_soc_data *soc = priv->soc;

	regmap_set_bits(priv->regmap,
			EDMA_REG_RXDESC_CTRL(soc->rxdesc_ring),
			EDMA_RXDESC_RX_EN);

	regmap_set_bits(priv->regmap,
			EDMA_REG_RXFILL_RING_EN(soc->rxfill_ring),
			EDMA_RXFILL_RING_EN);

	regmap_set_bits(priv->regmap,
			EDMA_REG_TXDESC_CTRL(soc->txdesc_ring),
			EDMA_TXDESC_TX_EN);
}

static void edma_hw_stop(struct edma_priv *priv)
{
	edma_irq_disable_all(priv);
	edma_rings_disable(priv);
	regmap_write(priv->regmap, EDMA_REG_PORT_CTRL, 0);
}

static void edma_hw_reset(struct edma_priv *priv)
{
	reset_control_assert(priv->rst);
	udelay(100);
	reset_control_deassert(priv->rst);
	udelay(100);
}

static int edma_hw_init(struct edma_priv *priv)
{
	const struct edma_soc_data *soc = priv->soc;
	int ret;
	u32 val;

	edma_hw_reset(priv);
	edma_hw_stop(priv);

	regmap_write(priv->regmap, EDMA_QID2RID_TABLE_MEM(0),
		     soc->rxdesc_ring & 0xF);

	ret = edma_rings_alloc(priv);
	if (ret)
		return ret;

	edma_configure_rings(priv);

	regmap_write(priv->regmap, EDMA_REG_RXDESC2FILL_MAP_0, 0);
	regmap_write(priv->regmap, EDMA_REG_RXDESC2FILL_MAP_1,
		     (soc->rxfill_ring & 0x7)
			<< ((soc->rxdesc_ring % 10) * 3));

	if (soc->txcmpl_ring != soc->txdesc_ring) {
		int map_idx, bit_pos;
		int i;

		for (i = 0; i < 3; i++)
			regmap_write(priv->regmap, EDMA_REG_TXDESC2CMPL_MAP(i), 0);

		map_idx = soc->txdesc_ring / 10;
		bit_pos = (soc->txdesc_ring % 10) * 3;
		regmap_set_bits(priv->regmap, EDMA_REG_TXDESC2CMPL_MAP(map_idx),
				(soc->txcmpl_ring & 0x7) << bit_pos);
	}

	val = EDMA_DMAR_BURST_LEN_SET(soc->burst_enable) |
	      EDMA_DMAR_REQ_PRI_SET(0) | EDMA_DMAR_TXDATA_NUM_SET(31) |
	      EDMA_DMAR_TXDESC_NUM_SET(7) | EDMA_DMAR_RXFILL_NUM_SET(7);
	regmap_write(priv->regmap, EDMA_REG_DMAR_CTRL, val);

	if (soc->axiw_enable)
		regmap_set_bits(priv->regmap, EDMA_REG_AXIW_CTRL,
				EDMA_AXIW_MAX_WR_SIZE_EN);

	regmap_write(priv->regmap, EDMA_REG_MISC_INT_MASK, soc->misc_int_mask);

	regmap_write(priv->regmap, EDMA_REG_PORT_CTRL,
		     EDMA_PORT_PAD_EN | EDMA_PORT_EDMA_EN);

	edma_rings_enable(priv);

	return 0;
}

static void edma_get_drvinfo(struct net_device *netdev,
			     struct ethtool_drvinfo *info)
{
	strscpy(info->driver, "qca-edma", sizeof(info->driver));
	strscpy(info->bus_info, dev_name(netdev->dev.parent),
		sizeof(info->bus_info));
}

static void edma_get_ringparam(struct net_device *netdev,
			       struct ethtool_ringparam *ring,
			       struct kernel_ethtool_ringparam *kernel_ring,
			       struct netlink_ext_ack *extack)
{
	ring->tx_max_pending = EDMA_TX_RING_SIZE;
	ring->rx_max_pending = EDMA_RX_RING_SIZE;
	ring->tx_pending = EDMA_TX_RING_SIZE;
	ring->rx_pending = EDMA_RX_RING_SIZE;
}

static const struct ethtool_ops edma_ethtool_ops = {
	.get_drvinfo = edma_get_drvinfo,
	.get_link = ethtool_op_get_link,
	.get_ringparam = edma_get_ringparam,
};

static int edma_ndo_open(struct net_device *netdev)
{
	struct edma_priv *priv = netdev_priv(netdev);

	netdev_tx_reset_queue(netdev_get_tx_queue(netdev, 0));
	napi_enable(&priv->tx_napi);
	napi_enable(&priv->rx_napi);
	netif_start_queue(netdev);
	edma_tx_irq_unmask(priv);
	edma_rx_irq_unmask(priv);

	return 0;
}

static int edma_ndo_stop(struct net_device *netdev)
{
	struct edma_priv *priv = netdev_priv(netdev);

	edma_tx_irq_mask(priv);
	edma_rx_irq_mask(priv);
	netif_stop_queue(netdev);
	napi_disable(&priv->tx_napi);
	napi_disable(&priv->rx_napi);

	return 0;
}

static netdev_tx_t edma_ndo_xmit(struct sk_buff *skb, struct net_device *netdev)
{
	struct edma_priv *priv = netdev_priv(netdev);
	const struct edma_soc_data *soc = priv->soc;
	netdev_tx_t ret;
	u32 nhead, ntail;

	if (skb->len < ETH_HLEN)
		goto drop;

	if (skb_is_nonlinear(skb) && skb_linearize(skb))
		goto drop;

	if (soc->tx_min_size && skb->len < soc->tx_min_size) {
		if (skb_padto(skb, soc->tx_min_size)) {
			netdev->stats.tx_dropped++;
			return NETDEV_TX_OK;
		}
		skb->len = soc->tx_min_size;
	}

	nhead = netdev->needed_headroom;
	ntail = netdev->needed_tailroom;

	if ((skb_cloned(skb) || skb_headroom(skb) < nhead ||
	     skb_tailroom(skb) < ntail) &&
	    pskb_expand_head(skb, nhead, ntail, GFP_ATOMIC))
		goto drop;

	ret = edma_ring_xmit(priv, netdev, skb, &priv->txdesc_ring);
	if (ret == NETDEV_TX_BUSY)
		netif_stop_queue(netdev);

	return ret;

drop:
	dev_kfree_skb_any(skb);
	netdev->stats.tx_dropped++;

	return NETDEV_TX_OK;
}

static const struct net_device_ops edma_netdev_ops = {
	.ndo_open = edma_ndo_open,
	.ndo_stop = edma_ndo_stop,
	.ndo_start_xmit = edma_ndo_xmit,
	.ndo_set_mac_address = eth_mac_addr,
	.ndo_validate_addr = eth_validate_addr,
	.ndo_get_stats64 = dev_get_tstats64,
};

static int edma_irq_init(struct edma_priv *priv)
{
	struct platform_device *pdev = priv->pdev;
	struct device *dev = &pdev->dev;
	int ret;

	priv->txcmpl_irq = platform_get_irq(pdev, 0);
	if (priv->txcmpl_irq < 0)
		return priv->txcmpl_irq;

	priv->rxfill_irq = platform_get_irq(pdev, 1);
	if (priv->rxfill_irq < 0)
		return priv->rxfill_irq;

	priv->rxdesc_irq = platform_get_irq(pdev, 2);
	if (priv->rxdesc_irq < 0)
		return priv->rxdesc_irq;

	priv->misc_irq = platform_get_irq(pdev, 3);
	if (priv->misc_irq < 0)
		return priv->misc_irq;

	ret = devm_request_irq(dev, priv->txcmpl_irq, edma_tx_irq_handle, 0,
			       "edma_txcmpl", priv);
	if (ret)
		return ret;

	ret = devm_request_irq(dev, priv->rxfill_irq, edma_rx_irq_handle, 0,
			       "edma_rxfill", priv);
	if (ret)
		return ret;

	ret = devm_request_irq(dev, priv->rxdesc_irq, edma_rx_irq_handle, 0,
			       "edma_rxdesc", priv);
	if (ret)
		return ret;

	ret = devm_request_irq(dev, priv->misc_irq, edma_misc_irq_handle, 0,
			       "edma_misc", priv);
	if (ret)
		return ret;

	return 0;
}

static int edma_page_pool_create(struct edma_priv *priv)
{
	struct page_pool_params pp = {
		.pool_size = EDMA_RX_RING_SIZE,
		.nid       = NUMA_NO_NODE,
		.dev       = &priv->pdev->dev,
		.dma_dir   = DMA_FROM_DEVICE,
		.offset    = NET_SKB_PAD,
		.max_len   = EDMA_RX_BUFFER_SIZE,
		.flags     = PP_FLAG_DMA_MAP | PP_FLAG_DMA_SYNC_DEV,
	};

	priv->page_pool = page_pool_create(&pp);
	return PTR_ERR_OR_ZERO(priv->page_pool);
}

static const struct regmap_config edma_regmap_cfg = {
	.reg_bits = 32,
	.reg_stride = 4,
	.val_bits = 32,
};

static int edma_probe(struct platform_device *pdev)
{
	struct clk_bulk_data *clks;
	struct device *dev = &pdev->dev;
	struct reset_control *rst;
	struct net_device *netdev;
	struct edma_priv *priv;
	struct regmap *regmap;
	void __iomem *base;
	int ret;

	ret = devm_clk_bulk_get_all_enabled(dev, &clks);
	if (ret < 0)
		return ret;

	rst = devm_reset_control_get(dev, EDMA_HW_RESET_ID);
	if (IS_ERR(rst))
		return PTR_ERR(rst);

	base = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(base))
		return dev_err_probe(dev, PTR_ERR(base), "failed to ioremap resource");

	regmap = devm_regmap_init_mmio(dev, base, &edma_regmap_cfg);
	if (IS_ERR(regmap))
		return dev_err_probe(dev, PTR_ERR(regmap), "failed to init regmap");

	ret = dma_set_mask_and_coherent(dev, DMA_BIT_MASK(32));
	if (ret)
		return ret;

	netdev = devm_alloc_etherdev(dev, sizeof(*priv));
	if (!netdev)
		return -ENOMEM;

	priv = netdev_priv(netdev);
	priv->regmap = regmap;
	priv->rst = rst;
	spin_lock_init(&priv->tx_lock);
	priv->pdev = pdev;
	priv->soc = device_get_match_data(dev);

	ret = edma_page_pool_create(priv);
	if (ret)
		return ret;

	ret = edma_hw_init(priv);
	if (ret)
		goto err_page_pool;

	SET_NETDEV_DEV(netdev, dev);
	netdev->dev.of_node = dev->of_node;
	eth_hw_addr_random(netdev);
	netdev->netdev_ops = &edma_netdev_ops;
	netdev->features = NETIF_F_GRO;
	netdev->pcpu_stat_type = NETDEV_PCPU_STAT_TSTATS;
	netdev->watchdog_timeo = 5 * HZ;
	netdev->max_mtu = EDMA_RX_BUFFER_SIZE - ETH_HLEN - (2 * VLAN_HLEN);
	netdev->needed_headroom = EDMA_TX_PREHDR_SIZE;
	netdev->ethtool_ops = &edma_ethtool_ops;

	priv->netdev = netdev;

	netif_napi_add(netdev, &priv->tx_napi, edma_tx_napi);
	netif_napi_add(netdev, &priv->rx_napi, edma_rx_napi);

	ret = edma_irq_init(priv);
	if (ret)
		goto err_irq;

	ret = register_netdev(netdev);
	if (ret) {
		dev_warn(dev, "failed to register conduit netdevice\n");
		goto err_irq;
	}

	platform_set_drvdata(pdev, priv);

	return 0;

err_irq:
	netif_napi_del(&priv->tx_napi);
	netif_napi_del(&priv->rx_napi);
	edma_hw_stop(priv);
	edma_rings_drain(priv);
err_page_pool:
	page_pool_destroy(priv->page_pool);
	return ret;
}

static void edma_remove(struct platform_device *pdev)
{
	struct edma_priv *priv = platform_get_drvdata(pdev);

	unregister_netdev(priv->netdev);
	netif_napi_del(&priv->tx_napi);
	netif_napi_del(&priv->rx_napi);
	edma_hw_stop(priv);
	edma_rings_drain(priv);
	page_pool_destroy(priv->page_pool);
}

static const struct edma_soc_data ipq60xx_data = {
	.txcmpl_base = 0x79000,
	.tx_int_base = 0x91000,
	.misc_int_mask = 0xff,
	.txdesc_ring = 23,
	.txcmpl_ring = 23,
	.rxfill_ring = 7,
	.rxdesc_ring = 15,
	.burst_enable = true,
	.axiw_enable = true,
};

static const struct edma_soc_data ipq807x_data = {
	.txcmpl_base = 0x19000,
	.tx_int_base = 0x21000,
	.misc_int_mask = 0x1ff,
	.txdesc_ring = 23,
	.txcmpl_ring = 7,
	.rxfill_ring = 7,
	.rxdesc_ring = 15,
	.tx_min_size = 33,
};

static const struct of_device_id edma_of_match[] = {
	{ .compatible = "qualcomm,ipq6018-edma", .data = &ipq60xx_data },
	{ .compatible = "qualcomm,ipq8074-edma", .data = &ipq807x_data },
	{},
};

static struct platform_driver edma_driver = {
	.driver = {
		.name = "qca-edma",
		.of_match_table = edma_of_match,
	},
	.probe = edma_probe,
	.remove = edma_remove,
};

module_platform_driver(edma_driver);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Qualcomm IPQ EDMA Ethernet driver");

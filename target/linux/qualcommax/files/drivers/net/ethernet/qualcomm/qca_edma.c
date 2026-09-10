// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2016-2021, The Linux Foundation. All rights reserved.
 * Copyright (c) 2023-2024, Qualcomm Innovation Center, Inc. All rights reserved.
 */

#include <linux/clk.h>
#include <linux/ethtool.h>
#include <linux/hash.h>
#include <linux/if_vlan.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_net.h>
#include <linux/of_platform.h>
#include <linux/property.h>
#include <linux/regmap.h>
#include <linux/reset.h>
#include <linux/version.h>
#include <net/netdev_queues.h>

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

	priv->stats.misc_error++;
	dev_warn_ratelimited(&priv->pdev->dev, "misc error %#x\n", val);

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

static int edma_rx_ring_alloc(struct edma_priv *priv, struct edma_ring *ring,
			      int count, int desc_size)
{
	int ret;

	ret = edma_ring_alloc(priv, ring, count, desc_size);
	if (ret)
		return ret;

	ring->page_store = kcalloc(count, sizeof(*ring->page_store),
				   GFP_KERNEL);
	if (!ring->page_store) {
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

static u32 edma_tx_release(struct edma_priv *priv, u32 idx, struct sk_buff *skb,
			   int napi_budget);

static void edma_tx_ring_free(struct edma_priv *priv, struct edma_ring *ring,
			      int desc_size)
{
	int i;

	if (ring->skb_store) {
		for (i = 0; i < ring->count; i++) {
			struct edma_txdesc *txdesc = EDMA_TXDESC_DESC(ring, i);
			struct sk_buff *skb = ring->skb_store[i];

			if (skb && (txdesc->word1 & EDMA_TXDESC_PREHEADER))
				edma_tx_release(priv, i, skb, 0);
		}
		kfree(ring->skb_store);
		ring->skb_store = NULL;
	}

	edma_ring_free(priv, ring, desc_size);
}

static int edma_rx_fill(struct edma_priv *priv, struct edma_ring *rxfill_ring)
{
	const struct edma_soc_data *soc = priv->soc;
	struct edma_rxfill_desc *rxfill_desc;
	struct edma_rx_preheader *rxph;
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
		/* The page may still be prefetched inside EDMA. */
		if (unlikely(rxfill_ring->page_store[prod]))
			break;

		page = page_pool_dev_alloc_pages(priv->page_pool);
		if (unlikely(!page))
			break;

		rxfill_desc = EDMA_RXFILL_DESC(rxfill_ring, prod);

		dma = page_pool_get_dma_addr(page) + NET_SKB_PAD;
		rxph = page_address(page) + NET_SKB_PAD;
		rxph->opaque = cpu_to_le32(prod);
		dma_sync_single_for_device(&priv->pdev->dev, dma,
					   sizeof(rxph->opaque), DMA_FROM_DEVICE);
		rxfill_ring->page_store[prod] = page;
		rxfill_desc->buffer_addr = cpu_to_le32(dma);
		rxfill_desc->word1 = cpu_to_le32(priv->rx_buffer_size &
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

static bool edma_rx_page_take(struct edma_priv *priv, struct page *page,
			      u32 store_idx)
{
	struct edma_ring *ring = &priv->rxfill_ring;
	int i;

	if (likely(store_idx < ring->count &&
		   ring->page_store[store_idx] == page)) {
		ring->page_store[store_idx] = NULL;
		return true;
	}

	for (i = 0; i < ring->count; i++) {
		if (ring->page_store[i] != page)
			continue;

		ring->page_store[i] = NULL;
		dev_warn_ratelimited(&priv->pdev->dev,
				     "rx page has invalid store index %u, expected %d\n",
				     store_idx, i);
		return true;
	}

	/* The page may already belong to an skb, so it cannot be freed safely. */
	dev_warn_ratelimited(&priv->pdev->dev,
			     "rx page with store index %u is not tracked\n",
			     store_idx);
	return false;
}

/* Descriptors the transmit ring has left, taken from the hardware rather than
 * from a cached index: the stop and its recheck have to see what the engine
 * has consumed by now, not what it had consumed when the frame arrived.
 */
static u16 edma_txdesc_free(struct edma_priv *priv)
{
	const struct edma_soc_data *soc = priv->soc;
	u32 prod, cons;

	regmap_read(priv->regmap, EDMA_REG_TXDESC_PROD_IDX(soc->txdesc_ring),
		    &prod);
	regmap_read(priv->regmap, EDMA_REG_TXDESC_CONS_IDX(soc->txdesc_ring),
		    &cons);

	return ((cons & EDMA_TXDESC_CONS_IDX_MASK) -
		(prod & EDMA_TXDESC_PROD_IDX_MASK) - 1) &
	       (priv->txdesc_ring.count - 1);
}

/* Unmaps every descriptor the frame was written from and releases the slots
 * that named them, returning the bytes the frame carried. A slot goes back
 * only once its descriptor has been read, so that a transmit taking the slot
 * cannot place a frame over what this walk has yet to reach.
 */
static u32 edma_tx_release(struct edma_priv *priv, u32 idx, struct sk_buff *skb,
			   int napi_budget)
{
	struct edma_ring *ring = &priv->txdesc_ring;
	struct device *dev = &priv->pdev->dev;
	struct edma_txdesc *txdesc;
	u32 bytes, len;

	txdesc = EDMA_TXDESC_DESC(ring, idx);
	len = skb_headlen(skb);

	dma_unmap_single(dev, le32_to_cpu(txdesc->buffer_addr), len,
			 DMA_TO_DEVICE);
	bytes = len - EDMA_TX_PREHDR_SIZE;
	ring->skb_store[idx] = NULL;

	idx = (idx + 1) & (ring->count - 1);
	while (ring->skb_store[idx] == skb) {
		txdesc = EDMA_TXDESC_DESC(ring, idx);
		len = txdesc->word1 & EDMA_TXDESC_DATA_LENGTH_MASK;

		dma_unmap_page(dev, le32_to_cpu(txdesc->buffer_addr), len,
			       DMA_TO_DEVICE);
		bytes += len;
		ring->skb_store[idx] = NULL;
		idx = (idx + 1) & (ring->count - 1);
	}

	napi_consume_skb(skb, napi_budget);

	return bytes;
}

/* @napi_budget is the NAPI budget the poll was given, or zero when the caller
 * is not a poll: the skb cache napi_consume_skb() recycles into is per-CPU and
 * is only safe to touch from softirq context.
 */
static u32 edma_clean_tx(struct edma_priv *priv, struct edma_ring *txcmpl_ring,
			 int budget, int napi_budget)
{
	const struct edma_soc_data *soc = priv->soc;
	struct platform_device *pdev = priv->pdev;
	u32 cleaned = 0, pkts = 0, bytes = 0;
	struct edma_txcmpl *txcmpl;
	struct sk_buff *skb;
	u16 prod, cons;
	u32 val;

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

		if (unlikely(txcmpl->status & EDMA_TXCMPL_ERROR)) {
			dev_warn_ratelimited(&pdev->dev, "tx error %#x\n",
					     txcmpl->status);
			priv->stats.tx_desc_error++;
			priv->netdev->stats.tx_errors++;
		}

		/* A frame is named by the first completion of its run and
		 * released on the one that clears the more bit; the opaque of
		 * the completions in between is not written.
		 */
		/* The opaque is only written for the descriptor that carried
		 * the preheader, so it is read once per run and not again if
		 * the run turns out to name no frame: a later completion of
		 * the same run would otherwise hand back whatever its field
		 * held and name a frame the engine still owns.
		 */
		if (!priv->txcmpl_run) {
			priv->txcmpl_run = true;
			priv->txcmpl_idx = txcmpl->buffer_addr;
			if (priv->txcmpl_idx < priv->txdesc_ring.count)
				priv->txcmpl_skb =
					priv->txdesc_ring.skb_store[priv->txcmpl_idx];
		}

		if (!(txcmpl->status & EDMA_TXCMPL_MORE)) {
			skb = priv->txcmpl_skb;
			priv->txcmpl_skb = NULL;
			priv->txcmpl_run = false;

			if (unlikely(!skb)) {
				dev_warn(&pdev->dev,
					 "invalid skb: cons:%u prod:%u status %x\n",
					 cons, prod, txcmpl->status);
				priv->stats.tx_unnamed_frame++;
			} else {
				bytes += edma_tx_release(priv,
							 priv->txcmpl_idx, skb,
							 napi_budget);
				pkts++;
			}
		}

		if (++cons == txcmpl_ring->count)
			cons = 0;

		cleaned++;
	}

	if (cleaned == 0)
		return 0;

	/* A drain runs with the queue deliberately stopped and the rings about
	 * to be freed, so only a poll may wake it.
	 */
	__netif_txq_completed_wake(netdev_get_tx_queue(priv->netdev, 0),
				   pkts, bytes, edma_txdesc_free(priv),
				   EDMA_TX_RING_THRESH, !napi_budget);

	/* Ensure all TX completions are processed before updating cons idx */
	wmb();
	regmap_write(priv->regmap,
		     EDMA_REG_TXCMPL_CONS_IDX(soc->txcmpl_base,
					      soc->txcmpl_ring),
		     cons);

	return cleaned;
}

/* The engine verifies the transport checksum of a TCP or UDP frame and
 * reports the result per descriptor. It reports a header checksum too, which
 * only IPv4 carries, and it names the packet type it parsed so that a verdict
 * on a frame it does not checksum is never taken for one it does.
 */
static void edma_rx_csum(struct net_device *netdev, struct sk_buff *skb,
			 const struct edma_rx_preheader *rxph, u32 status,
			 unsigned int l2_len)
{
	u32 pre4, sum, l4_off;
	__sum16 hw;
	u8 pid;

	if (!(netdev->features & NETIF_F_RXCSUM))
		return;

	pid = (status >> EDMA_RXDESC_PID_SHIFT) & EDMA_RXDESC_PID_MASK;

	/* A transport the engine parses is answered with a verdict, which
	 * settles the frame without the stack reading it at all.
	 */
	if (BIT(pid) & EDMA_RXDESC_PID_TCP_UDP) {
		if (!(status & EDMA_RXDESC_L4_CSUM_OK))
			return;

		if (!(pid & EDMA_RXDESC_PID_IPV6) &&
		    !(status & EDMA_RXDESC_L3_CSUM_OK))
			return;

		skb->ip_summed = CHECKSUM_UNNECESSARY;
		return;
	}

	/* Every other transport over IP is summed rather than judged, and the
	 * engine reports the complement of that sum in packet order, writing a
	 * sum of zero as its other representation. Carrying it up spares the
	 * stack the walk over the payload; only the headers ahead of the
	 * transport are left to add.
	 */
	if (pid == EDMA_RXDESC_PID_NON_IP)
		return;

	pre4 = le32_to_cpu(rxph->rx_pre4);
	l4_off = (pre4 >> EDMA_RXPH_L4_OFFSET_SHIFT) & EDMA_RXPH_L4_OFFSET_MASK;
	if (l4_off <= l2_len || l4_off > skb_headlen(skb) + l2_len)
		return;

	sum = (le32_to_cpu(rxph->rx_pre6) >> EDMA_RXPH_CSUM_SHIFT) &
	      EDMA_RXPH_CSUM_MASK;
	hw = (__force __sum16)cpu_to_be16(sum);

	skb->csum = csum_partial(skb->data, l4_off - l2_len, csum_unfold(~hw));
	skb->ip_summed = CHECKSUM_COMPLETE;
}

/* The parser hashes the tuple it matched and says which tuple that was. The
 * same field takes other values that are not a tuple hash, so only the two
 * tuple verdicts are taken.
 *
 * The value arrives in the low bits of the word while the stack scales a hash
 * across the whole of it, so a hash left where the parser put it selects the
 * same receive queue for every flow. Spreading it over the word is one to one,
 * so the frames of a flow still land together.
 */
static void edma_rx_hash(struct net_device *netdev, struct sk_buff *skb,
			 const struct edma_rx_preheader *rxph)
{
	u32 pre2 = le32_to_cpu(rxph->rx_pre2);
	u8 flag;

	if (!(netdev->features & NETIF_F_RXHASH))
		return;

	flag = (pre2 >> EDMA_RXPH_HASH_FLAG_SHIFT) & EDMA_RXPH_HASH_FLAG_MASK;
	if (flag != EDMA_RXPH_HASH_5TUPLE && flag != EDMA_RXPH_HASH_3TUPLE)
		return;

	skb_set_hash(skb, hash_32(pre2 & EDMA_RXPH_HASH_MASK, 32),
		     flag == EDMA_RXPH_HASH_5TUPLE ? PKT_HASH_TYPE_L4 :
						     PKT_HASH_TYPE_L3);
}

static u32 edma_clean_rx(struct edma_priv *priv, int budget,
			 struct edma_ring *rxdesc_ring)
{
	const struct edma_soc_data *soc = priv->soc;
	struct net_device *netdev = priv->netdev;
	struct platform_device *pdev = priv->pdev;
	struct dsa_oob_tag_info *tag_info;
	struct edma_rx_preheader *rxph;
	struct edma_rxdesc *rxdesc;
	unsigned char *frame;
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
		u32 desc_addr, desc_status;
		u32 store_idx;

		rxdesc = EDMA_RXDESC_DESC(rxdesc_ring, cons);
		desc_addr = le32_to_cpu(rxdesc->buffer_addr);
		desc_status = le32_to_cpu(rxdesc->status);
		rxph = phys_to_virt(desc_addr);
		page = virt_to_head_page(rxph);

		pkt_len = desc_status & EDMA_RXDESC_PACKET_LEN_MASK;

		if (unlikely(desc_status & EDMA_RXDESC_MORE))
			priv->stats.rx_split_frame++;

		page_pool_dma_sync_for_cpu(priv->page_pool, page, 0,
					   EDMA_RX_PREHDR_SIZE + pkt_len);
		store_idx = le32_to_cpu(rxph->opaque);
		if (unlikely(!edma_rx_page_take(priv, page, store_idx))) {
			priv->stats.rx_untracked_page++;
			netdev->stats.rx_errors++;
			goto next;
		}

		if (EDMA_RXPH_SRC_INFO_TYPE_GET(rxph) !=
		    EDMA_PREHDR_DSTINFO_PORTID_IND) {
			dev_warn_ratelimited(&pdev->dev,
					     "rx drop: src_info_type=%#x src_info=%#06x dst_info=%#06x\n",
					     EDMA_RXPH_SRC_INFO_TYPE_GET(rxph),
					     le16_to_cpu(rxph->src_info),
					     le16_to_cpu(rxph->dst_info));
			page_pool_put_full_page(priv->page_pool, page, true);
			priv->stats.rx_bad_src_info++;
			netdev->stats.rx_errors++;
			goto next;
		}

		src_port = rxph->src_info & EDMA_SRC_PORT_MASK;

		skb = napi_build_skb(page_address(page), page_size(page));
		if (unlikely(!skb)) {
			page_pool_put_full_page(priv->page_pool, page, true);
			priv->stats.rx_no_skb++;
			netdev->stats.rx_dropped++;
			goto next;
		}

		skb_mark_for_recycle(skb);
		skb_reserve(skb, NET_SKB_PAD + EDMA_RX_PREHDR_SIZE);
		skb_put(skb, pkt_len);

		frame = skb->data;
		skb->protocol = eth_type_trans(skb, priv->netdev);
		edma_rx_csum(netdev, skb, rxph, desc_status,
			     skb->data - frame);
		edma_rx_hash(netdev, skb, rxph);

		tag_info = skb_ext_add(skb, SKB_EXT_DSA_OOB);
		if (unlikely(!tag_info)) {
			dev_kfree_skb_any(skb);
			priv->stats.rx_no_tag++;
			netdev->stats.rx_dropped++;
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
	int work = edma_clean_tx(priv, &priv->txcmpl_ring, budget, budget);
	const struct edma_soc_data *soc = priv->soc;
	u32 val;

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
			priv->stats.rx_fill_starved++;
			priv->netdev->stats.rx_missed_errors++;
			edma_rx_fill(priv, &priv->rxfill_ring);
			return budget;
		}

		if (napi_complete_done(napi, done))
			edma_rx_irq_unmask(priv);
	}

	return done;
}

/* The engine generates the transport checksum of an outgoing TCP or UDP frame,
 * and the IPv4 header checksum with it. It parses the frame for the offsets
 * rather than being handed a start and an offset, so the offload is announced
 * per protocol.
 */
static void edma_tx_csum(struct sk_buff *skb, struct edma_tx_preheader *txph,
			 __be16 proto)
{
	if (skb->ip_summed != CHECKSUM_PARTIAL)
		return;

	txph->tx_pre4 |= EDMA_TX_PRE4_ADV_OFFLOAD_EN;
	txph->tx_pre6 |= EDMA_TX_PRE6_CSUM_MODE_L4;

	if (proto == htons(ETH_P_IP))
		txph->tx_pre6 |= EDMA_TX_PRE6_IP_CSUM_EN;
}

/* Segmentation is one bit in every descriptor of the frame and the segment
 * size in the preheader. The engine writes the headers of each segment it
 * cuts, so the checksums it is already asked for cover what it produced.
 */
static u32 edma_tx_tso(struct sk_buff *skb, struct edma_tx_preheader *txph)
{
	if (!skb_is_gso(skb))
		return 0;

	txph->tx_pre6 |= skb_shinfo(skb)->gso_size & EDMA_TX_PRE6_MSS_MASK;

	return EDMA_TXDESC_TSO_EN;
}

static netdev_tx_t edma_ring_xmit(struct edma_priv *priv, struct net_device *netdev,
				  struct sk_buff *skb,
				  struct edma_ring *txdesc_ring)
{
	const struct skb_shared_info *shinfo = skb_shinfo(skb);
	const struct edma_soc_data *soc = priv->soc;
	struct device *dev = &priv->pdev->dev;
	u16 mask = txdesc_ring->count - 1;
	struct edma_tx_preheader *txph;
	struct dsa_oob_tag_info *tag_info;
	u16 ndesc = shinfo->nr_frags + 1;
	struct edma_txdesc *txdesc;
	u16 prod, cons, dst_info;
	u32 val, idx, i, len, bytes, tso;
	dma_addr_t head_dma;
	bool taken = false;
	__be16 proto;

	spin_lock_bh(&priv->tx_lock);

	regmap_read(priv->regmap,
		    EDMA_REG_TXDESC_PROD_IDX(soc->txdesc_ring),
		    &val);
	prod = val & EDMA_TXDESC_PROD_IDX_MASK;

	regmap_read(priv->regmap,
		    EDMA_REG_TXDESC_CONS_IDX(soc->txdesc_ring),
		    &val);
	cons = val & EDMA_TXDESC_CONS_IDX_MASK;

	idx = prod & mask;

	/* A frame holds every store slot it spans, not only the one its
	 * preheader names, so that a later frame placed over its descriptors
	 * cannot change the addresses its completion still has to unmap.
	 */
	for (i = 0; i < ndesc; i++)
		taken |= !!txdesc_ring->skb_store[(idx + i) & mask];

	/* Both refusals come before the preheader is pushed: the qdisc requeues
	 * the frame as it was handed over, and a second push would prefix it
	 * twice and hand the hardware a length that no longer describes it.
	 */
	if (taken || ((cons - prod - 1) & mask) < ndesc)
		goto busy;

	len = skb_headlen(skb);
	bytes = skb->len;

	/* vlan_get_protocol() walks the tags from skb->data, so the protocol is
	 * taken while that still points at the MAC header.
	 */
	proto = vlan_get_protocol(skb);

	tag_info = skb_ext_find(skb, SKB_EXT_DSA_OOB);
	if (tag_info)
		dst_info = (EDMA_DST_PORT_TYPE << 8) |
			   (tag_info->port & EDMA_DST_PORT_ID_MASK);
	else
		dst_info = 0;

	txph = (struct edma_tx_preheader *)skb_push(skb, EDMA_TX_PREHDR_SIZE);
	memset((void *)txph, 0, EDMA_TX_PREHDR_SIZE);

	txph->dst_info = dst_info;
	edma_tx_csum(skb, txph, proto);
	tso = edma_tx_tso(skb, txph);

	txdesc_ring->skb_store[idx] = skb;
	txph->opaque = idx;

	head_dma = dma_map_single(dev, skb->data, len + EDMA_TX_PREHDR_SIZE,
				  DMA_TO_DEVICE);
	if (dma_mapping_error(dev, head_dma)) {
		txdesc_ring->skb_store[idx] = NULL;
		goto drop;
	}

	txdesc = EDMA_TXDESC_DESC(txdesc_ring, idx);
	txdesc->buffer_addr = cpu_to_le32(head_dma);
	txdesc->word1 = tso | (1 << EDMA_TXDESC_PREHEADER_SHIFT) |
			(ndesc > 1 ? EDMA_TXDESC_MORE : 0) |
			((EDMA_TX_PREHDR_SIZE & EDMA_TXDESC_DATA_OFFSET_MASK)
			 << EDMA_TXDESC_DATA_OFFSET_SHIFT) |
			(len & EDMA_TXDESC_DATA_LENGTH_MASK);

	for (i = 0; i < shinfo->nr_frags; i++) {
		const skb_frag_t *frag = &shinfo->frags[i];
		u32 fidx = (idx + 1 + i) & mask;
		dma_addr_t dma;

		len = skb_frag_size(frag);
		dma = skb_frag_dma_map(dev, frag, 0, len, DMA_TO_DEVICE);
		if (dma_mapping_error(dev, dma))
			goto unmap;

		txdesc_ring->skb_store[fidx] = skb;
		txdesc = EDMA_TXDESC_DESC(txdesc_ring, fidx);
		txdesc->buffer_addr = cpu_to_le32(dma);
		txdesc->word1 = tso | (i + 1 < shinfo->nr_frags ?
				       EDMA_TXDESC_MORE : 0) |
				(len & EDMA_TXDESC_DATA_LENGTH_MASK);
	}

	prod = (prod + ndesc) & mask;

	dev_sw_netstats_tx_add(netdev, 1, bytes);
	netdev_tx_sent_queue(netdev_get_tx_queue(netdev, 0), bytes);

	/* Ensure descriptor writes are visible before updating prod idx */
	wmb();
	regmap_write(priv->regmap,
		     EDMA_REG_TXDESC_PROD_IDX(soc->txdesc_ring),
		     prod & EDMA_TXDESC_PROD_IDX_MASK);

	/* The queue is rechecked against the hardware once it is stopped: a
	 * completion that drains the ring between the descriptor going out and
	 * the stop landing would otherwise find the queue still running and
	 * leave nothing behind to start it again. The indices this frame was
	 * placed from decide whether to stop at all, since a consumer index
	 * only ages into reporting less room than the ring has.
	 */
	if (((cons - prod - 1) & mask) < EDMA_TX_RING_THRESH)
		netif_txq_try_stop(netdev_get_tx_queue(netdev, 0),
				   edma_txdesc_free(priv),
				   EDMA_TX_RING_THRESH);

	spin_unlock_bh(&priv->tx_lock);
	return NETDEV_TX_OK;

unmap:
	while (i--) {
		u32 fidx = (idx + 1 + i) & mask;

		txdesc = EDMA_TXDESC_DESC(txdesc_ring, fidx);
		dma_unmap_page(dev, le32_to_cpu(txdesc->buffer_addr),
			       txdesc->word1 & EDMA_TXDESC_DATA_LENGTH_MASK,
			       DMA_TO_DEVICE);
		txdesc_ring->skb_store[fidx] = NULL;
	}

	dma_unmap_single(dev, head_dma, skb_headlen(skb), DMA_TO_DEVICE);
	txdesc_ring->skb_store[idx] = NULL;
drop:
	dev_kfree_skb_any(skb);
	spin_unlock_bh(&priv->tx_lock);
	return NETDEV_TX_OK;

busy:
	/* A refusal stops the queue here rather than in the caller, so that the
	 * recheck happens against the state this refusal was decided on: a
	 * completion that drains the ring once the lock is dropped would
	 * otherwise find the queue still running and leave nothing behind to
	 * start it again. A taken store slot outlives the descriptor that named
	 * it, because the engine releases the descriptor as soon as it reads it
	 * and only the completion clears the slot, so a free descriptor count
	 * would restart the queue on a resource the refused frame still lacks.
	 */
	netif_txq_try_stop(netdev_get_tx_queue(netdev, 0),
			   taken ? 0 : edma_txdesc_free(priv),
			   EDMA_TX_RING_THRESH);

	spin_unlock_bh(&priv->tx_lock);
	return NETDEV_TX_BUSY;
}

static void edma_rx_ring_free(struct edma_priv *priv, struct edma_ring *ring,
			      int desc_size)
{
	int i;

	if (ring->page_store) {
		/* Hardware indices do not account for prefetched RX pages. */
		for (i = 0; i < ring->count; i++) {
			if (!ring->page_store[i])
				continue;

			page_pool_put_full_page(priv->page_pool,
						ring->page_store[i], false);
			ring->page_store[i] = NULL;
		}

		kfree(ring->page_store);
		ring->page_store = NULL;
	}

	edma_ring_free(priv, ring, desc_size);
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

	ret = edma_rx_ring_alloc(priv, &priv->rxfill_ring, EDMA_RX_RING_SIZE,
				 sizeof(struct edma_rxfill_desc));
	if (ret)
		goto err_rxfill;

	ret = edma_ring_alloc(priv, &priv->rxdesc_ring, EDMA_RX_RING_SIZE,
			      sizeof(struct edma_rxdesc));
	if (ret)
		goto err_rxdesc;

	return 0;

err_rxdesc:
	edma_rx_ring_free(priv, &priv->rxfill_ring,
			  sizeof(struct edma_rxfill_desc));
err_rxfill:
	edma_ring_free(priv, &priv->txcmpl_ring, sizeof(struct edma_txcmpl));
err_txcmpl:
	edma_tx_ring_free(priv, &priv->txdesc_ring, sizeof(struct edma_txdesc));
	return ret;
}

static void edma_rings_drain(struct edma_priv *priv)
{
	edma_clean_tx(priv, &priv->txcmpl_ring, INT_MAX, 0);
	priv->txcmpl_skb = NULL;
	priv->txcmpl_run = false;

	edma_tx_ring_free(priv, &priv->txdesc_ring, sizeof(struct edma_txdesc));
	edma_ring_free(priv, &priv->txcmpl_ring, sizeof(struct edma_txcmpl));
	edma_rx_ring_free(priv, &priv->rxfill_ring,
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
	int i, ret;
	u32 val;

	edma_hw_reset(priv);
	edma_hw_stop(priv);

	/* Every queue names the one receive ring this driver enables. Ring 0 is
	 * never given a base address here, so a queue left pointing at it would
	 * deliver nowhere.
	 */
	val = (soc->rxdesc_ring & EDMA_QID2RID_RING_MASK) * 0x11111111u;
	for (i = 0; i < EDMA_QID2RID_DEPTH; i++)
		regmap_write(priv->regmap, EDMA_QID2RID_TABLE_MEM(i), val);

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

/* The registers the driver drives, in the order it configures them: the
 * global block, then each ring it owns and the interrupt that serves it. The
 * dump pairs every value with the offset it came from, so it reads without a
 * table on the other side. The misc status is left out, so that a dump cannot
 * take an error away from the handler.
 */
static int edma_get_regs_len(struct net_device *netdev)
{
	return EDMA_REGS_COUNT * 2 * sizeof(u32);
}

static void edma_get_regs(struct net_device *netdev,
			  struct ethtool_regs *regs, void *p)
{
	struct edma_priv *priv = netdev_priv(netdev);
	const struct edma_soc_data *soc = priv->soc;
	const u32 off[EDMA_REGS_COUNT] = {
		EDMA_REG_PORT_CTRL,
		EDMA_REG_DMAR_CTRL,
		EDMA_REG_AXIW_CTRL,
		EDMA_REG_MISC_INT_MASK,

		EDMA_REG_TXDESC_BA(soc->txdesc_ring),
		EDMA_REG_TXDESC_PROD_IDX(soc->txdesc_ring),
		EDMA_REG_TXDESC_CONS_IDX(soc->txdesc_ring),
		EDMA_REG_TXDESC_RING_SIZE(soc->txdesc_ring),
		EDMA_REG_TXDESC_CTRL(soc->txdesc_ring),

		EDMA_REG_TXCMPL_BA(soc->txcmpl_base, soc->txcmpl_ring),
		EDMA_REG_TXCMPL_PROD_IDX(soc->txcmpl_base, soc->txcmpl_ring),
		EDMA_REG_TXCMPL_CONS_IDX(soc->txcmpl_base, soc->txcmpl_ring),
		EDMA_REG_TXCMPL_RING_SIZE(soc->txcmpl_base, soc->txcmpl_ring),
		EDMA_REG_TXCMPL_CTRL(soc->txcmpl_base, soc->txcmpl_ring),

		EDMA_REG_TX_INT_STAT(soc->tx_int_base, soc->txcmpl_ring),
		EDMA_REG_TX_INT_MASK(soc->tx_int_base, soc->txcmpl_ring),
		EDMA_REG_TX_MOD_TIMER(soc->tx_int_base, soc->txcmpl_ring),
		EDMA_REG_TX_INT_CTRL(soc->tx_int_base, soc->txcmpl_ring),

		EDMA_REG_RXFILL_BA(soc->rxfill_ring),
		EDMA_REG_RXFILL_PROD_IDX(soc->rxfill_ring),
		EDMA_REG_RXFILL_CONS_IDX(soc->rxfill_ring),
		EDMA_REG_RXFILL_RING_SIZE(soc->rxfill_ring),
		EDMA_REG_RXFILL_RING_EN(soc->rxfill_ring),
		EDMA_REG_RXFILL_INT_STAT(soc->rxfill_ring),
		EDMA_REG_RXFILL_INT_MASK(soc->rxfill_ring),

		EDMA_REG_RXDESC_BA(soc->rxdesc_ring),
		EDMA_REG_RXDESC_PROD_IDX(soc->rxdesc_ring),
		EDMA_REG_RXDESC_CONS_IDX(soc->rxdesc_ring),
		EDMA_REG_RXDESC_RING_SIZE(soc->rxdesc_ring),
		EDMA_REG_RXDESC_CTRL(soc->rxdesc_ring),
		EDMA_REG_RXDESC_INT_STAT(soc->rxdesc_ring),
		EDMA_REG_RXDESC_INT_MASK(soc->rxdesc_ring),
		EDMA_REG_RX_MOD_TIMER(soc->rxdesc_ring),
		EDMA_REG_RX_INT_CTRL(soc->rxdesc_ring),
	};
	u32 *out = p;
	int i;

	regs->version = 1;

	for (i = 0; i < EDMA_REGS_COUNT; i++) {
		u32 val;

		regmap_read(priv->regmap, off[i], &val);
		*out++ = off[i];
		*out++ = val;
	}
}

static const char edma_stat_names[][ETH_GSTRING_LEN] = {
	"rx_untracked_page",
	"rx_bad_src_info",
	"rx_no_skb",
	"rx_no_tag",
	"rx_split_frame",
	"rx_fill_starved",
	"tx_desc_error",
	"tx_unnamed_frame",
	"misc_error",
};

static int edma_get_sset_count(struct net_device *netdev, int sset)
{
	if (sset != ETH_SS_STATS)
		return -EOPNOTSUPP;

	return ARRAY_SIZE(edma_stat_names);
}

static void edma_get_strings(struct net_device *netdev, u32 sset, u8 *data)
{
	if (sset == ETH_SS_STATS)
		memcpy(data, edma_stat_names, sizeof(edma_stat_names));
}

static void edma_get_ethtool_stats(struct net_device *netdev,
				   struct ethtool_stats *stats, u64 *data)
{
	struct edma_priv *priv = netdev_priv(netdev);

	BUILD_BUG_ON(sizeof(priv->stats) !=
		     ARRAY_SIZE(edma_stat_names) * sizeof(u64));

	memcpy(data, &priv->stats, sizeof(priv->stats));
}

static const struct ethtool_ops edma_ethtool_ops = {
	.get_sset_count = edma_get_sset_count,
	.get_strings = edma_get_strings,
	.get_ethtool_stats = edma_get_ethtool_stats,
	.get_drvinfo = edma_get_drvinfo,
	.get_link = ethtool_op_get_link,
	.get_ringparam = edma_get_ringparam,
	.get_regs_len = edma_get_regs_len,
	.get_regs = edma_get_regs,
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

static int edma_ndo_change_mtu(struct net_device *netdev, int new_mtu);

/* A frame outside the bounds it may be described within is made linear
 * instead. The last buffer has no length floor, and the head grows by the
 * preheader before it is mapped, so neither is counted here.
 */
static bool edma_tx_needs_linearize(const struct sk_buff *skb)
{
	const struct skb_shared_info *shinfo = skb_shinfo(skb);
	int i;

	if (shinfo->nr_frags + 1 > EDMA_TX_MAX_SEGS)
		return true;

	for (i = 0; i + 1 < shinfo->nr_frags; i++)
		if (skb_frag_size(&shinfo->frags[i]) < EDMA_TX_MIN_SEG)
			return true;

	return false;
}

static netdev_tx_t edma_ndo_xmit(struct sk_buff *skb, struct net_device *netdev)
{
	struct edma_priv *priv = netdev_priv(netdev);
	const struct edma_soc_data *soc = priv->soc;
	u32 nhead, ntail;

	if (skb->len < ETH_HLEN)
		goto drop;

	if (edma_tx_needs_linearize(skb) && skb_linearize(skb))
		goto drop;

	/* skb_padto() zero-fills the tailroom but leaves the tail where it
	 * was, so advancing the length by hand puts skb->len past the data a
	 * later head reallocation copies: the pad would be reallocated
	 * uninitialised and transmitted.
	 */
	if (soc->tx_min_size && skb_put_padto(skb, soc->tx_min_size)) {
		netdev->stats.tx_dropped++;
		return NETDEV_TX_OK;
	}

	nhead = netdev->needed_headroom;
	ntail = netdev->needed_tailroom;

	if ((skb_cloned(skb) || skb_headroom(skb) < nhead ||
	     skb_tailroom(skb) < ntail) &&
	    pskb_expand_head(skb, nhead, ntail, GFP_ATOMIC))
		goto drop;

	return edma_ring_xmit(priv, netdev, skb, &priv->txdesc_ring);

drop:
	dev_kfree_skb_any(skb);
	netdev->stats.tx_dropped++;

	return NETDEV_TX_OK;
}

static const struct net_device_ops edma_netdev_ops = {
	.ndo_open = edma_ndo_open,
	.ndo_stop = edma_ndo_stop,
	.ndo_start_xmit = edma_ndo_xmit,
	.ndo_change_mtu = edma_ndo_change_mtu,
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

static u8 edma_rx_page_order(int mtu)
{
	size_t size = NET_SKB_PAD + EDMA_RX_PREHDR_SIZE + mtu + ETH_HLEN +
		      2 * VLAN_HLEN +
		      SKB_DATA_ALIGN(sizeof(struct skb_shared_info));

	return get_order(size);
}

static u32 edma_rx_buffer_size(u8 order)
{
	return (PAGE_SIZE << order) - NET_SKB_PAD -
	       SKB_DATA_ALIGN(sizeof(struct skb_shared_info));
}

static struct page_pool *edma_page_pool_create(struct edma_priv *priv,
					       u8 order)
{
	struct page_pool_params pp = {
		.order     = order,
		.pool_size = EDMA_RX_RING_SIZE,
		.nid       = NUMA_NO_NODE,
		.dev       = &priv->pdev->dev,
		.dma_dir   = DMA_FROM_DEVICE,
		.offset    = NET_SKB_PAD,
		.max_len   = edma_rx_buffer_size(order),
		.flags     = PP_FLAG_DMA_MAP | PP_FLAG_DMA_SYNC_DEV,
	};

	return page_pool_create(&pp);
}

static int edma_ndo_change_mtu(struct net_device *netdev, int new_mtu)
{
	struct edma_priv *priv = netdev_priv(netdev);
	struct page_pool *old_pool, *new_pool;
	u8 old_order, new_order;
	bool running;
	int ret;

	new_order = edma_rx_page_order(new_mtu);
	if (new_order == priv->rx_page_order) {
		WRITE_ONCE(netdev->mtu, new_mtu);
		return 0;
	}

	new_pool = edma_page_pool_create(priv, new_order);
	if (IS_ERR(new_pool))
		return PTR_ERR(new_pool);

	running = netif_running(netdev);
	if (running) {
		/* The poll is the other writer of the queue state and it wakes
		 * a stopped queue whenever it completes a frame, so it is put
		 * down first: a wake landing after netif_tx_disable() leaves
		 * the transmit path running into the rings freed below.
		 */
		edma_ndo_stop(netdev);
		netif_tx_disable(netdev);
	}

	edma_hw_stop(priv);
	edma_rings_drain(priv);

	old_pool = priv->page_pool;
	old_order = priv->rx_page_order;
	priv->page_pool = new_pool;
	priv->rx_page_order = new_order;
	priv->rx_buffer_size = edma_rx_buffer_size(new_order);

	ret = edma_hw_init(priv);
	if (ret) {
		int restore_ret;

		priv->page_pool = old_pool;
		priv->rx_page_order = old_order;
		priv->rx_buffer_size = edma_rx_buffer_size(old_order);
		page_pool_destroy(new_pool);

		restore_ret = edma_hw_init(priv);
		if (restore_ret) {
			netdev_err(netdev,
				   "failed to restore receive buffers after MTU change: %d\n",
				   restore_ret);
			if (running) {
				napi_enable(&priv->tx_napi);
				napi_enable(&priv->rx_napi);
			}
			netif_device_detach(netdev);
			return restore_ret;
		}
	} else {
		page_pool_destroy(old_pool);
		WRITE_ONCE(netdev->mtu, new_mtu);
	}

	if (running)
		edma_ndo_open(netdev);

	return ret;
}

static const struct regmap_config edma_regmap_cfg = {
	.reg_bits = 32,
	.reg_stride = 4,
	.val_bits = 32,
};

/*
 * The conduit is a DMA engine behind the switch and has no address of its
 * own, so boards describe none: fall back to the switch this conduit serves,
 * whose ports carry the board's addresses either from DT or patched in by
 * the bootloader. DSA user ports without one of their own inherit whatever
 * ends up here.
 */
static int edma_get_mac_address(struct net_device *netdev,
				struct device_node *np)
{
	struct device_node *cpu_port;
	int ret;

	ret = of_get_ethdev_address(np, netdev);
	if (!ret || ret == -EPROBE_DEFER)
		return ret;

	for_each_node_with_property(cpu_port, "ethernet") {
		struct device_node *conduit __free(device_node) =
			of_parse_phandle(cpu_port, "ethernet", 0);

		if (conduit != np)
			continue;

		for_each_available_child_of_node_scoped(cpu_port->parent, port) {
			ret = of_get_ethdev_address(port, netdev);
			if (!ret || ret == -EPROBE_DEFER) {
				of_node_put(cpu_port);
				return ret;
			}
		}
	}

	return -ENODEV;
}

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

	ret = edma_get_mac_address(netdev, dev->of_node);
	if (ret == -EPROBE_DEFER)
		return dev_err_probe(dev, ret, "failed to get MAC address\n");
	if (ret)
		eth_hw_addr_random(netdev);

	priv->rx_page_order = edma_rx_page_order(netdev->mtu);
	priv->rx_buffer_size = edma_rx_buffer_size(priv->rx_page_order);
	priv->page_pool = edma_page_pool_create(priv, priv->rx_page_order);
	if (IS_ERR(priv->page_pool))
		return PTR_ERR(priv->page_pool);

	ret = edma_hw_init(priv);
	if (ret)
		goto err_page_pool;

	SET_NETDEV_DEV(netdev, dev);
	netdev->dev.of_node = dev->of_node;
	netdev->netdev_ops = &edma_netdev_ops;
	netdev->hw_features = NETIF_F_RXCSUM | NETIF_F_IP_CSUM |
			      NETIF_F_IPV6_CSUM | NETIF_F_SG | NETIF_F_TSO |
			      NETIF_F_TSO6 | NETIF_F_RXHASH;
	netdev->features = NETIF_F_GRO | netdev->hw_features;
	/* A DSA user port takes its features from the conduit's vlan_features. */
	netdev->vlan_features = netdev->hw_features;
	netdev->pcpu_stat_type = NETDEV_PCPU_STAT_TSTATS;
	netdev->watchdog_timeo = 5 * HZ;
	netdev->max_mtu = EDMA_MAX_MTU;
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

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 18, 0)
	ret = dev_set_threaded(netdev, NETDEV_NAPI_THREADED_ENABLED);
#else
	ret = dev_set_threaded(netdev, true);
#endif
	if (ret)
		dev_warn(dev, "failed to enable threaded NAPI: %d\n", ret);

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

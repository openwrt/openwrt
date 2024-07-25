// SPDX-License-Identifier: (GPL-2.0 OR ISC)
/* Copyright (c) 2014 - 2017, The Linux Foundation. All rights reserved.
 * Copyright (c) 2017 - 2018, John Crispin <john@phrozen.org>
 * Copyright (c) 2018 - 2019, Christian Lamparter <chunkeey@gmail.com>
 * Copyright (c) 2020 - 2021, Gabor Juhos <j4g8y7@gmail.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all copies.
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <linux/bitfield.h>
#include <linux/clk.h>
#include <linux/dsa/ipq4019.h>
#include <linux/if_vlan.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_mdio.h>
#include <linux/of_net.h>
#include <linux/phylink.h>
#include <linux/platform_device.h>
#include <linux/reset.h>
#include <linux/skbuff.h>
#include <linux/vmalloc.h>
#include <net/checksum.h>
#include <net/dsa.h>
#include <net/ip6_checksum.h>

#include "ipqess.h"

#define IPQESS_RRD_SIZE		16
#define IPQESS_NEXT_IDX(X, Y)  (((X) + 1) & ((Y) - 1))
#define IPQESS_TX_DMA_BUF_LEN	0x3fff

static void ipqess_w32(struct ipqess *ess, u32 reg, u32 val)
{
	writel(val, ess->hw_addr + reg);
}

static u32 ipqess_r32(struct ipqess *ess, u16 reg)
{
	return readl(ess->hw_addr + reg);
}

static void ipqess_m32(struct ipqess *ess, u32 mask, u32 val, u16 reg)
{
	u32 _val = ipqess_r32(ess, reg);
	_val &= ~mask;
	_val |= val;
	ipqess_w32(ess, reg, _val);
}

void ipqess_update_hw_stats(struct ipqess *ess)
{
	uint32_t *p;
	u32 stat;
	int i;

	lockdep_assert_held(&ess->stats_lock);

	p = (uint32_t *)&(ess->ipqessstats);
	for (i = 0; i < IPQESS_MAX_TX_QUEUE; i++) {
		stat = ipqess_r32(ess, IPQESS_REG_TX_STAT_PKT_Q(i));
		*p += stat;
		p++;
	}

	for (i = 0; i < IPQESS_MAX_TX_QUEUE; i++) {
		stat = ipqess_r32(ess, IPQESS_REG_TX_STAT_BYTE_Q(i));
		*p += stat;
		p++;
	}

	for (i = 0; i < IPQESS_MAX_RX_QUEUE; i++) {
		stat = ipqess_r32(ess, IPQESS_REG_RX_STAT_PKT_Q(i));
		*p += stat;
		p++;
	}

	for (i = 0; i < IPQESS_MAX_RX_QUEUE; i++) {
		stat = ipqess_r32(ess, IPQESS_REG_RX_STAT_BYTE_Q(i));
		*p += stat;
		p++;
	}
}

static int ipqess_tx_ring_alloc(struct ipqess *ess)
{
	struct device *dev = &ess->pdev->dev;
	int i;

	for (i = 0; i < IPQESS_NETDEV_QUEUES; i++) {
		struct ipqess_tx_ring *tx_ring = &ess->tx_ring[i];
		size_t size;
		u32 idx;

		tx_ring->ess = ess;
		tx_ring->ring_id = i;
		tx_ring->idx = i * 4;
		tx_ring->count = IPQESS_TX_RING_SIZE;
		tx_ring->nq = netdev_get_tx_queue(ess->netdev, i);

		size = sizeof(struct ipqess_buf) * IPQESS_TX_RING_SIZE;
		tx_ring->buf = devm_kzalloc(dev, size, GFP_KERNEL);
		if (!tx_ring->buf) {
			netdev_err(ess->netdev, "buffer alloc of tx ring failed");
			return -ENOMEM;
		}

		size = sizeof(struct ipqess_tx_desc) * IPQESS_TX_RING_SIZE;
		tx_ring->hw_desc = dmam_alloc_coherent(dev, size, &tx_ring->dma,
						       GFP_KERNEL | __GFP_ZERO);
		if (!tx_ring->hw_desc) {
			netdev_err(ess->netdev, "descriptor allocation for tx ring failed");
			return -ENOMEM;
		}

		ipqess_w32(ess, IPQESS_REG_TPD_BASE_ADDR_Q(tx_ring->idx),
			 (u32)tx_ring->dma);

		idx = ipqess_r32(ess, IPQESS_REG_TPD_IDX_Q(tx_ring->idx));
		idx >>= IPQESS_TPD_CONS_IDX_SHIFT; /* need u32 here */
		idx &= 0xffff;
		tx_ring->head = tx_ring->tail = idx;

		ipqess_m32(ess, IPQESS_TPD_PROD_IDX_MASK << IPQESS_TPD_PROD_IDX_SHIFT,
			 idx, IPQESS_REG_TPD_IDX_Q(tx_ring->idx));
		ipqess_w32(ess, IPQESS_REG_TX_SW_CONS_IDX_Q(tx_ring->idx), idx);
		ipqess_w32(ess, IPQESS_REG_TPD_RING_SIZE, IPQESS_TX_RING_SIZE);
	}

	return 0;
}

static int ipqess_tx_unmap_and_free(struct device *dev, struct ipqess_buf *buf)
{
	int len = 0;

	if (buf->flags & IPQESS_DESC_SINGLE)
		dma_unmap_single(dev, buf->dma,	buf->length, DMA_TO_DEVICE);
	else if (buf->flags & IPQESS_DESC_PAGE)
		dma_unmap_page(dev, buf->dma, buf->length, DMA_TO_DEVICE);

	if (buf->flags & IPQESS_DESC_LAST) {
		len = buf->skb->len;
		dev_kfree_skb_any(buf->skb);
	}

	buf->flags = 0;

	return len;
}

static void ipqess_tx_ring_free(struct ipqess *ess)
{
	int i;

	for (i = 0; i < IPQESS_NETDEV_QUEUES; i++) {
		int j;

		if (ess->tx_ring[i].hw_desc)
			continue;

		for (j = 0; j < IPQESS_TX_RING_SIZE; j++) {
			struct ipqess_buf *buf = &ess->tx_ring[i].buf[j];

			ipqess_tx_unmap_and_free(&ess->pdev->dev, buf);
		}

		ess->tx_ring[i].buf = NULL;
	}
}

static int ipqess_rx_buf_prepare(struct ipqess_buf *buf,
	struct ipqess_rx_ring *rx_ring)
{
	/* Clean the HW DESC header, otherwise we might end up
	 * with a spurious desc because of random garbage */
	memset(buf->skb->data, 0, sizeof(struct ipqess_rx_desc));

	buf->dma = dma_map_single(rx_ring->ppdev, buf->skb->data,
				  IPQESS_RX_HEAD_BUFF_SIZE, DMA_FROM_DEVICE);
	if (dma_mapping_error(rx_ring->ppdev, buf->dma)) {
		dev_err_once(rx_ring->ppdev,
			"IPQESS DMA mapping failed for linear address %x",
			buf->dma);
		dev_kfree_skb_any(buf->skb);
		buf->skb = NULL;
		return -EFAULT;
	}

	buf->length = IPQESS_RX_HEAD_BUFF_SIZE;
	rx_ring->hw_desc[rx_ring->head] = (struct ipqess_rx_desc *)buf->dma;
	rx_ring->head = (rx_ring->head + 1) % IPQESS_RX_RING_SIZE;

	ipqess_m32(rx_ring->ess, IPQESS_RFD_PROD_IDX_BITS,
		 (rx_ring->head + IPQESS_RX_RING_SIZE - 1) % IPQESS_RX_RING_SIZE,
		 IPQESS_REG_RFD_IDX_Q(rx_ring->idx));

	return 0;
}

/* locking is handled by the caller */
static int ipqess_rx_buf_alloc_napi(struct ipqess_rx_ring *rx_ring)
{
	struct ipqess_buf *buf = &rx_ring->buf[rx_ring->head];

	buf->skb = napi_alloc_skb(&rx_ring->napi_rx,
		IPQESS_RX_HEAD_BUFF_SIZE);
	if (!buf->skb)
		return -ENOMEM;

	return ipqess_rx_buf_prepare(buf, rx_ring);
}

static int ipqess_rx_buf_alloc(struct ipqess_rx_ring *rx_ring)
{
	struct ipqess_buf *buf = &rx_ring->buf[rx_ring->head];

	buf->skb = netdev_alloc_skb_ip_align(rx_ring->ess->netdev,
		IPQESS_RX_HEAD_BUFF_SIZE);
	if (!buf->skb)
		return -ENOMEM;

	return ipqess_rx_buf_prepare(buf, rx_ring);
}

static void ipqess_refill_work(struct work_struct *work)
{
	struct ipqess_rx_ring_refill *rx_refill = container_of(work,
		struct ipqess_rx_ring_refill, refill_work);
	struct ipqess_rx_ring *rx_ring = rx_refill->rx_ring;
	int refill = 0;

	/* don't let this loop by accident. */
	while (atomic_dec_and_test(&rx_ring->refill_count)) {
		napi_disable(&rx_ring->napi_rx);
		if (ipqess_rx_buf_alloc(rx_ring)) {
			refill++;
			dev_dbg(rx_ring->ppdev,
				"Not all buffers were reallocated");
		}
		napi_enable(&rx_ring->napi_rx);
	}

	if (atomic_add_return(refill, &rx_ring->refill_count))
		schedule_work(&rx_refill->refill_work);
}


static int ipqess_rx_ring_alloc(struct ipqess *ess)
{
	int i;

	for (i = 0; i < IPQESS_NETDEV_QUEUES; i++) {
		int j;

		ess->rx_ring[i].ess = ess;
		ess->rx_ring[i].ppdev = &ess->pdev->dev;
		ess->rx_ring[i].ring_id = i;
		ess->rx_ring[i].idx = i * 2;

		ess->rx_ring[i].buf = devm_kzalloc(&ess->pdev->dev,
			sizeof(struct ipqess_buf) * IPQESS_RX_RING_SIZE,
			GFP_KERNEL);
		if (!ess->rx_ring[i].buf)
			return -ENOMEM;

		ess->rx_ring[i].hw_desc = dmam_alloc_coherent(&ess->pdev->dev,
			sizeof(struct ipqess_rx_desc) * IPQESS_RX_RING_SIZE,
			&ess->rx_ring[i].dma, GFP_KERNEL);
		if (!ess->rx_ring[i].hw_desc)
			return -ENOMEM;

		for (j = 0; j < IPQESS_RX_RING_SIZE; j++)
			if (ipqess_rx_buf_alloc(&ess->rx_ring[i]) < 0)
				return -ENOMEM;

		ess->rx_refill[i].rx_ring = &ess->rx_ring[i];
		INIT_WORK(&ess->rx_refill[i].refill_work, ipqess_refill_work);

		ipqess_w32(ess, IPQESS_REG_RFD_BASE_ADDR_Q(ess->rx_ring[i].idx),
			 (u32)(ess->rx_ring[i].dma));
	}

	ipqess_w32(ess, IPQESS_REG_RX_DESC0,
		 (IPQESS_RX_HEAD_BUFF_SIZE << IPQESS_RX_BUF_SIZE_SHIFT) |
		 (IPQESS_RX_RING_SIZE << IPQESS_RFD_RING_SIZE_SHIFT));

	return 0;
}

static void ipqess_rx_ring_free(struct ipqess *ess)
{
	int i;

	for (i = 0; i < IPQESS_NETDEV_QUEUES; i++) {
		int j;

		atomic_set(&ess->rx_ring[i].refill_count, 0);
		cancel_work_sync(&ess->rx_refill[i].refill_work);

		for (j = 0; j < IPQESS_RX_RING_SIZE; j++) {
			dma_unmap_single(&ess->pdev->dev,
					 ess->rx_ring[i].buf[j].dma,
					 ess->rx_ring[i].buf[j].length,
					 DMA_FROM_DEVICE);
			dev_kfree_skb_any(ess->rx_ring[i].buf[j].skb);
		}
	}
}

static struct net_device_stats *ipqess_get_stats(struct net_device *netdev)
{
	struct ipqess *ess = netdev_priv(netdev);

	spin_lock(&ess->stats_lock);
	ipqess_update_hw_stats(ess);
	spin_unlock(&ess->stats_lock);

	return &ess->stats;
}

static int ipqess_rx_poll(struct ipqess_rx_ring *rx_ring, int budget)
{
	u32 length = 0, num_desc, tail, rx_ring_tail;
	int done = 0;

	rx_ring_tail = rx_ring->tail;

	tail = ipqess_r32(rx_ring->ess, IPQESS_REG_RFD_IDX_Q(rx_ring->idx));
	tail >>= IPQESS_RFD_CONS_IDX_SHIFT;
	tail &= IPQESS_RFD_CONS_IDX_MASK;

	while (done < budget) {
		struct sk_buff *skb;
		struct ipqess_rx_desc *rd;

		if (rx_ring_tail == tail)
			break;

		dma_unmap_single(rx_ring->ppdev,
				 rx_ring->buf[rx_ring_tail].dma,
				 rx_ring->buf[rx_ring_tail].length,
				 DMA_FROM_DEVICE);

		skb = xchg(&rx_ring->buf[rx_ring_tail].skb, NULL);
		rd = (struct ipqess_rx_desc *)skb->data;
		rx_ring_tail = IPQESS_NEXT_IDX(rx_ring_tail, IPQESS_RX_RING_SIZE);

		/* Check if RRD is valid */
		if (!(rd->rrd7 & IPQESS_RRD_DESC_VALID)) {
			num_desc = 1;
			dev_kfree_skb_any(skb);
			goto skip;
		}

		num_desc = rd->rrd1 & IPQESS_RRD_NUM_RFD_MASK;
		length = rd->rrd6 & IPQESS_RRD_PKT_SIZE_MASK;

		skb_reserve(skb, IPQESS_RRD_SIZE);
		if (num_desc > 1) {
			/* can we use build_skb here ? */
			struct sk_buff *skb_prev = NULL;
			int size_remaining;
			int i;

			skb->data_len = 0;
			skb->tail += (IPQESS_RX_HEAD_BUFF_SIZE - IPQESS_RRD_SIZE);
			skb->len = skb->truesize = length;
			size_remaining = length - (IPQESS_RX_HEAD_BUFF_SIZE - IPQESS_RRD_SIZE);

			for (i = 1; i < num_desc; i++) {
				/* TODO: use build_skb ? */
				struct sk_buff *skb_temp = rx_ring->buf[rx_ring_tail].skb;

				dma_unmap_single(rx_ring->ppdev,
						 rx_ring->buf[rx_ring_tail].dma,
						 rx_ring->buf[rx_ring_tail].length,
						 DMA_FROM_DEVICE);

				skb_put(skb_temp, min(size_remaining, IPQESS_RX_HEAD_BUFF_SIZE));
				if (skb_prev)
					skb_prev->next = rx_ring->buf[rx_ring_tail].skb;
				else
					skb_shinfo(skb)->frag_list = rx_ring->buf[rx_ring_tail].skb;
				skb_prev = rx_ring->buf[rx_ring_tail].skb;
				rx_ring->buf[rx_ring_tail].skb->next = NULL;

				skb->data_len += rx_ring->buf[rx_ring_tail].skb->len;
				size_remaining -= rx_ring->buf[rx_ring_tail].skb->len;

				rx_ring_tail = IPQESS_NEXT_IDX(rx_ring_tail, IPQESS_RX_RING_SIZE);
			}

		} else {
			skb_put(skb, length);
		}

		skb->dev = rx_ring->ess->netdev;
		skb->protocol = eth_type_trans(skb, rx_ring->ess->netdev);
		skb_record_rx_queue(skb, rx_ring->ring_id);

		if (rd->rrd6 & IPQESS_RRD_CSUM_FAIL_MASK)
			skb_checksum_none_assert(skb);
		else
			skb->ip_summed = CHECKSUM_UNNECESSARY;

		if (rd->rrd7 & IPQESS_RRD_CVLAN) {
			__vlan_hwaccel_put_tag(skb, htons(ETH_P_8021Q), rd->rrd4);
		} else if (rd->rrd1 & IPQESS_RRD_SVLAN) {
			__vlan_hwaccel_put_tag(skb, htons(ETH_P_8021AD), rd->rrd4);
		}
		napi_gro_receive(&rx_ring->napi_rx, skb);

		/* TODO: do we need to have these here ? */
		rx_ring->ess->stats.rx_packets++;
		rx_ring->ess->stats.rx_bytes += length;

		done++;
skip:

		num_desc += atomic_xchg(&rx_ring->refill_count, 0);
		while (num_desc) {
			if (ipqess_rx_buf_alloc_napi(rx_ring)) {
				num_desc = atomic_add_return(num_desc,
					 &rx_ring->refill_count);
				if (num_desc >= ((4 * IPQESS_RX_RING_SIZE + 6) / 7))
					schedule_work(&rx_ring->ess->rx_refill[rx_ring->ring_id].refill_work);
				break;
			}
			num_desc--;
		}
	}

	ipqess_w32(rx_ring->ess, IPQESS_REG_RX_SW_CONS_IDX_Q(rx_ring->idx),
		   rx_ring_tail);
	rx_ring->tail = rx_ring_tail;

	return done;
}

static int ipqess_tx_complete(struct ipqess_tx_ring *tx_ring, int budget)
{
	u32 tail;
	int done = 0;
	int total = 0, ret;

	tail = ipqess_r32(tx_ring->ess, IPQESS_REG_TPD_IDX_Q(tx_ring->idx));
	tail >>= IPQESS_TPD_CONS_IDX_SHIFT;
	tail &= IPQESS_TPD_CONS_IDX_MASK;

	while ((tx_ring->tail != tail) && (done < budget)) {
		//pr_info("freeing txq:%d tail:%d tailbuf:%p\n", tx_ring->idx, tx_ring->tail, &tx_ring->buf[tx_ring->tail]);
		ret = ipqess_tx_unmap_and_free(&tx_ring->ess->pdev->dev,
				       &tx_ring->buf[tx_ring->tail]);
		tx_ring->tail = IPQESS_NEXT_IDX(tx_ring->tail, tx_ring->count);
		if (ret) {
			total += ret;
			done++;
		}
	}

	ipqess_w32(tx_ring->ess,
		 IPQESS_REG_TX_SW_CONS_IDX_Q(tx_ring->idx),
		 tx_ring->tail);

	if (netif_tx_queue_stopped(tx_ring->nq)) {
		netdev_dbg(tx_ring->ess->netdev, "waking up tx queue %d\n",
			   tx_ring->idx);
		netif_tx_wake_queue(tx_ring->nq);
	}

	netdev_tx_completed_queue(tx_ring->nq, done, total);

	return done;
}

static int ipqess_tx_napi(struct napi_struct *napi, int budget)
{
	struct ipqess_tx_ring *tx_ring = container_of(napi, struct ipqess_tx_ring,
						    napi_tx);
	u32 tx_status;
	int work_done = 0;

	tx_status = ipqess_r32(tx_ring->ess, IPQESS_REG_TX_ISR);
	tx_status &= BIT(tx_ring->idx);

	work_done = ipqess_tx_complete(tx_ring, budget);

	ipqess_w32(tx_ring->ess, IPQESS_REG_TX_ISR, tx_status);

	if (likely(work_done < budget)) {
		if (napi_complete_done(napi, work_done))
			ipqess_w32(tx_ring->ess,
				   IPQESS_REG_TX_INT_MASK_Q(tx_ring->idx), 0x1);
	}

	return work_done;
}

static int ipqess_rx_napi(struct napi_struct *napi, int budget)
{
	struct ipqess_rx_ring *rx_ring = container_of(napi, struct ipqess_rx_ring,
						    napi_rx);
	struct ipqess *ess = rx_ring->ess;
	int remain_budget = budget;
	int rx_done;
	u32 rx_mask = BIT(rx_ring->idx);
	u32 status;

poll_again:
	ipqess_w32(ess, IPQESS_REG_RX_ISR, rx_mask);
	rx_done = ipqess_rx_poll(rx_ring, remain_budget);

	if (rx_done == remain_budget)
		return budget;

	status = ipqess_r32(ess, IPQESS_REG_RX_ISR);
	if (status & rx_mask) {
		remain_budget -= rx_done;
		goto poll_again;
	}

	if (napi_complete_done(napi, rx_done + budget - remain_budget))
		ipqess_w32(ess, IPQESS_REG_RX_INT_MASK_Q(rx_ring->idx), 0x1);

	return rx_done + budget - remain_budget;
}

static irqreturn_t ipqess_interrupt_tx(int irq, void *priv)
{
	struct ipqess_tx_ring *tx_ring = (struct ipqess_tx_ring *) priv;

	if (likely(napi_schedule_prep(&tx_ring->napi_tx))) {
		ipqess_w32(tx_ring->ess,
			 IPQESS_REG_TX_INT_MASK_Q(tx_ring->idx),
			 0x0);
		__napi_schedule(&tx_ring->napi_tx);
	}

	return IRQ_HANDLED;
}

static irqreturn_t ipqess_interrupt_rx(int irq, void *priv)
{
	struct ipqess_rx_ring *rx_ring = (struct ipqess_rx_ring *) priv;

	if (likely(napi_schedule_prep(&rx_ring->napi_rx))) {
		ipqess_w32(rx_ring->ess,
			 IPQESS_REG_RX_INT_MASK_Q(rx_ring->idx),
			 0x0);
		__napi_schedule(&rx_ring->napi_rx);
	}

	return IRQ_HANDLED;
}

static void ipqess_irq_enable(struct ipqess *ess)
{
	int i;

	ipqess_w32(ess, IPQESS_REG_RX_ISR, 0xff);
	ipqess_w32(ess, IPQESS_REG_TX_ISR, 0xffff);
	for (i = 0; i < IPQESS_NETDEV_QUEUES; i++) {
		ipqess_w32(ess, IPQESS_REG_RX_INT_MASK_Q(ess->rx_ring[i].idx), 1);
		ipqess_w32(ess, IPQESS_REG_TX_INT_MASK_Q(ess->tx_ring[i].idx), 1);
	}
}

static void ipqess_irq_disable(struct ipqess *ess)
{
	int i;

	for (i = 0; i < IPQESS_NETDEV_QUEUES; i++) {
		ipqess_w32(ess, IPQESS_REG_RX_INT_MASK_Q(ess->rx_ring[i].idx), 0);
		ipqess_w32(ess, IPQESS_REG_TX_INT_MASK_Q(ess->tx_ring[i].idx), 0);
	}
}

static int __init ipqess_init(struct net_device *netdev)
{
	struct ipqess *ess = netdev_priv(netdev);
	struct device_node *of_node = ess->pdev->dev.of_node;
	return phylink_of_phy_connect(ess->phylink, of_node, 0);
}

static void ipqess_uninit(struct net_device *netdev)
{
	struct ipqess *ess = netdev_priv(netdev);

	phylink_disconnect_phy(ess->phylink);
}

static int ipqess_open(struct net_device *netdev)
{
	struct ipqess *ess = netdev_priv(netdev);
	int i;

	for (i = 0; i < IPQESS_NETDEV_QUEUES; i++) {
		napi_enable(&ess->tx_ring[i].napi_tx);
		napi_enable(&ess->rx_ring[i].napi_rx);
	}
	ipqess_irq_enable(ess);
	phylink_start(ess->phylink);
	netif_tx_start_all_queues(netdev);

	return 0;
}

static int ipqess_stop(struct net_device *netdev)
{
	struct ipqess *ess = netdev_priv(netdev);
	int i;

	netif_tx_stop_all_queues(netdev);
	phylink_stop(ess->phylink);
	ipqess_irq_disable(ess);
	for (i = 0; i < IPQESS_NETDEV_QUEUES; i++) {
		napi_disable(&ess->tx_ring[i].napi_tx);
		napi_disable(&ess->rx_ring[i].napi_rx);
	}

	return 0;
}

static int ipqess_do_ioctl(struct net_device *netdev, struct ifreq *ifr, int cmd)
{
	struct ipqess *ess = netdev_priv(netdev);

	switch (cmd) {
	case SIOCGMIIPHY:
	case SIOCGMIIREG:
	case SIOCSMIIREG:
		return phylink_mii_ioctl(ess->phylink, ifr, cmd);
	default:
		break;
	}

	return -EOPNOTSUPP;
}


static inline u16 ipqess_tx_desc_available(struct ipqess_tx_ring *tx_ring)
{
	u16 count = 0;

	if (tx_ring->tail <= tx_ring->head)
		count = IPQESS_TX_RING_SIZE;

	count += tx_ring->tail - tx_ring->head - 1;

	return count;
}

static inline int ipqess_cal_txd_req(struct sk_buff *skb)
{
	int tpds;

	/* one TPD for the header, and one for each fragments */
	tpds = 1 + skb_shinfo(skb)->nr_frags;
	if (skb_is_gso(skb) && skb_is_gso_v6(skb)) {
		/* for LSOv2 one extra TPD is needed */
		tpds++;
	}

	return tpds;
}

static struct ipqess_buf *ipqess_get_tx_buffer(struct ipqess_tx_ring *tx_ring,
					       struct ipqess_tx_desc *desc)
{
	return &tx_ring->buf[desc - tx_ring->hw_desc];
}

static struct ipqess_tx_desc *ipqess_tx_desc_next(struct ipqess_tx_ring *tx_ring)
{
	struct ipqess_tx_desc *desc;

	desc = &tx_ring->hw_desc[tx_ring->head];
	tx_ring->head = IPQESS_NEXT_IDX(tx_ring->head, tx_ring->count);

	return desc;
}

static void ipqess_rollback_tx(struct ipqess *eth,
			    struct ipqess_tx_desc *first_desc, int ring_id)
{
	struct ipqess_tx_ring *tx_ring = &eth->tx_ring[ring_id];
	struct ipqess_buf *buf;
	struct ipqess_tx_desc *desc = NULL;
	u16 start_index, index;

	start_index = first_desc - tx_ring->hw_desc;

	index = start_index;
	while (index != tx_ring->head) {
		desc = &tx_ring->hw_desc[index];
		buf = &tx_ring->buf[index];
		ipqess_tx_unmap_and_free(&eth->pdev->dev, buf);
		memset(desc, 0, sizeof(struct ipqess_tx_desc));
		if (++index == tx_ring->count)
			index = 0;
	}
	tx_ring->head = start_index;
}

static bool ipqess_process_dsa_tag_sh(struct sk_buff *skb, u32 *word3)
{
	struct skb_shared_info *shinfo = skb_shinfo(skb);
	struct ipq40xx_dsa_tag_data *tag_data;

	if (shinfo->dsa_tag_proto != DSA_TAG_PROTO_IPQ4019)
		return false;

	tag_data = (struct ipq40xx_dsa_tag_data *)shinfo->dsa_tag_data;

	pr_debug("SH tag @ %08x, dp:%02x from_cpu:%u\n",
		 (u32)tag_data, tag_data->dp, tag_data->from_cpu);

	*word3 |= tag_data->dp << IPQESS_TPD_PORT_BITMAP_SHIFT;
	if (tag_data->from_cpu)
		*word3 |= BIT(IPQESS_TPD_FROM_CPU_SHIFT);

	return true;
}

static void ipqess_get_dp_info(struct ipqess *ess, struct sk_buff *skb,
			       u32 *word3)
{
	if (netdev_uses_dsa(ess->netdev)) {

		if (ipqess_process_dsa_tag_sh(skb, word3))
			return;
	}

	*word3 |= 0x3e << IPQESS_TPD_PORT_BITMAP_SHIFT;
}

static int ipqess_tx_map_and_fill(struct ipqess_tx_ring *tx_ring, struct sk_buff *skb)
{
	struct ipqess_buf *buf = NULL;
	struct platform_device *pdev = tx_ring->ess->pdev;
	struct ipqess_tx_desc *desc = NULL, *first_desc = NULL;
	u32 word1 = 0, word3 = 0, lso_word1 = 0, svlan_tag = 0;
	u16 len;
	int i;

	ipqess_get_dp_info(tx_ring->ess, skb, &word3);

	if (skb_is_gso(skb)) {
		if (skb_shinfo(skb)->gso_type & SKB_GSO_TCPV4) {
			lso_word1 |= IPQESS_TPD_IPV4_EN;
			ip_hdr(skb)->check = 0;
			tcp_hdr(skb)->check = ~csum_tcpudp_magic(ip_hdr(skb)->saddr,
				ip_hdr(skb)->daddr, 0, IPPROTO_TCP, 0);
		} else if (skb_shinfo(skb)->gso_type & SKB_GSO_TCPV6) {
			lso_word1 |= IPQESS_TPD_LSO_V2_EN;
			ipv6_hdr(skb)->payload_len = 0;
			tcp_hdr(skb)->check = ~csum_ipv6_magic(&ipv6_hdr(skb)->saddr,
				&ipv6_hdr(skb)->daddr, 0, IPPROTO_TCP, 0);
		}

		lso_word1 |= IPQESS_TPD_LSO_EN |
			     ((skb_shinfo(skb)->gso_size & IPQESS_TPD_MSS_MASK) << IPQESS_TPD_MSS_SHIFT) |
			     (skb_transport_offset(skb) << IPQESS_TPD_HDR_SHIFT);
	} else if (likely(skb->ip_summed == CHECKSUM_PARTIAL)) {
			u8 css, cso;
			cso = skb_checksum_start_offset(skb);
			css = cso + skb->csum_offset;

			word1 |= (IPQESS_TPD_CUSTOM_CSUM_EN);
			word1 |= (cso >> 1) << IPQESS_TPD_HDR_SHIFT;
			word1 |= ((css >> 1) << IPQESS_TPD_CUSTOM_CSUM_SHIFT);
	}

	if (skb_vlan_tag_present(skb)) {
		switch (skb->vlan_proto) {
		case htons(ETH_P_8021Q):
			word3 |= BIT(IPQESS_TX_INS_CVLAN);
			word3 |= skb_vlan_tag_get(skb) << IPQESS_TX_CVLAN_TAG_SHIFT;
			break;
		case htons(ETH_P_8021AD):
			word1 |= BIT(IPQESS_TX_INS_SVLAN);
			svlan_tag = skb_vlan_tag_get(skb);
			break;
		default:
			dev_err(&pdev->dev, "no ctag or stag present\n");
			goto vlan_tag_error;
		}
	}

	if (eth_type_vlan(skb->protocol))
		word1 |= IPQESS_TPD_VLAN_TAGGED;

        if (skb->protocol == htons(ETH_P_PPP_SES))
                word1 |= IPQESS_TPD_PPPOE_EN;

	len = skb_headlen(skb);

	first_desc = desc = ipqess_tx_desc_next(tx_ring);
	if (lso_word1 & IPQESS_TPD_LSO_V2_EN) {
		desc->addr = cpu_to_le16(skb->len);
		desc->word1 = word1 | lso_word1;
		desc->svlan_tag = svlan_tag;
		desc->word3 = word3;
		desc = ipqess_tx_desc_next(tx_ring);
	}

	buf = ipqess_get_tx_buffer(tx_ring, desc);
	buf->length = len;
	buf->dma = dma_map_single(&pdev->dev,
				skb->data, len, DMA_TO_DEVICE);
	if (dma_mapping_error(&pdev->dev, buf->dma))
		goto dma_error;

	desc->addr = cpu_to_le32(buf->dma);
	desc->len  = cpu_to_le16(len);

	buf->flags |= IPQESS_DESC_SINGLE;
	desc->word1 = word1 | lso_word1;
	desc->svlan_tag = svlan_tag;
	desc->word3 = word3;

	for (i = 0; i < skb_shinfo(skb)->nr_frags; i++) {
		skb_frag_t *frag = &skb_shinfo(skb)->frags[i];
		len = skb_frag_size(frag);
		desc = ipqess_tx_desc_next(tx_ring);
		buf = ipqess_get_tx_buffer(tx_ring, desc);
		buf->length = len;
		buf->flags |= IPQESS_DESC_PAGE;
		buf->dma = skb_frag_dma_map(&pdev->dev, frag, 0, len, DMA_TO_DEVICE);
		if (dma_mapping_error(&pdev->dev, buf->dma))
			goto dma_error;

		desc->addr = cpu_to_le32(buf->dma);
		desc->len  = cpu_to_le16(len);
		desc->svlan_tag = svlan_tag;
		desc->word1 = word1 | lso_word1;
		desc->word3 = word3;
	}
	desc->word1 |= 1 << IPQESS_TPD_EOP_SHIFT;
	buf->skb = skb;
	buf->flags |= IPQESS_DESC_LAST;

	return 0;

dma_error:
	ipqess_rollback_tx(tx_ring->ess, first_desc, tx_ring->ring_id);
	dev_err(&pdev->dev, "TX DMA map failed\n");

vlan_tag_error:
	return -ENOMEM;
}

static inline void ipqess_kick_tx(struct ipqess_tx_ring *tx_ring)
{
	/* Ensure that all TPDs has been written completely */
	dma_wmb();

	/* update software producer index */
	ipqess_w32(tx_ring->ess, IPQESS_REG_TPD_IDX_Q(tx_ring->idx),
		   tx_ring->head);
}

static netdev_tx_t ipqess_xmit(struct sk_buff *skb,
			     struct net_device *netdev)
{
	struct ipqess *ess = netdev_priv(netdev);
	struct ipqess_tx_ring *tx_ring;
	int avail;
	int tx_num;
	int ret;

	tx_ring = &ess->tx_ring[skb_get_queue_mapping(skb)];
	tx_num = ipqess_cal_txd_req(skb);
	avail = ipqess_tx_desc_available(tx_ring);
	if (avail < tx_num) {
		netdev_dbg(netdev,
			   "stopping tx queue %d, avail=%d req=%d im=%x\n",
			   tx_ring->idx, avail, tx_num,
			   ipqess_r32(tx_ring->ess,
				      IPQESS_REG_TX_INT_MASK_Q(tx_ring->idx)));
		netif_tx_stop_queue(tx_ring->nq);
		ipqess_w32(tx_ring->ess, IPQESS_REG_TX_INT_MASK_Q(tx_ring->idx), 0x1);
		ipqess_kick_tx(tx_ring);
		return NETDEV_TX_BUSY;
	}

	ret = ipqess_tx_map_and_fill(tx_ring, skb);
	if (ret) {
		dev_kfree_skb_any(skb);
		ess->stats.tx_errors++;
		goto err_out;
	}

	ess->stats.tx_packets++;
	ess->stats.tx_bytes += skb->len;
	netdev_tx_sent_queue(tx_ring->nq, skb->len);

	if (!netdev_xmit_more() || netif_xmit_stopped(tx_ring->nq))
		ipqess_kick_tx(tx_ring);

err_out:
	return NETDEV_TX_OK;
}

static int ipqess_set_mac_address(struct net_device *netdev, void *p)
{
	int ret = eth_mac_addr(netdev, p);
	struct ipqess *ess = netdev_priv(netdev);
	const char *macaddr = netdev->dev_addr;

	if (ret)
		return ret;

//	spin_lock_bh(&mac->hw->page_lock);
	ipqess_w32(ess, IPQESS_REG_MAC_CTRL1,
		 (macaddr[0] << 8) | macaddr[1]);
	ipqess_w32(ess, IPQESS_REG_MAC_CTRL0,
		 (macaddr[2] << 24) | (macaddr[3] << 16) |
		 (macaddr[4] << 8) | macaddr[5]);
//	spin_unlock_bh(&mac->hw->page_lock);

	return 0;
}

static void ipqess_tx_timeout(struct net_device *netdev, unsigned int txq_id)
{
	struct ipqess *ess = netdev_priv(netdev);
	struct ipqess_tx_ring *tr = &ess->tx_ring[txq_id];

	netdev_warn(netdev, "hardware queue %d is in stuck?\n",
		    tr->idx);

	/* TODO: dump hardware queue */
}

static const struct net_device_ops ipqess_axi_netdev_ops = {
	.ndo_init		= ipqess_init,
	.ndo_uninit		= ipqess_uninit,
	.ndo_open		= ipqess_open,
	.ndo_stop		= ipqess_stop,
	.ndo_eth_ioctl		= ipqess_do_ioctl,
	.ndo_start_xmit		= ipqess_xmit,
	.ndo_get_stats		= ipqess_get_stats,
	.ndo_set_mac_address	= ipqess_set_mac_address,
	.ndo_tx_timeout		= ipqess_tx_timeout,
};

static void ipqess_hw_stop(struct ipqess *ess)
{
	int i;

	/* disable all RX queue IRQs */
	for (i = 0; i < IPQESS_MAX_RX_QUEUE; i++)
		ipqess_w32(ess, IPQESS_REG_RX_INT_MASK_Q(i), 0);

	/* disable all TX queue IRQs */
	for (i = 0; i < IPQESS_MAX_TX_QUEUE; i++)
		ipqess_w32(ess, IPQESS_REG_TX_INT_MASK_Q(i), 0);

	/* disable all other IRQs */
	ipqess_w32(ess, IPQESS_REG_MISC_IMR, 0);
	ipqess_w32(ess, IPQESS_REG_WOL_IMR, 0);

	/* clear the IRQ status registers */
	ipqess_w32(ess, IPQESS_REG_RX_ISR, 0xff);
	ipqess_w32(ess, IPQESS_REG_TX_ISR, 0xffff);
	ipqess_w32(ess, IPQESS_REG_MISC_ISR, 0x1fff);
	ipqess_w32(ess, IPQESS_REG_WOL_ISR, 0x1);
	ipqess_w32(ess, IPQESS_REG_WOL_CTRL, 0);

	/* disable RX and TX queues */
	ipqess_m32(ess, IPQESS_RXQ_CTRL_EN_MASK, 0, IPQESS_REG_RXQ_CTRL);
	ipqess_m32(ess, IPQESS_TXQ_CTRL_TXQ_EN, 0, IPQESS_REG_TXQ_CTRL);
}

static int ipqess_hw_init(struct ipqess *ess)
{
	u32 tmp;
	int i, err;

	ipqess_hw_stop(ess);

	ipqess_m32(ess, BIT(IPQESS_INTR_SW_IDX_W_TYP_SHIFT),
		 IPQESS_INTR_SW_IDX_W_TYPE << IPQESS_INTR_SW_IDX_W_TYP_SHIFT,
		 IPQESS_REG_INTR_CTRL);

	/* enable IRQ delay slot */
	ipqess_w32(ess, IPQESS_REG_IRQ_MODRT_TIMER_INIT,
		 (IPQESS_TX_IMT << IPQESS_IRQ_MODRT_TX_TIMER_SHIFT) |
		 (IPQESS_RX_IMT << IPQESS_IRQ_MODRT_RX_TIMER_SHIFT));

	/* Set Customer and Service VLAN TPIDs */
	ipqess_w32(ess, IPQESS_REG_VLAN_CFG,
		   (ETH_P_8021Q << IPQESS_VLAN_CFG_CVLAN_TPID_SHIFT) |
		   (ETH_P_8021AD << IPQESS_VLAN_CFG_SVLAN_TPID_SHIFT));

	/* Configure the TX Queue bursting */
	ipqess_w32(ess, IPQESS_REG_TXQ_CTRL,
		 (IPQESS_TPD_BURST << IPQESS_TXQ_NUM_TPD_BURST_SHIFT) |
		 (IPQESS_TXF_BURST << IPQESS_TXQ_TXF_BURST_NUM_SHIFT) |
		 IPQESS_TXQ_CTRL_TPD_BURST_EN);

	/* Set RSS type */
	ipqess_w32(ess, IPQESS_REG_RSS_TYPE,
		 IPQESS_RSS_TYPE_IPV4TCP | IPQESS_RSS_TYPE_IPV6_TCP |
		 IPQESS_RSS_TYPE_IPV4_UDP | IPQESS_RSS_TYPE_IPV6UDP |
		 IPQESS_RSS_TYPE_IPV4 | IPQESS_RSS_TYPE_IPV6);

	/* Set RFD ring burst and threshold */
	ipqess_w32(ess, IPQESS_REG_RX_DESC1,
		(IPQESS_RFD_BURST << IPQESS_RXQ_RFD_BURST_NUM_SHIFT) |
		(IPQESS_RFD_THR << IPQESS_RXQ_RFD_PF_THRESH_SHIFT) |
		(IPQESS_RFD_LTHR << IPQESS_RXQ_RFD_LOW_THRESH_SHIFT));

	/* Set Rx FIFO
	 * - threshold to start to DMA data to host
	 */
	ipqess_w32(ess, IPQESS_REG_RXQ_CTRL,
		 IPQESS_FIFO_THRESH_128_BYTE | IPQESS_RXQ_CTRL_RMV_VLAN);

	err = ipqess_rx_ring_alloc(ess);
	if (err)
		return err;

	err = ipqess_tx_ring_alloc(ess);
	if (err)
		return err;

	/* Load all of ring base addresses above into the dma engine */
	ipqess_m32(ess, 0, BIT(IPQESS_LOAD_PTR_SHIFT),
		 IPQESS_REG_TX_SRAM_PART);

	/* Disable TX FIFO low watermark and high watermark */
	ipqess_w32(ess, IPQESS_REG_TXF_WATER_MARK, 0);

	/* Configure RSS indirection table.
	 * 128 hash will be configured in the following
	 * pattern: hash{0,1,2,3} = {Q0,Q2,Q4,Q6} respectively
	 * and so on
	 */
	for (i = 0; i < IPQESS_NUM_IDT; i++)
		ipqess_w32(ess, IPQESS_REG_RSS_IDT(i), IPQESS_RSS_IDT_VALUE);

	/* Configure load balance mapping table.
	 * 4 table entry will be configured according to the
	 * following pattern: load_balance{0,1,2,3} = {Q0,Q1,Q3,Q4}
	 * respectively.
	 */
	ipqess_w32(ess, IPQESS_REG_LB_RING, IPQESS_LB_REG_VALUE);

	/* Configure Virtual queue for Tx rings */
	ipqess_w32(ess, IPQESS_REG_VQ_CTRL0, IPQESS_VQ_REG_VALUE);
	ipqess_w32(ess, IPQESS_REG_VQ_CTRL1, IPQESS_VQ_REG_VALUE);

	/* Configure Max AXI Burst write size to 128 bytes*/
	ipqess_w32(ess, IPQESS_REG_AXIW_CTRL_MAXWRSIZE,
		 IPQESS_AXIW_MAXWRSIZE_VALUE);

	/* Enable TX queues */
	ipqess_m32(ess, 0, IPQESS_TXQ_CTRL_TXQ_EN, IPQESS_REG_TXQ_CTRL);

	/* Enable RX queues */
	tmp = 0;
	for (i = 0; i < IPQESS_NETDEV_QUEUES; i++)
		tmp |= IPQESS_RXQ_CTRL_EN(ess->rx_ring[i].idx);

	ipqess_m32(ess, IPQESS_RXQ_CTRL_EN_MASK, tmp, IPQESS_REG_RXQ_CTRL);

	return 0;
}

static void ipqess_validate(struct phylink_config *config,
			    unsigned long *supported,
			    struct phylink_link_state *state)
{
	struct ipqess *ess = container_of(config, struct ipqess, phylink_config);
	__ETHTOOL_DECLARE_LINK_MODE_MASK(mask) = { 0, };

	if (state->interface != PHY_INTERFACE_MODE_INTERNAL) {
		dev_err(&ess->pdev->dev, "unsupported interface mode: %d\n",
			state->interface);
		linkmode_zero(supported);
		return;
	}

	phylink_set_port_modes(mask);
	phylink_set(mask, 1000baseT_Full);
	phylink_set(mask, Pause);
	phylink_set(mask, Asym_Pause);

	linkmode_and(supported, supported, mask);
	linkmode_and(state->advertising, state->advertising, mask);
}

static void ipqess_mac_config(struct phylink_config *config, unsigned int mode,
			      const struct phylink_link_state *state)
{
	/* TODO */
}

static void ipqess_mac_link_down(struct phylink_config *config,
				 unsigned int mode,
				 phy_interface_t interface)
{
	/* TODO */
}

static void ipqess_mac_link_up(struct phylink_config *config,
			       struct phy_device *phy, unsigned int mode,
			       phy_interface_t interface,
			       int speed, int duplex,
			       bool tx_pause, bool rx_pause)
{
	/* TODO */
}

static struct phylink_mac_ops ipqess_phylink_mac_ops = {
	.validate		= ipqess_validate,
	.mac_config		= ipqess_mac_config,
	.mac_link_up		= ipqess_mac_link_up,
	.mac_link_down		= ipqess_mac_link_down,
};

static void ipqess_cleanup(struct ipqess *ess)
{
	ipqess_hw_stop(ess);
	unregister_netdev(ess->netdev);

	ipqess_tx_ring_free(ess);
	ipqess_rx_ring_free(ess);

	if (!IS_ERR_OR_NULL(ess->phylink))
		phylink_destroy(ess->phylink);
}

static void ess_reset(struct ipqess *ess)
{
	reset_control_assert(ess->ess_rst);

	mdelay(10);

	reset_control_deassert(ess->ess_rst);

	/* Waiting for all inner tables to be flushed and reinitialized.
	 * This takes between 5 and 10ms.
	 */
	mdelay(10);
}

static int ipqess_axi_probe(struct platform_device *pdev)
{
	struct device_node *np = pdev->dev.of_node;
	struct ipqess *ess;
	struct net_device *netdev;
	struct resource *res;
	int i, err = 0;

	netdev = devm_alloc_etherdev_mqs(&pdev->dev, sizeof(struct ipqess),
					 IPQESS_NETDEV_QUEUES,
					 IPQESS_NETDEV_QUEUES);
	if (!netdev)
		return -ENOMEM;

	ess = netdev_priv(netdev);
	ess->netdev = netdev;
	ess->pdev = pdev;
	spin_lock_init(&ess->stats_lock);
	SET_NETDEV_DEV(netdev, &pdev->dev);
	platform_set_drvdata(pdev, netdev);

	err = of_get_mac_address(np, netdev->dev_addr);
	if (err == -EPROBE_DEFER)
		return -EPROBE_DEFER;

	if (err) {

		random_ether_addr(netdev->dev_addr);
		dev_info(&ess->pdev->dev, "generated random MAC address %pM\n",
			netdev->dev_addr);
		netdev->addr_assign_type = NET_ADDR_RANDOM;
	}

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	ess->hw_addr = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(ess->hw_addr)) {
		err = PTR_ERR(ess->hw_addr);
		goto err_out;
	}

	ess->ess_clk = of_clk_get_by_name(np, "ess_clk");
	if (IS_ERR(ess->ess_clk)) {
		dev_err(&pdev->dev, "Failed to get ess_clk\n");
		return PTR_ERR(ess->ess_clk);
	}

	ess->ess_rst = devm_reset_control_get(&pdev->dev, "ess_rst");
	if (IS_ERR(ess->ess_rst)) {
		dev_err(&pdev->dev, "Failed to get ess_rst control!\n");
		return PTR_ERR(ess->ess_rst);
	}

	clk_prepare_enable(ess->ess_clk);

	ess_reset(ess);

	ess->phylink_config.dev = &netdev->dev;
	ess->phylink_config.type = PHYLINK_NETDEV;
	ess->phylink_config.pcs_poll = true;

	ess->phylink = phylink_create(&ess->phylink_config,
				      of_fwnode_handle(np),
				      PHY_INTERFACE_MODE_INTERNAL,
				      &ipqess_phylink_mac_ops);
	if (IS_ERR(ess->phylink)) {
		err = PTR_ERR(ess->phylink);
		goto err_out;
	}

	for (i = 0; i < IPQESS_MAX_TX_QUEUE; i++) {
		ess->tx_irq[i] = platform_get_irq(pdev, i);
		scnprintf(ess->tx_irq_names[i], sizeof(ess->tx_irq_names[i]),
			 "%s:txq%d", pdev->name, i);
	}

	for (i = 0; i < IPQESS_MAX_RX_QUEUE; i++) {
		ess->rx_irq[i] = platform_get_irq(pdev, i + IPQESS_MAX_TX_QUEUE);
		scnprintf(ess->rx_irq_names[i], sizeof(ess->rx_irq_names[i]),
			 "%s:rxq%d", pdev->name, i);
	}

#undef NETIF_F_TSO6
#define NETIF_F_TSO6 0

	netdev->netdev_ops = &ipqess_axi_netdev_ops;
	netdev->features = NETIF_F_HW_CSUM | NETIF_F_RXCSUM |
			   NETIF_F_HW_VLAN_CTAG_RX |
			   NETIF_F_HW_VLAN_CTAG_TX |
			   NETIF_F_TSO | NETIF_F_TSO6 |
			   NETIF_F_GRO | NETIF_F_SG;
	/* feature change is not supported yet */
	netdev->hw_features = 0;
	netdev->vlan_features = NETIF_F_HW_CSUM | NETIF_F_SG | NETIF_F_RXCSUM |
				NETIF_F_TSO | NETIF_F_TSO6 |
				NETIF_F_GRO;
	netdev->watchdog_timeo = 5 * HZ;
	netdev->base_addr = (u32) ess->hw_addr;
	netdev->max_mtu = 9000;
	netdev->gso_max_segs = IPQESS_TX_RING_SIZE / 2;

	ipqess_set_ethtool_ops(netdev);

	err = register_netdev(netdev);
	if (err)
		goto err_out;

	err = ipqess_hw_init(ess);
	if (err)
		goto err_out;

	dev_set_threaded(netdev, true);

	for (i = 0; i < IPQESS_NETDEV_QUEUES; i++) {
		int qid;

		netif_tx_napi_add(netdev, &ess->tx_ring[i].napi_tx,
				  ipqess_tx_napi, 64);
		netif_napi_add(netdev,
			       &ess->rx_ring[i].napi_rx,
			       ipqess_rx_napi, 64);

		qid = ess->tx_ring[i].idx;
		err = devm_request_irq(&ess->netdev->dev, ess->tx_irq[qid],
			ipqess_interrupt_tx, 0, ess->tx_irq_names[qid],
			&ess->tx_ring[i]);
		if (err)
			goto err_out;

		qid = ess->rx_ring[i].idx;
		err = devm_request_irq(&ess->netdev->dev, ess->rx_irq[qid],
			ipqess_interrupt_rx, 0, ess->rx_irq_names[qid],
			&ess->rx_ring[i]);
		if (err)
			goto err_out;
	}

	return 0;

err_out:
	ipqess_cleanup(ess);
	return err;
}

static int ipqess_axi_remove(struct platform_device *pdev)
{
	const struct net_device *netdev = platform_get_drvdata(pdev);
	struct ipqess *ess = netdev_priv(netdev);

	ipqess_cleanup(ess);

	return 0;
}

static const struct of_device_id ipqess_of_mtable[] = {
	{.compatible = "qcom,ipq4019-ess-edma" },
	{}
};
MODULE_DEVICE_TABLE(of, ipqess_of_mtable);

static struct platform_driver ipqess_axi_driver = {
	.driver = {
		.name    = "ipqess-edma",
		.of_match_table = ipqess_of_mtable,
	},
	.probe    = ipqess_axi_probe,
	.remove   = ipqess_axi_remove,
};

module_platform_driver(ipqess_axi_driver);

MODULE_AUTHOR("Qualcomm Atheros Inc");
MODULE_AUTHOR("John Crispin <john@phrozen.org>");
MODULE_AUTHOR("Christian Lamparter <chunkeey@gmail.com>");
MODULE_AUTHOR("Gabor Juhos <j4g8y7@gmail.com>");
MODULE_LICENSE("GPL");

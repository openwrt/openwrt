/*
 * Copyright (c) 2016-2018, 2020-21, The Linux Foundation. All rights reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE
 * USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <linux/version.h>
#include <linux/interrupt.h>
#include <linux/phy.h>
#include <linux/netdevice.h>
#include <linux/debugfs.h>

#include "nss_dp_dev.h"
#include "edma_regs.h"
#include "edma_data_plane.h"

/*
 * edma_alloc_rx_buffer()
 *	Alloc Rx buffers for one RxFill ring
 */
int edma_alloc_rx_buffer(struct edma_hw *ehw,
		struct edma_rxfill_ring *rxfill_ring)
{
	struct platform_device *pdev = ehw->pdev;
	struct sk_buff *skb;
	uint16_t num_alloc = 0;
	uint16_t cons, next, counter;
	struct edma_rxfill_desc *rxfill_desc;
	uint32_t reg_data = 0;
	uint32_t store_index = 0;
	struct edma_rx_preheader *rxph = NULL;

	/*
	 * Read RXFILL ring producer index
	 */
	reg_data = edma_reg_read(EDMA_REG_RXFILL_PROD_IDX(rxfill_ring->id));
	next = reg_data & EDMA_RXFILL_PROD_IDX_MASK & (rxfill_ring->count - 1);

	/*
	 * Read RXFILL ring consumer index
	 */
	reg_data = edma_reg_read(EDMA_REG_RXFILL_CONS_IDX(rxfill_ring->id));
	cons = reg_data & EDMA_RXFILL_CONS_IDX_MASK;

	while (1) {
		counter = next;
		if (++counter == rxfill_ring->count)
			counter = 0;

		if (counter == cons)
			break;

		/*
		 * Allocate buffer
		 */
		skb = dev_alloc_skb(EDMA_RX_BUFF_SIZE);
		if (unlikely(!skb))
			break;

		/*
		 * Get RXFILL descriptor
		 */
		rxfill_desc = EDMA_RXFILL_DESC(rxfill_ring, next);

		/*
		 * Make room for Rx preheader
		 */
		rxph = (struct edma_rx_preheader *)
			skb_push(skb, EDMA_RX_PREHDR_SIZE);

		/*
		 * Store the skb in the rx store
		 */
		store_index = next;
		if (ehw->rx_skb_store[store_index] != NULL) {
			dev_kfree_skb_any(skb);
			break;
		}
		ehw->rx_skb_store[store_index] = skb;
		memcpy((uint8_t *)&rxph->opaque, (uint8_t *)&store_index, 4);
		/*
		 * Save buffer size in RXFILL descriptor
		 */
		rxfill_desc->word1 = cpu_to_le32(EDMA_RX_BUFF_SIZE
					& EDMA_RXFILL_BUF_SIZE_MASK);

		/*
		 * Map Rx buffer for DMA
		 */
		rxfill_desc->buffer_addr = cpu_to_le32(dma_map_single(
						&pdev->dev,
						skb->data,
						EDMA_RX_BUFF_SIZE,
						DMA_FROM_DEVICE));

		if (!rxfill_desc->buffer_addr) {
			dev_kfree_skb_any(skb);
			ehw->rx_skb_store[store_index] = NULL;
			break;
		}

		num_alloc++;
		next = counter;
	}

	if (num_alloc) {
		/*
		 * Update RXFILL ring producer index
		 */
		reg_data = next & EDMA_RXFILL_PROD_IDX_MASK;

		/*
		 * make sure the producer index updated before
		 * updating the hardware
		 */
		wmb();

		edma_reg_write(EDMA_REG_RXFILL_PROD_IDX(rxfill_ring->id),
				reg_data);
	}

	return num_alloc;
}

/*
 * edma_clean_tx()
 *	Reap Tx descriptors
 */
uint32_t edma_clean_tx(struct edma_hw *ehw,
			struct edma_txcmpl_ring *txcmpl_ring)
{
	struct platform_device *pdev = ehw->pdev;
	struct edma_txcmpl_desc *txcmpl = NULL;
	uint16_t prod_idx = 0;
	uint16_t cons_idx = 0;
	uint32_t data = 0;
	uint32_t txcmpl_consumed = 0;
	struct sk_buff *skb;
	uint32_t len;
	int store_index;
	dma_addr_t daddr;

	/*
	 * Get TXCMPL ring producer index
	 */
	data = edma_reg_read(EDMA_REG_TXCMPL_PROD_IDX(txcmpl_ring->id));
	prod_idx = data & EDMA_TXCMPL_PROD_IDX_MASK;

	/*
	 * Get TXCMPL ring consumer index
	 */
	data = edma_reg_read(EDMA_REG_TXCMPL_CONS_IDX(txcmpl_ring->id));
	cons_idx = data & EDMA_TXCMPL_CONS_IDX_MASK;

	while (cons_idx != prod_idx) {
		txcmpl = &(((struct edma_txcmpl_desc *)
					(txcmpl_ring->desc))[cons_idx]);

		/*
		 * skb for this is stored in tx store and
		 * tx header contains the index in the field
		 * buffer address (opaque) of txcmpl
		 */
		store_index = txcmpl->buffer_addr;
		skb = ehw->tx_skb_store[store_index];
		ehw->tx_skb_store[store_index] = NULL;

		if (unlikely(!skb)) {
			pr_warn("Invalid skb: cons_idx:%u prod_idx:%u status %x\n",
				  cons_idx, prod_idx, txcmpl->status);
			goto next_txcmpl_desc;
		}

		len = skb_headlen(skb);
		daddr = (dma_addr_t)virt_to_phys(skb->data);

		pr_debug("skb:%px cons_idx:%d prod_idx:%d word1:0x%x\n",
			   skb, cons_idx, prod_idx, txcmpl->status);

		dma_unmap_single(&pdev->dev, daddr,
				 len, DMA_TO_DEVICE);
		dev_kfree_skb_any(skb);

next_txcmpl_desc:
		if (++cons_idx == txcmpl_ring->count)
			cons_idx = 0;

		txcmpl_consumed++;
	}

	if (txcmpl_consumed == 0)
		return 0;

	pr_debug("TXCMPL:%u txcmpl_consumed:%u prod_idx:%u cons_idx:%u\n",
			txcmpl_ring->id, txcmpl_consumed, prod_idx, cons_idx);

	/*
	 * Update TXCMPL ring consumer index
	 */
	wmb();
	edma_reg_write(EDMA_REG_TXCMPL_CONS_IDX(txcmpl_ring->id), cons_idx);

	return txcmpl_consumed;
}

/*
 * nss_phy_tstamp_rx_buf()
 *	Receive timestamp packet
 */
void nss_phy_tstamp_rx_buf(__attribute__((unused))void *app_data, struct sk_buff *skb)
{
	struct net_device *ndev = skb->dev;

	/*
	 * The PTP_CLASS_ value 0 is passed to phy driver, which will be
	 * set to the correct PTP class value by calling ptp_classify_raw
	 * in drv->rxtstamp function.
	 */
	if (ndev && ndev->phydev && ndev->phydev->drv &&
			ndev->phydev->drv->rxtstamp)
		if(ndev->phydev->drv->rxtstamp(ndev->phydev, skb, 0))
			return;

	netif_receive_skb(skb);
}
EXPORT_SYMBOL(nss_phy_tstamp_rx_buf);

/*
 * nss_phy_tstamp_tx_buf()
 *	Transmit timestamp packet
 */
void nss_phy_tstamp_tx_buf(struct net_device *ndev, struct sk_buff *skb)
{
	/*
	 * Function drv->txtstamp will create a clone of skb if necessary,
	 * the PTP_CLASS_ value 0 is passed to phy driver, which will be
	 * set to the correct PTP class value by calling ptp_classify_raw
	 * in the drv->txtstamp function.
	 */
	if (ndev && ndev->phydev && ndev->phydev->drv &&
			ndev->phydev->drv->txtstamp)
		ndev->phydev->drv->txtstamp(ndev->phydev, skb, 0);
}
EXPORT_SYMBOL(nss_phy_tstamp_tx_buf);

/*
 * edma_clean_rx()
 *	Reap Rx descriptors
 */
static uint32_t edma_clean_rx(struct edma_hw *ehw,
				int work_to_do,
				struct edma_rxdesc_ring *rxdesc_ring)
{
	struct platform_device *pdev = ehw->pdev;
	struct net_device *ndev;
	struct sk_buff *skb = NULL;
	struct edma_rxdesc_desc *rxdesc_desc;
	struct edma_rx_preheader *rxph = NULL;
	uint16_t prod_idx = 0;
	int src_port_num = 0;
	int pkt_length = 0;
	uint16_t cons_idx = 0;
	uint32_t work_done = 0;
	int store_index;

	/*
	 * Read Rx ring consumer index
	 */
	cons_idx = edma_reg_read(EDMA_REG_RXDESC_CONS_IDX(rxdesc_ring->id))
				& EDMA_RXDESC_CONS_IDX_MASK;

	while (1) {
		/*
		 * Read Rx ring producer index
		 */
		prod_idx = edma_reg_read(
			EDMA_REG_RXDESC_PROD_IDX(rxdesc_ring->id))
			& EDMA_RXDESC_PROD_IDX_MASK;

		if (cons_idx == prod_idx)
			break;

		if (work_done >= work_to_do)
			break;

		rxdesc_desc = EDMA_RXDESC_DESC(rxdesc_ring, cons_idx);

		/*
		 * Get Rx preheader
		 */
		rxph = (struct edma_rx_preheader *)
			phys_to_virt(rxdesc_desc->buffer_addr);

		/*
		 * DMA unmap Rx buffer
		 */
		dma_unmap_single(&pdev->dev,
				 rxdesc_desc->buffer_addr,
				 EDMA_RX_BUFF_SIZE,
				 DMA_FROM_DEVICE);

		store_index = rxph->opaque;
		skb = ehw->rx_skb_store[store_index];
		ehw->rx_skb_store[store_index] = NULL;
		if (unlikely(!skb)) {
			pr_warn("WARN: empty skb reference in rx_store:%d\n",
					cons_idx);
			goto next_rx_desc;
		}

		/*
		 * Check src_info from Rx preheader
		 */
		if (EDMA_RXPH_SRC_INFO_TYPE_GET(rxph) ==
				EDMA_PREHDR_DSTINFO_PORTID_IND) {
			src_port_num = rxph->src_info &
				EDMA_PREHDR_PORTNUM_BITS;
		} else {
			pr_warn("WARN: src_info_type:0x%x. Drop skb:%px\n",
				  EDMA_RXPH_SRC_INFO_TYPE_GET(rxph), skb);
			dev_kfree_skb_any(skb);
			goto next_rx_desc;
		}

		/*
		 * Get packet length
		 */
		pkt_length = rxdesc_desc->status & EDMA_RXDESC_PACKET_LEN_MASK;

		if (unlikely((src_port_num < NSS_DP_START_IFNUM)  ||
			(src_port_num > NSS_DP_HAL_MAX_PORTS))) {
			pr_warn("WARN: Port number error :%d. Drop skb:%px\n",
					src_port_num, skb);
			dev_kfree_skb_any(skb);
			goto next_rx_desc;
		}

		/*
		 * Get netdev for this port using the source port
		 * number as index into the netdev array. We need to
		 * subtract one since the indices start form '0' and
		 * port numbers start from '1'.
		 */
		ndev = ehw->netdev_arr[src_port_num - 1];
		if (unlikely(!ndev)) {
			pr_warn("WARN: netdev Null src_info_type:0x%x. Drop skb:%px\n",
					src_port_num, skb);
			dev_kfree_skb_any(skb);
			goto next_rx_desc;
		}

		if (unlikely(!netif_running(ndev))) {
			dev_kfree_skb_any(skb);
			goto next_rx_desc;
		}

		/*
		 * Remove Rx preheader
		 */
		skb_pull(skb, EDMA_RX_PREHDR_SIZE);

		/*
		 * Update skb fields and indicate packet to stack
		 */
		skb->dev = ndev;
		skb->skb_iif = ndev->ifindex;
		skb_put(skb, pkt_length);
		skb->protocol = eth_type_trans(skb, skb->dev);
#ifdef CONFIG_NET_SWITCHDEV
#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 5, 0))
		skb->offload_fwd_mark = ndev->offload_fwd_mark;
#else
		/*
		 * TODO: Implement ndo_get_devlink_port()
		 */
		 skb->offload_fwd_mark = 0;
#endif
		pr_debug("skb:%px ring_idx:%u pktlen:%d proto:0x%x mark:%u\n",
			   skb, cons_idx, pkt_length, skb->protocol,
			   skb->offload_fwd_mark);
#else
		pr_debug("skb:%px ring_idx:%u pktlen:%d proto:0x%x\n",
			   skb, cons_idx, pkt_length, skb->protocol);
#endif
		/*
		 * Deliver the ptp packet to phy driver for RX timestamping
		 */
		if (unlikely(EDMA_RXPH_SERVICE_CODE_GET(rxph) ==
					NSS_PTP_EVENT_SERVICE_CODE))
			nss_phy_tstamp_rx_buf(ndev, skb);
		else
			netif_receive_skb(skb);

next_rx_desc:
		/*
		 * Update consumer index
		 */
		if (++cons_idx == rxdesc_ring->count)
			cons_idx = 0;

		/*
		 * Update work done
		 */
		work_done++;
	}

	edma_alloc_rx_buffer(ehw, rxdesc_ring->rxfill);

	/*
	 * make sure the consumer index is updated
	 * before updating the hardware
	 */
	wmb();
	edma_reg_write(EDMA_REG_RXDESC_CONS_IDX(rxdesc_ring->id), cons_idx);
	return work_done;
}

/*
 * edma_napi()
 *	EDMA NAPI handler
 */
int edma_napi(struct napi_struct *napi, int budget)
{
	struct edma_hw *ehw = container_of(napi, struct edma_hw, napi);
	struct edma_txcmpl_ring *txcmpl_ring = NULL;
	struct edma_rxdesc_ring *rxdesc_ring = NULL;
	struct edma_rxfill_ring *rxfill_ring = NULL;

	struct net_device *ndev;
	int work_done = 0;
	int i;

	for (i = 0; i < ehw->rxdesc_rings; i++) {
		rxdesc_ring = &ehw->rxdesc_ring[i];
		work_done += edma_clean_rx(ehw, budget, rxdesc_ring);
	}

	for (i = 0; i < ehw->txcmpl_rings; i++) {
		txcmpl_ring = &ehw->txcmpl_ring[i];
		work_done += edma_clean_tx(ehw, txcmpl_ring);
	}

	for (i = 0; i < ehw->rxfill_rings; i++) {
		rxfill_ring = &ehw->rxfill_ring[i];
		work_done += edma_alloc_rx_buffer(ehw, rxfill_ring);
	}

	/*
	 * Resume netdev Tx queue
	 */
	/*
	 * TODO works currently since we have a single queue.
	 * Need to make sure we have support in place when there is
	 * support for multiple queues
	 */
	for (i = 0; i < EDMA_MAX_GMACS; i++) {
		ndev = ehw->netdev_arr[i];
		if (!ndev)
			continue;

		if (netif_queue_stopped(ndev) && netif_carrier_ok(ndev))
			netif_start_queue(ndev);
	}

	/*
	 * TODO - rework and fix the budget control
	 */
	if (work_done < budget) {
		/*
		 * TODO per core NAPI
		 */
		napi_complete(napi);

		/*
		 * Set RXDESC ring interrupt mask
		 */
		for (i = 0; i < ehw->rxdesc_rings; i++) {
			rxdesc_ring = &ehw->rxdesc_ring[i];
			edma_reg_write(
				EDMA_REG_RXDESC_INT_MASK(rxdesc_ring->id),
				ehw->rxdesc_intr_mask);
		}

		/*
		 * Set TXCMPL ring interrupt mask
		 */
		for (i = 0; i < ehw->txcmpl_rings; i++) {
			txcmpl_ring = &ehw->txcmpl_ring[i];
			edma_reg_write(EDMA_REG_TX_INT_MASK(txcmpl_ring->id),
					ehw->txcmpl_intr_mask);
		}

		/*
		 * Set RXFILL ring interrupt mask
		 */
		for (i = 0; i < ehw->rxfill_rings; i++) {
			rxfill_ring = &ehw->rxfill_ring[i];
			edma_reg_write(EDMA_REG_RXFILL_INT_MASK(
						rxfill_ring->id),
						edma_hw.rxfill_intr_mask);
		}
	}
	return work_done;
}

/*
 * edma_ring_xmit()
 *	Transmit a packet using an EDMA ring
 */
enum edma_tx edma_ring_xmit(struct edma_hw *ehw,
			    struct net_device *netdev,
			    struct sk_buff *skb,
			    struct edma_txdesc_ring *txdesc_ring)
{
	struct nss_dp_dev *dp_dev = netdev_priv(netdev);
	struct edma_txdesc_desc *txdesc = NULL;
	uint16_t buf_len;
	uint16_t hw_next_to_use, hw_next_to_clean, chk_idx;
	uint32_t data;
	uint32_t store_index = 0;
	struct edma_tx_preheader *txph = NULL;

	/*
	 * TODO - revisit locking
	 */
	spin_lock_bh(&txdesc_ring->tx_lock);

	/*
	 * Read TXDESC ring producer index
	 */
	data = edma_reg_read(EDMA_REG_TXDESC_PROD_IDX(txdesc_ring->id));
	hw_next_to_use = data & EDMA_TXDESC_PROD_IDX_MASK;

	/*
	 * Read TXDESC ring consumer index
	 */
	/*
	 * TODO - read to local variable to optimize uncached access
	 */
	data = edma_reg_read(EDMA_REG_TXDESC_CONS_IDX(txdesc_ring->id));
	hw_next_to_clean = data & EDMA_TXDESC_CONS_IDX_MASK;

	/*
	 * Check for available Tx descriptor
	 */
	chk_idx = (hw_next_to_use + 1) & (txdesc_ring->count-1);

	if (chk_idx == hw_next_to_clean) {
		spin_unlock_bh(&txdesc_ring->tx_lock);
		return EDMA_TX_DESC;
	}

#if defined(NSS_DP_EDMA_TX_SMALL_PKT_WAR)
	/*
	 * IPQ807x EDMA hardware can't process the packet if the packet size is
	 * less than EDMA_TX_PKT_MIN_SIZE (33 Byte). So, if the packet size
	 * is indeed less than EDMA_TX_PKT_MIN_SIZE, perform padding
	 * (if possible), otherwise drop the packet.
	 * Using skb_padto() API for padding the packet. This API will drop
	 * the packet if the padding is not possible.
	 */
	if (unlikely(skb->len < EDMA_TX_PKT_MIN_SIZE)) {
		if (skb_padto(skb, EDMA_TX_PKT_MIN_SIZE)) {
			netdev_dbg(netdev, "padding couldn't happen, skb is freed.\n");
			netdev->stats.tx_dropped++;
			spin_unlock_bh(&txdesc_ring->tx_lock);
			return EDMA_TX_OK;
		}
		skb->len = EDMA_TX_PKT_MIN_SIZE;
	}
#endif

	buf_len = skb_headlen(skb);

	/*
	 * Deliver the ptp packet to phy driver for TX timestamping
	 */
	if (unlikely(skb_shinfo(skb)->tx_flags & SKBTX_HW_TSTAMP))
		nss_phy_tstamp_tx_buf(netdev, skb);

	/*
	 * Make room for Tx preheader
	 */
	txph = (struct edma_tx_preheader *)skb_push(skb,
			EDMA_TX_PREHDR_SIZE);
	memset((void *)txph, 0, EDMA_TX_PREHDR_SIZE);

	/*
	 * Populate Tx preheader dst info, port id is macid in dp_dev
	 */
	txph->dst_info = (EDMA_PREHDR_DSTINFO_PORTID_IND << 8) |
			(dp_dev->macid & 0x0fff);

	/*
	 * Store the skb in tx_store
	 */
	store_index = hw_next_to_use & (txdesc_ring->count - 1);
	if (unlikely(ehw->tx_skb_store[store_index] != NULL)) {
		spin_unlock_bh(&txdesc_ring->tx_lock);
		return EDMA_TX_DESC;
	}

	ehw->tx_skb_store[store_index] = skb;
	memcpy(skb->data, &store_index, 4);

	/*
	 * Get Tx descriptor
	 */
	txdesc = EDMA_TXDESC_DESC(txdesc_ring, hw_next_to_use);
	memset(txdesc, 0, sizeof(struct edma_txdesc_desc));

	/*
	 * Map buffer to DMA address
	 */
	txdesc->buffer_addr = cpu_to_le32(dma_map_single(&(ehw->pdev)->dev,
				       skb->data,
				       buf_len + EDMA_TX_PREHDR_SIZE,
				       DMA_TO_DEVICE));
	if (!txdesc->buffer_addr) {
		/*
		 * DMA map failed for this address. Drop it
		 * and make sure does not got to stack again
		 */
		dev_kfree_skb_any(skb);

		ehw->tx_skb_store[store_index] = NULL;
		spin_unlock_bh(&txdesc_ring->tx_lock);
		return EDMA_TX_OK;
	}

	/*
	 * Populate Tx descriptor
	 */
	txdesc->word1 |= (1 << EDMA_TXDESC_PREHEADER_SHIFT)
			| ((EDMA_TX_PREHDR_SIZE & EDMA_TXDESC_DATA_OFFSET_MASK)
			   << EDMA_TXDESC_DATA_OFFSET_SHIFT);
	txdesc->word1 |= ((buf_len & EDMA_TXDESC_DATA_LENGTH_MASK)
			<< EDMA_TXDESC_DATA_LENGTH_SHIFT);

	netdev_dbg(netdev, "skb:%px tx_ring:%u proto:0x%x\n",
			skb, txdesc_ring->id, ntohs(skb->protocol));
	netdev_dbg(netdev, "port:%u prod_idx:%u cons_idx:%u\n",
			dp_dev->macid, hw_next_to_use, hw_next_to_clean);

	/*
	 * Update producer index
	 */
	hw_next_to_use = (hw_next_to_use + 1) & (txdesc_ring->count - 1);

	/*
	 * make sure the hw_next_to_use is updated before the
	 * write to hardware
	 */
	wmb();

	edma_reg_write(EDMA_REG_TXDESC_PROD_IDX(txdesc_ring->id),
		       hw_next_to_use & EDMA_TXDESC_PROD_IDX_MASK);
	spin_unlock_bh(&txdesc_ring->tx_lock);
	return EDMA_TX_OK;
}

/*
 * edma_handle_misc_irq()
 *	Process IRQ
 */
irqreturn_t edma_handle_misc_irq(int irq, void *ctx)
{
	uint32_t misc_intr_status = 0;
	uint32_t reg_data = 0;
	struct edma_hw *ehw = NULL;
	struct platform_device *pdev = (struct platform_device *)ctx;

	ehw = platform_get_drvdata(pdev);

	/*
	 * Read Misc intr status
	 */
	reg_data = edma_reg_read(EDMA_REG_MISC_INT_STAT);
	misc_intr_status = reg_data & ehw->misc_intr_mask;

	/*
	 * TODO - error logging
	 */
	if (misc_intr_status == 0)
		return IRQ_NONE;
	else
		edma_reg_write(EDMA_REG_MISC_INT_MASK, EDMA_MASK_INT_DISABLE);

	return IRQ_HANDLED;
}

/*
 * edma_handle_irq()
 *	Process IRQ and schedule napi
 */
irqreturn_t edma_handle_irq(int irq, void *ctx)
{
	uint32_t reg_data = 0;
	uint32_t rxdesc_intr_status = 0;
	uint32_t txcmpl_intr_status = 0;
	uint32_t rxfill_intr_status = 0;
	int i;
	struct edma_txcmpl_ring *txcmpl_ring = NULL;
	struct edma_rxdesc_ring *rxdesc_ring = NULL;
	struct edma_rxfill_ring *rxfill_ring = NULL;
	struct edma_hw *ehw = NULL;
	struct platform_device *pdev = (struct platform_device *)ctx;

	ehw = platform_get_drvdata(pdev);
	if (!ehw) {
		pr_info("Unable to retrieve platrofm data");
		return IRQ_HANDLED;
	}

	/*
	 * Read RxDesc intr status
	 */
	for (i = 0; i < ehw->rxdesc_rings; i++) {
		rxdesc_ring = &ehw->rxdesc_ring[i];
		reg_data = edma_reg_read(
				EDMA_REG_RXDESC_INT_STAT(rxdesc_ring->id));
		rxdesc_intr_status |= reg_data &
				EDMA_RXDESC_RING_INT_STATUS_MASK;

		/*
		 * Disable RxDesc intr
		 */
		edma_reg_write(EDMA_REG_RXDESC_INT_MASK(rxdesc_ring->id),
			       EDMA_MASK_INT_DISABLE);
	}

	/*
	 * Read TxCmpl intr status
	 */
	for (i = 0; i < ehw->txcmpl_rings; i++) {
		txcmpl_ring = &ehw->txcmpl_ring[i];
		reg_data = edma_reg_read(
				EDMA_REG_TX_INT_STAT(txcmpl_ring->id));
		txcmpl_intr_status |= reg_data &
				EDMA_TXCMPL_RING_INT_STATUS_MASK;

		/*
		 * Disable TxCmpl intr
		 */
		edma_reg_write(EDMA_REG_TX_INT_MASK(txcmpl_ring->id),
				EDMA_MASK_INT_DISABLE);
	}

	/*
	 * Read RxFill intr status
	 */
	for (i = 0; i < ehw->rxfill_rings; i++) {
		rxfill_ring = &ehw->rxfill_ring[i];
		reg_data = edma_reg_read(
				EDMA_REG_RXFILL_INT_STAT(rxfill_ring->id));
		rxfill_intr_status |= reg_data &
				EDMA_RXFILL_RING_INT_STATUS_MASK;

		/*
		 * Disable RxFill intr
		 */
		edma_reg_write(EDMA_REG_RXFILL_INT_MASK(rxfill_ring->id),
			       EDMA_MASK_INT_DISABLE);

	}

	if ((rxdesc_intr_status == 0) && (txcmpl_intr_status == 0) &&
			(rxfill_intr_status == 0))
		return IRQ_NONE;

	for (i = 0; i < ehw->rxdesc_rings; i++) {
		rxdesc_ring = &ehw->rxdesc_ring[i];
		edma_reg_write(EDMA_REG_RXDESC_INT_MASK(rxdesc_ring->id),
			       EDMA_MASK_INT_DISABLE);
	}

	/*
	 *TODO - per core NAPI
	 */
	if (rxdesc_intr_status || txcmpl_intr_status || rxfill_intr_status)
		if (likely(napi_schedule_prep(&ehw->napi)))
			__napi_schedule(&ehw->napi);

	return IRQ_HANDLED;
}

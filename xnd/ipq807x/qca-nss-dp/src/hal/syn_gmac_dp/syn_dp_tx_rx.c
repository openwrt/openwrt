/*
 * Copyright (c) 2020, The Linux Foundation. All rights reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <linux/module.h>
#include <linux/kernel.h>

#include <linux/interrupt.h>
#include <linux/netdevice.h>
#include <linux/debugfs.h>

#include "syn_data_plane.h"
#include "syn_reg.h"

/*
 * syn_dp_reset_rx_qptr
 *	Reset the descriptor after Rx is over.
 */
static inline void syn_dp_reset_rx_qptr(struct nss_dp_dev *gmac_dev, struct syn_dp_info *dev_info)
{

	/* Index of descriptor the DMA just completed.
	 * May be useful when data is spread over multiple buffers/descriptors
	 */
	uint32_t rxnext = dev_info->rx_busy;
	struct dma_desc *rxdesc = dev_info->rx_busy_desc;

	BUG_ON(rxdesc != (dev_info->rx_desc + rxnext));
	dev_info->rx_busy = (rxnext + 1) & (dev_info->rx_desc_count - 1);
	dev_info->rx_busy_desc = dev_info->rx_desc + dev_info->rx_busy;

	dev_info->rx_skb_list[rxnext] = NULL;
	rxdesc->status = 0;
	rxdesc->length &= desc_rx_desc_end_of_ring;
	rxdesc->buffer1 = 0;
	rxdesc->data1 = 0;
	rxdesc->reserved1 = 0;

	/*
	 * This returns one descriptor to processor. So busy count will be decremented by one.
	 */
	dev_info->busy_rx_desc--;
}

/*
 * syn_dp_set_rx_qptr
 *	Prepares the descriptor to receive packets.
 */
static inline int32_t syn_dp_set_rx_qptr(struct nss_dp_dev *gmac_dev, struct syn_dp_info *dev_info,
					uint32_t Buffer1, uint32_t Length1, struct sk_buff *skb)
{
	uint32_t rxnext = dev_info->rx_next;
	struct dma_desc *rxdesc = dev_info->rx_next_desc;
	uint32_t rx_skb_index = rxnext;

	BUG_ON(dev_info->busy_rx_desc >= dev_info->rx_desc_count);
	BUG_ON(rxdesc != (dev_info->rx_desc + rxnext));
	BUG_ON(!syn_dp_gmac_is_desc_empty(rxdesc));
	BUG_ON(syn_dp_gmac_is_desc_owned_by_dma(rxdesc));

	if (Length1 > SYN_DP_MAX_DESC_BUFF) {
		rxdesc->length |= (SYN_DP_MAX_DESC_BUFF << desc_size1_shift) & desc_size1_mask;
		rxdesc->length |= ((Length1 - SYN_DP_MAX_DESC_BUFF) << desc_size2_shift) & desc_size2_mask;
	} else {
		rxdesc->length |= ((Length1 << desc_size1_shift) & desc_size1_mask);
	}

	rxdesc->buffer1 = Buffer1;
	dev_info->rx_skb_list[rx_skb_index] = skb;

	/* Program second buffer address if using two buffers. */
	if (Length1 > SYN_DP_MAX_DESC_BUFF)
		rxdesc->data1 = Buffer1 + SYN_DP_MAX_DESC_BUFF;
	else
		rxdesc->data1 = 0;

	rxdesc->extstatus = 0;
	rxdesc->timestamplow = 0;
	rxdesc->timestamphigh = 0;

	/*
	 * Ensure all write completed before setting own by dma bit so when gmac
	 * HW takeover this descriptor, all the fields are filled correctly
	 */
	wmb();
	rxdesc->status = desc_own_by_dma;

	dev_info->rx_next = (rxnext + 1) & (dev_info->rx_desc_count - 1);
	dev_info->rx_next_desc = dev_info->rx_desc + dev_info->rx_next;

	/*
	 * 1 descriptor will be given to HW. So busy count incremented by 1.
	 */
	dev_info->busy_rx_desc++;

	return rxnext;
}

/*
 * syn_dp_rx_refill
 *	Refill the RX descrptor
 */
void syn_dp_rx_refill(struct nss_dp_dev *gmac_dev, struct syn_dp_info *dev_info)
{
	struct net_device *netdev = gmac_dev->netdev;
	struct device *dev = &gmac_dev->pdev->dev;
	int empty_count = SYN_DP_RX_DESC_SIZE - dev_info->busy_rx_desc;

	dma_addr_t dma_addr;
	int i;
	struct sk_buff *skb;

	for (i = 0; i < empty_count; i++) {
		skb = __netdev_alloc_skb(netdev, SYN_DP_MINI_JUMBO_FRAME_MTU, GFP_ATOMIC);
		if (unlikely(skb == NULL)) {
			netdev_dbg(netdev, "Unable to allocate skb, will try next time\n");
			break;
		}

		skb_reserve(skb, NET_IP_ALIGN);

		dma_addr = dma_map_single(dev, skb->data, SYN_DP_MINI_JUMBO_FRAME_MTU, DMA_FROM_DEVICE);
		if (unlikely(dma_mapping_error(dev, dma_addr))) {
			dev_kfree_skb(skb);
			netdev_dbg(netdev, "DMA mapping failed for empty buffer\n");
			break;
		}

		syn_dp_set_rx_qptr(gmac_dev, dev_info, dma_addr, SYN_DP_MINI_JUMBO_FRAME_MTU, skb);
	}
}

/*
 * syn_dp_rx()
 *	Process RX packets
 */
int syn_dp_rx(struct nss_dp_dev *gmac_dev, struct syn_dp_info *dev_info, int budget)
{
	struct dma_desc *desc = NULL;
	int frame_length, busy;
	uint32_t status;
	struct sk_buff *rx_skb;
	uint32_t rx_skb_index;

	if (!dev_info->busy_rx_desc) {
		/* no desc are held by gmac dma, we are done */
		return 0;
	}

	busy = dev_info->busy_rx_desc;
	if (busy > budget)
		busy = budget;

	do {
		desc = dev_info->rx_busy_desc;
		if (syn_dp_gmac_is_desc_owned_by_dma(desc)) {
			/* desc still hold by gmac dma, so we are done */
			break;
		}

		status = desc->status;

		rx_skb_index = dev_info->rx_busy;
		rx_skb = dev_info->rx_skb_list[rx_skb_index];

		dma_unmap_single(&(gmac_dev->netdev->dev), desc->buffer1,
				SYN_DP_MINI_JUMBO_FRAME_MTU, DMA_FROM_DEVICE);

		spin_lock_bh(&dev_info->stats_lock);
		if (likely(syn_dp_gmac_is_rx_desc_valid(status))) {
			/* We have a pkt to process get the frame length */
			frame_length = syn_dp_gmac_get_rx_desc_frame_length(status);
			/* Get rid of FCS: 4 */
			frame_length -= ETH_FCS_LEN;

			/* Valid packet, collect stats */
			dev_info->stats.stats.rx_packets++;
			dev_info->stats.stats.rx_bytes += frame_length;

			/* type_trans and deliver to linux */
			skb_put(rx_skb, frame_length);
			rx_skb->protocol = eth_type_trans(rx_skb, gmac_dev->netdev);
			rx_skb->ip_summed = CHECKSUM_UNNECESSARY;
			napi_gro_receive(&gmac_dev->napi, rx_skb);

		} else {
			dev_info->stats.stats.rx_errors++;
			dev_kfree_skb(rx_skb);

			if (status & (desc_rx_crc | desc_rx_collision |
					desc_rx_overflow | desc_rx_dribbling |
					desc_rx_length_error)) {
				dev_info->stats.stats.mmc_rx_crc_errors += (status & desc_rx_crc) ? 1 : 0;
				dev_info->stats.stats.rx_late_collision_errors += (status & desc_rx_collision) ? 1 : 0;
				dev_info->stats.stats.mmc_rx_overflow_errors += (status & desc_rx_overflow) ? 1 : 0;
				dev_info->stats.stats.rx_dribble_bit_errors += (status & desc_rx_dribbling) ? 1 : 0;
				dev_info->stats.stats.rx_length_errors += (status & desc_rx_length_error) ? 1 : 0;
			}
		}

		spin_unlock_bh(&dev_info->stats_lock);

		syn_dp_reset_rx_qptr(gmac_dev, dev_info);
		busy--;
	} while (busy > 0);
	return budget - busy;
}

/*
 * syn_dp_reset_tx_qptr
 *	Reset the descriptor after Tx is over.
 */
static inline void syn_dp_reset_tx_qptr(struct nss_dp_dev *gmac_dev, struct syn_dp_info *dev_info)
{
	uint32_t txover = dev_info->tx_busy;
	struct dma_desc *txdesc = dev_info->tx_busy_desc;

	BUG_ON(txdesc != (dev_info->tx_desc + txover));
	dev_info->tx_busy = (txover + 1) & (dev_info->tx_desc_count - 1);
	dev_info->tx_busy_desc = dev_info->tx_desc + dev_info->tx_busy;

	dev_info->tx_skb_list[txover] = NULL;
	txdesc->status &= desc_tx_desc_end_of_ring;
	txdesc->length = 0;
	txdesc->buffer1 = 0;
	txdesc->data1 = 0;
	txdesc->reserved1 = 0;

	/*
	 * Busy tx descriptor is reduced by one as
	 * it will be handed over to Processor now.
	 */
	dev_info->busy_tx_desc--;
}

/*
 * syn_dp_set_tx_qptr
 *	Populate the tx desc structure with the buffer address.
 */
static inline struct dma_desc *syn_dp_set_tx_qptr(struct nss_dp_dev *gmac_dev, struct syn_dp_info *dev_info,
					   uint32_t Buffer1, uint32_t Length1, struct sk_buff *skb, uint32_t offload_needed,
					   uint32_t tx_cntl, uint32_t set_dma)
{
	uint32_t txnext = dev_info->tx_next;
	struct dma_desc *txdesc = dev_info->tx_next_desc;
	uint32_t tx_skb_index = txnext;

	BUG_ON(dev_info->busy_tx_desc > dev_info->tx_desc_count);
	BUG_ON(txdesc != (dev_info->tx_desc + txnext));
	BUG_ON(!syn_dp_gmac_is_desc_empty(txdesc));
	BUG_ON(syn_dp_gmac_is_desc_owned_by_dma(txdesc));

	if (Length1 > SYN_DP_MAX_DESC_BUFF) {
		txdesc->length |= (SYN_DP_MAX_DESC_BUFF << desc_size1_shift) & desc_size1_mask;
		txdesc->length |=
		    ((Length1 - SYN_DP_MAX_DESC_BUFF) << desc_size2_shift) & desc_size2_mask;
	} else {
		txdesc->length |= ((Length1 << desc_size1_shift) & desc_size1_mask);
	}

	txdesc->status |= tx_cntl;
	txdesc->buffer1 = Buffer1;

	dev_info->tx_skb_list[tx_skb_index] = skb;

	/* Program second buffer address if using two buffers. */
	if (Length1 > SYN_DP_MAX_DESC_BUFF)
		txdesc->data1 = Buffer1 + SYN_DP_MAX_DESC_BUFF;
	else
		txdesc->data1 = 0;

	if (likely(offload_needed)) {
		syn_dp_gmac_tx_checksum_offload_tcp_pseudo(txdesc);
	}

	/*
	 * Ensure all write completed before setting own by dma bit so when gmac
	 * HW takeover this descriptor, all the fields are filled correctly
	 */
	wmb();
	txdesc->status |= set_dma;

	dev_info->tx_next = (txnext + 1) & (dev_info->tx_desc_count - 1);
	dev_info->tx_next_desc = dev_info->tx_desc + dev_info->tx_next;

	return txdesc;
}

/*
 * syn_dp_tx_queue_desc
 *	Queue TX descriptor to the TX ring
 */
static void syn_dp_tx_desc_queue(struct nss_dp_dev *gmac_dev, struct syn_dp_info *dev_info, struct sk_buff *skb, dma_addr_t dma_addr)
{
	unsigned int len = skb->len;

	spin_lock_bh(&dev_info->data_lock);

	syn_dp_set_tx_qptr(gmac_dev, dev_info, dma_addr, len, skb, (skb->ip_summed == CHECKSUM_PARTIAL),
				(desc_tx_last | desc_tx_first | desc_tx_int_enable), desc_own_by_dma);
	dev_info->busy_tx_desc++;

	spin_unlock_bh(&dev_info->data_lock);
}

/*
 * syn_dp_process_tx_complete
 *	Xmit complete, clear descriptor and free the skb
 */
void syn_dp_process_tx_complete(struct nss_dp_dev *gmac_dev, struct syn_dp_info *dev_info)
{
	int busy, len;
	uint32_t status;
	struct dma_desc *desc = NULL;
	struct sk_buff *skb;
	uint32_t tx_skb_index;

	spin_lock_bh(&dev_info->data_lock);
	busy = dev_info->busy_tx_desc;

	if (!busy) {
		/* No desc are hold by gmac dma, we are done */
		spin_unlock_bh(&dev_info->data_lock);
		return;
	}

	do {
		desc = dev_info->tx_busy_desc;
		if (syn_dp_gmac_is_desc_owned_by_dma(desc)) {
			/* desc still hold by gmac dma, so we are done */
			break;
		}

		len = (desc->length & desc_size1_mask) >> desc_size1_shift;
		dma_unmap_single(&(gmac_dev->pdev->dev), desc->buffer1, len, DMA_TO_DEVICE);

		status = desc->status;
		if (status & desc_tx_last) {
			/* TX is done for this whole skb, we can free it */
			/* Get the skb from the tx skb pool */
			tx_skb_index = dev_info->tx_busy;
			skb = dev_info->tx_skb_list[tx_skb_index];

			BUG_ON(!skb);
			dev_kfree_skb(skb);

			spin_lock_bh(&dev_info->stats_lock);

			if (unlikely(status & desc_tx_error)) {
				/* Some error happen, collect statistics */
				dev_info->stats.stats.tx_errors++;
				dev_info->stats.stats.tx_jabber_timeout_errors += (status & desc_tx_timeout) ? 1 : 0;
				dev_info->stats.stats.tx_frame_flushed_errors += (status & desc_tx_frame_flushed) ? 1 : 0;
				dev_info->stats.stats.tx_loss_of_carrier_errors += (status & desc_tx_lost_carrier) ? 1 : 0;
				dev_info->stats.stats.tx_no_carrier_errors += (status & desc_tx_no_carrier) ? 1 : 0;
				dev_info->stats.stats.tx_late_collision_errors += (status & desc_tx_late_collision) ? 1 : 0;
				dev_info->stats.stats.tx_excessive_collision_errors += (status & desc_tx_exc_collisions) ? 1 : 0;
				dev_info->stats.stats.tx_excessive_deferral_errors += (status & desc_tx_exc_deferral) ? 1 : 0;
				dev_info->stats.stats.tx_underflow_errors += (status & desc_tx_underflow) ? 1 : 0;
				dev_info->stats.stats.tx_ip_header_errors += (status & desc_tx_ipv4_chk_error) ? 1 : 0;
				dev_info->stats.stats.tx_ip_payload_errors += (status & desc_tx_pay_chk_error) ? 1 : 0;
			} else {
				/* No error, recored tx pkts/bytes and
				 * collision
				 */
				dev_info->stats.stats.tx_packets++;
				dev_info->stats.stats.tx_collisions += syn_dp_gmac_get_tx_collision_count(status);
				dev_info->stats.stats.tx_bytes += len;
			}

			spin_unlock_bh(&dev_info->stats_lock);
		}
		syn_dp_reset_tx_qptr(gmac_dev, dev_info);
		busy--;
	} while (busy > 0);

	spin_unlock_bh(&dev_info->data_lock);
}

/*
 * syn_dp_tx
 *	TX routine for Synopsys GMAC
 */
int syn_dp_tx(struct nss_dp_dev *gmac_dev, struct syn_dp_info *dev_info, struct sk_buff *skb)
{
	struct net_device *netdev = gmac_dev->netdev;
	struct nss_gmac_hal_dev *nghd = gmac_dev->gmac_hal_ctx;
	unsigned len = skb->len;
	dma_addr_t dma_addr;

	/*
	 * If we don't have enough tx descriptor for this pkt, return busy.
	 */
	if ((SYN_DP_TX_DESC_SIZE - dev_info->busy_tx_desc) < 1) {
		netdev_dbg(netdev, "Not enough descriptors available");
		return -1;
	}

	dma_addr = dma_map_single(&gmac_dev->pdev->dev, skb->data, len, DMA_TO_DEVICE);
	if (unlikely(dma_mapping_error(&gmac_dev->pdev->dev, dma_addr))) {
		netdev_dbg(netdev, "DMA mapping failed for empty buffer\n");
		return -1;
	}

	/*
	 * Queue packet to the GMAC rings
	 */
	syn_dp_tx_desc_queue(gmac_dev, dev_info, skb, dma_addr);

	syn_resume_dma_tx(nghd);

	return 0;
}
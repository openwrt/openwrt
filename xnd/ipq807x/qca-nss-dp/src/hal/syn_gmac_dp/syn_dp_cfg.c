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
#include "nss_dp_dev.h"
#include "syn_data_plane.h"
#include "syn_reg.h"

/*
 * syn_dp_setup_rx_desc_queue
 *	This sets up the receive Descriptor queue in ring or chain mode.
 */
static int syn_dp_setup_rx_desc_queue(struct net_device *netdev, struct device *dev, struct syn_dp_info *dev_info,
					uint32_t no_of_desc, uint32_t desc_mode)
{
	struct dma_desc *first_desc = NULL;
	dma_addr_t dma_addr;

	dev_info->rx_desc_count = 0;

	BUG_ON(desc_mode != RINGMODE);
	BUG_ON((no_of_desc & (no_of_desc - 1)) != 0);

	netdev_dbg(netdev, "total size of memory required for Rx Descriptors in Ring Mode = %u\n", (uint32_t)((sizeof(struct dma_desc) * no_of_desc)));

	first_desc = dma_alloc_coherent(dev, sizeof(struct dma_desc) * no_of_desc, &dma_addr, GFP_KERNEL);
	if (first_desc == NULL) {
		netdev_dbg(netdev, "Error in Rx Descriptor Memory allocation in Ring mode\n");
		return -ENOMEM;
	}

	dev_info->rx_desc_count = no_of_desc;
	dev_info->rx_desc = first_desc;
	dev_info->rx_desc_dma = dma_addr;

	netdev_dbg(netdev, "Rx Descriptors in Ring Mode: No. of descriptors = %d base = 0x%px dma = 0x%px\n",
			no_of_desc, first_desc, (void *)dma_addr);

	syn_dp_gmac_rx_desc_init_ring(dev_info->rx_desc, no_of_desc);

	dev_info->rx_next = 0;
	dev_info->rx_busy = 0;
	dev_info->rx_next_desc = first_desc;
	dev_info->rx_busy_desc = first_desc;
	dev_info->busy_rx_desc = 0;

	return 0;
}

/*
 * syn_dp_setup_tx_desc_queue
 *	This sets up the transmit Descriptor queue in ring or chain mode.
 */
static int syn_dp_setup_tx_desc_queue(struct net_device *netdev, struct device *dev, struct syn_dp_info *dev_info,
						uint32_t no_of_desc, uint32_t desc_mode)
{
	struct dma_desc *first_desc = NULL;
	dma_addr_t dma_addr;

	dev_info->tx_desc_count = 0;

	BUG_ON(desc_mode != RINGMODE);
	BUG_ON((no_of_desc & (no_of_desc - 1)) != 0);

	netdev_dbg(netdev, "Total size of memory required for Tx Descriptors in Ring Mode = %u\n", (uint32_t)((sizeof(struct dma_desc) * no_of_desc)));

	first_desc = dma_alloc_coherent(dev, sizeof(struct dma_desc) * no_of_desc, &dma_addr, GFP_KERNEL);
	if (first_desc == NULL) {
		netdev_dbg(netdev, "Error in Tx Descriptors memory allocation\n");
		return -ENOMEM;
	}

	dev_info->tx_desc_count = no_of_desc;
	dev_info->tx_desc = first_desc;
	dev_info->tx_desc_dma = dma_addr;
	netdev_dbg(netdev, "Tx Descriptors in Ring Mode: No. of descriptors = %d base = 0x%px dma = 0x%px\n"
			, no_of_desc, first_desc, (void *)dma_addr);

	syn_dp_gmac_tx_desc_init_ring(dev_info->tx_desc, dev_info->tx_desc_count);

	dev_info->tx_next = 0;
	dev_info->tx_busy = 0;
	dev_info->tx_next_desc = first_desc;
	dev_info->tx_busy_desc = first_desc;
	dev_info->busy_tx_desc = 0;

	return 0;
}

/*
 * syn_dp_setup_rings
 *	Perform initial setup of Tx/Rx rings
 */
int syn_dp_setup_rings(struct nss_dp_dev *gmac_dev, struct net_device *netdev, struct device *dev, struct syn_dp_info *dev_info)
{
	struct nss_gmac_hal_dev *nghd = gmac_dev->gmac_hal_ctx;
	int err;

	err = syn_dp_setup_rx_desc_queue(netdev, dev, dev_info, SYN_DP_RX_DESC_SIZE, RINGMODE);
	if (err) {
		netdev_dbg(netdev, "nss_dp_gmac: rx descriptor setup unsuccessfull, err code: %d", err);
		return NSS_DP_FAILURE;
	}

	err = syn_dp_setup_tx_desc_queue(netdev, dev, dev_info, SYN_DP_TX_DESC_SIZE, RINGMODE);
	if (err) {
		netdev_dbg(netdev, "nss_dp_gmac: tx descriptor setup unsuccessfull, err code: %d", err);
		return NSS_DP_FAILURE;
	}

	syn_dp_rx_refill(gmac_dev, dev_info);

	syn_init_tx_desc_base(nghd, dev_info->tx_desc_dma);
	syn_init_rx_desc_base(nghd, dev_info->rx_desc_dma);

	return NSS_DP_SUCCESS;
}

/*
 * syn_dp_cleanup_rings
 *	Cleanup Synopsys GMAC rings
 */
int syn_dp_cleanup_rings(struct nss_dp_dev *gmac_dev, struct net_device *netdev, struct syn_dp_info *dev_info)
{
	uint32_t rx_skb_index;
	struct dma_desc *rxdesc;

	uint32_t tx_skb_index;
	struct dma_desc *txdesc;
	int i;
	struct sk_buff *skb;

	/*
	 * Rx Ring cleaning
	 * We are assuming that the NAPI poll was already completed.
	 * No need of a lock here since the NAPI and interrupts have been disabled now
	 */
	rx_skb_index = dev_info->rx_busy;
	for (i = 0; i < dev_info->busy_rx_desc; i++) {
		rx_skb_index = rx_skb_index & (dev_info->rx_desc_count - 1);
		rxdesc = dev_info->rx_busy_desc;

		dma_unmap_single(&(gmac_dev->netdev->dev), rxdesc->buffer1,
			SYN_DP_MINI_JUMBO_FRAME_MTU, DMA_FROM_DEVICE);

		skb = dev_info->rx_skb_list[rx_skb_index];
		if (unlikely(skb != NULL)) {
			dev_kfree_skb(skb);
			dev_info->rx_skb_list[rx_skb_index] = NULL;
		}
	}

	dma_free_coherent(&(gmac_dev->netdev->dev), (sizeof(struct dma_desc) * SYN_DP_RX_DESC_SIZE),
				dev_info->rx_desc, dev_info->rx_desc_dma);

	/*
	 * Tx Ring cleaning
	 */
	spin_lock_bh(&dev_info->data_lock);

	tx_skb_index = dev_info->tx_busy;
	for (i = 0; i < dev_info->busy_tx_desc; i++) {
		tx_skb_index = tx_skb_index & (dev_info->tx_desc_count - 1);
		txdesc = dev_info->tx_busy_desc;

		dma_unmap_single(&(gmac_dev->netdev->dev), txdesc->buffer1,
			SYN_DP_MINI_JUMBO_FRAME_MTU, DMA_FROM_DEVICE);

		skb = dev_info->tx_skb_list[tx_skb_index];
		if (unlikely(skb != NULL)) {
			dev_kfree_skb(skb);
			dev_info->tx_skb_list[tx_skb_index] = NULL;
		}
	}

	spin_unlock_bh(&dev_info->data_lock);

	dma_free_coherent(&(gmac_dev->netdev->dev), (sizeof(struct dma_desc) * SYN_DP_TX_DESC_SIZE),
				dev_info->tx_desc, dev_info->tx_desc_dma);

	return 0;
}
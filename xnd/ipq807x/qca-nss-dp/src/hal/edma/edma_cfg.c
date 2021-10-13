/*
 * Copyright (c) 2016-2020, The Linux Foundation. All rights reserved.
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
#include <linux/interrupt.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_platform.h>
#include <linux/debugfs.h>
#include <linux/reset.h>

#include "nss_dp_dev.h"
#include "edma_regs.h"
#include "edma_data_plane.h"

#define EDMA_HW_RESET_ID "edma_rst"

/*
 * edma_cleanup_rxfill_ring_res()
 *	Cleanup resources for one RxFill ring
 */
static void edma_cleanup_rxfill_ring_res(struct edma_hw *ehw,
				struct edma_rxfill_ring *rxfill_ring)
{
	struct platform_device *pdev = ehw->pdev;
	struct sk_buff *skb;
	uint16_t cons_idx, curr_idx;
	struct edma_rxfill_desc *rxfill_desc;
	uint32_t reg_data = 0;
	struct edma_rx_preheader *rxph = NULL;
	int store_idx;

	/*
	 * Read RXFILL ring producer index
	 */
	reg_data = edma_reg_read(EDMA_REG_RXFILL_PROD_IDX(rxfill_ring->id));
	curr_idx = reg_data & EDMA_RXFILL_PROD_IDX_MASK;

	/*
	 * Read RXFILL ring consumer index
	 */
	reg_data = edma_reg_read(EDMA_REG_RXFILL_CONS_IDX(rxfill_ring->id));
	cons_idx = reg_data & EDMA_RXFILL_CONS_IDX_MASK;

	while (curr_idx != cons_idx) {
		/*
		 * Get RXFILL descriptor
		 */
		rxfill_desc = EDMA_RXFILL_DESC(rxfill_ring, cons_idx);

		/*
		 * Get Rx preheader
		 */
		rxph = (struct edma_rx_preheader *)
			phys_to_virt(rxfill_desc->buffer_addr);

		dma_unmap_single(&pdev->dev, rxfill_desc->buffer_addr,
					EDMA_RX_BUFF_SIZE, DMA_FROM_DEVICE);

		/*
		 * Get sk_buff and free it
		 */
		store_idx = rxph->opaque;
		skb = ehw->rx_skb_store[store_idx];
		ehw->rx_skb_store[store_idx] = NULL;
		dev_kfree_skb_any(skb);
		cons_idx++;
		if (cons_idx == rxfill_ring->count)
			cons_idx = 0;
	}

	/*
	 * Free RXFILL ring descriptors
	 */
	dma_free_coherent(&pdev->dev,
				(sizeof(struct edma_rxfill_desc)
				* rxfill_ring->count),
				rxfill_ring->desc, rxfill_ring->dma);
}

/*
 * edma_setup_rxfill_ring_res()
 *	Setup resources for one RxFill ring
 */
static int edma_setup_rxfill_ring_res(struct edma_hw *ehw,
				struct edma_rxfill_ring *rxfill_ring)
{
	struct platform_device *pdev = ehw->pdev;

	/*
	 * Allocate RxFill ring descriptors
	 */
	rxfill_ring->desc = dma_alloc_coherent(&pdev->dev,
				(sizeof(struct edma_rxfill_desc)
				* rxfill_ring->count),
				&rxfill_ring->dma, GFP_KERNEL);
	if (!rxfill_ring->desc) {
		pr_warn("Descriptor alloc for RXFILL ring %u failed\n",
				rxfill_ring->id);
		return -ENOMEM;
	}

	spin_lock_init(&rxfill_ring->lock);
	return 0;
}

/*
 * edma_setup_rxdesc_ring_res()
 *	Setup resources for one RxDesc ring
 */
static int edma_setup_rxdesc_ring_res(struct edma_hw *ehw,
				struct edma_rxdesc_ring *rxdesc_ring)
{
	struct platform_device *pdev = ehw->pdev;

	/*
	 * Allocate RxDesc ring descriptors
	 */
	rxdesc_ring->desc = dma_alloc_coherent(&pdev->dev,
				(sizeof(struct edma_rxdesc_desc)
				* rxdesc_ring->count),
				&rxdesc_ring->dma, GFP_KERNEL);
	if (!rxdesc_ring->desc) {
		pr_warn("Descriptor alloc for RXDESC ring %u failed\n",
				rxdesc_ring->id);
		return -ENOMEM;
	}

	return 0;
}

/*
 * edma_cleanup_rxdesc_ring_res()
 *	Cleanup resources for RxDesc ring
 */
static void edma_cleanup_rxdesc_ring_res(struct edma_hw *ehw,
				struct edma_rxdesc_ring *rxdesc_ring)
{
	struct platform_device *pdev = ehw->pdev;
	struct sk_buff *skb;
	struct edma_rxdesc_desc *rxdesc_desc;
	struct edma_rx_preheader *rxph = NULL;
	uint16_t prod_idx = 0;
	uint16_t cons_idx = 0;
	int store_idx;

	cons_idx = edma_reg_read(EDMA_REG_RXDESC_CONS_IDX(rxdesc_ring->id))
					& EDMA_RXDESC_CONS_IDX_MASK;

	prod_idx = edma_reg_read(EDMA_REG_RXDESC_PROD_IDX(rxdesc_ring->id))
					& EDMA_RXDESC_PROD_IDX_MASK;

	/*
	 * Free any buffers assigned to any descriptors
	 */
	while (cons_idx != prod_idx) {
		rxdesc_desc = EDMA_RXDESC_DESC(rxdesc_ring, cons_idx);

		rxph = (struct edma_rx_preheader *)
			phys_to_virt(rxdesc_desc->buffer_addr);

		dma_unmap_single(&pdev->dev, rxdesc_desc->buffer_addr,
					EDMA_RX_BUFF_SIZE, DMA_FROM_DEVICE);
		store_idx = rxph->opaque;
		skb = ehw->rx_skb_store[store_idx];
		ehw->rx_skb_store[store_idx] = NULL;
		dev_kfree_skb_any(skb);

		/*
		 * Update consumer index
		 */
		if (++cons_idx == rxdesc_ring->count)
			cons_idx = 0;
	}

	/*
	 * Free RXDESC ring descriptors
	 */
	dma_free_coherent(&pdev->dev,
				(sizeof(struct edma_rxdesc_desc)
				* rxdesc_ring->count),
				rxdesc_ring->desc, rxdesc_ring->dma);
}

/*
 * edma_cleanup_txcmpl_ring_res()
 *	Cleanup resources for one TxCmpl ring
 */
static void edma_cleanup_txcmpl_ring_res(struct edma_hw *ehw,
				struct edma_txcmpl_ring *txcmpl_ring)
{
	struct platform_device *pdev = ehw->pdev;

	/*
	 * Free any buffers assigned to any descriptors
	 */
	edma_clean_tx(ehw, txcmpl_ring);

	/*
	 * Free TxCmpl ring descriptors
	 */
	dma_free_coherent(&pdev->dev,
				(sizeof(struct edma_txcmpl_desc)
				* txcmpl_ring->count),
				txcmpl_ring->desc, txcmpl_ring->dma);
}

/*
 * edma_setup_txcmpl_ring_res()
 *	Setup resources for one TxCmpl ring
 */
static int edma_setup_txcmpl_ring_res(struct edma_hw *ehw,
				struct edma_txcmpl_ring *txcmpl_ring)
{
	struct platform_device *pdev = ehw->pdev;

	/*
	 * Allocate TxCmpl ring descriptors
	 */
	txcmpl_ring->desc = dma_alloc_coherent(&pdev->dev,
					(sizeof(struct edma_txcmpl_desc)
					* txcmpl_ring->count),
					&txcmpl_ring->dma, GFP_KERNEL);

	if (!txcmpl_ring->desc) {
		pr_warn("Descriptor alloc for TXCMPL ring %u failed\n",
				txcmpl_ring->id);

		return -ENOMEM;
	}

	return 0;
}

/*
 * edma_cleanup_txdesc_ring_res()
 *	Cleanup resources for one TxDesc ring
 */
static void edma_cleanup_txdesc_ring_res(struct edma_hw *ehw,
				struct edma_txdesc_ring *txdesc_ring)
{
	struct platform_device *pdev = ehw->pdev;
	struct sk_buff *skb = NULL;
	struct edma_txdesc_desc *txdesc = NULL;
	uint16_t prod_idx, cons_idx;
	size_t buf_len;
	uint32_t data;
	int store_idx;

	/*
	 * Free any buffers assigned to any descriptors
	 */
	data = edma_reg_read(EDMA_REG_TXDESC_PROD_IDX(txdesc_ring->id));
	prod_idx = data & EDMA_TXDESC_PROD_IDX_MASK;

	data = edma_reg_read(EDMA_REG_TXDESC_CONS_IDX(txdesc_ring->id));
	cons_idx = data & EDMA_TXDESC_CONS_IDX_MASK;

	while (cons_idx != prod_idx) {
		txdesc = EDMA_TXDESC_DESC(txdesc_ring, cons_idx);
		store_idx = txdesc->buffer_addr;
		skb = ehw->tx_skb_store[store_idx];
		ehw->tx_skb_store[store_idx] = NULL;

		buf_len = (txdesc->word1 & EDMA_TXDESC_DATA_LENGTH_MASK) >>
				EDMA_TXDESC_DATA_LENGTH_SHIFT;

		dma_unmap_single(&pdev->dev, (dma_addr_t)skb->data,
				buf_len + EDMA_TX_PREHDR_SIZE, DMA_TO_DEVICE);

		dev_kfree_skb_any(skb);
		cons_idx = (cons_idx + 1) & (txdesc_ring->count - 1);
		cons_idx++;
		if (cons_idx == txdesc_ring->count)
			cons_idx = 0;

	}

	/*
	 * Free Tx ring descriptors
	 */
	dma_free_coherent(&pdev->dev,
			  (sizeof(struct edma_txdesc_desc)
			   * txdesc_ring->count),
			   txdesc_ring->desc, txdesc_ring->dma);

}

/*
 * edma_setup_txdesc_ring_res()
 *	Setup resources for one TxDesc ring
 */
static int edma_setup_txdesc_ring_res(struct edma_hw *ehw,
				struct edma_txdesc_ring *txdesc_ring)
{
	struct platform_device *pdev = ehw->pdev;

	/*
	 * Allocate Tx ring descriptors
	 */
	txdesc_ring->desc = dma_alloc_coherent(&pdev->dev,
					(sizeof(struct edma_txdesc_desc)
					* txdesc_ring->count),
					&txdesc_ring->dma, GFP_KERNEL);
	if (!txdesc_ring->desc) {
		pr_warn("Descriptor alloc for TXDESC ring %u failed\n",
				txdesc_ring->id);
		return -ENOMEM;
	}

	spin_lock_init(&txdesc_ring->tx_lock);

	return 0;
}

/*
 * edma_setup_ring_resources()
 *	Allocate/setup resources for EDMA rings
 */
static int edma_setup_ring_resources(struct edma_hw *ehw)
{
	struct edma_txcmpl_ring *txcmpl_ring = NULL;
	struct edma_txdesc_ring *txdesc_ring = NULL;
	struct edma_rxfill_ring *rxfill_ring = NULL;
	struct edma_rxdesc_ring *rxdesc_ring = NULL;
	int i;
	int ret;
	int index;

	/*
	 * Allocate TxDesc ring descriptors
	 */
	for (i = 0; i < ehw->txdesc_rings; i++) {
		txdesc_ring = &ehw->txdesc_ring[i];
		txdesc_ring->count = EDMA_RING_SIZE;
		txdesc_ring->id = ehw->txdesc_ring_start + i;

		ret = edma_setup_txdesc_ring_res(ehw, txdesc_ring);
		if (ret != 0) {
			while (i-- >= 0)
				edma_cleanup_txdesc_ring_res(ehw,
					&ehw->txdesc_ring[i]);

			return -ENOMEM;

		}
	}

	/*
	 * Allocate TxCmpl ring descriptors
	 */
	for (i = 0; i < ehw->txcmpl_rings; i++) {
		txcmpl_ring = &ehw->txcmpl_ring[i];
		txcmpl_ring->count = EDMA_RING_SIZE;
		txcmpl_ring->id = ehw->txcmpl_ring_start + i;

		ret = edma_setup_txcmpl_ring_res(ehw, txcmpl_ring);

		if (ret != 0) {
			while (i-- >= 0)
				edma_cleanup_txcmpl_ring_res(ehw,
						&ehw->txcmpl_ring[i]);

			goto txcmpl_mem_alloc_fail;
		}
	}

	/*
	 * Allocate Rx fill ring descriptors
	 */
	for (i = 0; i < ehw->rxfill_rings; i++) {
		rxfill_ring = &ehw->rxfill_ring[i];
		rxfill_ring->count = EDMA_RING_SIZE;
		rxfill_ring->id = ehw->rxfill_ring_start + i;

		ret = edma_setup_rxfill_ring_res(ehw, rxfill_ring);
		if (ret != 0) {
			while (--i >= 0)
				edma_cleanup_rxfill_ring_res(ehw,
					&ehw->rxfill_ring[i]);

			goto rxfill_mem_alloc_fail;
		}
	}

	/*
	 * Allocate RxDesc ring descriptors
	 */
	for (i = 0; i < ehw->rxdesc_rings; i++) {
		rxdesc_ring = &ehw->rxdesc_ring[i];
		rxdesc_ring->count = EDMA_RING_SIZE;
		rxdesc_ring->id = ehw->rxdesc_ring_start + i;

		/*
		 * Create a mapping between RX Desc ring and Rx fill ring.
		 * Number of fill rings are lesser than the descriptor rings
		 * Share the fill rings across descriptor rings.
		 */

		index = ehw->rxfill_ring_start + (i % ehw->rxfill_rings);
		rxdesc_ring->rxfill =
			&ehw->rxfill_ring[index - ehw->rxfill_ring_start];

		ret = edma_setup_rxdesc_ring_res(ehw, rxdesc_ring);
		if (ret != 0) {
			while (--i >= 0)
				edma_cleanup_rxdesc_ring_res(ehw,
						&ehw->rxdesc_ring[i]);

			goto rxdesc_mem_alloc_fail;
		}
	}

	return 0;

rxdesc_mem_alloc_fail:
	for (i = 0; i < ehw->rxfill_rings; i++)
		edma_cleanup_rxfill_ring_res(ehw, &ehw->rxfill_ring[i]);

rxfill_mem_alloc_fail:
	for (i = 0; i < ehw->txcmpl_rings; i++)
		edma_cleanup_txcmpl_ring_res(ehw, &ehw->txcmpl_ring[i]);

txcmpl_mem_alloc_fail:
	for (i = 0; i < ehw->txdesc_rings; i++)
		edma_cleanup_txdesc_ring_res(ehw, &ehw->txdesc_ring[i]);

	return -ENOMEM;
}

/*
 * edma_free_rings()
 *	Free EDMA software rings
 */
static void edma_free_rings(struct edma_hw *ehw)
{
	kfree(ehw->rxfill_ring);
	kfree(ehw->rxdesc_ring);
	kfree(ehw->txdesc_ring);
	kfree(ehw->txcmpl_ring);
}

/*
 * edma_alloc_rings()
 *	Allocate EDMA software rings
 */
static int edma_alloc_rings(struct edma_hw *ehw)
{
	ehw->rxfill_ring = kzalloc((sizeof(struct edma_rxfill_ring) *
				ehw->rxfill_rings), GFP_KERNEL);
	if (!ehw->rxfill_ring)
		return -ENOMEM;

	ehw->rxdesc_ring = kzalloc((sizeof(struct edma_rxdesc_ring) *
				ehw->rxdesc_rings), GFP_KERNEL);
	if (!ehw->rxdesc_ring)
		goto rxdesc_ring_alloc_fail;

	ehw->txdesc_ring = kzalloc((sizeof(struct edma_txdesc_ring) *
				ehw->txdesc_rings), GFP_KERNEL);
	if (!ehw->txdesc_ring)
		goto txdesc_ring_alloc_fail;

	ehw->txcmpl_ring = kzalloc((sizeof(struct edma_txcmpl_ring) *
				ehw->txcmpl_rings), GFP_KERNEL);
	if (!ehw->txcmpl_ring)
		goto txcmpl_ring_alloc_fail;

	pr_info("Num rings - TxDesc:%u (%u-%u) TxCmpl:%u (%u-%u)\n",
			ehw->txdesc_rings, ehw->txdesc_ring_start,
			(ehw->txdesc_ring_start + ehw->txdesc_rings - 1),
			ehw->txcmpl_rings, ehw->txcmpl_ring_start,
			(ehw->txcmpl_ring_start + ehw->txcmpl_rings - 1));

	pr_info("RxDesc:%u (%u-%u) RxFill:%u (%u-%u)\n",
		ehw->rxdesc_rings, ehw->rxdesc_ring_start,
		(ehw->rxdesc_ring_start + ehw->rxdesc_rings - 1),
		ehw->rxfill_rings, ehw->rxfill_ring_start,
		(ehw->rxfill_ring_start + ehw->rxfill_rings - 1));

	return 0;
txcmpl_ring_alloc_fail:
	kfree(ehw->txdesc_ring);
txdesc_ring_alloc_fail:
	kfree(ehw->rxdesc_ring);
rxdesc_ring_alloc_fail:
	kfree(ehw->rxfill_ring);
	return -ENOMEM;
}

/*
 * edma_cleanup_rings()
 *	Cleanup EDMA rings
 */
void edma_cleanup_rings(struct edma_hw *ehw)
{
	int i;

	/*
	 * Free any buffers assigned to any descriptors
	 */
	for (i = 0; i < ehw->txdesc_rings; i++)
		edma_cleanup_txdesc_ring_res(ehw, &ehw->txdesc_ring[i]);

	/*
	 * Free Tx completion descriptors
	 */
	for (i = 0; i < ehw->txcmpl_rings; i++)
		edma_cleanup_txcmpl_ring_res(ehw, &ehw->txcmpl_ring[i]);

	/*
	 * Free Rx fill ring descriptors
	 */
	for (i = 0; i < ehw->rxfill_rings; i++)
		edma_cleanup_rxfill_ring_res(ehw, &ehw->rxfill_ring[i]);

	/*
	 * Free Rx completion ring descriptors
	 */
	for (i = 0; i < ehw->rxdesc_rings; i++)
		edma_cleanup_rxdesc_ring_res(ehw, &ehw->rxdesc_ring[i]);

	edma_free_rings(ehw);
}

/*
 * edma_init_rings()
 *	Initialize EDMA rings
 */
static int edma_init_rings(struct edma_hw *ehw)
{
	int ret = 0;

	ret = edma_alloc_rings(ehw);
	if (ret)
		return ret;

	ret = edma_setup_ring_resources(ehw);
	if (ret)
		return ret;

	return 0;
}

/*
 * edma_configure_txdesc_ring()
 *	Configure one TxDesc ring
 */
static void edma_configure_txdesc_ring(struct edma_hw *ehw,
					struct edma_txdesc_ring *txdesc_ring)
{
	uint32_t data = 0;
	uint16_t hw_cons_idx = 0;

	/*
	 * Configure TXDESC ring
	 */
	edma_reg_write(EDMA_REG_TXDESC_BA(txdesc_ring->id),
			(uint32_t)(txdesc_ring->dma &
			EDMA_RING_DMA_MASK));

	edma_reg_write(EDMA_REG_TXDESC_RING_SIZE(txdesc_ring->id),
			(uint32_t)(txdesc_ring->count &
			EDMA_TXDESC_RING_SIZE_MASK));

	data = edma_reg_read(EDMA_REG_TXDESC_CONS_IDX(txdesc_ring->id));
	data &= ~(EDMA_TXDESC_CONS_IDX_MASK);
	hw_cons_idx = data;

	data = edma_reg_read(EDMA_REG_TXDESC_PROD_IDX(txdesc_ring->id));
	data &= ~(EDMA_TXDESC_PROD_IDX_MASK);
	data |= hw_cons_idx & EDMA_TXDESC_PROD_IDX_MASK;
	edma_reg_write(EDMA_REG_TXDESC_PROD_IDX(txdesc_ring->id), data);
}

/*
 * edma_configure_txcmpl_ring()
 *	Configure one TxCmpl ring
 */
static void edma_configure_txcmpl_ring(struct edma_hw *ehw,
					struct edma_txcmpl_ring *txcmpl_ring)
{
	uint32_t tx_mod_timer;

	/*
	 * Configure TxCmpl ring base address
	 */
	edma_reg_write(EDMA_REG_TXCMPL_BA(txcmpl_ring->id),
			(uint32_t)(txcmpl_ring->dma & EDMA_RING_DMA_MASK));
	edma_reg_write(EDMA_REG_TXCMPL_RING_SIZE(txcmpl_ring->id),
			(uint32_t)(txcmpl_ring->count
			& EDMA_TXDESC_RING_SIZE_MASK));

	/*
	 * Set TxCmpl ret mode to opaque
	 */
	edma_reg_write(EDMA_REG_TXCMPL_CTRL(txcmpl_ring->id),
			EDMA_TXCMPL_RETMODE_OPAQUE);

	tx_mod_timer = (EDMA_TX_MOD_TIMER & EDMA_TX_MOD_TIMER_INIT_MASK)
			<< EDMA_TX_MOD_TIMER_INIT_SHIFT;
	edma_reg_write(EDMA_REG_TX_MOD_TIMER(txcmpl_ring->id),
				tx_mod_timer);

	edma_reg_write(EDMA_REG_TX_INT_CTRL(txcmpl_ring->id), 0x2);
}

/*
 * edma_configure_rxdesc_ring()
 *	Configure one RxDesc ring
 */
static void edma_configure_rxdesc_ring(struct edma_hw *ehw,
					struct edma_rxdesc_ring *rxdesc_ring)
{
	uint32_t data;

	edma_reg_write(EDMA_REG_RXDESC_BA(rxdesc_ring->id),
			(uint32_t)(rxdesc_ring->dma & 0xffffffff));

	data = rxdesc_ring->count & EDMA_RXDESC_RING_SIZE_MASK;
	data |= (ehw->rx_payload_offset & EDMA_RXDESC_PL_OFFSET_MASK)
		 << EDMA_RXDESC_PL_OFFSET_SHIFT;
	edma_reg_write(EDMA_REG_RXDESC_RING_SIZE(rxdesc_ring->id), data);

	data = (EDMA_RX_MOD_TIMER_INIT & EDMA_RX_MOD_TIMER_INIT_MASK)
			<< EDMA_RX_MOD_TIMER_INIT_SHIFT;
	edma_reg_write(EDMA_REG_RX_MOD_TIMER(rxdesc_ring->id), data);

	/*
	 * Enable ring. Set ret mode to 'opaque'.
	 */
	edma_reg_write(EDMA_REG_RX_INT_CTRL(rxdesc_ring->id), 0x2);
}

/*
 * edma_configure_rxfill_ring()
 *	Configure one RxFill ring
 */
static void edma_configure_rxfill_ring(struct edma_hw *ehw,
					struct edma_rxfill_ring *rxfill_ring)
{
	uint32_t data = 0;

	edma_reg_write(EDMA_REG_RXFILL_BA(rxfill_ring->id),
			(uint32_t)(rxfill_ring->dma & EDMA_RING_DMA_MASK));

	data = rxfill_ring->count & EDMA_RXFILL_RING_SIZE_MASK;
	edma_reg_write(EDMA_REG_RXFILL_RING_SIZE(rxfill_ring->id), data);

	/*
	 * Alloc Rx buffers
	 */
	edma_alloc_rx_buffer(ehw, rxfill_ring);
}

/*
 * edma_configure_rings()
 *	Configure EDMA rings
 */
static void edma_configure_rings(struct edma_hw *ehw)
{
	int i = 0;

	/*
	 * Initialize the store
	 */
	for (i = 0; i < EDMA_RING_SIZE; i++) {
		ehw->tx_skb_store[i] = NULL;
		ehw->rx_skb_store[i] = NULL;
	}

	/*
	 * Configure TXDESC ring
	 */
	for (i = 0; i < ehw->txdesc_rings; i++)
		edma_configure_txdesc_ring(ehw, &ehw->txdesc_ring[i]);

	/*
	 * Configure TXCMPL ring
	 */
	for (i = 0; i < ehw->txcmpl_rings; i++)
		edma_configure_txcmpl_ring(ehw, &ehw->txcmpl_ring[i]);

	/*
	 * Configure RXFILL rings
	 */
	for (i = 0; i < ehw->rxfill_rings; i++)
		edma_configure_rxfill_ring(ehw, &ehw->rxfill_ring[i]);

	/*
	 * Configure RXDESC ring
	 */
	for (i = 0; i < ehw->rxdesc_rings; i++)
		edma_configure_rxdesc_ring(ehw, &ehw->rxdesc_ring[i]);
}

/*
 * edma_hw_reset()
 *	Reset EDMA Hardware during initialization
 */
int edma_hw_reset(struct edma_hw *ehw)
{
	struct reset_control *rst;
	struct platform_device *pdev = ehw->pdev;

	rst = devm_reset_control_get(&pdev->dev, EDMA_HW_RESET_ID);
	if (IS_ERR(rst)) {
		pr_warn("DTS Node: %s does not exist\n", EDMA_HW_RESET_ID);
		return -EINVAL;
	}

	reset_control_assert(rst);
	udelay(100);

	reset_control_deassert(rst);
	udelay(100);

	pr_info("EDMA HW Reset completed succesfully\n");

	return 0;
}

/*
 * edma_hw_init()
 *	EDMA hw init
 */
int edma_hw_init(struct edma_hw *ehw)
{
	int ret = 0;
	int desc_index;
	uint32_t i, data, reg = 0;
	struct edma_rxdesc_ring *rxdesc_ring = NULL;

	data = edma_reg_read(EDMA_REG_MAS_CTRL);
	pr_info("EDMA ver %d hw init\n", data);

	/*
	 * Setup private data structure
	 */
	ehw->misc_intr_mask = 0x0;
	ehw->rxfill_intr_mask = EDMA_RXFILL_INT_MASK;
	ehw->rxdesc_intr_mask = EDMA_RXDESC_INT_MASK_PKT_INT;
	ehw->txcmpl_intr_mask = EDMA_TX_INT_MASK_PKT_INT |
				EDMA_TX_INT_MASK_UGT_INT;
	ehw->rx_payload_offset = EDMA_RX_PREHDR_SIZE;
	ehw->active = 0;
	ehw->edma_initialized = false;

	/* Reset EDMA */
	ret = edma_hw_reset(ehw);
	if (ret)
		return ret;

	/*
	 * Disable interrupts
	 */
	for (i = 0; i < EDMA_MAX_TXCMPL_RINGS; i++)
		edma_reg_write(EDMA_REG_TX_INT_MASK(i), 0);

	for (i = 0; i < EDMA_MAX_RXFILL_RINGS; i++)
		edma_reg_write(EDMA_REG_RXFILL_INT_MASK(i), 0);

	for (i = 0; i < EDMA_MAX_RXDESC_RINGS; i++)
		edma_reg_write(EDMA_REG_RX_INT_CTRL(i), 0);

	/*
	 * Disable Rx rings
	 */
	for (i = 0; i < EDMA_MAX_RXDESC_RINGS; i++) {
		data = edma_reg_read(EDMA_REG_RXDESC_CTRL(i));
		data &= ~EDMA_RXDESC_RX_EN;
		edma_reg_write(EDMA_REG_RXDESC_CTRL(i), data);
	}

	/*
	 * Disable RxFill Rings
	 */
	for (i = 0; i < EDMA_MAX_RXFILL_RINGS; i++) {
		data = edma_reg_read(EDMA_REG_RXFILL_RING_EN(i));
		data &= ~EDMA_RXFILL_RING_EN;
		edma_reg_write(EDMA_REG_RXFILL_RING_EN(i), data);
	}

	/*
	 * Disable Tx rings
	 */
	for (desc_index = 0; desc_index < EDMA_MAX_TXDESC_RINGS; desc_index++) {
		data = edma_reg_read(EDMA_REG_TXDESC_CTRL(desc_index));
		data &= ~EDMA_TXDESC_TX_EN;
		edma_reg_write(EDMA_REG_TXDESC_CTRL(desc_index), data);
	}

#if defined(NSS_DP_IPQ807X)
	/*
	 * Clear the TXDESC2CMPL_MAP_xx reg before setting up
	 * the mapping. This register holds TXDESC to TXFILL ring
	 * mapping.
	 */
	edma_reg_write(EDMA_REG_TXDESC2CMPL_MAP_0, 0);
	edma_reg_write(EDMA_REG_TXDESC2CMPL_MAP_1, 0);
	edma_reg_write(EDMA_REG_TXDESC2CMPL_MAP_2, 0);
	desc_index = ehw->txcmpl_ring_start;

	/*
	 * 3 registers to hold the completion mapping for total 24
	 * TX desc rings (0-9,10-19 and rest). In each entry 3 bits hold
	 * the mapping for a particular TX desc ring.
	 */
	for (i = ehw->txdesc_ring_start;
		i < ehw->txdesc_ring_end; i++) {
		if (i >= 0 && i <= 9)
			reg = EDMA_REG_TXDESC2CMPL_MAP_0;
		else if (i >= 10 && i <= 19)
			reg = EDMA_REG_TXDESC2CMPL_MAP_1;
		else
			reg = EDMA_REG_TXDESC2CMPL_MAP_2;

		pr_debug("Configure TXDESC:%u to use TXCMPL:%u\n",
				i, desc_index);

		data = edma_reg_read(reg);
		data |= (desc_index & 0x7) << ((i % 10) * 3);
		edma_reg_write(reg, data);

		desc_index++;
		if (desc_index == ehw->txcmpl_ring_end)
				desc_index = ehw->txcmpl_ring_start;
	}
#endif

	/*
	 * Set PPE QID to EDMA Rx ring mapping.
	 * When coming up use only queue 0.
	 * HOST EDMA rings. FW EDMA comes up and overwrites as required.
	 * Each entry can hold mapping for 8 PPE queues and entry size is
	 * 4 bytes
	 */
	desc_index = ehw->rxdesc_ring_start;
	data = 0;
	data |= (desc_index & 0xF);
	edma_reg_write(EDMA_QID2RID_TABLE_MEM(0), data);
	pr_debug("Configure QID2RID reg:0x%x to 0x%x\n", reg, data);

	ret = edma_init_rings(ehw);
	if (ret)
		return ret;

	edma_configure_rings(ehw);

	/*
	 * Set RXDESC2FILL_MAP_xx reg.
	 * There are two registers RXDESC2FILL_0 and RXDESC2FILL_1
	 * 3 bits holds the rx fill ring mapping for each of the
	 * rx descriptor ring.
	 */
	edma_reg_write(EDMA_REG_RXDESC2FILL_MAP_0, 0);
	edma_reg_write(EDMA_REG_RXDESC2FILL_MAP_1, 0);
	for (i = ehw->rxdesc_ring_start;
			i < ehw->rxdesc_ring_end; i++) {
		if ((i >= 0) && (i <= 9))
			reg = EDMA_REG_RXDESC2FILL_MAP_0;
		else
			reg = EDMA_REG_RXDESC2FILL_MAP_1;

		rxdesc_ring = &ehw->rxdesc_ring[i - ehw->rxdesc_ring_start];

		pr_debug("Configure RXDESC:%u to use RXFILL:%u\n",
				rxdesc_ring->id, rxdesc_ring->rxfill->id);

		data = edma_reg_read(reg);
		data |= (rxdesc_ring->rxfill->id & 0x7) << ((i % 10) * 3);
		edma_reg_write(reg, data);
	}

	reg = EDMA_REG_RXDESC2FILL_MAP_0;
	pr_debug("EDMA_REG_RXDESC2FILL_MAP_0: 0x%x\n", edma_reg_read(reg));
	reg = EDMA_REG_RXDESC2FILL_MAP_1;
	pr_debug("EDMA_REG_RXDESC2FILL_MAP_1: 0x%x\n", edma_reg_read(reg));

#if defined(NSS_DP_IPQ807X)
	reg = EDMA_REG_TXDESC2CMPL_MAP_0;
	pr_debug("EDMA_REG_TXDESC2CMPL_MAP_0: 0x%x\n", edma_reg_read(reg));
	reg = EDMA_REG_TXDESC2CMPL_MAP_1;
	pr_debug("EDMA_REG_TXDESC2CMPL_MAP_1: 0x%x\n", edma_reg_read(reg));
	reg = EDMA_REG_TXDESC2CMPL_MAP_2;
	pr_debug("EDMA_REG_TXDESC2CMPL_MAP_2: 0x%x\n", edma_reg_read(reg));
#endif

	/*
	 * Configure DMA request priority, DMA read burst length,
	 * and AXI write size.
	 */
	data = EDMA_DMAR_BURST_LEN_SET(EDMA_BURST_LEN_ENABLE)
		| EDMA_DMAR_REQ_PRI_SET(0)
		| EDMA_DMAR_TXDATA_OUTSTANDING_NUM_SET(31)
		| EDMA_DMAR_TXDESC_OUTSTANDING_NUM_SET(7)
		| EDMA_DMAR_RXFILL_OUTSTANDING_NUM_SET(7);
	edma_reg_write(EDMA_REG_DMAR_CTRL, data);
#if defined(NSS_DP_IPQ60XX)
	data = edma_reg_read(EDMA_REG_AXIW_CTRL);
	data |= EDMA_AXIW_MAX_WR_SIZE_EN;
	edma_reg_write(EDMA_REG_AXIW_CTRL, data);
#endif

	/*
	 * Misc error mask
	 */
	data = EDMA_MISC_AXI_RD_ERR_MASK_EN |
		EDMA_MISC_AXI_WR_ERR_MASK_EN |
		EDMA_MISC_RX_DESC_FIFO_FULL_MASK_EN |
		EDMA_MISC_RX_ERR_BUF_SIZE_MASK_EN |
		EDMA_MISC_TX_SRAM_FULL_MASK_EN |
		EDMA_MISC_TX_CMPL_BUF_FULL_MASK_EN |
		EDMA_MISC_DATA_LEN_ERR_MASK_EN;
#if defined(NSS_DP_IPQ807X)
	data |= EDMA_MISC_PKT_LEN_LA_64K_MASK_EN |
		 EDMA_MISC_PKT_LEN_LE_40_MASK_EN;
#else
	data |= EDMA_MISC_TX_TIMEOUT_MASK_EN;
#endif
	edma_reg_write(EDMA_REG_MISC_INT_MASK, data);

	/*
	 * Global EDMA enable and padding enable
	 */
	data = EDMA_PORT_PAD_EN | EDMA_PORT_EDMA_EN;
	edma_reg_write(EDMA_REG_PORT_CTRL, data);

	/*
	 * Enable Rx rings
	 */
	for (i = ehw->rxdesc_ring_start; i < ehw->rxdesc_ring_end; i++) {
		data = edma_reg_read(EDMA_REG_RXDESC_CTRL(i));
		data |= EDMA_RXDESC_RX_EN;
		edma_reg_write(EDMA_REG_RXDESC_CTRL(i), data);
	}

	for (i = ehw->rxfill_ring_start; i < ehw->rxfill_ring_end; i++) {
		data = edma_reg_read(EDMA_REG_RXFILL_RING_EN(i));
		data |= EDMA_RXFILL_RING_EN;
		edma_reg_write(EDMA_REG_RXFILL_RING_EN(i), data);
	}

	/*
	 * Enable Tx rings
	 */
	for (i = ehw->txdesc_ring_start; i < ehw->txdesc_ring_end; i++) {
		data = edma_reg_read(EDMA_REG_TXDESC_CTRL(i));
		data |= EDMA_TXDESC_TX_EN;
		edma_reg_write(EDMA_REG_TXDESC_CTRL(i), data);
	}

	ehw->edma_initialized = true;

	return 0;
}

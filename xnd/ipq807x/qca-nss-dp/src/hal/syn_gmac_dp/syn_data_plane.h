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


#ifndef __NSS_DP_SYN_DATAPLANE__
#define __NSS_DP_SYN_DATAPLANE__

#include "nss_dp_dev.h"
#include "syn_dma_desc.h"

#define SYN_DP_TX_DESC_SIZE		128	/* Tx Descriptors needed in the descriptor pool/queue */
#define SYN_DP_RX_DESC_SIZE		128	/* Rx Descriptors needed in the descriptor pool/queue */
#define SYN_DP_MINI_JUMBO_FRAME_MTU	1978
#define SYN_DP_MAX_DESC_BUFF		0x1FFF	/* Max size of buffer that can be programed into one field of desc */

/*
 * syn_dp_info
 *	Synopysys GMAC Dataplane information
 */
struct syn_dp_info {
	struct nss_dp_gmac_stats stats;			/* GMAC driver stats */

	struct sk_buff *rx_skb_list[SYN_DP_RX_DESC_SIZE];	/* Rx skb pool helping RX DMA descriptors*/

	dma_addr_t rx_desc_dma;				/* Dma-albe address of first rx descriptor
								either in ring or chain mode, this is
								used by the GMAC device */

	struct dma_desc *rx_desc;			/* start address of RX descriptors ring or
								chain, this is used by the driver */

	uint32_t busy_rx_desc;				/* Number of Rx Descriptors owned by
				   				DMA at any given time */

	uint32_t rx_desc_count;				/* number of rx descriptors in the
				   				tx descriptor queue/pool */

	uint32_t rx_busy;				/* index of the rx descriptor owned by DMA,
				   				obtained by nss_gmac_get_rx_qptr() */

	uint32_t rx_next;				/* index of the rx descriptor next available
				   				with driver, given to DMA by
				   				nss_gmac_set_rx_qptr()*/

	struct dma_desc *rx_busy_desc;			/* Rx Descriptor address corresponding
				   				to the index tx_busy */

	struct dma_desc *rx_next_desc;			/* Rx Descriptor address corresponding
								to the index rx_next */

	struct sk_buff *tx_skb_list[SYN_DP_RX_DESC_SIZE];	/* Tx skb pool helping RX DMA descriptors*/

	dma_addr_t tx_desc_dma;				/* Dma-able address of first tx descriptor
								either in ring or chain mode, this is used
								by the GMAC device */

	struct dma_desc *tx_desc;			/* start address of TX descriptors ring or
								chain, this is used by the driver */

	uint32_t busy_tx_desc;				/* Number of Tx Descriptors owned by
								DMA at any given time */

	uint32_t tx_desc_count;				/* number of tx descriptors in the
								rx descriptor queue/pool */

	uint32_t tx_busy;				/* index of the tx descriptor owned by DMA,
								is obtained by nss_gmac_get_tx_qptr() */

	uint32_t tx_next;				/* index of the tx descriptor next available
								with driver, given to DMA by
								nss_gmac_set_tx_qptr() */

	struct dma_desc *tx_busy_desc;			/* Tx Descriptor address corresponding
								to the index tx_busy */

	struct dma_desc *tx_next_desc;			/* Tx Descriptor address corresponding
								to the index tx_next */

	spinlock_t data_lock;				/* Lock to protect datapath */
	spinlock_t stats_lock;				/* Lock to protect datapath */
	int napi_added;					/* flag to indicate napi add status */
};

/*
 * GMAC Tx/Tx APIs
 */
int syn_dp_setup_rings(struct nss_dp_dev *gmac_dev, struct net_device *netdev, struct device *dev, struct syn_dp_info *dev_info);
int syn_dp_cleanup_rings(struct nss_dp_dev *gmac_dev, struct net_device *netdev, struct syn_dp_info *dev_info);

int syn_dp_rx(struct nss_dp_dev *gmac_dev, struct syn_dp_info *dev_info, int budget);
void syn_dp_rx_refill(struct nss_dp_dev *gmac_dev, struct syn_dp_info *dev_info);

int syn_dp_tx(struct nss_dp_dev *gmac_dev, struct syn_dp_info *dev_info, struct sk_buff *skb);
void syn_dp_process_tx_complete(struct nss_dp_dev *gmac_dev, struct syn_dp_info *dev_info);

#endif /*  __NSS_DP_SYN_DATAPLANE__ */
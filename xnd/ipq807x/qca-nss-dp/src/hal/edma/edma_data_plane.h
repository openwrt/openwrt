/*
 **************************************************************************
 * Copyright (c) 2016, 2018-2021, The Linux Foundation. All rights reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 **************************************************************************
*/

#include "nss_dp_dev.h"

#ifndef __NSS_DP_EDMA_DATAPLANE__
#define __NSS_DP_EDMA_DATAPLANE__

#define EDMA_BUF_SIZE			2000
#define EDMA_DEVICE_NODE_NAME		"edma"
#define EDMA_RX_BUFF_SIZE		(EDMA_BUF_SIZE + EDMA_RX_PREHDR_SIZE)
#define EDMA_RX_PREHDR_SIZE		(sizeof(struct edma_rx_preheader))
#define EDMA_TX_PREHDR_SIZE		(sizeof(struct edma_tx_preheader))
#define EDMA_RING_SIZE			128
#define EDMA_NAPI_WORK			100
#define EDMA_START_GMACS		NSS_DP_START_IFNUM
#define EDMA_MAX_GMACS			NSS_DP_HAL_MAX_PORTS
#define EDMA_TX_PKT_MIN_SIZE		33	/* IPQ807x EDMA needs a minimum packet size of 33 bytes */
#if defined(NSS_DP_IPQ60XX)
#define EDMA_MAX_TXCMPL_RINGS		24	/* Max TxCmpl rings */
#else
#define EDMA_MAX_TXCMPL_RINGS		8	/* Max TxCmpl rings */
#endif
#define EDMA_MAX_RXDESC_RINGS		16	/* Max RxDesc rings */
#define EDMA_MAX_RXFILL_RINGS		8	/* Max RxFill rings */
#define EDMA_MAX_TXDESC_RINGS		24	/* Max TxDesc rings */
#define EDMA_GET_DESC(R, i, type) (&(((type *)((R)->desc))[i]))
#define EDMA_RXFILL_DESC(R, i) EDMA_GET_DESC(R, i, struct edma_rxfill_desc)
#define EDMA_RXDESC_DESC(R, i) EDMA_GET_DESC(R, i, struct edma_rxdesc_desc)
#define EDMA_TXDESC_DESC(R, i) EDMA_GET_DESC(R, i, struct edma_txdesc_desc)
#define EDMA_RXPH_SRC_INFO_TYPE_GET(rxph)	(((rxph)->src_info >> 8) & 0xf0)
#define EDMA_RXPH_SERVICE_CODE_GET(rxph)	(((rxph)->rx_pre4) & 0xff)

/*
 * Tx descriptor
 */
struct edma_txdesc_desc {
	uint32_t buffer_addr;
		/* buffer address */
	uint32_t word1;
		/* more bit, TSO, preheader, pool, offset and length */
};

/*
 * TxCmpl descriptor
 */
struct edma_txcmpl_desc {
	uint32_t buffer_addr;	/* buffer address/opaque */
	uint32_t status;	/* status */
};

/*
 * Rx descriptor
 */
struct edma_rxdesc_desc {
	uint32_t buffer_addr;	/* buffer address */
	uint32_t status;	/* status */
};

/*
 * RxFill descriptor
 */
struct edma_rxfill_desc {
	uint32_t buffer_addr;	/* Buffer address */
	uint32_t word1;		/* opaque_ind and buffer size */
};

/*
 * Tx descriptor ring
 */
struct edma_txdesc_ring {
	uint32_t id;			/* TXDESC ring number */
	void *desc;			/* descriptor ring virtual address */
	dma_addr_t dma;			/* descriptor ring physical address */
	spinlock_t tx_lock;		/* Tx ring lock */
	uint16_t count;			/* number of descriptors */
};

/*
 * TxCmpl ring
 */
struct edma_txcmpl_ring {
	uint32_t id;			/* TXCMPL ring number */
	void *desc;			/* descriptor ring virtual address */
	dma_addr_t dma;			/* descriptor ring physical address */
	uint16_t count;			/* number of descriptors in the ring */
};

/*
 * RxFill ring
 */
struct edma_rxfill_ring {
	uint32_t id;			/* RXFILL ring number */
	void *desc;			/* descriptor ring virtual address */
	dma_addr_t dma;			/* descriptor ring physical address */
	spinlock_t lock;		/* Rx ring lock */
	uint16_t count;			/* number of descriptors in the ring */
};

/*
 * RxDesc ring
 */
struct edma_rxdesc_ring {
	uint32_t id;			/* RXDESC ring number */
	struct edma_rxfill_ring *rxfill;	/* RXFILL ring used */
	void *desc;			/* descriptor ring virtual address */
	dma_addr_t dma;			/* descriptor ring physical address */
	uint16_t count;			/* number of descriptors in the ring */
};

/*
 * EDMA Tx Preheader
 */
struct edma_tx_preheader {
	uint32_t opaque;   /* Opaque, contains skb pointer */
	uint16_t src_info; /* Src information */
	uint16_t dst_info; /* Dest information */
	uint32_t tx_pre2;  /* SVLAN & CVLAN flag, drop prec, hash value */
	uint32_t tx_pre3;  /* STAG, CTAG */
	uint32_t tx_pre4;  /* CPU code, L3 & L4 offset, service code */
	uint32_t tx_pre5;  /* IP addr index, ACL index */
	uint32_t tx_pre6;  /* IP payload checksum, copy2cpu, timestamp, dscp */
	uint32_t tx_pre7;  /* Timestamp, QoS TAG */
};

/*
 * EDMA Rx Preheader
 */
struct edma_rx_preheader {
	uint32_t opaque;
		/* Opaque, contains skb pointer*/
	uint16_t src_info;
		/* Src information */
	uint16_t dst_info;
		/* Dest information */
	uint32_t rx_pre2;
		/* SVLAN & CVLAN flag, drop prec, hash value */
	uint32_t rx_pre3;
		/* STAG, CTAG */
	uint32_t rx_pre4;
		/* CPU code, L3 & L4 offset, service code */
	uint32_t rx_pre5;
		/* IP addr index, ACL index */
	uint32_t rx_pre6;
		/* IP payload checksum, copy2cpu, timestamp, dscp */
	uint32_t rx_pre7;
		/* Timestamp, QoS TAG */
};

enum edma_tx {
	EDMA_TX_OK = 0,		/* Tx success */
	EDMA_TX_DESC = 1,	/* Not enough descriptors */
	EDMA_TX_FAIL = 2,	/* Tx failure */
};

/*
 * EDMA private data structure
 */
struct edma_hw {
	struct napi_struct napi;
			/* napi structure */
	struct net_device *netdev_arr[EDMA_MAX_GMACS];
			/* netdev for each gmac port */
	struct device_node *device_node;
			/* Device tree node */
	struct platform_device *pdev;
			/* Platform device */
	void __iomem *reg_base;
			/* Base register address */
	struct resource *reg_resource;
			/* Memory resource */
	uint16_t rx_payload_offset;
			/* start of the payload offset */
	uint32_t flags;
			/* internal flags */
	int active;
			/* status */
	int napi_added;
			/* flag to indicate napi add status */

	/*
	 * Debugfs entries
	 */
	struct dentry *edma_dentry;
	struct dentry *txdesc_dentry;
	struct dentry *txcmpl_dentry;
	struct dentry *rxdesc_dentry;

	/*
	 * Store for tx and rx skbs
	 */
	struct sk_buff *rx_skb_store[EDMA_RING_SIZE];
	struct sk_buff *tx_skb_store[EDMA_RING_SIZE];

	struct edma_rxfill_ring *rxfill_ring;
			/* Rx Fill Ring, SW is producer */
	struct edma_rxdesc_ring *rxdesc_ring;
			/* Rx Descriptor Ring, SW is consumer */
	struct edma_txdesc_ring *txdesc_ring;
			/* Tx Descriptor Ring, SW is producer */
	struct edma_txcmpl_ring *txcmpl_ring;
			/* Tx Completion Ring, SW is consumer */

	uint32_t txdesc_rings;
			/* Number of TxDesc rings */
	uint32_t txdesc_ring_start;
			/* Id of first TXDESC ring */
	uint32_t txdesc_ring_end;
			/* Id of the last TXDESC ring */
	uint32_t txcmpl_rings;
			/* Number of TxCmpl rings */
	uint32_t txcmpl_ring_start;
			/* Id of first TXCMPL ring */
	uint32_t txcmpl_ring_end;
			/* Id of last TXCMPL ring */
	uint32_t rxfill_rings;
			/* Number of RxFill rings */
	uint32_t rxfill_ring_start;
			/* Id of first RxFill ring */
	uint32_t rxfill_ring_end;
			/* Id of last RxFill ring */
	uint32_t rxdesc_rings;
			/* Number of RxDesc rings */
	uint32_t rxdesc_ring_start;
			/* Id of first RxDesc ring */
	uint32_t rxdesc_ring_end;
			/* Id of last RxDesc ring */
	uint32_t txcmpl_intr[EDMA_MAX_TXCMPL_RINGS];
			/* TxCmpl ring IRQ numbers */
	uint32_t rxfill_intr[EDMA_MAX_RXFILL_RINGS];
			/* Rx fill ring IRQ numbers */
	uint32_t rxdesc_intr[EDMA_MAX_RXDESC_RINGS];
			/* Rx desc ring IRQ numbers */
	uint32_t misc_intr;
			/* Misc IRQ number */

	uint32_t tx_intr_mask;
			/* Tx interrupt mask */
	uint32_t rxfill_intr_mask;
			/* Rx fill ring interrupt mask */
	uint32_t rxdesc_intr_mask;
			/* Rx Desc ring interrupt mask */
	uint32_t txcmpl_intr_mask;
			/* Tx Cmpl ring interrupt mask */
	uint32_t misc_intr_mask;
			/* misc interrupt interrupt mask */
	uint32_t dp_override_cnt;
			/* number of interfaces overriden */
	bool edma_initialized;
			/* flag to check initialization status */
};

extern struct edma_hw edma_hw;

uint32_t edma_reg_read(uint32_t reg_off);
void edma_reg_write(uint32_t reg_off, uint32_t val);

int edma_alloc_rx_buffer(struct edma_hw *ehw,
		struct edma_rxfill_ring *rxfill_ring);
enum edma_tx edma_ring_xmit(struct edma_hw *ehw,
			    struct net_device *netdev,
			    struct sk_buff *skb,
			    struct edma_txdesc_ring *txdesc_ring);
uint32_t edma_clean_tx(struct edma_hw *ehw,
			struct edma_txcmpl_ring *txcmpl_ring);
irqreturn_t edma_handle_irq(int irq, void *ctx);
irqreturn_t edma_handle_misc_irq(int irq, void *ctx);
int edma_napi(struct napi_struct *napi, int budget);
void edma_cleanup_rings(struct edma_hw *ehw);
void edma_cleanup(bool is_dp_override);
int edma_hw_init(struct edma_hw *ehw);
#endif /* __NSS_DP_EDMA_DATAPLANE__ */

/* SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021
 *
 * Richard van Schagen <vschagen@icloud.com>
 */

#include <linux/skbuff.h>
#include <net/xfrm.h>

#include "eip93-main.h"
#include "eip93-regs.h"

#define MTK_IPSEC_CB(__skb) ((struct mtk_ipsec_results *)&((__skb)->cb[0]))

struct ipsec_sa_entry {
	struct mtk_device		*mtk;
	struct saRecord_s		*sa;
	dma_addr_t			sa_base;
	struct crypto_shash		*shash;
	u32				blksize;
	struct eip93_descriptor_s	cdesc;
};

struct mtk_ipsec_results {
	u32		cb;
};

struct mtk_ipsec_cb {
	u32		org_cb;
	dma_addr_t	dstAddr;
	int 		err;
	int		len;
	u8		nexthdr;
};

int mtk_offload_register(void);

void mtk_offload_deregister(void);

int mtk_ipsec_offload(struct xfrm_state *x, struct sk_buff *skb);

void mtk_ipsec_rx_done(unsigned long data);

void mtk_ipsec_tx_done(unsigned long data);

/*
 **************************************************************************
 * Copyright (c) 2016-2017, 2019-2020, The Linux Foundation. All rights reserved.
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
 **************************************************************************
 */

/*
 * nss_drv_stats.h
 *	NSS driver stats header file.
 */

#ifndef __NSS_DRV_STATS_H
#define __NSS_DRV_STATS_H

#include <linux/debugfs.h>

/*
 * HLOS driver statistics
 *
 * WARNING: There is a 1:1 mapping between values below and corresponding
 *	stats string array in nss_stats.c.
 */
enum NSS_DRV_STATS {
	NSS_DRV_STATS_NBUF_ALLOC_FAILS = 0,	/* NBUF allocation errors */
	NSS_DRV_STATS_PAGED_BUF_ALLOC_FAILS,	/* Paged buf allocation errors */
	NSS_DRV_STATS_TX_QUEUE_FULL_0,		/* Tx queue full for Core 0*/
	NSS_DRV_STATS_TX_QUEUE_FULL_1,		/* Tx queue full for Core 1*/
	NSS_DRV_STATS_TX_EMPTY,			/* H2N Empty buffers */
	NSS_DRV_STATS_PAGED_TX_EMPTY,		/* H2N Paged Empty buffers */
	NSS_DRV_STATS_TX_PACKET,		/* H2N Data packets */
	NSS_DRV_STATS_TX_CMD_REQ,		/* H2N Control packets */
	NSS_DRV_STATS_TX_CRYPTO_REQ,		/* H2N Crypto requests */
	NSS_DRV_STATS_TX_BUFFER_REUSE,		/* H2N Reuse buffer count */
	NSS_DRV_STATS_RX_EMPTY,			/* N2H Empty buffers */
	NSS_DRV_STATS_RX_PACKET,		/* N2H Data packets */
	NSS_DRV_STATS_RX_EXT_PACKET,		/* N2H EXT type packets */
	NSS_DRV_STATS_RX_CMD_RESP,		/* N2H Command responses */
	NSS_DRV_STATS_RX_STATUS,		/* N2H Status packets */
	NSS_DRV_STATS_RX_CRYPTO_RESP,		/* N2H Crypto responses */
	NSS_DRV_STATS_RX_VIRTUAL,		/* N2H Virtual packets */
	NSS_DRV_STATS_TX_SIMPLE,		/* H2N Simple SKB Packets */
	NSS_DRV_STATS_TX_NR_FRAGS,		/* H2N NR Frags SKB Packets */
	NSS_DRV_STATS_TX_FRAGLIST,		/* H2N Fraglist SKB Packets */
	NSS_DRV_STATS_RX_SIMPLE,		/* N2H Simple SKB Packets */
	NSS_DRV_STATS_RX_NR_FRAGS,		/* N2H NR Frags SKB Packets */
	NSS_DRV_STATS_RX_SKB_FRAGLIST,		/* N2H Fraglist SKB Packets */
	NSS_DRV_STATS_RX_BAD_DESCRIPTOR,	/* N2H Bad descriptor reads */
	NSS_DRV_STATS_RX_INVALID_INTERFACE,	/* N2H Received descriptor for invalid interface */
	NSS_DRV_STATS_RX_INVALID_CORE_ID,	/* N2H Received packet for invalid core_id */
	NSS_DRV_STATS_RX_INVALID_BUFFER_TYPE,	/* N2H Received packet for invalid buffer type */
	NSS_DRV_STATS_NSS_SKB_COUNT,		/* NSS SKB Pool Count */
	NSS_DRV_STATS_CHAIN_SEG_PROCESSED,	/* N2H SKB Chain Processed Count */
	NSS_DRV_STATS_FRAG_SEG_PROCESSED,	/* N2H Frag Processed Count */
	NSS_DRV_STATS_TX_CMD_QUEUE_FULL,	/* Tx H2N Control packets fail due to queue full */
#ifdef NSS_MULTI_H2N_DATA_RING_SUPPORT
	NSS_DRV_STATS_TX_PACKET_QUEUE_0,	/* H2N Data packets on queue0 */
	NSS_DRV_STATS_TX_PACKET_QUEUE_1,	/* H2N Data packets on queue1 */
	NSS_DRV_STATS_TX_PACKET_QUEUE_2,	/* H2N Data packets on queue2 */
	NSS_DRV_STATS_TX_PACKET_QUEUE_3,	/* H2N Data packets on queue3 */
	NSS_DRV_STATS_TX_PACKET_QUEUE_4,	/* H2N Data packets on queue4 */
	NSS_DRV_STATS_TX_PACKET_QUEUE_5,	/* H2N Data packets on queue5 */
	NSS_DRV_STATS_TX_PACKET_QUEUE_6,	/* H2N Data packets on queue6 */
	NSS_DRV_STATS_TX_PACKET_QUEUE_7,	/* H2N Data packets on queue7 */
#endif
	NSS_DRV_STATS_MAX,
};

extern void nss_drv_stats_dentry_create(void);
extern ssize_t nss_wt_stats_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos);
#endif /* __NSS_DRV_STATS_H */

/*
 **************************************************************************
 * Copyright (c) 2017,2019-2020, The Linux Foundation. All rights reserved.
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

#ifndef __NSS_LSO_RX_STATS_H
#define __NSS_LSO_RX_STATS_H

#include <nss_cmn.h>

/*
 * lso_rx_node statistics.
 */
struct nss_lso_rx_stats_sync {
	struct nss_cmn_node_stats node_stats;

	uint32_t tx_dropped;				/* Number of packets dropped because lso_rx transmit queue is full */
	uint32_t dropped;				/* Total of packets dropped by the node internally */
	uint32_t pbuf_alloc_fail;			/* Count number of pbuf alloc fails */
	uint32_t pbuf_reference_fail;			/* Count number of pbuf ref fails */

	/*
	 * If we're generating per-packet statistics then we count total lso_rx processing ticks
	 * worst-case ticks and the number of iterations around the lso_rx handler that we take.
	 */
	uint32_t total_ticks;				/* Total clock ticks spend inside the lso_rx handler */
	uint32_t worst_case_ticks;
							/* Worst case iteration of the lso_rx handler in ticks */
	uint32_t iterations;				/* Number of iterations around the lso_rx handler */
};

/*
 * Message types for lso_rx
 */
enum nss_lso_rx_metadata_types {
	NSS_LSO_RX_STATS_SYNC_MSG,			/* Message type - stats sync message */
};

/*
 * Message structure to send receive LSO_RX commands
 */
struct nss_lso_rx_msg {
	struct nss_cmn_msg cm;					/* Message header */
	union {
		struct nss_lso_rx_stats_sync stats_sync;	/* Stats sub-message */
	} msg;
};

/*
 * lso_rx statistics APIs
 */
extern void nss_lso_rx_stats_notify(struct nss_ctx_instance *nss_ctx);
extern void nss_lso_rx_stats_sync(struct nss_ctx_instance *nss_ctx, struct nss_lso_rx_stats_sync *nlrss);
extern void nss_lso_rx_stats_dentry_create(void);

#endif /* __NSS_LSO_RX_STATS_H */

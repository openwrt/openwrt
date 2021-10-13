/*
 **************************************************************************
 * Copyright (c) 2017, 2020, The Linux Foundation. All rights reserved.
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

#ifndef __NSS_ETH_RX_STATS_H
#define __NSS_ETH_RX_STATS_H

#include <nss_cmn.h>

/*
 * nss_eth_rx_stats.h
 *	NSS driver ETH_RX statistics header file.
 */

/*
 * Request/Response types
 */
enum nss_eth_rx_metadata_types {
	NSS_RX_METADATA_TYPE_ETH_RX_STATS_SYNC,
	NSS_METADATA_TYPE_ETH_RX_MAX,
};

/*
 * The NSS eth_rx node stats structure.
 */
struct nss_eth_rx_node_sync {
	struct nss_cmn_node_stats node_stats;
				/* Common node stats for ETH_RX */
	uint32_t total_ticks;		/* Total clock ticks spend inside the eth_rx */
	uint32_t worst_case_ticks;	/* Worst case iteration of the eth_rx in ticks */
	uint32_t iterations;		/* Number of iterations around the eth_rx */
	uint32_t exception_events[NSS_ETH_RX_EXCEPTION_EVENT_MAX];
				/* Number of ETH_RX exception events */
};

/*
 * Message structure to send/receive eth_rx commands
 */
struct nss_eth_rx_msg {
	struct nss_cmn_msg cm;		/* Message Header */
	union {
		struct nss_eth_rx_node_sync node_sync;	/* Message: node statistics sync */
	} msg;
};

/*
 * eth_rx statistics APIs
 */
extern void nss_eth_rx_stats_notify(struct nss_ctx_instance *nss_ctx);
extern void nss_eth_rx_metadata_stats_sync(struct nss_ctx_instance *nss_ctx, struct nss_eth_rx_node_sync *nens);
extern void nss_eth_rx_stats_dentry_create(void);

#endif /* __NSS_ETH_RX_STATS_H */

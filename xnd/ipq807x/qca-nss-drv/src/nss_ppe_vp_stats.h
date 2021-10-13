/*
 **************************************************************************
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
 **************************************************************************
 */

/*
 * nss_ppe_vp_stats.h
 *	NSS PPE-VP statistics header file.
 */

#ifndef __NSS_PPE_VP_STATS_H
#define __NSS_PPE_VP_STATS_H

/*
 * NSS PPE-VP statistics
 */
struct nss_ppe_vp_statistics_debug {
	uint64_t ppe_port_num;			/* VP number */
	uint64_t nss_if;			/* NSS interface number corresponding to VP */
	uint64_t rx_packets;                    /* Number of packets received. */
	uint64_t rx_bytes;                    	/* Number of bytes received. */
	uint64_t tx_packets;                    /* Number of packets transmitted. */
	uint64_t tx_bytes;                    	/* Number of bytes transmitted. */
	uint64_t rx_inactive_drop;		/* Number of packets dropped from PPE to VP due to VP inactive */
	uint64_t tx_inactive_drop;		/* Number of packets dropped from VP to PPE due to VP inactive */
	uint64_t packet_big_err;		/* Number of packets not sent to PPE because packet was too large */
	uint64_t tx_dropped[NSS_MAX_NUM_PRI];   /* Tx packets dropped on due to queue full. */
};

/*
 * NSS PPE-VP statistics
 */
struct nss_ppe_vp_stats_debug {
	uint64_t rx_dropped[NSS_MAX_NUM_PRI];   /* Packets dropped on receive due to queue full. */
	struct nss_ppe_vp_statistics_debug vp_stats[NSS_PPE_VP_MAX_NUM];
						/* Per VP Tx and Rx stats. */
};

/*
 * Data structures to store NSS PPE_VP debug statistics
 */
extern struct nss_ppe_vp_stats_debug nss_ppe_vp_debug_stats;

/*
 * NSS PPE-VP statistics APIs
 */
extern void nss_ppe_vp_stats_sync(struct nss_ctx_instance *nss_ctx, struct nss_ppe_vp_sync_stats_msg *stats_msg, uint16_t if_num);
extern struct dentry *nss_ppe_vp_stats_dentry_create(void);

#endif /* __NSS_PPE_VP_STATS_H */

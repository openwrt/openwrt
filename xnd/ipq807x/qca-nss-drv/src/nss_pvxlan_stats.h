/*
 ******************************************************************************
 * Copyright (c) 2019, The Linux Foundation. All rights reserved.
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
 * ****************************************************************************
 */

#ifndef __NSS_PVXLAN_STATS_H
#define __NSS_PVXLAN_STATS_H

/*
 * pvxlan statistic counters
 */
enum nss_pvxlan_tunnel_stats {
	NSS_PVXLAN_TUNNEL_STATS_RX_PKTS,
	NSS_PVXLAN_TUNNEL_STATS_RX_BYTES,
	NSS_PVXLAN_TUNNEL_STATS_TX_PKTS,
	NSS_PVXLAN_TUNNEL_STATS_TX_BYTES,
	NSS_PVXLAN_TUNNEL_STATS_RX_QUEUE_0_DROPPED,
	NSS_PVXLAN_TUNNEL_STATS_RX_QUEUE_1_DROPPED,
	NSS_PVXLAN_TUNNEL_STATS_RX_QUEUE_2_DROPPED,
	NSS_PVXLAN_TUNNEL_STATS_RX_QUEUE_3_DROPPED,
	NSS_PVXLAN_TUNNEL_STATS_MAC_DB_LOOKUP_FAILED,
	NSS_PVXLAN_TUNNEL_STATS_UDP_ENCAP_LOOKUP_FAILED,
	NSS_PVXLAN_TUNNEL_STATS_DROP_MALFORMED,
	NSS_PVXLAN_TUNNEL_STATS_DROP_NEXT_NODE_QUEUE_FULL,
	NSS_PVXLAN_TUNNEL_STATS_DROP_HEADROOM_INSUFFICIENT,
	NSS_PVXLAN_TUNNEL_STATS_DROP_VERSION_MISMATCH,
	NSS_PVXLAN_TUNNEL_STATS_DROP_ZERO_SIZED_PACKET,
	NSS_PVXLAN_TUNNEL_STATS_DROP_PBUF_ALLOC_FAILED,
	NSS_PVXLAN_TUNNEL_STATS_DROP_LINEAR_FAILED,
	NSS_PVXLAN_TUNNEL_STATS_MAX,
};

/*
 * PVxLAN session debug statistics
 */
struct nss_pvxlan_tunnel_stats_debug {
	uint64_t stats[NSS_PVXLAN_TUNNEL_STATS_MAX];
	int32_t if_index;
	uint32_t if_num; /* nss interface number */
	bool valid;
};

/*
 * Data structures to store PVxLAN nss debug stats
 */
extern spinlock_t nss_pvxlan_tunnel_stats_debug_lock;
extern struct nss_pvxlan_tunnel_stats_debug nss_pvxlan_tunnel_debug_stats[NSS_PVXLAN_MAX_INTERFACES];

/*
 * PVxLAN statistics APIs
 */
extern void nss_pvxlan_stats_sync(struct nss_ctx_instance *nss_ctx, struct nss_pvxlan_stats_msg *stats_msg, uint32_t if_num);
extern void nss_pvxlan_stats_dentry_create(void);

#endif /* __NSS_PVXLAN_STATS_H */

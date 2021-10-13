/*
 **************************************************************************
 * Copyright (c) 2019-2020, The Linux Foundation. All rights reserved.
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

#include "nss_tx_rx_common.h"
#include "nss_pvxlan_stats.h"

DEFINE_SPINLOCK(nss_pvxlan_tunnel_stats_debug_lock);
struct nss_pvxlan_tunnel_stats_debug nss_pvxlan_tunnel_debug_stats[NSS_PVXLAN_MAX_INTERFACES];

/*
 * nss_pvxlan_tunnel_stats_debug_str
 *	PVxLAN statistics strings for nss tunnel stats
 */
static int8_t *nss_pvxlan_tunnel_stats_debug_str[NSS_PVXLAN_MAX_INTERFACES] = {
	"rx_pkts",
	"rx_bytes",
	"tx_pkts",
	"tx_bytes",
	"rx_queue_0_dropped",
	"rx_queue_1_dropped",
	"rx_queue_2_dropped",
	"rx_queue_3_dropped",
	"MAC DB look up failed",
	"UDP ENCAP look up failed",
	"dropped packet malformed",
	"dropped next node queue is full",
	"dropped headroom insufficient",
	"dropped version mismatch",
	"dropped zero sized packet",
	"dropped pbuf alloc failed",
	"dropped linearization failed"
};

/*
 * nss_pvxlan_tunnel_stats_debug_get()
 *	Get PVxLAN Tunnel statitics.
 */
static void nss_pvxlan_tunnel_stats_debug_get(struct nss_pvxlan_tunnel_stats_debug *stats)
{
	uint32_t i;

	if (!stats) {
		nss_warning("No memory to copy pvxlan tunnel stats");
		return;
	}

	spin_lock_bh(&nss_pvxlan_tunnel_stats_debug_lock);
	for (i = 0; i < NSS_PVXLAN_MAX_INTERFACES; i++) {
		if (nss_pvxlan_tunnel_debug_stats[i].valid) {
			memcpy(stats, &nss_pvxlan_tunnel_debug_stats[i],
				sizeof(struct nss_pvxlan_tunnel_stats_debug));
			stats++;
		}
	}
	spin_unlock_bh(&nss_pvxlan_tunnel_stats_debug_lock);
}

/*
 * nss_pvxlan_stats_read()
 *	Read PVxLAN Tunnel statistics
 */
static ssize_t nss_pvxlan_stats_read(struct file *fp, char __user *ubuf,
					size_t sz, loff_t *ppos)
{
	uint32_t max_output_lines = 2 + (NSS_PVXLAN_MAX_INTERFACES
					* (NSS_PVXLAN_TUNNEL_STATS_MAX + 2)) + 2;
	size_t size_al = NSS_STATS_MAX_STR_LENGTH * max_output_lines;
	size_t size_wr = 0;
	ssize_t bytes_read = 0;
	struct net_device *dev;
	uint32_t id, i;
	struct nss_pvxlan_tunnel_stats_debug *pvxlan_tunnel_stats = NULL;

	char *lbuf = kzalloc(size_al, GFP_KERNEL);
	if (unlikely(lbuf == NULL)) {
		nss_warning("Could not allocate memory for local statistics buffer");
		return 0;
	}

	pvxlan_tunnel_stats = kzalloc((sizeof(struct nss_pvxlan_tunnel_stats_debug)
						* NSS_PVXLAN_MAX_INTERFACES), GFP_KERNEL);
	if (unlikely(!pvxlan_tunnel_stats)) {
		nss_warning("Could not allocate memory for populating PVxLAN stats");
		kfree(lbuf);
		return 0;
	}

	/*
	 * Get all stats
	 */
	nss_pvxlan_tunnel_stats_debug_get(pvxlan_tunnel_stats);

	/*
	 * Tunnel stats
	 */
	size_wr += scnprintf(lbuf + size_wr, size_al - size_wr,
				"\n PVxLAN Tunnel stats start:\n\n");

	for (id = 0; id < NSS_PVXLAN_MAX_INTERFACES; id++) {
		if (!pvxlan_tunnel_stats[id].valid)
			break;

		dev = dev_get_by_index(&init_net, pvxlan_tunnel_stats[id].if_index);
		if (likely(dev)) {
			size_wr += scnprintf(lbuf + size_wr, size_al - size_wr,
						"%d. nss interface id=%d, netdevice=%s\n",
						id, pvxlan_tunnel_stats[id].if_num,
						dev->name);
			dev_put(dev);
		} else {
			size_wr += scnprintf(lbuf + size_wr, size_al - size_wr,
						"%d. nss interface id=%d\n", id,
						pvxlan_tunnel_stats[id].if_num);
		}

		for (i = 0; i < NSS_PVXLAN_TUNNEL_STATS_MAX; i++) {
			size_wr += scnprintf(lbuf + size_wr, size_al - size_wr,
						"\t%s = %llu\n",
						nss_pvxlan_tunnel_stats_debug_str[i],
						pvxlan_tunnel_stats[id].stats[i]);
		}

		size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, "\n");
	}

	size_wr += scnprintf(lbuf + size_wr, size_al - size_wr,
				"\n PVxLAN Tunnel stats end\n");
	bytes_read = simple_read_from_buffer(ubuf, sz, ppos, lbuf, size_wr);

	kfree(pvxlan_tunnel_stats);
	kfree(lbuf);
	return bytes_read;
}

/*
 * nss_pvxlan_stats_sync()
 *	Sync function for pvxlan statistics
 */
void nss_pvxlan_stats_sync(struct nss_ctx_instance *nss_ctx, struct nss_pvxlan_stats_msg *stats_msg, uint32_t if_num)
{
	uint32_t i;
	struct nss_pvxlan_tunnel_stats_debug *s = NULL;

	NSS_VERIFY_CTX_MAGIC(nss_ctx);

	spin_lock_bh(&nss_pvxlan_tunnel_stats_debug_lock);
	for (i = 0; i < NSS_PVXLAN_MAX_INTERFACES; i++) {
		if (nss_pvxlan_tunnel_debug_stats[i].if_num == if_num) {
			s = &nss_pvxlan_tunnel_debug_stats[i];
			break;
		}
	}

	if (!s) {
		spin_unlock_bh(&nss_pvxlan_tunnel_stats_debug_lock);
		nss_warning("%px: Tunnel not found: %u", nss_ctx, if_num);
		return;
	}

	s->stats[NSS_PVXLAN_TUNNEL_STATS_RX_PKTS] += stats_msg->node_stats.rx_packets;
	s->stats[NSS_PVXLAN_TUNNEL_STATS_RX_BYTES] += stats_msg->node_stats.rx_bytes;
	s->stats[NSS_PVXLAN_TUNNEL_STATS_TX_PKTS] += stats_msg->node_stats.tx_packets;
	s->stats[NSS_PVXLAN_TUNNEL_STATS_TX_BYTES] += stats_msg->node_stats.tx_bytes;
	for (i = 0; i < NSS_MAX_NUM_PRI; i++) {
		s->stats[NSS_PVXLAN_TUNNEL_STATS_RX_QUEUE_0_DROPPED + i] += stats_msg->node_stats.rx_dropped[i];
	}
	s->stats[NSS_PVXLAN_TUNNEL_STATS_MAC_DB_LOOKUP_FAILED] +=
				 stats_msg->mac_db_lookup_failed;
	s->stats[NSS_PVXLAN_TUNNEL_STATS_UDP_ENCAP_LOOKUP_FAILED] +=
				stats_msg->udp_encap_lookup_failed;
	s->stats[NSS_PVXLAN_TUNNEL_STATS_DROP_MALFORMED] +=
		stats_msg->dropped_malformed;
	s->stats[NSS_PVXLAN_TUNNEL_STATS_DROP_NEXT_NODE_QUEUE_FULL] +=
		stats_msg->dropped_next_node_queue_full;
	s->stats[NSS_PVXLAN_TUNNEL_STATS_DROP_HEADROOM_INSUFFICIENT] +=
		stats_msg->dropped_hroom;
	s->stats[NSS_PVXLAN_TUNNEL_STATS_DROP_VERSION_MISMATCH] +=
		stats_msg->dropped_ver_mis;
	s->stats[NSS_PVXLAN_TUNNEL_STATS_DROP_ZERO_SIZED_PACKET] +=
		stats_msg->dropped_zero_sized_packet;
	s->stats[NSS_PVXLAN_TUNNEL_STATS_DROP_PBUF_ALLOC_FAILED] +=
		stats_msg->dropped_pbuf_alloc_failed;
	s->stats[NSS_PVXLAN_TUNNEL_STATS_DROP_LINEAR_FAILED] +=
		stats_msg->dropped_linear_failed;
	spin_unlock_bh(&nss_pvxlan_tunnel_stats_debug_lock);
}

/*
 * nss_pvxlan_stats_ops
 */
NSS_STATS_DECLARE_FILE_OPERATIONS(pvxlan)

/*
 * nss_pvxlan_stats_dentry_create()
 *	Create gre tunnel statistics debug entry.
 */
void nss_pvxlan_stats_dentry_create(void)
{
	nss_stats_create_dentry("pvxlan", &nss_pvxlan_stats_ops);
}

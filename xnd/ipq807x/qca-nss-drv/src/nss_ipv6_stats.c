/*
 **************************************************************************
 * Copyright (c) 2016-2017, 2019-2021, The Linux Foundation. All rights reserved.
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

#include "nss_core.h"
#include <nss_ipv6.h>
#include "nss_ipv6_stats.h"
#include "nss_ipv6_strings.h"

/*
 * Declare atomic notifier data structure for statistics.
 */
ATOMIC_NOTIFIER_HEAD(nss_ipv6_stats_notifier);

uint64_t nss_ipv6_stats[NSS_IPV6_STATS_MAX];
uint64_t nss_ipv6_exception_stats[NSS_IPV6_EXCEPTION_EVENT_MAX];

/*
 * nss_ipv6_stats_read()
 *	Read IPV6 stats.
 */
static ssize_t nss_ipv6_stats_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	int32_t i;

	/*
	 * max output lines = #stats + Number of Extra outputlines for future reference to add new stats +
	 * start tag line + end tag line + three blank lines.
	 */
	uint32_t max_output_lines = NSS_STATS_NODE_MAX + NSS_IPV6_STATS_MAX + NSS_IPV6_EXCEPTION_EVENT_MAX + NSS_STATS_EXTRA_OUTPUT_LINES;
	size_t size_al = NSS_STATS_MAX_STR_LENGTH * max_output_lines;
	size_t size_wr = 0;
	ssize_t bytes_read = 0;
	uint64_t *stats_shadow;

	char *lbuf = kzalloc(size_al, GFP_KERNEL);
	if (unlikely(lbuf == NULL)) {
		nss_warning("Could not allocate memory for local statistics buffer");
		return 0;
	}

	/*
	 * Note: The assumption here is that exception event count is larger than other statistics count for IPv6.
	 */
	stats_shadow = kzalloc(NSS_IPV6_EXCEPTION_EVENT_MAX * 8, GFP_KERNEL);
	if (unlikely(stats_shadow == NULL)) {
		nss_warning("Could not allocate memory for local shadow buffer");
		kfree(lbuf);
		return 0;
	}

	size_wr += nss_stats_banner(lbuf, size_wr, size_al, "ipv6", NSS_STATS_SINGLE_CORE);
	size_wr += nss_stats_fill_common_stats(NSS_IPV6_RX_INTERFACE, NSS_STATS_SINGLE_INSTANCE, lbuf, size_wr, size_al, "ipv6");

	/*
	 * IPv6 node stats
	 */
	spin_lock_bh(&nss_top_main.stats_lock);
	for (i = 0; (i < NSS_IPV6_STATS_MAX); i++) {
		stats_shadow[i] = nss_ipv6_stats[i];
	}

	spin_unlock_bh(&nss_top_main.stats_lock);

	size_wr += nss_stats_print("ipv6", "ipv6 node stats", NSS_STATS_SINGLE_INSTANCE
					, nss_ipv6_strings_stats
					, stats_shadow
					, NSS_IPV6_STATS_MAX
					, lbuf, size_wr, size_al);

	/*
	 * Exception stats
	 */
	spin_lock_bh(&nss_top_main.stats_lock);
	for (i = 0; (i < NSS_IPV6_EXCEPTION_EVENT_MAX); i++) {
		stats_shadow[i] = nss_ipv6_exception_stats[i];
	}
	spin_unlock_bh(&nss_top_main.stats_lock);

	size_wr += nss_stats_print("ipv6", "ipv6 exception stats", NSS_STATS_SINGLE_INSTANCE
					, nss_ipv6_strings_exception_stats
					, stats_shadow
					, NSS_IPV6_EXCEPTION_EVENT_MAX
					, lbuf, size_wr, size_al);
	bytes_read = simple_read_from_buffer(ubuf, sz, ppos, lbuf, strlen(lbuf));
	kfree(lbuf);
	kfree(stats_shadow);

	return bytes_read;
}

/*
 * nss_ipv6_stats_conn_sync()
 *	Update driver specific information from the messsage.
 */
void nss_ipv6_stats_conn_sync(struct nss_ctx_instance *nss_ctx, struct nss_ipv6_conn_sync *nics)
{
	struct nss_top_instance *nss_top = nss_ctx->nss_top;

	/*
	 * Update statistics maintained by NSS driver
	 */
	spin_lock_bh(&nss_top->stats_lock);
	nss_ipv6_stats[NSS_IPV6_STATS_ACCELERATED_RX_PKTS] += nics->flow_rx_packet_count + nics->return_rx_packet_count;
	nss_ipv6_stats[NSS_IPV6_STATS_ACCELERATED_RX_BYTES] += nics->flow_rx_byte_count + nics->return_rx_byte_count;
	nss_ipv6_stats[NSS_IPV6_STATS_ACCELERATED_TX_PKTS] += nics->flow_tx_packet_count + nics->return_tx_packet_count;
	nss_ipv6_stats[NSS_IPV6_STATS_ACCELERATED_TX_BYTES] += nics->flow_tx_byte_count + nics->return_tx_byte_count;
	spin_unlock_bh(&nss_top->stats_lock);
}

/*
 * nss_ipv6_stats_conn_sync_many()
 *	Update driver specific information from the conn_sync_many messsage.
 */
void nss_ipv6_stats_conn_sync_many(struct nss_ctx_instance *nss_ctx, struct nss_ipv6_conn_sync_many_msg *nicsm)
{
	uint32_t i;

	/*
	 * Sanity check for the stats count
	 */
	if (nicsm->count * sizeof(struct nss_ipv6_conn_sync) >= nicsm->size) {
		nss_warning("%px: stats sync count %u exceeds the size of this msg %u", nss_ctx, nicsm->count, nicsm->size);
		return;
	}

	for (i = 0; i < nicsm->count; i++) {
		nss_ipv6_stats_conn_sync(nss_ctx, &nicsm->conn_sync[i]);
	}
}

/*
 * nss_ipv6_stats_node_sync()
 *	Update driver specific information from the messsage.
 */
void nss_ipv6_stats_node_sync(struct nss_ctx_instance *nss_ctx, struct nss_ipv6_node_sync *nins)
{
	struct nss_top_instance *nss_top = nss_ctx->nss_top;
	uint32_t i;

	/*
	 * Update statistics maintained by NSS driver
	 */
	spin_lock_bh(&nss_top->stats_lock);
	nss_top->stats_node[NSS_IPV6_RX_INTERFACE][NSS_STATS_NODE_RX_PKTS] += nins->node_stats.rx_packets;
	nss_top->stats_node[NSS_IPV6_RX_INTERFACE][NSS_STATS_NODE_RX_BYTES] += nins->node_stats.rx_bytes;
	nss_top->stats_node[NSS_IPV6_RX_INTERFACE][NSS_STATS_NODE_TX_PKTS] += nins->node_stats.tx_packets;
	nss_top->stats_node[NSS_IPV6_RX_INTERFACE][NSS_STATS_NODE_TX_BYTES] += nins->node_stats.tx_bytes;

	for (i = 0; i < NSS_MAX_NUM_PRI; i++) {
		nss_top->stats_node[NSS_IPV6_RX_INTERFACE][NSS_STATS_NODE_RX_QUEUE_0_DROPPED + i] += nins->node_stats.rx_dropped[i];
	}

	nss_ipv6_stats[NSS_IPV6_STATS_CONNECTION_CREATE_REQUESTS] += nins->ipv6_connection_create_requests;
	nss_ipv6_stats[NSS_IPV6_STATS_CONNECTION_CREATE_COLLISIONS] += nins->ipv6_connection_create_collisions;
	nss_ipv6_stats[NSS_IPV6_STATS_CONNECTION_CREATE_INVALID_INTERFACE] += nins->ipv6_connection_create_invalid_interface;
	nss_ipv6_stats[NSS_IPV6_STATS_CONNECTION_DESTROY_REQUESTS] += nins->ipv6_connection_destroy_requests;
	nss_ipv6_stats[NSS_IPV6_STATS_CONNECTION_DESTROY_MISSES] += nins->ipv6_connection_destroy_misses;
	nss_ipv6_stats[NSS_IPV6_STATS_CONNECTION_HASH_HITS] += nins->ipv6_connection_hash_hits;
	nss_ipv6_stats[NSS_IPV6_STATS_CONNECTION_HASH_REORDERS] += nins->ipv6_connection_hash_reorders;
	nss_ipv6_stats[NSS_IPV6_STATS_CONNECTION_FLUSHES] += nins->ipv6_connection_flushes;
	nss_ipv6_stats[NSS_IPV6_STATS_CONNECTION_EVICTIONS] += nins->ipv6_connection_evictions;
	nss_ipv6_stats[NSS_IPV6_STATS_FRAGMENTATIONS] += nins->ipv6_fragmentations;
	nss_ipv6_stats[NSS_IPV6_STATS_FRAG_FAILS] += nins->ipv6_frag_fails;
	nss_ipv6_stats[NSS_IPV6_STATS_MC_CONNECTION_CREATE_REQUESTS] += nins->ipv6_mc_connection_create_requests;
	nss_ipv6_stats[NSS_IPV6_STATS_MC_CONNECTION_UPDATE_REQUESTS] += nins->ipv6_mc_connection_update_requests;
	nss_ipv6_stats[NSS_IPV6_STATS_MC_CONNECTION_CREATE_INVALID_INTERFACE] += nins->ipv6_mc_connection_create_invalid_interface;
	nss_ipv6_stats[NSS_IPV6_STATS_MC_CONNECTION_DESTROY_REQUESTS] += nins->ipv6_mc_connection_destroy_requests;
	nss_ipv6_stats[NSS_IPV6_STATS_MC_CONNECTION_DESTROY_MISSES] += nins->ipv6_mc_connection_destroy_misses;
	nss_ipv6_stats[NSS_IPV6_STATS_MC_CONNECTION_FLUSHES] += nins->ipv6_mc_connection_flushes;
	nss_ipv6_stats[NSS_IPV6_STATS_CONNECTION_CREATE_INVALID_MIRROR_IFNUM] += nins->ipv6_connection_create_invalid_mirror_ifnum;
	nss_ipv6_stats[NSS_IPV6_STATS_CONNECTION_CREATE_INVALID_MIRROR_IFTYPE] += nins->ipv6_connection_create_invalid_mirror_iftype;
	nss_ipv6_stats[NSS_IPV6_STATS_MIRROR_FAILURES] += nins->ipv6_mirror_failures;

	for (i = 0; i < NSS_IPV6_EXCEPTION_EVENT_MAX; i++) {
		nss_ipv6_exception_stats[i] += nins->exception_events[i];
	}
	spin_unlock_bh(&nss_top->stats_lock);
}

/*
 * nss_ipv6_stats_ops
 */
NSS_STATS_DECLARE_FILE_OPERATIONS(ipv6);

/*
 * nss_ipv6_stats_dentry_create()
 *	Create IPv6 statistics debug entry.
 */
void nss_ipv6_stats_dentry_create(void)
{
	nss_stats_create_dentry("ipv6", &nss_ipv6_stats_ops);
}

/*
 * nss_ipv6_stats_notify()
 *	Sends notifications to all the registered modules.
 *
 * Leverage NSS-FW statistics timing to update Netlink.
 */
void nss_ipv6_stats_notify(struct nss_ctx_instance *nss_ctx)
{
	struct nss_ipv6_stats_notification ipv6_stats;

	ipv6_stats.core_id = nss_ctx->id;
	memcpy(ipv6_stats.cmn_node_stats, nss_top_main.stats_node[NSS_IPV6_RX_INTERFACE], sizeof(ipv6_stats.cmn_node_stats));
	memcpy(ipv6_stats.special_stats, nss_ipv6_stats, sizeof(ipv6_stats.special_stats));
	memcpy(ipv6_stats.exception_stats, nss_ipv6_exception_stats, sizeof(ipv6_stats.exception_stats));

	atomic_notifier_call_chain(&nss_ipv6_stats_notifier, NSS_STATS_EVENT_NOTIFY, (void *)&ipv6_stats);
}

/*
 * nss_ipv6_stats_register_notifier()
 *      Registers statistics notifier.
 */
int nss_ipv6_stats_register_notifier(struct notifier_block *nb)
{
	return atomic_notifier_chain_register(&nss_ipv6_stats_notifier, nb);
}
EXPORT_SYMBOL(nss_ipv6_stats_register_notifier);

/*
 * nss_ipv6_stats_unregister_notifier()
 *	Deregisters statistics notifier.
 */
int nss_ipv6_stats_unregister_notifier(struct notifier_block *nb)
{
	return atomic_notifier_chain_unregister(&nss_ipv6_stats_notifier, nb);
}
EXPORT_SYMBOL(nss_ipv6_stats_unregister_notifier);

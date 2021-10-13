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

#include "nss_core.h"
#include "nss_ipv6_reasm_stats.h"
#include "nss_ipv6_reasm.h"
#include "nss_ipv6_reasm_strings.h"

/*
 * Declare atomic notifier data structure for statistics.
 */
ATOMIC_NOTIFIER_HEAD(nss_ipv6_reasm_stats_notifier);

uint64_t nss_ipv6_reasm_stats[NSS_IPV6_REASM_STATS_MAX]; /* IPv6 reasm statistics */

/*
 * nss_ipv6_reasm_stats_read()
 *	Read IPV6 reassembly stats
 */
static ssize_t nss_ipv6_reasm_stats_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	int32_t i;
	/*
	 * Max output lines = #stats + few blank lines for banner printing +
	 * Number of Extra outputlines for future reference to add new stats
	 */
	uint32_t max_output_lines = NSS_STATS_NODE_MAX + NSS_IPV6_REASM_STATS_MAX + NSS_STATS_EXTRA_OUTPUT_LINES;
	size_t size_al = NSS_STATS_MAX_STR_LENGTH * max_output_lines;
	size_t size_wr = 0;
	ssize_t bytes_read = 0;
	uint64_t *stats_shadow;

	char *lbuf = kzalloc(size_al, GFP_KERNEL);
	if (unlikely(lbuf == NULL)) {
		nss_warning("Could not allocate memory for local statistics buffer");
		return 0;
	}

	stats_shadow = kzalloc(NSS_IPV6_REASM_STATS_MAX * 8, GFP_KERNEL);
	if (unlikely(stats_shadow == NULL)) {
		nss_warning("Could not allocate memory for local shadow buffer");
		kfree(lbuf);
		return 0;
	}

	size_wr += nss_stats_banner(lbuf, size_wr, size_al, "ipv6_reasm", NSS_STATS_SINGLE_CORE);

	size_wr += nss_stats_fill_common_stats(NSS_IPV6_REASM_INTERFACE, NSS_STATS_SINGLE_INSTANCE, lbuf, size_wr, size_al, "ipv6_reasm");

	/*
	 * Ipv6 reasm node stats
	 */

	spin_lock_bh(&nss_top_main.stats_lock);
	for (i = 0; (i < NSS_IPV6_REASM_STATS_MAX); i++) {
		stats_shadow[i] = nss_ipv6_reasm_stats[i];
	}

	spin_unlock_bh(&nss_top_main.stats_lock);

	size_wr += nss_stats_print("ipv6_reasm", NULL, NSS_STATS_SINGLE_INSTANCE
					, nss_ipv6_reasm_strings_stats
					, stats_shadow
					, NSS_IPV6_REASM_STATS_MAX
					, lbuf, size_wr, size_al);
	bytes_read = simple_read_from_buffer(ubuf, sz, ppos, lbuf, strlen(lbuf));
	kfree(lbuf);
	kfree(stats_shadow);

	return bytes_read;
}

/*
 * nss_ipv6_reasm_stats_ops
 */
NSS_STATS_DECLARE_FILE_OPERATIONS(ipv6_reasm);

/*
 * nss_ipv6_reasm_stats_dentry_create()
 *	Create IPv6 reasm statistics debug entry.
 */
void nss_ipv6_reasm_stats_dentry_create(void)
{
	nss_stats_create_dentry("ipv6_reasm", &nss_ipv6_reasm_stats_ops);
}

/*
 * nss_ipv6_reasm_stats_sync()
 *	Update driver specific information from the messsage.
 */
void nss_ipv6_reasm_stats_sync(struct nss_ctx_instance *nss_ctx, struct nss_ipv6_reasm_stats_sync *nirs)
{
	struct nss_top_instance *nss_top = nss_ctx->nss_top;
	int j;

	spin_lock_bh(&nss_top->stats_lock);

	/*
	 * Common node stats
	 */
	nss_top->stats_node[NSS_IPV6_REASM_INTERFACE][NSS_STATS_NODE_RX_PKTS] += nirs->node_stats.rx_packets;
	nss_top->stats_node[NSS_IPV6_REASM_INTERFACE][NSS_STATS_NODE_RX_BYTES] += nirs->node_stats.rx_bytes;
	nss_top->stats_node[NSS_IPV6_REASM_INTERFACE][NSS_STATS_NODE_TX_PKTS] += nirs->node_stats.tx_packets;
	nss_top->stats_node[NSS_IPV6_REASM_INTERFACE][NSS_STATS_NODE_TX_BYTES] += nirs->node_stats.tx_bytes;

	for (j = 0; j < NSS_MAX_NUM_PRI; j++) {
		nss_top->stats_node[NSS_IPV6_REASM_INTERFACE][NSS_STATS_NODE_RX_QUEUE_0_DROPPED + j] += nirs->node_stats.rx_dropped[j];
	}

	/*
	 * IPv6 reasm node stats
	 */
	nss_ipv6_reasm_stats[NSS_IPV6_REASM_STATS_ALLOC_FAILS] += nirs->ipv6_reasm_alloc_fails;
	nss_ipv6_reasm_stats[NSS_IPV6_REASM_STATS_TIMEOUTS] += nirs->ipv6_reasm_timeouts;
	nss_ipv6_reasm_stats[NSS_IPV6_REASM_STATS_DISCARDS] += nirs->ipv6_reasm_discards;

	spin_unlock_bh(&nss_top->stats_lock);
}

/*
 * nss_ipv6_reasm_stats_notify()
 *	Sends notifications to the registered modules.
 *
 * Leverage NSS-FW statistics timing to update Netlink.
 */
void nss_ipv6_reasm_stats_notify(struct nss_ctx_instance *nss_ctx)
{
	struct nss_ipv6_reasm_stats_notification ipv6_reasm_stats;

	ipv6_reasm_stats.core_id = nss_ctx->id;
	memcpy(ipv6_reasm_stats.cmn_node_stats, nss_top_main.stats_node[NSS_IPV6_REASM_INTERFACE], sizeof(ipv6_reasm_stats.cmn_node_stats));
	memcpy(ipv6_reasm_stats.ipv6_reasm_stats, nss_ipv6_reasm_stats, sizeof(ipv6_reasm_stats.ipv6_reasm_stats));
	atomic_notifier_call_chain(&nss_ipv6_reasm_stats_notifier, NSS_STATS_EVENT_NOTIFY, (void *)&ipv6_reasm_stats);
}

/*
 * nss_ipv6_reasm_stats_register_notifier()
 *	Registers statistics notifier.
 */
int nss_ipv6_reasm_stats_register_notifier(struct notifier_block *nb)
{
	return atomic_notifier_chain_register(&nss_ipv6_reasm_stats_notifier, nb);
}
EXPORT_SYMBOL(nss_ipv6_reasm_stats_register_notifier);

/*
 * nss_ipv6_reasm_stats_unregister_notifier()
 *	Deregisters statistics notifier.
 */
int nss_ipv6_reasm_stats_unregister_notifier(struct notifier_block *nb)
{
	return atomic_notifier_chain_unregister(&nss_ipv6_reasm_stats_notifier, nb);
}
EXPORT_SYMBOL(nss_ipv6_reasm_stats_unregister_notifier);

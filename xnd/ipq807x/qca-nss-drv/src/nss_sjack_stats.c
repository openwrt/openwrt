/*
 **************************************************************************
 * Copyright (c) 2016-2017, 2019 The Linux Foundation. All rights reserved.
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
#include "nss_sjack_stats.h"

/*
 * nss_sjack_stats_read()
 *	Read SJACK stats
 */
static ssize_t nss_sjack_stats_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	/*
	 * max output lines = #stats + start tag line + end tag line + three blank lines
	 */
	uint32_t max_output_lines = NSS_STATS_NODE_MAX + 5;
	size_t size_al = NSS_STATS_MAX_STR_LENGTH * max_output_lines;
	size_t size_wr = 0;
	ssize_t bytes_read = 0;
	uint64_t *stats_shadow;

	char *lbuf = kzalloc(size_al, GFP_KERNEL);
	if (unlikely(lbuf == NULL)) {
		nss_warning("Could not allocate memory for local statistics buffer");
		return 0;
	}

	stats_shadow = kzalloc(NSS_STATS_NODE_MAX * 8, GFP_KERNEL);
	if (unlikely(stats_shadow == NULL)) {
		nss_warning("Could not allocate memory for local shadow buffer");
		kfree(lbuf);
		return 0;
	}
	size_wr += nss_stats_banner(lbuf, size_wr, size_al, "sjack", NSS_STATS_SINGLE_CORE);
	size_wr += nss_stats_fill_common_stats(NSS_SJACK_INTERFACE, NSS_STATS_SINGLE_INSTANCE, lbuf, size_wr, size_al, "sjack");

	bytes_read = simple_read_from_buffer(ubuf, sz, ppos, lbuf, strlen(lbuf));
	kfree(lbuf);
	kfree(stats_shadow);

	return bytes_read;
}

/*
 * nss_sjack_stats_ops
 */
NSS_STATS_DECLARE_FILE_OPERATIONS(sjack)

/*
 * nss_sjack_stats_dentry_create()
 *	Create SJACK node statistics debug entry.
 */
void nss_sjack_stats_dentry_create(void)
{
	nss_stats_create_dentry("sjack", &nss_sjack_stats_ops);
}

/*
 * nss_sjack_stats_node_sync()
 *	Update sjack node stats.
 */
void nss_sjack_stats_node_sync(struct nss_ctx_instance *nss_ctx, struct nss_sjack_stats_sync_msg *nins)
{
	struct nss_top_instance *nss_top = nss_ctx->nss_top;
	int j;

	/*
	 * Update SJACK node stats.
	 */
	spin_lock_bh(&nss_top->stats_lock);
	nss_top->stats_node[NSS_SJACK_INTERFACE][NSS_SJACK_STATS_RX_PKTS] += nins->node_stats.rx_packets;
	nss_top->stats_node[NSS_SJACK_INTERFACE][NSS_SJACK_STATS_RX_BYTES] += nins->node_stats.rx_bytes;
	nss_top->stats_node[NSS_SJACK_INTERFACE][NSS_SJACK_STATS_TX_PKTS] += nins->node_stats.tx_packets;
	nss_top->stats_node[NSS_SJACK_INTERFACE][NSS_SJACK_STATS_TX_BYTES] += nins->node_stats.tx_bytes;

	for (j = 0; j < NSS_MAX_NUM_PRI; j++) {
		nss_top->stats_node[NSS_SJACK_INTERFACE][NSS_SJACK_STATS_RX_QUEUE_0_DROPPED + j] += nins->node_stats.rx_dropped[j];
	}

	spin_unlock_bh(&nss_top->stats_lock);
}

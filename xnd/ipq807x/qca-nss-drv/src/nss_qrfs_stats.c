/*
 **************************************************************************
 * Copyright (c) 2018-2019, The Linux Foundation. All rights reserved.
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
#include "nss_qrfs_stats.h"

/*
 * Spinlock to protect QRFS statistics update/read
 */
DEFINE_SPINLOCK(nss_qrfs_stats_lock);

/*
 * nss_qrfs_stats_str
 *	QRFS stats strings
 */
struct nss_stats_info nss_qrfs_stats_str[NSS_QRFS_STATS_MAX] = {
	{"rx_pkts"		, NSS_STATS_TYPE_COMMON},
	{"rx_byts"		, NSS_STATS_TYPE_COMMON},
	{"tx_pkts"		, NSS_STATS_TYPE_COMMON},
	{"tx_byts"		, NSS_STATS_TYPE_COMMON},
	{"rx_queue[0]_drops"	, NSS_STATS_TYPE_DROP},
	{"rx_queue[1]_drops"	, NSS_STATS_TYPE_DROP},
	{"rx_queue[2]_drops"	, NSS_STATS_TYPE_DROP},
	{"rx_queue[3]_drops"	, NSS_STATS_TYPE_DROP},
	{"invalid_offset"	, NSS_STATS_TYPE_EXCEPTION},
	{"unknown_protocol"	, NSS_STATS_TYPE_EXCEPTION},
	{"ipv4_flow_rule_hits"	, NSS_STATS_TYPE_SPECIAL},
	{"ipv6_flow_rule_hits"	, NSS_STATS_TYPE_SPECIAL}
};

uint64_t nss_qrfs_stats[NSS_MAX_CORES][NSS_QRFS_STATS_MAX];

/*
 * nss_qrfs_stats_read()
 *	Read QRFS statistics.
 */
static ssize_t nss_qrfs_stats_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	int32_t i, core;

	/*
	 * Max output lines = #stats + few blank lines for banner printing +
	 * Number of Extra outputlines for future reference to add new stats.
	 */
	uint32_t max_output_lines = (NSS_QRFS_STATS_MAX + 3) * NSS_MAX_CORES + NSS_STATS_EXTRA_OUTPUT_LINES;
	size_t size_al = NSS_STATS_MAX_STR_LENGTH * max_output_lines;
	size_t size_wr = 0;
	ssize_t bytes_read = 0;
	uint64_t *stats_shadow;

	char *lbuf = kzalloc(size_al, GFP_KERNEL);
	if (unlikely(lbuf == NULL)) {
		nss_warning("Could not allocate memory for local statistics buffer");
		return 0;
	}

	stats_shadow = kzalloc(NSS_QRFS_STATS_MAX * 8, GFP_KERNEL);
	if (unlikely(stats_shadow == NULL)) {
		nss_warning("Could not allocate memory for local shadow buffer");
		kfree(lbuf);
		return 0;
	}

	size_wr += nss_stats_banner(lbuf, size_wr, size_al, "qrfs", NSS_STATS_SINGLE_CORE);
	/*
	 * QRFS statistics
	 */
	for (core = 0; core < nss_top_main.num_nss; core++) {
		spin_lock_bh(&nss_qrfs_stats_lock);
		for (i = 0; i < NSS_QRFS_STATS_MAX; i++) {
			stats_shadow[i] = nss_qrfs_stats[core][i];
		}
		spin_unlock_bh(&nss_qrfs_stats_lock);

		size_wr += nss_stats_print("qrfs", NULL, NSS_STATS_SINGLE_INSTANCE
						, nss_qrfs_stats_str
						, stats_shadow
						, NSS_QRFS_STATS_MAX
						, lbuf, size_wr, size_al);
	}

	bytes_read = simple_read_from_buffer(ubuf, sz, ppos, lbuf, strlen(lbuf));
	kfree(lbuf);
	kfree(stats_shadow);

	return bytes_read;
}

/*
 * nss_qrfs_stats_ops
 */
NSS_STATS_DECLARE_FILE_OPERATIONS(qrfs)

/*
 * nss_qrfs_stats_dentry_create()
 *	Create QRFS statistics debug entry.
 */
void nss_qrfs_stats_dentry_create(void)
{
	nss_stats_create_dentry("qrfs", &nss_qrfs_stats_ops);
}

/*
 * nss_qrfs_stats_sync()
 *	Handle the syncing of NSS QRFS statistics.
 */
void nss_qrfs_stats_sync(struct nss_ctx_instance *nss_ctx, struct nss_qrfs_stats_sync_msg *nqssm)
{
	int id = nss_ctx->id;
	int j;

	spin_lock_bh(&nss_qrfs_stats_lock);

	/*
	 * Common node stats
	 */
	nss_qrfs_stats[id][NSS_STATS_NODE_RX_PKTS] += nqssm->node_stats.rx_packets;
	nss_qrfs_stats[id][NSS_STATS_NODE_RX_BYTES] += nqssm->node_stats.rx_bytes;
	nss_qrfs_stats[id][NSS_STATS_NODE_TX_PKTS] += nqssm->node_stats.tx_packets;
	nss_qrfs_stats[id][NSS_STATS_NODE_TX_BYTES] += nqssm->node_stats.tx_bytes;

	for (j = 0; j < NSS_MAX_NUM_PRI; j++) {
		nss_qrfs_stats[id][NSS_STATS_NODE_RX_QUEUE_0_DROPPED + j] += nqssm->node_stats.rx_dropped[j];
	}

	/*
	 * QRFS statistics
	 */
	nss_qrfs_stats[id][NSS_QRFS_STATS_INVALID_OFFSET] += nqssm->invalid_offset;
	nss_qrfs_stats[id][NSS_QRFS_STATS_UNKNOWN_PROTO] += nqssm->unknown_protocol;
	nss_qrfs_stats[id][NSS_QRFS_STATS_IPV4_FLOW_HITS] += nqssm->ipv4_flow_rule_hits;
	nss_qrfs_stats[id][NSS_QRFS_STATS_IPV6_FLOW_HITS] += nqssm->ipv6_flow_rule_hits;

	spin_unlock_bh(&nss_qrfs_stats_lock);
}

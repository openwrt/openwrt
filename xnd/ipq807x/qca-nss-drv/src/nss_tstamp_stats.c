/*
 **************************************************************************
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
 **************************************************************************
 */

#include "nss_stats.h"
#include "nss_core.h"
#include "nss_tstamp_stats.h"

/*
 * Spinlock to protect TSTAMP statistics update/read
 */
DEFINE_SPINLOCK(nss_tstamp_stats_lock);

/*
 * nss_tstamp_stats_str
 *	TSTAMP stats strings
 */
struct nss_stats_info nss_tstamp_stats_str[NSS_TSTAMP_STATS_MAX] = {
	{"rx_packets"			, NSS_STATS_TYPE_COMMON},
	{"rx_bytes"			, NSS_STATS_TYPE_COMMON},
	{"tx_packets"			, NSS_STATS_TYPE_COMMON},
	{"tx_bytes"			, NSS_STATS_TYPE_COMMON},
	{"rx_queue_0_dropped"		, NSS_STATS_TYPE_DROP},
	{"rx_queue_1_dropped"		, NSS_STATS_TYPE_DROP},
	{"rx_queue_2_dropped"		, NSS_STATS_TYPE_DROP},
	{"rx_queue_3_dropped"		, NSS_STATS_TYPE_DROP},
	{"boomeranged"			, NSS_STATS_TYPE_SPECIAL},
	{"dropped_fail_enqueue"		, NSS_STATS_TYPE_DROP},
	{"dropped_fail_alloc"		, NSS_STATS_TYPE_DROP},
	{"dropped_fail_copy"		, NSS_STATS_TYPE_DROP},
	{"dropped_no_interface"		, NSS_STATS_TYPE_DROP},
	{"dropped_no_headroom"		, NSS_STATS_TYPE_DROP}
};

/*
 * nss_tstamp_stats
 *	tstamp statistics
 */
uint64_t nss_tstamp_stats[2][NSS_TSTAMP_STATS_MAX];

/*
 * nss_tstamp_stats_read()
 *	Read tstamp statistics
 */
static ssize_t nss_tstamp_stats_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	int32_t i, num;

	/*
	 * Max output lines = (#stats + tx or rx tag + two blank lines) * 2(TX and RX)  +
	 * start tag line + end tag line + three blank lines
	 */
	uint32_t max_output_lines = (NSS_TSTAMP_STATS_MAX + 3) * 2 + 5;
	size_t size_al = NSS_STATS_MAX_STR_LENGTH * max_output_lines;
	size_t size_wr = 0;
	ssize_t bytes_read = 0;
	uint64_t *stats_shadow;

	char *lbuf = kzalloc(size_al, GFP_KERNEL);
	if (unlikely(lbuf == NULL)) {
		nss_warning("Could not allocate memory for local statistics buffer");
		return -ENOMEM;
	}

	stats_shadow = kzalloc(NSS_TSTAMP_STATS_MAX * sizeof(uint64_t), GFP_KERNEL);
	if (unlikely(stats_shadow == NULL)) {
		nss_warning("Could not allocate memory for local shadow buffer");
		kfree(lbuf);
		return -ENOMEM;
	}
	size_wr += nss_stats_banner(lbuf, size_wr, size_al, "tstamp", NSS_STATS_SINGLE_CORE);
	/*
	 * TSTAMP statistics
	 */
	for (num = 0; num < 2; num++) {
		if (num == 0) {
			size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, "\ntstamp TX stats:\n\n");
		} else {
			size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, "\ntstamp RX stats:\n\n");
		}

		spin_lock_bh(&nss_tstamp_stats_lock);
		for (i = 0; i < NSS_TSTAMP_STATS_MAX; i++) {
			stats_shadow[i] = nss_tstamp_stats[num][i];
		}
		spin_unlock_bh(&nss_tstamp_stats_lock);
		size_wr += nss_stats_print("tstamp", NULL, NSS_STATS_SINGLE_INSTANCE
						, nss_tstamp_stats_str
						, stats_shadow
						, NSS_TSTAMP_STATS_MAX
						, lbuf, size_wr, size_al);
	}

	bytes_read = simple_read_from_buffer(ubuf, sz, ppos, lbuf, strlen(lbuf));
	kfree(lbuf);
	kfree(stats_shadow);

	return bytes_read;
}

/*
 * nss_tstamp_stats_ops
 */
NSS_STATS_DECLARE_FILE_OPERATIONS(tstamp)

/*
 * nss_tstamp_stats_dentry_create()
 *	Create tstamp statistics debug entry.
 */
void nss_tstamp_stats_dentry_create(void)
{
	nss_stats_create_dentry("tstamp", &nss_tstamp_stats_ops);
}

/*
 * nss_tstamp_stats_sync()
 *	Handle the syncing of NSS TSTAMP statistics.
 */
void nss_tstamp_stats_sync(struct nss_ctx_instance *nss_ctx, struct nss_tstamp_stats_msg *nts, uint32_t interface)
{
	int id, j;

	if (interface == NSS_TSTAMP_TX_INTERFACE) {
		id = 0;
	} else {
		id = 1;
	}

	spin_lock_bh(&nss_tstamp_stats_lock);

	/*
	 * Common node stats
	 */
	nss_tstamp_stats[id][NSS_STATS_NODE_RX_PKTS] += nts->node_stats.rx_packets;
	nss_tstamp_stats[id][NSS_STATS_NODE_RX_BYTES] += nts->node_stats.rx_bytes;
	nss_tstamp_stats[id][NSS_STATS_NODE_TX_PKTS] += nts->node_stats.tx_packets;
	nss_tstamp_stats[id][NSS_STATS_NODE_TX_BYTES] += nts->node_stats.tx_bytes;

	for (j = 0; j < NSS_MAX_NUM_PRI; j++) {
		nss_tstamp_stats[id][NSS_STATS_NODE_RX_QUEUE_0_DROPPED + j] += nts->node_stats.rx_dropped[j];
	}

	/*
	 * TSTAMP statistics
	 */
	nss_tstamp_stats[id][NSS_TSTAMP_STATS_BOOMERANGED] += nts->boomeranged;
	nss_tstamp_stats[id][NSS_TSTAMP_STATS_DROPPED_FAIL_ENQUEUE] += nts->dropped_fail_enqueue;
	nss_tstamp_stats[id][NSS_TSTAMP_STATS_DROPPED_FAIL_ALLOC] += nts->dropped_fail_alloc;
	nss_tstamp_stats[id][NSS_TSTAMP_STATS_DROPPED_FAIL_COPY] += nts->dropped_fail_copy;
	nss_tstamp_stats[id][NSS_TSTAMP_STATS_DROPPED_NO_INTERFACE] += nts->dropped_no_interface;
	nss_tstamp_stats[id][NSS_TSTAMP_STATS_DROPPED_NO_HEADROOM] += nts->dropped_no_headroom;
	spin_unlock_bh(&nss_tstamp_stats_lock);
}

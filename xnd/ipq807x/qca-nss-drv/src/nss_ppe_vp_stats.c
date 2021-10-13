/*
 **************************************************************************
 * Copyright (c) 2020-2021, The Linux Foundation. All rights reserved.
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

#include "nss_core.h"
#include "nss_ppe_vp.h"
#include "nss_ppe_vp_stats.h"

/*
 * nss_ppe_vp_stats_cntrs
 *	PPE VP stats counters displayed using debugfs
 */
enum nss_ppe_vp_stats_cntrs {
	NSS_PPE_VP_STATS_VP_NUM,
	NSS_PPE_VP_STATS_NSS_IF,
	NSS_PPE_VP_STATS_RX_PKTS,
	NSS_PPE_VP_STATS_RX_BYTES,
	NSS_PPE_VP_STATS_TX_PKTS,
	NSS_PPE_VP_STATS_TX_BYTES,
	NSS_PPE_VP_STATS_RX_INACTIVE,
	NSS_PPE_VP_STATS_TX_INACTIVE,
	NSS_PPE_VP_STATS_PACKET_BIG,
	NSS_PPE_VP_STATS_TX_Q_0_DROP,
	NSS_PPE_VP_STATS_TX_Q_1_DROP,
	NSS_PPE_VP_STATS_TX_Q_2_DROP,
	NSS_PPE_VP_STATS_TX_Q_3_DROP,
	NSS_PPE_VP_STATS_MAX
};

/*
 * nss_ppe_vp_stats_rx_cntrs
 *	PPE VP RX stats counters displayed using debugfs
 */
enum nss_ppe_vp_stats_rx_cntrs {
	NSS_PPE_VP_STATS_RX_Q_0_DROP,
	NSS_PPE_VP_STATS_RX_Q_1_DROP,
	NSS_PPE_VP_STATS_RX_Q_2_DROP,
	NSS_PPE_VP_STATS_RX_Q_3_DROP,
	NSS_PPE_VP_STATS_RX_MAX
};

/*
 * nss_ppe_vp_rx_stats_str
 *	PPE VP Rx statistics strings
 */
struct nss_stats_info nss_ppe_vp_stats_rx_str[NSS_PPE_VP_STATS_RX_MAX] = {
	{"rx_queue_0_drop"	, NSS_STATS_TYPE_DROP},
	{"rx_queue_1_drop"	, NSS_STATS_TYPE_DROP},
	{"rx_queue_2_drop"	, NSS_STATS_TYPE_DROP},
	{"rx_queue_3_drop"	, NSS_STATS_TYPE_DROP},
};

/*
 * nss_ppe_vp_stats_str
 *	PPE VP statistics strings
 */
struct nss_stats_info nss_ppe_vp_stats_str[NSS_PPE_VP_STATS_MAX] = {
	{"ppe_port_num"		, NSS_STATS_TYPE_SPECIAL},
	{"nss_if"		, NSS_STATS_TYPE_SPECIAL},
	{"rx_packets"		, NSS_STATS_TYPE_COMMON},
	{"rx_bytes"		, NSS_STATS_TYPE_COMMON},
	{"tx_packets"		, NSS_STATS_TYPE_COMMON},
	{"tx_bytes"		, NSS_STATS_TYPE_COMMON},
	{"rx_inactive"		, NSS_STATS_TYPE_DROP},
	{"tx_inactive"		, NSS_STATS_TYPE_DROP},
	{"packet_large_err"	, NSS_STATS_TYPE_EXCEPTION},
	{"tx_queue_0_drop"	, NSS_STATS_TYPE_DROP},
	{"tx_queue_1_drop"	, NSS_STATS_TYPE_DROP},
	{"tx_queue_2_drop"	, NSS_STATS_TYPE_DROP},
	{"tx_queue_3_drop"	, NSS_STATS_TYPE_DROP},
};

/*
 * nss_ppe_vp_stats_sync
 *	PPE VP sync statistics from NSS
 */
void nss_ppe_vp_stats_sync(struct nss_ctx_instance *nss_ctx, struct nss_ppe_vp_sync_stats_msg *stats_msg, uint16_t if_num)
{
	uint16_t count = stats_msg->count;
	uint16_t vp_index, i;

	spin_lock_bh(&nss_ppe_vp_stats_lock);

	/*
	 * Update general rx dropped stats.
	 */
	for (i = 0; i < NSS_MAX_NUM_PRI; i++) {
		nss_ppe_vp_debug_stats.rx_dropped[i] += stats_msg->rx_dropped[i];
	}

	/*
	 * Update per VP tx and rx stats.
	 */
	while (count) {
		count--;

		/*
		 * Update stats in global array
		 */
		vp_index = stats_msg->vp_stats[count].ppe_port_num - NSS_PPE_VP_START;
		nss_ppe_vp_debug_stats.vp_stats[vp_index].ppe_port_num = stats_msg->vp_stats[count].ppe_port_num;
		nss_ppe_vp_debug_stats.vp_stats[vp_index].nss_if = stats_msg->vp_stats[count].nss_if;
		nss_ppe_vp_debug_stats.vp_stats[vp_index].rx_packets += stats_msg->vp_stats[count].stats.rx_packets;
		nss_ppe_vp_debug_stats.vp_stats[vp_index].rx_bytes += stats_msg->vp_stats[count].stats.rx_bytes;
		nss_ppe_vp_debug_stats.vp_stats[vp_index].tx_packets += stats_msg->vp_stats[count].stats.tx_packets;
		nss_ppe_vp_debug_stats.vp_stats[vp_index].tx_bytes += stats_msg->vp_stats[count].stats.tx_bytes;
		nss_ppe_vp_debug_stats.vp_stats[vp_index].rx_inactive_drop += stats_msg->vp_stats[count].rx_drop;
		nss_ppe_vp_debug_stats.vp_stats[vp_index].tx_inactive_drop += stats_msg->vp_stats[count].tx_drop;
		nss_ppe_vp_debug_stats.vp_stats[vp_index].packet_big_err += stats_msg->vp_stats[count].packet_big_err;
		for (i = 0; i < NSS_MAX_NUM_PRI; i++) {
			nss_ppe_vp_debug_stats.vp_stats[vp_index].tx_dropped[i] += stats_msg->vp_stats[count].stats.rx_dropped[i];
		}

		nss_trace("sync count:%d ppe_port_num %d rx_packets %d tx_packets %d\n",
				count, stats_msg->vp_stats[count].ppe_port_num,
				stats_msg->vp_stats[count].stats.rx_packets,
				stats_msg->vp_stats[count].stats.tx_packets);
	}
	spin_unlock_bh(&nss_ppe_vp_stats_lock);
}

/*
 * nss_ppe_vp_stats_read()
 *	Read ppe vp statistics
 */
static ssize_t nss_ppe_vp_stats_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	int i;
	char *lbuf = NULL;
	size_t size_wr = 0;
	ssize_t bytes_read = 0;
	struct nss_ppe_vp_stats_debug *ppe_vp_stats;
	uint32_t max_output_lines = ((NSS_PPE_VP_STATS_RX_MAX + NSS_PPE_VP_STATS_MAX) * NSS_PPE_VP_MAX_NUM) + NSS_STATS_EXTRA_OUTPUT_LINES;
	size_t size_al = NSS_STATS_MAX_STR_LENGTH * max_output_lines;
	size_t stats_sz = sizeof(struct nss_ppe_vp_stats_debug);

	ppe_vp_stats = kzalloc(stats_sz, GFP_KERNEL);
	if (!ppe_vp_stats) {
		nss_warning("Could not allocate memory for local statistics buffer");
		return 0;
	}


	lbuf = kzalloc(size_al, GFP_KERNEL);
	if (unlikely(!lbuf)) {
		kfree(ppe_vp_stats);
		nss_warning("Could not allocate memory for local statistics buffer");
		return 0;
	}

	/*
	 * Get vp stats
	 */
	spin_lock_bh(&nss_ppe_vp_stats_lock);
	memcpy(ppe_vp_stats, &nss_ppe_vp_debug_stats, stats_sz);
	spin_unlock_bh(&nss_ppe_vp_stats_lock);

	/*
	 * VP stats
	 */
	size_wr += nss_stats_banner(lbuf, size_wr, size_al, "ppe_vp", NSS_STATS_SINGLE_CORE);

	/*
	 * Print Rx dropped.
	 */
	size_wr += nss_stats_print("ppe_vp", "ppe_vp rx dropped:"
			, NSS_STATS_SINGLE_INSTANCE
			, nss_ppe_vp_stats_rx_str
			, ppe_vp_stats->rx_dropped
			, NSS_PPE_VP_STATS_RX_MAX
			, lbuf, size_wr, size_al);

	/*
	 * Print individual VP stats
	 */
	for (i = 0; i < NSS_PPE_VP_MAX_NUM; i++) {
		if (!ppe_vp_stats->vp_stats[i].nss_if) {
			continue;
		}

		size_wr += nss_stats_print("ppe_vp", "ppe_vp stats"
				, NSS_STATS_SINGLE_INSTANCE
				, nss_ppe_vp_stats_str
				, (uint64_t *) &ppe_vp_stats->vp_stats[i]
				, NSS_PPE_VP_STATS_MAX
				, lbuf, size_wr, size_al);
	}

	bytes_read = simple_read_from_buffer(ubuf, sz, ppos, lbuf, size_wr);

	kfree(ppe_vp_stats);
	kfree(lbuf);
	return bytes_read;
}

/*
 * nss_ppe_vp_stats_ops
 */
NSS_STATS_DECLARE_FILE_OPERATIONS(ppe_vp)

/*
 * nss_ppe_vp_stats_dentry_create()
 *	Create PPE statistics debug entry.
 */
struct dentry *nss_ppe_vp_stats_dentry_create(void)
{
	struct dentry *ppe_vp_d = debugfs_create_file("ppe_vp", 0400, nss_top_main.stats_dentry,
					&nss_top_main, &nss_ppe_vp_stats_ops);
	if (unlikely(ppe_vp_d == NULL)) {
		nss_warning("Failed to create qca-nss-drv/stats/ppe_vp file");
		return NULL;
	}

	return ppe_vp_d;
}

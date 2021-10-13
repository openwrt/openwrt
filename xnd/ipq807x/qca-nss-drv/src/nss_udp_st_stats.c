/*
 **************************************************************************
 * Copyright (c) 2021, The Linux Foundation. All rights reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all copies.
 *
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
#include "nss_udp_st_stats.h"
#include "nss_udp_st_strings.h"

uint32_t nss_udp_st_errors[NSS_UDP_ST_ERROR_MAX];
uint32_t nss_udp_st_stats_time[NSS_UDP_ST_TEST_MAX][NSS_UDP_ST_STATS_TIME_MAX];

/*
 * nss_udp_st_stats_read()
 *	Read UDP_ST stats.
 */
static ssize_t nss_udp_st_stats_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	/*
	 * Max output lines = #stats * NSS_MAX_CORES  +
	 * few blank lines for banner printing + Number of Extra outputlines for future reference to add new stats
	 */
	uint32_t max_output_lines = NSS_STATS_NODE_MAX + NSS_UDP_ST_ERROR_MAX + NSS_STATS_EXTRA_OUTPUT_LINES;
	size_t size_al = NSS_STATS_MAX_STR_LENGTH * max_output_lines;
	size_t size_wr = 0;
	ssize_t bytes_read = 0;
	uint64_t *stats_shadow;
	uint32_t i;

	char *lbuf = kzalloc(size_al, GFP_KERNEL);
	if (unlikely(lbuf == NULL)) {
		nss_warning("Could not allocate memory for local statistics buffer");
		return 0;
	}

	/*
	 * Note: The assumption here is that we do not have more than 64 stats.
	 */
	stats_shadow = kzalloc(64 * 8, GFP_KERNEL);
	if (unlikely(stats_shadow == NULL)) {
		nss_warning("Could not allocate memory for local shadow buffer");
		kfree(lbuf);
		return 0;
	}

	size_wr += nss_stats_banner(lbuf, size_wr, size_al, "udp_st", NSS_STATS_SINGLE_CORE);

	size_wr += nss_stats_fill_common_stats(NSS_UDP_ST_INTERFACE, NSS_STATS_SINGLE_INSTANCE, lbuf, size_wr, size_al, "udp_st");

	/*
	 * Error stats
	 */
	spin_lock_bh(&nss_top_main.stats_lock);
	for (i = 0; (i < NSS_UDP_ST_ERROR_MAX); i++) {
		stats_shadow[i] = nss_udp_st_errors[i];
	}
	spin_unlock_bh(&nss_top_main.stats_lock);
	size_wr += nss_stats_print("udp_st", "udp_st error stats"
					, NSS_STATS_SINGLE_INSTANCE
					, nss_udp_st_strings_error_stats
					, stats_shadow
					, NSS_UDP_ST_ERROR_MAX
					, lbuf, size_wr, size_al);

	/*
	 * Rx time stats
	 */
	spin_lock_bh(&nss_top_main.stats_lock);
	for (i = 0; (i < NSS_UDP_ST_STATS_TIME_MAX); i++) {
		stats_shadow[i] = nss_udp_st_stats_time[NSS_UDP_ST_TEST_RX][i];
	}
	spin_unlock_bh(&nss_top_main.stats_lock);
	size_wr += nss_stats_print("udp_st", "udp_st Rx time stats (ms)"
					, NSS_STATS_SINGLE_INSTANCE
					, nss_udp_st_strings_rx_time_stats
					, stats_shadow
					, NSS_UDP_ST_STATS_TIME_MAX
					, lbuf, size_wr, size_al);

	/*
	 * Tx time stats
	 */
	spin_lock_bh(&nss_top_main.stats_lock);
	for (i = 0; (i < NSS_UDP_ST_STATS_TIME_MAX); i++) {
		stats_shadow[i] = nss_udp_st_stats_time[NSS_UDP_ST_TEST_TX][i];
	}
	spin_unlock_bh(&nss_top_main.stats_lock);
	size_wr += nss_stats_print("udp_st", "udp_st Tx time stats (ms)"
					, NSS_STATS_SINGLE_INSTANCE
					, nss_udp_st_strings_tx_time_stats
					, stats_shadow
					, NSS_UDP_ST_STATS_TIME_MAX
					, lbuf, size_wr, size_al);

	bytes_read = simple_read_from_buffer(ubuf, sz, ppos, lbuf, strlen(lbuf));
	kfree(lbuf);
	kfree(stats_shadow);

	return bytes_read;
}

/*
 * nss_udp_st_stats_ops.
 */
NSS_STATS_DECLARE_FILE_OPERATIONS(udp_st);

/*
 * nss_udp_st_stats_dentry_create()
 *	Create udp_st statistics debug entry.
 */
void nss_udp_st_stats_dentry_create(void)
{
	nss_stats_create_dentry("udp_st", &nss_udp_st_stats_ops);
}

/*
 * nss_udp_st_stats_reset()
 *	Reset the udp_st statistics.
 */
void nss_udp_st_stats_reset(uint32_t if_num)
{
	uint32_t i;

	/*
	 * Reset common node stats.
	 */
	nss_stats_reset_common_stats(if_num);

	/*
	 * Reset error stats.
	 */
	spin_lock_bh(&nss_top_main.stats_lock);
	for (i = 0; i < NSS_UDP_ST_ERROR_MAX; i++) {
		nss_udp_st_errors[i] = 0;
	}
	spin_unlock_bh(&nss_top_main.stats_lock);
}

/*
 * nss_udp_st_stats_sync()
 *	Handle the syncing of UDP_ST node statistics.
 */
void nss_udp_st_stats_sync(struct nss_ctx_instance *nss_ctx, struct nss_udp_st_stats *nus)
{
	struct nss_top_instance *nss_top = nss_ctx->nss_top;
	uint32_t i, j;

	spin_lock_bh(&nss_top->stats_lock);

	nss_top->stats_node[NSS_UDP_ST_INTERFACE][NSS_STATS_NODE_RX_PKTS] += nus->nstats.node_stats.rx_packets;
	nss_top->stats_node[NSS_UDP_ST_INTERFACE][NSS_STATS_NODE_RX_BYTES] += nus->nstats.node_stats.rx_bytes;
	nss_top->stats_node[NSS_UDP_ST_INTERFACE][NSS_STATS_NODE_TX_PKTS] += nus->nstats.node_stats.tx_packets;
	nss_top->stats_node[NSS_UDP_ST_INTERFACE][NSS_STATS_NODE_TX_BYTES] += nus->nstats.node_stats.tx_bytes;

	for (i = 0; i < NSS_UDP_ST_ERROR_MAX; i++) {
		nss_udp_st_errors[i] += nus->nstats.errors[i];
	}

	for (i = 0; i < NSS_UDP_ST_TEST_MAX; i++) {
		for (j = 0; j < NSS_UDP_ST_STATS_TIME_MAX; j++) {
			nss_udp_st_stats_time[i][j] = nus->time_stats[i][j];
		}
	}
	spin_unlock_bh(&nss_top->stats_lock);
}

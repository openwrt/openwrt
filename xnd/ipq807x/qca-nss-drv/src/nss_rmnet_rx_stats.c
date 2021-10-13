/*
 **************************************************************************
 * Copyright (c) 2019-2020, The Linux Foundation. All rights reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.

 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 **************************************************************************
 */

#include "nss_stats.h"
#include "nss_core.h"
#include "nss_rmnet_rx_stats.h"

/*
 * Data structure that holds the virtual interface context.
 */
extern struct nss_rmnet_rx_handle *rmnet_rx_handle[];

/*
 * Spinlock to protect the global data structure virt_handle.
 */
extern spinlock_t nss_rmnet_rx_lock;

/*
 * nss_rmnet_rx_stats_str
 *	rmnet_rx interface stats strings
 */
struct nss_stats_info nss_rmnet_rx_stats_str[NSS_RMNET_RX_STATS_MAX] = {
	{"rx_packets"			, NSS_STATS_TYPE_COMMON},
	{"rx_bytes"			, NSS_STATS_TYPE_COMMON},
	{"tx_packets"			, NSS_STATS_TYPE_COMMON},
	{"tx_bytes"			, NSS_STATS_TYPE_COMMON},
	{"rx_queue_0_dropped"		, NSS_STATS_TYPE_DROP},
	{"rx_queue_1_dropped"		, NSS_STATS_TYPE_DROP},
	{"rx_queue_2_dropped"		, NSS_STATS_TYPE_DROP},
	{"rx_queue_3_dropped"		, NSS_STATS_TYPE_DROP},
	{"enqueue failed"		, NSS_STATS_TYPE_DROP},
	{"no available channel"		, NSS_STATS_TYPE_SPECIAL},
	{"linear pbuf count"		, NSS_STATS_TYPE_SPECIAL},
	{"no pbuf to linear"		, NSS_STATS_TYPE_SPECIAL},
	{"no enough room"		, NSS_STATS_TYPE_SPECIAL},
	{"channel[0]"			, NSS_STATS_TYPE_SPECIAL},
	{"channel[1]"			, NSS_STATS_TYPE_SPECIAL},
	{"channel[2]"			, NSS_STATS_TYPE_SPECIAL},
	{"channel[3]"			, NSS_STATS_TYPE_SPECIAL},
	{"channel[4]"			, NSS_STATS_TYPE_SPECIAL},
	{"channel[5]"			, NSS_STATS_TYPE_SPECIAL},
	{"channel[6]"			, NSS_STATS_TYPE_SPECIAL},
	{"channel[7]"			, NSS_STATS_TYPE_SPECIAL},
	{"channel[8]"			, NSS_STATS_TYPE_SPECIAL},
	{"channel[9]"			, NSS_STATS_TYPE_SPECIAL},
	{"channel[10]"			, NSS_STATS_TYPE_SPECIAL},
	{"channel[11]"			, NSS_STATS_TYPE_SPECIAL},
	{"DMA full"			, NSS_STATS_TYPE_SPECIAL}
};

/*
 * nss_rmnet_rx_stats_get()
 *	Get rmnet_rx interface stats by interface number.
 */
static bool nss_rmnet_rx_stats_get(struct nss_ctx_instance *nss_ctx, uint32_t if_num, uint64_t *stats, bool is_base)
{
	int i;
	uint32_t if_num_curr = if_num;
	uint64_t *stats_local;

	if_num = if_num - NSS_DYNAMIC_IF_START;

	spin_lock_bh(&nss_rmnet_rx_lock);
	if (!rmnet_rx_handle[if_num]) {
		spin_unlock_bh(&nss_rmnet_rx_lock);
		return false;
	}

	if (if_num_curr == rmnet_rx_handle[if_num]->if_num_n2h) {
		stats_local = rmnet_rx_handle[if_num]->stats_n2h;
	} else {
		stats_local = rmnet_rx_handle[if_num]->stats_h2n;
	}

	for (i = 0; i < NSS_RMNET_RX_STATS_MAX; i++) {
		stats[i] = stats_local[i];
	}
	spin_unlock_bh(&nss_rmnet_rx_lock);

	return true;
}

/*
 * nss_rmnet_rx_stats_read()
 *	Read rmnet_rx statistics
 */
static ssize_t nss_rmnet_rx_stats_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	struct nss_stats_data *data = fp->private_data;
	struct nss_ctx_instance *nss_ctx = nss_rmnet_rx_get_context();
	int32_t if_num = NSS_DYNAMIC_IF_START;
	int32_t max_if_num = if_num + NSS_MAX_DYNAMIC_INTERFACES;
	uint32_t max_output_lines = ((NSS_RMNET_RX_STATS_MAX + 3) * NSS_MAX_DYNAMIC_INTERFACES)
									+ NSS_STATS_EXTRA_OUTPUT_LINES;
	size_t size_al = NSS_STATS_MAX_STR_LENGTH * max_output_lines;
	size_t size_wr = 0;
	ssize_t bytes_read = 0;
	uint64_t *stats_shadow;
	char *lbuf = kzalloc(size_al, GFP_KERNEL);
	if (unlikely(!lbuf)) {
		nss_warning("%px: Could not allocate memory for local statistics buffer", data);
		return 0;
	}

	stats_shadow = kzalloc(NSS_RMNET_RX_STATS_MAX * sizeof(uint64_t), GFP_KERNEL);
	if (unlikely(!stats_shadow)) {
		nss_warning("%px: Could not allocate memory for local shadow buffer", data);
		kfree(lbuf);
		return 0;
	}

	if (data) {
		if_num = data->if_num;
	}

	if (if_num > max_if_num) {
		return 0;
	}

	size_wr += nss_stats_banner(lbuf, size_wr, size_al, "rmnet_rx", NSS_STATS_SINGLE_CORE);

	/*
	 * Interface statistics for all interfaces.
	 */
	for (; if_num < max_if_num; if_num++) {

		if (!nss_rmnet_rx_stats_get(nss_ctx, if_num, stats_shadow, false)) {
			continue;
		}

		size_wr += nss_stats_print("rmnet_rx", "interface", if_num,
				nss_rmnet_rx_stats_str, stats_shadow, NSS_RMNET_RX_STATS_MAX,
				lbuf, size_wr, size_al);
	}

	bytes_read = simple_read_from_buffer(ubuf, sz, ppos, lbuf, strlen(lbuf));
	kfree(lbuf);
	kfree(stats_shadow);

	return bytes_read;
}

/*
 * nss_rmnet_rx_stats_ops
 */
NSS_STATS_DECLARE_FILE_OPERATIONS(rmnet_rx)

/*
 * nss_rmnet_rx_stats_dentry_create()
 *	Create rmnet_rx statistics debug entry.
 */
void nss_rmnet_rx_stats_dentry_create(void)
{
	nss_stats_create_dentry("rmnet_rx", &nss_rmnet_rx_stats_ops);
}

/*
 * nss_rmnet_rx_stats_sync()
 *	Sync stats from the NSS FW
 */
void nss_rmnet_rx_stats_sync(struct nss_rmnet_rx_handle *handle,
			     struct nss_rmnet_rx_stats *nwis, uint32_t if_num)
{
	int i;
	uint64_t *stats;
	spin_lock_bh(&nss_rmnet_rx_lock);
	if (if_num == handle->if_num_n2h) {
		stats = handle->stats_n2h;
	} else {
		stats = handle->stats_h2n;
	}

	stats[NSS_RMNET_RX_STATS_RX_PKTS] += nwis->node_stats.rx_packets;
	stats[NSS_RMNET_RX_STATS_RX_BYTES] += nwis->node_stats.rx_bytes;
	stats[NSS_RMNET_RX_STATS_TX_PKTS] += nwis->node_stats.tx_packets;
	stats[NSS_RMNET_RX_STATS_TX_BYTES] += nwis->node_stats.tx_bytes;

	for (i = 0; i < NSS_MAX_NUM_PRI; i++) {
		stats[NSS_RMNET_RX_STATS_QUEUE_0_DROPPED + i] += nwis->node_stats.rx_dropped[i];
	}

	stats[NSS_RMNET_RX_STATS_ENQUEUE_FAILED] += nwis->enqueue_failed;
	stats[NSS_RMNET_RX_STATS_NO_AVAIL_CHANNEL] += nwis->no_avail_channel;
	stats[NSS_RMNET_RX_STATS_NUM_LINEAR_PBUF] += nwis->num_linear_pbuf;
	stats[NSS_RMNET_RX_STATS_NO_PBUF_TO_LINEAR] += nwis->no_pbuf_to_linear;
	stats[NSS_RMNET_RX_STATS_NO_ENOUGH_ROOM] += nwis->no_enough_room;

	for (i = 0; i < NSS_RMNET_RX_CHANNEL_MAX; i++) {
		stats[NSS_RMNET_RX_STATS_USING_CHANNEL0 + i] += nwis->using_channel[i];
	}

	stats[NSS_RMNET_RX_STATS_DMA_FAILED] += nwis->dma_failed;
	spin_unlock_bh(&nss_rmnet_rx_lock);
}

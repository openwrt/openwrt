/*
 **************************************************************************
 * Copyright (c) 2016-2017, 2019, The Linux Foundation. All rights reserved.
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
#include "nss_virt_if_stats.h"

/*
 * Data structure that holds the virtual interface context.
 */
extern struct nss_virt_if_handle *nss_virt_if_handle_t[];

/*
 * Spinlock to protect the global data structure virt_handle.
 */
extern spinlock_t nss_virt_if_lock;

/*
 * nss_virt_if_base_node_stats_str
 *	virt_if base node stats strings
 */
static int8_t *nss_virt_if_base_node_stats_str[NSS_VIRT_IF_BASE_NODE_STATS_MAX] = {
	"active_interfaces",
	"ocm_alloc_failed",
	"ddr_alloc_failed",
};

/*
 * nss_virt_if_interface_stats_str
 *	virt_if interface stats strings
 */
static int8_t *nss_virt_if_interface_stats_str[NSS_VIRT_IF_INTERFACE_STATS_MAX] = {
	"rx_packets",
	"rx_bytes",
	"rx_dropped",
	"tx_packets",
	"tx_bytes",
	"tx_enqueue_failed",
	"shaper_enqueue_failed",
	"ocm_alloc_failed",
};

/*
 * nss_virt_if_base_node_stats_fill_row()
 *	Fill one row of virt_if base node stats.
 */
static int32_t nss_virt_if_base_node_stats_fill_row(char *line, int len, int start, struct nss_virt_if_base_node_stats *stats)
{
	uint64_t tcnt = 0;
	switch (start) {
	case NSS_VIRT_IF_BASE_NODE_STATS_ACTIVE_INTERFACES:
		tcnt = stats->active_interfaces;
		break;

	case NSS_VIRT_IF_BASE_NODE_STATS_OCM_ALLOC_FAILED:
		tcnt = stats->ocm_alloc_failed;
		break;

	case NSS_VIRT_IF_BASE_NODE_STATS_DDR_ALLOC_FAILED:
		tcnt = stats->ddr_alloc_failed;
		break;

	default:
		return 0;
	}

	return scnprintf(line, len, "%s = %llu\n", nss_virt_if_base_node_stats_str[start], tcnt);
}

/*
 * nss_virt_if_interface_stats_fill_row()
 *	Fill one row of virt_if interface stats.
 */
static int32_t nss_virt_if_interface_stats_fill_row(char *line, int len, int start, struct nss_virt_if_interface_stats *stats)
{
	uint64_t tcnt = 0;
	switch (start) {
	case NSS_VIRT_IF_INTERFACE_STATS_RX_PACKETS:
		tcnt = stats->node_stats.rx_packets;
		break;

	case NSS_VIRT_IF_INTERFACE_STATS_RX_BYTES:
		tcnt = stats->node_stats.rx_bytes;
		break;

	case NSS_VIRT_IF_INTERFACE_STATS_RX_DROPPED:
		tcnt = nss_cmn_rx_dropped_sum(&stats->node_stats);
		break;

	case NSS_VIRT_IF_INTERFACE_STATS_TX_PACKETS:
		tcnt = stats->node_stats.tx_packets;
		break;

	case NSS_VIRT_IF_INTERFACE_STATS_TX_BYTES:
		tcnt = stats->node_stats.tx_bytes;
		break;

	case NSS_VIRT_IF_INTERFACE_STATS_TX_ENQUEUE_FAILED:
		tcnt = stats->tx_enqueue_failed;
		break;

	case NSS_VIRT_IF_INTERFACE_STATS_SHAPER_ENQUEUE_FAILED:
		tcnt = stats->shaper_enqueue_failed;
		break;

	case NSS_VIRT_IF_INTERFACE_STATS_OCM_ALLOC_FAILED:
		tcnt = stats->ocm_alloc_failed;
		break;

	default:
		return 0;
	}

	return scnprintf(line, len, "%s = %llu\n", nss_virt_if_interface_stats_str[start], tcnt);
}

/*
 * nss_virt_if_stats_get()
 *	Get virt_if base node stats or interface stats by interface number.
 */
bool nss_virt_if_stats_get(struct nss_ctx_instance *nss_ctx, uint32_t if_num, void *stats, bool is_base)
{
	if (nss_virt_if_verify_if_num(if_num) == false) {
		return false;
	}

	/*
	 * Statistics for redir_h2n and redir_n2h are collected on redir_h2n in NSS.
	 */
	if (nss_dynamic_interface_get_type(nss_ctx, if_num) != NSS_DYNAMIC_INTERFACE_TYPE_GENERIC_REDIR_H2N)
		return false;

	if_num = if_num - NSS_DYNAMIC_IF_START;
	spin_lock_bh(&nss_virt_if_lock);
	if (!nss_virt_if_handle_t[if_num]) {
		spin_unlock_bh(&nss_virt_if_lock);
		return false;
	}

	/*
	 * Check if it is base node statistics or interface statistics.
	 */
	if (is_base) {
		memcpy((struct nss_virt_if_base_node_stats *)stats,
			&nss_virt_if_handle_t[if_num]->stats.base_stats,
			sizeof(struct nss_virt_if_base_node_stats));
	} else {
		memcpy((struct nss_virt_if_interface_stats *)stats,
			&nss_virt_if_handle_t[if_num]->stats.if_stats,
			sizeof(struct nss_virt_if_interface_stats));
	}

	spin_unlock_bh(&nss_virt_if_lock);
	return true;
}

/*
 * nss_virt_if_stats_read()
 *	Read virt_if statistics
 */
static ssize_t nss_virt_if_stats_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	struct nss_stats_data *data = fp->private_data;
	struct nss_ctx_instance *nss_ctx = nss_virt_if_get_context();
	int32_t if_num = NSS_DYNAMIC_IF_START;
	int32_t max_if_num = if_num + NSS_MAX_DYNAMIC_INTERFACES;
	size_t bytes = 0;
	ssize_t bytes_read = 0;
	char line[80];
	int start, end;
	int32_t if_num_valid = NSS_DYNAMIC_IF_START - 1;
	struct nss_virt_if_base_node_stats base_node_stats_local;
	struct nss_virt_if_interface_stats interface_stats_local;

	if (data) {
		if_num = data->if_num;
	}

	if (if_num > max_if_num) {
		return 0;
	}

	/*
	 * Interface statistics for all virtual interface pairs.
	 */
	for (; if_num < max_if_num; if_num++) {

		if (!nss_virt_if_stats_get(nss_ctx, if_num, &interface_stats_local, false))
			continue;

		bytes = scnprintf(line, sizeof(line), "if_num %d stats start:\n\n", if_num);
		if ((bytes_read + bytes) > sz)
			break;

		if (copy_to_user(ubuf + bytes_read, line, bytes) != 0)
			return -EFAULT;

		bytes_read += bytes;

		start = NSS_VIRT_IF_INTERFACE_STATS_RX_PACKETS;
		end = NSS_VIRT_IF_INTERFACE_STATS_MAX;
		while (bytes_read < sz && start < end) {
			bytes = nss_virt_if_interface_stats_fill_row(line, sizeof(line), start, &interface_stats_local);
			if (!bytes)
				break;

			if ((bytes_read + bytes) > sz)
				break;

			if (copy_to_user(ubuf + bytes_read, line, bytes) != 0)
				return -EFAULT;

			bytes_read += bytes;
			start++;
		}

		/*
		 * Save one valid interface number for base node statistics.
		 */
		if_num_valid = if_num;

		bytes = scnprintf(line, sizeof(line), "if_num %d stats end:\n\n", if_num);
		if (bytes_read > (sz - bytes))
			break;

		if (copy_to_user(ubuf + bytes_read, line, bytes) != 0)
			return -EFAULT;

		bytes_read += bytes;
	}

	/*
	 * Base node statistics.
	 */
	if (!nss_virt_if_stats_get(nss_ctx, if_num_valid, &base_node_stats_local, true))
		goto done;

	bytes = scnprintf(line, sizeof(line), "base node stats begin (shown on if_num %d):\n\n", if_num_valid);
	if ((bytes_read + bytes) > sz)
		goto done;

	if (copy_to_user(ubuf + bytes_read, line, bytes) != 0)
		return -EFAULT;

	bytes_read += bytes;

	start = NSS_VIRT_IF_BASE_NODE_STATS_ACTIVE_INTERFACES;
	end = NSS_VIRT_IF_BASE_NODE_STATS_MAX;
	while (bytes_read < sz && start < end) {
		bytes = nss_virt_if_base_node_stats_fill_row(line, sizeof(line), start, &base_node_stats_local);
		if (!bytes)
			break;

		if ((bytes_read + bytes) > sz)
			break;

		if (copy_to_user(ubuf + bytes_read, line, bytes) != 0)
			return -EFAULT;

		bytes_read += bytes;
		start++;
	}

	bytes = scnprintf(line, sizeof(line), "base node stats end.\n\n");
	if ((bytes_read + bytes) > sz)
		goto done;

	if (copy_to_user(ubuf + bytes_read, line, bytes) != 0)
		return -EFAULT;

	bytes_read += bytes;

done:
	if (bytes_read > 0) {
		*ppos = bytes_read;
	}

	if (data) {
		data->if_num = if_num;
	}

	return bytes_read;
}

/*
 * nss_virt_if_stats_ops
 */
NSS_STATS_DECLARE_FILE_OPERATIONS(virt_if)

/*
 * nss_virt_if_stats_dentry_create()
 *	Create virt_if statistics debug entry.
 */
void nss_virt_if_stats_dentry_create(void)
{
	nss_stats_create_dentry("virt_if", &nss_virt_if_stats_ops);
}

/*
 * nss_virt_if_stats_sync()
 *	Sync stats from the NSS FW
 */
void nss_virt_if_stats_sync(struct nss_virt_if_handle *handle,
				struct nss_virt_if_stats *nwis)
{
	struct nss_virt_if_stats *stats = &handle->stats;
	int i;

	spin_lock_bh(&nss_virt_if_lock);
	stats->if_stats.node_stats.rx_packets += nwis->if_stats.node_stats.rx_packets;
	stats->if_stats.node_stats.rx_bytes += nwis->if_stats.node_stats.rx_bytes;
	stats->if_stats.node_stats.tx_packets += nwis->if_stats.node_stats.tx_packets;
	stats->if_stats.node_stats.tx_bytes += nwis->if_stats.node_stats.tx_bytes;

	for (i = 0; i < NSS_MAX_NUM_PRI; i++) {
		stats->if_stats.node_stats.rx_dropped[i] += nwis->if_stats.node_stats.rx_dropped[i];
	}

	stats->if_stats.tx_enqueue_failed += nwis->if_stats.tx_enqueue_failed;
	stats->if_stats.shaper_enqueue_failed += nwis->if_stats.shaper_enqueue_failed;
	stats->if_stats.ocm_alloc_failed += nwis->if_stats.ocm_alloc_failed;

	stats->base_stats.active_interfaces = nwis->base_stats.active_interfaces;
	stats->base_stats.ocm_alloc_failed = nwis->base_stats.ocm_alloc_failed;
	stats->base_stats.ddr_alloc_failed = nwis->base_stats.ddr_alloc_failed;
	spin_unlock_bh(&nss_virt_if_lock);
}

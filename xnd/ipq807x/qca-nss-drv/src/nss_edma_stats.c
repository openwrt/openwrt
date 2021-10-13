/*
 **************************************************************************
 * Copyright (c) 2017-2021, The Linux Foundation. All rights reserved.
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

/*
 * nss_edma_stats.c
 *	NSS EDMA statistics APIs
 */

#include "nss_edma_stats.h"
#include "nss_edma_strings.h"

/*
 * Declare atomic notifier data structure for statistics.
 */
ATOMIC_NOTIFIER_HEAD(nss_edma_stats_notifier);

struct nss_edma_stats edma_stats;

/*
 **********************************
 EDMA statistics APIs
 **********************************
 */

/*
 * nss_edma_port_stats_read()
 *	Read EDMA port statistics
 */
static ssize_t nss_edma_port_stats_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	int32_t i;

	/*
	 * Max output lines = #stats * NSS_MAX_CORES  +
	 * few blank lines for banner printing + Number of Extra outputlines for future reference to add new stats
	 */
	uint32_t max_output_lines = NSS_STATS_NODE_MAX + NSS_STATS_EXTRA_OUTPUT_LINES;
	size_t size_al = NSS_STATS_MAX_STR_LENGTH * max_output_lines;
	size_t size_wr = 0;
	ssize_t bytes_read = 0;
	uint64_t *stats_shadow;
	struct nss_stats_data *data = fp->private_data;

	char *lbuf = kzalloc(size_al, GFP_KERNEL);
	if (unlikely(lbuf == NULL)) {
		nss_warning("Could not allocate memory for local statistics buffer");
		return 0;
	}

	stats_shadow = kzalloc(NSS_STATS_NODE_MAX * sizeof(uint64_t), GFP_KERNEL);
	if (unlikely(stats_shadow == NULL)) {
		nss_warning("Could not allocate memory for local shadow buffer");
		kfree(lbuf);
		return 0;
	}

	size_wr += nss_stats_banner(lbuf, size_wr, size_al, "edma", NSS_STATS_SINGLE_CORE);

	/*
	 * Common node stats
	 */
	size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, "edma port %d stats:\n\n", data->edma_id);

	spin_lock_bh(&nss_top_main.stats_lock);
	for (i = 0; (i < NSS_STATS_NODE_MAX); i++) {
		stats_shadow[i] = edma_stats.port[data->edma_id].port_stats[i];
	}

	spin_unlock_bh(&nss_top_main.stats_lock);
	size_wr += nss_stats_print("edma_port", NULL, data->edma_id
					, nss_edma_strings_stats_node
					, stats_shadow
					, NSS_STATS_NODE_MAX
					, lbuf, size_wr, size_al);
	bytes_read = simple_read_from_buffer(ubuf, sz, ppos, lbuf, strlen(lbuf));
	kfree(lbuf);
	kfree(stats_shadow);

	return bytes_read;
}

/*
 * nss_edma_port_type_stats_read()
 *	Read EDMA port type
 */
static ssize_t nss_edma_port_type_stats_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	/*
	 * max output lines = #stats + start tag line + end tag line + three blank lines
	 */
	uint32_t max_output_lines = (1 + 2) + 3;
	size_t size_al = NSS_STATS_MAX_STR_LENGTH * max_output_lines;
	size_t size_wr = 0;
	ssize_t bytes_read = 0;
	uint64_t port_type;
	struct nss_stats_data *data = fp->private_data;

	char *lbuf = kzalloc(size_al, GFP_KERNEL);
	if (unlikely(lbuf == NULL)) {
		nss_warning("Could not allocate memory for local statistics buffer");
		return 0;
	}

	size_wr = scnprintf(lbuf, size_al, "edma port type start:\n\n");
	size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, "edma port %d type:\n\n", data->edma_id);

	/*
	 * Port type
	 */
	spin_lock_bh(&nss_top_main.stats_lock);
	port_type = edma_stats.port[data->edma_id].port_type;
	spin_unlock_bh(&nss_top_main.stats_lock);

	size_wr += scnprintf(lbuf + size_wr, size_al - size_wr,
					"port_type = %s\n", nss_edma_strings_stats_port_type[port_type].stats_name);

	size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, "\nedma stats end\n");
	bytes_read = simple_read_from_buffer(ubuf, sz, ppos, lbuf, strlen(lbuf));
	kfree(lbuf);

	return bytes_read;
}

/*
 * nss_edma_port_ring_map_stats_read()
 *	Read EDMA port ring map
 */
static ssize_t nss_edma_port_ring_map_stats_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	int32_t i;

	/*
	 * max output lines = #stats + start tag line + end tag line + three blank lines
	 */
	uint32_t max_output_lines = (4 + 2) + 3;
	size_t size_al = NSS_STATS_MAX_STR_LENGTH * max_output_lines;
	size_t size_wr = 0;
	ssize_t bytes_read = 0;
	uint64_t *stats_shadow;
	struct nss_stats_data *data = fp->private_data;

	char *lbuf = kzalloc(size_al, GFP_KERNEL);
	if (unlikely(lbuf == NULL)) {
		nss_warning("Could not allocate memory for local statistics buffer");
		return 0;
	}

	stats_shadow = kzalloc(NSS_EDMA_PORT_RING_MAP_MAX * sizeof(uint64_t), GFP_KERNEL);
	if (unlikely(stats_shadow == NULL)) {
		nss_warning("Could not allocate memory for local shadow buffer");
		kfree(lbuf);
		return 0;
	}

	size_wr = scnprintf(lbuf, size_al, "edma port ring map start:\n\n");

	/*
	 * Port ring map
	 */
	size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, "edma port %d ring map:\n\n", data->edma_id);
	spin_lock_bh(&nss_top_main.stats_lock);
	for (i = 0; i < NSS_EDMA_PORT_RING_MAP_MAX; i++) {
		stats_shadow[i] = edma_stats.port[data->edma_id].port_ring_map[i];
	}

	spin_unlock_bh(&nss_top_main.stats_lock);

	size_wr += nss_stats_print("edma_port_ring", NULL, data->edma_id
					, nss_edma_strings_stats_port_ring_map
					, stats_shadow
					, NSS_EDMA_PORT_RING_MAP_MAX
					, lbuf, size_wr, size_al);
	bytes_read = simple_read_from_buffer(ubuf, sz, ppos, lbuf, strlen(lbuf));
	kfree(lbuf);
	kfree(stats_shadow);

	return bytes_read;
}

/*
 * nss_edma_txring_stats_read()
 *	Read EDMA Tx ring stats
 */
static ssize_t nss_edma_txring_stats_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	int32_t i;

	/*
	 * max output lines = #stats + start tag line + end tag line + three blank lines
	 */
	uint32_t max_output_lines = (NSS_EDMA_STATS_TX_MAX + 2) + 3;
	size_t size_al = NSS_STATS_MAX_STR_LENGTH * max_output_lines;
	size_t size_wr = 0;
	ssize_t bytes_read = 0;
	uint64_t *stats_shadow;
	struct nss_stats_data *data = fp->private_data;

	char *lbuf = kzalloc(size_al, GFP_KERNEL);
	if (unlikely(lbuf == NULL)) {
		nss_warning("Could not allocate memory for local statistics buffer");
		return 0;
	}

	stats_shadow = kzalloc(NSS_EDMA_STATS_TX_MAX * sizeof(uint64_t), GFP_KERNEL);
	if (unlikely(stats_shadow == NULL)) {
		nss_warning("Could not allocate memory for local shadow buffer");
		kfree(lbuf);
		return 0;
	}

	size_wr = scnprintf(lbuf, size_al, "edma Tx ring stats start:\n\n");

	/*
	 * Tx ring stats
	 */
	size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, "Tx ring %d stats:\n\n", data->edma_id);
	spin_lock_bh(&nss_top_main.stats_lock);
	for (i = 0; i < NSS_EDMA_STATS_TX_MAX; i++) {
		stats_shadow[i] = edma_stats.tx_stats[data->edma_id][i];
	}

	spin_unlock_bh(&nss_top_main.stats_lock);

	size_wr += nss_stats_print("edma_tx_ring", NULL, data->edma_id
					, nss_edma_strings_stats_tx
					, stats_shadow
					, NSS_EDMA_STATS_TX_MAX
					, lbuf, size_wr, size_al);
	bytes_read = simple_read_from_buffer(ubuf, sz, ppos, lbuf, strlen(lbuf));
	kfree(lbuf);
	kfree(stats_shadow);

	return bytes_read;
}

/*
 * nss_edma_rxring_stats_read()
 *	Read EDMA rxring stats
 */
static ssize_t nss_edma_rxring_stats_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	int32_t i;

	/*
	 * max output lines = #stats + start tag line + end tag line + three blank lines
	 */
	uint32_t max_output_lines = (NSS_EDMA_STATS_RX_MAX + 2) + 3;
	size_t size_al = NSS_STATS_MAX_STR_LENGTH * max_output_lines;
	size_t size_wr = 0;
	ssize_t bytes_read = 0;
	uint64_t *stats_shadow;
	struct nss_stats_data *data = fp->private_data;

	char *lbuf = kzalloc(size_al, GFP_KERNEL);
	if (unlikely(lbuf == NULL)) {
		nss_warning("Could not allocate memory for local statistics buffer");
		return 0;
	}

	stats_shadow = kzalloc(NSS_EDMA_STATS_RX_MAX * sizeof(uint64_t), GFP_KERNEL);
	if (unlikely(stats_shadow == NULL)) {
		nss_warning("Could not allocate memory for local shadow buffer");
		kfree(lbuf);
		return 0;
	}

	/*
	 * RX ring stats
	 */
	spin_lock_bh(&nss_top_main.stats_lock);
	for (i = 0; i < NSS_EDMA_STATS_RX_MAX; i++) {
		stats_shadow[i] = edma_stats.rx_stats[data->edma_id][i];
	}

	spin_unlock_bh(&nss_top_main.stats_lock);
	size_wr += nss_stats_print("edma_rx_ring", NULL, data->edma_id
					, nss_edma_strings_stats_rx
					, stats_shadow
					, NSS_EDMA_STATS_RX_MAX
					, lbuf, size_wr, size_al);
	bytes_read = simple_read_from_buffer(ubuf, sz, ppos, lbuf, strlen(lbuf));
	kfree(lbuf);
	kfree(stats_shadow);

	return bytes_read;
}

/*
 * nss_edma_txcmplring_stats_read()
 *	Read EDMA txcmplring stats
 */
static ssize_t nss_edma_txcmplring_stats_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	int32_t i;

	/*
	 * max output lines = #stats + start tag line + end tag line + three blank lines
	 */
	uint32_t max_output_lines = (NSS_EDMA_STATS_TXCMPL_MAX + 2) + 3;
	size_t size_al = NSS_STATS_MAX_STR_LENGTH * max_output_lines;
	size_t size_wr = 0;
	ssize_t bytes_read = 0;
	uint64_t *stats_shadow;
	struct nss_stats_data *data = fp->private_data;

	char *lbuf = kzalloc(size_al, GFP_KERNEL);
	if (unlikely(lbuf == NULL)) {
		nss_warning("Could not allocate memory for local statistics buffer");
		return 0;
	}

	stats_shadow = kzalloc(NSS_EDMA_STATS_TXCMPL_MAX * sizeof(uint64_t), GFP_KERNEL);
	if (unlikely(stats_shadow == NULL)) {
		nss_warning("Could not allocate memory for local shadow buffer");
		kfree(lbuf);
		return 0;
	}

	size_wr = scnprintf(lbuf, size_al, "edma Tx cmpl ring stats start:\n\n");

	/*
	 * Tx cmpl ring stats
	 */
	size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, "Tx cmpl ring %d stats:\n\n", data->edma_id);
	spin_lock_bh(&nss_top_main.stats_lock);
	for (i = 0; i < NSS_EDMA_STATS_TXCMPL_MAX; i++) {
		stats_shadow[i] = edma_stats.txcmpl_stats[data->edma_id][i];
	}

	spin_unlock_bh(&nss_top_main.stats_lock);
	size_wr += nss_stats_print("edma_tx_cmpl_ring", NULL, data->edma_id
					, nss_edma_strings_stats_txcmpl
					, stats_shadow
					, NSS_EDMA_STATS_TXCMPL_MAX
					, lbuf, size_wr, size_al);
	size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, "\nedma Tx cmpl ring stats end\n\n");
	bytes_read = simple_read_from_buffer(ubuf, sz, ppos, lbuf, strlen(lbuf));
	kfree(lbuf);
	kfree(stats_shadow);

	return bytes_read;
}

/*
 * nss_edma_rxfillring_stats_read()
 *	Read EDMA rxfillring stats
 */
static ssize_t nss_edma_rxfillring_stats_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	int32_t i;

	/*
	 * max output lines = #stats + start tag line + end tag line + three blank lines
	 */
	uint32_t max_output_lines = (NSS_EDMA_STATS_RXFILL_MAX + 2) + 3;
	size_t size_al = NSS_STATS_MAX_STR_LENGTH * max_output_lines;
	size_t size_wr = 0;
	ssize_t bytes_read = 0;
	uint64_t *stats_shadow;
	struct nss_stats_data *data = fp->private_data;

	char *lbuf = kzalloc(size_al, GFP_KERNEL);
	if (unlikely(lbuf == NULL)) {
		nss_warning("Could not allocate memory for local statistics buffer");
		return 0;
	}

	stats_shadow = kzalloc(NSS_EDMA_STATS_RXFILL_MAX * sizeof(uint64_t), GFP_KERNEL);
	if (unlikely(stats_shadow == NULL)) {
		nss_warning("Could not allocate memory for local shadow buffer");
		kfree(lbuf);
		return 0;
	}

	size_wr = scnprintf(lbuf, size_al, "edma Rx fill ring stats start:\n\n");

	/*
	 * Rx fill ring stats
	 */
	size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, "Rx fill ring %d stats:\n\n", data->edma_id);
	spin_lock_bh(&nss_top_main.stats_lock);
	for (i = 0; i < NSS_EDMA_STATS_RXFILL_MAX; i++) {
		stats_shadow[i] = edma_stats.rxfill_stats[data->edma_id][i];
	}

	spin_unlock_bh(&nss_top_main.stats_lock);
	size_wr += nss_stats_print("edma_rx_fill_ring", NULL
					, NSS_STATS_SINGLE_INSTANCE
					, nss_edma_strings_stats_rxfill
					, stats_shadow
					, NSS_EDMA_STATS_RXFILL_MAX
					, lbuf, size_wr, size_al);
	bytes_read = simple_read_from_buffer(ubuf, sz, ppos, lbuf, strlen(lbuf));
	kfree(lbuf);
	kfree(stats_shadow);

	return bytes_read;
}

/*
 * nss_edma_err_stats_read()
 *      Read EDMA err stats
 */
static ssize_t nss_edma_err_stats_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	int32_t i;

	/*
	 * max output lines = #stats + start tag line + end tag line + three blank lines
	 */
	uint32_t max_output_lines = (NSS_EDMA_ERR_STATS_MAX + 2) + 3;
	size_t size_al = NSS_STATS_MAX_STR_LENGTH * max_output_lines;
	size_t size_wr = 0;
	ssize_t bytes_read = 0;
	uint64_t *stats_shadow;

	char *lbuf = kzalloc(size_al, GFP_KERNEL);
	if (unlikely(lbuf == NULL)) {
		nss_warning("Could not allocate memory for local statistics buffer");
		return 0;
	}

	stats_shadow = kzalloc(NSS_EDMA_ERR_STATS_MAX * sizeof(uint64_t), GFP_KERNEL);
	if (unlikely(stats_shadow == NULL)) {
		nss_warning("Could not allocate memory for local shadow buffer");
		kfree(lbuf);
		return 0;
	}

	size_wr = scnprintf(lbuf, size_al, "edma error stats start:\n\n");

	/*
	 * Common node stats
	 */
	spin_lock_bh(&nss_top_main.stats_lock);

	for (i = 0; (i < NSS_EDMA_ERR_STATS_MAX); i++)
		stats_shadow[i] = edma_stats.misc_err[i];

	spin_unlock_bh(&nss_top_main.stats_lock);
	size_wr += nss_stats_print("edma_err", NULL, NSS_STATS_SINGLE_INSTANCE
					, nss_edma_strings_stats_err_map
					, stats_shadow
					, NSS_EDMA_ERR_STATS_MAX
					, lbuf, size_wr, size_al);
	bytes_read = simple_read_from_buffer(ubuf, sz, ppos, lbuf, strlen(lbuf));
	kfree(lbuf);
	kfree(stats_shadow);

	return bytes_read;
}

/*
 * edma_port_stats_ops
 */
NSS_STATS_DECLARE_FILE_OPERATIONS(edma_port);

/*
 * edma_port_type_stats_ops
 */
NSS_STATS_DECLARE_FILE_OPERATIONS(edma_port_type);

/*
 * edma_port_ring_map_stats_ops
 */
NSS_STATS_DECLARE_FILE_OPERATIONS(edma_port_ring_map);

/*
 * edma_txring_stats_ops
 */
NSS_STATS_DECLARE_FILE_OPERATIONS(edma_txring);

/*
 * edma_rxring_stats_ops
 */
NSS_STATS_DECLARE_FILE_OPERATIONS(edma_rxring);

/*
 * edma_txcmplring_stats_ops
 */
NSS_STATS_DECLARE_FILE_OPERATIONS(edma_txcmplring);

/*
 * edma_rxfillring_stats_ops
 */
NSS_STATS_DECLARE_FILE_OPERATIONS(edma_rxfillring);

/*
 * edma_err_stats_ops
 */
NSS_STATS_DECLARE_FILE_OPERATIONS(edma_err);

/*
 * nss_edma_stats_dentry_create()
 *	Create edma statistics debug entry.
 */
void nss_edma_stats_dentry_create(void)
{
	int i;
	struct dentry *edma_d = NULL;
	struct dentry *edma_port_dir_d = NULL;
	struct dentry *edma_port_d = NULL;
	struct dentry *edma_port_type_d = NULL;
	struct dentry *edma_port_stats_d = NULL;
	struct dentry *edma_port_ring_map_d = NULL;
	struct dentry *edma_rings_dir_d = NULL;
	struct dentry *edma_tx_dir_d = NULL;
	struct dentry *edma_tx_d = NULL;
	struct dentry *edma_rx_dir_d = NULL;
	struct dentry *edma_rx_d = NULL;
	struct dentry *edma_txcmpl_dir_d = NULL;
	struct dentry *edma_txcmpl_d = NULL;
	struct dentry *edma_rxfill_dir_d = NULL;
	struct dentry *edma_rxfill_d = NULL;
	struct dentry *edma_err_stats_d = NULL;
	char file_name[10];

	edma_d = debugfs_create_dir("edma", nss_top_main.stats_dentry);
	if (unlikely(edma_d == NULL)) {
		nss_warning("Failed to create qca-nss-drv/stats/edma directory");
		return;
	}

	/*
	 * edma port stats
	 */
	edma_port_dir_d = debugfs_create_dir("ports", edma_d);
	if (unlikely(edma_port_dir_d == NULL)) {
		nss_warning("Failed to create qca-nss-drv/stats/edma/ports directory");
		return;
	}

	for (i = 0; i < NSS_EDMA_NUM_PORTS_MAX; i++) {
		memset(file_name, 0, sizeof(file_name));
		snprintf(file_name, sizeof(file_name), "%d", i);

		edma_port_d = debugfs_create_dir(file_name, edma_port_dir_d);
		if (unlikely(edma_port_d == NULL)) {
			nss_warning("Failed to create qca-nss-drv/stats/edma/ports/%d directory", i);
			return;
		}

		edma_port_stats_d = debugfs_create_file("stats", 0400, edma_port_d, (void *)(nss_ptr_t)i, &nss_edma_port_stats_ops);
		if (unlikely(edma_port_stats_d == NULL)) {
			nss_warning("Failed to create qca-nss-drv/stats/edma/ports/%d/stats file", i);
			return;
		}

		edma_port_type_d = debugfs_create_file("type", 0400, edma_port_d, (void *)(nss_ptr_t)i, &nss_edma_port_type_stats_ops);
		if (unlikely(edma_port_type_d == NULL)) {
			nss_warning("Failed to create qca-nss-drv/stats/edma/ports/%d/type file", i);
			return;
		}

		edma_port_ring_map_d = debugfs_create_file("ring_map", 0400, edma_port_d, (void *)(nss_ptr_t)i, &nss_edma_port_ring_map_stats_ops);
		if (unlikely(edma_port_ring_map_d == NULL)) {
			nss_warning("Failed to create qca-nss-drv/stats/edma/ports/%d/ring_map file", i);
			return;
		}
	}

	/*
	 *  edma error stats
	 */
	edma_err_stats_d = NULL;
	edma_err_stats_d = debugfs_create_file("err_stats", 0400, edma_d, &nss_top_main, &nss_edma_err_stats_ops);
	if (unlikely(edma_port_stats_d == NULL)) {
		nss_warning("Failed to create qca-nss-drv/stats/edma/%d/err_stats file", 0);
		return;
	}

	/*
	 * edma ring stats
	 */
	edma_rings_dir_d = debugfs_create_dir("rings", edma_d);
	if (unlikely(edma_rings_dir_d == NULL)) {
		nss_warning("Failed to create qca-nss-drv/stats/edma/rings directory");
		return;
	}

	/*
	 * edma tx ring stats
	 */
	edma_tx_dir_d = debugfs_create_dir("tx", edma_rings_dir_d);
	if (unlikely(edma_tx_dir_d == NULL)) {
		nss_warning("Failed to create qca-nss-drv/stats/edma/rings/tx directory");
		return;
	}

	for (i = 0; i < NSS_EDMA_NUM_TX_RING_MAX; i++) {
		memset(file_name, 0, sizeof(file_name));
		scnprintf(file_name, sizeof(file_name), "%d", i);
		edma_tx_d = debugfs_create_file(file_name, 0400, edma_tx_dir_d, (void *)(nss_ptr_t)i, &nss_edma_txring_stats_ops);
		if (unlikely(edma_tx_d == NULL)) {
			nss_warning("Failed to create qca-nss-drv/stats/edma/rings/tx/%d file", i);
			return;
		}
	}

	/*
	 * edma rx ring stats
	 */
	edma_rx_dir_d = debugfs_create_dir("rx", edma_rings_dir_d);
	if (unlikely(edma_rx_dir_d == NULL)) {
		nss_warning("Failed to create qca-nss-drv/stats/edma/rings/rx directory");
		return;
	}

	for (i = 0; i < NSS_EDMA_NUM_RX_RING_MAX; i++) {
		memset(file_name, 0, sizeof(file_name));
		scnprintf(file_name, sizeof(file_name), "%d", i);
		edma_rx_d = debugfs_create_file(file_name, 0400, edma_rx_dir_d, (void *)(nss_ptr_t)i, &nss_edma_rxring_stats_ops);
		if (unlikely(edma_rx_d == NULL)) {
			nss_warning("Failed to create qca-nss-drv/stats/edma/rings/rx/%d file", i);
			return;
		}
	}

	/*
	 * edma tx cmpl ring stats
	 */
	edma_txcmpl_dir_d = debugfs_create_dir("txcmpl", edma_rings_dir_d);
	if (unlikely(edma_txcmpl_dir_d == NULL)) {
		nss_warning("Failed to create qca-nss-drv/stats/edma/rings/txcmpl directory");
		return;
	}

	for (i = 0; i < NSS_EDMA_NUM_TXCMPL_RING_MAX; i++) {
		memset(file_name, 0, sizeof(file_name));
		scnprintf(file_name, sizeof(file_name), "%d", i);
		edma_txcmpl_d = debugfs_create_file(file_name, 0400, edma_txcmpl_dir_d, (void *)(nss_ptr_t)i, &nss_edma_txcmplring_stats_ops);
		if (unlikely(edma_txcmpl_d == NULL)) {
			nss_warning("Failed to create qca-nss-drv/stats/edma/rings/txcmpl/%d file", i);
			return;
		}
	}

	/*
	 * edma rx fill ring stats
	 */
	edma_rxfill_dir_d = debugfs_create_dir("rxfill", edma_rings_dir_d);
	if (unlikely(edma_rxfill_dir_d == NULL)) {
		nss_warning("Failed to create qca-nss-drv/stats/edma/rings/rxfill directory");
		return;
	}

	for (i = 0; i < NSS_EDMA_NUM_RXFILL_RING_MAX; i++) {
		memset(file_name, 0, sizeof(file_name));
		scnprintf(file_name, sizeof(file_name), "%d", i);
		edma_rxfill_d = debugfs_create_file(file_name, 0400, edma_rxfill_dir_d, (void *)(nss_ptr_t)i, &nss_edma_rxfillring_stats_ops);
		if (unlikely(edma_rxfill_d == NULL)) {
			nss_warning("Failed to create qca-nss-drv/stats/edma/rings/rxfill/%d file", i);
			return;
		}
	}
}

/*
 * nss_edma_metadata_port_stats_sync()
 *	Handle the syncing of EDMA port statistics.
 */
void nss_edma_metadata_port_stats_sync(struct nss_ctx_instance *nss_ctx, struct nss_edma_port_stats_sync *nepss)
{
	uint16_t i, j = 0;
	struct nss_top_instance *nss_top = nss_ctx->nss_top;

	spin_lock_bh(&nss_top->stats_lock);

	/*
	 * edma port stats
	 * We process a subset of port stats since msg payload is not enough to hold all ports at once.
	 */
	for (i = nepss->start_port; i < nepss->end_port; i++) {
		int k;

		edma_stats.port[i].port_stats[NSS_STATS_NODE_RX_PKTS] += nepss->port_stats[j].node_stats.rx_packets;
		edma_stats.port[i].port_stats[NSS_STATS_NODE_RX_BYTES] += nepss->port_stats[j].node_stats.rx_bytes;
		edma_stats.port[i].port_stats[NSS_STATS_NODE_TX_PKTS] += nepss->port_stats[j].node_stats.tx_packets;
		edma_stats.port[i].port_stats[NSS_STATS_NODE_TX_BYTES] += nepss->port_stats[j].node_stats.tx_bytes;

		for (k = 0; k < NSS_MAX_NUM_PRI; k++) {
			edma_stats.port[i].port_stats[NSS_STATS_NODE_RX_QUEUE_0_DROPPED + k] += nepss->port_stats[j].node_stats.rx_dropped[k];
		}

		edma_stats.port[i].port_type = nepss->port_stats[j].port_type;
		edma_stats.port[i].port_ring_map[NSS_EDMA_PORT_RX_RING] = nepss->port_stats[j].edma_rx_ring;
		edma_stats.port[i].port_ring_map[NSS_EDMA_PORT_TX_RING] = nepss->port_stats[j].edma_tx_ring;
		j++;
	}

	spin_unlock_bh(&nss_top->stats_lock);
}

/*
 * nss_edma_metadata_ring_stats_sync()
 *	Handle the syncing of EDMA ring statistics.
 */
void nss_edma_metadata_ring_stats_sync(struct nss_ctx_instance *nss_ctx, struct nss_edma_ring_stats_sync *nerss)
{
	int32_t i;
	struct nss_top_instance *nss_top = nss_ctx->nss_top;

	spin_lock_bh(&nss_top->stats_lock);

	/*
	 * edma tx ring stats
	 */
	for (i = 0; i < NSS_EDMA_NUM_TX_RING_MAX; i++) {
		edma_stats.tx_stats[i][NSS_EDMA_STATS_TX_ERR] += nerss->tx_ring[i].tx_err;
		edma_stats.tx_stats[i][NSS_EDMA_STATS_TX_DROPPED] += nerss->tx_ring[i].tx_dropped;
		edma_stats.tx_stats[i][NSS_EDMA_STATS_TX_DESC] += nerss->tx_ring[i].desc_cnt;
	}

	/*
	 * edma rx ring stats
	 */
	for (i = 0; i < NSS_EDMA_NUM_RX_RING_MAX; i++) {
		edma_stats.rx_stats[i][NSS_EDMA_STATS_RX_CSUM_ERR] += nerss->rx_ring[i].rx_csum_err;
		edma_stats.rx_stats[i][NSS_EDMA_STATS_RX_DESC] += nerss->rx_ring[i].desc_cnt;
		edma_stats.rx_stats[i][NSS_EDMA_STATS_RX_QOS_ERR] += nerss->rx_ring[i].qos_err;
		edma_stats.rx_stats[i][NSS_EDMA_STATS_RX_SRC_PORT_INVALID] += nerss->rx_ring[i].rx_src_port_invalid;
		edma_stats.rx_stats[i][NSS_EDMA_STATS_RX_SRC_IF_INVALID] += nerss->rx_ring[i].rx_src_if_invalid;
	}

	/*
	 * edma tx cmpl ring stats
	 */
	for (i = 0; i < NSS_EDMA_NUM_TXCMPL_RING_MAX; i++) {
		edma_stats.txcmpl_stats[i][NSS_EDMA_STATS_TXCMPL_DESC] += nerss->txcmpl_ring[i].desc_cnt;
	}

	/*
	 * edma rx fill ring stats
	 */
	for (i = 0; i < NSS_EDMA_NUM_RXFILL_RING_MAX; i++) {
		edma_stats.rxfill_stats[i][NSS_EDMA_STATS_RXFILL_DESC] += nerss->rxfill_ring[i].desc_cnt;
	}

	spin_unlock_bh(&nss_top->stats_lock);
}

/*
 * nss_edma_metadata_err_stats_sync()
 *	Handle the syncing of EDMA error statistics.
 */
void nss_edma_metadata_err_stats_sync(struct nss_ctx_instance *nss_ctx, struct nss_edma_err_stats_sync *nerss)
{

	struct nss_top_instance *nss_top = nss_ctx->nss_top;

	spin_lock_bh(&nss_top->stats_lock);

	edma_stats.misc_err[NSS_EDMA_AXI_RD_ERR] += nerss->msg_err_stats.axi_rd_err;
	edma_stats.misc_err[NSS_EDMA_AXI_WR_ERR] += nerss->msg_err_stats.axi_wr_err;
	edma_stats.misc_err[NSS_EDMA_RX_DESC_FIFO_FULL_ERR] += nerss->msg_err_stats.rx_desc_fifo_full_err;
	edma_stats.misc_err[NSS_EDMA_RX_BUF_SIZE_ERR] += nerss->msg_err_stats.rx_buf_size_err;
	edma_stats.misc_err[NSS_EDMA_TX_SRAM_FULL_ERR] += nerss->msg_err_stats.tx_sram_full_err;
	edma_stats.misc_err[NSS_EDMA_TX_CMPL_BUF_FULL_ERR] += nerss->msg_err_stats.tx_cmpl_buf_full_err;
	edma_stats.misc_err[NSS_EDMA_PKT_LEN_LA64K_ERR] += nerss->msg_err_stats.pkt_len_la64k_err;
	edma_stats.misc_err[NSS_EDMA_PKT_LEN_LE33_ERR] += nerss->msg_err_stats.pkt_len_le33_err;
	edma_stats.misc_err[NSS_EDMA_DATA_LEN_ERR] += nerss->msg_err_stats.data_len_err;
	edma_stats.misc_err[NSS_EDMA_ALLOC_FAIL_CNT] += nerss->msg_err_stats.alloc_fail_cnt;
	edma_stats.misc_err[NSS_EDMA_QOS_INVAL_DST_DROPS] += nerss->msg_err_stats.qos_inval_dst_drops;

	spin_unlock_bh(&nss_top->stats_lock);
}

/*
 * nss_edma_stats_notify()
 *	Calls statistics notifier.
 *
 * Leverage NSS-FW statistics timing to update Netlink.
 */
void nss_edma_stats_notify(struct nss_ctx_instance *nss_ctx)
{
	uint32_t core_id = nss_ctx->id;

	atomic_notifier_call_chain(&nss_edma_stats_notifier, NSS_STATS_EVENT_NOTIFY, (void *)&core_id);
}

/*
 * nss_edma_stats_register_notifier()
 *	Registers statistics notifier.
 */
int nss_edma_stats_register_notifier(struct notifier_block *nb)
{
	return atomic_notifier_chain_register(&nss_edma_stats_notifier, nb);
}
EXPORT_SYMBOL(nss_edma_stats_register_notifier);

/*
 * nss_edma_stats_unregister_notifier()
 *	Deregisters stats notifier.
 */
int nss_edma_stats_unregister_notifier(struct notifier_block *nb)
{
	return atomic_notifier_chain_unregister(&nss_edma_stats_notifier, nb);
}
EXPORT_SYMBOL(nss_edma_stats_unregister_notifier);

/*
 * nss_edma_get_stats
 *	Sends EDMA statistics to NSS clients.
 */
void nss_edma_get_stats(uint64_t  *stats, int port_id)
{
	memcpy(stats, edma_stats.port[port_id].port_stats, sizeof(uint64_t) * NSS_STATS_NODE_MAX);
}
EXPORT_SYMBOL(nss_edma_get_stats);

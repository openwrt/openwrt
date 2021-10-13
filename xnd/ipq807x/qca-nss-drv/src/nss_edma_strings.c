/*
 **************************************************************************
 * Copyright (c) 2020-2021, The Linux Foundation. All rights reserved.
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
#include <nss_edma.h>
#include "nss_strings.h"

/*
 * nss_edma_strings_stats_node
 *	EDMA statistics strings.
 */
struct nss_stats_info nss_edma_strings_stats_node[NSS_STATS_NODE_MAX] = {
	{"rx_pkts"		, NSS_STATS_TYPE_COMMON},
	{"rx_byts"		, NSS_STATS_TYPE_COMMON},
	{"tx_pkts"		, NSS_STATS_TYPE_COMMON},
	{"tx_byts"		, NSS_STATS_TYPE_COMMON},
	{"rx_queue[0]_drops"	, NSS_STATS_TYPE_DROP},
	{"rx_queue[1]_drops"	, NSS_STATS_TYPE_DROP},
	{"rx_queue[2]_drops"	, NSS_STATS_TYPE_DROP},
	{"rx_queue[3]_drops"	, NSS_STATS_TYPE_DROP}
};

/*
 * nss_edma_common_stats_strings_read()
 *	Read EDMA common node statistics names.
 */
static ssize_t nss_edma_common_stats_strings_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	return nss_strings_print(ubuf, sz, ppos, nss_edma_strings_stats_node, NSS_STATS_NODE_MAX);
}

/*
 * nss_edma_common_stats_strings_ops
 */
NSS_STRINGS_DECLARE_FILE_OPERATIONS(edma_common_stats);

/*
 * nss_edma_strings_stats_tx
 */
struct nss_stats_info nss_edma_strings_stats_tx[NSS_EDMA_STATS_TX_MAX] = {
	{"tx_err"	, NSS_STATS_TYPE_ERROR},
	{"tx_drops"	, NSS_STATS_TYPE_DROP},
	{"desc_cnt"	, NSS_STATS_TYPE_SPECIAL}
};

/*
 * nss_edma_txring_strings_read()
 *      Read EDMA txring names.
 */
static ssize_t nss_edma_txring_strings_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	return nss_strings_print(ubuf, sz, ppos, nss_edma_strings_stats_tx, NSS_EDMA_STATS_TX_MAX);
}

/*
 * edma_txring_strings_ops
 */
NSS_STRINGS_DECLARE_FILE_OPERATIONS(edma_txring);

/*
 * nss_edma_strings_stats_rx
 */
struct nss_stats_info nss_edma_strings_stats_rx[NSS_EDMA_STATS_RX_MAX] = {
	{"rx_csum_err"		, NSS_STATS_TYPE_ERROR},
	{"desc_cnt"		, NSS_STATS_TYPE_SPECIAL},
	{"qos_err"		, NSS_STATS_TYPE_DROP},
	{"rx_src_port_invalid"	, NSS_STATS_TYPE_DROP},
	{"rx_src_interface_invalid"	, NSS_STATS_TYPE_DROP}
};

/*
 * nss_edma_rxring_strings_read()
 *      Read EDMA rxring names.
 */
static ssize_t nss_edma_rxring_strings_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	return nss_strings_print(ubuf, sz, ppos, nss_edma_strings_stats_rx, NSS_EDMA_STATS_RX_MAX);
}

/*
 * edma_rxring_strings_ops
 */
NSS_STRINGS_DECLARE_FILE_OPERATIONS(edma_rxring);

/*
 * nss_edma_strings_stats_txcmpl
 */
struct nss_stats_info nss_edma_strings_stats_txcmpl[NSS_EDMA_STATS_TXCMPL_MAX] = {
	{"desc_cnt"	, NSS_STATS_TYPE_SPECIAL}
};

/*
 * nss_edma_txcmplring_strings_read()
 *      Read EDMA txcmplring names.
 */
static ssize_t nss_edma_txcmplring_strings_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	return nss_strings_print(ubuf, sz, ppos, nss_edma_strings_stats_txcmpl, NSS_EDMA_STATS_TXCMPL_MAX);
}

/*
 * edma_txcmplring_strings_ops
 */
NSS_STRINGS_DECLARE_FILE_OPERATIONS(edma_txcmplring);

/*
 * nss_edma_strings_stats_rxfill
 */
struct nss_stats_info nss_edma_strings_stats_rxfill[NSS_EDMA_STATS_RXFILL_MAX] = {
	{"desc_cnt"	, NSS_STATS_TYPE_SPECIAL}
};

/*
 * nss_edma_rxfillring_strings_read()
 *      Read EDMA rxfillring names.
 */
static ssize_t nss_edma_rxfillring_strings_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	return nss_strings_print(ubuf, sz, ppos, nss_edma_strings_stats_rxfill, NSS_EDMA_STATS_RXFILL_MAX);
}

/*
 * edma_rxfillring_strings_ops
 */
NSS_STRINGS_DECLARE_FILE_OPERATIONS(edma_rxfillring);

/*
 * nss_edma_strings_stats_port_type
 */
struct nss_stats_info nss_edma_strings_stats_port_type[NSS_EDMA_PORT_TYPE_MAX] = {
	{"physical_port", NSS_STATS_TYPE_SPECIAL},
	{"virtual_port" , NSS_STATS_TYPE_SPECIAL}
};

/*
 * nss_edma_port_type_strings_read()
 *      Read EDMA port type names.
 */
static ssize_t nss_edma_port_type_strings_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	return nss_strings_print(ubuf, sz, ppos, nss_edma_strings_stats_port_type, NSS_EDMA_PORT_TYPE_MAX);
}

/*
 * edma_port_type_strings_ops
 */
NSS_STRINGS_DECLARE_FILE_OPERATIONS(edma_port_type);

/*
 * nss_edma_strings_stats_port_ring_map
 */
struct nss_stats_info nss_edma_strings_stats_port_ring_map[NSS_EDMA_PORT_RING_MAP_MAX] = {
	{"rx_ring"	, NSS_STATS_TYPE_SPECIAL},
	{"tx_ring"	, NSS_STATS_TYPE_SPECIAL}
};

/*
 * nss_edma_port_ring_map_strings_read()
 *	Read EDMA port ring map names.
 */
static ssize_t nss_edma_port_ring_map_strings_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	return nss_strings_print(ubuf, sz, ppos, nss_edma_strings_stats_port_ring_map, NSS_EDMA_PORT_RING_MAP_MAX);
}

/*
 * edma_port_ring_map_strings_ops
 */
NSS_STRINGS_DECLARE_FILE_OPERATIONS(edma_port_ring_map);

/*
 * nss_edma_strings_stats_err_map
 */
struct nss_stats_info nss_edma_strings_stats_err_map[NSS_EDMA_ERR_STATS_MAX] = {
	{"axi_rd_err"		, NSS_STATS_TYPE_ERROR},
	{"axi_wr_err"		, NSS_STATS_TYPE_ERROR},
	{"rx_desc_fifo_full_err", NSS_STATS_TYPE_ERROR},
	{"rx_buf_size_err"	, NSS_STATS_TYPE_ERROR},
	{"tx_sram_full_err"	, NSS_STATS_TYPE_ERROR},
	{"tx_cmpl_buf_full_err"	, NSS_STATS_TYPE_ERROR},
	{"pkt_len_la64k_err"	, NSS_STATS_TYPE_ERROR},
	{"pkt_len_le33_err"	, NSS_STATS_TYPE_ERROR},
	{"data_len_err"		, NSS_STATS_TYPE_ERROR},
	{"alloc_fail_cnt"	, NSS_STATS_TYPE_ERROR},
	{"qos_inval_dst_drops"	, NSS_STATS_TYPE_DROP}
};

/*
 * nss_edma_err_strings_read()
 *      Read EDMA error names.
 */
static ssize_t nss_edma_err_strings_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	return nss_strings_print(ubuf, sz, ppos, nss_edma_strings_stats_err_map, NSS_EDMA_ERR_STATS_MAX);
}

/*
 * edma_err_strings_ops
 */
NSS_STRINGS_DECLARE_FILE_OPERATIONS(edma_err);

/*
 * nss_edma_strings_dentry_create()
 *      Create EDMA statistics strings debug entry.
 */
void nss_edma_strings_dentry_create(void)
{
	struct dentry *edma_d;
	struct dentry *edma_port_dir_d;
	struct dentry *edma_rings_dir_d;
	struct dentry *edma_rx_dir_d;
	struct dentry *edma_tx_dir_d;
	struct dentry *edma_rxfill_dir_d;
	struct dentry *edma_txcmpl_dir_d;
	struct dentry *file_d;

	if (!nss_top_main.strings_dentry) {
		nss_warning("qca-nss-drv/strings is not present");
		return;
	}

	edma_d = debugfs_create_dir("edma", nss_top_main.strings_dentry);
	if (!edma_d) {
		nss_warning("Failed to create qca-nss-drv/strings/edma directory");
		return;
	}

	/*
	 * EDMA port stats.
	 */
	edma_port_dir_d = debugfs_create_dir("ports", edma_d);
	if (!edma_port_dir_d) {
		nss_warning("Failed to create qca-nss-drv/strings/edma/ports directory");
		goto fail;
	}

	file_d = debugfs_create_file("common_stats_str", 0400, edma_port_dir_d, &nss_top_main, &nss_edma_common_stats_strings_ops);
	if (!file_d) {
		nss_warning("Failed to create qca-nss-drv/strings/edma/ports/common_stats_str file");
		goto fail;
	}

	file_d = debugfs_create_file("type", 0400, edma_port_dir_d, &nss_top_main, &nss_edma_port_type_strings_ops);
	if (!file_d) {
		nss_warning("Failed to create qca-nss-drv/strings/edma/ports/type file");
		goto fail;
	}

	file_d = debugfs_create_file("ring_map", 0400, edma_port_dir_d, &nss_top_main, &nss_edma_port_ring_map_strings_ops);
	if (!file_d) {
		nss_warning("Failed to create qca-nss-drv/strings/edma/ports/ring_map file");
		goto fail;
	}

	/*
	 *  edma error stats
	 */
	file_d = debugfs_create_file("err_stats", 0400, edma_d, &nss_top_main, &nss_edma_err_strings_ops);
	if (!file_d) {
		nss_warning("Failed to create qca-nss-drv/strings/edma/err_stats file");
		goto fail;
	}

	/*
	 * edma ring stats
	 */
	edma_rings_dir_d = debugfs_create_dir("rings", edma_d);
	if (!edma_rings_dir_d) {
		nss_warning("Failed to create qca-nss-drv/strings/edma/rings directory");
		goto fail;
	}

	/*
	 * edma tx ring stats
	 */
	edma_tx_dir_d = debugfs_create_dir("tx", edma_rings_dir_d);
	if (!edma_tx_dir_d) {
		nss_warning("Failed to create qca-nss-drv/strings/edma/rings/tx directory");
		goto fail;
	}

	file_d = debugfs_create_file("tx_str", 0400, edma_tx_dir_d, &nss_top_main, &nss_edma_txring_strings_ops);
	if (!file_d) {
		nss_warning("Failed to create qca-nss-drv/strings/edma/rings/tx file");
		goto fail;
	}

	/*
	 * edma rx ring stats
	 */
	edma_rx_dir_d = debugfs_create_dir("rx", edma_rings_dir_d);
	if (!edma_rx_dir_d) {
		nss_warning("Failed to create qca-nss-drv/strings/edma/rings/rx directory");
		goto fail;
	}

	file_d = debugfs_create_file("rx_str", 0400, edma_rx_dir_d, &nss_top_main, &nss_edma_rxring_strings_ops);
	if (!file_d) {
		nss_warning("Failed to create qca-nss-drv/strings/edma/rings/rx file");
		goto fail;
	}

	/*
	 * edma tx cmpl ring stats
	 */
	edma_txcmpl_dir_d = debugfs_create_dir("txcmpl", edma_rings_dir_d);
	if (!edma_txcmpl_dir_d) {
		nss_warning("Failed to create qca-nss-drv/strings/edma/rings/txcmpl directory");
		goto fail;
	}

	file_d = debugfs_create_file("txcmpl_str", 0400, edma_txcmpl_dir_d, &nss_top_main, &nss_edma_txcmplring_strings_ops);
	if (!file_d) {
		nss_warning("Failed to create qca-nss-drv/strings/edma/rings/txcmpl file");
		goto fail;
	}

	/*
	 * edma rx fill ring stats
	 */
	edma_rxfill_dir_d = debugfs_create_dir("rxfill", edma_rings_dir_d);
	if (!edma_rxfill_dir_d) {
		nss_warning("Failed to create qca-nss-drv/strings/edma/rings/rxfill directory");
		goto fail;
	}

	file_d = debugfs_create_file("rxfill_str", 0400, edma_rxfill_dir_d, &nss_top_main, &nss_edma_rxfillring_strings_ops);
	if (!file_d) {
		nss_warning("Failed to create qca-nss-drv/strings/edma/rings/rxfill file");
		goto fail;
	}

	return;
fail:
	debugfs_remove_recursive(edma_d);
}

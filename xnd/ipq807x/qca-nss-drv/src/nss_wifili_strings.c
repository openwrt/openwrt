/*
 **************************************************************************
 * Copyright (c) 2019-2020, The Linux Foundation. All rights reserved.
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

#include "nss_stats.h"
#include "nss_core.h"
#include <nss_wifili_if.h>
#include "nss_strings.h"
#include "nss_wifili_strings.h"

/*
 * nss_wifili_strings_stats_txrx
 *	wifili txrx statistics
 */
struct nss_stats_info nss_wifili_strings_stats_txrx[NSS_WIFILI_STATS_TXRX_MAX] = {
	{"rx_msdu_error"			, NSS_STATS_TYPE_ERROR},
	{"rx_inv_peer_rcv"			, NSS_STATS_TYPE_SPECIAL},
	{"rx_wds_srcport_exception"		, NSS_STATS_TYPE_EXCEPTION},
	{"rx_wds_srcport_exception_fail"	, NSS_STATS_TYPE_DROP},
	{"rx_deliverd"				, NSS_STATS_TYPE_SPECIAL},
	{"rx_deliver_drops"			, NSS_STATS_TYPE_DROP},
	{"rx_intra_bss_ucast"			, NSS_STATS_TYPE_SPECIAL},
	{"rx_intra_bss_ucast_fail"		, NSS_STATS_TYPE_DROP},
	{"rx_intra_bss_mcast"			, NSS_STATS_TYPE_SPECIAL},
	{"rx_intra_bss_mcast_fail"		, NSS_STATS_TYPE_DROP},
	{"rx_sg_rcv_send"			, NSS_STATS_TYPE_SPECIAL},
	{"rx_sg_rcv_fail"			, NSS_STATS_TYPE_DROP},
	{"rx_mcast_echo"			, NSS_STATS_TYPE_SPECIAL},
	{"rx_inv_tid"				, NSS_STATS_TYPE_SPECIAL},
	{"stats_rx_frag_inv_sc"			, NSS_STATS_TYPE_SPECIAL},
	{"stats_rx_frag_inv_fc"			, NSS_STATS_TYPE_SPECIAL},
	{"stats_rx_frag_non_frag"		, NSS_STATS_TYPE_SPECIAL},
	{"stats_rx_frag_retry"			, NSS_STATS_TYPE_SPECIAL},
	{"stats_rx_frag_ooo"			, NSS_STATS_TYPE_SPECIAL},
	{"stats_rx_frag_ooo_seq"		, NSS_STATS_TYPE_SPECIAL},
	{"stats_rx_frag_all_frag_rcv"		, NSS_STATS_TYPE_SPECIAL},
	{"stats_rx_frag_deliver"		, NSS_STATS_TYPE_SPECIAL},
	{"tx_enqueue"				, NSS_STATS_TYPE_SPECIAL},
	{"tx_enqueue_drop"			, NSS_STATS_TYPE_DROP},
	{"tx_dequeue"				, NSS_STATS_TYPE_SPECIAL},
	{"tx_hw_enqueue_fail"			, NSS_STATS_TYPE_DROP},
	{"tx_sent_count"			, NSS_STATS_TYPE_SPECIAL}
};

/*
 * nss_wifili_strings_stats_tcl
 *	wifili tcl stats
 */
struct nss_stats_info nss_wifili_strings_stats_tcl[NSS_WIFILI_STATS_TCL_MAX] = {
	{"tcl_no_hw_desc"	, NSS_STATS_TYPE_SPECIAL},
	{"tcl_ring_full"	, NSS_STATS_TYPE_SPECIAL},
	{"tcl_ring_sent"	, NSS_STATS_TYPE_SPECIAL}
};

/*
 * nss_wifili_strings_stats_tx_comp
 *	wifili tx comp stats
 */
struct nss_stats_info nss_wifili_strings_stats_tx_comp[NSS_WIFILI_STATS_TX_DESC_FREE_MAX] = {
	{"tx_desc_free_inv_bufsrc"	, NSS_STATS_TYPE_ERROR},
	{"tx_desc_free_inv_cookie"	, NSS_STATS_TYPE_SPECIAL},
	{"tx_desc_free_hw_ring_empty"	, NSS_STATS_TYPE_SPECIAL},
	{"tx_desc_free_reaped"		, NSS_STATS_TYPE_SPECIAL}
};

/*
 * nss_wifili_strings_stats_reo
 *	wifili tx reo stats
 */
struct nss_stats_info nss_wifili_strings_stats_reo[NSS_WIFILI_STATS_REO_MAX] = {
	{"reo_error"		, NSS_STATS_TYPE_ERROR},
	{"reo_reaped"		, NSS_STATS_TYPE_SPECIAL},
	{"reo_inv_cookie"	, NSS_STATS_TYPE_SPECIAL},
	{"stats_reo_frag_rcv"	, NSS_STATS_TYPE_SPECIAL}
};

/*
 * nss_wifili_strings_stats_txsw_pool
 *	wifili tx desc stats
 */
struct nss_stats_info nss_wifili_strings_stats_txsw_pool[NSS_WIFILI_STATS_TX_DESC_MAX] = {
	{"tx_desc_in_use"		, NSS_STATS_TYPE_SPECIAL},
	{"tx_desc_alloc_fail"		, NSS_STATS_TYPE_SPECIAL},
	{"tx_desc_already_allocated"	, NSS_STATS_TYPE_SPECIAL},
	{"tx_desc_invalid_free"		, NSS_STATS_TYPE_SPECIAL},
	{"tx_desc_free_src_fw"		, NSS_STATS_TYPE_SPECIAL},
	{"tx_desc_free_completion"	, NSS_STATS_TYPE_SPECIAL},
	{"tx_desc_no_pb"		, NSS_STATS_TYPE_SPECIAL},
	{"tx_desc_queuelimit_drop"	, NSS_STATS_TYPE_DROP}
};

/*
 * nss_wifili_strings_stats_ext_txsw_pool
 *	wifili tx ext desc stats
 */
struct nss_stats_info nss_wifili_strings_stats_ext_txsw_pool[NSS_WIFILI_STATS_EXT_TX_DESC_MAX] = {
	{"ext_tx_desc_in_use"			, NSS_STATS_TYPE_SPECIAL},
	{"ext_tx_desc_alloc_fail"		, NSS_STATS_TYPE_SPECIAL},
	{"ext_tx_desc_already_allocated"	, NSS_STATS_TYPE_SPECIAL},
	{"ext_tx_desc_invalid_free"		, NSS_STATS_TYPE_SPECIAL}
};

/*
 * nss_wifili_strings_stats_rxdma_pool
 *	wifili rx desc stats
 */
struct nss_stats_info nss_wifili_strings_stats_rxdma_pool[NSS_WIFILI_STATS_RX_DESC_MAX] = {
	{"rx_desc_no_pb"	, NSS_STATS_TYPE_SPECIAL},
	{"rx_desc_alloc_fail"	, NSS_STATS_TYPE_SPECIAL},
	{"rx_desc_in_use"	, NSS_STATS_TYPE_SPECIAL}
};

/*
 * nss_wifili_strings_stats_rxdma_ring
 *	wifili rx dma ring stats
 */
struct nss_stats_info nss_wifili_strings_stats_rxdma_ring[NSS_WIFILI_STATS_RXDMA_DESC_MAX] = {
	{"rxdma_hw_desc_unavailable"	, NSS_STATS_TYPE_SPECIAL},
	{"rxdma_buf_replenished"	, NSS_STATS_TYPE_SPECIAL}
};

/*
 * nss_wifili_strings_stats_wbm
 *	wifili wbm ring stats
 */
struct nss_stats_info nss_wifili_strings_stats_wbm[NSS_WIFILI_STATS_WBM_MAX] = {
	{"wbm_ie_local_alloc_fail"	, NSS_STATS_TYPE_ERROR},
	{"wbm_src_dma"			, NSS_STATS_TYPE_SPECIAL},
	{"wbm_src_dma_code_inv"		, NSS_STATS_TYPE_SPECIAL},
	{"wbm_src_reo"			, NSS_STATS_TYPE_SPECIAL},
	{"wbm_src_reo_code_nullq"	, NSS_STATS_TYPE_SPECIAL},
	{"wbm_src_reo_code_inv"		, NSS_STATS_TYPE_ERROR},
	{"wbm_src_inv"			, NSS_STATS_TYPE_ERROR}
};

/*
 * nss_wifili_txrx_strings_read()
 *	Read wifili Tx Rx statistics names.
 */
static ssize_t nss_wifili_txrx_strings_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	return nss_strings_print(ubuf, sz, ppos, nss_wifili_strings_stats_txrx, NSS_WIFILI_STATS_TXRX_MAX);
}

/*
 * nss_wifili_tcl_ring_strings_read()
 *	Read wifili TCL ring statistics names.
 */
static ssize_t nss_wifili_tcl_ring_strings_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	return nss_strings_print(ubuf, sz, ppos, nss_wifili_strings_stats_tcl, NSS_WIFILI_STATS_TCL_MAX);
}

/*
 * nss_wifili_tcl_comp_strings_read()
 *	Read wifili TCL comp statistics names.
 */
static ssize_t nss_wifili_tcl_comp_strings_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	return nss_strings_print(ubuf, sz, ppos, nss_wifili_strings_stats_tx_comp, NSS_WIFILI_STATS_TX_DESC_FREE_MAX);
}

/*
 * nss_wifili_reo_ring_strings_read()
 *	Read wifili reorder ring statistics names.
 */
static ssize_t nss_wifili_reo_ring_strings_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	return nss_strings_print(ubuf, sz, ppos, nss_wifili_strings_stats_reo, NSS_WIFILI_STATS_REO_MAX);
}

/*
 * nss_wifili_tx_sw_strings_read()
 *	Read wifili Tx sw statistics names.
 */
static ssize_t nss_wifili_tx_sw_strings_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	return nss_strings_print(ubuf, sz, ppos, nss_wifili_strings_stats_txsw_pool, NSS_WIFILI_STATS_TX_DESC_MAX);
}

/*
 * nss_wifili_tx_ext_sw_strings_read()
 *	Read wifili Tx ext sw statistics names.
 */
static ssize_t nss_wifili_tx_ext_sw_strings_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	return nss_strings_print(ubuf, sz, ppos, nss_wifili_strings_stats_ext_txsw_pool, NSS_WIFILI_STATS_EXT_TX_DESC_MAX);
}

/*
 * nss_wifili_rx_dma_pool_strings_read()
 *	Read wifili Rx DMA pool statistics names.
 */
static ssize_t nss_wifili_rx_dma_pool_strings_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	return nss_strings_print(ubuf, sz, ppos, nss_wifili_strings_stats_rxdma_pool, NSS_WIFILI_STATS_RX_DESC_MAX);
}

/*
 * nss_wifili_rx_dma_ring_strings_read()
 *	Read wifili Rx DMA ring statistics names.
 */
static ssize_t nss_wifili_rx_dma_ring_strings_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	return nss_strings_print(ubuf, sz, ppos, nss_wifili_strings_stats_rxdma_ring, NSS_WIFILI_STATS_RXDMA_DESC_MAX);
}

/*
 * nss_wifili_wbm_ring_strings_read()
 *	Read wifili WBM ring statistics names.
 */
static ssize_t nss_wifili_wbm_ring_strings_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	return nss_strings_print(ubuf, sz, ppos, nss_wifili_strings_stats_wbm, NSS_WIFILI_STATS_WBM_MAX);
}

/*
 * nss_wifili_txrx_strings_ops
 */
NSS_STRINGS_DECLARE_FILE_OPERATIONS(wifili_txrx);

/*
 * nss_wifili_tcl_ring_strings_ops
 */
NSS_STRINGS_DECLARE_FILE_OPERATIONS(wifili_tcl_ring);

/*
 * nss_wifili_tcl_comp_strings_ops
 */
NSS_STRINGS_DECLARE_FILE_OPERATIONS(wifili_tcl_comp);

/*
 * nss_wifili_reo_ring_strings_ops
 */
NSS_STRINGS_DECLARE_FILE_OPERATIONS(wifili_reo_ring);

/*
 * nss_wifili_tx_sw_strings_ops
 */
NSS_STRINGS_DECLARE_FILE_OPERATIONS(wifili_tx_sw);

/*
 * nss_wifili_tx_ext_sw_strings_ops
 */
NSS_STRINGS_DECLARE_FILE_OPERATIONS(wifili_tx_ext_sw);

/*
 * nss_wifili_rx_dma_pool_strings_ops
 */
NSS_STRINGS_DECLARE_FILE_OPERATIONS(wifili_rx_dma_pool);

/*
 * nss_wifili_rx_dma_ring_strings_ops
 */
NSS_STRINGS_DECLARE_FILE_OPERATIONS(wifili_rx_dma_ring);

/*
 * nss_wifili_wbm_ring_strings_ops
 */
NSS_STRINGS_DECLARE_FILE_OPERATIONS(wifili_wbm_ring);

/*
 * nss_wifili_strings_dentry_create()
 *	Create wifili statistics strings debug entry.
 */
void nss_wifili_strings_dentry_create(void)
{
	struct dentry *wifili_d = NULL;
	struct dentry *wifili_txrx_d = NULL;
	struct dentry *wifili_tcl_ring_d = NULL;
	struct dentry *wifili_tcl_comp_d = NULL;
	struct dentry *wifili_reo_ring_d = NULL;
	struct dentry *wifili_tx_sw_d = NULL;
	struct dentry *wifili_tx_ext_sw_d = NULL;
	struct dentry *wifili_rx_dma_pool_d = NULL;
	struct dentry *wifili_rx_dma_ring_d = NULL;
	struct dentry *wifili_wbm_ring_d = NULL;

	if (!nss_top_main.strings_dentry) {
		nss_warning("qca-nss-drv/strings is not present");
		return;
	}

	wifili_d = debugfs_create_dir("wifili", nss_top_main.strings_dentry);
	if (!wifili_d) {
		nss_warning("Failed to create qca-nss-drv/strings/wifili directory");
		return;
	}

	wifili_txrx_d = debugfs_create_file("txrx_str", 0400, wifili_d, &nss_top_main, &nss_wifili_txrx_strings_ops);
	if (!wifili_txrx_d) {
		nss_warning("Failed to create qca-nss-drv/strings/wifili/txrx_str file");
		debugfs_remove_recursive(wifili_d);
		return;
	}

	wifili_tcl_ring_d = debugfs_create_file("tcl_ring_str", 0400, wifili_d, &nss_top_main, &nss_wifili_tcl_ring_strings_ops);
	if (!wifili_tcl_ring_d) {
		nss_warning("Failed to create qca-nss-drv/strings/wifili/tcl_ring_str file");
		debugfs_remove_recursive(wifili_d);
		return;
	}

	wifili_tcl_comp_d = debugfs_create_file("tcl_comp_str", 0400, wifili_d, &nss_top_main, &nss_wifili_tcl_comp_strings_ops);
	if (!wifili_tcl_comp_d) {
		nss_warning("Failed to create qca-nss-drv/strings/wifili/tcl_comp_str file");
		debugfs_remove_recursive(wifili_d);
		return;
	}

	wifili_reo_ring_d = debugfs_create_file("reo_ring_str", 0400, wifili_d, &nss_top_main, &nss_wifili_reo_ring_strings_ops);
	if (!wifili_reo_ring_d) {
		nss_warning("Failed to create qca-nss-drv/strings/wifili/reo_ring_str file");
		debugfs_remove_recursive(wifili_d);
		return;
	}

	wifili_tx_sw_d = debugfs_create_file("tx_sw_str", 0400, wifili_d, &nss_top_main, &nss_wifili_tx_sw_strings_ops);
	if (!wifili_tx_sw_d) {
		nss_warning("Failed to create qca-nss-drv/strings/wifili/tx_sw_str file");
		debugfs_remove_recursive(wifili_d);
		return;
	}

	wifili_tx_ext_sw_d = debugfs_create_file("tx_ext_sw_str", 0400, wifili_d, &nss_top_main, &nss_wifili_tx_ext_sw_strings_ops);
	if (!wifili_tx_ext_sw_d) {
		nss_warning("Failed to create qca-nss-drv/strings/wifili/tx_ext_sw_str file");
		debugfs_remove_recursive(wifili_d);
		return;
	}

	wifili_rx_dma_pool_d = debugfs_create_file("rx_dma_pool_str", 0400, wifili_d, &nss_top_main, &nss_wifili_rx_dma_pool_strings_ops);
	if (!wifili_rx_dma_pool_d) {
		nss_warning("Failed to create qca-nss-drv/strings/wifili/rx_dma_pool_str file");
		debugfs_remove_recursive(wifili_d);
		return;
	}

	wifili_rx_dma_ring_d = debugfs_create_file("rx_dma_ring_str", 0400, wifili_d, &nss_top_main, &nss_wifili_rx_dma_ring_strings_ops);
	if (!wifili_rx_dma_ring_d) {
		nss_warning("Failed to create qca-nss-drv/strings/wifili/rx_dma_ring_str file");
		debugfs_remove_recursive(wifili_d);
		return;
	}

	wifili_wbm_ring_d = debugfs_create_file("wbm_ring_str", 0400, wifili_d, &nss_top_main, &nss_wifili_wbm_ring_strings_ops);
	if (!wifili_wbm_ring_d) {
		nss_warning("Failed to create qca-nss-drv/strings/wifili/wbm_ring_str file");
		debugfs_remove_recursive(wifili_d);
		return;
	}
}

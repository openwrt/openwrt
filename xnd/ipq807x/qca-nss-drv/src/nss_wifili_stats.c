/*
 **************************************************************************
 * Copyright (c) 2017-2020, The Linux Foundation. All rights reserved.
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
 * nss_wifili_stats.c
 *	NSS wifili statistics APIs
 */

#include "nss_tx_rx_common.h"
#include "nss_core.h"
#include "nss_wifili_if.h"
#include "nss_wifili_stats.h"
#include "nss_wifili_strings.h"

/*
 * Declare atomic notifier data structure for statistics.
 */
ATOMIC_NOTIFIER_HEAD(nss_wifili_stats_notifier);

/*
 * Statistics structures
 * The structure will hold the statistics for 3 SOCs.
 */
struct nss_wifili_soc_stats soc_stats[NSS_WIFILI_MAX_SOC_NUM];

/*
 * nss_wifili_stats_read()
 *	Read wifili statistics
 */
static ssize_t nss_wifili_stats_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	uint32_t i;

	/*
	 * max output lines = ((#stats + eight blank lines) * #WIFILI #STATS) + start/end tag + 3 blank
	 * + Number of Extra outputlines for future reference to add new stats
	 */
	uint32_t max_pdev = 0;
	uint32_t max_output_lines;
	size_t size_al = 0;
	size_t size_wr = 0;
	ssize_t bytes_read = 0;
	char *lbuf = NULL;
	uint32_t soc_idx;
	struct nss_wifili_stats *stats_wifili = NULL;

	/*
	 * Max number of pdev depends on type of soc (Internal/Attached).
	 */
	for (soc_idx = 0; soc_idx < NSS_WIFILI_MAX_SOC_NUM; soc_idx++) {
		max_pdev += soc_stats[soc_idx].soc_maxpdev;
	}

	/*
	 * Max pdev cannot be null.
	 */
	if (unlikely(max_pdev == 0)) {
		nss_warning("Cannot have max pdev zero ");
		return 0;
	}

	max_output_lines = (((NSS_WIFILI_STATS_MAX + 9) * max_pdev) +
				NSS_WIFILI_STATS_WBM_MAX + NSS_STATS_EXTRA_OUTPUT_LINES);

	size_al = NSS_STATS_MAX_STR_LENGTH * max_output_lines;

	lbuf = kzalloc(size_al, GFP_KERNEL);
	if (unlikely(lbuf == NULL)) {
		nss_warning("Could not allocate memory for local statistics buffer");
		return 0;
	}

	size_wr += nss_stats_banner(lbuf, size_wr, size_al, "wifili", NSS_STATS_SINGLE_CORE);

	for (soc_idx = 0; soc_idx < NSS_WIFILI_MAX_SOC_NUM; soc_idx++) {
		stats_wifili = &(soc_stats[soc_idx].stats_wifili);
		for (i = 0; i < soc_stats[soc_idx].soc_maxpdev; i++) {

			spin_lock_bh(&nss_top_main.stats_lock);
			size_wr += nss_stats_print("wifili", "txrx", i
					, nss_wifili_strings_stats_txrx
					, stats_wifili->stats_txrx[i]
					, NSS_WIFILI_STATS_TXRX_MAX
					, lbuf, size_wr, size_al);
			spin_unlock_bh(&nss_top_main.stats_lock);
			size_wr += scnprintf(lbuf + size_wr
					, size_al - size_wr, "\n");

			/*
			 * Filling TCL ring stats
			 */
			spin_lock_bh(&nss_top_main.stats_lock);
			size_wr += nss_stats_print("wifili", "tcl ring", i
					, nss_wifili_strings_stats_tcl
					, stats_wifili->stats_tcl_ring[i]
					, NSS_WIFILI_STATS_TCL_MAX
					, lbuf, size_wr, size_al);
			spin_unlock_bh(&nss_top_main.stats_lock);
			size_wr += scnprintf(lbuf + size_wr
					, size_al - size_wr, "\n");

			/*
			 * Filling TCL comp stats
			 */
			spin_lock_bh(&nss_top_main.stats_lock);
			size_wr += nss_stats_print("wifili", "tcl comp", i
					, nss_wifili_strings_stats_tx_comp
					, stats_wifili->stats_tx_comp[i]
					, NSS_WIFILI_STATS_TX_DESC_FREE_MAX
					, lbuf, size_wr, size_al);
			spin_unlock_bh(&nss_top_main.stats_lock);
			size_wr += scnprintf(lbuf + size_wr
					, size_al - size_wr, "\n");

			/*
			 * Filling reo ring stats
			 */
			spin_lock_bh(&nss_top_main.stats_lock);
			size_wr += nss_stats_print("wifili", "reo ring", i
					, nss_wifili_strings_stats_reo
					, stats_wifili->stats_reo[i]
					, NSS_WIFILI_STATS_REO_MAX
					, lbuf, size_wr, size_al);

			spin_unlock_bh(&nss_top_main.stats_lock);
			size_wr += scnprintf(lbuf + size_wr
					, size_al - size_wr, "\n");

			/*
			 * Filling TX SW Pool
			 */
			spin_lock_bh(&nss_top_main.stats_lock);
			size_wr += nss_stats_print("wifili", "tx sw pool", i
					, nss_wifili_strings_stats_txsw_pool
					, stats_wifili->stats_tx_desc[i]
					, NSS_WIFILI_STATS_TX_DESC_MAX
					, lbuf, size_wr, size_al);
			spin_unlock_bh(&nss_top_main.stats_lock);
			size_wr += scnprintf(lbuf + size_wr
					, size_al - size_wr, "\n");

			/*
			 * Filling TX EXt SW Pool
			 */
			spin_lock_bh(&nss_top_main.stats_lock);
			size_wr += nss_stats_print("wifili", "tx ext sw pool", i
					, nss_wifili_strings_stats_ext_txsw_pool
					, stats_wifili->stats_ext_tx_desc[i]
					, NSS_WIFILI_STATS_EXT_TX_DESC_MAX
					, lbuf, size_wr, size_al);
			spin_unlock_bh(&nss_top_main.stats_lock);
			size_wr += scnprintf(lbuf + size_wr
					, size_al - size_wr, "\n");

			/*
			 * Filling rxdma pool stats
			 */
			spin_lock_bh(&nss_top_main.stats_lock);
			size_wr += nss_stats_print("wifili", "rxdma pool", i
					, nss_wifili_strings_stats_rxdma_pool
					, stats_wifili->stats_rx_desc[i]
					, NSS_WIFILI_STATS_RX_DESC_MAX
					, lbuf, size_wr, size_al);
			spin_unlock_bh(&nss_top_main.stats_lock);
			size_wr += scnprintf(lbuf + size_wr
					, size_al - size_wr, "\n");

			/*
			 * Filling rxdma ring stats
			 */
			spin_lock_bh(&nss_top_main.stats_lock);
			size_wr += nss_stats_print("wifili", "rxdma ring", i
					, nss_wifili_strings_stats_rxdma_ring
					, stats_wifili->stats_rxdma[i]
					, NSS_WIFILI_STATS_RXDMA_DESC_MAX
					, lbuf, size_wr, size_al);
			spin_unlock_bh(&nss_top_main.stats_lock);
			size_wr += scnprintf(lbuf + size_wr
					, size_al - size_wr, "\n");
		}

		/*
		 * Filling wbm ring stats
		 */
		spin_lock_bh(&nss_top_main.stats_lock);
		size_wr += nss_stats_print("wifili", "wbm ring"
				, NSS_STATS_SINGLE_INSTANCE
				, nss_wifili_strings_stats_wbm
				, stats_wifili->stats_wbm
				, NSS_WIFILI_STATS_WBM_MAX
				, lbuf, size_wr, size_al);
		spin_unlock_bh(&nss_top_main.stats_lock);
		size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, "\n");
	}

	bytes_read = simple_read_from_buffer(ubuf, sz, ppos, lbuf, strlen(lbuf));
	kfree(lbuf);

	return bytes_read;
}

/*
 * wifili_stats_ops
 */
NSS_STATS_DECLARE_FILE_OPERATIONS(wifili);

/*
 * nss_wifili_stats_dentry_create()
 *	Create wifili statistics debug entry.
 */
void nss_wifili_stats_dentry_create(void)
{
	nss_stats_create_dentry("wifili", &nss_wifili_stats_ops);
}

/*
 * nss_wifili_stats_sync()
 *	Handle the syncing of WIFI stats.
 */
void nss_wifili_stats_sync(struct nss_ctx_instance *nss_ctx,
		struct nss_wifili_stats_sync_msg *wlsoc_stats, uint16_t interface)
{
	struct nss_top_instance *nss_top = nss_ctx->nss_top;
	struct nss_wifili_soc_stats *nwss = NULL;
	struct nss_wifili_stats *stats = NULL;
	struct nss_wifili_device_stats *devstats = &wlsoc_stats->stats;
	uint32_t index;

	/*
	 * Max number of pdev depends on type of soc (Internal/Attached).
	 */
	switch (interface) {
	case NSS_WIFILI_INTERNAL_INTERFACE:
		nwss = &soc_stats[0];
		nwss->soc_maxpdev = NSS_WIFILI_MAX_PDEV_NUM_MSG;
		break;

	case NSS_WIFILI_EXTERNAL_INTERFACE0:
		nwss = &soc_stats[1];
		nwss->soc_maxpdev = NSS_WIFILI_SOC_ATTACHED_MAX_PDEV_NUM;
		break;

	case NSS_WIFILI_EXTERNAL_INTERFACE1:
		nwss = &soc_stats[2];
		nwss->soc_maxpdev = NSS_WIFILI_SOC_ATTACHED_MAX_PDEV_NUM;
		break;

	default:
		nss_warning("%px: Invalid wifili interface\n", nss_ctx);
		return;
	}

	/*
	 * Wifili statistics structure.
	 */
	stats = &(nwss->stats_wifili);

	spin_lock_bh(&nss_top->stats_lock);

	for (index = 0; index < nwss->soc_maxpdev; index++) {
		/*
		 * Rx stats
		 */
		stats->stats_txrx[index][NSS_WIFILI_STATS_RX_MSDU_ERROR] +=
							devstats->rx_data_stats[index].rx_msdu_err;
		stats->stats_txrx[index][NSS_WIFILI_STATS_RX_INV_PEER_RCV] +=
							(devstats->rx_data_stats[index].rx_inv_peer +
							devstats->rx_data_stats[index].rx_scatter_inv_peer);
		stats->stats_txrx[index][NSS_WIFILI_STATS_RX_WDS_SRCPORT_EXCEPTION] +=
							devstats->rx_data_stats[index].rx_wds_learn_send;
		stats->stats_txrx[index][NSS_WIFILI_STATS_RX_WDS_SRCPORT_EXCEPTION_FAIL] +=
							devstats->rx_data_stats[index].rx_wds_learn_send_fail;
		stats->stats_txrx[index][NSS_WIFILI_STATS_RX_DELIVERD] +=
							devstats->rx_data_stats[index].rx_deliver_cnt;
		stats->stats_txrx[index][NSS_WIFILI_STATS_RX_DELIVER_DROPPED] +=
							devstats->rx_data_stats[index].rx_deliver_cnt_fail;
		stats->stats_txrx[index][NSS_WIFILI_STATS_RX_INTRA_BSS_UCAST] +=
							devstats->rx_data_stats[index].rx_intra_bss_ucast_send;
		stats->stats_txrx[index][NSS_WIFILI_STATS_RX_INTRA_BSS_UCAST_FAIL] +=
							devstats->rx_data_stats[index].rx_intra_bss_ucast_send_fail;
		stats->stats_txrx[index][NSS_WIFILI_STATS_RX_INTRA_BSS_MCAST] +=
							devstats->rx_data_stats[index].rx_intra_bss_mcast_send;
		stats->stats_txrx[index][NSS_WIFILI_STATS_RX_INTRA_BSS_MCAST_FAIL] +=
							devstats->rx_data_stats[index].rx_intra_bss_mcast_send_fail;
		stats->stats_txrx[index][NSS_WIFILI_STATS_RX_SG_RCV_SEND] +=
							devstats->rx_data_stats[index].rx_sg_recv_send;
		stats->stats_txrx[index][NSS_WIFILI_STATS_RX_SG_RCV_FAIL] +=
							devstats->rx_data_stats[index].rx_sg_recv_fail;
		stats->stats_txrx[index][NSS_STATS_WIFILI_RX_MCAST_ECHO] +=
							devstats->rx_data_stats[index].rx_me_pkts;
		stats->stats_txrx[index][NSS_STATS_WIFILI_RX_INV_TID] +=
							devstats->rx_data_stats[index].rx_inv_tid;
		stats->stats_txrx[index][NSS_WIFILI_STATS_RX_FRAG_INV_SC] +=
							devstats->rx_data_stats[index].rx_frag_inv_sc;
		stats->stats_txrx[index][NSS_WIFILI_STATS_RX_FRAG_INV_FC] +=
							devstats->rx_data_stats[index].rx_frag_inv_fc;
		stats->stats_txrx[index][NSS_WIFILI_STATS_RX_FRAG_NON_FRAG] +=
							devstats->rx_data_stats[index].rx_non_frag_err;
		stats->stats_txrx[index][NSS_WIFILI_STATS_RX_FRAG_RETRY] +=
							devstats->rx_data_stats[index].rx_repeat_fragno;
		stats->stats_txrx[index][NSS_WIFILI_STATS_RX_FRAG_OOO] +=
							devstats->rx_data_stats[index].rx_ooo_frag;
		stats->stats_txrx[index][NSS_WIFILI_STATS_RX_FRAG_OOO_SEQ] +=
							devstats->rx_data_stats[index].rx_ooo_frag_seq;
		stats->stats_txrx[index][NSS_WIFILI_STATS_RX_FRAG_ALL_FRAG_RCV] +=
							devstats->rx_data_stats[index].rx_all_frag_rcv;
		stats->stats_txrx[index][NSS_WIFILI_STATS_RX_FRAG_DELIVER] +=
							devstats->rx_data_stats[index].rx_frag_deliver;

		/*
		 * Tx stats
		 */
		stats->stats_txrx[index][NSS_WIFILI_STATS_TX_ENQUEUE] +=
							devstats->tx_data_stats[index].tx_enqueue_cnt;
		stats->stats_txrx[index][NSS_WIFILI_STATS_TX_ENQUEUE_DROP] +=
							devstats->tx_data_stats[index].tx_enqueue_dropped;
		stats->stats_txrx[index][NSS_WIFILI_STATS_TX_DEQUEUE] +=
							devstats->tx_data_stats[index].tx_dequeue_cnt;
		stats->stats_txrx[index][NSS_WIFILI_STATS_TX_HW_ENQUEUE_FAIL] +=
							devstats->tx_data_stats[index].tx_send_fail_cnt;
		stats->stats_txrx[index][NSS_WIFILI_STATS_TX_SENT_COUNT] +=
							devstats->tx_data_stats[index].tx_processed_pkt;
	}

	/*
	 * update the tcl ring stats
	 */
	for (index = 0; index < NSS_WIFILI_MAX_TCL_DATA_RINGS_MSG; index++) {
		stats->stats_tcl_ring[index][NSS_WIFILI_STATS_TCL_NO_HW_DESC] +=
							devstats->tcl_stats[index].tcl_no_hw_desc;
		stats->stats_tcl_ring[index][NSS_WIFILI_STATS_TCL_RING_FULL] +=
							devstats->tcl_stats[index].tcl_ring_full;
		stats->stats_tcl_ring[index][NSS_WIFILI_STATS_TCL_RING_SENT] +=
							devstats->tcl_stats[index].tcl_ring_sent;
	}

	/*
	 * update the tcl comp stats
	 */
	for (index = 0; index < NSS_WIFILI_MAX_TCL_DATA_RINGS_MSG; index++) {
		stats->stats_tx_comp[index][NSS_WIFILI_STATS_TX_DESC_FREE_INV_BUFSRC] +=
								devstats->txcomp_stats[index].invalid_bufsrc;
		stats->stats_tx_comp[index][NSS_WIFILI_STATS_TX_DESC_FREE_INV_COOKIE] +=
								devstats->txcomp_stats[index].invalid_cookie;
		stats->stats_tx_comp[index][NSS_WIFILI_STATS_TX_DESC_FREE_HW_RING_EMPTY] +=
								devstats->txcomp_stats[index].hw_ring_empty;
		stats->stats_tx_comp[index][NSS_WIFILI_STATS_TX_DESC_FREE_REAPED] +=
								devstats->txcomp_stats[index].ring_reaped;
	}

	/*
	 * update reo ring stats
	 */
	for (index = 0; index < NSS_WIFILI_MAX_REO_DATA_RINGS_MSG; index++) {
		stats->stats_reo[index][NSS_WIFILI_STATS_REO_ERROR] +=
								devstats->rxreo_stats[index].ring_error;
		stats->stats_reo[index][NSS_WIFILI_STATS_REO_REAPED] +=
								devstats->rxreo_stats[index].ring_reaped;
		stats->stats_reo[index][NSS_WIFILI_STATS_REO_INV_COOKIE] +=
								devstats->rxreo_stats[index].invalid_cookie;
		stats->stats_reo[index][NSS_WIFILI_STATS_REO_FRAG_RCV] +=
								devstats->rxreo_stats[index].defrag_reaped;
	}

	/*
	 * update tx sw pool
	 */
	for (index = 0; index < NSS_WIFILI_MAX_TXDESC_POOLS_MSG; index++) {
		stats->stats_tx_desc[index][NSS_WIFILI_STATS_TX_DESC_IN_USE] =
								devstats->tx_sw_pool_stats[index].desc_alloc;
		stats->stats_tx_desc[index][NSS_WIFILI_STATS_TX_DESC_ALLOC_FAIL] +=
								devstats->tx_sw_pool_stats[index].desc_alloc_fail;
		stats->stats_tx_desc[index][NSS_WIFILI_STATS_TX_DESC_ALREADY_ALLOCATED] +=
								devstats->tx_sw_pool_stats[index].desc_already_allocated;
		stats->stats_tx_desc[index][NSS_WIFILI_STATS_TX_DESC_INVALID_FREE] +=
								devstats->tx_sw_pool_stats[index].desc_invalid_free;
		stats->stats_tx_desc[index][NSS_WIFILI_STATS_TX_DESC_FREE_SRC_FW] +=
								devstats->tx_sw_pool_stats[index].tx_rel_src_fw;
		stats->stats_tx_desc[index][NSS_WIFILI_STATS_TX_DESC_FREE_COMPLETION] +=
								devstats->tx_sw_pool_stats[index].tx_rel_tx_desc;
		stats->stats_tx_desc[index][NSS_WIFILI_STATS_TX_DESC_NO_PB] +=
								devstats->tx_sw_pool_stats[index].tx_rel_no_pb;
		stats->stats_tx_desc[index][NSS_WIFILI_STATS_TX_QUEUELIMIT_DROP] +=
								devstats->tx_sw_pool_stats[index].tx_queue_limit_drop;
	}

	/*
	 * update ext tx desc pool stats
	 */
	for (index = 0; index < NSS_WIFILI_MAX_TX_EXT_DESC_POOLS_MSG; index++) {
		stats->stats_ext_tx_desc[index][NSS_WIFILI_STATS_EXT_TX_DESC_IN_USE] =
								devstats->tx_ext_sw_pool_stats[index].desc_alloc;
		stats->stats_ext_tx_desc[index][NSS_WIFILI_STATS_EXT_TX_DESC_ALLOC_FAIL] +=
								devstats->tx_ext_sw_pool_stats[index].desc_alloc_fail;
		stats->stats_ext_tx_desc[index][NSS_WIFILI_STATS_EXT_TX_DESC_ALREADY_ALLOCATED] +=
								devstats->tx_ext_sw_pool_stats[index].desc_already_allocated;
		stats->stats_ext_tx_desc[index][NSS_WIFILI_STATS_EXT_TX_DESC_INVALID_FREE] +=
								devstats->tx_ext_sw_pool_stats[index].desc_invalid_free;
	}

	/*
	 * update rx desc pool stats
	 */
	for (index = 0; index < nwss->soc_maxpdev; index++) {
		stats->stats_rx_desc[index][NSS_WIFILI_STATS_RX_DESC_NO_PB] +=
								devstats->rx_sw_pool_stats[index].rx_no_pb;
		stats->stats_rx_desc[index][NSS_WIFILI_STATS_RX_DESC_ALLOC_FAIL] +=
								devstats->rx_sw_pool_stats[index].desc_alloc_fail;
		stats->stats_rx_desc[index][NSS_WIFILI_STATS_RX_DESC_IN_USE] =
								devstats->rx_sw_pool_stats[index].desc_alloc;
	}

	/*
	 * update rx dma ring stats
	 */
	for (index = 0; index < nwss->soc_maxpdev; index++) {
		stats->stats_rxdma[index][NSS_WIFILI_STATS_RXDMA_DESC_UNAVAILABLE] +=
								devstats->rxdma_stats[index].rx_hw_desc_unavailable;
		stats->stats_rxdma[index][NSS_WIFILI_STATS_RXDMA_BUF_REPLENISHED] +=
								devstats->rxdma_stats[index].rx_buf_replenished;
	}

	/*
	 * update wbm ring stats
	 */
	stats->stats_wbm[NSS_WIFILI_STATS_WBM_IE_LOCAL_ALLOC_FAIL] += devstats->rxwbm_stats.invalid_buf_mgr;
	stats->stats_wbm[NSS_WIFILI_STATS_WBM_SRC_DMA] += devstats->rxwbm_stats.err_src_rxdma;
	stats->stats_wbm[NSS_WIFILI_STATS_WBM_SRC_DMA_CODE_INV] += devstats->rxwbm_stats.err_src_rxdma_code_inv;
	stats->stats_wbm[NSS_WIFILI_STATS_WBM_SRC_REO] += devstats->rxwbm_stats.err_src_reo;
	stats->stats_wbm[NSS_WIFILI_STATS_WBM_SRC_REO_CODE_NULLQ] += devstats->rxwbm_stats.err_src_reo_code_nullq;
	stats->stats_wbm[NSS_WIFILI_STATS_WBM_SRC_REO_CODE_INV] += devstats->rxwbm_stats.err_src_reo_code_inv;
	stats->stats_wbm[NSS_WIFILI_STATS_WBM_SRC_INV] += devstats->rxwbm_stats.err_src_invalid;
	spin_unlock_bh(&nss_top->stats_lock);
	return;
}

/*
 * nss_wifili_stats_notify()
 *	Sends notifications to the registered modules.
 *
 * Leverage NSS-FW statistics timing to update Netlink.
 */
void nss_wifili_stats_notify(struct nss_ctx_instance *nss_ctx, uint32_t if_num)
{
	struct nss_wifili_stats_notification *wifili_stats;
	uint32_t index = 0;

	wifili_stats = kzalloc(sizeof(struct nss_wifili_stats_notification), GFP_ATOMIC);
	if (!wifili_stats) {
		nss_warning("%px: Failed to allocate memory for wifili stats\n", nss_ctx);
		return;
	}

	wifili_stats->core_id = nss_ctx->id;
	switch (if_num) {
	case NSS_WIFILI_INTERNAL_INTERFACE:
		index = 0;
		break;

	case NSS_WIFILI_EXTERNAL_INTERFACE0:
		index = 1;
		break;

	case NSS_WIFILI_EXTERNAL_INTERFACE1:
		index = 2;
		break;

	default:
		nss_warning("%px: Invalid wifili interface\n", nss_ctx);
		goto done;
	}
	wifili_stats->if_num = if_num;
	memcpy(&wifili_stats->stats, &soc_stats[index].stats_wifili, sizeof(wifili_stats->stats));
	atomic_notifier_call_chain(&nss_wifili_stats_notifier, NSS_STATS_EVENT_NOTIFY, (void *)wifili_stats);

done:
	kfree(wifili_stats);
	return;
}

/*
 * nss_wifili_stats_register_notifier()
 *	Registers statistics notifier.
 */
int nss_wifili_stats_register_notifier(struct notifier_block *nb)
{
	return atomic_notifier_chain_register(&nss_wifili_stats_notifier, nb);
}
EXPORT_SYMBOL(nss_wifili_stats_register_notifier);

/*
 * nss_wifili_stats_unregister_notifier()
 *	Deregisters statistics notifier.
 */
int nss_wifili_stats_unregister_notifier(struct notifier_block *nb)
{
	return atomic_notifier_chain_unregister(&nss_wifili_stats_notifier, nb);
}
EXPORT_SYMBOL(nss_wifili_stats_unregister_notifier);

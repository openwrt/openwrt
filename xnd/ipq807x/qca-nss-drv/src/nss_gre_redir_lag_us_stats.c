/*
 **************************************************************************
 * Copyright (c) 2019, 2021, The Linux Foundation. All rights reserved.
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
#include "nss_gre_redir_lag.h"
#include "nss_gre_redir_lag_us_stats.h"
#include "nss_gre_redir_lag_us_strings.h"

#define NSS_GRE_REDIR_LAG_US_STATS_SYNC_PERIOD msecs_to_jiffies(4000)
#define NSS_GRE_REDIR_LAG_US_STATS_SYNC_UDELAY 4000

/*
 * Declare atomic notifier data structure for statistics.
 */
ATOMIC_NOTIFIER_HEAD(nss_gre_redir_lag_us_stats_notifier);

extern struct nss_gre_redir_lag_us_cmn_ctx cmn_ctx;

/*
 * nss_gre_redir_lag_us_stats_get
 *	Get the common upstream statistics.
 */
bool nss_gre_redir_lag_us_stats_get(struct nss_gre_redir_lag_us_tunnel_stats *cmn_stats, uint32_t index)
{
	if (index >= NSS_GRE_REDIR_LAG_MAX_NODE) {
		nss_warning("Index is out of valid range %u\n", index);
		return false;
	}

	spin_lock_bh(&cmn_ctx.nss_gre_redir_lag_us_stats_lock);
	if (!cmn_ctx.stats_ctx[index].valid) {
		spin_unlock_bh(&cmn_ctx.nss_gre_redir_lag_us_stats_lock);
		nss_warning("Common context not found for the index %u\n", index);
		return false;
	}

	memcpy((void *)cmn_stats, (void *)&(cmn_ctx.stats_ctx[index].tun_stats), sizeof(*cmn_stats));
	spin_unlock_bh(&cmn_ctx.nss_gre_redir_lag_us_stats_lock);
	return true;
}

/*
 * nss_gre_redir_lag_us_cmn_stats_read()
 *	Read and copy stats to user buffer.
 */
static ssize_t nss_gre_redir_lag_us_cmn_stats_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	/*
	 * Max output lines = #stats +
	 * few blank lines for banner printing + Number of Extra outputlines
	 * for future reference to add new stats
	 */
	uint32_t max_output_lines = NSS_GRE_REDIR_LAG_US_STATS_MAX + NSS_STATS_EXTRA_OUTPUT_LINES;
	size_t size_al = NSS_STATS_MAX_STR_LENGTH * max_output_lines;
	struct nss_stats_data *data = fp->private_data;
	struct nss_gre_redir_lag_us_tunnel_stats stats;
	ssize_t bytes_read = 0;
	size_t size_wr = 0;

	char *lbuf = kzalloc(size_al, GFP_KERNEL);
	if (unlikely(!lbuf)) {
		nss_warning("Could not allocate memory for local statistics buffer");
		return 0;
	}

	while (data->index < NSS_GRE_REDIR_LAG_MAX_NODE) {
		if (nss_gre_redir_lag_us_stats_get(&stats, data->index)) {
			break;
		}

		data->index++;
	}

	if (data->index == NSS_GRE_REDIR_LAG_MAX_NODE) {
		kfree(lbuf);
		return 0;
	}

	size_wr += nss_stats_banner(lbuf, size_wr, size_al, "gre_redir_lag_us stats", NSS_STATS_SINGLE_CORE);
	size_wr += nss_stats_print("gre_redir_lag_us", NULL, NSS_STATS_SINGLE_INSTANCE, nss_gre_redir_lag_us_strings_stats,
					&stats.rx_packets, NSS_GRE_REDIR_LAG_US_STATS_MAX, lbuf, size_wr, size_al);

	bytes_read = simple_read_from_buffer(ubuf, sz, ppos, lbuf, strlen(lbuf));
	data->index++;
	kfree(lbuf);
	return bytes_read;
}

/*
 * nss_gre_redir_lag_us_stats_ops
 */
NSS_STATS_DECLARE_FILE_OPERATIONS(gre_redir_lag_us_cmn)

/*
 * nss_gre_redir_lag_us_stats_dentry_create()
 *	Create debugfs directory for stats.
 */
struct dentry *nss_gre_redir_lag_us_stats_dentry_create(void)
{
	struct dentry *gre_redir;
	struct dentry *cmn_stats;

	gre_redir = nss_gre_redir_get_dentry();
	if (unlikely(!gre_redir)) {
		nss_warning("Failed to retrieve directory entry qca-nss-drv/stats/gre_redir/\n");
		return NULL;
	}

	cmn_stats = debugfs_create_file("lag_us_cmn_stats", 0400, gre_redir,
			&nss_top_main, &nss_gre_redir_lag_us_cmn_stats_ops);
	if (unlikely(!cmn_stats)) {
		nss_warning("Failed to create qca-nss-drv/stats/gre_redir/lag_us_cmn_stats file\n");
		return NULL;
	}

	return cmn_stats;
}

/*
 * nss_gre_redir_lag_us_stats_sync()
 *	Update synchonized statistics.
 */
void nss_gre_redir_lag_us_stats_sync(struct nss_ctx_instance *nss_ctx,
				struct nss_gre_redir_lag_us_cmn_sync_stats_msg *ngss, uint32_t ifnum)
{
	int idx, j;
	struct nss_gre_redir_lag_us_tunnel_stats *node_stats;

	spin_lock_bh(&cmn_ctx.nss_gre_redir_lag_us_stats_lock);
	if (!nss_gre_redir_lag_us_get_node_idx(ifnum, &idx)) {
		spin_unlock_bh(&cmn_ctx.nss_gre_redir_lag_us_stats_lock);
		nss_warning("%px: Unable to update hash stats msg. Stats context not found.\n", nss_ctx);
		return;
	}

	node_stats = &cmn_ctx.stats_ctx[idx].tun_stats;

	node_stats->tx_packets += ngss->node_stats.tx_packets;
	node_stats->tx_bytes += ngss->node_stats.tx_bytes;
	node_stats->rx_packets += ngss->node_stats.rx_packets;
	node_stats->rx_bytes += ngss->node_stats.rx_bytes;
	for (j = 0; j < NSS_MAX_NUM_PRI; j++) {
		node_stats->rx_dropped[j] += ngss->node_stats.rx_dropped[j];
	}

	node_stats->us_stats.amsdu_pkts += ngss->us_stats.amsdu_pkts;
	node_stats->us_stats.amsdu_pkts_enqueued += ngss->us_stats.amsdu_pkts_enqueued;
	node_stats->us_stats.amsdu_pkts_exceptioned += ngss->us_stats.amsdu_pkts_exceptioned;
	node_stats->us_stats.exceptioned += ngss->us_stats.exceptioned;
	node_stats->us_stats.freed += ngss->us_stats.freed;
	node_stats->db_stats.add_attempt += ngss->db_stats.add_attempt;
	node_stats->db_stats.add_success += ngss->db_stats.add_success;
	node_stats->db_stats.add_fail_table_full += ngss->db_stats.add_fail_table_full;
	node_stats->db_stats.add_fail_exists += ngss->db_stats.add_fail_exists;
	node_stats->db_stats.del_attempt += ngss->db_stats.del_attempt;
	node_stats->db_stats.del_success += ngss->db_stats.del_success;
	node_stats->db_stats.del_fail_not_found += ngss->db_stats.del_fail_not_found;
	spin_unlock_bh(&cmn_ctx.nss_gre_redir_lag_us_stats_lock);
}

/*
 * nss_gre_redir_lag_us_stats_notify()
 *	Sends notifications to all the registered modules.
 *
 * Leverage NSS-FW statistics timing to update Netlink.
 */
void nss_gre_redir_lag_us_stats_notify(struct nss_ctx_instance *nss_ctx, uint32_t if_num)
{
	struct nss_gre_redir_lag_us_stats_notification *stats_notify;
	int idx;

	stats_notify = kzalloc(sizeof(struct nss_gre_redir_lag_us_stats_notification), GFP_ATOMIC);
	if (!stats_notify) {
		nss_warning("Unable to allocate memory for stats notification\n");
		return;
	}

	spin_lock_bh(&cmn_ctx.nss_gre_redir_lag_us_stats_lock);
	if (!nss_gre_redir_lag_us_get_node_idx(if_num, &idx)) {
		spin_unlock_bh(&cmn_ctx.nss_gre_redir_lag_us_stats_lock);
		nss_warning("%px: Unable to update hash stats msg. Stats context not found.\n", nss_ctx);
		kfree(stats_notify);
		return;
	}

	stats_notify->core_id = nss_ctx->id;
	stats_notify->if_num = if_num;
	memcpy(&(stats_notify->stats_ctx), &(cmn_ctx.stats_ctx[idx].tun_stats), sizeof(stats_notify->stats_ctx));
	spin_unlock_bh(&cmn_ctx.nss_gre_redir_lag_us_stats_lock);
	atomic_notifier_call_chain(&nss_gre_redir_lag_us_stats_notifier, NSS_STATS_EVENT_NOTIFY, stats_notify);
	kfree(stats_notify);
}

/*
 * nss_gre_redir_lag_us_stats_unregister_notifier()
 *	Deregisters statistics notifier.
 */
int nss_gre_redir_lag_us_stats_unregister_notifier(struct notifier_block *nb)
{
	return atomic_notifier_chain_unregister(&nss_gre_redir_lag_us_stats_notifier, nb);
}
EXPORT_SYMBOL(nss_gre_redir_lag_us_stats_unregister_notifier);

/*
 * nss_gre_redir_lag_us_stats_register_notifier()
 *	Registers statistics notifier.
 */
int nss_gre_redir_lag_us_stats_register_notifier(struct notifier_block *nb)
{
	return atomic_notifier_chain_register(&nss_gre_redir_lag_us_stats_notifier, nb);
}
EXPORT_SYMBOL(nss_gre_redir_lag_us_stats_register_notifier);


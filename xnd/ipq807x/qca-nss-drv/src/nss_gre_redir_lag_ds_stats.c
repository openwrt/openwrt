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
#include "nss_gre_redir_lag_ds_stats.h"
#include "nss_gre_redir_lag_ds_strings.h"

/*
 * Declare atomic notifier data structure for statistics.
 */
ATOMIC_NOTIFIER_HEAD(nss_gre_redir_lag_ds_stats_notifier);

/*
 * Spinlock to protect GRE redirect lag ds statistics update/read
 */
DEFINE_SPINLOCK(nss_gre_redir_lag_ds_stats_lock);

extern struct nss_gre_redir_lag_ds_tun_stats tun_ds_stats[NSS_GRE_REDIR_LAG_MAX_NODE];

/*
 * nss_gre_redir_lag_ds_stats_get()
 *	Get statistics for downstream LAG node.
 */
bool nss_gre_redir_lag_ds_stats_get(struct nss_gre_redir_lag_ds_tun_stats *cmn_stats, uint32_t index)
{
	if (index >= NSS_GRE_REDIR_LAG_MAX_NODE)
		return false;

	spin_lock_bh(&nss_gre_redir_lag_ds_stats_lock);
	if (!tun_ds_stats[index].valid) {
		spin_unlock_bh(&nss_gre_redir_lag_ds_stats_lock);
		return false;
	}

	memcpy((void *)cmn_stats, (void *)&tun_ds_stats[index], sizeof(*cmn_stats));
	spin_unlock_bh(&nss_gre_redir_lag_ds_stats_lock);
	return true;
}

/*
 * nss_gre_redir_lag_ds_stats_read()
 *	Read gre_redir_lag_ds tunnel stats.
 */
static ssize_t nss_gre_redir_lag_ds_cmn_stats_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	/*
	 * Max output lines = #stats +
	 * few blank lines for banner printing + Number of Extra outputlines
	 * for future reference to add new stats
	 */
	uint32_t max_output_lines = NSS_GRE_REDIR_LAG_DS_STATS_MAX + NSS_STATS_EXTRA_OUTPUT_LINES;
	size_t size_al = NSS_STATS_MAX_STR_LENGTH * max_output_lines;
	struct nss_stats_data *data = fp->private_data;
	struct nss_gre_redir_lag_ds_tun_stats stats;
	ssize_t bytes_read = 0;
	size_t size_wr = 0;

	char *lbuf = kzalloc(size_al, GFP_KERNEL);
	if (unlikely(!lbuf)) {
		nss_warning("Could not allocate memory for local statistics buffer");
		return 0;
	}

	while (data->index < NSS_GRE_REDIR_LAG_MAX_NODE) {
		if (nss_gre_redir_lag_ds_stats_get(&stats, data->index)) {
			break;
		}

		data->index++;
	}

	if (data->index >= NSS_GRE_REDIR_LAG_MAX_NODE) {
		kfree(lbuf);
		return 0;
	}

	size_wr += nss_stats_banner(lbuf, size_wr, size_al, "gre_redir_lag_ds stats", NSS_STATS_SINGLE_CORE);
	size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, "\nTunnel stats for %03u\n", stats.ifnum);
	size_wr += nss_stats_print("gre_redir_lag_ds", NULL, NSS_STATS_SINGLE_INSTANCE, nss_gre_redir_lag_ds_strings_stats,
					&stats.rx_packets, NSS_GRE_REDIR_LAG_DS_STATS_MAX, lbuf, size_wr, size_al);

	bytes_read = simple_read_from_buffer(ubuf, sz, ppos, lbuf, strlen(lbuf));
	data->index++;
	kfree(lbuf);
	return bytes_read;
}

/*
 * nss_gre_redir_lag_ds_stats_ops
 */
NSS_STATS_DECLARE_FILE_OPERATIONS(gre_redir_lag_ds_cmn)

/*
 * nss_gre_redir_lag_ds_stats_dentry_create()
 *	Create debugfs directory entry.
 */
struct dentry *nss_gre_redir_lag_ds_stats_dentry_create(void)
{
	struct dentry *gre_redir;
	struct dentry *cmn_stats;

	gre_redir = nss_gre_redir_get_dentry();
	if (unlikely(!gre_redir)) {
		nss_warning("Failed to retrieve directory entry qca-nss-drv/stats/gre_redir/\n");
		return NULL;
	}

	cmn_stats = debugfs_create_file("lag_ds_cmn_stats", 0400, gre_redir,
			&nss_top_main, &nss_gre_redir_lag_ds_cmn_stats_ops);
	if (unlikely(!cmn_stats)) {
		nss_warning("Failed to create qca-nss-drv/stats/gre_redir/lag_ds_cmn_stats file\n");
		return NULL;
	}

	return cmn_stats;
}

/*
 * nss_gre_redir_lag_ds_stats_sync()
 *	Update synchonized statistics.
 */
void nss_gre_redir_lag_ds_stats_sync(struct nss_ctx_instance *nss_ctx, struct nss_gre_redir_lag_ds_sync_stats_msg *ngss, uint32_t ifnum)
{
	int idx, j;

	spin_lock_bh(&nss_gre_redir_lag_ds_stats_lock);
	if (!nss_gre_redir_lag_ds_get_node_idx(ifnum, &idx)) {
		spin_unlock_bh(&nss_gre_redir_lag_ds_stats_lock);
		nss_warning("%px: Unable to update hash stats msg. Stats context not found.\n", nss_ctx);
		return;
	}

	tun_ds_stats[idx].tx_packets += ngss->node_stats.tx_packets;
	tun_ds_stats[idx].tx_bytes += ngss->node_stats.tx_bytes;
	tun_ds_stats[idx].rx_packets += ngss->node_stats.rx_packets;
	tun_ds_stats[idx].rx_bytes += ngss->node_stats.rx_bytes;
	for (j = 0; j < NSS_MAX_NUM_PRI; j++) {
		tun_ds_stats[idx].rx_dropped[j] += ngss->node_stats.rx_dropped[j];
	}

	tun_ds_stats[idx].dst_invalid += ngss->ds_stats.dst_invalid;
	tun_ds_stats[idx].exception_cnt += ngss->ds_stats.exception_cnt;
	spin_unlock_bh(&nss_gre_redir_lag_ds_stats_lock);
}

/*
 * nss_gre_redir_lag_ds_stats_notify()
 *	Sends notifications to all the registered modules.
 *
 * Leverage NSS-FW statistics timing to update Netlink.
 */
void nss_gre_redir_lag_ds_stats_notify(struct nss_ctx_instance *nss_ctx, uint32_t if_num)
{
	struct nss_gre_redir_lag_ds_stats_notification *stats_notify;
	int idx;

	stats_notify = kzalloc(sizeof(struct nss_gre_redir_lag_ds_stats_notification), GFP_ATOMIC);
	if (!stats_notify) {
		nss_warning("Unable to allocate memory for stats notification\n");
		return;
	}

	spin_lock_bh(&nss_gre_redir_lag_ds_stats_lock);
	if (!nss_gre_redir_lag_ds_get_node_idx(if_num, &idx)) {
		spin_unlock_bh(&nss_gre_redir_lag_ds_stats_lock);
		nss_warning("%px: Unable to update hash stats msg. Stats context not found.\n", nss_ctx);
		kfree(stats_notify);
		return;
	}

	stats_notify->core_id = nss_ctx->id;
	stats_notify->if_num = if_num;
	memcpy(&(stats_notify->stats_ctx), &(tun_ds_stats[idx]), sizeof(stats_notify->stats_ctx));
	spin_unlock_bh(&nss_gre_redir_lag_ds_stats_lock);
	atomic_notifier_call_chain(&nss_gre_redir_lag_ds_stats_notifier, NSS_STATS_EVENT_NOTIFY, stats_notify);
	kfree(stats_notify);
}

/*
 * nss_gre_redir_lag_ds_stats_unregister_notifier()
 *	Deregisters statistics notifier.
 */
int nss_gre_redir_lag_ds_stats_unregister_notifier(struct notifier_block *nb)
{
	return atomic_notifier_chain_unregister(&nss_gre_redir_lag_ds_stats_notifier, nb);
}
EXPORT_SYMBOL(nss_gre_redir_lag_ds_stats_unregister_notifier);

/*
 * nss_gre_redir_lag_ds_stats_register_notifier()
 *	Registers statistics notifier.
 */
int nss_gre_redir_lag_ds_stats_register_notifier(struct notifier_block *nb)
{
	return atomic_notifier_chain_register(&nss_gre_redir_lag_ds_stats_notifier, nb);
}
EXPORT_SYMBOL(nss_gre_redir_lag_ds_stats_register_notifier);

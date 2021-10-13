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
#include "nss_stats.h"
#include "nss_gre_redir_mark.h"
#include "nss_gre_redir_mark_stats.h"
#include "nss_gre_redir_mark_strings.h"

#define NSS_GRE_REDIR_MARK_STATS_STR_LEN 50
#define NSS_GRE_REDIR_MARK_STATS_LEN ((NSS_GRE_REDIR_MARK_STATS_MAX + 7 ) * NSS_GRE_REDIR_MARK_STATS_STR_LEN)

/*
 * Declare atomic notifier data structure for statistics.
 */
ATOMIC_NOTIFIER_HEAD(nss_gre_redir_mark_stats_notifier);

/*
 * Spinlock to protect GRE redirect mark statistics update/read
 */
DEFINE_SPINLOCK(nss_gre_redir_mark_stats_lock);

/*
 * Global GRE redirect mark stats structure.
 */
struct nss_gre_redir_mark_stats gre_mark_stats;

/*
 * nss_gre_redir_mark_stats_get()
 *	Get gre_redir tunnel stats.
 */
bool nss_gre_redir_mark_stats_get(struct nss_gre_redir_mark_stats *stats_mem)
{
	if (!stats_mem) {
		nss_warning("No memory to copy GRE redir mark stats");
		return false;
	}

	/*
	 * Copy the GRE redir mark stats in the memory.
	 */
	spin_lock_bh(&nss_gre_redir_mark_stats_lock);
	memcpy(stats_mem, &gre_mark_stats, sizeof(struct nss_gre_redir_mark_stats));
	spin_unlock_bh(&nss_gre_redir_mark_stats_lock);
	return true;
}
EXPORT_SYMBOL(nss_gre_redir_mark_stats_get);

/**
 * nss_gre_redir_mark_stats_read()
 *	READ GRE redir mark stats.
 */
static ssize_t nss_gre_redir_mark_stats_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	/*
	 * Max output lines = #stats +
	 * few blank lines for banner printing + Number of Extra outputlines
	 * for future reference to add new stats
	 */
	uint32_t max_output_lines = NSS_GRE_REDIR_MARK_STATS_MAX + NSS_STATS_EXTRA_OUTPUT_LINES;
	size_t size_al = NSS_STATS_MAX_STR_LENGTH * max_output_lines;
	struct nss_gre_redir_mark_stats stats;
	size_t size_wr = 0;
	ssize_t bytes_read = 0;
	bool isthere;

	char *lbuf = kzalloc(size_al, GFP_KERNEL);
	if (unlikely(!lbuf)) {
		nss_warning("Could not allocate memory for local statistics buffer");
		return 0;
	}

	/*
	 * If GRE redir mark does not exists, then (isthere) will be false.
	 */
	isthere = nss_gre_redir_mark_stats_get(&stats);
	if (!isthere) {
		nss_warning("Could not get GRE redirect stats");
		kfree(lbuf);
		return 0;
	}

	size_wr += nss_stats_banner(lbuf, size_wr, size_al, "gre_redir_mark stats", NSS_STATS_SINGLE_CORE);
	size_wr += nss_stats_print("gre_redir_mark", NULL, NSS_STATS_SINGLE_INSTANCE, nss_gre_redir_mark_strings_stats,
					stats.stats, NSS_GRE_REDIR_MARK_STATS_MAX, lbuf, size_wr, size_al);

	bytes_read = simple_read_from_buffer(ubuf, sz, ppos, lbuf, size_wr);

	kfree(lbuf);
	return bytes_read;
}

/*
 * nss_gre_redir_mark_stats_ops
 */
NSS_STATS_DECLARE_FILE_OPERATIONS(gre_redir_mark)

/*
 * nss_gre_redir_mark_stats_dentry_create()
 *	Create debugfs directory entry for stats.
 */
struct dentry *nss_gre_redir_mark_stats_dentry_create(void)
{
	struct dentry *gre_redir_mark;

	gre_redir_mark = debugfs_create_file("gre_redir_mark", 0400, nss_top_main.stats_dentry,
			&nss_top_main, &nss_gre_redir_mark_stats_ops);
	if (unlikely(!gre_redir_mark)) {
		nss_warning("Failed to create file entry qca-nss-drv/stats/gre_redir_mark/\n");
		return NULL;
	}

	return gre_redir_mark;
}

/*
 * nss_gre_redir_mark_stats_sync()
 *	Update GRE redir mark stats.
 */
void nss_gre_redir_mark_stats_sync(struct nss_ctx_instance *nss_ctx, int if_num, struct nss_gre_redir_mark_stats_sync_msg *ngss)
{
	int i;
	struct net_device *dev;
	dev = nss_cmn_get_interface_dev(nss_ctx, if_num);
	if (!dev) {
		nss_warning("%px: Unable to find net device for the interface %d\n", nss_ctx, if_num);
		return;
	}

	if (if_num != NSS_GRE_REDIR_MARK_INTERFACE) {
		nss_warning("%px: Unknown type for interface %d\n", nss_ctx, if_num);
		return;
	}

	/*
	 * Update the stats in exclusive mode to prevent the read from the process
	 * context through debug fs.
	 */
	spin_lock_bh(&nss_gre_redir_mark_stats_lock);

	/*
	 * Update the common node stats
	 */
	gre_mark_stats.stats[NSS_STATS_NODE_TX_PKTS] += ngss->node_stats.tx_packets;
	gre_mark_stats.stats[NSS_STATS_NODE_TX_BYTES] += ngss->node_stats.tx_bytes;
	gre_mark_stats.stats[NSS_STATS_NODE_RX_PKTS] += ngss->node_stats.rx_packets;
	gre_mark_stats.stats[NSS_STATS_NODE_RX_BYTES] += ngss->node_stats.rx_bytes;
	for (i = 0; i < NSS_MAX_NUM_PRI; i++) {
		gre_mark_stats.stats[NSS_STATS_NODE_RX_QUEUE_0_DROPPED + i] += ngss->node_stats.rx_dropped[i];
	}

	/*
	 * Update the GRE redir mark specific stats
	 */
	gre_mark_stats.stats[NSS_GRE_REDIR_MARK_STATS_HLOS_MAGIC_FAILED] += ngss->hlos_magic_fail;
	gre_mark_stats.stats[NSS_GRE_REDIR_MARK_STATS_INV_DST_IF_DROPS] += ngss->invalid_dst_drop;
	gre_mark_stats.stats[NSS_GRE_REDIR_MARK_STATS_DST_IF_ENQUEUE] += ngss->dst_enqueue_success;
	gre_mark_stats.stats[NSS_GRE_REDIR_MARK_STATS_DST_IF_ENQUEUE_DROPS] += ngss->dst_enqueue_drop;
	gre_mark_stats.stats[NSS_GRE_REDIR_MARK_STATS_INV_APPID] += ngss->inv_appid;
	gre_mark_stats.stats[NSS_GRE_REDIR_MARK_STATS_HEADROOM_UNAVAILABLE] += ngss->headroom_unavail;
	gre_mark_stats.stats[NSS_GRE_REDIR_MARK_STATS_TX_COMPLETION_SUCCESS] += ngss->tx_completion_success;
	gre_mark_stats.stats[NSS_GRE_REDIR_MARK_STATS_TX_COMPLETION_DROPS] += ngss->tx_completion_drop;

	spin_unlock_bh(&nss_gre_redir_mark_stats_lock);
}

/*
 * nss_gre_redir_mark_stats_notify()
 *	Sends notifications to all the registered modules.
 *
 * Leverage NSS-FW statistics timing to update Netlink.
 */
void nss_gre_redir_mark_stats_notify(struct nss_ctx_instance *nss_ctx, uint32_t if_num)
{
	struct nss_gre_redir_mark_stats_notification *stats_notify;

	stats_notify = kzalloc(sizeof(struct nss_gre_redir_mark_stats_notification), GFP_ATOMIC);
	if (!stats_notify) {
		nss_warning("Unable to allocate memory for stats notification\n");
		return;
	}

	if (if_num != NSS_GRE_REDIR_MARK_INTERFACE) {
		nss_warning("%px: Unknown type for interface %d\n", nss_ctx, if_num);
		kfree(stats_notify);
		return;
	}

	spin_lock_bh(&nss_gre_redir_mark_stats_lock);
	stats_notify->core_id = nss_ctx->id;
	stats_notify->if_num = if_num;
	memcpy(stats_notify->stats_ctx, gre_mark_stats.stats, sizeof(stats_notify->stats_ctx));
	spin_unlock_bh(&nss_gre_redir_mark_stats_lock);

	atomic_notifier_call_chain(&nss_gre_redir_mark_stats_notifier, NSS_STATS_EVENT_NOTIFY, stats_notify);
	kfree(stats_notify);
}

/*
 * nss_gre_redir_mark_stats_unregister_notifier()
 *	Deregisters statistics notifier.
 */
int nss_gre_redir_mark_stats_unregister_notifier(struct notifier_block *nb)
{
	return atomic_notifier_chain_unregister(&nss_gre_redir_mark_stats_notifier, nb);
}
EXPORT_SYMBOL(nss_gre_redir_mark_stats_unregister_notifier);

/*
 * nss_gre_redir_mark_stats_register_notifier()
 *	Registers statistics notifier.
 */
int nss_gre_redir_mark_stats_register_notifier(struct notifier_block *nb)
{
	return atomic_notifier_chain_register(&nss_gre_redir_mark_stats_notifier, nb);
}
EXPORT_SYMBOL(nss_gre_redir_mark_stats_register_notifier);

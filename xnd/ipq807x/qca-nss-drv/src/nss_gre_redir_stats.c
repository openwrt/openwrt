/*
 ****************************************************************************
 * Copyright (c) 2017-2019, 2021, The Linux Foundation. All rights reserved.
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
 ****************************************************************************
 */

#include "nss_core.h"
#include "nss_gre_redir.h"
#include "nss_gre_redir_stats.h"
#include "nss_gre_redir_strings.h"

/*
 * Declare atomic notifier data structure for statistics.
 */
ATOMIC_NOTIFIER_HEAD(nss_gre_redir_stats_notifier);

/*
 * Spinlock to protect GRE redirect statistics update/read
 */
DEFINE_SPINLOCK(nss_gre_redir_stats_lock);

/*
 * Array to hold tunnel stats along with if_num
 */
extern struct nss_gre_redir_tunnel_stats tun_stats[NSS_GRE_REDIR_MAX_INTERFACES];

/*
 * nss_gre_redir_stats_get()
 *	Get GRE redirect tunnel stats.
 */
bool nss_gre_redir_stats_get(int index, struct nss_gre_redir_tunnel_stats *stats)
{
	spin_lock_bh(&nss_gre_redir_stats_lock);
	if (tun_stats[index].ref_count == 0) {
		spin_unlock_bh(&nss_gre_redir_stats_lock);
		return false;
	}

	memcpy(stats, &tun_stats[index], sizeof(struct nss_gre_redir_tunnel_stats));
	spin_unlock_bh(&nss_gre_redir_stats_lock);
	return true;
}
EXPORT_SYMBOL(nss_gre_redir_stats_get);

/*
 * nss_gre_redir_stats_read()
 *	READ gre_redir tunnel stats.
 */
static ssize_t nss_gre_redir_stats_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	 /*
	  * Max output lines = #stats +
	  * few blank lines for banner printing + Number of Extra outputlines
	  * for future reference to add new stats
	  */
	uint32_t max_output_lines = NSS_GRE_REDIR_STATS_MAX + NSS_STATS_EXTRA_OUTPUT_LINES;
	size_t size_al = NSS_STATS_MAX_STR_LENGTH * max_output_lines * NSS_GRE_REDIR_MAX_INTERFACES;
	struct nss_stats_data *data = fp->private_data;
	struct nss_gre_redir_tunnel_stats stats;
	ssize_t bytes_read = 0;
	size_t size_wr = 0;
	int index = 0;

	char *lbuf = kzalloc(size_al, GFP_KERNEL);
	if (unlikely(!lbuf)) {
		nss_warning("Could not allocate memory for local statistics buffer");
		return 0;
	}

	if (data) {
		index = data->index;
	}

	/*
	 * If we are done accomodating all the GRE_REDIR tunnels.
	 */
	if (index >= NSS_GRE_REDIR_MAX_INTERFACES) {
		kfree(lbuf);
		return 0;
	}

	for (; index < NSS_GRE_REDIR_MAX_INTERFACES; index++) {
		bool isthere;

		/*
		 * If gre_redir tunnel does not exists, then isthere will be false.
		 */
		isthere = nss_gre_redir_stats_get(index, &stats);
		if (!isthere) {
			continue;
		}

		size_wr += nss_stats_banner(lbuf, size_wr, size_al, "gre_redir stats", NSS_STATS_SINGLE_CORE);
		size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, "\nTunnel stats for %s\n", stats.dev->name);
		size_wr += nss_stats_print("gre_redir", NULL, NSS_STATS_SINGLE_INSTANCE, nss_gre_redir_strings_stats,
					&stats.tstats.rx_packets, NSS_GRE_REDIR_STATS_MAX, lbuf, size_wr, size_al);
	}

	bytes_read = simple_read_from_buffer(ubuf, sz, ppos, lbuf, strlen(lbuf));
	if (data) {
		data->index = index;
	}

	kfree(lbuf);
	return bytes_read;
}

/*
 * nss_gre_redir_stats_ops
 */
NSS_STATS_DECLARE_FILE_OPERATIONS(gre_redir)

/*
 * nss_gre_redir_stats_dentry_create()
 *	Create debugfs directory entry for stats.
 */
struct dentry *nss_gre_redir_stats_dentry_create(void)
{
	struct dentry *gre_redir;
	struct dentry *tun_stats;

	gre_redir = debugfs_create_dir("gre_redir", nss_top_main.stats_dentry);
	if (unlikely(!gre_redir)) {
		nss_warning("Failed to create directory entry qca-nss-drv/stats/gre_redir/\n");
		return NULL;
	}

	tun_stats = debugfs_create_file("tun_stats", 0400, gre_redir,
			&nss_top_main, &nss_gre_redir_stats_ops);
	if (unlikely(!tun_stats)) {
		debugfs_remove_recursive(gre_redir);
		nss_warning("Failed to create file entry qca-nss-drv/stats/gre_redir/tun_stats\n");
		return NULL;
	}

	return gre_redir;
}

/*
 * nss_gre_redir_stats_sync()
 *	Update gre_redir tunnel stats.
 */
void nss_gre_redir_stats_sync(struct nss_ctx_instance *nss_ctx, int if_num, struct nss_gre_redir_stats_sync_msg *ngss)
{
	int i, j;
	uint32_t type;
	struct net_device *dev;
	struct nss_gre_redir_tun_stats *node_stats;

	type = nss_dynamic_interface_get_type(nss_ctx, if_num);
	dev = nss_cmn_get_interface_dev(nss_ctx, if_num);
	if (!dev) {
		nss_warning("%px: Unable to find net device for the interface %d\n", nss_ctx, if_num);
		return;
	}

	if (!nss_gre_redir_verify_ifnum(if_num)) {
		nss_warning("%px: Unknown type for interface %d\n", nss_ctx, if_num);
		return;
	}

	spin_lock_bh(&nss_gre_redir_stats_lock);
	for (i = 0; i < NSS_GRE_REDIR_MAX_INTERFACES; i++) {
		if (tun_stats[i].dev == dev) {
			break;
		}
	}

	if (i == NSS_GRE_REDIR_MAX_INTERFACES) {
		nss_warning("%px: Unable to find tunnel stats instance for interface %d\n", nss_ctx, if_num);
		spin_unlock_bh(&nss_gre_redir_stats_lock);
		return;
	}

	nss_assert(tun_stats[i].ref_count);
	node_stats = &tun_stats[i].tstats;
	switch (type) {
	case NSS_DYNAMIC_INTERFACE_TYPE_GRE_REDIR_WIFI_HOST_INNER:
	case NSS_DYNAMIC_INTERFACE_TYPE_GRE_REDIR_WIFI_OFFL_INNER:
	case NSS_DYNAMIC_INTERFACE_TYPE_GRE_REDIR_SJACK_INNER:
		node_stats->tx_packets += ngss->node_stats.tx_packets;
		node_stats->tx_bytes += ngss->node_stats.tx_bytes;
		node_stats->sjack_tx_packets += ngss->sjack_rx_packets;
		node_stats->encap_sg_alloc_drop += ngss->encap_sg_alloc_drop;
		node_stats->tx_dropped += nss_cmn_rx_dropped_sum(&(ngss->node_stats));
		for (j = 0; j < NSS_GRE_REDIR_MAX_RADIO; j++) {
			node_stats->offl_tx_pkts[j] += ngss->offl_rx_pkts[j];
		}

		break;

	case NSS_DYNAMIC_INTERFACE_TYPE_GRE_REDIR_OUTER:
		node_stats->rx_packets += ngss->node_stats.rx_packets;
		node_stats->rx_bytes += ngss->node_stats.rx_bytes;
		node_stats->sjack_rx_packets += ngss->sjack_rx_packets;
		node_stats->decap_fail_drop += ngss->decap_fail_drop;
		node_stats->decap_split_drop += ngss->decap_split_drop;
		node_stats->split_sg_alloc_fail += ngss->split_sg_alloc_fail;
		node_stats->split_linear_copy_fail += ngss->split_linear_copy_fail;
		node_stats->split_not_enough_tailroom += ngss->split_not_enough_tailroom;
		node_stats->decap_eapol_frames += ngss->decap_eapol_frames;
		for (j = 0; j < NSS_MAX_NUM_PRI; j++) {
			node_stats->rx_dropped[j] += ngss->node_stats.rx_dropped[j];
		}

		for (j = 0; j < NSS_GRE_REDIR_MAX_RADIO; j++) {
			node_stats->offl_rx_pkts[j] += ngss->offl_rx_pkts[j];
		}

		break;

	case NSS_DYNAMIC_INTERFACE_TYPE_GRE_REDIR_EXCEPTION_US:
		node_stats->exception_us_rx += ngss->node_stats.rx_packets;
		node_stats->exception_us_tx += ngss->node_stats.tx_packets;
		break;

	case NSS_DYNAMIC_INTERFACE_TYPE_GRE_REDIR_EXCEPTION_DS:
		node_stats->exception_ds_rx += ngss->node_stats.rx_packets;
		node_stats->exception_ds_tx += ngss->node_stats.tx_packets;
		node_stats->exception_ds_invalid_dst_drop += ngss->exception_ds_invalid_dst_drop;
		node_stats->exception_ds_inv_appid += ngss->exception_ds_inv_appid;
		node_stats->headroom_unavail += ngss->headroom_unavail;
		node_stats->tx_completion_success += ngss->tx_completion_success;
		node_stats->tx_completion_drop += ngss->tx_completion_drop;
		break;
	}

	spin_unlock_bh(&nss_gre_redir_stats_lock);
}

/*
 * nss_gre_redir_stats_notify()
 *	Sends notifications to all the registered modules.
 *
 * Leverage NSS-FW statistics timing to update Netlink.
 */
void nss_gre_redir_stats_notify(struct nss_ctx_instance *nss_ctx, uint32_t if_num)
{
	struct nss_gre_redir_stats_notification *stats_notify;
	struct net_device *dev;
	int i;

	stats_notify = kzalloc(sizeof(struct nss_gre_redir_stats_notification), GFP_ATOMIC);
	if (!stats_notify) {
		nss_warning("Unable to allocate memory for stats notification\n");
		return;
	}

	dev = nss_cmn_get_interface_dev(nss_ctx, if_num);
	if (!dev) {
		nss_warning("%px: Unable to find net device for the interface %d\n", nss_ctx, if_num);
		kfree(stats_notify);
		return;
	}

	if (!nss_gre_redir_verify_ifnum(if_num)) {
		nss_warning("%px: Unknown type for interface %d\n", nss_ctx, if_num);
		kfree(stats_notify);
		return;
	}

	spin_lock_bh(&nss_gre_redir_stats_lock);
	for (i = 0; i < NSS_GRE_REDIR_MAX_INTERFACES; i++) {
		if (tun_stats[i].dev == dev) {
			break;
		}
	}

	if (i == NSS_GRE_REDIR_MAX_INTERFACES) {
		nss_warning("%px: Unable to find tunnel stats instance for interface %d\n", nss_ctx, if_num);
		spin_unlock_bh(&nss_gre_redir_stats_lock);
		kfree(stats_notify);
		return;
	}

	stats_notify->core_id = nss_ctx->id;
	stats_notify->if_num = if_num;
	memcpy(&(stats_notify->stats_ctx), &(tun_stats[i]), sizeof(stats_notify->stats_ctx));
	spin_unlock_bh(&nss_gre_redir_stats_lock);
	atomic_notifier_call_chain(&nss_gre_redir_stats_notifier, NSS_STATS_EVENT_NOTIFY, stats_notify);
	kfree(stats_notify);
}

/*
 * nss_gre_redir_stats_unregister_notifier()
 *	Degisters statistics notifier.
 */
int nss_gre_redir_stats_unregister_notifier(struct notifier_block *nb)
{
	return atomic_notifier_chain_unregister(&nss_gre_redir_stats_notifier, nb);
}
EXPORT_SYMBOL(nss_gre_redir_stats_unregister_notifier);

/*
 * nss_gre_redir_stats_register_notifier()
 *	Registers statistics notifier.
 */
int nss_gre_redir_stats_register_notifier(struct notifier_block *nb)
{
	return atomic_notifier_chain_register(&nss_gre_redir_stats_notifier, nb);
}
EXPORT_SYMBOL(nss_gre_redir_stats_register_notifier);

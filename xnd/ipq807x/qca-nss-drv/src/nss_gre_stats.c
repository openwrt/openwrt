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
 * nss_gre_stats.c
 *	NSS GRE statistics APIs
 *
 */

#include "nss_tx_rx_common.h"
#include "nss_gre.h"
#include "nss_gre_stats.h"
#include "nss_gre_strings.h"

/*
 * Declare atomic notifier data structure for statistics.
 */
ATOMIC_NOTIFIER_HEAD(nss_gre_stats_notifier);

/*
 * Data structures to store GRE nss debug stats
 */
static DEFINE_SPINLOCK(nss_gre_stats_lock);
static struct nss_gre_stats_session session_stats[NSS_GRE_MAX_DEBUG_SESSION_STATS];
static struct nss_gre_stats_base base_stats;

/*
 * GRE statistics APIs
 */

/*
 * nss_gre_stats_session_unregister()
 *	Unregister debug statistic for GRE session.
 */
void nss_gre_stats_session_unregister(uint32_t if_num)
{
	int i;

	spin_lock_bh(&nss_gre_stats_lock);
	for (i = 0; i < NSS_GRE_MAX_DEBUG_SESSION_STATS; i++) {
		if (session_stats[i].if_num == if_num) {
			memset(&session_stats[i], 0, sizeof(struct nss_gre_stats_session));
			break;
		}
	}
	spin_unlock_bh(&nss_gre_stats_lock);
}

/*
 * nss_gre_stats_session_register()
 *	Register debug statistic for GRE session.
 */
void nss_gre_stats_session_register(uint32_t if_num, struct net_device *netdev)
{
	int i;

	spin_lock_bh(&nss_gre_stats_lock);
	for (i = 0; i < NSS_GRE_MAX_DEBUG_SESSION_STATS; i++) {
		if (!session_stats[i].valid) {
			session_stats[i].valid = true;
			session_stats[i].if_num = if_num;
			session_stats[i].if_index = netdev->ifindex;
			break;
		}
	}
	spin_unlock_bh(&nss_gre_stats_lock);
}

/*
 * nss_gre_stats_session_sync()
 *	debug statistics sync for GRE session.
 */
void nss_gre_stats_session_sync(struct nss_ctx_instance *nss_ctx, struct nss_gre_session_stats_msg *sstats, uint16_t if_num)
{
	int i, j;
	enum nss_dynamic_interface_type interface_type = nss_dynamic_interface_get_type(nss_ctx, if_num);

	spin_lock_bh(&nss_gre_stats_lock);
	for (i = 0; i < NSS_GRE_MAX_DEBUG_SESSION_STATS; i++) {
		if (session_stats[i].if_num == if_num) {
			for (j = 0; j < NSS_GRE_SESSION_DEBUG_MAX; j++) {
				session_stats[i].stats[j] += sstats->stats[j];
			}

			if (interface_type == NSS_DYNAMIC_INTERFACE_TYPE_GRE_INNER) {
				session_stats[i].stats[NSS_GRE_SESSION_ENCAP_RX_RECEIVED] += sstats->node_stats.rx_packets;
			} else if (interface_type == NSS_DYNAMIC_INTERFACE_TYPE_GRE_OUTER) {
				session_stats[i].stats[NSS_GRE_SESSION_DECAP_TX_FORWARDED] += sstats->node_stats.tx_packets;
			}
			break;
		}
	}
	spin_unlock_bh(&nss_gre_stats_lock);
}

/*
 * nss_gre_stats_base_sync()
 *	Debug statistics sync for GRE base node.
 */
void nss_gre_stats_base_sync(struct nss_ctx_instance *nss_ctx, struct nss_gre_base_stats_msg *bstats)
{
	int i;

	spin_lock_bh(&nss_gre_stats_lock);
	for (i = 0; i < NSS_GRE_BASE_DEBUG_MAX; i++) {
		base_stats.stats[i] += bstats->stats[i];
	}
	spin_unlock_bh(&nss_gre_stats_lock);
}

/*
 * nss_gre_stats_session_get()
 *	Get GRE session debug statistics.
 */
static void nss_gre_stats_session_get(void *stats_mem, int size)
{
	struct nss_gre_stats_session *stats = (struct nss_gre_stats_session *)stats_mem;
	int i;

	if (!stats || (size < (sizeof(struct nss_gre_stats_session) * NSS_GRE_MAX_DEBUG_SESSION_STATS)))  {
		nss_warning("No memory to copy gre stats");
		return;
	}

	spin_lock_bh(&nss_gre_stats_lock);
	for (i = 0; i < NSS_GRE_MAX_DEBUG_SESSION_STATS; i++) {
		if (session_stats[i].valid) {
			memcpy(stats, &session_stats[i], sizeof(struct nss_gre_stats_session));
			stats++;
		}
	}
	spin_unlock_bh(&nss_gre_stats_lock);
}

/*
 * nss_gre_stats_base_get()
 *	Get GRE debug base statistics.
 */
static void nss_gre_stats_base_get(void *stats_mem, int size)
{
	struct nss_gre_stats_base *stats = (struct nss_gre_stats_base *)stats_mem;

	if (!stats) {
		nss_warning("No memory to copy GRE base stats\n");
		return;
	}

	if (size < sizeof(struct nss_gre_stats_base)) {
		nss_warning("Not enough memory to copy GRE base stats\n");
		return;
	}

	spin_lock_bh(&nss_gre_stats_lock);
	memcpy(stats, &base_stats, sizeof(struct nss_gre_stats_base));
	spin_unlock_bh(&nss_gre_stats_lock);
}

/*
 * nss_gre_stats_read()
 *	Read GRE statistics
 */
static ssize_t nss_gre_stats_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	uint32_t max_output_lines = 2 /* header & footer for base debug stats */
		+ 2 /* header & footer for session debug stats */
		+ NSS_GRE_BASE_DEBUG_MAX  /* Base debug */
		+ NSS_GRE_MAX_DEBUG_SESSION_STATS * (NSS_GRE_SESSION_DEBUG_MAX + 2) /*session stats */
		+ 2;
	size_t size_al = NSS_STATS_MAX_STR_LENGTH * max_output_lines;
	size_t size_wr = 0;
	ssize_t bytes_read = 0;
	struct net_device *dev;
	struct nss_gre_stats_session *sstats;
	struct nss_gre_stats_base *bstats;
	int id;

	char *lbuf = kzalloc(size_al, GFP_KERNEL);
	if (unlikely(!lbuf)) {
		nss_warning("Could not allocate memory for local statistics buffer");
		return 0;
	}

	bstats = kzalloc(sizeof(struct nss_gre_stats_base), GFP_KERNEL);
	if (unlikely(!bstats)) {
		nss_warning("Could not allocate memory for base debug statistics buffer");
		kfree(lbuf);
		return 0;
	}

	sstats = kzalloc(sizeof(struct nss_gre_stats_session) * NSS_GRE_MAX_DEBUG_SESSION_STATS, GFP_KERNEL);
	if (unlikely(!sstats)) {
		nss_warning("Could not allocate memory for base debug statistics buffer");
		kfree(lbuf);
		kfree(bstats);
		return 0;
	}

	size_wr += nss_stats_banner(lbuf, size_wr, size_al, "gre", NSS_STATS_SINGLE_CORE);

	/*
	 * Get all base stats
	 */
	nss_gre_stats_base_get((void *)bstats, sizeof(struct nss_gre_stats_base));

	size_wr += nss_stats_print("gre", NULL, NSS_STATS_SINGLE_INSTANCE
					, nss_gre_strings_base_stats
					, bstats->stats
					, NSS_GRE_BASE_DEBUG_MAX
					, lbuf, size_wr, size_al);

	/*
	 * Get all session stats
	 */
	nss_gre_stats_session_get(sstats, sizeof(struct nss_gre_stats_session) * NSS_GRE_MAX_DEBUG_SESSION_STATS);

	for (id = 0; id < NSS_GRE_MAX_DEBUG_SESSION_STATS; id++) {

		if (!((sstats + id)->valid)) {
			continue;
		}

		dev = dev_get_by_index(&init_net, (sstats + id)->if_index);
		if (likely(dev)) {

			size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, "%d. nss interface id=%d, netdevice=%s\n", id,
					     (sstats + id)->if_num, dev->name);
			dev_put(dev);
		} else {
			size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, "%d. nss interface id=%d\n", id,
					     (sstats + id)->if_num);
		}
		size_wr += nss_stats_print("gre_session", NULL, id
						, nss_gre_strings_session_stats
						, (sstats + id)->stats
						, NSS_GRE_SESSION_DEBUG_MAX
						, lbuf, size_wr, size_al);
		size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, "\n");
	}

	bytes_read = simple_read_from_buffer(ubuf, sz, ppos, lbuf, size_wr);

	kfree(sstats);
	kfree(bstats);
	kfree(lbuf);
	return bytes_read;
}

/*
 * nss_gre_stats_ops
 */
NSS_STATS_DECLARE_FILE_OPERATIONS(gre)

/*
 * nss_gre_stats_dentry_create()
 *	Create gre statistics debug entry.
 */
void nss_gre_stats_dentry_create(void)
{
	nss_stats_create_dentry("gre", &nss_gre_stats_ops);
}

/*
 * nss_gre_stats_base_notify()
 *	Sends notifications to all the registered modules.
 *
 * Leverage NSS-FW statistics timing to update Netlink.
 */
void nss_gre_stats_base_notify(struct nss_ctx_instance *nss_ctx)
{
	struct nss_gre_base_stats_notification gre_stats;

	spin_lock_bh(&nss_gre_stats_lock);
	gre_stats.core_id = nss_ctx->id;
	memcpy(gre_stats.stats_base_ctx, base_stats.stats, sizeof(gre_stats.stats_base_ctx));
	spin_unlock_bh(&nss_gre_stats_lock);

	atomic_notifier_call_chain(&nss_gre_stats_notifier, NSS_STATS_EVENT_NOTIFY, &gre_stats);
}

/*
 * nss_gre_stats_session_notify()
 *	Sends notifications to all the registered modules.
 *
 * Leverage NSS-FW statistics timing to update Netlink.
 */
void nss_gre_stats_session_notify(struct nss_ctx_instance *nss_ctx, uint32_t if_num)
{
	struct nss_gre_session_stats_notification gre_stats;
	int i;

	spin_lock_bh(&nss_gre_stats_lock);
	for (i = 0; i < NSS_GRE_MAX_DEBUG_SESSION_STATS; i++) {
		if (session_stats[i].if_num != if_num) {
			continue;
		}

		memcpy(gre_stats.stats_session_ctx, session_stats[i].stats, sizeof(gre_stats.stats_session_ctx));
		gre_stats.core_id = nss_ctx->id;
		gre_stats.if_num = if_num;
		spin_unlock_bh(&nss_gre_stats_lock);
		atomic_notifier_call_chain(&nss_gre_stats_notifier, NSS_STATS_EVENT_NOTIFY, &gre_stats);
		return;
	}
	spin_unlock_bh(&nss_gre_stats_lock);
}

/*
 * nss_gre_stats_unregister_notifier()
 *	Deregisters statistics notifier.
 */
int nss_gre_stats_unregister_notifier(struct notifier_block *nb)
{
	return atomic_notifier_chain_unregister(&nss_gre_stats_notifier, nb);
}
EXPORT_SYMBOL(nss_gre_stats_unregister_notifier);

/*
 * nss_gre_stats_register_notifier()
 *	Registers statistics notifier.
 */
int nss_gre_stats_register_notifier(struct notifier_block *nb)
{
	return atomic_notifier_chain_register(&nss_gre_stats_notifier, nb);
}
EXPORT_SYMBOL(nss_gre_stats_register_notifier);

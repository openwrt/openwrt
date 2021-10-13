/*
 **************************************************************************
 * Copyright (c) 2020-2021, The Linux Foundation. All rights reserved.
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
#include "nss_mirror.h"
#include "nss_mirror_stats.h"
#include "nss_mirror_strings.h"

static struct nss_mirror_stats_debug_instance *stats_db[NSS_MAX_MIRROR_DYNAMIC_INTERFACES];
					/* Mirror stats data structure. */

/*
 * Atomic notifier data structure for statistics
 */
ATOMIC_NOTIFIER_HEAD(nss_mirror_stats_notifier);

static DEFINE_SPINLOCK(nss_mirror_stats_lock);

/*
 * nss_mirror_stats_get()
 *	Get mirror interface statistics.
 */
static void nss_mirror_stats_get(void *stats_mem, uint32_t stats_num)
{
	struct nss_mirror_stats_debug_instance *stats = (struct nss_mirror_stats_debug_instance *)stats_mem;
	int i;

	if (!stats) {
		nss_warning("No memory to copy mirror interface stats");
		return;
	}

	spin_lock_bh(&nss_mirror_stats_lock);
	for (i = 0; i < NSS_MAX_MIRROR_DYNAMIC_INTERFACES; i++) {

		/*
		 * Copy maximum for given number of instances only.
		 */
		if (likely(stats_db[i])) {
			if (likely(stats_num)) {
				memcpy(stats, stats_db[i], sizeof(struct nss_mirror_stats_debug_instance));
				stats++;
				stats_num--;
			} else {
				break;
			}
		}
	}
	spin_unlock_bh(&nss_mirror_stats_lock);
}

/*
 * nss_mirror_stats_read()
 *	Read mirror interface statistics.
 */
static ssize_t nss_mirror_stats_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{

	uint32_t max_output_lines = 2 /* header & footer for instance stats */
					+ NSS_MAX_MIRROR_DYNAMIC_INTERFACES *
					 ((NSS_STATS_NODE_MAX + 3 ) + (NSS_MIRROR_STATS_MAX + 3)) /*instance stats */
					 + 2;
	size_t size_al = NSS_STATS_MAX_STR_LENGTH * max_output_lines;
	size_t size_wr = 0;
	ssize_t bytes_read = 0;
	struct net_device *dev;
	struct nss_mirror_stats_debug_instance *mirror_shadow_stats;
	uint32_t id, mirror_active_instances = atomic_read(&nss_mirror_num_instances);
	char *lbuf;

	if (!mirror_active_instances) {
		return 0;
	}

	lbuf = vzalloc(size_al);
	if (unlikely(!lbuf)) {
		nss_warning("Could not allocate memory for local statistics buffer");
		return 0;
	}

	mirror_shadow_stats = vzalloc(sizeof(struct nss_mirror_stats_debug_instance) *
			 mirror_active_instances);
	if (unlikely(!mirror_shadow_stats)) {
		nss_warning("Could not allocate memory for base debug statistics buffer");
		vfree(lbuf);
		return 0;
	}

	/*
	 * Get all stats
	 */
	nss_mirror_stats_get((void *)mirror_shadow_stats, mirror_active_instances);
	size_wr += nss_stats_banner(lbuf, size_wr, size_al, "mirror stats", NSS_STATS_SINGLE_CORE);

	/*
	 * Session stats
	 */
	for (id = 0; id < mirror_active_instances; id++) {
			dev = dev_get_by_index(&init_net, mirror_shadow_stats[id].if_index);
			if (likely(dev)) {

				size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, "%d. nss interface id=%d, netdevice=%s\n", id,
						mirror_shadow_stats[id].if_num, dev->name);
				dev_put(dev);
			} else {
				size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, "%d. nss interface id=%d\n", id,
						mirror_shadow_stats[id].if_num);
			}

			size_wr += nss_stats_fill_common_stats(mirror_shadow_stats[id].if_num, id, lbuf, size_wr, size_al, "mirror");

			/*
			 * Mirror interface exception stats.
			 */
			size_wr += nss_stats_print("mirror", "mirror exception stats",
							 id,
							 nss_mirror_strings_stats,
							 mirror_shadow_stats[id].stats,
							 NSS_MIRROR_STATS_MAX,
							 lbuf, size_wr, size_al);
	}

	bytes_read = simple_read_from_buffer(ubuf, sz, ppos, lbuf, size_wr);

	vfree(mirror_shadow_stats);
	vfree(lbuf);
	return bytes_read;
}

/*
 * nss_mirror_stats_sync()
 *	API to sync statistics for mirror interface.
 */
void nss_mirror_stats_sync(struct nss_ctx_instance *nss_ctx,
		 struct nss_mirror_msg *nmm, uint16_t if_num)
{
	uint8_t i, j;
	struct nss_top_instance *nss_top = nss_ctx->nss_top;
	struct nss_mirror_stats_sync_msg *stats_msg = &nmm->msg.stats;
	struct nss_cmn_node_stats *node_stats_ptr = &stats_msg->node_stats;
	uint32_t *mirror_stats_ptr = (uint32_t *)&stats_msg->mirror_stats;

	spin_lock_bh(&nss_mirror_stats_lock);
	for (i = 0; i < NSS_MAX_MIRROR_DYNAMIC_INTERFACES; i++) {
		if (!stats_db[i] || (stats_db[i]->if_num != if_num)) {
			continue;
		}

		for (j = 0; j < NSS_MIRROR_STATS_MAX; j++) {
			/*
			 * Sync stats.
			 */
			stats_db[i]->stats[j] += mirror_stats_ptr[j];
		}
		spin_unlock_bh(&nss_mirror_stats_lock);
		goto sync_cmn_stats;
	}

	spin_unlock_bh(&nss_mirror_stats_lock);
	nss_warning("Invalid mirror stats sync message received for %d interface\n", if_num);
	return;

sync_cmn_stats:
	spin_lock_bh(&nss_mirror_stats_lock);

	/*
	 * Sync common stats.
	 */
	nss_top->stats_node[if_num][NSS_STATS_NODE_RX_PKTS] += node_stats_ptr->rx_packets;
	nss_top->stats_node[if_num][NSS_STATS_NODE_RX_BYTES] += node_stats_ptr->rx_bytes;
	nss_top->stats_node[if_num][NSS_STATS_NODE_TX_PKTS] += node_stats_ptr->tx_packets;
	nss_top->stats_node[if_num][NSS_STATS_NODE_TX_BYTES] += node_stats_ptr->tx_bytes;

	for (i = 0; i < NSS_MAX_NUM_PRI; i++) {
		nss_top->stats_node[if_num][NSS_STATS_NODE_RX_QUEUE_0_DROPPED + i] +=
			node_stats_ptr->rx_dropped[i];
	}

	spin_unlock_bh(&nss_mirror_stats_lock);
}

/*
 * nss_mirror_stats_reset()
 *	API to reset the mirror interface stats.
 */
void nss_mirror_stats_reset(uint32_t if_num)
{
	struct nss_mirror_stats_debug_instance *mirror_debug_instance = NULL;
	uint8_t i;

	/*
	 * Reset common node stats.
	 */
	nss_stats_reset_common_stats(if_num);

	/*
	 * Reset mirror stats.
	 */
	spin_lock_bh(&nss_mirror_stats_lock);
	for (i = 0; i < NSS_MAX_MIRROR_DYNAMIC_INTERFACES; i++) {
		if (!stats_db[i] || (stats_db[i]->if_num != if_num)) {
			continue;
		}

		mirror_debug_instance = stats_db[i];
		stats_db[i] = NULL;
		break;
	}
	spin_unlock_bh(&nss_mirror_stats_lock);

	if (mirror_debug_instance) {
		vfree(mirror_debug_instance);
	}
}

/*
 * nss_mirror_stats_init()
 *	API to initialize mirror debug instance statistics.
 */
int nss_mirror_stats_init(uint32_t if_num, struct net_device *netdev)
{
	struct nss_mirror_stats_debug_instance *mirror_debug_instance = NULL;
	uint8_t i;

	mirror_debug_instance =
		(struct nss_mirror_stats_debug_instance *)vzalloc(sizeof(struct nss_mirror_stats_debug_instance));
	if (!mirror_debug_instance) {
		nss_warning("Memory alloc failed for mirror stats instance.\n");
		return -1;
	}

	spin_lock_bh(&nss_mirror_stats_lock);
	for (i = 0; i < NSS_MAX_MIRROR_DYNAMIC_INTERFACES; i++) {
		if (stats_db[i] != NULL) {
			continue;
		}

		stats_db[i] = mirror_debug_instance;
		stats_db[i]->if_num = if_num;
		stats_db[i]->if_index = netdev->ifindex;
		spin_unlock_bh(&nss_mirror_stats_lock);
		return 0;
	}
	spin_unlock_bh(&nss_mirror_stats_lock);
	vfree(mirror_debug_instance);
	return -1;
}

/*
 * nss_mirror_stats_ops
 */
NSS_STATS_DECLARE_FILE_OPERATIONS(mirror)

/*
 * nss_mirror_stats_dentry_create()
 *	Create mirror interface statistics debug entry.
 */
void nss_mirror_stats_dentry_create(void)
{
	nss_stats_create_dentry("mirror", &nss_mirror_stats_ops);
}

/*
 * nss_mirror_stats_notify()
 *	Sends notifications to all the registered modules.
 *
 * Leverage NSS-FW statistics timing to update Netlink.
 */
void nss_mirror_stats_notify(struct nss_ctx_instance *nss_ctx, uint32_t if_num)
{
	struct nss_mirror_stats_notification mirror_stats;
	int i;

	spin_lock_bh(&nss_mirror_stats_lock);
	for (i = 0; i < NSS_MAX_MIRROR_DYNAMIC_INTERFACES; i++) {
		if (!stats_db[i] || (stats_db[i]->if_num != if_num)) {
			continue;
		}

		memcpy(mirror_stats.stats_ctx, stats_db[i]->stats, sizeof(mirror_stats.stats_ctx));
		mirror_stats.core_id = nss_ctx->id;
		mirror_stats.if_num = if_num;
		spin_unlock_bh(&nss_mirror_stats_lock);
		atomic_notifier_call_chain(&nss_mirror_stats_notifier, NSS_STATS_EVENT_NOTIFY, &mirror_stats);
		return;
	}
	spin_unlock_bh(&nss_mirror_stats_lock);
}

/*
 * nss_mirror_stats_unregister_notifier()
 *	Deregisters statistics notifier.
 */
int nss_mirror_stats_unregister_notifier(struct notifier_block *nb)
{
	return atomic_notifier_chain_unregister(&nss_mirror_stats_notifier, nb);
}
EXPORT_SYMBOL(nss_mirror_stats_unregister_notifier);

/*
 * nss_mirror_stats_register_notifier()
 *	Registers statistics notifier.
 */
int nss_mirror_stats_register_notifier(struct notifier_block *nb)
{
	return atomic_notifier_chain_register(&nss_mirror_stats_notifier, nb);
}
EXPORT_SYMBOL(nss_mirror_stats_register_notifier);

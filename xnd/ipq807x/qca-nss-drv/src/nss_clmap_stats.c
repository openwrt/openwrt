/*
 **************************************************************************
 * Copyright (c) 2019-2021, The Linux Foundation. All rights reserved.
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

#include "nss_tx_rx_common.h"
#include "nss_clmap.h"
#include "nss_clmap_stats.h"
#include "nss_clmap_strings.h"

/*
 * Declare atomic notifier data structure for statistics.
 */
ATOMIC_NOTIFIER_HEAD(nss_clmap_stats_notifier);

/*
 * Spinlock to protect clmap statistics update/read
 */
DEFINE_SPINLOCK(nss_clmap_stats_lock);

struct nss_clmap_stats *stats_db[NSS_CLMAP_MAX_DEBUG_INTERFACES] = {NULL};

/*
 * nss_clmap_interface_type_str
 *	Clmap interface type string.
 */
static char *nss_clmap_interface_type_str[NSS_CLMAP_INTERFACE_TYPE_MAX] = {
	"Upstream",
	"Downstream"
};

/*
 * nss_clmap_stats_session_unregister
 *	Unregister debug statistic for clmap session.
 */
void nss_clmap_stats_session_unregister(uint32_t if_num)
{
	uint32_t i;

	spin_lock_bh(&nss_clmap_stats_lock);
	for (i = 0; i < NSS_CLMAP_MAX_DEBUG_INTERFACES; i++) {
		if (stats_db[i] && (stats_db[i]->nss_if_num == if_num)) {
			kfree(stats_db[i]);
			stats_db[i] = NULL;
			break;
		}
	}
	spin_unlock_bh(&nss_clmap_stats_lock);
}

/*
 * nss_clmap_stats_session_register
 * 	Register debug statistic for clmap session.
 */
bool nss_clmap_stats_session_register(uint32_t if_num, uint32_t if_type, struct net_device *netdev)
{
	uint32_t i;
	bool stats_status = false;

	if (!netdev) {
		nss_warning("Could not allocate statistics memory as the net device is NULL!\n");
		return stats_status;
	}

	spin_lock_bh(&nss_clmap_stats_lock);
	for (i = 0; i < NSS_CLMAP_MAX_DEBUG_INTERFACES; i++) {
		if (!stats_db[i]) {
			stats_db[i] = (struct nss_clmap_stats *)kzalloc(sizeof(struct nss_clmap_stats), GFP_KERNEL);
			if (!stats_db[i]) {
				nss_warning("%px: could not allocate memory for statistics database for interface id: %d\n", netdev, if_num);
				break;
			}
			stats_db[i]->valid = true;
			stats_db[i]->nss_if_num = if_num;
			stats_db[i]->nss_if_type = if_type;
			stats_db[i]->if_index = netdev->ifindex;
			stats_status = true;
			break;
		}
	}
	spin_unlock_bh(&nss_clmap_stats_lock);
	return stats_status;
}

/*
 * nss_clmap_get_debug_stats()
 *	Get clmap debug statistics.
 */
static int nss_clmap_get_debug_stats(struct nss_clmap_stats *stats)
{
	uint32_t i;
	int interface_cnt = 0;

	spin_lock_bh(&nss_clmap_stats_lock);
	for (i = 0; i < NSS_CLMAP_MAX_DEBUG_INTERFACES; i++) {
		if (stats_db[i]) {
			memcpy(stats, stats_db[i], sizeof(struct nss_clmap_stats));
			stats++;
			interface_cnt++;
		}
	}
	spin_unlock_bh(&nss_clmap_stats_lock);

	return interface_cnt;
}

/*
 * nss_clmap_stats_read()
 *	Read clmap statistics
 */
static ssize_t nss_clmap_stats_read(struct file *fp, char __user *ubuf,
					size_t sz, loff_t *ppos)
{
	uint32_t max_output_lines = (NSS_CLMAP_INTERFACE_STATS_MAX * NSS_CLMAP_MAX_DEBUG_INTERFACES) + NSS_STATS_EXTRA_OUTPUT_LINES;
	size_t size_al = NSS_STATS_MAX_STR_LENGTH * max_output_lines;
	size_t size_wr = 0;
	ssize_t bytes_read = 0;
	struct net_device *dev;
	uint32_t id;
	struct nss_clmap_stats *clmap_stats = NULL;
	int interface_cnt;

	char *lbuf = kzalloc(size_al, GFP_KERNEL);
	if (unlikely(!lbuf)) {
		nss_warning("Could not allocate memory for local statistics buffer");
		return 0;
	}

	/*
	 * Allocate statistics memory only for all interfaces.
	 */
	clmap_stats = kzalloc((NSS_CLMAP_MAX_DEBUG_INTERFACES * sizeof(struct nss_clmap_stats)), GFP_KERNEL);
	if (unlikely(!clmap_stats)) {
		nss_warning("Could not allocate memory for populating clmap statistics\n");
		kfree(lbuf);
		return 0;
	}

	/*
	 * Get clmap statistics.
	 */
	interface_cnt = nss_clmap_get_debug_stats(clmap_stats);
	size_wr += nss_stats_banner(lbuf, size_wr, size_al, "clmap stats", NSS_STATS_SINGLE_CORE);
	for (id = 0; id < interface_cnt; id++) {
		struct nss_clmap_stats *clmsp = clmap_stats + id;

		if (unlikely(!clmsp->valid)) {
			continue;
		}

		dev = dev_get_by_index(&init_net, clmsp->if_index);
		if (unlikely(!dev)) {
			nss_warning("No netdev available for nss interface id:%d\n", clmsp->nss_if_num);
			continue;
		}

		size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, "%d. nss interface id=%d, interface type=%s, netdevice=%s\n", id,
				clmsp->nss_if_num, nss_clmap_interface_type_str[clmsp->nss_if_type], dev->name);
		dev_put(dev);

		size_wr += nss_stats_print("clmap", NULL, NSS_STATS_SINGLE_INSTANCE, nss_clmap_strings_stats,
						clmsp->stats, NSS_CLMAP_INTERFACE_STATS_MAX, lbuf, size_wr, size_al);
	}

	bytes_read = simple_read_from_buffer(ubuf, sz, ppos, lbuf, size_wr);

	kfree(clmap_stats);
	kfree(lbuf);
	return bytes_read;
}

/*
 * nss_clmap_stats_sync()
 *	Sync function for clmap statistics
 */
void nss_clmap_stats_sync(struct nss_ctx_instance *nss_ctx, struct nss_clmap_stats_msg *stats_msg, uint32_t if_num)
{
	uint32_t i;
	struct nss_clmap_stats *s = NULL;

	NSS_VERIFY_CTX_MAGIC(nss_ctx);

	spin_lock_bh(&nss_clmap_stats_lock);
	for (i = 0; i < NSS_CLMAP_MAX_DEBUG_INTERFACES; i++) {
		if (stats_db[i] && (stats_db[i]->nss_if_num == if_num)) {
			s = stats_db[i];
			break;
		}
	}

	if (!s) {
		spin_unlock_bh(&nss_clmap_stats_lock);
		nss_warning("%px: Interface not found: %u", nss_ctx, if_num);
		return;
	}

        s->stats[NSS_CLMAP_INTERFACE_STATS_RX_PKTS] += stats_msg->node_stats.rx_packets;
        s->stats[NSS_CLMAP_INTERFACE_STATS_RX_BYTES] += stats_msg->node_stats.rx_bytes;
        s->stats[NSS_CLMAP_INTERFACE_STATS_TX_PKTS] += stats_msg->node_stats.tx_packets;
        s->stats[NSS_CLMAP_INTERFACE_STATS_TX_BYTES] += stats_msg->node_stats.tx_bytes;

	for (i = 0; i < NSS_MAX_NUM_PRI; i++) {
		s->stats[NSS_CLMAP_INTERFACE_STATS_RX_QUEUE_0_DROPPED + i] += stats_msg->node_stats.rx_dropped[i];
	}

	s->stats[NSS_CLMAP_INTERFACE_STATS_DROPPED_MACDB_LOOKUP_FAILED] += stats_msg->dropped_macdb_lookup_failed;
	s->stats[NSS_CLMAP_INTERFACE_STATS_DROPPED_INVALID_PACKET_SIZE] += stats_msg->dropped_invalid_packet_size;
	s->stats[NSS_CLMAP_INTERFACE_STATS_DROPPED_LOW_HEADROOM] += stats_msg->dropped_low_hroom;
	s->stats[NSS_CLMAP_INTERFACE_STATS_DROPPED_NEXT_NODE_QUEUE_FULL] += stats_msg->dropped_next_node_queue_full;
	s->stats[NSS_CLMAP_INTERFACE_STATS_DROPPED_PBUF_ALLOC_FAILED] += stats_msg->dropped_pbuf_alloc_failed;
	s->stats[NSS_CLMAP_INTERFACE_STATS_DROPPED_LINEAR_FAILED] += stats_msg->dropped_linear_failed;
	s->stats[NSS_CLMAP_INTERFACE_STATS_SHARED_PACKET_CNT] += stats_msg->shared_packet_count;
	s->stats[NSS_CLMAP_INTERFACE_STATS_ETHERNET_FRAME_ERROR] += stats_msg->ethernet_frame_error;
	s->stats[NSS_CLMAP_INTERFACE_STATS_MACDB_CREATE_REQUESTS_CNT] += stats_msg->macdb_create_requests;
	s->stats[NSS_CLMAP_INTERFACE_STATS_MACDB_CREATE_MAC_EXISTS_CNT] += stats_msg->macdb_create_mac_exists;
	s->stats[NSS_CLMAP_INTERFACE_STATS_MACDB_CREATE_MAC_TABLE_FULL_CNT] += stats_msg->macdb_create_table_full;
	s->stats[NSS_CLMAP_INTERFACE_STATS_MACDB_DESTROY_REQUESTS_CNT] += stats_msg->macdb_destroy_requests;
	s->stats[NSS_CLMAP_INTERFACE_STATS_MACDB_DESTROY_MAC_NOT_FOUND_CNT] += stats_msg->macdb_destroy_mac_notfound;
	s->stats[NSS_CLMAP_INTERFACE_STATS_MACDB_DESTROY_MAC_UNHASHED_CNT] += stats_msg->macdb_destroy_mac_unhashed;
	s->stats[NSS_CLMAP_INTERFACE_STATS_MACDB_FLUSH_REQUESTS_CNT] += stats_msg->macdb_flush_requests;
	spin_unlock_bh(&nss_clmap_stats_lock);
}

/*
 * nss_clmap_stats_ops
 */
NSS_STATS_DECLARE_FILE_OPERATIONS(clmap)

/*
 * nss_clmap_stats_dentry_create()
 *	Create client map statistics debug entry.
 */
void nss_clmap_stats_dentry_create(void)
{
	nss_stats_create_dentry("clmap", &nss_clmap_stats_ops);
}

/*
 * nss_clmap_stats_notify()
 *	Sends notifications to all the registered modules.
 *
 * Leverage NSS-FW statistics timing to update Netlink.
 */
void nss_clmap_stats_notify(struct nss_ctx_instance *nss_ctx, uint32_t if_num)
{
	struct nss_clmap_stats_notification clmap_stats;
	struct nss_clmap_stats *s = NULL;
	int i;

	spin_lock_bh(&nss_clmap_stats_lock);
	for (i = 0; i < NSS_CLMAP_MAX_DEBUG_INTERFACES; i++) {
		if (!stats_db[i] || (stats_db[i]->nss_if_num != if_num)) {
			continue;
		}

		s = stats_db[i];
		clmap_stats.core_id = nss_ctx->id;
		clmap_stats.if_num = if_num;
		memcpy(clmap_stats.stats_ctx, s->stats, sizeof(clmap_stats.stats_ctx));
		spin_unlock_bh(&nss_clmap_stats_lock);
		atomic_notifier_call_chain(&nss_clmap_stats_notifier, NSS_STATS_EVENT_NOTIFY, &clmap_stats);
		return;
	}
	spin_unlock_bh(&nss_clmap_stats_lock);
}

/*
 * nss_clmap_stats_unregister_notifier()
 *	Deregisters statistics notifier.
 */
int nss_clmap_stats_unregister_notifier(struct notifier_block *nb)
{
	return atomic_notifier_chain_unregister(&nss_clmap_stats_notifier, nb);
}
EXPORT_SYMBOL(nss_clmap_stats_unregister_notifier);

/*
 * nss_clmap_stats_register_notifier()
 *	Registers statistics notifier.
 */
int nss_clmap_stats_register_notifier(struct notifier_block *nb)
{
	return atomic_notifier_chain_register(&nss_clmap_stats_notifier, nb);
}
EXPORT_SYMBOL(nss_clmap_stats_register_notifier);

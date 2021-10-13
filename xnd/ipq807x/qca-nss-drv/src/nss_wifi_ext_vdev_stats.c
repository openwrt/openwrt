/*
 **************************************************************************
 * Copyright (c) 2020, The Linux Foundation. All rights reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 **************************************************************************
 */

#include "nss_tx_rx_common.h"
#include "nss_wifi_ext_vdev_stats.h"

DEFINE_SPINLOCK(nss_wifi_ext_vdev_debug_lock);
struct nss_wifi_ext_vdev_debug nss_wifi_ext_vdev_debug_stats[NSS_WIFI_EXT_VDEV_MAX];

/*
 * nss_wifi_ext_vdev_debug_str
 *	WiFi extended VAP statistics strings.
 */
struct nss_stats_info nss_wifi_ext_vdev_debug_str[NSS_WIFI_EXT_VDEV_STATS_MAX] = {
	{"node_rx_pkts"					, NSS_STATS_TYPE_COMMON},
	{"node_rx_bytes"				, NSS_STATS_TYPE_COMMON},
	{"node_tx_pkts"					, NSS_STATS_TYPE_COMMON},
	{"node_tx_bytes"				, NSS_STATS_TYPE_COMMON},
	{"node_rx_dropped"				, NSS_STATS_TYPE_DROP},
	{"mc_count"					, NSS_STATS_TYPE_SPECIAL},
	{"uc_count"					, NSS_STATS_TYPE_SPECIAL},
	{"nxt_hop_drop"					, NSS_STATS_TYPE_DROP},
};

/*
 * WiFi extended vdev statistics APIs
 */

/*
 * nss_wifi_ext_vdev_stats_register()
 *	Register debug statistic for WiFi extended VAP.
 */
void nss_wifi_ext_vdev_stats_register(uint32_t if_num, struct net_device *netdev)
{
	int i;

	spin_lock_bh(&nss_wifi_ext_vdev_debug_lock);
	for (i = 0; i < NSS_WIFI_EXT_VDEV_MAX; i++) {
		if (!nss_wifi_ext_vdev_debug_stats[i].valid) {
			nss_wifi_ext_vdev_debug_stats[i].valid = true;
			nss_wifi_ext_vdev_debug_stats[i].if_num = if_num;
			nss_wifi_ext_vdev_debug_stats[i].if_index = netdev->ifindex;
			break;
		}
	}

	spin_unlock_bh(&nss_wifi_ext_vdev_debug_lock);
}

/*
 * nss_wifi_ext_vdev_stats_unregister()
 *	Register debug statistic for WiFi extended vap.
 */
void nss_wifi_ext_vdev_stats_unregister(uint32_t if_num, struct net_device *netdev)
{
	int i;

	spin_lock_bh(&nss_wifi_ext_vdev_debug_lock);
	for (i = 0; i < NSS_WIFI_EXT_VDEV_MAX; i++) {
		if (nss_wifi_ext_vdev_debug_stats[i].if_num == if_num) {
			memset(&nss_wifi_ext_vdev_debug_stats[i], 0,
			       sizeof(struct nss_wifi_ext_vdev_debug));
			break;
		}
	}
	spin_unlock_bh(&nss_wifi_ext_vdev_debug_lock);
}

/*
 * nss_wifi_ext_vdev_stats_sync()
 *	Sync function for WiFi extendev vap statistics.
 */
void nss_wifi_ext_vdev_stats_sync(struct nss_ctx_instance *nss_ctx, struct nss_wifi_ext_vdev_stats *stats_msg,
					uint16_t if_num)
{
	int i;
	struct nss_wifi_ext_vdev_debug *s = NULL;

	NSS_VERIFY_CTX_MAGIC(nss_ctx);

	spin_lock_bh(&nss_wifi_ext_vdev_debug_lock);
	for (i = 0; i < NSS_WIFI_EXT_VDEV_MAX; i++) {
		if (nss_wifi_ext_vdev_debug_stats[i].if_num == if_num) {
			s = &nss_wifi_ext_vdev_debug_stats[i];
			break;
		}
	}

	if (!s) {
		spin_unlock_bh(&nss_wifi_ext_vdev_debug_lock);
		nss_warning("%px: Interface:%u not found", nss_ctx, if_num);
		return;
	}

	s->stats[NSS_WIFI_EXT_VDEV_STATS_NODE_RX_PKTS ] += stats_msg->node_stats.rx_packets;
	s->stats[NSS_WIFI_EXT_VDEV_STATS_NODE_RX_BYTES] += stats_msg->node_stats.rx_bytes;
	s->stats[NSS_WIFI_EXT_VDEV_STATS_NODE_TX_PKTS] += stats_msg->node_stats.tx_packets;
	s->stats[NSS_WIFI_EXT_VDEV_STATS_NODE_TX_BYTES] += stats_msg->node_stats.tx_bytes;
	for (i = 0; i < NSS_MAX_NUM_PRI; i++) {
		s->stats[NSS_WIFI_EXT_VDEV_STATS_NODE_TOTAL_DROPPED] += stats_msg->node_stats.rx_dropped[i];
	}
	s->stats[NSS_WIFI_EXT_VDEV_STATS_MULTICAST_COUNT] += stats_msg->mc_count;
	s->stats[NSS_WIFI_EXT_VDEV_STATS_UNICAST_COUNT] += stats_msg->node_stats.rx_packets - stats_msg->mc_count;
	s->stats[NSS_WIFI_EXT_VDEV_STATS_NEXT_HOP_DROP_COUNT] += stats_msg->nxt_hop_drp;
	spin_unlock_bh(&nss_wifi_ext_vdev_debug_lock);
}

/*
 * nss_wifi_ext_vdev_debug_get()
 *	Get WiFi extendev vap debug statitics.
 */
static void nss_wifi_ext_vdev_debug_get(struct nss_wifi_ext_vdev_debug *stats)
{
	int i;

	if (!stats) {
		nss_warning("No memory to copy WiFi extended VAP stats");
		return;
	}

	spin_lock_bh(&nss_wifi_ext_vdev_debug_lock);
	for (i = 0; i < NSS_WIFI_EXT_VDEV_MAX; i++) {
		if (nss_wifi_ext_vdev_debug_stats[i].valid) {
			memcpy(stats, &nss_wifi_ext_vdev_debug_stats[i],
				sizeof(struct nss_wifi_ext_vdev_debug));
			stats++;
		}
	}
	spin_unlock_bh(&nss_wifi_ext_vdev_debug_lock);
}

/*
 * nss_wifi_ext_vdev_read()
 *	Read WiFi extended VAP statistics
 */
static ssize_t nss_wifi_ext_vdev_stats_read(struct file *fp, char __user *ubuf,
					size_t sz, loff_t *ppos)
{
	uint32_t max_output_lines = 2 /* header and footer of the interface stats*/
		+ (NSS_WIFI_EXT_VDEV_STATS_MAX * (NSS_WIFI_EXT_VDEV_MAX + 2)) /* Interface stats */
		+ 2;

	size_t size_al = NSS_STATS_MAX_STR_LENGTH * max_output_lines;
	size_t size_wr = 0;
	size_t bytes_read = 0;
	struct net_device *dev;
	int id;
	struct nss_wifi_ext_vdev_debug *wifi_ext_vdev_stats = NULL;

	char *lbuf = kzalloc(size_al, GFP_KERNEL);
	if (unlikely(lbuf == NULL)) {
		nss_warning("Could not allocate memory for local statistics buffer");
		return 0;
	}

	wifi_ext_vdev_stats = kzalloc((sizeof(struct nss_wifi_ext_vdev_debug) * NSS_WIFI_EXT_VDEV_MAX), GFP_KERNEL);
	if (unlikely(wifi_ext_vdev_stats == NULL)) {
		nss_warning("Could not allocate memory for populating stats");
		kfree(lbuf);
		return 0;
	}

	/*
	 * Get all stats
	 */
	nss_wifi_ext_vdev_debug_get(wifi_ext_vdev_stats);

	/*
	 * WiFi extended vap stats.
	 */
	size_wr += nss_stats_banner(lbuf, size_wr, size_al, "WiFi extended VAP stats", NSS_STATS_SINGLE_CORE);

	for (id = 0; id < NSS_WIFI_EXT_VDEV_MAX; id++) {
		if (!wifi_ext_vdev_stats[id].valid) {
			continue;
		}

		dev = dev_get_by_index(&init_net, wifi_ext_vdev_stats[id].if_index);
		if (likely(dev)) {
			size_wr += scnprintf(lbuf + size_wr, size_al - size_wr,
						"%d. nss interface id=%d, netdevice=%s\n",
						id, wifi_ext_vdev_stats[id].if_num,
						dev->name);
			dev_put(dev);
		} else {
			size_wr += scnprintf(lbuf + size_wr, size_al - size_wr,
						"%d. nss interface id=%d\n", id,
						wifi_ext_vdev_stats[id].if_num);
		}

		size_wr += nss_stats_print("vdev", "debug", id
						, nss_wifi_ext_vdev_debug_str
						, wifi_ext_vdev_stats[id].stats
						, NSS_WIFI_EXT_VDEV_STATS_MAX
						, lbuf, size_wr, size_al);

		size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, "\n");
	}

	bytes_read = simple_read_from_buffer(ubuf, sz, ppos, lbuf, size_wr);

	kfree(wifi_ext_vdev_stats);
	kfree(lbuf);
	return bytes_read;
}

/*
 * nss_wifi_ext_vdev_ops
 */
NSS_STATS_DECLARE_FILE_OPERATIONS(wifi_ext_vdev);

/*
 * nss_wifi_ext_vdev_dentry_create()
 *	Create wifi extension vap statistics debug entry.
 */
void nss_wifi_ext_vdev_stats_dentry_create(void)
{
	nss_stats_create_dentry("wifi_ext_vdev", &nss_wifi_ext_vdev_stats_ops);
}

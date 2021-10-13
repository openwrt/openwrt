/*
 ******************************************************************************
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
 * ****************************************************************************
 */

#ifndef __NSS_WIFI_EXT_VDEV_STATS_H
#define __NSS_WIFI_EXT_VDEV_STATS_H

/*
 * WiFi extendev vap debug statistic counters.
 */
enum nss_wifi_ext_vdev_stats_types {
	NSS_WIFI_EXT_VDEV_STATS_NODE_RX_PKTS,
	NSS_WIFI_EXT_VDEV_STATS_NODE_RX_BYTES,
	NSS_WIFI_EXT_VDEV_STATS_NODE_TX_PKTS,
	NSS_WIFI_EXT_VDEV_STATS_NODE_TX_BYTES,
	NSS_WIFI_EXT_VDEV_STATS_NODE_TOTAL_DROPPED,
	NSS_WIFI_EXT_VDEV_STATS_MULTICAST_COUNT,
	NSS_WIFI_EXT_VDEV_STATS_UNICAST_COUNT,
	NSS_WIFI_EXT_VDEV_STATS_NEXT_HOP_DROP_COUNT,
	NSS_WIFI_EXT_VDEV_STATS_MAX,
};

/*
 * WiFi extendev vap debug statistics.
 */
struct nss_wifi_ext_vdev_debug {
	uint64_t stats[NSS_WIFI_EXT_VDEV_STATS_MAX];
	int32_t if_index;					/**< Netdevice's ifindex. */
	uint32_t if_num;					/**< NSS interface number. */
	bool valid;						/**< Is node valid ? */
};

/*
 * Data structures to store WiFi extended VAP debug stats.
 */
extern struct nss_wifi_ext_vdev_debug nss_wifi_ext_vdev_debug_stats[NSS_WIFI_EXT_VDEV_MAX];

/*
 * WiFi extendev vap statistics APIs
 */
extern void nss_wifi_ext_vdev_stats_register(uint32_t if_num, struct net_device *netdev);
extern void nss_wifi_ext_vdev_stats_unregister(uint32_t if_num, struct net_device *netdev);
extern void nss_wifi_ext_vdev_stats_sync(struct nss_ctx_instance *nss_ctx, struct nss_wifi_ext_vdev_stats *stats_msg, uint16_t if_num);
extern void nss_wifi_ext_vdev_stats_dentry_create(void);

#endif /* __NSS_WIFI_EXT_VDEV_STATS_H */

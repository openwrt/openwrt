/*
 **************************************************************************
 * Copyright (c) 2019, The Linux Foundation. All rights reserved.
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
#include "nss_vxlan_stats.h"
#include <nss_vxlan.h>

#define NSS_VXLAN_STATS_MAX_LINES (NSS_STATS_NODE_MAX + 32)
					/**< Maximum number of lines for VXLAN statistics dump. */
#define NSS_VXLAN_STATS_SIZE_PER_IF (NSS_STATS_MAX_STR_LENGTH * NSS_VXLAN_STATS_MAX_LINES)
					/**< Total number of statistics per VXLAN interface. */

/*
 * nss_vxlan_stats_read()
 *	Read vxlan node statiistics.
 */
static ssize_t nss_vxlan_stats_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	struct nss_ctx_instance *nss_ctx = nss_vxlan_get_ctx();
	enum nss_dynamic_interface_type type;
	ssize_t bytes_read = 0;
	size_t len = 0, size;
	uint32_t if_num;
	char *buf;

	size = NSS_VXLAN_STATS_SIZE_PER_IF;
	buf = kzalloc(size, GFP_KERNEL);
	if (!buf) {
		nss_warning("Could not allocate memory for local statistics buffer\n");
		return 0;
	}

	/*
	 * Common node stats for each VxLAN dynamic interface.
	 */
	for (if_num = 0; if_num < NSS_MAX_NET_INTERFACES; if_num++) {
		if (if_num == NSS_VXLAN_INTERFACE) {
			len += scnprintf(buf + len, size - len, "\nBase node if_num:%03u", if_num);
			len += scnprintf(buf + len, size - len, "\n-------------------\n");
			len += nss_stats_fill_common_stats(if_num, NSS_STATS_SINGLE_INSTANCE, buf, len, size - len, "vxlan");
			continue;
		}

		type = nss_dynamic_interface_get_type(nss_ctx, if_num);
		switch (type) {
		case NSS_DYNAMIC_INTERFACE_TYPE_VXLAN_INNER:
			len += scnprintf(buf + len, size - len, "\nInner if_num:%03u", if_num);
			break;

		case NSS_DYNAMIC_INTERFACE_TYPE_VXLAN_OUTER:
			len += scnprintf(buf + len, size - len, "\nOuter if_num:%03u", if_num);
			break;

		default:
			continue;
		}

		len += scnprintf(buf + len, size - len, "\n-------------------\n");
		len += nss_stats_fill_common_stats(if_num, NSS_STATS_SINGLE_INSTANCE, buf, len, size - len, "vxlan");
	}

	bytes_read = simple_read_from_buffer(ubuf, sz, ppos, buf, len);
	kfree(buf);
	return bytes_read;
}

/*
 * nss_vxlan_stats_sync()
 *	Update vxlan common node statistics.
 */
void nss_vxlan_stats_sync(struct nss_ctx_instance *nss_ctx, struct nss_vxlan_msg *nvm)
{
	struct nss_top_instance *nss_top = nss_ctx->nss_top;
	struct nss_vxlan_stats_msg *msg_stats = &nvm->msg.stats;
	uint64_t *if_stats;

	spin_lock_bh(&nss_top->stats_lock);

	/*
	 * Update common node stats
	 */
	if_stats = nss_top->stats_node[nvm->cm.interface];
	if_stats[NSS_STATS_NODE_RX_PKTS] += msg_stats->node_stats.rx_packets;
	if_stats[NSS_STATS_NODE_RX_BYTES] += msg_stats->node_stats.rx_bytes;
	if_stats[NSS_STATS_NODE_RX_QUEUE_0_DROPPED] += msg_stats->node_stats.rx_dropped[0];
	if_stats[NSS_STATS_NODE_RX_QUEUE_1_DROPPED] += msg_stats->node_stats.rx_dropped[1];
	if_stats[NSS_STATS_NODE_RX_QUEUE_2_DROPPED] += msg_stats->node_stats.rx_dropped[2];
	if_stats[NSS_STATS_NODE_RX_QUEUE_3_DROPPED] += msg_stats->node_stats.rx_dropped[3];

	if_stats[NSS_STATS_NODE_TX_PKTS] += msg_stats->node_stats.tx_packets;
	if_stats[NSS_STATS_NODE_TX_BYTES] += msg_stats->node_stats.tx_bytes;

	spin_unlock_bh(&nss_top->stats_lock);
}

/*
 * nss_vxlan_stats_ops
 */
NSS_STATS_DECLARE_FILE_OPERATIONS(vxlan)

/*
 * nss_vxlan_stats_dentry_create()
 *	Create vxlan statistics debug entry.
 */
void nss_vxlan_stats_dentry_create(void)
{
	nss_stats_create_dentry("vxlan", &nss_vxlan_stats_ops);
}

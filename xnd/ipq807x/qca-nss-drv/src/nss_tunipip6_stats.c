/*
 **************************************************************************
 * Copyright (c) 2020, The Linux Foundation. All rights reserved.

 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.

 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 **************************************************************************
 */

#include "nss_core.h"
#include "nss_tunipip6.h"
#include "nss_stats.h"
#include "nss_tunipip6_stats.h"

#define NSS_TUNIPIP6_STATS_MAX_LINES (NSS_STATS_NODE_MAX + 32)
					/**< Maximum number of lines for tunipip6 statistics dump. */
#define NSS_TUNIPIP6_STATS_SIZE_PER_IF (NSS_STATS_MAX_STR_LENGTH * NSS_TUNIPIP6_STATS_MAX_LINES)
					/**< Total number of statistics per tunipip6 interface. */

/*
 * nss_tunipip6_stats_read()
 *	Read tunipip6 common node statistics
 */
static ssize_t nss_tunipip6_stats_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	struct nss_ctx_instance *nss_ctx = nss_tunipip6_get_context();
	enum nss_dynamic_interface_type type;
	ssize_t bytes_read = 0;
	size_t len = 0, size;
	uint32_t if_num;
	char *buf;

	/*
	 * Allocate memory for NSS_TUNIPIP6_TUNNEL_MAX tunnels and one
	 * static interface.
	 */
	size = NSS_TUNIPIP6_STATS_SIZE_PER_IF * (NSS_TUNIPIP6_TUNNEL_MAX << 1) + 1;
	buf = vzalloc(size);
	if (!buf) {
		nss_warning("tunipip6: Could not allocate memory for local statistics buffer\n");
		return 0;
	}

	len += nss_stats_banner(buf, len, size, "tunipip6", NSS_STATS_SINGLE_CORE);

	len += scnprintf(buf + len, size - len, "\nBase node if_num:%03u", NSS_TUNIPIP6_INTERFACE);
	len += scnprintf(buf + len, size - len, "\n-------------------\n");
	len += nss_stats_fill_common_stats(NSS_TUNIPIP6_INTERFACE, NSS_STATS_SINGLE_INSTANCE, buf, len, size - len, "tunipip6");

	/*
	 * Common node stats for each tunipip6 dynamic interface.
	 */
	for (if_num = NSS_DYNAMIC_IF_START; if_num < NSS_DYNAMIC_IF_START + NSS_MAX_DYNAMIC_INTERFACES; if_num++) {
		type = nss_dynamic_interface_get_type(nss_ctx, if_num);
		switch (type) {
		case NSS_DYNAMIC_INTERFACE_TYPE_TUNIPIP6_INNER:
			len += scnprintf(buf + len, size - len, "\nInner if_num:%03u", if_num);
			break;

		case NSS_DYNAMIC_INTERFACE_TYPE_TUNIPIP6_OUTER:
			len += scnprintf(buf + len, size - len, "\nOuter if_num:%03u", if_num);
			break;

		default:
			continue;
		}

		len += scnprintf(buf + len, size - len, "\n-------------------\n");
		len += nss_stats_fill_common_stats(if_num, NSS_STATS_SINGLE_INSTANCE, buf, len, size - len, "tunipip6");
	}

        bytes_read = simple_read_from_buffer(ubuf, sz, ppos, buf, len);
        vfree(buf);
        return bytes_read;
}

/*
 * nss_tunipip6_stats_sync()
 * 	Update tunipip6 common node statistics.
 */
void nss_tunipip6_stats_sync(struct nss_ctx_instance *nss_ctx, struct nss_tunipip6_msg *ntm)
{
	struct nss_top_instance *nss_top = nss_ctx->nss_top;
	struct nss_tunipip6_stats_sync_msg *msg_stats = &ntm->msg.stats;
	uint64_t i, *dest;
	uint32_t *src;

	spin_lock_bh(&nss_top->stats_lock);

	/*
	 * Update common node stats
	 */
	dest = nss_top->stats_node[ntm->cm.interface];
	src = &msg_stats->node_stats.rx_packets;
	for (i = NSS_STATS_NODE_RX_PKTS; i <= NSS_STATS_NODE_RX_QUEUE_3_DROPPED; i++) {
		*dest++ = *src++;
	}

	spin_unlock_bh(&nss_top->stats_lock);

}

/*
 * nss_tunipip6_stats_ops
 */
NSS_STATS_DECLARE_FILE_OPERATIONS(tunipip6)

/*
 * nss_tunipip6_stats_dentry_create()
 *	Create tunipip6 statistics debug entry.
 */
void nss_tunipip6_stats_dentry_create(void)
{
	nss_stats_create_dentry("tunipip6", &nss_tunipip6_stats_ops);
}

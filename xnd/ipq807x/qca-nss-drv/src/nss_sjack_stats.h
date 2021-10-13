/*
 ******************************************************************************
 * Copyright (c) 2017, The Linux Foundation. All rights reserved.
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
 * ****************************************************************************
 */

#ifndef __NSS_SJACK_STATS_H
#define __NSS_SJACK_STATS_H

/*
 * SJACK statistics
 */
enum nss_sjack_stats_types {
	NSS_SJACK_STATS_RX_PKTS,	/* sjack node RX packets */
	NSS_SJACK_STATS_RX_BYTES,	/* sjack node RX bytes */
	NSS_SJACK_STATS_TX_PKTS,	/* sjack node TX packets */
	NSS_SJACK_STATS_TX_BYTES,	/* sjack node TX bytes */
	NSS_SJACK_STATS_RX_QUEUE_0_DROPPED,
					/* sjack node RX Queue 0 dropped */
	NSS_SJACK_STATS_RX_QUEUE_1_DROPPED,
					/* sjack node RX Queue 1 dropped */
	NSS_SJACK_STATS_RX_QUEUE_2_DROPPED,
					/* sjack node RX Queue 2 dropped */
	NSS_SJACK_STATS_RX_QUEUE_3_DROPPED,
					/* sjack node RX Queue 3 dropped */
	NSS_SJACK_STATS_MAX,
};

/*
 * SJACK statistics APIs
 */
extern void nss_sjack_stats_node_sync(struct nss_ctx_instance *nss_ctx, struct nss_sjack_stats_sync_msg *nins);
extern void nss_sjack_stats_dentry_create(void);

#endif /* __NSS_SJACK_STATS_H */

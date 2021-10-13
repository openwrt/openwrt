/*
 **************************************************************************
 * Copyright (c) 2017, 2021, The Linux Foundation. All rights reserved.
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
 * nss_gre_stats.h
 *	NSS GRE statistics header file.
 */

#ifndef __NSS_GRE_STATS_H
#define __NSS_GRE_STATS_H

#include <nss_cmn.h>

/*
 *  GRE base debug statistics
 */
struct nss_gre_stats_base {
	uint64_t stats[NSS_GRE_BASE_DEBUG_MAX];	/**< GRE debug statistics. */
};

/*
 *  GRE session debug statistics
 */
struct nss_gre_stats_session {
	uint64_t stats[NSS_GRE_SESSION_DEBUG_MAX];	/**< Session debug statistics. */
	int32_t if_index;				/**< Netdevice's ifindex. */
	uint32_t if_num;				/**< NSS interface number. */
	bool valid;					/**< Is node valid ? */
};

/*
 * GRE statistics APIs
 */
extern void nss_gre_stats_base_notify(struct nss_ctx_instance *nss_ctx);
extern void nss_gre_stats_session_notify(struct nss_ctx_instance *nss_ctx, uint32_t if_num);
extern void nss_gre_stats_session_sync(struct nss_ctx_instance *nss_ctx, struct nss_gre_session_stats_msg *sstats, uint16_t if_num);
extern void nss_gre_stats_base_sync(struct nss_ctx_instance *nss_ctx, struct nss_gre_base_stats_msg *bstats);
extern void nss_gre_stats_session_register(uint32_t if_num, struct net_device *netdev);
extern void nss_gre_stats_session_unregister(uint32_t if_num);
extern void nss_gre_stats_dentry_create(void);

#endif /* __NSS_GRE_STATS_H */

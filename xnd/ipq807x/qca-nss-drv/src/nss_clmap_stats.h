/*
 ******************************************************************************
 * Copyright (c) 2019, 2021, The Linux Foundation. All rights reserved.
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

#ifndef __NSS_CLMAP_STATS_H
#define __NSS_CLMAP_STATS_H

#define NSS_CLMAP_MAX_DEBUG_INTERFACES 2 * NSS_CLMAP_MAX_INTERFACES

/*
 * Clmap session debug statistics.
 */
struct nss_clmap_stats {
	uint64_t stats[NSS_CLMAP_INTERFACE_STATS_MAX];	/* Clmap statistics. */
	int32_t if_index;				/* Interface index. */
	uint32_t nss_if_num;				/* NSS interface number. */
	enum nss_clmap_interface_type nss_if_type;	/* NSS interface type. */
	bool valid;
};

/*
 * Clmap statistics APIs.
 */
extern void nss_clmap_stats_notify(struct nss_ctx_instance *nss_ctx, uint32_t if_num);
extern bool nss_clmap_stats_session_register(uint32_t if_num, enum nss_clmap_interface_type if_type, struct net_device *netdev);
extern void nss_clmap_stats_session_unregister(uint32_t if_num);
extern void nss_clmap_stats_sync(struct nss_ctx_instance *nss_ctx, struct nss_clmap_stats_msg *stats_msg, uint32_t if_num);
extern void nss_clmap_stats_dentry_create(void);

#endif /* __NSS_CLMAP_STATS_H */

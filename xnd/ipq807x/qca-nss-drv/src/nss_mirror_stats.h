/*
 ******************************************************************************
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
 * ****************************************************************************
 */

#ifndef __NSS_MIRROR_STATS_H
#define __NSS_MIRROR_STATS_H

/*
 * Number of active mirror stats instances.
 */
extern atomic_t nss_mirror_num_instances;

/*
 * nss_mirror_stats_debug_instance
 *	Stucture for H2N/N2H mirror interface debug stats.
 */
struct nss_mirror_stats_debug_instance {
	uint64_t stats[NSS_MIRROR_STATS_MAX];	/* Mirror statistics for each instance. */
	int32_t if_index;			/* Mirror instance netdev index. */
	uint32_t if_num;			/* Mirror instance NSS interface number */
};

extern void nss_mirror_stats_sync(struct nss_ctx_instance *nss_ctx,
		 struct nss_mirror_msg *nmm, uint16_t if_num);
extern void nss_mirror_stats_reset(uint32_t if_num);
extern int nss_mirror_stats_init(uint32_t if_num, struct net_device *netdev);
extern void nss_mirror_stats_dentry_create(void);
extern void nss_mirror_stats_notify(struct nss_ctx_instance *nss_ctx, uint32_t if_num);

#endif /* __NSS_MIRROR_STATS_H */

/*
 ******************************************************************************
 * Copyright (c) 2016-2017, 2021, The Linux Foundation. All rights reserved.
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

#ifndef __NSS_GRE_TUNNEL_STATS_H
#define __NSS_GRE_TUNNEL_STATS_H

/*
 * GRE Tunnel session debug statistics
 */
struct nss_gre_tunnel_stats_session {
	uint64_t stats[NSS_GRE_TUNNEL_STATS_SESSION_MAX + NSS_CRYPTO_CMN_RESP_ERROR_MAX];
				/* GRE tunnel statistics */
	int32_t if_index;	/* Interface index */
	uint32_t if_num;	/* NSS interface number */
	bool valid;
};

/*
 * Data structures to store GRE Tunnel nss debug stats
 */
extern spinlock_t nss_gre_tunnel_stats_lock;
extern struct nss_gre_tunnel_stats_session session_stats[NSS_MAX_GRE_TUNNEL_SESSIONS];

/*
 * GRE Tunnel statistics APIs
 */
extern void nss_gre_tunnel_stats_notify(struct nss_ctx_instance *nss_ctx, uint32_t if_num);
extern void nss_gre_tunnel_stats_session_sync(struct nss_ctx_instance *nss_ctx, struct nss_gre_tunnel_stats *stats_msg, uint16_t if_num);
extern void nss_gre_tunnel_stats_dentry_create(void);

#endif /* __NSS_GRE_TUNNEL_STATS_H */

/*
 ******************************************************************************
 * Copyright (c) 2018, 2021, The Linux Foundation. All rights reserved.
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

#ifndef __NSS_GRE_REDIR_LAG_US_STATS_H__
#define __NSS_GRE_REDIR_LAG_US_STATS_H__

/*
 * nss_gre_redir_lag_us_pvt_sync_stats
 *	Hash statistics synchronization context.
 */
struct nss_gre_redir_lag_us_pvt_sync_stats {
	struct delayed_work nss_gre_redir_lag_us_work;		/**< Delayed work per LAG US node. */
	struct nss_gre_redir_lag_us_msg db_sync_msg;		/**< Hash statistics message. */
	struct nss_gre_redir_lag_us_tunnel_stats tun_stats;	/**< GRE redirect LAG common statistics. */
	nss_gre_redir_lag_us_msg_callback_t cb;			/**< Callback for hash query message. */
	void *app_data;						/**< app_data for hash query message. */
	uint32_t ifnum;						/**< NSS interface number. */
	bool valid;						/**< Valid flag. */
};

/*
 * Common context for stats update.
 */
struct nss_gre_redir_lag_us_cmn_ctx {
	struct workqueue_struct *nss_gre_redir_lag_us_wq;		/**< Work queue. */
	spinlock_t nss_gre_redir_lag_us_stats_lock;			/**< Spin lock. */
	struct nss_gre_redir_lag_us_pvt_sync_stats stats_ctx[NSS_GRE_REDIR_LAG_MAX_NODE];
};

extern void nss_gre_redir_lag_us_stats_notify(struct nss_ctx_instance *nss_ctx, uint32_t if_num);
extern bool nss_gre_redir_lag_us_get_node_idx(uint32_t ifnum, uint32_t *idx);
extern bool nss_gre_redir_lag_us_verify_ifnum(uint32_t if_num);
extern void nss_gre_redir_lag_us_stats_sync(struct nss_ctx_instance *nss_ctx,
					struct nss_gre_redir_lag_us_cmn_sync_stats_msg *ngss, uint32_t ifnum);
extern struct dentry *nss_gre_redir_lag_us_stats_dentry_create(void);

#endif

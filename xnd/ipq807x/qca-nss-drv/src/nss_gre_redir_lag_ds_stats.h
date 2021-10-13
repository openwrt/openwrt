/*
 **************************************************************************
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
 * ************************************************************************
 */

#ifndef __NSS_GRE_REDIR_LAG_DS_STATS_H__
#define __NSS_GRE_REDIR_LAG_DS_STATS_H__

extern spinlock_t nss_gre_redir_lag_ds_stats_lock;
extern void nss_gre_redir_lag_ds_stats_notify(struct nss_ctx_instance *nss_ctx, uint32_t if_num);
extern bool nss_gre_redir_lag_ds_verify_ifnum(uint32_t if_num);
extern bool nss_gre_redir_lag_ds_get_node_idx(uint32_t ifnum, uint32_t *idx);
extern void nss_gre_redir_lag_ds_stats_sync(struct nss_ctx_instance *nss_ctx,
					struct nss_gre_redir_lag_ds_sync_stats_msg *ngss, uint32_t ifnum);
extern struct dentry *nss_gre_redir_lag_ds_stats_dentry_create(void);

#endif

/*
 ***************************************************************************
 * Copyright (c) 2020-2021, The Linux Foundation. All rights reserved.
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
 ***************************************************************************
 */

#include "nss_stats.h"
#include "nss_core.h"
#include "nss_strings.h"
#include "nss_gre_redir_lag_ds_strings.h"

/*
 * nss_gre_redir_lag_ds_strings_stats
 *	GRE REDIR LAG DS common statistics strings.
 */
struct nss_stats_info nss_gre_redir_lag_ds_strings_stats[NSS_GRE_REDIR_LAG_DS_STATS_MAX] = {
	{"rx_packets",		NSS_STATS_TYPE_COMMON},
	{"rx_bytes",		NSS_STATS_TYPE_COMMON},
	{"tx_packets",		NSS_STATS_TYPE_COMMON},
	{"tx_bytes",		NSS_STATS_TYPE_COMMON},
	{"rx_dropped_0",	NSS_STATS_TYPE_DROP},
	{"rx_dropped_1",	NSS_STATS_TYPE_DROP},
	{"rx_dropped_2",	NSS_STATS_TYPE_DROP},
	{"rx_dropped_3",	NSS_STATS_TYPE_DROP},
	{"dst_invalid",		NSS_STATS_TYPE_EXCEPTION},
	{"exception_packets",	NSS_STATS_TYPE_SPECIAL}
};

/*
 * nss_gre_redir_lag_ds_strings_read()
 *	Read gre_redir_lag_ds statistics names
 */
static ssize_t nss_gre_redir_lag_ds_strings_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	return nss_strings_print(ubuf, sz, ppos, nss_gre_redir_lag_ds_strings_stats, NSS_GRE_REDIR_LAG_DS_STATS_MAX);
}

/*
 * nss_gre_redir_lag_ds_strings_ops
 */
NSS_STRINGS_DECLARE_FILE_OPERATIONS(gre_redir_lag_ds);

/*
 * nss_gre_redir_lag_ds_strings_dentry_create()
 *	Create gre_redir_lag_ds statistics strings debug entry.
 */
void nss_gre_redir_lag_ds_strings_dentry_create(void)
{
	nss_strings_create_dentry("gre_redir_lag_ds", &nss_gre_redir_lag_ds_strings_ops);
}

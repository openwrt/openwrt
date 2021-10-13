/*
 **************************************************************************
 * Copyright (c) 2019-2020, The Linux Foundation. All rights reserved.
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
 **************************************************************************
 */

#include "nss_stats.h"
#include "nss_core.h"
#include <nss_lso_rx.h>
#include "nss_strings.h"
#include "nss_lso_rx_strings.h"

/*
 * nss_lso_rx_strings_stats
 *	LSO Rx statistics strings.
 */
struct nss_stats_info nss_lso_rx_strings_stats[NSS_LSO_RX_STATS_MAX] = {
	{"tx_drops"		,NSS_STATS_TYPE_DROP},
	{"drops"		,NSS_STATS_TYPE_DROP},
	{"pbuf_alloc_fail"	,NSS_STATS_TYPE_ERROR},
	{"pbuf_reference_fail"	,NSS_STATS_TYPE_ERROR}
};

/*
 * nss_lso_rx_strings_read()
 *	Read LSO Rx node statistics names.
 */
static ssize_t nss_lso_rx_strings_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	return nss_strings_print(ubuf, sz, ppos, nss_lso_rx_strings_stats, NSS_LSO_RX_STATS_MAX);
}

/*
 * nss_lso_rx_strings_ops
 */
NSS_STRINGS_DECLARE_FILE_OPERATIONS(lso_rx);

/*
 * nss_lso_rx_strings_dentry_create()
 *	Create LSO Rx statistics strings debug entry.
 */
void nss_lso_rx_strings_dentry_create(void)
{
	nss_strings_create_dentry("lso_rx", &nss_lso_rx_strings_ops);
}

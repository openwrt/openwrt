/*
 **************************************************************************
 * Copyright (c) 2020, The Linux Foundation. All rights reserved.
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

#include "nss_stats.h"
#include "nss_core.h"
#include "nss_strings.h"
#include "nss_crypto_cmn_strings.h"

/*
 * nss_crypto_cmn_strings_stats
 *	crypto common statistics strings.
 */
struct nss_stats_info nss_crypto_cmn_strings_stats[NSS_CRYPTO_CMN_STATS_MAX] = {
	{"rx_pkts",		NSS_STATS_TYPE_COMMON},
	{"rx_byts",		NSS_STATS_TYPE_COMMON},
	{"tx_pkts",		NSS_STATS_TYPE_COMMON},
	{"tx_byts",		NSS_STATS_TYPE_COMMON},
	{"rx_queue[0]_drops",	NSS_STATS_TYPE_DROP},
	{"rx_queue[1]_drops",	NSS_STATS_TYPE_DROP},
	{"rx_queue[2]_drops",	NSS_STATS_TYPE_DROP},
	{"rx_queue[3]_drops",	NSS_STATS_TYPE_DROP},
	{"fail_version",	NSS_STATS_TYPE_SPECIAL},
	{"fail_ctx",		NSS_STATS_TYPE_SPECIAL},
	{"fail_dma",		NSS_STATS_TYPE_SPECIAL}
};

/*
 * nss_crypto_cmn_strings_read()
 *	Read crypto common node statistics names
 */
static ssize_t nss_crypto_cmn_strings_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	return nss_strings_print(ubuf, sz, ppos, nss_crypto_cmn_strings_stats, NSS_CRYPTO_CMN_STATS_MAX);
}

/*
 * nss_crypto_cmn_strings_ops
 */
NSS_STRINGS_DECLARE_FILE_OPERATIONS(crypto_cmn);

/*
 * nss_crypto_cmn_strings_dentry_create()
 *	Create crypto common statistics strings debug entry.
 */
void nss_crypto_cmn_strings_dentry_create(void)
{
	nss_strings_create_dentry("crypto_cmn", &nss_crypto_cmn_strings_ops);
}

/*
 *****************************************************************************
 * Copyright (c) 2021, The Linux Foundation. All rights reserved.
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
 *****************************************************************************
 */

#include "nss_stats.h"
#include "nss_core.h"
#include "nss_strings.h"
#include "nss_qvpn_strings.h"

/*
 * nss_qvpn_strings_stats
 *	qvpn statistics strings.
 */
struct nss_stats_info nss_qvpn_strings_stats[NSS_STATS_NODE_MAX] = {
	{"rx_pkts",                     NSS_STATS_TYPE_COMMON},
	{"rx_byts",                     NSS_STATS_TYPE_COMMON},
	{"tx_pkts",                     NSS_STATS_TYPE_COMMON},
	{"tx_byts",                     NSS_STATS_TYPE_COMMON},
	{"rx_queue[0]_drops",           NSS_STATS_TYPE_DROP},
	{"rx_queue[1]_drops",           NSS_STATS_TYPE_DROP},
	{"rx_queue[2]_drops",           NSS_STATS_TYPE_DROP},
	{"rx_queue[3]_drops",           NSS_STATS_TYPE_DROP}
};

/*
 * nss_qvpn_strings_read()
 *	Read qvpn statistics names
 */
static ssize_t nss_qvpn_strings_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	return nss_strings_print(ubuf, sz, ppos, nss_qvpn_strings_stats, NSS_STATS_NODE_MAX);
}

/*
 * nss_qvpn_strings_ops
 */
NSS_STRINGS_DECLARE_FILE_OPERATIONS(qvpn);

/*
 * nss_qvpn_strings_dentry_create()
 *	Create qvpn statistics strings debug entry.
 */
void nss_qvpn_strings_dentry_create(void)
{
	nss_strings_create_dentry("qvpn", &nss_qvpn_strings_ops);
}

/*
 **************************************************************************
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
 **************************************************************************
 */

#include "nss_stats.h"
#include "nss_core.h"
#include "nss_strings.h"
#include "nss_clmap_strings.h"

/*
 * nss_clmap_strings_stats
 *	Clmap statistics strings for nss tunnel stats
 */
struct nss_stats_info nss_clmap_strings_stats[NSS_CLMAP_INTERFACE_STATS_MAX] = {
	{"rx_pkts",					NSS_STATS_TYPE_COMMON},
	{"rx_bytes",					NSS_STATS_TYPE_COMMON},
	{"tx_pkts",					NSS_STATS_TYPE_COMMON},
	{"tx_bytes",					NSS_STATS_TYPE_COMMON},
	{"rx_queue_0_dropped",				NSS_STATS_TYPE_DROP},
	{"rx_queue_1_dropped",				NSS_STATS_TYPE_DROP},
	{"rx_queue_2_dropped",				NSS_STATS_TYPE_DROP},
	{"rx_queue_3_dropped",				NSS_STATS_TYPE_DROP},
	{"MAC DB look up failed",			NSS_STATS_TYPE_SPECIAL},
	{"Invalid packet count",			NSS_STATS_TYPE_SPECIAL},
	{"Headroom drop",				NSS_STATS_TYPE_SPECIAL},
	{"Next node queue full drop",			NSS_STATS_TYPE_SPECIAL},
	{"Pbuf alloc failed drop",			NSS_STATS_TYPE_SPECIAL},
	{"Linear failed drop",				NSS_STATS_TYPE_SPECIAL},
	{"Shared packet count",				NSS_STATS_TYPE_SPECIAL},
	{"Ethernet frame error",			NSS_STATS_TYPE_SPECIAL},
	{"Macdb create requests count",			NSS_STATS_TYPE_SPECIAL},
	{"Macdb create failures MAC exists count",	NSS_STATS_TYPE_SPECIAL},
	{"Macdb create failures MAC table full count",	NSS_STATS_TYPE_SPECIAL},
	{"Macdb destroy requests count",		NSS_STATS_TYPE_SPECIAL},
	{"Macdb destroy failures MAC not found count",	NSS_STATS_TYPE_SPECIAL},
	{"Macdb destroy failures MAC unhashed count",	NSS_STATS_TYPE_SPECIAL},
	{"Macdb flush requests count",			NSS_STATS_TYPE_SPECIAL}
};

/*
 * nss_clmap_strings_read()
 *	Read clmap statistics names
 */
static ssize_t nss_clmap_strings_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	return nss_strings_print(ubuf, sz, ppos, nss_clmap_strings_stats, NSS_CLMAP_INTERFACE_STATS_MAX);
}

/*
 * nss_clmap_strings_ops
 */
NSS_STRINGS_DECLARE_FILE_OPERATIONS(clmap);

/*
 * nss_clmap_strings_dentry_create()
 *	Create clmap statistics strings debug entry.
 */
void nss_clmap_strings_dentry_create(void)
{
	nss_strings_create_dentry("clmap", &nss_clmap_strings_ops);
}

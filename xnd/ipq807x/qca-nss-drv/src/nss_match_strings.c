/*
 ***************************************************************************
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
 ***************************************************************************
 */

#include "nss_stats.h"
#include "nss_core.h"
#include "nss_strings.h"
#include "nss_match_strings.h"

/*
 * nss_match_strings_stats
 *	match statistics strings.
 */
struct nss_stats_info nss_match_strings_stats[NSS_MATCH_STATS_MAX] = {
	{"rx_pkts",		NSS_STATS_TYPE_COMMON},
	{"rx_byts",		NSS_STATS_TYPE_COMMON},
	{"tx_pkts",		NSS_STATS_TYPE_COMMON},
	{"tx_byts",		NSS_STATS_TYPE_COMMON},
	{"rx_queue[0]_drops",	NSS_STATS_TYPE_DROP},
	{"rx_queue[1]_drops",	NSS_STATS_TYPE_DROP},
	{"rx_queue[2]_drops",	NSS_STATS_TYPE_DROP},
	{"rx_queue[3]_drops",	NSS_STATS_TYPE_DROP},
	{"hit_count[0]",	NSS_STATS_TYPE_SPECIAL},
	{"hit_count[1]",	NSS_STATS_TYPE_SPECIAL},
	{"hit_count[2]",	NSS_STATS_TYPE_SPECIAL},
	{"hit_count[3]",	NSS_STATS_TYPE_SPECIAL},
	{"hit_count[4]",	NSS_STATS_TYPE_SPECIAL},
	{"hit_count[5]",	NSS_STATS_TYPE_SPECIAL},
	{"hit_count[6]",	NSS_STATS_TYPE_SPECIAL},
	{"hit_count[7]",	NSS_STATS_TYPE_SPECIAL},
	{"hit_count[8]",	NSS_STATS_TYPE_SPECIAL},
	{"hit_count[9]",	NSS_STATS_TYPE_SPECIAL},
	{"hit_count[10]",	NSS_STATS_TYPE_SPECIAL},
	{"hit_count[11]",	NSS_STATS_TYPE_SPECIAL},
	{"hit_count[12]",	NSS_STATS_TYPE_SPECIAL},
	{"hit_count[13]",	NSS_STATS_TYPE_SPECIAL},
	{"hit_count[14]",	NSS_STATS_TYPE_SPECIAL},
	{"hit_count[15]",	NSS_STATS_TYPE_SPECIAL},
	{"hit_count[16]",	NSS_STATS_TYPE_SPECIAL},
	{"hit_count[17]",	NSS_STATS_TYPE_SPECIAL},
	{"hit_count[18]",	NSS_STATS_TYPE_SPECIAL},
	{"hit_count[19]",	NSS_STATS_TYPE_SPECIAL},
	{"hit_count[20]",	NSS_STATS_TYPE_SPECIAL},
	{"hit_count[21]",	NSS_STATS_TYPE_SPECIAL},
	{"hit_count[22]",	NSS_STATS_TYPE_SPECIAL},
	{"hit_count[23]",	NSS_STATS_TYPE_SPECIAL},
	{"hit_count[24]",	NSS_STATS_TYPE_SPECIAL},
	{"hit_count[25]",	NSS_STATS_TYPE_SPECIAL},
	{"hit_count[26]",	NSS_STATS_TYPE_SPECIAL},
	{"hit_count[27]",	NSS_STATS_TYPE_SPECIAL},
	{"hit_count[28]",	NSS_STATS_TYPE_SPECIAL},
	{"hit_count[29]",	NSS_STATS_TYPE_SPECIAL},
	{"hit_count[30]",	NSS_STATS_TYPE_SPECIAL},
	{"hit_count[31]",	NSS_STATS_TYPE_SPECIAL}
};

/*
 * nss_match_stats_strings_read()
 *	Read match statistics names
 */
static ssize_t nss_match_strings_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	return nss_strings_print(ubuf, sz, ppos, nss_match_strings_stats, NSS_MATCH_STATS_MAX);
}

/*
 * nss_match_strings_ops
 */
NSS_STRINGS_DECLARE_FILE_OPERATIONS(match);

/*
 * nss_match_strings_dentry_create()
 *	Create match statistics strings debug entry.
 */
void nss_match_strings_dentry_create(void)
{
	nss_strings_create_dentry("match", &nss_match_strings_ops);
}

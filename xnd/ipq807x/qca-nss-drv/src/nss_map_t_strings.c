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
#include <nss_map_t.h>
#include "nss_strings.h"

/*
 * nss_map_t_strings_instance_stats
 *	MAP-T statistics strings for NSS session statistics.
 */
struct nss_stats_info nss_map_t_strings_instance_stats[NSS_MAP_T_STATS_MAX] = {
	{"V4_TO_V6_PBUF_EXCEPTION_PKTS",	NSS_STATS_TYPE_EXCEPTION},
	{"V4_TO_V6_PBUF_NO_MATCHING_RULE",	NSS_STATS_TYPE_SPECIAL},
	{"V4_TO_V6_PBUF_NOT_TCP_OR_UDP",	NSS_STATS_TYPE_SPECIAL},
	{"V4_TO_V6_RULE_ERR_LOCAL_PSID",	NSS_STATS_TYPE_ERROR},
	{"V4_TO_V6_RULE_ERR_LOCAL_IPV6",	NSS_STATS_TYPE_ERROR},
	{"V4_TO_V6_RULE_ERR_REMOTE_PSID",	NSS_STATS_TYPE_ERROR},
	{"V4_TO_V6_RULE_ERR_REMOTE_EA_BITS",	NSS_STATS_TYPE_ERROR},
	{"V4_TO_V6_RULE_ERR_REMOTE_IPV6",	NSS_STATS_TYPE_ERROR},
	{"V6_TO_V4_PBUF_EXCEPTION_PKTS",	NSS_STATS_TYPE_EXCEPTION},
	{"V6_TO_V4_PBUF_NO_MATCHING_RULE",	NSS_STATS_TYPE_SPECIAL},
	{"V6_TO_V4_PBUF_NOT_TCP_OR_UDP",	NSS_STATS_TYPE_SPECIAL},
	{"V6_TO_V4_RULE_ERR_LOCAL_IPV4",	NSS_STATS_TYPE_ERROR},
	{"V6_TO_V4_RULE_ERR_REMOTE_IPV4",	NSS_STATS_TYPE_ERROR}
};

/*
 * nss_map_t_strings_read()
 *	Read MAP-T node statistics names.
 */
static ssize_t nss_map_t_strings_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	return nss_strings_print(ubuf, sz, ppos, nss_map_t_strings_instance_stats, NSS_MAP_T_STATS_MAX);
}

/*
 * nss_map_t_strings_ops
 */
NSS_STRINGS_DECLARE_FILE_OPERATIONS(map_t);

/*
 * nss_map_t_strings_dentry_create()
 *	Create MAP-T statistics strings debug entry.
 */
void nss_map_t_strings_dentry_create(void)
{
	nss_strings_create_dentry("map_t", &nss_map_t_strings_ops);
}

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
#include "nss_mirror_strings.h"

/*
 * nss_mirror_strings_stats
 *	Mirror statistics strings for nss session stats.
 */
struct nss_stats_info nss_mirror_strings_stats[NSS_MIRROR_STATS_MAX] = {
	{"pkts",		NSS_STATS_TYPE_SPECIAL},
	{"bytes",		NSS_STATS_TYPE_SPECIAL},
	{"tx_fail",		NSS_STATS_TYPE_DROP},
	{"dest_lookup_fail",	NSS_STATS_TYPE_DROP},
	{"mem_alloc_fail",	NSS_STATS_TYPE_ERROR},
	{"copy_fail",		NSS_STATS_TYPE_ERROR},
};

/*
 * nss_mirror_strings_read()
 *	Read mirror statistics names
 */
static ssize_t nss_mirror_strings_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	return nss_strings_print(ubuf, sz, ppos, nss_mirror_strings_stats, NSS_MIRROR_STATS_MAX);
}

/*
 * nss_mirror_strings_ops
 */
NSS_STRINGS_DECLARE_FILE_OPERATIONS(mirror);

/*
 * nss_mirror_strings_dentry_create()
 *	Create mirror statistics strings debug entry.
 */
void nss_mirror_strings_dentry_create(void)
{
	nss_strings_create_dentry("mirror", &nss_mirror_strings_ops);
}

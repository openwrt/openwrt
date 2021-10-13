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
#include "nss_ipsec_cmn_strings.h"

/*
 * nss_ipsec_cmn_strings_stats
 *	ipsec common statistics strings.
 */
struct nss_stats_info nss_ipsec_cmn_strings_stats[NSS_IPSEC_CMN_STATS_MAX] = {
	{"rx_pkts",			NSS_STATS_TYPE_COMMON},
	{"rx_byts",			NSS_STATS_TYPE_COMMON},
	{"tx_pkts",			NSS_STATS_TYPE_COMMON},
	{"tx_byts",			NSS_STATS_TYPE_COMMON},
	{"rx_queue[0]_drops",		NSS_STATS_TYPE_DROP},
	{"rx_queue[1]_drops",		NSS_STATS_TYPE_DROP},
	{"rx_queue[2]_drops",		NSS_STATS_TYPE_DROP},
	{"rx_queue[3]_drops",		NSS_STATS_TYPE_DROP},
	{"fail_headroom",		NSS_STATS_TYPE_DROP},
	{"fail_tailroom",		NSS_STATS_TYPE_DROP},
	{"fail_replay",			NSS_STATS_TYPE_DROP},
	{"fail_replay_dup",		NSS_STATS_TYPE_DROP},
	{"fail_replay_win",		NSS_STATS_TYPE_DROP},
	{"fail_pbuf_crypto",		NSS_STATS_TYPE_DROP},
	{"fail_queue",			NSS_STATS_TYPE_DROP},
	{"fail_queue_crypto",		NSS_STATS_TYPE_DROP},
	{"fail_queue_nexthop",		NSS_STATS_TYPE_DROP},
	{"fail_pbuf_alloc",		NSS_STATS_TYPE_DROP},
	{"fail_pbuf_linear",		NSS_STATS_TYPE_DROP},
	{"fail_pbuf_stats",		NSS_STATS_TYPE_DROP},
	{"fail_pbuf_align",		NSS_STATS_TYPE_DROP},
	{"fail_cipher",			NSS_STATS_TYPE_EXCEPTION},
	{"fail_auth",			NSS_STATS_TYPE_EXCEPTION},
	{"fail_seq_ovf",		NSS_STATS_TYPE_DROP},
	{"fail_blk_len",		NSS_STATS_TYPE_DROP},
	{"fail_hash_len",		NSS_STATS_TYPE_DROP},
	{"fail_transform",		NSS_STATS_TYPE_DROP},
	{"fail_crypto",			NSS_STATS_TYPE_DROP},
	{"fail_cle",			NSS_STATS_TYPE_DROP},
	{"is_stopped",			NSS_STATS_TYPE_SPECIAL}
};

/*
 * nss_ipsec_cmn_strings_read()
 *	Read ipsec common statistics names
 */
static ssize_t nss_ipsec_cmn_strings_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	return nss_strings_print(ubuf, sz, ppos, nss_ipsec_cmn_strings_stats, NSS_IPSEC_CMN_STATS_MAX);
}

/*
 * nss_ipsec_cmn_strings_ops
 */
NSS_STRINGS_DECLARE_FILE_OPERATIONS(ipsec_cmn);

/*
 * nss_ipsec_cmn_strings_dentry_create()
 *	Create ipsec common statistics strings debug entry.
 */
void nss_ipsec_cmn_strings_dentry_create(void)
{
	nss_strings_create_dentry("ipsec_cmn", &nss_ipsec_cmn_strings_ops);
}

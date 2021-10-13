/*
 ****************************************************************************
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
 ****************************************************************************
 */

#include "nss_stats.h"
#include "nss_core.h"
#include "nss_strings.h"
#include "nss_gre_tunnel_strings.h"

/*
 * nss_gre_tunnel_strings_stats
 *	GRE Tunnel statistics strings for nss session stats
 */
struct nss_stats_info nss_gre_tunnel_strings_stats[NSS_GRE_TUNNEL_STATS_SESSION_MAX] = {
	{"rx_pkts",			NSS_STATS_TYPE_COMMON},
	{"tx_pkts",			NSS_STATS_TYPE_COMMON},
	{"rx_queue_0_dropped",		NSS_STATS_TYPE_DROP},
	{"rx_queue_1_dropped",		NSS_STATS_TYPE_DROP},
	{"rx_queue_2_dropped",		NSS_STATS_TYPE_DROP},
	{"rx_queue_3_dropped",		NSS_STATS_TYPE_DROP},
	{"rx_malformed",		NSS_STATS_TYPE_SPECIAL},
	{"rx_invalid_prot",		NSS_STATS_TYPE_SPECIAL},
	{"decap_queue_full",		NSS_STATS_TYPE_SPECIAL},
	{"rx_single_rec_dgram",		NSS_STATS_TYPE_SPECIAL},
	{"rx_invalid_rec_dgram",	NSS_STATS_TYPE_SPECIAL},
	{"buffer_alloc_fail",		NSS_STATS_TYPE_SPECIAL},
	{"buffer_copy_fail",		NSS_STATS_TYPE_SPECIAL},
	{"outflow_queue_full",		NSS_STATS_TYPE_SPECIAL},
	{"tx_dropped_hroom",		NSS_STATS_TYPE_DROP},
	{"rx_cbuffer_alloc_fail",	NSS_STATS_TYPE_SPECIAL},
	{"rx_cenqueue_fail",		NSS_STATS_TYPE_SPECIAL},
	{"rx_decrypt_done",		NSS_STATS_TYPE_SPECIAL},
	{"rx_forward_enqueue_fail",	NSS_STATS_TYPE_SPECIAL},
	{"tx_cbuffer_alloc_fail",	NSS_STATS_TYPE_SPECIAL},
	{"tx_cenqueue_fail",		NSS_STATS_TYPE_SPECIAL},
	{"rx_dropped_troom",		NSS_STATS_TYPE_DROP},
	{"tx_forward_enqueue_fail",	NSS_STATS_TYPE_SPECIAL},
	{"tx_cipher_done",		NSS_STATS_TYPE_SPECIAL},
	{"crypto_nosupp",		NSS_STATS_TYPE_SPECIAL},
	{"rx_dropped_mh_version",	NSS_STATS_TYPE_SPECIAL},
	{"rx_unaligned_pkt",		NSS_STATS_TYPE_SPECIAL}
};

/*
 * nss_gre_tunnel_strings_read()
 *	Read gre_tunnel session debug statistics names
 */
static ssize_t nss_gre_tunnel_strings_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	return nss_strings_print(ubuf, sz, ppos, nss_gre_tunnel_strings_stats, NSS_GRE_TUNNEL_STATS_SESSION_MAX);
}

/*
 * nss_gre_tunnel_strings_ops
 */
NSS_STRINGS_DECLARE_FILE_OPERATIONS(gre_tunnel);

/*
 * nss_gre_tunnel_strings_dentry_create()
 *	Create gre_tunnel statistics strings debug entry.
 */
void nss_gre_tunnel_strings_dentry_create(void)
{
	nss_strings_create_dentry("gre_tunnel", &nss_gre_tunnel_strings_ops);
}

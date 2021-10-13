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
#include <nss_pptp.h>
#include "nss_strings.h"

/*
 * nss_pptp_strings_session_debug_stats
 *	PPTP statistics strings for NSS session statistics.
 */
struct nss_stats_info nss_pptp_strings_session_debug_stats[NSS_PPTP_STATS_SESSION_MAX] = {
	{"ENCAP_RX_PACKETS",		NSS_STATS_TYPE_COMMON},
	{"ENCAP_RX_BYTES",		NSS_STATS_TYPE_COMMON},
	{"ENCAP_TX_PACKETS",		NSS_STATS_TYPE_COMMON},
	{"ENCAP_TX_BYTES",		NSS_STATS_TYPE_COMMON},
	{"ENCAP_RX_QUEUE_0_DROP",	NSS_STATS_TYPE_DROP},
	{"ENCAP_RX_QUEUE_1_DROP",	NSS_STATS_TYPE_DROP},
	{"ENCAP_RX_QUEUE_2_DROP",	NSS_STATS_TYPE_DROP},
	{"ENCAP_RX_QUEUE_3_DROP",	NSS_STATS_TYPE_DROP},
	{"DECAP_RX_PACKETS",		NSS_STATS_TYPE_COMMON},
	{"DECAP_RX_BYTES",		NSS_STATS_TYPE_COMMON},
	{"DECAP_TX_PACKETS",		NSS_STATS_TYPE_COMMON},
	{"DECAP_TX_BYTES",		NSS_STATS_TYPE_COMMON},
	{"DECAP_RX_QUEUE_0_DROP",	NSS_STATS_TYPE_DROP},
	{"DECAP_RX_QUEUE_1_DROP",	NSS_STATS_TYPE_DROP},
	{"DECAP_RX_QUEUE_2_DROP",	NSS_STATS_TYPE_DROP},
	{"DECAP_RX_QUEUE_3_DROP",	NSS_STATS_TYPE_DROP},
	{"ENCAP_HEADROOM_ERR",		NSS_STATS_TYPE_ERROR},
	{"ENCAP_SMALL_SIZE",		NSS_STATS_TYPE_SPECIAL},
	{"ENCAP_PNODE_ENQUEUE_FAIL",	NSS_STATS_TYPE_ERROR},
	{"DECAP_NO_SEQ_NOR_ACK",	NSS_STATS_TYPE_ERROR},
	{"DECAP_INVAL_GRE_FLAGS",	NSS_STATS_TYPE_ERROR},
	{"DECAP_INVAL_GRE_PROTO",	NSS_STATS_TYPE_ERROR},
	{"DECAP_WRONG_SEQ",		NSS_STATS_TYPE_ERROR},
	{"DECAP_INVAL_PPP_HDR",		NSS_STATS_TYPE_ERROR},
	{"DECAP_PPP_LCP",		NSS_STATS_TYPE_SPECIAL},
	{"DECAP_UNSUPPORTED_PPP_PROTO",	NSS_STATS_TYPE_ERROR},
	{"DECAP_PNODE_ENQUEUE_FAIL",	NSS_STATS_TYPE_ERROR}
};

/*
 * nss_pptp_strings_read()
 *	Read PPTP node statistics names.
 */
static ssize_t nss_pptp_strings_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	return nss_strings_print(ubuf, sz, ppos, nss_pptp_strings_session_debug_stats, NSS_PPTP_STATS_SESSION_MAX);
}

/*
 * nss_pptp_strings_ops
 */
NSS_STRINGS_DECLARE_FILE_OPERATIONS(pptp);

/*
 * nss_pptp_strings_dentry_create()
 *	Create PPTP statistics strings debug entry.
 */
void nss_pptp_strings_dentry_create(void)
{
	nss_strings_create_dentry("pptp", &nss_pptp_strings_ops);
}

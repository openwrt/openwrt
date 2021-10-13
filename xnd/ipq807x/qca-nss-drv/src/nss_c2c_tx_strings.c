/*
 **************************************************************************
 * Copyright (c) 2019-2020, The Linux Foundation. All rights reserved.
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

/*
 * nss_c2c_tx_strings_stats
 *	C2C Tx statistics strings.
 */
struct nss_stats_info nss_c2c_tx_strings_stats[NSS_C2C_TX_STATS_MAX] = {
	{"rx_pkts"		, NSS_STATS_TYPE_COMMON},
	{"rx_byts"		, NSS_STATS_TYPE_COMMON},
	{"tx_pkts"		, NSS_STATS_TYPE_COMMON},
	{"tx_byts"		, NSS_STATS_TYPE_COMMON},
	{"rx_queue[0]_drops"	, NSS_STATS_TYPE_DROP},
	{"rx_queue[1]_drops"	, NSS_STATS_TYPE_DROP},
	{"rx_queue[2]_drops"	, NSS_STATS_TYPE_DROP},
	{"rx_queue[3]_drops"	, NSS_STATS_TYPE_DROP},
	{"pbuf_simple"		, NSS_STATS_TYPE_SPECIAL},
	{"pbuf_sg"		, NSS_STATS_TYPE_SPECIAL},
	{"pbuf_returning"	, NSS_STATS_TYPE_SPECIAL}
};


/*
 * nss_c2c_tx_strings_read()
 *	Read c2c Tx node statistics names
 */
static ssize_t nss_c2c_tx_strings_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	return nss_strings_print(ubuf, sz, ppos, nss_c2c_tx_strings_stats, NSS_C2C_TX_STATS_MAX);
}

/*
 * nss_c2c_tx_strings_ops
 */
NSS_STRINGS_DECLARE_FILE_OPERATIONS(c2c_tx);

/*
 * nss_c2c_tx_strings_dentry_create()
 *	Create C2C Tx statistics strings debug entry.
 */
void nss_c2c_tx_strings_dentry_create(void)
{
	nss_strings_create_dentry("c2c_tx", &nss_c2c_tx_strings_ops);
}

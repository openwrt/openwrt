/*
 **************************************************************************
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
 **************************************************************************
 */

#include "nss_stats.h"
#include "nss_core.h"
#include <nss_udp_st.h>
#include "nss_strings.h"

/*
 * nss_udp_st_strings_error_stats
 *	Statistics strings for udp_st errors.
 */
struct nss_stats_info nss_udp_st_strings_error_stats[NSS_UDP_ST_ERROR_MAX] = {
	{"error_none"				, NSS_STATS_TYPE_SPECIAL},
	{"incorrect_rate"			, NSS_STATS_TYPE_DROP},
	{"incorrect_buffer_size"		, NSS_STATS_TYPE_DROP},
	{"memory_failure"			, NSS_STATS_TYPE_DROP},
	{"incorrect_state"			, NSS_STATS_TYPE_DROP},
	{"incorrect_flags"			, NSS_STATS_TYPE_DROP},
	{"entry_exist"				, NSS_STATS_TYPE_DROP},
	{"entry_add_failed"			, NSS_STATS_TYPE_DROP},
	{"entry_not_exist"			, NSS_STATS_TYPE_DROP},
	{"wrong_start_msg_type"			, NSS_STATS_TYPE_DROP},
	{"wrong_stop_msg_type"			, NSS_STATS_TYPE_DROP},
	{"too_many_users"			, NSS_STATS_TYPE_DROP},
	{"unknown_msg_type"			, NSS_STATS_TYPE_DROP},
	{"pb_alloc_failure"			, NSS_STATS_TYPE_DROP},
	{"pb_size_failure"			, NSS_STATS_TYPE_DROP},
	{"drop_queue_failure"			, NSS_STATS_TYPE_DROP},
	{"timer call is missed"			, NSS_STATS_TYPE_SPECIAL},
};

/*
 * nss_udp_st_strings_rx_time_stats
 *	Statistics strings for Rx udp_st time.
 */
struct nss_stats_info nss_udp_st_strings_rx_time_stats[NSS_UDP_ST_STATS_TIME_MAX] = {
	{"rx_start_time"		, NSS_STATS_TYPE_SPECIAL},
	{"rx_current_time"		, NSS_STATS_TYPE_SPECIAL},
	{"rx_elapsed_time"		, NSS_STATS_TYPE_SPECIAL}
};

/*
 * nss_udp_st_strings_tx_time_stats
 *	Statistics strings for Tx udp_st time.
 */
struct nss_stats_info nss_udp_st_strings_tx_time_stats[NSS_UDP_ST_STATS_TIME_MAX] = {
	{"tx_start_time"		, NSS_STATS_TYPE_SPECIAL},
	{"tx_current_time"		, NSS_STATS_TYPE_SPECIAL},
	{"tx_elapsed_time"			, NSS_STATS_TYPE_SPECIAL}
};

/*
 * nss_udp_st_error_stats_strings_read()
 *	Read udp_st error statistics names.
 */
static ssize_t nss_udp_st_error_stats_strings_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	return nss_strings_print(ubuf, sz, ppos, nss_udp_st_strings_error_stats, NSS_UDP_ST_ERROR_MAX);
}

/*
 * nss_udp_st_rx_time_stats_strings_read()
 *	Read Rx udp_st time statistics names.
 */
static ssize_t nss_udp_st_rx_time_stats_strings_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	return nss_strings_print(ubuf, sz, ppos, nss_udp_st_strings_rx_time_stats, NSS_UDP_ST_STATS_TIME_MAX);
}

/*
 * nss_udp_st_tx_time_stats_strings_read()
 *	Read Tx udp_st time statistics names.
 */
static ssize_t nss_udp_st_tx_time_stats_strings_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	return nss_strings_print(ubuf, sz, ppos, nss_udp_st_strings_tx_time_stats, NSS_UDP_ST_STATS_TIME_MAX);
}

/*
 * nss_udp_st_error_stats_strings_ops
 */
NSS_STRINGS_DECLARE_FILE_OPERATIONS(udp_st_error_stats);

/*
 * nss_udp_st_rx_time_stats_strings_ops
 */
NSS_STRINGS_DECLARE_FILE_OPERATIONS(udp_st_rx_time_stats);

/*
 * nss_udp_st_tx_time_stats_strings_ops
 */
NSS_STRINGS_DECLARE_FILE_OPERATIONS(udp_st_tx_time_stats);

/*
 * nss_udp_st_strings_dentry_create()
 *      Create udp_st statistics strings debug entry.
 */
void nss_udp_st_strings_dentry_create(void)
{
	struct dentry *dir_d;
	struct dentry *file_d;

	if (!nss_top_main.strings_dentry) {
		nss_warning("qca-nss-drv/strings is not present");
		return;
	}

	dir_d = debugfs_create_dir("udp_st", nss_top_main.strings_dentry);
	if (!dir_d) {
		nss_warning("Failed to create qca-nss-drv/strings/udp_st directory");
		return;
	}

	file_d = debugfs_create_file("error_stats_str", 0400, dir_d, &nss_top_main, &nss_udp_st_error_stats_strings_ops);
	if (!file_d) {
		nss_warning("Failed to create qca-nss-drv/stats/udp_st/error_stats_str file");
		goto fail;
	}

	file_d = debugfs_create_file("rx_time_stats_str", 0400, dir_d, &nss_top_main, &nss_udp_st_rx_time_stats_strings_ops);
	if (!file_d) {
		nss_warning("Failed to create qca-nss-drv/stats/udp_st/rx_time_stats_str file");
		goto fail;
	}

	file_d = debugfs_create_file("tx_time_stats_str", 0400, dir_d, &nss_top_main, &nss_udp_st_tx_time_stats_strings_ops);
	if (!file_d) {
		nss_warning("Failed to create qca-nss-drv/stats/udp_st/tx_time_stats_str file");
		goto fail;
	}

	return;
fail:
	debugfs_remove_recursive(dir_d);
}

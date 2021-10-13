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
#include <nss_wifi_mesh.h>
#include "nss_wifi_mesh_stats.h"
#include "nss_strings.h"
#include "nss_wifi_mesh_strings.h"

/*
 * nss_wifi_mesh_strings_encap_stats
 *	Wi-Fi mesh encap statistics string.
 */
struct nss_stats_info nss_wifi_mesh_strings_encap_stats[NSS_WIFI_MESH_ENCAP_STATS_TYPE_MAX] = {
	{"rx_packets",			NSS_STATS_TYPE_COMMON},
	{"rx_bytes",			NSS_STATS_TYPE_COMMON},
	{"tx_packets",			NSS_STATS_TYPE_COMMON},
	{"tx_bytes",			NSS_STATS_TYPE_COMMON},
	{"rx_dropped",			NSS_STATS_TYPE_COMMON},
	{"expiry_notify_sent",		NSS_STATS_TYPE_SPECIAL},
	{"mc_count",			NSS_STATS_TYPE_SPECIAL},
	{"mp_not_found",		NSS_STATS_TYPE_SPECIAL},
	{"mp_active",			NSS_STATS_TYPE_SPECIAL},
	{"mpp_not_found",		NSS_STATS_TYPE_SPECIAL},
	{"mpp_found",			NSS_STATS_TYPE_SPECIAL},
	{"encap_hdr_fail",		NSS_STATS_TYPE_SPECIAL},
	{"mp_del_notify_fail",		NSS_STATS_TYPE_SPECIAL},
	{"link_enqueue",		NSS_STATS_TYPE_SPECIAL},
	{"link_enq_fail",		NSS_STATS_TYPE_SPECIAL},
	{"ra_lup_fail",			NSS_STATS_TYPE_SPECIAL},
	{"dummy_add_count",		NSS_STATS_TYPE_SPECIAL},
	{"encap_mp_add_notify_fail", 	NSS_STATS_TYPE_SPECIAL},
	{"dummy_add_fail", 		NSS_STATS_TYPE_SPECIAL},
	{"dummy_lup_fail", 		NSS_STATS_TYPE_SPECIAL},
	{"send_to_host_failed",		NSS_STATS_TYPE_SPECIAL},
	{"sent_to_host",		NSS_STATS_TYPE_SPECIAL},
	{"expiry_notify_fail",		NSS_STATS_TYPE_SPECIAL},
	{"no_headroom", 		NSS_STATS_TYPE_SPECIAL},
	{"path_refresh_sent", 		NSS_STATS_TYPE_SPECIAL},
	{"linearise_failed", 		NSS_STATS_TYPE_SPECIAL}
};

/*
 * nss_wifi_mesh_encap_strings_read()
 *	Read Wi-Fi mesh encap statistics names.
 */
static ssize_t nss_wifi_mesh_encap_stats_strings_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	return nss_strings_print(ubuf, sz, ppos, nss_wifi_mesh_strings_encap_stats, NSS_WIFI_MESH_ENCAP_STATS_TYPE_MAX);
}

/*
 * nss_wifi_mesh_strings_path_stats
 * 	Wi-Fi mesh path statistics string.
 */
struct nss_stats_info nss_wifi_mesh_strings_path_stats[NSS_WIFI_MESH_PATH_STATS_TYPE_MAX] = {
	{"alloc_failures",		NSS_STATS_TYPE_SPECIAL},
	{"error_max_radio_count",	NSS_STATS_TYPE_SPECIAL},
	{"invalid_interface_failures",	NSS_STATS_TYPE_SPECIAL},
	{"add_success",			NSS_STATS_TYPE_SPECIAL},
	{"table_full_errors",		NSS_STATS_TYPE_SPECIAL},
	{"insert_failures",		NSS_STATS_TYPE_SPECIAL},
	{"not_found",			NSS_STATS_TYPE_SPECIAL},
	{"delete_success",		NSS_STATS_TYPE_SPECIAL},
	{"update_success",		NSS_STATS_TYPE_SPECIAL},
	{"mesh_path_expired",		NSS_STATS_TYPE_SPECIAL},
	{"mesh_path_refresh_needed",	NSS_STATS_TYPE_SPECIAL},
	{"add_requests",		NSS_STATS_TYPE_SPECIAL},
	{"del_requests",		NSS_STATS_TYPE_SPECIAL},
	{"update_requests",		NSS_STATS_TYPE_SPECIAL},
	{"next_hop_updations",		NSS_STATS_TYPE_SPECIAL},
	{"hop_count_updations",		NSS_STATS_TYPE_SPECIAL},
	{"flag_updations",		NSS_STATS_TYPE_SPECIAL},
	{"metric_updations",		NSS_STATS_TYPE_SPECIAL},
	{"block_mesh_fwd_updations",	NSS_STATS_TYPE_SPECIAL},
	{"delete_failures",		NSS_STATS_TYPE_SPECIAL}
};

/*
 * nss_wifi_mesh_path_strings_read()
 *	Read Wi-Fi mesh path statistics names.
 */
static ssize_t nss_wifi_mesh_path_stats_strings_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	return nss_strings_print(ubuf, sz, ppos, nss_wifi_mesh_strings_path_stats, NSS_WIFI_MESH_PATH_STATS_TYPE_MAX);
}

/*
 * nss_wifi_mesh_strings_proxy_path_stats
 * 	Wi-Fi mesh proxy path statistics string.
 */
struct nss_stats_info nss_wifi_mesh_strings_proxy_path_stats[NSS_WIFI_MESH_PROXY_PATH_STATS_TYPE_MAX] = {
	{"alloc_failures",		NSS_STATS_TYPE_SPECIAL},
	{"entry_exist_failures",	NSS_STATS_TYPE_SPECIAL},
	{"add_success",			NSS_STATS_TYPE_SPECIAL},
	{"table_full_errors",		NSS_STATS_TYPE_SPECIAL},
	{"insert_failures",		NSS_STATS_TYPE_SPECIAL},
	{"not_found",			NSS_STATS_TYPE_SPECIAL},
	{"unhashed_errors",		NSS_STATS_TYPE_SPECIAL},
	{"delete_failures",		NSS_STATS_TYPE_SPECIAL},
	{"delete_success",		NSS_STATS_TYPE_SPECIAL},
	{"update_success",		NSS_STATS_TYPE_SPECIAL},
	{"lookup_success",		NSS_STATS_TYPE_SPECIAL},
	{"add_requests",		NSS_STATS_TYPE_SPECIAL},
	{"del_requests",		NSS_STATS_TYPE_SPECIAL},
	{"update_requests",		NSS_STATS_TYPE_SPECIAL},
	{"mda_updations",		NSS_STATS_TYPE_SPECIAL},
	{"flag_updations",		NSS_STATS_TYPE_SPECIAL}
};

/*
 * nss_wifi_mesh_proxy_path_strings_read()
 *	Read Wi-Fi mesh proxy path statistics names.
 */
static ssize_t nss_wifi_mesh_proxy_path_stats_strings_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	return nss_strings_print(ubuf, sz, ppos, nss_wifi_mesh_strings_proxy_path_stats, NSS_WIFI_MESH_PROXY_PATH_STATS_TYPE_MAX);
}

/*
 * nss_wifi_mesh_strings_decap_stats
 *	Wi-Fi mesh decap statistics string.
 */
struct nss_stats_info nss_wifi_mesh_strings_decap_stats[NSS_WIFI_MESH_DECAP_STATS_TYPE_MAX] = {
	{"rx_packets",			NSS_STATS_TYPE_COMMON},
	{"rx_bytes",			NSS_STATS_TYPE_COMMON},
	{"tx_packets",			NSS_STATS_TYPE_COMMON},
	{"tx_bytes",			NSS_STATS_TYPE_COMMON},
	{"rx_dropped",			NSS_STATS_TYPE_COMMON},
	{"path_refresh_sent",		NSS_STATS_TYPE_SPECIAL},
	{"reserved",			NSS_STATS_TYPE_SPECIAL},
	{"mc_drop",			NSS_STATS_TYPE_DROP},
	{"ttl_0",			NSS_STATS_TYPE_SPECIAL},
	{"mpp_lup_fail",		NSS_STATS_TYPE_SPECIAL},
	{"decap_hdr_fail",		NSS_STATS_TYPE_SPECIAL},
	{"rx_fwd_fail",			NSS_STATS_TYPE_SPECIAL},
	{"rx_fwd_success",		NSS_STATS_TYPE_SPECIAL},
	{"mp_fwd_lookup_fail",		NSS_STATS_TYPE_SPECIAL},
	{"mp_fwd_inactive",		NSS_STATS_TYPE_SPECIAL},
	{"nxt_mnode_fwd_success",	NSS_STATS_TYPE_SPECIAL},
	{"nxt_mnode_fwd_fail",		NSS_STATS_TYPE_SPECIAL},
	{"mpp_add_fail",		NSS_STATS_TYPE_SPECIAL},
	{"mpp_add_event2host_fail",	NSS_STATS_TYPE_SPECIAL},
	{"mpp_upate_fail",		NSS_STATS_TYPE_SPECIAL},
	{"mpp_update_even2host_fail", 	NSS_STATS_TYPE_SPECIAL},
	{"mpp_learn2host_fail",		NSS_STATS_TYPE_SPECIAL},
	{"block_mesh_fwd_packets",	NSS_STATS_TYPE_SPECIAL},
	{"no_headroom",			NSS_STATS_TYPE_SPECIAL},
	{"linearise_failed",		NSS_STATS_TYPE_SPECIAL},
	{"mpp_learn_event_rl_dropped",	NSS_STATS_TYPE_DROP},
	{"mp_missging_event_rl_dropped", NSS_STATS_TYPE_DROP}
};

/*
 * nss_wifi_mesh_decap_strings_read()
 *	Read Wi-Fi mesh decap statistics names.
 */
static ssize_t nss_wifi_mesh_decap_stats_strings_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	return nss_strings_print(ubuf, sz, ppos, nss_wifi_mesh_strings_decap_stats, NSS_WIFI_MESH_DECAP_STATS_TYPE_MAX);
}

/*
 * nss_wifi_mesh_strings_exception_stats
 *	Wi-Fi mesh exception statistics string.
 */
struct nss_stats_info nss_wifi_mesh_strings_exception_stats[NSS_WIFI_MESH_EXCEPTION_STATS_TYPE_MAX] = {
	{"packets_success",		NSS_STATS_TYPE_SPECIAL},
	{"packets_failure",		NSS_STATS_TYPE_DROP}
};

/*
 * nss_wifi_mesh_exception_strings_read()
 *	Read Wi-Fi mesh exception statistics names.
 */
static ssize_t nss_wifi_mesh_exception_stats_strings_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	return nss_strings_print(ubuf, sz, ppos, nss_wifi_mesh_strings_exception_stats, NSS_WIFI_MESH_EXCEPTION_STATS_TYPE_MAX);
}

/*
 * nss_wifi_mesh_decap_strings_ops
 */
NSS_STRINGS_DECLARE_FILE_OPERATIONS(wifi_mesh_decap_stats);

/*
 * nss_wifi_mesh_encap_strings_ops
 */
NSS_STRINGS_DECLARE_FILE_OPERATIONS(wifi_mesh_encap_stats);

/*
 * nss_wifi_mesh_path_strings_ops
 */
NSS_STRINGS_DECLARE_FILE_OPERATIONS(wifi_mesh_path_stats);

/*
 * nss_wifi_mesh_proxy_path_strings_ops
 */
NSS_STRINGS_DECLARE_FILE_OPERATIONS(wifi_mesh_proxy_path_stats);

/*
 * nss_wifi_mesh_exception_strings_ops
 */
NSS_STRINGS_DECLARE_FILE_OPERATIONS(wifi_mesh_exception_stats);

/*
 * nss_wifi_mesh_strings_dentry_create()
 *	Create Wi-Fi mesh statistics strings debug entry.
 */
struct dentry *nss_wifi_mesh_strings_dentry_create(void)
{
	struct dentry *str_dentry_dir;
	struct dentry *str_file;

	if (!nss_top_main.strings_dentry) {
		nss_warning("qca-nss-drv/strings is not present\n");
		return NULL;
	}

	str_dentry_dir = debugfs_create_dir("wifi_mesh", nss_top_main.strings_dentry);
	if (!str_dentry_dir) {
		nss_warning("Failed to create qca-nss-drv/string/wifi_mesh directory\n");
		return NULL;
	}

	str_file = debugfs_create_file("encap_stats", 0400, str_dentry_dir, &nss_top_main, &nss_wifi_mesh_encap_stats_strings_ops);
	if (!str_file) {
		nss_warning("Failed to create qca-nss-drv/string/wifi_mesh/encap_stats file\n");
		goto fail;
	}

	str_file = debugfs_create_file("decap_stats", 0400, str_dentry_dir, &nss_top_main, &nss_wifi_mesh_decap_stats_strings_ops);
	if (!str_file) {
		nss_warning("Failed to create qca-nss-drv/string/wifi_mesh/decap_stats file\n");
		goto fail;
	}

	str_file = debugfs_create_file("path_stats", 0400, str_dentry_dir, &nss_top_main, &nss_wifi_mesh_path_stats_strings_ops);
	if (!str_file) {
		nss_warning("Failed to create qca-nss-drv/string/wifi_mesh/path_stats file\n");
		goto fail;
	}

	str_file = debugfs_create_file("proxy_path_stats", 0400, str_dentry_dir, &nss_top_main, &nss_wifi_mesh_proxy_path_stats_strings_ops);
	if (!str_file) {
		nss_warning("Failed to create qca-nss-drv/string/wifi_mesh/proxy_path_stats file\n");
		goto fail;
	}

	str_file = debugfs_create_file("exception_stats", 0400, str_dentry_dir, &nss_top_main, &nss_wifi_mesh_exception_stats_strings_ops);
	if (!str_file) {
		nss_warning("Failed to create qca-nss-drv/string/wifi_mesh/exception_stats file\n");
		goto fail;
	}

	return str_dentry_dir;
fail:
	debugfs_remove_recursive(str_dentry_dir);
	return NULL;
}

/*
 **************************************************************************
 * Copyright (c) 2021, The Linux Foundation. All rights reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 **************************************************************************
 */

#include "nss_core.h"
#include "nss_tx_rx_common.h"
#include "nss_wifi_mesh.h"
#include "nss_wifi_mesh_stats.h"
#include "nss_wifi_mesh_strings.h"

#define NSS_WIFI_MESH_OUTER_STATS 0
#define NSS_WIFI_MESH_INNER_STATS 1
#define NSS_WIFI_MESH_PATH_STATS 2
#define NSS_WIFI_MESH_PROXY_PATH_STATS 3
#define NSS_WIFI_MESH_EXCEPTION_STATS 4

/*
 * Wi-Fi mesh stats dentry file size.
 */
#define NSS_WIFI_MESH_DENTRY_FILE_SIZE 19

/*
 * Spinlock for protecting tunnel operations colliding with a tunnel destroy
 */
static DEFINE_SPINLOCK(nss_wifi_mesh_stats_lock);

/*
 * Declare atomic notifier data structure for statistics.
 */
static ATOMIC_NOTIFIER_HEAD(nss_wifi_mesh_stats_notifier);

/*
 * Declare an array of Wi-Fi mesh stats handle.
 */
struct nss_wifi_mesh_stats_handle *nss_wifi_mesh_stats_hdl[NSS_WIFI_MESH_MAX_DYNAMIC_INTERFACE];

/*
 * nss_wifi_mesh_max_statistics()
 * 	Wi-Fi mesh maximum statistics.
 */
static uint32_t nss_wifi_mesh_max_statistics(void)
{
	uint32_t max1;
	uint32_t exception_stats_max = NSS_WIFI_MESH_EXCEPTION_STATS_TYPE_MAX;
	uint32_t encap_stats_max = NSS_WIFI_MESH_ENCAP_STATS_TYPE_MAX;
	uint32_t decap_stats_max = NSS_WIFI_MESH_DECAP_STATS_TYPE_MAX;
	uint32_t path_stats_max = NSS_WIFI_MESH_PATH_STATS_TYPE_MAX;
	uint32_t proxy_path_stats_max = NSS_WIFI_MESH_PROXY_PATH_STATS_TYPE_MAX;

	max1 = max(max(encap_stats_max, decap_stats_max), max(path_stats_max, proxy_path_stats_max));

	return (max(max1, exception_stats_max));
}

/*
 * nss_wifi_mesh_stats_handle_alloc()
 *	Allocate Wi-Fi mesh tunnel instance
 */
bool nss_wifi_mesh_stats_handle_alloc(nss_if_num_t if_num, int32_t ifindex)
{
	struct nss_wifi_mesh_stats_handle *h;
	uint32_t idx;

	/*
	 * Allocate a handle
	 */
	h = kzalloc(sizeof(struct nss_wifi_mesh_stats_handle), GFP_ATOMIC);
	if (!h) {
		nss_warning("Failed to allocate memory for Wi-Fi mesh instance for interface : 0x%x\n", if_num);
		return false;
	}

	spin_lock(&nss_wifi_mesh_stats_lock);
	for (idx = 0; idx < NSS_WIFI_MESH_MAX_DYNAMIC_INTERFACE; idx++) {
		if (nss_wifi_mesh_stats_hdl[idx] && nss_wifi_mesh_stats_hdl[idx]->if_num == if_num) {
			spin_unlock(&nss_wifi_mesh_stats_lock);
			nss_warning("Already a handle present for this interface number: 0x%x\n", if_num);
			kfree(h);
			return false;
		}
	}

	for (idx = 0; idx < NSS_WIFI_MESH_MAX_DYNAMIC_INTERFACE; idx++) {
		if (nss_wifi_mesh_stats_hdl[idx]) {
			continue;
		}

		h->if_num = if_num;
		h->mesh_idx = idx;
		h->ifindex = ifindex;
		nss_wifi_mesh_stats_hdl[idx] = h;
		spin_unlock(&nss_wifi_mesh_stats_lock);
		return true;
	}
	spin_unlock(&nss_wifi_mesh_stats_lock);
	nss_warning("No free index available for handle with ifnum: 0x%x\n", if_num);
	kfree(h);
	return false;
}

/*
 * nss_wifi_mesh_stats_handle_free()
 *	Free Wi-Fi mesh tunnel handle instance.
 */
bool nss_wifi_mesh_stats_handle_free(nss_if_num_t if_num)
{
	struct nss_wifi_mesh_stats_handle *h;

	spin_lock(&nss_wifi_mesh_stats_lock);
	h = nss_wifi_mesh_get_stats_handle(if_num);
	if (!h) {
		spin_unlock(&nss_wifi_mesh_stats_lock);
		nss_warning("Unable to free Wi-Fi mesh stats handle instance for interface number: 0x%x\n", if_num);
		return false;
	}

	nss_wifi_mesh_stats_hdl[h->mesh_idx] = NULL;
	spin_unlock(&nss_wifi_mesh_stats_lock);
	kfree(h);
	return true;
}

/**
 * nss_wifi_mesh_get_stats_handle()
 * 	Get Wi-Fi mesh stats handle from interface number.
 */
struct nss_wifi_mesh_stats_handle *nss_wifi_mesh_get_stats_handle(nss_if_num_t if_num)
{
	uint32_t idx;

	assert_spin_locked(&nss_wifi_mesh_stats_lock);

	for (idx = 0; idx < NSS_WIFI_MESH_MAX_DYNAMIC_INTERFACE; idx++) {
		if (nss_wifi_mesh_stats_hdl[idx]) {
			if (nss_wifi_mesh_stats_hdl[idx]->if_num == if_num) {
				struct nss_wifi_mesh_stats_handle *h = nss_wifi_mesh_stats_hdl[idx];
				return h;
			}
		}
	}
	return NULL;
}

/*
 * nss_wifi_mesh_get_stats()
 *	API for getting stats from a Wi-Fi mesh interface stats
 */
static bool nss_wifi_mesh_get_stats(nss_if_num_t if_num, struct nss_wifi_mesh_hdl_stats_sync_msg *stats)
{
	struct nss_wifi_mesh_stats_handle *h;

	if (!nss_wifi_mesh_verify_if_num(if_num)) {
		return false;
	}

	spin_lock(&nss_wifi_mesh_stats_lock);
	h = nss_wifi_mesh_get_stats_handle(if_num);
	if (!h) {
		spin_unlock(&nss_wifi_mesh_stats_lock);
		nss_warning("Invalid Wi-Fi mesh stats handle for interface number: %d\n", if_num);
		return false;
	}

	memcpy(stats, &h->stats, sizeof(*stats));
	spin_unlock(&nss_wifi_mesh_stats_lock);
	return true;
}

/*
 * nss_wifi_mesh_get_valid_interface_count()
 * 	Get count of valid Wi-Fi mesh interfaces up.
 */
static uint32_t nss_wifi_mesh_get_valid_interface_count(uint16_t type, uint32_t if_num, uint32_t max_if_num)
{
	uint32_t interface_count = 0;
	enum nss_dynamic_interface_type dtype;

	for (; if_num <= max_if_num; if_num++) {
		if (!nss_is_dynamic_interface(if_num)) {
			continue;
		}

		dtype = nss_dynamic_interface_get_type(nss_wifi_mesh_get_context(), if_num);

		if ((type == NSS_WIFI_MESH_OUTER_STATS) && (dtype != NSS_DYNAMIC_INTERFACE_TYPE_WIFI_MESH_OUTER)) {
			continue;
		}

		if ((type == NSS_WIFI_MESH_INNER_STATS) && (dtype != NSS_DYNAMIC_INTERFACE_TYPE_WIFI_MESH_INNER)) {
			continue;
		}

		if ((type == NSS_WIFI_MESH_PATH_STATS) && (dtype != NSS_DYNAMIC_INTERFACE_TYPE_WIFI_MESH_INNER)) {
			continue;
		}

		if ((type == NSS_WIFI_MESH_PROXY_PATH_STATS) && (dtype != NSS_DYNAMIC_INTERFACE_TYPE_WIFI_MESH_INNER)) {
			continue;
		}
		interface_count++;
	}
	return interface_count;
}

/**
 * nss_wifi_mesh_stats_read()
 * 	Read Wi-Fi Mesh stats.
 */
static ssize_t nss_wifi_mesh_stats_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos, uint16_t type)
{
	uint32_t max_output_lines, max_stats;
	size_t size_al, size_wr;
	ssize_t bytes_read = 0;
	struct nss_stats_data *data = fp->private_data;
	int ifindex;
	uint32_t if_num = NSS_DYNAMIC_IF_START;
	uint32_t interface_count = 0;
	uint32_t max_if_num = NSS_DYNAMIC_IF_START + NSS_MAX_DYNAMIC_INTERFACES;
	struct nss_wifi_mesh_hdl_stats_sync_msg *stats;
	struct net_device *ndev;
	struct nss_wifi_mesh_stats_handle *handle;
	char *lbuf;
	enum nss_dynamic_interface_type dtype;

	if (data) {
		if_num = data->if_num;
	}

	/*
	 * If we are done accomodating all the Wi-Fi mesh interfaces.
	 */
	if (if_num > max_if_num) {
		return 0;
	}

	/*
	 * Get number of Wi-Fi mesh interfaces up.
	 */
	interface_count = nss_wifi_mesh_get_valid_interface_count(type, if_num, max_if_num);
	if (!interface_count) {
		nss_warning("%px: Invalid number of valid interface for if_num: 0x%x\n", data, if_num);
		return 0;
	}

	/*
	 * max output lines = #stats + Number of Extra outputlines for future reference to add new stats +
	 * Maximum node stats + Maximum of all the stats + three blank lines.
	 */
	max_stats = nss_wifi_mesh_max_statistics();
	max_output_lines = max_stats + NSS_STATS_NODE_MAX + NSS_STATS_EXTRA_OUTPUT_LINES;
	size_al = NSS_STATS_MAX_STR_LENGTH * max_output_lines * interface_count;

	lbuf = kzalloc(size_al, GFP_KERNEL);
	if (unlikely(lbuf == NULL)) {
		nss_warning("Could not allocate memory for local statistics buffer\n");
		return 0;
	}

	size_wr += nss_stats_banner(lbuf, size_wr, size_al, "wifi_mesh", NSS_STATS_SINGLE_CORE);

	stats = kzalloc(sizeof(struct nss_wifi_mesh_hdl_stats_sync_msg), GFP_KERNEL);
	if (!stats) {
		nss_warning("%px: Failed to allocate stats memory for if_num: 0x%x\n", data, if_num);
		kfree(lbuf);
		return 0;
	}

	for (; if_num <= max_if_num; if_num++) {
		bool ret;

		if (!nss_is_dynamic_interface(if_num)) {
			continue;
		}

		dtype = nss_dynamic_interface_get_type(nss_wifi_mesh_get_context(), if_num);

		if ((type == NSS_WIFI_MESH_OUTER_STATS) && (dtype != NSS_DYNAMIC_INTERFACE_TYPE_WIFI_MESH_OUTER)) {
			continue;
		}

		if ((type == NSS_WIFI_MESH_INNER_STATS) && (dtype != NSS_DYNAMIC_INTERFACE_TYPE_WIFI_MESH_INNER)) {
			continue;
		}

		if ((type == NSS_WIFI_MESH_PATH_STATS) && (dtype != NSS_DYNAMIC_INTERFACE_TYPE_WIFI_MESH_INNER)) {
			continue;
		}

		if ((type == NSS_WIFI_MESH_PROXY_PATH_STATS) && (dtype != NSS_DYNAMIC_INTERFACE_TYPE_WIFI_MESH_INNER)) {
			continue;
		}

		/*
		 * If Wi-Fi mesh stats handle does not exists, then ret will be false.
		 */
		ret = nss_wifi_mesh_get_stats(if_num, stats);
		if (!ret) {
			continue;
		}

		spin_lock(&nss_wifi_mesh_stats_lock);
		handle = nss_wifi_mesh_get_stats_handle(if_num);
		if (!handle) {
			spin_unlock(&nss_wifi_mesh_stats_lock);
			nss_warning("Invalid Wi-Fi mesh stats handle, if_num: %d\n", if_num);
			continue;
		}
		ifindex = handle->ifindex;
		spin_unlock(&nss_wifi_mesh_stats_lock);

		ndev = dev_get_by_index(&init_net, ifindex);
		if (!ndev) {
			continue;
		}

		size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, "\n%s if_num:%03u\n",
				     ndev->name, if_num);
		dev_put(ndev);

		/*
		 * Read encap stats, path stats, proxy path stats from inner node and decap stats from outer node.
		 */
		switch (type) {
		case NSS_WIFI_MESH_INNER_STATS:
			size_wr += nss_stats_print("wifi_mesh", "encap stats", NSS_STATS_SINGLE_INSTANCE
					, nss_wifi_mesh_strings_encap_stats
					, stats->encap_stats
					, NSS_WIFI_MESH_ENCAP_STATS_TYPE_MAX
					, lbuf, size_wr, size_al);
			break;

		case NSS_WIFI_MESH_PATH_STATS:
			size_wr += nss_stats_print("wifi_mesh", "path stats", NSS_STATS_SINGLE_INSTANCE
					, nss_wifi_mesh_strings_path_stats
					, stats->path_stats
					, NSS_WIFI_MESH_PATH_STATS_TYPE_MAX
					, lbuf, size_wr, size_al);
			break;

		case NSS_WIFI_MESH_PROXY_PATH_STATS:
			size_wr += nss_stats_print("wifi_mesh", "proxy path stats", NSS_STATS_SINGLE_INSTANCE
					, nss_wifi_mesh_strings_proxy_path_stats
					, stats->proxy_path_stats
					, NSS_WIFI_MESH_PROXY_PATH_STATS_TYPE_MAX
					, lbuf, size_wr, size_al);
			break;

		case NSS_WIFI_MESH_OUTER_STATS:
			size_wr += nss_stats_print("wifi_mesh", "decap stats", NSS_STATS_SINGLE_INSTANCE
					, nss_wifi_mesh_strings_decap_stats
					, stats->decap_stats
					, NSS_WIFI_MESH_DECAP_STATS_TYPE_MAX
					, lbuf, size_wr, size_al);
			break;

		case NSS_WIFI_MESH_EXCEPTION_STATS:
			size_wr += nss_stats_print("wifi_mesh", "exception stats", NSS_STATS_SINGLE_INSTANCE
					, nss_wifi_mesh_strings_exception_stats
					, stats->except_stats
					, NSS_WIFI_MESH_EXCEPTION_STATS_TYPE_MAX
					, lbuf, size_wr, size_al);
			break;

		default:
			nss_warning("%px: Invalid stats type: %d\n", stats, type);
			nss_assert(0);
			kfree(stats);
			kfree(lbuf);
			return 0;
		}
	}

	bytes_read = simple_read_from_buffer(ubuf, sz, ppos, lbuf, size_wr);
	kfree(stats);
	kfree(lbuf);
	return bytes_read;
}

/**
 * nss_wifi_mesh_decap_stats_read()
 *	Read Wi-Fi Mesh decap stats.
 */
static ssize_t nss_wifi_mesh_decap_stats_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	return nss_wifi_mesh_stats_read(fp, ubuf, sz, ppos, NSS_WIFI_MESH_OUTER_STATS);
}

/**
 * nss_wifi_mesh_encap_stats_read()
 *	Read Wi-Fi Mesh encap stats
 */
static ssize_t nss_wifi_mesh_encap_stats_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	return nss_wifi_mesh_stats_read(fp, ubuf, sz, ppos, NSS_WIFI_MESH_INNER_STATS);
}

/**
 * nss_wifi_mesh_path_stats_read()
 *	Read Wi-Fi Mesh path stats
 */
static ssize_t nss_wifi_mesh_path_stats_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	return nss_wifi_mesh_stats_read(fp, ubuf, sz, ppos, NSS_WIFI_MESH_PATH_STATS);
}

/**
 * nss_wifi_mesh_proxy_path_stats_read()
 *	Read Wi-Fi Mesh proxy path stats
 */
static ssize_t nss_wifi_mesh_proxy_path_stats_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	return nss_wifi_mesh_stats_read(fp, ubuf, sz, ppos, NSS_WIFI_MESH_PROXY_PATH_STATS);
}

/**
 * nss_wifi_mesh_exception_stats_read()
 *	Read Wi-Fi Mesh exception stats
 */
static ssize_t nss_wifi_mesh_exception_stats_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	return nss_wifi_mesh_stats_read(fp, ubuf, sz, ppos, NSS_WIFI_MESH_EXCEPTION_STATS);
}

/*
 * nss_wifi_mesh_stats_ops
 */
NSS_STATS_DECLARE_FILE_OPERATIONS(wifi_mesh_encap);
NSS_STATS_DECLARE_FILE_OPERATIONS(wifi_mesh_decap);
NSS_STATS_DECLARE_FILE_OPERATIONS(wifi_mesh_path);
NSS_STATS_DECLARE_FILE_OPERATIONS(wifi_mesh_proxy_path);
NSS_STATS_DECLARE_FILE_OPERATIONS(wifi_mesh_exception);

/*
 * nss_wifi_mesh_get_interface_type()
 * 	Function to get the type of dynamic interface.
 */
static enum nss_dynamic_interface_type nss_wifi_mesh_get_interface_type(nss_if_num_t if_num)
{
	struct nss_ctx_instance *nss_ctx = &nss_top_main.nss[nss_top_main.wifi_handler_id];
	NSS_VERIFY_CTX_MAGIC(nss_ctx);
	return nss_dynamic_interface_get_type(nss_ctx, if_num);
}

/*
 * nss_wifi_mesh_update_stats()
 *	Update stats for Wi-Fi mesh interface.
 */
void nss_wifi_mesh_update_stats(nss_if_num_t if_num, struct nss_wifi_mesh_stats_sync_msg *mstats)
{
	struct nss_wifi_mesh_stats_handle *handle;
	struct nss_wifi_mesh_hdl_stats_sync_msg *stats;
	enum nss_dynamic_interface_type type;
	uint64_t *dst;
	uint32_t *src;
	int i;

	spin_lock(&nss_wifi_mesh_stats_lock);
	handle = nss_wifi_mesh_get_stats_handle(if_num);
	if (!handle) {
		spin_unlock(&nss_wifi_mesh_stats_lock);
		nss_warning("Invalid Wi-Fi mesh stats handle, if_num: %d\n", if_num);
		return;
	}

	type = nss_wifi_mesh_get_interface_type(handle->if_num);;
	stats = &handle->stats;

	switch (type) {
	case NSS_DYNAMIC_INTERFACE_TYPE_WIFI_MESH_INNER:
		/*
		 * Update pnode Rx stats.
		 */
		stats->encap_stats[NSS_WIFI_MESH_ENCAP_STATS_TYPE_PNODE_RX_PACKETS] += mstats->pnode_stats.rx_packets;
		stats->encap_stats[NSS_WIFI_MESH_ENCAP_STATS_TYPE_PNODE_RX_BYTES] += mstats->pnode_stats.rx_bytes;
		stats->encap_stats[NSS_WIFI_MESH_ENCAP_STATS_TYPE_PNODE_RX_DROPPED] += nss_cmn_rx_dropped_sum(&mstats->pnode_stats);

		/*
		 * Update pnode Tx stats.
		 */
		stats->encap_stats[NSS_WIFI_MESH_ENCAP_STATS_TYPE_PNODE_TX_PACKETS] += mstats->pnode_stats.tx_packets;
		stats->encap_stats[NSS_WIFI_MESH_ENCAP_STATS_TYPE_PNODE_TX_BYTES] += mstats->pnode_stats.tx_bytes;

		/*
		 * Update encap stats.
		 */
		dst = &stats->encap_stats[NSS_WIFI_MESH_ENCAP_STATS_TYPE_EXPIRY_NOTIFY_SENT];
		src = &mstats->mesh_encap_stats.expiry_notify_sent;
		for (i = NSS_WIFI_MESH_ENCAP_STATS_TYPE_EXPIRY_NOTIFY_SENT; i < NSS_WIFI_MESH_ENCAP_STATS_TYPE_MAX; i++) {
			*dst++ += *src++;
		}

		/*
		 * Update mesh path stats.
		 */
		dst = &stats->path_stats[NSS_WIFI_MESH_PATH_STATS_TYPE_ALLOC_FAILURES];
		src = &mstats->mesh_path_stats.alloc_failures;
		for (i = NSS_WIFI_MESH_PATH_STATS_TYPE_ALLOC_FAILURES; i < NSS_WIFI_MESH_PATH_STATS_TYPE_MAX; i++) {
			*dst++ += *src++;
		}

		/*
		 * Update mesh proxy path stats.
		 */
		dst = &stats->proxy_path_stats[NSS_WIFI_MESH_PROXY_PATH_STATS_TYPE_ALLOC_FAILURES];
		src = &mstats->mesh_proxy_path_stats.alloc_failures;
		for (i = NSS_WIFI_MESH_PROXY_PATH_STATS_TYPE_ALLOC_FAILURES; i < NSS_WIFI_MESH_PROXY_PATH_STATS_TYPE_MAX; i++) {
			*dst++ += *src++;
		}

		/*
		 * Update exception stats.
		 */
		dst = &stats->except_stats[NSS_WIFI_MESH_EXCEPTION_STATS_TYPE_PACKETS_SUCCESS];
		src = &mstats->mesh_except_stats.packets_success;
		for (i = NSS_WIFI_MESH_EXCEPTION_STATS_TYPE_PACKETS_SUCCESS; i < NSS_WIFI_MESH_EXCEPTION_STATS_TYPE_MAX; i++) {
			*dst++ += *src++;
		}
		spin_unlock(&nss_wifi_mesh_stats_lock);
		break;

	case NSS_DYNAMIC_INTERFACE_TYPE_WIFI_MESH_OUTER:
		/*
		 * Update pnode Rx stats.
		 */
		stats->decap_stats[NSS_WIFI_MESH_DECAP_STATS_TYPE_PNODE_RX_PACKETS] += mstats->pnode_stats.rx_packets;
		stats->decap_stats[NSS_WIFI_MESH_DECAP_STATS_TYPE_PNODE_RX_BYTES] += mstats->pnode_stats.rx_bytes;
		stats->decap_stats[NSS_WIFI_MESH_DECAP_STATS_TYPE_PNODE_RX_DROPPED] += nss_cmn_rx_dropped_sum(&mstats->pnode_stats);

		/*
		 * Update pnode Tx stats.
		 */
		stats->decap_stats[NSS_WIFI_MESH_DECAP_STATS_TYPE_PNODE_TX_PACKETS] += mstats->pnode_stats.tx_packets;
		stats->decap_stats[NSS_WIFI_MESH_DECAP_STATS_TYPE_PNODE_TX_BYTES] += mstats->pnode_stats.tx_bytes;

		/*
		 * Update decap stats.
		 */
		dst = &stats->decap_stats[NSS_WIFI_MESH_DECAP_STATS_TYPE_PATH_REFRESH_SENT];
		src = &mstats->mesh_decap_stats.path_refresh_sent;
		for (i = NSS_WIFI_MESH_DECAP_STATS_TYPE_PATH_REFRESH_SENT; i < NSS_WIFI_MESH_DECAP_STATS_TYPE_MAX; i++) {
			*dst++ += *src++;
		}
		spin_unlock(&nss_wifi_mesh_stats_lock);
		break;

	default:
		spin_unlock(&nss_wifi_mesh_stats_lock);
		nss_warning("%px: Received invalid dynamic interface type: %d\n", handle, type);
		nss_assert(0);
	}
}

/*
 * nss_wifi_mesh_stats_notify()
 *	Sends notifications to the registered modules.
 *
 * Leverage NSS-FW statistics timing to update Netlink.
 */
void nss_wifi_mesh_stats_notify(nss_if_num_t if_num, uint32_t core_id)
{
	struct nss_wifi_mesh_stats_notification wifi_mesh_stats;

	if (!nss_wifi_mesh_get_stats(if_num, &wifi_mesh_stats.stats)) {
		nss_warning("No handle is present with ifnum: 0x%x\n", if_num);
		return;
	}

	wifi_mesh_stats.core_id = core_id;
	wifi_mesh_stats.if_num = if_num;
	atomic_notifier_call_chain(&nss_wifi_mesh_stats_notifier, NSS_STATS_EVENT_NOTIFY, (void *)&wifi_mesh_stats);
}

/*
 * nss_wifi_mesh_stats_dentry_create()
 *	Create Wi-Fi Mesh statistics debug entry
 */
struct dentry *nss_wifi_mesh_stats_dentry_create(void)
{
	struct dentry *stats_dentry_dir;
	struct dentry *stats_file;
	char dir_name[NSS_WIFI_MESH_DENTRY_FILE_SIZE] = {0};

	if (!nss_top_main.stats_dentry) {
		nss_warning("qca-nss-drv/stats is not present\n");
		return NULL;
	}

	snprintf(dir_name, sizeof(dir_name), "wifi_mesh");

	stats_dentry_dir = debugfs_create_dir(dir_name,  nss_top_main.stats_dentry);
	if (!stats_dentry_dir) {
		nss_warning("Failed to create qca-nss-drv/stats/wifi_mesh directory\n");
		return NULL;
	}

	stats_file = debugfs_create_file("encap_stats", 0400, stats_dentry_dir, &nss_top_main, &nss_wifi_mesh_encap_stats_ops);
	if (!stats_file) {
		nss_warning("Failed to create qca-nss-drv/stats/wifi_mesh/encap_stats file\n");
		goto fail;
	}

	stats_file = debugfs_create_file("decap_stats", 0400, stats_dentry_dir, &nss_top_main, &nss_wifi_mesh_decap_stats_ops);
	if (!stats_file) {
		nss_warning("Failed to create qca-nss-drv/stats/wifi_mesh/decap_stats file\n");
		goto fail;
	}

	stats_file = debugfs_create_file("path_stats", 0400, stats_dentry_dir, &nss_top_main, &nss_wifi_mesh_path_stats_ops);
	if (!stats_file) {
		nss_warning("Failed to create qca-nss-drv/stats/wifi_mesh/path_stats file\n");
		goto fail;
	}

	stats_file = debugfs_create_file("proxy_path_stats", 0400, stats_dentry_dir, &nss_top_main, &nss_wifi_mesh_proxy_path_stats_ops);
	if (!stats_file) {
		nss_warning("Failed to create qca-nss-drv/stats/wifi_mesh/proxy_path_stats file\n");
		goto fail;
	}
	stats_file = debugfs_create_file("exception_stats", 0400, stats_dentry_dir, &nss_top_main, &nss_wifi_mesh_exception_stats_ops);
	if (!stats_file) {
		nss_warning("Failed to create qca-nss-drv/stats/wifi_mesh/exception_stats file\n");
		goto fail;
	}
	return stats_dentry_dir;
fail:
	debugfs_remove_recursive(stats_dentry_dir);
	return NULL;
}

/**
 * nss_wifi_mesh_stats_register_notifier()
 *	Registers statistics notifier.
 */
int nss_wifi_mesh_stats_register_notifier(struct notifier_block *nb)
{
	return atomic_notifier_chain_register(&nss_wifi_mesh_stats_notifier, nb);
}
EXPORT_SYMBOL(nss_wifi_mesh_stats_register_notifier);

/**
 * nss_wifi_mesh_stats_unregister_notifier()
 *	Deregisters statistics notifier.
 */
int nss_wifi_mesh_stats_unregister_notifier(struct notifier_block *nb)
{
	return atomic_notifier_chain_unregister(&nss_wifi_mesh_stats_notifier, nb);
}
EXPORT_SYMBOL(nss_wifi_mesh_stats_unregister_notifier);

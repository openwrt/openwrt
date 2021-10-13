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

#ifndef __NSS_WIFI_MESH_STATS_H__
#define __NSS_WIFI_MESH_STATS_H__

/**
 * Array of pointer for NSS Wi-Fi mesh handles.
 * Each handle has per-tunnel statistics based on the interface number which is an index.
 */
struct nss_wifi_mesh_stats_handle {
	nss_if_num_t if_num;				/**< Interface number. */
	uint32_t ifindex;				/**< Netdev index. */
	uint32_t mesh_idx;				/**< Mesh index.  */
	struct nss_wifi_mesh_hdl_stats_sync_msg stats;	/**< Stats per-interface number. */
};

/*
 * Wi-Fi Mesh statistics APIs
 */
extern void nss_wifi_mesh_update_stats(nss_if_num_t if_num, struct nss_wifi_mesh_stats_sync_msg *mstats);
extern void nss_wifi_mesh_stats_notify(nss_if_num_t if_num, uint32_t core_id);
extern struct dentry *nss_wifi_mesh_stats_dentry_create(void);
extern struct nss_wifi_mesh_stats_handle *nss_wifi_mesh_get_stats_handle(nss_if_num_t if_num);
extern bool nss_wifi_mesh_stats_handle_alloc(nss_if_num_t if_num, int32_t ifindex);
extern bool nss_wifi_mesh_stats_handle_free(nss_if_num_t if_num);
#endif /* __NSS_WIFI_MESH_STATS_H__ */

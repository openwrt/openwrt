/*
 ******************************************************************************
 * Copyright (c) 2017,2019 The Linux Foundation. All rights reserved.
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
 * ****************************************************************************
 */

#ifndef __NSS_VIRT_IF_STATS_H
#define __NSS_VIRT_IF_STATS_H

/*
 * virt_if base node statistics types.
 */
enum nss_virt_if_base_node_stats_types {
	NSS_VIRT_IF_BASE_NODE_STATS_ACTIVE_INTERFACES,		/* Number of active virtual interfaces */
	NSS_VIRT_IF_BASE_NODE_STATS_OCM_ALLOC_FAILED,		/* Number of interface allocation failure on OCM */
	NSS_VIRT_IF_BASE_NODE_STATS_DDR_ALLOC_FAILED,		/* Number of interface allocation failure on DDR */
	NSS_VIRT_IF_BASE_NODE_STATS_MAX,
};

/*
 * virt_if interface statistics types.
 */
enum nss_virt_if_interface_stats_types {
	NSS_VIRT_IF_INTERFACE_STATS_RX_PACKETS,			/* Rx packets */
	NSS_VIRT_IF_INTERFACE_STATS_RX_BYTES,			/* Rx bytes */
	NSS_VIRT_IF_INTERFACE_STATS_RX_DROPPED,			/* Rx drop count */
	NSS_VIRT_IF_INTERFACE_STATS_TX_PACKETS,			/* Tx packets */
	NSS_VIRT_IF_INTERFACE_STATS_TX_BYTES,			/* Tx bytes */
	NSS_VIRT_IF_INTERFACE_STATS_TX_ENQUEUE_FAILED,		/* Number of Tx enqueue failure */
	NSS_VIRT_IF_INTERFACE_STATS_SHAPER_ENQUEUE_FAILED,	/* Number of shaper enqueue failure */
	NSS_VIRT_IF_INTERFACE_STATS_OCM_ALLOC_FAILED,		/* Number of interface allocation failure on OCM */
	NSS_VIRT_IF_INTERFACE_STATS_MAX,
};

/*
 * Virtual interface statistics APIs
 */
extern void nss_virt_if_stats_sync(struct nss_virt_if_handle *handle, struct nss_virt_if_stats *nwis);
extern void nss_virt_if_stats_dentry_create(void);

#endif /* __NSS_VIRT_IF_STATS_H */

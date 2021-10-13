/*
 ******************************************************************************
 * Copyright (c) 2018-2020, The Linux Foundation. All rights reserved.
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

#ifndef __NSS_C2C_RX_STATS_H
#define __NSS_C2C_RX_STATS_H

#include <nss_cmn.h>

/*
 * c2c_rx_msg_type
 *	Message types supported
 */
enum c2c_rx_msg_type {
	NSS_C2C_RX_MSG_TYPE_STATS,		/* Statistics synchronization */
	NSS_C2C_RX_MSG_TYPE_MAX
};

/*
 * nss_c2c_rx_stats
 *	The NSS c2c_rx node stats structure.
 */
struct nss_c2c_rx_stats {
	struct nss_cmn_node_stats node_stats;
					/* Common node stats for core-to-core reception. */
	uint32_t pbuf_simple;		/* Number of received simple pbuf. */
	uint32_t pbuf_sg;		/* Number of S/G pbuf received. */
	uint32_t pbuf_returning;	/* Number of returning S/G pbuf. */
	uint32_t inval_dest;		/* Number of pbuf enqueue failure because of dest is invalid. */
};

/*
 * nss_c2c_rx_msg
 *	Message structure to send/receive c2c_rx commands
 */
struct nss_c2c_rx_msg {
	struct nss_cmn_msg cm;			/* Message Header */
	union {
		struct nss_c2c_rx_stats stats;	/* c2c_rx statistics */
	} msg;
};

/*
 * C2C_RX statistics APIs
 */
extern void nss_c2c_rx_stats_notify(struct nss_ctx_instance *nss_ctx);
typedef void (*nss_c2c_rx_msg_callback_t)(void *app_data, struct nss_c2c_rx_msg *msg);
extern void nss_c2c_rx_stats_sync(struct nss_ctx_instance *nss_ctx, struct nss_c2c_rx_stats *ncrs);
extern void nss_c2c_rx_stats_dentry_create(void);

#endif /* __NSS_C2C_RX_STATS_H */

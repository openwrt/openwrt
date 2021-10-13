/*
 **************************************************************************
 * Copyright (c) 2020-2021, The Linux Foundation. All rights reserved.
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

/*
 * nss_ppe_vp.h
 *      NSS PPE virtual port header file
 */

#include <net/sock.h>
#include "nss_tx_rx_common.h"

/*
 * Maximum number of virtual port supported by PPE hardware
 */
#define NSS_PPE_VP_MAX_NUM 192
#define NSS_PPE_VP_START 64
#define NSS_PPE_VP_NODE_STATS_MAX 32
#define NSS_PPE_VP_SWITCH_ID 0
#define NSS_PPE_VP_MAX_CMD_STR 200

/*
 * ppe_vp nss debug stats lock
 */
extern spinlock_t nss_ppe_vp_stats_lock;

/*
 * nss_ppe_vp_msg_error_type
 *	ppe_vp message errors
 */
enum nss_ppe_vp_msg_error_type {
	NSS_PPE_VP_MSG_ERROR_TYPE_UNKNOWN,	/* Unknown message error */
	PPE_VP_MSG_ERROR_TYPE_INVALID_DI,	/* Invalid dynamic interface type */
	NSS_PPE_VP_MSG_ERROR_TYPE_MAX		/* Maximum error type */
};

/*
 * nss_ppe_vp_message_types
 *	Message types for Packet Processing Engine (PPE) requests and responses.
 */
enum nss_ppe_vp_message_types {
	NSS_PPE_VP_MSG_SYNC_STATS,
	NSS_PPE_VP_MSG_DESTROY_NOTIFY,
	NSS_PPE_VP_MSG_MAX,
};

/*
 * nss_ppe_vp_statistics
 *	Message structure for ppe_vp statistics
 */
struct nss_ppe_vp_statistics {
	uint32_t nss_if;			/* NSS interface number corresponding to VP */
	nss_ppe_port_t ppe_port_num;			/* VP number */
	uint32_t rx_drop;			/* Rx drops due to VP node inactive */
	uint32_t tx_drop;			/* Tx drops due to VP node inactive */
	uint32_t packet_big_err;		/* Number of packets not sent to PPE because packet was too large */
	struct nss_cmn_node_stats stats;	/* Common node statistics */
};

/*
 * nss_ppe_vp_sync_stats_msg
 *	Message information for ppe_vp synchronization statistics.
 */
struct nss_ppe_vp_sync_stats_msg {
	uint16_t count;				/* Number of VP node stats with the sync message */
	uint32_t rx_dropped[NSS_MAX_NUM_PRI];	/* Rx packet dropped due to queue full */
	struct nss_ppe_vp_statistics vp_stats[NSS_PPE_VP_NODE_STATS_MAX];
						/* Per service-code stats */
};

/*
 * nss_ppe_vp_destroy_notify_msg
 *	Message received as part of destroy notification from Firmware to Host.
 */
struct nss_ppe_vp_destroy_notify_msg {
	nss_ppe_port_t ppe_port_num;			/* VP number */
};

/*
 * nss_ppe_vp_msg
 *	Message for receiving ppe_vp NSS to host messages.
 */
struct nss_ppe_vp_msg {
	struct nss_cmn_msg cm;		/* Common message header. */

	/*
	 * Payload.
	 */
	union {
		union nss_if_msgs if_msg;
				/* NSS interface base messages. */
		struct nss_ppe_vp_sync_stats_msg stats;
				/* Synchronization statistics. */
		struct nss_ppe_vp_destroy_notify_msg destroy_notify;
				/* Information for the VP destroyed in Firmware. */
	} msg;			/* Message payload. */
};

/*
 * nss_vp_mapping
 *	Structure to maintain the one-to-one mapping between the NSS interface number and VP number.
 */
struct nss_vp_mapping {
	nss_if_num_t if_num;	/* NSS interface number. */
	nss_ppe_port_t ppe_port_num;	/* PPE port number corresponding to the NSS interface number. */
	uint32_t vsi_id;	/* VSI ID allocated for NSS interface */
	bool vsi_id_valid;	/* Set to true if vsi_id field has a valid VSI else set to false. */
};

typedef void (*nss_ppe_vp_msg_callback_t)(void *app_data, struct nss_ppe_vp_msg *msg);

/*
 * Logging APIs.
 */
void nss_ppe_vp_log_tx_msg(struct nss_ppe_vp_msg *npvm);
void nss_ppe_vp_log_rx_msg(struct nss_ppe_vp_msg *npvm);

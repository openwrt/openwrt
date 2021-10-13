/*
 **************************************************************************
 * Copyright (c) 2017, 2019 The Linux Foundation. All rights reserved.
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

/**
 * @file nss_tstamp.h
 *	NSS to HLOS Tstamp interface definitions.
 */

#ifndef __NSS_TSTAMP_H
#define __NSS_TSTAMP_H

/**
 * nss_tstamp_msg_type
 *	Time stamp message types.
 */
enum nss_tstamp_msg_type {
	NSS_TSTAMP_MSG_TYPE_SYNC_STATS,	/**< Statistic synchronization message. */
	NSS_TSTAMP_MSG_TYPE_MAX,	/**< Maximum message type. */
};

/**
 *  nss_tstamp_h2n_pre_hdr
 *	Metadata added by the time stamp HLOS driver.
 *
 * It is used while sending the packet to the NSS time stamp module.
 */
struct nss_tstamp_h2n_pre_hdr {
	uint32_t ts_ifnum;	/**< Time stamp interface number. */
	uint32_t ts_tx_hdr_sz;	/**< Total header size. */
};

/*
 * nss_tstamp_n2h_pre_hdr
 *	Metadata added by the NSS time stamp module.
 *
 * It is added before sending the packet to host.
 */
struct nss_tstamp_n2h_pre_hdr {
	uint32_t ts_ifnum;	/**< Time stamp interface number. */
	uint32_t ts_data_lo;	/**< Time stamp lower order bits. */
	uint32_t ts_data_hi;	/**< Time stamp higher order bits. */

	uint32_t ts_tx;		/**< Time stamp direction. */
	uint32_t ts_hdr_sz;	/**< Size of the header including the skb data alignment padding. */
	uint32_t reserved;	/**< Reserved for cache alignment. */
};

/**
 * nss_tstamp_stats_msg
 *	Statistics messages from the NSS firmware.
 */
struct nss_tstamp_stats_msg {
	struct nss_cmn_node_stats node_stats;
					/**< Common node statistics for time stamp. */
	uint32_t boomeranged;		/**< Boomeranged packets. */
	uint32_t dropped_fail_enqueue;	/**< Enqueue failed. */
	uint32_t dropped_fail_alloc;	/**< Allocation for copy failed. */
	uint32_t dropped_fail_copy;	/**< Copy failed. */
	uint32_t dropped_no_interface;	/**< Next interface not found. */
	uint32_t dropped_no_headroom;	/**< Packet does not have enough headroom. */
};

/**
 * nss_tstamp_msg
 *	Data for sending and receiving time stamp messages.
 */
struct nss_tstamp_msg {
	struct nss_cmn_msg cm;		/**< Common message header. */

	/**
	 * Payload of a time stamp common message.
	 */
	union {
		struct nss_tstamp_stats_msg stats;
				/**< Time stamp statistics. */
	} msg;			/**< Message payload. */
};

/**
 * Callback function for receiving core-to-core transmissions messages.
 *
 * @datatypes
 * nss_tstamp_msg
 *
 * @param[in] app_data  Pointer to the application context of the message.
 * @param[in] msg       Pointer to the message data.
 */
typedef void (*nss_tstamp_msg_callback_t)(void *app_data, struct nss_tstamp_msg *msg);

/**
 * nss_tstamp_notify_register
 *	Registers a notifier callback for time stamp messages with the NSS.
 *
 * @datatypes
 * nss_tstamp_msg_callback_t
 *
 * @param[in] cb        Callback function for the message.
 * @param[in] app_data  Pointer to the application context of the message.
 *
 * @return
 * Pointer to the NSS core context.
 */
struct nss_ctx_instance *nss_tstamp_notify_register(nss_tstamp_msg_callback_t cb, void *app_data);

/**
 * @brief Transfer the packet to time stamp NSS module.
 *
 * @return nss_tx_status
 */
nss_tx_status_t nss_tstamp_tx_buf(struct nss_ctx_instance *nss_ctx, struct sk_buff *skb, uint32_t if_num);

#endif /* __NSS_TSTAMP_H */

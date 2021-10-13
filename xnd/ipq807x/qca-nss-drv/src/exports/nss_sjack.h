/*
 **************************************************************************
 * Copyright (c) 2014-2015, 2017, The Linux Foundation. All rights reserved.
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
 * @file nss_sjack.h
 *	NSS SJACK interface definitions.
 */

#ifndef __NSS_SJACK_H
#define __NSS_SJACK_H

/**
 * @addtogroup nss_sjack_subsystem
 * @{
 */

/**
 * nss_sjack_msg_types
 *	Message types for SJACK requests and responses.
 */
enum nss_sjack_msg_types {
	NSS_SJACK_CONFIGURE_MSG,
	NSS_SJACK_UNCONFIGURE_MSG,
	NSS_SJACK_STATS_SYNC_MSG,
	NSS_SJACK_MAX_MSG_TYPE
};

/**
 * nss_sjack_configure_msg
 *	Message information for configuring the SJACK interface.
 */
struct nss_sjack_configure_msg {
	uint32_t ingress_if_num;
			/**< Ingress interface number corresponding to the SJACK device. */
	uint32_t egress_if_num;
			/**< Egress interface number corresponding to the SJACK device. */
	uint16_t tunnel_id;		/**< SJACK tunnel ID. */
	uint8_t ip_dscp;		/**< Differentiated services code point value. */
	uint8_t gre_prio;		/**< GRE priority information. */
	uint8_t gre_flags;		/**< GRE flags. */
	uint8_t use_ipsec_sa_pattern;	/**< IPsec security association pattern flag. */
};

/**
 * nss_sjack_unconfigure_msg
 *	Message information for de-configuring the SJACK interface.
 */
struct nss_sjack_unconfigure_msg {
	uint32_t ingress_if_num;
			/**< Ingress interface number corresponding to the SJACK device. */
};

/**
 * nss_sjack_stats_sync_msg
 *	Message information for SJACK synchronization statistics.
 */
struct nss_sjack_stats_sync_msg {
	struct nss_cmn_node_stats node_stats;	/**< Common node statistics. */
};

/**
 * nss_sjack_msg
 *	Data for sending and receiving SJACK messages.
 */
struct nss_sjack_msg {
	struct nss_cmn_msg cm;		/**< Common message header. */

	/**
	 * Payload of an SJACK message.
	 */
	union {
		struct nss_sjack_configure_msg configure;
				/**< Configure SJACK. */
		struct nss_sjack_unconfigure_msg unconfigure;
				/**< De-configure SJACK. */
		struct nss_sjack_stats_sync_msg stats_sync;
				/**< Synchronized statistics for SJACK. */
	} msg;			/**< Message payload for SJACK interface messages exchanged with NSS core. */
};

/**
 * Callback function for receiving SJACK messages.
 *
 * @datatypes
 * nss_cmn_msg
 *
 * @param[in] app_data  Pointer to the application context of the message.
 * @param[in] msg       Pointer to the message data.
 */
typedef void (*nss_sjack_msg_callback_t)(void *app_data, struct nss_cmn_msg *msg);

/**
 * nss_sjack_register_if
 *	Registers with the NSS for sending and receiving SJACK messages.
 *
 * @datatypes
 * net_device \n
 * nss_sjack_msg_callback_t
 *
 * @param[in] dev             Pointer to the associated network device.
 * @param[in] if_num          NSS interface number.
 * @param[in] event_callback  Callback for the message.
 *
 * @return
 * Pointer to the NSS core context.
 */
extern struct nss_ctx_instance *nss_sjack_register_if(uint32_t if_num, struct net_device *dev, nss_sjack_msg_callback_t event_callback);

/**
 * nss_sjack_unregister_if
 *	Deregisters the SJACK interface from the NSS.
 *
 * @param[in] if_num  NSS interface number.
 *
 * @return
 * None.
 *
 * @dependencies
 * The interface must have been previously registered.
 */
extern void nss_sjack_unregister_if(uint32_t if_num);

/**
 * nss_sjack_tx_msg
 *	Send SJACK messages to the NSS.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_sjack_msg
 *
 * @param[in,out] nss_ctx  Pointer to the NSS context.
 * @param[in]     msg      Pointer to the message data.
 *
 * @return
 * Status of the Tx operation.
 */
extern nss_tx_status_t nss_sjack_tx_msg(struct nss_ctx_instance *nss_ctx, struct nss_sjack_msg *msg);

/** @} */ /* end_addtogroup nss_sjack_subsystem */

#endif /* __NSS_SJACK_H */

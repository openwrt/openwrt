/*
 **************************************************************************
 * Copyright (c) 2016-2017, 2020 The Linux Foundation. All rights reserved.
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
 * @file nss_trustsec_tx.h
 *	NSS TrustSec interface definitions.
 */

#ifndef __NSS_TRUSTSEC_TX_H
#define __NSS_TRUSTSEC_TX_H

/**
 * @addtogroup nss_trustsec_tx_subsystem
 * @{
 */

/**
 * nss_trustsec_tx_msg_types
 *	Message types for TrustSec Tx requests and responses.
 */
enum nss_trustsec_tx_msg_types {
	NSS_TRUSTSEC_TX_MSG_CONFIGURE,		/** Configure the TrustSec node. */
	NSS_TRUSTSEC_TX_MSG_UNCONFIGURE,	/** Unconfigure the TrustSec node. */
	NSS_TRUSTSEC_TX_MSG_STATS_SYNC,		/** Statistics sychronization. */
	NSS_TRUSTSEC_TX_MSG_UPDATE_NEXTHOP,	/** Update next hop. */
	NSS_TRUSTSEC_TX_MSG_MAX			/** Maximum message type. */
};

/**
 * nss_trustsec_tx_error_types
 *	Error types for the TrustSec Tx interface.
 */
enum nss_trustsec_tx_error_types {
	NSS_TRUSTSEC_TX_ERR_NONE,		/** No error */
	NSS_TRUSTSEC_TX_ERR_INVAL_SRC_IF,	/** Source interface is invalid. */
	NSS_TRUSTSEC_TX_ERR_RECONFIGURE_SRC_IF,	/** Source interface is already configured. */
	NSS_TRUSTSEC_TX_ERR_DEST_IF_NOT_FOUND,	/** Destination interface is not found. */
	NSS_TRUSTSEC_TX_ERR_NOT_CONFIGURED,	/** Source interface is not configured. */
	NSS_TRUSTSEC_TX_ERR_SGT_MISMATCH,	/** SGT mismatches. */
	NSS_TRUSTSEC_TX_ERR_UNKNOWN,		/** Error is unknown. */
	NSS_TRUSTSEC_TX_ERR_MAX,		/** Maximum error message. */
};

/**
 * nss_trustsec_tx_configure_msg
 *	Message information for configuring a TrustSec Tx interface.
 */
struct nss_trustsec_tx_configure_msg {
	uint32_t src;	/**< Interface number of the source tunnel. */
	uint32_t dest;	/**< Outgoing interface number. */
	uint16_t sgt;	/**< Security Group Tag value to embed in the TrustSec header. */
	uint8_t reserved[2];	/**< Reserved for word alignment. */
};

/**
 * nss_trustsec_tx_unconfigure_msg
 *	Message information for de-configuring a TrustSec Tx interface.
 */
struct nss_trustsec_tx_unconfigure_msg {
	uint32_t src;		/**< Interface number of the source tunnel. */
	uint16_t sgt;		/**< Security Group Tag value configured for this interface. */
	uint8_t reserved[2];	/**< Reserved for word alignment. */
};

/**
 * nss_trustsec_tx_stats_sync_msg
 *	Statistics synchronization message for the TrustSec Tx interface.
 */
struct nss_trustsec_tx_stats_sync_msg {
	struct nss_cmn_node_stats node_stats;	/**< Common node statistics. */
	uint32_t invalid_src;		/**< Received packets with an invalid source interface. */
	uint32_t unconfigured_src;	/**< Received packets with a de-configured source interface. */
	uint32_t headroom_not_enough;	/**< Not enough headroom to insert a TrustSec header. */
};

/**
 * nss_trustsec_tx_update_nexthop_msg
 *	Message information for updating the next hop for a TrustSec Tx interface.
 */
struct nss_trustsec_tx_update_nexthop_msg {
	uint32_t src;	/**< Interface number of the source tunnel. */
	uint32_t dest;	/**< Outgoing interface number. */
	uint16_t sgt;	/**< Security Group Tag value to embed in the TrustSec header. */
	uint8_t reserved[2];	/**< Reserved for word alignment. */
};

/**
 * nss_trustsec_tx_msg
 *	Data for sending and receiving TrustSec Tx messages.
 */
struct nss_trustsec_tx_msg {
	struct nss_cmn_msg cm;		/**< Common message header. */

	/**
	 * Payload of a TrustSec Tx message.
	 */
	union {
		struct nss_trustsec_tx_configure_msg configure;
				/**< Configure TrustSec Tx. */
		struct nss_trustsec_tx_unconfigure_msg unconfigure;
				/**< De-configure TrustSec Tx. */
		struct nss_trustsec_tx_stats_sync_msg stats_sync;
				/**< Synchronize TrustSec Tx statistics. */
		struct nss_trustsec_tx_update_nexthop_msg upd_nexthop;
				/**< Update next hop of TrustSec Tx. */
	} msg;			/**< Message payload. */
};

/**
 * Callback function for receiving TrustSec Tx messages.
 *
 * @datatypes
 * nss_trustsec_tx_msg
 *
 * @param[in] app_data  Pointer to the application context of the message.
 * @param[in] msg       Pointer to the message data.
 */
typedef void (*nss_trustsec_tx_msg_callback_t)(void *app_data, struct nss_trustsec_tx_msg *npm);

/**
 * nss_trustsec_tx_msg_init
 *	Initializes a TrustSec Tx message.
 *
 * @datatypes
 * nss_trustsec_tx_msg
 *
 * @param[in,out] npm       Pointer to the NSS Profiler message.
 * @param[in]     if_num    NSS interface number.
 * @param[in]     type      Type of message.
 * @param[in]     len       Size of the message.
 * @param[in]     cb        Callback function for the message.
 * @param[in]     app_data  Pointer to the application context of the message.
 *
 * @return
 * TRUE or FALSE.
 */
extern void nss_trustsec_tx_msg_init(struct nss_trustsec_tx_msg *npm, uint16_t if_num, uint32_t type, uint32_t len,
							nss_trustsec_tx_msg_callback_t cb, void *app_data);

/**
 * nss_trustsec_tx_msg
 *	Sends a TrustSec Tx message to the NSS.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_trustsec_tx_msg
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 * @param[in] msg      Pointer to the message data.
 *
 * @return
 * Status of the Tx operation.
 */
extern nss_tx_status_t nss_trustsec_tx_msg(struct nss_ctx_instance *nss_ctx, struct nss_trustsec_tx_msg *msg);

/**
 * nss_trustsec_tx_msg_sync
 *	Sends a TrustSec Tx message to the NSS and waits for a response.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_trustsec_tx_msg
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 * @param[in] msg      Pointer to the message data.
 *
 * @return
 * Status of the Tx operation.
 */
extern nss_tx_status_t nss_trustsec_tx_msg_sync(struct nss_ctx_instance *nss_ctx, struct nss_trustsec_tx_msg *msg);

/**
 * nss_trustsec_tx_update_nexthop
 *	Updates the next hop of the TrustSec.
 *
 * @param[in] src   Source interface number.
 * @param[in] dest  Destination interface number.
 * @param[in] sgt   Security Group Tag value.
 *
 * @return
 * Pointer to the NSS core context.
 */
extern nss_tx_status_t nss_trustsec_tx_update_nexthop(uint32_t src, uint32_t dest, uint16_t sgt);

/**
 * nss_trustsec_tx_get_ctx
 *	Gets the NSS context.
 *
 * @return
 * Pointer to the NSS core context.
 */
extern struct nss_ctx_instance *nss_trustsec_tx_get_ctx(void);

/**
 * nss_trustsec_tx_configure_sgt
 *	Configures the Security Group Tag value for a source interface.
 *
 * @param[in] src   Source interface number.
 * @param[in] dest  Destination interface number.
 * @param[in] sgt   Security Group Tag value.
 *
 * @return
 * Pointer to the NSS core context.
 */
extern nss_tx_status_t nss_trustsec_tx_configure_sgt(uint32_t src, uint32_t dest, uint16_t sgt);

/**
 * nss_trustsec_tx_unconfigure_sgt
 *	De-configures the Security Group Tag value for a source interface.
 *
 * @param[in] src  Source interface number.
 * @param[in] sgt  Security Group Tag value.
 *
 * @return
 * Pointer to the NSS core context.
 */
extern nss_tx_status_t nss_trustsec_tx_unconfigure_sgt(uint32_t src, uint16_t sgt);

/** @} */ /* end_addtogroup nss_trustsec_tx_subsystem */

#endif /* __NSS_TRUSTSEC_TX_H */

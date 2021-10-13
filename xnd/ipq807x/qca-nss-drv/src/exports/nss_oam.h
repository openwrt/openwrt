/*
 **************************************************************************
 * Copyright (c) 2016-2017 The Linux Foundation. All rights reserved.
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
 * @file nss_oam.h
 *	NSS OAM - Operations, Administration and Maintenance Service
 */

#ifndef __NSS_OAM_H
#define __NSS_OAM_H

/**
 * @addtogroup nss_oam_subsystem
 * @{
 */

#define NSS_OAM_FW_VERSION_LEN	132	/**< Size of the OAM firmware version. */

/**
 * nss_oam_msg_types
 *	OAM command types.
 *
 * The OAM proxy sends these command messages to the NSS OAM server via the OAM adapter.
 */
enum nss_oam_msg_types {
	NSS_OAM_MSG_TYPE_NONE,
	NSS_OAM_MSG_TYPE_GET_FW_VER,
	NSS_OAM_MSG_TYPE_MAX,
};

/**
 * nss_oam_error
 *	OAM error responses.
 */
enum nss_oam_error {
	NSS_OAM_ERROR_NONE,
	NSS_OAM_ERROR_INVAL_MSG_TYPE,
	NSS_OAM_ERROR_INVAL_MSG_LEN,
	NSS_OAM_ERROR_MAX,
};

/**
 * nss_oam_fw_ver
 *	OAM firmware version.
 */
struct nss_oam_fw_ver {
	uint8_t string[NSS_OAM_FW_VERSION_LEN];	/**< OAM firmware version. */
};

/**
 * nss_oam_msg
 *	Data for sending and receiving OAM messages.
 */
struct nss_oam_msg {
	struct nss_cmn_msg cm;	/**< Common message header. */

	/**
	 * Payload of an OAM message.
	 */
	union {
		struct nss_oam_fw_ver fw_ver;
				/**< Firmware version. */
	} msg;			/**< Message payload. */
};

/**
 * Callback function for receiving OAM messages.
 *
 * @datatypes
 * nss_oam_msg
 *
 * @param[in] app_data  Pointer to the application context of the message.
 * @param[in] msg       Pointer to the message data.
 */
typedef void (*nss_oam_msg_callback_t)(void *app_data, struct nss_oam_msg *msg);

/**
 * nss_oam_tx_msg
 *	Transmits an OAM message to the NSS.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_oam_msg
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 * @param[in] msg      Pointer to the message data.
 *
 * @return
 * Status of the Tx operation
 */
extern nss_tx_status_t nss_oam_tx_msg(struct nss_ctx_instance *nss_ctx, struct nss_oam_msg *msg);

/**
 * nss_oam_notify_register
 *	Registers a notifier callback with the NSS for sending and receiving OAM messages.
 *
 * @datatypes
 * nss_oam_msg_callback_t
 *
 * @param[in] cb        Callback function for the message.
 * @param[in] app_data  Pointer to the application context of the message.
 *
 * @return
 * Pointer to the NSS core context.
 */
extern struct nss_ctx_instance *nss_oam_notify_register(nss_oam_msg_callback_t cb, void *app_data);

/**
 * nss_oam_notify_unregister
 *	Deregisters an OAM message notifier callback from the NSS.
 *
 * @return
 * None.
 */
extern void nss_oam_notify_unregister(void);

/**
 * nss_register_oam_if
 *	Registers the OAM interface handler with the NSS.
 *
 * @param[in] if_number  Interface number of the OAM interface.
 *
 * @return
 * Boolean status of handler registration
 */
extern bool nss_register_oam_if(uint16_t if_number);

/**
 * @}
 */

#endif /* __NSS_OAM_H */

/*
 **************************************************************************
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
 **************************************************************************
 */

/*
 * nss_c2c_rx.c
 *	NSS C2C_RX APIs
 */

#include <nss_hal.h>
#include "nss_c2c_rx_stats.h"
#include "nss_c2c_rx_strings.h"

/*
 * nss_c2c_rx_verify_if_num()
 *	Verify if_num passed to us.
 */
static bool nss_c2c_rx_verify_if_num(uint32_t if_num)
{
	return if_num == NSS_C2C_RX_INTERFACE;
}

/*
 * nss_c2c_rx_interface_handler()
 *	Handle NSS -> HLOS messages for C2C_RX Statistics
 */
static void nss_c2c_rx_interface_handler(struct nss_ctx_instance *nss_ctx,
		struct nss_cmn_msg *ncm, __attribute__((unused))void *app_data)
{
	struct nss_c2c_rx_msg *ncrm = (struct nss_c2c_rx_msg *)ncm;
	nss_c2c_rx_msg_callback_t cb;

	if (!nss_c2c_rx_verify_if_num(ncm->interface)) {
		nss_warning("%px: invalid interface %d for c2c_tx\n", nss_ctx, ncm->interface);
		return;
	}

	/*
	 * Is this a valid request/response packet?
	 */
	if (ncm->type >= NSS_C2C_RX_MSG_TYPE_MAX) {
		nss_warning("%px: received invalid message %d for c2c_rx", nss_ctx, ncm->type);
		return;
	}

	if (nss_cmn_get_msg_len(ncm) > sizeof(struct nss_c2c_rx_msg)) {
		nss_warning("%px: Length of message is greater than required: %d", nss_ctx, nss_cmn_get_msg_len(ncm));
		return;
	}

	/*
	 * Log failures
	 */
	nss_core_log_msg_failures(nss_ctx, ncm);

	switch (ncrm->cm.type) {
	case NSS_C2C_RX_MSG_TYPE_STATS:
		/*
		 * Update driver statistics and send statistics notifications to the registered modules.
		 */
		nss_c2c_rx_stats_sync(nss_ctx, &ncrm->msg.stats);
		nss_c2c_rx_stats_notify(nss_ctx);
		break;
	}

	/*
	 * Update the callback and app_data for NOTIFY messages
	 * TODO: Add notify callbacks for c2c_rx
	 */
	if (ncm->response == NSS_CMN_RESPONSE_NOTIFY) {
		return;
	}

	/*
	 * Do we have a callback?
	 */
	if (!ncm->cb) {
		return;
	}

	/*
	 * callback
	 */
	cb = (nss_c2c_rx_msg_callback_t)ncm->cb;
	cb((void *)ncm->app_data, ncrm);
}

/*
 * nss_c2c_rx_register_handler()
 *	Register handler for messaging
 */
void nss_c2c_rx_register_handler(struct nss_ctx_instance *nss_ctx)
{
	nss_core_register_handler(nss_ctx, NSS_C2C_RX_INTERFACE, nss_c2c_rx_interface_handler, NULL);

	if (nss_ctx->id == NSS_CORE_0) {
		nss_c2c_rx_stats_dentry_create();
	}
	nss_c2c_rx_strings_dentry_create();
}
EXPORT_SYMBOL(nss_c2c_rx_register_handler);

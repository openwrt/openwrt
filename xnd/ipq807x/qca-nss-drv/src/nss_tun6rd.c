/*
 **************************************************************************
 * Copyright (c) 2014-2018, 2020, The Linux Foundation. All rights reserved.
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

#include "nss_tx_rx_common.h"
#include "nss_tun6rd_log.h"

/*
 * nss_tun6rd_handler()
 * 	Handle NSS -> HLOS messages for 6rd tunnel
 */
static void nss_tun6rd_handler(struct nss_ctx_instance *nss_ctx, struct nss_cmn_msg *ncm, __attribute__((unused))void *app_data)
{
	struct nss_tun6rd_msg *ntm = (struct nss_tun6rd_msg *)ncm;
	void *ctx;

	nss_tun6rd_msg_callback_t cb;

	BUG_ON(!nss_is_dynamic_interface(ncm->interface));

	/*
	 * Trace Messages
	 */
	nss_tun6rd_log_rx_msg(ntm);

	/*
	 * Is this a valid request/response packet?
	 */
	if (ncm->type >= NSS_TUN6RD_MAX) {
		nss_warning("%px: received invalid message %d for Tun6RD interface", nss_ctx, ncm->type);
		return;
	}

	if (nss_cmn_get_msg_len(ncm) > sizeof(struct nss_tun6rd_msg)) {
		nss_warning("%px: Length of message is greater than required: %d", nss_ctx, nss_cmn_get_msg_len(ncm));
		return;
	}

	/*
 	 * Update the callback and app_data for NOTIFY messages, tun6rd sends all notify messages
 	 * to the same callback/app_data.
 	 */
	if (ncm->response == NSS_CMN_RESPONSE_NOTIFY) {
		ncm->cb = (nss_ptr_t)nss_ctx->nss_top->tun6rd_msg_callback;
	}

	/*
	 * Log failures
	 */
	nss_core_log_msg_failures(nss_ctx, ncm);

	/*
	 * Do we have a call back
	 */
	if (!ncm->cb) {
		return;
	}

	/*
	 * callback
	 */
	cb = (nss_tun6rd_msg_callback_t)ncm->cb;
	ctx = nss_ctx->subsys_dp_register[ncm->interface].ndev;

	/*
	 * call 6rd tunnel callback
	 */
	if (!ctx) {
		nss_warning("%px: Event received for 6rd tunnel interface %d before registration", nss_ctx, ncm->interface);
		return;
	}

	cb(ctx, ntm);
}

/*
 * nss_tun6rd_tx()
 * 	Transmit a tun6rd message to NSSFW
 */
nss_tx_status_t nss_tun6rd_tx(struct nss_ctx_instance *nss_ctx, struct nss_tun6rd_msg *msg)
{
	struct nss_cmn_msg *ncm = &msg->cm;

	/*
	 * Trace Messages
	 */
	nss_tun6rd_log_tx_msg(msg);

	/*
	 * Sanity check the message
	 */
	if (!nss_is_dynamic_interface(ncm->interface)) {
		nss_warning("%px: tx request for another interface: %d", nss_ctx, ncm->interface);
		return NSS_TX_FAILURE;
	}

	if (ncm->type > NSS_TUN6RD_MAX) {
		nss_warning("%px: message type out of range: %d", nss_ctx, ncm->type);
		return NSS_TX_FAILURE;
	}

	return nss_core_send_cmd(nss_ctx, msg, sizeof(*msg), NSS_NBUF_PAYLOAD_SIZE);
}

/*
 ***********************************
 * Register/Unregister/Miscellaneous APIs
 ***********************************
 */

/*
 * nss_register_tun6rd_if()
 */
struct nss_ctx_instance *nss_register_tun6rd_if(uint32_t if_num, uint32_t type, nss_tun6rd_callback_t tun6rd_callback,
			nss_tun6rd_msg_callback_t event_callback, struct net_device *netdev, uint32_t features)
{
	struct nss_ctx_instance *nss_ctx = (struct nss_ctx_instance *)&nss_top_main.nss[nss_top_main.tun6rd_handler_id];

	nss_assert(nss_ctx);
	nss_assert((if_num >=  NSS_DYNAMIC_IF_START) && (if_num < NSS_SPECIAL_IF_START));

	nss_core_register_subsys_dp(nss_ctx, if_num, tun6rd_callback, NULL, NULL, netdev, features);
	nss_ctx->subsys_dp_register[if_num].type = type;

	nss_top_main.tun6rd_msg_callback = event_callback;

	nss_core_register_handler(nss_ctx, if_num, nss_tun6rd_handler, NULL);

	return nss_ctx;
}

/*
 * nss_tun6rd_get_context()
 */
struct nss_ctx_instance *nss_tun6rd_get_context()
{
	return (struct nss_ctx_instance *)&nss_top_main.nss[nss_top_main.tun6rd_handler_id];
}

/*
 * nss_unregister_tun6rd_if()
 */
void nss_unregister_tun6rd_if(uint32_t if_num)
{
	struct nss_ctx_instance *nss_ctx = (struct nss_ctx_instance *)&nss_top_main.nss[nss_top_main.tun6rd_handler_id];

	nss_assert(nss_ctx);
	nss_assert(nss_is_dynamic_interface(if_num));

	nss_core_unregister_subsys_dp(nss_ctx, if_num);
	nss_ctx->subsys_dp_register[if_num].type = 0;

	nss_top_main.tun6rd_msg_callback = NULL;

	nss_core_unregister_handler(nss_ctx, if_num);
}

/*
 * nss_tun6rd_msg_init()
 *      Initialize nss_tun6rd msg.
 */
void nss_tun6rd_msg_init(struct nss_tun6rd_msg *ncm, uint16_t if_num, uint32_t type,  uint32_t len, void *cb, void *app_data)
{
	nss_cmn_msg_init(&ncm->cm, if_num, type, len, cb, app_data);
}

EXPORT_SYMBOL(nss_tun6rd_get_context);
EXPORT_SYMBOL(nss_tun6rd_tx);
EXPORT_SYMBOL(nss_register_tun6rd_if);
EXPORT_SYMBOL(nss_unregister_tun6rd_if);
EXPORT_SYMBOL(nss_tun6rd_msg_init);

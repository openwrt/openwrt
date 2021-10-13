/*
 **************************************************************************
 * Copyright (c) 2015-2018, 2020, The Linux Foundation. All rights reserved.
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
#include "nss_wifi_stats.h"
#include "nss_wifi_log.h"

/*
 * nss_wifi_get_context()
 *	Get NSS context of Wifi.
 */
struct nss_ctx_instance *nss_wifi_get_context()
{
	return (struct nss_ctx_instance *)&nss_top_main.nss[nss_top_main.wifi_handler_id];
}

/*
 * nss_wifi_handler()
 * 	Handle NSS -> HLOS messages for wifi
 */
static void nss_wifi_handler(struct nss_ctx_instance *nss_ctx, struct nss_cmn_msg *ncm, __attribute__((unused))void *app_data)
{
	struct nss_wifi_msg *ntm = (struct nss_wifi_msg *)ncm;
	void *ctx;
	nss_wifi_msg_callback_t cb;

	nss_info("%px: NSS ->HLOS message for wifi\n", nss_ctx);

	BUG_ON(((ncm->interface < NSS_WIFI_INTERFACE0) || (ncm->interface > NSS_WIFI_INTERFACE2)));

	/*
	 * Trace messages.
	 */
	nss_wifi_log_rx_msg(ntm);

	/*
	 * Is this a valid request/response packet?
	 */
	if (ncm->type >= NSS_WIFI_MAX_MSG) {
		nss_warning("%px: received invalid message %d for wifi interface", nss_ctx, ncm->type);
		return;
	}

	if (nss_cmn_get_msg_len(ncm) > sizeof(struct nss_wifi_msg)) {
		nss_warning("%px: Length of message is greater than required: %d", nss_ctx, nss_cmn_get_msg_len(ncm));
		return;
	}

	/*
	 * Snoop messages for local driver and handle
	 */
	switch (ntm->cm.type) {
	case NSS_WIFI_STATS_MSG:
		/*
		 * To create the old API gmac statistics, we use the new extended GMAC stats.
		 */
		nss_wifi_stats_sync(nss_ctx, &ntm->msg.statsmsg, ncm->interface);
		break;
	}

	/*
	 * Update the callback and app_data for NOTIFY messages, wifi sends all notify messages
	 * to the same callback/app_data.
	 */
	if (ncm->response == NSS_CMN_RESPONSE_NOTIFY) {
		ncm->cb = (nss_ptr_t)nss_ctx->nss_top->wifi_msg_callback;
	}

	/*
	 * Log failures
	 */
	nss_core_log_msg_failures(nss_ctx, ncm);

	/*
	 * Do we have a call back
	 */
	if (!ncm->cb) {
		nss_info("%px: cb null for wifi interface %d", nss_ctx, ncm->interface);
		return;
	}

	/*
	 * Get callback & context
	 */
	cb = (nss_wifi_msg_callback_t)ncm->cb;
	ctx = nss_ctx->subsys_dp_register[ncm->interface].ndev;

	/*
	 * call wifi msg callback
	 */
	if (!ctx) {
		nss_warning("%px: Event received for wifi interface %d before registration", nss_ctx, ncm->interface);
		return;
	}

	cb(ctx, ntm);
}

/*
 * nss_wifi_tx_msg
 * 	Transmit a wifi message to NSS FW
 */
nss_tx_status_t nss_wifi_tx_msg(struct nss_ctx_instance *nss_ctx, struct nss_wifi_msg *msg)
{
	struct nss_cmn_msg *ncm = &msg->cm;

	/*
	 * Trace messages.
	 */
	nss_wifi_log_tx_msg(msg);

	if (ncm->type > NSS_WIFI_MAX_MSG) {
		nss_warning("%px: wifi message type out of range: %d", nss_ctx, ncm->type);
		return NSS_TX_FAILURE;
	}

	return nss_core_send_cmd(nss_ctx, msg, sizeof(*msg), NSS_NBUF_PAYLOAD_SIZE);
}

/*
 ****************************************
 * Register/Unregister/Miscellaneous APIs
 ****************************************
 */

/*
 * nss_register_wifi_if()
 * 	Register Wifi with nss driver
 */
struct nss_ctx_instance *nss_register_wifi_if(uint32_t if_num, nss_wifi_callback_t wifi_callback,
			nss_wifi_callback_t wifi_ext_callback,
			nss_wifi_msg_callback_t event_callback, struct net_device *netdev, uint32_t features)
{
	struct nss_ctx_instance *nss_ctx = (struct nss_ctx_instance *)&nss_top_main.nss[nss_top_main.wifi_handler_id];

	nss_assert(nss_ctx);
	nss_assert((if_num >= NSS_MAX_VIRTUAL_INTERFACES) && (if_num < NSS_MAX_NET_INTERFACES));

	nss_info("%px: nss_register_wifi_if if_num %d wifictx %px", nss_ctx, if_num, netdev);

	nss_core_register_subsys_dp(nss_ctx, if_num, wifi_callback, wifi_ext_callback, NULL, netdev, features);

	nss_top_main.wifi_msg_callback = event_callback;

	return nss_ctx;
}

/*
 * nss_unregister_wifi_if()
 * 	Unregister wifi with nss driver
 */
void nss_unregister_wifi_if(uint32_t if_num)
{
	struct nss_ctx_instance *nss_ctx = (struct nss_ctx_instance *)&nss_top_main.nss[nss_top_main.wifi_handler_id];

	nss_assert(nss_ctx);
	nss_assert((if_num >= NSS_MAX_VIRTUAL_INTERFACES) && (if_num < NSS_MAX_NET_INTERFACES));

	nss_ctx->nss_top->wifi_msg_callback = NULL;
	nss_core_unregister_subsys_dp(nss_ctx, if_num);
}

/*
 * nss_wifi_register_handler()
 * 	Register handle for notfication messages received on wifi interface
 */
void nss_wifi_register_handler(void )
{
	struct nss_ctx_instance *nss_ctx = (struct nss_ctx_instance *)&nss_top_main.nss[nss_top_main.wifi_handler_id];

	nss_assert(nss_ctx);

	nss_info("nss_wifi_register_handler");

	nss_core_register_handler(nss_ctx, NSS_WIFI_INTERFACE0, nss_wifi_handler, NULL);
	nss_core_register_handler(nss_ctx, NSS_WIFI_INTERFACE1, nss_wifi_handler, NULL);
	nss_core_register_handler(nss_ctx, NSS_WIFI_INTERFACE2, nss_wifi_handler, NULL);

	nss_wifi_stats_dentry_create();
}

EXPORT_SYMBOL(nss_wifi_get_context);
EXPORT_SYMBOL(nss_wifi_tx_msg);
EXPORT_SYMBOL(nss_register_wifi_if);
EXPORT_SYMBOL(nss_unregister_wifi_if);

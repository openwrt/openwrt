/*
 **************************************************************************
 * Copyright (c) 2020, The Linux Foundation. All rights reserved.
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

#include "nss_core.h"
#include "nss_wifi_mac_db_if.h"

/*
 * Compile time assertion.
 */
#define NSS_WIFI_MAC_DB_COMPILE_TIME_ASSERT(assertion_name, predicate) \
    typedef char assertion_name[(predicate) ? 1 : -1]

#define NSS_WIFI_MAC_DB_TX_TIMEOUT 1000 /* Millisecond to jiffies*/

/*
 * Validate the Wi-Fi MAC database message size not exceeding buffer size.
 */
NSS_WIFI_MAC_DB_COMPILE_TIME_ASSERT(NSS_WIFI_MAC_DB_MAX_BUF_MSG,
		(sizeof(struct nss_wifi_mac_db_msg) < NSS_NBUF_PAYLOAD_SIZE));

/*
 * nss_wifi_mac_db_get_context()
 */
struct nss_ctx_instance *nss_wifi_mac_db_get_context(void)
{
	return (struct nss_ctx_instance *)&nss_top_main.nss[nss_top_main.wmdb_handler_id];
}
EXPORT_SYMBOL(nss_wifi_mac_db_get_context);

/*
 * nss_wifi_mac_db_pvt
 *	Private data structure
 */
static struct nss_wifi_mac_db_pvt {
	struct semaphore sem;
	struct completion complete;
	int response;
	void *cb;
	void *app_data;
} wifi_mac_db_pvt;

/*
 * nss_wifi_mac_db_handler()
 *	Handle NSS -> HLOS messages for wifi_mac_db
 */
static void nss_wifi_mac_db_handler(struct nss_ctx_instance *nss_ctx, struct nss_cmn_msg *ncm, __attribute__((unused))void *app_data)
{
	struct nss_wifi_mac_db_msg *ntm = (struct nss_wifi_mac_db_msg *)ncm;
	void *ctx;
	nss_wifi_mac_db_msg_callback_t cb;

	nss_info("%px: NSS->HLOS message for wifi_mac_db\n", nss_ctx);

	/*
	 * The interface number shall be wifi_mac_db soc interface or wifi_mac_db radio interface
	 */
	BUG_ON((ncm->interface != NSS_WIFI_MAC_DB_INTERFACE));

	/*
	 * Is this a valid request/response packet?
	 */
	if (ncm->type >= NSS_WIFI_MAC_DB_MAX_MSG) {
		nss_warning("%px: Received invalid message %d for wifi_mac_db interface", nss_ctx, ncm->type);
		return;
	}

	if (nss_cmn_get_msg_len(ncm) > sizeof(struct nss_wifi_mac_db_msg)) {
		nss_warning("%px: Length of message is greater than required: %d", nss_ctx, nss_cmn_get_msg_len(ncm));
		return;
	}

	/*
	 * Update the callback and app_data for notify messages, wifi_mac_db sends all notify messages
	 * to the same callback/app_data.
	 */
	if (ncm->response == NSS_CMN_RESPONSE_NOTIFY) {
		ncm->cb = (nss_ptr_t)nss_ctx->nss_top->wifi_mac_db_msg_callback;
	}

	/*
	 * Log failures
	 */
	nss_core_log_msg_failures(nss_ctx, ncm);

	/*
	 * Do we have a call back
	 */
	if (!ncm->cb) {
		nss_info("%px: cb null for wifi_mac_db interface %d", nss_ctx, ncm->interface);
		return;
	}

	/*
	 * Get callback & context
	 */
	cb = (nss_wifi_mac_db_msg_callback_t)ncm->cb;
	ctx = nss_ctx->subsys_dp_register[ncm->interface].ndev;

	/*
	 * call wifi_mac_db msg callback
	 */
	if (!ctx) {
		nss_warning("%px: Event received for wifi_mac_db interface %d before registration", nss_ctx, ncm->interface);
		return;
	}

	cb(ctx, ntm);
}

/*
 * nss_wifi_mac_db_tx_msg
 *	Transmit a wifi_mac_db message to NSS FW
 *
 * NOTE: The caller is expected to handle synchronous wait for message
 * response if needed.
 */
nss_tx_status_t nss_wifi_mac_db_tx_msg(struct nss_ctx_instance *nss_ctx, struct nss_wifi_mac_db_msg *msg)
{
	struct nss_cmn_msg *ncm = &msg->cm;

	if (ncm->type >= NSS_WIFI_MAC_DB_MAX_MSG) {
		nss_warning("%px: wifi_mac_db message type out of range: %d", nss_ctx, ncm->type);
		return NSS_TX_FAILURE;
	}

	/*
	 * The interface number shall be one of the wifi_mac_db soc interfaces
	 */
	if ((ncm->interface != NSS_WIFI_MAC_DB_INTERFACE)) {
		nss_warning("%px: tx request for interface that is not a wifi_mac_db: %d", nss_ctx, ncm->interface);
		return NSS_TX_FAILURE;
	}

	return nss_core_send_cmd(nss_ctx, msg, sizeof(*msg), NSS_NBUF_PAYLOAD_SIZE);
}
EXPORT_SYMBOL(nss_wifi_mac_db_tx_msg);

/*
 ****************************************
 * Register/Unregister/Miscellaneous APIs
 ****************************************
 */

/*
 * nss_register_wifi_mac_db_if()
 *	Register wifi_mac_db with nss driver
 */
struct nss_ctx_instance *nss_register_wifi_mac_db_if(uint32_t if_num, nss_wifi_mac_db_callback_t wifi_mac_db_callback,
			nss_wifi_mac_db_callback_t wifi_mac_db_ext_callback,
			nss_wifi_mac_db_msg_callback_t event_callback, struct net_device *netdev, uint32_t features)
{
	struct nss_ctx_instance *nss_ctx = (struct nss_ctx_instance *)&nss_top_main.nss[nss_top_main.wmdb_handler_id];

	/*
	 * The interface number shall be wifi_mac_db interface
	 */
	nss_assert(if_num == NSS_WIFI_MAC_DB_INTERFACE);

	nss_info("%px: nss_register_wifi_mac_db_if if_num:%d wifi_mac_db_dev:%px", nss_ctx, if_num, netdev);

	nss_core_register_subsys_dp(nss_ctx, if_num, wifi_mac_db_callback, NULL, NULL, netdev, features);

	nss_top_main.wifi_mac_db_msg_callback = event_callback;

	return (struct nss_ctx_instance *)&nss_top_main.nss[nss_top_main.wmdb_handler_id];
}
EXPORT_SYMBOL(nss_register_wifi_mac_db_if);

/*
 * nss_unregister_wifi_mac_db_if()
 *	Unregister wifi_mac_db with nss driver
 */
void nss_unregister_wifi_mac_db_if(uint32_t if_num)
{
	struct nss_ctx_instance *nss_ctx = (struct nss_ctx_instance *)&nss_top_main.nss[nss_top_main.wmdb_handler_id];

	/*
	 * The interface number shall be wifi_mac_db interface
	 */
	nss_assert(if_num == NSS_WIFI_MAC_DB_INTERFACE);

	nss_core_unregister_subsys_dp(nss_ctx, if_num);
}
EXPORT_SYMBOL(nss_unregister_wifi_mac_db_if);

/*
 * nss_wifi_mac_db_register_handler()
 *	Register handle for notfication messages received on wifi mac db
 */
void nss_wifi_mac_db_register_handler(void)
{
	struct nss_ctx_instance *nss_ctx =
		(struct nss_ctx_instance *)&nss_top_main.nss[nss_top_main.wmdb_handler_id];

	nss_info("wifi_mac_db_register_handler");
	nss_core_register_handler(nss_ctx, NSS_WIFI_MAC_DB_INTERFACE, nss_wifi_mac_db_handler, NULL);

	sema_init(&wifi_mac_db_pvt.sem, 1);
	init_completion(&wifi_mac_db_pvt.complete);
}

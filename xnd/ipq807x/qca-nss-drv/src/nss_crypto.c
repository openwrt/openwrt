/*
 **************************************************************************
 * Copyright (c) 2013,2015-2020, The Linux Foundation. All rights reserved.
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
 * nss_crypto.c
 *	NSS Crypto APIs
 */

#include "nss_tx_rx_common.h"
#include "nss_crypto.h"
#include "nss_crypto_log.h"

/*
 **********************************
 General APIs
 **********************************
 */

/*
 * nss_crypto_set_msg_callback()
 *	this sets the message callback handler and its associated context
 */
static inline void nss_crypto_set_msg_callback(struct nss_ctx_instance *nss_ctx, nss_crypto_msg_callback_t cb, void *crypto_ctx)
{
	struct nss_top_instance *nss_top = nss_ctx->nss_top;

	nss_top->crypto_ctx = crypto_ctx;
	nss_top->crypto_msg_callback = cb;
}

/*
 * nss_crypto_get_msg_callback()
 *	this gets the message callback handler and its associated context
 */
static inline nss_crypto_msg_callback_t nss_crypto_get_msg_callback(struct nss_ctx_instance *nss_ctx, void **crypto_ctx)
{
	struct nss_top_instance *nss_top = nss_ctx->nss_top;

	*crypto_ctx = nss_top->crypto_ctx;
	return nss_top->crypto_msg_callback;
}

/*
 * nss_crypto_msg_handler()
 *	this handles all the IPsec events and responses
 */
static void nss_crypto_msg_handler(struct nss_ctx_instance *nss_ctx, struct nss_cmn_msg *ncm, void *app_data __attribute((unused)))
{
	struct nss_crypto_msg *nim = (struct nss_crypto_msg *)ncm;
	nss_crypto_msg_callback_t cb = NULL;
	void *crypto_ctx = NULL;

	/*
	 * Sanity check the message type
	 */
	if (ncm->type > NSS_CRYPTO_MSG_TYPE_MAX) {
		nss_warning("%px: rx message type out of range: %d", nss_ctx, ncm->type);
		return;
	}

	if (nss_cmn_get_msg_len(ncm) > sizeof(struct nss_crypto_msg)) {
		nss_warning("%px: rx message length is invalid: %d", nss_ctx, nss_cmn_get_msg_len(ncm));
		return;
	}

	if (ncm->interface != NSS_CRYPTO_INTERFACE) {
		nss_warning("%px: rx message request for another interface: %d", nss_ctx, ncm->interface);
		return;
	}

	if (ncm->response == NSS_CMN_RESPONSE_LAST) {
		nss_warning("%px: rx message response for if %d, type %d, is invalid: %d", nss_ctx, ncm->interface,
				ncm->type, ncm->response);
		return;
	}

	if (ncm->response == NSS_CMN_RESPONSE_NOTIFY) {
		ncm->cb = (nss_ptr_t)nss_crypto_get_msg_callback(nss_ctx, &crypto_ctx);
		ncm->app_data = (nss_ptr_t)crypto_ctx;
	}

	nss_core_log_msg_failures(nss_ctx, ncm);

	/*
	 * Trace messages.
	 */
	nss_crypto_log_rx_msg(nim);

	/*
	 * Load, Test & call
	 */
	cb = (nss_crypto_msg_callback_t)ncm->cb;
	if (unlikely(!cb)) {
		nss_trace("%px: rx handler has been unregistered for i/f: %d", nss_ctx, ncm->interface);
		return;
	}
	cb((void *)ncm->app_data, nim);
}
/*
 **********************************
 Tx APIs
 **********************************
 */

/*
 * nss_crypto_tx_msg
 *	Send crypto config to NSS.
 */
nss_tx_status_t nss_crypto_tx_msg(struct nss_ctx_instance *nss_ctx, struct nss_crypto_msg *msg)
{
	struct nss_cmn_msg *ncm = &msg->cm;

	nss_info("%px: tx message %d for if %d\n", nss_ctx, ncm->type, ncm->interface);

	BUILD_BUG_ON(NSS_NBUF_PAYLOAD_SIZE < sizeof(struct nss_crypto_msg));

	if (ncm->interface != NSS_CRYPTO_INTERFACE) {
		nss_warning("%px: tx message request for another interface: %d", nss_ctx, ncm->interface);
	}

	if (ncm->type > NSS_CRYPTO_MSG_TYPE_MAX) {
		nss_warning("%px: tx message type out of range: %d", nss_ctx, ncm->type);
		return NSS_TX_FAILURE;
	}

	nss_info("msg params version:%d, interface:%d, type:%d, cb:%px, app_data:%px, len:%d\n",
			ncm->version, ncm->interface, ncm->type, (void *)ncm->cb, (void *)ncm->app_data, ncm->len);

	/*
	 * Trace messages.
	 */
	nss_crypto_log_tx_msg(msg);

	return nss_core_send_cmd(nss_ctx, msg, sizeof(*msg), NSS_NBUF_PAYLOAD_SIZE);
}

/*
 * nss_crypto_tx_data()
 *	NSS crypto TX data API. Sends a crypto buffer to NSS.
 */
nss_tx_status_t nss_crypto_tx_buf(struct nss_ctx_instance *nss_ctx, uint32_t if_num, struct sk_buff *skb)
{
	int32_t status;

	nss_trace("%px: tx_data buf=%px", nss_ctx, skb);

	NSS_VERIFY_CTX_MAGIC(nss_ctx);
	if (unlikely(nss_ctx->state != NSS_CORE_STATE_INITIALIZED)) {
		nss_warning("%px: tx_data packet dropped as core not ready", nss_ctx);
		return NSS_TX_FAILURE_NOT_READY;
	}

	status = nss_core_send_buffer(nss_ctx, if_num, skb, NSS_IF_H2N_DATA_QUEUE, H2N_BUFFER_PACKET, H2N_BIT_FLAG_BUFFER_REUSABLE);
	if (unlikely(status != NSS_CORE_STATUS_SUCCESS)) {
		nss_warning("%px: tx_data Unable to enqueue packet", nss_ctx);
		if (status == NSS_CORE_STATUS_FAILURE_QUEUE) {
			return NSS_TX_FAILURE_QUEUE;
		}

		return NSS_TX_FAILURE;
	}

	/*
	 * Kick the NSS awake so it can process our new entry.
	 */
	nss_hal_send_interrupt(nss_ctx, NSS_H2N_INTR_DATA_COMMAND_QUEUE);

	NSS_PKT_STATS_INC(&nss_ctx->nss_top->stats_drv[NSS_DRV_STATS_TX_CRYPTO_REQ]);

	return NSS_TX_SUCCESS;
}

/*
 **********************************
 Register APIs
 **********************************
 */

/*
 * nss_crypto_notify_register()
 *	register message notifier for crypto interface
 */
struct nss_ctx_instance *nss_crypto_notify_register(nss_crypto_msg_callback_t cb, void *app_data)
{
	struct nss_ctx_instance *nss_ctx;

	nss_ctx = &nss_top_main.nss[nss_top_main.crypto_handler_id];

	nss_crypto_set_msg_callback(nss_ctx, cb, app_data);

	return nss_ctx;
}

/*
 * nss_crypto_notify_unregister()
 *	unregister message notifier for crypto interface
 */
void nss_crypto_notify_unregister(struct nss_ctx_instance *nss_ctx)
{
	nss_crypto_set_msg_callback(nss_ctx, NULL, NULL);
}

/*
 * nss_crypto_data_register()
 *	register a data callback routine
 */
struct nss_ctx_instance *nss_crypto_data_register(uint32_t if_num, nss_crypto_buf_callback_t cb,
		struct net_device *netdev, uint32_t features)
{
	struct nss_ctx_instance *nss_ctx;

	nss_ctx = &nss_top_main.nss[nss_top_main.crypto_handler_id];

	if ((if_num >= NSS_MAX_NET_INTERFACES) && (if_num < NSS_MAX_PHYSICAL_INTERFACES)) {
		nss_warning("%px: data register received for invalid interface %d", nss_ctx, if_num);
		return NULL;
	}

	/*
	 * Register subsystem, ensuring that no duplicate registrations occur.
	 */
	nss_core_register_subsys_dp(nss_ctx, if_num, cb, NULL, NULL, netdev, features);

	return nss_ctx;
}

/*
 * nss_crypto_data_unregister()
 *	unregister a data callback routine
 */
void nss_crypto_data_unregister(struct nss_ctx_instance *nss_ctx, uint32_t if_num)
{
	if ((if_num >= NSS_MAX_NET_INTERFACES) && (if_num < NSS_MAX_PHYSICAL_INTERFACES)) {
		nss_warning("%px: data unregister received for invalid interface %d", nss_ctx, if_num);
		return;
	}

	nss_core_unregister_subsys_dp(nss_ctx, if_num);
}

/*
 * nss_crypto_pm_notify_register()
 *	register a PM notify callback routine
 */
void nss_crypto_pm_notify_register(nss_crypto_pm_event_callback_t cb, void *app_data)
{
	nss_top_main.crypto_pm_ctx = app_data;
	nss_top_main.crypto_pm_callback = cb;
}

/*
 * nss_crypto_pm_notify_unregister()
 *	unregister a PM notify callback routine
 */
void nss_crypto_pm_notify_unregister(void)
{
	nss_top_main.crypto_pm_ctx = NULL;
	nss_top_main.crypto_pm_callback = NULL;
}

/*
 * nss_crypto_register_handler()
 */
void nss_crypto_register_handler(void)
{
	struct nss_ctx_instance *nss_ctx = &nss_top_main.nss[nss_top_main.crypto_handler_id];

	nss_core_register_handler(nss_ctx, NSS_CRYPTO_INTERFACE, nss_crypto_msg_handler, NULL);
}

/*
 * nss_crypto_msg_init()
 *	Initialize crypto message
 */
void nss_crypto_msg_init(struct nss_crypto_msg *ncm, uint16_t if_num, uint32_t type, uint32_t len,
				nss_crypto_msg_callback_t cb, void *app_data)
{
	nss_cmn_msg_init(&ncm->cm, if_num, type, len, (void *)cb, app_data);
}

EXPORT_SYMBOL(nss_crypto_notify_register);
EXPORT_SYMBOL(nss_crypto_notify_unregister);
EXPORT_SYMBOL(nss_crypto_data_register);
EXPORT_SYMBOL(nss_crypto_data_unregister);
EXPORT_SYMBOL(nss_crypto_pm_notify_register);
EXPORT_SYMBOL(nss_crypto_pm_notify_unregister);
EXPORT_SYMBOL(nss_crypto_tx_msg);
EXPORT_SYMBOL(nss_crypto_tx_buf);
EXPORT_SYMBOL(nss_crypto_msg_init);

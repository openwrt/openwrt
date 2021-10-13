/*
 **************************************************************************
 * Copyright (c) 2016-2020, The Linux Foundation. All rights reserved.
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
 * nss_edma.c
 *	NSS EDMA APIs
 */
#include "nss_edma_stats.h"
#include "nss_edma_strings.h"

/*
 **********************************
 Rx APIs
 **********************************
 */

/*
 * nss_edma_interface_handler()
 *	Handle NSS -> HLOS messages for EDMA node
 */
static void nss_edma_interface_handler(struct nss_ctx_instance *nss_ctx, struct nss_cmn_msg *ncm, __attribute__((unused))void *app_data)
{
	struct nss_edma_msg *nem = (struct nss_edma_msg *)ncm;
	nss_edma_msg_callback_t cb;

	/*
	 * Is this a valid request/response packet?
	 */
	if (nem->cm.type >= NSS_METADATA_TYPE_EDMA_MAX) {
		nss_warning("%px: received invalid message %d for edma interface", nss_ctx, nem->cm.type);
		return;
	}

	/*
	 * Handle different types of messages
	 */
	switch (nem->cm.type) {
	case NSS_METADATA_TYPE_EDMA_PORT_STATS_SYNC:
		/*
		 * Update driver statistics and send statistics notifications to the registered modules.
		 */
		nss_edma_metadata_port_stats_sync(nss_ctx, &nem->msg.port_stats);
		nss_edma_stats_notify(nss_ctx);

		break;
	case NSS_METADATA_TYPE_EDMA_RING_STATS_SYNC:
		nss_edma_metadata_ring_stats_sync(nss_ctx, &nem->msg.ring_stats);
		break;
	case NSS_METADATA_TYPE_EDMA_ERR_STATS_SYNC:
		nss_edma_metadata_err_stats_sync(nss_ctx, &nem->msg.err_stats);
		break;
	default:
		if (ncm->response != NSS_CMN_RESPONSE_ACK) {
			/*
			 * Check response
			 */
			nss_info("%px: Received response %d for type %d, interface %d",
						nss_ctx, ncm->response, ncm->type, ncm->interface);
		}
	}
	/*
	 * Update the callback and app_data for NOTIFY messages, edma sends all notify messages
	 * to the same callback/app_data.
	 */
	if (nem->cm.response == NSS_CMN_RESPONSE_NOTIFY) {
		ncm->cb = (nss_ptr_t)nss_ctx->nss_top->edma_callback;
		ncm->app_data = (nss_ptr_t)nss_ctx->nss_top->edma_ctx;
	}

	/*
	 * Do we have a callback?
	 */
	if (!ncm->cb) {
		return;
	}

	/*
	 * Callback
	 */
	cb = (nss_edma_msg_callback_t)ncm->cb;
	cb((void *)ncm->app_data, nem);
}

/*
 * nss_edma_notify_register()
 *	Register to received EDMA events.
 */
struct nss_ctx_instance *nss_edma_notify_register(nss_edma_msg_callback_t cb, void *app_data)
{
	nss_top_main.edma_callback = cb;
	nss_top_main.edma_ctx = app_data;
	return &nss_top_main.nss[nss_top_main.edma_handler_id];
}
EXPORT_SYMBOL(nss_edma_notify_register);

/*
 * nss_edma_notify_unregister()
 *	Unregister to received EDMA events.
 */
void nss_edma_notify_unregister(void)
{
	nss_top_main.edma_callback = NULL;
}
EXPORT_SYMBOL(nss_edma_notify_unregister);

/*
 * nss_get_edma_context()
 */
struct nss_ctx_instance *nss_edma_get_context(void)
{
	return (struct nss_ctx_instance *)&nss_top_main.nss[nss_top_main.edma_handler_id];
}
EXPORT_SYMBOL(nss_edma_get_context);

/*
 * nss_edma_register_handler()
 */
void nss_edma_register_handler(void)
{
	struct nss_ctx_instance *nss_ctx = nss_edma_get_context();

	nss_core_register_handler(nss_ctx, NSS_EDMA_INTERFACE, nss_edma_interface_handler, NULL);

	nss_edma_stats_dentry_create();
	nss_edma_strings_dentry_create();
}

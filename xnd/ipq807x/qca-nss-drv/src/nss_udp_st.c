/*
 **************************************************************************
 * Copyright (c) 2021, The Linux Foundation. All rights reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all copies.
 *
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
 * nss_udp_st.c
 *	NSS UDP_ST APIs
 */

#include "nss_core.h"
#include "nss_udp_st_stats.h"
#include "nss_udp_st_strings.h"
#include "nss_udp_st_log.h"

#define NSS_UDP_ST_TX_MSG_TIMEOUT 1000	/* 1 sec timeout for udp_st messages */

/*
 * Private data structure for udp_st configuration
 */
struct nss_udp_st_pvt {
	struct semaphore sem;		/* Semaphore structure */
	struct completion complete;	/* completion structure */
	int response;			/* Response from FW */
	void *cb;			/* Original cb for sync msgs */
	void *app_data;			/* Original app_data for sync msgs */
} nss_udp_st_pvt;

/*
 * nss_udp_st_msg_handler()
 *	Handle NSS -> HLOS messages for UDP_ST node
 */
static void nss_udp_st_msg_handler(struct nss_ctx_instance *nss_ctx, struct nss_cmn_msg *ncm, __attribute__((unused))void *app_data)
{
	struct nss_udp_st_msg *num = (struct nss_udp_st_msg *)ncm;
	nss_udp_st_msg_callback_t cb;

	/*
	 * Is this a valid message type?
	 */
	if (num->cm.type >= NSS_UDP_ST_MAX_MSG_TYPES) {
		nss_warning("%px: received invalid message %d for udp_st interface", nss_ctx, num->cm.type);
		return;
	}

	/*
	 * Log messages.
	 */
	nss_udp_st_log_rx_msg(num);

	switch (num->cm.type) {
	case NSS_UDP_ST_STATS_SYNC_MSG:
		/*
		 * Update driver statistics and send stats notifications to the registered modules.
		 */
		nss_udp_st_stats_sync(nss_ctx, &num->msg.stats);
		break;

	case NSS_UDP_ST_RESET_STATS_MSG:
		/*
		 * This is a response to the statistics reset message.
		 */
		nss_udp_st_stats_reset(NSS_UDP_ST_INTERFACE);
		break;
	default:
		if (ncm->response != NSS_CMN_RESPONSE_ACK) {
			/*
			 * Check response.
			 */
			nss_info("%px: Received response %d for type %d, interface %d",
						nss_ctx, ncm->response, ncm->type, ncm->interface);
		}
	}

	/*
	 * Return for NOTIFY messages because there is no notifier functions.
	 */
	if (num->cm.response == NSS_CMN_RESPONSE_NOTIFY) {
		return;
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
	cb = (nss_udp_st_msg_callback_t)ncm->cb;
	cb((void *)ncm->app_data, num);
}

/*
 * nss_udp_st_tx_sync_callback()
 *	Callback to handle the completion of synchronous tx messages.
 */
static void nss_udp_st_tx_sync_callback(void *app_data, struct nss_udp_st_msg *num)
{
	nss_udp_st_msg_callback_t callback = (nss_udp_st_msg_callback_t)nss_udp_st_pvt.cb;
	void *data = nss_udp_st_pvt.app_data;

	nss_udp_st_pvt.cb = NULL;
	nss_udp_st_pvt.app_data = NULL;

	if (num->cm.response != NSS_CMN_RESPONSE_ACK) {
		nss_warning("udp_st error response %d\n", num->cm.response);
		nss_udp_st_pvt.response = NSS_TX_FAILURE;
	} else {
		nss_udp_st_pvt.response = NSS_TX_SUCCESS;
	}

	if (callback) {
		callback(data, num);
	}

	complete(&nss_udp_st_pvt.complete);
}

/*
 * nss_udp_st_tx()
 *	Transmit a udp_st message to the FW.
 */
nss_tx_status_t nss_udp_st_tx(struct nss_ctx_instance *nss_ctx, struct nss_udp_st_msg *num)
{
	struct nss_cmn_msg *ncm = &num->cm;

	/*
	 * Sanity check the message
	 */
	if (ncm->interface != NSS_UDP_ST_INTERFACE) {
		nss_warning("%px: tx request for another interface: %d", nss_ctx, ncm->interface);
		return NSS_TX_FAILURE;
	}

	if (ncm->type >= NSS_UDP_ST_MAX_MSG_TYPES) {
		nss_warning("%px: message type out of range: %d", nss_ctx, ncm->type);
		return NSS_TX_FAILURE;
	}

	/*
	 * Trace messages.
	 */
	nss_udp_st_log_tx_msg(num);

	return nss_core_send_cmd(nss_ctx, num, sizeof(*num), NSS_NBUF_PAYLOAD_SIZE);
}
EXPORT_SYMBOL(nss_udp_st_tx);

/*
 * nss_udp_st_tx_sync()
 *	Transmit a synchronous udp_st message to the FW.
 */
nss_tx_status_t nss_udp_st_tx_sync(struct nss_ctx_instance *nss_ctx, struct nss_udp_st_msg *num)
{
	nss_tx_status_t status;
	int ret = 0;

	down(&nss_udp_st_pvt.sem);
	nss_udp_st_pvt.cb = (void *)num->cm.cb;
	nss_udp_st_pvt.app_data = (void *)num->cm.app_data;

	num->cm.cb = (nss_ptr_t)nss_udp_st_tx_sync_callback;
	num->cm.app_data = (nss_ptr_t)NULL;

	status = nss_udp_st_tx(nss_ctx, num);
	if (status != NSS_TX_SUCCESS) {
		nss_warning("%px: nss udp_st msg tx failed\n", nss_ctx);
		up(&nss_udp_st_pvt.sem);
		return status;
	}

	ret = wait_for_completion_timeout(&nss_udp_st_pvt.complete, msecs_to_jiffies(NSS_UDP_ST_TX_MSG_TIMEOUT));
	if (!ret) {
		nss_warning("%px: udp_st tx sync failed due to timeout\n", nss_ctx);
		nss_udp_st_pvt.response = NSS_TX_FAILURE;
	}

	status = nss_udp_st_pvt.response;
	up(&nss_udp_st_pvt.sem);
	return status;
}
EXPORT_SYMBOL(nss_udp_st_tx_sync);

/*
 * nss_udp_st_msg_init()
 *	Initialize udp_st message.
 */
void nss_udp_st_msg_init(struct nss_udp_st_msg *num, uint16_t if_num, uint32_t type, uint32_t len,
			nss_udp_st_msg_callback_t cb, void *app_data)
{
	nss_cmn_msg_init(&num->cm, if_num, type, len, (void *)cb, app_data);
}
EXPORT_SYMBOL(nss_udp_st_msg_init);

/*
 * nss_udp_st_register_handler()
 */
void nss_udp_st_register_handler(struct nss_ctx_instance *nss_ctx)
{
	nss_core_register_handler(nss_ctx, NSS_UDP_ST_INTERFACE, nss_udp_st_msg_handler, NULL);

	nss_udp_st_stats_dentry_create();
	nss_udp_st_strings_dentry_create();

	sema_init(&nss_udp_st_pvt.sem, 1);
	init_completion(&nss_udp_st_pvt.complete);
}

/*
 * nss_udp_st_get_mgr()
 *
 */
struct nss_ctx_instance *nss_udp_st_get_mgr(void)
{
	return (void *)&nss_top_main.nss[nss_top_main.udp_st_handler_id];
}
EXPORT_SYMBOL(nss_udp_st_get_mgr);

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
#include "nss_tunipip6_log.h"
#include "nss_tunipip6_stats.h"

#define NSS_TUNIPIP6_TX_TIMEOUT 3000

/*
 * Data structure used to handle sync message.
 */
static struct nss_tunipip6_pvt {
	struct semaphore sem;           /* Semaphore structure. */
	struct completion complete;     /* Completion structure. */
	int response;                   /* Response from FW. */
	void *cb;                       /* Original cb for msgs. */
	void *app_data;                 /* Original app_data for msgs. */
} tunipip6_pvt;

/*
 * nss_tunipip6_verify_if_num
 *	Verify the interface is a valid interface
 */
static bool nss_tunipip6_verify_if_num(uint32_t if_num)
{
	enum nss_dynamic_interface_type type;

	type = nss_dynamic_interface_get_type(nss_tunipip6_get_context(), if_num);

	switch (type) {
	case NSS_DYNAMIC_INTERFACE_TYPE_TUNIPIP6_INNER:
	case NSS_DYNAMIC_INTERFACE_TYPE_TUNIPIP6_OUTER:
		return true;
	default:
		return false;
	}
}

/*
 * nss_tunipip6_handler()
 *	Handle NSS -> HLOS messages for ipip6 tunnel
 */
static void nss_tunipip6_handler(struct nss_ctx_instance *nss_ctx, struct nss_cmn_msg *ncm, __attribute__((unused))void *app_data)
{
	struct nss_tunipip6_msg *ntm = (struct nss_tunipip6_msg *)ncm;
	void *ctx;
	nss_tunipip6_msg_callback_t cb;

	BUG_ON(!nss_tunipip6_verify_if_num(ncm->interface));

	/*
	 * Trace Messages
	 */
	nss_tunipip6_log_rx_msg(ntm);

	/*
	 * Is this a valid request/response packet?
	 */
	if (ncm->type >= NSS_TUNIPIP6_MAX) {
		nss_warning("%px: received invalid message %d for DS-Lite interface", nss_ctx, ncm->type);
		return;
	}

	if (nss_cmn_get_msg_len(ncm) > sizeof(struct nss_tunipip6_msg)) {
		nss_warning("%px: Length of message is greater than required: %d", nss_ctx, nss_cmn_get_msg_len(ncm));
		return;
	}

	switch (ntm->cm.type) {
	case NSS_TUNIPIP6_STATS_SYNC:
		/*
		 * Sync common node stats.
		 */
		nss_tunipip6_stats_sync(nss_ctx, ntm);
		break;
	}

	/*
	 * Update the callback and app_data for NOTIFY messages, tunipip6 sends all notify messages
	 * to the same callback/app_data.
	 */
	if (ncm->response == NSS_CMN_RESPONSE_NOTIFY) {
		ncm->cb = (nss_ptr_t)nss_ctx->nss_top->tunipip6_msg_callback;
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
	cb = (nss_tunipip6_msg_callback_t)ncm->cb;
	ctx = nss_ctx->subsys_dp_register[ncm->interface].ndev;

	/*
	 * call ipip6 tunnel callback
	 */
	if (!ctx) {
		 nss_warning("%px: Event received for DS-Lite tunnel interface %d before registration", nss_ctx, ncm->interface);
		return;
	}

	cb(ctx, ntm);
}

/*
 * nss_tunipip6_tx()
 * 	Transmit a tunipip6 message to NSSFW
 */
nss_tx_status_t nss_tunipip6_tx(struct nss_ctx_instance *nss_ctx, struct nss_tunipip6_msg *msg)
{
	struct nss_cmn_msg *ncm = &msg->cm;

	/*
	 * Trace Messages
	 */
	nss_tunipip6_log_tx_msg(msg);

	/*
	 * Sanity check the message
	 */
	if (!nss_tunipip6_verify_if_num(ncm->interface)) {
		nss_warning("%px: tx request for another interface: %d", nss_ctx, ncm->interface);
		return NSS_TX_FAILURE;
	}

	if (ncm->type > NSS_TUNIPIP6_MAX) {
		nss_warning("%px: message type out of range: %d", nss_ctx, ncm->type);
		return NSS_TX_FAILURE;
	}

	return nss_core_send_cmd(nss_ctx, msg, sizeof(*msg), NSS_NBUF_PAYLOAD_SIZE);
}
EXPORT_SYMBOL(nss_tunipip6_tx);

/*
 * nss_tunipip6_callback()
 *	Callback to handle the completion of NSS->HLOS messages.
 */
static void nss_tunipip6_callback(void *app_data, struct nss_tunipip6_msg *nclm)
{
	tunipip6_pvt.response = NSS_TX_SUCCESS;
	tunipip6_pvt.cb = NULL;
	tunipip6_pvt.app_data = NULL;

	if (nclm->cm.response != NSS_CMN_RESPONSE_ACK) {
		nss_warning("%px: tunipip6 Error response %d Error: %d\n", app_data, nclm->cm.response, nclm->cm.error);
		tunipip6_pvt.response = nclm->cm.response;
	}

	/*
	 * Write memory barrier.
	 */
	smp_wmb();
	complete(&tunipip6_pvt.complete);
}

/*
 * nss_tunipip6_tx_sync()
 * 	Transmit a tunipip6 message to NSSFW synchronously.
 */
nss_tx_status_t nss_tunipip6_tx_sync(struct nss_ctx_instance *nss_ctx, struct nss_tunipip6_msg *msg)
{
	nss_tx_status_t status;
	int ret;

	down(&tunipip6_pvt.sem);
	msg->cm.cb = (nss_ptr_t)nss_tunipip6_callback;
	msg->cm.app_data = (nss_ptr_t)NULL;

	status = nss_tunipip6_tx(nss_ctx, msg);
	if (status != NSS_TX_SUCCESS) {
		nss_warning("%px: tunipip6_tx_msg failed\n", nss_ctx);
		up(&tunipip6_pvt.sem);
		return status;
	}

	ret = wait_for_completion_timeout(&tunipip6_pvt.complete, msecs_to_jiffies(NSS_TUNIPIP6_TX_TIMEOUT));
	if (!ret) {
		nss_warning("%px: tunipip6 tx sync failed due to timeout\n", nss_ctx);
		tunipip6_pvt.response = NSS_TX_FAILURE;
	}

	status = tunipip6_pvt.response;
	up(&tunipip6_pvt.sem);
	return status;
}
EXPORT_SYMBOL(nss_tunipip6_tx_sync);

/*
 * **********************************
 *  Register/Unregister/Miscellaneous APIs
 * **********************************
 */

/*
 * nss_register_tunipip6_if()
 */
struct nss_ctx_instance *nss_register_tunipip6_if(uint32_t if_num,
			uint32_t dynamic_interface_type,
			nss_tunipip6_callback_t tunipip6_callback,
			nss_tunipip6_msg_callback_t event_callback,
			struct net_device *netdev,
			uint32_t features)
{
	struct nss_ctx_instance *nss_ctx = (struct nss_ctx_instance *)&nss_top_main.nss[nss_top_main.tunipip6_handler_id];

	nss_assert(nss_ctx);
	nss_assert(nss_tunipip6_verify_if_num(if_num));

	nss_ctx->subsys_dp_register[if_num].type = dynamic_interface_type;
	nss_top_main.tunipip6_msg_callback = event_callback;
	nss_core_register_subsys_dp(nss_ctx, if_num, tunipip6_callback, NULL, NULL, netdev, features);
	nss_core_register_handler(nss_ctx, if_num, nss_tunipip6_handler, NULL);

	return nss_ctx;
}
EXPORT_SYMBOL(nss_register_tunipip6_if);

/*
 * nss_unregister_tunipip6_if()
 */
void nss_unregister_tunipip6_if(uint32_t if_num)
{
	struct nss_ctx_instance *nss_ctx = (struct nss_ctx_instance *)&nss_top_main.nss[nss_top_main.tunipip6_handler_id];

	nss_assert(nss_ctx);
	nss_assert(nss_tunipip6_verify_if_num(if_num));

	nss_stats_reset_common_stats(if_num);
	nss_core_unregister_handler(nss_ctx, if_num);
	nss_core_unregister_subsys_dp(nss_ctx, if_num);

	nss_top_main.tunipip6_msg_callback = NULL;
}
EXPORT_SYMBOL(nss_unregister_tunipip6_if);

/*
 * nss_tunipip6_get_context()
 */
struct nss_ctx_instance *nss_tunipip6_get_context(void)
{
	return (struct nss_ctx_instance *)&nss_top_main.nss[nss_top_main.tunipip6_handler_id];
}
EXPORT_SYMBOL(nss_tunipip6_get_context);

/*
 * nss_tunipip6_register_handler()
 */
void nss_tunipip6_register_handler()
{
	struct nss_ctx_instance *nss_ctx = nss_tunipip6_get_context();

	nss_core_register_handler(nss_ctx, NSS_TUNIPIP6_INTERFACE, nss_tunipip6_handler, NULL);
	nss_tunipip6_stats_dentry_create();
	sema_init(&tunipip6_pvt.sem, 1);
	init_completion(&tunipip6_pvt.complete);
}

/*
 * nss_tunipip6_msg_init()
 *	Initialize nss_tunipip6 msg.
 */
void nss_tunipip6_msg_init(struct nss_tunipip6_msg *ntm, uint16_t if_num, uint32_t type,  uint32_t len, void *cb, void *app_data)
{
	nss_cmn_msg_init(&ntm->cm, if_num, type, len, cb, app_data);
}
EXPORT_SYMBOL(nss_tunipip6_msg_init);

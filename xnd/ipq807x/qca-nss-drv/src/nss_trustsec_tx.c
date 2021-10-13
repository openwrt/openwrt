/*
 **************************************************************************
 * Copyright (c) 2016-2018, 2020 The Linux Foundation. All rights reserved.
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
#include "nss_trustsec_tx_stats.h"
#include "nss_trustsec_tx_log.h"

#define NSS_TRUSTSEC_TX_TIMEOUT 3000 /* 3 Seconds */

/*
 * Private data structure for trustsec_tx interface
 */
static struct nss_trustsec_tx_pvt {
	struct semaphore sem;
	struct completion complete;
	int response;
} ttx;

/*
 * nss_trustsec_tx_handler()
 *	Handle NSS -> HLOS messages for trustsec_tx
 */
static void nss_trustsec_tx_handler(struct nss_ctx_instance *nss_ctx, struct nss_cmn_msg *ncm,
				__attribute__((unused))void *app_data)
{
	nss_trustsec_tx_msg_callback_t cb;
	struct nss_trustsec_tx_msg *npm = (struct nss_trustsec_tx_msg *)ncm;

	BUG_ON(ncm->interface != NSS_TRUSTSEC_TX_INTERFACE);

	/*
	 * Trace messages.
	 */
	nss_trustsec_tx_log_rx_msg(npm);

	/*
	 * Is this a valid request/response packet?
	 */
	if (ncm->type >= NSS_TRUSTSEC_TX_MSG_MAX) {
		nss_warning("%px: received invalid message %d for trustsec_tx interface", nss_ctx, ncm->type);
		return;
	}

	if (nss_cmn_get_msg_len(ncm) > sizeof(struct nss_trustsec_tx_msg)) {
		nss_warning("%px: message size incorrect: %d", nss_ctx, nss_cmn_get_msg_len(ncm));
		return;
	}

	/*
	 * Log failures
	 */
	nss_core_log_msg_failures(nss_ctx, ncm);

	switch (ncm->type) {
	case NSS_TRUSTSEC_TX_MSG_STATS_SYNC:
		/*
		 * Update trustsec_tx statistics.
		 */
		nss_trustsec_tx_stats_sync(nss_ctx, &npm->msg.stats_sync);
		break;
	}

	/*
	 * Update the callback and app_data for NOTIFY messages, trustsec_tx sends all notify messages
	 * to the same callback/app_data.
	 */
	if (ncm->response == NSS_CMN_RESPONSE_NOTIFY) {
		ncm->cb = (nss_ptr_t)nss_core_get_msg_handler(nss_ctx, ncm->interface);
		ncm->app_data = (nss_ptr_t)nss_ctx->subsys_dp_register[ncm->interface].ndev;
	}

	/*
	 * Do we have a call back
	 */
	if (!ncm->cb) {
		return;
	}

	/*
	 * callback
	 */
	cb = (nss_trustsec_tx_msg_callback_t)ncm->cb;

	cb((void *)ncm->app_data, npm);
}

/*
 * nss_trustsec_tx_msg()
 *	Transmit a trustsec_tx message to NSSFW
 */
nss_tx_status_t nss_trustsec_tx_msg(struct nss_ctx_instance *nss_ctx, struct nss_trustsec_tx_msg *msg)
{
	struct nss_cmn_msg *ncm = &msg->cm;

	/*
	 * Trace messages.
	 */
	nss_trustsec_tx_log_tx_msg(msg);

	/*
	 * Sanity check the message
	 */
	if (ncm->interface != NSS_TRUSTSEC_TX_INTERFACE) {
		nss_warning("%px: tx request for another interface: %d", nss_ctx, ncm->interface);
		return NSS_TX_FAILURE;
	}

	if (ncm->type > NSS_TRUSTSEC_TX_MSG_MAX) {
		nss_warning("%px: message type out of range: %d", nss_ctx, ncm->type);
		return NSS_TX_FAILURE;
	}

	return nss_core_send_cmd(nss_ctx, msg, sizeof(*msg), NSS_NBUF_PAYLOAD_SIZE);
}
EXPORT_SYMBOL(nss_trustsec_tx_msg);

/*
 * nss_trustsec_tx_callback
 *	Callback to handle the completion of NSS ->HLOS messages.
 */
static void nss_trustsec_tx_callback(void *app_data, struct nss_trustsec_tx_msg *npm)
{
	if (npm->cm.response != NSS_CMN_RESPONSE_ACK) {
		nss_warning("trustsec_tx error response %d\n", npm->cm.response);
		ttx.response = NSS_TX_FAILURE;
		complete(&ttx.complete);
		return;
	}

	ttx.response = NSS_TX_SUCCESS;
	complete(&ttx.complete);
}

/*
 * nss_trustsec_tx_msg_sync()
 *	Send a message to trustsec_tx interface & wait for the response.
 */
nss_tx_status_t nss_trustsec_tx_msg_sync(struct nss_ctx_instance *nss_ctx, struct nss_trustsec_tx_msg *msg)
{
	nss_tx_status_t status;
	int ret = 0;

	down(&ttx.sem);

	msg->cm.cb = (nss_ptr_t)nss_trustsec_tx_callback;
	msg->cm.app_data = (nss_ptr_t)NULL;

	status = nss_trustsec_tx_msg(nss_ctx, msg);
	if (status != NSS_TX_SUCCESS) {
		nss_warning("%px: nss_trustsec_tx_msg failed\n", nss_ctx);
		up(&ttx.sem);
		return status;
	}

	ret = wait_for_completion_timeout(&ttx.complete, msecs_to_jiffies(NSS_TRUSTSEC_TX_TIMEOUT));
	if (!ret) {
		nss_warning("%px: trustsec_tx tx failed due to timeout\n", nss_ctx);
		ttx.response = NSS_TX_FAILURE;
	}

	status = ttx.response;
	up(&ttx.sem);

	return status;
}
EXPORT_SYMBOL(nss_trustsec_tx_msg_sync);

/*
 * nss_trustsec_tx_get_ctx()
 *	Return a TrustSec TX NSS context.
 */
struct nss_ctx_instance *nss_trustsec_tx_get_ctx()
{
	return &nss_top_main.nss[nss_top_main.trustsec_tx_handler_id];
}
EXPORT_SYMBOL(nss_trustsec_tx_get_ctx);

/*
 * nss_trustsec_tx_msg_init()
 *	Initialize trustsec_tx message.
 */
void nss_trustsec_tx_msg_init(struct nss_trustsec_tx_msg *npm, uint16_t if_num, uint32_t type, uint32_t len,
				nss_trustsec_tx_msg_callback_t cb, void *app_data)
{
	nss_cmn_msg_init(&npm->cm, if_num, type, len, (void *)cb, app_data);
}
EXPORT_SYMBOL(nss_trustsec_tx_msg_init);

/*
 * nss_trustsec_tx_update_nexthop()
 */
nss_tx_status_t nss_trustsec_tx_update_nexthop(uint32_t src, uint32_t dest, uint16_t sgt)
{
	struct nss_ctx_instance *ctx = nss_trustsec_tx_get_ctx();
	struct nss_trustsec_tx_msg ttx_msg = {{0}};
	struct nss_trustsec_tx_update_nexthop_msg *ttxunh;
	nss_tx_status_t status;

	ttxunh = &ttx_msg.msg.upd_nexthop;
	ttxunh->src = src;
	ttxunh->dest = dest;
	ttxunh->sgt = sgt;

	nss_trustsec_tx_msg_init(&ttx_msg, NSS_TRUSTSEC_TX_INTERFACE, NSS_TRUSTSEC_TX_MSG_UPDATE_NEXTHOP,
			sizeof(*ttxunh), NULL, NULL);

	BUG_ON(in_atomic());
	status = nss_trustsec_tx_msg_sync(ctx, &ttx_msg);
	if (status != NSS_TX_SUCCESS) {
		nss_warning("%px: configure trustsec_tx failed: %d\n", ctx, status);
	}

	return status;
}
EXPORT_SYMBOL(nss_trustsec_tx_update_nexthop);

/*
 * nss_trustsec_tx_configure_sgt()
 */
nss_tx_status_t nss_trustsec_tx_configure_sgt(uint32_t src, uint32_t dest, uint16_t sgt)
{
	struct nss_ctx_instance *ctx = nss_trustsec_tx_get_ctx();
	struct nss_trustsec_tx_msg ttx_msg = {{0}};
	struct nss_trustsec_tx_configure_msg *ttxcfg;
	nss_tx_status_t status;

	ttxcfg = &ttx_msg.msg.configure;
	ttxcfg->src = src;
	ttxcfg->dest = dest;
	ttxcfg->sgt = sgt;

	nss_trustsec_tx_msg_init(&ttx_msg, NSS_TRUSTSEC_TX_INTERFACE, NSS_TRUSTSEC_TX_MSG_CONFIGURE,
			sizeof(*ttxcfg), NULL, NULL);

	BUG_ON(in_atomic());
	status = nss_trustsec_tx_msg_sync(ctx, &ttx_msg);
	if (status != NSS_TX_SUCCESS) {
		nss_warning("%px: configure trustsec_tx failed: %d\n", ctx, status);
	}

	return status;
}
EXPORT_SYMBOL(nss_trustsec_tx_configure_sgt);

/*
 * nss_trustsec_tx_unconfigure()
 */
nss_tx_status_t nss_trustsec_tx_unconfigure_sgt(uint32_t src, uint16_t sgt)
{
	struct nss_ctx_instance *ctx = nss_trustsec_tx_get_ctx();
	struct nss_trustsec_tx_msg ttx_msg = {{0}};
	struct nss_trustsec_tx_unconfigure_msg *ttxucfg;
	nss_tx_status_t status;

	ttxucfg = &ttx_msg.msg.unconfigure;
	ttxucfg->src = src;
	ttxucfg->sgt = sgt;

	nss_trustsec_tx_msg_init(&ttx_msg, NSS_TRUSTSEC_TX_INTERFACE, NSS_TRUSTSEC_TX_MSG_UNCONFIGURE,
			sizeof(*ttxucfg), NULL, NULL);

	BUG_ON(in_atomic());
	status = nss_trustsec_tx_msg_sync(ctx, &ttx_msg);
	if (status != NSS_TX_SUCCESS) {
		nss_warning("%px: unconfigure trustsec_tx failed: %d\n", ctx, status);
	}

	return status;
}
EXPORT_SYMBOL(nss_trustsec_tx_unconfigure_sgt);

/*
 * nss_trustsec_tx_register_handler()
 *	Registering handler for sending msg to trustsec_tx node on NSS.
 */
void nss_trustsec_tx_register_handler(void)
{
	struct nss_ctx_instance *nss_ctx = nss_trustsec_tx_get_ctx();

	nss_core_register_handler(nss_ctx, NSS_TRUSTSEC_TX_INTERFACE, nss_trustsec_tx_handler, NULL);

	nss_trustsec_tx_stats_dentry_create();

	sema_init(&ttx.sem, 1);
	init_completion(&ttx.complete);
}

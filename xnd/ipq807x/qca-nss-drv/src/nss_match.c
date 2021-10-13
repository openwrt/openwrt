/*
 ***************************************************************************
 * Copyright (c) 2020-2021, The Linux Foundation. All rights reserved.
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
 ***************************************************************************
 */

/*
 * nss_match.c
 */

#include "nss_tx_rx_common.h"
#include "nss_match_log.h"
#include "nss_match_stats.h"
#include "nss_match_strings.h"

#define NSS_MATCH_TX_TIMEOUT 1000	/* 1 Seconds */

/*
 * Private data structure for match interface
 */
static struct nss_match_pvt {
	struct semaphore sem;
	struct completion complete;
	int32_t response;
} match_pvt;

/*
 * nss_get_match_context()
 */
struct nss_ctx_instance *nss_match_get_context(void)
{
	return (struct nss_ctx_instance *)&nss_top_main.nss[nss_top_main.match_handler_id];
}
EXPORT_SYMBOL(nss_match_get_context);

/*
 * nss_match_verify_if_num()
 */
static bool nss_match_verify_if_num(uint32_t if_num)
{
	if (nss_is_dynamic_interface(if_num) == false) {
		return false;
	}

	if (nss_dynamic_interface_get_type(nss_match_get_context(), if_num)
		!= NSS_DYNAMIC_INTERFACE_TYPE_MATCH) {
		return false;
	}

	return true;
}

/*
 * nss_match_msg_sync_callback
 *	Callback to handle the completion of NSS to HLOS messages.
 */
static void nss_match_msg_sync_callback(void *app_data, struct nss_match_msg *matchm)
{
	match_pvt.response = NSS_TX_SUCCESS;

	if (matchm->cm.response != NSS_CMN_RESPONSE_ACK) {
		nss_warning("Match Error response %d\n", matchm->cm.response);
		match_pvt.response = NSS_TX_FAILURE;
	}

	complete(&match_pvt.complete);
}

/*
 * nss_match_msg_tx()
 *	Sends message to NSS.
 */
static nss_tx_status_t nss_match_msg_tx(struct nss_ctx_instance *nss_ctx, struct nss_match_msg *matchm)
{
	struct nss_cmn_msg *ncm = &matchm->cm;

	NSS_VERIFY_CTX_MAGIC(nss_ctx);

	/*
	 * Trace Messages
	 */
	nss_match_log_tx_msg(matchm);

	/*
	 * Sanity check the message
	 */
	if (!nss_match_verify_if_num(ncm->interface)) {
		nss_warning("%px: Tx request for non dynamic interface: %d", nss_ctx, ncm->interface);
		return NSS_TX_FAILURE;
	}

	if (ncm->type > NSS_MATCH_MSG_MAX) {
		nss_warning("%px: Message type out of range: %d", nss_ctx, ncm->type);
		return NSS_TX_FAILURE;
	}

	return nss_core_send_cmd(nss_ctx, matchm, sizeof(*matchm), NSS_NBUF_PAYLOAD_SIZE);
}

/*
 * nss_match_handler()
 *	Handle NSS to HLOS messages for Match node
 */
static void nss_match_handler(struct nss_ctx_instance *nss_ctx, struct nss_cmn_msg *ncm, __attribute__((unused))void *app_data)
{
	struct nss_match_msg *nem = (struct nss_match_msg *)ncm;

	nss_match_msg_sync_callback_t cb;
	void *ctx;

	NSS_VERIFY_CTX_MAGIC(nss_ctx);
	BUG_ON(!nss_match_verify_if_num(ncm->interface));

	/*
	 * Trace Messages
	 */
	nss_match_log_rx_msg(nem);

	/*
	 * Is this a valid request/response packet?
	 */
	if (nem->cm.type >= NSS_MATCH_MSG_MAX) {
		nss_warning("%px: Received invalid message %d for MATCH interface", nss_ctx, nem->cm.type);
		return;
	}

	if (nss_cmn_get_msg_len(ncm) > sizeof(struct nss_match_msg)) {
		nss_warning("%px: Unexpected message length: %d, on interface: %d",
				nss_ctx, nss_cmn_get_msg_len(ncm), ncm->interface);
		return;
	}

	switch (nem->cm.type) {
	case NSS_MATCH_STATS_SYNC:

		/*
		 * Update common node statistics
		 */
		nss_match_stats_sync(nss_ctx, nem);
		nss_match_stats_notify(nss_ctx, nem->cm.interface);
	}

	if (ncm->response == NSS_CMN_RESPONSE_NOTIFY) {
		ncm->cb = (nss_ptr_t)nss_core_get_msg_handler(nss_ctx, ncm->interface);
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
	cb = (nss_match_msg_sync_callback_t)ncm->cb;
	ctx = (void *)ncm->app_data;

	cb(ctx, nem);
}

/*
 * nss_match_msg_tx_sync()
 *	Send a message to match node and wait for the response.
 */
nss_tx_status_t nss_match_msg_tx_sync(struct nss_ctx_instance *nss_ctx, struct nss_match_msg *matchm)
{
	nss_tx_status_t status;
	int ret = 0;
	down(&match_pvt.sem);

	matchm->cm.cb = (nss_ptr_t)nss_match_msg_sync_callback;
	matchm->cm.app_data = (nss_ptr_t)NULL;

	status = nss_match_msg_tx(nss_ctx, matchm);
	if (status != NSS_TX_SUCCESS) {
		nss_warning("%px: nss_match_msg_tx failed\n", nss_ctx);
		up(&match_pvt.sem);
		return status;
	}

	ret = wait_for_completion_timeout(&match_pvt.complete, msecs_to_jiffies(NSS_MATCH_TX_TIMEOUT));
	if (!ret) {
		nss_warning("%px: MATCH tx failed due to timeout\n", nss_ctx);
		match_pvt.response = NSS_TX_FAILURE;
	}

	status = match_pvt.response;
	up(&match_pvt.sem);

	return status;
}
EXPORT_SYMBOL(nss_match_msg_tx_sync);

/*
 * nss_match_unregister_instance()
 *	Unregisters match instance.
 */
bool nss_match_unregister_instance(int if_num)
{
	struct nss_ctx_instance *nss_ctx;
	uint32_t status;

	nss_ctx = nss_match_get_context();
	NSS_VERIFY_CTX_MAGIC(nss_ctx);

	if (!nss_match_verify_if_num(if_num)) {
		nss_warning("%px: Incorrect interface number: %d", nss_ctx, if_num);
		return false;
	}

	nss_core_unregister_handler(nss_ctx, if_num);
	status = nss_core_unregister_msg_handler(nss_ctx, if_num);
	if (status != NSS_CORE_STATUS_SUCCESS) {
		nss_warning("%px: Not able to unregister handler for interface %d with NSS core\n", nss_ctx, if_num);
		return false;
	}

	nss_match_ifnum_delete(if_num);

	return true;
}
EXPORT_SYMBOL(nss_match_unregister_instance);

/*
 * nss_match_register_instance()
 *	Registers match instance.
 */
struct nss_ctx_instance *nss_match_register_instance(int if_num, nss_match_msg_sync_callback_t notify_cb)
{
	struct nss_ctx_instance *nss_ctx;
	uint32_t status;

	nss_ctx = nss_match_get_context();
	NSS_VERIFY_CTX_MAGIC(nss_ctx);

	if (!nss_match_verify_if_num(if_num)) {
		nss_warning("%px: Incorrect interface number: %d", nss_ctx, if_num);
		return NULL;
	}

	nss_core_register_handler(nss_ctx, if_num, nss_match_handler, NULL);
	status = nss_core_register_msg_handler(nss_ctx, if_num, (nss_if_rx_msg_callback_t)notify_cb);
	if (status != NSS_CORE_STATUS_SUCCESS) {
		nss_warning("%px: Not able to register handler for interface %d with NSS core\n", nss_ctx, if_num);
		return NULL;
	}

	if (!nss_match_ifnum_add(if_num)) {
		nss_warning("%px: Unable to add match inteface : %u\n", nss_ctx, if_num);
		nss_core_unregister_handler(nss_ctx, if_num);
		nss_core_unregister_msg_handler(nss_ctx, if_num);
		return NULL;
	}

	return nss_ctx;
}
EXPORT_SYMBOL(nss_match_register_instance);

/*
 * nss_match_msg_init()
 *	Initialize match message.
 */
void nss_match_msg_init(struct nss_match_msg *nmm, uint16_t if_num, uint32_t type, uint32_t len,
		nss_match_msg_sync_callback_t cb, void *app_data)
{
	nss_cmn_msg_init(&nmm->cm, if_num, type, len, (void*)cb, app_data);
}
EXPORT_SYMBOL(nss_match_msg_init);

/*
 * nss_match_init()
 * 	Initialize match.
 */
void nss_match_init()
{
	nss_match_stats_dentry_create();
	nss_match_strings_dentry_create();
	sema_init(&match_pvt.sem, 1);
	init_completion(&match_pvt.complete);
}

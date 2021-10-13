/*
 **************************************************************************
 * Copyright (c) 2017-2021, The Linux Foundation. All rights reserved.
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
#include "nss_dtls_cmn_log.h"
#include "nss_dtls_cmn_stats.h"
#include "nss_dtls_cmn_strings.h"

#define NSS_DTLS_CMN_TX_TIMEOUT 3000 /* 3 Seconds */
#define NSS_DTLS_CMN_INTERFACE_MAX_LONG BITS_TO_LONGS(NSS_MAX_NET_INTERFACES)

/*
 * Private data structure.
 */
static struct nss_dtls_cmn_pvt {
	struct semaphore sem;
	struct completion complete;
	enum nss_dtls_cmn_error resp;
	unsigned long if_map[NSS_DTLS_CMN_INTERFACE_MAX_LONG];
} dtls_cmn_pvt;

/*
 * nss_dtls_cmn_verify_ifnum()
 *	Verify if the interface number is a DTLS interface.
 */
static bool nss_dtls_cmn_verify_ifnum(struct nss_ctx_instance *nss_ctx, uint32_t if_num)
{
	enum nss_dynamic_interface_type type = nss_dynamic_interface_get_type(nss_ctx, if_num);

	if (type == NSS_DYNAMIC_INTERFACE_TYPE_DTLS_CMN_INNER)
		return true;

	if (type == NSS_DYNAMIC_INTERFACE_TYPE_DTLS_CMN_OUTER)
		return true;

	if (if_num == NSS_DTLS_INTERFACE)
		return true;

	return false;
}

/*
 * nss_dtls_cmn_handler()
 *	Handle NSS -> HLOS messages for dtls tunnel.
 */
static void nss_dtls_cmn_handler(struct nss_ctx_instance *nss_ctx, struct nss_cmn_msg *ncm, void *data)
{
	nss_dtls_cmn_msg_callback_t cb;
	void *app_data;

	NSS_VERIFY_CTX_MAGIC(nss_ctx);

	nss_trace("%px: handle event for interface num :%u", nss_ctx, ncm->interface);

	/*
	 * Is this a valid request/response packet?
	 */
	if (ncm->type >= NSS_DTLS_CMN_MSG_MAX) {
		nss_warning("%px:Bad message type(%d) for DTLS interface %d", nss_ctx, ncm->type, ncm->interface);
		return;
	}

	if (nss_cmn_get_msg_len(ncm) > sizeof(struct nss_dtls_cmn_msg)) {
		nss_warning("%px:Bad message length(%d)", nss_ctx, ncm->len);
		return;
	}

	if (ncm->type == NSS_DTLS_CMN_MSG_TYPE_SYNC_STATS) {
		nss_dtls_cmn_stats_sync(nss_ctx, ncm);
		nss_dtls_cmn_stats_notify(nss_ctx, ncm->interface);
	}

	/*
	 * Update the callback and app_data for NOTIFY messages.
	 */
	if (ncm->response == NSS_CMN_RESPONSE_NOTIFY) {
		ncm->cb = (nss_ptr_t)nss_core_get_msg_handler(nss_ctx, ncm->interface);
		ncm->app_data = (nss_ptr_t)nss_ctx->nss_rx_interface_handlers[ncm->interface].app_data;
	}

	/*
	 * Log failures.
	 */
	nss_core_log_msg_failures(nss_ctx, ncm);

	/*
	 * Trace messages.
	 */
	nss_dtls_cmn_log_rx_msg((struct nss_dtls_cmn_msg *)ncm);

	/*
	 * Callback.
	 */
	cb = (nss_dtls_cmn_msg_callback_t)ncm->cb;
	app_data = (void *)ncm->app_data;

	/*
	 * Call DTLS session callback.
	 */
	if (!cb) {
		nss_warning("%px: No callback for dtls session interface %d", nss_ctx, ncm->interface);
		return;
	}

	nss_trace("%px: calling dtlsmgr event handler(%u)", nss_ctx, ncm->interface);
	cb(app_data, ncm);
}

/*
 * nss_dtls_cmn_callback()
 *	Callback to handle the completion of NSS->HLOS messages.
 */
static void nss_dtls_cmn_callback(void *app_data, struct nss_cmn_msg *ncm)
{
	/*
	 * This callback is for synchronous operation. The caller sends its
	 * response pointer which needs to be loaded with the response
	 * data arriving from the NSS.
	 */
	enum nss_dtls_cmn_error *resp = (enum nss_dtls_cmn_error *)app_data;

	*resp = (ncm->response == NSS_CMN_RESPONSE_ACK) ?  NSS_DTLS_CMN_ERROR_NONE : ncm->error;
	complete(&dtls_cmn_pvt.complete);

	return;
}

/*
 * nss_dtls_cmn_ifmap_get()
 *	Return DTLS common active interfaces map.
 */
unsigned long *nss_dtls_cmn_ifmap_get(void)
{
	return dtls_cmn_pvt.if_map;
}

/*
 * nss_dtls_cmn_tx_buf()
 *	Transmit buffer over DTLS interface.
 */
nss_tx_status_t nss_dtls_cmn_tx_buf(struct sk_buff *skb, uint32_t if_num, struct nss_ctx_instance *nss_ctx)
{
	if (!nss_dtls_cmn_verify_ifnum(nss_ctx, if_num))
		return NSS_TX_FAILURE;

	return nss_core_send_packet(nss_ctx, skb, if_num, H2N_BIT_FLAG_VIRTUAL_BUFFER | H2N_BIT_FLAG_BUFFER_REUSABLE);
}
EXPORT_SYMBOL(nss_dtls_cmn_tx_buf);

/*
 * nss_dtls_cmn_tx_msg()
 *	Transmit a DTLS message to NSS firmware.
 */
nss_tx_status_t nss_dtls_cmn_tx_msg(struct nss_ctx_instance *nss_ctx, struct nss_dtls_cmn_msg *msg)
{
	struct nss_cmn_msg *ncm = &msg->cm;

	if (ncm->type >= NSS_DTLS_CMN_MSG_MAX) {
		nss_warning("%px: dtls message type out of range: %d", nss_ctx, ncm->type);
		return NSS_TX_FAILURE;
	}

	if (!nss_dtls_cmn_verify_ifnum(nss_ctx, ncm->interface)) {
		nss_warning("%px: dtls message interface is bad: %u", nss_ctx, ncm->interface);
		return NSS_TX_FAILURE;
	}

	/*
	 * Trace messages.
	 */
	nss_dtls_cmn_log_tx_msg(msg);

	return nss_core_send_cmd(nss_ctx, msg, sizeof(*msg), NSS_NBUF_PAYLOAD_SIZE);
}
EXPORT_SYMBOL(nss_dtls_cmn_tx_msg);

/*
 * nss_dtls_cmn_tx_msg_sync()
 *	Transmit a DTLS message to NSS firmware synchronously.
 */
nss_tx_status_t nss_dtls_cmn_tx_msg_sync(struct nss_ctx_instance *nss_ctx, uint32_t if_num,
					enum nss_dtls_cmn_msg_type type, uint16_t len,
					struct nss_dtls_cmn_msg *ndcm, enum nss_dtls_cmn_error *resp)
{
	struct nss_dtls_cmn_msg ndcm_local;
	nss_tx_status_t status;
	int ret;

	/*
	 * Length of the message should be the based on type.
	 */
	if (len > sizeof(ndcm_local.msg)) {
		nss_warning("%px: (%u)Bad message length(%u) for type (%d)", nss_ctx, if_num, len, type);
		return NSS_TX_FAILURE_TOO_LARGE;
	}

	/*
	 * Response buffer is a required for copying the response for message.
	 */
	if (!resp) {
		nss_warning("%px: (%u)Response buffer is empty, type(%d)", nss_ctx, if_num, type);
		return NSS_TX_FAILURE_BAD_PARAM;
	}

	/*
	 * TODO: this can be removed in future as we need to ensure that the response
	 * memory is only updated when the current outstanding request is waiting.
	 * This can be solved by introducing sequence no. in messages and only completing
	 * the message if the sequence no. matches. For now this is solved by passing
	 * a known memory dtls_cmn_pvt.resp.
	 */
	down(&dtls_cmn_pvt.sem);

	/*
	 * We need to copy the message content into the actual message
	 * to be sent to NSS.
	 */
	nss_dtls_cmn_msg_init(&ndcm_local, if_num, type, len, nss_dtls_cmn_callback, &dtls_cmn_pvt.resp);
	memcpy(&ndcm_local.msg, &ndcm->msg, len);

	status = nss_dtls_cmn_tx_msg(nss_ctx, &ndcm_local);
	if (status != NSS_TX_SUCCESS) {
		nss_warning("%px: dtls_tx_msg failed", nss_ctx);
		goto done;
	}

	ret = wait_for_completion_timeout(&dtls_cmn_pvt.complete, msecs_to_jiffies(NSS_DTLS_CMN_TX_TIMEOUT));
	if (!ret) {
		nss_warning("%px: DTLS msg tx failed due to timeout", nss_ctx);
		status = NSS_TX_FAILURE_NOT_READY;
		goto done;
	}

	/*
	 * Read memory barrier.
	 */
	smp_rmb();

	/*
	 * Copy the response received.
	 */
	*resp = dtls_cmn_pvt.resp;

	/*
	 * Only in case of non-error response we will
	 * indicate success.
	 */
	if (dtls_cmn_pvt.resp != NSS_DTLS_CMN_ERROR_NONE)
		status = NSS_TX_FAILURE;

done:
	up(&dtls_cmn_pvt.sem);
	return status;
}
EXPORT_SYMBOL(nss_dtls_cmn_tx_msg_sync);

/*
 * nss_dtls_cmn_notify_register()
 *	Register a handler for notification from NSS firmware.
 */
struct nss_ctx_instance *nss_dtls_cmn_notify_register(uint32_t if_num, nss_dtls_cmn_msg_callback_t ev_cb,
							void *app_data)
{
	struct nss_ctx_instance *nss_ctx = nss_dtls_cmn_get_context();
	uint32_t ret;

	BUG_ON(!nss_ctx);

	ret = nss_core_register_handler(nss_ctx, if_num, nss_dtls_cmn_handler, app_data);
	if (ret != NSS_CORE_STATUS_SUCCESS) {
		nss_warning("%px: unable to register event handler for interface(%u)", nss_ctx, if_num);
		return NULL;
	}

	ret = nss_core_register_msg_handler(nss_ctx, if_num, ev_cb);
	if (ret != NSS_CORE_STATUS_SUCCESS) {
		nss_core_unregister_handler(nss_ctx, if_num);
		nss_warning("%px: unable to register event handler for interface(%u)", nss_ctx, if_num);
		return NULL;
	}

	return nss_ctx;
}
EXPORT_SYMBOL(nss_dtls_cmn_notify_register);

/*
 * nss_dtls_cmn_notify_unregister()
 *	Unregister notification callback handler.
 */
void nss_dtls_cmn_notify_unregister(uint32_t if_num)
{
	struct nss_ctx_instance *nss_ctx = nss_dtls_cmn_get_context();
	uint32_t ret;

	BUG_ON(!nss_ctx);

	ret = nss_core_unregister_msg_handler(nss_ctx, if_num);
	if (ret != NSS_CORE_STATUS_SUCCESS) {
		nss_warning("%px: unable to unregister event handler for interface(%u)", nss_ctx, if_num);
		return;
	}

	ret = nss_core_unregister_handler(nss_ctx, if_num);
	if (ret != NSS_CORE_STATUS_SUCCESS) {
		nss_warning("%px: unable to unregister event handler for interface(%u)", nss_ctx, if_num);
		return;
	}

	return;
}
EXPORT_SYMBOL(nss_dtls_cmn_notify_unregister);

/*
 * nss_dtls_cmn_register_if()
 *	Register data and event callback handlers for dynamic interface.
 */
struct nss_ctx_instance *nss_dtls_cmn_register_if(uint32_t if_num,
						  nss_dtls_cmn_data_callback_t data_cb,
						  nss_dtls_cmn_msg_callback_t ev_cb,
						  struct net_device *netdev,
						  uint32_t features,
						  uint32_t type,
						  void *app_data)
{
	struct nss_ctx_instance *nss_ctx = nss_dtls_cmn_get_context();
	uint32_t ret;

	if (!nss_dtls_cmn_verify_ifnum(nss_ctx, if_num)) {
		nss_warning("%px: DTLS Interface is not dynamic:%u", nss_ctx, if_num);
		return NULL;
	}

	if (nss_ctx->subsys_dp_register[if_num].ndev) {
		nss_warning("%px: Cannot find free slot for DTLS NSS I/F:%u", nss_ctx, if_num);
		return NULL;
	}

	nss_core_register_subsys_dp(nss_ctx, if_num, data_cb, NULL, app_data, netdev, features);
	nss_ctx->subsys_dp_register[if_num].type = type;

	ret = nss_core_register_handler(nss_ctx, if_num, nss_dtls_cmn_handler, app_data);
	if (ret != NSS_CORE_STATUS_SUCCESS) {
		nss_warning("%px: unable to register event handler for interface(%u)", nss_ctx, if_num);
		return NULL;
	}

	ret = nss_core_register_msg_handler(nss_ctx, if_num, ev_cb);
	if (ret != NSS_CORE_STATUS_SUCCESS) {
		nss_core_unregister_handler(nss_ctx, if_num);
		nss_warning("%px: unable to register event handler for interface(%u)", nss_ctx, if_num);
		return NULL;
	}

	/*
	 * Atomically set the bitmap for the interface number.
	 */
	set_bit(if_num, dtls_cmn_pvt.if_map);
	return nss_ctx;
}
EXPORT_SYMBOL(nss_dtls_cmn_register_if);

/*
 * nss_dtls_cmn_unregister_if()
 *	Unregister data and event callback handlers for the interface.
 */
void nss_dtls_cmn_unregister_if(uint32_t if_num)
{
	struct nss_ctx_instance *nss_ctx = nss_dtls_cmn_get_context();
	uint32_t ret;

	if (!nss_ctx->subsys_dp_register[if_num].ndev) {
		nss_warning("%px: Cannot find registered netdev for DTLS NSS I/F:%u", nss_ctx, if_num);
		return;
	}

	/*
	 * Atomically clear the bitmap for the interface number.
	 */
	clear_bit(if_num, dtls_cmn_pvt.if_map);

	ret = nss_core_unregister_msg_handler(nss_ctx, if_num);
	if (ret != NSS_CORE_STATUS_SUCCESS) {
		nss_warning("%px: unable to unregister event handler for interface(%u)", nss_ctx, if_num);
		return;
	}

	nss_core_unregister_handler(nss_ctx, if_num);

	nss_core_unregister_subsys_dp(nss_ctx, if_num);
	nss_ctx->subsys_dp_register[if_num].type = 0;
}
EXPORT_SYMBOL(nss_dtls_cmn_unregister_if);

/*
 * nss_dtls_get_context()
 *	Return DTLS NSS context.
 */
struct nss_ctx_instance *nss_dtls_cmn_get_context(void)
{
	return (struct nss_ctx_instance *)&nss_top_main.nss[nss_top_main.dtls_handler_id];
}
EXPORT_SYMBOL(nss_dtls_cmn_get_context);

/*
 * nss_dtls_cmn_msg_init()
 *	Initialize nss_dtls_cmn msg.
 */
void nss_dtls_cmn_msg_init(struct nss_dtls_cmn_msg *ncm, uint32_t if_num,
		       uint32_t type, uint32_t len, void *cb, void *app_data)
{
	nss_cmn_msg_init(&ncm->cm, if_num, type, len, cb, app_data);
}
EXPORT_SYMBOL(nss_dtls_cmn_msg_init);

/*
 * nss_dtls_cmn_get_ifnum()
 *	Return DTLS interface number with coreid.
 */
int32_t nss_dtls_cmn_get_ifnum(int32_t if_num)
{
	struct nss_ctx_instance *nss_ctx = nss_dtls_cmn_get_context();

	NSS_VERIFY_CTX_MAGIC(nss_ctx);
	return NSS_INTERFACE_NUM_APPEND_COREID(nss_ctx, if_num);
}
EXPORT_SYMBOL(nss_dtls_cmn_get_ifnum);

/*
 * nss_dtls_cmn_register_handler()
 *	DTLS initialization.
 */
void nss_dtls_cmn_register_handler(void)
{
	sema_init(&dtls_cmn_pvt.sem, 1);
	init_completion(&dtls_cmn_pvt.complete);
	nss_dtls_cmn_stats_dentry_create();
	nss_dtls_cmn_strings_dentry_create();
}

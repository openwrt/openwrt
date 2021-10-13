/*
 **************************************************************************
 * Copyright (c) 2019-2021, The Linux Foundation. All rights reserved.
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
#include "nss_qvpn_stats.h"
#include "nss_qvpn_strings.h"
#include "nss_qvpn_log.h"

#define NSS_QVPN_TX_TIMEOUT 1000	/* 1 Second */
#define NSS_QVPN_INTERFACE_MAX_LONG BITS_TO_LONGS(NSS_MAX_NET_INTERFACES)	/**< QVPN interface mapping bits. */

/*
 * Private data structure
 */
static struct nss_qvpn_pvt {
	struct semaphore sem;
	struct completion complete;
	unsigned long if_map[NSS_QVPN_INTERFACE_MAX_LONG];
	enum nss_qvpn_error_type resp;
} qvpn_pvt;

/*
 * nss_qvpn_verify_if_num()
 *	Verify if_num passed to us.
 */
static bool nss_qvpn_verify_if_num(uint32_t if_num)
{
	enum nss_dynamic_interface_type if_type;

	if_type = nss_dynamic_interface_get_type(nss_qvpn_get_context(), if_num);
	if ((if_type != NSS_DYNAMIC_INTERFACE_TYPE_QVPN_INNER) &&
	    (if_type != NSS_DYNAMIC_INTERFACE_TYPE_QVPN_OUTER)) {
		nss_warning("%px: if_num = %u interface type returned is %d\n", nss_qvpn_get_context(), if_num, if_type);
		return false;
	}

	return true;
}

/*
 * nss_qvpn_handler()
 *	Handle NSS to HLOS messages for QVPN
 */
static void nss_qvpn_handler(struct nss_ctx_instance *nss_ctx, struct nss_cmn_msg *ncm, void *app_data)
{
	nss_qvpn_msg_callback_t cb;

	nss_assert(nss_qvpn_verify_if_num(ncm->interface));

	NSS_VERIFY_CTX_MAGIC(nss_ctx);

	/*
	 * Is this a valid request/response packet?
	 */
	if (ncm->type >= NSS_QVPN_MSG_TYPE_MAX) {
		nss_warning("%px: received invalid message %d for qvpn interface", nss_ctx, ncm->type);
		return;
	}

	if (nss_cmn_get_msg_len(ncm) > sizeof(struct nss_qvpn_msg)) {
		nss_warning("%px: length of message is greater than required: %d", nss_ctx, nss_cmn_get_msg_len(ncm));
		return;
	}

	/*
	 * Log failures
	 */

	nss_core_log_msg_failures(nss_ctx, ncm);
	/*
	 * Trace messages.
	 */
	nss_qvpn_log_rx_msg((struct nss_qvpn_msg *)ncm);

	if (ncm->type == NSS_QVPN_MSG_TYPE_SYNC_STATS) {
		nss_qvpn_stats_tunnel_sync(nss_ctx, ncm);
		nss_qvpn_stats_notify(nss_ctx, ncm->interface);
	}

	/*
	 * Update the callback and app_data for NOTIFY messages, qvpn sends all notify messages
	 * to the same callback/app_data.
	 */
	if (ncm->response == NSS_CMN_RESPONSE_NOTIFY) {
		ncm->cb = (nss_ptr_t)nss_core_get_msg_handler(nss_ctx, ncm->interface);
		ncm->app_data = (nss_ptr_t)nss_ctx->nss_rx_interface_handlers[ncm->interface].app_data;
	}

	/*
	 * load, test & call
	 */
	cb = (nss_qvpn_msg_callback_t)ncm->cb;
	if (unlikely(!cb)) {
		nss_trace("%px: rx handler unregistered for i/f: %u\n", nss_ctx, ncm->interface);
		return;
	}

	cb((void *)ncm->app_data, ncm);
}

/*
 * nss_qvpn_callback()
 *	Callback to handle the completion of NSS to HLOS messages.
 */
static void nss_qvpn_callback(void *app_data, struct nss_qvpn_msg *nvm)
{
	enum nss_qvpn_error_type *resp = (enum nss_qvpn_error_type *)app_data;

	*resp = (nvm->cm.response == NSS_CMN_RESPONSE_ACK) ?  NSS_QVPN_ERROR_TYPE_NONE : nvm->cm.error;

	/*
	 * Write memory barrier
	 */
	smp_wmb();

	complete(&qvpn_pvt.complete);
}

/*
 * nss_qvpn_ifmap_get()
 *	Return QVPN active interfaces map.
 */
unsigned long *nss_qvpn_ifmap_get(void)
{
	return qvpn_pvt.if_map;
}

/*
 * nss_qvpn_get_context()
 *	Return NSS QVPN context.
 */
struct nss_ctx_instance *nss_qvpn_get_context(void)
{
	return &nss_top_main.nss[nss_top_main.qvpn_handler_id];
}
EXPORT_SYMBOL(nss_qvpn_get_context);

/*
 * nss_qvpn_tx_msg()
 *	Transmit a QVPN message to NSS firmware
 */
nss_tx_status_t nss_qvpn_tx_msg(struct nss_ctx_instance *nss_ctx, struct nss_qvpn_msg *msg)
{
	struct nss_cmn_msg *ncm = &msg->cm;

	/*
	 * Sanity check the message
	 */
	if (!nss_qvpn_verify_if_num(ncm->interface)) {
		nss_warning("%px: tx request for interface that is not a qvpn: %u\n", nss_ctx, ncm->interface);
		return NSS_TX_FAILURE_BAD_PARAM;
	}

	if (ncm->type >= NSS_QVPN_MSG_TYPE_MAX) {
		nss_warning("%px: message type out of range: %d\n", nss_ctx, ncm->type);
		return NSS_TX_FAILURE_BAD_PARAM;
	}

	/*
	 * Trace messages.
	 */
	nss_qvpn_log_tx_msg(msg);

	return nss_core_send_cmd(nss_ctx, msg, sizeof(*msg), NSS_NBUF_PAYLOAD_SIZE);
}
EXPORT_SYMBOL(nss_qvpn_tx_msg);

/*
 * nss_qvpn_tx_msg_sync()
 *	Transmit a QVPN message to NSS firmware synchronously.
 */
nss_tx_status_t nss_qvpn_tx_msg_sync(struct nss_ctx_instance *nss_ctx, struct nss_qvpn_msg *nvm,
		uint32_t if_num, enum nss_qvpn_msg_type type, uint16_t len, enum nss_qvpn_error_type *resp)
{
	struct nss_qvpn_msg nqm;
	nss_tx_status_t status;
	int ret = 0;

	NSS_VERIFY_CTX_MAGIC(nss_ctx);

	if (len > sizeof(nqm.msg)) {
		nss_warning("%px: Incorrect message length=%u for type %d and if_num=%u\n", nss_ctx, len, type, if_num);
		return NSS_TX_FAILURE_TOO_LARGE;
	}

	if (!resp) {
		nss_warning("%px: Invalid input, resp=NULL\n", nss_ctx);
		return NSS_TX_FAILURE_BAD_PARAM;
	}

	nss_qvpn_msg_init(&nqm, if_num, type, len, nss_qvpn_callback, &qvpn_pvt.resp);
	memcpy(&nqm.msg, &nvm->msg, len);

	down(&qvpn_pvt.sem);

	status = nss_qvpn_tx_msg(nss_ctx, &nqm);
	if (status != NSS_TX_SUCCESS) {
		nss_warning("%px: qvpn_tx_msg failed\n", nss_ctx);
		goto done;
	}

	ret = wait_for_completion_timeout(&qvpn_pvt.complete, msecs_to_jiffies(NSS_QVPN_TX_TIMEOUT));
	if (!ret) {
		nss_warning("%px: qvpn msg tx failed due to timeout\n", nss_ctx);
		status = NSS_TX_FAILURE_SYNC_TIMEOUT;
		goto done;
	}

	/*
	 * Read memory barrier
	 */
	smp_rmb();

	*resp = qvpn_pvt.resp;
	if (*resp != NSS_QVPN_ERROR_TYPE_NONE)
		status = NSS_TX_FAILURE;
done:
	up(&qvpn_pvt.sem);
	return status;
}
EXPORT_SYMBOL(nss_qvpn_tx_msg_sync);

/*
 * nss_qvpn_tx_buf()
 *	Send packet to QVPN interface owned by NSS
 */
nss_tx_status_t nss_qvpn_tx_buf(struct nss_ctx_instance *nss_ctx, uint32_t if_num, struct sk_buff *skb)
{
	if (!nss_qvpn_verify_if_num(if_num)) {
		nss_warning("%px: tx request for interface that is not a qvpn: %u\n", nss_ctx, if_num);
		return NSS_TX_FAILURE_BAD_PARAM;
	}

	return nss_core_send_packet(nss_ctx, skb, if_num, H2N_BIT_FLAG_BUFFER_REUSABLE);
}
EXPORT_SYMBOL(nss_qvpn_tx_buf);

/*
 * nss_qvpn_msg_init()
 *	Initialize nss_qvpn_msg.
 */
void nss_qvpn_msg_init(struct nss_qvpn_msg *ncm, uint16_t if_num, uint32_t type, uint32_t len, void *cb, void *app_data)
{
	nss_cmn_msg_init(&ncm->cm, if_num, type, len, cb, app_data);
}
EXPORT_SYMBOL(nss_qvpn_msg_init);

/*
 * nss_qvpn_register_if()
 *	Register QVPN interface.
 */
struct nss_ctx_instance *nss_qvpn_register_if(uint32_t if_num, nss_qvpn_callback_t qvpn_data_callback,
					      nss_qvpn_msg_callback_t qvpn_event_callback,
					      struct net_device *netdev, uint32_t features, void *app_ctx)
{
	struct nss_ctx_instance *nss_ctx = nss_qvpn_get_context();
	uint32_t status;

	nss_assert(nss_ctx);
	nss_assert(nss_qvpn_verify_if_num(if_num));

	nss_core_register_subsys_dp(nss_ctx, if_num, qvpn_data_callback, NULL, app_ctx, netdev, features);
	nss_core_register_handler(nss_ctx, if_num, nss_qvpn_handler, app_ctx);
	status = nss_core_register_msg_handler(nss_ctx, if_num, qvpn_event_callback);
	if (status != NSS_CORE_STATUS_SUCCESS) {
		nss_warning("%px: Not able to register handler for interface %d with NSS core\n", nss_ctx, if_num);
		return NULL;
	}

	set_bit(if_num, qvpn_pvt.if_map);
	return nss_ctx;
}
EXPORT_SYMBOL(nss_qvpn_register_if);

/*
 * nss_unregister_qvpn_if()
 *	Unregister QVPN interface.
 */
void nss_qvpn_unregister_if(uint32_t if_num)
{
	struct nss_ctx_instance *nss_ctx = nss_qvpn_get_context();
	uint32_t status;

	nss_assert(nss_qvpn_verify_if_num(if_num));

	clear_bit(if_num, qvpn_pvt.if_map);

	nss_core_unregister_subsys_dp(nss_ctx, if_num);
	status = nss_core_unregister_msg_handler(nss_ctx, if_num);
	if (status != NSS_CORE_STATUS_SUCCESS) {
		nss_warning("%px: Failed to unregister handler for IPsec NSS I/F:%u\n", nss_ctx, if_num);
		return;
	}

	status = nss_core_unregister_handler(nss_ctx, if_num);
	if (status != NSS_CORE_STATUS_SUCCESS) {
		nss_warning("%px: Failed to unregister handler for IPsec NSS I/F:%u\n", nss_ctx, if_num);
		return;
	}
}
EXPORT_SYMBOL(nss_qvpn_unregister_if);

/*
 * nss_qvpn_ifnum_with_core_id()
 *	Append core id to QVPN interface number
 */
int nss_qvpn_ifnum_with_core_id(int if_num)
{
	struct nss_ctx_instance *nss_ctx = nss_qvpn_get_context();

	NSS_VERIFY_CTX_MAGIC(nss_ctx);
	if (nss_qvpn_verify_if_num(if_num) == false) {
		nss_info("%px: if_num: %u is not QVPN interface\n", nss_ctx, if_num);
		return 0;
	}
	return NSS_INTERFACE_NUM_APPEND_COREID(nss_ctx, if_num);
}
EXPORT_SYMBOL(nss_qvpn_ifnum_with_core_id);

/*
 * nss_qvpn_register_handler()
 *	Intialize QVPN driver and register handler.
 */
void nss_qvpn_register_handler(void)
{
	nss_info("nss_qvpn_register_handler\n");
	sema_init(&qvpn_pvt.sem, 1);
	init_completion(&qvpn_pvt.complete);
	nss_qvpn_stats_dentry_create();
	nss_qvpn_strings_dentry_create();
}

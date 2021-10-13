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

 /*
  * nss_clmap.c
  *	NSS clmap driver interface APIs
  */
#include "nss_core.h"
#include "nss_clmap.h"
#include "nss_cmn.h"
#include "nss_tx_rx_common.h"
#include "nss_clmap_stats.h"
#include "nss_clmap_strings.h"
#include "nss_clmap_log.h"

#define NSS_CLMAP_TX_TIMEOUT 3000

/*
 * Private data structure
 */
static struct nss_clmap_pvt {
	struct semaphore sem;		/* Semaphore structure. */
	struct completion complete;	/* Completion structure. */
	int response;			/* Response from FW. */
	void *cb;			/* Original cb for msgs. */
	void *app_data;			/* Original app_data for msgs. */
} clmap_pvt;

/*
 * nss_clmap_verify_if_num()
 *	Verify if_num passed to us.
 */
static bool nss_clmap_verify_if_num(uint32_t if_num)
{
	uint32_t type = nss_dynamic_interface_get_type(nss_clmap_get_ctx(), if_num);

	return ((type == NSS_DYNAMIC_INTERFACE_TYPE_CLMAP_US) ||
				(type == NSS_DYNAMIC_INTERFACE_TYPE_CLMAP_DS));
}

/*
 * nss_clmap_callback()
 *	Callback to handle the completion of NSS->HLOS messages.
 */
static void nss_clmap_callback(void *app_data, struct nss_clmap_msg *nclm)
{
	clmap_pvt.response = NSS_TX_SUCCESS;
	clmap_pvt.cb = NULL;
	clmap_pvt.app_data = NULL;

	if (nclm->cm.response != NSS_CMN_RESPONSE_ACK) {
		nss_warning("clmap Error response %d\n", nclm->cm.response);
		clmap_pvt.response = nclm->cm.response;
	}

	/*
	 * Write memory barrier.
	 */
	smp_wmb();
	complete(&clmap_pvt.complete);
}

/*
 * nss_clmap_handler()
 *	Handle NSS -> HLOS messages for clmap.
 */
static void nss_clmap_msg_handler(struct nss_ctx_instance *nss_ctx, struct nss_cmn_msg *ncm, __attribute__((unused))void *app_data)
{
	struct nss_clmap_msg *nclm = (struct nss_clmap_msg *)ncm;
	nss_clmap_msg_callback_t cb;

	BUG_ON(!nss_clmap_verify_if_num(ncm->interface));
	NSS_VERIFY_CTX_MAGIC(nss_ctx);

	/*
	 * Is this a valid request/response packet?
	 */
	if (ncm->type >= NSS_CLMAP_MSG_TYPE_MAX) {
		nss_warning("%px: received invalid message %d for clmap interface", nss_ctx, ncm->type);
		return;
	}

	if (nss_cmn_get_msg_len(ncm) > sizeof(struct nss_clmap_msg)) {
		nss_warning("%px: Length of message is greater than required: %d", nss_ctx, nss_cmn_get_msg_len(ncm));
		return;
	}

	/*
	 * Trace messages.
	 */
	nss_core_log_msg_failures(nss_ctx, ncm);
	nss_clmap_log_rx_msg(nclm);

	switch (nclm->cm.type) {
	case NSS_CLMAP_MSG_TYPE_SYNC_STATS:
		nss_clmap_stats_sync(nss_ctx, &nclm->msg.stats, ncm->interface);
		nss_clmap_stats_notify(nss_ctx, ncm->interface);
		break;
	}

	/*
	 * Update the callback and app_data for NOTIFY messages.
	 */
	if (ncm->response == NSS_CMN_RESPONSE_NOTIFY) {
		ncm->cb = (nss_ptr_t)nss_core_get_msg_handler(nss_ctx, ncm->interface);
		ncm->app_data = (nss_ptr_t)nss_ctx->nss_rx_interface_handlers[ncm->interface].app_data;
	}

	/*
	 * Do we have a callback
	 */
	cb = (nss_clmap_msg_callback_t)ncm->cb;
	if (!cb) {
		nss_trace("%px: cb is null for interface %d", nss_ctx, ncm->interface);
		return;
	}

	cb((void *)ncm->app_data, ncm);
}

/*
 * nss_clmap_tx_msg()
 *	Transmit a clmap message to NSS FW. Don't call this from softirq/interrupts.
 */
nss_tx_status_t nss_clmap_tx_msg(struct nss_ctx_instance *nss_ctx, struct nss_clmap_msg *msg)
{
	struct nss_cmn_msg *ncm = &msg->cm;

	if (!nss_clmap_verify_if_num(msg->cm.interface)) {
		return NSS_TX_FAILURE_BAD_PARAM;
	}

	if (ncm->type >= NSS_CLMAP_MSG_TYPE_MAX) {
		return NSS_TX_FAILURE_BAD_PARAM;
	}

	/*
	 * Trace messages.
	 */
	nss_clmap_log_tx_msg(msg);

	return nss_core_send_cmd(nss_ctx, msg, sizeof(*msg), NSS_NBUF_PAYLOAD_SIZE);
}
EXPORT_SYMBOL(nss_clmap_tx_msg);

/*
 * nss_clmap_tx_msg_sync()
 *	Transmit a clmap message to NSS firmware synchronously.
 */
nss_tx_status_t nss_clmap_tx_msg_sync(struct nss_ctx_instance *nss_ctx, struct nss_clmap_msg *nclm)
{
	nss_tx_status_t status;
	int ret;

	down(&clmap_pvt.sem);
	nclm->cm.cb = (nss_ptr_t)nss_clmap_callback;
	nclm->cm.app_data = (nss_ptr_t)NULL;

	status = nss_clmap_tx_msg(nss_ctx, nclm);
	if (status != NSS_TX_SUCCESS) {
		nss_warning("%px: clmap_tx_msg failed\n", nss_ctx);
		up(&clmap_pvt.sem);
		return status;
	}

	ret = wait_for_completion_timeout(&clmap_pvt.complete, msecs_to_jiffies(NSS_CLMAP_TX_TIMEOUT));
	if (!ret) {
		nss_warning("%px: clmap tx sync failed due to timeout\n", nss_ctx);
		clmap_pvt.response = NSS_TX_FAILURE;
	}

	status = clmap_pvt.response;
	up(&clmap_pvt.sem);
	return status;
}
EXPORT_SYMBOL(nss_clmap_tx_msg_sync);

/*
 * nss_clmap_tx_buf()
 *	Transmit data buffer (skb) to a NSS interface number
 */
nss_tx_status_t nss_clmap_tx_buf(struct nss_ctx_instance *nss_ctx, struct sk_buff *buf, uint32_t if_num)
{
	BUG_ON(!nss_clmap_verify_if_num(if_num));

	return nss_core_send_packet(nss_ctx, buf, if_num, H2N_BIT_FLAG_VIRTUAL_BUFFER);
}
EXPORT_SYMBOL(nss_clmap_tx_buf);

/*
 * nss_clmap_unregister()
 * 	Un-register a clmap interface from NSS.
 */
bool nss_clmap_unregister(uint32_t if_num)
{
	struct nss_ctx_instance *nss_ctx;
	int status;

	nss_ctx = nss_clmap_get_ctx();
	NSS_VERIFY_CTX_MAGIC(nss_ctx);

	if (!nss_clmap_verify_if_num(if_num)) {
		nss_warning("%px: clmap unregister request received for invalid interface %d", nss_ctx, if_num);
		return false;
	}

	status = nss_core_unregister_msg_handler(nss_ctx, if_num);
	if (status != NSS_CORE_STATUS_SUCCESS) {
		nss_warning("%px: Failed to unregister handler for clmap NSS I/F:%u\n", nss_ctx, if_num);
		return false;
	}

	nss_core_unregister_subsys_dp(nss_ctx, if_num);
	nss_core_unregister_handler(nss_ctx, if_num);
	nss_clmap_stats_session_unregister(if_num);

	return true;
}
EXPORT_SYMBOL(nss_clmap_unregister);

/*
 * nss_clmap_register()
 * 	Registers a clmap interface with the NSS.
 */
struct nss_ctx_instance *nss_clmap_register(uint32_t if_num,
						uint32_t di_type,
						nss_clmap_buf_callback_t data_cb,
						nss_clmap_msg_callback_t notify_cb,
						struct net_device *netdev,
						uint32_t features)
{
	struct nss_ctx_instance *nss_ctx;
	int core_status;
	bool stats_status = false;

	nss_ctx = nss_clmap_get_ctx();
	NSS_VERIFY_CTX_MAGIC(nss_ctx);

	if (!nss_clmap_verify_if_num(if_num)) {
		nss_warning("%px: clmap register request received for invalid interface %d", nss_ctx, if_num);
		goto fail;
	}

	if (di_type == NSS_DYNAMIC_INTERFACE_TYPE_CLMAP_US) {
		stats_status = nss_clmap_stats_session_register(if_num, NSS_CLMAP_INTERFACE_TYPE_US, netdev);
	} else {
		stats_status = nss_clmap_stats_session_register(if_num, NSS_CLMAP_INTERFACE_TYPE_DS, netdev);
	}

	if (!stats_status) {
		nss_warning("%px: statistics registration failed for interface: %d\n", nss_ctx, if_num);
		goto fail;
	}

	core_status = nss_core_register_handler(nss_ctx, if_num, nss_clmap_msg_handler, (void *)netdev);
	if (core_status != NSS_CORE_STATUS_SUCCESS) {
		goto core_reg_fail;
	}

	core_status = nss_core_register_msg_handler(nss_ctx, if_num, notify_cb);
	if (core_status != NSS_CORE_STATUS_SUCCESS) {
		goto msg_reg_fail;
	}

	nss_core_register_subsys_dp(nss_ctx, if_num, data_cb, NULL, (void *)netdev, netdev, features);
	nss_core_set_subsys_dp_type(nss_ctx, netdev, if_num, di_type);

	return nss_ctx;

msg_reg_fail:
	nss_core_unregister_handler(nss_ctx, if_num);
core_reg_fail:
	nss_clmap_stats_session_unregister(if_num);
	nss_warning("%px: NSS core register handler failed for if_num:%d with error :%d", nss_ctx, if_num, core_status);
fail:
	return NULL;

}
EXPORT_SYMBOL(nss_clmap_register);

/*
 * nss_clmap_ifnum_with_core_id()
 *	Append core ID to clmap interface num.
 */
int nss_clmap_ifnum_with_core_id(int if_num)
{
	struct nss_ctx_instance *nss_ctx = nss_clmap_get_ctx();

	NSS_VERIFY_CTX_MAGIC(nss_ctx);
	if (!nss_is_dynamic_interface(if_num)) {
		nss_warning("%px: Invalid if_num: %d, must be a dynamic interface\n", nss_ctx, if_num);
		return 0;
	}
	return NSS_INTERFACE_NUM_APPEND_COREID(nss_ctx, if_num);
}
EXPORT_SYMBOL(nss_clmap_ifnum_with_core_id);

/*
 * nss_clmap_msg_init()
 *	Initialize clmap message.
 */
void nss_clmap_msg_init(struct nss_clmap_msg *ncm, uint16_t if_num, uint32_t type, uint32_t len,
				nss_clmap_msg_callback_t cb, void *app_data)
{
	nss_cmn_msg_init(&ncm->cm, if_num, type, len, (void*)cb, app_data);
}
EXPORT_SYMBOL(nss_clmap_msg_init);

/*
 * nss_clmap_get_ctx()
 *	Return a clmap NSS context.
 */
struct nss_ctx_instance *nss_clmap_get_ctx()
{
	struct nss_ctx_instance *nss_ctx;

	nss_ctx = (struct nss_ctx_instance *)&nss_top_main.nss[nss_top_main.clmap_handler_id];
	return nss_ctx;
}
EXPORT_SYMBOL(nss_clmap_get_ctx);

/*
 * nss_clmap_init()
 *	Initializes clmap. Gets called from nss_init.c.
 */
void nss_clmap_init()
{
	sema_init(&clmap_pvt.sem, 1);
	init_completion(&clmap_pvt.complete);

	nss_clmap_stats_dentry_create();
	nss_clmap_strings_dentry_create();
}

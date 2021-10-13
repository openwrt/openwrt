/*
 **************************************************************************
 * Copyright (c) 2019-2020, The Linux Foundation. All rights reserved.
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
 * nss_vxlan.c
 *	NSS VxLAN driver interface APIs
 */
#include "nss_core.h"
#include "nss_vxlan.h"
#include "nss_cmn.h"
#include "nss_tx_rx_common.h"
#include "nss_vxlan_log.h"
#include "nss_vxlan_stats.h"

#define NSS_VXLAN_TX_TIMEOUT 3000

/*
 * Private data structure
 */
static struct {
	struct semaphore sem;		/* Semaphore structure. */
	struct completion complete;	/* Completion structure. */
	int response;			/* Response from FW. */
	void *cb;			/* Original cb for msgs. */
	void *app_data;			/* Original app_data for msgs. */
} nss_vxlan_pvt;

/*
 * nss_vxlan_verify_if_num()
 *	Verify if_num passed to us.
 */
static bool nss_vxlan_verify_if_num(uint32_t if_num)
{
	uint32_t type;

	if (if_num == NSS_VXLAN_INTERFACE) {
		return true;
	}

	type = nss_dynamic_interface_get_type(nss_vxlan_get_ctx(), if_num);

	return ((type == NSS_DYNAMIC_INTERFACE_TYPE_VXLAN_INNER) ||
			(type == NSS_DYNAMIC_INTERFACE_TYPE_VXLAN_OUTER));
}

/*
 * nss_vxlan_callback()
 *	Callback to handle the completion of NSS->HLOS messages.
 */
static void nss_vxlan_callback(void *app_data, struct nss_cmn_msg *msg)
{
	nss_vxlan_msg_callback_t callback = (nss_vxlan_msg_callback_t)nss_vxlan_pvt.cb;
	void *data = nss_vxlan_pvt.app_data;

	nss_vxlan_pvt.response = NSS_TX_SUCCESS;
	nss_vxlan_pvt.cb = NULL;
	nss_vxlan_pvt.app_data = NULL;

	if (msg->response != NSS_CMN_RESPONSE_ACK) {
		nss_warning("Vxlan Error response %d\n", msg->response);
		nss_vxlan_pvt.response = NSS_TX_FAILURE;
	}

	if (callback) {
		callback(data, msg);
	}
	complete(&nss_vxlan_pvt.complete);
}

/*
 * nss_vxlan_handler()
 *	Handle NSS -> HLOS messages for vxlan.
 */
static void nss_vxlan_msg_handler(struct nss_ctx_instance *nss_ctx, struct nss_cmn_msg *ncm, __attribute__((unused))void *app_data)
{
	struct nss_vxlan_msg *nvm = (struct nss_vxlan_msg *)ncm;
	nss_vxlan_msg_callback_t cb;

	BUG_ON(!nss_vxlan_verify_if_num(ncm->interface));

	/*
	 * Is this a valid request/response packet?
	 */
	if (ncm->type >= NSS_VXLAN_MSG_TYPE_MAX) {
		nss_warning("%px: received invalid message %d for vxlan interface", nss_ctx, ncm->type);
		return;
	}

	if (nss_cmn_get_msg_len(ncm) > sizeof(struct nss_vxlan_msg)) {
		nss_warning("%px: Length of message is greater than required: %d", nss_ctx, nss_cmn_get_msg_len(ncm));
		return;
	}

	/*
	 * Log messages.
	 */
	nss_core_log_msg_failures(nss_ctx, ncm);
	nss_vxlan_log_rx_msg(nvm);

	switch (nvm->cm.type) {
	case NSS_VXLAN_MSG_TYPE_STATS_SYNC:
		/*
		 * Update common node statistics
		 */
		nss_vxlan_stats_sync(nss_ctx, nvm);
	}

	/*
	 * Update the callback for NOTIFY messages
	 */
	if (ncm->response == NSS_CMN_RESPONSE_NOTIFY) {
		ncm->cb = (nss_ptr_t)nss_core_get_msg_handler(nss_ctx, ncm->interface);
	}

	cb = (nss_vxlan_msg_callback_t)ncm->cb;

	/*
	 * Do we have a callback?
	 */
	if (!cb) {
		nss_trace("%px: cb is null for interface %d\n", nss_ctx, ncm->interface);
		return;
	}

	cb((void *)nss_ctx->subsys_dp_register[ncm->interface].ndev, ncm);
}

/*
 * nss_vxlan_tx_msg()
 *	Transmit a vxlan message to NSS FW. Don't call this from softirq/interrupts.
 */
nss_tx_status_t nss_vxlan_tx_msg(struct nss_ctx_instance *nss_ctx, struct nss_vxlan_msg *nvm)
{
	struct nss_cmn_msg *ncm = &nvm->cm;

	if (!nss_vxlan_verify_if_num(ncm->interface)) {
		nss_warning("%px: wrong interface number %u\n", nss_ctx, nvm->cm.interface);
		return NSS_TX_FAILURE_BAD_PARAM;
	}

	if (ncm->type >= NSS_VXLAN_MSG_TYPE_MAX) {
		nss_warning("%px: wrong message type %u\n", nss_ctx, ncm->type);
		return NSS_TX_FAILURE_BAD_PARAM;
	}

	/*
	 * Trace messages.
	 */
	nss_vxlan_log_tx_msg(nvm);

	return nss_core_send_cmd(nss_ctx, nvm, sizeof(*nvm), NSS_NBUF_PAYLOAD_SIZE);
}
EXPORT_SYMBOL(nss_vxlan_tx_msg);

/*
 * nss_vxlan_tx_msg_sync()
 *	Transmit a vxlan message to NSS firmware synchronously.
 */
nss_tx_status_t nss_vxlan_tx_msg_sync(struct nss_ctx_instance *nss_ctx, struct nss_vxlan_msg *nvm)
{
	nss_tx_status_t status;
	int ret;

	down(&nss_vxlan_pvt.sem);
	nss_vxlan_pvt.cb = (void *)nvm->cm.cb;
	nss_vxlan_pvt.app_data = (void *)nvm->cm.app_data;

	nvm->cm.cb = (nss_ptr_t)nss_vxlan_callback;
	nvm->cm.app_data = (nss_ptr_t)NULL;

	status = nss_vxlan_tx_msg(nss_ctx, nvm);
	if (status != NSS_TX_SUCCESS) {
		nss_warning("%px: vxlan_tx_msg failed\n", nss_ctx);
		up(&nss_vxlan_pvt.sem);
		return status;
	}

	ret = wait_for_completion_timeout(&nss_vxlan_pvt.complete, msecs_to_jiffies(NSS_VXLAN_TX_TIMEOUT));
	if (!ret) {
		nss_warning("%px: vxlan tx sync failed due to timeout\n", nss_ctx);
		nss_vxlan_pvt.response = NSS_TX_FAILURE;
	}

	status = nss_vxlan_pvt.response;
	up(&nss_vxlan_pvt.sem);
	return status;
}
EXPORT_SYMBOL(nss_vxlan_tx_msg_sync);

/*
 * nss_vxlan_msg_init()
 *	Initialize VxLAN message.
 */
void nss_vxlan_msg_init(struct nss_vxlan_msg *nvm, uint16_t if_num, uint32_t type, uint32_t len,
				nss_vxlan_msg_callback_t cb, void *app_data)
{
	nss_cmn_msg_init(&nvm->cm, if_num, type, len, (void*)cb, app_data);
}
EXPORT_SYMBOL(nss_vxlan_msg_init);

/*
 * nss_vxlan_unregister_if()
 *	Unregister a data packet notifier with NSS FW.
 */
bool nss_vxlan_unregister_if(uint32_t if_num)
{
	struct nss_ctx_instance *nss_ctx;

	nss_ctx = nss_vxlan_get_ctx();
	if (!nss_vxlan_verify_if_num(if_num)) {
		nss_warning("%px: data unregister received for invalid interface %d", nss_ctx, if_num);
		return false;
	}

	nss_core_unregister_handler(nss_ctx, if_num);
	nss_core_unregister_subsys_dp(nss_ctx, if_num);
	return true;
}
EXPORT_SYMBOL(nss_vxlan_unregister_if);

/*
 * nss_vxlan_register_if()
 *	Registers a data packet notifier with NSS FW.
 */
struct nss_ctx_instance *nss_vxlan_register_if(uint32_t if_num,
						uint32_t type,
						nss_vxlan_buf_callback_t data_cb,
						nss_vxlan_msg_callback_t notify_cb,
						struct net_device *netdev,
						uint32_t features)
{
	struct nss_ctx_instance *nss_ctx;
	int core_status;

	nss_ctx = nss_vxlan_get_ctx();
	if (!nss_vxlan_verify_if_num(if_num)) {
		nss_warning("%px: data register received for invalid interface %d", nss_ctx, if_num);
		return NULL;
	}

	core_status = nss_core_register_handler(nss_ctx, if_num, nss_vxlan_msg_handler, NULL);
	if (core_status != NSS_CORE_STATUS_SUCCESS) {
		nss_warning("%px: nss core register handler failed for if_num:%d with error :%d", nss_ctx, if_num, core_status);
		return NULL;
	}

	core_status = nss_core_register_msg_handler(nss_ctx, if_num, notify_cb);
	if (core_status != NSS_CORE_STATUS_SUCCESS) {
		nss_core_unregister_handler(nss_ctx, if_num);
		nss_warning("%px: nss core register handler failed for if_num:%d with error :%d", nss_ctx, if_num, core_status);
		return NULL;
	}

	nss_core_register_subsys_dp(nss_ctx, if_num, data_cb, NULL, NULL, netdev, features);
	nss_core_set_subsys_dp_type(nss_ctx, netdev, if_num, type);
	return nss_ctx;
}
EXPORT_SYMBOL(nss_vxlan_register_if);

/*
 * nss_vxlan_ifnum_with_core_id()
 *	Append core id to vxlan interface num.
 */
int nss_vxlan_ifnum_with_core_id(int if_num)
{
	struct nss_ctx_instance *nss_ctx = nss_vxlan_get_ctx();

	NSS_VERIFY_CTX_MAGIC(nss_ctx);

	if (!nss_vxlan_verify_if_num(if_num)) {
		nss_warning("%px: Invalid if_num: %d, must be a dynamic interface\n", nss_ctx, if_num);
		return 0;
	}
	return NSS_INTERFACE_NUM_APPEND_COREID(nss_ctx, if_num);
}
EXPORT_SYMBOL(nss_vxlan_ifnum_with_core_id);

/*
 * nss_vxlan_get_ctx()
 *	Return a VxLAN NSS context.
 */
struct nss_ctx_instance *nss_vxlan_get_ctx()
{
	struct nss_ctx_instance *nss_ctx;

	nss_ctx = &nss_top_main.nss[nss_top_main.vxlan_handler_id];
	return nss_ctx;
}
EXPORT_SYMBOL(nss_vxlan_get_ctx);

/*
 * nss_vxlan_init()
 *	Initializes Vxlan. Gets called from nss_init.c.
 */
void nss_vxlan_init()
{
	uint32_t core_status;
	struct nss_ctx_instance *nss_ctx = nss_vxlan_get_ctx();
	if (!nss_ctx) {
		nss_warning("%px: VxLAN is not registered", nss_ctx);
		return;
	}

	nss_vxlan_stats_dentry_create();
	sema_init(&nss_vxlan_pvt.sem, 1);
	init_completion(&nss_vxlan_pvt.complete);
	core_status = nss_core_register_handler(nss_ctx, NSS_VXLAN_INTERFACE, nss_vxlan_msg_handler, NULL);

	if (core_status != NSS_CORE_STATUS_SUCCESS) {
		nss_warning("%px: nss core register handler failed for if_num:%d with error :%d", nss_ctx, NSS_VXLAN_INTERFACE, core_status);
	}

}

/*
 **************************************************************************
 * Copyright (c) 2020-2021, The Linux Foundation. All rights reserved.
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

#include "nss_tx_rx_common.h"
#include "nss_mirror_stats.h"
#include "nss_mirror_strings.h"
#include "nss_mirror_log.h"

#define NSS_MIRROR_TX_TIMEOUT 3000 /* 3 Seconds */

/*
 * Private data structure
 */
static struct {
	struct semaphore sem;
	struct completion complete;
	int response;
} nss_mirror_pvt;

atomic_t nss_mirror_num_instances;		/* Number of active mirror stats instances. */

/*
 * nss_mirror_verify_if_num()
 *	Verify interface number passed to us.
 */
bool nss_mirror_verify_if_num(uint32_t if_num)
{
	enum nss_dynamic_interface_type if_type;

	if_type = nss_dynamic_interface_get_type(nss_mirror_get_context(), if_num);
	if (if_type == NSS_DYNAMIC_INTERFACE_TYPE_MIRROR) {
		return true;
	}

	return false;
}
EXPORT_SYMBOL(nss_mirror_verify_if_num);

/*
 * nss_mirror_handler()
 *	Handle NSS -> HLOS messages for mirror device.
 */
static void nss_mirror_handler(struct nss_ctx_instance *nss_ctx, struct nss_cmn_msg *ncm,
		 void *app_data)
{
	struct nss_mirror_msg *nmm = (struct nss_mirror_msg *)ncm;
	void *ctx;
	nss_mirror_msg_callback_t cb;

	NSS_VERIFY_CTX_MAGIC(nss_ctx);
	nss_assert(nss_mirror_verify_if_num(ncm->interface));

	/*
	 * Is this a valid request/response packet?
	 */
	if (ncm->type >= NSS_MIRROR_MSG_MAX) {
		nss_warning("%px: received invalid message %d for mirror interface", nss_ctx, ncm->type);
		return;
	}

	if (nss_cmn_get_msg_len(ncm) > sizeof(struct nss_mirror_msg)) {
		nss_warning("%px: Length of message is greater than expected.", nss_ctx);
		return;
	}

	/*
	 * Log messages.
	 */
	nss_core_log_msg_failures(nss_ctx, ncm);
	nss_mirror_log_rx_msg(nmm);

	switch (ncm->type) {
	case NSS_MIRROR_MSG_SYNC_STATS:
		/*
		 * Debug stats embedded in stats msg.
		 */
		nss_mirror_stats_sync(nss_ctx, nmm, ncm->interface);
		nss_mirror_stats_notify(nss_ctx, ncm->interface);
		break;
	}

	/*
	 * Update the callback and app_data for NOTIFY messages.
	 */
	if (ncm->response == NSS_CMN_RESPONSE_NOTIFY) {
		ncm->cb = (nss_ptr_t)nss_core_get_msg_handler(nss_ctx, ncm->interface);
		ncm->app_data = (nss_ptr_t)app_data;
	}

	/*
	 * Callback.
	 */
	cb = (nss_mirror_msg_callback_t)ncm->cb;
	ctx = (void *)ncm->app_data;

	/*
	 * Call mirror interface callback.
	 */
	if (!cb) {
		nss_warning("%px: No callback for mirror interface %d",
			    nss_ctx, ncm->interface);
		return;
	}

	cb(ctx, ncm);
}

/*
 * nss_mirror_callback()
 *	Callback to handle the completion of NSS->HLOS messages.
 */
static void nss_mirror_callback(void *app_data, struct nss_cmn_msg *ncm)
{
	nss_mirror_pvt.response = NSS_TX_SUCCESS;

	if (ncm->response != NSS_CMN_RESPONSE_ACK) {
		nss_warning("mirror interface error response %d\n", ncm->response);
		nss_mirror_pvt.response = NSS_TX_FAILURE;
	}

	/*
	 * Write memory barrier.
	 */
	smp_wmb();
	complete(&nss_mirror_pvt.complete);
}

/*
 * nss_mirror_tx_msg()
 *	Transmit a mirror interface message to NSS firmware.
 */
nss_tx_status_t nss_mirror_tx_msg(struct nss_ctx_instance *nss_ctx, struct nss_mirror_msg *msg)
{
	struct nss_cmn_msg *ncm = &msg->cm;

	/*
	 * Sanity check the message.
	 */
	if (!nss_mirror_verify_if_num(ncm->interface)) {
		nss_warning("%px: tx request for non mirror interface: %d", nss_ctx, ncm->interface);
		return NSS_TX_FAILURE;
	}

	if (ncm->type > NSS_MIRROR_MSG_MAX) {
		nss_warning("%px: message type out of range: %d", nss_ctx, ncm->type);
		return NSS_TX_FAILURE;
	}

	/*
	 * Trace messages.
	 */
	nss_mirror_log_tx_msg(msg);

	return nss_core_send_cmd(nss_ctx, msg, sizeof(*msg), NSS_NBUF_PAYLOAD_SIZE);
}
EXPORT_SYMBOL(nss_mirror_tx_msg);

/*
 * nss_mirror_tx_msg_sync()
 *	Transmit a mirror interface message to NSS firmware synchronously.
 */
nss_tx_status_t nss_mirror_tx_msg_sync(struct nss_ctx_instance *nss_ctx, struct nss_mirror_msg *msg)
{
	nss_tx_status_t status;
	int ret = 0;

	down(&nss_mirror_pvt.sem);
	msg->cm.cb = (nss_ptr_t)nss_mirror_callback;
	msg->cm.app_data = (nss_ptr_t)NULL;

	status = nss_mirror_tx_msg(nss_ctx, msg);
	if (status != NSS_TX_SUCCESS) {
		nss_warning("%px: mirror_tx_msg failed\n", nss_ctx);
		up(&nss_mirror_pvt.sem);
		return status;
	}

	ret = wait_for_completion_timeout(&nss_mirror_pvt.complete, msecs_to_jiffies(NSS_MIRROR_TX_TIMEOUT));
	if (!ret) {
		nss_warning("%px: Mirror interface tx sync failed due to timeout\n", nss_ctx);
		nss_mirror_pvt.response = NSS_TX_FAILURE;
	}

	status = nss_mirror_pvt.response;
	up(&nss_mirror_pvt.sem);
	return status;
}
EXPORT_SYMBOL(nss_mirror_tx_msg_sync);

/*
 * nss_mirror_unregister_if()
 *	Un-registers mirror interface from the NSS.
 */
void nss_mirror_unregister_if(uint32_t if_num)
{
	struct nss_ctx_instance *nss_ctx = (struct nss_ctx_instance *)&nss_top_main.nss[nss_top_main.mirror_handler_id];
	uint32_t status;

	nss_assert(nss_ctx);
	nss_assert(nss_mirror_verify_if_num(if_num));

	nss_core_unregister_subsys_dp(nss_ctx, if_num);

	nss_core_unregister_handler(nss_ctx, if_num);

	status = nss_core_unregister_msg_handler(nss_ctx, if_num);
	if (status != NSS_CORE_STATUS_SUCCESS) {
		nss_warning("%px: Not able to unregister handler for interface %d with NSS core\n", nss_ctx, if_num);
	}

	atomic_dec(&nss_mirror_num_instances);
	nss_mirror_stats_reset(if_num);
}
EXPORT_SYMBOL(nss_mirror_unregister_if);

/*
 * nss_mirror_register_if()
 *	Registers the mirror interface with NSS.
 */
struct nss_ctx_instance *nss_mirror_register_if(uint32_t if_num,
		 nss_mirror_data_callback_t data_callback,
		 nss_mirror_msg_callback_t event_callback,
		 struct net_device *netdev, uint32_t features)
{
	struct nss_ctx_instance *nss_ctx = (struct nss_ctx_instance *)&nss_top_main.nss[nss_top_main.mirror_handler_id];
	int ret;

	nss_assert(nss_ctx);
	nss_assert(netdev);
	nss_assert(nss_mirror_verify_if_num(if_num));

	if (atomic_read(&nss_mirror_num_instances) == NSS_MAX_MIRROR_DYNAMIC_INTERFACES) {
		nss_warning("%px: Maximum number of mirror interfaces are already allocated\n", nss_ctx);
		return NULL;
	}

	ret = nss_mirror_stats_init(if_num, netdev);
	if (ret < 0) {
		nss_warning("%px: Error in initializaing mirror stats.\n", nss_ctx);
		return NULL;
	}

	nss_core_register_handler(nss_ctx, if_num, nss_mirror_handler, netdev);
	ret = nss_core_register_msg_handler(nss_ctx, if_num, event_callback);
	if (ret != NSS_CORE_STATUS_SUCCESS) {
		nss_core_unregister_handler(nss_ctx, if_num);
		nss_warning("%px: Not able to register handler for mirror interface %d with NSS core\n", nss_ctx, if_num);
		return NULL;
	}

	nss_core_register_subsys_dp(nss_ctx, if_num, data_callback, NULL, NULL, netdev, features);
	nss_core_set_subsys_dp_type(nss_ctx, netdev, if_num, NSS_DYNAMIC_INTERFACE_TYPE_MIRROR);

	atomic_inc(&nss_mirror_num_instances);
	return nss_ctx;
}
EXPORT_SYMBOL(nss_mirror_register_if);

/*
 * nss_mirror_get_context()
 *	Get the mirror instance context.
 */
struct nss_ctx_instance *nss_mirror_get_context(void)
{
	return (struct nss_ctx_instance *)&nss_top_main.nss[nss_top_main.mirror_handler_id];
}
EXPORT_SYMBOL(nss_mirror_get_context);

/*
 * nss_mirror_register_handler()
 *	Initialize and register mirror instance handler.
 */
void nss_mirror_register_handler(void)
{
	nss_info("nss_mirror_register_handler");
	sema_init(&nss_mirror_pvt.sem, 1);
	init_completion(&nss_mirror_pvt.complete);

	nss_mirror_stats_dentry_create();
	nss_mirror_strings_dentry_create();
}

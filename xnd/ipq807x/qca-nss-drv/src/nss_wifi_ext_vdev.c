/*
 **************************************************************************
 * Copyright (c) 2020, The Linux Foundation. All rights reserved.
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
 **************************************************************************
 */

#include "nss_tx_rx_common.h"
#include "nss_wifi_ext_vdev_stats.h"
#include "nss_wifi_ext_vdev_log.h"

#define NSS_WIFI_EXT_VDEV_TX_TIMEOUT 3000 /* 3 seconds */

/*
 * Private data structure
 */
static struct nss_wifi_ext_vdev_pvt {
	struct semaphore sem;
	struct completion complete;
	int response;
	void *cb;
	void *app_data;
} wifi_ext_vdev_pvt;

/*
 * nss_wifi_ext_vdev_verify_if_num()
 *	Verify if_num passed to us.
 */
static bool nss_wifi_ext_vdev_verify_if_num(uint32_t if_num)
{
	uint32_t type = nss_dynamic_interface_get_type(nss_wifi_ext_vdev_get_ctx(), if_num);

	switch (type) {
	case NSS_DYNAMIC_INTERFACE_TYPE_WIFI_EXT_VDEV_WDS:
	case NSS_DYNAMIC_INTERFACE_TYPE_WIFI_EXT_VDEV_VLAN:
		return true;
	default:
		return false;
	}
}

/*
 * nss_wifi_ext_vdev_handler()
 *	Handle NSS -> HLOS messages for wifi_ext_vdev
 */
static void nss_wifi_ext_vdev_handler(struct nss_ctx_instance *nss_ctx, struct nss_cmn_msg *ncm, void *app_data)
{
	struct nss_wifi_ext_vdev_msg *nwevm = (struct nss_wifi_ext_vdev_msg *)ncm;
	void *ctx;

	nss_wifi_ext_vdev_msg_callback_t cb;

	NSS_VERIFY_CTX_MAGIC(nss_ctx);
	BUG_ON(!nss_wifi_ext_vdev_verify_if_num(ncm->interface));

	/*
	 * Trace Messages
	 */
	nss_wifi_ext_vdev_log_rx_msg(nwevm);

	/*
	 * Is this a valid request/response packet?
	 */
	if (ncm->type >= NSS_WIFI_EXT_VDEV_MSG_MAX) {
		nss_warning("%px: received invalid message %d for WiFi extended VAP interface %d", nss_ctx, ncm->type, ncm->interface);
		return;
	}

	if (nss_cmn_get_msg_len(ncm) > sizeof(struct nss_wifi_ext_vdev_msg)) {
		nss_warning("%px: wifi_ext_vdev message length is invalid: %d", nss_ctx, ncm->len);
		return;
	}

	/*
	 * Check messages
	 */
	switch (nwevm->cm.type) {
	case NSS_WIFI_EXT_VDEV_MSG_STATS_SYNC:
		nss_wifi_ext_vdev_stats_sync(nss_ctx, &nwevm->msg.stats, ncm->interface);
		break;
	}

	/*
	 * Update the callback and app_data for NOTIFY messages
	 */
	if (ncm->response == NSS_CMN_RESPONSE_NOTIFY) {
		ncm->cb = (nss_ptr_t)nss_core_get_msg_handler(nss_ctx, ncm->interface);
		ncm->app_data = (nss_ptr_t)nss_ctx->subsys_dp_register[ncm->interface].app_data;
	}

	nss_core_log_msg_failures(nss_ctx, ncm);

	/*
	 * callback
	 */
	cb = (nss_wifi_ext_vdev_msg_callback_t)ncm->cb;
	ctx = (void *)ncm->app_data;

	/*
	 * call the callback
	 */
	if (!cb) {
		return;
	}

	cb(ctx, ncm);
}

/*
 * nss_wifi_ext_vdev_msg_init()
 *	Initialize wifi message.
 */
void nss_wifi_ext_vdev_msg_init(struct nss_wifi_ext_vdev_msg *nim, uint32_t if_num,
			uint32_t type, uint32_t len,
			nss_wifi_ext_vdev_msg_callback_t cb, void *app_data)
{
	nss_cmn_msg_init(&nim->cm, if_num, type, len, cb, app_data);
}
EXPORT_SYMBOL(nss_wifi_ext_vdev_msg_init);

/*
 * nss_wifi_ext_vdev_tx_msg()
 *	Transmit a wifi vdev message to NSSFW
 */
nss_tx_status_t nss_wifi_ext_vdev_tx_msg(struct nss_ctx_instance *nss_ctx, struct nss_wifi_ext_vdev_msg *msg)
{
	struct nss_cmn_msg *ncm = &msg->cm;

	/*
	 * Trace Messages
	 */
	nss_wifi_ext_vdev_log_tx_msg(msg);

	if (ncm->type >= NSS_WIFI_EXT_VDEV_MSG_MAX) {
		nss_warning("%px: wifi vdev message type out of range: %d", nss_ctx, ncm->type);
		return NSS_TX_FAILURE;
	}

	BUG_ON(!nss_wifi_ext_vdev_verify_if_num(ncm->interface));

	return nss_core_send_cmd(nss_ctx, msg, sizeof(*msg), NSS_NBUF_PAYLOAD_SIZE);
}
EXPORT_SYMBOL(nss_wifi_ext_vdev_tx_msg);

/*
 * nss_wifi_ext_vdev_callback()
 *	Callback to handle the completion of NSS->HLOS messages.
 */
static void nss_wifi_ext_vdev_callback(void *app_data, struct nss_cmn_msg *ncm)
{
	nss_wifi_ext_vdev_msg_callback_t callback = (nss_wifi_ext_vdev_msg_callback_t)wifi_ext_vdev_pvt.cb;
	void *data = wifi_ext_vdev_pvt.app_data;

	wifi_ext_vdev_pvt.response = NSS_TX_SUCCESS;
	wifi_ext_vdev_pvt.cb = NULL;
	wifi_ext_vdev_pvt.app_data = NULL;

	if (ncm->response != NSS_CMN_RESPONSE_ACK) {
		nss_warning("WiFi extension vap Error response %d\n", ncm->response);
		wifi_ext_vdev_pvt.response = NSS_TX_FAILURE;
	}

	if (callback) {
		callback(data, ncm);
	}
	complete(&wifi_ext_vdev_pvt.complete);
}

/*
 * nss_wifi_ext_vdev_tx_msg()
 *	Transmit a WiFi extended virtual interface to NSS firmware synchronously.
 */
nss_tx_status_t nss_wifi_ext_vdev_tx_msg_sync(struct nss_ctx_instance *nss_ctx, struct nss_wifi_ext_vdev_msg *nwevm)
{
	nss_tx_status_t status;
	int ret = 0;

	down(&wifi_ext_vdev_pvt.sem);
	wifi_ext_vdev_pvt.cb = (void *)nwevm->cm.cb;
	wifi_ext_vdev_pvt.app_data = (void *)nwevm->cm.app_data;

	nwevm->cm.cb = (nss_ptr_t)nss_wifi_ext_vdev_callback;
	nwevm->cm.app_data = (nss_ptr_t)NULL;

	status = nss_wifi_ext_vdev_tx_msg(nss_ctx, nwevm);
	if (status != NSS_TX_SUCCESS) {
		nss_warning("%px: wifi_ext_vdev_tx_msg failed\n", nss_ctx);
		up(&wifi_ext_vdev_pvt.sem);
		return status;
	}

	/*
	 * Wait for the acknowledgement
	 */
	ret = wait_for_completion_timeout(&wifi_ext_vdev_pvt.complete, msecs_to_jiffies(NSS_WIFI_EXT_VDEV_TX_TIMEOUT));
	if (!ret) {
		nss_warning("%px: WiFi extended vap msg tx failed due to timeout\n", nss_ctx);
		wifi_ext_vdev_pvt.response = NSS_TX_FAILURE;
	}

	status = wifi_ext_vdev_pvt.response;
	up(&wifi_ext_vdev_pvt.sem);
	return status;
}
EXPORT_SYMBOL(nss_wifi_ext_vdev_tx_msg_sync);

/*
 * nss_wifi_ext_vdev_tx_buf
 * 	Send data packet for vap processing
 */
nss_tx_status_t nss_wifi_ext_vdev_tx_buf(struct nss_ctx_instance *nss_ctx, struct sk_buff *skb, uint32_t if_num)
{
	BUG_ON(!nss_wifi_ext_vdev_verify_if_num(if_num));

	return nss_core_send_packet(nss_ctx, skb, if_num, H2N_BIT_FLAG_BUFFER_REUSABLE);
}
EXPORT_SYMBOL(nss_wifi_ext_vdev_tx_buf);

/*
 * nss_wifi_ext_vdev_set_next_hop()
 *	Set the WiFI extended vap next hop.
 */
nss_tx_status_t nss_wifi_ext_vdev_set_next_hop(struct nss_ctx_instance *ctx, int if_num, int next_hop)
{
	struct nss_wifi_ext_vdev_msg *nwevm = kzalloc(sizeof(struct nss_wifi_ext_vdev_msg), GFP_KERNEL);
	struct nss_wifi_ext_vdev_set_next_hop_msg *nhm = NULL;
	nss_tx_status_t status;

	if (!nwevm) {
		nss_warning("%px: Unable to allocate next hop message", ctx);
		return NSS_TX_FAILURE;
	}

	nhm = &nwevm->msg.wnhm;

	nhm->if_num = next_hop;
	nss_wifi_ext_vdev_msg_init(nwevm, if_num, NSS_WIFI_EXT_VDEV_SET_NEXT_HOP,
			sizeof(struct nss_wifi_ext_vdev_set_next_hop_msg), NULL, NULL);

	status = nss_wifi_ext_vdev_tx_msg(ctx, nwevm);
	if (status != NSS_TX_SUCCESS) {
		nss_warning("%px: Unable to send next hop message", ctx);
	}

	kfree(nwevm);
	return status;
}
EXPORT_SYMBOL(nss_wifi_ext_vdev_set_next_hop);

/*
 * nss_get_wifi_ext_vdev_ext_context()
 *	Return the core ctx which the feature is on
 */
struct nss_ctx_instance *nss_wifi_ext_vdev_get_ctx(void)
{
	return (struct nss_ctx_instance *)&nss_top_main.nss[nss_top_main.wifi_handler_id];
}
EXPORT_SYMBOL(nss_wifi_ext_vdev_get_ctx);

/*
 * nss_wifi_ext_vdev_register_if()
 */
struct nss_ctx_instance *nss_wifi_ext_vdev_register_if(uint32_t if_num,
				nss_wifi_ext_vdev_data_callback_t data_callback,
				nss_wifi_ext_vdev_ext_data_callback_t ext_callback,
				nss_wifi_ext_vdev_msg_callback_t event_callback,
				struct net_device *netdev,
				uint32_t features, void *app_data)
{
	struct nss_ctx_instance *nss_ctx = nss_wifi_ext_vdev_get_ctx();

	BUG_ON(!nss_wifi_ext_vdev_verify_if_num(if_num));

	nss_core_register_subsys_dp(nss_ctx, if_num, data_callback, ext_callback, app_data, netdev, features);

	nss_core_register_msg_handler(nss_ctx, if_num, event_callback);

	nss_core_register_handler(nss_ctx, if_num, nss_wifi_ext_vdev_handler, app_data);

	nss_wifi_ext_vdev_stats_register(if_num, netdev);

	return nss_ctx;
}
EXPORT_SYMBOL(nss_wifi_ext_vdev_register_if);

/*
 * nss_wifi_ext_vdev_unregister_if()
 */
bool nss_wifi_ext_vdev_unregister_if(uint32_t if_num)
{
	struct nss_ctx_instance *nss_ctx = nss_wifi_ext_vdev_get_ctx();
	struct net_device *netdev;

	BUG_ON(!nss_wifi_ext_vdev_verify_if_num(if_num));

	nss_assert(nss_ctx);

	netdev = nss_cmn_get_interface_dev(nss_ctx, if_num);
	if (!netdev) {
		nss_warning("%px: Unable to find net device for the interface %d\n", nss_ctx, if_num);
		return false;
	}
	nss_core_unregister_subsys_dp(nss_ctx, if_num);

	nss_core_unregister_msg_handler(nss_ctx, if_num);

	nss_core_unregister_handler(nss_ctx, if_num);

	nss_wifi_ext_vdev_stats_unregister(if_num, netdev);
	return true;
}
EXPORT_SYMBOL(nss_wifi_ext_vdev_unregister_if);

/*
 * nss_wifi_ext_vdev_register_handler()
 *	Register debugfs handler received on base interface
 */
void nss_wifi_ext_vdev_register_handler(void)
{
	struct nss_ctx_instance *nss_ctx = nss_wifi_ext_vdev_get_ctx();

	nss_info("nss_wifi_ext_vdev_handler");
	sema_init(&wifi_ext_vdev_pvt.sem, 1);
	init_completion(&wifi_ext_vdev_pvt.complete);
	nss_core_register_handler(nss_ctx, NSS_WIFI_EXT_VDEV_INTERFACE, nss_wifi_ext_vdev_handler, NULL);
	nss_wifi_ext_vdev_stats_dentry_create();
}

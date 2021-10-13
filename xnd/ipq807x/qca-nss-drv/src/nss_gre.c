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
#include "nss_gre_stats.h"
#include "nss_gre_log.h"
#include "nss_gre_strings.h"

#define NSS_GRE_TX_TIMEOUT 3000 /* 3 Seconds */

/*
 * Private data structure
 */
static struct {
	struct semaphore sem;
	struct completion complete;
	int response;
	void *cb;
	void *app_data;
} nss_gre_pvt;

/*
 * TODO: Register separate callbacks for inner and outer GRE nodes.
 */
static atomic64_t pkt_cb_addr = ATOMIC64_INIT(0);

/*
 * nss_gre_inner_rx_handler()
 *	GRE inner rx handler.
 */
static void nss_gre_inner_rx_handler(struct net_device *dev, struct sk_buff *skb,
		    __attribute__((unused)) struct napi_struct *napi)
{
	nss_gre_data_callback_t cb;

	nss_gre_pkt_callback_t scb = (nss_gre_pkt_callback_t)(unsigned long)atomic64_read(&pkt_cb_addr);
	if (unlikely(scb)) {
		struct nss_gre_info *info = (struct nss_gre_info *)netdev_priv(dev);
		if (likely(info->next_dev_inner)) {
			scb(info->next_dev_inner, skb);
		}
	}

	cb = nss_top_main.gre_inner_data_callback;
	cb(dev, skb, 0);
}

/*
 * nss_gre_outer_rx_handler()
 *	GRE outer rx handler.
 */
static void nss_gre_outer_rx_handler(struct net_device *dev, struct sk_buff *skb,
		    __attribute__((unused)) struct napi_struct *napi)
{
	nss_gre_data_callback_t cb;

	nss_gre_pkt_callback_t scb = (nss_gre_pkt_callback_t)(unsigned long)atomic64_read(&pkt_cb_addr);
	if (unlikely(scb)) {
		struct nss_gre_info *info = (struct nss_gre_info *)netdev_priv(dev);
		if (likely(info->next_dev_outer)) {
			scb(info->next_dev_outer, skb);
		}
	}

	cb = nss_top_main.gre_outer_data_callback;
	cb(dev, skb, 0);
}

/*
 * nss_gre_msg_handler()
 *	Handle NSS -> HLOS messages for GRE
 */
static void nss_gre_msg_handler(struct nss_ctx_instance *nss_ctx, struct nss_cmn_msg *ncm, __attribute__((unused))void *app_data)
{
	struct nss_gre_msg *ntm = (struct nss_gre_msg *)ncm;
	void *ctx;

	nss_gre_msg_callback_t cb;

	NSS_VERIFY_CTX_MAGIC(nss_ctx);
	BUG_ON(!(nss_is_dynamic_interface(ncm->interface) || ncm->interface == NSS_GRE_INTERFACE));

	/*
	 * Trace Messages
	 */
	nss_gre_log_rx_msg(ntm);

	/*
	 * Is this a valid request/response packet?
	 */
	if (ncm->type >= NSS_GRE_MSG_MAX) {
		nss_warning("%px: received invalid message %d for GRE STD interface", nss_ctx, ncm->type);
		return;
	}

	if (nss_cmn_get_msg_len(ncm) > sizeof(struct nss_gre_msg)) {
		nss_warning("%px: tx request for another interface: %d", nss_ctx, ncm->interface);
		return;
	}

	switch (ntm->cm.type) {
	case NSS_GRE_MSG_SESSION_STATS:
		/*
		 * debug stats embedded in stats msg
		 */
		nss_gre_stats_session_sync(nss_ctx, &ntm->msg.sstats, ncm->interface);
		nss_gre_stats_session_notify(nss_ctx, ncm->interface);
		break;

	case NSS_GRE_MSG_BASE_STATS:
		nss_gre_stats_base_sync(nss_ctx, &ntm->msg.bstats);
		nss_gre_stats_base_notify(nss_ctx);
		break;

	default:
		break;

	}

	/*
	 * Update the callback and app_data for NOTIFY messages, gre sends all notify messages
	 * to the same callback/app_data.
	 */
	if (ncm->response == NSS_CMN_RESPONSE_NOTIFY) {
		ncm->cb = (nss_ptr_t)nss_ctx->nss_top->gre_msg_callback;
		ncm->app_data = (nss_ptr_t)nss_ctx->subsys_dp_register[ncm->interface].app_data;
	}

	/*
	 * Log failures
	 */
	nss_core_log_msg_failures(nss_ctx, ncm);

	/*
	 * callback
	 */
	cb = (nss_gre_msg_callback_t)ncm->cb;
	ctx = (void *)ncm->app_data;

	/*
	 * call gre-std callback
	 */
	if (!cb) {
		nss_warning("%px: No callback for gre-std interface %d",
			    nss_ctx, ncm->interface);
		return;
	}

	cb(ctx, ntm);
}

/*
 * nss_gre_callback()
 *	Callback to handle the completion of HLOS-->NSS messages.
 */
static void nss_gre_callback(void *app_data, struct nss_gre_msg *nim)
{
	nss_gre_msg_callback_t callback = (nss_gre_msg_callback_t)nss_gre_pvt.cb;
	void *data = nss_gre_pvt.app_data;

	nss_gre_pvt.cb = NULL;
	nss_gre_pvt.app_data = NULL;

	if (nim->cm.response != NSS_CMN_RESPONSE_ACK) {
		nss_warning("gre Error response %d\n", nim->cm.response);
		nss_gre_pvt.response = NSS_TX_FAILURE;
	} else {
		nss_gre_pvt.response = NSS_TX_SUCCESS;
	}

	if (callback) {
		callback(data, nim);
	}

	complete(&nss_gre_pvt.complete);
}

/*
 * nss_gre_register_pkt_callback()
 *	Register for data callback.
 */
void nss_gre_register_pkt_callback(nss_gre_pkt_callback_t cb)
{
	atomic64_set(&pkt_cb_addr, (unsigned long)cb);
}
EXPORT_SYMBOL(nss_gre_register_pkt_callback);

/*
 * nss_gre_unregister_pkt_callback()
 *	Unregister for data callback.
 */
void nss_gre_unregister_pkt_callback()
{
	atomic64_set(&pkt_cb_addr, 0);
}
EXPORT_SYMBOL(nss_gre_unregister_pkt_callback);

/*
 * nss_gre_tx_msg()
 *	Transmit a GRE message to NSS firmware
 */
nss_tx_status_t nss_gre_tx_msg(struct nss_ctx_instance *nss_ctx, struct nss_gre_msg *msg)
{
	struct nss_cmn_msg *ncm = &msg->cm;

	/*
	 * Sanity check the message
	 */
	if (!nss_is_dynamic_interface(ncm->interface)) {
		nss_warning("%px: tx request for non dynamic interface: %d", nss_ctx, ncm->interface);
		return NSS_TX_FAILURE;
	}

	if (ncm->type > NSS_GRE_MSG_MAX) {
		nss_warning("%px: message type out of range: %d", nss_ctx, ncm->type);
		return NSS_TX_FAILURE;
	}

	/*
	 * Trace Messages
	 */
	nss_gre_log_tx_msg(msg);

	return nss_core_send_cmd(nss_ctx, msg, sizeof(*msg), NSS_NBUF_PAYLOAD_SIZE);
}
EXPORT_SYMBOL(nss_gre_tx_msg);

/*
 * nss_gre_tx_msg_sync()
 *	Transmit a GRE message to NSS firmware synchronously.
 */
nss_tx_status_t nss_gre_tx_msg_sync(struct nss_ctx_instance *nss_ctx, struct nss_gre_msg *msg)
{
	nss_tx_status_t status;
	int ret = 0;

	down(&nss_gre_pvt.sem);
	nss_gre_pvt.cb = (void *)msg->cm.cb;
	nss_gre_pvt.app_data = (void *)msg->cm.app_data;

	msg->cm.cb = (nss_ptr_t)nss_gre_callback;
	msg->cm.app_data = (nss_ptr_t)NULL;

	status = nss_gre_tx_msg(nss_ctx, msg);
	if (status != NSS_TX_SUCCESS) {
		nss_warning("%px: gre_tx_msg failed\n", nss_ctx);
		up(&nss_gre_pvt.sem);
		return status;
	}
	ret = wait_for_completion_timeout(&nss_gre_pvt.complete, msecs_to_jiffies(NSS_GRE_TX_TIMEOUT));

	if (!ret) {
		nss_warning("%px: GRE STD tx sync failed due to timeout\n", nss_ctx);
		nss_gre_pvt.response = NSS_TX_FAILURE;
	}

	status = nss_gre_pvt.response;
	up(&nss_gre_pvt.sem);
	return status;
}
EXPORT_SYMBOL(nss_gre_tx_msg_sync);

/*
 * nss_gre_tx_buf()
 *	Send packet to GRE interface owned by NSS
 */
nss_tx_status_t nss_gre_tx_buf(struct nss_ctx_instance *nss_ctx, uint32_t if_num, struct sk_buff *skb)
{
	return nss_core_send_packet(nss_ctx, skb, if_num, H2N_BIT_FLAG_VIRTUAL_BUFFER | H2N_BIT_FLAG_BUFFER_REUSABLE);
}
EXPORT_SYMBOL(nss_gre_tx_buf);

/*
 ***********************************
 * Register/Unregister/Miscellaneous APIs
 ***********************************
 */

/*
 * nss_gre_register_if()
 *	Register data and message handlers for GRE.
 */
struct nss_ctx_instance *nss_gre_register_if(uint32_t if_num, uint32_t type, nss_gre_data_callback_t data_callback,
			nss_gre_msg_callback_t event_callback, struct net_device *netdev, uint32_t features)
{
	struct nss_ctx_instance *nss_ctx = (struct nss_ctx_instance *)&nss_top_main.nss[nss_top_main.gre_handler_id];

	nss_assert(nss_ctx);
	nss_assert(nss_is_dynamic_interface(if_num));

	switch (type) {
	case NSS_DYNAMIC_INTERFACE_TYPE_GRE_INNER:
		nss_core_register_subsys_dp(nss_ctx, if_num, nss_gre_inner_rx_handler, NULL, netdev, netdev, features);
		nss_top_main.gre_inner_data_callback = data_callback;
		break;

	case NSS_DYNAMIC_INTERFACE_TYPE_GRE_OUTER:
		nss_core_register_subsys_dp(nss_ctx, if_num, nss_gre_outer_rx_handler, NULL, netdev, netdev, features);
		nss_top_main.gre_outer_data_callback = data_callback;
		break;

	default:
		nss_warning("%px: Unable to register. Wrong interface type %d\n", nss_ctx, type);
		return NULL;
	}

	nss_core_set_subsys_dp_type(nss_ctx, netdev, if_num, type);

	nss_top_main.gre_msg_callback = event_callback;

	nss_core_register_handler(nss_ctx, if_num, nss_gre_msg_handler, NULL);

	nss_gre_stats_session_register(if_num, netdev);

	return nss_ctx;
}
EXPORT_SYMBOL(nss_gre_register_if);

/*
 * nss_gre_unregister_if()
 *	Unregister data and message handler.
 */
void nss_gre_unregister_if(uint32_t if_num)
{
	struct nss_ctx_instance *nss_ctx = (struct nss_ctx_instance *)&nss_top_main.nss[nss_top_main.gre_handler_id];
	struct net_device *dev;

	nss_assert(nss_ctx);
	nss_assert(nss_is_dynamic_interface(if_num));

	dev = nss_cmn_get_interface_dev(nss_ctx, if_num);
	if (!dev) {
		nss_warning("%px: Unable to find net device for the interface %d\n", nss_ctx, if_num);
		return;
	}

	nss_core_unregister_subsys_dp(nss_ctx, if_num);
	nss_core_set_subsys_dp_type(nss_ctx, dev, if_num, NSS_DYNAMIC_INTERFACE_TYPE_NONE);
	nss_top_main.gre_msg_callback = NULL;

	nss_core_unregister_handler(nss_ctx, if_num);

	nss_gre_stats_session_unregister(if_num);
}
EXPORT_SYMBOL(nss_gre_unregister_if);

/*
 * nss_get_gre_context()
 */
struct nss_ctx_instance *nss_gre_get_context(void)
{
	return (struct nss_ctx_instance *)&nss_top_main.nss[nss_top_main.gre_handler_id];
}
EXPORT_SYMBOL(nss_gre_get_context);

/*
 * nss_gre_ifnum_with_core_id()
 *	Append core id to GRE interface num.
 */
int nss_gre_ifnum_with_core_id(int if_num)
{
	struct nss_ctx_instance *nss_ctx = nss_gre_get_context();

	NSS_VERIFY_CTX_MAGIC(nss_ctx);
	if (!nss_is_dynamic_interface(if_num)) {
		nss_warning("%px: Invalid if_num: %d, must be a dynamic interface\n", nss_ctx, if_num);
		return 0;
	}

	return NSS_INTERFACE_NUM_APPEND_COREID(nss_ctx, if_num);
}
EXPORT_SYMBOL(nss_gre_ifnum_with_core_id);

/*
 * nss_gre_msg_init()
 *	Initialize nss_gre msg.
 */
void nss_gre_msg_init(struct nss_gre_msg *ncm, uint16_t if_num, uint32_t type,  uint32_t len, void *cb, void *app_data)
{
	nss_cmn_msg_init(&ncm->cm, if_num, type, len, cb, app_data);
}
EXPORT_SYMBOL(nss_gre_msg_init);

/*
 * nss_gre_register_handler()
 *	debugfs stats msg handler received on static gre interface
 */
void nss_gre_register_handler(void)
{
	struct nss_ctx_instance *nss_ctx = nss_gre_get_context();

	nss_info("nss_gre_register_handler");
	sema_init(&nss_gre_pvt.sem, 1);
	init_completion(&nss_gre_pvt.complete);
	nss_core_register_handler(nss_ctx, NSS_GRE_INTERFACE, nss_gre_msg_handler, NULL);
	nss_gre_stats_dentry_create();
	nss_gre_strings_dentry_create();
}

/*
 **************************************************************************
 * Copyright (c) 2016-2021, The Linux Foundation. All rights reserved.
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
#include "nss_gre_tunnel_stats.h"
#include "nss_gre_tunnel_log.h"
#include "nss_gre_tunnel_strings.h"

#define NSS_GRE_TUNNEL_TX_TIMEOUT 3000 /* 3 Seconds */

/*
 * Private data structure
 */
static struct nss_gre_tunnel_pvt {
	struct semaphore sem;
	struct completion complete;
	int response;
	void *cb;
	void *app_data;
} gre_tunnel_pvt;

/*
 * nss_gre_tunnel_verify_if_num()
 *	Verify if_num passed to us.
 */
static bool nss_gre_tunnel_verify_if_num(uint32_t if_num)
{
	uint32_t type = nss_dynamic_interface_get_type(nss_gre_tunnel_get_ctx(), if_num);

	switch (type) {
	case NSS_DYNAMIC_INTERFACE_TYPE_GRE_TUNNEL_INNER:
	case NSS_DYNAMIC_INTERFACE_TYPE_GRE_TUNNEL_INLINE_INNER:
	case NSS_DYNAMIC_INTERFACE_TYPE_GRE_TUNNEL_OUTER:
	case NSS_DYNAMIC_INTERFACE_TYPE_GRE_TUNNEL_INLINE_OUTER:
	case NSS_DYNAMIC_INTERFACE_TYPE_GRE_TUNNEL_INNER_EXCEPTION:
		return true;
	default:
		return false;
	}
}

/*
 * nss_gre_tunnel_handler()
 *	Handle NSS to HLOS messages for gre_tunnel
 */
static void nss_gre_tunnel_handler(struct nss_ctx_instance *nss_ctx, struct nss_cmn_msg *ncm, __attribute__((unused))void *app_data)
{
	struct nss_gre_tunnel_msg *ngtm = (struct nss_gre_tunnel_msg *)ncm;
	void *ctx;

	nss_gre_tunnel_msg_callback_t cb;

	NSS_VERIFY_CTX_MAGIC(nss_ctx);
	BUG_ON(!nss_gre_tunnel_verify_if_num(ncm->interface));

	/*
	 * Trace Messages
	 */
	nss_gre_tunnel_log_rx_msg(ngtm);

	/*
	 * Is this a valid request/response packet?
	 */
	if (ncm->type >= NSS_GRE_TUNNEL_MSG_MAX) {
		nss_warning("%px: received invalid message %d for GRE_TUNNEL interface %d", nss_ctx, ncm->type, ncm->interface);
		return;
	}

	if (nss_cmn_get_msg_len(ncm) > sizeof(struct nss_gre_tunnel_msg)) {
		nss_warning("%px: gre_tunnel message length is invalid: %d", nss_ctx, ncm->len);
		return;
	}

	/*
	 * Check messages
	 */
	switch (ngtm->cm.type) {
	case NSS_GRE_TUNNEL_MSG_STATS:
		nss_gre_tunnel_stats_session_sync(nss_ctx, &ngtm->msg.stats, ncm->interface);
		nss_gre_tunnel_stats_notify(nss_ctx, ncm->interface);
		break;
	}

	/*
	 * Update the callback and app_data for NOTIFY messages
	 */
	if (ncm->response == NSS_CMN_RESPONSE_NOTIFY) {
		ncm->cb = (nss_ptr_t)nss_ctx->nss_top->gre_tunnel_msg_callback;
		ncm->app_data = (nss_ptr_t)nss_ctx->subsys_dp_register[ncm->interface].app_data;
	}

	nss_core_log_msg_failures(nss_ctx, ncm);

	/*
	 * callback
	 */
	cb = (nss_gre_tunnel_msg_callback_t)ncm->cb;
	ctx = (void *)ncm->app_data;

	/*
	 * call GRE Tunnel session callback
	 */
	if (!cb) {
		return;
	}

	cb(ctx, ngtm);
}

/*
 * nss_gre_tunnel_inquiry()
 *	Inquiry if a GRE tunnel has been established in NSS FW.
 *
 * Input parameters:
 *	inquiry_info->ip_type
 *	inquiry_info->src_ip
 *	inquiry_info->dest_ip
 *	inquiry_info->gre_mode
 *	 if (gre_mode == NSS_GRE_TUNNEL_MODE_GRE_UDP)
 *		inquiry_info->src_port
 *		inquiry_info->dest_port
 *	inquiry_info->encrypt_type	-- currently not checked in FW,
 */
nss_tx_status_t nss_gre_tunnel_inquiry(
		struct nss_gre_tunnel_configure *inquiry_info,
		nss_gre_tunnel_msg_callback_t cb, void *app_data)
{
	nss_tx_status_t nss_tx_status;
	struct nss_gre_tunnel_msg nim;
	struct nss_ctx_instance *nss_ctx = nss_gre_tunnel_get_ctx();

	/*
	 * Initialize inquiry message structure.
	 * This is async message and the result will be returned
	 * to the caller by the msg_callback passed in.
	 */
	memset(&nim, 0, sizeof(nim));
	nss_gre_tunnel_msg_init(&nim, NSS_GRE_TUNNEL_INTERFACE,
			NSS_GRE_TUNNEL_MSG_INQUIRY,
			sizeof(struct nss_gre_tunnel_configure),
			cb, app_data);
	nim.msg.configure = *inquiry_info;
	nss_tx_status = nss_gre_tunnel_tx_msg(nss_ctx, &nim);
	if (nss_tx_status != NSS_TX_SUCCESS) {
		nss_warning("%px: Send GT inquiry message failed\n", inquiry_info);
	}

	return nss_tx_status;
}
EXPORT_SYMBOL(nss_gre_tunnel_inquiry);

/*
 * nss_get_gre_tunnel_context()
 *	Return the core ctx which the feature is on
 */
struct nss_ctx_instance *nss_gre_tunnel_get_ctx(void)
{
	return (struct nss_ctx_instance *)&nss_top_main.nss[nss_top_main.gre_tunnel_handler_id];
}
EXPORT_SYMBOL(nss_gre_tunnel_get_ctx);

/*
 * nss_gre_tunnel_ifnum_with_core_id()
 *	Append core id to GRE tunnel interface num
 */
int nss_gre_tunnel_ifnum_with_core_id(int if_num)
{
	struct nss_ctx_instance *nss_ctx = nss_gre_tunnel_get_ctx();
	BUG_ON(!nss_gre_tunnel_verify_if_num(if_num));
	NSS_VERIFY_CTX_MAGIC(nss_ctx);

	if (nss_is_dynamic_interface(if_num) == false) {
		nss_info("%px: Invalid if_num: %d, must be a dynamic interface\n", nss_ctx, if_num);
		return 0;
	}

	return NSS_INTERFACE_NUM_APPEND_COREID(nss_ctx, if_num);
}
EXPORT_SYMBOL(nss_gre_tunnel_ifnum_with_core_id);

/*
 * nss_gre_tunnel_tx_buf()
 *	Transmit buffer over GRE Tunnel interface
 */
nss_tx_status_t nss_gre_tunnel_tx_buf(struct sk_buff *skb, uint32_t if_num,
				struct nss_ctx_instance *nss_ctx)
{
	BUG_ON(!nss_gre_tunnel_verify_if_num(if_num));

	return nss_core_send_packet(nss_ctx, skb, if_num, H2N_BIT_FLAG_VIRTUAL_BUFFER | H2N_BIT_FLAG_BUFFER_REUSABLE);
}
EXPORT_SYMBOL(nss_gre_tunnel_tx_buf);

/*
 * nss_gre_tunnel_tx_msg()
 *	Transmit a gre_tunnel message to NSS firmware
 */
nss_tx_status_t nss_gre_tunnel_tx_msg(struct nss_ctx_instance *nss_ctx, struct nss_gre_tunnel_msg *msg)
{
	struct nss_cmn_msg *ncm = &msg->cm;

	/*
	 * Trace Messages
	 */
	nss_gre_tunnel_log_tx_msg(msg);

	/*
	 * Sanity check message
	 */
	if (ncm->type >= NSS_GRE_TUNNEL_MSG_MAX) {
		nss_warning("%px: gre_tunnel message type out of range: %d",
			nss_ctx, ncm->type);
		return NSS_TX_FAILURE;
	}

	BUG_ON(!nss_gre_tunnel_verify_if_num(ncm->interface));

	return nss_core_send_cmd(nss_ctx, msg, sizeof(*msg), NSS_NBUF_PAYLOAD_SIZE);
}
EXPORT_SYMBOL(nss_gre_tunnel_tx_msg);

/*
 * nss_gre_tunnel_callback()
 *	Callback to handle the completion of NSS->HLOS messages.
 */
static void nss_gre_tunnel_callback(void *app_data, struct nss_gre_tunnel_msg *ngtm)
{
	nss_gre_tunnel_msg_callback_t callback = (nss_gre_tunnel_msg_callback_t)gre_tunnel_pvt.cb;
	void *data = gre_tunnel_pvt.app_data;

	gre_tunnel_pvt.response = NSS_TX_SUCCESS;
	gre_tunnel_pvt.cb = NULL;
	gre_tunnel_pvt.app_data = NULL;

	if (ngtm->cm.response != NSS_CMN_RESPONSE_ACK) {
		nss_warning("gre tunnel Error response %d\n", ngtm->cm.response);
		gre_tunnel_pvt.response = ngtm->cm.response;
	}

	if (callback) {
		callback(data, ngtm);
	}
	complete(&gre_tunnel_pvt.complete);
}

/*
 * nss_gre_tunnel_tx_msg()
 *	Transmit a GRE Tunnel message to NSS firmware synchronously.
 */
nss_tx_status_t nss_gre_tunnel_tx_msg_sync(struct nss_ctx_instance *nss_ctx, struct nss_gre_tunnel_msg *ngtm)
{
	nss_tx_status_t status;
	int ret = 0;

	down(&gre_tunnel_pvt.sem);
	gre_tunnel_pvt.cb = (void *)ngtm->cm.cb;
	gre_tunnel_pvt.app_data = (void *)ngtm->cm.app_data;

	ngtm->cm.cb = (nss_ptr_t)nss_gre_tunnel_callback;
	ngtm->cm.app_data = (nss_ptr_t)NULL;

	status = nss_gre_tunnel_tx_msg(nss_ctx, ngtm);
	if (status != NSS_TX_SUCCESS) {
		nss_warning("%px: gre_tunnel_tx_msg failed\n", nss_ctx);
		up(&gre_tunnel_pvt.sem);
		return status;
	}

	ret = wait_for_completion_timeout(&gre_tunnel_pvt.complete, msecs_to_jiffies(NSS_GRE_TUNNEL_TX_TIMEOUT));

	if (!ret) {
		nss_warning("%px: GRE Tunnel msg tx failed due to timeout\n", nss_ctx);
		gre_tunnel_pvt.response = NSS_TX_FAILURE;
	}

	status = gre_tunnel_pvt.response;
	up(&gre_tunnel_pvt.sem);
	return status;
}
EXPORT_SYMBOL(nss_gre_tunnel_tx_msg_sync);

/*
 * nss_gre_tunnel_msg_init()
 *	Initialize gre_tunnel msg.
 */
void nss_gre_tunnel_msg_init(struct nss_gre_tunnel_msg *ngtm, uint16_t if_num,
			uint32_t type, uint32_t len, void *cb, void *app_data)
{
	nss_cmn_msg_init(&ngtm->cm, if_num, type, len, cb, app_data);
}
EXPORT_SYMBOL(nss_gre_tunnel_msg_init);

/*
 * nss_gre_tunnel_register_if()
 *	Register netdev
 */
struct nss_ctx_instance *nss_gre_tunnel_register_if(uint32_t if_num,
					      nss_gre_tunnel_data_callback_t cb,
					      nss_gre_tunnel_msg_callback_t ev_cb,
					      struct net_device *netdev,
					      uint32_t features,
					      void *app_ctx)
{
	int32_t i;

	struct nss_ctx_instance *nss_ctx = nss_gre_tunnel_get_ctx();

	BUG_ON(!nss_gre_tunnel_verify_if_num(if_num));

	spin_lock_bh(&nss_gre_tunnel_stats_lock);
	for (i = 0; i < NSS_MAX_GRE_TUNNEL_SESSIONS; i++) {
		if (!session_stats[i].valid) {
			session_stats[i].valid = true;
			session_stats[i].if_num = if_num;
			session_stats[i].if_index = netdev->ifindex;
			break;
		}
	}
	spin_unlock_bh(&nss_gre_tunnel_stats_lock);

	if (i == NSS_MAX_GRE_TUNNEL_SESSIONS) {
		nss_warning("%px: Cannot find free slot for GRE Tunnel session stats, I/F:%u\n", nss_ctx, if_num);
		return NULL;
	}

	if (nss_ctx->subsys_dp_register[if_num].ndev) {
		nss_warning("%px: Cannot find free slot for GRE Tunnel NSS I/F:%u\n", nss_ctx, if_num);
		session_stats[i].valid = false;
		session_stats[i].if_num = 0;
		session_stats[i].if_index = 0;
		return NULL;
	}

	nss_core_register_subsys_dp(nss_ctx, if_num, cb, NULL, app_ctx, netdev, features);

	nss_top_main.gre_tunnel_msg_callback = ev_cb;
	nss_core_register_handler(nss_ctx, if_num, nss_gre_tunnel_handler, app_ctx);
	nss_gre_tunnel_stats_dentry_create();
	nss_gre_tunnel_strings_dentry_create();

	return nss_ctx;
}
EXPORT_SYMBOL(nss_gre_tunnel_register_if);

/*
 * nss_gre_tunnel_unregister_if()
 *	Unregister netdev
 */
void nss_gre_tunnel_unregister_if(uint32_t if_num)
{
	int32_t i;
	struct nss_ctx_instance *nss_ctx = nss_gre_tunnel_get_ctx();

	BUG_ON(!nss_gre_tunnel_verify_if_num(if_num));

	spin_lock_bh(&nss_gre_tunnel_stats_lock);
	for (i = 0; i < NSS_MAX_GRE_TUNNEL_SESSIONS; i++) {
		if (session_stats[i].if_num == if_num) {
			memset(&session_stats[i], 0,
				sizeof(struct nss_gre_tunnel_stats_session));
			break;
		}
	}
	spin_unlock_bh(&nss_gre_tunnel_stats_lock);

	if (i == NSS_MAX_GRE_TUNNEL_SESSIONS) {
		nss_warning("%px: Cannot find debug stats for GRE Tunnel session: %d\n", nss_ctx, if_num);
		return;
	}

	if (!nss_ctx->subsys_dp_register[if_num].ndev) {
		nss_warning("%px: Cannot find registered netdev for GRE Tunnel NSS I/F: %d\n", nss_ctx, if_num);

		return;
	}

	nss_core_unregister_subsys_dp(nss_ctx, if_num);

	nss_top_main.gre_tunnel_msg_callback = NULL;
	nss_core_unregister_handler(nss_ctx, if_num);
}
EXPORT_SYMBOL(nss_gre_tunnel_unregister_if);

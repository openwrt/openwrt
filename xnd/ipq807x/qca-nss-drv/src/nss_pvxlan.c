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
  * nss_pvxlan.c
  *	NSS PVXLAN driver interface APIs
  */
#include "nss_core.h"
#include "nss_pvxlan.h"
#include "nss_cmn.h"
#include "nss_tx_rx_common.h"
#include "nss_pvxlan_stats.h"
#include "nss_pvxlan_log.h"

#define NSS_PVXLAN_TX_TIMEOUT 3000

/*
 * Spinlock for protecting tunnel operations colliding with a tunnel destroy
 */
DEFINE_SPINLOCK(nss_pvxlan_spinlock);

/*
 * Private data structure
 */
static struct nss_pvxlan_pvt {
	struct semaphore sem;		/* Semaphore structure. */
	struct completion complete;	/* Completion structure. */
	int response;			/* Response from FW. */
	void *cb;			/* Original cb for msgs. */
	void *app_data;			/* Original app_data for msgs. */
} pvxlan_pvt;

/*
 * Per PVxLAN tunnel/interface number instance.
 */
struct nss_pvxlan_handle {
	atomic_t refcnt;			/* Reference count on the tunnel */
	uint32_t if_num;			/* Interface number */
	uint32_t tunnel_status;			/* 0=disable, 1=enabled */
	nss_pvxlan_msg_callback_t msg_callback;	/* Msg callback */
	void *app_data;				/* App data (argument) */
};

/*
 * Array of pointer for NSS PvLAN handles. Each handle has per-tunnel
 * stats based on the if_num which is an index.
 */
static struct nss_pvxlan_handle *nss_pvxlan_hdl[NSS_MAX_DYNAMIC_INTERFACES];

/*
 * nss_pvxlan_verify_if_num()
 *	Verify if_num passed to us.
 */
static bool nss_pvxlan_verify_if_num(uint32_t if_num)
{
	uint32_t type = nss_dynamic_interface_get_type(nss_pvxlan_get_ctx(), if_num);

	return ((type == NSS_DYNAMIC_INTERFACE_TYPE_PVXLAN_HOST_INNER) ||
				(type == NSS_DYNAMIC_INTERFACE_TYPE_PVXLAN_OUTER));
}

/*
 * nss_pvxlan_hdl_instance_free()
 *	Free PVxLAN tunnel handle instance.
 */
static bool nss_pvxlan_hdl_instance_free(struct nss_ctx_instance *nss_ctx, uint32_t if_num)
{
	struct nss_pvxlan_handle *h;

	spin_lock_bh(&nss_pvxlan_spinlock);
	h = nss_pvxlan_hdl[if_num - NSS_DYNAMIC_IF_START];
	if (!h) {
		spin_unlock_bh(&nss_pvxlan_spinlock);
		nss_warning("%px: Instance does not exist: %d", nss_ctx, if_num);
		return false;
	}

	if (h->if_num != if_num) {
		spin_unlock_bh(&nss_pvxlan_spinlock);
		nss_warning("%px: Not correct if_num: %d", nss_ctx, if_num);
		return false;
	}

	nss_pvxlan_hdl[if_num - NSS_DYNAMIC_IF_START] = NULL;
	spin_unlock_bh(&nss_pvxlan_spinlock);
	kfree(h);
	return true;
}

/*
 * nss_pvxlan_hdl_instance_alloc()
 *	Allocate PVxLAN tunnel instance.
 */
static bool nss_pvxlan_hdl_instance_alloc(struct nss_ctx_instance *nss_ctx, uint32_t if_num,
							nss_pvxlan_msg_callback_t notify_cb, void *app_data)
{
	struct nss_pvxlan_handle *h;

	/*
	 * Allocate a handle
	 */
	h = kzalloc(sizeof(struct nss_pvxlan_handle), GFP_ATOMIC);
	if (!h) {
		nss_warning("%px: no memory for allocating PVxLAN handle instance for interface : %d", nss_ctx, if_num);
		return false;
	}
	h->if_num = if_num;

	spin_lock_bh(&nss_pvxlan_spinlock);
	if (nss_pvxlan_hdl[if_num - NSS_DYNAMIC_IF_START] != NULL) {
		spin_unlock_bh(&nss_pvxlan_spinlock);
		kfree(h);
		nss_warning("%px: The handle has been taken by another thread :%d", nss_ctx, if_num);
		return false;
	}

	h->msg_callback = notify_cb;
	h->app_data = app_data;
	nss_pvxlan_hdl[if_num - NSS_DYNAMIC_IF_START] = h;
	spin_unlock_bh(&nss_pvxlan_spinlock);

	return true;
}

/*
 * nss_pvxlan_callback()
 *	Callback to handle the completion of NSS->HLOS messages.
 */
static void nss_pvxlan_callback(void *app_data, struct nss_pvxlan_msg *nvxm)
{
	nss_pvxlan_msg_callback_t callback = (nss_pvxlan_msg_callback_t)pvxlan_pvt.cb;
	void *data = pvxlan_pvt.app_data;

	pvxlan_pvt.response = NSS_TX_SUCCESS;
	pvxlan_pvt.cb = NULL;
	pvxlan_pvt.app_data = NULL;

	if (nvxm->cm.response != NSS_CMN_RESPONSE_ACK) {
		nss_warning("Pvxlan Error response %d\n", nvxm->cm.response);
		pvxlan_pvt.response = nvxm->cm.response;
	}

	if (callback) {
		callback(data, nvxm);
	}
	complete(&pvxlan_pvt.complete);
}

/*
 * nss_pvxlan_handler()
 *	Handle NSS -> HLOS messages for PVxLAN.
 */
static void nss_pvxlan_msg_handler(struct nss_ctx_instance *nss_ctx, struct nss_cmn_msg *ncm, __attribute__((unused))void *app_data)
{
	struct nss_pvxlan_msg *nvxm = (struct nss_pvxlan_msg *)ncm;
	nss_pvxlan_msg_callback_t cb;
	struct nss_pvxlan_handle * h;

	BUG_ON(!nss_pvxlan_verify_if_num(ncm->interface));

	/*
	 * Is this a valid request/response packet?
	 */
	if (ncm->type >= NSS_PVXLAN_MSG_TYPE_MAX) {
		nss_warning("%px: received invalid message %d for PVXLAN interface", nss_ctx, ncm->type);
		return;
	}

	if (nss_cmn_get_msg_len(ncm) > sizeof(struct nss_pvxlan_msg)) {
		nss_warning("%px: Length of message is greater than required: %d", nss_ctx, nss_cmn_get_msg_len(ncm));
		return;
	}

	/*
	 * Trace messages.
	 */
	nss_core_log_msg_failures(nss_ctx, ncm);
	nss_pvxlan_log_rx_msg(nvxm);

	switch (nvxm->cm.type) {
	case NSS_PVXLAN_MSG_TYPE_SYNC_STATS:
		nss_pvxlan_stats_sync(nss_ctx, &nvxm->msg.stats, ncm->interface);
		break;
	}

	/*
	 * Update the callback and app_data for NOTIFY messages.
	 */
	if (ncm->response == NSS_CMN_RESPONSE_NOTIFY) {
		uint32_t if_num = ncm->interface - NSS_DYNAMIC_IF_START;
		spin_lock_bh(&nss_pvxlan_spinlock);
		h = nss_pvxlan_hdl[if_num];
		if (h) {
			ncm->cb = (nss_ptr_t)h->msg_callback;
			ncm->app_data = (nss_ptr_t)h->app_data;
		}
		spin_unlock_bh(&nss_pvxlan_spinlock);

	}

	cb = (nss_pvxlan_msg_callback_t)ncm->cb;

	/*
	 * Do we have a callback
	 */
	if (!cb) {
		nss_trace("%px: cb is null for interface %d", nss_ctx, ncm->interface);
		return;
	}

	cb((void *)ncm->app_data, nvxm);
}

/*
 * nss_pvxlan_tx_msg()
 *	Transmit a PVXLAN message to NSS FW. Don't call this from softirq/interrupts.
 */
nss_tx_status_t nss_pvxlan_tx_msg(struct nss_ctx_instance *nss_ctx, struct nss_pvxlan_msg *msg)
{
	struct nss_cmn_msg *ncm = &msg->cm;

	if (!nss_pvxlan_verify_if_num(msg->cm.interface)) {
		return NSS_TX_FAILURE_BAD_PARAM;
	}

	if (ncm->type >= NSS_PVXLAN_MSG_TYPE_MAX) {
		return NSS_TX_FAILURE_BAD_PARAM;
	}

	/*
	 * Trace messages.
	 */
	nss_pvxlan_log_tx_msg(msg);

	return nss_core_send_cmd(nss_ctx, msg, sizeof(*msg), NSS_NBUF_PAYLOAD_SIZE);
}
EXPORT_SYMBOL(nss_pvxlan_tx_msg);

/*
 * nss_pvxlan_tx_msg_sync()
 *	Transmit a pvxlan message to NSS firmware synchronously.
 */
nss_tx_status_t nss_pvxlan_tx_msg_sync(struct nss_ctx_instance *nss_ctx, struct nss_pvxlan_msg *nvxm)
{
	nss_tx_status_t status;
	int ret;

	down(&pvxlan_pvt.sem);
	nvxm->cm.cb = (nss_ptr_t)nss_pvxlan_callback;
	nvxm->cm.app_data = (nss_ptr_t)NULL;

	status = nss_pvxlan_tx_msg(nss_ctx, nvxm);
	if (status != NSS_TX_SUCCESS) {
		nss_warning("%px: pvxlan_tx_msg failed\n", nss_ctx);
		up(&pvxlan_pvt.sem);
		return status;
	}

	ret = wait_for_completion_timeout(&pvxlan_pvt.complete, msecs_to_jiffies(NSS_PVXLAN_TX_TIMEOUT));
	if (!ret) {
		nss_warning("%px: pvxlan tx sync failed due to timeout\n", nss_ctx);
		pvxlan_pvt.response = NSS_TX_FAILURE;
	}

	status = pvxlan_pvt.response;
	up(&pvxlan_pvt.sem);
	return status;
}
EXPORT_SYMBOL(nss_pvxlan_tx_msg_sync);

/*
 * nss_pvxlan_tx_buf()
 *	Transmit data buffer (skb) to a NSS interface number
 */
nss_tx_status_t nss_pvxlan_tx_buf(struct nss_ctx_instance *nss_ctx, struct sk_buff *buf, uint32_t if_num)
{
	BUG_ON(!nss_pvxlan_verify_if_num(if_num));

	return nss_core_send_packet(nss_ctx, buf, if_num, H2N_BIT_FLAG_VIRTUAL_BUFFER | H2N_BIT_FLAG_BUFFER_REUSABLE);
}
EXPORT_SYMBOL(nss_pvxlan_tx_buf);

/*
 ***********************************
 * Register/Unregister/Miscellaneous APIs
 ***********************************
 */

/*
 * nss_pvxlan_unregister()
 *	Unregister a data packet notifier with NSS FW.
 */
bool nss_pvxlan_unregister(uint32_t if_num)
{
	struct nss_ctx_instance *nss_ctx;
	int32_t i;

	nss_ctx = nss_pvxlan_get_ctx();
	if (!nss_pvxlan_verify_if_num(if_num)) {
		nss_warning("%px: data unregister received for invalid interface %d", nss_ctx, if_num);
		return false;
	}

	spin_lock_bh(&nss_pvxlan_tunnel_stats_debug_lock);
	for (i = 0; i < NSS_PVXLAN_MAX_INTERFACES; i++) {
		if (nss_pvxlan_tunnel_debug_stats[i].if_num != if_num) {
			continue;
		}

		memset(&nss_pvxlan_tunnel_debug_stats[i], 0,
			sizeof(struct nss_pvxlan_tunnel_stats_debug));
		break;
	}
	spin_unlock_bh(&nss_pvxlan_tunnel_stats_debug_lock);

	nss_core_unregister_handler(nss_ctx, if_num);
	nss_core_unregister_subsys_dp(nss_ctx, if_num);
	nss_pvxlan_hdl_instance_free(nss_ctx, if_num);
	return true;
}
EXPORT_SYMBOL(nss_pvxlan_unregister);

/*
 * nss_pvxlan_register()
 *	Registers a data packet notifier with NSS FW.
 */
struct nss_ctx_instance *nss_pvxlan_register(uint32_t if_num,
						nss_pvxlan_buf_callback_t data_cb,
						nss_pvxlan_msg_callback_t notify_cb,
						struct net_device *netdev,
						uint32_t features)
{
	struct nss_ctx_instance *nss_ctx;
	int core_status;
	int32_t i;

	nss_ctx = nss_pvxlan_get_ctx();
	if (!nss_pvxlan_verify_if_num(if_num)) {
		nss_warning("%px: data register received for invalid interface %d", nss_ctx, if_num);
		return NULL;
	}

	core_status = nss_core_register_handler(nss_ctx, if_num, nss_pvxlan_msg_handler, NULL);
	if (core_status != NSS_CORE_STATUS_SUCCESS) {
		nss_warning("%px: nss core register handler failed for if_num:%d with error :%d", nss_ctx, if_num, core_status);
		return NULL;
	}

	if (!nss_pvxlan_hdl_instance_alloc(nss_ctx, if_num, notify_cb, (void *)netdev)) {
		nss_core_unregister_handler(nss_ctx, if_num);
		nss_warning("%px: couldn't allocate handle instance for if_num:%d", nss_ctx, if_num);
		return NULL;
	}

	spin_lock_bh(&nss_pvxlan_tunnel_stats_debug_lock);
	for (i = 0; i < NSS_PVXLAN_MAX_INTERFACES; i++) {
		if (nss_pvxlan_tunnel_debug_stats[i].valid) {
			continue;
		}

		nss_pvxlan_tunnel_debug_stats[i].valid = true;
		nss_pvxlan_tunnel_debug_stats[i].if_num = if_num;
		nss_pvxlan_tunnel_debug_stats[i].if_index = netdev->ifindex;
		break;
	}
	spin_unlock_bh(&nss_pvxlan_tunnel_stats_debug_lock);

	if (i == NSS_PVXLAN_MAX_INTERFACES) {
		nss_warning("%px: No available debug stats instance :%d", nss_ctx, if_num);
		nss_pvxlan_hdl_instance_free(nss_ctx, if_num);
		nss_core_unregister_handler(nss_ctx, if_num);
		return NULL;
	}

	nss_core_register_subsys_dp(nss_ctx, if_num, data_cb, NULL, NULL, netdev, features);
	return nss_ctx;
}
EXPORT_SYMBOL(nss_pvxlan_register);

/*
 * nss_pvxlan_ifnum_with_core_id()
 *	Append core id to pvxlan interface num.
 */
int nss_pvxlan_ifnum_with_core_id(int if_num)
{
	struct nss_ctx_instance *nss_ctx = nss_pvxlan_get_ctx();

	NSS_VERIFY_CTX_MAGIC(nss_ctx);

	if (!nss_is_dynamic_interface(if_num)) {
		nss_warning("%px: Invalid if_num: %d, must be a dynamic interface\n", nss_ctx, if_num);
		return 0;
	}
	return NSS_INTERFACE_NUM_APPEND_COREID(nss_ctx, if_num);
}
EXPORT_SYMBOL(nss_pvxlan_ifnum_with_core_id);

/*
 * nss_pvxlan_msg_init()
 *	Initialize pvxlan message.
 */
void nss_pvxlan_msg_init(struct nss_pvxlan_msg *ncm, uint16_t if_num, uint32_t type, uint32_t len,
				nss_pvxlan_msg_callback_t cb, void *app_data)
{
	nss_cmn_msg_init(&ncm->cm, if_num, type, len, (void*)cb, app_data);
}
EXPORT_SYMBOL(nss_pvxlan_msg_init);

/*
 * nss_pvxlan_get_ctx()
 *	Return a Pvxlan NSS context.
 */
struct nss_ctx_instance *nss_pvxlan_get_ctx()
{
	struct nss_ctx_instance *nss_ctx;

	nss_ctx = &nss_top_main.nss[nss_top_main.pvxlan_handler_id];
	return nss_ctx;
}
EXPORT_SYMBOL(nss_pvxlan_get_ctx);

/*
 * nss_pvxlan_init()
 *	Initializes Pvxlan. Gets called from nss_init.c.
 */
void nss_pvxlan_init()
{
	nss_pvxlan_stats_dentry_create();
	sema_init(&pvxlan_pvt.sem, 1);
	init_completion(&pvxlan_pvt.complete);

	memset(&nss_pvxlan_hdl, 0, sizeof(nss_pvxlan_hdl));
}

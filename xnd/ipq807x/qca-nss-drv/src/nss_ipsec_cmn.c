/*
 **************************************************************************
 * Copyright (c) 2018-2021, The Linux Foundation. All rights reserved.
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
#include "nss_dynamic_interface.h"
#include "nss_ipsec_cmn.h"
#include "nss_ppe.h"
#include "nss_ipsec_cmn_log.h"
#include "nss_ipsec_cmn_stats.h"
#include "nss_ipsec_cmn_strings.h"

#define NSS_IPSEC_CMN_TX_TIMEOUT 3000 /* 3 Seconds */
#define NSS_IPSEC_CMN_INTERFACE_MAX_LONG BITS_TO_LONGS(NSS_MAX_NET_INTERFACES)

/*
 * Private data structure for handling synchronous messaging.
 */
static struct nss_ipsec_cmn_pvt {
	struct semaphore sem;
	struct completion complete;
	struct nss_ipsec_cmn_msg nicm;
	unsigned long if_map[NSS_IPSEC_CMN_INTERFACE_MAX_LONG];
} ipsec_cmn_pvt;

/*
 * nss_ipsec_cmn_verify_ifnum()
 *	Verify if the interface number is a IPsec interface.
 */
static bool nss_ipsec_cmn_verify_ifnum(struct nss_ctx_instance *nss_ctx, uint32_t if_num)
{
	enum nss_dynamic_interface_type type = nss_dynamic_interface_get_type(nss_ctx, if_num);

	if (if_num == NSS_IPSEC_CMN_INTERFACE)
		return true;

	switch (type) {
	case NSS_DYNAMIC_INTERFACE_TYPE_IPSEC_CMN_INNER:
	case NSS_DYNAMIC_INTERFACE_TYPE_IPSEC_CMN_OUTER:
	case NSS_DYNAMIC_INTERFACE_TYPE_IPSEC_CMN_MDATA_INNER:
	case NSS_DYNAMIC_INTERFACE_TYPE_IPSEC_CMN_MDATA_OUTER:
	case NSS_DYNAMIC_INTERFACE_TYPE_IPSEC_CMN_REDIRECT:
		return true;

	default:
		return false;
	}

	return false;
}

/*
 * nss_ipsec_cmn_msg_handler()
 *	Handle NSS -> HLOS messages for IPSEC tunnel.
 */
static void nss_ipsec_cmn_msg_handler(struct nss_ctx_instance *nss_ctx, struct nss_cmn_msg *ncm, void *app_data)
{
	nss_ipsec_cmn_msg_callback_t cb;
	struct nss_ipsec_cmn_msg *nim;

	NSS_VERIFY_CTX_MAGIC(nss_ctx);

	/*
	 * Trace messages.
	 */
	nim = (struct nss_ipsec_cmn_msg *)ncm;
	nss_ipsec_cmn_log_rx_msg(nim);

	/*
	 * Is this a valid request/response packet?
	 */
	if (ncm->type >=  NSS_IPSEC_CMN_MSG_TYPE_MAX) {
		nss_warning("%px: Invalid message type(%u) for interface(%u)\n", nss_ctx, ncm->type, ncm->interface);
		return;
	}

	if (nss_cmn_get_msg_len(ncm) > sizeof(struct nss_ipsec_cmn_msg)) {
		nss_warning("%px: Invalid message length(%d)\n", nss_ctx, nss_cmn_get_msg_len(ncm));
		return;
	}

	if (ncm->type == NSS_IPSEC_CMN_MSG_TYPE_CTX_SYNC) {
		nss_ipsec_cmn_stats_sync(nss_ctx, ncm);
		nss_ipsec_cmn_stats_notify(nss_ctx, ncm->interface);
	}

	/*
	 * Update the callback and app_data for NOTIFY messages, ipsec_cmn sends all notify messages
	 * to the same callback/app_data.
	 */
	if (ncm->response == NSS_CMN_RESPONSE_NOTIFY) {
		ncm->cb = (nss_ptr_t)nss_core_get_msg_handler(nss_ctx, ncm->interface);
		ncm->app_data = (nss_ptr_t)nss_ctx->nss_rx_interface_handlers[ncm->interface].app_data;
	}

	/*
	 * Log failures
	 */
	nss_core_log_msg_failures(nss_ctx, ncm);

	/*
	 * Callback
	 */
	cb = (nss_ipsec_cmn_msg_callback_t)ncm->cb;
	app_data = (void *)ncm->app_data;

	/*
	 * Call IPsec message callback
	 */
	if (!cb) {
		nss_warning("%px: No callback for IPsec interface %d\n", nss_ctx, ncm->interface);
		return;
	}

	nss_trace("%px: calling ipsecsmgr message handler(%u)\n", nss_ctx, ncm->interface);
	cb(app_data, ncm);
}

/*
 * nss_ipsec_cmn_sync_resp()
 *	Callback to handle the completion of HLOS-->NSS messages.
 */
static void nss_ipsec_cmn_sync_resp(void *app_data, struct nss_cmn_msg *ncm)
{
	struct nss_ipsec_cmn_msg *pvt_msg = app_data;
	struct nss_ipsec_cmn_msg *resp_msg = container_of(ncm, struct nss_ipsec_cmn_msg, cm);

	/*
	 * Copy response message to pvt message
	 */
	memcpy(pvt_msg, resp_msg, sizeof(*resp_msg));

	/*
	 * Write memory barrier
	 */
	smp_wmb();

	complete(&ipsec_cmn_pvt.complete);
}

/*
 * nss_ipsec_cmn_ifmap_get()
 *	Return IPsec common active interfaces map.
 */
unsigned long *nss_ipsec_cmn_ifmap_get(void)
{
	return ipsec_cmn_pvt.if_map;
}

/*
 * nss_ipsec_cmn_get_context()
 *	Retrieve context for IPSEC redir.
 */
struct nss_ctx_instance *nss_ipsec_cmn_get_context(void)
{
	return (struct nss_ctx_instance *)&nss_top_main.nss[nss_top_main.ipsec_handler_id];
}
EXPORT_SYMBOL(nss_ipsec_cmn_get_context);

/*
 * nss_ipsec_cmn_get_ifnum_with_coreid()
 *	Return IPsec interface number with coreid.
 */
uint32_t nss_ipsec_cmn_get_ifnum_with_coreid(int32_t ifnum)
{
	struct nss_ctx_instance *nss_ctx = nss_ipsec_cmn_get_context();

	NSS_VERIFY_CTX_MAGIC(nss_ctx);
	return NSS_INTERFACE_NUM_APPEND_COREID(nss_ctx, ifnum);
}
EXPORT_SYMBOL(nss_ipsec_cmn_get_ifnum_with_coreid);

/*
 * nss_ipsec_cmn_msg_init()
 *	Initialize message
 */
void nss_ipsec_cmn_msg_init(struct nss_ipsec_cmn_msg *nim, uint16_t if_num, enum nss_ipsec_cmn_msg_type type,
				uint16_t len, nss_ipsec_cmn_msg_callback_t cb, void *app_data)
{
	nss_cmn_msg_init(&nim->cm, if_num, type, len, cb, app_data);
}
EXPORT_SYMBOL(nss_ipsec_cmn_msg_init);

/*
 * nss_ipsec_cmn_tx_msg()
 *	Transmit a IPSEC message to NSS FW.
 */
nss_tx_status_t nss_ipsec_cmn_tx_msg(struct nss_ctx_instance *nss_ctx, struct nss_ipsec_cmn_msg *msg)
{
	struct nss_cmn_msg *ncm = &msg->cm;

	/*
	 * Trace messages.
	 */
	nss_ipsec_cmn_log_tx_msg(msg);

	/*
	 * Sanity check the message
	 */
	if (ncm->type >= NSS_IPSEC_CMN_MSG_TYPE_MAX) {
		nss_warning("%px: Invalid message type(%u)\n", nss_ctx, ncm->type);
		return NSS_TX_FAILURE;
	}

	if (!nss_ipsec_cmn_verify_ifnum(nss_ctx, ncm->interface)) {
		nss_warning("%px: Invalid message interface(%u)\n", nss_ctx, ncm->interface);
		return NSS_TX_FAILURE;
	}

	if (nss_cmn_get_msg_len(ncm) > sizeof(struct nss_ipsec_cmn_msg)) {
		nss_warning("%px: Invalid message length(%u)\n", nss_ctx, nss_cmn_get_msg_len(ncm));
		return NSS_TX_FAILURE;
	}

	return nss_core_send_cmd(nss_ctx, msg, sizeof(*msg), NSS_NBUF_PAYLOAD_SIZE);
}
EXPORT_SYMBOL(nss_ipsec_cmn_tx_msg);

/*
 * nss_ipsec_cmn_tx_msg_sync()
 *	Transmit a IPSEC redir message to NSS firmware synchronously.
 */
nss_tx_status_t nss_ipsec_cmn_tx_msg_sync(struct nss_ctx_instance *nss_ctx, uint32_t if_num,
					enum nss_ipsec_cmn_msg_type type, uint16_t len,
					struct nss_ipsec_cmn_msg *nicm)
{
	struct nss_ipsec_cmn_msg *local_nicm = &ipsec_cmn_pvt.nicm;
	nss_tx_status_t status;
	int ret = 0;

	/*
	 * Length of the message should be the based on type
	 */
	if (len > sizeof(struct nss_ipsec_cmn_msg)) {
		nss_warning("%px: Invalid message length(%u), type (%d), I/F(%u)\n", nss_ctx, len, type, if_num);
		return NSS_TX_FAILURE;
	}

	down(&ipsec_cmn_pvt.sem);

	/*
	 * We need to copy the message content into the actual message
	 * to be sent to NSS
	 */
	memset(local_nicm, 0, sizeof(*local_nicm));

	nss_ipsec_cmn_msg_init(local_nicm, if_num, type, len, nss_ipsec_cmn_sync_resp, local_nicm);
	memcpy(&local_nicm->msg, &nicm->msg, len);

	status = nss_ipsec_cmn_tx_msg(nss_ctx, local_nicm);
	if (status != NSS_TX_SUCCESS) {
		nss_warning("%px: Failed to send message\n", nss_ctx);
		goto done;
	}

	ret = wait_for_completion_timeout(&ipsec_cmn_pvt.complete, msecs_to_jiffies(NSS_IPSEC_CMN_TX_TIMEOUT));
	if (!ret) {
		nss_warning("%px: Failed to receive response, timeout(%d)\n", nss_ctx, ret);
		status = NSS_TX_FAILURE_NOT_READY;
		goto done;
	}

	/*
	 * Read memory barrier
	 */
	smp_rmb();

	if (local_nicm->cm.response != NSS_CMN_RESPONSE_ACK) {
		status = NSS_TX_FAILURE;
		nicm->cm.response = local_nicm->cm.response;
		nicm->cm.error = local_nicm->cm.error;
		goto done;
	}

	/*
	 * Copy the message received
	 */
	memcpy(&nicm->msg, &local_nicm->msg, len);

done:
	up(&ipsec_cmn_pvt.sem);
	return status;
}
EXPORT_SYMBOL(nss_ipsec_cmn_tx_msg_sync);

/*
 * nss_ipsec_cmn_tx_buf()
 *	Send packet to IPsec interface in NSS.
 */
nss_tx_status_t nss_ipsec_cmn_tx_buf(struct nss_ctx_instance *nss_ctx, struct sk_buff *os_buf, uint32_t if_num)
{
	nss_trace("%px: Send to IPsec I/F(%u), skb(%px)\n", nss_ctx, if_num, os_buf);
	NSS_VERIFY_CTX_MAGIC(nss_ctx);

	if (!nss_ipsec_cmn_verify_ifnum(nss_ctx, if_num)) {
		nss_warning("%px: Interface number(%d) is not IPSec type\n", nss_ctx, if_num);
		return NSS_TX_FAILURE;
	}

	return nss_core_send_packet(nss_ctx, os_buf, if_num, H2N_BIT_FLAG_BUFFER_REUSABLE);
}
EXPORT_SYMBOL(nss_ipsec_cmn_tx_buf);

/*
 * nss_ipsec_cmn_register_if()
 *	Register dynamic node for IPSEC redir.
 */
struct nss_ctx_instance *nss_ipsec_cmn_register_if(uint32_t if_num, struct net_device *netdev,
						nss_ipsec_cmn_data_callback_t cb_data,
						nss_ipsec_cmn_msg_callback_t cb_msg,
						uint32_t features, enum nss_dynamic_interface_type type, void *app_ctx)
{
	struct nss_ctx_instance *nss_ctx = nss_ipsec_cmn_get_context();
	uint32_t status;

	if (!nss_ipsec_cmn_verify_ifnum(nss_ctx, if_num)) {
		nss_warning("%px: Invalid IPsec interface(%u)\n", nss_ctx, if_num);
		return NULL;
	}

	if (nss_ctx->subsys_dp_register[if_num].ndev) {
		nss_warning("%px: Failed find free slot for IPsec NSS I/F:%u\n", nss_ctx, if_num);
		return NULL;
	}

#ifdef NSS_DRV_PPE_ENABLE
	if (features & NSS_IPSEC_CMN_FEATURE_INLINE_ACCEL)
		nss_ppe_tx_ipsec_add_intf_msg(nss_ipsec_cmn_get_ifnum_with_coreid(if_num));
#endif

	/*
	 * Registering handler for sending tunnel interface msgs to NSS.
	 */
	status = nss_core_register_handler(nss_ctx, if_num, nss_ipsec_cmn_msg_handler, app_ctx);
	if (status != NSS_CORE_STATUS_SUCCESS){
		nss_warning("%px: Failed to register message handler for IPsec NSS I/F:%u\n", nss_ctx, if_num);
		return NULL;
	}

	status = nss_core_register_msg_handler(nss_ctx, if_num, cb_msg);
	if (status != NSS_CORE_STATUS_SUCCESS) {
		nss_core_unregister_handler(nss_ctx, if_num);
		nss_warning("%px: Failed to register message handler for IPsec NSS I/F:%u\n", nss_ctx, if_num);
		return NULL;
	}

	nss_core_register_subsys_dp(nss_ctx, if_num, cb_data, NULL, app_ctx, netdev, features);
	nss_core_set_subsys_dp_type(nss_ctx, netdev, if_num, type);

	/*
	 * Atomically set the bitmap for the interface number
	 */
	set_bit(if_num, ipsec_cmn_pvt.if_map);

	return nss_ctx;
}
EXPORT_SYMBOL(nss_ipsec_cmn_register_if);

/*
 * nss_ipsec_cmn_unregister_if()
 *	Unregister dynamic node for IPSEC redir.
 */
bool nss_ipsec_cmn_unregister_if(uint32_t if_num)
{
	struct nss_ctx_instance *nss_ctx = nss_ipsec_cmn_get_context();
	struct net_device *dev;
	uint32_t status;

	nss_assert(nss_ctx);

	if (!nss_ipsec_cmn_verify_ifnum(nss_ctx, if_num)) {
		nss_warning("%px: Invalid IPsec interface(%u)\n", nss_ctx, if_num);
		return false;
	}

	dev = nss_cmn_get_interface_dev(nss_ctx, if_num);
	if (!dev) {
		nss_warning("%px: Failed to find registered netdev for IPsec NSS I/F:%u\n", nss_ctx, if_num);
		return false;
	}

	nss_core_unregister_subsys_dp(nss_ctx, if_num);

	/*
	 * Atomically clear the bitmap for the interface number
	 */
	clear_bit(if_num, ipsec_cmn_pvt.if_map);

	status = nss_core_unregister_msg_handler(nss_ctx, if_num);
	if (status != NSS_CORE_STATUS_SUCCESS) {
		nss_warning("%px: Failed to unregister handler for IPsec NSS I/F:%u\n", nss_ctx, if_num);
		return false;
	}

	status = nss_core_unregister_handler(nss_ctx, if_num);
	if (status != NSS_CORE_STATUS_SUCCESS) {
		nss_warning("%px: Failed to unregister handler for IPsec NSS I/F:%u\n", nss_ctx, if_num);
		return false;
	}

	return true;
}
EXPORT_SYMBOL(nss_ipsec_cmn_unregister_if);

/*
 * nss_ipsec_cmn_notify_register()
 *	Register a handler for notification from NSS firmware.
 */
struct nss_ctx_instance *nss_ipsec_cmn_notify_register(uint32_t if_num, nss_ipsec_cmn_msg_callback_t cb, void *app_data)
{
	struct nss_ctx_instance *nss_ctx = nss_ipsec_cmn_get_context();
	uint32_t ret;

	BUG_ON(!nss_ctx);

	ret = nss_core_register_handler(nss_ctx, if_num, nss_ipsec_cmn_msg_handler, app_data);
	if (ret != NSS_CORE_STATUS_SUCCESS) {
		nss_warning("%px: unable to register event handler for interface(%u)\n", nss_ctx, if_num);
		return NULL;
	}

	ret = nss_core_register_msg_handler(nss_ctx, if_num, cb);
	if (ret != NSS_CORE_STATUS_SUCCESS) {
		nss_core_unregister_handler(nss_ctx, if_num);
		nss_warning("%px: Failed to register message handler for IPsec NSS I/F:%u\n", nss_ctx, if_num);
		return NULL;
	}

	return nss_ctx;
}
EXPORT_SYMBOL(nss_ipsec_cmn_notify_register);

/*
 * nss_ipsec_cmn_notify_unregister()
 * 	unregister the IPsec notifier for the given interface number (if_num)
 */
void nss_ipsec_cmn_notify_unregister(struct nss_ctx_instance *nss_ctx, uint32_t if_num)
{
	uint32_t ret;

	if (if_num >= NSS_MAX_NET_INTERFACES) {
		nss_warning("%px: notify unregister received for invalid interface %d\n", nss_ctx, if_num);
		return;
	}

	ret = nss_core_unregister_msg_handler(nss_ctx, if_num);
	if (ret != NSS_CORE_STATUS_SUCCESS) {
		nss_warning("%px: unable to unregister event handler for interface(%u)\n", nss_ctx, if_num);
		return;
	}

	ret = nss_core_unregister_handler(nss_ctx, if_num);
	if (ret != NSS_CORE_STATUS_SUCCESS) {
		nss_warning("%px: unable to unregister event handler for interface(%u)\n", nss_ctx, if_num);
		return;
	}
}
EXPORT_SYMBOL(nss_ipsec_cmn_notify_unregister);

/*
 * nss_ipsec_cmn_ppe_port_config()
 *	Configure PPE port for IPsec inline
 */
bool nss_ipsec_cmn_ppe_port_config(struct nss_ctx_instance *nss_ctx, struct net_device *netdev,
					uint32_t if_num, uint32_t vsi_num)
{
#ifdef NSS_PPE_SUPPORTED
	if_num = NSS_INTERFACE_NUM_APPEND_COREID(nss_ctx, if_num);

	if (nss_ppe_tx_ipsec_config_msg(if_num, vsi_num, netdev->mtu, netdev->mtu) != NSS_TX_SUCCESS) {
		nss_warning("%px: Failed to configure PPE IPsec port\n", nss_ctx);
		return false;
	}

	return true;
#else
	return false;
#endif
}
EXPORT_SYMBOL(nss_ipsec_cmn_ppe_port_config);

/*
 * nss_ipsec_cmn_ppe_mtu_update()
 *	Update PPE MTU for IPsec inline
 */
bool nss_ipsec_cmn_ppe_mtu_update(struct nss_ctx_instance *nss_ctx, uint32_t if_num, uint16_t mtu, uint16_t mru)
{
#ifdef NSS_PPE_SUPPORTED
	if_num = NSS_INTERFACE_NUM_APPEND_COREID(nss_ctx, if_num);

	if (nss_ppe_tx_ipsec_mtu_msg(if_num, mtu, mru) != NSS_TX_SUCCESS) {
		nss_warning("%px: Failed to update PPE MTU for IPsec port\n", nss_ctx);
		return false;
	}

	return true;
#else
	return false;
#endif
}
EXPORT_SYMBOL(nss_ipsec_cmn_ppe_mtu_update);

/*
 * nss_ipsec_cmn_register_handler()
 *	Registering handler for sending msg to base ipsec_cmn node on NSS.
 */
void nss_ipsec_cmn_register_handler(void)
{
	sema_init(&ipsec_cmn_pvt.sem, 1);
	init_completion(&ipsec_cmn_pvt.complete);
	nss_ipsec_cmn_stats_dentry_create();
	nss_ipsec_cmn_strings_dentry_create();
}

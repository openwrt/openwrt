/*
 **************************************************************************
 * Copyright (c) 2017-2018, 2020, The Linux Foundation. All rights reserved.
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
#include "nss_vlan_log.h"

#define NSS_VLAN_TX_TIMEOUT 1000 /* 1 Second */

/*
 * Private data structure
 */
static struct nss_vlan_pvt {
	struct semaphore sem;
	struct completion complete;
	int response;
	void *cb;
	void *app_data;
} vlan_pvt;

/*
 * nss_vlan_get_context()
 */
struct nss_ctx_instance *nss_vlan_get_context(void)
{
	return (struct nss_ctx_instance *)&nss_top_main.nss[nss_top_main.vlan_handler_id];
}
EXPORT_SYMBOL(nss_vlan_get_context);

/*
 * nss_vlan_verify_if_num()
 *	Verify if_num passed to us.
 */
static bool nss_vlan_verify_if_num(uint32_t if_num)
{
	if (!nss_is_dynamic_interface(if_num)) {
		return false;
	}

	if (nss_dynamic_interface_get_type(nss_vlan_get_context(), if_num) != NSS_DYNAMIC_INTERFACE_TYPE_VLAN) {
		return false;
	}

	return true;
}

/*
 * nss_vlan_handler()
 *	Handle NSS -> HLOS messages for vlan
 */
static void nss_vlan_handler(struct nss_ctx_instance *nss_ctx, struct nss_cmn_msg *ncm, void *app_data)
{
	struct nss_vlan_msg *nvm = (struct nss_vlan_msg *)ncm;
	nss_vlan_msg_callback_t cb;

	nss_assert(nss_vlan_verify_if_num(ncm->interface));

	/*
	 * Trace messages.
	 */
	nss_vlan_log_rx_msg(nvm);

	/*
	 * Is this a valid request/response packet?
	 */
	if (ncm->type >= NSS_VLAN_MSG_TYPE_MAX) {
		nss_warning("%px: received invalid message %d for vlan interface", nss_ctx, ncm->type);
		return;
	}

	if (nss_cmn_get_msg_len(ncm) > sizeof(struct nss_vlan_msg)) {
		nss_warning("%px: length of message is greater than required: %d", nss_ctx, nss_cmn_get_msg_len(ncm));
		return;
	}

	/*
	 * Update the callback and app_data for NOTIFY messages, vlan sends all notify messages
	 * to the same callback/app_data.
	 */
	if (ncm->response == NSS_CMN_RESPONSE_NOTIFY) {
		ncm->cb = (nss_ptr_t)nss_ctx->nss_top->vlan_callback;
		ncm->app_data = (nss_ptr_t)app_data;
	}

	/*
	 * Log failures
	 */
	nss_core_log_msg_failures(nss_ctx, ncm);

	/*
	 * Do we have a call back
	 */
	if (!ncm->cb) {
		return;
	}

	/*
	 * callback
	 */
	cb = (nss_vlan_msg_callback_t)ncm->cb;
	cb((void *)ncm->app_data, nvm);
}

/*
 * nss_vlan_callback()
 *	Callback to handle the completion of NSS->HLOS messages.
 */
static void nss_vlan_callback(void *app_data, struct nss_vlan_msg *nvm)
{
	nss_vlan_msg_callback_t callback = (nss_vlan_msg_callback_t)vlan_pvt.cb;
	void *data = vlan_pvt.app_data;

	vlan_pvt.response = NSS_TX_SUCCESS;
	vlan_pvt.cb = NULL;
	vlan_pvt.app_data = NULL;

	if (nvm->cm.response != NSS_CMN_RESPONSE_ACK) {
		nss_warning("vlan error response %d\n", nvm->cm.response);
		vlan_pvt.response = nvm->cm.response;
	}

	if (callback) {
		callback(data, nvm);
	}
	complete(&vlan_pvt.complete);
}

/*
 * nss_vlan_tx_msg()
 *	Transmit a vlan message to NSSFW
 */
nss_tx_status_t nss_vlan_tx_msg(struct nss_ctx_instance *nss_ctx, struct nss_vlan_msg *msg)
{
	struct nss_cmn_msg *ncm = &msg->cm;

	/*
	 * Trace messages.
	 */
	nss_vlan_log_tx_msg(msg);

	/*
	 * Sanity check the message
	 */
	if (!nss_vlan_verify_if_num(ncm->interface)) {
		nss_warning("%px: tx request for interface that is not a vlan: %d", nss_ctx, ncm->interface);
		return NSS_TX_FAILURE;
	}

	if (ncm->type >= NSS_VLAN_MSG_TYPE_MAX) {
		nss_warning("%px: message type out of range: %d", nss_ctx, ncm->type);
		return NSS_TX_FAILURE;
	}

	return nss_core_send_cmd(nss_ctx, msg, sizeof(*msg), NSS_NBUF_PAYLOAD_SIZE);
}
EXPORT_SYMBOL(nss_vlan_tx_msg);

/*
 * nss_vlan_tx_msg_sync()
 *	Transmit a vlan message to NSS firmware synchronously.
 */
nss_tx_status_t nss_vlan_tx_msg_sync(struct nss_ctx_instance *nss_ctx, struct nss_vlan_msg *nvm)
{
	nss_tx_status_t status;
	int ret = 0;

	down(&vlan_pvt.sem);
	vlan_pvt.cb = (void *)nvm->cm.cb;
	vlan_pvt.app_data = (void *)nvm->cm.app_data;

	nvm->cm.cb = (nss_ptr_t)nss_vlan_callback;
	nvm->cm.app_data = (nss_ptr_t)NULL;

	status = nss_vlan_tx_msg(nss_ctx, nvm);
	if (status != NSS_TX_SUCCESS) {
		nss_warning("%px: vlan_tx_msg failed\n", nss_ctx);
		up(&vlan_pvt.sem);
		return status;
	}

	ret = wait_for_completion_timeout(&vlan_pvt.complete, msecs_to_jiffies(NSS_VLAN_TX_TIMEOUT));
	if (!ret) {
		nss_warning("%px: vlan msg tx failed due to timeout\n", nss_ctx);
		vlan_pvt.response = NSS_TX_FAILURE;
	}

	status = vlan_pvt.response;
	up(&vlan_pvt.sem);
	return status;
}
EXPORT_SYMBOL(nss_vlan_tx_msg_sync);

/*
 * nss_vlan_msg_init()
 *	Initialize nss_vlan_msg.
 */
void nss_vlan_msg_init(struct nss_vlan_msg *ncm, uint16_t if_num, uint32_t type, uint32_t len, void *cb, void *app_data)
{
	nss_cmn_msg_init(&ncm->cm, if_num, type, len, cb, app_data);
}
EXPORT_SYMBOL(nss_vlan_msg_init);

/*
 * nss_vlan_tx_change_mtu_msg
 *	API to send change mtu message to NSS FW
 */
nss_tx_status_t nss_vlan_tx_set_mtu_msg(uint32_t vlan_if_num, uint32_t mtu)
{
	struct nss_ctx_instance *nss_ctx = nss_vlan_get_context();
	struct nss_vlan_msg nvm;
	struct nss_if_mtu_change *nimc;

	if (!nss_ctx) {
		nss_warning("Can't get nss context\n");
		return NSS_TX_FAILURE;
	}

	if (nss_vlan_verify_if_num(vlan_if_num) == false) {
		nss_warning("%px: received invalid interface %d", nss_ctx, vlan_if_num);
		return NSS_TX_FAILURE;
	}

	nss_vlan_msg_init(&nvm, vlan_if_num, NSS_IF_MTU_CHANGE,
			sizeof(struct nss_if_mtu_change), NULL, NULL);

	nimc = &nvm.msg.if_msg.mtu_change;
	nimc->min_buf_size = (uint16_t)mtu;

	return nss_vlan_tx_msg_sync(nss_ctx, &nvm);
}
EXPORT_SYMBOL(nss_vlan_tx_set_mtu_msg);

/*
 * nss_vlan_tx_set_mac_addr_msg
 *	API to send change mac addr message to NSS FW
 */
nss_tx_status_t nss_vlan_tx_set_mac_addr_msg(uint32_t vlan_if_num, uint8_t *addr)
{
	struct nss_ctx_instance *nss_ctx = nss_vlan_get_context();
	struct nss_vlan_msg nvm;
	struct nss_if_mac_address_set *nmas;

	if (!nss_ctx) {
		nss_warning("Can't get nss context\n");
		return NSS_TX_FAILURE;
	}

	if (nss_vlan_verify_if_num(vlan_if_num) == false) {
		nss_warning("%px: received invalid interface %d", nss_ctx, vlan_if_num);
		return NSS_TX_FAILURE;
	}

	nss_vlan_msg_init(&nvm, vlan_if_num, NSS_IF_MAC_ADDR_SET,
			sizeof(struct nss_if_mac_address_set), NULL, NULL);

	nmas = &nvm.msg.if_msg.mac_address_set;
	memcpy(nmas->mac_addr, addr, ETH_ALEN);
	return nss_vlan_tx_msg_sync(nss_ctx, &nvm);
}
EXPORT_SYMBOL(nss_vlan_tx_set_mac_addr_msg);

/*
 * nss_vlan_tx_vsi_attach_msg
 *	API to send VSI attach message to NSS FW
 */
nss_tx_status_t nss_vlan_tx_vsi_attach_msg(uint32_t vlan_if_num, uint32_t vsi)
{
	struct nss_ctx_instance *nss_ctx = nss_vlan_get_context();
	struct nss_vlan_msg nvm;

	if (!nss_ctx) {
		nss_warning("Can't get nss context\n");
		return NSS_TX_FAILURE;
	}

	if (nss_vlan_verify_if_num(vlan_if_num) == false) {
		nss_warning("%px: received invalid interface %d\n", nss_ctx, vlan_if_num);
		return NSS_TX_FAILURE;
	}

	nvm.msg.if_msg.vsi_assign.vsi = vsi;
	nss_vlan_msg_init(&nvm, vlan_if_num, NSS_IF_VSI_ASSIGN,
				sizeof(struct nss_if_vsi_assign), NULL, NULL);

	return nss_vlan_tx_msg_sync(nss_ctx, &nvm);
}
EXPORT_SYMBOL(nss_vlan_tx_vsi_attach_msg);

/*
 * nss_vlan_tx_vsi_detach_msg
 *	API to send VSI detach message to NSS FW
 */
nss_tx_status_t nss_vlan_tx_vsi_detach_msg(uint32_t vlan_if_num, uint32_t vsi)
{
	struct nss_ctx_instance *nss_ctx = nss_vlan_get_context();
	struct nss_vlan_msg nvm;

	if (!nss_ctx) {
		nss_warning("Can't get nss context\n");
		return NSS_TX_FAILURE;
	}

	if (nss_vlan_verify_if_num(vlan_if_num) == false) {
		nss_warning("%px: received invalid interface %d\n", nss_ctx, vlan_if_num);
		return NSS_TX_FAILURE;
	}

	nvm.msg.if_msg.vsi_unassign.vsi = vsi;
	nss_vlan_msg_init(&nvm, vlan_if_num, NSS_IF_VSI_UNASSIGN,
				sizeof(struct nss_if_vsi_unassign), NULL, NULL);

	return nss_vlan_tx_msg_sync(nss_ctx, &nvm);
}
EXPORT_SYMBOL(nss_vlan_tx_vsi_detach_msg);

/*
 * nss_vlan_tx_add_tag_msg
 *	API to send vlan add tag message to NSS FW
 */
nss_tx_status_t nss_vlan_tx_add_tag_msg(uint32_t vlan_if_num, uint32_t vlan_tag, uint32_t next_hop, uint32_t physical_dev)
{
	struct nss_ctx_instance *nss_ctx = nss_vlan_get_context();
	struct nss_vlan_msg nvm;

	if (!nss_ctx) {
		nss_warning("Can't get nss context\n");
		return NSS_TX_FAILURE;
	}

	if (nss_vlan_verify_if_num(vlan_if_num) == false) {
		nss_warning("%px: received invalid interface %d\n", nss_ctx, vlan_if_num);
		return NSS_TX_FAILURE;
	}

	nvm.msg.add_tag.next_hop = next_hop;
	nvm.msg.add_tag.if_num = physical_dev;
	nvm.msg.add_tag.vlan_tag = vlan_tag;
	nss_vlan_msg_init(&nvm, vlan_if_num, NSS_VLAN_MSG_ADD_TAG,
				sizeof(struct nss_vlan_msg_add_tag), NULL, NULL);

	return nss_vlan_tx_msg_sync(nss_ctx, &nvm);
}
EXPORT_SYMBOL(nss_vlan_tx_add_tag_msg);

/**
 * @brief Register to send/receive vlan messages to NSS
 *
 * @param if_num NSS interface number
 * @param vlan_data_callback Callback for vlan data
 * @param netdev netdevice associated with the vlan interface
 * @param features denotes the skb types supported by this interface
 *
 * @return nss_ctx_instance* NSS context
 */
struct nss_ctx_instance *nss_register_vlan_if(uint32_t if_num, nss_vlan_callback_t vlan_data_callback,
					      struct net_device *netdev, uint32_t features, void *app_ctx)
{
	struct nss_ctx_instance *nss_ctx = nss_vlan_get_context();

	nss_assert(nss_vlan_verify_if_num(if_num));

	nss_core_register_subsys_dp(nss_ctx, if_num, vlan_data_callback, NULL, app_ctx, netdev, features);

	nss_core_register_handler(nss_ctx, if_num, nss_vlan_handler, app_ctx);

	return nss_ctx;
}
EXPORT_SYMBOL(nss_register_vlan_if);

/*
 * nss_unregister_vlan_if()
 */
void nss_unregister_vlan_if(uint32_t if_num)
{
	struct nss_ctx_instance *nss_ctx = nss_vlan_get_context();

	nss_assert(nss_vlan_verify_if_num(if_num));

	nss_core_unregister_subsys_dp(nss_ctx, if_num);

	nss_core_unregister_handler(nss_ctx, if_num);
}
EXPORT_SYMBOL(nss_unregister_vlan_if);

/*
 * nss_vlan_register_handler()
 *   debugfs stats msg handler received on static vlan interface
 */
void nss_vlan_register_handler(void)
{
	struct nss_ctx_instance *nss_ctx = nss_vlan_get_context();

	nss_info("nss_vlan_register_handler\n");
	nss_core_register_handler(nss_ctx, NSS_VLAN_INTERFACE, nss_vlan_handler, NULL);

	sema_init(&vlan_pvt.sem, 1);
	init_completion(&vlan_pvt.complete);
}
EXPORT_SYMBOL(nss_vlan_register_handler);

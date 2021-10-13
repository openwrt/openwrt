/*
 **************************************************************************
 * Copyright (c) 2016-2018, 2020, The Linux Foundation. All rights reserved.
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
#include "nss_bridge_log.h"

#define NSS_BRIDGE_TX_TIMEOUT 1000 /* 1 Second */

/*
 * Private data structure
 */
static struct nss_bridge_pvt {
	struct semaphore sem;
	struct completion complete;
	int response;
	void *cb;
	void *app_data;
} bridge_pvt;

/*
 * nss_bridge_handler()
 * 	Handle NSS -> HLOS messages for bridge
 */
static void nss_bridge_handler(struct nss_ctx_instance *nss_ctx, struct nss_cmn_msg *ncm, void *app_data)
{
	struct nss_bridge_msg *nbm = (struct nss_bridge_msg *)ncm;
	nss_bridge_msg_callback_t cb;

	BUG_ON(!nss_is_dynamic_interface(ncm->interface));

	/*
	 * Is this a valid request/response packet?
	 */
	if (ncm->type >= NSS_BRIDGE_MSG_TYPE_MAX) {
		nss_warning("%px: received invalid message %d for bridge interface", nss_ctx, ncm->type);
		return;
	}

	if (nss_cmn_get_msg_len(ncm) > sizeof(struct nss_bridge_msg)) {
		nss_warning("%px: length of message is greater than required: %d", nss_ctx, nss_cmn_get_msg_len(ncm));
		return;
	}

	/*
	 * Trace Messages
	 */
	nss_bridge_log_rx_msg(nbm);

	/*
	 * Log failures
	 */
	nss_core_log_msg_failures(nss_ctx, ncm);

	/*
	 * Update the callback and app_data for NOTIFY messages, IPv4 sends all notify messages
	 * to the same callback/app_data.
	 */
	if (ncm->response == NSS_CMN_RESPONSE_NOTIFY) {
		ncm->cb = (nss_ptr_t)nss_ctx->nss_top->bridge_callback;
		ncm->app_data = (nss_ptr_t)nss_ctx->nss_top->bridge_ctx;
	}

	/*
	 * Do we have a call back
	 */
	if (!ncm->cb) {
		return;
	}

	/*
	 * callback
	 */
	cb = (nss_bridge_msg_callback_t)ncm->cb;
	cb((void *)ncm->app_data, nbm);
}

/*
 * nss_bridge_get_context()
 */
struct nss_ctx_instance *nss_bridge_get_context(void)
{
	return (struct nss_ctx_instance *)&nss_top_main.nss[nss_top_main.bridge_handler_id];
}
EXPORT_SYMBOL(nss_bridge_get_context);

/*
 * nss_bridge_callback()
 *	Callback to handle the completion of NSS->HLOS messages.
 */
static void nss_bridge_callback(void *app_data, struct nss_bridge_msg *nbm)
{
	nss_bridge_msg_callback_t callback = (nss_bridge_msg_callback_t)bridge_pvt.cb;
	void *data = bridge_pvt.app_data;

	bridge_pvt.response = NSS_TX_SUCCESS;
	bridge_pvt.cb = NULL;
	bridge_pvt.app_data = NULL;

	if (nbm->cm.response != NSS_CMN_RESPONSE_ACK) {
		nss_warning("bridge error response %d\n", nbm->cm.response);
		bridge_pvt.response = nbm->cm.response;
	}

	if (callback) {
		callback(data, nbm);
	}
	complete(&bridge_pvt.complete);
}

/*
 * nss_bridge_verify_if_num()
 *	Verify if_num passed to us.
 */
bool nss_bridge_verify_if_num(uint32_t if_num)
{
	if (nss_is_dynamic_interface(if_num) == false) {
		return false;
	}

	if (nss_dynamic_interface_get_type(nss_bridge_get_context(), if_num) != NSS_DYNAMIC_INTERFACE_TYPE_BRIDGE) {
		return false;
	}

	return true;
}
EXPORT_SYMBOL(nss_bridge_verify_if_num);

/*
 * nss_bridge_tx_msg()
 *	Transmit a bridge message to NSSFW
 */
nss_tx_status_t nss_bridge_tx_msg(struct nss_ctx_instance *nss_ctx, struct nss_bridge_msg *msg)
{
	struct nss_cmn_msg *ncm = &msg->cm;

	/*
	 * Sanity check the message
	 */
	if (!nss_is_dynamic_interface(ncm->interface)) {
		nss_warning("%px: tx request for interface that is not a bridge: %d", nss_ctx, ncm->interface);
		return NSS_TX_FAILURE;
	}

	if (ncm->type >= NSS_BRIDGE_MSG_TYPE_MAX) {
		nss_warning("%px: message type out of range: %d", nss_ctx, ncm->type);
		return NSS_TX_FAILURE;
	}

	/*
	 * Trace Messages
	 */
	nss_bridge_log_tx_msg(msg);

	return nss_core_send_cmd(nss_ctx, msg, sizeof(*msg), NSS_NBUF_PAYLOAD_SIZE);
}
EXPORT_SYMBOL(nss_bridge_tx_msg);

/*
 * nss_bridge_tx_msg_sync()
 *	Transmit a bridge message to NSS firmware synchronously.
 */
nss_tx_status_t nss_bridge_tx_msg_sync(struct nss_ctx_instance *nss_ctx, struct nss_bridge_msg *nbm)
{
	nss_tx_status_t status;
	int ret = 0;

	down(&bridge_pvt.sem);
	bridge_pvt.cb = (void *)nbm->cm.cb;
	bridge_pvt.app_data = (void *)nbm->cm.app_data;

	nbm->cm.cb = (nss_ptr_t)nss_bridge_callback;
	nbm->cm.app_data = (nss_ptr_t)NULL;

	status = nss_bridge_tx_msg(nss_ctx, nbm);
	if (status != NSS_TX_SUCCESS) {
		nss_warning("%px: bridge_tx_msg failed\n", nss_ctx);
		up(&bridge_pvt.sem);
		return status;
	}

	ret = wait_for_completion_timeout(&bridge_pvt.complete, msecs_to_jiffies(NSS_BRIDGE_TX_TIMEOUT));

	if (!ret) {
		nss_warning("%px: bridge msg tx failed due to timeout\n", nss_ctx);
		bridge_pvt.response = NSS_TX_FAILURE;
	}

	status = bridge_pvt.response;
	up(&bridge_pvt.sem);
	return status;
}
EXPORT_SYMBOL(nss_bridge_tx_msg_sync);

/*
 * nss_bridge_msg_init()
 *	Initialize nss_bridge_msg.
 */
void nss_bridge_msg_init(struct nss_bridge_msg *ncm, uint16_t if_num, uint32_t type, uint32_t len, void *cb, void *app_data)
{
	nss_cmn_msg_init(&ncm->cm, if_num, type, len, cb, app_data);
}
EXPORT_SYMBOL(nss_bridge_msg_init);

/*
 * nss_bridge_tx_vsi_assign_msg
 *	API to send vsi assign message to NSS FW
 */
nss_tx_status_t nss_bridge_tx_vsi_assign_msg(uint32_t if_num, uint32_t vsi)
{
	struct nss_ctx_instance *nss_ctx = nss_bridge_get_context();
	struct nss_bridge_msg nbm;

	if (!nss_ctx) {
		nss_warning("Can't get nss context\n");
		return NSS_TX_FAILURE;
	}

	if (nss_bridge_verify_if_num(if_num) == false) {
		nss_warning("%px: invalid interface %d", nss_ctx, if_num);
		return NSS_TX_FAILURE;
	}

	nss_bridge_msg_init(&nbm, if_num, NSS_IF_VSI_ASSIGN,
			sizeof(struct nss_if_vsi_assign), NULL, NULL);

	nbm.msg.if_msg.vsi_assign.vsi = vsi;

	return nss_bridge_tx_msg_sync(nss_ctx, &nbm);
}
EXPORT_SYMBOL(nss_bridge_tx_vsi_assign_msg);

/*
 * nss_bridge_tx_vsi_unassign_msg
 *	API to send vsi unassign message to NSS FW
 */
nss_tx_status_t nss_bridge_tx_vsi_unassign_msg(uint32_t if_num, uint32_t vsi)
{
	struct nss_ctx_instance *nss_ctx = nss_bridge_get_context();
	struct nss_bridge_msg nbm;

	if (!nss_ctx) {
		nss_warning("Can't get nss context\n");
		return NSS_TX_FAILURE;
	}

	if (nss_bridge_verify_if_num(if_num) == false) {
		nss_warning("%px: invalid interface %d", nss_ctx, if_num);
		return NSS_TX_FAILURE;
	}

	nss_bridge_msg_init(&nbm, if_num, NSS_IF_VSI_UNASSIGN,
			sizeof(struct nss_if_vsi_unassign), NULL, NULL);

	nbm.msg.if_msg.vsi_unassign.vsi = vsi;

	return nss_bridge_tx_msg_sync(nss_ctx, &nbm);
}
EXPORT_SYMBOL(nss_bridge_tx_vsi_unassign_msg);

/*
 * nss_bridge_tx_change_mtu_msg
 *	API to send change mtu message to NSS FW
 */
nss_tx_status_t nss_bridge_tx_set_mtu_msg(uint32_t bridge_if_num, uint32_t mtu)
{
	struct nss_ctx_instance *nss_ctx = nss_bridge_get_context();
	struct nss_bridge_msg nbm;
	struct nss_if_mtu_change *nimc;

	if (!nss_ctx) {
		nss_warning("Can't get nss context\n");
		return NSS_TX_FAILURE;
	}

	if (nss_bridge_verify_if_num(bridge_if_num) == false) {
		nss_warning("%px: received invalid interface %d", nss_ctx, bridge_if_num);
		return NSS_TX_FAILURE;
	}

	nss_bridge_msg_init(&nbm, bridge_if_num, NSS_IF_MTU_CHANGE,
			sizeof(struct nss_if_mtu_change), NULL, NULL);

	nimc = &nbm.msg.if_msg.mtu_change;
	nimc->min_buf_size = (uint16_t)mtu;

	return nss_bridge_tx_msg_sync(nss_ctx, &nbm);
}
EXPORT_SYMBOL(nss_bridge_tx_set_mtu_msg);

/*
 * nss_bridge_tx_set_mac_addr_msg
 *	API to send change mac addr message to NSS FW
 */
nss_tx_status_t nss_bridge_tx_set_mac_addr_msg(uint32_t bridge_if_num, uint8_t *addr)
{
	struct nss_ctx_instance *nss_ctx = nss_bridge_get_context();
	struct nss_bridge_msg nbm;
	struct nss_if_mac_address_set *nmas;

	if (!nss_ctx) {
		nss_warning("Can't get nss context\n");
		return NSS_TX_FAILURE;
	}

	if (nss_bridge_verify_if_num(bridge_if_num) == false) {
		nss_warning("%px: received invalid interface %d", nss_ctx, bridge_if_num);
		return NSS_TX_FAILURE;
	}

	nss_bridge_msg_init(&nbm, bridge_if_num, NSS_IF_MAC_ADDR_SET,
			sizeof(struct nss_if_mac_address_set), NULL, NULL);

	nmas = &nbm.msg.if_msg.mac_address_set;
	memcpy(nmas->mac_addr, addr, ETH_ALEN);
	return nss_bridge_tx_msg_sync(nss_ctx, &nbm);
}
EXPORT_SYMBOL(nss_bridge_tx_set_mac_addr_msg);

/*
 * nss_bridge_tx_join_msg
 *	API to send slave join message to NSS FW
 */
nss_tx_status_t nss_bridge_tx_join_msg(uint32_t bridge_if_num, struct net_device *netdev)
{
	struct nss_ctx_instance *nss_ctx = nss_bridge_get_context();
	struct nss_bridge_msg nbm;
	uint32_t slave_if_num;

	if (!nss_ctx) {
		nss_warning("Can't get nss context\n");
		return NSS_TX_FAILURE;
	}

	if (nss_bridge_verify_if_num(bridge_if_num) == false) {
		nss_warning("%px: received invalid interface %d\n", nss_ctx, bridge_if_num);
		return NSS_TX_FAILURE;
	}

	slave_if_num = nss_cmn_get_interface_number_by_dev(netdev);
	if (slave_if_num < 0) {
		nss_warning("%px: invalid slave device %px\n", nss_ctx, netdev);
		return NSS_TX_FAILURE;
	}

	nbm.msg.br_join.if_num = slave_if_num;
	nss_bridge_msg_init(&nbm, bridge_if_num, NSS_BRIDGE_MSG_JOIN,
				sizeof(struct nss_bridge_join_msg), NULL, NULL);

	return nss_bridge_tx_msg_sync(nss_ctx, &nbm);
}
EXPORT_SYMBOL(nss_bridge_tx_join_msg);

/*
 * nss_bridge_tx_leave_msg
 *	API to send slave leave message to NSS FW
 */
nss_tx_status_t nss_bridge_tx_leave_msg(uint32_t bridge_if_num, struct net_device *netdev)
{
	struct nss_ctx_instance *nss_ctx = nss_bridge_get_context();
	struct nss_bridge_msg nbm;
	uint32_t slave_if_num;

	if (!nss_ctx) {
		nss_warning("Can't get nss context\n");
		return NSS_TX_FAILURE;
	}

	if (nss_bridge_verify_if_num(bridge_if_num) == false) {
		nss_warning("%px: received invalid interface %d\n", nss_ctx, bridge_if_num);
		return NSS_TX_FAILURE;
	}

	slave_if_num = nss_cmn_get_interface_number_by_dev(netdev);
	if (slave_if_num < 0) {
		nss_warning("%px: invalid slave device %px\n", nss_ctx, netdev);
		return NSS_TX_FAILURE;
	}

	nbm.msg.br_leave.if_num = slave_if_num;
	nss_bridge_msg_init(&nbm, bridge_if_num, NSS_BRIDGE_MSG_LEAVE,
				sizeof(struct nss_bridge_leave_msg), NULL, NULL);

	return nss_bridge_tx_msg_sync(nss_ctx, &nbm);
}
EXPORT_SYMBOL(nss_bridge_tx_leave_msg);

/*
 * nss_bridge_tx_set_fdb_learn_msg
 *	API to send FDB learn message to NSS FW
 */
nss_tx_status_t nss_bridge_tx_set_fdb_learn_msg(uint32_t bridge_if_num, enum nss_bridge_fdb_learn_mode fdb_learn)
{
	struct nss_ctx_instance *nss_ctx = nss_bridge_get_context();
	struct nss_bridge_msg nbm;

	if (!nss_ctx) {
		nss_warning("Can't get nss context\n");
		return NSS_TX_FAILURE;
	}

	if (nss_bridge_verify_if_num(bridge_if_num) == false) {
		nss_warning("%px: received invalid interface %d\n", nss_ctx, bridge_if_num);
		return NSS_TX_FAILURE;
	}

	if (fdb_learn >= NSS_BRIDGE_FDB_LEARN_MODE_MAX) {
		nss_warning("%px: received invalid fdb learn mode %d\n", nss_ctx, fdb_learn);
		return NSS_TX_FAILURE;
	}

	nss_bridge_msg_init(&nbm, bridge_if_num, NSS_BRIDGE_MSG_SET_FDB_LEARN,
				sizeof(struct nss_bridge_set_fdb_learn_msg), NULL, NULL);

	nbm.msg.fdb_learn.mode = fdb_learn;

	return nss_bridge_tx_msg_sync(nss_ctx, &nbm);
}
EXPORT_SYMBOL(nss_bridge_tx_set_fdb_learn_msg);

/*
 * nss_bridge_init()
 */
void nss_bridge_init(void)
{
	sema_init(&bridge_pvt.sem, 1);
	init_completion(&bridge_pvt.complete);
}

/*
 * nss_bridge_unregister()
 */
void nss_bridge_unregister(uint32_t if_num)
{
	struct nss_ctx_instance *nss_ctx = nss_bridge_get_context();

	nss_assert(nss_bridge_verify_if_num(if_num));

	nss_core_unregister_subsys_dp(nss_ctx, if_num);

	nss_top_main.bridge_callback = NULL;

	nss_core_unregister_handler(nss_ctx, if_num);
}
EXPORT_SYMBOL(nss_bridge_unregister);

/*
 * nss_bridge_register()
 */
struct nss_ctx_instance *nss_bridge_register(uint32_t if_num, struct net_device *netdev,
						nss_bridge_callback_t bridge_data_cb,
						nss_bridge_msg_callback_t bridge_msg_cb,
						uint32_t features,
						void *app_data)
{
	struct nss_ctx_instance *nss_ctx = nss_bridge_get_context();

	nss_assert(nss_bridge_verify_if_num(if_num));

	nss_core_register_subsys_dp(nss_ctx, if_num, bridge_data_cb, NULL, app_data, netdev, features);

	nss_top_main.bridge_callback = bridge_msg_cb;

	nss_core_register_handler(nss_ctx, if_num, nss_bridge_handler, app_data);
	return nss_ctx;
}
EXPORT_SYMBOL(nss_bridge_register);

/*
 * nss_bridge_notify_register()
 *	Register to receive bridge notify messages.
 */
struct nss_ctx_instance *nss_bridge_notify_register(nss_bridge_msg_callback_t cb, void *app_data)
{
	nss_top_main.bridge_callback = cb;
	nss_top_main.bridge_ctx = app_data;
	return nss_bridge_get_context();
}
EXPORT_SYMBOL(nss_bridge_notify_register);

/*
 * nss_bridge_notify_unregister()
 *	Unregister to receive bridge notify messages.
 */
void nss_bridge_notify_unregister(void)
{
	nss_top_main.bridge_callback = NULL;
}
EXPORT_SYMBOL(nss_bridge_notify_unregister);

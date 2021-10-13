/*
 **************************************************************************
 * Copyright (c) 2016-2018, 2020-2021, The Linux Foundation. All rights reserved.
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

#include "nss_ppe.h"
#include "nss_ppe_stats.h"
#include "nss_ppe_strings.h"

DEFINE_SPINLOCK(nss_ppe_stats_lock);

struct nss_ppe_stats_debug nss_ppe_debug_stats;
struct nss_ppe_pvt ppe_pvt;

/*
 * nss_ppe_verify_ifnum()
 *	Verify PPE interface number.
 */
static inline bool nss_ppe_verify_ifnum(int if_num)
{
	return nss_is_dynamic_interface(if_num) || (if_num == NSS_PPE_INTERFACE);
}

/*
 * nss_ppe_callback()
 *	Callback to handle the completion of NSS->HLOS messages.
 */
static void nss_ppe_callback(void *app_data, struct nss_ppe_msg *npm)
{
	nss_ppe_msg_callback_t callback = (nss_ppe_msg_callback_t)ppe_pvt.cb;
	void *data = ppe_pvt.app_data;

	ppe_pvt.response = NSS_TX_SUCCESS;
	ppe_pvt.cb = NULL;
	ppe_pvt.app_data = NULL;

	if (npm->cm.response != NSS_CMN_RESPONSE_ACK) {
		nss_warning("ppe error response %d\n", npm->cm.response);
		ppe_pvt.response = npm->cm.response;
	}

	if (callback) {
		callback(data, npm);
	}
	complete(&ppe_pvt.complete);
}

/*
 * nss_ppe_tx_msg()
 *	Transmit a ppe message to NSSFW
 */
nss_tx_status_t nss_ppe_tx_msg(struct nss_ctx_instance *nss_ctx, struct nss_ppe_msg *msg)
{
	struct nss_cmn_msg *ncm = &msg->cm;

	/*
	 * Trace messages.
	 */
	nss_ppe_log_tx_msg(msg);

	/*
	 * Sanity check the message
	 */
	if (ncm->type >= NSS_PPE_MSG_MAX) {
		nss_warning("%px: message type out of range: %d\n", nss_ctx, ncm->type);
		return NSS_TX_FAILURE;
	}

	if (!nss_ppe_verify_ifnum(ncm->interface)) {
		nss_warning("%px: invalid interface %d\n", nss_ctx, ncm->interface);
		return NSS_TX_FAILURE;
	}

	return nss_core_send_cmd(nss_ctx, msg, sizeof(*msg), NSS_NBUF_PAYLOAD_SIZE);
}

/*
 * nss_ppe_tx_msg_sync()
 *	Transmit a ppe message to NSS firmware synchronously.
 */
nss_tx_status_t nss_ppe_tx_msg_sync(struct nss_ctx_instance *nss_ctx, struct nss_ppe_msg *npm)
{
	nss_tx_status_t status;
	int ret = 0;

	down(&ppe_pvt.sem);
	ppe_pvt.cb = (void *)npm->cm.cb;
	ppe_pvt.app_data = (void *)npm->cm.app_data;

	npm->cm.cb = (nss_ptr_t)nss_ppe_callback;
	npm->cm.app_data = (nss_ptr_t)NULL;

	status = nss_ppe_tx_msg(nss_ctx, npm);
	if (status != NSS_TX_SUCCESS) {
		nss_warning("%px: ppe_tx_msg failed\n", nss_ctx);
		up(&ppe_pvt.sem);
		return status;
	}

	ret = wait_for_completion_timeout(&ppe_pvt.complete, msecs_to_jiffies(NSS_PPE_TX_TIMEOUT));
	if (!ret) {
		nss_warning("%px: ppe msg tx failed due to timeout\n", nss_ctx);
		ppe_pvt.response = NSS_TX_FAILURE;
	}

	status = ppe_pvt.response;
	up(&ppe_pvt.sem);
	return status;
}

/*
 * nss_ppe_get_context()
 *	Get NSS context instance for ppe
 */
struct nss_ctx_instance *nss_ppe_get_context(void)
{
	return (struct nss_ctx_instance *)&nss_top_main.nss[nss_top_main.ppe_handler_id];
}

/*
 * nss_ppe_msg_init()
 *	Initialize nss_ppe_msg.
 */
void nss_ppe_msg_init(struct nss_ppe_msg *ncm, uint16_t if_num, uint32_t type, uint32_t len, void *cb, void *app_data)
{
	nss_cmn_msg_init(&ncm->cm, if_num, type, len, cb, app_data);
}

/*
 * nss_ppe_tx_ipsec_config_msg
 *	API to send inline IPsec port configure message to NSS FW
 */
nss_tx_status_t nss_ppe_tx_ipsec_config_msg(uint32_t nss_ifnum, uint32_t vsi_num, uint16_t mtu,
						__attribute__((unused))uint16_t mru)
{
	struct nss_ctx_instance *nss_ctx = nss_ppe_get_context();
	struct nss_ppe_msg npm = {0};

	if (!nss_ctx) {
		nss_warning("Can't get nss context\n");
		return NSS_TX_FAILURE;
	}

	if (vsi_num >= NSS_PPE_VSI_NUM_MAX) {
		nss_warning("Invalid vsi number:%u\n", vsi_num);
		return NSS_TX_FAILURE;
	}

	nss_ppe_msg_init(&npm, NSS_PPE_INTERFACE, NSS_PPE_MSG_IPSEC_PORT_CONFIG,
			sizeof(struct nss_ppe_ipsec_port_config_msg), NULL, NULL);

	npm.msg.ipsec_config.nss_ifnum = nss_ifnum;
	npm.msg.ipsec_config.vsi_num = vsi_num;
	npm.msg.ipsec_config.mtu = mtu;

	return nss_ppe_tx_msg_sync(nss_ctx, &npm);
}

/*
 * nss_ppe_tx_ipsec_mtu_msg
 *	API to send IPsec port MTU change message to NSS FW
 */
nss_tx_status_t nss_ppe_tx_ipsec_mtu_msg(uint32_t nss_ifnum, uint16_t mtu, __attribute__((unused))uint16_t mru)
{
	struct nss_ctx_instance *nss_ctx = nss_ppe_get_context();
	struct nss_ppe_msg npm = {0};

	if (!nss_ctx) {
		nss_warning("Can't get nss context\n");
		return NSS_TX_FAILURE;
	}

	nss_ppe_msg_init(&npm, NSS_PPE_INTERFACE, NSS_PPE_MSG_IPSEC_PORT_MTU_CHANGE,
			sizeof(struct nss_ppe_ipsec_port_mtu_msg), NULL, NULL);

	npm.msg.ipsec_mtu.nss_ifnum = nss_ifnum;
	npm.msg.ipsec_mtu.mtu = mtu;

	return nss_ppe_tx_msg_sync(nss_ctx, &npm);
}

/*
 * nss_ppe_tx_ipsec_add_intf_msg
 *	API to attach NSS interface to IPsec port
 */
nss_tx_status_t nss_ppe_tx_ipsec_add_intf_msg(uint32_t nss_ifnum)
{
	struct nss_ctx_instance *nss_ctx = nss_ppe_get_context();
	struct nss_ppe_msg npm = {0};

	if (!nss_ctx) {
		nss_warning("Can't get nss context\n");
		return NSS_TX_FAILURE;
	}

	nss_ppe_msg_init(&npm, NSS_PPE_INTERFACE, NSS_PPE_MSG_IPSEC_ADD_INTF,
			sizeof(struct nss_ppe_ipsec_add_intf_msg), NULL, NULL);

	npm.msg.ipsec_addif.nss_ifnum = nss_ifnum;

	return nss_ppe_tx_msg_sync(nss_ctx, &npm);
}

/*
 * nss_ppe_tx_ipsec_del_intf_msg
 *	API to detach NSS interface to IPsec port
 */
nss_tx_status_t nss_ppe_tx_ipsec_del_intf_msg(uint32_t nss_ifnum)
{
	struct nss_ctx_instance *nss_ctx = nss_ppe_get_context();
	struct nss_ppe_msg npm = {0};

	if (!nss_ctx) {
		nss_warning("Can't get nss context\n");
		return NSS_TX_FAILURE;
	}

	nss_ppe_msg_init(&npm, NSS_PPE_INTERFACE, NSS_PPE_MSG_IPSEC_DEL_INTF,
			sizeof(struct nss_ppe_ipsec_del_intf_msg), NULL, NULL);

	npm.msg.ipsec_delif.nss_ifnum = nss_ifnum;

	return nss_ppe_tx_msg_sync(nss_ctx, &npm);
}

/*
 * nss_ppe_handler()
 *	Handle NSS -> HLOS messages for ppe
 */
static void nss_ppe_handler(struct nss_ctx_instance *nss_ctx, struct nss_cmn_msg *ncm, __attribute__((unused))void *app_data)
{
	struct nss_ppe_msg *msg = (struct nss_ppe_msg *)ncm;
	void *ctx;

	nss_ppe_msg_callback_t cb;

	nss_trace("nss_ctx: %px ppe msg: %px\n", nss_ctx, msg);
	BUG_ON(!nss_ppe_verify_ifnum(ncm->interface));

	/*
	 * Trace messages.
	 */
	nss_ppe_log_rx_msg(msg);

	/*
	 * Is this a valid request/response packet?
	 */
	if (ncm->type >= NSS_PPE_MSG_MAX) {
		nss_warning("%px: received invalid message %d for PPE interface\n", nss_ctx, ncm->type);
		return;
	}

	if (nss_cmn_get_msg_len(ncm) > sizeof(struct nss_ppe_msg)) {
		nss_warning("%px: Length of message is greater than required: %d\n", nss_ctx, nss_cmn_get_msg_len(ncm));
		return;
	}

	switch (msg->cm.type) {
	case NSS_PPE_MSG_SYNC_STATS:
		/*
		 * session debug stats embeded in session stats msg
		 */
		nss_ppe_stats_sync(nss_ctx, &msg->msg.stats, ncm->interface);
		nss_ppe_stats_notify(nss_ctx, ncm->interface);
		return;
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
	cb = (nss_ppe_msg_callback_t)ncm->cb;
	ctx = (void *)ncm->app_data;

	cb(ctx, msg);
}

/*
 * nss_ppe_register_handler()
 *	debugfs stats msg handler received on static ppe interface
 *
 *	TODO: Export API so that others can also read PPE stats.
 */
void nss_ppe_register_handler(void)
{
	struct nss_ctx_instance *nss_ctx = nss_ppe_get_context();

	nss_core_register_handler(nss_ctx, NSS_PPE_INTERFACE, nss_ppe_handler, NULL);

	if (nss_ppe_debug_stats.valid) {
		nss_ppe_stats_dentry_create();
		nss_ppe_strings_dentry_create();
	}
}

/*
 * nss_ppe_free()
 *	Uninitialize PPE base
 */
void nss_ppe_free(void)
{
	/*
	 * Check if PPE base is already uninitialized.
	 */
	if (!ppe_pvt.ppe_base) {
		return;
	}

	/*
	 * Unmap PPE base address
	 */
	iounmap(ppe_pvt.ppe_base);
	ppe_pvt.ppe_base = NULL;

	spin_lock_bh(&nss_ppe_stats_lock);
	nss_ppe_debug_stats.valid = false;
	nss_ppe_debug_stats.if_num = 0;
	nss_ppe_debug_stats.if_index = 0;
	spin_unlock_bh(&nss_ppe_stats_lock);
}

/*
 * nss_ppe_init()
 *	Initialize PPE base
 */
void nss_ppe_init(void)
{
	/*
	 * Check if PPE base is already initialized.
	 */
	if (ppe_pvt.ppe_base) {
		return;
	}

	/*
	 * Get the PPE base address
	 */
	ppe_pvt.ppe_base = ioremap_nocache(PPE_BASE_ADDR, PPE_REG_SIZE);
	if (!ppe_pvt.ppe_base) {
		nss_warning("DRV can't get PPE base address\n");
		return;
	}

	spin_lock_bh(&nss_ppe_stats_lock);
	nss_ppe_debug_stats.valid = true;
	nss_ppe_debug_stats.if_num = 0;
	nss_ppe_debug_stats.if_index = 0;
	spin_unlock_bh(&nss_ppe_stats_lock);

	sema_init(&ppe_pvt.sem, 1);
	init_completion(&ppe_pvt.complete);
}

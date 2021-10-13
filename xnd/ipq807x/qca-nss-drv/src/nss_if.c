/*
 **************************************************************************
 * Copyright (c) 2014-2016, 2018-2021, The Linux Foundation. All rights reserved.
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
 * nss_if.c
 *	NSS base interfaces
 */

#include "nss_tx_rx_common.h"
#include "nss_if_log.h"

/*
 * nss_if_pvt
 *	NSS private structure to handle the completion of NSS -> HLOS messages.
 */
static struct nss_if_pvt {
	struct semaphore sem;
	struct completion complete;
	int response;
} nss_if;

static bool nss_if_sem_init_done;

/*
 * nss_if_callback
 *	Callback to handle the completion of NSS ->HLOS messages.
 */
static void nss_if_callback(void *app_data, struct nss_if_msg *nim)
{
	if (nim->cm.response != NSS_CMN_RESPONSE_ACK) {
		nss_warning("nss_if Error response %d\n", nim->cm.response);
		nss_if.response = NSS_TX_FAILURE;
		complete(&nss_if.complete);
		return;
	}

	nss_if.response = NSS_TX_SUCCESS;
	complete(&nss_if.complete);
}

/*
 * nss_if_msg_sync()
 *	Send a message to an interface and wait for the response.
 */
nss_tx_status_t nss_if_msg_sync(struct nss_ctx_instance *nss_ctx, struct nss_if_msg *nim)
{
	nss_tx_status_t status;
	int ret = 0;

	if (!nss_if_sem_init_done) {
		sema_init(&nss_if.sem, 1);
		init_completion(&nss_if.complete);
		nss_if_sem_init_done = 1;
	}

	down(&nss_if.sem);

	status = nss_if_tx_msg(nss_ctx, nim);
	if (status != NSS_TX_SUCCESS) {
		nss_warning("%px: nss_if_msg failed\n", nss_ctx);
		up(&nss_if.sem);
		return status;
	}

	ret = wait_for_completion_timeout(&nss_if.complete, msecs_to_jiffies(NSS_IF_TX_TIMEOUT));

	if (!ret) {
		nss_warning("%px: nss_if tx failed due to timeout\n", nss_ctx);
		nss_if.response = NSS_TX_FAILURE;
	}

	status = nss_if.response;
	up(&nss_if.sem);

	return status;
}
EXPORT_SYMBOL(nss_if_msg_sync);

/*
 * nss_if_msg_handler()
 *	Handle NSS -> HLOS messages for base class interfaces
 */
void nss_if_msg_handler(struct nss_ctx_instance *nss_ctx, struct nss_cmn_msg *ncm,
		__attribute__((unused))void *app_data)
{
	struct nss_if_msg *nim = (struct nss_if_msg *)ncm;
	nss_if_msg_callback_t cb;

	/*
	 * We only support base class messages with this interface
	 */
	if (ncm->type > NSS_IF_MAX_MSG_TYPES) {
		nss_warning("%px: message type out of range: %d", nss_ctx, ncm->type);
		return;
	}

	if (!nss_is_dynamic_interface(ncm->interface) &&
		!((ncm->interface >= NSS_PHYSICAL_IF_START) && (ncm->interface < NSS_VIRTUAL_IF_START))) {
		nss_warning("%px: interface %d not in physical or dynamic if range\n", nss_ctx, ncm->interface);
		return;
	}

	if (nss_cmn_get_msg_len(ncm) > sizeof(struct nss_if_msg)) {
		nss_warning("%px: message length too big: %d", nss_ctx, nss_cmn_get_msg_len(ncm));
		return;
	}

	/*
	 * Log failures
	 */
	nss_core_log_msg_failures(nss_ctx, ncm);

	/*
	 * Trace messages.
	 */
	nss_if_log_rx_msg(nim);

	/*
	 * Do we have a callback?
	 */
	if (!ncm->cb) {
		return;
	}

	/*
	 * Callback
	 */
	cb = (nss_if_msg_callback_t)ncm->cb;
	cb((void *)ncm->app_data, nim);
}

/*
 * nss_if_tx_buf()
 *	Send packet to interface owned by NSS
 */
nss_tx_status_t nss_if_tx_buf(struct nss_ctx_instance *nss_ctx, struct sk_buff *os_buf, uint32_t if_num)
{
	nss_trace("%px: If Tx packet, id:%d, data=%px", nss_ctx, if_num, os_buf->data);

	if (!nss_is_dynamic_interface(if_num) &&
		!((if_num >= NSS_PHYSICAL_IF_START) && (if_num < NSS_VIRTUAL_IF_START))) {
		nss_warning("%px: interface %d not in physical or dynamic if range\n", nss_ctx, if_num);
		return NSS_TX_FAILURE_BAD_PARAM;
	}

	return nss_core_send_packet(nss_ctx, os_buf, if_num, H2N_BIT_FLAG_BUFFER_REUSABLE);
}

/*
 * nss_if_tx_msg()
 *	Transmit a message to the specific interface on this core.
 */
nss_tx_status_t nss_if_tx_msg(struct nss_ctx_instance *nss_ctx, struct nss_if_msg *nim)
{
	struct nss_cmn_msg *ncm = &nim->cm;
	struct net_device *dev;

	NSS_VERIFY_CTX_MAGIC(nss_ctx);

	/*
	 * Sanity check the message
	 */
	if (ncm->type >= NSS_IF_MAX_MSG_TYPES) {
		nss_warning("%px: message type out of range: %d", nss_ctx, ncm->type);
		return NSS_TX_FAILURE;
	}

	/*
	 * Sanity check the message for valid interfaces.
	 */
	if (ncm->interface < NSS_PHYSICAL_IF_START ||
		ncm->interface >= NSS_MAX_NET_INTERFACES ) {
		nss_warning("%px: Tx request for invalid interface: %d", nss_ctx, ncm->interface);
		return NSS_TX_FAILURE_BAD_PARAM;
	}

	/*
	 * Trace messages.
	 */
	nss_if_log_tx_msg(nim);

	dev = nss_ctx->subsys_dp_register[ncm->interface].ndev;
	if (!dev) {
		nss_warning("%px: Unregister interface %d: no context", nss_ctx, ncm->interface);
		return NSS_TX_FAILURE_BAD_PARAM;
	}

	return nss_core_send_cmd(nss_ctx, nim, sizeof(*nim), NSS_NBUF_PAYLOAD_SIZE);
}

/*
 * nss_if_register()
 *	Primary registration for receiving data and msgs from an interface.
 */
struct nss_ctx_instance *nss_if_register(uint32_t if_num,
				nss_if_rx_callback_t rx_callback,
				nss_if_msg_callback_t msg_callback,
				struct net_device *if_ctx)
{
	return NULL;
}

/*
 * nss_if_unregister()
 *	Unregisteer the callback for this interface
 */
void nss_if_unregister(uint32_t if_num)
{
}

/*
 * nss_if_reset_nexthop()
 *	De-configures the nexthop for an interface
 */
nss_tx_status_t nss_if_reset_nexthop(struct nss_ctx_instance *nss_ctx, uint32_t if_num)
{
	struct nss_if_msg nim;

	NSS_VERIFY_CTX_MAGIC(nss_ctx);

	nss_trace("Resetting Nexthop. nss_ctx: %px ifnum: %u", nss_ctx, if_num);

	nss_cmn_msg_init(&nim.cm, if_num, NSS_IF_RESET_NEXTHOP, 0, nss_if_callback, NULL);

	return nss_if_msg_sync(nss_ctx, &nim);
}
EXPORT_SYMBOL(nss_if_reset_nexthop);

/*
 * nss_if_set_nexthop()
 *	Configures the nexthop for an interface
 */
nss_tx_status_t nss_if_set_nexthop(struct nss_ctx_instance *nss_ctx, uint32_t if_num, uint32_t nexthop)
{
	struct nss_if_msg nim;

	NSS_VERIFY_CTX_MAGIC(nss_ctx);

	if (nexthop >= NSS_MAX_NET_INTERFACES) {
		nss_warning("%px: Invalid nexthop interface number: %d", nss_ctx, nexthop);
		return NSS_TX_FAILURE_BAD_PARAM;
	}

	nss_trace("%px: NSS If nexthop will be set to %d, id:%d\n", nss_ctx, nexthop, if_num);

	nss_cmn_msg_init(&nim.cm, if_num, NSS_IF_SET_NEXTHOP,
				sizeof(struct nss_if_set_nexthop), nss_if_callback, NULL);

	nim.msg.set_nexthop.nexthop = nexthop;

	return nss_if_msg_sync(nss_ctx, &nim);
}
EXPORT_SYMBOL(nss_if_set_nexthop);

/*
 * nss_if_change_mtu()
 *	Change the MTU of the interface.
 */
nss_tx_status_t nss_if_change_mtu(struct nss_ctx_instance *nss_ctx, nss_if_num_t if_num, uint16_t mtu)
{
	struct nss_if_msg nim;

	NSS_VERIFY_CTX_MAGIC(nss_ctx);

	nss_trace("%px: NSS If MTU will be changed to %u, of NSS if num: %u\n", nss_ctx, mtu, if_num);

	nss_cmn_msg_init(&nim.cm, if_num, NSS_IF_MTU_CHANGE,
				sizeof(struct nss_if_mtu_change), nss_if_callback, NULL);

	nim.msg.mtu_change.min_buf_size = mtu;

	return nss_if_msg_sync(nss_ctx, &nim);
}
EXPORT_SYMBOL(nss_if_change_mtu);

/*
 * nss_if_change_mac_addr()
 *	Change the MAC address of the interface.
 */
nss_tx_status_t nss_if_change_mac_addr(struct nss_ctx_instance *nss_ctx, nss_if_num_t if_num, uint8_t *mac_addr)
{
	struct nss_if_msg nim;

	NSS_VERIFY_CTX_MAGIC(nss_ctx);

	nss_trace("%px: NSS If MAC address will be changed to %s, of NSS if num: %u\n", nss_ctx, mac_addr, if_num);

	nss_cmn_msg_init(&nim.cm, if_num, NSS_IF_MAC_ADDR_SET,
				sizeof(struct nss_if_mac_address_set), nss_if_callback, NULL);

	memcpy(nim.msg.mac_address_set.mac_addr, mac_addr, ETH_ALEN);

	return nss_if_msg_sync(nss_ctx, &nim);
}
EXPORT_SYMBOL(nss_if_change_mac_addr);

/*
 * nss_if_vsi_unassign()
 *	API to send VSI detach message to NSS FW.
 */
nss_tx_status_t nss_if_vsi_unassign(struct nss_ctx_instance *nss_ctx, nss_if_num_t if_num, uint32_t vsi)
{
	struct nss_if_msg nim;

	NSS_VERIFY_CTX_MAGIC(nss_ctx);

	nss_trace("%px: VSI to be unassigned is %u\n", nss_ctx, vsi);

	nss_cmn_msg_init(&nim.cm, if_num, NSS_IF_VSI_UNASSIGN,
				sizeof(struct nss_if_vsi_unassign), nss_if_callback, NULL);

	nim.msg.vsi_unassign.vsi = vsi;

	return nss_if_msg_sync(nss_ctx, &nim);
}
EXPORT_SYMBOL(nss_if_vsi_unassign);

/*
 * nss_if_vsi_assign()
 *	API to send VSI attach message to NSS FW.
 */
nss_tx_status_t nss_if_vsi_assign(struct nss_ctx_instance *nss_ctx, nss_if_num_t if_num, uint32_t vsi)
{
	struct nss_if_msg nim;

	NSS_VERIFY_CTX_MAGIC(nss_ctx);

	nss_trace("%px: VSI to be assigned is %u\n", nss_ctx, vsi);

	nss_cmn_msg_init(&nim.cm, if_num, NSS_IF_VSI_ASSIGN,
				sizeof(struct nss_if_vsi_assign), nss_if_callback, NULL);

	nim.msg.vsi_assign.vsi = vsi;

	return nss_if_msg_sync(nss_ctx, &nim);
}
EXPORT_SYMBOL(nss_if_vsi_assign);

EXPORT_SYMBOL(nss_if_tx_msg);
EXPORT_SYMBOL(nss_if_register);
EXPORT_SYMBOL(nss_if_unregister);

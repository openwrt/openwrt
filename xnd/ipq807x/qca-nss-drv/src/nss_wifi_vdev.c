/*
 **************************************************************************
 * Copyright (c) 2015-2020, The Linux Foundation. All rights reserved.
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

/*
 * nss_wifi_vdev_handler()
 *	Handle NSS -> HLOS messages for wifi_vdev
 */
static void nss_wifi_vdev_handler(struct nss_ctx_instance *nss_ctx, struct nss_cmn_msg *ncm, __attribute__((unused))void *app_data)
{
	nss_wifi_vdev_msg_callback_t cb;

	nss_info("%px: NSS->HLOS message for wifi vdev on interface:%d", nss_ctx, ncm->interface);

	BUG_ON(((ncm->interface < NSS_DYNAMIC_IF_START) || (ncm->interface >= (NSS_DYNAMIC_IF_START + NSS_MAX_DYNAMIC_INTERFACES))));

	/*
	 * Is this a valid request/response packet?
	 */
	if (ncm->type >= NSS_WIFI_VDEV_MAX_MSG) {
		nss_warning("%px: received invalid message %d for wifi vdev interface", nss_ctx, ncm->type);
		return;
	}

	if (nss_cmn_get_msg_len(ncm) > sizeof(struct nss_wifi_vdev_msg)) {
		nss_warning("%px: Length of message %d is greater than required: %d", nss_ctx, nss_cmn_get_msg_len(ncm), (int)sizeof(struct nss_wifi_vdev_msg));
		return;
	}

	/*
	 * Log failures
	 */
	nss_core_log_msg_failures(nss_ctx, ncm);

	/*
	 * callback
	 */
	if (!nss_ctx->subsys_dp_register[ncm->interface].ndev) {
		nss_warning("%px: Event received wifi vdev interface %d before registration", nss_ctx, ncm->interface);
		return;

	}

	if (ncm->response == NSS_CMN_RESPONSE_NOTIFY) {
		ncm->cb = (nss_ptr_t)nss_core_get_msg_handler(nss_ctx, ncm->interface);
		ncm->app_data = (nss_ptr_t)nss_ctx->subsys_dp_register[ncm->interface].ndev;
	}

	/*
	 * Do we have a callback?
	 */
	if (!ncm->cb) {
		return;
	}

	cb = (nss_wifi_vdev_msg_callback_t)ncm->cb;
	cb((void *)ncm->app_data, ncm);
}

/*
 * nss_wifi_vdev_msg_init()
 *	Initialize wifi message.
 */
void nss_wifi_vdev_msg_init(struct nss_wifi_vdev_msg *nim, uint32_t if_num, uint32_t type, uint32_t len,
			nss_wifi_vdev_msg_callback_t *cb, void *app_data)
{
	nss_cmn_msg_init(&nim->cm, if_num, type, len, (void *)cb, app_data);
}
EXPORT_SYMBOL(nss_wifi_vdev_msg_init);

/*
 * nss_wifi_vdev_base_tx_msg()
 *	Transmit a wifi vdev base message to NSSFW
 */
nss_tx_status_t nss_wifi_vdev_base_tx_msg(struct nss_ctx_instance *nss_ctx, struct nss_wifi_vdev_msg *msg)
{
	struct nss_cmn_msg *ncm = &msg->cm;

	nss_trace("%px: Sending wifi vdev message on interface :%d", nss_ctx, ncm->interface);

	/*
	 * Sanity checks on the message
	 */

	/*
	 * The interface number shall be wifi vdev base vap
	 */
	 if (ncm->interface != NSS_VAP_INTERFACE) {
		nss_warning("%px: wifi vdev base tx request not on wifi vdev vap: %d", nss_ctx, ncm->interface);
		return NSS_TX_FAILURE;
	}

	if (ncm->type >= NSS_WIFI_VDEV_MAX_MSG) {
		nss_warning("%px: wifi vdev base message type out of range: %d", nss_ctx, ncm->type);
		return NSS_TX_FAILURE;
	}

	return nss_core_send_cmd(nss_ctx, msg, sizeof(*msg), NSS_NBUF_PAYLOAD_SIZE);
}
EXPORT_SYMBOL(nss_wifi_vdev_base_tx_msg);

/*
 * nss_wifi_vdev_tx_msg()
 *	Transmit a wifi vdev message to NSSFW
 */
nss_tx_status_t nss_wifi_vdev_tx_msg(struct nss_ctx_instance *nss_ctx, struct nss_wifi_vdev_msg *msg)
{
	struct nss_cmn_msg *ncm = &msg->cm;

	nss_trace("%px: Sending wifi vdev message on interface :%d", nss_ctx, ncm->interface);

	/*
	 * Sanity checks on the message
	 */

	/*
	 * Interface shall be of dynamic interface type
	 */
	 if ((ncm->interface < NSS_DYNAMIC_IF_START) || (ncm->interface >= (NSS_DYNAMIC_IF_START + NSS_MAX_DYNAMIC_INTERFACES))) {
		nss_warning("%px: wifi vdev tx request for invalid interface: %d", nss_ctx, ncm->interface);
		return NSS_TX_FAILURE;
	}

	if (ncm->type >= NSS_WIFI_VDEV_MAX_MSG) {
		nss_warning("%px: wifi vdev message type out of range: %d", nss_ctx, ncm->type);
		return NSS_TX_FAILURE;
	}

	return nss_core_send_cmd(nss_ctx, msg, sizeof(*msg), NSS_NBUF_PAYLOAD_SIZE);
}
EXPORT_SYMBOL(nss_wifi_vdev_tx_msg);

/*
 * nss_wifi_vdev_tx_msg_ext()
 * 	Send special data packet with metadata for vap processing
 */
nss_tx_status_t nss_wifi_vdev_tx_msg_ext(struct nss_ctx_instance *nss_ctx, struct sk_buff *os_buf)
{
	struct nss_wifi_vdev_msg *nm;
	struct nss_cmn_msg *ncm;
	nss_tx_status_t status;

	NSS_VERIFY_CTX_MAGIC(nss_ctx);

	if (unlikely(nss_ctx->state != NSS_CORE_STATE_INITIALIZED)) {
		nss_warning("%px: wifi vdev message dropped as core not ready", nss_ctx);
		return NSS_TX_FAILURE_NOT_READY;
	}

	nm = (struct nss_wifi_vdev_msg *) os_buf->data;
	ncm = &nm->cm;

	nss_trace("%px: Sending wifi vdev message on interface :%d", nss_ctx, ncm->interface);

	/*
	 * Interface shall be of dynamic interface type
	 */
	if ((ncm->interface < NSS_DYNAMIC_IF_START) || (ncm->interface >= (NSS_DYNAMIC_IF_START + NSS_MAX_DYNAMIC_INTERFACES))) {
		nss_warning("%px: wifi vdev tx request for invalid interface: %d", nss_ctx, ncm->interface);
		return NSS_TX_FAILURE;
	}

	if (ncm->type >= NSS_WIFI_VDEV_MAX_MSG) {
		nss_warning("%px: wifi vdev message type out of range: %d", nss_ctx, ncm->type);
		return NSS_TX_FAILURE;
	}

	status = nss_core_send_buffer(nss_ctx, 0, os_buf, NSS_IF_H2N_CMD_QUEUE, H2N_BUFFER_CTRL, H2N_BIT_FLAG_BUFFER_REUSABLE);
	if (status != NSS_CORE_STATUS_SUCCESS) {
		nss_warning("%px: Unable to enqueue 'wifi vdev message'", nss_ctx);
		return NSS_TX_FAILURE;
	}

	nss_hal_send_interrupt(nss_ctx, NSS_H2N_INTR_DATA_COMMAND_QUEUE);

	NSS_PKT_STATS_INC(&nss_ctx->nss_top->stats_drv[NSS_DRV_STATS_TX_CMD_REQ]);

return status;
}
EXPORT_SYMBOL(nss_wifi_vdev_tx_msg_ext);

/*
 * nss_wifi_vdev_tx_buf
 * 	Send data packet for vap processing
 */
nss_tx_status_t nss_wifi_vdev_tx_buf(struct nss_ctx_instance *nss_ctx, struct sk_buff *os_buf, uint32_t if_num)
{
	BUG_ON(((if_num < NSS_DYNAMIC_IF_START) || (if_num >= (NSS_DYNAMIC_IF_START + NSS_MAX_DYNAMIC_INTERFACES))));

	return nss_core_send_packet(nss_ctx, os_buf, if_num, H2N_BIT_FLAG_BUFFER_REUSABLE);
}
EXPORT_SYMBOL(nss_wifi_vdev_tx_buf);

/*
 * nss_wifi_vdev_set_next_hop()
 */
nss_tx_status_t nss_wifi_vdev_set_next_hop(struct nss_ctx_instance *ctx, int if_num, int next_hop)
{
	nss_tx_status_t status;
	struct nss_wifi_vdev_msg *wifivdevmsg = kzalloc(sizeof(struct nss_wifi_vdev_msg), GFP_KERNEL);
	struct nss_wifi_vdev_set_next_hop_msg *next_hop_msg = NULL;

	if (!wifivdevmsg) {
		nss_warning("%px: Unable to allocate next hop message", ctx);
		return NSS_TX_FAILURE;
	}

	next_hop_msg = &wifivdevmsg->msg.next_hop;

	next_hop_msg->ifnumber = next_hop;
	nss_wifi_vdev_msg_init(wifivdevmsg, if_num, NSS_WIFI_VDEV_SET_NEXT_HOP, sizeof(struct nss_wifi_vdev_set_next_hop_msg), NULL, NULL);

	status = nss_wifi_vdev_tx_msg(ctx, wifivdevmsg);
	if (status != NSS_TX_SUCCESS) {
		nss_warning("%px: Unable to send next hop message", ctx);
	}

	kfree(wifivdevmsg);
	return status;
}
EXPORT_SYMBOL(nss_wifi_vdev_set_next_hop);

/*
 * nss_wifi_vdev_base_set_next_hop()
 */
nss_tx_status_t nss_wifi_vdev_base_set_next_hop(struct nss_ctx_instance *ctx, int next_hop)
{
	nss_tx_status_t status;
	struct nss_wifi_vdev_msg *wifivdevmsg = kzalloc(sizeof(struct nss_wifi_vdev_msg), GFP_KERNEL);
	struct nss_wifi_vdev_set_next_hop_msg *next_hop_msg = NULL;

	if (!wifivdevmsg) {
		nss_warning("%px: Unable to allocate next hop message", ctx);
		return NSS_TX_FAILURE;
	}

	next_hop_msg = &wifivdevmsg->msg.next_hop;

	next_hop_msg->ifnumber = next_hop;
	nss_wifi_vdev_msg_init(wifivdevmsg, NSS_VAP_INTERFACE, NSS_WIFI_VDEV_SET_NEXT_HOP, sizeof(struct nss_wifi_vdev_set_next_hop_msg), NULL, NULL);

	status = nss_wifi_vdev_base_tx_msg(ctx, wifivdevmsg);
	if (status != NSS_TX_SUCCESS) {
		nss_warning("%px: Unable to send next hop message", ctx);
	}

	kfree(wifivdevmsg);
	return status;
}
EXPORT_SYMBOL(nss_wifi_vdev_base_set_next_hop);

/*
 * nss_wifi_vdev_set_peer_next_hop()
 */
nss_tx_status_t nss_wifi_vdev_set_peer_next_hop(struct nss_ctx_instance *ctx, uint32_t nss_if, uint8_t *addr, uint32_t next_hop_if)
{
	nss_tx_status_t status;
	struct nss_wifi_vdev_msg *wifivdevmsg = kzalloc(sizeof(struct nss_wifi_vdev_msg), GFP_KERNEL);
	struct nss_wifi_vdev_set_peer_next_hop_msg *peer_next_hop_msg = NULL;

	if (!wifivdevmsg) {
		nss_warning("%px: Unable to allocate next hop message", ctx);
		return NSS_TX_FAILURE;
	}

	peer_next_hop_msg = &wifivdevmsg->msg.vdev_set_peer_next_hp;
	memcpy(peer_next_hop_msg->peer_mac_addr, addr, ETH_ALEN);

	peer_next_hop_msg->if_num = next_hop_if;
	nss_wifi_vdev_msg_init(wifivdevmsg, nss_if, NSS_WIFI_VDEV_SET_PEER_NEXT_HOP,
			sizeof(struct nss_wifi_vdev_set_peer_next_hop_msg), NULL, NULL);

	status = nss_wifi_vdev_tx_msg(ctx, wifivdevmsg);
	if (status != NSS_TX_SUCCESS) {
		nss_warning("%px: Unable to send peer next hop message", ctx);
	}

	kfree(wifivdevmsg);
	return status;
}
EXPORT_SYMBOL(nss_wifi_vdev_set_peer_next_hop);

/*
 * nss_wifi_vdev_set_dp_type()
 *	Set the vap datapath type of the packet.
 */
bool nss_wifi_vdev_set_dp_type(struct nss_ctx_instance *nss_ctx, struct net_device *netdev,
						uint32_t if_num, enum nss_wifi_vdev_dp_type dp_type)
{

	NSS_VERIFY_CTX_MAGIC(nss_ctx);

	nss_assert((if_num >= NSS_DYNAMIC_IF_START) && (if_num < (NSS_DYNAMIC_IF_START + NSS_MAX_DYNAMIC_INTERFACES)));

	if (unlikely(nss_ctx->state != NSS_CORE_STATE_INITIALIZED)) {
		nss_warning("%px: Vap interface dp type could not be set as core is not initialized\n", nss_ctx);
		return false;
	}

	/*
	 * set the subsytem dp type for the Wi-Fi vdev
	 */
	nss_core_set_subsys_dp_type(nss_ctx, netdev, if_num, dp_type);

	return true;
}
EXPORT_SYMBOL(nss_wifi_vdev_set_dp_type);

/*
 ***********************************
 * Register/Unregister/Miscellaneous APIs
 ***********************************
 */

/*
 * nss_register_wifi_vdev_if()
 */
uint32_t nss_register_wifi_vdev_if(struct nss_ctx_instance *nss_ctx,
				int32_t if_num,
				nss_wifi_vdev_callback_t vdev_data_callback,
				nss_wifi_vdev_ext_data_callback_t vdev_ext_data_callback,
				nss_wifi_vdev_msg_callback_t vdev_event_callback,
				struct net_device *netdev,
				uint32_t features)
{
	uint32_t status;

	nss_assert((if_num >= NSS_DYNAMIC_IF_START) && (if_num < (NSS_DYNAMIC_IF_START + NSS_MAX_DYNAMIC_INTERFACES)));

	nss_core_register_subsys_dp(nss_ctx, if_num, vdev_data_callback, vdev_ext_data_callback, NULL, netdev, features);

	status = nss_core_register_msg_handler(nss_ctx, if_num, vdev_event_callback);
	if (status != NSS_CORE_STATUS_SUCCESS) {
		nss_warning("%px: unable to register event handler for interface(%u)", nss_ctx, if_num);
		return status;
	}

	nss_core_register_handler(nss_ctx, if_num, nss_wifi_vdev_handler, NULL);

	return NSS_CORE_STATUS_SUCCESS;
}
EXPORT_SYMBOL(nss_register_wifi_vdev_if);

/*
 * nss_unregister_wifi_vdev_if()
 */
void nss_unregister_wifi_vdev_if(uint32_t if_num)
{
	struct nss_ctx_instance *nss_ctx = (struct nss_ctx_instance *)&nss_top_main.nss[nss_top_main.wifi_handler_id];
	uint32_t status;

	nss_assert(nss_ctx);
	nss_assert((if_num >= NSS_DYNAMIC_IF_START) && (if_num < (NSS_DYNAMIC_IF_START + NSS_MAX_DYNAMIC_INTERFACES)));

	nss_core_unregister_subsys_dp(nss_ctx, if_num);

	status = nss_core_unregister_msg_handler(nss_ctx, if_num);
	if (status != NSS_CORE_STATUS_SUCCESS) {
		nss_warning("%px: unable to unregister event handler for interface(%u)", nss_ctx, if_num);
		return;
	}

	nss_core_unregister_handler(nss_ctx, if_num);
}
EXPORT_SYMBOL(nss_unregister_wifi_vdev_if);

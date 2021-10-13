/*
 **************************************************************************
 * Copyright (c) 2015-2018, 2020, The Linux Foundation. All rights reserved.
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
#include "nss_portid_stats.h"
#include "nss_portid_log.h"

/*
 * Spinlock to protect portid interface create/destroy/update
 */
DEFINE_SPINLOCK(nss_portid_spinlock);

#define NSS_PORTID_TX_TIMEOUT 3000 /* 3 Seconds */

/*
 * Private data structure for phys_if interface
 */
static struct nss_portid_pvt {
	struct semaphore sem;
	struct completion complete;
	int response;
} pid;

/*
 * Array of portid interface handles. Indexing based on the physical port_id
 */
struct nss_portid_handle nss_portid_hdl[NSS_PORTID_MAX_SWITCH_PORT];

/*
 * nss_portid_handler()
 * 	Handle NSS -> HLOS messages for portid
 */
static void nss_portid_handler(struct nss_ctx_instance *nss_ctx, struct nss_cmn_msg *ncm,
				__attribute__((unused))void *app_data)
{
	nss_portid_msg_callback_t cb;
	struct nss_portid_msg *npm = (struct nss_portid_msg *)ncm;

	BUG_ON(ncm->interface != NSS_PORTID_INTERFACE);

	/*
	 * Trace Messages
	 */
	nss_portid_log_rx_msg(npm);

	/*
	 * Is this a valid request/response packet?
	 */
	if (ncm->type >= NSS_PORTID_MAX_MSG_TYPE) {
		nss_warning("%px: received invalid message %d for portid interface", nss_ctx, ncm->type);
		return;
	}

	if (nss_cmn_get_msg_len(ncm) > sizeof(struct nss_portid_msg)) {
		nss_warning("%px: message size incorrect: %d", nss_ctx, nss_cmn_get_msg_len(ncm));
		return;
	}

	/*
	 * Log failures
	 */
	nss_core_log_msg_failures(nss_ctx, ncm);

	switch (ncm->type) {
	case NSS_PORTID_STATS_SYNC_MSG:
		/*
		 * Update portid statistics.
		 */
		nss_portid_stats_sync(nss_ctx, &npm->msg.stats_sync);
		break;
	}

	/*
	 * Update the callback and app_data for NOTIFY messages, portid sends all notify messages
	 * to the same callback/app_data.
	 */
	if (ncm->response == NSS_CMN_RESPONSE_NOTIFY) {
		ncm->cb = (nss_ptr_t)nss_core_get_msg_handler(nss_ctx, ncm->interface);
		ncm->app_data = (nss_ptr_t)nss_ctx->subsys_dp_register[ncm->interface].ndev;
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
	cb = (nss_portid_msg_callback_t)ncm->cb;

	cb((void *)ncm->app_data, npm);
}

/*
 * nss_portid_get_ctx()
 *	Return a portid's NSS context.
 */
struct nss_ctx_instance *nss_portid_get_ctx(void)
{
	struct nss_ctx_instance *nss_ctx;

	nss_ctx = &nss_top_main.nss[nss_top_main.portid_handler_id];
	return nss_ctx;
}

/*
 * nss_portid_verify_if_num()
 *	Verify if_num passed to us.
 */
static bool nss_portid_verify_if_num(uint32_t if_num)
{
	if (nss_is_dynamic_interface(if_num) == false) {
		return false;
	}

	if (nss_dynamic_interface_get_type(nss_portid_get_ctx(), if_num) != NSS_DYNAMIC_INTERFACE_TYPE_PORTID) {
		return false;
	}

	return true;
}

/*
 * nss_portid_get_stats()
 *	API for getting stats from a port interface
 */
bool nss_portid_get_stats(uint32_t if_num, struct rtnl_link_stats64 *stats)
{
	int i;

	spin_lock_bh(&nss_portid_spinlock);
	for (i = 0; i < NSS_PORTID_MAX_SWITCH_PORT; i++) {
		if (nss_portid_hdl[i].if_num == if_num) {
			memcpy(stats, &nss_portid_hdl[i].stats, sizeof(*stats));
			spin_unlock_bh(&nss_portid_spinlock);
			return true;
		}
	}
	spin_unlock_bh(&nss_portid_spinlock);
	return false;
}
EXPORT_SYMBOL(nss_portid_get_stats);

/*
 * nss_portid_if_tx_data()
 *	Transmit data buffer (skb) to a NSS interface number
 */
nss_tx_status_t nss_portid_if_tx_data(struct nss_ctx_instance *nss_ctx, struct sk_buff *os_buf, uint32_t if_num)
{
	return nss_if_tx_buf(nss_ctx, os_buf, if_num);
}
EXPORT_SYMBOL(nss_portid_if_tx_data);

/*
 * nss_portid_tx_msg()
 * 	Transmit a portid message to NSSFW
 */
nss_tx_status_t nss_portid_tx_msg(struct nss_ctx_instance *nss_ctx, struct nss_portid_msg *msg)
{
	struct nss_cmn_msg *ncm = &msg->cm;

	/*
	 * Trace Messages
	 */
	nss_portid_log_tx_msg(msg);

	/*
	 * Sanity check the message
	 */
	if (ncm->interface != NSS_PORTID_INTERFACE) {
		nss_warning("%px: tx request for another interface: %d", nss_ctx, ncm->interface);
		return NSS_TX_FAILURE;
	}

	if (ncm->type > NSS_PORTID_MAX_MSG_TYPE) {
		nss_warning("%px: message type out of range: %d", nss_ctx, ncm->type);
		return NSS_TX_FAILURE;
	}

	return nss_core_send_cmd(nss_ctx, msg, sizeof(*msg), NSS_NBUF_PAYLOAD_SIZE);
}
EXPORT_SYMBOL(nss_portid_tx_msg);

/*
 * nss_portid_callback
 *	Callback to handle the completion of NSS ->HLOS messages.
 */
static void nss_portid_callback(void *app_data, struct nss_portid_msg *npm)
{
	if(npm->cm.response != NSS_CMN_RESPONSE_ACK) {
		nss_warning("portid error response %d\n", npm->cm.response);
		pid.response = NSS_TX_FAILURE;
		complete(&pid.complete);
		return;
	}

	pid.response = NSS_TX_SUCCESS;
	complete(&pid.complete);
}

/*
 * nss_portid_tx_msg_sync()
 *	Send a message to portid interface & wait for the response.
 */
nss_tx_status_t nss_portid_tx_msg_sync(struct nss_ctx_instance *nss_ctx, struct nss_portid_msg *msg)
{
	nss_tx_status_t status;
	int ret = 0;

	down(&pid.sem);

	status = nss_portid_tx_msg(nss_ctx, msg);
	if(status != NSS_TX_SUCCESS)
	{
		nss_warning("%px: nss_phys_if_msg failed\n", nss_ctx);
		up(&pid.sem);
		return status;
	}

	ret = wait_for_completion_timeout(&pid.complete, msecs_to_jiffies(NSS_PORTID_TX_TIMEOUT));

	if(!ret)
	{
		nss_warning("%px: portid tx failed due to timeout\n", nss_ctx);
		pid.response = NSS_TX_FAILURE;
	}

	status = pid.response;
	up(&pid.sem);

	return status;
}
EXPORT_SYMBOL(nss_portid_tx_msg_sync);

/*
 * nss_portid_msg_init()
 *	Initialize portid message.
 */
void nss_portid_msg_init(struct nss_portid_msg *npm, uint16_t if_num, uint32_t type, uint32_t len,
				nss_portid_msg_callback_t cb, void *app_data)
{
	nss_cmn_msg_init(&npm->cm, if_num, type, len, (void*)cb, app_data);
}
EXPORT_SYMBOL(nss_portid_msg_init);

/*
 * nss_portid_tx_configure_port_if_msg
 *	API to send configure port message to NSS FW
 */
nss_tx_status_t nss_portid_tx_configure_port_if_msg(struct nss_ctx_instance *nss_ctx, uint32_t port_if_num, uint8_t port_id, uint8_t gmac_id)
{
	struct nss_portid_msg npm;
	struct nss_portid_configure_msg *npcm;

	if (nss_portid_verify_if_num(port_if_num) == false) {
		nss_warning("received invalid interface %d", port_if_num);
		return NSS_TX_FAILURE;
	}

	if (port_id >= NSS_PORTID_MAX_SWITCH_PORT) {
		nss_warning("port_id %d exceeds NSS_PORTID_MAX_SWITCH_PORT\n", port_id);
		return NSS_TX_FAILURE;
	}

	if (gmac_id >= NSS_MAX_PHYSICAL_INTERFACES) {
		nss_warning("gmac_id %d not valid\n", gmac_id);
		return NSS_TX_FAILURE;
	}

	/*
	 * Prepare message to configure a port interface
	 */
	npcm = &npm.msg.configure;
	npcm->port_if_num = port_if_num;
	npcm->port_id = port_id;
	npcm->gmac_id = gmac_id;

	nss_portid_msg_init(&npm, NSS_PORTID_INTERFACE, NSS_PORTID_CONFIGURE_MSG,
				sizeof(struct nss_portid_configure_msg), nss_portid_callback, NULL);
	nss_info("Dynamic interface allocated, sending message to FW with port_if_num %d port_id %d gmac_id %d\n",
									npcm->port_if_num, npcm->port_id, npcm->gmac_id);
	return nss_portid_tx_msg_sync(nss_ctx, &npm);
}
EXPORT_SYMBOL(nss_portid_tx_configure_port_if_msg);

/*
 * nss_portid_tx_unconfigure_port_if_msg
 *	API to send unconfigure port message to NSS FW
 */
nss_tx_status_t nss_portid_tx_unconfigure_port_if_msg(struct nss_ctx_instance *nss_ctx, uint32_t port_if_num, uint8_t port_id)
{
	struct nss_portid_msg npm;
	struct nss_portid_unconfigure_msg *npum;

	if (nss_portid_verify_if_num(port_if_num) == false) {
		nss_warning("received invalid interface %d", port_if_num);
		return NSS_TX_FAILURE;
	}

	if (port_id >= NSS_PORTID_MAX_SWITCH_PORT) {
		nss_warning("port_id %d exceeds NSS_PORTID_MAX_SWITCH_PORT\n", port_id);
		return NSS_TX_FAILURE;
	}

	/*
	 * Prepare message to unconfigure a port interface
	 */
	npum = &npm.msg.unconfigure;
	npum->port_if_num = port_if_num;
	npum->port_id = port_id;

	nss_portid_msg_init(&npm, NSS_PORTID_INTERFACE, NSS_PORTID_UNCONFIGURE_MSG,
				sizeof(struct nss_portid_configure_msg), nss_portid_callback, NULL);

	return nss_portid_tx_msg_sync(nss_ctx, &npm);
}
EXPORT_SYMBOL(nss_portid_tx_unconfigure_port_if_msg);

/*
 * nss_portid_register_port_if()
 *	Register with portid node and get back nss_ctx
 */
struct nss_ctx_instance *nss_portid_register_port_if(uint32_t if_num, uint32_t port_id, struct net_device *netdev,
						nss_portid_buf_callback_t buf_callback)
{
	struct nss_ctx_instance *nss_ctx = nss_portid_get_ctx();

	if (nss_portid_verify_if_num(if_num) == false) {
		nss_warning("nss portid register received invalid interface %d", if_num);
		return NULL;
	}

	if (port_id >= NSS_PORTID_MAX_SWITCH_PORT) {
		nss_warning("nss portid register received invalid port number %d", port_id);
		return NULL;
	}

	spin_lock(&nss_portid_spinlock);
	if (nss_portid_hdl[port_id].if_num != 0) {
		nss_warning("nss portid failed: port already registered %d", port_id);
		spin_unlock(&nss_portid_spinlock);
		return NULL;
	}
	nss_portid_hdl[port_id].if_num = if_num;
	spin_unlock(&nss_portid_spinlock);

	nss_core_register_subsys_dp(nss_ctx, if_num, buf_callback, NULL, NULL, netdev, 0);

	return nss_ctx;
}
EXPORT_SYMBOL(nss_portid_register_port_if);

/*
 * nss_portid_unregister_port_if()
 *	Unregister portid node with NSS FW
 */
bool nss_portid_unregister_port_if(uint32_t if_num)
{
	struct nss_ctx_instance *nss_ctx;
	int i;

	nss_ctx = nss_portid_get_ctx();
	if (nss_portid_verify_if_num(if_num) == false) {
		nss_warning("%px: unregister received for invalid interface %d", nss_ctx, if_num);
		return false;
	}

	spin_lock(&nss_portid_spinlock);
	for (i = 0; i < NSS_PORTID_MAX_SWITCH_PORT; i++) {
		if (nss_portid_hdl[i].if_num == if_num) {
			nss_portid_hdl[i].if_num = 0;
		}
	}
	spin_unlock(&nss_portid_spinlock);

	nss_core_unregister_handler(nss_ctx, if_num);

	nss_core_unregister_subsys_dp(nss_ctx, if_num);

	return true;
}
EXPORT_SYMBOL(nss_portid_unregister_port_if);

/*
 * nss_portid_init()
 *	Initializes portid node. Gets called from nss_init.c
 */
void nss_portid_init(void)
{
	memset(&nss_portid_hdl, 0, sizeof(struct nss_portid_handle) * NSS_PORTID_MAX_SWITCH_PORT);
}

/*
 * nss_portid_register_handler()
 *	Registering handler for sending msg to portid node on NSS.
 */
void nss_portid_register_handler(void)
{
	struct nss_ctx_instance *nss_ctx = nss_portid_get_ctx();

	nss_core_register_handler(nss_ctx, NSS_PORTID_INTERFACE, nss_portid_handler, NULL);

	nss_portid_stats_dentry_create();

	sema_init(&pid.sem, 1);
	init_completion(&pid.complete);
}

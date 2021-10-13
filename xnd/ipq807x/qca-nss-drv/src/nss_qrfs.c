/*
 **************************************************************************
 * Copyright (c) 2017-2020, The Linux Foundation. All rights reserved.
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
#include "nss_qrfs_stats.h"
#include "nss_qrfs_log.h"

/*
 * Notify data structure
 */
struct nss_qrfs_notify_data {
	nss_qrfs_msg_callback_t qrfs_callback;
	void *app_data;
};

static struct nss_qrfs_notify_data nss_qrfs_notify[NSS_CORE_MAX];

/*
 * nss_qrfs_verify_if_num()
 *	Verify if_num passed to us.
 */
static bool nss_qrfs_verify_if_num(uint32_t if_num)
{
	return if_num == NSS_QRFS_INTERFACE;
}

/*
 * nss_qrfs_msg_handler()
 *	Handle NSS -> HLOS messages for QRFS
 */
static void nss_qrfs_msg_handler(struct nss_ctx_instance *nss_ctx, struct nss_cmn_msg *ncm, void *app_data)
{
	struct nss_qrfs_msg *nqm = (struct nss_qrfs_msg *)ncm;
	nss_qrfs_msg_callback_t cb;

	/*
	 * Trace messages.
	 */
	nss_qrfs_log_rx_msg(nqm);

	if (!nss_qrfs_verify_if_num(ncm->interface)) {
		nss_warning("%px: invalid interface %d for QRFS\n", nss_ctx, ncm->interface);
		return;
	}

	/*
	 * Is this a valid request/response?
	 */
	if (ncm->type >= NSS_QRFS_MSG_MAX) {
		nss_warning("%px: invalid message %d for QRFS\n", nss_ctx, ncm->type);
		return;
	}

	if (nss_cmn_get_msg_len(ncm) > sizeof(struct nss_qrfs_msg)) {
		nss_warning("%px: message length is greater than required: %d\n", nss_ctx, nss_cmn_get_msg_len(ncm));
		return;
	}

	/*
	 * Log failures
	 */
	nss_core_log_msg_failures(nss_ctx, ncm);

	switch (ncm->type) {
	case NSS_QRFS_MSG_STATS_SYNC:
		/*
		 * Update QRFS statistics.
		 */
		nss_qrfs_stats_sync(nss_ctx, &nqm->msg.stats_sync);
		break;
	}

	/*
	 * Update the callback and app_data for NOTIFY messages
	 */
	if (ncm->response == NSS_CMN_RESPONSE_NOTIFY) {
		ncm->cb = (nss_ptr_t)nss_qrfs_notify[nss_ctx->id].qrfs_callback;
		ncm->app_data = (nss_ptr_t)nss_qrfs_notify[nss_ctx->id].app_data;
	}

	/*
	 * Do we have a callback?
	 */
	if (!ncm->cb) {
		return;
	}

	/*
	 * callback
	 */
	cb = (nss_qrfs_msg_callback_t)ncm->cb;
	cb((void *)ncm->app_data, nqm);
}

/*
 * nss_qrfs_get_ctx()
 */
static struct nss_ctx_instance *nss_qrfs_get_ctx(int core_id)
{
	return &nss_top_main.nss[core_id];
}

/*
 * nss_qrfs_get_flow_keys()
 *	Get 5 tuple information from flow keys and set in flow rule message.
 */
#if (LINUX_VERSION_CODE <= KERNEL_VERSION(3, 18, 21))
static bool nss_qrfs_get_flow_keys(struct nss_ctx_instance *nss_ctx, struct sk_buff *skb,
					struct nss_qrfs_flow_rule_msg *nqfrm)
{
	struct flow_keys keys;
	uint16_t protocol = skb->protocol;
	bool res;
	struct ipv6hdr *ip6hdr;

	res = skb_flow_dissect(skb, &keys);
	if (!res) {
		nss_warning("%px: failed to get flow keys\n", nss_ctx);
		return res;
	}

	nqfrm->protocol = keys.ip_proto;
	nqfrm->src_port = keys.port16[0];
	nqfrm->dst_port = keys.port16[1];

	if (protocol == htons(ETH_P_IP)) {
		nqfrm->ip_version = 4;
		nqfrm->src_addr[0] = keys.src;
		nqfrm->dst_addr[0] = keys.dst;
		return true;
	}

	nqfrm->ip_version = 6;
	ip6hdr = (struct ipv6hdr *)skb_network_header(skb);
	if (!ip6hdr) {
		nss_warning("%px: failed to get IPv6 address\n", nss_ctx);
		return false;
	}

	memcpy(nqfrm->src_addr, &ip6hdr->saddr, sizeof(struct in6_addr));
	memcpy(nqfrm->dst_addr, &ip6hdr->daddr, sizeof(struct in6_addr));

	return true;
}
#else
static bool nss_qrfs_get_flow_keys(struct nss_ctx_instance *nss_ctx, struct sk_buff *skb,
					struct nss_qrfs_flow_rule_msg *nqfrm)
{
	struct flow_keys keys;
	bool res;

	res = skb_flow_dissect_flow_keys(skb, &keys, 0);
	if (!res) {
		nss_warning("%px: failed to get flow keys\n", nss_ctx);
		return res;
	}

	nqfrm->protocol = (uint16_t)keys.basic.ip_proto;
	nqfrm->src_port = keys.ports.src;
	nqfrm->dst_port = keys.ports.dst;

	if (keys.basic.n_proto == htons(ETH_P_IP)) {
		nqfrm->ip_version = 4;
		nqfrm->src_addr[0] = keys.addrs.v4addrs.src;
		nqfrm->dst_addr[0] = keys.addrs.v4addrs.dst;
		return true;
	}

	nqfrm->ip_version = 6;
	memcpy(nqfrm->src_addr, &keys.addrs.v6addrs.src, sizeof(struct in6_addr));
	memcpy(nqfrm->dst_addr, &keys.addrs.v6addrs.dst, sizeof(struct in6_addr));

	return true;
}
#endif

/*
 * nss_qrfs_flow_add_msg_callback()
 *	Callback function for receiving flow add response messages.
 */
static void nss_qrfs_flow_add_msg_callback(void *app_data, struct nss_qrfs_msg *nqm)
{
	struct nss_ctx_instance *nss_ctx __maybe_unused = (struct nss_ctx_instance *)app_data;
	struct nss_qrfs_flow_rule_msg *nqfrm;

	if (nqm->cm.type != NSS_QRFS_MSG_FLOW_ADD) {
		nss_warning("%px: invalid flow response message %d\n", nss_ctx, nqm->cm.type);
		return;
	}

	nqfrm = &nqm->msg.flow_add;

	if ((nqfrm->ip_version != 4) && (nqfrm->ip_version != 6)) {
		nss_warning("%px: invalid IP version %d\n", nss_ctx, nqfrm->ip_version);
		return;
	}

	if (nqm->cm.response != NSS_CMN_RESPONSE_ACK) {
		nss_warning("%px: flow add configuration error: %d for NSS core %d\n",
				nss_ctx, nqm->cm.error, nss_ctx->id);
	}
}

/*
 * nss_qrfs_flow_delete_msg_callback()
 *	Callback function for receiving flow delete response messages.
 */
static void nss_qrfs_flow_delete_msg_callback(void *app_data, struct nss_qrfs_msg *nqm)
{
	struct nss_ctx_instance *nss_ctx __maybe_unused = (struct nss_ctx_instance *)app_data;
	struct nss_qrfs_flow_rule_msg *nqfrm;

	if (nqm->cm.type != NSS_QRFS_MSG_FLOW_DELETE) {
		nss_warning("%px: invalid flow response message %d\n", nss_ctx, nqm->cm.type);
		return;
	}

	nqfrm = &nqm->msg.flow_delete;

	if ((nqfrm->ip_version != 4) && (nqfrm->ip_version != 6)) {
		nss_warning("%px: invalid IP version %d\n", nss_ctx, nqfrm->ip_version);
		return;
	}

	if (nqm->cm.response != NSS_CMN_RESPONSE_ACK) {
		nss_warning("%px: flow delete configuration error: %d for NSS core %d\n",
				nss_ctx, nqm->cm.error, nss_ctx->id);
	}
}

/*
 * nss_qrfs_msg_init()
 *	Initialize the common header of QRFS message
 */
static void nss_qrfs_msg_init(struct nss_qrfs_msg *nqm, uint16_t if_num, uint32_t type, uint32_t len, void *cb, void *app_data)
{
	nss_cmn_msg_init(&nqm->cm, if_num, type, len, cb, app_data);
}

/*
 * nss_qrfs_tx_msg()
 *	Transmit a QRFS message to NSS firmware
 */
static nss_tx_status_t nss_qrfs_tx_msg(struct nss_ctx_instance *nss_ctx, struct nss_qrfs_msg *msg)
{
	struct nss_cmn_msg *ncm = &msg->cm;

	/*
	 * Trace messages.
	 */
	nss_qrfs_log_tx_msg(msg);

	/*
	 * Sanity check the message
	 */
	if (!nss_qrfs_verify_if_num(ncm->interface)) {
		nss_warning("%px: interface is not QRFS interface: %d\n", nss_ctx, ncm->interface);
		return NSS_TX_FAILURE;
	}

	if (ncm->type >= NSS_QRFS_MSG_MAX) {
		nss_warning("%px: message type is out of range: %d\n", nss_ctx, ncm->type);
		return NSS_TX_FAILURE;
	}

	return nss_core_send_cmd(nss_ctx, msg, sizeof(*msg), NSS_NBUF_PAYLOAD_SIZE);
}

/*
 * nss_qrfs_add_flow_rule()
 *	Set a QRFS flow rule add message and transmit the message to NSS core.
 */
static nss_tx_status_t nss_qrfs_add_flow_rule(struct nss_ctx_instance *nss_ctx, uint32_t if_num,
					struct sk_buff *skb, uint32_t cpu, bool need_cb)
{
	struct nss_qrfs_msg nqm;
	struct nss_qrfs_flow_rule_msg *nqfrm;
	nss_tx_status_t status;
	nss_qrfs_msg_callback_t cb = NULL;
	void *app_data = NULL;
	bool res;

	memset(&nqm, 0, sizeof(struct nss_qrfs_msg));

	if (need_cb) {
		cb = nss_qrfs_flow_add_msg_callback;
		app_data = (void *)nss_ctx;
	}

	/*
	 * Initialize common header of QRFS flow rule add message.
	 */
	nss_qrfs_msg_init(&nqm, NSS_QRFS_INTERFACE, NSS_QRFS_MSG_FLOW_ADD,
				sizeof(struct nss_qrfs_flow_rule_msg), cb, app_data);

	/*
	 * Set flow rule of QRFS flow rule add message
	 */
	nqfrm = &nqm.msg.flow_add;
	res = nss_qrfs_get_flow_keys(nss_ctx, skb, nqfrm);
	if (!res) {
		return NSS_TX_FAILURE;
	}

	nqfrm->cpu = (uint16_t)cpu;
	nqfrm->if_num = if_num;

	/*
	 * Send QRFS flow rule add message to NSS core
	 */
	status = nss_qrfs_tx_msg(nss_ctx, &nqm);
	if (status == NSS_TX_SUCCESS) {
		return status;
	}

	return NSS_TX_FAILURE;
}

/*
 * nss_qrfs_delete_flow_rule()
 *	Set a QRFS delete flow rule message and transmit the message to all NSS core.
 */
static nss_tx_status_t nss_qrfs_delete_flow_rule(struct nss_ctx_instance *nss_ctx, uint32_t if_num,
						struct sk_buff *skb, uint32_t cpu, bool need_cb)
{
	struct nss_qrfs_msg nqm;
	struct nss_qrfs_flow_rule_msg *nqfrm;
	nss_tx_status_t status;
	nss_qrfs_msg_callback_t cb = NULL;
	void *app_data = NULL;
	bool res;

	memset(&nqm, 0, sizeof(struct nss_qrfs_msg));

	if (need_cb) {
		cb = nss_qrfs_flow_delete_msg_callback;
		app_data = (void *)nss_ctx;
	}

	/*
	 * Initialize common header of QRFS flow rule delete message.
	 */
	nss_qrfs_msg_init(&nqm, NSS_QRFS_INTERFACE, NSS_QRFS_MSG_FLOW_DELETE,
				sizeof(struct nss_qrfs_flow_rule_msg), cb, app_data);

	/*
	 * Set flow rule of QRFS flow rule delete message
	 */
	nqfrm = &nqm.msg.flow_delete;
	res = nss_qrfs_get_flow_keys(nss_ctx, skb, nqfrm);
	if (!res) {
		return NSS_TX_FAILURE;
	}

	nqfrm->cpu = (uint16_t)cpu;
	nqfrm->if_num = if_num;

	/*
	 * Send QRFS flow rule delete message to NSS core
	 */
	status = nss_qrfs_tx_msg(nss_ctx, &nqm);
	if (status == NSS_TX_SUCCESS) {
		return status;
	}

	return NSS_TX_FAILURE;
}

/*
 * nss_qrfs_set_flow_rule()
 *	Set a QRFS flow rule message and transmit the message to all NSS cores.
 */
nss_tx_status_t nss_qrfs_set_flow_rule(struct sk_buff *skb, uint32_t cpu, uint32_t action)
{
	struct nss_ctx_instance *nss_ctx;
	nss_tx_status_t status;
	int i;

	for (i = 0; i < NSS_CORE_MAX; i++) {
		nss_ctx = nss_qrfs_get_ctx(i);

		/*
		 * Set QRFS flow rule message and transmit the message to NSS core.
		 *
		 * TODO: Remove if_num parameter from add_flow_rule() and
		 * delete_flow_rule(), since it is unused in firmware.
		 */
		if (action == NSS_QRFS_MSG_FLOW_ADD) {
			status = nss_qrfs_add_flow_rule(nss_ctx, 0, skb, cpu, true);
		} else {
			status = nss_qrfs_delete_flow_rule(nss_ctx, 0, skb, cpu, true);
		}

		if (status != NSS_TX_SUCCESS) {
			nss_warning("%px: failed to send flow rule to NSS core %d\n", nss_ctx, i);
			return NSS_TX_FAILURE;
		}
	}

	return NSS_TX_SUCCESS;
}
EXPORT_SYMBOL(nss_qrfs_set_flow_rule);

/*
 * nss_qrfs_register_handler()
 */
void nss_qrfs_register_handler(struct nss_ctx_instance *nss_ctx)
{
	nss_core_register_handler(nss_ctx, NSS_QRFS_INTERFACE, nss_qrfs_msg_handler, NULL);

	if (nss_ctx->id == NSS_CORE_0) {
		nss_qrfs_stats_dentry_create();
	}
}
EXPORT_SYMBOL(nss_qrfs_register_handler);

/*
 * nss_qrfs_notify_register()
 *	Register to receive QRFS notify messages.
 */
struct nss_ctx_instance *nss_qrfs_notify_register(int core, nss_qrfs_msg_callback_t cb, void *app_data)
{
	if (core >= NSS_CORE_MAX) {
		nss_warning("Input core number %d is wrong\n", core);
		return NULL;
	}

	nss_qrfs_notify[core].qrfs_callback = cb;
	nss_qrfs_notify[core].app_data = app_data;

	return (struct nss_ctx_instance *)&nss_top_main.nss[core];
}

/*
 * nss_qrfs_notify_unregister()
 *	Unregister to receive QRFS notify messages.
 */
void nss_qrfs_notify_unregister(int core)
{
	if (core >= NSS_CORE_MAX) {
		nss_warning("Input core number %d is wrong\n", core);
		return;
	}

	nss_qrfs_notify[core].qrfs_callback = NULL;
	nss_qrfs_notify[core].app_data = NULL;
}

/*
 * nss_qrfs_init()
 */
void nss_qrfs_init(void)
{
	int core;

	for (core = 0; core < NSS_CORE_MAX; core++) {
		nss_qrfs_notify_register(core, NULL, NULL);
	}
}

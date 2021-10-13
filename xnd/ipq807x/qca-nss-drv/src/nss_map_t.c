/*
 **************************************************************************
 * Copyright (c) 2016-2020, The Linux Foundation. All rights reserved.
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
#include "nss_map_t_stats.h"
#include "nss_map_t_log.h"
#include "nss_map_t_strings.h"

#define NSS_MAP_T_TX_TIMEOUT 3000 /* 3 Seconds */

/*
 * Private data structure
 */
static struct {
	struct semaphore sem;
	struct completion complete;
	int response;
	void *cb;
	void *app_data;
} nss_map_t_pvt;

/*
 * Data structures to store map_t nss debug stats
 */
static DEFINE_SPINLOCK(nss_map_t_debug_stats_lock);
static struct nss_map_t_stats_instance_debug nss_map_t_debug_stats[NSS_MAX_MAP_T_DYNAMIC_INTERFACES];

/*
 * nss_map_t_verify_if_num()
 *	Verify if_num passed to us.
 */
static bool nss_map_t_verify_if_num(uint32_t if_num)
{
	enum nss_dynamic_interface_type if_type;

	if (nss_is_dynamic_interface(if_num) == false) {
		return false;
	}

	if_type = nss_dynamic_interface_get_type(nss_map_t_get_context(), if_num);
	switch (if_type) {
	case NSS_DYNAMIC_INTERFACE_TYPE_MAP_T_INNER:
	case NSS_DYNAMIC_INTERFACE_TYPE_MAP_T_OUTER:
		return true;

	default:
		return false;
	}
}

/*
 * nss_map_t_instance_debug_stats_sync
 *	debug stats for map_t
 */
void nss_map_t_instance_debug_stats_sync(struct nss_ctx_instance *nss_ctx, struct nss_map_t_sync_stats_msg *stats_msg, uint16_t if_num)
{
	int i;
	enum nss_dynamic_interface_type if_type;

	if_type = nss_dynamic_interface_get_type(nss_ctx, if_num);

	spin_lock_bh(&nss_map_t_debug_stats_lock);
	for (i = 0; i < NSS_MAX_MAP_T_DYNAMIC_INTERFACES; i++) {
		if (nss_map_t_debug_stats[i].if_num != if_num) {
			continue;
		}
		switch (if_type) {
		case NSS_DYNAMIC_INTERFACE_TYPE_MAP_T_INNER:
			nss_map_t_debug_stats[i].stats[NSS_MAP_T_STATS_V4_TO_V6_PBUF_EXCEPTION] +=
				stats_msg->debug_stats.v4_to_v6.exception_pkts;
			nss_map_t_debug_stats[i].stats[NSS_MAP_T_STATS_V4_TO_V6_PBUF_NO_MATCHING_RULE] +=
				stats_msg->debug_stats.v4_to_v6.no_matching_rule;
			nss_map_t_debug_stats[i].stats[NSS_MAP_T_STATS_V4_TO_V6_PBUF_NOT_TCP_OR_UDP] +=
				stats_msg->debug_stats.v4_to_v6.not_tcp_or_udp;
			nss_map_t_debug_stats[i].stats[NSS_MAP_T_STATS_V4_TO_V6_RULE_ERR_LOCAL_PSID] +=
				stats_msg->debug_stats.v4_to_v6.rule_err_local_psid;
			nss_map_t_debug_stats[i].stats[NSS_MAP_T_STATS_V4_TO_V6_RULE_ERR_LOCAL_IPV6] +=
				stats_msg->debug_stats.v4_to_v6.rule_err_local_ipv6;
			nss_map_t_debug_stats[i].stats[NSS_MAP_T_STATS_V4_TO_V6_RULE_ERR_REMOTE_PSID] +=
				stats_msg->debug_stats.v4_to_v6.rule_err_remote_psid;
			nss_map_t_debug_stats[i].stats[NSS_MAP_T_STATS_V4_TO_V6_RULE_ERR_REMOTE_EA_BITS] +=
				stats_msg->debug_stats.v4_to_v6.rule_err_remote_ea_bits;
			nss_map_t_debug_stats[i].stats[NSS_MAP_T_STATS_V4_TO_V6_RULE_ERR_REMOTE_IPV6] +=
				stats_msg->debug_stats.v4_to_v6.rule_err_remote_ipv6;
			break;

		case NSS_DYNAMIC_INTERFACE_TYPE_MAP_T_OUTER:
			nss_map_t_debug_stats[i].stats[NSS_MAP_T_STATS_V6_TO_V4_PBUF_EXCEPTION] +=
				stats_msg->debug_stats.v6_to_v4.exception_pkts;
			nss_map_t_debug_stats[i].stats[NSS_MAP_T_STATS_V6_TO_V4_PBUF_NO_MATCHING_RULE] +=
				stats_msg->debug_stats.v6_to_v4.no_matching_rule;
			nss_map_t_debug_stats[i].stats[NSS_MAP_T_STATS_V6_TO_V4_PBUF_NOT_TCP_OR_UDP] +=
				stats_msg->debug_stats.v6_to_v4.not_tcp_or_udp;
			nss_map_t_debug_stats[i].stats[NSS_MAP_T_STATS_V6_TO_V4_RULE_ERR_LOCAL_IPV4] +=
				stats_msg->debug_stats.v6_to_v4.rule_err_local_ipv4;
			nss_map_t_debug_stats[i].stats[NSS_MAP_T_STATS_V6_TO_V4_RULE_ERR_REMOTE_IPV4] +=
				stats_msg->debug_stats.v6_to_v4.rule_err_remote_ipv4;
			break;

		default:
			nss_warning("Invalid MAP-T interface encountered: %u\n", if_type);
			break;
		}
	}
	spin_unlock_bh(&nss_map_t_debug_stats_lock);
}

/*
 * nss_map_t_instance_debug_stats_get()
 *	Get map_t statitics.
 */
void nss_map_t_instance_debug_stats_get(void *stats_mem)
{
	struct nss_map_t_stats_instance_debug *stats = (struct nss_map_t_stats_instance_debug *)stats_mem;
	int i;

	if (!stats) {
		nss_warning("No memory to copy map_t stats");
		return;
	}

	spin_lock_bh(&nss_map_t_debug_stats_lock);
	for (i = 0; i < NSS_MAX_MAP_T_DYNAMIC_INTERFACES; i++) {
		if (nss_map_t_debug_stats[i].valid) {
			memcpy(stats, &nss_map_t_debug_stats[i], sizeof(struct nss_map_t_stats_instance_debug));
			stats++;
		}
	}
	spin_unlock_bh(&nss_map_t_debug_stats_lock);
}

/*
 * nss_map_t_handler()
 *	Handle NSS -> HLOS messages for map_t tunnel
 */
static void nss_map_t_handler(struct nss_ctx_instance *nss_ctx, struct nss_cmn_msg *ncm, __attribute__((unused))void *app_data)
{
	struct nss_map_t_msg *ntm = (struct nss_map_t_msg *)ncm;
	void *ctx;

	nss_map_t_msg_callback_t cb;

	NSS_VERIFY_CTX_MAGIC(nss_ctx);
	BUG_ON(!nss_map_t_verify_if_num(ncm->interface));

	/*
	 * Trace Messages
	 */
	nss_map_t_log_rx_msg(ntm);

	/*
	 * Is this a valid request/response packet?
	 */
	if (ncm->type >= NSS_MAP_T_MSG_MAX) {
		nss_warning("%px: received invalid message %d for MAP-T interface", nss_ctx, ncm->type);
		return;
	}

	if (nss_cmn_get_msg_len(ncm) > sizeof(struct nss_map_t_msg)) {
		nss_warning("%px: tx request for another interface: %d", nss_ctx, ncm->interface);
		return;
	}

	switch (ntm->cm.type) {
	case NSS_MAP_T_MSG_SYNC_STATS:
		/*
		 * Update debug stats in stats msg and send statistics notifications to the registered modules
		 */
		nss_map_t_instance_debug_stats_sync(nss_ctx, &ntm->msg.stats, ncm->interface);
		nss_map_t_stats_notify(nss_ctx, ncm->interface);
		break;
	}

	/*
	 * Update the callback and app_data for NOTIFY messages, map_t sends all notify messages
	 * to the same callback/app_data.
	 */
	if (ncm->response == NSS_CMN_RESPONSE_NOTIFY) {
		ncm->cb = (nss_ptr_t)nss_ctx->nss_top->map_t_msg_callback;
		ncm->app_data = (nss_ptr_t)nss_ctx->subsys_dp_register[ncm->interface].app_data;
	}

	/*
	 * Log failures
	 */
	nss_core_log_msg_failures(nss_ctx, ncm);

	/*
	 * callback
	 */
	cb = (nss_map_t_msg_callback_t)ncm->cb;
	ctx = (void *)ncm->app_data;

	/*
	 * call map-t callback
	 */
	if (!cb) {
		nss_warning("%px: No callback for map-t interface %d",
			    nss_ctx, ncm->interface);
		return;
	}

	cb(ctx, ntm);
}

/*
 * nss_map_t_callback()
 *	Callback to handle the completion of NSS->HLOS messages.
 */
static void nss_map_t_callback(void *app_data, struct nss_map_t_msg *nim)
{
	nss_map_t_msg_callback_t callback = (nss_map_t_msg_callback_t)nss_map_t_pvt.cb;
	void *data = nss_map_t_pvt.app_data;

	nss_map_t_pvt.cb = NULL;
	nss_map_t_pvt.app_data = NULL;

	if (nim->cm.response != NSS_CMN_RESPONSE_ACK) {
		nss_warning("map_t Error response %d\n", nim->cm.response);
		nss_map_t_pvt.response = NSS_TX_FAILURE;
	} else {
		nss_map_t_pvt.response = NSS_TX_SUCCESS;
	}

	if (callback) {
		callback(data, nim);
	}

	complete(&nss_map_t_pvt.complete);
}

/*
 * nss_map_t_tx()
 *	Transmit a map_t message to NSS firmware
 */
nss_tx_status_t nss_map_t_tx(struct nss_ctx_instance *nss_ctx, struct nss_map_t_msg *msg)
{
	struct nss_cmn_msg *ncm = &msg->cm;

	/*
	 * Trace Messages
	 */
	nss_map_t_log_tx_msg(msg);

	/*
	 * Sanity check the message
	 */
	if (!nss_map_t_verify_if_num(ncm->interface)) {
		nss_warning("%px: tx request is not for a MAP-T dynamic interface: %d", nss_ctx, ncm->interface);
		return NSS_TX_FAILURE;
	}

	if (ncm->type > NSS_MAP_T_MSG_MAX) {
		nss_warning("%px: message type out of range: %d", nss_ctx, ncm->type);
		return NSS_TX_FAILURE;
	}

	return nss_core_send_cmd(nss_ctx, msg, sizeof(*msg), NSS_NBUF_PAYLOAD_SIZE);
}
EXPORT_SYMBOL(nss_map_t_tx);

/*
 * nss_map_t_tx_sync()
 *	Transmit a MAP-T message to NSS firmware synchronously.
 */
nss_tx_status_t nss_map_t_tx_sync(struct nss_ctx_instance *nss_ctx, struct nss_map_t_msg *msg)
{
	nss_tx_status_t status;
	int ret = 0;

	down(&nss_map_t_pvt.sem);
	nss_map_t_pvt.cb = (void *)msg->cm.cb;
	nss_map_t_pvt.app_data = (void *)msg->cm.app_data;

	msg->cm.cb = (nss_ptr_t)nss_map_t_callback;
	msg->cm.app_data = (nss_ptr_t)NULL;

	status = nss_map_t_tx(nss_ctx, msg);
	if (status != NSS_TX_SUCCESS) {
		nss_warning("%px: map_t_tx_msg failed\n", nss_ctx);
		up(&nss_map_t_pvt.sem);
		return status;
	}
	ret = wait_for_completion_timeout(&nss_map_t_pvt.complete, msecs_to_jiffies(NSS_MAP_T_TX_TIMEOUT));

	if (!ret) {
		nss_warning("%px: MAP-T tx sync failed due to timeout\n", nss_ctx);
		nss_map_t_pvt.response = NSS_TX_FAILURE;
	}

	status = nss_map_t_pvt.response;
	up(&nss_map_t_pvt.sem);
	return status;
}
EXPORT_SYMBOL(nss_map_t_tx_sync);

/*
 ***********************************
 * Register/Unregister/Miscellaneous APIs
 ***********************************
 */

/*
 * nss_map_t_register_if()
 */
struct nss_ctx_instance *nss_map_t_register_if(uint32_t if_num, uint32_t type, nss_map_t_callback_t map_t_callback,
			nss_map_t_msg_callback_t event_callback, struct net_device *netdev, uint32_t features)
{
	struct nss_ctx_instance *nss_ctx = (struct nss_ctx_instance *)&nss_top_main.nss[nss_top_main.map_t_handler_id];
	int i = 0;

	nss_assert(nss_ctx);
	nss_assert(nss_map_t_verify_if_num(if_num));

	nss_core_register_subsys_dp(nss_ctx, if_num, map_t_callback, 0, netdev, netdev, features);
	nss_ctx->subsys_dp_register[if_num].type = type;

	nss_top_main.map_t_msg_callback = event_callback;

	nss_core_register_handler(nss_ctx, if_num, nss_map_t_handler, NULL);

	spin_lock_bh(&nss_map_t_debug_stats_lock);
	for (i = 0; i < NSS_MAX_MAP_T_DYNAMIC_INTERFACES; i++) {
		if (!nss_map_t_debug_stats[i].valid) {
			nss_map_t_debug_stats[i].valid = true;
			nss_map_t_debug_stats[i].if_num = if_num;
			nss_map_t_debug_stats[i].if_index = netdev->ifindex;
			break;
		}
	}
	spin_unlock_bh(&nss_map_t_debug_stats_lock);

	return nss_ctx;
}
EXPORT_SYMBOL(nss_map_t_register_if);

/*
 * nss_map_t_unregister_if()
 */
void nss_map_t_unregister_if(uint32_t if_num)
{
	struct nss_ctx_instance *nss_ctx = (struct nss_ctx_instance *)&nss_top_main.nss[nss_top_main.map_t_handler_id];
	int i;

	nss_assert(nss_ctx);
	nss_assert(nss_map_t_verify_if_num(if_num));

	nss_core_unregister_subsys_dp(nss_ctx, if_num);

	nss_top_main.map_t_msg_callback = NULL;

	nss_core_unregister_handler(nss_ctx, if_num);

	spin_lock_bh(&nss_map_t_debug_stats_lock);
	for (i = 0; i < NSS_MAX_MAP_T_DYNAMIC_INTERFACES; i++) {
		if (nss_map_t_debug_stats[i].if_num == if_num) {
			memset(&nss_map_t_debug_stats[i], 0, sizeof(struct nss_map_t_stats_instance_debug));
			break;
		}
	}
	spin_unlock_bh(&nss_map_t_debug_stats_lock);
}
EXPORT_SYMBOL(nss_map_t_unregister_if);

/*
 * nss_get_map_t_context()
 */
struct nss_ctx_instance *nss_map_t_get_context()
{
	return (struct nss_ctx_instance *)&nss_top_main.nss[nss_top_main.map_t_handler_id];
}
EXPORT_SYMBOL(nss_map_t_get_context);

/*
 * nss_map_t_msg_init()
 *	Initialize nss_map_t msg.
 */
void nss_map_t_msg_init(struct nss_map_t_msg *ncm, uint16_t if_num, uint32_t type,  uint32_t len, void *cb, void *app_data)
{
	nss_cmn_msg_init(&ncm->cm, if_num, type, len, cb, app_data);
}
EXPORT_SYMBOL(nss_map_t_msg_init);

/*
 * nss_map_t_register_handler()
 *	debugfs stats msg handler received on static map_t interface
 */
void nss_map_t_register_handler(void)
{
	struct nss_ctx_instance *nss_ctx = nss_map_t_get_context();

	nss_info("nss_map_t_register_handler");
	sema_init(&nss_map_t_pvt.sem, 1);
	init_completion(&nss_map_t_pvt.complete);
	nss_core_register_handler(nss_ctx, NSS_MAP_T_INTERFACE, nss_map_t_handler, NULL);

	nss_map_t_stats_dentry_create();
	nss_map_t_strings_dentry_create();
}

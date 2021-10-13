/*
 **************************************************************************
 * Copyright (c) 2014-2021, The Linux Foundation. All rights reserved.
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
 * nss_ipv6.c
 *	NSS IPv6 APIs
 */
#include <nss_core.h>
#include "nss_dscp_map.h"
#include "nss_ipv6_stats.h"
#include "nss_ipv6_strings.h"

#define NSS_IPV6_TX_MSG_TIMEOUT 1000	/* 1 sec timeout for IPv6 messages */

/*
 * Private data structure for ipv6 configure messages
 */
struct nss_ipv6_cfg_pvt {
	struct semaphore sem;		/* Semaphore structure */
	struct completion complete;	/* Completion structure */
	int response;			/* Response from FW */
	void *cb;			/* Original cb for sync msgs */
	void *app_data;			/* Original app_data for sync msgs */
} nss_ipv6_pvt;

/*
 * Private data structure for ipv6 connection information.
 */
struct nss_ipv6_conn_table_info {
	uint32_t ce_table_size;		/* Size of connection entry table in NSS FW */
	uint32_t cme_table_size;	/* Size of connection match entry table in NSS FW */
	unsigned long ce_mem;		/* Start address for connection entry table */
	unsigned long cme_mem;		/* Start address for connection match entry table */
} nss_ipv6_ct_info;

int nss_ipv6_conn_cfg = NSS_DEFAULT_NUM_CONN;
int nss_ipv6_accel_mode_cfg __read_mostly = 1;

static struct nss_dscp_map_entry mapping[NSS_DSCP_MAP_ARRAY_SIZE];

/*
 * Callback for conn_sync_many request message.
 */
nss_ipv6_msg_callback_t nss_ipv6_conn_sync_many_msg_cb = NULL;

/*
 * nss_ipv6_dscp_map_usage()
 *	Help function shows the usage of the command.
 */
static inline void nss_ipv6_dscp_map_usage(void)
{
	nss_info_always("\nUsage:\n");
	nss_info_always("echo <dscp> <action> <prio> > /proc/sys/dev/nss/ipv6cfg/ipv6_dscp_map\n\n");
	nss_info_always("dscp[0-63] action[0-%u] prio[0-%u]:\n\n",
				NSS_IPV6_DSCP_MAP_ACTION_MAX - 1,
				NSS_DSCP_MAP_PRIORITY_MAX - 1);
}

/*
 * nss_ipv6_rx_msg_handler()
 *	Handle NSS -> HLOS messages for IPv6 bridge/route
 */
static void nss_ipv6_rx_msg_handler(struct nss_ctx_instance *nss_ctx, struct nss_cmn_msg *ncm, __attribute__((unused))void *app_data)
{
	struct nss_ipv6_msg *nim = (struct nss_ipv6_msg *)ncm;
	nss_ipv6_msg_callback_t cb;

	BUG_ON(ncm->interface != NSS_IPV6_RX_INTERFACE);

	/*
	 * Is this a valid request/response packet?
	 */
	if (ncm->type >= NSS_IPV6_MAX_MSG_TYPES) {
		nss_warning("%px: received invalid message %d for IPv6 interface", nss_ctx, nim->cm.type);
		return;
	}

	if (nss_cmn_get_msg_len(ncm) > sizeof(struct nss_ipv6_msg)) {
		nss_warning("%px: message length is invalid: %d", nss_ctx, nss_cmn_get_msg_len(ncm));
		return;
	}

	/*
	 * Trace messages.
	 */
	nss_ipv6_log_rx_msg(nim);

	/*
	 * Handle deprecated messages. Eventually these messages should be removed.
	 */
	switch (nim->cm.type) {
	case NSS_IPV6_RX_NODE_STATS_SYNC_MSG:
		/*
		* Update driver statistics on node sync and send statistics notifications to the registered modules.
		*/
		nss_ipv6_stats_node_sync(nss_ctx, &nim->msg.node_stats);
		nss_ipv6_stats_notify(nss_ctx);
		break;

	case NSS_IPV6_RX_CONN_STATS_SYNC_MSG:
		/*
		 * Update driver statistics on connection sync.
		 */
		nss_ipv6_stats_conn_sync(nss_ctx, &nim->msg.conn_stats);
		break;

	case NSS_IPV6_TX_CONN_STATS_SYNC_MANY_MSG:
		/*
		 * Update driver statistics on connection sync many.
		 */
		nss_ipv6_stats_conn_sync_many(nss_ctx, &nim->msg.conn_stats_many);
		ncm->cb = (nss_ptr_t)nss_ipv6_conn_sync_many_msg_cb;
		break;
	}

	/*
	 * Update the callback and app_data for NOTIFY messages, IPv6 sends all notify messages
	 * to the same callback/app_data.
	 */
	if (nim->cm.response == NSS_CMN_RESPONSE_NOTIFY) {
		ncm->cb = (nss_ptr_t)nss_ctx->nss_top->ipv6_callback;
		ncm->app_data = (nss_ptr_t)nss_ctx->nss_top->ipv6_ctx;
	}

	/*
	 * Do we have a callback?
	 */
	if (!ncm->cb) {
		return;
	}

	/*
	 * Callback
	 */
	cb = (nss_ipv6_msg_callback_t)ncm->cb;
	cb((void *)ncm->app_data, nim);
}

/*
 * nss_ipv6_tx_sync_callback()
 *	Callback to handle the completion of synchronous tx messages.
 */
static void nss_ipv6_tx_sync_callback(void *app_data, struct nss_ipv6_msg *nim)
{
	nss_ipv6_msg_callback_t callback = (nss_ipv6_msg_callback_t)nss_ipv6_pvt.cb;
	void *data = nss_ipv6_pvt.app_data;

	nss_ipv6_pvt.cb = NULL;
	nss_ipv6_pvt.app_data = NULL;

	if (nim->cm.response != NSS_CMN_RESPONSE_ACK) {
		nss_warning("ipv6 error response %d\n", nim->cm.response);
		nss_ipv6_pvt.response = NSS_TX_FAILURE;
	} else {
		nss_ipv6_pvt.response = NSS_TX_SUCCESS;
	}

	if (callback) {
		callback(data, nim);
	}

	complete(&nss_ipv6_pvt.complete);
}

/*
 * nss_ipv6_dscp_action_get()
 *	Gets the action mapped to dscp.
 */
enum nss_ipv6_dscp_map_actions nss_ipv6_dscp_action_get(uint8_t dscp)
{
	if (dscp >= NSS_DSCP_MAP_ARRAY_SIZE) {
		nss_warning("dscp:%u invalid\n", dscp);
		return NSS_IPV6_DSCP_MAP_ACTION_MAX;
	}

	return mapping[dscp].action;
}
EXPORT_SYMBOL(nss_ipv6_dscp_action_get);

/*
 * nss_ipv6_max_conn_count()
 *	Return the maximum number of IPv6 connections that the NSS acceleration engine supports.
 */
int nss_ipv6_max_conn_count(void)
{
	return nss_ipv6_conn_cfg;
}
EXPORT_SYMBOL(nss_ipv6_max_conn_count);

/*
 * nss_ipv6_conn_inquiry()
 *	Inquiry if a connection has been established in NSS FW
 */
nss_tx_status_t nss_ipv6_conn_inquiry(struct nss_ipv6_5tuple *ipv6_5t_p,
				nss_ipv6_msg_callback_t cb)
{
	nss_tx_status_t nss_tx_status;
	struct nss_ipv6_msg nim;
	struct nss_ctx_instance *nss_ctx = &nss_top_main.nss[0];

	/*
	 * Initialize inquiry message structure.
	 * This is async message and the result will be returned
	 * to the caller by the msg_callback passed in.
	 */
	memset(&nim, 0, sizeof(nim));
	nss_ipv6_msg_init(&nim, NSS_IPV6_RX_INTERFACE,
			NSS_IPV6_TX_CONN_CFG_INQUIRY_MSG,
			sizeof(struct nss_ipv6_inquiry_msg),
			cb, NULL);
	nim.msg.inquiry.rr.tuple = *ipv6_5t_p;
	nss_tx_status = nss_ipv6_tx(nss_ctx, &nim);
	if (nss_tx_status != NSS_TX_SUCCESS) {
		nss_warning("%px: Send inquiry message failed\n", ipv6_5t_p);
	}

	return nss_tx_status;
}
EXPORT_SYMBOL(nss_ipv6_conn_inquiry);

/*
 * nss_ipv6_tx_with_size()
 *	Transmit an ipv6 message to the FW with a specified size.
 */
nss_tx_status_t nss_ipv6_tx_with_size(struct nss_ctx_instance *nss_ctx, struct nss_ipv6_msg *nim, uint32_t size)
{
	struct nss_cmn_msg *ncm = &nim->cm;

	/*
	 * Sanity check the message
	 */
	if (ncm->interface != NSS_IPV6_RX_INTERFACE) {
		nss_warning("%px: tx request for another interface: %d", nss_ctx, ncm->interface);
		return NSS_TX_FAILURE;
	}

	if (ncm->type >= NSS_IPV6_MAX_MSG_TYPES) {
		nss_warning("%px: message type out of range: %d", nss_ctx, ncm->type);
		return NSS_TX_FAILURE;
	}

	/*
	 * Trace messages.
	 */
	nss_ipv6_log_tx_msg(nim);

	return nss_core_send_cmd(nss_ctx, nim, sizeof(*nim), size);
}
EXPORT_SYMBOL(nss_ipv6_tx_with_size);

/*
 * nss_ipv6_tx()
 *	Transmit an ipv6 message to the FW.
 */
nss_tx_status_t nss_ipv6_tx(struct nss_ctx_instance *nss_ctx, struct nss_ipv6_msg *nim)
{
	return nss_ipv6_tx_with_size(nss_ctx, nim, NSS_NBUF_PAYLOAD_SIZE);
}
EXPORT_SYMBOL(nss_ipv6_tx);

/*
 * nss_ipv6_tx_sync()
 *	Transmit a synchronous ipv6 message to the FW.
 */
nss_tx_status_t nss_ipv6_tx_sync(struct nss_ctx_instance *nss_ctx, struct nss_ipv6_msg *nim)
{
	nss_tx_status_t status;
	int ret = 0;

	down(&nss_ipv6_pvt.sem);
	nss_ipv6_pvt.cb = (void *)nim->cm.cb;
	nss_ipv6_pvt.app_data = (void *)nim->cm.app_data;

	nim->cm.cb = (nss_ptr_t)nss_ipv6_tx_sync_callback;
	nim->cm.app_data = (nss_ptr_t)NULL;

	status = nss_ipv6_tx(nss_ctx, nim);
	if (status != NSS_TX_SUCCESS) {
		nss_warning("%px: nss ipv6 msg tx failed\n", nss_ctx);
		up(&nss_ipv6_pvt.sem);
		return status;
	}

	ret = wait_for_completion_timeout(&nss_ipv6_pvt.complete, msecs_to_jiffies(NSS_IPV6_TX_MSG_TIMEOUT));
	if (!ret) {
		nss_warning("%px: IPv6 tx sync failed due to timeout\n", nss_ctx);
		nss_ipv6_pvt.response = NSS_TX_FAILURE;
	}

	status = nss_ipv6_pvt.response;
	up(&nss_ipv6_pvt.sem);
	return status;
}
EXPORT_SYMBOL(nss_ipv6_tx_sync);

/*
 **********************************
 Register/Unregister/Miscellaneous APIs
 **********************************
 */

/*
 * nss_ipv6_notify_register()
 *	Register to received IPv6 events.
 *
 * NOTE: Do we want to pass an nss_ctx here so that we can register for ipv6 on any core?
 */
struct nss_ctx_instance *nss_ipv6_notify_register(nss_ipv6_msg_callback_t cb, void *app_data)
{
	/*
	 * TODO: We need to have a new array in support of the new API
	 * TODO: If we use a per-context array, we would move the array into nss_ctx based.
	 */
	nss_top_main.ipv6_callback = cb;
	nss_top_main.ipv6_ctx = app_data;
	return &nss_top_main.nss[nss_top_main.ipv6_handler_id];
}
EXPORT_SYMBOL(nss_ipv6_notify_register);

/*
 * nss_ipv6_notify_unregister()
 *	Unregister to received IPv6 events.
 *
 * NOTE: Do we want to pass an nss_ctx here so that we can register for ipv6 on any core?
 */
void nss_ipv6_notify_unregister(void)
{
	nss_top_main.ipv6_callback = NULL;
}
EXPORT_SYMBOL(nss_ipv6_notify_unregister);

/*
 * nss_ipv6_conn_sync_many_notify_register()
 *	Register to receive IPv6 conn_sync_many message response.
 */
void nss_ipv6_conn_sync_many_notify_register(nss_ipv6_msg_callback_t cb)
{
	nss_ipv6_conn_sync_many_msg_cb = cb;
}
EXPORT_SYMBOL(nss_ipv6_conn_sync_many_notify_register);

/*
 * nss_ipv6_conn_sync_many_notify_unregister()
 *	Unregister to receive IPv6 conn_sync_many message response.
 */
void nss_ipv6_conn_sync_many_notify_unregister(void)
{
	nss_ipv6_conn_sync_many_msg_cb = NULL;
}
EXPORT_SYMBOL(nss_ipv6_conn_sync_many_notify_unregister);

/*
 * nss_ipv6_get_mgr()
 *
 * TODO: This only suppports a single ipv6, do we ever want to support more?
 */
struct nss_ctx_instance *nss_ipv6_get_mgr(void)
{
	return (void *)&nss_top_main.nss[nss_top_main.ipv6_handler_id];
}
EXPORT_SYMBOL(nss_ipv6_get_mgr);

/*
 * nss_ipv6_register_handler()
 *	Register our handler to receive messages for this interface
 */
void nss_ipv6_register_handler()
{
	struct nss_ctx_instance *nss_ctx = nss_ipv6_get_mgr();

	if (nss_core_register_handler(nss_ctx, NSS_IPV6_RX_INTERFACE, nss_ipv6_rx_msg_handler, NULL) != NSS_CORE_STATUS_SUCCESS) {
		nss_warning("IPv6 handler failed to register");
	}

	nss_ipv6_stats_dentry_create();
	nss_ipv6_strings_dentry_create();
}

/*
 * nss_ipv6_conn_cfg_process_callback()
 *	Call back function for the ipv6 connection configuration process.
 */
static void nss_ipv6_conn_cfg_process_callback(void *app_data, struct nss_ipv6_msg *nim)
{
	struct nss_ipv6_rule_conn_cfg_msg *nirccm = &nim->msg.rule_conn_cfg;
	struct nss_ctx_instance *nss_ctx __maybe_unused = nss_ipv6_get_mgr();

	if (nim->cm.response != NSS_CMN_RESPONSE_ACK) {
		nss_warning("%px: IPv6 connection configuration failed with error: %d\n", nss_ctx, nim->cm.error);
		nss_core_update_max_ipv6_conn(NSS_FW_DEFAULT_NUM_CONN);
		nss_ipv6_free_conn_tables();
		return;
	}

	nss_ipv6_conn_cfg = ntohl(nirccm->num_conn);

	nss_info("%px: IPv6 connection configuration success: %d\n", nss_ctx, nim->cm.error);
}

/*
 * nss_ipv6_conn_cfg_process()
 *	Process request to configure number of ipv6 connections
 */
static int nss_ipv6_conn_cfg_process(struct nss_ctx_instance *nss_ctx, int conn)
{
	struct nss_ipv6_msg nim;
	struct nss_ipv6_rule_conn_cfg_msg *nirccm;
	nss_tx_status_t nss_tx_status;

	if ((!nss_ipv6_ct_info.ce_table_size) || (!nss_ipv6_ct_info.cme_table_size)) {
		nss_warning("%px: connection entry or connection match entry table size not available\n",
				nss_ctx);
		return -EINVAL;
	}

	nss_info("%px: IPv6 supported connections: %d\n", nss_ctx, conn);

	nss_ipv6_ct_info.ce_mem = __get_free_pages(GFP_ATOMIC | __GFP_NOWARN | __GFP_ZERO,
					get_order(nss_ipv6_ct_info.ce_table_size));
	if (!nss_ipv6_ct_info.ce_mem) {
		nss_warning("%px: Memory allocation failed for IPv6 Connections: %d\n",
							nss_ctx,
							conn);
		goto fail;
	}
	nss_info("%px: CE Memory allocated for IPv6 Connections: %d\n",
							nss_ctx,
							conn);

	nss_ipv6_ct_info.cme_mem = __get_free_pages(GFP_ATOMIC | __GFP_NOWARN | __GFP_ZERO,
					get_order(nss_ipv6_ct_info.cme_table_size));
	if (!nss_ipv6_ct_info.cme_mem) {
		nss_warning("%px: Memory allocation failed for IPv6 Connections: %d\n",
							nss_ctx,
							conn);
		goto fail;
	}
	nss_info("%px: CME Memory allocated for IPv6 Connections: %d\n",
							nss_ctx,
							conn);

	memset(&nim, 0, sizeof(struct nss_ipv6_msg));
	nss_ipv6_msg_init(&nim, NSS_IPV6_RX_INTERFACE, NSS_IPV6_TX_CONN_CFG_RULE_MSG,
		sizeof(struct nss_ipv6_rule_conn_cfg_msg), nss_ipv6_conn_cfg_process_callback, NULL);

	nirccm = &nim.msg.rule_conn_cfg;
	nirccm->num_conn = htonl(conn);
	nirccm->ce_mem = dma_map_single(nss_ctx->dev, (void *)nss_ipv6_ct_info.ce_mem, nss_ipv6_ct_info.ce_table_size, DMA_TO_DEVICE);
	if (unlikely(dma_mapping_error(nss_ctx->dev, nirccm->ce_mem))) {
		nss_warning("%px: DMA mapping failed for virtual address = %px", nss_ctx, (void *)nss_ipv6_ct_info.ce_mem);
		goto fail;
	}

	nirccm->cme_mem = dma_map_single(nss_ctx->dev, (void *)nss_ipv6_ct_info.cme_mem, nss_ipv6_ct_info.cme_table_size, DMA_TO_DEVICE);
	if (unlikely(dma_mapping_error(nss_ctx->dev, nirccm->cme_mem))) {
		nss_warning("%px: DMA mapping failed for virtual address = %px", nss_ctx, (void *)nss_ipv6_ct_info.cme_mem);
		goto fail;
	}

	nss_tx_status = nss_ipv6_tx(nss_ctx, &nim);
	if (nss_tx_status != NSS_TX_SUCCESS) {
		nss_warning("%px: nss_tx error setting IPv6 Connections: %d\n",
						nss_ctx,
						conn);
		goto fail;
	}

	return 0;

fail:
	nss_ipv6_free_conn_tables();
	return -EINVAL;
}

/*
 * nss_ipv6_update_conn_count_callback()
 *	Call back function for the ipv6 get connection info message.
 */
static void nss_ipv6_update_conn_count_callback(void *app_data, struct nss_ipv6_msg *nim)
{
	struct nss_ipv6_rule_conn_get_table_size_msg *nircgts = &nim->msg.size;
	struct nss_ctx_instance *nss_ctx = nss_ipv6_get_mgr();

	if (nim->cm.response != NSS_CMN_RESPONSE_ACK) {
		nss_warning("%px: IPv6 fetch connection info failed with error: %d\n", nss_ctx, nim->cm.error);
		nss_core_update_max_ipv6_conn(NSS_FW_DEFAULT_NUM_CONN);
		return;
	}

	nss_info("IPv6 get connection info success\n");

	nss_ipv6_ct_info.ce_table_size = ntohl(nircgts->ce_table_size);
	nss_ipv6_ct_info.cme_table_size = ntohl(nircgts->cme_table_size);

	if (nss_ipv6_conn_cfg_process(nss_ctx, ntohl(nircgts->num_conn)) != 0) {
		nss_warning("%px: IPv6 connection entry or connection match entry table size\
				not available\n", nss_ctx);
	}

	return;
}

/*
 * nss_ipv6_update_conn_count()
 *	Sets the maximum number of IPv6 connections.
 *
 * It first gets the connection tables size information from NSS FW
 * and then configures the connections in NSS FW.
 */
int nss_ipv6_update_conn_count(int ipv6_num_conn)
{
	struct nss_ctx_instance *nss_ctx = nss_ipv6_get_mgr();
	struct nss_ipv6_msg nim;
	struct nss_ipv6_rule_conn_get_table_size_msg *nircgts;
	nss_tx_status_t nss_tx_status;
	uint32_t sum_of_conn;

	/*
	 * By default, NSS FW is configured with default number of connections.
	 */
	if (ipv6_num_conn == NSS_FW_DEFAULT_NUM_CONN) {
		nss_info("%px: Default number of connections (%d) already configured\n", nss_ctx, ipv6_num_conn);
		return 0;
	}

	/*
	 * Specifications for input
	 * 1) The input should be power of 2.
	 * 2) Input for ipv4 and ipv6 sum togther should not exceed 8k
	 * 3) Min. value should be at leat 256 connections. This is the
	 * minimum connections we will support for each of them.
	 */
	sum_of_conn = nss_ipv4_conn_cfg + ipv6_num_conn;
	if ((ipv6_num_conn & NSS_NUM_CONN_QUANTA_MASK) ||
		(sum_of_conn > NSS_MAX_TOTAL_NUM_CONN_IPV4_IPV6) ||
		(ipv6_num_conn < NSS_MIN_NUM_CONN)) {
		nss_warning("%px: input supported connections (%d) does not adhere\
				specifications\n1) not power of 2,\n2) is less than \
				min val: %d, OR\n	IPv4/6 total exceeds %d\n",
				nss_ctx,
				ipv6_num_conn,
				NSS_MIN_NUM_CONN,
				NSS_MAX_TOTAL_NUM_CONN_IPV4_IPV6);
		return -EINVAL;
	}

	memset(&nim, 0, sizeof(struct nss_ipv6_msg));
	nss_ipv6_msg_init(&nim, NSS_IPV6_RX_INTERFACE, NSS_IPV6_TX_CONN_TABLE_SIZE_MSG,
		sizeof(struct nss_ipv6_rule_conn_get_table_size_msg), nss_ipv6_update_conn_count_callback, NULL);

	nircgts = &nim.msg.size;
	nircgts->num_conn = htonl(ipv6_num_conn);
	nss_tx_status = nss_ipv6_tx(nss_ctx, &nim);
	if (nss_tx_status != NSS_TX_SUCCESS) {
		nss_warning("%px: Send acceleration mode message failed\n", nss_ctx);
		return -EINVAL;
	}

	return 0;
}

/*
 * nss_ipv6_free_conn_tables()
 *	Frees memory allocated for connection tables
 */
void nss_ipv6_free_conn_tables(void)
{
	if (nss_ipv6_ct_info.ce_mem) {
		free_pages(nss_ipv6_ct_info.ce_mem, get_order(nss_ipv6_ct_info.ce_table_size));
	}

	if (nss_ipv6_ct_info.cme_mem) {
		free_pages(nss_ipv6_ct_info.cme_mem, get_order(nss_ipv6_ct_info.cme_table_size));
	}

	memset(&nss_ipv6_ct_info, 0, sizeof(struct nss_ipv6_conn_table_info));
	return;
}

/*
 * nss_ipv6_accel_mode_cfg_handler()
 *	Configure acceleration mode for IPv6
 */
static int nss_ipv6_accel_mode_cfg_handler(struct ctl_table *ctl, int write, void __user *buffer, size_t *lenp, loff_t *ppos)
{
	struct nss_top_instance *nss_top = &nss_top_main;
	struct nss_ctx_instance *nss_ctx = &nss_top->nss[0];
	struct nss_ipv6_msg nim;
	struct nss_ipv6_accel_mode_cfg_msg *nipcm;
	nss_tx_status_t nss_tx_status;
	int ret = NSS_FAILURE;
	int current_value;

	/*
	 * Take snap shot of current value
	 */
	current_value = nss_ipv6_accel_mode_cfg;

	/*
	 * Write the variable with user input
	 */
	ret = proc_dointvec(ctl, write, buffer, lenp, ppos);
	if (ret || (!write)) {
		return ret;
	}

	memset(&nim, 0, sizeof(struct nss_ipv6_msg));
	nss_ipv6_msg_init(&nim, NSS_IPV6_RX_INTERFACE, NSS_IPV6_TX_ACCEL_MODE_CFG_MSG,
		sizeof(struct nss_ipv6_accel_mode_cfg_msg), NULL, NULL);

	nipcm = &nim.msg.accel_mode_cfg;
	nipcm->mode = htonl(nss_ipv6_accel_mode_cfg);

	nss_tx_status = nss_ipv6_tx_sync(nss_ctx, &nim);
	if (nss_tx_status != NSS_TX_SUCCESS) {
		nss_warning("%px: Send acceleration mode message failed\n", nss_ctx);
		nss_ipv6_accel_mode_cfg = current_value;
		return -EIO;
	}

	return 0;
}

/*
 * nss_ipv6_dscp_map_cfg_handler()
 *	Sysctl handler for dscp/pri mappings.
 */
static int nss_ipv6_dscp_map_cfg_handler(struct ctl_table *ctl, int write, void __user *buffer, size_t *lenp, loff_t *ppos)
{
	struct nss_top_instance *nss_top = &nss_top_main;
	struct nss_ctx_instance *nss_ctx = &nss_top->nss[0];
	struct nss_dscp_map_parse out;
	struct nss_ipv6_msg nim;
	struct nss_ipv6_dscp2pri_cfg_msg *nipd2p;
	nss_tx_status_t status;
	int ret;

	if (!write) {
		return nss_dscp_map_print(ctl, buffer, lenp, ppos, mapping);
	}

	ret = nss_dscp_map_parse(ctl, buffer, lenp, ppos, &out);
	if (ret) {
		nss_warning("failed to parse dscp mapping:%d\n", ret);
		return ret;
	}

	if (out.action >= NSS_IPV6_DSCP_MAP_ACTION_MAX) {
		nss_warning("invalid action value: %d\n", out.action);
		nss_ipv6_dscp_map_usage();
		return -EINVAL;
	}

	memset(&nim, 0, sizeof(struct nss_ipv6_msg));
	nss_ipv6_msg_init(&nim, NSS_IPV6_RX_INTERFACE, NSS_IPV6_TX_DSCP2PRI_CFG_MSG,
		sizeof(struct nss_ipv6_dscp2pri_cfg_msg), NULL, NULL);

	nipd2p = &nim.msg.dscp2pri_cfg;
	nipd2p->dscp = out.dscp;
	nipd2p->priority = out.priority;

	status = nss_ipv6_tx_sync(nss_ctx, &nim);
	if (status != NSS_TX_SUCCESS) {
		nss_warning("%px: ipv6 dscp2pri config message failed\n", nss_ctx);
		return -EFAULT;
	}

	/*
	 * NSS firmware acknowleged the configuration, so update the mapping
	 * table on HOST side as well.
	 */
	mapping[out.dscp].action = out.action;
	mapping[out.dscp].priority = out.priority;

	return 0;
}

static struct ctl_table nss_ipv6_table[] = {
	{
		.procname		= "ipv6_accel_mode",
		.data			= &nss_ipv6_accel_mode_cfg,
		.maxlen			= sizeof(int),
		.mode			= 0644,
		.proc_handler		= &nss_ipv6_accel_mode_cfg_handler,
	},
	{
		.procname		= "ipv6_dscp_map",
		.data			= &mapping[NSS_DSCP_MAP_ARRAY_SIZE],
		.maxlen			= sizeof(struct nss_dscp_map_entry),
		.mode			= 0644,
		.proc_handler		= &nss_ipv6_dscp_map_cfg_handler,
	},
	{ }
};

static struct ctl_table nss_ipv6_dir[] = {
	{
		.procname		= "ipv6cfg",
		.mode			= 0555,
		.child			= nss_ipv6_table,
	},
	{ }
};

static struct ctl_table nss_ipv6_root_dir[] = {
	{
		.procname		= "nss",
		.mode			= 0555,
		.child			= nss_ipv6_dir,
	},
	{ }
};

static struct ctl_table nss_ipv6_root[] = {
	{
		.procname		= "dev",
		.mode			= 0555,
		.child			= nss_ipv6_root_dir,
	},
	{ }
};

static struct ctl_table_header *nss_ipv6_header;

/*
 * nss_ipv6_register_sysctl()
 *	Register sysctl specific to ipv6
 */
void nss_ipv6_register_sysctl(void)
{
	sema_init(&nss_ipv6_pvt.sem, 1);
	init_completion(&nss_ipv6_pvt.complete);

	/*
	 * Register sysctl table.
	 */
	nss_ipv6_header = register_sysctl_table(nss_ipv6_root);
}

/*
 * nss_ipv6_unregister_sysctl()
 *	Unregister sysctl specific to ipv6
 */
void nss_ipv6_unregister_sysctl(void)
{
	/*
	 * Unregister sysctl table.
	 */
	if (nss_ipv6_header) {
		unregister_sysctl_table(nss_ipv6_header);
	}
}

/*
 * nss_ipv6_msg_init()
 *	Initialize IPv6 message.
 */
void nss_ipv6_msg_init(struct nss_ipv6_msg *nim, uint16_t if_num, uint32_t type, uint32_t len,
			nss_ipv6_msg_callback_t cb, void *app_data)
{
	nss_cmn_msg_init(&nim->cm, if_num, type, len, (void *)cb, app_data);
}
EXPORT_SYMBOL(nss_ipv6_msg_init);

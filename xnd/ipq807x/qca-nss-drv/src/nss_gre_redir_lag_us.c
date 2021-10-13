/*
 ****************************************************************************
 * Copyright (c) 2018, 2020-2021, The Linux Foundation. All rights reserved.
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
 ****************************************************************************
 */

#include "nss_tx_rx_common.h"
#include "nss_gre_redir_lag_us_stats.h"
#include "nss_gre_redir_lag_us_log.h"
#include "nss_gre_redir_lag_us_strings.h"

#define NSS_GRE_REDIR_LAG_US_TX_TIMEOUT 3000 /* 3 Seconds */
#define NSS_GRE_REDIR_LAG_US_STATS_SYNC_PERIOD msecs_to_jiffies(4000)
#define NSS_GRE_REDIR_LAG_US_STATS_SYNC_UDELAY 4000

struct nss_gre_redir_lag_us_cmn_ctx cmn_ctx;

/*
 * Sync response context.
 */
static struct {
	struct semaphore sem;
	struct completion complete;
	int response;
	nss_gre_redir_lag_us_msg_callback_t *cb;
	void *app_data;
} nss_gre_redir_lag_us_sync_ctx;

/*
 * nss_gre_redir_lag_us_callback()
 *	Callback to handle the completion of HLOS-->NSS messages.
 */
static void nss_gre_redir_lag_us_callback(void *app_data, struct nss_gre_redir_lag_us_msg *nim)
{
	nss_gre_redir_lag_us_msg_callback_t callback = (nss_gre_redir_lag_us_msg_callback_t)nss_gre_redir_lag_us_sync_ctx.cb;
	void *data = nss_gre_redir_lag_us_sync_ctx.app_data;

	nss_gre_redir_lag_us_sync_ctx.cb = NULL;
	nss_gre_redir_lag_us_sync_ctx.app_data = NULL;

	if (nim->cm.response != NSS_CMN_RESPONSE_ACK) {
		nss_warning("GRE redir LAG US Error response %d\n", nim->cm.response);
		nss_gre_redir_lag_us_sync_ctx.response = NSS_TX_FAILURE;
	} else {
		nss_gre_redir_lag_us_sync_ctx.response = NSS_TX_SUCCESS;
	}

	if (callback) {
		callback(data, &nim->cm);
	}

	complete(&nss_gre_redir_lag_us_sync_ctx.complete);
}

/*
 * nss_gre_redir_lag_us_hash_update_stats_req()
 *	Update query hash message's index for next request.
 */
static void nss_gre_redir_lag_us_hash_update_stats_req(struct nss_ctx_instance *nss_ctx, struct nss_gre_redir_lag_us_msg *ngrm)
{
	uint32_t ifnum = ngrm->cm.interface;
	uint32_t idx, sync_delay = NSS_GRE_REDIR_LAG_US_STATS_SYNC_PERIOD;
	struct nss_gre_redir_lag_us_hash_stats_query_msg *nim = &ngrm->msg.hash_stats;

	spin_lock_bh(&cmn_ctx.nss_gre_redir_lag_us_stats_lock);
	if (!nss_gre_redir_lag_us_get_node_idx(ifnum, &idx)) {
		spin_unlock_bh(&cmn_ctx.nss_gre_redir_lag_us_stats_lock);
		nss_warning("%px: Unable to update hash stats msg. Stats context not found.\n", nss_ctx);
		return;
	}

	/*
	 * Update start index for next iteration of the query.
	 */
	if (ngrm->cm.response == NSS_CMN_RESPONSE_ACK) {
		cmn_ctx.stats_ctx[idx].db_sync_msg.msg.hash_stats.db_entry_idx = nim->db_entry_next;
	} else {
		cmn_ctx.stats_ctx[idx].db_sync_msg.msg.hash_stats.db_entry_idx = 0;
	}

	/*
	 * If more hash entries are to be fetched from FW, queue work with delay of one eighth of
	 * the polling period. Else, schedule work with a delay of polling period.
	 */
	if (cmn_ctx.stats_ctx[idx].db_sync_msg.msg.hash_stats.db_entry_idx)
		sync_delay = NSS_GRE_REDIR_LAG_US_STATS_SYNC_PERIOD / 8;

	queue_delayed_work(cmn_ctx.nss_gre_redir_lag_us_wq, &(cmn_ctx.stats_ctx[idx].nss_gre_redir_lag_us_work), sync_delay);
	spin_unlock_bh(&cmn_ctx.nss_gre_redir_lag_us_stats_lock);
}

/*
 * nss_gre_redir_lag_us_handler()
 *	Handle NSS -> HLOS messages for gre tunnel
 */
static void nss_gre_redir_lag_us_msg_handler(struct nss_ctx_instance *nss_ctx, struct nss_cmn_msg *ncm, __attribute__((unused))void *app_data)
{
	void *ctx;
	struct nss_gre_redir_lag_us_msg *ngrm = (struct nss_gre_redir_lag_us_msg *)ncm;
	nss_gre_redir_lag_us_msg_callback_t cb;

	/*
	 * Interface should be a dynamic interface of type NSS_DYNAMIC_INTERFACE_TYPE_GRE_REDIR_LAG_US.
	 */
	BUG_ON(!nss_gre_redir_lag_us_verify_ifnum(ncm->interface));

	/*
	 * Trace messages.
	 */
	nss_gre_redir_lag_us_log_rx_msg(ngrm);

	/*
	 * Is this a valid request/response packet?
	 */
	if (ncm->type >=  NSS_GRE_REDIR_LAG_US_MAX_MSG_TYPES) {
		nss_warning("%px: received invalid message %d for gre interface\n", nss_ctx, ncm->type);
		return;
	}

	if (nss_cmn_get_msg_len(ncm) > sizeof(struct nss_gre_redir_lag_us_msg)) {
		nss_warning("%px: Length of message is greater than required: %d\n", nss_ctx, nss_cmn_get_msg_len(ncm));
		return;
	}

	/*
	 * Update the callback and app_data for NOTIFY messages, GRE sends all notify messages
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

	switch (ncm->type) {
	case NSS_GRE_REDIR_LAG_US_CMN_STATS_SYNC_MSG:
		nss_gre_redir_lag_us_stats_sync(nss_ctx, &ngrm->msg.us_sync_stats, ncm->interface);
		nss_gre_redir_lag_us_stats_notify(nss_ctx, ncm->interface);
		break;

	case NSS_GRE_REDIR_LAG_US_DB_HASH_NODE_MSG:
		nss_gre_redir_lag_us_hash_update_stats_req(nss_ctx, ngrm);
		break;
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
	cb = (nss_gre_redir_lag_us_msg_callback_t)ncm->cb;
	ctx = (void *)ncm->app_data;

	/*
	 * call gre tunnel callback
	 */
	cb(ctx, ncm);
}

/*
 * nss_gre_redir_lag_us_tx_msg_with_size()
 *	Transmit a GRE message to NSSFW with size.
 */
static nss_tx_status_t nss_gre_redir_lag_us_tx_msg_with_size(struct nss_ctx_instance *nss_ctx, struct nss_gre_redir_lag_us_msg *msg, uint32_t size)
{
	struct nss_cmn_msg *ncm = &msg->cm;

	/*
	 * Sanity check the message. Interface should be a dynamic
	 * interface of type NSS_DYNAMIC_INTERFACE_TYPE_GRE_REDIR_LAG_US.
	 */
	if (!nss_gre_redir_lag_us_verify_ifnum(ncm->interface)) {
		nss_warning("%px: tx request for another interface: %d\n", nss_ctx, ncm->interface);
		return NSS_TX_FAILURE;
	}

	if (ncm->type >= NSS_GRE_REDIR_LAG_US_MAX_MSG_TYPES) {
		nss_warning("%px: message type out of range: %d\n", nss_ctx, ncm->type);
		return NSS_TX_FAILURE;
	}

	return nss_core_send_cmd(nss_ctx, msg, sizeof(*msg), size);
}

/*
 * nss_gre_redir_lag_us_tx_msg_sync_with_size()
 *	Transmit a GRE LAG message to NSS firmware synchronously with size.
 */
static nss_tx_status_t nss_gre_redir_lag_us_tx_msg_sync_with_size(struct nss_ctx_instance *nss_ctx,
		 struct nss_gre_redir_lag_us_msg *ngrm, uint32_t size)
{
	nss_tx_status_t status;
	int ret = 0;

	down(&nss_gre_redir_lag_us_sync_ctx.sem);

	/*
	 * Save the client's callback, and initialize the message
	 * with the callback which releases the semaphore after message
	 * response is received, This callback will inturn call the client's
	 * callback.
	 */
	nss_gre_redir_lag_us_sync_ctx.cb = (void *)ngrm->cm.cb;
	nss_gre_redir_lag_us_sync_ctx.app_data = (void *)ngrm->cm.app_data;
	ngrm->cm.cb = (nss_ptr_t)nss_gre_redir_lag_us_callback;
	ngrm->cm.app_data = (nss_ptr_t)NULL;

	status = nss_gre_redir_lag_us_tx_msg_with_size(nss_ctx, ngrm, size);
	if (status != NSS_TX_SUCCESS) {
		nss_warning("%px: gre_tx_msg failed\n", nss_ctx);
		up(&nss_gre_redir_lag_us_sync_ctx.sem);
		return status;
	}

	ret = wait_for_completion_timeout(&nss_gre_redir_lag_us_sync_ctx.complete, msecs_to_jiffies(NSS_GRE_REDIR_LAG_US_TX_TIMEOUT));
	if (!ret) {
		nss_warning("%px: GRE LAG US tx sync failed due to timeout\n", nss_ctx);
		nss_gre_redir_lag_us_sync_ctx.response = NSS_TX_FAILURE;
	}

	status = nss_gre_redir_lag_us_sync_ctx.response;
	up(&nss_gre_redir_lag_us_sync_ctx.sem);
	return status;
}

/*
 * nss_gre_redir_lag_us_stats_sync_req_work()
 *	Work function for hash statistics synchronization.
 */
static void nss_gre_redir_lag_us_stats_sync_req_work(struct work_struct *work)
{
	struct delayed_work *d_work = container_of(work, struct delayed_work, work);
	struct nss_gre_redir_lag_us_pvt_sync_stats *sync_ctx = container_of(d_work, struct nss_gre_redir_lag_us_pvt_sync_stats,
			nss_gre_redir_lag_us_work);
	struct nss_gre_redir_lag_us_hash_stats_query_msg *nicsm_req = &(sync_ctx->db_sync_msg.msg.hash_stats);
	nss_tx_status_t nss_tx_status;
	nss_gre_redir_lag_us_msg_callback_t cb;
	void *app_data;
	struct nss_ctx_instance *nss_ctx __maybe_unused = nss_gre_redir_lag_us_get_context();
	int retry = NSS_GRE_REDIR_LAG_US_STATS_SYNC_RETRY;

	spin_lock_bh(&cmn_ctx.nss_gre_redir_lag_us_stats_lock);
	cb = sync_ctx->cb;
	app_data = sync_ctx->app_data;
	spin_unlock_bh(&cmn_ctx.nss_gre_redir_lag_us_stats_lock);

	nss_cmn_msg_init(&(sync_ctx->db_sync_msg.cm), sync_ctx->ifnum,
			NSS_GRE_REDIR_LAG_US_DB_HASH_NODE_MSG, sizeof(struct nss_gre_redir_lag_us_hash_stats_query_msg),
			cb, app_data);
	while (retry) {
		nss_tx_status = nss_gre_redir_lag_us_tx_msg_sync_with_size(nss_ctx, &(sync_ctx->db_sync_msg), PAGE_SIZE);
		if (nss_tx_status == NSS_TX_SUCCESS) {
			return;
		}

		retry--;
		nss_warning("%px: TX_NOT_OKAY, try again later\n", nss_ctx);
		usleep_range(100, 200);
	}

	/*
	 * TX failed after retries, take fresh start.
	 */
	nicsm_req->count = 0;
	nicsm_req->db_entry_idx = 0;
	queue_delayed_work(cmn_ctx.nss_gre_redir_lag_us_wq, &(sync_ctx->nss_gre_redir_lag_us_work), NSS_GRE_REDIR_LAG_US_STATS_SYNC_PERIOD);
}

/*
 * nss_gre_redir_lag_us_sync_work_init()
 *	Initialize work.
 */
static bool nss_gre_redir_lag_us_sync_work_init(uint32_t ifnum)
{
	struct nss_gre_redir_lag_us_hash_stats_query_msg *hash_stats_msg;
	struct nss_ctx_instance __maybe_unused *nss_ctx = nss_gre_redir_lag_us_get_context();
	int ret, idx;

	spin_lock_bh(&cmn_ctx.nss_gre_redir_lag_us_stats_lock);
	if (!nss_gre_redir_lag_us_get_node_idx(ifnum, &idx)) {
		spin_unlock_bh(&cmn_ctx.nss_gre_redir_lag_us_stats_lock);
		nss_warning("%px: Unable to init work. Stats context not found.\n", nss_ctx);
		return false;
	}

	hash_stats_msg = &(cmn_ctx.stats_ctx[idx].db_sync_msg.msg.hash_stats);
	hash_stats_msg->db_entry_idx = 0;
	INIT_DELAYED_WORK(&(cmn_ctx.stats_ctx[idx].nss_gre_redir_lag_us_work), nss_gre_redir_lag_us_stats_sync_req_work);
	ret = queue_delayed_work(cmn_ctx.nss_gre_redir_lag_us_wq,
			&(cmn_ctx.stats_ctx[idx].nss_gre_redir_lag_us_work), NSS_GRE_REDIR_LAG_US_STATS_SYNC_PERIOD);
	if (!ret) {
		spin_unlock_bh(&cmn_ctx.nss_gre_redir_lag_us_stats_lock);
		nss_warning("%px: Unable to queue work function to work queue\n", nss_ctx);
		return false;
	}

	spin_unlock_bh(&cmn_ctx.nss_gre_redir_lag_us_stats_lock);
	return true;
}

/*
 * nss_gre_redir_lag_us_unregister_if()
 *	Unregister GRE redirect LAG upstream node.
 */
static enum nss_gre_redir_lag_err_types nss_gre_redir_lag_us_unregister_if(uint32_t if_num)
{
	struct nss_ctx_instance *nss_ctx __maybe_unused = nss_gre_redir_lag_us_get_context();
	uint32_t status;
	int idx;

	nss_assert(nss_ctx);
	nss_assert(!nss_gre_redir_lag_us_verify_ifnum(if_num));

	status = nss_core_unregister_msg_handler(nss_ctx, if_num);
	if (status != NSS_CORE_STATUS_SUCCESS) {
		nss_warning("%px: Not able to unregister handler for gre_lag interface %d with NSS core\n", nss_ctx, if_num);
		return NSS_GRE_REDIR_LAG_ERR_CORE_UNREGISTER_FAILED;
	}

	status = nss_core_unregister_handler(nss_ctx, if_num);
	if (status != NSS_CORE_STATUS_SUCCESS) {
		nss_warning("%px: Not able to unregister handler for gre_lag interface %d with NSS core\n", nss_ctx, if_num);
		return NSS_GRE_REDIR_LAG_ERR_CORE_UNREGISTER_FAILED;
	}

	nss_core_unregister_subsys_dp(nss_ctx, if_num);

	spin_lock_bh(&cmn_ctx.nss_gre_redir_lag_us_stats_lock);
	if (!nss_gre_redir_lag_us_get_node_idx(if_num, &idx)) {
		spin_unlock_bh(&cmn_ctx.nss_gre_redir_lag_us_stats_lock);
		nss_warning("Stats context not found.\n");
		return NSS_GRE_REDIR_LAG_ERR_STATS_INDEX_NOT_FOUND;
	}

	cmn_ctx.stats_ctx[idx].cb = NULL;
	cmn_ctx.stats_ctx[idx].app_data = NULL;
	cmn_ctx.stats_ctx[idx].valid = false;
	spin_unlock_bh(&cmn_ctx.nss_gre_redir_lag_us_stats_lock);

	/*
	 * Work is per LAG US node. Cancel works for this node.
	 */
	cancel_delayed_work_sync(&(cmn_ctx.stats_ctx[idx].nss_gre_redir_lag_us_work));
	return NSS_GRE_REDIR_LAG_SUCCESS;
}

/*
 * nss_gre_redir_lag_us_register_if()
 *	Register GRE redirect LAG upstream node.
 */
static struct nss_ctx_instance *nss_gre_redir_lag_us_register_if(uint32_t if_num, struct net_device *netdev,
		nss_gre_redir_lag_us_data_callback_t cb_func_data,
		nss_gre_redir_lag_us_msg_callback_t cb_func_msg, uint32_t features, uint32_t type, void *app_ctx)
{
	struct nss_ctx_instance *nss_ctx = nss_gre_redir_lag_us_get_context();
	uint32_t status;
	int i;
	nss_assert(nss_ctx);
	nss_assert(!nss_gre_redir_lag_us_verify_ifnum(if_num));

	spin_lock_bh(&cmn_ctx.nss_gre_redir_lag_us_stats_lock);
	for (i = 0; i < NSS_GRE_REDIR_LAG_MAX_NODE; i++) {
		if (!cmn_ctx.stats_ctx[i].valid) {
			cmn_ctx.stats_ctx[i].ifnum = if_num;
			cmn_ctx.stats_ctx[i].valid = true;
			cmn_ctx.stats_ctx[i].cb = cb_func_msg;
			cmn_ctx.stats_ctx[i].app_data = app_ctx;
			break;
		}
	}

	spin_unlock_bh(&cmn_ctx.nss_gre_redir_lag_us_stats_lock);
	if (i == NSS_GRE_REDIR_LAG_MAX_NODE) {
		nss_warning("Maximum number of LAG nodes are already present.\n");
		return NULL;
	}

	/*
	 * Registering handler for sending tunnel interface msgs to NSS.
	 */
	status = nss_core_register_handler(nss_ctx, if_num, nss_gre_redir_lag_us_msg_handler, app_ctx);
	if (status != NSS_CORE_STATUS_SUCCESS) {
		nss_warning("%px: Not able to register handler for gre_lag interface %d with NSS core\n", nss_ctx, if_num);
		spin_lock_bh(&cmn_ctx.nss_gre_redir_lag_us_stats_lock);
		cmn_ctx.stats_ctx[i].valid = false;
		cmn_ctx.stats_ctx[i].cb = NULL;
		cmn_ctx.stats_ctx[i].app_data = NULL;
		spin_unlock_bh(&cmn_ctx.nss_gre_redir_lag_us_stats_lock);
		return NULL;
	}

	/*
	 * Registering handler for sending tunnel interface msgs to NSS.
	 */
	status = nss_core_register_msg_handler(nss_ctx, if_num, cb_func_msg);
	if (status != NSS_CORE_STATUS_SUCCESS) {
		nss_core_unregister_handler(nss_ctx, if_num);
		nss_warning("%px: Not able to register handler for gre_lag interface %d with NSS core\n", nss_ctx, if_num);
		spin_lock_bh(&cmn_ctx.nss_gre_redir_lag_us_stats_lock);
		cmn_ctx.stats_ctx[i].valid = false;
		cmn_ctx.stats_ctx[i].cb = NULL;
		cmn_ctx.stats_ctx[i].app_data = NULL;
		spin_unlock_bh(&cmn_ctx.nss_gre_redir_lag_us_stats_lock);
		return NULL;
	}

	nss_core_register_subsys_dp(nss_ctx, if_num, cb_func_data, NULL, NULL, netdev, features);
	nss_core_set_subsys_dp_type(nss_ctx, netdev, if_num, type);
	return nss_ctx;
}

/*
 * nss_gre_redir_lag_us_get_node_idx()
 *	Returns index of statistics context.
 */
bool nss_gre_redir_lag_us_get_node_idx(uint32_t ifnum, uint32_t *idx)
{
	uint32_t node_idx;
	for (node_idx = 0; node_idx < NSS_GRE_REDIR_LAG_MAX_NODE; node_idx++) {
		if ((cmn_ctx.stats_ctx[node_idx].valid) && (cmn_ctx.stats_ctx[node_idx].ifnum == ifnum)) {
			*idx = node_idx;
			return true;
		}
	}

	return false;
}

/*
 * nss_gre_redir_lag_us_verify_ifnum()
 *	Verify interface type.
 */
bool nss_gre_redir_lag_us_verify_ifnum(uint32_t if_num)
{
	return nss_dynamic_interface_get_type(nss_gre_redir_lag_us_get_context(), if_num) == NSS_DYNAMIC_INTERFACE_TYPE_GRE_REDIR_LAG_US;
}

/*
 * nss_gre_redir_lag_us_get_context()
 *	Retrieve context for GRE redirect LAG upstream node.
 */
struct nss_ctx_instance *nss_gre_redir_lag_us_get_context(void)
{
	return (struct nss_ctx_instance *)&nss_top_main.nss[nss_top_main.gre_redir_lag_us_handler_id];
}
EXPORT_SYMBOL(nss_gre_redir_lag_us_get_context);

/*
 * nss_gre_redir_lag_us_configure_node()
 *	Configure upstream lag node.
 */
bool nss_gre_redir_lag_us_configure_node(uint32_t ifnum,
		struct nss_gre_redir_lag_us_config_msg *ngluc)
{
	struct nss_gre_redir_lag_us_msg *config;
	uint32_t len, iftype, idx = 0, i;
	bool ret;
	nss_tx_status_t status;
	struct nss_ctx_instance *nss_ctx __maybe_unused = nss_ctx = nss_gre_redir_lag_us_get_context();

	if (!nss_ctx) {
		nss_warning("Unable to retrieve NSS context.\n");
		return false;
	}

	config = (struct nss_gre_redir_lag_us_msg *) kzalloc(sizeof(struct nss_gre_redir_lag_us_msg), GFP_KERNEL);
	if (!config) {
		nss_warning("%px: Unable to allocate memory to send configure message.\n", nss_ctx);
		return false;
	}

	iftype = nss_dynamic_interface_get_type(nss_ctx, ifnum);
	if (iftype != NSS_DYNAMIC_INTERFACE_TYPE_GRE_REDIR_LAG_US) {
		nss_warning("%px: Incorrect interface type %u\n", nss_ctx, iftype);
		kfree(config);
		return false;
	}

	if (!ngluc) {
		nss_warning("%px: Pointer to GRE redir LAG US message is NULL.\n", nss_ctx);
		kfree(config);
		return false;
	}

	if ((ngluc->num_slaves < NSS_GRE_REDIR_LAG_MIN_SLAVE) || (ngluc->num_slaves > NSS_GRE_REDIR_LAG_MAX_SLAVE)) {
		nss_warning("%px: Number of slaves is not in reange\n", nss_ctx);
		kfree(config);
		return false;
	}

	ret = nss_gre_redir_lag_us_sync_work_init(ifnum);
	if (!ret) {
		nss_warning("%px: Unable to initialize work queue\n", nss_ctx);
		kfree(config);
		return false;
	}

	len = sizeof(struct nss_gre_redir_lag_us_msg) - sizeof(struct nss_cmn_msg);
	nss_cmn_msg_init(&config->cm, ifnum, NSS_GRE_REDIR_LAG_US_CONFIG_MSG, len, NULL, NULL);
	config->msg.config_us.hash_mode = ngluc->hash_mode;
	config->msg.config_us.num_slaves = ngluc->num_slaves;
	for (i = 0; i < ngluc->num_slaves; i++) {
		config->msg.config_us.if_num[i] = ngluc->if_num[i];
	}

	status = nss_gre_redir_lag_us_tx_msg_sync(nss_ctx, config);
	kfree(config);
	if (status == NSS_TX_SUCCESS) {
		return true;
	}

	spin_lock_bh(&cmn_ctx.nss_gre_redir_lag_us_stats_lock);
	if (nss_gre_redir_lag_us_get_node_idx(ifnum, &idx)) {
		spin_unlock_bh(&cmn_ctx.nss_gre_redir_lag_us_stats_lock);
		nss_warning("%px: Stats context not found.\n", nss_ctx);
		return false;
	}

	spin_unlock_bh(&cmn_ctx.nss_gre_redir_lag_us_stats_lock);

	/*
	 * Work is per LAG US node. Cancel work as configuration failed.
	 */
	cancel_delayed_work_sync(&(cmn_ctx.stats_ctx[idx].nss_gre_redir_lag_us_work));
	nss_warning("%px: Unable to configure upstream lag node %d.\n", nss_ctx, ifnum);
	return false;
}
EXPORT_SYMBOL(nss_gre_redir_lag_us_configure_node);

/*
 * nss_gre_redir_lag_us_tx_msg()
 *	Transmit a GRE LAG message to NSS firmware asynchronously.
 */
nss_tx_status_t nss_gre_redir_lag_us_tx_msg(struct nss_ctx_instance *nss_ctx, struct nss_gre_redir_lag_us_msg *ngrm)
{
	/*
	 * Trace messages.
	 */
	nss_gre_redir_lag_us_log_tx_msg(ngrm);

	return nss_gre_redir_lag_us_tx_msg_with_size(nss_ctx, ngrm, NSS_NBUF_PAYLOAD_SIZE);
}
EXPORT_SYMBOL(nss_gre_redir_lag_us_tx_msg);

/*
 * nss_gre_redir_lag_us_tx_msg_sync()
 *	Transmit a GRE lag message to NSS firmware synchronously.
 */
nss_tx_status_t nss_gre_redir_lag_us_tx_msg_sync(struct nss_ctx_instance *nss_ctx, struct nss_gre_redir_lag_us_msg *ngrm)
{
	return nss_gre_redir_lag_us_tx_msg_sync_with_size(nss_ctx, ngrm, NSS_NBUF_PAYLOAD_SIZE);
}
EXPORT_SYMBOL(nss_gre_redir_lag_us_tx_msg_sync);

/*
 * nss_gre_redir_lag_us_unregister_and_dealloc()
 *	Unregister and deallocate nss gre redirect LAG US node.
 */
enum nss_gre_redir_lag_err_types nss_gre_redir_lag_us_unregister_and_dealloc(uint32_t ifnum)
{
	uint32_t ret;
	struct nss_ctx_instance *nss_ctx __maybe_unused = nss_gre_redir_lag_us_get_context();
	nss_tx_status_t status;

	if (!nss_gre_redir_lag_us_verify_ifnum(ifnum)) {
		nss_warning("%px: Unknown interface type %u.\n", nss_ctx, ifnum);
		return NSS_GRE_REDIR_LAG_ERR_INCORRECT_IFNUM;
	}

	ret = nss_gre_redir_lag_us_unregister_if(ifnum);
	if (ret) {
		nss_warning("%px: Unable to unregister interface %u.\n", nss_ctx, ifnum);
		return ret;
	}

	status = nss_dynamic_interface_dealloc_node(ifnum, NSS_DYNAMIC_INTERFACE_TYPE_GRE_REDIR_LAG_US);
	if (status != NSS_TX_SUCCESS) {
		nss_warning("%px: Unable to deallocate node %u\n", nss_ctx, ifnum);
		return NSS_GRE_REDIR_LAG_ERR_DEALLOC_FAILED;
	}

	return NSS_GRE_REDIR_LAG_SUCCESS;
}
EXPORT_SYMBOL(nss_gre_redir_lag_us_unregister_and_dealloc);

/*
 * nss_gre_redir_lag_us_alloc_and_register_node()
 *	Allocates and registers GRE upstream type dynamic nodes with NSS.
 */
int nss_gre_redir_lag_us_alloc_and_register_node(struct net_device *dev,
		nss_gre_redir_lag_us_data_callback_t cb_func_data,
		nss_gre_redir_lag_us_msg_callback_t cb_func_msg, void *app_ctx)
{
	int ifnum;
	nss_tx_status_t status;
	struct nss_ctx_instance *nss_ctx;

	ifnum = nss_dynamic_interface_alloc_node(NSS_DYNAMIC_INTERFACE_TYPE_GRE_REDIR_LAG_US);
	if (ifnum == -1) {
		nss_warning("%px: Unable to allocate GRE_LAG node of type = %u\n", dev, NSS_DYNAMIC_INTERFACE_TYPE_GRE_REDIR_LAG_US);
		return -1;
	}

	nss_ctx = nss_gre_redir_lag_us_register_if(ifnum, dev, cb_func_data,
			cb_func_msg, 0, NSS_DYNAMIC_INTERFACE_TYPE_GRE_REDIR_LAG_US, app_ctx);
	if (!nss_ctx) {
		nss_warning("%px: Unable to register GRE_LAG node of type = %u\n", dev, NSS_DYNAMIC_INTERFACE_TYPE_GRE_REDIR_LAG_US);
		status = nss_dynamic_interface_dealloc_node(ifnum, NSS_DYNAMIC_INTERFACE_TYPE_GRE_REDIR_LAG_US);
		if (status != NSS_TX_SUCCESS) {
			nss_warning("%px: Unable to deallocate node.\n", dev);
		}

		return -1;
	}

	return ifnum;
}
EXPORT_SYMBOL(nss_gre_redir_lag_us_alloc_and_register_node);

/*
 * nss_gre_redir_lag_us_register_handler()
 *	Registering handler for sending msg to base gre_lag node on NSS.
 */
void nss_gre_redir_lag_us_register_handler(void)
{
	struct dentry *d_entry = nss_gre_redir_lag_us_stats_dentry_create();

	if (!d_entry) {
		nss_warning(" Unable to create debugfs entry for LAG US node.\n");
		return;
	}

	cmn_ctx.nss_gre_redir_lag_us_wq = create_singlethread_workqueue("nss_gre_redir_lag_us_workqueue");
	if (!cmn_ctx.nss_gre_redir_lag_us_wq) {
		debugfs_remove_recursive(d_entry);
		nss_warning("Unable to create workqueue for LAG US node.\n");
		return;
	}

	nss_gre_redir_lag_us_strings_dentry_create();
	nss_gre_redir_lag_us_sync_ctx.cb = NULL;
	nss_gre_redir_lag_us_sync_ctx.app_data = NULL;
	sema_init(&nss_gre_redir_lag_us_sync_ctx.sem, 1);
	init_completion(&nss_gre_redir_lag_us_sync_ctx.complete);
	spin_lock_init(&cmn_ctx.nss_gre_redir_lag_us_stats_lock);
}

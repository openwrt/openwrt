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
  * nss_capwap.c
  *	NSS CAPWAP driver interface APIs
  */
#include "nss_core.h"
#include "nss_capwap.h"
#include "nss_cmn.h"
#include "nss_tx_rx_common.h"
#include "nss_capwap_stats.h"
#include "nss_capwap_log.h"
#include "nss_capwap_strings.h"

/*
 * Spinlock for protecting tunnel operations colliding with a tunnel destroy
 */
DEFINE_SPINLOCK(nss_capwap_spinlock);

/*
 * Array of pointer for NSS CAPWAP handles. Each handle has per-tunnel
 * stats based on the if_num which is an index.
 *
 * Per CAPWAP tunnel/interface number instance.
 */
struct nss_capwap_handle {
	atomic_t refcnt;			/**< Reference count on the tunnel */
	uint32_t if_num;			/**< Interface number */
	uint32_t tunnel_status;			/**< 0=disable, 1=enabled */
	struct nss_ctx_instance *ctx;		/**< Pointer to context */
	nss_capwap_msg_callback_t msg_callback;	/**< Msg callback */
	void *app_data;				/**< App data (argument) */
	struct nss_capwap_tunnel_stats stats;	/**< Stats per-interface number */
};
static struct nss_capwap_handle *nss_capwap_hdl[NSS_MAX_DYNAMIC_INTERFACES];

/*
 * nss_capwap_get_interface_type()
 *	Function to get the type of dynamic interface.
 */
static enum nss_dynamic_interface_type nss_capwap_get_interface_type(uint32_t if_num)
{
	struct nss_ctx_instance *nss_ctx;
	nss_ctx = &nss_top_main.nss[nss_top_main.capwap_handler_id];
	NSS_VERIFY_CTX_MAGIC(nss_ctx);

	return nss_dynamic_interface_get_type(nss_ctx, if_num);
}

/*
 * nss_capwap_verify_if_num()
 *	Verify if_num passed to us.
 */
static bool nss_capwap_verify_if_num(uint32_t if_num)
{
	enum nss_dynamic_interface_type type;

	if (nss_is_dynamic_interface(if_num) == false) {
		return false;
	}

	type = nss_capwap_get_interface_type(if_num);
	if ((type != NSS_DYNAMIC_INTERFACE_TYPE_CAPWAP_HOST_INNER) && (type != NSS_DYNAMIC_INTERFACE_TYPE_CAPWAP_OUTER) ) {
		return false;
	}

	return true;
}

/*
 * nss_capwap_refcnt_inc()
 *	Increments refcnt on the tunnel.
 */
static void nss_capwap_refcnt_inc(int32_t if_num)
{
	if_num = if_num - NSS_DYNAMIC_IF_START;
	atomic_inc(&nss_capwap_hdl[if_num]->refcnt);
	nss_assert(atomic_read(&nss_capwap_hdl[if_num]->refcnt) > 0);
}

/*
 * nss_capwap_refcnt_dec()
 *	Decrements refcnt on the tunnel.
 */
static void nss_capwap_refcnt_dec(int32_t if_num)
{
	if_num = if_num - NSS_DYNAMIC_IF_START;
	nss_assert(atomic_read(&nss_capwap_hdl[if_num]->refcnt) > 0);
	atomic_dec(&nss_capwap_hdl[if_num]->refcnt);
}

/*
 * nss_capwap_refcnt_get()
 *	Get refcnt on the tunnel.
 */
static uint32_t nss_capwap_refcnt_get(int32_t if_num)
{
	if_num = if_num - NSS_DYNAMIC_IF_START;
	return atomic_read(&nss_capwap_hdl[if_num]->refcnt);
}

/*
 * nss_capwap_set_msg_callback()
 *	This sets the message callback handler and its associated context
 */
static void nss_capwap_set_msg_callback(int32_t if_num, nss_capwap_msg_callback_t cb, void *app_data)
{
	struct nss_capwap_handle *h;

	h = nss_capwap_hdl[if_num - NSS_DYNAMIC_IF_START];
	if (!h) {
		return;
	}

	h->app_data = app_data;
	h->msg_callback = cb;
}

/*
 * nss_capwap_get_msg_callback()
 *	This gets the message callback handler and its associated context
 */
static nss_capwap_msg_callback_t nss_capwap_get_msg_callback(int32_t if_num, void **app_data)
{
	struct nss_capwap_handle *h;

	h = nss_capwap_hdl[if_num - NSS_DYNAMIC_IF_START];
	if (!h) {
		*app_data = NULL;
		return NULL;
	}

	*app_data = h->app_data;
	return h->msg_callback;
}

/*
 * nss_capwap_update_stats()
 *	Update per-tunnel stats for each CAPWAP interface.
 */
static void nss_capwap_update_stats(struct nss_capwap_handle *handle, struct nss_capwap_stats_msg *fstats)
{
	struct nss_capwap_tunnel_stats *stats;
	enum nss_dynamic_interface_type type;

	stats = &handle->stats;
	type = nss_capwap_get_interface_type(handle->if_num);

	switch(type) {
	case NSS_DYNAMIC_INTERFACE_TYPE_CAPWAP_OUTER:
		stats->rx_segments += fstats->rx_segments;
		stats->dtls_pkts += fstats->dtls_pkts;
		stats->rx_dup_frag += fstats->rx_dup_frag;
		stats->rx_oversize_drops += fstats->rx_oversize_drops;
		stats->rx_frag_timeout_drops += fstats->rx_frag_timeout_drops;
		stats->rx_n2h_drops += fstats->rx_n2h_drops;
		stats->rx_n2h_queue_full_drops += fstats->rx_n2h_queue_full_drops;
		stats->rx_mem_failure_drops += fstats->rx_mem_failure_drops;
		stats->rx_csum_drops += fstats->rx_csum_drops;
		stats->rx_malformed += fstats->rx_malformed;
		stats->rx_frag_gap_drops += fstats->rx_frag_gap_drops;

		/*
		 * Update pnode rx stats for OUTER node.
		 */
		stats->pnode_stats.rx_packets += fstats->pnode_stats.rx_packets;
		stats->pnode_stats.rx_bytes += fstats->pnode_stats.rx_bytes;
		stats->pnode_stats.rx_dropped += nss_cmn_rx_dropped_sum(&fstats->pnode_stats);
		break;

	case NSS_DYNAMIC_INTERFACE_TYPE_CAPWAP_HOST_INNER:
		stats->tx_segments += fstats->tx_segments;
		stats->tx_queue_full_drops += fstats->tx_queue_full_drops;
		stats->tx_mem_failure_drops += fstats->tx_mem_failure_drops;
		stats->tx_dropped_sg_ref += fstats->tx_dropped_sg_ref;
		stats->tx_dropped_ver_mis += fstats->tx_dropped_ver_mis;
		stats->tx_dropped_hroom += fstats->tx_dropped_hroom;
		stats->tx_dropped_dtls += fstats->tx_dropped_dtls;
		stats->tx_dropped_nwireless += fstats->tx_dropped_nwireless;

		/*
		 * Update pnode tx stats for INNER node.
		 */
		stats->pnode_stats.tx_packets += fstats->pnode_stats.tx_packets;
		stats->pnode_stats.tx_bytes += fstats->pnode_stats.tx_bytes;
		stats->tx_dropped_inner += nss_cmn_rx_dropped_sum(&fstats->pnode_stats);
		break;

	default:
		nss_warning("%px: Received invalid dynamic interface type: %d", handle, type);
		nss_assert(0);
		return;
	}

	/*
	 * Set to 1 when the tunnel is operating in fast memory.
	 */
	stats->fast_mem = fstats->fast_mem;
}

/*
 * nss_capwap_handler()
 *	Handle NSS -> HLOS messages for CAPWAP
 */
static void nss_capwap_msg_handler(struct nss_ctx_instance *nss_ctx, struct nss_cmn_msg *ncm, __attribute__((unused))void *app_data)
{
	struct nss_capwap_msg *ntm = (struct nss_capwap_msg *)ncm;
	nss_capwap_msg_callback_t cb;

	/*
	 * Is this a valid request/response packet?
	 */
	if (ncm->type > NSS_CAPWAP_MSG_TYPE_MAX) {
		nss_warning("%px: received invalid message %d for CAPWAP interface", nss_ctx, ncm->type);
		return;
	}

	if (nss_cmn_get_msg_len(ncm) > sizeof(struct nss_capwap_msg)) {
		nss_warning("%px: Length of message is greater than required: %d", nss_ctx, nss_cmn_get_msg_len(ncm));
		return;
	}

	nss_core_log_msg_failures(nss_ctx, ncm);

	/*
	 * Trace messages.
	 */
	nss_capwap_log_rx_msg(ntm);

	switch (ntm->cm.type) {
	case NSS_CAPWAP_MSG_TYPE_SYNC_STATS: {
			uint32_t if_num;

			if_num = ncm->interface - NSS_DYNAMIC_IF_START;
			if (nss_capwap_hdl[if_num] != NULL) {
				/*
				 * Update driver statistics and send statistics notifications to the registered modules.
				 */
				nss_capwap_update_stats(nss_capwap_hdl[if_num], &ntm->msg.stats);
				nss_capwap_stats_notify(ncm->interface, nss_ctx->id);
			}
		}
	}

	/*
	 * Update the callback and app_data for NOTIFY messages.
	 */
	if (ncm->response == NSS_CMN_RESPONSE_NOTIFY) {
		ncm->cb = (nss_ptr_t)nss_capwap_get_msg_callback(ncm->interface, (void **)&ncm->app_data);
	}

	/*
	 * Do we have a callback
	 */
	if (!ncm->cb) {
		nss_trace("%px: cb is null for interface %d", nss_ctx, ncm->interface);
		return;
	}

	cb = (nss_capwap_msg_callback_t)ncm->cb;
	cb((void *)ncm->app_data, ntm);
}

/*
 * nss_capwap_instance_alloc()
 *	Allocate CAPWAP tunnel instance
 */
static bool nss_capwap_instance_alloc(struct nss_ctx_instance *nss_ctx, uint32_t if_num)
{
	struct nss_capwap_handle *h;

	/*
	 * Allocate a handle
	 */
	h = kmalloc(sizeof(struct nss_capwap_handle), GFP_ATOMIC);
	if (h == NULL) {
		nss_warning("%px: no memory for allocating CAPWAP instance for interface : %d", nss_ctx, if_num);
		return false;
	}

	memset(h, 0, sizeof(struct nss_capwap_handle));
	h->if_num = if_num;

	spin_lock_bh(&nss_capwap_spinlock);
	if (nss_capwap_hdl[if_num - NSS_DYNAMIC_IF_START] != NULL) {
		spin_unlock_bh(&nss_capwap_spinlock);
		kfree(h);
		nss_warning("%px: Another thread is already allocated instance for :%d", nss_ctx, if_num);
		return false;
	}

	nss_capwap_hdl[if_num - NSS_DYNAMIC_IF_START] = h;
	spin_unlock_bh(&nss_capwap_spinlock);

	return true;
}

/*
 * nss_capwap_tx_msg()
 *	Transmit a CAPWAP message to NSS FW. Don't call this from softirq/interrupts.
 */
nss_tx_status_t nss_capwap_tx_msg(struct nss_ctx_instance *nss_ctx, struct nss_capwap_msg *msg)
{
	struct nss_cmn_msg *ncm = &msg->cm;
	int32_t status;
	int32_t if_num;

	BUG_ON(in_interrupt());
	BUG_ON(in_softirq());
	BUG_ON(in_serving_softirq());

	if (nss_capwap_verify_if_num(msg->cm.interface) == false) {
		return NSS_TX_FAILURE_BAD_PARAM;
	}

	if (ncm->type >= NSS_CAPWAP_MSG_TYPE_MAX) {
		return NSS_TX_FAILURE_BAD_PARAM;
	}

	if_num = msg->cm.interface - NSS_DYNAMIC_IF_START;
	spin_lock_bh(&nss_capwap_spinlock);
	if (!nss_capwap_hdl[if_num]) {
		spin_unlock_bh(&nss_capwap_spinlock);
		nss_warning("%px: capwap tunnel if_num is not there: %d", nss_ctx, msg->cm.interface);
		return NSS_TX_FAILURE_BAD_PARAM;
	}
	nss_capwap_refcnt_inc(msg->cm.interface);
	spin_unlock_bh(&nss_capwap_spinlock);

	/*
	 * Trace messages.
	 */
	nss_capwap_log_tx_msg(msg);

	status = nss_core_send_cmd(nss_ctx, msg, sizeof(*msg), NSS_NBUF_PAYLOAD_SIZE);
	nss_capwap_refcnt_dec(msg->cm.interface);
	return status;
}
EXPORT_SYMBOL(nss_capwap_tx_msg);

/*
 * nss_capwap_tx_buf()
 *	Transmit data buffer (skb) to a NSS interface number
 */
nss_tx_status_t nss_capwap_tx_buf(struct nss_ctx_instance *nss_ctx, struct sk_buff *os_buf, uint32_t if_num)
{
	BUG_ON(!nss_capwap_verify_if_num(if_num));

	return nss_core_send_packet(nss_ctx, os_buf, if_num, H2N_BIT_FLAG_VIRTUAL_BUFFER | H2N_BIT_FLAG_BUFFER_REUSABLE);
}
EXPORT_SYMBOL(nss_capwap_tx_buf);

/*
 ***********************************
 * Register/Unregister/Miscellaneous APIs
 ***********************************
 */

/*
 * nss_capwap_get_stats()
 *	API for getting stats from a CAPWAP tunnel interface stats
 */
bool nss_capwap_get_stats(uint32_t if_num, struct nss_capwap_tunnel_stats *stats)
{
	if (nss_capwap_verify_if_num(if_num) == false) {
		return false;
	}

	if_num = if_num - NSS_DYNAMIC_IF_START;
	spin_lock_bh(&nss_capwap_spinlock);
	if (nss_capwap_hdl[if_num] == NULL) {
		spin_unlock_bh(&nss_capwap_spinlock);
		return false;
	}

	memcpy(stats, &nss_capwap_hdl[if_num]->stats, sizeof(struct nss_capwap_tunnel_stats));
	spin_unlock_bh(&nss_capwap_spinlock);
	return true;
}
EXPORT_SYMBOL(nss_capwap_get_stats);

/*
 * nss_capwap_notify_register()
 *	Registers a message notifier with NSS FW. It should not be called from
 *	softirq or interrupts.
 */
struct nss_ctx_instance *nss_capwap_notify_register(uint32_t if_num, nss_capwap_msg_callback_t cb, void *app_data)
{
	struct nss_ctx_instance *nss_ctx;

	nss_ctx = &nss_top_main.nss[nss_top_main.capwap_handler_id];

	if (nss_capwap_verify_if_num(if_num) == false) {
		nss_warning("%px: notfiy register received for invalid interface %d", nss_ctx, if_num);
		return NULL;
	}

	spin_lock_bh(&nss_capwap_spinlock);
	if (nss_capwap_hdl[if_num - NSS_DYNAMIC_IF_START] != NULL) {
		spin_unlock_bh(&nss_capwap_spinlock);
		nss_warning("%px: notfiy register tunnel already exists for interface %d", nss_ctx, if_num);
		return NULL;
	}
	spin_unlock_bh(&nss_capwap_spinlock);

	return nss_ctx;
}
EXPORT_SYMBOL(nss_capwap_notify_register);

/*
 * nss_capwap_notify_unregister()
 *	unregister the CAPWAP notifier for the given interface number (if_num).
 *	It shouldn't be called from softirq or interrupts.
 */
nss_tx_status_t nss_capwap_notify_unregister(struct nss_ctx_instance *nss_ctx, uint32_t if_num)
{
	struct nss_top_instance *nss_top;
	int index;

	if (nss_capwap_verify_if_num(if_num) == false) {
		nss_warning("%px: notify unregister received for invalid interface %d", nss_ctx, if_num);
		return NSS_TX_FAILURE_BAD_PARAM;
	}

	nss_top = nss_ctx->nss_top;
	if (nss_top == NULL) {
		nss_warning("%px: notify unregister received for invalid nss_top %d", nss_ctx, if_num);
		return NSS_TX_FAILURE_BAD_PARAM;
	}

	index = if_num - NSS_DYNAMIC_IF_START;
	spin_lock_bh(&nss_capwap_spinlock);
	if (nss_capwap_hdl[index] == NULL) {
		spin_unlock_bh(&nss_capwap_spinlock);
		nss_warning("%px: notify unregister received for unallocated if_num: %d", nss_ctx, if_num);
		return NSS_TX_FAILURE_BAD_PARAM;
	}

	/*
	 * It's the responsibility of caller to wait and call us again. We return failure saying
	 * that we can't remove msg handler now.
	 */
	if (nss_capwap_refcnt_get(if_num) != 0) {
		spin_unlock_bh(&nss_capwap_spinlock);
		nss_warning("%px: notify unregister tunnel %d: has reference", nss_ctx, if_num);
		return NSS_TX_FAILURE_QUEUE;
	}

	nss_capwap_set_msg_callback(if_num, NULL, NULL);
	spin_unlock_bh(&nss_capwap_spinlock);

	return NSS_TX_SUCCESS;
}
EXPORT_SYMBOL(nss_capwap_notify_unregister);

/*
 * nss_capwap_data_register()
 *	Registers a data packet notifier with NSS FW.
 */
struct nss_ctx_instance *nss_capwap_data_register(uint32_t if_num, nss_capwap_buf_callback_t cb, struct net_device *netdev, uint32_t features)
{
	struct nss_ctx_instance *nss_ctx;
	int core_status;

	nss_ctx = nss_capwap_get_ctx();
	if (nss_capwap_verify_if_num(if_num) == false) {
		nss_warning("%px: data register received for invalid interface %d", nss_ctx, if_num);
		return NULL;
	}

	spin_lock_bh(&nss_capwap_spinlock);
	if (nss_ctx->subsys_dp_register[if_num].ndev != NULL) {
		spin_unlock_bh(&nss_capwap_spinlock);
		return NULL;
	}
	spin_unlock_bh(&nss_capwap_spinlock);

	core_status = nss_core_register_handler(nss_ctx, if_num, nss_capwap_msg_handler, NULL);
	if (core_status != NSS_CORE_STATUS_SUCCESS) {
		nss_warning("%px: nss core register handler failed for if_num:%d with error :%d", nss_ctx, if_num, core_status);
		return NULL;
	}

	if (nss_capwap_instance_alloc(nss_ctx, if_num) == false) {
		nss_warning("%px: couldn't allocate tunnel  instance for if_num:%d", nss_ctx, if_num);
		return NULL;
	}

	nss_core_register_subsys_dp(nss_ctx, if_num, cb, NULL, NULL, netdev, features);

	return nss_ctx;
}
EXPORT_SYMBOL(nss_capwap_data_register);

/*
 * nss_capwap_data_unregister()
 *	Unregister a data packet notifier with NSS FW
 */
bool nss_capwap_data_unregister(uint32_t if_num)
{
	struct nss_ctx_instance *nss_ctx;
	struct nss_capwap_handle *h;

	nss_ctx = nss_capwap_get_ctx();
	if (nss_capwap_verify_if_num(if_num) == false) {
		nss_warning("%px: data unregister received for invalid interface %d", nss_ctx, if_num);
		return false;
	}

	spin_lock_bh(&nss_capwap_spinlock);
	/*
	 * It's the responsibility of caller to wait and call us again.
	 */
	if (nss_capwap_refcnt_get(if_num) != 0) {
		spin_unlock_bh(&nss_capwap_spinlock);
		nss_warning("%px: notify unregister tunnel %d: has reference", nss_ctx, if_num);
		return false;
	}
	h = nss_capwap_hdl[if_num - NSS_DYNAMIC_IF_START];
	nss_capwap_hdl[if_num - NSS_DYNAMIC_IF_START] = NULL;
	spin_unlock_bh(&nss_capwap_spinlock);

	(void) nss_core_unregister_handler(nss_ctx, if_num);

	nss_core_unregister_subsys_dp(nss_ctx, if_num);

	kfree(h);
	return true;
}
EXPORT_SYMBOL(nss_capwap_data_unregister);

/*
 * nss_capwap_get_ctx()
 *	Return a CAPWAP NSS context.
 */
struct nss_ctx_instance *nss_capwap_get_ctx()
{
	struct nss_ctx_instance *nss_ctx;

	nss_ctx = &nss_top_main.nss[nss_top_main.capwap_handler_id];
	return nss_ctx;
}
EXPORT_SYMBOL(nss_capwap_get_ctx);

/*
 * nss_capwap_ifnum_with_core_id()
 *	Append core id to capwap interface num
 */
int nss_capwap_ifnum_with_core_id(int if_num)
{
	struct nss_ctx_instance *nss_ctx = nss_capwap_get_ctx();

	NSS_VERIFY_CTX_MAGIC(nss_ctx);
	if (nss_is_dynamic_interface(if_num) == false) {
		nss_info("%px: Invalid if_num: %d, must be a dynamic interface\n", nss_ctx, if_num);
		return 0;
	}
	return NSS_INTERFACE_NUM_APPEND_COREID(nss_ctx, if_num);
}
EXPORT_SYMBOL(nss_capwap_ifnum_with_core_id);

/*
 * nss_capwap_get_max_buf_size()
 *	Return a CAPWAP NSS max_buf_size.
 */
uint32_t nss_capwap_get_max_buf_size(struct nss_ctx_instance *nss_ctx)
{
	return nss_core_get_max_buf_size(nss_ctx);
}
EXPORT_SYMBOL(nss_capwap_get_max_buf_size);

/*
 * nss_capwap_init()
 *	Initializes CAPWAP. Gets called from nss_init.c
 */
void nss_capwap_init()
{
	memset(&nss_capwap_hdl, 0, sizeof(nss_capwap_hdl));
	nss_capwap_stats_dentry_create();
	nss_capwap_strings_dentry_create();
}

/*
 * nss_capwap_msg_init()
 *	Initialize capwap message.
 */
void nss_capwap_msg_init(struct nss_capwap_msg *ncm, uint16_t if_num, uint32_t type, uint32_t len,
				nss_capwap_msg_callback_t cb, void *app_data)
{
	nss_cmn_msg_init(&ncm->cm, if_num, type, len, (void*)cb, app_data);
}
EXPORT_SYMBOL(nss_capwap_msg_init);

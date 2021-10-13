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
#include "nss_dtls_stats.h"
#include "nss_dtls_log.h"

#define NSS_DTLS_TX_TIMEOUT 3000 /* 3 Seconds */

/*
 * Data structures to store DTLS nss debug stats
 */
static DEFINE_SPINLOCK(nss_dtls_session_stats_lock);
static struct nss_dtls_stats_session session_stats[NSS_MAX_DTLS_SESSIONS];

/*
 * Private data structure
 */
static struct nss_dtls_pvt {
	struct semaphore sem;
	struct completion complete;
	int response;
	void *cb;
	void *app_data;
} dtls_pvt;

/*
 * nss_dtls_verify_if_num()
 *	Verify if_num passed to us.
 */
static bool nss_dtls_verify_if_num(uint32_t if_num)
{
	if (nss_is_dynamic_interface(if_num) == false)
		return false;

	if (nss_dynamic_interface_get_type(nss_dtls_get_context(), if_num)
	    != NSS_DYNAMIC_INTERFACE_TYPE_DTLS)
		return false;

	return true;
}

/*
 * nss_dtls_session_stats_sync
 *	Per DTLS session debug stats
 */
static void nss_dtls_session_stats_sync(struct nss_ctx_instance *nss_ctx,
					struct nss_dtls_session_stats *stats_msg,
					uint16_t if_num)
{
	int i;
	struct nss_dtls_stats_session *s = NULL;

	NSS_VERIFY_CTX_MAGIC(nss_ctx);

	spin_lock_bh(&nss_dtls_session_stats_lock);
	for (i = 0; i < NSS_MAX_DTLS_SESSIONS; i++) {
		if (session_stats[i].if_num != if_num) {
			continue;
		}

		s = &session_stats[i];
		break;
	}

	if (!s) {
		spin_unlock_bh(&nss_dtls_session_stats_lock);
		return;
	}

	s->stats[NSS_DTLS_STATS_SESSION_RX_PKTS] += stats_msg->node_stats.rx_packets;
	s->stats[NSS_DTLS_STATS_SESSION_TX_PKTS] += stats_msg->node_stats.tx_packets;
	for (i = 0; i < NSS_MAX_NUM_PRI; i++) {
		s->stats[NSS_DTLS_STATS_SESSION_RX_QUEUE_0_DROPPED + i] += stats_msg->node_stats.rx_dropped[i];
	}
	s->stats[NSS_DTLS_STATS_SESSION_RX_AUTH_DONE] += stats_msg->rx_auth_done;
	s->stats[NSS_DTLS_STATS_SESSION_TX_AUTH_DONE] += stats_msg->tx_auth_done;
	s->stats[NSS_DTLS_STATS_SESSION_RX_CIPHER_DONE] += stats_msg->rx_cipher_done;
	s->stats[NSS_DTLS_STATS_SESSION_TX_CIPHER_DONE] += stats_msg->tx_cipher_done;
	s->stats[NSS_DTLS_STATS_SESSION_RX_CBUF_ALLOC_FAIL] += stats_msg->rx_cbuf_alloc_fail;
	s->stats[NSS_DTLS_STATS_SESSION_TX_CBUF_ALLOC_FAIL] += stats_msg->tx_cbuf_alloc_fail;
	s->stats[NSS_DTLS_STATS_SESSION_TX_CENQUEUE_FAIL] += stats_msg->tx_cenqueue_fail;
	s->stats[NSS_DTLS_STATS_SESSION_RX_CENQUEUE_FAIL] += stats_msg->rx_cenqueue_fail;
	s->stats[NSS_DTLS_STATS_SESSION_TX_DROPPED_HROOM] += stats_msg->tx_dropped_hroom;
	s->stats[NSS_DTLS_STATS_SESSION_TX_DROPPED_TROOM] += stats_msg->tx_dropped_troom;
	s->stats[NSS_DTLS_STATS_SESSION_TX_FORWARD_ENQUEUE_FAIL] += stats_msg->tx_forward_enqueue_fail;
	s->stats[NSS_DTLS_STATS_SESSION_RX_FORWARD_ENQUEUE_FAIL] += stats_msg->rx_forward_enqueue_fail;
	s->stats[NSS_DTLS_STATS_SESSION_RX_INVALID_VERSION] += stats_msg->rx_invalid_version;
	s->stats[NSS_DTLS_STATS_SESSION_RX_INVALID_EPOCH] += stats_msg->rx_invalid_epoch;
	s->stats[NSS_DTLS_STATS_SESSION_RX_MALFORMED] += stats_msg->rx_malformed;
	s->stats[NSS_DTLS_STATS_SESSION_RX_CIPHER_FAIL] += stats_msg->rx_cipher_fail;
	s->stats[NSS_DTLS_STATS_SESSION_RX_AUTH_FAIL] += stats_msg->rx_auth_fail;
	s->stats[NSS_DTLS_STATS_SESSION_RX_CAPWAP_CLASSIFY_FAIL] += stats_msg->rx_capwap_classify_fail;
	s->stats[NSS_DTLS_STATS_SESSION_RX_SINGLE_REC_DGRAM] += stats_msg->rx_single_rec_dgram;
	s->stats[NSS_DTLS_STATS_SESSION_RX_MULTI_REC_DGRAM] += stats_msg->rx_multi_rec_dgram;
	s->stats[NSS_DTLS_STATS_SESSION_RX_REPLAY_FAIL] += stats_msg->rx_replay_fail;
	s->stats[NSS_DTLS_STATS_SESSION_RX_REPLAY_DUPLICATE] += stats_msg->rx_replay_duplicate;
	s->stats[NSS_DTLS_STATS_SESSION_RX_REPLAY_OUT_OF_WINDOW] += stats_msg->rx_replay_out_of_window;
	s->stats[NSS_DTLS_STATS_SESSION_OUTFLOW_QUEUE_FULL] += stats_msg->outflow_queue_full;
	s->stats[NSS_DTLS_STATS_SESSION_DECAP_QUEUE_FULL] += stats_msg->decap_queue_full;
	s->stats[NSS_DTLS_STATS_SESSION_PBUF_ALLOC_FAIL] += stats_msg->pbuf_alloc_fail;
	s->stats[NSS_DTLS_STATS_SESSION_PBUF_COPY_FAIL] += stats_msg->pbuf_copy_fail;
	s->stats[NSS_DTLS_STATS_SESSION_EPOCH] = stats_msg->epoch;
	s->stats[NSS_DTLS_STATS_SESSION_TX_SEQ_HIGH] = stats_msg->tx_seq_high;
	s->stats[NSS_DTLS_STATS_SESSION_TX_SEQ_LOW] = stats_msg->tx_seq_low;
	spin_unlock_bh(&nss_dtls_session_stats_lock);
}

/*
 * nss_dtls_session_stats_get()
 *	Get session DTLS statitics.
 */
void nss_dtls_session_stats_get(struct nss_dtls_stats_session *stats)
{
	int i;

	if (!stats) {
		nss_warning("No memory to copy dtls session stats");
		return;
	}

	spin_lock_bh(&nss_dtls_session_stats_lock);
	for (i = 0; i < NSS_MAX_DTLS_SESSIONS; i++) {
		if (session_stats[i].valid) {
			memcpy(stats, &session_stats[i],
			       sizeof(struct nss_dtls_stats_session));
			stats++;
		}
	}
	spin_unlock_bh(&nss_dtls_session_stats_lock);
}

/*
 * nss_dtls_handler()
 *	Handle NSS -> HLOS messages for dtls tunnel
 */
static void nss_dtls_handler(struct nss_ctx_instance *nss_ctx,
			     struct nss_cmn_msg *ncm,
			     __attribute__((unused))void *app_data)
{
	struct nss_dtls_msg *ntm = (struct nss_dtls_msg *)ncm;
	void *ctx;

	nss_dtls_msg_callback_t cb;

	NSS_VERIFY_CTX_MAGIC(nss_ctx);
	BUG_ON(!nss_dtls_verify_if_num(ncm->interface));

	/*
	 * Is this a valid request/response packet?
	 */
	if (ncm->type >= NSS_DTLS_MSG_MAX) {
		nss_warning("%px: received invalid message %d "
			    "for DTLS interface %d",
			    nss_ctx, ncm->type, ncm->interface);
		return;
	}

	if (nss_cmn_get_msg_len(ncm) > sizeof(struct nss_dtls_msg)) {
		nss_warning("%px: dtls message length is invalid: %d",
			    nss_ctx, ncm->len);
		return;
	}

	switch (ntm->cm.type) {
	case NSS_DTLS_MSG_SESSION_STATS:
		nss_dtls_session_stats_sync(nss_ctx,
					    &ntm->msg.stats,
					    ncm->interface);
		break;
	}

	/*
	 * Update the callback and app_data for NOTIFY messages
	 */
	if (ncm->response == NSS_CMN_RESPONSE_NOTIFY) {
		ncm->cb = (nss_ptr_t)nss_ctx->nss_top->dtls_msg_callback;
		ncm->app_data = (nss_ptr_t)nss_ctx->subsys_dp_register[ncm->interface].app_data;
	}

	/*
	 * Log failures
	 */
	nss_core_log_msg_failures(nss_ctx, ncm);

	/*
	 * Trace messages.
	 */
	nss_dtls_log_rx_msg(ntm);

	/*
	 * callback
	 */
	cb = (nss_dtls_msg_callback_t)ncm->cb;
	ctx = (void *)ncm->app_data;

	/*
	 * call dtls session callback
	 */
	if (!cb) {
		nss_warning("%px: No callback for dtls session interface %d",
			    nss_ctx, ncm->interface);
		return;
	}

	cb(ctx, ntm);
}

/*
 * nss_dtls_callback()
 *	Callback to handle the completion of NSS->HLOS messages.
 */
static void nss_dtls_callback(void *app_data, struct nss_dtls_msg *nim)
{
	nss_dtls_msg_callback_t callback = (nss_dtls_msg_callback_t)dtls_pvt.cb;
	void *data = dtls_pvt.app_data;

	dtls_pvt.cb = NULL;
	dtls_pvt.app_data = NULL;

	if (nim->cm.response != NSS_CMN_RESPONSE_ACK) {
		nss_warning("dtls Error response %d\n", nim->cm.response);

		dtls_pvt.response = NSS_TX_FAILURE;
		if (callback) {
			callback(data, nim);
		}

		complete(&dtls_pvt.complete);
		return;
	}

	dtls_pvt.response = NSS_TX_SUCCESS;
	if (callback) {
		callback(data, nim);
	}

	complete(&dtls_pvt.complete);
}

/*
 * nss_dtls_tx_buf()
 *	Transmit buffer over DTLS interface
 */
nss_tx_status_t nss_dtls_tx_buf(struct sk_buff *skb, uint32_t if_num,
				struct nss_ctx_instance *nss_ctx)
{
	BUG_ON(!nss_dtls_verify_if_num(if_num));

	return nss_core_send_packet(nss_ctx, skb, if_num, H2N_BIT_FLAG_VIRTUAL_BUFFER | H2N_BIT_FLAG_BUFFER_REUSABLE);
}
EXPORT_SYMBOL(nss_dtls_tx_buf);

/*
 * nss_dtls_tx_msg()
 *	Transmit a DTLS message to NSS firmware
 */
nss_tx_status_t nss_dtls_tx_msg(struct nss_ctx_instance *nss_ctx,
				struct nss_dtls_msg *msg)
{
	struct nss_cmn_msg *ncm = &msg->cm;

	/*
	 * Sanity check the message
	 */
	BUG_ON(!nss_dtls_verify_if_num(ncm->interface));

	if (ncm->type > NSS_DTLS_MSG_MAX) {
		nss_warning("%px: dtls message type out of range: %d",
			    nss_ctx, ncm->type);
		return NSS_TX_FAILURE;
	}

	/*
	 * Trace messages.
	 */
	nss_dtls_log_tx_msg(msg);

	return nss_core_send_cmd(nss_ctx, msg, sizeof(*msg), NSS_NBUF_PAYLOAD_SIZE);
}
EXPORT_SYMBOL(nss_dtls_tx_msg);

/*
 * nss_dtls_tx_msg()
 *	Transmit a DTLS message to NSS firmware synchronously.
 */
nss_tx_status_t nss_dtls_tx_msg_sync(struct nss_ctx_instance *nss_ctx, struct nss_dtls_msg *msg)
{

	nss_tx_status_t status;
	int ret = 0;

	down(&dtls_pvt.sem);
	dtls_pvt.cb = (void *)msg->cm.cb;
	dtls_pvt.app_data = (void *)msg->cm.app_data;

	msg->cm.cb = (nss_ptr_t)nss_dtls_callback;
	msg->cm.app_data = (nss_ptr_t)NULL;

	status = nss_dtls_tx_msg(nss_ctx, msg);
	if (status != NSS_TX_SUCCESS) {
		nss_warning("%px: dtls_tx_msg failed\n", nss_ctx);
		up(&dtls_pvt.sem);
		return status;
	}

	ret = wait_for_completion_timeout(&dtls_pvt.complete, msecs_to_jiffies(NSS_DTLS_TX_TIMEOUT));

	if (!ret) {
		nss_warning("%px: DTLS msg tx failed due to timeout\n", nss_ctx);
		dtls_pvt.response = NSS_TX_FAILURE;
	}

	status = dtls_pvt.response;
	up(&dtls_pvt.sem);
	return status;
}
EXPORT_SYMBOL(nss_dtls_tx_msg_sync);

/*
 ***********************************
 * Register/Unregister/Miscellaneous APIs
 ***********************************
 */

/*
 * nss_dtls_register_if()
 */
struct nss_ctx_instance *nss_dtls_register_if(uint32_t if_num,
					      nss_dtls_data_callback_t cb,
					      nss_dtls_msg_callback_t ev_cb,
					      struct net_device *netdev,
					      uint32_t features,
					      void *app_ctx)
{
	int32_t i;

	struct nss_ctx_instance *nss_ctx = nss_dtls_get_context();

	BUG_ON(!nss_dtls_verify_if_num(if_num));

	spin_lock_bh(&nss_dtls_session_stats_lock);
	for (i = 0; i < NSS_MAX_DTLS_SESSIONS; i++) {
		if (!session_stats[i].valid) {
			session_stats[i].valid = true;
			session_stats[i].if_num = if_num;
			session_stats[i].if_index = netdev->ifindex;
			break;
		}
	}
	spin_unlock_bh(&nss_dtls_session_stats_lock);

	if (i == NSS_MAX_DTLS_SESSIONS) {
		nss_warning("%px: Cannot find free slot for "
			    "DTLS session stats, I/F:%u\n", nss_ctx, if_num);
		return NULL;
	}

	if (nss_ctx->subsys_dp_register[if_num].ndev) {
		nss_warning("%px: Cannot find free slot for "
			    "DTLS NSS I/F:%u\n", nss_ctx, if_num);

		return NULL;
	}

	nss_core_register_subsys_dp(nss_ctx, if_num, cb, NULL, app_ctx, netdev, features);
	nss_ctx->subsys_dp_register[if_num].type = NSS_DYNAMIC_INTERFACE_TYPE_DTLS;

	nss_top_main.dtls_msg_callback = ev_cb;
	nss_core_register_handler(nss_ctx, if_num, nss_dtls_handler, app_ctx);

	return nss_ctx;
}
EXPORT_SYMBOL(nss_dtls_register_if);

/*
 * nss_dtls_unregister_if()
 */
void nss_dtls_unregister_if(uint32_t if_num)
{
	struct nss_ctx_instance *nss_ctx = nss_dtls_get_context();
	int32_t i;

	BUG_ON(!nss_dtls_verify_if_num(if_num));

	spin_lock_bh(&nss_dtls_session_stats_lock);
	for (i = 0; i < NSS_MAX_DTLS_SESSIONS; i++) {
		if (session_stats[i].if_num == if_num) {
			memset(&session_stats[i], 0,
			       sizeof(struct nss_dtls_stats_session));
			break;
		}
	}
	spin_unlock_bh(&nss_dtls_session_stats_lock);

	if (i == NSS_MAX_DTLS_SESSIONS) {
		nss_warning("%px: Cannot find debug stats for DTLS session %d\n", nss_ctx, if_num);
		return;
	}

	if (!nss_ctx->subsys_dp_register[if_num].ndev) {
		nss_warning("%px: Cannot find registered netdev for DTLS NSS I/F:%u\n", nss_ctx, if_num);

		return;
	}

	nss_core_unregister_subsys_dp(nss_ctx, if_num);

	nss_top_main.dtls_msg_callback = NULL;
	nss_core_unregister_handler(nss_ctx, if_num);
}
EXPORT_SYMBOL(nss_dtls_unregister_if);

/*
 * nss_get_dtls_context()
 */
struct nss_ctx_instance *nss_dtls_get_context(void)
{
	return (struct nss_ctx_instance *)&nss_top_main.nss[nss_top_main.dtls_handler_id];
}
EXPORT_SYMBOL(nss_dtls_get_context);

/*
 * nss_dtls_msg_init()
 *	Initialize nss_dtls msg.
 */
void nss_dtls_msg_init(struct nss_dtls_msg *ncm, uint16_t if_num,
		       uint32_t type, uint32_t len, void *cb, void *app_data)
{
	nss_cmn_msg_init(&ncm->cm, if_num, type, len, cb, app_data);
}
EXPORT_SYMBOL(nss_dtls_msg_init);

/*
 * nss_dtls_get_ifnum_with_coreid()
 */
int32_t nss_dtls_get_ifnum_with_coreid(int32_t if_num)
{
	struct nss_ctx_instance *nss_ctx = nss_dtls_get_context();

	NSS_VERIFY_CTX_MAGIC(nss_ctx);
	return NSS_INTERFACE_NUM_APPEND_COREID(nss_ctx, if_num);
}
EXPORT_SYMBOL(nss_dtls_get_ifnum_with_coreid);

/*
 * nss_dtls_register_handler()
 */
void nss_dtls_register_handler(void)
{
	sema_init(&dtls_pvt.sem, 1);
	init_completion(&dtls_pvt.complete);

	nss_dtls_stats_dentry_create();
}

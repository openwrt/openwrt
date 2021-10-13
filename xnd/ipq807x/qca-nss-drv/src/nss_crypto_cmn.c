/*
 **************************************************************************
 * Copyright (c) 2013,2015-2020, The Linux Foundation. All rights reserved.
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
 * nss_crypto_cmn.c
 *	NSS Crypto common API implementation
 */

#include "nss_tx_rx_common.h"
#include "nss_crypto_cmn.h"
#include "nss_crypto_cmn_strings.h"
#include "nss_crypto_cmn_stats.h"
#include "nss_crypto_cmn_log.h"

/*
 * Amount time the synchronous message should wait for response from
 * NSS before the timeout happens. After the timeout the message
 * response even if it arrives has to be discarded. Typically, the
 * time needs to be selected based on the worst case time in case of
 * peak throughput between host & NSS.
 */
#define NSS_CRYPTO_CMN_TX_TIMEO_TICKS msecs_to_jiffies(3000) /* milliseconds */

/*
 * Private data structure to hold state for
 * the crypto specific NSS interaction
 */
struct nss_crypto_cmn_pvt {
	struct semaphore sem;		/* used for synchronizing 'tx_msg_sync' */
	struct completion complete;	/* completion callback */
	atomic_t seq_no;		/* used for tracking tx_msg_sync requests */
};

/*
 * This is a single instance applicable for all crypto synchronous
 * messaging interaction with NSS.
 */
static struct nss_crypto_cmn_pvt g_nss_crypto_cmn;

/*
 * nss_crypto_cmn_msg_handler()
 *	this handles all the IPsec events and responses
 */
static void nss_crypto_cmn_msg_handler(struct nss_ctx_instance *nss_ctx, struct nss_cmn_msg *ncm,
					void *app_data __attribute((unused)))
{
	struct nss_crypto_cmn_msg *nim = (struct nss_crypto_cmn_msg *)ncm;
	nss_crypto_cmn_msg_callback_t cb = NULL;

	/*
	 * Sanity check the message type
	 */
	if (ncm->type > NSS_CRYPTO_CMN_MSG_TYPE_MAX) {
		nss_warning("%px: rx message type out of range: %d", nss_ctx, ncm->type);
		return;
	}

	/*
	 * Check if the message structure length matches that of Host side. In case
	 * of failure this indicates ether the structure is different or this is not
	 * the intended interface.
	 */
	if (nss_cmn_get_msg_len(ncm) > sizeof(*nim)) {
		nss_warning("%px: rx message length is invalid: %d", nss_ctx, nss_cmn_get_msg_len(ncm));
		return;
	}

	if (ncm->response == NSS_CMN_RESPONSE_LAST) {
		nss_warning("%px: rx message response for if %d, type %d, is invalid: %d", nss_ctx,
				ncm->interface, ncm->type, ncm->response);
		return;
	}

	if (ncm->response == NSS_CMN_RESPONSE_NOTIFY) {
		ncm->cb = (nss_ptr_t)nss_ctx->nss_top->crypto_cmn_msg_callback;
		ncm->app_data = (nss_ptr_t)nss_ctx->nss_top->crypto_ctx;
	}

	nss_core_log_msg_failures(nss_ctx, ncm);

	/*
	 * Trace messages.
	 */
	nss_crypto_cmn_log_rx_msg(nim);

	switch (nim->cm.type) {
	case NSS_CRYPTO_CMN_MSG_TYPE_SYNC_NODE_STATS:
	case NSS_CRYPTO_CMN_MSG_TYPE_SYNC_ENG_STATS:
	case NSS_CRYPTO_CMN_MSG_TYPE_SYNC_CTX_STATS:
		/*
		 * Update driver statistics and send statistics
		 * notification to the registered modules.
		 */
		nss_crypto_cmn_stats_sync(nss_ctx, &nim->msg.stats);
		nss_crypto_cmn_stats_notify(nss_ctx);
		break;
	}
	/*
	 * Load, Test & call
	 */
	cb = (nss_crypto_cmn_msg_callback_t)ncm->cb;
	if (unlikely(!cb)) {
		nss_warning("%px: rx handler has been unregistered for i/f: %d", nss_ctx, ncm->interface);
		return;
	}

	cb((void *)ncm->app_data, nim);
}

/*
 * nss_crypto_cmn_tx_msg
 *	Send crypto config to NSS.
 */
nss_tx_status_t nss_crypto_cmn_tx_msg(struct nss_ctx_instance *nss_ctx, struct nss_crypto_cmn_msg *msg)
{
	struct nss_cmn_msg *ncm = &msg->cm;
	uint16_t msg_len = nss_cmn_get_msg_len(ncm);

	nss_info("%px: tx message %d for if %d", nss_ctx, ncm->type, ncm->interface);

	BUILD_BUG_ON(NSS_NBUF_PAYLOAD_SIZE < sizeof(*msg));

	if (ncm->type > NSS_CRYPTO_CMN_MSG_TYPE_MAX) {
		nss_warning("%px: message type out of range: %d", nss_ctx, ncm->type);
		return NSS_TX_FAILURE;
	}

	/*
	 * Check if the message structure length matches the structure length. Otherwise
	 * the sender accidentally programmed a incorrect length into the message.
	 */
	if (msg_len != sizeof(*msg)) {
		nss_warning("%px: message request len bad: %d", nss_ctx, msg_len);
		return NSS_TX_FAILURE_BAD_PARAM;
	}

	nss_trace("%px: msg params version:%d, interface:%d, type:%d, cb:%px, app_data:%px, len:%d",
			nss_ctx, ncm->version, ncm->interface, ncm->type,
			(void *)ncm->cb, (void *)ncm->app_data, ncm->len);

	/*
	 * Trace messages.
	 */
	nss_crypto_cmn_log_tx_msg(msg);

	return nss_core_send_cmd(nss_ctx, msg, sizeof(*msg), NSS_NBUF_PAYLOAD_SIZE);
}
EXPORT_SYMBOL(nss_crypto_cmn_tx_msg);

/*
 * nss_crypto_cmn_tx_msg_cb()
 *	Callback to handle the synchronous completion of messages.
 */
static void nss_crypto_cmn_tx_msg_cb(void *app_data, struct nss_crypto_cmn_msg *nim)
{
	struct nss_crypto_cmn_pvt *pvt = &g_nss_crypto_cmn;
	struct nss_crypto_cmn_msg *resp = (struct nss_crypto_cmn_msg *)nim->cm.app_data;

	/*
	 * Only update the message structure if the sequence no. matches
	 * Otherwise, a timeout might have happened in between and we
	 * are probably receiving the completion for an older message
	 */
	if (atomic_read(&pvt->seq_no) == nim->seq_num) {
		memcpy(resp, nim, sizeof(struct nss_crypto_cmn_msg));
		complete(&pvt->complete);
	}
}

/*
 * nss_crypto_cmn_tx_msg_sync()
 *	Transmit a crypto message to NSS firmware synchronously.
 */
nss_tx_status_t nss_crypto_cmn_tx_msg_sync(struct nss_ctx_instance *nss_ctx, struct nss_crypto_cmn_msg *msg)
{
	struct nss_crypto_cmn_pvt *pvt = &g_nss_crypto_cmn;
	nss_tx_status_t status;
	int ret = 0;

	down(&pvt->sem);
	atomic_inc(&pvt->seq_no);

	/*
	 * this is a synchronous message; overload the callback
	 * and app_data
	 */
	msg->cm.cb = (nss_ptr_t)nss_crypto_cmn_tx_msg_cb;
	msg->cm.app_data = (nss_ptr_t)msg;
	msg->seq_num = atomic_read(&pvt->seq_no);

	status = nss_crypto_cmn_tx_msg(nss_ctx, msg);
	if (status != NSS_TX_SUCCESS) {
		nss_warning("%px: tx_msg failed", nss_ctx);
		up(&pvt->sem);
		return status;
	}

	/*
	 * Note: This cannot be called in atomic context
	 */
	ret = wait_for_completion_timeout(&pvt->complete, NSS_CRYPTO_CMN_TX_TIMEO_TICKS);
	if (!ret) {
		atomic_inc(&pvt->seq_no);
		nss_warning("%px: tx_msg_sync timed out", nss_ctx);
		up(&pvt->sem);
		return NSS_TX_FAILURE;
	}

	/*
	 * This ensures that the even if the response arrives on a different
	 * CPU core the data copied by the response callback will be visible
	 * to the caller which is sleeping for it on a different core. For
	 * further details read Linux/Documentation/memory-barrier.txt
	 */
	smp_rmb();
	up(&pvt->sem);

	return NSS_TX_SUCCESS;
}
EXPORT_SYMBOL(nss_crypto_cmn_tx_msg_sync);

/*
 * nss_crypto_cmn_tx_buf()
 *	NSS crypto TX data API. Sends a crypto buffer to NSS.
 */
nss_tx_status_t nss_crypto_cmn_tx_buf(struct nss_ctx_instance *nss_ctx, uint32_t if_num,
					struct sk_buff *skb)
{
	int32_t status;

	NSS_VERIFY_CTX_MAGIC(nss_ctx);
	if (unlikely(nss_ctx->state != NSS_CORE_STATE_INITIALIZED)) {
		nss_warning("%px: tx_data packet dropped as core not ready", nss_ctx);
		return NSS_TX_FAILURE_NOT_READY;
	}

	status = nss_core_send_packet(nss_ctx, skb, if_num, H2N_BIT_FLAG_BUFFER_REUSABLE);
	switch (status) {
	case NSS_CORE_STATUS_SUCCESS:
		break;

	case NSS_CORE_STATUS_FAILURE_QUEUE: /* queue full condition */
		nss_warning("%px: H2N queue full for tx_buf", nss_ctx);
		return NSS_TX_FAILURE_QUEUE;

	default:
		nss_warning("%px: general failure for tx_buf", nss_ctx);
		return NSS_TX_FAILURE;
	}

	/*
	 * Kick the NSS awake so it can process our new entry.
	 */
	nss_hal_send_interrupt(nss_ctx, NSS_H2N_INTR_DATA_COMMAND_QUEUE);
	NSS_PKT_STATS_INC(&nss_ctx->nss_top->stats_drv[NSS_DRV_STATS_TX_CRYPTO_REQ]);

	return NSS_TX_SUCCESS;
}
EXPORT_SYMBOL(nss_crypto_cmn_tx_buf);

/*
 * nss_crypto_cmn_notify_register()
 *	register message notifier for crypto interface
 */
struct nss_ctx_instance *nss_crypto_cmn_notify_register(nss_crypto_cmn_msg_callback_t cb, void *app_data)
{
	struct nss_top_instance *nss_top = &nss_top_main;
	struct nss_ctx_instance *nss_ctx = &nss_top->nss[nss_top->crypto_handler_id];

	nss_top->crypto_ctx = app_data;
	nss_top->crypto_cmn_msg_callback = cb;

	return nss_ctx;
}
EXPORT_SYMBOL(nss_crypto_cmn_notify_register);

/*
 * nss_crypto_cmn_notify_unregister()
 *	De-register the message notifier for crypto interface
 */
void nss_crypto_cmn_notify_unregister(struct nss_ctx_instance *nss_ctx)
{
	struct nss_top_instance *nss_top = &nss_top_main;

	nss_top->crypto_ctx = NULL;
	nss_top->crypto_cmn_msg_callback = NULL;
}
EXPORT_SYMBOL(nss_crypto_cmn_notify_unregister);

/*
 * nss_crypto_cmn_data_register()
 *	Register the data callback routine
 */
struct nss_ctx_instance *nss_crypto_cmn_data_register(uint32_t if_num, nss_crypto_cmn_buf_callback_t cb,
		struct net_device *netdev, uint32_t features)
{
	struct nss_ctx_instance *nss_ctx;

	nss_ctx = &nss_top_main.nss[nss_top_main.crypto_handler_id];

	if (if_num < NSS_SPECIAL_IF_START) {
		nss_warning("%px: interface number is not special interface %d", nss_ctx, if_num);
		return NULL;
	}

	/*
	 * avoid multiple registration for same interface number
	 */
	if (nss_ctx->subsys_dp_register[if_num].cb)
		return nss_ctx;

	/*
	 * Note: no locking is required for updating this as
	 * the registration is only a module load time operation.
	 */
	nss_core_register_subsys_dp(nss_ctx, if_num, cb, NULL, NULL, netdev, features);

	return nss_ctx;
}
EXPORT_SYMBOL(nss_crypto_cmn_data_register);

/*
 * nss_crypto_cmn_data_unregister()
 *	De-register the data callback routine
 */
void nss_crypto_cmn_data_unregister(struct nss_ctx_instance *nss_ctx, uint32_t if_num)
{
	if (if_num < NSS_SPECIAL_IF_START) {
		nss_warning("%px: interface number is not special interface %d", nss_ctx, if_num);
		return;
	}

	/*
	 * Note: no locking is required for updating this as
	 * the registration is only a module load time operation.
	 */
	nss_core_unregister_subsys_dp(nss_ctx, if_num);
}
EXPORT_SYMBOL(nss_crypto_cmn_data_unregister);

/*
 * nss_crypto_cmn_get_context()
 *	get NSS context instance for crypto handle
 */
struct nss_ctx_instance *nss_crypto_cmn_get_context(void)
{
	return (struct nss_ctx_instance *)&nss_top_main.nss[nss_top_main.crypto_handler_id];
}
EXPORT_SYMBOL(nss_crypto_cmn_get_context);

/*
 * nss_crypto_cmn_register_handler()
 */
void nss_crypto_cmn_register_handler(void)
{
	struct nss_ctx_instance *nss_ctx = nss_crypto_cmn_get_context();

	sema_init(&g_nss_crypto_cmn.sem, 1);
	init_completion(&g_nss_crypto_cmn.complete);
	nss_core_register_handler(nss_ctx, NSS_CRYPTO_CMN_INTERFACE, nss_crypto_cmn_msg_handler, NULL);

	nss_crypto_cmn_stats_dentry_create();
	nss_crypto_cmn_strings_dentry_create();
}

/*
 * nss_crypto_cmn_msg_init()
 *	Initialize crypto message
 */
void nss_crypto_cmn_msg_init(struct nss_crypto_cmn_msg *ncm, uint16_t if_num, uint32_t type,
			uint32_t len, nss_crypto_cmn_msg_callback_t cb, void *app_data)
{
	nss_cmn_msg_init(&ncm->cm, if_num, type, len, (void *)cb, app_data);
}
EXPORT_SYMBOL(nss_crypto_cmn_msg_init);

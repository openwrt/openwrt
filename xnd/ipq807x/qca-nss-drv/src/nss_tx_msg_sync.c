/*
 * Copyright (c) 2018-2020, The Linux Foundation. All rights reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * nss_tx_msg_sync.c
 *	NSS Tx msg sync core APIs
 */

#include "nss_tx_rx_common.h"

/*
 * nss_tx_msg_sync_callback()
 *	Internal callback used to handle the message response.
 */
static void nss_tx_msg_sync_callback(void *app_data, struct nss_cmn_msg *ncm)
{
	uint32_t resp_offset;

	/*
	 * Per-message sync data was used as app_data.
	 * Retrieve the address of the original message from it.
	 */
	struct nss_tx_msg_sync_cmn_data *sync_data = (struct nss_tx_msg_sync_cmn_data *)app_data;
	struct nss_cmn_msg *original_msg = (struct nss_cmn_msg *)sync_data->original_msg;

	/*
	 * Set TX status. And Copy back ncm->error and ncm->response if it is NACK.
	 */
	sync_data->status = NSS_TX_SUCCESS;
	if (ncm->response != NSS_CMN_RESPONSE_ACK) {
		nss_warning("Tx msg sync error response %d\n", ncm->response);
		sync_data->status = NSS_TX_FAILURE_SYNC_FW_ERR;
		original_msg->error = ncm->error;
		original_msg->response = ncm->response;
	}

	/*
	 * ncm is the return message containing message response.
	 * It is different from the original message caller built.
	 * Because the return message is only visible in this callback context,
	 * we copy back message response by specifying offset and length to
	 * the return message. So the caller can use response in their context
	 * once wake up instead of calling a passed-in user callback here.
	 */
	resp_offset = sync_data->resp_offset + sizeof(struct nss_cmn_msg);

	if (sync_data->copy_len > 0)
		memcpy((uint8_t *)((nss_ptr_t)original_msg + resp_offset),
			(uint8_t *)((nss_ptr_t)ncm + resp_offset),
			sync_data->copy_len);

	/*
	 * Wake up the caller
	 */
	complete(&sync_data->complete);
}

/*
 * nss_tx_msg_sync_internal()
 *	Internal call for sending messages to FW synchronously.
 */
static nss_tx_status_t nss_tx_msg_sync_internal(struct nss_ctx_instance *nss_ctx,
						nss_tx_msg_sync_subsys_async_t tx_msg_async,
						nss_tx_msg_sync_subsys_async_with_size_t tx_msg_async_with_size,
						uint32_t msg_buf_size,
						struct nss_tx_msg_sync_cmn_data *sync_data,
						struct nss_cmn_msg *ncm,
						uint32_t timeout)
{
	nss_tx_status_t status;
	int ret;

	/*
	 * Per-msg sync data is used as app_data.
	 * A generic callback is used to handle the return message.
	 */
	ncm->cb = (nss_ptr_t)nss_tx_msg_sync_callback;
	ncm->app_data = (nss_ptr_t)sync_data;

	BUG_ON(!tx_msg_async && !tx_msg_async_with_size);

	/*
	 * Per-subsystem asynchronous call to send down the message.
	 */
	if (tx_msg_async)
		status = tx_msg_async(nss_ctx, ncm);
	else
		status = tx_msg_async_with_size(nss_ctx, ncm, msg_buf_size);

	if (status != NSS_TX_SUCCESS) {
		nss_warning("%px: Tx msg async failed\n", nss_ctx);
		return status;
	}

	/*
	 * Sleep. Wake up either by notification or timeout.
	 */
	ret = wait_for_completion_timeout(&sync_data->complete, msecs_to_jiffies(timeout));
	if (!ret) {
		nss_warning("%px: Tx msg sync timeout\n", nss_ctx);
		return NSS_TX_FAILURE_SYNC_TIMEOUT;
	}

	/*
	 * Wake up. Message response has been received within timeout.
	 */
	return sync_data->status;
}

/*
 * nss_tx_msg_sync()
 *	Send messages to FW synchronously with default message buffer size.
 *
 * tx_msg_async specifies the per-subsystem asynchronous call.
 * timeout specifies the maximum sleep time for the completion.
 * ncm is the original message the caller built.
 * Since the caller cannot access the return message containing message response,
 * we copy back message response from return message.
 * resp_offset and copy_len specify the part of return message it'll copy.
 */
nss_tx_status_t nss_tx_msg_sync(struct nss_ctx_instance *nss_ctx,
				nss_tx_msg_sync_subsys_async_t tx_msg_async,
				uint32_t timeout, struct nss_cmn_msg *ncm,
				uint32_t resp_offset, uint32_t copy_len)
{
	struct nss_tx_msg_sync_cmn_data sync_data;

	NSS_VERIFY_CTX_MAGIC(nss_ctx);

	/*
	 * Check Tx msg async API
	 */
	if (!unlikely(tx_msg_async)) {
		nss_warning("%px: missing Tx msg async API\n", nss_ctx);
		return NSS_TX_FAILURE_SYNC_BAD_PARAM;
	}

	/*
	 * Initialize the per-message sync data.
	 */
	init_completion(&sync_data.complete);
	sync_data.status = NSS_TX_FAILURE;
	sync_data.original_msg = (void *)ncm;
	sync_data.resp_offset = resp_offset;
	sync_data.copy_len = copy_len;

	return nss_tx_msg_sync_internal(nss_ctx, tx_msg_async, NULL, 0, &sync_data, ncm, timeout);
}
EXPORT_SYMBOL(nss_tx_msg_sync);

/*
 * nss_tx_msg_sync_with_size()
 *	Send messages to FW synchronously with specified message buffer size.
 */
nss_tx_status_t nss_tx_msg_sync_with_size(struct nss_ctx_instance *nss_ctx,
				nss_tx_msg_sync_subsys_async_with_size_t tx_msg_async_with_size,
				uint32_t msg_buf_size, uint32_t timeout,
				struct nss_cmn_msg *ncm, uint32_t resp_offset, uint32_t copy_len)
{
	struct nss_tx_msg_sync_cmn_data sync_data;

	NSS_VERIFY_CTX_MAGIC(nss_ctx);

	/*
	 * Check Tx msg async API
	 */
	if (!unlikely(tx_msg_async_with_size)) {
		nss_warning("%px: missing Tx msg async API\n", nss_ctx);
		return NSS_TX_FAILURE_SYNC_BAD_PARAM;
	}

	/*
	 * Initialize the per-message sync data.
	 */
	init_completion(&sync_data.complete);
	sync_data.status = NSS_TX_FAILURE;
	sync_data.original_msg = (void *)ncm;
	sync_data.resp_offset = resp_offset;
	sync_data.copy_len = copy_len;

	return nss_tx_msg_sync_internal(nss_ctx, NULL, tx_msg_async_with_size,
					msg_buf_size, &sync_data, ncm, timeout);
}
EXPORT_SYMBOL(nss_tx_msg_sync_with_size);

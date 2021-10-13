/*
 * Copyright (c) 2018-2019, The Linux Foundation. All rights reserved.
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
 * nss_tx_msg_sync.h
 *	NSS Tx msg sync header file
 */

#ifndef __NSS_TX_MSG_SYNC_H
#define __NSS_TX_MSG_SYNC_H

#include <nss_core.h>

/*
 * Amount time in msec the synchronous message should wait for response
 * from NSS before the timeout happens.
 */
#define NSS_TX_MSG_SYNC_DEFAULT_TIMEOUT_MSEC	(5000)

/*
 * Per-message sync data
 *	Used as message app_data.
 */
struct nss_tx_msg_sync_cmn_data {
	struct completion complete;		/* Completion structure */
	nss_tx_status_t status;			/* Tx status */
	void *original_msg;			/* Address of the caller-build message */
	uint32_t resp_offset;			/* Response offset in message payload */
	uint32_t copy_len;			/* Length in bytes copied from the return message */
};

/*
 * nss_tx_msg_sync_subsys_async_t()
 *	Tx msg asynchronous API of each subsystem.
 */
typedef nss_tx_status_t (*nss_tx_msg_sync_subsys_async_t)(struct nss_ctx_instance *nss_ctx, struct nss_cmn_msg *ncm);

/*
 * nss_tx_msg_sync_subsys_async_with_size_t()
 *	Tx msg asynchronous API of each subsystem with message buffer size specified.
 */
typedef nss_tx_status_t (*nss_tx_msg_sync_subsys_async_with_size_t)(struct nss_ctx_instance *nss_ctx, struct nss_cmn_msg *ncm, uint32_t size);

/*
 * nss_tx_msg_sync()
 *	Core function to send message to FW synchronously.
 *
 * tx_msg_async specifies the per-subsystem asynchronous call.
 * timeout specifies the maximum sleep time for the completion.
 * ncm is the original message the caller built.
 * Since the caller cannot access the return message containing message response,
 * we copy back message response from the return message.
 * resp_offset and copy_len specify the part of return message it'll copy.
 */
nss_tx_status_t nss_tx_msg_sync(struct nss_ctx_instance *nss_ctx,
				nss_tx_msg_sync_subsys_async_t tx_msg_async,
				uint32_t timeout, struct nss_cmn_msg *ncm,
				uint32_t resp_offset, uint32_t copy_len);

/*
 * nss_tx_msg_sync_with_size()
 *	Send messages to FW synchronously with specified message buffer size.
 */
nss_tx_status_t nss_tx_msg_sync_with_size(struct nss_ctx_instance *nss_ctx,
				nss_tx_msg_sync_subsys_async_with_size_t tx_msg_async_with_size,
				uint32_t msg_buf_size, uint32_t timeout,
				struct nss_cmn_msg *ncm, uint32_t resp_offset, uint32_t copy_len);

#endif /* __NSS_TX_MSG_SYNC_H */

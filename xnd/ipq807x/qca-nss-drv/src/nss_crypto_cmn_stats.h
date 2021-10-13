/*
 ******************************************************************************
 * Copyright (c) 2020, The Linux Foundation. All rights reserved.
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
 * ****************************************************************************
 */

#ifndef __NSS_CRYPTO_CMN_STATS_H
#define __NSS_CRYPTO_CMN_STATS_H

#include <nss_cmn.h>

/**
 * nss_crypto_cmn_stats_types
 *	crypto common transmission node statistics
 */
enum nss_crypto_cmn_stats_types {
	NSS_CRYPTO_CMN_STATS_FAIL_VERSION = NSS_STATS_NODE_MAX,	/* version mismatch failures */
	NSS_CRYPTO_CMN_STATS_FAIL_CTX,				/* context related failures */
	NSS_CRYPTO_CMN_STATS_FAIL_DMA,				/* dma descriptor full */
	NSS_CRYPTO_CMN_STATS_MAX,				/* Maximum message type */
};

/**
 * nss_crypto_cmn_stats_notification
 *	crypto common transmission statistics structure
 */
struct nss_crypto_cmn_stats_notification {
	uint32_t core_id;				/* core ID */
	uint64_t stats[NSS_CRYPTO_CMN_STATS_MAX];	/* transmission statistics */
};

/*
 * crypto common statistics APIs
 */
extern void nss_crypto_cmn_stats_notify(struct nss_ctx_instance *nss_ctx);
extern void nss_crypto_cmn_stats_sync(struct nss_ctx_instance *nss_ctx, struct nss_crypto_cmn_stats *nct);
extern void nss_crypto_cmn_stats_dentry_create(void);

/**
 * nss_crypto_cmn_stats_register_notifier
 *	Registers a statistics notifier.
 *
 * @datatypes
 * notifier_block
 *
 * @param[in] nb Notifier block.
 *
 * @return
 * 0 on success or -2 on failure.
 */
extern int nss_crypto_cmn_stats_register_notifier(struct notifier_block *nb);

/**
 * nss_crypto_cmn_stats_unregister_notifier
 *	Deregisters a statistics notifier.
 *
 * @datatypes
 * notifier_block
 *
 * @param[in] nb Notifier block.
 *
 * @return
 * 0 on success or -2 on failure.
 */
extern int nss_crypto_cmn_stats_unregister_notifier(struct notifier_block *nb);

#endif /* __NSS_CRYPTO_CMN_STATS_H */

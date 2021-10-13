/*
 **************************************************************************
 * Copyright (c) 2017, The Linux Foundation. All rights reserved.
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
 * nss_trustsec_tx_stats.h
 *      NSS TRUSTSEC TX statistics header file.
 */

#ifndef __NSS_TRUSTSEC_TX_STATS_H
#define __NSS_TRUSTSEC_TX_STATS_H

/*
 * Trustsec TX statistics
 */
enum nss_trustsec_tx_stats {
	NSS_TRUSTSEC_TX_STATS_INVALID_SRC,
					/* Number of packets with invalid src if */
	NSS_TRUSTSEC_TX_STATS_UNCONFIGURED_SRC,
					/* Number of packets with unconfigured src if */
	NSS_TRUSTSEC_TX_STATS_HEADROOM_NOT_ENOUGH,
					/* Number of packets with not enough headroom */
	NSS_TRUSTSEC_TX_STATS_MAX
};

/*
 * Trustsec TX statistics APIs
 */
extern void nss_trustsec_tx_stats_sync(struct nss_ctx_instance *nss_ctx, struct nss_trustsec_tx_stats_sync_msg *ntsm);
extern void nss_trustsec_tx_stats_dentry_create(void);

#endif /* __NSS_TRUSTSEC_TX_STATS_H */

/*
 **************************************************************************
 * Copyright (c) 2019-2020, The Linux Foundation. All rights reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 **************************************************************************
 */

#ifndef __NSS_WIFILI_STRINGS_H
#define __NSS_WIFILI_STRINGS_H

/*
 * Maximum string length:
 * This should be equal to maximum string size of any stats
 * inclusive of stats value
 */
#define NSS_WIFILI_STATS_MAX    (NSS_WIFILI_STATS_TXRX_MAX + NSS_WIFILI_STATS_TCL_MAX + \
                                NSS_WIFILI_STATS_TX_DESC_FREE_MAX + NSS_WIFILI_STATS_REO_MAX + \
                                NSS_WIFILI_STATS_TX_DESC_MAX + NSS_WIFILI_STATS_EXT_TX_DESC_MAX + \
                                NSS_WIFILI_STATS_RX_DESC_MAX + NSS_WIFILI_STATS_RXDMA_DESC_MAX)

extern struct nss_stats_info nss_wifili_strings_stats_txrx[NSS_WIFILI_STATS_TXRX_MAX];
extern struct nss_stats_info nss_wifili_strings_stats_tcl[NSS_WIFILI_STATS_TCL_MAX];
extern struct nss_stats_info nss_wifili_strings_stats_tx_comp[NSS_WIFILI_STATS_TX_DESC_FREE_MAX];
extern struct nss_stats_info nss_wifili_strings_stats_reo[NSS_WIFILI_STATS_REO_MAX];
extern struct nss_stats_info nss_wifili_strings_stats_txsw_pool[NSS_WIFILI_STATS_TX_DESC_MAX];
extern struct nss_stats_info nss_wifili_strings_stats_ext_txsw_pool[NSS_WIFILI_STATS_EXT_TX_DESC_MAX];
extern struct nss_stats_info nss_wifili_strings_stats_rxdma_pool[NSS_WIFILI_STATS_RX_DESC_MAX];
extern struct nss_stats_info nss_wifili_strings_stats_rxdma_ring[NSS_WIFILI_STATS_RXDMA_DESC_MAX];
extern struct nss_stats_info nss_wifili_strings_stats_wbm[NSS_WIFILI_STATS_WBM_MAX];

extern void nss_wifili_strings_dentry_create(void);

#endif /* __NSS_WIFILI_STRINGS_H */

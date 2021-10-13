/*
 **************************************************************************
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
 **************************************************************************
 */

#ifndef __NSS_EDMA_STRINGS_H
#define __NSS_EDMA_STRINGS_H

extern struct nss_stats_info nss_edma_strings_stats_node[NSS_STATS_NODE_MAX];
extern struct nss_stats_info nss_edma_strings_stats_tx[NSS_EDMA_STATS_TX_MAX];
extern struct nss_stats_info nss_edma_strings_stats_rx[NSS_EDMA_STATS_RX_MAX];
extern struct nss_stats_info nss_edma_strings_stats_txcmpl[NSS_EDMA_STATS_TXCMPL_MAX];
extern struct nss_stats_info nss_edma_strings_stats_rxfill[NSS_EDMA_STATS_RXFILL_MAX];
extern struct nss_stats_info nss_edma_strings_stats_port_type[NSS_EDMA_PORT_TYPE_MAX];
extern struct nss_stats_info nss_edma_strings_stats_port_ring_map[NSS_EDMA_PORT_RING_MAP_MAX];
extern struct nss_stats_info nss_edma_strings_stats_err_map[NSS_EDMA_ERR_STATS_MAX];
extern void nss_edma_strings_dentry_create(void);

#endif /* __NSS_EDMA_STRINGS_H */

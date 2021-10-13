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

/*
 * nss_lso_rx.h
 *	NSS driver LSO (Large Send Offload) Rx header file.
 */

#ifndef __NSS_LSO_RX_H
#define __NSS_LSO_RX_H

/**
 * @addtogroup nss_lso_rx_subsystem
 * @{
 */

/**
 * nss_lso_rx_stats_types
 *	LSO Rx driver statistics.
 */
enum nss_lso_rx_stats_types {
	NSS_LSO_RX_STATS_TX_DROPPED,		/**< Number of packets dropped because transmit queue is full. */
	NSS_LSO_RX_STATS_DROPPED,		/**< Number of packets dropped because of node internal errors. */
	NSS_LSO_RX_STATS_PBUF_ALLOC_FAIL,	/**< Number of pbuf allocation failures. */
	NSS_LSO_RX_STATS_PBUF_REFERENCE_FAIL,	/**< Number of pbuf reference failures. */
	NSS_LSO_RX_STATS_MAX,			/**< Maximum message type. */
};

/**
 * nss_lso_rx_stats_notification
 *	Data for sending LSO Rx statistics.
 */
struct nss_lso_rx_stats_notification {
	uint32_t core_id;				/**< Core ID. */
	uint64_t cmn_node_stats[NSS_STATS_NODE_MAX];	/**< Common node statistics. */
	uint64_t node_stats[NSS_LSO_RX_STATS_MAX];	/**< LSO Rx special statistics. */
};

#ifdef __KERNEL__ /* only kernel will use. */
/**
 * nss_lso_rx_stats_register_notifier
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
extern int nss_lso_rx_stats_register_notifier(struct notifier_block *nb);

/**
 * nss_lso_rx_stats_unregister_notifier
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
extern int nss_lso_rx_stats_unregister_notifier(struct notifier_block *nb);
#endif /*__KERNEL__ */

/**
 * @}
 */

#endif /* __NSS_LSO_RX_H */

/*
 **************************************************************************
 * Copyright (c) 2020, The Linux Foundation. All rights reserved.
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

/**
 * @file nss_eth_rx.h
 *	NSS Ethernet interface definitions.
 */

#ifndef __NSS_ETH_RX_H
#define __NSS_ETH_RX_H

/**
 * @addtogroup nss_eth_rx_subsystem
 * @{
 */

/**
 * nss_eth_rx_stats
 *	Ethernet node statistics.
 */
enum nss_eth_rx_stats {
	NSS_ETH_RX_STATS_TOTAL_TICKS,		/**< Total clock ticks spent inside the Ethernet package. */
	NSS_ETH_RX_STATS_WORST_CASE_TICKS,	/**< Worst case iteration of the Ethernet in ticks. */
	NSS_ETH_RX_STATS_ITERATIONS,		/**< Number of iterations around Ethernet. */
	NSS_ETH_RX_STATS_MAX,			/**< Maximum message type. */
};

/**
 * nss_eth_rx_exception_events
 *	Exception events from bridge or route handler.
 */
enum nss_eth_rx_exception_events {
	NSS_ETH_RX_EXCEPTION_EVENT_UNKNOWN_L3_PROTOCOL,
	NSS_ETH_RX_EXCEPTION_EVENT_ETH_HDR_MISSING,
	NSS_ETH_RX_EXCEPTION_EVENT_VLAN_MISSING,
	NSS_ETH_RX_EXCEPTION_EVENT_TRUSTSEC_HDR_MISSING,
	NSS_ETH_RX_EXCEPTION_EVENT_MAX,
};

/**
 * nss_eth_rx_stats_notification
 *	Data for sending Ethernet statistics.
 */
struct nss_eth_rx_stats_notification {
	uint32_t core_id;						/**< Core ID. */
	uint64_t cmn_node_stats[NSS_STATS_NODE_MAX];			/**< Node statistics. */
	uint64_t special_stats[NSS_ETH_RX_STATS_MAX];			/**< Special statistics. */
	uint64_t exception_stats[NSS_ETH_RX_EXCEPTION_EVENT_MAX];	/**< Exception statistics. */
};

#ifdef __KERNEL__ /* only kernel will use. */
/**
 * nss_eth_rx_stats_register_notifier
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
extern int nss_eth_rx_stats_register_notifier(struct notifier_block *nb);

/**
 * nss_eth_rx_stats_unregister_notifier
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
extern int nss_eth_rx_stats_unregister_notifier(struct notifier_block *nb);
#endif /*__KERNEL__ */

/**
 *@}
 */

#endif /* __NSS_ETH_RX_H */

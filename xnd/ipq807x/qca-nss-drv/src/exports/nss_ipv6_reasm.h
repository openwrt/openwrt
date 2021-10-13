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

/**
 * @file nss_ipv6_reasm.h
 *	NSS IPv6 reassembly interface definitions.
 */

#ifndef __NSS_IPV6_REASM_H
#define __NSS_IPV6_REASM_H

/**
 * @addtogroup nss_ipv6_reasm_subsystem
 * @{
 */

/**
 * nss_ipv6_reasm_stats
 *	IPv6 reassembly node statistics.
 */
enum nss_ipv6_reasm_stats {
	NSS_IPV6_REASM_STATS_ALLOC_FAILS,
				/**< Number of fragment queue allocation failures. */
	NSS_IPV6_REASM_STATS_TIMEOUTS,
				/**< Number of expired fragment queues. */
	NSS_IPV6_REASM_STATS_DISCARDS,
				/**< Number of fragment queues discarded due to malformed fragments. */
	NSS_IPV6_REASM_STATS_MAX,
				/**< Maximum message type. */
};

/**
 * nss_ipv6_reasm_stats_notification
 *	Data for sending IPv6 reassembly statistics.
 */
struct nss_ipv6_reasm_stats_notification {
	uint32_t core_id;					/**< Core ID. */
	uint64_t cmn_node_stats[NSS_STATS_NODE_MAX];		/**< Common node statistics. */
	uint64_t ipv6_reasm_stats[NSS_IPV6_REASM_STATS_MAX];	/**< IPv6 reassembly statistics. */
};

#ifdef __KERNEL__

/**
 * nss_ipv6_reasm_stats_register_notifier
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
extern int nss_ipv6_reasm_stats_register_notifier(struct notifier_block *nb);

/**
 * nss_ipv6_reasm_stats_unregister_notifier
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
extern int nss_ipv6_reasm_stats_unregister_notifier(struct notifier_block *nb);
#endif

/**
 * @}
 */

#endif /* __NSS_IPV6_REASM_H */

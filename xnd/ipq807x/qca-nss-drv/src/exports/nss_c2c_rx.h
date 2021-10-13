/*
 **************************************************************************
 * Copyright (c) 2019-2020, The Linux Foundation. All rights reserved.
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

/**
 * @file nss_c2c_rx.h
 *	NSS core-to-core reception interface definitions.
 */

#ifndef __NSS_C2C_RX_H
#define __NSS_C2C_RX_H

/**
 * @addtogroup nss_c2c_rx_subsystem
 * @{
 */

/**
 * nss_c2c_rx_stats_types
 *	Core-to-core reception node statistics.
 */
enum nss_c2c_rx_stats_types {
	NSS_C2C_RX_STATS_PBUF_SIMPLE = NSS_STATS_NODE_MAX,
						/**< Number of received simple pbufs. */
	NSS_C2C_RX_STATS_PBUF_SG,		/**< Number of scatter-gather pbufs received. */
	NSS_C2C_RX_STATS_PBUF_RETURNING,	/**< Number of returning scatter-gather pbufs. */
	NSS_C2C_RX_STATS_INVAL_DEST,		/**< Number of pbuf enqueue failures because of destination is invalid. */
	NSS_C2C_RX_STATS_MAX,			/**< Maximum message type. */
};

/**
 * nss_c2c_rx_stats_notification
 *	Core-to-core reception statistics structure.
 */
struct nss_c2c_rx_stats_notification {
	uint32_t core_id;			/**< Core ID. */
	uint64_t stats[NSS_C2C_RX_STATS_MAX];	/**< Core-to-core reception statistics. */
};

#ifdef __KERNEL__ /* only kernel will use. */
/**
 * nss_c2c_rx_stats_register_notifier
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
extern int nss_c2c_rx_stats_register_notifier(struct notifier_block *nb);

/**
 * nss_c2c_rx_stats_unregister_notifier
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
extern int nss_c2c_rx_stats_unregister_notifier(struct notifier_block *nb);
#endif /*__KERNEL__ */

/**
 * @}
 */

#endif /* __NSS_C2C_RX_H */

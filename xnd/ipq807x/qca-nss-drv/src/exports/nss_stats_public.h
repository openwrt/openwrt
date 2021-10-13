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
 * @file nss_stats_public.h
 *	NSS statistics Structure and APIs
 */

#ifndef __NSS_STATS_PUBLIC_H
#define __NSS_STATS_PUBLIC_H

/**
 * @addtogroup nss_stats_public_subsystem
 * @{
 */

/**
 * Maximum string length.
 *
 * This should be equal to maximum string size of any statistics
 * inclusive of statistics value.
 */
#define NSS_STATS_MAX_STR_LENGTH 96

/**
 * nss_stats_node
 *	Node statistics.
 */
enum nss_stats_node {
	NSS_STATS_NODE_RX_PKTS,			/**< Accelerated node Rx packets. */
	NSS_STATS_NODE_RX_BYTES,		/**< Accelerated node Rx bytes. */
	NSS_STATS_NODE_TX_PKTS,			/**< Accelerated node Tx packets. */
	NSS_STATS_NODE_TX_BYTES,		/**< Accelerated node Tx bytes. */
	NSS_STATS_NODE_RX_QUEUE_0_DROPPED,	/**< Accelerated node Rx Queue 0 dropped. */
	NSS_STATS_NODE_RX_QUEUE_1_DROPPED,	/**< Accelerated node Rx Queue 1 dropped. */
	NSS_STATS_NODE_RX_QUEUE_2_DROPPED,	/**< Accelerated node Rx Queue 2 dropped. */
	NSS_STATS_NODE_RX_QUEUE_3_DROPPED,	/**< Accelerated node Rx Queue 3 dropped. */
	NSS_STATS_NODE_MAX,			/**< Maximum message type. */
};

/*
 * WARNING: There is a 1:1 mapping between values of enum nss_stats_drv and corresponding
 * statistics string array in nss_drv_strings.c.
 */
/**
 * nss_stats_drv
 *	HLOS driver statistics.
 */
enum nss_stats_drv {
	NSS_STATS_DRV_NBUF_ALLOC_FAILS = 0,	/**< Networking buffer allocation errors. */
	NSS_STATS_DRV_PAGED_BUF_ALLOC_FAILS,	/**< Paged buffer allocation errors. */
	NSS_STATS_DRV_TX_QUEUE_FULL_0,		/**< Tx queue full for Core 0. */
	NSS_STATS_DRV_TX_QUEUE_FULL_1,		/**< Tx queue full for Core 1. */
	NSS_STATS_DRV_TX_EMPTY,			/**< Host-to-network empty buffers. */
	NSS_STATS_DRV_PAGED_TX_EMPTY,		/**< Host-to-network paged empty buffers. */
	NSS_STATS_DRV_TX_PACKET,		/**< Host-to-network data packets. */
	NSS_STATS_DRV_TX_CMD_REQ,		/**< Host-to-network control packets. */
	NSS_STATS_DRV_TX_CRYPTO_REQ,		/**< Host-to-network crypto requests. */
	NSS_STATS_DRV_TX_BUFFER_REUSE,		/**< Host-to-network reuse buffer count. */
	NSS_STATS_DRV_RX_EMPTY,			/**< Network-to-host empty buffers. */
	NSS_STATS_DRV_RX_PACKET,		/**< Network-to-host data packets. */
	NSS_STATS_DRV_RX_CMD_RESP,		/**< Network-to-host command responses. */
	NSS_STATS_DRV_RX_STATUS,		/**< Network-to-host status packets. */
	NSS_STATS_DRV_RX_CRYPTO_RESP,		/**< Network-to-host crypto responses. */
	NSS_STATS_DRV_RX_VIRTUAL,		/**< Network-to-host virtual packets. */
	NSS_STATS_DRV_TX_SIMPLE,		/**< Host-to-network simple SKB packets. */
	NSS_STATS_DRV_TX_NR_FRAGS,		/**< Host-to-network number of fragmented SKB packets. */
	NSS_STATS_DRV_TX_FRAGLIST,		/**< Host-to-network fragmentation list of SKB packets. */
	NSS_STATS_DRV_RX_SIMPLE,		/**< Network-to-host simple SKB packets. */
	NSS_STATS_DRV_RX_NR_FRAGS,		/**< Network-to-host number of fragmented SKB packets. */
	NSS_STATS_DRV_RX_SKB_FRAGLIST,		/**< Network-to-host fragmentation list of SKB packets. */
	NSS_STATS_DRV_RX_BAD_DESCRIPTOR,	/**< Network-to-host bad descriptor reads. */
	NSS_STATS_DRV_NSS_SKB_COUNT,		/**< NSS SKB pool count. */
	NSS_STATS_DRV_CHAIN_SEG_PROCESSED,	/**< Network-to-host SKB chain processed count. */
	NSS_STATS_DRV_FRAG_SEG_PROCESSED,	/**< Network-to-host fragments processed count. */
	NSS_STATS_DRV_TX_CMD_QUEUE_FULL,	/**< Tx host-to-network control packets fail due to queue full. */
#ifdef NSS_MULTI_H2N_DATA_RING_SUPPORT
	NSS_STATS_DRV_TX_PACKET_QUEUE_0,	/**< Host-to-network data packets on queue0. */
	NSS_STATS_DRV_TX_PACKET_QUEUE_1,	/**< Host-to-network data packets on queue1. */
	NSS_STATS_DRV_TX_PACKET_QUEUE_2,	/**< Host-to-network data packets on queue2. */
	NSS_STATS_DRV_TX_PACKET_QUEUE_3,	/**< Host-to-network data packets on queue3. */
	NSS_STATS_DRV_TX_PACKET_QUEUE_4,	/**< Host-to-network data packets on queue4. */
	NSS_STATS_DRV_TX_PACKET_QUEUE_5,	/**< Host-to-network data packets on queue5. */
	NSS_STATS_DRV_TX_PACKET_QUEUE_6,	/**< Host-to-network data packets on queue6. */
	NSS_STATS_DRV_TX_PACKET_QUEUE_7,	/**< Host-to-network data packets on queue7. */
#endif
	NSS_STATS_DRV_MAX,			/**< Maximum message type. */
};

/**
 * nss_stats_types
 *	List of statistics categories.
 */
enum nss_stats_types {
	NSS_STATS_TYPE_COMMON,			/**< Common pnode statistics. */
	NSS_STATS_TYPE_DROP,			/**< Packet drop statistics. */
	NSS_STATS_TYPE_ERROR,			/**< Hardware or software errors different from drop or exception statistics. */
	NSS_STATS_TYPE_EXCEPTION,		/**< Packet exception (to host) statistics. */
	NSS_STATS_TYPE_SPECIAL,			/**< Statistics that do not fall into the above types. */
	NSS_STATS_TYPE_MAX			/**< Maximum message type. */
};

/**
 * nss_stats_notifier_action
 *	Statistics notification types.
 */
enum nss_stats_notifier_action {
	NSS_STATS_EVENT_NOTIFY,
	NSS_STATS_EVENT_MAX
};

/**
 * @}
 */

#endif /* __NSS_STATS_PUBLIC_H */

/*
 **************************************************************************
 * Copyright (c) 2017-2019, The Linux Foundation. All rights reserved.
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
 * @file nss_unaligned.h
 *	NSS unaligned interface definitions.
 */

#ifndef __NSS_UNALIGNED_H
#define __NSS_UNALIGNED_H

/**
 * @addtogroup nss_unaligned_subsystem
 * @{
 */

#define NSS_UNALIGNED_OPS_PER_MSG 54
			/**< The number of operations whose statistics are included in a message. */
#define NSS_UNALIGNED_EMULATED_OPS 64
			/**< The number of operations that are emulated. */

/**
 * nss_unaligned_msg_types
 *	Unaligned message types.
 */
enum nss_unaligned_msg_types {
	NSS_UNALIGNED_MSG_STATS,	/**< Performance statistics message. */
	NSS_UNALIGNED_MSG_MAX,		/**< Maximum unaligned message type. */
};

/**
 * nss_unaligned_stats_op
 *	Performance statistics for emulating a single operation.
 */
struct nss_unaligned_stats_op {
	uint32_t opcode_primary;
		/**< Primary operation code. */
	uint32_t opcode_extension;
		/**< Extension operation code, if applicable. */
	uint64_t count;
		/**< Number of times operation was emulated. */
	uint32_t ticks_min;
		/**< Minimum number of ticks spent emulating operation. */
	uint32_t ticks_avg;
		/**< Average number of ticks spent emulating operation. */
	uint32_t ticks_max;
		/**< Maximum number of ticks spent emulating operation. */
	uint32_t padding;
		/**< Used for consistent alignment, can be re-used. */
};

/**
 * nss_unaligned_stats
 *	Message containing all non-zero operation statistics.
 */
struct nss_unaligned_stats {
	uint64_t trap_count;
					/**< Number of unaligned traps encountered. */
	struct nss_unaligned_stats_op ops[NSS_UNALIGNED_EMULATED_OPS];
					/**< Statistics for each operation. */
};

/**
 * nss_unaligned_stats_msg
 * 	Message containing all non-zero operation statistics.
 */
struct nss_unaligned_stats_msg {
	uint64_t trap_count;		/**< Number of unaligned traps encountered. */
	struct nss_unaligned_stats_op ops[NSS_UNALIGNED_OPS_PER_MSG];
					/**< Statistics for each operation. */
	uint32_t current_iteration;	/**< Number of full statistics messages sent without reaching the end. */
};

/**
 * nss_unaligned_msg
 *	Message from unaligned handler node.
 */
struct nss_unaligned_msg {
	struct nss_cmn_msg cm;		/**< Message header. */

	/**
	 * Unaligned message payload.
	 */
	union {
		struct nss_unaligned_stats_msg stats_msg;
					/**< Message containing statistics. */
	} msg;				/**< Message payload. */
};

/**
 * nss_unaligned_register_handler()
 *	Registers message handler on the NSS unaligned interface and
 *	statistics dentry.
 *
 * @datatypes
 * nss_ctx_instance
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 *
 * @return
 * None.
 */
void nss_unaligned_register_handler(struct nss_ctx_instance *nss_ctx);

/**
 * @}
 */
#endif

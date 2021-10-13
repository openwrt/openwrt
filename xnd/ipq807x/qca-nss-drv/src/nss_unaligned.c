/*
 **************************************************************************
 * Copyright (c) 2017-2020, The Linux Foundation. All rights reserved.
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
 * nss_unaligned.c
 *	NSS unaligned APIs
 */

#include "nss_tx_rx_common.h"
#include "nss_unaligned_stats.h"
#include "nss_unaligned_log.h"

/*
 * nss_unaligned_update_stats()
 *	Updates the statistics in the nss_ctx.
 */
static void nss_unaligned_update_stats(struct nss_ctx_instance *nss_ctx,
					struct nss_unaligned_stats_msg *usm)
{
	uint32_t start_index = NSS_UNALIGNED_OPS_PER_MSG * usm->current_iteration;
	uint32_t i;
	spin_lock_bh(&nss_top_main.stats_lock);
	nss_ctx->unaligned_stats.trap_count = usm->trap_count;
	for (i = 0; i < NSS_UNALIGNED_OPS_PER_MSG; i++) {
		uint32_t index = i + start_index;
		if (unlikely(index >= NSS_UNALIGNED_EMULATED_OPS)) {
			break;
		}
		nss_ctx->unaligned_stats.ops[index] = usm->ops[i];
	}
	spin_unlock_bh(&nss_top_main.stats_lock);
}

/*
 * nss_unaligned_msg_handler()
 *	Handles metadata messages on the unaligned interface.
 */
static void nss_unaligned_msg_handler(struct nss_ctx_instance *nss_ctx,
		struct nss_cmn_msg *ncm, __attribute__((unused))void *app_data)
{
	struct nss_unaligned_msg *um = (struct nss_unaligned_msg *)ncm;

	/*
	 * Sanity checks on message
	 */
	if (um->cm.type >= NSS_UNALIGNED_MSG_MAX) {
		nss_warning("%px: message type out of range: %d\n", nss_ctx, um->cm.type);
		return;
	}

	if (nss_cmn_get_msg_len(&(um->cm)) > sizeof(struct nss_unaligned_msg)) {
		nss_warning("%px: message length is invalid: %d\n", nss_ctx, nss_cmn_get_msg_len(&(um->cm)));
		return;
	}

	nss_unaligned_log_rx_msg(um);

	switch (um->cm.type) {
	case NSS_UNALIGNED_MSG_STATS:
		nss_unaligned_update_stats(nss_ctx, &um->msg.stats_msg);
		return;
	}

	nss_core_log_msg_failures(nss_ctx, ncm);
}

/*
 * nss_unaligned_register_handler()
 *	Registers message handler on the NSS unaligned interface and stats dentry.
 */
void nss_unaligned_register_handler(struct nss_ctx_instance *nss_ctx)
{
	nss_core_register_handler(nss_ctx, NSS_UNALIGNED_INTERFACE, nss_unaligned_msg_handler, NULL);

	if (nss_ctx->id == NSS_CORE_0) {
		nss_unaligned_stats_dentry_create();
	}
}

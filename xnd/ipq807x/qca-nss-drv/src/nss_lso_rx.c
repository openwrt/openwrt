/*
 **************************************************************************
 * Copyright (c) 2014,2017,2019-2020, The Linux Foundation. All rights reserved.
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
 * nss_lso_rx.c
 *	NSS LSO_RX APIs
 */

#include <nss_core.h>
#include "nss_lso_rx_stats.h"
#include "nss_lso_rx_strings.h"

/*
 * nss_rx_lso_rx_interface_handler()
 *	Handle NSS -> HLOS messages for LSO_RX Changes and Statistics
 */
static void nss_rx_lso_rx_interface_handler(struct nss_ctx_instance *nss_ctx, struct nss_cmn_msg *ncm, __attribute__((unused))void *app_data) {

	struct nss_lso_rx_msg *nlrm = (struct nss_lso_rx_msg *)ncm;

	switch (nlrm->cm.type) {
	case NSS_LSO_RX_STATS_SYNC_MSG:
		/*
		 * Update LSO_RX driver statistics and send statistics notifications to the registered modules
		 */
		nss_lso_rx_stats_sync(nss_ctx, &nlrm->msg.stats_sync);
		nss_lso_rx_stats_notify(nss_ctx);
		break;

	default:
		if (ncm->response != NSS_CMN_RESPONSE_ACK) {
			/*
			 * Check response
			 */
			nss_info("%px: Received response %d for type %d, interface %d", nss_ctx, ncm->response, ncm->type, ncm->interface);
		}
	}
}

/*
 * nss_lso_rx_register_handler()
 *	Register handler for messaging
 */
void nss_lso_rx_register_handler(struct nss_ctx_instance *nss_ctx)
{
	nss_core_register_handler(nss_ctx, NSS_LSO_RX_INTERFACE, nss_rx_lso_rx_interface_handler, NULL);
	nss_lso_rx_stats_dentry_create();
	nss_lso_rx_strings_dentry_create();
}

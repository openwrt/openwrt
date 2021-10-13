/*
 **************************************************************************
 * Copyright (c) 2013-2017, 2019-2020 The Linux Foundation. All rights reserved.
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
 * nss_eth_rx.c
 *	NSS ETH_RX APIs
 */

#include <nss_core.h>
#include "nss_eth_rx_stats.h"
#include "nss_eth_rx_strings.h"

/*
 **********************************
 Rx APIs
 **********************************
 */

/*
 * nss_eth_rx_interface_handler()
 *	Handle NSS -> HLOS messages for ETH_RX node
 */
static void nss_eth_rx_interface_handler(struct nss_ctx_instance *nss_ctx, struct nss_cmn_msg *ncm, __attribute__((unused))void *app_data)
{
	struct nss_eth_rx_msg *nem = (struct nss_eth_rx_msg *)ncm;

	/*
	 * Is this a valid request/response packet?
	 */
	if (nem->cm.type >= NSS_METADATA_TYPE_ETH_RX_MAX) {
		nss_warning("%px: received invalid message %d for eth_rx interface", nss_ctx, nem->cm.type);
		return;
	}

	switch (nem->cm.type) {
	case NSS_RX_METADATA_TYPE_ETH_RX_STATS_SYNC:
		/*
		 * Update driver statistics and send stats notifications to the registered modules.
		 */
		nss_eth_rx_metadata_stats_sync(nss_ctx, &nem->msg.node_sync);
		nss_eth_rx_stats_notify(nss_ctx);
		break;

	default:
		if (ncm->response != NSS_CMN_RESPONSE_ACK) {
			/*
			 * Check response
			 */
			nss_info("%px: Received response %d for type %d, interface %d",
						nss_ctx, ncm->response, ncm->type, ncm->interface);
		}
	}
}

/*
 * nss_eth_rx_register_handler()
 */
void nss_eth_rx_register_handler(struct nss_ctx_instance *nss_ctx)
{
	nss_core_register_handler(nss_ctx, NSS_ETH_RX_INTERFACE, nss_eth_rx_interface_handler, NULL);

	nss_eth_rx_stats_dentry_create();
	nss_eth_rx_strings_dentry_create();
}

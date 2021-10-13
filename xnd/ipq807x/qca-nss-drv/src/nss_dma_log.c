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

/*
 * nss_dma_log.c
 *	NSS DMA logger file.
 */

#include "nss_core.h"

/*
 * nss_dma_log_message_types_str
 *	DMA message strings
 */
static int8_t *nss_dma_log_message_types_str[NSS_DMA_MSG_TYPE_MAX] __maybe_unused = {
	"DMA invalid message",
	"DMA Configure message",
	"DMA Statistics sync message",
	"DMA Test linearization performance",
};

/*
 * nss_dma_log_error_response_types_str
 *	Strings for error types for DMA messages
 */
static int8_t *nss_dma_log_error_response_types_str[NSS_DMA_MSG_ERROR_MAX] __maybe_unused = {
	"No error",
	"HW initialization failed",
	"Unhandled message type for node",
	"Error performing the test",
};

/*
 * nss_dma_map_msg()
 *	Log NSS DMA configure message.
 */
static void nss_dma_configure_msg(struct nss_dma_msg *ndm)
{
	nss_trace("%px: NSS DMA configure message: \n",ndm);
}

/*
 * nss_dma_test_perf_msg()
 *	Log NSS DMA performace test message.
 */
static void nss_dma_test_perf_msg(struct nss_dma_msg *ndm)
{
	struct nss_dma_test_cfg *ndtc = &ndm->msg.test_cfg;
	struct nss_cmn_node_stats *ncns = &ndtc->node_stats;

	nss_trace("%px: NSS DMA test perf message: \n",ndm);
	nss_trace("%px: processed (TX: %u, RX:%u, time:%u)\n", ndm, ncns->tx_packets, ncns->rx_packets, ndtc->time_delta);
	nss_trace("%px: test parameters (type:%u, packet_cnt:%u)\n", ndm, ndtc->type, ndtc->packet_count);
}

/*
 * nss_dma_log_verbose()
 *	Log message contents.
 */
static void nss_dma_log_verbose(struct nss_dma_msg *ndm)
{
	switch (ndm->cm.type) {
	case NSS_DMA_MSG_TYPE_CONFIGURE:
		nss_dma_configure_msg(ndm);
		break;

	case NSS_DMA_MSG_TYPE_TEST_PERF:
		nss_dma_test_perf_msg(ndm);
		break;

	default:
		nss_trace("%px: Invalid message type\n", ndm);
		break;
	}
}

/*
 * nss_dma_log_tx_msg()
 *	Log messages transmitted to FW.
 */
void nss_dma_log_tx_msg(struct nss_dma_msg *ndm)
{
	if (ndm->cm.type >= NSS_DMA_MSG_TYPE_MAX) {
		nss_info("%px: Invalid message type\n", ndm);
		return;
	}

	nss_info("%px: type[%d]:%s\n", ndm, ndm->cm.type, nss_dma_log_message_types_str[ndm->cm.type]);
	nss_dma_log_verbose(ndm);
}

/*
 * nss_dma_log_rx_msg()
 *	Log messages received from FW.
 */
void nss_dma_log_rx_msg(struct nss_dma_msg *ndm)
{
	if (ndm->cm.response >= NSS_CMN_RESPONSE_LAST) {
		nss_warning("%px: Invalid response\n", ndm);
		return;
	}

	if (ndm->cm.response == NSS_CMN_RESPONSE_NOTIFY || (ndm->cm.response == NSS_CMN_RESPONSE_ACK)) {
		nss_info("%px: type[%d]:%s, response[%d]:%s\n", ndm, ndm->cm.type,
			nss_dma_log_message_types_str[ndm->cm.type],
			ndm->cm.response, nss_cmn_response_str[ndm->cm.response]);
		goto verbose;
	}

	if (ndm->cm.error >= NSS_DMA_MSG_ERROR_MAX) {
		nss_warning("%px: msg failure - type[%d]:%s, response[%d]:%s, error[%d]:Invalid error\n",
			ndm, ndm->cm.type, nss_dma_log_message_types_str[ndm->cm.type],
			ndm->cm.response, nss_cmn_response_str[ndm->cm.response],
			ndm->cm.error);
		goto verbose;
	}

	nss_info("%px: msg nack - type[%d]:%s, response[%d]:%s, error[%d]:%s\n",
		ndm, ndm->cm.type, nss_dma_log_message_types_str[ndm->cm.type],
		ndm->cm.response, nss_cmn_response_str[ndm->cm.response],
		ndm->cm.error, nss_dma_log_error_response_types_str[ndm->cm.error]);

verbose:
	nss_dma_log_verbose(ndm);
}

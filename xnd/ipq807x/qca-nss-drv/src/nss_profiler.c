/*
 **************************************************************************
 * Copyright (c) 2013-2020, The Linux Foundation. All rights reserved.
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
 * nss_profiler.c
 *	NSS profiler APIs
 */

#include "nss_tx_rx_common.h"

/*
 * nss_profiler_rx_msg_handler()
 *	Handle profiler information.
 */
static void nss_profiler_rx_msg_handler(struct nss_ctx_instance *nss_ctx, struct nss_cmn_msg *ncm, void *app)
{
	struct nss_profiler_msg *pm = (struct nss_profiler_msg*)ncm;
	void *ctx = nss_ctx->nss_top->profiler_ctx[nss_ctx->id];
	nss_profiler_callback_t cb = nss_ctx->nss_top->profiler_callback[nss_ctx->id];

	if (ncm->type >= NSS_PROFILER_MAX_MSG_TYPES) {
		nss_warning("%px: message type out of range: %d", nss_ctx, ncm->type);
		return;
	}

	if (ncm->type <= NSS_PROFILER_FLOWCTRL_MSG) {
		if (ncm->len > sizeof(pm->payload.pcmdp)) {
			nss_warning("%px: reply for cmd %d size is wrong %d : %d\n", nss_ctx, ncm->type, ncm->len, ncm->interface);
			return;
		}
	} else if (ncm->type <= NSS_PROFILER_DEBUG_REPLY_MSG) {
		if (ncm->len > sizeof(pm->payload.pdm)) {
			nss_warning("%px: reply for debug %d is too big %d\n", nss_ctx, ncm->type, ncm->len);
			return;
		}
	} else if (ncm->type <= NSS_PROFILER_COUNTERS_MSG) {
		if (ncm->len < (sizeof(pm->payload.pcmdp) - (PROFILE_MAX_APP_COUNTERS - pm->payload.pcmdp.num_counters) * sizeof(pm->payload.pcmdp.counters[0])) || ncm->len > sizeof(pm->payload.pcmdp)) {
			nss_warning("%px: %d params data is too big %d : %d\n", nss_ctx, ncm->type, ncm->len, ncm->interface);
			return;
		}
	}

	/*
	 * status per request callback
	 */
	if (ncm->response != NSS_CMN_RESPONSE_NOTIFY && ncm->cb) {
		nss_info("%px: reply CB %px for %d %d\n", nss_ctx, (void *)ncm->cb, ncm->type, ncm->response);
		cb = (nss_profiler_callback_t)ncm->cb;
	}

	/*
	 * sample related callback
	 */
	if (!cb || !ctx) {
		nss_warning("%px: Event received for profiler interface before registration", nss_ctx);
		return;
	}

	cb(ctx, (struct nss_profiler_msg *)ncm);
}

/*
 * nss_tx_profiler_if_buf()
 *	NSS profiler Tx API
 */
nss_tx_status_t nss_profiler_if_tx_buf(void *ctx, void *buf, uint32_t len,
					void *cb, void *app_data)
{
	struct nss_ctx_instance *nss_ctx = (struct nss_ctx_instance *)ctx;
	struct nss_profiler_msg *npm;
	struct nss_profiler_data_msg *pdm = (struct nss_profiler_data_msg *)buf;
	nss_tx_status_t ret;

	nss_trace("%px: Profiler If Tx, buf=%px", nss_ctx, buf);

	if (sizeof(npm->payload) < len) {
		nss_warning("%px: (%u)Bad message length(%u)", nss_ctx, NSS_PROFILER_INTERFACE, len);
		return NSS_TX_FAILURE_TOO_LARGE;
	}

	if (NSS_NBUF_PAYLOAD_SIZE < (len + sizeof(npm->cm))) {
		nss_warning("%px: (%u)Message length(%u) is larger than payload size (%u)",
			nss_ctx, NSS_PROFILER_INTERFACE, (uint32_t)(len + sizeof(npm->cm)), NSS_NBUF_PAYLOAD_SIZE);
		return NSS_TX_FAILURE_TOO_LARGE;
	}

	npm = kzalloc(sizeof(*npm), GFP_KERNEL);
	if (!npm) {
		nss_warning("%px: Failed to allocate memory for message\n", nss_ctx);
		return NSS_TX_FAILURE;
	}

	memcpy(&npm->payload, pdm, len);
	nss_profiler_msg_init(npm, NSS_PROFILER_INTERFACE, pdm->hd_magic & 0xFF, len,
				cb, app_data);

	ret = nss_core_send_cmd(nss_ctx, npm, sizeof(npm->cm) + len, NSS_NBUF_PAYLOAD_SIZE);
	kfree(npm);
	return ret;
}
EXPORT_SYMBOL(nss_profiler_if_tx_buf);

/*
 * nss_profiler_alloc_dma()
 *	Allocate a DMA for profiler.
 */
void *nss_profiler_alloc_dma(struct nss_ctx_instance *nss_ctx, struct nss_profile_sdma_producer **dma_p)
{
	int size;
	void *kaddr;
	struct nss_profile_sdma_producer *dma;
	struct nss_profile_sdma_ctrl *ctrl = (struct nss_profile_sdma_ctrl *)nss_ctx->meminfo_ctx.sdma_ctrl;
	if (!ctrl)
		return NULL;

	dma = ctrl->producer;
	*dma_p = dma;
	size = dma->num_bufs * dma->buf_size;
	kaddr = kmalloc(size, GFP_KERNEL | __GFP_ZERO);

	if (kaddr) {
		dma->desc_ring = dma_map_single(nss_ctx->dev, kaddr, size, DMA_FROM_DEVICE);
		NSS_CORE_DSB();
	}
	ctrl->consumer[0].ring.kp = kaddr;
	return kaddr;
}
EXPORT_SYMBOL(nss_profiler_alloc_dma);

/*
 * nss_profiler_release_dma()
 *	Free profiler DMA.
 */
void nss_profiler_release_dma(struct nss_ctx_instance *nss_ctx)
{
	struct nss_profile_sdma_ctrl *ctrl;
	if (!nss_ctx)
		return;

	ctrl = nss_ctx->meminfo_ctx.sdma_ctrl;

	if (ctrl && ctrl->consumer[0].ring.kp) {
		kfree(ctrl->consumer[0].ring.kp);
		ctrl->consumer[0].ring.kp = NULL;
	}
}
EXPORT_SYMBOL(nss_profiler_release_dma);

/*
 * nss_profile_dma_register_cb
 *      Register a handler for profile DMA.
 */
bool nss_profile_dma_register_cb(struct nss_ctx_instance *nss_ctx, int id,
				void (*cb)(void*), void *arg)
{
	struct nss_profile_sdma_ctrl *ctrl = (struct nss_profile_sdma_ctrl *)nss_ctx->meminfo_ctx.sdma_ctrl;
	nss_info("%px dma_register_cb %d: %px %px\n", ctrl, id, cb, arg);
	if (!ctrl)
		return false;

	ctrl->consumer[id].dispatch.fp = cb;
	ctrl->consumer[id].arg.kp = arg;
	return true;
}
EXPORT_SYMBOL(nss_profile_dma_register_cb);

/*
 * nss_profile_dma_deregister_cb
 *      Deregister callback for profile DMA.
 */
bool nss_profile_dma_deregister_cb(struct nss_ctx_instance *nss_ctx, int id)
{
	struct nss_profile_sdma_ctrl *ctrl = (struct nss_profile_sdma_ctrl *)nss_ctx->meminfo_ctx.sdma_ctrl;
	if (!ctrl)
		return false;

	ctrl->consumer[id].dispatch.fp = NULL;
	return true;
}
EXPORT_SYMBOL(nss_profile_dma_deregister_cb);

/*
 * nss_profile_dma_get_ctrl
 *      Wrapper to get profile DMA control.
 */
struct nss_profile_sdma_ctrl *nss_profile_dma_get_ctrl(struct nss_ctx_instance *nss_ctx)
{
	struct nss_profile_sdma_ctrl *ctrl = nss_ctx->meminfo_ctx.sdma_ctrl;
	if (!ctrl) {
		return ctrl;
	}

	dmac_inv_range(ctrl, &ctrl->cidx);
	dsb(sy);
	return ctrl;
}
EXPORT_SYMBOL(nss_profile_dma_get_ctrl);

/*
 * nss_profiler_notify_register()
 */
void *nss_profiler_notify_register(nss_core_id_t core_id, nss_profiler_callback_t profiler_callback, void *ctx)
{
	nss_assert(core_id < NSS_CORE_MAX);

	if (NSS_CORE_STATUS_SUCCESS !=
		nss_core_register_handler(&nss_top_main.nss[core_id], NSS_PROFILER_INTERFACE, nss_profiler_rx_msg_handler, NULL)) {
			nss_warning("Message handler FAILED to be registered for profiler");
			return NULL;
	}

	nss_top_main.profiler_ctx[core_id] = ctx;
	nss_top_main.profiler_callback[core_id] = profiler_callback;

	return (void *)&nss_top_main.nss[core_id];
}
EXPORT_SYMBOL(nss_profiler_notify_register);

/*
 * nss_profiler_notify_unregister()
 */
void nss_profiler_notify_unregister(nss_core_id_t core_id)
{
	nss_assert(core_id < NSS_CORE_MAX);

	nss_core_unregister_handler(&nss_top_main.nss[core_id], NSS_PROFILER_INTERFACE);
	nss_top_main.profiler_callback[core_id] = NULL;
	nss_top_main.profiler_ctx[core_id] = NULL;
}
EXPORT_SYMBOL(nss_profiler_notify_unregister);

/*
 * nss_profiler_msg_init()
 *	Initialize profiler message.
 */
void nss_profiler_msg_init(struct nss_profiler_msg *npm, uint16_t if_num, uint32_t type, uint32_t len,
				nss_profiler_callback_t cb, void *app_data)
{
	nss_cmn_msg_init(&npm->cm, if_num, type, len, (void *)cb, app_data);
}
EXPORT_SYMBOL(nss_profiler_msg_init);

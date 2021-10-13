/*
 **************************************************************************
 * Copyright (c) 2014-2016,2020 The Linux Foundation. All rights reserved.
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

#include "nss_data_plane.h"
#include "nss_core.h"

static struct delayed_work nss_data_plane_work;
static struct workqueue_struct *nss_data_plane_workqueue;

extern bool pn_mq_en;
extern uint16_t pn_qlimits[NSS_MAX_NUM_PRI];

/*
 * nss_data_plane_work_function()
 *	Work function that gets queued to "install" the data plane overlays
 */
static void nss_data_plane_work_function(struct work_struct *work)
{
	int ret;
	struct nss_ctx_instance *nss_ctx = &nss_top_main.nss[NSS_CORE_0];
	struct nss_top_instance *nss_top = nss_ctx->nss_top;

	/*
	 * The queue config command is a synchronous command and needs to be issued
	 * in process context, before NSS data plane switch.
	 */
	ret = nss_n2h_update_queue_config_sync(nss_ctx, pn_mq_en, pn_qlimits);
	if (ret != NSS_TX_SUCCESS) {
		nss_warning("Failed to send pnode queue config to core 0\n");
	}

	nss_top->data_plane_ops->data_plane_register(nss_ctx);
}

/*
 * nss_data_plane_schedule_registration()
 *	Called from nss_init to schedule a work to do data_plane register to data plane host
 */
bool nss_data_plane_schedule_registration(void)
{
	if (!queue_work_on(1, nss_data_plane_workqueue, &nss_data_plane_work.work)) {
		nss_warning("Failed to register data plane workqueue on core 1\n");
		return false;
	}

	nss_info("Register data plane workqueue on core 1\n");
	return true;
}

/*
 * nss_data_plane_init_delay_work()
 */
int nss_data_plane_init_delay_work(void)
{
	nss_data_plane_workqueue = create_singlethread_workqueue("nss_data_plane_workqueue");
	if (!nss_data_plane_workqueue) {
		nss_warning("Can't allocate workqueue\n");
		return -ENOMEM;
	}

	INIT_DELAYED_WORK(&nss_data_plane_work, nss_data_plane_work_function);
	return 0;
}

/*
 * nss_data_plane_destroy_delay_work()
 */
void nss_data_plane_destroy_delay_work(void)
{
	destroy_workqueue(nss_data_plane_workqueue);
}

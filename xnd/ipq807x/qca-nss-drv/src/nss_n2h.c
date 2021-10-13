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
 * nss_n2h.c
 *	NSS N2H node APIs
 */

#include "nss_tx_rx_common.h"
#include "nss_n2h_stats.h"
#include "nss_n2h_strings.h"
#include "nss_drv_strings.h"

#define NSS_N2H_MAX_BUF_POOL_SIZE (1024 * 1024 * 20) /* 20MB */
#define NSS_N2H_MIN_EMPTY_POOL_BUF_SZ		32
#define NSS_N2H_MAX_EMPTY_POOL_BUF_SZ		131072
#define NSS_N2H_DEFAULT_EMPTY_POOL_BUF_SZ	8192
#define NSS_N2H_TX_TIMEOUT 3000 /* 3 Seconds */

int nss_n2h_empty_pool_buf_cfg[NSS_MAX_CORES] __read_mostly = {-1, -1};
int nss_n2h_empty_paged_pool_buf_cfg[NSS_MAX_CORES] __read_mostly = {-1, -1};
int nss_n2h_water_mark[NSS_MAX_CORES][2] __read_mostly = {{-1, -1}, {-1, -1} };
int nss_n2h_paged_water_mark[NSS_MAX_CORES][2] __read_mostly = {{-1, -1}, {-1, -1} };
int nss_n2h_wifi_pool_buf_cfg __read_mostly = -1;
int nss_n2h_core0_mitigation_cfg __read_mostly = 1;
int nss_n2h_core1_mitigation_cfg __read_mostly = 1;
int nss_n2h_core0_add_buf_pool_size __read_mostly;
int nss_n2h_core1_add_buf_pool_size __read_mostly;
int nss_n2h_queue_limit[NSS_MAX_CORES] __read_mostly = {NSS_DEFAULT_QUEUE_LIMIT, NSS_DEFAULT_QUEUE_LIMIT};
int nss_n2h_host_bp_config[NSS_MAX_CORES] __read_mostly;

struct nss_n2h_registered_data {
	nss_n2h_msg_callback_t n2h_callback;
	void *app_data;
};

static struct nss_n2h_cfg_pvt nss_n2h_nepbcfgp[NSS_MAX_CORES];
static struct nss_n2h_registered_data nss_n2h_rd[NSS_MAX_CORES];
static struct nss_n2h_cfg_pvt nss_n2h_rcp;
static struct nss_n2h_cfg_pvt nss_n2h_mitigationcp[NSS_CORE_MAX];
static struct nss_n2h_cfg_pvt nss_n2h_bufcp[NSS_CORE_MAX];
static struct nss_n2h_cfg_pvt nss_n2h_wp;
static struct nss_n2h_cfg_pvt nss_n2h_q_cfg_pvt;
static struct nss_n2h_cfg_pvt nss_n2h_q_lim_pvt;
static struct nss_n2h_cfg_pvt nss_n2h_host_bp_cfg_pvt;

/*
 * nss_n2h_interface_handler()
 *	Handle NSS -> HLOS messages for N2H node
 */
static void nss_n2h_interface_handler(struct nss_ctx_instance *nss_ctx,
					struct nss_cmn_msg *ncm,
					void *app_data)
{
	struct nss_n2h_msg *nnm = (struct nss_n2h_msg *)ncm;
	nss_n2h_msg_callback_t cb;

	BUG_ON(ncm->interface != NSS_N2H_INTERFACE);

	/*
	 * Is this a valid request/response packet?
	 */
	if (nnm->cm.type >= NSS_METADATA_TYPE_N2H_MAX) {
		nss_warning("%px: received invalid message %d for Offload stats interface", nss_ctx, nnm->cm.type);
		return;
	}

	switch (nnm->cm.type) {
	case NSS_TX_METADATA_TYPE_N2H_RPS_CFG:
		nss_info("NSS N2H rps_en %d \n",nnm->msg.rps_cfg.enable);
		break;

	case NSS_TX_METADATA_TYPE_N2H_MITIGATION_CFG:
		nss_info("NSS N2H mitigation_dis %d \n",nnm->msg.mitigation_cfg.enable);
		break;

	case NSS_TX_METADATA_TYPE_N2H_EMPTY_POOL_BUF_CFG:
		nss_info("%px: empty pool buf cfg response from FW", nss_ctx);
		break;

	case NSS_TX_METADATA_TYPE_N2H_FLUSH_PAYLOADS:
		nss_info("%px: flush payloads cmd response from FW", nss_ctx);
		break;

	case NSS_RX_METADATA_TYPE_N2H_STATS_SYNC:
		/*
		 * Update driver statistics and send statistics notifications to the registered modules.
		 */
		nss_n2h_stats_sync(nss_ctx, &nnm->msg.stats_sync);
		nss_n2h_stats_notify(nss_ctx);
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

	/*
	 * Update the callback and app_data for NOTIFY messages, n2h sends all notify messages
	 * to the same callback/app_data.
	 */
	if (nnm->cm.response == NSS_CMN_RESPONSE_NOTIFY) {
		/*
		 * Place holder for the user to create right call
		 * back and app data when response is NSS_CMN_RESPONSE_NOTIFY
		 */
		ncm->cb = (nss_ptr_t)nss_n2h_rd[nss_ctx->id].n2h_callback;
		ncm->app_data = (nss_ptr_t)nss_n2h_rd[nss_ctx->id].app_data;
	}

	/*
	 * Do we have a callback?
	 */
	if (!ncm->cb) {
		return;
	}

	/*
	 * Callback
	 */
	cb = (nss_n2h_msg_callback_t)ncm->cb;
	cb((void *)ncm->app_data, nnm);
}

/*
 * nss_n2h_mitigation_cfg_callback()
 *	call back function for mitigation configuration
 */
static void nss_n2h_mitigation_cfg_callback(void *app_data, struct nss_n2h_msg *nnm)
{
	uint32_t core_num = (uint32_t)(nss_ptr_t)app_data;
	struct nss_top_instance *nss_top = &nss_top_main;
	struct nss_ctx_instance *nss_ctx = &nss_top->nss[core_num];

	if (nnm->cm.response != NSS_CMN_RESPONSE_ACK) {

		/*
		 * Error, hence we are not updating the nss_n2h_mitigate_en
		 */
		nss_n2h_mitigationcp[core_num].response = NSS_FAILURE;
		complete(&nss_n2h_mitigationcp[core_num].complete);
		nss_warning("core%d: MITIGATION configuration failed : %d\n", core_num, nnm->cm.error);
		return;
	}

	nss_info("core%d: MITIGATION configuration succeeded: %d\n", core_num, nnm->cm.error);

	nss_ctx->n2h_mitigate_en = nnm->msg.mitigation_cfg.enable;
	nss_n2h_mitigationcp[core_num].response = NSS_SUCCESS;
	complete(&nss_n2h_mitigationcp[core_num].complete);
}

/*
 * nss_n2h_buf_cfg_callback()
 *	call back function for pbuf configuration
 */
static void nss_n2h_bufs_cfg_callback(void *app_data, struct nss_n2h_msg *nnm)
{
	uint32_t core_num = (uint32_t)(nss_ptr_t)app_data;
	unsigned int allocated_sz;

	struct nss_top_instance *nss_top = &nss_top_main;
	struct nss_ctx_instance *nss_ctx = &nss_top->nss[core_num];

	if (nnm->cm.response != NSS_CMN_RESPONSE_ACK) {
		nss_n2h_bufcp[core_num].response = NSS_FAILURE;
		nss_warning("core%d: buf configuration failed : %d\n", core_num, nnm->cm.error);
		goto done;
	}

	nss_info("core%d: buf configuration succeeded: %d\n", core_num, nnm->cm.error);

	allocated_sz = nnm->msg.buf_pool.nss_buf_page_size * nnm->msg.buf_pool.nss_buf_num_pages;
	nss_ctx->buf_sz_allocated += allocated_sz;

	nss_n2h_bufcp[core_num].response = NSS_SUCCESS;

done:
	complete(&nss_n2h_bufcp[core_num].complete);
}

/*
 * nss_n2h_payload_stats_callback()
 *	It gets called response to payload accounting.
 */
static void nss_n2h_payload_stats_callback(void *app_data,
					struct nss_n2h_msg *nnm)
{
	uint32_t core_num = (uint32_t)(nss_ptr_t)app_data;

	if (nnm->cm.response != NSS_CMN_RESPONSE_ACK) {
		struct nss_n2h_empty_pool_buf *nnepbcm;
		nnepbcm = &nnm->msg.empty_pool_buf_cfg;

		nss_warning("%d: core empty pool buf set failure: %d\n",
				core_num, nnm->cm.error);
		nss_n2h_nepbcfgp[core_num].response = NSS_FAILURE;
		complete(&nss_n2h_nepbcfgp[core_num].complete);
		return;
	}

	if (nnm->cm.type == NSS_TX_METADATA_TYPE_GET_WATER_MARK) {
		nss_n2h_nepbcfgp[core_num].empty_buf_pool_info.pool_size =
			ntohl(nnm->msg.payload_info.pool_size);
		nss_n2h_nepbcfgp[core_num].empty_buf_pool_info.low_water =
			ntohl(nnm->msg.payload_info.low_water);
		nss_n2h_nepbcfgp[core_num].empty_buf_pool_info.high_water =
			ntohl(nnm->msg.payload_info.high_water);
	}

	if (nnm->cm.type == NSS_TX_METADATA_TYPE_GET_PAGED_WATER_MARK) {
		nss_n2h_nepbcfgp[core_num].empty_paged_buf_pool_info.pool_size =
			ntohl(nnm->msg.paged_payload_info.pool_size);
		nss_n2h_nepbcfgp[core_num].empty_paged_buf_pool_info.low_water =
			ntohl(nnm->msg.paged_payload_info.low_water);
		nss_n2h_nepbcfgp[core_num].empty_paged_buf_pool_info.high_water =
			ntohl(nnm->msg.paged_payload_info.high_water);
	}

	nss_n2h_nepbcfgp[core_num].response = NSS_SUCCESS;
	complete(&nss_n2h_nepbcfgp[core_num].complete);
}

/*
 * nss_n2h_set_wifi_payloads_callback()
 *	call back function for response to wifi pool configuration
 *
 */
static void nss_n2h_set_wifi_payloads_callback(void *app_data,
					struct nss_n2h_msg *nnm)
{
	struct nss_ctx_instance *nss_ctx __maybe_unused = (struct nss_ctx_instance *)app_data;
	if (nnm->cm.response != NSS_CMN_RESPONSE_ACK) {

		nss_n2h_wp.response = NSS_FAILURE;
		complete(&nss_n2h_wp.complete);
		nss_warning("%px: wifi pool configuration failed : %d\n", nss_ctx,
				nnm->cm.error);
		return;
	}

	nss_info("%px: wifi payload configuration succeeded: %d\n", nss_ctx,
			nnm->cm.error);
	nss_n2h_wp.response = NSS_SUCCESS;
	nss_n2h_wp.wifi_pool = ntohl(nnm->msg.wp.payloads);
	complete(&nss_n2h_wp.complete);
}

/*
 * nss_n2h_get_payload_info()
 *	Gets Payload information.
 */
static int nss_n2h_get_payload_info(nss_ptr_t core_num, struct nss_n2h_msg *nnm, struct nss_n2h_payload_info *nnepbcm)
{
	struct nss_top_instance *nss_top = &nss_top_main;
	struct nss_ctx_instance *nss_ctx = &nss_top->nss[core_num];
	nss_tx_status_t nss_tx_status;
	int ret = NSS_FAILURE;

	/*
	 * Note that semaphore should be already held.
	 */

	nss_tx_status = nss_n2h_tx_msg(nss_ctx, nnm);

	if (nss_tx_status != NSS_TX_SUCCESS) {
		nss_warning("%px: core %d nss_tx error errorn", nss_ctx, (int)core_num);
		return NSS_FAILURE;
	}

	/*
	 * Blocking call, wait till we get ACK for this msg.
	 */
	ret = wait_for_completion_timeout(&nss_n2h_nepbcfgp[core_num].complete,
			msecs_to_jiffies(NSS_CONN_CFG_TIMEOUT));
	if (ret == 0) {
		nss_warning("%px: core %d waiting for ack timed out\n", nss_ctx, (int)core_num);
		return NSS_FAILURE;
	}

	if (NSS_FAILURE == nss_n2h_nepbcfgp[core_num].response) {
		nss_warning("%px: core %d response returned failure\n", nss_ctx, (int)core_num);
		return NSS_FAILURE;
	}

	return NSS_SUCCESS;
}

/*
 * nss_n2h_get_default_payload_info()
 *	Gets the default payload information.
 */
static int nss_n2h_get_default_payload_info(nss_ptr_t core_num)
{
	struct nss_n2h_msg nnm;

	nss_n2h_msg_init(&nnm, NSS_N2H_INTERFACE,
			NSS_TX_METADATA_TYPE_GET_WATER_MARK,
			sizeof(struct nss_n2h_payload_info),
			nss_n2h_payload_stats_callback,
			(void *)core_num);

	return nss_n2h_get_payload_info(core_num, &nnm,
			&nnm.msg.payload_info);
}

/*
 * nss_n2h_get_paged_payload_info()
 *	Gets the paged payload information.
 */
static int nss_n2h_get_paged_payload_info(nss_ptr_t core_num)
{
	struct nss_n2h_msg nnm;

	nss_n2h_msg_init(&nnm, NSS_N2H_INTERFACE,
			NSS_TX_METADATA_TYPE_GET_PAGED_WATER_MARK,
			sizeof(struct nss_n2h_payload_info),
			nss_n2h_payload_stats_callback,
			(void *)core_num);

	return nss_n2h_get_payload_info(core_num, &nnm,
			&nnm.msg.paged_payload_info);
}

/*
 * nss_n2h_set_empty_buf_pool()
 *	Sets empty pool buffer
 */
static int nss_n2h_set_empty_buf_pool(struct ctl_table *ctl, int write,
				void __user *buffer,
				size_t *lenp, loff_t *ppos,
				nss_ptr_t core_num, int *new_val)
{
	struct nss_top_instance *nss_top = &nss_top_main;
	struct nss_ctx_instance *nss_ctx = &nss_top->nss[core_num];
	struct nss_n2h_msg nnm;
	struct nss_n2h_empty_pool_buf *nnepbcm;
	nss_tx_status_t nss_tx_status;
	int ret = NSS_FAILURE;

	/*
	 * Acquiring semaphore
	 */
	down(&nss_n2h_nepbcfgp[core_num].sem);

	/*
	 * Take snap shot of current value
	 */
	nss_n2h_nepbcfgp[core_num].empty_buf_pool_info.pool_size = *new_val;

	if (!write) {
		ret = nss_n2h_get_default_payload_info(core_num);
		*new_val = nss_n2h_nepbcfgp[core_num].empty_buf_pool_info.pool_size;
		if (ret == NSS_FAILURE) {
			up(&nss_n2h_nepbcfgp[core_num].sem);
			return -EBUSY;
		}

		up(&nss_n2h_nepbcfgp[core_num].sem);

		ret = proc_dointvec(ctl, write, buffer, lenp, ppos);
		return ret;
	}

	ret = proc_dointvec(ctl, write, buffer, lenp, ppos);
	if (ret) {
		up(&nss_n2h_nepbcfgp[core_num].sem);
		return ret;
	}

	if ((*new_val < NSS_N2H_MIN_EMPTY_POOL_BUF_SZ)) {
		nss_warning("%px: core %d setting %d < min number of buffer",
				nss_ctx, (int)core_num, *new_val);
		goto failure;
	}

	nss_info("%px: core %d number of empty pool buffer is : %d\n",
		nss_ctx, (int)core_num, *new_val);

	nss_n2h_msg_init(&nnm, NSS_N2H_INTERFACE,
			NSS_TX_METADATA_TYPE_N2H_EMPTY_POOL_BUF_CFG,
			sizeof(struct nss_n2h_empty_pool_buf),
			nss_n2h_payload_stats_callback,
			(nss_ptr_t *)core_num);

	nnepbcm = &nnm.msg.empty_pool_buf_cfg;
	nnepbcm->pool_size = htonl(*new_val);
	nss_tx_status = nss_n2h_tx_msg(nss_ctx, &nnm);

	if (nss_tx_status != NSS_TX_SUCCESS) {
		nss_warning("%px: core %d nss_tx error empty pool buffer: %d\n",
				nss_ctx, (int)core_num, *new_val);
		goto failure;
	}

	/*
	 * Blocking call, wait till we get ACK for this msg.
	 */
	ret = wait_for_completion_timeout(&nss_n2h_nepbcfgp[core_num].complete,
			msecs_to_jiffies(NSS_CONN_CFG_TIMEOUT));
	if (ret == 0) {
		nss_warning("%px: core %d Waiting for ack timed out\n", nss_ctx, (int)core_num);
		goto failure;
	}

	/*
	 * ACK/NACK received from NSS FW
	 * If ACK: Callback function will update nss_n2h_empty_pool_buf with
	 * nss_n2h_nepbcfgp.num_conn_valid, which holds the user input
	 */
	if (NSS_FAILURE == nss_n2h_nepbcfgp[core_num].response) {
		goto failure;
	}

	up(&nss_n2h_nepbcfgp[core_num].sem);
	return 0;

failure:
	/*
	 * Restore the current_value to its previous state
	 */
	*new_val = nss_n2h_nepbcfgp[core_num].empty_buf_pool_info.pool_size;
	up(&nss_n2h_nepbcfgp[core_num].sem);
	return NSS_FAILURE;
}

/*
 * nss_n2h_set_empty_paged_pool_buf()
 *	Sets empty paged pool buffer
 */
static int nss_n2h_set_empty_paged_pool_buf(struct ctl_table *ctl, int write,
				void __user *buffer,
				size_t *lenp, loff_t *ppos,
				nss_ptr_t core_num, int *new_val)
{
	struct nss_top_instance *nss_top = &nss_top_main;
	struct nss_ctx_instance *nss_ctx = &nss_top->nss[core_num];
	struct nss_n2h_msg nnm;
	struct nss_n2h_empty_pool_buf *nneppbcm;
	nss_tx_status_t nss_tx_status;
	int ret = NSS_FAILURE;

	/*
	 * Acquiring semaphore
	 */
	down(&nss_n2h_nepbcfgp[core_num].sem);

	/*
	 * Take snap shot of current value
	 */
	nss_n2h_nepbcfgp[core_num].empty_paged_buf_pool_info.pool_size = *new_val;

	if (!write) {
		ret = nss_n2h_get_paged_payload_info(core_num);
		*new_val = nss_n2h_nepbcfgp[core_num].empty_paged_buf_pool_info.pool_size;
		if (ret == NSS_FAILURE) {
			up(&nss_n2h_nepbcfgp[core_num].sem);
			return -EBUSY;
		}

		up(&nss_n2h_nepbcfgp[core_num].sem);

		ret = proc_dointvec(ctl, write, buffer, lenp, ppos);
		return ret;
	}

	ret = proc_dointvec(ctl, write, buffer, lenp, ppos);
	if (ret) {
		up(&nss_n2h_nepbcfgp[core_num].sem);
		return ret;
	}

	if ((*new_val < NSS_N2H_MIN_EMPTY_POOL_BUF_SZ)) {
		nss_warning("%px: core %d setting %d < min number of buffer",
				nss_ctx, (int)core_num, *new_val);
		goto failure;
	}

	nss_info("%px: core %d number of empty paged pool buffer is : %d\n",
		nss_ctx, (int)core_num, *new_val);

	nss_n2h_msg_init(&nnm, NSS_N2H_INTERFACE,
			NSS_TX_METADATA_TYPE_N2H_EMPTY_PAGED_POOL_BUF_CFG,
			sizeof(struct nss_n2h_empty_pool_buf),
			nss_n2h_payload_stats_callback,
			(nss_ptr_t *)core_num);

	nneppbcm = &nnm.msg.empty_pool_buf_cfg;
	nneppbcm->pool_size = htonl(*new_val);
	nss_tx_status = nss_n2h_tx_msg(nss_ctx, &nnm);

	if (nss_tx_status != NSS_TX_SUCCESS) {
		nss_warning("%px: core %d nss_tx error empty paged pool buffer: %d\n",
				nss_ctx, (int)core_num, *new_val);
		goto failure;
	}

	/*
	 * Blocking call, wait till we get ACK for this msg.
	 */
	ret = wait_for_completion_timeout(&nss_n2h_nepbcfgp[core_num].complete,
			msecs_to_jiffies(NSS_CONN_CFG_TIMEOUT));
	if (ret == 0) {
		nss_warning("%px: core %d Waiting for ack timed out\n", nss_ctx, (int)core_num);
		goto failure;
	}

	/*
	 * ACK/NACK received from NSS FW
	 * If ACK: Callback function will update nss_n2h_empty_pool_buf with
	 * nss_n2h_nepbcfgp.num_conn_valid, which holds the user input
	 */
	if (NSS_FAILURE == nss_n2h_nepbcfgp[core_num].response) {
		goto failure;
	}

	up(&nss_n2h_nepbcfgp[core_num].sem);
	return 0;

failure:
	/*
	 * Restore the current_value to its previous state
	 */
	*new_val = nss_n2h_nepbcfgp[core_num].empty_paged_buf_pool_info.pool_size;
	up(&nss_n2h_nepbcfgp[core_num].sem);
	return NSS_FAILURE;
}

/*
 * nss_n2h_set_water_mark()
 *	Sets water mark for N2H SOS
 */
static int nss_n2h_set_water_mark(struct ctl_table *ctl, int write,
					void __user *buffer,
					size_t *lenp, loff_t *ppos,
					uint32_t core_num, int *low, int *high)
{
	struct nss_top_instance *nss_top = &nss_top_main;
	struct nss_ctx_instance *nss_ctx = &nss_top->nss[core_num];
	struct nss_n2h_msg nnm;
	struct nss_n2h_water_mark *wm;
	nss_tx_status_t nss_tx_status;
	int ret = NSS_FAILURE;

	/*
	 * Acquiring semaphore
	 */
	down(&nss_n2h_nepbcfgp[core_num].sem);

	/*
	 * Take snap shot of current value
	 */
	nss_n2h_nepbcfgp[core_num].empty_buf_pool_info.low_water = *low;
	nss_n2h_nepbcfgp[core_num].empty_buf_pool_info.high_water = *high;

	if (!write || *low == -1 || *high == -1) {
		ret = nss_n2h_get_default_payload_info(core_num);
		if (ret == NSS_FAILURE) {
			up(&nss_n2h_nepbcfgp[core_num].sem);
			return -EBUSY;
		}

		*low = nss_n2h_nepbcfgp[core_num].empty_buf_pool_info.low_water;
		*high = nss_n2h_nepbcfgp[core_num].empty_buf_pool_info.high_water;
	}

	ret = proc_dointvec(ctl, write, buffer, lenp, ppos);
	if (!write || ret) {
		up(&nss_n2h_nepbcfgp[core_num].sem);
		return ret;
	}

	if ((*low < NSS_N2H_MIN_EMPTY_POOL_BUF_SZ) ||
		(*high < NSS_N2H_MIN_EMPTY_POOL_BUF_SZ)) {
		nss_warning("%px: core %d setting %d, %d < min number of buffer",
				nss_ctx, core_num, *low, *high);
		goto failure;
	}

	if ((*low > NSS_N2H_MAX_EMPTY_POOL_BUF_SZ) ||
		(*high > NSS_N2H_MAX_EMPTY_POOL_BUF_SZ)) {
		nss_warning("%px: core %d setting %d, %d is > upper limit",
				nss_ctx, core_num, *low, *high);
		goto failure;
	}

	if (*low > *high) {
		nss_warning("%px: core %d setting low %d is more than high %d",
				nss_ctx, core_num, *low, *high);
		goto failure;
	}

	nss_info("%px: core %d number of low : %d and high : %d\n",
		nss_ctx, core_num, *low, *high);

	nss_n2h_msg_init(&nnm, NSS_N2H_INTERFACE,
			NSS_TX_METADATA_TYPE_SET_WATER_MARK,
			sizeof(struct nss_n2h_water_mark),
			nss_n2h_payload_stats_callback,
			(void *)(nss_ptr_t)core_num);

	wm = &nnm.msg.wm;
	wm->low_water = htonl(*low);
	wm->high_water = htonl(*high);
	nss_tx_status = nss_n2h_tx_msg(nss_ctx, &nnm);

	if (nss_tx_status != NSS_TX_SUCCESS) {
		nss_warning("%px: core %d nss_tx error setting : %d, %d\n",
				nss_ctx, core_num, *low, *high);
		goto failure;
	}

	/*
	 * Blocking call, wait till we get ACK for this msg.
	 */
	ret = wait_for_completion_timeout(&nss_n2h_nepbcfgp[core_num].complete,
			msecs_to_jiffies(NSS_CONN_CFG_TIMEOUT));
	if (ret == 0) {
		nss_warning("%px: core %d Waiting for ack timed out\n", nss_ctx,
			core_num);
		goto failure;
	}

	/*
	 * ACK/NACK received from NSS FW
	 */
	if (NSS_FAILURE == nss_n2h_nepbcfgp[core_num].response)
		goto failure;

	up(&nss_n2h_nepbcfgp[core_num].sem);
	return NSS_SUCCESS;

failure:
	/*
	 * Restore the current_value to its previous state
	 */
	*low = nss_n2h_nepbcfgp[core_num].empty_buf_pool_info.low_water;
	*high = nss_n2h_nepbcfgp[core_num].empty_buf_pool_info.high_water;
	up(&nss_n2h_nepbcfgp[core_num].sem);
	return -EINVAL;
}

/*
 * nss_n2h_set_paged_water_mark()
 *	Sets water mark for paged pool N2H SOS
 */
static int nss_n2h_set_paged_water_mark(struct ctl_table *ctl, int write,
					void __user *buffer,
					size_t *lenp, loff_t *ppos,
					uint32_t core_num, int *low, int *high)
{
	struct nss_top_instance *nss_top = &nss_top_main;
	struct nss_ctx_instance *nss_ctx = &nss_top->nss[core_num];
	struct nss_n2h_msg nnm;
	struct nss_n2h_water_mark *pwm;
	nss_tx_status_t nss_tx_status;
	int ret = NSS_FAILURE;

	/*
	 * Acquiring semaphore
	 */
	down(&nss_n2h_nepbcfgp[core_num].sem);

	/*
	 * Take snap shot of current value
	 */
	nss_n2h_nepbcfgp[core_num].empty_paged_buf_pool_info.low_water = *low;
	nss_n2h_nepbcfgp[core_num].empty_paged_buf_pool_info.high_water = *high;

	if (!write || *low == -1 || *high == -1) {
		ret = nss_n2h_get_paged_payload_info(core_num);
		if (ret == NSS_FAILURE) {
			up(&nss_n2h_nepbcfgp[core_num].sem);
			return -EBUSY;
		}

		*low = nss_n2h_nepbcfgp[core_num].empty_paged_buf_pool_info.low_water;
		*high = nss_n2h_nepbcfgp[core_num].empty_paged_buf_pool_info.high_water;
	}

	ret = proc_dointvec(ctl, write, buffer, lenp, ppos);
	if (!write || ret) {
		up(&nss_n2h_nepbcfgp[core_num].sem);
		return ret;
	}

	if ((*low < NSS_N2H_MIN_EMPTY_POOL_BUF_SZ) ||
		(*high < NSS_N2H_MIN_EMPTY_POOL_BUF_SZ)) {
		nss_warning("%px: core %d setting %d, %d < min number of buffer",
				nss_ctx, core_num, *low, *high);
		goto failure;
	}

	if ((*low > NSS_N2H_MAX_EMPTY_POOL_BUF_SZ) ||
		(*high > NSS_N2H_MAX_EMPTY_POOL_BUF_SZ)) {
		nss_warning("%px: core %d setting %d, %d is > upper limit",
				nss_ctx, core_num, *low, *high);
		goto failure;
	}

	if (*low > *high) {
		nss_warning("%px: core %d setting low %d is more than high %d",
				nss_ctx, core_num, *low, *high);
		goto failure;
	}

	nss_info("%px: core %d number of low : %d and high : %d\n",
		nss_ctx, core_num, *low, *high);

	nss_n2h_msg_init(&nnm, NSS_N2H_INTERFACE,
			NSS_TX_METADATA_TYPE_SET_PAGED_WATER_MARK,
			sizeof(struct nss_n2h_water_mark),
			nss_n2h_payload_stats_callback,
			(void *)(nss_ptr_t)core_num);

	pwm = &nnm.msg.wm_paged;
	pwm->low_water = htonl(*low);
	pwm->high_water = htonl(*high);
	nss_tx_status = nss_n2h_tx_msg(nss_ctx, &nnm);

	if (nss_tx_status != NSS_TX_SUCCESS) {
		nss_warning("%px: core %d nss_tx error setting : %d, %d\n",
				nss_ctx, core_num, *low, *high);
		goto failure;
	}

	/*
	 * Blocking call, wait till we get ACK for this msg.
	 */
	ret = wait_for_completion_timeout(&nss_n2h_nepbcfgp[core_num].complete,
			msecs_to_jiffies(NSS_CONN_CFG_TIMEOUT));
	if (ret == 0) {
		nss_warning("%px: core %d Waiting for ack timed out\n", nss_ctx,
			core_num);
		goto failure;
	}

	/*
	 * ACK/NACK received from NSS FW
	 */
	if (NSS_FAILURE == nss_n2h_nepbcfgp[core_num].response)
		goto failure;

	up(&nss_n2h_nepbcfgp[core_num].sem);
	return NSS_SUCCESS;

failure:
	/*
	 * Restore the current_value to its previous state
	 */
	*low = nss_n2h_nepbcfgp[core_num].empty_paged_buf_pool_info.low_water;
	*high = nss_n2h_nepbcfgp[core_num].empty_paged_buf_pool_info.high_water;
	up(&nss_n2h_nepbcfgp[core_num].sem);
	return -EINVAL;
}

/*
 * nss_n2h_cfg_wifi_pool()
 *	Sets number of wifi payloads to adjust high water mark for N2H SoS
 */
static int nss_n2h_cfg_wifi_pool(struct ctl_table *ctl, int write,
					void __user *buffer,
					size_t *lenp, loff_t *ppos,
					int *payloads)
{
	struct nss_top_instance *nss_top = &nss_top_main;
	struct nss_ctx_instance *nss_ctx = &nss_top->nss[0];
	struct nss_n2h_msg nnm;
	struct nss_n2h_wifi_payloads *wp;
	nss_tx_status_t nss_tx_status;
	int ret = NSS_FAILURE;

	/*
	 * Acquiring semaphore
	 */
	down(&nss_n2h_wp.sem);

	if (!write) {
		*payloads = nss_n2h_wp.wifi_pool;

		up(&nss_n2h_wp.sem);
		ret = proc_dointvec(ctl, write, buffer, lenp, ppos);
		return ret;
	}

	ret = proc_dointvec(ctl, write, buffer, lenp, ppos);
	if (ret) {
		up(&nss_n2h_wp.sem);
		return ret;
	}

	/*
	 * If payloads parameter is not set, we do
	 * nothing.
	 */
	if (*payloads == -1)
		goto failure;

	if ((*payloads < NSS_N2H_MIN_EMPTY_POOL_BUF_SZ)) {
		nss_warning("%px: wifi setting %d < min number of buffer",
				nss_ctx, *payloads);
		goto failure;
	}

	if ((*payloads > NSS_N2H_MAX_EMPTY_POOL_BUF_SZ)) {
		nss_warning("%px: wifi setting %d > max number of buffer",
				nss_ctx, *payloads);
		goto failure;
	}

	nss_info("%px: wifi payloads : %d\n",
		nss_ctx, *payloads);

	nss_n2h_msg_init(&nnm, NSS_N2H_INTERFACE,
			NSS_TX_METADATA_TYPE_N2H_WIFI_POOL_BUF_CFG,
			sizeof(struct nss_n2h_wifi_payloads),
			nss_n2h_set_wifi_payloads_callback,
			(void *)nss_ctx);

	wp = &nnm.msg.wp;
	wp->payloads = htonl(*payloads);
	nss_tx_status = nss_n2h_tx_msg(nss_ctx, &nnm);

	if (nss_tx_status != NSS_TX_SUCCESS) {
		nss_warning("%px: wifi setting %d nss_tx error",
				nss_ctx, *payloads);
		goto failure;
	}

	/*
	 * Blocking call, wait till we get ACK for this msg.
	 */
	ret = wait_for_completion_timeout(&nss_n2h_wp.complete,
			msecs_to_jiffies(NSS_CONN_CFG_TIMEOUT));
	if (ret == 0) {
		nss_warning("%px: Waiting for ack timed out\n", nss_ctx);
		goto failure;
	}

	/*
	 * ACK/NACK received from NSS FW
	 */
	if (NSS_FAILURE == nss_n2h_wp.response)
		goto failure;

	up(&nss_n2h_wp.sem);
	return NSS_SUCCESS;

failure:
	up(&nss_n2h_wp.sem);
	return -EINVAL;
}

/*
 * nss_n2h_empty_pool_buf_core1_handler()
 *	Sets the number of empty buffer for core 1
 */
static int nss_n2h_empty_pool_buf_cfg_core1_handler(struct ctl_table *ctl,
				int write, void __user *buffer,
				size_t *lenp, loff_t *ppos)
{
	return nss_n2h_set_empty_buf_pool(ctl, write, buffer, lenp, ppos,
			NSS_CORE_1, &nss_n2h_empty_pool_buf_cfg[NSS_CORE_1]);
}

/*
 * nss_n2h_empty_pool_buf_core0_handler()
 *	Sets the number of empty buffer for core 0
 */
static int nss_n2h_empty_pool_buf_cfg_core0_handler(struct ctl_table *ctl,
				int write, void __user *buffer,
				size_t *lenp, loff_t *ppos)
{
	return nss_n2h_set_empty_buf_pool(ctl, write, buffer, lenp, ppos,
			NSS_CORE_0, &nss_n2h_empty_pool_buf_cfg[NSS_CORE_0]);
}

/*
 * nss_n2h_empty_paged_pool_buf_cfg_core1_handler()
 *	Sets the number of empty paged buffer for core 1
 */
static int nss_n2h_empty_paged_pool_buf_cfg_core1_handler(struct ctl_table *ctl,
				int write, void __user *buffer,
				size_t *lenp, loff_t *ppos)
{
	return nss_n2h_set_empty_paged_pool_buf(ctl, write, buffer, lenp, ppos,
			NSS_CORE_1, &nss_n2h_empty_paged_pool_buf_cfg[NSS_CORE_1]);
}

/*
 * nss_n2h_empty_paged_pool_buf_cfg_core0_handler()
 *	Sets the number of empty paged buffer for core 0
 */
static int nss_n2h_empty_paged_pool_buf_cfg_core0_handler(struct ctl_table *ctl,
				int write, void __user *buffer,
				size_t *lenp, loff_t *ppos)
{
	return nss_n2h_set_empty_paged_pool_buf(ctl, write, buffer, lenp, ppos,
			NSS_CORE_0, &nss_n2h_empty_paged_pool_buf_cfg[NSS_CORE_0]);
}

/*
 * nss_n2h_water_mark_core1_handler()
 *	Sets water mark for core 1
 */
static int nss_n2h_water_mark_core1_handler(struct ctl_table *ctl,
			int write, void __user *buffer,
			size_t *lenp, loff_t *ppos)
{
	return nss_n2h_set_water_mark(ctl, write, buffer, lenp, ppos,
			NSS_CORE_1, &nss_n2h_water_mark[NSS_CORE_1][0],
			&nss_n2h_water_mark[NSS_CORE_1][1]);
}

/*
 * nss_n2h_water_mark_core0_handler()
 *	Sets water mark for core 0
 */
static int nss_n2h_water_mark_core0_handler(struct ctl_table *ctl,
			int write, void __user *buffer,
			size_t *lenp, loff_t *ppos)
{
	return nss_n2h_set_water_mark(ctl, write, buffer, lenp, ppos,
			NSS_CORE_0, &nss_n2h_water_mark[NSS_CORE_0][0],
			&nss_n2h_water_mark[NSS_CORE_0][1]);
}

/*
 * nss_n2h_paged_water_mark_core1_handler()
 *	Sets paged water mark for core 1
 */
static int nss_n2h_paged_water_mark_core1_handler(struct ctl_table *ctl,
			int write, void __user *buffer,
			size_t *lenp, loff_t *ppos)
{
	return nss_n2h_set_paged_water_mark(ctl, write, buffer, lenp, ppos,
			NSS_CORE_1, &nss_n2h_paged_water_mark[NSS_CORE_1][0],
			&nss_n2h_paged_water_mark[NSS_CORE_1][1]);
}

/*
 * nss_n2h_paged_water_mark_core0_handler()
 *	Sets paged water mark for core 0
 */
static int nss_n2h_paged_water_mark_core0_handler(struct ctl_table *ctl,
			int write, void __user *buffer,
			size_t *lenp, loff_t *ppos)
{
	return nss_n2h_set_paged_water_mark(ctl, write, buffer, lenp, ppos,
			NSS_CORE_0, &nss_n2h_paged_water_mark[NSS_CORE_0][0],
			&nss_n2h_paged_water_mark[NSS_CORE_0][1]);
}

/*
 * nss_n2h_wifi_payloads_handler()
 *	Sets number of wifi payloads
 */
static int nss_n2h_wifi_payloads_handler(struct ctl_table *ctl,
			int write, void __user *buffer,
			size_t *lenp, loff_t *ppos)
{
	return nss_n2h_cfg_wifi_pool(ctl, write, buffer, lenp, ppos,
			&nss_n2h_wifi_pool_buf_cfg);
}

/*
 * nss_n2h_update_queue_config_callback()
 *	Callback to handle the completion of queue config command
 */
static void nss_n2h_update_queue_config_callback(void *app_data, struct nss_n2h_msg *nim)
{
	if (nim->cm.response != NSS_CMN_RESPONSE_ACK) {
		nss_warning("n2h Error response %d\n", nim->cm.response);
		nss_n2h_q_cfg_pvt.response = NSS_TX_FAILURE;
	} else {
		nss_n2h_q_cfg_pvt.response = NSS_TX_SUCCESS;
	}

	complete(&nss_n2h_q_cfg_pvt.complete);
}

/*
 * nss_n2h_update_queue_config_async()
 *	Asynchronous call to send pnode queue configuration.
 */
nss_tx_status_t nss_n2h_update_queue_config_async(struct nss_ctx_instance *nss_ctx, bool mq_en, uint16_t *qlimits)
{

	struct nss_n2h_msg nnm;
	struct nss_n2h_pnode_queue_config *cfg;
	nss_tx_status_t status;
	int i;

	if (!mq_en) {
		return NSS_TX_SUCCESS;
	}

	/*
	 * MQ mode doesnot make any sense if number of priority queues in NSS
	 * is 1
	 */
	if (NSS_MAX_NUM_PRI <= 1) {
		return NSS_TX_SUCCESS;
	}

	memset(&nnm, 0, sizeof(struct nss_n2h_msg));

	nss_n2h_msg_init(&nnm, NSS_N2H_INTERFACE,
			 NSS_TX_METADATA_TYPE_N2H_SET_PNODE_QUEUE_CFG,
			 sizeof(struct nss_n2h_pnode_queue_config), NULL, 0);

	cfg = &nnm.msg.pn_q_cfg;

	/*
	 * Update limits
	 */
	for (i = 0; i < NSS_MAX_NUM_PRI; i++) {
		cfg->qlimits[i] = qlimits[i];
	}
	cfg->mq_en = true;

	status = nss_n2h_tx_msg(nss_ctx, &nnm);
	if (status != NSS_TX_SUCCESS) {
		nss_warning("%px: nss_tx error to send pnode queue config\n", nss_ctx);
		return status;
	}

	return NSS_TX_SUCCESS;
}
EXPORT_SYMBOL(nss_n2h_update_queue_config_async);

/*
 * nss_n2h_update_queue_config_sync()
 *	Synchronous call to send pnode queue configuration.
 */
nss_tx_status_t nss_n2h_update_queue_config_sync(struct nss_ctx_instance *nss_ctx, bool mq_en, uint16_t *qlimits)
{

	struct nss_n2h_msg nnm;
	struct nss_n2h_pnode_queue_config *cfg;
	nss_tx_status_t status;
	int ret, i;

	if (!mq_en) {
		return NSS_TX_SUCCESS;
	}

	/*
	 * MQ mode doesnot make any sense if number of priority queues in NSS
	 * is 1
	 */
	if (NSS_MAX_NUM_PRI <= 1) {
		return NSS_TX_SUCCESS;
	}

	memset(&nnm, 0, sizeof(struct nss_n2h_msg));

	nss_n2h_msg_init(&nnm, NSS_N2H_INTERFACE,
			 NSS_TX_METADATA_TYPE_N2H_SET_PNODE_QUEUE_CFG,
			 sizeof(struct nss_n2h_pnode_queue_config), nss_n2h_update_queue_config_callback, 0);

	cfg = &nnm.msg.pn_q_cfg;

	/*
	 * Update limits
	 */
	for (i = 0; i < NSS_MAX_NUM_PRI; i++) {
		cfg->qlimits[i] = qlimits[i];
	}
	cfg->mq_en = true;

	down(&nss_n2h_q_cfg_pvt.sem);

	status = nss_n2h_tx_msg(nss_ctx, &nnm);

	if (status != NSS_TX_SUCCESS) {
		nss_warning("%px: n2h_tx_msg failed\n", nss_ctx);
		up(&nss_n2h_q_cfg_pvt.sem);
		return status;
	}
	ret = wait_for_completion_timeout(&nss_n2h_q_cfg_pvt.complete, msecs_to_jiffies(NSS_N2H_TX_TIMEOUT));

	if (!ret) {
		nss_warning("%px: Timeout expired for pnode queue config sync message\n", nss_ctx);
		nss_n2h_q_cfg_pvt.response = NSS_TX_FAILURE;
	}

	status = nss_n2h_q_cfg_pvt.response;
	up(&nss_n2h_q_cfg_pvt.sem);
	return status;
}
EXPORT_SYMBOL(nss_n2h_update_queue_config_sync);

/*
 * nss_n2h_mitigation_cfg()
 *	Send Message to NSS to disable MITIGATION.
 */
static nss_tx_status_t nss_n2h_mitigation_cfg(struct nss_ctx_instance *nss_ctx, int enable_mitigation, nss_core_id_t core_num)
{
	struct nss_n2h_msg nnm;
	struct nss_n2h_mitigation *mitigation_cfg;
	nss_tx_status_t nss_tx_status;
	int ret;

	nss_assert(core_num < NSS_CORE_MAX);

	down(&nss_n2h_mitigationcp[core_num].sem);
	nss_n2h_msg_init(&nnm, NSS_N2H_INTERFACE, NSS_TX_METADATA_TYPE_N2H_MITIGATION_CFG,
			sizeof(struct nss_n2h_mitigation),
			nss_n2h_mitigation_cfg_callback,
			(void *)core_num);

	mitigation_cfg = &nnm.msg.mitigation_cfg;
	mitigation_cfg->enable = enable_mitigation;

	nss_tx_status = nss_n2h_tx_msg(nss_ctx, &nnm);

	if (nss_tx_status != NSS_TX_SUCCESS) {
		nss_warning("%px: nss_tx error setting mitigation\n", nss_ctx);
		goto failure;
	}

	/*
	 * Blocking call, wait till we get ACK for this msg.
	 */
	ret = wait_for_completion_timeout(&nss_n2h_mitigationcp[core_num].complete, msecs_to_jiffies(NSS_CONN_CFG_TIMEOUT));
	if (ret == 0) {
		nss_warning("%px: Waiting for ack timed out\n", nss_ctx);
		goto failure;
	}

	/*
	 * ACK/NACK received from NSS FW
	 */
	if (NSS_FAILURE == nss_n2h_mitigationcp[core_num].response) {
		goto failure;
	}

	up(&nss_n2h_mitigationcp[core_num].sem);
	return NSS_SUCCESS;

failure:
	up(&nss_n2h_mitigationcp[core_num].sem);
	return NSS_FAILURE;
}

static inline void nss_n2h_buf_pool_free(struct nss_n2h_buf_pool *buf_pool)
{
	int page_count;
	for (page_count = 0; page_count < buf_pool->nss_buf_num_pages; page_count++) {
		kfree((void *)buf_pool->nss_buf_pool_vaddr[page_count]);
	}
}

/*
 * nss_n2h_buf_cfg()
 *	Send Message to NSS to enable pbufs.
 */
static nss_tx_status_t nss_n2h_buf_pool_cfg(struct nss_ctx_instance *nss_ctx,
					int buf_pool_size, nss_core_id_t core_num)
{
	static struct nss_n2h_msg nnm;
	struct nss_n2h_buf_pool *buf_pool;
	nss_tx_status_t nss_tx_status;
	int ret;
	int page_count;
	int num_pages = ALIGN(buf_pool_size, PAGE_SIZE)/PAGE_SIZE;

	nss_assert(core_num < NSS_CORE_MAX);

	nss_n2h_msg_init(&nnm, NSS_N2H_INTERFACE, NSS_METADATA_TYPE_N2H_ADD_BUF_POOL,
			sizeof(struct nss_n2h_buf_pool),
			nss_n2h_bufs_cfg_callback,
			(void *)core_num);

	do {

		down(&nss_n2h_bufcp[core_num].sem);

		buf_pool = &nnm.msg.buf_pool;
		buf_pool->nss_buf_page_size = PAGE_SIZE;

		for (page_count = 0; page_count < MAX_PAGES_PER_MSG && num_pages; page_count++, num_pages--) {
			void *kern_addr = kzalloc(PAGE_SIZE, GFP_ATOMIC);
			if (!kern_addr) {
				BUG_ON(!page_count);
				break;
			}

			kmemleak_not_leak(kern_addr);
			buf_pool->nss_buf_pool_vaddr[page_count] = (nss_ptr_t)kern_addr;
			buf_pool->nss_buf_pool_addr[page_count] = dma_map_single(nss_ctx->dev, kern_addr, PAGE_SIZE, DMA_TO_DEVICE);
		}

		buf_pool->nss_buf_num_pages = page_count;
		nss_tx_status = nss_n2h_tx_msg(nss_ctx, &nnm);
		if (nss_tx_status != NSS_TX_SUCCESS) {

			nss_n2h_buf_pool_free(buf_pool);
			nss_warning("%px: nss_tx error setting pbuf\n", nss_ctx);
			goto failure;
		}

		/*
		 * Blocking call, wait till we get ACK for this msg.
		 */
		ret = wait_for_completion_timeout(&nss_n2h_bufcp[core_num].complete, msecs_to_jiffies(NSS_CONN_CFG_TIMEOUT));
		if (ret == 0) {
			nss_warning("%px: Waiting for ack timed out\n", nss_ctx);
			goto failure;
		}

		/*
		 * ACK/NACK received from NSS FW
		 */
		if (NSS_FAILURE == nss_n2h_bufcp[core_num].response) {

			nss_n2h_buf_pool_free(buf_pool);
			goto failure;
		}

		up(&nss_n2h_bufcp[core_num].sem);
	} while(num_pages);

	return NSS_SUCCESS;
failure:
	up(&nss_n2h_bufcp[core_num].sem);
	return NSS_FAILURE;
}

/*
 * nss_mitigation_handler()
 * Enable NSS MITIGATION
 */
static int nss_n2h_mitigationcfg_core0_handler(struct ctl_table *ctl, int write, void __user *buffer, size_t *lenp, loff_t *ppos)
{
	struct nss_top_instance *nss_top = &nss_top_main;
	struct nss_ctx_instance *nss_ctx = &nss_top->nss[NSS_CORE_0];
	int ret;

	ret = proc_dointvec(ctl, write, buffer, lenp, ppos);
	if (ret) {
		return ret;
	}

	/*
	 * It's a read operation
	 */
	if (!write) {
		return ret;
	}

	if (!nss_n2h_core0_mitigation_cfg) {
		printk(KERN_INFO "Disabling NSS MITIGATION\n");
		nss_n2h_mitigation_cfg(nss_ctx, 0, NSS_CORE_0);
		return 0;
	}
	printk(KERN_INFO "Invalid input value.Valid value is 0, Runtime re-enabling not supported\n");
	return -EINVAL;
}

/*
 * nss_mitigation_handler()
 * Enable NSS MITIGATION
 */
static int nss_n2h_mitigationcfg_core1_handler(struct ctl_table *ctl, int write, void __user *buffer, size_t *lenp, loff_t *ppos)
{
	struct nss_top_instance *nss_top = &nss_top_main;
	struct nss_ctx_instance *nss_ctx = &nss_top->nss[NSS_CORE_1];
	int ret;

	ret = proc_dointvec(ctl, write, buffer, lenp, ppos);
	if (ret) {
		return ret;
	}

	/*
	 * It's a read operation
	 */
	if (!write) {
		return ret;
	}

	if (!nss_n2h_core1_mitigation_cfg) {
		printk(KERN_INFO "Disabling NSS MITIGATION\n");
		nss_n2h_mitigation_cfg(nss_ctx, 0, NSS_CORE_1);
		return 0;
	}
	printk(KERN_INFO "Invalid input value.Valid value is 0, Runtime re-enabling not supported\n");
	return -EINVAL;
}

/*
 * nss_buf_handler()
 *	Add extra NSS bufs from host memory
 */
static int nss_n2h_buf_cfg_core0_handler(struct ctl_table *ctl, int write, void __user *buffer, size_t *lenp, loff_t *ppos)
{
	struct nss_top_instance *nss_top = &nss_top_main;
	struct nss_ctx_instance *nss_ctx = &nss_top->nss[NSS_CORE_0];
	int ret;

	ret = proc_dointvec(ctl, write, buffer, lenp, ppos);
	if (ret) {
		return ret;
	}

	/*
	 * It's a read operation
	 */
	if (!write) {
		return ret;
	}

	if (nss_ctx->buf_sz_allocated) {
		nss_n2h_core0_add_buf_pool_size = nss_ctx->buf_sz_allocated;
		return -EPERM;
	}

	if ((nss_n2h_core0_add_buf_pool_size >= 1) && (nss_n2h_core0_add_buf_pool_size <= NSS_N2H_MAX_BUF_POOL_SIZE)) {
		printk(KERN_INFO "configuring additional NSS pbufs\n");
		ret = nss_n2h_buf_pool_cfg(nss_ctx, nss_n2h_core0_add_buf_pool_size, NSS_CORE_0);
		nss_n2h_core0_add_buf_pool_size = nss_ctx->buf_sz_allocated;
		printk(KERN_INFO "additional pbufs of size %d got added to NSS\n", nss_ctx->buf_sz_allocated);
		return ret;
	}

	printk(KERN_INFO "Invalid input value. should be greater than 1 and less than %d\n", NSS_N2H_MAX_BUF_POOL_SIZE);
	return -EINVAL;
}

/*
 * nss_n2h_buf_handler()
 *	Add extra NSS bufs from host memory
 */
static int nss_n2h_buf_cfg_core1_handler(struct ctl_table *ctl, int write, void __user *buffer, size_t *lenp, loff_t *ppos)
{
	struct nss_top_instance *nss_top = &nss_top_main;
	struct nss_ctx_instance *nss_ctx = &nss_top->nss[NSS_CORE_1];
	int ret;

	ret = proc_dointvec(ctl, write, buffer, lenp, ppos);
	if (ret) {
		return ret;
	}

	/*
	 * It's a read operation
	 */
	if (!write) {
		return ret;
	}

	if (nss_ctx->buf_sz_allocated) {
		nss_n2h_core1_add_buf_pool_size = nss_ctx->buf_sz_allocated;
		return -EPERM;
	}

	if ((nss_n2h_core1_add_buf_pool_size >= 1) && (nss_n2h_core1_add_buf_pool_size <= NSS_N2H_MAX_BUF_POOL_SIZE)) {
		printk(KERN_INFO "configuring additional NSS pbufs\n");
		ret = nss_n2h_buf_pool_cfg(nss_ctx, nss_n2h_core1_add_buf_pool_size, NSS_CORE_1);
		nss_n2h_core1_add_buf_pool_size = nss_ctx->buf_sz_allocated;
		printk(KERN_INFO "additional pbufs of size %d got added to NSS\n", nss_ctx->buf_sz_allocated);
		return ret;
	}

	printk(KERN_INFO "Invalid input value. should be greater than 1 and less than %d\n", NSS_N2H_MAX_BUF_POOL_SIZE);
	return -EINVAL;
}

/*
 * nss_n2h_queue_limit_callback()
 *	Callback to handle the completion of queue limit command.
 */
static void nss_n2h_queue_limit_callback(void *app_data, struct nss_n2h_msg *nim)
{
	if (nim->cm.response != NSS_CMN_RESPONSE_ACK) {
		nss_warning("n2h error response %d\n", nim->cm.response);
	}

	nss_n2h_q_lim_pvt.response = nim->cm.response;
	complete(&nss_n2h_q_lim_pvt.complete);
}

/*
 * nss_n2h_set_queue_limit_sync()
 *	Sets the n2h queue size limit synchronously.
 */
static int nss_n2h_set_queue_limit_sync(struct ctl_table *ctl, int write, void __user *buffer,
					size_t *lenp, loff_t *ppos, uint32_t core_id)
{
	struct nss_top_instance *nss_top = &nss_top_main;
	struct nss_ctx_instance *nss_ctx = &nss_top->nss[core_id];
	struct nss_n2h_msg nim;
	struct nss_n2h_queue_limit_config *nnqlc = NULL;
	int ret, current_val;
	nss_tx_status_t nss_tx_status;

	/*
	 * Take a snap shot of current value
	 */
	current_val = nss_n2h_queue_limit[core_id];

	/*
	 * Write the variable with user input
	 */
	ret = proc_dointvec(ctl, write, buffer, lenp, ppos);
	if (ret || (!write)) {
		return ret;
	}

	/*
	 * We dont allow shortening of the queue size at run-time
	 */
	if (nss_n2h_queue_limit[core_id] < current_val) {
		nss_warning("%px: New queue limit %d less than previous value %d. Cant allow shortening\n",
				nss_ctx, nss_n2h_queue_limit[core_id], current_val);
		nss_n2h_queue_limit[core_id] = current_val;
		return NSS_TX_FAILURE;
	}

	memset(&nim, 0, sizeof(struct nss_n2h_msg));
	nss_n2h_msg_init(&nim, NSS_N2H_INTERFACE,
			NSS_TX_METADATA_TYPE_N2H_QUEUE_LIMIT_CFG,
			sizeof(struct nss_n2h_queue_limit_config), nss_n2h_queue_limit_callback, NULL);

	nnqlc = &nim.msg.ql_cfg;
	nnqlc->qlimit = nss_n2h_queue_limit[core_id];

	/*
	 * Send synchronous message to firmware
	 */
	down(&nss_n2h_q_lim_pvt.sem);

	nss_tx_status = nss_n2h_tx_msg(nss_ctx, &nim);
	if (nss_tx_status != NSS_TX_SUCCESS) {
		nss_warning("%px: n2h queue limit message send failed\n", nss_ctx);
		nss_n2h_queue_limit[core_id] = current_val;
		up(&nss_n2h_q_lim_pvt.sem);
		return nss_tx_status;
	}

	ret = wait_for_completion_timeout(&nss_n2h_q_lim_pvt.complete, msecs_to_jiffies(NSS_N2H_TX_TIMEOUT));
	if (!ret) {
		nss_warning("%px: Timeout expired for queue limit sync message\n", nss_ctx);
		nss_n2h_queue_limit[core_id] = current_val;
		up(&nss_n2h_q_lim_pvt.sem);
		return NSS_TX_FAILURE;
	}

	/*
	 * If setting the queue limit failed, reset the value to original value
	 */
	if (nss_n2h_q_lim_pvt.response != NSS_CMN_RESPONSE_ACK) {
		nss_n2h_queue_limit[core_id] = current_val;
	}

	up(&nss_n2h_q_lim_pvt.sem);
	return NSS_TX_SUCCESS;
}

/*
 * nss_n2h_queue_limit_core0_handler()
 *	Sets the n2h queue size limit for core0
 */
static int nss_n2h_queue_limit_core0_handler(struct ctl_table *ctl,
				int write, void __user *buffer,
				size_t *lenp, loff_t *ppos)
{
	return nss_n2h_set_queue_limit_sync(ctl, write, buffer, lenp, ppos,
			NSS_CORE_0);
}

/*
 * nss_n2h_queue_limit_core1_handler()
 *	Sets the n2h queue size limit for core1
 */
static int nss_n2h_queue_limit_core1_handler(struct ctl_table *ctl,
				int write, void __user *buffer,
				size_t *lenp, loff_t *ppos)
{
	return nss_n2h_set_queue_limit_sync(ctl, write, buffer, lenp, ppos,
			NSS_CORE_1);
}

/*
 * nss_n2h_host_bp_cfg_callback()
 *	Callback function for back pressure configuration.
 */
static void nss_n2h_host_bp_cfg_callback(void *app_data, struct nss_n2h_msg *nnm)
{
	struct nss_ctx_instance *nss_ctx __maybe_unused = (struct nss_ctx_instance *)app_data;
	if (nnm->cm.response != NSS_CMN_RESPONSE_ACK) {
		nss_n2h_host_bp_cfg_pvt.response = NSS_FAILURE;
		complete(&nss_n2h_host_bp_cfg_pvt.complete);
		nss_warning("%px: n2h back pressure configuration failed : %d\n", nss_ctx, nnm->cm.error);
		return;
	}

	nss_info("%px: n2h back pressure configuration succeeded: %d\n", nss_ctx, nnm->cm.error);
	nss_n2h_host_bp_cfg_pvt.response = NSS_SUCCESS;
	complete(&nss_n2h_host_bp_cfg_pvt.complete);
}

/*
 * nss_n2h_host_bp_cfg()
 *	Send Message to n2h to enable back pressure.
 */
static nss_tx_status_t nss_n2h_host_bp_cfg_sync(struct nss_ctx_instance *nss_ctx, int enable_bp)
{
	struct nss_n2h_msg nnm;
	nss_tx_status_t nss_tx_status;
	int ret;

	down(&nss_n2h_host_bp_cfg_pvt.sem);
	nss_n2h_msg_init(&nnm, NSS_N2H_INTERFACE, NSS_TX_METADATA_TYPE_N2H_HOST_BACK_PRESSURE_CFG,
			sizeof(struct nss_n2h_host_back_pressure),
			nss_n2h_host_bp_cfg_callback,
			(void *)nss_ctx);

	nnm.msg.host_bp_cfg.enable = enable_bp;

	nss_tx_status = nss_n2h_tx_msg(nss_ctx, &nnm);
	if (nss_tx_status != NSS_TX_SUCCESS) {
		nss_warning("%px: nss_tx error setting back pressure\n", nss_ctx);
		up(&nss_n2h_host_bp_cfg_pvt.sem);
		return NSS_FAILURE;
	}

	/*
	 * Blocking call, wait till we get ACK for this msg.
	 */
	ret = wait_for_completion_timeout(&nss_n2h_host_bp_cfg_pvt.complete, msecs_to_jiffies(NSS_CONN_CFG_TIMEOUT));
	if (ret == 0) {
		nss_warning("%px: Waiting for ack timed out\n", nss_ctx);
		up(&nss_n2h_host_bp_cfg_pvt.sem);
		return NSS_FAILURE;
	}

	/*
	 * Response received from NSS FW
	 */
	if (nss_n2h_host_bp_cfg_pvt.response == NSS_FAILURE) {
		up(&nss_n2h_host_bp_cfg_pvt.sem);
		return NSS_FAILURE;
	}

	up(&nss_n2h_host_bp_cfg_pvt.sem);
	return NSS_SUCCESS;
}

/*
 * nss_n2h_host_bp_cfg_handler()
 *	Enable n2h back pressure.
 */
static int nss_n2h_host_bp_cfg_handler(struct ctl_table *ctl, int write,
				void __user *buffer, size_t *lenp, loff_t *ppos, uint32_t core_id)
{
	struct nss_top_instance *nss_top = &nss_top_main;
	struct nss_ctx_instance *nss_ctx = &nss_top->nss[core_id];
	int ret, ret_bp, current_state;
	current_state = nss_n2h_host_bp_config[core_id];
	ret = proc_dointvec(ctl, write, buffer, lenp, ppos);

	if (ret != NSS_SUCCESS) {
		return ret;
	}

	if (!write) {
		return ret;
	}

	if ((nss_n2h_host_bp_config[core_id] != 0) && (nss_n2h_host_bp_config[core_id] != 1)) {
		nss_info_always("Invalid input value. Valid values are 0 and 1\n");
		nss_n2h_host_bp_config[core_id] = current_state;
		return ret;
	}

	nss_info("Configuring n2h back pressure\n");
	ret_bp = nss_n2h_host_bp_cfg_sync(nss_ctx, nss_n2h_host_bp_config[core_id]);

	if (ret_bp != NSS_SUCCESS) {
		nss_warning("%px: n2h back pressure config failed\n", nss_ctx);
		nss_n2h_host_bp_config[core_id] = current_state;
	}

	return ret_bp;
}

/*
 * nss_n2h_host_bp_cfg_core0_handler()
 *	Enable n2h back pressure in core 0.
 */
static int nss_n2h_host_bp_cfg_core0_handler(struct ctl_table *ctl, int write,
				void __user *buffer, size_t *lenp, loff_t *ppos)
{
	return nss_n2h_host_bp_cfg_handler(ctl, write, buffer, lenp, ppos, NSS_CORE_0);
}

/*
 * nss_n2h_host_bp_cfg_core1_handler()
 *	Enable n2h back pressure in core 1.
 */
static int nss_n2h_host_bp_cfg_core1_handler(struct ctl_table *ctl, int write,
				void __user *buffer, size_t *lenp, loff_t *ppos)
{
	return nss_n2h_host_bp_cfg_handler(ctl, write, buffer, lenp, ppos, NSS_CORE_1);
}

static struct ctl_table nss_n2h_table_single_core[] = {
	{
		.procname	= "n2h_empty_pool_buf_core0",
		.data		= &nss_n2h_empty_pool_buf_cfg[NSS_CORE_0],
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= &nss_n2h_empty_pool_buf_cfg_core0_handler,
	},
	{
		.procname	= "n2h_empty_paged_pool_buf_core0",
		.data		= &nss_n2h_empty_paged_pool_buf_cfg[NSS_CORE_0],
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= &nss_n2h_empty_paged_pool_buf_cfg_core0_handler,
	},
	{
		.procname	= "n2h_low_water_core0",
		.data		= &nss_n2h_water_mark[NSS_CORE_0][0],
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= &nss_n2h_water_mark_core0_handler,
	},
	{
		.procname	= "n2h_high_water_core0",
		.data		= &nss_n2h_water_mark[NSS_CORE_0][1],
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= &nss_n2h_water_mark_core0_handler,
	},
	{
		.procname	= "n2h_paged_low_water_core0",
		.data		= &nss_n2h_paged_water_mark[NSS_CORE_0][0],
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= &nss_n2h_paged_water_mark_core0_handler,
	},
	{
		.procname	= "n2h_paged_high_water_core0",
		.data		= &nss_n2h_paged_water_mark[NSS_CORE_0][1],
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= &nss_n2h_paged_water_mark_core0_handler,
	},
	{
		.procname	= "n2h_wifi_pool_buf",
		.data		= &nss_n2h_wifi_pool_buf_cfg,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= &nss_n2h_wifi_payloads_handler,
	},
	{
		.procname	= "mitigation_core0",
		.data		= &nss_n2h_core0_mitigation_cfg,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= &nss_n2h_mitigationcfg_core0_handler,
	},
	{
		.procname	= "extra_pbuf_core0",
		.data		= &nss_n2h_core0_add_buf_pool_size,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= &nss_n2h_buf_cfg_core0_handler,
	},
	{
		.procname	= "n2h_queue_limit_core0",
		.data		= &nss_n2h_queue_limit[NSS_CORE_0],
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= &nss_n2h_queue_limit_core0_handler,
	},
	{
		.procname	= "host_bp_enable0",
		.data		= &nss_n2h_host_bp_config[NSS_CORE_0],
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= &nss_n2h_host_bp_cfg_core0_handler,
	},

	{ }
};

static struct ctl_table nss_n2h_table_multi_core[] = {
	{
		.procname	= "n2h_empty_pool_buf_core0",
		.data		= &nss_n2h_empty_pool_buf_cfg[NSS_CORE_0],
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= &nss_n2h_empty_pool_buf_cfg_core0_handler,
	},
	{
		.procname	= "n2h_empty_pool_buf_core1",
		.data		= &nss_n2h_empty_pool_buf_cfg[NSS_CORE_1],
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= &nss_n2h_empty_pool_buf_cfg_core1_handler,
	},
	{
		.procname	= "n2h_empty_paged_pool_buf_core0",
		.data		= &nss_n2h_empty_paged_pool_buf_cfg[NSS_CORE_0],
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= &nss_n2h_empty_paged_pool_buf_cfg_core0_handler,
	},
	{
		.procname	= "n2h_empty_paged_pool_buf_core1",
		.data		= &nss_n2h_empty_paged_pool_buf_cfg[NSS_CORE_1],
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= &nss_n2h_empty_paged_pool_buf_cfg_core1_handler,
	},

	{
		.procname	= "n2h_low_water_core0",
		.data		= &nss_n2h_water_mark[NSS_CORE_0][0],
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= &nss_n2h_water_mark_core0_handler,
	},
	{
		.procname	= "n2h_low_water_core1",
		.data		= &nss_n2h_water_mark[NSS_CORE_1][0],
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= &nss_n2h_water_mark_core1_handler,
	},
	{
		.procname	= "n2h_high_water_core0",
		.data		= &nss_n2h_water_mark[NSS_CORE_0][1],
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= &nss_n2h_water_mark_core0_handler,
	},
	{
		.procname	= "n2h_high_water_core1",
		.data		= &nss_n2h_water_mark[NSS_CORE_1][1],
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= &nss_n2h_water_mark_core1_handler,
	},
	{
		.procname	= "n2h_paged_low_water_core0",
		.data		= &nss_n2h_paged_water_mark[NSS_CORE_0][0],
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= &nss_n2h_paged_water_mark_core0_handler,
	},
	{
		.procname	= "n2h_paged_low_water_core1",
		.data		= &nss_n2h_paged_water_mark[NSS_CORE_1][0],
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= &nss_n2h_paged_water_mark_core1_handler,
	},
	{
		.procname	= "n2h_paged_high_water_core0",
		.data		= &nss_n2h_paged_water_mark[NSS_CORE_0][1],
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= &nss_n2h_paged_water_mark_core0_handler,
	},
	{
		.procname	= "n2h_paged_high_water_core1",
		.data		= &nss_n2h_paged_water_mark[NSS_CORE_1][1],
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= &nss_n2h_paged_water_mark_core1_handler,
	},
	{
		.procname	= "n2h_wifi_pool_buf",
		.data		= &nss_n2h_wifi_pool_buf_cfg,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= &nss_n2h_wifi_payloads_handler,
	},
	{
		.procname	= "mitigation_core0",
		.data		= &nss_n2h_core0_mitigation_cfg,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= &nss_n2h_mitigationcfg_core0_handler,
	},
	{
		.procname	= "mitigation_core1",
		.data		= &nss_n2h_core1_mitigation_cfg,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= &nss_n2h_mitigationcfg_core1_handler,
	},
	{
		.procname	= "extra_pbuf_core0",
		.data		= &nss_n2h_core0_add_buf_pool_size,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= &nss_n2h_buf_cfg_core0_handler,
	},
	{
		.procname	= "extra_pbuf_core1",
		.data		= &nss_n2h_core1_add_buf_pool_size,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= &nss_n2h_buf_cfg_core1_handler,
	},
	{
		.procname	= "n2h_queue_limit_core0",
		.data		= &nss_n2h_queue_limit[NSS_CORE_0],
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= &nss_n2h_queue_limit_core0_handler,
	},
	{
		.procname	= "n2h_queue_limit_core1",
		.data		= &nss_n2h_queue_limit[NSS_CORE_1],
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= &nss_n2h_queue_limit_core1_handler,
	},
	{
		.procname	= "host_bp_enable0",
		.data		= &nss_n2h_host_bp_config[NSS_CORE_0],
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= &nss_n2h_host_bp_cfg_core0_handler,
	},
	{
		.procname	= "host_bp_enable1",
		.data		= &nss_n2h_host_bp_config[NSS_CORE_1],
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= &nss_n2h_host_bp_cfg_core1_handler,
	},
	{ }
};

/*
 * This table will be overwritten during single-core registration
 */
static struct ctl_table nss_n2h_dir[] = {
	{
		.procname		= "n2hcfg",
		.mode			= 0555,
		.child			= nss_n2h_table_multi_core,
	},
	{ }
};

static struct ctl_table nss_n2h_root_dir[] = {
	{
		.procname		= "nss",
		.mode			= 0555,
		.child			= nss_n2h_dir,
	},
	{ }
};

static struct ctl_table nss_n2h_root[] = {
	{
		.procname		= "dev",
		.mode			= 0555,
		.child			= nss_n2h_root_dir,
	},
	{ }
};

static struct ctl_table_header *nss_n2h_header;

/*
 * nss_n2h_cfg_empty_pool_size()
 *	Config empty buffer pool
 */
nss_tx_status_t nss_n2h_cfg_empty_pool_size(struct nss_ctx_instance *nss_ctx, uint32_t pool_sz)
{
	struct nss_n2h_msg nnm;
	struct nss_n2h_empty_pool_buf *nnepbcm;
	nss_tx_status_t nss_tx_status;

	if (pool_sz < NSS_N2H_MIN_EMPTY_POOL_BUF_SZ) {
		nss_warning("%px: setting pool size %d < min number of buffer",
				nss_ctx, pool_sz);
		return NSS_TX_FAILURE;
	}

	if (pool_sz > NSS_N2H_MAX_EMPTY_POOL_BUF_SZ) {
		nss_warning("%px: setting pool size %d > max number of buffer",
				nss_ctx, pool_sz);
		return NSS_TX_FAILURE;
	}

	nss_info("%px: update number of empty buffer pool size: %d\n",
		nss_ctx, pool_sz);

	nss_n2h_msg_init(&nnm, NSS_N2H_INTERFACE,
			NSS_TX_METADATA_TYPE_N2H_EMPTY_POOL_BUF_CFG,
			sizeof(struct nss_n2h_empty_pool_buf), NULL, 0);

	nnepbcm = &nnm.msg.empty_pool_buf_cfg;
	nnepbcm->pool_size = htonl(pool_sz);
	nss_tx_status = nss_n2h_tx_msg(nss_ctx, &nnm);

	if (nss_tx_status != NSS_TX_SUCCESS) {
		nss_warning("%px: nss_tx error empty buffer pool: %d\n", nss_ctx, pool_sz);
		return nss_tx_status;
	}

	return nss_tx_status;
}

/*
 * nss_n2h_paged_buf_pool_init()
 *	Sends a command down to NSS to initialize paged buffer pool
 */
nss_tx_status_t nss_n2h_paged_buf_pool_init(struct nss_ctx_instance *nss_ctx)
{
	struct nss_n2h_msg nnm;
	nss_tx_status_t nss_tx_status;

	/*
	 * No additional information needed at this point
	 */
	nss_n2h_msg_init(&nnm, NSS_N2H_INTERFACE,
			NSS_TX_METADATA_TYPE_N2H_PAGED_BUFFER_POOL_INIT,
			sizeof(struct nss_n2h_paged_buffer_pool_init),
			NULL,
			NULL);

	nss_tx_status = nss_n2h_tx_msg(nss_ctx, &nnm);
	if (nss_tx_status != NSS_TX_SUCCESS) {
		nss_warning("%px: failed to send paged buf configuration init command to NSS\n",
				nss_ctx);
		return NSS_TX_FAILURE;
	}

	return NSS_TX_SUCCESS;
}

/*
 * nss_n2h_flush_payloads()
 *	Sends a command down to NSS for flushing all payloads
 */
nss_tx_status_t nss_n2h_flush_payloads(struct nss_ctx_instance *nss_ctx)
{
	struct nss_n2h_msg nnm;
	struct nss_n2h_flush_payloads *nnflshpl;
	nss_tx_status_t nss_tx_status;

	nnflshpl = &nnm.msg.flush_payloads;

	/*
	 * TODO: No additional information sent in message
	 * as of now. Need to initialize message content accordingly
	 * if needed.
	 */
	nss_n2h_msg_init(&nnm, NSS_N2H_INTERFACE,
			NSS_TX_METADATA_TYPE_N2H_FLUSH_PAYLOADS,
			sizeof(struct nss_n2h_flush_payloads),
			NULL,
			NULL);

	nss_tx_status = nss_n2h_tx_msg(nss_ctx, &nnm);
	if (nss_tx_status != NSS_TX_SUCCESS) {
		nss_warning("%px: failed to send flush payloads command to NSS\n",
				nss_ctx);

		return NSS_TX_FAILURE;
	}

	return NSS_TX_SUCCESS;
}

/*
 * nss_n2h_msg_init()
 *	Initialize n2h message.
 */
void nss_n2h_msg_init(struct nss_n2h_msg *nim, uint16_t if_num, uint32_t type,
		      uint32_t len, nss_n2h_msg_callback_t cb, void *app_data)
{
	nss_cmn_msg_init(&nim->cm, if_num, type, len, (void *)cb, app_data);
}

/*
 * nss_n2h_tx_msg()
 *	Send messages to NSS n2h package.
 */
nss_tx_status_t nss_n2h_tx_msg(struct nss_ctx_instance *nss_ctx, struct nss_n2h_msg *nnm)
{
	struct nss_cmn_msg *ncm = &nnm->cm;

	/*
	 * Sanity check the message
	 */
	if (ncm->interface != NSS_N2H_INTERFACE) {
		nss_warning("%px: tx request for another interface: %d", nss_ctx, ncm->interface);
		return NSS_TX_FAILURE;
	}

	if (ncm->type >= NSS_METADATA_TYPE_N2H_MAX) {
		nss_warning("%px: message type out of range: %d", nss_ctx, ncm->type);
		return NSS_TX_FAILURE;
	}

	return nss_core_send_cmd(nss_ctx, nnm, sizeof(*nnm), NSS_NBUF_PAYLOAD_SIZE);
}

/*
 * nss_n2h_notify_register()
 *	Register to received N2H events.
 *
 * NOTE: Do we want to pass an nss_ctx here so that we can register for n2h on any core?
 */
struct nss_ctx_instance *nss_n2h_notify_register(int core, nss_n2h_msg_callback_t cb, void *app_data)
{
	if (core >= nss_top_main.num_nss) {
		nss_warning("Input core number %d is wrong \n", core);
		return NULL;
	}
	/*
	 * TODO: We need to have a new array in support of the new API
	 * TODO: If we use a per-context array, we would move the array into nss_ctx based.
	 */
	nss_n2h_rd[core].n2h_callback = cb;
	nss_n2h_rd[core].app_data = app_data;
	return &nss_top_main.nss[core];
}

/*
 * nss_n2h_register_handler()
 */
void nss_n2h_register_handler(struct nss_ctx_instance *nss_ctx)
{
	sema_init(&nss_n2h_q_cfg_pvt.sem, 1);
	init_completion(&nss_n2h_q_cfg_pvt.complete);

	nss_core_register_handler(nss_ctx, NSS_N2H_INTERFACE, nss_n2h_interface_handler, NULL);

	if (nss_ctx->id == NSS_CORE_0) {
		nss_n2h_stats_dentry_create();
	}
	nss_n2h_strings_dentry_create();

	nss_drv_strings_dentry_create();
}

/*
 * nss_n2h_single_core_register_sysctl()
 */
void nss_n2h_single_core_register_sysctl(void)
{
	/*
	 * RPS sema init
	 */
	sema_init(&nss_n2h_rcp.sem, 1);
	init_completion(&nss_n2h_rcp.complete);

	/*
	 * MITIGATION sema init for core0
	 */
	sema_init(&nss_n2h_mitigationcp[NSS_CORE_0].sem, 1);
	init_completion(&nss_n2h_mitigationcp[NSS_CORE_0].complete);

	/*
	 * PBUF addition sema init for core0
	 */
	sema_init(&nss_n2h_bufcp[NSS_CORE_0].sem, 1);
	init_completion(&nss_n2h_bufcp[NSS_CORE_0].complete);

	/*
	 * Core0
	 */
	sema_init(&nss_n2h_nepbcfgp[NSS_CORE_0].sem, 1);
	init_completion(&nss_n2h_nepbcfgp[NSS_CORE_0].complete);
	nss_n2h_nepbcfgp[NSS_CORE_0].empty_buf_pool_info.pool_size =
		nss_n2h_empty_pool_buf_cfg[NSS_CORE_0];
	nss_n2h_nepbcfgp[NSS_CORE_0].empty_buf_pool_info.low_water =
		nss_n2h_water_mark[NSS_CORE_0][0];
	nss_n2h_nepbcfgp[NSS_CORE_0].empty_buf_pool_info.high_water =
		nss_n2h_water_mark[NSS_CORE_0][1];
	nss_n2h_nepbcfgp[NSS_CORE_0].empty_paged_buf_pool_info.pool_size =
		nss_n2h_empty_paged_pool_buf_cfg[NSS_CORE_0];
	nss_n2h_nepbcfgp[NSS_CORE_0].empty_paged_buf_pool_info.low_water =
		nss_n2h_paged_water_mark[NSS_CORE_0][0];
	nss_n2h_nepbcfgp[NSS_CORE_0].empty_paged_buf_pool_info.high_water =
		nss_n2h_paged_water_mark[NSS_CORE_0][1];

	/*
	 * WiFi pool buf cfg sema init
	 */
	sema_init(&nss_n2h_wp.sem, 1);
	init_completion(&nss_n2h_wp.complete);

	/*
	 * N2H queue config sema init
	 */
	sema_init(&nss_n2h_q_lim_pvt.sem, 1);
	init_completion(&nss_n2h_q_lim_pvt.complete);

	/*
	 * Back pressure config sema init
	 */
	sema_init(&nss_n2h_host_bp_cfg_pvt.sem, 1);
	init_completion(&nss_n2h_host_bp_cfg_pvt.complete);

	nss_n2h_notify_register(NSS_CORE_0, NULL, NULL);

	/*
	 * Register sysctl table.
	 */
	nss_n2h_dir[0].child = nss_n2h_table_single_core;
	nss_n2h_header = register_sysctl_table(nss_n2h_root);
}

/*
 * nss_n2h_multi_core_register_sysctl()
 */
void nss_n2h_multi_core_register_sysctl(void)
{
	/*
	 * RPS sema init
	 */
	sema_init(&nss_n2h_rcp.sem, 1);
	init_completion(&nss_n2h_rcp.complete);

	/*
	 * MITIGATION sema init for core0
	 */
	sema_init(&nss_n2h_mitigationcp[NSS_CORE_0].sem, 1);
	init_completion(&nss_n2h_mitigationcp[NSS_CORE_0].complete);

	/*
	 * MITIGATION sema init for core1
	 */
	sema_init(&nss_n2h_mitigationcp[NSS_CORE_1].sem, 1);
	init_completion(&nss_n2h_mitigationcp[NSS_CORE_1].complete);

	/*
	 * PBUF addition sema init for core0
	 */
	sema_init(&nss_n2h_bufcp[NSS_CORE_0].sem, 1);
	init_completion(&nss_n2h_bufcp[NSS_CORE_0].complete);

	/*
	 * PBUF addition sema init for core1
	 */
	sema_init(&nss_n2h_bufcp[NSS_CORE_1].sem, 1);
	init_completion(&nss_n2h_bufcp[NSS_CORE_1].complete);

	/*
	 * Core0
	 */
	sema_init(&nss_n2h_nepbcfgp[NSS_CORE_0].sem, 1);
	init_completion(&nss_n2h_nepbcfgp[NSS_CORE_0].complete);
	nss_n2h_nepbcfgp[NSS_CORE_0].empty_buf_pool_info.pool_size =
		nss_n2h_empty_pool_buf_cfg[NSS_CORE_0];
	nss_n2h_nepbcfgp[NSS_CORE_0].empty_buf_pool_info.low_water =
		nss_n2h_water_mark[NSS_CORE_0][0];
	nss_n2h_nepbcfgp[NSS_CORE_0].empty_buf_pool_info.high_water =
		nss_n2h_water_mark[NSS_CORE_0][1];
	nss_n2h_nepbcfgp[NSS_CORE_0].empty_paged_buf_pool_info.pool_size =
		nss_n2h_empty_paged_pool_buf_cfg[NSS_CORE_0];
	nss_n2h_nepbcfgp[NSS_CORE_0].empty_paged_buf_pool_info.low_water =
		nss_n2h_paged_water_mark[NSS_CORE_0][0];
	nss_n2h_nepbcfgp[NSS_CORE_0].empty_paged_buf_pool_info.high_water =
		nss_n2h_paged_water_mark[NSS_CORE_0][1];

	/*
	 * Core1
	 */
	sema_init(&nss_n2h_nepbcfgp[NSS_CORE_1].sem, 1);
	init_completion(&nss_n2h_nepbcfgp[NSS_CORE_1].complete);
	nss_n2h_nepbcfgp[NSS_CORE_1].empty_buf_pool_info.pool_size =
		nss_n2h_empty_pool_buf_cfg[NSS_CORE_1];
	nss_n2h_nepbcfgp[NSS_CORE_1].empty_buf_pool_info.low_water =
		nss_n2h_water_mark[NSS_CORE_1][0];
	nss_n2h_nepbcfgp[NSS_CORE_1].empty_buf_pool_info.high_water =
		nss_n2h_water_mark[NSS_CORE_1][1];
	nss_n2h_nepbcfgp[NSS_CORE_1].empty_paged_buf_pool_info.pool_size =
		nss_n2h_empty_paged_pool_buf_cfg[NSS_CORE_1];
	nss_n2h_nepbcfgp[NSS_CORE_1].empty_paged_buf_pool_info.low_water =
		nss_n2h_paged_water_mark[NSS_CORE_1][0];
	nss_n2h_nepbcfgp[NSS_CORE_1].empty_paged_buf_pool_info.high_water =
		nss_n2h_paged_water_mark[NSS_CORE_1][1];

	/*
	 * WiFi pool buf cfg sema init
	 */
	sema_init(&nss_n2h_wp.sem, 1);
	init_completion(&nss_n2h_wp.complete);

	/*
	 * N2H queue config sema init
	 */
	sema_init(&nss_n2h_q_lim_pvt.sem, 1);
	init_completion(&nss_n2h_q_lim_pvt.complete);

	/*
	 * Back pressure config sema init
	 */
	sema_init(&nss_n2h_host_bp_cfg_pvt.sem, 1);
	init_completion(&nss_n2h_host_bp_cfg_pvt.complete);

	nss_n2h_notify_register(NSS_CORE_0, NULL, NULL);
	nss_n2h_notify_register(NSS_CORE_1, NULL, NULL);

	/*
	 * Register sysctl table.
	 */
	nss_n2h_header = register_sysctl_table(nss_n2h_root);
}

/*
 * nss_n2h_unregister_sysctl()
 *	Unregister sysctl specific to n2h
 */
void nss_n2h_unregister_sysctl(void)
{
	/*
	 * Unregister sysctl table.
	 */
	if (nss_n2h_header) {
		unregister_sysctl_table(nss_n2h_header);
	}
}

EXPORT_SYMBOL(nss_n2h_notify_register);

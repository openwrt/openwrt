/*
 **************************************************************************
 * Copyright (c) 2017-2018, 2020, The Linux Foundation. All rights reserved.
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
 * @file nss_project.h
 *	NSS project APIs.
 */
#include "nss_tx_rx_common.h"

static int nss_project_wt_stats_enable;

/*
 * nss_project_free_wt_stats()
 *	Frees a number of allocated worker thread statistics.
 */
static void nss_project_free_wt_stats(struct nss_worker_thread_stats *wt_stats, int num_alloc)
{
	int i;

	if (!wt_stats) {
		return;
	}

	for (i = 0; i < num_alloc; i++) {
		kfree(wt_stats[i].irq_stats);
	}
	kfree(wt_stats);
}

/*
 * nss_project_alloc_wt_stats()
 * 	Allocates worker thread stats for a given  number of threads and IRQs.
 */
static struct nss_worker_thread_stats *nss_project_alloc_wt_stats(uint32_t thread_count, uint32_t irq_count)
{
	struct nss_worker_thread_stats *wt_stats;
	int i;

	wt_stats = kzalloc(thread_count * sizeof(struct nss_worker_thread_stats), GFP_ATOMIC);
	if (unlikely(!wt_stats)) {
		return NULL;
	}

	for (i = 0; i < thread_count; i++) {
		wt_stats[i].irq_stats =
			kzalloc(irq_count * sizeof(struct nss_project_irq_stats), GFP_ATOMIC);
		if (unlikely(!wt_stats[i].irq_stats)) {
			nss_project_free_wt_stats(wt_stats, i);
			return NULL;
		}
	}

	return wt_stats;
}

/*
 * nss_project_wt_stats_enable_callback()
 *	Callback function for wt stats enable messages
 */
static void nss_project_wt_stats_enable_callback(void *app_data, struct nss_project_msg *msg)
{
	struct nss_ctx_instance *nss_ctx = (struct nss_ctx_instance *)app_data;
	struct nss_project_msg_wt_stats_enable *stats_enable = &msg->msg.wt_stats_enable;
	struct nss_worker_thread_stats *stats_temp;

	NSS_VERIFY_CTX_MAGIC(nss_ctx);
	if (msg->cm.response != NSS_CMN_RESPONSE_ACK) {
		return;
	}

	nss_info("%px: Received response ACK for worker thread stats enable msg.\n", nss_ctx);

	/*
	 * If statistics have already been allocated, nothing else to do.
	 */
	if (nss_ctx->wt_stats) {
		return;
	}

	stats_temp = nss_project_alloc_wt_stats(stats_enable->worker_thread_count,
						stats_enable->irq_count);
	if (unlikely(!stats_temp)) {
		nss_warning("%px: Unable to allocate worker thread statistics.\n", nss_ctx);
		return;
	}

	spin_lock_bh(&nss_ctx->nss_top->stats_lock);
	nss_ctx->wt_stats = stats_temp;
	nss_ctx->worker_thread_count = stats_enable->worker_thread_count;
	nss_ctx->irq_count = stats_enable->irq_count;
	spin_unlock_bh(&nss_ctx->nss_top->stats_lock);
}

/*
 * nss_project_wt_stats_send_enable()
 *	Sends message to firmware to enable or disable worker_thread statistics collection.
 */
static nss_tx_status_t nss_project_wt_stats_send_enable(struct nss_ctx_instance *nss_ctx, bool enable)
{
	struct nss_project_msg *npm;
	struct nss_cmn_msg *ncm;
	nss_tx_status_t ret;

	npm = kzalloc(sizeof(*npm), GFP_KERNEL);
	if (!npm) {
		nss_warning("%px: Failed to allocate buffer for message\n", nss_ctx);
		return NSS_TX_FAILURE;
	}

	/*
	 * Populate the message
	 */
	ncm = &npm->cm;
	nss_cmn_msg_init(ncm, NSS_PROJECT_INTERFACE,
		NSS_PROJECT_MSG_WT_STATS_ENABLE,
		sizeof(struct nss_project_msg_wt_stats_enable),
		(void *)nss_project_wt_stats_enable_callback,
		(void *)nss_ctx);
	npm->msg.wt_stats_enable.enable = enable;

	ret = nss_core_send_cmd(nss_ctx, npm, sizeof(*npm), NSS_NBUF_PAYLOAD_SIZE);
	kfree(npm);
	return ret;
}

/*
 * nss_project_wt_stats_update()
 *	Updates stored statistics with the data found in the notify.
 */
static void nss_project_wt_stats_update(struct nss_ctx_instance *nss_ctx,
			struct nss_project_msg_wt_stats_notify *stats_notify)
{
	struct nss_worker_thread_stats *wt_stats;
	int i;

	if (unlikely(!nss_ctx->wt_stats)) {
		nss_warning("%px: Worker thread statistics not yet allocated.\n", nss_ctx);
		return;
	}

	if (unlikely(stats_notify->threadno >= nss_ctx->worker_thread_count)) {
		nss_warning("%px: Invalid WT number %d\n", nss_ctx, stats_notify->threadno);
		return;
	}

	if (unlikely(stats_notify->stats_written > NSS_PROJECT_IRQS_PER_MESSAGE)) {
		nss_warning("%px: Invalid worker thread stats written count %d\n",
				nss_ctx, stats_notify->stats_written);
		return;
	}

	wt_stats = &(nss_ctx->wt_stats[stats_notify->threadno]);

	if (unlikely(!wt_stats->irq_stats)) {
		nss_warning("%px: Worker thread statistics not allocated for thread %d\n",
				nss_ctx, stats_notify->threadno);
		return;
	}

	spin_lock_bh(&nss_ctx->nss_top->stats_lock);
	for (i = 0; i < stats_notify->stats_written; ++i) {
		int irq = stats_notify->stats[i].irq;
		if (unlikely(irq >= nss_ctx->irq_count)) {
			nss_warning("%px: Invalid IRQ number %d\n", nss_ctx, irq);
			continue;
		}

		wt_stats->irq_stats[irq] = stats_notify->stats[i];
	}
	spin_unlock_bh(&nss_ctx->nss_top->stats_lock);
}

/*
 * nss_project_msg_handler()
 *	Handles metadata messages on the project interface.
 */
static void nss_project_msg_handler(struct nss_ctx_instance *nss_ctx,
	struct nss_cmn_msg *ncm, __attribute__((unused))void *app_data)
{
	struct nss_project_msg *npm = (struct nss_project_msg *)ncm;
	nss_project_msg_callback_t cb;

	/*
	 * Sanity checks on message
	 */
	if (npm->cm.type >= NSS_PROJECT_MSG_MAX) {
		nss_warning("%px: message type out of range: %d\n", nss_ctx, npm->cm.type);
		return;
	}

	if (nss_cmn_get_msg_len(&(npm->cm)) > sizeof(struct nss_project_msg)) {
		nss_warning("%px: message length is invalid: %d\n", nss_ctx, nss_cmn_get_msg_len(&(npm->cm)));
		return;
	}

	switch (npm->cm.type) {
	case NSS_PROJECT_MSG_WT_STATS_NOTIFY:
		nss_project_wt_stats_update(nss_ctx, &(npm->msg.wt_stats_notify));
		return;
	}

	nss_core_log_msg_failures(nss_ctx, ncm);

	if (!ncm->cb) {
		return;
	}

	cb = (nss_project_msg_callback_t)ncm->cb;
	cb((void *)nss_ctx, npm);
}

/*
 * nss_project_wt_stats_handler()
 *	Sysctl handler for wt_stats.
 *
 * Uses proc_dointvec to process data. For a write operation, also sends worker
 * thread stats enable messages containing the new value to each NSS core.
 */
static int nss_project_wt_stats_handler(struct ctl_table *ctl, int write,
	void __user *buffer, size_t *lenp, loff_t *ppos)
{
	int ret;
	int i;

	ret = proc_dointvec(ctl, write, buffer, lenp, ppos);

	/*
	 * In case of error, stop now.
	 */
	if (ret) {
		return ret;
	}

	/*
	 * No additional behavior necessary for a read operation.
	 */
	if (!write) {
		return ret;
	}

	/*
	 * If a value was written, send a message containing that value to each
	 * NSS core.
	 */
	for (i = 0; i < nss_top_main.num_nss; ++i) {
		nss_project_wt_stats_send_enable(&(nss_top_main.nss[i]),
			nss_project_wt_stats_enable);
	}
	return ret;

}

/*
 * Tree of ctl_tables used to put the wt_stats proc node in the correct place in
 * the file system. Allows the command $ echo 1 > proc/sys/dev/nss/project/wt_stats
 * to enable worker thread statistics (echoing 0 into the same target will disable).
 */
static struct ctl_table nss_project_table[] = {
	{
		.procname		= "wt_stats",
		.data			= &nss_project_wt_stats_enable,
		.maxlen			= sizeof(int),
		.mode			= 0644,
		.proc_handler		= &nss_project_wt_stats_handler,
	},
	{ }
};

static struct ctl_table nss_project_dir[] = {
	{
		.procname		= "project",
		.mode			= 0555,
		.child			= nss_project_table,
	},
	{ }
};

static struct ctl_table nss_project_root_dir[] = {
	{
		.procname		= "nss",
		.mode			= 0555,
		.child			= nss_project_dir,
	},
	{ }
};

static struct ctl_table nss_project_root[] = {
	{
		.procname		= "dev",
		.mode			= 0555,
		.child			= nss_project_root_dir,
	},
	{ }
};

static struct ctl_table_header *nss_project_header;

/*
 * nss_project_register_sysctl()
 *	Registers any sysctl handlers for the project.
 */
void nss_project_register_sysctl(void)
{
	nss_project_header = register_sysctl_table(nss_project_root);
}

/*
 * nss_project_unregister_sysctl()
 *	De-registers any sysctl handlers for the project.
 */
void nss_project_unregister_sysctl(void)
{
	if (nss_project_header) {
		unregister_sysctl_table(nss_project_header);
	}
}

/*
 * nss_project_register_handler()
 *	Registers the handler for NSS->HLOS messages
 */
void nss_project_register_handler(struct nss_ctx_instance *nss_ctx)
{
	nss_core_register_handler(nss_ctx, NSS_PROJECT_INTERFACE, nss_project_msg_handler, NULL);
}

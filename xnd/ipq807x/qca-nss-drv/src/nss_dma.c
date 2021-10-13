/*
 **************************************************************************
 * Copyright (c) 2020-2021, The Linux Foundation. All rights reserved.
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
 * nss_dma.c
 *	NSS DMA APIs
 */

#include <nss_hal.h>
#include "nss_dma_stats.h"
#include "nss_dma_log.h"
#include "nss_dma_strings.h"

/*
 * Test configuration value
 */
struct nss_dma_test_cfg_val {
	int val;			/* field value */
	int min;			/* Minimum value */
	int max;			/* Maximum value */
};

/*
 * Test configuration for user
 */
struct nss_dma_test_cfg_user {
	struct nss_dma_test_cfg_val run;	/* test run state */
	struct nss_dma_test_cfg_val code;	/* test run code */
	struct nss_dma_test_cfg_val type;	/* test type code */
	struct nss_dma_test_cfg_val packets;	/* packet count per loop */
	int result_tx_packets;			/* test results TX packets */
	int result_rx_packets;			/* test result RX packets */
	int result_time;			/* test time */
};

static struct nss_dma_test_cfg_user test_cfg = {
	.run = {.val = 0, .min = 0 /* stopped */, .max = 1 /* running */},
	.code = {.val = 1, .min = 1 /* linearize */, .max = 2 /* split */},
	.type = {.val = NSS_DMA_TEST_TYPE_DEFAULT, .min = NSS_DMA_TEST_TYPE_DEFAULT, .max = NSS_DMA_TEST_TYPE_MAX},
	.packets = {.val = 1, .min = 1, .max = 65536},
};

/*
 * Private data structure.
 */
struct nss_dma_pvt {
	struct semaphore sem;		/* Semaphore structure. */
	struct completion complete;	/* Completion structure. */
	int response;			/* Response from FW. */
	void *cb;			/* Original cb for sync msgs. */
	void *app_data;			/* Original app_data for sync msgs. */
};

static struct nss_dma_pvt nss_dma_cfg_pvt;

/*
 * nss_dma_verify_if_num()
 *	Verify if_num passed to us.
 */
static inline bool nss_dma_verify_if_num(uint32_t if_num)
{
	return if_num == NSS_DMA_INTERFACE;
}

/*
 * nss_dma_interface_handler()
 *	Handle NSS -> HLOS messages for DMA Statistics
 */
static void nss_dma_msg_handler(struct nss_ctx_instance *nss_ctx, struct nss_cmn_msg *ncm,
				 __attribute__((unused))void *app_data)
{
	struct nss_dma_msg *ndm = (struct nss_dma_msg *)ncm;
	nss_dma_msg_callback_t cb;

	if (!nss_dma_verify_if_num(ncm->interface)) {
		nss_warning("%px: invalid interface %d for dma\n", nss_ctx, ncm->interface);
		return;
	}

	/*
	 * Is this a valid request/response packet?
	 */
	if (ncm->type >= NSS_DMA_MSG_TYPE_MAX) {
		nss_warning("%px: received invalid message %d for dma", nss_ctx, ncm->type);
		return;
	}

	if (nss_cmn_get_msg_len(ncm) > sizeof(struct nss_dma_msg)) {
		nss_warning("%px: Length of message is greater than required: %d", nss_ctx, nss_cmn_get_msg_len(ncm));
		return;
	}

	/*
	 * Trace messages.
	 */
	nss_dma_log_rx_msg(ndm);

	/*
	 * Log failures
	 */
	nss_core_log_msg_failures(nss_ctx, ncm);

	/*
	 * Update driver statistics and send statistics notifications to the registered modules.
	 */
	if (ndm->cm.type == NSS_DMA_MSG_TYPE_SYNC_STATS) {
		nss_dma_stats_sync(nss_ctx, &ndm->msg.stats);
		nss_dma_stats_notify(nss_ctx);
	}

	/*
	 * Update the callback and app_data for NOTIFY messages
	 */
	if (ncm->response == NSS_CMN_RESPONSE_NOTIFY) {
		ncm->cb = (nss_ptr_t)nss_core_get_msg_handler(nss_ctx, ncm->interface);
		ncm->app_data = (nss_ptr_t)nss_ctx->nss_rx_interface_handlers[ncm->interface].app_data;
	}

	/*
	 * Do we have a callback?
	 */
	if (!ncm->cb) {
		return;
	}

	/*
	 * callback
	 */
	cb = (nss_dma_msg_callback_t)ncm->cb;
	cb((void *)ncm->app_data, ncm);
}

/*
 * nss_dma_register_handler()
 *	Register handler for messaging
 */
void nss_dma_register_handler(void)
{
	struct nss_ctx_instance *nss_ctx = nss_dma_get_context();

	nss_info("%px: nss_dma_register_handler", nss_ctx);
	nss_core_register_handler(nss_ctx, NSS_DMA_INTERFACE, nss_dma_msg_handler, NULL);

	nss_dma_stats_dentry_create();
	nss_dma_strings_dentry_create();
}
EXPORT_SYMBOL(nss_dma_register_handler);

/*
 * nss_dma_tx_msg()
 *	Transmit an dma message to the FW with a specified size.
 */
nss_tx_status_t nss_dma_tx_msg(struct nss_ctx_instance *nss_ctx, struct nss_dma_msg *ndm)
{
	struct nss_cmn_msg *ncm = &ndm->cm;

	/*
	 * Sanity check the message
	 */
	if (!nss_dma_verify_if_num(ncm->interface)) {
		nss_warning("%px: tx request for another interface: %d", nss_ctx, ncm->interface);
		return NSS_TX_FAILURE;
	}

	if (ncm->type >= NSS_DMA_MSG_TYPE_MAX) {
		nss_warning("%px: message type out of range: %d", nss_ctx, ncm->type);
		return NSS_TX_FAILURE;
	}

	/*
	 * Trace messages.
	 */
	nss_dma_log_tx_msg(ndm);

	return nss_core_send_cmd(nss_ctx, ndm, sizeof(*ndm), NSS_NBUF_PAYLOAD_SIZE);
}
EXPORT_SYMBOL(nss_dma_tx_msg);

/*
 * nss_dma_msg_test_callback()
 *	Callback function for dma test start configuration
 */
static void nss_dma_msg_test_callback(void *app_data, struct nss_cmn_msg *ncm)
{
	struct nss_ctx_instance *nss_ctx __attribute__((unused)) = (struct nss_ctx_instance *)app_data;
	struct nss_dma_msg *ndm = (struct nss_dma_msg *)ncm;
	struct nss_dma_test_cfg *ndtc = &ndm->msg.test_cfg;
	struct nss_cmn_node_stats *ncns = &ndtc->node_stats;

	test_cfg.run.val = 0; /* test completed */

	/*
	 * Test start has been failed. Restore the value to initial state.
	 */
	if (ndm->cm.response != NSS_CMN_RESPONSE_ACK) {
		nss_warning("%px: nss dma test failed: %d \n", nss_ctx, ndm->cm.error);
		test_cfg.result_tx_packets = 0;
		test_cfg.result_rx_packets = 0;
		test_cfg.result_time = 0;
		return;
	}

	test_cfg.result_tx_packets = ncns->tx_packets;
	test_cfg.result_rx_packets = ncns->rx_packets;
	test_cfg.result_time = ndtc->time_delta;

	nss_info("%px: nss dma test complete\n", nss_ctx);
	nss_info("%px: results tx=%u, rx=%u, time=%u\n", ndm, ncns->tx_packets, ncns->rx_packets, ndtc->time_delta);
}

/*
 * nss_dma_msg_test()
 *	Send NSS DMA test start message.
 */
static nss_tx_status_t nss_dma_msg_test(struct nss_ctx_instance *nss_ctx)
{
	struct nss_dma_msg ndm;
	uint32_t flags = 0;
	int32_t status;
	size_t len;

	len = sizeof(struct nss_cmn_msg) + sizeof(struct nss_dma_test_cfg);

	nss_info("%px: DMA test message:%x\n", nss_ctx, test_cfg.run.val);
	if (test_cfg.code.val == 1) {
		flags = NSS_DMA_TEST_FLAGS_LINEARIZE;
	}

	nss_dma_msg_init(&ndm, NSS_DMA_INTERFACE, NSS_DMA_MSG_TYPE_TEST_PERF, len, nss_dma_msg_test_callback, nss_ctx);

	ndm.msg.test_cfg.packet_count = test_cfg.packets.val;
	ndm.msg.test_cfg.type = test_cfg.type.val;
	ndm.msg.test_cfg.flags = flags;

	status = nss_dma_tx_msg(nss_ctx, &ndm);
	if (unlikely(status != NSS_TX_SUCCESS)) {
		return status;
	}

	/*
	 * Test is now running
	 */
	test_cfg.run.val = 1;
	return NSS_TX_SUCCESS;
}

/*
 * nss_dma_msg_init()
 *	Initialize DMA message.
 */
void nss_dma_msg_init(struct nss_dma_msg *ndm, uint16_t if_num, uint32_t type, uint32_t len, nss_dma_msg_callback_t cb,
			void *app_data)
{
	nss_cmn_msg_init(&ndm->cm, if_num, type, len, (void *)cb, app_data);
}
EXPORT_SYMBOL(nss_dma_msg_init);

/*
 * nss_crypto_cmn_get_context()
 *	get NSS context instance for crypto handle
 */
struct nss_ctx_instance *nss_dma_get_context(void)
{
	return (struct nss_ctx_instance *)&nss_top_main.nss[nss_top_main.dma_handler_id];
}
EXPORT_SYMBOL(nss_dma_get_context);

/*
 * nss_dma_test_handler()
 *	Handles the performance test.
 */
static int nss_dma_test_handler(struct ctl_table *ctl, int write, void __user *buffer, size_t *lenp, loff_t *ppos)
{
	struct nss_ctx_instance *nss_ctx = nss_dma_get_context();
	int cur_state = test_cfg.run.val;
	int ret;

	ret = proc_dointvec(ctl, write, buffer, lenp, ppos);
	if (ret != NSS_SUCCESS) {
		return ret;
	}

	if (!write) {
		return ret;
	}

	/*
	 * Check any tests are already scheduled
	 */
	if (cur_state > 0) {
		nss_info("%px: Test is already running, stopping it.\n", nss_ctx);
	} else {
		nss_info("%px: Test is not running, starting it.\n", nss_ctx);
	}

	ret = nss_dma_msg_test(nss_ctx);
	if (ret != NSS_SUCCESS) {
		nss_warning("%px: Test configuration has failed.\n", nss_ctx);
		test_cfg.run.val = 0;
	}

	return ret;
}

static struct ctl_table nss_dma_table[] = {
	{
		.procname	= "test_run",
		.data		= &test_cfg.run.val,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= nss_dma_test_handler,
		.extra1		= &test_cfg.run.min,
		.extra2		= &test_cfg.run.max,
	},
	{
		.procname	= "test_code",
		.data		= &test_cfg.code.val,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= proc_dointvec_minmax,
		.extra1		= &test_cfg.code.min,
		.extra2		= &test_cfg.code.max,
	},
	{
		.procname	= "test_type",
		.data		= &test_cfg.type.val,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= proc_dointvec_minmax,
		.extra1		= &test_cfg.type.min,
		.extra2		= &test_cfg.type.max,
	},
	{
		.procname	= "test_packets",
		.data		= &test_cfg.packets.val,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= proc_dointvec_minmax,
		.extra1		= &test_cfg.packets.min,
		.extra2		= &test_cfg.packets.max,
	},
	{
		.procname	= "result_tx",
		.data		= &test_cfg.result_tx_packets,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= proc_dointvec,
	},
	{
		.procname	= "result_rx",
		.data		= &test_cfg.result_rx_packets,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= proc_dointvec,
	},
	{
		.procname	= "result_time",
		.data		= &test_cfg.result_time,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= proc_dointvec,
	},
	{ }
};

static struct ctl_table nss_dma_dir[] = {
	{
		.procname		= "dma",
		.mode			= 0555,
		.child			= nss_dma_table,
	},
	{ }
};

static struct ctl_table nss_dma_root_dir[] = {
	{
		.procname		= "nss",
		.mode			= 0555,
		.child			= nss_dma_dir,
	},
	{ }
};

static struct ctl_table nss_dma_root[] = {
	{
		.procname		= "dev",
		.mode			= 0555,
		.child			= nss_dma_root_dir,
	},
	{ }
};

static struct ctl_table_header *nss_dma_header;

/*
 * nss_dma_register_sysctl()
 */
void nss_dma_register_sysctl(void)
{

	/*
	 * dma sema init.
	 */
	sema_init(&nss_dma_cfg_pvt.sem, 1);
	init_completion(&nss_dma_cfg_pvt.complete);

	/*
	 * Register sysctl table.
	 */
	nss_dma_header = register_sysctl_table(nss_dma_root);
}

/*
 * nss_dma_unregister_sysctl()
 *      Unregister sysctl specific to dma
 */
void nss_dma_unregister_sysctl(void)
{
	/*
	 * Unregister sysctl table.
	 */
	if (nss_dma_header) {
		unregister_sysctl_table(nss_dma_header);
	}
}

/*
 * nss_dma_notify_register()
 *	Register to receive dma notify messages.
 */
struct nss_ctx_instance *nss_dma_notify_register(int core, nss_dma_msg_callback_t cb, void *app_data)
{
	struct nss_ctx_instance *nss_ctx = nss_dma_get_context();
	uint32_t ret;

	ret = nss_core_register_handler(nss_ctx, NSS_DMA_INTERFACE, nss_dma_msg_handler, app_data);
	if (ret != NSS_CORE_STATUS_SUCCESS) {
		nss_warning("%px: unable to register event handler for DMA interface", nss_ctx);
		return NULL;
	}

	ret = nss_core_register_msg_handler(nss_ctx, NSS_DMA_INTERFACE, cb);
	if (ret != NSS_CORE_STATUS_SUCCESS) {
		nss_core_unregister_handler(nss_ctx, NSS_DMA_INTERFACE);
		nss_warning("%px: unable to register event handler for DMA interface", nss_ctx);
		return NULL;
	}

	return nss_ctx;
}
EXPORT_SYMBOL(nss_dma_notify_register);

/*
 * nss_dma_notify_unregister()
 *	Unregister to receive dma notify messages.
 */
void nss_dma_notify_unregister(int core)
{
	struct nss_ctx_instance *nss_ctx = nss_dma_get_context();
	uint32_t ret;

	BUG_ON(!nss_ctx);

	ret = nss_core_unregister_msg_handler(nss_ctx, NSS_DMA_INTERFACE);
	if (ret != NSS_CORE_STATUS_SUCCESS) {
		nss_warning("%px: unable to unregister event handler for DMA interface", nss_ctx);
		return;
	}

	ret = nss_core_unregister_handler(nss_ctx, NSS_DMA_INTERFACE);
	if (ret != NSS_CORE_STATUS_SUCCESS) {
		nss_warning("%px: unable to unregister event handler for DMA interface", nss_ctx);
		return;
	}

	return;
}
EXPORT_SYMBOL(nss_dma_notify_unregister);

/*
 * nss_dma_init()
 */
void nss_dma_init(void)
{
	nss_dma_register_sysctl();
}

/*
 **************************************************************************
 * Copyright (c) 2018-2020, The Linux Foundation. All rights reserved.
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
 * nss_c2c_tx.c
 *	NSS C2C_TX APIs
 */

#include <nss_hal.h>
#include "nss_c2c_tx_stats.h"
#include "nss_c2c_tx_log.h"
#include "nss_c2c_tx_strings.h"

int nss_c2c_tx_test_id = -1;

/*
 * Private data structure.
 */
struct nss_c2c_tx_pvt {
	struct semaphore sem;		/* Semaphore structure. */
	struct completion complete;	/* Completion structure. */
	int response;			/* Response from FW. */
	void *cb;			/* Original cb for sync msgs. */
	void *app_data;			/* Original app_data for sync msgs. */
};

/*
 * Notify data structure
 */
struct nss_c2c_tx_notify_data {
	nss_c2c_tx_msg_callback_t c2c_tx_callback;
	void *app_data;
};

static struct nss_c2c_tx_notify_data nss_c2c_tx_notify[NSS_CORE_MAX];
static struct nss_c2c_tx_pvt nss_c2c_tx_cfg_pvt;

/*
 * nss_c2c_tx_verify_if_num()
 *	Verify if_num passed to us.
 */
static inline bool nss_c2c_tx_verify_if_num(uint32_t if_num)
{
	return if_num == NSS_C2C_TX_INTERFACE;
}

/*
 * nss_c2c_tx_interface_handler()
 *	Handle NSS -> HLOS messages for C2C_TX Statistics
 */
static void nss_c2c_tx_msg_handler(struct nss_ctx_instance *nss_ctx,
		struct nss_cmn_msg *ncm, __attribute__((unused))void *app_data)
{
	struct nss_c2c_tx_msg *nctm = (struct nss_c2c_tx_msg *)ncm;
	nss_c2c_tx_msg_callback_t cb;

	if (!nss_c2c_tx_verify_if_num(ncm->interface)) {
		nss_warning("%px: invalid interface %d for c2c_tx\n", nss_ctx, ncm->interface);
		return;
	}

	/*
	 * Is this a valid request/response packet?
	 */
	if (ncm->type >= NSS_C2C_TX_MSG_TYPE_MAX) {
		nss_warning("%px: received invalid message %d for c2c_tx", nss_ctx, ncm->type);
		return;
	}

	if (nss_cmn_get_msg_len(ncm) > sizeof(struct nss_c2c_tx_msg)) {
		nss_warning("%px: Length of message is greater than required: %d", nss_ctx, nss_cmn_get_msg_len(ncm));
		return;
	}

	/*
	 * Trace messages.
	 */
	nss_c2c_tx_log_rx_msg(nctm);

	/*
	 * Log failures
	 */
	nss_core_log_msg_failures(nss_ctx, ncm);

	switch (nctm->cm.type) {
	case NSS_C2C_TX_MSG_TYPE_TX_MAP:
	case NSS_C2C_TX_MSG_TYPE_PERFORMANCE_TEST:
		break;

	case NSS_C2C_TX_MSG_TYPE_STATS:
		/*
		 * Update driver statistics and send statistics notifications to the registered modules.
		 */
		nss_c2c_tx_stats_sync(nss_ctx, &nctm->msg.stats);
		nss_c2c_tx_stats_notify(nss_ctx);
		break;
	}

	/*
	 * Update the callback and app_data for NOTIFY messages
	 */
	if (ncm->response == NSS_CMN_RESPONSE_NOTIFY) {
		ncm->cb = (nss_ptr_t)nss_c2c_tx_notify[nss_ctx->id].c2c_tx_callback;
		ncm->app_data = (nss_ptr_t)nss_c2c_tx_notify[nss_ctx->id].app_data;
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
	cb = (nss_c2c_tx_msg_callback_t)ncm->cb;
	cb((void *)ncm->app_data, nctm);
}

/*
 * nss_c2c_tx_register_handler()
 *	Register handler for messaging
 */
void nss_c2c_tx_register_handler(struct nss_ctx_instance *nss_ctx)
{
	nss_info("%px: nss_c2c_tx_register_handler", nss_ctx);
	nss_core_register_handler(nss_ctx, NSS_C2C_TX_INTERFACE, nss_c2c_tx_msg_handler, NULL);

	if (nss_ctx->id == NSS_CORE_0) {
		nss_c2c_tx_stats_dentry_create();
	}
	nss_c2c_tx_strings_dentry_create();
}
EXPORT_SYMBOL(nss_c2c_tx_register_handler);

/*
 * nss_c2c_tx_tx_msg()
 *	Transmit an c2c_tx message to the FW with a specified size.
 */
nss_tx_status_t nss_c2c_tx_tx_msg(struct nss_ctx_instance *nss_ctx, struct nss_c2c_tx_msg *nctm)
{
	struct nss_cmn_msg *ncm = &nctm->cm;

	/*
	 * Sanity check the message
	 */
	if (!nss_c2c_tx_verify_if_num(ncm->interface)) {
		nss_warning("%px: tx request for another interface: %d", nss_ctx, ncm->interface);
		return NSS_TX_FAILURE;
	}

	if (ncm->type >= NSS_C2C_TX_MSG_TYPE_MAX) {
		nss_warning("%px: message type out of range: %d", nss_ctx, ncm->type);
		return NSS_TX_FAILURE;
	}

	/*
	 * Trace messages.
	 */
	nss_c2c_tx_log_tx_msg(nctm);

	return nss_core_send_cmd(nss_ctx, nctm, sizeof(*nctm), NSS_NBUF_PAYLOAD_SIZE);
}
EXPORT_SYMBOL(nss_c2c_tx_tx_msg);

/*
 * nss_c2c_tx_msg_cfg_map_callback()
 *	Callback function for tx_map configuration
 */
static void nss_c2c_tx_msg_cfg_map_callback(void *app_data, struct nss_c2c_tx_msg *nctm)
{
	struct nss_ctx_instance *nss_ctx __attribute__((unused)) = (struct nss_ctx_instance *)app_data;
	if (nctm->cm.response != NSS_CMN_RESPONSE_ACK) {
		nss_warning("%px: nss c2c_tx_map configuration failed: %d for NSS core %d\n",
			nss_ctx, nctm->cm.error, nss_ctx->id);
	}

	nss_info("%px: nss c2c_tx_map configuration succeeded for NSS core %d\n",
		nss_ctx, nss_ctx->id);
}

/*
 * nss_c2c_tx_msg_performance_test_start_callback()
 *	Callback function for c2c_tx test start configuration
 */
static void nss_c2c_tx_msg_performance_test_callback(void *app_data, struct nss_c2c_tx_msg *nctm)
{
	struct nss_ctx_instance *nss_ctx __attribute__((unused)) = (struct nss_ctx_instance *)app_data;

	/*
	 * Test start has been failed. Restore the value to initial state.
	 */
	if (nctm->cm.response != NSS_CMN_RESPONSE_ACK) {
		nss_warning("%px: nss c2c_tx test start failed: %d for NSS core %d\n",
			nss_ctx, nctm->cm.error, nss_ctx->id);
		nss_c2c_tx_test_id = -1;
		return;
	}

	nss_info("%px: nss c2c_tx test successfully initialized for NSS core %d\n",
		nss_ctx, nss_ctx->id);
}

/*
 * nss_c2c_tx_msg_cfg_map()
 *	Send NSS to c2c_map
 */
nss_tx_status_t nss_c2c_tx_msg_cfg_map(struct nss_ctx_instance *nss_ctx, uint32_t tx_map, uint32_t c2c_intr_addr)
{
	int32_t status;
	struct nss_c2c_tx_msg nctm;
	struct nss_c2c_tx_map *cfg_map;

	nss_info("%px: C2C map:%x\n", nss_ctx, tx_map);
	nss_c2c_tx_msg_init(&nctm, NSS_C2C_TX_INTERFACE, NSS_C2C_TX_MSG_TYPE_TX_MAP,
		sizeof(struct nss_c2c_tx_map), nss_c2c_tx_msg_cfg_map_callback, (void *)nss_ctx);

	cfg_map = &nctm.msg.map;
	cfg_map->tx_map = tx_map;
	cfg_map->c2c_intr_addr = c2c_intr_addr;

	status = nss_c2c_tx_tx_msg(nss_ctx, &nctm);
	if (unlikely(status != NSS_TX_SUCCESS)) {
		return status;
	}

	return NSS_TX_SUCCESS;
}

/*
 * nss_c2c_tx_msg_performance_test()
 *	Send NSS c2c peformance test start message.
 */
nss_tx_status_t nss_c2c_tx_msg_performance_test(struct nss_ctx_instance *nss_ctx, uint32_t test_id)
{
	int32_t status;
	struct nss_c2c_tx_msg nctm;
	struct nss_c2c_tx_test *test;

	nss_info("%px: C2C test message:%x\n", nss_ctx, test_id);
	nss_c2c_tx_msg_init(&nctm, NSS_C2C_TX_INTERFACE, NSS_C2C_TX_MSG_TYPE_PERFORMANCE_TEST,
		sizeof(struct nss_c2c_tx_test), nss_c2c_tx_msg_performance_test_callback, (void *)nss_ctx);

	test = &nctm.msg.test;
	test->test_id = test_id;

	status = nss_c2c_tx_tx_msg(nss_ctx, &nctm);
	if (unlikely(status != NSS_TX_SUCCESS)) {
		return status;
	}

	return NSS_TX_SUCCESS;
}

/*
 * nss_c2c_tx_msg_init()
 *	Initialize C2C_TX message.
 */
void nss_c2c_tx_msg_init(struct nss_c2c_tx_msg *nctm, uint16_t if_num, uint32_t type, uint32_t len,
			nss_c2c_tx_msg_callback_t cb, void *app_data)
{
	nss_cmn_msg_init(&nctm->cm, if_num, type, len, (void *)cb, app_data);
}
EXPORT_SYMBOL(nss_c2c_tx_msg_init);

/*
 * nss_c2c_tx_performance_test_handler()
 *	Handles the performance test.
 */
static int nss_c2c_tx_performance_test_handler(struct ctl_table *ctl, int write,
				void __user *buffer, size_t *lenp, loff_t *ppos)
{
	struct nss_top_instance *nss_top = &nss_top_main;
	struct nss_ctx_instance *nss_ctx = &nss_top->nss[0];
	int ret, ret_c2c_tx, current_state;
	current_state = nss_c2c_tx_test_id;
	ret = proc_dointvec(ctl, write, buffer, lenp, ppos);

	if (ret != NSS_SUCCESS) {
		return ret;
	}

	if (!write) {
		return ret;
	}

	if (current_state != -1) {
		nss_warning("%px: Another test is running.\n", nss_ctx);
		return -EINVAL;
	}

	if (nss_c2c_tx_test_id >= NSS_C2C_TX_TEST_TYPE_MAX || nss_c2c_tx_test_id <= 0) {
		nss_warning("%px: Invalid test ID.\n", nss_ctx);
		nss_c2c_tx_test_id = current_state;
		return -EINVAL;
	}

	nss_info("Starting the c2c_tx performance test\n");
	ret_c2c_tx = nss_c2c_tx_msg_performance_test(nss_ctx, nss_c2c_tx_test_id);

	if (ret_c2c_tx != NSS_SUCCESS) {
		nss_warning("%px: Starting the test has failed.\n", nss_ctx);
		nss_c2c_tx_test_id = -1;
	}

	return ret_c2c_tx;
}

static struct ctl_table nss_c2c_tx_table[] = {
	{
		.procname	= "test_code",
		.data		= &nss_c2c_tx_test_id,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= &nss_c2c_tx_performance_test_handler,
	},
	{ }
};

static struct ctl_table nss_c2c_tx_dir[] = {
	{
		.procname		= "c2c_tx",
		.mode			= 0555,
		.child			= nss_c2c_tx_table,
	},
	{ }
};

static struct ctl_table nss_c2c_tx_root_dir[] = {
	{
		.procname		= "nss",
		.mode			= 0555,
		.child			= nss_c2c_tx_dir,
	},
	{ }
};

static struct ctl_table nss_c2c_tx_root[] = {
	{
		.procname		= "dev",
		.mode			= 0555,
		.child			= nss_c2c_tx_root_dir,
	},
	{ }
};

static struct ctl_table_header *nss_c2c_tx_header;

/*
 * nss_c2c_tx_register_sysctl()
 */
void nss_c2c_tx_register_sysctl(void)
{

	/*
	 * c2c_tx sema init.
	 */
	sema_init(&nss_c2c_tx_cfg_pvt.sem, 1);
	init_completion(&nss_c2c_tx_cfg_pvt.complete);

	/*
	 * Register sysctl table.
	 */
	nss_c2c_tx_header = register_sysctl_table(nss_c2c_tx_root);
}

/*
 * nss_c2c_tx_unregister_sysctl()
 *      Unregister sysctl specific to c2c_tx
 */
void nss_c2c_tx_unregister_sysctl(void)
{
	/*
	 * Unregister sysctl table.
	 */
	if (nss_c2c_tx_header) {
		unregister_sysctl_table(nss_c2c_tx_header);
	}
}

/*
 * nss_c2c_tx_notify_register()
 *	Register to receive c2c_tx notify messages.
 */
struct nss_ctx_instance *nss_c2c_tx_notify_register(int core, nss_c2c_tx_msg_callback_t cb, void *app_data)
{
	if (core >= NSS_CORE_MAX) {
		nss_warning("Input core number %d is wrong\n", core);
		return NULL;
	}

	nss_c2c_tx_notify[core].c2c_tx_callback = cb;
	nss_c2c_tx_notify[core].app_data = app_data;

	return (struct nss_ctx_instance *)&nss_top_main.nss[core];
}
EXPORT_SYMBOL(nss_c2c_tx_notify_register);

/*
 * nss_c2c_tx_notify_unregister()
 *	Unregister to receive c2c_tx notify messages.
 */
void nss_c2c_tx_notify_unregister(int core)
{
	if (core >= NSS_CORE_MAX) {
		nss_warning("Input core number %d is wrong\n", core);
		return;
	}

	nss_c2c_tx_notify[core].c2c_tx_callback = NULL;
	nss_c2c_tx_notify[core].app_data = NULL;
}
EXPORT_SYMBOL(nss_c2c_tx_notify_unregister);

/*
 * nss_c2c_tx_init()
 */
void nss_c2c_tx_init(void)
{
	int core;

	for (core = 0; core < NSS_CORE_MAX; core++) {
		nss_c2c_tx_notify_register(core, NULL, NULL);
	}
}

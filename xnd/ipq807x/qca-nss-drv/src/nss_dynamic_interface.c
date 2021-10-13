/*
 **************************************************************************
 * Copyright (c) 2014-2020, The Linux Foundation. All rights reserved.
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

#include "nss_tx_rx_common.h"
#include "nss_dynamic_interface_log.h"
#include "nss_dynamic_interface_stats.h"

#define NSS_DYNAMIC_INTERFACE_COMP_TIMEOUT 60000	/* 60 Sec */

/*
 * Declare atomic notifier data structure for statistics.
 */
ATOMIC_NOTIFIER_HEAD(nss_dynamic_interface_stats_notifier);

void nss_dynamic_interface_stats_notify(uint32_t if_num, uint32_t core_id);

/*
 * Message data structure to store the message result
 */
struct nss_dynamic_interface_msg_data {
	struct completion complete;	/* completion structure */
	int if_num;			/* Interface number */
	enum nss_cmn_response response;	/* Message response */
};

static nss_dynamic_interface_assigned nss_dynamic_interface_assigned_types[NSS_CORE_MAX][NSS_MAX_DYNAMIC_INTERFACES]; /* Array of assigned interface types */

/*
 * nss_dynamic_interface_handler()
 * 	Handle NSS -> HLOS messages for dynamic interfaces
 */
static void nss_dynamic_interface_handler(struct nss_ctx_instance *nss_ctx, struct nss_cmn_msg *ncm, __attribute__((unused))void *app_data)
{
	nss_dynamic_interface_msg_callback_t cb;
	struct nss_dynamic_interface_msg *ndim = (struct nss_dynamic_interface_msg *)ncm;
	int32_t if_num;

	BUG_ON(ncm->interface != NSS_DYNAMIC_INTERFACE);

	/*
	 * Is this a valid request/response packet?
	 */
	if (ncm->type >= NSS_DYNAMIC_INTERFACE_MAX) {
		nss_warning("%px: received invalid message %d for dynamic interface", nss_ctx, ncm->type);
		return;
	}

	if (nss_cmn_get_msg_len(ncm) > sizeof(struct nss_dynamic_interface_msg)) {
		nss_warning("%px: Length of message is greater than required: %d", nss_ctx, nss_cmn_get_msg_len(ncm));
		return;
	}

	/*
	 * Log failures
	 */
	nss_core_log_msg_failures(nss_ctx, ncm);

	/*
	 * Trace messages.
	 */
	nss_dynamic_interface_log_rx_msg(ndim);

	/*
	 * Handling dynamic interface messages coming from NSS fw.
	 */
	switch (ndim->cm.type) {
	case NSS_DYNAMIC_INTERFACE_ALLOC_NODE:
		if (ncm->response == NSS_CMN_RESPONSE_ACK) {
			nss_info("%px alloc_node response ack if_num %d\n", nss_ctx, ndim->msg.alloc_node.if_num);
			if_num = ndim->msg.alloc_node.if_num;
			if (if_num > 0) {
				nss_dynamic_interface_assigned_types[nss_ctx->id][if_num - NSS_DYNAMIC_IF_START] = ndim->msg.alloc_node.type;
			} else {
				nss_warning("%px: if_num < 0\n", nss_ctx);
			}
		}

		break;

	case NSS_DYNAMIC_INTERFACE_DEALLOC_NODE:
		if (ncm->response == NSS_CMN_RESPONSE_ACK) {
			nss_info("%px dealloc_node response ack if_num %d\n", nss_ctx, ndim->msg.dealloc_node.if_num);
			if_num = ndim->msg.dealloc_node.if_num;
			nss_dynamic_interface_assigned_types[nss_ctx->id][if_num - NSS_DYNAMIC_IF_START] = NSS_DYNAMIC_INTERFACE_TYPE_NONE;
			/*
			 * Send dynamic interface dealloc notifications to the registered modules.
			 */
			nss_dynamic_interface_stats_notify(ndim->msg.dealloc_node.if_num, nss_ctx->id);
		}

		break;

	default:
		nss_warning("%px: Received response %d for type %d, interface %d",
				nss_ctx, ncm->response, ncm->type, ncm->interface);
		return;
	}

	/*
	 * Do we have a callback?
	 */
	if (!ncm->cb) {
		nss_warning("%px: nss_dynamic_interface_handler cb is NULL\n", nss_ctx);
		return;
	}

	/*
	 * Callback
	 */
	cb = (nss_dynamic_interface_msg_callback_t)ncm->cb;
	cb((void *)ncm->app_data, ncm);
}

/*
 * nss_dynamic_interface_callback
 *	Callback to handle the message response from NSS FW.
 */
static void nss_dynamic_interface_callback(void *app_data, struct nss_cmn_msg *ncm)
{
	struct nss_dynamic_interface_msg_data *di_data = (struct nss_dynamic_interface_msg_data *)app_data;
	struct nss_dynamic_interface_msg *ndim = (struct nss_dynamic_interface_msg *)ncm;

	di_data->response = ncm->response;
	di_data->if_num = ndim->msg.alloc_node.if_num;

	/*
	 * Unblock the sleeping function.
	 */
	complete(&di_data->complete);
}

/*
 * nss_dynamic_interface_tx()
 * 	Transmit a dynamic interface message to NSSFW, asynchronously.
 */
nss_tx_status_t nss_dynamic_interface_tx(struct nss_ctx_instance *nss_ctx, struct nss_dynamic_interface_msg *msg)
{
	struct nss_cmn_msg *ncm = &msg->cm;

	/*
	 * Sanity check the message
	 */
	if (ncm->interface != NSS_DYNAMIC_INTERFACE) {
		nss_warning("%px: tx request for another interface: %d", nss_ctx, ncm->interface);
		return NSS_TX_FAILURE;
	}

	if (ncm->type > NSS_DYNAMIC_INTERFACE_MAX) {
		nss_warning("%px: message type out of range: %d", nss_ctx, ncm->type);
		return NSS_TX_FAILURE;
	}

	/*
	 * Trace messages.
	 */
	nss_dynamic_interface_log_tx_msg(msg);

	return nss_core_send_cmd(nss_ctx, msg, sizeof(*msg), NSS_NBUF_PAYLOAD_SIZE);
}

/*
 * nss_dynamic_interface_tx_sync()
 *	Send the message to NSS and wait till we get an ACK or NACK for this msg.
 */
static nss_tx_status_t nss_dynamic_interface_tx_sync(struct nss_ctx_instance *nss_ctx, struct nss_dynamic_interface_msg_data *di_data,
						     struct nss_dynamic_interface_msg *ndim)
{
	nss_tx_status_t status;
	int ret;

	status = nss_dynamic_interface_tx(nss_ctx, ndim);
	if (status != NSS_TX_SUCCESS) {
		nss_warning("%px: not able to transmit msg successfully\n", nss_ctx);
		return status;
	}

	/*
	 * Blocking call, wait till we get ACK for this msg.
	 */
	ret = wait_for_completion_timeout(&di_data->complete, msecs_to_jiffies(NSS_DYNAMIC_INTERFACE_COMP_TIMEOUT));
	if (ret == 0) {
		nss_warning("%px: Waiting for ack timed out\n", nss_ctx);
		return NSS_TX_FAILURE;
	}

	return status;
}

/*
 * nss_dynamic_interface_alloc_node()
 *	Allocates node of perticular type on NSS and returns interface_num for this node or -1 in case of failure.
 *
 * Note: This function should not be called from soft_irq or interrupt context because it blocks till ACK/NACK is
 * received for the message sent to NSS.
 */
int nss_dynamic_interface_alloc_node(enum nss_dynamic_interface_type type)
{
	struct nss_ctx_instance *nss_ctx = NULL;
	struct nss_dynamic_interface_msg ndim;
	struct nss_dynamic_interface_alloc_node_msg *ndia;
	struct nss_dynamic_interface_msg_data di_data;
	uint32_t core_id;
	nss_tx_status_t status;

	if (type >= NSS_DYNAMIC_INTERFACE_TYPE_MAX) {
		nss_warning("Dynamic if msg drooped as type is wrong %d\n", type);
		return -1;
	}

	core_id = nss_top_main.dynamic_interface_table[type];
	nss_ctx = (struct nss_ctx_instance *)&nss_top_main.nss[core_id];
	di_data.if_num = -1;
	di_data.response = false;
	init_completion(&di_data.complete);

	nss_dynamic_interface_msg_init(&ndim, NSS_DYNAMIC_INTERFACE, NSS_DYNAMIC_INTERFACE_ALLOC_NODE,
				sizeof(struct nss_dynamic_interface_alloc_node_msg), nss_dynamic_interface_callback, (void *)&di_data);

	ndia = &ndim.msg.alloc_node;
	ndia->type = type;

	/*
	 * Initialize if_num to -1. The allocated if_num is returned by the firmware
	 * in the response message.
	 */
	ndia->if_num = -1;

	/*
	 * Calling synchronous transmit function.
	 */
	status = nss_dynamic_interface_tx_sync(nss_ctx, &di_data, &ndim);
	if (status != NSS_TX_SUCCESS) {
		nss_warning("%px not able to transmit alloc node msg\n", nss_ctx);
		return -1;
	}

	/*
	 * Check response and return -1 if its a NACK else proceed.
	 */
	if (di_data.response != NSS_CMN_RESPONSE_ACK) {
		nss_warning("%px Received NACK from NSS - Response:%d\n", nss_ctx, di_data.response);
		return -1;
	}

	return di_data.if_num;
}

/*
 * nss_dynamic_interface_dealloc_node()
 *	Deallocate node of particular type and if_num in NSS.
 *
 * Note: This will just mark the state of node as not active, actual memory will be freed when reference count of that node becomes 0.
 * This function should not be called from soft_irq or interrupt context because it blocks till ACK/NACK is received for the message
 * sent to NSS.
 */
nss_tx_status_t nss_dynamic_interface_dealloc_node(int if_num, enum nss_dynamic_interface_type type)
{
	struct nss_ctx_instance *nss_ctx = NULL;
	struct nss_dynamic_interface_msg ndim;
	struct nss_dynamic_interface_dealloc_node_msg *ndid;
	struct nss_dynamic_interface_msg_data di_data;
	uint32_t core_id;
	nss_tx_status_t status;

	if (type >= NSS_DYNAMIC_INTERFACE_TYPE_MAX) {
		nss_warning("Dynamic if msg dropped as type is wrong type %d if_num %d\n", type, if_num);
		return NSS_TX_FAILURE_BAD_PARAM;
	}

	core_id = nss_top_main.dynamic_interface_table[type];
	nss_ctx = (struct nss_ctx_instance *)&nss_top_main.nss[core_id];
	di_data.response = false;
	init_completion(&di_data.complete);

	if (nss_is_dynamic_interface(if_num) == false) {
		nss_warning("%px: nss_dynamic_interface if_num is not in range %d\n", nss_ctx, if_num);
		return NSS_TX_FAILURE_BAD_PARAM;
	}

	nss_dynamic_interface_msg_init(&ndim, NSS_DYNAMIC_INTERFACE, NSS_DYNAMIC_INTERFACE_DEALLOC_NODE,
				sizeof(struct nss_dynamic_interface_dealloc_node_msg), nss_dynamic_interface_callback, (void *)&di_data);

	ndid = &ndim.msg.dealloc_node;
	ndid->type = type;
	ndid->if_num = if_num;

	/*
	 * Calling synchronous transmit function.
	 */
	status = nss_dynamic_interface_tx_sync(nss_ctx, &di_data, &ndim);
	if (status != NSS_TX_SUCCESS) {
		nss_warning("%px not able to transmit alloc node msg\n", nss_ctx);
		return status;
	}

	if (di_data.response != NSS_CMN_RESPONSE_ACK) {
		nss_warning("%px Received NACK from NSS\n", nss_ctx);
		return -1;
	}

	return status;
}

/*
 * nss_dynamic_interface_register_handler()
 */
void nss_dynamic_interface_register_handler(struct nss_ctx_instance *nss_ctx)
{
	nss_core_register_handler(nss_ctx, NSS_DYNAMIC_INTERFACE, nss_dynamic_interface_handler, NULL);
	nss_dynamic_interface_stats_dentry_create();
}

/*
 * nss_is_dynamic_interface()
 *	Judge it is a valid dynamic interface
 */
bool nss_is_dynamic_interface(int if_num)
{
	return (if_num >= NSS_DYNAMIC_IF_START && if_num < NSS_SPECIAL_IF_START);
}

/*
 * nss_dynamic_interface_get_nss_ctx_by_type()
 * 	Gets the NSS context using NSS dynamic interface type.
 */
struct nss_ctx_instance *nss_dynamic_interface_get_nss_ctx_by_type(enum nss_dynamic_interface_type type)
{
	struct nss_ctx_instance *nss_ctx = NULL;
	uint32_t core_id;

	if (type >= NSS_DYNAMIC_INTERFACE_TYPE_MAX) {
		nss_warning("Invalid param: Type is wrong %d\n", type);
		return NULL;
	}

	core_id = nss_top_main.dynamic_interface_table[type];
	nss_ctx = (struct nss_ctx_instance *)&nss_top_main.nss[core_id];

	NSS_VERIFY_CTX_MAGIC(nss_ctx);

	return nss_ctx;
}

/*
 * nss_dynamic_interface_get_type()
 *	Gets the type of dynamic interface
 */
enum nss_dynamic_interface_type nss_dynamic_interface_get_type(struct nss_ctx_instance *nss_ctx, int if_num)
{
	NSS_VERIFY_CTX_MAGIC(nss_ctx);

	if (nss_is_dynamic_interface(if_num) == false) {
		return NSS_DYNAMIC_INTERFACE_TYPE_NONE;
	}

	return nss_dynamic_interface_assigned_types[nss_ctx->id][if_num - NSS_DYNAMIC_IF_START];
}

/*
 * nss_dynamic_interface_msg_init()
 *	Initialize dynamic interface message.
 */
void nss_dynamic_interface_msg_init(struct nss_dynamic_interface_msg *ndm, uint16_t if_num, uint32_t type, uint32_t len,
						void *cb, void *app_data)
{
	nss_cmn_msg_init(&ndm->cm, if_num, type, len, cb, app_data);
}

/*
 * nss_dynamic_interface_stats_notify()
 *	Sends notifications to all the registered modules.
 */
void nss_dynamic_interface_stats_notify(uint32_t if_num, uint32_t core_id)
{
	struct nss_dynamic_interface_notification stats;

	stats.core_id = core_id;
	stats.if_num = if_num;
	atomic_notifier_call_chain(&nss_dynamic_interface_stats_notifier, NSS_STATS_EVENT_NOTIFY, (void *)&stats);
}
EXPORT_SYMBOL(nss_dynamic_interface_stats_notify);

/*
 * nss_dynamic_interface_stats_register_notifier()
 *	Registers statistics notifier.
 */
int nss_dynamic_interface_stats_register_notifier(struct notifier_block *nb)
{
	return atomic_notifier_chain_register(&nss_dynamic_interface_stats_notifier, nb);
}
EXPORT_SYMBOL(nss_dynamic_interface_stats_register_notifier);

/*
 * nss_dynamic_interface_stats_unregister_notifier()
 *	Deregisters statistics notifier.
 */
int nss_dynamic_interface_stats_unregister_notifier(struct notifier_block *nb)
{
	return atomic_notifier_chain_unregister(&nss_dynamic_interface_stats_notifier, nb);
}
EXPORT_SYMBOL(nss_dynamic_interface_stats_unregister_notifier);

EXPORT_SYMBOL(nss_dynamic_interface_alloc_node);
EXPORT_SYMBOL(nss_dynamic_interface_dealloc_node);
EXPORT_SYMBOL(nss_is_dynamic_interface);
EXPORT_SYMBOL(nss_dynamic_interface_get_type);
EXPORT_SYMBOL(nss_dynamic_interface_get_nss_ctx_by_type);

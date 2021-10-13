/*
 **************************************************************************
 * Copyright (c) 2021, The Linux Foundation. All rights reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 **************************************************************************
 */

#include "nss_tx_rx_common.h"
#include "nss_core.h"
#include "nss_cmn.h"
#include "nss_wifi_mesh.h"
#include "nss_wifi_mesh_log.h"
#include "nss_wifi_mesh_strings.h"

/*
 * nss_wifi_mesh_verify_if_num()
 *	Verify interface number.
 */
bool nss_wifi_mesh_verify_if_num(nss_if_num_t if_num)
{
	enum nss_dynamic_interface_type if_type = nss_dynamic_interface_get_type(nss_wifi_mesh_get_context(), if_num);

	return ((if_type == NSS_DYNAMIC_INTERFACE_TYPE_WIFI_MESH_INNER) ||
				(if_type == NSS_DYNAMIC_INTERFACE_TYPE_WIFI_MESH_OUTER));
}
EXPORT_SYMBOL(nss_wifi_mesh_verify_if_num);

/* nss_wifi_mesh_handler()
 *	Handles Wi-Fi mesh messages from NSS to HLOS.
 */
static void nss_wifi_mesh_handler(struct nss_ctx_instance *nss_ctx, struct nss_cmn_msg *ncm, void *app_data)
{
	nss_wifi_mesh_msg_callback_t cb;
	struct nss_wifi_mesh_msg *nwmm = (struct nss_wifi_mesh_msg *)ncm;

	NSS_VERIFY_CTX_MAGIC(nss_ctx);
	nss_assert(nss_is_dynamic_interface(ncm->interface));
	nss_assert(nss_wifi_mesh_verify_if_num(ncm->interface));

	/*
	 * Is this a valid request/response packet?
	 */
	if (ncm->type >= NSS_WIFI_MESH_MSG_MAX) {
		nss_warning("%px: Received invalid message %d for wifi_mesh interface\n", nss_ctx, ncm->type);
		return;
	}


	/*
	 * For variable array the size of the common length will be greater the nss_wifi_mesh_msg
	 * length. Add conditional checking for messages where length check will fail.
	 */
	if ((nss_cmn_get_msg_len(ncm) > sizeof(struct nss_wifi_mesh_msg)) &&
		(ncm->type != NSS_WIFI_MESH_MSG_PATH_TABLE_DUMP) &&
		(ncm->type != NSS_WIFI_MESH_MSG_PROXY_PATH_TABLE_DUMP)) {
			nss_warning("%px: Length of message is greater than expected, type: %d, len: %d",
			    		nss_ctx, ncm->type, ncm->len);
		return;
	}

	/*
	 * Log failures
	 */
	nss_core_log_msg_failures(nss_ctx, ncm);

	/*
	 * Trace Messages
	 */
	nss_wifi_mesh_log_rx_msg(nwmm);

	/*
	 * Update the stats and send statistics notifications to the registered modules.
	 */
	if (nwmm->cm.type == NSS_WIFI_MESH_MSG_STATS_SYNC) {
		nss_wifi_mesh_update_stats(ncm->interface, &nwmm->msg.stats_sync_msg);
		nss_wifi_mesh_stats_notify(ncm->interface, nss_ctx->id);
	}

	if (ncm->response == NSS_CMN_RESPONSE_NOTIFY) {
		ncm->cb = (nss_ptr_t)nss_core_get_msg_handler(nss_ctx, ncm->interface);
		ncm->app_data = (nss_ptr_t)app_data;
	}

	if (!ncm->cb) {
		return;
	}

	cb = (nss_wifi_mesh_msg_callback_t)ncm->cb;
	cb((void *)ncm->app_data, ncm);
}

/*
 * nss_wifi_mesh_msg_init()
 * 	Initiliaze a Wi-Fi mesh message.
 */
void nss_wifi_mesh_msg_init(struct nss_wifi_mesh_msg *nwm, nss_if_num_t if_num, uint32_t type, uint32_t len,
			nss_wifi_mesh_msg_callback_t cb, void *app_data)
{
	nss_assert(nss_wifi_mesh_verify_if_num(if_num));
	nss_cmn_msg_init(&nwm->cm, if_num, type, len, cb, app_data);
}
EXPORT_SYMBOL(nss_wifi_mesh_msg_init);

/*
 * nss_wifi_mesh_tx_buf
 * 	Send data packet for vap processing asynchronously.
 */
nss_tx_status_t nss_wifi_mesh_tx_buf(struct nss_ctx_instance *nss_ctx, struct sk_buff *os_buf, nss_if_num_t if_num)
{
	nss_assert(nss_is_dynamic_interface(if_num));
	return nss_core_send_packet(nss_ctx, os_buf, if_num, H2N_BIT_FLAG_BUFFER_REUSABLE);
}
EXPORT_SYMBOL(nss_wifi_mesh_tx_buf);

/*
 * nss_wifi_mesh_tx_msg
 *	Transmit a Wi-Fi mesh message to the NSS firmware asynchronously.
 *
 * NOTE: The caller is expected to handle synchronous waiting for message
 * response if needed.
 */
nss_tx_status_t nss_wifi_mesh_tx_msg(struct nss_ctx_instance *nss_ctx, struct nss_wifi_mesh_msg *msg)
{
	struct nss_cmn_msg *ncm = &msg->cm;

	NSS_VERIFY_CTX_MAGIC(nss_ctx);

	if (ncm->type >= NSS_WIFI_MESH_MSG_MAX) {
		nss_warning("%px: wifi_mesh message type out of range: %d\n", nss_ctx, ncm->type);
		return NSS_TX_FAILURE;
	}

	/*
	 * Log messages.
	 */
	nss_wifi_mesh_log_tx_msg(msg);

	/*
	 * The interface number shall be one of the Wi-Fi mesh socket interfaces.
	 */
	nss_assert(nss_is_dynamic_interface(ncm->interface));

	return nss_core_send_cmd(nss_ctx, msg, sizeof(*msg), NSS_NBUF_PAYLOAD_SIZE);
}
EXPORT_SYMBOL(nss_wifi_mesh_tx_msg);

/*
 ****************************************
 * Register/Unregister/Miscellaneous APIs
 ****************************************
 */

/*
 * nss_wifi_mesh_get_context()
 *	Return the core ctx which the feature is on.
 */
struct nss_ctx_instance *nss_wifi_mesh_get_context(void)
{
	return (struct nss_ctx_instance *)&nss_top_main.nss[nss_top_main.wifi_handler_id];
}
EXPORT_SYMBOL(nss_wifi_mesh_get_context);

/*
 * nss_unregister_wifi_mesh_if()
 *	Unregister Wi-Fi mesh from the NSS driver.
 */
void nss_unregister_wifi_mesh_if(nss_if_num_t if_num)
{
	struct nss_ctx_instance *nss_ctx = nss_wifi_mesh_get_context();

	NSS_VERIFY_CTX_MAGIC(nss_ctx);

	nss_core_unregister_subsys_dp(nss_ctx, if_num);
	nss_core_unregister_msg_handler(nss_ctx, if_num);
	nss_core_unregister_handler(nss_ctx, if_num);
	nss_wifi_mesh_stats_handle_free(if_num);
}
EXPORT_SYMBOL(nss_unregister_wifi_mesh_if);

/*
 * nss_register_wifi_mesh_if()
 *	Register wifi_mesh with nss driver.
 */
uint32_t nss_register_wifi_mesh_if(nss_if_num_t if_num,
				   nss_wifi_mesh_data_callback_t  mesh_data_callback,
				   nss_wifi_mesh_ext_data_callback_t mesh_ext_data_callback,
				   nss_wifi_mesh_msg_callback_t mesh_event_callback,
				   uint32_t dp_type, struct net_device *netdev, uint32_t features)
{
	struct nss_ctx_instance *nss_ctx = nss_wifi_mesh_get_context();
	uint32_t status;

	NSS_VERIFY_CTX_MAGIC(nss_ctx);
	nss_assert(netdev);
	nss_assert(nss_wifi_mesh_verify_if_num(if_num));

	if (!nss_wifi_mesh_stats_handle_alloc(if_num, netdev->ifindex)) {
		nss_warning("%px: couldn't allocate stats handle for device name: %s, if_num: 0x%x\n", nss_ctx, netdev->name, if_num);
		return NSS_CORE_STATUS_FAILURE;
	}

	nss_core_register_handler(nss_ctx, if_num, nss_wifi_mesh_handler, netdev);

	status = nss_core_register_msg_handler(nss_ctx, if_num, mesh_event_callback);
	if (status != NSS_CORE_STATUS_SUCCESS) {
		nss_warning("%px: unable to register event handler for interface(%u)\n", nss_ctx, if_num);
		nss_core_unregister_handler(nss_ctx, if_num);
		nss_wifi_mesh_stats_handle_free(if_num);
		return status;
	}

	nss_core_register_subsys_dp(nss_ctx, if_num, mesh_data_callback, mesh_ext_data_callback, NULL, netdev, features);
	nss_core_set_subsys_dp_type(nss_ctx, netdev, if_num, dp_type);
	return NSS_CORE_STATUS_SUCCESS;
}
EXPORT_SYMBOL(nss_register_wifi_mesh_if);

/*
 * nss_wifi_mesh_init()
 *	Initialize the mesh stats dentries.
 */
void nss_wifi_mesh_init(void)
{
	if (!nss_wifi_mesh_strings_dentry_create()) {
		nss_warning("Unable to create dentry for Wi-Fi mesh strings\n");
	}

	if (!nss_wifi_mesh_stats_dentry_create()) {
		nss_warning("Unable to create dentry for Wi-Fi mesh stats\n");
	}
}

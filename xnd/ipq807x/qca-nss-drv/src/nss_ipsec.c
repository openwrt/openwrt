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
 * nss_ipsec.c
 *	NSS IPsec APIs
 */

#include "nss_tx_rx_common.h"
#include "nss_ipsec.h"
#include "nss_ppe.h"
#include "nss_ipsec_log.h"

#if defined(NSS_HAL_IPQ806X_SUPPORT)
#define NSS_IPSEC_ENCAP_INTERFACE_NUM NSS_IPSEC_ENCAP_IF_NUMBER
#define NSS_IPSEC_DECAP_INTERFACE_NUM NSS_IPSEC_DECAP_IF_NUMBER
#define NSS_IPSEC_DATA_INTERFACE_NUM NSS_C2C_TX_INTERFACE

#elif defined(NSS_HAL_FSM9010_SUPPORT)
#define NSS_IPSEC_ENCAP_INTERFACE_NUM NSS_IPSEC_ENCAP_IF_NUMBER
#define NSS_IPSEC_DECAP_INTERFACE_NUM NSS_IPSEC_DECAP_IF_NUMBER
#define NSS_IPSEC_DATA_INTERFACE_NUM NSS_IPSEC_CMN_INTERFACE

#elif defined(NSS_HAL_IPQ807x_SUPPORT)
#define NSS_IPSEC_ENCAP_INTERFACE_NUM NSS_IPSEC_CMN_INTERFACE
#define NSS_IPSEC_DECAP_INTERFACE_NUM NSS_IPSEC_CMN_INTERFACE
#define NSS_IPSEC_DATA_INTERFACE_NUM NSS_IPSEC_CMN_INTERFACE

#elif defined(NSS_HAL_IPQ60XX_SUPPORT)
#define NSS_IPSEC_ENCAP_INTERFACE_NUM NSS_IPSEC_CMN_INTERFACE
#define NSS_IPSEC_DECAP_INTERFACE_NUM NSS_IPSEC_CMN_INTERFACE
#define NSS_IPSEC_DATA_INTERFACE_NUM NSS_IPSEC_CMN_INTERFACE

#elif defined(NSS_HAL_IPQ50XX_SUPPORT)
#define NSS_IPSEC_ENCAP_INTERFACE_NUM NSS_IPSEC_CMN_INTERFACE
#define NSS_IPSEC_DECAP_INTERFACE_NUM NSS_IPSEC_CMN_INTERFACE
#define NSS_IPSEC_DATA_INTERFACE_NUM NSS_IPSEC_CMN_INTERFACE

#else
#define NSS_IPSEC_ENCAP_INTERFACE_NUM -1
#define NSS_IPSEC_DECAP_INTERFACE_NUM -1
#define NSS_IPSEC_DATA_INTERFACE_NUM -1

#endif

/*
 * Amount time the synchronous message should wait for response from
 * NSS before the timeout happens. After the timeout the message
 * response even if it arrives has to be discarded. Typically, the
 * time needs to be selected based on the worst case time in case of
 * peak throughput between host & NSS.
 */
#define NSS_IPSEC_TX_TIMEO_TICKS msecs_to_jiffies(3000) /* 3 Seconds */

/*
 * Private data structure to hold state for
 * the ipsec specific NSS interaction
 */
struct nss_ipsec_pvt {
	struct semaphore sem;		/* used for synchronizing 'tx_msg_sync' */
	struct completion complete;	/* completion callback */
	atomic_t resp;	/* Response error type */
} nss_ipsec;

/*
 * nss_ipsec_get_msg_ctx()
 * 	return ipsec message context assoicated with the callback
 *
 * Note: certain SOC the decap interface specially programmed
 */
static inline nss_ptr_t nss_ipsec_get_msg_ctx(struct nss_ctx_instance *nss_ctx, uint32_t interface_num)
{
	struct nss_top_instance *nss_top = nss_ctx->nss_top;

	/*
	 * the encap is primary interface
	 */
	if (interface_num == NSS_IPSEC_ENCAP_INTERFACE_NUM)
		return (nss_ptr_t)nss_top->ipsec_encap_ctx;

	return (nss_ptr_t)nss_top->ipsec_decap_ctx;
}

/*
 * nss_ipsec_get_msg_callback()
 * 	this gets the message callback handler
 */
static inline nss_ptr_t nss_ipsec_get_msg_callback(struct nss_ctx_instance *nss_ctx, uint32_t interface_num)
{
	struct nss_top_instance *nss_top = nss_ctx->nss_top;

	/*
	 * the encap is primary interface
	 */
	if (interface_num == NSS_IPSEC_ENCAP_INTERFACE_NUM)
		return (nss_ptr_t)nss_top->ipsec_encap_callback;

	return (nss_ptr_t)nss_top->ipsec_decap_callback;
}

/*
 **********************************
 Rx APIs
 **********************************
 */

/*
 * nss_ipsec_msg_handler()
 * 	this handles all the IPsec events and responses
 */
static void nss_ipsec_msg_handler(struct nss_ctx_instance *nss_ctx, struct nss_cmn_msg *ncm, void *app_data __attribute((unused)))
{
	struct nss_ipsec_msg *nim = (struct nss_ipsec_msg *)ncm;
	nss_ipsec_msg_callback_t cb = NULL;
	uint32_t if_num = ncm->interface;

	/*
	 * Trace messages.
	 */
	nss_ipsec_log_rx_msg(nim);

	/*
	 * Sanity check the message type
	 */
	if (ncm->type > NSS_IPSEC_MSG_TYPE_MAX) {
		nss_warning("%px: rx message type out of range: %d", nss_ctx, ncm->type);
		return;
	}

	if (nss_cmn_get_msg_len(ncm) > sizeof(struct nss_ipsec_msg)) {
		nss_warning("%px: rx message length is invalid: %d", nss_ctx, nss_cmn_get_msg_len(ncm));
		return;
	}

	BUG_ON((if_num != NSS_IPSEC_ENCAP_INTERFACE_NUM) && (if_num != NSS_IPSEC_DECAP_INTERFACE_NUM));

	if (ncm->response == NSS_CMN_RESPONSE_LAST) {
		nss_warning("%px: rx message response for if %d, type %d, is invalid: %d", nss_ctx, ncm->interface,
				ncm->type, ncm->response);
		return;
	}

	/*
	 * Is this a notification? if, yes then fill up the callback and app_data from
	 * locally stored state
	 */
	if (ncm->response == NSS_CMN_RESPONSE_NOTIFY) {
		ncm->cb = nss_ipsec_get_msg_callback(nss_ctx, if_num);
		ncm->app_data = nss_ipsec_get_msg_ctx(nss_ctx, if_num);
	}

	nss_core_log_msg_failures(nss_ctx, ncm);

	/*
	 * load, test & call
	 */
	cb = (nss_ipsec_msg_callback_t)ncm->cb;
	if (unlikely(!cb)) {
		nss_trace("%px: rx handler has been unregistered for i/f: %d", nss_ctx, ncm->interface);
		return;
	}

	cb((void *)ncm->app_data, nim);
}

/*
 **********************************
 Tx APIs
 **********************************
 */

/*
 * nss_ipsec_callback()
 *	Callback to handle the completion of NSS->HLOS messages.
 */
static void nss_ipsec_callback(void *app_data, struct nss_ipsec_msg *nim)
{
	struct nss_cmn_msg *ncm = &nim->cm;

	/*
	 * This callback is for synchronous operation. The caller sends its
	 * response pointer which needs to be loaded with the response
	 * data arriving from the NSS
	 */
	atomic_t *resp = (atomic_t *)app_data;

	if (ncm->response == NSS_CMN_RESPONSE_ACK) {
		atomic_set(resp, NSS_IPSEC_ERROR_TYPE_NONE);
		complete(&nss_ipsec.complete);
		return;
	}

	atomic_set(resp, ncm->error);
	complete(&nss_ipsec.complete);
}

/*
 * nss_ipsec_tx_msg
 *	Send ipsec rule to NSS.
 */
nss_tx_status_t nss_ipsec_tx_msg(struct nss_ctx_instance *nss_ctx, struct nss_ipsec_msg *msg)
{
	struct nss_cmn_msg *ncm = &msg->cm;

	nss_info("%px: message %d for if %d\n", nss_ctx, ncm->type, ncm->interface);

	BUILD_BUG_ON(NSS_NBUF_PAYLOAD_SIZE < sizeof(struct nss_ipsec_msg));

	/*
	 * Trace messages.
	 */
	nss_ipsec_log_tx_msg(msg);

	if ((ncm->interface != NSS_IPSEC_ENCAP_INTERFACE_NUM) && (ncm->interface != NSS_IPSEC_DECAP_INTERFACE_NUM)) {
		nss_warning("%px: tx message request for another interface: %d", nss_ctx, ncm->interface);
		return NSS_TX_FAILURE;
	}

	if (ncm->type > NSS_IPSEC_MSG_TYPE_MAX) {
		nss_warning("%px: tx message type out of range: %d", nss_ctx, ncm->type);
		return NSS_TX_FAILURE;
	}

	nss_info("msg params version:%d, interface:%d, type:%d, cb:%px, app_data:%px, len:%d\n",
			ncm->version, ncm->interface, ncm->type, (void *)ncm->cb, (void *)ncm->app_data, ncm->len);

	return nss_core_send_cmd(nss_ctx, msg, sizeof(*msg), NSS_NBUF_PAYLOAD_SIZE);
}
EXPORT_SYMBOL(nss_ipsec_tx_msg);

/*
 * nss_ipsec_tx_msg_sync()
 *	Transmit a ipsec message to NSS firmware synchronously.
 */
nss_tx_status_t nss_ipsec_tx_msg_sync(struct nss_ctx_instance *nss_ctx, uint32_t if_num,
					enum nss_ipsec_msg_type type, uint16_t len,
					struct nss_ipsec_msg *nim, enum nss_ipsec_error_type *resp)
{
	struct nss_ipsec_msg nim_local = { {0} };
	nss_tx_status_t status;
	int ret;

	/*
	 * Length of the message should be the based on type
	 */
	if (len > sizeof(nim_local.msg)) {
		nss_warning("%px: (%u)Bad message length(%u) for type (%d)", nss_ctx, if_num, len, type);
		return NSS_TX_FAILURE_TOO_LARGE;
	}

	/*
	 * Response buffer is a required for copying the response for message
	 */
	if (!resp) {
		nss_warning("%px: (%u)Response buffer is empty, type(%d)", nss_ctx, if_num, type);
		return NSS_TX_FAILURE_BAD_PARAM;
	}

	/*
	 * TODO: this can be removed in future as we need to ensure that the response
	 * memory is only updated when the current outstanding request is waiting.
	 * This can be solved by introducing sequence no. in messages and only completing
	 * the message if the sequence no. matches. For now this is solved by passing
	 * a known memory nss_ipsec.resp
	 */
	down(&nss_ipsec.sem);

	/*
	 * Initializing it to a fail error type
	 */
	atomic_set(&nss_ipsec.resp, NSS_IPSEC_ERROR_TYPE_UNHANDLED_MSG);

	/*
	 * We need to copy the message content into the actual message
	 * to be sent to NSS
	 *
	 * Note: Here pass the nss_ipsec.resp as the pointer. Since, the caller
	 * provided pointer is not allocated by us and may go away when this function
	 * returns with failure. The callback is not aware of this and may try to
	 * access the pointer incorrectly potentially resulting in a crash.
	 */
	nss_ipsec_msg_init(&nim_local, if_num, type, len, nss_ipsec_callback, &nss_ipsec.resp);
	memcpy(&nim_local.msg, &nim->msg, len);

	status = nss_ipsec_tx_msg(nss_ctx, &nim_local);
	if (status != NSS_TX_SUCCESS) {
		nss_warning("%px: ipsec_tx_msg failed", nss_ctx);
		goto done;
	}

	ret = wait_for_completion_timeout(&nss_ipsec.complete, NSS_IPSEC_TX_TIMEO_TICKS);
	if (!ret) {
		nss_warning("%px: IPsec msg tx failed due to timeout", nss_ctx);
		status = NSS_TX_FAILURE_NOT_ENABLED;
		goto done;
	}

	/*
	 * Read memory barrier
	 */
	smp_rmb();

	/*
	 * Copy the response received
	 */
	*resp = atomic_read(&nss_ipsec.resp);

	/*
	 * Only in case of non-error response we will
	 * indicate success
	 */
	if (*resp != NSS_IPSEC_ERROR_TYPE_NONE)
		status = NSS_TX_FAILURE;

done:
	up(&nss_ipsec.sem);
	return status;
}
EXPORT_SYMBOL(nss_ipsec_tx_msg_sync);

/*
 * nss_ipsec_tx_buf
 * 	Send data packet for ipsec processing
 */
nss_tx_status_t nss_ipsec_tx_buf(struct sk_buff *skb, uint32_t if_num)
{
	struct nss_ctx_instance *nss_ctx = &nss_top_main.nss[nss_top_main.ipsec_handler_id];

	nss_trace("%px: IPsec If Tx packet, id:%d, data=%px", nss_ctx, if_num, skb->data);

	return nss_core_send_packet(nss_ctx, skb, if_num, H2N_BIT_FLAG_BUFFER_REUSABLE);
}
EXPORT_SYMBOL(nss_ipsec_tx_buf);

/*
 **********************************
 Register APIs
 **********************************
 */

/*
 * nss_ipsec_notify_register()
 * 	register message notifier for the given interface (if_num)
 */
struct nss_ctx_instance *nss_ipsec_notify_register(uint32_t if_num, nss_ipsec_msg_callback_t cb, void *app_data)
{
	struct nss_top_instance *nss_top = &nss_top_main;
	uint8_t core_id = nss_top->ipsec_handler_id;
	struct nss_ctx_instance *nss_ctx = &nss_top->nss[core_id];

	if (if_num >= NSS_MAX_NET_INTERFACES) {
		nss_warning("%px: notfiy register received for invalid interface %d", nss_ctx, if_num);
		return NULL;
	}

	/*
	 * the encap is primary interface
	 */
	if (if_num == NSS_IPSEC_ENCAP_INTERFACE_NUM) {
		nss_top->ipsec_encap_callback = cb;
		nss_top->ipsec_encap_ctx = app_data;
		return nss_ctx;
	}

	nss_top->ipsec_decap_callback = cb;
	nss_top->ipsec_decap_ctx = app_data;
	return nss_ctx;
}
EXPORT_SYMBOL(nss_ipsec_notify_register);

/*
 * nss_ipsec_notify_unregister()
 * 	unregister the IPsec notifier for the given interface number (if_num)
 */
void nss_ipsec_notify_unregister(struct nss_ctx_instance *nss_ctx, uint32_t if_num)
{
	struct nss_top_instance *nss_top = nss_ctx->nss_top;

	if (if_num >= NSS_MAX_NET_INTERFACES) {
		nss_warning("%px: notify unregister received for invalid interface %d", nss_ctx, if_num);
		return;
	}

	/*
	 * the encap is primary interface
	 */
	if (if_num == NSS_IPSEC_ENCAP_INTERFACE_NUM) {
		nss_top->ipsec_encap_callback = NULL;
		nss_top->ipsec_encap_ctx = NULL;
		return;
	}

	nss_top->ipsec_decap_callback = NULL;
	nss_top->ipsec_decap_ctx = NULL;
}
EXPORT_SYMBOL(nss_ipsec_notify_unregister);

/*
 * nss_ipsec_data_register()
 * 	register a data callback routine
 */
struct nss_ctx_instance *nss_ipsec_data_register(uint32_t if_num, nss_ipsec_buf_callback_t cb, struct net_device *netdev, uint32_t features)
{
	struct nss_ctx_instance *nss_ctx, *nss_ctx0;

	nss_ctx = &nss_top_main.nss[nss_top_main.ipsec_handler_id];

	if ((if_num >= NSS_MAX_NET_INTERFACES) && (if_num < NSS_MAX_PHYSICAL_INTERFACES)){
		nss_warning("%px: data register received for invalid interface %d", nss_ctx, if_num);
		return NULL;
	}

	/*
	 * avoid multiple registeration for multiple tunnels
	 */
	if (nss_ctx->subsys_dp_register[if_num].cb) {
		return nss_ctx;
	}

	nss_core_register_subsys_dp(nss_ctx, if_num, cb, NULL, NULL, netdev, features);

	if (nss_top_main.ipsec_handler_id == 1) {
		nss_ctx0 = &nss_top_main.nss[0];

		nss_core_register_subsys_dp(nss_ctx0, if_num, cb, NULL, NULL, netdev, features);
	}

	return nss_ctx;
}
EXPORT_SYMBOL(nss_ipsec_data_register);

/*
 * nss_ipsec_data_unregister()
 * 	unregister a data callback routine
 */
void nss_ipsec_data_unregister(struct nss_ctx_instance *nss_ctx, uint32_t if_num)
{
	struct nss_ctx_instance *nss_ctx0;

	if ((if_num >= NSS_MAX_NET_INTERFACES) && (if_num < NSS_MAX_PHYSICAL_INTERFACES)){
		nss_warning("%px: data unregister received for invalid interface %d", nss_ctx, if_num);
		return;
	}

	if (nss_top_main.ipsec_handler_id == 1) {
		nss_ctx0 = &nss_top_main.nss[0];

		nss_core_unregister_subsys_dp(nss_ctx0, if_num);
	}

	nss_core_unregister_subsys_dp(nss_ctx, if_num);
}
EXPORT_SYMBOL(nss_ipsec_data_unregister);

/*
 * nss_ipsec_get_encap_interface()
 * 	Get the NSS interface number for encap message
 */
int32_t nss_ipsec_get_encap_interface(void)
{
	return NSS_IPSEC_ENCAP_INTERFACE_NUM;
}
EXPORT_SYMBOL(nss_ipsec_get_encap_interface);

/*
 * nss_ipsec_get_decap_interface()
 * 	Get the NSS interface number for decap message
 */
int32_t nss_ipsec_get_decap_interface(void)
{
	return NSS_IPSEC_DECAP_INTERFACE_NUM;
}
EXPORT_SYMBOL(nss_ipsec_get_decap_interface);

/*
 * nss_ipsec_get_data_interface()
 * 	Get the NSS interface number used for data path
 */
int32_t nss_ipsec_get_data_interface(void)
{
	return NSS_IPSEC_DATA_INTERFACE_NUM;
}
EXPORT_SYMBOL(nss_ipsec_get_data_interface);

/*
 * nss_ipsec_get_context()
 * 	Get NSS context instance for IPsec handle
 */
struct nss_ctx_instance *nss_ipsec_get_context(void)
{
	return &nss_top_main.nss[nss_top_main.ipsec_handler_id];
}
EXPORT_SYMBOL(nss_ipsec_get_context);

/*
 * nss_ipsec_get_ifnum()
 *	Return IPsec interface number with coreid.
 */
int32_t nss_ipsec_get_ifnum(int32_t if_num)
{
	struct nss_ctx_instance *nss_ctx = nss_ipsec_get_context();

	NSS_VERIFY_CTX_MAGIC(nss_ctx);
	return NSS_INTERFACE_NUM_APPEND_COREID(nss_ctx, if_num);
}
EXPORT_SYMBOL(nss_ipsec_get_ifnum);

/*
 * nss_ipsec_ppe_port_config()
 *	Configure PPE port for IPsec inline
 */
bool nss_ipsec_ppe_port_config(struct nss_ctx_instance *nss_ctx, struct net_device *netdev,
					uint32_t if_num, uint32_t vsi_num)
{
#ifdef NSS_PPE_SUPPORTED
	if_num = NSS_INTERFACE_NUM_APPEND_COREID(nss_ctx, if_num);

	if (nss_ppe_tx_ipsec_config_msg(if_num, vsi_num, netdev->mtu, netdev->mtu) != NSS_TX_SUCCESS) {
		nss_warning("%px: Failed to configure PPE IPsec port", nss_ctx);
		return false;
	}

	return true;
#else
	return false;
#endif
}
EXPORT_SYMBOL(nss_ipsec_ppe_port_config);

/*
 * nss_ipsec_ppe_mtu_update()
 *	Update PPE MTU for IPsec inline
 */
bool nss_ipsec_ppe_mtu_update(struct nss_ctx_instance *nss_ctx, uint32_t if_num, uint16_t mtu, uint16_t mru)
{
#ifdef NSS_PPE_SUPPORTED
	if_num = NSS_INTERFACE_NUM_APPEND_COREID(nss_ctx, if_num);

	if (nss_ppe_tx_ipsec_mtu_msg(if_num, mtu, mru) != NSS_TX_SUCCESS) {
		nss_warning("%px: Failed to update PPE MTU for IPsec port", nss_ctx);
		return false;
	}

	return true;
#else
	return false;
#endif
}
EXPORT_SYMBOL(nss_ipsec_ppe_mtu_update);

/*
 * nss_ipsec_register_handler()
 */
void nss_ipsec_register_handler()
{
	struct nss_ctx_instance *nss_ctx = &nss_top_main.nss[nss_top_main.ipsec_handler_id];

	BUILD_BUG_ON(NSS_IPSEC_ENCAP_INTERFACE_NUM < 0);
	BUILD_BUG_ON(NSS_IPSEC_DECAP_INTERFACE_NUM < 0);

	sema_init(&nss_ipsec.sem, 1);
	init_completion(&nss_ipsec.complete);
	atomic_set(&nss_ipsec.resp, NSS_IPSEC_ERROR_TYPE_NONE);

	nss_ctx->nss_top->ipsec_encap_callback = NULL;
	nss_ctx->nss_top->ipsec_decap_callback = NULL;

	nss_ctx->nss_top->ipsec_encap_ctx = NULL;
	nss_ctx->nss_top->ipsec_decap_ctx = NULL;

	nss_core_register_handler(nss_ctx, NSS_IPSEC_ENCAP_INTERFACE_NUM, nss_ipsec_msg_handler, NULL);
	nss_core_register_handler(nss_ctx, NSS_IPSEC_DECAP_INTERFACE_NUM, nss_ipsec_msg_handler, NULL);
}

/*
 * nss_ipsec_msg_init()
 *	Initialize ipsec message.
 */
void nss_ipsec_msg_init(struct nss_ipsec_msg *nim, uint16_t if_num, uint32_t type, uint32_t len,
			nss_ipsec_msg_callback_t cb, void *app_data)
{
	nss_cmn_msg_init(&nim->cm, if_num, type, len, (void *)cb, app_data);
}
EXPORT_SYMBOL(nss_ipsec_msg_init);

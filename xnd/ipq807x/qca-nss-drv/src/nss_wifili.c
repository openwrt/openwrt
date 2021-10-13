/*
 **************************************************************************
 * Copyright (c) 2017-2021, The Linux Foundation. All rights reserved.
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
#include "nss_wifili_stats.h"
#include "nss_wifili_log.h"
#include "nss_wifili_strings.h"

#define NSS_WIFILI_TX_TIMEOUT 1000 /* Millisecond to jiffies*/
#define NSS_WIFILI_INVALID_SCHEME_ID  -1
#define NSS_WIFILI_THREAD_SCHEME_ENTRY_MAX  4 /* Maximum number of thread scheme entries. */
#define NSS_WIFILI_EXTERNAL_INTERFACE_MAX 2 /* Maximum external I/F supported */

/*
 * NSS external interface number table
 */
nss_if_num_t nss_wifili_external_tbl[NSS_WIFILI_EXTERNAL_INTERFACE_MAX] =
	{NSS_WIFILI_EXTERNAL_INTERFACE0, NSS_WIFILI_EXTERNAL_INTERFACE1};

/*
 * nss_wifili_thread_scheme_entry
 *	Details of thread scheme.
 */
struct nss_wifili_thread_scheme_entry {
	int32_t radio_ifnum;		/* Radio interface number. */
	uint32_t radio_priority;	/* Priority of radio. */
	uint32_t scheme_priority;	/* Priority of scheme. */
	uint8_t scheme_index;		/* Scheme index allocated to radio. */
	bool allocated;			/* Flag to check if scheme is allocated. */
};

/*
 * nss_wifili_thread_scheme_db
 *	Wifili thread scheme database.
 */
struct nss_wifili_thread_scheme_db {
	spinlock_t lock;	/* Lock to protect from simultaneous access. */
	uint32_t radio_count;	/* Radio counter. */
	struct nss_wifili_thread_scheme_entry nwtse[NSS_WIFILI_THREAD_SCHEME_ENTRY_MAX];
				/* Metadata for each of scheme. */
};

/*
 * nss_wifili_external_if_state_tbl
 *	External interface state table
 */
struct nss_wifili_external_if_state_tbl {
	nss_if_num_t ifnum;
	bool in_use;
};

/*
 * nss_wifili_external_if_info
 *	Wifili external interface info
 */
struct nss_wifili_external_if_info {
	spinlock_t lock;
	struct nss_wifili_external_if_state_tbl state_tbl[NSS_WIFILI_EXTERNAL_INTERFACE_MAX];
} nss_wifi_eif_info;

/*
 * nss_wifili_pvt
 *	Private data structure
 */
static struct nss_wifili_pvt {
	struct semaphore sem;
	struct completion complete;
	int response;
	void *cb;
	void *app_data;
} wifili_pvt;

/*
 * Scheme to radio mapping database
 */
static struct nss_wifili_thread_scheme_db ts_db[NSS_MAX_CORES];

/*
 * nss_wifili_handler()
 *	Handle NSS -> HLOS messages for wifi
 */
static void nss_wifili_handler(struct nss_ctx_instance *nss_ctx, struct nss_cmn_msg *ncm, __attribute__((unused))void *app_data)
{
	struct nss_wifili_msg *ntm = (struct nss_wifili_msg *)ncm;
	void *ctx;
	nss_wifili_msg_callback_t cb;

	nss_info("%px: NSS->HLOS message for wifili\n", nss_ctx);

	/*
	 * The interface number shall be wifili soc interface or wifili radio interface
	 */
	BUG_ON((nss_is_dynamic_interface(ncm->interface))
		|| ((ncm->interface != NSS_WIFILI_INTERNAL_INTERFACE)
		&& (ncm->interface != NSS_WIFILI_EXTERNAL_INTERFACE0)
		&& (ncm->interface != NSS_WIFILI_EXTERNAL_INTERFACE1)));

	/*
	 * Trace messages.
	 */
	nss_wifili_log_rx_msg(ntm);

	/*
	 * Is this a valid request/response packet?
	 */
	if (ncm->type >= NSS_WIFILI_MAX_MSG) {
		nss_warning("%px: Received invalid message %d for wifili interface", nss_ctx, ncm->type);
		return;
	}

	if ((nss_cmn_get_msg_len(ncm) > sizeof(struct nss_wifili_msg)) &&
		ntm->cm.type != NSS_WIFILI_PEER_EXT_STATS_MSG) {
		nss_warning("%px: Length of message is greater than required: %d", nss_ctx, nss_cmn_get_msg_len(ncm));
		return;
	}

	/*
	 * Snoop messages for local driver and handle
	 */
	switch (ntm->cm.type) {
	case NSS_WIFILI_STATS_MSG:
		/*
		 * Update WIFI driver statistics and send statistics notifications to the registered modules
		 */
		nss_wifili_stats_sync(nss_ctx, &ntm->msg.wlsoc_stats, ncm->interface);
		nss_wifili_stats_notify(nss_ctx, ncm->interface);
		break;
	}

	/*
	 * Update the callback and app_data for notify messages, wifili sends all notify messages
	 * to the same callback/app_data.
	 */
	if (ncm->response == NSS_CMN_RESPONSE_NOTIFY) {
		ncm->cb = (nss_ptr_t)nss_ctx->nss_top->wifili_msg_callback;
	}

	/*
	 * Log failures
	 */
	nss_core_log_msg_failures(nss_ctx, ncm);

	/*
	 * Do we have a call back
	 */
	if (!ncm->cb) {
		nss_info("%px: cb null for wifili interface %d", nss_ctx, ncm->interface);
		return;
	}

	/*
	 * Get callback & context
	 */
	cb = (nss_wifili_msg_callback_t)ncm->cb;
	ctx = nss_ctx->subsys_dp_register[ncm->interface].ndev;

	/*
	 * call wifili msg callback
	 */
	if (!ctx) {
		nss_warning("%px: Event received for wifili interface %d before registration", nss_ctx, ncm->interface);
		return;
	}

	cb(ctx, ntm);
}

/*
 * nss_wifili_callback()
 *	Callback to handle the completion of NSS->HLOS messages.
 */
static void nss_wifili_callback(void *app_data, struct nss_wifili_msg *nvm)
{
	nss_wifili_msg_callback_t callback = (nss_wifili_msg_callback_t)wifili_pvt.cb;
	void *data = wifili_pvt.app_data;

	wifili_pvt.response = NSS_TX_SUCCESS;
	wifili_pvt.cb = NULL;
	wifili_pvt.app_data = NULL;

	if (nvm->cm.response != NSS_CMN_RESPONSE_ACK) {
		nss_warning("wifili error response %d\n", nvm->cm.response);
		wifili_pvt.response = nvm->cm.response;
	}

	if (callback) {
		callback(data, nvm);
	}
	complete(&wifili_pvt.complete);
}

/*
 * nss_wifili_tx_msg
 *	Transmit a wifili message to NSS FW
 *
 * NOTE: The caller is expected to handle synchronous wait for message
 * response if needed.
 */
nss_tx_status_t nss_wifili_tx_msg(struct nss_ctx_instance *nss_ctx, struct nss_wifili_msg *msg)
{
	struct nss_cmn_msg *ncm = &msg->cm;

	/*
	 * Trace messages.
	 */
	nss_wifili_log_tx_msg(msg);

	if (ncm->type >= NSS_WIFILI_MAX_MSG) {
		nss_warning("%px: wifili message type out of range: %d", nss_ctx, ncm->type);
		return NSS_TX_FAILURE;
	}

	/*
	 * The interface number shall be one of the wifili soc interfaces
	 */
	if ((ncm->interface != NSS_WIFILI_INTERNAL_INTERFACE)
		&& (ncm->interface != NSS_WIFILI_EXTERNAL_INTERFACE0)
		&& (ncm->interface != NSS_WIFILI_EXTERNAL_INTERFACE1)) {
		nss_warning("%px: tx request for interface that is not a wifili: %d", nss_ctx, ncm->interface);
		return NSS_TX_FAILURE;
	}

	return nss_core_send_cmd(nss_ctx, msg, sizeof(*msg), NSS_NBUF_PAYLOAD_SIZE);
}
EXPORT_SYMBOL(nss_wifili_tx_msg);

/*
 * nss_wifili_tx_msg_sync()
 *	Transmit a wifili message to NSS firmware synchronously.
 */
nss_tx_status_t nss_wifili_tx_msg_sync(struct nss_ctx_instance *nss_ctx, struct nss_wifili_msg *nvm)
{
	nss_tx_status_t status;
	int ret = 0;

	down(&wifili_pvt.sem);
	wifili_pvt.cb = (void *)nvm->cm.cb;
	wifili_pvt.app_data = (void *)nvm->cm.app_data;

	nvm->cm.cb = (nss_ptr_t)nss_wifili_callback;
	nvm->cm.app_data = (nss_ptr_t)NULL;

	status = nss_wifili_tx_msg(nss_ctx, nvm);
	if (status != NSS_TX_SUCCESS) {
		nss_warning("%px: wifili_tx_msg failed\n", nss_ctx);
		up(&wifili_pvt.sem);
		return status;
	}

	ret = wait_for_completion_timeout(&wifili_pvt.complete, msecs_to_jiffies(NSS_WIFILI_TX_TIMEOUT));
	if (!ret) {
		nss_warning("%px: wifili msg tx failed due to timeout\n", nss_ctx);
		wifili_pvt.response = NSS_TX_FAILURE;
	}

	status = wifili_pvt.response;
	up(&wifili_pvt.sem);
	return status;
}
EXPORT_SYMBOL(nss_wifili_tx_msg_sync);

/*
 * nss_wifili_get_context()
 */
struct nss_ctx_instance *nss_wifili_get_context(void)
{
	return (struct nss_ctx_instance *)&nss_top_main.nss[nss_top_main.wifi_handler_id];
}
EXPORT_SYMBOL(nss_wifili_get_context);

/*
 * nss_wifili_release_external_if()
 *	Release the external interface.
 */
void nss_wifili_release_external_if(nss_if_num_t ifnum)
{
	uint32_t idx;

	spin_lock_bh(&nss_wifi_eif_info.lock);
	for (idx = 0; idx < NSS_WIFILI_EXTERNAL_INTERFACE_MAX; idx++) {
		if (nss_wifi_eif_info.state_tbl[idx].ifnum != ifnum) {
			continue;
		}

		if (!nss_wifi_eif_info.state_tbl[idx].in_use) {
			spin_unlock_bh(&nss_wifi_eif_info.lock);
			nss_warning("%px: I/F num:%d is not in use\n", &nss_wifi_eif_info, ifnum);
			return;
		}

		nss_wifi_eif_info.state_tbl[idx].in_use = false;
		break;
	}

	spin_unlock_bh(&nss_wifi_eif_info.lock);

	if (idx == NSS_WIFILI_EXTERNAL_INTERFACE_MAX) {
		nss_warning("%px: Trying to release invalid ifnum:%d\n", &nss_wifi_eif_info, ifnum);
	}
}
EXPORT_SYMBOL(nss_wifili_release_external_if);

/*
 * nss_get_available_wifili_external_if()
 *	Check and return the available external interface
 */
nss_if_num_t nss_get_available_wifili_external_if(void)
{
	nss_if_num_t ifnum = -1;
	uint32_t idx;

	/*
	 * Check if the external interface is registered.
	 * Return the interface number if not registered.
	 */
	spin_lock_bh(&nss_wifi_eif_info.lock);
	for (idx = 0; idx < NSS_WIFILI_EXTERNAL_INTERFACE_MAX; idx++) {
		if (nss_wifi_eif_info.state_tbl[idx].in_use) {
			continue;
		}

		nss_wifi_eif_info.state_tbl[idx].in_use = true;
		ifnum = nss_wifi_eif_info.state_tbl[idx].ifnum;
		break;
	}

	spin_unlock_bh(&nss_wifi_eif_info.lock);

	BUG_ON(idx == NSS_WIFILI_EXTERNAL_INTERFACE_MAX);
	return ifnum;
}
EXPORT_SYMBOL(nss_get_available_wifili_external_if);

/*
 * nss_wifili_get_radio_num()
 *     Get NSS wifili radio count.
 *
 * Wi-Fi host driver needs to know the current radio count
 * to extract the radio priority from ini file.
 */
uint32_t nss_wifili_get_radio_num(struct nss_ctx_instance *nss_ctx)
{
	uint8_t core_id;
	uint32_t radio_count;

	nss_assert(nss_ctx);
	nss_assert(nss_ctx->id < nss_top_main.num_nss);

	core_id = nss_ctx->id;

	spin_lock_bh(&ts_db[core_id].lock);
	radio_count = ts_db[core_id].radio_count;
	spin_unlock_bh(&ts_db[core_id].lock);

	return radio_count;
}
EXPORT_SYMBOL(nss_wifili_get_radio_num);

/*
 * nss_wifili_thread_scheme_alloc()
 *	Allocate NSS worker thread scheme index.
 *
 * API does search on scheme database and returns scheme index based on
 * priority of radio and free entry available.
 * Wi-Fi driver fetches radio priority from ini file and calls this API
 * to get the scheme index based on radio priority.
 *
 */
uint8_t nss_wifili_thread_scheme_alloc(struct nss_ctx_instance *nss_ctx,
					int32_t radio_ifnum,
					enum nss_wifili_thread_scheme_priority radio_priority)
{
	uint8_t i;
	uint8_t scheme_idx;
	uint8_t core_id;
	uint8_t next_avail_entry_idx = NSS_WIFILI_THREAD_SCHEME_ENTRY_MAX;

	nss_assert(nss_ctx);
	nss_assert(nss_ctx->id < nss_top_main.num_nss);

	core_id = nss_ctx->id;

	/*
	 * Iterate through scheme database and allocate
	 * scheme_id matching the priority requested.
	 */
	spin_lock_bh(&ts_db[core_id].lock);
	for (i = 0; i < NSS_WIFILI_THREAD_SCHEME_ENTRY_MAX; i++) {
		if (ts_db[core_id].nwtse[i].allocated) {
			continue;
		}

		if (radio_priority ==
				ts_db[core_id].nwtse[i].scheme_priority) {
			ts_db[core_id].nwtse[i].radio_ifnum = radio_ifnum;
			ts_db[core_id].nwtse[i].radio_priority = radio_priority;
			ts_db[core_id].nwtse[i].allocated = true;
			ts_db[core_id].radio_count++;
			scheme_idx = ts_db[core_id].nwtse[i].scheme_index;
			spin_unlock_bh(&ts_db[core_id].lock);

			nss_info("%px: Allocated scheme index:%d radio_ifnum:%d",
					nss_ctx,
					scheme_idx,
					radio_ifnum);

			return scheme_idx;
		}

		next_avail_entry_idx = i;
	}

	/*
	 * When radio priority does not match any of scheme entry priority
	 * and database has unallocated entries, provide available unallocated entry.
	 * This prevents any catastrophic failure during attach of Wi-Fi radio.
	 */
	if (next_avail_entry_idx != NSS_WIFILI_THREAD_SCHEME_ENTRY_MAX) {

		ts_db[core_id].nwtse[next_avail_entry_idx].radio_ifnum = radio_ifnum;
		ts_db[core_id].nwtse[next_avail_entry_idx].radio_priority = radio_priority;
		ts_db[core_id].nwtse[next_avail_entry_idx].allocated = true;
		ts_db[core_id].radio_count++;
		scheme_idx = ts_db[core_id].nwtse[next_avail_entry_idx].scheme_index;
		spin_unlock_bh(&ts_db[core_id].lock);

		nss_info("%px: Priority did not match for radio_ifnum:%d, allocated a next available scheme:%d",
				nss_ctx,
				radio_ifnum,
				scheme_idx);

		return scheme_idx;
	}
	spin_unlock_bh(&ts_db[core_id].lock);

	nss_warning("%px: Could not find scheme - radio_ifnum:%d radio_map:%d\n",
			nss_ctx,
			radio_ifnum,
			radio_priority);

	return NSS_WIFILI_INVALID_SCHEME_ID;
}
EXPORT_SYMBOL(nss_wifili_thread_scheme_alloc);

/*
 * nss_wifili_thread_scheme_dealloc()
 *	Reset thread scheme metadata.
 */
void nss_wifili_thread_scheme_dealloc(struct nss_ctx_instance *nss_ctx,
					int32_t radio_ifnum)
{
	uint32_t id;
	uint8_t core_id;

	nss_assert(nss_ctx);
	nss_assert(nss_ctx->id < nss_top_main.num_nss);

	core_id = nss_ctx->id;

	/*
	 * Radio count cannot be zero here.
	 */
	nss_assert(ts_db[core_id].radio_count);

	spin_lock_bh(&ts_db[core_id].lock);
	for (id = 0; id < NSS_WIFILI_THREAD_SCHEME_ENTRY_MAX; id++) {
		if (ts_db[core_id].nwtse[id].radio_ifnum != radio_ifnum) {
			continue;
		}

		ts_db[core_id].nwtse[id].radio_priority = 0;
		ts_db[core_id].nwtse[id].allocated = false;
		ts_db[core_id].nwtse[id].radio_ifnum = 0;
		ts_db[core_id].radio_count--;
		break;
	}
	spin_unlock_bh(&ts_db[core_id].lock);

	if (id == NSS_WIFILI_THREAD_SCHEME_ENTRY_MAX) {
		nss_warning("%px: Could not find scheme database with radio_ifnum:%d",
				nss_ctx,
				radio_ifnum);
	}
}
EXPORT_SYMBOL(nss_wifili_thread_scheme_dealloc);

/*
 * nss_wifili_thread_scheme_db_init()
 *	Initialize thread scheme database.
 */
void nss_wifili_thread_scheme_db_init(uint8_t core_id)
{
	uint32_t id;

	spin_lock_init(&ts_db[core_id].lock);

	/*
	 * Iterate through scheme database and assign
	 * scheme_id and priority for each entry
	 */
	ts_db[core_id].radio_count = 0;
	for (id = 0; id < NSS_WIFILI_THREAD_SCHEME_ENTRY_MAX; id++) {
		ts_db[core_id].nwtse[id].radio_priority = 0;
		ts_db[core_id].nwtse[id].radio_ifnum = 0;
		ts_db[core_id].nwtse[id].allocated = false;

		switch (id) {
		case 0:
			ts_db[core_id].nwtse[id].scheme_priority = NSS_WIFILI_HIGH_PRIORITY_SCHEME;
			ts_db[core_id].nwtse[id].scheme_index = NSS_WIFILI_THREAD_SCHEME_ID_0;
			break;
		case 1:
			ts_db[core_id].nwtse[id].scheme_priority = NSS_WIFILI_LOW_PRIORITY_SCHEME;
			ts_db[core_id].nwtse[id].scheme_index = NSS_WIFILI_THREAD_SCHEME_ID_1;
			break;
		case 2:
		case 3:
			ts_db[core_id].nwtse[id].scheme_priority = NSS_WIFILI_HIGH_PRIORITY_SCHEME;
			ts_db[core_id].nwtse[id].scheme_index = NSS_WIFILI_THREAD_SCHEME_ID_2;
			break;
		default:
			nss_warning("Invalid scheme index:%d", id);
		}
	}
}

/*
 * nss_wifili_msg_init()
 *	Initialize nss_wifili_msg.
 */
void nss_wifili_msg_init(struct nss_wifili_msg *ncm, uint16_t if_num, uint32_t type, uint32_t len, void *cb, void *app_data)
{
	nss_cmn_msg_init(&ncm->cm, if_num, type, len, cb, app_data);
}
EXPORT_SYMBOL(nss_wifili_msg_init);

/*
 ****************************************
 * Register/Unregister/Miscellaneous APIs
 ****************************************
 */

/*
 * nss_register_wifili_if()
 *	Register wifili with nss driver
 */
struct nss_ctx_instance *nss_register_wifili_if(uint32_t if_num, nss_wifili_callback_t wifili_callback,
			nss_wifili_callback_t wifili_ext_callback,
			nss_wifili_msg_callback_t event_callback, struct net_device *netdev, uint32_t features)
{
	struct nss_ctx_instance *nss_ctx = (struct nss_ctx_instance *)&nss_top_main.nss[nss_top_main.wifi_handler_id];

	/*
	 * The interface number shall be wifili soc interface
	 */
	nss_assert((if_num == NSS_WIFILI_INTERNAL_INTERFACE)
			|| (if_num == NSS_WIFILI_EXTERNAL_INTERFACE0)
			|| (if_num == NSS_WIFILI_EXTERNAL_INTERFACE1));

	nss_info("nss_register_wifili_if if_num %d wifictx %px", if_num, netdev);

	nss_core_register_subsys_dp(nss_ctx, if_num, wifili_callback, wifili_ext_callback, NULL, netdev, features);

	nss_top_main.wifili_msg_callback = event_callback;

	return (struct nss_ctx_instance *)&nss_top_main.nss[nss_top_main.wifi_handler_id];
}
EXPORT_SYMBOL(nss_register_wifili_if);

/*
 * nss_unregister_wifili_if()
 *	Unregister wifili with nss driver
 */
void nss_unregister_wifili_if(uint32_t if_num)
{
	struct nss_ctx_instance *nss_ctx = (struct nss_ctx_instance *)&nss_top_main.nss[nss_top_main.wifi_handler_id];

	/*
	 * The interface number shall be wifili soc interface
	 */
	nss_assert((if_num == NSS_WIFILI_INTERNAL_INTERFACE)
			|| (if_num == NSS_WIFILI_EXTERNAL_INTERFACE0)
			|| (if_num == NSS_WIFILI_EXTERNAL_INTERFACE1));

	nss_core_unregister_subsys_dp(nss_ctx, if_num);
	nss_wifili_release_external_if(if_num);
}
EXPORT_SYMBOL(nss_unregister_wifili_if);

/*
 * nss_register_wifili_radio_if()
 *	Register wifili radio with nss driver
 */
struct nss_ctx_instance *nss_register_wifili_radio_if(uint32_t if_num, nss_wifili_callback_t wifili_callback,
			nss_wifili_callback_t wifili_ext_callback,
			nss_wifili_msg_callback_t event_callback, struct net_device *netdev, uint32_t features)
{
	struct nss_ctx_instance *nss_ctx = (struct nss_ctx_instance *)&nss_top_main.nss[nss_top_main.wifi_handler_id];

	/*
	 * The interface number shall be wifili radio dynamic interface
	 */
	nss_assert(nss_is_dynamic_interface(if_num));
	nss_info("nss_register_wifili_if if_num %d wifictx %px", if_num, netdev);

	nss_core_register_subsys_dp(nss_ctx, if_num, wifili_callback, wifili_ext_callback, NULL, netdev, features);

	return (struct nss_ctx_instance *)&nss_top_main.nss[nss_top_main.wifi_handler_id];
}
EXPORT_SYMBOL(nss_register_wifili_radio_if);

/*
 * nss_unregister_wifili_radio_if()
 *	Unregister wifili radio with nss driver
 */
void nss_unregister_wifili_radio_if(uint32_t if_num)
{
	struct nss_ctx_instance *nss_ctx = (struct nss_ctx_instance *)&nss_top_main.nss[nss_top_main.wifi_handler_id];

	/*
	 * The interface number shall be wifili radio dynamic interface
	 */
	nss_assert(nss_is_dynamic_interface(if_num));

	nss_core_unregister_subsys_dp(nss_ctx, if_num);
}
EXPORT_SYMBOL(nss_unregister_wifili_radio_if);

/*
 * nss_wifili_register_handler()
 *	Register handle for notfication messages received on wifi interface
 */
void nss_wifili_register_handler(void)
{
	struct nss_ctx_instance *nss_ctx = (struct nss_ctx_instance *)&nss_top_main.nss[nss_top_main.wifi_handler_id];
	uint32_t idx;

	nss_info("nss_wifili_register_handler");
	nss_core_register_handler(nss_ctx, NSS_WIFILI_INTERNAL_INTERFACE, nss_wifili_handler, NULL);
	nss_core_register_handler(nss_ctx, NSS_WIFILI_EXTERNAL_INTERFACE0, nss_wifili_handler, NULL);
	nss_core_register_handler(nss_ctx, NSS_WIFILI_EXTERNAL_INTERFACE1, nss_wifili_handler, NULL);

	nss_wifili_stats_dentry_create();
	nss_wifili_strings_dentry_create();

	sema_init(&wifili_pvt.sem, 1);
	init_completion(&wifili_pvt.complete);

	/*
	 * Intialize the external interfaces info.
	 */
	spin_lock_init(&nss_wifi_eif_info.lock);
	for (idx = 0; idx < NSS_WIFILI_EXTERNAL_INTERFACE_MAX; idx++) {
		nss_wifi_eif_info.state_tbl[idx].ifnum = nss_wifili_external_tbl[idx];
		nss_wifi_eif_info.state_tbl[idx].in_use = false;
	}
}

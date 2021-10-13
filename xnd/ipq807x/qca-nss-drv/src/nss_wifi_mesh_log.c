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

/*
 * nss_wifi_mesh_log.c
 *	NSS WiFi Mesh logger file.
 */

#include "nss_core.h"
#include "nss_wifi_mesh.h"

#define NSS_WIFI_MESH_LOG_MESSAGE_TYPE_INDEX(type) ((type) - NSS_IF_MAX_MSG_TYPES)

/*
 * nss_wifi_mesh_log_message_types_str
 *	NSS Wi-Fi mesh message strings.
 */
static uint8_t *nss_wifi_mesh_log_message_types_str[NSS_WIFI_MESH_LOG_MESSAGE_TYPE_INDEX(NSS_WIFI_MESH_MSG_MAX)] __maybe_unused = {
	"WiFi Mesh configure",
	"WiFi Mesh configure Mpath Add",
	"WiFi Mesh configure Mpath Delete",
	"WiFi Mesh configure Mpath Update",
	"WiFi Mesh configure Proxy Learn",
	"WiFi Mesh configure Proxy Add",
	"WiFi Mesh configure Proxy Update",
	"WiFi Mesh configure Proxy Delete",
	"WiFi Mesh configure Mpath Not Found",
	"WiFi Mesh configure Refresh"
	"WiFi Mesh configure Mpath Table Dump",
	"WiFi Mesh configure Proxy Path Table Dump",
	"WiFi Mesh configure Assoc Link Vap",
	"WiFi Mesh configure Exception Message",
	"WiFi Mesh configure Stats Sync"
};

/*
 * nss_wifi_mesh_log_configure_msg()
 *	Log a NSS Wi-Fi mesh interface configure message.
 */
static void nss_wifi_mesh_log_configure_if_msg(struct nss_wifi_mesh_msg *nwmm)
{
	struct nss_wifi_mesh_config_msg *cmsg __maybe_unused = &nwmm->msg.mesh_config;
	nss_trace("%px: WiFi Mesh configure message\n"
		"Local Mac address: %pM\n"
		"TTL: %d\n"
		"Mesh Path Refresh Time: %d\n"
		"Mpp Learning Mode: %d\n"
		"Block Mesh Forwarding: %d\n"
		"Configs Flags: 0x%x\n",
		cmsg, cmsg->local_mac_addr, cmsg->ttl,
		cmsg->mesh_path_refresh_time,
		cmsg->mpp_learning_mode,
		cmsg->block_mesh_forwarding,
		cmsg->config_flags);
}

/*
 * nss_wifi_mesh_log_mpath_add_msg()
 *	Log a NSS Wi-Fi mesh mpath add message.
 */
static void nss_wifi_mesh_log_mpath_add_msg(struct nss_wifi_mesh_msg *nwmm)
{
	struct nss_wifi_mesh_mpath_add_msg *mamsg __maybe_unused = &nwmm->msg.mpath_add;
	nss_trace("%px: NSS WiFi Mesh Mpath add message:\n"
		"Dest Mac address: %pM\n"
		"Next Hop Mac address: %pM\n"
		"Metric: %d\n"
		"Expiry Time: %d\n"
		"Hop Count: %d\n"
		"Flags: 0x%x\n"
		"Link Vap id: %d\n"
		"Is Mesh Gate: %d\n",
		mamsg, mamsg->dest_mac_addr, mamsg->next_hop_mac_addr,
		mamsg->metric, mamsg->expiry_time, mamsg->hop_count,
		mamsg->path_flags, mamsg->link_vap_id, mamsg->is_mesh_gate);
}

/*
 * nss_wifi_mesh_log_mpath_delete_msg()
 *	Log a NSS Wi-Fi mesh mpath delete message.
 */
static void nss_wifi_mesh_log_mpath_delete_msg(struct nss_wifi_mesh_msg *nwmm)
{
	struct nss_wifi_mesh_mpath_del_msg *mdmsg __maybe_unused = &nwmm->msg.mpath_del;
	nss_trace("%px: NSS WiFi Mesh Mpath delete message:\n"
		"Dest Mac Address: %pM\n"
		"Link Vap id: %d\n"
		"Next Hop Mac address: %pM\n",
		mdmsg, mdmsg->mesh_dest_mac_addr, mdmsg->link_vap_id, mdmsg->next_hop_mac_addr);
}

/*
 * nss_wifi_mesh_log_mpath_update_msg()
 *	Log a NSS Wi-Fi mesh mpath update message.
 */
static void nss_wifi_mesh_log_mpath_update_msg(struct nss_wifi_mesh_msg *nwmm)
{
	struct nss_wifi_mesh_mpath_update_msg *mumsg __maybe_unused = &nwmm->msg.mpath_update;
	nss_trace("%px: NSS WiFi Mesh Mpath update message:\n"
		"Dest Mac address: %pM\n"
		"Next Hop Mac address: %pM\n"
		"Metric: %d\n"
		"Expiry Time: %d\n"
		"Hop Count: %d\n"
		"Flags: 0x%x\n"
		"Link Vap id: %d\n"
		"Is Mesh Gate: %d\n"
		"Update Flags: %d\n",
		mumsg, mumsg->dest_mac_addr, mumsg->next_hop_mac_addr,
		mumsg->metric, mumsg->expiry_time, mumsg->hop_count,
		mumsg->path_flags, mumsg->link_vap_id, mumsg->is_mesh_gate,
		mumsg->update_flags);
}

/*
 * nss_wifi_mesh_log_proxy_path_learn_msg()
 *	Log a NSS Wi-Fi mesh proxy path learn message.
 */
static void nss_wifi_mesh_log_proxy_path_learn_msg(struct nss_wifi_mesh_msg *nwmm)
{
	struct nss_wifi_mesh_proxy_path_learn_msg *pplm __maybe_unused = &nwmm->msg.proxy_learn_msg;
	nss_trace("%px: NSS WiFi Mesh Proxy Path Learn message:\n"
		"Mesh Dest Mac address: %pM\n"
		"Destination Mac address: %pM\n"
		"flags: 0x%x\n",
		pplm, pplm->mesh_dest_mac, pplm->dest_mac_addr,
		pplm->path_flags);
}

/*
 * nss_wifi_mesh_log_proxy_path_add_msg()
 *	Log a NSS Wi-Fi Mesh proxy path add message.
 */
static void nss_wifi_mesh_log_proxy_path_add_msg(struct nss_wifi_mesh_msg *nwmm)
{
	struct nss_wifi_mesh_proxy_path_add_msg *ppam __maybe_unused = &nwmm->msg.proxy_add_msg;
	nss_trace("%px: NSS WiFi Mesh Proxy Path Add message:\n"
		"Mesh Dest Mac address: %pM\n"
		"Destination Mac address: %pM\n"
		"flags: 0x%x\n",
		ppam, ppam->mesh_dest_mac, ppam->dest_mac_addr,
		ppam->path_flags);
}

/*
 * nss_wifi_mesh_log_proxy_path_delete_msg()
 *	Log a NSS Wi-Fi proxy path delete message.
 */
static void nss_wifi_mesh_log_proxy_path_delete_msg(struct nss_wifi_mesh_msg *nwmm)
{
	struct nss_wifi_mesh_proxy_path_del_msg *ppdm __maybe_unused = &nwmm->msg.proxy_del_msg;
	nss_trace("%px: NSS WiFi Mesh Proxy Path Delete message:\n"
		"Mesh Dest Mac address: %pM\n"
		"Destination Mac address: %pM\n",
		ppdm, ppdm->mesh_dest_mac_addr, ppdm->dest_mac_addr);
}

/*
 * nss_wifi_mesh_log_proxy_path_update_msg()
 *	Log a NSS Wi-Fi mesh proxy path update message.
 */
static void nss_wifi_mesh_log_proxy_path_update_msg(struct nss_wifi_mesh_msg *nwmm)
{
	struct nss_wifi_mesh_proxy_path_update_msg *ppum __maybe_unused = &nwmm->msg.proxy_update_msg;
	nss_trace("%px: NSS WiFi Mesh Proxy Path Add message:\n"
		"Mesh Dest Mac address: %pM\n"
		"Destination Mac address: %pM\n"
		"flags: 0x%x\n"
		"Bitmap: %d\n",
		ppum, ppum->mesh_dest_mac, ppum->dest_mac_addr,
		ppum->path_flags, ppum->bitmap);
}

/*
 * nss_wifi_mesh_log_mpath_not_found_msg()
 *	Log a NSS Wi-Fi mesh mpath not found message.
 */
static void nss_wifi_mesh_log_mpath_not_found_msg(struct nss_wifi_mesh_msg *nwmm)
{
	struct nss_wifi_mesh_mpath_not_found_msg *mnfm __maybe_unused = &nwmm->msg.mpath_not_found_msg;
	nss_trace("%px: NSS WiFi Mesh Mpath not found message:\n"
		"Destination Mac address: %pM\n"
		"Transmitter Mac address: %pM\n"
		"Link Vap Id: %d\n"
		"Is Mesh Forwarding Path: %d\n",
		mnfm, mnfm->dest_mac_addr, mnfm->transmitter_mac_addr,
		mnfm->link_vap_id, mnfm->is_mesh_forward_path);
}

/*
 * nss_wifi_mesh_log_mpath_refresh_msg()
 *	Log a NSS Wi-Fi mesh mpath refresh message.
 */
static void nss_wifi_mesh_log_mpath_refresh_msg(struct nss_wifi_mesh_msg *nwmm)
{
	struct nss_wifi_mesh_path_refresh_msg *mprm __maybe_unused = &nwmm->msg.path_refresh_msg;
	nss_trace("%px: NSS WiFi Mesh Mpath refresh message:\n"
		"Destination Mac address: %pM\n"
		"Next Hop Mac address: %pM\n"
		"Flags: 0x%x\n"
		"Link Vap Id: %d\n",
		mprm, mprm->dest_mac_addr, mprm->next_hop_mac_addr,
		mprm->path_flags, mprm->link_vap_id);
}

/*
 * nss_wifi_mesh_log_mpath_expiry_msg()
 *	Log a NSS Wi-Fi mesh mpath expiry message.
 */
static void nss_wifi_mesh_log_mpath_expiry_msg(struct nss_wifi_mesh_msg *nwmm)
{
	struct nss_wifi_mesh_path_expiry_msg *mpem __maybe_unused = &nwmm->msg.path_expiry_msg;
	nss_trace("%px: NSS WiFi Mesh Mpath expiry message:\n"
		"Destination Mac address: %pM\n"
		"Next Hop Mac address: %pM\n"
		"Flags: 0x%x\n"
		"Link Vap Id: %d\n",
		mpem, mpem->mesh_dest_mac_addr, mpem->next_hop_mac_addr,
		mpem->path_flags, mpem->link_vap_id);
}

/*
 * nss_wifi_mesh_log_exception_flag_msg()
 *	Log a NSS Wi-Fi mesh exception flag message.
 */
static void nss_wifi_mesh_log_exception_flag_msg(struct nss_wifi_mesh_msg *nwmm)
{
	struct nss_wifi_mesh_exception_flag_msg *efm __maybe_unused = &nwmm->msg.exception_msg;
	nss_trace("%px: NSS WiFi Mesh Exception Flag message:\n"
		"Destination Mac address: %pM\n",
		efm, efm->dest_mac_addr);
}

/*
 * nss_wifi_mesh_log_verbose()
 *	Log message contents.
 */
static void nss_wifi_mesh_log_verbose(struct nss_wifi_mesh_msg *nwmm)
{
	switch (nwmm->cm.type) {
	case NSS_WIFI_MESH_MSG_INTERFACE_CONFIGURE:
		nss_wifi_mesh_log_configure_if_msg(nwmm);
		break;

	case NSS_WIFI_MESH_MSG_MPATH_ADD:
		nss_wifi_mesh_log_mpath_add_msg(nwmm);
		break;

	case NSS_WIFI_MESH_MSG_MPATH_DELETE:
		nss_wifi_mesh_log_mpath_delete_msg(nwmm);
		break;

	case NSS_WIFI_MESH_MSG_MPATH_UPDATE:
		nss_wifi_mesh_log_mpath_update_msg(nwmm);
		break;

	case NSS_WIFI_MESH_MSG_PROXY_PATH_LEARN:
		nss_wifi_mesh_log_proxy_path_learn_msg(nwmm);
		break;

	case NSS_WIFI_MESH_MSG_PROXY_PATH_ADD:
		nss_wifi_mesh_log_proxy_path_add_msg(nwmm);
		break;

	case NSS_WIFI_MESH_MSG_PROXY_PATH_DELETE:
		nss_wifi_mesh_log_proxy_path_delete_msg(nwmm);
		break;

	case NSS_WIFI_MESH_MSG_PROXY_PATH_UPDATE:
		nss_wifi_mesh_log_proxy_path_update_msg(nwmm);
		break;

	case NSS_WIFI_MESH_MSG_PATH_NOT_FOUND:
		nss_wifi_mesh_log_mpath_not_found_msg(nwmm);
		break;

	case NSS_WIFI_MESH_MSG_PATH_REFRESH:
		nss_wifi_mesh_log_mpath_refresh_msg(nwmm);
		break;

	case NSS_WIFI_MESH_MSG_PATH_EXPIRY:
		nss_wifi_mesh_log_mpath_expiry_msg(nwmm);
		break;

	case NSS_WIFI_MESH_MSG_PATH_TABLE_DUMP:
		break;

	case NSS_WIFI_MESH_MSG_PROXY_PATH_TABLE_DUMP:
		break;

	case NSS_WIFI_MESH_MSG_STATS_SYNC:
		break;

	case NSS_WIFI_MESH_MSG_EXCEPTION_FLAG:
		nss_wifi_mesh_log_exception_flag_msg(nwmm);
		break;

	default:
		nss_trace("%px: Invalid message, type: %d\n", nwmm, nwmm->cm.type);
		break;
	}
}

/*
 * nss_wifi_mesh_log_tx_msg()
 *	Log messages transmitted to firmware.
 */
void nss_wifi_mesh_log_tx_msg(struct nss_wifi_mesh_msg *nwmm)
{
	uint32_t index;
	if ((nwmm->cm.type >= NSS_WIFI_MESH_MSG_MAX) || (nwmm->cm.type <= NSS_IF_MAX_MSG_TYPES)) {
		nss_warning("%px: Invalid message, type: %d\n", nwmm, nwmm->cm.type);
		return;
	}

	index = NSS_WIFI_MESH_LOG_MESSAGE_TYPE_INDEX(nwmm->cm.type);

	nss_info("%px: type[%d]:%s\n", nwmm, nwmm->cm.type, nss_wifi_mesh_log_message_types_str[index - 1]);
	nss_wifi_mesh_log_verbose(nwmm);
}

/*
 * nss_wifi_mesh_log_rx_msg()
 *	Log messages received from firmware.
 */
void nss_wifi_mesh_log_rx_msg(struct nss_wifi_mesh_msg *nwmm)
{
	uint32_t index;
	if (nwmm->cm.response >= NSS_CMN_RESPONSE_LAST) {
		nss_warning("%px: Invalid response, message type: %d\n", nwmm, nwmm->cm.type);
		return;
	}

	if (nwmm->cm.type <= NSS_IF_MAX_MSG_TYPES) {
		return;
	}

	index = NSS_WIFI_MESH_LOG_MESSAGE_TYPE_INDEX(nwmm->cm.type);

	if (nwmm->cm.response == NSS_CMN_RESPONSE_NOTIFY || (nwmm->cm.response == NSS_CMN_RESPONSE_ACK)) {
		nss_info("%px: type[%d]:%s, response[%d]:%s\n", nwmm, nwmm->cm.type,
			nss_wifi_mesh_log_message_types_str[index - 1],
			nwmm->cm.response, nss_cmn_response_str[nwmm->cm.response]);
		goto verbose;
	}

	nss_info("%px: msg nack - type[%d]:%s, response[%d]:%s\n",
		nwmm, nwmm->cm.type, nss_wifi_mesh_log_message_types_str[index - 1],
		nwmm->cm.response, nss_cmn_response_str[nwmm->cm.response]);

verbose:
	nss_wifi_mesh_log_verbose(nwmm);
}

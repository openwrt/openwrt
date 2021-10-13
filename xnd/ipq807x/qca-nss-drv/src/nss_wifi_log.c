/*
 **************************************************************************
 * Copyright (c) 2018, 2020, The Linux Foundation. All rights reserved.
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
 * nss_wifi_log.c
 *	NSS WIFI logger file.
 */

#include "nss_core.h"

/*
 * nss_wifi_log_message_types_str
 *	WIFI message strings
 */
static int8_t *nss_wifi_log_message_types_str[NSS_WIFI_MAX_MSG] __maybe_unused = {
	"WIFI INIT MSG",
	"WIFI POST RECV MSG",
	"WIFI HTT INIT MSG",
	"WIFI TX INIT MSG",
	"WIFI RAW SEND MSG",
	"WIFI MGMT SEND MSG",
	"WIFI WDS PEER ADD MSG",
	"WIFI WDS PEER DEL MSG",
	"WIFI STOP MSG",
	"WIFI RESET MSG",
	"WIFI STATS MSG",
	"WIFI PEER FREELIST APPEND MSG",
	"WIFI RX REORDER ARRAY FREELIST APPEND MSG",
	"WIFI SEND PEER MEMORY REQUEST MSG",
	"WIFI SEND RRA MEMORY REQUEST MSG",
	"WIFI FW STATS MSG",
	"WIFI MONITOR FILTER SET MSG",
	"WIFI PEER BS STATE MSG",
	"WIFI MSDU TTL SET MSG",
	"WIFI RX VOW EXTSTATS SET MSG",
	"WIFI PKTLOG CFG MSG",
	"WIFI ENABLE PERPKT TXSTATS MSG",
	"WIFI IGMP MLD TOS OVERRIDE MSG",
	"WIFI OL STATS CFG MSG",
	"WIFI OL STATS MSG",
	"WIFI TX QUEUE CFG MSG",
	"WIFI TX MIN THRESHOLD CFG MSG",
	"WIFI DBDC PROCESS ENABLE MSG",
	"WIFI PRIMARY RADIO SET MSG",
	"WIFI FORCE CLIENT MCAST TRAFFIC SET MSG",
	"WIFI STORE OTHER PDEV STAVAP MSG",
	"WIFI STA KICKOUT MSG",
	"WIFI WNM PEER RX ACTIVITY MSG",
	"WIFI PEER STATS MSG",
	"WIFI WDS VENDOR MSG",
	"WIFI TX CAPTURE SET MSG",
	"WIFI ALWAYS PRIMARY SET MSG",
	"WIFI FLUSH HTT CMD MSG",
	"WIFI CMD MSG",
	"WIFI ENABLE OL STATSV2 MSG",
	"WIFI OL PEER TIME MSG",
};

/*
 * nss_wifi_log_error_response_types_str
 *	Strings for error types for WIFI messages
 */
static int8_t *nss_wifi_log_error_response_types_str[NSS_WIFI_EMSG_MAX] __maybe_unused = {
	"WIFI NO ERROR",
	"WIFI UNKNOWN MSG",
	"WIFI MGMT DLEN",
	"WIFI MGMT SEND",
	"WIFI CE INIT FAIL",
	"WIFI PDEV INIT FAIL",
	"WIFI HTT INIT FAIL",
	"WIFI PEER ADD",
	"WIFI WIFI START FAIL",
	"WIFI STATE NOT RESET",
	"WIFI STATE NOT INIT DONE",
	"WIFI STATE NULL CE HANDLE",
	"WIFI STATE NOT CE READY",
	"WIFI STATE NOT HTT READY",
	"WIFI FW STATS DLEN",
	"WIFI FW STATS SEND",
	"WIFI STATE TX INIT FAILED",
	"WIFI IGMP MLD TOS OVERRIDE CFG",
	"WIFI PDEV INVALID",
	"WIFI OTHER PDEV STAVAP INVALID",
	"WIFI HTT SEND FAIL",
	"WIFI CE RING INIT",
	"WIFI NOTIFY CB",
	"WIFI PEERID INVALID",
	"WIFI PEER INVALID",
	"WIFI UNKNOWN CMD"
};

/*
 * nss_wifi_log_init_msg()
 *	Log NSS WIFI Init message.
 */
static void nss_wifi_log_init_msg(struct nss_wifi_msg *ncm)
{
	struct nss_wifi_init_msg *nwim __maybe_unused = &ncm->msg.initmsg;

	nss_trace("%px: NSS WIFI Init Message:\n"
		"WIFI Radio ID: %d\n"
		"WIFI PCI Memory Address: %x\n"
		"WIFI Target Type: %d\n"
		"WIFI MU MIMO Enhancement Enable Flag: %d\n"
		"WIFI Transmit Copy Engine Source Ring:\n"
		"\tNumber of Entries: %d\n"
		"\tNumber of Entries Mask: %x\n"
		"\tInitial Software Index: %d\n"
		"\tInitial Write Index: %d\n"
		"\tInitial Hardware Index: %d\n"
		"\tPhysical Address: %x\n"
		"\tVirtual Address: %x\n"
		"WIFI Transmit Copy Engine Dest Ring:\n"
		"\tNumber of Entries: %d\n"
		"\tNumber of Entries Mask: %x\n"
		"\tInitial Software Index: %d\n"
		"\tInitial Write Index: %d\n"
		"\tInitial Hardware Index: %d\n"
		"\tPhysical Address: %x\n"
		"\tVirtual Address: %x\n"
		"WIFI Transmit Control Address of PCIe Bar: %x\n"
		"WIFI Receive Copy Engine Source Ring:\n"
		"\tNumber of Entries: %d\n"
		"\tNumber of Entries Mask: %x\n"
		"\tInitial Software Index: %d\n"
		"\tInitial Write Index: %d\n"
		"\tInitial Hardware Index: %d\n"
		"\tPhysical Address: %x\n"
		"\tVirtual Address: %x\n"
		"WIFI Receive Copy Engine Dest Ring:\n"
		"\tNumber of Entries: %d\n"
		"\tNumber of Entries Mask: %x\n"
		"\tInitial Software Index: %d\n"
		"\tInitial Write Index: %d\n"
		"\tInitial Hardware Index: %d\n"
		"\tPhysical Address: %x\n"
		"\tVirtual Address: %x\n"
		"WIFI Receive Control Address of PCIe Bar: %x\n"
		"WIFI Bypass Network Process: %d",
		nwim, nwim->radio_id,
		nwim->pci_mem, nwim->target_type,
		nwim->mu_mimo_enhancement_en, nwim->ce_tx_state.src_ring.nentries,
		nwim->ce_tx_state.src_ring.nentries_mask, nwim->ce_tx_state.src_ring.sw_index,
		nwim->ce_tx_state.src_ring.write_index, nwim->ce_tx_state.src_ring.hw_index,
		nwim->ce_tx_state.src_ring.base_addr_CE_space, nwim->ce_tx_state.src_ring.base_addr_owner_space,
		nwim->ce_tx_state.dest_ring.nentries, nwim->ce_tx_state.dest_ring.nentries_mask,
		nwim->ce_tx_state.dest_ring.sw_index, nwim->ce_tx_state.dest_ring.write_index,
		nwim->ce_tx_state.dest_ring.hw_index, nwim->ce_tx_state.dest_ring.base_addr_CE_space,
		nwim->ce_tx_state.dest_ring.base_addr_owner_space, nwim->ce_tx_state.ctrl_addr,
		nwim->ce_rx_state.src_ring.nentries, nwim->ce_rx_state.src_ring.nentries_mask,
		nwim->ce_rx_state.src_ring.sw_index, nwim->ce_rx_state.src_ring.write_index,
		nwim->ce_rx_state.src_ring.hw_index, nwim->ce_rx_state.src_ring.base_addr_CE_space,
		nwim->ce_rx_state.src_ring.base_addr_owner_space, nwim->ce_rx_state.dest_ring.nentries,
		nwim->ce_rx_state.dest_ring.nentries_mask, nwim->ce_rx_state.dest_ring.sw_index,
		nwim->ce_rx_state.dest_ring.write_index, nwim->ce_rx_state.dest_ring.hw_index,
		nwim->ce_rx_state.dest_ring.base_addr_CE_space, nwim->ce_rx_state.dest_ring.base_addr_owner_space,
		nwim->ce_rx_state.ctrl_addr, nwim->bypass_nw_process);
}

/*
 * nss_wifi_log_stop_msg()
 *	Log NSS WIFI Init message.
 */
static void nss_wifi_log_stop_msg(struct nss_wifi_msg *ncm)
{
	struct nss_wifi_stop_msg *nwsm __maybe_unused = &ncm->msg.stopmsg;
	nss_trace("%px: NSS WIFI Init Message:\n"
		"WIFI Radio ID: %d\n",
		nwsm, nwsm->radio_id);
}

/*
 * nss_wifi_log_reset_msg()
 *	Log NSS WIFI Init message.
 */
static void nss_wifi_log_reset_msg(struct nss_wifi_msg *ncm)
{
	struct nss_wifi_reset_msg *nwrm __maybe_unused = &ncm->msg.resetmsg;
	nss_trace("%px: NSS WIFI Init Message:\n"
		"WIFI Radio ID: %d\n",
		nwrm, nwrm->radio_id);
}

/*
 * nss_wifi_log_htt_init_msg()
 *	Log NSS WIFI HTT Init message.
 */
static void nss_wifi_log_htt_init_msg(struct nss_wifi_msg *ncm)
{
	struct nss_wifi_htt_init_msg *nwim __maybe_unused = &ncm->msg.httinitmsg;
	nss_trace("%px: NSS WIFI HTT Init Message:\n"
		"WIFI Radio ID: %d\n"
		"WIFI Ring Size: %d\n"
		"WIFI Fill Level: %d\n"
		"WIFI MAC Hardware Ring Phy Address: %x\n"
		"WIFI MAC Hardware Ring Virtual Address: %x\n"
		"WIFI Hardware Ring Index Phy Address: %x\n"
		"WIFI Hardware Ring Index Virtual Address: %x\n",
		nwim, nwim->radio_id,
		nwim->ringsize, nwim->fill_level,
		nwim->paddrs_ringptr, nwim->paddrs_ringpaddr,
		nwim->alloc_idx_paddr, nwim->alloc_idx_vaddr);
}

/*
 * nss_wifi_log_tx_init_msg()
 *	Log NSS TX HTT Init message.
 */
static void nss_wifi_log_tx_init_msg(struct nss_wifi_msg *ncm)
{
	struct nss_wifi_tx_init_msg *nwim __maybe_unused = &ncm->msg.pdevtxinitmsg;
	nss_trace("%px: NSS WIFI HTT Init Message:\n"
		"WIFI Radio ID: %d\n"
		"WIFI Number of Descriptor Pools Allocated: %d\n"
		"WIFI TX Descriptor Array: %x\n"
		"WIFI MAC extenstion descriptor Address: %x\n"
		"WIFI WLAN MAC extenstion descriptor size: %d\n"
		"WIFI HTT Tx descriptor memory start virtual address: %x\n"
		"WIFI HTT Tx descriptor memory base virtual address: %x\n"
		"WIFI HTT Tx descriptor memory offset: %x\n"
		"WIFI Firmware shared TID map: %x\n",
		nwim, nwim->radio_id,
		nwim->desc_pool_size, nwim->tx_desc_array,
		nwim->wlanextdesc_addr, nwim->wlanextdesc_size,
		nwim->htt_tx_desc_base_vaddr, nwim->htt_tx_desc_base_paddr,
		nwim->htt_tx_desc_offset, nwim->pmap_addr);
}

/*
 * nss_wifi_log_rawsend_msg()
 *	Log NSS WIFI RAW Send message.
 */
static void nss_wifi_log_rawsend_msg(struct nss_wifi_msg *ncm)
{
	struct nss_wifi_rawsend_msg *nwrm __maybe_unused = &ncm->msg.rawmsg;
	nss_trace("%px: NSS WIFI RAW Send Message:\n"
		"WIFI Radio ID: %d\n"
		"WIFI Size of Raw Data: %d\n"
		"WIFI Raw Data: %px",
		nwrm, nwrm->radio_id,
		nwrm->len, nwrm->array);
}

/*
 * nss_wifi_log_mgmtsend_msg()
 *	Log NSS WIFI Management Send message.
 */
static void nss_wifi_log_mgmtsend_msg(struct nss_wifi_msg *ncm)
{
	struct nss_wifi_mgmtsend_msg *nwmm __maybe_unused = &ncm->msg.mgmtmsg;
	nss_trace("%px: NSS WIFI Management Send Message:\n"
		"WIFI Descriptor ID: %d\n"
		"WIFI Size of Management Data: %d\n"
		"WIFI Management Data: %px",
		nwmm, nwmm->desc_id,
		nwmm->len, nwmm->array);
}

/*
 * nss_wifi_log_wds_peer_msg()
 *	Log NSS WIFI WDS Peer message.
 */
static void nss_wifi_log_wds_peer_msg(struct nss_wifi_msg *ncm)
{
	struct nss_wifi_wds_peer_msg *nwmm __maybe_unused = &ncm->msg.pdevwdspeermsg;
	nss_trace("%px: NSS WIFI WDS Peer Message:\n"
		"WIFI Dest MAC: %pM\n"
		"WIFI Peer MAC: %pM\n",
		nwmm, nwmm->dest_mac,
		nwmm->peer_mac);
}

/*
 * nss_wifi_log_peer_freelist_append_msg()
 *	Log NSS WIFI Create/Append Freelist message
 */
static void nss_wifi_log_peer_freelist_append_msg(struct nss_wifi_msg *ncm)
{
	struct nss_wifi_peer_freelist_append_msg *nwpm __maybe_unused = &ncm->msg.peer_freelist_append;
	nss_trace("%px: NSS WIFI Create/Append Freelist Message:\n"
		"WIFI Starting Address of Peer Freelist Pool: %x\n"
		"WIFI Length of freelist pool: %d\n"
		"WIFI Number of Peers supported in freelist pool: %d\n",
		nwpm, nwpm->addr,
		nwpm->length, nwpm->num_peers);
}

/*
 * nss_wifi_log_rx_reorder_array_freelist_append_msg()
 *	Log NSS WIFI RX Reorder Array Freelist message
 */
static void nss_wifi_log_rx_reorder_array_freelist_append_msg(struct nss_wifi_msg *ncm)
{
	struct nss_wifi_rx_reorder_array_freelist_append_msg *nwpm __maybe_unused = &ncm->msg.rx_reorder_array_freelist_append;
	nss_trace("%px: NSS WIFI RX Reorder Array Freelist Message:\n"
		"WIFI Starting Address of TIDQ Freelist Pool: %x\n"
		"WIFI Length of TIDQ freelist pool: %d\n"
		"WIFI Number of Rx reorder array entries supported in freelist pool: %d\n",
		nwpm, nwpm->addr,
		nwpm->length, nwpm->num_rra);
}

/*
 * nss_wifi_log_set_filter_msg()
 *	Log NSS WIFI Set Filter message
 */
static void nss_wifi_log_set_filter_msg(struct nss_wifi_msg *ncm)
{
	struct nss_wifi_monitor_set_filter_msg *nwfm __maybe_unused = &ncm->msg.monitor_filter_msg;
	nss_trace("%px: NSS WIFI Set Filter Message:\n"
		"WIFI Filter Type: %dn",
		nwfm, nwfm->filter_type);
}

/*
 * nss_wifi_log_peer_activity_msg()
 *	Log NSS WIFI Get Active Peer for Radio message
 */
static void nss_wifi_log_peer_activity_msg(struct nss_wifi_msg *ncm)
{
	struct nss_wifi_bs_peer_activity *nwpm __maybe_unused = &ncm->msg.peer_activity;
	nss_trace("%px: NSS WIFI Get Active Peer Message:\n"
		"WIFI Number of Entries in Peer ID Array: %d\n"
		"WIFI PEER ID: %d\n",
		nwpm, nwpm->nentries,
		nwpm->peer_id[0]);
}

/*
 * nss_wifi_rx_vow_extstats_set_msg()
 *	Log NSS WIFI VoW Extended Statistics Set Message.
 */
static void nss_wifi_log_rx_vow_extstats_set_msg(struct nss_wifi_msg *ncm)
{
	struct nss_wifi_rx_vow_extstats_set_msg *nwpm __maybe_unused = &ncm->msg.vow_extstats_msg;
	nss_trace("%px: NSS WIFI VoW Extended Statistics Set Message:\n"
		"WIFI VoW Extended Statistics Enable:: %d\n",
		nwpm, nwpm->vow_extstats_en);
}

/*
 * nss_wifi_log_pktlog_cfg_msg()
 *	Log NSS WIFI Packet Log Configuration Message.
 */
static void nss_wifi_log_pktlog_cfg_msg(struct nss_wifi_msg *ncm)
{
	struct nss_wifi_pktlog_cfg_msg *nwpm __maybe_unused = &ncm->msg.pcm_msg;
	nss_trace("%px: NSS WIFI Packet Log Configuration Message:\n"
		"WIFI Packet Log Enable: %d\n"
		"WIFI PAcket Log buffer Size: %d\n"
		"WIFI Size of packet log header: %d\n"
		"WIFI Offset for the MSDU ID: %d\n",
		nwpm, nwpm->enable,
		nwpm->bufsize, nwpm->hdrsize,
		nwpm->msdu_id_offset);
}

/*
 * nss_wifi_log_enable_perpkt_txstats_msg()
 *	Log NSS WIFI Enable TX Stats Message.
 */
static void nss_wifi_log_enable_perpkt_txstats_msg(struct nss_wifi_msg *ncm)
{
	struct nss_wifi_enable_perpkt_txstats_msg *nwpm __maybe_unused = &ncm->msg.ept_msg;
	nss_trace("%px: NSS WIFI Enable TX Stats Message:\n"
		"WIFI TX Stats Enable Flag: %d\n",
		nwpm, nwpm->perpkt_txstats_flag);
}

/*
 * nss_wifi_log_override_tos_msg()
 *	Log NSS WIFI Override TOS Message.
 */
static void nss_wifi_log_override_tos_msg(struct nss_wifi_msg *ncm)
{
	struct nss_wifi_igmp_mld_override_tos_msg *nwpm __maybe_unused = &ncm->msg.wigmpmldtm_msg;
	nss_trace("%px: NSS WIFI Override TOS Message:\n"
		"WIFI enable TID override Flag: %d\n"
		"WIFI Value of TID to be overriden: %d\n",
		nwpm, nwpm->igmp_mld_ovride_tid_en,
		nwpm->igmp_mld_ovride_tid_val);
}

/*
 * nss_wifi_log_ol_stats_cfg_msg()
 *	Log NSS WIFI Offload Stats Config Message.
 */
static void nss_wifi_log_ol_stats_cfg_msg(struct nss_wifi_msg *ncm)
{
	struct nss_wifi_ol_stats_cfg_msg *nwpm __maybe_unused = &ncm->msg.scm_msg;
	nss_trace("%px: NSS WIFI Enable/Disable Offload Stats Message:\n"
		"WIFI enable/disable offload stats config: %d\n",
		nwpm, nwpm->stats_cfg);
}

/*
 * nss_wifi_log_tx_queue_cfg_msg()
 *	Log NSS WIFI TX Queue Configuration message.
 */
static void nss_wifi_log_tx_queue_cfg_msg(struct nss_wifi_msg *ncm)
{
	struct nss_wifi_tx_queue_cfg_msg *nwpm __maybe_unused = &ncm->msg.wtxqcm;
	nss_trace("%px: NSS WIFI TX Queue Config Message:\n"
		"WIFI TX Queue Size: %d\n"
		"WIFI TX Queue Range: %d\n",
		nwpm, nwpm->size, nwpm->range);
}

/*
 * nss_wifi_log_tx_min_threshold_cfg()
 *	Log NSS WIFI TX Queue Min Threshold Configuration message.
 */
static void nss_wifi_log_tx_min_threshold_cfg(struct nss_wifi_msg *ncm)
{
	struct nss_wifi_tx_min_threshold_cfg_msg *nwpm __maybe_unused = &ncm->msg.wtx_min_threshold_cm;
	nss_trace("%px: NSS WIFI TX Queue Min Threshold Config Message:\n"
		"WIFI TX Queue Min Threshold Value: %d\n",
		nwpm, nwpm->min_threshold);
}

/*
 * nss_wifi_log_dbdc_process_enable_msg()
 *	Log NSS WIFI DBDC repeater process configuration.
 */
static void nss_wifi_log_dbdc_process_enable_msg(struct nss_wifi_msg *ncm)
{
	struct nss_wifi_dbdc_process_enable_msg *nwpm __maybe_unused = &ncm->msg.dbdcpe_msg;
	nss_trace("%px: NSS WIFI DBDC repeater process configuration:\n"
		"WIFI DBDC Process Enable Flag: %d\n",
		nwpm, nwpm->dbdc_process_enable);
}

/*
 * nss_wifi_log_primary_radio_set_msg()
 *	Log NSS WIFI Primary Radio Set message.
 */
static void nss_wifi_log_primary_radio_set_msg(struct nss_wifi_msg *ncm)
{
	struct nss_wifi_primary_radio_set_msg *nwpm __maybe_unused = &ncm->msg.wprs_msg;
	nss_trace("%px: NSS WIFI Primary Radio Set Message:\n"
		"WIFI Current Radio as Primary Radio Enable/Disable Flag: %d\n",
		nwpm, nwpm->flag);
}

/*
 * nss_wifi_log_force_client_mcast_traffic_set_msg()
 *	Log NSS WIFI Force Multicat Traffic for Radio
 */
static void nss_wifi_log_force_client_mcast_traffic_set_msg(struct nss_wifi_msg *ncm)
{
	struct nss_wifi_force_client_mcast_traffic_set_msg *nwpm __maybe_unused = &ncm->msg.wfcmts_msg;
	nss_trace("%px: NSS WIFI Force Multicat Traffic for Radio Message:\n"
		"WIFI Radio Multicast Traffic Flag: %d\n",
		nwpm, nwpm->flag);
}

/*
 * nss_wifi_log_store_other_pdev_stavap_msg()
 *	Log NSS WIFI Store Other Radio Station VAP Message.
 */
static void nss_wifi_log_store_other_pdev_stavap_msg(struct nss_wifi_msg *ncm)
{
	struct nss_wifi_store_other_pdev_stavap_msg *nwpm __maybe_unused = &ncm->msg.wsops_msg;
	nss_trace("%px: NSS WIFI Store Other Radio Station VAP Message:\n"
		"WIFI Station VAP Interface Number: %d\n",
		nwpm, nwpm->stavap_ifnum);
}

/*
 * nss_wifi_log_sta_kickout_msg()
 *	Log NSS WIFI Station Kickout Message.
 */
static void nss_wifi_log_sta_kickout_msg(struct nss_wifi_msg *ncm)
{
	struct nss_wifi_sta_kickout_msg *nwpm __maybe_unused = &ncm->msg.sta_kickout_msg;
	nss_trace("%px: NSS WIFI Station Kickout Message:\n"
		"WIFI PEER ID: %d\n",
		nwpm, nwpm->peer_id);
}

/*
 * nss_wifi_log_wnm_peer_rx_activity()
 *	Log NSS WIFI RX Active State Information of Peer.
 */
static void nss_wifi_log_wnm_peer_rx_activity(struct nss_wifi_msg *ncm)
{
	struct nss_wifi_wnm_peer_rx_activity_msg *nwpm __maybe_unused = &ncm->msg.wprm;
	nss_trace("%px: NSS WIFI RX Active State Information of Peer:\n"
		"WIFI Peer ID: %px\n"
		"WIFI Number of Entries: %d\n",
		nwpm, nwpm->peer_id, nwpm->nentries);
}

/*
 * nss_wifi_log_wds_extn_peer_cfg_msg()
 *	Log NSS WIFI WDS Extension Enabled Configuraion Message.
 */
static void nss_wifi_log_wds_extn_peer_cfg_msg(struct nss_wifi_msg *ncm)
{
	struct nss_wifi_wds_extn_peer_cfg_msg *nwpm __maybe_unused = &ncm->msg.wpeercfg;
	nss_trace("%px: NSS WIFI Extension Enabled Configuraion Message:\n"
		"WIFI Peer MAC Address: %pM\n"
		"WIFI WDS Flags: %d\n"
		"WIFI Peer ID: %d\n",
		nwpm, nwpm->mac_addr, nwpm->wds_flags,
		nwpm->peer_id);
}

/*
 * nss_wifi_log_tx_capture_msg()
 *	Log NSS WIFI Enable TX Capture Message.
 */
static void nss_wifi_log_tx_capture_msg(struct nss_wifi_msg *ncm)
{
	struct nss_wifi_tx_capture_msg *nwpm __maybe_unused = &ncm->msg.tx_capture_msg;
	nss_trace("%px: NSS WIFI Enable TX Capture Message:\n"
		"WIFI TX Capture Enable Flag: %d\n",
		nwpm, nwpm->tx_capture_enable);
}

/*
 * nss_wifi_log_always_primary_set_msg()
 *	Log NSS WIFI Always Set Current Radio Primary Message.
 */
static void nss_wifi_log_always_primary_set_msg(struct nss_wifi_msg *ncm)
{
	struct nss_wifi_always_primary_set_msg *nwpm __maybe_unused = &ncm->msg.waps_msg;
	nss_trace("%px: NSS WIFI Always Set Current Radio Primary Message:\n"
		"WIFI Always Set Flag: %d\n",
		nwpm, nwpm->flag);
}

/*
 * nss_wifi_log_cmd_msg()
 *	Log NSS WIFI PDEV Command Message.
 */
static void nss_wifi_log_cmd_msg(struct nss_wifi_msg *ncm)
{
	struct nss_wifi_cmd_msg *nwpm __maybe_unused = &ncm->msg.wcmdm;
	nss_trace("%px: NSS WIFI PDEV Command Message:\n"
		"WIFI Type of Command: %d\n"
		"WIFI Value of Command: %d\n",
		nwpm, nwpm->cmd, nwpm->value);
}

/*
 * nss_wifi_log_enable_ol_statsv2_msg()
 *	Log NSS WIFI Enable Version 2 of TX/RX Stats
 */
static void nss_wifi_log_enable_ol_statsv2_msg(struct nss_wifi_msg *ncm)
{
	struct nss_wifi_enable_ol_statsv2 *nwpm __maybe_unused = &ncm->msg.wesh_msg;
	nss_trace("%px: NSS WIFI Enable Version 2 of TX/RX Stats:\n"
		"WIFI Enable Version 2 Stats: %d\n",
		nwpm, nwpm->enable_ol_statsv2);
}

/*
 * nss_wifi_log_enable_ol_peer_time_msg()
 *	Log NSS WIFI Enable Per Peer Stats to Host
 */
static void nss_wifi_log_enable_ol_peer_time_msg(struct nss_wifi_msg *ncm)
{
	struct nss_wifi_ol_peer_time_msg *nwpm __maybe_unused = &ncm->msg.wopt_msg;
	int32_t i;

	nss_trace("%px: NSS WIFI Enable Per PEer Stats to Host:\n"
		"WIFI Number of Peers: %d\n"
		"WIFI Peed ID: %d\n",
		nwpm, nwpm->npeers,
		nwpm->tstats[0].peer_id);
	/*
	 * Continuation of the log.
	 */
	nss_trace("WIFI TX Timestamp:\n");
	nss_trace("\tSum of sojourn for each packet:");
	for (i = 0; i < NSS_WIFI_TX_NUM_TOS_TIDS; i++) {
		nss_trace("\t\t%d = %x", i, nwpm->tstats[0].sum[i].sum_tx);
	}
	nss_trace("\tNumber of MSDU per peer per TID:");
	for (i = 0; i < NSS_WIFI_TX_NUM_TOS_TIDS; i++) {
		nss_trace("\t\t%d = %x", i, nwpm->tstats[0].sum[i].sum_msdus);
	}
	nss_trace("WIFI Exponential Weighted Average:");
	for (i = 0; i < NSS_WIFI_TX_NUM_TOS_TIDS; i++) {
		nss_trace("\t%d = %d", i, nwpm->tstats[0].avg[i]);
	}
}

/*
 * nss_wifi_log_verbose()
 *	Log message contents.
 */
static void nss_wifi_log_verbose(struct nss_wifi_msg *ncm)
{
	switch (ncm->cm.type) {
	case NSS_WIFI_INIT_MSG:
		nss_wifi_log_init_msg(ncm);
		break;

	case NSS_WIFI_HTT_INIT_MSG:
		nss_wifi_log_htt_init_msg(ncm);
		break;

	case NSS_WIFI_TX_INIT_MSG:
		nss_wifi_log_tx_init_msg(ncm);
		break;

	case NSS_WIFI_RAW_SEND_MSG:
		nss_wifi_log_rawsend_msg(ncm);
		break;

	case NSS_WIFI_MGMT_SEND_MSG:
		nss_wifi_log_mgmtsend_msg(ncm);
		break;

	case NSS_WIFI_WDS_PEER_ADD_MSG:
		nss_wifi_log_wds_peer_msg(ncm);
		break;

	case NSS_WIFI_WDS_PEER_DEL_MSG:
		nss_wifi_log_wds_peer_msg(ncm);
		break;

	case NSS_WIFI_STOP_MSG:
		nss_wifi_log_stop_msg(ncm);
		break;

	case NSS_WIFI_RESET_MSG:
		nss_wifi_log_reset_msg(ncm);
		break;

	case NSS_WIFI_PEER_FREELIST_APPEND_MSG:
		nss_wifi_log_peer_freelist_append_msg(ncm);
		break;

	case NSS_WIFI_RX_REORDER_ARRAY_FREELIST_APPEND_MSG:
		nss_wifi_log_rx_reorder_array_freelist_append_msg(ncm);
		break;

	case NSS_WIFI_MONITOR_FILTER_SET_MSG:
		nss_wifi_log_set_filter_msg(ncm);
		break;

	case NSS_WIFI_PEER_BS_STATE_MSG:
		nss_wifi_log_peer_activity_msg(ncm);
		break;

	case NSS_WIFI_RX_VOW_EXTSTATS_SET_MSG:
		nss_wifi_log_rx_vow_extstats_set_msg(ncm);
		break;

	case NSS_WIFI_PKTLOG_CFG_MSG:
		nss_wifi_log_pktlog_cfg_msg(ncm);
		break;

	case NSS_WIFI_ENABLE_PERPKT_TXSTATS_MSG:
		nss_wifi_log_enable_perpkt_txstats_msg(ncm);
		break;

	case NSS_WIFI_IGMP_MLD_TOS_OVERRIDE_MSG:
		nss_wifi_log_override_tos_msg(ncm);
		break;

	case NSS_WIFI_OL_STATS_CFG_MSG:
		nss_wifi_log_ol_stats_cfg_msg(ncm);
		break;

	case NSS_WIFI_TX_QUEUE_CFG_MSG:
		nss_wifi_log_tx_queue_cfg_msg(ncm);
		break;

	case NSS_WIFI_TX_MIN_THRESHOLD_CFG_MSG:
		nss_wifi_log_tx_min_threshold_cfg(ncm);
		break;

	case NSS_WIFI_DBDC_PROCESS_ENABLE_MSG:
		nss_wifi_log_dbdc_process_enable_msg(ncm);
		break;

	case NSS_WIFI_PRIMARY_RADIO_SET_MSG:
		nss_wifi_log_primary_radio_set_msg(ncm);
		break;

	case NSS_WIFI_FORCE_CLIENT_MCAST_TRAFFIC_SET_MSG:
		nss_wifi_log_force_client_mcast_traffic_set_msg(ncm);
		break;

	case NSS_WIFI_STORE_OTHER_PDEV_STAVAP_MSG:
		nss_wifi_log_store_other_pdev_stavap_msg(ncm);
		break;

	case NSS_WIFI_STA_KICKOUT_MSG:
		nss_wifi_log_sta_kickout_msg(ncm);
		break;

	case NSS_WIFI_WNM_PEER_RX_ACTIVITY_MSG:
		nss_wifi_log_wnm_peer_rx_activity(ncm);
		break;

	case NSS_WIFI_WDS_VENDOR_MSG:
		nss_wifi_log_wds_extn_peer_cfg_msg(ncm);
		break;

	case NSS_WIFI_TX_CAPTURE_SET_MSG:
		nss_wifi_log_tx_capture_msg(ncm);
		break;

	case NSS_WIFI_ALWAYS_PRIMARY_SET_MSG:
		nss_wifi_log_always_primary_set_msg(ncm);
		break;

	case NSS_WIFI_CMD_MSG:
		nss_wifi_log_cmd_msg(ncm);
		break;

	case NSS_WIFI_ENABLE_OL_STATSV2_MSG:
		nss_wifi_log_enable_ol_statsv2_msg(ncm);
		break;

	case NSS_WIFI_OL_PEER_TIME_MSG:
		nss_wifi_log_enable_ol_peer_time_msg(ncm);
		break;

	case NSS_WIFI_FLUSH_HTT_CMD_MSG:
	case NSS_WIFI_OL_STATS_MSG:
	case NSS_WIFI_MSDU_TTL_SET_MSG:
	case NSS_WIFI_PEER_STATS_MSG:
	case NSS_WIFI_FW_STATS_MSG:
	case NSS_WIFI_SEND_RRA_MEMORY_REQUEST_MSG:
	case NSS_WIFI_STATS_MSG:
	case NSS_WIFI_POST_RECV_MSG:
	case NSS_WIFI_SEND_PEER_MEMORY_REQUEST_MSG:
		/*
		 * No log for these valid messages.
		 */
		break;

	default:
		nss_warning("%px: Invalid message type\n", ncm);
		break;
	}
}

/*
 * nss_wifi_log_tx_msg()
 *	Log messages transmitted to FW.
 */
void nss_wifi_log_tx_msg(struct nss_wifi_msg *ncm)
{
	if (ncm->cm.type >= NSS_WIFI_MAX_MSG) {
		nss_warning("%px: Invalid message type\n", ncm);
		return;
	}

	nss_info("%px: type[%d]:%s\n", ncm, ncm->cm.type, nss_wifi_log_message_types_str[ncm->cm.type]);
	nss_wifi_log_verbose(ncm);
}

/*
 * nss_wifi_log_rx_msg()
 *	Log messages received from FW.
 */
void nss_wifi_log_rx_msg(struct nss_wifi_msg *ncm)
{
	if (ncm->cm.response >= NSS_CMN_RESPONSE_LAST) {
		nss_warning("%px: Invalid response\n", ncm);
		return;
	}

	if (ncm->cm.response == NSS_CMN_RESPONSE_NOTIFY || (ncm->cm.response == NSS_CMN_RESPONSE_ACK)) {
		nss_info("%px: type[%d]:%s, response[%d]:%s\n", ncm, ncm->cm.type,
			nss_wifi_log_message_types_str[ncm->cm.type],
			ncm->cm.response, nss_cmn_response_str[ncm->cm.response]);
		goto verbose;
	}

	if (ncm->cm.error >= NSS_WIFI_EMSG_MAX) {
		nss_warning("%px: msg failure - type[%d]:%s, response[%d]:%s, error[%d]:Invalid error\n",
			ncm, ncm->cm.type, nss_wifi_log_message_types_str[ncm->cm.type],
			ncm->cm.response, nss_cmn_response_str[ncm->cm.response],
			ncm->cm.error);
		goto verbose;
	}

	nss_info("%px: msg nack - type[%d]:%s, response[%d]:%s, error[%d]:%s\n",
		ncm, ncm->cm.type, nss_wifi_log_message_types_str[ncm->cm.type],
		ncm->cm.response, nss_cmn_response_str[ncm->cm.response],
		ncm->cm.error, nss_wifi_log_error_response_types_str[ncm->cm.error]);

verbose:
	nss_wifi_log_verbose(ncm);
}

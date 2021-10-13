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
 * nss_wifili_log.c
 *	NSS WIFILI logger file.
 */

#include "nss_core.h"

/*
 * nss_wifili_log_message_types_str
 *	WIFILI message strings
 */
static int8_t *nss_wifili_log_message_types_str[NSS_WIFILI_MAX_MSG] __maybe_unused = {
	"WIFILI INIT MSG",
	"WIFILI SOC RESET MSG",
	"WIFILI PDEV INIT MSG",
	"WIFILI PDEV DEINIT MSG",
	"WIFILI START MSG",
	"WIFILI STOP MSG",
	"WIFILI PEER CREATE MSG",
	"WIFILI PEER DELETE MSG",
	"WIFILI SEND PEER MEMORY REQUEST MSG",
	"WIFILI PEER FREELIST APPEND MSG",
	"WIFILI STATS MSG",
	"WIFILI WDS VENDOR MSG",
	"WIFILI PEER STATS MSG",
	"WIFILI WDS PEER ADD MSG",
	"WIFILI WDS PEER DEL MSG",
	"WIFILI WDS PEER MAP MSG",
	"WIFILI WDS ACTIVE INFO MSG",
	"WIFILI STATS CFG MSG",
	"WIFILI TID REOQ SETUP MSG",
	"WIFILI RADIO CMD MSG",
	"WIFILI LINK DESC INFO MSG",
	"WIFILI PEER SECURITY TYPE MSG",
	"WIFILI PEER NAWDS ENABLE MSG",
	"WIFILI RADIO BUF CFG",
	"WIFILI DBDC REPEATER SET MSG",
	"WIFILI DBDC REPEATER AST FLUSH MSG"
};

/*
 * nss_wifili_log_error_response_types_str
 *	Strings for error types for WIFILI messages
 */
static int8_t *nss_wifili_log_error_response_types_str[NSS_WIFILI_EMSG_UNKNOWN] __maybe_unused = {
	"WIFILI NO ERROR",
	"WIFILI INIT FAIL IMPROPER STATE",
	"WIFILI RINGS INIT FAIL",
	"WIFILI PDEV INIT IMPROPER STATE FAIL",
	"WIFILI PDEV INIT INVALID RADIOID FAIL",
	"WIFILI PDEV TX IRQ ALLOC FAIL",
	"WIFILI PDEV RESET INVALID RADIOID FAIL",
	"WIFILI PDEV RESET PDEV NULL FAIL",
	"WIFILI PDEV RESET IMPROPER STATE FAIL",
	"WIFILI START IMPROPER STATE FAIL",
	"WIFILI PEER CREATE FAIL",
	"WIFILI PEER DELETE FAIL",
	"WIFILI HASHMEM INIT FAIL",
	"WIFILI PEER FREELIST APPEND FAIL",
	"WIFILI PEER CREATE INVALID VDEVID FAIL",
	"WIFILI PEER CREATE INVALID PEER ID FAIL",
	"WIFILI PEER CREATE VDEV NULL FAIL",
	"WIFILI PEER CREATE PDEV NULL FAIL",
	"WIFILI PEER CREATE ALLOC FAIL",
	"WIFILI PEER DELETE VAPID INVALID FAIL",
	"WIFILI PEER DELETE INVALID PEERID FAIL",
	"WIFILI PEER DELETE VDEV NULL FAIL",
	"WIFILI PEER DELETE PDEV NULL FAIL",
	"WIFILI PEER DELETE PEER NULL FAIL",
	"WIFILI PEER DELETE PEER CORRUPTED FAIL",
	"WIFILI PEER DUPLICATE AST INDEX PEER ID FAIL",
	"WIFILI GROUP0 TIMER ALLOC FAIL",
	"WIFILI INSUFFICIENT WT FAIL",
	"WIFILI INVALID NUM TCL RING FAIL",
	"WIFILI INVALID NUM REO DST RING FAIL",
	"WIFILI HAL SRNG SOC ALLOC FAIL",
	"WIFILI HAL SRNG INVALID RING INFO FAIL",
	"WIFILI HAL SRNG TCL ALLOC FAIL",
	"WIFILI HAL SRNG TXCOMP ALLOC FAIL",
	"WIFILI HAL SRNG REODST ALLOC FAIL",
	"WIFILI HAL SRNG REOREINJECT ALLOC FAIL",
	"WIFILI HAL SRNG RXRELEASE ALLOC FAIL",
	"WIFILI HAL SRNG RXEXCP ALLOC FAIL",
	"WIFILI HAL TX MEMALLOC FAIL",
	"WIFILI HAL TX INVLID POOL NUM FAIL",
	"WIFILI HAL TX INVALID PAGE NUM FAIL",
	"WIFILI HAL TX DESC MEM ALLOC FAIL",
	"WIFILI HAL RX MEMALLOC FAIL",
	"WIFILI PDEV RXDMA RING ALLOC FAIL",
	"WIFILI NAWDSEN PEERID INVALID",
	"WIFILI NAWDSEN PEER NULL",
	"WIFILI NAWDSEN PEER CORRUPTED",
	"WIFILI WDS PEER CFG FAIL",
	"WIFILI RESET NO STOP",
	"WIFILI HAL SRNG INVALID RING BASE FAIL",
	"WIFILI PDEV RX INIT FAIL",
	"WIFILI EMESG AST ADD FAIL",
	"WIFILI EMESG AST REMOVE FAIL",
	"WIFILI EMESG WDS ADD FAIL",
	"WIFILI EMESG WDS REMOVE FAIL",
	"WIFILI EMESG WDS MAP FAIL",
	"WIFILI WDS INVALID PEERID FAIL",
	"WIFILI WDS DUPLICATE AST INDEX PEER ID FAIL",
	"WIFILI INVALID RADIO CMD",
	"WIFILI INVALID RADIO IFNUM",
	"WIFILI PEER SECURITY PEER NULL FAIL",
	"WIFILI PEER SECURITY PEER CORRUPTED FAIL",
	"WIFILI RADIO INVALID BUF CFG",
};

/*
 * nss_wifili_log_wifili_hal_srng()
 *	Log NSS WIFILI HAL SRNG Information
 */
static void nss_wifili_log_wifili_hal_srng(struct nss_wifili_hal_srng_info *ring)
{
	int32_t i;
	nss_trace("\tRing ID: %d\n"
		"\tMAC ID: %d\n"
		"\tRing base physical address: %x\n"
		"\tNumber of entries: %d\n"
		"\tFlags: %x\n"
		"\tDirection: %d\n"
		"\tEntry size: %d\n"
		"\tLow Threshold: %d\n",
		ring->ring_id, ring->mac_id,
		ring->ring_base_paddr, ring->num_entries,
		ring->flags, ring->ring_dir,
		ring->entry_size, ring->low_threshold);
	nss_trace("Ring Base Addresses:");
	for (i = 0; i < NSS_WIFILI_MAX_SRNG_REG_GROUPS_MSG; i++) {
		nss_trace("\t%x", ring->hwreg_base[i]);
	}
}

/*
 * nss_wifili_log_init_msg()
 *	Log NSS WIFILI Init message.
 */
static void nss_wifili_log_init_msg(struct nss_wifili_msg *nwm)
{
	struct nss_wifili_init_msg *nwim __maybe_unused = &nwm->msg.init;
	int32_t i;
	nss_trace("%px: NSS WIFILI Init Message:\n"
		"WIFILI HAL Source Ring Base Address: %x\n"
		"WIFILI HAL Source Ring Shadow Read Pointer Address: %x\n"
		"WIFILI HAL Source Ring Shadow Write Pointer Address: %x\n"
		"WIFILI Number of Transmit Classifier data rings: %d\n"
		"WIFILI Number of reorder rings: %d\n"
		"WIFILI Flags for SoC initialization: %d\n"
		"WIFILI Tx descriptor initialization number of software descriptors: %d"
		"WIFILI Tx descriptor initialization number of software extended descriptors: %d"
		"WIFILI Tx descriptor initialization number of descriptor pools: %d"
		"WIFILI Tx descriptor initialization number of memory addresses: %d"
		"WIFILI Tx descriptor initialization extended descriptor page number: %d"
		"WIFILI Tx descriptor initialization number of software secriptors for second radio: %d"
		"WIFILI Tx descriptor initialization number of software extended descriptors for second radio: %d",
		nwim, nwim->hssm.dev_base_addr,
		nwim->hssm.shadow_rdptr_mem_addr, nwim->hssm.shadow_wrptr_mem_addr,
		nwim->num_tcl_data_rings, nwim->num_reo_dest_rings,
		nwim->flags, nwim->wtdim.num_tx_desc,
		nwim->wtdim.num_tx_desc_ext, nwim->wtdim.num_pool,
		nwim->wtdim.num_memaddr, nwim->wtdim.ext_desc_page_num,
		nwim->wtdim.num_tx_desc_2, nwim->wtdim.num_tx_desc_ext_2);
	/*
	 * Continuation of the log.
	 */
	nss_trace("WIFILI Tx descriptor initialization memory start address and size:");
	for (i = 0; i < NSS_WIFILI_MAX_NUMBER_OF_PAGE_MSG; i++) {
		nss_trace("\tPage[%d]: Addr: %x Size: %d", i, nwim->wtdim.memory_addr[i], nwim->wtdim.memory_size[i]);
	}
	nss_trace("WIFILI Transmit Classifier data ring Information:");
	for (i = 0; i < NSS_WIFILI_MAX_TCL_DATA_RINGS_MSG; i++) {
		nss_wifili_log_wifili_hal_srng(&nwim->tcl_ring_info[i]);
	}
	nss_trace("WIFILI TX Completion Ring configuration information:");
	for (i = 0; i < NSS_WIFILI_MAX_TCL_DATA_RINGS_MSG; i++) {
		nss_wifili_log_wifili_hal_srng(&nwim->tx_comp_ring[i]);
	}
	nss_trace("WIFILI Reorder destination ring configuration information:");
	for (i = 0; i < NSS_WIFILI_MAX_REO_DATA_RINGS_MSG; i++) {
		nss_wifili_log_wifili_hal_srng(&nwim->reo_dest_ring[i]);
	}
	nss_trace("WIFILI Reorder exception ring configuration information:");
	nss_wifili_log_wifili_hal_srng(&nwim->reo_exception_ring);
	nss_trace("WIFILI Reinject ring configuration information:");
	nss_wifili_log_wifili_hal_srng(&nwim->reo_reinject_ring);
}

/*
 * nss_wifili_log_pdev_init_msg()
 *	Log NSS WIFILI PDEV Init message.
 */
static void nss_wifili_log_pdev_init_msg(struct nss_wifili_msg *nwm)
{
	struct nss_wifili_pdev_init_msg *nwim __maybe_unused = &nwm->msg.pdevmsg;
	nss_trace("%px: NSS WIFILI PDEV Init Message:\n"
		"WIFILI Radio ID: %x\n"
		"WIFILI MAC Hardware Mode: %d\n"
		"WIFILI Lower MAC ID: %x\n",
		nwim, nwim->radio_id,
		nwim->hwmode, nwim->lmac_id);
	/*
	 * Continuation of the log.
	 */
	nss_trace("WIFILI Media Access Point ring information:");
	nss_wifili_log_wifili_hal_srng(&nwim->rxdma_ring);
}

/*
 * nss_wifili_log_pdev_init_msg()
 *	Log NSS WIFILI PDEV Deinit message.
 */
static void nss_wifili_log_pdev_deinit_msg(struct nss_wifili_msg *nwm)
{
	struct nss_wifili_pdev_deinit_msg *nwim __maybe_unused = &nwm->msg.pdevdeinit;
	nss_trace("%px: NSS WIFILI PDEV Deinit Message:\n"
		"WIFILI Interface Number: %d\n",
		nwim, nwim->ifnum);
}

/*
 * nss_wifili_log_peer_msg()
 *	Log NSS WIFILI Peer message.
 */
static void nss_wifili_log_peer_msg(struct nss_wifili_msg *nwm)
{
	struct nss_wifili_peer_msg *nwim __maybe_unused = &nwm->msg.peermsg;
	nss_trace("%px: NSS WIFILI Peer Message:\n"
		"WIFILI Peer MAC Address: %pM\n"
		"WIFILI VAP ID: %d\n"
		"WIFILI Peed ID: %d\n"
		"WIFILI Hardware address search table index: %d\n"
		"WIFILI NAWDS enabled for peer: %d\n"
		"WIFILI peer memory adderss for NSS: %x\n",
		nwim, nwim->peer_mac_addr,
		nwim->vdev_id, nwim->peer_id,
		nwim->hw_ast_idx, nwim->is_nawds,
		nwim->nss_peer_mem);
}

/*
 * nss_wifili_log_peer_freelist_append_msg()
 *	Log NSS WIFILI Peer memory request message.
 */
static void nss_wifili_log_peer_freelist_append_msg(struct nss_wifili_msg *nwm)
{
	struct nss_wifili_peer_freelist_append_msg *nwim __maybe_unused = &nwm->msg.peer_freelist_append;
	nss_trace("%px: NSS WIFILI Peer Memory Request Message:\n"
		"WIFILI Starting Address of Freelist: %x\n"
		"WIFILI Length: %d\n"
		"WIFILI Maximum number of peer entries supported in pool: %d\n",
		nwim, nwim->addr,
		nwim->length, nwim->num_peers);
}

/*
 * nss_wifili_log_wds_peer_msg()
 *	Log NSS WIFILI WDS Peer message.
 */
static void nss_wifili_log_wds_peer_msg(struct nss_wifili_msg *nwm)
{
	struct nss_wifili_wds_peer_msg *nwim __maybe_unused = &nwm->msg.wdspeermsg;
	nss_trace("%px: NSS WIFILI WDS Peer Message:\n"
		"WIFILI Destination MAC: %pM\n"
		"WIFILI Peer MAC: %pM\n",
		nwim, nwim->dest_mac, nwim->peer_mac);
}

/*
 * nss_wifili_log_wds_active_info_msg()
 *	Log NSS WIFILI WDS Active Info message.
 */
static void nss_wifili_log_wds_active_info_msg(struct nss_wifili_msg *nwm)
{
	struct nss_wifili_wds_active_info_msg *nwim __maybe_unused = &nwm->msg.wdsinfomsg;
	nss_trace("%px: NSS WIFILI WDS Active Info Message:\n"
		"WIFILI Number OF Entries: %d\n"
		"WIFILI Hardware AST Index: %d\n",
		nwim, nwim->nentries, nwim->info[0].ast_idx);
}

/*
 * nss_wifili_log_stats_cfg_msg()
 *	Log NSS WIFILI Stats Configuration Message.
 */
static void nss_wifili_log_stats_cfg_msg(struct nss_wifili_msg *nwm)
{
	struct nss_wifili_stats_cfg_msg *nwim __maybe_unused = &nwm->msg.scm;
	nss_trace("%px: NSS WIFILI Stats Config Message:\n"
		"WIFILI Enable/Disable Config: %d\n",
		nwim, nwim->cfg);
}

/*
 * nss_wifili_log_reo_tidq_msg()
 *	Log NSS WIFILI REO TIDQ Setup Message.
 */
static void nss_wifili_log_reo_tidq_msg(struct nss_wifili_msg *nwm)
{
	struct nss_wifili_reo_tidq_msg *nwim __maybe_unused = &nwm->msg.reotidqmsg;
	nss_trace("%px: NSS WIFILI reo tidq setup Message:\n"
		"WIFILI Traffic Identification Value: %d\n"
		"WIFILI Peer ID: %d\n",
		nwim, nwim->tid, nwim->peer_id);
}

/*
 * nss_wifili_log_radio_cfg_msg()
 *	Log NSS WIFILI Radio Command Message.
 */
static void nss_wifili_log_radio_cfg_msg(struct nss_wifili_msg *nwm)
{
	struct nss_wifili_radio_cfg_msg *nwim __maybe_unused = &nwm->msg.radiocfgmsg;
	nss_trace("%px: NSS WIFILI Radio Command Message:\n"
		"WIFILI Radio Interface Number %d\n",
		nwim, nwim->radio_if_num);
}

/*
 * nss_wifili_log_wds_extn_peer_cfg_msg()
 *	Log NSS WIFILI WDS vendor extension configuration message.
 */
static void nss_wifili_log_wds_extn_peer_cfg_msg(struct nss_wifili_msg *nwm)
{
	struct nss_wifili_wds_extn_peer_cfg_msg *nwim __maybe_unused = &nwm->msg.wpeercfg;
	nss_trace("%px: NSS WIFILI WDS vendor extension configuration message:\n"
		"WIFILI Peer MAC Addr: %pM\n"
		"WIFILI WDS Flags: %d\n"
		"WIFILI Peer ID: %d\n",
		nwim, nwim->peer_mac_addr,
		nwim->wds_flags, nwim->peer_id);
}

/*
 * nss_wifili_log_soc_linkdesc_buf_info_msg()
 *	Log NSS WIFILI Link descriptor buffer address information.
 */
static void nss_wifili_log_soc_linkdesc_buf_info_msg(struct nss_wifili_msg *nwm)
{
	struct nss_wifili_soc_linkdesc_buf_info_msg *nwim __maybe_unused = &nwm->msg.linkdescinfomsg;
	nss_trace("%px: NSS WIFILI Link descriptor buffer address information:\n"
		"WIFILI Link Descriptor Low Address: %x\n"
		"WIFILI Link Descriptor High Address: %x\n",
		nwim, nwim->buffer_addr_low,
		nwim->buffer_addr_high);
}

/*
 * nss_wifili_log_peer_security_msg()
 *	Log NSS WIFILI Peer Security Message.
 */
static void nss_wifili_log_peer_security_msg(struct nss_wifili_msg *nwm)
{
	struct nss_wifili_peer_security_type_msg *nwim __maybe_unused = &nwm->msg.securitymsg;
	int32_t i;
	nss_trace("%px: NSS WIFILI Peer Security Message:\n"
		"WIFILI Peer ID: %d\n"
		"WIFILI Packet Type: %d\n"
		"WIFILI Security Type: %d\n",
		nwim, nwim->peer_id,
		nwim->pkt_type, nwim->security_type);
	/*
	 * Continuation of the log.
	 */
	nss_trace("WIFILI MIC KEY:");
	for (i = 0; i < NSS_WIFILI_MIC_KEY_LEN; i++) {
		nss_trace("\t%x", nwim->mic_key[i]);
	}
}

/*
 * nss_wifili_log_peer_nawds_enable_msg()
 *	Log NSS WIFILI NAWDS enable for peer.
 */
static void nss_wifili_log_peer_nawds_enable_msg(struct nss_wifili_msg *nwm)
{
	struct nss_wifili_peer_nawds_enable_msg *nwim __maybe_unused = &nwm->msg.nawdsmsg;
	nss_trace("%px: NSS WIFILI NAWDS enable for peer:\n"
		"WIFILI Peer ID: %d\n"
		"WIFILI Enable NAWDS: %d\n",
		nwim, nwim->peer_id, nwim->is_nawds);
}

/*
 * nss_wifili_log_dbdc_repeater_set_msg()
 *	Log NSS WIFILI DBDC Repeaster Enable Message
 */
static void nss_wifili_log_dbdc_repeater_set_msg(struct nss_wifili_msg *nwm)
{
	struct nss_wifili_dbdc_repeater_set_msg *nwim __maybe_unused = &nwm->msg.dbdcrptrmsg;
	nss_trace("%px: NSS WIFILI DBDC Repeater Enable Message:\n"
		"WIFILI DBDC Enable Flag: %d\n",
		nwim, nwim->is_dbdc_en);
}

/*
 * nss_wifili_log_verbose()
 *	Log message contents.
 */
static void nss_wifili_log_verbose(struct nss_wifili_msg *nwm)
{
	switch (nwm->cm.type) {
	case NSS_WIFILI_INIT_MSG:
		nss_wifili_log_init_msg(nwm);
		break;

	case NSS_WIFILI_SOC_RESET_MSG:
		break;

	case NSS_WIFILI_PDEV_INIT_MSG:
		nss_wifili_log_pdev_init_msg(nwm);
		break;

	case NSS_WIFILI_PDEV_DEINIT_MSG:
		nss_wifili_log_pdev_deinit_msg(nwm);
		break;

	case NSS_WIFILI_PEER_CREATE_MSG:
	case NSS_WIFILI_PEER_DELETE_MSG:
		nss_wifili_log_peer_msg(nwm);
		break;

	case NSS_WIFILI_PEER_FREELIST_APPEND_MSG:
		nss_wifili_log_peer_freelist_append_msg(nwm);
		break;

	case NSS_WIFILI_WDS_VENDOR_MSG:
		nss_wifili_log_wds_extn_peer_cfg_msg(nwm);
		break;

	case NSS_WIFILI_WDS_PEER_ADD_MSG:
	case NSS_WIFILI_WDS_PEER_DEL_MSG:
	case NSS_WIFILI_WDS_PEER_MAP_MSG:
		nss_wifili_log_wds_peer_msg(nwm);
		break;

	case NSS_WIFILI_WDS_ACTIVE_INFO_MSG:
		nss_wifili_log_wds_active_info_msg(nwm);
		break;

	case NSS_WIFILI_STATS_CFG_MSG:
		nss_wifili_log_stats_cfg_msg(nwm);
		break;

	case NSS_WIFILI_TID_REOQ_SETUP_MSG:
		nss_wifili_log_reo_tidq_msg(nwm);
		break;

	case NSS_WIFILI_RADIO_CMD_MSG:
		nss_wifili_log_radio_cfg_msg(nwm);
		break;

	case NSS_WIFILI_LINK_DESC_INFO_MSG:
		nss_wifili_log_soc_linkdesc_buf_info_msg(nwm);
		break;

	case NSS_WIFILI_PEER_SECURITY_TYPE_MSG:
		nss_wifili_log_peer_security_msg(nwm);
		break;

	case NSS_WIFILI_PEER_NAWDS_ENABLE_MSG:
		nss_wifili_log_peer_nawds_enable_msg(nwm);
		break;

	case NSS_WIFILI_DBDC_REPEATER_SET_MSG:
		nss_wifili_log_dbdc_repeater_set_msg(nwm);
		break;

	case NSS_WIFILI_SOJOURN_STATS_MSG:
	case NSS_DBDC_REPEATER_AST_FLUSH_MSG:
	case NSS_WIFILI_SEND_PEER_MEMORY_REQUEST_MSG:
	case NSS_WIFILI_PEER_STATS_MSG:
	case NSS_WIFILI_RADIO_BUF_CFG:
	case NSS_WIFILI_STATS_MSG:
	case NSS_WIFILI_START_MSG:
	case NSS_WIFILI_STOP_MSG:
		/*
		 * No log for these valid messages.
		 */
		break;

	default:
		nss_warning("%px: Invalid message type\n", nwm);
		break;
	}
}

/*
 * nss_wifili_log_tx_msg()
 *	Log messages transmitted to FW.
 */
void nss_wifili_log_tx_msg(struct nss_wifili_msg *nwm)
{
	if (nwm->cm.type >= NSS_WIFILI_MAX_MSG) {
		nss_warning("%px: Invalid message type\n", nwm);
		return;
	}

	nss_info("%px: type[%d]:%s\n", nwm, nwm->cm.type, nss_wifili_log_message_types_str[nwm->cm.type]);
	nss_wifili_log_verbose(nwm);
}

/*
 * nss_wifili_log_rx_msg()
 *	Log messages received from FW.
 */
void nss_wifili_log_rx_msg(struct nss_wifili_msg *nwm)
{
	if (nwm->cm.response >= NSS_CMN_RESPONSE_LAST) {
		nss_warning("%px: Invalid response\n", nwm);
		return;
	}

	if (nwm->cm.response == NSS_CMN_RESPONSE_NOTIFY || (nwm->cm.response == NSS_CMN_RESPONSE_ACK)) {
		nss_info("%px: type[%d]:%s, response[%d]:%s\n", nwm, nwm->cm.type,
			nss_wifili_log_message_types_str[nwm->cm.type],
			nwm->cm.response, nss_cmn_response_str[nwm->cm.response]);
		goto verbose;
	}

	if (nwm->cm.error >= NSS_WIFILI_EMSG_UNKNOWN) {
		nss_warning("%px: msg failure - type[%d]:%s, response[%d]:%s, error[%d]:Invalid error\n",
			nwm, nwm->cm.type, nss_wifili_log_message_types_str[nwm->cm.type],
			nwm->cm.response, nss_cmn_response_str[nwm->cm.response],
			nwm->cm.error);
		goto verbose;
	}

	nss_info("%px: msg nack - type[%d]:%s, response[%d]:%s, error[%d]:%s\n",
		nwm, nwm->cm.type, nss_wifili_log_message_types_str[nwm->cm.type],
		nwm->cm.response, nss_cmn_response_str[nwm->cm.response],
		nwm->cm.error, nss_wifili_log_error_response_types_str[nwm->cm.error]);

verbose:
	nss_wifili_log_verbose(nwm);
}

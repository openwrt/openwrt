/******************************************************************************
 *
 * Copyright(c) 2007 - 2019 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *****************************************************************************/

#include <drv_types.h>

#ifdef CONFIG_RTW_CORE_RXSC
#define is_equal_mac_addr(lhs,rhs) ((((unsigned long)(lhs)|(unsigned long)(rhs)) & 0x01) ? \
				    ((*(unsigned char*)(lhs) ^ (*(unsigned char*)(rhs))) | (*(unsigned char*)(lhs + 1) ^ (*(unsigned char*)(rhs + 1))) | (*(unsigned char*)(lhs + 2) ^ (*(unsigned char*)(rhs + 2)))|\
				    (*(unsigned char*)(lhs + 3) ^ (*(unsigned char*)(rhs + 3))) | (*(unsigned char*)(lhs + 4) ^ (*(unsigned char*)(rhs + 4))) | (*(unsigned char*)(lhs + 5) ^ (*(unsigned char*)(rhs + 5))))==0 :\
				    ((*(unsigned short*)(lhs) ^ (*(unsigned short*)(rhs))) | (*(unsigned short*)(lhs + 2) ^ (*(unsigned short*)(rhs + 2))) | (*(unsigned short*)(lhs + 4) ^ (*(unsigned short*)(rhs + 4))))==0  \
				   )

u8 core_rxsc_check_alloc(_adapter *adapter, union recv_frame *prframe)
{
	struct rx_pkt_attrib *pattrib = &prframe->u.hdr.attrib;

	if (!adapter->enable_rxsc)
		return _FAIL;

	if (pattrib->amsdu ||
		!pattrib->bdecrypted ||
		pattrib->mfrag ||
		pattrib->to_fr_ds == 3 ||
		!pattrib->bsnaphdr)
		return _FAIL;

	if (pattrib->eth_type == ETH_P_ARP ||
		pattrib->eth_type == 0x888e ||
		pattrib->eth_type == 0x8899)
		return _FAIL;

	if (!prframe->u.hdr.psta)
		return _FAIL;

	if (IS_MCAST(pattrib->ra) || IS_MCAST(pattrib->dst))
		return _FAIL;

#ifdef CONFIG_RTW_MESH
	if (pattrib->mesh_ctrl_present)
		return _FAIL;
#endif

#ifdef CONFIG_WAPI_SUPPORT
	if (prframe->u.hdr.bIsWaiPacket)
		return _FAIL;
#endif

	return _SUCCESS;
}

struct core_rxsc_entry *core_rxsc_alloc_entry(_adapter *adapter, union recv_frame *prframe)
{
	struct rx_pkt_attrib *pattrib = &prframe->u.hdr.attrib;
	struct sta_info *psta = prframe->u.hdr.psta;
	struct core_rxsc_entry *rxsc_entry = NULL;
	u8 rxsc_idx, cnt = 0;

	if (core_rxsc_check_alloc(adapter, prframe)!=_SUCCESS)
		return NULL;

	rxsc_idx = psta->rxsc_idx_new;

	for (cnt=0; cnt<NUM_RXSC_ENTRY; cnt++) {
		rxsc_idx = rxsc_idx%NUM_RXSC_ENTRY;
		if (psta->rxsc_entry[rxsc_idx].status != RXSC_ENTRY_APPLYING)
			break;
		rxsc_idx++;
	}

	rxsc_entry = &psta->rxsc_entry[rxsc_idx];

	if (rxsc_entry->status == RXSC_ENTRY_APPLYING)
		return NULL;

	rxsc_entry->status = RXSC_ENTRY_VALID;
	psta->rxsc_idx_new = (rxsc_idx + 1) % NUM_RXSC_ENTRY;

	return rxsc_entry;
}

sint core_rxsc_get_entry(_adapter *adapter, union recv_frame *prframe)
{
	struct sta_info *psta = NULL;
	u8 *pframe = prframe->u.hdr.rx_data;
	u8 *lhs, *rhs = NULL;
	u8 idx = 0;
	struct core_rxsc_entry *rxsc_entry = NULL;

	if (GetFrameType(pframe)!=WIFI_DATA_TYPE)
		return _FAIL;

	psta = rtw_get_stainfo(&adapter->stapriv, get_addr2_ptr(pframe));

	if (!psta)
		return _FAIL;

	lhs = GetAddr1Ptr(pframe);

	for (idx = 0; idx < NUM_RXSC_ENTRY; idx++) {
		rxsc_entry = &psta->rxsc_entry[(psta->rxsc_idx_cached + idx) % NUM_RXSC_ENTRY];

		if (rxsc_entry->status == RXSC_ENTRY_INVALID)
			continue;

		rhs = (u8*)rxsc_entry->rxsc_wlanhdr.addr1;
#if 0
		DBGP("(A1)lhs=%pM rhs=%pM isEqual=%d %d\n", lhs, rhs, is_equal_mac_addr(lhs,rhs), is_equal_mac_addr(lhs+1,rhs));
		DBGP("(A2)lhs=%pM rhs=%pM\n", lhs+6, rhs+6);
		DBGP("(A3)lhs=%pM rhs=%pM\n", lhs+12, rhs+12);
		DBGP("rxsc_payload_offset=%d \n", rxsc_entry->rxsc_payload_offset);
		DBGP("(LLC)lhs=%pM rhs=%pM\n",
			&pframe[rxsc_entry->rxsc_payload_offset+(sizeof(struct ethhdr))-SNAP_SIZE-2],  &rtw_rfc1042_header);
		DBGP("(eth_type)lhs=0x%x rhs=0x%x\n",
			*(unsigned short *)(&pframe[rxsc_entry->rxsc_payload_offset+(ETH_ALEN*2)]), *(unsigned short *)(&rxsc_entry->rxsc_ethhdr.h_proto));

		DBGP("%d %d %d %d %d \n",
			is_equal_mac_addr(lhs,rhs),
			is_equal_mac_addr(lhs+6,rhs+6),
			is_equal_mac_addr(lhs+12,rhs+12),
			!memcmp(&pframe[rxsc_entry->rxsc_payload_offset+(sizeof(struct ethhdr))-SNAP_SIZE-2], &rtw_rfc1042_header, ETH_ALEN),
			(*(unsigned short *)(&pframe[rxsc_entry->rxsc_payload_offset+(ETH_ALEN*2)]) == *(unsigned short *)(&rxsc_entry->rxsc_ethhdr.h_proto))
			);
#endif
		if (is_equal_mac_addr(lhs,rhs) &&
			is_equal_mac_addr(lhs + 6, rhs + 6) &&
			is_equal_mac_addr(lhs + 12, rhs + 12) &&
			!memcmp(&pframe[rxsc_entry->rxsc_payload_offset+(sizeof(struct ethhdr)) - SNAP_SIZE - 2], &rtw_rfc1042_header, ETH_ALEN) &&
			(*(unsigned short *)(&pframe[rxsc_entry->rxsc_payload_offset+(ETH_ALEN * 2)]) == *(unsigned short *)(&rxsc_entry->rxsc_ethhdr.h_proto))) {
			prframe->u.hdr.psta = psta;
			prframe->u.hdr.rxsc_entry = rxsc_entry;
			psta->rxsc_idx_cached = idx;
			return _SUCCESS;
		}
	}

	return _FAIL;
}

s32 core_rxsc_apply_check(_adapter *adapter, union recv_frame *prframe)
{
	struct rx_pkt_attrib *pattrib = &prframe->u.hdr.attrib;
	struct core_rxsc_entry *rxsc_entry = NULL;

	if (!adapter->enable_rxsc)
		return CORE_RX_CONTINUE;

	if (core_rxsc_get_entry(adapter, prframe) != _SUCCESS)
		return CORE_RX_CONTINUE;

	rxsc_entry = prframe->u.hdr.rxsc_entry;

	rxsc_entry->status = RXSC_ENTRY_APPLYING;
	wmb();
	_rtw_memcpy(pattrib, &rxsc_entry->rxsc_attrib, sizeof(rxsc_entry->rxsc_attrib));
	wmb();
	rxsc_entry->status = RXSC_ENTRY_VALID;

	return CORE_RX_GO_SHORTCUT;
}

s32 core_rxsc_apply_shortcut(_adapter *adapter, union recv_frame *prframe)
{
	struct rx_pkt_attrib *pattrib = &prframe->u.hdr.attrib;
	struct core_rxsc_entry *rxsc_entry = prframe->u.hdr.rxsc_entry;
	struct sta_info *psta = prframe->u.hdr.psta;
	u8 *pframe = prframe->u.hdr.rx_data;
	u8 *ptr = NULL;

	/* Shortcut of validate_recv_frame */
	if (pattrib->encrypt == _AES_) {
		if (pattrib->qos)
			pattrib->priority = GetPriority((pframe + WLAN_HDR_A3_LEN));
		else
			pattrib->priority = 0;

		if (!IS_MCAST(pattrib->ra)) {

#ifdef CONFIG_RECV_REORDERING_CTRL
			if (pattrib->qos)
				prframe->u.hdr.preorder_ctrl = &psta->recvreorder_ctrl[pattrib->priority];
#endif

			if (recv_ucast_pn_decache(prframe) == _FAIL) {
			#ifdef DBG_RX_DROP_FRAME
				RTW_INFO("DBG_RX_DROP_FRAME "FUNC_ADPT_FMT" recv_ucast_pn_decache return _FAIL for sta="MAC_FMT"\n"
					, FUNC_ADPT_ARG(adapter), MAC_ARG(psta->cmn.mac_addr));
			#endif
				return CORE_RX_DROP;
			}
		} else {
			if (recv_bcast_pn_decache(prframe) == _FAIL) {
			#ifdef DBG_RX_DROP_FRAME
				RTW_INFO("DBG_RX_DROP_FRAME "FUNC_ADPT_FMT" recv_bcast_pn_decache return _FAIL for sta="MAC_FMT"\n"
					, FUNC_ADPT_ARG(adapter), MAC_ARG(psta->cmn.mac_addr));
			#endif
				return CORE_RX_DROP;
			}
		}
	}

	process_pwrbit_data(adapter, prframe, psta);

	if ((get_frame_sub_type(pframe) & WIFI_QOS_DATA_TYPE) == WIFI_QOS_DATA_TYPE)
		process_wmmps_data(adapter, prframe, psta);

	/* Shortcut of rtw_core_rx_data_pre_process */
	count_rx_stats(adapter, prframe, NULL);

	/* Shortcut of rtw_core_rx_data_post_process */
	/* 802.11 -> 802.3 */
	if (rxsc_entry->rxsc_trim_pad)
		recvframe_pull_tail(prframe, rxsc_entry->rxsc_trim_pad);

	ptr = recvframe_pull(prframe, rxsc_entry->rxsc_payload_offset);
	_rtw_memcpy(ptr, (u8 *)(&rxsc_entry->rxsc_ethhdr), sizeof(rxsc_entry->rxsc_ethhdr));
	rtw_rframe_set_os_pkt(prframe);

	/* Packet forward to OS */
	rtw_os_recv_indicate_pkt(adapter, prframe->u.hdr.pkt, prframe);
	prframe->u.hdr.pkt = NULL;
	rtw_free_recvframe(prframe);
#ifdef RTW_PHL_DBG_CMD
	adapter->core_logs.rxCnt_data_shortcut++;
#endif
	return CORE_RX_DONE;
}

void core_rxsc_clear_entry(_adapter *adapter, struct sta_info *psta)
{
	u32 idx = 0;
	for (idx=0; idx<NUM_RXSC_ENTRY; idx++)
		psta->rxsc_entry[idx].status = RXSC_ENTRY_INVALID;
}
#endif /* CONFIG_RTW_CORE_RXSC */


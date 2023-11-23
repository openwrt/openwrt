/******************************************************************************
 *
 * Copyright(c) 2007 - 2020 Realtek Corporation.
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

#ifndef _RTW_RECV_SHORTCUT_H_
#define _RTW_RECV_SHORTCUT_H_

#ifdef CONFIG_RTW_CORE_RXSC

#define NUM_RXSC_ENTRY	4

enum rxsc_entry_status  {
	RXSC_ENTRY_INVALID = 0,
	RXSC_ENTRY_VALID,
	RXSC_ENTRY_APPLYING,
};

struct rxsc_wlan_hdr {
	unsigned short	fmctrl;
	unsigned short	duration;
	unsigned char	addr1[ETH_ALEN];
	unsigned char	addr2[ETH_ALEN];
	unsigned char	addr3[ETH_ALEN];
	unsigned short	sequence;
	unsigned char	addr4[ETH_ALEN];
	unsigned short	qosctrl;
	unsigned char	iv[8];
};

struct core_rxsc_entry {
	u8 status;
	u32 rxsc_payload_offset;
	u32 rxsc_trim_pad;

	struct rxsc_wlan_hdr rxsc_wlanhdr;
	struct ethhdr rxsc_ethhdr;
	struct rx_pkt_attrib rxsc_attrib;

	u8 rxsc_wd[64];
};

struct core_rxsc_entry *core_rxsc_alloc_entry(_adapter *adapter, union recv_frame *prframe);
s32 core_rxsc_apply_check(_adapter *adapter, union recv_frame *prframe);
s32 core_rxsc_apply_shortcut(_adapter *adapter, union recv_frame *prframe);
void core_rxsc_clear_entry(_adapter *adapter, struct sta_info *psta);

#endif /* CONFIG_RTW_CORE_RXSC */
#endif /* _RTW_RECV_SHORTCUT_H_ */


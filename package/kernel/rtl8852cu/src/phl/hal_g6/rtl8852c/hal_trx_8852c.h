/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation.
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
#ifndef _HAL_TRX_8852C_H_
#define _HAL_TRX_8852C_H_

/* wifi packet(RXD.RPKT_TYPE = 0x0) = 32 bytes, otherwise 16 bytes */
#define RX_DESC_L_SIZE_8852C 32
#define RX_DESC_S_SIZE_8852C 16

#define RX_BD_INFO_SIZE 4

#define RX_PPDU_MAC_INFO_SIZE_8852C 4

#define RX_DESC_DRV_INFO_UNIT_8852C 16 /* unit : byte */

#define ACH0_QUEUE_IDX_8852C 0x0
#define ACH1_QUEUE_IDX_8852C 0x1
#define ACH2_QUEUE_IDX_8852C 0x2
#define ACH3_QUEUE_IDX_8852C 0x3
#define ACH4_QUEUE_IDX_8852C 0x4
#define ACH5_QUEUE_IDX_8852C 0x5
#define ACH6_QUEUE_IDX_8852C 0x6
#define ACH7_QUEUE_IDX_8852C 0x7
#define MGQ_B0_QUEUE_IDX_8852C 0x8
#define HIQ_B0_QUEUE_IDX_8852C 0x9
#define MGQ_B1_QUEUE_IDX_8852C 0xa
#define HIQ_B1_QUEUE_IDX_8852C 0xb
#define FWCMD_QUEUE_IDX_8852C 0xc

/* AX RX DESC */
/* DWORD 0 ; Offset 00h */
#define GET_RX_AX_DESC_PKT_LEN_8852C(__pRxStatusDesc) LE_BITS_TO_4BYTE(__pRxStatusDesc, 0, 14)
#define GET_RX_AX_DESC_SHIFT_8852C(__pRxStatusDesc) LE_BITS_TO_4BYTE(__pRxStatusDesc, 14, 2)
#define GET_RX_AX_DESC_HDR_IV_L_8852C(__pRxStatusDesc) LE_BITS_TO_4BYTE(__pRxStatusDesc, 16, 6)
#define GET_RX_AX_DESC_BB_SEL_8852C(__pRxStatusDesc) LE_BITS_TO_4BYTE(__pRxStatusDesc, 22, 1)
#define GET_RX_AX_DESC_MAC_INFO_VLD_8852C(__pRxStatusDesc) LE_BITS_TO_4BYTE( __pRxStatusDesc, 23, 1)
#define GET_RX_AX_DESC_RPKT_TYPE_8852C(__pRxStatusDesc) LE_BITS_TO_4BYTE(__pRxStatusDesc, 24, 4)
#define GET_RX_AX_DESC_DRV_INFO_SIZE_8852C(__pRxStatusDesc) LE_BITS_TO_4BYTE( __pRxStatusDesc, 28, 3)
#define GET_RX_AX_DESC_LONG_RXD_8852C(__pRxStatusDesc) LE_BITS_TO_4BYTE(__pRxStatusDesc, 31, 1)

/* DWORD 1 ; Offset 04h */
#define GET_RX_AX_DESC_PPDU_TYPE_8852C(__pRxStatusDesc) LE_BITS_TO_4BYTE(__pRxStatusDesc+4, 0, 4)
#define GET_RX_AX_DESC_PPDU_CNT_8852C(__pRxStatusDesc) LE_BITS_TO_4BYTE(__pRxStatusDesc+4, 4, 3)
#define GET_RX_AX_DESC_SR_EN_8852C(__pRxStatusDesc) LE_BITS_TO_4BYTE(__pRxStatusDesc+4, 7, 1)
#define GET_RX_AX_DESC_USER_ID_8852C(__pRxStatusDesc) LE_BITS_TO_4BYTE(__pRxStatusDesc+4, 8, 6)
#define GET_RX_AX_DESC_NON_SRG_PPDU_8852C(__pRxStatusDesc) LE_BITS_TO_4BYTE(__pRxStatusDesc+4, 14, 1)
#define GET_RX_AX_DESC_INTER_PPDU_8852C(__pRxStatusDesc) LE_BITS_TO_4BYTE(__pRxStatusDesc+4, 15, 1)
#define GET_RX_AX_DESC_RX_DATARATE_8852C(__pRxStatusDesc) LE_BITS_TO_4BYTE(__pRxStatusDesc+4, 16, 9)
#define GET_RX_AX_DESC_RX_GI_LTF_8852C(__pRxStatusDesc) LE_BITS_TO_4BYTE(__pRxStatusDesc+4, 25, 3)
#define GET_RX_AX_DESC_PHY_RPT_EN_8852C(__pRxStatusDesc) LE_BITS_TO_4BYTE(__pRxStatusDesc+4, 28, 1)
#define GET_RX_AX_DESC_BW_8852C(__pRxStatusDesc) LE_BITS_TO_4BYTE(__pRxStatusDesc+4, 29, 3)

/* DWORD 2 ; Offset 08h */
#define GET_RX_AX_DESC_FREERUN_CNT_8852C(__pRxStatusDesc) LE_BITS_TO_4BYTE(__pRxStatusDesc+8, 0, 32)

/* DWORD 3 ; Offset 0ch */
#define GET_RX_AX_DESC_A1_MATCH_8852C(__pRxStatusDesc) LE_BITS_TO_4BYTE(__pRxStatusDesc+12, 0, 1)
#define GET_RX_AX_DESC_SW_DEC_8852C(__pRxStatusDesc) LE_BITS_TO_4BYTE(__pRxStatusDesc+12, 1, 1)
#define GET_RX_AX_DESC_HW_DEC_8852C(__pRxStatusDesc) LE_BITS_TO_4BYTE(__pRxStatusDesc+12, 2, 1)
#define GET_RX_AX_DESC_AMPDU_8852C(__pRxStatusDesc) LE_BITS_TO_4BYTE(__pRxStatusDesc+12, 3, 1)
#define GET_RX_AX_DESC_AMPDU_EDN_PKT_8852C(__pRxStatusDesc) LE_BITS_TO_4BYTE(__pRxStatusDesc+12, 4, 1)
#define GET_RX_AX_DESC_AMSDU_8852C(__pRxStatusDesc) LE_BITS_TO_4BYTE(__pRxStatusDesc+12, 5, 1)
#define GET_RX_AX_DESC_AMSDU_CUT_8852C(__pRxStatusDesc) LE_BITS_TO_4BYTE(__pRxStatusDesc+12, 6, 1)
#define GET_RX_AX_DESC_LAST_MSDU_8852C(__pRxStatusDesc) LE_BITS_TO_4BYTE(__pRxStatusDesc+12, 7, 1)
#define GET_RX_AX_DESC_BYPASS_8852C(__pRxStatusDesc) LE_BITS_TO_4BYTE(__pRxStatusDesc+12, 8, 1)
#define GET_RX_AX_DESC_CRC32_8852C(__pRxStatusDesc) LE_BITS_TO_4BYTE(__pRxStatusDesc+12, 9, 1)
#define GET_RX_AX_DESC_ICVERR_8852C(__pRxStatusDesc) LE_BITS_TO_4BYTE(__pRxStatusDesc+12, 10, 1)
#define GET_RX_AX_DESC_MAGIC_WAKE_8852C(__pRxStatusDesc) LE_BITS_TO_4BYTE(__pRxStatusDesc+12, 11, 1)
#define GET_RX_AX_DESC_UNICAST_WAKE_8852C(__pRxStatusDesc) LE_BITS_TO_4BYTE(__pRxStatusDesc+12, 12, 1)
#define GET_RX_AX_DESC_PATTERN_WAKE_8852C(__pRxStatusDesc) LE_BITS_TO_4BYTE(__pRxStatusDesc+12, 13, 1)

#define GET_RX_AX_DESC_CH_INFO_8852C(__pRxStatusDesc) LE_BITS_TO_4BYTE(__pRxStatusDesc+12, 14, 2)
#define GET_RX_AX_DESC_PATTERN_IDX_8852C(__pRxStatusDesc) LE_BITS_TO_4BYTE(__pRxStatusDesc+12, 16, 5)
#define GET_RX_AX_DESC_TARGET_IDC_8852C(__pRxStatusDesc) LE_BITS_TO_4BYTE(__pRxStatusDesc+12, 21, 3)
#define GET_RX_AX_DESC_CHKSUM_OFFLOAD_8852C(__pRxStatusDesc) LE_BITS_TO_4BYTE(__pRxStatusDesc+12, 24, 1)
#define GET_RX_AX_DESC_WITH_LLC_8852C(__pRxStatusDesc) LE_BITS_TO_4BYTE(__pRxStatusDesc+12, 25, 1)
#define GET_RX_AX_DESC_STATISTICS_8852C(__pRxStatusDesc) LE_BITS_TO_4BYTE(__pRxStatusDesc+12, 26, 1)


/* DWORD 4 ; Offset 10h */
#define GET_RX_AX_DESC_TYPE_8852C(__pRxStatusDesc) LE_BITS_TO_4BYTE(__pRxStatusDesc+16, 0, 2)
#define GET_RX_AX_DESC_MC_8852C(__pRxStatusDesc) LE_BITS_TO_4BYTE(__pRxStatusDesc+16, 2, 1)
#define GET_RX_AX_DESC_BC_8852C(__pRxStatusDesc) LE_BITS_TO_4BYTE(__pRxStatusDesc+16, 3, 1)
#define GET_RX_AX_DESC_MD_8852C(__pRxStatusDesc) LE_BITS_TO_4BYTE(__pRxStatusDesc+16, 4, 1)
#define GET_RX_AX_DESC_MF_8852C(__pRxStatusDesc) LE_BITS_TO_4BYTE(__pRxStatusDesc+16, 5, 1)
#define GET_RX_AX_DESC_PWR_8852C(__pRxStatusDesc) LE_BITS_TO_4BYTE(__pRxStatusDesc+16, 6, 1)
#define GET_RX_AX_DESC_QOS_8852C(__pRxStatusDesc) LE_BITS_TO_4BYTE(__pRxStatusDesc+16, 7, 1)
#define GET_RX_AX_DESC_TID_8852C(__pRxStatusDesc) LE_BITS_TO_4BYTE(__pRxStatusDesc+16, 8, 4)
#define GET_RX_AX_DESC_EOSP_8852C(__pRxStatusDesc) LE_BITS_TO_4BYTE(__pRxStatusDesc+16, 12, 1)
#define GET_RX_AX_DESC_HTC_8852C(__pRxStatusDesc) LE_BITS_TO_4BYTE(__pRxStatusDesc+16, 13, 1)
#define GET_RX_AX_DESC_QNULL_8852C(__pRxStatusDesc) LE_BITS_TO_4BYTE(__pRxStatusDesc+16, 14, 1)

#define GET_RX_AX_DESC_SEQ_8852C(__pRxStatusDesc) LE_BITS_TO_4BYTE(__pRxStatusDesc+16, 16, 12)
#define GET_RX_AX_DESC_FRAG_8852C(__pRxStatusDesc) LE_BITS_TO_4BYTE(__pRxStatusDesc+16, 28, 4)

/* DWORD 5 ; Offset 14h */
#define GET_RX_AX_DESC_CAM_IDX_8852C(__pRxStatusDesc) LE_BITS_TO_4BYTE(__pRxStatusDesc+20, 0, 8)
#define GET_RX_AX_DESC_ADDR_CAM_8852C(__pRxStatusDesc) LE_BITS_TO_4BYTE(__pRxStatusDesc+20, 8, 8)
#define GET_RX_AX_DESC_MACID_8852C(__pRxStatusDesc) LE_BITS_TO_4BYTE(__pRxStatusDesc+20, 16, 8)
#define GET_RX_AX_DESC_PL_ID_8852C(__pRxStatusDesc) LE_BITS_TO_4BYTE(__pRxStatusDesc+20, 24, 4)
#define GET_RX_AX_DESC_CAM_VLD_8852C(__pRxStatusDesc) LE_BITS_TO_4BYTE(__pRxStatusDesc+20, 28, 1)
#define GET_RX_AX_DESC_FWD_EN_8852C(__pRxStatusDesc) LE_BITS_TO_4BYTE(__pRxStatusDesc+20, 29, 1)
#define GET_RX_AX_DESC_PL_MATCH_8852C(__pRxStatusDesc) LE_BITS_TO_4BYTE(__pRxStatusDesc+20, 30, 1)

/* DWORD 6 ; Offset 18h */
//#define GET_RX_AX_DESC_MAC_ADDR_8852C(__pRxStatusDesc) LE_BITS_TO_4BYTE(__pRxStatusDesc+24, 0, 32)

/* DWORD 7 ; Offset 1ch */
//#define GET_RX_AX_DESC_MAC_ADDR_H_8852C(__pRxStatusDesc) LE_BITS_TO_4BYTE(__pRxStatusDesc+28, 0, 16)
#define GET_RX_AX_DESC_SEC_TYPE_8852C(__pRxStatusDesc) LE_BITS_TO_4BYTE(__pRxStatusDesc+28, 17, 4)


#define GET_RX_AX_DRV_INFO_RX_PWR_DBM_8852C(_drv_info) LE_BITS_TO_4BYTE(_drv_info, 0, 12)
#define GET_RX_AX_DRV_INFO_RX_SC_8852C(_drv_info) LE_BITS_TO_4BYTE(_drv_info, 12, 4)
#define GET_RX_AX_DRV_INFO_CENTRAL_CH_8852C(_drv_info) LE_BITS_TO_4BYTE(_drv_info, 16, 8)

/*
0000: WIFI packet
0001: PPDU status
0010: channel info
0011: BB scope mode
0100: F2P TX CMD report
0101: SS2FW report
0110: TX report
0111: TX payload release to host
1000: DFS report
1001: TX payload release to WLCPU
1010: C2H packet */
#define RX_8852C_DESC_PKT_T_WIFI 0
#define RX_8852C_DESC_PKT_T_PPDU_STATUS 1
#define RX_8852C_DESC_PKT_T_CHANNEL_INFO 2
#define RX_8852C_DESC_PKT_T_BB_SCOPE 3
#define RX_8852C_DESC_PKT_T_F2P_TX_CMD_RPT 4
#define RX_8852C_DESC_PKT_T_SS2FW_RPT 5
#define RX_8852C_DESC_PKT_T_TX_RPT 6
#define RX_8852C_DESC_PKT_T_TX_PD_RELEASE_HOST 7
#define RX_8852C_DESC_PKT_T_DFS_RPT 8
#define RX_8852C_DESC_PKT_T_TX_PD_RELEASE_WLCPU 9
#define RX_8852C_DESC_PKT_T_C2H 10


#define RX_8852C_DESC_PPDU_T_LCCK 0
#define RX_8852C_DESC_PPDU_T_SCCK 1
#define RX_8852C_DESC_PPDU_T_OFDM 2
#define RX_8852C_DESC_PPDU_T_HT 3
#define RX_8852C_DESC_PPDU_T_HTGF 4
#define RX_8852C_DESC_PPDU_T_VHT_SU 5
#define RX_8852C_DESC_PPDU_T_VHT_MU 6
#define RX_8852C_DESC_PPDU_T_HE_SU 7
#define RX_8852C_DESC_PPDU_T_HE_ERSU 8
#define RX_8852C_DESC_PPDU_T_HE_MU 9
#define RX_8852C_DESC_PPDU_T_HE_TB 10
#define RX_8852C_DESC_PPDU_T_UNKNOWN 15


struct rx_ppdu_status{
	u32 mac_info_length;
	u32 phy_info_length;
	//struct mac_info macinfo;
	//struct phy_info phyinfo;
};

enum rtw_hal_status
hal_handle_rx_buffer_8852c(struct rtw_phl_com_t *phl_com,
				struct hal_info_t *hal,
				u8 *buf, u32 buf_len,
				struct rtw_phl_rx_pkt *phl_rx);

#endif /*_HAL_TRX_8852C_H_*/

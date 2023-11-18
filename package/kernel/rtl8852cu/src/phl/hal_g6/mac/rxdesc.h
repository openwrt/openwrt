/** @file */
/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation. All rights reserved.
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
 ******************************************************************************/

#ifndef _MAC_AX_RXDESC_H_
#define _MAC_AX_RXDESC_H_

/* dword0 */
#define AX_RXD_RPKT_LEN_SH		0
#define AX_RXD_RPKT_LEN_MSK		0x3fff
#define AX_RXD_SHIFT_SH		14
#define AX_RXD_SHIFT_MSK		0x3
#define AX_RXD_WL_HD_IV_LEN_SH		16
#define AX_RXD_WL_HD_IV_LEN_MSK		0x3f
#define AX_RXD_BB_SEL		BIT(22)
#define AX_RXD_MAC_INFO_VLD		BIT(23)
#define AX_RXD_RPKT_TYPE_SH		24
#define AX_RXD_RPKT_TYPE_MSK		0xf
#define AX_RXD_DRV_INFO_SIZE_SH		28
#define AX_RXD_DRV_INFO_SIZE_MSK		0x7
#define AX_RXD_LONG_RXD		BIT(31)

/* dword1 */
#define AX_RXD_PPDU_TYPE_SH		0
#define AX_RXD_PPDU_TYPE_MSK		0xf
#define AX_RXD_PPDU_CNT_SH		4
#define AX_RXD_PPDU_CNT_MSK		0x7
#define AX_RXD_SR_EN		BIT(7)
#define AX_RXD_USER_ID_SH		8
#define AX_RXD_USER_ID_MSK		0xff
#define AX_RXD_USER_ID_v1_SH	8
#define AX_RXD_USER_ID_v1_MSK	0x3f
#define AX_RXD_RX_DATARATE_SH		16
#define AX_RXD_RX_DATARATE_MSK		0x1ff
#define AX_RXD_RX_GI_LTF_SH		25
#define AX_RXD_RX_GI_LTF_MSK		0x7
#define AX_RXD_NON_SRG_PPDU		BIT(28)
#define AX_RXD_INTER_PPDU		BIT(29)
#define AX_RXD_NON_SRG_PPDU_v1	BIT(14)
#define AX_RXD_INTER_PPDU_v1	BIT(15)
#define AX_RXD_BW_SH		30
#define AX_RXD_BW_MSK		0x3
#define AX_RXD_BW_v1_SH		29
#define AX_RXD_BW_v1_MSK	0x7

/* dword2 */
#define AX_RXD_FREERUN_CNT_SH		0
#define AX_RXD_FREERUN_CNT_MSK		0xffffffff

/* dword3 */
#define AX_RXD_A1_MATCH		BIT(0)
#define AX_RXD_SW_DEC		BIT(1)
#define AX_RXD_HW_DEC		BIT(2)
#define AX_RXD_AMPDU		BIT(3)
#define AX_RXD_AMPDU_END_PKT		BIT(4)
#define AX_RXD_AMSDU		BIT(5)
#define AX_RXD_AMSDU_CUT		BIT(6)
#define AX_RXD_LAST_MSDU		BIT(7)
#define AX_RXD_BYPASS		BIT(8)
#define AX_RXD_CRC32_ERR		BIT(9)
#define AX_RXD_ICV_ERR		BIT(10)
#define AX_RXD_MAGIC_WAKE		BIT(11)
#define AX_RXD_UNICAST_WAKE		BIT(12)
#define AX_RXD_PATTERN_WAKE		BIT(13)
#define AX_RXD_GET_CH_INFO_SH		14
#define AX_RXD_GET_CH_INFO_MSK		0x3
#define AX_RXD_PATTERN_IDX_SH		16
#define AX_RXD_PATTERN_IDX_MSK		0x1f
#define AX_RXD_TARGET_IDC_SH		21
#define AX_RXD_TARGET_IDC_MSK		0x7
#define AX_RXD_CHKSUM_OFFLOAD_EN		BIT(24)
#define AX_RXD_WITH_LLC		BIT(25)
#define AX_RXD_RX_STATISTICS		BIT(26)

/* dword4 */
#define AX_RXD_TYPE_SH		0
#define AX_RXD_TYPE_MSK		0x3
#define AX_RXD_MC		BIT(2)
#define AX_RXD_BC		BIT(3)
#define AX_RXD_MD		BIT(4)
#define AX_RXD_MF		BIT(5)
#define AX_RXD_PWR		BIT(6)
#define AX_RXD_QOS		BIT(7)
#define AX_RXD_TID_SH		8
#define AX_RXD_TID_MSK		0xf
#define AX_RXD_EOSP		BIT(12)
#define AX_RXD_HTC		BIT(13)
#define AX_RXD_QNULL		BIT(14)
#define AX_RXD_SEQ_SH		16
#define AX_RXD_SEQ_MSK		0xfff
#define AX_RXD_FRAG_SH		28
#define AX_RXD_FRAG_MSK		0xf

/* dword5 */
#define AX_RXD_SEC_CAM_IDX_SH		0
#define AX_RXD_SEC_CAM_IDX_MSK		0xff
#define AX_RXD_ADDR_CAM_SH		8
#define AX_RXD_ADDR_CAM_MSK		0xff
#define AX_RXD_MAC_ID_SH		16
#define AX_RXD_MAC_ID_MSK		0xff
#define AX_RXD_RX_PL_ID_SH		24
#define AX_RXD_RX_PL_ID_MSK		0xf
#define AX_RXD_ADDR_CAM_VLD		BIT(28)
#define AX_RXD_ADDR_FWD_EN		BIT(29)
#define AX_RXD_RX_PL_MATCH		BIT(30)

/* dword6 */
#define AX_RXD_MAC_ADDR_SH		0
#define AX_RXD_MAC_ADDR_MSK		0xffffffff

/* dword7 */
#define AX_RXD_MAC_ADDR_H_SH		0
#define AX_RXD_MAC_ADDR_H_MSK		0xffff
#define AX_RXD_SMART_ANT			BIT(16)
#define AX_RXD_SEC_TYPE_SH			17
#define AX_RXD_SEC_TYPE_MSK			0xf
#define AX_RXD_HDR_CNV				BIT(21)
#define AX_RXD_HDR_OFFSET_SH		22
#define AX_RXD_HDR_OFFSET_MSK		0x1f
#define AX_RXD_BIP_KEYID			BIT(27)
#define AX_RXD_BIP_ENC				BIT(28)

#define RXD_S_RPKT_TYPE_WIFI		0
#define RXD_S_RPKT_TYPE_PPDU		1
#define RXD_S_RPKT_TYPE_CH_INFO		2
#define RXD_S_RPKT_TYPE_BB_SCORE	3
#define RXD_S_RPKT_TYPE_TXCMD_RPT	4
#define RXD_S_RPKT_TYPE_SS2FW_RPT	5
#define RXD_S_RPKT_TYPE_TXRPT		6
#define RXD_S_RPKT_TYPE_PLDREL_HOST	7
#define RXD_S_RPKT_TYPE_DFS_RPT		8
#define RXD_S_RPKT_TYPE_PLDREL_WLCPU	9
#define RXD_S_RPKT_TYPE_C2H		10
#define RXD_S_RPKT_TYPE_CSI		11
#define RXD_S_RPKT_TYPE_CQI		12
#define RXD_S_RPKT_TYPE_H2C		13
#define RXD_S_RPKT_TYPE_FWDL		14

#endif


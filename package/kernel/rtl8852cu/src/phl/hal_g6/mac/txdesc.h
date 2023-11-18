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

#ifndef _MAC_AX_TXDESC_H_
#define _MAC_AX_TXDESC_H_

#if MAC_AX_8852A_SUPPORT
/* dword0 */
#define AX_TXD_WP_OFFSET_SH		24
#define AX_TXD_WP_OFFSET_MSK		0xff
#define AX_TXD_MOREDATA		BIT(23)
#define AX_TXD_WDINFO_EN		BIT(22)
#define AX_TXD_PKT_OFFSET		BIT(21)
#define AX_TXD_FWDL_EN		BIT(20)
#define AX_TXD_CH_DMA_SH		16
#define AX_TXD_CH_DMA_MSK		0xf
#define AX_TXD_HDR_LLC_LEN_SH		11
#define AX_TXD_HDR_LLC_LEN_MSK		0x1f
#define AX_TXD_STF_MODE		BIT(10)
#define AX_TXD_WP_INT		BIT(9)
#define AX_TXD_CHK_EN		BIT(8)
#define AX_TXD_WD_PAGE		BIT(7)
#define AX_TXD_HW_AES_IV		BIT(6)
#define AX_TXD_HWAMSDU		BIT(5)
#define AX_TXD_SMH_EN		BIT(4)
#define AX_TXD_HW_SSN_SEL_SH		2
#define AX_TXD_HW_SSN_SEL_MSK		0x3
#define AX_TXD_EN_HWSEQ_MODE_SH		0
#define AX_TXD_EN_HWSEQ_MODE_MSK		0x3

/* dword1 */
#define AX_TXD_PLD_SH		16
#define AX_TXD_PLD_MSK		0xffff
#define AX_TXD_DMA_TXAGG_NUM_SH		8
#define AX_TXD_DMA_TXAGG_NUM_MSK		0xff
#define AX_TXD_SHCUT_CAMID_SH		0
#define AX_TXD_SHCUT_CAMID_MSK		0xff

/* dword2 */
#define AX_TXD_MACID_SH		24
#define AX_TXD_MACID_MSK		0x7f
#define AX_TXD_TID_IND		BIT(23)
#define AX_TXD_QSEL_SH		17
#define AX_TXD_QSEL_MSK		0x3f
#define AX_TXD_RU_TC_SH		14
#define AX_TXD_RU_TC_MSK		0x7
#define AX_TXD_TXPKTSIZE_SH		0
#define AX_TXD_TXPKTSIZE_MSK		0x3fff

/* dword3 */
#define AX_TXD_MU_TC_SH		29
#define AX_TXD_MU_TC_MSK		0x7
#define AX_TXD_MU_2ND_TC_SH		26
#define AX_TXD_MU_2ND_TC_MSK		0x7
#define AX_TXD_DATA_TC_SH		20
#define AX_TXD_DATA_TC_MSK		0x3f
#define AX_TXD_RTS_TC_SH		14
#define AX_TXD_RTS_TC_MSK		0x3f
#define AX_TXD_BK		BIT(13)
#define AX_TXD_AGG_EN		BIT(12)
#define AX_TXD_WIFI_SEQ_SH		0
#define AX_TXD_WIFI_SEQ_MSK		0xfff

/* dword4 */
#define AX_TXD_AES_IV_L_SH		16
#define AX_TXD_AES_IV_L_MSK		0xffff
#define AX_TXD_TXDESC_CHECKSUM_SH		0
#define AX_TXD_TXDESC_CHECKSUM_MSK		0xffff

/* dword5 */
#define AX_TXD_AES_IV_H_SH		0
#define AX_TXD_AES_IV_H_MSK		0xffffffff

/* dword6 */
#define AX_TXD_ACK_CH_INFO		BIT(31)
#define AX_TXD_USERATE_SEL		BIT(30)
#define AX_TXD_DATA_BW_SH		28
#define AX_TXD_DATA_BW_MSK		0x3
#define AX_TXD_GI_LTF_SH		25
#define AX_TXD_GI_LTF_MSK		0x7
#define AX_TXD_DATARATE_SH		16
#define AX_TXD_DATARATE_MSK		0x1ff
#define AX_TXD_DATA_ER		BIT(15)
#define AX_TXD_DATA_DCM		BIT(14)
#define AX_TXD_DATA_STBC		BIT(12)
#define AX_TXD_DATA_LDPC		BIT(11)
#define AX_TXD_DISDATAFB		BIT(10)
#define AX_TXD_DISRTSFB		BIT(9)
#define AX_TXD_DATA_BW_ER		BIT(8)
#define AX_TXD_MULTIPORT_ID_SH		4
#define AX_TXD_MULTIPORT_ID_MSK		0x7
#define AX_TXD_MBSSID_SH		0
#define AX_TXD_MBSSID_MSK		0xf

/* dword7 */
#define AX_TXD_DATA_TXCNT_LMT_SEL		BIT(31)
#define AX_TXD_DATA_TXCNT_LMT_SH		25
#define AX_TXD_DATA_TXCNT_LMT_MSK		0x3f
#define AX_TXD_DATA_RTY_LOWEST_RATE_SH		16
#define AX_TXD_DATA_RTY_LOWEST_RATE_MSK		0x1ff
#define AX_TXD_A_CTRL_CAS		BIT(15)
#define AX_TXD_A_CTRL_BSR		BIT(14)
#define AX_TXD_A_CTRL_UPH		BIT(13)
#define AX_TXD_A_CTRL_BQR		BIT(12)
#define AX_TXD_BMC		BIT(11)
#define AX_TXD_NAVUSEHDR		BIT(10)
#define AX_TXD_BCN_SRCH_SEQ_SH		8
#define AX_TXD_BCN_SRCH_SEQ_MSK		0x3
#define AX_TXD_MAX_AGG_NUM_SH		0
#define AX_TXD_MAX_AGG_NUM_MSK		0xff

/* dword8 */
#define AX_TXD_OBW_CTS2SELF_DUP_TYPE_SH		26
#define AX_TXD_OBW_CTS2SELF_DUP_TYPE_MSK		0xf
#define AX_TXD_TXPWR_OFSET_TYPE_SH		22
#define AX_TXD_TXPWR_OFSET_TYPE_MSK		0x7
#define AX_TXD_LSIG_TXOP_EN		BIT(21)
#define AX_TXD_AMPDU_DENSITY_SH		18
#define AX_TXD_AMPDU_DENSITY_MSK		0x7
#define AX_TXD_FORCE_TXOP		BIT(17)
#define AX_TXD_LIFETIME_SEL_SH		13
#define AX_TXD_LIFETIME_SEL_MSK		0x7
#define AX_TXD_SECTYPE_SH		9
#define AX_TXD_SECTYPE_MSK		0xf
#define AX_TXD_SEC_HW_ENC		BIT(8)
#define AX_TXD_SEC_CAM_IDX_SH		0
#define AX_TXD_SEC_CAM_IDX_MSK		0xff

/* dword9 */
#define AX_TXD_FORCE_BSS_CLR		BIT(31)
#define AX_TXD_SIGNALING_TA_PKT_SC_SH		27
#define AX_TXD_SIGNALING_TA_PKT_SC_MSK		0xf
#define AX_TXD_BCNPKT_TSF_CTRL		BIT(26)
#define AX_TXD_GROUP_BIT_IE_OFFSET_SH		16
#define AX_TXD_GROUP_BIT_IE_OFFSET_MSK		0xff
#define AX_TXD_RAW		BIT(15)
#define AX_TXD_NULL_1		BIT(14)
#define AX_TXD_NULL_0		BIT(13)
#define AX_TXD_TRI_FRAME		BIT(12)
#define AX_TXD_BT_NULL		BIT(11)
#define AX_TXD_SPE_RPT		BIT(10)
#define AX_TXD_RTT_EN		BIT(9)
#define AX_TXD_HT_DATA_SND		BIT(7)
#define AX_TXD_SIFS_TX		BIT(6)
#define AX_TXD_SND_PKT_SEL_SH		3
#define AX_TXD_SND_PKT_SEL_MSK		0x7
#define AX_TXD_NDPA_SH		1
#define AX_TXD_NDPA_MSK		0x3
#define AX_TXD_SIGNALING_TA_PKT_EN		BIT(0)

/* dword10 */
#define AX_TXD_HW_RTS_EN		BIT(31)
#define AX_TXD_CCA_RTS_SH		29
#define AX_TXD_CCA_RTS_MSK		0x3
#define AX_TXD_CTS2SELF		BIT(28)
#define AX_TXD_RTS_EN		BIT(27)
#define AX_TXD_SW_DEFINE_SH		0
#define AX_TXD_SW_DEFINE_MSK		0xf

/* dword11 */
#define AX_TXD_NDPA_DURATION_SH		16
#define AX_TXD_NDPA_DURATION_MSK		0xffff

/* dword12 */
#define AX_TXD_VALID_1		BIT(31)
#define AX_TXD_PCIE_SEQ_NUM_1_SH		16
#define AX_TXD_PCIE_SEQ_NUM_1_MSK		0x7fff
#define AX_TXD_VALID_0		BIT(15)
#define AX_TXD_PCIE_SEQ_NUM_0_SH		0
#define AX_TXD_PCIE_SEQ_NUM_0_MSK		0x7fff

/* dword13 */
#define AX_TXD_VALID_3		BIT(31)
#define AX_TXD_PCIE_SEQ_NUM_3_SH		16
#define AX_TXD_PCIE_SEQ_NUM_3_MSK		0x7fff
#define AX_TXD_VALID_2		BIT(15)
#define AX_TXD_PCIE_SEQ_NUM_2_SH		0
#define AX_TXD_PCIE_SEQ_NUM_2_MSK		0x7fff

#endif

#if MAC_AX_8852B_SUPPORT
/* dword0 */
#define AX_TXD_WP_OFFSET_SH		24
#define AX_TXD_WP_OFFSET_MSK		0xff
#define AX_TXD_MOREDATA		BIT(23)
#define AX_TXD_WDINFO_EN		BIT(22)
#define AX_TXD_PKT_OFFSET		BIT(21)
#define AX_TXD_FWDL_EN		BIT(20)
#define AX_TXD_CH_DMA_SH		16
#define AX_TXD_CH_DMA_MSK		0xf
#define AX_TXD_HDR_LLC_LEN_SH		11
#define AX_TXD_HDR_LLC_LEN_MSK		0x1f
#define AX_TXD_STF_MODE		BIT(10)
#define AX_TXD_WP_INT		BIT(9)
#define AX_TXD_CHK_EN		BIT(8)
#define AX_TXD_WD_PAGE		BIT(7)
#define AX_TXD_HW_AES_IV		BIT(6)
#define AX_TXD_HWAMSDU		BIT(5)
#define AX_TXD_SMH_EN		BIT(4)
#define AX_TXD_HW_SSN_SEL_SH		2
#define AX_TXD_HW_SSN_SEL_MSK		0x3
#define AX_TXD_EN_HWSEQ_MODE_SH		0
#define AX_TXD_EN_HWSEQ_MODE_MSK		0x3

/* dword1 */
#define AX_TXD_PLD_SH		16
#define AX_TXD_PLD_MSK		0xffff
#define AX_TXD_DMA_TXAGG_NUM_SH		8
#define AX_TXD_DMA_TXAGG_NUM_MSK		0xff
#define AX_TXD_SHCUT_CAMID_SH		0
#define AX_TXD_SHCUT_CAMID_MSK		0xff

/* dword2 */
#define AX_TXD_MACID_SH		24
#define AX_TXD_MACID_MSK		0x7f
#define AX_TXD_TID_IND		BIT(23)
#define AX_TXD_QSEL_SH		17
#define AX_TXD_QSEL_MSK		0x3f
#define AX_TXD_RU_TC_SH		14
#define AX_TXD_RU_TC_MSK		0x7
#define AX_TXD_TXPKTSIZE_SH		0
#define AX_TXD_TXPKTSIZE_MSK		0x3fff

/* dword3 */
#define AX_TXD_MU_TC_SH		29
#define AX_TXD_MU_TC_MSK		0x7
#define AX_TXD_MU_2ND_TC_SH		26
#define AX_TXD_MU_2ND_TC_MSK		0x7
#define AX_TXD_DATA_TC_SH		20
#define AX_TXD_DATA_TC_MSK		0x3f
#define AX_TXD_RTS_TC_SH		14
#define AX_TXD_RTS_TC_MSK		0x3f
#define AX_TXD_BK		BIT(13)
#define AX_TXD_AGG_EN		BIT(12)
#define AX_TXD_WIFI_SEQ_SH		0
#define AX_TXD_WIFI_SEQ_MSK		0xfff

/* dword4 */
#define AX_TXD_AES_IV_L_SH		16
#define AX_TXD_AES_IV_L_MSK		0xffff
#define AX_TXD_TXDESC_CHECKSUM_SH		0
#define AX_TXD_TXDESC_CHECKSUM_MSK		0xffff

/* dword5 */
#define AX_TXD_AES_IV_H_SH		0
#define AX_TXD_AES_IV_H_MSK		0xffffffff

/* dword6 */
#define AX_TXD_ACK_CH_INFO		BIT(31)
#define AX_TXD_USERATE_SEL		BIT(30)
#define AX_TXD_DATA_BW_SH		28
#define AX_TXD_DATA_BW_MSK		0x3
#define AX_TXD_GI_LTF_SH		25
#define AX_TXD_GI_LTF_MSK		0x7
#define AX_TXD_DATARATE_SH		16
#define AX_TXD_DATARATE_MSK		0x1ff
#define AX_TXD_DATA_ER		BIT(15)
#define AX_TXD_DATA_DCM		BIT(14)
#define AX_TXD_DATA_STBC		BIT(12)
#define AX_TXD_DATA_LDPC		BIT(11)
#define AX_TXD_DISDATAFB		BIT(10)
#define AX_TXD_DISRTSFB		BIT(9)
#define AX_TXD_DATA_BW_ER		BIT(8)
#define AX_TXD_MULTIPORT_ID_SH		4
#define AX_TXD_MULTIPORT_ID_MSK		0x7
#define AX_TXD_MBSSID_SH		0
#define AX_TXD_MBSSID_MSK		0xf

/* dword7 */
#define AX_TXD_DATA_TXCNT_LMT_SEL		BIT(31)
#define AX_TXD_DATA_TXCNT_LMT_SH		25
#define AX_TXD_DATA_TXCNT_LMT_MSK		0x3f
#define AX_TXD_DATA_RTY_LOWEST_RATE_SH		16
#define AX_TXD_DATA_RTY_LOWEST_RATE_MSK		0x1ff
#define AX_TXD_A_CTRL_CAS		BIT(15)
#define AX_TXD_A_CTRL_BSR		BIT(14)
#define AX_TXD_A_CTRL_UPH		BIT(13)
#define AX_TXD_A_CTRL_BQR		BIT(12)
#define AX_TXD_BMC		BIT(11)
#define AX_TXD_NAVUSEHDR		BIT(10)
#define AX_TXD_BCN_SRCH_SEQ_SH		8
#define AX_TXD_BCN_SRCH_SEQ_MSK		0x3
#define AX_TXD_MAX_AGG_NUM_SH		0
#define AX_TXD_MAX_AGG_NUM_MSK		0xff

/* dword8 */
#define AX_TXD_OBW_CTS2SELF_DUP_TYPE_SH		26
#define AX_TXD_OBW_CTS2SELF_DUP_TYPE_MSK		0xf
#define AX_TXD_TXPWR_OFSET_TYPE_SH		22
#define AX_TXD_TXPWR_OFSET_TYPE_MSK		0x7
#define AX_TXD_LSIG_TXOP_EN		BIT(21)
#define AX_TXD_AMPDU_DENSITY_SH		18
#define AX_TXD_AMPDU_DENSITY_MSK		0x7
#define AX_TXD_FORCE_TXOP		BIT(17)
#define AX_TXD_LIFETIME_SEL_SH		13
#define AX_TXD_LIFETIME_SEL_MSK		0x7
#define AX_TXD_SECTYPE_SH		9
#define AX_TXD_SECTYPE_MSK		0xf
#define AX_TXD_SEC_HW_ENC		BIT(8)
#define AX_TXD_SEC_CAM_IDX_SH		0
#define AX_TXD_SEC_CAM_IDX_MSK		0xff

/* dword9 */
#define AX_TXD_FORCE_BSS_CLR		BIT(31)
#define AX_TXD_SIGNALING_TA_PKT_SC_SH		27
#define AX_TXD_SIGNALING_TA_PKT_SC_MSK		0xf
#define AX_TXD_BCNPKT_TSF_CTRL		BIT(26)
#define AX_TXD_GROUP_BIT_IE_OFFSET_SH		16
#define AX_TXD_GROUP_BIT_IE_OFFSET_MSK		0xff
#define AX_TXD_RAW		BIT(15)
#define AX_TXD_NULL_1		BIT(14)
#define AX_TXD_NULL_0		BIT(13)
#define AX_TXD_TRI_FRAME		BIT(12)
#define AX_TXD_BT_NULL		BIT(11)
#define AX_TXD_SPE_RPT		BIT(10)
#define AX_TXD_RTT_EN		BIT(9)
#define AX_TXD_HT_DATA_SND		BIT(7)
#define AX_TXD_SIFS_TX		BIT(6)
#define AX_TXD_SND_PKT_SEL_SH		3
#define AX_TXD_SND_PKT_SEL_MSK		0x7
#define AX_TXD_NDPA_SH		1
#define AX_TXD_NDPA_MSK		0x3
#define AX_TXD_SIGNALING_TA_PKT_EN		BIT(0)

/* dword10 */
#define AX_TXD_HW_RTS_EN		BIT(31)
#define AX_TXD_CCA_RTS_SH		29
#define AX_TXD_CCA_RTS_MSK		0x3
#define AX_TXD_CTS2SELF		BIT(28)
#define AX_TXD_RTS_EN		BIT(27)
#define AX_TXD_SW_DEFINE_SH		0
#define AX_TXD_SW_DEFINE_MSK		0xf

/* dword11 */
#define AX_TXD_NDPA_DURATION_SH		16
#define AX_TXD_NDPA_DURATION_MSK		0xffff

/* dword12 */
#define AX_TXD_VALID_1		BIT(31)
#define AX_TXD_PCIE_SEQ_NUM_1_SH		16
#define AX_TXD_PCIE_SEQ_NUM_1_MSK		0x7fff
#define AX_TXD_VALID_0		BIT(15)
#define AX_TXD_PCIE_SEQ_NUM_0_SH		0
#define AX_TXD_PCIE_SEQ_NUM_0_MSK		0x7fff

/* dword13 */
#define AX_TXD_VALID_3		BIT(31)
#define AX_TXD_PCIE_SEQ_NUM_3_SH		16
#define AX_TXD_PCIE_SEQ_NUM_3_MSK		0x7fff
#define AX_TXD_VALID_2		BIT(15)
#define AX_TXD_PCIE_SEQ_NUM_2_SH		0
#define AX_TXD_PCIE_SEQ_NUM_2_MSK		0x7fff

#endif

#if MAC_AX_8852C_SUPPORT
/* dword0 */
#define AX_TXD_NO_ACK		BIT(31)
#define AX_TXD_UPD_WLAN_HDR		BIT(30)
#define AX_TXD_WP_OFFSET_V1_SH		24
#define AX_TXD_WP_OFFSET_V1_MSK		0x1f
#define AX_TXD_MOREDATA		BIT(23)
#define AX_TXD_WDINFO_EN		BIT(22)
#define AX_TXD_PKT_OFFSET		BIT(21)
#define AX_TXD_FWDL_EN		BIT(20)
#define AX_TXD_CH_DMA_SH		16
#define AX_TXD_CH_DMA_MSK		0xf
#define AX_TXD_HDR_LLC_LEN_SH		11
#define AX_TXD_HDR_LLC_LEN_MSK		0x1f
#define AX_TXD_STF_MODE		BIT(10)
#define AX_TXD_WP_INT		BIT(9)
#define AX_TXD_CHK_EN		BIT(8)
#define AX_TXD_WD_PAGE		BIT(7)
#define AX_TXD_HW_SEC_IV		BIT(6)
#define AX_TXD_HWAMSDU		BIT(5)
#define AX_TXD_SMH_EN		BIT(4)
#define AX_TXD_HW_SSN_SEL_SH		2
#define AX_TXD_HW_SSN_SEL_MSK		0x3
#define AX_TXD_EN_HWSEQ_MODE_SH		0
#define AX_TXD_EN_HWSEQ_MODE_MSK		0x3

/* dword1 */
#define AX_TXD_ADDR_INFO_NUM_SH		26
#define AX_TXD_ADDR_INFO_NUM_MSK		0x3f
#define AX_TXD_REUSE_START_NUM_SH		24
#define AX_TXD_REUSE_START_NUM_MSK		0x3
#define AX_TXD_REUSE_SIZE_SH		20
#define AX_TXD_REUSE_SIZE_MSK		0xf
#define AX_TXD_DMA_TXAGG_NUM_V1_SH		8
#define AX_TXD_DMA_TXAGG_NUM_V1_MSK		0x7f
#define AX_TXD_SW_SEC_IV		BIT(6)
#define AX_TXD_SEC_KEYID_SH		4
#define AX_TXD_SEC_KEYID_MSK		0x3
#define AX_TXD_SEC_TYPE_SH		0
#define AX_TXD_SEC_TYPE_MSK		0xf

/* dword2 */
#define AX_TXD_MACID_SH		24
#define AX_TXD_MACID_MSK		0x7f
#define AX_TXD_TID_IND		BIT(23)
#define AX_TXD_QSEL_SH		17
#define AX_TXD_QSEL_MSK		0x3f
#define AX_TXD_MU_2ND_RTY		BIT(16)
#define AX_TXD_MU_PRI_RTY		BIT(15)
#define AX_TXD_RU_RTY		BIT(14)
#define AX_TXD_TXPKTSIZE_SH		0
#define AX_TXD_TXPKTSIZE_MSK		0x3fff

/* dword3 */
#define AX_TXD_TB_SR_RTY		BIT(31)
#define AX_TXD_DATA_TC_SH		20
#define AX_TXD_DATA_TC_MSK		0x3f
#define AX_TXD_RTS_TC_SH		14
#define AX_TXD_RTS_TC_MSK		0x3f
#define AX_TXD_BK		BIT(13)
#define AX_TXD_AGG_EN		BIT(12)
#define AX_TXD_WIFI_SEQ_SH		0
#define AX_TXD_WIFI_SEQ_MSK		0xfff

/* dword4 */
#define AX_TXD_SEC_IV_L_SH		16
#define AX_TXD_SEC_IV_L_MSK		0xffff
#define AX_TXD_TXDESC_CHECKSUM_SH		0
#define AX_TXD_TXDESC_CHECKSUM_MSK		0xffff

/* dword5 */
#define AX_TXD_SEC_IV_H_SH		0
#define AX_TXD_SEC_IV_H_MSK		0xffffffff

/* dword6 */
#define AX_TXD_RU_POS_SH		24
#define AX_TXD_RU_POS_MSK		0xff
#define AX_TXD_S_IDX_SH		16
#define AX_TXD_S_IDX_MSK		0xff
#define AX_TXD_RU_TC_V1_SH		5
#define AX_TXD_RU_TC_V1_MSK		0x1f
#define AX_TXD_MU_TC_V1_SH		0
#define AX_TXD_MU_TC_V1_MSK		0x1f

/* dword7 */
#define AX_TXD_USERATE_SEL_V1		BIT(31)
#define AX_TXD_DATA_DCM_V1		BIT(30)
#define AX_TXD_DATA_BW_SH		28
#define AX_TXD_DATA_BW_MSK		0x3
#define AX_TXD_GI_LTF_SH		25
#define AX_TXD_GI_LTF_MSK		0x7
#define AX_TXD_DATARATE_SH		16
#define AX_TXD_DATARATE_MSK		0x1ff

/* dword8 */
#define AX_TXD_ACK_CH_INFO		BIT(31)
#define AX_TXD_RLS_TO_CPUIO		BIT(30)
#define AX_TXD_GI_LTF_SH		25
#define AX_TXD_GI_LTF_MSK		0x7
#define AX_TXD_DATA_ER		BIT(15)
#define AX_TXD_DATA_STBC		BIT(12)
#define AX_TXD_DATA_LDPC		BIT(11)
#define AX_TXD_DISDATAFB		BIT(10)
#define AX_TXD_DISRTSFB		BIT(9)
#define AX_TXD_DATA_BW_ER		BIT(8)
#define AX_TXD_MULTIPORT_ID_SH		4
#define AX_TXD_MULTIPORT_ID_MSK		0x7
#define AX_TXD_MBSSID_SH		0
#define AX_TXD_MBSSID_MSK		0xf

/* dword9 */
#define AX_TXD_DATA_TXCNT_LMT_SEL		BIT(31)
#define AX_TXD_DATA_TXCNT_LMT_SH		25
#define AX_TXD_DATA_TXCNT_LMT_MSK		0x3f
#define AX_TXD_DATA_RTY_LOWEST_RATE_SH		16
#define AX_TXD_DATA_RTY_LOWEST_RATE_MSK		0x1ff
#define AX_TXD_A_CTRL_CAS		BIT(15)
#define AX_TXD_A_CTRL_BSR		BIT(14)
#define AX_TXD_A_CTRL_UPH		BIT(13)
#define AX_TXD_A_CTRL_BQR		BIT(12)
#define AX_TXD_BMC		BIT(11)
#define AX_TXD_NAVUSEHDR		BIT(10)
#define AX_TXD_BCN_SRCH_SEQ_SH		8
#define AX_TXD_BCN_SRCH_SEQ_MSK		0x3
#define AX_TXD_MAX_AGG_NUM_SH		0
#define AX_TXD_MAX_AGG_NUM_MSK		0xff

/* dword10 */
#define AX_TXD_OBW_CTS2SELF_DUP_TYPE_SH		26
#define AX_TXD_OBW_CTS2SELF_DUP_TYPE_MSK		0xf
#define AX_TXD_TXPWR_OFSET_TYPE_SH		22
#define AX_TXD_TXPWR_OFSET_TYPE_MSK		0x7
#define AX_TXD_LSIG_TXOP_EN		BIT(21)
#define AX_TXD_AMPDU_DENSITY_SH		18
#define AX_TXD_AMPDU_DENSITY_MSK		0x7
#define AX_TXD_FORCE_TXOP		BIT(17)
#define AX_TXD_LIFETIME_SEL_SH		13
#define AX_TXD_LIFETIME_SEL_MSK		0x7
#define AX_TXD_FORCE_KEY_EN		BIT(8)
#define AX_TXD_SEC_CAM_IDX_SH		0
#define AX_TXD_SEC_CAM_IDX_MSK		0xff

/* dword11 */
#define AX_TXD_FORCE_BSS_CLR		BIT(31)
#define AX_TXD_SIGNALING_TA_PKT_SC_SH		27
#define AX_TXD_SIGNALING_TA_PKT_SC_MSK		0xf
#define AX_TXD_BCNPKT_TSF_CTRL		BIT(26)
#define AX_TXD_GROUP_BIT_IE_OFFSET_SH		16
#define AX_TXD_GROUP_BIT_IE_OFFSET_MSK		0xff
#define AX_TXD_RAW		BIT(15)
#define AX_TXD_NULL_1		BIT(14)
#define AX_TXD_NULL_0		BIT(13)
#define AX_TXD_TRI_FRAME		BIT(12)
#define AX_TXD_BT_NULL		BIT(11)
#define AX_TXD_SPE_RPT		BIT(10)
#define AX_TXD_RTT_EN		BIT(9)
#define AX_TXD_HT_DATA_SND		BIT(7)
#define AX_TXD_SIFS_TX		BIT(6)
#define AX_TXD_SND_PKT_SEL_SH		3
#define AX_TXD_SND_PKT_SEL_MSK		0x7
#define AX_TXD_NDPA_SH		1
#define AX_TXD_NDPA_MSK		0x3
#define AX_TXD_SIGNALING_TA_PKT_EN		BIT(0)

/* dword12 */
#define AX_TXD_HW_RTS_EN		BIT(31)
#define AX_TXD_CCA_RTS_SH		29
#define AX_TXD_CCA_RTS_MSK		0x3
#define AX_TXD_CTS2SELF		BIT(28)
#define AX_TXD_RTS_EN		BIT(27)
#define AX_TXD_SW_DEFINE_SH		0
#define AX_TXD_SW_DEFINE_MSK		0xf

/* dword13 */
#define AX_TXD_NDPA_DURATION_SH		16
#define AX_TXD_NDPA_DURATION_MSK		0xffff

/* dword14 */
#define AX_TXD_VALID_1		BIT(31)
#define AX_TXD_PCIE_SEQ_NUM_1_SH		16
#define AX_TXD_PCIE_SEQ_NUM_1_MSK		0x7fff
#define AX_TXD_VALID_0		BIT(15)
#define AX_TXD_PCIE_SEQ_NUM_0_SH		0
#define AX_TXD_PCIE_SEQ_NUM_0_MSK		0x7fff

/* dword15 */
#define AX_TXD_VALID_3		BIT(31)
#define AX_TXD_PCIE_SEQ_NUM_3_SH		16
#define AX_TXD_PCIE_SEQ_NUM_3_MSK		0x7fff
#define AX_TXD_VALID_2		BIT(15)
#define AX_TXD_PCIE_SEQ_NUM_2_SH		0
#define AX_TXD_PCIE_SEQ_NUM_2_MSK		0x7fff

#endif

#if MAC_AX_8192XB_SUPPORT
/* dword0 */
#define AX_TXD_NO_ACK		BIT(31)
#define AX_TXD_UPD_WLAN_HDR		BIT(30)
#define AX_TXD_WP_OFFSET_V1_SH		24
#define AX_TXD_WP_OFFSET_V1_MSK		0x1f
#define AX_TXD_MOREDATA		BIT(23)
#define AX_TXD_WDINFO_EN		BIT(22)
#define AX_TXD_PKT_OFFSET		BIT(21)
#define AX_TXD_FWDL_EN		BIT(20)
#define AX_TXD_CH_DMA_SH		16
#define AX_TXD_CH_DMA_MSK		0xf
#define AX_TXD_HDR_LLC_LEN_SH		11
#define AX_TXD_HDR_LLC_LEN_MSK		0x1f
#define AX_TXD_STF_MODE		BIT(10)
#define AX_TXD_WP_INT		BIT(9)
#define AX_TXD_CHK_EN		BIT(8)
#define AX_TXD_WD_PAGE		BIT(7)
#define AX_TXD_HW_SEC_IV		BIT(6)
#define AX_TXD_HWAMSDU		BIT(5)
#define AX_TXD_SMH_EN		BIT(4)
#define AX_TXD_HW_SSN_SEL_SH		2
#define AX_TXD_HW_SSN_SEL_MSK		0x3
#define AX_TXD_EN_HWSEQ_MODE_SH		0
#define AX_TXD_EN_HWSEQ_MODE_MSK		0x3

/* dword1 */
#define AX_TXD_ADDR_INFO_NUM_SH		26
#define AX_TXD_ADDR_INFO_NUM_MSK		0x3f
#define AX_TXD_REUSE_START_NUM_SH		24
#define AX_TXD_REUSE_START_NUM_MSK		0x3
#define AX_TXD_REUSE_SIZE_SH		20
#define AX_TXD_REUSE_SIZE_MSK		0xf
#define AX_TXD_DMA_TXAGG_NUM_V1_SH		8
#define AX_TXD_DMA_TXAGG_NUM_V1_MSK		0x7f
#define AX_TXD_SW_SEC_IV		BIT(6)
#define AX_TXD_SEC_KEYID_SH		4
#define AX_TXD_SEC_KEYID_MSK		0x3
#define AX_TXD_SEC_TYPE_SH		0
#define AX_TXD_SEC_TYPE_MSK		0xf

/* dword2 */
#define AX_TXD_MACID_SH		24
#define AX_TXD_MACID_MSK		0x7f
#define AX_TXD_TID_IND		BIT(23)
#define AX_TXD_QSEL_SH		17
#define AX_TXD_QSEL_MSK		0x3f
#define AX_TXD_MU_2ND_RTY		BIT(16)
#define AX_TXD_MU_PRI_RTY		BIT(15)
#define AX_TXD_RU_RTY		BIT(14)
#define AX_TXD_TXPKTSIZE_SH		0
#define AX_TXD_TXPKTSIZE_MSK		0x3fff

/* dword3 */
#define AX_TXD_TB_SR_RTY		BIT(31)
#define AX_TXD_DATA_TC_SH		20
#define AX_TXD_DATA_TC_MSK		0x3f
#define AX_TXD_RTS_TC_SH		14
#define AX_TXD_RTS_TC_MSK		0x3f
#define AX_TXD_BK		BIT(13)
#define AX_TXD_AGG_EN		BIT(12)
#define AX_TXD_WIFI_SEQ_SH		0
#define AX_TXD_WIFI_SEQ_MSK		0xfff

/* dword4 */
#define AX_TXD_SEC_IV_L_SH		16
#define AX_TXD_SEC_IV_L_MSK		0xffff
#define AX_TXD_TXDESC_CHECKSUM_SH		0
#define AX_TXD_TXDESC_CHECKSUM_MSK		0xffff

/* dword5 */
#define AX_TXD_SEC_IV_H_SH		0
#define AX_TXD_SEC_IV_H_MSK		0xffffffff

/* dword6 */
#define AX_TXD_RU_POS_SH		24
#define AX_TXD_RU_POS_MSK		0xff
#define AX_TXD_S_IDX_SH		16
#define AX_TXD_S_IDX_MSK		0xff
#define AX_TXD_RU_TC_V1_SH		5
#define AX_TXD_RU_TC_V1_MSK		0x1f
#define AX_TXD_MU_TC_V1_SH		0
#define AX_TXD_MU_TC_V1_MSK		0x1f

/* dword7 */
#define AX_TXD_USERATE_SEL_V1		BIT(31)
#define AX_TXD_DATA_DCM_V1		BIT(30)
#define AX_TXD_DATA_BW_SH		28
#define AX_TXD_DATA_BW_MSK		0x3
#define AX_TXD_GI_LTF_SH		25
#define AX_TXD_GI_LTF_MSK		0x7
#define AX_TXD_DATARATE_SH		16
#define AX_TXD_DATARATE_MSK		0x1ff

/* dword8 */
#define AX_TXD_ACK_CH_INFO		BIT(31)
#define AX_TXD_RLS_TO_CPUIO		BIT(30)
#define AX_TXD_GI_LTF_SH		25
#define AX_TXD_GI_LTF_MSK		0x7
#define AX_TXD_DATA_ER		BIT(15)
#define AX_TXD_DATA_STBC		BIT(12)
#define AX_TXD_DATA_LDPC		BIT(11)
#define AX_TXD_DISDATAFB		BIT(10)
#define AX_TXD_DISRTSFB		BIT(9)
#define AX_TXD_DATA_BW_ER		BIT(8)
#define AX_TXD_MULTIPORT_ID_SH		4
#define AX_TXD_MULTIPORT_ID_MSK		0x7
#define AX_TXD_MBSSID_SH		0
#define AX_TXD_MBSSID_MSK		0xf

/* dword9 */
#define AX_TXD_DATA_TXCNT_LMT_SEL		BIT(31)
#define AX_TXD_DATA_TXCNT_LMT_SH		25
#define AX_TXD_DATA_TXCNT_LMT_MSK		0x3f
#define AX_TXD_DATA_RTY_LOWEST_RATE_SH		16
#define AX_TXD_DATA_RTY_LOWEST_RATE_MSK		0x1ff
#define AX_TXD_A_CTRL_CAS		BIT(15)
#define AX_TXD_A_CTRL_BSR		BIT(14)
#define AX_TXD_A_CTRL_UPH		BIT(13)
#define AX_TXD_A_CTRL_BQR		BIT(12)
#define AX_TXD_BMC		BIT(11)
#define AX_TXD_NAVUSEHDR		BIT(10)
#define AX_TXD_BCN_SRCH_SEQ_SH		8
#define AX_TXD_BCN_SRCH_SEQ_MSK		0x3
#define AX_TXD_MAX_AGG_NUM_SH		0
#define AX_TXD_MAX_AGG_NUM_MSK		0xff

/* dword10 */
#define AX_TXD_OBW_CTS2SELF_DUP_TYPE_SH		26
#define AX_TXD_OBW_CTS2SELF_DUP_TYPE_MSK		0xf
#define AX_TXD_TXPWR_OFSET_TYPE_SH		22
#define AX_TXD_TXPWR_OFSET_TYPE_MSK		0x7
#define AX_TXD_LSIG_TXOP_EN		BIT(21)
#define AX_TXD_AMPDU_DENSITY_SH		18
#define AX_TXD_AMPDU_DENSITY_MSK		0x7
#define AX_TXD_FORCE_TXOP		BIT(17)
#define AX_TXD_LIFETIME_SEL_SH		13
#define AX_TXD_LIFETIME_SEL_MSK		0x7
#define AX_TXD_FORCE_KEY_EN		BIT(8)
#define AX_TXD_SEC_CAM_IDX_SH		0
#define AX_TXD_SEC_CAM_IDX_MSK		0xff

/* dword11 */
#define AX_TXD_FORCE_BSS_CLR		BIT(31)
#define AX_TXD_SIGNALING_TA_PKT_SC_SH		27
#define AX_TXD_SIGNALING_TA_PKT_SC_MSK		0xf
#define AX_TXD_BCNPKT_TSF_CTRL		BIT(26)
#define AX_TXD_GROUP_BIT_IE_OFFSET_SH		16
#define AX_TXD_GROUP_BIT_IE_OFFSET_MSK		0xff
#define AX_TXD_RAW		BIT(15)
#define AX_TXD_NULL_1		BIT(14)
#define AX_TXD_NULL_0		BIT(13)
#define AX_TXD_TRI_FRAME		BIT(12)
#define AX_TXD_BT_NULL		BIT(11)
#define AX_TXD_SPE_RPT		BIT(10)
#define AX_TXD_RTT_EN		BIT(9)
#define AX_TXD_HT_DATA_SND		BIT(7)
#define AX_TXD_SIFS_TX		BIT(6)
#define AX_TXD_SND_PKT_SEL_SH		3
#define AX_TXD_SND_PKT_SEL_MSK		0x7
#define AX_TXD_NDPA_SH		1
#define AX_TXD_NDPA_MSK		0x3
#define AX_TXD_SIGNALING_TA_PKT_EN		BIT(0)

/* dword12 */
#define AX_TXD_HW_RTS_EN		BIT(31)
#define AX_TXD_CCA_RTS_SH		29
#define AX_TXD_CCA_RTS_MSK		0x3
#define AX_TXD_CTS2SELF		BIT(28)
#define AX_TXD_RTS_EN		BIT(27)
#define AX_TXD_SW_DEFINE_SH		0
#define AX_TXD_SW_DEFINE_MSK		0xf

/* dword13 */
#define AX_TXD_NDPA_DURATION_SH		16
#define AX_TXD_NDPA_DURATION_MSK		0xffff

/* dword14 */
#define AX_TXD_VALID_1		BIT(31)
#define AX_TXD_PCIE_SEQ_NUM_1_SH		16
#define AX_TXD_PCIE_SEQ_NUM_1_MSK		0x7fff
#define AX_TXD_VALID_0		BIT(15)
#define AX_TXD_PCIE_SEQ_NUM_0_SH		0
#define AX_TXD_PCIE_SEQ_NUM_0_MSK		0x7fff

/* dword15 */
#define AX_TXD_VALID_3		BIT(31)
#define AX_TXD_PCIE_SEQ_NUM_3_SH		16
#define AX_TXD_PCIE_SEQ_NUM_3_MSK		0x7fff
#define AX_TXD_VALID_2		BIT(15)
#define AX_TXD_PCIE_SEQ_NUM_2_SH		0
#define AX_TXD_PCIE_SEQ_NUM_2_MSK		0x7fff

#endif

#if MAC_AX_8851B_SUPPORT
/* dword0 */
#define AX_TXD_WP_OFFSET_SH		24
#define AX_TXD_WP_OFFSET_MSK		0xff
#define AX_TXD_MOREDATA		BIT(23)
#define AX_TXD_WDINFO_EN		BIT(22)
#define AX_TXD_PKT_OFFSET		BIT(21)
#define AX_TXD_FWDL_EN		BIT(20)
#define AX_TXD_CH_DMA_SH		16
#define AX_TXD_CH_DMA_MSK		0xf
#define AX_TXD_HDR_LLC_LEN_SH		11
#define AX_TXD_HDR_LLC_LEN_MSK		0x1f
#define AX_TXD_STF_MODE		BIT(10)
#define AX_TXD_WP_INT		BIT(9)
#define AX_TXD_CHK_EN		BIT(8)
#define AX_TXD_WD_PAGE		BIT(7)
#define AX_TXD_HW_AES_IV		BIT(6)
#define AX_TXD_HWAMSDU		BIT(5)
#define AX_TXD_SMH_EN		BIT(4)
#define AX_TXD_HW_SSN_SEL_SH		2
#define AX_TXD_HW_SSN_SEL_MSK		0x3
#define AX_TXD_EN_HWSEQ_MODE_SH		0
#define AX_TXD_EN_HWSEQ_MODE_MSK		0x3

/* dword1 */
#define AX_TXD_PLD_SH		16
#define AX_TXD_PLD_MSK		0xffff
#define AX_TXD_DMA_TXAGG_NUM_SH		8
#define AX_TXD_DMA_TXAGG_NUM_MSK		0xff
#define AX_TXD_SHCUT_CAMID_SH		0
#define AX_TXD_SHCUT_CAMID_MSK		0xff

/* dword2 */
#define AX_TXD_MACID_SH		24
#define AX_TXD_MACID_MSK		0x7f
#define AX_TXD_TID_IND		BIT(23)
#define AX_TXD_QSEL_SH		17
#define AX_TXD_QSEL_MSK		0x3f
#define AX_TXD_RU_TC_SH		14
#define AX_TXD_RU_TC_MSK		0x7
#define AX_TXD_TXPKTSIZE_SH		0
#define AX_TXD_TXPKTSIZE_MSK		0x3fff

/* dword3 */
#define AX_TXD_MU_TC_SH		29
#define AX_TXD_MU_TC_MSK		0x7
#define AX_TXD_MU_2ND_TC_SH		26
#define AX_TXD_MU_2ND_TC_MSK		0x7
#define AX_TXD_DATA_TC_SH		20
#define AX_TXD_DATA_TC_MSK		0x3f
#define AX_TXD_RTS_TC_SH		14
#define AX_TXD_RTS_TC_MSK		0x3f
#define AX_TXD_BK		BIT(13)
#define AX_TXD_AGG_EN		BIT(12)
#define AX_TXD_WIFI_SEQ_SH		0
#define AX_TXD_WIFI_SEQ_MSK		0xfff

/* dword4 */
#define AX_TXD_AES_IV_L_SH		16
#define AX_TXD_AES_IV_L_MSK		0xffff
#define AX_TXD_TXDESC_CHECKSUM_SH		0
#define AX_TXD_TXDESC_CHECKSUM_MSK		0xffff

/* dword5 */
#define AX_TXD_AES_IV_H_SH		0
#define AX_TXD_AES_IV_H_MSK		0xffffffff

/* dword6 */
#define AX_TXD_ACK_CH_INFO		BIT(31)
#define AX_TXD_USERATE_SEL		BIT(30)
#define AX_TXD_DATA_BW_SH		28
#define AX_TXD_DATA_BW_MSK		0x3
#define AX_TXD_GI_LTF_SH		25
#define AX_TXD_GI_LTF_MSK		0x7
#define AX_TXD_DATARATE_SH		16
#define AX_TXD_DATARATE_MSK		0x1ff
#define AX_TXD_DATA_ER		BIT(15)
#define AX_TXD_DATA_DCM		BIT(14)
#define AX_TXD_DATA_STBC		BIT(12)
#define AX_TXD_DATA_LDPC		BIT(11)
#define AX_TXD_DISDATAFB		BIT(10)
#define AX_TXD_DISRTSFB		BIT(9)
#define AX_TXD_DATA_BW_ER		BIT(8)
#define AX_TXD_MULTIPORT_ID_SH		4
#define AX_TXD_MULTIPORT_ID_MSK		0x7
#define AX_TXD_MBSSID_SH		0
#define AX_TXD_MBSSID_MSK		0xf

/* dword7 */
#define AX_TXD_DATA_TXCNT_LMT_SEL		BIT(31)
#define AX_TXD_DATA_TXCNT_LMT_SH		25
#define AX_TXD_DATA_TXCNT_LMT_MSK		0x3f
#define AX_TXD_DATA_RTY_LOWEST_RATE_SH		16
#define AX_TXD_DATA_RTY_LOWEST_RATE_MSK		0x1ff
#define AX_TXD_A_CTRL_CAS		BIT(15)
#define AX_TXD_A_CTRL_BSR		BIT(14)
#define AX_TXD_A_CTRL_UPH		BIT(13)
#define AX_TXD_A_CTRL_BQR		BIT(12)
#define AX_TXD_BMC		BIT(11)
#define AX_TXD_NAVUSEHDR		BIT(10)
#define AX_TXD_BCN_SRCH_SEQ_SH		8
#define AX_TXD_BCN_SRCH_SEQ_MSK		0x3
#define AX_TXD_MAX_AGG_NUM_SH		0
#define AX_TXD_MAX_AGG_NUM_MSK		0xff

/* dword8 */
#define AX_TXD_OBW_CTS2SELF_DUP_TYPE_SH		26
#define AX_TXD_OBW_CTS2SELF_DUP_TYPE_MSK		0xf
#define AX_TXD_TXPWR_OFSET_TYPE_SH		22
#define AX_TXD_TXPWR_OFSET_TYPE_MSK		0x7
#define AX_TXD_LSIG_TXOP_EN		BIT(21)
#define AX_TXD_AMPDU_DENSITY_SH		18
#define AX_TXD_AMPDU_DENSITY_MSK		0x7
#define AX_TXD_FORCE_TXOP		BIT(17)
#define AX_TXD_LIFETIME_SEL_SH		13
#define AX_TXD_LIFETIME_SEL_MSK		0x7
#define AX_TXD_SECTYPE_SH		9
#define AX_TXD_SECTYPE_MSK		0xf
#define AX_TXD_SEC_HW_ENC		BIT(8)
#define AX_TXD_SEC_CAM_IDX_SH		0
#define AX_TXD_SEC_CAM_IDX_MSK		0xff

/* dword9 */
#define AX_TXD_FORCE_BSS_CLR		BIT(31)
#define AX_TXD_SIGNALING_TA_PKT_SC_SH		27
#define AX_TXD_SIGNALING_TA_PKT_SC_MSK		0xf
#define AX_TXD_BCNPKT_TSF_CTRL		BIT(26)
#define AX_TXD_GROUP_BIT_IE_OFFSET_SH		16
#define AX_TXD_GROUP_BIT_IE_OFFSET_MSK		0xff
#define AX_TXD_RAW		BIT(15)
#define AX_TXD_NULL_1		BIT(14)
#define AX_TXD_NULL_0		BIT(13)
#define AX_TXD_TRI_FRAME		BIT(12)
#define AX_TXD_BT_NULL		BIT(11)
#define AX_TXD_SPE_RPT		BIT(10)
#define AX_TXD_RTT_EN		BIT(9)
#define AX_TXD_HT_DATA_SND		BIT(7)
#define AX_TXD_SIFS_TX		BIT(6)
#define AX_TXD_SND_PKT_SEL_SH		3
#define AX_TXD_SND_PKT_SEL_MSK		0x7
#define AX_TXD_NDPA_SH		1
#define AX_TXD_NDPA_MSK		0x3
#define AX_TXD_SIGNALING_TA_PKT_EN		BIT(0)

/* dword10 */
#define AX_TXD_HW_RTS_EN		BIT(31)
#define AX_TXD_CCA_RTS_SH		29
#define AX_TXD_CCA_RTS_MSK		0x3
#define AX_TXD_CTS2SELF		BIT(28)
#define AX_TXD_RTS_EN		BIT(27)
#define AX_TXD_SW_DEFINE_SH		0
#define AX_TXD_SW_DEFINE_MSK		0xf

/* dword11 */
#define AX_TXD_NDPA_DURATION_SH		16
#define AX_TXD_NDPA_DURATION_MSK		0xffff

/* dword12 */
#define AX_TXD_VALID_1		BIT(31)
#define AX_TXD_PCIE_SEQ_NUM_1_SH		16
#define AX_TXD_PCIE_SEQ_NUM_1_MSK		0x7fff
#define AX_TXD_VALID_0		BIT(15)
#define AX_TXD_PCIE_SEQ_NUM_0_SH		0
#define AX_TXD_PCIE_SEQ_NUM_0_MSK		0x7fff

/* dword13 */
#define AX_TXD_VALID_3		BIT(31)
#define AX_TXD_PCIE_SEQ_NUM_3_SH		16
#define AX_TXD_PCIE_SEQ_NUM_3_MSK		0x7fff
#define AX_TXD_VALID_2		BIT(15)
#define AX_TXD_PCIE_SEQ_NUM_2_SH		0
#define AX_TXD_PCIE_SEQ_NUM_2_MSK		0x7fff

#endif

#if MAC_AX_8851E_SUPPORT
/* dword0 */
#define AX_TXD_NO_ACK		BIT(31)
#define AX_TXD_UPD_WLAN_HDR		BIT(30)
#define AX_TXD_WP_OFFSET_V1_SH		24
#define AX_TXD_WP_OFFSET_V1_MSK		0x1f
#define AX_TXD_MOREDATA		BIT(23)
#define AX_TXD_WDINFO_EN		BIT(22)
#define AX_TXD_PKT_OFFSET		BIT(21)
#define AX_TXD_FWDL_EN		BIT(20)
#define AX_TXD_CH_DMA_SH		16
#define AX_TXD_CH_DMA_MSK		0xf
#define AX_TXD_HDR_LLC_LEN_SH		11
#define AX_TXD_HDR_LLC_LEN_MSK		0x1f
#define AX_TXD_STF_MODE		BIT(10)
#define AX_TXD_WP_INT		BIT(9)
#define AX_TXD_CHK_EN		BIT(8)
#define AX_TXD_WD_PAGE		BIT(7)
#define AX_TXD_HW_SEC_IV		BIT(6)
#define AX_TXD_HWAMSDU		BIT(5)
#define AX_TXD_SMH_EN		BIT(4)
#define AX_TXD_HW_SSN_SEL_SH		2
#define AX_TXD_HW_SSN_SEL_MSK		0x3
#define AX_TXD_EN_HWSEQ_MODE_SH		0
#define AX_TXD_EN_HWSEQ_MODE_MSK		0x3

/* dword1 */
#define AX_TXD_ADDR_INFO_NUM_SH		26
#define AX_TXD_ADDR_INFO_NUM_MSK		0x3f
#define AX_TXD_REUSE_START_NUM_SH		24
#define AX_TXD_REUSE_START_NUM_MSK		0x3
#define AX_TXD_REUSE_SIZE_SH		20
#define AX_TXD_REUSE_SIZE_MSK		0xf
#define AX_TXD_DMA_TXAGG_NUM_V1_SH		8
#define AX_TXD_DMA_TXAGG_NUM_V1_MSK		0x7f
#define AX_TXD_SW_SEC_IV		BIT(6)
#define AX_TXD_SEC_KEYID_SH		4
#define AX_TXD_SEC_KEYID_MSK		0x3
#define AX_TXD_SEC_TYPE_SH		0
#define AX_TXD_SEC_TYPE_MSK		0xf

/* dword2 */
#define AX_TXD_MACID_SH		24
#define AX_TXD_MACID_MSK		0x7f
#define AX_TXD_TID_IND		BIT(23)
#define AX_TXD_QSEL_SH		17
#define AX_TXD_QSEL_MSK		0x3f
#define AX_TXD_MU_2ND_RTY		BIT(16)
#define AX_TXD_MU_PRI_RTY		BIT(15)
#define AX_TXD_RU_RTY		BIT(14)
#define AX_TXD_TXPKTSIZE_SH		0
#define AX_TXD_TXPKTSIZE_MSK		0x3fff

/* dword3 */
#define AX_TXD_TB_SR_RTY		BIT(31)
#define AX_TXD_DATA_TC_SH		20
#define AX_TXD_DATA_TC_MSK		0x3f
#define AX_TXD_RTS_TC_SH		14
#define AX_TXD_RTS_TC_MSK		0x3f
#define AX_TXD_BK		BIT(13)
#define AX_TXD_AGG_EN		BIT(12)
#define AX_TXD_WIFI_SEQ_SH		0
#define AX_TXD_WIFI_SEQ_MSK		0xfff

/* dword4 */
#define AX_TXD_SEC_IV_L_SH		16
#define AX_TXD_SEC_IV_L_MSK		0xffff
#define AX_TXD_TXDESC_CHECKSUM_SH		0
#define AX_TXD_TXDESC_CHECKSUM_MSK		0xffff

/* dword5 */
#define AX_TXD_SEC_IV_H_SH		0
#define AX_TXD_SEC_IV_H_MSK		0xffffffff

/* dword6 */
#define AX_TXD_RU_POS_SH		24
#define AX_TXD_RU_POS_MSK		0xff
#define AX_TXD_S_IDX_SH		16
#define AX_TXD_S_IDX_MSK		0xff
#define AX_TXD_RU_TC_V1_SH		5
#define AX_TXD_RU_TC_V1_MSK		0x1f
#define AX_TXD_MU_TC_V1_SH		0
#define AX_TXD_MU_TC_V1_MSK		0x1f

/* dword7 */
#define AX_TXD_USERATE_SEL_V1		BIT(31)
#define AX_TXD_DATA_DCM_V1		BIT(30)
#define AX_TXD_DATA_BW_SH		28
#define AX_TXD_DATA_BW_MSK		0x3
#define AX_TXD_GI_LTF_SH		25
#define AX_TXD_GI_LTF_MSK		0x7
#define AX_TXD_DATARATE_SH		16
#define AX_TXD_DATARATE_MSK		0x1ff

/* dword8 */
#define AX_TXD_ACK_CH_INFO		BIT(31)
#define AX_TXD_RLS_TO_CPUIO		BIT(30)
#define AX_TXD_GI_LTF_SH		25
#define AX_TXD_GI_LTF_MSK		0x7
#define AX_TXD_DATA_ER		BIT(15)
#define AX_TXD_DATA_STBC		BIT(12)
#define AX_TXD_DATA_LDPC		BIT(11)
#define AX_TXD_DISDATAFB		BIT(10)
#define AX_TXD_DISRTSFB		BIT(9)
#define AX_TXD_DATA_BW_ER		BIT(8)
#define AX_TXD_MULTIPORT_ID_SH		4
#define AX_TXD_MULTIPORT_ID_MSK		0x7
#define AX_TXD_MBSSID_SH		0
#define AX_TXD_MBSSID_MSK		0xf

/* dword9 */
#define AX_TXD_DATA_TXCNT_LMT_SEL		BIT(31)
#define AX_TXD_DATA_TXCNT_LMT_SH		25
#define AX_TXD_DATA_TXCNT_LMT_MSK		0x3f
#define AX_TXD_DATA_RTY_LOWEST_RATE_SH		16
#define AX_TXD_DATA_RTY_LOWEST_RATE_MSK		0x1ff
#define AX_TXD_A_CTRL_CAS		BIT(15)
#define AX_TXD_A_CTRL_BSR		BIT(14)
#define AX_TXD_A_CTRL_UPH		BIT(13)
#define AX_TXD_A_CTRL_BQR		BIT(12)
#define AX_TXD_BMC		BIT(11)
#define AX_TXD_NAVUSEHDR		BIT(10)
#define AX_TXD_BCN_SRCH_SEQ_SH		8
#define AX_TXD_BCN_SRCH_SEQ_MSK		0x3
#define AX_TXD_MAX_AGG_NUM_SH		0
#define AX_TXD_MAX_AGG_NUM_MSK		0xff

/* dword10 */
#define AX_TXD_OBW_CTS2SELF_DUP_TYPE_SH		26
#define AX_TXD_OBW_CTS2SELF_DUP_TYPE_MSK		0xf
#define AX_TXD_TXPWR_OFSET_TYPE_SH		22
#define AX_TXD_TXPWR_OFSET_TYPE_MSK		0x7
#define AX_TXD_LSIG_TXOP_EN		BIT(21)
#define AX_TXD_AMPDU_DENSITY_SH		18
#define AX_TXD_AMPDU_DENSITY_MSK		0x7
#define AX_TXD_FORCE_TXOP		BIT(17)
#define AX_TXD_LIFETIME_SEL_SH		13
#define AX_TXD_LIFETIME_SEL_MSK		0x7
#define AX_TXD_FORCE_KEY_EN		BIT(8)
#define AX_TXD_SEC_CAM_IDX_SH		0
#define AX_TXD_SEC_CAM_IDX_MSK		0xff

/* dword11 */
#define AX_TXD_FORCE_BSS_CLR		BIT(31)
#define AX_TXD_SIGNALING_TA_PKT_SC_SH		27
#define AX_TXD_SIGNALING_TA_PKT_SC_MSK		0xf
#define AX_TXD_BCNPKT_TSF_CTRL		BIT(26)
#define AX_TXD_GROUP_BIT_IE_OFFSET_SH		16
#define AX_TXD_GROUP_BIT_IE_OFFSET_MSK		0xff
#define AX_TXD_RAW		BIT(15)
#define AX_TXD_NULL_1		BIT(14)
#define AX_TXD_NULL_0		BIT(13)
#define AX_TXD_TRI_FRAME		BIT(12)
#define AX_TXD_BT_NULL		BIT(11)
#define AX_TXD_SPE_RPT		BIT(10)
#define AX_TXD_RTT_EN		BIT(9)
#define AX_TXD_HT_DATA_SND		BIT(7)
#define AX_TXD_SIFS_TX		BIT(6)
#define AX_TXD_SND_PKT_SEL_SH		3
#define AX_TXD_SND_PKT_SEL_MSK		0x7
#define AX_TXD_NDPA_SH		1
#define AX_TXD_NDPA_MSK		0x3
#define AX_TXD_SIGNALING_TA_PKT_EN		BIT(0)

/* dword12 */
#define AX_TXD_HW_RTS_EN		BIT(31)
#define AX_TXD_CCA_RTS_SH		29
#define AX_TXD_CCA_RTS_MSK		0x3
#define AX_TXD_CTS2SELF		BIT(28)
#define AX_TXD_RTS_EN		BIT(27)
#define AX_TXD_SW_DEFINE_SH		0
#define AX_TXD_SW_DEFINE_MSK		0xf

/* dword13 */
#define AX_TXD_NDPA_DURATION_SH		16
#define AX_TXD_NDPA_DURATION_MSK		0xffff

/* dword14 */
#define AX_TXD_VALID_1		BIT(31)
#define AX_TXD_PCIE_SEQ_NUM_1_SH		16
#define AX_TXD_PCIE_SEQ_NUM_1_MSK		0x7fff
#define AX_TXD_VALID_0		BIT(15)
#define AX_TXD_PCIE_SEQ_NUM_0_SH		0
#define AX_TXD_PCIE_SEQ_NUM_0_MSK		0x7fff

/* dword15 */
#define AX_TXD_VALID_3		BIT(31)
#define AX_TXD_PCIE_SEQ_NUM_3_SH		16
#define AX_TXD_PCIE_SEQ_NUM_3_MSK		0x7fff
#define AX_TXD_VALID_2		BIT(15)
#define AX_TXD_PCIE_SEQ_NUM_2_SH		0
#define AX_TXD_PCIE_SEQ_NUM_2_MSK		0x7fff

#endif

#if MAC_AX_8852D_SUPPORT
/* dword0 */
#define AX_TXD_NO_ACK		BIT(31)
#define AX_TXD_UPD_WLAN_HDR		BIT(30)
#define AX_TXD_WP_OFFSET_V1_SH		24
#define AX_TXD_WP_OFFSET_V1_MSK		0x1f
#define AX_TXD_MOREDATA		BIT(23)
#define AX_TXD_WDINFO_EN		BIT(22)
#define AX_TXD_PKT_OFFSET		BIT(21)
#define AX_TXD_FWDL_EN		BIT(20)
#define AX_TXD_CH_DMA_SH		16
#define AX_TXD_CH_DMA_MSK		0xf
#define AX_TXD_HDR_LLC_LEN_SH		11
#define AX_TXD_HDR_LLC_LEN_MSK		0x1f
#define AX_TXD_STF_MODE		BIT(10)
#define AX_TXD_WP_INT		BIT(9)
#define AX_TXD_CHK_EN		BIT(8)
#define AX_TXD_WD_PAGE		BIT(7)
#define AX_TXD_HW_SEC_IV		BIT(6)
#define AX_TXD_HWAMSDU		BIT(5)
#define AX_TXD_SMH_EN		BIT(4)
#define AX_TXD_HW_SSN_SEL_SH		2
#define AX_TXD_HW_SSN_SEL_MSK		0x3
#define AX_TXD_EN_HWSEQ_MODE_SH		0
#define AX_TXD_EN_HWSEQ_MODE_MSK		0x3

/* dword1 */
#define AX_TXD_ADDR_INFO_NUM_SH		26
#define AX_TXD_ADDR_INFO_NUM_MSK		0x3f
#define AX_TXD_REUSE_START_NUM_SH		24
#define AX_TXD_REUSE_START_NUM_MSK		0x3
#define AX_TXD_REUSE_SIZE_SH		20
#define AX_TXD_REUSE_SIZE_MSK		0xf
#define AX_TXD_DMA_TXAGG_NUM_V1_SH		8
#define AX_TXD_DMA_TXAGG_NUM_V1_MSK		0x7f
#define AX_TXD_SW_SEC_IV		BIT(6)
#define AX_TXD_SEC_KEYID_SH		4
#define AX_TXD_SEC_KEYID_MSK		0x3
#define AX_TXD_SEC_TYPE_SH		0
#define AX_TXD_SEC_TYPE_MSK		0xf

/* dword2 */
#define AX_TXD_MACID_SH		24
#define AX_TXD_MACID_MSK		0x7f
#define AX_TXD_TID_IND		BIT(23)
#define AX_TXD_QSEL_SH		17
#define AX_TXD_QSEL_MSK		0x3f
#define AX_TXD_MU_2ND_RTY		BIT(16)
#define AX_TXD_MU_PRI_RTY		BIT(15)
#define AX_TXD_RU_RTY		BIT(14)
#define AX_TXD_TXPKTSIZE_SH		0
#define AX_TXD_TXPKTSIZE_MSK		0x3fff

/* dword3 */
#define AX_TXD_TB_SR_RTY		BIT(31)
#define AX_TXD_DATA_TC_SH		20
#define AX_TXD_DATA_TC_MSK		0x3f
#define AX_TXD_RTS_TC_SH		14
#define AX_TXD_RTS_TC_MSK		0x3f
#define AX_TXD_BK		BIT(13)
#define AX_TXD_AGG_EN		BIT(12)
#define AX_TXD_WIFI_SEQ_SH		0
#define AX_TXD_WIFI_SEQ_MSK		0xfff

/* dword4 */
#define AX_TXD_SEC_IV_L_SH		16
#define AX_TXD_SEC_IV_L_MSK		0xffff
#define AX_TXD_TXDESC_CHECKSUM_SH		0
#define AX_TXD_TXDESC_CHECKSUM_MSK		0xffff

/* dword5 */
#define AX_TXD_SEC_IV_H_SH		0
#define AX_TXD_SEC_IV_H_MSK		0xffffffff

/* dword6 */
#define AX_TXD_RU_POS_SH		24
#define AX_TXD_RU_POS_MSK		0xff
#define AX_TXD_S_IDX_SH		16
#define AX_TXD_S_IDX_MSK		0xff
#define AX_TXD_RU_TC_V1_SH		5
#define AX_TXD_RU_TC_V1_MSK		0x1f
#define AX_TXD_MU_TC_V1_SH		0
#define AX_TXD_MU_TC_V1_MSK		0x1f

/* dword7 */
#define AX_TXD_USERATE_SEL_V1		BIT(31)
#define AX_TXD_DATA_DCM_V1		BIT(30)
#define AX_TXD_DATA_BW_SH		28
#define AX_TXD_DATA_BW_MSK		0x3
#define AX_TXD_GI_LTF_SH		25
#define AX_TXD_GI_LTF_MSK		0x7
#define AX_TXD_DATARATE_SH		16
#define AX_TXD_DATARATE_MSK		0x1ff

/* dword8 */
#define AX_TXD_ACK_CH_INFO		BIT(31)
#define AX_TXD_RLS_TO_CPUIO		BIT(30)
#define AX_TXD_GI_LTF_SH		25
#define AX_TXD_GI_LTF_MSK		0x7
#define AX_TXD_DATA_ER		BIT(15)
#define AX_TXD_DATA_STBC		BIT(12)
#define AX_TXD_DATA_LDPC		BIT(11)
#define AX_TXD_DISDATAFB		BIT(10)
#define AX_TXD_DISRTSFB		BIT(9)
#define AX_TXD_DATA_BW_ER		BIT(8)
#define AX_TXD_MULTIPORT_ID_SH		4
#define AX_TXD_MULTIPORT_ID_MSK		0x7
#define AX_TXD_MBSSID_SH		0
#define AX_TXD_MBSSID_MSK		0xf

/* dword9 */
#define AX_TXD_DATA_TXCNT_LMT_SEL		BIT(31)
#define AX_TXD_DATA_TXCNT_LMT_SH		25
#define AX_TXD_DATA_TXCNT_LMT_MSK		0x3f
#define AX_TXD_DATA_RTY_LOWEST_RATE_SH		16
#define AX_TXD_DATA_RTY_LOWEST_RATE_MSK		0x1ff
#define AX_TXD_A_CTRL_CAS		BIT(15)
#define AX_TXD_A_CTRL_BSR		BIT(14)
#define AX_TXD_A_CTRL_UPH		BIT(13)
#define AX_TXD_A_CTRL_BQR		BIT(12)
#define AX_TXD_BMC		BIT(11)
#define AX_TXD_NAVUSEHDR		BIT(10)
#define AX_TXD_BCN_SRCH_SEQ_SH		8
#define AX_TXD_BCN_SRCH_SEQ_MSK		0x3
#define AX_TXD_MAX_AGG_NUM_SH		0
#define AX_TXD_MAX_AGG_NUM_MSK		0xff

/* dword10 */
#define AX_TXD_OBW_CTS2SELF_DUP_TYPE_SH		26
#define AX_TXD_OBW_CTS2SELF_DUP_TYPE_MSK		0xf
#define AX_TXD_TXPWR_OFSET_TYPE_SH		22
#define AX_TXD_TXPWR_OFSET_TYPE_MSK		0x7
#define AX_TXD_LSIG_TXOP_EN		BIT(21)
#define AX_TXD_AMPDU_DENSITY_SH		18
#define AX_TXD_AMPDU_DENSITY_MSK		0x7
#define AX_TXD_FORCE_TXOP		BIT(17)
#define AX_TXD_LIFETIME_SEL_SH		13
#define AX_TXD_LIFETIME_SEL_MSK		0x7
#define AX_TXD_FORCE_KEY_EN		BIT(8)
#define AX_TXD_SEC_CAM_IDX_SH		0
#define AX_TXD_SEC_CAM_IDX_MSK		0xff

/* dword11 */
#define AX_TXD_FORCE_BSS_CLR		BIT(31)
#define AX_TXD_SIGNALING_TA_PKT_SC_SH		27
#define AX_TXD_SIGNALING_TA_PKT_SC_MSK		0xf
#define AX_TXD_BCNPKT_TSF_CTRL		BIT(26)
#define AX_TXD_GROUP_BIT_IE_OFFSET_SH		16
#define AX_TXD_GROUP_BIT_IE_OFFSET_MSK		0xff
#define AX_TXD_RAW		BIT(15)
#define AX_TXD_NULL_1		BIT(14)
#define AX_TXD_NULL_0		BIT(13)
#define AX_TXD_TRI_FRAME		BIT(12)
#define AX_TXD_BT_NULL		BIT(11)
#define AX_TXD_SPE_RPT		BIT(10)
#define AX_TXD_RTT_EN		BIT(9)
#define AX_TXD_HT_DATA_SND		BIT(7)
#define AX_TXD_SIFS_TX		BIT(6)
#define AX_TXD_SND_PKT_SEL_SH		3
#define AX_TXD_SND_PKT_SEL_MSK		0x7
#define AX_TXD_NDPA_SH		1
#define AX_TXD_NDPA_MSK		0x3
#define AX_TXD_SIGNALING_TA_PKT_EN		BIT(0)

/* dword12 */
#define AX_TXD_HW_RTS_EN		BIT(31)
#define AX_TXD_CCA_RTS_SH		29
#define AX_TXD_CCA_RTS_MSK		0x3
#define AX_TXD_CTS2SELF		BIT(28)
#define AX_TXD_RTS_EN		BIT(27)
#define AX_TXD_SW_DEFINE_SH		0
#define AX_TXD_SW_DEFINE_MSK		0xf

/* dword13 */
#define AX_TXD_NDPA_DURATION_SH		16
#define AX_TXD_NDPA_DURATION_MSK		0xffff

/* dword14 */
#define AX_TXD_VALID_1		BIT(31)
#define AX_TXD_PCIE_SEQ_NUM_1_SH		16
#define AX_TXD_PCIE_SEQ_NUM_1_MSK		0x7fff
#define AX_TXD_VALID_0		BIT(15)
#define AX_TXD_PCIE_SEQ_NUM_0_SH		0
#define AX_TXD_PCIE_SEQ_NUM_0_MSK		0x7fff

/* dword15 */
#define AX_TXD_VALID_3		BIT(31)
#define AX_TXD_PCIE_SEQ_NUM_3_SH		16
#define AX_TXD_PCIE_SEQ_NUM_3_MSK		0x7fff
#define AX_TXD_VALID_2		BIT(15)
#define AX_TXD_PCIE_SEQ_NUM_2_SH		0
#define AX_TXD_PCIE_SEQ_NUM_2_MSK		0x7fff

#endif

#if MAC_AX_1115E_SUPPORT
/* dword0 */
#define BE_TXD_EN_HWSEQ_MODE_SH		0
#define BE_TXD_EN_HWSEQ_MODE_MSK		0x3
#define BE_TXD_HW_SSN_SEL_SH		2
#define BE_TXD_HW_SSN_SEL_MSK		0x7
#define BE_TXD_HWAMSDU		BIT(5)
#define BE_TXD_HW_SEC_IV		BIT(6)
#define BE_TXD_WD_PAGE		BIT(7)
#define BE_TXD_CHK_EN		BIT(8)
#define BE_TXD_WP_INT		BIT(9)
#define BE_TXD_STF_MODE		BIT(10)
#define BE_TXD_HDR_LLC_LEN_SH		11
#define BE_TXD_HDR_LLC_LEN_MSK		0x1f
#define BE_TXD_CHANNEL_DMA_SH		16
#define BE_TXD_CHANNEL_DMA_MSK		0xf
#define BE_TXD_SMH_EN		BIT(20)
#define BE_TXD_PKT_OFFSET		BIT(21)
#define BE_TXD_WDINFO_EN		BIT(22)
#define BE_TXD_MOREDATA		BIT(23)
#define BE_TXD_WP_OFFSET_SH		24
#define BE_TXD_WP_OFFSET_MSK		0x1f
#define BE_TXD_WD_SOURCE_SH		29
#define BE_TXD_WD_SOURCE_MSK		0x3
#define BE_TXD_HCI_SEQNUM_MODE		BIT(31)

/* dword1 */
#define BE_TXD_DMA_TXAGG_NUM_SH		0
#define BE_TXD_DMA_TXAGG_NUM_MSK		0x7f
#define BE_TXD_REUSE_NUM_SH		7
#define BE_TXD_REUSE_NUM_MSK		0x1f
#define BE_TXD_SEC_TYPE_SH		12
#define BE_TXD_SEC_TYPE_MSK		0xf
#define BE_TXD_SEC_KEYID_SH		16
#define BE_TXD_SEC_KEYID_MSK		0x3
#define BE_TXD_SW_SEC_IV		BIT(18)
#define BE_TXD_REUSE_SIZE_SH		20
#define BE_TXD_REUSE_SIZE_MSK		0xf
#define BE_TXD_REUSE_START_OFFSET_SH		24
#define BE_TXD_REUSE_START_OFFSET_MSK		0x3
#define BE_TXD_ADDR_INFO_NUM_SH		26
#define BE_TXD_ADDR_INFO_NUM_MSK		0x3f

/* dword2 */
#define BE_TXD_TXPKTSIZE_SH		0
#define BE_TXD_TXPKTSIZE_MSK		0x3fff
#define BE_TXD_AGG_EN		BIT(14)
#define BE_TXD_BK		BIT(15)
#define BE_TXD_QSEL_SH		17
#define BE_TXD_QSEL_MSK		0x3f
#define BE_TXD_TID_IND		BIT(23)
#define BE_TXD_MACID_SH		24
#define BE_TXD_MACID_MSK		0xff

/* dword3 */
#define BE_TXD_WIFI_SEQ_SH		0
#define BE_TXD_WIFI_SEQ_MSK		0xfff
#define BE_TXD_MLO_FLAG		BIT(12)
#define BE_TXD_IS_MLD_SW_EN		BIT(13)
#define BE_TXD_TRY_RATE		BIT(14)
#define BE_TXD_SU_TC_SH		16
#define BE_TXD_SU_TC_MSK		0x3f
#define BE_TXD_TOTAL_TC_SH		22
#define BE_TXD_TOTAL_TC_MSK		0x3f
#define BE_TXD_RU_RTY		BIT(28)
#define BE_TXD_MU_PRI_RTY		BIT(29)
#define BE_TXD_MU_2ND_RTY		BIT(30)
#define BE_TXD_TB_SR_RTY		BIT(31)

/* dword4 */
#define BE_TXD_TXDESC_CHECKSUM_SH		0
#define BE_TXD_TXDESC_CHECKSUM_MSK		0xffff
#define BE_TXD_SEC_IV_L_SH		16
#define BE_TXD_SEC_IV_L_MSK		0xffff

/* dword5 */
#define BE_TXD_SEC_IV_H_SH		0
#define BE_TXD_SEC_IV_H_MSK		0xffffffff

/* dword6 */
#define BE_TXD_MU_TC_SH		0
#define BE_TXD_MU_TC_MSK		0x1f
#define BE_TXD_RU_TC_SH		5
#define BE_TXD_RU_TC_MSK		0x1f
#define BE_TXD_CHG_LINK_FLAG		BIT(10)
#define BE_TXD_BMC		BIT(11)
#define BE_TXD_NO_ACK		BIT(12)
#define BE_TXD_UPD_WLAN_HDR		BIT(13)
#define BE_TXD_A4_HDR		BIT(14)
#define BE_TXD_EOSP_BIT		BIT(15)
#define BE_TXD_S_IDX_SH		16
#define BE_TXD_S_IDX_MSK		0xff
#define BE_TXD_RU_POS_SH		24
#define BE_TXD_RU_POS_MSK		0xff

/* dword7 */
#define BE_TXD_RTS_TC_SH		0
#define BE_TXD_RTS_TC_MSK		0x3f
#define BE_TXD_MSDU_NUM_SH		6
#define BE_TXD_MSDU_NUM_MSK		0xf
#define BE_TXD_DATA_ER		BIT(10)
#define BE_TXD_DATA_BW_ER		BIT(11)
#define BE_TXD_DATA_DCM		BIT(12)
#define BE_TXD_GI_LTF_SH		13
#define BE_TXD_GI_LTF_MSK		0x7
#define BE_TXD_DATARATE_SH		16
#define BE_TXD_DATARATE_MSK		0xfff
#define BE_TXD_DATA_BW_SH		28
#define BE_TXD_DATA_BW_MSK		0x7
#define BE_TXD_USERATE_SEL		BIT(31)

/* dword8 */
#define BE_TXD_MBSSID_SH		0
#define BE_TXD_MBSSID_MSK		0xf
#define BE_TXD_MULTIPORT_ID_SH		4
#define BE_TXD_MULTIPORT_ID_MSK		0x7
#define BE_TXD_DISRTSFB		BIT(9)
#define BE_TXD_DISDATAFB		BIT(10)
#define BE_TXD_DATA_LDPC		BIT(11)
#define BE_TXD_DATA_STBC		BIT(12)
#define BE_TXD_BYPASS_PUNC		BIT(13)
#define BE_TXD_DATA_TXCNT_LMT_SH		16
#define BE_TXD_DATA_TXCNT_LMT_MSK		0x3f
#define BE_TXD_DATA_TXCNT_LMT_SEL		BIT(22)
#define BE_TXD_RLS_TO_CPUIO		BIT(30)
#define BE_TXD_ACK_CH_INFO		BIT(31)

/* dword9 */
#define BE_TXD_MAX_AGG_NUM_SH		0
#define BE_TXD_MAX_AGG_NUM_MSK		0xff
#define BE_TXD_BCN_SRCH_SEQ_SH		8
#define BE_TXD_BCN_SRCH_SEQ_MSK		0x3
#define BE_TXD_NAVUSEHDR		BIT(10)
#define BE_TXD_A_CTRL_BQR		BIT(12)
#define BE_TXD_A_CTRL_UPH		BIT(13)
#define BE_TXD_A_CTRL_BSR		BIT(14)
#define BE_TXD_A_CTRL_CAS		BIT(15)
#define BE_TXD_DATA_RTY_LOWEST_RATE_SH		16
#define BE_TXD_DATA_RTY_LOWEST_RATE_MSK		0xfff
#define BE_TXD_SW_DEFINE_SH		28
#define BE_TXD_SW_DEFINE_MSK		0xf

/* dword10 */
#define BE_TXD_SEC_CAM_IDX_SH		0
#define BE_TXD_SEC_CAM_IDX_MSK		0xff
#define BE_TXD_FORCE_KEY_EN		BIT(8)
#define BE_TXD_LIFETIME_SEL_SH		13
#define BE_TXD_LIFETIME_SEL_MSK		0x7
#define BE_TXD_FORCE_TXOP		BIT(17)
#define BE_TXD_AMPDU_DENSITY_SH		18
#define BE_TXD_AMPDU_DENSITY_MSK		0x7
#define BE_TXD_LSIG_TXOP_EN		BIT(21)
#define BE_TXD_TXPWR_OFSET_TYPE_SH		22
#define BE_TXD_TXPWR_OFSET_TYPE_MSK		0x7
#define BE_TXD_OBW_CTS2SELF_DUP_TYPE_SH		26
#define BE_TXD_OBW_CTS2SELF_DUP_TYPE_MSK		0xf

/* dword11 */
#define BE_TXD_NDPA_SH		0
#define BE_TXD_NDPA_MSK		0x7
#define BE_TXD_SND_PKT_SEL_SH		3
#define BE_TXD_SND_PKT_SEL_MSK		0x7
#define BE_TXD_SIFS_TX		BIT(6)
#define BE_TXD_HT_DATA_SND		BIT(7)
#define BE_TXD_CQI_SND		BIT(8)
#define BE_TXD_RTT_EN		BIT(9)
#define BE_TXD_SPE_RPT		BIT(10)
#define BE_TXD_BT_NULL		BIT(11)
#define BE_TXD_TRI_FRAME		BIT(12)
#define BE_TXD_NULL_0		BIT(13)
#define BE_TXD_NULL_1		BIT(14)
#define BE_TXD_RAW		BIT(15)
#define BE_TXD_GROUP_BIT_IE_OFFSET_SH		16
#define BE_TXD_GROUP_BIT_IE_OFFSET_MSK		0xff
#define BE_TXD_SIGNALING_TA_PKT_EN		BIT(25)
#define BE_TXD_BCNPKT_TSF_CTRL		BIT(26)
#define BE_TXD_SIGNALING_TA_PKT_SC_SH		27
#define BE_TXD_SIGNALING_TA_PKT_SC_MSK		0xf
#define BE_TXD_FORCE_BSS_CLR		BIT(31)

/* dword12 */
#define BE_TXD_PUNCTURE_PATTERN_SH		0
#define BE_TXD_PUNCTURE_PATTERN_MSK		0x1ffff
#define BE_TXD_RTS_EN		BIT(27)
#define BE_TXD_CTS2SELF		BIT(28)
#define BE_TXD_CCA_RTS_SH		29
#define BE_TXD_CCA_RTS_MSK		0x3
#define BE_TXD_HW_RTS_EN		BIT(31)

/* dword13 */
#define BE_TXD_NDPA_DURATION_SH		16
#define BE_TXD_NDPA_DURATION_MSK		0xffff

#endif

#endif

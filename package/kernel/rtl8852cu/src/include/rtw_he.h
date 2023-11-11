/******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation.
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
#ifndef _RTW_HE_H_
#define _RTW_HE_H_

/* Set HE MAC Capabilities Information */
#define SET_HE_MAC_CAP_HTC_HE_SUPPORT(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE(_pEleStart, 0, 1, _val)
#define SET_HE_MAC_CAP_TWT_REQUESTER_SUPPORT(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE(_pEleStart, 1, 1, _val)
#define SET_HE_MAC_CAP_TWT_RESPONDER_SUPPORT(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE(_pEleStart, 2, 1, _val)
#define SET_HE_MAC_CAP_DYNAMIC_FRAG_SUPPORT(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE(_pEleStart, 3, 2, _val)
#define SET_HE_MAC_CAP_MAX_FRAG_MSDU_EXP(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE(_pEleStart, 5, 3, _val)

#define SET_HE_MAC_CAP_MIN_FRAG_SIZE(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 1, 0, 2, _val)
#define SET_HE_MAC_CAP_TRI_FRAME_PADDING_DUR(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 1, 2, 2, _val)
#define SET_HE_MAC_CAP_MULTI_TID_AGG_RX_SUPPORT(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 1, 4, 3, _val)
#define SET_HE_MAC_CAP_LINK_ADAPT_SUPPORT(_pEleStart, _val) \
	SET_BITS_TO_LE_2BYTE((_pEleStart) + 1, 7, 2, _val)

#define SET_HE_MAC_CAP_ALL_ACK_SUPPORT(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 2, 1, 1, _val)
#define SET_HE_MAC_CAP_TRS_SUPPORT(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 2, 2, 1, _val)
#define SET_HE_MAC_CAP_BRS_SUPPORT(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 2, 3, 1, _val)
#define SET_HE_MAC_CAP_BC_TWT_SUPPORT(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 2, 4, 1, _val)
#define SET_HE_MAC_CAP_32_BIT_BMP_SUPPORT(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 2, 5, 1, _val)
#define SET_HE_MAC_CAP_MU_CASCADE_SUPPORT(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 2, 6, 1, _val)
#define SET_HE_MAC_CAP_ACK_ENABLED_AGG_SUPPORT(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 2, 7, 1, _val)

#define SET_HE_MAC_CAP_OM_CTRL_SUPPORT(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 3, 1, 1, _val)
#define SET_HE_MAC_CAP_OFDMA_RA_SUPPORT(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 3, 2, 1, _val)
#define SET_HE_MAC_CAP_MAX_AMPDU_LEN_EXP_EXT(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 3, 3, 2, _val)
#define SET_HE_MAC_CAP_AMSDU_FRAG_SUPPORT(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 3, 5, 1, _val)
#define SET_HE_MAC_CAP_FLEX_TWT_SCHED_SUPPORT(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 3, 6, 1, _val)
#define SET_HE_MAC_CAP_RX_CTRL_FRAME_TO_MULTI_BSS(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 3, 7, 1, _val)

#define SET_HE_MAC_CAP_BSRP_BQRP_AMPDU_AGG(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 4, 0, 1, _val)
#define SET_HE_MAC_CAP_QTP_SUPPORT(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 4, 1, 1, _val)
#define SET_HE_MAC_CAP_BQR_SUPPORT(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 4, 2, 1, _val)
#define SET_HE_MAC_CAP_PSR_RESPONDER(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 4, 3, 1, _val)
#define SET_HE_MAC_CAP_NDP_FEEDBACK_RPT_SUPPORT(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 4, 4, 1, _val)
#define SET_HE_MAC_CAP_OPS_SUPPORT(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 4, 5, 1, _val)
#define SET_HE_MAC_CAP_AMSDU_NOT_UNDER_BA_IN_ACK_EN_AMPDU(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 4, 6, 1, _val)
#define SET_HE_MAC_CAP_MULTI_AID_AGG_TX_SUPPORT(_pEleStart, _val) \
	SET_BITS_TO_LE_2BYTE((_pEleStart) + 4, 7, 3, _val)

#define SET_HE_MAC_CAP_HE_SUB_CH_SELECTIVE_TX(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 5, 2, 1, _val)
#define SET_HE_MAC_CAP_UL_2_996_TONE_RU_SUPPORT(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 5, 3, 1, _val)
#define SET_HE_MAC_CAP_OM_CTRL_UL_MU_DATA_DISABLE_RX(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 5, 4, 1, _val)
#define SET_HE_MAC_CAP_HE_DYNAMIC_SM_POWER_SAVE(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 5, 5, 1, _val)
#define SET_HE_MAC_CAP_PUNCTURED_SND_SUPPORT(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 5, 6, 1, _val)
#define SET_HE_MAC_CAP_HT_VHT_TRIG_FRAME_RX(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 5, 7, 1, _val)

/* Set HE PHY Capabilities Information */
#define SET_HE_PHY_CAP_SUPPORT_CHAN_WIDTH_SET(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE(_pEleStart, 1, 7, _val)

#define SET_HE_PHY_CAP_PUNCTURED_PREAMBLE_RX(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 1, 0, 4, _val)
#define SET_HE_PHY_CAP_DEVICE_CLASS(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 1, 4, 1, _val)
#define SET_HE_PHY_CAP_LDPC_IN_PAYLOAD(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 1, 5, 1, _val)
#define SET_HE_PHY_CAP_SU_PPDU_1X_LTF_0_POINT_8_GI(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 1, 6, 1, _val)
#define SET_HE_PHY_CAP_MIDAMBLE_TRX_MAX_NSTS(_pEleStart, _val) \
	SET_BITS_TO_LE_2BYTE((_pEleStart) + 1, 7, 2, _val)

#define SET_HE_PHY_CAP_NDP_4X_LTF_3_POINT_2_GI(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 2, 1, 1, _val)
#define SET_HE_PHY_CAP_STBC_TX_LESS_THAN_80MHZ(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 2, 2, 1, _val)
#define SET_HE_PHY_CAP_STBC_RX_LESS_THAN_80MHZ(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 2, 3, 1, _val)
#define SET_HE_PHY_CAP_DOPPLER_TX(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 2, 4, 1, _val)
#define SET_HE_PHY_CAP_DOPPLER_RX(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 2, 5, 1, _val)
#define SET_HE_PHY_CAP_FULL_BW_UL_MUMIMO(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 2, 6, 1, _val)
#define SET_HE_PHY_CAP_PARTIAL_BW_UL_MUMIMO(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 2, 7, 1, _val)

#define SET_HE_PHY_CAP_DCM_MAX_CONSTELLATION_TX(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 3, 0, 2, _val)
#define SET_HE_PHY_CAP_DCM_MAX_NSS_TX(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 3, 2, 1, _val)
#define SET_HE_PHY_CAP_DCM_MAX_CONSTELLATION_RX(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 3, 3, 2, _val)
#define SET_HE_PHY_CAP_DCM_MAX_NSS_RX(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 3, 5, 1, _val)
#define SET_HE_PHY_CAP_RX_PARTIAL_BW_SU_IN_20MHZ_MUPPDU(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 3, 6, 1, _val)
#define SET_HE_PHY_CAP_SU_BFER(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 3, 7, 1, _val)

#define SET_HE_PHY_CAP_SU_BFEE(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 4, 0, 1, _val)
#define SET_HE_PHY_CAP_MU_BFER(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 4, 1, 1, _val)
#define SET_HE_PHY_CAP_BFEE_STS_LESS_THAN_80MHZ(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 4, 2, 3, _val)
#define SET_HE_PHY_CAP_BFEE_STS_GREATER_THAN_80MHZ(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 4, 5, 3, _val)

#define SET_HE_PHY_CAP_NUM_SND_DIMEN_LESS_THAN_80MHZ(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 5, 0, 3, _val)
#define SET_HE_PHY_CAP_NUM_SND_DIMEN_GREATER_THAN_80MHZ(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 5, 3, 3, _val)
#define SET_HE_PHY_CAP_NG_16_SU_FEEDBACK(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 5, 6, 1, _val)
#define SET_HE_PHY_CAP_NG_16_MU_FEEDBACK(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 5, 7, 1, _val)

#define SET_HE_PHY_CAP_CODEBOOK_4_2_SU_FEEDBACK(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 6, 0, 1, _val)
#define SET_HE_PHY_CAP_CODEBOOK_7_5_MU_FEEDBACK(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 6, 1, 1, _val)
#define SET_HE_PHY_CAP_TRIG_SUBF_FEEDBACK(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 6, 2, 1, _val)
#define SET_HE_PHY_CAP_TRIG_MUBF_PARTIAL_BW_FEEDBACK(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 6, 3, 1, _val)
#define SET_HE_PHY_CAP_TRIG_CQI_FEEDBACK(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 6, 4, 1, _val)
#define SET_HE_PHY_CAP_PARTIAL_BW_EXT_RANGE(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 6, 5, 1, _val)
#define SET_HE_PHY_CAP_PARTIAL_BW_DL_MU_MIMO(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 6, 6, 1, _val)
#define SET_HE_PHY_CAP_PPE_THRESHOLD_PRESENT(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 6, 7, 1, _val)

#define SET_HE_PHY_CAP_PSR_BASED_SR_SUPPORT(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 7, 0, 1, _val)
#define SET_HE_PHY_CAP_PWR_BOOST_FACTOR_SUPPORT(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 7, 1, 1, _val)
#define SET_HE_PHY_CAP_SU_MU_PPDU_4X_LTF_0_POINT_8_GI(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 7, 2, 1, _val)
#define SET_HE_PHY_CAP_MAX_NC(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 7, 3, 3, _val)
#define SET_HE_PHY_CAP_STBC_TX_GREATER_THAN_80MHZ(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 7, 6, 1, _val)
#define SET_HE_PHY_CAP_STBC_RX_GREATER_THAN_80MHZ(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 7, 7, 1, _val)

#define SET_HE_PHY_CAP_ERSU_PPDU_4X_LTF_0_POINT_8_GI(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 8, 0, 1, _val)
#define SET_HE_PHY_CAP_20M_IN_40M_HE_PPDU_IN_2G4(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 8, 1, 1, _val)
#define SET_HE_PHY_CAP_20M_IN_160C_160NC_HE_PPDU(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 8, 2, 1, _val)
#define SET_HE_PHY_CAP_80M_IN_160C_160NC_HE_PPDU(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 8, 3, 1, _val)
#define SET_HE_PHY_CAP_ERSU_PPDU_1X_LTF_0_POINT_8_GI(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 8, 4, 1, _val)
#define SET_HE_PHY_CAP_MIDAMBLE_TRX_2X_1X_LTF(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 8, 5, 1, _val)
#define SET_HE_PHY_CAP_DCM_MAX_RU(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 8, 6, 2, _val)

#define SET_HE_PHY_CAP_LONGER_THAN_16_HESIGB_OFDM_SYM(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 9, 0, 1, _val)
#define SET_HE_PHY_CAP_NON_TRIGGER_CQI_FEEDBACK(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 9, 1, 1, _val)
#define SET_HE_PHY_CAP_TX_1024_QAM_LESS_THAN_242_TONE_RU(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 9, 2, 1, _val)
#define SET_HE_PHY_CAP_RX_1024_QAM_LESS_THAN_242_TONE_RU(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 9, 3, 1, _val)
#define SET_HE_PHY_CAP_RX_FULLBW_SU_USE_MUPPDU_CMP_SIGB(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 9, 4, 1, _val)
#define SET_HE_PHY_CAP_RX_FULLBW_SU_USE_MUPPDU_NONCMP_SIGB(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 9, 5, 1, _val)
#define SET_HE_PHY_CAP_NOMINAL_PACKET_PADDING(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 9, 6, 2, _val)

/* Set Supported HE-MCS And NSS Set Information */
#define SET_HE_CAP_MCS_1SS(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE(_pEleStart, 0, 2, _val)
#define SET_HE_CAP_MCS_2SS(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE(_pEleStart, 2, 2, _val)
#define SET_HE_CAP_MCS_3SS(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE(_pEleStart, 4, 2, _val)
#define SET_HE_CAP_MCS_4SS(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE(_pEleStart, 6, 2, _val)
#define SET_HE_CAP_MCS_5SS(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 1, 0, 2, _val)
#define SET_HE_CAP_MCS_6SS(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 1, 2, 2, _val)
#define SET_HE_CAP_MCS_7SS(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 1, 4, 2, _val)
#define SET_HE_CAP_MCS_8SS(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 1, 6, 2, _val)

#define SET_HE_CAP_RX_MCS_LESS_THAN_80MHZ_1SS(_pEleStart, _val) \
	SET_HE_CAP_MCS_1SS(_pEleStart, _val)
#define SET_HE_CAP_RX_MCS_LESS_THAN_80MHZ_2SS(_pEleStart, _val) \
	SET_HE_CAP_MCS_2SS(_pEleStart, _val)
#define SET_HE_CAP_RX_MCS_LESS_THAN_80MHZ_3SS(_pEleStart, _val) \
	SET_HE_CAP_MCS_3SS(_pEleStart, _val)
#define SET_HE_CAP_RX_MCS_LESS_THAN_80MHZ_4SS(_pEleStart, _val) \
	SET_HE_CAP_MCS_4SS(_pEleStart, _val)
#define SET_HE_CAP_RX_MCS_LESS_THAN_80MHZ_5SS(_pEleStart, _val) \
	SET_HE_CAP_MCS_5SS(_pEleStart, _val)
#define SET_HE_CAP_RX_MCS_LESS_THAN_80MHZ_6SS(_pEleStart, _val) \
	SET_HE_CAP_MCS_6SS(_pEleStart, _val)
#define SET_HE_CAP_RX_MCS_LESS_THAN_80MHZ_7SS(_pEleStart, _val) \
	SET_HE_CAP_MCS_7SS(_pEleStart, _val)
#define SET_HE_CAP_RX_MCS_LESS_THAN_80MHZ_8SS(_pEleStart, _val) \
	SET_HE_CAP_MCS_8SS(_pEleStart, _val)

#define SET_HE_CAP_TX_MCS_LESS_THAN_80MHZ_1SS(_pEleStart, _val) \
	SET_HE_CAP_MCS_1SS(_pEleStart + 2, _val)
#define SET_HE_CAP_TX_MCS_LESS_THAN_80MHZ_2SS(_pEleStart, _val) \
	SET_HE_CAP_MCS_2SS(_pEleStart + 2, _val)
#define SET_HE_CAP_TX_MCS_LESS_THAN_80MHZ_3SS(_pEleStart, _val) \
	SET_HE_CAP_MCS_3SS(_pEleStart + 2, _val)
#define SET_HE_CAP_TX_MCS_LESS_THAN_80MHZ_4SS(_pEleStart, _val) \
	SET_HE_CAP_MCS_4SS(_pEleStart + 2, _val)
#define SET_HE_CAP_TX_MCS_LESS_THAN_80MHZ_5SS(_pEleStart, _val) \
	SET_HE_CAP_MCS_5SS(_pEleStart + 2, _val)
#define SET_HE_CAP_TX_MCS_LESS_THAN_80MHZ_6SS(_pEleStart, _val) \
	SET_HE_CAP_MCS_6SS(_pEleStart + 2, _val)
#define SET_HE_CAP_TX_MCS_LESS_THAN_80MHZ_7SS(_pEleStart, _val) \
	SET_HE_CAP_MCS_7SS(_pEleStart + 2, _val)
#define SET_HE_CAP_TX_MCS_LESS_THAN_80MHZ_8SS(_pEleStart, _val) \
	SET_HE_CAP_MCS_8SS(_pEleStart + 2, _val)

#define SET_HE_CAP_RX_MCS_160MHZ_1SS(_pEleStart, _val) \
	SET_HE_CAP_MCS_1SS(_pEleStart + 4, _val)
#define SET_HE_CAP_RX_MCS_160MHZ_2SS(_pEleStart, _val) \
	SET_HE_CAP_MCS_2SS(_pEleStart + 4, _val)
#define SET_HE_CAP_RX_MCS_160MHZ_3SS(_pEleStart, _val) \
	SET_HE_CAP_MCS_3SS(_pEleStart + 4, _val)
#define SET_HE_CAP_RX_MCS_160MHZ_4SS(_pEleStart, _val) \
	SET_HE_CAP_MCS_4SS(_pEleStart + 4, _val)
#define SET_HE_CAP_RX_MCS_160MHZ_5SS(_pEleStart, _val) \
	SET_HE_CAP_MCS_5SS(_pEleStart + 4, _val)
#define SET_HE_CAP_RX_MCS_160MHZ_6SS(_pEleStart, _val) \
	SET_HE_CAP_MCS_6SS(_pEleStart + 4, _val)
#define SET_HE_CAP_RX_MCS_160MHZ_7SS(_pEleStart, _val) \
	SET_HE_CAP_MCS_7SS(_pEleStart + 4, _val)
#define SET_HE_CAP_RX_MCS_160MHZ_8SS(_pEleStart, _val) \
	SET_HE_CAP_MCS_8SS(_pEleStart + 4, _val)

#define SET_HE_CAP_TX_MCS_160MHZ_1SS(_pEleStart, _val) \
	SET_HE_CAP_MCS_1SS(_pEleStart + 6, _val)
#define SET_HE_CAP_TX_MCS_160MHZ_2SS(_pEleStart, _val) \
	SET_HE_CAP_MCS_2SS(_pEleStart + 6, _val)
#define SET_HE_CAP_TX_MCS_160MHZ_3SS(_pEleStart, _val) \
	SET_HE_CAP_MCS_3SS(_pEleStart + 6, _val)
#define SET_HE_CAP_TX_MCS_160MHZ_4SS(_pEleStart, _val) \
	SET_HE_CAP_MCS_4SS(_pEleStart + 6, _val)
#define SET_HE_CAP_TX_MCS_160MHZ_5SS(_pEleStart, _val) \
	SET_HE_CAP_MCS_5SS(_pEleStart + 6, _val)
#define SET_HE_CAP_TX_MCS_160MHZ_6SS(_pEleStart, _val) \
	SET_HE_CAP_MCS_6SS(_pEleStart + 6, _val)
#define SET_HE_CAP_TX_MCS_160MHZ_7SS(_pEleStart, _val) \
	SET_HE_CAP_MCS_7SS(_pEleStart + 6, _val)
#define SET_HE_CAP_TX_MCS_160MHZ_8SS(_pEleStart, _val) \
	SET_HE_CAP_MCS_8SS(_pEleStart + 6, _val)

#define SET_HE_CAP_RX_MCS_80_80MHZ_1SS(_pEleStart, _val) \
	SET_HE_CAP_MCS_1SS(_pEleStart + 8, _val)
#define SET_HE_CAP_RX_MCS_80_80MHZ_2SS(_pEleStart, _val) \
	SET_HE_CAP_MCS_2SS(_pEleStart + 8, _val)
#define SET_HE_CAP_RX_MCS_80_80MHZ_3SS(_pEleStart, _val) \
	SET_HE_CAP_MCS_3SS(_pEleStart + 8, _val)
#define SET_HE_CAP_RX_MCS_80_80MHZ_4SS(_pEleStart, _val) \
	SET_HE_CAP_MCS_4SS(_pEleStart + 8, _val)
#define SET_HE_CAP_RX_MCS_80_80MHZ_5SS(_pEleStart, _val) \
	SET_HE_CAP_MCS_5SS(_pEleStart + 8, _val)
#define SET_HE_CAP_RX_MCS_80_80MHZ_6SS(_pEleStart, _val) \
	SET_HE_CAP_MCS_6SS(_pEleStart + 8, _val)
#define SET_HE_CAP_RX_MCS_80_80MHZ_7SS(_pEleStart, _val) \
	SET_HE_CAP_MCS_7SS(_pEleStart + 8, _val)
#define SET_HE_CAP_RX_MCS_80_80MHZ_8SS(_pEleStart, _val) \
	SET_HE_CAP_MCS_8SS(_pEleStart + 8, _val)

#define SET_HE_CAP_TX_MCS_80_80MHZ_1SS(_pEleStart, _val) \
	SET_HE_CAP_MCS_1SS(_pEleStart + 10, _val)
#define SET_HE_CAP_TX_MCS_80_80MHZ_2SS(_pEleStart, _val) \
	SET_HE_CAP_MCS_2SS(_pEleStart + 10, _val)
#define SET_HE_CAP_TX_MCS_80_80MHZ_3SS(_pEleStart, _val) \
	SET_HE_CAP_MCS_3SS(_pEleStart + 10, _val)
#define SET_HE_CAP_TX_MCS_80_80MHZ_4SS(_pEleStart, _val) \
	SET_HE_CAP_MCS_4SS(_pEleStart + 10, _val)
#define SET_HE_CAP_TX_MCS_80_80MHZ_5SS(_pEleStart, _val) \
	SET_HE_CAP_MCS_5SS(_pEleStart + 10, _val)
#define SET_HE_CAP_TX_MCS_80_80MHZ_6SS(_pEleStart, _val) \
	SET_HE_CAP_MCS_6SS(_pEleStart + 10, _val)
#define SET_HE_CAP_TX_MCS_80_80MHZ_7SS(_pEleStart, _val) \
	SET_HE_CAP_MCS_7SS(_pEleStart + 10, _val)
#define SET_HE_CAP_TX_MCS_80_80MHZ_8SS(_pEleStart, _val) \
	SET_HE_CAP_MCS_8SS(_pEleStart + 10, _val)

/* Set PPE Threshold */
#define SET_HE_CAP_PPE_NSTS(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE(_pEleStart, 0, 3, _val)
#define SET_HE_CAP_PPE_PU_IDX_BITMASK(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE(_pEleStart, 3, 4, _val)

/* Get HE MAC Capabilities Information */
#define GET_HE_MAC_CAP_HTC_HE_SUPPORT(_pEleStart) \
	LE_BITS_TO_1BYTE(_pEleStart, 0, 1)
#define GET_HE_MAC_CAP_TWT_REQUESTER_SUPPORT(_pEleStart) \
	LE_BITS_TO_1BYTE(_pEleStart, 1, 1)
#define GET_HE_MAC_CAP_TWT_RESPONDER_SUPPORT(_pEleStart) \
	LE_BITS_TO_1BYTE(_pEleStart, 2, 1)
#define GET_HE_MAC_CAP_DYNAMIC_FRAG_SUPPORT(_pEleStart) \
	LE_BITS_TO_1BYTE(_pEleStart, 3, 2)
#define GET_HE_MAC_CAP_MAX_FRAG_MSDU_EXP(_pEleStart) \
	LE_BITS_TO_1BYTE(_pEleStart, 5, 3)

#define GET_HE_MAC_CAP_MIN_FRAG_SIZE(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 1, 0, 2)
#define GET_HE_MAC_CAP_TRI_FRAME_PADDING_DUR(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 1, 2, 2)
#define GET_HE_MAC_CAP_MULTI_TID_AGG_RX_SUPPORT(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 1, 4, 3)
#define GET_HE_MAC_CAP_LINK_ADAPT_SUPPORT(_pEleStart) \
	LE_BITS_TO_2BYTE((_pEleStart) + 1, 7, 2)

#define GET_HE_MAC_CAP_ALL_ACK_SUPPORT(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 2, 1, 1)
#define GET_HE_MAC_CAP_TRS_SUPPORT(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 2, 2, 1)
#define GET_HE_MAC_CAP_BRS_SUPPORT(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 2, 3, 1)
#define GET_HE_MAC_CAP_BC_TWT_SUPPORT(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 2, 4, 1)
#define GET_HE_MAC_CAP_32_BIT_BMP_SUPPORT(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 2, 5, 1)
#define GET_HE_MAC_CAP_MU_CASCADE_SUPPORT(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 2, 6, 1)
#define GET_HE_MAC_CAP_ACK_ENABLED_AGG_SUPPORT(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 2, 7, 1)

#define GET_HE_MAC_CAP_OM_CTRL_SUPPORT(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 3, 1, 1)
#define GET_HE_MAC_CAP_OFDMA_RA_SUPPORT(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 3, 2, 1)
#define GET_HE_MAC_CAP_MAX_AMPDU_LEN_EXP_EXT(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 3, 3, 2)
#define GET_HE_MAC_CAP_AMSDU_FRAG_SUPPORT(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 3, 5, 1)
#define GET_HE_MAC_CAP_FLEX_TWT_SCHED_SUPPORT(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 3, 6, 1)
#define GET_HE_MAC_CAP_RX_CTRL_FRAME_TO_MULTI_BSS(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 3, 7, 1)

#define GET_HE_MAC_CAP_BSRP_BQRP_AMPDU_AGG(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 4, 0, 1)
#define GET_HE_MAC_CAP_QTP_SUPPORT(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 4, 1, 1)
#define GET_HE_MAC_CAP_BQR_SUPPORT(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 4, 2, 1)
#define GET_HE_MAC_CAP_PSR_RESPONDER(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 4, 3, 1)
#define GET_HE_MAC_CAP_NDP_FEEDBACK_RPT_SUPPORT(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 4, 4, 1)
#define GET_HE_MAC_CAP_OPS_SUPPORT(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 4, 5, 1)
#define GET_HE_MAC_CAP_AMSDU_NOT_UNDER_BA_IN_ACK_EN_AMPDU(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 4, 6, 1)
#define GET_HE_MAC_CAP_MULTI_AID_AGG_TX_SUPPORT(_pEleStart) \
	LE_BITS_TO_2BYTE((_pEleStart) + 4, 7, 3)

#define GET_HE_MAC_CAP_HE_SUB_CH_SELECTIVE_TX(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 5, 2, 1)
#define GET_HE_MAC_CAP_UL_2_996_TONE_RU_SUPPORT(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 5, 3, 1)
#define GET_HE_MAC_CAP_OM_CTRL_UL_MU_DATA_DISABLE_RX(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 5, 4, 1)
#define GET_HE_MAC_CAP_HE_DYNAMIC_SM_POWER_SAVE(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 5, 5, 1)
#define GET_HE_MAC_CAP_PUNCTURED_SND_SUPPORT(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 5, 6, 1)
#define GET_HE_MAC_CAP_HT_VHT_TRIG_FRAME_RX(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 5, 7, 1)

/* Get HE PHY Capabilities Information */
#define GET_HE_PHY_CAP_SUPPORT_CHAN_WIDTH_SET(_pEleStart) \
	LE_BITS_TO_1BYTE(_pEleStart, 1, 7)

#define GET_HE_PHY_CAP_PUNCTURED_PREAMBLE_RX(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 1, 0, 4)
#define GET_HE_PHY_CAP_DEVICE_CLASS(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 1, 4, 1)
#define GET_HE_PHY_CAP_LDPC_IN_PAYLOAD(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 1, 5, 1)
#define GET_HE_PHY_CAP_SU_PPDU_1X_LTF_0_POINT_8_GI(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 1, 6, 1)
#define GET_HE_PHY_CAP_MIDAMBLE_TRX_MAX_NSTS(_pEleStart) \
	LE_BITS_TO_2BYTE((_pEleStart) + 1, 7, 2)

#define GET_HE_PHY_CAP_NDP_4X_LTF_3_POINT_2_GI(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 2, 1, 1)
#define GET_HE_PHY_CAP_STBC_TX_LESS_THAN_80MHZ(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 2, 2, 1)
#define GET_HE_PHY_CAP_STBC_RX_LESS_THAN_80MHZ(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 2, 3, 1)
#define GET_HE_PHY_CAP_DOPPLER_TX(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 2, 4, 1)
#define GET_HE_PHY_CAP_DOPPLER_RX(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 2, 5, 1)
#define GET_HE_PHY_CAP_FULL_BW_UL_MUMIMO(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 2, 6, 1)
#define GET_HE_PHY_CAP_PARTIAL_BW_UL_MUMIMO(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 2, 7, 1)

#define GET_HE_PHY_CAP_DCM_MAX_CONSTELLATION_TX(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 3, 0, 2)
#define GET_HE_PHY_CAP_DCM_MAX_NSS_TX(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 3, 2, 1)
#define GET_HE_PHY_CAP_DCM_MAX_CONSTELLATION_RX(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 3, 3, 2)
#define GET_HE_PHY_CAP_DCM_MAX_NSS_RX(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 3, 5, 1)
#define GET_HE_PHY_CAP_RX_PARTIAL_BW_SU_IN_20MHZ_MUPPDU(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 3, 6, 1)
#define GET_HE_PHY_CAP_SU_BFER(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 3, 7, 1)

#define GET_HE_PHY_CAP_SU_BFEE(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 4, 0, 1)
#define GET_HE_PHY_CAP_MU_BFER(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 4, 1, 1)
#define GET_HE_PHY_CAP_BFEE_STS_LESS_THAN_80MHZ(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 4, 2, 3)
#define GET_HE_PHY_CAP_BFEE_STS_GREATER_THAN_80MHZ(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 4, 5, 3)

#define GET_HE_PHY_CAP_NUM_SND_DIMEN_LESS_THAN_80MHZ(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 5, 0, 3)
#define GET_HE_PHY_CAP_NUM_SND_DIMEN_GREATER_THAN_80MHZ(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 5, 3, 3)
#define GET_HE_PHY_CAP_NG_16_SU_FEEDBACK(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 5, 6, 1)
#define GET_HE_PHY_CAP_NG_16_MU_FEEDBACK(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 5, 7, 1)

#define GET_HE_PHY_CAP_CODEBOOK_4_2_SU_FEEDBACK(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 6, 0, 1)
#define GET_HE_PHY_CAP_CODEBOOK_7_5_MU_FEEDBACK(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 6, 1, 1)
#define GET_HE_PHY_CAP_TRIG_SUBF_FEEDBACK(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 6, 2, 1)
#define GET_HE_PHY_CAP_TRIG_MUBF_PARTIAL_BW_FEEDBACK(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 6, 3, 1)
#define GET_HE_PHY_CAP_TRIG_CQI_FEEDBACK(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 6, 4, 1)
#define GET_HE_PHY_CAP_PARTIAL_BW_EXT_RANGE(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 6, 5, 1)
#define GET_HE_PHY_CAP_PARTIAL_BW_DL_MU_MIMO(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 6, 6, 1)
#define GET_HE_PHY_CAP_PPE_THRESHOLD_PRESENT(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 6, 7, 1)

#define GET_HE_PHY_CAP_PSR_BASED_SR_SUPPORT(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 7, 0, 1)
#define GET_HE_PHY_CAP_PWR_BOOST_FACTOR_SUPPORT(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 7, 1, 1)
#define GET_HE_PHY_CAP_SU_MU_PPDU_4X_LTF_0_POINT_8_GI(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 7, 2, 1)
#define GET_HE_PHY_CAP_MAX_NC(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 7, 3, 3)
#define GET_HE_PHY_CAP_STBC_TX_GREATER_THAN_80MHZ(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 7, 6, 1)
#define GET_HE_PHY_CAP_STBC_RX_GREATER_THAN_80MHZ(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 7, 7, 1)

#define GET_HE_PHY_CAP_ERSU_PPDU_4X_LTF_0_POINT_8_GI(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 8, 0, 1)
#define GET_HE_PHY_CAP_20M_IN_40M_HE_PPDU_IN_2G4(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 8, 1, 1)
#define GET_HE_PHY_CAP_20M_IN_160C_160NC_HE_PPDU(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 8, 2, 1)
#define GET_HE_PHY_CAP_80M_IN_160C_160NC_HE_PPDU(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 8, 3, 1)
#define GET_HE_PHY_CAP_ERSU_PPDU_1X_LTF_0_POINT_8_GI(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 8, 4, 1)
#define GET_HE_PHY_CAP_MIDAMBLE_TRX_2X_1X_LTF(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 8, 5, 1)
#define GET_HE_PHY_CAP_DCM_MAX_RU(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 8, 6, 2)

#define GET_HE_PHY_CAP_LONGER_THAN_16_HESIGB_OFDM_SYM(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 9, 0, 1)
#define GET_HE_PHY_CAP_NON_TRIGGER_CQI_FEEDBACK(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 9, 1, 1)
#define GET_HE_PHY_CAP_TX_1024_QAM_LESS_THAN_242_TONE_RU(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 9, 2, 1)
#define GET_HE_PHY_CAP_RX_1024_QAM_LESS_THAN_242_TONE_RU(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 9, 3, 1)
#define GET_HE_PHY_CAP_RX_FULLBW_SU_USE_MUPPDU_CMP_SIGB(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 9, 4, 1)
#define GET_HE_PHY_CAP_RX_FULLBW_SU_USE_MUPPDU_NONCMP_SIGB(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 9, 5, 1)
#define GET_HE_PHY_CAP_NOMINAL_PACKET_PADDING(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 9, 6, 2)

/* Get Supported HE-MCS And NSS Set Information */
#define GET_HE_CAP_MCS_1SS(_pEleStart) \
	LE_BITS_TO_1BYTE(_pEleStart, 0, 2)
#define GET_HE_CAP_MCS_2SS(_pEleStart) \
	LE_BITS_TO_1BYTE(_pEleStart, 2, 2)
#define GET_HE_CAP_MCS_3SS(_pEleStart) \
	LE_BITS_TO_1BYTE(_pEleStart, 4, 2)
#define GET_HE_CAP_MCS_4SS(_pEleStart) \
	LE_BITS_TO_1BYTE(_pEleStart, 6, 2)
#define GET_HE_CAP_MCS_5SS(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 1, 0, 2)
#define GET_HE_CAP_MCS_6SS(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 1, 2, 2)
#define GET_HE_CAP_MCS_7SS(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 1, 4, 2)
#define GET_HE_CAP_MCS_8SS(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 1, 6, 2)

#define GET_HE_CAP_RX_MCS_LESS_THAN_80MHZ_1SS(_pEleStart) \
	GET_HE_CAP_MCS_1SS(_pEleStart)
#define GET_HE_CAP_RX_MCS_LESS_THAN_80MHZ_2SS(_pEleStart) \
	GET_HE_CAP_MCS_2SS(_pEleStart)
#define GET_HE_CAP_RX_MCS_LESS_THAN_80MHZ_3SS(_pEleStart) \
	GET_HE_CAP_MCS_3SS(_pEleStart)
#define GET_HE_CAP_RX_MCS_LESS_THAN_80MHZ_4SS(_pEleStart) \
	GET_HE_CAP_MCS_4SS(_pEleStart)
#define GET_HE_CAP_RX_MCS_LESS_THAN_80MHZ_5SS(_pEleStart) \
	GET_HE_CAP_MCS_5SS(_pEleStart)
#define GET_HE_CAP_RX_MCS_LESS_THAN_80MHZ_6SS(_pEleStart) \
	GET_HE_CAP_MCS_6SS(_pEleStart)
#define GET_HE_CAP_RX_MCS_LESS_THAN_80MHZ_7SS(_pEleStart) \
	GET_HE_CAP_MCS_7SS(_pEleStart)
#define GET_HE_CAP_RX_MCS_LESS_THAN_80MHZ_8SS(_pEleStart) \
	GET_HE_CAP_MCS_8SS(_pEleStart)

#define GET_HE_CAP_TX_MCS_LESS_THAN_80MHZ_1SS(_pEleStart) \
	GET_HE_CAP_MCS_1SS(_pEleStart + 2)
#define GET_HE_CAP_TX_MCS_LESS_THAN_80MHZ_2SS(_pEleStart) \
	GET_HE_CAP_MCS_2SS(_pEleStart + 2)
#define GET_HE_CAP_TX_MCS_LESS_THAN_80MHZ_3SS(_pEleStart) \
	GET_HE_CAP_MCS_3SS(_pEleStart + 2)
#define GET_HE_CAP_TX_MCS_LESS_THAN_80MHZ_4SS(_pEleStart) \
	GET_HE_CAP_MCS_4SS(_pEleStart + 2)
#define GET_HE_CAP_TX_MCS_LESS_THAN_80MHZ_5SS(_pEleStart) \
	GET_HE_CAP_MCS_5SS(_pEleStart + 2)
#define GET_HE_CAP_TX_MCS_LESS_THAN_80MHZ_6SS(_pEleStart) \
	GET_HE_CAP_MCS_6SS(_pEleStart + 2)
#define GET_HE_CAP_TX_MCS_LESS_THAN_80MHZ_7SS(_pEleStart) \
	GET_HE_CAP_MCS_7SS(_pEleStart + 2)
#define GET_HE_CAP_TX_MCS_LESS_THAN_80MHZ_8SS(_pEleStart) \
	GET_HE_CAP_MCS_8SS(_pEleStart + 2)

#define GET_HE_CAP_RX_MCS_160MHZ_1SS(_pEleStart) \
	GET_HE_CAP_MCS_1SS(_pEleStart + 4)
#define GET_HE_CAP_RX_MCS_160MHZ_2SS(_pEleStart) \
	GET_HE_CAP_MCS_2SS(_pEleStart + 4)
#define GET_HE_CAP_RX_MCS_160MHZ_3SS(_pEleStart) \
	GET_HE_CAP_MCS_3SS(_pEleStart + 4)
#define GET_HE_CAP_RX_MCS_160MHZ_4SS(_pEleStart) \
	GET_HE_CAP_MCS_4SS(_pEleStart + 4)
#define GET_HE_CAP_RX_MCS_160MHZ_5SS(_pEleStart) \
	GET_HE_CAP_MCS_5SS(_pEleStart + 4)
#define GET_HE_CAP_RX_MCS_160MHZ_6SS(_pEleStart) \
	GET_HE_CAP_MCS_6SS(_pEleStart + 4)
#define GET_HE_CAP_RX_MCS_160MHZ_7SS(_pEleStart) \
	GET_HE_CAP_MCS_7SS(_pEleStart + 4)
#define GET_HE_CAP_RX_MCS_160MHZ_8SS(_pEleStart) \
	GET_HE_CAP_MCS_8SS(_pEleStart + 4)

#define GET_HE_CAP_TX_MCS_160MHZ_1SS(_pEleStart) \
	GET_HE_CAP_MCS_1SS(_pEleStart + 6)
#define GET_HE_CAP_TX_MCS_160MHZ_2SS(_pEleStart) \
	GET_HE_CAP_MCS_2SS(_pEleStart + 6)
#define GET_HE_CAP_TX_MCS_160MHZ_3SS(_pEleStart) \
	GET_HE_CAP_MCS_3SS(_pEleStart + 6)
#define GET_HE_CAP_TX_MCS_160MHZ_4SS(_pEleStart) \
	GET_HE_CAP_MCS_4SS(_pEleStart + 6)
#define GET_HE_CAP_TX_MCS_160MHZ_5SS(_pEleStart) \
	GET_HE_CAP_MCS_5SS(_pEleStart + 6)
#define GET_HE_CAP_TX_MCS_160MHZ_6SS(_pEleStart) \
	GET_HE_CAP_MCS_6SS(_pEleStart + 6)
#define GET_HE_CAP_TX_MCS_160MHZ_7SS(_pEleStart) \
	GET_HE_CAP_MCS_7SS(_pEleStart + 6)
#define GET_HE_CAP_TX_MCS_160MHZ_8SS(_pEleStart) \
	GET_HE_CAP_MCS_8SS(_pEleStart + 6)

#define GET_HE_CAP_RX_MCS_80_80MHZ_1SS(_pEleStart) \
	GET_HE_CAP_MCS_1SS(_pEleStart + 8)
#define GET_HE_CAP_RX_MCS_80_80MHZ_2SS(_pEleStart) \
	GET_HE_CAP_MCS_2SS(_pEleStart + 8)
#define GET_HE_CAP_RX_MCS_80_80MHZ_3SS(_pEleStart) \
	GET_HE_CAP_MCS_3SS(_pEleStart + 8)
#define GET_HE_CAP_RX_MCS_80_80MHZ_4SS(_pEleStart) \
	GET_HE_CAP_MCS_4SS(_pEleStart + 8)
#define GET_HE_CAP_RX_MCS_80_80MHZ_5SS(_pEleStart) \
	GET_HE_CAP_MCS_5SS(_pEleStart + 8)
#define GET_HE_CAP_RX_MCS_80_80MHZ_6SS(_pEleStart) \
	GET_HE_CAP_MCS_6SS(_pEleStart + 8)
#define GET_HE_CAP_RX_MCS_80_80MHZ_7SS(_pEleStart) \
	GET_HE_CAP_MCS_7SS(_pEleStart + 8)
#define GET_HE_CAP_RX_MCS_80_80MHZ_8SS(_pEleStart) \
	GET_HE_CAP_MCS_8SS(_pEleStart + 8)

#define GET_HE_CAP_TX_MCS_80_80MHZ_1SS(_pEleStart) \
	GET_HE_CAP_MCS_1SS(_pEleStart + 10)
#define GET_HE_CAP_TX_MCS_80_80MHZ_2SS(_pEleStart) \
	GET_HE_CAP_MCS_2SS(_pEleStart + 10)
#define GET_HE_CAP_TX_MCS_80_80MHZ_3SS(_pEleStart) \
	GET_HE_CAP_MCS_3SS(_pEleStart + 10)
#define GET_HE_CAP_TX_MCS_80_80MHZ_4SS(_pEleStart) \
	GET_HE_CAP_MCS_4SS(_pEleStart + 10)
#define GET_HE_CAP_TX_MCS_80_80MHZ_5SS(_pEleStart) \
	GET_HE_CAP_MCS_5SS(_pEleStart + 10)
#define GET_HE_CAP_TX_MCS_80_80MHZ_6SS(_pEleStart) \
	GET_HE_CAP_MCS_6SS(_pEleStart + 10)
#define GET_HE_CAP_TX_MCS_80_80MHZ_7SS(_pEleStart) \
	GET_HE_CAP_MCS_7SS(_pEleStart + 10)
#define GET_HE_CAP_TX_MCS_80_80MHZ_8SS(_pEleStart) \
	GET_HE_CAP_MCS_8SS(_pEleStart + 10)

/* Get PPE Threshold */
#define GET_HE_CAP_PPE_NSTS(_pEleStart) \
	LE_BITS_TO_1BYTE(_pEleStart, 0, 3)
#define GET_HE_CAP_PPE_PU_IDX_BITMASK(_pEleStart) \
	LE_BITS_TO_1BYTE(_pEleStart, 3, 4)

/* Set HE Operation element */
#define SET_HE_OP_PARA_DEFAULT_PE_DUR(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE(_pEleStart, 0, 3, _val)
#define SET_HE_OP_PARA_TWT_REQUIRED(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE(_pEleStart, 3, 1, _val)
#define SET_HE_OP_PARA_TXOP_DUR_RTS_THRESHOLD(_pEleStart, _val) \
	SET_BITS_TO_LE_2BYTE(_pEleStart, 4, 10, _val)

#define SET_HE_OP_PARA_VHT_OP_INFO_PRESENT(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 1, 6, 1, _val)
#define SET_HE_OP_PARA_CO_HOSTED_BSS(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 1, 7, 1, _val)

#define SET_HE_OP_PARA_ER_SU_DISABLE(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 2, 0, 1, _val)
#define SET_HE_OP_PARA_6GHZ_OP_INFO_PRESENT(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 2, 1, 1, _val)

#define SET_HE_OP_BSS_COLOR_INFO_BSS_COLOR(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE(_pEleStart, 0, 6, _val)
#define SET_HE_OP_BSS_COLOR_INFO_PARTIAL_BSS_COLOR(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE(_pEleStart, 6, 1, _val)
#define SET_HE_OP_BSS_COLOR_INFO_BSS_COLOR_DISABLE(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE(_pEleStart, 7, 1, _val)

#define SET_HE_OP_BASIC_MCS_1SS(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE(_pEleStart, 0, 2, _val)
#define SET_HE_OP_BASIC_MCS_2SS(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE(_pEleStart, 2, 2, _val)
#define SET_HE_OP_BASIC_MCS_3SS(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE(_pEleStart, 4, 2, _val)
#define SET_HE_OP_BASIC_MCS_4SS(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE(_pEleStart, 6, 2, _val)
#define SET_HE_OP_BASIC_MCS_5SS(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 1, 0, 2, _val)
#define SET_HE_OP_BASIC_MCS_6SS(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 1, 2, 2, _val)
#define SET_HE_OP_BASIC_MCS_7SS(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 1, 4, 2, _val)
#define SET_HE_OP_BASIC_MCS_8SS(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 1, 6, 2, _val)

#define SET_HE_OP_INFO_PRIMARY_CHAN(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart), 0, 8, _val)
#define SET_HE_OP_INFO_CHAN_WIDTH(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 1, 0, 2, _val)
#define SET_HE_OP_INFO_DUPL_BCN(_pEleStart) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 1, 2, 1, _val)
#define SET_HE_OP_INFO_REGULATORY_INFO(_pEleStart) \
	SET_BITS_TO_LE_1BYTE((_pEleStart) + 1, 3, 3, _val)

#define SET_HE_OP_INFO_CENTER_FREQ_0(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart)+ 2, 0, 8, _val)
#define SET_HE_OP_INFO_CENTER_FREQ_1(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart)+ 3, 0, 8, _val)
#define SET_HE_OP_INFO_MIN_RATE(_pEleStart, _val) \
	SET_BITS_TO_LE_1BYTE((_pEleStart)+ 4, 0, 8, _val)

/* Values in HE spec */
#define TXOP_DUR_RTS_TH_DISABLED	1023

/* Get HE Operation element */
#define GET_HE_OP_PARA_DEFAULT_PE_DUR(_pEleStart) \
	LE_BITS_TO_1BYTE(_pEleStart, 0, 3)
#define GET_HE_OP_PARA_TWT_REQUIRED(_pEleStart) \
	LE_BITS_TO_1BYTE(_pEleStart, 3, 1)
#define GET_HE_OP_PARA_TXOP_DUR_RTS_THRESHOLD(_pEleStart) \
	LE_BITS_TO_2BYTE(_pEleStart, 4, 10)

#define GET_HE_OP_PARA_VHT_OP_INFO_PRESENT(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 1, 6, 1)
#define GET_HE_OP_PARA_CO_HOSTED_BSS(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 1, 7, 1)

#define GET_HE_OP_PARA_ER_SU_DISABLE(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 2, 0, 1)
#define GET_HE_OP_PARA_6GHZ_OP_INFO_PRESENT(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 2, 1, 1)

#define GET_HE_OP_BSS_COLOR_INFO_BSS_COLOR(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 3, 0, 6)
#define GET_HE_OP_BSS_COLOR_INFO_PARTIAL_BSS_COLOR(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 3, 6, 1)
#define GET_HE_OP_BSS_COLOR_INFO_BSS_COLOR_DISABLE(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 3, 7, 1)

#define GET_HE_OP_BASIC_MCS_1SS(_pEleStart) \
	LE_BITS_TO_1BYTE(_pEleStart, 0, 2)
#define GET_HE_OP_BASIC_MCS_2SS(_pEleStart) \
	LE_BITS_TO_1BYTE(_pEleStart, 2, 2)
#define GET_HE_OP_BASIC_MCS_3SS(_pEleStart) \
	LE_BITS_TO_1BYTE(_pEleStart, 4, 2)
#define GET_HE_OP_BASIC_MCS_4SS(_pEleStart) \
	LE_BITS_TO_1BYTE(_pEleStart, 6, 2)
#define GET_HE_OP_BASIC_MCS_5SS(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 1, 0, 2)
#define GET_HE_OP_BASIC_MCS_6SS(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 1, 2, 2)
#define GET_HE_OP_BASIC_MCS_7SS(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 1, 4, 2)
#define GET_HE_OP_BASIC_MCS_8SS(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 1, 6, 2)

#define GET_HE_OP_INFO_PRIMARY_CHAN(_pEleStart) \
	LE_BITS_TO_1BYTE(_pEleStart, 0, 8)
#define GET_HE_OP_INFO_CHAN_WIDTH(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 1, 0, 2)
#define GET_HE_OP_INFO_DUPL_BCN(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 1, 2, 1)
#define GET_HE_OP_INFO_REG_INFO(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 1, 3, 3)
#define GET_HE_OP_INFO_CHAN_CTR_FREQ_SEG0(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 2, 0, 8)
#define GET_HE_OP_INFO_CHAN_CTR_FREQ_SEG1(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 3, 0, 8)
#define GET_HE_OP_INFO_MIN_RATE(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 4, 0, 8)

/* Get MU EDCA Parameter Set element */
#define GET_HE_MU_EDCA_QOS_INFO(_pEleStart) \
	LE_BITS_TO_1BYTE(_pEleStart, 0, 8)
#define GET_HE_MU_EDCA_QOS_INFO_UPDATE_CNT(_pEleStart) \
	LE_BITS_TO_1BYTE(_pEleStart, 0, 4)
#define GET_HE_MU_EDCA_BE_AIFSN(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 1, 0, 4)
#define GET_HE_MU_EDCA_BE_ACI(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 1, 5, 2)
#define GET_HE_MU_EDCA_BE_ECW_MIN_MAX(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 2, 0, 8)
#define GET_HE_MU_EDCA_BE_TIMER(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 3, 0, 8)
#define GET_HE_MU_EDCA_BK_AIFSN(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 4, 0, 4)
#define GET_HE_MU_EDCA_BK_ACI(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 4, 5, 2)
#define GET_HE_MU_EDCA_BK_ECW_MIN_MAX(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 5, 0, 8)
#define GET_HE_MU_EDCA_BK_TIMER(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 6, 0, 8)
#define GET_HE_MU_EDCA_VI_AIFSN(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 7, 0, 4)
#define GET_HE_MU_EDCA_VI_ACI(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 7, 5, 2)
#define GET_HE_MU_EDCA_VI_ECW_MIN_MAX(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 8, 0, 8)
#define GET_HE_MU_EDCA_VI_TIMER(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 9, 0, 8)
#define GET_HE_MU_EDCA_VO_AIFSN(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 10, 0, 4)
#define GET_HE_MU_EDCA_VO_ACI(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 10, 5, 2)
#define GET_HE_MU_EDCA_VO_ECW_MIN_MAX(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 11, 0, 8)
#define GET_HE_MU_EDCA_VO_TIMER(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart) + 12, 0, 8)

#define GET_HE_6G_BAND_CAP_MIN_MPDU_SPACING(_pEleStart) \
	LE_BITS_TO_1BYTE(_pEleStart, 0, 3)
#define GET_HE_6G_BAND_CAP_MAX_AMPDU_LEN_EXP(_pEleStart) \
	LE_BITS_TO_1BYTE(_pEleStart, 3, 3)
#define GET_HE_6G_BAND_CAP_MAX_MPDU_LEN(_pEleStart) \
        LE_BITS_TO_1BYTE(_pEleStart, 6, 2)
#define GET_HE_6G_BAND_CAP_SM_PS(_pEleStart) \
        LE_BITS_TO_1BYTE((_pEleStart) + 1, 1, 2)
#define GET_HE_6G_BAND_CAP_RD_RESP(_pEleStart) \
        LE_BITS_TO_1BYTE((_pEleStart) + 1, 3, 1)
#define GET_HE_6G_BAND_CAP_RX_ANT_PATTERN(_pEleStart) \
        LE_BITS_TO_1BYTE((_pEleStart) + 1, 4, 1)
#define GET_HE_6G_BAND_CAP_TX_ANT_PATTERN(_pEleStart) \
        LE_BITS_TO_1BYTE((_pEleStart) + 1, 5, 1)

#define SET_HE_6G_BAND_CAP_MIN_MPDU_SPACING(_pEleStart, _val) \
        SET_BITS_TO_LE_1BYTE(_pEleStart, 0, 3, _val)
#define SET_HE_6G_BAND_CAP_MAX_AMPDU_LEN_EXP(_pEleStart, _val) \
        SET_BITS_TO_LE_1BYTE(_pEleStart, 3, 3, _val)
#define SET_HE_6G_BAND_CAP_MAX_MPDU_LEN(_pEleStart, _val) \
        SET_BITS_TO_LE_1BYTE(_pEleStart, 6, 2, _val)
#define SET_HE_6G_BAND_CAP_SM_PS(_pEleStart, _val) \
        SET_BITS_TO_LE_1BYTE((_pEleStart) + 1, 1, 2, _val)
#define SET_HE_6G_BAND_CAP_RD_RESP(_pEleStart, _val) \
        SET_BITS_TO_LE_1BYTE((_pEleStart) + 1, 3, 1, _val)
#define SET_HE_6G_BAND_CAP_RX_ANT_PATTERN(_pEleStart, _val) \
        SET_BITS_TO_LE_1BYTE((_pEleStart) + 1, 4, 1, _val)
#define SET_HE_6G_BAND_CAP_TX_ANT_PATTERN(_pEleStart, _val) \
        SET_BITS_TO_LE_1BYTE((_pEleStart) + 1, 5, 1, _val)

/* HE variant HT Control */
#define HE_VAR_HTC	3

#define HE_VAR_HTC_CID_TRS		0
#define HE_VAR_HTC_CID_OM		1
#define HE_VAR_HTC_CID_HLA		2
#define HE_VAR_HTC_CID_BSR		3
#define HE_VAR_HTC_CID_UPH		4
#define HE_VAR_HTC_CID_BQR		5
#define HE_VAR_HTC_CID_CAS		6

/* Set HE variant HT Control field */
#define SET_HE_VAR_HTC(_pStart) \
	SET_BITS_TO_LE_4BYTE((u8 *)_pStart, 0, 2, HE_VAR_HTC)

#define SET_HE_VAR_HTC_CID_TRS(_pStart) \
	SET_BITS_TO_LE_4BYTE((u8 *)_pStart, 2, 4, HE_VAR_HTC_CID_TRS)
#define SET_HE_VAR_HTC_CID_OM(_pStart) \
	SET_BITS_TO_LE_4BYTE((u8 *)_pStart, 2, 4, HE_VAR_HTC_CID_OM)
#define SET_HE_VAR_HTC_CID_HLA(_pStart) \
	SET_BITS_TO_LE_4BYTE((u8 *)_pStart, 2, 4, HE_VAR_HTC_CID_HLA)
#define SET_HE_VAR_HTC_CID_BSR(_pStart) \
	SET_BITS_TO_LE_4BYTE((u8 *)_pStart, 2, 4, HE_VAR_HTC_CID_BSR)
#define SET_HE_VAR_HTC_CID_UPH(_pStart) \
	SET_BITS_TO_LE_4BYTE((u8 *)_pStart, 2, 4, HE_VAR_HTC_CID_UPH)
#define SET_HE_VAR_HTC_CID_BQR(_pStart) \
	SET_BITS_TO_LE_4BYTE((u8 *)_pStart, 2, 4, HE_VAR_HTC_CID_BQR)
#define SET_HE_VAR_HTC_CID_CAS(_pStart) \
	SET_BITS_TO_LE_4BYTE((u8 *)_pStart, 2, 4, HE_VAR_HTC_CID_CAS)

#define SET_HE_VAR_HTC_OM_RX_NSS(_pStart, _val) \
	SET_BITS_TO_LE_4BYTE((u8 *)_pStart, 6, 3, _val)
#define SET_HE_VAR_HTC_OM_CH_WIDTH(_pStart, _val) \
	SET_BITS_TO_LE_4BYTE((u8 *)_pStart, 6 + 3, 2, _val)
#define SET_HE_VAR_HTC_OM_UL_MU_DIS(_pStart, _val) \
	SET_BITS_TO_LE_4BYTE((u8 *)_pStart, 6 + 5, 1, _val)
#define SET_HE_VAR_HTC_OM_TX_NSTS(_pStart, _val) \
	SET_BITS_TO_LE_4BYTE((u8 *)_pStart, 6 + 6, 3, _val)
#define SET_HE_VAR_HTC_OM_ER_SU_DIS(_pStart, _val) \
	SET_BITS_TO_LE_4BYTE((u8 *)_pStart, 6 + 9, 1, _val)
#define SET_HE_VAR_HTC_OM_DL_MU_MIMO_RR(_pStart, _val) \
	SET_BITS_TO_LE_4BYTE((u8 *)_pStart, 6 + 10, 1, _val)
#define SET_HE_VAR_HTC_OM_UL_MU_DATA_DIS(_pStart, _val) \
	SET_BITS_TO_LE_4BYTE((u8 *)_pStart, 6 + 11, 1, _val)

/* Get HE variant HT Control field */
#define GET_VAR_HTC(_pStart) \
	LE_BITS_TO_1BYTE(_pStart, 0, 2)
#define GET_HE_VAR_HTC_CID(_pStart) \
	LE_BITS_TO_1BYTE(_pStart, 2, 4)

#define HE_MCS_SUPP_MSC0_TO_MSC7		0x0	/* 2b00 */
#define HE_MCS_SUPP_MSC0_TO_MSC9		0x1	/* 2b01 */
#define HE_MCS_SUPP_MSC0_TO_MSC11	0x2	/* 2b10 */
#define HE_MSC_NOT_SUPP				0x3	/* 2b11 */
#define HE_MSC_NOT_SUPP_BYTE			((HE_MSC_NOT_SUPP << 6) | (HE_MSC_NOT_SUPP << 4) \
											| (HE_MSC_NOT_SUPP << 2) | HE_MSC_NOT_SUPP)

#define HE_DEV_CLASS_A					1
#define HE_DEV_CLASS_B					0

/*
 * HE_MAC_Cap (6)
 * HE_PHY_Cap (11)
 * HE_Support_MCS (4, 8 or 12)
 * PPE_Thres (variable, max = 25)
 */
#define HE_CAP_ELE_MAC_CAP_LEN			6
#define HE_CAP_ELE_PHY_CAP_LEN			11

#define HE_CAP_ELE_SUPP_MCS_LEN_RX_80M		2
#define HE_CAP_ELE_SUPP_MCS_LEN_TX_80M		2
#define HE_CAP_ELE_SUPP_MCS_LEN_RX_160M		2
#define HE_CAP_ELE_SUPP_MCS_LEN_TX_160M		2
#define HE_CAP_ELE_SUPP_MCS_LEN_RX_80M_80M	2
#define HE_CAP_ELE_SUPP_MCS_LEN_TX_80M_80M	2
#define HE_CAP_ELE_SUPP_MCS_MAX_LEN		(HE_CAP_ELE_SUPP_MCS_LEN_RX_80M \
	+ HE_CAP_ELE_SUPP_MCS_LEN_TX_80M + HE_CAP_ELE_SUPP_MCS_LEN_RX_160M \
	+ HE_CAP_ELE_SUPP_MCS_LEN_TX_160M + HE_CAP_ELE_SUPP_MCS_LEN_RX_80M_80M \
	+ HE_CAP_ELE_SUPP_MCS_LEN_TX_80M_80M)

#define HE_CAP_ELE_PPE_THRE_MAX_LEN		25

#define HE_CAP_ELE_MAX_LEN (1 + HE_CAP_ELE_MAC_CAP_LEN + HE_CAP_ELE_PHY_CAP_LEN \
	+ HE_CAP_ELE_SUPP_MCS_MAX_LEN + HE_CAP_ELE_PPE_THRE_MAX_LEN)
/* #define HE_CAP_MAC_CAP_OFFSET 0
#define HE_CAP_PHY_CAP_OFFSET 6
#define HE_CAP_SUPPORT_MCS_OFFSET 17
*/

/*
 * HE_Ope_Para (3)
 * BSS_Color (1)
 * Basic_MCS (2)
 * VHT_Op (0 or 3)
 * CoHosted_Bssid_Ind (0 or 1)
 * 6Ghz_Ope_Info (0 or 5)
 */
#define HE_OPER_PARAMS_LEN 				3
#define HE_OPER_BSS_COLOR_INFO_LEN 		1
#define HE_OPER_BASIC_MCS_LEN				2
#define HE_OPER_VHT_OPER_INFO_LEN		3
#define HE_OPER_MAX_COHOST_BSSID_LEN 	1
#define HE_OPER_6G_OPER_INFO_LEN			5
 
#define HE_OPER_ELE_MAX_LEN (1 + HE_OPER_PARAMS_LEN + HE_OPER_BSS_COLOR_INFO_LEN \
	+ HE_OPER_BASIC_MCS_LEN + HE_OPER_VHT_OPER_INFO_LEN \
	+ HE_OPER_MAX_COHOST_BSSID_LEN + HE_OPER_6G_OPER_INFO_LEN)
/* #define HE_OPER_PARAS_OFFSET 0
#define HE_OPER_BSS_COLOR_OFFSET 3
#define HE_OPER_BASIC_MCS_OFFSET 4
*/

#define HE_6G_BAND_CAP_INFO_LEN		2
#define HE_6G_BAND_CAP_MAX_LEN	(1 + HE_6G_BAND_CAP_INFO_LEN)

#define MAX_HE_GI_TYPE 3
#define MAX_HE_MCS_INDEX 12 * 2 /* 1SS + 2SS */

enum rtw_he_actrl_om_mask {
	OM_RX_NSS = BIT0,
	OM_CH_BW = BIT1,
	OM_UL_MU_DIS = BIT2,
	OM_TX_NSTS = BIT3,
	OM_ER_SU_DIS = BIT4,
	OM_DL_MU_RR = BIT5,
	OM_UL_MU_DATA_DIS = BIT6
};

struct rtw_he_actrl_om_ele {
	u8 rx_nss;
	u8 channel_width;
	u8 ul_mu_disable;
	u8 tx_nsts;
	u8 er_su_disable;
	u8 dl_mu_mimo_rr;
	u8 ul_mu_data_disable;
};


struct rtw_he_actrl_om {
	/* om ctrl flag for normal tx pkt */
	u8 actrl_om_normal_tx;
	u8 actrl_om_normal_tx_cnt;
	/* current om ctrl element content */
	struct rtw_he_actrl_om_ele om_actrl_ele;
};

struct he_priv {
	u8	he_option;
	u8 	he_cap[HE_CAP_ELE_MAX_LEN];
	u8	he_op[HE_OPER_ELE_MAX_LEN];
	u8 	he_6g_band_cap[HE_6G_BAND_CAP_MAX_LEN];
	u8 	op_present;
	u8	he_highest_rate;
	u8	pre_he_muedca_cnt;
	u8	mpdu_min_spacing;
	struct rtw_he_actrl_om om_info;
};

/*trigger frame*/
#define TRIGGER_FRAME_USER_INFO_SZ 5 /* byte */
#define TRIGGER_FRAME_MIN_LENGTH 24 + TRIGGER_FRAME_USER_INFO_SZ /* byte , aleast one user info !!! */
/*basic tigger frame with 1 byte trigger dependent info */
#define TRIGGER_FRAME_BASIC_USER_INFO_SZ TRIGGER_FRAME_USER_INFO_SZ + 1

/*trigger frame User Info*/
#define GET_TRIGGER_FRAME_TYPE(_pEleStart) \
	LE_BITS_TO_1BYTE((_pEleStart + 16), 0, 4)

#define GET_TRIGGER_FRAME_USER_INFO_AID12(_user_info) \
	LE_BITS_TO_2BYTE(_user_info, 0, 12)

#define GET_TRIGGER_FRAME_USER_INFO_RUA(_user_info) \
	LE_BITS_TO_2BYTE((_user_info + 1), 4, 8)

#define GET_TRIGGER_FRAME_USER_INFO_UL_MCS(_user_info) \
	LE_BITS_TO_2BYTE((_user_info + 2), 5, 4)


enum rtw_he_trigger_frame_type {
	TRIGGER_FRAME_T_BASIC = 0,
	TRIGGER_FRAME_T_BFRP,
	TRIGGER_FRAME_T_MUBAR,
	TRIGGER_FRAME_T_MURTS,
	TRIGGER_FRAME_T_BSRP,
	TRIGGER_FRAME_T_GCR_MUBAR,
	TRIGGER_FRAME_T_BQRP,
	TRIGGER_FRAME_T_NFRP = 7,
	TRIGGER_FRAME_T_RSVD = 8,
};

u16 rtw_he_mcs_to_data_rate(u8 bw, u8 gi, u8 he_mcs_rate);
void	rtw_he_use_default_setting(_adapter *padapter, struct _ADAPTER_LINK *padapter_link);
void	update_sta_he_info_apmode(_adapter *padapter, void *sta);
#if CONFIG_IEEE80211_BAND_6GHZ
void update_sta_6ghz_info_apmode(_adapter *padapter, void *sta);
#endif
void	update_hw_he_param(_adapter *padapter);
void HE_caps_handler(_adapter *padapter, struct _ADAPTER_LINK *padapter_link,
			PNDIS_802_11_VARIABLE_IEs pIE);
struct rtw_phl_stainfo_t;
void HE_mac_caps_handler(_adapter *padapter, struct rtw_phl_stainfo_t *phl_sta, u8 *ele_start);
void HE_phy_caps_handler(_adapter *padapter, struct rtw_phl_stainfo_t *phl_sta, u8 *ele_start, u8 *supp_mcs_len);
void HE_supp_mcs_handler(_adapter *padapter, struct rtw_phl_stainfo_t *phl_sta, u8 *ele_start, u8 supp_mcs_len);
void HE_ppe_thre_handler(_adapter *padapter, struct rtw_phl_stainfo_t *phl_sta, u8 *ele_start);
void rtw_he_set_asoc_cap_supp_mcs(_adapter *padapter, struct rtw_phl_stainfo_t *phl_sta, u8 *ele_start, u8 supp_mcs_len);
u8 rtw_he_get_highest_rate(u8 *he_mcs_map);
void HE_operation_handler(_adapter *padapter, struct _ADAPTER_LINK *padapter_link,
				PNDIS_802_11_VARIABLE_IEs pIE);
void HE_mu_edca_handler(_adapter *padapter, struct _ADAPTER_LINK *padapter_link,
			PNDIS_802_11_VARIABLE_IEs pIE, u8 first);
void HE_6g_bandcap_handler(_adapter *padapter, struct _ADAPTER_LINK *padapter_link,
			PNDIS_802_11_VARIABLE_IEs pIE);
u32 rtw_build_he_cap_ie(_adapter *padapter, struct _ADAPTER_LINK *padapter_link, u8 *pbuf, enum band_type band);
u32 rtw_build_he_operation_ie(_adapter *padapter, struct _ADAPTER_LINK *padapter_link, u8 *pbuf, struct rtw_chan_def *chandef);

struct protocol_cap_t;
struct phy_cap_t;
u32 rtw_get_dft_he_cap_ie(_adapter *padapter, struct phy_cap_t *phy_cap,
		struct protocol_cap_t *proto_cap, u8 *pbuf, enum band_type band);
u32 rtw_build_he_6g_band_cap_ie_by_proto(_adapter *padapter,
		struct protocol_cap_t *proto_cap, u8 *pbuf);

u32 rtw_restructure_he_ie(_adapter *padapter, struct _ADAPTER_LINK *padapter_link,
				u8 *in_ie, u8 *out_ie, uint in_len, uint *pout_len, enum band_type band);
void HEOnAssocRsp(_adapter *padapter);
void rtw_he_ies_attach(_adapter *padapter, struct _ADAPTER_LINK *padapter_link, WLAN_BSSID_EX *pnetwork, enum band_type band);
void rtw_he_ies_detach(_adapter *padapter, struct _ADAPTER_LINK *padapter_link, WLAN_BSSID_EX *pnetwork);
u8 rtw_he_htc_en(_adapter *padapter, struct sta_info *psta);
void rtw_he_fill_htc(_adapter *padapter, struct pkt_attrib *pattrib, u32 *phtc_buf);
void rtw_he_set_om_info(_adapter *padapter, struct _ADAPTER_LINK *alink, u8 om_mask, struct rtw_he_actrl_om *om_info);
void rtw_he_init_om_info(_adapter *padapter, struct _ADAPTER_LINK *padapter_link);
void rtw_update_he_ies(_adapter *padapter, struct _ADAPTER_LINK *padapter_link, WLAN_BSSID_EX *pnetwork, struct rtw_chan_def *chandef);
void rtw_update_probe_rsp_he_cap_and_op(struct _ADAPTER *a, u8 *ies, sint *ies_len, enum band_type band);
void rtw_he_om_ctrl_trx_ss(_adapter *adapter, struct _ADAPTER_LINK *alink,
							struct sta_info *sta, u8 chg_ss, bool need_update_ra);
void rtw_process_he_triggerframe(_adapter *padapter,union recv_frame *precv_frame);
#endif /* _RTW_HE_H_ */


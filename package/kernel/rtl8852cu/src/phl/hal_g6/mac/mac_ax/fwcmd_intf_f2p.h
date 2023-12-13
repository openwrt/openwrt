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

 #ifndef _MAC_AX_FWCMD_INTF_F2P_H_
#define _MAC_AX_FWCMD_INTF_F2P_H_

/* f2p test cmd para*/
#define FWCMD_F2PTEST_ULBW_SH		0
#define FWCMD_F2PTEST_ULBW_MSK		0x3
#define FWCMD_F2PTEST_GILTF_SH		2
#define FWCMD_F2PTEST_GILTF_MSK		0x3
#define FWCMD_F2PTEST_NUMLTF_SH		4
#define FWCMD_F2PTEST_NUMLTF_MSK	0x7
#define FWCMD_F2PTEST_ULSTBC_SH		7
#define FWCMD_F2PTEST_ULSTBC_MSK	0x1
#define FWCMD_F2PTEST_DPLR_SH		8
#define FWCMD_F2PTEST_DPLR_MSK		0x1
#define FWCMD_F2PTEST_TXPWR_SH		9
#define FWCMD_F2PTEST_TXPWR_MSK		0x3F
#define FWCMD_F2PTEST_USERNUM_SH	16
#define FWCMD_F2PTEST_USERNUM_MSK	0xF
#define FWCMD_F2PTEST_PKTNUM_SH		20
#define FWCMD_F2PTEST_PKTNUM_MSK	0xF
#define FWCMD_F2PTEST_BITMAP_SH		24
#define FWCMD_F2PTEST_BITMAP_MSK	0xFF

#define FWCMD_F2PTEST_AID12_SH		0
#define FWCMD_F2PTEST_AID12_MSK		0xFFF
#define FWCMD_F2PTEST_ULMCS_SH		12
#define FWCMD_F2PTEST_ULMCS_MSK		0xF
#define FWCMD_F2PTEST_MACID_SH		16
#define FWCMD_F2PTEST_MACID_MSK		0xFF
#define FWCMD_F2PTEST_RUPOS_SH		24
#define FWCMD_F2PTEST_RUPOS_MSK		0xFF

#define FWCMD_F2PTEST_ULFEC_SH		0
#define FWCMD_F2PTEST_ULFEC_MSK		0x1
#define FWCMD_F2PTEST_ULDCM_SH		1
#define FWCMD_F2PTEST_ULDCM_MSK		0x1
#define FWCMD_F2PTEST_SS_ALLOC_SH	2
#define FWCMD_F2PTEST_SS_ALLOC_MSK	0x3F
#define FWCMD_F2PTEST_UL_TGTRSSI_SH	8
#define FWCMD_F2PTEST_UL_TGTRSSI_MSK	0x7F

#define FWCMD_F2PTEST_PREF_AC_SH	0
#define FWCMD_F2PTEST_PREF_AC_MSK	0x3

#define FWCMD_F2PTEST_DATARATE_SH	0
#define FWCMD_F2PTEST_DATARATE_MSK	0x1FF
#define FWCMD_F2PTEST_MULPORT_SH	9
#define FWCMD_F2PTEST_MULPORT_MSK	0x7
#define FWCMD_F2PTEST_PWR_OFSET_SH	12
#define FWCMD_F2PTEST_PWR_OFSET_MSK	0x7
#define FWCMD_F2PTEST_MODE_SH		16
#define FWCMD_F2PTEST_MODE_MSK		0x3
#define FWCMD_F2PTEST_TYPE_SH		18
#define FWCMD_F2PTEST_TYPE_MSK		0x3F
#define FWCMD_F2PTEST_SIGB_LEN_SH		24
#define FWCMD_F2PTEST_SIGB_LEN_MSK		0xFF

#define FWCMD_F2PTEST_TXCMD_ADDR_SH	0
#define FWCMD_F2PTEST_TXCMD_ADDR_MSK	0xFF
#define FWCMD_F2PTEST_SIGB_ADDR_SH	8
#define FWCMD_F2PTEST_SIGB_ADDR_MSK	0xFF

/* f2p_wd*/
/* dword0 */
#define F2P_WD_CMD_QSEL_SH		0
#define F2P_WD_CMD_QSEL_MSK		0x3f
#define F2P_WD_LS			BIT(10)
#define F2P_WD_FS			BIT(11)
#define F2P_WD_TOTAL_NUMBER_SH		12
#define F2P_WD_TOTAL_NUMBER_MSK		0xf
#define F2P_WD_SEQ_SH			16
#define F2P_WD_SEQ_MSK			0xff
#define F2P_WD_LENGTH_SH		24
#define F2P_WD_LENGTH_MSK		0xff

/* f2p_tx_cmd*/
/* dword0 */
#define F2P_CMD_TYPE_SH			0
#define F2P_CMD_TYPE_MSK		0xff
#define F2P_CMD_SUB_TYPE_SH		8
#define F2P_CMD_SUB_TYPE_MSK		0xff
#define F2P_DL_USER_NUM_SH		16
#define F2P_DL_USER_NUM_MSK		0x1f
#define F2P_BW_SH			21
#define F2P_BW_MSK			0x3
#define F2P_TX_POWER_SH			23
#define F2P_TX_POWER_MSK		0x1ff

/* dword1 */
#define F2P_FW_DEFINE_SH		0
#define F2P_FW_DEFINE_MSK		0xffff
#define F2P_SS_SEL_MODE_SH		16
#define F2P_SS_SEL_MODE_MSK		0x3
#define F2P_NEXT_QSEL_SH		18
#define F2P_NEXT_QSEL_MSK		0x3f
#define F2P_TWT_GROUP_SH		24
#define F2P_TWT_GROUP_MSK		0xf
#define F2P_DIS_CHK_SLP			BIT(28)
#define F2P_RU_MU_2_SU			BIT(29)
#define F2P_DL_T_PE_SH			30
#define F2P_DL_T_PE_MSK			0x3

/* dword2 */
#define F2P_SIGB_CH1_LEN_SH		0
#define F2P_SIGB_CH1_LEN_MSK		0xff
#define F2P_SIGB_CH2_LEN_SH		8
#define F2P_SIGB_CH2_LEN_MSK		0xff
#define F2P_SIGB_SYM_NUM_SH		16
#define F2P_SIGB_SYM_NUM_MSK		0x3f
#define F2P_SIGB_CH2_OFS_SH		22
#define F2P_SIGB_CH2_OFS_MSK		0x1f
#define F2P_DIS_HTP_ACK			BIT(27)
#define F2P_TX_TIME_REF_SH		28
#define F2P_TX_TIME_REF_MSK		0x3
#define F2P_PRI_USER_IDX_SH		30
#define F2P_PRI_USER_IDX_MSK		0x3

/* dword3 */
#define F2P_AMPDU_MAX_TXTIME_SH		0
#define F2P_AMPDU_MAX_TXTIME_MSK	0x3fff
#define F2P_GROUP_ID_SH			16
#define F2P_GROUP_ID_MSK		0x3f
#define F2P_TWT_CHK_EN			BIT(28)
#define F2P_TWT_PORT_ID_SH		29
#define F2P_TWT_PORT_ID_MSK		0x7

/* dword4 */
#define F2P_TWT_START_TIME_SH		0
#define F2P_TWT_START_TIME_MSK		0xffffffff

/* dword5 */
#define F2P_TWT_END_TIME_SH		0
#define F2P_TWT_END_TIME_MSK		0xffffffff

/* dword6 */
#define F2P_APEP_LEN_SH			0
#define F2P_APEP_LEN_MSK		0xfff
#define F2P_TRI_PAD_SH			12
#define F2P_TRI_PAD_MSK			0x3
#define F2P_UL_T_PE_SH			14
#define F2P_UL_T_PE_MSK			0x3
#define F2P_RF_GAIN_IDX_SH		16
#define F2P_RF_GAIN_IDX_MSK		0x3ff
#define F2P_FIXED_GAIN_EN		BIT(26)
#define F2P_UL_GI_LTF_SH		27
#define F2P_UL_GI_LTF_MSK		0x7
#define F2P_UL_DOPPLER			BIT(30)
#define F2P_UL_STBC			BIT(31)

/* dword7 */
#define F2P_UL_MID_PER			BIT(0)
#define F2P_UL_CQI_RRP_TRI		BIT(1)
#define F2P_SIGB_DCM			BIT(16)
#define F2P_SIGB_COMP			BIT(17)
#define F2P_DOPPLER			BIT(18)
#define F2P_STBC			BIT(19)
#define F2P_MID_PER			BIT(20)
#define F2P_GI_LTF_SIZE_SH		21
#define F2P_GI_LTF_SIZE_MSK		0x7
#define F2P_SIGB_MCS_SH			24
#define F2P_SIGB_MCS_MSK		0x7

/* dword8 */
#define F2P_MACID_U0_SH			0
#define F2P_MACID_U0_MSK		0xff
#define F2P_AC_TYPE_U0_SH		8
#define F2P_AC_TYPE_U0_MSK		0x3
#define F2P_MU_STA_POS_U0_SH		10
#define F2P_MU_STA_POS_U0_MSK		0x3
#define F2P_DL_RATE_IDX_U0_SH		12
#define F2P_DL_RATE_IDX_U0_MSK		0x1ff
#define F2P_TX_CMD_DL_DCM_EN_U0		BIT(21)
#define F2P_RU_ALO_IDX_U0_SH		24
#define F2P_RU_ALO_IDX_U0_MSK		0xff

/* dword9 */
#define F2P_PWR_BOOST_U0_SH		0
#define F2P_PWR_BOOST_U0_MSK		0x1f
#define F2P_AGG_BMP_ALO_U0_SH		5
#define F2P_AGG_BMP_ALO_U0_MSK		0x7
#define F2P_AMPDU_MAX_NUM_U0_SH		8
#define F2P_AMPDU_MAX_NUM_U0_MSK	0xff
#define F2P_USER_DEFINE_U0_SH		16
#define F2P_USER_DEFINE_U0_MSK		0xff
#define F2P_USER_DEFINE_EXT_U0_SH	24
#define F2P_USER_DEFINE_EXT_U0_MSK	0xff

/* dword10 */
#define F2P_UL_ADDR_IDX_U0_SH		0
#define F2P_UL_ADDR_IDX_U0_MSK		0xff
#define F2P_UL_DCM_U0			BIT(8)
#define F2P_UL_FEC_COD_U0		BIT(9)
#define F2P_UL_RU_RATE_U0_SH		10
#define F2P_UL_RU_RATE_U0_MSK		0x7f
#define F2P_UL_RU_ALO_IDX_U0_SH		24
#define F2P_UL_RU_ALO_IDX_U0_MSK	0xff

/* dword11 */

/* dword12 */
#define F2P_MACID_U1_SH			0
#define F2P_MACID_U1_MSK		0xff
#define F2P_AC_TYPE_U1_SH		8
#define F2P_AC_TYPE_U1_MSK		0x3
#define F2P_MU_STA_POS_U1_SH		10
#define F2P_MU_STA_POS_U1_MSK		0x3
#define F2P_DL_RATE_IDX_U1_SH		12
#define F2P_DL_RATE_IDX_U1_MSK		0x1ff
#define F2P_TX_CMD_DL_DCM_EN_U1		BIT(21)
#define F2P_RU_ALO_IDX_U1_SH		24
#define F2P_RU_ALO_IDX_U1_MSK		0xff

/* dword13 */
#define F2P_PWR_BOOST_U1_SH		0
#define F2P_PWR_BOOST_U1_MSK		0x1f
#define F2P_AGG_BMP_ALO_U1_SH		5
#define F2P_AGG_BMP_ALO_U1_MSK		0x7
#define F2P_AMPDU_MAX_NUM_U1_SH		8
#define F2P_AMPDU_MAX_NUM_U1_MSK	0xff
#define F2P_USER_DEFINE_U1_SH		16
#define F2P_USER_DEFINE_U1_MSK		0xff
#define F2P_USER_DEFINE_EXT_U1_SH	24
#define F2P_USER_DEFINE_EXT_U1_MSK	0xff

/* dword14 */
#define F2P_UL_ADDR_IDX_U1_SH		0
#define F2P_UL_ADDR_IDX_U1_MSK		0xff
#define F2P_UL_DCM_U1			BIT(8)
#define F2P_UL_FEC_COD_U1		BIT(9)
#define F2P_UL_RU_RATE_U1_SH		10
#define F2P_UL_RU_RATE_U1_MSK		0x7f
#define F2P_UL_RU_ALO_IDX_U1_SH		24
#define F2P_UL_RU_ALO_IDX_U1_MSK	0xff

/* dword15 */

/* dword16 */
#define F2P_MACID_U2_SH			0
#define F2P_MACID_U2_MSK		0xff
#define F2P_AC_TYPE_U2_SH		8
#define F2P_AC_TYPE_U2_MSK		0x3
#define F2P_MU_STA_POS_U2_SH		10
#define F2P_MU_STA_POS_U2_MSK		0x3
#define F2P_DL_RATE_IDX_U2_SH		12
#define F2P_DL_RATE_IDX_U2_MSK		0x1ff
#define F2P_TX_CMD_DL_DCM_EN_U2		BIT(21)
#define F2P_RU_ALO_IDX_U2_SH		24
#define F2P_RU_ALO_IDX_U2_MSK		0xff

/* dword17 */
#define F2P_PWR_BOOST_U2_SH		0
#define F2P_PWR_BOOST_U2_MSK		0x1f
#define F2P_AGG_BMP_ALO_U2_SH		5
#define F2P_AGG_BMP_ALO_U2_MSK		0x7
#define F2P_AMPDU_MAX_NUM_U2_SH		8
#define F2P_AMPDU_MAX_NUM_U2_MSK	0xff
#define F2P_USER_DEFINE_U2_SH		16
#define F2P_USER_DEFINE_U2_MSK		0xff
#define F2P_USER_DEFINE_EXT_U2_SH	24
#define F2P_USER_DEFINE_EXT_U2_MSK	0xff

/* dword18 */
#define F2P_UL_ADDR_IDX_U2_SH		0
#define F2P_UL_ADDR_IDX_U2_MSK		0xff
#define F2P_UL_DCM_U2			BIT(8)
#define F2P_UL_FEC_COD_U2		BIT(9)
#define F2P_UL_RU_RATE_U2_SH		10
#define F2P_UL_RU_RATE_U2_MSK		0x7f
#define F2P_UL_RU_ALO_IDX_U2_SH		24
#define F2P_UL_RU_ALO_IDX_U2_MSK	0xff

/* dword19 */

/* dword20 */
#define F2P_MACID_U3_SH			0
#define F2P_MACID_U3_MSK		0xff
#define F2P_AC_TYPE_U3_SH		8
#define F2P_AC_TYPE_U3_MSK		0x3
#define F2P_MU_STA_POS_U3_SH		10
#define F2P_MU_STA_POS_U3_MSK		0x3
#define F2P_DL_RATE_IDX_U3_SH		12
#define F2P_DL_RATE_IDX_U3_MSK		0x1ff
#define F2P_TX_CMD_DL_DCM_EN_U3		BIT(21)
#define F2P_RU_ALO_IDX_U3_SH		24
#define F2P_RU_ALO_IDX_U3_MSK		0xff

/* dword21 */
#define F2P_PWR_BOOST_U3_SH		0
#define F2P_PWR_BOOST_U3_MSK		0x1f
#define F2P_AGG_BMP_ALO_U3_SH		5
#define F2P_AGG_BMP_ALO_U3_MSK		0x7
#define F2P_AMPDU_MAX_NUM_U3_SH		8
#define F2P_AMPDU_MAX_NUM_U3_MSK	0xff
#define F2P_USER_DEFINE_U3_SH		16
#define F2P_USER_DEFINE_U3_MSK		0xff
#define F2P_USER_DEFINE_EXT_U3_SH	24
#define F2P_USER_DEFINE_EXT_U3_MSK	0xff

/* dword22 */
#define F2P_UL_ADDR_IDX_U3_SH		0
#define F2P_UL_ADDR_IDX_U3_MSK		0xff
#define F2P_UL_DCM_U3			BIT(8)
#define F2P_UL_FEC_COD_U3		BIT(9)
#define F2P_UL_RU_RATE_U3_SH		10
#define F2P_UL_RU_RATE_U3_MSK		0x7f
#define F2P_UL_RU_ALO_IDX_U3_SH		24
#define F2P_UL_RU_ALO_IDX_U3_MSK	0xff

/* dword23 */

/* dword24 */
#define F2P_PKT_ID_0_SH			0
#define F2P_PKT_ID_0_MSK		0xfff
#define F2P_VALID_0			BIT(15)
#define F2P_UL_USER_NUM_0_SH		16
#define F2P_UL_USER_NUM_0_MSK		0xf

/* dword25 */
#define F2P_PKT_ID_1_SH			0
#define F2P_PKT_ID_1_MSK		0xfff
#define F2P_VALID_1			BIT(15)
#define F2P_UL_USER_NUM_1_SH		16
#define F2P_UL_USER_NUM_1_MSK		0xf

/* dword26 */
#define F2P_PKT_ID_2_SH			0
#define F2P_PKT_ID_2_MSK		0xfff
#define F2P_VALID_2			BIT(15)
#define F2P_UL_USER_NUM_2_SH		16
#define F2P_UL_USER_NUM_2_MSK		0xf

/* dword27 */
#define F2P_PKT_ID_3_SH			0
#define F2P_PKT_ID_3_MSK		0xfff
#define F2P_VALID_3			BIT(15)
#define F2P_UL_USER_NUM_3_SH		16
#define F2P_UL_USER_NUM_3_MSK		0xf

/* dword28 */
#define F2P_PKT_ID_4_SH			0
#define F2P_PKT_ID_4_MSK		0xfff
#define F2P_VALID_4			BIT(15)
#define F2P_UL_USER_NUM_4_SH		16
#define F2P_UL_USER_NUM_4_MSK		0xf

/* dword29 */
#define F2P_PKT_ID_5_SH			0
#define F2P_PKT_ID_5_MSK		0xfff
#define F2P_VALID_5			BIT(15)
#define F2P_UL_USER_NUM_5_SH		16
#define F2P_UL_USER_NUM_5_MSK		0xf

/**
 * @struct fwcmd_test_para
 * @brief fwcmd_test_para
 *
 * @var fwcmd_test_para::dword0
 * Please Place Description here.
 * @var fwcmd_test_para::dword1
 * Please Place Description here.
 * @var fwcmd_test_para::dword2
 * Please Place Description here.
 * @var fwcmd_test_para::dword3
 * Please Place Description here.
 * @var fwcmd_test_para::dword4
 * Please Place Description here.
 * @var fwcmd_test_para::dword5
 * Please Place Description here.
 * @var fwcmd_test_para::dword6
 * Please Place Description here.
 * @var fwcmd_test_para::dword7
 * Please Place Description here.
 * @var fwcmd_test_para::dword8
 * Please Place Description here.
 * @var fwcmd_test_para::byte9
 * Please Place Description here.
 * @var fwcmd_test_para::byte10
 * Please Place Description here.
 * @var fwcmd_test_para::byte11
 * Please Place Description here.
 * @var fwcmd_test_para::byte12
 * Please Place Description here.
 * @var fwcmd_test_para::dword13
 * Please Place Description here.
 * @var fwcmd_test_para::dword14
 * Please Place Description here.
 * @var fwcmd_test_para::dword15
 * Please Place Description here.
 * @var fwcmd_test_para::dword16
 * Please Place Description here.
 * @var fwcmd_test_para::dword17
 * Please Place Description here.
 * @var fwcmd_test_para::dword18
 * Please Place Description here.
 * @var fwcmd_test_para::dword19
 * Please Place Description here.
 * @var fwcmd_test_para::dword20
 * Please Place Description here.
 * @var fwcmd_test_para::dword21
 * Please Place Description here.
 * @var fwcmd_test_para::dword22
 * Please Place Description here.
 * @var fwcmd_test_para::dword23
 * Please Place Description here.
 * @var fwcmd_test_para::dword24
 * Please Place Description here.
 * @var fwcmd_test_para::dword25
 * Please Place Description here.
 * @var fwcmd_test_para::dword26
 * Please Place Description here.
 * @var fwcmd_test_para::dword27
 * Please Place Description here.
 * @var fwcmd_test_para::dword28
 * Please Place Description here.
 * @var fwcmd_test_para::dword29
 * Please Place Description here.
 * @var fwcmd_test_para::dword30
 * Please Place Description here.
 * @var fwcmd_test_para::dword31
 * Please Place Description here.
 * @var fwcmd_test_para::dword32
 * Please Place Description here.
 * @var fwcmd_test_para::dword33
 * Please Place Description here.
 * @var fwcmd_test_para::dword34
 * Please Place Description here.
 * @var fwcmd_test_para::dword35
 * Please Place Description here.
 * @var fwcmd_test_para::dword36
 * Please Place Description here.
 * @var fwcmd_test_para::dword37
 * Please Place Description here.
 * @var fwcmd_test_para::dword38
 * Please Place Description here.
 * @var fwcmd_test_para::dword39
 * Please Place Description here.
 * @var fwcmd_test_para::dword40
 * Please Place Description here.
 * @var fwcmd_test_para::dword41
 * Please Place Description here.
 * @var fwcmd_test_para::dword42
 * Please Place Description here.
 * @var fwcmd_test_para::dword43
 * Please Place Description here.
 * @var fwcmd_test_para::dword44
 * Please Place Description here.
 * @var fwcmd_test_para::dword45
 * Please Place Description here.
 * @var fwcmd_test_para::byte46
 * Please Place Description here.
 */

struct fwcmd_test_para {
#define MAX_SIGB_LEN 64
	u32 dword0;
	u32 dword1;
	u32 dword2;
	u32 dword3;
	u32 dword4;
	u32 dword5;
	u32 dword6;
	u32 dword7;
	u32 dword8;
	u8 byte9;
	u8 byte10;
	u8 byte11;
	u8 byte12;
	u32 dword13;
	u32 dword14;
	u32 dword15;
	u32 dword16;
	u32 dword17;
	u32 dword18;
	u32 dword19;
	u32 dword20;
	u32 dword21;
	u32 dword22;
	u32 dword23;
	u32 dword24;
	u32 dword25;
	u32 dword26;
	u32 dword27;
	u32 dword28;
	u32 dword29;
	u32 dword30;
	u32 dword31;
	u32 dword32;
	u32 dword33;
	u32 dword34;
	u32 dword35;
	u32 dword36;
	u32 dword37;
	u32 dword38;
	u32 dword39;
	u32 dword40;
	u32 dword41;
	u32 dword42;
	u32 dword43;
	u32 dword44;
	u32 dword45;
	u8 byte46[MAX_SIGB_LEN];
};

#endif

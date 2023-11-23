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
#ifndef __HAL_RATE_H__
#define __HAL_RATE_H__

/*-----------------------------------------------------------
 *	HW Rate def
 * -----------------------------------------------------------
 * CCK Rates, TxHT = 0 */
#define DESC_RATE1M				0x00
#define DESC_RATE2M				0x01
#define DESC_RATE5_5M			0x02
#define DESC_RATE11M			0x03

/* OFDM Rates, TxHT = 0 */
#define DESC_RATE6M				0x04
#define DESC_RATE9M				0x05
#define DESC_RATE12M			0x06
#define DESC_RATE18M			0x07
#define DESC_RATE24M			0x08
#define DESC_RATE36M			0x09
#define DESC_RATE48M			0x0A
#define DESC_RATE54M			0x0B

/* MCS Rates, TxHT = 1 */
#define DESC_RATEMCS0			0x0C
#define DESC_RATEMCS1			0x0D
#define DESC_RATEMCS2			0x0E
#define DESC_RATEMCS3			0x0F
#define DESC_RATEMCS4			0x10
#define DESC_RATEMCS5			0x11
#define DESC_RATEMCS6			0x12
#define DESC_RATEMCS7			0x13
#define DESC_RATEMCS8			0x14
#define DESC_RATEMCS9			0x15
#define DESC_RATEMCS10			0x16
#define DESC_RATEMCS11			0x17
#define DESC_RATEMCS12			0x18
#define DESC_RATEMCS13			0x19
#define DESC_RATEMCS14			0x1A
#define DESC_RATEMCS15			0x1B
#define DESC_RATEMCS16			0x1C
#define DESC_RATEMCS17			0x1D
#define DESC_RATEMCS18			0x1E
#define DESC_RATEMCS19			0x1F
#define DESC_RATEMCS20			0x20
#define DESC_RATEMCS21			0x21
#define DESC_RATEMCS22			0x22
#define DESC_RATEMCS23			0x23
#define DESC_RATEMCS24			0x24
#define DESC_RATEMCS25			0x25
#define DESC_RATEMCS26			0x26
#define DESC_RATEMCS27			0x27
#define DESC_RATEMCS28			0x28
#define DESC_RATEMCS29			0x29
#define DESC_RATEMCS30			0x2A
#define DESC_RATEMCS31			0x2B
#define DESC_RATEVHTSS1MCS0		0x2C
#define DESC_RATEVHTSS1MCS1		0x2D
#define DESC_RATEVHTSS1MCS2		0x2E
#define DESC_RATEVHTSS1MCS3		0x2F
#define DESC_RATEVHTSS1MCS4		0x30
#define DESC_RATEVHTSS1MCS5		0x31
#define DESC_RATEVHTSS1MCS6		0x32
#define DESC_RATEVHTSS1MCS7		0x33
#define DESC_RATEVHTSS1MCS8		0x34
#define DESC_RATEVHTSS1MCS9		0x35
#define DESC_RATEVHTSS2MCS0		0x36
#define DESC_RATEVHTSS2MCS1		0x37
#define DESC_RATEVHTSS2MCS2		0x38
#define DESC_RATEVHTSS2MCS3		0x39
#define DESC_RATEVHTSS2MCS4		0x3A
#define DESC_RATEVHTSS2MCS5		0x3B
#define DESC_RATEVHTSS2MCS6		0x3C
#define DESC_RATEVHTSS2MCS7		0x3D
#define DESC_RATEVHTSS2MCS8		0x3E
#define DESC_RATEVHTSS2MCS9		0x3F
#define DESC_RATEVHTSS3MCS0		0x40
#define DESC_RATEVHTSS3MCS1		0x41
#define DESC_RATEVHTSS3MCS2		0x42
#define DESC_RATEVHTSS3MCS3		0x43
#define DESC_RATEVHTSS3MCS4		0x44
#define DESC_RATEVHTSS3MCS5		0x45
#define DESC_RATEVHTSS3MCS6		0x46
#define DESC_RATEVHTSS3MCS7		0x47
#define DESC_RATEVHTSS3MCS8		0x48
#define DESC_RATEVHTSS3MCS9		0x49
#define DESC_RATEVHTSS4MCS0		0x4A
#define DESC_RATEVHTSS4MCS1		0x4B
#define DESC_RATEVHTSS4MCS2		0x4C
#define DESC_RATEVHTSS4MCS3		0x4D
#define DESC_RATEVHTSS4MCS4		0x4E
#define DESC_RATEVHTSS4MCS5		0x4F
#define DESC_RATEVHTSS4MCS6		0x50
#define DESC_RATEVHTSS4MCS7		0x51
#define DESC_RATEVHTSS4MCS8		0x52
#define DESC_RATEVHTSS4MCS9		0x53

#define HDATA_RATE(rate)\
	(rate == DESC_RATE1M) ? "CCK_1M" :\
	(rate == DESC_RATE2M) ? "CCK_2M" :\
	(rate == DESC_RATE5_5M) ? "CCK5_5M" :\
	(rate == DESC_RATE11M) ? "CCK_11M" :\
	(rate == DESC_RATE6M) ? "OFDM_6M" :\
	(rate == DESC_RATE9M) ? "OFDM_9M" :\
	(rate == DESC_RATE12M) ? "OFDM_12M" :\
	(rate == DESC_RATE18M) ? "OFDM_18M" :\
	(rate == DESC_RATE24M) ? "OFDM_24M" :\
	(rate == DESC_RATE36M) ? "OFDM_36M" :\
	(rate == DESC_RATE48M) ? "OFDM_48M" :\
	(rate == DESC_RATE54M) ? "OFDM_54M" :\
	(rate == DESC_RATEMCS0) ? "MCS0" :\
	(rate == DESC_RATEMCS1) ? "MCS1" :\
	(rate == DESC_RATEMCS2) ? "MCS2" :\
	(rate == DESC_RATEMCS3) ? "MCS3" :\
	(rate == DESC_RATEMCS4) ? "MCS4" :\
	(rate == DESC_RATEMCS5) ? "MCS5" :\
	(rate == DESC_RATEMCS6) ? "MCS6" :\
	(rate == DESC_RATEMCS7) ? "MCS7" :\
	(rate == DESC_RATEMCS8) ? "MCS8" :\
	(rate == DESC_RATEMCS9) ? "MCS9" :\
	(rate == DESC_RATEMCS10) ? "MCS10" :\
	(rate == DESC_RATEMCS11) ? "MCS11" :\
	(rate == DESC_RATEMCS12) ? "MCS12" :\
	(rate == DESC_RATEMCS13) ? "MCS13" :\
	(rate == DESC_RATEMCS14) ? "MCS14" :\
	(rate == DESC_RATEMCS15) ? "MCS15" :\
	(rate == DESC_RATEMCS16) ? "MCS16" :\
	(rate == DESC_RATEMCS17) ? "MCS17" :\
	(rate == DESC_RATEMCS18) ? "MCS18" :\
	(rate == DESC_RATEMCS19) ? "MCS19" :\
	(rate == DESC_RATEMCS20) ? "MCS20" :\
	(rate == DESC_RATEMCS21) ? "MCS21" :\
	(rate == DESC_RATEMCS22) ? "MCS22" :\
	(rate == DESC_RATEMCS23) ? "MCS23" :\
	(rate == DESC_RATEMCS24) ? "MCS24" :\
	(rate == DESC_RATEMCS25) ? "MCS25" :\
	(rate == DESC_RATEMCS26) ? "MCS26" :\
	(rate == DESC_RATEMCS27) ? "MCS27" :\
	(rate == DESC_RATEMCS28) ? "MCS28" :\
	(rate == DESC_RATEMCS29) ? "MCS29" :\
	(rate == DESC_RATEMCS30) ? "MCS30" :\
	(rate == DESC_RATEMCS31) ? "MCS31" :\
	(rate == DESC_RATEVHTSS1MCS0) ? "VHTSS1MCS0" :\
	(rate == DESC_RATEVHTSS1MCS1) ? "VHTSS1MCS1" :\
	(rate == DESC_RATEVHTSS1MCS2) ? "VHTSS1MCS2" :\
	(rate == DESC_RATEVHTSS1MCS3) ? "VHTSS1MCS3" :\
	(rate == DESC_RATEVHTSS1MCS4) ? "VHTSS1MCS4" :\
	(rate == DESC_RATEVHTSS1MCS5) ? "VHTSS1MCS5" :\
	(rate == DESC_RATEVHTSS1MCS6) ? "VHTSS1MCS6" :\
	(rate == DESC_RATEVHTSS1MCS7) ? "VHTSS1MCS7" :\
	(rate == DESC_RATEVHTSS1MCS8) ? "VHTSS1MCS8" :\
	(rate == DESC_RATEVHTSS1MCS9) ? "VHTSS1MCS9" :\
	(rate == DESC_RATEVHTSS2MCS0) ? "VHTSS2MCS0" :\
	(rate == DESC_RATEVHTSS2MCS1) ? "VHTSS2MCS1" :\
	(rate == DESC_RATEVHTSS2MCS2) ? "VHTSS2MCS2" :\
	(rate == DESC_RATEVHTSS2MCS3) ? "VHTSS2MCS3" :\
	(rate == DESC_RATEVHTSS2MCS4) ? "VHTSS2MCS4" :\
	(rate == DESC_RATEVHTSS2MCS5) ? "VHTSS2MCS5" :\
	(rate == DESC_RATEVHTSS2MCS6) ? "VHTSS2MCS6" :\
	(rate == DESC_RATEVHTSS2MCS7) ? "VHTSS2MCS7" :\
	(rate == DESC_RATEVHTSS2MCS8) ? "VHTSS2MCS8" :\
	(rate == DESC_RATEVHTSS2MCS9) ? "VHTSS2MCS9" :\
	(rate == DESC_RATEVHTSS3MCS0) ? "VHTSS3MCS0" :\
	(rate == DESC_RATEVHTSS3MCS1) ? "VHTSS3MCS1" :\
	(rate == DESC_RATEVHTSS3MCS2) ? "VHTSS3MCS2" :\
	(rate == DESC_RATEVHTSS3MCS3) ? "VHTSS3MCS3" :\
	(rate == DESC_RATEVHTSS3MCS4) ? "VHTSS3MCS4" :\
	(rate == DESC_RATEVHTSS3MCS5) ? "VHTSS3MCS5" :\
	(rate == DESC_RATEVHTSS3MCS6) ? "VHTSS3MCS6" :\
	(rate == DESC_RATEVHTSS3MCS7) ? "VHTSS3MCS7" :\
	(rate == DESC_RATEVHTSS3MCS8) ? "VHTSS3MCS8" :\
	(rate == DESC_RATEVHTSS3MCS9) ? "VHTSS3MCS9" :\
	(rate == DESC_RATEVHTSS4MCS0) ? "VHTSS4MCS0" :\
	(rate == DESC_RATEVHTSS4MCS1) ? "VHTSS4MCS1" :\
	(rate == DESC_RATEVHTSS4MCS2) ? "VHTSS4MCS2" :\
	(rate == DESC_RATEVHTSS4MCS3) ? "VHTSS4MCS3" :\
	(rate == DESC_RATEVHTSS4MCS4) ? "VHTSS4MCS4" :\
	(rate == DESC_RATEVHTSS4MCS5) ? "VHTSS4MCS5" :\
	(rate == DESC_RATEVHTSS4MCS6) ? "VHTSS4MCS6" :\
	(rate == DESC_RATEVHTSS4MCS7) ? "VHTSS4MCS7" :\
	(rate == DESC_RATEVHTSS4MCS8) ? "VHTSS4MCS8" :\
	(rate == DESC_RATEVHTSS4MCS9) ? "VHTSS4MCS9" :\
	"UNKNOWN"

enum hw_data_rate {
	HRATE_CCK1		= 0x0,
	HRATE_CCK2		= 0x1,
	HRATE_CCK5_5	= 0x2,
	HRATE_CCK11		= 0x3,
	HRATE_OFDM6		= 0x4,
	HRATE_OFDM9		= 0x5,
	HRATE_OFDM12	= 0x6,
	HRATE_OFDM18	= 0x7,
	HRATE_OFDM24	= 0x8,
	HRATE_OFDM36	= 0x9,
	HRATE_OFDM48	= 0xA,
	HRATE_OFDM54	= 0xB,
	HRATE_MCS0		= 0x80,
	HRATE_MCS1		= 0x81,
	HRATE_MCS2		= 0x82,
	HRATE_MCS3		= 0x83,
	HRATE_MCS4		= 0x84,
	HRATE_MCS5		= 0x85,
	HRATE_MCS6		= 0x86,
	HRATE_MCS7		= 0x87,
	HRATE_MCS8		= 0x88,
	HRATE_MCS9		= 0x89,
	HRATE_MCS10		= 0x8A,
	HRATE_MCS11		= 0x8B,
	HRATE_MCS12		= 0x8C,
	HRATE_MCS13		= 0x8D,
	HRATE_MCS14		= 0x8E,
	HRATE_MCS15		= 0x8F,
	HRATE_MCS16		= 0x90,
	HRATE_MCS17		= 0x91,
	HRATE_MCS18		= 0x92,
	HRATE_MCS19		= 0x93,
	HRATE_MCS20		= 0x94,
	HRATE_MCS21		= 0x95,
	HRATE_MCS22		= 0x96,
	HRATE_MCS23		= 0x97,
	HRATE_MCS24		= 0x98,
	HRATE_MCS25		= 0x99,
	HRATE_MCS26		= 0x9A,
	HRATE_MCS27		= 0x9B,
	HRATE_MCS28		= 0x9C,
	HRATE_MCS29		= 0x9D,
	HRATE_MCS30		= 0x9E,
	HRATE_MCS31		= 0x9F,
	HRATE_VHT_NSS1_MCS0	= 0x100,
	HRATE_VHT_NSS1_MCS1	= 0x101,
	HRATE_VHT_NSS1_MCS2	= 0x102,
	HRATE_VHT_NSS1_MCS3	= 0x103,
	HRATE_VHT_NSS1_MCS4	= 0x104,
	HRATE_VHT_NSS1_MCS5	= 0x105,
	HRATE_VHT_NSS1_MCS6	= 0x106,
	HRATE_VHT_NSS1_MCS7	= 0x107,
	HRATE_VHT_NSS1_MCS8	= 0x108,
	HRATE_VHT_NSS1_MCS9	= 0x109,
	HRATE_VHT_NSS2_MCS0	= 0x110,
	HRATE_VHT_NSS2_MCS1	= 0x111,
	HRATE_VHT_NSS2_MCS2	= 0x112,
	HRATE_VHT_NSS2_MCS3	= 0x113,
	HRATE_VHT_NSS2_MCS4	= 0x114,
	HRATE_VHT_NSS2_MCS5	= 0x115,
	HRATE_VHT_NSS2_MCS6	= 0x116,
	HRATE_VHT_NSS2_MCS7	= 0x117,
	HRATE_VHT_NSS2_MCS8	= 0x118,
	HRATE_VHT_NSS2_MCS9	= 0x119,
	HRATE_VHT_NSS3_MCS0	= 0x120,
	HRATE_VHT_NSS3_MCS1	= 0x121,
	HRATE_VHT_NSS3_MCS2	= 0x122,
	HRATE_VHT_NSS3_MCS3	= 0x123,
	HRATE_VHT_NSS3_MCS4	= 0x124,
	HRATE_VHT_NSS3_MCS5	= 0x125,
	HRATE_VHT_NSS3_MCS6	= 0x126,
	HRATE_VHT_NSS3_MCS7	= 0x127,
	HRATE_VHT_NSS3_MCS8	= 0x128,
	HRATE_VHT_NSS3_MCS9	= 0x129,
	HRATE_VHT_NSS4_MCS0	= 0x130,
	HRATE_VHT_NSS4_MCS1	= 0x131,
	HRATE_VHT_NSS4_MCS2	= 0x132,
	HRATE_VHT_NSS4_MCS3	= 0x133,
	HRATE_VHT_NSS4_MCS4	= 0x134,
	HRATE_VHT_NSS4_MCS5	= 0x135,
	HRATE_VHT_NSS4_MCS6	= 0x136,
	HRATE_VHT_NSS4_MCS7	= 0x137,
	HRATE_VHT_NSS4_MCS8	= 0x138,
	HRATE_VHT_NSS4_MCS9	= 0x139,
	HRATE_HE_NSS1_MCS0	= 0x180,
	HRATE_HE_NSS1_MCS1	= 0x181,
	HRATE_HE_NSS1_MCS2	= 0x182,
	HRATE_HE_NSS1_MCS3	= 0x183,
	HRATE_HE_NSS1_MCS4	= 0x184,
	HRATE_HE_NSS1_MCS5	= 0x185,
	HRATE_HE_NSS1_MCS6	= 0x186,
	HRATE_HE_NSS1_MCS7	= 0x187,
	HRATE_HE_NSS1_MCS8	= 0x188,
	HRATE_HE_NSS1_MCS9	= 0x189,
	HRATE_HE_NSS1_MCS10	= 0x18A,
	HRATE_HE_NSS1_MCS11	= 0x18B,
	HRATE_HE_NSS2_MCS0	= 0x190,
	HRATE_HE_NSS2_MCS1	= 0x191,
	HRATE_HE_NSS2_MCS2	= 0x192,
	HRATE_HE_NSS2_MCS3	= 0x193,
	HRATE_HE_NSS2_MCS4	= 0x194,
	HRATE_HE_NSS2_MCS5	= 0x195,
	HRATE_HE_NSS2_MCS6	= 0x196,
	HRATE_HE_NSS2_MCS7	= 0x197,
	HRATE_HE_NSS2_MCS8	= 0x198,
	HRATE_HE_NSS2_MCS9	= 0x199,
	HRATE_HE_NSS2_MCS10	= 0x19A,
	HRATE_HE_NSS2_MCS11	= 0x19B,
	HRATE_HE_NSS3_MCS0	= 0x1A0,
	HRATE_HE_NSS3_MCS1	= 0x1A1,
	HRATE_HE_NSS3_MCS2	= 0x1A2,
	HRATE_HE_NSS3_MCS3	= 0x1A3,
	HRATE_HE_NSS3_MCS4	= 0x1A4,
	HRATE_HE_NSS3_MCS5	= 0x1A5,
	HRATE_HE_NSS3_MCS6	= 0x1A6,
	HRATE_HE_NSS3_MCS7	= 0x1A7,
	HRATE_HE_NSS3_MCS8	= 0x1A8,
	HRATE_HE_NSS3_MCS9	= 0x1A9,
	HRATE_HE_NSS3_MCS10	= 0x1AA,
	HRATE_HE_NSS3_MCS11	= 0x1AB,
	HRATE_HE_NSS4_MCS0	= 0x1B0,
	HRATE_HE_NSS4_MCS1	= 0x1B1,
	HRATE_HE_NSS4_MCS2	= 0x1B2,
	HRATE_HE_NSS4_MCS3	= 0x1B3,
	HRATE_HE_NSS4_MCS4	= 0x1B4,
	HRATE_HE_NSS4_MCS5	= 0x1B5,
	HRATE_HE_NSS4_MCS6	= 0x1B6,
	HRATE_HE_NSS4_MCS7	= 0x1B7,
	HRATE_HE_NSS4_MCS8	= 0x1B8,
	HRATE_HE_NSS4_MCS9	= 0x1B9,
	HRATE_HE_NSS4_MCS10	= 0x1BA,
	HRATE_HE_NSS4_MCS11	= 0x1BB
};

static inline u16 mrate_to_hwrate(u16 rate)
{
	u16 ret = DESC_RATE1M;

	switch (rate) {
	case MGN_1M:
		ret = RTW_DATA_RATE_CCK1;
		break;
	case MGN_2M:
		ret = RTW_DATA_RATE_CCK2;
		break;
	case MGN_5_5M:
		ret = RTW_DATA_RATE_CCK5_5;
		break;
	case MGN_11M:
		ret = RTW_DATA_RATE_CCK11;
		break;
	case MGN_6M:
		ret = RTW_DATA_RATE_OFDM6;
		break;
	case MGN_9M:
		ret = RTW_DATA_RATE_OFDM9;
		break;
	case MGN_12M:
		ret = RTW_DATA_RATE_OFDM12;
		break;
	case MGN_18M:
		ret = RTW_DATA_RATE_OFDM18;
		break;
	case MGN_24M:
		ret = RTW_DATA_RATE_OFDM24;
		break;
	case MGN_36M:
		ret = RTW_DATA_RATE_OFDM36;
		break;
	case MGN_48M:
		ret = RTW_DATA_RATE_OFDM48;
		break;
	case MGN_54M:
		ret = RTW_DATA_RATE_OFDM54;
		break;

	case MGN_MCS0:
		ret = RTW_DATA_RATE_MCS0;
		break;
	case MGN_MCS1:
		ret = RTW_DATA_RATE_MCS1;
		break;
	case MGN_MCS2:
		ret = RTW_DATA_RATE_MCS2;
		break;
	case MGN_MCS3:
		ret = RTW_DATA_RATE_MCS3;
		break;
	case MGN_MCS4:
		ret = RTW_DATA_RATE_MCS4;
		break;
	case MGN_MCS5:
		ret = RTW_DATA_RATE_MCS5;
		break;
	case MGN_MCS6:
		ret = RTW_DATA_RATE_MCS6;
		break;
	case MGN_MCS7:
		ret = RTW_DATA_RATE_MCS7;
		break;
	case MGN_MCS8:
		ret = RTW_DATA_RATE_MCS8;
		break;
	case MGN_MCS9:
		ret = RTW_DATA_RATE_MCS9;
		break;
	case MGN_MCS10:
		ret = RTW_DATA_RATE_MCS10;
		break;
	case MGN_MCS11:
		ret = RTW_DATA_RATE_MCS11;
		break;
	case MGN_MCS12:
		ret = RTW_DATA_RATE_MCS12;
		break;
	case MGN_MCS13:
		ret = RTW_DATA_RATE_MCS13;
		break;
	case MGN_MCS14:
		ret = RTW_DATA_RATE_MCS14;
		break;
	case MGN_MCS15:
		ret = RTW_DATA_RATE_MCS15;
		break;
	case MGN_MCS16:
		ret = RTW_DATA_RATE_MCS16;
		break;
	case MGN_MCS17:
		ret = RTW_DATA_RATE_MCS17;
		break;
	case MGN_MCS18:
		ret = RTW_DATA_RATE_MCS18;
		break;
	case MGN_MCS19:
		ret = RTW_DATA_RATE_MCS19;
		break;
	case MGN_MCS20:
		ret = RTW_DATA_RATE_MCS20;
		break;
	case MGN_MCS21:
		ret = RTW_DATA_RATE_MCS21;
		break;
	case MGN_MCS22:
		ret = RTW_DATA_RATE_MCS22;
		break;
	case MGN_MCS23:
		ret = RTW_DATA_RATE_MCS23;
		break;
	case MGN_MCS24:
		ret = RTW_DATA_RATE_MCS24;
		break;
	case MGN_MCS25:
		ret = RTW_DATA_RATE_MCS25;
		break;
	case MGN_MCS26:
		ret = RTW_DATA_RATE_MCS26;
		break;
	case MGN_MCS27:
		ret = RTW_DATA_RATE_MCS27;
		break;
	case MGN_MCS28:
		ret = RTW_DATA_RATE_MCS28;
		break;
	case MGN_MCS29:
		ret = RTW_DATA_RATE_MCS29;
		break;
	case MGN_MCS30:
		ret = RTW_DATA_RATE_MCS30;
		break;
	case MGN_MCS31:
		ret = RTW_DATA_RATE_MCS31;
		break;

	case MGN_VHT1SS_MCS0:
		ret = RTW_DATA_RATE_VHT_NSS1_MCS0;
		break;
	case MGN_VHT1SS_MCS1:
		ret = RTW_DATA_RATE_VHT_NSS1_MCS1;
		break;
	case MGN_VHT1SS_MCS2:
		ret = RTW_DATA_RATE_VHT_NSS1_MCS2;
		break;
	case MGN_VHT1SS_MCS3:
		ret = RTW_DATA_RATE_VHT_NSS1_MCS3;
		break;
	case MGN_VHT1SS_MCS4:
		ret = RTW_DATA_RATE_VHT_NSS1_MCS4;
		break;
	case MGN_VHT1SS_MCS5:
		ret = RTW_DATA_RATE_VHT_NSS1_MCS5;
		break;
	case MGN_VHT1SS_MCS6:
		ret = RTW_DATA_RATE_VHT_NSS1_MCS6;
		break;
	case MGN_VHT1SS_MCS7:
		ret = RTW_DATA_RATE_VHT_NSS1_MCS7;
		break;
	case MGN_VHT1SS_MCS8:
		ret = RTW_DATA_RATE_VHT_NSS1_MCS8;
		break;
	case MGN_VHT1SS_MCS9:
		ret = RTW_DATA_RATE_VHT_NSS1_MCS9;
		break;
	case MGN_VHT2SS_MCS0:
		ret = RTW_DATA_RATE_VHT_NSS2_MCS0;
		break;
	case MGN_VHT2SS_MCS1:
		ret = RTW_DATA_RATE_VHT_NSS2_MCS1;
		break;
	case MGN_VHT2SS_MCS2:
		ret = RTW_DATA_RATE_VHT_NSS2_MCS2;
		break;
	case MGN_VHT2SS_MCS3:
		ret = RTW_DATA_RATE_VHT_NSS2_MCS3;
		break;
	case MGN_VHT2SS_MCS4:
		ret = RTW_DATA_RATE_VHT_NSS2_MCS4;
		break;
	case MGN_VHT2SS_MCS5:
		ret = RTW_DATA_RATE_VHT_NSS2_MCS5;
		break;
	case MGN_VHT2SS_MCS6:
		ret = RTW_DATA_RATE_VHT_NSS2_MCS6;
		break;
	case MGN_VHT2SS_MCS7:
		ret = RTW_DATA_RATE_VHT_NSS2_MCS7;
		break;
	case MGN_VHT2SS_MCS8:
		ret = RTW_DATA_RATE_VHT_NSS2_MCS8;
		break;
	case MGN_VHT2SS_MCS9:
		ret = RTW_DATA_RATE_VHT_NSS2_MCS9;
		break;
	case MGN_VHT3SS_MCS0:
		ret = RTW_DATA_RATE_VHT_NSS3_MCS0;
		break;
	case MGN_VHT3SS_MCS1:
		ret = RTW_DATA_RATE_VHT_NSS3_MCS1;
		break;
	case MGN_VHT3SS_MCS2:
		ret = RTW_DATA_RATE_VHT_NSS3_MCS2;
		break;
	case MGN_VHT3SS_MCS3:
		ret = RTW_DATA_RATE_VHT_NSS3_MCS3;
		break;
	case MGN_VHT3SS_MCS4:
		ret = RTW_DATA_RATE_VHT_NSS3_MCS4;
		break;
	case MGN_VHT3SS_MCS5:
		ret = RTW_DATA_RATE_VHT_NSS3_MCS5;
		break;
	case MGN_VHT3SS_MCS6:
		ret = RTW_DATA_RATE_VHT_NSS3_MCS6;
		break;
	case MGN_VHT3SS_MCS7:
		ret = RTW_DATA_RATE_VHT_NSS3_MCS7;
		break;
	case MGN_VHT3SS_MCS8:
		ret = RTW_DATA_RATE_VHT_NSS3_MCS8;
		break;
	case MGN_VHT3SS_MCS9:
		ret = RTW_DATA_RATE_VHT_NSS3_MCS9;
		break;
	case MGN_VHT4SS_MCS0:
		ret = RTW_DATA_RATE_VHT_NSS4_MCS0;
		break;
	case MGN_VHT4SS_MCS1:
		ret = RTW_DATA_RATE_VHT_NSS4_MCS1;
		break;
	case MGN_VHT4SS_MCS2:
		ret = RTW_DATA_RATE_VHT_NSS4_MCS2;
		break;
	case MGN_VHT4SS_MCS3:
		ret = RTW_DATA_RATE_VHT_NSS4_MCS3;
		break;
	case MGN_VHT4SS_MCS4:
		ret = RTW_DATA_RATE_VHT_NSS4_MCS4;
		break;
	case MGN_VHT4SS_MCS5:
		ret = RTW_DATA_RATE_VHT_NSS4_MCS5;
		break;
	case MGN_VHT4SS_MCS6:
		ret = RTW_DATA_RATE_VHT_NSS4_MCS6;
		break;
	case MGN_VHT4SS_MCS7:
		ret = RTW_DATA_RATE_VHT_NSS4_MCS7;
		break;
	case MGN_VHT4SS_MCS8:
		ret = RTW_DATA_RATE_VHT_NSS4_MCS8;
		break;
	case MGN_VHT4SS_MCS9:
		ret = RTW_DATA_RATE_VHT_NSS4_MCS9;
		break;

	case MGN_HE1SS_MCS0:
		ret = RTW_DATA_RATE_HE_NSS1_MCS0;
		break;
	case MGN_HE1SS_MCS1:
		ret = RTW_DATA_RATE_HE_NSS1_MCS1;
		break;
	case MGN_HE1SS_MCS2:
		ret = RTW_DATA_RATE_HE_NSS1_MCS2;
		break;
	case MGN_HE1SS_MCS3:
		ret = RTW_DATA_RATE_HE_NSS1_MCS3;
		break;
	case MGN_HE1SS_MCS4:
		ret = RTW_DATA_RATE_HE_NSS1_MCS4;
		break;
	case MGN_HE1SS_MCS5:
		ret = RTW_DATA_RATE_HE_NSS1_MCS5;
		break;
	case MGN_HE1SS_MCS6:
		ret = RTW_DATA_RATE_HE_NSS1_MCS6;
		break;
	case MGN_HE1SS_MCS7:
		ret = RTW_DATA_RATE_HE_NSS1_MCS7;
		break;
	case MGN_HE1SS_MCS8:
		ret = RTW_DATA_RATE_HE_NSS1_MCS8;
		break;
	case MGN_HE1SS_MCS9:
		ret = RTW_DATA_RATE_HE_NSS1_MCS9;
		break;
	case MGN_HE1SS_MCS10:
		ret = RTW_DATA_RATE_HE_NSS1_MCS10;
		break;
	case MGN_HE1SS_MCS11:
		ret = RTW_DATA_RATE_HE_NSS1_MCS11;
		break;
	case MGN_HE2SS_MCS0:
		ret = RTW_DATA_RATE_HE_NSS2_MCS0;
		break;
	case MGN_HE2SS_MCS1:
		ret = RTW_DATA_RATE_HE_NSS2_MCS1;
		break;
	case MGN_HE2SS_MCS2:
		ret = RTW_DATA_RATE_HE_NSS2_MCS2;
		break;
	case MGN_HE2SS_MCS3:
		ret = RTW_DATA_RATE_HE_NSS2_MCS3;
		break;
	case MGN_HE2SS_MCS4:
		ret = RTW_DATA_RATE_HE_NSS2_MCS4;
		break;
	case MGN_HE2SS_MCS5:
		ret = RTW_DATA_RATE_HE_NSS2_MCS5;
		break;
	case MGN_HE2SS_MCS6:
		ret = RTW_DATA_RATE_HE_NSS2_MCS6;
		break;
	case MGN_HE2SS_MCS7:
		ret = RTW_DATA_RATE_HE_NSS2_MCS7;
		break;
	case MGN_HE2SS_MCS8:
		ret = RTW_DATA_RATE_HE_NSS2_MCS8;
		break;
	case MGN_HE2SS_MCS9:
		ret = RTW_DATA_RATE_HE_NSS2_MCS9;
		break;
	case MGN_HE2SS_MCS10:
		ret = RTW_DATA_RATE_HE_NSS2_MCS10;
		break;
	case MGN_HE2SS_MCS11:
		ret = RTW_DATA_RATE_HE_NSS2_MCS11;
		break;
	case MGN_HE3SS_MCS0:
		ret = RTW_DATA_RATE_HE_NSS3_MCS0;
		break;
	case MGN_HE3SS_MCS1:
		ret = RTW_DATA_RATE_HE_NSS3_MCS1;
		break;
	case MGN_HE3SS_MCS2:
		ret = RTW_DATA_RATE_HE_NSS3_MCS2;
		break;
	case MGN_HE3SS_MCS3:
		ret = RTW_DATA_RATE_HE_NSS3_MCS3;
		break;
	case MGN_HE3SS_MCS4:
		ret = RTW_DATA_RATE_HE_NSS3_MCS4;
		break;
	case MGN_HE3SS_MCS5:
		ret = RTW_DATA_RATE_HE_NSS3_MCS5;
		break;
	case MGN_HE3SS_MCS6:
		ret = RTW_DATA_RATE_HE_NSS3_MCS6;
		break;
	case MGN_HE3SS_MCS7:
		ret = RTW_DATA_RATE_HE_NSS3_MCS7;
		break;
	case MGN_HE3SS_MCS8:
		ret = RTW_DATA_RATE_HE_NSS3_MCS8;
		break;
	case MGN_HE3SS_MCS9:
		ret = RTW_DATA_RATE_HE_NSS3_MCS9;
		break;
	case MGN_HE3SS_MCS10:
		ret = RTW_DATA_RATE_HE_NSS3_MCS10;
		break;
	case MGN_HE3SS_MCS11:
		ret = RTW_DATA_RATE_HE_NSS3_MCS11;
		break;
	case MGN_HE4SS_MCS0:
		ret = RTW_DATA_RATE_HE_NSS4_MCS0;
		break;
	case MGN_HE4SS_MCS1:
		ret = RTW_DATA_RATE_HE_NSS4_MCS1;
		break;
	case MGN_HE4SS_MCS2:
		ret = RTW_DATA_RATE_HE_NSS4_MCS2;
		break;
	case MGN_HE4SS_MCS3:
		ret = RTW_DATA_RATE_HE_NSS4_MCS3;
		break;
	case MGN_HE4SS_MCS4:
		ret = RTW_DATA_RATE_HE_NSS4_MCS4;
		break;
	case MGN_HE4SS_MCS5:
		ret = RTW_DATA_RATE_HE_NSS4_MCS5;
		break;
	case MGN_HE4SS_MCS6:
		ret = RTW_DATA_RATE_HE_NSS4_MCS6;
		break;
	case MGN_HE4SS_MCS7:
		ret = RTW_DATA_RATE_HE_NSS4_MCS7;
		break;
	case MGN_HE4SS_MCS8:
		ret = RTW_DATA_RATE_HE_NSS4_MCS8;
		break;
	case MGN_HE4SS_MCS9:
		ret = RTW_DATA_RATE_HE_NSS4_MCS9;
		break;
	case MGN_HE4SS_MCS10:
		ret = RTW_DATA_RATE_HE_NSS4_MCS10;
		break;
	case MGN_HE4SS_MCS11:
		ret = RTW_DATA_RATE_HE_NSS4_MCS11;
		break;
	default:
		break;
	}

	return ret;
}

static inline u8 hwrate_to_mrate(u16 rate)
{
	u16 ret_rate = MGN_1M;

	switch (rate) {

	case RTW_DATA_RATE_CCK1:
		ret_rate = MGN_1M;
		break;
	case RTW_DATA_RATE_CCK2:
		ret_rate = MGN_2M;
		break;
	case RTW_DATA_RATE_CCK5_5:
		ret_rate = MGN_5_5M;
		break;
	case RTW_DATA_RATE_CCK11:
		ret_rate = MGN_11M;
		break;
	case RTW_DATA_RATE_OFDM6:
		ret_rate = MGN_6M;
		break;
	case RTW_DATA_RATE_OFDM9:
		ret_rate = MGN_9M;
		break;
	case RTW_DATA_RATE_OFDM12:
		ret_rate = MGN_12M;
		break;
	case RTW_DATA_RATE_OFDM18:
		ret_rate = MGN_18M;
		break;
	case RTW_DATA_RATE_OFDM24:
		ret_rate = MGN_24M;
		break;
	case RTW_DATA_RATE_OFDM36:
		ret_rate = MGN_36M;
		break;
	case RTW_DATA_RATE_OFDM48:
		ret_rate = MGN_48M;
		break;
	case RTW_DATA_RATE_OFDM54:
		ret_rate = MGN_54M;
		break;
	case RTW_DATA_RATE_MCS0:
		ret_rate = MGN_MCS0;
		break;
	case RTW_DATA_RATE_MCS1:
		ret_rate = MGN_MCS1;
		break;
	case RTW_DATA_RATE_MCS2:
		ret_rate = MGN_MCS2;
		break;
	case RTW_DATA_RATE_MCS3:
		ret_rate = MGN_MCS3;
		break;
	case RTW_DATA_RATE_MCS4:
		ret_rate = MGN_MCS4;
		break;
	case RTW_DATA_RATE_MCS5:
		ret_rate = MGN_MCS5;
		break;
	case RTW_DATA_RATE_MCS6:
		ret_rate = MGN_MCS6;
		break;
	case RTW_DATA_RATE_MCS7:
		ret_rate = MGN_MCS7;
		break;
	case RTW_DATA_RATE_MCS8:
		ret_rate = MGN_MCS8;
		break;
	case RTW_DATA_RATE_MCS9:
		ret_rate = MGN_MCS9;
		break;
	case RTW_DATA_RATE_MCS10:
		ret_rate = MGN_MCS10;
		break;
	case RTW_DATA_RATE_MCS11:
		ret_rate = MGN_MCS11;
		break;
	case RTW_DATA_RATE_MCS12:
		ret_rate = MGN_MCS12;
		break;
	case RTW_DATA_RATE_MCS13:
		ret_rate = MGN_MCS13;
		break;
	case RTW_DATA_RATE_MCS14:
		ret_rate = MGN_MCS14;
		break;
	case RTW_DATA_RATE_MCS15:
		ret_rate = MGN_MCS15;
		break;
	case RTW_DATA_RATE_MCS16:
		ret_rate = MGN_MCS16;
		break;
	case RTW_DATA_RATE_MCS17:
		ret_rate = MGN_MCS17;
		break;
	case RTW_DATA_RATE_MCS18:
		ret_rate = MGN_MCS18;
		break;
	case RTW_DATA_RATE_MCS19:
		ret_rate = MGN_MCS19;
		break;
	case RTW_DATA_RATE_MCS20:
		ret_rate = MGN_MCS20;
		break;
	case RTW_DATA_RATE_MCS21:
		ret_rate = MGN_MCS21;
		break;
	case RTW_DATA_RATE_MCS22:
		ret_rate = MGN_MCS22;
		break;
	case RTW_DATA_RATE_MCS23:
		ret_rate = MGN_MCS23;
		break;
	case RTW_DATA_RATE_MCS24:
		ret_rate = MGN_MCS24;
		break;
	case RTW_DATA_RATE_MCS25:
		ret_rate = MGN_MCS25;
		break;
	case RTW_DATA_RATE_MCS26:
		ret_rate = MGN_MCS26;
		break;
	case RTW_DATA_RATE_MCS27:
		ret_rate = MGN_MCS27;
		break;
	case RTW_DATA_RATE_MCS28:
		ret_rate = MGN_MCS28;
		break;
	case RTW_DATA_RATE_MCS29:
		ret_rate = MGN_MCS29;
		break;
	case RTW_DATA_RATE_MCS30:
		ret_rate = MGN_MCS30;
		break;
	case RTW_DATA_RATE_MCS31:
		ret_rate = MGN_MCS31;
		break;
	case RTW_DATA_RATE_VHT_NSS1_MCS0:
		ret_rate = MGN_VHT1SS_MCS0;
		break;
	case RTW_DATA_RATE_VHT_NSS1_MCS1:
		ret_rate = MGN_VHT1SS_MCS1;
		break;
	case RTW_DATA_RATE_VHT_NSS1_MCS2:
		ret_rate = MGN_VHT1SS_MCS2;
		break;
	case RTW_DATA_RATE_VHT_NSS1_MCS3:
		ret_rate = MGN_VHT1SS_MCS3;
		break;
	case RTW_DATA_RATE_VHT_NSS1_MCS4:
		ret_rate = MGN_VHT1SS_MCS4;
		break;
	case RTW_DATA_RATE_VHT_NSS1_MCS5:
		ret_rate = MGN_VHT1SS_MCS5;
		break;
	case RTW_DATA_RATE_VHT_NSS1_MCS6:
		ret_rate = MGN_VHT1SS_MCS6;
		break;
	case RTW_DATA_RATE_VHT_NSS1_MCS7:
		ret_rate = MGN_VHT1SS_MCS7;
		break;
	case RTW_DATA_RATE_VHT_NSS1_MCS8:
		ret_rate = MGN_VHT1SS_MCS8;
		break;
	case RTW_DATA_RATE_VHT_NSS1_MCS9:
		ret_rate = MGN_VHT1SS_MCS9;
		break;
	case RTW_DATA_RATE_VHT_NSS2_MCS0:
		ret_rate = MGN_VHT2SS_MCS0;
		break;
	case RTW_DATA_RATE_VHT_NSS2_MCS1:
		ret_rate = MGN_VHT2SS_MCS1;
		break;
	case RTW_DATA_RATE_VHT_NSS2_MCS2:
		ret_rate = MGN_VHT2SS_MCS2;
		break;
	case RTW_DATA_RATE_VHT_NSS2_MCS3:
		ret_rate = MGN_VHT2SS_MCS3;
		break;
	case RTW_DATA_RATE_VHT_NSS2_MCS4:
		ret_rate = MGN_VHT2SS_MCS4;
		break;
	case RTW_DATA_RATE_VHT_NSS2_MCS5:
		ret_rate = MGN_VHT2SS_MCS5;
		break;
	case RTW_DATA_RATE_VHT_NSS2_MCS6:
		ret_rate = MGN_VHT2SS_MCS6;
		break;
	case RTW_DATA_RATE_VHT_NSS2_MCS7:
		ret_rate = MGN_VHT2SS_MCS7;
		break;
	case RTW_DATA_RATE_VHT_NSS2_MCS8:
		ret_rate = MGN_VHT2SS_MCS8;
		break;
	case RTW_DATA_RATE_VHT_NSS2_MCS9:
		ret_rate = MGN_VHT2SS_MCS9;
		break;
	case RTW_DATA_RATE_VHT_NSS3_MCS0:
		ret_rate = MGN_VHT3SS_MCS0;
		break;
	case RTW_DATA_RATE_VHT_NSS3_MCS1:
		ret_rate = MGN_VHT3SS_MCS1;
		break;
	case RTW_DATA_RATE_VHT_NSS3_MCS2:
		ret_rate = MGN_VHT3SS_MCS2;
		break;
	case RTW_DATA_RATE_VHT_NSS3_MCS3:
		ret_rate = MGN_VHT3SS_MCS3;
		break;
	case RTW_DATA_RATE_VHT_NSS3_MCS4:
		ret_rate = MGN_VHT3SS_MCS4;
		break;
	case RTW_DATA_RATE_VHT_NSS3_MCS5:
		ret_rate = MGN_VHT3SS_MCS5;
		break;
	case RTW_DATA_RATE_VHT_NSS3_MCS6:
		ret_rate = MGN_VHT3SS_MCS6;
		break;
	case RTW_DATA_RATE_VHT_NSS3_MCS7:
		ret_rate = MGN_VHT3SS_MCS7;
		break;
	case RTW_DATA_RATE_VHT_NSS3_MCS8:
		ret_rate = MGN_VHT3SS_MCS8;
		break;
	case RTW_DATA_RATE_VHT_NSS3_MCS9:
		ret_rate = MGN_VHT3SS_MCS9;
		break;
	case RTW_DATA_RATE_VHT_NSS4_MCS0:
		ret_rate = MGN_VHT4SS_MCS0;
		break;
	case RTW_DATA_RATE_VHT_NSS4_MCS1:
		ret_rate = MGN_VHT4SS_MCS1;
		break;
	case RTW_DATA_RATE_VHT_NSS4_MCS2:
		ret_rate = MGN_VHT4SS_MCS2;
		break;
	case RTW_DATA_RATE_VHT_NSS4_MCS3:
		ret_rate = MGN_VHT4SS_MCS3;
		break;
	case RTW_DATA_RATE_VHT_NSS4_MCS4:
		ret_rate = MGN_VHT4SS_MCS4;
		break;
	case RTW_DATA_RATE_VHT_NSS4_MCS5:
		ret_rate = MGN_VHT4SS_MCS5;
		break;
	case RTW_DATA_RATE_VHT_NSS4_MCS6:
		ret_rate = MGN_VHT4SS_MCS6;
		break;
	case RTW_DATA_RATE_VHT_NSS4_MCS7:
		ret_rate = MGN_VHT4SS_MCS7;
		break;
	case RTW_DATA_RATE_VHT_NSS4_MCS8:
		ret_rate = MGN_VHT4SS_MCS8;
		break;
	case RTW_DATA_RATE_VHT_NSS4_MCS9:
		ret_rate = MGN_VHT4SS_MCS9;
		break;
	case RTW_DATA_RATE_HE_NSS1_MCS0:
		ret_rate = MGN_HE1SS_MCS0;
		break;
	case RTW_DATA_RATE_HE_NSS1_MCS1:
		ret_rate = MGN_HE1SS_MCS1;
		break;
	case RTW_DATA_RATE_HE_NSS1_MCS2:
		ret_rate = MGN_HE1SS_MCS2;
		break;
	case RTW_DATA_RATE_HE_NSS1_MCS3:
		ret_rate = MGN_HE1SS_MCS3;
		break;
	case RTW_DATA_RATE_HE_NSS1_MCS4:
		ret_rate = MGN_HE1SS_MCS4;
		break;
	case RTW_DATA_RATE_HE_NSS1_MCS5:
		ret_rate = MGN_HE1SS_MCS5;
		break;
	case RTW_DATA_RATE_HE_NSS1_MCS6:
		ret_rate = MGN_HE1SS_MCS6;
		break;
	case RTW_DATA_RATE_HE_NSS1_MCS7:
		ret_rate = MGN_HE1SS_MCS7;
		break;
	case RTW_DATA_RATE_HE_NSS1_MCS8:
		ret_rate = MGN_HE1SS_MCS8;
		break;
	case RTW_DATA_RATE_HE_NSS1_MCS9:
		ret_rate = MGN_HE1SS_MCS9;
		break;
	case RTW_DATA_RATE_HE_NSS1_MCS10:
		ret_rate = MGN_HE1SS_MCS10;
		break;
	case RTW_DATA_RATE_HE_NSS1_MCS11:
		ret_rate = MGN_HE1SS_MCS11;
		break;
	case RTW_DATA_RATE_HE_NSS2_MCS0:
		ret_rate = MGN_HE2SS_MCS0;
		break;
	case RTW_DATA_RATE_HE_NSS2_MCS1:
		ret_rate = MGN_HE2SS_MCS1;
		break;
	case RTW_DATA_RATE_HE_NSS2_MCS2:
		ret_rate = MGN_HE2SS_MCS2;
		break;
	case RTW_DATA_RATE_HE_NSS2_MCS3:
		ret_rate = MGN_HE2SS_MCS3;
		break;
	case RTW_DATA_RATE_HE_NSS2_MCS4:
		ret_rate = MGN_HE2SS_MCS4;
		break;
	case RTW_DATA_RATE_HE_NSS2_MCS5:
		ret_rate = MGN_HE2SS_MCS5;
		break;
	case RTW_DATA_RATE_HE_NSS2_MCS6:
		ret_rate = MGN_HE2SS_MCS6;
		break;
	case RTW_DATA_RATE_HE_NSS2_MCS7:
		ret_rate = MGN_HE2SS_MCS7;
		break;
	case RTW_DATA_RATE_HE_NSS2_MCS8:
		ret_rate = MGN_HE2SS_MCS8;
		break;
	case RTW_DATA_RATE_HE_NSS2_MCS9:
		ret_rate = MGN_HE2SS_MCS9;
		break;
	case RTW_DATA_RATE_HE_NSS2_MCS10:
		ret_rate = MGN_HE2SS_MCS10;
		break;
	case RTW_DATA_RATE_HE_NSS2_MCS11:
		ret_rate = MGN_HE2SS_MCS11;
		break;
	case RTW_DATA_RATE_HE_NSS3_MCS0:
		ret_rate = MGN_HE3SS_MCS0;
		break;
	case RTW_DATA_RATE_HE_NSS3_MCS1:
		ret_rate = MGN_HE3SS_MCS1;
		break;
	case RTW_DATA_RATE_HE_NSS3_MCS2:
		ret_rate = MGN_HE3SS_MCS2;
		break;
	case RTW_DATA_RATE_HE_NSS3_MCS3:
		ret_rate = MGN_HE3SS_MCS3;
		break;
	case RTW_DATA_RATE_HE_NSS3_MCS4:
		ret_rate = MGN_HE3SS_MCS4;
		break;
	case RTW_DATA_RATE_HE_NSS3_MCS5:
		ret_rate = MGN_HE3SS_MCS5;
		break;
	case RTW_DATA_RATE_HE_NSS3_MCS6:
		ret_rate = MGN_HE3SS_MCS6;
		break;
	case RTW_DATA_RATE_HE_NSS3_MCS7:
		ret_rate = MGN_HE3SS_MCS7;
		break;
	case RTW_DATA_RATE_HE_NSS3_MCS8:
		ret_rate = MGN_HE3SS_MCS8;
		break;
	case RTW_DATA_RATE_HE_NSS3_MCS9:
		ret_rate = MGN_HE3SS_MCS9;
		break;
	case RTW_DATA_RATE_HE_NSS3_MCS10:
		ret_rate = MGN_HE3SS_MCS10;
		break;
	case RTW_DATA_RATE_HE_NSS3_MCS11:
		ret_rate = MGN_HE3SS_MCS11;
		break;
	case RTW_DATA_RATE_HE_NSS4_MCS0:
		ret_rate = MGN_HE4SS_MCS0;
		break;
	case RTW_DATA_RATE_HE_NSS4_MCS1:
		ret_rate = MGN_HE4SS_MCS1;
		break;
	case RTW_DATA_RATE_HE_NSS4_MCS2:
		ret_rate = MGN_HE4SS_MCS2;
		break;
	case RTW_DATA_RATE_HE_NSS4_MCS3:
		ret_rate = MGN_HE4SS_MCS3;
		break;
	case RTW_DATA_RATE_HE_NSS4_MCS4:
		ret_rate = MGN_HE4SS_MCS4;
		break;
	case RTW_DATA_RATE_HE_NSS4_MCS5:
		ret_rate = MGN_HE4SS_MCS5;
		break;
	case RTW_DATA_RATE_HE_NSS4_MCS6:
		ret_rate = MGN_HE4SS_MCS6;
		break;
	case RTW_DATA_RATE_HE_NSS4_MCS7:
		ret_rate = MGN_HE4SS_MCS7;
		break;
	case RTW_DATA_RATE_HE_NSS4_MCS8:
		ret_rate = MGN_HE4SS_MCS8;
		break;
	case RTW_DATA_RATE_HE_NSS4_MCS9:
		ret_rate = MGN_HE4SS_MCS9;
		break;
	case RTW_DATA_RATE_HE_NSS4_MCS10:
		ret_rate = MGN_HE4SS_MCS10;
		break;
	case RTW_DATA_RATE_HE_NSS4_MCS11:
		ret_rate = MGN_HE4SS_MCS11;
		break;

	default:
		break;
	}

	return ret_rate;
}

#endif /* __HAL_RATE_H__ */

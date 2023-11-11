/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2021, Realtek Semiconductor Corp. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice, this
 *     list of conditions and the following disclaimer.
 *
 *   * Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   * Neither the name of the Realtek nor the names of its contributors may
 *     be used to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __HALBB_IC_HW_INFO_H__
#define __HALBB_IC_HW_INFO_H__

enum bb_ic_t {
	/*AC IC*/
	BB_RTL8852A	=	BIT(1), /*8852A > Bcut*/
	BB_RTL8852B	=	BIT(2),
	BB_RTL8852C	=	BIT(3),
	BB_RTL8834A	=	BIT(4),
	BB_RTL8192XB	=	BIT(5),
	BB_RTL8851B	=	BIT(6),
	/*BE IC*/
	BB_RTL8922A	=	BIT(16) /*1115*/
};

enum bb_ic_sub_t {
	BB_IC_SUB_TYPE_8852B_8852B	=	20,
	BB_IC_SUB_TYPE_8852B_8852BP,

	BB_IC_SUB_TYPE_8192XB_8192XB	=	50,
	BB_IC_SUB_TYPE_8192XB_8832BR,
	BB_IC_SUB_TYPE_8192XB_8832BR_VT
};

enum bb_cr_t {
	BB_AP		=	1,
	BB_AP2		=	2,
	BB_CLIENT	=	3,
	BB_BE0		=	4
};

enum bb_80211spec_t {
	BB_AX_IC	=	1,
	BB_BE_IC	=	2
};

#define BB_IC_N_1SS		0
#define BB_IC_N_2SS		0
#define BB_IC_N_3SS		0
#define BB_IC_N_4SS		0

#define BB_IC_AC_1SS		0
#define BB_IC_AC_2SS		0
#define BB_IC_AC_3SS		0
#define BB_IC_AC_4SS		0

#define BB_IC_AX_1SS		BB_RTL8851B
#define BB_IC_AX_2SS		(BB_RTL8852A | BB_RTL8852B | BB_RTL8852C |\
				 BB_RTL8192XB)
#define BB_IC_AX_3SS		0
#define BB_IC_AX_4SS		(BB_RTL8834A)

#define BB_IC_BE_1SS		0
#define BB_IC_BE_2SS		BB_RTL8922A
#define BB_IC_BE_3SS		0
#define BB_IC_BE_4SS		0

/*@====the following macro DO NOT need to update when adding a new IC======= */
#define BB_IC_1SS		(BB_IC_N_1SS | BB_IC_AC_1SS | BB_IC_AX_1SS | BB_IC_BE_1SS)
#define BB_IC_2SS		(BB_IC_N_2SS | BB_IC_AC_2SS | BB_IC_AX_2SS | BB_IC_BE_2SS)
#define BB_IC_3SS		(BB_IC_N_3SS | BB_IC_AC_3SS | BB_IC_AX_3SS | BB_IC_BE_3SS)
#define BB_IC_4SS		(BB_IC_N_4SS | BB_IC_AC_4SS | BB_IC_AX_4SS | BB_IC_BE_4SS)

#define BB_IC_ABOVE_1SS		(BB_IC_1SS | BB_IC_2SS | BB_IC_3SS |\
				BB_IC_4SS)
#define BB_IC_ABOVE_2SS		(BB_IC_2SS | BB_IC_3SS | BB_IC_4SS)
#define BB_IC_ABOVE_3SS		(BB_IC_3SS | BB_IC_4SS)
#define BB_IC_ABOVE_4SS		BB_IC_4SS

#define BB_IC_N_SERIES		(BB_IC_N_1SS | BB_IC_N_2SS | BB_IC_N_3SS |\
				 BB_IC_N_4SS)
#define BB_IC_AC_SERIES		(BB_IC_AC_1SS | BB_IC_AC_2SS |\
				 BB_IC_AC_3SS | BB_IC_AC_4SS)
#define BB_IC_AX_SERIES		(BB_IC_AX_1SS | BB_IC_AX_2SS |\
				 BB_IC_AX_3SS | BB_IC_AX_4SS)
#define BB_IC_BE_SERIES		(BB_IC_BE_1SS | BB_IC_BE_2SS |\
				 BB_IC_BE_3SS | BB_IC_BE_4SS)
/*@==========================================================================*/

#define BB_IC_AX_AP		(BB_RTL8852A | BB_RTL8834A)
#define BB_IC_AX_AP2		(BB_RTL8852C | BB_RTL8192XB)
#define BB_IC_AX_CLIENT		(BB_RTL8852B | BB_RTL8851B)
#define BB_IC_BE_0		(BB_RTL8922A)

/*@==========================================================================*/

#define BB_IC_MAX_BW_80		(BB_RTL8852A | BB_RTL8852B | BB_RTL8192XB |\
				 BB_RTL8851B)
#define BB_IC_MAX_BW_160	(BB_RTL8852C | BB_RTL8834A)
/*@==========================================================================*/

#define BB_IC_DBCC_LEGACY	(BB_RTL8852A | BB_RTL8852C)
#define BB_IC_DBCC_MLO		(BB_RTL8922A)

/*@==========================================================================*/
#if defined(BB_8852A_2_SUPPORT) || defined(BB_8852C_SUPPORT)
	#define HALBB_COMPILE_IC_DBCC_LEGACY
#endif

#if defined(BB_8922A_SUPPORT)
	#define HALBB_COMPILE_IC_DBCC_MLO
#endif

#if defined(HALBB_COMPILE_IC_DBCC_LEGACY) || defined(HALBB_COMPILE_IC_DBCC_MLO)
	#define HALBB_COMPILE_IC_DBCC
#endif

#if defined(BB_8852A_2_SUPPORT) || defined(BB_8852B_SUPPORT) || defined(BB_8852C_SUPPORT) || defined(BB_8851B_SUPPORT)
	/* FW OFFLOAD will be used in non-AP-only ICs*/
	#define HALBB_COMPILE_IC_FWOFLD
#endif

#if defined(BB_8851B_SUPPORT)
	#define HALBB_COMPILE_IC_1SS
#endif

#if (defined(BB_8852A_2_SUPPORT) || defined(BB_8852B_SUPPORT) ||\
     defined(BB_8852C_SUPPORT) || defined(BB_8192XB_SUPPORT) || defined(BB_8922A_SUPPORT))
	#define HALBB_COMPILE_IC_2SS
#endif

#if defined(RTL8853A_SUPPORT)
	#define HALBB_COMPILE_IC_3SS
#endif

#if defined(RTL8834A_SUPPORT)
	#define HALBB_COMPILE_IC_4SS
#endif

/*@==========================================================================*/
#if (defined(HALBB_COMPILE_IC_4SS))
	#define HALBB_COMPILE_ABOVE_4SS
#endif

#if (defined(HALBB_COMPILE_IC_3SS) || defined(HALBB_COMPILE_ABOVE_4SS))
	#define HALBB_COMPILE_ABOVE_3SS
#endif

#if (defined(HALBB_COMPILE_IC_2SS) || defined(HALBB_COMPILE_ABOVE_3SS) || defined(HALBB_COMPILE_ABOVE_4SS))
	#define HALBB_COMPILE_ABOVE_2SS
#endif

#if (defined(HALBB_COMPILE_IC_1SS) || defined(HALBB_COMPILE_ABOVE_2SS) || defined(HALBB_COMPILE_ABOVE_3SS) || defined(HALBB_COMPILE_ABOVE_4SS))
	#define HALBB_COMPILE_ABOVE_1SS
#endif

#if (defined(HALBB_COMPILE_ABOVE_4SS))
	#define HALBB_MAX_PATH	4
#elif (defined(HALBB_COMPILE_ABOVE_3SS))
	#define HALBB_MAX_PATH	3
#elif (defined(HALBB_COMPILE_ABOVE_2SS))
	#define HALBB_MAX_PATH	2
#else
	#define HALBB_MAX_PATH	1
#endif
/*@==========================================================================*/

#if (defined(BB_8852A_2_SUPPORT))
	#define HALBB_COMPILE_AP_SERIES
#endif

#if (defined(BB_8852C_SUPPORT) || defined(BB_8192XB_SUPPORT))
	#define HALBB_COMPILE_AP2_SERIES
#endif

#if (defined(BB_8852B_SUPPORT) || defined(BB_8851B_SUPPORT))
	#define HALBB_COMPILE_CLIENT_SERIES
#endif

#if (defined(BB_8922A_SUPPORT))
	#define HALBB_COMPILE_BE0_SERIES
#endif

#if (defined(HALBB_COMPILE_AP_SERIES)||defined(HALBB_COMPILE_AP2_SERIES)||defined(HALBB_COMPILE_CLIENT_SERIES))
	#define HALBB_COMPILE_AX_SERIOUS
#endif

#if (defined(BB_8922A_SUPPORT))
	#define HALBB_COMPILE_BE_SERIES
#endif

/*@==========================================================================*/

#if (defined(BB_8852C_SUPPORT))
#define HALBB_TW_DFS_SERIES
#endif

/*@==========================================================================*/
enum halbb_rate_type {
	BB_1SS			= 1,	/*HE/VHT/HT 1SS*/
	BB_2SS			= 2,	/*HE/VHT/HT 2SS*/
	BB_3SS			= 3,	/*HE/VHT/HT 3SS*/
	BB_4SS			= 4,	/*HE/VHT/HT 4SS*/
	BB_CCK			= 11,	/*B mode*/
	BB_OFDM			= 12	/*G mode*/
};

enum halbb_rate_table {
	BB_01M			= 0,
	BB_02M			= 1,
	BB_05M			= 2,
	BB_11M			= 3,
	BB_06M			= 4,
	BB_09M			= 5,
	BB_12M			= 6,
	BB_18M			= 7,
	BB_24M			= 8,
	BB_36M			= 9,
	BB_48M			= 10,
	BB_54M			= 11,
	BB_HT_MCS0		= 128,	/*0x1000000*/
	BB_VHT_1SS_MCS0	= 256,		/*0x2000000*/
	BB_VHT_2SS_MCS0	= 272,
	BB_VHT_3SS_MCS0	= 288,
	BB_VHT_4SS_MCS0	= 304,
	BB_VHT_5SS_MCS0	= 320,
	BB_VHT_6SS_MCS0	= 336,
	BB_VHT_7SS_MCS0	= 352,
	BB_VHT_8SS_MCS0	= 368,	
	BB_HE_1SS_MCS0		= 384,	/*0x3000000*/
	BB_HE_2SS_MCS0		= 400,
	BB_HE_3SS_MCS0		= 416,
	BB_HE_4SS_MCS0		= 432,
	BB_HE_5SS_MCS0		= 448,
	BB_HE_6SS_MCS0		= 464,
	BB_HE_7SS_MCS0		= 480,
	BB_HE_8SS_MCS0		= 496	
};

enum halbb_rate_table_be {
	BE_BB_01M			= 0,
	BE_BB_02M			= 1,
	BE_BB_05M			= 2,
	BE_BB_11M			= 3,
	BE_BB_06M			= 4,
	BE_BB_09M			= 5,
	BE_BB_12M			= 6,
	BE_BB_18M			= 7,
	BE_BB_24M			= 8,
	BE_BB_36M			= 9,
	BE_BB_48M			= 10,
	BE_BB_54M			= 11,
	BE_BB_HT_MCS0		= 256,	/*0x100*/
	BE_BB_VHT_1SS_MCS0	= 512,	/*0x200*/
	BE_BB_VHT_2SS_MCS0	= 544,
	BE_BB_VHT_3SS_MCS0	= 576,
	BE_BB_VHT_4SS_MCS0	= 608,
	BE_BB_VHT_5SS_MCS0	= 640,
	BE_BB_VHT_6SS_MCS0	= 672,
	BE_BB_VHT_7SS_MCS0	= 704,
	BE_BB_VHT_8SS_MCS0	= 736,
	BE_BB_HE_1SS_MCS0		= 768,	/*0x300*/
	BE_BB_HE_2SS_MCS0		= 800,
	BE_BB_HE_3SS_MCS0		= 832,
	BE_BB_HE_4SS_MCS0		= 864,
	BE_BB_HE_5SS_MCS0		= 896,
	BE_BB_HE_6SS_MCS0		= 928,
	BE_BB_HE_7SS_MCS0		= 950,
	BE_BB_HE_8SS_MCS0		= 982,
	BE_BB_EHT_1SS_MCS0		= 1024,	/*0x400*/
	BE_BB_EHT_2SS_MCS0		= 1056,
	BE_BB_EHT_3SS_MCS0		= 1088,
	BE_BB_EHT_4SS_MCS0		= 1120,
	BE_BB_EHT_5SS_MCS0		= 1152,
	BE_BB_EHT_6SS_MCS0		= 1184,
	BE_BB_EHT_7SS_MCS0		= 1216,
	BE_BB_EHT_8SS_MCS0		= 1248
};

enum halbb_legacy_spec_rate {
	BB_SPEC_RATE_6M		= 0xb,
	BB_SPEC_RATE_9M		= 0xf,
	BB_SPEC_RATE_12M	= 0xa,
	BB_SPEC_RATE_18M	= 0xe,
	BB_SPEC_RATE_24M	= 0x9,
	BB_SPEC_RATE_36M	= 0xd,
	BB_SPEC_RATE_48M	= 0x8,
	BB_SPEC_RATE_54M	= 0xc
};

#define GEN_HT_RATE_IDX(MCS) (0x80 | ((MCS) & 0x1f))
#define GEN_VHT_RATE_IDX(SS, MCS) (0x100 | (((SS) & 0x3) << 4) | ((MCS) & 0xf))
#define GEN_HE_RATE_IDX(SS, MCS) (0x180 | (((SS) & 0x3) << 4) | ((MCS) & 0xf))

#define BB_HT_MCS(x) 		(BB_HT_MCS0 + x)
#define BB_VHT_1SS_MCS(x)	(BB_VHT_1SS_MCS0 + x)
#define BB_VHT_2SS_MCS(x)	(BB_VHT_2SS_MCS0 + x)
#define BB_VHT_3SS_MCS(x)	(BB_VHT_3SS_MCS0 + x)
#define BB_VHT_4SS_MCS(x)	(BB_VHT_4SS_MCS0 + x)
#define BB_VHT_5SS_MCS(x)	(BB_VHT_5SS_MCS0 + x)
#define BB_VHT_6SS_MCS(x)	(BB_VHT_6SS_MCS0 + x)
#define BB_VHT_7SS_MCS(x)	(BB_VHT_7SS_MCS0 + x)
#define BB_VHT_8SS_MCS(x)	(BB_VHT_8SS_MCS0 + x)
#define BB_HE_1SS_MCS(x)	(BB_HE_1SS_MCS0 + x)
#define BB_HE_2SS_MCS(x)	(BB_HE_2SS_MCS0 + x)
#define BB_HE_3SS_MCS(x)	(BB_HE_3SS_MCS0 + x)
#define BB_HE_4SS_MCS(x)	(BB_HE_4SS_MCS0 + x)
#define BB_HE_5SS_MCS(x)	(BB_HE_5SS_MCS0 + x)
#define BB_HE_6SS_MCS(x)	(BB_HE_6SS_MCS0 + x)
#define BB_HE_7SS_MCS(x)	(BB_HE_7SS_MCS0 + x)
#define BB_HE_8SS_MCS(x)	(BB_HE_8SS_MCS0 + x)
#define BB_EHT_1SS_MCS(x)	(BE_BB_EHT_1SS_MCS0 + x)
#define BB_EHT_2SS_MCS(x)	(BE_BB_EHT_2SS_MCS0 + x)
#define BB_EHT_3SS_MCS(x)	(BE_BB_EHT_3SS_MCS0 + x)
#define BB_EHT_4SS_MCS(x)	(BE_BB_EHT_4SS_MCS0 + x)
#define BB_EHT_5SS_MCS(x)	(BE_BB_EHT_5SS_MCS0 + x)
#define BB_EHT_6SS_MCS(x)	(BE_BB_EHT_6SS_MCS0 + x)
#define BB_EHT_7SS_MCS(x)	(BE_BB_EHT_7SS_MCS0 + x)
#define BB_EHT_8SS_MCS(x)	(BE_BB_EHT_8SS_MCS0 + x)

/*For RTK AX ICs*/
#define BB_HT_MCS(x)		(BB_HT_MCS0 + x)
#define BB_VHT_MCS(SS, x)	(BB_VHT_1SS_MCS0 + ((SS - 1) * 16 ) + x)
#define BB_HE_MCS(SS, x)	(BB_HE_1SS_MCS0 + ((SS - 1) * 16 ) + x)
/*For RTK > BE ICs*/
#define BB_BE_HT_MCS(x)		(BE_BB_HT_MCS0 + x)
#define BB_BE_VHT_MCS(SS, x)	(BE_BB_VHT_1SS_MCS0 + ((SS - 1) << 5) + x)
#define BB_BE_HE_MCS(SS, x)	(BE_BB_HE_1SS_MCS0 + ((SS - 1) << 5) + x)
#define BB_BE_EHT_MCS(SS, x)	(BE_BB_EHT_1SS_MCS0 + ((SS - 1) << 5) + x)

/*[Rate Number]*/
#define	HT_NUM_MCS		8
#define	HE_VHT_NUM_MCS		12
#define	EHT_NUM_MCS		16
#define	LEGACY_RATE_NUM		12

#define	HT_RATE_NUM_4SS		(HT_NUM_MCS * 4)
#define	VHT_RATE_NUM_4SS	(HE_VHT_NUM_MCS * 4)
#define	HE_RATE_NUM_4SS		(HE_VHT_NUM_MCS * 4)
#define	EHT_RATE_NUM_4SS	(EHT_NUM_MCS * 4)

#define	HT_RATE_NUM_3SS		(HT_NUM_MCS * 3)
#define	VHT_RATE_NUM_3SS	(HE_VHT_NUM_MCS * 3)
#define	HE_RATE_NUM_3SS		(HE_VHT_NUM_MCS * 3)
#define	EHT_RATE_NUM_3SS	(EHT_NUM_MCS * 3)

#define	HT_RATE_NUM_2SS		(HT_NUM_MCS * 2)
#define	VHT_RATE_NUM_2SS	(HE_VHT_NUM_MCS * 2)
#define	HE_RATE_NUM_2SS		(HE_VHT_NUM_MCS * 2)
#define	EHT_RATE_NUM_2SS	(EHT_NUM_MCS * 2)

#define	HT_RATE_NUM_1SS		HT_NUM_MCS
#define	VHT_RATE_NUM_1SS	HE_VHT_NUM_MCS
#define	HE_RATE_NUM_1SS		HE_VHT_NUM_MCS
#define	EHT_RATE_NUM_1SS	EHT_NUM_MCS

#if (defined(HALBB_COMPILE_ABOVE_4SS))
	#define	HT_RATE_NUM	HT_RATE_NUM_4SS
	#define	VHT_RATE_NUM	VHT_RATE_NUM_4SS
	#define	HE_RATE_NUM	HE_RATE_NUM_4SS
	#define	EHT_RATE_NUM	EHT_RATE_NUM_4SS
#elif (defined(HALBB_COMPILE_ABOVE_3SS))
	#define	HT_RATE_NUM	HT_RATE_NUM_3SS
	#define	VHT_RATE_NUM	VHT_RATE_NUM_3SS
	#define	HE_RATE_NUM	HE_RATE_NUM_3SS
	#define	EHT_RATE_NUM	EHT_RATE_NUM_3SS
#elif (defined(HALBB_COMPILE_ABOVE_2SS))
	#define	HT_RATE_NUM	HT_RATE_NUM_2SS
	#define	VHT_RATE_NUM	VHT_RATE_NUM_2SS
	#define	HE_RATE_NUM	HE_RATE_NUM_2SS
	#define	EHT_RATE_NUM	EHT_RATE_NUM_2SS
#else
	#define	HT_RATE_NUM	HT_RATE_NUM_1SS
	#define	VHT_RATE_NUM	VHT_RATE_NUM_1SS
	#define	HE_RATE_NUM	HE_RATE_NUM_1SS
	#define	EHT_RATE_NUM	EHT_RATE_NUM_1SS
#endif

#define	LOW_BW_RATE_NUM		HE_RATE_NUM

/*@==========================================================================*/

/****************************************************************
 * 1 ============================================================
 * 1  enumeration
 * 1 ============================================================
 ***************************************************************/

enum bb_qam_type {
	BB_QAM_CCK	= 0,
	BB_QAM_BPSK	= 1,
	BB_QAM_QPSK	= 2,
	BB_QAM_16QAM	= 3,
	BB_QAM_64QAM	= 4,
	BB_QAM_256QAM 	= 5,
	BB_QAM_1024QAM	= 6
};

enum bb_mode_type {
	BB_LEGACY_MODE	= 0,
	BB_HT_MODE	= 1,
	BB_VHT_MODE	= 2,
	BB_HE_MODE	= 3,
	BB_EHT_MODE	= 4
};


/* BB_CMNINFO_CART_VER */
enum halbb_cart_ver {
	BB_CART_A		= 0,
	BB_CART_B		= 1,
	BB_CART_C		= 2,
	BB_CART_D		= 3,
	BB_CART_E		= 4,
	BB_CART_F		= 5,
	BB_CART_G		= 6,
	BB_CART_H		= 7,
	BB_CART_I		= 8,
	BB_CART_J		= 9,
	BB_CART_K		= 10,
	BB_CART_L		= 11,
	BB_CART_M		= 12,
	BB_CART_N		= 13,
	BB_CART_O		= 14,
	BB_CART_TEST		= 15,
};

enum bb_path {
	BB_PATH_NON	= 0,
	BB_PATH_A	= 0x00000001,
	BB_PATH_B	= 0x00000002,
	BB_PATH_C	= 0x00000004,
	BB_PATH_D	= 0x00000008,

	BB_PATH_AB	= (BB_PATH_A | BB_PATH_B),
	BB_PATH_AC	= (BB_PATH_A | BB_PATH_C),
	BB_PATH_AD	= (BB_PATH_A | BB_PATH_D),
	BB_PATH_BC	= (BB_PATH_B | BB_PATH_C),
	BB_PATH_BD	= (BB_PATH_B | BB_PATH_D),
	BB_PATH_CD	= (BB_PATH_C | BB_PATH_D),

	BB_PATH_ABC	= (BB_PATH_A | BB_PATH_B | BB_PATH_C),
	BB_PATH_ABD	= (BB_PATH_A | BB_PATH_B | BB_PATH_D),
	BB_PATH_ACD	= (BB_PATH_A | BB_PATH_C | BB_PATH_D),
	BB_PATH_BCD	= (BB_PATH_B | BB_PATH_C | BB_PATH_D),

	BB_PATH_ABCD	= (BB_PATH_A | BB_PATH_B | BB_PATH_C | BB_PATH_D),
	BB_PATH_AUTO	= 0xff /*for auto path selection*/
};

enum rf_syn {
	RF_SYN0 = 0,
	RF_SYN1 = 1,
};

#endif

/******************************************************************************
 *
 * Copyright(c) 2007 - 2020  Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * The full GNU General Public License is included in this distribution in the
 * file called LICENSE.
 *
 * Contact Information:
 * wlanfae <wlanfae@realtek.com>
 * Realtek Corporation, No. 2, Innovation Road II, Hsinchu Science Park,
 * Hsinchu 300, Taiwan.
 *
 * Larry Finger <Larry.Finger@lwfinger.net>
 *
 *****************************************************************************/
#ifndef __HALRF_IC_HW_INFO_H__
#define __HALRF_IC_HW_INFO_H__

enum halrf_ic {
	RF_RTL8852A	=	BIT(0),
	RF_RTL8852B	=	BIT(1),
	RF_RTL8834A	=	BIT(2),
	RF_RTL8852C	=	BIT(3),
	RF_RTL8832BR	=	BIT(4),
	RF_RTL8192XB	=	BIT(5),
	RF_RTL8852BP	=	BIT(6),
	RF_RTL8730A	=	BIT(7),
	RF_RTL8720E	=	BIT(8),
	RF_RTL8851B	=	BIT(9),
	RF_RTL8922A	=	BIT(10),
	RF_RTL8832CR_VU	=	BIT(11),	
	RF_RTL8832BR_VT	=	BIT(12),
};

enum halrf_subdid {
	RF_SUBDID_RTL8832CRVU = 0xc832,
	RF_SUBDID_RTL8832BRVT = 0xb83b,
};

#define RF_N_1SS		0
#define RF_N_2SS		0
#define RF_N_3SS		0
#define RF_N_4SS		0

#define RF_AC_1SS		0
#define RF_AC_2SS		0
#define RF_AC_3SS		0
#define RF_AC_4SS		0

#define RF_AX_1SS		(RF_RTL8851B)
#define RF_AX_2SS		(RF_RTL8852A | RF_RTL8852B | RF_RTL8852C |\
				 RF_RTL8832BR | RF_RTL8192XB | RF_RTL8852BP)
#define RF_AX_3SS		0
#define RF_AX_4SS		(RTL8834A)

/*@====the following macro DO NOT need to update when adding a new IC======= */
#define RF_1SS		(RF_N_1SS | RF_AC_1SS | RF_AX_1SS)
#define RF_2SS		(RF_N_2SS | RF_AC_2SS | RF_AX_2SS)
#define RF_3SS		(RF_N_3SS | RF_AC_3SS | RF_AX_3SS)
#define RF_4SS		(RF_N_4SS | RF_AC_4SS | RF_AX_4SS)


#define RF_N_SERIES		(RF_N_1SS | RF_N_2SS | RF_N_3SS |\
				 RF_N_4SS)
#define RF_AC_SERIES	(RF_AC_1SS | RF_AC_2SS |\
				 RF_AC_3SS | RF_AC_4SS)
#define RF_AX_SERIES	(RF_AX_1SS | RF_AX_2SS |\
				 RF_AX_3SS | RF_AX_4SS)
/*@==========================================================================*/
#if defined (RF_8852C_SUPPORT)
#define KIP_REG 3136
#elif defined (RF_8852B_SUPPORT)
#define KIP_REG 2048
#else
#define KIP_REG 2048
#endif

#if defined(RF_8834A_SUPPORT)
#define KPATH 4
#elif (defined(RF_8852A_SUPPORT) || defined(RF_8852B_SUPPORT) ||\
       defined(RF_8852C_SUPPORT) || defined(RF_8832BR_SUPPORT) ||\
       defined(RF_8192XB_SUPPORT) || defined(RF_8852BP_SUPPORT))
#define KPATH 2
#else
#define KPATH 1
#endif

/*@==========================================================================*/

/*@==========================================================================*/



/*@==========================================================================*/

/****************************************************************
 * 1 ============================================================
 * 1  enumeration
 * 1 ============================================================
 ***************************************************************/

enum rf_path_bit {
	RF_A	= BIT(0),
	RF_B	= BIT(1),
	RF_C	= BIT(2),
	RF_D	= BIT(3),

	RF_AB	= (RF_A | RF_B),
	RF_AC	= (RF_A | RF_C),
	RF_AD	= (RF_A | RF_D),
	RF_BC	= (RF_B | RF_C),
	RF_BD	= (RF_B | RF_D),
	RF_CD	= (RF_C | RF_D),

	RF_ABC	= (RF_A | RF_B | RF_C),
	RF_ABD	= (RF_A | RF_B | RF_D),
	RF_ACD	= (RF_A | RF_C | RF_D),
	RF_BCD	= (RF_B | RF_C | RF_D),

	RF_ABCD	= (RF_A | RF_B | RF_C | RF_D),
};

enum halrf_ant {
	RF_MAIN_ANT	= 1,
	RF_AUX_ANT	= 2,
};

#endif

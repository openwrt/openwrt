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

#ifndef _MAC_AX_HW_H_
#define _MAC_AX_HW_H_

#define BT_2_DW(B3, B2, B1, B0)	\
	(((B3) << 24) | ((B2) << 16) | ((B1) << 8) | (B0))

#define NIB_2_DW(B7, B6, B5, B4, B3, B2, B1, B0)	\
	((((B7) & 0xf) << 28) | (((B6) & 0xf) << 24) | \
	(((B5) & 0xf) << 20) | (((B4) & 0xf) << 16) | \
	(((B3) & 0xf) << 12) | (((B2) & 0xf) << 8) | \
	(((B1) & 0xf) << 4) | ((B0) & 0xf))

#include "../type.h"
#include "status.h"
#include "wowlan.h"
#include "tblupd.h"
#include "ser.h"
#include "../pcie_reg.h"

#if MAC_AX_SDIO_SUPPORT
#include "_sdio.h"
#endif

#if MAC_AX_PCIE_SUPPORT
#include "_pcie.h"
#endif

#if MAC_AX_USB_SUPPORT
#if MAC_AX_8852A_SUPPORT
#include "mac_8852a/_usb_8852a.h"
#endif
#if MAC_AX_8852B_SUPPORT
#include "mac_8852b/_usb_8852b.h"
#endif
#if MAC_AX_8852C_SUPPORT
#include "mac_8852c/_usb_8852c.h"
#endif
#if MAC_AX_8851B_SUPPORT
#include "mac_8851b/_usb_8851b.h"
#endif
#if MAC_AX_8851E_SUPPORT
#include "mac_8851e/_usb_8851e.h"
#endif
#if MAC_AX_8852D_SUPPORT
#include "mac_8852d/_usb_8852d.h"
#endif
#endif
/*--------------------Define -------------------------------------------*/

#define BITS_WLRF_CTRL 0x82
#define BITS_WLRF1_CTRL 0x8200
#define PHYREG_SET_ALL_CYCLE 0xC
#define PHYREG_SET_XYN_CYCLE 0xE
#define PHYREG_SET_X_CYCLE 0x4
#define PHYREG_SET_N_CYCLE 0x2
#define PHYREG_SET_Y_CYCLE 0x1

#define TXSC_80M 0x91
#define TXSC_40M 0x1
#define TXSC_20M 0x0

#define TBL_READ_OP 0x0
#define TBL_WRITE_OP 0x1
#define TXCNT_LMT_MSK 0x1

#define CHANNEL_5G 34

#define CR_TXCNT_MSK 0x7FFFFFFF

/*For XTAL_SI */
#define XTAL_SI_POLLING_CNT 1000
#define XTAL_SI_POLLING_DLY_US 50
#define XTAL_SI_NORMAL_WRITE 0x00
#define XTAL_SI_NORMAL_READ 0x01
#define XTAL_SI_XTAL_SC_XI 0x04
#define XTAL_SI_XTAL_SC_XO 0x05
#define XTAL_SI_XTAL_DRV 0x15
#define XTAL_SI_XTAL_XMD_2 0x24
#define XTAL_SI_XTAL_XMD_4 0x26
#define XTAL_SI_CV 0x41
#define XTAL0 0x0
#define XTAL3 0x3
#define XTAL_SI_WL_RFC_S0 0x80
#define XTAL_SI_WL_RFC_S1 0x81
#define XTAL_SI_ANAPAR_WL 0x90
#define XTAL_SI_GNT_CTRL 0x91
#define XTAL_SI_SRAM_CTRL 0xA1
#define FULL_BIT_MASK 0xFF

/*For ADC LDO setting*/
#define LDO2PW_LDO_VSEL 0x3600000

/* For TXPWR Usage*/
#define PWR_BY_RATE_LGCY_OFFSET 0XC0
#define PWR_BY_RATE_OFFSET 0XCC

#define PWR_LMT_CCK_OFFSET 0XEC
#define PWR_LMT_LGCY_OFFSET 0XF0
#define PWR_LMT_TBL2_OFFSET 0XF4
#define PWR_LMT_TBL5_OFFSET 0X100
#define PWR_LMT_TBL6_OFFSET 0X104
#define PWR_LMT_TBL7_OFFSET 0X108
#define PWR_LMT_TBL8_OFFSET 0X10C
#define PWR_LMT_TBL9_OFFSET 0X110

#define PWR_LMT_TBL_UNIT 0X28
#define PWR_BY_RATE_TBL_UNIT 0XF

#define POLL_SWR_VOLT_CNT 2
#define POLL_SWR_VOLT_US 50
#define CORE_SWR_VOLT_MAX 0xE
#define CORE_SWR_VOLT_MIN 0x8

/* For SS SRAM access*/
#define SS_R_QUOTA_SETTING  0
#define SS_W_QUOTA_SETTING  1
#define SS_R_QUOTA          2
#define SS_W_QUOTA          3
#define SS_R_TX_LEN         5
#define SS_R_DL_MURU_DIS    8
#define SS_W_DL_MURU_DIS    9
#define SS_R_UL_TBL         10
#define SS_W_UL_TBL         11
#define SS_R_BSR_LEN        12
#define SS_W_BSR_LEN        13

#define SS_QUOTA_SETTING_MSK    0xF
#define SS_MACID_SH		8

/* WDRLS filter map */
#define B_WDRLS_FLTR_TXOK BIT(24)
#define B_WDRLS_FLTR_RTYLMT BIT(25)
#define B_WDRLS_FLTR_LIFTIM BIT(26)
#define B_WDRLS_FLTR_MACID BIT(27)

#define MAC_AX_DRV_INFO_SIZE_0 0
#define MAC_AX_DRV_INFO_SIZE_16 1
#define MAC_AX_DRV_INFO_SIZE_32 2
#define MAC_AX_DRV_INFO_SIZE_48 3
#define MAC_AX_DRV_INFO_SIZE_64 4
#define MAC_AX_DRV_INFO_SIZE_80 5

#define CHK_BBRF_IO 0

/* Narrow BW*/
#define US_TIME_10M     0x50
#define US_TIME_5M      0x40
#define SLOTTIME_10M    0xD
#define SLOTTIME_5M     0x15
#define ACK_TO_10M      0x80
#define ACK_TO_5M       0xFF

#define MACID_NONE 0xFF

#define PCIE_5G6G_PER_GEN1GEN2_ANA21_VAL 0x3700
#define PCIE_5G6G_PER_GEN1GEN2_ANA23_VAL 0xFB66
#define PCIE_5G6G_PER_GEN1GEN2_ANA2F_VAL 0x3008

/*--------------------Define Enum---------------------------------------*/

/**
 * @enum tx_tf_info
 *
 * @brief tx_tf_info
 *
 * @var tx_tf_info::USER_INFO0_SEL
 * Please Place Description here.
 * @var tx_tf_info::USER_INFO1_SEL
 * Please Place Description here.
 * @var tx_tf_info::USER_INFO2_SEL
 * Please Place Description here.
 * @var tx_tf_info::USER_INFO3_SEL
 * Please Place Description here.
 * @var tx_tf_info::COMMON_INFO_SEL
 * Please Place Description here.
 */
enum tx_tf_info {
	USER_INFO0_SEL		= 0,
	USER_INFO1_SEL		= 1,
	USER_INFO2_SEL		= 2,
	USER_INFO3_SEL		= 3,
	COMMON_INFO_SEL		= 4,
};

enum addr_rang_idx {
	ADDR_AON = 0,
	ADDR_HCI,
	ADDR_PON,
	ADDR_DMAC,
	ADDR_CMAC0,
	ADDR_CMAC1,
	ADDR_BB0,
	ADDR_BB1,
	ADDR_RF,
	ADDR_IND_ACES,
	ADDR_RSVD,

	/* keep last */
	ADDR_LAST,
	ADDR_MAX = ADDR_LAST,
	ADDR_INVALID = ADDR_LAST
};

/*--------------------Define MACRO--------------------------------------*/
#define ADDR_IS_AON_8852A(addr) ((addr) <= 0x4FF ? 1 : 0)
#define ADDR_IS_AON_8852B(addr) ((addr) <= 0x4FF ? 1 : 0)
#define ADDR_IS_AON_8852C(addr) ((addr) <= 0x4FF ? 1 : 0)
#define ADDR_IS_AON_8192XB(addr) ((addr) <= 0x4FF ? 1 : 0)
#define ADDR_IS_AON_8851B(addr) ((addr) <= 0x4FF ? 1 : 0)
#define ADDR_IS_AON_8851E(addr) ((addr) <= 0x4FF ? 1 : 0)
#define ADDR_IS_AON_8852D(addr) ((addr) <= 0x4FF ? 1 : 0)

#define ADDR_IS_HCI_8852A(addr) \
	((addr) >= 0x1000 && (addr) <= 0x1FFF ? 1 : 0)
#define ADDR_IS_HCI_8852B(addr) \
	((addr) >= 0x1000 && (addr) <= 0x1FFF ? 1 : 0)
#define ADDR_IS_HCI_8852C(addr) \
	(((addr) >= 0x2000 && (addr) <= 0x63FF) || \
	 ((addr) >= 0x7C00 && (addr) <= 0x7FFF) ? 1 : 0)
#define ADDR_IS_HCI_8192XB(addr) \
	(((addr) >= 0x2000 && (addr) <= 0x63FF) || \
	 ((addr) >= 0x7C00 && (addr) <= 0x7FFF) ? 1 : 0)
#define ADDR_IS_HCI_8851B(addr) \
	((addr) >= 0x1000 && (addr) <= 0x1FFF ? 1 : 0)
#define ADDR_IS_HCI_8851E(addr) \
	(((addr) >= 0x2000 && (addr) <= 0x63FF) || \
	 ((addr) >= 0x7C00 && (addr) <= 0x7FFF) ? 1 : 0)
#define ADDR_IS_HCI_8852D(addr) \
	(((addr) >= 0x2000 && (addr) <= 0x63FF) || \
	 ((addr) >= 0x7C00 && (addr) <= 0x7FFF) ? 1 : 0)

#define ADDR_IS_PON_8852A(addr) \
	(((addr) >= 0x400 && (addr) <= 0xFFF) || \
	 ((addr) >= 0x8000 && (addr) <= 0x8407) || \
	 ((addr) >= 0xC000 && (addr) <= 0xC007) || \
	 ((addr) >= 0xE000 && (addr) <= 0xE007) ? 1 : 0)
#define ADDR_IS_PON_8852B(addr) \
	(((addr) >= 0x400 && (addr) <= 0xFFF) || \
	 ((addr) >= 0x8000 && (addr) <= 0x8407) || \
	 ((addr) >= 0xC000 && (addr) <= 0xC007) ? 1 : 0)
#define ADDR_IS_PON_8852C(addr) \
	(((addr) >= 0x400 && (addr) <= 0xFFF) || \
	 ((addr) >= 0x7000 && (addr) <= 0x7BFF) || \
	 ((addr) >= 0x8000 && (addr) <= 0x8407) || \
	 ((addr) >= 0xC000 && (addr) <= 0xC007) || \
	 ((addr) >= 0xE000 && (addr) <= 0xE007) ? 1 : 0)
#define ADDR_IS_PON_8192XB(addr) \
	(((addr) >= 0x400 && (addr) <= 0xFFF) || \
	 ((addr) >= 0x7000 && (addr) <= 0x7BFF) || \
	 ((addr) >= 0x8000 && (addr) <= 0x8407) || \
	 ((addr) >= 0xC000 && (addr) <= 0xC007) || \
	 ((addr) >= 0xE000 && (addr) <= 0xE007) ? 1 : 0)
#define ADDR_IS_PON_8851B(addr) \
	(((addr) >= 0x400 && (addr) <= 0xFFF) || \
	 ((addr) >= 0x8000 && (addr) <= 0x8407) || \
	 ((addr) >= 0xC000 && (addr) <= 0xC007) ? 1 : 0)
#define ADDR_IS_PON_8851E(addr) \
	(((addr) >= 0x400 && (addr) <= 0xFFF) || \
	 ((addr) >= 0x7000 && (addr) <= 0x7BFF) || \
	 ((addr) >= 0x8000 && (addr) <= 0x8407) || \
	 ((addr) >= 0xC000 && (addr) <= 0xC007) || \
	 ((addr) >= 0xE000 && (addr) <= 0xE007) ? 1 : 0)
#define ADDR_IS_PON_8852D(addr) \
	(((addr) >= 0x400 && (addr) <= 0xFFF) || \
	 ((addr) >= 0x7000 && (addr) <= 0x7BFF) || \
	 ((addr) >= 0x8000 && (addr) <= 0x8407) || \
	 ((addr) >= 0xC000 && (addr) <= 0xC007) || \
	 ((addr) >= 0xE000 && (addr) <= 0xE007) ? 1 : 0)

#define ADDR_IS_DMAC_8852A(addr) \
	((addr) >= 0x8408 && (addr) <= 0xBFFF ? 1 : 0)
#define ADDR_IS_DMAC_8852B(addr) \
	((addr) >= 0x8408 && (addr) <= 0xBFFF ? 1 : 0)
#define ADDR_IS_DMAC_8852C(addr) \
	(((addr) >= 0x1000 && (addr) <= 0x1FFF) || \
	 ((addr) >= 0x8408 && (addr) <= 0xBFFF) ? 1 : 0)
#define ADDR_IS_DMAC_8192XB(addr) \
	(((addr) >= 0x1000 && (addr) <= 0x1FFF) || \
	 ((addr) >= 0x8408 && (addr) <= 0xBFFF) ? 1 : 0)
#define ADDR_IS_DMAC_8851B(addr) \
	((addr) >= 0x8408 && (addr) <= 0xBFFF ? 1 : 0)
#define ADDR_IS_DMAC_8851E(addr) \
	(((addr) >= 0x1000 && (addr) <= 0x1FFF) || \
	 ((addr) >= 0x8408 && (addr) <= 0xBFFF) ? 1 : 0)
#define ADDR_IS_DMAC_8852D(addr) \
	(((addr) >= 0x1000 && (addr) <= 0x1FFF) || \
	 ((addr) >= 0x8408 && (addr) <= 0xBFFF) ? 1 : 0)

#define ADDR_IS_CMAC0_8852A(addr) \
	((addr) >= 0xC008 && (addr) <= 0xDFFF ? 1 : 0)
#define ADDR_IS_CMAC0_8852B(addr) \
	((addr) >= 0xC008 && (addr) <= 0xDFFF ? 1 : 0)
#define ADDR_IS_CMAC0_8852C(addr) \
	((addr) >= 0xC008 && (addr) <= 0xDFFF ? 1 : 0)
#define ADDR_IS_CMAC0_8192XB(addr) \
	((addr) >= 0xC008 && (addr) <= 0xDFFF ? 1 : 0)
#define ADDR_IS_CMAC0_8851B(addr) \
	((addr) >= 0xC008 && (addr) <= 0xDFFF ? 1 : 0)
#define ADDR_IS_CMAC0_8851E(addr) \
	((addr) >= 0xC008 && (addr) <= 0xDFFF ? 1 : 0)
#define ADDR_IS_CMAC0_8852D(addr) \
	((addr) >= 0xC008 && (addr) <= 0xDFFF ? 1 : 0)

#define ADDR_IS_CMAC1_8852A(addr) \
	((addr) >= 0xE008 && (addr) <= 0xFFFF ? 1 : 0)
#define ADDR_IS_CMAC1_8852B(addr) \
	((addr) >= 0xE008 && (addr) <= 0xFFFF ? 0 : 0)
#define ADDR_IS_CMAC1_8852C(addr) \
	((addr) >= 0xE008 && (addr) <= 0xFFFF ? 1 : 0)
#define ADDR_IS_CMAC1_8192XB(addr) \
	((addr) >= 0xE008 && (addr) <= 0xFFFF ? 1 : 0)
#define ADDR_IS_CMAC1_8851B(addr) \
	((addr) >= 0xE008 && (addr) <= 0xFFFF ? 0 : 0)
#define ADDR_IS_CMAC1_8851E(addr) \
	((addr) >= 0xE008 && (addr) <= 0xFFFF ? 1 : 0)
#define ADDR_IS_CMAC1_8852D(addr) \
	((addr) >= 0xE008 && (addr) <= 0xFFFF ? 1 : 0)

#define ADDR_IS_BB0_8852A(addr) \
	(((addr) >= 0x10000 && (addr) <= 0x125FF) || \
	 ((addr) >= 0x12E00 && (addr) <= 0x138FF) || \
	 ((addr) >= 0x13C00 && (addr) <= 0x15FFF) || \
	 ((addr) >= 0x17000 && (addr) <= 0x17FFF) ? 1 : 0)
#define ADDR_IS_BB0_8852B(addr) \
	(((addr) >= 0x10000 && (addr) <= 0x125FF) || \
	 ((addr) >= 0x12E00 && (addr) <= 0x138FF) || \
	 ((addr) >= 0x13C00 && (addr) <= 0x15FFF) || \
	 ((addr) >= 0x17000 && (addr) <= 0x17FFF) ? 1 : 0)
#define ADDR_IS_BB0_8852C(addr) \
	(((addr) >= 0x10000 && (addr) <= 0x125FF) || \
	 ((addr) >= 0x12E00 && (addr) <= 0x138FF) || \
	 ((addr) >= 0x13C00 && (addr) <= 0x15FFF) || \
	 ((addr) >= 0x17000 && (addr) <= 0x17FFF) ? 1 : 0)
#define ADDR_IS_BB0_8192XB(addr) \
	(((addr) >= 0x10000 && (addr) <= 0x125FF) || \
	 ((addr) >= 0x12E00 && (addr) <= 0x138FF) || \
	 ((addr) >= 0x13C00 && (addr) <= 0x15FFF) || \
	 ((addr) >= 0x17000 && (addr) <= 0x17FFF) ? 1 : 0)
#define ADDR_IS_BB0_8851B(addr) \
	(((addr) >= 0x10000 && (addr) <= 0x125FF) || \
	 ((addr) >= 0x12E00 && (addr) <= 0x138FF) || \
	 ((addr) >= 0x13C00 && (addr) <= 0x15FFF) || \
	 ((addr) >= 0x17000 && (addr) <= 0x17FFF) ? 1 : 0)
#define ADDR_IS_BB0_8851E(addr) \
	(((addr) >= 0x10000 && (addr) <= 0x125FF) || \
	 ((addr) >= 0x12E00 && (addr) <= 0x138FF) || \
	 ((addr) >= 0x13C00 && (addr) <= 0x15FFF) || \
	 ((addr) >= 0x17000 && (addr) <= 0x17FFF) ? 1 : 0)
#define ADDR_IS_BB0_8852D(addr) \
	(((addr) >= 0x10000 && (addr) <= 0x125FF) || \
	 ((addr) >= 0x12E00 && (addr) <= 0x138FF) || \
	 ((addr) >= 0x13C00 && (addr) <= 0x15FFF) || \
	 ((addr) >= 0x17000 && (addr) <= 0x17FFF) ? 1 : 0)

#define ADDR_IS_BB1_8852A(addr) \
	(((addr) >= 0x12600 && (addr) <= 0x12DFF) || \
	 ((addr) >= 0x13900 && (addr) <= 0x13BFF) || \
	 ((addr) >= 0x16000 && (addr) <= 0x16FFF) ? 1 : 0)
#define ADDR_IS_BB1_8852B(addr) \
	(((addr) >= 0x12600 && (addr) <= 0x12DFF) || \
	 ((addr) >= 0x13900 && (addr) <= 0x13BFF) || \
	 ((addr) >= 0x16000 && (addr) <= 0x16FFF) ? 1 : 0)
#define ADDR_IS_BB1_8852C(addr) \
	(((addr) >= 0x12600 && (addr) <= 0x12DFF) || \
	 ((addr) >= 0x13900 && (addr) <= 0x13BFF) || \
	 ((addr) >= 0x16000 && (addr) <= 0x16FFF) ? 1 : 0)
#define ADDR_IS_BB1_8192XB(addr) \
	(((addr) >= 0x12600 && (addr) <= 0x12DFF) || \
	 ((addr) >= 0x13900 && (addr) <= 0x13BFF) || \
	 ((addr) >= 0x16000 && (addr) <= 0x16FFF) ? 1 : 0)
#define ADDR_IS_BB1_8851B(addr) \
	(((addr) >= 0x12600 && (addr) <= 0x12DFF) || \
	 ((addr) >= 0x13900 && (addr) <= 0x13BFF) || \
	 ((addr) >= 0x16000 && (addr) <= 0x16FFF) ? 1 : 0)
#define ADDR_IS_BB1_8851E(addr) \
	(((addr) >= 0x12600 && (addr) <= 0x12DFF) || \
	 ((addr) >= 0x13900 && (addr) <= 0x13BFF) || \
	 ((addr) >= 0x16000 && (addr) <= 0x16FFF) ? 1 : 0)
#define ADDR_IS_BB1_8852D(addr) \
	(((addr) >= 0x12600 && (addr) <= 0x12DFF) || \
	 ((addr) >= 0x13900 && (addr) <= 0x13BFF) || \
	 ((addr) >= 0x16000 && (addr) <= 0x16FFF) ? 1 : 0)

#define ADDR_IS_RF_8852A(addr) \
	((addr) >= 0x18000 && (addr) <= 0x1DFFF ? 1 : 0)
#define ADDR_IS_RF_8852B(addr) \
	((addr) >= 0x18000 && (addr) <= 0x1FFFF ? 1 : 0)
#define ADDR_IS_RF_8852C(addr) \
	((addr) >= 0x18000 && (addr) <= 0x1FFFF ? 1 : 0)
#define ADDR_IS_RF_8192XB(addr) \
	((addr) >= 0x18000 && (addr) <= 0x1FFFF ? 1 : 0)
#define ADDR_IS_RF_8851B(addr) \
	((addr) >= 0x18000 && (addr) <= 0x1FFFF ? 1 : 0)
#define ADDR_IS_RF_8851E(addr) \
	((addr) >= 0x18000 && (addr) <= 0x1FFFF ? 1 : 0)
#define ADDR_IS_RF_8852D(addr) \
	((addr) >= 0x18000 && (addr) <= 0x1FFFF ? 1 : 0)

#define ADDR_IS_IND_ACES_8852A(addr) \
	((addr) >= 0x40000 && (addr) <= 0x7FFFF ? 1 : 0)
#define ADDR_IS_IND_ACES_8852B(addr) \
	((addr) >= 0x40000 && (addr) <= 0x7FFFF ? 1 : 0)
#define ADDR_IS_IND_ACES_8852C(addr) \
	((addr) >= 0x40000 && (addr) <= 0x7FFFF ? 1 : 0)
#define ADDR_IS_IND_ACES_8192XB(addr) \
	((addr) >= 0x40000 && (addr) <= 0x7FFFF ? 1 : 0)
#define ADDR_IS_IND_ACES_8851B(addr) \
	((addr) >= 0x40000 && (addr) <= 0x7FFFF ? 1 : 0)
#define ADDR_IS_IND_ACES_8851E(addr) \
	((addr) >= 0x40000 && (addr) <= 0x7FFFF ? 1 : 0)
#define ADDR_IS_IND_ACES_8852D(addr) \
	((addr) >= 0x40000 && (addr) <= 0x7FFFF ? 1 : 0)

#define ADDR_IS_RSVD_8852A(addr) \
	(((addr) >= 0x2000 && (addr) <= 0x7FFF) || \
	 ((addr) >= 0x1E000 && (addr) <= 0x3FFFF) || \
	 (addr) >= 0x80000 ? 1 : 0)
#define ADDR_IS_RSVD_8852B(addr) \
	(((addr) >= 0x2000 && (addr) <= 0x7FFF) || \
	 ((addr) >= 0x20000 && (addr) <= 0x3FFFF) || \
	 (addr) >= 0x80000 ? 1 : 0)
#define ADDR_IS_RSVD_8852C(addr) \
	(((addr) >= 0x6400 && (addr) <= 0x6FFF) || \
	 ((addr) >= 0x20000 && (addr) <= 0x3FFFF) || \
	 (addr) >= 0x80000 ? 1 : 0)
#define ADDR_IS_RSVD_8192XB(addr) \
	(((addr) >= 0x6400 && (addr) <= 0x6FFF) || \
	 ((addr) >= 0x20000 && (addr) <= 0x3FFFF) || \
	 (addr) >= 0x80000 ? 1 : 0)
#define ADDR_IS_RSVD_8851B(addr) \
	(((addr) >= 0x2000 && (addr) <= 0x7FFF) || \
	 ((addr) >= 0x20000 && (addr) <= 0x3FFFF) || \
	 (addr) >= 0x80000 ? 1 : 0)
#define ADDR_IS_RSVD_8851E(addr) \
	(((addr) >= 0x6400 && (addr) <= 0x6FFF) || \
	 ((addr) >= 0x20000 && (addr) <= 0x3FFFF) || \
	 (addr) >= 0x80000 ? 1 : 0)
#define ADDR_IS_RSVD_8852D(addr) \
	(((addr) >= 0x6400 && (addr) <= 0x6FFF) || \
	 ((addr) >= 0x20000 && (addr) <= 0x3FFFF) || \
	 (addr) >= 0x80000 ? 1 : 0)

#if (MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT)
#define ADDR_NOT_ALLOW_SERL1(addr) \
	((addr) != R_AX_SER_DBG_INFO && (addr) != R_AX_HCI_FUNC_EN && \
	 (addr) != R_AX_HD0IMR && (addr) != R_AX_HD0ISR ? 1 : 0)
#define ADDR_NOT_ALLOW_LPS(addr) ((addr) != R_AX_CPWM ? 1 : 0)
#else // (MAC_AX_8852C_SUPPORT || MAC_AX_8192XB_SUPPORT)
#define ADDR_NOT_ALLOW_SERL1(addr) \
	((addr) != R_AX_SER_DBG_INFO && (addr) != R_AX_HCI_FUNC_EN_V1 && \
	 (addr) != R_AX_HD0IMR_V1 && (addr) != R_AX_HD0ISR_V1 ? 1 : 0)
#define ADDR_NOT_ALLOW_LPS(addr) ((addr) != R_AX_CPWM_V1 ? 1 : 0)
#endif

/*--------------------Define Struct-------------------------------------*/

/*--------------------Function declaration------------------------------*/

/**
 * @addtogroup Common
 * @{
 * @addtogroup HW_Related
 * @{
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup HW_Related
 * @{
 */
/**
 * @brief mac_get_hw_info
 *
 * @param *adapter
 * @return Please Place Description here.
 * @retval  mac_ax_hw_info
 */
struct mac_ax_hw_info *mac_get_hw_info(struct mac_ax_adapter *adapter);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup HW_Related
 * @{
 */

/**
 * @brief mac_set_hw_value
 *
 * @param *adapter
 * @param hw_id
 * @param *val
 * @return Please Place Description here.
 * @retval u32
 */

u32 mac_set_hw_value(struct mac_ax_adapter *adapter,
		     enum mac_ax_hw_id hw_id, void *val);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup HW_Related
 * @{
 */

/**
 * @brief mac_get_hw_value
 *
 * @param *adapter
 * @param hw_id
 * @param *val
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_get_hw_value(struct mac_ax_adapter *adapter,
		     enum mac_ax_hw_id hw_id, void *val);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup HW_Related
 * @{
 */

/**
 * @brief mac_write_xtal_si
 *
 * @param *adapter
 * @param offset
 * @param val
 * @param bitmask
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_write_xtal_si(struct mac_ax_adapter *adapter,
		      u8 offset, u8 val, u8 bitmask);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup HW_Related
 * @{
 */

/**
 * @brief mac_read_xtal_si
 *
 * @param *adapter
 * @param offset
 * @param *val
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_read_xtal_si(struct mac_ax_adapter *adapter,
		     u8 offset, u8 *val);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup HW_Related
 * @{
 */

/**
 * @brief set_host_rpr
 *
 * @param *adapter
 * @param *cfg
 * @return Please Place Description here.
 * @retval u32
 */
u32 set_host_rpr(struct mac_ax_adapter *adapter,
		 struct mac_ax_host_rpr_cfg *cfg);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup HW_Related
 * @{
 */

/**
 * @brief mac_read_pwr_reg
 *
 * @param *adapter
 * @param band
 * @param offset
 * @param *val
 * @return Please Place Description here.
 * @retval u32
 */
u32 set_l2_status(struct mac_ax_adapter *adapter);
u32 mac_read_pwr_reg(struct mac_ax_adapter *adapter, u8 band,
		     const u32 offset, u32 *val);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup HW_Related
 * @{
 */

/**
 * @brief mac_write_pwr_reg
 *
 * @param *adapter
 * @param band
 * @param offset
 * @param val
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_write_pwr_reg(struct mac_ax_adapter *adapter, u8 band,
		      const u32 offset, u32 val);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup HW_Related
 * @{
 */

/**
 * @brief mac_write_msk_pwr_reg
 *
 * @param *adapter
 * @param band
 * @param offset
 * @param mask
 * @param val
 * @return write tx power reg with mask value into fw
 * @retval u32
 */
u32 mac_write_msk_pwr_reg(struct mac_ax_adapter *adapter, u8 band,
			  const u32 offset, u32 mask, u32 val);

/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup HW_Related
 * @{
 */

/**
 * @brief mac_write_pwr_ofst_mode
 *
 * @param *adapter
 * @param band
 * @param *tpu
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_write_pwr_ofst_mode(struct mac_ax_adapter *adapter,
			    u8 band, struct rtw_tpu_info *tpu);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup HW_Related
 * @{
 */

/**
 * @brief mac_write_pwr_ofst_bw
 *
 * @param *adapter
 * @param band
 * @param *tpu
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_write_pwr_ofst_bw(struct mac_ax_adapter *adapter,
			  u8 band, struct rtw_tpu_info *tpu);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup HW_Related
 * @{
 */

/**
 * @brief mac_write_pwr_ref_reg
 *
 * @param *adapter
 * @param band
 * @param *tpu
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_write_pwr_ref_reg(struct mac_ax_adapter *adapter,
			  u8 band, struct rtw_tpu_info *tpu);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup HW_Related
 * @{
 */

/**
 * @brief mac_write_pwr_limit_en
 *
 * @param *adapter
 * @param band
 * @param *tpu
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_write_pwr_limit_en(struct mac_ax_adapter *adapter,
			   u8 band, struct rtw_tpu_info *tpu);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup HW_Related
 * @{
 */

/**
 * @brief mac_write_pwr_limit_rua_reg
 *
 * @param *adapter
 * @param band
 * @param *tpu
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_write_pwr_limit_rua_reg(struct mac_ax_adapter *adapter,
				u8 band, struct rtw_tpu_info *tpu);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup HW_Related
 * @{
 */

/**
 * @brief mac_write_pwr_limit_reg
 *
 * @param *adapter
 * @param band
 * @param *tpu
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_write_pwr_limit_reg(struct mac_ax_adapter *adapter,
			    u8 band, struct rtw_tpu_pwr_imt_info *tpu);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup HW_Related
 * @{
 */

/**
 * @brief mac_write_pwr_by_rate_reg
 *
 * @param *adapter
 * @param band
 * @param *tpu
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_write_pwr_by_rate_reg(struct mac_ax_adapter *adapter,
			      u8 band, struct rtw_tpu_pwr_by_rate_info *tpu);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup HW_Related
 * @{
 */

/** * @brief mac_read_xcap_reg
 *
 * @param *adapter
 * @param sc_xo
 * @param *val
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_read_xcap_reg(struct mac_ax_adapter *adapter, u8 sc_xo, u32 *val);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup HW_Related
 * @{
 */

/**
 * @brief mac_write_xcap_reg
 *
 * @param *adapter
 * @param sc_xo
 * @param val
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_write_xcap_reg(struct mac_ax_adapter *adapter, u8 sc_xo, u32 val);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup HW_Related
 * @{
 */

/**
 * @brief mac_write_bbrst_reg
 *
 * @param *adapter
 * @param val
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_read_xcap_reg_dav(struct mac_ax_adapter *adapter, u8 sc_xo, u32 *val);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup HW_Related
 * @{
 */

/**
 * @brief mac_write_xcap_reg
 *
 * @param *adapter
 * @param sc_xo
 * @param val
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_write_xcap_reg_dav(struct mac_ax_adapter *adapter, u8 sc_xo, u32 val);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup HW_Related
 * @{
 */

/**
 * @brief mac_write_bbrst_reg
 *
 * @param *adapter
 * @param val
 * @return Please Place Description here.
 * @retval u32
 */

u32 mac_write_bbrst_reg(struct mac_ax_adapter *adapter, u8 val);
/**
 * @}
 * @}
 */

/** * @brief set_macid_pause
 *
 * @param *adapter
 * @param *cfg
 * @return Please Place Description here.
 * @retval u32
 */
u32 set_macid_pause(struct mac_ax_adapter *adapter,
		    struct mac_ax_macid_pause_cfg *cfg);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup HW_Related
 * @{
 */

/**
 * @brief macid_pause
 *
 * @param *adapter
 * @param *grp
 * @return Please Place Description here.
 * @retval u32
 */
u32 macid_pause(struct mac_ax_adapter *adapter,
		struct mac_ax_macid_pause_grp *grp);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup HW_Related
 * @{
 */

/** * @brief set_macid_pause_sleep
 *
 * @param *adapter
 * @param *cfg
 * @return Please Place Description here.
 * @retval u32
 */
u32 set_macid_pause_sleep(struct mac_ax_adapter *adapter,
			  struct mac_ax_macid_pause_sleep_cfg *cfg);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup HW_Related
 * @{
 */

/**
 * @brief macid_pause_sleep
 *
 * @param *adapter
 * @param *grp
 * @return Please Place Description here.
 * @retval u32
 */
u32 macid_pause_sleep(struct mac_ax_adapter *adapter,
		      struct mac_ax_macid_pause_sleep_grp *grp);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup HW_Related
 * @{
 */

/**
 * @brief get_macid_pause
 *
 * @param *adapter
 * @param *cfg
 * @return Please Place Description here.
 * @retval u32
 */
u32 get_macid_pause(struct mac_ax_adapter *adapter,
		    struct mac_ax_macid_pause_cfg *cfg);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup HW_Related
 * @{
 */

/**
 * @brief get_ss_wmm_tbl
 *
 * @param *adapter
 * @param *ctrl
 * @return Please Place Description here.
 * @retval u32
 */
u32 get_ss_wmm_tbl(struct mac_ax_adapter *adapter,
		   struct mac_ax_ss_wmm_tbl_ctrl *ctrl);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup HW_Related
 * @{
 */

/**
 * @brief set_enable_bb_rf
 *
 * @param *adapter
 * @param enable
 * @return Please Place Description here.
 * @retval u32
 */
u32 set_enable_bb_rf(struct mac_ax_adapter *adapter, u8 enable);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup HW_Related
 * @{
 */

/**
 * @brief set_cctl_rty_limit
 *
 * @param *adapter
 * @param *cfg
 * @return Please Place Description here.
 * @retval u32
 */
u32 set_cctl_rty_limit(struct mac_ax_adapter *adapter,
		       struct mac_ax_cctl_rty_lmt_cfg *cfg);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup HW_Related
 * @{
 */

/**
 * @brief cfg_mac_bw
 *
 * @param *adapter
 * @param *cfg
 * @return Please Place Description here.
 * @retval u32
 */
u32 cfg_mac_bw(struct mac_ax_adapter *adapter,
	       struct mac_ax_cfg_bw *cfg);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup HW_Related
 * @{
 */

/**
 * @brief get_ss_quota_mode
 *
 * @param *adapter
 * @param *ctrl
 * @return Please Place Description here.
 * @retval u32
 */
u32 get_ss_quota_mode(struct mac_ax_adapter *adapter,
		      struct mac_ax_ss_quota_mode_ctrl *ctrl);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup HW_Related
 * @{
 */

/**
 * @brief set_ss_quota_mode
 *
 * @param *adapter
 * @param *ctrl
 * @return Please Place Description here.
 * @retval u32
 */
u32 set_ss_quota_mode(struct mac_ax_adapter *adapter,
		      struct mac_ax_ss_quota_mode_ctrl *ctrl);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup HW_Related
 * @{
 */

/**
 * @brief ss_get_quotasetting
 *
 * @param *adapter
 * @param *para
 * @return Please Place Description here.
 * @retval u32
 */
u32 ss_get_quotasetting(struct mac_ax_adapter *adapter,
			struct mac_ax_ss_quota_setting *para);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup HW_Related
 * @{
 */

/**
 * @brief ss_set_quotasetting
 *
 * @param *adapter
 * @param *para
 * @return Please Place Description here.
 * @retval u32
 */
u32 ss_set_quotasetting(struct mac_ax_adapter *adapter,
			struct mac_ax_ss_quota_setting *para);
/**
 * @}
 * @}
 */

/**
 * @brief scheduler_set_prebkf
 *
 * @param *adapter
 * @param *para
 * @return Please Place Description here.
 * @retval u32
 */
u32 scheduler_set_prebkf(struct mac_ax_adapter *adapter,
			 struct mac_ax_prebkf_setting *para);
/**
 * @}
 * @}
 */

/**
 * @brief set_bacam_mode
 *
 * @param *adapter
 * @param mode_sel
 * @return Set the R_AX_RESPBA_CAM_CTRL bit 4 to be 0 or 1 which decide the
 * option mode in BA CAM.
 * @retval u32
 */
u32 set_bacam_mode(struct mac_ax_adapter *adapter, u8 mode_sel);
/**
 * @}
 * @}
 */

/**
 * @brief scheduler_set_prebkf
 *
 * @param *adapter
 * @param *mode_sel
 * @return Get the option mode from R_AX_RESPBA_CAM_CTRL bit 4 in the BA CAM.
 * @retval u32
 */
u32 get_bacam_mode(struct mac_ax_adapter *adapter, u8 *mode_sel);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup HW_Related
 * @{
 */

/**
 * @brief mac_io_chk_access
 *
 * @param *adapter
 * @param offset
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_io_chk_access(struct mac_ax_adapter *adapter, u32 offset);
/**
 * @}
 * @}
 */

u32 mac_get_bt_dis(struct mac_ax_adapter *adapter);

u32 mac_set_bt_dis(struct mac_ax_adapter *adapter, u8 en);

/**
 * @brief mac_watchdog
 *
 * @param *adapter
 * @return Please Place Description here.
 * @param *wdt_param
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_watchdog(struct mac_ax_adapter *adapter,
		 struct mac_ax_wdt_param *wdt_param);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup HW_Related
 * @{
 */

/**
 * @brief mac_get_freerun
 *
 * @param *adapter
 * @param *freerun
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_get_freerun(struct mac_ax_adapter *adapter,
		    struct mac_ax_freerun *freerun);

/**
 * @}
 * @}
 */
#endif

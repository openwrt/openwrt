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

#ifndef _MAC_AX_DBGPORT_HW_H_
#define _MAC_AX_DBGPORT_HW_H_

#include "../mac_def.h"
#include "../mac_ax.h"

#define MAC_AX_DP_MODE_DUMP	0
#define MAC_AX_DP_MODE_LA	1

#define AXIDMA_DBG_SEL_INTN0_SH 0
#define AXIDMA_DBG_SEL_INTN1_SH 3
#define AXIDMA_DBG_SEL_INTN2_SH 6

/**
 * @enum mac_ax_dbgport_sel
 *
 * @brief mac_ax_dbgport_sel
 *
 * @var mac_ax_dbgport_sel::MAC_AX_DP_SEL_SYS_0
 * Please Place Description here.
 * @var mac_ax_dbgport_sel::MAC_AX_DP_SEL_SYS_1F
 * Please Place Description here.
 * @var mac_ax_dbgport_sel::MAC_AX_DP_SEL_LAST
 * Please Place Description here.
 */
enum mac_ax_dbgport_sel {
	/* SYSTEM */
	MAC_AX_DP_SEL_SYS_0 = 0x0, // 0x0 -0x1F
	MAC_AX_DP_SEL_SYS_1F = 0x1F,
	/* PINMUX */
	MAC_AX_DP_SEL_PINMUX_0 = 0x20, // 0x20 - 0x27
	MAC_AX_DP_SEL_PINMUX_7 = 0x27,
	/* LOADER */
	MAC_AX_DP_SEL_LOADER_0 = 0x28, //0x28-0x2B
	MAC_AX_DP_SEL_LOADER_3 = 0x2B,
	/* HMUX */
	MAC_AX_DP_SEL_HMUX_0 = 0x2C, //0x2C-0x2F
	MAC_AX_DP_SEL_HMUX_3 = 0x2F,
	/* PCIE */
	MAC_AX_DP_SEL_PCIE_0 = 0x30,
	MAC_AX_DP_SEL_PCIE_1 = 0x31,
	MAC_AX_DP_SEL_PCIE_2 = 0x32,
	MAC_AX_DP_SEL_PCIE_3 = 0x33,
	MAC_AX_DP_SEL_PCIE_4 = 0x34,
	MAC_AX_DP_SEL_PCIE_5 = 0x35,
	MAC_AX_DP_SEL_PCIE_6 = 0x36,
	MAC_AX_DP_SEL_PCIE_7 = 0x37,
	MAC_AX_DP_SEL_PCIE_8 = 0x38,
	MAC_AX_DP_SEL_PCIE_9 = 0x39,
	MAC_AX_DP_SEL_PCIE_A = 0x3A,
	MAC_AX_DP_SEL_PCIE_B = 0x3B,
	MAC_AX_DP_SEL_PCIE_C = 0x3C,
	MAC_AX_DP_SEL_PCIE_D = 0x3D,
	MAC_AX_DP_SEL_PCIE_E = 0x3E,
	MAC_AX_DP_SEL_PCIE_F = 0x3F,
	/* USB */
	MAC_AX_DP_SEL_USB_0 = 0x40, //0x40-0x4F
	MAC_AX_DP_SEL_USB_F = 0x4F,
	/* SDIO */
	MAC_AX_DP_SEL_SDIO_0 = 0x50, //0x50-0x5F
	MAC_AX_DP_SEL_SDIO_F = 0x5F,
	/* BT */
	MAC_AX_DP_SEL_BT = 0x60,
	/* WLAN_MAC */
	MAC_AX_DP_SEL_AXIDMA = 0x71,
	MAC_AX_DP_SEL_WLPHYDBG_GPIO = 0x72,
	MAC_AX_DP_SEL_BTCOEXIST = 0x74,
	MAC_AX_DP_SEL_LTECOEX = 0x75,
	MAC_AX_DP_SEL_WLPHYDBG = 0x76,
	MAC_AX_DP_SEL_WLAN_MAC_REG = 0x77,
	MAC_AX_DP_SEL_WLAN_MAC_PMC = 0x78,
	MAC_AX_DP_SEL_CALIB_TOP = 0x79,
	/* MAC */
	// DMAC
	MAC_AX_DP_SEL_DISPATCHER_TOP = 0x80,
	MAC_AX_DP_SEL_WDE_DLE = 0x81,
	MAC_AX_DP_SEL_PLE_DLE = 0x82,
	MAC_AX_DP_SEL_WDRLS = 0x83,
	MAC_AX_DP_SEL_DLE_CPUIO = 0x84,
	MAC_AX_DP_SEL_BBRPT = 0x85,
	MAC_AX_DP_SEL_TXPKTCTL = 0x86,
	MAC_AX_DP_SEL_PKTBUFFER = 0x87,
	MAC_AX_DP_SEL_DMAC_TABLE = 0x88,
	MAC_AX_DP_SEL_STA_SCHEDULER = 0x89,
	MAC_AX_DP_SEL_DMAC_PKTIN = 0x8A,
	MAC_AX_DP_SEL_WSEC_TOP = 0x8B,
	MAC_AX_DP_SEL_MPDU_PROCESSOR = 0x8C,
	MAC_AX_DP_SEL_DMAC_APB_BRIDGE = 0x8D,
	MAC_AX_DP_SEL_LTR_CTRL = 0x8E,
	// CMAC_0
	MAC_AX_DP_SEL_CMAC0_CMAC_DMAC_TOP = 0xA0,
	MAC_AX_DP_SEL_CMAC0_PTCLTOP = 0xA1,
	MAC_AX_DP_SEL_CMAC0_SCHEDULERTOP = 0xA2,
	MAC_AX_DP_SEL_CMAC0_TXPWR_CTRL = 0xA3,
	MAC_AX_DP_SEL_CMAC0_CMAC_APB_BRIDGE = 0xA4,
	MAC_AX_DP_SEL_CMAC0_MACTX = 0xA5,
	MAC_AX_DP_SEL_CMAC0_MACRX = 0xA6,
	MAC_AX_DP_SEL_CMAC0_WMAC_TRXPTCL = 0xA7,
	// CMAC_1
	MAC_AX_DP_SEL_CMAC1_CMAC_DMAC_TOP = 0xB0,
	MAC_AX_DP_SEL_CMAC1_PTCLTOP = 0xB1,
	MAC_AX_DP_SEL_CMAC1_SCHEDULERTOP = 0xB2,
	MAC_AX_DP_SEL_CMAC1_TXPWR_CTRL = 0xB3,
	MAC_AX_DP_SEL_CMAC1_CMAC_APB_BRIDGE = 0xB4,
	MAC_AX_DP_SEL_CMAC1_MACTX = 0xB5,
	MAC_AX_DP_SEL_CMAC1_MACRX = 0xB6,
	MAC_AX_DP_SEL_CMAC1_WMAC_TRXPTCL = 0xB7,
	MAC_AX_DP_SEL_CMAC_SHARE = 0xC0,
	/* WLAN_MAC */
	MAC_AX_DP_SEL_WL_CPU_0 = 0xF0, //0xF0-0xFF
	MAC_AX_DP_SEL_WL_CPU_F = 0xFF,

	/* keep last */
	MAC_AX_DP_SEL_LAST
};

/**
 * @enum mac_ax_dbgport_sel0_16b
 *
 * @brief mac_ax_dbgport_sel0_16b
 *
 * @var mac_ax_dbgport_sel0_16b::MAC_AX_DP_SEL0_16B_0_15
 * Please Place Description here.
 * @var mac_ax_dbgport_sel0_16b::MAC_AX_DP_SEL0_16B_16_31
 * Please Place Description here.
 * @var mac_ax_dbgport_sel0_16b::MAC_AX_DP_SEL0_16B_END
 * Please Place Description here.
 */
enum mac_ax_dbgport_sel0_16b {
	MAC_AX_DP_SEL0_16B_0_15 = 0,
	MAC_AX_DP_SEL0_16B_16_31 = 1,
	MAC_AX_DP_SEL0_16B_END
};

/**
 * @enum mac_ax_dbgport_sel1_16b
 *
 * @brief mac_ax_dbgport_sel1_16b
 *
 * @var mac_ax_dbgport_sel1_16b::MAC_AX_DP_SEL1_16B_16_31
 * Please Place Description here.
 * @var mac_ax_dbgport_sel1_16b::MAC_AX_DP_SEL1_16B_0_15
 * Please Place Description here.
 * @var mac_ax_dbgport_sel1_16b::MAC_AX_DP_SEL1_16B_END
 * Please Place Description here.
 */
enum mac_ax_dbgport_sel1_16b {
	MAC_AX_DP_SEL1_16B_16_31 = 0,
	MAC_AX_DP_SEL1_16B_0_15 = 1,
	MAC_AX_DP_SEL1_16B_END
};

/**
 * @enum mac_ax_dbgport_sel_4b
 *
 * @brief mac_ax_dbgport_sel_4b
 *
 * @var mac_ax_dbgport_sel_4b::MAC_AX_DP_SEL_4B_0_7
 * Please Place Description here.
 * @var mac_ax_dbgport_sel_4b::MAC_AX_DP_SEL_4B_4_11
 * Please Place Description here.
 * @var mac_ax_dbgport_sel_4b::MAC_AX_DP_SEL_4B_8_15
 * Please Place Description here.
 * @var mac_ax_dbgport_sel_4b::MAC_AX_DP_SEL_4B_0_3_12_15
 * Please Place Description here.
 * @var mac_ax_dbgport_sel_4b::MAC_AX_DP_SEL_4B_END
 * Please Place Description here.
 */
enum mac_ax_dbgport_sel_4b {
	MAC_AX_DP_SEL_4B_0_7 = 0,
	MAC_AX_DP_SEL_4B_4_11 = 1,
	MAC_AX_DP_SEL_4B_8_15 = 2,
	MAC_AX_DP_SEL_4B_0_3_12_15 = 3,
	MAC_AX_DP_SEL_4B_END
};

/* For internal index */
/* COMMON */
enum mac_ax_dbgport_intn_idx_na {
	MAC_AX_DP_INTN_IDX_NA = 0x0,
	MAC_AX_DP_INTN_IDX_NA_MAX = 0x1,
};

/* SYSTEM */
// MAC_AX_DP_SEL_SYS_0 = 0x0, // 0x0 -0x1F
// MAC_AX_DP_SEL_SYS_1F = 0x1F,

/* PINMUX */
// MAC_AX_DP_SEL_PINMUX_0 = 0x20, // 0x20 - 0x27
// MAC_AX_DP_SEL_PINMUX_7 = 0x27,

/* LOADER */
// MAC_AX_DP_SEL_LOADER_0 = 0x28, //0x28-0x2B
// MAC_AX_DP_SEL_LOADER_3 = 0x2B,

/* HMUX */
// MAC_AX_DP_SEL_HMUX_0 = 0x2C, //0x2C-0x2F
// MAC_AX_DP_SEL_HMUX_3 = 0x2F,

/* PCIE */
// MAC_AX_DP_SEL_PCIE_0 = 0x30,
enum mac_ax_dbgport_intn_idx_pcie_0 {
	MAC_AX_DP_INTN_IDX_PCIE_0_0 = 0x0,
	MAC_AX_DP_INTN_IDX_PCIE_0_1 = 0x1,
	MAC_AX_DP_INTN_IDX_PCIE_0_2 = 0x2,
	MAC_AX_DP_INTN_IDX_PCIE_0_3 = 0x3,
	MAC_AX_DP_INTN_IDX_PCIE_0_MAX = 0x4
};

// MAC_AX_DP_SEL_PCIE_1 = 0x31,
enum mac_ax_dbgport_intn_idx_pcie_1 {
	MAC_AX_DP_INTN_IDX_PCIE_1_0 = 0x0,
	MAC_AX_DP_INTN_IDX_PCIE_1_1 = 0x1,
	MAC_AX_DP_INTN_IDX_PCIE_1_2 = 0x2,
	MAC_AX_DP_INTN_IDX_PCIE_1_3 = 0x3,
	MAC_AX_DP_INTN_IDX_PCIE_1_4 = 0x4,
	MAC_AX_DP_INTN_IDX_PCIE_1_MAX = 0x5
};

// MAC_AX_DP_SEL_PCIE_2 = 0x32,
enum mac_ax_dbgport_intn_idx_pcie_2 {
	MAC_AX_DP_INTN_IDX_PCIE_2_0 = 0x0,
	MAC_AX_DP_INTN_IDX_PCIE_2_1 = 0x1,
	MAC_AX_DP_INTN_IDX_PCIE_2_MAX = 0x2
};

// MAC_AX_DP_SEL_PCIE_3 = 0x33,
enum mac_ax_dbgport_intn_idx_pcie_3 {
	MAC_AX_DP_INTN_IDX_PCIE_3_0 = 0x0,
	MAC_AX_DP_INTN_IDX_PCIE_3_1 = 0x1,
	MAC_AX_DP_INTN_IDX_PCIE_3_2 = 0x2,
	MAC_AX_DP_INTN_IDX_PCIE_3_3 = 0x3,
	MAC_AX_DP_INTN_IDX_PCIE_3_4 = 0x4,
	MAC_AX_DP_INTN_IDX_PCIE_3_5 = 0x5,
	MAC_AX_DP_INTN_IDX_PCIE_3_MAX = 0x6
};

// MAC_AX_DP_SEL_PCIE_4 = 0x34,
// MAC_AX_DP_SEL_PCIE_5 = 0x35,
// MAC_AX_DP_SEL_PCIE_6 = 0x36,
// MAC_AX_DP_SEL_PCIE_7 = 0x37,
enum mac_ax_dbgport_intn_idx_pcie_7 {
	MAC_AX_DP_INTN_IDX_PCIE_7_0 = 0x0,
	MAC_AX_DP_INTN_IDX_PCIE_7_1 = 0x1,
	MAC_AX_DP_INTN_IDX_PCIE_7_2 = 0x2,
	MAC_AX_DP_INTN_IDX_PCIE_7_3 = 0x3,
	MAC_AX_DP_INTN_IDX_PCIE_7_4 = 0x4,
	MAC_AX_DP_INTN_IDX_PCIE_7_MAX = 0x5
};

// MAC_AX_DP_SEL_PCIE_8 = 0x38,
enum mac_ax_dbgport_intn_idx_pcie_8 {
	MAC_AX_DP_INTN_IDX_PCIE_8_0 = 0x0,
	MAC_AX_DP_INTN_IDX_PCIE_8_1 = 0x1,
	MAC_AX_DP_INTN_IDX_PCIE_8_2 = 0x2,
	MAC_AX_DP_INTN_IDX_PCIE_8_3 = 0x3,
	MAC_AX_DP_INTN_IDX_PCIE_8_4 = 0x4,
	MAC_AX_DP_INTN_IDX_PCIE_8_5 = 0x5,
	MAC_AX_DP_INTN_IDX_PCIE_8_6 = 0x6,
	MAC_AX_DP_INTN_IDX_PCIE_8_MAX = 0x7
};

// MAC_AX_DP_SEL_PCIE_9 = 0x39,
// MAC_AX_DP_SEL_PCIE_A = 0x3A,
// MAC_AX_DP_SEL_PCIE_B = 0x3B,
// MAC_AX_DP_SEL_PCIE_C = 0x3C,
// MAC_AX_DP_SEL_PCIE_D = 0x3D,
// MAC_AX_DP_SEL_PCIE_E = 0x3E,
// MAC_AX_DP_SEL_PCIE_F = 0x3F,

/* USB */
// MAC_AX_DP_SEL_USB_0 = 0x40, //0x40-0x4F
// MAC_AX_DP_SEL_USB_F = 0x4F,

/* SDIO */
// MAC_AX_DP_SEL_SDIO_0 = 0x50, //0x50-0x5F
// MAC_AX_DP_SEL_SDIO_F = 0x5F,

/* BT */
// MAC_AX_DP_SEL_BT = 0x60,

/* WLAN_MAC */
// MAC_AX_DP_SEL_AXIDMA = 0x71,
enum mac_ax_dbgport_intn_idx_axidma {
	MAC_AX_DP_INTN_IDX_AXIDMA_0 = 0x0,
	MAC_AX_DP_INTN_IDX_AXIDMA_1 = 0x1,
	MAC_AX_DP_INTN_IDX_AXIDMA_2 = 0x2,
	MAC_AX_DP_INTN_IDX_AXIDMA_3 = 0x3,
	MAC_AX_DP_INTN_IDX_AXIDMA_4 = 0x4,
	MAC_AX_DP_INTN_IDX_AXIDMA_5 = 0x5,
	MAC_AX_DP_INTN_IDX_AXIDMA_6 = 0x6,
	MAC_AX_DP_INTN_IDX_AXIDMA_7 = 0x7,
	MAC_AX_DP_INTN_IDX_AXIDMA_8 = 0x8,
	MAC_AX_DP_INTN_IDX_AXIDMA_9 = 0x9,
	MAC_AX_DP_INTN_IDX_AXIDMA_A = 0xA,
	MAC_AX_DP_INTN_IDX_AXIDMA_B = 0xB,
	MAC_AX_DP_INTN_IDX_AXIDMA_C = 0xC,
	MAC_AX_DP_INTN_IDX_AXIDMA_D = 0xD,
	MAC_AX_DP_INTN_IDX_AXIDMA_MAX = 0xE
};

// MAC_AX_DP_SEL_WLPHYDBG_GPIO = 0x72,
// MAC_AX_DP_SEL_BTCOEXIST = 0x74,
// MAC_AX_DP_SEL_LTECOEX = 0x75,
// MAC_AX_DP_SEL_WLPHYDBG = 0x76,
// MAC_AX_DP_SEL_WLAN_MAC_REG = 0x77,
// MAC_AX_DP_SEL_WLAN_MAC_PMC = 0x78,
// MAC_AX_DP_SEL_CALIB_TOP = 0x79,

/* MAC */
// DMAC
// MAC_AX_DP_SEL_DISPATCHER_TOP = 0x80,
// MAC_AX_DP_SEL_WDE_DLE = 0x81,
enum mac_ax_dbgport_intn_idx_wde_dle {
	MAC_AX_DP_INTN_IDX_WDE_DLE_0 = 0x0,
	MAC_AX_DP_INTN_IDX_WDE_DLE_1 = 0x1,
	MAC_AX_DP_INTN_IDX_WDE_DLE_2 = 0x2,
	MAC_AX_DP_INTN_IDX_WDE_DLE_3 = 0x3,
	MAC_AX_DP_INTN_IDX_WDE_DLE_4 = 0x4,
	MAC_AX_DP_INTN_IDX_WDE_DLE_5 = 0x5,
	MAC_AX_DP_INTN_IDX_WDE_DLE_6 = 0x6,
	MAC_AX_DP_INTN_IDX_WDE_DLE_7 = 0x7,
	MAC_AX_DP_INTN_IDX_WDE_DLE_8 = 0x8,
	MAC_AX_DP_INTN_IDX_WDE_DLE_9 = 0x9,
	MAC_AX_DP_INTN_IDX_WDE_DLE_A = 0xA,
	MAC_AX_DP_INTN_IDX_WDE_DLE_B = 0xB,
	MAC_AX_DP_INTN_IDX_WDE_DLE_C = 0xC,
	MAC_AX_DP_INTN_IDX_WDE_DLE_D = 0xD,
	MAC_AX_DP_INTN_IDX_WDE_DLE_E = 0xE,
	MAC_AX_DP_INTN_IDX_WDE_DLE_F = 0xF,
	MAC_AX_DP_INTN_IDX_WDE_DLE_10 = 0x10,
	MAC_AX_DP_INTN_IDX_WDE_DLE_11 = 0x11,
	MAC_AX_DP_INTN_IDX_WDE_DLE_12 = 0x12,
	MAC_AX_DP_INTN_IDX_WDE_DLE_13 = 0x13,
	MAC_AX_DP_INTN_IDX_WDE_DLE_14 = 0x14,
	MAC_AX_DP_INTN_IDX_WDE_DLE_15 = 0x15,
	MAC_AX_DP_INTN_IDX_WDE_DLE_16 = 0x16,
	MAC_AX_DP_INTN_IDX_WDE_DLE_17 = 0x17,
	MAC_AX_DP_INTN_IDX_WDE_DLE_18 = 0x18,
	MAC_AX_DP_INTN_IDX_WDE_DLE_MAX = 0x19
};

// MAC_AX_DP_SEL_PLE_DLE = 0x82,
enum mac_ax_dbgport_intn_idx_ple_dle {
	MAC_AX_DP_INTN_IDX_PLE_DLE_0 = 0x0,
	MAC_AX_DP_INTN_IDX_PLE_DLE_1 = 0x1,
	MAC_AX_DP_INTN_IDX_PLE_DLE_2 = 0x2,
	MAC_AX_DP_INTN_IDX_PLE_DLE_3 = 0x3,
	MAC_AX_DP_INTN_IDX_PLE_DLE_4 = 0x4,
	MAC_AX_DP_INTN_IDX_PLE_DLE_5 = 0x5,
	MAC_AX_DP_INTN_IDX_PLE_DLE_6 = 0x6,
	MAC_AX_DP_INTN_IDX_PLE_DLE_7 = 0x7,
	MAC_AX_DP_INTN_IDX_PLE_DLE_8 = 0x8,
	MAC_AX_DP_INTN_IDX_PLE_DLE_9 = 0x9,
	MAC_AX_DP_INTN_IDX_PLE_DLE_A = 0xA,
	MAC_AX_DP_INTN_IDX_PLE_DLE_B = 0xB,
	MAC_AX_DP_INTN_IDX_PLE_DLE_C = 0xC,
	MAC_AX_DP_INTN_IDX_PLE_DLE_D = 0xD,
	MAC_AX_DP_INTN_IDX_PLE_DLE_E = 0xE,
	MAC_AX_DP_INTN_IDX_PLE_DLE_F = 0xF,
	MAC_AX_DP_INTN_IDX_PLE_DLE_10 = 0x10,
	MAC_AX_DP_INTN_IDX_PLE_DLE_11 = 0x11,
	MAC_AX_DP_INTN_IDX_PLE_DLE_12 = 0x12,
	MAC_AX_DP_INTN_IDX_PLE_DLE_13 = 0x13,
	MAC_AX_DP_INTN_IDX_PLE_DLE_14 = 0x14,
	MAC_AX_DP_INTN_IDX_PLE_DLE_15 = 0x15,
	MAC_AX_DP_INTN_IDX_PLE_DLE_16 = 0x16,
	MAC_AX_DP_INTN_IDX_PLE_DLE_17 = 0x17,
	MAC_AX_DP_INTN_IDX_PLE_DLE_18 = 0x18,
	MAC_AX_DP_INTN_IDX_PLE_DLE_19 = 0x19,
	MAC_AX_DP_INTN_IDX_PLE_DLE_1A = 0x1A,
	MAC_AX_DP_INTN_IDX_PLE_DLE_1B = 0x1B,
	MAC_AX_DP_INTN_IDX_PLE_DLE_MAX = 0x1C
};

// MAC_AX_DP_SEL_WDRLS = 0x83,
// MAC_AX_DP_SEL_DLE_CPUIO = 0x84,
// MAC_AX_DP_SEL_BBRPT = 0x85,
// MAC_AX_DP_SEL_TXPKTCTL = 0x86,
// MAC_AX_DP_SEL_PKTBUFFER = 0x87,
// MAC_AX_DP_SEL_DMAC_TABLE = 0x88,
// MAC_AX_DP_SEL_STA_SCHEDULER = 0x89,
enum mac_ax_dbgport_intn_idx_sta_scheduler {
	MAC_AX_DP_INTN_IDX_STA_SCHEDULER_0 = 0x0,
	MAC_AX_DP_INTN_IDX_STA_SCHEDULER_1 = 0x1,
	MAC_AX_DP_INTN_IDX_STA_SCHEDULER_2 = 0x2,
	MAC_AX_DP_INTN_IDX_STA_SCHEDULER_3 = 0x3,
	MAC_AX_DP_INTN_IDX_STA_SCHEDULER_4 = 0x4,
	MAC_AX_DP_INTN_IDX_STA_SCHEDULER_5 = 0x5,
	MAC_AX_DP_INTN_IDX_STA_SCHEDULER_6 = 0x6,
	MAC_AX_DP_INTN_IDX_STA_SCHEDULER_7 = 0x7,
	MAC_AX_DP_INTN_IDX_STA_SCHEDULER_8 = 0x8,
	MAC_AX_DP_INTN_IDX_STA_SCHEDULER_9 = 0x9,
	MAC_AX_DP_INTN_IDX_STA_SCHEDULER_A = 0xA,
	MAC_AX_DP_INTN_IDX_STA_SCHEDULER_B = 0xB,
	MAC_AX_DP_INTN_IDX_STA_SCHEDULER_C = 0xC,
	MAC_AX_DP_INTN_IDX_STA_SCHEDULER_D = 0xD,
	MAC_AX_DP_INTN_IDX_STA_SCHEDULER_E = 0xE,
	MAC_AX_DP_INTN_IDX_STA_SCHEDULER_F = 0xF,
	MAC_AX_DP_INTN_IDX_STA_SCHEDULER_10 = 0x10,
	MAC_AX_DP_INTN_IDX_STA_SCHEDULER_MAX = 0x11
};

// MAC_AX_DP_SEL_DMAC_PKTIN = 0x8A,
// MAC_AX_DP_SEL_WSEC_TOP = 0x8B,
// MAC_AX_DP_SEL_MPDU_PROCESSOR = 0x8C,
// MAC_AX_DP_SEL_DMAC_APB_BRIDGE = 0x8D,
// MAC_AX_DP_SEL_LTR_CTRL = 0x8E,
// CMAC_0
// MAC_AX_DP_SEL_CMAC0_CMAC_DMAC_TOP = 0xA0,
// MAC_AX_DP_SEL_CMAC0_PTCLTOP = 0xA1,
// MAC_AX_DP_SEL_CMAC0_SCHEDULERTOP = 0xA2,
// MAC_AX_DP_SEL_CMAC0_TXPWR_CTRL = 0xA3,
// MAC_AX_DP_SEL_CMAC0_CMAC_APB_BRIDGE = 0xA4,
// MAC_AX_DP_SEL_CMAC0_MACTX = 0xA5,
// MAC_AX_DP_SEL_CMAC0_MACRX = 0xA6,
// MAC_AX_DP_SEL_CMAC0_WMAC_TRXPTCL = 0xA7,
// CMAC_1
// MAC_AX_DP_SEL_CMAC1_CMAC_DMAC_TOP = 0xB0,
// MAC_AX_DP_SEL_CMAC1_PTCLTOP = 0xB1,
// MAC_AX_DP_SEL_CMAC1_SCHEDULERTOP = 0xB2,
// MAC_AX_DP_SEL_CMAC1_TXPWR_CTRL = 0xB3,
// MAC_AX_DP_SEL_CMAC1_CMAC_APB_BRIDGE = 0xB4,
// MAC_AX_DP_SEL_CMAC1_MACTX = 0xB5,
// MAC_AX_DP_SEL_CMAC1_MACRX = 0xB6,
// MAC_AX_DP_SEL_CMAC1_WMAC_TRXPTCL = 0xB7,
// MAC_AX_DP_SEL_CMAC_SHARE = 0xC0,

/* WLAN_MAC */
// MAC_AX_DP_SEL_WL_CPU_0 = 0xF0, //0xF0-0xFF
// MAC_AX_DP_SEL_WL_CPU_F = 0xFF,

/**
 * @brief mac_dbgport_hw_dump
 *
 * @param *adapter
 * @param *dp_hw_en
 * @return Please Place Description here.
 * @retval u32
 */
u32 dbgport_hw_dump(struct mac_ax_adapter *adapter,
		    struct mac_ax_dbgport_hw_en *dp_hw_en);

/**
 * @brief mac_dbgport_hw_set
 *
 * @param *adapter
 * @param *dbgport_hw
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_dbgport_hw_set(struct mac_ax_adapter *adapter,
		       struct mac_ax_dbgport_hw *dp_hw);

#endif

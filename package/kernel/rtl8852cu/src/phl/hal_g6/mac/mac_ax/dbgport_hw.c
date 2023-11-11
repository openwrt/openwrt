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

#include "dbgpkg.h"
#include "dbgport_hw.h"

static u32 dp_intn_idx_set(struct mac_ax_adapter *adapter,
			   struct mac_ax_dbgport_hw *dp_hw, u8 sel_idx)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32, intn_val;
	u16 val16;
	u8 dbg_sel, intn_idx;

	dbg_sel = dp_hw->dbg_sel[sel_idx];
	intn_idx = dp_hw->intn_idx[sel_idx];

	switch (dbg_sel) {
	case MAC_AX_DP_SEL_SYS_0:
	case MAC_AX_DP_SEL_SYS_1F:
		/* To do... */
		break;
	case MAC_AX_DP_SEL_PINMUX_0:
	case MAC_AX_DP_SEL_PINMUX_7:
		/* To do... */
		break;
	case MAC_AX_DP_SEL_LOADER_0:
	case MAC_AX_DP_SEL_LOADER_3:
		/* To do... */
		break;
	case MAC_AX_DP_SEL_HMUX_0:
	case MAC_AX_DP_SEL_HMUX_3:
		/* To do... */
		break;
	case MAC_AX_DP_SEL_PCIE_0:
		switch (intn_idx) {
		case MAC_AX_DP_INTN_IDX_PCIE_0_0:
		case MAC_AX_DP_INTN_IDX_PCIE_0_1:
		case MAC_AX_DP_INTN_IDX_PCIE_0_2:
		case MAC_AX_DP_INTN_IDX_PCIE_0_3:
			break;
		default:
			return MACNOITEM;
		}
#if (MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT)
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
			val32 = MAC_REG_R32(R_AX_PCIE_DBG_CTRL);
			val32 = SET_CLR_WORD(val32, intn_idx, B_AX_DBG_SEL);
			MAC_REG_W32(R_AX_PCIE_DBG_CTRL, val32);
		}
#endif
#if MAC_AX_8852C_SUPPORT || MAC_AX_8192XB_SUPPORT || MAC_AX_8851E_SUPPORT || MAC_AX_8852D_SUPPORT
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
			val32 = MAC_REG_R32(R_AX_HAXI_DBG_CTRL);
			val32 = SET_CLR_WORD(val32, intn_idx, B_AX_DBG_SEL);
			MAC_REG_W32(R_AX_HAXI_DBG_CTRL, val32);
		}
#endif
		break;
	case MAC_AX_DP_SEL_PCIE_1:
		switch (intn_idx) {
		case MAC_AX_DP_INTN_IDX_PCIE_1_0:
		case MAC_AX_DP_INTN_IDX_PCIE_1_1:
		case MAC_AX_DP_INTN_IDX_PCIE_1_2:
		case MAC_AX_DP_INTN_IDX_PCIE_1_3:
		case MAC_AX_DP_INTN_IDX_PCIE_1_4:
			break;
		default:
			return MACNOITEM;
		}
#if (MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT)
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
			val32 = MAC_REG_R32(R_AX_PCIE_DBG_CTRL);
			val32 = SET_CLR_WORD(val32, intn_idx, B_AX_DBG_SEL);
			MAC_REG_W32(R_AX_PCIE_DBG_CTRL, val32);
		}
#endif
#if MAC_AX_8852C_SUPPORT || MAC_AX_8192XB_SUPPORT || MAC_AX_8851E_SUPPORT || MAC_AX_8852D_SUPPORT
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
			val32 = MAC_REG_R32(R_AX_HAXI_DBG_CTRL);
			val32 = SET_CLR_WORD(val32, intn_idx, B_AX_DBG_SEL);
			MAC_REG_W32(R_AX_HAXI_DBG_CTRL, val32);
		}
#endif
		break;
	case MAC_AX_DP_SEL_PCIE_2:
		switch (intn_idx) {
		case MAC_AX_DP_INTN_IDX_PCIE_2_0:
		case MAC_AX_DP_INTN_IDX_PCIE_2_1:
			break;
		default:
			return MACNOITEM;
		}
#if (MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT)
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
			val32 = MAC_REG_R32(R_AX_PCIE_DBG_CTRL);
			val32 = SET_CLR_WORD(val32, intn_idx, B_AX_DBG_SEL);
			MAC_REG_W32(R_AX_PCIE_DBG_CTRL, val32);
		}
#endif
#if MAC_AX_8852C_SUPPORT || MAC_AX_8192XB_SUPPORT || MAC_AX_8851E_SUPPORT || MAC_AX_8852D_SUPPORT
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
			val32 = MAC_REG_R32(R_AX_HAXI_DBG_CTRL);
			val32 = SET_CLR_WORD(val32, intn_idx, B_AX_DBG_SEL);
			MAC_REG_W32(R_AX_HAXI_DBG_CTRL, val32);
		}
#endif
		break;
	case MAC_AX_DP_SEL_PCIE_3:
		switch (intn_idx) {
		case MAC_AX_DP_INTN_IDX_PCIE_3_0:
		case MAC_AX_DP_INTN_IDX_PCIE_3_1:
		case MAC_AX_DP_INTN_IDX_PCIE_3_2:
		case MAC_AX_DP_INTN_IDX_PCIE_3_3:
		case MAC_AX_DP_INTN_IDX_PCIE_3_4:
		case MAC_AX_DP_INTN_IDX_PCIE_3_5:
			break;
		default:
			return MACNOITEM;
		}
#if (MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT)
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
			val32 = MAC_REG_R32(R_AX_PCIE_DBG_CTRL);
			val32 = SET_CLR_WORD(val32, intn_idx, B_AX_DBG_SEL);
			MAC_REG_W32(R_AX_PCIE_DBG_CTRL, val32);
		}
#endif
#if MAC_AX_8852C_SUPPORT || MAC_AX_8192XB_SUPPORT || MAC_AX_8851E_SUPPORT || MAC_AX_8852D_SUPPORT
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
			val32 = MAC_REG_R32(R_AX_HAXI_DBG_CTRL);
			val32 = SET_CLR_WORD(val32, intn_idx, B_AX_DBG_SEL);
			MAC_REG_W32(R_AX_HAXI_DBG_CTRL, val32);
		}
#endif
		break;
	case MAC_AX_DP_SEL_PCIE_4:
	case MAC_AX_DP_SEL_PCIE_5:
	case MAC_AX_DP_SEL_PCIE_6:
		// don't have internal setting
		break;
	case MAC_AX_DP_SEL_PCIE_7:
		switch (intn_idx) {
		case MAC_AX_DP_INTN_IDX_PCIE_7_0:
			intn_val = 0x0;
			break;
		case MAC_AX_DP_INTN_IDX_PCIE_7_1:
			intn_val = 0x4;
			break;
		case MAC_AX_DP_INTN_IDX_PCIE_7_2:
			intn_val = 0x1;
			break;
		case MAC_AX_DP_INTN_IDX_PCIE_7_3:
			intn_val = 0x5;
			break;
		case MAC_AX_DP_INTN_IDX_PCIE_7_4:
			intn_val = 0x2;
			break;
		default:
			return MACNOITEM;
		}
#if (MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT)
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
			val32 = MAC_REG_R32(R_AX_PCIE_DBG_CTRL);
			val32 = SET_CLR_WORD(val32, intn_val, B_AX_DBG_SEL);
			MAC_REG_W32(R_AX_PCIE_DBG_CTRL, val32);
		}
#endif
#if MAC_AX_8852C_SUPPORT || MAC_AX_8192XB_SUPPORT || MAC_AX_8851E_SUPPORT || MAC_AX_8852D_SUPPORT
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
			val32 = MAC_REG_R32(R_AX_HAXI_DBG_CTRL);
			val32 = SET_CLR_WORD(val32, intn_val, B_AX_DBG_SEL);
			MAC_REG_W32(R_AX_HAXI_DBG_CTRL, val32);
		}
#endif
		break;
	case MAC_AX_DP_SEL_PCIE_8:
		switch (intn_idx) {
		case MAC_AX_DP_INTN_IDX_PCIE_8_0:
		case MAC_AX_DP_INTN_IDX_PCIE_8_1:
		case MAC_AX_DP_INTN_IDX_PCIE_8_2:
		case MAC_AX_DP_INTN_IDX_PCIE_8_3:
		case MAC_AX_DP_INTN_IDX_PCIE_8_4:
		case MAC_AX_DP_INTN_IDX_PCIE_8_5:
		case MAC_AX_DP_INTN_IDX_PCIE_8_6:
			break;
		default:
			return MACNOITEM;
		}
#if (MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT)
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
			val32 = MAC_REG_R32(R_AX_PCIE_DBG_CTRL);
			val32 = SET_CLR_WORD(val32, intn_idx, B_AX_DBG_SEL);
			MAC_REG_W32(R_AX_PCIE_DBG_CTRL, val32);
		}
#endif
#if MAC_AX_8852C_SUPPORT || MAC_AX_8192XB_SUPPORT || MAC_AX_8851E_SUPPORT || MAC_AX_8852D_SUPPORT
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
			val32 = MAC_REG_R32(R_AX_HAXI_DBG_CTRL);
			val32 = SET_CLR_WORD(val32, intn_idx, B_AX_DBG_SEL);
			MAC_REG_W32(R_AX_HAXI_DBG_CTRL, val32);
		}
#endif
		break;
	case MAC_AX_DP_SEL_PCIE_9:
	case MAC_AX_DP_SEL_PCIE_A:
	case MAC_AX_DP_SEL_PCIE_B:
	case MAC_AX_DP_SEL_PCIE_C:
		// don't have internal setting
		break;
	case MAC_AX_DP_SEL_PCIE_D:
	case MAC_AX_DP_SEL_PCIE_E:
	case MAC_AX_DP_SEL_PCIE_F:
		// don't have this item
		return MACNOITEM;
	case MAC_AX_DP_SEL_USB_0:
	case MAC_AX_DP_SEL_USB_F:
		/* To do... */
		break;
	case MAC_AX_DP_SEL_SDIO_0:
	case MAC_AX_DP_SEL_SDIO_F:
		/* To do... */
		break;
	case MAC_AX_DP_SEL_BT:
		/* To do... */
		break;
	case MAC_AX_DP_SEL_AXIDMA:
		switch (intn_idx) {
		case MAC_AX_DP_INTN_IDX_AXIDMA_0:
			intn_val = (0x0 << AXIDMA_DBG_SEL_INTN0_SH) |
				   (0x0 << AXIDMA_DBG_SEL_INTN1_SH) |
				   (0x0 << AXIDMA_DBG_SEL_INTN2_SH);
			break;
		case MAC_AX_DP_INTN_IDX_AXIDMA_1:
			intn_val = (0x1 << AXIDMA_DBG_SEL_INTN0_SH) |
				   (0x1 << AXIDMA_DBG_SEL_INTN1_SH) |
				   (0x0 << AXIDMA_DBG_SEL_INTN2_SH);
			break;
		case MAC_AX_DP_INTN_IDX_AXIDMA_2:
			intn_val = (0x2 << AXIDMA_DBG_SEL_INTN0_SH) |
				   (0x2 << AXIDMA_DBG_SEL_INTN1_SH) |
				   (0x0 << AXIDMA_DBG_SEL_INTN2_SH);
			break;
		case MAC_AX_DP_INTN_IDX_AXIDMA_3:
			intn_val = (0x3 << AXIDMA_DBG_SEL_INTN0_SH) |
				   (0x3 << AXIDMA_DBG_SEL_INTN1_SH) |
				   (0x0 << AXIDMA_DBG_SEL_INTN2_SH);
			break;
		case MAC_AX_DP_INTN_IDX_AXIDMA_4:
			intn_val = (0x0 << AXIDMA_DBG_SEL_INTN0_SH) |
				   (0x0 << AXIDMA_DBG_SEL_INTN1_SH) |
				   (0x1 << AXIDMA_DBG_SEL_INTN2_SH);
			break;
		case MAC_AX_DP_INTN_IDX_AXIDMA_5:
			intn_val = (0x1 << AXIDMA_DBG_SEL_INTN0_SH) |
				   (0x1 << AXIDMA_DBG_SEL_INTN1_SH) |
				   (0x1 << AXIDMA_DBG_SEL_INTN2_SH);
			break;
		case MAC_AX_DP_INTN_IDX_AXIDMA_6:
			intn_val = (0x2 << AXIDMA_DBG_SEL_INTN0_SH) |
				   (0x2 << AXIDMA_DBG_SEL_INTN1_SH) |
				   (0x1 << AXIDMA_DBG_SEL_INTN2_SH);
			break;
		case MAC_AX_DP_INTN_IDX_AXIDMA_7:
			intn_val = (0x3 << AXIDMA_DBG_SEL_INTN0_SH) |
				   (0x3 << AXIDMA_DBG_SEL_INTN1_SH) |
				   (0x1 << AXIDMA_DBG_SEL_INTN2_SH);
			break;
		case MAC_AX_DP_INTN_IDX_AXIDMA_8:
			intn_val = (0x4 << AXIDMA_DBG_SEL_INTN0_SH) |
				   (0x4 << AXIDMA_DBG_SEL_INTN1_SH) |
				   (0x1 << AXIDMA_DBG_SEL_INTN2_SH);
			break;
		case MAC_AX_DP_INTN_IDX_AXIDMA_9:
			intn_val = (0x5 << AXIDMA_DBG_SEL_INTN0_SH) |
				   (0x5 << AXIDMA_DBG_SEL_INTN1_SH) |
				   (0x1 << AXIDMA_DBG_SEL_INTN2_SH);
			break;
		case MAC_AX_DP_INTN_IDX_AXIDMA_A:
			intn_val = (0x6 << AXIDMA_DBG_SEL_INTN0_SH) |
				   (0x6 << AXIDMA_DBG_SEL_INTN1_SH) |
				   (0x1 << AXIDMA_DBG_SEL_INTN2_SH);
			break;
		case MAC_AX_DP_INTN_IDX_AXIDMA_B:
			intn_val = (0x0 << AXIDMA_DBG_SEL_INTN0_SH) |
				   (0x0 << AXIDMA_DBG_SEL_INTN1_SH) |
				   (0x2 << AXIDMA_DBG_SEL_INTN2_SH);
			break;
		case MAC_AX_DP_INTN_IDX_AXIDMA_C:
			intn_val = (0x0 << AXIDMA_DBG_SEL_INTN0_SH) |
				   (0x0 << AXIDMA_DBG_SEL_INTN1_SH) |
				   (0x3 << AXIDMA_DBG_SEL_INTN2_SH);
			break;
		case MAC_AX_DP_INTN_IDX_AXIDMA_D:
			intn_val = (0x0 << AXIDMA_DBG_SEL_INTN0_SH) |
				   (0x0 << AXIDMA_DBG_SEL_INTN1_SH) |
				   (0x4 << AXIDMA_DBG_SEL_INTN2_SH);
			break;
		default:
			return MACNOITEM;
		}
		MAC_REG_W32(R_AX_FILTER_MODEL_ADDR, AXIDMA_BASE_ADDR);
		val32 = MAC_REG_R32(R_AX_INDIR_ACCESS_ENTRY + R_PL_AXIDMA_DBG_CTRL);
		val32 = SET_CLR_WORD(val32, intn_val, B_PL_AXIDMA_DBG_SEL);
		MAC_REG_W32(R_AX_INDIR_ACCESS_ENTRY + R_PL_AXIDMA_DBG_CTRL, val32);
		break;
	case MAC_AX_DP_SEL_WLPHYDBG_GPIO:
		/* To do... */
		break;
	case MAC_AX_DP_SEL_BTCOEXIST:
		/* To do... */
		break;
	case MAC_AX_DP_SEL_LTECOEX:
		/* To do... */
		break;
	case MAC_AX_DP_SEL_WLPHYDBG:
		/* To do... */
		break;
	case MAC_AX_DP_SEL_WLAN_MAC_REG:
		/* To do... */
		break;
	case MAC_AX_DP_SEL_WLAN_MAC_PMC:
		/* To do... */
		break;
	case MAC_AX_DP_SEL_CALIB_TOP:
		/* To do... */
		break;
	case MAC_AX_DP_SEL_DISPATCHER_TOP:
		/* To do... */
		break;
	case MAC_AX_DP_SEL_WDE_DLE:
		switch (intn_idx) {
		case MAC_AX_DP_INTN_IDX_WDE_DLE_0:
			intn_val = 0x0;
			break;
		case MAC_AX_DP_INTN_IDX_WDE_DLE_1:
			intn_val = 0x1;
			break;
		case MAC_AX_DP_INTN_IDX_WDE_DLE_2:
			intn_val = 0xE;
			break;
		case MAC_AX_DP_INTN_IDX_WDE_DLE_3:
			intn_val = 0x10;
			break;
		case MAC_AX_DP_INTN_IDX_WDE_DLE_4:
			intn_val = 0x11;
			break;
		case MAC_AX_DP_INTN_IDX_WDE_DLE_5:
			intn_val = 0x14;
			break;
		case MAC_AX_DP_INTN_IDX_WDE_DLE_6:
			intn_val = 0x1E;
			break;
		case MAC_AX_DP_INTN_IDX_WDE_DLE_7:
			intn_val = 0x80;
			break;
		case MAC_AX_DP_INTN_IDX_WDE_DLE_8:
			intn_val = 0x81;
			break;
		case MAC_AX_DP_INTN_IDX_WDE_DLE_9:
			intn_val = 0x82;
			break;
		case MAC_AX_DP_INTN_IDX_WDE_DLE_A:
			intn_val = 0x90;
			break;
		case MAC_AX_DP_INTN_IDX_WDE_DLE_B:
			intn_val = 0x91;
			break;
		case MAC_AX_DP_INTN_IDX_WDE_DLE_C:
			intn_val = 0x92;
			break;
		case MAC_AX_DP_INTN_IDX_WDE_DLE_D:
			intn_val = 0xB0;
			break;
		case MAC_AX_DP_INTN_IDX_WDE_DLE_E:
			intn_val = 0xB1;
			break;
		case MAC_AX_DP_INTN_IDX_WDE_DLE_F:
			intn_val = 0xB2;
			break;
		case MAC_AX_DP_INTN_IDX_WDE_DLE_10:
			intn_val = 0xC0;
			break;
		case MAC_AX_DP_INTN_IDX_WDE_DLE_11:
			intn_val = 0xC1;
			break;
		case MAC_AX_DP_INTN_IDX_WDE_DLE_12:
			intn_val = 0xC2;
			break;
		case MAC_AX_DP_INTN_IDX_WDE_DLE_13:
			intn_val = 0xE0;
			break;
		case MAC_AX_DP_INTN_IDX_WDE_DLE_14:
			intn_val = 0xE1;
			break;
		case MAC_AX_DP_INTN_IDX_WDE_DLE_15:
			intn_val = 0xE2;
			break;
		case MAC_AX_DP_INTN_IDX_WDE_DLE_16:
			intn_val = 0xF0;
			break;
		case MAC_AX_DP_INTN_IDX_WDE_DLE_17:
			intn_val = 0xF1;
			break;
		case MAC_AX_DP_INTN_IDX_WDE_DLE_18:
			intn_val = 0xF2;
			break;
		default:
			return MACNOITEM;
		}
		val16 = MAC_REG_R16(R_AX_WDE_DBG_CTL);
		val16 = SET_CLR_WORD(val16, intn_val, B_AX_WDE_DBG0_SEL);
		val16 = SET_CLR_WORD(val16, intn_val, B_AX_WDE_DBG1_SEL);
		MAC_REG_W16(R_AX_WDE_DBG_CTL, val16);
		break;
	case MAC_AX_DP_SEL_PLE_DLE:
		switch (intn_idx) {
		case MAC_AX_DP_INTN_IDX_PLE_DLE_0:
			intn_val = 0x0;
			break;
		case MAC_AX_DP_INTN_IDX_PLE_DLE_1:
			intn_val = 0x1;
			break;
		case MAC_AX_DP_INTN_IDX_PLE_DLE_2:
			intn_val = 0xE;
			break;
		case MAC_AX_DP_INTN_IDX_PLE_DLE_3:
			intn_val = 0x10;
			break;
		case MAC_AX_DP_INTN_IDX_PLE_DLE_4:
			intn_val = 0x11;
			break;
		case MAC_AX_DP_INTN_IDX_PLE_DLE_5:
			intn_val = 0x14;
			break;
		case MAC_AX_DP_INTN_IDX_PLE_DLE_6:
			intn_val = 0x1E;
			break;
		case MAC_AX_DP_INTN_IDX_PLE_DLE_7:
			intn_val = 0x80;
			break;
		case MAC_AX_DP_INTN_IDX_PLE_DLE_8:
			intn_val = 0x81;
			break;
		case MAC_AX_DP_INTN_IDX_PLE_DLE_9:
			intn_val = 0x82;
			break;
		case MAC_AX_DP_INTN_IDX_PLE_DLE_A:
			intn_val = 0x90;
			break;
		case MAC_AX_DP_INTN_IDX_PLE_DLE_B:
			intn_val = 0x91;
			break;
		case MAC_AX_DP_INTN_IDX_PLE_DLE_C:
			intn_val = 0x92;
			break;
		case MAC_AX_DP_INTN_IDX_PLE_DLE_D:
			intn_val = 0xA0;
			break;
		case MAC_AX_DP_INTN_IDX_PLE_DLE_E:
			intn_val = 0xA1;
			break;
		case MAC_AX_DP_INTN_IDX_PLE_DLE_F:
			intn_val = 0xA2;
			break;
		case MAC_AX_DP_INTN_IDX_PLE_DLE_10:
			intn_val = 0xB0;
			break;
		case MAC_AX_DP_INTN_IDX_PLE_DLE_11:
			intn_val = 0xB1;
			break;
		case MAC_AX_DP_INTN_IDX_PLE_DLE_12:
			intn_val = 0xB2;
			break;
		case MAC_AX_DP_INTN_IDX_PLE_DLE_13:
			intn_val = 0xC0;
			break;
		case MAC_AX_DP_INTN_IDX_PLE_DLE_14:
			intn_val = 0xC1;
			break;
		case MAC_AX_DP_INTN_IDX_PLE_DLE_15:
			intn_val = 0xC2;
			break;
		case MAC_AX_DP_INTN_IDX_PLE_DLE_16:
			intn_val = 0xD0;
			break;
		case MAC_AX_DP_INTN_IDX_PLE_DLE_17:
			intn_val = 0xD1;
			break;
		case MAC_AX_DP_INTN_IDX_PLE_DLE_18:
			intn_val = 0xD2;
			break;
		case MAC_AX_DP_INTN_IDX_PLE_DLE_19:
			intn_val = 0xE0;
			break;
		case MAC_AX_DP_INTN_IDX_PLE_DLE_1A:
			intn_val = 0xE1;
			break;
		case MAC_AX_DP_INTN_IDX_PLE_DLE_1B:
			intn_val = 0xE2;
			break;
		default:
			return MACNOITEM;
		}
		val16 = MAC_REG_R16(R_AX_PLE_DBG_CTL);
		val16 = SET_CLR_WORD(val16, intn_val, B_AX_PLE_DBG0_SEL);
		val16 = SET_CLR_WORD(val16, intn_val, B_AX_PLE_DBG1_SEL);
		MAC_REG_W16(R_AX_PLE_DBG_CTL, val16);
		break;
	case MAC_AX_DP_SEL_WDRLS:
		/* To do... */
		break;
	case MAC_AX_DP_SEL_DLE_CPUIO:
		/* To do... */
		break;
	case MAC_AX_DP_SEL_BBRPT:
		/* To do... */
		break;
	case MAC_AX_DP_SEL_TXPKTCTL:
		/* To do... */
		break;
	case MAC_AX_DP_SEL_PKTBUFFER:
		/* To do... */
		break;
	case MAC_AX_DP_SEL_DMAC_TABLE:
		/* To do... */
		break;
	case MAC_AX_DP_SEL_STA_SCHEDULER:
		val32 = MAC_REG_R32(R_AX_SS_DBG_0);
		val32 = SET_CLR_WORD(val32, 0x1, B_AX_SS_LM_STAT);
		val32 = SET_CLR_WORD(val32, 0x1, B_AX_SS_SA_STAT);
		val32 = SET_CLR_WORD(val32, 0x1, B_AX_SS_PC_STAT);
		val32 = SET_CLR_WORD(val32, 0x1, B_AX_SS_PARAM_STAT);
		MAC_REG_W32(R_AX_SS_DBG_0, val32);

		val32 = MAC_REG_R32(R_AX_SS_DBG_1);
		val32 = SET_CLR_WORD(val32, 0x1, B_AX_SS_LEN_STAT);
		val32 = SET_CLR_WORD(val32, 0x1, B_AX_SS_DLTX_STAT);
		val32 = SET_CLR_WORD(val32, 0x1, B_AX_SS_ULRU_STAT);
		val32 = SET_CLR_WORD(val32, 0x1, B_AX_SS_ADD_STAT);
		val32 = SET_CLR_WORD(val32, 0x1, B_AX_SS_DEL_STAT);
		MAC_REG_W32(R_AX_SS_DBG_1, val32);

		val32 = MAC_REG_R32(R_AX_SS_DBG_2);
		val32 = SET_CLR_WORD(val32, 0x1, B_AX_SS_FWTX_STAT);
		val32 = SET_CLR_WORD(val32, 0x1, B_AX_SS_RPTA_STAT);
#if (MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT)
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
			val32 = SET_CLR_WORD(val32, 0x1, B_AX_SS_WDEA_STAT);
			val32 = SET_CLR_WORD(val32, 0x1, B_AX_SS_PLEA_STAT);
		}
#endif
#if MAC_AX_8852C_SUPPORT || MAC_AX_8192XB_SUPPORT || MAC_AX_8851E_SUPPORT || MAC_AX_8852D_SUPPORT
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
			val32 = SET_CLR_WORD(val32, 0x1, B_AX_SS_WDEA_STAT_V1);
			val32 = SET_CLR_WORD(val32, 0x1, B_AX_SS_PLEA_STAT_V1);
		}
#endif
		MAC_REG_W32(R_AX_SS_DBG_2, val32);

#if (MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT)
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
			val32 = MAC_REG_R32(R_AX_SS_MU_CTRL);
			val32 = SET_CLR_WORD(val32, 0x1, B_AX_SS_DLMU_STATE);
			val32 = SET_CLR_WORD(val32, 0x1, B_AX_SS_DLRU_STATE);
			MAC_REG_W32(R_AX_SS_MU_CTRL, val32);
		}
#endif

		switch (intn_idx) {
		case MAC_AX_DP_INTN_IDX_STA_SCHEDULER_0:
		case MAC_AX_DP_INTN_IDX_STA_SCHEDULER_1:
		case MAC_AX_DP_INTN_IDX_STA_SCHEDULER_2:
		case MAC_AX_DP_INTN_IDX_STA_SCHEDULER_3:
		case MAC_AX_DP_INTN_IDX_STA_SCHEDULER_4:
		case MAC_AX_DP_INTN_IDX_STA_SCHEDULER_5:
		case MAC_AX_DP_INTN_IDX_STA_SCHEDULER_6:
		case MAC_AX_DP_INTN_IDX_STA_SCHEDULER_7:
		case MAC_AX_DP_INTN_IDX_STA_SCHEDULER_8:
		case MAC_AX_DP_INTN_IDX_STA_SCHEDULER_9:
		case MAC_AX_DP_INTN_IDX_STA_SCHEDULER_A:
		case MAC_AX_DP_INTN_IDX_STA_SCHEDULER_B:
		case MAC_AX_DP_INTN_IDX_STA_SCHEDULER_C:
		case MAC_AX_DP_INTN_IDX_STA_SCHEDULER_D:
		case MAC_AX_DP_INTN_IDX_STA_SCHEDULER_E:
		case MAC_AX_DP_INTN_IDX_STA_SCHEDULER_F:
		case MAC_AX_DP_INTN_IDX_STA_SCHEDULER_10:
			break;
		default:
			return MACNOITEM;
		}
		val32 = MAC_REG_R32(R_AX_SS_DBG_3);
#if (MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT)
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
			val32 = SET_CLR_WORD(val32, intn_idx,
					     B_AX_SS_TOP_DBG_SEL);
		}
#endif
#if MAC_AX_8852C_SUPPORT || MAC_AX_8192XB_SUPPORT || MAC_AX_8851E_SUPPORT || MAC_AX_8852D_SUPPORT
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
			val32 = SET_CLR_WORD(val32, intn_idx,
					     B_AX_SS_TOP_DBG_SEL_V1);
		}
#endif
		MAC_REG_W32(R_AX_SS_DBG_3, val32);
		break;
	case MAC_AX_DP_SEL_DMAC_PKTIN:
		/* To do... */
		break;
	case MAC_AX_DP_SEL_WSEC_TOP:
		/* To do... */
		break;
	case MAC_AX_DP_SEL_MPDU_PROCESSOR:
		/* To do... */
		break;
	case MAC_AX_DP_SEL_DMAC_APB_BRIDGE:
		/* To do... */
		break;
	case MAC_AX_DP_SEL_LTR_CTRL:
		/* To do... */
		break;
	case MAC_AX_DP_SEL_CMAC0_CMAC_DMAC_TOP:
		/* To do... */
		break;
	case MAC_AX_DP_SEL_CMAC0_PTCLTOP:
		/* To do... */
		break;
	case MAC_AX_DP_SEL_CMAC0_SCHEDULERTOP:
		/* To do... */
		break;
	case MAC_AX_DP_SEL_CMAC0_TXPWR_CTRL:
		/* To do... */
		break;
	case MAC_AX_DP_SEL_CMAC0_CMAC_APB_BRIDGE:
		/* To do... */
		break;
	case MAC_AX_DP_SEL_CMAC0_MACTX:
		/* To do... */
		break;
	case MAC_AX_DP_SEL_CMAC0_MACRX:
		/* To do... */
		break;
	case MAC_AX_DP_SEL_CMAC0_WMAC_TRXPTCL:
		/* To do... */
		break;
	case MAC_AX_DP_SEL_CMAC1_CMAC_DMAC_TOP:
		/* To do... */
		break;
	case MAC_AX_DP_SEL_CMAC1_PTCLTOP:
		/* To do... */
		break;
	case MAC_AX_DP_SEL_CMAC1_SCHEDULERTOP:
		/* To do... */
		break;
	case MAC_AX_DP_SEL_CMAC1_TXPWR_CTRL:
		/* To do... */
		break;
	case MAC_AX_DP_SEL_CMAC1_CMAC_APB_BRIDGE:
		/* To do... */
		break;
	case MAC_AX_DP_SEL_CMAC1_MACTX:
		/* To do... */
		break;
	case MAC_AX_DP_SEL_CMAC1_MACRX:
		/* To do... */
		break;
	case MAC_AX_DP_SEL_CMAC1_WMAC_TRXPTCL:
		/* To do... */
		break;
	case MAC_AX_DP_SEL_CMAC_SHARE:
		/* To do... */
		break;
	/* WLAN_MAC */
	case MAC_AX_DP_SEL_WL_CPU_0:
	case MAC_AX_DP_SEL_WL_CPU_F:
		/* To do... */
		break;
	default:
		return MACNOITEM;
	}

	return MACSUCCESS;
}

static u32 dp_intn_dump(struct mac_ax_adapter *adapter,
			u8 dbg_sel, u8 intn_idx_max)
{
	struct mac_ax_dbgport_hw dp_hw;
	u32 ret = MACSUCCESS;
	u8 intn_idx;

	PLTFM_MEMSET(&dp_hw, 0, sizeof(struct mac_ax_dbgport_hw));

	for (intn_idx = 0; intn_idx < intn_idx_max; intn_idx++) {
		dp_hw.dbg_sel[0] = dbg_sel;
		dp_hw.intn_idx[0] = intn_idx;
		dp_hw.dbg_sel_16b[0] = MAC_AX_DP_SEL0_16B_0_15;
		dp_hw.dbg_sel_4b[0] = MAC_AX_DP_SEL_4B_0_7;
		dp_hw.dbg_sel[1] = dbg_sel;
		dp_hw.intn_idx[1] = intn_idx;
		dp_hw.dbg_sel_16b[1] = MAC_AX_DP_SEL1_16B_16_31;
		dp_hw.dbg_sel_4b[1] = MAC_AX_DP_SEL_4B_0_7;
		dp_hw.mode = MAC_AX_DP_MODE_DUMP;

		PLTFM_MSG_ERR("Internal index(%d):", intn_idx);
		ret = mac_dbgport_hw_set(adapter, &dp_hw);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("%s, %d\n", __func__, ret);
			return ret;
		}
	}

	return ret;
}

u32 dbgport_hw_dump(struct mac_ax_adapter *adapter,
		    struct mac_ax_dbgport_hw_en *dp_hw_en)
{
	u32 ret;
	u8 dbg_sel, intn_idx_max = MAC_AX_DP_INTN_IDX_NA_MAX;

	/* PCIE */
	if (dp_hw_en->pcie) {
		for (dbg_sel = MAC_AX_DP_SEL_PCIE_0;
		     dbg_sel <= MAC_AX_DP_SEL_PCIE_C; dbg_sel++) {
			switch (dbg_sel) {
			case MAC_AX_DP_SEL_PCIE_0:
				intn_idx_max = MAC_AX_DP_INTN_IDX_PCIE_0_MAX;
				break;
			case MAC_AX_DP_SEL_PCIE_1:
				intn_idx_max = MAC_AX_DP_INTN_IDX_PCIE_1_MAX;
				break;
			case MAC_AX_DP_SEL_PCIE_2:
				intn_idx_max = MAC_AX_DP_INTN_IDX_PCIE_2_MAX;
				break;
			case MAC_AX_DP_SEL_PCIE_3:
				intn_idx_max = MAC_AX_DP_INTN_IDX_PCIE_3_MAX;
				break;
			case MAC_AX_DP_SEL_PCIE_4:
			case MAC_AX_DP_SEL_PCIE_5:
			case MAC_AX_DP_SEL_PCIE_6:
				intn_idx_max = MAC_AX_DP_INTN_IDX_NA_MAX;
				break;
			case MAC_AX_DP_SEL_PCIE_7:
				intn_idx_max = MAC_AX_DP_INTN_IDX_PCIE_7_MAX;
				break;
			case MAC_AX_DP_SEL_PCIE_8:
				intn_idx_max = MAC_AX_DP_INTN_IDX_PCIE_8_MAX;
				break;
			case MAC_AX_DP_SEL_PCIE_9:
			case MAC_AX_DP_SEL_PCIE_A:
			case MAC_AX_DP_SEL_PCIE_B:
			case MAC_AX_DP_SEL_PCIE_C:
				intn_idx_max = MAC_AX_DP_INTN_IDX_NA_MAX;
				break;
			}
			PLTFM_MSG_ERR("Dbgport PCIE(0x%x):\n", dbg_sel);

			ret = dp_intn_dump(adapter, dbg_sel, intn_idx_max);
			if (ret != MACSUCCESS) {
				PLTFM_MSG_ERR("%s, %d\n", __func__, ret);
				return ret;
			}
		}
	}

	/* WLAN_MAC */
	if (dp_hw_en->axidma) {
		if (adapter->sm.fwdl != MAC_AX_FWDL_INIT_RDY) {
			ret = MACFWNONRDY;
			PLTFM_MSG_ERR("%s, %d\n", __func__, ret);
			return ret;
		}
		dbg_sel = MAC_AX_DP_SEL_AXIDMA;
		intn_idx_max = MAC_AX_DP_INTN_IDX_AXIDMA_MAX;
		PLTFM_MSG_TRACE("Dbgport axidma(0x%x):\n", dbg_sel);

		ret = dp_intn_dump(adapter, dbg_sel, intn_idx_max);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("%s, %d\n", __func__, ret);
			return ret;
		}
	}

	/* DMAC */
	if (dp_hw_en->wde_dle) {
		dbg_sel = MAC_AX_DP_SEL_WDE_DLE;
		intn_idx_max = MAC_AX_DP_INTN_IDX_WDE_DLE_MAX;
		PLTFM_MSG_TRACE("Dbgport wde dle(0x%x):\n", dbg_sel);

		ret = dp_intn_dump(adapter, dbg_sel, intn_idx_max);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("%s, %d\n", __func__, ret);
			return ret;
		}
	}

	if (dp_hw_en->ple_dle) {
		dbg_sel = MAC_AX_DP_SEL_PLE_DLE;
		intn_idx_max = MAC_AX_DP_INTN_IDX_PLE_DLE_MAX;
		PLTFM_MSG_TRACE("Dbgport ple dle(0x%x):\n", dbg_sel);

		ret = dp_intn_dump(adapter, dbg_sel, intn_idx_max);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("%s, %d\n", __func__, ret);
			return ret;
		}
	}

	if (dp_hw_en->sta_scheduler) {
		dbg_sel = MAC_AX_DP_SEL_STA_SCHEDULER;
		intn_idx_max = MAC_AX_DP_INTN_IDX_STA_SCHEDULER_MAX;
		PLTFM_MSG_TRACE("Dbgport sta scheduler(0x%x):\n", dbg_sel);

		ret = dp_intn_dump(adapter, dbg_sel, intn_idx_max);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("%s, %d\n", __func__, ret);
			return ret;
		}
	}

	return MACSUCCESS;
}

u32 mac_dbgport_hw_set(struct mac_ax_adapter *adapter,
		       struct mac_ax_dbgport_hw *dp_hw)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 ret = MACSUCCESS, backup = 0, val32;
	u8 dbg_sel, dbg_sel_16b, dbg_sel_4b, sel_idx;

	PLTFM_MUTEX_LOCK(&adapter->hw_info->ind_access_lock);
	adapter->hw_info->ind_aces_cnt++;

	if (dp_hw->mode == MAC_AX_DP_MODE_LA) {
		/* For AL mode, set GPIO PINMUX */
		val32 = 0xEEEEEEEE;
		MAC_REG_W32(R_AX_GPIO0_7_FUNC_SEL, val32);
		MAC_REG_W32(R_AX_GPIO8_15_FUNC_SEL, val32);
	} else {
		/* For dump mode, need backup R_AX_DBG_CTRL */
		backup = MAC_REG_R32(R_AX_DBG_CTRL);
	}

	val32 = 0;
	for (sel_idx = 0; sel_idx < MAC_AX_DP_SEL_NUM; sel_idx++) {
		dbg_sel = dp_hw->dbg_sel[sel_idx];
		dbg_sel_16b = dp_hw->dbg_sel_16b[sel_idx];
		dbg_sel_4b = dp_hw->dbg_sel_4b[sel_idx];

		/* dbg port select */
		if (sel_idx == 0) {
			val32 = SET_CLR_WORD(val32, dbg_sel, B_AX_DBG_SEL0);
			val32 |= (dbg_sel_16b ? B_AX_DBG_SEL0_16BIT : 0);
			val32 = SET_CLR_WORD(val32, dbg_sel_4b,
					     B_AX_DBG_SEL0_4BIT);
		} else {
			val32 = SET_CLR_WORD(val32, dbg_sel, B_AX_DBG_SEL1);
			val32 |= (dbg_sel_16b ? B_AX_DBG_SEL1_16BIT : 0);
			val32 = SET_CLR_WORD(val32, dbg_sel_4b,
					     B_AX_DBG_SEL1_4BIT);
		}

		/* internal index for each module */
		ret = dp_intn_idx_set(adapter, dp_hw, sel_idx);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("%s, %d\n", __func__, ret);
			goto DONE;
		}
	}
	MAC_REG_W32(R_AX_DBG_CTRL, val32);

	/* Enable */
	val32 = MAC_REG_R32(R_AX_SYS_STATUS1);
	val32 = SET_CLR_WORD(val32, MAC_DBG_SEL, B_AX_SEL_0XC0);
	MAC_REG_W32(R_AX_SYS_STATUS1, val32);

	/* dump info */
	dp_hw->rsp_val = MAC_REG_R32(R_AX_DBG_PORT_SEL);
	PLTFM_MSG_TRACE("0x%08X\n", dp_hw->rsp_val);

	/* For dump mode, need restore R_AX_DBG_CTRL */
	if (dp_hw->mode == MAC_AX_DP_MODE_DUMP)
		MAC_REG_W32(R_AX_DBG_CTRL, backup);

DONE:
	adapter->hw_info->ind_aces_cnt--;
	PLTFM_MUTEX_UNLOCK(&adapter->hw_info->ind_access_lock);

	return ret;
}


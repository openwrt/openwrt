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
#include "_usb.h"
#include "../mac_ax.h"
#include "mac_priv.h"

#if MAC_AX_USB_SUPPORT

u32 usb_flush_mode(struct mac_ax_adapter *adapter, u8 mode)
{
	u32 reg, val32;

#if MAC_AX_8852A_SUPPORT
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A)) {
		reg = R_AX_USB_WLAN0_1;
		if (mode == MAC_AX_FUNC_DIS) {
			val32 = PLTFM_REG_R32(reg) & ~B_AX_USBRX_RST & ~B_AX_USBTX_RST;
			PLTFM_REG_W32(reg, val32);
			return MACSUCCESS;
		} else if (mode == MAC_AX_FUNC_EN) {
			val32 = PLTFM_REG_R32(reg) | B_AX_USBRX_RST | B_AX_USBTX_RST;
			PLTFM_REG_W32(reg, val32);
			return MACSUCCESS;
		} else {
			return MACLV1STEPERR;
		}
	}
#endif
#if MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
		reg = R_AX_USB_WLAN0_1;
		if (mode == MAC_AX_FUNC_DIS) {
			val32 = PLTFM_REG_R32(reg) & ~B_AX_USBRX_RST & ~B_AX_USBTX_RST;
			PLTFM_REG_W32(reg, val32);
			return MACSUCCESS;
		} else if (mode == MAC_AX_FUNC_EN) {
			val32 = PLTFM_REG_R32(reg) | B_AX_USBRX_RST | B_AX_USBTX_RST;
			PLTFM_REG_W32(reg, val32);
			return MACSUCCESS;
		} else {
			return MACLV1STEPERR;
		}
	}
#endif
#if MAC_AX_8852C_SUPPORT
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C)) {
		reg = R_AX_USB_WLAN0_1_V1;
		if (mode == MAC_AX_FUNC_DIS) {
			val32 = PLTFM_REG_R32(reg) & ~B_AX_USBRX_RST_V1 & ~B_AX_USBTX_RST_V1;
			PLTFM_REG_W32(reg, val32);
			return MACSUCCESS;
		} else if (mode == MAC_AX_FUNC_EN) {
			val32 = PLTFM_REG_R32(reg) | B_AX_USBRX_RST_V1 | B_AX_USBTX_RST_V1;
			PLTFM_REG_W32(reg, val32);
			return MACSUCCESS;
		} else {
			return MACLV1STEPERR;
		}
	}
#endif
#if MAC_AX_8192XB_SUPPORT
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB)) {
		reg = R_AX_USB_WLAN0_1_V1;
		if (mode == MAC_AX_FUNC_DIS) {
			val32 = PLTFM_REG_R32(reg) & ~B_AX_USBRX_RST_V1 & ~B_AX_USBTX_RST_V1;
			PLTFM_REG_W32(reg, val32);
			return MACSUCCESS;
		} else if (mode == MAC_AX_FUNC_EN) {
			val32 = PLTFM_REG_R32(reg) | B_AX_USBRX_RST_V1 | B_AX_USBTX_RST_V1;
			PLTFM_REG_W32(reg, val32);
			return MACSUCCESS;
		} else {
			return MACLV1STEPERR;
		}
	}
#endif
#if MAC_AX_8851B_SUPPORT
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
		reg = R_AX_USB_WLAN0_1;
		if (mode == MAC_AX_FUNC_DIS) {
			val32 = PLTFM_REG_R32(reg) & ~B_AX_USBRX_RST & ~B_AX_USBTX_RST;
			PLTFM_REG_W32(reg, val32);
			return MACSUCCESS;
		} else if (mode == MAC_AX_FUNC_EN) {
			val32 = PLTFM_REG_R32(reg) | B_AX_USBRX_RST | B_AX_USBTX_RST;
			PLTFM_REG_W32(reg, val32);
			return MACSUCCESS;
		} else {
			return MACLV1STEPERR;
		}
	}
#endif
#if MAC_AX_8851E_SUPPORT
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8851E)) {
		reg = R_AX_USB_WLAN0_1_V1;
		if (mode == MAC_AX_FUNC_DIS) {
			val32 = PLTFM_REG_R32(reg) & ~B_AX_USBRX_RST_V1 & ~B_AX_USBTX_RST_V1;
			PLTFM_REG_W32(reg, val32);
			return MACSUCCESS;
		} else if (mode == MAC_AX_FUNC_EN) {
			val32 = PLTFM_REG_R32(reg) | B_AX_USBRX_RST_V1 | B_AX_USBTX_RST_V1;
			PLTFM_REG_W32(reg, val32);
			return MACSUCCESS;
		} else {
			return MACLV1STEPERR;
		}
	}
#endif
#if MAC_AX_8852D_SUPPORT
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
		reg = R_AX_USB_WLAN0_1_V1;
		if (mode == MAC_AX_FUNC_DIS) {
			val32 = PLTFM_REG_R32(reg) & ~B_AX_USBRX_RST_V1 & ~B_AX_USBTX_RST_V1;
			PLTFM_REG_W32(reg, val32);
			return MACSUCCESS;
		} else if (mode == MAC_AX_FUNC_EN) {
			val32 = PLTFM_REG_R32(reg) | B_AX_USBRX_RST_V1 | B_AX_USBTX_RST_V1;
			PLTFM_REG_W32(reg, val32);
			return MACSUCCESS;
		} else {
			return MACLV1STEPERR;
		}
	}
#endif
	return MACCHIPID;
}

u32 get_usb_mode(struct mac_ax_adapter *adapter)
{
	u32 reg, val32, hs;

#if MAC_AX_8852A_SUPPORT
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A)) {
		reg = R_AX_USB_STATUS;
		val32 = PLTFM_REG_R32(reg) & B_AX_R_USB2_SEL;
		hs = PLTFM_REG_R32(reg) & B_AX_MODE_HS;
		if (val32 == B_AX_R_USB2_SEL)
			val32 = MAC_AX_USB3;
		else if ((val32 != B_AX_R_USB2_SEL) && (hs == B_AX_MODE_HS))
			val32 = MAC_AX_USB2;
		else
			val32 = MAC_AX_USB11;
		return val32;
	}
#endif

#if MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
		reg = R_AX_USB_STATUS;
		val32 = PLTFM_REG_R32(reg) & B_AX_R_USB2_SEL;
		hs = PLTFM_REG_R32(reg) & B_AX_MODE_HS;
		if (val32 == B_AX_R_USB2_SEL)
			val32 = MAC_AX_USB3;
		else if ((val32 != B_AX_R_USB2_SEL) && (hs == B_AX_MODE_HS))
			val32 = MAC_AX_USB2;
		else
			val32 = MAC_AX_USB11;
		return val32;
	}
#endif

#if MAC_AX_8852C_SUPPORT
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C)) {
		reg = R_AX_USB_STATUS_V1;
		val32 = PLTFM_REG_R32(reg) & B_AX_R_USB2_SEL_V1;
		hs = PLTFM_REG_R32(reg) & B_AX_MODE_HS_V1;
		if (val32 == B_AX_R_USB2_SEL_V1)
			val32 = MAC_AX_USB3;
		else if ((val32 != B_AX_R_USB2_SEL_V1) && (hs == B_AX_MODE_HS_V1))
			val32 = MAC_AX_USB2;
		else
			val32 = MAC_AX_USB11;
		return val32;
	}
#endif

#if MAC_AX_8192XB_SUPPORT
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB)) {
		reg = R_AX_USB_STATUS_V1;
		val32 = PLTFM_REG_R32(reg) & B_AX_R_USB2_SEL_V1;
		hs = PLTFM_REG_R32(reg) & B_AX_MODE_HS_V1;
		if (val32 == B_AX_R_USB2_SEL_V1)
			val32 = MAC_AX_USB3;
		else if ((val32 != B_AX_R_USB2_SEL_V1) && (hs == B_AX_MODE_HS_V1))
			val32 = MAC_AX_USB2;
		else
			val32 = MAC_AX_USB11;
		return val32;
	}
#endif

#if MAC_AX_8851B_SUPPORT
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
		reg = R_AX_USB_STATUS;
		val32 = PLTFM_REG_R32(reg) & B_AX_R_USB2_SEL;
		hs = PLTFM_REG_R32(reg) & B_AX_MODE_HS;
		if (val32 == B_AX_R_USB2_SEL)
			val32 = MAC_AX_USB3;
		else if ((val32 != B_AX_R_USB2_SEL) && (hs == B_AX_MODE_HS))
			val32 = MAC_AX_USB2;
		else
			val32 = MAC_AX_USB11;
		return val32;
	}
#endif

#if MAC_AX_8851E_SUPPORT
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8851E)) {
		reg = R_AX_USB_STATUS_V1;
		val32 = PLTFM_REG_R32(reg) & B_AX_R_USB2_SEL_V1;
		hs = PLTFM_REG_R32(reg) & B_AX_MODE_HS_V1;
		if (val32 == B_AX_R_USB2_SEL_V1)
			val32 = MAC_AX_USB3;
		else if ((val32 != B_AX_R_USB2_SEL_V1) && (hs == B_AX_MODE_HS_V1))
			val32 = MAC_AX_USB2;
		else
			val32 = MAC_AX_USB11;
		return val32;
	}
#endif

#if MAC_AX_8852D_SUPPORT
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
		reg = R_AX_USB_STATUS_V1;
		val32 = PLTFM_REG_R32(reg) & B_AX_R_USB2_SEL_V1;
		hs = PLTFM_REG_R32(reg) & B_AX_MODE_HS_V1;
		if (val32 == B_AX_R_USB2_SEL_V1)
			val32 = MAC_AX_USB3;
		else if ((val32 != B_AX_R_USB2_SEL_V1) && (hs == B_AX_MODE_HS_V1))
			val32 = MAC_AX_USB2;
		else
			val32 = MAC_AX_USB11;
		return val32;
	}
#endif
	return MACCHIPID;
}

u32 dbcc_hci_ctrl_usb(struct mac_ax_adapter *adapter,
		      struct mac_ax_dbcc_hci_ctrl *info)
{
	enum mac_ax_func_sw en;
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);
	struct mac_ax_usb_ep cfg;
	u32 ret, cnt, val32, reg = 0;
	u8 pause, val8;

	if (!info)
		return MACNPTR;

	pause = info->pause;
	en = pause ? MAC_AX_FUNC_EN : MAC_AX_FUNC_DIS;

	if (en) {
		cfg.ep4 = DISABLE;
		cfg.ep5 = ENABLE;
		cfg.ep6 = ENABLE;
		cfg.ep7 = DISABLE;
		cfg.ep8 = DISABLE;
		cfg.ep9 = ENABLE;
		cfg.ep10 = ENABLE;
		cfg.ep11 = ENABLE;
		cfg.ep12 = ENABLE;
	} else {
		cfg.ep4 = DISABLE;
		cfg.ep5 = DISABLE;
		cfg.ep6 = DISABLE;
		cfg.ep7 = DISABLE;
		cfg.ep8 = DISABLE;
		cfg.ep9 = DISABLE;
		cfg.ep10 = DISABLE;
		cfg.ep11 = DISABLE;
		cfg.ep12 = DISABLE;
	}

	ret = p_ops->usb_ep_cfg(adapter, &cfg);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("dbcc usb_ep_cfg %d fail %d\n", en, ret);
		return ret;
	}

#if MAC_AX_8852A_SUPPORT
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A))
		reg = R_AX_USB_DEBUG_3;
#endif
#if MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B))
		reg = R_AX_USB_DEBUG_3;
#endif
#if MAC_AX_8852C_SUPPORT
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C))
		reg = R_AX_USB_DEBUG_3_V1;
#endif
#if MAC_AX_8192XB_SUPPORT
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB))
		reg = R_AX_USB_DEBUG_3_V1;
#endif
#if MAC_AX_8851B_SUPPORT
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8851B))
		reg = R_AX_USB_DEBUG_3;
#endif
#if MAC_AX_8851E_SUPPORT
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8851E))
		reg = R_AX_USB_DEBUG_3_V1;
#endif
#if MAC_AX_8852D_SUPPORT
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852D))
		reg = R_AX_USB_DEBUG_3_V1;
#endif

	cnt = 2000;
	while (--cnt) {
		val32 = PLTFM_REG_R32(reg);
		PLTFM_REG_W32(reg, val32 | BIT20);
		val8 = GET_FIELD(PLTFM_REG_R32(reg), B_AX_TX_STATE_MACHINE);
		if (val8 == USB_TX_IDLE)
			break;
		PLTFM_DELAY_US(1);
	}
	if (!cnt)
		return MACUSBPAUSEERR;
	return ret;
}

u32 ltr_set_usb(struct mac_ax_adapter *adapter,
		struct mac_ax_pcie_ltr_param *param)
{
	return MACNOTSUP;
}

u32 ctrl_txdma_ch_usb(struct mac_ax_adapter *adapter,
		      struct mac_ax_txdma_ch_map *ch_map)
{
	return MACNOTSUP;
}

u32 clr_idx_all_usb(struct mac_ax_adapter *adapter)
{
	return MACNOTSUP;
}

u32 poll_txdma_ch_idle_usb(struct mac_ax_adapter *adapter,
			   struct mac_ax_txdma_ch_map *ch_map)
{
	return MACNOTSUP;
}

u32 set_pcie_speed_usb(struct mac_ax_adapter *adapter,
		       enum mac_ax_pcie_phy speed)
{
	return MACNOTSUP;
}

u32 get_pcie_speed_usb(struct mac_ax_adapter *adapter,
		       u8 *speed)
{
	return MACNOTSUP;
}

u32 poll_rxdma_ch_idle_usb(struct mac_ax_adapter *adapter,
			   struct mac_ax_rxdma_ch_map *ch_map)
{
	return MACNOTSUP;
}

u32 ctrl_txhci_usb(struct mac_ax_adapter *adapter, enum mac_ax_func_sw en)
{
	return MACNOTSUP;
}

u32 ctrl_rxhci_usb(struct mac_ax_adapter *adapter, enum mac_ax_func_sw en)
{
	return MACNOTSUP;
}

u32 ctrl_dma_io_usb(struct mac_ax_adapter *adapter, enum mac_ax_func_sw en)
{
	return MACNOTSUP;
}

u32 get_io_stat_usb(struct mac_ax_adapter *adapter,
		    struct mac_ax_io_stat *out_st)
{
	return MACNOTSUP;
}

u32 get_avail_txbd_usb(struct mac_ax_adapter *adapter, u8 ch_idx,
		       u16 *host_idx, u16 *hw_idx, u16 *avail_txbd)
{
	return MACNOTSUP;
}

u32 get_avail_rxbd_usb(struct mac_ax_adapter *adapter, u8 ch_idx,
		       u16 *host_idx, u16 *hw_idx, u16 *avail_rxbd)
{
	return MACNOTSUP;
}

u32 trigger_txdma_usb(struct mac_ax_adapter *adapter,
		      struct tx_base_desc *txbd_ring, u8 ch_idx)
{
	return MACNOTSUP;
}

u32 notify_rxdone_usb(struct mac_ax_adapter *adapter,
		      struct rx_base_desc *rxbd, u8 ch)
{
	return MACNOTSUP;
}

u32 usb_autok_counter_avg(struct mac_ax_adapter *adapter)
{
	return MACSUCCESS;
}

u32 usb_tp_adjust(struct mac_ax_adapter *adapter, struct mac_ax_tp_param tp)
{
	return MACSUCCESS;
}

#endif /* #if MAC_AX_USB_SUPPORT */

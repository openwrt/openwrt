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

#include "phy_rpt_8852c.h"
#include "../hw.h"

#if MAC_AX_8852C_SUPPORT

#define MAC_AX_DISP_QID_HOST 0x0
#define MAC_AX_DISP_QID_WLCPU 0x8
#define MAC_AX_DISP_PID_HOST 0x0
#define MAC_AX_DISP_PID_WLCPU 0x0

u32 get_bbrpt_dle_cfg_8852c(struct mac_ax_adapter *adapter,
			    u8 is2wlcpu, u32 *port_id, u32 *queue_id)
{
	if (is2wlcpu) {
		*port_id = MAC_AX_DISP_PID_WLCPU;
		*queue_id = MAC_AX_DISP_QID_WLCPU;
	} else {
		*port_id = MAC_AX_DISP_PID_HOST;
		*queue_id = MAC_AX_DISP_QID_HOST;
	}

	return MACSUCCESS;
}

u32 mac_cfg_per_pkt_phy_rpt_8852c(struct mac_ax_adapter *adapter,
				  struct mac_ax_per_pkt_phy_rpt *rpt)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	enum mac_ax_drv_info_size *s;
	u32 drv_offset, rpt_offset, drv_info, val;

	if (rpt->band) {
		s = &adapter->hw_info->cmac1_drv_info;
		drv_offset = R_AX_RCR_C1;
		rpt_offset = R_AX_DRV_INFO_OPTION_C1;
	} else {
		s = &adapter->hw_info->cmac0_drv_info;
		drv_offset = R_AX_RCR;
		rpt_offset = R_AX_DRV_INFO_OPTION;
	}

	if (rpt->en) {
		switch (*s) {
		case MAC_AX_DRV_INFO_PHY_RPT:
		case MAC_AX_DRV_INFO_NONE:
			*s = MAC_AX_DRV_INFO_PHY_RPT;
			drv_info = MAC_AX_DRV_INFO_SIZE_16;
			break;
		case MAC_AX_DRV_INFO_PHY_RPT_BSSID:
		case MAC_AX_DRV_INFO_BSSID:
			*s = MAC_AX_DRV_INFO_PHY_RPT_BSSID;
			drv_info = MAC_AX_DRV_INFO_SIZE_80;
			break;
		case MAC_AX_DRV_INFO_PHY_RPT_MAC_HDR:
		case MAC_AX_DRV_INFO_MAC_HDR:
			*s = MAC_AX_DRV_INFO_PHY_RPT_MAC_HDR;
			drv_info = MAC_AX_DRV_INFO_SIZE_80;
			break;
		default:
			PLTFM_MSG_ERR("%s: wrong driver info size", __func__);
			return MACNOITEM;
		}
	} else {
		switch (*s) {
		case MAC_AX_DRV_INFO_NONE:
		case MAC_AX_DRV_INFO_PHY_RPT:
			*s = MAC_AX_DRV_INFO_NONE;
			drv_info = MAC_AX_DRV_INFO_SIZE_0;
			break;
		case MAC_AX_DRV_INFO_BSSID:
		case MAC_AX_DRV_INFO_PHY_RPT_BSSID:
			*s = MAC_AX_DRV_INFO_BSSID;
			drv_info = MAC_AX_DRV_INFO_SIZE_16;
			break;
		case MAC_AX_DRV_INFO_MAC_HDR:
		case MAC_AX_DRV_INFO_PHY_RPT_MAC_HDR:
			*s = MAC_AX_DRV_INFO_MAC_HDR;
			drv_info = MAC_AX_DRV_INFO_SIZE_80;
			break;
		default:
			PLTFM_MSG_ERR("%s: wrong driver info size", __func__);
			return MACNOITEM;
		}
	}

	val = MAC_REG_R32(drv_offset);
	val = SET_CLR_WORD(val, drv_info, B_AX_DRV_INFO_SIZE);
	MAC_REG_W32(drv_offset, val);

	val = MAC_REG_R32(rpt_offset);
	val = rpt->en ? (val | B_AX_DRV_INFO_PHYRPT_EN) :
				(val & ~B_AX_DRV_INFO_PHYRPT_EN);
	MAC_REG_W32(rpt_offset, val);

	return MACSUCCESS;
}

#endif /* #if MAC_AX_8852C_SUPPORT */

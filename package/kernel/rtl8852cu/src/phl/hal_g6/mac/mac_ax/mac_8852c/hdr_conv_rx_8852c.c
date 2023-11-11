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
#include "hdr_conv_rx_8852c.h"

#if MAC_AX_8852C_SUPPORT
u32 mac_hdr_conv_rx_en_8852c(struct mac_ax_adapter *adapter,
			     struct mac_ax_rx_hdr_conv_cfg *cfg)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val = 0;

	val = MAC_REG_R32(R_AX_HDR_CNV_CTRL);

	if (cfg->en) {
		if (cfg->rx_hdr_conv_type ==
		    MAC_AX_RX_HDR_TYPE_802P3_SNAP) {
			val &= ~B_AX_HDR_CNV;
		} else if (cfg->rx_hdr_conv_type ==
			   MAC_AX_RX_HDR_TYPE_ETH_II) {
			val |= B_AX_HDR_CNV;
		} else {
			PLTFM_MSG_ERR("%s: rx hdr conv type configruation\n", __func__);
			return MACNOTSUP;
		}

		if (cfg->chk_addr_cam_hit)
			val |= B_AX_HC_ADDR_HIT_EN;
		else
			val &= ~B_AX_HC_ADDR_HIT_EN;

		val |= B_AX_RX_HDR_CNV_EN;
	} else {
		val &= ~B_AX_RX_HDR_CNV_EN;
	}
	MAC_REG_W32(R_AX_HDR_CNV_CTRL, val);

	return MACSUCCESS;
}

u32 mac_hdr_conv_rx_en_driv_info_hdr_8852c(struct mac_ax_adapter *adapter,
					   struct mac_ax_rx_driv_info_hdr_cfg *cfg)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	enum mac_ax_drv_info_size *ori_cmac_0_size = &adapter->hw_info->cmac0_drv_info;
	enum mac_ax_drv_info_size *ori_cmac_1_size = &adapter->hw_info->cmac1_drv_info;
	enum mac_ax_drv_info_size drv_info_0 = *ori_cmac_0_size;
	enum mac_ax_drv_info_size drv_info_1 = *ori_cmac_1_size;
	u32 val, b1_en;

	b1_en = check_mac_en(adapter, 1, MAC_AX_CMAC_SEL);

	switch (*ori_cmac_0_size) {
	case MAC_AX_DRV_INFO_NONE:
	case MAC_AX_DRV_INFO_BSSID:
	case MAC_AX_DRV_INFO_MAC_HDR:
		if (cfg->en) {
			if (cfg->driv_info_hdr_type ==
			    MAC_AX_RX_DRIV_INFO_HDR_BSSID) {
				drv_info_0 = MAC_AX_DRV_INFO_BSSID;
			} else if (cfg->driv_info_hdr_type ==
				   MAC_AX_RX_DRIV_INFO_HDR_FULLY_HDR) {
				drv_info_0 = MAC_AX_DRV_INFO_MAC_HDR;
			} else if (cfg->driv_info_hdr_type ==
				   MAC_AX_RX_DRIV_INFO_NONE) {
				drv_info_0 = MAC_AX_DRV_INFO_NONE;
			} else {
				PLTFM_MSG_ERR("%s: wrong driver info config", __func__);
				return MACNOITEM;
			}
		} else {
			drv_info_0 = MAC_AX_DRV_INFO_NONE;
		}
		break;
	case MAC_AX_DRV_INFO_PHY_RPT:
	case MAC_AX_DRV_INFO_PHY_RPT_MAC_HDR:
	case MAC_AX_DRV_INFO_PHY_RPT_BSSID:
		if (cfg->en) {
			if (cfg->driv_info_hdr_type ==
			    MAC_AX_RX_DRIV_INFO_HDR_BSSID) {
				drv_info_0 = MAC_AX_DRV_INFO_PHY_RPT_BSSID;
			} else if (cfg->driv_info_hdr_type ==
				   MAC_AX_RX_DRIV_INFO_HDR_FULLY_HDR) {
				drv_info_0 = MAC_AX_DRV_INFO_PHY_RPT_MAC_HDR;
			} else if (cfg->driv_info_hdr_type ==
				   MAC_AX_RX_DRIV_INFO_NONE) {
				drv_info_0 = MAC_AX_DRV_INFO_PHY_RPT;
			} else {
				PLTFM_MSG_ERR("%s: wrong driver info config", __func__);
				return MACNOITEM;
			}
		} else {
			drv_info_0 = MAC_AX_DRV_INFO_PHY_RPT;
		}
		break;
	default:
		PLTFM_MSG_ERR("%s: wrong driver info size", __func__);
		return MACNOITEM;
	}

	if (b1_en == MACSUCCESS) {
		switch (*ori_cmac_1_size) {
		case MAC_AX_DRV_INFO_NONE:
		case MAC_AX_DRV_INFO_BSSID:
		case MAC_AX_DRV_INFO_MAC_HDR:
			if (cfg->en) {
				if (cfg->driv_info_hdr_type ==
				    MAC_AX_RX_DRIV_INFO_HDR_BSSID) {
					drv_info_1 = MAC_AX_DRV_INFO_BSSID;
				} else if (cfg->driv_info_hdr_type ==
					   MAC_AX_RX_DRIV_INFO_HDR_FULLY_HDR) {
					drv_info_1 = MAC_AX_DRV_INFO_MAC_HDR;
				} else if (cfg->driv_info_hdr_type ==
					   MAC_AX_RX_DRIV_INFO_NONE) {
					drv_info_1 = MAC_AX_DRV_INFO_NONE;
				} else {
					PLTFM_MSG_ERR("%s: wrong driver info config", __func__);
					return MACNOITEM;
				}
			} else {
				drv_info_1 = MAC_AX_DRV_INFO_NONE;
			}
			break;
		case MAC_AX_DRV_INFO_PHY_RPT:
		case MAC_AX_DRV_INFO_PHY_RPT_MAC_HDR:
		case MAC_AX_DRV_INFO_PHY_RPT_BSSID:
			if (cfg->en) {
				if (cfg->driv_info_hdr_type ==
				    MAC_AX_RX_DRIV_INFO_HDR_BSSID) {
					drv_info_1 = MAC_AX_DRV_INFO_PHY_RPT_BSSID;
				} else if (cfg->driv_info_hdr_type ==
					   MAC_AX_RX_DRIV_INFO_HDR_FULLY_HDR) {
					drv_info_1 = MAC_AX_DRV_INFO_PHY_RPT_MAC_HDR;
				} else if (cfg->driv_info_hdr_type ==
					   MAC_AX_RX_DRIV_INFO_NONE) {
					drv_info_1 = MAC_AX_DRV_INFO_PHY_RPT;
				} else {
					PLTFM_MSG_ERR("%s: wrong driver info config", __func__);
					return MACNOITEM;
				}
			} else {
				drv_info_1 = MAC_AX_DRV_INFO_PHY_RPT;
			}
			break;
		default:
			PLTFM_MSG_ERR("%s: wrong driver info size", __func__);
			return MACNOITEM;
		}
	}

	val = MAC_REG_R32(R_AX_HDR_CNV_CTRL);
	if (cfg->en)
		val = SET_CLR_WORD(val, (u32)cfg->driv_info_hdr_type, B_AX_HDR_INFO);
	else
		val = SET_CLR_WORD(val, (u32)MAC_AX_RX_DRIV_INFO_NONE, B_AX_HDR_INFO);
	MAC_REG_W32(R_AX_HDR_CNV_CTRL, val);

	val = MAC_REG_R32(R_AX_RCR);
	val = SET_CLR_WORD(val,  (u32)drv_info_0, B_AX_DRV_INFO_SIZE);
	MAC_REG_W32(R_AX_RCR, val);
	*ori_cmac_0_size = drv_info_0;

	if (b1_en == MACSUCCESS) {
		val = MAC_REG_R32(R_AX_RCR_C1);
		val = SET_CLR_WORD(val, (u32)drv_info_1, B_AX_DRV_INFO_SIZE);
		MAC_REG_W32(R_AX_RCR_C1, val);
		*ori_cmac_1_size = drv_info_1;
	}

	return MACSUCCESS;
}
#endif

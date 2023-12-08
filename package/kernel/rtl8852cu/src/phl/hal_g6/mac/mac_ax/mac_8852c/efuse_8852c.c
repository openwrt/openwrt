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
#include "efuse_8852c.h"

#if MAC_AX_8852C_SUPPORT

u32 switch_efuse_bank_8852c(struct mac_ax_adapter *adapter,
			    enum mac_ax_efuse_bank bank)
{
	struct mac_ax_efuse_param *efuse_param = &adapter->efuse_param;
	struct mac_ax_hw_info *hw_info = adapter->hw_info;

	switch (bank) {
	case MAC_AX_EFUSE_BANK_WIFI:
		if (dv_sel == DAV) {
			bank_efuse_info.phy_map = &efuse_param->dav_efuse_map;
			bank_efuse_info.log_map =
				&efuse_param->dav_log_efuse_map;
			bank_efuse_info.phy_map_valid =
				&efuse_param->dav_efuse_map_valid;
			bank_efuse_info.log_map_valid =
				&efuse_param->dav_log_efuse_map_valid;
			bank_efuse_info.efuse_end =
				&efuse_param->dav_efuse_end;
			bank_efuse_info.phy_map_size =
				&hw_info->dav_efuse_size;
			bank_efuse_info.log_map_size =
				&hw_info->dav_log_efuse_size;
			bank_efuse_info.efuse_start =
				&hw_info->dav_efuse_start_addr;
			break;
		}

		bank_efuse_info.phy_map = &efuse_param->efuse_map;
		bank_efuse_info.log_map = &efuse_param->log_efuse_map;
		bank_efuse_info.phy_map_valid = &efuse_param->efuse_map_valid;
		bank_efuse_info.log_map_valid =
			&efuse_param->log_efuse_map_valid;
		bank_efuse_info.efuse_end = &efuse_param->efuse_end;
		bank_efuse_info.phy_map_size = &hw_info->efuse_size;
		bank_efuse_info.log_map_size = &hw_info->log_efuse_size;
		bank_efuse_info.efuse_start = &hw_info->wl_efuse_start_addr;
		break;
	case MAC_AX_EFUSE_BANK_BT:
		bank_efuse_info.phy_map = &efuse_param->bt_efuse_map;
		bank_efuse_info.log_map = &efuse_param->bt_log_efuse_map;
		bank_efuse_info.phy_map_valid =
			&efuse_param->bt_efuse_map_valid;
		bank_efuse_info.log_map_valid =
			&efuse_param->bt_log_efuse_map_valid;
		bank_efuse_info.efuse_end = &efuse_param->bt_efuse_end;
		bank_efuse_info.phy_map_size = &hw_info->bt_efuse_size;
		bank_efuse_info.log_map_size = &hw_info->bt_log_efuse_size;
		bank_efuse_info.efuse_start = &hw_info->bt_efuse_start_addr;
		break;
	default:
		return MACEFUSEBANK;
	}

	return MACSUCCESS;
}

void enable_efuse_sw_pwr_cut_8852c(struct mac_ax_adapter *adapter,
				   bool is_write)
{
	u16 val16;
	u8 val8;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	if (dv_sel == DDV) {
		if (is_write)
			MAC_REG_W8(R_AX_PMC_DBG_CTRL2 + 3, UNLOCK_CODE);

		val8 = MAC_REG_R8(R_AX_PMC_DBG_CTRL2);
		MAC_REG_W8(R_AX_PMC_DBG_CTRL2, val8 | B_AX_SYSON_DIS_PMCR_AX_WRMSK);

		val16 = MAC_REG_R16(R_AX_SYS_ISO_CTRL);
		MAC_REG_W16(R_AX_SYS_ISO_CTRL, val16 | BIT(B_AX_PWC_EV2EF_SH));

		PLTFM_DELAY_US(1000);

		val16 = MAC_REG_R16(R_AX_SYS_ISO_CTRL);
		MAC_REG_W16(R_AX_SYS_ISO_CTRL,
			    val16 | BIT(B_AX_PWC_EV2EF_SH + 1));

		val16 = MAC_REG_R16(R_AX_SYS_ISO_CTRL);
		MAC_REG_W16(R_AX_SYS_ISO_CTRL, val16 & ~(B_AX_ISO_EB2CORE));
	} else {
		enable_efuse_pwr_cut_dav(adapter, is_write);
	}
}

void disable_efuse_sw_pwr_cut_8852c(struct mac_ax_adapter *adapter,
				    bool is_write)
{
	u16 value16;
	u8 value8;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	if (dv_sel == DDV) {
		value16 = MAC_REG_R16(R_AX_SYS_ISO_CTRL);
		MAC_REG_W16(R_AX_SYS_ISO_CTRL, value16 | B_AX_ISO_EB2CORE);

		value16 = MAC_REG_R16(R_AX_SYS_ISO_CTRL);
		MAC_REG_W16(R_AX_SYS_ISO_CTRL,
			    value16 & ~(BIT(B_AX_PWC_EV2EF_SH + 1)));

		PLTFM_DELAY_US(1000);

		value16 = MAC_REG_R16(R_AX_SYS_ISO_CTRL);
		MAC_REG_W16(R_AX_SYS_ISO_CTRL,
			    value16 & ~(BIT(B_AX_PWC_EV2EF_SH)));

		if (is_write)
			MAC_REG_W8(R_AX_PMC_DBG_CTRL2 + 3, 0x00);

		value8 = MAC_REG_R8(R_AX_PMC_DBG_CTRL2);
		MAC_REG_W8(R_AX_PMC_DBG_CTRL2, value8 & ~B_AX_SYSON_DIS_PMCR_AX_WRMSK);
	} else {
		disable_efuse_pwr_cut_dav(adapter, is_write);
	}
}

#endif /* MAC_AX_8852C_SUPPORT */

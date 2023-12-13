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
#include "../coex.h"
#include "../../mac_reg.h"

#if MAC_AX_8852C_SUPPORT

#define MAC_AX_RTK_RATE 5

#define MAC_AX_RTK_MODE 0
#define MAC_AX_CSR_MODE 1

u32 mac_write_lte_8852c(struct mac_ax_adapter *adapter,
			const u32 offset, u32 val)
{
	return MACNOTSUP;
}

u32 mac_read_lte_8852c(struct mac_ax_adapter *adapter,
		       const u32 offset, u32 *val)
{
	return MACNOTSUP;
}

u32 coex_mac_init_8852c(struct mac_ax_adapter *adapter)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	MAC_REG_W32(R_AX_GNT_SW_CTRL, 0);

	return MACSUCCESS;
}

u32 mac_coex_init_8852c(struct mac_ax_adapter *adapter,
			struct mac_ax_coex *coex)
{
	u32 val;
	u16 val16;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	val = MAC_REG_R32(R_AX_BTC_CFG);
	MAC_REG_W32(R_AX_BTC_CFG, val |
		    B_AX_BTC_EN | B_AX_BTG_LNA1_GAIN_SEL);

	val = MAC_REG_R32(R_AX_BT_CNT_CFG);
	MAC_REG_W32(R_AX_BT_CNT_CFG, val | B_AX_BT_CNT_EN);

	val16 = MAC_REG_R16(R_AX_CCA_CFG_0);
	val16 = (val16 | B_AX_BTCCA_EN) & ~B_AX_BTCCA_BRK_TXOP_EN;
	MAC_REG_W16(R_AX_CCA_CFG_0, val16);

	switch (coex->pta_mode) {
	case MAC_AX_COEX_RTK_MODE:
		val = MAC_REG_R32(R_AX_BTC_CFG);
		val = SET_CLR_WORD(val, MAC_AX_RTK_MODE,
				   B_AX_BTC_MODE);
		MAC_REG_W32(R_AX_BTC_CFG, val);

		val = MAC_REG_R32(R_AX_RTK_MODE_CFG_V1);
		val = SET_CLR_WORD(val, MAC_AX_RTK_RATE,
				   B_AX_SAMPLE_CLK);
		MAC_REG_W32(R_AX_RTK_MODE_CFG_V1, val);
		break;
	case MAC_AX_COEX_CSR_MODE:
		val = MAC_REG_R32(R_AX_BTC_CFG);
		val = SET_CLR_WORD(val, MAC_AX_CSR_MODE,
				   B_AX_BTC_MODE);
		MAC_REG_W32(R_AX_BTC_CFG, val);
		break;
	default:
		return MACNOITEM;
	}

	return MACSUCCESS;
}

u32 mac_get_gnt_8852c(struct mac_ax_adapter *adapter,
		      struct mac_ax_coex_gnt *gnt_cfg)
{
	u32 val, status;
	struct mac_ax_gnt *gnt;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	val = MAC_REG_R32(R_AX_GNT_SW_CTRL);
	status = MAC_REG_R32(R_AX_GNT_VAL);

	gnt = &gnt_cfg->band0;
	gnt->gnt_bt_sw_en = !!(val & B_AX_GNT_BT_RFC_S0_SWCTRL);
	gnt->gnt_bt = !!(status & B_AX_GNT_BT_RFC_S0);
	gnt->gnt_wl_sw_en = !!(val & B_AX_GNT_WL_RFC_S0_SWCTRL);
	gnt->gnt_wl = !!(status & B_AX_GNT_WL_RFC_S0);

	gnt = &gnt_cfg->band1;
	gnt->gnt_bt_sw_en = !!(val & B_AX_GNT_BT_RFC_S1_SWCTRL);
	gnt->gnt_bt = !!(status & B_AX_GNT_BT_RFC_S1);
	gnt->gnt_wl_sw_en = !!(val & B_AX_GNT_WL_RFC_S1_SWCTRL);
	gnt->gnt_wl = !!(status & B_AX_GNT_WL_RFC_S1);

	return MACSUCCESS;
}

u32 mac_cfg_gnt_8852c(struct mac_ax_adapter *adapter,
		      struct mac_ax_coex_gnt *gnt_cfg)
{
	u32 val;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	val = (gnt_cfg->band0.gnt_bt ?
	       (B_AX_GNT_BT_RFC_S0_VAL | B_AX_GNT_BT_RX_VAL |
		B_AX_GNT_BT_TX_VAL) : 0) |
	      (gnt_cfg->band0.gnt_bt_sw_en ?
	       (B_AX_GNT_BT_RFC_S0_SWCTRL | B_AX_GNT_BT_RX_SWCTRL |
		B_AX_GNT_BT_TX_SWCTRL) : 0) |
	      (gnt_cfg->band0.gnt_wl ?
	       (B_AX_GNT_WL_RFC_S0_VAL | B_AX_GNT_WL_RX_VAL |
		B_AX_GNT_WL_TX_VAL | B_AX_GNT_WL_BB_VAL) : 0) |
	      (gnt_cfg->band0.gnt_wl_sw_en ?
	       (B_AX_GNT_WL_RFC_S0_SWCTRL | B_AX_GNT_WL_RX_SWCTRL |
		B_AX_GNT_WL_TX_SWCTRL | B_AX_GNT_WL_BB_SWCTRL) : 0) |
	      (gnt_cfg->band1.gnt_bt ?
	       (B_AX_GNT_BT_RFC_S1_VAL | B_AX_GNT_BT_RX_VAL |
		B_AX_GNT_BT_TX_VAL) : 0) |
	      (gnt_cfg->band1.gnt_bt_sw_en ?
	       (B_AX_GNT_BT_RFC_S1_SWCTRL | B_AX_GNT_BT_RX_SWCTRL |
		B_AX_GNT_BT_TX_SWCTRL) : 0) |
	      (gnt_cfg->band1.gnt_wl ?
	       (B_AX_GNT_WL_RFC_S1_VAL | B_AX_GNT_WL_RX_VAL |
		B_AX_GNT_WL_TX_VAL | B_AX_GNT_WL_BB_VAL) : 0) |
	      (gnt_cfg->band1.gnt_wl_sw_en ?
	       (B_AX_GNT_WL_RFC_S1_SWCTRL | B_AX_GNT_WL_RX_SWCTRL |
		B_AX_GNT_WL_TX_SWCTRL | B_AX_GNT_WL_BB_SWCTRL) : 0);

	MAC_REG_W32(R_AX_GNT_SW_CTRL, val);

	return MACSUCCESS;
}

u32 mac_read_coex_reg_8852c(struct mac_ax_adapter *adapter,
			    const u32 offset, u32 *val)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	if (offset > 0x1FF) {
		PLTFM_MSG_ERR("[ERR]offset exceed coex reg\n");
		return MACBADDR;
	}

	*val = MAC_REG_R32(R_AX_BTC_CFG + offset);

	return MACSUCCESS;
}

u32 mac_write_coex_reg_8852c(struct mac_ax_adapter *adapter,
			     const u32 offset, const u32 val)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	if (offset > 0x1FF) {
		PLTFM_MSG_ERR("[ERR]offset exceed coex reg\n");
		return MACBADDR;
	}

	MAC_REG_W32(R_AX_BTC_CFG + offset, val);

	return MACSUCCESS;
}

u32 mac_cfg_ctrl_path_8852c(struct mac_ax_adapter *adapter, u32 wl)
{
	struct mac_ax_coex_gnt gnt_cfg;

	if (wl)
		return MACSUCCESS;

	gnt_cfg.band0.gnt_bt_sw_en = 1;
	gnt_cfg.band0.gnt_bt = 1;
	gnt_cfg.band0.gnt_wl_sw_en = 1;
	gnt_cfg.band0.gnt_wl = 0;

	gnt_cfg.band1.gnt_bt_sw_en = 1;
	gnt_cfg.band1.gnt_bt = 1;
	gnt_cfg.band1.gnt_wl_sw_en = 1;
	gnt_cfg.band1.gnt_wl = 0;

	return mac_cfg_gnt_8852c(adapter, &gnt_cfg);
}

u32 mac_get_ctrl_path_8852c(struct mac_ax_adapter *adapter, u32 *wl)
{
	*wl = 0;

	return MACSUCCESS;
}

#endif /* MAC_AX_8852C_SUPPORT */

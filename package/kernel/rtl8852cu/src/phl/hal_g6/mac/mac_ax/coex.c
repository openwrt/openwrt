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
#include "coex.h"
#include "../mac_reg.h"
#include "hw.h"
#include "power_saving.h"

#define MAC_AX_RTK_RATE 5

#define MAC_AX_BT_MODE_0_3 0
#define MAC_AX_BT_MODE_2 2

#define MAC_AX_CSR_DELAY 0
#define MAC_AX_CSR_PRI_TO 5
#define MAC_AX_CSR_TRX_TO 4

#define MAC_AX_CSR_RATE 80

#define MAC_AX_SB_DRV_MSK 0xFFFFFF
#define MAC_AX_SB_DRV_SH 0
#define MAC_AX_SB_FW_MSK 0x7F
#define MAC_AX_SB_FW_SH 24

#define MAC_AX_BTGS1_NOTIFY BIT(0)

u32 mac_cfg_plt(struct mac_ax_adapter *adapter, struct mac_ax_plt *plt)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 reg, ret;
	u16 val;

	ret = check_mac_en(adapter, plt->band, MAC_AX_CMAC_SEL);
	if (ret)
		return ret;

	reg = plt->band == 0 ? R_AX_BT_PLT : R_AX_BT_PLT_C1;
	val = (plt->tx & MAC_AX_PLT_LTE_RX ? B_AX_TX_PLT_GNT_LTE_RX : 0) |
		(plt->tx & MAC_AX_PLT_GNT_BT_TX ? B_AX_TX_PLT_GNT_BT_TX : 0) |
		(plt->tx & MAC_AX_PLT_GNT_BT_RX ? B_AX_TX_PLT_GNT_BT_RX : 0) |
		(plt->tx & MAC_AX_PLT_GNT_WL ? B_AX_TX_PLT_GNT_WL : 0) |
		(plt->rx & MAC_AX_PLT_LTE_RX ? B_AX_RX_PLT_GNT_LTE_RX : 0) |
		(plt->rx & MAC_AX_PLT_GNT_BT_TX ? B_AX_RX_PLT_GNT_BT_TX : 0) |
		(plt->rx & MAC_AX_PLT_GNT_BT_RX ? B_AX_RX_PLT_GNT_BT_RX : 0) |
		(plt->rx & MAC_AX_PLT_GNT_WL ? B_AX_RX_PLT_GNT_WL : 0) |
		B_AX_PLT_EN;

#if MAC_AX_FW_REG_OFLD
	if (adapter->sm.fwdl == MAC_AX_FWDL_INIT_RDY) {
		ret = MAC_REG_W_OFLD((u16)reg,
				     B_AX_TX_PLT_GNT_LTE_RX |
				     B_AX_TX_PLT_GNT_BT_TX |
				     B_AX_TX_PLT_GNT_BT_RX |
				     B_AX_TX_PLT_GNT_WL |
				     B_AX_RX_PLT_GNT_LTE_RX |
				     B_AX_RX_PLT_GNT_BT_TX |
				     B_AX_RX_PLT_GNT_BT_RX |
				     B_AX_RX_PLT_GNT_WL,
				     val, 1);
		if (ret != MACSUCCESS)
			PLTFM_MSG_ERR("%s: write offload fail %d",
				      __func__, ret);

		return ret;
	}
#endif
	MAC_REG_W16(reg, val);

	return MACSUCCESS;
}

void mac_cfg_sb(struct mac_ax_adapter *adapter, u32 val)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 fw_sb;

	fw_sb = MAC_REG_R32(R_AX_SCOREBOARD);
	fw_sb = GET_FIELD(fw_sb, MAC_AX_SB_FW);
	fw_sb = fw_sb & ~MAC_AX_BTGS1_NOTIFY;
	if (adapter->sm.pwr == MAC_AX_PWR_OFF || _is_in_lps(adapter))
		fw_sb = fw_sb | MAC_AX_NOTIFY_PWR_MAJOR;
	else
		fw_sb = fw_sb | MAC_AX_NOTIFY_TP_MAJOR;
	val = GET_FIELD(val, MAC_AX_SB_DRV);
	val = B_AX_TOGGLE |
		SET_WORD(val, MAC_AX_SB_DRV) |
		SET_WORD(fw_sb, MAC_AX_SB_FW);
	MAC_REG_W32(R_AX_SCOREBOARD, val);
}

u32 mac_get_bt_polt_cnt(struct mac_ax_adapter *adapter,
			struct mac_ax_bt_polt_cnt *cnt)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 offset = cnt->band ? R_AX_BT_PLT_C1 : R_AX_BT_PLT;
	u8 val;

	cnt->cnt = MAC_REG_R16(offset + 2);

	val = MAC_REG_R8(offset + 1);
	MAC_REG_W8(offset + 1, val | BIT(1));

	return MACSUCCESS;
}

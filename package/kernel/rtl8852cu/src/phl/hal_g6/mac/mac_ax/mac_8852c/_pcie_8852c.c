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
#include "_pcie_8852c.h"

#if MAC_AX_8852C_SUPPORT

#if MAC_AX_PCIE_SUPPORT

static struct mac_ax_intf_info intf_info_def_8852c = {
	MAC_AX_BD_NORM,
	MAC_AX_BD_NORM,
	MAC_AX_RXBD_SEP,
	MAC_AX_TAG_MULTI,
	MAC_AX_TX_BURST_V1_256B,
	MAC_AX_RX_BURST_V1_128B,
	MAC_AX_WD_DMA_INTVL_256NS,
	MAC_AX_WD_DMA_INTVL_256NS,
	MAC_AX_TAG_NUM_8,
	0,
	NULL,
	NULL,
	0,
	NULL,
	MAC_AX_PCIE_ENABLE,
	MAC_AX_LBC_TMR_2MS,
	MAC_AX_PCIE_DISABLE,
	MAC_AX_PCIE_ENABLE,
	MAC_AX_IO_RCY_ANA_TMR_6MS
};

static struct mac_ax_pcie_ltr_param pcie_ltr_param_def_8852c = {
	0,
	0,
	MAC_AX_PCIE_ENABLE,
	MAC_AX_PCIE_ENABLE,
	MAC_AX_PCIE_LTR_SPC_500US,
	MAC_AX_PCIE_LTR_IDLE_TIMER_3_2MS,
	{MAC_AX_PCIE_ENABLE, 0x28},
	{MAC_AX_PCIE_ENABLE, 0x28},
	{MAC_AX_PCIE_ENABLE, 0x90039003},
	{MAC_AX_PCIE_ENABLE, 0x880b880b}
};

static u32 ltr_ltcy_regl_8852c[PCIE_LTR_IDX_MAX] = {
	R_AX_LTR_LATENCY_IDX0, R_AX_LTR_LATENCY_IDX1,
	R_AX_LTR_LATENCY_IDX2, R_AX_LTR_LATENCY_IDX3
};

static struct mac_ax_pcie_cfgspc_param pcie_cfgspc_param_def_8852c = {
	0,
	0,
	MAC_AX_PCIE_DISABLE,
	MAC_AX_PCIE_ENABLE,
	MAC_AX_PCIE_ENABLE,
	MAC_AX_PCIE_ENABLE,
	MAC_AX_PCIE_ENABLE,
	MAC_AX_PCIE_CLKDLY_V1_16US,
	MAC_AX_PCIE_L0SDLY_4US,
	MAC_AX_PCIE_L1DLY_16US
};

struct txbd_ram mac_bdram_tbl_8852c[] = {
		/* ACH0_QUEUE_IDX_8852CE */ {0, 5, 2},
		/* ACH1_QUEUE_IDX_8852CE */ {5, 5, 2},
		/* ACH2_QUEUE_IDX_8852CE */ {10, 5, 2},
		/* ACH3_QUEUE_IDX_8852CE */ {15, 5, 2},
		/* ACH4_QUEUE_IDX_8852CE */ {20, 5, 2},
		/* ACH5_QUEUE_IDX_8852CE */ {25, 5, 2},
		/* ACH6_QUEUE_IDX_8852CE */ {30, 5, 2},
		/* ACH7_QUEUE_IDX_8852CE */ {35, 5, 2},
		/* MGQ_B0_QUEUE_IDX_8852CE */ {40, 5, 1},
		/* HIQ_B0_QUEUE_IDX_8852CE */ {45, 5, 1},
		/* MGQ_B1_QUEUE_IDX_8852CE */ {50, 5, 1},
		/* HIQ_B1_QUEUE_IDX_8852CE */ {55, 5, 1},
		/* FWCMD_QUEUE_IDX_8852CE */ {60, 4, 1}
};

static u32 _patch_l11_exit(struct mac_ax_adapter *adapter)
{
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);
	u32 stus_val, ret = MACSUCCESS;

	if (chk_patch_l11_exit(adapter) == PATCH_DISABLE)
		return MACSUCCESS;

	ret = p_ops->mio_r32_pcie(adapter, PCIE_L1SS_STS_V1, &stus_val);
	if (ret != MACSUCCESS)
		return ret;

	stus_val &= (~PCIE_BIT_ASPM_L11 & ~PCIE_BIT_PCI_L11);

	ret = p_ops->mio_w32_pcie(adapter, PCIE_L1SS_STS_V1, stus_val);

	return ret;
}

static u32 _patch_fix_emac_delay(struct mac_ax_adapter *adapter, u32 *val)
{
	if (chk_patch_fix_emac_delay(adapter))
		return MACSUCCESS;

	*val |= (B_AX_CLK_REQ_SEL_OPT | B_AX_CLK_REQ_SEL);

	return MACSUCCESS;
}

static u32 chk_stus_l1ss(struct mac_ax_adapter *adapter, u8 *val)
{
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);
	u32 stus_val;
	u32 sup_val;
	u32 cap_val;
	u32 ret = MACSUCCESS;

	ret = p_ops->mio_r32_pcie(adapter, PCIE_L1SS_CAP_V1, &cap_val);
	if (ret != MACSUCCESS)
		return ret;

	ret = p_ops->mio_r32_pcie(adapter, PCIE_L1SS_SUP_V1, &sup_val);
	if (ret != MACSUCCESS)
		return ret;

	ret = p_ops->mio_r32_pcie(adapter, PCIE_L1SS_STS_V1, &stus_val);
	if (ret != MACSUCCESS)
		return ret;

	if ((cap_val & PCIE_L1SS_ID_MASK) == PCIE_L1SS_ID &&
	    (sup_val & PCIE_BIT_L1SSSUP) &&
	    (sup_val & PCIE_L1SS_MASK) != 0 &&
	    (stus_val & PCIE_L1SS_MASK) != 0)
		*val = 1;
	else
		*val = 0;

	return MACSUCCESS;
}

static u32 update_clkdly(struct mac_ax_adapter *adapter, u32 *val,
			 enum mac_ax_pcie_clkdly ctrl,
			 enum mac_ax_pcie_clkdly def_ctrl)
{
	u8 tmp;

	if (ctrl == MAC_AX_PCIE_CLKDLY_IGNORE ||
	    def_ctrl == MAC_AX_PCIE_CLKDLY_IGNORE)
		return MACSUCCESS;

	tmp = (u8)((ctrl == MAC_AX_PCIE_CLKDLY_DEF) ? def_ctrl : ctrl);
	switch (tmp) {
	case MAC_AX_PCIE_CLKDLY_V1_0:
		*val = SET_CLR_WOR2(*val, PCIE_CLKDLY_HW_V1_0,
				    B_AX_CLK_REQ_LAT_SH, B_AX_CLK_REQ_LAT_MSK);
		break;

	case MAC_AX_PCIE_CLKDLY_V1_16US:
		*val = SET_CLR_WOR2(*val, PCIE_CLKDLY_HW_V1_16US,
				    B_AX_CLK_REQ_LAT_SH, B_AX_CLK_REQ_LAT_MSK);
		break;

	case MAC_AX_PCIE_CLKDLY_V1_32US:
		*val = SET_CLR_WOR2(*val, PCIE_CLKDLY_HW_V1_32US,
				    B_AX_CLK_REQ_LAT_SH, B_AX_CLK_REQ_LAT_MSK);
		break;

	case MAC_AX_PCIE_CLKDLY_V1_64US:
		*val = SET_CLR_WOR2(*val, PCIE_CLKDLY_HW_V1_64US,
				    B_AX_CLK_REQ_LAT_SH, B_AX_CLK_REQ_LAT_MSK);
		break;

	case MAC_AX_PCIE_CLKDLY_V1_80US:
		*val = SET_CLR_WOR2(*val, PCIE_CLKDLY_HW_V1_80US,
				    B_AX_CLK_REQ_LAT_SH, B_AX_CLK_REQ_LAT_MSK);
		break;

	case MAC_AX_PCIE_CLKDLY_V1_96US:
		*val = SET_CLR_WOR2(*val, PCIE_CLKDLY_HW_V1_96US,
				    B_AX_CLK_REQ_LAT_SH, B_AX_CLK_REQ_LAT_MSK);
		break;

	default:
		PLTFM_MSG_ERR("[ERR]CLKDLY wt val illegal!\n");
		return MACSTCAL;
	}
	return MACSUCCESS;
}

static u32 update_aspmdly(struct mac_ax_adapter *adapter, u32 *val,
			  struct mac_ax_pcie_cfgspc_param *param,
			  struct mac_ax_pcie_cfgspc_param *param_def)
{
	u32 l1mask = PCIE_ASPMDLY_MASK << SHFT_L1DLY_V1;
	u32 l0smask = PCIE_ASPMDLY_MASK << SHFT_L0SDLY_V1;
	u8 l1updval = param->l1dly_ctrl;
	u8 l0supdval = param->l0sdly_ctrl;
	u8 l1defval = param_def->l1dly_ctrl;
	u8 l0sdefval = param_def->l0sdly_ctrl;
	u32 tmp;
	u32 hwval;

	if (l1updval != MAC_AX_PCIE_L1DLY_IGNORE) {
		tmp = (l1updval == MAC_AX_PCIE_L1DLY_DEF) ? l1defval : l1updval;
		switch (tmp) {
		case MAC_AX_PCIE_L1DLY_16US:
			hwval = PCIE_L1DLY_HW_16US;
			break;

		case MAC_AX_PCIE_L1DLY_32US:
			hwval = PCIE_L1DLY_HW_32US;
			break;

		case MAC_AX_PCIE_L1DLY_64US:
			hwval = PCIE_L1DLY_HW_64US;
			break;

		case MAC_AX_PCIE_L1DLY_INFI:
			hwval = PCIE_L1DLY_HW_INFI;
			break;

		default:
			PLTFM_MSG_ERR("[ERR]L1DLY wt val illegal!\n");
			return MACSTCAL;
		}

		tmp = (hwval << SHFT_L1DLY_V1) & l1mask;
		*val = (*val & ~(l1mask)) | tmp;
	}

	if (l0supdval != MAC_AX_PCIE_L0SDLY_IGNORE) {
		tmp = (l0supdval == MAC_AX_PCIE_L0SDLY_DEF) ?
		       l0sdefval : l0supdval;
		switch (tmp) {
		case MAC_AX_PCIE_L0SDLY_1US:
			hwval = PCIE_L0SDLY_HW_1US;
			break;

		case MAC_AX_PCIE_L0SDLY_2US:
			hwval = PCIE_L0SDLY_HW_2US;
			break;

		case MAC_AX_PCIE_L0SDLY_3US:
			hwval = PCIE_L0SDLY_HW_3US;
			break;

		case MAC_AX_PCIE_L0SDLY_4US:
			hwval = PCIE_L0SDLY_HW_4US;
			break;

		case MAC_AX_PCIE_L0SDLY_5US:
			hwval = PCIE_L0SDLY_HW_5US;
			break;

		case MAC_AX_PCIE_L0SDLY_6US:
			hwval = PCIE_L0SDLY_HW_6US;
			break;

		case MAC_AX_PCIE_L0SDLY_7US:
			hwval = PCIE_L0SDLY_HW_7US;
			break;

		default:
			PLTFM_MSG_ERR("[ERR]L0SDLY wt val illegal!\n");
			return MACSTCAL;
		}
		tmp = (hwval << SHFT_L0SDLY_V1) & l0smask;
		*val = (*val & ~(l0smask)) | tmp;
	}

	return MACSUCCESS;
}

struct mac_ax_intf_info *
get_pcie_info_def_8852c(struct mac_ax_adapter *adapter)
{
	return &intf_info_def_8852c;
}

struct txbd_ram *
get_bdram_tbl_pcie_8852c(struct mac_ax_adapter *adapter)
{
	return mac_bdram_tbl_8852c;
}

u32 mio_w32_pcie_8852c(struct mac_ax_adapter *adapter, u16 addr, u32 value)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u8 addr_lsb;
	u8 page_sel;
	u8 bytemap = MIO_WRITE_BYTE_ALL;
	u16 count;
	u32 value32;

	page_sel = GET_FIEL2(addr, MIO_ADDR_PAGE_SH,
			     B_AX_PCIE_MIO_ADDR_PAGE_V1_MSK);
	addr_lsb = GET_FIEL2(addr, B_AX_PCIE_MIO_ADDR_SH,
			     B_AX_PCIE_MIO_ADDR_MSK);

	if ((addr_lsb % MIO_4BYTE_ALIGN) != 0) {
		PLTFM_MSG_ERR("[ERR]MIO w32 addr: %X\n", addr_lsb);
		return MACFUNCINPUT;
	}

	MAC_REG_W32(R_AX_SYS_SDIO_CTRL, MAC_REG_R32(R_AX_SYS_SDIO_CTRL) |
		    B_AX_PCIE_AUXCLK_GATE);

	value32 = 0;
	value32 = SET_CLR_WOR2(value32, addr_lsb, B_AX_PCIE_MIO_ADDR_SH,
			       B_AX_PCIE_MIO_ADDR_MSK);
	value32 = SET_CLR_WOR2(value32, page_sel, B_AX_PCIE_MIO_ADDR_PAGE_V1_SH,
			       B_AX_PCIE_MIO_ADDR_PAGE_V1_MSK);
	value32 = SET_CLR_WOR2(value32, bytemap, B_AX_PCIE_MIO_WE_SH,
			       B_AX_PCIE_MIO_WE_MSK);

	MAC_REG_W32(R_AX_PCIE_MIO_INTF, value32);
	MAC_REG_W32(R_AX_PCIE_MIO_INTD, value);
	value32 |= B_AX_PCIE_MIO_BYIOREG;
	MAC_REG_W32(R_AX_PCIE_MIO_INTF, value32);

	count = MIO_SYNC_CNT;
	while (MAC_REG_R32(R_AX_PCIE_MIO_INTF) & B_AX_PCIE_MIO_BYIOREG) {
		PLTFM_DELAY_US(MIO_SYNC_DLY);
		if (--count == 0) {
			PLTFM_MSG_ERR("[ERR]MIO W32 timeout: %X\n",
				      MAC_REG_R32(R_AX_PCIE_MIO_INTF));
			value32 = SET_CLR_WOR2(value32, 0, B_AX_PCIE_MIO_WE_MSK,
					       B_AX_PCIE_MIO_WE_SH);
			value32 &= ~B_AX_PCIE_MIO_BYIOREG;
			MAC_REG_W32(R_AX_PCIE_MIO_INTF, value32);
			MAC_REG_W32(R_AX_SYS_SDIO_CTRL,
				    MAC_REG_R32(R_AX_SYS_SDIO_CTRL) &
				    ~B_AX_PCIE_AUXCLK_GATE);
			return MACPOLLTO;
		}
	}

	MAC_REG_W32(R_AX_SYS_SDIO_CTRL, MAC_REG_R32(R_AX_SYS_SDIO_CTRL) & ~B_AX_PCIE_AUXCLK_GATE);

	return MACSUCCESS;
}

u32 mio_r32_pcie_8852c(struct mac_ax_adapter *adapter, u16 addr, u32 *val)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u8 addr_lsb;
	u8 page_sel;
	u32 value32;
	u16 count;

	page_sel = GET_FIEL2(addr, MIO_ADDR_PAGE_SH,
			     B_AX_PCIE_MIO_ADDR_PAGE_V1_MSK);
	addr_lsb = GET_FIEL2(addr, B_AX_PCIE_MIO_ADDR_SH,
			     B_AX_PCIE_MIO_ADDR_MSK);

	if ((addr_lsb % MIO_4BYTE_ALIGN) != 0) {
		PLTFM_MSG_ERR("[ERR]MIO r32 addr: %X\n", addr_lsb);
		return MACFUNCINPUT;
	}

	MAC_REG_W32(R_AX_SYS_SDIO_CTRL, MAC_REG_R32(R_AX_SYS_SDIO_CTRL) |
		    B_AX_PCIE_AUXCLK_GATE);

	value32 = 0;
	value32 = SET_CLR_WOR2(value32, addr_lsb, B_AX_PCIE_MIO_ADDR_SH,
			       B_AX_PCIE_MIO_ADDR_MSK);
	value32 = SET_CLR_WOR2(value32, page_sel, B_AX_PCIE_MIO_ADDR_PAGE_V1_SH,
			       B_AX_PCIE_MIO_ADDR_PAGE_V1_MSK);
	value32 |= B_AX_PCIE_MIO_RE;
	MAC_REG_W32(R_AX_PCIE_MIO_INTF, value32);
	value32 |= B_AX_PCIE_MIO_BYIOREG;
	MAC_REG_W32(R_AX_PCIE_MIO_INTF, value32);

	count = MIO_SYNC_CNT;
	while (MAC_REG_R32(R_AX_PCIE_MIO_INTF) & B_AX_PCIE_MIO_BYIOREG) {
		PLTFM_DELAY_US(MIO_SYNC_DLY);
		if (--count == 0) {
			PLTFM_MSG_ERR("[ERR]MIO R32 timeout: %X\n",
				      MAC_REG_R32(R_AX_PCIE_MIO_INTF));
			value32 &= ~(B_AX_PCIE_MIO_RE | B_AX_PCIE_MIO_BYIOREG);
			MAC_REG_W32(R_AX_PCIE_MIO_INTF, value32);
			MAC_REG_W32(R_AX_SYS_SDIO_CTRL,
				    MAC_REG_R32(R_AX_SYS_SDIO_CTRL) &
				    ~B_AX_PCIE_AUXCLK_GATE);
			return MACPOLLTO;
		}
	}

	*val = MAC_REG_R32(R_AX_PCIE_MIO_INTD);

	MAC_REG_W32(R_AX_SYS_SDIO_CTRL, MAC_REG_R32(R_AX_SYS_SDIO_CTRL) & ~B_AX_PCIE_AUXCLK_GATE);

	return MACSUCCESS;
}

u32 get_txbd_reg_pcie_8852c(struct mac_ax_adapter *adapter, u8 dma_ch, u32 *reg,
			    enum pcie_bd_ctrl_type type)
{
	*reg = MAC_AX_R32_FF;

	switch (dma_ch) {
	case MAC_AX_DMA_ACH0:
		switch (type) {
		case PCIE_BD_CTRL_DESC_L:
			*reg = R_AX_ACH0_TXBD_DESA_L_V1;
			break;
		case PCIE_BD_CTRL_DESC_H:
			*reg = R_AX_ACH0_TXBD_DESA_H_V1;
			break;
		case PCIE_BD_CTRL_NUM:
			*reg = R_AX_ACH0_TXBD_NUM;
			break;
		case PCIE_BD_CTRL_IDX:
			if (adapter->mac_pwr_info.pwr_in_lps)
				*reg = R_AX_DRV_FW_HSK_0;
			else
				*reg = R_AX_ACH0_TXBD_IDX;
			break;
		case PCIE_BD_CTRL_BDRAM:
			*reg = R_AX_ACH0_BDRAM_CTRL_V1;
			break;
		default:
			PLTFM_MSG_ERR("TXBD reg type%d invalid\n", type);
			return MACFUNCINPUT;
		}
		break;
	case MAC_AX_DMA_ACH1:
		switch (type) {
		case PCIE_BD_CTRL_DESC_L:
			*reg = R_AX_ACH1_TXBD_DESA_L_V1;
			break;
		case PCIE_BD_CTRL_DESC_H:
			*reg = R_AX_ACH1_TXBD_DESA_H_V1;
			break;
		case PCIE_BD_CTRL_NUM:
			*reg = R_AX_ACH1_TXBD_NUM;
			break;
		case PCIE_BD_CTRL_IDX:
			if (adapter->mac_pwr_info.pwr_in_lps)
				*reg = R_AX_DRV_FW_HSK_1;
			else
				*reg = R_AX_ACH1_TXBD_IDX;
			break;
		case PCIE_BD_CTRL_BDRAM:
			*reg = R_AX_ACH1_BDRAM_CTRL_V1;
			break;
		default:
			PLTFM_MSG_ERR("TXBD reg type%d invalid\n", type);
			return MACFUNCINPUT;
		}
		break;
	case MAC_AX_DMA_ACH2:
		switch (type) {
		case PCIE_BD_CTRL_DESC_L:
			*reg = R_AX_ACH2_TXBD_DESA_L_V1;
			break;
		case PCIE_BD_CTRL_DESC_H:
			*reg = R_AX_ACH2_TXBD_DESA_H_V1;
			break;
		case PCIE_BD_CTRL_NUM:
			*reg = R_AX_ACH2_TXBD_NUM;
			break;
		case PCIE_BD_CTRL_IDX:
			if (adapter->mac_pwr_info.pwr_in_lps)
				*reg = R_AX_DRV_FW_HSK_2;
			else
				*reg = R_AX_ACH2_TXBD_IDX;
			break;
		case PCIE_BD_CTRL_BDRAM:
			*reg = R_AX_ACH2_BDRAM_CTRL_V1;
			break;
		default:
			PLTFM_MSG_ERR("TXBD reg type%d invalid\n", type);
			return MACFUNCINPUT;
		}
		break;
	case MAC_AX_DMA_ACH3:
		switch (type) {
		case PCIE_BD_CTRL_DESC_L:
			*reg = R_AX_ACH3_TXBD_DESA_L_V1;
			break;
		case PCIE_BD_CTRL_DESC_H:
			*reg = R_AX_ACH3_TXBD_DESA_H_V1;
			break;
		case PCIE_BD_CTRL_NUM:
			*reg = R_AX_ACH3_TXBD_NUM;
			break;
		case PCIE_BD_CTRL_IDX:
			if (adapter->mac_pwr_info.pwr_in_lps)
				*reg = R_AX_DRV_FW_HSK_3;
			else
				*reg = R_AX_ACH3_TXBD_IDX;
			break;
		case PCIE_BD_CTRL_BDRAM:
			*reg = R_AX_ACH3_BDRAM_CTRL_V1;
			break;
		default:
			PLTFM_MSG_ERR("TXBD reg type%d invalid\n", type);
			return MACFUNCINPUT;
		}
		break;
	case MAC_AX_DMA_ACH4:
		switch (type) {
		case PCIE_BD_CTRL_DESC_L:
			*reg = R_AX_ACH4_TXBD_DESA_L_V1;
			break;
		case PCIE_BD_CTRL_DESC_H:
			*reg = R_AX_ACH4_TXBD_DESA_H_V1;
			break;
		case PCIE_BD_CTRL_NUM:
			*reg = R_AX_ACH4_TXBD_NUM;
			break;
		case PCIE_BD_CTRL_IDX:
			if (adapter->mac_pwr_info.pwr_in_lps)
				return MACNOTSUP;
			else
				*reg = R_AX_ACH4_TXBD_IDX;
			break;
		case PCIE_BD_CTRL_BDRAM:
			*reg = R_AX_ACH4_BDRAM_CTRL_V1;
			break;
		default:
			PLTFM_MSG_ERR("TXBD reg type%d invalid\n", type);
			return MACFUNCINPUT;
		}
		break;
	case MAC_AX_DMA_ACH5:
		switch (type) {
		case PCIE_BD_CTRL_DESC_L:
			*reg = R_AX_ACH5_TXBD_DESA_L_V1;
			break;
		case PCIE_BD_CTRL_DESC_H:
			*reg = R_AX_ACH5_TXBD_DESA_H_V1;
			break;
		case PCIE_BD_CTRL_NUM:
			*reg = R_AX_ACH5_TXBD_NUM;
			break;
		case PCIE_BD_CTRL_IDX:
			if (adapter->mac_pwr_info.pwr_in_lps)
				return MACNOTSUP;
			else
				*reg = R_AX_ACH5_TXBD_IDX;
			break;
		case PCIE_BD_CTRL_BDRAM:
			*reg = R_AX_ACH5_BDRAM_CTRL_V1;
			break;
		default:
			PLTFM_MSG_ERR("TXBD reg type%d invalid\n", type);
			return MACFUNCINPUT;
		}
		break;
	case MAC_AX_DMA_ACH6:
		switch (type) {
		case PCIE_BD_CTRL_DESC_L:
			*reg = R_AX_ACH6_TXBD_DESA_L_V1;
			break;
		case PCIE_BD_CTRL_DESC_H:
			*reg = R_AX_ACH6_TXBD_DESA_H_V1;
			break;
		case PCIE_BD_CTRL_NUM:
			*reg = R_AX_ACH6_TXBD_NUM;
			break;
		case PCIE_BD_CTRL_IDX:
			if (adapter->mac_pwr_info.pwr_in_lps)
				return MACNOTSUP;
			else
				*reg = R_AX_ACH6_TXBD_IDX;
			break;
		case PCIE_BD_CTRL_BDRAM:
			*reg = R_AX_ACH6_BDRAM_CTRL_V1;
			break;
		default:
			PLTFM_MSG_ERR("TXBD reg type%d invalid\n", type);
			return MACFUNCINPUT;
		}
		break;
	case MAC_AX_DMA_ACH7:
		switch (type) {
		case PCIE_BD_CTRL_DESC_L:
			*reg = R_AX_ACH7_TXBD_DESA_L_V1;
			break;
		case PCIE_BD_CTRL_DESC_H:
			*reg = R_AX_ACH7_TXBD_DESA_H_V1;
			break;
		case PCIE_BD_CTRL_NUM:
			*reg = R_AX_ACH7_TXBD_NUM;
			break;
		case PCIE_BD_CTRL_IDX:
			if (adapter->mac_pwr_info.pwr_in_lps)
				return MACNOTSUP;
			else
				*reg = R_AX_ACH7_TXBD_IDX;
			break;
		case PCIE_BD_CTRL_BDRAM:
			*reg = R_AX_ACH7_BDRAM_CTRL_V1;
			break;
		default:
			PLTFM_MSG_ERR("TXBD reg type%d invalid\n", type);
			return MACFUNCINPUT;
		}
		break;
	case MAC_AX_DMA_B0MG:
		switch (type) {
		case PCIE_BD_CTRL_DESC_L:
			*reg = R_AX_CH8_TXBD_DESA_L_V1;
			break;
		case PCIE_BD_CTRL_DESC_H:
			*reg = R_AX_CH8_TXBD_DESA_H_V1;
			break;
		case PCIE_BD_CTRL_NUM:
			*reg = R_AX_CH8_TXBD_NUM;
			break;
		case PCIE_BD_CTRL_IDX:
			if (adapter->mac_pwr_info.pwr_in_lps)
				*reg = R_AX_DRV_FW_HSK_4;
			else
				*reg = R_AX_CH8_TXBD_IDX;
			break;
		case PCIE_BD_CTRL_BDRAM:
			*reg = R_AX_CH8_BDRAM_CTRL_V1;
			break;
		default:
			PLTFM_MSG_ERR("TXBD reg type%d invalid\n", type);
			return MACFUNCINPUT;
		}
		break;
	case MAC_AX_DMA_B0HI:
		switch (type) {
		case PCIE_BD_CTRL_DESC_L:
			*reg = R_AX_CH9_TXBD_DESA_L_V1;
			break;
		case PCIE_BD_CTRL_DESC_H:
			*reg = R_AX_CH9_TXBD_DESA_H_V1;
			break;
		case PCIE_BD_CTRL_NUM:
			*reg = R_AX_CH9_TXBD_NUM;
			break;
		case PCIE_BD_CTRL_IDX:
			if (adapter->mac_pwr_info.pwr_in_lps)
				return MACNOTSUP;
			else
				*reg = R_AX_CH9_TXBD_IDX;
			break;
		case PCIE_BD_CTRL_BDRAM:
			*reg = R_AX_CH9_BDRAM_CTRL_V1;
			break;
		default:
			PLTFM_MSG_ERR("TXBD reg type%d invalid\n", type);
			return MACFUNCINPUT;
		}
		break;
	case MAC_AX_DMA_B1MG:
		switch (type) {
		case PCIE_BD_CTRL_DESC_L:
			*reg = R_AX_CH10_TXBD_DESA_L_V1;
			break;
		case PCIE_BD_CTRL_DESC_H:
			*reg = R_AX_CH10_TXBD_DESA_H_V1;
			break;
		case PCIE_BD_CTRL_NUM:
			*reg = R_AX_CH10_TXBD_NUM_V1;
			break;
		case PCIE_BD_CTRL_IDX:
			if (adapter->mac_pwr_info.pwr_in_lps)
				return MACNOTSUP;
			else
				*reg = R_AX_CH10_TXBD_IDX_V1;
			break;
		case PCIE_BD_CTRL_BDRAM:
			*reg = R_AX_CH10_BDRAM_CTRL_V1;
			break;
		default:
			PLTFM_MSG_ERR("TXBD reg type%d invalid\n", type);
			return MACFUNCINPUT;
		}
		break;
	case MAC_AX_DMA_B1HI:
		switch (type) {
		case PCIE_BD_CTRL_DESC_L:
			*reg = R_AX_CH11_TXBD_DESA_L_V1;
			break;
		case PCIE_BD_CTRL_DESC_H:
			*reg = R_AX_CH11_TXBD_DESA_H_V1;
			break;
		case PCIE_BD_CTRL_NUM:
			*reg = R_AX_CH11_TXBD_NUM_V1;
			break;
		case PCIE_BD_CTRL_IDX:
			if (adapter->mac_pwr_info.pwr_in_lps)
				return MACNOTSUP;
			else
				*reg = R_AX_CH11_TXBD_IDX_V1;
			break;
		case PCIE_BD_CTRL_BDRAM:
			*reg = R_AX_CH11_BDRAM_CTRL_V1;
			break;
		default:
			PLTFM_MSG_ERR("TXBD reg type%d invalid\n", type);
			return MACFUNCINPUT;
		}
		break;
	case MAC_AX_DMA_H2C:
		switch (type) {
		case PCIE_BD_CTRL_DESC_L:
			*reg = R_AX_CH12_TXBD_DESA_L_V1;
			break;
		case PCIE_BD_CTRL_DESC_H:
			*reg = R_AX_CH12_TXBD_DESA_H_V1;
			break;
		case PCIE_BD_CTRL_NUM:
			*reg = R_AX_CH12_TXBD_NUM;
			break;
		case PCIE_BD_CTRL_IDX:
			if (adapter->mac_pwr_info.pwr_in_lps)
				*reg = R_AX_DRV_FW_HSK_5;
			else
				*reg = R_AX_CH12_TXBD_IDX;
			break;
		case PCIE_BD_CTRL_BDRAM:
			*reg = R_AX_CH12_BDRAM_CTRL_V1;
			break;
		default:
			PLTFM_MSG_ERR("TXBD reg type%d invalid\n", type);
			return MACFUNCINPUT;
		}
		break;
	default:
		PLTFM_MSG_ERR("[ERR] TXBD num CH%d invalid\n", dma_ch);
		return MACFUNCINPUT;
	}

	return MACSUCCESS;
}

u32 set_txbd_reg_pcie_8852c(struct mac_ax_adapter *adapter, u8 dma_ch,
			    enum pcie_bd_ctrl_type type, u32 val0, u32 val1, u32 val2)
{
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 ret, reg, val32;
	u16 val16;

	ret = p_ops->get_txbd_reg_pcie(adapter, dma_ch, &reg, type);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("get txbd%d reg type%d %d\n", dma_ch, type, ret);
		return ret;
	}

	switch (type) {
	case PCIE_BD_CTRL_DESC_L:
	case PCIE_BD_CTRL_DESC_H:
		MAC_REG_W32(reg, val0);
		break;
	case PCIE_BD_CTRL_NUM:
		val16 = MAC_REG_R16(reg);
		val16 = SET_CLR_WORD(val16, val0, B_AX_ACH0_DESC_NUM);
		MAC_REG_W16(reg, val16);
		break;
	case PCIE_BD_CTRL_IDX:
		val16 = MAC_REG_R16(reg);
		val16 = SET_CLR_WORD(val16, val0, B_AX_ACH0_HOST_IDX);
		MAC_REG_W16(reg, val16);
		break;
	case PCIE_BD_CTRL_BDRAM:
		val32 = MAC_REG_R32(reg);
		val32 = SET_CLR_WORD(val32, val0, B_AX_ACH0_BDRAM_SIDX);
		val32 = SET_CLR_WORD(val32, val1, B_AX_ACH0_BDRAM_MAX);
		val32 = SET_CLR_WORD(val32, val2, B_AX_ACH0_BDRAM_MIN);
		MAC_REG_W32(reg, val32);
		break;
	default:
		PLTFM_MSG_ERR("TXBD reg type%d invalid\n", type);
		return MACFUNCINPUT;
	}

	return MACSUCCESS;
}

u32 get_rxbd_reg_pcie_8852c(struct mac_ax_adapter *adapter, u8 dma_ch, u32 *reg,
			    enum pcie_bd_ctrl_type type)
{
	*reg = MAC_AX_R32_FF;

	switch (dma_ch) {
	case MAC_AX_RX_CH_RXQ:
		switch (type) {
		case PCIE_BD_CTRL_DESC_L:
			*reg = R_AX_RXQ_RXBD_DESA_L_V1;
			break;
		case PCIE_BD_CTRL_DESC_H:
			*reg = R_AX_RXQ_RXBD_DESA_H_V1;
			break;
		case PCIE_BD_CTRL_NUM:
			*reg = R_AX_RXQ_RXBD_NUM_V1;
			break;
		case PCIE_BD_CTRL_IDX:
			if (adapter->mac_pwr_info.pwr_in_lps)
				*reg = R_AX_DRV_FW_HSK_6;
			else
				*reg = R_AX_RXQ_RXBD_IDX_V1;
			break;
		case PCIE_BD_CTRL_BDRAM:
		default:
			PLTFM_MSG_ERR("RXBD reg type%d invalid\n", type);
			return MACFUNCINPUT;
		}
		break;
	case MAC_AX_RX_CH_RPQ:
		switch (type) {
		case PCIE_BD_CTRL_DESC_L:
			*reg = R_AX_RPQ_RXBD_DESA_L_V1;
			break;
		case PCIE_BD_CTRL_DESC_H:
			*reg = R_AX_RPQ_RXBD_DESA_H_V1;
			break;
		case PCIE_BD_CTRL_NUM:
			*reg = R_AX_RPQ_RXBD_NUM_V1;
			break;
		case PCIE_BD_CTRL_IDX:
			if (adapter->mac_pwr_info.pwr_in_lps)
				*reg = R_AX_DRV_FW_HSK_7;
			else
				*reg = R_AX_RPQ_RXBD_IDX_V1;
			break;
		case PCIE_BD_CTRL_BDRAM:
		default:
			PLTFM_MSG_ERR("RXBD reg type%d invalid\n", type);
			return MACFUNCINPUT;
		}
		break;
	default:
		PLTFM_MSG_ERR("[ERR] RXBD CH%d invalid\n", dma_ch);
		return MACFUNCINPUT;
	}

	return MACSUCCESS;
}

u32 set_rxbd_reg_pcie_8852c(struct mac_ax_adapter *adapter, u8 dma_ch,
			    enum pcie_bd_ctrl_type type, u32 val0, u32 val1, u32 val2)
{
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 ret, reg;
	u16 val16;

	ret = p_ops->get_rxbd_reg_pcie(adapter, dma_ch, &reg, type);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("get rxbd%d reg type%d %d\n", dma_ch, type, ret);
		return ret;
	}

	switch (type) {
	case PCIE_BD_CTRL_DESC_L:
	case PCIE_BD_CTRL_DESC_H:
		MAC_REG_W32(reg, val0);
		break;
	case PCIE_BD_CTRL_NUM:
		val16 = MAC_REG_R16(reg);
		val16 = SET_CLR_WORD(val16, val0, B_AX_RXQ_DESC_NUM);
		MAC_REG_W16(reg, val16);
		break;
	case PCIE_BD_CTRL_IDX:
		val16 = MAC_REG_R16(reg);
		val16 = SET_CLR_WORD(val16, val0, B_AX_RXQ_HOST_IDX);
		MAC_REG_W16(reg, val16);
		break;
	case PCIE_BD_CTRL_BDRAM:
	default:
		PLTFM_MSG_ERR("RXBD reg type%d invalid\n", type);
		return MACFUNCINPUT;
	}

	return MACSUCCESS;
}

static u16 gray_code_to_binary(u16 gray_code, u32 bit_num)
{
	u16 binary = 0, gray_bit;
	u32 bit_idx;

	for (bit_idx = 0; bit_idx < bit_num; bit_idx++) {
		gray_bit = GET_FIEL2(gray_code, bit_idx, 0x1);
		/* not highest bit */
		if (bit_num - bit_idx > 1)
			gray_bit ^= GET_FIEL2(gray_code, (bit_idx + 1), 0x1);
		binary |= (gray_bit << bit_idx);
	}
	return binary;
}

static u32 count_bits(u16 val)
{
	u32 bit_num = 0;

	while (val) {
		if ((val & BIT(0)) == 0x1)
			bit_num++;
		val = val >> 1;
	}
	return bit_num;
}

static u32 _patch_filter_out(struct mac_ax_adapter *adapter)
{
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u16 val16, filter_out_val;
	u32 val32, ret = MACSUCCESS;
	u32 phy_offset = 0;

	if (chk_patch_filter_out(adapter) == PATCH_DISABLE)
		return MACSUCCESS;

	val32 = MAC_REG_R32(R_AX_PCIE_MIX_CFG_V1);
	val32 = GET_FIELD(val32, B_AX_ASPM_CTRL);
	if (val32 == 0x2)
		return MACSUCCESS;

#if MAC_AX_FEATURE_HV
	if (adapter->env == HV_AX_FPGA || adapter->env == HV_AX_PXP)
		return MACSUCCESS;
#endif

	ret = p_ops->mio_r32_pcie(adapter, PCIE_L1_STS_V1, &val32);
	if (ret != MACSUCCESS)
		return ret;
	val32 = GET_FIEL2(val32, BCFG_LINK_SPEED_SH, BCFG_LINK_SPEED_MSK);
	if (val32 == PCIE_GEN1_SPEED) {
		phy_offset = RAC_DIRECT_OFFSET_G1;
	} else if (val32 == PCIE_GEN2_SPEED) {
		phy_offset = RAC_DIRECT_OFFSET_G2;
		val16 = MAC_REG_R16(phy_offset + RAC_ANA10 * RAC_MULT);
		MAC_REG_W16(phy_offset + RAC_ANA10 * RAC_MULT,
			    val16 | PCIE_BIT_PINOUT_DIS);
		val16 = MAC_REG_R16(phy_offset + RAC_ANA19 * RAC_MULT);
		MAC_REG_W16(phy_offset + RAC_ANA19 * RAC_MULT,
			    val16 & ~PCIE_BIT_RD_SEL);
		val16 = MAC_REG_R16(phy_offset + RAC_ANA1F * RAC_MULT);
		val16 = GET_FIELD(val16, FILTER_OUT_EQ);
		val16 = gray_code_to_binary(val16, count_bits(val16));
		filter_out_val = MAC_REG_R16(phy_offset + RAC_ANA24 * RAC_MULT);
		filter_out_val = SET_CLR_WORD(filter_out_val, val16, REG_FILTER_OUT);
		MAC_REG_W16(phy_offset + RAC_ANA24 * RAC_MULT, filter_out_val);

		val16 = MAC_REG_R16(phy_offset + RAC_ANA0A * RAC_MULT);
		MAC_REG_W16(phy_offset + RAC_ANA0A * RAC_MULT,
			    val16 | BAC_EQ_SEL);
		val16 = MAC_REG_R16(RAC_DIRECT_OFFSET_G1 + RAC_ANA0C * RAC_MULT);
		MAC_REG_W16(RAC_DIRECT_OFFSET_G1 + RAC_ANA0C * RAC_MULT,
			    val16 | PCIE_BIT_PSAVE);
	} else {
		PLTFM_MSG_ERR("[ERR]PCIe PHY rate not support\n");
		return MACHWNOSUP;
	}
	val16 = MAC_REG_R16(phy_offset + RAC_ANA0C * RAC_MULT);
	MAC_REG_W16(phy_offset + RAC_ANA0C * RAC_MULT,
		    val16 | PCIE_BIT_PSAVE);

	return MACSUCCESS;
}

u32 pcie_cfgspc_write_8852c(struct mac_ax_adapter *adapter,
			    struct mac_ax_pcie_cfgspc_param *param)
{
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	struct mac_ax_pcie_cfgspc_param *param_def =
		&pcie_cfgspc_param_def_8852c;
	u32 ret, tmp32, l1_val, clk_val, crq_val, aspm_val;
	u8 val8;

	if (chk_patch_fix_emac_delay(adapter) == PATCH_ENABLE)
		return MACSUCCESS;

	ret = _patch_filter_out(adapter);
	if (ret != MACSUCCESS)
		return ret;

	l1_val = MAC_REG_R32(R_AX_PCIE_MIX_CFG_V1);
	ret = p_ops->mio_r32_pcie(adapter, PCIE_ASPM_CTRL_V1, &aspm_val);
	if (ret != MACSUCCESS)
		return ret;
	clk_val = MAC_REG_R32(R_AX_PCIE_LAT_CTRL);
	crq_val = MAC_REG_R32(R_AX_L1_CLK_CTRL);

	ret = p_ops->mio_r32_pcie(adapter, PCIE_L1_STS_V1, &tmp32);
	if (ret != MACSUCCESS)
		return ret;

	if (((tmp32 & PCIE_BIT_STS_L0S) && param->l0s_ctrl ==
	    MAC_AX_PCIE_DEFAULT) || (param->l0s_ctrl != MAC_AX_PCIE_DEFAULT &&
	    param->l0s_ctrl != MAC_AX_PCIE_IGNORE))
		update_pcie_func_u32(&l1_val, PCIE_BIT_L0S_V1,
				     param->l0s_ctrl, param_def->l0s_ctrl);

	if (((tmp32 & PCIE_BIT_STS_L1) && param->l1_ctrl ==
	    MAC_AX_PCIE_DEFAULT) || (param->l1_ctrl != MAC_AX_PCIE_DEFAULT &&
	    param->l1_ctrl != MAC_AX_PCIE_IGNORE))
		update_pcie_func_u32(&l1_val, PCIE_BIT_L1_V1,
				     param->l1_ctrl, param_def->l1_ctrl);
	update_pcie_func_u32(&crq_val, B_AX_CLK_PM_EN,
			     param->crq_ctrl, param_def->crq_ctrl);
	ret = chk_stus_l1ss(adapter, &val8);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR] PCIE chk_stus_l1ss\n");
		return ret;
	}

	if ((val8 && param->l1ss_ctrl == MAC_AX_PCIE_DEFAULT) ||
	    (param->l1ss_ctrl != MAC_AX_PCIE_DEFAULT &&
	     param->l1ss_ctrl != MAC_AX_PCIE_IGNORE)) {
		update_pcie_func_u32(&l1_val, B_AX_L1SUB_DISABLE,
				     param->l1ss_ctrl, param_def->l1ss_ctrl);
		l1_val ^= B_AX_L1SUB_DISABLE;
		ret = _patch_l11_exit(adapter);
		if (ret != MACSUCCESS)
			return ret;
	}

	ret = update_clkdly(adapter, &clk_val, param->clkdly_ctrl,
			    param_def->clkdly_ctrl);
	if (ret != MACSUCCESS)
		return ret;

	ret = update_aspmdly(adapter, &aspm_val, param, param_def);
	if (ret != MACSUCCESS)
		return ret;

	ret = _patch_fix_emac_delay(adapter, &clk_val);
	if (ret != MACSUCCESS)
		return ret;

	if (param->l0s_ctrl != MAC_AX_PCIE_IGNORE ||
	    param->l1_ctrl != MAC_AX_PCIE_IGNORE ||
	    param->l1ss_ctrl != MAC_AX_PCIE_IGNORE) {
		MAC_REG_W32(R_AX_PCIE_MIX_CFG_V1, l1_val);
	}
	if (param->l1dly_ctrl != MAC_AX_PCIE_L1DLY_IGNORE ||
	    param->l0sdly_ctrl != MAC_AX_PCIE_L0SDLY_IGNORE) {
		ret = p_ops->mio_w32_pcie(adapter, PCIE_ASPM_CTRL_V1, aspm_val);
		if (ret != MACSUCCESS)
			return ret;
	}
	if (param->clkdly_ctrl != MAC_AX_PCIE_CLKDLY_IGNORE)
		MAC_REG_W32(R_AX_PCIE_LAT_CTRL, clk_val);

	if (param->crq_ctrl != MAC_AX_PCIE_IGNORE)
		MAC_REG_W32(R_AX_L1_CLK_CTRL, crq_val);

	return ret;
}

u32 pcie_cfgspc_read_8852c(struct mac_ax_adapter *adapter,
			   struct mac_ax_pcie_cfgspc_param *param)
{
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 l1_val, clk_val, crq_val;
	u32 aspm_val;
	u32 l0smask;
	u32 l1mask;
	u32 ret = MACSUCCESS;

	l1_val = MAC_REG_R32(R_AX_PCIE_MIX_CFG_V1);
	ret = p_ops->mio_r32_pcie(adapter, PCIE_ASPM_CTRL_V1, &aspm_val);
	if (ret != MACSUCCESS)
		return ret;
	clk_val = MAC_REG_R32(R_AX_PCIE_LAT_CTRL);
	clk_val = GET_FIEL2(clk_val, B_AX_CLK_REQ_LAT_SH, B_AX_CLK_REQ_LAT_MSK);
	crq_val = MAC_REG_R32(R_AX_L1_CLK_CTRL);

	param->l0s_ctrl = GET_PCIE_FUNC_STUS(l1_val, PCIE_BIT_L0S_V1);
	param->l1_ctrl = GET_PCIE_FUNC_STUS(l1_val, PCIE_BIT_L1_V1);
	param->l1ss_ctrl = GET_PCIE_FUNC_STUS(~l1_val, B_AX_L1SUB_DISABLE);
	param->wake_ctrl = MAC_AX_PCIE_ENABLE;
	param->crq_ctrl = GET_PCIE_FUNC_STUS(crq_val, B_AX_CLK_PM_EN);

	switch (clk_val) {
	case PCIE_CLKDLY_HW_V1_0:
		param->clkdly_ctrl = MAC_AX_PCIE_CLKDLY_V1_0;
		break;

	case PCIE_CLKDLY_HW_V1_16US:
		param->clkdly_ctrl = MAC_AX_PCIE_CLKDLY_V1_16US;
		break;

	case PCIE_CLKDLY_HW_V1_32US:
		param->clkdly_ctrl = MAC_AX_PCIE_CLKDLY_V1_32US;
		break;

	case PCIE_CLKDLY_HW_V1_64US:
		param->clkdly_ctrl = MAC_AX_PCIE_CLKDLY_V1_64US;
		break;

	case PCIE_CLKDLY_HW_V1_80US:
		param->clkdly_ctrl = MAC_AX_PCIE_CLKDLY_V1_80US;
		break;

	case PCIE_CLKDLY_HW_V1_96US:
		param->clkdly_ctrl = MAC_AX_PCIE_CLKDLY_V1_96US;
		break;

	default:
		param->clkdly_ctrl = MAC_AX_PCIE_CLKDLY_R_ERR;
		break;
	}

	l0smask = PCIE_ASPMDLY_MASK << SHFT_L0SDLY_V1;
	l1mask = PCIE_ASPMDLY_MASK << SHFT_L1DLY_V1;

	switch ((aspm_val & l0smask) >> SHFT_L0SDLY_V1) {
	case PCIE_L0SDLY_HW_1US:
		param->l0sdly_ctrl = MAC_AX_PCIE_L0SDLY_1US;
		break;

	case PCIE_L0SDLY_HW_3US:
		param->l0sdly_ctrl = MAC_AX_PCIE_L0SDLY_3US;
		break;

	case PCIE_L0SDLY_HW_5US:
		param->l0sdly_ctrl = MAC_AX_PCIE_L0SDLY_5US;
		break;

	case PCIE_L0SDLY_HW_7US:
		param->l0sdly_ctrl = MAC_AX_PCIE_L0SDLY_7US;
		break;

	default:
		param->l0sdly_ctrl = MAC_AX_PCIE_L0SDLY_R_ERR;
		break;
	}

	switch ((aspm_val & l1mask) >> SHFT_L1DLY_V1) {
	case PCIE_L1DLY_HW_16US:
		param->l1dly_ctrl = MAC_AX_PCIE_L1DLY_16US;
		break;

	case PCIE_L1DLY_HW_32US:
		param->l1dly_ctrl = MAC_AX_PCIE_L1DLY_32US;
		break;

	case PCIE_L1DLY_HW_64US:
		param->l1dly_ctrl = MAC_AX_PCIE_L1DLY_64US;
		break;

	case PCIE_L1DLY_HW_INFI:
		param->l1dly_ctrl = MAC_AX_PCIE_L1DLY_INFI;
		break;

	default:
		param->l1dly_ctrl = MAC_AX_PCIE_L1DLY_R_ERR;
		break;
	}

	return ret;
}

u32 pcie_ltr_write_8852c(struct mac_ax_adapter *adapter,
			 struct mac_ax_pcie_ltr_param *param)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	struct mac_ax_pcie_ltr_param *param_def = &pcie_ltr_param_def_8852c;
	u32 ctrl0, ctrl1, dec_ctrl, idle_ltcy, act_ltcy;
	enum mac_ax_pcie_func_ctrl ctrl;
	u32 val32;

	ctrl0 = MAC_REG_R32(R_AX_LTR_CTRL_0);
	ctrl1 = MAC_REG_R32(R_AX_LTR_CTRL_1);
	dec_ctrl = MAC_REG_R32(R_AX_LTR_DEC_CTRL);
	idle_ltcy = MAC_REG_R32(ltr_ltcy_regl_8852c[PCIE_LTR_IDX_IDLE]);
	act_ltcy = MAC_REG_R32(ltr_ltcy_regl_8852c[PCIE_LTR_IDX_ACT]);

	if (ctrl0 == MAC_AX_R32_FF || ctrl0 == MAC_AX_R32_EA ||
	    ctrl1 == MAC_AX_R32_FF || ctrl1 == MAC_AX_R32_EA ||
	    dec_ctrl == MAC_AX_R32_FF || dec_ctrl == MAC_AX_R32_EA ||
	    idle_ltcy == MAC_AX_R32_FF || idle_ltcy == MAC_AX_R32_EA ||
	    act_ltcy == MAC_AX_R32_FF || act_ltcy == MAC_AX_R32_EA) {
		PLTFM_MSG_ERR("[ERR] LTR reg read nothing!\n");
		return MACCMP;
	}

	if (!(param_def->ltr_ctrl == MAC_AX_PCIE_IGNORE ||
	      param->ltr_ctrl == MAC_AX_PCIE_IGNORE ||
	      param_def->ltr_ctrl == MAC_AX_PCIE_DEFAULT)) {
		ctrl = param->ltr_ctrl == MAC_AX_PCIE_DEFAULT ?
		       param_def->ltr_ctrl : param->ltr_ctrl;
		if (ctrl == MAC_AX_PCIE_DISABLE) {
			dec_ctrl &= ~LTR_EN_BITS;
			dec_ctrl = SET_CLR_WORD(dec_ctrl, PCIE_LTR_IDX_IDLE,
						B_AX_LTR_IDX_DRV);
			dec_ctrl |= B_AX_LTR_REQ_DRV;
		}
	}

	if (!(param_def->ltr_hw_ctrl == MAC_AX_PCIE_IGNORE ||
	      param->ltr_hw_ctrl == MAC_AX_PCIE_IGNORE ||
	      param_def->ltr_hw_ctrl == MAC_AX_PCIE_DEFAULT)) {
		ctrl = param->ltr_hw_ctrl == MAC_AX_PCIE_DEFAULT ?
		       param_def->ltr_hw_ctrl : param->ltr_hw_ctrl;
		if (ctrl == MAC_AX_PCIE_ENABLE)
			ctrl0 |= B_AX_LTR_WD_NOEMP_CHK_V1 | B_AX_LTR_HW_EN;
		dec_ctrl = ctrl == MAC_AX_PCIE_ENABLE ?
			   (dec_ctrl | B_AX_LTR_HW_DEC_EN) :
			   (dec_ctrl & ~B_AX_LTR_HW_DEC_EN);
	}

	if (!(param_def->ltr_spc_ctrl == MAC_AX_PCIE_LTR_SPC_IGNORE ||
	      param->ltr_spc_ctrl == MAC_AX_PCIE_LTR_SPC_IGNORE ||
	      param_def->ltr_spc_ctrl == MAC_AX_PCIE_LTR_SPC_DEF)) {
		val32 = param->ltr_spc_ctrl == MAC_AX_PCIE_LTR_SPC_DEF ?
			param_def->ltr_spc_ctrl : param->ltr_spc_ctrl;
		dec_ctrl = SET_CLR_WORD(dec_ctrl, val32, B_AX_LTR_SPACE_IDX_V1);
	}

	if (!(param_def->ltr_idle_timer_ctrl ==
	      MAC_AX_PCIE_LTR_IDLE_TIMER_IGNORE ||
	      param->ltr_idle_timer_ctrl ==
	      MAC_AX_PCIE_LTR_IDLE_TIMER_IGNORE ||
	      param_def->ltr_idle_timer_ctrl ==
	      MAC_AX_PCIE_LTR_IDLE_TIMER_DEF)) {
		val32 = param->ltr_idle_timer_ctrl ==
			MAC_AX_PCIE_LTR_IDLE_TIMER_DEF ?
			param_def->ltr_idle_timer_ctrl :
			param->ltr_idle_timer_ctrl;
		ctrl0 = SET_CLR_WORD(ctrl0, val32, B_AX_LTR_IDLE_TIMER_IDX);
	}

	if (!(param_def->ltr_rx0_th_ctrl.ctrl == MAC_AX_PCIE_IGNORE ||
	      param->ltr_rx0_th_ctrl.ctrl == MAC_AX_PCIE_IGNORE ||
	      param_def->ltr_rx0_th_ctrl.ctrl == MAC_AX_PCIE_DEFAULT)) {
		val32 = param->ltr_rx0_th_ctrl.ctrl == MAC_AX_PCIE_DEFAULT ?
			param_def->ltr_rx0_th_ctrl.val :
			param->ltr_rx0_th_ctrl.val;
		ctrl1 = SET_CLR_WORD(ctrl1, val32, B_AX_LTR_RX0_TH);
	}

	if (!(param_def->ltr_rx1_th_ctrl.ctrl == MAC_AX_PCIE_IGNORE ||
	      param->ltr_rx1_th_ctrl.ctrl == MAC_AX_PCIE_IGNORE ||
	      param_def->ltr_rx1_th_ctrl.ctrl == MAC_AX_PCIE_DEFAULT)) {
		val32 = param->ltr_rx1_th_ctrl.ctrl == MAC_AX_PCIE_DEFAULT ?
			param_def->ltr_rx1_th_ctrl.val :
			param->ltr_rx1_th_ctrl.val;
		ctrl1 = SET_CLR_WORD(ctrl1, val32, B_AX_LTR_RX1_TH);
	}

	if (!(param_def->ltr_idle_lat_ctrl.ctrl == MAC_AX_PCIE_IGNORE ||
	      param->ltr_idle_lat_ctrl.ctrl == MAC_AX_PCIE_IGNORE ||
	      param_def->ltr_idle_lat_ctrl.ctrl == MAC_AX_PCIE_DEFAULT)) {
		idle_ltcy =
			param->ltr_idle_lat_ctrl.ctrl == MAC_AX_PCIE_DEFAULT ?
			param_def->ltr_idle_lat_ctrl.val :
			param->ltr_idle_lat_ctrl.val;
	}

	if (!(param_def->ltr_act_lat_ctrl.ctrl == MAC_AX_PCIE_IGNORE ||
	      param->ltr_act_lat_ctrl.ctrl == MAC_AX_PCIE_IGNORE ||
	      param_def->ltr_act_lat_ctrl.ctrl == MAC_AX_PCIE_DEFAULT)) {
		act_ltcy =
			param->ltr_act_lat_ctrl.ctrl == MAC_AX_PCIE_DEFAULT ?
			param_def->ltr_act_lat_ctrl.val :
			param->ltr_act_lat_ctrl.val;
	}

	MAC_REG_W32(R_AX_LTR_CTRL_0, ctrl0);
	MAC_REG_W32(R_AX_LTR_CTRL_1, ctrl1);
	MAC_REG_W32(R_AX_LTR_DEC_CTRL, dec_ctrl);
	MAC_REG_W32(ltr_ltcy_regl_8852c[PCIE_LTR_IDX_IDLE], idle_ltcy);
	MAC_REG_W32(ltr_ltcy_regl_8852c[PCIE_LTR_IDX_ACT], act_ltcy);

	return MACSUCCESS;
}

u32 pcie_ltr_read_8852c(struct mac_ax_adapter *adapter,
			struct mac_ax_pcie_ltr_param *param)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 ctrl0, ctrl1, dec_ctrl, idle_ltcy, act_ltcy;

	ctrl0 = MAC_REG_R32(R_AX_LTR_CTRL_0);
	ctrl1 = MAC_REG_R32(R_AX_LTR_CTRL_1);
	dec_ctrl = MAC_REG_R32(R_AX_LTR_DEC_CTRL);
	idle_ltcy = MAC_REG_R32(ltr_ltcy_regl_8852c[PCIE_LTR_IDX_IDLE]);
	act_ltcy = MAC_REG_R32(ltr_ltcy_regl_8852c[PCIE_LTR_IDX_ACT]);

	if (ctrl0 == MAC_AX_R32_FF || ctrl0 == MAC_AX_R32_EA ||
	    ctrl1 == MAC_AX_R32_FF || ctrl1 == MAC_AX_R32_EA ||
	    dec_ctrl == MAC_AX_R32_FF || dec_ctrl == MAC_AX_R32_EA ||
	    idle_ltcy == MAC_AX_R32_FF || idle_ltcy == MAC_AX_R32_EA ||
	    act_ltcy == MAC_AX_R32_FF || act_ltcy == MAC_AX_R32_EA) {
		PLTFM_MSG_ERR("[ERR] LTR reg read nothing!\n");
		return MACCMP;
	}

	param->ltr_ctrl = (dec_ctrl & LTR_EN_BITS) ?
			  MAC_AX_PCIE_ENABLE : MAC_AX_PCIE_DISABLE;

	param->ltr_hw_ctrl =
		((dec_ctrl & B_AX_LTR_HW_DEC_EN) && (ctrl0 & B_AX_LTR_HW_EN)) ?
		MAC_AX_PCIE_ENABLE : MAC_AX_PCIE_DISABLE;

	param->ltr_spc_ctrl =
		(enum mac_ax_pcie_ltr_spc)
		GET_FIELD(dec_ctrl, B_AX_LTR_SPACE_IDX_V1);
	param->ltr_idle_timer_ctrl =
		(enum mac_ax_pcie_ltr_idle_timer)
		GET_FIELD(ctrl0, B_AX_LTR_IDLE_TIMER_IDX);

	param->ltr_rx0_th_ctrl.val = (u16)GET_FIELD(ctrl1, B_AX_LTR_RX0_TH);
	param->ltr_rx1_th_ctrl.val = (u16)GET_FIELD(ctrl1, B_AX_LTR_RX1_TH);
	param->ltr_idle_lat_ctrl.val = idle_ltcy;
	param->ltr_act_lat_ctrl.val = act_ltcy;

	return MACSUCCESS;
}

u32 ltr_sw_trigger_8852c(struct mac_ax_adapter *adapter,
			 enum mac_ax_pcie_ltr_sw_ctrl ctrl)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32;

	val32 = MAC_REG_R32(R_AX_LTR_DEC_CTRL);
	if (!(val32 & B_AX_LTR_DRV_DEC_EN)) {
		val32 |= B_AX_LTR_DRV_DEC_EN;
		MAC_REG_W32(R_AX_LTR_DEC_CTRL, val32);
	}
	switch (ctrl) {
	case MAC_AX_PCIE_LTR_SW_ACT:
		val32 = SET_CLR_WORD(val32, PCIE_LTR_IDX_ACT, B_AX_LTR_IDX_DRV);
		break;
	case MAC_AX_PCIE_LTR_SW_IDLE:
		val32 = SET_CLR_WORD(val32, PCIE_LTR_IDX_IDLE, B_AX_LTR_IDX_DRV);
		break;
	default:
		PLTFM_MSG_ERR("[ERR]Invalid LTR SW ctrl %d\n", ctrl);
		return MACFUNCINPUT;
	}
	val32 |= B_AX_LTR_REQ_DRV;
	MAC_REG_W32(R_AX_LTR_DEC_CTRL, val32);

	return MACSUCCESS;
}

u32 get_avail_txbd_8852c(struct mac_ax_adapter *adapter, u8 ch_idx,
			 u16 *host_idx, u16 *hw_idx, u16 *avail_txbd)
{
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 ret = MACSUCCESS, reg = 0, tmp32;
	u16 bndy = adapter->pcie_info.txbd_bndy;
	u8 tx_dma_ch = 0;

	tx_dma_ch = MAC_AX_DMA_ACH0 + ch_idx;

	ret = p_ops->get_txbd_reg_pcie(adapter, tx_dma_ch, &reg, PCIE_BD_CTRL_IDX);
	if (ret != MACSUCCESS) {
		*avail_txbd = 0;
		return ret;
	}

	tmp32 = MAC_REG_R32(reg);
	*host_idx = (u16)GET_FIELD(tmp32, B_AX_ACH0_HOST_IDX);
	*hw_idx = (u16)GET_FIELD(tmp32, B_AX_ACH0_HW_IDX);
	*avail_txbd = calc_avail_wptr(*hw_idx, *host_idx, bndy);
	PLTFM_MSG_TRACE("%s: ", __func__);
	PLTFM_MSG_TRACE("dma_ch %d, host_idx %d, hw_idx %d avail_txbd %d\n",
			ch_idx, *host_idx, *hw_idx, avail_txbd);

	return MACSUCCESS;
}

u32 get_avail_rxbd_8852c(struct mac_ax_adapter *adapter, u8 ch_idx,
			 u16 *host_idx, u16 *hw_idx, u16 *avail_rxbd)
{
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 ret = MACSUCCESS, reg = 0, tmp32;
	u16 bndy;
	u8 rx_dma_ch = 0;

	rx_dma_ch = MAC_AX_RX_CH_RXQ + ch_idx;

	if (rx_dma_ch == MAC_AX_RX_CH_RXQ) {
		bndy = adapter->pcie_info.rxbd_bndy;
	} else if (rx_dma_ch == MAC_AX_RX_CH_RPQ) {
		bndy = adapter->pcie_info.rpbd_bndy;
	} else {
		PLTFM_MSG_ERR("Unkwown rx_dma_ch: %d\n", rx_dma_ch);
		return MACFUNCINPUT;
	}

	ret = p_ops->get_rxbd_reg_pcie(adapter, rx_dma_ch, &reg, PCIE_BD_CTRL_IDX);
	if (ret != MACSUCCESS) {
		*avail_rxbd = 0;
		return ret;
	}

	tmp32 = MAC_REG_R32(reg);
	*host_idx = (u16)GET_FIELD(tmp32, B_AX_RXQ_HOST_IDX);
	*hw_idx = (u16)GET_FIELD(tmp32, B_AX_RXQ_HW_IDX);
	*avail_rxbd = calc_avail_rptr(*host_idx, *hw_idx, bndy);

	return MACSUCCESS;
}

u32 get_io_stat_pcie_8852c(struct mac_ax_adapter *adapter,
			   struct mac_ax_io_stat *out_st)
{
	return MACNOTSUP;
}

u32 ctrl_hci_dma_en_pcie_8852c(struct mac_ax_adapter *adapter,
			       enum mac_ax_pcie_func_ctrl txen,
			       enum mac_ax_pcie_func_ctrl rxen)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 reg = R_AX_HCI_FUNC_EN_V1;
	u32 rval = MAC_REG_R32(reg);

	if (txen == MAC_AX_PCIE_ENABLE)
		rval |= B_AX_HCI_TXDMA_EN;
	else if (txen == MAC_AX_PCIE_DISABLE)
		rval &= ~B_AX_HCI_TXDMA_EN;

	if (rxen == MAC_AX_PCIE_ENABLE)
		rval |= B_AX_HCI_RXDMA_EN;
	else if (rxen == MAC_AX_PCIE_DISABLE)
		rval &= ~B_AX_HCI_RXDMA_EN;

	MAC_REG_W32(reg, rval);

	return MACSUCCESS;
}

u32 ctrl_trxdma_pcie_8852c(struct mac_ax_adapter *adapter,
			   enum mac_ax_pcie_func_ctrl txen,
			   enum mac_ax_pcie_func_ctrl rxen,
			   enum mac_ax_pcie_func_ctrl ioen)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 reg_init = R_AX_HAXI_INIT_CFG1;
	u32 val_init = MAC_REG_R32(reg_init);

	if (txen == MAC_AX_PCIE_ENABLE)
		val_init |= B_AX_TXHCI_EN_V1;
	else if (txen == MAC_AX_PCIE_DISABLE)
		val_init &= ~B_AX_TXHCI_EN_V1;

	if (rxen == MAC_AX_PCIE_ENABLE)
		val_init |= B_AX_RXHCI_EN_V1;
	else if (rxen == MAC_AX_PCIE_DISABLE)
		val_init &= ~B_AX_RXHCI_EN_V1;

	if (ioen == MAC_AX_PCIE_ENABLE)
		val_init &= ~B_AX_STOP_AXI_MST;
	else if (ioen == MAC_AX_PCIE_DISABLE)
		val_init |= B_AX_STOP_AXI_MST;

	MAC_REG_W32(reg_init, val_init);

	return MACSUCCESS;
}

u32 ctrl_txdma_ch_pcie_8852c(struct mac_ax_adapter *adapter,
			     struct mac_ax_txdma_ch_map *ch_map)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 reg_stop1 = R_AX_HAXI_DMA_STOP1;
	u32 reg_stop2 = R_AX_HAXI_DMA_STOP2;
	u32 val32;

	val32 = MAC_REG_R32(reg_stop1);
	if (ch_map->ch0 == MAC_AX_PCIE_ENABLE)
		val32 &= ~B_AX_STOP_ACH0;
	else if (ch_map->ch0 == MAC_AX_PCIE_DISABLE)
		val32 |= B_AX_STOP_ACH0;

	if (ch_map->ch1 == MAC_AX_PCIE_ENABLE)
		val32 &= ~B_AX_STOP_ACH1;
	else if (ch_map->ch1 == MAC_AX_PCIE_DISABLE)
		val32 |= B_AX_STOP_ACH1;

	if (ch_map->ch2 == MAC_AX_PCIE_ENABLE)
		val32 &= ~B_AX_STOP_ACH2;
	else if (ch_map->ch2 == MAC_AX_PCIE_DISABLE)
		val32 |= B_AX_STOP_ACH2;

	if (ch_map->ch3 == MAC_AX_PCIE_ENABLE)
		val32 &= ~B_AX_STOP_ACH3;
	else if (ch_map->ch3 == MAC_AX_PCIE_DISABLE)
		val32 |= B_AX_STOP_ACH3;

	if (ch_map->ch4 == MAC_AX_PCIE_ENABLE)
		val32 &= ~B_AX_STOP_ACH4;
	else if (ch_map->ch4 == MAC_AX_PCIE_DISABLE)
		val32 |= B_AX_STOP_ACH4;

	if (ch_map->ch5 == MAC_AX_PCIE_ENABLE)
		val32 &= ~B_AX_STOP_ACH5;
	else if (ch_map->ch5 == MAC_AX_PCIE_DISABLE)
		val32 |= B_AX_STOP_ACH5;

	if (ch_map->ch6 == MAC_AX_PCIE_ENABLE)
		val32 &= ~B_AX_STOP_ACH6;
	else if (ch_map->ch6 == MAC_AX_PCIE_DISABLE)
		val32 |= B_AX_STOP_ACH6;

	if (ch_map->ch7 == MAC_AX_PCIE_ENABLE)
		val32 &= ~B_AX_STOP_ACH7;
	else if (ch_map->ch7 == MAC_AX_PCIE_DISABLE)
		val32 |= B_AX_STOP_ACH7;

	if (ch_map->ch8 == MAC_AX_PCIE_ENABLE)
		val32 &= ~B_AX_STOP_CH8;
	else if (ch_map->ch8 == MAC_AX_PCIE_DISABLE)
		val32 |= B_AX_STOP_CH8;

	if (ch_map->ch9 == MAC_AX_PCIE_ENABLE)
		val32 &= ~B_AX_STOP_CH9;
	else if (ch_map->ch9 == MAC_AX_PCIE_DISABLE)
		val32 |= B_AX_STOP_CH9;

	if (ch_map->ch12 == MAC_AX_PCIE_ENABLE)
		val32 &= ~B_AX_STOP_CH12;
	else if (ch_map->ch12 == MAC_AX_PCIE_DISABLE)
		val32 |= B_AX_STOP_CH12;
	MAC_REG_W32(reg_stop1, val32);

	val32 = MAC_REG_R32(reg_stop2);
	if (ch_map->ch10 == MAC_AX_PCIE_ENABLE)
		val32 &= ~B_AX_STOP_CH10;
	else if (ch_map->ch10 == MAC_AX_PCIE_DISABLE)
		val32 |= B_AX_STOP_CH10;

	if (ch_map->ch11 == MAC_AX_PCIE_ENABLE)
		val32 &= ~B_AX_STOP_CH11;
	else if (ch_map->ch11 == MAC_AX_PCIE_DISABLE)
		val32 |= B_AX_STOP_CH11;
	MAC_REG_W32(reg_stop2, val32);

	return MACSUCCESS;
}

u32 set_pcie_speed_8852c(struct mac_ax_adapter *adapter,
			 enum mac_ax_pcie_phy set_speed)
{
	u32 ret;
	u32 support_gen;
	u32 cnt;
	u32 poll_val32;
	u8 link_speed;
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);

	if  (!(set_speed == MAC_AX_PCIE_PHY_GEN1 || set_speed == MAC_AX_PCIE_PHY_GEN2)) {
		PLTFM_MSG_ERR("[ERR]Do not support that speed!\n");
		return MACFUNCINPUT;
	}

	ret = p_ops->mio_r32_pcie(adapter, PCIE_CAPABILITY_SPEED, &support_gen);
	if (ret != MACSUCCESS)
		return ret;
	support_gen = GET_FIEL2(support_gen, PCIE_SUPPORT_GEN_SH, PCIE_LINK_SPEED_BITS_MSK);

	if (support_gen == MAC_AX_PCIE_PHY_GEN1) {
		if (set_speed == MAC_AX_PCIE_PHY_GEN1) {
			ret = MACSUCCESS;
		} else {
			PLTFM_MSG_ERR("[ERR]Do not support that gen!\n");
			ret = MACFUNCINPUT;
		}
	} else if (support_gen == MAC_AX_PCIE_PHY_GEN2) {
		ret = get_pcie_speed_8852c(adapter, &link_speed);
		if (link_speed == set_speed) {
			ret = MACSUCCESS;
		} else {
			ret = p_ops->mio_w32_pcie(adapter, PCIE_LINK_CHANGE_SPEED, set_speed);
			if (ret != MACSUCCESS)
				return ret;
			ret = p_ops->mio_r32_pcie(adapter, PCIE_FTS, &poll_val32);
			if (ret != MACSUCCESS)
				return ret;
			ret = p_ops->mio_w32_pcie(adapter, PCIE_FTS, poll_val32 | PCIE_POLLING_BIT);
			if (ret != MACSUCCESS)
				return ret;

			cnt = PCIE_POLL_SPEED_CHANGE_CNT;
			while (cnt) {
				ret = p_ops->mio_r32_pcie(adapter, PCIE_FTS, &poll_val32);
				if (ret != MACSUCCESS)
					return ret;
				if (!(poll_val32 & PCIE_POLLING_BIT))
					break;
				cnt--;
				PLTFM_DELAY_US(PCIE_POLL_IO_IDLE_DLY_US);
			}
			if (!cnt) {
				PLTFM_MSG_ERR("[ERR]PCIE polling timeout\n");
				return MACPOLLTO;
			}
		}
	} else {
		PLTFM_MSG_ERR("[ERR]Do Not support that speed gen!\n");
		ret = MACFUNCINPUT;
	}

	return ret;
}

u32 get_pcie_speed_8852c(struct mac_ax_adapter *adapter,
			 u8 *speed)
{
	u32 ret;
	u32 val32;
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);

	ret = p_ops->mio_r32_pcie(adapter, PCIE_LINK_SPEED_32BIT, &val32);
	if (ret != MACSUCCESS)
		return ret;
	val32 = GET_FIEL2(val32, PCIE_LINK_SPEED_SH, PCIE_LINK_SPEED_BITS_MSK);

	if (val32 == MAC_AX_PCIE_PHY_GEN1) {
		*speed = MAC_AX_PCIE_PHY_GEN1;
	} else if (val32 == MAC_AX_PCIE_PHY_GEN2) {
		*speed = MAC_AX_PCIE_PHY_GEN2;
	} else {
		ret = MACFUNCINPUT;
		PLTFM_MSG_ERR("[ERR]Do not support that speed!\n");
	}

	return ret;
}

u32 ctrl_wpdma_pcie_8852c(struct mac_ax_adapter *adapter,
			  enum mac_ax_pcie_func_ctrl wpen)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 reg_stop1 = R_AX_HAXI_DMA_STOP1;
	u32 val32;

	val32 = MAC_REG_R32(reg_stop1);
	if (wpen == MAC_AX_PCIE_ENABLE)
		val32 &= ~B_AX_STOP_WPDMA;
	else if (wpen == MAC_AX_PCIE_DISABLE)
		val32 |= B_AX_STOP_WPDMA;
	MAC_REG_W32(reg_stop1, val32);

	return MACSUCCESS;
}

u32 poll_io_idle_pcie_8852c(struct mac_ax_adapter *adapter)
{
#define B_IO_BUSY (B_AX_HAXIIO_BUSY)
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 reg_busy1 = R_AX_HAXI_DMA_BUSY1;
	u32 val32;
	u32 cnt;

	cnt = PCIE_POLL_IO_IDLE_CNT;
	while (cnt) {
		val32 = MAC_REG_R32(reg_busy1);
		if (!(val32 & B_IO_BUSY))
			break;
		cnt--;
		PLTFM_DELAY_US(PCIE_POLL_IO_IDLE_DLY_US);
	}

	if (!cnt) {
		PLTFM_MSG_ERR("[ERR]PCIE dmach busy1 0x%X\n", val32);
		return MACPOLLTO;
	}

	return MACSUCCESS;
}

u32 poll_txdma_ch_idle_pcie_8852c(struct mac_ax_adapter *adapter,
				  struct mac_ax_txdma_ch_map *ch_map)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 reg_busy1 = R_AX_HAXI_DMA_BUSY1;
	u32 reg_busy2 = R_AX_HAXI_DMA_BUSY2;
	u32 val32 = 0, rval32, cnt;

	if (ch_map->ch0 == MAC_AX_PCIE_ENABLE)
		val32 |= B_AX_ACH0_BUSY;

	if (ch_map->ch1 == MAC_AX_PCIE_ENABLE)
		val32 |= B_AX_ACH1_BUSY;

	if (ch_map->ch2 == MAC_AX_PCIE_ENABLE)
		val32 |= B_AX_ACH2_BUSY;

	if (ch_map->ch3 == MAC_AX_PCIE_ENABLE)
		val32 |= B_AX_ACH3_BUSY;

	if (ch_map->ch4 == MAC_AX_PCIE_ENABLE)
		val32 |= B_AX_ACH4_BUSY;

	if (ch_map->ch5 == MAC_AX_PCIE_ENABLE)
		val32 |= B_AX_ACH5_BUSY;

	if (ch_map->ch6 == MAC_AX_PCIE_ENABLE)
		val32 |= B_AX_ACH6_BUSY;

	if (ch_map->ch7 == MAC_AX_PCIE_ENABLE)
		val32 |= B_AX_ACH7_BUSY;

	if (ch_map->ch8 == MAC_AX_PCIE_ENABLE)
		val32 |= B_AX_CH8_BUSY;

	if (ch_map->ch9 == MAC_AX_PCIE_ENABLE)
		val32 |= B_AX_CH9_BUSY;

	if (ch_map->ch12 == MAC_AX_PCIE_ENABLE)
		val32 |= B_AX_CH12_BUSY;

	cnt = PCIE_POLL_DMACH_IDLE_CNT;
	while (cnt) {
		rval32 = MAC_REG_R32(reg_busy1);
		if (!(rval32 & val32))
			break;
		cnt--;
		PLTFM_DELAY_US(PCIE_POLL_DMACH_IDLE_DLY_US);
	}

	if (!cnt) {
		PLTFM_MSG_ERR("[ERR]PCIE dmach busy1 0x%X\n", rval32);
		return MACPOLLTO;
	}

	val32 = 0;
	if (ch_map->ch10 == MAC_AX_PCIE_ENABLE)
		val32 |= B_AX_CH10_BUSY;

	if (ch_map->ch11 == MAC_AX_PCIE_ENABLE)
		val32 |= B_AX_CH11_BUSY;

	cnt = PCIE_POLL_DMACH_IDLE_CNT;
	while (cnt) {
		rval32 = MAC_REG_R32(reg_busy2);
		if (!(rval32 & val32))
			break;
		cnt--;
		PLTFM_DELAY_US(PCIE_POLL_DMACH_IDLE_DLY_US);
	}

	if (!cnt) {
		PLTFM_MSG_ERR("[ERR]PCIE dmach busy2 0x%X\n", rval32);
		return MACPOLLTO;
	}

	return MACSUCCESS;
}

u32 poll_rxdma_ch_idle_pcie_8852c(struct mac_ax_adapter *adapter,
				  struct mac_ax_rxdma_ch_map *ch_map)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 reg_busy3 = R_AX_HAXI_DMA_BUSY3;
	u32 val32 = 0, rval32, cnt;

	if (ch_map->rxq == MAC_AX_PCIE_ENABLE)
		val32 |= B_AX_RXQ_BUSY;
	if (ch_map->rpq == MAC_AX_PCIE_ENABLE)
		val32 |= B_AX_RPQ_BUSY;

	cnt = PCIE_POLL_DMACH_IDLE_CNT;
	while (cnt) {
		rval32 = MAC_REG_R32(reg_busy3);
		if (!(rval32 & val32))
			break;
		cnt--;
		PLTFM_DELAY_US(PCIE_POLL_DMACH_IDLE_DLY_US);
	}

	if (!cnt) {
		PLTFM_MSG_ERR("[ERR]PCIE dmach busy3 0x%X\n", rval32);
		return MACPOLLTO;
	}

	return MACSUCCESS;
}

u32 poll_dma_all_idle_pcie_8852c(struct mac_ax_adapter *adapter)
{
	struct mac_ax_txdma_ch_map txch_map;
	struct mac_ax_rxdma_ch_map rxch_map;
	u32 ret;

	txch_map.ch0 = MAC_AX_PCIE_ENABLE;
	txch_map.ch1 = MAC_AX_PCIE_ENABLE;
	txch_map.ch2 = MAC_AX_PCIE_ENABLE;
	txch_map.ch3 = MAC_AX_PCIE_ENABLE;
	txch_map.ch4 = MAC_AX_PCIE_ENABLE;
	txch_map.ch5 = MAC_AX_PCIE_ENABLE;
	txch_map.ch6 = MAC_AX_PCIE_ENABLE;
	txch_map.ch7 = MAC_AX_PCIE_ENABLE;
	txch_map.ch8 = MAC_AX_PCIE_ENABLE;
	txch_map.ch9 = MAC_AX_PCIE_ENABLE;
	txch_map.ch10 = MAC_AX_PCIE_ENABLE;
	txch_map.ch11 = MAC_AX_PCIE_ENABLE;
	txch_map.ch12 = MAC_AX_PCIE_ENABLE;
	ret = poll_txdma_ch_idle_pcie_8852c(adapter, &txch_map);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]PCIE poll txdma all ch idle %d\n", ret);
		return ret;
	}

	rxch_map.rxq = MAC_AX_PCIE_ENABLE;
	rxch_map.rpq = MAC_AX_PCIE_ENABLE;
	ret = poll_rxdma_ch_idle_pcie_8852c(adapter, &rxch_map);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]PCIE poll rxdma all ch idle %d\n", ret);
		return ret;
	}

	return ret;
}

u32 clr_idx_ch_pcie_8852c(struct mac_ax_adapter *adapter,
			  struct mac_ax_txdma_ch_map *txch_map,
			  struct mac_ax_rxdma_ch_map *rxch_map)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 reg_clr_tx1 = R_AX_TXBD_RWPTR_CLR1;
	u32 reg_clr_tx2 = R_AX_TXBD_RWPTR_CLR2_V1;
	u32 reg_clr_rx = R_AX_RXBD_RWPTR_CLR_V1;
	u32 val32 = 0;

	if (txch_map->ch0 == MAC_AX_PCIE_ENABLE)
		val32 |= B_AX_CLR_ACH0_IDX;

	if (txch_map->ch1 == MAC_AX_PCIE_ENABLE)
		val32 |= B_AX_CLR_ACH1_IDX;

	if (txch_map->ch2 == MAC_AX_PCIE_ENABLE)
		val32 |= B_AX_CLR_ACH2_IDX;

	if (txch_map->ch3 == MAC_AX_PCIE_ENABLE)
		val32 |= B_AX_CLR_ACH3_IDX;

	if (txch_map->ch4 == MAC_AX_PCIE_ENABLE)
		val32 |= B_AX_CLR_ACH4_IDX;

	if (txch_map->ch5 == MAC_AX_PCIE_ENABLE)
		val32 |= B_AX_CLR_ACH5_IDX;

	if (txch_map->ch6 == MAC_AX_PCIE_ENABLE)
		val32 |= B_AX_CLR_ACH6_IDX;

	if (txch_map->ch7 == MAC_AX_PCIE_ENABLE)
		val32 |= B_AX_CLR_ACH7_IDX;

	if (txch_map->ch8 == MAC_AX_PCIE_ENABLE)
		val32 |= B_AX_CLR_CH8_IDX;

	if (txch_map->ch9 == MAC_AX_PCIE_ENABLE)
		val32 |= B_AX_CLR_CH9_IDX;

	if (txch_map->ch12 == MAC_AX_PCIE_ENABLE)
		val32 |= B_AX_CLR_CH12_IDX;

	MAC_REG_W32(reg_clr_tx1, val32);

	val32 = 0;
	if (txch_map->ch10 == MAC_AX_PCIE_ENABLE)
		val32 |= B_AX_CLR_CH10_IDX;

	if (txch_map->ch11 == MAC_AX_PCIE_ENABLE)
		val32 |= B_AX_CLR_CH11_IDX;

	MAC_REG_W32(reg_clr_tx2, val32);

	val32 = 0;
	if (rxch_map->rxq == MAC_AX_PCIE_ENABLE)
		val32 |= B_AX_CLR_RXQ_IDX;
	if (rxch_map->rpq == MAC_AX_PCIE_ENABLE)
		val32 |= B_AX_CLR_RPQ_IDX;

	MAC_REG_W32(reg_clr_rx, val32);

	return MACSUCCESS;
}

u32 rst_bdram_pcie_8852c(struct mac_ax_adapter *adapter, u8 val)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 reg_init = R_AX_HAXI_INIT_CFG1;
	u32 cnt, val32;

	MAC_REG_W32(reg_init, MAC_REG_R32(reg_init) | B_AX_RST_BDRAM);

	cnt = PCIE_POLL_BDRAM_RST_CNT;
	while (cnt) {
		val32 = MAC_REG_R32(reg_init);
		if (!(val32 & B_AX_RST_BDRAM))
			break;
		cnt--;
		PLTFM_DELAY_US(PCIE_POLL_BDRAM_RST_DLY_US);
	}

	if (!cnt) {
		PLTFM_MSG_ERR("[ERR]rst bdram timeout 0x%X\n", val32);
		return MACPOLLTO;
	}

	return MACSUCCESS;
}

u32 trx_mit_pcie_8852c(struct mac_ax_adapter *adapter,
		       struct mac_ax_pcie_trx_mitigation *mit_info)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u8 tmr_unit = 0;
	u32 value32 = 0;

	if (mit_info->txch_map->ch0 == MAC_AX_PCIE_ENABLE)
		value32 |= B_AX_TXMIT_ACH0_SEL;
	else if (mit_info->txch_map->ch0 == MAC_AX_PCIE_DISABLE)
		value32 &= ~B_AX_TXMIT_ACH0_SEL;

	if (mit_info->txch_map->ch1 == MAC_AX_PCIE_ENABLE)
		value32 |= B_AX_TXMIT_ACH1_SEL;
	else if (mit_info->txch_map->ch1 == MAC_AX_PCIE_DISABLE)
		value32 &= ~B_AX_TXMIT_ACH1_SEL;

	if (mit_info->txch_map->ch2 == MAC_AX_PCIE_ENABLE)
		value32 |= B_AX_TXMIT_ACH2_SEL;
	else if (mit_info->txch_map->ch2 == MAC_AX_PCIE_DISABLE)
		value32 &= ~B_AX_TXMIT_ACH2_SEL;

	if (mit_info->txch_map->ch3 == MAC_AX_PCIE_ENABLE)
		value32 |= B_AX_TXMIT_ACH3_SEL;
	else if (mit_info->txch_map->ch3 == MAC_AX_PCIE_DISABLE)
		value32 &= ~B_AX_TXMIT_ACH3_SEL;

	if (mit_info->txch_map->ch4 == MAC_AX_PCIE_ENABLE)
		value32 |= B_AX_TXMIT_ACH4_SEL;
	else if (mit_info->txch_map->ch4 == MAC_AX_PCIE_DISABLE)
		value32 &= ~B_AX_TXMIT_ACH4_SEL;

	if (mit_info->txch_map->ch5 == MAC_AX_PCIE_ENABLE)
		value32 |= B_AX_TXMIT_ACH5_SEL;
	else if (mit_info->txch_map->ch5 == MAC_AX_PCIE_DISABLE)
		value32 &= ~B_AX_TXMIT_ACH5_SEL;

	if (mit_info->txch_map->ch6 == MAC_AX_PCIE_ENABLE)
		value32 |= B_AX_TXMIT_ACH6_SEL;
	else if (mit_info->txch_map->ch6 == MAC_AX_PCIE_DISABLE)
		value32 &= ~B_AX_TXMIT_ACH6_SEL;

	if (mit_info->txch_map->ch7 == MAC_AX_PCIE_ENABLE)
		value32 |= B_AX_TXMIT_ACH7_SEL;
	else if (mit_info->txch_map->ch7 == MAC_AX_PCIE_DISABLE)
		value32 &= ~B_AX_TXMIT_ACH7_SEL;

	if (mit_info->txch_map->ch8 == MAC_AX_PCIE_ENABLE)
		value32 |= B_AX_TXMIT_CH8_SEL;
	else if (mit_info->txch_map->ch8 == MAC_AX_PCIE_DISABLE)
		value32 &= ~B_AX_TXMIT_CH8_SEL;

	if (mit_info->txch_map->ch9 == MAC_AX_PCIE_ENABLE)
		value32 |= B_AX_TXMIT_CH9_SEL;
	else if (mit_info->txch_map->ch9 == MAC_AX_PCIE_DISABLE)
		value32 &= ~B_AX_TXMIT_CH9_SEL;

	if (mit_info->txch_map->ch10 == MAC_AX_PCIE_ENABLE)
		value32 |= B_AX_TXMIT_CH10_SEL;
	else if (mit_info->txch_map->ch10 == MAC_AX_PCIE_DISABLE)
		value32 &= ~B_AX_TXMIT_CH10_SEL;

	if (mit_info->txch_map->ch11 == MAC_AX_PCIE_ENABLE)
		value32 |= B_AX_TXMIT_CH11_SEL;
	else if (mit_info->txch_map->ch11 == MAC_AX_PCIE_DISABLE)
		value32 &= ~B_AX_TXMIT_CH11_SEL;

	if (mit_info->txch_map->ch12 == MAC_AX_PCIE_ENABLE)
		value32 |= B_AX_TXMIT_CH12_SEL;
	else if (mit_info->txch_map->ch12 == MAC_AX_PCIE_DISABLE)
		value32 &= ~B_AX_TXMIT_CH12_SEL;

	switch (mit_info->tx_timer_unit) {
	case MAC_AX_MIT_64US:
		tmr_unit = 0;
		break;
	case MAC_AX_MIT_128US:
		tmr_unit = 1;
		break;
	case MAC_AX_MIT_256US:
		tmr_unit = 2;
		break;
	case MAC_AX_MIT_512US:
		tmr_unit = 3;
		break;
	default:
		PLTFM_MSG_WARN("[WARN]Set TX MIT timer unit fail\n");
		break;
	}

	value32 = SET_CLR_WORD(value32, tmr_unit, B_AX_MIT_TXTIMER_UNIT);
	value32 = SET_CLR_WORD(value32, mit_info->tx_counter, B_AX_MIT_TXCOUNTER_MATCH);
	value32 = SET_CLR_WORD(value32, mit_info->tx_timer, B_AX_MIT_TXTIMER_MATCH);
	MAC_REG_W32(R_AX_INT_MIT_TX_V1, value32);

	value32 = 0;
	if (mit_info->rxch_map->rxq == MAC_AX_PCIE_ENABLE)
		value32 |= B_AX_RXMIT_RXP2_SEL_V1;
	else if (mit_info->rxch_map->rxq == MAC_AX_PCIE_DISABLE)
		value32 &= ~B_AX_RXMIT_RXP2_SEL_V1;

	if (mit_info->rxch_map->rpq == MAC_AX_PCIE_ENABLE)
		value32 |= B_AX_RXMIT_RXP1_SEL_V1;
	else if (mit_info->rxch_map->rpq == MAC_AX_PCIE_DISABLE)
		value32 &= ~B_AX_RXMIT_RXP1_SEL_V1;

	switch (mit_info->rx_timer_unit) {
	case MAC_AX_MIT_64US:
		tmr_unit = 0;
		break;
	case MAC_AX_MIT_128US:
		tmr_unit = 1;
		break;
	case MAC_AX_MIT_256US:
		tmr_unit = 2;
		break;
	case MAC_AX_MIT_512US:
		tmr_unit = 3;
		break;
	default:
		PLTFM_MSG_WARN("[WARN]Set RX MIT timer unit fail\n");
		break;
	}

	value32 = SET_CLR_WORD(value32, tmr_unit, B_AX_MIT_RXTIMER_UNIT);
	value32 = SET_CLR_WORD(value32, mit_info->rx_counter, B_AX_MIT_RXCOUNTER_MATCH);
	value32 = SET_CLR_WORD(value32, mit_info->rx_timer, B_AX_MIT_RXTIMER_MATCH);
	MAC_REG_W32(R_AX_INT_MIT_RX_V1, value32);

	return MACSUCCESS;
}

u32 mode_op_pcie_8852c(struct mac_ax_adapter *adapter,
		       struct mac_ax_intf_info *intf_info)
{
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	enum mac_ax_rxbd_mode *rxbd_mode = (&intf_info->rxbd_mode);
	enum mac_ax_tag_mode *tag_mode = (&intf_info->tag_mode);
	enum mac_ax_multi_tag_num *multi_tag_num = (&intf_info->multi_tag_num);
	enum mac_ax_wd_dma_intvl *wd_dma_idle_intvl =
		(&intf_info->wd_dma_idle_intvl);
	enum mac_ax_wd_dma_intvl *wd_dma_act_intvl =
		(&intf_info->wd_dma_act_intvl);
	enum mac_ax_tx_burst *tx_burst = &intf_info->tx_burst;
	enum mac_ax_rx_burst *rx_burst = &intf_info->rx_burst;
	struct mac_ax_intf_info *intf_info_def;
	u32 reg_init1 = R_AX_HAXI_INIT_CFG1;
	u32 reg_rxapp = R_AX_HAXI_DMA_RXQ_APPLEN;
	u32 reg_exp = R_AX_HAXI_EXP_CTRL;
	u32 val32_init1, val32_rxapp, val32_exp;

	intf_info_def = p_ops->get_pcie_info_def(adapter);
	if (!intf_info_def) {
		PLTFM_MSG_ERR("%s: NULL intf_info\n", __func__);
		return MACNPTR;
	}

	if (intf_info->rxbd_mode == MAC_AX_RXBD_DEF)
		rxbd_mode = (&intf_info_def->rxbd_mode);
	if (intf_info->tx_burst == MAC_AX_TX_BURST_DEF)
		tx_burst = &intf_info_def->tx_burst;
	if (intf_info->rx_burst == MAC_AX_RX_BURST_DEF)
		rx_burst = &intf_info_def->rx_burst;
	if (intf_info->tag_mode == MAC_AX_TAG_DEF)
		tag_mode = (&intf_info_def->tag_mode);
	if (intf_info->multi_tag_num == MAC_AX_TAG_NUM_DEF)
		multi_tag_num = (&intf_info_def->multi_tag_num);
	if (intf_info->wd_dma_act_intvl == MAC_AX_WD_DMA_INTVL_DEF)
		wd_dma_act_intvl = (&intf_info_def->wd_dma_act_intvl);
	if (intf_info->wd_dma_idle_intvl == MAC_AX_WD_DMA_INTVL_DEF)
		wd_dma_idle_intvl = (&intf_info_def->wd_dma_idle_intvl);

	if (*tag_mode == MAC_AX_TAG_SGL) {
		PLTFM_MSG_ERR("Single tag no support\n");
		return MACHWNOSUP;
	}

	val32_init1 = MAC_REG_R32(reg_init1);
	val32_rxapp = MAC_REG_R32(reg_rxapp);
	val32_exp = MAC_REG_R32(reg_exp);

	if ((*rxbd_mode) == MAC_AX_RXBD_PKT) {
		val32_init1 &= ~B_AX_RXBD_MODE_V1;
	} else if ((*rxbd_mode) == MAC_AX_RXBD_SEP) {
		if (intf_info->rx_sep_append_len > B_AX_PCIE_RX_APPLEN_MSK) {
			PLTFM_MSG_ERR("rx sep app len %d\n",
				      intf_info->rx_sep_append_len);
			return MACFUNCINPUT;
		}

		val32_init1 |= B_AX_RXBD_MODE_V1;
		val32_rxapp = SET_CLR_WORD(val32_rxapp, intf_info->rx_sep_append_len,
					   B_AX_PCIE_RX_APPLEN);
	}

	val32_init1 = SET_CLR_WORD(val32_init1, *tx_burst, B_AX_HAXI_MAX_TXDMA);
	val32_init1 = SET_CLR_WORD(val32_init1, *rx_burst, B_AX_HAXI_MAX_RXDMA);

	val32_exp = SET_CLR_WORD(val32_exp, *multi_tag_num, B_AX_MAX_TAG_NUM_V1);

	val32_init1 = SET_CLR_WORD(val32_init1, *wd_dma_idle_intvl, B_AX_WD_ITVL_IDLE_V1);
	val32_init1 = SET_CLR_WORD(val32_init1, *wd_dma_act_intvl, B_AX_WD_ITVL_ACT_V1);

	MAC_REG_W32(reg_init1, val32_init1);
	MAC_REG_W32(reg_rxapp, val32_rxapp);
	MAC_REG_W32(reg_exp, val32_exp);

	return MACSUCCESS;
}

u32 get_err_flag_pcie_8852c(struct mac_ax_adapter *adapter,
			    struct mac_ax_pcie_err_info *out_info)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32 = MAC_REG_R32(R_AX_DBG_ERR_FLAG_V1);

	out_info->txbd_len_zero = (val32 & B_AX_HAXI_TXBD_LEN0) ? 1 : 0;
	out_info->tx_stuck = (val32 & B_AX_TX_STUCK_V1) ? 1 : 0;
	out_info->rx_stuck = (val32 & B_AX_RX_STUCK_V1) ? 1 : 0;

	return MACSUCCESS;
}

static u32 get_target_8852c(struct mac_ax_adapter *adapter, u16 *target,
			    u32 phy_offset)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u16 tmp_u16, count;
	u16 tar;

	/* Enable counter */
	tmp_u16 = MAC_REG_R16(phy_offset + RAC_CTRL_PPR_V1 * RAC_MULT);
	MAC_REG_W16(phy_offset + RAC_CTRL_PPR_V1 * RAC_MULT,
		    tmp_u16 & ~BAC_AUTOK_ONCE_EN);
	MAC_REG_W16(phy_offset + RAC_CTRL_PPR_V1 * RAC_MULT,
		    tmp_u16 | BAC_AUTOK_ONCE_EN);

	tar = MAC_REG_R16(phy_offset + RAC_CTRL_PPR_V1 * RAC_MULT);
	count = PCIE_POLL_AUTOK_CNT;
	while (count && (tar & BAC_AUTOK_ONCE_EN)) {
		tar = MAC_REG_R16(phy_offset + RAC_CTRL_PPR_V1 * RAC_MULT);
		count--;
		PLTFM_DELAY_US(PCIE_POLL_AUTOK_DLY_US);
	}

	if (!count) {
		PLTFM_MSG_ERR("[ERR]AutoK get target timeout: %X\n", tar);
		return MACPOLLTO;
	}

	tar = tar & BAC_AUTOK_HW_TAR_MSK;
	if (tar == 0 || tar == BAC_AUTOK_HW_TAR_MSK) {
		PLTFM_MSG_ERR("[ERR]Get target failed.\n");
		return MACHWERR;
	}

	*target = tar;
	return MACSUCCESS;
}

u32 mac_auto_refclk_cal_pcie_8852c(struct mac_ax_adapter *adapter,
				   enum mac_ax_pcie_func_ctrl en)
{
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u16 tmp_u16;
	u16 mgn_set = PCIE_AUTOK_MGN;
	u16 tar;
	u8 l1_flag = 0;
	u32 bdr_ori, val32, ret = MACSUCCESS;
	u32 phy_offset = 0;

#if MAC_AX_FEATURE_HV
	if (adapter->env == HV_AX_FPGA || adapter->env == HV_AX_PXP)
		return MACSUCCESS;
#endif

#if (INTF_INTGRA_HOSTREF_V1 <= INTF_INTGRA_MINREF_V1)
	PLTFM_MSG_ERR("[ERR]INTF_INTGRA_MINREF_V1 define fail\n");
	return MACCMP;
#endif
	ret = p_ops->mio_r32_pcie(adapter, PCIE_L1_STS, &val32);
	if (ret != MACSUCCESS)
		return ret;

	val32 = GET_FIEL2(val32, BCFG_LINK_SPEED_SH, BCFG_LINK_SPEED_MSK);
	if (val32 == PCIE_GEN1_SPEED) {
		phy_offset = RAC_DIRECT_OFFSET_G1;
	} else if (val32 == PCIE_GEN2_SPEED) {
		phy_offset = RAC_DIRECT_OFFSET_G2;
	} else {
		PLTFM_MSG_ERR("[ERR]PCIe PHY rate not support\n");
		return MACHWNOSUP;
	}

	/* Disable L1BD */
	bdr_ori = MAC_REG_R32(R_AX_PCIE_MIX_CFG_V1);

	if (bdr_ori & PCIE_BIT_L1_V1) {
		MAC_REG_W32(R_AX_PCIE_MIX_CFG_V1, bdr_ori & ~PCIE_BIT_L1_V1);
		l1_flag = 1;
	}

	/* Disable function */
	tmp_u16 = MAC_REG_R16(phy_offset + RAC_CTRL_PPR_V1 * RAC_MULT);

	if (tmp_u16 & BAC_AUTOK_EN)
		MAC_REG_W16(phy_offset + RAC_CTRL_PPR_V1 * RAC_MULT,
			    tmp_u16 & ~(BAC_AUTOK_EN));

	if (en != MAC_AX_PCIE_ENABLE)
		goto end;

	/* Set div */
	tmp_u16 = MAC_REG_R16(phy_offset + RAC_CTRL_PPR_V1 * RAC_MULT);
	tmp_u16 = SET_CLR_WOR2(tmp_u16, PCIE_AUTOK_DIV_2048, BAC_AUTOK_DIV_SH,
			       BAC_AUTOK_DIV_MSK);
	MAC_REG_W16(phy_offset + RAC_CTRL_PPR_V1 * RAC_MULT, tmp_u16);

	/*  Obtain div and margin */
	ret = get_target_8852c(adapter, &tar, phy_offset);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]1st get target fail %d\n", ret);
		goto end;
	}

	MAC_REG_W16(phy_offset + RAC_SET_PPR_V1 * RAC_MULT,
		    (tar & BAC_AUTOK_TAR_MSK) | (mgn_set << BAC_AUTOK_MGN_SH));

	/* Enable function */
	tmp_u16 = MAC_REG_R16(phy_offset + RAC_CTRL_PPR_V1 * RAC_MULT);
	tmp_u16 |= BAC_AUTOK_EN;
	MAC_REG_W16(phy_offset + RAC_CTRL_PPR_V1 * RAC_MULT,
		    tmp_u16 | BAC_AUTOK_EN);
end:
	/* Set L1BD to ori */
	if (l1_flag == 1)
		MAC_REG_W32(R_AX_PCIE_MIX_CFG_V1, bdr_ori);

	PLTFM_MSG_TRACE("[TRACE]%s: <==\n", __func__);

	return ret;
}

#endif /* #if MAC_AX_PCIE_SUPPORT */

#endif /* #if MAC_AX_8852C_SUPPORT */

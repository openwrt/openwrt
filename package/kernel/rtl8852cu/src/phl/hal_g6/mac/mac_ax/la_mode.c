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
#include "la_mode.h"

u32 mac_lamode_cfg(struct mac_ax_adapter *adapter,
		   struct mac_ax_la_cfg *cfg)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32;

	val32 = MAC_REG_R32(R_AX_DMAC_FUNC_EN);
	val32 |= B_AX_BBRPT_EN;
	MAC_REG_W32(R_AX_DMAC_FUNC_EN, val32);

	val32 = MAC_REG_R32(R_AX_DMAC_CLK_EN);
	val32 |= B_AX_BBRPT_CLK_EN;
	MAC_REG_W32(R_AX_DMAC_CLK_EN, val32);

	val32 = MAC_REG_R32(R_AX_LA_CFG);
	val32 &= ~BITS_AX_LA_CFG;
	val32 |= ((cfg->la_func_en ? B_AX_LA_FEN : 0) |
		  (cfg->la_restart_en ? B_AX_LA_RESTART_EN : 0) |
		  (cfg->la_timeout_en ? B_AX_LA_TO_EN : 0) |
		  SET_WORD(cfg->la_timeout_val, B_AX_LA_TO_VAL) |
		  SET_WORD(cfg->la_tgr_tu_sel, B_AX_LA_TRIG_TU_SEL) |
		  SET_WORD(cfg->la_tgr_time_val, B_AX_LA_TRIG_TIME_VAL));
	MAC_REG_W32(R_AX_LA_CFG, val32);

	if (cfg->la_data_loss_imr)
		MAC_REG_W8(R_AX_LA_ERRFLAG, BIT0);

	return MACSUCCESS;
}

u32 mac_lamode_buf_cfg(struct mac_ax_adapter *adapter,
		       struct mac_ax_la_buf_param *param)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	struct mac_ax_hw_info *hw_info = adapter->hw_info;
	u32 val32;

	val32 = MAC_REG_R32(R_AX_LA_CFG);
	val32 = SET_CLR_WORD(val32, param->la_buf_sel, B_AX_LA_BUF_SEL);
	if (param->la_buf_sel == LA_BUF_SEL_256K) { /* la buf 256K */
		val32 = SET_CLR_WORD(val32, LA_SIZE_256K_BUF_BNDY, B_AX_LA_BUF_BNDY);
		param->start_addr = LA_SIZE_256K_BUF_BNDY * DLE_BLOCK_SIZE;
		param->end_addr = DLE_BUF_BNDY_8852A;
	} else if (param->la_buf_sel == LA_BUF_SEL_192K) { /* la buf 192K */
		val32 = SET_CLR_WORD(val32, LA_SIZE_192K_BUF_BNDY, B_AX_LA_BUF_BNDY);
		param->start_addr = LA_SIZE_192K_BUF_BNDY * DLE_BLOCK_SIZE;
		param->end_addr = DLE_BUF_BNDY_8852A;
	} else if (param->la_buf_sel == LA_BUF_SEL_128K) { /* la buf 128K */
		switch (hw_info->chip_id) {
#if MAC_AX_8852A_SUPPORT /*temp setting, will move to per IC files later*/
		case MAC_AX_CHIP_ID_8852A:
			PLTFM_MSG_ERR("[ERR]8852A Non support buf sel %d\n",
				      param->la_buf_sel);
			break;
#endif
#ifdef MAC_AX_8852B_SUPPORT /*temp setting, will move to per IC files later*/
		case MAC_AX_CHIP_ID_8852B:
			val32 = SET_CLR_WORD(val32, LA_SIZE_128K_BUF_BNDY_8852B,
					     B_AX_LA_BUF_BNDY);
			param->start_addr = LA_SIZE_128K_BUF_BNDY_8852B *
					    DLE_BLOCK_SIZE;
			param->end_addr = DLE_BUF_BNDY_8852B;
			break;
#endif
#ifdef MAC_AX_8852C_SUPPORT
		case MAC_AX_CHIP_ID_8852C:
			val32 = SET_CLR_WORD(val32, LA_SIZE_256K_BUF_BNDY,
					     B_AX_LA_BUF_BNDY);
			param->start_addr = LA_SIZE_256K_BUF_BNDY * DLE_BLOCK_SIZE;
			param->end_addr = LA_BUF_END_8852C_128K;
			break;
#endif
#ifdef MAC_AX_8192XB_SUPPORT
		case MAC_AX_CHIP_ID_8192XB:
			val32 = SET_CLR_WORD(val32, LA_SIZE_256K_BUF_BNDY,
					     B_AX_LA_BUF_BNDY);
			param->start_addr = LA_SIZE_256K_BUF_BNDY * DLE_BLOCK_SIZE;
			param->end_addr = LA_BUF_END_8192XB_128K;
			break;
#endif
#ifdef MAC_AX_8851B_SUPPORT /*temp setting, will move to per IC files later*/
		case MAC_AX_CHIP_ID_8851B:
			val32 = SET_CLR_WORD(val32, LA_SIZE_128K_BUF_BNDY_8851B,
					     B_AX_LA_BUF_BNDY);
			param->start_addr = LA_SIZE_128K_BUF_BNDY_8851B *
					    DLE_BLOCK_SIZE;
			param->end_addr = DLE_BUF_BNDY_8851B;
			break;
#endif
#ifdef MAC_AX_8851E_SUPPORT
		case MAC_AX_CHIP_ID_8851E:
			val32 = SET_CLR_WORD(val32, LA_SIZE_256K_BUF_BNDY,
					     B_AX_LA_BUF_BNDY);
			param->start_addr = LA_SIZE_256K_BUF_BNDY * DLE_BLOCK_SIZE;
			param->end_addr = LA_BUF_END_8851E_128K;
			break;
#endif
#ifdef MAC_AX_8852D_SUPPORT
		case MAC_AX_CHIP_ID_8852D:
			val32 = SET_CLR_WORD(val32, LA_SIZE_256K_BUF_BNDY,
					     B_AX_LA_BUF_BNDY);
			param->start_addr = LA_SIZE_256K_BUF_BNDY * DLE_BLOCK_SIZE;
			param->end_addr = LA_BUF_END_8852D_128K;
			break;
#endif
		default:
			PLTFM_MSG_ERR("[ERR]Non support 128K buffer sel\n");
			break;
		}
	} else if (param->la_buf_sel == LA_BUF_SEL_64K) { /* la buf 64K */
		val32 |= SET_WORD(LA_SIZE_128K_BUF_BNDY_8852B,
				  B_AX_LA_BUF_BNDY);
		param->start_addr = LA_SIZE_128K_BUF_BNDY_8852B *
				    DLE_BLOCK_SIZE;
		param->end_addr = DLE_BUF_BNDY_8852B;
	} else {
		PLTFM_MSG_ERR("[ERR]Non support buf sel %d\n",
			      param->la_buf_sel);
	}

	MAC_REG_W32(R_AX_LA_CFG, val32);

	return MACSUCCESS;
}

u32 mac_lamode_trigger(struct mac_ax_adapter *adapter, u8 tgr)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 count = 3000;
	u8 val8;

	if (tgr) {
		val8 = MAC_REG_R8(R_AX_LA_CFG);
		MAC_REG_W8(R_AX_LA_CFG, val8 | B_AX_LA_TRIG_START);
	}

	val8 = MAC_REG_R8(R_AX_LA_CFG);
	while (--count) {
		if (!(val8 & B_AX_LA_TRIG_START))
			break;
		PLTFM_DELAY_MS(1);
	}
	if (!count)
		return MACPOLLTO;

	return MACSUCCESS;
}

struct mac_ax_la_status mac_get_lamode_st(struct mac_ax_adapter *adapter)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32;
	struct mac_ax_la_status info;

	info.la_buf_wptr = 0;
	info.la_buf_rndup_ind = 0;
	info.la_sw_fsmst = 0;
	info.la_data_loss = 0;

	val32 = MAC_REG_R32(R_AX_LA_STATUS);
	info.la_sw_fsmst = (val32 >> B_AX_LA_SW_FSMST_SH) &
			    B_AX_LA_SW_FSMST_MSK;
	info.la_buf_wptr = (val32 >> B_AX_LA_BUF_WPTR_SH) &
			    B_AX_LA_BUF_WPTR_MSK;
	info.la_buf_rndup_ind = (val32 & B_AX_LA_BUF_RNDUP) ? 1 : 0;

	val32 = MAC_REG_R32(R_AX_LA_ERRFLAG);
	info.la_data_loss = (val32 & B_AX_LA_ISR_DATA_LOSS_ERR) ? 1 : 0;

	return info;
}


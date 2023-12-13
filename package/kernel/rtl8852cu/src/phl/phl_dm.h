/******************************************************************************
 *
 * Copyright(c) 2021 Realtek Corporation.
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
 *****************************************************************************/
#ifndef _PHL_DM_H_
#define _PHL_DM_H_

#ifdef CONFIG_PCI_HCI
#ifdef RTW_WKARD_DYNAMIC_LTR
enum rtw_phl_status
phl_ltr_sw_trigger(struct rtw_phl_com_t *phl_com, void *hal,
	enum rtw_pcie_ltr_state state);
enum rtw_phl_status
phl_ltr_sw_ctrl(struct rtw_phl_com_t *phl_com, void *hal, bool enable);
void
phl_ltr_hw_ctrl(struct rtw_phl_com_t *phl_com, bool enable);
void phl_ltr_sw_ctrl_ntfy(struct rtw_phl_com_t *phl_com, bool enable);
u8 phl_ltr_get_cur_state(struct rtw_phl_com_t *phl_com);
u32 phl_ltr_get_last_trigger_time(struct rtw_phl_com_t *phl_com);
u32 phl_ltr_get_tri_cnt(struct rtw_phl_com_t *phl_com,
	enum rtw_pcie_ltr_state state);
void phl_ltr_ctrl_watchdog(struct phl_info_t *phl_info);
#endif
#endif

#endif /*_PHL_DM_H_*/
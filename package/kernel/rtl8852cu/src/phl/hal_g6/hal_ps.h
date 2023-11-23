/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation.
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
#ifndef _HAL_PS_H_
#define _HAL_PS_H_
#ifdef CONFIG_POWER_SAVE
/* Can be used to verify sw flow without configure hw */
#define HAL_LPS_SKIP_HW_CFG 0
#define HAL_IPS_SKIP_HW_CFG 0

enum rtw_hal_status rtw_hal_ps_pwr_req(struct rtw_phl_com_t *phl_com, u8 src, bool pwr_req);
#endif
#endif /*_HAL_PS_H_*/

/******************************************************************************
 *
 * Copyright(c) 2020 Realtek Corporation.
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
#ifndef _PHL_THERMAL_H_
#define _PHL_THERMAL_H_

#define THERMAL_TX_DUTY_CTRL_DURATION 100
#define THERMAL_NO_TX_DUTY_CTRL 100
#define THERMAL_NO_SW_THRESHOLD 0xff

void phl_thermal_protect_watchdog(struct phl_info_t *phl_info);

void phl_thermal_protect_cfg_tx_duty(
	struct phl_info_t *phl_info,
	u16 tx_duty_interval,
	u8 ratio);

void phl_thermal_protect_stop_tx_duty(struct phl_info_t *phl_info);
#endif /*_PHL_THERMAL_H_*/

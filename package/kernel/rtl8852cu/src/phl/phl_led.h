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
#ifndef _PHL_LED_H_
#define _PHL_LED_H_

enum rtw_phl_status phl_register_led_module(struct phl_info_t *phl_info);

void phl_led_control(struct phl_info_t *phl_info, enum rtw_led_event led_event);

#endif /*_PHL_LED_H_*/

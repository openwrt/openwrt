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
#ifndef _HAL_NOTIFY_H_
#define _HAL_NOTIFY_H_


void rtw_hal_notification_ex(void *hal, enum phl_msg_evt_id event,
			     bool to_bb, bool to_mac, bool to_rf, u8 hw_idx);

void rtw_hal_notification(void *hal, enum phl_msg_evt_id event, u8 hw_idx);


void rtw_hal_cmd_notification(void *hal,
                   enum phl_msg_evt_id event,
                   void *hal_cmd,
                   u8 hw_idx);


enum rtw_phl_status
rtw_hal_cmd_notify(struct rtw_phl_com_t *phl_com,
                   enum phl_msg_evt_id event,
                   void *hal_cmd,
                   u8 hw_idx);

#endif /*_HAL_NOTIFY_H_*/


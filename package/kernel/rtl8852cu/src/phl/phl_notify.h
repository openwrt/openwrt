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
#ifndef _PHL_NOTIFY_H_
#define _PHL_NOTIFY_H_

#ifdef CONFIG_CMD_DISP
enum rtw_phl_status
rtw_phl_cmd_notify(struct rtw_phl_com_t *phl_com,
                   enum phl_msg_evt_id event,
                   void *hal_cmd,
                   u8 hw_idx);

enum rtw_phl_status
phl_notify_cmd_hdl(struct phl_info_t *phl_info, u8 *param);
#endif /* CONFIG_CMD_DISP */

#endif /*_PHL_NOTIFY_H_*/


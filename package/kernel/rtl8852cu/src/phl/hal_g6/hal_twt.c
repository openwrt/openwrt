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
#define _HAL_TWT_C_
#include "hal_headers.h"

#ifdef CONFIG_PHL_TWT
enum rtw_hal_status
rtw_hal_twt_info_update(void *hal,
                        struct rtw_phl_twt_info twt_info,
                        struct rtw_wifi_role_link_t *rlink,
                        u8 action)
{
	return rtw_hal_mac_twt_info_update(hal, twt_info, rlink, action);
}

enum rtw_hal_status
rtw_hal_twt_sta_update(void *hal, u8 macid, u8 twt_id, u8 action)
{
	return rtw_hal_mac_twt_sta_update(hal, macid, twt_id, action);
}

enum rtw_hal_status
rtw_hal_twt_sta_announce(void *hal, u8 macid)
{
	return rtw_hal_mac_twt_sta_announce(hal, macid);
}
#endif /* CONFIG_PHL_TWT */


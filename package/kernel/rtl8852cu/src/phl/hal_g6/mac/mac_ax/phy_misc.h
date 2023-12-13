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

#ifndef _MAC_AX_PHY_MISC_H_
#define _MAC_AX_PHY_MISC_H_

#include "../type.h"
#include "fwcmd.h"

u32 mac_fast_ch_sw(struct mac_ax_adapter *adapter,
		   struct mac_ax_fast_ch_sw_param *fast_ch_sw_param);
u32 mac_fast_ch_sw_done(struct mac_ax_adapter *adapter);
u32 mac_get_fast_ch_sw_rpt(struct mac_ax_adapter *adapter, u32 *fast_ch_sw_status_code);
#endif // __MAC_AX_PHY_MISC_H_
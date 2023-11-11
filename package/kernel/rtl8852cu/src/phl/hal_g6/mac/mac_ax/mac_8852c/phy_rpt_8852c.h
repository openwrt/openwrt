/** @file */
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

#ifndef _MAC_AX_PHY_RPT_8852C_H_
#define _MAC_AX_PHY_RPT_8852C_H_

#include "../../mac_def.h"
#if MAC_AX_8852C_SUPPORT

u32 get_bbrpt_dle_cfg_8852c(struct mac_ax_adapter *adapter,
			    u8 is2wlcpu, u32 *port_id, u32 *queue_id);

u32 mac_cfg_per_pkt_phy_rpt_8852c(struct mac_ax_adapter *adapter,
				  struct mac_ax_per_pkt_phy_rpt *rpt);

#endif /* #if MAC_AX_8852C_SUPPORT */
#endif

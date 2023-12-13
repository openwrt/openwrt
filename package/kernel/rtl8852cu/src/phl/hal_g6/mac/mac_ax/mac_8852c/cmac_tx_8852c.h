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

#ifndef _MAC_AX_CMAC_TX_8852C_H_
#define _MAC_AX_CMAC_TX_8852C_H_

#include "../../mac_def.h"
#include "../mac_priv.h"
#if MAC_AX_8852C_SUPPORT

u32 macid_idle_ck_8852c(struct mac_ax_adapter *adapter,
			struct mac_role_tbl *role);
u32 stop_sch_tx_8852c(struct mac_ax_adapter *adapter, enum sch_tx_sel sel,
		      struct mac_ax_sch_tx_en_cfg *bak);
#endif /* #if MAC_AX_8852C_SUPPORT */
#endif
